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

	بینښه صحيح gen64$l_longword [2];   // ...two 32-bit values, or
	بینښه لنډ صحيح gen64$w_کلمه [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#د_که_پای

تعريفنوع بنية
{
    حرف	class;
    حرف	type;
    لنډ	پلنوالی;
    اتحاد
    {
	بنية
	{
	    حرف	_basic[3];
	    حرف	اوږدتیا;
	}	y;
	صحيح	basic;
    }	x;
    صحيح		extended;
}	TT_ډېرپېښ;

تعريفنوع بنية
{
    لنډ	buflen;
    لنډ	itemcode;
    حرف	*bufadrs;
    صحيح		*retlen;
}	توکی_3;

تعريفنوع بنية
{
    توکی_3	equ;
    صحيح		nul;
}	ITMLST1;

تعريفنوع بنية
{
    توکی_3	شاخص;
    توکی_3	string;
    صحيح	nul;
}	ITMLST2;

ساكن TT_ډېرپېښ	orgmode;
ساكن لنډ	iochan;			// TTY I/O channel
ساكن لنډ	iosb[4];		// IO status block

ساكن صحيح vms_سمون_عدد = 0;
ساكن صحيح vms_سمون_free = 0;
ساكن char_u **vms_fmatch = عدم;
ساكن حرف *Fspec_Rms;		       // rms file spec, passed implicitly between routines



ساكن TT_ډېرپېښ	get_tty(فراغ);
ساكن فراغ	سېټ_tty(صحيح لیکه, صحيح ستون);

#تعریف_کړه EXPL_ALLOC_INC 64

#تعریف_کړه EQN(S1,S2,LN) (مزي_پرتله(S1,S2,LN) == 0)
#تعریف_کړه SKIP_FOLLOWING_SLASHES(Str) افعل { طالما (Str[1] == '/') ++Str; } طالما (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    ساكن فراغ
vul_desc(DESC *des, حرف *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_اشاره_ګر = str;
    des->dsc$w_اوږدتیا = str ? مزي_اوږدوالی(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    ساكن فراغ
vul_توکی(توکی_3 *itm, لنډ len, لنډ cod, حرف *adr, صحيح *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    فراغ
mch_settmode(tmode_T tmode)
{
    صحيح	حالت;

    إذا ( tmode == TMODE_RAW )
	سېټ_tty(0, 0);
    وإلا
    {
	بدل (orgmode.پلنوالی)
	{
	    حالة 132:	بهر_STR_NF((char_u *)"\033[?3h\033>");	اكسر;
	    حالة 80:	بهر_STR_NF((char_u *)"\033[?3l\033>");	اكسر;
	    افتراضي:	اكسر;
	}
	بهر_flush();
	حالت = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, حجم(TT_ډېرپېښ), 0,0,0,0);
	إذا (حالت!=SS$نورمال_3 || (iosb[0]&0xFFFF)!=SS$نورمال_3)
	    أرجع;
	(فراغ)sys$dassgn(iochan);
	iochan = 0;
    }
}

    ساكن فراغ
سېټ_tty(صحيح لیکه, صحيح ستون)
{
    صحيح		    حالت;
    TT_ډېرپېښ	    newmode;		// New TTY mode bits
    ساكن لنډ    لومړی_وخت = TRUE;

    إذا (لومړی_وخت)
    {
	orgmode = get_tty();
	لومړی_وخت = FALSE;
    }
    newmode = get_tty();
    إذا (ستون)
	newmode.پلنوالی		 = ستون;
    إذا (لیکه)
	newmode.x.y.اوږدتیا       = لیکه;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    حالت = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, حجم(newmode), 0, 0, 0, 0);
    إذا (حالت!=SS$نورمال_3 || (iosb[0]&0xFFFF)!=SS$نورمال_3)
	أرجع;
}

    ساكن TT_ډېرپېښ
get_tty(فراغ)
{

    ساكن $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    صحيح		حالت;
    TT_ډېرپېښ	tt_ډېرپېښ;

    إذا (!iochan)
	حالت = sys$ټاکل(&odsc,&iochan,0,0);

    حالت = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_ډېرپېښ, حجم(tt_ډېرپېښ), 0, 0, 0, 0);
    إذا (حالت != SS$نورمال_3 || (iosb[0] & 0xFFFF) != SS$نورمال_3)
    {
	tt_ډېرپېښ.پلنوالی		= 0;
	tt_ډېرپېښ.type		= 0;
	tt_ډېرپېښ.class		= 0;
	tt_ډېرپېښ.x.basic		= 0;
	tt_ډېرپېښ.x.y.اوږدتیا	= 0;
	tt_ډېرپېښ.extended	= 0;
    }
    أرجع(tt_ډېرپېښ);
}

/*
 * Get the current window size in Rows and Columns.
 */
    صحيح
mch_get_shellsize(فراغ)
{
    TT_ډېرپېښ	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.پلنوالی;
    لیکې = tmode.x.y.اوږدتیا;
    أرجع OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    فراغ
mch_سېټ_shellsize(فراغ)
{
    سېټ_tty(لیکې, Columns);
    بدل (Columns)
    {
	حالة 132:	بهر_STR_NF((char_u *)"\033[?3h\033>");	اكسر;
	حالة 80:	بهر_STR_NF((char_u *)"\033[?3l\033>");	اكسر;
	افتراضي:	اكسر;
    }
    بهر_flush();
    screen_پیل();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    ساكن حرف		بفر[LNM$C_NAMLENGTH+1];
    char_u		*cp = عدم;
    بینښه اوږد	attrib;
    صحيح			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    حرف		*sbuf = عدم;

    vul_desc(&d_lognam, (حرف *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_توکی(&itmlst.شاخص, حجم(صحيح), LNM$شاخص_2, (حرف *)&idx, &dum);
    vul_توکی(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, بفر, &lengte);
    itmlst.nul	= 0;
    إذا (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, عدم,&itmlst) == SS$نورمال_3)
    {
	بفر[lengte] = '\0';
	إذا (cp = alloc(lengte + 1))
	    strcpy((حرف *)cp, بفر);
	أرجع(cp);
    }
    وإلا إذا ((sbuf = getenv((حرف *)lognam)))
    {
	lengte = مزي_اوږدوالی(sbuf) + 1;
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
mch_setenv(حرف *var, حرف *ارزښت, صحيح x)
{
    صحيح		res, dum;
    اوږد	attrib = 0L;
    حرف	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_توکی(&itmlst.equ, ارزښت ? مزي_اوږدوالی(ارزښت) : 0, ارزښت ? LNM$_STRING : 0,
	    ارزښت, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    أرجع((res == 1) ? 0 : -1);
}

    صحيح
vms_sys(حرف *cmd, حرف *بهر, حرف *inp)
{
    DESC	cdsc, odsc, idsc;
    اوږد	حالت;

    إذا (cmd)
	vul_desc(&cdsc, cmd);
    إذا (بهر)
	vul_desc(&odsc, بهر);
    إذا (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : عدم,		// command string
	      inp ? &idsc : عدم,		// input file
	      بهر ? &odsc : عدم,		// output file
	      0, 0, 0, &حالت, 0, 0, 0, 0, 0, 0);
    أرجع حالت;
}

/*
 * Convert string to lowercase - most often filename
 */
    حرف *
vms_tolower( حرف *name )
{
    صحيح i,nlen = مزي_اوږدوالی(name);
    لكل (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    أرجع name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    صحيح
vms_sys_حالت(صحيح حالت)
{
    إذا (حالت != SS$نورمال_3 && (حالت & STS$M_SUCCESS) == 0)
	أرجع حالت;		// Command failed.
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
    صحيح		حالت, function, len;
    TT_ډېرپېښ	tt_ډېرپېښ;
    توکی_3	itmlst[2];     // terminates on everything
    ساكن اوږد trm_ماسک[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    إذا (!iochan)
	tt_ډېرپېښ = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_توکی(&itmlst[0], 0, TRM$_MODIFIERS,
	 (حرف *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_توکی(&itmlst[1], حجم(trm_ماسک), TRM$غړی_2, (حرف *)&trm_ماسک, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    حالت = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, حجم(itmlst));
    len = مزي_اوږدوالی(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    حالت = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = مزي_اوږدوالی(inbuf); // return the total length

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
    ساكن صحيح vms_سمون_alloced = 0;

    إذا (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	أرجع 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    إذا (vms_سمون_عدد == 0)
    {
	// first time through, setup some things
	إذا (عدم == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    إذا (!vms_fmatch)
		أرجع 0;
	    vms_سمون_alloced = EXPL_ALLOC_INC;
	    vms_سمون_free = EXPL_ALLOC_INC;
	}
	وإلا
	{
	    // re-use existing space
	    vms_سمون_free = vms_سمون_alloced;
	}
    }

    // make matches look uniform
    vms_remove_نسخه(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    لكل (i = 0; i<vms_سمون_عدد; i++)
    {
	إذا (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    أرجع 1;
    }
    إذا (--vms_سمون_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_سمون_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		حجم(حرف **) * vms_سمون_alloced);
	إذا (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    أرجع 0;
	}
	vms_سمون_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_سمون_عدد] = vim_strsave((char_u *)name);

    ++vms_سمون_عدد;
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
    صحيح		عدد_pat,
    char_u	**pat,
    صحيح		*عدد_file,
    char_u	***file,
    صحيح		flags)
{
    صحيح		i, cnt = 0;
    char_u	buf[MAXPATHL];
    حرف       *پایله;
    صحيح		dir;
    صحيح files_alloced, files_free;

    *عدد_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    إذا (*file == عدم)
    {
	*عدد_file = 0;
	أرجع FAIL;
    }
    لكل (i = 0; i < عدد_pat; i++)
    {
	// expand environment var or home dir
	إذا (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	وإلا
	    STRCPY(buf,pat[i]);

	vms_سمون_عدد = 0; // reset collection counter
	پایله = decc$translate_vms(vms_fixfilename(buf));
	إذا ( (صحيح) پایله == 0 || (صحيح) پایله == -1  )
	{
	    cnt = 0;
	}
	وإلا
	{
	    cnt = decc$منزل_vms(پایله, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	إذا (cnt > 0)
	    cnt = vms_سمون_عدد;

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
		    *عدد_file = 0;
		    أرجع(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*عدد_file++] = vms_fmatch[i];
	}
    }
    أرجع OK;
}

    صحيح
mch_expandpath(garray_T *gap, char_u *لار, صحيح flags)
{
    صحيح		i,cnt = 0;
    حرف       *پایله;

    vms_سمون_عدد = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    پایله = decc$translate_vms(vms_fixfilename(لار));
    إذا ( (صحيح) پایله == 0 || (صحيح) پایله == -1  )
    {
	cnt = 0;
    }
    وإلا
    {
	cnt = decc$منزل_vms(پایله, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    إذا (cnt > 0)
	cnt = vms_سمون_عدد;
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
vms_unix_mixed_filespec(حرف *دننه, حرف *بهر)
{
    حرف *lastcolon;
    حرف *پای_ټکی_د_dir;
    حرف ch;
    صحيح len;
    حرف *بهر_str=بهر;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(دننه, ':');   // find last colon
    إذا (lastcolon != عدم)
    {
	len = lastcolon - دننه + 1;
	strncpy(بهر, دننه, len);
	بهر += len;
	دننه += len;
    }

    پای_ټکی_د_dir = عدم;	// default: no directory

    // start of directory portion
    ch = *دننه;
    إذا ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(دننه);
    }
    وإلا إذا (EQN(دننه, "../", 3))      // Unix parent directory?
    {
	*بهر++ = '[';
	*بهر++ = '-';
	پای_ټکی_د_dir = بهر;
	ch = '.';
	دننه += 2;
	SKIP_FOLLOWING_SLASHES(دننه);
    }
    وإلا
    {                                // not a special character
	طالما (EQN(دننه, "./", 2))     // Ignore Unix "current dir"
	{
	    دننه += 2;
	    SKIP_FOLLOWING_SLASHES(دننه);
    }
    إذا (strchr(دننه, '/') == عدم)     // any more Unix directories ?
    {
	strcpy(بهر, دننه);             // No - get rest of the spec
	أرجع;
    }
    وإلا
    {
	*بهر++ = '[';                // Yes, denote a Vms subdirectory
	ch = '.';
	--دننه;
	}
    }

    // if we get here, there is a directory part of the filename

    // initialize output file spec
    *بهر++ = ch;
    ++دننه;

    طالما (*دننه != '\0')
    {
	ch = *دننه;
	إذا ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    پای_ټکی_د_dir = بهر;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(دننه);
	    }
	وإلا إذا (EQN(دننه, "../", 3))     // Unix parent directory?
	{
	    *بهر++ = '-';
	    پای_ټکی_د_dir = بهر;
	    ch = '.';
	    دننه += 2;
	    SKIP_FOLLOWING_SLASHES(دننه);
	    }
	وإلا
	{
	    طالما (EQN(دننه, "./", 2))    // Ignore Unix "current dir"
	    {
		پای_ټکی_د_dir = بهر;
		دننه += 2;
		SKIP_FOLLOWING_SLASHES(دننه);
		ch = *دننه;
	    }
	}

    // Place next character into output file spec
	*بهر++ = ch;
	++دننه;
    }

    *بهر = '\0';            // Terminate output file spec

    إذا (پای_ټکی_د_dir != عدم) // Terminate directory portion
	*پای_ټکی_د_dir = ']';
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
    len = مزي_اوږدوالی(instring) + 1;
    إذا (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * حجم(حرف));
    }

#که_تعریف_شوی DEBUG
    حرف		 *tmpbuf = عدم;
    tmpbuf = ALLOC_MULT(حرف, buflen);
    strcpy(tmpbuf, instring);
#د_که_پای

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
	    decc$منزل_vms(instring, vms_fspec_proc, 0, 0);
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
vms_remove_نسخه(فراغ * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    صحيح		بشپړ = 0;
    صحيح		vdigits = 0;

    rp = (char_u *)fname+ مزي_اوږدوالی( (حرف *)fname)- 1;    // Rightmost char
    طالما ((بشپړ == 0) && (rp > (char_u *)fname))       // Count digits
    {
	إذا (که_عدد( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	وإلا
	{
	    بشپړ = 1;                                   // Quit at non-digit
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
		بشپړ = 0;
		طالما ((بشپړ == 0) && (dp >= (char_u *)fname))
		{
		    إذا ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			إذا ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    بشپړ = 1;   // No previous dot found in name
			}
		    }
		    وإلا إذا (*dp == '.')
		    {                   // Possible dot delimiter
			إذا ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dot delimiter
			    بشپړ = 1;   // Previous dot found in name
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
    بینښه لنډ numchars;
    بینښه حرف  firstchar;
    بینښه حرف  reserved0;
    بینښه اوږد  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    صحيح
RealWaitForChar(
    صحيح		fd ناکارول_شوی, // always read from iochan
    اوږد	msec,
    صحيح		*check_for_gpm ناکارول_شوی,
    صحيح		*interrupted)
{
    صحيح حالت;
    بنية _generic_64 وخت_curr;
    بنية _generic_64 وخت_diff;
    بنية _generic_64 وخت_بهر;
    بینښه صحيح convert_operation = LIB$K_DELTA_SECONDS_F;
    عائم sec =(عائم) msec/1000;

    // make sure the iochan is set
    إذا (!iochan)
	get_tty();

    إذا (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	حالت = sys$gettim(&وخت_curr);
	إذا (حالت != SS$نورمال_3)
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
# ifdef lib$cvts_منزل_دننی_وخت
#  define LIB_CVTX_منزل_دننی_وخت lib$cvts_منزل_دننی_وخت // IEEE
# وإلا
#  define LIB_CVTX_منزل_دننی_وخت lib$cvtf_منزل_دننی_وخت
# endif
#وإلا
# define LIB_CVTX_منزل_دننی_وخت lib$cvtf_منزل_دننی_وخت // VAX
#د_که_پای // __IEEE_FLOAT CVTS

	حالت = LIB_CVTX_منزل_دننی_وخت(
		&convert_operation, &sec, &وخت_diff);
	إذا (حالت != LIB$نورمال_3)
	    أرجع 0; // error
	// add them up
	حالت = lib$جمع_کړه_times(
		&وخت_curr,
		&وخت_diff,
		&وخت_بهر);
	إذا (حالت != LIB$نورمال_3)
	    أرجع 0; // error
    }

    طالما (TRUE)
    {
	// select()
	حالت = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	إذا (حالت != SS$نورمال_3 || (iosb[0] & 0xFFFF) != SS$نورمال_3)
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
	    حالت = sys$gettim(&وخت_curr);
	    إذا (حالت != SS$نورمال_3)
		أرجع 0; // error

	    حالت = lib$sub_times(
		    &وخت_بهر,
		    &وخت_curr,
		    &وخت_diff);
	    إذا (حالت != LIB$نورمال_3)
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
   صحيح ارزښت;
   } decc_feat_t;

صحيح vms_init_بشپړ = -1;

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

    vms_init_بشپړ = 1;

    صحيح feat_شاخص;
    صحيح feat_ارزښت;
    صحيح feat_ارزښت_max;
    صحيح feat_ارزښت_min;
    صحيح i;
    صحيح sts;

    // Loop through all items in the decc_feat_array[]
    لكل (i = 0; decc_feat_array[i].name != عدم; i++)
    {
	// Get the feature index
	feat_شاخص = decc$feature_get_شاخص(decc_feat_array[i].name);
	إذا (feat_شاخص >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_ارزښت = decc$feature_get_ارزښت(feat_شاخص, 1);
	    feat_ارزښت_min = decc$feature_get_ارزښت(feat_شاخص, 2);
	    feat_ارزښت_max = decc$feature_get_ارزښت(feat_شاخص, 3);

	    إذا ((decc_feat_array[i].ارزښت >= feat_ارزښت_min) && (decc_feat_array[i].ارزښت <= feat_ارزښت_max))
	    {
		// Valid value.  Set it if necessary
		إذا (feat_ارزښت != decc_feat_array[i].ارزښت)
		    sts = decc$feature_سېټ_ارزښت(feat_شاخص, 1, decc_feat_array[i].ارزښت);
	    }
	    وإلا
		// Invalid DECC feature value
		چاپ_کړه("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_ارزښت, feat_ارزښت_min, decc_feat_array[i].name, feat_ارزښت_max);
	}
	وإلا
	    // Invalid DECC feature name
	    چاپ_کړه("UNKNOWN DECC FEATURE: %s.\n", decc_feat_array[i].name);
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
ثابت صحيح اضافي[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
صحيح LIB$INITIALIZE(فراغ);
# pragma extern_model strict_refdef
صحيح dmy_lib$initialize = (صحيح) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#د_که_پای // !defined( __VAX) && (__CRTL_VER >= 70301000)
