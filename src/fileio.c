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

#ضمن "vim.h"

#إذا defined(__TANDEM)
# include <limits.h>		// for SSIZE_MAX
#د_که_پای
#إذا (defined(UNIX) || defined(VMS)) && defined(FEAT_EVAL)
# include <pwd.h>
# include <grp.h>
#د_که_پای
#إذا defined(VMS) && defined(HAVE_XOS_R_H)
# include <x11/xos_r.h>
#د_که_پای

// Is there any system that doesn't have access()?
#تعریف_کړه USE_MCH_ACCESS

// Bitmask with 0x80 set in each byte of a long_u word, used to detect
// non-ASCII bytes (high bit set) in multiple bytes at once.
#تعریف_کړه NONASCII_ماسک (((long_u)-1 / 0xFF) * 0x80)

#إذا defined(__hpux) && !defined(HAVE_DIRFD)
# define dirfd(x) ((x)->__dd_fd)
# define HAVE_DIRFD
#د_که_پای

ساكن char_u *بل_fenc(char_u **pp, صحيح *alloced);
#که_تعریف_شوی FEAT_EVAL
ساكن char_u *readfile_charconvert(char_u *fname, char_u *fenc, صحيح *fdp);
#د_که_پای
#که_تعریف_شوی FEAT_CRYPT
ساكن char_u *check_for_cryptkey(char_u *cryptkey, char_u *ptr, اوږد *sizep, off_T *filesizep, صحيح newfile, char_u *fname, صحيح *did_ask);
#د_که_پای
ساكن linenr_T readfile_linenr(linenr_T linecnt, char_u *p, char_u *endp);
ساكن char_u *check_for_bom(char_u *p, اوږد اندازه, صحيح *lenp, صحيح flags);

#که_تعریف_شوی FEAT_EVAL
ساكن صحيح readdirex_ترتیبول;
#د_که_پای

    فراغ
filemess(
    buf_T	*buf,
    char_u	*name,
    char_u	*s,
    صحيح		attr)
{
    صحيح		پیغام_scroll_save;
    صحيح		prev_پیغام_ستون = پیغام_ستون;
    size_t	len;

    إذا (پیغام_silent != 0)
	أرجع;
    پیغام_جمع_کړه_fname(buf, name);	    // put file name in IObuff with quotes

    // If it's extremely long, truncate it.
    len = STRLEN(IObuff);
    إذا (len > IOSIZE - 100)
    {
	len = IOSIZE - 100;
	IObuff[len] = NUL;
    }

    // Avoid an over-long translation to cause trouble.
    إذا (*s != NUL)
	STRNCPY(IObuff + len, s, 99);

    /*
     * For the first message may have to start a new line.
     * For further ones overwrite the previous one, reset msg_scroll before
     * calling filemess().
     */
    پیغام_scroll_save = پیغام_scroll;
    إذا (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	پیغام_scroll = FALSE;
    إذا (!پیغام_scroll)	// wait a bit when overwriting an error msg
	check_for_delay(FALSE);
    پیغام_پیل();
    إذا (prev_پیغام_ستون != 0 && پیغام_ستون == 0)
	پیغام_putchar('\r');  // overwrite any previous message.
    پیغام_scroll = پیغام_scroll_save;
    پیغام_scrolled_ign = TRUE;
    // may truncate the message to avoid a hit-return prompt
    پیغام_outtrans_attr(پیغام_may_trunc(FALSE, IObuff), attr);
    پیغام_clr_eos();
    بهر_flush();
    پیغام_scrolled_ign = FALSE;
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
    صحيح
readfile(
    char_u	*fname,
    char_u	*sfname,
    linenr_T	from,
    linenr_T	کرښې_منزل_skip,
    linenr_T	کرښې_منزل_read,
    exarg_T	*eap,			// can be NULL!
    صحيح		flags)
{
    صحيح		retval = FAIL;	// jump to "theend" instead of returning
    صحيح		fd = 0;
    صحيح		newfile = (flags & READ_NEW);
    صحيح		check_readonly;
    صحيح		filtering = (flags & READ_FILTER);
    صحيح		read_stdin = (flags & READ_STDIN);
    صحيح		read_بفر = (flags & READ_بفر);
    صحيح		read_fifo = (flags & READ_FIFO);
    صحيح		سېټ_options = newfile || read_بفر
					   || (eap != عدم && eap->read_edit);
    linenr_T	read_buf_lnum = 1;	// next line to read from curbuf
    colnr_T	read_buf_ستون = 0;	// next char to read from this line
    char_u	c;
    linenr_T	lnum = from;
    char_u	*ptr = عدم;		// pointer into read buffer
    char_u	*بفر = عدم;		// read buffer
    char_u	*new_بفر = عدم;	// init to shut up gcc
    char_u	*کرښه_پیل = عدم;	// init to shut up gcc
    صحيح		wasempty;		// buffer was empty before reading
    colnr_T	len;
    اوږد	اندازه = 0;
    char_u	*p;
    off_T	filesize = 0;
    صحيح		skip_read = FALSE;
#که_تعریف_شوی FEAT_CRYPT
    off_T       filesize_disk = 0;      // file size read from disk
    off_T       filesize_شمېر = 0;     // counter
    char_u	*cryptkey = عدم;
    صحيح		did_ask_for_کلی = FALSE;
#د_که_پای
#که_تعریف_شوی FEAT_PERSISTENT_UNDO
    context_sha256_T sha_ctx;
    صحيح		read_undo_file = FALSE;
#د_که_پای
    صحيح		وېشل = 0;		// number of split lines
#تعریف_کړه UNKNOWN	 0x0fffffff		// file size is unknown
    linenr_T	linecnt;
    صحيح		error = FALSE;		// errors encountered
    صحيح		ff_error = EOL_UNKNOWN; // file format with errors
    اوږد	linerest = 0;		// remaining chars in line
#که_تعریف_شوی UNIX
    صحيح		perm = 0;
    صحيح		بدلون_ډېرپېښ = -1;		// protection bits for swap file
#وإلا
    صحيح		perm;
#د_که_پای
    صحيح		fileformat = 0;		// end-of-line format
    صحيح		keep_fileformat = FALSE;
    stat_T	st;
    صحيح		file_readonly;
    linenr_T	skip_شمېر = 0;
    linenr_T	read_شمېر = 0;
    صحيح		پیغام_save = پیغام_scroll;
    linenr_T	read_no_eol_lnum = 0;   // non-zero lnum when last line of
					// last read was missing the eol
    صحيح		try_mac;
    صحيح		try_dos;
    صحيح		try_unix;
    صحيح		file_rewind = FALSE;
    صحيح		can_retry;
    linenr_T	conv_error = 0;		// line nr with conversion error
    linenr_T	illegal_byte = 0;	// line nr with illegal byte
    صحيح		keep_dest_enc = FALSE;	// don't retry when char doesn't fit
					// in destination encoding
    صحيح		bad_char_behavior = BAD_REPLACE;
					// BAD_KEEP, BAD_DROP or character to
					// replace with
    char_u	*tmpname = عدم;	// name of 'charconvert' output file
    صحيح		fio_flags = 0;
    char_u	*fenc;			// fileencoding to use
    صحيح		fenc_alloced;		// fenc_next is in allocated memory
    char_u	*fenc_بل = عدم;	// next item in 'fencs' or NULL
    صحيح		advance_fenc = FALSE;
    اوږد	real_اندازه = 0;
#که_تعریف_شوی USE_ICONV
    iconv_t	iconv_fd = (iconv_t)-1;	// descriptor for iconv() or -1
# ifdef FEAT_EVAL
    صحيح		did_iconv = FALSE;	// TRUE when iconv() failed and trying
					// 'charconvert' next
# endif
#د_که_پای
    صحيح		converted = FALSE;	// TRUE if conversion done
    صحيح		notconverted = FALSE;	// TRUE if conversion wanted but it
					// wasn't possible
    char_u	conv_rest[CONV_RESTLEN];
    صحيح		conv_restlen = 0;	// nr of bytes in conv_rest[]
    pos_T	orig_پیل;
    buf_T	*old_curbuf;
    char_u	*old_b_ffname;
    char_u	*old_b_fname;
    صحيح		using_b_ffname;
    صحيح		using_b_fname;
    ساكن حرف *پیغام_is_a_directory = N_("is a directory");
#که_تعریف_شوی FEAT_CRYPT
    صحيح		eof = FALSE;
#د_که_پای
#که_تعریف_شوی FEAT_SODIUM
    صحيح		may_need_lseek = FALSE;
#د_که_پای
    size_t	fnamelen = 0;

    curbuf->b_au_did_filetype = false; // reset before triggering any autocommands

    curbuf->b_no_eol_lnum = 0;	// in case it was set by the previous read

    /*
     * If there is no file name yet, use the one for the read file.
     * BF_NOTEDITED is set to reflect this.
     * Don't do this for a read from a filter.
     * Only do this when 'cpoptions' contains the 'f' flag.
     */
    إذا (curbuf->b_ffname == عدم
	    && !filtering
	    && fname != عدم
	    && vim_strchr(p_cpo, CPO_FNAMER) != عدم
	    && !(flags & READ_DUMMY))
    {
	إذا (سېټ_rw_fname(fname, sfname) == FAIL)
	    اذهب theend;
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
    إذا (sfname == عدم)
	sfname = fname;
#إذا defined(UNIX)
    fname = sfname;
#د_که_پای

    /*
     * The BufReadCmd and FileReadCmd events intercept the reading process by
     * executing the associated commands instead.
     */
    إذا (!filtering && !read_stdin && !read_بفر)
    {
	orig_پیل = curbuf->b_op_پیل;

	// Set '[ mark to the line above where the lines go (line 1 if zero).
	curbuf->b_op_پیل.lnum = ((from == 0) ? 1 : from);
	curbuf->b_op_پیل.ستون = 0;

	إذا (newfile)
	{
	    إذا (apply_autocmds_exarg(EVENT_BUFREADCMD, عدم, sfname,
							  FALSE, curbuf, eap))
	    {
		retval = OK;
#که_تعریف_شوی FEAT_EVAL
		إذا (aborting())
		    retval = FAIL;
#د_که_پای
		// The BufReadCmd code usually uses ":read" to get the text and
		// perhaps ":file" to change the buffer name. But we should
		// consider this to work like ":edit", thus reset the
		// BF_NOTEDITED flag.  Then ":write" will work to overwrite the
		// same file.
		إذا (retval == OK)
		    curbuf->b_flags &= ~BF_NOTEDITED;
		اذهب theend;
	    }
	}
	وإلا إذا (apply_autocmds_exarg(EVENT_FILEREADCMD, sfname, sfname,
							    FALSE, عدم, eap))
	{
#که_تعریف_شوی FEAT_EVAL
	    retval = aborting() ? FAIL : OK;
#وإلا
	    retval = OK;
#د_که_پای
	    اذهب theend;
	}

	curbuf->b_op_پیل = orig_پیل;

	إذا (flags & READ_NOFILE)
	{
	    // Return NOTDONE instead of FAIL so that BufEnter can be triggered
	    // and other operations don't fail.
	    retval = NOTDONE;
	    اذهب theend;
	}
    }

    إذا ((shortmess(SHM_OVER) || curbuf->b_help) && p_verbose == 0)
	پیغام_scroll = FALSE;	// overwrite previous file message
    وإلا
	پیغام_scroll = TRUE;	// don't overwrite previous file message

    إذا (fname != عدم && *fname != NUL)
    {
	fnamelen = STRLEN(fname);

	// If the name is too long we might crash further on, quit here.
	إذا (fnamelen >= MAXPATHL)
	{
	    filemess(curbuf, fname, (char_u *)_("Illegal file name"), 0);
	    پیغام_پای_ټکی();
	    پیغام_scroll = پیغام_save;
	    اذهب theend;
	}

	// If the name ends in a path separator, we can't open it.  Check here,
	// because reading the file may actually work, but then creating the
	// swap file may destroy it!  Reported on MS-DOS and Win 95.
	إذا (after_pathsep(fname, fname + fnamelen))
	{
	    filemess(curbuf, fname, (char_u *)_(پیغام_is_a_directory), 0);
	    پیغام_پای_ټکی();
	    پیغام_scroll = پیغام_save;
	    retval = NOTDONE;
	    اذهب theend;
	}
    }
#إذا defined(UNIX) || defined(VMS)
    إذا (!read_stdin && fname != عدم)
	perm = mch_getperm(fname);
#د_که_پای

    إذا (!read_stdin && !read_بفر && !read_fifo)
    {
#إذا defined(UNIX) || defined(VMS)
	إذا (perm >= 0 && !S_ISREG(perm)		    // not a regular file ...
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
	    إذا (S_ISDIR(perm))
	    {
		filemess(curbuf, fname, (char_u *)_(پیغام_is_a_directory), 0);
		retval = NOTDONE;
	    }
	    وإلا
		filemess(curbuf, fname, (char_u *)_("is not a file"), 0);
	    پیغام_پای_ټکی();
	    پیغام_scroll = پیغام_save;
	    اذهب theend;
	}
#د_که_پای
#إذا defined(MSWIN)
	/*
	 * MS-Windows allows opening a device, but we will probably get stuck
	 * trying to read it.
	 */
	إذا (!p_odev && mch_nodetype(fname) == غوټه_WRITABLE)
	{
	    filemess(curbuf, fname, (char_u *)_("is a device (disabled with 'opendevice' option)"), 0);
	    پیغام_پای_ټکی();
	    پیغام_scroll = پیغام_save;
	    اذهب theend;
	}
#د_که_پای
    }

    // Set default or forced 'fileformat' and 'binary'.
    سېټ_file_options(سېټ_options, eap);

    /*
     * When opening a new file we take the readonly flag from the file.
     * Default is r/w, can be set to r/o below.
     * Don't reset it when in readonly mode
     * Only set/reset b_p_ro when BF_CHECK_RO is set.
     */
    check_readonly = (newfile && (curbuf->b_flags & BF_CHECK_RO));
    إذا (check_readonly && !readonlymode)
	curbuf->b_p_ro = FALSE;

    إذا (newfile && !read_stdin && !read_بفر && !read_fifo)
    {
	// Remember time of file.
	إذا (mch_stat((حرف *)fname, &st) >= 0)
	{
	    buf_store_وخت(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
#که_تعریف_شوی FEAT_CRYPT
	    filesize_disk = st.st_size;
#د_که_پای
#که_تعریف_شوی UNIX
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
	    بدلون_ډېرپېښ = (st.st_ډېرپېښ & 0644) | 0600;
#د_که_پای
#که_تعریف_شوی VMS
	    curbuf->b_fab_rfm = st.st_fab_rfm;
	    curbuf->b_fab_rat = st.st_fab_rat;
	    curbuf->b_fab_mrs = st.st_fab_mrs;
#د_که_پای
	}
	وإلا
	{
	    curbuf->b_mtime = 0;
	    curbuf->b_mtime_ns = 0;
	    curbuf->b_mtime_read = 0;
	    curbuf->b_mtime_read_ns = 0;
	    curbuf->b_orig_اندازه = 0;
	    curbuf->b_orig_ډېرپېښ = 0;
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
    إذا (read_stdin)
    {
#إذا defined(MSWIN)
	// Force binary I/O on stdin to avoid CR-LF -> LF conversion.
	setmode(0, O_دوګونی);
#د_که_پای
    }
    وإلا إذا (!read_بفر)
    {
#که_تعریف_شوی USE_MCH_ACCESS
	إذا (
# ifdef UNIX
	    !(perm & 0222) ||
# endif
				mch_access((حرف *)fname, W_OK))
	    file_readonly = TRUE;
	fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0);
#وإلا
	إذا (!newfile
		|| readonlymode
		|| (fd = mch_open((حرف *)fname, O_RDWR | O_EXTRA, 0)) < 0)
	{
	    file_readonly = TRUE;
	    // try to open ro
	    fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0);
	}
#د_که_پای
    }

    إذا (fd < 0)			    // cannot open at all
    {
#که_نه_تعریف_شوی UNIX
	صحيح	isdir_f;
#د_که_پای
	پیغام_scroll = پیغام_save;
#که_نه_تعریف_شوی UNIX
	/*
	 * On Amiga we can't open a directory, check here.
	 */
	isdir_f = (mch_isdir(fname));
	perm = mch_getperm(fname);  // check if the file exists
	إذا (isdir_f)
	{
	    filemess(curbuf, sfname, (char_u *)_(پیغام_is_a_directory), 0);
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	}
	وإلا
#د_که_پای
	    إذا (newfile)
	    {
		إذا (perm < 0
#که_تعریف_شوی ENOENT
			&& errno == ENOENT
#د_که_پای
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
		    إذا (!bt_dontwrite(curbuf))
		    {
			check_need_بدلون(newfile);
			// SwapExists autocommand may mess things up
			إذا (curbuf != old_curbuf
				|| (using_b_ffname
					&& (old_b_ffname != curbuf->b_ffname))
				|| (using_b_fname
					 && (old_b_fname != curbuf->b_fname)))
			{
			    emsg(_(e_autocommands_بدل_شوی_بفر_or_بفر_name));
			    اذهب theend;
			}
		    }
		    إذا (dir_د_file_exists(fname))
			filemess(curbuf, sfname,
					      (char_u *)new_file_پیغام(), 0);
		    وإلا
			filemess(curbuf, sfname,
					   (char_u *)_("[New DIRECTORY]"), 0);
#که_تعریف_شوی FEAT_VIMINFO
		    // Even though this is a new file, it might have been
		    // edited before and deleted.  Get the old marks.
		    check_marks_read();
#د_که_پای
		    // Set forced 'fileencoding'.
		    إذا (eap != عدم)
			سېټ_forced_fenc(eap);
		    apply_autocmds_exarg(EVENT_BUFNEWFILE, sfname, sfname,
							  FALSE, curbuf, eap);
		    // remember the current fileformat
		    save_file_ff(curbuf);

#إذا defined(FEAT_EVAL)
		    إذا (!aborting())   // autocmds may abort script processing
#د_که_پای
			retval = OK;	    // a new file is not an error
		    اذهب theend;
		}
		وإلا
		{
		    filemess(curbuf, sfname, (char_u *)(
#که_تعریف_شوی EFBIG
			    (errno == EFBIG) ? _("[File too big]") :
#د_که_پای
#که_تعریف_شوی EOVERFLOW
			    (errno == EOVERFLOW) ? _("[File too big]") :
#د_که_پای
						_("[Permission Denied]")), 0);
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
		}
	    }

	اذهب theend;
    }

    /*
     * Only set the 'ro' flag for readonly files the first time they are
     * loaded.	Help files always get readonly mode
     */
    إذا ((check_readonly && file_readonly) || curbuf->b_help)
	curbuf->b_p_ro = TRUE;

    إذا (سېټ_options)
    {
	// Don't change 'eol' if reading from buffer as it will already be
	// correctly set when reading stdin.
	إذا (!read_بفر)
	{
	    curbuf->b_p_eof = FALSE;
	    curbuf->b_پیل_eof = FALSE;
	    curbuf->b_p_eol = TRUE;
	    curbuf->b_پیل_eol = TRUE;
	}
	curbuf->b_p_bomb = FALSE;
	curbuf->b_پیل_bomb = FALSE;
    }

    // Create a swap file now, so that other Vims are warned that we are
    // editing this file.
    // Don't do this for a "nofile" or "nowrite" buffer type.
    إذا (!bt_dontwrite(curbuf))
    {
	check_need_بدلون(newfile);
	إذا (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))))
	{
	    emsg(_(e_autocommands_بدل_شوی_بفر_or_بفر_name));
	    إذا (!read_بفر)
		close(fd);
	    اذهب theend;
	}
#که_تعریف_شوی UNIX
	// Set swap file protection bits after creating it.
	إذا (بدلون_ډېرپېښ > 0 && curbuf->b_ml.ml_mfp != عدم
			  && curbuf->b_ml.ml_mfp->mf_fname != عدم)
	{
	    char_u *بدلون_fname = curbuf->b_ml.ml_mfp->mf_fname;

	    /*
	     * If the group-read bit is set but not the world-read bit, then
	     * the group must be equal to the group of the original file.  If
	     * we can't make that happen then reset the group-read bit.  This
	     * avoids making the swap file readable to more users when the
	     * primary group of the user is too permissive.
	     */
	    إذا ((بدلون_ډېرپېښ & 044) == 040)
	    {
		stat_T	بدلون_st;

		إذا (mch_stat((حرف *)بدلون_fname, &بدلون_st) >= 0
			&& st.st_gid != بدلون_st.st_gid
# ifdef HAVE_FCHOWN
			&& fchown(curbuf->b_ml.ml_mfp->mf_fd, -1, st.st_gid)
									  == -1
# endif
		   )
		    بدلون_ډېرپېښ &= 0600;
	    }

	    (فراغ)mch_setperm(بدلون_fname, (اوږد)بدلون_ډېرپېښ);
	}
#د_که_پای
    }

    // If "Quit" selected at ATTENTION dialog, don't load the file
    إذا (بدلون_exists_action == SEA_QUIT)
    {
	إذا (!read_بفر && !read_stdin)
	    close(fd);
	اذهب theend;
    }

    ++no_wait_return;	    // don't wait for return yet

    /*
     * Set '[ mark to the line above where the lines go (line 1 if zero).
     */
    orig_پیل = curbuf->b_op_پیل;
    curbuf->b_op_پیل.lnum = ((from == 0) ? 1 : from);
    curbuf->b_op_پیل.ستون = 0;

    try_mac = (vim_strchr(p_ffs, 'm') != عدم);
    try_dos = (vim_strchr(p_ffs, 'd') != عدم);
    try_unix = (vim_strchr(p_ffs, 'x') != عدم);

    إذا (!read_بفر)
    {
	صحيح	m = پیغام_scroll;
	صحيح	n = پیغام_scrolled;

	/*
	 * The file must be closed again, the autocommands may want to change
	 * the file before reading it.
	 */
	إذا (!read_stdin)
	    close(fd);		// ignore errors

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	پیغام_scroll = TRUE;
	إذا (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPRE, عدم, sfname,
							  FALSE, curbuf, eap);
	وإلا إذا (read_stdin)
	    apply_autocmds_exarg(EVENT_STDINREADPRE, عدم, sfname,
							  FALSE, curbuf, eap);
	وإلا إذا (newfile)
	    apply_autocmds_exarg(EVENT_BUFREADPRE, عدم, sfname,
							  FALSE, curbuf, eap);
	وإلا
	    apply_autocmds_exarg(EVENT_FILEREADPRE, sfname, sfname,
							    FALSE, عدم, eap);
	// autocommands may have changed it
	try_mac = (vim_strchr(p_ffs, 'm') != عدم);
	try_dos = (vim_strchr(p_ffs, 'd') != عدم);
	try_unix = (vim_strchr(p_ffs, 'x') != عدم);
	curbuf->b_op_پیل = orig_پیل;

	إذا (پیغام_scrolled == n)
	    پیغام_scroll = m;

#که_تعریف_شوی FEAT_EVAL
	إذا (aborting())	    // autocmds may abort script processing
	{
	    --no_wait_return;
	    پیغام_scroll = پیغام_save;
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    اذهب theend;
	}
#د_که_پای
	/*
	 * Don't allow the autocommands to change the current buffer.
	 * Try to re-open the file.
	 *
	 * Don't allow the autocommands to change the buffer name either
	 * (cd for example) if it invalidates fname or sfname.
	 */
	إذا (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))
		|| (fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0)) < 0))
	{
	    --no_wait_return;
	    پیغام_scroll = پیغام_save;
	    إذا (fd < 0)
		emsg(_(e_readpre_autocommands_made_file_unreadable));
	    وإلا
		emsg(_(e_readpre_autocommands_must_not_change_current_بفر));
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    اذهب theend;
	}
    }

    // Autocommands may add lines to the file, need to check if it is empty
    wasempty = (curbuf->b_ml.ml_flags & ML_تشه);

    إذا (!recoverymode && !filtering && !(flags & READ_DUMMY))
    {
	/*
	 * Show the user that we are busy reading the input.  Sometimes this
	 * may take a while.  When reading from stdin another program may
	 * still be running, don't move the cursor to the last line, unless
	 * always using the GUI.
	 */
	إذا (read_stdin)
	{
	    إذا (!is_not_a_غړی())
	    {
#که_نه_تعریف_شوی ALWAYS_USE_GUI
# ifdef VIMDLL
		إذا (!gui.دننه_use)
# endif
		    mch_پیغام(_("Vim: Reading from stdin...\n"));
#د_که_پای
#که_تعریف_شوی FEAT_GUI
		// Also write a message in the GUI window, if there is one.
		إذا (gui.دننه_use && !gui.dying && !gui.starting)
		{
		    size_t  plen = STRLEN(_("Reading from stdin..."));

		    // make a copy, gui_write() may try to change it
		    p = vim_strnsave((char_u *)_("Reading from stdin..."), plen);
		    إذا (p != عدم)
		    {
			gui_write(p, (صحيح)plen);
			vim_free(p);
		    }
		}
#د_که_پای
	    }
	}
	وإلا إذا (!read_بفر)
	    filemess(curbuf, sfname, (char_u *)"", 0);
    }

    پیغام_scroll = FALSE;			// overwrite the file message

    /*
     * Set linecnt now, before the "retry" caused by a wrong guess for
     * fileformat, and after the autocommands, which may change them.
     */
    linecnt = curbuf->b_ml.ml_کرښه_شمېر;

    // "++bad=" argument.
    إذا (eap != عدم && eap->bad_char != 0)
    {
	bad_char_behavior = eap->bad_char;
	إذا (سېټ_options)
	    curbuf->b_bad_char = eap->bad_char;
    }
    وإلا
	curbuf->b_bad_char = 0;

    /*
     * Decide which 'encoding' to use or use first.
     */
    إذا (eap != عدم && eap->force_enc != 0)
    {
	fenc = enc_canonize(eap->cmd + eap->force_enc);
	fenc_alloced = TRUE;
	keep_dest_enc = TRUE;
    }
    وإلا إذا (curbuf->b_p_bin)
    {
	fenc = (char_u *)"";		// binary: don't convert
	fenc_alloced = FALSE;
    }
    وإلا إذا (curbuf->b_help)
    {
	char_u	    firstline[80];
	صحيح	    fc;

	// Help files are either utf-8 or latin1.  Try utf-8 first, if this
	// fails it must be latin1.
	// Always do this when 'encoding' is "utf-8".  Otherwise only do
	// this when needed to avoid [converted] remarks all the time.
	// It is needed when the first line contains non-ASCII characters.
	// That is only in *.??x files.
	fenc = (char_u *)"latin1";
	c = enc_utf8;
	إذا (!c && !read_stdin)
	{
	    fc = fname[fnamelen - 1];
	    إذا (TOLOWER_ASC(fc) == 'x')
	    {
		// Read the first line (and a bit more).  Immediately rewind to
		// the start of the file.  If the read() fails "len" is -1.
		len = read_eintr(fd, firstline, 80);
		vim_lseek(fd, (off_T)0L, SEEK_SET);
		لكل (p = firstline; p < firstline + len; ++p)
		    إذا (*p >= 0x80)
		    {
			c = TRUE;
			اكسر;
		    }
	    }
	}

	إذا (c)
	{
	    fenc_بل = fenc;
	    fenc = (char_u *)"utf-8";

	    // When the file is utf-8 but a character doesn't fit in
	    // 'encoding' don't retry.  In help text editing utf-8 bytes
	    // doesn't make sense.
	    إذا (!enc_utf8)
		keep_dest_enc = TRUE;
	}
	fenc_alloced = FALSE;
    }
    وإلا إذا (*p_fencs == NUL)
    {
	fenc = curbuf->b_p_fenc;	// use format from buffer
	fenc_alloced = FALSE;
    }
    وإلا
    {
	fenc_بل = p_fencs;		// try items in 'fileencodings'
	fenc = بل_fenc(&fenc_بل, &fenc_alloced);
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

    إذا (file_rewind)
    {
	إذا (read_بفر)
	{
	    read_buf_lnum = 1;
	    read_buf_ستون = 0;
	}
	وإلا إذا (read_stdin || vim_lseek(fd, (off_T)0L, SEEK_SET) != 0)
	{
	    // Can't rewind the file, give up.
	    error = TRUE;
	    اذهب failed;
	}
	// Delete the previously read lines.
	طالما (lnum > from)
	    ml_delete(lnum--);
	file_rewind = FALSE;
	إذا (سېټ_options)
	{
	    curbuf->b_p_bomb = FALSE;
	    curbuf->b_پیل_bomb = FALSE;
	}
	conv_error = 0;
    }

    /*
     * When retrying with another "fenc" and the first time "fileformat"
     * will be reset.
     */
    إذا (keep_fileformat)
	keep_fileformat = FALSE;
    وإلا
    {
	إذا (eap != عدم && eap->force_ff != 0)
	{
	    fileformat = get_fileformat_force(curbuf, eap);
	    try_unix = try_dos = try_mac = FALSE;
	}
	وإلا إذا (curbuf->b_p_bin)
	    fileformat = EOL_UNIX;		// binary: use Unix format
	وإلا إذا (*p_ffs == NUL)
	    fileformat = get_fileformat(curbuf);// use format from buffer
	وإلا
	    fileformat = EOL_UNKNOWN;		// detect from file
    }

#که_تعریف_شوی USE_ICONV
    إذا (iconv_fd != (iconv_t)-1)
    {
	// aborted conversion with iconv(), close the descriptor
	iconv_close(iconv_fd);
	iconv_fd = (iconv_t)-1;
    }
#د_که_پای

    إذا (advance_fenc)
    {
	/*
	 * Try the next entry in 'fileencodings'.
	 */
	advance_fenc = FALSE;

	إذا (eap != عدم && eap->force_enc != 0)
	{
	    // Conversion given with "++cc=" wasn't possible, read
	    // without conversion.
	    notconverted = TRUE;
	    conv_error = 0;
	    إذا (fenc_alloced)
		vim_free(fenc);
	    fenc = (char_u *)"";
	    fenc_alloced = FALSE;
	}
	وإلا
	{
	    إذا (fenc_alloced)
		vim_free(fenc);
	    إذا (fenc_بل != عدم)
	    {
		fenc = بل_fenc(&fenc_بل, &fenc_alloced);
	    }
	    وإلا
	    {
		fenc = (char_u *)"";
		fenc_alloced = FALSE;
	    }
	}
	إذا (tmpname != عدم)
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
    إذا (converted)
    {
	// "ucs-bom" means we need to check the first bytes of the file
	// for a BOM.
	إذا (STRCMP(fenc, ENC_UCSBOM) == 0)
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
	وإلا إذا (enc_utf8 || STRCMP(p_enc, "latin1") == 0)
	    fio_flags = get_fio_flags(fenc);

#که_تعریف_شوی MSWIN
	/*
	 * Conversion from an MS-Windows codepage to UTF-8 or another codepage
	 * is handled with MultiByteToWideChar().
	 */
	إذا (fio_flags == 0)
	    fio_flags = get_win_fio_flags(fenc);
#د_که_پای

#که_تعریف_شوی MACOS_CONVERT
	// Conversion from Apple MacRoman to latin1 or UTF-8
	إذا (fio_flags == 0)
	    fio_flags = get_mac_fio_flags(fenc);
#د_که_پای

#که_تعریف_شوی USE_ICONV
	/*
	 * Try using iconv() if we can't convert internally.
	 */
	إذا (fio_flags == 0
# ifdef FEAT_EVAL
		&& !did_iconv
# endif
		)
	    iconv_fd = (iconv_t)my_iconv_open(
				  enc_utf8 ? (char_u *)"utf-8" : p_enc, fenc);
#د_که_پای

#که_تعریف_شوی FEAT_EVAL
	/*
	 * Use the 'charconvert' expression when conversion is required
	 * and we can't do it internally or with iconv().
	 */
	إذا (fio_flags == 0 && !read_stdin && !read_بفر && *p_ccv != NUL
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
	    إذا (tmpname == عدم)
	    {
		tmpname = readfile_charconvert(fname, fenc, &fd);
		إذا (tmpname == عدم)
		{
		    // Conversion failed.  Try another one.
		    advance_fenc = TRUE;
		    إذا (fd < 0)
		    {
			// Re-opening the original file failed!
			emsg(_(e_conversion_mad_file_unreadable));
			error = TRUE;
			اذهب failed;
		    }
		    اذهب retry;
		}
	    }
	}
	وإلا
#د_که_پای
	{
	    إذا (fio_flags == 0
#که_تعریف_شوی USE_ICONV
		    && iconv_fd == (iconv_t)-1
#د_که_پای
	       )
	    {
		// Conversion wanted but we can't.
		// Try the next conversion in 'fileencodings'
		advance_fenc = TRUE;
		اذهب retry;
	    }
	}
    }

    // Set "can_retry" when it's possible to rewind the file and try with
    // another "fenc" value.  It's FALSE when no other "fenc" to try, reading
    // stdin or fixed at a specific encoding.
    can_retry = (*fenc != NUL && !read_stdin && !read_fifo && !keep_dest_enc);

    إذا (!skip_read)
    {
	linerest = 0;
	filesize = 0;
#که_تعریف_شوی FEAT_CRYPT
	filesize_شمېر = 0;
#د_که_پای
	skip_شمېر = کرښې_منزل_skip;
	read_شمېر = کرښې_منزل_read;
	conv_restlen = 0;
#که_تعریف_شوی FEAT_PERSISTENT_UNDO
	read_undo_file = (newfile && (flags & READ_KEEP_UNDO) == 0
				  && curbuf->b_ffname != عدم
				  && curbuf->b_p_udf
				  && !filtering
				  && !read_fifo
				  && !read_stdin
				  && !read_بفر);
	إذا (read_undo_file)
	    sha256_پیل(&sha_ctx);
#د_که_پای
#که_تعریف_شوی FEAT_CRYPT
	إذا (curbuf->b_cryptstate != عدم)
	{
	    // Need to free the state, but keep the key, don't want to ask for
	    // it again.
	    crypt_free_state(curbuf->b_cryptstate);
	    curbuf->b_cryptstate = عدم;
	}
#د_که_پای
    }

    طالما (!error && !got_int)
    {
	/*
	 * We allocate as much space for the file as we can get, plus
	 * space for the old line plus room for one terminating NUL.
	 * The amount is limited by the fact that read() only can read
	 * up to max_unsigned characters (and other things).
	 */
	إذا (!skip_read)
	{
#إذا defined(SSIZE_MAX) && (SSIZE_MAX < 0x10000L)
		اندازه = SSIZE_MAX;		    // use max I/O size, 52K
#وإلا
		// Use buffer >= 64K.  Add linerest to double the size if the
		// line gets very long, to avoid a lot of copying. But don't
		// read more than 1 Mbyte at a time, so we can be interrupted.
		اندازه = 0x10000L + linerest;
		إذا (اندازه > 0x100000L)
		    اندازه = 0x100000L;
#د_که_پای
	}

	// Protect against the argument of lalloc() going negative.
	// Also split lines that are too long for colnr_T.  After this check
	// passes, we read up to 'size' more bytes.  We must ensure that even
	// after that read, the line length won't exceed MAXCOL - 1 (because
	// we add 1 for the NUL when casting to colnr_T).  If this check fires,
	// we insert a synthetic newline immediately, so linerest doesn't grow.
	إذا (اندازه < 0 || اندازه + linerest + 1 < 0 || linerest >= MAXCOL - اندازه)
	{
	    ++وېشل;
	    *ptr = NL;		    // split line by inserting a NL
	    اندازه = 1;
	}
	وإلا
	{
	    إذا (!skip_read)
	    {
		لكل ( ; اندازه >= 10; اندازه = (اوږد)((long_u)اندازه >> 1))
		{
		    إذا ((new_بفر = lalloc(اندازه + linerest + 1,
							      FALSE)) != عدم)
			اكسر;
		}
		إذا (new_بفر == عدم)
		{
		    do_outofmem_پیغام((long_u)(اندازه * 2 + linerest + 1));
		    error = TRUE;
		    اكسر;
		}
		إذا (linerest)	// copy characters from the previous buffer
		    mch_memmove(new_بفر, ptr - linerest, (size_t)linerest);
		vim_free(بفر);
		بفر = new_بفر;
		ptr = بفر + linerest;
		کرښه_پیل = بفر;

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
		real_اندازه = (صحيح)اندازه;
#که_تعریف_شوی USE_ICONV
		إذا (iconv_fd != (iconv_t)-1)
		    اندازه = اندازه / ICONV_MULT;
		وإلا
#د_که_پای
		    إذا (fio_flags & FIO_LATIN1)
		    اندازه = اندازه / 2;
		وإلا إذا (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    اندازه = (اندازه * 2 / 3) & ~1;
		وإلا إذا (fio_flags & FIO_UCS4)
		    اندازه = (اندازه * 2 / 3) & ~3;
		وإلا إذا (fio_flags == FIO_UCSBOM)
		    اندازه = اندازه / ICONV_MULT;	// worst case
#که_تعریف_شوی MSWIN
		وإلا إذا (fio_flags & FIO_CODEPAGE)
		    اندازه = اندازه / ICONV_MULT;	// also worst case
#د_که_پای
#که_تعریف_شوی MACOS_CONVERT
		وإلا إذا (fio_flags & FIO_MACROMAN)
		    اندازه = اندازه / ICONV_MULT;	// also worst case
#د_که_پای

		إذا (conv_restlen > 0)
		{
		    // Insert unconverted bytes from previous line.
		    mch_memmove(ptr, conv_rest, conv_restlen);
		    ptr += conv_restlen;
		    اندازه -= conv_restlen;
		}

		إذا (read_بفر)
		{
		    /*
		     * Read bytes from curbuf.  Used for converting text read
		     * from stdin.
		     */
		    إذا (read_buf_lnum > from)
			اندازه = 0;
		    وإلا
		    {
			صحيح	n, ni;
			اوږد	tlen;

			tlen = 0;
			لكل (;;)
			{
			    p = ml_get(read_buf_lnum) + read_buf_ستون;
			    n = ml_get_len(read_buf_lnum) - read_buf_ستون;
			    إذا ((صحيح)tlen + n + 1 > اندازه)
			    {
				// Filled up to "size", append partial line.
				// Change NL to NUL to reverse the effect done
				// below.
				n = (صحيح)(اندازه - tlen);
				لكل (ni = 0; ni < n; ++ni)
				{
				    إذا (p[ni] == NL)
					ptr[tlen++] = NUL;
				    وإلا
					ptr[tlen++] = p[ni];
				}
				read_buf_ستون += n;
				اكسر;
			    }

			    // Append whole line and new-line.  Change NL
			    // to NUL to reverse the effect done below.
			    لكل (ni = 0; ni < n; ++ni)
			    {
				إذا (p[ni] == NL)
				    ptr[tlen++] = NUL;
				وإلا
				    ptr[tlen++] = p[ni];
			    }
			    ptr[tlen++] = NL;
			    read_buf_ستون = 0;
			    إذا (++read_buf_lnum > from)
			    {
				// When the last line didn't have an
				// end-of-line don't add it now either.
				إذا (!curbuf->b_p_eol)
				    --tlen;
				اندازه = tlen;
#که_تعریف_شوی FEAT_CRYPT
				eof = TRUE;
#د_که_پای
				اكسر;
			    }
			}
		    }
		}
		وإلا
		{
		    /*
		     * Read bytes from the file.
		     */
#که_تعریف_شوی FEAT_SODIUM
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
		    إذا (filesize == 0)
		    {
			// set size to 8K + Sodium Crypt Metadata
			اندازه = WRITEBUFSIZE + crypt_get_max_سرلیک_len()
			    + crypto_secretstream_xchacha20poly1305_HEADERBYTES
				+ crypto_secretstream_xchacha20poly1305_ABYTES;
			may_need_lseek = TRUE;
		    }

		    وإلا إذا (filesize > 0 && (curbuf->b_cryptstate != عدم
				&& crypt_لاره_is_sodium(
					     curbuf->b_cryptstate->لاره_nr)))
		    {
			اندازه = WRITEBUFSIZE + crypto_secretstream_xchacha20poly1305_ABYTES;
			// need to rewind by - add_len from CRYPT_M_SOD2 (see
			// description above)
			إذا (curbuf->b_cryptstate->لاره_nr == CRYPT_M_SOD
						     && !eof && may_need_lseek)
			{
			    lseek(fd, crypt_get_سرلیک_len(
					       curbuf->b_cryptstate->لاره_nr)
				       - crypt_get_max_سرلیک_len(), SEEK_CUR);
			    may_need_lseek = FALSE;
			}
		    }
#د_که_پای
		    اوږد read_اندازه = اندازه;
		    اندازه = read_eintr(fd, ptr, read_اندازه);
#که_تعریف_شوی FEAT_CRYPT
		    // Did we reach end of file?
		    filesize_شمېر += اندازه;
		    eof = (اندازه < read_اندازه || filesize_شمېر == filesize_disk);
#د_که_پای
		}

#که_تعریف_شوی FEAT_CRYPT
		/*
		 * At start of file: Check for magic number of encryption.
		 */
		إذا (filesize == 0 && اندازه > 0)
		{
		    cryptkey = check_for_cryptkey(cryptkey, ptr, &اندازه,
						  &filesize, newfile, sfname,
						  &did_ask_for_کلی);
# إذا defined(CRYPT_NOT_INPLACE) && defined(FEAT_PERSISTENT_UNDO)
		    إذا (curbuf->b_cryptstate != عدم
				 && !crypt_کار_کوي_inplace(curbuf->b_cryptstate))
			// reading undo file requires crypt_decode_inplace()
			read_undo_file = FALSE;
# endif
		}
		/*
		 * Decrypt the read bytes.  This is done before checking for
		 * EOF because the crypt layer may be buffering.
		 */
		إذا (cryptkey != عدم && curbuf->b_cryptstate != عدم
								   && اندازه > 0)
		{
# ifdef CRYPT_NOT_INPLACE
		    إذا (crypt_کار_کوي_inplace(curbuf->b_cryptstate))
		    {
# endif
			crypt_decode_inplace(curbuf->b_cryptstate, ptr,
								    اندازه, eof);
# ifdef CRYPT_NOT_INPLACE
		    }
		    وإلا
		    {
			char_u	*newptr = عدم;
			صحيح	decrypted_اندازه;

			decrypted_اندازه = crypt_decode_alloc(
				    curbuf->b_cryptstate, ptr, اندازه,
								 &newptr, eof);

			إذا (decrypted_اندازه < 0)
			{
			    // error message already given
			    error = TRUE;
			    vim_free(newptr);
			    اكسر;
			}
			// If the crypt layer is buffering, not producing
			// anything yet, need to read more.
			إذا (decrypted_اندازه == 0)
			    استمر;

			إذا (linerest == 0)
			{
			    // Simple case: reuse returned buffer (may be
			    // NULL, checked later).
			    new_بفر = newptr;
			}
			وإلا
			{
			    long_u	new_اندازه;

			    // Need new buffer to add bytes carried over.
			    new_اندازه = (long_u)(decrypted_اندازه + linerest + 1);
			    new_بفر = lalloc(new_اندازه, FALSE);
			    إذا (new_بفر == عدم)
			    {
				do_outofmem_پیغام(new_اندازه);
				error = TRUE;
				اكسر;
			    }

			    mch_memmove(new_بفر, بفر, linerest + conv_restlen);
			    إذا (newptr != عدم)
				mch_memmove(new_بفر + linerest + conv_restlen,
					newptr, decrypted_اندازه);
			    vim_free(newptr);
			}

			إذا (new_بفر != عدم)
			{
			    vim_free(بفر);
			    بفر = new_بفر;
			    new_بفر = عدم;
			    کرښه_پیل = بفر;
			    ptr = بفر + linerest + conv_restlen;
			    real_اندازه = اندازه;
			}
			اندازه = decrypted_اندازه;
		    }
# endif
		}
#د_که_پای

		إذا (اندازه <= 0)
		{
		    إذا (اندازه < 0)		    // read error
			error = TRUE;
		    وإلا إذا (conv_restlen > 0)
		    {
			/*
			 * Reached end-of-file but some trailing bytes could
			 * not be converted.  Truncated file?
			 */

			// When we did a conversion report an error.
			إذا (fio_flags != 0
#که_تعریف_شوی USE_ICONV
				|| iconv_fd != (iconv_t)-1
#د_که_پای
			   )
			{
			    إذا (can_retry)
				اذهب rewind_retry;
			    إذا (conv_error == 0)
				conv_error = curbuf->b_ml.ml_کرښه_شمېر
								- linecnt + 1;
			}
			// Remember the first linenr with an illegal byte
			وإلا إذا (illegal_byte == 0)
			    illegal_byte = curbuf->b_ml.ml_کرښه_شمېر
								- linecnt + 1;
			إذا (bad_char_behavior == BAD_DROP)
			{
			    *(ptr - conv_restlen) = NUL;
			    conv_restlen = 0;
			}
			وإلا
			{
			    // Replace the trailing bytes with the replacement
			    // character if we were converting; if we weren't,
			    // leave the UTF8 checking code to do it, as it
			    // works slightly differently.
			    إذا (bad_char_behavior != BAD_KEEP && (fio_flags != 0
#که_تعریف_شوی USE_ICONV
				    || iconv_fd != (iconv_t)-1
#د_که_پای
			       ))
			    {
				طالما (conv_restlen > 0)
				{
				    *(--ptr) = bad_char_behavior;
				    --conv_restlen;
				}
			    }
			    fio_flags = 0;	// don't convert this
#که_تعریف_شوی USE_ICONV
			    إذا (iconv_fd != (iconv_t)-1)
			    {
				iconv_close(iconv_fd);
				iconv_fd = (iconv_t)-1;
			    }
#د_که_پای
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
	    إذا ((filesize == 0
#که_تعریف_شوی FEAT_CRYPT
		   || (cryptkey != عدم
			&& filesize == crypt_get_سرلیک_len(
						 crypt_get_لاره_nr(curbuf)))
#د_که_پای
		       )
		    && (fio_flags == FIO_UCSBOM
			|| (!curbuf->b_p_bomb
			    && tmpname == عدم
			    && (*fenc == 'u' || (*fenc == NUL && enc_utf8)))))
	    {
		char_u	*ccname;
		صحيح	blen;

		// no BOM detection in a short file or in binary mode
		إذا (اندازه < 2 || curbuf->b_p_bin)
		    ccname = عدم;
		وإلا
		    ccname = check_for_bom(ptr, اندازه, &blen,
		      fio_flags == FIO_UCSBOM ? FIO_ALL : get_fio_flags(fenc));
		إذا (ccname != عدم)
		{
		    // Remove BOM from the text
		    filesize += blen;
		    اندازه -= blen;
		    mch_memmove(ptr, ptr + blen, (size_t)اندازه);
		    إذا (سېټ_options)
		    {
			curbuf->b_p_bomb = TRUE;
			curbuf->b_پیل_bomb = TRUE;
		    }
		}

		إذا (fio_flags == FIO_UCSBOM)
		{
		    إذا (ccname == عدم)
		    {
			// No BOM detected: retry with next encoding.
			advance_fenc = TRUE;
		    }
		    وإلا
		    {
			// BOM detected: set "fenc" and jump back
			إذا (fenc_alloced)
			    vim_free(fenc);
			fenc = ccname;
			fenc_alloced = FALSE;
		    }
		    // retry reading without getting new bytes or rewinding
		    skip_read = TRUE;
		    اذهب retry;
		}
	    }

	    // Include not converted bytes.
	    ptr -= conv_restlen;
	    اندازه += conv_restlen;
	    conv_restlen = 0;
	    /*
	     * Break here for a read error or end-of-file.
	     */
	    إذا (اندازه <= 0)
		اكسر;


#که_تعریف_شوی USE_ICONV
	    إذا (iconv_fd != (iconv_t)-1)
	    {
		/*
		 * Attempt conversion of the read bytes to 'encoding' using
		 * iconv().
		 */
		ثابت حرف	*fromp;
		حرف		*سر_2;
		size_t		from_اندازه;
		size_t		منزل_اندازه;

		fromp = (حرف *)ptr;
		from_اندازه = اندازه;
		ptr += اندازه;
		سر_2 = (حرف *)ptr;
		منزل_اندازه = real_اندازه - اندازه;

		/*
		 * If there is conversion error or not enough room try using
		 * another conversion.  Except for when there is no
		 * alternative (help files).
		 */
		طالما ((iconv(iconv_fd, (فراغ *)&fromp, &from_اندازه,
							       &سر_2, &منزل_اندازه)
			    == (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						  || from_اندازه > CONV_RESTLEN)
		{
		    إذا (can_retry)
			اذهب rewind_retry;
		    إذا (conv_error == 0)
			conv_error = readfile_linenr(linecnt,
							  ptr, (char_u *)سر_2);

		    // Deal with a bad byte and continue with the next.
		    ++fromp;
		    --from_اندازه;
		    إذا (bad_char_behavior == BAD_KEEP)
		    {
			*سر_2++ = *(fromp - 1);
			--منزل_اندازه;
		    }
		    وإلا إذا (bad_char_behavior != BAD_DROP)
		    {
			*سر_2++ = bad_char_behavior;
			--منزل_اندازه;
		    }
		}

		إذا (from_اندازه > 0)
		{
		    // Some remaining characters, keep them for the next
		    // round.
		    mch_memmove(conv_rest, (char_u *)fromp, from_اندازه);
		    conv_restlen = (صحيح)from_اندازه;
		}

		// move the linerest to before the converted characters
		کرښه_پیل = ptr - linerest;
		mch_memmove(کرښه_پیل, بفر, (size_t)linerest);
		اندازه = (اوږد)((char_u *)سر_2 - ptr);
	    }
#د_که_پای

#که_تعریف_شوی MSWIN
	    إذا (fio_flags & FIO_CODEPAGE)
	    {
		char_u	*src, *dst;
		WCHAR	ucs2buf[3];
		صحيح	ucs2len;
		صحيح	codepage = FIO_GET_CP(fio_flags);
		صحيح	bytelen;
		صحيح	وموندل_bad;
		حرف	replstr[2];

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
		إذا (bad_char_behavior > 0)
		    replstr[0] = bad_char_behavior;
		وإلا
		    replstr[0] = '?';
		replstr[1] = NUL;

		/*
		 * Move the bytes to the end of the buffer, so that we have
		 * room to put the result at the start.
		 */
		src = ptr + real_اندازه - اندازه;
		mch_memmove(src, ptr, اندازه);

		/*
		 * Do the conversion.
		 */
		dst = ptr;
		طالما (اندازه > 0)
		{
		    وموندل_bad = FALSE;

# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
		    إذا (codepage == CP_UTF8)
		    {
			// Handle CP_UTF8 input ourselves to be able to handle
			// trailing bytes properly.
			// Get one UTF-8 character from src.
			bytelen = (صحيح)utf_ptr2len_len(src, اندازه);
			إذا (bytelen > اندازه)
			{
			    // Only got some bytes of a character.  Normally
			    // it's put in "conv_rest", but if it's too long
			    // deal with it as if they were illegal bytes.
			    إذا (bytelen <= CONV_RESTLEN)
				اكسر;

			    // weird overlong byte sequence
			    bytelen = اندازه;
			    وموندل_bad = TRUE;
			}
			وإلا
			{
			    صحيح	    u8c = utf_ptr2char(src);

			    إذا (u8c > 0xffff || (*src >= 0x80 && bytelen == 1))
				وموندل_bad = TRUE;
			    ucs2buf[0] = u8c;
			    ucs2len = 1;
			}
		    }
		    وإلا
# endif
		    {
			// We don't know how long the byte sequence is, try
			// from one to three bytes.
			لكل (bytelen = 1; bytelen <= اندازه && bytelen <= 3;
								    ++bytelen)
			{
			    ucs2len = Multi_Byte_منزل_Wide_Char(codepage,
							 MB_ERR_INVALID_CHARS,
							 (LPCSTR)src, bytelen,
								   ucs2buf, 3);
			    إذا (ucs2len > 0)
				اكسر;
			}
			إذا (ucs2len == 0)
			{
			    // If we have only one byte then it's probably an
			    // incomplete byte sequence.  Otherwise discard
			    // one byte as a bad character.
			    إذا (اندازه == 1)
				اكسر;
			    وموندل_bad = TRUE;
			    bytelen = 1;
			}
		    }

		    إذا (!وموندل_bad)
		    {
			صحيح	i;

			// Convert "ucs2buf[ucs2len]" to 'enc' in "dst".
			إذا (enc_utf8)
			{
			    // From UCS-2 to UTF-8.  Cannot fail.
			    لكل (i = 0; i < ucs2len; ++i)
				dst += utf_char2bytes(ucs2buf[i], dst);
			}
			وإلا
			{
			    BOOL	bad = FALSE;
			    صحيح		dstlen;

			    // From UCS-2 to "enc_codepage".  If the
			    // conversion uses the default character "?",
			    // the data doesn't fit in this encoding.
			    dstlen = Wide_Char_منزل_Multi_Byte(enc_codepage, 0,
				    (LPCWSTR)ucs2buf, ucs2len,
				    (LPSTR)dst, (صحيح)(src - dst),
				    replstr, &bad);
			    إذا (bad)
				وموندل_bad = TRUE;
			    وإلا
				dst += dstlen;
			}
		    }

		    إذا (وموندل_bad)
		    {
			// Deal with bytes we can't convert.
			إذا (can_retry)
			    اذهب rewind_retry;
			إذا (conv_error == 0)
			    conv_error = readfile_linenr(linecnt, ptr, dst);
			إذا (bad_char_behavior != BAD_DROP)
			{
			    إذا (bad_char_behavior == BAD_KEEP)
			    {
				mch_memmove(dst, src, bytelen);
				dst += bytelen;
			    }
			    وإلا
				*dst++ = bad_char_behavior;
			}
		    }

		    src += bytelen;
		    اندازه -= bytelen;
		}

		إذا (اندازه > 0)
		{
		    // An incomplete byte sequence remaining.
		    mch_memmove(conv_rest, src, اندازه);
		    conv_restlen = اندازه;
		}

		// The new size is equal to how much "dst" was advanced.
		اندازه = (اوږد)(dst - ptr);
	    }
	    وإلا
#د_که_پای
#که_تعریف_شوی MACOS_CONVERT
	    إذا (fio_flags & FIO_MACROMAN)
	    {
		/*
		 * Conversion from Apple MacRoman char encoding to UTF-8 or
		 * latin1.  This is in os_mac_conv.c.
		 */
		إذا (macroman2enc(ptr, &اندازه, real_اندازه) == FAIL)
		    اذهب rewind_retry;
	    }
	    وإلا
#د_که_پای
	    إذا (fio_flags != 0)
	    {
		صحيح	u8c;
		char_u	*dest;
		char_u	*لکۍ = عدم;

		/*
		 * "enc_utf8" set: Convert Unicode or Latin1 to UTF-8.
		 * "enc_utf8" not set: Convert Unicode to Latin1.
		 * Go from end to start through the buffer, because the number
		 * of bytes may increase.
		 * "dest" points to after where the UTF-8 bytes go, "p" points
		 * to after the next character to convert.
		 */
		dest = ptr + real_اندازه;
		إذا (fio_flags == FIO_LATIN1 || fio_flags == FIO_UTF8)
		{
		    p = ptr + اندازه;
		    إذا (fio_flags == FIO_UTF8)
		    {
			// Check for a trailing incomplete UTF-8 sequence
			لکۍ = ptr + اندازه - 1;
			طالما (لکۍ > ptr && (*لکۍ & 0xc0) == 0x80)
			    --لکۍ;
			إذا (لکۍ + utf_byte2len(*لکۍ) <= ptr + اندازه)
			    لکۍ = عدم;
			وإلا
			    p = لکۍ;
		    }
		}
		وإلا إذا (fio_flags & (FIO_UCS2 | FIO_UTF16))
		{
		    // Check for a trailing byte
		    p = ptr + (اندازه & ~1);
		    إذا (اندازه & 1)
			لکۍ = p;
		    إذا ((fio_flags & FIO_UTF16) && p > ptr)
		    {
			// Check for a trailing leading word
			إذا (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			وإلا
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			إذا (u8c >= 0xd800 && u8c <= 0xdbff)
			    لکۍ = p;
			وإلا
			    p += 2;
		    }
		}
		وإلا //  FIO_UCS4
		{
		    // Check for trailing 1, 2 or 3 bytes
		    p = ptr + (اندازه & ~3);
		    إذا (اندازه & 3)
			لکۍ = p;
		}

		// If there is a trailing incomplete sequence move it to
		// conv_rest[].
		إذا (لکۍ != عدم)
		{
		    conv_restlen = (صحيح)((ptr + اندازه) - لکۍ);
		    mch_memmove(conv_rest, (char_u *)لکۍ, conv_restlen);
		    اندازه -= conv_restlen;
		}


		طالما (p > ptr)
		{
		    إذا (fio_flags & FIO_LATIN1)
			u8c = *--p;
		    وإلا إذا (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    {
			إذا (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			وإلا
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			إذا ((fio_flags & FIO_UTF16)
					    && u8c >= 0xdc00 && u8c <= 0xdfff)
			{
			    صحيح u16c;

			    إذا (p == ptr)
			    {
				// Missing leading word.
				إذا (can_retry)
				    اذهب rewind_retry;
				إذا (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				إذا (bad_char_behavior == BAD_DROP)
				    استمر;
				إذا (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }

			    // found second word of double-word, get the first
			    // word and compute the resulting character
			    إذا (fio_flags & FIO_ENDIAN_L)
			    {
				u16c = (*--p << 8);
				u16c += *--p;
			    }
			    وإلا
			    {
				u16c = *--p;
				u16c += (*--p << 8);
			    }
			    u8c = 0x10000 + ((u16c & 0x3ff) << 10)
							      + (u8c & 0x3ff);

			    // Check if the word is indeed a leading word.
			    إذا (u16c < 0xd800 || u16c > 0xdbff)
			    {
				إذا (can_retry)
				    اذهب rewind_retry;
				إذا (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				إذا (bad_char_behavior == BAD_DROP)
				    استمر;
				إذا (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    وإلا إذا (fio_flags & FIO_UCS4)
		    {
			إذا (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (بینښه)*--p << 24;
			    u8c += (بینښه)*--p << 16;
			    u8c += (بینښه)*--p << 8;
			    u8c += *--p;
			}
			وإلا	// big endian
			{
			    u8c = *--p;
			    u8c += (بینښه)*--p << 8;
			    u8c += (بینښه)*--p << 16;
			    u8c += (بینښه)*--p << 24;
			}
		    }
		    وإلا    // UTF-8
		    {
			إذا (*--p < 0x80)
			    u8c = *p;
			وإلا
			{
			    len = utf_سر_off(ptr, p);
			    p -= len;
			    u8c = utf_ptr2char(p);
			    إذا (len == 0)
			    {
				// Not a valid UTF-8 character, retry with
				// another fenc when possible, otherwise just
				// report the error.
				إذا (can_retry)
				    اذهب rewind_retry;
				إذا (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				إذا (bad_char_behavior == BAD_DROP)
				    استمر;
				إذا (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    إذا (enc_utf8)	// produce UTF-8
		    {
			dest -= utf_char2len(u8c);
			(فراغ)utf_char2bytes(u8c, dest);
		    }
		    وإلا		// produce Latin1
		    {
			--dest;
			إذا (u8c >= 0x100)
			{
			    // character doesn't fit in latin1, retry with
			    // another fenc when possible, otherwise just
			    // report the error.
			    إذا (can_retry)
				اذهب rewind_retry;
			    إذا (conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
			    إذا (bad_char_behavior == BAD_DROP)
				++dest;
			    وإلا إذا (bad_char_behavior == BAD_KEEP)
				*dest = u8c;
			    وإلا إذا (eap != عدم && eap->bad_char != 0)
				*dest = bad_char_behavior;
			    وإلا
				*dest = 0xBF;
			}
			وإلا
			    *dest = u8c;
		    }
		}

		// move the linerest to before the converted characters
		کرښه_پیل = dest - linerest;
		mch_memmove(کرښه_پیل, بفر, (size_t)linerest);
		اندازه = (اوږد)((ptr + real_اندازه) - dest);
		ptr = dest;
	    }
	    وإلا إذا (enc_utf8 && !curbuf->b_p_bin)
	    {
		صحيح  incomplete_لکۍ = FALSE;

		// Reading UTF-8: Check if the bytes are valid UTF-8.
		لكل (p = ptr; ; )
		{
		    صحيح	 todo;
		    صحيح	 l;

		    // Skip ASCII bytes quickly using word-at-a-time check.
		    {
			char_u *ascii_پای_ټکی = ptr + اندازه;
			طالما (ascii_پای_ټکی - p >= (اوږد)حجم(long_u))
			{
			    long_u کلمه;
			    حافظه_نقل(&کلمه, p, حجم(long_u));
			    إذا (کلمه & NONASCII_ماسک)
				اكسر;
			    p += حجم(long_u);
			}
			طالما (p < ascii_پای_ټکی && *p < 0x80)
			    ++p;
		    }

		    todo = (صحيح)((ptr + اندازه) - p);
		    إذا (todo <= 0)
			اكسر;
		    إذا (*p >= 0x80)
		    {
			// A length of 1 means it's an illegal byte.  Accept
			// an incomplete character at the end though, the next
			// read() will get the next bytes, we'll check it
			// then.
			l = utf_ptr2len_len(p, todo);
			إذا (l > todo && !incomplete_لکۍ)
			{
			    // Avoid retrying with a different encoding when
			    // a truncated file is more likely, or attempting
			    // to read the rest of an incomplete sequence when
			    // we have already done so.
			    إذا (p > ptr || filesize > 0)
				incomplete_لکۍ = TRUE;
			    // Incomplete byte sequence, move it to conv_rest[]
			    // and try to read the rest of it, unless we've
			    // already done so.
			    إذا (p > ptr)
			    {
				conv_restlen = todo;
				mch_memmove(conv_rest, p, conv_restlen);
				اندازه -= conv_restlen;
				اكسر;
			    }
			}
			إذا (l == 1 || l > todo)
			{
			    // Illegal byte.  If we can try another encoding
			    // do that, unless at EOF where a truncated
			    // file is more likely than a conversion error.
			    إذا (can_retry && !incomplete_لکۍ)
				اكسر;
#که_تعریف_شوی USE_ICONV
			    // When we did a conversion report an error.
			    إذا (iconv_fd != (iconv_t)-1 && conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
#د_که_پای
			    // Remember the first linenr with an illegal byte
			    إذا (conv_error == 0 && illegal_byte == 0)
				illegal_byte = readfile_linenr(linecnt, ptr, p);

			    // Drop, keep or replace the bad byte.
			    إذا (bad_char_behavior == BAD_DROP)
			    {
				mch_memmove(p, p + 1, todo - 1);
				--اندازه;
			    }
			    وإلا
			    {
				إذا (bad_char_behavior != BAD_KEEP)
				    *p = bad_char_behavior;
				++p;
			    }
			}
			وإلا
			    p += l;
		    }
		}
		إذا (p < ptr + اندازه && !incomplete_لکۍ)
		{
		    // Detected a UTF-8 error.
rewind_retry:
		    // Retry reading with another conversion.
#إذا defined(FEAT_EVAL) && defined(USE_ICONV)
		    إذا (*p_ccv != NUL && iconv_fd != (iconv_t)-1)
			// iconv() failed, try 'charconvert'
			did_iconv = TRUE;
		    وإلا
#د_که_پای
			// use next item from 'fileencodings'
			advance_fenc = TRUE;
		    file_rewind = TRUE;
		    اذهب retry;
		}
	    }

	    // count the number of characters (after conversion!)
	    filesize += اندازه;

	    /*
	     * when reading the first part of a file: guess EOL type
	     */
	    إذا (fileformat == EOL_UNKNOWN)
	    {
		// First try finding a NL, for Dos and Unix
		إذا (try_dos || try_unix)
		{
		    // Reset the carriage return counter.
		    إذا (try_mac)
			try_mac = 1;

		    لكل (p = ptr; p < ptr + اندازه; ++p)
		    {
			إذا (*p == NL)
			{
			    إذا (!try_unix
				    || (try_dos && p > ptr && p[-1] == CAR))
				fileformat = EOL_DOS;
			    وإلا
				fileformat = EOL_UNIX;
			    اكسر;
			}
			وإلا إذا (*p == CAR && try_mac)
			    try_mac++;
		    }

		    // Don't give in to EOL_UNIX if EOL_MAC is more likely
		    إذا (fileformat == EOL_UNIX && try_mac)
		    {
			// Need to reset the counters when retrying fenc.
			try_mac = 1;
			try_unix = 1;
			لكل (; p >= ptr && *p != CAR; p--)
			    ;
			إذا (p >= ptr)
			{
			    لكل (p = ptr; p < ptr + اندازه; ++p)
			    {
				إذا (*p == NL)
				    try_unix++;
				وإلا إذا (*p == CAR)
				    try_mac++;
			    }
			    إذا (try_mac > try_unix)
				fileformat = EOL_MAC;
			}
		    }
		    وإلا إذا (fileformat == EOL_UNKNOWN && try_mac == 1)
			// Looking for CR but found no end-of-line markers at
			// all: use the default format.
			fileformat = default_fileformat();
		}

		// No NL found: may use Mac format
		إذا (fileformat == EOL_UNKNOWN && try_mac)
		    fileformat = EOL_MAC;

		// Still nothing found?  Use first format in 'ffs'
		إذا (fileformat == EOL_UNKNOWN)
		    fileformat = default_fileformat();

		// if editing a new file: may set p_tx and p_ff
		إذا (سېټ_options)
		    سېټ_fileformat(fileformat, OPT_LOCAL);
	    }
	}

	/*
	 * This loop is executed once for every character read.
	 * Keep it fast!
	 */
	إذا (fileformat == EOL_MAC)
	{
	    --ptr;
	    طالما (++ptr, --اندازه >= 0)
	    {
		// catch most common case first
		إذا ((c = *ptr) != NUL && c != CAR && c != NL)
		    استمر;
		إذا (c == NUL)
		    *ptr = NL;	// NULs are replaced by newlines!
		وإلا إذا (c == NL)
		    *ptr = CAR;	// NLs are replaced by CRs!
		وإلا
		{
		    إذا (skip_شمېر == 0)
		    {
			*ptr = NUL;	    // end of line
			len = (colnr_T) (ptr - کرښه_پیل + 1);
			إذا (ml_append(lnum, کرښه_پیل, len, newfile) == FAIL)
			{
			    error = TRUE;
			    اكسر;
			}
#که_تعریف_شوی FEAT_PERSISTENT_UNDO
			إذا (read_undo_file)
			    sha256_update(&sha_ctx, کرښه_پیل, len);
#د_که_پای
			++lnum;
			إذا (--read_شمېر == 0)
			{
			    error = TRUE;	// break loop
			    کرښه_پیل = ptr;	// nothing left to write
			    اكسر;
			}
		    }
		    وإلا
			--skip_شمېر;
		    کرښه_پیل = ptr + 1;
		}
	    }
	}
	وإلا
	{
	    // Use memchr() for SIMD-optimized newline scanning instead
	    // of scanning each byte individually.
	    char_u *پای_ټکی = ptr + اندازه;

	    طالما (ptr < پای_ټکی)
	    {
		char_u *nl = (char_u *)memchr(ptr, NL, پای_ټکی - ptr);
		char_u *nul_لټون;

		إذا (nl == عدم)
		{
		    // No more newlines in buffer.
		    // Replace any NUL bytes with NL in remaining data.
		    طالما ((nul_لټون = (char_u *)memchr(ptr, NUL,
						      پای_ټکی - ptr)) != عدم)
		    {
			*nul_لټون = NL;
			ptr = nul_لټون + 1;
		    }
		    ptr = پای_ټکی;
		    اكسر;
		}

		// Replace NUL bytes with NL before the newline.
		{
		    char_u *لټون = ptr;
		    طالما ((nul_لټون = (char_u *)memchr(لټون, NUL,
						       nl - لټون)) != عدم)
		    {
			*nul_لټون = NL;
			لټون = nul_لټون + 1;
		    }
		}

		// Process the newline.
		ptr = nl;
		إذا (skip_شمېر == 0)
		{
		    *ptr = NUL;		// end of line
		    len = (colnr_T)(ptr - کرښه_پیل + 1);
		    إذا (fileformat == EOL_DOS)
		    {
			إذا (ptr > کرښه_پیل && ptr[-1] == CAR)
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
			وإلا إذا (ff_error != EOL_DOS)
			{
			    إذا (   try_unix
				&& !read_stdin
				&& (read_بفر
				    || vim_lseek(fd, (off_T)0L, SEEK_SET)
								      == 0))
			    {
				fileformat = EOL_UNIX;
				إذا (سېټ_options)
				    سېټ_fileformat(EOL_UNIX, OPT_LOCAL);
				file_rewind = TRUE;
				keep_fileformat = TRUE;
				اذهب retry;
			    }
			    ff_error = EOL_DOS;
			}
		    }
		    إذا (ml_append(lnum, کرښه_پیل, len, newfile) == FAIL)
		    {
			error = TRUE;
			اكسر;
		    }
#که_تعریف_شوی FEAT_PERSISTENT_UNDO
		    إذا (read_undo_file)
			sha256_update(&sha_ctx, کرښه_پیل, len);
#د_که_پای
		    ++lnum;
		    إذا (--read_شمېر == 0)
		    {
			error = TRUE;	    // break loop
			کرښه_پیل = ptr;   // nothing left to write
			اكسر;
		    }
		}
		وإلا
		    --skip_شمېر;
		کرښه_پیل = ptr + 1;
		++ptr;
	    }
	    اندازه = -1;
	}
	linerest = (اوږد)(ptr - کرښه_پیل);
	ui_breakcheck();
    }

failed:
    // not an error, max. number of lines reached
    إذا (error && read_شمېر == 0)
	error = FALSE;

    // In Dos format ignore a trailing CTRL-Z, unless 'binary' is set.
    // In old days the file length was in sector count and the CTRL-Z the
    // marker where the file really ended.  Assuming we write it to a file
    // system that keeps file length properly the CTRL-Z should be dropped.
    // Set the 'endoffile' option so the user can decide what to write later.
    // In Unix format the CTRL-Z is just another character.
    إذا (linerest != 0
	    && !curbuf->b_p_bin
	    && fileformat == EOL_DOS
	    && ptr[-1] == Ctrl_Z)
    {
	ptr--;
	linerest--;
	إذا (سېټ_options)
	    curbuf->b_p_eof = TRUE;
    }

    // If we get EOF in the middle of a line, note the fact by resetting
    // 'endofline' and add the line normally.
    إذا (!error
	    && !got_int
	    && linerest != 0)
    {
	// remember for when writing
	إذا (سېټ_options)
	    curbuf->b_p_eol = FALSE;
	*ptr = NUL;
	len = (colnr_T)(ptr - کرښه_پیل + 1);
	إذا (ml_append(lnum, کرښه_پیل, len, newfile) == FAIL)
	    error = TRUE;
	وإلا
	{
#که_تعریف_شوی FEAT_PERSISTENT_UNDO
	    إذا (read_undo_file)
		sha256_update(&sha_ctx, کرښه_پیل, len);
#د_که_پای
	    read_no_eol_lnum = ++lnum;
	}
    }

    إذا (سېټ_options)
	save_file_ff(curbuf);		// remember the current file format

#که_تعریف_شوی FEAT_CRYPT
    إذا (curbuf->b_cryptstate != عدم)
    {
	crypt_free_state(curbuf->b_cryptstate);
	curbuf->b_cryptstate = عدم;
    }
    إذا (cryptkey != عدم && cryptkey != curbuf->b_p_کلی)
	crypt_free_کلی(cryptkey);
    // Don't set cryptkey to NULL, it's used below as a flag that
    // encryption was used.
#د_که_پای

    // If editing a new file: set 'fenc' for the current buffer.
    // Also for ":read ++edit file".
    إذا (سېټ_options)
	سېټ_string_option_مستقیم((char_u *)"fenc", -1, fenc,
						       OPT_FREE|OPT_LOCAL, 0);
    إذا (fenc_alloced)
	vim_free(fenc);
#که_تعریف_شوی USE_ICONV
    إذا (iconv_fd != (iconv_t)-1)
	iconv_close(iconv_fd);
#د_که_پای

    إذا (!read_بفر && !read_stdin)
	close(fd);				// errors are ignored
#که_تعریف_شوی HAVE_FD_CLOEXEC
    وإلا
    {
	صحيح fdflags = fcntl(fd, F_GETFD);

	إذا (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (فراغ)fcntl(fd, F_SETFD, fdflags | FD_CLOEXEC);
    }
#د_که_پای
    vim_free(بفر);

#که_تعریف_شوی HAVE_DUP
    إذا (read_stdin)
    {
	// Use stderr for stdin, makes shell commands work.
	close(0);
	vim_ignored = dup(2);
    }
#د_که_پای

    إذا (tmpname != عدم)
    {
	mch_remove(tmpname);		// delete converted file
	vim_free(tmpname);
    }
    --no_wait_return;			// may wait for return now

    /*
     * In recovery mode everything but autocommands is skipped.
     */
    إذا (!recoverymode)
    {
	// need to delete the last line, which comes from the empty buffer
	إذا (newfile && wasempty && !(curbuf->b_ml.ml_flags & ML_تشه))
	{
#که_تعریف_شوی FEAT_NETBEANS_INTG
	    netbeansFireChanges = 0;
#د_که_پای
	    ml_delete(curbuf->b_ml.ml_کرښه_شمېر);
#که_تعریف_شوی FEAT_NETBEANS_INTG
	    netbeansFireChanges = 1;
#د_که_پای
	    --linecnt;
	}
	linecnt = curbuf->b_ml.ml_کرښه_شمېر - linecnt;
	إذا (filesize == 0)
	    linecnt = 0;
	إذا (newfile || read_بفر)
	{
	    redraw_curbuf_later(UPD_NOT_VALID);
#که_تعریف_شوی FEAT_DIFF
	    // After reading the text into the buffer the diff info needs to
	    // be updated.
	    diff_invalidate(curbuf);
#د_که_پای
#که_تعریف_شوی FEAT_FOLDING
	    // All folds in the window are invalid now.  Mark them for update
	    // before triggering autocommands.
	    foldUpdateAll(curwin);
#د_که_پای
	}
	وإلا إذا (linecnt)		// appended at least one line
	    appended_کرښې_mark(from, linecnt);

#که_نه_تعریف_شوی ALWAYS_USE_GUI
	/*
	 * If we were reading from the same terminal as where messages go,
	 * the screen will have been messed up.
	 * Switch on raw mode now and clear the screen.
	 */
	إذا (read_stdin)
	{
	    settmode(TMODE_RAW);	// set to raw mode
	    starttermcap();
	    screenclear();
	}
#د_که_پای

	إذا (got_int)
	{
	    إذا (!(flags & READ_DUMMY))
	    {
		filemess(curbuf, sfname, (char_u *)_(e_interrupted), 0);
		إذا (newfile)
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    }
	    پیغام_scroll = پیغام_save;
#که_تعریف_شوی FEAT_VIMINFO
	    check_marks_read();
#د_که_پای
	    retval = OK;	// an interrupt isn't really an error
	    اذهب theend;
	}

	إذا (!filtering && !(flags & READ_DUMMY))
	{
	    صحيح	buflen;

	    پیغام_جمع_کړه_fname(curbuf, sfname);   // fname in IObuff with quotes
	    c = FALSE;

	    buflen = (صحيح)STRLEN(IObuff);
#که_تعریف_شوی UNIX
	    إذا (S_ISFIFO(perm))			    // fifo
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[fifo]"));
		c = TRUE;
	    }
	    إذا (S_ISSOCK(perm))			    // or socket
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[socket]"));
		c = TRUE;
	    }
# ifdef OPEN_CHR_FILES
	    إذا (S_ISCHR(perm))			    // or character special
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[character special]"));
		c = TRUE;
	    }
# endif
#د_که_پای
	    إذا (curbuf->b_p_ro)
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			"%s", shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
		c = TRUE;
	    }
	    إذا (read_no_eol_lnum)
	    {
		پیغام_جمع_کړه_eol();
		c = TRUE;
	    }
	    إذا (ff_error == EOL_DOS)
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[CR missing]"));
		c = TRUE;
	    }
	    إذا (وېشل)
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[long lines split]"));
		c = TRUE;
	    }
	    إذا (notconverted)
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[NOT converted]"));
		c = TRUE;
	    }
	    وإلا إذا (converted)
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[converted]"));
		c = TRUE;
	    }
#که_تعریف_شوی FEAT_CRYPT
	    إذا (cryptkey != عدم)
	    {
		crypt_append_پیغام(curbuf);
		c = TRUE;
	    }
#د_که_پای
	    إذا (conv_error != 0)
	    {
		vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[CONVERSION ERROR in line %ld]"), (اوږد)conv_error);
		c = TRUE;
	    }
	    وإلا إذا (illegal_byte > 0)
	    {
		vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[ILLEGAL BYTE in line %ld]"), (اوږد)illegal_byte);
		c = TRUE;
	    }
	    وإلا إذا (error)
	    {
		vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[READ ERRORS]"));
		c = TRUE;
	    }
	    إذا (پیغام_جمع_کړه_fileformat(fileformat))
		c = TRUE;
#که_تعریف_شوی FEAT_CRYPT
	    إذا (cryptkey != عدم)
		پیغام_جمع_کړه_کرښې(c, (اوږد)linecnt, filesize
			 - crypt_get_سرلیک_len(crypt_get_لاره_nr(curbuf)));
	    وإلا
#د_که_پای
		پیغام_جمع_کړه_کرښې(c, (اوږد)linecnt, filesize);

	    VIM_CLEAR(keep_پیغام);
	    پیغام_scrolled_ign = TRUE;
#که_تعریف_شوی ALWAYS_USE_GUI
	    // Don't show the message when reading stdin, it would end up in a
	    // message box (which might be shown when exiting!)
	    إذا (read_stdin || read_بفر)
		p = پیغام_may_trunc(FALSE, IObuff);
	    وإلا
#د_که_پای
	    {
		إذا (پیغام_ستون > 0)
		    پیغام_putchar('\r');  // overwrite previous message
		p = (char_u *)پیغام_trunc_attr((حرف *)IObuff, FALSE, 0);
	    }
	    إذا (read_stdin || read_بفر || restart_edit != 0
		    || (پیغام_scrolled != 0 && !need_wait_return))
		// Need to repeat the message after redrawing when:
		// - When reading from stdin (the screen will be cleared next).
		// - When restart_edit is set (otherwise there will be a delay
		//   before redrawing).
		// - When the screen was scrolled but there is no wait-return
		//   prompt.
		سېټ_keep_پیغام(p, 0);
	    پیغام_scrolled_ign = FALSE;
	}

	// with errors writing the file requires ":w!"
	إذا (newfile && (error
		    || conv_error != 0
		    || (illegal_byte > 0 && bad_char_behavior != BAD_KEEP)))
	    curbuf->b_p_ro = TRUE;

	u_clearline();	    // cannot use "U" command after adding lines

	/*
	 * In Ex mode: cursor at last new line.
	 * Otherwise: cursor at first new line.
	 */
	إذا (exmode_فعال)
	    curwin->w_cursor.lnum = from + linecnt;
	وإلا
	    curwin->w_cursor.lnum = from + 1;
	check_cursor_lnum();
	beginline(BL_WHITE | BL_FIX);	    // on first non-blank

	إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set '[ and '] marks to the newly read lines.
	    curbuf->b_op_پیل.lnum = from + 1;
	    curbuf->b_op_پیل.ستون = 0;
	    curbuf->b_op_پای_ټکی.lnum = from + linecnt;
	    curbuf->b_op_پای_ټکی.ستون = 0;
	}

#که_تعریف_شوی MSWIN
	/*
	 * Work around a weird problem: When a file has two links (only
	 * possible on NTFS) and we write through one link, then stat() it
	 * through the other link, the timestamp information may be wrong.
	 * It's correct again after reading the file, thus reset the timestamp
	 * here.
	 */
	إذا (newfile && !read_stdin && !read_بفر
					 && mch_stat((حرف *)fname, &st) >= 0)
	{
	    buf_store_وخت(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
	}
#د_که_پای
    }
    پیغام_scroll = پیغام_save;

#که_تعریف_شوی FEAT_VIMINFO
    /*
     * Get the marks before executing autocommands, so they can be used there.
     */
    check_marks_read();
#د_که_پای

    /*
     * We remember if the last line of the read didn't have
     * an eol even when 'binary' is off, to support turning 'fixeol' off,
     * or writing the read again with 'binary' on.  The latter is required
     * for ":autocmd FileReadPost *.gz set bin|'[,']!gunzip" to work.
     */
    curbuf->b_no_eol_lnum = read_no_eol_lnum;

    // When reloading a buffer put the cursor at the first line that is
    // different.
    إذا (flags & READ_KEEP_UNDO)
	u_پیدا_کړه_لومړی_بدل_شوی();

#که_تعریف_شوی FEAT_PERSISTENT_UNDO
    /*
     * When opening a new file locate undo info and read it.
     */
    إذا (read_undo_file)
    {
	char_u	hash[UNDO_HASH_اندازه];

	sha256_پای(&sha_ctx, hash);
	u_read_undo(عدم, hash, fname);
    }
#د_که_پای

    إذا (!read_stdin && !read_fifo && (!read_بفر || sfname != عدم))
    {
	صحيح m = پیغام_scroll;
	صحيح n = پیغام_scrolled;

	// Save the fileformat now, otherwise the buffer will be considered
	// modified if the format/encoding was automatically detected.
	إذا (سېټ_options)
	    save_file_ff(curbuf);

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	پیغام_scroll = TRUE;
	إذا (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPOST, عدم, sfname,
							  FALSE, curbuf, eap);
	وإلا إذا (newfile || (read_بفر && sfname != عدم))
	{
	    apply_autocmds_exarg(EVENT_BUFREADPOST, عدم, sfname,
							  FALSE, curbuf, eap);
	    إذا (!curbuf->b_au_did_filetype && *curbuf->b_p_ft != NUL)
		/*
		 * EVENT_FILETYPE was not triggered but the buffer already has a
		 * filetype. Trigger EVENT_FILETYPE using the existing filetype.
		 */
		apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
			TRUE, curbuf);
	}
	وإلا
	    apply_autocmds_exarg(EVENT_FILEREADPOST, sfname, sfname,
							    FALSE, عدم, eap);
	إذا (پیغام_scrolled == n)
	    پیغام_scroll = m;
#که_تعریف_شوی FEAT_EVAL
	إذا (aborting())	    // autocmds may abort script processing
	    اذهب theend;
#د_که_پای
    }

    إذا (!(recoverymode && error))
	retval = OK;

theend:
    إذا (curbuf->b_ml.ml_mfp != عدم
		       && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	// OK to sync the swap file now
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    أرجع retval;
}

#إذا defined(OPEN_CHR_FILES)
/*
 * Returns TRUE if the file name argument is of the form "/dev/fd/\d\+",
 * which is the name of files used for process substitution output by
 * some shells on some operating systems, e.g., bash on SunOS.
 * Do not accept "/dev/fd/[012]", opening these may hang Vim.
 */
    صحيح
is_dev_fd_file(char_u *fname)
{
    أرجع (STRNCMP(fname, "/dev/fd/", 8) == 0
	    && VIM_ISDIGIT(fname[8])
	    && *skipdigits(fname + 9) == NUL
	    && (fname[9] != NUL
		|| (fname[8] != '0' && fname[8] != '1' && fname[8] != '2')));
}
#د_که_پای

/*
 * From the current line count and characters read after that, estimate the
 * line number where we are now.
 * Used for error messages that include a line number.
 */
    ساكن linenr_T
readfile_linenr(
    linenr_T	linecnt,	// line count before reading more bytes
    char_u	*p,		// start of more bytes read
    char_u	*endp)		// end of more bytes read
{
    char_u	*s;
    linenr_T	lnum;

    lnum = curbuf->b_ml.ml_کرښه_شمېر - linecnt + 1;
    لكل (s = p; s < endp; ++s)
	إذا (*s == '\n')
	    ++lnum;
    أرجع lnum;
}

/*
 * Fill "*eap" to force the 'fileencoding', 'fileformat' and 'binary' to be
 * equal to the buffer "buf".  Used for calling readfile().
 * Returns OK or FAIL.
 */
    صحيح
prep_exarg(exarg_T *eap, buf_T *buf)
{
    eap->cmd = alloc(15 + (بینښه)STRLEN(buf->b_p_fenc));
    إذا (eap->cmd == عدم)
	أرجع FAIL;

    sprintf((حرف *)eap->cmd, "e ++enc=%s", buf->b_p_fenc);
    eap->force_enc = 8;
    eap->bad_char = buf->b_bad_char;
    eap->force_ff = *buf->b_p_ff;

    eap->force_bin = buf->b_p_bin ? FORCE_BIN : FORCE_NOBIN;
    eap->read_edit = FALSE;
    eap->forceit = FALSE;
    أرجع OK;
}

/*
 * Set default or forced 'fileformat' and 'binary'.
 */
    فراغ
سېټ_file_options(صحيح سېټ_options, exarg_T *eap)
{
    // set default 'fileformat'
    إذا (سېټ_options)
    {
	إذا (eap != عدم && eap->force_ff != 0)
	    سېټ_fileformat(get_fileformat_force(curbuf, eap), OPT_LOCAL);
	وإلا إذا (*p_ffs != NUL)
	    سېټ_fileformat(default_fileformat(), OPT_LOCAL);
    }

    // set or reset 'binary'
    إذا (eap != عدم && eap->force_bin != 0)
    {
	صحيح	oldval = curbuf->b_p_bin;

	curbuf->b_p_bin = (eap->force_bin == FORCE_BIN);
	سېټ_options_bin(oldval, curbuf->b_p_bin, OPT_LOCAL);
    }
}

/*
 * Set forced 'fileencoding'.
 */
    فراغ
سېټ_forced_fenc(exarg_T *eap)
{
    إذا (eap->force_enc == 0)
	أرجع;

    char_u *fenc = enc_canonize(eap->cmd + eap->force_enc);

    إذا (fenc != عدم)
	سېټ_string_option_مستقیم((char_u *)"fenc", -1,
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
    ساكن char_u *
بل_fenc(char_u **pp, صحيح *alloced)
{
    char_u	*p;
    char_u	*r;

    *alloced = FALSE;
    إذا (**pp == NUL)
    {
	*pp = عدم;
	أرجع (char_u *)"";
    }
    p = vim_strchr(*pp, ',');
    إذا (p == عدم)
    {
	r = enc_canonize(*pp);
	*pp += STRLEN(*pp);
    }
    وإلا
    {
	r = vim_strnsave(*pp, p - *pp);
	*pp = p + 1;
	إذا (r != عدم)
	{
	    p = enc_canonize(r);
	    vim_free(r);
	    r = p;
	}
    }
    إذا (r != عدم)
	*alloced = TRUE;
    وإلا
    {
	// out of memory
	r = (char_u *)"";
	*pp = عدم;
    }
    أرجع r;
}

#که_تعریف_شوی FEAT_EVAL
/*
 * Convert a file with the 'charconvert' expression.
 * This closes the file which is to be read, converts it and opens the
 * resulting file for reading.
 * Returns name of the resulting converted file (the caller should delete it
 * after reading it).
 * Returns NULL if the conversion failed ("*fdp" is not set) .
 */
    ساكن char_u *
readfile_charconvert(
    char_u	*fname,		// name of input file
    char_u	*fenc,		// converted from
    صحيح		*fdp)		// in/out: file descriptor of file
{
    char_u	*tmpname;
    حرف	*errmsg = عدم;

    tmpname = vim_tempname('r', FALSE);
    إذا (tmpname == عدم)
	errmsg = _("Can't find temp file for conversion");
    وإلا
    {
	close(*fdp);		// close the input file, ignore errors
	*fdp = -1;
	إذا (eval_charconvert(fenc, enc_utf8 ? (char_u *)"utf-8" : p_enc,
						      fname, tmpname) == FAIL)
	    errmsg = _("Conversion with 'charconvert' failed");
	إذا (errmsg == عدم && (*fdp = mch_open((حرف *)tmpname,
						  O_RDONLY | O_EXTRA, 0)) < 0)
	    errmsg = _("can't read output of 'charconvert'");
    }

    إذا (errmsg != عدم)
    {
	// Don't use emsg(), it breaks mappings, the retry with
	// another type of conversion might still work.
	پیغام(errmsg);
	إذا (tmpname != عدم)
	{
	    mch_remove(tmpname);	// delete converted file
	    VIM_CLEAR(tmpname);
	}
    }

    // If the input file is closed, open it (caller should check for error).
    إذا (*fdp < 0)
	*fdp = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0);

    أرجع tmpname;
}
#د_که_پای

#إذا defined(FEAT_CRYPT)
/*
 * Check for magic number used for encryption.  Applies to the current buffer.
 * If found, the magic number is removed from ptr[*sizep] and *sizep and
 * *filesizep are updated.
 * Return the (new) encryption key, NULL for no encryption.
 */
    ساكن char_u *
check_for_cryptkey(
    char_u	*cryptkey,	// previous encryption key or NULL
    char_u	*ptr,		// pointer to read bytes
    اوږد	*sizep,		// length of read bytes
    off_T	*filesizep,	// nr of bytes used from file
    صحيح		newfile,	// editing a new buffer
    char_u	*fname,		// file name to display
    صحيح		*did_ask)	// flag: whether already asked for key
{
    صحيح لاره = crypt_لاره_nr_from_magic((حرف *)ptr, *sizep);
    صحيح b_p_ro = curbuf->b_p_ro;

    إذا (لاره >= 0)
    {
	// Mark the buffer as read-only until the decryption has taken place.
	// Avoids accidentally overwriting the file with garbage.
	curbuf->b_p_ro = TRUE;

	// Set the cryptmethod local to the buffer.
	crypt_سېټ_cm_option(curbuf, لاره);
	إذا (cryptkey == عدم && !*did_ask)
	{
	    إذا (*curbuf->b_p_کلی)
	    {
		cryptkey = curbuf->b_p_کلی;
		crypt_check_swapfile_curbuf();
	    }
	    وإلا
	    {
		// When newfile is TRUE, store the typed key in the 'key'
		// option and don't free it.  bf needs hash of the key saved.
		// Don't ask for the key again when first time Enter was hit.
		// Happens when retrying to detect encoding.
		smsg(_(need_کلی_پیغام), fname);
		پیغام_scroll = TRUE;
		crypt_check_لاره(لاره);
		cryptkey = crypt_get_کلی(newfile, FALSE);
		*did_ask = TRUE;

		// check if empty key entered
		إذا (cryptkey != عدم && *cryptkey == NUL)
		{
		    إذا (cryptkey != curbuf->b_p_کلی)
			vim_free(cryptkey);
		    cryptkey = عدم;
		}
	    }
	}

	إذا (cryptkey != عدم)
	{
	    صحيح سرلیک_len;

	    سرلیک_len = crypt_get_سرلیک_len(لاره);
	    إذا (*sizep < سرلیک_len)
	    {
		// invalid header, buffer can't be encrypted
		إذا (cryptkey != curbuf->b_p_کلی)
		    vim_free(cryptkey);
		أرجع عدم;
	    }

	    curbuf->b_cryptstate = crypt_create_from_سرلیک(
							لاره, cryptkey, ptr);
	    crypt_سېټ_cm_option(curbuf, لاره);

	    // Remove cryptmethod specific header from the text.
	    *filesizep += سرلیک_len;
	    *sizep -= سرلیک_len;
	    mch_memmove(ptr, ptr + سرلیک_len, (size_t)*sizep);

	    // Restore the read-only flag.
	    curbuf->b_p_ro = b_p_ro;
	}
    }
    // When starting to edit a new file which does not have encryption, clear
    // the 'key' option, except when starting up (called with -x argument)
    وإلا إذا (newfile && *curbuf->b_p_کلی != NUL && !starting)
	سېټ_option_ارزښت_give_err((char_u *)"key", 0L, (char_u *)"", OPT_LOCAL);

    أرجع cryptkey;
}
#د_که_پای  // FEAT_CRYPT

/*
 * Return TRUE if a file appears to be read-only from the file permissions.
 */
    صحيح
check_file_readonly(
    char_u	*fname,		// full path to file
    صحيح		perm ناکارول_شوی)	// known permissions on file
{
#که_نه_تعریف_شوی USE_MCH_ACCESS
    صحيح	    fd = 0;
#د_که_پای

    أرجع (
#که_تعریف_شوی USE_MCH_ACCESS
# ifdef UNIX
	(perm & 0222) == 0 ||
# endif
	mch_access((حرف *)fname, W_OK)
#وإلا
	(fd = mch_open((حرف *)fname, O_RDWR | O_EXTRA, 0)) < 0
					? TRUE : (close(fd), FALSE)
#د_که_پای
	);
}

#إذا defined(HAVE_FSYNC)
/*
 * Call fsync() with Mac-specific exception.
 * Return fsync() result: zero for success.
 */
    صحيح
vim_fsync(صحيح fd)
{
    صحيح r;

# ifdef MACOS_X
    r = fcntl(fd, F_FULLFSYNC);
    إذا (r != 0)  // F_FULLFSYNC not working or not supported
# endif
	r = fsync(fd);
    أرجع r;
}
#د_که_پای

/*
 * Set the name of the current buffer.  Use when the buffer doesn't have a
 * name and a ":r" or ":w" command with a file name is used.
 */
    صحيح
سېټ_rw_fname(char_u *fname, char_u *sfname)
{
    buf_T	*buf = curbuf;

    // It's like the unnamed buffer is deleted....
    إذا (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFDELETE, عدم, عدم, FALSE, curbuf);
    apply_autocmds(EVENT_BUFWIPEOUT, عدم, عدم, FALSE, curbuf);
#که_تعریف_شوی FEAT_EVAL
    إذا (aborting())	    // autocmds may abort script processing
	أرجع FAIL;
#د_که_پای
    إذا (curbuf != buf)
    {
	// We are in another buffer now, don't do the renaming.
	emsg(_(e_autocommands_بدل_شوی_بفر_or_بفر_name));
	أرجع FAIL;
    }

    إذا (setfname(curbuf, fname, sfname, FALSE) == OK)
	curbuf->b_flags |= BF_NOTEDITED;

    // ....and a new named one is created
    apply_autocmds(EVENT_BUFNEW, عدم, عدم, FALSE, curbuf);
    إذا (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFADD, عدم, عدم, FALSE, curbuf);
#که_تعریف_شوی FEAT_EVAL
    إذا (aborting())	    // autocmds may abort script processing
	أرجع FAIL;
#د_که_پای

    // Do filetype detection now if 'filetype' is empty.
    إذا (*curbuf->b_p_ft == NUL)
    {
	إذا (au_has_ډله((char_u *)"filetypedetect"))
	    (فراغ)do_doautocmd((char_u *)"filetypedetect BufRead", FALSE, عدم);
	do_modelines(0);
    }

    أرجع OK;
}

/*
 * Put file name into IObuff with quotes.
 */
    فراغ
پیغام_جمع_کړه_fname(buf_T *buf, char_u *fname)
{
    size_t  IObufflen = 0;

    إذا (fname == عدم)
	fname = (char_u *)"-stdin-";
    IObuff[IObufflen++] = '"';
    IObufflen += home_replace(buf, fname, IObuff + IObufflen, IOSIZE - 4, TRUE);
    STRCPY(IObuff + IObufflen, "\" ");
}

/*
 * Append message for text mode to IObuff.
 * Return TRUE if something appended.
 */
    صحيح
پیغام_جمع_کړه_fileformat(صحيح eol_type)
{
#که_نه_تعریف_شوی USE_CRNL
    إذا (eol_type == EOL_DOS)
    {
	STRCAT(IObuff, shortmess(SHM_متن) ? _("[dos]") : _("[dos format]"));
	أرجع TRUE;
    }
#د_که_پای
    إذا (eol_type == EOL_MAC)
    {
	STRCAT(IObuff, shortmess(SHM_متن) ? _("[mac]") : _("[mac format]"));
	أرجع TRUE;
    }
#که_تعریف_شوی USE_CRNL
    إذا (eol_type == EOL_UNIX)
    {
	STRCAT(IObuff, shortmess(SHM_متن) ? _("[unix]") : _("[unix format]"));
	أرجع TRUE;
    }
#د_که_پای
    أرجع FALSE;
}

/*
 * Append line and character count to IObuff.
 */
    فراغ
پیغام_جمع_کړه_کرښې(
    صحيح	    ورننباسه_space,
    اوږد    lnum,
    off_T   nchars)
{
    صحيح  len = (صحيح)STRLEN(IObuff);

    إذا (shortmess(SHM_کرښې))
	vim_snprintf((حرف *)IObuff + len, IOSIZE - (size_t)len,
		// l10n: L as in line, B as in byte
		_("%s%ldL, %lldB"), ورننباسه_space ? " " : "", lnum, (varnumber_T)nchars);
    وإلا
    {
	len += vim_snprintf((حرف *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%s%ld line, ", "%s%ld lines, ", lnum), ورننباسه_space ? " " : "", lnum);
	vim_snprintf((حرف *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%lld byte", "%lld bytes", nchars), (varnumber_T)nchars);
    }
}

/*
 * Append message for missing line separator to IObuff.
 */
    فراغ
پیغام_جمع_کړه_eol(فراغ)
{
    STRCAT(IObuff, shortmess(SHM_وروستی) ? _("[noeol]") : _("[Incomplete last line]"));
}

    صحيح
وخت_differs(stat_T *st, اوږد mtime, اوږد mtime_ns ناکارول_شوی)
{
    أرجع
#که_تعریف_شوی ST_MTIM_NSEC
	(اوږد)st->ST_MTIM_NSEC != mtime_ns ||
#د_که_پای
#إذا defined(__linux__) || defined(MSWIN)
	// On a FAT filesystem, esp. under Linux, there are only 5 bits to store
	// the seconds.  Since the roundoff is done when flushing the inode, the
	// time may change unexpectedly by one second!!!
	(اوږد)st->st_mtime - mtime > 1 || mtime - (اوږد)st->st_mtime > 1
#وإلا
	(اوږد)st->st_mtime != mtime
#د_که_پای
	;
}

/*
 * Return TRUE if file encoding "fenc" requires conversion from or to
 * 'encoding'.
 */
    صحيح
need_conversion(char_u *fenc)
{
    صحيح		same_encoding;
    صحيح		enc_flags;
    صحيح		fenc_flags;

    إذا (*fenc == NUL || STRCMP(p_enc, fenc) == 0)
    {
	same_encoding = TRUE;
	fenc_flags = 0;
    }
    وإلا
    {
	// Ignore difference between "ansi" and "latin1", "ucs-4" and
	// "ucs-4be", etc.
	enc_flags = get_fio_flags(p_enc);
	fenc_flags = get_fio_flags(fenc);
	same_encoding = (enc_flags != 0 && fenc_flags == enc_flags);
    }
    إذا (same_encoding)
    {
	// Specified encoding matches with 'encoding'.  This requires
	// conversion when 'encoding' is Unicode but not UTF-8.
	أرجع enc_unicode != 0;
    }

    // Encodings differ.  However, conversion is not needed when 'enc' is any
    // Unicode encoding and the file is UTF-8.
    أرجع !(enc_utf8 && fenc_flags == FIO_UTF8);
}

/*
 * Check "ptr" for a unicode encoding and return the FIO_ flags needed for the
 * internal conversion.
 * if "ptr" is an empty string, use 'encoding'.
 */
    صحيح
get_fio_flags(char_u *ptr)
{
    صحيح		prop;

    إذا (*ptr == NUL)
	ptr = p_enc;

    prop = enc_canon_props(ptr);
    إذا (prop & ENC_UNICODE)
    {
	إذا (prop & ENC_2BYTE)
	{
	    إذا (prop & ENC_ENDIAN_L)
		أرجع FIO_UCS2 | FIO_ENDIAN_L;
	    أرجع FIO_UCS2;
	}
	إذا (prop & ENC_4BYTE)
	{
	    إذا (prop & ENC_ENDIAN_L)
		أرجع FIO_UCS4 | FIO_ENDIAN_L;
	    أرجع FIO_UCS4;
	}
	إذا (prop & ENC_2_کلمه)
	{
	    إذا (prop & ENC_ENDIAN_L)
		أرجع FIO_UTF16 | FIO_ENDIAN_L;
	    أرجع FIO_UTF16;
	}
	أرجع FIO_UTF8;
    }
    إذا (prop & ENC_LATIN1)
	أرجع FIO_LATIN1;
    // must be ENC_DBCS, requires iconv()
    أرجع 0;
}

#إذا defined(MSWIN)
/*
 * Check "ptr" for a MS-Windows codepage name and return the FIO_ flags needed
 * for the conversion MS-Windows can do for us.  Also accept "utf-8".
 * Used for conversion between 'encoding' and 'fileencoding'.
 */
    صحيح
get_win_fio_flags(char_u *ptr)
{
    صحيح		cp;

    // Cannot do this when 'encoding' is not utf-8 and not a codepage.
    إذا (!enc_utf8 && enc_codepage <= 0)
	أرجع 0;

    cp = encname2codepage(ptr);
    إذا (cp == 0)
    {
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	إذا (STRCMP(ptr, "utf-8") == 0)
	    cp = CP_UTF8;
	وإلا
# endif
	    أرجع 0;
    }
    أرجع FIO_PUT_CP(cp) | FIO_CODEPAGE;
}
#د_که_پای

#إذا defined(MACOS_CONVERT)
/*
 * Check "ptr" for a Carbon supported encoding and return the FIO_ flags
 * needed for the internal conversion to/from utf-8 or latin1.
 */
    صحيح
get_mac_fio_flags(char_u *ptr)
{
    إذا ((enc_utf8 || STRCMP(p_enc, "latin1") == 0)
				     && (enc_canon_props(ptr) & ENC_MACROMAN))
	أرجع FIO_MACROMAN;
    أرجع 0;
}
#د_که_پای

/*
 * Check for a Unicode BOM (Byte Order Mark) at the start of p[size].
 * "size" must be at least 2.
 * Return the name of the encoding and set "*lenp" to the length.
 * Returns NULL when no BOM found.
 */
    ساكن char_u *
check_for_bom(
    char_u	*p,
    اوږد	اندازه,
    صحيح		*lenp,
    صحيح		flags)
{
    حرف	*name = عدم;
    صحيح		len = 2;

    إذا (p[0] == 0xef && p[1] == 0xbb && اندازه >= 3 && p[2] == 0xbf
	    && (flags == FIO_ALL || flags == FIO_UTF8 || flags == 0))
    {
	name = "utf-8";		// EF BB BF
	len = 3;
    }
    وإلا إذا (p[0] == 0xff && p[1] == 0xfe)
    {
	إذا (اندازه >= 4 && p[2] == 0 && p[3] == 0
	    && (flags == FIO_ALL || flags == (FIO_UCS4 | FIO_ENDIAN_L)))
	{
	    name = "ucs-4le";	// FF FE 00 00
	    len = 4;
	}
	وإلا إذا (flags == (FIO_UCS2 | FIO_ENDIAN_L))
	    name = "ucs-2le";	// FF FE
	وإلا إذا (flags == FIO_ALL || flags == (FIO_UTF16 | FIO_ENDIAN_L))
	    // utf-16le is preferred, it also works for ucs-2le text
	    name = "utf-16le";	// FF FE
    }
    وإلا إذا (p[0] == 0xfe && p[1] == 0xff
	    && (flags == FIO_ALL || flags == FIO_UCS2 || flags == FIO_UTF16))
    {
	// Default to utf-16, it works also for ucs-2 text.
	إذا (flags == FIO_UCS2)
	    name = "ucs-2";	// FE FF
	وإلا
	    name = "utf-16";	// FE FF
    }
    وإلا إذا (اندازه >= 4 && p[0] == 0 && p[1] == 0 && p[2] == 0xfe
	    && p[3] == 0xff && (flags == FIO_ALL || flags == FIO_UCS4))
    {
	name = "ucs-4";		// 00 00 FE FF
	len = 4;
    }

    *lenp = len;
    أرجع (char_u *)name;
}

/*
 * Try to find a shortname by comparing the fullname with the current
 * directory.
 * Returns "full_path" or pointer into "full_path" if shortened.
 */
    char_u *
shorten_fname1(char_u *ډک_لار)
{
    char_u	*dirname;
    char_u	*p = ډک_لار;

    dirname = alloc(MAXPATHL);
    إذا (dirname == عدم)
	أرجع ډک_لار;
    إذا (mch_dirname(dirname, MAXPATHL) == OK)
    {
	p = shorten_fname(ډک_لار, dirname);
	إذا (p == عدم || *p == NUL)
	    p = ډک_لار;
    }
    vim_free(dirname);
    أرجع p;
}

/*
 * Try to find a shortname by comparing the fullname with the current
 * directory.
 * Returns NULL if not shorter name possible, pointer into "full_path"
 * otherwise.
 */
    char_u *
shorten_fname(char_u *ډک_لار, char_u *dir_name)
{
    صحيح		len;
    char_u	*p;

    إذا (ډک_لار == عدم)
	أرجع عدم;
    len = (صحيح)STRLEN(dir_name);
    إذا (fnamencmp(dir_name, ډک_لار, len) == 0)
    {
	p = ډک_لار + len;
#إذا defined(MSWIN)
	/*
	 * MS-Windows: when a file is in the root directory, dir_name will end
	 * in a slash, since C: by itself does not define a specific dir. In
	 * this case p may already be correct. <negri>
	 */
	إذا (!((len > 2) && (*(p - 2) == ':')))
#د_که_پای
	{
	    إذا (vim_ispathsep(*p))
		افعل
		    ++p;
		طالما (vim_ispathsep_nocolon(*p));
#که_نه_تعریف_شوی VMS   // the path separator is always part of the path
	    وإلا
		p = عدم;
#د_که_پای
	}
    }
#إذا defined(MSWIN)
    /*
     * When using a file in the current drive, remove the drive name:
     * "A:\dir\file" -> "\dir\file".  This helps when moving a session file on
     * a floppy from "A:\dir" to "B:\dir".
     */
    وإلا إذا (len > 3
	    && TOUPPER_LOC(ډک_لار[0]) == TOUPPER_LOC(dir_name[0])
	    && ډک_لار[1] == ':'
	    && vim_ispathsep(ډک_لار[2]))
	p = ډک_لار + 2;
#د_که_پای
    وإلا
	p = عدم;
    أرجع p;
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
    فراغ
shorten_buf_fname(buf_T *buf, char_u *dirname, صحيح force)
{
    char_u	*p;

    إذا (buf->b_fname != عدم
	    && !bt_nofilename(buf)
	    && !لار_with_url(buf->b_fname)
	    && (force
		|| buf->b_sfname == عدم
		|| mch_is_ډک_Name(buf->b_sfname)))
    {
	إذا (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	p = shorten_fname(buf->b_ffname, dirname);
	إذا (p != عدم)
	{
	    buf->b_sfname = vim_strsave(p);
	    buf->b_fname = buf->b_sfname;
	}
	إذا (p == عدم || buf->b_fname == عدم)
	    buf->b_fname = buf->b_ffname;
    }
}

/*
 * Shorten filenames for all buffers.
 */
    فراغ
shorten_fnames(صحيح force)
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
    حالت_redraw_all();
    redraw_tabline = TRUE;
#إذا defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#د_که_پای
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    popup_update_preview_title();
#د_که_پای
}

#إذا (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
/*
 * Shorten all filenames in "fnames[count]" by current directory.
 */
    فراغ
shorten_filenames(char_u **fnames, صحيح شمېر)
{
    صحيح		i;
    char_u	dirname[MAXPATHL];
    char_u	*p;

    إذا (fnames == عدم || شمېر < 1)
	أرجع;
    mch_dirname(dirname, حجم(dirname));
    لكل (i = 0; i < شمېر; ++i)
    {
	إذا ((p = shorten_fname(fnames[i], dirname)) != عدم)
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
#د_که_پای

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
    صحيح	    prepend_dot)	// may prepend a '.' to file name
{
    أرجع buf_modname((curbuf->b_p_sn || curbuf->b_shortname),
						     fname, ext, prepend_dot);
}

    char_u *
buf_modname(
    صحيح	    shortname,		// use 8.3 file name
    char_u  *fname,
    char_u  *ext,
    صحيح	    prepend_dot)	// may prepend a '.' to file name
{
    char_u	*retval;
    char_u	*s;
    char_u	*e;
    char_u	*ptr;
    size_t	ptrlen;
    صحيح		fnamelen, extlen;

    extlen = (صحيح)STRLEN(ext);

    /*
     * If there is no file name we must get the name of the current directory
     * (we need the full path in case :cd is used).
     */
    إذا (fname == عدم || *fname == NUL)
    {
	retval = alloc(MAXPATHL + extlen + 3);
	إذا (retval == عدم)
	    أرجع عدم;
	إذا (mch_dirname(retval, MAXPATHL) == FAIL ||
				     (fnamelen = (صحيح)STRLEN(retval)) == 0)
	{
	    vim_free(retval);
	    أرجع عدم;
	}
	إذا (!after_pathsep(retval, retval + fnamelen))
	{
	    retval[fnamelen++] = PATHSEP;
	    retval[fnamelen] = NUL;
	}
	prepend_dot = FALSE;	    // nothing to prepend a dot to
    }
    وإلا
    {
	fnamelen = (صحيح)STRLEN(fname);
	retval = alloc(fnamelen + extlen + 3);
	إذا (retval == عدم)
	    أرجع عدم;
	STRCPY(retval, fname);
#که_تعریف_شوی VMS
	vms_remove_نسخه(retval); // we do not need versions here
#د_که_پای
    }

    /*
     * search backwards until we hit a '/', '\' or ':' replacing all '.'
     * by '_' for MSDOS or when shortname option set and ext starts with a dot.
     * Then truncate what is after the '/', '\' or ':' to 8 characters for
     * MSDOS and 26 characters for AMIGA, a lot more for UNIX.
     */
    لكل (ptr = retval + fnamelen; ptr > retval; MB_PTR_شا(retval, ptr))
    {
	إذا (*ext == '.' && shortname)
	    إذا (*ptr == '.')	// replace '.' by '_'
		*ptr = '_';
	إذا (vim_ispathsep(*ptr))
	{
	    ++ptr;
	    اكسر;
	}
    }

    // the file name has at most BASENAMELEN characters.
    ptrlen = (size_t)(fnamelen - (ptr - retval));
    إذا (ptrlen > (بینښه)BASENAMELEN)
    {
	ptrlen = BASENAMELEN;
	ptr[ptrlen] = NUL;
    }

    s = ptr + ptrlen;

    /*
     * For 8.3 file names we may have to reduce the length.
     */
    إذا (shortname)
    {
	/*
	 * If there is no file name, or the file name ends in '/', and the
	 * extension starts with '.', put a '_' before the dot, because just
	 * ".ext" is invalid.
	 */
	إذا (fname == عدم || *fname == NUL
				   || vim_ispathsep(fname[STRLEN(fname) - 1]))
	{
	    إذا (*ext == '.')
		*s++ = '_';
	}
	/*
	 * If the extension starts with '.', truncate the base name at 8
	 * characters
	 */
	وإلا إذا (*ext == '.')
	{
	    إذا ((size_t)(s - ptr) > (size_t)8)
	    {
		s = ptr + 8;
		*s = '\0';
	    }
	}
	/*
	 * If the extension doesn't start with '.', and the file name
	 * doesn't have an extension yet, append a '.'
	 */
	وإلا إذا ((e = vim_strchr(ptr, '.')) == عدم)
	    *s++ = '.';
	/*
	 * If the extension doesn't start with '.', and there already is an
	 * extension, it may need to be truncated
	 */
	وإلا إذا ((صحيح)(ptrlen - (e - retval)) + extlen > 4)
	    s = e + 4 - extlen;
    }
#که_تعریف_شوی MSWIN
    /*
     * If there is no file name, and the extension starts with '.', put a
     * '_' before the dot, because just ".ext" may be invalid if it's on a
     * FAT partition, and on HPFS it doesn't matter.
     */
    وإلا إذا ((fname == عدم || *fname == NUL) && *ext == '.')
	*s++ = '_';
#د_که_پای

    /*
     * Append the extension.
     * ext can start with '.' and cannot exceed 3 more characters.
     */
    STRCPY(s, ext);
    /*
     * Prepend the dot.
     */
    إذا (prepend_dot && !shortname && *(e = gettail(retval)) != '.')
    {
	mch_memmove(e + 1, e, (size_t)(((fnamelen + extlen) - (e - retval)) + 1));	// +1 for NUL
	*e = '.';
    }

    /*
     * Check that, after appending the extension, the file name is really
     * different.
     */
    إذا (fname != عدم && STRCMP(fname, retval) == 0)
    {
	// we search for a character that can be replaced by '_'
	طالما (--s >= ptr)
	{
	    إذا (*s != '_')
	    {
		*s = '_';
		اكسر;
	    }
	}
	إذا (s < ptr)	// fname was "________.<ext>", how tricky!
	    *ptr = 'v';
    }
    أرجع retval;
}

/*
 * Like fgets(), but if the file line is too long, it is truncated and the
 * rest of the line is thrown away.  Returns TRUE for end-of-file.
 * If the line is truncated then buf[size - 2] will not be NUL.
 */
    صحيح
vim_fgets(char_u *buf, صحيح اندازه, FILE *fp)
{
    حرف	*eof;
#تعریف_کړه FGETS_اندازه 200
    حرف	tbuf[FGETS_اندازه];

    // safety check
    إذا (اندازه < 2)
    {
	إذا (اندازه == 1)
	    buf[0] = NUL;
	أرجع TRUE;
    }

    buf[اندازه - 2] = NUL;
    eof = fgets((حرف *)buf, اندازه, fp);
    إذا (buf[اندازه - 2] != NUL && buf[اندازه - 2] != '\n')
    {
	buf[اندازه - 1] = NUL;	    // Truncate the line

	// Now throw away the rest of the line:
	افعل
	{
	    tbuf[FGETS_اندازه - 2] = NUL;
	    vim_ignoredp = fgets((حرف *)tbuf, FGETS_اندازه, fp);
	} طالما (tbuf[FGETS_اندازه - 2] != NUL && tbuf[FGETS_اندازه - 2] != '\n');
    }
    أرجع (eof == عدم);
}

/*
 * rename() only works if both files are on the same file system, this
 * function will (attempts to?) copy the file across if rename fails -- webb
 * Return -1 for failure, 0 for success.
 */
    صحيح
vim_rename(char_u *from, char_u *منزل)
{
    صحيح		n;
    صحيح		ret;
#که_تعریف_شوی AMIGA
    BPTR	flock;
#د_که_پای
    stat_T	st;
    صحيح		use_tmp_file = FALSE;

    /*
     * When the names are identical, there is nothing to do.  When they refer
     * to the same file (ignoring case and slash/backslash differences) but
     * the file name differs we need to go through a temp file.
     */
    إذا (fnamecmp(from, منزل) == 0)
    {
	إذا (p_fic && STRCMP(gettail(from), gettail(منزل)) != 0)
	    use_tmp_file = TRUE;
	وإلا
	    أرجع 0;
    }

    /*
     * Fail if the "from" file doesn't exist.  Avoids that "to" is deleted.
     */
    إذا (mch_stat((حرف *)from, &st) < 0)
	أرجع -1;

#که_تعریف_شوی UNIX
    {
	stat_T	st_منزل;

	// It's possible for the source and destination to be the same file.
	// This happens when "from" and "to" differ in case and are on a FAT32
	// filesystem.  In that case go through a temp file name.
	إذا (mch_stat((حرف *)منزل, &st_منزل) >= 0
		&& st.st_dev == st_منزل.st_dev
		&& st.st_ino == st_منزل.st_ino)
	    use_tmp_file = TRUE;
    }
#د_که_پای
#که_تعریف_شوی MSWIN
    {
	BY_HANDLE_FILE_INFORMATION info1, info2;

	// It's possible for the source and destination to be the same file.
	// In that case go through a temp file name.  This makes rename("foo",
	// "./foo") a no-op (in a complicated way).
	إذا (win32_fileinfo(from, &info1) == FILEINFO_OK
		&& win32_fileinfo(منزل, &info2) == FILEINFO_OK
		&& info1.dw_Volume_Serial_شمېره == info2.dw_Volume_Serial_شمېره
		&& info1.n_File_شاخص_لوړ == info2.n_File_شاخص_لوړ
		&& info1.n_File_شاخص_ټیټ == info2.n_File_شاخص_ټیټ)
	    use_tmp_file = TRUE;
    }
#د_که_پای

    إذا (use_tmp_file)
    {
	حرف	tempname[MAXPATHL + 1];

	/*
	 * Find a name that doesn't exist and is in the same directory.
	 * Rename "from" to "tempname" and then rename "tempname" to "to".
	 */
	إذا (STRLEN(from) >= MAXPATHL - 5)
	    أرجع -1;
	STRCPY(tempname, from);
	لكل (n = 123; n < 99999; ++n)
	{
	    sprintf((حرف *)gettail((char_u *)tempname), "%d", n);
	    إذا (mch_stat(tempname, &st) < 0)
	    {
		إذا (mch_rename((حرف *)from, tempname) == 0)
		{
		    إذا (mch_rename(tempname, (حرف *)منزل) == 0)
			أرجع 0;
		    // Strange, the second step failed.  Try moving the
		    // file back and return failure.
		    (فراغ)mch_rename(tempname, (حرف *)from);
		    أرجع -1;
		}
		// If it fails for one temp name it will most likely fail
		// for any temp name, give up.
		أرجع -1;
	    }
	}
	أرجع -1;
    }

    /*
     * Delete the "to" file, this is required on some systems to make the
     * mch_rename() work, on other systems it makes sure that we don't have
     * two files when the mch_rename() fails.
     */

#که_تعریف_شوی AMIGA
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
    flock = قلف_2((UBYTE *)from, (اوږد)VIM_ACCESS_READ);
#د_که_پای
    mch_remove(منزل);
#که_تعریف_شوی AMIGA
    إذا (flock)
	Un_قلف(flock);
#د_که_پای

    /*
     * First try a normal rename, return if it works.
     */
    إذا (mch_rename((حرف *)from, (حرف *)منزل) == 0)
	أرجع 0;

    /*
     * Rename() failed, try copying the file.
     */
    ret = vim_copyfile(from, منزل);
    إذا (ret != OK)
	أرجع -1;

    /*
     * Remove copied original file
     */
    إذا (mch_stat((حرف *)from, &st) >= 0)
	mch_remove(from);

    أرجع 0;
}


/*
 * Create the new file with same permissions as the original.
 * Return FAIL for failure, OK for success.
 */
    صحيح
vim_copyfile(char_u *from, char_u *منزل)
{
    صحيح		fd_دننه;
    صحيح		fd_بهر;
    صحيح		n;
    حرف	*errmsg = عدم;
    حرف	*بفر;
    اوږد	perm;
#که_تعریف_شوی HAVE_ACL
    vim_acl_T	acl;		// ACL from original file
#د_که_پای

#که_تعریف_شوی HAVE_READLINK
    صحيح		ret;
    صحيح		len;
    stat_T	st;
    حرف	linkbuf[MAXPATHL + 1];

    ret = mch_lstat((حرف *)from, &st);
    إذا (ret >= 0 && S_ISLNK(st.st_ډېرپېښ))
    {
	ret = -1;

	len = readlink((حرف *)from, linkbuf, MAXPATHL);
	إذا (len > 0)
	{
	    linkbuf[len] = NUL;

	    // Create link
	    ret = symlink(linkbuf, (حرف *)منزل);
	}

	أرجع ret == 0 ? OK : FAIL;
    }
#د_که_پای

    perm = mch_getperm(from);
#که_تعریف_شوی HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    acl = mch_get_acl(from);
#د_که_پای
    fd_دننه = mch_open((حرف *)from, O_RDONLY|O_EXTRA, 0);
    إذا (fd_دننه == -1)
    {
#که_تعریف_شوی HAVE_ACL
	mch_free_acl(acl);
#د_که_پای
	أرجع FAIL;
    }

    // Create the new file with same permissions as the original.
    fd_بهر = mch_open((حرف *)منزل,
		       O_CREAT|O_EXCL|O_WRONLY|O_EXTRA|O_NOFOLLOW, (صحيح)perm);
    إذا (fd_بهر == -1)
    {
	close(fd_دننه);
#که_تعریف_شوی HAVE_ACL
	mch_free_acl(acl);
#د_که_پای
	أرجع FAIL;
    }

    بفر = alloc(WRITEBUFSIZE);
    إذا (بفر == عدم)
    {
	close(fd_بهر);
	close(fd_دننه);
#که_تعریف_شوی HAVE_ACL
	mch_free_acl(acl);
#د_که_پای
	أرجع FAIL;
    }

    طالما ((n = read_eintr(fd_دننه, بفر, WRITEBUFSIZE)) > 0)
	إذا (write_eintr(fd_بهر, بفر, n) != n)
	{
	    errmsg = _(e_error_writing_منزل_str);
	    اكسر;
	}

    vim_free(بفر);
    close(fd_دننه);
    إذا (close(fd_بهر) < 0)
	errmsg = _(e_error_closing_str);
    إذا (n < 0)
    {
	errmsg = _(e_error_reading_str);
	منزل = from;
    }
#که_نه_تعریف_شوی UNIX	    // for Unix mch_open() already set the permission
    mch_setperm(منزل, perm);
#د_که_پای
#که_تعریف_شوی HAVE_ACL
    mch_سېټ_acl(منزل, acl);
    mch_free_acl(acl);
#د_که_پای
#إذا defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    mch_copy_sec(from, منزل);
#د_که_پای
    إذا (errmsg != عدم)
    {
	semsg(errmsg, منزل);
	أرجع FAIL;
    }
    أرجع OK;
}

ساكن صحيح already_warned = FALSE;

/*
 * Check if any not hidden buffer has been changed.
 * Postpone the check if there are characters in the stuff buffer, a global
 * command is being executed, a mapping is being executed or an autocommand is
 * busy.
 * Returns TRUE if some message was written (screen should be redrawn and
 * cursor positioned).
 */
    صحيح
check_timestamps(
    صحيح		focus)		// called for GUI focus event
{
    buf_T	*buf;
    صحيح		didit = 0;
    صحيح		n;

    // Don't check timestamps while system() or another low-level function may
    // cause us to lose and gain focus.
    إذا (no_check_timestamps > 0)
	أرجع FALSE;

    // Avoid doing a check twice.  The OK/Reload dialog can cause a focus
    // event and we would keep on checking if the file is steadily growing.
    // Do check again after typing something.
    إذا (focus && did_check_timestamps)
    {
	need_check_timestamps = TRUE;
	أرجع FALSE;
    }

    إذا (!stuff_تشه() || global_busy || !typebuf_typed()
			|| autocmd_busy || curbuf_قلف > 0 || allbuf_قلف > 0)
	need_check_timestamps = TRUE;		// check later
    وإلا
    {
	++no_wait_return;
	did_check_timestamps = TRUE;
	already_warned = FALSE;
	FOR_ALL_BUFFERS(buf)
	{
	    // Only check buffers in a window.
	    إذا (buf->b_nwindows > 0)
	    {
		bufref_T bufref;

		سېټ_bufref(&bufref, buf);
		n = buf_check_timestamp(buf, focus);
		إذا (didit < n)
		    didit = n;
		إذا (n > 0 && !bufref_valid(&bufref))
		{
		    // Autocommands have removed the buffer, start at the
		    // first one again.
		    buf = firstbuf;
		    استمر;
		}
	    }
	}
	--no_wait_return;
	need_check_timestamps = FALSE;
	إذا (need_wait_return && didit == 2)
	{
	    // make sure msg isn't overwritten
	    پیغام_puts("\n");
	    بهر_flush();
	}
    }
    أرجع didit;
}

/*
 * Move all the lines from buffer "frombuf" to buffer "tobuf".
 * Return OK or FAIL.  When FAIL "tobuf" is incomplete and/or "frombuf" is not
 * empty.
 */
    ساكن صحيح
move_کرښې(buf_T *frombuf, buf_T *tobuf)
{
    buf_T	*tbuf = curbuf;
    صحيح		retval = OK;
    linenr_T	lnum;
    char_u	*p;

    // Copy the lines in "frombuf" to "tobuf".
    curbuf = tobuf;
    لكل (lnum = 1; lnum <= frombuf->b_ml.ml_کرښه_شمېر; ++lnum)
    {
	p = vim_strnsave(ml_get_buf(frombuf, lnum, FALSE), ml_get_buf_len(frombuf, lnum));
	إذا (p == عدم || ml_append(lnum - 1, p, 0, FALSE) == FAIL)
	{
	    vim_free(p);
	    retval = FAIL;
	    اكسر;
	}
	vim_free(p);
    }

    // Delete all the lines in "frombuf".
    إذا (retval != FAIL)
    {
	curbuf = frombuf;
	لكل (lnum = curbuf->b_ml.ml_کرښه_شمېر; lnum > 0; --lnum)
	    إذا (ml_delete(lnum) == FAIL)
	    {
		// Oops!  We could try putting back the saved lines, but that
		// might fail again...
		retval = FAIL;
		اكسر;
	    }
    }

    curbuf = tbuf;
    أرجع retval;
}

/*
 * Check if buffer "buf" has been changed.
 * Also check if the file for a new buffer unexpectedly appeared.
 * return 1 if a changed buffer was found.
 * return 2 if a message has been displayed.
 * return 0 otherwise.
 */
    صحيح
buf_check_timestamp(
    buf_T	*buf,
    صحيح		focus ناکارول_شوی)	// called for GUI focus event
{
    stat_T	st;
    صحيح		stat_res;
    صحيح		retval = 0;
    حرف	*mesg = عدم;
    حرف	*mesg2 = "";
    صحيح		helpmesg = FALSE;
    تعداد {
	RELOAD_NONE,
	RELOAD_نورمال,
	RELOAD_DETECT
    }		reload = RELOAD_NONE;
#إذا defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
    صحيح		can_reload = FALSE;
#د_که_پای
    off_T	orig_اندازه = buf->b_orig_اندازه;
    صحيح		orig_ډېرپېښ = buf->b_orig_ډېرپېښ;
#که_تعریف_شوی FEAT_GUI
    صحيح		save_mouse_correct = need_mouse_correct;
#د_که_پای
    ساكن صحيح	busy = FALSE;
    صحيح		n;
    bufref_T	bufref;

    سېټ_bufref(&bufref, buf);

    // If there is no file name, the buffer is not loaded, 'buftype' is
    // set, we are in the middle of a save or being called recursively: ignore
    // this buffer.
    إذا (buf->b_ffname == عدم
	    || buf->b_ml.ml_mfp == عدم
	    || !bt_نورمال(buf)
	    || buf->b_saving
	    || busy
#که_تعریف_شوی FEAT_NETBEANS_INTG
	    || is_Netbeans_بفر(buf)
#د_که_پای
#که_تعریف_شوی FEAT_TERMINAL
	    || buf->b_غړی != عدم
#د_که_پای
	    )
	أرجع 0;

    إذا (       !(buf->b_flags & BF_NOTEDITED)
	    && buf->b_mtime != 0
	    && ((stat_res = mch_stat((حرف *)buf->b_ffname, &st)) < 0
		|| وخت_differs(&st, buf->b_mtime, buf->b_mtime_ns)
		|| st.st_size != buf->b_orig_اندازه
#که_تعریف_شوی HAVE_ST_ډېرپېښ
		|| (صحيح)st.st_ډېرپېښ != buf->b_orig_ډېرپېښ
#وإلا
		|| mch_getperm(buf->b_ffname) != buf->b_orig_ډېرپېښ
#د_که_پای
		))
    {
	اوږد prev_b_mtime = buf->b_mtime;

	retval = 1;

	// set b_mtime to stop further warnings (e.g., when executing
	// FileChangedShell autocmd)
	إذا (stat_res < 0)
	{
	    // Check the file again later to see if it re-appears.
	    buf->b_mtime = -1;
	    buf->b_orig_اندازه = 0;
	    buf->b_orig_ډېرپېښ = 0;
	}
	وإلا
	    buf_store_وخت(buf, &st, buf->b_ffname);

	// Don't do anything for a directory.  Might contain the file
	// explorer.
	إذا (mch_isdir(buf->b_fname))
	    ;

	/*
	 * If 'autoread' is set, the buffer has no changes and the file still
	 * exists, reload the buffer.  Use the buffer-local option value if it
	 * was set, the global option value otherwise.
	 */
	وإلا إذا ((buf->b_p_ar >= 0 ? buf->b_p_ar : p_ar)
				       && !buf_Is_بدل_شوی(buf) && stat_res >= 0)
	    reload = RELOAD_نورمال;
	وإلا
	{
	    حرف    *reason;
#که_تعریف_شوی FEAT_EVAL
	    size_t  reasonlen;
#د_که_پای

	    إذا (stat_res < 0)
	    {
		reason = "deleted";
#که_تعریف_شوی FEAT_EVAL
		reasonlen = STRLEN_LITERAL("deleted");
#د_که_پای
	    }
	    وإلا إذا (buf_Is_بدل_شوی(buf))
	    {
		reason = "conflict";
#که_تعریف_شوی FEAT_EVAL
		reasonlen = STRLEN_LITERAL("conflict");
#د_که_پای
	    }
	    /*
	     * Check if the file contents really changed to avoid giving a
	     * warning when only the timestamp was set (e.g., checked out of
	     * CVS).  Always warn when the buffer was changed.
	     */
	    وإلا إذا (orig_اندازه != buf->b_orig_اندازه || buf_contents_بدل_شوی(buf))
	    {
		reason = "changed";
#که_تعریف_شوی FEAT_EVAL
		reasonlen = STRLEN_LITERAL("changed");
#د_که_پای
	    }
	    وإلا إذا (orig_ډېرپېښ != buf->b_orig_ډېرپېښ)
	    {
		reason = "mode";
#که_تعریف_شوی FEAT_EVAL
		reasonlen = STRLEN_LITERAL("mode");
#د_که_پای
	    }
	    وإلا
	    {
		reason = "time";
#که_تعریف_شوی FEAT_EVAL
		reasonlen = STRLEN_LITERAL("time");
#د_که_پای
	    }

	    /*
	     * Only give the warning if there are no FileChangedShell
	     * autocommands.
	     * Avoid being called recursively by setting "busy".
	     */
	    busy = TRUE;
#که_تعریف_شوی FEAT_EVAL
	    سېټ_vim_var_string(VV_FCS_REASON, (char_u *)reason, (صحيح)reasonlen);
	    سېټ_vim_var_string(VV_FCS_CHOICE, (char_u *)"", 0);
#د_که_پای
	    ++allbuf_قلف;
	    n = apply_autocmds(EVENT_FILECHANGEDSHELL,
				      buf->b_fname, buf->b_fname, FALSE, buf);
	    --allbuf_قلف;
	    busy = FALSE;
	    إذا (n)
	    {
		إذا (!bufref_valid(&bufref))
		    emsg(_(e_filechangedshell_autocommand_deleted_بفر));
#که_تعریف_شوی FEAT_EVAL
		char_u	*s = get_vim_var_str(VV_FCS_CHOICE);
		إذا (STRCMP(s, "reload") == 0 && *reason != 'd')
		    reload = RELOAD_نورمال;
		وإلا إذا (STRCMP(s, "edit") == 0)
		    reload = RELOAD_DETECT;
		وإلا إذا (STRCMP(s, "ask") == 0)
		    n = FALSE;
		وإلا
#د_که_پای
		    أرجع 2;
	    }
	    إذا (!n)
	    {
		إذا (*reason == 'd')
		{
		    // Only give the message once.
		    إذا (prev_b_mtime != -1)
			mesg = _(e_file_str_no_longer_شته);
		}
		وإلا
		{
		    helpmesg = TRUE;
#إذا defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		    can_reload = TRUE;
#د_که_پای
		    إذا (reason[2] == 'n')
		    {
			mesg = _("W12: Warning: File \"%s\" has changed and the buffer was changed in Vim as well");
			mesg2 = _("See \":help W12\" for more info.");
		    }
		    وإلا إذا (reason[1] == 'h')
		    {
			mesg = _("W11: Warning: File \"%s\" has changed since editing started");
			mesg2 = _("See \":help W11\" for more info.");
		    }
		    وإلا إذا (*reason == 'm')
		    {
			mesg = _("W16: Warning: Mode of file \"%s\" has changed since editing started");
			mesg2 = _("See \":help W16\" for more info.");
		    }
		    وإلا
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
    وإلا إذا ((buf->b_flags & BF_NEW) && !(buf->b_flags & BF_NEW_W)
						&& vim_fexists(buf->b_ffname))
    {
	retval = 1;
	mesg = _("W13: Warning: File \"%s\" has been created after editing started");
	buf->b_flags |= BF_NEW_W;
#إذا defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
	can_reload = TRUE;
#د_که_پای
    }

    إذا (mesg != عدم)
    {
	char_u *لار;

	لار = home_replace_save(buf, buf->b_fname);
	إذا (لار != عدم)
	{
	    size_t  tbufsize;
	    حرف    *tbuf;

	    إذا (!helpmesg)
		mesg2 = "";
	    tbufsize = STRLEN(mesg) + STRLEN(لار) + 2 + STRLEN(mesg2) + 1; // +2 for either '\n' or "; "
									    // and +1 for NUL
	    tbuf = alloc(tbufsize);
	    إذا (tbuf != عدم)
	    {
		صحيح tbuflen;

		tbuflen = vim_snprintf(tbuf, tbufsize, mesg, لار);
#که_تعریف_شوی FEAT_EVAL
		// Set warningmsg here, before the unimportant and output-specific
		// mesg2 has been appended.
		سېټ_vim_var_string(VV_WARNINGMSG, (char_u *)tbuf, tbuflen);
#د_که_پای
#إذا defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		إذا (can_reload)
		{
		    إذا (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "\n%s", mesg2);
		    بدل (do_dialog(VIM_WARNING, (char_u *)_("Warning"),
			    (char_u *)tbuf,
			    (char_u *)_("&OK\n&Load File\nLoad File &and Options"),
			    1, عدم, TRUE))
		    {
			حالة 2:
			    reload = RELOAD_نورمال;
			    اكسر;
			حالة 3:
			    reload = RELOAD_DETECT;
			    اكسر;
		    }
		}
		وإلا
#د_که_پای
		إذا (State > ډېرپېښ_نورمال_BUSY || (State & ډېرپېښ_CMDLINE)
								 || already_warned)
		{
		    إذا (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "; %s", mesg2);
		    emsg(tbuf);
		    retval = 2;
		}
		وإلا
		{
		    إذا (!autocmd_busy)
		    {
			پیغام_پیل();
			پیغام_puts_attr(tbuf, HL_ATTR(HLF_E) + پیغام_HIST);
			إذا (*mesg2 != NUL)
			    پیغام_puts_attr(mesg2, HL_ATTR(HLF_W) + پیغام_HIST);
			پیغام_clr_eos();
			(فراغ)پیغام_پای_ټکی();
			إذا (emsg_silent == 0 && !دننه_assert_fails)
			{
			    بهر_flush();
#که_تعریف_شوی FEAT_GUI
			    إذا (!focus)
#د_که_پای
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
	    vim_free(لار);
	}
    }

    إذا (reload != RELOAD_NONE)
    {
	// Reload the buffer.
	buf_reload(buf, orig_ډېرپېښ, reload == RELOAD_DETECT);
#که_تعریف_شوی FEAT_PERSISTENT_UNDO
	إذا (bufref_valid(&bufref) && buf->b_p_udf && buf->b_ffname != عدم)
	{
	    char_u	    hash[UNDO_HASH_اندازه];
	    buf_T	    *save_curbuf = curbuf;

	    // Any existing undo file is unusable, write it now.
	    curbuf = buf;
	    u_compute_hash(hash);
	    u_write_undo(عدم, FALSE, buf, hash);
	    curbuf = save_curbuf;
	}
#د_که_پای
    }

    // Trigger FileChangedShell when the file was changed in any way.
    إذا (bufref_valid(&bufref) && retval != 0)
	(فراغ)apply_autocmds(EVENT_FILECHANGEDSHELLPOST,
				      buf->b_fname, buf->b_fname, FALSE, buf);
#که_تعریف_شوی FEAT_GUI
    // restore this in case an autocommand has set it; it would break
    // 'mousefocus'
    need_mouse_correct = save_mouse_correct;
#د_که_پای

    أرجع retval;
}

/*
 * Reload a buffer that is already loaded.
 * Used when the file was changed outside of Vim.
 * "orig_mode" is buf->b_orig_mode before the need for reloading was detected.
 * buf->b_orig_mode may have been reset already.
 */
    فراغ
buf_reload(buf_T *buf, صحيح orig_ډېرپېښ, صحيح reload_options)
{
    exarg_T	ea;
    pos_T	old_cursor;
    linenr_T	old_topline;
    صحيح		old_ro = buf->b_p_ro;
    buf_T	*savebuf;
    bufref_T	bufref;
    صحيح		saved = OK;
    aco_save_T	aco;
    صحيح		flags = READ_NEW;
    صحيح		prepped = OK;

    // Set curwin/curbuf for "buf" and save some things.
    aucmd_prepbuf(&aco, buf);
    إذا (curbuf != buf)
    {
	// Failed to find a window for "buf", it is dangerous to continue,
	// better bail out.
	أرجع;
    }

    // Unless reload_options is set, we only want to read the text from the
    // file, not reset the syntax highlighting, clear marks, diff status, etc.
    // Force the fileformat and encoding to be the same.
    إذا (reload_options)
	CLEAR_ډګر(ea);
    وإلا
	prepped = prep_exarg(&ea, buf);

    إذا (prepped == OK)
    {
	old_cursor = curwin->w_cursor;
	old_topline = curwin->w_topline;

	إذا (p_ur < 0 || curbuf->b_ml.ml_کرښه_شمېر <= p_ur)
	{
	    // Save all the text, so that the reload can be undone.
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    saved = u_savecommon(0, curbuf->b_ml.ml_کرښه_شمېر + 1, 0, TRUE);
	    flags |= READ_KEEP_UNDO;
	}

	/*
	 * To behave like when a new file is edited (matters for
	 * BufReadPost autocommands) we first need to delete the current
	 * buffer contents.  But if reading the file fails we should keep
	 * the old contents.  Can't use memory only, the file might be
	 * too big.  Use a hidden buffer to move the buffer contents to.
	 */
	إذا (BUFEMPTY() || saved == FAIL)
	    savebuf = عدم;
	وإلا
	{
	    // Allocate a buffer without putting it in the buffer list.
	    savebuf = buflist_new(عدم, عدم, (linenr_T)1, BLN_DUMMY);
	    سېټ_bufref(&bufref, savebuf);
	    إذا (savebuf != عدم && buf == curbuf)
	    {
		// Open the memline.
		curbuf = savebuf;
		curwin->w_بفر = savebuf;
		saved = ml_open(curbuf);
		curbuf = buf;
		curwin->w_بفر = buf;
	    }
	    إذا (savebuf == عدم || saved == FAIL || buf != curbuf
				      || move_کرښې(buf, savebuf) == FAIL)
	    {
		semsg(_(e_could_not_prepare_for_reloading_str), buf->b_fname);
		saved = FAIL;
	    }
	}

	إذا (saved == OK)
	{
	    صحيح old_پیغام_silent = پیغام_silent;

	    curbuf->b_flags |= BF_CHECK_RO;	// check for RO again
	    curbuf->b_keep_filetype = true;	// don't detect 'filetype'

	    إذا (shortmess(SHM_FILEINFO))
		پیغام_silent = 1;

	    إذا (readfile(buf->b_ffname, buf->b_fname, (linenr_T)0,
			(linenr_T)0,
			(linenr_T)MAXLNUM, &ea, flags) != OK)
	    {
#إذا defined(FEAT_EVAL)
		إذا (!aborting())
#د_که_پای
		    semsg(_(e_could_not_reload_str), buf->b_fname);
		إذا (savebuf != عدم && bufref_valid(&bufref) && buf == curbuf)
		{
		    // Put the text back from the save buffer.  First
		    // delete any lines that readfile() added.
		    طالما (!BUFEMPTY())
			إذا (ml_delete(buf->b_ml.ml_کرښه_شمېر) == FAIL)
			    اكسر;
		    (فراغ)move_کرښې(savebuf, buf);
		}
	    }
	    وإلا إذا (buf == curbuf)  // "buf" still valid
	    {
		// Mark the buffer as unmodified and free undo info.
		unchanged(buf, TRUE, TRUE);
		إذا ((flags & READ_KEEP_UNDO) == 0)
		    u_clearallandblockfree(buf);
		وإلا
		{
		    // Mark all undo states as changed.
		    u_unchanged(curbuf);
		}
	    }

	    پیغام_silent = old_پیغام_silent;
	}
	vim_free(ea.cmd);

	إذا (savebuf != عدم && bufref_valid(&bufref))
	    wipe_بفر(savebuf, FALSE);

#که_تعریف_شوی FEAT_DIFF
	// Invalidate diff info if necessary.
	diff_invalidate(curbuf);
#د_که_پای

	// Restore the topline and cursor position and check it (lines may
	// have been removed).
	إذا (old_topline > curbuf->b_ml.ml_کرښه_شمېر)
	    curwin->w_topline = curbuf->b_ml.ml_کرښه_شمېر;
	وإلا
	    curwin->w_topline = old_topline;
	curwin->w_cursor = old_cursor;
	check_cursor();
	update_topline();
	curbuf->b_keep_filetype = false;
#که_تعریف_شوی FEAT_FOLDING
	{
	    win_T	*wp;
	    tabpage_T	*tp;

	    // Update folds unless they are defined manually.
	    FOR_ALL_TAB_WINDOWS(tp, wp)
		إذا (wp->w_بفر == curwin->w_بفر
			&& !foldmethodIsManual(wp))
		    foldUpdateAll(wp);
	}
#د_که_پای
	// If the mode didn't change and 'readonly' was set, keep the old
	// value; the user probably used the ":view" command.  But don't
	// reset it, might have had a read error.
	إذا (orig_ډېرپېښ == curbuf->b_orig_ډېرپېښ)
	    curbuf->b_p_ro |= old_ro;

	// Modelines must override settings done by autocommands.
	do_modelines(0);
    }

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);
    // Careful: autocommands may have made "buf" invalid!
}

    فراغ
buf_store_وخت(buf_T *buf, stat_T *st, char_u *fname ناکارول_شوی)
{
    buf->b_mtime = (اوږد)st->st_mtime;
#که_تعریف_شوی ST_MTIM_NSEC
    buf->b_mtime_ns = (اوږد)st->ST_MTIM_NSEC;
#وإلا
    buf->b_mtime_ns = 0;
#د_که_پای
    buf->b_orig_اندازه = st->st_size;
#که_تعریف_شوی HAVE_ST_ډېرپېښ
    buf->b_orig_ډېرپېښ = (صحيح)st->st_ډېرپېښ;
#وإلا
    buf->b_orig_ډېرپېښ = mch_getperm(fname);
#د_که_پای
}

/*
 * Adjust the line with missing eol, used for the next write.
 * Used for do_filter(), when the input lines for the filter are deleted.
 */
    فراغ
write_lnum_adjust(linenr_T دننه_واټن)
{
    إذا (curbuf->b_no_eol_lnum != 0)	// only if there is a missing eol
	curbuf->b_no_eol_lnum += دننه_واټن;
}

// Subfuncions for readdirex()
#که_تعریف_شوی FEAT_EVAL
# ifdef MSWIN
    ساكن char_u *
getfpermwfd(WIN32_پیدا_کړه_DATAW *wfd, char_u *perm)
{
    stat_T	    st;
    بینښه لنډ  st_ډېرپېښ;
    DWORD	    flag = wfd->dw_File_ځانګړتیاوې;
    WCHAR	    *wp;

    st_ډېرپېښ = (flag & FILE_ATTRIBUTE_DIRECTORY)
					? (_S_IFDIR | _S_IEXEC) : _S_IFREG;
    st_ډېرپېښ |= (flag & FILE_ATTRIBUTE_READONLY)
					? _S_IREAD : (_S_IREAD | _S_IWRITE);

    wp = wcsrchr(wfd->cFileName, L'.');
    إذا (wp != عدم)
    {
	إذا (_wcsicmp(wp, L".exe") == 0 ||
		_wcsicmp(wp, L".com") == 0 ||
		_wcsicmp(wp, L".cmd") == 0 ||
		_wcsicmp(wp, L".bat") == 0)
	    st_ډېرپېښ |= _S_IEXEC;
    }

    // Copy user bits to group/other.
    st_ډېرپېښ |= (st_ډېرپېښ & 0700) >> 3;
    st_ډېرپېښ |= (st_ډېرپېښ & 0700) >> 6;

    st.st_ډېرپېښ = st_ډېرپېښ;
    أرجع getfpermst(&st, perm);
}

    ساكن فراغ
getftypewfd(WIN32_پیدا_کړه_DATAW *wfd, string_T *ret)
{
    DWORD flag = wfd->dw_File_ځانګړتیاوې;
    DWORD نښان = wfd->dwReserved0;

    إذا (flag & FILE_ATTRIBUTE_REPARSE_ټکی)
    {
	إذا (نښان == IO_REPARSE_نښان_MOUNT_ټکی)
	{
	    ret->string = (char_u *)"junction";
	    ret->اوږدتیا = STRLEN_LITERAL("junction");
	    أرجع;
	}
	إذا (نښان == IO_REPARSE_نښان_SYMLINK)
	{
	    إذا (flag & FILE_ATTRIBUTE_DIRECTORY)
	    {
		ret->string = (char_u *)"linkd";
		ret->اوږدتیا = STRLEN_LITERAL("linkd");
		أرجع;
	    }

	    ret->string = (char_u *)"link";
	    ret->اوږدتیا = STRLEN_LITERAL("link");
	    أرجع;
	}

	// unknown reparse point type
	ret->string = (char_u *)"reparse";
	ret->اوږدتیا = STRLEN_LITERAL("reparse");
	أرجع;
    }

    إذا (flag & FILE_ATTRIBUTE_DIRECTORY)
    {
	ret->string = (char_u *)"dir";
	ret->اوږدتیا = STRLEN_LITERAL("dir");
	أرجع;
    }

    ret->string = (char_u *)"file";
    ret->اوږدتیا = STRLEN_LITERAL("file");
}

    ساكن dict_T *
create_readdirex_توکی(WIN32_پیدا_کړه_DATAW *wfd)
{
    dict_T	*توکی;
    char_u	*p;
    varnumber_T	اندازه, وخت;
    char_u	permbuf[] = "---------";
    string_T	s;

    توکی = dict_alloc();
    إذا (توکی == عدم)
	أرجع عدم;
    توکی->dv_refcount++;

    p = utf16_منزل_enc(wfd->cFileName, عدم);
    إذا (p == عدم)
	اذهب theend;
    إذا (dict_جمع_کړه_string(توکی, "name", p) == FAIL)
    {
	vim_free(p);
	اذهب theend;
    }
    vim_free(p);

    اندازه = (((varnumber_T)wfd->n_File_اندازه_لوړ) << 32) | wfd->n_File_اندازه_ټیټ;
    إذا (dict_جمع_کړه_شمېره(توکی, "size", اندازه) == FAIL)
	اذهب theend;

    // Convert FILETIME to unix time.
    وخت = (((((varnumber_T)wfd->ft_وروستی_Write_وخت.dw_لوړ_Date_وخت) << 32) |
		wfd->ft_وروستی_Write_وخت.dw_ټیټ_Date_وخت)
	    - 116444736000000000) / 10000000;
    إذا (dict_جمع_کړه_شمېره(توکی, "time", وخت) == FAIL)
	اذهب theend;

    getftypewfd(wfd, &s);
    إذا (dict_جمع_کړه_string_len(توکی, "type", s.string, (صحيح)s.اوږدتیا) == FAIL)
	اذهب theend;
    إذا (dict_جمع_کړه_string(توکی, "perm", getfpermwfd(wfd, permbuf)) == FAIL)
	اذهب theend;

    إذا (dict_جمع_کړه_string_len(توکی, "user", (char_u *)"", 0) == FAIL)
	اذهب theend;
    إذا (dict_جمع_کړه_string_len(توکی, "group", (char_u *)"", 0) == FAIL)
	اذهب theend;

    أرجع توکی;

theend:
    dict_unref(توکی);
    أرجع عدم;
}
# وإلا
    ساكن dict_T *
create_readdirex_توکی(char_u *لار, char_u *name)
{
    dict_T	*توکی;
    حرف	*p;
    size_t	pathlen, len;
    size_t	namelen;
    stat_T	st;
    صحيح		ret, link = FALSE;
    varnumber_T	اندازه;
    char_u	permbuf[] = "---------";
    string_T	q = {عدم, 0};
    بنية passwd *pw;
    بنية ډله  *gr;

    توکی = dict_alloc();
    إذا (توکی == عدم)
	أرجع عدم;
    توکی->dv_refcount++;

    pathlen = STRLEN(لار);
    namelen = STRLEN(name);
    len = pathlen + 1 + namelen + 1;
    p = alloc(len);
    إذا (p == عدم)
	اذهب theend;
    إذا (pathlen > 0 && لار[pathlen - 1] == '/')
	vim_snprintf(p, len, "%s%s", لار, name);
    وإلا
	vim_snprintf(p, len, "%s/%s", لار, name);
    ret = mch_lstat(p, &st);
    إذا (ret >= 0 && S_ISLNK(st.st_ډېرپېښ))
    {
	link = TRUE;
	ret = mch_stat(p, &st);
	إذا (ret < 0)
	    STR_LITERAL_سېټ(q, "link");
    }
    vim_free(p);

    إذا (dict_جمع_کړه_string_len(توکی, "name", name, (صحيح)namelen) == FAIL)
	اذهب theend;

    إذا (ret >= 0)
    {
	اندازه = (varnumber_T)st.st_size;
	إذا (S_ISDIR(st.st_ډېرپېښ))
	    اندازه = 0;
	// non-perfect check for overflow
	وإلا إذا ((off_T)اندازه != (off_T)st.st_size)
	    اندازه = -2;
	إذا (dict_جمع_کړه_شمېره(توکی, "size", اندازه) == FAIL)
	    اذهب theend;
	إذا (dict_جمع_کړه_شمېره(توکی, "time", (varnumber_T)st.st_mtime) == FAIL)
	    اذهب theend;

	إذا (link)
	{
	    إذا (S_ISDIR(st.st_ډېرپېښ))
		STR_LITERAL_سېټ(q, "linkd");
	    وإلا
		STR_LITERAL_سېټ(q, "link");
	}
	وإلا
	{
	    q.string = getftypest(&st);
	    q.اوږدتیا = STRLEN(q.string);
	}
	إذا (dict_جمع_کړه_string_len(توکی, "type", q.string, (صحيح)q.اوږدتیا) == FAIL)
	    اذهب theend;
	إذا (dict_جمع_کړه_string(توکی, "perm", getfpermst(&st, permbuf)) == FAIL)
	    اذهب theend;

	pw = getpwuid(st.st_uid);
	إذا (pw == عدم)
	    STR_LITERAL_سېټ(q, "");
	وإلا
	{
	    q.string = (char_u *)pw->pw_name;
	    q.اوږدتیا = STRLEN(q.string);
	}
	إذا (dict_جمع_کړه_string_len(توکی, "user", q.string, (صحيح)q.اوږدتیا) == FAIL)
	    اذهب theend;
#  إذا !defined(VMS) || (defined(VMS) && defined(HAVE_XOS_R_H))
	gr = getgrgid(st.st_gid);
	إذا (gr == عدم)
	    STR_LITERAL_سېټ(q, "");
	وإلا
	{
	    q.string = (char_u *)gr->gr_name;
	    q.اوږدتیا = STRLEN(q.string);
	}
#  endif
	إذا (dict_جمع_کړه_string_len(توکی, "group", q.string, (صحيح)q.اوږدتیا) == FAIL)
	    اذهب theend;
    }
    وإلا
    {
	إذا (dict_جمع_کړه_شمېره(توکی, "size", -1) == FAIL)
	    اذهب theend;
	إذا (dict_جمع_کړه_شمېره(توکی, "time", -1) == FAIL)
	    اذهب theend;
	إذا (q.string == عدم)
	{
	    إذا (dict_جمع_کړه_string_len(توکی, "type", (char_u *)"", 0) == FAIL)
		اذهب theend;
	}
	وإلا
	{
	    إذا (dict_جمع_کړه_string_len(توکی, "type", q.string, (صحيح)q.اوږدتیا) == FAIL)
		اذهب theend;
	}
	إذا (dict_جمع_کړه_string_len(توکی, "perm", (char_u *)"", 0) == FAIL)
	    اذهب theend;
	إذا (dict_جمع_کړه_string_len(توکی, "user", (char_u *)"", 0) == FAIL)
	    اذهب theend;
	إذا (dict_جمع_کړه_string_len(توکی, "group", (char_u *)"", 0) == FAIL)
	    اذهب theend;
    }
    أرجع توکی;

theend:
    dict_unref(توکی);
    أرجع عدم;
}
# endif

    ساكن صحيح
compare_readdirex_توکی(ثابت فراغ *p1, ثابت فراغ *p2)
{
    char_u  *name1, *name2;

    name1 = dict_get_string(*(dict_T**)p1, "name", FALSE);
    name2 = dict_get_string(*(dict_T**)p2, "name", FALSE);
    إذا (readdirex_ترتیبول == READDIR_ترتیبول_BYTE)
	أرجع STRCMP(name1, name2);
    إذا (readdirex_ترتیبول == READDIR_ترتیبول_IC)
	أرجع STRICMP(name1, name2);

    أرجع STRCOLL(name1, name2);
}

    ساكن صحيح
compare_readdir_توکی(ثابت فراغ *s1, ثابت فراغ *s2)
{
    إذا (readdirex_ترتیبول == READDIR_ترتیبول_BYTE)
	أرجع STRCMP(*(حرف **)s1, *(حرف **)s2);
    إذا (readdirex_ترتیبول == READDIR_ترتیبول_IC)
	أرجع STRICMP(*(حرف **)s1, *(حرف **)s2);

    أرجع STRCOLL(*(حرف **)s1, *(حرف **)s2);
}
#د_که_پای

#إذا defined(TEMPDIRNAMES) || defined(FEAT_EVAL)
/*
 * Core part of "readdir()" and "readdirex()" function.
 * Retrieve the list of files/directories of "path" into "gap".
 * If "withattr" is TRUE, retrieve the names and their attributes.
 * If "withattr" is FALSE, retrieve the names only.
 * Return OK for success, FAIL for failure.
 */
    صحيح
readdir_core(
    garray_T	*gap,
    char_u	*لار,
    صحيح		withattr ناکارول_شوی,
    فراغ	*context,
    صحيح		(*checkitem)(فراغ *context, فراغ *توکی),
    صحيح		ترتیبول)
{
    صحيح			failed = FALSE;
    char_u		*p;
# ifdef MSWIN
    char_u		*buf;
    صحيح			ok;
    HANDLE		h_پیدا_کړه = INVALID_HANDLE_ارزښت;
    WIN32_پیدا_کړه_DATAW    wfd;
    WCHAR		*wn = عدم;	// UTF-16 name, NULL when not used.
    char_u		*p_پای_ټکی;
# وإلا
    DIR			*dirp;
    بنية dirent	*dp;
# endif

    ga_init2(gap, حجم(فراغ *), 20);

# ifdef FEAT_EVAL
#  define FREE_توکی(توکی)   افعل { \
	إذا (withattr) \
	    dict_unref((dict_T*)(توکی)); \
	وإلا \
	    vim_free(توکی); \
    } طالما (0)

    readdirex_ترتیبول = READDIR_ترتیبول_BYTE;
# وإلا
#  define FREE_توکی(توکی)   vim_free(توکی)
# endif

# ifdef MSWIN
    buf = alloc(MAXPATHL);
    إذا (buf == عدم)
	أرجع FAIL;
    STRNCPY(buf, لار, MAXPATHL-5);
    p = p_پای_ټکی = buf + STRLEN(buf);
    MB_PTR_شا(buf, p);
    إذا (*p == '\\' || *p == '/')
	p_پای_ټکی = p;
    STRCPY(p_پای_ټکی, "\\*");

    wn = enc_منزل_utf16(buf, عدم);
    إذا (wn != عدم)
	h_پیدا_کړه = پیدا_کړه_لومړی_File_W(wn, &wfd);
    ok = (h_پیدا_کړه != INVALID_HANDLE_ارزښت);
    إذا (!ok)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), لار);
    }
    وإلا
    {
	طالما (ok)
	{
	    صحيح	    ignore;
	    فراغ    *توکی;
	    WCHAR   *wp;

	    wp = wfd.cFileName;
	    ignore = wp[0] == L'.' &&
		    (wp[1] == NUL ||
		     (wp[1] == L'.' && wp[2] == NUL));
	    إذا (ignore)
	    {
		ok = پیدا_کړه_بل_File_W(h_پیدا_کړه, &wfd);
		استمر;
	    }
#  ifdef FEAT_EVAL
	    إذا (withattr)
		توکی = (فراغ*)create_readdirex_توکی(&wfd);
	    وإلا
#  endif
		توکی = (فراغ*)utf16_منزل_enc(wfd.cFileName, عدم);
	    إذا (توکی == عدم)
	    {
		failed = TRUE;
		اكسر;
	    }

	    إذا (!ignore && checkitem != عدم)
	    {
		صحيح r = checkitem(context, توکی);

		إذا (r < 0)
		{
		    FREE_توکی(توکی);
		    اكسر;
		}
		إذا (r == 0)
		    ignore = TRUE;
	    }

	    إذا (!ignore)
	    {
		إذا (ga_grow(gap, 1) == OK)
		    ((فراغ**)gap->ga_معلومات)[gap->ga_len++] = توکی;
		وإلا
		{
		    failed = TRUE;
		    FREE_توکی(توکی);
		    اكسر;
		}
	    }
	    وإلا
		FREE_توکی(توکی);

	    ok = پیدا_کړه_بل_File_W(h_پیدا_کړه, &wfd);
	}
	پیدا_کړه_Close(h_پیدا_کړه);
    }

    vim_free(buf);
    vim_free(wn);
# وإلا	// MSWIN
    dirp = opendir((حرف *)لار);
    إذا (dirp == عدم)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), لار);
    }
    وإلا
    {
	لكل (;;)
	{
	    صحيح	    ignore;
	    فراغ    *توکی;

	    dp = readdir(dirp);
	    إذا (dp == عدم)
		اكسر;
	    p = (char_u *)dp->d_name;

	    ignore = p[0] == '.' &&
		    (p[1] == NUL ||
		     (p[1] == '.' && p[2] == NUL));
	    إذا (ignore)
		استمر;
#  ifdef FEAT_EVAL
	    إذا (withattr)
		توکی = (فراغ*)create_readdirex_توکی(لار, p);
	    وإلا
#  endif
		توکی = (فراغ*)vim_strsave(p);
	    إذا (توکی == عدم)
	    {
		failed = TRUE;
		اكسر;
	    }

	    إذا (checkitem != عدم)
	    {
		صحيح r = checkitem(context, توکی);

		إذا (r < 0)
		{
		    FREE_توکی(توکی);
		    اكسر;
		}
		إذا (r == 0)
		    ignore = TRUE;
	    }

	    إذا (!ignore)
	    {
		إذا (ga_grow(gap, 1) == OK)
		    ((فراغ**)gap->ga_معلومات)[gap->ga_len++] = توکی;
		وإلا
		{
		    failed = TRUE;
		    FREE_توکی(توکی);
		    اكسر;
		}
	    }
	    وإلا
		FREE_توکی(توکی);
	}

	closedir(dirp);
    }
# endif	// MSWIN

# undef FREE_توکی

    إذا (!failed && gap->ga_len > 0 && ترتیبول > READDIR_ترتیبول_NONE)
    {
# ifdef FEAT_EVAL
	readdirex_ترتیبول = ترتیبول;
	إذا (withattr)
	    qsort((فراغ*)gap->ga_معلومات, (size_t)gap->ga_len, حجم(dict_T*),
		    compare_readdirex_توکی);
	وإلا
	    qsort((فراغ*)gap->ga_معلومات, (size_t)gap->ga_len, حجم(char_u *),
		    compare_readdir_توکی);
# وإلا
	    ترتیبول_مزي_ګانې((char_u **)gap->ga_معلومات, gap->ga_len);
# endif
    }

    أرجع failed ? FAIL : OK;
}

/*
 * Delete "name" and everything in it, recursively.
 * return 0 for success, -1 if some file was not deleted.
 */
    صحيح
delete_recursive(char_u *name)
{
    صحيح پایله = 0;

    // A symbolic link to a directory itself is deleted, not the directory it
    // points to.
    إذا (
# إذا defined(UNIX) || defined(MSWIN)
	 mch_isrealdir(name)
# وإلا
	 mch_isdir(name)
# endif
	    )
    {
	char_u	    *توانیز_تابع = vim_strsave(name);
	garray_T    ga;

	إذا (توانیز_تابع == عدم)
	    أرجع -1;
	إذا (readdir_core(&ga, توانیز_تابع, FALSE, عدم, عدم, READDIR_ترتیبول_NONE) == OK)
	{
	    صحيح	len = vim_snprintf((حرف *)NameBuff, MAXPATHL, "%s/", توانیز_تابع);
	    صحيح	i;

	    لكل (i = 0; i < ga.ga_len; ++i)
	    {
		vim_snprintf((حرف *)NameBuff + len, MAXPATHL - len, "%s", ((char_u **)ga.ga_معلومات)[i]);
		إذا (delete_recursive(NameBuff) != 0)
		    // Remember the failure but continue deleting any further
		    // entries.
		    پایله = -1;
	    }
	    ga_clear_مزي_ګانې(&ga);
	    إذا (mch_rmdir(توانیز_تابع) != 0)
		پایله = -1;
	}
	وإلا
	    پایله = -1;
	vim_free(توانیز_تابع);
    }
    وإلا
	پایله = mch_remove(name) == 0 ? 0 : -1;

    أرجع پایله;
}
#د_که_پای

#إذا defined(TEMPDIRNAMES)
ساكن اوږد	لنډمهاله_شمېر = 0;		// Temp filename counter.

# إذا defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
/*
 * Open temporary directory and take file lock to prevent
 * to be auto-cleaned.
 */
   ساكن فراغ
vim_opentempdir(فراغ)
{
    DIR *dp = عدم;

    إذا (vim_tempdir_dp != عدم)
	أرجع;

    dp = opendir((ثابت حرف*)vim_tempdir);
    إذا (dp == عدم)
	أرجع;

    vim_tempdir_dp = dp;
    flock(dirfd(vim_tempdir_dp), LOCK_SH);
}

/*
 * Close temporary directory - it automatically release file lock.
 */
   ساكن فراغ
vim_closetempdir(فراغ)
{
    إذا (vim_tempdir_dp == عدم)
	أرجع;

    closedir(vim_tempdir_dp);
    vim_tempdir_dp = عدم;
}

/*
 * Return true if the temp directory we created is gone.
 */
    ساكن bool
vim_tempdir_gone(فراغ)
{
    stat_T	st;

    إذا (vim_tempdir_dp == عدم)
	أرجع false;
    أرجع fstat(dirfd(vim_tempdir_dp), &st) < 0 || st.st_nlink == 0;
}
# endif

/*
 * Delete the temp directory and all files it contains.
 */
    فراغ
vim_deltempdir(فراغ)
{
    إذا (vim_tempdir == عدم)
	أرجع;

# إذا defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
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
    ساكن فراغ
vim_settempdir(char_u *tempdir)
{
    char_u	*buf;
    size_t	buflen;

    buf = alloc(MAXPATHL + 2);
    إذا (buf == عدم)
	أرجع;

    إذا (vim_ډک_Name(tempdir, buf, MAXPATHL, FALSE) == FAIL)
	STRCPY(buf, tempdir);
    buflen = STRLEN(buf);
    إذا (!after_pathsep(buf, buf + buflen))
    {
	STRCPY(buf + buflen, PATHSEPSTR);
	buflen += حجم(PATHSEP);
    }
    vim_tempdir = vim_strnsave(buf, buflen);
# إذا defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    vim_opentempdir();
# endif
    vim_free(buf);
}
#د_که_پای

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
    صحيح	    extra_char ناکارول_شوی,  // char to use in the name instead of '?'
    صحيح	    keep ناکارول_شوی)
{
#که_تعریف_شوی USE_TMPNAM
    char_u	itmp[L_tmpnam];	// use tmpnam()
#وإلاإذا defined(MSWIN)
    WCHAR	itmp[TEMPNAMELEN];
#وإلا
    char_u	itmp[TEMPNAMELEN];
#د_که_پای

#که_تعریف_شوی TEMPDIRNAMES
    ساكن حرف	*(tempdirs[]) = {TEMPDIRNAMES};
    صحيح		i;
# ifndef EEXIST
    stat_T	st;
# endif

# إذا defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    // if the temp directory is gone, force re-creation of it
    إذا (vim_tempdir != عدم && vim_tempdir_gone())
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
    إذا (vim_tempdir == عدم)
    {
	/*
	 * Try the entries in TEMPDIRNAMES to create the temp directory.
	 */
	لكل (i = 0; i < (صحيح)ARRAY_اوږدتیا(tempdirs); ++i)
	{
# ifndef HAVE_MKDTEMP
	    اوږد	nr;
	    اوږد	off;
# endif
	    size_t	itmplen;

	    // Expand $TMP, leave room for "/v1100000/999999999".
	    // Skip the directory check if the expansion fails.
	    itmplen = expand_env((char_u *)tempdirs[i], itmp, TEMPNAMELEN - 20);
	    إذا (itmp[0] != '$' && mch_isdir(itmp))
	    {
		// directory exists
		إذا (!after_pathsep(itmp, itmp + itmplen))
		{
		    STRCPY(itmp + itmplen, PATHSEPSTR);
		    itmplen += حجم(PATHSEP);
		}

# ifdef HAVE_MKDTEMP
		{
#  إذا defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    mode_t	umask_save = umask(077);
#  endif
		    // Leave room for filename
		    STRCPY(itmp + itmplen, "vXXXXXX");
		    itmplen += STRLEN_LITERAL("vXXXXXX");
		    إذا (mkdtemp((حرف *)itmp) != عدم)
			vim_settempdir(itmp);
#  إذا defined(UNIX) || defined(VMS)
		    (فراغ)umask(umask_save);
#  endif
		}
# وإلا
		// Get an arbitrary number of up to 6 digits.  When it's
		// unlikely that it already exists it will be faster,
		// otherwise it doesn't matter.  The use of mkdir() avoids any
		// security problems because of the predictable number.
		nr = (mch_get_pid() + (اوږد)وخت(عدم)) % 1000000L;

		// Try up to 10000 different values until we find a name that
		// doesn't exist.
		لكل (off = 0; off < 10000L; ++off)
		{
		    صحيح		r;
#  إذا defined(UNIX) || defined(VMS)
		    mode_t	umask_save;
#  endif

		    vim_snprintf((حرف *)itmp + itmplen, حجم(itmp) - itmplen, "v%ld", nr + off);
#  ifndef EEXIST
		    // If mkdir() does not set errno to EEXIST, check for
		    // existing file here.  There is a race condition then,
		    // although it's fail-safe.
		    إذا (mch_stat((حرف *)itmp, &st) >= 0)
			استمر;
#  endif
#  إذا defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    umask_save = umask(077);
#  endif
		    r = vim_mkdir(itmp, 0700);
#  إذا defined(UNIX) || defined(VMS)
		    (فراغ)umask(umask_save);
#  endif
		    إذا (r == 0)
		    {
			vim_settempdir(itmp);
			اكسر;
		    }
#  ifdef EEXIST
		    // If the mkdir() didn't fail because the file/dir exists,
		    // we probably can't create any dir here, try another
		    // place.
		    إذا (errno != EEXIST)
#  endif
			اكسر;
		}
# endif // HAVE_MKDTEMP
		إذا (vim_tempdir != عدم)
		    اكسر;
	    }
	}
    }

    إذا (vim_tempdir != عدم)
    {
	// There is no need to check if the file exists, because we own the
	// directory and nobody else creates a file in it.
	صحيح itmplen = vim_snprintf((حرف *)itmp, حجم(itmp), "%s%ld", vim_tempdir, لنډمهاله_شمېر++);
	أرجع vim_strnsave(itmp, (size_t)itmplen);
    }

    أرجع عدم;

#وإلا // TEMPDIRNAMES
    char_u	*p;

# ifdef MSWIN
    WCHAR	wsz_لنډمهاله_File[MAX_لار_2 + 1];
    WCHAR	buf4[4];
    WCHAR	*chartab = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char_u	*retval;
    char_u	*shname;
    اوږد	i;

    wcscpy(itmp, L"");
    إذا (Get_لنډمهاله_لار_W(MAX_لار_2, wsz_لنډمهاله_File) == 0)
    {
	wsz_لنډمهاله_File[0] = L'.';	// GetTempPathW() failed, use current dir
	wsz_لنډمهاله_File[1] = L'\\';
	wsz_لنډمهاله_File[2] = NUL;
    }
    wcscpy(buf4, L"VIM");

    // randomize the name to avoid collisions
    i = mch_get_pid() + extra_char;
    buf4[1] = chartab[i % 36];
    buf4[2] = chartab[(i / 36) % 36];
    إذا (Get_لنډمهاله_File_Name_W(wsz_لنډمهاله_File, buf4, 0, itmp) == 0)
	أرجع عدم;
    إذا (!keep)
	// GetTempFileNameW() will create the file, we don't want that
	(فراغ)DeleteFileW(itmp);

    // Backslashes in a temp file name cause problems when filtering with
    // "sh".  NOTE: This also checks 'shellcmdflag' to help those people who
    // didn't set 'shellslash' but only if not using PowerShell.
    retval = utf16_منزل_enc(itmp, عدم);
    إذا (retval == عدم)
	أرجع عدم;
    shname = gettail(p_sh);
    إذا ((*p_shcf == '-' && !(مزي_پلټنه((حرف *)shname, "powershell") != عدم
			     || مزي_پلټنه((حرف *)shname, "pwsh") != عدم ))
								    || p_ssl)
	لكل (p = retval; *p; ++p)
	    إذا (*p == '\\')
		*p = '/';
    أرجع retval;

# وإلا // MSWIN
    صحيح		itmplen;

#  ifdef USE_TMPNAM
    // tmpnam() will make its own name
    p = tmpnam((حرف *)itmp);
    إذا (p == عدم || *p == NUL)
	أرجع عدم;
#  وإلا

#   ifdef VMS_TEMPNAM
    // mktemp() is not working on VMS.  It seems to be
    // a do-nothing function. Therefore we use tempnam().
    vim_snprintf((حرف *)itmp, حجم(itmp), "VIM%c", extra_char);
    p = (char_u *)tempnam("tmp:", (حرف *)itmp);
    إذا (p != عدم)
    {
	// VMS will use '.LIS' if we don't explicitly specify an extension,
	// and VIM will then be unable to find the file later
	itmplen = vim_snprintf((حرف *)itmp, حجم(itmp), "%s.txt", p);
	حرر(p);
    }
    وإلا
	أرجع عدم;
#   وإلا
    STRCPY(itmp, TEMPNAME);
    itmplen = STRLEN_LITERAL(TEMPNAME);
    إذا ((p = vim_strchr(itmp, '?')) != عدم)
	*p = extra_char;
    إذا (mktemp((حرف *)itmp) == عدم)
	أرجع عدم;
#   endif
#  endif

    أرجع vim_strnsave(itmp, (size_t)itmplen);
# endif // MSWIN
#د_که_پای // TEMPDIRNAMES
}

#إذا defined(BACKSLASH_دننه_FILENAME)
/*
 * Convert all backslashes in fname to forward slashes in-place, unless when
 * it looks like a URL.
 */
    فراغ
forward_slash(char_u *fname)
{
    char_u	*p;

    إذا (لار_with_url(fname))
	أرجع;
    لكل (p = fname; *p != NUL; ++p)
	// The Big5 encoding can have '\' in the trail byte.
	إذا (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
	    ++p;
	وإلا إذا (*p == '\\')
	    *p = '/';
}
#د_که_پای

/*
 * Try matching a filename with a "pattern" ("prog" is NULL), or use the
 * precompiled regprog "prog" ("pattern" is NULL).  That avoids calling
 * vim_regcomp() often.
 * Used for autocommands and 'wildignore'.
 * Returns TRUE if there is a match, FALSE otherwise.
 */
    صحيح
سمون_file_pat(
    char_u	*نمونه,		// pattern to match with
    regprog_T	**prog,			// pre-compiled regprog or NULL
    char_u	*fname,			// full path of file name
    char_u	*sfname,		// short file name or NULL
    char_u	*لکۍ,			// tail of path
    صحيح		allow_dirs)		// allow matching with dir
{
    regmatch_T	regmatch;
    صحيح		پایله = FALSE;

    regmatch.rm_ic = p_fic; // ignore case if 'fileignorecase' is set
    إذا (prog != عدم)
	regmatch.regprog = *prog;
    وإلا
	regmatch.regprog = vim_regcomp(نمونه, RE_MAGIC);

    /*
     * Try for a match with the pattern with:
     * 1. the full file name, when the pattern has a '/'.
     * 2. the short file name, when the pattern has a '/'.
     * 3. the tail of the file name, when the pattern has no '/'.
     */
    إذا (regmatch.regprog != عدم
	     && ((allow_dirs
		     && (vim_regexec(&regmatch, fname, (colnr_T)0)
			 || (sfname != عدم
			     && vim_regexec(&regmatch, sfname, (colnr_T)0))))
		 || (!allow_dirs && vim_regexec(&regmatch, لکۍ, (colnr_T)0))))
	پایله = TRUE;

    إذا (prog != عدم)
	*prog = regmatch.regprog;
    وإلا
	vim_regfree(regmatch.regprog);
    أرجع پایله;
}

/*
 * Return TRUE if a file matches with a pattern in "list".
 * "list" is a comma-separated list of patterns, like 'wildignore'.
 * "sfname" is the short file name or NULL, "ffname" the long file name.
 */
    صحيح
سمون_file_list(char_u *list, char_u *sfname, char_u *ffname)
{
    char_u	buf[MAXPATHL];
    char_u	*لکۍ;
    char_u	*regpat;
    حرف	allow_dirs;
    صحيح		سمون;
    char_u	*p;

    لکۍ = gettail(sfname);

    // try all patterns in 'wildignore'
    p = list;
    طالما (*p)
    {
	copy_option_part(&p, buf, MAXPATHL, ",");
	regpat = file_pat_منزل_reg_pat(buf, عدم, &allow_dirs, FALSE);
	إذا (regpat == عدم)
	    اكسر;
	سمون = سمون_file_pat(regpat, عدم, ffname, sfname,
						       لکۍ, (صحيح)allow_dirs);
	vim_free(regpat);
	إذا (سمون)
	    أرجع TRUE;
    }
    أرجع FALSE;
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
file_pat_منزل_reg_pat(
    char_u	*pat,
    char_u	*pat_پای_ټکی,	// first char after pattern or NULL
    حرف	*allow_dirs,	// Result passed back out in here
    صحيح		no_bslash ناکارول_شوی) // Don't use a backward slash as pathsep
{
    صحيح		اندازه = 2; // '^' at start, '$' at end
    char_u	*endp;
    char_u	*reg_pat;
    char_u	*p;
    صحيح		i;
    صحيح		nested = 0;
    صحيح		جمع_کړه_dollar = TRUE;

    إذا (allow_dirs != عدم)
	*allow_dirs = FALSE;
    إذا (pat_پای_ټکی == عدم)
	pat_پای_ټکی = pat + STRLEN(pat);

    لكل (p = pat; p < pat_پای_ټکی; p++)
    {
	بدل (*p)
	{
	    حالة '*':
	    حالة '.':
	    حالة ',':
	    حالة '{':
	    حالة '}':
	    حالة '~':
		اندازه += 2;	// extra backslash
		اكسر;
#که_تعریف_شوی BACKSLASH_دننه_FILENAME
	    حالة '\\':
	    حالة '/':
		اندازه += 4;	// could become "[\/]"
		اكسر;
#د_که_پای
	    افتراضي:
		اندازه++;
		إذا (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		{
		    ++p;
		    ++اندازه;
		}
		اكسر;
	}
    }
    reg_pat = alloc(اندازه + 1);
    إذا (reg_pat == عدم)
	أرجع عدم;

    i = 0;

    إذا (pat[0] == '*')
	طالما (pat[0] == '*' && pat < pat_پای_ټکی - 1)
	    pat++;
    وإلا
	reg_pat[i++] = '^';
    endp = pat_پای_ټکی - 1;
    إذا (endp >= pat && *endp == '*')
    {
	طالما (endp - pat > 0 && *endp == '*')
	    endp--;
	جمع_کړه_dollar = FALSE;
    }
    لكل (p = pat; *p && nested >= 0 && p <= endp; p++)
    {
	بدل (*p)
	{
	    حالة '*':
		reg_pat[i++] = '.';
		reg_pat[i++] = '*';
		طالما (p[1] == '*')	// "**" matches like "*"
		    ++p;
		اكسر;
	    حالة '.':
	    حالة '~':
		reg_pat[i++] = '\\';
		reg_pat[i++] = *p;
		اكسر;
	    حالة '?':
		reg_pat[i++] = '.';
		اكسر;
	    حالة '\\':
		إذا (p[1] == NUL)
		    اكسر;
#که_تعریف_شوی BACKSLASH_دننه_FILENAME
		إذا (!no_bslash)
		{
		    // translate:
		    // "\x" to "\\x"  e.g., "dir\file"
		    // "\*" to "\\.*" e.g., "dir\*.c"
		    // "\?" to "\\."  e.g., "dir\??.c"
		    // "\+" to "\+"   e.g., "fileX\+.c"
		    إذا ((vim_isfilec(p[1]) || p[1] == '*' || p[1] == '?')
			    && p[1] != '+')
		    {
			reg_pat[i++] = '[';
			reg_pat[i++] = '\\';
			reg_pat[i++] = '/';
			reg_pat[i++] = ']';
			إذا (allow_dirs != عدم)
			    *allow_dirs = TRUE;
			اكسر;
		    }
		}
#د_که_پای
		// Undo escaping from ExpandEscape():
		// foo\?bar -> foo?bar
		// foo\%bar -> foo%bar
		// foo\,bar -> foo,bar
		// foo\ bar -> foo bar
		// Don't unescape \, * and others that are also special in a
		// regexp.
		// An escaped { must be unescaped since we use magic not
		// verymagic.  Use "\\\{n,m\}"" to get "\{n,m}".
		إذا (*++p == '?'
#که_تعریف_شوی BACKSLASH_دننه_FILENAME
			&& no_bslash
#د_که_پای
			)
		    reg_pat[i++] = '?';
		وإلا
		    إذا (*p == ',' || *p == '%' || *p == '#'
			       || vim_isspace(*p) || *p == '{' || *p == '}')
			reg_pat[i++] = *p;
		    وإلا إذا (*p == '\\' && p[1] == '\\' && p[2] == '{')
		    {
			reg_pat[i++] = '\\';
			reg_pat[i++] = '{';
			p += 2;
		    }
		    وإلا
		    {
			إذا (allow_dirs != عدم && vim_ispathsep(*p)
#که_تعریف_شوی BACKSLASH_دننه_FILENAME
				&& (!no_bslash || *p != '\\')
#د_که_پای
				)
			    *allow_dirs = TRUE;
			reg_pat[i++] = '\\';
			إذا (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
			    reg_pat[i++] = *p++;
			reg_pat[i++] = *p;
		    }
		اكسر;
#که_تعریف_شوی BACKSLASH_دننه_FILENAME
	    حالة '/':
		reg_pat[i++] = '[';
		reg_pat[i++] = '\\';
		reg_pat[i++] = '/';
		reg_pat[i++] = ']';
		إذا (allow_dirs != عدم)
		    *allow_dirs = TRUE;
		اكسر;
#د_که_پای
	    حالة '{':
		reg_pat[i++] = '\\';
		reg_pat[i++] = '(';
		nested++;
		اكسر;
	    حالة '}':
		reg_pat[i++] = '\\';
		reg_pat[i++] = ')';
		--nested;
		اكسر;
	    حالة ',':
		إذا (nested)
		{
		    reg_pat[i++] = '\\';
		    reg_pat[i++] = '|';
		}
		وإلا
		    reg_pat[i++] = ',';
		اكسر;
	    افتراضي:
		إذا (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		    reg_pat[i++] = *p++;
		وإلا إذا (allow_dirs != عدم && vim_ispathsep(*p))
		    *allow_dirs = TRUE;
		reg_pat[i++] = *p;
		اكسر;
	}
    }
    إذا (جمع_کړه_dollar)
	reg_pat[i++] = '$';
    reg_pat[i] = NUL;
    إذا (nested != 0)
    {
	إذا (nested < 0)
	    emsg(_(e_missing_open_curly));
	وإلا
	    emsg(_(e_missing_close_curly));
	VIM_CLEAR(reg_pat);
    }
    أرجع reg_pat;
}

#إذا defined(EINTR)
/*
 * Version of read() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    اوږد
read_eintr(صحيح fd, فراغ *buf, size_t bufsize)
{
    اوږد ret;

    لكل (;;)
    {
	ret = vim_read(fd, buf, bufsize);
	إذا (ret >= 0 || errno != EINTR)
	    اكسر;
    }
    أرجع ret;
}

/*
 * Version of write() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    اوږد
write_eintr(صحيح fd, فراغ *buf, size_t bufsize)
{
    اوږد    ret = 0;
    اوږد    wlen;

    // Repeat the write() so long it didn't fail, other than being interrupted
    // by a signal.
    طالما (ret < (اوږد)bufsize)
    {
	wlen = vim_write(fd, (حرف *)buf + ret, bufsize - ret);
	إذا (wlen < 0)
	{
	    إذا (errno != EINTR)
		اكسر;
	}
	وإلا
	    ret += wlen;
    }
    أرجع ret;
}
#د_که_پای
