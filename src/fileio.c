/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * fileio.c: read from and write to a file
 */

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(__TANDEM)
# include <limits.h>		// for SSIZE_MAX
#అయితే_ముగింపు
#ఒకవేళ (defined(UNIX) || defined(VMS)) && defined(FEAT_EVAL)
# include <pwd.h>
# include <grp.h>
#అయితే_ముగింపు
#ఒకవేళ defined(VMS) && defined(HAVE_XOS_R_H)
# include <x11/xos_r.h>
#అయితే_ముగింపు

// Is there any system that doesn't have access()?
#నిర్వచించు USE_MCH_ACCESS

// Bitmask with 0x80 set in each byte of a long_u word, used to detect
// non-ASCII bytes (high bit set) in multiple bytes at once.
#నిర్వచించు NONASCII_ముసుగు (((long_u)-1 / 0xFF) * 0x80)

#ఒకవేళ defined(__hpux) && !defined(HAVE_DIRFD)
# define dirfd(x) ((x)->__dd_fd)
# define HAVE_DIRFD
#అయితే_ముగింపు

స్థిర char_u *తదుపరి_fenc(char_u **pp, పూర్ణాంకం *alloced);
#నిర్వచితమైతే FEAT_EVAL
స్థిర char_u *readfile_charconvert(char_u *fname, char_u *fenc, పూర్ణాంకం *fdp);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CRYPT
స్థిర char_u *check_for_cryptkey(char_u *cryptkey, char_u *ptr, దీర్ఘ *sizep, off_T *filesizep, పూర్ణాంకం newfile, char_u *fname, పూర్ణాంకం *did_ask);
#అయితే_ముగింపు
స్థిర linenr_T readfile_linenr(linenr_T linecnt, char_u *p, char_u *endp);
స్థిర char_u *check_for_bom(char_u *p, దీర్ఘ కొలత, పూర్ణాంకం *lenp, పూర్ణాంకం flags);

#నిర్వచితమైతే FEAT_EVAL
స్థిర పూర్ణాంకం readdirex_క్రమీకరించు;
#అయితే_ముగింపు

    శూన్య
filemess(
    buf_T	*buf,
    char_u	*name,
    char_u	*s,
    పూర్ణాంకం		attr)
{
    పూర్ణాంకం		సందేశం_scroll_save;
    పూర్ణాంకం		prev_సందేశం_నిలువు = సందేశం_నిలువు;
    size_t	len;

    ఒకవేళ (సందేశం_silent != 0)
	తిరిగి;
    సందేశం_కూడు_fname(buf, name);	    // put file name in IObuff with quotes

    // If it's extremely long, truncate it.
    len = STRLEN(IObuff);
    ఒకవేళ (len > IOSIZE - 100)
    {
	len = IOSIZE - 100;
	IObuff[len] = NUL;
    }

    // Avoid an over-long translation to cause trouble.
    ఒకవేళ (*s != NUL)
	STRNCPY(IObuff + len, s, 99);

    /*
     * For the first message may have to start a new line.
     * For further ones overwrite the previous one, reset msg_scroll before
     * calling filemess().
     */
    సందేశం_scroll_save = సందేశం_scroll;
    ఒకవేళ (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	సందేశం_scroll = FALSE;
    ఒకవేళ (!సందేశం_scroll)	// wait a bit when overwriting an error msg
	check_for_delay(FALSE);
    సందేశం_ప్రారంభం();
    ఒకవేళ (prev_సందేశం_నిలువు != 0 && సందేశం_నిలువు == 0)
	సందేశం_putchar('\r');  // overwrite any previous message.
    సందేశం_scroll = సందేశం_scroll_save;
    సందేశం_scrolled_ign = TRUE;
    // may truncate the message to avoid a hit-return prompt
    సందేశం_outtrans_attr(సందేశం_may_trunc(FALSE, IObuff), attr);
    సందేశం_clr_eos();
    బహిర్గతం_flush();
    సందేశం_scrolled_ign = FALSE;
}

/*
 * Read lines from file "fname" into the buffer after line "from".
 *
 * 1. We allocate blocks with lalloc, as big as possible.
 * 2. Each block is filled with characters from the file with a single read().
 * 3. The lines are inserted in the buffer with ml_append().
 *
 * (caller must check that fname != NULL, unless READ_STDIN is used)
 *
 * "lines_to_skip" is the number of lines that must be skipped
 * "lines_to_read" is the number of lines that are appended
 * When not recovering lines_to_skip is 0 and lines_to_read MAXLNUM.
 *
 * flags:
 * READ_NEW	starting to edit a new buffer
 * READ_FILTER	reading filter output
 * READ_STDIN	read from stdin instead of a file
 * READ_BUFFER	read from curbuf instead of a file (converting after reading
 *		stdin)
 * READ_NOFILE	do not read a file, only trigger BufReadCmd
 * READ_DUMMY	read into a dummy buffer (to check if file contents changed)
 * READ_KEEP_UNDO  don't clear undo info or read it from a file
 * READ_FIFO	read from fifo/socket instead of a file
 *
 * return FAIL for failure, NOTDONE for directory (failure), or OK
 */
    పూర్ణాంకం
readfile(
    char_u	*fname,
    char_u	*sfname,
    linenr_T	from,
    linenr_T	పంక్తులు_గమ్యం_skip,
    linenr_T	పంక్తులు_గమ్యం_read,
    exarg_T	*eap,			// can be NULL!
    పూర్ణాంకం		flags)
{
    పూర్ణాంకం		retval = FAIL;	// jump to "theend" instead of returning
    పూర్ణాంకం		fd = 0;
    పూర్ణాంకం		newfile = (flags & READ_NEW);
    పూర్ణాంకం		check_readonly;
    పూర్ణాంకం		filtering = (flags & READ_FILTER);
    పూర్ణాంకం		read_stdin = (flags & READ_STDIN);
    పూర్ణాంకం		read_బఫరు = (flags & READ_బఫరు);
    పూర్ణాంకం		read_fifo = (flags & READ_FIFO);
    పూర్ణాంకం		సమితి_options = newfile || read_బఫరు
					   || (eap != ఖాళీ && eap->read_edit);
    linenr_T	read_buf_lnum = 1;	// next line to read from curbuf
    colnr_T	read_buf_నిలువు = 0;	// next char to read from this line
    char_u	c;
    linenr_T	lnum = from;
    char_u	*ptr = ఖాళీ;		// pointer into read buffer
    char_u	*బఫరు = ఖాళీ;		// read buffer
    char_u	*new_బఫరు = ఖాళీ;	// init to shut up gcc
    char_u	*పంక్తి_ప్రారంభం = ఖాళీ;	// init to shut up gcc
    పూర్ణాంకం		wasempty;		// buffer was empty before reading
    colnr_T	len;
    దీర్ఘ	కొలత = 0;
    char_u	*p;
    off_T	filesize = 0;
    పూర్ణాంకం		skip_read = FALSE;
#నిర్వచితమైతే FEAT_CRYPT
    off_T       filesize_disk = 0;      // file size read from disk
    off_T       filesize_లెక్క = 0;     // counter
    char_u	*cryptkey = ఖాళీ;
    పూర్ణాంకం		did_ask_for_కీలకం = FALSE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
    context_sha256_T sha_ctx;
    పూర్ణాంకం		read_undo_file = FALSE;
#అయితే_ముగింపు
    పూర్ణాంకం		విభజించు = 0;		// number of split lines
#నిర్వచించు UNKNOWN	 0x0fffffff		// file size is unknown
    linenr_T	linecnt;
    పూర్ణాంకం		error = FALSE;		// errors encountered
    పూర్ణాంకం		ff_error = EOL_UNKNOWN; // file format with errors
    దీర్ఘ	linerest = 0;		// remaining chars in line
#నిర్వచితమైతే UNIX
    పూర్ణాంకం		perm = 0;
    పూర్ణాంకం		మార్పిడి_బహుళకం = -1;		// protection bits for swap file
#లేకపోతే
    పూర్ణాంకం		perm;
#అయితే_ముగింపు
    పూర్ణాంకం		fileformat = 0;		// end-of-line format
    పూర్ణాంకం		keep_fileformat = FALSE;
    stat_T	st;
    పూర్ణాంకం		file_readonly;
    linenr_T	skip_లెక్క = 0;
    linenr_T	read_లెక్క = 0;
    పూర్ణాంకం		సందేశం_save = సందేశం_scroll;
    linenr_T	read_no_eol_lnum = 0;   // non-zero lnum when last line of
					// last read was missing the eol
    పూర్ణాంకం		try_mac;
    పూర్ణాంకం		try_dos;
    పూర్ణాంకం		try_unix;
    పూర్ణాంకం		file_rewind = FALSE;
    పూర్ణాంకం		can_retry;
    linenr_T	conv_error = 0;		// line nr with conversion error
    linenr_T	illegal_byte = 0;	// line nr with illegal byte
    పూర్ణాంకం		keep_dest_enc = FALSE;	// don't retry when char doesn't fit
					// in destination encoding
    పూర్ణాంకం		bad_char_behavior = BAD_REPLACE;
					// BAD_KEEP, BAD_DROP or character to
					// replace with
    char_u	*tmpname = ఖాళీ;	// name of 'charconvert' output file
    పూర్ణాంకం		fio_flags = 0;
    char_u	*fenc;			// fileencoding to use
    పూర్ణాంకం		fenc_alloced;		// fenc_next is in allocated memory
    char_u	*fenc_తదుపరి = ఖాళీ;	// next item in 'fencs' or NULL
    పూర్ణాంకం		advance_fenc = FALSE;
    దీర్ఘ	real_కొలత = 0;
#నిర్వచితమైతే USE_ICONV
    iconv_t	iconv_fd = (iconv_t)-1;	// descriptor for iconv() or -1
# ifdef FEAT_EVAL
    పూర్ణాంకం		did_iconv = FALSE;	// TRUE when iconv() failed and trying
					// 'charconvert' next
# endif
#అయితే_ముగింపు
    పూర్ణాంకం		converted = FALSE;	// TRUE if conversion done
    పూర్ణాంకం		notconverted = FALSE;	// TRUE if conversion wanted but it
					// wasn't possible
    char_u	conv_rest[CONV_RESTLEN];
    పూర్ణాంకం		conv_restlen = 0;	// nr of bytes in conv_rest[]
    pos_T	orig_ప్రారంభం;
    buf_T	*old_curbuf;
    char_u	*old_b_ffname;
    char_u	*old_b_fname;
    పూర్ణాంకం		using_b_ffname;
    పూర్ణాంకం		using_b_fname;
    స్థిర అక్షరం *సందేశం_is_a_directory = N_("is a directory");
#నిర్వచితమైతే FEAT_CRYPT
    పూర్ణాంకం		eof = FALSE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SODIUM
    పూర్ణాంకం		may_need_lseek = FALSE;
#అయితే_ముగింపు
    size_t	fnamelen = 0;

    curbuf->b_au_did_filetype = false; // reset before triggering any autocommands

    curbuf->b_no_eol_lnum = 0;	// in case it was set by the previous read

    /*
     * If there is no file name yet, use the one for the read file.
     * BF_NOTEDITED is set to reflect this.
     * Don't do this for a read from a filter.
     * Only do this when 'cpoptions' contains the 'f' flag.
     */
    ఒకవేళ (curbuf->b_ffname == ఖాళీ
	    && !filtering
	    && fname != ఖాళీ
	    && vim_strchr(p_cpo, CPO_FNAMER) != ఖాళీ
	    && !(flags & READ_DUMMY))
    {
	ఒకవేళ (సమితి_rw_fname(fname, sfname) == FAIL)
	    వెళ్ళు theend;
    }

    // Remember the initial values of curbuf, curbuf->b_ffname and
    // curbuf->b_fname to detect whether they are altered as a result of
    // executing nasty autocommands.  Also check if "fname" and "sfname"
    // point to one of these values.
    old_curbuf = curbuf;
    old_b_ffname = curbuf->b_ffname;
    old_b_fname = curbuf->b_fname;
    using_b_ffname = (fname == curbuf->b_ffname)
					      || (sfname == curbuf->b_ffname);
    using_b_fname = (fname == curbuf->b_fname) || (sfname == curbuf->b_fname);

    // After reading a file the cursor line changes but we don't want to
    // display the line.
    ex_no_reprint = TRUE;

    // don't display the file info for another buffer now
    need_fileinfo = FALSE;

    /*
     * For Unix: Use the short file name whenever possible.
     * Avoids problems with networks and when directory names are changed.
     * Don't do this for MS-DOS, a "cd" in a sub-shell may have moved us to
     * another directory, which we don't detect.
     */
    ఒకవేళ (sfname == ఖాళీ)
	sfname = fname;
#ఒకవేళ defined(UNIX)
    fname = sfname;
#అయితే_ముగింపు

    /*
     * The BufReadCmd and FileReadCmd events intercept the reading process by
     * executing the associated commands instead.
     */
    ఒకవేళ (!filtering && !read_stdin && !read_బఫరు)
    {
	orig_ప్రారంభం = curbuf->b_op_ప్రారంభం;

	// Set '[ mark to the line above where the lines go (line 1 if zero).
	curbuf->b_op_ప్రారంభం.lnum = ((from == 0) ? 1 : from);
	curbuf->b_op_ప్రారంభం.నిలువు = 0;

	ఒకవేళ (newfile)
	{
	    ఒకవేళ (apply_autocmds_exarg(EVENT_BUFREADCMD, ఖాళీ, sfname,
							  FALSE, curbuf, eap))
	    {
		retval = OK;
#నిర్వచితమైతే FEAT_EVAL
		ఒకవేళ (aborting())
		    retval = FAIL;
#అయితే_ముగింపు
		// The BufReadCmd code usually uses ":read" to get the text and
		// perhaps ":file" to change the buffer name. But we should
		// consider this to work like ":edit", thus reset the
		// BF_NOTEDITED flag.  Then ":write" will work to overwrite the
		// same file.
		ఒకవేళ (retval == OK)
		    curbuf->b_flags &= ~BF_NOTEDITED;
		వెళ్ళు theend;
	    }
	}
	లేకపోతే ఒకవేళ (apply_autocmds_exarg(EVENT_FILEREADCMD, sfname, sfname,
							    FALSE, ఖాళీ, eap))
	{
#నిర్వచితమైతే FEAT_EVAL
	    retval = aborting() ? FAIL : OK;
#లేకపోతే
	    retval = OK;
#అయితే_ముగింపు
	    వెళ్ళు theend;
	}

	curbuf->b_op_ప్రారంభం = orig_ప్రారంభం;

	ఒకవేళ (flags & READ_NOFILE)
	{
	    // Return NOTDONE instead of FAIL so that BufEnter can be triggered
	    // and other operations don't fail.
	    retval = NOTDONE;
	    వెళ్ళు theend;
	}
    }

    ఒకవేళ ((shortmess(SHM_OVER) || curbuf->b_help) && p_verbose == 0)
	సందేశం_scroll = FALSE;	// overwrite previous file message
    లేకపోతే
	సందేశం_scroll = TRUE;	// don't overwrite previous file message

    ఒకవేళ (fname != ఖాళీ && *fname != NUL)
    {
	fnamelen = STRLEN(fname);

	// If the name is too long we might crash further on, quit here.
	ఒకవేళ (fnamelen >= MAXPATHL)
	{
	    filemess(curbuf, fname, (char_u *)_("Illegal file name"), 0);
	    సందేశం_చివర();
	    సందేశం_scroll = సందేశం_save;
	    వెళ్ళు theend;
	}

	// If the name ends in a path separator, we can't open it.  Check here,
	// because reading the file may actually work, but then creating the
	// swap file may destroy it!  Reported on MS-DOS and Win 95.
	ఒకవేళ (after_pathsep(fname, fname + fnamelen))
	{
	    filemess(curbuf, fname, (char_u *)_(సందేశం_is_a_directory), 0);
	    సందేశం_చివర();
	    సందేశం_scroll = సందేశం_save;
	    retval = NOTDONE;
	    వెళ్ళు theend;
	}
    }
#ఒకవేళ defined(UNIX) || defined(VMS)
    ఒకవేళ (!read_stdin && fname != ఖాళీ)
	perm = mch_getperm(fname);
#అయితే_ముగింపు

    ఒకవేళ (!read_stdin && !read_బఫరు && !read_fifo)
    {
#ఒకవేళ defined(UNIX) || defined(VMS)
	ఒకవేళ (perm >= 0 && !S_ISREG(perm)		    // not a regular file ...
		      && !S_ISFIFO(perm)	    // ... or fifo
		      && !S_ISSOCK(perm)	    // ... or socket
# ifdef OPEN_CHR_FILES
		      && !(S_ISCHR(perm) && is_dev_fd_file(fname))
			// ... or a character special file named /dev/fd/<n>
# endif
						)
	{
	    /*
	     * On Unix it is possible to read a directory, so we have to
	     * check for it before the mch_open().
	     */
	    ఒకవేళ (S_ISDIR(perm))
	    {
		filemess(curbuf, fname, (char_u *)_(సందేశం_is_a_directory), 0);
		retval = NOTDONE;
	    }
	    లేకపోతే
		filemess(curbuf, fname, (char_u *)_("is not a file"), 0);
	    సందేశం_చివర();
	    సందేశం_scroll = సందేశం_save;
	    వెళ్ళు theend;
	}
#అయితే_ముగింపు
#ఒకవేళ defined(MSWIN)
	/*
	 * MS-Windows allows opening a device, but we will probably get stuck
	 * trying to read it.
	 */
	ఒకవేళ (!p_odev && mch_nodetype(fname) == కణుపు_WRITABLE)
	{
	    filemess(curbuf, fname, (char_u *)_("is a device (disabled with 'opendevice' option)"), 0);
	    సందేశం_చివర();
	    సందేశం_scroll = సందేశం_save;
	    వెళ్ళు theend;
	}
#అయితే_ముగింపు
    }

    // Set default or forced 'fileformat' and 'binary'.
    సమితి_file_options(సమితి_options, eap);

    /*
     * When opening a new file we take the readonly flag from the file.
     * Default is r/w, can be set to r/o below.
     * Don't reset it when in readonly mode
     * Only set/reset b_p_ro when BF_CHECK_RO is set.
     */
    check_readonly = (newfile && (curbuf->b_flags & BF_CHECK_RO));
    ఒకవేళ (check_readonly && !readonlymode)
	curbuf->b_p_ro = FALSE;

    ఒకవేళ (newfile && !read_stdin && !read_బఫరు && !read_fifo)
    {
	// Remember time of file.
	ఒకవేళ (mch_stat((అక్షరం *)fname, &st) >= 0)
	{
	    buf_store_సమయం(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
#నిర్వచితమైతే FEAT_CRYPT
	    filesize_disk = st.st_size;
#అయితే_ముగింపు
#నిర్వచితమైతే UNIX
	    /*
	     * Use the protection bits of the original file for the swap file.
	     * This makes it possible for others to read the name of the
	     * edited file from the swapfile, but only if they can read the
	     * edited file.
	     * Remove the "write" and "execute" bits for group and others
	     * (they must not write the swapfile).
	     * Add the "read" and "write" bits for the user, otherwise we may
	     * not be able to write to the file ourselves.
	     * Setting the bits is done below, after creating the swap file.
	     */
	    మార్పిడి_బహుళకం = (st.st_బహుళకం & 0644) | 0600;
#అయితే_ముగింపు
#నిర్వచితమైతే VMS
	    curbuf->b_fab_rfm = st.st_fab_rfm;
	    curbuf->b_fab_rat = st.st_fab_rat;
	    curbuf->b_fab_mrs = st.st_fab_mrs;
#అయితే_ముగింపు
	}
	లేకపోతే
	{
	    curbuf->b_mtime = 0;
	    curbuf->b_mtime_ns = 0;
	    curbuf->b_mtime_read = 0;
	    curbuf->b_mtime_read_ns = 0;
	    curbuf->b_orig_కొలత = 0;
	    curbuf->b_orig_బహుళకం = 0;
	}

	// Reset the "new file" flag.  It will be set again below when the
	// file doesn't exist.
	curbuf->b_flags &= ~(BF_NEW | BF_NEW_W);
    }

/*
 * for UNIX: check readonly with perm and mch_access()
 * for Amiga: check readonly by trying to open the file for writing
 */
    file_readonly = FALSE;
    ఒకవేళ (read_stdin)
    {
#ఒకవేళ defined(MSWIN)
	// Force binary I/O on stdin to avoid CR-LF -> LF conversion.
	setmode(0, O_ద్విమాన);
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (!read_బఫరు)
    {
#నిర్వచితమైతే USE_MCH_ACCESS
	ఒకవేళ (
# ifdef UNIX
	    !(perm & 0222) ||
# endif
				mch_access((అక్షరం *)fname, W_OK))
	    file_readonly = TRUE;
	fd = mch_open((అక్షరం *)fname, O_RDONLY | O_EXTRA, 0);
#లేకపోతే
	ఒకవేళ (!newfile
		|| readonlymode
		|| (fd = mch_open((అక్షరం *)fname, O_RDWR | O_EXTRA, 0)) < 0)
	{
	    file_readonly = TRUE;
	    // try to open ro
	    fd = mch_open((అక్షరం *)fname, O_RDONLY | O_EXTRA, 0);
	}
#అయితే_ముగింపు
    }

    ఒకవేళ (fd < 0)			    // cannot open at all
    {
#నిర్వచితం_కాకపోతే UNIX
	పూర్ణాంకం	isdir_f;
#అయితే_ముగింపు
	సందేశం_scroll = సందేశం_save;
#నిర్వచితం_కాకపోతే UNIX
	/*
	 * On Amiga we can't open a directory, check here.
	 */
	isdir_f = (mch_isdir(fname));
	perm = mch_getperm(fname);  // check if the file exists
	ఒకవేళ (isdir_f)
	{
	    filemess(curbuf, sfname, (char_u *)_(సందేశం_is_a_directory), 0);
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	}
	లేకపోతే
#అయితే_ముగింపు
	    ఒకవేళ (newfile)
	    {
		ఒకవేళ (perm < 0
#నిర్వచితమైతే ENOENT
			&& errno == ENOENT
#అయితే_ముగింపు
		   )
		{
		    /*
		     * Set the 'new-file' flag, so that when the file has
		     * been created by someone else, a ":w" will complain.
		     */
		    curbuf->b_flags |= BF_NEW;

		    // Create a swap file now, so that other Vims are warned
		    // that we are editing this file.  Don't do this for a
		    // "nofile" or "nowrite" buffer type.
		    ఒకవేళ (!bt_dontwrite(curbuf))
		    {
			check_need_మార్పిడి(newfile);
			// SwapExists autocommand may mess things up
			ఒకవేళ (curbuf != old_curbuf
				|| (using_b_ffname
					&& (old_b_ffname != curbuf->b_ffname))
				|| (using_b_fname
					 && (old_b_fname != curbuf->b_fname)))
			{
			    emsg(_(e_autocommands_మారింది_బఫరు_or_బఫరు_name));
			    వెళ్ళు theend;
			}
		    }
		    ఒకవేళ (dir_యొక్క_file_exists(fname))
			filemess(curbuf, sfname,
					      (char_u *)new_file_సందేశం(), 0);
		    లేకపోతే
			filemess(curbuf, sfname,
					   (char_u *)_("[New DIRECTORY]"), 0);
#నిర్వచితమైతే FEAT_VIMINFO
		    // Even though this is a new file, it might have been
		    // edited before and deleted.  Get the old marks.
		    check_marks_read();
#అయితే_ముగింపు
		    // Set forced 'fileencoding'.
		    ఒకవేళ (eap != ఖాళీ)
			సమితి_forced_fenc(eap);
		    apply_autocmds_exarg(EVENT_BUFNEWFILE, sfname, sfname,
							  FALSE, curbuf, eap);
		    // remember the current fileformat
		    save_file_ff(curbuf);

#ఒకవేళ defined(FEAT_EVAL)
		    ఒకవేళ (!aborting())   // autocmds may abort script processing
#అయితే_ముగింపు
			retval = OK;	    // a new file is not an error
		    వెళ్ళు theend;
		}
		లేకపోతే
		{
		    filemess(curbuf, sfname, (char_u *)(
#నిర్వచితమైతే EFBIG
			    (errno == EFBIG) ? _("[File too big]") :
#అయితే_ముగింపు
#నిర్వచితమైతే EOVERFLOW
			    (errno == EOVERFLOW) ? _("[File too big]") :
#అయితే_ముగింపు
						_("[Permission Denied]")), 0);
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
		}
	    }

	వెళ్ళు theend;
    }

    /*
     * Only set the 'ro' flag for readonly files the first time they are
     * loaded.	Help files always get readonly mode
     */
    ఒకవేళ ((check_readonly && file_readonly) || curbuf->b_help)
	curbuf->b_p_ro = TRUE;

    ఒకవేళ (సమితి_options)
    {
	// Don't change 'eol' if reading from buffer as it will already be
	// correctly set when reading stdin.
	ఒకవేళ (!read_బఫరు)
	{
	    curbuf->b_p_eof = FALSE;
	    curbuf->b_ప్రారంభం_eof = FALSE;
	    curbuf->b_p_eol = TRUE;
	    curbuf->b_ప్రారంభం_eol = TRUE;
	}
	curbuf->b_p_bomb = FALSE;
	curbuf->b_ప్రారంభం_bomb = FALSE;
    }

    // Create a swap file now, so that other Vims are warned that we are
    // editing this file.
    // Don't do this for a "nofile" or "nowrite" buffer type.
    ఒకవేళ (!bt_dontwrite(curbuf))
    {
	check_need_మార్పిడి(newfile);
	ఒకవేళ (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))))
	{
	    emsg(_(e_autocommands_మారింది_బఫరు_or_బఫరు_name));
	    ఒకవేళ (!read_బఫరు)
		close(fd);
	    వెళ్ళు theend;
	}
#నిర్వచితమైతే UNIX
	// Set swap file protection bits after creating it.
	ఒకవేళ (మార్పిడి_బహుళకం > 0 && curbuf->b_ml.ml_mfp != ఖాళీ
			  && curbuf->b_ml.ml_mfp->mf_fname != ఖాళీ)
	{
	    char_u *మార్పిడి_fname = curbuf->b_ml.ml_mfp->mf_fname;

	    /*
	     * If the group-read bit is set but not the world-read bit, then
	     * the group must be equal to the group of the original file.  If
	     * we can't make that happen then reset the group-read bit.  This
	     * avoids making the swap file readable to more users when the
	     * primary group of the user is too permissive.
	     */
	    ఒకవేళ ((మార్పిడి_బహుళకం & 044) == 040)
	    {
		stat_T	మార్పిడి_st;

		ఒకవేళ (mch_stat((అక్షరం *)మార్పిడి_fname, &మార్పిడి_st) >= 0
			&& st.st_gid != మార్పిడి_st.st_gid
# ifdef HAVE_FCHOWN
			&& fchown(curbuf->b_ml.ml_mfp->mf_fd, -1, st.st_gid)
									  == -1
# endif
		   )
		    మార్పిడి_బహుళకం &= 0600;
	    }

	    (శూన్య)mch_setperm(మార్పిడి_fname, (దీర్ఘ)మార్పిడి_బహుళకం);
	}
#అయితే_ముగింపు
    }

    // If "Quit" selected at ATTENTION dialog, don't load the file
    ఒకవేళ (మార్పిడి_exists_action == SEA_QUIT)
    {
	ఒకవేళ (!read_బఫరు && !read_stdin)
	    close(fd);
	వెళ్ళు theend;
    }

    ++no_wait_return;	    // don't wait for return yet

    /*
     * Set '[ mark to the line above where the lines go (line 1 if zero).
     */
    orig_ప్రారంభం = curbuf->b_op_ప్రారంభం;
    curbuf->b_op_ప్రారంభం.lnum = ((from == 0) ? 1 : from);
    curbuf->b_op_ప్రారంభం.నిలువు = 0;

    try_mac = (vim_strchr(p_ffs, 'm') != ఖాళీ);
    try_dos = (vim_strchr(p_ffs, 'd') != ఖాళీ);
    try_unix = (vim_strchr(p_ffs, 'x') != ఖాళీ);

    ఒకవేళ (!read_బఫరు)
    {
	పూర్ణాంకం	m = సందేశం_scroll;
	పూర్ణాంకం	n = సందేశం_scrolled;

	/*
	 * The file must be closed again, the autocommands may want to change
	 * the file before reading it.
	 */
	ఒకవేళ (!read_stdin)
	    close(fd);		// ignore errors

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	సందేశం_scroll = TRUE;
	ఒకవేళ (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPRE, ఖాళీ, sfname,
							  FALSE, curbuf, eap);
	లేకపోతే ఒకవేళ (read_stdin)
	    apply_autocmds_exarg(EVENT_STDINREADPRE, ఖాళీ, sfname,
							  FALSE, curbuf, eap);
	లేకపోతే ఒకవేళ (newfile)
	    apply_autocmds_exarg(EVENT_BUFREADPRE, ఖాళీ, sfname,
							  FALSE, curbuf, eap);
	లేకపోతే
	    apply_autocmds_exarg(EVENT_FILEREADPRE, sfname, sfname,
							    FALSE, ఖాళీ, eap);
	// autocommands may have changed it
	try_mac = (vim_strchr(p_ffs, 'm') != ఖాళీ);
	try_dos = (vim_strchr(p_ffs, 'd') != ఖాళీ);
	try_unix = (vim_strchr(p_ffs, 'x') != ఖాళీ);
	curbuf->b_op_ప్రారంభం = orig_ప్రారంభం;

	ఒకవేళ (సందేశం_scrolled == n)
	    సందేశం_scroll = m;

#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (aborting())	    // autocmds may abort script processing
	{
	    --no_wait_return;
	    సందేశం_scroll = సందేశం_save;
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    వెళ్ళు theend;
	}
#అయితే_ముగింపు
	/*
	 * Don't allow the autocommands to change the current buffer.
	 * Try to re-open the file.
	 *
	 * Don't allow the autocommands to change the buffer name either
	 * (cd for example) if it invalidates fname or sfname.
	 */
	ఒకవేళ (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))
		|| (fd = mch_open((అక్షరం *)fname, O_RDONLY | O_EXTRA, 0)) < 0))
	{
	    --no_wait_return;
	    సందేశం_scroll = సందేశం_save;
	    ఒకవేళ (fd < 0)
		emsg(_(e_readpre_autocommands_made_file_unreadable));
	    లేకపోతే
		emsg(_(e_readpre_autocommands_must_not_change_current_బఫరు));
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    వెళ్ళు theend;
	}
    }

    // Autocommands may add lines to the file, need to check if it is empty
    wasempty = (curbuf->b_ml.ml_flags & ML_ఖాళీలు);

    ఒకవేళ (!recoverymode && !filtering && !(flags & READ_DUMMY))
    {
	/*
	 * Show the user that we are busy reading the input.  Sometimes this
	 * may take a while.  When reading from stdin another program may
	 * still be running, don't move the cursor to the last line, unless
	 * always using the GUI.
	 */
	ఒకవేళ (read_stdin)
	{
	    ఒకవేళ (!is_not_a_పదం())
	    {
#నిర్వచితం_కాకపోతే ALWAYS_USE_GUI
# ifdef VIMDLL
		ఒకవేళ (!gui.లోపల_use)
# endif
		    mch_సందేశం(_("Vim: Reading from stdin...\n"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
		// Also write a message in the GUI window, if there is one.
		ఒకవేళ (gui.లోపల_use && !gui.dying && !gui.starting)
		{
		    size_t  plen = STRLEN(_("Reading from stdin..."));

		    // make a copy, gui_write() may try to change it
		    p = vim_strnsave((char_u *)_("Reading from stdin..."), plen);
		    ఒకవేళ (p != ఖాళీ)
		    {
			gui_write(p, (పూర్ణాంకం)plen);
			vim_free(p);
		    }
		}
#అయితే_ముగింపు
	    }
	}
	లేకపోతే ఒకవేళ (!read_బఫరు)
	    filemess(curbuf, sfname, (char_u *)"", 0);
    }

    సందేశం_scroll = FALSE;			// overwrite the file message

    /*
     * Set linecnt now, before the "retry" caused by a wrong guess for
     * fileformat, and after the autocommands, which may change them.
     */
    linecnt = curbuf->b_ml.ml_పంక్తి_లెక్క;

    // "++bad=" argument.
    ఒకవేళ (eap != ఖాళీ && eap->bad_char != 0)
    {
	bad_char_behavior = eap->bad_char;
	ఒకవేళ (సమితి_options)
	    curbuf->b_bad_char = eap->bad_char;
    }
    లేకపోతే
	curbuf->b_bad_char = 0;

    /*
     * Decide which 'encoding' to use or use first.
     */
    ఒకవేళ (eap != ఖాళీ && eap->force_enc != 0)
    {
	fenc = enc_canonize(eap->cmd + eap->force_enc);
	fenc_alloced = TRUE;
	keep_dest_enc = TRUE;
    }
    లేకపోతే ఒకవేళ (curbuf->b_p_bin)
    {
	fenc = (char_u *)"";		// binary: don't convert
	fenc_alloced = FALSE;
    }
    లేకపోతే ఒకవేళ (curbuf->b_help)
    {
	char_u	    firstline[80];
	పూర్ణాంకం	    fc;

	// Help files are either utf-8 or latin1.  Try utf-8 first, if this
	// fails it must be latin1.
	// Always do this when 'encoding' is "utf-8".  Otherwise only do
	// this when needed to avoid [converted] remarks all the time.
	// It is needed when the first line contains non-ASCII characters.
	// That is only in *.??x files.
	fenc = (char_u *)"latin1";
	c = enc_utf8;
	ఒకవేళ (!c && !read_stdin)
	{
	    fc = fname[fnamelen - 1];
	    ఒకవేళ (TOLOWER_ASC(fc) == 'x')
	    {
		// Read the first line (and a bit more).  Immediately rewind to
		// the start of the file.  If the read() fails "len" is -1.
		len = read_eintr(fd, firstline, 80);
		vim_lseek(fd, (off_T)0L, SEEK_SET);
		కొరకు (p = firstline; p < firstline + len; ++p)
		    ఒకవేళ (*p >= 0x80)
		    {
			c = TRUE;
			ఆపు;
		    }
	    }
	}

	ఒకవేళ (c)
	{
	    fenc_తదుపరి = fenc;
	    fenc = (char_u *)"utf-8";

	    // When the file is utf-8 but a character doesn't fit in
	    // 'encoding' don't retry.  In help text editing utf-8 bytes
	    // doesn't make sense.
	    ఒకవేళ (!enc_utf8)
		keep_dest_enc = TRUE;
	}
	fenc_alloced = FALSE;
    }
    లేకపోతే ఒకవేళ (*p_fencs == NUL)
    {
	fenc = curbuf->b_p_fenc;	// use format from buffer
	fenc_alloced = FALSE;
    }
    లేకపోతే
    {
	fenc_తదుపరి = p_fencs;		// try items in 'fileencodings'
	fenc = తదుపరి_fenc(&fenc_తదుపరి, &fenc_alloced);
    }

    /*
     * Jump back here to retry reading the file in different ways.
     * Reasons to retry:
     * - encoding conversion failed: try another one from "fenc_next"
     * - BOM detected and fenc was set, need to setup conversion
     * - "fileformat" check failed: try another
     *
     * Variables set for special retry actions:
     * "file_rewind"	Rewind the file to start reading it again.
     * "advance_fenc"	Advance "fenc" using "fenc_next".
     * "skip_read"	Re-use already read bytes (BOM detected).
     * "did_iconv"	iconv() conversion failed, try 'charconvert'.
     * "keep_fileformat" Don't reset "fileformat".
     *
     * Other status indicators:
     * "tmpname"	When != NULL did conversion with 'charconvert'.
     *			Output file has to be deleted afterwards.
     * "iconv_fd"	When != -1 did conversion with iconv().
     */
retry:

    ఒకవేళ (file_rewind)
    {
	ఒకవేళ (read_బఫరు)
	{
	    read_buf_lnum = 1;
	    read_buf_నిలువు = 0;
	}
	లేకపోతే ఒకవేళ (read_stdin || vim_lseek(fd, (off_T)0L, SEEK_SET) != 0)
	{
	    // Can't rewind the file, give up.
	    error = TRUE;
	    వెళ్ళు failed;
	}
	// Delete the previously read lines.
	అయితే (lnum > from)
	    ml_delete(lnum--);
	file_rewind = FALSE;
	ఒకవేళ (సమితి_options)
	{
	    curbuf->b_p_bomb = FALSE;
	    curbuf->b_ప్రారంభం_bomb = FALSE;
	}
	conv_error = 0;
    }

    /*
     * When retrying with another "fenc" and the first time "fileformat"
     * will be reset.
     */
    ఒకవేళ (keep_fileformat)
	keep_fileformat = FALSE;
    లేకపోతే
    {
	ఒకవేళ (eap != ఖాళీ && eap->force_ff != 0)
	{
	    fileformat = get_fileformat_force(curbuf, eap);
	    try_unix = try_dos = try_mac = FALSE;
	}
	లేకపోతే ఒకవేళ (curbuf->b_p_bin)
	    fileformat = EOL_UNIX;		// binary: use Unix format
	లేకపోతే ఒకవేళ (*p_ffs == NUL)
	    fileformat = get_fileformat(curbuf);// use format from buffer
	లేకపోతే
	    fileformat = EOL_UNKNOWN;		// detect from file
    }

#నిర్వచితమైతే USE_ICONV
    ఒకవేళ (iconv_fd != (iconv_t)-1)
    {
	// aborted conversion with iconv(), close the descriptor
	iconv_close(iconv_fd);
	iconv_fd = (iconv_t)-1;
    }
#అయితే_ముగింపు

    ఒకవేళ (advance_fenc)
    {
	/*
	 * Try the next entry in 'fileencodings'.
	 */
	advance_fenc = FALSE;

	ఒకవేళ (eap != ఖాళీ && eap->force_enc != 0)
	{
	    // Conversion given with "++cc=" wasn't possible, read
	    // without conversion.
	    notconverted = TRUE;
	    conv_error = 0;
	    ఒకవేళ (fenc_alloced)
		vim_free(fenc);
	    fenc = (char_u *)"";
	    fenc_alloced = FALSE;
	}
	లేకపోతే
	{
	    ఒకవేళ (fenc_alloced)
		vim_free(fenc);
	    ఒకవేళ (fenc_తదుపరి != ఖాళీ)
	    {
		fenc = తదుపరి_fenc(&fenc_తదుపరి, &fenc_alloced);
	    }
	    లేకపోతే
	    {
		fenc = (char_u *)"";
		fenc_alloced = FALSE;
	    }
	}
	ఒకవేళ (tmpname != ఖాళీ)
	{
	    mch_remove(tmpname);		// delete converted file
	    VIM_CLEAR(tmpname);
	}
    }

    /*
     * Conversion may be required when the encoding of the file is different
     * from 'encoding' or 'encoding' is UTF-16, UCS-2 or UCS-4.
     */
    fio_flags = 0;
    converted = need_conversion(fenc);
    ఒకవేళ (converted)
    {
	// "ucs-bom" means we need to check the first bytes of the file
	// for a BOM.
	ఒకవేళ (STRCMP(fenc, ENC_UCSBOM) == 0)
	    fio_flags = FIO_UCSBOM;

	/*
	 * Check if UCS-2/4 or Latin1 to UTF-8 conversion needs to be
	 * done.  This is handled below after read().  Prepare the
	 * fio_flags to avoid having to parse the string each time.
	 * Also check for Unicode to Latin1 conversion, because iconv()
	 * appears not to handle this correctly.  This works just like
	 * conversion to UTF-8 except how the resulting character is put in
	 * the buffer.
	 */
	లేకపోతే ఒకవేళ (enc_utf8 || STRCMP(p_enc, "latin1") == 0)
	    fio_flags = get_fio_flags(fenc);

#నిర్వచితమైతే MSWIN
	/*
	 * Conversion from an MS-Windows codepage to UTF-8 or another codepage
	 * is handled with MultiByteToWideChar().
	 */
	ఒకవేళ (fio_flags == 0)
	    fio_flags = get_win_fio_flags(fenc);
#అయితే_ముగింపు

#నిర్వచితమైతే MACOS_CONVERT
	// Conversion from Apple MacRoman to latin1 or UTF-8
	ఒకవేళ (fio_flags == 0)
	    fio_flags = get_mac_fio_flags(fenc);
#అయితే_ముగింపు

#నిర్వచితమైతే USE_ICONV
	/*
	 * Try using iconv() if we can't convert internally.
	 */
	ఒకవేళ (fio_flags == 0
# ifdef FEAT_EVAL
		&& !did_iconv
# endif
		)
	    iconv_fd = (iconv_t)my_iconv_open(
				  enc_utf8 ? (char_u *)"utf-8" : p_enc, fenc);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
	/*
	 * Use the 'charconvert' expression when conversion is required
	 * and we can't do it internally or with iconv().
	 */
	ఒకవేళ (fio_flags == 0 && !read_stdin && !read_బఫరు && *p_ccv != NUL
						    && !read_fifo
# ifdef USE_ICONV
						    && iconv_fd == (iconv_t)-1
# endif
		)
	{
# ifdef USE_ICONV
	    did_iconv = FALSE;
# endif
	    // Skip conversion when it's already done (retry for wrong
	    // "fileformat").
	    ఒకవేళ (tmpname == ఖాళీ)
	    {
		tmpname = readfile_charconvert(fname, fenc, &fd);
		ఒకవేళ (tmpname == ఖాళీ)
		{
		    // Conversion failed.  Try another one.
		    advance_fenc = TRUE;
		    ఒకవేళ (fd < 0)
		    {
			// Re-opening the original file failed!
			emsg(_(e_conversion_mad_file_unreadable));
			error = TRUE;
			వెళ్ళు failed;
		    }
		    వెళ్ళు retry;
		}
	    }
	}
	లేకపోతే
#అయితే_ముగింపు
	{
	    ఒకవేళ (fio_flags == 0
#నిర్వచితమైతే USE_ICONV
		    && iconv_fd == (iconv_t)-1
#అయితే_ముగింపు
	       )
	    {
		// Conversion wanted but we can't.
		// Try the next conversion in 'fileencodings'
		advance_fenc = TRUE;
		వెళ్ళు retry;
	    }
	}
    }

    // Set "can_retry" when it's possible to rewind the file and try with
    // another "fenc" value.  It's FALSE when no other "fenc" to try, reading
    // stdin or fixed at a specific encoding.
    can_retry = (*fenc != NUL && !read_stdin && !read_fifo && !keep_dest_enc);

    ఒకవేళ (!skip_read)
    {
	linerest = 0;
	filesize = 0;
#నిర్వచితమైతే FEAT_CRYPT
	filesize_లెక్క = 0;
#అయితే_ముగింపు
	skip_లెక్క = పంక్తులు_గమ్యం_skip;
	read_లెక్క = పంక్తులు_గమ్యం_read;
	conv_restlen = 0;
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
	read_undo_file = (newfile && (flags & READ_KEEP_UNDO) == 0
				  && curbuf->b_ffname != ఖాళీ
				  && curbuf->b_p_udf
				  && !filtering
				  && !read_fifo
				  && !read_stdin
				  && !read_బఫరు);
	ఒకవేళ (read_undo_file)
	    sha256_ప్రారంభం(&sha_ctx);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CRYPT
	ఒకవేళ (curbuf->b_cryptstate != ఖాళీ)
	{
	    // Need to free the state, but keep the key, don't want to ask for
	    // it again.
	    crypt_free_state(curbuf->b_cryptstate);
	    curbuf->b_cryptstate = ఖాళీ;
	}
#అయితే_ముగింపు
    }

    అయితే (!error && !got_int)
    {
	/*
	 * We allocate as much space for the file as we can get, plus
	 * space for the old line plus room for one terminating NUL.
	 * The amount is limited by the fact that read() only can read
	 * up to max_unsigned characters (and other things).
	 */
	ఒకవేళ (!skip_read)
	{
#ఒకవేళ defined(SSIZE_MAX) && (SSIZE_MAX < 0x10000L)
		కొలత = SSIZE_MAX;		    // use max I/O size, 52K
#లేకపోతే
		// Use buffer >= 64K.  Add linerest to double the size if the
		// line gets very long, to avoid a lot of copying. But don't
		// read more than 1 Mbyte at a time, so we can be interrupted.
		కొలత = 0x10000L + linerest;
		ఒకవేళ (కొలత > 0x100000L)
		    కొలత = 0x100000L;
#అయితే_ముగింపు
	}

	// Protect against the argument of lalloc() going negative.
	// Also split lines that are too long for colnr_T.  After this check
	// passes, we read up to 'size' more bytes.  We must ensure that even
	// after that read, the line length won't exceed MAXCOL - 1 (because
	// we add 1 for the NUL when casting to colnr_T).  If this check fires,
	// we insert a synthetic newline immediately, so linerest doesn't grow.
	ఒకవేళ (కొలత < 0 || కొలత + linerest + 1 < 0 || linerest >= MAXCOL - కొలత)
	{
	    ++విభజించు;
	    *ptr = NL;		    // split line by inserting a NL
	    కొలత = 1;
	}
	లేకపోతే
	{
	    ఒకవేళ (!skip_read)
	    {
		కొరకు ( ; కొలత >= 10; కొలత = (దీర్ఘ)((long_u)కొలత >> 1))
		{
		    ఒకవేళ ((new_బఫరు = lalloc(కొలత + linerest + 1,
							      FALSE)) != ఖాళీ)
			ఆపు;
		}
		ఒకవేళ (new_బఫరు == ఖాళీ)
		{
		    do_outofmem_సందేశం((long_u)(కొలత * 2 + linerest + 1));
		    error = TRUE;
		    ఆపు;
		}
		ఒకవేళ (linerest)	// copy characters from the previous buffer
		    mch_memmove(new_బఫరు, ptr - linerest, (size_t)linerest);
		vim_free(బఫరు);
		బఫరు = new_బఫరు;
		ptr = బఫరు + linerest;
		పంక్తి_ప్రారంభం = బఫరు;

		// May need room to translate into.
		// For iconv() we don't really know the required space, use a
		// factor ICONV_MULT.
		// latin1 to utf-8: 1 byte becomes up to 2 bytes
		// utf-16 to utf-8: 2 bytes become up to 3 bytes, 4 bytes
		// become up to 4 bytes, size must be multiple of 2
		// ucs-2 to utf-8: 2 bytes become up to 3 bytes, size must be
		// multiple of 2
		// ucs-4 to utf-8: 4 bytes become up to 6 bytes, size must be
		// multiple of 4
		real_కొలత = (పూర్ణాంకం)కొలత;
#నిర్వచితమైతే USE_ICONV
		ఒకవేళ (iconv_fd != (iconv_t)-1)
		    కొలత = కొలత / ICONV_MULT;
		లేకపోతే
#అయితే_ముగింపు
		    ఒకవేళ (fio_flags & FIO_LATIN1)
		    కొలత = కొలత / 2;
		లేకపోతే ఒకవేళ (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    కొలత = (కొలత * 2 / 3) & ~1;
		లేకపోతే ఒకవేళ (fio_flags & FIO_UCS4)
		    కొలత = (కొలత * 2 / 3) & ~3;
		లేకపోతే ఒకవేళ (fio_flags == FIO_UCSBOM)
		    కొలత = కొలత / ICONV_MULT;	// worst case
#నిర్వచితమైతే MSWIN
		లేకపోతే ఒకవేళ (fio_flags & FIO_CODEPAGE)
		    కొలత = కొలత / ICONV_MULT;	// also worst case
#అయితే_ముగింపు
#నిర్వచితమైతే MACOS_CONVERT
		లేకపోతే ఒకవేళ (fio_flags & FIO_MACROMAN)
		    కొలత = కొలత / ICONV_MULT;	// also worst case
#అయితే_ముగింపు

		ఒకవేళ (conv_restlen > 0)
		{
		    // Insert unconverted bytes from previous line.
		    mch_memmove(ptr, conv_rest, conv_restlen);
		    ptr += conv_restlen;
		    కొలత -= conv_restlen;
		}

		ఒకవేళ (read_బఫరు)
		{
		    /*
		     * Read bytes from curbuf.  Used for converting text read
		     * from stdin.
		     */
		    ఒకవేళ (read_buf_lnum > from)
			కొలత = 0;
		    లేకపోతే
		    {
			పూర్ణాంకం	n, ni;
			దీర్ఘ	tlen;

			tlen = 0;
			కొరకు (;;)
			{
			    p = ml_get(read_buf_lnum) + read_buf_నిలువు;
			    n = ml_get_len(read_buf_lnum) - read_buf_నిలువు;
			    ఒకవేళ ((పూర్ణాంకం)tlen + n + 1 > కొలత)
			    {
				// Filled up to "size", append partial line.
				// Change NL to NUL to reverse the effect done
				// below.
				n = (పూర్ణాంకం)(కొలత - tlen);
				కొరకు (ni = 0; ni < n; ++ni)
				{
				    ఒకవేళ (p[ni] == NL)
					ptr[tlen++] = NUL;
				    లేకపోతే
					ptr[tlen++] = p[ni];
				}
				read_buf_నిలువు += n;
				ఆపు;
			    }

			    // Append whole line and new-line.  Change NL
			    // to NUL to reverse the effect done below.
			    కొరకు (ni = 0; ni < n; ++ni)
			    {
				ఒకవేళ (p[ni] == NL)
				    ptr[tlen++] = NUL;
				లేకపోతే
				    ptr[tlen++] = p[ni];
			    }
			    ptr[tlen++] = NL;
			    read_buf_నిలువు = 0;
			    ఒకవేళ (++read_buf_lnum > from)
			    {
				// When the last line didn't have an
				// end-of-line don't add it now either.
				ఒకవేళ (!curbuf->b_p_eol)
				    --tlen;
				కొలత = tlen;
#నిర్వచితమైతే FEAT_CRYPT
				eof = TRUE;
#అయితే_ముగింపు
				ఆపు;
			    }
			}
		    }
		}
		లేకపోతే
		{
		    /*
		     * Read bytes from the file.
		     */
#నిర్వచితమైతే FEAT_SODIUM
		    // Let the crypt layer work with a buffer size of 8192
		    //
		    // Sodium encryption requires a fixed block size to
		    // successfully decrypt. However, unfortunately the file
		    // header size changes between xchacha20 and xchacha20v2 by
		    // 'add_len' bytes.
		    // So we will now read the maximum header size + encryption
		    // metadata, but after determining to read an xchacha20
		    // encrypted file, we have to rewind the file descriptor by
		    // 'add_len' bytes in the second round.
		    //
		    // Be careful with changing it, it needs to stay the same
		    // for reading back previously encrypted files!
		    ఒకవేళ (filesize == 0)
		    {
			// set size to 8K + Sodium Crypt Metadata
			కొలత = WRITEBUFSIZE + crypt_get_max_శీర్షిక_len()
			    + crypto_secretstream_xchacha20poly1305_HEADERBYTES
				+ crypto_secretstream_xchacha20poly1305_ABYTES;
			may_need_lseek = TRUE;
		    }

		    లేకపోతే ఒకవేళ (filesize > 0 && (curbuf->b_cryptstate != ఖాళీ
				&& crypt_పద్ధతి_is_sodium(
					     curbuf->b_cryptstate->పద్ధతి_nr)))
		    {
			కొలత = WRITEBUFSIZE + crypto_secretstream_xchacha20poly1305_ABYTES;
			// need to rewind by - add_len from CRYPT_M_SOD2 (see
			// description above)
			ఒకవేళ (curbuf->b_cryptstate->పద్ధతి_nr == CRYPT_M_SOD
						     && !eof && may_need_lseek)
			{
			    lseek(fd, crypt_get_శీర్షిక_len(
					       curbuf->b_cryptstate->పద్ధతి_nr)
				       - crypt_get_max_శీర్షిక_len(), SEEK_CUR);
			    may_need_lseek = FALSE;
			}
		    }
#అయితే_ముగింపు
		    దీర్ఘ read_కొలత = కొలత;
		    కొలత = read_eintr(fd, ptr, read_కొలత);
#నిర్వచితమైతే FEAT_CRYPT
		    // Did we reach end of file?
		    filesize_లెక్క += కొలత;
		    eof = (కొలత < read_కొలత || filesize_లెక్క == filesize_disk);
#అయితే_ముగింపు
		}

#నిర్వచితమైతే FEAT_CRYPT
		/*
		 * At start of file: Check for magic number of encryption.
		 */
		ఒకవేళ (filesize == 0 && కొలత > 0)
		{
		    cryptkey = check_for_cryptkey(cryptkey, ptr, &కొలత,
						  &filesize, newfile, sfname,
						  &did_ask_for_కీలకం);
# ఒకవేళ defined(CRYPT_NOT_INPLACE) && defined(FEAT_PERSISTENT_UNDO)
		    ఒకవేళ (curbuf->b_cryptstate != ఖాళీ
				 && !crypt_పనిచేస్తుంది_inplace(curbuf->b_cryptstate))
			// reading undo file requires crypt_decode_inplace()
			read_undo_file = FALSE;
# endif
		}
		/*
		 * Decrypt the read bytes.  This is done before checking for
		 * EOF because the crypt layer may be buffering.
		 */
		ఒకవేళ (cryptkey != ఖాళీ && curbuf->b_cryptstate != ఖాళీ
								   && కొలత > 0)
		{
# ifdef CRYPT_NOT_INPLACE
		    ఒకవేళ (crypt_పనిచేస్తుంది_inplace(curbuf->b_cryptstate))
		    {
# endif
			crypt_decode_inplace(curbuf->b_cryptstate, ptr,
								    కొలత, eof);
# ifdef CRYPT_NOT_INPLACE
		    }
		    లేకపోతే
		    {
			char_u	*newptr = ఖాళీ;
			పూర్ణాంకం	decrypted_కొలత;

			decrypted_కొలత = crypt_decode_alloc(
				    curbuf->b_cryptstate, ptr, కొలత,
								 &newptr, eof);

			ఒకవేళ (decrypted_కొలత < 0)
			{
			    // error message already given
			    error = TRUE;
			    vim_free(newptr);
			    ఆపు;
			}
			// If the crypt layer is buffering, not producing
			// anything yet, need to read more.
			ఒకవేళ (decrypted_కొలత == 0)
			    కొనసాగు;

			ఒకవేళ (linerest == 0)
			{
			    // Simple case: reuse returned buffer (may be
			    // NULL, checked later).
			    new_బఫరు = newptr;
			}
			లేకపోతే
			{
			    long_u	new_కొలత;

			    // Need new buffer to add bytes carried over.
			    new_కొలత = (long_u)(decrypted_కొలత + linerest + 1);
			    new_బఫరు = lalloc(new_కొలత, FALSE);
			    ఒకవేళ (new_బఫరు == ఖాళీ)
			    {
				do_outofmem_సందేశం(new_కొలత);
				error = TRUE;
				ఆపు;
			    }

			    mch_memmove(new_బఫరు, బఫరు, linerest + conv_restlen);
			    ఒకవేళ (newptr != ఖాళీ)
				mch_memmove(new_బఫరు + linerest + conv_restlen,
					newptr, decrypted_కొలత);
			    vim_free(newptr);
			}

			ఒకవేళ (new_బఫరు != ఖాళీ)
			{
			    vim_free(బఫరు);
			    బఫరు = new_బఫరు;
			    new_బఫరు = ఖాళీ;
			    పంక్తి_ప్రారంభం = బఫరు;
			    ptr = బఫరు + linerest + conv_restlen;
			    real_కొలత = కొలత;
			}
			కొలత = decrypted_కొలత;
		    }
# endif
		}
#అయితే_ముగింపు

		ఒకవేళ (కొలత <= 0)
		{
		    ఒకవేళ (కొలత < 0)		    // read error
			error = TRUE;
		    లేకపోతే ఒకవేళ (conv_restlen > 0)
		    {
			/*
			 * Reached end-of-file but some trailing bytes could
			 * not be converted.  Truncated file?
			 */

			// When we did a conversion report an error.
			ఒకవేళ (fio_flags != 0
#నిర్వచితమైతే USE_ICONV
				|| iconv_fd != (iconv_t)-1
#అయితే_ముగింపు
			   )
			{
			    ఒకవేళ (can_retry)
				వెళ్ళు rewind_retry;
			    ఒకవేళ (conv_error == 0)
				conv_error = curbuf->b_ml.ml_పంక్తి_లెక్క
								- linecnt + 1;
			}
			// Remember the first linenr with an illegal byte
			లేకపోతే ఒకవేళ (illegal_byte == 0)
			    illegal_byte = curbuf->b_ml.ml_పంక్తి_లెక్క
								- linecnt + 1;
			ఒకవేళ (bad_char_behavior == BAD_DROP)
			{
			    *(ptr - conv_restlen) = NUL;
			    conv_restlen = 0;
			}
			లేకపోతే
			{
			    // Replace the trailing bytes with the replacement
			    // character if we were converting; if we weren't,
			    // leave the UTF8 checking code to do it, as it
			    // works slightly differently.
			    ఒకవేళ (bad_char_behavior != BAD_KEEP && (fio_flags != 0
#నిర్వచితమైతే USE_ICONV
				    || iconv_fd != (iconv_t)-1
#అయితే_ముగింపు
			       ))
			    {
				అయితే (conv_restlen > 0)
				{
				    *(--ptr) = bad_char_behavior;
				    --conv_restlen;
				}
			    }
			    fio_flags = 0;	// don't convert this
#నిర్వచితమైతే USE_ICONV
			    ఒకవేళ (iconv_fd != (iconv_t)-1)
			    {
				iconv_close(iconv_fd);
				iconv_fd = (iconv_t)-1;
			    }
#అయితే_ముగింపు
			}
		    }
		}
	    }
	    skip_read = FALSE;

	    /*
	     * At start of file (or after crypt magic number): Check for BOM.
	     * Also check for a BOM for other Unicode encodings, but not after
	     * converting with 'charconvert' or when a BOM has already been
	     * found.
	     */
	    ఒకవేళ ((filesize == 0
#నిర్వచితమైతే FEAT_CRYPT
		   || (cryptkey != ఖాళీ
			&& filesize == crypt_get_శీర్షిక_len(
						 crypt_get_పద్ధతి_nr(curbuf)))
#అయితే_ముగింపు
		       )
		    && (fio_flags == FIO_UCSBOM
			|| (!curbuf->b_p_bomb
			    && tmpname == ఖాళీ
			    && (*fenc == 'u' || (*fenc == NUL && enc_utf8)))))
	    {
		char_u	*ccname;
		పూర్ణాంకం	blen;

		// no BOM detection in a short file or in binary mode
		ఒకవేళ (కొలత < 2 || curbuf->b_p_bin)
		    ccname = ఖాళీ;
		లేకపోతే
		    ccname = check_for_bom(ptr, కొలత, &blen,
		      fio_flags == FIO_UCSBOM ? FIO_ALL : get_fio_flags(fenc));
		ఒకవేళ (ccname != ఖాళీ)
		{
		    // Remove BOM from the text
		    filesize += blen;
		    కొలత -= blen;
		    mch_memmove(ptr, ptr + blen, (size_t)కొలత);
		    ఒకవేళ (సమితి_options)
		    {
			curbuf->b_p_bomb = TRUE;
			curbuf->b_ప్రారంభం_bomb = TRUE;
		    }
		}

		ఒకవేళ (fio_flags == FIO_UCSBOM)
		{
		    ఒకవేళ (ccname == ఖాళీ)
		    {
			// No BOM detected: retry with next encoding.
			advance_fenc = TRUE;
		    }
		    లేకపోతే
		    {
			// BOM detected: set "fenc" and jump back
			ఒకవేళ (fenc_alloced)
			    vim_free(fenc);
			fenc = ccname;
			fenc_alloced = FALSE;
		    }
		    // retry reading without getting new bytes or rewinding
		    skip_read = TRUE;
		    వెళ్ళు retry;
		}
	    }

	    // Include not converted bytes.
	    ptr -= conv_restlen;
	    కొలత += conv_restlen;
	    conv_restlen = 0;
	    /*
	     * Break here for a read error or end-of-file.
	     */
	    ఒకవేళ (కొలత <= 0)
		ఆపు;


#నిర్వచితమైతే USE_ICONV
	    ఒకవేళ (iconv_fd != (iconv_t)-1)
	    {
		/*
		 * Attempt conversion of the read bytes to 'encoding' using
		 * iconv().
		 */
		స్థిరరాశి అక్షరం	*fromp;
		అక్షరం		*శిఖరం;
		size_t		from_కొలత;
		size_t		గమ్యం_కొలత;

		fromp = (అక్షరం *)ptr;
		from_కొలత = కొలత;
		ptr += కొలత;
		శిఖరం = (అక్షరం *)ptr;
		గమ్యం_కొలత = real_కొలత - కొలత;

		/*
		 * If there is conversion error or not enough room try using
		 * another conversion.  Except for when there is no
		 * alternative (help files).
		 */
		అయితే ((iconv(iconv_fd, (శూన్య *)&fromp, &from_కొలత,
							       &శిఖరం, &గమ్యం_కొలత)
			    == (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						  || from_కొలత > CONV_RESTLEN)
		{
		    ఒకవేళ (can_retry)
			వెళ్ళు rewind_retry;
		    ఒకవేళ (conv_error == 0)
			conv_error = readfile_linenr(linecnt,
							  ptr, (char_u *)శిఖరం);

		    // Deal with a bad byte and continue with the next.
		    ++fromp;
		    --from_కొలత;
		    ఒకవేళ (bad_char_behavior == BAD_KEEP)
		    {
			*శిఖరం++ = *(fromp - 1);
			--గమ్యం_కొలత;
		    }
		    లేకపోతే ఒకవేళ (bad_char_behavior != BAD_DROP)
		    {
			*శిఖరం++ = bad_char_behavior;
			--గమ్యం_కొలత;
		    }
		}

		ఒకవేళ (from_కొలత > 0)
		{
		    // Some remaining characters, keep them for the next
		    // round.
		    mch_memmove(conv_rest, (char_u *)fromp, from_కొలత);
		    conv_restlen = (పూర్ణాంకం)from_కొలత;
		}

		// move the linerest to before the converted characters
		పంక్తి_ప్రారంభం = ptr - linerest;
		mch_memmove(పంక్తి_ప్రారంభం, బఫరు, (size_t)linerest);
		కొలత = (దీర్ఘ)((char_u *)శిఖరం - ptr);
	    }
#అయితే_ముగింపు

#నిర్వచితమైతే MSWIN
	    ఒకవేళ (fio_flags & FIO_CODEPAGE)
	    {
		char_u	*src, *dst;
		WCHAR	ucs2buf[3];
		పూర్ణాంకం	ucs2len;
		పూర్ణాంకం	codepage = FIO_GET_CP(fio_flags);
		పూర్ణాంకం	bytelen;
		పూర్ణాంకం	దొరికింది_bad;
		అక్షరం	replstr[2];

		/*
		 * Conversion from an MS-Windows codepage or UTF-8 to UTF-8 or
		 * a codepage, using standard MS-Windows functions.  This
		 * requires two steps:
		 * 1. convert from 'fileencoding' to ucs-2
		 * 2. convert from ucs-2 to 'encoding'
		 *
		 * Because there may be illegal bytes AND an incomplete byte
		 * sequence at the end, we may have to do the conversion one
		 * character at a time to get it right.
		 */

		// Replacement string for WideCharToMultiByte().
		ఒకవేళ (bad_char_behavior > 0)
		    replstr[0] = bad_char_behavior;
		లేకపోతే
		    replstr[0] = '?';
		replstr[1] = NUL;

		/*
		 * Move the bytes to the end of the buffer, so that we have
		 * room to put the result at the start.
		 */
		src = ptr + real_కొలత - కొలత;
		mch_memmove(src, ptr, కొలత);

		/*
		 * Do the conversion.
		 */
		dst = ptr;
		అయితే (కొలత > 0)
		{
		    దొరికింది_bad = FALSE;

# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
		    ఒకవేళ (codepage == CP_UTF8)
		    {
			// Handle CP_UTF8 input ourselves to be able to handle
			// trailing bytes properly.
			// Get one UTF-8 character from src.
			bytelen = (పూర్ణాంకం)utf_ptr2len_len(src, కొలత);
			ఒకవేళ (bytelen > కొలత)
			{
			    // Only got some bytes of a character.  Normally
			    // it's put in "conv_rest", but if it's too long
			    // deal with it as if they were illegal bytes.
			    ఒకవేళ (bytelen <= CONV_RESTLEN)
				ఆపు;

			    // weird overlong byte sequence
			    bytelen = కొలత;
			    దొరికింది_bad = TRUE;
			}
			లేకపోతే
			{
			    పూర్ణాంకం	    u8c = utf_ptr2char(src);

			    ఒకవేళ (u8c > 0xffff || (*src >= 0x80 && bytelen == 1))
				దొరికింది_bad = TRUE;
			    ucs2buf[0] = u8c;
			    ucs2len = 1;
			}
		    }
		    లేకపోతే
# endif
		    {
			// We don't know how long the byte sequence is, try
			// from one to three bytes.
			కొరకు (bytelen = 1; bytelen <= కొలత && bytelen <= 3;
								    ++bytelen)
			{
			    ucs2len = Multi_Byte_గమ్యం_Wide_Char(codepage,
							 MB_ERR_INVALID_CHARS,
							 (LPCSTR)src, bytelen,
								   ucs2buf, 3);
			    ఒకవేళ (ucs2len > 0)
				ఆపు;
			}
			ఒకవేళ (ucs2len == 0)
			{
			    // If we have only one byte then it's probably an
			    // incomplete byte sequence.  Otherwise discard
			    // one byte as a bad character.
			    ఒకవేళ (కొలత == 1)
				ఆపు;
			    దొరికింది_bad = TRUE;
			    bytelen = 1;
			}
		    }

		    ఒకవేళ (!దొరికింది_bad)
		    {
			పూర్ణాంకం	i;

			// Convert "ucs2buf[ucs2len]" to 'enc' in "dst".
			ఒకవేళ (enc_utf8)
			{
			    // From UCS-2 to UTF-8.  Cannot fail.
			    కొరకు (i = 0; i < ucs2len; ++i)
				dst += utf_char2bytes(ucs2buf[i], dst);
			}
			లేకపోతే
			{
			    BOOL	bad = FALSE;
			    పూర్ణాంకం		dstlen;

			    // From UCS-2 to "enc_codepage".  If the
			    // conversion uses the default character "?",
			    // the data doesn't fit in this encoding.
			    dstlen = Wide_Char_గమ్యం_Multi_Byte(enc_codepage, 0,
				    (LPCWSTR)ucs2buf, ucs2len,
				    (LPSTR)dst, (పూర్ణాంకం)(src - dst),
				    replstr, &bad);
			    ఒకవేళ (bad)
				దొరికింది_bad = TRUE;
			    లేకపోతే
				dst += dstlen;
			}
		    }

		    ఒకవేళ (దొరికింది_bad)
		    {
			// Deal with bytes we can't convert.
			ఒకవేళ (can_retry)
			    వెళ్ళు rewind_retry;
			ఒకవేళ (conv_error == 0)
			    conv_error = readfile_linenr(linecnt, ptr, dst);
			ఒకవేళ (bad_char_behavior != BAD_DROP)
			{
			    ఒకవేళ (bad_char_behavior == BAD_KEEP)
			    {
				mch_memmove(dst, src, bytelen);
				dst += bytelen;
			    }
			    లేకపోతే
				*dst++ = bad_char_behavior;
			}
		    }

		    src += bytelen;
		    కొలత -= bytelen;
		}

		ఒకవేళ (కొలత > 0)
		{
		    // An incomplete byte sequence remaining.
		    mch_memmove(conv_rest, src, కొలత);
		    conv_restlen = కొలత;
		}

		// The new size is equal to how much "dst" was advanced.
		కొలత = (దీర్ఘ)(dst - ptr);
	    }
	    లేకపోతే
#అయితే_ముగింపు
#నిర్వచితమైతే MACOS_CONVERT
	    ఒకవేళ (fio_flags & FIO_MACROMAN)
	    {
		/*
		 * Conversion from Apple MacRoman char encoding to UTF-8 or
		 * latin1.  This is in os_mac_conv.c.
		 */
		ఒకవేళ (macroman2enc(ptr, &కొలత, real_కొలత) == FAIL)
		    వెళ్ళు rewind_retry;
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    ఒకవేళ (fio_flags != 0)
	    {
		పూర్ణాంకం	u8c;
		char_u	*dest;
		char_u	*తోక = ఖాళీ;

		/*
		 * "enc_utf8" set: Convert Unicode or Latin1 to UTF-8.
		 * "enc_utf8" not set: Convert Unicode to Latin1.
		 * Go from end to start through the buffer, because the number
		 * of bytes may increase.
		 * "dest" points to after where the UTF-8 bytes go, "p" points
		 * to after the next character to convert.
		 */
		dest = ptr + real_కొలత;
		ఒకవేళ (fio_flags == FIO_LATIN1 || fio_flags == FIO_UTF8)
		{
		    p = ptr + కొలత;
		    ఒకవేళ (fio_flags == FIO_UTF8)
		    {
			// Check for a trailing incomplete UTF-8 sequence
			తోక = ptr + కొలత - 1;
			అయితే (తోక > ptr && (*తోక & 0xc0) == 0x80)
			    --తోక;
			ఒకవేళ (తోక + utf_byte2len(*తోక) <= ptr + కొలత)
			    తోక = ఖాళీ;
			లేకపోతే
			    p = తోక;
		    }
		}
		లేకపోతే ఒకవేళ (fio_flags & (FIO_UCS2 | FIO_UTF16))
		{
		    // Check for a trailing byte
		    p = ptr + (కొలత & ~1);
		    ఒకవేళ (కొలత & 1)
			తోక = p;
		    ఒకవేళ ((fio_flags & FIO_UTF16) && p > ptr)
		    {
			// Check for a trailing leading word
			ఒకవేళ (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			లేకపోతే
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			ఒకవేళ (u8c >= 0xd800 && u8c <= 0xdbff)
			    తోక = p;
			లేకపోతే
			    p += 2;
		    }
		}
		లేకపోతే //  FIO_UCS4
		{
		    // Check for trailing 1, 2 or 3 bytes
		    p = ptr + (కొలత & ~3);
		    ఒకవేళ (కొలత & 3)
			తోక = p;
		}

		// If there is a trailing incomplete sequence move it to
		// conv_rest[].
		ఒకవేళ (తోక != ఖాళీ)
		{
		    conv_restlen = (పూర్ణాంకం)((ptr + కొలత) - తోక);
		    mch_memmove(conv_rest, (char_u *)తోక, conv_restlen);
		    కొలత -= conv_restlen;
		}


		అయితే (p > ptr)
		{
		    ఒకవేళ (fio_flags & FIO_LATIN1)
			u8c = *--p;
		    లేకపోతే ఒకవేళ (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    {
			ఒకవేళ (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			లేకపోతే
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			ఒకవేళ ((fio_flags & FIO_UTF16)
					    && u8c >= 0xdc00 && u8c <= 0xdfff)
			{
			    పూర్ణాంకం u16c;

			    ఒకవేళ (p == ptr)
			    {
				// Missing leading word.
				ఒకవేళ (can_retry)
				    వెళ్ళు rewind_retry;
				ఒకవేళ (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				ఒకవేళ (bad_char_behavior == BAD_DROP)
				    కొనసాగు;
				ఒకవేళ (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }

			    // found second word of double-word, get the first
			    // word and compute the resulting character
			    ఒకవేళ (fio_flags & FIO_ENDIAN_L)
			    {
				u16c = (*--p << 8);
				u16c += *--p;
			    }
			    లేకపోతే
			    {
				u16c = *--p;
				u16c += (*--p << 8);
			    }
			    u8c = 0x10000 + ((u16c & 0x3ff) << 10)
							      + (u8c & 0x3ff);

			    // Check if the word is indeed a leading word.
			    ఒకవేళ (u16c < 0xd800 || u16c > 0xdbff)
			    {
				ఒకవేళ (can_retry)
				    వెళ్ళు rewind_retry;
				ఒకవేళ (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				ఒకవేళ (bad_char_behavior == BAD_DROP)
				    కొనసాగు;
				ఒకవేళ (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    లేకపోతే ఒకవేళ (fio_flags & FIO_UCS4)
		    {
			ఒకవేళ (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (గుర్తులేని)*--p << 24;
			    u8c += (గుర్తులేని)*--p << 16;
			    u8c += (గుర్తులేని)*--p << 8;
			    u8c += *--p;
			}
			లేకపోతే	// big endian
			{
			    u8c = *--p;
			    u8c += (గుర్తులేని)*--p << 8;
			    u8c += (గుర్తులేని)*--p << 16;
			    u8c += (గుర్తులేని)*--p << 24;
			}
		    }
		    లేకపోతే    // UTF-8
		    {
			ఒకవేళ (*--p < 0x80)
			    u8c = *p;
			లేకపోతే
			{
			    len = utf_తల_off(ptr, p);
			    p -= len;
			    u8c = utf_ptr2char(p);
			    ఒకవేళ (len == 0)
			    {
				// Not a valid UTF-8 character, retry with
				// another fenc when possible, otherwise just
				// report the error.
				ఒకవేళ (can_retry)
				    వెళ్ళు rewind_retry;
				ఒకవేళ (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				ఒకవేళ (bad_char_behavior == BAD_DROP)
				    కొనసాగు;
				ఒకవేళ (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    ఒకవేళ (enc_utf8)	// produce UTF-8
		    {
			dest -= utf_char2len(u8c);
			(శూన్య)utf_char2bytes(u8c, dest);
		    }
		    లేకపోతే		// produce Latin1
		    {
			--dest;
			ఒకవేళ (u8c >= 0x100)
			{
			    // character doesn't fit in latin1, retry with
			    // another fenc when possible, otherwise just
			    // report the error.
			    ఒకవేళ (can_retry)
				వెళ్ళు rewind_retry;
			    ఒకవేళ (conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
			    ఒకవేళ (bad_char_behavior == BAD_DROP)
				++dest;
			    లేకపోతే ఒకవేళ (bad_char_behavior == BAD_KEEP)
				*dest = u8c;
			    లేకపోతే ఒకవేళ (eap != ఖాళీ && eap->bad_char != 0)
				*dest = bad_char_behavior;
			    లేకపోతే
				*dest = 0xBF;
			}
			లేకపోతే
			    *dest = u8c;
		    }
		}

		// move the linerest to before the converted characters
		పంక్తి_ప్రారంభం = dest - linerest;
		mch_memmove(పంక్తి_ప్రారంభం, బఫరు, (size_t)linerest);
		కొలత = (దీర్ఘ)((ptr + real_కొలత) - dest);
		ptr = dest;
	    }
	    లేకపోతే ఒకవేళ (enc_utf8 && !curbuf->b_p_bin)
	    {
		పూర్ణాంకం  incomplete_తోక = FALSE;

		// Reading UTF-8: Check if the bytes are valid UTF-8.
		కొరకు (p = ptr; ; )
		{
		    పూర్ణాంకం	 todo;
		    పూర్ణాంకం	 l;

		    // Skip ASCII bytes quickly using word-at-a-time check.
		    {
			char_u *ascii_చివర = ptr + కొలత;
			అయితే (ascii_చివర - p >= (దీర్ఘ)పరిమాణం(long_u))
			{
			    long_u మాట;
			    జ్ఞాపకం_నకలు(&మాట, p, పరిమాణం(long_u));
			    ఒకవేళ (మాట & NONASCII_ముసుగు)
				ఆపు;
			    p += పరిమాణం(long_u);
			}
			అయితే (p < ascii_చివర && *p < 0x80)
			    ++p;
		    }

		    todo = (పూర్ణాంకం)((ptr + కొలత) - p);
		    ఒకవేళ (todo <= 0)
			ఆపు;
		    ఒకవేళ (*p >= 0x80)
		    {
			// A length of 1 means it's an illegal byte.  Accept
			// an incomplete character at the end though, the next
			// read() will get the next bytes, we'll check it
			// then.
			l = utf_ptr2len_len(p, todo);
			ఒకవేళ (l > todo && !incomplete_తోక)
			{
			    // Avoid retrying with a different encoding when
			    // a truncated file is more likely, or attempting
			    // to read the rest of an incomplete sequence when
			    // we have already done so.
			    ఒకవేళ (p > ptr || filesize > 0)
				incomplete_తోక = TRUE;
			    // Incomplete byte sequence, move it to conv_rest[]
			    // and try to read the rest of it, unless we've
			    // already done so.
			    ఒకవేళ (p > ptr)
			    {
				conv_restlen = todo;
				mch_memmove(conv_rest, p, conv_restlen);
				కొలత -= conv_restlen;
				ఆపు;
			    }
			}
			ఒకవేళ (l == 1 || l > todo)
			{
			    // Illegal byte.  If we can try another encoding
			    // do that, unless at EOF where a truncated
			    // file is more likely than a conversion error.
			    ఒకవేళ (can_retry && !incomplete_తోక)
				ఆపు;
#నిర్వచితమైతే USE_ICONV
			    // When we did a conversion report an error.
			    ఒకవేళ (iconv_fd != (iconv_t)-1 && conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
#అయితే_ముగింపు
			    // Remember the first linenr with an illegal byte
			    ఒకవేళ (conv_error == 0 && illegal_byte == 0)
				illegal_byte = readfile_linenr(linecnt, ptr, p);

			    // Drop, keep or replace the bad byte.
			    ఒకవేళ (bad_char_behavior == BAD_DROP)
			    {
				mch_memmove(p, p + 1, todo - 1);
				--కొలత;
			    }
			    లేకపోతే
			    {
				ఒకవేళ (bad_char_behavior != BAD_KEEP)
				    *p = bad_char_behavior;
				++p;
			    }
			}
			లేకపోతే
			    p += l;
		    }
		}
		ఒకవేళ (p < ptr + కొలత && !incomplete_తోక)
		{
		    // Detected a UTF-8 error.
rewind_retry:
		    // Retry reading with another conversion.
#ఒకవేళ defined(FEAT_EVAL) && defined(USE_ICONV)
		    ఒకవేళ (*p_ccv != NUL && iconv_fd != (iconv_t)-1)
			// iconv() failed, try 'charconvert'
			did_iconv = TRUE;
		    లేకపోతే
#అయితే_ముగింపు
			// use next item from 'fileencodings'
			advance_fenc = TRUE;
		    file_rewind = TRUE;
		    వెళ్ళు retry;
		}
	    }

	    // count the number of characters (after conversion!)
	    filesize += కొలత;

	    /*
	     * when reading the first part of a file: guess EOL type
	     */
	    ఒకవేళ (fileformat == EOL_UNKNOWN)
	    {
		// First try finding a NL, for Dos and Unix
		ఒకవేళ (try_dos || try_unix)
		{
		    // Reset the carriage return counter.
		    ఒకవేళ (try_mac)
			try_mac = 1;

		    కొరకు (p = ptr; p < ptr + కొలత; ++p)
		    {
			ఒకవేళ (*p == NL)
			{
			    ఒకవేళ (!try_unix
				    || (try_dos && p > ptr && p[-1] == CAR))
				fileformat = EOL_DOS;
			    లేకపోతే
				fileformat = EOL_UNIX;
			    ఆపు;
			}
			లేకపోతే ఒకవేళ (*p == CAR && try_mac)
			    try_mac++;
		    }

		    // Don't give in to EOL_UNIX if EOL_MAC is more likely
		    ఒకవేళ (fileformat == EOL_UNIX && try_mac)
		    {
			// Need to reset the counters when retrying fenc.
			try_mac = 1;
			try_unix = 1;
			కొరకు (; p >= ptr && *p != CAR; p--)
			    ;
			ఒకవేళ (p >= ptr)
			{
			    కొరకు (p = ptr; p < ptr + కొలత; ++p)
			    {
				ఒకవేళ (*p == NL)
				    try_unix++;
				లేకపోతే ఒకవేళ (*p == CAR)
				    try_mac++;
			    }
			    ఒకవేళ (try_mac > try_unix)
				fileformat = EOL_MAC;
			}
		    }
		    లేకపోతే ఒకవేళ (fileformat == EOL_UNKNOWN && try_mac == 1)
			// Looking for CR but found no end-of-line markers at
			// all: use the default format.
			fileformat = default_fileformat();
		}

		// No NL found: may use Mac format
		ఒకవేళ (fileformat == EOL_UNKNOWN && try_mac)
		    fileformat = EOL_MAC;

		// Still nothing found?  Use first format in 'ffs'
		ఒకవేళ (fileformat == EOL_UNKNOWN)
		    fileformat = default_fileformat();

		// if editing a new file: may set p_tx and p_ff
		ఒకవేళ (సమితి_options)
		    సమితి_fileformat(fileformat, OPT_LOCAL);
	    }
	}

	/*
	 * This loop is executed once for every character read.
	 * Keep it fast!
	 */
	ఒకవేళ (fileformat == EOL_MAC)
	{
	    --ptr;
	    అయితే (++ptr, --కొలత >= 0)
	    {
		// catch most common case first
		ఒకవేళ ((c = *ptr) != NUL && c != CAR && c != NL)
		    కొనసాగు;
		ఒకవేళ (c == NUL)
		    *ptr = NL;	// NULs are replaced by newlines!
		లేకపోతే ఒకవేళ (c == NL)
		    *ptr = CAR;	// NLs are replaced by CRs!
		లేకపోతే
		{
		    ఒకవేళ (skip_లెక్క == 0)
		    {
			*ptr = NUL;	    // end of line
			len = (colnr_T) (ptr - పంక్తి_ప్రారంభం + 1);
			ఒకవేళ (ml_append(lnum, పంక్తి_ప్రారంభం, len, newfile) == FAIL)
			{
			    error = TRUE;
			    ఆపు;
			}
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
			ఒకవేళ (read_undo_file)
			    sha256_update(&sha_ctx, పంక్తి_ప్రారంభం, len);
#అయితే_ముగింపు
			++lnum;
			ఒకవేళ (--read_లెక్క == 0)
			{
			    error = TRUE;	// break loop
			    పంక్తి_ప్రారంభం = ptr;	// nothing left to write
			    ఆపు;
			}
		    }
		    లేకపోతే
			--skip_లెక్క;
		    పంక్తి_ప్రారంభం = ptr + 1;
		}
	    }
	}
	లేకపోతే
	{
	    // Use memchr() for SIMD-optimized newline scanning instead
	    // of scanning each byte individually.
	    char_u *చివర = ptr + కొలత;

	    అయితే (ptr < చివర)
	    {
		char_u *nl = (char_u *)memchr(ptr, NL, చివర - ptr);
		char_u *nul_పరిశీలన;

		ఒకవేళ (nl == ఖాళీ)
		{
		    // No more newlines in buffer.
		    // Replace any NUL bytes with NL in remaining data.
		    అయితే ((nul_పరిశీలన = (char_u *)memchr(ptr, NUL,
						      చివర - ptr)) != ఖాళీ)
		    {
			*nul_పరిశీలన = NL;
			ptr = nul_పరిశీలన + 1;
		    }
		    ptr = చివర;
		    ఆపు;
		}

		// Replace NUL bytes with NL before the newline.
		{
		    char_u *పరిశీలన = ptr;
		    అయితే ((nul_పరిశీలన = (char_u *)memchr(పరిశీలన, NUL,
						       nl - పరిశీలన)) != ఖాళీ)
		    {
			*nul_పరిశీలన = NL;
			పరిశీలన = nul_పరిశీలన + 1;
		    }
		}

		// Process the newline.
		ptr = nl;
		ఒకవేళ (skip_లెక్క == 0)
		{
		    *ptr = NUL;		// end of line
		    len = (colnr_T)(ptr - పంక్తి_ప్రారంభం + 1);
		    ఒకవేళ (fileformat == EOL_DOS)
		    {
			ఒకవేళ (ptr > పంక్తి_ప్రారంభం && ptr[-1] == CAR)
			{
			    // remove CR before NL
			    ptr[-1] = NUL;
			    --len;
			}
			/*
			 * Reading in Dos format, but no CR-LF found!
			 * When 'fileformats' includes "unix", delete all
			 * the lines read so far and start all over again.
			 * Otherwise give an error message later.
			 */
			లేకపోతే ఒకవేళ (ff_error != EOL_DOS)
			{
			    ఒకవేళ (   try_unix
				&& !read_stdin
				&& (read_బఫరు
				    || vim_lseek(fd, (off_T)0L, SEEK_SET)
								      == 0))
			    {
				fileformat = EOL_UNIX;
				ఒకవేళ (సమితి_options)
				    సమితి_fileformat(EOL_UNIX, OPT_LOCAL);
				file_rewind = TRUE;
				keep_fileformat = TRUE;
				వెళ్ళు retry;
			    }
			    ff_error = EOL_DOS;
			}
		    }
		    ఒకవేళ (ml_append(lnum, పంక్తి_ప్రారంభం, len, newfile) == FAIL)
		    {
			error = TRUE;
			ఆపు;
		    }
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
		    ఒకవేళ (read_undo_file)
			sha256_update(&sha_ctx, పంక్తి_ప్రారంభం, len);
#అయితే_ముగింపు
		    ++lnum;
		    ఒకవేళ (--read_లెక్క == 0)
		    {
			error = TRUE;	    // break loop
			పంక్తి_ప్రారంభం = ptr;   // nothing left to write
			ఆపు;
		    }
		}
		లేకపోతే
		    --skip_లెక్క;
		పంక్తి_ప్రారంభం = ptr + 1;
		++ptr;
	    }
	    కొలత = -1;
	}
	linerest = (దీర్ఘ)(ptr - పంక్తి_ప్రారంభం);
	ui_breakcheck();
    }

failed:
    // not an error, max. number of lines reached
    ఒకవేళ (error && read_లెక్క == 0)
	error = FALSE;

    // In Dos format ignore a trailing CTRL-Z, unless 'binary' is set.
    // In old days the file length was in sector count and the CTRL-Z the
    // marker where the file really ended.  Assuming we write it to a file
    // system that keeps file length properly the CTRL-Z should be dropped.
    // Set the 'endoffile' option so the user can decide what to write later.
    // In Unix format the CTRL-Z is just another character.
    ఒకవేళ (linerest != 0
	    && !curbuf->b_p_bin
	    && fileformat == EOL_DOS
	    && ptr[-1] == Ctrl_Z)
    {
	ptr--;
	linerest--;
	ఒకవేళ (సమితి_options)
	    curbuf->b_p_eof = TRUE;
    }

    // If we get EOF in the middle of a line, note the fact by resetting
    // 'endofline' and add the line normally.
    ఒకవేళ (!error
	    && !got_int
	    && linerest != 0)
    {
	// remember for when writing
	ఒకవేళ (సమితి_options)
	    curbuf->b_p_eol = FALSE;
	*ptr = NUL;
	len = (colnr_T)(ptr - పంక్తి_ప్రారంభం + 1);
	ఒకవేళ (ml_append(lnum, పంక్తి_ప్రారంభం, len, newfile) == FAIL)
	    error = TRUE;
	లేకపోతే
	{
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
	    ఒకవేళ (read_undo_file)
		sha256_update(&sha_ctx, పంక్తి_ప్రారంభం, len);
#అయితే_ముగింపు
	    read_no_eol_lnum = ++lnum;
	}
    }

    ఒకవేళ (సమితి_options)
	save_file_ff(curbuf);		// remember the current file format

#నిర్వచితమైతే FEAT_CRYPT
    ఒకవేళ (curbuf->b_cryptstate != ఖాళీ)
    {
	crypt_free_state(curbuf->b_cryptstate);
	curbuf->b_cryptstate = ఖాళీ;
    }
    ఒకవేళ (cryptkey != ఖాళీ && cryptkey != curbuf->b_p_కీలకం)
	crypt_free_కీలకం(cryptkey);
    // Don't set cryptkey to NULL, it's used below as a flag that
    // encryption was used.
#అయితే_ముగింపు

    // If editing a new file: set 'fenc' for the current buffer.
    // Also for ":read ++edit file".
    ఒకవేళ (సమితి_options)
	సమితి_string_option_ప్రత్యక్ష((char_u *)"fenc", -1, fenc,
						       OPT_FREE|OPT_LOCAL, 0);
    ఒకవేళ (fenc_alloced)
	vim_free(fenc);
#నిర్వచితమైతే USE_ICONV
    ఒకవేళ (iconv_fd != (iconv_t)-1)
	iconv_close(iconv_fd);
#అయితే_ముగింపు

    ఒకవేళ (!read_బఫరు && !read_stdin)
	close(fd);				// errors are ignored
#నిర్వచితమైతే HAVE_FD_CLOEXEC
    లేకపోతే
    {
	పూర్ణాంకం fdflags = fcntl(fd, F_GETFD);

	ఒకవేళ (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (శూన్య)fcntl(fd, F_SETFD, fdflags | FD_CLOEXEC);
    }
#అయితే_ముగింపు
    vim_free(బఫరు);

#నిర్వచితమైతే HAVE_DUP
    ఒకవేళ (read_stdin)
    {
	// Use stderr for stdin, makes shell commands work.
	close(0);
	vim_ignored = dup(2);
    }
#అయితే_ముగింపు

    ఒకవేళ (tmpname != ఖాళీ)
    {
	mch_remove(tmpname);		// delete converted file
	vim_free(tmpname);
    }
    --no_wait_return;			// may wait for return now

    /*
     * In recovery mode everything but autocommands is skipped.
     */
    ఒకవేళ (!recoverymode)
    {
	// need to delete the last line, which comes from the empty buffer
	ఒకవేళ (newfile && wasempty && !(curbuf->b_ml.ml_flags & ML_ఖాళీలు))
	{
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	    netbeansFireChanges = 0;
#అయితే_ముగింపు
	    ml_delete(curbuf->b_ml.ml_పంక్తి_లెక్క);
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	    netbeansFireChanges = 1;
#అయితే_ముగింపు
	    --linecnt;
	}
	linecnt = curbuf->b_ml.ml_పంక్తి_లెక్క - linecnt;
	ఒకవేళ (filesize == 0)
	    linecnt = 0;
	ఒకవేళ (newfile || read_బఫరు)
	{
	    redraw_curbuf_later(UPD_NOT_VALID);
#నిర్వచితమైతే FEAT_DIFF
	    // After reading the text into the buffer the diff info needs to
	    // be updated.
	    diff_invalidate(curbuf);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_FOLDING
	    // All folds in the window are invalid now.  Mark them for update
	    // before triggering autocommands.
	    foldUpdateAll(curwin);
#అయితే_ముగింపు
	}
	లేకపోతే ఒకవేళ (linecnt)		// appended at least one line
	    appended_పంక్తులు_mark(from, linecnt);

#నిర్వచితం_కాకపోతే ALWAYS_USE_GUI
	/*
	 * If we were reading from the same terminal as where messages go,
	 * the screen will have been messed up.
	 * Switch on raw mode now and clear the screen.
	 */
	ఒకవేళ (read_stdin)
	{
	    settmode(TMODE_RAW);	// set to raw mode
	    starttermcap();
	    screenclear();
	}
#అయితే_ముగింపు

	ఒకవేళ (got_int)
	{
	    ఒకవేళ (!(flags & READ_DUMMY))
	    {
		filemess(curbuf, sfname, (char_u *)_(e_interrupted), 0);
		ఒకవేళ (newfile)
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    }
	    సందేశం_scroll = సందేశం_save;
#నిర్వచితమైతే FEAT_VIMINFO
	    check_marks_read();
#అయితే_ముగింపు
	    retval = OK;	// an interrupt isn't really an error
	    వెళ్ళు theend;
	}

	ఒకవేళ (!filtering && !(flags & READ_DUMMY))
	{
	    పూర్ణాంకం	buflen;

	    సందేశం_కూడు_fname(curbuf, sfname);   // fname in IObuff with quotes
	    c = FALSE;

	    buflen = (పూర్ణాంకం)STRLEN(IObuff);
#నిర్వచితమైతే UNIX
	    ఒకవేళ (S_ISFIFO(perm))			    // fifo
	    {
		buflen += vim_snprintf((అక్షరం *)IObuff + buflen, IOSIZE - buflen,
			_("[fifo]"));
		c = TRUE;
	    }
	    ఒకవేళ (S_ISSOCK(perm))			    // or socket
	    {
		buflen += vim_snprintf((అక్షరం *)IObuff + buflen, IOSIZE - buflen,
			_("[socket]"));
		c = TRUE;
	    }
# ifdef OPEN_CHR_FILES
	    ఒకవేళ (S_ISCHR(perm))			    // or character special
	    {
		buflen += vim_snprintf((అక్షరం *)IObuff + buflen, IOSIZE - buflen,
			_("[character special]"));
		c = TRUE;
	    }
# endif
#అయితే_ముగింపు
	    ఒకవేళ (curbuf->b_p_ro)
	    {
		buflen += vim_snprintf((అక్షరం *)IObuff + buflen, IOSIZE - buflen,
			"%s", shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
		c = TRUE;
	    }
	    ఒకవేళ (read_no_eol_lnum)
	    {
		సందేశం_కూడు_eol();
		c = TRUE;
	    }
	    ఒకవేళ (ff_error == EOL_DOS)
	    {
		buflen += vim_snprintf((అక్షరం *)IObuff + buflen, IOSIZE - buflen,
			_("[CR missing]"));
		c = TRUE;
	    }
	    ఒకవేళ (విభజించు)
	    {
		buflen += vim_snprintf((అక్షరం *)IObuff + buflen, IOSIZE - buflen,
			_("[long lines split]"));
		c = TRUE;
	    }
	    ఒకవేళ (notconverted)
	    {
		buflen += vim_snprintf((అక్షరం *)IObuff + buflen, IOSIZE - buflen,
			_("[NOT converted]"));
		c = TRUE;
	    }
	    లేకపోతే ఒకవేళ (converted)
	    {
		buflen += vim_snprintf((అక్షరం *)IObuff + buflen, IOSIZE - buflen,
			_("[converted]"));
		c = TRUE;
	    }
#నిర్వచితమైతే FEAT_CRYPT
	    ఒకవేళ (cryptkey != ఖాళీ)
	    {
		crypt_append_సందేశం(curbuf);
		c = TRUE;
	    }
#అయితే_ముగింపు
	    ఒకవేళ (conv_error != 0)
	    {
		vim_snprintf((అక్షరం *)IObuff + buflen, IOSIZE - buflen,
			_("[CONVERSION ERROR in line %ld]"), (దీర్ఘ)conv_error);
		c = TRUE;
	    }
	    లేకపోతే ఒకవేళ (illegal_byte > 0)
	    {
		vim_snprintf((అక్షరం *)IObuff + buflen, IOSIZE - buflen,
			_("[ILLEGAL BYTE in line %ld]"), (దీర్ఘ)illegal_byte);
		c = TRUE;
	    }
	    లేకపోతే ఒకవేళ (error)
	    {
		vim_snprintf((అక్షరం *)IObuff + buflen, IOSIZE - buflen,
			_("[READ ERRORS]"));
		c = TRUE;
	    }
	    ఒకవేళ (సందేశం_కూడు_fileformat(fileformat))
		c = TRUE;
#నిర్వచితమైతే FEAT_CRYPT
	    ఒకవేళ (cryptkey != ఖాళీ)
		సందేశం_కూడు_పంక్తులు(c, (దీర్ఘ)linecnt, filesize
			 - crypt_get_శీర్షిక_len(crypt_get_పద్ధతి_nr(curbuf)));
	    లేకపోతే
#అయితే_ముగింపు
		సందేశం_కూడు_పంక్తులు(c, (దీర్ఘ)linecnt, filesize);

	    VIM_CLEAR(keep_సందేశం);
	    సందేశం_scrolled_ign = TRUE;
#నిర్వచితమైతే ALWAYS_USE_GUI
	    // Don't show the message when reading stdin, it would end up in a
	    // message box (which might be shown when exiting!)
	    ఒకవేళ (read_stdin || read_బఫరు)
		p = సందేశం_may_trunc(FALSE, IObuff);
	    లేకపోతే
#అయితే_ముగింపు
	    {
		ఒకవేళ (సందేశం_నిలువు > 0)
		    సందేశం_putchar('\r');  // overwrite previous message
		p = (char_u *)సందేశం_trunc_attr((అక్షరం *)IObuff, FALSE, 0);
	    }
	    ఒకవేళ (read_stdin || read_బఫరు || restart_edit != 0
		    || (సందేశం_scrolled != 0 && !need_wait_return))
		// Need to repeat the message after redrawing when:
		// - When reading from stdin (the screen will be cleared next).
		// - When restart_edit is set (otherwise there will be a delay
		//   before redrawing).
		// - When the screen was scrolled but there is no wait-return
		//   prompt.
		సమితి_keep_సందేశం(p, 0);
	    సందేశం_scrolled_ign = FALSE;
	}

	// with errors writing the file requires ":w!"
	ఒకవేళ (newfile && (error
		    || conv_error != 0
		    || (illegal_byte > 0 && bad_char_behavior != BAD_KEEP)))
	    curbuf->b_p_ro = TRUE;

	u_clearline();	    // cannot use "U" command after adding lines

	/*
	 * In Ex mode: cursor at last new line.
	 * Otherwise: cursor at first new line.
	 */
	ఒకవేళ (exmode_చురుకైన)
	    curwin->w_cursor.lnum = from + linecnt;
	లేకపోతే
	    curwin->w_cursor.lnum = from + 1;
	check_cursor_lnum();
	beginline(BL_WHITE | BL_FIX);	    // on first non-blank

	ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set '[ and '] marks to the newly read lines.
	    curbuf->b_op_ప్రారంభం.lnum = from + 1;
	    curbuf->b_op_ప్రారంభం.నిలువు = 0;
	    curbuf->b_op_చివర.lnum = from + linecnt;
	    curbuf->b_op_చివర.నిలువు = 0;
	}

#నిర్వచితమైతే MSWIN
	/*
	 * Work around a weird problem: When a file has two links (only
	 * possible on NTFS) and we write through one link, then stat() it
	 * through the other link, the timestamp information may be wrong.
	 * It's correct again after reading the file, thus reset the timestamp
	 * here.
	 */
	ఒకవేళ (newfile && !read_stdin && !read_బఫరు
					 && mch_stat((అక్షరం *)fname, &st) >= 0)
	{
	    buf_store_సమయం(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
	}
#అయితే_ముగింపు
    }
    సందేశం_scroll = సందేశం_save;

#నిర్వచితమైతే FEAT_VIMINFO
    /*
     * Get the marks before executing autocommands, so they can be used there.
     */
    check_marks_read();
#అయితే_ముగింపు

    /*
     * We remember if the last line of the read didn't have
     * an eol even when 'binary' is off, to support turning 'fixeol' off,
     * or writing the read again with 'binary' on.  The latter is required
     * for ":autocmd FileReadPost *.gz set bin|'[,']!gunzip" to work.
     */
    curbuf->b_no_eol_lnum = read_no_eol_lnum;

    // When reloading a buffer put the cursor at the first line that is
    // different.
    ఒకవేళ (flags & READ_KEEP_UNDO)
	u_కనుగొను_మొదటి_మారింది();

#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
    /*
     * When opening a new file locate undo info and read it.
     */
    ఒకవేళ (read_undo_file)
    {
	char_u	hash[UNDO_HASH_కొలత];

	sha256_ముగింపు(&sha_ctx, hash);
	u_read_undo(ఖాళీ, hash, fname);
    }
#అయితే_ముగింపు

    ఒకవేళ (!read_stdin && !read_fifo && (!read_బఫరు || sfname != ఖాళీ))
    {
	పూర్ణాంకం m = సందేశం_scroll;
	పూర్ణాంకం n = సందేశం_scrolled;

	// Save the fileformat now, otherwise the buffer will be considered
	// modified if the format/encoding was automatically detected.
	ఒకవేళ (సమితి_options)
	    save_file_ff(curbuf);

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	సందేశం_scroll = TRUE;
	ఒకవేళ (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPOST, ఖాళీ, sfname,
							  FALSE, curbuf, eap);
	లేకపోతే ఒకవేళ (newfile || (read_బఫరు && sfname != ఖాళీ))
	{
	    apply_autocmds_exarg(EVENT_BUFREADPOST, ఖాళీ, sfname,
							  FALSE, curbuf, eap);
	    ఒకవేళ (!curbuf->b_au_did_filetype && *curbuf->b_p_ft != NUL)
		/*
		 * EVENT_FILETYPE was not triggered but the buffer already has a
		 * filetype. Trigger EVENT_FILETYPE using the existing filetype.
		 */
		apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
			TRUE, curbuf);
	}
	లేకపోతే
	    apply_autocmds_exarg(EVENT_FILEREADPOST, sfname, sfname,
							    FALSE, ఖాళీ, eap);
	ఒకవేళ (సందేశం_scrolled == n)
	    సందేశం_scroll = m;
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (aborting())	    // autocmds may abort script processing
	    వెళ్ళు theend;
#అయితే_ముగింపు
    }

    ఒకవేళ (!(recoverymode && error))
	retval = OK;

theend:
    ఒకవేళ (curbuf->b_ml.ml_mfp != ఖాళీ
		       && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	// OK to sync the swap file now
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    తిరిగి retval;
}

#ఒకవేళ defined(OPEN_CHR_FILES)
/*
 * Returns TRUE if the file name argument is of the form "/dev/fd/\d\+",
 * which is the name of files used for process substitution output by
 * some shells on some operating systems, e.g., bash on SunOS.
 * Do not accept "/dev/fd/[012]", opening these may hang Vim.
 */
    పూర్ణాంకం
is_dev_fd_file(char_u *fname)
{
    తిరిగి (STRNCMP(fname, "/dev/fd/", 8) == 0
	    && VIM_ISDIGIT(fname[8])
	    && *skipdigits(fname + 9) == NUL
	    && (fname[9] != NUL
		|| (fname[8] != '0' && fname[8] != '1' && fname[8] != '2')));
}
#అయితే_ముగింపు

/*
 * From the current line count and characters read after that, estimate the
 * line number where we are now.
 * Used for error messages that include a line number.
 */
    స్థిర linenr_T
readfile_linenr(
    linenr_T	linecnt,	// line count before reading more bytes
    char_u	*p,		// start of more bytes read
    char_u	*endp)		// end of more bytes read
{
    char_u	*s;
    linenr_T	lnum;

    lnum = curbuf->b_ml.ml_పంక్తి_లెక్క - linecnt + 1;
    కొరకు (s = p; s < endp; ++s)
	ఒకవేళ (*s == '\n')
	    ++lnum;
    తిరిగి lnum;
}

/*
 * Fill "*eap" to force the 'fileencoding', 'fileformat' and 'binary' to be
 * equal to the buffer "buf".  Used for calling readfile().
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
prep_exarg(exarg_T *eap, buf_T *buf)
{
    eap->cmd = alloc(15 + (గుర్తులేని)STRLEN(buf->b_p_fenc));
    ఒకవేళ (eap->cmd == ఖాళీ)
	తిరిగి FAIL;

    sprintf((అక్షరం *)eap->cmd, "e ++enc=%s", buf->b_p_fenc);
    eap->force_enc = 8;
    eap->bad_char = buf->b_bad_char;
    eap->force_ff = *buf->b_p_ff;

    eap->force_bin = buf->b_p_bin ? FORCE_BIN : FORCE_NOBIN;
    eap->read_edit = FALSE;
    eap->forceit = FALSE;
    తిరిగి OK;
}

/*
 * Set default or forced 'fileformat' and 'binary'.
 */
    శూన్య
సమితి_file_options(పూర్ణాంకం సమితి_options, exarg_T *eap)
{
    // set default 'fileformat'
    ఒకవేళ (సమితి_options)
    {
	ఒకవేళ (eap != ఖాళీ && eap->force_ff != 0)
	    సమితి_fileformat(get_fileformat_force(curbuf, eap), OPT_LOCAL);
	లేకపోతే ఒకవేళ (*p_ffs != NUL)
	    సమితి_fileformat(default_fileformat(), OPT_LOCAL);
    }

    // set or reset 'binary'
    ఒకవేళ (eap != ఖాళీ && eap->force_bin != 0)
    {
	పూర్ణాంకం	oldval = curbuf->b_p_bin;

	curbuf->b_p_bin = (eap->force_bin == FORCE_BIN);
	సమితి_options_bin(oldval, curbuf->b_p_bin, OPT_LOCAL);
    }
}

/*
 * Set forced 'fileencoding'.
 */
    శూన్య
సమితి_forced_fenc(exarg_T *eap)
{
    ఒకవేళ (eap->force_enc == 0)
	తిరిగి;

    char_u *fenc = enc_canonize(eap->cmd + eap->force_enc);

    ఒకవేళ (fenc != ఖాళీ)
	సమితి_string_option_ప్రత్యక్ష((char_u *)"fenc", -1,
		fenc, OPT_FREE|OPT_LOCAL, 0);
    vim_free(fenc);
}

/*
 * Find next fileencoding to use from 'fileencodings'.
 * "pp" points to fenc_next.  It's advanced to the next item.
 * When there are no more items, an empty string is returned and *pp is set to
 * NULL.
 * When *pp is not set to NULL, the result is in allocated memory and "alloced"
 * is set to TRUE.
 */
    స్థిర char_u *
తదుపరి_fenc(char_u **pp, పూర్ణాంకం *alloced)
{
    char_u	*p;
    char_u	*r;

    *alloced = FALSE;
    ఒకవేళ (**pp == NUL)
    {
	*pp = ఖాళీ;
	తిరిగి (char_u *)"";
    }
    p = vim_strchr(*pp, ',');
    ఒకవేళ (p == ఖాళీ)
    {
	r = enc_canonize(*pp);
	*pp += STRLEN(*pp);
    }
    లేకపోతే
    {
	r = vim_strnsave(*pp, p - *pp);
	*pp = p + 1;
	ఒకవేళ (r != ఖాళీ)
	{
	    p = enc_canonize(r);
	    vim_free(r);
	    r = p;
	}
    }
    ఒకవేళ (r != ఖాళీ)
	*alloced = TRUE;
    లేకపోతే
    {
	// out of memory
	r = (char_u *)"";
	*pp = ఖాళీ;
    }
    తిరిగి r;
}

#నిర్వచితమైతే FEAT_EVAL
/*
 * Convert a file with the 'charconvert' expression.
 * This closes the file which is to be read, converts it and opens the
 * resulting file for reading.
 * Returns name of the resulting converted file (the caller should delete it
 * after reading it).
 * Returns NULL if the conversion failed ("*fdp" is not set) .
 */
    స్థిర char_u *
readfile_charconvert(
    char_u	*fname,		// name of input file
    char_u	*fenc,		// converted from
    పూర్ణాంకం		*fdp)		// in/out: file descriptor of file
{
    char_u	*tmpname;
    అక్షరం	*errmsg = ఖాళీ;

    tmpname = vim_tempname('r', FALSE);
    ఒకవేళ (tmpname == ఖాళీ)
	errmsg = _("Can't find temp file for conversion");
    లేకపోతే
    {
	close(*fdp);		// close the input file, ignore errors
	*fdp = -1;
	ఒకవేళ (eval_charconvert(fenc, enc_utf8 ? (char_u *)"utf-8" : p_enc,
						      fname, tmpname) == FAIL)
	    errmsg = _("Conversion with 'charconvert' failed");
	ఒకవేళ (errmsg == ఖాళీ && (*fdp = mch_open((అక్షరం *)tmpname,
						  O_RDONLY | O_EXTRA, 0)) < 0)
	    errmsg = _("can't read output of 'charconvert'");
    }

    ఒకవేళ (errmsg != ఖాళీ)
    {
	// Don't use emsg(), it breaks mappings, the retry with
	// another type of conversion might still work.
	సందేశం(errmsg);
	ఒకవేళ (tmpname != ఖాళీ)
	{
	    mch_remove(tmpname);	// delete converted file
	    VIM_CLEAR(tmpname);
	}
    }

    // If the input file is closed, open it (caller should check for error).
    ఒకవేళ (*fdp < 0)
	*fdp = mch_open((అక్షరం *)fname, O_RDONLY | O_EXTRA, 0);

    తిరిగి tmpname;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_CRYPT)
/*
 * Check for magic number used for encryption.  Applies to the current buffer.
 * If found, the magic number is removed from ptr[*sizep] and *sizep and
 * *filesizep are updated.
 * Return the (new) encryption key, NULL for no encryption.
 */
    స్థిర char_u *
check_for_cryptkey(
    char_u	*cryptkey,	// previous encryption key or NULL
    char_u	*ptr,		// pointer to read bytes
    దీర్ఘ	*sizep,		// length of read bytes
    off_T	*filesizep,	// nr of bytes used from file
    పూర్ణాంకం		newfile,	// editing a new buffer
    char_u	*fname,		// file name to display
    పూర్ణాంకం		*did_ask)	// flag: whether already asked for key
{
    పూర్ణాంకం పద్ధతి = crypt_పద్ధతి_nr_from_magic((అక్షరం *)ptr, *sizep);
    పూర్ణాంకం b_p_ro = curbuf->b_p_ro;

    ఒకవేళ (పద్ధతి >= 0)
    {
	// Mark the buffer as read-only until the decryption has taken place.
	// Avoids accidentally overwriting the file with garbage.
	curbuf->b_p_ro = TRUE;

	// Set the cryptmethod local to the buffer.
	crypt_సమితి_cm_option(curbuf, పద్ధతి);
	ఒకవేళ (cryptkey == ఖాళీ && !*did_ask)
	{
	    ఒకవేళ (*curbuf->b_p_కీలకం)
	    {
		cryptkey = curbuf->b_p_కీలకం;
		crypt_check_swapfile_curbuf();
	    }
	    లేకపోతే
	    {
		// When newfile is TRUE, store the typed key in the 'key'
		// option and don't free it.  bf needs hash of the key saved.
		// Don't ask for the key again when first time Enter was hit.
		// Happens when retrying to detect encoding.
		smsg(_(need_కీలకం_సందేశం), fname);
		సందేశం_scroll = TRUE;
		crypt_check_పద్ధతి(పద్ధతి);
		cryptkey = crypt_get_కీలకం(newfile, FALSE);
		*did_ask = TRUE;

		// check if empty key entered
		ఒకవేళ (cryptkey != ఖాళీ && *cryptkey == NUL)
		{
		    ఒకవేళ (cryptkey != curbuf->b_p_కీలకం)
			vim_free(cryptkey);
		    cryptkey = ఖాళీ;
		}
	    }
	}

	ఒకవేళ (cryptkey != ఖాళీ)
	{
	    పూర్ణాంకం శీర్షిక_len;

	    శీర్షిక_len = crypt_get_శీర్షిక_len(పద్ధతి);
	    ఒకవేళ (*sizep < శీర్షిక_len)
	    {
		// invalid header, buffer can't be encrypted
		ఒకవేళ (cryptkey != curbuf->b_p_కీలకం)
		    vim_free(cryptkey);
		తిరిగి ఖాళీ;
	    }

	    curbuf->b_cryptstate = crypt_create_from_శీర్షిక(
							పద్ధతి, cryptkey, ptr);
	    crypt_సమితి_cm_option(curbuf, పద్ధతి);

	    // Remove cryptmethod specific header from the text.
	    *filesizep += శీర్షిక_len;
	    *sizep -= శీర్షిక_len;
	    mch_memmove(ptr, ptr + శీర్షిక_len, (size_t)*sizep);

	    // Restore the read-only flag.
	    curbuf->b_p_ro = b_p_ro;
	}
    }
    // When starting to edit a new file which does not have encryption, clear
    // the 'key' option, except when starting up (called with -x argument)
    లేకపోతే ఒకవేళ (newfile && *curbuf->b_p_కీలకం != NUL && !starting)
	సమితి_option_విలువ_give_err((char_u *)"key", 0L, (char_u *)"", OPT_LOCAL);

    తిరిగి cryptkey;
}
#అయితే_ముగింపు  // FEAT_CRYPT

/*
 * Return TRUE if a file appears to be read-only from the file permissions.
 */
    పూర్ణాంకం
check_file_readonly(
    char_u	*fname,		// full path to file
    పూర్ణాంకం		perm వాడని)	// known permissions on file
{
#నిర్వచితం_కాకపోతే USE_MCH_ACCESS
    పూర్ణాంకం	    fd = 0;
#అయితే_ముగింపు

    తిరిగి (
#నిర్వచితమైతే USE_MCH_ACCESS
# ifdef UNIX
	(perm & 0222) == 0 ||
# endif
	mch_access((అక్షరం *)fname, W_OK)
#లేకపోతే
	(fd = mch_open((అక్షరం *)fname, O_RDWR | O_EXTRA, 0)) < 0
					? TRUE : (close(fd), FALSE)
#అయితే_ముగింపు
	);
}

#ఒకవేళ defined(HAVE_FSYNC)
/*
 * Call fsync() with Mac-specific exception.
 * Return fsync() result: zero for success.
 */
    పూర్ణాంకం
vim_fsync(పూర్ణాంకం fd)
{
    పూర్ణాంకం r;

# ifdef MACOS_X
    r = fcntl(fd, F_FULLFSYNC);
    ఒకవేళ (r != 0)  // F_FULLFSYNC not working or not supported
# endif
	r = fsync(fd);
    తిరిగి r;
}
#అయితే_ముగింపు

/*
 * Set the name of the current buffer.  Use when the buffer doesn't have a
 * name and a ":r" or ":w" command with a file name is used.
 */
    పూర్ణాంకం
సమితి_rw_fname(char_u *fname, char_u *sfname)
{
    buf_T	*buf = curbuf;

    // It's like the unnamed buffer is deleted....
    ఒకవేళ (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFDELETE, ఖాళీ, ఖాళీ, FALSE, curbuf);
    apply_autocmds(EVENT_BUFWIPEOUT, ఖాళీ, ఖాళీ, FALSE, curbuf);
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (aborting())	    // autocmds may abort script processing
	తిరిగి FAIL;
#అయితే_ముగింపు
    ఒకవేళ (curbuf != buf)
    {
	// We are in another buffer now, don't do the renaming.
	emsg(_(e_autocommands_మారింది_బఫరు_or_బఫరు_name));
	తిరిగి FAIL;
    }

    ఒకవేళ (setfname(curbuf, fname, sfname, FALSE) == OK)
	curbuf->b_flags |= BF_NOTEDITED;

    // ....and a new named one is created
    apply_autocmds(EVENT_BUFNEW, ఖాళీ, ఖాళీ, FALSE, curbuf);
    ఒకవేళ (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFADD, ఖాళీ, ఖాళీ, FALSE, curbuf);
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (aborting())	    // autocmds may abort script processing
	తిరిగి FAIL;
#అయితే_ముగింపు

    // Do filetype detection now if 'filetype' is empty.
    ఒకవేళ (*curbuf->b_p_ft == NUL)
    {
	ఒకవేళ (au_has_సమూహం((char_u *)"filetypedetect"))
	    (శూన్య)do_doautocmd((char_u *)"filetypedetect BufRead", FALSE, ఖాళీ);
	do_modelines(0);
    }

    తిరిగి OK;
}

/*
 * Put file name into IObuff with quotes.
 */
    శూన్య
సందేశం_కూడు_fname(buf_T *buf, char_u *fname)
{
    size_t  IObufflen = 0;

    ఒకవేళ (fname == ఖాళీ)
	fname = (char_u *)"-stdin-";
    IObuff[IObufflen++] = '"';
    IObufflen += home_replace(buf, fname, IObuff + IObufflen, IOSIZE - 4, TRUE);
    STRCPY(IObuff + IObufflen, "\" ");
}

/*
 * Append message for text mode to IObuff.
 * Return TRUE if something appended.
 */
    పూర్ణాంకం
సందేశం_కూడు_fileformat(పూర్ణాంకం eol_type)
{
#నిర్వచితం_కాకపోతే USE_CRNL
    ఒకవేళ (eol_type == EOL_DOS)
    {
	STRCAT(IObuff, shortmess(SHM_పాఠ్యం) ? _("[dos]") : _("[dos format]"));
	తిరిగి TRUE;
    }
#అయితే_ముగింపు
    ఒకవేళ (eol_type == EOL_MAC)
    {
	STRCAT(IObuff, shortmess(SHM_పాఠ్యం) ? _("[mac]") : _("[mac format]"));
	తిరిగి TRUE;
    }
#నిర్వచితమైతే USE_CRNL
    ఒకవేళ (eol_type == EOL_UNIX)
    {
	STRCAT(IObuff, shortmess(SHM_పాఠ్యం) ? _("[unix]") : _("[unix format]"));
	తిరిగి TRUE;
    }
#అయితే_ముగింపు
    తిరిగి FALSE;
}

/*
 * Append line and character count to IObuff.
 */
    శూన్య
సందేశం_కూడు_పంక్తులు(
    పూర్ణాంకం	    చొప్పించు_space,
    దీర్ఘ    lnum,
    off_T   nchars)
{
    పూర్ణాంకం  len = (పూర్ణాంకం)STRLEN(IObuff);

    ఒకవేళ (shortmess(SHM_పంక్తులు))
	vim_snprintf((అక్షరం *)IObuff + len, IOSIZE - (size_t)len,
		// l10n: L as in line, B as in byte
		_("%s%ldL, %lldB"), చొప్పించు_space ? " " : "", lnum, (varnumber_T)nchars);
    లేకపోతే
    {
	len += vim_snprintf((అక్షరం *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%s%ld line, ", "%s%ld lines, ", lnum), చొప్పించు_space ? " " : "", lnum);
	vim_snprintf((అక్షరం *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%lld byte", "%lld bytes", nchars), (varnumber_T)nchars);
    }
}

/*
 * Append message for missing line separator to IObuff.
 */
    శూన్య
సందేశం_కూడు_eol(శూన్య)
{
    STRCAT(IObuff, shortmess(SHM_చివరి) ? _("[noeol]") : _("[Incomplete last line]"));
}

    పూర్ణాంకం
సమయం_differs(stat_T *st, దీర్ఘ mtime, దీర్ఘ mtime_ns వాడని)
{
    తిరిగి
#నిర్వచితమైతే ST_MTIM_NSEC
	(దీర్ఘ)st->ST_MTIM_NSEC != mtime_ns ||
#అయితే_ముగింపు
#ఒకవేళ defined(__linux__) || defined(MSWIN)
	// On a FAT filesystem, esp. under Linux, there are only 5 bits to store
	// the seconds.  Since the roundoff is done when flushing the inode, the
	// time may change unexpectedly by one second!!!
	(దీర్ఘ)st->st_mtime - mtime > 1 || mtime - (దీర్ఘ)st->st_mtime > 1
#లేకపోతే
	(దీర్ఘ)st->st_mtime != mtime
#అయితే_ముగింపు
	;
}

/*
 * Return TRUE if file encoding "fenc" requires conversion from or to
 * 'encoding'.
 */
    పూర్ణాంకం
need_conversion(char_u *fenc)
{
    పూర్ణాంకం		same_encoding;
    పూర్ణాంకం		enc_flags;
    పూర్ణాంకం		fenc_flags;

    ఒకవేళ (*fenc == NUL || STRCMP(p_enc, fenc) == 0)
    {
	same_encoding = TRUE;
	fenc_flags = 0;
    }
    లేకపోతే
    {
	// Ignore difference between "ansi" and "latin1", "ucs-4" and
	// "ucs-4be", etc.
	enc_flags = get_fio_flags(p_enc);
	fenc_flags = get_fio_flags(fenc);
	same_encoding = (enc_flags != 0 && fenc_flags == enc_flags);
    }
    ఒకవేళ (same_encoding)
    {
	// Specified encoding matches with 'encoding'.  This requires
	// conversion when 'encoding' is Unicode but not UTF-8.
	తిరిగి enc_unicode != 0;
    }

    // Encodings differ.  However, conversion is not needed when 'enc' is any
    // Unicode encoding and the file is UTF-8.
    తిరిగి !(enc_utf8 && fenc_flags == FIO_UTF8);
}

/*
 * Check "ptr" for a unicode encoding and return the FIO_ flags needed for the
 * internal conversion.
 * if "ptr" is an empty string, use 'encoding'.
 */
    పూర్ణాంకం
get_fio_flags(char_u *ptr)
{
    పూర్ణాంకం		prop;

    ఒకవేళ (*ptr == NUL)
	ptr = p_enc;

    prop = enc_canon_props(ptr);
    ఒకవేళ (prop & ENC_UNICODE)
    {
	ఒకవేళ (prop & ENC_2BYTE)
	{
	    ఒకవేళ (prop & ENC_ENDIAN_L)
		తిరిగి FIO_UCS2 | FIO_ENDIAN_L;
	    తిరిగి FIO_UCS2;
	}
	ఒకవేళ (prop & ENC_4BYTE)
	{
	    ఒకవేళ (prop & ENC_ENDIAN_L)
		తిరిగి FIO_UCS4 | FIO_ENDIAN_L;
	    తిరిగి FIO_UCS4;
	}
	ఒకవేళ (prop & ENC_2_మాట)
	{
	    ఒకవేళ (prop & ENC_ENDIAN_L)
		తిరిగి FIO_UTF16 | FIO_ENDIAN_L;
	    తిరిగి FIO_UTF16;
	}
	తిరిగి FIO_UTF8;
    }
    ఒకవేళ (prop & ENC_LATIN1)
	తిరిగి FIO_LATIN1;
    // must be ENC_DBCS, requires iconv()
    తిరిగి 0;
}

#ఒకవేళ defined(MSWIN)
/*
 * Check "ptr" for a MS-Windows codepage name and return the FIO_ flags needed
 * for the conversion MS-Windows can do for us.  Also accept "utf-8".
 * Used for conversion between 'encoding' and 'fileencoding'.
 */
    పూర్ణాంకం
get_win_fio_flags(char_u *ptr)
{
    పూర్ణాంకం		cp;

    // Cannot do this when 'encoding' is not utf-8 and not a codepage.
    ఒకవేళ (!enc_utf8 && enc_codepage <= 0)
	తిరిగి 0;

    cp = encname2codepage(ptr);
    ఒకవేళ (cp == 0)
    {
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	ఒకవేళ (STRCMP(ptr, "utf-8") == 0)
	    cp = CP_UTF8;
	లేకపోతే
# endif
	    తిరిగి 0;
    }
    తిరిగి FIO_PUT_CP(cp) | FIO_CODEPAGE;
}
#అయితే_ముగింపు

#ఒకవేళ defined(MACOS_CONVERT)
/*
 * Check "ptr" for a Carbon supported encoding and return the FIO_ flags
 * needed for the internal conversion to/from utf-8 or latin1.
 */
    పూర్ణాంకం
get_mac_fio_flags(char_u *ptr)
{
    ఒకవేళ ((enc_utf8 || STRCMP(p_enc, "latin1") == 0)
				     && (enc_canon_props(ptr) & ENC_MACROMAN))
	తిరిగి FIO_MACROMAN;
    తిరిగి 0;
}
#అయితే_ముగింపు

/*
 * Check for a Unicode BOM (Byte Order Mark) at the start of p[size].
 * "size" must be at least 2.
 * Return the name of the encoding and set "*lenp" to the length.
 * Returns NULL when no BOM found.
 */
    స్థిర char_u *
check_for_bom(
    char_u	*p,
    దీర్ఘ	కొలత,
    పూర్ణాంకం		*lenp,
    పూర్ణాంకం		flags)
{
    అక్షరం	*name = ఖాళీ;
    పూర్ణాంకం		len = 2;

    ఒకవేళ (p[0] == 0xef && p[1] == 0xbb && కొలత >= 3 && p[2] == 0xbf
	    && (flags == FIO_ALL || flags == FIO_UTF8 || flags == 0))
    {
	name = "utf-8";		// EF BB BF
	len = 3;
    }
    లేకపోతే ఒకవేళ (p[0] == 0xff && p[1] == 0xfe)
    {
	ఒకవేళ (కొలత >= 4 && p[2] == 0 && p[3] == 0
	    && (flags == FIO_ALL || flags == (FIO_UCS4 | FIO_ENDIAN_L)))
	{
	    name = "ucs-4le";	// FF FE 00 00
	    len = 4;
	}
	లేకపోతే ఒకవేళ (flags == (FIO_UCS2 | FIO_ENDIAN_L))
	    name = "ucs-2le";	// FF FE
	లేకపోతే ఒకవేళ (flags == FIO_ALL || flags == (FIO_UTF16 | FIO_ENDIAN_L))
	    // utf-16le is preferred, it also works for ucs-2le text
	    name = "utf-16le";	// FF FE
    }
    లేకపోతే ఒకవేళ (p[0] == 0xfe && p[1] == 0xff
	    && (flags == FIO_ALL || flags == FIO_UCS2 || flags == FIO_UTF16))
    {
	// Default to utf-16, it works also for ucs-2 text.
	ఒకవేళ (flags == FIO_UCS2)
	    name = "ucs-2";	// FE FF
	లేకపోతే
	    name = "utf-16";	// FE FF
    }
    లేకపోతే ఒకవేళ (కొలత >= 4 && p[0] == 0 && p[1] == 0 && p[2] == 0xfe
	    && p[3] == 0xff && (flags == FIO_ALL || flags == FIO_UCS4))
    {
	name = "ucs-4";		// 00 00 FE FF
	len = 4;
    }

    *lenp = len;
    తిరిగి (char_u *)name;
}

/*
 * Try to find a shortname by comparing the fullname with the current
 * directory.
 * Returns "full_path" or pointer into "full_path" if shortened.
 */
    char_u *
shorten_fname1(char_u *నిండిన_మార్గం)
{
    char_u	*dirname;
    char_u	*p = నిండిన_మార్గం;

    dirname = alloc(MAXPATHL);
    ఒకవేళ (dirname == ఖాళీ)
	తిరిగి నిండిన_మార్గం;
    ఒకవేళ (mch_dirname(dirname, MAXPATHL) == OK)
    {
	p = shorten_fname(నిండిన_మార్గం, dirname);
	ఒకవేళ (p == ఖాళీ || *p == NUL)
	    p = నిండిన_మార్గం;
    }
    vim_free(dirname);
    తిరిగి p;
}

/*
 * Try to find a shortname by comparing the fullname with the current
 * directory.
 * Returns NULL if not shorter name possible, pointer into "full_path"
 * otherwise.
 */
    char_u *
shorten_fname(char_u *నిండిన_మార్గం, char_u *dir_name)
{
    పూర్ణాంకం		len;
    char_u	*p;

    ఒకవేళ (నిండిన_మార్గం == ఖాళీ)
	తిరిగి ఖాళీ;
    len = (పూర్ణాంకం)STRLEN(dir_name);
    ఒకవేళ (fnamencmp(dir_name, నిండిన_మార్గం, len) == 0)
    {
	p = నిండిన_మార్గం + len;
#ఒకవేళ defined(MSWIN)
	/*
	 * MS-Windows: when a file is in the root directory, dir_name will end
	 * in a slash, since C: by itself does not define a specific dir. In
	 * this case p may already be correct. <negri>
	 */
	ఒకవేళ (!((len > 2) && (*(p - 2) == ':')))
#అయితే_ముగింపు
	{
	    ఒకవేళ (vim_ispathsep(*p))
		చేయి
		    ++p;
		అయితే (vim_ispathsep_nocolon(*p));
#నిర్వచితం_కాకపోతే VMS   // the path separator is always part of the path
	    లేకపోతే
		p = ఖాళీ;
#అయితే_ముగింపు
	}
    }
#ఒకవేళ defined(MSWIN)
    /*
     * When using a file in the current drive, remove the drive name:
     * "A:\dir\file" -> "\dir\file".  This helps when moving a session file on
     * a floppy from "A:\dir" to "B:\dir".
     */
    లేకపోతే ఒకవేళ (len > 3
	    && TOUPPER_LOC(నిండిన_మార్గం[0]) == TOUPPER_LOC(dir_name[0])
	    && నిండిన_మార్గం[1] == ':'
	    && vim_ispathsep(నిండిన_మార్గం[2]))
	p = నిండిన_మార్గం + 2;
#అయితే_ముగింపు
    లేకపోతే
	p = ఖాళీ;
    తిరిగి p;
}

/*
 * Shorten filename of a buffer.
 * When "force" is TRUE: Use full path from now on for files currently being
 * edited, both for file name and swap file name.  Try to shorten the file
 * names a bit, if safe to do so.
 * When "force" is FALSE: Only try to shorten absolute file names.
 * For buffers that have buftype "nofile" or "scratch": never change the file
 * name.
 */
    శూన్య
shorten_buf_fname(buf_T *buf, char_u *dirname, పూర్ణాంకం force)
{
    char_u	*p;

    ఒకవేళ (buf->b_fname != ఖాళీ
	    && !bt_nofilename(buf)
	    && !మార్గం_with_url(buf->b_fname)
	    && (force
		|| buf->b_sfname == ఖాళీ
		|| mch_is_నిండిన_Name(buf->b_sfname)))
    {
	ఒకవేళ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	p = shorten_fname(buf->b_ffname, dirname);
	ఒకవేళ (p != ఖాళీ)
	{
	    buf->b_sfname = vim_strsave(p);
	    buf->b_fname = buf->b_sfname;
	}
	ఒకవేళ (p == ఖాళీ || buf->b_fname == ఖాళీ)
	    buf->b_fname = buf->b_ffname;
    }
}

/*
 * Shorten filenames for all buffers.
 */
    శూన్య
shorten_fnames(పూర్ణాంకం force)
{
    char_u	dirname[MAXPATHL];
    buf_T	*buf;

    mch_dirname(dirname, MAXPATHL);
    FOR_ALL_BUFFERS(buf)
    {
	shorten_buf_fname(buf, dirname, force);

	// Always make the swap file name a full path, a "nofile" buffer may
	// also have a swap file.
	mf_fullname(buf->b_ml.ml_mfp);
    }
    స్థితి_redraw_all();
    redraw_tabline = TRUE;
#ఒకవేళ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    popup_update_preview_title();
#అయితే_ముగింపు
}

#ఒకవేళ (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
/*
 * Shorten all filenames in "fnames[count]" by current directory.
 */
    శూన్య
shorten_filenames(char_u **fnames, పూర్ణాంకం లెక్క)
{
    పూర్ణాంకం		i;
    char_u	dirname[MAXPATHL];
    char_u	*p;

    ఒకవేళ (fnames == ఖాళీ || లెక్క < 1)
	తిరిగి;
    mch_dirname(dirname, పరిమాణం(dirname));
    కొరకు (i = 0; i < లెక్క; ++i)
    {
	ఒకవేళ ((p = shorten_fname(fnames[i], dirname)) != ఖాళీ)
	{
	    // shorten_fname() returns pointer in given "fnames[i]".  If free
	    // "fnames[i]" first, "p" becomes invalid.  So we need to copy
	    // "p" first then free fnames[i].
	    p = vim_strsave(p);
	    vim_free(fnames[i]);
	    fnames[i] = p;
	}
    }
}
#అయితే_ముగింపు

/*
 * Add extension to file name - change path/fo.o.h to path/fo.o.h.ext or
 * fo_o_h.ext for MSDOS or when shortname option set.
 *
 * Assumed that fname is a valid name found in the filesystem we assure that
 * the return value is a different name and ends in 'ext'.
 * "ext" MUST be at most 4 characters long if it starts with a dot, 3
 * characters otherwise.
 * Space for the returned name is allocated, must be freed later.
 * Returns NULL when out of memory.
 */
    char_u *
modname(
    char_u *fname,
    char_u *ext,
    పూర్ణాంకం	    prepend_dot)	// may prepend a '.' to file name
{
    తిరిగి buf_modname((curbuf->b_p_sn || curbuf->b_shortname),
						     fname, ext, prepend_dot);
}

    char_u *
buf_modname(
    పూర్ణాంకం	    shortname,		// use 8.3 file name
    char_u  *fname,
    char_u  *ext,
    పూర్ణాంకం	    prepend_dot)	// may prepend a '.' to file name
{
    char_u	*retval;
    char_u	*s;
    char_u	*e;
    char_u	*ptr;
    size_t	ptrlen;
    పూర్ణాంకం		fnamelen, extlen;

    extlen = (పూర్ణాంకం)STRLEN(ext);

    /*
     * If there is no file name we must get the name of the current directory
     * (we need the full path in case :cd is used).
     */
    ఒకవేళ (fname == ఖాళీ || *fname == NUL)
    {
	retval = alloc(MAXPATHL + extlen + 3);
	ఒకవేళ (retval == ఖాళీ)
	    తిరిగి ఖాళీ;
	ఒకవేళ (mch_dirname(retval, MAXPATHL) == FAIL ||
				     (fnamelen = (పూర్ణాంకం)STRLEN(retval)) == 0)
	{
	    vim_free(retval);
	    తిరిగి ఖాళీ;
	}
	ఒకవేళ (!after_pathsep(retval, retval + fnamelen))
	{
	    retval[fnamelen++] = PATHSEP;
	    retval[fnamelen] = NUL;
	}
	prepend_dot = FALSE;	    // nothing to prepend a dot to
    }
    లేకపోతే
    {
	fnamelen = (పూర్ణాంకం)STRLEN(fname);
	retval = alloc(fnamelen + extlen + 3);
	ఒకవేళ (retval == ఖాళీ)
	    తిరిగి ఖాళీ;
	STRCPY(retval, fname);
#నిర్వచితమైతే VMS
	vms_remove_సంస్కరణ(retval); // we do not need versions here
#అయితే_ముగింపు
    }

    /*
     * search backwards until we hit a '/', '\' or ':' replacing all '.'
     * by '_' for MSDOS or when shortname option set and ext starts with a dot.
     * Then truncate what is after the '/', '\' or ':' to 8 characters for
     * MSDOS and 26 characters for AMIGA, a lot more for UNIX.
     */
    కొరకు (ptr = retval + fnamelen; ptr > retval; MB_PTR_వెనుక(retval, ptr))
    {
	ఒకవేళ (*ext == '.' && shortname)
	    ఒకవేళ (*ptr == '.')	// replace '.' by '_'
		*ptr = '_';
	ఒకవేళ (vim_ispathsep(*ptr))
	{
	    ++ptr;
	    ఆపు;
	}
    }

    // the file name has at most BASENAMELEN characters.
    ptrlen = (size_t)(fnamelen - (ptr - retval));
    ఒకవేళ (ptrlen > (గుర్తులేని)BASENAMELEN)
    {
	ptrlen = BASENAMELEN;
	ptr[ptrlen] = NUL;
    }

    s = ptr + ptrlen;

    /*
     * For 8.3 file names we may have to reduce the length.
     */
    ఒకవేళ (shortname)
    {
	/*
	 * If there is no file name, or the file name ends in '/', and the
	 * extension starts with '.', put a '_' before the dot, because just
	 * ".ext" is invalid.
	 */
	ఒకవేళ (fname == ఖాళీ || *fname == NUL
				   || vim_ispathsep(fname[STRLEN(fname) - 1]))
	{
	    ఒకవేళ (*ext == '.')
		*s++ = '_';
	}
	/*
	 * If the extension starts with '.', truncate the base name at 8
	 * characters
	 */
	లేకపోతే ఒకవేళ (*ext == '.')
	{
	    ఒకవేళ ((size_t)(s - ptr) > (size_t)8)
	    {
		s = ptr + 8;
		*s = '\0';
	    }
	}
	/*
	 * If the extension doesn't start with '.', and the file name
	 * doesn't have an extension yet, append a '.'
	 */
	లేకపోతే ఒకవేళ ((e = vim_strchr(ptr, '.')) == ఖాళీ)
	    *s++ = '.';
	/*
	 * If the extension doesn't start with '.', and there already is an
	 * extension, it may need to be truncated
	 */
	లేకపోతే ఒకవేళ ((పూర్ణాంకం)(ptrlen - (e - retval)) + extlen > 4)
	    s = e + 4 - extlen;
    }
#నిర్వచితమైతే MSWIN
    /*
     * If there is no file name, and the extension starts with '.', put a
     * '_' before the dot, because just ".ext" may be invalid if it's on a
     * FAT partition, and on HPFS it doesn't matter.
     */
    లేకపోతే ఒకవేళ ((fname == ఖాళీ || *fname == NUL) && *ext == '.')
	*s++ = '_';
#అయితే_ముగింపు

    /*
     * Append the extension.
     * ext can start with '.' and cannot exceed 3 more characters.
     */
    STRCPY(s, ext);
    /*
     * Prepend the dot.
     */
    ఒకవేళ (prepend_dot && !shortname && *(e = gettail(retval)) != '.')
    {
	mch_memmove(e + 1, e, (size_t)(((fnamelen + extlen) - (e - retval)) + 1));	// +1 for NUL
	*e = '.';
    }

    /*
     * Check that, after appending the extension, the file name is really
     * different.
     */
    ఒకవేళ (fname != ఖాళీ && STRCMP(fname, retval) == 0)
    {
	// we search for a character that can be replaced by '_'
	అయితే (--s >= ptr)
	{
	    ఒకవేళ (*s != '_')
	    {
		*s = '_';
		ఆపు;
	    }
	}
	ఒకవేళ (s < ptr)	// fname was "________.<ext>", how tricky!
	    *ptr = 'v';
    }
    తిరిగి retval;
}

/*
 * Like fgets(), but if the file line is too long, it is truncated and the
 * rest of the line is thrown away.  Returns TRUE for end-of-file.
 * If the line is truncated then buf[size - 2] will not be NUL.
 */
    పూర్ణాంకం
vim_fgets(char_u *buf, పూర్ణాంకం కొలత, FILE *fp)
{
    అక్షరం	*eof;
#నిర్వచించు FGETS_కొలత 200
    అక్షరం	tbuf[FGETS_కొలత];

    // safety check
    ఒకవేళ (కొలత < 2)
    {
	ఒకవేళ (కొలత == 1)
	    buf[0] = NUL;
	తిరిగి TRUE;
    }

    buf[కొలత - 2] = NUL;
    eof = fgets((అక్షరం *)buf, కొలత, fp);
    ఒకవేళ (buf[కొలత - 2] != NUL && buf[కొలత - 2] != '\n')
    {
	buf[కొలత - 1] = NUL;	    // Truncate the line

	// Now throw away the rest of the line:
	చేయి
	{
	    tbuf[FGETS_కొలత - 2] = NUL;
	    vim_ignoredp = fgets((అక్షరం *)tbuf, FGETS_కొలత, fp);
	} అయితే (tbuf[FGETS_కొలత - 2] != NUL && tbuf[FGETS_కొలత - 2] != '\n');
    }
    తిరిగి (eof == ఖాళీ);
}

/*
 * rename() only works if both files are on the same file system, this
 * function will (attempts to?) copy the file across if rename fails -- webb
 * Return -1 for failure, 0 for success.
 */
    పూర్ణాంకం
vim_rename(char_u *from, char_u *గమ్యం)
{
    పూర్ణాంకం		n;
    పూర్ణాంకం		ret;
#నిర్వచితమైతే AMIGA
    BPTR	flock;
#అయితే_ముగింపు
    stat_T	st;
    పూర్ణాంకం		use_tmp_file = FALSE;

    /*
     * When the names are identical, there is nothing to do.  When they refer
     * to the same file (ignoring case and slash/backslash differences) but
     * the file name differs we need to go through a temp file.
     */
    ఒకవేళ (fnamecmp(from, గమ్యం) == 0)
    {
	ఒకవేళ (p_fic && STRCMP(gettail(from), gettail(గమ్యం)) != 0)
	    use_tmp_file = TRUE;
	లేకపోతే
	    తిరిగి 0;
    }

    /*
     * Fail if the "from" file doesn't exist.  Avoids that "to" is deleted.
     */
    ఒకవేళ (mch_stat((అక్షరం *)from, &st) < 0)
	తిరిగి -1;

#నిర్వచితమైతే UNIX
    {
	stat_T	st_గమ్యం;

	// It's possible for the source and destination to be the same file.
	// This happens when "from" and "to" differ in case and are on a FAT32
	// filesystem.  In that case go through a temp file name.
	ఒకవేళ (mch_stat((అక్షరం *)గమ్యం, &st_గమ్యం) >= 0
		&& st.st_dev == st_గమ్యం.st_dev
		&& st.st_ino == st_గమ్యం.st_ino)
	    use_tmp_file = TRUE;
    }
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
    {
	BY_HANDLE_FILE_INFORMATION info1, info2;

	// It's possible for the source and destination to be the same file.
	// In that case go through a temp file name.  This makes rename("foo",
	// "./foo") a no-op (in a complicated way).
	ఒకవేళ (win32_fileinfo(from, &info1) == FILEINFO_OK
		&& win32_fileinfo(గమ్యం, &info2) == FILEINFO_OK
		&& info1.dw_Volume_Serial_సంఖ్య == info2.dw_Volume_Serial_సంఖ్య
		&& info1.n_File_సూచీ_ఎగువ == info2.n_File_సూచీ_ఎగువ
		&& info1.n_File_సూచీ_దిగువ == info2.n_File_సూచీ_దిగువ)
	    use_tmp_file = TRUE;
    }
#అయితే_ముగింపు

    ఒకవేళ (use_tmp_file)
    {
	అక్షరం	tempname[MAXPATHL + 1];

	/*
	 * Find a name that doesn't exist and is in the same directory.
	 * Rename "from" to "tempname" and then rename "tempname" to "to".
	 */
	ఒకవేళ (STRLEN(from) >= MAXPATHL - 5)
	    తిరిగి -1;
	STRCPY(tempname, from);
	కొరకు (n = 123; n < 99999; ++n)
	{
	    sprintf((అక్షరం *)gettail((char_u *)tempname), "%d", n);
	    ఒకవేళ (mch_stat(tempname, &st) < 0)
	    {
		ఒకవేళ (mch_rename((అక్షరం *)from, tempname) == 0)
		{
		    ఒకవేళ (mch_rename(tempname, (అక్షరం *)గమ్యం) == 0)
			తిరిగి 0;
		    // Strange, the second step failed.  Try moving the
		    // file back and return failure.
		    (శూన్య)mch_rename(tempname, (అక్షరం *)from);
		    తిరిగి -1;
		}
		// If it fails for one temp name it will most likely fail
		// for any temp name, give up.
		తిరిగి -1;
	    }
	}
	తిరిగి -1;
    }

    /*
     * Delete the "to" file, this is required on some systems to make the
     * mch_rename() work, on other systems it makes sure that we don't have
     * two files when the mch_rename() fails.
     */

#నిర్వచితమైతే AMIGA
    /*
     * With MSDOS-compatible filesystems (crossdos, messydos) it is possible
     * that the name of the "to" file is the same as the "from" file, even
     * though the names are different. To avoid the chance of accidentally
     * deleting the "from" file (horror!) we lock it during the remove.
     *
     * When used for making a backup before writing the file: This should not
     * happen with ":w", because startscript() should detect this problem and
     * set buf->b_shortname, causing modname() to return a correct ".bak" file
     * name.  This problem does exist with ":w filename", but then the
     * original file will be somewhere else so the backup isn't really
     * important. If autoscripting is off the rename may fail.
     */
    flock = తాళం_2((UBYTE *)from, (దీర్ఘ)VIM_ACCESS_READ);
#అయితే_ముగింపు
    mch_remove(గమ్యం);
#నిర్వచితమైతే AMIGA
    ఒకవేళ (flock)
	Un_తాళం(flock);
#అయితే_ముగింపు

    /*
     * First try a normal rename, return if it works.
     */
    ఒకవేళ (mch_rename((అక్షరం *)from, (అక్షరం *)గమ్యం) == 0)
	తిరిగి 0;

    /*
     * Rename() failed, try copying the file.
     */
    ret = vim_copyfile(from, గమ్యం);
    ఒకవేళ (ret != OK)
	తిరిగి -1;

    /*
     * Remove copied original file
     */
    ఒకవేళ (mch_stat((అక్షరం *)from, &st) >= 0)
	mch_remove(from);

    తిరిగి 0;
}


/*
 * Create the new file with same permissions as the original.
 * Return FAIL for failure, OK for success.
 */
    పూర్ణాంకం
vim_copyfile(char_u *from, char_u *గమ్యం)
{
    పూర్ణాంకం		fd_లోపల;
    పూర్ణాంకం		fd_బహిర్గతం;
    పూర్ణాంకం		n;
    అక్షరం	*errmsg = ఖాళీ;
    అక్షరం	*బఫరు;
    దీర్ఘ	perm;
#నిర్వచితమైతే HAVE_ACL
    vim_acl_T	acl;		// ACL from original file
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_READLINK
    పూర్ణాంకం		ret;
    పూర్ణాంకం		len;
    stat_T	st;
    అక్షరం	linkbuf[MAXPATHL + 1];

    ret = mch_lstat((అక్షరం *)from, &st);
    ఒకవేళ (ret >= 0 && S_ISLNK(st.st_బహుళకం))
    {
	ret = -1;

	len = readlink((అక్షరం *)from, linkbuf, MAXPATHL);
	ఒకవేళ (len > 0)
	{
	    linkbuf[len] = NUL;

	    // Create link
	    ret = symlink(linkbuf, (అక్షరం *)గమ్యం);
	}

	తిరిగి ret == 0 ? OK : FAIL;
    }
#అయితే_ముగింపు

    perm = mch_getperm(from);
#నిర్వచితమైతే HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    acl = mch_get_acl(from);
#అయితే_ముగింపు
    fd_లోపల = mch_open((అక్షరం *)from, O_RDONLY|O_EXTRA, 0);
    ఒకవేళ (fd_లోపల == -1)
    {
#నిర్వచితమైతే HAVE_ACL
	mch_free_acl(acl);
#అయితే_ముగింపు
	తిరిగి FAIL;
    }

    // Create the new file with same permissions as the original.
    fd_బహిర్గతం = mch_open((అక్షరం *)గమ్యం,
		       O_CREAT|O_EXCL|O_WRONLY|O_EXTRA|O_NOFOLLOW, (పూర్ణాంకం)perm);
    ఒకవేళ (fd_బహిర్గతం == -1)
    {
	close(fd_లోపల);
#నిర్వచితమైతే HAVE_ACL
	mch_free_acl(acl);
#అయితే_ముగింపు
	తిరిగి FAIL;
    }

    బఫరు = alloc(WRITEBUFSIZE);
    ఒకవేళ (బఫరు == ఖాళీ)
    {
	close(fd_బహిర్గతం);
	close(fd_లోపల);
#నిర్వచితమైతే HAVE_ACL
	mch_free_acl(acl);
#అయితే_ముగింపు
	తిరిగి FAIL;
    }

    అయితే ((n = read_eintr(fd_లోపల, బఫరు, WRITEBUFSIZE)) > 0)
	ఒకవేళ (write_eintr(fd_బహిర్గతం, బఫరు, n) != n)
	{
	    errmsg = _(e_error_writing_గమ్యం_str);
	    ఆపు;
	}

    vim_free(బఫరు);
    close(fd_లోపల);
    ఒకవేళ (close(fd_బహిర్గతం) < 0)
	errmsg = _(e_error_closing_str);
    ఒకవేళ (n < 0)
    {
	errmsg = _(e_error_reading_str);
	గమ్యం = from;
    }
#నిర్వచితం_కాకపోతే UNIX	    // for Unix mch_open() already set the permission
    mch_setperm(గమ్యం, perm);
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ACL
    mch_సమితి_acl(గమ్యం, acl);
    mch_free_acl(acl);
#అయితే_ముగింపు
#ఒకవేళ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    mch_copy_sec(from, గమ్యం);
#అయితే_ముగింపు
    ఒకవేళ (errmsg != ఖాళీ)
    {
	semsg(errmsg, గమ్యం);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

స్థిర పూర్ణాంకం already_warned = FALSE;

/*
 * Check if any not hidden buffer has been changed.
 * Postpone the check if there are characters in the stuff buffer, a global
 * command is being executed, a mapping is being executed or an autocommand is
 * busy.
 * Returns TRUE if some message was written (screen should be redrawn and
 * cursor positioned).
 */
    పూర్ణాంకం
check_timestamps(
    పూర్ణాంకం		focus)		// called for GUI focus event
{
    buf_T	*buf;
    పూర్ణాంకం		didit = 0;
    పూర్ణాంకం		n;

    // Don't check timestamps while system() or another low-level function may
    // cause us to lose and gain focus.
    ఒకవేళ (no_check_timestamps > 0)
	తిరిగి FALSE;

    // Avoid doing a check twice.  The OK/Reload dialog can cause a focus
    // event and we would keep on checking if the file is steadily growing.
    // Do check again after typing something.
    ఒకవేళ (focus && did_check_timestamps)
    {
	need_check_timestamps = TRUE;
	తిరిగి FALSE;
    }

    ఒకవేళ (!stuff_ఖాళీలు() || global_busy || !typebuf_typed()
			|| autocmd_busy || curbuf_తాళం > 0 || allbuf_తాళం > 0)
	need_check_timestamps = TRUE;		// check later
    లేకపోతే
    {
	++no_wait_return;
	did_check_timestamps = TRUE;
	already_warned = FALSE;
	FOR_ALL_BUFFERS(buf)
	{
	    // Only check buffers in a window.
	    ఒకవేళ (buf->b_nwindows > 0)
	    {
		bufref_T bufref;

		సమితి_bufref(&bufref, buf);
		n = buf_check_timestamp(buf, focus);
		ఒకవేళ (didit < n)
		    didit = n;
		ఒకవేళ (n > 0 && !bufref_valid(&bufref))
		{
		    // Autocommands have removed the buffer, start at the
		    // first one again.
		    buf = firstbuf;
		    కొనసాగు;
		}
	    }
	}
	--no_wait_return;
	need_check_timestamps = FALSE;
	ఒకవేళ (need_wait_return && didit == 2)
	{
	    // make sure msg isn't overwritten
	    సందేశం_puts("\n");
	    బహిర్గతం_flush();
	}
    }
    తిరిగి didit;
}

/*
 * Move all the lines from buffer "frombuf" to buffer "tobuf".
 * Return OK or FAIL.  When FAIL "tobuf" is incomplete and/or "frombuf" is not
 * empty.
 */
    స్థిర పూర్ణాంకం
move_పంక్తులు(buf_T *frombuf, buf_T *tobuf)
{
    buf_T	*tbuf = curbuf;
    పూర్ణాంకం		retval = OK;
    linenr_T	lnum;
    char_u	*p;

    // Copy the lines in "frombuf" to "tobuf".
    curbuf = tobuf;
    కొరకు (lnum = 1; lnum <= frombuf->b_ml.ml_పంక్తి_లెక్క; ++lnum)
    {
	p = vim_strnsave(ml_get_buf(frombuf, lnum, FALSE), ml_get_buf_len(frombuf, lnum));
	ఒకవేళ (p == ఖాళీ || ml_append(lnum - 1, p, 0, FALSE) == FAIL)
	{
	    vim_free(p);
	    retval = FAIL;
	    ఆపు;
	}
	vim_free(p);
    }

    // Delete all the lines in "frombuf".
    ఒకవేళ (retval != FAIL)
    {
	curbuf = frombuf;
	కొరకు (lnum = curbuf->b_ml.ml_పంక్తి_లెక్క; lnum > 0; --lnum)
	    ఒకవేళ (ml_delete(lnum) == FAIL)
	    {
		// Oops!  We could try putting back the saved lines, but that
		// might fail again...
		retval = FAIL;
		ఆపు;
	    }
    }

    curbuf = tbuf;
    తిరిగి retval;
}

/*
 * Check if buffer "buf" has been changed.
 * Also check if the file for a new buffer unexpectedly appeared.
 * return 1 if a changed buffer was found.
 * return 2 if a message has been displayed.
 * return 0 otherwise.
 */
    పూర్ణాంకం
buf_check_timestamp(
    buf_T	*buf,
    పూర్ణాంకం		focus వాడని)	// called for GUI focus event
{
    stat_T	st;
    పూర్ణాంకం		stat_res;
    పూర్ణాంకం		retval = 0;
    అక్షరం	*mesg = ఖాళీ;
    అక్షరం	*mesg2 = "";
    పూర్ణాంకం		helpmesg = FALSE;
    గణన {
	RELOAD_NONE,
	RELOAD_సాధారణ,
	RELOAD_DETECT
    }		reload = RELOAD_NONE;
#ఒకవేళ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
    పూర్ణాంకం		can_reload = FALSE;
#అయితే_ముగింపు
    off_T	orig_కొలత = buf->b_orig_కొలత;
    పూర్ణాంకం		orig_బహుళకం = buf->b_orig_బహుళకం;
#నిర్వచితమైతే FEAT_GUI
    పూర్ణాంకం		save_mouse_correct = need_mouse_correct;
#అయితే_ముగింపు
    స్థిర పూర్ణాంకం	busy = FALSE;
    పూర్ణాంకం		n;
    bufref_T	bufref;

    సమితి_bufref(&bufref, buf);

    // If there is no file name, the buffer is not loaded, 'buftype' is
    // set, we are in the middle of a save or being called recursively: ignore
    // this buffer.
    ఒకవేళ (buf->b_ffname == ఖాళీ
	    || buf->b_ml.ml_mfp == ఖాళీ
	    || !bt_సాధారణ(buf)
	    || buf->b_saving
	    || busy
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	    || is_Netbeans_బఫరు(buf)
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
	    || buf->b_పదం != ఖాళీ
#అయితే_ముగింపు
	    )
	తిరిగి 0;

    ఒకవేళ (       !(buf->b_flags & BF_NOTEDITED)
	    && buf->b_mtime != 0
	    && ((stat_res = mch_stat((అక్షరం *)buf->b_ffname, &st)) < 0
		|| సమయం_differs(&st, buf->b_mtime, buf->b_mtime_ns)
		|| st.st_size != buf->b_orig_కొలత
#నిర్వచితమైతే HAVE_ST_బహుళకం
		|| (పూర్ణాంకం)st.st_బహుళకం != buf->b_orig_బహుళకం
#లేకపోతే
		|| mch_getperm(buf->b_ffname) != buf->b_orig_బహుళకం
#అయితే_ముగింపు
		))
    {
	దీర్ఘ prev_b_mtime = buf->b_mtime;

	retval = 1;

	// set b_mtime to stop further warnings (e.g., when executing
	// FileChangedShell autocmd)
	ఒకవేళ (stat_res < 0)
	{
	    // Check the file again later to see if it re-appears.
	    buf->b_mtime = -1;
	    buf->b_orig_కొలత = 0;
	    buf->b_orig_బహుళకం = 0;
	}
	లేకపోతే
	    buf_store_సమయం(buf, &st, buf->b_ffname);

	// Don't do anything for a directory.  Might contain the file
	// explorer.
	ఒకవేళ (mch_isdir(buf->b_fname))
	    ;

	/*
	 * If 'autoread' is set, the buffer has no changes and the file still
	 * exists, reload the buffer.  Use the buffer-local option value if it
	 * was set, the global option value otherwise.
	 */
	లేకపోతే ఒకవేళ ((buf->b_p_ar >= 0 ? buf->b_p_ar : p_ar)
				       && !buf_Is_మారింది(buf) && stat_res >= 0)
	    reload = RELOAD_సాధారణ;
	లేకపోతే
	{
	    అక్షరం    *reason;
#నిర్వచితమైతే FEAT_EVAL
	    size_t  reasonlen;
#అయితే_ముగింపు

	    ఒకవేళ (stat_res < 0)
	    {
		reason = "deleted";
#నిర్వచితమైతే FEAT_EVAL
		reasonlen = STRLEN_LITERAL("deleted");
#అయితే_ముగింపు
	    }
	    లేకపోతే ఒకవేళ (buf_Is_మారింది(buf))
	    {
		reason = "conflict";
#నిర్వచితమైతే FEAT_EVAL
		reasonlen = STRLEN_LITERAL("conflict");
#అయితే_ముగింపు
	    }
	    /*
	     * Check if the file contents really changed to avoid giving a
	     * warning when only the timestamp was set (e.g., checked out of
	     * CVS).  Always warn when the buffer was changed.
	     */
	    లేకపోతే ఒకవేళ (orig_కొలత != buf->b_orig_కొలత || buf_contents_మారింది(buf))
	    {
		reason = "changed";
#నిర్వచితమైతే FEAT_EVAL
		reasonlen = STRLEN_LITERAL("changed");
#అయితే_ముగింపు
	    }
	    లేకపోతే ఒకవేళ (orig_బహుళకం != buf->b_orig_బహుళకం)
	    {
		reason = "mode";
#నిర్వచితమైతే FEAT_EVAL
		reasonlen = STRLEN_LITERAL("mode");
#అయితే_ముగింపు
	    }
	    లేకపోతే
	    {
		reason = "time";
#నిర్వచితమైతే FEAT_EVAL
		reasonlen = STRLEN_LITERAL("time");
#అయితే_ముగింపు
	    }

	    /*
	     * Only give the warning if there are no FileChangedShell
	     * autocommands.
	     * Avoid being called recursively by setting "busy".
	     */
	    busy = TRUE;
#నిర్వచితమైతే FEAT_EVAL
	    సమితి_vim_var_string(VV_FCS_REASON, (char_u *)reason, (పూర్ణాంకం)reasonlen);
	    సమితి_vim_var_string(VV_FCS_CHOICE, (char_u *)"", 0);
#అయితే_ముగింపు
	    ++allbuf_తాళం;
	    n = apply_autocmds(EVENT_FILECHANGEDSHELL,
				      buf->b_fname, buf->b_fname, FALSE, buf);
	    --allbuf_తాళం;
	    busy = FALSE;
	    ఒకవేళ (n)
	    {
		ఒకవేళ (!bufref_valid(&bufref))
		    emsg(_(e_filechangedshell_autocommand_deleted_బఫరు));
#నిర్వచితమైతే FEAT_EVAL
		char_u	*s = get_vim_var_str(VV_FCS_CHOICE);
		ఒకవేళ (STRCMP(s, "reload") == 0 && *reason != 'd')
		    reload = RELOAD_సాధారణ;
		లేకపోతే ఒకవేళ (STRCMP(s, "edit") == 0)
		    reload = RELOAD_DETECT;
		లేకపోతే ఒకవేళ (STRCMP(s, "ask") == 0)
		    n = FALSE;
		లేకపోతే
#అయితే_ముగింపు
		    తిరిగి 2;
	    }
	    ఒకవేళ (!n)
	    {
		ఒకవేళ (*reason == 'd')
		{
		    // Only give the message once.
		    ఒకవేళ (prev_b_mtime != -1)
			mesg = _(e_file_str_no_longer_అందుబాటు);
		}
		లేకపోతే
		{
		    helpmesg = TRUE;
#ఒకవేళ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		    can_reload = TRUE;
#అయితే_ముగింపు
		    ఒకవేళ (reason[2] == 'n')
		    {
			mesg = _("W12: Warning: File \"%s\" has changed and the buffer was changed in Vim as well");
			mesg2 = _("See \":help W12\" for more info.");
		    }
		    లేకపోతే ఒకవేళ (reason[1] == 'h')
		    {
			mesg = _("W11: Warning: File \"%s\" has changed since editing started");
			mesg2 = _("See \":help W11\" for more info.");
		    }
		    లేకపోతే ఒకవేళ (*reason == 'm')
		    {
			mesg = _("W16: Warning: Mode of file \"%s\" has changed since editing started");
			mesg2 = _("See \":help W16\" for more info.");
		    }
		    లేకపోతే
		    {
			// Only timestamp changed, store it to avoid a warning
			// in check_mtime() later.
			buf->b_mtime_read = buf->b_mtime;
			buf->b_mtime_read_ns = buf->b_mtime_ns;
		    }
		}
	    }
	}

    }
    లేకపోతే ఒకవేళ ((buf->b_flags & BF_NEW) && !(buf->b_flags & BF_NEW_W)
						&& vim_fexists(buf->b_ffname))
    {
	retval = 1;
	mesg = _("W13: Warning: File \"%s\" has been created after editing started");
	buf->b_flags |= BF_NEW_W;
#ఒకవేళ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
	can_reload = TRUE;
#అయితే_ముగింపు
    }

    ఒకవేళ (mesg != ఖాళీ)
    {
	char_u *మార్గం;

	మార్గం = home_replace_save(buf, buf->b_fname);
	ఒకవేళ (మార్గం != ఖాళీ)
	{
	    size_t  tbufsize;
	    అక్షరం    *tbuf;

	    ఒకవేళ (!helpmesg)
		mesg2 = "";
	    tbufsize = STRLEN(mesg) + STRLEN(మార్గం) + 2 + STRLEN(mesg2) + 1; // +2 for either '\n' or "; "
									    // and +1 for NUL
	    tbuf = alloc(tbufsize);
	    ఒకవేళ (tbuf != ఖాళీ)
	    {
		పూర్ణాంకం tbuflen;

		tbuflen = vim_snprintf(tbuf, tbufsize, mesg, మార్గం);
#నిర్వచితమైతే FEAT_EVAL
		// Set warningmsg here, before the unimportant and output-specific
		// mesg2 has been appended.
		సమితి_vim_var_string(VV_WARNINGMSG, (char_u *)tbuf, tbuflen);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		ఒకవేళ (can_reload)
		{
		    ఒకవేళ (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "\n%s", mesg2);
		    ఎంపిక (do_dialog(VIM_WARNING, (char_u *)_("Warning"),
			    (char_u *)tbuf,
			    (char_u *)_("&OK\n&Load File\nLoad File &and Options"),
			    1, ఖాళీ, TRUE))
		    {
			స్థితి 2:
			    reload = RELOAD_సాధారణ;
			    ఆపు;
			స్థితి 3:
			    reload = RELOAD_DETECT;
			    ఆపు;
		    }
		}
		లేకపోతే
#అయితే_ముగింపు
		ఒకవేళ (State > బహుళకం_సాధారణ_BUSY || (State & బహుళకం_CMDLINE)
								 || already_warned)
		{
		    ఒకవేళ (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "; %s", mesg2);
		    emsg(tbuf);
		    retval = 2;
		}
		లేకపోతే
		{
		    ఒకవేళ (!autocmd_busy)
		    {
			సందేశం_ప్రారంభం();
			సందేశం_puts_attr(tbuf, HL_ATTR(HLF_E) + సందేశం_HIST);
			ఒకవేళ (*mesg2 != NUL)
			    సందేశం_puts_attr(mesg2, HL_ATTR(HLF_W) + సందేశం_HIST);
			సందేశం_clr_eos();
			(శూన్య)సందేశం_చివర();
			ఒకవేళ (emsg_silent == 0 && !లోపల_assert_fails)
			{
			    బహిర్గతం_flush();
#నిర్వచితమైతే FEAT_GUI
			    ఒకవేళ (!focus)
#అయితే_ముగింపు
				// give the user some time to think about it
				ui_delay(1004L, TRUE);

			    // don't redraw and erase the message
			    redraw_cmdline = FALSE;
			}
		    }
		    already_warned = TRUE;
		}
	    }

	    vim_free(tbuf);
	    vim_free(మార్గం);
	}
    }

    ఒకవేళ (reload != RELOAD_NONE)
    {
	// Reload the buffer.
	buf_reload(buf, orig_బహుళకం, reload == RELOAD_DETECT);
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
	ఒకవేళ (bufref_valid(&bufref) && buf->b_p_udf && buf->b_ffname != ఖాళీ)
	{
	    char_u	    hash[UNDO_HASH_కొలత];
	    buf_T	    *save_curbuf = curbuf;

	    // Any existing undo file is unusable, write it now.
	    curbuf = buf;
	    u_compute_hash(hash);
	    u_write_undo(ఖాళీ, FALSE, buf, hash);
	    curbuf = save_curbuf;
	}
#అయితే_ముగింపు
    }

    // Trigger FileChangedShell when the file was changed in any way.
    ఒకవేళ (bufref_valid(&bufref) && retval != 0)
	(శూన్య)apply_autocmds(EVENT_FILECHANGEDSHELLPOST,
				      buf->b_fname, buf->b_fname, FALSE, buf);
#నిర్వచితమైతే FEAT_GUI
    // restore this in case an autocommand has set it; it would break
    // 'mousefocus'
    need_mouse_correct = save_mouse_correct;
#అయితే_ముగింపు

    తిరిగి retval;
}

/*
 * Reload a buffer that is already loaded.
 * Used when the file was changed outside of Vim.
 * "orig_mode" is buf->b_orig_mode before the need for reloading was detected.
 * buf->b_orig_mode may have been reset already.
 */
    శూన్య
buf_reload(buf_T *buf, పూర్ణాంకం orig_బహుళకం, పూర్ణాంకం reload_options)
{
    exarg_T	ea;
    pos_T	old_cursor;
    linenr_T	old_topline;
    పూర్ణాంకం		old_ro = buf->b_p_ro;
    buf_T	*savebuf;
    bufref_T	bufref;
    పూర్ణాంకం		saved = OK;
    aco_save_T	aco;
    పూర్ణాంకం		flags = READ_NEW;
    పూర్ణాంకం		prepped = OK;

    // Set curwin/curbuf for "buf" and save some things.
    aucmd_prepbuf(&aco, buf);
    ఒకవేళ (curbuf != buf)
    {
	// Failed to find a window for "buf", it is dangerous to continue,
	// better bail out.
	తిరిగి;
    }

    // Unless reload_options is set, we only want to read the text from the
    // file, not reset the syntax highlighting, clear marks, diff status, etc.
    // Force the fileformat and encoding to be the same.
    ఒకవేళ (reload_options)
	CLEAR_క్షేత్రం(ea);
    లేకపోతే
	prepped = prep_exarg(&ea, buf);

    ఒకవేళ (prepped == OK)
    {
	old_cursor = curwin->w_cursor;
	old_topline = curwin->w_topline;

	ఒకవేళ (p_ur < 0 || curbuf->b_ml.ml_పంక్తి_లెక్క <= p_ur)
	{
	    // Save all the text, so that the reload can be undone.
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    saved = u_savecommon(0, curbuf->b_ml.ml_పంక్తి_లెక్క + 1, 0, TRUE);
	    flags |= READ_KEEP_UNDO;
	}

	/*
	 * To behave like when a new file is edited (matters for
	 * BufReadPost autocommands) we first need to delete the current
	 * buffer contents.  But if reading the file fails we should keep
	 * the old contents.  Can't use memory only, the file might be
	 * too big.  Use a hidden buffer to move the buffer contents to.
	 */
	ఒకవేళ (BUFEMPTY() || saved == FAIL)
	    savebuf = ఖాళీ;
	లేకపోతే
	{
	    // Allocate a buffer without putting it in the buffer list.
	    savebuf = buflist_new(ఖాళీ, ఖాళీ, (linenr_T)1, BLN_DUMMY);
	    సమితి_bufref(&bufref, savebuf);
	    ఒకవేళ (savebuf != ఖాళీ && buf == curbuf)
	    {
		// Open the memline.
		curbuf = savebuf;
		curwin->w_బఫరు = savebuf;
		saved = ml_open(curbuf);
		curbuf = buf;
		curwin->w_బఫరు = buf;
	    }
	    ఒకవేళ (savebuf == ఖాళీ || saved == FAIL || buf != curbuf
				      || move_పంక్తులు(buf, savebuf) == FAIL)
	    {
		semsg(_(e_could_not_prepare_for_reloading_str), buf->b_fname);
		saved = FAIL;
	    }
	}

	ఒకవేళ (saved == OK)
	{
	    పూర్ణాంకం old_సందేశం_silent = సందేశం_silent;

	    curbuf->b_flags |= BF_CHECK_RO;	// check for RO again
	    curbuf->b_keep_filetype = true;	// don't detect 'filetype'

	    ఒకవేళ (shortmess(SHM_FILEINFO))
		సందేశం_silent = 1;

	    ఒకవేళ (readfile(buf->b_ffname, buf->b_fname, (linenr_T)0,
			(linenr_T)0,
			(linenr_T)MAXLNUM, &ea, flags) != OK)
	    {
#ఒకవేళ defined(FEAT_EVAL)
		ఒకవేళ (!aborting())
#అయితే_ముగింపు
		    semsg(_(e_could_not_reload_str), buf->b_fname);
		ఒకవేళ (savebuf != ఖాళీ && bufref_valid(&bufref) && buf == curbuf)
		{
		    // Put the text back from the save buffer.  First
		    // delete any lines that readfile() added.
		    అయితే (!BUFEMPTY())
			ఒకవేళ (ml_delete(buf->b_ml.ml_పంక్తి_లెక్క) == FAIL)
			    ఆపు;
		    (శూన్య)move_పంక్తులు(savebuf, buf);
		}
	    }
	    లేకపోతే ఒకవేళ (buf == curbuf)  // "buf" still valid
	    {
		// Mark the buffer as unmodified and free undo info.
		unchanged(buf, TRUE, TRUE);
		ఒకవేళ ((flags & READ_KEEP_UNDO) == 0)
		    u_clearallandblockfree(buf);
		లేకపోతే
		{
		    // Mark all undo states as changed.
		    u_unchanged(curbuf);
		}
	    }

	    సందేశం_silent = old_సందేశం_silent;
	}
	vim_free(ea.cmd);

	ఒకవేళ (savebuf != ఖాళీ && bufref_valid(&bufref))
	    wipe_బఫరు(savebuf, FALSE);

#నిర్వచితమైతే FEAT_DIFF
	// Invalidate diff info if necessary.
	diff_invalidate(curbuf);
#అయితే_ముగింపు

	// Restore the topline and cursor position and check it (lines may
	// have been removed).
	ఒకవేళ (old_topline > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    curwin->w_topline = curbuf->b_ml.ml_పంక్తి_లెక్క;
	లేకపోతే
	    curwin->w_topline = old_topline;
	curwin->w_cursor = old_cursor;
	check_cursor();
	update_topline();
	curbuf->b_keep_filetype = false;
#నిర్వచితమైతే FEAT_FOLDING
	{
	    win_T	*wp;
	    tabpage_T	*tp;

	    // Update folds unless they are defined manually.
	    FOR_ALL_TAB_WINDOWS(tp, wp)
		ఒకవేళ (wp->w_బఫరు == curwin->w_బఫరు
			&& !foldmethodIsManual(wp))
		    foldUpdateAll(wp);
	}
#అయితే_ముగింపు
	// If the mode didn't change and 'readonly' was set, keep the old
	// value; the user probably used the ":view" command.  But don't
	// reset it, might have had a read error.
	ఒకవేళ (orig_బహుళకం == curbuf->b_orig_బహుళకం)
	    curbuf->b_p_ro |= old_ro;

	// Modelines must override settings done by autocommands.
	do_modelines(0);
    }

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);
    // Careful: autocommands may have made "buf" invalid!
}

    శూన్య
buf_store_సమయం(buf_T *buf, stat_T *st, char_u *fname వాడని)
{
    buf->b_mtime = (దీర్ఘ)st->st_mtime;
#నిర్వచితమైతే ST_MTIM_NSEC
    buf->b_mtime_ns = (దీర్ఘ)st->ST_MTIM_NSEC;
#లేకపోతే
    buf->b_mtime_ns = 0;
#అయితే_ముగింపు
    buf->b_orig_కొలత = st->st_size;
#నిర్వచితమైతే HAVE_ST_బహుళకం
    buf->b_orig_బహుళకం = (పూర్ణాంకం)st->st_బహుళకం;
#లేకపోతే
    buf->b_orig_బహుళకం = mch_getperm(fname);
#అయితే_ముగింపు
}

/*
 * Adjust the line with missing eol, used for the next write.
 * Used for do_filter(), when the input lines for the filter are deleted.
 */
    శూన్య
write_lnum_adjust(linenr_T స్థానభ్రంశం)
{
    ఒకవేళ (curbuf->b_no_eol_lnum != 0)	// only if there is a missing eol
	curbuf->b_no_eol_lnum += స్థానభ్రంశం;
}

// Subfuncions for readdirex()
#నిర్వచితమైతే FEAT_EVAL
# ifdef MSWIN
    స్థిర char_u *
getfpermwfd(WIN32_కనుగొను_DATAW *wfd, char_u *perm)
{
    stat_T	    st;
    గుర్తులేని పొట్టి  st_బహుళకం;
    DWORD	    flag = wfd->dw_File_లక్షణాలు;
    WCHAR	    *wp;

    st_బహుళకం = (flag & FILE_ATTRIBUTE_DIRECTORY)
					? (_S_IFDIR | _S_IEXEC) : _S_IFREG;
    st_బహుళకం |= (flag & FILE_ATTRIBUTE_READONLY)
					? _S_IREAD : (_S_IREAD | _S_IWRITE);

    wp = wcsrchr(wfd->cFileName, L'.');
    ఒకవేళ (wp != ఖాళీ)
    {
	ఒకవేళ (_wcsicmp(wp, L".exe") == 0 ||
		_wcsicmp(wp, L".com") == 0 ||
		_wcsicmp(wp, L".cmd") == 0 ||
		_wcsicmp(wp, L".bat") == 0)
	    st_బహుళకం |= _S_IEXEC;
    }

    // Copy user bits to group/other.
    st_బహుళకం |= (st_బహుళకం & 0700) >> 3;
    st_బహుళకం |= (st_బహుళకం & 0700) >> 6;

    st.st_బహుళకం = st_బహుళకం;
    తిరిగి getfpermst(&st, perm);
}

    స్థిర శూన్య
getftypewfd(WIN32_కనుగొను_DATAW *wfd, string_T *ret)
{
    DWORD flag = wfd->dw_File_లక్షణాలు;
    DWORD ట్యాగ్ = wfd->dwReserved0;

    ఒకవేళ (flag & FILE_ATTRIBUTE_REPARSE_బిందువు)
    {
	ఒకవేళ (ట్యాగ్ == IO_REPARSE_ట్యాగ్_MOUNT_బిందువు)
	{
	    ret->string = (char_u *)"junction";
	    ret->నిడివి = STRLEN_LITERAL("junction");
	    తిరిగి;
	}
	ఒకవేళ (ట్యాగ్ == IO_REPARSE_ట్యాగ్_SYMLINK)
	{
	    ఒకవేళ (flag & FILE_ATTRIBUTE_DIRECTORY)
	    {
		ret->string = (char_u *)"linkd";
		ret->నిడివి = STRLEN_LITERAL("linkd");
		తిరిగి;
	    }

	    ret->string = (char_u *)"link";
	    ret->నిడివి = STRLEN_LITERAL("link");
	    తిరిగి;
	}

	// unknown reparse point type
	ret->string = (char_u *)"reparse";
	ret->నిడివి = STRLEN_LITERAL("reparse");
	తిరిగి;
    }

    ఒకవేళ (flag & FILE_ATTRIBUTE_DIRECTORY)
    {
	ret->string = (char_u *)"dir";
	ret->నిడివి = STRLEN_LITERAL("dir");
	తిరిగి;
    }

    ret->string = (char_u *)"file";
    ret->నిడివి = STRLEN_LITERAL("file");
}

    స్థిర dict_T *
create_readdirex_అంశం(WIN32_కనుగొను_DATAW *wfd)
{
    dict_T	*అంశం;
    char_u	*p;
    varnumber_T	కొలత, సమయం;
    char_u	permbuf[] = "---------";
    string_T	s;

    అంశం = dict_alloc();
    ఒకవేళ (అంశం == ఖాళీ)
	తిరిగి ఖాళీ;
    అంశం->dv_refcount++;

    p = utf16_గమ్యం_enc(wfd->cFileName, ఖాళీ);
    ఒకవేళ (p == ఖాళీ)
	వెళ్ళు theend;
    ఒకవేళ (dict_కూడు_string(అంశం, "name", p) == FAIL)
    {
	vim_free(p);
	వెళ్ళు theend;
    }
    vim_free(p);

    కొలత = (((varnumber_T)wfd->n_File_కొలత_ఎగువ) << 32) | wfd->n_File_కొలత_దిగువ;
    ఒకవేళ (dict_కూడు_సంఖ్య(అంశం, "size", కొలత) == FAIL)
	వెళ్ళు theend;

    // Convert FILETIME to unix time.
    సమయం = (((((varnumber_T)wfd->ft_చివరి_Write_సమయం.dw_ఎగువ_Date_సమయం) << 32) |
		wfd->ft_చివరి_Write_సమయం.dw_దిగువ_Date_సమయం)
	    - 116444736000000000) / 10000000;
    ఒకవేళ (dict_కూడు_సంఖ్య(అంశం, "time", సమయం) == FAIL)
	వెళ్ళు theend;

    getftypewfd(wfd, &s);
    ఒకవేళ (dict_కూడు_string_len(అంశం, "type", s.string, (పూర్ణాంకం)s.నిడివి) == FAIL)
	వెళ్ళు theend;
    ఒకవేళ (dict_కూడు_string(అంశం, "perm", getfpermwfd(wfd, permbuf)) == FAIL)
	వెళ్ళు theend;

    ఒకవేళ (dict_కూడు_string_len(అంశం, "user", (char_u *)"", 0) == FAIL)
	వెళ్ళు theend;
    ఒకవేళ (dict_కూడు_string_len(అంశం, "group", (char_u *)"", 0) == FAIL)
	వెళ్ళు theend;

    తిరిగి అంశం;

theend:
    dict_unref(అంశం);
    తిరిగి ఖాళీ;
}
# లేకపోతే
    స్థిర dict_T *
create_readdirex_అంశం(char_u *మార్గం, char_u *name)
{
    dict_T	*అంశం;
    అక్షరం	*p;
    size_t	pathlen, len;
    size_t	namelen;
    stat_T	st;
    పూర్ణాంకం		ret, link = FALSE;
    varnumber_T	కొలత;
    char_u	permbuf[] = "---------";
    string_T	q = {ఖాళీ, 0};
    నిర్మాణం passwd *pw;
    నిర్మాణం సమూహం  *gr;

    అంశం = dict_alloc();
    ఒకవేళ (అంశం == ఖాళీ)
	తిరిగి ఖాళీ;
    అంశం->dv_refcount++;

    pathlen = STRLEN(మార్గం);
    namelen = STRLEN(name);
    len = pathlen + 1 + namelen + 1;
    p = alloc(len);
    ఒకవేళ (p == ఖాళీ)
	వెళ్ళు theend;
    ఒకవేళ (pathlen > 0 && మార్గం[pathlen - 1] == '/')
	vim_snprintf(p, len, "%s%s", మార్గం, name);
    లేకపోతే
	vim_snprintf(p, len, "%s/%s", మార్గం, name);
    ret = mch_lstat(p, &st);
    ఒకవేళ (ret >= 0 && S_ISLNK(st.st_బహుళకం))
    {
	link = TRUE;
	ret = mch_stat(p, &st);
	ఒకవేళ (ret < 0)
	    STR_LITERAL_సమితి(q, "link");
    }
    vim_free(p);

    ఒకవేళ (dict_కూడు_string_len(అంశం, "name", name, (పూర్ణాంకం)namelen) == FAIL)
	వెళ్ళు theend;

    ఒకవేళ (ret >= 0)
    {
	కొలత = (varnumber_T)st.st_size;
	ఒకవేళ (S_ISDIR(st.st_బహుళకం))
	    కొలత = 0;
	// non-perfect check for overflow
	లేకపోతే ఒకవేళ ((off_T)కొలత != (off_T)st.st_size)
	    కొలత = -2;
	ఒకవేళ (dict_కూడు_సంఖ్య(అంశం, "size", కొలత) == FAIL)
	    వెళ్ళు theend;
	ఒకవేళ (dict_కూడు_సంఖ్య(అంశం, "time", (varnumber_T)st.st_mtime) == FAIL)
	    వెళ్ళు theend;

	ఒకవేళ (link)
	{
	    ఒకవేళ (S_ISDIR(st.st_బహుళకం))
		STR_LITERAL_సమితి(q, "linkd");
	    లేకపోతే
		STR_LITERAL_సమితి(q, "link");
	}
	లేకపోతే
	{
	    q.string = getftypest(&st);
	    q.నిడివి = STRLEN(q.string);
	}
	ఒకవేళ (dict_కూడు_string_len(అంశం, "type", q.string, (పూర్ణాంకం)q.నిడివి) == FAIL)
	    వెళ్ళు theend;
	ఒకవేళ (dict_కూడు_string(అంశం, "perm", getfpermst(&st, permbuf)) == FAIL)
	    వెళ్ళు theend;

	pw = getpwuid(st.st_uid);
	ఒకవేళ (pw == ఖాళీ)
	    STR_LITERAL_సమితి(q, "");
	లేకపోతే
	{
	    q.string = (char_u *)pw->pw_name;
	    q.నిడివి = STRLEN(q.string);
	}
	ఒకవేళ (dict_కూడు_string_len(అంశం, "user", q.string, (పూర్ణాంకం)q.నిడివి) == FAIL)
	    వెళ్ళు theend;
#  ఒకవేళ !defined(VMS) || (defined(VMS) && defined(HAVE_XOS_R_H))
	gr = getgrgid(st.st_gid);
	ఒకవేళ (gr == ఖాళీ)
	    STR_LITERAL_సమితి(q, "");
	లేకపోతే
	{
	    q.string = (char_u *)gr->gr_name;
	    q.నిడివి = STRLEN(q.string);
	}
#  endif
	ఒకవేళ (dict_కూడు_string_len(అంశం, "group", q.string, (పూర్ణాంకం)q.నిడివి) == FAIL)
	    వెళ్ళు theend;
    }
    లేకపోతే
    {
	ఒకవేళ (dict_కూడు_సంఖ్య(అంశం, "size", -1) == FAIL)
	    వెళ్ళు theend;
	ఒకవేళ (dict_కూడు_సంఖ్య(అంశం, "time", -1) == FAIL)
	    వెళ్ళు theend;
	ఒకవేళ (q.string == ఖాళీ)
	{
	    ఒకవేళ (dict_కూడు_string_len(అంశం, "type", (char_u *)"", 0) == FAIL)
		వెళ్ళు theend;
	}
	లేకపోతే
	{
	    ఒకవేళ (dict_కూడు_string_len(అంశం, "type", q.string, (పూర్ణాంకం)q.నిడివి) == FAIL)
		వెళ్ళు theend;
	}
	ఒకవేళ (dict_కూడు_string_len(అంశం, "perm", (char_u *)"", 0) == FAIL)
	    వెళ్ళు theend;
	ఒకవేళ (dict_కూడు_string_len(అంశం, "user", (char_u *)"", 0) == FAIL)
	    వెళ్ళు theend;
	ఒకవేళ (dict_కూడు_string_len(అంశం, "group", (char_u *)"", 0) == FAIL)
	    వెళ్ళు theend;
    }
    తిరిగి అంశం;

theend:
    dict_unref(అంశం);
    తిరిగి ఖాళీ;
}
# endif

    స్థిర పూర్ణాంకం
compare_readdirex_అంశం(స్థిరరాశి శూన్య *p1, స్థిరరాశి శూన్య *p2)
{
    char_u  *name1, *name2;

    name1 = dict_get_string(*(dict_T**)p1, "name", FALSE);
    name2 = dict_get_string(*(dict_T**)p2, "name", FALSE);
    ఒకవేళ (readdirex_క్రమీకరించు == READDIR_క్రమీకరించు_BYTE)
	తిరిగి STRCMP(name1, name2);
    ఒకవేళ (readdirex_క్రమీకరించు == READDIR_క్రమీకరించు_IC)
	తిరిగి STRICMP(name1, name2);

    తిరిగి STRCOLL(name1, name2);
}

    స్థిర పూర్ణాంకం
compare_readdir_అంశం(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    ఒకవేళ (readdirex_క్రమీకరించు == READDIR_క్రమీకరించు_BYTE)
	తిరిగి STRCMP(*(అక్షరం **)s1, *(అక్షరం **)s2);
    ఒకవేళ (readdirex_క్రమీకరించు == READDIR_క్రమీకరించు_IC)
	తిరిగి STRICMP(*(అక్షరం **)s1, *(అక్షరం **)s2);

    తిరిగి STRCOLL(*(అక్షరం **)s1, *(అక్షరం **)s2);
}
#అయితే_ముగింపు

#ఒకవేళ defined(TEMPDIRNAMES) || defined(FEAT_EVAL)
/*
 * Core part of "readdir()" and "readdirex()" function.
 * Retrieve the list of files/directories of "path" into "gap".
 * If "withattr" is TRUE, retrieve the names and their attributes.
 * If "withattr" is FALSE, retrieve the names only.
 * Return OK for success, FAIL for failure.
 */
    పూర్ణాంకం
readdir_core(
    garray_T	*gap,
    char_u	*మార్గం,
    పూర్ణాంకం		withattr వాడని,
    శూన్య	*context,
    పూర్ణాంకం		(*checkitem)(శూన్య *context, శూన్య *అంశం),
    పూర్ణాంకం		క్రమీకరించు)
{
    పూర్ణాంకం			failed = FALSE;
    char_u		*p;
# ifdef MSWIN
    char_u		*buf;
    పూర్ణాంకం			ok;
    HANDLE		h_కనుగొను = INVALID_HANDLE_విలువ;
    WIN32_కనుగొను_DATAW    wfd;
    WCHAR		*wn = ఖాళీ;	// UTF-16 name, NULL when not used.
    char_u		*p_చివర;
# లేకపోతే
    DIR			*dirp;
    నిర్మాణం dirent	*dp;
# endif

    ga_init2(gap, పరిమాణం(శూన్య *), 20);

# ifdef FEAT_EVAL
#  define FREE_అంశం(అంశం)   చేయి { \
	ఒకవేళ (withattr) \
	    dict_unref((dict_T*)(అంశం)); \
	లేకపోతే \
	    vim_free(అంశం); \
    } అయితే (0)

    readdirex_క్రమీకరించు = READDIR_క్రమీకరించు_BYTE;
# లేకపోతే
#  define FREE_అంశం(అంశం)   vim_free(అంశం)
# endif

# ifdef MSWIN
    buf = alloc(MAXPATHL);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి FAIL;
    STRNCPY(buf, మార్గం, MAXPATHL-5);
    p = p_చివర = buf + STRLEN(buf);
    MB_PTR_వెనుక(buf, p);
    ఒకవేళ (*p == '\\' || *p == '/')
	p_చివర = p;
    STRCPY(p_చివర, "\\*");

    wn = enc_గమ్యం_utf16(buf, ఖాళీ);
    ఒకవేళ (wn != ఖాళీ)
	h_కనుగొను = కనుగొను_మొదటి_File_W(wn, &wfd);
    ok = (h_కనుగొను != INVALID_HANDLE_విలువ);
    ఒకవేళ (!ok)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), మార్గం);
    }
    లేకపోతే
    {
	అయితే (ok)
	{
	    పూర్ణాంకం	    ignore;
	    శూన్య    *అంశం;
	    WCHAR   *wp;

	    wp = wfd.cFileName;
	    ignore = wp[0] == L'.' &&
		    (wp[1] == NUL ||
		     (wp[1] == L'.' && wp[2] == NUL));
	    ఒకవేళ (ignore)
	    {
		ok = కనుగొను_తదుపరి_File_W(h_కనుగొను, &wfd);
		కొనసాగు;
	    }
#  ifdef FEAT_EVAL
	    ఒకవేళ (withattr)
		అంశం = (శూన్య*)create_readdirex_అంశం(&wfd);
	    లేకపోతే
#  endif
		అంశం = (శూన్య*)utf16_గమ్యం_enc(wfd.cFileName, ఖాళీ);
	    ఒకవేళ (అంశం == ఖాళీ)
	    {
		failed = TRUE;
		ఆపు;
	    }

	    ఒకవేళ (!ignore && checkitem != ఖాళీ)
	    {
		పూర్ణాంకం r = checkitem(context, అంశం);

		ఒకవేళ (r < 0)
		{
		    FREE_అంశం(అంశం);
		    ఆపు;
		}
		ఒకవేళ (r == 0)
		    ignore = TRUE;
	    }

	    ఒకవేళ (!ignore)
	    {
		ఒకవేళ (ga_grow(gap, 1) == OK)
		    ((శూన్య**)gap->ga_దత్తాంశం)[gap->ga_len++] = అంశం;
		లేకపోతే
		{
		    failed = TRUE;
		    FREE_అంశం(అంశం);
		    ఆపు;
		}
	    }
	    లేకపోతే
		FREE_అంశం(అంశం);

	    ok = కనుగొను_తదుపరి_File_W(h_కనుగొను, &wfd);
	}
	కనుగొను_Close(h_కనుగొను);
    }

    vim_free(buf);
    vim_free(wn);
# లేకపోతే	// MSWIN
    dirp = opendir((అక్షరం *)మార్గం);
    ఒకవేళ (dirp == ఖాళీ)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), మార్గం);
    }
    లేకపోతే
    {
	కొరకు (;;)
	{
	    పూర్ణాంకం	    ignore;
	    శూన్య    *అంశం;

	    dp = readdir(dirp);
	    ఒకవేళ (dp == ఖాళీ)
		ఆపు;
	    p = (char_u *)dp->d_name;

	    ignore = p[0] == '.' &&
		    (p[1] == NUL ||
		     (p[1] == '.' && p[2] == NUL));
	    ఒకవేళ (ignore)
		కొనసాగు;
#  ifdef FEAT_EVAL
	    ఒకవేళ (withattr)
		అంశం = (శూన్య*)create_readdirex_అంశం(మార్గం, p);
	    లేకపోతే
#  endif
		అంశం = (శూన్య*)vim_strsave(p);
	    ఒకవేళ (అంశం == ఖాళీ)
	    {
		failed = TRUE;
		ఆపు;
	    }

	    ఒకవేళ (checkitem != ఖాళీ)
	    {
		పూర్ణాంకం r = checkitem(context, అంశం);

		ఒకవేళ (r < 0)
		{
		    FREE_అంశం(అంశం);
		    ఆపు;
		}
		ఒకవేళ (r == 0)
		    ignore = TRUE;
	    }

	    ఒకవేళ (!ignore)
	    {
		ఒకవేళ (ga_grow(gap, 1) == OK)
		    ((శూన్య**)gap->ga_దత్తాంశం)[gap->ga_len++] = అంశం;
		లేకపోతే
		{
		    failed = TRUE;
		    FREE_అంశం(అంశం);
		    ఆపు;
		}
	    }
	    లేకపోతే
		FREE_అంశం(అంశం);
	}

	closedir(dirp);
    }
# endif	// MSWIN

# undef FREE_అంశం

    ఒకవేళ (!failed && gap->ga_len > 0 && క్రమీకరించు > READDIR_క్రమీకరించు_NONE)
    {
# ifdef FEAT_EVAL
	readdirex_క్రమీకరించు = క్రమీకరించు;
	ఒకవేళ (withattr)
	    qsort((శూన్య*)gap->ga_దత్తాంశం, (size_t)gap->ga_len, పరిమాణం(dict_T*),
		    compare_readdirex_అంశం);
	లేకపోతే
	    qsort((శూన్య*)gap->ga_దత్తాంశం, (size_t)gap->ga_len, పరిమాణం(char_u *),
		    compare_readdir_అంశం);
# లేకపోతే
	    క్రమీకరించు_అక్షరశ్రేణులు((char_u **)gap->ga_దత్తాంశం, gap->ga_len);
# endif
    }

    తిరిగి failed ? FAIL : OK;
}

/*
 * Delete "name" and everything in it, recursively.
 * return 0 for success, -1 if some file was not deleted.
 */
    పూర్ణాంకం
delete_recursive(char_u *name)
{
    పూర్ణాంకం ఫలితం = 0;

    // A symbolic link to a directory itself is deleted, not the directory it
    // points to.
    ఒకవేళ (
# ఒకవేళ defined(UNIX) || defined(MSWIN)
	 mch_isrealdir(name)
# లేకపోతే
	 mch_isdir(name)
# endif
	    )
    {
	char_u	    *ఘాతాంకం = vim_strsave(name);
	garray_T    ga;

	ఒకవేళ (ఘాతాంకం == ఖాళీ)
	    తిరిగి -1;
	ఒకవేళ (readdir_core(&ga, ఘాతాంకం, FALSE, ఖాళీ, ఖాళీ, READDIR_క్రమీకరించు_NONE) == OK)
	{
	    పూర్ణాంకం	len = vim_snprintf((అక్షరం *)NameBuff, MAXPATHL, "%s/", ఘాతాంకం);
	    పూర్ణాంకం	i;

	    కొరకు (i = 0; i < ga.ga_len; ++i)
	    {
		vim_snprintf((అక్షరం *)NameBuff + len, MAXPATHL - len, "%s", ((char_u **)ga.ga_దత్తాంశం)[i]);
		ఒకవేళ (delete_recursive(NameBuff) != 0)
		    // Remember the failure but continue deleting any further
		    // entries.
		    ఫలితం = -1;
	    }
	    ga_clear_అక్షరశ్రేణులు(&ga);
	    ఒకవేళ (mch_rmdir(ఘాతాంకం) != 0)
		ఫలితం = -1;
	}
	లేకపోతే
	    ఫలితం = -1;
	vim_free(ఘాతాంకం);
    }
    లేకపోతే
	ఫలితం = mch_remove(name) == 0 ? 0 : -1;

    తిరిగి ఫలితం;
}
#అయితే_ముగింపు

#ఒకవేళ defined(TEMPDIRNAMES)
స్థిర దీర్ఘ	తాత్కాలిక_లెక్క = 0;		// Temp filename counter.

# ఒకవేళ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
/*
 * Open temporary directory and take file lock to prevent
 * to be auto-cleaned.
 */
   స్థిర శూన్య
vim_opentempdir(శూన్య)
{
    DIR *dp = ఖాళీ;

    ఒకవేళ (vim_tempdir_dp != ఖాళీ)
	తిరిగి;

    dp = opendir((స్థిరరాశి అక్షరం*)vim_tempdir);
    ఒకవేళ (dp == ఖాళీ)
	తిరిగి;

    vim_tempdir_dp = dp;
    flock(dirfd(vim_tempdir_dp), LOCK_SH);
}

/*
 * Close temporary directory - it automatically release file lock.
 */
   స్థిర శూన్య
vim_closetempdir(శూన్య)
{
    ఒకవేళ (vim_tempdir_dp == ఖాళీ)
	తిరిగి;

    closedir(vim_tempdir_dp);
    vim_tempdir_dp = ఖాళీ;
}

/*
 * Return true if the temp directory we created is gone.
 */
    స్థిర bool
vim_tempdir_gone(శూన్య)
{
    stat_T	st;

    ఒకవేళ (vim_tempdir_dp == ఖాళీ)
	తిరిగి false;
    తిరిగి fstat(dirfd(vim_tempdir_dp), &st) < 0 || st.st_nlink == 0;
}
# endif

/*
 * Delete the temp directory and all files it contains.
 */
    శూన్య
vim_deltempdir(శూన్య)
{
    ఒకవేళ (vim_tempdir == ఖాళీ)
	తిరిగి;

# ఒకవేళ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    vim_closetempdir();
# endif
    // remove the trailing path separator
    gettail(vim_tempdir)[-1] = NUL;
    delete_recursive(vim_tempdir);
    VIM_CLEAR(vim_tempdir);
}

/*
 * Directory "tempdir" was created.  Expand this name to a full path and put
 * it in "vim_tempdir".  This avoids that using ":cd" would confuse us.
 * "tempdir" must be no longer than MAXPATHL.
 */
    స్థిర శూన్య
vim_settempdir(char_u *tempdir)
{
    char_u	*buf;
    size_t	buflen;

    buf = alloc(MAXPATHL + 2);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    ఒకవేళ (vim_నిండిన_Name(tempdir, buf, MAXPATHL, FALSE) == FAIL)
	STRCPY(buf, tempdir);
    buflen = STRLEN(buf);
    ఒకవేళ (!after_pathsep(buf, buf + buflen))
    {
	STRCPY(buf + buflen, PATHSEPSTR);
	buflen += పరిమాణం(PATHSEP);
    }
    vim_tempdir = vim_strnsave(buf, buflen);
# ఒకవేళ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    vim_opentempdir();
# endif
    vim_free(buf);
}
#అయితే_ముగింపు

/*
 * vim_tempname(): Return a unique name that can be used for a temp file.
 *
 * The temp file is NOT guaranteed to be created.  If "keep" is FALSE it is
 * guaranteed to NOT be created.
 *
 * The returned pointer is to allocated memory.
 * The returned pointer is NULL if no valid name was found.
 */
    char_u  *
vim_tempname(
    పూర్ణాంకం	    extra_char వాడని,  // char to use in the name instead of '?'
    పూర్ణాంకం	    keep వాడని)
{
#నిర్వచితమైతే USE_TMPNAM
    char_u	itmp[L_tmpnam];	// use tmpnam()
#లేదావేళ defined(MSWIN)
    WCHAR	itmp[TEMPNAMELEN];
#లేకపోతే
    char_u	itmp[TEMPNAMELEN];
#అయితే_ముగింపు

#నిర్వచితమైతే TEMPDIRNAMES
    స్థిర అక్షరం	*(tempdirs[]) = {TEMPDIRNAMES};
    పూర్ణాంకం		i;
# ifndef EEXIST
    stat_T	st;
# endif

# ఒకవేళ defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    // if the temp directory is gone, force re-creation of it
    ఒకవేళ (vim_tempdir != ఖాళీ && vim_tempdir_gone())
    {
	vim_closetempdir();
	VIM_CLEAR(vim_tempdir);
    }
# endif

    /*
     * This will create a directory for private use by this instance of Vim.
     * This is done once, and the same directory is used for all temp files.
     * This method avoids security problems because of symlink attacks et al.
     * It's also a bit faster, because we only need to check for an existing
     * file when creating the directory and not for each temp file.
     */
    ఒకవేళ (vim_tempdir == ఖాళీ)
    {
	/*
	 * Try the entries in TEMPDIRNAMES to create the temp directory.
	 */
	కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(tempdirs); ++i)
	{
# ifndef HAVE_MKDTEMP
	    దీర్ఘ	nr;
	    దీర్ఘ	off;
# endif
	    size_t	itmplen;

	    // Expand $TMP, leave room for "/v1100000/999999999".
	    // Skip the directory check if the expansion fails.
	    itmplen = expand_env((char_u *)tempdirs[i], itmp, TEMPNAMELEN - 20);
	    ఒకవేళ (itmp[0] != '$' && mch_isdir(itmp))
	    {
		// directory exists
		ఒకవేళ (!after_pathsep(itmp, itmp + itmplen))
		{
		    STRCPY(itmp + itmplen, PATHSEPSTR);
		    itmplen += పరిమాణం(PATHSEP);
		}

# ifdef HAVE_MKDTEMP
		{
#  ఒకవేళ defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    mode_t	umask_save = umask(077);
#  endif
		    // Leave room for filename
		    STRCPY(itmp + itmplen, "vXXXXXX");
		    itmplen += STRLEN_LITERAL("vXXXXXX");
		    ఒకవేళ (mkdtemp((అక్షరం *)itmp) != ఖాళీ)
			vim_settempdir(itmp);
#  ఒకవేళ defined(UNIX) || defined(VMS)
		    (శూన్య)umask(umask_save);
#  endif
		}
# లేకపోతే
		// Get an arbitrary number of up to 6 digits.  When it's
		// unlikely that it already exists it will be faster,
		// otherwise it doesn't matter.  The use of mkdir() avoids any
		// security problems because of the predictable number.
		nr = (mch_get_pid() + (దీర్ఘ)సమయం(ఖాళీ)) % 1000000L;

		// Try up to 10000 different values until we find a name that
		// doesn't exist.
		కొరకు (off = 0; off < 10000L; ++off)
		{
		    పూర్ణాంకం		r;
#  ఒకవేళ defined(UNIX) || defined(VMS)
		    mode_t	umask_save;
#  endif

		    vim_snprintf((అక్షరం *)itmp + itmplen, పరిమాణం(itmp) - itmplen, "v%ld", nr + off);
#  ifndef EEXIST
		    // If mkdir() does not set errno to EEXIST, check for
		    // existing file here.  There is a race condition then,
		    // although it's fail-safe.
		    ఒకవేళ (mch_stat((అక్షరం *)itmp, &st) >= 0)
			కొనసాగు;
#  endif
#  ఒకవేళ defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    umask_save = umask(077);
#  endif
		    r = vim_mkdir(itmp, 0700);
#  ఒకవేళ defined(UNIX) || defined(VMS)
		    (శూన్య)umask(umask_save);
#  endif
		    ఒకవేళ (r == 0)
		    {
			vim_settempdir(itmp);
			ఆపు;
		    }
#  ifdef EEXIST
		    // If the mkdir() didn't fail because the file/dir exists,
		    // we probably can't create any dir here, try another
		    // place.
		    ఒకవేళ (errno != EEXIST)
#  endif
			ఆపు;
		}
# endif // HAVE_MKDTEMP
		ఒకవేళ (vim_tempdir != ఖాళీ)
		    ఆపు;
	    }
	}
    }

    ఒకవేళ (vim_tempdir != ఖాళీ)
    {
	// There is no need to check if the file exists, because we own the
	// directory and nobody else creates a file in it.
	పూర్ణాంకం itmplen = vim_snprintf((అక్షరం *)itmp, పరిమాణం(itmp), "%s%ld", vim_tempdir, తాత్కాలిక_లెక్క++);
	తిరిగి vim_strnsave(itmp, (size_t)itmplen);
    }

    తిరిగి ఖాళీ;

#లేకపోతే // TEMPDIRNAMES
    char_u	*p;

# ifdef MSWIN
    WCHAR	wsz_తాత్కాలిక_File[MAX_మార్గం_2 + 1];
    WCHAR	buf4[4];
    WCHAR	*chartab = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char_u	*retval;
    char_u	*shname;
    దీర్ఘ	i;

    wcscpy(itmp, L"");
    ఒకవేళ (Get_తాత్కాలిక_మార్గం_W(MAX_మార్గం_2, wsz_తాత్కాలిక_File) == 0)
    {
	wsz_తాత్కాలిక_File[0] = L'.';	// GetTempPathW() failed, use current dir
	wsz_తాత్కాలిక_File[1] = L'\\';
	wsz_తాత్కాలిక_File[2] = NUL;
    }
    wcscpy(buf4, L"VIM");

    // randomize the name to avoid collisions
    i = mch_get_pid() + extra_char;
    buf4[1] = chartab[i % 36];
    buf4[2] = chartab[(i / 36) % 36];
    ఒకవేళ (Get_తాత్కాలిక_File_Name_W(wsz_తాత్కాలిక_File, buf4, 0, itmp) == 0)
	తిరిగి ఖాళీ;
    ఒకవేళ (!keep)
	// GetTempFileNameW() will create the file, we don't want that
	(శూన్య)DeleteFileW(itmp);

    // Backslashes in a temp file name cause problems when filtering with
    // "sh".  NOTE: This also checks 'shellcmdflag' to help those people who
    // didn't set 'shellslash' but only if not using PowerShell.
    retval = utf16_గమ్యం_enc(itmp, ఖాళీ);
    ఒకవేళ (retval == ఖాళీ)
	తిరిగి ఖాళీ;
    shname = gettail(p_sh);
    ఒకవేళ ((*p_shcf == '-' && !(అక్షరశ్రేణి_శోధన((అక్షరం *)shname, "powershell") != ఖాళీ
			     || అక్షరశ్రేణి_శోధన((అక్షరం *)shname, "pwsh") != ఖాళీ ))
								    || p_ssl)
	కొరకు (p = retval; *p; ++p)
	    ఒకవేళ (*p == '\\')
		*p = '/';
    తిరిగి retval;

# లేకపోతే // MSWIN
    పూర్ణాంకం		itmplen;

#  ifdef USE_TMPNAM
    // tmpnam() will make its own name
    p = tmpnam((అక్షరం *)itmp);
    ఒకవేళ (p == ఖాళీ || *p == NUL)
	తిరిగి ఖాళీ;
#  లేకపోతే

#   ifdef VMS_TEMPNAM
    // mktemp() is not working on VMS.  It seems to be
    // a do-nothing function. Therefore we use tempnam().
    vim_snprintf((అక్షరం *)itmp, పరిమాణం(itmp), "VIM%c", extra_char);
    p = (char_u *)tempnam("tmp:", (అక్షరం *)itmp);
    ఒకవేళ (p != ఖాళీ)
    {
	// VMS will use '.LIS' if we don't explicitly specify an extension,
	// and VIM will then be unable to find the file later
	itmplen = vim_snprintf((అక్షరం *)itmp, పరిమాణం(itmp), "%s.txt", p);
	విడుదల(p);
    }
    లేకపోతే
	తిరిగి ఖాళీ;
#   లేకపోతే
    STRCPY(itmp, TEMPNAME);
    itmplen = STRLEN_LITERAL(TEMPNAME);
    ఒకవేళ ((p = vim_strchr(itmp, '?')) != ఖాళీ)
	*p = extra_char;
    ఒకవేళ (mktemp((అక్షరం *)itmp) == ఖాళీ)
	తిరిగి ఖాళీ;
#   endif
#  endif

    తిరిగి vim_strnsave(itmp, (size_t)itmplen);
# endif // MSWIN
#అయితే_ముగింపు // TEMPDIRNAMES
}

#ఒకవేళ defined(BACKSLASH_లోపల_FILENAME)
/*
 * Convert all backslashes in fname to forward slashes in-place, unless when
 * it looks like a URL.
 */
    శూన్య
forward_slash(char_u *fname)
{
    char_u	*p;

    ఒకవేళ (మార్గం_with_url(fname))
	తిరిగి;
    కొరకు (p = fname; *p != NUL; ++p)
	// The Big5 encoding can have '\' in the trail byte.
	ఒకవేళ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
	    ++p;
	లేకపోతే ఒకవేళ (*p == '\\')
	    *p = '/';
}
#అయితే_ముగింపు

/*
 * Try matching a filename with a "pattern" ("prog" is NULL), or use the
 * precompiled regprog "prog" ("pattern" is NULL).  That avoids calling
 * vim_regcomp() often.
 * Used for autocommands and 'wildignore'.
 * Returns TRUE if there is a match, FALSE otherwise.
 */
    పూర్ణాంకం
సరిపోలిక_file_pat(
    char_u	*నమూనా,		// pattern to match with
    regprog_T	**prog,			// pre-compiled regprog or NULL
    char_u	*fname,			// full path of file name
    char_u	*sfname,		// short file name or NULL
    char_u	*తోక,			// tail of path
    పూర్ణాంకం		allow_dirs)		// allow matching with dir
{
    regmatch_T	regmatch;
    పూర్ణాంకం		ఫలితం = FALSE;

    regmatch.rm_ic = p_fic; // ignore case if 'fileignorecase' is set
    ఒకవేళ (prog != ఖాళీ)
	regmatch.regprog = *prog;
    లేకపోతే
	regmatch.regprog = vim_regcomp(నమూనా, RE_MAGIC);

    /*
     * Try for a match with the pattern with:
     * 1. the full file name, when the pattern has a '/'.
     * 2. the short file name, when the pattern has a '/'.
     * 3. the tail of the file name, when the pattern has no '/'.
     */
    ఒకవేళ (regmatch.regprog != ఖాళీ
	     && ((allow_dirs
		     && (vim_regexec(&regmatch, fname, (colnr_T)0)
			 || (sfname != ఖాళీ
			     && vim_regexec(&regmatch, sfname, (colnr_T)0))))
		 || (!allow_dirs && vim_regexec(&regmatch, తోక, (colnr_T)0))))
	ఫలితం = TRUE;

    ఒకవేళ (prog != ఖాళీ)
	*prog = regmatch.regprog;
    లేకపోతే
	vim_regfree(regmatch.regprog);
    తిరిగి ఫలితం;
}

/*
 * Return TRUE if a file matches with a pattern in "list".
 * "list" is a comma-separated list of patterns, like 'wildignore'.
 * "sfname" is the short file name or NULL, "ffname" the long file name.
 */
    పూర్ణాంకం
సరిపోలిక_file_list(char_u *list, char_u *sfname, char_u *ffname)
{
    char_u	buf[MAXPATHL];
    char_u	*తోక;
    char_u	*regpat;
    అక్షరం	allow_dirs;
    పూర్ణాంకం		సరిపోలిక;
    char_u	*p;

    తోక = gettail(sfname);

    // try all patterns in 'wildignore'
    p = list;
    అయితే (*p)
    {
	copy_option_part(&p, buf, MAXPATHL, ",");
	regpat = file_pat_గమ్యం_reg_pat(buf, ఖాళీ, &allow_dirs, FALSE);
	ఒకవేళ (regpat == ఖాళీ)
	    ఆపు;
	సరిపోలిక = సరిపోలిక_file_pat(regpat, ఖాళీ, ffname, sfname,
						       తోక, (పూర్ణాంకం)allow_dirs);
	vim_free(regpat);
	ఒకవేళ (సరిపోలిక)
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Convert the given pattern "pat" which has shell style wildcards in it, into
 * a regular expression, and return the result in allocated memory.  If there
 * is a directory path separator to be matched, then TRUE is put in
 * allow_dirs, otherwise FALSE is put there -- webb.
 * Handle backslashes before special characters, like "\*" and "\ ".
 *
 * no_bslash only makes a difference, when BACKSLASH_IN_FILENAME is defined
 *
 * Returns NULL when out of memory.
 */
    char_u *
file_pat_గమ్యం_reg_pat(
    char_u	*pat,
    char_u	*pat_చివర,	// first char after pattern or NULL
    అక్షరం	*allow_dirs,	// Result passed back out in here
    పూర్ణాంకం		no_bslash వాడని) // Don't use a backward slash as pathsep
{
    పూర్ణాంకం		కొలత = 2; // '^' at start, '$' at end
    char_u	*endp;
    char_u	*reg_pat;
    char_u	*p;
    పూర్ణాంకం		i;
    పూర్ణాంకం		nested = 0;
    పూర్ణాంకం		కూడు_dollar = TRUE;

    ఒకవేళ (allow_dirs != ఖాళీ)
	*allow_dirs = FALSE;
    ఒకవేళ (pat_చివర == ఖాళీ)
	pat_చివర = pat + STRLEN(pat);

    కొరకు (p = pat; p < pat_చివర; p++)
    {
	ఎంపిక (*p)
	{
	    స్థితి '*':
	    స్థితి '.':
	    స్థితి ',':
	    స్థితి '{':
	    స్థితి '}':
	    స్థితి '~':
		కొలత += 2;	// extra backslash
		ఆపు;
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	    స్థితి '\\':
	    స్థితి '/':
		కొలత += 4;	// could become "[\/]"
		ఆపు;
#అయితే_ముగింపు
	    అప్రమేయ:
		కొలత++;
		ఒకవేళ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		{
		    ++p;
		    ++కొలత;
		}
		ఆపు;
	}
    }
    reg_pat = alloc(కొలత + 1);
    ఒకవేళ (reg_pat == ఖాళీ)
	తిరిగి ఖాళీ;

    i = 0;

    ఒకవేళ (pat[0] == '*')
	అయితే (pat[0] == '*' && pat < pat_చివర - 1)
	    pat++;
    లేకపోతే
	reg_pat[i++] = '^';
    endp = pat_చివర - 1;
    ఒకవేళ (endp >= pat && *endp == '*')
    {
	అయితే (endp - pat > 0 && *endp == '*')
	    endp--;
	కూడు_dollar = FALSE;
    }
    కొరకు (p = pat; *p && nested >= 0 && p <= endp; p++)
    {
	ఎంపిక (*p)
	{
	    స్థితి '*':
		reg_pat[i++] = '.';
		reg_pat[i++] = '*';
		అయితే (p[1] == '*')	// "**" matches like "*"
		    ++p;
		ఆపు;
	    స్థితి '.':
	    స్థితి '~':
		reg_pat[i++] = '\\';
		reg_pat[i++] = *p;
		ఆపు;
	    స్థితి '?':
		reg_pat[i++] = '.';
		ఆపు;
	    స్థితి '\\':
		ఒకవేళ (p[1] == NUL)
		    ఆపు;
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
		ఒకవేళ (!no_bslash)
		{
		    // translate:
		    // "\x" to "\\x"  e.g., "dir\file"
		    // "\*" to "\\.*" e.g., "dir\*.c"
		    // "\?" to "\\."  e.g., "dir\??.c"
		    // "\+" to "\+"   e.g., "fileX\+.c"
		    ఒకవేళ ((vim_isfilec(p[1]) || p[1] == '*' || p[1] == '?')
			    && p[1] != '+')
		    {
			reg_pat[i++] = '[';
			reg_pat[i++] = '\\';
			reg_pat[i++] = '/';
			reg_pat[i++] = ']';
			ఒకవేళ (allow_dirs != ఖాళీ)
			    *allow_dirs = TRUE;
			ఆపు;
		    }
		}
#అయితే_ముగింపు
		// Undo escaping from ExpandEscape():
		// foo\?bar -> foo?bar
		// foo\%bar -> foo%bar
		// foo\,bar -> foo,bar
		// foo\ bar -> foo bar
		// Don't unescape \, * and others that are also special in a
		// regexp.
		// An escaped { must be unescaped since we use magic not
		// verymagic.  Use "\\\{n,m\}"" to get "\{n,m}".
		ఒకవేళ (*++p == '?'
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
			&& no_bslash
#అయితే_ముగింపు
			)
		    reg_pat[i++] = '?';
		లేకపోతే
		    ఒకవేళ (*p == ',' || *p == '%' || *p == '#'
			       || vim_isspace(*p) || *p == '{' || *p == '}')
			reg_pat[i++] = *p;
		    లేకపోతే ఒకవేళ (*p == '\\' && p[1] == '\\' && p[2] == '{')
		    {
			reg_pat[i++] = '\\';
			reg_pat[i++] = '{';
			p += 2;
		    }
		    లేకపోతే
		    {
			ఒకవేళ (allow_dirs != ఖాళీ && vim_ispathsep(*p)
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
				&& (!no_bslash || *p != '\\')
#అయితే_ముగింపు
				)
			    *allow_dirs = TRUE;
			reg_pat[i++] = '\\';
			ఒకవేళ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
			    reg_pat[i++] = *p++;
			reg_pat[i++] = *p;
		    }
		ఆపు;
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	    స్థితి '/':
		reg_pat[i++] = '[';
		reg_pat[i++] = '\\';
		reg_pat[i++] = '/';
		reg_pat[i++] = ']';
		ఒకవేళ (allow_dirs != ఖాళీ)
		    *allow_dirs = TRUE;
		ఆపు;
#అయితే_ముగింపు
	    స్థితి '{':
		reg_pat[i++] = '\\';
		reg_pat[i++] = '(';
		nested++;
		ఆపు;
	    స్థితి '}':
		reg_pat[i++] = '\\';
		reg_pat[i++] = ')';
		--nested;
		ఆపు;
	    స్థితి ',':
		ఒకవేళ (nested)
		{
		    reg_pat[i++] = '\\';
		    reg_pat[i++] = '|';
		}
		లేకపోతే
		    reg_pat[i++] = ',';
		ఆపు;
	    అప్రమేయ:
		ఒకవేళ (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		    reg_pat[i++] = *p++;
		లేకపోతే ఒకవేళ (allow_dirs != ఖాళీ && vim_ispathsep(*p))
		    *allow_dirs = TRUE;
		reg_pat[i++] = *p;
		ఆపు;
	}
    }
    ఒకవేళ (కూడు_dollar)
	reg_pat[i++] = '$';
    reg_pat[i] = NUL;
    ఒకవేళ (nested != 0)
    {
	ఒకవేళ (nested < 0)
	    emsg(_(e_missing_open_curly));
	లేకపోతే
	    emsg(_(e_missing_close_curly));
	VIM_CLEAR(reg_pat);
    }
    తిరిగి reg_pat;
}

#ఒకవేళ defined(EINTR)
/*
 * Version of read() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    దీర్ఘ
read_eintr(పూర్ణాంకం fd, శూన్య *buf, size_t bufsize)
{
    దీర్ఘ ret;

    కొరకు (;;)
    {
	ret = vim_read(fd, buf, bufsize);
	ఒకవేళ (ret >= 0 || errno != EINTR)
	    ఆపు;
    }
    తిరిగి ret;
}

/*
 * Version of write() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    దీర్ఘ
write_eintr(పూర్ణాంకం fd, శూన్య *buf, size_t bufsize)
{
    దీర్ఘ    ret = 0;
    దీర్ఘ    wlen;

    // Repeat the write() so long it didn't fail, other than being interrupted
    // by a signal.
    అయితే (ret < (దీర్ఘ)bufsize)
    {
	wlen = vim_write(fd, (అక్షరం *)buf + ret, bufsize - ret);
	ఒకవేళ (wlen < 0)
	{
	    ఒకవేళ (errno != EINTR)
		ఆపు;
	}
	లేకపోతే
	    ret += wlen;
    }
    తిరిగి ret;
}
#అయితే_ముగింపు
