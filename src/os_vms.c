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

#شامل "vim.h"

// define _generic_64 for use in time functions
#اگر !defined(VAX)
# include <gen64def.h>
#وگرنه
// based on Alpha's gen64def.h; the file is absent on VAX
تعریفنوع ساختار _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	unsigned صحیح gen64$l_longword [2];   // ...two 32-bit values, or
	unsigned short صحیح gen64$w_word [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#endif

تعریفنوع ساختار
{
    نویسه	class;
    نویسه	type;
    short	width;
    اجتماع
    {
	ساختار
	{
	    نویسه	_basic[3];
	    نویسه	length;
	}	y;
	صحیح	basic;
    }	x;
    صحیح		extended;
}	TT_MODE;

تعریفنوع ساختار
{
    short	buflen;
    short	itemcode;
    نویسه	*bufadrs;
    صحیح		*retlen;
}	ITEM;

تعریفنوع ساختار
{
    ITEM	equ;
    صحیح		nul;
}	ITMLST1;

تعریفنوع ساختار
{
    ITEM	index;
    ITEM	string;
    صحیح	nul;
}	ITMLST2;

ایستا TT_MODE	orgmode;
ایستا short	iochan;			// TTY I/O channel
ایستا short	iosb[4];		// IO status block

ایستا صحیح vms_match_رقم = 0;
ایستا صحیح vms_match_free = 0;
ایستا char_u **vms_fmatch = پوچ;
ایستا نویسه *Fspec_Rms;		       // rms file spec, passed implicitly between routines



ایستا TT_MODE	get_tty(تهی);
ایستا تهی	set_tty(صحیح row, صحیح col);

#define EXPL_ALLOC_INC 64

#define EQN(S1,S2,LN) (strncmp(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) انجامبده { وقتی (Str[1] == '/') ++Str; } وقتی (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    ایستا تهی
vul_desc(DESC *des, نویسه *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_pointer = str;
    des->dsc$w_length = str ? strlen(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    ایستا تهی
vul_item(ITEM *itm, short len, short cod, نویسه *adr, صحیح *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    تهی
mch_settmode(tmode_T tmode)
{
    صحیح	status;

    اگر ( tmode == TMODE_RAW )
	set_tty(0, 0);
    وگرنه
    {
	انتخاب (orgmode.width)
	{
	    حالت 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	بشکن;
	    حالت 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	بشکن;
	    پیشفرض:	بشکن;
	}
	out_flush();
	status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, اندازه(TT_MODE), 0,0,0,0);
	اگر (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	    بازگشت;
	(تهی)sys$dassgn(iochan);
	iochan = 0;
    }
}

    ایستا تهی
set_tty(صحیح row, صحیح col)
{
    صحیح		    status;
    TT_MODE	    newmode;		// New TTY mode bits
    ایستا short    first_time = TRUE;

    اگر (first_time)
    {
	orgmode = get_tty();
	first_time = FALSE;
    }
    newmode = get_tty();
    اگر (col)
	newmode.width		 = col;
    اگر (row)
	newmode.x.y.length       = row;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, اندازه(newmode), 0, 0, 0, 0);
    اگر (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	بازگشت;
}

    ایستا TT_MODE
get_tty(تهی)
{

    ایستا $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    صحیح		status;
    TT_MODE	tt_mode;

    اگر (!iochan)
	status = sys$assign(&odsc,&iochan,0,0);

    status = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, اندازه(tt_mode), 0, 0, 0, 0);
    اگر (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.length	= 0;
	tt_mode.extended	= 0;
    }
    بازگشت(tt_mode);
}

/*
 * Get the current window size in Rows and Columns.
 */
    صحیح
mch_get_shellsize(تهی)
{
    TT_MODE	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    بازگشت OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    تهی
mch_set_shellsize(تهی)
{
    set_tty(Rows, Columns);
    انتخاب (Columns)
    {
	حالت 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	بشکن;
	حالت 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	بشکن;
	پیشفرض:	بشکن;
    }
    out_flush();
    screen_start();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    ایستا نویسه		buffer[LNM$C_NAMLENGTH+1];
    char_u		*cp = پوچ;
    unsigned long	attrib;
    صحیح			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    نویسه		*sbuf = پوچ;

    vul_desc(&d_lognam, (نویسه *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.index, اندازه(صحیح), LNM$_INDEX, (نویسه *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, buffer, &lengte);
    itmlst.nul	= 0;
    اگر (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, پوچ,&itmlst) == SS$_NORMAL)
    {
	buffer[lengte] = '\0';
	اگر (cp = alloc(lengte + 1))
	    strcpy((نویسه *)cp, buffer);
	بازگشت(cp);
    }
    وگرنه اگر ((sbuf = getenv((نویسه *)lognam)))
    {
	lengte = strlen(sbuf) + 1;
	cp = alloc(lengte);
	اگر (cp)
	    strcpy((نویسه *)cp, sbuf);
	بازگشت cp;
    }
    وگرنه
	بازگشت(پوچ);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    صحیح
mch_setenv(نویسه *var, نویسه *مقدار, صحیح x)
{
    صحیح		res, dum;
    long	attrib = 0L;
    نویسه	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_item(&itmlst.equ, مقدار ? strlen(مقدار) : 0, مقدار ? LNM$_STRING : 0,
	    مقدار, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    بازگشت((res == 1) ? 0 : -1);
}

    صحیح
vms_sys(نویسه *cmd, نویسه *out, نویسه *inp)
{
    DESC	cdsc, odsc, idsc;
    long	status;

    اگر (cmd)
	vul_desc(&cdsc, cmd);
    اگر (out)
	vul_desc(&odsc, out);
    اگر (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : پوچ,		// command string
	      inp ? &idsc : پوچ,		// input file
	      out ? &odsc : پوچ,		// output file
	      0, 0, 0, &status, 0, 0, 0, 0, 0, 0);
    بازگشت status;
}

/*
 * Convert string to lowercase - most often filename
 */
    نویسه *
vms_tolower( نویسه *name )
{
    صحیح i,nlen = strlen(name);
    برای (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    بازگشت name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    صحیح
vms_sys_status(صحیح status)
{
    اگر (status != SS$_NORMAL && (status & STS$M_SUCCESS) == 0)
	بازگشت status;		// Command failed.
    بازگشت 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    صحیح
vms_read(نویسه *inbuf, size_t nbytes)
{
    صحیح		status, function, len;
    TT_MODE	tt_mode;
    ITEM	itmlst[2];     // terminates on everything
    ایستا long trm_mask[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    اگر (!iochan)
	tt_mode = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_item(&itmlst[0], 0, TRM$_MODIFIERS,
	 (نویسه *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_item(&itmlst[1], اندازه(trm_mask), TRM$_TERM, (نویسه *)&trm_mask, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, اندازه(itmlst));
    len = strlen(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = strlen(inbuf); // return the total length

    بازگشت len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    ایستا صحیح
vms_wproc(نویسه *name, صحیح val)
{
    صحیح i;
    ایستا صحیح vms_match_alloced = 0;

    اگر (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	بازگشت 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    اگر (vms_match_رقم == 0)
    {
	// first time through, setup some things
	اگر (پوچ == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    اگر (!vms_fmatch)
		بازگشت 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	وگرنه
	{
	    // re-use existing space
	    vms_match_free = vms_match_alloced;
	}
    }

    // make matches look uniform
    vms_remove_version(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    برای (i = 0; i<vms_match_رقم; i++)
    {
	اگر (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    بازگشت 1;
    }
    اگر (--vms_match_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		اندازه(نویسه **) * vms_match_alloced);
	اگر (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    بازگشت 0;
	}
	vms_match_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_match_رقم] = vim_strsave((char_u *)name);

    ++vms_match_رقم;
    بازگشت 1;
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
    صحیح
mch_expand_wildcards(
    صحیح		رقم_pat,
    char_u	**pat,
    صحیح		*رقم_file,
    char_u	***file,
    صحیح		flags)
{
    صحیح		i, cnt = 0;
    char_u	buf[MAXPATHL];
    نویسه       *result;
    صحیح		dir;
    صحیح files_alloced, files_free;

    *رقم_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    اگر (*file == پوچ)
    {
	*رقم_file = 0;
	بازگشت FAIL;
    }
    برای (i = 0; i < رقم_pat; i++)
    {
	// expand environment var or home dir
	اگر (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	وگرنه
	    STRCPY(buf,pat[i]);

	vms_match_رقم = 0; // reset collection counter
	result = decc$translate_vms(vms_fixfilename(buf));
	اگر ( (صحیح) result == 0 || (صحیح) result == -1  )
	{
	    cnt = 0;
	}
	وگرنه
	{
	    cnt = decc$to_vms(result, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	اگر (cnt > 0)
	    cnt = vms_match_رقم;

	اگر (cnt < 1)
	    ادامه;

	برای (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    اگر (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		ادامه;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    اگر (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		ادامه;

	    // Skip files that are not executable if we check for that.
	    اگر (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], پوچ, !(flags & EW_SHELLCMD)))
		ادامه;

	    // allocate memory for pointers
	    اگر (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, اندازه(char_u **) * files_alloced);
		اگر (*file == پوچ)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *رقم_file = 0;
		    بازگشت(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*رقم_file++] = vms_fmatch[i];
	}
    }
    بازگشت OK;
}

    صحیح
mch_expandpath(garray_T *gap, char_u *path, صحیح flags)
{
    صحیح		i,cnt = 0;
    نویسه       *result;

    vms_match_رقم = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    result = decc$translate_vms(vms_fixfilename(path));
    اگر ( (صحیح) result == 0 || (صحیح) result == -1  )
    {
	cnt = 0;
    }
    وگرنه
    {
	cnt = decc$to_vms(result, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    اگر (cnt > 0)
	cnt = vms_match_رقم;
    برای (i = 0; i < cnt; i++)
    {
	اگر (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    بازگشت cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    ایستا تهی
vms_unix_mixed_filespec(نویسه *in, نویسه *out)
{
    نویسه *lastcolon;
    نویسه *end_of_dir;
    نویسه ch;
    صحیح len;
    نویسه *out_str=out;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(in, ':');   // find last colon
    اگر (lastcolon != پوچ)
    {
	len = lastcolon - in + 1;
	strncpy(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = پوچ;	// default: no directory

    // start of directory portion
    ch = *in;
    اگر ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    }
    وگرنه اگر (EQN(in, "../", 3))      // Unix parent directory?
    {
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    }
    وگرنه
    {                                // not a special character
	وقتی (EQN(in, "./", 2))     // Ignore Unix "current dir"
	{
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    اگر (strchr(in, '/') == پوچ)     // any more Unix directories ?
    {
	strcpy(out, in);             // No - get rest of the spec
	بازگشت;
    }
    وگرنه
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

    وقتی (*in != '\0')
    {
	ch = *in;
	اگر ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	وگرنه اگر (EQN(in, "../", 3))     // Unix parent directory?
	{
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	وگرنه
	{
	    وقتی (EQN(in, "./", 2))    // Ignore Unix "current dir"
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

    اگر (end_of_dir != پوچ) // Terminate directory portion
	*end_of_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    ایستا صحیح
vms_fspec_proc(نویسه *fil, صحیح val)
{
    strcpy(Fspec_Rms,fil);
    بازگشت(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    تهی *
vms_fixfilename(تهی *instring)
{
    ایستا نویسه		*buf = پوچ;
    ایستا size_t	buflen = 0;
    size_t		len;

    // get a big-enough buffer
    len = strlen(instring) + 1;
    اگر (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * اندازه(نویسه));
    }

#ifdef DEBUG
    نویسه		 *tmpbuf = پوچ;
    tmpbuf = ALLOC_MULT(نویسه, buflen);
    strcpy(tmpbuf, instring);
#endif

    Fspec_Rms = buf;				// for decc$to_vms

    اگر (strchr(instring,'/') == پوچ)
	// It is already a VMS file spec
	strcpy(buf, instring);
    وگرنه اگر (strchr(instring,'"') == پوچ)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	اگر ( (strchr(instring,'[') == پوچ) && (strchr(instring,'<') == پوچ) &&
	     (strchr(instring,']') == پوچ) && (strchr(instring,'>') == پوچ) &&
	     (strchr(instring,':') == پوچ) )
	{
	    // It must be a truly unix fspec
	    decc$to_vms(instring, vms_fspec_proc, 0, 0);
	}
	وگرنه
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    وگرنه
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    بازگشت buf;
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
    تهی
vms_remove_version(تهی * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    صحیح		done = 0;
    صحیح		vdigits = 0;

    rp = (char_u *)fname+ strlen( (نویسه *)fname)- 1;    // Rightmost char
    وقتی ((done == 0) && (rp > (char_u *)fname))       // Count digits
    {
	اگر (isdigit( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	وگرنه
	{
	    done = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    اگر (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	اگر (*rp == (char_u)';')
	{
	    اگر ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	وگرنه اگر (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    اگر ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		done = 0;
		وقتی ((done == 0) && (dp >= (char_u *)fname))
		{
		    اگر ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			اگر ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    done = 1;   // No previous dot found in name
			}
		    }
		    وگرنه اگر (*dp == '.')
		    {                   // Possible dot delimiter
			اگر ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
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

    بازگشت ;
}

ساختار typeahead_st {
    unsigned short numchars;
    unsigned نویسه  firstchar;
    unsigned نویسه  reserved0;
    unsigned long  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    صحیح
RealWaitForChar(
    صحیح		fd UNUSED, // always read from iochan
    long	msec,
    صحیح		*check_for_gpm UNUSED,
    صحیح		*interrupted)
{
    صحیح status;
    ساختار _generic_64 time_curr;
    ساختار _generic_64 time_diff;
    ساختار _generic_64 time_out;
    unsigned صحیح convert_operation = LIB$K_DELTA_SECONDS_F;
    اعشاری sec =(اعشاری) msec/1000;

    // make sure the iochan is set
    اگر (!iochan)
	get_tty();

    اگر (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	status = sys$gettim(&time_curr);
	اگر (status != SS$_NORMAL)
	    بازگشت 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#اگر __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_to_internal_time
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvts_to_internal_time // IEEE
# وگرنه
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time
# endif
#وگرنه
# define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time // VAX
#endif // __IEEE_FLOAT CVTS

	status = LIB_CVTX_TO_INTERNAL_TIME(
		&convert_operation, &sec, &time_diff);
	اگر (status != LIB$_NORMAL)
	    بازگشت 0; // error
	// add them up
	status = lib$add_times(
		&time_curr,
		&time_diff,
		&time_out);
	اگر (status != LIB$_NORMAL)
	    بازگشت 0; // error
    }

    وقتی (TRUE)
    {
	// select()
	status = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	اگر (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
	    بازگشت 0; // error

	اگر (typeahead.numchars)
	    بازگشت 1; // ready to read

	// there's nothing to read; what now?
	اگر (msec == 0)
	{
	    // immediate time-out; return impatiently
	    بازگشت 0;
	}
	وگرنه اگر (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    بازگشت 1; // fakeout to force a wait in vms_read()
	}
	وگرنه
	{
	    // time-out needs to be checked
	    status = sys$gettim(&time_curr);
	    اگر (status != SS$_NORMAL)
		بازگشت 0; // error

	    status = lib$sub_times(
		    &time_out,
		    &time_curr,
		    &time_diff);
	    اگر (status != LIB$_NORMAL)
		بازگشت 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#اگر !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

تعریفنوع ساختار
   {
   نویسه *name;
   صحیح مقدار;
   } decc_feat_t;

صحیح vms_init_done = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (نویسه *)پوچ, 0 } };


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

ایستا تهی
vms_init(تهی)
{
    // Set the global flag to indicate that LIB$INITIALIZE worked

    vms_init_done = 1;

    صحیح feat_index;
    صحیح feat_مقدار;
    صحیح feat_مقدار_max;
    صحیح feat_مقدار_min;
    صحیح i;
    صحیح sts;

    // Loop through all items in the decc_feat_array[]
    برای (i = 0; decc_feat_array[i].name != پوچ; i++)
    {
	// Get the feature index
	feat_index = decc$feature_get_index(decc_feat_array[i].name);
	اگر (feat_index >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_مقدار = decc$feature_get_مقدار(feat_index, 1);
	    feat_مقدار_min = decc$feature_get_مقدار(feat_index, 2);
	    feat_مقدار_max = decc$feature_get_مقدار(feat_index, 3);

	    اگر ((decc_feat_array[i].مقدار >= feat_مقدار_min) && (decc_feat_array[i].مقدار <= feat_مقدار_max))
	    {
		// Valid value.  Set it if necessary
		اگر (feat_مقدار != decc_feat_array[i].مقدار)
		    sts = decc$feature_set_مقدار(feat_index, 1, decc_feat_array[i].مقدار);
	    }
	    وگرنه
		// Invalid DECC feature value
		printf("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_مقدار, feat_مقدار_min, decc_feat_array[i].name, feat_مقدار_max);
	}
	وگرنه
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
تهی (*ثابت x_vms_init)() = vms_init;

# pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
ثابت صحیح spare[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
صحیح LIB$INITIALIZE(تهی);
# pragma extern_model strict_refdef
صحیح dmy_lib$initialize = (صحیح) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#endif // !defined( __VAX) && (__CRTL_VER >= 70301000)
