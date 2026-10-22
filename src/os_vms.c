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

#ಅನ್ತರ್ಭಾವ "vim.h"

// define _generic_64 for use in time functions
#ಆದರೆ !defined(VAX)
# include <gen64def.h>
#ಇಲ್ಲದಿದ್ದರೆ
// based on Alpha's gen64def.h; the file is absent on VAX
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ gen64$l_longword [2];   // ...two 32-bit values, or
	ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಪೂರ್ಣಾಂಕ gen64$w_ಶಬ್ದ [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಅಕ್ಷರ	class;
    ಅಕ್ಷರ	type;
    ಗಿಡ್ಡ	ಅಗಲ;
    ಒಕ್ಕೂಟ
    {
	ರಚನೆ
	{
	    ಅಕ್ಷರ	_basic[3];
	    ಅಕ್ಷರ	ಉದ್ದಳತೆ;
	}	y;
	ಪೂರ್ಣಾಂಕ	basic;
    }	x;
    ಪೂರ್ಣಾಂಕ		extended;
}	TT_ಬಹುಲಕ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಗಿಡ್ಡ	buflen;
    ಗಿಡ್ಡ	itemcode;
    ಅಕ್ಷರ	*bufadrs;
    ಪೂರ್ಣಾಂಕ		*retlen;
}	ಅಂಶ_3;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಅಂಶ_3	equ;
    ಪೂರ್ಣಾಂಕ		nul;
}	ITMLST1;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಅಂಶ_3	ಸೂಚ್ಯಂಕ;
    ಅಂಶ_3	string;
    ಪೂರ್ಣಾಂಕ	nul;
}	ITMLST2;

ಸ್ಥಿರ TT_ಬಹುಲಕ	orgmode;
ಸ್ಥಿರ ಗಿಡ್ಡ	iochan;			// TTY I/O channel
ಸ್ಥಿರ ಗಿಡ್ಡ	iosb[4];		// IO status block

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vms_ಹೊಂದಿಕೆ_ಅಂಕೆ = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vms_ಹೊಂದಿಕೆ_free = 0;
ಸ್ಥಿರ char_u **vms_fmatch = ಖಾಲಿ;
ಸ್ಥಿರ ಅಕ್ಷರ *Fspec_Rms;		       // rms file spec, passed implicitly between routines



ಸ್ಥಿರ TT_ಬಹುಲಕ	get_tty(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	ಗಣ_tty(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು);

#ವ್ಯಾಖ್ಯಾನಿಸು EXPL_ALLOC_INC 64

#ವ್ಯಾಖ್ಯಾನಿಸು EQN(S1,S2,LN) (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(S1,S2,LN) == 0)
#ವ್ಯಾಖ್ಯಾನಿಸು SKIP_FOLLOWING_SLASHES(Str) ಮಾಡು { ಆಗ (Str[1] == '/') ++Str; } ಆಗ (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vul_desc(DESC *des, ಅಕ್ಷರ *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_ಸೂಚಕ = str;
    des->dsc$w_ಉದ್ದಳತೆ = str ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vul_ಅಂಶ(ಅಂಶ_3 *itm, ಗಿಡ್ಡ len, ಗಿಡ್ಡ cod, ಅಕ್ಷರ *adr, ಪೂರ್ಣಾಂಕ *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    ಶೂನ್ಯ
mch_settmode(tmode_T tmode)
{
    ಪೂರ್ಣಾಂಕ	ಸ್ಥಿತಿ_2;

    ಆದರೆ ( tmode == TMODE_RAW )
	ಗಣ_tty(0, 0);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆಯ್ಕೆ (orgmode.ಅಗಲ)
	{
	    ಸ್ಥಿತಿ 132:	ಹೊರಹೋಗು_STR_NF((char_u *)"\033[?3h\033>");	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 80:	ಹೊರಹೋಗು_STR_NF((char_u *)"\033[?3l\033>");	ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:	ನಿಲ್ಲಿಸು;
	}
	ಹೊರಹೋಗು_flush();
	ಸ್ಥಿತಿ_2 = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, ಗಾತ್ರ(TT_ಬಹುಲಕ), 0,0,0,0);
	ಆದರೆ (ಸ್ಥಿತಿ_2!=SS$ಸಾಮಾನ್ಯ_3 || (iosb[0]&0xFFFF)!=SS$ಸಾಮಾನ್ಯ_3)
	    ಹಿಂತಿರುಗಿ;
	(ಶೂನ್ಯ)sys$dassgn(iochan);
	iochan = 0;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_tty(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ		    ಸ್ಥಿತಿ_2;
    TT_ಬಹುಲಕ	    newmode;		// New TTY mode bits
    ಸ್ಥಿರ ಗಿಡ್ಡ    ಮೊದಲ_ಸಮಯ = TRUE;

    ಆದರೆ (ಮೊದಲ_ಸಮಯ)
    {
	orgmode = get_tty();
	ಮೊದಲ_ಸಮಯ = FALSE;
    }
    newmode = get_tty();
    ಆದರೆ (ಲಂಬಸಾಲು)
	newmode.ಅಗಲ		 = ಲಂಬಸಾಲು;
    ಆದರೆ (ಅಡ್ಡಸಾಲು)
	newmode.x.y.ಉದ್ದಳತೆ       = ಅಡ್ಡಸಾಲು;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    ಸ್ಥಿತಿ_2 = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, ಗಾತ್ರ(newmode), 0, 0, 0, 0);
    ಆದರೆ (ಸ್ಥಿತಿ_2!=SS$ಸಾಮಾನ್ಯ_3 || (iosb[0]&0xFFFF)!=SS$ಸಾಮಾನ್ಯ_3)
	ಹಿಂತಿರುಗಿ;
}

    ಸ್ಥಿರ TT_ಬಹುಲಕ
get_tty(ಶೂನ್ಯ)
{

    ಸ್ಥಿರ $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;
    TT_ಬಹುಲಕ	tt_ಬಹುಲಕ;

    ಆದರೆ (!iochan)
	ಸ್ಥಿತಿ_2 = sys$ನಿಯೋಜಿಸು(&odsc,&iochan,0,0);

    ಸ್ಥಿತಿ_2 = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_ಬಹುಲಕ, ಗಾತ್ರ(tt_ಬಹುಲಕ), 0, 0, 0, 0);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != SS$ಸಾಮಾನ್ಯ_3 || (iosb[0] & 0xFFFF) != SS$ಸಾಮಾನ್ಯ_3)
    {
	tt_ಬಹುಲಕ.ಅಗಲ		= 0;
	tt_ಬಹುಲಕ.type		= 0;
	tt_ಬಹುಲಕ.class		= 0;
	tt_ಬಹುಲಕ.x.basic		= 0;
	tt_ಬಹುಲಕ.x.y.ಉದ್ದಳತೆ	= 0;
	tt_ಬಹುಲಕ.extended	= 0;
    }
    ಹಿಂತಿರುಗಿ(tt_ಬಹುಲಕ);
}

/*
 * Get the current window size in Rows and Columns.
 */
    ಪೂರ್ಣಾಂಕ
mch_get_shellsize(ಶೂನ್ಯ)
{
    TT_ಬಹುಲಕ	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.ಅಗಲ;
    ಅಡ್ಡಸಾಲುಗಳು = tmode.x.y.ಉದ್ದಳತೆ;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    ಶೂನ್ಯ
mch_ಗಣ_shellsize(ಶೂನ್ಯ)
{
    ಗಣ_tty(ಅಡ್ಡಸಾಲುಗಳು, Columns);
    ಆಯ್ಕೆ (Columns)
    {
	ಸ್ಥಿತಿ 132:	ಹೊರಹೋಗು_STR_NF((char_u *)"\033[?3h\033>");	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 80:	ಹೊರಹೋಗು_STR_NF((char_u *)"\033[?3l\033>");	ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:	ನಿಲ್ಲಿಸು;
    }
    ಹೊರಹೋಗು_flush();
    screen_ಆರಂಭ();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    ಸ್ಥಿರ ಅಕ್ಷರ		ಬಫರ್[LNM$C_NAMLENGTH+1];
    char_u		*cp = ಖಾಲಿ;
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ	attrib;
    ಪೂರ್ಣಾಂಕ			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    ಅಕ್ಷರ		*sbuf = ಖಾಲಿ;

    vul_desc(&d_lognam, (ಅಕ್ಷರ *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_ಅಂಶ(&itmlst.ಸೂಚ್ಯಂಕ, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ), LNM$ಸೂಚ್ಯಂಕ_2, (ಅಕ್ಷರ *)&idx, &dum);
    vul_ಅಂಶ(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, ಬಫರ್, &lengte);
    itmlst.nul	= 0;
    ಆದರೆ (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, ಖಾಲಿ,&itmlst) == SS$ಸಾಮಾನ್ಯ_3)
    {
	ಬಫರ್[lengte] = '\0';
	ಆದರೆ (cp = alloc(lengte + 1))
	    strcpy((ಅಕ್ಷರ *)cp, ಬಫರ್);
	ಹಿಂತಿರುಗಿ(cp);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((sbuf = getenv((ಅಕ್ಷರ *)lognam)))
    {
	lengte = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sbuf) + 1;
	cp = alloc(lengte);
	ಆದರೆ (cp)
	    strcpy((ಅಕ್ಷರ *)cp, sbuf);
	ಹಿಂತಿರುಗಿ cp;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ(ಖಾಲಿ);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    ಪೂರ್ಣಾಂಕ
mch_setenv(ಅಕ್ಷರ *var, ಅಕ್ಷರ *ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ x)
{
    ಪೂರ್ಣಾಂಕ		res, dum;
    ದೀರ್ಘ	attrib = 0L;
    ಅಕ್ಷರ	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_ಅಂಶ(&itmlst.equ, ಮೌಲ್ಯ ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮೌಲ್ಯ) : 0, ಮೌಲ್ಯ ? LNM$_STRING : 0,
	    ಮೌಲ್ಯ, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    ಹಿಂತಿರುಗಿ((res == 1) ? 0 : -1);
}

    ಪೂರ್ಣಾಂಕ
vms_sys(ಅಕ್ಷರ *cmd, ಅಕ್ಷರ *ಹೊರಹೋಗು, ಅಕ್ಷರ *inp)
{
    DESC	cdsc, odsc, idsc;
    ದೀರ್ಘ	ಸ್ಥಿತಿ_2;

    ಆದರೆ (cmd)
	vul_desc(&cdsc, cmd);
    ಆದರೆ (ಹೊರಹೋಗು)
	vul_desc(&odsc, ಹೊರಹೋಗು);
    ಆದರೆ (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : ಖಾಲಿ,		// command string
	      inp ? &idsc : ಖಾಲಿ,		// input file
	      ಹೊರಹೋಗು ? &odsc : ಖಾಲಿ,		// output file
	      0, 0, 0, &ಸ್ಥಿತಿ_2, 0, 0, 0, 0, 0, 0);
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * Convert string to lowercase - most often filename
 */
    ಅಕ್ಷರ *
vms_tolower( ಅಕ್ಷರ *name )
{
    ಪೂರ್ಣಾಂಕ i,nlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);
    ಗಾಗಿ (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    ಹಿಂತಿರುಗಿ name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    ಪೂರ್ಣಾಂಕ
vms_sys_ಸ್ಥಿತಿ(ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (ಸ್ಥಿತಿ_2 != SS$ಸಾಮಾನ್ಯ_3 && (ಸ್ಥಿತಿ_2 & STS$M_SUCCESS) == 0)
	ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;		// Command failed.
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    ಪೂರ್ಣಾಂಕ
vms_read(ಅಕ್ಷರ *inbuf, size_t nbytes)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2, function, len;
    TT_ಬಹುಲಕ	tt_ಬಹುಲಕ;
    ಅಂಶ_3	itmlst[2];     // terminates on everything
    ಸ್ಥಿರ ದೀರ್ಘ trm_ಮುಖವಾಡ[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    ಆದರೆ (!iochan)
	tt_ಬಹುಲಕ = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_ಅಂಶ(&itmlst[0], 0, TRM$_MODIFIERS,
	 (ಅಕ್ಷರ *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_ಅಂಶ(&itmlst[1], ಗಾತ್ರ(trm_ಮುಖವಾಡ), TRM$ಪದ_2, (ಅಕ್ಷರ *)&trm_ಮುಖವಾಡ, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    ಸ್ಥಿತಿ_2 = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, ಗಾತ್ರ(itmlst));
    len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    ಸ್ಥಿತಿ_2 = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(inbuf); // return the total length

    ಹಿಂತಿರುಗಿ len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vms_wproc(ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ val)
{
    ಪೂರ್ಣಾಂಕ i;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vms_ಹೊಂದಿಕೆ_alloced = 0;

    ಆದರೆ (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	ಹಿಂತಿರುಗಿ 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    ಆದರೆ (vms_ಹೊಂದಿಕೆ_ಅಂಕೆ == 0)
    {
	// first time through, setup some things
	ಆದರೆ (ಖಾಲಿ == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    ಆದರೆ (!vms_fmatch)
		ಹಿಂತಿರುಗಿ 0;
	    vms_ಹೊಂದಿಕೆ_alloced = EXPL_ALLOC_INC;
	    vms_ಹೊಂದಿಕೆ_free = EXPL_ALLOC_INC;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // re-use existing space
	    vms_ಹೊಂದಿಕೆ_free = vms_ಹೊಂದಿಕೆ_alloced;
	}
    }

    // make matches look uniform
    vms_remove_ಆವೃತ್ತಿ(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    ಗಾಗಿ (i = 0; i<vms_ಹೊಂದಿಕೆ_ಅಂಕೆ; i++)
    {
	ಆದರೆ (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    ಹಿಂತಿರುಗಿ 1;
    }
    ಆದರೆ (--vms_ಹೊಂದಿಕೆ_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_ಹೊಂದಿಕೆ_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		ಗಾತ್ರ(ಅಕ್ಷರ **) * vms_ಹೊಂದಿಕೆ_alloced);
	ಆದರೆ (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    ಹಿಂತಿರುಗಿ 0;
	}
	vms_ಹೊಂದಿಕೆ_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_ಹೊಂದಿಕೆ_ಅಂಕೆ] = vim_strsave((char_u *)name);

    ++vms_ಹೊಂದಿಕೆ_ಅಂಕೆ;
    ಹಿಂತಿರುಗಿ 1;
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
    ಪೂರ್ಣಾಂಕ
mch_expand_wildcards(
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_pat,
    char_u	**pat,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_file,
    char_u	***file,
    ಪೂರ್ಣಾಂಕ		flags)
{
    ಪೂರ್ಣಾಂಕ		i, cnt = 0;
    char_u	buf[MAXPATHL];
    ಅಕ್ಷರ       *ಫಲಿತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		dir;
    ಪೂರ್ಣಾಂಕ files_alloced, files_free;

    *ಅಂಕೆ_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    ಆದರೆ (*file == ಖಾಲಿ)
    {
	*ಅಂಕೆ_file = 0;
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_pat; i++)
    {
	// expand environment var or home dir
	ಆದರೆ (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	ಇಲ್ಲದಿದ್ದರೆ
	    STRCPY(buf,pat[i]);

	vms_ಹೊಂದಿಕೆ_ಅಂಕೆ = 0; // reset collection counter
	ಫಲಿತಾಂಶ = decc$translate_vms(vms_fixfilename(buf));
	ಆದರೆ ( (ಪೂರ್ಣಾಂಕ) ಫಲಿತಾಂಶ == 0 || (ಪೂರ್ಣಾಂಕ) ಫಲಿತಾಂಶ == -1  )
	{
	    cnt = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cnt = decc$ಗಮ್ಯ_vms(ಫಲಿತಾಂಶ, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	ಆದರೆ (cnt > 0)
	    cnt = vms_ಹೊಂದಿಕೆ_ಅಂಕೆ;

	ಆದರೆ (cnt < 1)
	    ಮುಂದುವರಿ;

	ಗಾಗಿ (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    ಆದರೆ (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		ಮುಂದುವರಿ;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    ಆದರೆ (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		ಮುಂದುವರಿ;

	    // Skip files that are not executable if we check for that.
	    ಆದರೆ (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], ಖಾಲಿ, !(flags & EW_SHELLCMD)))
		ಮುಂದುವರಿ;

	    // allocate memory for pointers
	    ಆದರೆ (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, ಗಾತ್ರ(char_u **) * files_alloced);
		ಆದರೆ (*file == ಖಾಲಿ)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *ಅಂಕೆ_file = 0;
		    ಹಿಂತಿರುಗಿ(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*ಅಂಕೆ_file++] = vms_fmatch[i];
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಪೂರ್ಣಾಂಕ
mch_expandpath(garray_T *gap, char_u *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags)
{
    ಪೂರ್ಣಾಂಕ		i,cnt = 0;
    ಅಕ್ಷರ       *ಫಲಿತಾಂಶ;

    vms_ಹೊಂದಿಕೆ_ಅಂಕೆ = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    ಫಲಿತಾಂಶ = decc$translate_vms(vms_fixfilename(ಮಾರ್ಗ));
    ಆದರೆ ( (ಪೂರ್ಣಾಂಕ) ಫಲಿತಾಂಶ == 0 || (ಪೂರ್ಣಾಂಕ) ಫಲಿತಾಂಶ == -1  )
    {
	cnt = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	cnt = decc$ಗಮ್ಯ_vms(ಫಲಿತಾಂಶ, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    ಆದರೆ (cnt > 0)
	cnt = vms_ಹೊಂದಿಕೆ_ಅಂಕೆ;
    ಗಾಗಿ (i = 0; i < cnt; i++)
    {
	ಆದರೆ (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    ಹಿಂತಿರುಗಿ cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vms_unix_mixed_filespec(ಅಕ್ಷರ *ಒಳ, ಅಕ್ಷರ *ಹೊರಹೋಗು)
{
    ಅಕ್ಷರ *lastcolon;
    ಅಕ್ಷರ *ಕೊನೆ_ದ_dir;
    ಅಕ್ಷರ ch;
    ಪೂರ್ಣಾಂಕ len;
    ಅಕ್ಷರ *ಹೊರಹೋಗು_str=ಹೊರಹೋಗು;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(ಒಳ, ':');   // find last colon
    ಆದರೆ (lastcolon != ಖಾಲಿ)
    {
	len = lastcolon - ಒಳ + 1;
	strncpy(ಹೊರಹೋಗು, ಒಳ, len);
	ಹೊರಹೋಗು += len;
	ಒಳ += len;
    }

    ಕೊನೆ_ದ_dir = ಖಾಲಿ;	// default: no directory

    // start of directory portion
    ch = *ಒಳ;
    ಆದರೆ ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(ಒಳ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (EQN(ಒಳ, "../", 3))      // Unix parent directory?
    {
	*ಹೊರಹೋಗು++ = '[';
	*ಹೊರಹೋಗು++ = '-';
	ಕೊನೆ_ದ_dir = ಹೊರಹೋಗು;
	ch = '.';
	ಒಳ += 2;
	SKIP_FOLLOWING_SLASHES(ಒಳ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {                                // not a special character
	ಆಗ (EQN(ಒಳ, "./", 2))     // Ignore Unix "current dir"
	{
	    ಒಳ += 2;
	    SKIP_FOLLOWING_SLASHES(ಒಳ);
    }
    ಆದರೆ (strchr(ಒಳ, '/') == ಖಾಲಿ)     // any more Unix directories ?
    {
	strcpy(ಹೊರಹೋಗು, ಒಳ);             // No - get rest of the spec
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	*ಹೊರಹೋಗು++ = '[';                // Yes, denote a Vms subdirectory
	ch = '.';
	--ಒಳ;
	}
    }

    // if we get here, there is a directory part of the filename

    // initialize output file spec
    *ಹೊರಹೋಗು++ = ch;
    ++ಒಳ;

    ಆಗ (*ಒಳ != '\0')
    {
	ch = *ಒಳ;
	ಆದರೆ ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    ಕೊನೆ_ದ_dir = ಹೊರಹೋಗು;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(ಒಳ);
	    }
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (EQN(ಒಳ, "../", 3))     // Unix parent directory?
	{
	    *ಹೊರಹೋಗು++ = '-';
	    ಕೊನೆ_ದ_dir = ಹೊರಹೋಗು;
	    ch = '.';
	    ಒಳ += 2;
	    SKIP_FOLLOWING_SLASHES(ಒಳ);
	    }
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆಗ (EQN(ಒಳ, "./", 2))    // Ignore Unix "current dir"
	    {
		ಕೊನೆ_ದ_dir = ಹೊರಹೋಗು;
		ಒಳ += 2;
		SKIP_FOLLOWING_SLASHES(ಒಳ);
		ch = *ಒಳ;
	    }
	}

    // Place next character into output file spec
	*ಹೊರಹೋಗು++ = ch;
	++ಒಳ;
    }

    *ಹೊರಹೋಗು = '\0';            // Terminate output file spec

    ಆದರೆ (ಕೊನೆ_ದ_dir != ಖಾಲಿ) // Terminate directory portion
	*ಕೊನೆ_ದ_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vms_fspec_proc(ಅಕ್ಷರ *fil, ಪೂರ್ಣಾಂಕ val)
{
    strcpy(Fspec_Rms,fil);
    ಹಿಂತಿರುಗಿ(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    ಶೂನ್ಯ *
vms_fixfilename(ಶೂನ್ಯ *instring)
{
    ಸ್ಥಿರ ಅಕ್ಷರ		*buf = ಖಾಲಿ;
    ಸ್ಥಿರ size_t	buflen = 0;
    size_t		len;

    // get a big-enough buffer
    len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(instring) + 1;
    ಆದರೆ (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * ಗಾತ್ರ(ಅಕ್ಷರ));
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
    ಅಕ್ಷರ		 *tmpbuf = ಖಾಲಿ;
    tmpbuf = ALLOC_MULT(ಅಕ್ಷರ, buflen);
    strcpy(tmpbuf, instring);
#ಆದರೆ_ಅಂತ್ಯ

    Fspec_Rms = buf;				// for decc$to_vms

    ಆದರೆ (strchr(instring,'/') == ಖಾಲಿ)
	// It is already a VMS file spec
	strcpy(buf, instring);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strchr(instring,'"') == ಖಾಲಿ)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	ಆದರೆ ( (strchr(instring,'[') == ಖಾಲಿ) && (strchr(instring,'<') == ಖಾಲಿ) &&
	     (strchr(instring,']') == ಖಾಲಿ) && (strchr(instring,'>') == ಖಾಲಿ) &&
	     (strchr(instring,':') == ಖಾಲಿ) )
	{
	    // It must be a truly unix fspec
	    decc$ಗಮ್ಯ_vms(instring, vms_fspec_proc, 0, 0);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    ಹಿಂತಿರುಗಿ buf;
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
    ಶೂನ್ಯ
vms_remove_ಆವೃತ್ತಿ(ಶೂನ್ಯ * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ = 0;
    ಪೂರ್ಣಾಂಕ		vdigits = 0;

    rp = (char_u *)fname+ ಅಕ್ಷರಸರಣಿ_ಉದ್ದ( (ಅಕ್ಷರ *)fname)- 1;    // Rightmost char
    ಆಗ ((ಮುಗಿದ == 0) && (rp > (char_u *)fname))       // Count digits
    {
	ಆದರೆ (ಅಂಕಿಯೇ( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಮುಗಿದ = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    ಆದರೆ (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	ಆದರೆ (*rp == (char_u)';')
	{
	    ಆದರೆ ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    ಆದರೆ ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		ಮುಗಿದ = 0;
		ಆಗ ((ಮುಗಿದ == 0) && (dp >= (char_u *)fname))
		{
		    ಆದರೆ ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			ಆದರೆ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    ಮುಗಿದ = 1;   // No previous dot found in name
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*dp == '.')
		    {                   // Possible dot delimiter
			ಆದರೆ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dot delimiter
			    ಮುಗಿದ = 1;   // Previous dot found in name
			    *rp = '\0'; // Trim off ".nnn"
			}
		    }
		    dp--;               // Next char to right
		} // while
	    }
	}
	// Else no version found to remove
    } // if (vdigits  <= 5)

    ಹಿಂತಿರುಗಿ ;
}

ರಚನೆ typeahead_st {
    ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ numchars;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ  firstchar;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ  reserved0;
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    ಪೂರ್ಣಾಂಕ
RealWaitForChar(
    ಪೂರ್ಣಾಂಕ		fd ಬಳಸದ, // always read from iochan
    ದೀರ್ಘ	msec,
    ಪೂರ್ಣಾಂಕ		*check_for_gpm ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		*interrupted)
{
    ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2;
    ರಚನೆ _generic_64 ಸಮಯ_curr;
    ರಚನೆ _generic_64 ಸಮಯ_diff;
    ರಚನೆ _generic_64 ಸಮಯ_ಹೊರಹೋಗು;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ convert_operation = LIB$K_DELTA_SECONDS_F;
    ದಶಮಾಂಶ sec =(ದಶಮಾಂಶ) msec/1000;

    // make sure the iochan is set
    ಆದರೆ (!iochan)
	get_tty();

    ಆದರೆ (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	ಸ್ಥಿತಿ_2 = sys$gettim(&ಸಮಯ_curr);
	ಆದರೆ (ಸ್ಥಿತಿ_2 != SS$ಸಾಮಾನ್ಯ_3)
	    ಹಿಂತಿರುಗಿ 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#ಆದರೆ __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_ಗಮ್ಯ_ಆಂತರಿಕ_ಸಮಯ
#  define LIB_CVTX_ಗಮ್ಯ_ಆಂತರಿಕ_ಸಮಯ lib$cvts_ಗಮ್ಯ_ಆಂತರಿಕ_ಸಮಯ // IEEE
# ಇಲ್ಲದಿದ್ದರೆ
#  define LIB_CVTX_ಗಮ್ಯ_ಆಂತರಿಕ_ಸಮಯ lib$cvtf_ಗಮ್ಯ_ಆಂತರಿಕ_ಸಮಯ
# endif
#ಇಲ್ಲದಿದ್ದರೆ
# define LIB_CVTX_ಗಮ್ಯ_ಆಂತರಿಕ_ಸಮಯ lib$cvtf_ಗಮ್ಯ_ಆಂತರಿಕ_ಸಮಯ // VAX
#ಆದರೆ_ಅಂತ್ಯ // __IEEE_FLOAT CVTS

	ಸ್ಥಿತಿ_2 = LIB_CVTX_ಗಮ್ಯ_ಆಂತರಿಕ_ಸಮಯ(
		&convert_operation, &sec, &ಸಮಯ_diff);
	ಆದರೆ (ಸ್ಥಿತಿ_2 != LIB$ಸಾಮಾನ್ಯ_3)
	    ಹಿಂತಿರುಗಿ 0; // error
	// add them up
	ಸ್ಥಿತಿ_2 = lib$ಕೂಡಿಸು_times(
		&ಸಮಯ_curr,
		&ಸಮಯ_diff,
		&ಸಮಯ_ಹೊರಹೋಗು);
	ಆದರೆ (ಸ್ಥಿತಿ_2 != LIB$ಸಾಮಾನ್ಯ_3)
	    ಹಿಂತಿರುಗಿ 0; // error
    }

    ಆಗ (TRUE)
    {
	// select()
	ಸ್ಥಿತಿ_2 = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	ಆದರೆ (ಸ್ಥಿತಿ_2 != SS$ಸಾಮಾನ್ಯ_3 || (iosb[0] & 0xFFFF) != SS$ಸಾಮಾನ್ಯ_3)
	    ಹಿಂತಿರುಗಿ 0; // error

	ಆದರೆ (typeahead.numchars)
	    ಹಿಂತಿರುಗಿ 1; // ready to read

	// there's nothing to read; what now?
	ಆದರೆ (msec == 0)
	{
	    // immediate time-out; return impatiently
	    ಹಿಂತಿರುಗಿ 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    ಹಿಂತಿರುಗಿ 1; // fakeout to force a wait in vms_read()
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // time-out needs to be checked
	    ಸ್ಥಿತಿ_2 = sys$gettim(&ಸಮಯ_curr);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 != SS$ಸಾಮಾನ್ಯ_3)
		ಹಿಂತಿರುಗಿ 0; // error

	    ಸ್ಥಿತಿ_2 = lib$sub_times(
		    &ಸಮಯ_ಹೊರಹೋಗು,
		    &ಸಮಯ_curr,
		    &ಸಮಯ_diff);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 != LIB$ಸಾಮಾನ್ಯ_3)
		ಹಿಂತಿರುಗಿ 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#ಆದರೆ !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
   {
   ಅಕ್ಷರ *name;
   ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ;
   } decc_feat_t;

ಪೂರ್ಣಾಂಕ vms_init_ಮುಗಿದ = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (ಅಕ್ಷರ *)ಖಾಲಿ, 0 } };


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

ಸ್ಥಿರ ಶೂನ್ಯ
vms_init(ಶೂನ್ಯ)
{
    // Set the global flag to indicate that LIB$INITIALIZE worked

    vms_init_ಮುಗಿದ = 1;

    ಪೂರ್ಣಾಂಕ feat_ಸೂಚ್ಯಂಕ;
    ಪೂರ್ಣಾಂಕ feat_ಮೌಲ್ಯ;
    ಪೂರ್ಣಾಂಕ feat_ಮೌಲ್ಯ_max;
    ಪೂರ್ಣಾಂಕ feat_ಮೌಲ್ಯ_min;
    ಪೂರ್ಣಾಂಕ i;
    ಪೂರ್ಣಾಂಕ sts;

    // Loop through all items in the decc_feat_array[]
    ಗಾಗಿ (i = 0; decc_feat_array[i].name != ಖಾಲಿ; i++)
    {
	// Get the feature index
	feat_ಸೂಚ್ಯಂಕ = decc$feature_get_ಸೂಚ್ಯಂಕ(decc_feat_array[i].name);
	ಆದರೆ (feat_ಸೂಚ್ಯಂಕ >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_ಮೌಲ್ಯ = decc$feature_get_ಮೌಲ್ಯ(feat_ಸೂಚ್ಯಂಕ, 1);
	    feat_ಮೌಲ್ಯ_min = decc$feature_get_ಮೌಲ್ಯ(feat_ಸೂಚ್ಯಂಕ, 2);
	    feat_ಮೌಲ್ಯ_max = decc$feature_get_ಮೌಲ್ಯ(feat_ಸೂಚ್ಯಂಕ, 3);

	    ಆದರೆ ((decc_feat_array[i].ಮೌಲ್ಯ >= feat_ಮೌಲ್ಯ_min) && (decc_feat_array[i].ಮೌಲ್ಯ <= feat_ಮೌಲ್ಯ_max))
	    {
		// Valid value.  Set it if necessary
		ಆದರೆ (feat_ಮೌಲ್ಯ != decc_feat_array[i].ಮೌಲ್ಯ)
		    sts = decc$feature_ಗಣ_ಮೌಲ್ಯ(feat_ಸೂಚ್ಯಂಕ, 1, decc_feat_array[i].ಮೌಲ್ಯ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		// Invalid DECC feature value
		ಬರೆ("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_ಮೌಲ್ಯ, feat_ಮೌಲ್ಯ_min, decc_feat_array[i].name, feat_ಮೌಲ್ಯ_max);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // Invalid DECC feature name
	    ಬರೆ("UNKNOWN DECC FEATURE: %s.\n", decc_feat_array[i].name);
    }
}


/* Get "vms_init()" into a valid, loaded LIB$INITIALIZE PSECT. */

# pragma nostandard

/* Establish the LIB$INITIALIZE PSECTs, with proper alignment and
 * other attributes.  Note that "nopic" is significant only on VAX.
 */
# pragma extern_model save

# pragma extern_model strict_refdef "LIB$INITIALIZE" 2, nopic, nowrt
ಶೂನ್ಯ (*ಸ್ಥಿರಾಂಕ x_vms_init)() = vms_init;

# pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಹೆಚ್ಚುವರಿ[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
ಪೂರ್ಣಾಂಕ LIB$INITIALIZE(ಶೂನ್ಯ);
# pragma extern_model strict_refdef
ಪೂರ್ಣಾಂಕ dmy_lib$initialize = (ಪೂರ್ಣಾಂಕ) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#ಆದರೆ_ಅಂತ್ಯ // !defined( __VAX) && (__CRTL_VER >= 70301000)
