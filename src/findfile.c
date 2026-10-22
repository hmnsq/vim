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

#ಅನ್ತರ್ಭಾವ "vim.h"

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ff_ರಾಶಿ
{
    ರಚನೆ ff_ರಾಶಿ	*ffs_prev;

    // the fix part (no wildcards) and the part containing the wildcards
    // of the search path
    string_T		ffs_fix_ಮಾರ್ಗ;
    string_T		ffs_wc_ಮಾರ್ಗ;

    // files/dirs found in the above directory, matched by the first wildcard
    // of wc_part
    char_u		**ffs_filearray;
    ಪೂರ್ಣಾಂಕ			ffs_filearray_ಗಾತ್ರ;
    ಪೂರ್ಣಾಂಕ			ffs_filearray_cur;   // needed for partly handled dirs

    // to store status of partly handled directories
    // 0: we work on this directory for the first time
    // 1: this directory was partly searched in an earlier step
    ಪೂರ್ಣಾಂಕ			ffs_stage;

    // How deep are we in the directory tree?
    // Counts backward from value of level parameter to vim_findfile_init
    ಪೂರ್ಣಾಂಕ			ffs_level;

    // Did we already expand '**' to an empty string?
    ಪೂರ್ಣಾಂಕ			ffs_ನಕ್ಷತ್ರ_ನಕ್ಷತ್ರ_ಖಾಲಿಗಳು;
} ff_ರಾಶಿ_T;

/*
 * type for already visited directories or files.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ff_ಭೇಟಿಯಾದ
{
    ರಚನೆ ff_ಭೇಟಿಯಾದ	*ffv_ಮುಂದಿನ;

    // Visited directories are different if the wildcard string are
    // different. So we have to save it.
    char_u		*ffv_wc_ಮಾರ್ಗ;

    // for unix use inode etc for comparison (needed because of links), else
    // use filename.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಪೂರ್ಣಾಂಕ			ffv_dev_valid;	// ffv_dev and ffv_ino were set
    dev_t		ffv_dev;	// device number
    ino_t		ffv_ino;	// inode number
#ಆದರೆ_ಅಂತ್ಯ
    // The memory for this struct is allocated according to the length of
    // ffv_fname.
    char_u		ffv_fname[1];	// actually longer
} ff_ಭೇಟಿಯಾದ_T;

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ff_ಭೇಟಿಯಾದ_list_hdr
{
    ರಚನೆ ff_ಭೇಟಿಯಾದ_list_hdr	*ffvl_ಮುಂದಿನ;

    // the filename the attached visited list is for
    char_u			*ffvl_filename;

    ff_ಭೇಟಿಯಾದ_T		*ffvl_ಭೇಟಿಯಾದ_list;

} ff_ಭೇಟಿಯಾದ_list_hdr_T;


/*
 * '**' can be expanded to several directory levels.
 * Set the default maximum depth.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು FF_MAX_ನಕ್ಷತ್ರ_ನಕ್ಷತ್ರ_EXPAND ((char_u)30)

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ff_ಹುಡುಕು_ctx_T
{
    ff_ರಾಶಿ_T			*ffsc_ರಾಶಿ_ptr;
    ff_ಭೇಟಿಯಾದ_list_hdr_T	*ffsc_ಭೇಟಿಯಾದ_list;
    ff_ಭೇಟಿಯಾದ_list_hdr_T	*ffsc_dir_ಭೇಟಿಯಾದ_list;
    ff_ಭೇಟಿಯಾದ_list_hdr_T	*ffsc_ಭೇಟಿಯಾದ_lists_list;
    ff_ಭೇಟಿಯಾದ_list_hdr_T	*ffsc_dir_ಭೇಟಿಯಾದ_lists_list;
    string_T			ffsc_file_ಗಮ್ಯ_ಹುಡುಕು;
    string_T			ffsc_ಆರಂಭ_dir;
    string_T			ffsc_fix_ಮಾರ್ಗ;
    string_T			ffsc_wc_ಮಾರ್ಗ;
    ಪೂರ್ಣಾಂಕ				ffsc_level;
    string_T			*ffsc_stopdirs_v;
    ಪೂರ್ಣಾಂಕ				ffsc_ಕಂಡುಹಿಡಿ_what;
    ಪೂರ್ಣಾಂಕ				ffsc_tagfile;
} ff_ಹುಡುಕು_ctx_T;

// locally needed functions
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ff_check_ಭೇಟಿಯಾದ(ff_ಭೇಟಿಯಾದ_T **, char_u *, size_t, char_u *, size_t);
ಸ್ಥಿರ ಶೂನ್ಯ vim_findfile_free_ಭೇಟಿಯಾದ(ಶೂನ್ಯ *ಹುಡುಕು_ctx_arg);
ಸ್ಥಿರ ಶೂನ್ಯ vim_findfile_free_ಭೇಟಿಯಾದ_list(ff_ಭೇಟಿಯಾದ_list_hdr_T **list_headp);
ಸ್ಥಿರ ಶೂನ್ಯ ff_free_ಭೇಟಿಯಾದ_list(ff_ಭೇಟಿಯಾದ_T *vl);
ಸ್ಥಿರ ff_ಭೇಟಿಯಾದ_list_hdr_T* ff_get_ಭೇಟಿಯಾದ_list(char_u *, size_t, ff_ಭೇಟಿಯಾದ_list_hdr_T **list_headp);

ಸ್ಥಿರ ಶೂನ್ಯ ff_ತಳ್ಳು(ff_ಹುಡುಕು_ctx_T *ಹುಡುಕು_ctx, ff_ರಾಶಿ_T *ರಾಶಿ_ptr);
ಸ್ಥಿರ ff_ರಾಶಿ_T *ff_ತೆಗೆ(ff_ಹುಡುಕು_ctx_T *ಹುಡುಕು_ctx);
ಸ್ಥಿರ ಶೂನ್ಯ ff_clear(ff_ಹುಡುಕು_ctx_T *ಹುಡುಕು_ctx);
ಸ್ಥಿರ ಶೂನ್ಯ ff_free_ರಾಶಿ_element(ff_ರಾಶಿ_T *ರಾಶಿ_ptr);
ಸ್ಥಿರ ff_ರಾಶಿ_T *ff_create_ರಾಶಿ_element(char_u *, size_t, char_u *, size_t, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ff_ಮಾರ್ಗ_ಒಳ_stoplist(char_u *, ಪೂರ್ಣಾಂಕ, string_T *);

ಸ್ಥಿರ string_T ff_expand_ಬಫರ್ = {ಖಾಲಿ, 0};	    // used for expanding filenames

#ಆದರೆ 0
/*
 * if someone likes findfirst/findnext, here are the functions
 * NOT TESTED!!
 */

ಸ್ಥಿರ ಶೂನ್ಯ *ff_fn_ಹುಡುಕು_context = ಖಾಲಿ;

    char_u *
vim_findfirst(char_u *ಮಾರ್ಗ, char_u *filename, ಪೂರ್ಣಾಂಕ level)
{
    ff_fn_ಹುಡುಕು_context =
	vim_findfile_init(ಮಾರ್ಗ, filename, ಖಾಲಿ, level, TRUE, FALSE,
		ff_fn_ಹುಡುಕು_context, rel_fname);
    ಆದರೆ (ಖಾಲಿ == ff_fn_ಹುಡುಕು_context)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ vim_findnext()
}

    char_u *
vim_findnext(ಶೂನ್ಯ)
{
    char_u *ret = vim_findfile(ff_fn_ಹುಡುಕು_context);

    ಆದರೆ (ಖಾಲಿ == ret)
    {
	vim_findfile_cleanup(ff_fn_ಹುಡುಕು_context);
	ff_fn_ಹುಡುಕು_context = ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ ret;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಶೂನ್ಯ *
vim_findfile_init(
    char_u	*ಮಾರ್ಗ,
    char_u	*filename,
    size_t	filenamelen,
    char_u	*stopdirs ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		level,
    ಪೂರ್ಣಾಂಕ		free_ಭೇಟಿಯಾದ,
    ಪೂರ್ಣಾಂಕ		ಕಂಡುಹಿಡಿ_what,
    ಶೂನ್ಯ	*ಹುಡುಕು_ctx_arg,
    ಪೂರ್ಣಾಂಕ		tagfile,	// expanding names of tags files
    char_u	*rel_fname)	// file name to use for "."
{
    char_u		*wc_part;
    ff_ರಾಶಿ_T		*sptr;
    ff_ಹುಡುಕು_ctx_T	*ಹುಡುಕು_ctx;
    ಪೂರ್ಣಾಂಕ			ಕೂಡಿಸು_sep;

    // If a search context is given by the caller, reuse it, else allocate a
    // new one.
    ಆದರೆ (ಹುಡುಕು_ctx_arg != ಖಾಲಿ)
	ಹುಡುಕು_ctx = ಹುಡುಕು_ctx_arg;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಹುಡುಕು_ctx = ALLOC_CLEAR_ONE(ff_ಹುಡುಕು_ctx_T);
	ಆದರೆ (ಹುಡುಕು_ctx == ಖಾಲಿ)
	    ಹೋಗು error_return;
    }
    ಹುಡುಕು_ctx->ffsc_ಕಂಡುಹಿಡಿ_what = ಕಂಡುಹಿಡಿ_what;
    ಹುಡುಕು_ctx->ffsc_tagfile = tagfile;

    // clear the search context, but NOT the visited lists
    ff_clear(ಹುಡುಕು_ctx);

    // clear visited list if wanted
    ಆದರೆ (free_ಭೇಟಿಯಾದ == TRUE)
	vim_findfile_free_ಭೇಟಿಯಾದ(ಹುಡುಕು_ctx);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Reuse old visited lists. Get the visited list for the given
	// filename. If no list for the current filename exists, creates a new
	// one.
	ಹುಡುಕು_ctx->ffsc_ಭೇಟಿಯಾದ_list = ff_get_ಭೇಟಿಯಾದ_list(filename,
					filenamelen, &ಹುಡುಕು_ctx->ffsc_ಭೇಟಿಯಾದ_lists_list);
	ಆದರೆ (ಹುಡುಕು_ctx->ffsc_ಭೇಟಿಯಾದ_list == ಖಾಲಿ)
	    ಹೋಗು error_return;
	ಹುಡುಕು_ctx->ffsc_dir_ಭೇಟಿಯಾದ_list = ff_get_ಭೇಟಿಯಾದ_list(filename,
				    filenamelen, &ಹುಡುಕು_ctx->ffsc_dir_ಭೇಟಿಯಾದ_lists_list);
	ಆದರೆ (ಹುಡುಕು_ctx->ffsc_dir_ಭೇಟಿಯಾದ_list == ಖಾಲಿ)
	    ಹೋಗು error_return;
    }

    ಆದರೆ (ff_expand_ಬಫರ್.string == ಖಾಲಿ)
    {
	ff_expand_ಬಫರ್.ಉದ್ದಳತೆ = 0;
	ff_expand_ಬಫರ್.string = alloc(MAXPATHL);
	ಆದರೆ (ff_expand_ಬಫರ್.string == ಖಾಲಿ)
	    ಹೋಗು error_return;
    }

    // Store information on starting dir now if path is relative.
    // If path is absolute, we do that later.
    ಆದರೆ (ಮಾರ್ಗ[0] == '.'
	    && (vim_ispathsep(ಮಾರ್ಗ[1]) || ಮಾರ್ಗ[1] == NUL)
	    && (!tagfile || vim_strchr(p_cpo, CPO_DOTTAG) == ಖಾಲಿ)
	    && rel_fname != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	len = (ಪೂರ್ಣಾಂಕ)(gettail(rel_fname) - rel_fname);

	ಆದರೆ (!vim_isAbsName(rel_fname) && len + 1 < MAXPATHL)
	{
	    // Make the start dir an absolute path name.
	    vim_strncpy(ff_expand_ಬಫರ್.string, rel_fname, len);
	    ff_expand_ಬಫರ್.ಉದ್ದಳತೆ = len;

	    ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string = ತುಂಬಿದ_Name_save(ff_expand_ಬಫರ್.string, FALSE);
	    ಆದರೆ (ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string == ಖಾಲಿ)
		ಹೋಗು error_return;
	    ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.ಉದ್ದಳತೆ = STRLEN(ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.ಉದ್ದಳತೆ = len;
	    ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string = vim_strnsave(rel_fname,
		ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.ಉದ್ದಳತೆ);
	    ಆದರೆ (ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string == ಖಾಲಿ)
		ಹೋಗು error_return;
	}

	ಆದರೆ (*++ಮಾರ್ಗ != NUL)
	    ++ಮಾರ್ಗ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಮಾರ್ಗ == NUL || !vim_isAbsName(ಮಾರ್ಗ))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	// "c:dir" needs "c:" to be expanded, otherwise use current dir
	ಆದರೆ (*ಮಾರ್ಗ != NUL && ಮಾರ್ಗ[1] == ':')
	{
	    char_u  drive[3];

	    drive[0] = ಮಾರ್ಗ[0];
	    drive[1] = ':';
	    drive[2] = NUL;
	    ಆದರೆ (vim_ತುಂಬಿದ_Name(drive, ff_expand_ಬಫರ್.string, MAXPATHL, TRUE) == FAIL)
		ಹೋಗು error_return;
	    ಮಾರ್ಗ += 2;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (mch_dirname(ff_expand_ಬಫರ್.string, MAXPATHL) == FAIL)
	    ಹೋಗು error_return;

	ff_expand_ಬಫರ್.ಉದ್ದಳತೆ = STRLEN(ff_expand_ಬಫರ್.string);

	ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.ಉದ್ದಳತೆ = ff_expand_ಬಫರ್.ಉದ್ದಳತೆ;
	ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string = vim_strnsave(ff_expand_ಬಫರ್.string,
	    ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.ಉದ್ದಳತೆ);
	ಆದರೆ (ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string == ಖಾಲಿ)
	    ಹೋಗು error_return;
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
    ಆದರೆ (stopdirs != ಖಾಲಿ)
    {
	char_u	*walker = stopdirs;
	ಪೂರ್ಣಾಂಕ	dircount;

	ಆಗ (*walker == ';')
	    walker++;

	dircount = 1;
	ಹುಡುಕು_ctx->ffsc_stopdirs_v = ALLOC_ONE(string_T);

	ಆದರೆ (ಹುಡುಕು_ctx->ffsc_stopdirs_v != ಖಾಲಿ)
	{
	    string_T	*tmp;			// for convenience

	    ಮಾಡು
	    {
		char_u	*helper;
		ಶೂನ್ಯ	*ptr;
		size_t	len;

		helper = walker;
		ptr = vim_realloc(ಹುಡುಕು_ctx->ffsc_stopdirs_v,
					   (dircount + 1) * ಗಾತ್ರ(string_T));
		ಆದರೆ (ptr)
		    ಹುಡುಕು_ctx->ffsc_stopdirs_v = ptr;
		ಇಲ್ಲದಿದ್ದರೆ
		    // ignore, keep what we have and continue
		    ನಿಲ್ಲಿಸು;
		walker = vim_strchr(walker, ';');
		len = walker ? (size_t)(walker - helper) : STRLEN(helper);
		// "" means ascent till top of directory tree.

		ಆದರೆ (*helper != NUL && !vim_isAbsName(helper)
							 && len + 1 < MAXPATHL)
		{
		    // Make the stop dir an absolute path name.
		    vim_strncpy(ff_expand_ಬಫರ್.string, helper, len);
		    ff_expand_ಬಫರ್.ಉದ್ದಳತೆ = len;

		    tmp = &ಹುಡುಕು_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->string = ತುಂಬಿದ_Name_save(ff_expand_ಬಫರ್.string, FALSE);
		    ಆದರೆ (tmp->string != ಖಾಲಿ)
			tmp->ಉದ್ದಳತೆ = STRLEN(tmp->string);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    tmp = &ಹುಡುಕು_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->ಉದ್ದಳತೆ = len;
		    tmp->string = vim_strnsave(helper, tmp->ಉದ್ದಳತೆ);
		    ಆದರೆ (tmp->string == ಖಾಲಿ)
			tmp->ಉದ್ದಳತೆ = 0;
		}
		ಆದರೆ (walker)
		    walker++;
		dircount++;

	    } ಆಗ (walker != ಖಾಲಿ);

	    tmp = &ಹುಡುಕು_ctx->ffsc_stopdirs_v[dircount - 1];
	    tmp->string = ಖಾಲಿ;
	    tmp->ಉದ್ದಳತೆ = 0;
	}
    }

    ಹುಡುಕು_ctx->ffsc_level = level;

    /*
     * split into:
     *  -fix path
     *  -wildcard_stuff (might be NULL)
     */
    wc_part = vim_strchr(ಮಾರ್ಗ, '*');
    ಆದರೆ (wc_part != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	llevel;
	ಅಕ್ಷರ	*errpt;

	// save the fix part of the path
	ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ = (size_t)(wc_part - ಮಾರ್ಗ);
	ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string = vim_strnsave(ಮಾರ್ಗ,
	    ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ);
	ಆದರೆ (ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string == ಖಾಲಿ)
	    ಹೋಗು error_return;

	/*
	 * copy wc_path and add restricts to the '**' wildcard.
	 * The octet after a '**' is used as a (binary) counter.
	 * So '**3' is transposed to '**^C' ('^C' is ASCII value 3)
	 * or '**76' is transposed to '**N'( 'N' is ASCII value 76).
	 * If no restrict is given after '**' the default is used.
	 * Due to this technique the path looks awful if you print it as a
	 * string.
	 */
	ff_expand_ಬಫರ್.ಉದ್ದಳತೆ = 0;
	ಆಗ (*wc_part != NUL)
	{
	    ಆದರೆ (ff_expand_ಬಫರ್.ಉದ್ದಳತೆ + 5 >= MAXPATHL)
	    {
		emsg(_(e_ಮಾರ್ಗ_too_long_for_completion));
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (STRNCMP(wc_part, "**", 2) == 0)
	    {
		ff_expand_ಬಫರ್.string[ff_expand_ಬಫರ್.ಉದ್ದಳತೆ++] = *wc_part++;
		ff_expand_ಬಫರ್.string[ff_expand_ಬಫರ್.ಉದ್ದಳತೆ++] = *wc_part++;

		llevel = strtol((ಅಕ್ಷರ *)wc_part, &errpt, 10);
		ಆದರೆ ((char_u *)errpt != wc_part && llevel > 0 && llevel < 255)
		    ff_expand_ಬಫರ್.string[ff_expand_ಬಫರ್.ಉದ್ದಳತೆ++] = llevel;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((char_u *)errpt != wc_part && llevel == 0)
		    // restrict is 0 -> remove already added '**'
		    ff_expand_ಬಫರ್.ಉದ್ದಳತೆ -= 2;
		ಇಲ್ಲದಿದ್ದರೆ
		    ff_expand_ಬಫರ್.string[ff_expand_ಬಫರ್.ಉದ್ದಳತೆ++] = FF_MAX_ನಕ್ಷತ್ರ_ನಕ್ಷತ್ರ_EXPAND;
		wc_part = (char_u *)errpt;
		ಆದರೆ (*wc_part != NUL && !vim_ispathsep(*wc_part))
		{
		    semsg(_(e_invalid_ಮಾರ್ಗ_ಸಂಖ್ಯೆ_must_be_at_ಕೊನೆ_ದ_ಮಾರ್ಗ_or_be_followed_by_str), PATHSEPSTR);
		    ಹೋಗು error_return;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ff_expand_ಬಫರ್.string[ff_expand_ಬಫರ್.ಉದ್ದಳತೆ++] = *wc_part++;
	}
	ff_expand_ಬಫರ್.string[ff_expand_ಬಫರ್.ಉದ್ದಳತೆ] = NUL;

	ಹುಡುಕು_ctx->ffsc_wc_ಮಾರ್ಗ.ಉದ್ದಳತೆ = ff_expand_ಬಫರ್.ಉದ್ದಳತೆ;
	ಹುಡುಕು_ctx->ffsc_wc_ಮಾರ್ಗ.string = vim_strnsave(ff_expand_ಬಫರ್.string,
	    ಹುಡುಕು_ctx->ffsc_wc_ಮಾರ್ಗ.ಉದ್ದಳತೆ);
	ಆದರೆ (ಹುಡುಕು_ctx->ffsc_wc_ಮಾರ್ಗ.string == ಖಾಲಿ)
	    ಹೋಗು error_return;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ = STRLEN(ಮಾರ್ಗ);
	ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string = vim_strnsave(ಮಾರ್ಗ,
	    ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ);
	ಆದರೆ (ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string == ಖಾಲಿ)
	    ಹೋಗು error_return;
    }

    ಆದರೆ (ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string == ಖಾಲಿ)
    {
	// store the fix part as startdir.
	// This is needed if the parameter path is fully qualified.
	ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.ಉದ್ದಳತೆ = ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ;
	ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string = vim_strnsave(ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string,
	    ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.ಉದ್ದಳತೆ);
	ಆದರೆ (ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string == ಖಾಲಿ)
	    ಹೋಗು error_return;
	ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string[0] = NUL;
	ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ = 0;
    }

    // create an absolute path
    ಆದರೆ (ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.ಉದ್ದಳತೆ
	    + ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ + 3 >= MAXPATHL)
    {
	emsg(_(e_ಮಾರ್ಗ_too_long_for_completion));
	ಹೋಗು error_return;
    }

    ಕೂಡಿಸು_sep = !after_pathsep(ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string,
	ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string + ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.ಉದ್ದಳತೆ);
    ff_expand_ಬಫರ್.ಉದ್ದಳತೆ = vim_snprintf(
	    (ಅಕ್ಷರ *)ff_expand_ಬಫರ್.string,
	    MAXPATHL,
	    "%s%s",
	    ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string,
	    ಕೂಡಿಸು_sep ? PATHSEPSTR : "");

    {
	size_t	bufsize = ff_expand_ಬಫರ್.ಉದ್ದಳತೆ + ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ + 1;
	char_u	*buf = alloc(bufsize);

	ಆದರೆ (buf == ಖಾಲಿ)
	    ಹೋಗು error_return;

	vim_snprintf(
		(ಅಕ್ಷರ *)buf,
		bufsize,
		"%s%s",
		ff_expand_ಬಫರ್.string,
		ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string);
	ಆದರೆ (mch_isdir(buf))
	{
	    ಆದರೆ (ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ > 0)
	    {
		ಕೂಡಿಸು_sep = !after_pathsep(ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string,
		    ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string + ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ);
		ff_expand_ಬಫರ್.ಉದ್ದಳತೆ += vim_snprintf(
			(ಅಕ್ಷರ *)ff_expand_ಬಫರ್.string + ff_expand_ಬಫರ್.ಉದ್ದಳತೆ,
			MAXPATHL - ff_expand_ಬಫರ್.ಉದ್ದಳತೆ,
			"%s%s",
			ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string,
			ಕೂಡಿಸು_sep ? PATHSEPSTR : "");
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u *p = gettail(ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string);
	    ಪೂರ್ಣಾಂಕ    len = (ಪೂರ್ಣಾಂಕ)ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ;

	    ಆದರೆ (p > ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string)
	    {
		// do not add '..' to the path and start upwards searching
		len = (ಪೂರ್ಣಾಂಕ)(p - ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string) - 1;
		ಆದರೆ ((len >= 2
			&& STRNCMP(ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string, "..", 2) == 0)
			&& (len == 2 || ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string[2] == PATHSEP))
		{
		    vim_free(buf);
		    ಹೋಗು error_return;
		}

		ಕೂಡಿಸು_sep = !after_pathsep(ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string,
		    ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string + ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ);
		ff_expand_ಬಫರ್.ಉದ್ದಳತೆ += vim_snprintf(
			(ಅಕ್ಷರ *)ff_expand_ಬಫರ್.string + ff_expand_ಬಫರ್.ಉದ್ದಳತೆ,
			MAXPATHL - ff_expand_ಬಫರ್.ಉದ್ದಳತೆ,
			"%.*s%s",
			len,
			ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string,
			ಕೂಡಿಸು_sep ? PATHSEPSTR : "");
	    }

	    ಆದರೆ (ಹುಡುಕು_ctx->ffsc_wc_ಮಾರ್ಗ.string != ಖಾಲಿ)
	    {
		size_t	tempsize = (ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ - len)
				+ ಹುಡುಕು_ctx->ffsc_wc_ಮಾರ್ಗ.ಉದ್ದಳತೆ
				+ 1;
		char_u	*ತಾತ್ಕಾಲಿಕ = alloc(tempsize);

		ಆದರೆ (ತಾತ್ಕಾಲಿಕ == ಖಾಲಿ)
		{
		    vim_free(buf);
		    vim_free(ತಾತ್ಕಾಲಿಕ);
		    ಹೋಗು error_return;
		}

		ಹುಡುಕು_ctx->ffsc_wc_ಮಾರ್ಗ.ಉದ್ದಳತೆ = vim_snprintf(
			    (ಅಕ್ಷರ *)ತಾತ್ಕಾಲಿಕ,
			    tempsize,
			    "%s%s",
			    ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string + len,
			    ಹುಡುಕು_ctx->ffsc_wc_ಮಾರ್ಗ.string);
		vim_free(ಹುಡುಕು_ctx->ffsc_wc_ಮಾರ್ಗ.string);
		ಹುಡುಕು_ctx->ffsc_wc_ಮಾರ್ಗ.string = ತಾತ್ಕಾಲಿಕ;
	    }
	}
	vim_free(buf);
    }

    sptr = ff_create_ರಾಶಿ_element(ff_expand_ಬಫರ್.string,
			ff_expand_ಬಫರ್.ಉದ್ದಳತೆ,
			ಹುಡುಕು_ctx->ffsc_wc_ಮಾರ್ಗ.string,
			ಹುಡುಕು_ctx->ffsc_wc_ಮಾರ್ಗ.ಉದ್ದಳತೆ,
			level,
			0);

    ಆದರೆ (sptr == ಖಾಲಿ)
	ಹೋಗು error_return;

    ff_ತಳ್ಳು(ಹುಡುಕು_ctx, sptr);

    ಹುಡುಕು_ctx->ffsc_file_ಗಮ್ಯ_ಹುಡುಕು.ಉದ್ದಳತೆ = filenamelen;
    ಹುಡುಕು_ctx->ffsc_file_ಗಮ್ಯ_ಹುಡುಕು.string = vim_strnsave(filename,
	ಹುಡುಕು_ctx->ffsc_file_ಗಮ್ಯ_ಹುಡುಕು.ಉದ್ದಳತೆ);
    ಆದರೆ (ಹುಡುಕು_ctx->ffsc_file_ಗಮ್ಯ_ಹುಡುಕು.string == ಖಾಲಿ)
	ಹೋಗು error_return;

    ಹಿಂತಿರುಗಿ ಹುಡುಕು_ctx;

error_return:
    /*
     * We clear the search context now!
     * Even when the caller gave us a (perhaps valid) context we free it here,
     * as we might have already destroyed it.
     */
    vim_findfile_cleanup(ಹುಡುಕು_ctx);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Get the stopdir string.  Check that ';' is not escaped.
 */
    char_u *
vim_findfile_stopdir(char_u *buf)
{
    char_u	*r_ptr = buf;
    char_u	*r_ptr_ಕೊನೆ = ಖಾಲಿ;	    // points to NUL at end of string "r_ptr"

    ಆಗ (*r_ptr != NUL && *r_ptr != ';')
    {
	ಆದರೆ (r_ptr[0] == '\\' && r_ptr[1] == ';')
	{
	    // Overwrite the escape char,
	    // use STRLEN(r_ptr) to move the trailing '\0'.
	    ಆದರೆ (r_ptr_ಕೊನೆ == ಖಾಲಿ)
		r_ptr_ಕೊನೆ = r_ptr + STRLEN(r_ptr);
	    mch_memmove(r_ptr, r_ptr + 1,
		(size_t)(r_ptr_ಕೊನೆ - (r_ptr + 1)) + 1);	// +1 for NUL
	    r_ptr++;
	    --r_ptr_ಕೊನೆ;
	}
	r_ptr++;
    }
    ಆದರೆ (*r_ptr == ';')
    {
	*r_ptr = NUL;
	r_ptr++;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*r_ptr == NUL)
	r_ptr = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ r_ptr;
}

/*
 * Clean up the given search context. Can handle a NULL pointer.
 */
    ಶೂನ್ಯ
vim_findfile_cleanup(ಶೂನ್ಯ *ctx)
{
    ಆದರೆ (ctx == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    vim_findfile_free_ಭೇಟಿಯಾದ(ctx);
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
vim_findfile(ಶೂನ್ಯ *ಹುಡುಕು_ctx_arg)
{
    string_T	file_ಮಾರ್ಗ;
    string_T	rest_ದ_wildcards;
    char_u	*ಮಾರ್ಗ_ಕೊನೆ = ಖಾಲಿ;
    ff_ರಾಶಿ_T	*stackp;
    ff_ಹುಡುಕು_ctx_T *ಹುಡುಕು_ctx;

    ಆದರೆ (ಹುಡುಕು_ctx_arg == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹುಡುಕು_ctx = (ff_ಹುಡುಕು_ctx_T *)ಹುಡುಕು_ctx_arg;

    /*
     * filepath is used as buffer for various actions and as the storage to
     * return a found filename.
     */
    ಆದರೆ ((file_ಮಾರ್ಗ.string = alloc(MAXPATHL)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // store the end of the start dir -- needed for upward search
    ಆದರೆ (ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string != ಖಾಲಿ)
	ಮಾರ್ಗ_ಕೊನೆ = &ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string[
					    ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.ಉದ್ದಳತೆ];

    // upward search loop
    ಗಾಗಿ (;;)
    {
	// downward search loop
	ಗಾಗಿ (;;)
	{
	    // check if user wants to stop the search
	    ui_breakcheck();
	    ಆದರೆ (got_int)
		ನಿಲ್ಲಿಸು;

	    // get directory to work on from stack
	    stackp = ff_ತೆಗೆ(ಹುಡುಕು_ctx);
	    ಆದರೆ (stackp == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;

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
	    ಆದರೆ (stackp->ffs_filearray == ಖಾಲಿ
		    && ff_check_ಭೇಟಿಯಾದ(&ಹುಡುಕು_ctx->ffsc_dir_ಭೇಟಿಯಾದ_list
							  ->ffvl_ಭೇಟಿಯಾದ_list,
			    stackp->ffs_fix_ಮಾರ್ಗ.string,
			    stackp->ffs_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ,
			    stackp->ffs_wc_ಮಾರ್ಗ.string,
			    stackp->ffs_wc_ಮಾರ್ಗ.ಉದ್ದಳತೆ) == FAIL)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FF_VERBOSE
		ಆದರೆ (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("Already Searched: %s (%s)",
				   stackp->ffs_fix_ಮಾರ್ಗ.string, stackp->ffs_wc_ಮಾರ್ಗ.string);
		    // don't overwrite this either
		    ಸಂದೇಶ_puts("\n");
		    verbose_leave_scroll();
		}
#ಆದರೆ_ಅಂತ್ಯ
		ff_free_ರಾಶಿ_element(stackp);
		ಮುಂದುವರಿ;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FF_VERBOSE
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_verbose >= 5)
	    {
		verbose_enter_scroll();
		smsg("Searching: %s (%s)",
				   stackp->ffs_fix_ಮಾರ್ಗ.string, stackp->ffs_wc_ಮಾರ್ಗ.string);
		// don't overwrite this either
		ಸಂದೇಶ_puts("\n");
		verbose_leave_scroll();
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    // check depth
	    ಆದರೆ (stackp->ffs_level <= 0)
	    {
		ff_free_ರಾಶಿ_element(stackp);
		ಮುಂದುವರಿ;
	    }

	    file_ಮಾರ್ಗ.string[0] = NUL;
	    file_ಮಾರ್ಗ.ಉದ್ದಳತೆ = 0;

	    /*
	     * If no filearray till now expand wildcards
	     * The function expand_wildcards() can handle an array of paths
	     * and all possible expands are returned in one array. We use this
	     * to handle the expansion of '**' into an empty string.
	     */
	    ಆದರೆ (stackp->ffs_filearray == ಖಾಲಿ)
	    {
		char_u *dirptrs[2];

		// we use filepath to build the path expand_wildcards() should
		// expand.
		dirptrs[0] = file_ಮಾರ್ಗ.string;
		dirptrs[1] = ಖಾಲಿ;

		// if we have a start dir copy it in
		ಆದರೆ (!vim_isAbsName(stackp->ffs_fix_ಮಾರ್ಗ.string)
						&& ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string)
		{
		    ಆದರೆ (ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.ಉದ್ದಳತೆ + 1 < MAXPATHL)
		    {
			ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_sep = !after_pathsep(ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string,
			    ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string + ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.ಉದ್ದಳತೆ);
			file_ಮಾರ್ಗ.ಉದ್ದಳತೆ = vim_snprintf(
			    (ಅಕ್ಷರ *)file_ಮಾರ್ಗ.string,
			    MAXPATHL,
			    "%s%s",
			    ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string,
			    ಕೂಡಿಸು_sep ? PATHSEPSTR : "");
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ff_free_ರಾಶಿ_element(stackp);
			ಹೋಗು fail;
		    }
		}

		// append the fix part of the search path
		ಆದರೆ (file_ಮಾರ್ಗ.ಉದ್ದಳತೆ + stackp->ffs_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ + 1 < MAXPATHL)
		{
		    ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_sep = !after_pathsep(stackp->ffs_fix_ಮಾರ್ಗ.string,
			stackp->ffs_fix_ಮಾರ್ಗ.string + stackp->ffs_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ);
		    file_ಮಾರ್ಗ.ಉದ್ದಳತೆ += vim_snprintf(
			(ಅಕ್ಷರ *)file_ಮಾರ್ಗ.string + file_ಮಾರ್ಗ.ಉದ್ದಳತೆ,
			MAXPATHL - file_ಮಾರ್ಗ.ಉದ್ದಳತೆ,
			"%s%s",
			stackp->ffs_fix_ಮಾರ್ಗ.string,
			ಕೂಡಿಸು_sep ? PATHSEPSTR : "");
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ff_free_ರಾಶಿ_element(stackp);
		    ಹೋಗು fail;
		}

		rest_ದ_wildcards.string = stackp->ffs_wc_ಮಾರ್ಗ.string;
		rest_ದ_wildcards.ಉದ್ದಳತೆ = stackp->ffs_wc_ಮಾರ್ಗ.ಉದ್ದಳತೆ;
		ಆದರೆ (*rest_ದ_wildcards.string != NUL)
		{
		    ಆದರೆ (STRNCMP(rest_ದ_wildcards.string, "**", 2) == 0)
		    {
			char_u	*p;

			// pointer to the restrict byte
			// The restrict byte is not a character!
			p = rest_ದ_wildcards.string + 2;

			ಆದರೆ (*p > 0)
			{
			    (*p)--;
			    ಆದರೆ (file_ಮಾರ್ಗ.ಉದ್ದಳತೆ + 1 < MAXPATHL)
				file_ಮಾರ್ಗ.string[file_ಮಾರ್ಗ.ಉದ್ದಳತೆ++] = '*';
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				ff_free_ರಾಶಿ_element(stackp);
				ಹೋಗು fail;
			    }
			}

			ಆದರೆ (*p == 0)
			{
			    // remove '**<numb> from wildcards
			    mch_memmove(rest_ದ_wildcards.string,
					rest_ದ_wildcards.string + 3,
					(size_t)(rest_ದ_wildcards.ಉದ್ದಳತೆ - 3) + 1);    // +1 for NUL
			    rest_ದ_wildcards.ಉದ್ದಳತೆ -= 3;
			    stackp->ffs_wc_ಮಾರ್ಗ.ಉದ್ದಳತೆ = rest_ದ_wildcards.ಉದ್ದಳತೆ;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    rest_ದ_wildcards.string += 3;
			    rest_ದ_wildcards.ಉದ್ದಳತೆ -= 3;
			}

			ಆದರೆ (stackp->ffs_ನಕ್ಷತ್ರ_ನಕ್ಷತ್ರ_ಖಾಲಿಗಳು == 0)
			{
			    // if not done before, expand '**' to empty
			    stackp->ffs_ನಕ್ಷತ್ರ_ನಕ್ಷತ್ರ_ಖಾಲಿಗಳು = 1;
			    dirptrs[1] = stackp->ffs_fix_ಮಾರ್ಗ.string;
			}
		    }

		    /*
		     * Here we copy until the next path separator or the end of
		     * the path. If we stop at a path separator, there is
		     * still something else left. This is handled below by
		     * pushing every directory returned from expand_wildcards()
		     * on the stack again for further search.
		     */
		    ಆಗ (*rest_ದ_wildcards.string
			    && !vim_ispathsep(*rest_ದ_wildcards.string))
		    {
			ಆದರೆ (file_ಮಾರ್ಗ.ಉದ್ದಳತೆ + 1 < MAXPATHL)
			{
			    file_ಮಾರ್ಗ.string[file_ಮಾರ್ಗ.ಉದ್ದಳತೆ++] = *rest_ದ_wildcards.string++;
			    --rest_ದ_wildcards.ಉದ್ದಳತೆ;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ff_free_ರಾಶಿ_element(stackp);
			    ಹೋಗು fail;
			}
		    }

		    file_ಮಾರ್ಗ.string[file_ಮಾರ್ಗ.ಉದ್ದಳತೆ] = NUL;
		    ಆದರೆ (vim_ispathsep(*rest_ದ_wildcards.string))
		    {
			rest_ದ_wildcards.string++;
			rest_ದ_wildcards.ಉದ್ದಳತೆ--;
		    }
		}

		/*
		 * Expand wildcards like "*" and "$VAR".
		 * If the path is a URL don't try this.
		 */
		ಆದರೆ (ಮಾರ್ಗ_with_url(dirptrs[0]))
		{
		    stackp->ffs_filearray = ALLOC_ONE(char_u *);
		    ಆದರೆ (stackp->ffs_filearray != ಖಾಲಿ
			    && (stackp->ffs_filearray[0]
				= vim_strnsave(dirptrs[0], file_ಮಾರ್ಗ.ಉದ್ದಳತೆ)) != ಖಾಲಿ)
			stackp->ffs_filearray_ಗಾತ್ರ = 1;
		    ಇಲ್ಲದಿದ್ದರೆ
			stackp->ffs_filearray_ಗಾತ್ರ = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    // Add EW_NOTWILD because the expanded path may contain
		    // wildcard characters that are to be taken literally.
		    // This is a bit of a hack.
		    expand_wildcards((dirptrs[1] == ಖಾಲಿ) ? 1 : 2, dirptrs,
			    &stackp->ffs_filearray_ಗಾತ್ರ,
			    &stackp->ffs_filearray,
			    EW_DIR|EW_ADDSLASH|EW_SILENT|EW_NOTWILD);

		stackp->ffs_filearray_cur = 0;
		stackp->ffs_stage = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		rest_ದ_wildcards.string = &stackp->ffs_wc_ಮಾರ್ಗ.string[
							stackp->ffs_wc_ಮಾರ್ಗ.ಉದ್ದಳತೆ];
		rest_ದ_wildcards.ಉದ್ದಳತೆ = 0;
	    }

	    ಆದರೆ (stackp->ffs_stage == 0)
	    {
		ಪೂರ್ಣಾಂಕ i;

		// this is the first time we work on this directory
		ಆದರೆ (*rest_ದ_wildcards.string == NUL)
		{
		    size_t  len;
		    char_u  *suf;

		    /*
		     * We don't have further wildcards to expand, so we have to
		     * check for the final file now.
		     */
		    ಗಾಗಿ (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_ಗಾತ್ರ; ++i)
		    {
			ಆದರೆ (!ಮಾರ್ಗ_with_url(stackp->ffs_filearray[i])
				      && !mch_isdir(stackp->ffs_filearray[i]))
			    ಮುಂದುವರಿ;   // not a directory

			// prepare the filename to be checked for existence
			//ಕೆಳಗೆ
			len = STRLEN(stackp->ffs_filearray[i]);
			ಆದರೆ (len + 1 + ಹುಡುಕು_ctx->ffsc_file_ಗಮ್ಯ_ಹುಡುಕು.ಉದ್ದಳತೆ
									< MAXPATHL)
			{
			    ಪೂರ್ಣಾಂಕ	ಕೂಡಿಸು_sep = !after_pathsep(stackp->ffs_filearray[i],
				    stackp->ffs_filearray[i] + len);
			    file_ಮಾರ್ಗ.ಉದ್ದಳತೆ = vim_snprintf(
				    (ಅಕ್ಷರ *)file_ಮಾರ್ಗ.string,
				    MAXPATHL,
				    "%s%s%s",
				    stackp->ffs_filearray[i],
				    ಕೂಡಿಸು_sep ? PATHSEPSTR : "",
				    ಹುಡುಕು_ctx->ffsc_file_ಗಮ್ಯ_ಹುಡುಕು.string);
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ff_free_ರಾಶಿ_element(stackp);
			    ಹೋಗು fail;
			}

			/*
			 * Try without extra suffix and then with suffixes
			 * from 'suffixesadd'.
			 */
			len = file_ಮಾರ್ಗ.ಉದ್ದಳತೆ;
			ಆದರೆ (ಹುಡುಕು_ctx->ffsc_tagfile)
			    suf = (char_u *)"";
			ಇಲ್ಲದಿದ್ದರೆ
			    suf = curbuf->b_p_sua;
			ಗಾಗಿ (;;)
			{
			    // if file exists and we didn't already find it
			    ಆದರೆ ((ಮಾರ್ಗ_with_url(file_ಮಾರ್ಗ.string)
				  || (mch_getperm(file_ಮಾರ್ಗ.string) >= 0
				      && (ಹುಡುಕು_ctx->ffsc_ಕಂಡುಹಿಡಿ_what
							      == FINDFILE_BOTH
					  || ((ಹುಡುಕು_ctx->ffsc_ಕಂಡುಹಿಡಿ_what
							      == FINDFILE_DIR)
						   == mch_isdir(file_ಮಾರ್ಗ.string)))))
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FF_VERBOSE
				    && (ff_check_ಭೇಟಿಯಾದ(
					    &ಹುಡುಕು_ctx->ffsc_ಭೇಟಿಯಾದ_list
							   ->ffvl_ಭೇಟಿಯಾದ_list,
					    file_ಮಾರ್ಗ.string,
					    file_ಮಾರ್ಗ.ಉದ್ದಳತೆ,
					    (char_u *)"", 0) == OK)
#ಆದರೆ_ಅಂತ್ಯ
			       )
			    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FF_VERBOSE
				ಆದರೆ (ff_check_ಭೇಟಿಯಾದ(
					    &ಹುಡುಕು_ctx->ffsc_ಭೇಟಿಯಾದ_list
							   ->ffvl_ಭೇಟಿಯಾದ_list,
					      file_ಮಾರ್ಗ.string,
					      file_ಮಾರ್ಗ.ಉದ್ದಳತೆ,
					      (char_u *)"", 0) == FAIL)
				{
				    ಆದರೆ (p_verbose >= 5)
				    {
					verbose_enter_scroll();
					smsg("Already: %s", file_ಮಾರ್ಗ.string);
					// don't overwrite this either
					ಸಂದೇಶ_puts("\n");
					verbose_leave_scroll();
				    }
				    ಮುಂದುವರಿ;
				}
#ಆದರೆ_ಅಂತ್ಯ

				// push dir to examine rest of subdirs later
				stackp->ffs_filearray_cur = i + 1;
				ff_ತಳ್ಳು(ಹುಡುಕು_ctx, stackp);

				ಆದರೆ (!ಮಾರ್ಗ_with_url(file_ಮಾರ್ಗ.string))
				    file_ಮಾರ್ಗ.ಉದ್ದಳತೆ = simplify_filename(file_ಮಾರ್ಗ.string);

				ಆದರೆ (mch_dirname(ff_expand_ಬಫರ್.string, MAXPATHL)
									== OK)
				{
				    char_u  *p;

				    ff_expand_ಬಫರ್.ಉದ್ದಳತೆ = STRLEN(ff_expand_ಬಫರ್.string);
				    p = shorten_fname(file_ಮಾರ್ಗ.string,
							    ff_expand_ಬಫರ್.string);
				    ಆದರೆ (p != ಖಾಲಿ)
				    {
					mch_memmove(file_ಮಾರ್ಗ.string, p,
					    (size_t)((file_ಮಾರ್ಗ.string + file_ಮಾರ್ಗ.ಉದ್ದಳತೆ) - p) + 1);  // +1 for NUL
					file_ಮಾರ್ಗ.ಉದ್ದಳತೆ -= (p - file_ಮಾರ್ಗ.string);
				    }
				}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FF_VERBOSE
				ಆದರೆ (p_verbose >= 5)
				{
				    verbose_enter_scroll();
				    smsg("HIT: %s", file_ಮಾರ್ಗ.string);
				    // don't overwrite this either
				    ಸಂದೇಶ_puts("\n");
				    verbose_leave_scroll();
				}
#ಆದರೆ_ಅಂತ್ಯ
				ಹಿಂತಿರುಗಿ file_ಮಾರ್ಗ.string;
			    }

			    // Not found or found already, try next suffix.
			    ಆದರೆ (*suf == NUL)
				ನಿಲ್ಲಿಸು;
			    file_ಮಾರ್ಗ.ಉದ್ದಳತೆ = len + copy_option_part(&suf,
				  file_ಮಾರ್ಗ.string + len,
				  (ಪೂರ್ಣಾಂಕ)(MAXPATHL - len), ",");
			}
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    /*
		     * still wildcards left, push the directories for further
		     * search
		     */
		    ಗಾಗಿ (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_ಗಾತ್ರ; ++i)
		    {
			ಆದರೆ (!mch_isdir(stackp->ffs_filearray[i]))
			    ಮುಂದುವರಿ;	// not a directory

			ff_ತಳ್ಳು(ಹುಡುಕು_ctx,
				ff_create_ರಾಶಿ_element(
						     stackp->ffs_filearray[i],
						     STRLEN(stackp->ffs_filearray[i]),
						     rest_ದ_wildcards.string,
						     rest_ದ_wildcards.ಉದ್ದಳತೆ,
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
	    ಆದರೆ (STRNCMP(stackp->ffs_wc_ಮಾರ್ಗ.string, "**", 2) == 0)
	    {
		ಪೂರ್ಣಾಂಕ i;

		ಗಾಗಿ (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_ಗಾತ್ರ; ++i)
		{
		    ಆದರೆ (fnamecmp(stackp->ffs_filearray[i],
						   stackp->ffs_fix_ಮಾರ್ಗ.string) == 0)
			ಮುಂದುವರಿ; // don't repush same directory
		    ಆದರೆ (!mch_isdir(stackp->ffs_filearray[i]))
			ಮುಂದುವರಿ;   // not a directory
		    ff_ತಳ್ಳು(ಹುಡುಕು_ctx,
			    ff_create_ರಾಶಿ_element(
				stackp->ffs_filearray[i],
				STRLEN(stackp->ffs_filearray[i]),
				stackp->ffs_wc_ಮಾರ್ಗ.string,
				stackp->ffs_wc_ಮಾರ್ಗ.ಉದ್ದಳತೆ,
				stackp->ffs_level - 1, 1));
		}
	    }

	    // we are done with the current directory
	    ff_free_ರಾಶಿ_element(stackp);
	}

	// If we reached this, we didn't find anything downwards.
	// Let's check if we should do an upward search.
	ಆದರೆ (ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string
		&& ಹುಡುಕು_ctx->ffsc_stopdirs_v != ಖಾಲಿ && !got_int)
	{
	    ff_ರಾಶಿ_T  *sptr;
	    // path_end may point to the NUL or the previous path separator
	    ಪೂರ್ಣಾಂಕ plen = (ಮಾರ್ಗ_ಕೊನೆ - ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string)
							  + (*ಮಾರ್ಗ_ಕೊನೆ != NUL);

	    // is the last starting directory in the stop list?
	    ಆದರೆ (ff_ಮಾರ್ಗ_ಒಳ_stoplist(ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string,
				    plen, ಹುಡುಕು_ctx->ffsc_stopdirs_v) == TRUE)
		ನಿಲ್ಲಿಸು;

	    // cut of last dir
	    ಆಗ (ಮಾರ್ಗ_ಕೊನೆ > ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string
						  && vim_ispathsep(*ಮಾರ್ಗ_ಕೊನೆ))
		ಮಾರ್ಗ_ಕೊನೆ--;
	    ಆಗ (ಮಾರ್ಗ_ಕೊನೆ > ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string
					      && !vim_ispathsep(ಮಾರ್ಗ_ಕೊನೆ[-1]))
		ಮಾರ್ಗ_ಕೊನೆ--;
	    *ಮಾರ್ಗ_ಕೊನೆ = NUL;

	    // we may have shortened search_ctx->ffsc_start_dir, so update it's length
	    ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.ಉದ್ದಳತೆ = (size_t)(ಮಾರ್ಗ_ಕೊನೆ - ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string);
	    ಮಾರ್ಗ_ಕೊನೆ--;

	    ಆದರೆ (*ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string == NUL)
		ನಿಲ್ಲಿಸು;

	    ಆದರೆ (ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.ಉದ್ದಳತೆ + 1
		    + ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ < MAXPATHL)
	    {
		ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_sep = !after_pathsep(ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string,
			    ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string + ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.ಉದ್ದಳತೆ);
		file_ಮಾರ್ಗ.ಉದ್ದಳತೆ = vim_snprintf(
			(ಅಕ್ಷರ *)file_ಮಾರ್ಗ.string,
			MAXPATHL,
			"%s%s%s",
			ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir.string,
			ಕೂಡಿಸು_sep ? PATHSEPSTR : "",
			ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ.string);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹೋಗು fail;

	    // create a new stack entry
	    sptr = ff_create_ರಾಶಿ_element(file_ಮಾರ್ಗ.string, file_ಮಾರ್ಗ.ಉದ್ದಳತೆ,
		    ಹುಡುಕು_ctx->ffsc_wc_ಮಾರ್ಗ.string, ಹುಡುಕು_ctx->ffsc_wc_ಮಾರ್ಗ.ಉದ್ದಳತೆ,
		    ಹುಡುಕು_ctx->ffsc_level, 0);
	    ಆದರೆ (sptr == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    ff_ತಳ್ಳು(ಹುಡುಕು_ctx, sptr);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }

fail:
    vim_free(file_ಮಾರ್ಗ.string);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Free the list of lists of visited files and directories
 * Can handle it if the passed search_context is NULL;
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vim_findfile_free_ಭೇಟಿಯಾದ(ಶೂನ್ಯ *ಹುಡುಕು_ctx_arg)
{
    ff_ಹುಡುಕು_ctx_T *ಹುಡುಕು_ctx;

    ಆದರೆ (ಹುಡುಕು_ctx_arg == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಹುಡುಕು_ctx = (ff_ಹುಡುಕು_ctx_T *)ಹುಡುಕು_ctx_arg;
    vim_findfile_free_ಭೇಟಿಯಾದ_list(&ಹುಡುಕು_ctx->ffsc_ಭೇಟಿಯಾದ_lists_list);
    vim_findfile_free_ಭೇಟಿಯಾದ_list(&ಹುಡುಕು_ctx->ffsc_dir_ಭೇಟಿಯಾದ_lists_list);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_findfile_free_ಭೇಟಿಯಾದ_list(ff_ಭೇಟಿಯಾದ_list_hdr_T **list_headp)
{
    ff_ಭೇಟಿಯಾದ_list_hdr_T *vp;

    ಆಗ (*list_headp != ಖಾಲಿ)
    {
	vp = (*list_headp)->ffvl_ಮುಂದಿನ;
	ff_free_ಭೇಟಿಯಾದ_list((*list_headp)->ffvl_ಭೇಟಿಯಾದ_list);

	vim_free((*list_headp)->ffvl_filename);
	vim_free(*list_headp);
	*list_headp = vp;
    }
    *list_headp = ಖಾಲಿ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ff_free_ಭೇಟಿಯಾದ_list(ff_ಭೇಟಿಯಾದ_T *vl)
{
    ff_ಭೇಟಿಯಾದ_T *vp;

    ಆಗ (vl != ಖಾಲಿ)
    {
	vp = vl->ffv_ಮುಂದಿನ;
	vim_free(vl->ffv_wc_ಮಾರ್ಗ);
	vim_free(vl);
	vl = vp;
    }
    vl = ಖಾಲಿ;
}

/*
 * Returns the already visited list for the given filename. If none is found it
 * allocates a new one.
 */
    ಸ್ಥಿರ ff_ಭೇಟಿಯಾದ_list_hdr_T*
ff_get_ಭೇಟಿಯಾದ_list(
    char_u			*filename,
    size_t			filenamelen,
    ff_ಭೇಟಿಯಾದ_list_hdr_T	**list_headp)
{
    ff_ಭೇಟಿಯಾದ_list_hdr_T  *retptr = ಖಾಲಿ;

    // check if a visited list for the given filename exists
    ಆದರೆ (*list_headp != ಖಾಲಿ)
    {
	retptr = *list_headp;
	ಆಗ (retptr != ಖಾಲಿ)
	{
	    ಆದರೆ (fnamecmp(filename, retptr->ffvl_filename) == 0)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FF_VERBOSE
		ಆದರೆ (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("ff_get_visited_list: FOUND list for %s",
								    filename);
		    // don't overwrite this either
		    ಸಂದೇಶ_puts("\n");
		    verbose_leave_scroll();
		}
#ಆದರೆ_ಅಂತ್ಯ
		ಹಿಂತಿರುಗಿ retptr;
	    }
	    retptr = retptr->ffvl_ಮುಂದಿನ;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FF_VERBOSE
    ಆದರೆ (p_verbose >= 5)
    {
	verbose_enter_scroll();
	smsg("ff_get_visited_list: new list for %s", filename);
	// don't overwrite this either
	ಸಂದೇಶ_puts("\n");
	verbose_leave_scroll();
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * if we reach this we didn't find a list and we have to allocate new list
     */
    retptr = ALLOC_ONE(ff_ಭೇಟಿಯಾದ_list_hdr_T);
    ಆದರೆ (retptr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    retptr->ffvl_ಭೇಟಿಯಾದ_list = ಖಾಲಿ;
    retptr->ffvl_filename = vim_strnsave(filename, filenamelen);
    ಆದರೆ (retptr->ffvl_filename == ಖಾಲಿ)
    {
	vim_free(retptr);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    retptr->ffvl_ಮುಂದಿನ = *list_headp;
    *list_headp = retptr;

    ಹಿಂತಿರುಗಿ retptr;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ff_wc_equal(char_u *s1, char_u *s2)
{
    ಪೂರ್ಣಾಂಕ		i, j;
    ಪೂರ್ಣಾಂಕ		c1 = NUL;
    ಪೂರ್ಣಾಂಕ		c2 = NUL;
    ಪೂರ್ಣಾಂಕ		prev1 = NUL;
    ಪೂರ್ಣಾಂಕ		prev2 = NUL;

    ಆದರೆ (s1 == s2)
	ಹಿಂತಿರುಗಿ TRUE;

    ಆದರೆ (s1 == ಖಾಲಿ || s2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಗಾಗಿ (i = 0, j = 0; s1[i] != NUL && s2[j] != NUL;)
    {
	c1 = PTR2CHAR(s1 + i);
	c2 = PTR2CHAR(s2 + j);

	ಆದರೆ ((p_fic ? MB_TOLOWER(c1) != MB_TOLOWER(c2) : c1 != c2)
		&& (prev1 != '*' || prev2 != '*'))
	    ಹಿಂತಿರುಗಿ FALSE;
	prev2 = prev1;
	prev1 = c1;

	i += mb_ptr2len(s1 + i);
	j += mb_ptr2len(s2 + j);
    }
    ಹಿಂತಿರುಗಿ s1[i] == s2[j];
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ff_check_ಭೇಟಿಯಾದ(
    ff_ಭೇಟಿಯಾದ_T	**ಭೇಟಿಯಾದ_list,
    char_u		*fname,
    size_t		fnamelen,
    char_u		*wc_ಮಾರ್ಗ,
    size_t		wc_pathlen)
{
    ff_ಭೇಟಿಯಾದ_T	*vp;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    stat_T		st;
    ಪೂರ್ಣಾಂಕ			url = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // For a URL we only compare the name, otherwise we compare the
    // device/inode (unix) or the full path name (not Unix).
    ಆದರೆ (ಮಾರ್ಗ_with_url(fname))
    {
	vim_strncpy(ff_expand_ಬಫರ್.string, fname, fnamelen);
	ff_expand_ಬಫರ್.ಉದ್ದಳತೆ = fnamelen;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	url = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ff_expand_ಬಫರ್.string[0] = NUL;
	ff_expand_ಬಫರ್.ಉದ್ದಳತೆ = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname, &st) < 0)
	    ಹಿಂತಿರುಗಿ FAIL;
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (vim_ತುಂಬಿದ_Name(fname, ff_expand_ಬಫರ್.string, MAXPATHL, TRUE) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ff_expand_ಬಫರ್.ಉದ್ದಳತೆ = STRLEN(ff_expand_ಬಫರ್.string);
#ಆದರೆ_ಅಂತ್ಯ
    }

    // check against list of already visited files
    ಗಾಗಿ (vp = *ಭೇಟಿಯಾದ_list; vp != ಖಾಲಿ; vp = vp->ffv_ಮುಂದಿನ)
    {
	ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		!url ? (vp->ffv_dev_valid && vp->ffv_dev == st.st_dev
						  && vp->ffv_ino == st.st_ino)
		     :
#ಆದರೆ_ಅಂತ್ಯ
		fnamecmp(vp->ffv_fname, ff_expand_ಬಫರ್.string) == 0
	   )
	{
	    // are the wildcard parts equal
	    ಆದರೆ (ff_wc_equal(vp->ffv_wc_ಮಾರ್ಗ, wc_ಮಾರ್ಗ) == TRUE)
		// already visited
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    /*
     * New file/dir.  Add it to the list of visited files/dirs.
     */
    vp = alloc(
	     offsetof(ff_ಭೇಟಿಯಾದ_T, ffv_fname) + ff_expand_ಬಫರ್.ಉದ್ದಳತೆ + 1);
    ಆದರೆ (vp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಆದರೆ (!url)
    {
	vp->ffv_dev_valid = TRUE;
	vp->ffv_dev = st.st_dev;
	vp->ffv_ino = st.st_ino;
	vp->ffv_fname[0] = NUL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vp->ffv_dev_valid = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
	STRCPY(vp->ffv_fname, ff_expand_ಬಫರ್.string);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (wc_ಮಾರ್ಗ != ಖಾಲಿ)
	vp->ffv_wc_ಮಾರ್ಗ = vim_strnsave(wc_ಮಾರ್ಗ, wc_pathlen);
    ಇಲ್ಲದಿದ್ದರೆ
	vp->ffv_wc_ಮಾರ್ಗ = ಖಾಲಿ;

    vp->ffv_ಮುಂದಿನ = *ಭೇಟಿಯಾದ_list;
    *ಭೇಟಿಯಾದ_list = vp;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * create stack element from given path pieces
 */
    ಸ್ಥಿರ ff_ರಾಶಿ_T *
ff_create_ರಾಶಿ_element(
    char_u	*fix_part,
    size_t	fix_partlen,
    char_u	*wc_part,
    size_t	wc_partlen,
    ಪೂರ್ಣಾಂಕ		level,
    ಪೂರ್ಣಾಂಕ		ನಕ್ಷತ್ರ_ನಕ್ಷತ್ರ_ಖಾಲಿಗಳು)
{
    ff_ರಾಶಿ_T	*new;

    new = ALLOC_ONE(ff_ರಾಶಿ_T);
    ಆದರೆ (new == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    new->ffs_prev	   = ಖಾಲಿ;
    new->ffs_filearray	   = ಖಾಲಿ;
    new->ffs_filearray_ಗಾತ್ರ = 0;
    new->ffs_filearray_cur  = 0;
    new->ffs_stage	   = 0;
    new->ffs_level	   = level;
    new->ffs_ನಕ್ಷತ್ರ_ನಕ್ಷತ್ರ_ಖಾಲಿಗಳು = ನಕ್ಷತ್ರ_ನಕ್ಷತ್ರ_ಖಾಲಿಗಳು;

    // the following saves NULL pointer checks in vim_findfile
    ಆದರೆ (fix_part == ಖಾಲಿ)
    {
	fix_part = (char_u *)"";
	fix_partlen = 0;
    }
    new->ffs_fix_ಮಾರ್ಗ.string = vim_strnsave(fix_part, fix_partlen);
    new->ffs_fix_ಮಾರ್ಗ.ಉದ್ದಳತೆ = fix_partlen;

    ಆದರೆ (wc_part == ಖಾಲಿ)
    {
	wc_part = (char_u *)"";
	wc_partlen = 0;
    }
    new->ffs_wc_ಮಾರ್ಗ.string = vim_strnsave(wc_part, wc_partlen);
    new->ffs_wc_ಮಾರ್ಗ.ಉದ್ದಳತೆ = wc_partlen;

    ಆದರೆ (new->ffs_fix_ಮಾರ್ಗ.string == ಖಾಲಿ || new->ffs_wc_ಮಾರ್ಗ.string == ಖಾಲಿ)
    {
	ff_free_ರಾಶಿ_element(new);
	new = ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ new;
}

/*
 * Push a dir on the directory stack.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ff_ತಳ್ಳು(ff_ಹುಡುಕು_ctx_T *ಹುಡುಕು_ctx, ff_ರಾಶಿ_T *ರಾಶಿ_ptr)
{
    // check for NULL pointer, not to return an error to the user, but
    // to prevent a crash
    ಆದರೆ (ರಾಶಿ_ptr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ರಾಶಿ_ptr->ffs_prev = ಹುಡುಕು_ctx->ffsc_ರಾಶಿ_ptr;
    ಹುಡುಕು_ctx->ffsc_ರಾಶಿ_ptr = ರಾಶಿ_ptr;
}

/*
 * Pop a dir from the directory stack.
 * Returns NULL if stack is empty.
 */
    ಸ್ಥಿರ ff_ರಾಶಿ_T *
ff_ತೆಗೆ(ff_ಹುಡುಕು_ctx_T *ಹುಡುಕು_ctx)
{
    ff_ರಾಶಿ_T  *sptr;

    sptr = ಹುಡುಕು_ctx->ffsc_ರಾಶಿ_ptr;
    ಆದರೆ (ಹುಡುಕು_ctx->ffsc_ರಾಶಿ_ptr != ಖಾಲಿ)
	ಹುಡುಕು_ctx->ffsc_ರಾಶಿ_ptr = ಹುಡುಕು_ctx->ffsc_ರಾಶಿ_ptr->ffs_prev;

    ಹಿಂತಿರುಗಿ sptr;
}

/*
 * free the given stack element
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ff_free_ರಾಶಿ_element(ff_ರಾಶಿ_T *ರಾಶಿ_ptr)
{
    // VIM_CLEAR_STRING handles possible NULL pointers
    VIM_CLEAR_STRING(ರಾಶಿ_ptr->ffs_fix_ಮಾರ್ಗ);
    VIM_CLEAR_STRING(ರಾಶಿ_ptr->ffs_wc_ಮಾರ್ಗ);

    ಆದರೆ (ರಾಶಿ_ptr->ffs_filearray != ಖಾಲಿ)
	FreeWild(ರಾಶಿ_ptr->ffs_filearray_ಗಾತ್ರ, ರಾಶಿ_ptr->ffs_filearray);

    vim_free(ರಾಶಿ_ptr);
}

/*
 * Clear the search context, but NOT the visited list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ff_clear(ff_ಹುಡುಕು_ctx_T *ಹುಡುಕು_ctx)
{
    ff_ರಾಶಿ_T   *sptr;

    // clear up stack
    ಆಗ ((sptr = ff_ತೆಗೆ(ಹುಡುಕು_ctx)) != ಖಾಲಿ)
	ff_free_ರಾಶಿ_element(sptr);

    ಆದರೆ (ಹುಡುಕು_ctx->ffsc_stopdirs_v != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ  i = 0;

	ಆಗ (ಹುಡುಕು_ctx->ffsc_stopdirs_v[i].string != ಖಾಲಿ)
	{
	    vim_free(ಹುಡುಕು_ctx->ffsc_stopdirs_v[i].string);
	    i++;
	}
	VIM_CLEAR(ಹುಡುಕು_ctx->ffsc_stopdirs_v);
    }

    // reset everything
    VIM_CLEAR_STRING(ಹುಡುಕು_ctx->ffsc_file_ಗಮ್ಯ_ಹುಡುಕು);
    VIM_CLEAR_STRING(ಹುಡುಕು_ctx->ffsc_ಆರಂಭ_dir);
    VIM_CLEAR_STRING(ಹುಡುಕು_ctx->ffsc_fix_ಮಾರ್ಗ);
    VIM_CLEAR_STRING(ಹುಡುಕು_ctx->ffsc_wc_ಮಾರ್ಗ);
    ಹುಡುಕು_ctx->ffsc_level = 0;
}

/*
 * check if the given path is in the stopdirs
 * returns TRUE if yes else FALSE
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ff_ಮಾರ್ಗ_ಒಳ_stoplist(char_u *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_len, string_T *stopdirs_v)
{
    ಪೂರ್ಣಾಂಕ		i = 0;

    // eat up trailing path separators, except the first
    ಆಗ (ಮಾರ್ಗ_len > 1 && vim_ispathsep(ಮಾರ್ಗ[ಮಾರ್ಗ_len - 1]))
	ಮಾರ್ಗ_len--;

    // if no path consider it as match
    ಆದರೆ (ಮಾರ್ಗ_len == 0)
	ಹಿಂತಿರುಗಿ TRUE;

    ಗಾಗಿ (i = 0; stopdirs_v[i].string != ಖಾಲಿ; i++)
	// match for parent directory. So '/home' also matches
	// '/home/rks'. Check for PATHSEP in stopdirs_v[i], else
	// '/home/r' would also match '/home/rks'
	ಆದರೆ (fnamencmp(stopdirs_v[i].string, ಮಾರ್ಗ, ಮಾರ್ಗ_len) == 0
		&& ((ಪೂರ್ಣಾಂಕ)stopdirs_v[i].ಉದ್ದಳತೆ <= ಮಾರ್ಗ_len
		    || vim_ispathsep(stopdirs_v[i].string[ಮಾರ್ಗ_len])))
	    ಹಿಂತಿರುಗಿ TRUE;

    ಹಿಂತಿರುಗಿ FALSE;
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
ಕಂಡುಹಿಡಿ_file_ಒಳ_ಮಾರ್ಗ(
    char_u	*ptr,		// file name
    ಪೂರ್ಣಾಂಕ		len,		// length of file name
    ಪೂರ್ಣಾಂಕ		options,
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ,		// use count'th matching file name
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ,	// in/out: modified copy of file name
    ಅಕ್ಷರ	**ಹುಡುಕು_ctx)	// in/out: state of the search
{
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_file_ಒಳ_ಮಾರ್ಗ_option(ptr, len, options, ಮೊದಲ,
	    *curbuf->b_p_ಮಾರ್ಗ == NUL ? p_ಮಾರ್ಗ : curbuf->b_p_ಮಾರ್ಗ,
	    FINDFILE_BOTH, rel_fname, curbuf->b_p_sua,
	    file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ, ಹುಡುಕು_ctx);
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_findfile(ಶೂನ್ಯ)
{
    VIM_CLEAR_STRING(ff_expand_ಬಫರ್);
}
#ಆದರೆ_ಅಂತ್ಯ

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
ಕಂಡುಹಿಡಿ_directory_ಒಳ_ಮಾರ್ಗ(
    char_u	*ptr,		// file name
    ಪೂರ್ಣಾಂಕ		len,		// length of file name
    ಪೂರ್ಣಾಂಕ		options,
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ,	// in/out: modified copy of file name
    ಅಕ್ಷರ	**ಹುಡುಕು_ctx)	// in/out: state of the search
{
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_file_ಒಳ_ಮಾರ್ಗ_option(ptr, len, options, TRUE, p_cdpath,
				       FINDFILE_DIR, rel_fname, (char_u *)"",
				       file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ, ಹುಡುಕು_ctx);
}

    char_u *
ಕಂಡುಹಿಡಿ_file_ಒಳ_ಮಾರ್ಗ_option(
    char_u	*ptr,		// file name
    ಪೂರ್ಣಾಂಕ		len,		// length of file name
    ಪೂರ್ಣಾಂಕ		options,
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ,		// use count'th matching file name
    char_u	*ಮಾರ್ಗ_option,	// p_path or p_cdpath
    ಪೂರ್ಣಾಂಕ		ಕಂಡುಹಿಡಿ_what,	// FINDFILE_FILE, _DIR or _BOTH
    char_u	*rel_fname,	// file name we are looking relative to.
    char_u	*suffixes,	// list of suffixes, 'suffixesadd' option
    char_u	**file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ,	// in/out: modified copy of file name
    ಅಕ್ಷರ	**ಹುಡುಕು_ctx_arg) // in/out: state of the search
{
    ff_ಹುಡುಕು_ctx_T	**ಹುಡುಕು_ctx = (ff_ಹುಡುಕು_ctx_T **)ಹುಡುಕು_ctx_arg;
    ಸ್ಥಿರ char_u	*dir;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		did_findfile_init = FALSE;
    char_u		*file_name = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			rel_ಗಮ್ಯ_curdir;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
    ರಚನೆ Process	*proc = (ರಚನೆ Process *)ಕಂಡುಹಿಡಿ_Task(0L);
    APTR		save_winptr = proc->pr_ಕಿಟಕಿ_Ptr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_ಕಿಟಕಿ_Ptr = (APTR)-1L;
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿರ size_t	file_ಗಮ್ಯ_findlen = 0;

    ಆದರೆ (ಮೊದಲ == TRUE)
    {
	char_u	save_char;

	ಆದರೆ (len == 0)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	// copy file name into NameBuff, expanding environment variables
	save_char = ptr[len];
	ptr[len] = NUL;
	file_ಗಮ್ಯ_findlen = expand_env_esc(ptr, NameBuff, MAXPATHL, ಖಾಲಿ, TRUE, ಖಾಲಿ);
	ptr[len] = save_char;

	vim_free(*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ);
	*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ = vim_strnsave(NameBuff, file_ಗಮ್ಯ_findlen);
	ಆದರೆ (*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ == ಖಾಲಿ)	// out of memory
	{
	    file_name = ಖಾಲಿ;
	    file_ಗಮ್ಯ_findlen = 0;
	    ಹೋಗು theend;
	}
	ಆದರೆ (options & FNAME_UNESC)
	{
	    // Change all "\ " to " ".
	    ಗಾಗಿ (ptr = *file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ; *ptr != NUL; ++ptr)
		ಆದರೆ (ptr[0] == '\\' && ptr[1] == ' ')
		{
		    mch_memmove(ptr, ptr + 1,
			(size_t)((*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ + file_ಗಮ್ಯ_findlen) - (ptr + 1)) + 1);
		    --file_ಗಮ್ಯ_findlen;
		}
	}
    }

    rel_ಗಮ್ಯ_curdir = ((*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ)[0] == '.'
		    && ((*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ)[1] == NUL
			|| vim_ispathsep((*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ)[1])
			|| ((*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ)[1] == '.'
			    && ((*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ)[2] == NUL
				|| vim_ispathsep((*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ)[2])))));
    ಆದರೆ (vim_isAbsName(*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ)
	    // "..", "../path", "." and "./path": don't use the path_option
	    || rel_ಗಮ್ಯ_curdir
#ಆದರೆ defined(MSWIN)
	    // handle "\tmp" as absolute path
	    || vim_ispathsep((*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ)[0])
	    // handle "c:name" as absolute path
	    || ((*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ)[0] != NUL && (*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ)[1] == ':')
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
	    // handle ":tmp" as absolute path
	    || (*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ)[0] == ':'
#ಆದರೆ_ಅಂತ್ಯ
       )
    {
	/*
	 * Absolute path, no need to use "path_option".
	 * If this is not a first call, return NULL.  We already returned a
	 * filename on the first call.
	 */
	ಆದರೆ (ಮೊದಲ == TRUE)
	{
	    ಪೂರ್ಣಾಂಕ		l;
	    ಪೂರ್ಣಾಂಕ		ಸತತ;
	    size_t	rel_fnamelen = 0;
	    char_u	*suffix;

	    ಆದರೆ (ಮಾರ್ಗ_with_url(*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ))
	    {
		file_name = vim_strnsave(*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ, file_ಗಮ್ಯ_findlen);
		ಹೋಗು theend;
	    }

	    ಆದರೆ (rel_fname != ಖಾಲಿ)
		rel_fnamelen = STRLEN(rel_fname);

	    // When FNAME_REL flag given first use the directory of the file.
	    // Otherwise or when this fails use the current directory.
	    ಗಾಗಿ (ಸತತ = 1; ಸತತ <= 2; ++ಸತತ)
	    {
		l = (ಪೂರ್ಣಾಂಕ)file_ಗಮ್ಯ_findlen;
		ಆದರೆ (ಸತತ == 1
			&& rel_ಗಮ್ಯ_curdir
			&& (options & FNAME_REL)
			&& rel_fname != ಖಾಲಿ
			&& rel_fnamelen + l < MAXPATHL)
		{
		    l = vim_snprintf(
			(ಅಕ್ಷರ *)NameBuff,
			MAXPATHL,
			"%.*s%s",
			(ಪೂರ್ಣಾಂಕ)(gettail(rel_fname) - rel_fname),
			rel_fname,
			*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    STRCPY(NameBuff, *file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ);
		    ಸತತ = 2;
		}

		// When the file doesn't exist, try adding parts of
		// 'suffixesadd'.
		suffix = suffixes;
		ಗಾಗಿ (;;)
		{
		    ಆದರೆ (mch_getperm(NameBuff) >= 0
			     && (ಕಂಡುಹಿಡಿ_what == FINDFILE_BOTH
				 || ((ಕಂಡುಹಿಡಿ_what == FINDFILE_DIR)
						    == mch_isdir(NameBuff))))
		    {
			file_name = vim_strnsave(NameBuff,
						  simplify_filename(NameBuff));
			ಹೋಗು theend;
		    }
		    ಆದರೆ (*suffix == NUL)
			ನಿಲ್ಲಿಸು;
		    copy_option_part(&suffix, NameBuff + l, MAXPATHL - l, ",");
		}
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * Loop over all paths in the 'path' or 'cdpath' option.
	 * When "first" is set, first setup to the start of the option.
	 * Otherwise continue to find the next match.
	 */
	ಆದರೆ (ಮೊದಲ == TRUE)
	{
	    // vim_findfile_free_visited can handle a possible NULL pointer
	    vim_findfile_free_ಭೇಟಿಯಾದ(*ಹುಡುಕು_ctx);
	    dir = ಮಾರ್ಗ_option;
	    did_findfile_init = FALSE;
	}

	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (did_findfile_init)
	    {
		file_name = vim_findfile(*ಹುಡುಕು_ctx);
		ಆದರೆ (file_name != ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;

		did_findfile_init = FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		char_u	*buf;
		char_u  *r_ptr;

		ಆದರೆ (dir == ಖಾಲಿ || *dir == NUL)
		{
		    // We searched all paths of the option, now we can
		    // free the search context.
		    vim_findfile_cleanup(*ಹುಡುಕು_ctx);
		    *ಹುಡುಕು_ctx = ಖಾಲಿ;
		    ನಿಲ್ಲಿಸು;
		}

		ಆದರೆ ((buf = alloc(MAXPATHL)) == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;

		// copy next path
		buf[0] = NUL;
		copy_option_part(&dir, buf, MAXPATHL, " ,");

		// get the stopdir string
		r_ptr = vim_findfile_stopdir(buf);
		*ಹುಡುಕು_ctx = vim_findfile_init(buf, *file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ, file_ಗಮ್ಯ_findlen,
					    r_ptr, 100, FALSE, ಕಂಡುಹಿಡಿ_what,
					   *ಹುಡುಕು_ctx, FALSE, rel_fname);
		ಆದರೆ (*ಹುಡುಕು_ctx != ಖಾಲಿ)
		    did_findfile_init = TRUE;
		vim_free(buf);
	    }
	}
    }
    ಆದರೆ (file_name == ಖಾಲಿ && (options & FNAME_MESS))
    {
	ಆದರೆ (ಮೊದಲ == TRUE)
	{
	    ಆದರೆ (ಕಂಡುಹಿಡಿ_what == FINDFILE_DIR)
		semsg(_(e_cant_ಕಂಡುಹಿಡಿ_directory_str_ಒಳ_cdpath), *file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_cant_ಕಂಡುಹಿಡಿ_file_str_ಒಳ_ಮಾರ್ಗ), *file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (ಕಂಡುಹಿಡಿ_what == FINDFILE_DIR)
		semsg(_(e_no_ಇನ್ನಷ್ಟು_directory_str_ಸಿಕ್ಕಿತು_ಒಳ_cdpath),
								*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_no_ಇನ್ನಷ್ಟು_file_str_ಸಿಕ್ಕಿತು_ಒಳ_ಮಾರ್ಗ), *file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ);
	}
    }

theend:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
    proc->pr_ಕಿಟಕಿ_Ptr = save_winptr;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ file_name;
}

/*
 * Get the file name at the cursor.
 * If Visual mode is active, use the selected text if it's in one line.
 * Returns the name in allocated memory, NULL for failure.
 */
    char_u *
grab_file_name(ದೀರ್ಘ ಎಣಿಕೆ_2, linenr_T *file_lnum)
{
    ಪೂರ್ಣಾಂಕ options = FNAME_MESS|FNAME_EXP|FNAME_REL|FNAME_UNESC;

    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	ಪೂರ್ಣಾಂಕ	len;
	char_u	*ptr;

	ಆದರೆ (get_visual_ಪಠ್ಯ(ಖಾಲಿ, &ptr, &len) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	// Only recognize ":123" here
	ಆದರೆ (file_lnum != ಖಾಲಿ && ptr[len] == ':' && ಸುರಕ್ಷಿತ_isdigit(ptr[len + 1]))
	{
	    char_u *p = ptr + len + 1;

	    *file_lnum = getdigits(&p);
	}
	ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_file_name_ಒಳ_ಮಾರ್ಗ(ptr, len, options,
						     ಎಣಿಕೆ_2, curbuf->b_ffname);
    }
    ಹಿಂತಿರುಗಿ file_name_at_cursor(options | FNAME_HYP, ಎಣಿಕೆ_2, file_lnum);
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
file_name_at_cursor(ಪೂರ್ಣಾಂಕ options, ದೀರ್ಘ ಎಣಿಕೆ_2, linenr_T *file_lnum)
{
    ಹಿಂತಿರುಗಿ file_name_ಒಳ_ಸಾಲು(ml_get_curline(),
		      curwin->w_cursor.ಲಂಬಸಾಲು, options, ಎಣಿಕೆ_2, curbuf->b_ffname,
		      file_lnum);
}

/*
 * Return the name of the file under or after ptr[col].
 * Otherwise like file_name_at_cursor().
 */
    char_u *
file_name_ಒಳ_ಸಾಲು(
    char_u	*ಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ		options,
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    char_u	*rel_fname,	// file we are searching relative to
    linenr_T	*file_lnum)	// line number after the file name
{
    char_u	*ptr;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		ಒಳ_type = TRUE;
    ಪೂರ್ಣಾಂಕ		is_url = FALSE;

    /*
     * search forward for what could be the start of a file name
     */
    ptr = ಸಾಲು + ಲಂಬಸಾಲು;
    ಆಗ (*ptr != NUL && !vim_isfilec(*ptr))
	MB_PTR_ADV(ptr);
    ಆದರೆ (*ptr == NUL)		// nothing found
    {
	ಆದರೆ (options & FNAME_MESS)
	    emsg(_(e_no_file_name_under_cursor));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    /*
     * Search backward for first char of the file name.
     * Go one char back to ":" before "//" even when ':' is not in 'isfname'.
     */
    ಆಗ (ptr > ಸಾಲು)
    {
	ಆದರೆ (has_mbyte && (len = (*mb_ತಲೆ_off)(ಸಾಲು, ptr - 1)) > 0)
	    ptr -= len + 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_isfilec(ptr[-1])
		|| ((options & FNAME_HYP) && ಮಾರ್ಗ_is_url(ptr - 1)))
	    --ptr;
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }

    /*
     * Search forward for the last char of the file name.
     * Also allow "://" when ':' is not in 'isfname'.
     */
    len = 0;
    ಆಗ (vim_isfilec(ptr[len]) || (ptr[len] == '\\' && ptr[len + 1] == ' ')
		 || ((options & FNAME_HYP) && ಮಾರ್ಗ_is_url(ptr + len))
		 || (is_url && vim_strchr((char_u *)":?&=", ptr[len]) != ಖಾಲಿ))
    {
	// After type:// we also include :, ?, & and = as valid characters, so
	// that http://google.com:8080?q=this&that=ok works.
	ಆದರೆ ((ptr[len] >= 'A' && ptr[len] <= 'Z')
				       || (ptr[len] >= 'a' && ptr[len] <= 'z'))
	{
	    ಆದರೆ (ಒಳ_type && ಮಾರ್ಗ_is_url(ptr + len + 1))
		is_url = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಒಳ_type = FALSE;

	ಆದರೆ (ptr[len] == '\\')
	    // Skip over the "\" in "\ ".
	    ++len;
	ಆದರೆ (has_mbyte)
	    len += (*mb_ptr2len)(ptr + len);
	ಇಲ್ಲದಿದ್ದರೆ
	    ++len;
    }

    /*
     * If there is trailing punctuation, remove it.
     * But don't remove "..", could be a directory name.
     */
    ಆದರೆ (len > 2 && vim_strchr((char_u *)".,:;!", ptr[len - 1]) != ಖಾಲಿ
						       && ptr[len - 2] != '.')
	--len;

    ಆದರೆ (file_lnum != ಖಾಲಿ)
    {
	char_u	*p;
	ಅಕ್ಷರ	*ಹೊಂದಿಕೆ_ಪಠ್ಯ = " line ";		// english
	size_t	ಹೊಂದಿಕೆ_textlen = 6;

	// Get the number after the file name and a separator character.
	// Also accept " line 999" with and without the same translation as
	// used in last_set_msg().
	p = ptr + len;
	ಆದರೆ (STRNCMP(p, ಹೊಂದಿಕೆ_ಪಠ್ಯ, ಹೊಂದಿಕೆ_textlen) == 0)
	    p += ಹೊಂದಿಕೆ_textlen;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // no match with english, try localized
	    ಹೊಂದಿಕೆ_ಪಠ್ಯ = _(ಸಾಲು_ಸಂದೇಶ);
	    ಹೊಂದಿಕೆ_textlen = STRLEN(ಹೊಂದಿಕೆ_ಪಠ್ಯ);

	    ಆದರೆ (STRNCMP(p, ಹೊಂದಿಕೆ_ಪಠ್ಯ, ಹೊಂದಿಕೆ_textlen) == 0)
		p += ಹೊಂದಿಕೆ_textlen;
	    ಇಲ್ಲದಿದ್ದರೆ
		p = skipwhite(p);
	}
	ಆದರೆ (*p != NUL)
	{
	    ಆದರೆ (!ಸುರಕ್ಷಿತ_isdigit(*p))
		++p;		    // skip the separator
	    p = skipwhite(p);
	    ಆದರೆ (ಸುರಕ್ಷಿತ_isdigit(*p))
		*file_lnum = (ಪೂರ್ಣಾಂಕ)getdigits(&p);
	}
    }

    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_file_name_ಒಳ_ಮಾರ್ಗ(ptr, len, options, ಎಣಿಕೆ_2, rel_fname);
}

#ಆದರೆ defined(FEAT_ಕಂಡುಹಿಡಿ_ID) && defined(FEAT_EVAL)
    ಸ್ಥಿರ char_u *
eval_includeexpr(char_u *ptr, ಪೂರ್ಣಾಂಕ len)
{
    char_u	*res;
    sctx_T	save_sctx = current_sctx;

    ಗಣ_vim_var_string(VV_FNAME, ptr, len);
    current_sctx = curbuf->b_p_script_ctx[BV_INEX];

    res = eval_ಗಮ್ಯ_string_ಸುರಕ್ಷಿತ(curbuf->b_p_inex,
	    was_ಗಣ_insecurely(curwin, (char_u *)"includeexpr", OPT_LOCAL),
								   TRUE, TRUE);

    ಗಣ_vim_var_string(VV_FNAME, ಖಾಲಿ, 0);
    current_sctx = save_sctx;
    ಹಿಂತಿರುಗಿ res;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return the name of the file ptr[len] in 'path'.
 * Otherwise like file_name_at_cursor().
 */
    char_u *
ಕಂಡುಹಿಡಿ_file_name_ಒಳ_ಮಾರ್ಗ(
    char_u	*ptr,
    ಪೂರ್ಣಾಂಕ		len,
    ಪೂರ್ಣಾಂಕ		options,
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    char_u	*rel_fname)	// file we are searching relative to
{
    char_u	*file_name;
    ಪೂರ್ಣಾಂಕ		c;
#ಆದರೆ defined(FEAT_ಕಂಡುಹಿಡಿ_ID) && defined(FEAT_EVAL)
    char_u	*tofree = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

#ಆದರೆ defined(FEAT_ಕಂಡುಹಿಡಿ_ID) && defined(FEAT_EVAL)
    ಆದರೆ ((options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
    {
	tofree = eval_includeexpr(ptr, len);
	ಆದರೆ (tofree != ಖಾಲಿ)
	{
	    ptr = tofree;
	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(ptr);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (options & FNAME_EXP)
    {
	char_u	*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ = ಖಾಲಿ;
	ಅಕ್ಷರ	*ಹುಡುಕು_ctx = ಖಾಲಿ;

	file_name = ಕಂಡುಹಿಡಿ_file_ಒಳ_ಮಾರ್ಗ(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ, &ಹುಡುಕು_ctx);

#ಆದರೆ defined(FEAT_ಕಂಡುಹಿಡಿ_ID) && defined(FEAT_EVAL)
	/*
	 * If the file could not be found in a normal way, try applying
	 * 'includeexpr' (unless done already).
	 */
	ಆದರೆ (file_name == ಖಾಲಿ
		&& !(options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
	{
	    tofree = eval_includeexpr(ptr, len);
	    ಆದರೆ (tofree != ಖಾಲಿ)
	    {
		ptr = tofree;
		len = (ಪೂರ್ಣಾಂಕ)STRLEN(ptr);
		file_name = ಕಂಡುಹಿಡಿ_file_ಒಳ_ಮಾರ್ಗ(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ, &ಹುಡುಕು_ctx);
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (file_name == ಖಾಲಿ && (options & FNAME_MESS))
	{
	    c = ptr[len];
	    ptr[len] = NUL;
	    semsg(_(e_cant_ಕಂಡುಹಿಡಿ_file_str_ಒಳ_ಮಾರ್ಗ_2), ptr);
	    ptr[len] = c;
	}

	// Repeat finding the file "count" times.  This matters when it
	// appears several times in the path.
	ಆಗ (file_name != ಖಾಲಿ && --ಎಣಿಕೆ_2 > 0)
	{
	    vim_free(file_name);
	    file_name = ಕಂಡುಹಿಡಿ_file_ಒಳ_ಮಾರ್ಗ(ptr, len, options, FALSE, rel_fname,
						   &file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ, &ಹುಡುಕು_ctx);
	}

	vim_free(file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ);
	vim_findfile_cleanup(ಹುಡುಕು_ctx);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	file_name = vim_strnsave(ptr, len);

#ಆದರೆ defined(FEAT_ಕಂಡುಹಿಡಿ_ID) && defined(FEAT_EVAL)
    vim_free(tofree);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ file_name;
}

/*
 * Return the end of the directory name, on the first path
 * separator:
 * "/path/file", "/path/dir/", "/path//dir", "/file"
 *	 ^	       ^	     ^	      ^
 */
    ಸ್ಥಿರ char_u *
gettail_dir(char_u *fname)
{
    char_u	*dir_ಕೊನೆ = fname;
    char_u	*ಮುಂದಿನ_dir_ಕೊನೆ = fname;
    ಪೂರ್ಣಾಂಕ		look_for_sep = TRUE;
    char_u	*p;

    ಗಾಗಿ (p = fname; *p != NUL; )
    {
	ಆದರೆ (vim_ispathsep(*p))
	{
	    ಆದರೆ (look_for_sep)
	    {
		ಮುಂದಿನ_dir_ಕೊನೆ = p;
		look_for_sep = FALSE;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (!look_for_sep)
		dir_ಕೊನೆ = ಮುಂದಿನ_dir_ಕೊನೆ;
	    look_for_sep = TRUE;
	}
	MB_PTR_ADV(p);
    }
    ಹಿಂತಿರುಗಿ dir_ಕೊನೆ;
}

/*
 * return TRUE if 'c' is a path list separator.
 */
    ಪೂರ್ಣಾಂಕ
vim_ispathlistsep(ಪೂರ್ಣಾಂಕ c)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಹಿಂತಿರುಗಿ (c == ':');
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ (c == ';');	// might not be right for every system...
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Moves "*psep" back to the previous path separator in "path".
 * Returns FAIL is "*psep" ends up at the beginning of "path".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_ಹಿಂದಿನ_pathsep(char_u *ಮಾರ್ಗ, char_u **psep)
{
    // skip the current separator
    ಆದರೆ (*psep > ಮಾರ್ಗ && vim_ispathsep(**psep))
	--*psep;

    // find the previous separator
    ಆಗ (*psep > ಮಾರ್ಗ)
    {
	ಆದರೆ (vim_ispathsep(**psep))
	    ಹಿಂತಿರುಗಿ OK;
	MB_PTR_ಹಿಂಭಾಗ(ಮಾರ್ಗ, *psep);
    }

    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Returns TRUE if "maybe_unique" is unique wrt other_paths in "gap".
 * "maybe_unique" is the end portion of "((char_u **)gap->ga_data)[i]".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_unique(char_u *maybe_unique, garray_T *gap, ಪೂರ್ಣಾಂಕ i)
{
    ಪೂರ್ಣಾಂಕ	    j;
    ಪೂರ್ಣಾಂಕ	    candidate_len = (ಪೂರ್ಣಾಂಕ)STRLEN(maybe_unique);
    ಪೂರ್ಣಾಂಕ	    other_ಮಾರ್ಗ_len;
    char_u  **other_paths = (char_u **)gap->ga_ದತ್ತಾಂಶ;
    char_u  *rival;

    ಗಾಗಿ (j = 0; j < gap->ga_len; j++)
    {
	ಆದರೆ (j == i)
	    ಮುಂದುವರಿ;  // don't compare it with itself

	other_ಮಾರ್ಗ_len = (ಪೂರ್ಣಾಂಕ)STRLEN(other_paths[j]);
	ಆದರೆ (other_ಮಾರ್ಗ_len < candidate_len)
	    ಮುಂದುವರಿ;  // it's different when it's shorter

	rival = other_paths[j] + other_ಮಾರ್ಗ_len - candidate_len;
	ಆದರೆ (fnamecmp(maybe_unique, rival) == 0
		&& (rival == other_paths[j] || vim_ispathsep(*(rival - 1))))
	    ಹಿಂತಿರುಗಿ FALSE;  //ಹೊಂದಿಕೆ
    }

    ಹಿಂತಿರುಗಿ TRUE;  // no match found
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
    ಸ್ಥಿರ ಶೂನ್ಯ
expand_ಮಾರ್ಗ_option(
	char_u		*curdir,
	char_u		*ಮಾರ್ಗ_option,	// p_path or p_cdpath
	garray_T	*gap)
{
    char_u	*buf;
    size_t	buflen;
    char_u	*p;
    size_t	curdirlen = 0;

    ಆದರೆ ((buf = alloc(MAXPATHL)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆಗ (*ಮಾರ್ಗ_option != NUL)
    {
	buflen = copy_option_part(&ಮಾರ್ಗ_option, buf, MAXPATHL, " ,");

	// do not expand backticks, could have been set via a modeline
	ಆದರೆ (vim_strchr(buf, '`') != ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	ಆದರೆ (buf[0] == '.' && (buf[1] == NUL || vim_ispathsep(buf[1])))
	{
	    size_t  plen;

	    // Relative to current buffer:
	    // "/path/file" + "." -> "/path/"
	    // "/path/file"  + "./subdir" -> "/path/subdir"
	    ಆದರೆ (curbuf->b_ffname == ಖಾಲಿ)
		ಮುಂದುವರಿ;
	    p = gettail(curbuf->b_ffname);
	    plen = (size_t)(p - curbuf->b_ffname);
	    ಆದರೆ (plen + buflen >= MAXPATHL)
		ಮುಂದುವರಿ;
	    ಆದರೆ (buf[1] == NUL)
		buf[plen] = NUL;
	    ಇಲ್ಲದಿದ್ದರೆ
		mch_memmove(buf + plen, buf + 2, (buflen - 2) + 1); // +1 for NUL
	    mch_memmove(buf, curbuf->b_ffname, plen);
	    buflen = simplify_filename(buf);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf[0] == NUL)
	{
	    // relative to current directory
	    STRCPY(buf, curdir);
	    ಆದರೆ (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    buflen = curdirlen;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮಾರ್ಗ_with_url(buf))
	    // URL can't be used here
	    ಮುಂದುವರಿ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!mch_is_ತುಂಬಿದ_Name(buf))
	{
	    // Expand relative path to their full path equivalent
	    ಆದರೆ (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    ಆದರೆ (curdirlen + buflen + 3 > MAXPATHL)
		ಮುಂದುವರಿ;

	    mch_memmove(buf + curdirlen + 1, buf, buflen + 1);	    // +1 for NUL
	    STRCPY(buf, curdir);
	    buf[curdirlen] = PATHSEP;
	    buflen = simplify_filename(buf);
	}

	ಆದರೆ (ga_grow(gap, 1) == FAIL)
	    ನಿಲ್ಲಿಸು;

#ಆದರೆ defined(MSWIN)
	// Avoid the path ending in a backslash, it fails when a comma is
	// appended.
	ಆದರೆ (buf[buflen - 1] == '\\')
	    buf[buflen - 1] = '/';
#ಆದರೆ_ಅಂತ್ಯ

	p = vim_strnsave(buf, buflen);
	ಆದರೆ (p == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	((char_u **)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len++] = p;
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
    ಸ್ಥಿರ char_u *
get_ಮಾರ್ಗ_cutoff(char_u *fname, garray_T *gap)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    maxlen = 0;
    char_u  **ಮಾರ್ಗ_part = (char_u **)gap->ga_ದತ್ತಾಂಶ;
    char_u  *cutoff = ಖಾಲಿ;

    ಗಾಗಿ (i = 0; i < gap->ga_len; i++)
    {
	ಪೂರ್ಣಾಂಕ j = 0;

	ಆಗ ((fname[j] == ಮಾರ್ಗ_part[i][j]
#ಆದರೆ defined(MSWIN)
		|| (vim_ispathsep(fname[j]) && vim_ispathsep(ಮಾರ್ಗ_part[i][j]))
#ಆದರೆ_ಅಂತ್ಯ
			     ) && fname[j] != NUL && ಮಾರ್ಗ_part[i][j] != NUL)
	    j++;
	ಆದರೆ (j > maxlen)
	{
	    maxlen = j;
	    cutoff = &fname[j];
	}
    }

    // skip to the file or directory name
    ಆದರೆ (cutoff != ಖಾಲಿ)
	ಆಗ (vim_ispathsep(*cutoff))
	    MB_PTR_ADV(cutoff);

    ಹಿಂತಿರುಗಿ cutoff;
}

/*
 * Sorts, removes duplicates and modifies all the fullpath names in "gap" so
 * that they are unique with respect to each other while conserving the part
 * that matches the pattern. Beware, this is at least O(n^2) wrt "gap->ga_len".
 */
    ಶೂನ್ಯ
uniquefy_paths(
	garray_T *gap,
	char_u *ನಮೂನೆ,
	char_u *ಮಾರ್ಗ_option)	// p_path or p_cdpath
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		len;
    char_u	**fnames = (char_u **)gap->ga_ದತ್ತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		ವಿಂಗಡಿಸು_again = FALSE;
    char_u	*pat;
    char_u      *file_ನಮೂನೆ;
    char_u	*curdir;
    regmatch_T	regmatch;
    garray_T	ಮಾರ್ಗ_ga;
    char_u	**ಒಳ_curdir = ಖಾಲಿ;
    char_u	*short_name;

    remove_duplicates(gap);
    ga_init2(&ಮಾರ್ಗ_ga, ಗಾತ್ರ(char_u *), 1);

    /*
     * We need to prepend a '*' at the beginning of file_pattern so that the
     * regex matches anywhere in the path. FIXME: is this valid for all
     * possible patterns?
     */
    len = (ಪೂರ್ಣಾಂಕ)STRLEN(ನಮೂನೆ);
    file_ನಮೂನೆ = alloc(len + 2);
    ಆದರೆ (file_ನಮೂನೆ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    file_ನಮೂನೆ[0] = '*';
    STRCPY(file_ನಮೂನೆ + 1, ನಮೂನೆ);
    pat = file_pat_ಗಮ್ಯ_reg_pat(file_ನಮೂನೆ, ಖಾಲಿ, ಖಾಲಿ, FALSE);
    vim_free(file_ನಮೂನೆ);
    ಆದರೆ (pat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    regmatch.rm_ic = TRUE;		// always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    vim_free(pat);
    ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((curdir = alloc(MAXPATHL)) == ಖಾಲಿ)
	ಹೋಗು theend;
    mch_dirname(curdir, MAXPATHL);
    expand_ಮಾರ್ಗ_option(curdir, ಮಾರ್ಗ_option, &ಮಾರ್ಗ_ga);

    ಒಳ_curdir = ALLOC_CLEAR_MULT(char_u *, gap->ga_len);
    ಆದರೆ (ಒಳ_curdir == ಖಾಲಿ)
	ಹೋಗು theend;

    ಗಾಗಿ (i = 0; i < gap->ga_len && !got_int; i++)
    {
	char_u	    *ಮಾರ್ಗ = fnames[i];
	ಪೂರ್ಣಾಂಕ	    is_ಒಳ_curdir;
	char_u	    *dir_ಕೊನೆ = gettail_dir(ಮಾರ್ಗ);
	char_u	    *ಮಾರ್ಗ_cutoff;

	len = (ಪೂರ್ಣಾಂಕ)STRLEN(ಮಾರ್ಗ);
	is_ಒಳ_curdir = fnamencmp(curdir, ಮಾರ್ಗ, dir_ಕೊನೆ - ಮಾರ್ಗ) == 0
					     && curdir[dir_ಕೊನೆ - ಮಾರ್ಗ] == NUL;
	ಆದರೆ (is_ಒಳ_curdir)
	    ಒಳ_curdir[i] = vim_strnsave(ಮಾರ್ಗ, len);

	// Shorten the filename while maintaining its uniqueness
	ಮಾರ್ಗ_cutoff = get_ಮಾರ್ಗ_cutoff(ಮಾರ್ಗ, &ಮಾರ್ಗ_ga);

	// Don't assume all files can be reached without path when search
	// pattern starts with star star slash, so only remove path_cutoff
	// when possible.
	ಆದರೆ (ನಮೂನೆ[0] == '*' && ನಮೂನೆ[1] == '*'
		&& vim_ispathsep_nocolon(ನಮೂನೆ[2])
		&& ಮಾರ್ಗ_cutoff != ಖಾಲಿ
		&& vim_regexec(&regmatch, ಮಾರ್ಗ_cutoff, (colnr_T)0)
		&& is_unique(ಮಾರ್ಗ_cutoff, gap, i))
	{
	    ವಿಂಗಡಿಸು_again = TRUE;
	    mch_memmove(ಮಾರ್ಗ, ಮಾರ್ಗ_cutoff, STRLEN(ಮಾರ್ಗ_cutoff) + 1);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Here all files can be reached without path, so get shortest
	    // unique path.  We start at the end of the path.
	    char_u  *pathsep_p = ಮಾರ್ಗ + len - 1;

	    ಆಗ (ಕಂಡುಹಿಡಿ_ಹಿಂದಿನ_pathsep(ಮಾರ್ಗ, &pathsep_p))
	    {
		ಆದರೆ (vim_regexec(&regmatch, pathsep_p + 1, (colnr_T)0)
			&& is_unique(pathsep_p + 1, gap, i)
			&& ಮಾರ್ಗ_cutoff != ಖಾಲಿ && pathsep_p + 1 >= ಮಾರ್ಗ_cutoff)
		{
		    ವಿಂಗಡಿಸು_again = TRUE;
		    mch_memmove(ಮಾರ್ಗ, pathsep_p + 1,
			    (size_t)((ಮಾರ್ಗ + len) - (pathsep_p + 1)) + 1);  // +1 for NUL
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}

	ಆದರೆ (mch_is_ತುಂಬಿದ_Name(ಮಾರ್ಗ))
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
	    short_name = shorten_fname(ಮಾರ್ಗ, curdir);
	    ಆದರೆ (short_name != ಖಾಲಿ && short_name > ಮಾರ್ಗ + 1
#ಆದರೆ defined(MSWIN)
		    // On windows,
		    //	    shorten_fname("c:\a\a.txt", "c:\a\b")
		    // returns "\a\a.txt", which is not really the short
		    // name, hence:
		    && !vim_ispathsep(*short_name)
#ಆದರೆ_ಅಂತ್ಯ
		)
	    {
		vim_snprintf((ಅಕ್ಷರ *)ಮಾರ್ಗ, MAXPATHL, ".%s%s", PATHSEPSTR, short_name);
	    }
	}
	ui_breakcheck();
    }

    // Shorten filenames in /in/current/directory/{filename}
    ಗಾಗಿ (i = 0; i < gap->ga_len && !got_int; i++)
    {
	size_t	rel_pathsize;
	char_u	*rel_ಮಾರ್ಗ;
	char_u	*ಮಾರ್ಗ = ಒಳ_curdir[i];

	ಆದರೆ (ಮಾರ್ಗ == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	// If the {filename} is not unique, change it to ./{filename}.
	// Else reduce it to {filename}
	short_name = shorten_fname(ಮಾರ್ಗ, curdir);
	ಆದರೆ (short_name == ಖಾಲಿ)
	    short_name = ಮಾರ್ಗ;
	ಆದರೆ (is_unique(short_name, gap, i))
	{
	    STRCPY(fnames[i], short_name);
	    ಮುಂದುವರಿ;
	}

	rel_pathsize = 1 + ಗಾತ್ರ(PATHSEP) + STRLEN(short_name) + 1;
	rel_ಮಾರ್ಗ = alloc(rel_pathsize);
	ಆದರೆ (rel_ಮಾರ್ಗ == ಖಾಲಿ)
	    ಹೋಗು theend;

	vim_snprintf((ಅಕ್ಷರ *)rel_ಮಾರ್ಗ, rel_pathsize, ".%s%s", PATHSEPSTR, short_name);

	vim_free(fnames[i]);
	fnames[i] = rel_ಮಾರ್ಗ;
	ವಿಂಗಡಿಸು_again = TRUE;
	ui_breakcheck();
    }

theend:
    vim_free(curdir);
    ಆದರೆ (ಒಳ_curdir != ಖಾಲಿ)
    {
	ಗಾಗಿ (i = 0; i < gap->ga_len; i++)
	    vim_free(ಒಳ_curdir[i]);
	vim_free(ಒಳ_curdir);
    }
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ಮಾರ್ಗ_ga);
    vim_regfree(regmatch.regprog);

    ಆದರೆ (ವಿಂಗಡಿಸು_again)
	remove_duplicates(gap);
}

/*
 * Calls globpath() with 'path' values for the given pattern and stores the
 * result in "gap".
 * Returns the total number of matches.
 */
    ಪೂರ್ಣಾಂಕ
expand_ಒಳ_ಮಾರ್ಗ(
    garray_T	*gap,
    char_u	*ನಮೂನೆ,
    ಪೂರ್ಣಾಂಕ		flags)		// EW_* flags
{
    char_u	*curdir;
    garray_T	ಮಾರ್ಗ_ga;
    char_u	*paths = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		glob_flags = 0;
    char_u	*ಮಾರ್ಗ_option = *curbuf->b_p_ಮಾರ್ಗ == NUL ? p_ಮಾರ್ಗ : curbuf->b_p_ಮಾರ್ಗ;

    ಆದರೆ ((curdir = alloc(MAXPATHL)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;
    mch_dirname(curdir, MAXPATHL);

    ga_init2(&ಮಾರ್ಗ_ga, ಗಾತ್ರ(char_u *), 1);
    ಆದರೆ (flags & EW_CDPATH)
	expand_ಮಾರ್ಗ_option(curdir, p_cdpath, &ಮಾರ್ಗ_ga);
    ಇಲ್ಲದಿದ್ದರೆ
	expand_ಮಾರ್ಗ_option(curdir, ಮಾರ್ಗ_option, &ಮಾರ್ಗ_ga);
    vim_free(curdir);
    ಆದರೆ (ಮಾರ್ಗ_ga.ga_len == 0)
	ಹಿಂತಿರುಗಿ 0;

    paths = ga_concat_ಅಕ್ಷರಸರಣಿಗಳು(&ಮಾರ್ಗ_ga, ",");
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ಮಾರ್ಗ_ga);
    ಆದರೆ (paths == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (flags & EW_ICASE)
	glob_flags |= WILD_ICASE;
    ಆದರೆ (flags & EW_ADDSLASH)
	glob_flags |= WILD_ಕೂಡಿಸು_SLASH;
    globpath(paths, ನಮೂನೆ, gap, glob_flags, !!(flags & EW_CDPATH));
    vim_free(paths);

    ಹಿಂತಿರುಗಿ gap->ga_len;
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
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ AMIGA	    // Amiga doesn't have "..", it uses "/"
    ಪೂರ್ಣಾಂಕ		components = 0;
    char_u	*p, *ಬಾಲ, *ಆರಂಭ;
    char_u	*p_ಕೊನೆ;			    // point to NUL at end of string "p"
    ಪೂರ್ಣಾಂಕ		stripping_disabled = FALSE;
    ಪೂರ್ಣಾಂಕ		relative = TRUE;

    p = filename;
# ifdef BACKSLASH_ಒಳ_FILENAME
    ಆದರೆ (p[0] != NUL && p[1] == ':')	    // skip "x:"
	p += 2;
# endif

    ಆದರೆ (vim_ispathsep(*p))
    {
	relative = FALSE;
	ಮಾಡು
	    ++p;
	ಆಗ (vim_ispathsep(*p));
    }
    ಆರಂಭ = p;	    // remember start after "c:/" or "/" or "///"
    p_ಕೊನೆ = p + STRLEN(p);
# ifdef UNIX
    // Posix says that "//path" is unchanged but "///path" is "/path".
    ಆದರೆ (ಆರಂಭ > filename + 2)
    {
	mch_memmove(filename + 1, p, (size_t)(p_ಕೊನೆ - p) + 1);	    // +1 for NUL
	p_ಕೊನೆ -= (size_t)(p - (filename + 1));
	ಆರಂಭ = p = filename + 1;
    }
# endif

    ಮಾಡು
    {
	// At this point "p" is pointing to the char following a single "/"
	// or "p" is at the "start" of the (absolute or relative) path name.
# ifdef VMS
	// VMS allows device:[path] - don't strip the [ in directory
	ಆದರೆ ((*p == '[' || *p == '<') && p > filename && p[-1] == ':')
	{
	    // :[ or :< composition: vms directory component
	    ++components;
	    p = getnextcomp(p + 1);
	}
	// allow remote calls as host"user passwd"::device:[path]
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == ':' && p[1] == ':' && p > filename && p[-1] == '"' )
	{
	    // ":: composition: vms host/passwd component
	    ++components;
	    p = getnextcomp(p + 2);
	}
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	ಆದರೆ (vim_ispathsep(*p))
	{
	    mch_memmove(p, p + 1, (size_t)(p_ಕೊನೆ - (p + 1)) + 1); // remove duplicate "/"
	    --p_ಕೊನೆ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == '.' && (vim_ispathsep(p[1]) || p[1] == NUL))
	{
	    ಆದರೆ (p == ಆರಂಭ && relative)
		p += 1 + (p[1] != NUL);	// keep single "." or leading "./"
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Strip "./" or ".///".  If we are at the end of the file name
		// and there is no trailing path separator, either strip "/." if
		// we are after "start", or strip "." if we are at the beginning
		// of an absolute path name .
		ಬಾಲ = p + 1;
		ಆದರೆ (p[1] != NUL)
		    ಆಗ (vim_ispathsep(*ಬಾಲ))
			MB_PTR_ADV(ಬಾಲ);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p > ಆರಂಭ)
		    --p;		// strip preceding path separator

		mch_memmove(p, ಬಾಲ, (size_t)(p_ಕೊನೆ - ಬಾಲ) + 1);
		p_ಕೊನೆ -= (size_t)(ಬಾಲ - p);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == '.' && p[1] == '.' &&
	    (vim_ispathsep(p[2]) || p[2] == NUL))
	{
	    // Skip to after ".." or "../" or "..///".
	    ಬಾಲ = p + 2;
	    ಆಗ (vim_ispathsep(*ಬಾಲ))
		MB_PTR_ADV(ಬಾಲ);

	    ಆದರೆ (components > 0)		// strip one preceding component
	    {
		ಪೂರ್ಣಾಂಕ		do_strip = FALSE;
		char_u		saved_char;
		stat_T		st;

		// Don't strip for an erroneous file name.
		ಆದರೆ (!stripping_disabled)
		{
		    // If the preceding component does not exist in the file
		    // system, we strip it.  On Unix, we don't accept a symbolic
		    // link that refers to a non-existent file.
		    saved_char = p[-1];
		    p[-1] = NUL;
# ifdef UNIX
		    ಆದರೆ (mch_lstat((ಅಕ್ಷರ *)filename, &st) < 0)
# ಇಲ್ಲದಿದ್ದರೆ
			ಆದರೆ (mch_stat((ಅಕ್ಷರ *)filename, &st) < 0)
# endif
			    do_strip = TRUE;
		    p[-1] = saved_char;

		    --p;
		    // Skip back to after previous '/'.
		    ಆಗ (p > ಆರಂಭ && !after_pathsep(ಆರಂಭ, p))
			MB_PTR_ಹಿಂಭಾಗ(ಆರಂಭ, p);

		    ಆದರೆ (!do_strip)
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
			saved_char = *ಬಾಲ;
			*ಬಾಲ = NUL;
			ಆದರೆ (mch_stat((ಅಕ್ಷರ *)filename, &st) >= 0)
			    do_strip = TRUE;
			ಇಲ್ಲದಿದ್ದರೆ
			    stripping_disabled = TRUE;
			*ಬಾಲ = saved_char;
# ifdef UNIX
			ಆದರೆ (do_strip)
			{
			    stat_T	new_st;

			    // On Unix, the check for the unstripped file name
			    // above works also for a symbolic link pointing to
			    // a searchable directory.  But then the parent of
			    // the directory pointed to by the link must be the
			    // same as the stripped file name.  (The latter
			    // exists in the file system since it is the
			    // component's parent directory.)
			    ಆದರೆ (p == ಆರಂಭ && relative)
				(ಶೂನ್ಯ)mch_stat(".", &new_st);
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				saved_char = *p;
				*p = NUL;
				(ಶೂನ್ಯ)mch_stat((ಅಕ್ಷರ *)filename, &new_st);
				*p = saved_char;
			    }

			    ಆದರೆ (new_st.st_ino != st.st_ino ||
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

		ಆದರೆ (!do_strip)
		{
		    // Skip the ".." or "../" and reset the counter for the
		    // components that might be stripped later on.
		    p = ಬಾಲ;
		    components = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Strip previous component.  If the result would get empty
		    // and there is no trailing path separator, leave a single
		    // "." instead.  If we are at the end of the file name and
		    // there is no trailing path separator and a preceding
		    // component is left after stripping, strip its trailing
		    // path separator as well.
		    ಆದರೆ (p == ಆರಂಭ && relative && ಬಾಲ[-1] == '.')
		    {
			*p++ = '.';
			*p = NUL;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (p > ಆರಂಭ && ಬಾಲ[-1] == '.')
			    --p;

			mch_memmove(p, ಬಾಲ, (size_t)(p_ಕೊನೆ - ಬಾಲ) + 1);	// strip previous component
			p_ಕೊನೆ -= (size_t)(ಬಾಲ - p);
		    }

		    --components;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p == ಆರಂಭ && !relative)	// leading "/.." or "/../"
	    {
		mch_memmove(p, ಬಾಲ, (size_t)(p_ಕೊನೆ - ಬಾಲ) + 1);		// strip ".." or "../"
		p_ಕೊನೆ -= (size_t)(ಬಾಲ - p);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (p == ಆರಂಭ + 2 && p[-2] == '.')	// leading "./../"
		{
		    mch_memmove(p - 2, p, (size_t)(p_ಕೊನೆ - p) + 1); // strip leading "./"
		    p_ಕೊನೆ -= 2;
		    ಬಾಲ -= 2;
		}
		p = ಬಾಲ;		// skip to char after ".." or "../"
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ++components;		// simple path component
	    p = getnextcomp(p);
	}
    } ಆಗ (*p != NUL);

    ಹಿಂತಿರುಗಿ (size_t)(p_ಕೊನೆ - filename);
#ಇಲ್ಲದಿದ್ದರೆ
    // Don't touch Amiga filenames
    ಹಿಂತಿರುಗಿ STRLEN(filename);
#ಆದರೆ_ಅಂತ್ಯ // !AMIGA
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * "simplify()" function
 */
    ಶೂನ್ಯ
f_simplify(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    p = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_string = vim_strsave(p);
    simplify_filename(rettv->vval.v_string);	// simplify in place
    rettv->v_type = VAR_STRING;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL
