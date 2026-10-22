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

#অন্তর্ভাব "vim.h"

// define _generic_64 for use in time functions
#যদি !defined(VAX)
# include <gen64def.h>
#অন্যথা
// based on Alpha's gen64def.h; the file is absent on VAX
প্রকারলেখ কাঠামো _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	unsigned পূর্ণসংখ্যা gen64$l_longword [2];   // ...two 32-bit values, or
	unsigned short পূর্ণসংখ্যা gen64$w_word [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#endif

প্রকারলেখ কাঠামো
{
    অক্ষর	class;
    অক্ষর	type;
    short	width;
    সংঘ
    {
	কাঠামো
	{
	    অক্ষর	_basic[3];
	    অক্ষর	দৈর্ঘ্য;
	}	y;
	পূর্ণসংখ্যা	basic;
    }	x;
    পূর্ণসংখ্যা		extended;
}	TT_MODE;

প্রকারলেখ কাঠামো
{
    short	buflen;
    short	itemcode;
    অক্ষর	*bufadrs;
    পূর্ণসংখ্যা		*retlen;
}	ITEM;

প্রকারলেখ কাঠামো
{
    ITEM	equ;
    পূর্ণসংখ্যা		nul;
}	ITMLST1;

প্রকারলেখ কাঠামো
{
    ITEM	সূচক;
    ITEM	string;
    পূর্ণসংখ্যা	nul;
}	ITMLST2;

স্থির TT_MODE	orgmode;
স্থির short	iochan;			// TTY I/O channel
স্থির short	iosb[4];		// IO status block

স্থির পূর্ণসংখ্যা vms_match_অঙ্ক = 0;
স্থির পূর্ণসংখ্যা vms_match_free = 0;
স্থির char_u **vms_fmatch = ফাঁকা;
স্থির অক্ষর *Fspec_Rms;		       // rms file spec, passed implicitly between routines



স্থির TT_MODE	get_tty(শূন্য);
স্থির শূন্য	set_tty(পূর্ণসংখ্যা row, পূর্ণসংখ্যা col);

#define EXPL_ALLOC_INC 64

#define EQN(S1,S2,LN) (strncmp(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) করো { যখন (Str[1] == '/') ++Str; } যখন (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    স্থির শূন্য
vul_desc(DESC *des, অক্ষর *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_pointer = str;
    des->dsc$w_দৈর্ঘ্য = str ? strlen(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    স্থির শূন্য
vul_item(ITEM *itm, short len, short cod, অক্ষর *adr, পূর্ণসংখ্যা *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    শূন্য
mch_settmode(tmode_T tmode)
{
    পূর্ণসংখ্যা	status;

    যদি ( tmode == TMODE_RAW )
	set_tty(0, 0);
    অন্যথা
    {
	নির্বাচন (orgmode.width)
	{
	    ক্ষেত্র 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	বিরতি;
	    ক্ষেত্র 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	বিরতি;
	    পূর্বনির্ধারিত:	বিরতি;
	}
	out_flush();
	status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, আকার(TT_MODE), 0,0,0,0);
	যদি (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	    ফেরত;
	(শূন্য)sys$dassgn(iochan);
	iochan = 0;
    }
}

    স্থির শূন্য
set_tty(পূর্ণসংখ্যা row, পূর্ণসংখ্যা col)
{
    পূর্ণসংখ্যা		    status;
    TT_MODE	    newmode;		// New TTY mode bits
    স্থির short    first_time = TRUE;

    যদি (first_time)
    {
	orgmode = get_tty();
	first_time = FALSE;
    }
    newmode = get_tty();
    যদি (col)
	newmode.width		 = col;
    যদি (row)
	newmode.x.y.দৈর্ঘ্য       = row;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, আকার(newmode), 0, 0, 0, 0);
    যদি (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	ফেরত;
}

    স্থির TT_MODE
get_tty(শূন্য)
{

    স্থির $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    পূর্ণসংখ্যা		status;
    TT_MODE	tt_mode;

    যদি (!iochan)
	status = sys$assign(&odsc,&iochan,0,0);

    status = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, আকার(tt_mode), 0, 0, 0, 0);
    যদি (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.দৈর্ঘ্য	= 0;
	tt_mode.extended	= 0;
    }
    ফেরত(tt_mode);
}

/*
 * Get the current window size in Rows and Columns.
 */
    পূর্ণসংখ্যা
mch_get_shellsize(শূন্য)
{
    TT_MODE	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.width;
    Rows = tmode.x.y.দৈর্ঘ্য;
    ফেরত OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    শূন্য
mch_set_shellsize(শূন্য)
{
    set_tty(Rows, Columns);
    নির্বাচন (Columns)
    {
	ক্ষেত্র 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	বিরতি;
	ক্ষেত্র 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	বিরতি;
	পূর্বনির্ধারিত:	বিরতি;
    }
    out_flush();
    screen_start();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    স্থির অক্ষর		বাফার[LNM$C_NAMLENGTH+1];
    char_u		*cp = ফাঁকা;
    unsigned long	attrib;
    পূর্ণসংখ্যা			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    অক্ষর		*sbuf = ফাঁকা;

    vul_desc(&d_lognam, (অক্ষর *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.সূচক, আকার(পূর্ণসংখ্যা), LNM$সূচক_2, (অক্ষর *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, বাফার, &lengte);
    itmlst.nul	= 0;
    যদি (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, ফাঁকা,&itmlst) == SS$_NORMAL)
    {
	বাফার[lengte] = '\0';
	যদি (cp = alloc(lengte + 1))
	    strcpy((অক্ষর *)cp, বাফার);
	ফেরত(cp);
    }
    অন্যথা যদি ((sbuf = getenv((অক্ষর *)lognam)))
    {
	lengte = strlen(sbuf) + 1;
	cp = alloc(lengte);
	যদি (cp)
	    strcpy((অক্ষর *)cp, sbuf);
	ফেরত cp;
    }
    অন্যথা
	ফেরত(ফাঁকা);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    পূর্ণসংখ্যা
mch_setenv(অক্ষর *var, অক্ষর *মান, পূর্ণসংখ্যা x)
{
    পূর্ণসংখ্যা		res, dum;
    long	attrib = 0L;
    অক্ষর	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_item(&itmlst.equ, মান ? strlen(মান) : 0, মান ? LNM$_STRING : 0,
	    মান, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    ফেরত((res == 1) ? 0 : -1);
}

    পূর্ণসংখ্যা
vms_sys(অক্ষর *cmd, অক্ষর *out, অক্ষর *inp)
{
    DESC	cdsc, odsc, idsc;
    long	status;

    যদি (cmd)
	vul_desc(&cdsc, cmd);
    যদি (out)
	vul_desc(&odsc, out);
    যদি (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : ফাঁকা,		// command string
	      inp ? &idsc : ফাঁকা,		// input file
	      out ? &odsc : ফাঁকা,		// output file
	      0, 0, 0, &status, 0, 0, 0, 0, 0, 0);
    ফেরত status;
}

/*
 * Convert string to lowercase - most often filename
 */
    অক্ষর *
vms_tolower( অক্ষর *name )
{
    পূর্ণসংখ্যা i,nlen = strlen(name);
    জন্য (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    ফেরত name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    পূর্ণসংখ্যা
vms_sys_status(পূর্ণসংখ্যা status)
{
    যদি (status != SS$_NORMAL && (status & STS$M_SUCCESS) == 0)
	ফেরত status;		// Command failed.
    ফেরত 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    পূর্ণসংখ্যা
vms_read(অক্ষর *inbuf, size_t nbytes)
{
    পূর্ণসংখ্যা		status, function, len;
    TT_MODE	tt_mode;
    ITEM	itmlst[2];     // terminates on everything
    স্থির long trm_mask[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    যদি (!iochan)
	tt_mode = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_item(&itmlst[0], 0, TRM$_MODIFIERS,
	 (অক্ষর *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_item(&itmlst[1], আকার(trm_mask), TRM$_TERM, (অক্ষর *)&trm_mask, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, আকার(itmlst));
    len = strlen(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = strlen(inbuf); // return the total length

    ফেরত len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    স্থির পূর্ণসংখ্যা
vms_wproc(অক্ষর *name, পূর্ণসংখ্যা val)
{
    পূর্ণসংখ্যা i;
    স্থির পূর্ণসংখ্যা vms_match_alloced = 0;

    যদি (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	ফেরত 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    যদি (vms_match_অঙ্ক == 0)
    {
	// first time through, setup some things
	যদি (ফাঁকা == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    যদি (!vms_fmatch)
		ফেরত 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	অন্যথা
	{
	    // re-use existing space
	    vms_match_free = vms_match_alloced;
	}
    }

    // make matches look uniform
    vms_remove_version(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    জন্য (i = 0; i<vms_match_অঙ্ক; i++)
    {
	যদি (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    ফেরত 1;
    }
    যদি (--vms_match_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		আকার(অক্ষর **) * vms_match_alloced);
	যদি (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    ফেরত 0;
	}
	vms_match_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_match_অঙ্ক] = vim_strsave((char_u *)name);

    ++vms_match_অঙ্ক;
    ফেরত 1;
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
    পূর্ণসংখ্যা
mch_expand_wildcards(
    পূর্ণসংখ্যা		অঙ্ক_pat,
    char_u	**pat,
    পূর্ণসংখ্যা		*অঙ্ক_file,
    char_u	***file,
    পূর্ণসংখ্যা		flags)
{
    পূর্ণসংখ্যা		i, cnt = 0;
    char_u	buf[MAXPATHL];
    অক্ষর       *ফলাফল;
    পূর্ণসংখ্যা		dir;
    পূর্ণসংখ্যা files_alloced, files_free;

    *অঙ্ক_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    যদি (*file == ফাঁকা)
    {
	*অঙ্ক_file = 0;
	ফেরত FAIL;
    }
    জন্য (i = 0; i < অঙ্ক_pat; i++)
    {
	// expand environment var or home dir
	যদি (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	অন্যথা
	    STRCPY(buf,pat[i]);

	vms_match_অঙ্ক = 0; // reset collection counter
	ফলাফল = decc$translate_vms(vms_fixfilename(buf));
	যদি ( (পূর্ণসংখ্যা) ফলাফল == 0 || (পূর্ণসংখ্যা) ফলাফল == -1  )
	{
	    cnt = 0;
	}
	অন্যথা
	{
	    cnt = decc$to_vms(ফলাফল, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	যদি (cnt > 0)
	    cnt = vms_match_অঙ্ক;

	যদি (cnt < 1)
	    চালু;

	জন্য (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    যদি (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		চালু;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    যদি (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		চালু;

	    // Skip files that are not executable if we check for that.
	    যদি (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], ফাঁকা, !(flags & EW_SHELLCMD)))
		চালু;

	    // allocate memory for pointers
	    যদি (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, আকার(char_u **) * files_alloced);
		যদি (*file == ফাঁকা)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *অঙ্ক_file = 0;
		    ফেরত(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*অঙ্ক_file++] = vms_fmatch[i];
	}
    }
    ফেরত OK;
}

    পূর্ণসংখ্যা
mch_expandpath(garray_T *gap, char_u *path, পূর্ণসংখ্যা flags)
{
    পূর্ণসংখ্যা		i,cnt = 0;
    অক্ষর       *ফলাফল;

    vms_match_অঙ্ক = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    ফলাফল = decc$translate_vms(vms_fixfilename(path));
    যদি ( (পূর্ণসংখ্যা) ফলাফল == 0 || (পূর্ণসংখ্যা) ফলাফল == -1  )
    {
	cnt = 0;
    }
    অন্যথা
    {
	cnt = decc$to_vms(ফলাফল, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    যদি (cnt > 0)
	cnt = vms_match_অঙ্ক;
    জন্য (i = 0; i < cnt; i++)
    {
	যদি (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    ফেরত cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    স্থির শূন্য
vms_unix_mixed_filespec(অক্ষর *in, অক্ষর *out)
{
    অক্ষর *lastcolon;
    অক্ষর *end_of_dir;
    অক্ষর ch;
    পূর্ণসংখ্যা len;
    অক্ষর *out_str=out;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(in, ':');   // find last colon
    যদি (lastcolon != ফাঁকা)
    {
	len = lastcolon - in + 1;
	strncpy(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = ফাঁকা;	// default: no directory

    // start of directory portion
    ch = *in;
    যদি ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    }
    অন্যথা যদি (EQN(in, "../", 3))      // Unix parent directory?
    {
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    }
    অন্যথা
    {                                // not a special character
	যখন (EQN(in, "./", 2))     // Ignore Unix "current dir"
	{
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    যদি (strchr(in, '/') == ফাঁকা)     // any more Unix directories ?
    {
	strcpy(out, in);             // No - get rest of the spec
	ফেরত;
    }
    অন্যথা
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

    যখন (*in != '\0')
    {
	ch = *in;
	যদি ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	অন্যথা যদি (EQN(in, "../", 3))     // Unix parent directory?
	{
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	অন্যথা
	{
	    যখন (EQN(in, "./", 2))    // Ignore Unix "current dir"
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

    যদি (end_of_dir != ফাঁকা) // Terminate directory portion
	*end_of_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    স্থির পূর্ণসংখ্যা
vms_fspec_proc(অক্ষর *fil, পূর্ণসংখ্যা val)
{
    strcpy(Fspec_Rms,fil);
    ফেরত(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    শূন্য *
vms_fixfilename(শূন্য *instring)
{
    স্থির অক্ষর		*buf = ফাঁকা;
    স্থির size_t	buflen = 0;
    size_t		len;

    // get a big-enough buffer
    len = strlen(instring) + 1;
    যদি (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * আকার(অক্ষর));
    }

#ifdef DEBUG
    অক্ষর		 *tmpbuf = ফাঁকা;
    tmpbuf = ALLOC_MULT(অক্ষর, buflen);
    strcpy(tmpbuf, instring);
#endif

    Fspec_Rms = buf;				// for decc$to_vms

    যদি (strchr(instring,'/') == ফাঁকা)
	// It is already a VMS file spec
	strcpy(buf, instring);
    অন্যথা যদি (strchr(instring,'"') == ফাঁকা)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	যদি ( (strchr(instring,'[') == ফাঁকা) && (strchr(instring,'<') == ফাঁকা) &&
	     (strchr(instring,']') == ফাঁকা) && (strchr(instring,'>') == ফাঁকা) &&
	     (strchr(instring,':') == ফাঁকা) )
	{
	    // It must be a truly unix fspec
	    decc$to_vms(instring, vms_fspec_proc, 0, 0);
	}
	অন্যথা
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    অন্যথা
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    ফেরত buf;
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
    শূন্য
vms_remove_version(শূন্য * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    পূর্ণসংখ্যা		done = 0;
    পূর্ণসংখ্যা		vdigits = 0;

    rp = (char_u *)fname+ strlen( (অক্ষর *)fname)- 1;    // Rightmost char
    যখন ((done == 0) && (rp > (char_u *)fname))       // Count digits
    {
	যদি (isdigit( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	অন্যথা
	{
	    done = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    যদি (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	যদি (*rp == (char_u)';')
	{
	    যদি ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	অন্যথা যদি (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    যদি ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		done = 0;
		যখন ((done == 0) && (dp >= (char_u *)fname))
		{
		    যদি ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			যদি ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    done = 1;   // No previous dot found in name
			}
		    }
		    অন্যথা যদি (*dp == '.')
		    {                   // Possible dot delimiter
			যদি ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
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

    ফেরত ;
}

কাঠামো typeahead_st {
    unsigned short numchars;
    unsigned অক্ষর  firstchar;
    unsigned অক্ষর  reserved0;
    unsigned long  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    পূর্ণসংখ্যা
RealWaitForChar(
    পূর্ণসংখ্যা		fd UNUSED, // always read from iochan
    long	msec,
    পূর্ণসংখ্যা		*check_for_gpm UNUSED,
    পূর্ণসংখ্যা		*interrupted)
{
    পূর্ণসংখ্যা status;
    কাঠামো _generic_64 time_curr;
    কাঠামো _generic_64 time_diff;
    কাঠামো _generic_64 time_out;
    unsigned পূর্ণসংখ্যা convert_operation = LIB$K_DELTA_SECONDS_F;
    দশমিক sec =(দশমিক) msec/1000;

    // make sure the iochan is set
    যদি (!iochan)
	get_tty();

    যদি (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	status = sys$gettim(&time_curr);
	যদি (status != SS$_NORMAL)
	    ফেরত 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#যদি __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_to_internal_time
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvts_to_internal_time // IEEE
# অন্যথা
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time
# endif
#অন্যথা
# define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time // VAX
#endif // __IEEE_FLOAT CVTS

	status = LIB_CVTX_TO_INTERNAL_TIME(
		&convert_operation, &sec, &time_diff);
	যদি (status != LIB$_NORMAL)
	    ফেরত 0; // error
	// add them up
	status = lib$add_times(
		&time_curr,
		&time_diff,
		&time_out);
	যদি (status != LIB$_NORMAL)
	    ফেরত 0; // error
    }

    যখন (TRUE)
    {
	// select()
	status = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	যদি (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
	    ফেরত 0; // error

	যদি (typeahead.numchars)
	    ফেরত 1; // ready to read

	// there's nothing to read; what now?
	যদি (msec == 0)
	{
	    // immediate time-out; return impatiently
	    ফেরত 0;
	}
	অন্যথা যদি (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    ফেরত 1; // fakeout to force a wait in vms_read()
	}
	অন্যথা
	{
	    // time-out needs to be checked
	    status = sys$gettim(&time_curr);
	    যদি (status != SS$_NORMAL)
		ফেরত 0; // error

	    status = lib$sub_times(
		    &time_out,
		    &time_curr,
		    &time_diff);
	    যদি (status != LIB$_NORMAL)
		ফেরত 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#যদি !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

প্রকারলেখ কাঠামো
   {
   অক্ষর *name;
   পূর্ণসংখ্যা মান;
   } decc_feat_t;

পূর্ণসংখ্যা vms_init_done = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (অক্ষর *)ফাঁকা, 0 } };


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

স্থির শূন্য
vms_init(শূন্য)
{
    // Set the global flag to indicate that LIB$INITIALIZE worked

    vms_init_done = 1;

    পূর্ণসংখ্যা feat_সূচক;
    পূর্ণসংখ্যা feat_মান;
    পূর্ণসংখ্যা feat_মান_max;
    পূর্ণসংখ্যা feat_মান_min;
    পূর্ণসংখ্যা i;
    পূর্ণসংখ্যা sts;

    // Loop through all items in the decc_feat_array[]
    জন্য (i = 0; decc_feat_array[i].name != ফাঁকা; i++)
    {
	// Get the feature index
	feat_সূচক = decc$feature_get_সূচক(decc_feat_array[i].name);
	যদি (feat_সূচক >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_মান = decc$feature_get_মান(feat_সূচক, 1);
	    feat_মান_min = decc$feature_get_মান(feat_সূচক, 2);
	    feat_মান_max = decc$feature_get_মান(feat_সূচক, 3);

	    যদি ((decc_feat_array[i].মান >= feat_মান_min) && (decc_feat_array[i].মান <= feat_মান_max))
	    {
		// Valid value.  Set it if necessary
		যদি (feat_মান != decc_feat_array[i].মান)
		    sts = decc$feature_set_মান(feat_সূচক, 1, decc_feat_array[i].মান);
	    }
	    অন্যথা
		// Invalid DECC feature value
		printf("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_মান, feat_মান_min, decc_feat_array[i].name, feat_মান_max);
	}
	অন্যথা
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
শূন্য (*ধ্রুবক x_vms_init)() = vms_init;

# pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
ধ্রুবক পূর্ণসংখ্যা spare[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
পূর্ণসংখ্যা LIB$INITIALIZE(শূন্য);
# pragma extern_model strict_refdef
পূর্ণসংখ্যা dmy_lib$initialize = (পূর্ণসংখ্যা) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#endif // !defined( __VAX) && (__CRTL_VER >= 70301000)
