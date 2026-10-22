/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 * VMS port			by Henk Elbers
 * VMS deport			by Zoltan Arpadffy
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

// define _generic_64 for use in time functions
#إذا !defined(VAX)
# include <gen64def.h>
#وإلا
// based on Alpha's gen64def.h; the file is absent on VAX
تعريفنوع بنية _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	unsigned صحيح gen64$l_longword [2];   // ...two 32-bit values, or
	unsigned short صحيح gen64$w_word [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#endif

تعريفنوع بنية
{
    حرف	class;
    حرف	type;
    short	width;
    اتحاد
    {
	بنية
	{
	    حرف	_basic[3];
	    حرف	length;
	}	y;
	صحيح	basic;
    }	x;
    صحيح		extended;
}	TT_MODE;

تعريفنوع بنية
{
    short	buflen;
    short	itemcode;
    حرف	*bufadrs;
    صحيح		*retlen;
}	ITEM;

تعريفنوع بنية
{
    ITEM	equ;
    صحيح		nul;
}	ITMLST1;

تعريفنوع بنية
{
    ITEM	index;
    ITEM	string;
    صحيح	nul;
}	ITMLST2;

ساكن TT_MODE	orgmode;
ساكن short	iochan;			// TTY I/O channel
ساكن short	iosb[4];		// IO status block

ساكن صحيح vms_match_ژمارە = 0;
ساكن صحيح vms_match_free = 0;
ساكن char_u **vms_fmatch = عدم;
ساكن حرف *Fspec_Rms;		       // rms file spec, passed implicitly between routines



ساكن TT_MODE	get_tty(فراغ);
ساكن فراغ	set_tty(صحيح row, صحيح col);

#define EXPL_ALLOC_INC 64

#define EQN(S1,S2,LN) (strncmp(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) افعل { طالما (Str[1] == '/') ++Str; } طالما (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    ساكن فراغ
vul_desc(DESC *des, حرف *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_pointer = str;
    des->dsc$w_length = str ? strlen(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    ساكن فراغ
vul_item(ITEM *itm, short len, short cod, حرف *adr, صحيح *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    فراغ
mch_settmode(tmode_T tmode)
{
    صحيح	status;

    إذا ( tmode == TMODE_RAW )
	set_tty(0, 0);
    وإلا
    {
	بدل (orgmode.width)
	{
	    حالة 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	اكسر;
	    حالة 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	اكسر;
	    افتراضي:	اكسر;
	}
	out_flush();
	status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, حجم(TT_MODE), 0,0,0,0);
	إذا (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	    أرجع;
	(فراغ)sys$dassgn(iochan);
	iochan = 0;
    }
}

    ساكن فراغ
set_tty(صحيح row, صحيح col)
{
    صحيح		    status;
    TT_MODE	    newmode;		// New TTY mode bits
    ساكن short    first_time = TRUE;

    إذا (first_time)
    {
	orgmode = get_tty();
	first_time = FALSE;
    }
    newmode = get_tty();
    إذا (col)
	newmode.width		 = col;
    إذا (row)
	newmode.x.y.length       = row;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, حجم(newmode), 0, 0, 0, 0);
    إذا (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	أرجع;
}

    ساكن TT_MODE
get_tty(فراغ)
{

    ساكن $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    صحيح		status;
    TT_MODE	tt_mode;

    إذا (!iochan)
	status = sys$assign(&odsc,&iochan,0,0);

    status = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, حجم(tt_mode), 0, 0, 0, 0);
    إذا (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.length	= 0;
	tt_mode.extended	= 0;
    }
    أرجع(tt_mode);
}

/*
 * Get the current window size in Rows and Columns.
 */
    صحيح
mch_get_shellsize(فراغ)
{
    TT_MODE	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    أرجع OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    فراغ
mch_set_shellsize(فراغ)
{
    set_tty(Rows, Columns);
    بدل (Columns)
    {
	حالة 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	اكسر;
	حالة 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	اكسر;
	افتراضي:	اكسر;
    }
    out_flush();
    screen_start();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    ساكن حرف		buffer[LNM$C_NAMLENGTH+1];
    char_u		*cp = عدم;
    unsigned long	attrib;
    صحيح			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    حرف		*sbuf = عدم;

    vul_desc(&d_lognam, (حرف *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.index, حجم(صحيح), LNM$_INDEX, (حرف *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, buffer, &lengte);
    itmlst.nul	= 0;
    إذا (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, عدم,&itmlst) == SS$_NORMAL)
    {
	buffer[lengte] = '\0';
	إذا (cp = alloc(lengte + 1))
	    strcpy((حرف *)cp, buffer);
	أرجع(cp);
    }
    وإلا إذا ((sbuf = getenv((حرف *)lognam)))
    {
	lengte = strlen(sbuf) + 1;
	cp = alloc(lengte);
	إذا (cp)
	    strcpy((حرف *)cp, sbuf);
	أرجع cp;
    }
    وإلا
	أرجع(عدم);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    صحيح
mch_setenv(حرف *var, حرف *بەها, صحيح x)
{
    صحيح		res, dum;
    long	attrib = 0L;
    حرف	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_item(&itmlst.equ, بەها ? strlen(بەها) : 0, بەها ? LNM$_STRING : 0,
	    بەها, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    أرجع((res == 1) ? 0 : -1);
}

    صحيح
vms_sys(حرف *cmd, حرف *out, حرف *inp)
{
    DESC	cdsc, odsc, idsc;
    long	status;

    إذا (cmd)
	vul_desc(&cdsc, cmd);
    إذا (out)
	vul_desc(&odsc, out);
    إذا (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : عدم,		// command string
	      inp ? &idsc : عدم,		// input file
	      out ? &odsc : عدم,		// output file
	      0, 0, 0, &status, 0, 0, 0, 0, 0, 0);
    أرجع status;
}

/*
 * Convert string to lowercase - most often filename
 */
    حرف *
vms_tolower( حرف *name )
{
    صحيح i,nlen = strlen(name);
    لكل (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    أرجع name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    صحيح
vms_sys_status(صحيح status)
{
    إذا (status != SS$_NORMAL && (status & STS$M_SUCCESS) == 0)
	أرجع status;		// Command failed.
    أرجع 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    صحيح
vms_read(حرف *inbuf, size_t nbytes)
{
    صحيح		status, function, len;
    TT_MODE	tt_mode;
    ITEM	itmlst[2];     // terminates on everything
    ساكن long trm_mask[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    إذا (!iochan)
	tt_mode = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_item(&itmlst[0], 0, TRM$_MODIFIERS,
	 (حرف *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_item(&itmlst[1], حجم(trm_mask), TRM$_TERM, (حرف *)&trm_mask, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, حجم(itmlst));
    len = strlen(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = strlen(inbuf); // return the total length

    أرجع len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    ساكن صحيح
vms_wproc(حرف *name, صحيح val)
{
    صحيح i;
    ساكن صحيح vms_match_alloced = 0;

    إذا (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	أرجع 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    إذا (vms_match_ژمارە == 0)
    {
	// first time through, setup some things
	إذا (عدم == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    إذا (!vms_fmatch)
		أرجع 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	وإلا
	{
	    // re-use existing space
	    vms_match_free = vms_match_alloced;
	}
    }

    // make matches look uniform
    vms_remove_version(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    لكل (i = 0; i<vms_match_ژمارە; i++)
    {
	إذا (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    أرجع 1;
    }
    إذا (--vms_match_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		حجم(حرف **) * vms_match_alloced);
	إذا (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    أرجع 0;
	}
	vms_match_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_match_ژمارە] = vim_strsave((char_u *)name);

    ++vms_match_ژمارە;
    أرجع 1;
}

/*
 *	mch_expand_wildcards	this code does wild-card pattern
 *				matching NOT using the shell
 *
 *	return OK for success, FAIL for error (you may lose some
 *	memory) and put an error message in *file.
 *
 *	num_pat	   number of input patterns
 *	pat	   array of pointers to input patterns
 *	num_file   pointer to number of matched file names
 *	file	   pointer to array of pointers to matched file names
 *
 */
    صحيح
mch_expand_wildcards(
    صحيح		ژمارە_pat,
    char_u	**pat,
    صحيح		*ژمارە_file,
    char_u	***file,
    صحيح		flags)
{
    صحيح		i, cnt = 0;
    char_u	buf[MAXPATHL];
    حرف       *result;
    صحيح		dir;
    صحيح files_alloced, files_free;

    *ژمارە_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    إذا (*file == عدم)
    {
	*ژمارە_file = 0;
	أرجع FAIL;
    }
    لكل (i = 0; i < ژمارە_pat; i++)
    {
	// expand environment var or home dir
	إذا (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	وإلا
	    STRCPY(buf,pat[i]);

	vms_match_ژمارە = 0; // reset collection counter
	result = decc$translate_vms(vms_fixfilename(buf));
	إذا ( (صحيح) result == 0 || (صحيح) result == -1  )
	{
	    cnt = 0;
	}
	وإلا
	{
	    cnt = decc$to_vms(result, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	إذا (cnt > 0)
	    cnt = vms_match_ژمارە;

	إذا (cnt < 1)
	    استمر;

	لكل (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    إذا (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		استمر;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    إذا (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		استمر;

	    // Skip files that are not executable if we check for that.
	    إذا (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], عدم, !(flags & EW_SHELLCMD)))
		استمر;

	    // allocate memory for pointers
	    إذا (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, حجم(char_u **) * files_alloced);
		إذا (*file == عدم)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *ژمارە_file = 0;
		    أرجع(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*ژمارە_file++] = vms_fmatch[i];
	}
    }
    أرجع OK;
}

    صحيح
mch_expandpath(garray_T *gap, char_u *path, صحيح flags)
{
    صحيح		i,cnt = 0;
    حرف       *result;

    vms_match_ژمارە = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    result = decc$translate_vms(vms_fixfilename(path));
    إذا ( (صحيح) result == 0 || (صحيح) result == -1  )
    {
	cnt = 0;
    }
    وإلا
    {
	cnt = decc$to_vms(result, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    إذا (cnt > 0)
	cnt = vms_match_ژمارە;
    لكل (i = 0; i < cnt; i++)
    {
	إذا (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    أرجع cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    ساكن فراغ
vms_unix_mixed_filespec(حرف *in, حرف *out)
{
    حرف *lastcolon;
    حرف *end_of_dir;
    حرف ch;
    صحيح len;
    حرف *out_str=out;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(in, ':');   // find last colon
    إذا (lastcolon != عدم)
    {
	len = lastcolon - in + 1;
	strncpy(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = عدم;	// default: no directory

    // start of directory portion
    ch = *in;
    إذا ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    }
    وإلا إذا (EQN(in, "../", 3))      // Unix parent directory?
    {
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    }
    وإلا
    {                                // not a special character
	طالما (EQN(in, "./", 2))     // Ignore Unix "current dir"
	{
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    إذا (strchr(in, '/') == عدم)     // any more Unix directories ?
    {
	strcpy(out, in);             // No - get rest of the spec
	أرجع;
    }
    وإلا
    {
	*out++ = '[';                // Yes, denote a Vms subdirectory
	ch = '.';
	--in;
	}
    }

    // if we get here, there is a directory part of the filename

    // initialize output file spec
    *out++ = ch;
    ++in;

    طالما (*in != '\0')
    {
	ch = *in;
	إذا ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	وإلا إذا (EQN(in, "../", 3))     // Unix parent directory?
	{
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	وإلا
	{
	    طالما (EQN(in, "./", 2))    // Ignore Unix "current dir"
	    {
		end_of_dir = out;
		in += 2;
		SKIP_FOLLOWING_SLASHES(in);
		ch = *in;
	    }
	}

    // Place next character into output file spec
	*out++ = ch;
	++in;
    }

    *out = '\0';            // Terminate output file spec

    إذا (end_of_dir != عدم) // Terminate directory portion
	*end_of_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    ساكن صحيح
vms_fspec_proc(حرف *fil, صحيح val)
{
    strcpy(Fspec_Rms,fil);
    أرجع(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    فراغ *
vms_fixfilename(فراغ *instring)
{
    ساكن حرف		*buf = عدم;
    ساكن size_t	buflen = 0;
    size_t		len;

    // get a big-enough buffer
    len = strlen(instring) + 1;
    إذا (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * حجم(حرف));
    }

#ifdef DEBUG
    حرف		 *tmpbuf = عدم;
    tmpbuf = ALLOC_MULT(حرف, buflen);
    strcpy(tmpbuf, instring);
#endif

    Fspec_Rms = buf;				// for decc$to_vms

    إذا (strchr(instring,'/') == عدم)
	// It is already a VMS file spec
	strcpy(buf, instring);
    وإلا إذا (strchr(instring,'"') == عدم)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	إذا ( (strchr(instring,'[') == عدم) && (strchr(instring,'<') == عدم) &&
	     (strchr(instring,']') == عدم) && (strchr(instring,'>') == عدم) &&
	     (strchr(instring,':') == عدم) )
	{
	    // It must be a truly unix fspec
	    decc$to_vms(instring, vms_fspec_proc, 0, 0);
	}
	وإلا
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    وإلا
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    أرجع buf;
}

/*
 * Remove version number from file name
 * we need it in some special cases as:
 * creating swap file name and writing new file
 */

/*
 * 2025-05-13 SMS.
 * Using $PARSE would be simpler and more accurate, if all-VMS (not
 * mixed UNIX+VMS) path were ensured.  Meanwhile, to improve (imperfect)
 * handling of extended name syntax:
 *     o All characters (up to five (32767)) after semi-colon (or last
 *       of multiple dots) must be numeric.
 *     o Caret-escaped semi-colon (^;) or dot (^.) does not delimit
 *       version.
 * Whether it makes sense to detect syntax errors here is not entirely
 * clear.  Currently, many invalid version strings are not treated as
 * version strings.  (More could be.)
 */
    فراغ
vms_remove_version(فراغ * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    صحيح		done = 0;
    صحيح		vdigits = 0;

    rp = (char_u *)fname+ strlen( (حرف *)fname)- 1;    // Rightmost char
    طالما ((done == 0) && (rp > (char_u *)fname))       // Count digits
    {
	إذا (isdigit( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	وإلا
	{
	    done = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    إذا (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	إذا (*rp == (char_u)';')
	{
	    إذا ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	وإلا إذا (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    إذا ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		done = 0;
		طالما ((done == 0) && (dp >= (char_u *)fname))
		{
		    إذا ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			إذا ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    done = 1;   // No previous dot found in name
			}
		    }
		    وإلا إذا (*dp == '.')
		    {                   // Possible dot delimiter
			إذا ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dot delimiter
			    done = 1;   // Previous dot found in name
			    *rp = '\0'; // Trim off ".nnn"
			}
		    }
		    dp--;               // Next char to right
		} // while
	    }
	}
	// Else no version found to remove
    } // if (vdigits  <= 5)

    أرجع ;
}

بنية typeahead_st {
    unsigned short numchars;
    unsigned حرف  firstchar;
    unsigned حرف  reserved0;
    unsigned long  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    صحيح
RealWaitForChar(
    صحيح		fd UNUSED, // always read from iochan
    long	msec,
    صحيح		*check_for_gpm UNUSED,
    صحيح		*interrupted)
{
    صحيح status;
    بنية _generic_64 time_curr;
    بنية _generic_64 time_diff;
    بنية _generic_64 time_out;
    unsigned صحيح convert_operation = LIB$K_DELTA_SECONDS_F;
    عائم sec =(عائم) msec/1000;

    // make sure the iochan is set
    إذا (!iochan)
	get_tty();

    إذا (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	status = sys$gettim(&time_curr);
	إذا (status != SS$_NORMAL)
	    أرجع 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#إذا __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_to_internal_time
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvts_to_internal_time // IEEE
# وإلا
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time
# endif
#وإلا
# define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time // VAX
#endif // __IEEE_FLOAT CVTS

	status = LIB_CVTX_TO_INTERNAL_TIME(
		&convert_operation, &sec, &time_diff);
	إذا (status != LIB$_NORMAL)
	    أرجع 0; // error
	// add them up
	status = lib$add_times(
		&time_curr,
		&time_diff,
		&time_out);
	إذا (status != LIB$_NORMAL)
	    أرجع 0; // error
    }

    طالما (TRUE)
    {
	// select()
	status = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	إذا (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
	    أرجع 0; // error

	إذا (typeahead.numchars)
	    أرجع 1; // ready to read

	// there's nothing to read; what now?
	إذا (msec == 0)
	{
	    // immediate time-out; return impatiently
	    أرجع 0;
	}
	وإلا إذا (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    أرجع 1; // fakeout to force a wait in vms_read()
	}
	وإلا
	{
	    // time-out needs to be checked
	    status = sys$gettim(&time_curr);
	    إذا (status != SS$_NORMAL)
		أرجع 0; // error

	    status = lib$sub_times(
		    &time_out,
		    &time_curr,
		    &time_diff);
	    إذا (status != LIB$_NORMAL)
		أرجع 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#إذا !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

تعريفنوع بنية
   {
   حرف *name;
   صحيح بەها;
   } decc_feat_t;

صحيح vms_init_done = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (حرف *)عدم, 0 } };


/* LIB$INITIALIZE initialization.
 *
 * On sufficiently recent non-VAX systems, set a collection of C RTL
 * features without using the DECC$* logical name method.
 *
 * Note: Old VAX VMS versions may suffer from a linker complaint like
 * this:
 *
 * %LINK-W-MULPSC, conflicting attributes for psect LIB$INITIALIZE
 * in module LIB$INITIALIZE file SYS$COMMON:[SYSLIB]STARLET.OLB;1
 *
 * Using a LINK options file which includes a line like this one should
 * stop this complaint:
 *
 * PSECT_ATTR=LIB$INITIALIZE,NOPIC
 */

/* vms_init()
 *
 * Uses LIB$INITIALIZE to set a collection of C RTL features without
 * requiring the user to define the corresponding logical names.
 *
 * LIB$INITIALIZE initialization function
 */

ساكن فراغ
vms_init(فراغ)
{
    // Set the global flag to indicate that LIB$INITIALIZE worked

    vms_init_done = 1;

    صحيح feat_index;
    صحيح feat_بەها;
    صحيح feat_بەها_max;
    صحيح feat_بەها_min;
    صحيح i;
    صحيح sts;

    // Loop through all items in the decc_feat_array[]
    لكل (i = 0; decc_feat_array[i].name != عدم; i++)
    {
	// Get the feature index
	feat_index = decc$feature_get_index(decc_feat_array[i].name);
	إذا (feat_index >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_بەها = decc$feature_get_بەها(feat_index, 1);
	    feat_بەها_min = decc$feature_get_بەها(feat_index, 2);
	    feat_بەها_max = decc$feature_get_بەها(feat_index, 3);

	    إذا ((decc_feat_array[i].بەها >= feat_بەها_min) && (decc_feat_array[i].بەها <= feat_بەها_max))
	    {
		// Valid value.  Set it if necessary
		إذا (feat_بەها != decc_feat_array[i].بەها)
		    sts = decc$feature_set_بەها(feat_index, 1, decc_feat_array[i].بەها);
	    }
	    وإلا
		// Invalid DECC feature value
		printf("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_بەها, feat_بەها_min, decc_feat_array[i].name, feat_بەها_max);
	}
	وإلا
	    // Invalid DECC feature name
	    printf("UNKNOWN DECC FEATURE: %s.\n", decc_feat_array[i].name);
    }
}


/* Get "vms_init()" into a valid, loaded LIB$INITIALIZE PSECT. */

# pragma nostandard

/* Establish the LIB$INITIALIZE PSECTs, with proper alignment and
 * other attributes.  Note that "nopic" is significant only on VAX.
 */
# pragma extern_model save

# pragma extern_model strict_refdef "LIB$INITIALIZE" 2, nopic, nowrt
فراغ (*ثابت x_vms_init)() = vms_init;

# pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
ثابت صحيح spare[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
صحيح LIB$INITIALIZE(فراغ);
# pragma extern_model strict_refdef
صحيح dmy_lib$initialize = (صحيح) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#endif // !defined( __VAX) && (__CRTL_VER >= 70301000)
