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

#अन्तर्भाव "vim.h"

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
प्रकारनिर्देश संरचना ff_थाक
{
    संरचना ff_थाक	*ffs_prev;

    // the fix part (no wildcards) and the part containing the wildcards
    // of the search path
    string_T		ffs_fix_path;
    string_T		ffs_wc_path;

    // files/dirs found in the above directory, matched by the first wildcard
    // of wc_part
    char_u		**ffs_filearray;
    पूर्णाङ्क			ffs_filearray_size;
    पूर्णाङ्क			ffs_filearray_cur;   // needed for partly handled dirs

    // to store status of partly handled directories
    // 0: we work on this directory for the first time
    // 1: this directory was partly searched in an earlier step
    पूर्णाङ्क			ffs_stage;

    // How deep are we in the directory tree?
    // Counts backward from value of level parameter to vim_findfile_init
    पूर्णाङ्क			ffs_level;

    // Did we already expand '**' to an empty string?
    पूर्णाङ्क			ffs_star_star_empty;
} ff_थाक_T;

/*
 * type for already visited directories or files.
 */
प्रकारनिर्देश संरचना ff_visited
{
    संरचना ff_visited	*ffv_next;

    // Visited directories are different if the wildcard string are
    // different. So we have to save it.
    char_u		*ffv_wc_path;

    // for unix use inode etc for comparison (needed because of links), else
    // use filename.
#ifdef UNIX
    पूर्णाङ्क			ffv_dev_valid;	// ffv_dev and ffv_ino were set
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
प्रकारनिर्देश संरचना ff_visited_list_hdr
{
    संरचना ff_visited_list_hdr	*ffvl_next;

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
प्रकारनिर्देश संरचना ff_खोज_ctx_T
{
    ff_थाक_T			*ffsc_थाक_ptr;
    ff_visited_list_hdr_T	*ffsc_visited_list;
    ff_visited_list_hdr_T	*ffsc_dir_visited_list;
    ff_visited_list_hdr_T	*ffsc_visited_lists_list;
    ff_visited_list_hdr_T	*ffsc_dir_visited_lists_list;
    string_T			ffsc_file_to_खोज;
    string_T			ffsc_start_dir;
    string_T			ffsc_fix_path;
    string_T			ffsc_wc_path;
    पूर्णाङ्क				ffsc_level;
    string_T			*ffsc_stopdirs_v;
    पूर्णाङ्क				ffsc_find_what;
    पूर्णाङ्क				ffsc_tagfile;
} ff_खोज_ctx_T;

// locally needed functions
स्थिर पूर्णाङ्क ff_check_visited(ff_visited_T **, char_u *, माप_प्रकार, char_u *, माप_प्रकार);
स्थिर शून्य vim_findfile_free_visited(शून्य *खोज_ctx_arg);
स्थिर शून्य vim_findfile_free_visited_list(ff_visited_list_hdr_T **list_headp);
स्थिर शून्य ff_free_visited_list(ff_visited_T *vl);
स्थिर ff_visited_list_hdr_T* ff_get_visited_list(char_u *, माप_प्रकार, ff_visited_list_hdr_T **list_headp);

स्थिर शून्य ff_थप(ff_खोज_ctx_T *खोज_ctx, ff_थाक_T *थाक_ptr);
स्थिर ff_थाक_T *ff_झिक(ff_खोज_ctx_T *खोज_ctx);
स्थिर शून्य ff_clear(ff_खोज_ctx_T *खोज_ctx);
स्थिर शून्य ff_free_थाक_element(ff_थाक_T *थाक_ptr);
स्थिर ff_थाक_T *ff_create_थाक_element(char_u *, माप_प्रकार, char_u *, माप_प्रकार, पूर्णाङ्क, पूर्णाङ्क);
स्थिर पूर्णाङ्क ff_path_in_stoplist(char_u *, पूर्णाङ्क, string_T *);

स्थिर string_T ff_expand_buffer = {रिक्त, 0};	    // used for expanding filenames

#यदि 0
/*
 * if someone likes findfirst/findnext, here are the functions
 * NOT TESTED!!
 */

स्थिर शून्य *ff_fn_खोज_context = रिक्त;

    char_u *
vim_findfirst(char_u *path, char_u *filename, पूर्णाङ्क level)
{
    ff_fn_खोज_context =
	vim_findfile_init(path, filename, रिक्त, level, TRUE, FALSE,
		ff_fn_खोज_context, rel_fname);
    यदि (रिक्त == ff_fn_खोज_context)
	फर्काऊ रिक्त;
    अन्यथा
	फर्काऊ vim_findnext()
}

    char_u *
vim_findnext(शून्य)
{
    char_u *ret = vim_findfile(ff_fn_खोज_context);

    यदि (रिक्त == ret)
    {
	vim_findfile_cleanup(ff_fn_खोज_context);
	ff_fn_खोज_context = रिक्त;
    }
    फर्काऊ ret;
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
    शून्य *
vim_findfile_init(
    char_u	*path,
    char_u	*filename,
    माप_प्रकार	filenamelen,
    char_u	*stopdirs UNUSED,
    पूर्णाङ्क		level,
    पूर्णाङ्क		free_visited,
    पूर्णाङ्क		find_what,
    शून्य	*खोज_ctx_arg,
    पूर्णाङ्क		tagfile,	// expanding names of tags files
    char_u	*rel_fname)	// file name to use for "."
{
    char_u		*wc_part;
    ff_थाक_T		*sptr;
    ff_खोज_ctx_T	*खोज_ctx;
    पूर्णाङ्क			add_sep;

    // If a search context is given by the caller, reuse it, else allocate a
    // new one.
    यदि (खोज_ctx_arg != रिक्त)
	खोज_ctx = खोज_ctx_arg;
    अन्यथा
    {
	खोज_ctx = ALLOC_CLEAR_ONE(ff_खोज_ctx_T);
	यदि (खोज_ctx == रिक्त)
	    गच्छ error_return;
    }
    खोज_ctx->ffsc_find_what = find_what;
    खोज_ctx->ffsc_tagfile = tagfile;

    // clear the search context, but NOT the visited lists
    ff_clear(खोज_ctx);

    // clear visited list if wanted
    यदि (free_visited == TRUE)
	vim_findfile_free_visited(खोज_ctx);
    अन्यथा
    {
	// Reuse old visited lists. Get the visited list for the given
	// filename. If no list for the current filename exists, creates a new
	// one.
	खोज_ctx->ffsc_visited_list = ff_get_visited_list(filename,
					filenamelen, &खोज_ctx->ffsc_visited_lists_list);
	यदि (खोज_ctx->ffsc_visited_list == रिक्त)
	    गच्छ error_return;
	खोज_ctx->ffsc_dir_visited_list = ff_get_visited_list(filename,
				    filenamelen, &खोज_ctx->ffsc_dir_visited_lists_list);
	यदि (खोज_ctx->ffsc_dir_visited_list == रिक्त)
	    गच्छ error_return;
    }

    यदि (ff_expand_buffer.string == रिक्त)
    {
	ff_expand_buffer.length = 0;
	ff_expand_buffer.string = alloc(MAXPATHL);
	यदि (ff_expand_buffer.string == रिक्त)
	    गच्छ error_return;
    }

    // Store information on starting dir now if path is relative.
    // If path is absolute, we do that later.
    यदि (path[0] == '.'
	    && (vim_ispathsep(path[1]) || path[1] == NUL)
	    && (!tagfile || vim_strchr(p_cpo, CPO_DOTTAG) == रिक्त)
	    && rel_fname != रिक्त)
    {
	पूर्णाङ्क	len = (पूर्णाङ्क)(gettail(rel_fname) - rel_fname);

	यदि (!vim_isAbsName(rel_fname) && len + 1 < MAXPATHL)
	{
	    // Make the start dir an absolute path name.
	    vim_strncpy(ff_expand_buffer.string, rel_fname, len);
	    ff_expand_buffer.length = len;

	    खोज_ctx->ffsc_start_dir.string = FullName_save(ff_expand_buffer.string, FALSE);
	    यदि (खोज_ctx->ffsc_start_dir.string == रिक्त)
		गच्छ error_return;
	    खोज_ctx->ffsc_start_dir.length = STRLEN(खोज_ctx->ffsc_start_dir.string);
	}
	अन्यथा
	{
	    खोज_ctx->ffsc_start_dir.length = len;
	    खोज_ctx->ffsc_start_dir.string = vim_strnsave(rel_fname,
		खोज_ctx->ffsc_start_dir.length);
	    यदि (खोज_ctx->ffsc_start_dir.string == रिक्त)
		गच्छ error_return;
	}

	यदि (*++path != NUL)
	    ++path;
    }
    अन्यथा यदि (*path == NUL || !vim_isAbsName(path))
    {
#ifdef BACKSLASH_IN_FILENAME
	// "c:dir" needs "c:" to be expanded, otherwise use current dir
	यदि (*path != NUL && path[1] == ':')
	{
	    char_u  drive[3];

	    drive[0] = path[0];
	    drive[1] = ':';
	    drive[2] = NUL;
	    यदि (vim_FullName(drive, ff_expand_buffer.string, MAXPATHL, TRUE) == FAIL)
		गच्छ error_return;
	    path += 2;
	}
	अन्यथा
#endif
	यदि (mch_dirname(ff_expand_buffer.string, MAXPATHL) == FAIL)
	    गच्छ error_return;

	ff_expand_buffer.length = STRLEN(ff_expand_buffer.string);

	खोज_ctx->ffsc_start_dir.length = ff_expand_buffer.length;
	खोज_ctx->ffsc_start_dir.string = vim_strnsave(ff_expand_buffer.string,
	    खोज_ctx->ffsc_start_dir.length);
	यदि (खोज_ctx->ffsc_start_dir.string == रिक्त)
	    गच्छ error_return;
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
    यदि (stopdirs != रिक्त)
    {
	char_u	*walker = stopdirs;
	पूर्णाङ्क	dircount;

	जबसम्म (*walker == ';')
	    walker++;

	dircount = 1;
	खोज_ctx->ffsc_stopdirs_v = ALLOC_ONE(string_T);

	यदि (खोज_ctx->ffsc_stopdirs_v != रिक्त)
	{
	    string_T	*tmp;			// for convenience

	    गर
	    {
		char_u	*helper;
		शून्य	*ptr;
		माप_प्रकार	len;

		helper = walker;
		ptr = vim_realloc(खोज_ctx->ffsc_stopdirs_v,
					   (dircount + 1) * परिमाण(string_T));
		यदि (ptr)
		    खोज_ctx->ffsc_stopdirs_v = ptr;
		अन्यथा
		    // ignore, keep what we have and continue
		    रोक;
		walker = vim_strchr(walker, ';');
		len = walker ? (माप_प्रकार)(walker - helper) : STRLEN(helper);
		// "" means ascent till top of directory tree.

		यदि (*helper != NUL && !vim_isAbsName(helper)
							 && len + 1 < MAXPATHL)
		{
		    // Make the stop dir an absolute path name.
		    vim_strncpy(ff_expand_buffer.string, helper, len);
		    ff_expand_buffer.length = len;

		    tmp = &खोज_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->string = FullName_save(ff_expand_buffer.string, FALSE);
		    यदि (tmp->string != रिक्त)
			tmp->length = STRLEN(tmp->string);
		}
		अन्यथा
		{
		    tmp = &खोज_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->length = len;
		    tmp->string = vim_strnsave(helper, tmp->length);
		    यदि (tmp->string == रिक्त)
			tmp->length = 0;
		}
		यदि (walker)
		    walker++;
		dircount++;

	    } जबसम्म (walker != रिक्त);

	    tmp = &खोज_ctx->ffsc_stopdirs_v[dircount - 1];
	    tmp->string = रिक्त;
	    tmp->length = 0;
	}
    }

    खोज_ctx->ffsc_level = level;

    /*
     * split into:
     *  -fix path
     *  -wildcard_stuff (might be NULL)
     */
    wc_part = vim_strchr(path, '*');
    यदि (wc_part != रिक्त)
    {
	पूर्णाङ्क	llevel;
	वर्ण	*errpt;

	// save the fix part of the path
	खोज_ctx->ffsc_fix_path.length = (माप_प्रकार)(wc_part - path);
	खोज_ctx->ffsc_fix_path.string = vim_strnsave(path,
	    खोज_ctx->ffsc_fix_path.length);
	यदि (खोज_ctx->ffsc_fix_path.string == रिक्त)
	    गच्छ error_return;

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
	जबसम्म (*wc_part != NUL)
	{
	    यदि (ff_expand_buffer.length + 5 >= MAXPATHL)
	    {
		emsg(_(e_path_too_long_for_completion));
		रोक;
	    }
	    यदि (STRNCMP(wc_part, "**", 2) == 0)
	    {
		ff_expand_buffer.string[ff_expand_buffer.length++] = *wc_part++;
		ff_expand_buffer.string[ff_expand_buffer.length++] = *wc_part++;

		llevel = म_से_दीर्घ((वर्ण *)wc_part, &errpt, 10);
		यदि ((char_u *)errpt != wc_part && llevel > 0 && llevel < 255)
		    ff_expand_buffer.string[ff_expand_buffer.length++] = llevel;
		अन्यथा यदि ((char_u *)errpt != wc_part && llevel == 0)
		    // restrict is 0 -> remove already added '**'
		    ff_expand_buffer.length -= 2;
		अन्यथा
		    ff_expand_buffer.string[ff_expand_buffer.length++] = FF_MAX_STAR_STAR_EXPAND;
		wc_part = (char_u *)errpt;
		यदि (*wc_part != NUL && !vim_ispathsep(*wc_part))
		{
		    semsg(_(e_invalid_path_सङ्ख्या_must_be_at_end_of_path_or_be_followed_by_str), PATHSEPSTR);
		    गच्छ error_return;
		}
	    }
	    अन्यथा
		ff_expand_buffer.string[ff_expand_buffer.length++] = *wc_part++;
	}
	ff_expand_buffer.string[ff_expand_buffer.length] = NUL;

	खोज_ctx->ffsc_wc_path.length = ff_expand_buffer.length;
	खोज_ctx->ffsc_wc_path.string = vim_strnsave(ff_expand_buffer.string,
	    खोज_ctx->ffsc_wc_path.length);
	यदि (खोज_ctx->ffsc_wc_path.string == रिक्त)
	    गच्छ error_return;
    }
    अन्यथा
    {
	खोज_ctx->ffsc_fix_path.length = STRLEN(path);
	खोज_ctx->ffsc_fix_path.string = vim_strnsave(path,
	    खोज_ctx->ffsc_fix_path.length);
	यदि (खोज_ctx->ffsc_fix_path.string == रिक्त)
	    गच्छ error_return;
    }

    यदि (खोज_ctx->ffsc_start_dir.string == रिक्त)
    {
	// store the fix part as startdir.
	// This is needed if the parameter path is fully qualified.
	खोज_ctx->ffsc_start_dir.length = खोज_ctx->ffsc_fix_path.length;
	खोज_ctx->ffsc_start_dir.string = vim_strnsave(खोज_ctx->ffsc_fix_path.string,
	    खोज_ctx->ffsc_start_dir.length);
	यदि (खोज_ctx->ffsc_start_dir.string == रिक्त)
	    गच्छ error_return;
	खोज_ctx->ffsc_fix_path.string[0] = NUL;
	खोज_ctx->ffsc_fix_path.length = 0;
    }

    // create an absolute path
    यदि (खोज_ctx->ffsc_start_dir.length
	    + खोज_ctx->ffsc_fix_path.length + 3 >= MAXPATHL)
    {
	emsg(_(e_path_too_long_for_completion));
	गच्छ error_return;
    }

    add_sep = !after_pathsep(खोज_ctx->ffsc_start_dir.string,
	खोज_ctx->ffsc_start_dir.string + खोज_ctx->ffsc_start_dir.length);
    ff_expand_buffer.length = vim_snprintf(
	    (वर्ण *)ff_expand_buffer.string,
	    MAXPATHL,
	    "%s%s",
	    खोज_ctx->ffsc_start_dir.string,
	    add_sep ? PATHSEPSTR : "");

    {
	माप_प्रकार	bufsize = ff_expand_buffer.length + खोज_ctx->ffsc_fix_path.length + 1;
	char_u	*buf = alloc(bufsize);

	यदि (buf == रिक्त)
	    गच्छ error_return;

	vim_snprintf(
		(वर्ण *)buf,
		bufsize,
		"%s%s",
		ff_expand_buffer.string,
		खोज_ctx->ffsc_fix_path.string);
	यदि (mch_isdir(buf))
	{
	    यदि (खोज_ctx->ffsc_fix_path.length > 0)
	    {
		add_sep = !after_pathsep(खोज_ctx->ffsc_fix_path.string,
		    खोज_ctx->ffsc_fix_path.string + खोज_ctx->ffsc_fix_path.length);
		ff_expand_buffer.length += vim_snprintf(
			(वर्ण *)ff_expand_buffer.string + ff_expand_buffer.length,
			MAXPATHL - ff_expand_buffer.length,
			"%s%s",
			खोज_ctx->ffsc_fix_path.string,
			add_sep ? PATHSEPSTR : "");
	    }
	}
	अन्यथा
	{
	    char_u *p = gettail(खोज_ctx->ffsc_fix_path.string);
	    पूर्णाङ्क    len = (पूर्णाङ्क)खोज_ctx->ffsc_fix_path.length;

	    यदि (p > खोज_ctx->ffsc_fix_path.string)
	    {
		// do not add '..' to the path and start upwards searching
		len = (पूर्णाङ्क)(p - खोज_ctx->ffsc_fix_path.string) - 1;
		यदि ((len >= 2
			&& STRNCMP(खोज_ctx->ffsc_fix_path.string, "..", 2) == 0)
			&& (len == 2 || खोज_ctx->ffsc_fix_path.string[2] == PATHSEP))
		{
		    vim_free(buf);
		    गच्छ error_return;
		}

		add_sep = !after_pathsep(खोज_ctx->ffsc_fix_path.string,
		    खोज_ctx->ffsc_fix_path.string + खोज_ctx->ffsc_fix_path.length);
		ff_expand_buffer.length += vim_snprintf(
			(वर्ण *)ff_expand_buffer.string + ff_expand_buffer.length,
			MAXPATHL - ff_expand_buffer.length,
			"%.*s%s",
			len,
			खोज_ctx->ffsc_fix_path.string,
			add_sep ? PATHSEPSTR : "");
	    }

	    यदि (खोज_ctx->ffsc_wc_path.string != रिक्त)
	    {
		माप_प्रकार	tempsize = (खोज_ctx->ffsc_fix_path.length - len)
				+ खोज_ctx->ffsc_wc_path.length
				+ 1;
		char_u	*temp = alloc(tempsize);

		यदि (temp == रिक्त)
		{
		    vim_free(buf);
		    vim_free(temp);
		    गच्छ error_return;
		}

		खोज_ctx->ffsc_wc_path.length = vim_snprintf(
			    (वर्ण *)temp,
			    tempsize,
			    "%s%s",
			    खोज_ctx->ffsc_fix_path.string + len,
			    खोज_ctx->ffsc_wc_path.string);
		vim_free(खोज_ctx->ffsc_wc_path.string);
		खोज_ctx->ffsc_wc_path.string = temp;
	    }
	}
	vim_free(buf);
    }

    sptr = ff_create_थाक_element(ff_expand_buffer.string,
			ff_expand_buffer.length,
			खोज_ctx->ffsc_wc_path.string,
			खोज_ctx->ffsc_wc_path.length,
			level,
			0);

    यदि (sptr == रिक्त)
	गच्छ error_return;

    ff_थप(खोज_ctx, sptr);

    खोज_ctx->ffsc_file_to_खोज.length = filenamelen;
    खोज_ctx->ffsc_file_to_खोज.string = vim_strnsave(filename,
	खोज_ctx->ffsc_file_to_खोज.length);
    यदि (खोज_ctx->ffsc_file_to_खोज.string == रिक्त)
	गच्छ error_return;

    फर्काऊ खोज_ctx;

error_return:
    /*
     * We clear the search context now!
     * Even when the caller gave us a (perhaps valid) context we free it here,
     * as we might have already destroyed it.
     */
    vim_findfile_cleanup(खोज_ctx);
    फर्काऊ रिक्त;
}

/*
 * Get the stopdir string.  Check that ';' is not escaped.
 */
    char_u *
vim_findfile_stopdir(char_u *buf)
{
    char_u	*r_ptr = buf;
    char_u	*r_ptr_end = रिक्त;	    // points to NUL at end of string "r_ptr"

    जबसम्म (*r_ptr != NUL && *r_ptr != ';')
    {
	यदि (r_ptr[0] == '\\' && r_ptr[1] == ';')
	{
	    // Overwrite the escape char,
	    // use STRLEN(r_ptr) to move the trailing '\0'.
	    यदि (r_ptr_end == रिक्त)
		r_ptr_end = r_ptr + STRLEN(r_ptr);
	    mch_memmove(r_ptr, r_ptr + 1,
		(माप_प्रकार)(r_ptr_end - (r_ptr + 1)) + 1);	// +1 for NUL
	    r_ptr++;
	    --r_ptr_end;
	}
	r_ptr++;
    }
    यदि (*r_ptr == ';')
    {
	*r_ptr = NUL;
	r_ptr++;
    }
    अन्यथा यदि (*r_ptr == NUL)
	r_ptr = रिक्त;
    फर्काऊ r_ptr;
}

/*
 * Clean up the given search context. Can handle a NULL pointer.
 */
    शून्य
vim_findfile_cleanup(शून्य *ctx)
{
    यदि (ctx == रिक्त)
	फर्काऊ;

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
vim_findfile(शून्य *खोज_ctx_arg)
{
    string_T	file_path;
    string_T	rest_of_wildcards;
    char_u	*path_end = रिक्त;
    ff_थाक_T	*stackp;
    ff_खोज_ctx_T *खोज_ctx;

    यदि (खोज_ctx_arg == रिक्त)
	फर्काऊ रिक्त;

    खोज_ctx = (ff_खोज_ctx_T *)खोज_ctx_arg;

    /*
     * filepath is used as buffer for various actions and as the storage to
     * return a found filename.
     */
    यदि ((file_path.string = alloc(MAXPATHL)) == रिक्त)
	फर्काऊ रिक्त;

    // store the end of the start dir -- needed for upward search
    यदि (खोज_ctx->ffsc_start_dir.string != रिक्त)
	path_end = &खोज_ctx->ffsc_start_dir.string[
					    खोज_ctx->ffsc_start_dir.length];

    // upward search loop
    लागि (;;)
    {
	// downward search loop
	लागि (;;)
	{
	    // check if user wants to stop the search
	    ui_breakcheck();
	    यदि (got_int)
		रोक;

	    // get directory to work on from stack
	    stackp = ff_झिक(खोज_ctx);
	    यदि (stackp == रिक्त)
		रोक;

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
	    यदि (stackp->ffs_filearray == रिक्त
		    && ff_check_visited(&खोज_ctx->ffsc_dir_visited_list
							  ->ffvl_visited_list,
			    stackp->ffs_fix_path.string,
			    stackp->ffs_fix_path.length,
			    stackp->ffs_wc_path.string,
			    stackp->ffs_wc_path.length) == FAIL)
	    {
#ifdef FF_VERBOSE
		यदि (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("Already Searched: %s (%s)",
				   stackp->ffs_fix_path.string, stackp->ffs_wc_path.string);
		    // don't overwrite this either
		    सन्देश_puts("\n");
		    verbose_leave_scroll();
		}
#endif
		ff_free_थाक_element(stackp);
		जारी;
	    }
#ifdef FF_VERBOSE
	    अन्यथा यदि (p_verbose >= 5)
	    {
		verbose_enter_scroll();
		smsg("Searching: %s (%s)",
				   stackp->ffs_fix_path.string, stackp->ffs_wc_path.string);
		// don't overwrite this either
		सन्देश_puts("\n");
		verbose_leave_scroll();
	    }
#endif

	    // check depth
	    यदि (stackp->ffs_level <= 0)
	    {
		ff_free_थाक_element(stackp);
		जारी;
	    }

	    file_path.string[0] = NUL;
	    file_path.length = 0;

	    /*
	     * If no filearray till now expand wildcards
	     * The function expand_wildcards() can handle an array of paths
	     * and all possible expands are returned in one array. We use this
	     * to handle the expansion of '**' into an empty string.
	     */
	    यदि (stackp->ffs_filearray == रिक्त)
	    {
		char_u *dirptrs[2];

		// we use filepath to build the path expand_wildcards() should
		// expand.
		dirptrs[0] = file_path.string;
		dirptrs[1] = रिक्त;

		// if we have a start dir copy it in
		यदि (!vim_isAbsName(stackp->ffs_fix_path.string)
						&& खोज_ctx->ffsc_start_dir.string)
		{
		    यदि (खोज_ctx->ffsc_start_dir.length + 1 < MAXPATHL)
		    {
			पूर्णाङ्क add_sep = !after_pathsep(खोज_ctx->ffsc_start_dir.string,
			    खोज_ctx->ffsc_start_dir.string + खोज_ctx->ffsc_start_dir.length);
			file_path.length = vim_snprintf(
			    (वर्ण *)file_path.string,
			    MAXPATHL,
			    "%s%s",
			    खोज_ctx->ffsc_start_dir.string,
			    add_sep ? PATHSEPSTR : "");
		    }
		    अन्यथा
		    {
			ff_free_थाक_element(stackp);
			गच्छ fail;
		    }
		}

		// append the fix part of the search path
		यदि (file_path.length + stackp->ffs_fix_path.length + 1 < MAXPATHL)
		{
		    पूर्णाङ्क add_sep = !after_pathsep(stackp->ffs_fix_path.string,
			stackp->ffs_fix_path.string + stackp->ffs_fix_path.length);
		    file_path.length += vim_snprintf(
			(वर्ण *)file_path.string + file_path.length,
			MAXPATHL - file_path.length,
			"%s%s",
			stackp->ffs_fix_path.string,
			add_sep ? PATHSEPSTR : "");
		}
		अन्यथा
		{
		    ff_free_थाक_element(stackp);
		    गच्छ fail;
		}

		rest_of_wildcards.string = stackp->ffs_wc_path.string;
		rest_of_wildcards.length = stackp->ffs_wc_path.length;
		यदि (*rest_of_wildcards.string != NUL)
		{
		    यदि (STRNCMP(rest_of_wildcards.string, "**", 2) == 0)
		    {
			char_u	*p;

			// pointer to the restrict byte
			// The restrict byte is not a character!
			p = rest_of_wildcards.string + 2;

			यदि (*p > 0)
			{
			    (*p)--;
			    यदि (file_path.length + 1 < MAXPATHL)
				file_path.string[file_path.length++] = '*';
			    अन्यथा
			    {
				ff_free_थाक_element(stackp);
				गच्छ fail;
			    }
			}

			यदि (*p == 0)
			{
			    // remove '**<numb> from wildcards
			    mch_memmove(rest_of_wildcards.string,
					rest_of_wildcards.string + 3,
					(माप_प्रकार)(rest_of_wildcards.length - 3) + 1);    // +1 for NUL
			    rest_of_wildcards.length -= 3;
			    stackp->ffs_wc_path.length = rest_of_wildcards.length;
			}
			अन्यथा
			{
			    rest_of_wildcards.string += 3;
			    rest_of_wildcards.length -= 3;
			}

			यदि (stackp->ffs_star_star_empty == 0)
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
		    जबसम्म (*rest_of_wildcards.string
			    && !vim_ispathsep(*rest_of_wildcards.string))
		    {
			यदि (file_path.length + 1 < MAXPATHL)
			{
			    file_path.string[file_path.length++] = *rest_of_wildcards.string++;
			    --rest_of_wildcards.length;
			}
			अन्यथा
			{
			    ff_free_थाक_element(stackp);
			    गच्छ fail;
			}
		    }

		    file_path.string[file_path.length] = NUL;
		    यदि (vim_ispathsep(*rest_of_wildcards.string))
		    {
			rest_of_wildcards.string++;
			rest_of_wildcards.length--;
		    }
		}

		/*
		 * Expand wildcards like "*" and "$VAR".
		 * If the path is a URL don't try this.
		 */
		यदि (path_with_url(dirptrs[0]))
		{
		    stackp->ffs_filearray = ALLOC_ONE(char_u *);
		    यदि (stackp->ffs_filearray != रिक्त
			    && (stackp->ffs_filearray[0]
				= vim_strnsave(dirptrs[0], file_path.length)) != रिक्त)
			stackp->ffs_filearray_size = 1;
		    अन्यथा
			stackp->ffs_filearray_size = 0;
		}
		अन्यथा
		    // Add EW_NOTWILD because the expanded path may contain
		    // wildcard characters that are to be taken literally.
		    // This is a bit of a hack.
		    expand_wildcards((dirptrs[1] == रिक्त) ? 1 : 2, dirptrs,
			    &stackp->ffs_filearray_size,
			    &stackp->ffs_filearray,
			    EW_DIR|EW_ADDSLASH|EW_SILENT|EW_NOTWILD);

		stackp->ffs_filearray_cur = 0;
		stackp->ffs_stage = 0;
	    }
	    अन्यथा
	    {
		rest_of_wildcards.string = &stackp->ffs_wc_path.string[
							stackp->ffs_wc_path.length];
		rest_of_wildcards.length = 0;
	    }

	    यदि (stackp->ffs_stage == 0)
	    {
		पूर्णाङ्क i;

		// this is the first time we work on this directory
		यदि (*rest_of_wildcards.string == NUL)
		{
		    माप_प्रकार  len;
		    char_u  *suf;

		    /*
		     * We don't have further wildcards to expand, so we have to
		     * check for the final file now.
		     */
		    लागि (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_size; ++i)
		    {
			यदि (!path_with_url(stackp->ffs_filearray[i])
				      && !mch_isdir(stackp->ffs_filearray[i]))
			    जारी;   // not a directory

			// prepare the filename to be checked for existence
			// below
			len = STRLEN(stackp->ffs_filearray[i]);
			यदि (len + 1 + खोज_ctx->ffsc_file_to_खोज.length
									< MAXPATHL)
			{
			    पूर्णाङ्क	add_sep = !after_pathsep(stackp->ffs_filearray[i],
				    stackp->ffs_filearray[i] + len);
			    file_path.length = vim_snprintf(
				    (वर्ण *)file_path.string,
				    MAXPATHL,
				    "%s%s%s",
				    stackp->ffs_filearray[i],
				    add_sep ? PATHSEPSTR : "",
				    खोज_ctx->ffsc_file_to_खोज.string);
			}
			अन्यथा
			{
			    ff_free_थाक_element(stackp);
			    गच्छ fail;
			}

			/*
			 * Try without extra suffix and then with suffixes
			 * from 'suffixesadd'.
			 */
			len = file_path.length;
			यदि (खोज_ctx->ffsc_tagfile)
			    suf = (char_u *)"";
			अन्यथा
			    suf = curbuf->b_p_sua;
			लागि (;;)
			{
			    // if file exists and we didn't already find it
			    यदि ((path_with_url(file_path.string)
				  || (mch_getperm(file_path.string) >= 0
				      && (खोज_ctx->ffsc_find_what
							      == FINDFILE_BOTH
					  || ((खोज_ctx->ffsc_find_what
							      == FINDFILE_DIR)
						   == mch_isdir(file_path.string)))))
#ifndef FF_VERBOSE
				    && (ff_check_visited(
					    &खोज_ctx->ffsc_visited_list
							   ->ffvl_visited_list,
					    file_path.string,
					    file_path.length,
					    (char_u *)"", 0) == OK)
#endif
			       )
			    {
#ifdef FF_VERBOSE
				यदि (ff_check_visited(
					    &खोज_ctx->ffsc_visited_list
							   ->ffvl_visited_list,
					      file_path.string,
					      file_path.length,
					      (char_u *)"", 0) == FAIL)
				{
				    यदि (p_verbose >= 5)
				    {
					verbose_enter_scroll();
					smsg("Already: %s", file_path.string);
					// don't overwrite this either
					सन्देश_puts("\n");
					verbose_leave_scroll();
				    }
				    जारी;
				}
#endif

				// push dir to examine rest of subdirs later
				stackp->ffs_filearray_cur = i + 1;
				ff_थप(खोज_ctx, stackp);

				यदि (!path_with_url(file_path.string))
				    file_path.length = simplify_filename(file_path.string);

				यदि (mch_dirname(ff_expand_buffer.string, MAXPATHL)
									== OK)
				{
				    char_u  *p;

				    ff_expand_buffer.length = STRLEN(ff_expand_buffer.string);
				    p = shorten_fname(file_path.string,
							    ff_expand_buffer.string);
				    यदि (p != रिक्त)
				    {
					mch_memmove(file_path.string, p,
					    (माप_प्रकार)((file_path.string + file_path.length) - p) + 1);  // +1 for NUL
					file_path.length -= (p - file_path.string);
				    }
				}
#ifdef FF_VERBOSE
				यदि (p_verbose >= 5)
				{
				    verbose_enter_scroll();
				    smsg("HIT: %s", file_path.string);
				    // don't overwrite this either
				    सन्देश_puts("\n");
				    verbose_leave_scroll();
				}
#endif
				फर्काऊ file_path.string;
			    }

			    // Not found or found already, try next suffix.
			    यदि (*suf == NUL)
				रोक;
			    file_path.length = len + copy_option_part(&suf,
				  file_path.string + len,
				  (पूर्णाङ्क)(MAXPATHL - len), ",");
			}
		    }
		}
		अन्यथा
		{
		    /*
		     * still wildcards left, push the directories for further
		     * search
		     */
		    लागि (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_size; ++i)
		    {
			यदि (!mch_isdir(stackp->ffs_filearray[i]))
			    जारी;	// not a directory

			ff_थप(खोज_ctx,
				ff_create_थाक_element(
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
	    यदि (STRNCMP(stackp->ffs_wc_path.string, "**", 2) == 0)
	    {
		पूर्णाङ्क i;

		लागि (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_size; ++i)
		{
		    यदि (fnamecmp(stackp->ffs_filearray[i],
						   stackp->ffs_fix_path.string) == 0)
			जारी; // don't repush same directory
		    यदि (!mch_isdir(stackp->ffs_filearray[i]))
			जारी;   // not a directory
		    ff_थप(खोज_ctx,
			    ff_create_थाक_element(
				stackp->ffs_filearray[i],
				STRLEN(stackp->ffs_filearray[i]),
				stackp->ffs_wc_path.string,
				stackp->ffs_wc_path.length,
				stackp->ffs_level - 1, 1));
		}
	    }

	    // we are done with the current directory
	    ff_free_थाक_element(stackp);
	}

	// If we reached this, we didn't find anything downwards.
	// Let's check if we should do an upward search.
	यदि (खोज_ctx->ffsc_start_dir.string
		&& खोज_ctx->ffsc_stopdirs_v != रिक्त && !got_int)
	{
	    ff_थाक_T  *sptr;
	    // path_end may point to the NUL or the previous path separator
	    पूर्णाङ्क plen = (path_end - खोज_ctx->ffsc_start_dir.string)
							  + (*path_end != NUL);

	    // is the last starting directory in the stop list?
	    यदि (ff_path_in_stoplist(खोज_ctx->ffsc_start_dir.string,
				    plen, खोज_ctx->ffsc_stopdirs_v) == TRUE)
		रोक;

	    // cut of last dir
	    जबसम्म (path_end > खोज_ctx->ffsc_start_dir.string
						  && vim_ispathsep(*path_end))
		path_end--;
	    जबसम्म (path_end > खोज_ctx->ffsc_start_dir.string
					      && !vim_ispathsep(path_end[-1]))
		path_end--;
	    *path_end = NUL;

	    // we may have shortened search_ctx->ffsc_start_dir, so update it's length
	    खोज_ctx->ffsc_start_dir.length = (माप_प्रकार)(path_end - खोज_ctx->ffsc_start_dir.string);
	    path_end--;

	    यदि (*खोज_ctx->ffsc_start_dir.string == NUL)
		रोक;

	    यदि (खोज_ctx->ffsc_start_dir.length + 1
		    + खोज_ctx->ffsc_fix_path.length < MAXPATHL)
	    {
		पूर्णाङ्क add_sep = !after_pathsep(खोज_ctx->ffsc_start_dir.string,
			    खोज_ctx->ffsc_start_dir.string + खोज_ctx->ffsc_start_dir.length);
		file_path.length = vim_snprintf(
			(वर्ण *)file_path.string,
			MAXPATHL,
			"%s%s%s",
			खोज_ctx->ffsc_start_dir.string,
			add_sep ? PATHSEPSTR : "",
			खोज_ctx->ffsc_fix_path.string);
	    }
	    अन्यथा
		गच्छ fail;

	    // create a new stack entry
	    sptr = ff_create_थाक_element(file_path.string, file_path.length,
		    खोज_ctx->ffsc_wc_path.string, खोज_ctx->ffsc_wc_path.length,
		    खोज_ctx->ffsc_level, 0);
	    यदि (sptr == रिक्त)
		रोक;
	    ff_थप(खोज_ctx, sptr);
	}
	अन्यथा
	    रोक;
    }

fail:
    vim_free(file_path.string);
    फर्काऊ रिक्त;
}

/*
 * Free the list of lists of visited files and directories
 * Can handle it if the passed search_context is NULL;
 */
    स्थिर शून्य
vim_findfile_free_visited(शून्य *खोज_ctx_arg)
{
    ff_खोज_ctx_T *खोज_ctx;

    यदि (खोज_ctx_arg == रिक्त)
	फर्काऊ;

    खोज_ctx = (ff_खोज_ctx_T *)खोज_ctx_arg;
    vim_findfile_free_visited_list(&खोज_ctx->ffsc_visited_lists_list);
    vim_findfile_free_visited_list(&खोज_ctx->ffsc_dir_visited_lists_list);
}

    स्थिर शून्य
vim_findfile_free_visited_list(ff_visited_list_hdr_T **list_headp)
{
    ff_visited_list_hdr_T *vp;

    जबसम्म (*list_headp != रिक्त)
    {
	vp = (*list_headp)->ffvl_next;
	ff_free_visited_list((*list_headp)->ffvl_visited_list);

	vim_free((*list_headp)->ffvl_filename);
	vim_free(*list_headp);
	*list_headp = vp;
    }
    *list_headp = रिक्त;
}

    स्थिर शून्य
ff_free_visited_list(ff_visited_T *vl)
{
    ff_visited_T *vp;

    जबसम्म (vl != रिक्त)
    {
	vp = vl->ffv_next;
	vim_free(vl->ffv_wc_path);
	vim_free(vl);
	vl = vp;
    }
    vl = रिक्त;
}

/*
 * Returns the already visited list for the given filename. If none is found it
 * allocates a new one.
 */
    स्थिर ff_visited_list_hdr_T*
ff_get_visited_list(
    char_u			*filename,
    माप_प्रकार			filenamelen,
    ff_visited_list_hdr_T	**list_headp)
{
    ff_visited_list_hdr_T  *retptr = रिक्त;

    // check if a visited list for the given filename exists
    यदि (*list_headp != रिक्त)
    {
	retptr = *list_headp;
	जबसम्म (retptr != रिक्त)
	{
	    यदि (fnamecmp(filename, retptr->ffvl_filename) == 0)
	    {
#ifdef FF_VERBOSE
		यदि (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("ff_get_visited_list: FOUND list for %s",
								    filename);
		    // don't overwrite this either
		    सन्देश_puts("\n");
		    verbose_leave_scroll();
		}
#endif
		फर्काऊ retptr;
	    }
	    retptr = retptr->ffvl_next;
	}
    }

#ifdef FF_VERBOSE
    यदि (p_verbose >= 5)
    {
	verbose_enter_scroll();
	smsg("ff_get_visited_list: new list for %s", filename);
	// don't overwrite this either
	सन्देश_puts("\n");
	verbose_leave_scroll();
    }
#endif

    /*
     * if we reach this we didn't find a list and we have to allocate new list
     */
    retptr = ALLOC_ONE(ff_visited_list_hdr_T);
    यदि (retptr == रिक्त)
	फर्काऊ रिक्त;

    retptr->ffvl_visited_list = रिक्त;
    retptr->ffvl_filename = vim_strnsave(filename, filenamelen);
    यदि (retptr->ffvl_filename == रिक्त)
    {
	vim_free(retptr);
	फर्काऊ रिक्त;
    }
    retptr->ffvl_next = *list_headp;
    *list_headp = retptr;

    फर्काऊ retptr;
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
    स्थिर पूर्णाङ्क
ff_wc_equal(char_u *s1, char_u *s2)
{
    पूर्णाङ्क		i, j;
    पूर्णाङ्क		c1 = NUL;
    पूर्णाङ्क		c2 = NUL;
    पूर्णाङ्क		prev1 = NUL;
    पूर्णाङ्क		prev2 = NUL;

    यदि (s1 == s2)
	फर्काऊ TRUE;

    यदि (s1 == रिक्त || s2 == रिक्त)
	फर्काऊ FALSE;

    लागि (i = 0, j = 0; s1[i] != NUL && s2[j] != NUL;)
    {
	c1 = PTR2CHAR(s1 + i);
	c2 = PTR2CHAR(s2 + j);

	यदि ((p_fic ? MB_TOLOWER(c1) != MB_TOLOWER(c2) : c1 != c2)
		&& (prev1 != '*' || prev2 != '*'))
	    फर्काऊ FALSE;
	prev2 = prev1;
	prev1 = c1;

	i += mb_ptr2len(s1 + i);
	j += mb_ptr2len(s2 + j);
    }
    फर्काऊ s1[i] == s2[j];
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
    स्थिर पूर्णाङ्क
ff_check_visited(
    ff_visited_T	**visited_list,
    char_u		*fname,
    माप_प्रकार		fnamelen,
    char_u		*wc_path,
    माप_प्रकार		wc_pathlen)
{
    ff_visited_T	*vp;
#ifdef UNIX
    stat_T		st;
    पूर्णाङ्क			url = FALSE;
#endif

    // For a URL we only compare the name, otherwise we compare the
    // device/inode (unix) or the full path name (not Unix).
    यदि (path_with_url(fname))
    {
	vim_strncpy(ff_expand_buffer.string, fname, fnamelen);
	ff_expand_buffer.length = fnamelen;
#ifdef UNIX
	url = TRUE;
#endif
    }
    अन्यथा
    {
	ff_expand_buffer.string[0] = NUL;
	ff_expand_buffer.length = 0;
#ifdef UNIX
	यदि (mch_stat((वर्ण *)fname, &st) < 0)
	    फर्काऊ FAIL;
#अन्यथा
	यदि (vim_FullName(fname, ff_expand_buffer.string, MAXPATHL, TRUE) == FAIL)
	    फर्काऊ FAIL;
	ff_expand_buffer.length = STRLEN(ff_expand_buffer.string);
#endif
    }

    // check against list of already visited files
    लागि (vp = *visited_list; vp != रिक्त; vp = vp->ffv_next)
    {
	यदि (
#ifdef UNIX
		!url ? (vp->ffv_dev_valid && vp->ffv_dev == st.st_dev
						  && vp->ffv_ino == st.st_ino)
		     :
#endif
		fnamecmp(vp->ffv_fname, ff_expand_buffer.string) == 0
	   )
	{
	    // are the wildcard parts equal
	    यदि (ff_wc_equal(vp->ffv_wc_path, wc_path) == TRUE)
		// already visited
		फर्काऊ FAIL;
	}
    }

    /*
     * New file/dir.  Add it to the list of visited files/dirs.
     */
    vp = alloc(
	     दुरत्व(ff_visited_T, ffv_fname) + ff_expand_buffer.length + 1);
    यदि (vp == रिक्त)
	फर्काऊ OK;

#ifdef UNIX
    यदि (!url)
    {
	vp->ffv_dev_valid = TRUE;
	vp->ffv_dev = st.st_dev;
	vp->ffv_ino = st.st_ino;
	vp->ffv_fname[0] = NUL;
    }
    अन्यथा
    {
	vp->ffv_dev_valid = FALSE;
#endif
	STRCPY(vp->ffv_fname, ff_expand_buffer.string);
#ifdef UNIX
    }
#endif
    यदि (wc_path != रिक्त)
	vp->ffv_wc_path = vim_strnsave(wc_path, wc_pathlen);
    अन्यथा
	vp->ffv_wc_path = रिक्त;

    vp->ffv_next = *visited_list;
    *visited_list = vp;

    फर्काऊ OK;
}

/*
 * create stack element from given path pieces
 */
    स्थिर ff_थाक_T *
ff_create_थाक_element(
    char_u	*fix_part,
    माप_प्रकार	fix_partlen,
    char_u	*wc_part,
    माप_प्रकार	wc_partlen,
    पूर्णाङ्क		level,
    पूर्णाङ्क		star_star_empty)
{
    ff_थाक_T	*new;

    new = ALLOC_ONE(ff_थाक_T);
    यदि (new == रिक्त)
	फर्काऊ रिक्त;

    new->ffs_prev	   = रिक्त;
    new->ffs_filearray	   = रिक्त;
    new->ffs_filearray_size = 0;
    new->ffs_filearray_cur  = 0;
    new->ffs_stage	   = 0;
    new->ffs_level	   = level;
    new->ffs_star_star_empty = star_star_empty;

    // the following saves NULL pointer checks in vim_findfile
    यदि (fix_part == रिक्त)
    {
	fix_part = (char_u *)"";
	fix_partlen = 0;
    }
    new->ffs_fix_path.string = vim_strnsave(fix_part, fix_partlen);
    new->ffs_fix_path.length = fix_partlen;

    यदि (wc_part == रिक्त)
    {
	wc_part = (char_u *)"";
	wc_partlen = 0;
    }
    new->ffs_wc_path.string = vim_strnsave(wc_part, wc_partlen);
    new->ffs_wc_path.length = wc_partlen;

    यदि (new->ffs_fix_path.string == रिक्त || new->ffs_wc_path.string == रिक्त)
    {
	ff_free_थाक_element(new);
	new = रिक्त;
    }

    फर्काऊ new;
}

/*
 * Push a dir on the directory stack.
 */
    स्थिर शून्य
ff_थप(ff_खोज_ctx_T *खोज_ctx, ff_थाक_T *थाक_ptr)
{
    // check for NULL pointer, not to return an error to the user, but
    // to prevent a crash
    यदि (थाक_ptr == रिक्त)
	फर्काऊ;

    थाक_ptr->ffs_prev = खोज_ctx->ffsc_थाक_ptr;
    खोज_ctx->ffsc_थाक_ptr = थाक_ptr;
}

/*
 * Pop a dir from the directory stack.
 * Returns NULL if stack is empty.
 */
    स्थिर ff_थाक_T *
ff_झिक(ff_खोज_ctx_T *खोज_ctx)
{
    ff_थाक_T  *sptr;

    sptr = खोज_ctx->ffsc_थाक_ptr;
    यदि (खोज_ctx->ffsc_थाक_ptr != रिक्त)
	खोज_ctx->ffsc_थाक_ptr = खोज_ctx->ffsc_थाक_ptr->ffs_prev;

    फर्काऊ sptr;
}

/*
 * free the given stack element
 */
    स्थिर शून्य
ff_free_थाक_element(ff_थाक_T *थाक_ptr)
{
    // VIM_CLEAR_STRING handles possible NULL pointers
    VIM_CLEAR_STRING(थाक_ptr->ffs_fix_path);
    VIM_CLEAR_STRING(थाक_ptr->ffs_wc_path);

    यदि (थाक_ptr->ffs_filearray != रिक्त)
	FreeWild(थाक_ptr->ffs_filearray_size, थाक_ptr->ffs_filearray);

    vim_free(थाक_ptr);
}

/*
 * Clear the search context, but NOT the visited list.
 */
    स्थिर शून्य
ff_clear(ff_खोज_ctx_T *खोज_ctx)
{
    ff_थाक_T   *sptr;

    // clear up stack
    जबसम्म ((sptr = ff_झिक(खोज_ctx)) != रिक्त)
	ff_free_थाक_element(sptr);

    यदि (खोज_ctx->ffsc_stopdirs_v != रिक्त)
    {
	पूर्णाङ्क  i = 0;

	जबसम्म (खोज_ctx->ffsc_stopdirs_v[i].string != रिक्त)
	{
	    vim_free(खोज_ctx->ffsc_stopdirs_v[i].string);
	    i++;
	}
	VIM_CLEAR(खोज_ctx->ffsc_stopdirs_v);
    }

    // reset everything
    VIM_CLEAR_STRING(खोज_ctx->ffsc_file_to_खोज);
    VIM_CLEAR_STRING(खोज_ctx->ffsc_start_dir);
    VIM_CLEAR_STRING(खोज_ctx->ffsc_fix_path);
    VIM_CLEAR_STRING(खोज_ctx->ffsc_wc_path);
    खोज_ctx->ffsc_level = 0;
}

/*
 * check if the given path is in the stopdirs
 * returns TRUE if yes else FALSE
 */
    स्थिर पूर्णाङ्क
ff_path_in_stoplist(char_u *path, पूर्णाङ्क path_len, string_T *stopdirs_v)
{
    पूर्णाङ्क		i = 0;

    // eat up trailing path separators, except the first
    जबसम्म (path_len > 1 && vim_ispathsep(path[path_len - 1]))
	path_len--;

    // if no path consider it as match
    यदि (path_len == 0)
	फर्काऊ TRUE;

    लागि (i = 0; stopdirs_v[i].string != रिक्त; i++)
	// match for parent directory. So '/home' also matches
	// '/home/rks'. Check for PATHSEP in stopdirs_v[i], else
	// '/home/r' would also match '/home/rks'
	यदि (fnamencmp(stopdirs_v[i].string, path, path_len) == 0
		&& ((पूर्णाङ्क)stopdirs_v[i].length <= path_len
		    || vim_ispathsep(stopdirs_v[i].string[path_len])))
	    फर्काऊ TRUE;

    फर्काऊ FALSE;
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
    पूर्णाङ्क		len,		// length of file name
    पूर्णाङ्क		options,
    पूर्णाङ्क		first,		// use count'th matching file name
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_to_find,	// in/out: modified copy of file name
    वर्ण	**खोज_ctx)	// in/out: state of the search
{
    फर्काऊ find_file_in_path_option(ptr, len, options, first,
	    *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path,
	    FINDFILE_BOTH, rel_fname, curbuf->b_p_sua,
	    file_to_find, खोज_ctx);
}

#यदि defined(EXITFREE)
    शून्य
free_findfile(शून्य)
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
    पूर्णाङ्क		len,		// length of file name
    पूर्णाङ्क		options,
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_to_find,	// in/out: modified copy of file name
    वर्ण	**खोज_ctx)	// in/out: state of the search
{
    फर्काऊ find_file_in_path_option(ptr, len, options, TRUE, p_cdpath,
				       FINDFILE_DIR, rel_fname, (char_u *)"",
				       file_to_find, खोज_ctx);
}

    char_u *
find_file_in_path_option(
    char_u	*ptr,		// file name
    पूर्णाङ्क		len,		// length of file name
    पूर्णाङ्क		options,
    पूर्णाङ्क		first,		// use count'th matching file name
    char_u	*path_option,	// p_path or p_cdpath
    पूर्णाङ्क		find_what,	// FINDFILE_FILE, _DIR or _BOTH
    char_u	*rel_fname,	// file name we are looking relative to.
    char_u	*suffixes,	// list of suffixes, 'suffixesadd' option
    char_u	**file_to_find,	// in/out: modified copy of file name
    वर्ण	**खोज_ctx_arg) // in/out: state of the search
{
    ff_खोज_ctx_T	**खोज_ctx = (ff_खोज_ctx_T **)खोज_ctx_arg;
    स्थिर char_u	*dir;
    स्थिर पूर्णाङ्क		did_findfile_init = FALSE;
    char_u		*file_name = रिक्त;
    पूर्णाङ्क			rel_to_curdir;
#ifdef AMIGA
    संरचना Process	*proc = (संरचना Process *)FindTask(0L);
    APTR		save_winptr = proc->pr_WindowPtr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_WindowPtr = (APTR)-1L;
#endif
    स्थिर माप_प्रकार	file_to_findlen = 0;

    यदि (first == TRUE)
    {
	char_u	save_char;

	यदि (len == 0)
	    फर्काऊ रिक्त;

	// copy file name into NameBuff, expanding environment variables
	save_char = ptr[len];
	ptr[len] = NUL;
	file_to_findlen = expand_env_esc(ptr, NameBuff, MAXPATHL, रिक्त, TRUE, रिक्त);
	ptr[len] = save_char;

	vim_free(*file_to_find);
	*file_to_find = vim_strnsave(NameBuff, file_to_findlen);
	यदि (*file_to_find == रिक्त)	// out of memory
	{
	    file_name = रिक्त;
	    file_to_findlen = 0;
	    गच्छ theend;
	}
	यदि (options & FNAME_UNESC)
	{
	    // Change all "\ " to " ".
	    लागि (ptr = *file_to_find; *ptr != NUL; ++ptr)
		यदि (ptr[0] == '\\' && ptr[1] == ' ')
		{
		    mch_memmove(ptr, ptr + 1,
			(माप_प्रकार)((*file_to_find + file_to_findlen) - (ptr + 1)) + 1);
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
    यदि (vim_isAbsName(*file_to_find)
	    // "..", "../path", "." and "./path": don't use the path_option
	    || rel_to_curdir
#यदि defined(MSWIN)
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
	यदि (first == TRUE)
	{
	    पूर्णाङ्क		l;
	    पूर्णाङ्क		run;
	    माप_प्रकार	rel_fnamelen = 0;
	    char_u	*suffix;

	    यदि (path_with_url(*file_to_find))
	    {
		file_name = vim_strnsave(*file_to_find, file_to_findlen);
		गच्छ theend;
	    }

	    यदि (rel_fname != रिक्त)
		rel_fnamelen = STRLEN(rel_fname);

	    // When FNAME_REL flag given first use the directory of the file.
	    // Otherwise or when this fails use the current directory.
	    लागि (run = 1; run <= 2; ++run)
	    {
		l = (पूर्णाङ्क)file_to_findlen;
		यदि (run == 1
			&& rel_to_curdir
			&& (options & FNAME_REL)
			&& rel_fname != रिक्त
			&& rel_fnamelen + l < MAXPATHL)
		{
		    l = vim_snprintf(
			(वर्ण *)NameBuff,
			MAXPATHL,
			"%.*s%s",
			(पूर्णाङ्क)(gettail(rel_fname) - rel_fname),
			rel_fname,
			*file_to_find);
		}
		अन्यथा
		{
		    STRCPY(NameBuff, *file_to_find);
		    run = 2;
		}

		// When the file doesn't exist, try adding parts of
		// 'suffixesadd'.
		suffix = suffixes;
		लागि (;;)
		{
		    यदि (mch_getperm(NameBuff) >= 0
			     && (find_what == FINDFILE_BOTH
				 || ((find_what == FINDFILE_DIR)
						    == mch_isdir(NameBuff))))
		    {
			file_name = vim_strnsave(NameBuff,
						  simplify_filename(NameBuff));
			गच्छ theend;
		    }
		    यदि (*suffix == NUL)
			रोक;
		    copy_option_part(&suffix, NameBuff + l, MAXPATHL - l, ",");
		}
	    }
	}
    }
    अन्यथा
    {
	/*
	 * Loop over all paths in the 'path' or 'cdpath' option.
	 * When "first" is set, first setup to the start of the option.
	 * Otherwise continue to find the next match.
	 */
	यदि (first == TRUE)
	{
	    // vim_findfile_free_visited can handle a possible NULL pointer
	    vim_findfile_free_visited(*खोज_ctx);
	    dir = path_option;
	    did_findfile_init = FALSE;
	}

	लागि (;;)
	{
	    यदि (did_findfile_init)
	    {
		file_name = vim_findfile(*खोज_ctx);
		यदि (file_name != रिक्त)
		    रोक;

		did_findfile_init = FALSE;
	    }
	    अन्यथा
	    {
		char_u	*buf;
		char_u  *r_ptr;

		यदि (dir == रिक्त || *dir == NUL)
		{
		    // We searched all paths of the option, now we can
		    // free the search context.
		    vim_findfile_cleanup(*खोज_ctx);
		    *खोज_ctx = रिक्त;
		    रोक;
		}

		यदि ((buf = alloc(MAXPATHL)) == रिक्त)
		    रोक;

		// copy next path
		buf[0] = NUL;
		copy_option_part(&dir, buf, MAXPATHL, " ,");

		// get the stopdir string
		r_ptr = vim_findfile_stopdir(buf);
		*खोज_ctx = vim_findfile_init(buf, *file_to_find, file_to_findlen,
					    r_ptr, 100, FALSE, find_what,
					   *खोज_ctx, FALSE, rel_fname);
		यदि (*खोज_ctx != रिक्त)
		    did_findfile_init = TRUE;
		vim_free(buf);
	    }
	}
    }
    यदि (file_name == रिक्त && (options & FNAME_MESS))
    {
	यदि (first == TRUE)
	{
	    यदि (find_what == FINDFILE_DIR)
		semsg(_(e_cant_find_directory_str_in_cdpath), *file_to_find);
	    अन्यथा
		semsg(_(e_cant_find_file_str_in_path), *file_to_find);
	}
	अन्यथा
	{
	    यदि (find_what == FINDFILE_DIR)
		semsg(_(e_no_more_directory_str_found_in_cdpath),
								*file_to_find);
	    अन्यथा
		semsg(_(e_no_more_file_str_found_in_path), *file_to_find);
	}
    }

theend:
#ifdef AMIGA
    proc->pr_WindowPtr = save_winptr;
#endif
    फर्काऊ file_name;
}

/*
 * Get the file name at the cursor.
 * If Visual mode is active, use the selected text if it's in one line.
 * Returns the name in allocated memory, NULL for failure.
 */
    char_u *
grab_file_name(दीर्घ गन्ती, linenr_T *file_lnum)
{
    पूर्णाङ्क options = FNAME_MESS|FNAME_EXP|FNAME_REL|FNAME_UNESC;

    यदि (VIsual_active)
    {
	पूर्णाङ्क	len;
	char_u	*ptr;

	यदि (get_visual_text(रिक्त, &ptr, &len) == FAIL)
	    फर्काऊ रिक्त;
	// Only recognize ":123" here
	यदि (file_lnum != रिक्त && ptr[len] == ':' && SAFE_isdigit(ptr[len + 1]))
	{
	    char_u *p = ptr + len + 1;

	    *file_lnum = getdigits(&p);
	}
	फर्काऊ find_file_name_in_path(ptr, len, options,
						     गन्ती, curbuf->b_ffname);
    }
    फर्काऊ file_name_at_cursor(options | FNAME_HYP, गन्ती, file_lnum);
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
file_name_at_cursor(पूर्णाङ्क options, दीर्घ गन्ती, linenr_T *file_lnum)
{
    फर्काऊ file_name_in_पङ्क्ति(ml_get_curline(),
		      curwin->w_cursor.col, options, गन्ती, curbuf->b_ffname,
		      file_lnum);
}

/*
 * Return the name of the file under or after ptr[col].
 * Otherwise like file_name_at_cursor().
 */
    char_u *
file_name_in_पङ्क्ति(
    char_u	*पङ्क्ति,
    पूर्णाङ्क		col,
    पूर्णाङ्क		options,
    दीर्घ	गन्ती,
    char_u	*rel_fname,	// file we are searching relative to
    linenr_T	*file_lnum)	// line number after the file name
{
    char_u	*ptr;
    पूर्णाङ्क		len;
    पूर्णाङ्क		in_type = TRUE;
    पूर्णाङ्क		is_url = FALSE;

    /*
     * search forward for what could be the start of a file name
     */
    ptr = पङ्क्ति + col;
    जबसम्म (*ptr != NUL && !vim_isfilec(*ptr))
	MB_PTR_ADV(ptr);
    यदि (*ptr == NUL)		// nothing found
    {
	यदि (options & FNAME_MESS)
	    emsg(_(e_no_file_name_under_cursor));
	फर्काऊ रिक्त;
    }

    /*
     * Search backward for first char of the file name.
     * Go one char back to ":" before "//" even when ':' is not in 'isfname'.
     */
    जबसम्म (ptr > पङ्क्ति)
    {
	यदि (has_mbyte && (len = (*mb_head_off)(पङ्क्ति, ptr - 1)) > 0)
	    ptr -= len + 1;
	अन्यथा यदि (vim_isfilec(ptr[-1])
		|| ((options & FNAME_HYP) && path_is_url(ptr - 1)))
	    --ptr;
	अन्यथा
	    रोक;
    }

    /*
     * Search forward for the last char of the file name.
     * Also allow "://" when ':' is not in 'isfname'.
     */
    len = 0;
    जबसम्म (vim_isfilec(ptr[len]) || (ptr[len] == '\\' && ptr[len + 1] == ' ')
		 || ((options & FNAME_HYP) && path_is_url(ptr + len))
		 || (is_url && vim_strchr((char_u *)":?&=", ptr[len]) != रिक्त))
    {
	// After type:// we also include :, ?, & and = as valid characters, so
	// that http://google.com:8080?q=this&that=ok works.
	यदि ((ptr[len] >= 'A' && ptr[len] <= 'Z')
				       || (ptr[len] >= 'a' && ptr[len] <= 'z'))
	{
	    यदि (in_type && path_is_url(ptr + len + 1))
		is_url = TRUE;
	}
	अन्यथा
	    in_type = FALSE;

	यदि (ptr[len] == '\\')
	    // Skip over the "\" in "\ ".
	    ++len;
	यदि (has_mbyte)
	    len += (*mb_ptr2len)(ptr + len);
	अन्यथा
	    ++len;
    }

    /*
     * If there is trailing punctuation, remove it.
     * But don't remove "..", could be a directory name.
     */
    यदि (len > 2 && vim_strchr((char_u *)".,:;!", ptr[len - 1]) != रिक्त
						       && ptr[len - 2] != '.')
	--len;

    यदि (file_lnum != रिक्त)
    {
	char_u	*p;
	वर्ण	*match_text = " line ";		// english
	माप_प्रकार	match_textlen = 6;

	// Get the number after the file name and a separator character.
	// Also accept " line 999" with and without the same translation as
	// used in last_set_msg().
	p = ptr + len;
	यदि (STRNCMP(p, match_text, match_textlen) == 0)
	    p += match_textlen;
	अन्यथा
	{
	    // no match with english, try localized
	    match_text = _(पङ्क्ति_सन्देश);
	    match_textlen = STRLEN(match_text);

	    यदि (STRNCMP(p, match_text, match_textlen) == 0)
		p += match_textlen;
	    अन्यथा
		p = skipwhite(p);
	}
	यदि (*p != NUL)
	{
	    यदि (!SAFE_isdigit(*p))
		++p;		    // skip the separator
	    p = skipwhite(p);
	    यदि (SAFE_isdigit(*p))
		*file_lnum = (पूर्णाङ्क)getdigits(&p);
	}
    }

    फर्काऊ find_file_name_in_path(ptr, len, options, गन्ती, rel_fname);
}

#यदि defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    स्थिर char_u *
eval_includeexpr(char_u *ptr, पूर्णाङ्क len)
{
    char_u	*res;
    sctx_T	save_sctx = current_sctx;

    set_vim_var_string(VV_FNAME, ptr, len);
    current_sctx = curbuf->b_p_script_ctx[BV_INEX];

    res = eval_to_string_safe(curbuf->b_p_inex,
	    was_set_insecurely(curwin, (char_u *)"includeexpr", OPT_LOCAL),
								   TRUE, TRUE);

    set_vim_var_string(VV_FNAME, रिक्त, 0);
    current_sctx = save_sctx;
    फर्काऊ res;
}
#endif

/*
 * Return the name of the file ptr[len] in 'path'.
 * Otherwise like file_name_at_cursor().
 */
    char_u *
find_file_name_in_path(
    char_u	*ptr,
    पूर्णाङ्क		len,
    पूर्णाङ्क		options,
    दीर्घ	गन्ती,
    char_u	*rel_fname)	// file we are searching relative to
{
    char_u	*file_name;
    पूर्णाङ्क		c;
#यदि defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    char_u	*tofree = रिक्त;
#endif

    यदि (len == 0)
	फर्काऊ रिक्त;

#यदि defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    यदि ((options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
    {
	tofree = eval_includeexpr(ptr, len);
	यदि (tofree != रिक्त)
	{
	    ptr = tofree;
	    len = (पूर्णाङ्क)STRLEN(ptr);
	}
    }
#endif

    यदि (options & FNAME_EXP)
    {
	char_u	*file_to_find = रिक्त;
	वर्ण	*खोज_ctx = रिक्त;

	file_name = find_file_in_path(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_to_find, &खोज_ctx);

#यदि defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
	/*
	 * If the file could not be found in a normal way, try applying
	 * 'includeexpr' (unless done already).
	 */
	यदि (file_name == रिक्त
		&& !(options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
	{
	    tofree = eval_includeexpr(ptr, len);
	    यदि (tofree != रिक्त)
	    {
		ptr = tofree;
		len = (पूर्णाङ्क)STRLEN(ptr);
		file_name = find_file_in_path(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_to_find, &खोज_ctx);
	    }
	}
#endif
	यदि (file_name == रिक्त && (options & FNAME_MESS))
	{
	    c = ptr[len];
	    ptr[len] = NUL;
	    semsg(_(e_cant_find_file_str_in_path_2), ptr);
	    ptr[len] = c;
	}

	// Repeat finding the file "count" times.  This matters when it
	// appears several times in the path.
	जबसम्म (file_name != रिक्त && --गन्ती > 0)
	{
	    vim_free(file_name);
	    file_name = find_file_in_path(ptr, len, options, FALSE, rel_fname,
						   &file_to_find, &खोज_ctx);
	}

	vim_free(file_to_find);
	vim_findfile_cleanup(खोज_ctx);
    }
    अन्यथा
	file_name = vim_strnsave(ptr, len);

#यदि defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    vim_free(tofree);
#endif

    फर्काऊ file_name;
}

/*
 * Return the end of the directory name, on the first path
 * separator:
 * "/path/file", "/path/dir/", "/path//dir", "/file"
 *	 ^	       ^	     ^	      ^
 */
    स्थिर char_u *
gettail_dir(char_u *fname)
{
    char_u	*dir_end = fname;
    char_u	*next_dir_end = fname;
    पूर्णाङ्क		look_for_sep = TRUE;
    char_u	*p;

    लागि (p = fname; *p != NUL; )
    {
	यदि (vim_ispathsep(*p))
	{
	    यदि (look_for_sep)
	    {
		next_dir_end = p;
		look_for_sep = FALSE;
	    }
	}
	अन्यथा
	{
	    यदि (!look_for_sep)
		dir_end = next_dir_end;
	    look_for_sep = TRUE;
	}
	MB_PTR_ADV(p);
    }
    फर्काऊ dir_end;
}

/*
 * return TRUE if 'c' is a path list separator.
 */
    पूर्णाङ्क
vim_ispathlistsep(पूर्णाङ्क c)
{
#ifdef UNIX
    फर्काऊ (c == ':');
#अन्यथा
    फर्काऊ (c == ';');	// might not be right for every system...
#endif
}

/*
 * Moves "*psep" back to the previous path separator in "path".
 * Returns FAIL is "*psep" ends up at the beginning of "path".
 */
    स्थिर पूर्णाङ्क
find_previous_pathsep(char_u *path, char_u **psep)
{
    // skip the current separator
    यदि (*psep > path && vim_ispathsep(**psep))
	--*psep;

    // find the previous separator
    जबसम्म (*psep > path)
    {
	यदि (vim_ispathsep(**psep))
	    फर्काऊ OK;
	MB_PTR_BACK(path, *psep);
    }

    फर्काऊ FAIL;
}

/*
 * Returns TRUE if "maybe_unique" is unique wrt other_paths in "gap".
 * "maybe_unique" is the end portion of "((char_u **)gap->ga_data)[i]".
 */
    स्थिर पूर्णाङ्क
is_unique(char_u *maybe_unique, garray_T *gap, पूर्णाङ्क i)
{
    पूर्णाङ्क	    j;
    पूर्णाङ्क	    candidate_len = (पूर्णाङ्क)STRLEN(maybe_unique);
    पूर्णाङ्क	    other_path_len;
    char_u  **other_paths = (char_u **)gap->ga_data;
    char_u  *rival;

    लागि (j = 0; j < gap->ga_len; j++)
    {
	यदि (j == i)
	    जारी;  // don't compare it with itself

	other_path_len = (पूर्णाङ्क)STRLEN(other_paths[j]);
	यदि (other_path_len < candidate_len)
	    जारी;  // it's different when it's shorter

	rival = other_paths[j] + other_path_len - candidate_len;
	यदि (fnamecmp(maybe_unique, rival) == 0
		&& (rival == other_paths[j] || vim_ispathsep(*(rival - 1))))
	    फर्काऊ FALSE;  // match
    }

    फर्काऊ TRUE;  // no match found
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
    स्थिर शून्य
expand_path_option(
	char_u		*curdir,
	char_u		*path_option,	// p_path or p_cdpath
	garray_T	*gap)
{
    char_u	*buf;
    माप_प्रकार	buflen;
    char_u	*p;
    माप_प्रकार	curdirlen = 0;

    यदि ((buf = alloc(MAXPATHL)) == रिक्त)
	फर्काऊ;

    जबसम्म (*path_option != NUL)
    {
	buflen = copy_option_part(&path_option, buf, MAXPATHL, " ,");

	// do not expand backticks, could have been set via a modeline
	यदि (vim_strchr(buf, '`') != रिक्त)
	    जारी;

	यदि (buf[0] == '.' && (buf[1] == NUL || vim_ispathsep(buf[1])))
	{
	    माप_प्रकार  plen;

	    // Relative to current buffer:
	    // "/path/file" + "." -> "/path/"
	    // "/path/file"  + "./subdir" -> "/path/subdir"
	    यदि (curbuf->b_ffname == रिक्त)
		जारी;
	    p = gettail(curbuf->b_ffname);
	    plen = (माप_प्रकार)(p - curbuf->b_ffname);
	    यदि (plen + buflen >= MAXPATHL)
		जारी;
	    यदि (buf[1] == NUL)
		buf[plen] = NUL;
	    अन्यथा
		mch_memmove(buf + plen, buf + 2, (buflen - 2) + 1); // +1 for NUL
	    mch_memmove(buf, curbuf->b_ffname, plen);
	    buflen = simplify_filename(buf);
	}
	अन्यथा यदि (buf[0] == NUL)
	{
	    // relative to current directory
	    STRCPY(buf, curdir);
	    यदि (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    buflen = curdirlen;
	}
	अन्यथा यदि (path_with_url(buf))
	    // URL can't be used here
	    जारी;
	अन्यथा यदि (!mch_isFullName(buf))
	{
	    // Expand relative path to their full path equivalent
	    यदि (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    यदि (curdirlen + buflen + 3 > MAXPATHL)
		जारी;

	    mch_memmove(buf + curdirlen + 1, buf, buflen + 1);	    // +1 for NUL
	    STRCPY(buf, curdir);
	    buf[curdirlen] = PATHSEP;
	    buflen = simplify_filename(buf);
	}

	यदि (ga_grow(gap, 1) == FAIL)
	    रोक;

#यदि defined(MSWIN)
	// Avoid the path ending in a backslash, it fails when a comma is
	// appended.
	यदि (buf[buflen - 1] == '\\')
	    buf[buflen - 1] = '/';
#endif

	p = vim_strnsave(buf, buflen);
	यदि (p == रिक्त)
	    रोक;
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
    स्थिर char_u *
get_path_cutoff(char_u *fname, garray_T *gap)
{
    पूर्णाङ्क	    i;
    पूर्णाङ्क	    maxlen = 0;
    char_u  **path_part = (char_u **)gap->ga_data;
    char_u  *cutoff = रिक्त;

    लागि (i = 0; i < gap->ga_len; i++)
    {
	पूर्णाङ्क j = 0;

	जबसम्म ((fname[j] == path_part[i][j]
#यदि defined(MSWIN)
		|| (vim_ispathsep(fname[j]) && vim_ispathsep(path_part[i][j]))
#endif
			     ) && fname[j] != NUL && path_part[i][j] != NUL)
	    j++;
	यदि (j > maxlen)
	{
	    maxlen = j;
	    cutoff = &fname[j];
	}
    }

    // skip to the file or directory name
    यदि (cutoff != रिक्त)
	जबसम्म (vim_ispathsep(*cutoff))
	    MB_PTR_ADV(cutoff);

    फर्काऊ cutoff;
}

/*
 * Sorts, removes duplicates and modifies all the fullpath names in "gap" so
 * that they are unique with respect to each other while conserving the part
 * that matches the pattern. Beware, this is at least O(n^2) wrt "gap->ga_len".
 */
    शून्य
uniquefy_paths(
	garray_T *gap,
	char_u *pattern,
	char_u *path_option)	// p_path or p_cdpath
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		len;
    char_u	**fnames = (char_u **)gap->ga_data;
    पूर्णाङ्क		sort_again = FALSE;
    char_u	*pat;
    char_u      *file_pattern;
    char_u	*curdir;
    regmatch_T	regmatch;
    garray_T	path_ga;
    char_u	**in_curdir = रिक्त;
    char_u	*short_name;

    remove_duplicates(gap);
    ga_init2(&path_ga, परिमाण(char_u *), 1);

    /*
     * We need to prepend a '*' at the beginning of file_pattern so that the
     * regex matches anywhere in the path. FIXME: is this valid for all
     * possible patterns?
     */
    len = (पूर्णाङ्क)STRLEN(pattern);
    file_pattern = alloc(len + 2);
    यदि (file_pattern == रिक्त)
	फर्काऊ;
    file_pattern[0] = '*';
    STRCPY(file_pattern + 1, pattern);
    pat = file_pat_to_reg_pat(file_pattern, रिक्त, रिक्त, FALSE);
    vim_free(file_pattern);
    यदि (pat == रिक्त)
	फर्काऊ;

    regmatch.rm_ic = TRUE;		// always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    vim_free(pat);
    यदि (regmatch.regprog == रिक्त)
	फर्काऊ;

    यदि ((curdir = alloc(MAXPATHL)) == रिक्त)
	गच्छ theend;
    mch_dirname(curdir, MAXPATHL);
    expand_path_option(curdir, path_option, &path_ga);

    in_curdir = ALLOC_CLEAR_MULT(char_u *, gap->ga_len);
    यदि (in_curdir == रिक्त)
	गच्छ theend;

    लागि (i = 0; i < gap->ga_len && !got_int; i++)
    {
	char_u	    *path = fnames[i];
	पूर्णाङ्क	    is_in_curdir;
	char_u	    *dir_end = gettail_dir(path);
	char_u	    *path_cutoff;

	len = (पूर्णाङ्क)STRLEN(path);
	is_in_curdir = fnamencmp(curdir, path, dir_end - path) == 0
					     && curdir[dir_end - path] == NUL;
	यदि (is_in_curdir)
	    in_curdir[i] = vim_strnsave(path, len);

	// Shorten the filename while maintaining its uniqueness
	path_cutoff = get_path_cutoff(path, &path_ga);

	// Don't assume all files can be reached without path when search
	// pattern starts with star star slash, so only remove path_cutoff
	// when possible.
	यदि (pattern[0] == '*' && pattern[1] == '*'
		&& vim_ispathsep_nocolon(pattern[2])
		&& path_cutoff != रिक्त
		&& vim_regexec(&regmatch, path_cutoff, (colnr_T)0)
		&& is_unique(path_cutoff, gap, i))
	{
	    sort_again = TRUE;
	    mch_memmove(path, path_cutoff, STRLEN(path_cutoff) + 1);
	}
	अन्यथा
	{
	    // Here all files can be reached without path, so get shortest
	    // unique path.  We start at the end of the path.
	    char_u  *pathsep_p = path + len - 1;

	    जबसम्म (find_previous_pathsep(path, &pathsep_p))
	    {
		यदि (vim_regexec(&regmatch, pathsep_p + 1, (colnr_T)0)
			&& is_unique(pathsep_p + 1, gap, i)
			&& path_cutoff != रिक्त && pathsep_p + 1 >= path_cutoff)
		{
		    sort_again = TRUE;
		    mch_memmove(path, pathsep_p + 1,
			    (माप_प्रकार)((path + len) - (pathsep_p + 1)) + 1);  // +1 for NUL
		    रोक;
		}
	    }
	}

	यदि (mch_isFullName(path))
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
	    यदि (short_name != रिक्त && short_name > path + 1
#यदि defined(MSWIN)
		    // On windows,
		    //	    shorten_fname("c:\a\a.txt", "c:\a\b")
		    // returns "\a\a.txt", which is not really the short
		    // name, hence:
		    && !vim_ispathsep(*short_name)
#endif
		)
	    {
		vim_snprintf((वर्ण *)path, MAXPATHL, ".%s%s", PATHSEPSTR, short_name);
	    }
	}
	ui_breakcheck();
    }

    // Shorten filenames in /in/current/directory/{filename}
    लागि (i = 0; i < gap->ga_len && !got_int; i++)
    {
	माप_प्रकार	rel_pathsize;
	char_u	*rel_path;
	char_u	*path = in_curdir[i];

	यदि (path == रिक्त)
	    जारी;

	// If the {filename} is not unique, change it to ./{filename}.
	// Else reduce it to {filename}
	short_name = shorten_fname(path, curdir);
	यदि (short_name == रिक्त)
	    short_name = path;
	यदि (is_unique(short_name, gap, i))
	{
	    STRCPY(fnames[i], short_name);
	    जारी;
	}

	rel_pathsize = 1 + परिमाण(PATHSEP) + STRLEN(short_name) + 1;
	rel_path = alloc(rel_pathsize);
	यदि (rel_path == रिक्त)
	    गच्छ theend;

	vim_snprintf((वर्ण *)rel_path, rel_pathsize, ".%s%s", PATHSEPSTR, short_name);

	vim_free(fnames[i]);
	fnames[i] = rel_path;
	sort_again = TRUE;
	ui_breakcheck();
    }

theend:
    vim_free(curdir);
    यदि (in_curdir != रिक्त)
    {
	लागि (i = 0; i < gap->ga_len; i++)
	    vim_free(in_curdir[i]);
	vim_free(in_curdir);
    }
    ga_clear_strings(&path_ga);
    vim_regfree(regmatch.regprog);

    यदि (sort_again)
	remove_duplicates(gap);
}

/*
 * Calls globpath() with 'path' values for the given pattern and stores the
 * result in "gap".
 * Returns the total number of matches.
 */
    पूर्णाङ्क
expand_in_path(
    garray_T	*gap,
    char_u	*pattern,
    पूर्णाङ्क		flags)		// EW_* flags
{
    char_u	*curdir;
    garray_T	path_ga;
    char_u	*paths = रिक्त;
    पूर्णाङ्क		glob_flags = 0;
    char_u	*path_option = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;

    यदि ((curdir = alloc(MAXPATHL)) == रिक्त)
	फर्काऊ 0;
    mch_dirname(curdir, MAXPATHL);

    ga_init2(&path_ga, परिमाण(char_u *), 1);
    यदि (flags & EW_CDPATH)
	expand_path_option(curdir, p_cdpath, &path_ga);
    अन्यथा
	expand_path_option(curdir, path_option, &path_ga);
    vim_free(curdir);
    यदि (path_ga.ga_len == 0)
	फर्काऊ 0;

    paths = ga_concat_strings(&path_ga, ",");
    ga_clear_strings(&path_ga);
    यदि (paths == रिक्त)
	फर्काऊ 0;

    यदि (flags & EW_ICASE)
	glob_flags |= WILD_ICASE;
    यदि (flags & EW_ADDSLASH)
	glob_flags |= WILD_ADD_SLASH;
    globpath(paths, pattern, gap, glob_flags, !!(flags & EW_CDPATH));
    vim_free(paths);

    फर्काऊ gap->ga_len;
}


/*
 * Converts a file name into a canonical form. It simplifies a file name into
 * its simplest form by stripping out unneeded components, if any.  The
 * resulting file name is simplified in place and will either be the same
 * length as that supplied, or shorter.
 */
    माप_प्रकार
simplify_filename(char_u *filename)
{
#ifndef AMIGA	    // Amiga doesn't have "..", it uses "/"
    पूर्णाङ्क		components = 0;
    char_u	*p, *tail, *start;
    char_u	*p_end;			    // point to NUL at end of string "p"
    पूर्णाङ्क		stripping_disabled = FALSE;
    पूर्णाङ्क		relative = TRUE;

    p = filename;
# ifdef BACKSLASH_IN_FILENAME
    यदि (p[0] != NUL && p[1] == ':')	    // skip "x:"
	p += 2;
# endif

    यदि (vim_ispathsep(*p))
    {
	relative = FALSE;
	गर
	    ++p;
	जबसम्म (vim_ispathsep(*p));
    }
    start = p;	    // remember start after "c:/" or "/" or "///"
    p_end = p + STRLEN(p);
# ifdef UNIX
    // Posix says that "//path" is unchanged but "///path" is "/path".
    यदि (start > filename + 2)
    {
	mch_memmove(filename + 1, p, (माप_प्रकार)(p_end - p) + 1);	    // +1 for NUL
	p_end -= (माप_प्रकार)(p - (filename + 1));
	start = p = filename + 1;
    }
# endif

    गर
    {
	// At this point "p" is pointing to the char following a single "/"
	// or "p" is at the "start" of the (absolute or relative) path name.
# ifdef VMS
	// VMS allows device:[path] - don't strip the [ in directory
	यदि ((*p == '[' || *p == '<') && p > filename && p[-1] == ':')
	{
	    // :[ or :< composition: vms directory component
	    ++components;
	    p = getnextcomp(p + 1);
	}
	// allow remote calls as host"user passwd"::device:[path]
	अन्यथा यदि (p[0] == ':' && p[1] == ':' && p > filename && p[-1] == '"' )
	{
	    // ":: composition: vms host/passwd component
	    ++components;
	    p = getnextcomp(p + 2);
	}
	अन्यथा
# endif
	यदि (vim_ispathsep(*p))
	{
	    mch_memmove(p, p + 1, (माप_प्रकार)(p_end - (p + 1)) + 1); // remove duplicate "/"
	    --p_end;
	}
	अन्यथा यदि (p[0] == '.' && (vim_ispathsep(p[1]) || p[1] == NUL))
	{
	    यदि (p == start && relative)
		p += 1 + (p[1] != NUL);	// keep single "." or leading "./"
	    अन्यथा
	    {
		// Strip "./" or ".///".  If we are at the end of the file name
		// and there is no trailing path separator, either strip "/." if
		// we are after "start", or strip "." if we are at the beginning
		// of an absolute path name .
		tail = p + 1;
		यदि (p[1] != NUL)
		    जबसम्म (vim_ispathsep(*tail))
			MB_PTR_ADV(tail);
		अन्यथा यदि (p > start)
		    --p;		// strip preceding path separator

		mch_memmove(p, tail, (माप_प्रकार)(p_end - tail) + 1);
		p_end -= (माप_प्रकार)(tail - p);
	    }
	}
	अन्यथा यदि (p[0] == '.' && p[1] == '.' &&
	    (vim_ispathsep(p[2]) || p[2] == NUL))
	{
	    // Skip to after ".." or "../" or "..///".
	    tail = p + 2;
	    जबसम्म (vim_ispathsep(*tail))
		MB_PTR_ADV(tail);

	    यदि (components > 0)		// strip one preceding component
	    {
		पूर्णाङ्क		do_strip = FALSE;
		char_u		saved_char;
		stat_T		st;

		// Don't strip for an erroneous file name.
		यदि (!stripping_disabled)
		{
		    // If the preceding component does not exist in the file
		    // system, we strip it.  On Unix, we don't accept a symbolic
		    // link that refers to a non-existent file.
		    saved_char = p[-1];
		    p[-1] = NUL;
# ifdef UNIX
		    यदि (mch_lstat((वर्ण *)filename, &st) < 0)
# अन्यथा
			यदि (mch_stat((वर्ण *)filename, &st) < 0)
# endif
			    do_strip = TRUE;
		    p[-1] = saved_char;

		    --p;
		    // Skip back to after previous '/'.
		    जबसम्म (p > start && !after_pathsep(start, p))
			MB_PTR_BACK(start, p);

		    यदि (!do_strip)
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
			यदि (mch_stat((वर्ण *)filename, &st) >= 0)
			    do_strip = TRUE;
			अन्यथा
			    stripping_disabled = TRUE;
			*tail = saved_char;
# ifdef UNIX
			यदि (do_strip)
			{
			    stat_T	new_st;

			    // On Unix, the check for the unstripped file name
			    // above works also for a symbolic link pointing to
			    // a searchable directory.  But then the parent of
			    // the directory pointed to by the link must be the
			    // same as the stripped file name.  (The latter
			    // exists in the file system since it is the
			    // component's parent directory.)
			    यदि (p == start && relative)
				(शून्य)mch_stat(".", &new_st);
			    अन्यथा
			    {
				saved_char = *p;
				*p = NUL;
				(शून्य)mch_stat((वर्ण *)filename, &new_st);
				*p = saved_char;
			    }

			    यदि (new_st.st_ino != st.st_ino ||
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

		यदि (!do_strip)
		{
		    // Skip the ".." or "../" and reset the counter for the
		    // components that might be stripped later on.
		    p = tail;
		    components = 0;
		}
		अन्यथा
		{
		    // Strip previous component.  If the result would get empty
		    // and there is no trailing path separator, leave a single
		    // "." instead.  If we are at the end of the file name and
		    // there is no trailing path separator and a preceding
		    // component is left after stripping, strip its trailing
		    // path separator as well.
		    यदि (p == start && relative && tail[-1] == '.')
		    {
			*p++ = '.';
			*p = NUL;
		    }
		    अन्यथा
		    {
			यदि (p > start && tail[-1] == '.')
			    --p;

			mch_memmove(p, tail, (माप_प्रकार)(p_end - tail) + 1);	// strip previous component
			p_end -= (माप_प्रकार)(tail - p);
		    }

		    --components;
		}
	    }
	    अन्यथा यदि (p == start && !relative)	// leading "/.." or "/../"
	    {
		mch_memmove(p, tail, (माप_प्रकार)(p_end - tail) + 1);		// strip ".." or "../"
		p_end -= (माप_प्रकार)(tail - p);
	    }
	    अन्यथा
	    {
		यदि (p == start + 2 && p[-2] == '.')	// leading "./../"
		{
		    mch_memmove(p - 2, p, (माप_प्रकार)(p_end - p) + 1); // strip leading "./"
		    p_end -= 2;
		    tail -= 2;
		}
		p = tail;		// skip to char after ".." or "../"
	    }
	}
	अन्यथा
	{
	    ++components;		// simple path component
	    p = getnextcomp(p);
	}
    } जबसम्म (*p != NUL);

    फर्काऊ (माप_प्रकार)(p_end - filename);
#अन्यथा
    // Don't touch Amiga filenames
    फर्काऊ STRLEN(filename);
#endif // !AMIGA
}

#यदि defined(FEAT_EVAL)
/*
 * "simplify()" function
 */
    शून्य
f_simplify(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	फर्काऊ;

    p = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_string = vim_strsave(p);
    simplify_filename(rettv->vval.v_string);	// simplify in place
    rettv->v_type = VAR_STRING;
}
#endif // FEAT_EVAL
