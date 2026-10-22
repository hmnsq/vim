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

#అన్తర్భావ "vim.h"

// define _generic_64 for use in time functions
#ఒకవేళ !defined(VAX)
# include <gen64def.h>
#లేకపోతే
// based on Alpha's gen64def.h; the file is absent on VAX
రకనిర్వచనం నిర్మాణం _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	గుర్తులేని పూర్ణాంకం gen64$l_longword [2];   // ...two 32-bit values, or
	గుర్తులేని పొట్టి పూర్ణాంకం gen64$w_మాట [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#అయితే_ముగింపు

రకనిర్వచనం నిర్మాణం
{
    అక్షరం	class;
    అక్షరం	type;
    పొట్టి	వెడల్పు;
    సంఘం
    {
	నిర్మాణం
	{
	    అక్షరం	_basic[3];
	    అక్షరం	నిడివి;
	}	y;
	పూర్ణాంకం	basic;
    }	x;
    పూర్ణాంకం		extended;
}	TT_బహుళకం;

రకనిర్వచనం నిర్మాణం
{
    పొట్టి	buflen;
    పొట్టి	itemcode;
    అక్షరం	*bufadrs;
    పూర్ణాంకం		*retlen;
}	అంశం_3;

రకనిర్వచనం నిర్మాణం
{
    అంశం_3	equ;
    పూర్ణాంకం		nul;
}	ITMLST1;

రకనిర్వచనం నిర్మాణం
{
    అంశం_3	సూచీ;
    అంశం_3	string;
    పూర్ణాంకం	nul;
}	ITMLST2;

స్థిర TT_బహుళకం	orgmode;
స్థిర పొట్టి	iochan;			// TTY I/O channel
స్థిర పొట్టి	iosb[4];		// IO status block

స్థిర పూర్ణాంకం vms_సరిపోలిక_అంకె = 0;
స్థిర పూర్ణాంకం vms_సరిపోలిక_free = 0;
స్థిర char_u **vms_fmatch = ఖాళీ;
స్థిర అక్షరం *Fspec_Rms;		       // rms file spec, passed implicitly between routines



స్థిర TT_బహుళకం	get_tty(శూన్య);
స్థిర శూన్య	సమితి_tty(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు);

#నిర్వచించు EXPL_ALLOC_INC 64

#నిర్వచించు EQN(S1,S2,LN) (అక్షరశ్రేణి_పోలిక(S1,S2,LN) == 0)
#నిర్వచించు SKIP_FOLLOWING_SLASHES(Str) చేయి { అయితే (Str[1] == '/') ++Str; } అయితే (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    స్థిర శూన్య
vul_desc(DESC *des, అక్షరం *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_సూచిక = str;
    des->dsc$w_నిడివి = str ? అక్షరశ్రేణి_పొడవు(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    స్థిర శూన్య
vul_అంశం(అంశం_3 *itm, పొట్టి len, పొట్టి cod, అక్షరం *adr, పూర్ణాంకం *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    శూన్య
mch_settmode(tmode_T tmode)
{
    పూర్ణాంకం	స్థితి_2;

    ఒకవేళ ( tmode == TMODE_RAW )
	సమితి_tty(0, 0);
    లేకపోతే
    {
	ఎంపిక (orgmode.వెడల్పు)
	{
	    స్థితి 132:	బహిర్గతం_STR_NF((char_u *)"\033[?3h\033>");	ఆపు;
	    స్థితి 80:	బహిర్గతం_STR_NF((char_u *)"\033[?3l\033>");	ఆపు;
	    అప్రమేయ:	ఆపు;
	}
	బహిర్గతం_flush();
	స్థితి_2 = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, పరిమాణం(TT_బహుళకం), 0,0,0,0);
	ఒకవేళ (స్థితి_2!=SS$సాధారణ_3 || (iosb[0]&0xFFFF)!=SS$సాధారణ_3)
	    తిరిగి;
	(శూన్య)sys$dassgn(iochan);
	iochan = 0;
    }
}

    స్థిర శూన్య
సమితి_tty(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    పూర్ణాంకం		    స్థితి_2;
    TT_బహుళకం	    newmode;		// New TTY mode bits
    స్థిర పొట్టి    మొదటి_సమయం = TRUE;

    ఒకవేళ (మొదటి_సమయం)
    {
	orgmode = get_tty();
	మొదటి_సమయం = FALSE;
    }
    newmode = get_tty();
    ఒకవేళ (నిలువు)
	newmode.వెడల్పు		 = నిలువు;
    ఒకవేళ (అడ్డువరుస)
	newmode.x.y.నిడివి       = అడ్డువరుస;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    స్థితి_2 = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, పరిమాణం(newmode), 0, 0, 0, 0);
    ఒకవేళ (స్థితి_2!=SS$సాధారణ_3 || (iosb[0]&0xFFFF)!=SS$సాధారణ_3)
	తిరిగి;
}

    స్థిర TT_బహుళకం
get_tty(శూన్య)
{

    స్థిర $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    పూర్ణాంకం		స్థితి_2;
    TT_బహుళకం	tt_బహుళకం;

    ఒకవేళ (!iochan)
	స్థితి_2 = sys$కేటాయించు(&odsc,&iochan,0,0);

    స్థితి_2 = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_బహుళకం, పరిమాణం(tt_బహుళకం), 0, 0, 0, 0);
    ఒకవేళ (స్థితి_2 != SS$సాధారణ_3 || (iosb[0] & 0xFFFF) != SS$సాధారణ_3)
    {
	tt_బహుళకం.వెడల్పు		= 0;
	tt_బహుళకం.type		= 0;
	tt_బహుళకం.class		= 0;
	tt_బహుళకం.x.basic		= 0;
	tt_బహుళకం.x.y.నిడివి	= 0;
	tt_బహుళకం.extended	= 0;
    }
    తిరిగి(tt_బహుళకం);
}

/*
 * Get the current window size in Rows and Columns.
 */
    పూర్ణాంకం
mch_get_shellsize(శూన్య)
{
    TT_బహుళకం	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.వెడల్పు;
    అడ్డువరుసలు = tmode.x.y.నిడివి;
    తిరిగి OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    శూన్య
mch_సమితి_shellsize(శూన్య)
{
    సమితి_tty(అడ్డువరుసలు, Columns);
    ఎంపిక (Columns)
    {
	స్థితి 132:	బహిర్గతం_STR_NF((char_u *)"\033[?3h\033>");	ఆపు;
	స్థితి 80:	బహిర్గతం_STR_NF((char_u *)"\033[?3l\033>");	ఆపు;
	అప్రమేయ:	ఆపు;
    }
    బహిర్గతం_flush();
    screen_ప్రారంభం();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    స్థిర అక్షరం		బఫరు[LNM$C_NAMLENGTH+1];
    char_u		*cp = ఖాళీ;
    గుర్తులేని దీర్ఘ	attrib;
    పూర్ణాంకం			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    అక్షరం		*sbuf = ఖాళీ;

    vul_desc(&d_lognam, (అక్షరం *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_అంశం(&itmlst.సూచీ, పరిమాణం(పూర్ణాంకం), LNM$సూచీ_2, (అక్షరం *)&idx, &dum);
    vul_అంశం(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, బఫరు, &lengte);
    itmlst.nul	= 0;
    ఒకవేళ (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, ఖాళీ,&itmlst) == SS$సాధారణ_3)
    {
	బఫరు[lengte] = '\0';
	ఒకవేళ (cp = alloc(lengte + 1))
	    strcpy((అక్షరం *)cp, బఫరు);
	తిరిగి(cp);
    }
    లేకపోతే ఒకవేళ ((sbuf = getenv((అక్షరం *)lognam)))
    {
	lengte = అక్షరశ్రేణి_పొడవు(sbuf) + 1;
	cp = alloc(lengte);
	ఒకవేళ (cp)
	    strcpy((అక్షరం *)cp, sbuf);
	తిరిగి cp;
    }
    లేకపోతే
	తిరిగి(ఖాళీ);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    పూర్ణాంకం
mch_setenv(అక్షరం *var, అక్షరం *విలువ, పూర్ణాంకం x)
{
    పూర్ణాంకం		res, dum;
    దీర్ఘ	attrib = 0L;
    అక్షరం	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_అంశం(&itmlst.equ, విలువ ? అక్షరశ్రేణి_పొడవు(విలువ) : 0, విలువ ? LNM$_STRING : 0,
	    విలువ, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    తిరిగి((res == 1) ? 0 : -1);
}

    పూర్ణాంకం
vms_sys(అక్షరం *cmd, అక్షరం *బహిర్గతం, అక్షరం *inp)
{
    DESC	cdsc, odsc, idsc;
    దీర్ఘ	స్థితి_2;

    ఒకవేళ (cmd)
	vul_desc(&cdsc, cmd);
    ఒకవేళ (బహిర్గతం)
	vul_desc(&odsc, బహిర్గతం);
    ఒకవేళ (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : ఖాళీ,		// command string
	      inp ? &idsc : ఖాళీ,		// input file
	      బహిర్గతం ? &odsc : ఖాళీ,		// output file
	      0, 0, 0, &స్థితి_2, 0, 0, 0, 0, 0, 0);
    తిరిగి స్థితి_2;
}

/*
 * Convert string to lowercase - most often filename
 */
    అక్షరం *
vms_tolower( అక్షరం *name )
{
    పూర్ణాంకం i,nlen = అక్షరశ్రేణి_పొడవు(name);
    కొరకు (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    తిరిగి name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    పూర్ణాంకం
vms_sys_స్థితి(పూర్ణాంకం స్థితి_2)
{
    ఒకవేళ (స్థితి_2 != SS$సాధారణ_3 && (స్థితి_2 & STS$M_SUCCESS) == 0)
	తిరిగి స్థితి_2;		// Command failed.
    తిరిగి 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    పూర్ణాంకం
vms_read(అక్షరం *inbuf, size_t nbytes)
{
    పూర్ణాంకం		స్థితి_2, function, len;
    TT_బహుళకం	tt_బహుళకం;
    అంశం_3	itmlst[2];     // terminates on everything
    స్థిర దీర్ఘ trm_ముసుగు[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    ఒకవేళ (!iochan)
	tt_బహుళకం = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_అంశం(&itmlst[0], 0, TRM$_MODIFIERS,
	 (అక్షరం *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_అంశం(&itmlst[1], పరిమాణం(trm_ముసుగు), TRM$పదం_2, (అక్షరం *)&trm_ముసుగు, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    స్థితి_2 = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, పరిమాణం(itmlst));
    len = అక్షరశ్రేణి_పొడవు(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    స్థితి_2 = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = అక్షరశ్రేణి_పొడవు(inbuf); // return the total length

    తిరిగి len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    స్థిర పూర్ణాంకం
vms_wproc(అక్షరం *name, పూర్ణాంకం val)
{
    పూర్ణాంకం i;
    స్థిర పూర్ణాంకం vms_సరిపోలిక_alloced = 0;

    ఒకవేళ (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	తిరిగి 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    ఒకవేళ (vms_సరిపోలిక_అంకె == 0)
    {
	// first time through, setup some things
	ఒకవేళ (ఖాళీ == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    ఒకవేళ (!vms_fmatch)
		తిరిగి 0;
	    vms_సరిపోలిక_alloced = EXPL_ALLOC_INC;
	    vms_సరిపోలిక_free = EXPL_ALLOC_INC;
	}
	లేకపోతే
	{
	    // re-use existing space
	    vms_సరిపోలిక_free = vms_సరిపోలిక_alloced;
	}
    }

    // make matches look uniform
    vms_remove_సంస్కరణ(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    కొరకు (i = 0; i<vms_సరిపోలిక_అంకె; i++)
    {
	ఒకవేళ (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    తిరిగి 1;
    }
    ఒకవేళ (--vms_సరిపోలిక_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_సరిపోలిక_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		పరిమాణం(అక్షరం **) * vms_సరిపోలిక_alloced);
	ఒకవేళ (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    తిరిగి 0;
	}
	vms_సరిపోలిక_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_సరిపోలిక_అంకె] = vim_strsave((char_u *)name);

    ++vms_సరిపోలిక_అంకె;
    తిరిగి 1;
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
    పూర్ణాంకం
mch_expand_wildcards(
    పూర్ణాంకం		అంకె_pat,
    char_u	**pat,
    పూర్ణాంకం		*అంకె_file,
    char_u	***file,
    పూర్ణాంకం		flags)
{
    పూర్ణాంకం		i, cnt = 0;
    char_u	buf[MAXPATHL];
    అక్షరం       *ఫలితం;
    పూర్ణాంకం		dir;
    పూర్ణాంకం files_alloced, files_free;

    *అంకె_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    ఒకవేళ (*file == ఖాళీ)
    {
	*అంకె_file = 0;
	తిరిగి FAIL;
    }
    కొరకు (i = 0; i < అంకె_pat; i++)
    {
	// expand environment var or home dir
	ఒకవేళ (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	లేకపోతే
	    STRCPY(buf,pat[i]);

	vms_సరిపోలిక_అంకె = 0; // reset collection counter
	ఫలితం = decc$translate_vms(vms_fixfilename(buf));
	ఒకవేళ ( (పూర్ణాంకం) ఫలితం == 0 || (పూర్ణాంకం) ఫలితం == -1  )
	{
	    cnt = 0;
	}
	లేకపోతే
	{
	    cnt = decc$గమ్యం_vms(ఫలితం, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	ఒకవేళ (cnt > 0)
	    cnt = vms_సరిపోలిక_అంకె;

	ఒకవేళ (cnt < 1)
	    కొనసాగు;

	కొరకు (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    ఒకవేళ (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		కొనసాగు;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    ఒకవేళ (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		కొనసాగు;

	    // Skip files that are not executable if we check for that.
	    ఒకవేళ (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], ఖాళీ, !(flags & EW_SHELLCMD)))
		కొనసాగు;

	    // allocate memory for pointers
	    ఒకవేళ (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, పరిమాణం(char_u **) * files_alloced);
		ఒకవేళ (*file == ఖాళీ)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *అంకె_file = 0;
		    తిరిగి(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*అంకె_file++] = vms_fmatch[i];
	}
    }
    తిరిగి OK;
}

    పూర్ణాంకం
mch_expandpath(garray_T *gap, char_u *మార్గం, పూర్ణాంకం flags)
{
    పూర్ణాంకం		i,cnt = 0;
    అక్షరం       *ఫలితం;

    vms_సరిపోలిక_అంకె = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    ఫలితం = decc$translate_vms(vms_fixfilename(మార్గం));
    ఒకవేళ ( (పూర్ణాంకం) ఫలితం == 0 || (పూర్ణాంకం) ఫలితం == -1  )
    {
	cnt = 0;
    }
    లేకపోతే
    {
	cnt = decc$గమ్యం_vms(ఫలితం, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    ఒకవేళ (cnt > 0)
	cnt = vms_సరిపోలిక_అంకె;
    కొరకు (i = 0; i < cnt; i++)
    {
	ఒకవేళ (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    తిరిగి cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    స్థిర శూన్య
vms_unix_mixed_filespec(అక్షరం *లోపల, అక్షరం *బహిర్గతం)
{
    అక్షరం *lastcolon;
    అక్షరం *చివర_యొక్క_dir;
    అక్షరం ch;
    పూర్ణాంకం len;
    అక్షరం *బహిర్గతం_str=బహిర్గతం;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(లోపల, ':');   // find last colon
    ఒకవేళ (lastcolon != ఖాళీ)
    {
	len = lastcolon - లోపల + 1;
	strncpy(బహిర్గతం, లోపల, len);
	బహిర్గతం += len;
	లోపల += len;
    }

    చివర_యొక్క_dir = ఖాళీ;	// default: no directory

    // start of directory portion
    ch = *లోపల;
    ఒకవేళ ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(లోపల);
    }
    లేకపోతే ఒకవేళ (EQN(లోపల, "../", 3))      // Unix parent directory?
    {
	*బహిర్గతం++ = '[';
	*బహిర్గతం++ = '-';
	చివర_యొక్క_dir = బహిర్గతం;
	ch = '.';
	లోపల += 2;
	SKIP_FOLLOWING_SLASHES(లోపల);
    }
    లేకపోతే
    {                                // not a special character
	అయితే (EQN(లోపల, "./", 2))     // Ignore Unix "current dir"
	{
	    లోపల += 2;
	    SKIP_FOLLOWING_SLASHES(లోపల);
    }
    ఒకవేళ (strchr(లోపల, '/') == ఖాళీ)     // any more Unix directories ?
    {
	strcpy(బహిర్గతం, లోపల);             // No - get rest of the spec
	తిరిగి;
    }
    లేకపోతే
    {
	*బహిర్గతం++ = '[';                // Yes, denote a Vms subdirectory
	ch = '.';
	--లోపల;
	}
    }

    // if we get here, there is a directory part of the filename

    // initialize output file spec
    *బహిర్గతం++ = ch;
    ++లోపల;

    అయితే (*లోపల != '\0')
    {
	ch = *లోపల;
	ఒకవేళ ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    చివర_యొక్క_dir = బహిర్గతం;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(లోపల);
	    }
	లేకపోతే ఒకవేళ (EQN(లోపల, "../", 3))     // Unix parent directory?
	{
	    *బహిర్గతం++ = '-';
	    చివర_యొక్క_dir = బహిర్గతం;
	    ch = '.';
	    లోపల += 2;
	    SKIP_FOLLOWING_SLASHES(లోపల);
	    }
	లేకపోతే
	{
	    అయితే (EQN(లోపల, "./", 2))    // Ignore Unix "current dir"
	    {
		చివర_యొక్క_dir = బహిర్గతం;
		లోపల += 2;
		SKIP_FOLLOWING_SLASHES(లోపల);
		ch = *లోపల;
	    }
	}

    // Place next character into output file spec
	*బహిర్గతం++ = ch;
	++లోపల;
    }

    *బహిర్గతం = '\0';            // Terminate output file spec

    ఒకవేళ (చివర_యొక్క_dir != ఖాళీ) // Terminate directory portion
	*చివర_యొక్క_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    స్థిర పూర్ణాంకం
vms_fspec_proc(అక్షరం *fil, పూర్ణాంకం val)
{
    strcpy(Fspec_Rms,fil);
    తిరిగి(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    శూన్య *
vms_fixfilename(శూన్య *instring)
{
    స్థిర అక్షరం		*buf = ఖాళీ;
    స్థిర size_t	buflen = 0;
    size_t		len;

    // get a big-enough buffer
    len = అక్షరశ్రేణి_పొడవు(instring) + 1;
    ఒకవేళ (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * పరిమాణం(అక్షరం));
    }

#నిర్వచితమైతే DEBUG
    అక్షరం		 *tmpbuf = ఖాళీ;
    tmpbuf = ALLOC_MULT(అక్షరం, buflen);
    strcpy(tmpbuf, instring);
#అయితే_ముగింపు

    Fspec_Rms = buf;				// for decc$to_vms

    ఒకవేళ (strchr(instring,'/') == ఖాళీ)
	// It is already a VMS file spec
	strcpy(buf, instring);
    లేకపోతే ఒకవేళ (strchr(instring,'"') == ఖాళీ)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	ఒకవేళ ( (strchr(instring,'[') == ఖాళీ) && (strchr(instring,'<') == ఖాళీ) &&
	     (strchr(instring,']') == ఖాళీ) && (strchr(instring,'>') == ఖాళీ) &&
	     (strchr(instring,':') == ఖాళీ) )
	{
	    // It must be a truly unix fspec
	    decc$గమ్యం_vms(instring, vms_fspec_proc, 0, 0);
	}
	లేకపోతే
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    లేకపోతే
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    తిరిగి buf;
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
    శూన్య
vms_remove_సంస్కరణ(శూన్య * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    పూర్ణాంకం		పూర్తి = 0;
    పూర్ణాంకం		vdigits = 0;

    rp = (char_u *)fname+ అక్షరశ్రేణి_పొడవు( (అక్షరం *)fname)- 1;    // Rightmost char
    అయితే ((పూర్తి == 0) && (rp > (char_u *)fname))       // Count digits
    {
	ఒకవేళ (అంకెనా( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	లేకపోతే
	{
	    పూర్తి = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    ఒకవేళ (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	ఒకవేళ (*rp == (char_u)';')
	{
	    ఒకవేళ ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	లేకపోతే ఒకవేళ (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    ఒకవేళ ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		పూర్తి = 0;
		అయితే ((పూర్తి == 0) && (dp >= (char_u *)fname))
		{
		    ఒకవేళ ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			ఒకవేళ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    పూర్తి = 1;   // No previous dot found in name
			}
		    }
		    లేకపోతే ఒకవేళ (*dp == '.')
		    {                   // Possible dot delimiter
			ఒకవేళ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dot delimiter
			    పూర్తి = 1;   // Previous dot found in name
			    *rp = '\0'; // Trim off ".nnn"
			}
		    }
		    dp--;               // Next char to right
		} // while
	    }
	}
	// Else no version found to remove
    } // if (vdigits  <= 5)

    తిరిగి ;
}

నిర్మాణం typeahead_st {
    గుర్తులేని పొట్టి numchars;
    గుర్తులేని అక్షరం  firstchar;
    గుర్తులేని అక్షరం  reserved0;
    గుర్తులేని దీర్ఘ  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    పూర్ణాంకం
RealWaitForChar(
    పూర్ణాంకం		fd వాడని, // always read from iochan
    దీర్ఘ	msec,
    పూర్ణాంకం		*check_for_gpm వాడని,
    పూర్ణాంకం		*interrupted)
{
    పూర్ణాంకం స్థితి_2;
    నిర్మాణం _generic_64 సమయం_curr;
    నిర్మాణం _generic_64 సమయం_diff;
    నిర్మాణం _generic_64 సమయం_బహిర్గతం;
    గుర్తులేని పూర్ణాంకం convert_operation = LIB$K_DELTA_SECONDS_F;
    దశాంశం sec =(దశాంశం) msec/1000;

    // make sure the iochan is set
    ఒకవేళ (!iochan)
	get_tty();

    ఒకవేళ (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	స్థితి_2 = sys$gettim(&సమయం_curr);
	ఒకవేళ (స్థితి_2 != SS$సాధారణ_3)
	    తిరిగి 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#ఒకవేళ __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_గమ్యం_అంతర్గత_సమయం
#  define LIB_CVTX_గమ్యం_అంతర్గత_సమయం lib$cvts_గమ్యం_అంతర్గత_సమయం // IEEE
# లేకపోతే
#  define LIB_CVTX_గమ్యం_అంతర్గత_సమయం lib$cvtf_గమ్యం_అంతర్గత_సమయం
# endif
#లేకపోతే
# define LIB_CVTX_గమ్యం_అంతర్గత_సమయం lib$cvtf_గమ్యం_అంతర్గత_సమయం // VAX
#అయితే_ముగింపు // __IEEE_FLOAT CVTS

	స్థితి_2 = LIB_CVTX_గమ్యం_అంతర్గత_సమయం(
		&convert_operation, &sec, &సమయం_diff);
	ఒకవేళ (స్థితి_2 != LIB$సాధారణ_3)
	    తిరిగి 0; // error
	// add them up
	స్థితి_2 = lib$కూడు_times(
		&సమయం_curr,
		&సమయం_diff,
		&సమయం_బహిర్గతం);
	ఒకవేళ (స్థితి_2 != LIB$సాధారణ_3)
	    తిరిగి 0; // error
    }

    అయితే (TRUE)
    {
	// select()
	స్థితి_2 = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	ఒకవేళ (స్థితి_2 != SS$సాధారణ_3 || (iosb[0] & 0xFFFF) != SS$సాధారణ_3)
	    తిరిగి 0; // error

	ఒకవేళ (typeahead.numchars)
	    తిరిగి 1; // ready to read

	// there's nothing to read; what now?
	ఒకవేళ (msec == 0)
	{
	    // immediate time-out; return impatiently
	    తిరిగి 0;
	}
	లేకపోతే ఒకవేళ (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    తిరిగి 1; // fakeout to force a wait in vms_read()
	}
	లేకపోతే
	{
	    // time-out needs to be checked
	    స్థితి_2 = sys$gettim(&సమయం_curr);
	    ఒకవేళ (స్థితి_2 != SS$సాధారణ_3)
		తిరిగి 0; // error

	    స్థితి_2 = lib$sub_times(
		    &సమయం_బహిర్గతం,
		    &సమయం_curr,
		    &సమయం_diff);
	    ఒకవేళ (స్థితి_2 != LIB$సాధారణ_3)
		తిరిగి 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#ఒకవేళ !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

రకనిర్వచనం నిర్మాణం
   {
   అక్షరం *name;
   పూర్ణాంకం విలువ;
   } decc_feat_t;

పూర్ణాంకం vms_init_పూర్తి = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (అక్షరం *)ఖాళీ, 0 } };


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

స్థిర శూన్య
vms_init(శూన్య)
{
    // Set the global flag to indicate that LIB$INITIALIZE worked

    vms_init_పూర్తి = 1;

    పూర్ణాంకం feat_సూచీ;
    పూర్ణాంకం feat_విలువ;
    పూర్ణాంకం feat_విలువ_max;
    పూర్ణాంకం feat_విలువ_min;
    పూర్ణాంకం i;
    పూర్ణాంకం sts;

    // Loop through all items in the decc_feat_array[]
    కొరకు (i = 0; decc_feat_array[i].name != ఖాళీ; i++)
    {
	// Get the feature index
	feat_సూచీ = decc$feature_get_సూచీ(decc_feat_array[i].name);
	ఒకవేళ (feat_సూచీ >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_విలువ = decc$feature_get_విలువ(feat_సూచీ, 1);
	    feat_విలువ_min = decc$feature_get_విలువ(feat_సూచీ, 2);
	    feat_విలువ_max = decc$feature_get_విలువ(feat_సూచీ, 3);

	    ఒకవేళ ((decc_feat_array[i].విలువ >= feat_విలువ_min) && (decc_feat_array[i].విలువ <= feat_విలువ_max))
	    {
		// Valid value.  Set it if necessary
		ఒకవేళ (feat_విలువ != decc_feat_array[i].విలువ)
		    sts = decc$feature_సమితి_విలువ(feat_సూచీ, 1, decc_feat_array[i].విలువ);
	    }
	    లేకపోతే
		// Invalid DECC feature value
		రాయి("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_విలువ, feat_విలువ_min, decc_feat_array[i].name, feat_విలువ_max);
	}
	లేకపోతే
	    // Invalid DECC feature name
	    రాయి("UNKNOWN DECC FEATURE: %s.\n", decc_feat_array[i].name);
    }
}


/* Get "vms_init()" into a valid, loaded LIB$INITIALIZE PSECT. */

# pragma nostandard

/* Establish the LIB$INITIALIZE PSECTs, with proper alignment and
 * other attributes.  Note that "nopic" is significant only on VAX.
 */
# pragma extern_model save

# pragma extern_model strict_refdef "LIB$INITIALIZE" 2, nopic, nowrt
శూన్య (*స్థిరరాశి x_vms_init)() = vms_init;

# pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
స్థిరరాశి పూర్ణాంకం అదనపు[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
పూర్ణాంకం LIB$INITIALIZE(శూన్య);
# pragma extern_model strict_refdef
పూర్ణాంకం dmy_lib$initialize = (పూర్ణాంకం) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#అయితే_ముగింపు // !defined( __VAX) && (__CRTL_VER >= 70301000)
