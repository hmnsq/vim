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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

// define _generic_64 for use in time functions
#ਜੇ !defined(VAX)
# include <gen64def.h>
#ਨਹੀਂਤਾਂ
// based on Alpha's gen64def.h; the file is absent on VAX
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ gen64$l_longword [2];   // ...two 32-bit values, or
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਪੂਰਨ gen64$w_ਸ਼ਬਦ [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਅੱਖਰ	class;
    ਅੱਖਰ	type;
    ਛੋਟਾ	ਚੌੜਾਈ;
    ਸੰਘ
    {
	ਬਣਤਰ
	{
	    ਅੱਖਰ	_basic[3];
	    ਅੱਖਰ	ਲੰਮਾਈ;
	}	y;
	ਪੂਰਨ	basic;
    }	x;
    ਪੂਰਨ		extended;
}	TT_ਬਹੁਲਕ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਛੋਟਾ	buflen;
    ਛੋਟਾ	itemcode;
    ਅੱਖਰ	*bufadrs;
    ਪੂਰਨ		*retlen;
}	ਵਸਤੂ_3;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਵਸਤੂ_3	equ;
    ਪੂਰਨ		nul;
}	ITMLST1;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਵਸਤੂ_3	ਸੂਚਕਾਂਕ;
    ਵਸਤੂ_3	string;
    ਪੂਰਨ	nul;
}	ITMLST2;

ਸਥਿਰ TT_ਬਹੁਲਕ	orgmode;
ਸਥਿਰ ਛੋਟਾ	iochan;			// TTY I/O channel
ਸਥਿਰ ਛੋਟਾ	iosb[4];		// IO status block

ਸਥਿਰ ਪੂਰਨ vms_ਮੇਲ_ਅੰਕ = 0;
ਸਥਿਰ ਪੂਰਨ vms_ਮੇਲ_free = 0;
ਸਥਿਰ char_u **vms_fmatch = ਨਿਰਾ;
ਸਥਿਰ ਅੱਖਰ *Fspec_Rms;		       // rms file spec, passed implicitly between routines



ਸਥਿਰ TT_ਬਹੁਲਕ	get_tty(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ	ਸੈੱਟ_tty(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPL_ALLOC_INC 64

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EQN(S1,S2,LN) (ਲੜੀ_ਤੁਲਨਾ(S1,S2,LN) == 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SKIP_FOLLOWING_SLASHES(Str) ਕਰੋ { ਜਦੋਂ (Str[1] == '/') ++Str; } ਜਦੋਂ (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    ਸਥਿਰ ਖਾਲੀ
vul_desc(DESC *des, ਅੱਖਰ *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_ਸੂਚਕ = str;
    des->dsc$w_ਲੰਮਾਈ = str ? ਲੜੀ_ਲੰਬਾਈ(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    ਸਥਿਰ ਖਾਲੀ
vul_ਵਸਤੂ(ਵਸਤੂ_3 *itm, ਛੋਟਾ len, ਛੋਟਾ cod, ਅੱਖਰ *adr, ਪੂਰਨ *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    ਖਾਲੀ
mch_settmode(tmode_T tmode)
{
    ਪੂਰਨ	ਸਥਿਤੀ;

    ਜੇ ( tmode == TMODE_RAW )
	ਸੈੱਟ_tty(0, 0);
    ਨਹੀਂਤਾਂ
    {
	ਚੋਣ (orgmode.ਚੌੜਾਈ)
	{
	    ਹਾਲਤ 132:	ਬਾਹਰਲਾ_STR_NF((char_u *)"\033[?3h\033>");	ਰੋਕੋ;
	    ਹਾਲਤ 80:	ਬਾਹਰਲਾ_STR_NF((char_u *)"\033[?3l\033>");	ਰੋਕੋ;
	    ਮੂਲ:	ਰੋਕੋ;
	}
	ਬਾਹਰਲਾ_flush();
	ਸਥਿਤੀ = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, ਆਕਾਰ(TT_ਬਹੁਲਕ), 0,0,0,0);
	ਜੇ (ਸਥਿਤੀ!=SS$ਸਾਧਾਰਨ_3 || (iosb[0]&0xFFFF)!=SS$ਸਾਧਾਰਨ_3)
	    ਵਾਪਸ;
	(ਖਾਲੀ)sys$dassgn(iochan);
	iochan = 0;
    }
}

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_tty(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਪੂਰਨ		    ਸਥਿਤੀ;
    TT_ਬਹੁਲਕ	    newmode;		// New TTY mode bits
    ਸਥਿਰ ਛੋਟਾ    ਪਹਿਲਾ_ਸਮਾਂ = TRUE;

    ਜੇ (ਪਹਿਲਾ_ਸਮਾਂ)
    {
	orgmode = get_tty();
	ਪਹਿਲਾ_ਸਮਾਂ = FALSE;
    }
    newmode = get_tty();
    ਜੇ (ਖਾਨਾ)
	newmode.ਚੌੜਾਈ		 = ਖਾਨਾ;
    ਜੇ (ਪੰਕਤੀ)
	newmode.x.y.ਲੰਮਾਈ       = ਪੰਕਤੀ;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    ਸਥਿਤੀ = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, ਆਕਾਰ(newmode), 0, 0, 0, 0);
    ਜੇ (ਸਥਿਤੀ!=SS$ਸਾਧਾਰਨ_3 || (iosb[0]&0xFFFF)!=SS$ਸਾਧਾਰਨ_3)
	ਵਾਪਸ;
}

    ਸਥਿਰ TT_ਬਹੁਲਕ
get_tty(ਖਾਲੀ)
{

    ਸਥਿਰ $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    ਪੂਰਨ		ਸਥਿਤੀ;
    TT_ਬਹੁਲਕ	tt_ਬਹੁਲਕ;

    ਜੇ (!iochan)
	ਸਥਿਤੀ = sys$ਸੌਂਪੋ(&odsc,&iochan,0,0);

    ਸਥਿਤੀ = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_ਬਹੁਲਕ, ਆਕਾਰ(tt_ਬਹੁਲਕ), 0, 0, 0, 0);
    ਜੇ (ਸਥਿਤੀ != SS$ਸਾਧਾਰਨ_3 || (iosb[0] & 0xFFFF) != SS$ਸਾਧਾਰਨ_3)
    {
	tt_ਬਹੁਲਕ.ਚੌੜਾਈ		= 0;
	tt_ਬਹੁਲਕ.type		= 0;
	tt_ਬਹੁਲਕ.class		= 0;
	tt_ਬਹੁਲਕ.x.basic		= 0;
	tt_ਬਹੁਲਕ.x.y.ਲੰਮਾਈ	= 0;
	tt_ਬਹੁਲਕ.extended	= 0;
    }
    ਵਾਪਸ(tt_ਬਹੁਲਕ);
}

/*
 * Get the current window size in Rows and Columns.
 */
    ਪੂਰਨ
mch_get_shellsize(ਖਾਲੀ)
{
    TT_ਬਹੁਲਕ	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.ਚੌੜਾਈ;
    ਪੰਕਤੀਆਂ = tmode.x.y.ਲੰਮਾਈ;
    ਵਾਪਸ OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    ਖਾਲੀ
mch_ਸੈੱਟ_shellsize(ਖਾਲੀ)
{
    ਸੈੱਟ_tty(ਪੰਕਤੀਆਂ, Columns);
    ਚੋਣ (Columns)
    {
	ਹਾਲਤ 132:	ਬਾਹਰਲਾ_STR_NF((char_u *)"\033[?3h\033>");	ਰੋਕੋ;
	ਹਾਲਤ 80:	ਬਾਹਰਲਾ_STR_NF((char_u *)"\033[?3l\033>");	ਰੋਕੋ;
	ਮੂਲ:	ਰੋਕੋ;
    }
    ਬਾਹਰਲਾ_flush();
    screen_ਸ਼ੁਰੂ();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    ਸਥਿਰ ਅੱਖਰ		ਬਫਰ[LNM$C_NAMLENGTH+1];
    char_u		*cp = ਨਿਰਾ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ	attrib;
    ਪੂਰਨ			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    ਅੱਖਰ		*sbuf = ਨਿਰਾ;

    vul_desc(&d_lognam, (ਅੱਖਰ *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_ਵਸਤੂ(&itmlst.ਸੂਚਕਾਂਕ, ਆਕਾਰ(ਪੂਰਨ), LNM$ਸੂਚਕਾਂਕ_2, (ਅੱਖਰ *)&idx, &dum);
    vul_ਵਸਤੂ(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, ਬਫਰ, &lengte);
    itmlst.nul	= 0;
    ਜੇ (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, ਨਿਰਾ,&itmlst) == SS$ਸਾਧਾਰਨ_3)
    {
	ਬਫਰ[lengte] = '\0';
	ਜੇ (cp = alloc(lengte + 1))
	    strcpy((ਅੱਖਰ *)cp, ਬਫਰ);
	ਵਾਪਸ(cp);
    }
    ਨਹੀਂਤਾਂ ਜੇ ((sbuf = getenv((ਅੱਖਰ *)lognam)))
    {
	lengte = ਲੜੀ_ਲੰਬਾਈ(sbuf) + 1;
	cp = alloc(lengte);
	ਜੇ (cp)
	    strcpy((ਅੱਖਰ *)cp, sbuf);
	ਵਾਪਸ cp;
    }
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ(ਨਿਰਾ);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    ਪੂਰਨ
mch_setenv(ਅੱਖਰ *var, ਅੱਖਰ *ਮੁੱਲ, ਪੂਰਨ x)
{
    ਪੂਰਨ		res, dum;
    ਲੰਮਾ	attrib = 0L;
    ਅੱਖਰ	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_ਵਸਤੂ(&itmlst.equ, ਮੁੱਲ ? ਲੜੀ_ਲੰਬਾਈ(ਮੁੱਲ) : 0, ਮੁੱਲ ? LNM$_STRING : 0,
	    ਮੁੱਲ, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    ਵਾਪਸ((res == 1) ? 0 : -1);
}

    ਪੂਰਨ
vms_sys(ਅੱਖਰ *cmd, ਅੱਖਰ *ਬਾਹਰਲਾ, ਅੱਖਰ *inp)
{
    DESC	cdsc, odsc, idsc;
    ਲੰਮਾ	ਸਥਿਤੀ;

    ਜੇ (cmd)
	vul_desc(&cdsc, cmd);
    ਜੇ (ਬਾਹਰਲਾ)
	vul_desc(&odsc, ਬਾਹਰਲਾ);
    ਜੇ (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : ਨਿਰਾ,		// command string
	      inp ? &idsc : ਨਿਰਾ,		// input file
	      ਬਾਹਰਲਾ ? &odsc : ਨਿਰਾ,		// output file
	      0, 0, 0, &ਸਥਿਤੀ, 0, 0, 0, 0, 0, 0);
    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * Convert string to lowercase - most often filename
 */
    ਅੱਖਰ *
vms_tolower( ਅੱਖਰ *name )
{
    ਪੂਰਨ i,nlen = ਲੜੀ_ਲੰਬਾਈ(name);
    ਲਈ (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    ਵਾਪਸ name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    ਪੂਰਨ
vms_sys_ਸਥਿਤੀ(ਪੂਰਨ ਸਥਿਤੀ)
{
    ਜੇ (ਸਥਿਤੀ != SS$ਸਾਧਾਰਨ_3 && (ਸਥਿਤੀ & STS$M_SUCCESS) == 0)
	ਵਾਪਸ ਸਥਿਤੀ;		// Command failed.
    ਵਾਪਸ 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    ਪੂਰਨ
vms_read(ਅੱਖਰ *inbuf, size_t nbytes)
{
    ਪੂਰਨ		ਸਥਿਤੀ, function, len;
    TT_ਬਹੁਲਕ	tt_ਬਹੁਲਕ;
    ਵਸਤੂ_3	itmlst[2];     // terminates on everything
    ਸਥਿਰ ਲੰਮਾ trm_ਨਕਾਬ[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    ਜੇ (!iochan)
	tt_ਬਹੁਲਕ = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_ਵਸਤੂ(&itmlst[0], 0, TRM$_MODIFIERS,
	 (ਅੱਖਰ *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_ਵਸਤੂ(&itmlst[1], ਆਕਾਰ(trm_ਨਕਾਬ), TRM$ਪਦ_2, (ਅੱਖਰ *)&trm_ਨਕਾਬ, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    ਸਥਿਤੀ = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, ਆਕਾਰ(itmlst));
    len = ਲੜੀ_ਲੰਬਾਈ(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    ਸਥਿਤੀ = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = ਲੜੀ_ਲੰਬਾਈ(inbuf); // return the total length

    ਵਾਪਸ len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    ਸਥਿਰ ਪੂਰਨ
vms_wproc(ਅੱਖਰ *name, ਪੂਰਨ val)
{
    ਪੂਰਨ i;
    ਸਥਿਰ ਪੂਰਨ vms_ਮੇਲ_alloced = 0;

    ਜੇ (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	ਵਾਪਸ 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    ਜੇ (vms_ਮੇਲ_ਅੰਕ == 0)
    {
	// first time through, setup some things
	ਜੇ (ਨਿਰਾ == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    ਜੇ (!vms_fmatch)
		ਵਾਪਸ 0;
	    vms_ਮੇਲ_alloced = EXPL_ALLOC_INC;
	    vms_ਮੇਲ_free = EXPL_ALLOC_INC;
	}
	ਨਹੀਂਤਾਂ
	{
	    // re-use existing space
	    vms_ਮੇਲ_free = vms_ਮੇਲ_alloced;
	}
    }

    // make matches look uniform
    vms_remove_ਸੰਸਕਰਣ(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    ਲਈ (i = 0; i<vms_ਮੇਲ_ਅੰਕ; i++)
    {
	ਜੇ (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    ਵਾਪਸ 1;
    }
    ਜੇ (--vms_ਮੇਲ_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_ਮੇਲ_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		ਆਕਾਰ(ਅੱਖਰ **) * vms_ਮੇਲ_alloced);
	ਜੇ (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    ਵਾਪਸ 0;
	}
	vms_ਮੇਲ_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_ਮੇਲ_ਅੰਕ] = vim_strsave((char_u *)name);

    ++vms_ਮੇਲ_ਅੰਕ;
    ਵਾਪਸ 1;
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
    ਪੂਰਨ
mch_expand_wildcards(
    ਪੂਰਨ		ਅੰਕ_pat,
    char_u	**pat,
    ਪੂਰਨ		*ਅੰਕ_file,
    char_u	***file,
    ਪੂਰਨ		flags)
{
    ਪੂਰਨ		i, cnt = 0;
    char_u	buf[MAXPATHL];
    ਅੱਖਰ       *ਨਤੀਜਾ;
    ਪੂਰਨ		dir;
    ਪੂਰਨ files_alloced, files_free;

    *ਅੰਕ_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    ਜੇ (*file == ਨਿਰਾ)
    {
	*ਅੰਕ_file = 0;
	ਵਾਪਸ FAIL;
    }
    ਲਈ (i = 0; i < ਅੰਕ_pat; i++)
    {
	// expand environment var or home dir
	ਜੇ (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	ਨਹੀਂਤਾਂ
	    STRCPY(buf,pat[i]);

	vms_ਮੇਲ_ਅੰਕ = 0; // reset collection counter
	ਨਤੀਜਾ = decc$translate_vms(vms_fixfilename(buf));
	ਜੇ ( (ਪੂਰਨ) ਨਤੀਜਾ == 0 || (ਪੂਰਨ) ਨਤੀਜਾ == -1  )
	{
	    cnt = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    cnt = decc$ਮੰਜ਼ਿਲ_vms(ਨਤੀਜਾ, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	ਜੇ (cnt > 0)
	    cnt = vms_ਮੇਲ_ਅੰਕ;

	ਜੇ (cnt < 1)
	    ਜਾਰੀ;

	ਲਈ (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    ਜੇ (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		ਜਾਰੀ;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    ਜੇ (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		ਜਾਰੀ;

	    // Skip files that are not executable if we check for that.
	    ਜੇ (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], ਨਿਰਾ, !(flags & EW_SHELLCMD)))
		ਜਾਰੀ;

	    // allocate memory for pointers
	    ਜੇ (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, ਆਕਾਰ(char_u **) * files_alloced);
		ਜੇ (*file == ਨਿਰਾ)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *ਅੰਕ_file = 0;
		    ਵਾਪਸ(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*ਅੰਕ_file++] = vms_fmatch[i];
	}
    }
    ਵਾਪਸ OK;
}

    ਪੂਰਨ
mch_expandpath(garray_T *gap, char_u *ਰਸਤਾ, ਪੂਰਨ flags)
{
    ਪੂਰਨ		i,cnt = 0;
    ਅੱਖਰ       *ਨਤੀਜਾ;

    vms_ਮੇਲ_ਅੰਕ = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    ਨਤੀਜਾ = decc$translate_vms(vms_fixfilename(ਰਸਤਾ));
    ਜੇ ( (ਪੂਰਨ) ਨਤੀਜਾ == 0 || (ਪੂਰਨ) ਨਤੀਜਾ == -1  )
    {
	cnt = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	cnt = decc$ਮੰਜ਼ਿਲ_vms(ਨਤੀਜਾ, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    ਜੇ (cnt > 0)
	cnt = vms_ਮੇਲ_ਅੰਕ;
    ਲਈ (i = 0; i < cnt; i++)
    {
	ਜੇ (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    ਵਾਪਸ cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    ਸਥਿਰ ਖਾਲੀ
vms_unix_mixed_filespec(ਅੱਖਰ *ਵਿੱਚ, ਅੱਖਰ *ਬਾਹਰਲਾ)
{
    ਅੱਖਰ *lastcolon;
    ਅੱਖਰ *ਸਿਰਾ_ਦਾ_dir;
    ਅੱਖਰ ch;
    ਪੂਰਨ len;
    ਅੱਖਰ *ਬਾਹਰਲਾ_str=ਬਾਹਰਲਾ;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(ਵਿੱਚ, ':');   // find last colon
    ਜੇ (lastcolon != ਨਿਰਾ)
    {
	len = lastcolon - ਵਿੱਚ + 1;
	strncpy(ਬਾਹਰਲਾ, ਵਿੱਚ, len);
	ਬਾਹਰਲਾ += len;
	ਵਿੱਚ += len;
    }

    ਸਿਰਾ_ਦਾ_dir = ਨਿਰਾ;	// default: no directory

    // start of directory portion
    ch = *ਵਿੱਚ;
    ਜੇ ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(ਵਿੱਚ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (EQN(ਵਿੱਚ, "../", 3))      // Unix parent directory?
    {
	*ਬਾਹਰਲਾ++ = '[';
	*ਬਾਹਰਲਾ++ = '-';
	ਸਿਰਾ_ਦਾ_dir = ਬਾਹਰਲਾ;
	ch = '.';
	ਵਿੱਚ += 2;
	SKIP_FOLLOWING_SLASHES(ਵਿੱਚ);
    }
    ਨਹੀਂਤਾਂ
    {                                // not a special character
	ਜਦੋਂ (EQN(ਵਿੱਚ, "./", 2))     // Ignore Unix "current dir"
	{
	    ਵਿੱਚ += 2;
	    SKIP_FOLLOWING_SLASHES(ਵਿੱਚ);
    }
    ਜੇ (strchr(ਵਿੱਚ, '/') == ਨਿਰਾ)     // any more Unix directories ?
    {
	strcpy(ਬਾਹਰਲਾ, ਵਿੱਚ);             // No - get rest of the spec
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ
    {
	*ਬਾਹਰਲਾ++ = '[';                // Yes, denote a Vms subdirectory
	ch = '.';
	--ਵਿੱਚ;
	}
    }

    // if we get here, there is a directory part of the filename

    // initialize output file spec
    *ਬਾਹਰਲਾ++ = ch;
    ++ਵਿੱਚ;

    ਜਦੋਂ (*ਵਿੱਚ != '\0')
    {
	ch = *ਵਿੱਚ;
	ਜੇ ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    ਸਿਰਾ_ਦਾ_dir = ਬਾਹਰਲਾ;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(ਵਿੱਚ);
	    }
	ਨਹੀਂਤਾਂ ਜੇ (EQN(ਵਿੱਚ, "../", 3))     // Unix parent directory?
	{
	    *ਬਾਹਰਲਾ++ = '-';
	    ਸਿਰਾ_ਦਾ_dir = ਬਾਹਰਲਾ;
	    ch = '.';
	    ਵਿੱਚ += 2;
	    SKIP_FOLLOWING_SLASHES(ਵਿੱਚ);
	    }
	ਨਹੀਂਤਾਂ
	{
	    ਜਦੋਂ (EQN(ਵਿੱਚ, "./", 2))    // Ignore Unix "current dir"
	    {
		ਸਿਰਾ_ਦਾ_dir = ਬਾਹਰਲਾ;
		ਵਿੱਚ += 2;
		SKIP_FOLLOWING_SLASHES(ਵਿੱਚ);
		ch = *ਵਿੱਚ;
	    }
	}

    // Place next character into output file spec
	*ਬਾਹਰਲਾ++ = ch;
	++ਵਿੱਚ;
    }

    *ਬਾਹਰਲਾ = '\0';            // Terminate output file spec

    ਜੇ (ਸਿਰਾ_ਦਾ_dir != ਨਿਰਾ) // Terminate directory portion
	*ਸਿਰਾ_ਦਾ_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    ਸਥਿਰ ਪੂਰਨ
vms_fspec_proc(ਅੱਖਰ *fil, ਪੂਰਨ val)
{
    strcpy(Fspec_Rms,fil);
    ਵਾਪਸ(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    ਖਾਲੀ *
vms_fixfilename(ਖਾਲੀ *instring)
{
    ਸਥਿਰ ਅੱਖਰ		*buf = ਨਿਰਾ;
    ਸਥਿਰ size_t	buflen = 0;
    size_t		len;

    // get a big-enough buffer
    len = ਲੜੀ_ਲੰਬਾਈ(instring) + 1;
    ਜੇ (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * ਆਕਾਰ(ਅੱਖਰ));
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
    ਅੱਖਰ		 *tmpbuf = ਨਿਰਾ;
    tmpbuf = ALLOC_MULT(ਅੱਖਰ, buflen);
    strcpy(tmpbuf, instring);
#ਜੇ_ਅੰਤ

    Fspec_Rms = buf;				// for decc$to_vms

    ਜੇ (strchr(instring,'/') == ਨਿਰਾ)
	// It is already a VMS file spec
	strcpy(buf, instring);
    ਨਹੀਂਤਾਂ ਜੇ (strchr(instring,'"') == ਨਿਰਾ)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	ਜੇ ( (strchr(instring,'[') == ਨਿਰਾ) && (strchr(instring,'<') == ਨਿਰਾ) &&
	     (strchr(instring,']') == ਨਿਰਾ) && (strchr(instring,'>') == ਨਿਰਾ) &&
	     (strchr(instring,':') == ਨਿਰਾ) )
	{
	    // It must be a truly unix fspec
	    decc$ਮੰਜ਼ਿਲ_vms(instring, vms_fspec_proc, 0, 0);
	}
	ਨਹੀਂਤਾਂ
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    ਨਹੀਂਤਾਂ
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    ਵਾਪਸ buf;
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
    ਖਾਲੀ
vms_remove_ਸੰਸਕਰਣ(ਖਾਲੀ * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    ਪੂਰਨ		ਪੂਰਾ = 0;
    ਪੂਰਨ		vdigits = 0;

    rp = (char_u *)fname+ ਲੜੀ_ਲੰਬਾਈ( (ਅੱਖਰ *)fname)- 1;    // Rightmost char
    ਜਦੋਂ ((ਪੂਰਾ == 0) && (rp > (char_u *)fname))       // Count digits
    {
	ਜੇ (ਅੰਕ_ਹੈ( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਾ = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    ਜੇ (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	ਜੇ (*rp == (char_u)';')
	{
	    ਜੇ ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    ਜੇ ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		ਪੂਰਾ = 0;
		ਜਦੋਂ ((ਪੂਰਾ == 0) && (dp >= (char_u *)fname))
		{
		    ਜੇ ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			ਜੇ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    ਪੂਰਾ = 1;   // No previous dot found in name
			}
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (*dp == '.')
		    {                   // Possible dot delimiter
			ਜੇ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dot delimiter
			    ਪੂਰਾ = 1;   // Previous dot found in name
			    *rp = '\0'; // Trim off ".nnn"
			}
		    }
		    dp--;               // Next char to right
		} // while
	    }
	}
	// Else no version found to remove
    } // if (vdigits  <= 5)

    ਵਾਪਸ ;
}

ਬਣਤਰ typeahead_st {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ numchars;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ  firstchar;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ  reserved0;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    ਪੂਰਨ
RealWaitForChar(
    ਪੂਰਨ		fd ਅਣਵਰਤਿਆ, // always read from iochan
    ਲੰਮਾ	msec,
    ਪੂਰਨ		*check_for_gpm ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		*interrupted)
{
    ਪੂਰਨ ਸਥਿਤੀ;
    ਬਣਤਰ _generic_64 ਸਮਾਂ_curr;
    ਬਣਤਰ _generic_64 ਸਮਾਂ_diff;
    ਬਣਤਰ _generic_64 ਸਮਾਂ_ਬਾਹਰਲਾ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ convert_operation = LIB$K_DELTA_SECONDS_F;
    ਦਸ਼ਮਲਵ sec =(ਦਸ਼ਮਲਵ) msec/1000;

    // make sure the iochan is set
    ਜੇ (!iochan)
	get_tty();

    ਜੇ (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	ਸਥਿਤੀ = sys$gettim(&ਸਮਾਂ_curr);
	ਜੇ (ਸਥਿਤੀ != SS$ਸਾਧਾਰਨ_3)
	    ਵਾਪਸ 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#ਜੇ __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_ਮੰਜ਼ਿਲ_ਅੰਦਰੂਨੀ_ਸਮਾਂ
#  define LIB_CVTX_ਮੰਜ਼ਿਲ_ਅੰਦਰੂਨੀ_ਸਮਾਂ lib$cvts_ਮੰਜ਼ਿਲ_ਅੰਦਰੂਨੀ_ਸਮਾਂ // IEEE
# ਨਹੀਂਤਾਂ
#  define LIB_CVTX_ਮੰਜ਼ਿਲ_ਅੰਦਰੂਨੀ_ਸਮਾਂ lib$cvtf_ਮੰਜ਼ਿਲ_ਅੰਦਰੂਨੀ_ਸਮਾਂ
# endif
#ਨਹੀਂਤਾਂ
# define LIB_CVTX_ਮੰਜ਼ਿਲ_ਅੰਦਰੂਨੀ_ਸਮਾਂ lib$cvtf_ਮੰਜ਼ਿਲ_ਅੰਦਰੂਨੀ_ਸਮਾਂ // VAX
#ਜੇ_ਅੰਤ // __IEEE_FLOAT CVTS

	ਸਥਿਤੀ = LIB_CVTX_ਮੰਜ਼ਿਲ_ਅੰਦਰੂਨੀ_ਸਮਾਂ(
		&convert_operation, &sec, &ਸਮਾਂ_diff);
	ਜੇ (ਸਥਿਤੀ != LIB$ਸਾਧਾਰਨ_3)
	    ਵਾਪਸ 0; // error
	// add them up
	ਸਥਿਤੀ = lib$ਜੋੜੋ_times(
		&ਸਮਾਂ_curr,
		&ਸਮਾਂ_diff,
		&ਸਮਾਂ_ਬਾਹਰਲਾ);
	ਜੇ (ਸਥਿਤੀ != LIB$ਸਾਧਾਰਨ_3)
	    ਵਾਪਸ 0; // error
    }

    ਜਦੋਂ (TRUE)
    {
	// select()
	ਸਥਿਤੀ = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	ਜੇ (ਸਥਿਤੀ != SS$ਸਾਧਾਰਨ_3 || (iosb[0] & 0xFFFF) != SS$ਸਾਧਾਰਨ_3)
	    ਵਾਪਸ 0; // error

	ਜੇ (typeahead.numchars)
	    ਵਾਪਸ 1; // ready to read

	// there's nothing to read; what now?
	ਜੇ (msec == 0)
	{
	    // immediate time-out; return impatiently
	    ਵਾਪਸ 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    ਵਾਪਸ 1; // fakeout to force a wait in vms_read()
	}
	ਨਹੀਂਤਾਂ
	{
	    // time-out needs to be checked
	    ਸਥਿਤੀ = sys$gettim(&ਸਮਾਂ_curr);
	    ਜੇ (ਸਥਿਤੀ != SS$ਸਾਧਾਰਨ_3)
		ਵਾਪਸ 0; // error

	    ਸਥਿਤੀ = lib$sub_times(
		    &ਸਮਾਂ_ਬਾਹਰਲਾ,
		    &ਸਮਾਂ_curr,
		    &ਸਮਾਂ_diff);
	    ਜੇ (ਸਥਿਤੀ != LIB$ਸਾਧਾਰਨ_3)
		ਵਾਪਸ 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#ਜੇ !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
   {
   ਅੱਖਰ *name;
   ਪੂਰਨ ਮੁੱਲ;
   } decc_feat_t;

ਪੂਰਨ vms_init_ਪੂਰਾ = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (ਅੱਖਰ *)ਨਿਰਾ, 0 } };


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

ਸਥਿਰ ਖਾਲੀ
vms_init(ਖਾਲੀ)
{
    // Set the global flag to indicate that LIB$INITIALIZE worked

    vms_init_ਪੂਰਾ = 1;

    ਪੂਰਨ feat_ਸੂਚਕਾਂਕ;
    ਪੂਰਨ feat_ਮੁੱਲ;
    ਪੂਰਨ feat_ਮੁੱਲ_max;
    ਪੂਰਨ feat_ਮੁੱਲ_min;
    ਪੂਰਨ i;
    ਪੂਰਨ sts;

    // Loop through all items in the decc_feat_array[]
    ਲਈ (i = 0; decc_feat_array[i].name != ਨਿਰਾ; i++)
    {
	// Get the feature index
	feat_ਸੂਚਕਾਂਕ = decc$feature_get_ਸੂਚਕਾਂਕ(decc_feat_array[i].name);
	ਜੇ (feat_ਸੂਚਕਾਂਕ >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_ਮੁੱਲ = decc$feature_get_ਮੁੱਲ(feat_ਸੂਚਕਾਂਕ, 1);
	    feat_ਮੁੱਲ_min = decc$feature_get_ਮੁੱਲ(feat_ਸੂਚਕਾਂਕ, 2);
	    feat_ਮੁੱਲ_max = decc$feature_get_ਮੁੱਲ(feat_ਸੂਚਕਾਂਕ, 3);

	    ਜੇ ((decc_feat_array[i].ਮੁੱਲ >= feat_ਮੁੱਲ_min) && (decc_feat_array[i].ਮੁੱਲ <= feat_ਮੁੱਲ_max))
	    {
		// Valid value.  Set it if necessary
		ਜੇ (feat_ਮੁੱਲ != decc_feat_array[i].ਮੁੱਲ)
		    sts = decc$feature_ਸੈੱਟ_ਮੁੱਲ(feat_ਸੂਚਕਾਂਕ, 1, decc_feat_array[i].ਮੁੱਲ);
	    }
	    ਨਹੀਂਤਾਂ
		// Invalid DECC feature value
		ਲਿਖੋ("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_ਮੁੱਲ, feat_ਮੁੱਲ_min, decc_feat_array[i].name, feat_ਮੁੱਲ_max);
	}
	ਨਹੀਂਤਾਂ
	    // Invalid DECC feature name
	    ਲਿਖੋ("UNKNOWN DECC FEATURE: %s.\n", decc_feat_array[i].name);
    }
}


/* Get "vms_init()" into a valid, loaded LIB$INITIALIZE PSECT. */

# pragma nostandard

/* Establish the LIB$INITIALIZE PSECTs, with proper alignment and
 * other attributes.  Note that "nopic" is significant only on VAX.
 */
# pragma extern_model save

# pragma extern_model strict_refdef "LIB$INITIALIZE" 2, nopic, nowrt
ਖਾਲੀ (*ਅਚਲ x_vms_init)() = vms_init;

# pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
ਅਚਲ ਪੂਰਨ ਵਾਧੂ[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
ਪੂਰਨ LIB$INITIALIZE(ਖਾਲੀ);
# pragma extern_model strict_refdef
ਪੂਰਨ dmy_lib$initialize = (ਪੂਰਨ) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#ਜੇ_ਅੰਤ // !defined( __VAX) && (__CRTL_VER >= 70301000)
