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

#ଅନ୍ତର୍ଭାଵ "vim.h"

// define _generic_64 for use in time functions
#ଯଦି !defined(VAX)
# include <gen64def.h>
#ଅନ୍ୟଥା
// based on Alpha's gen64def.h; the file is absent on VAX
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା gen64$l_longword [2];   // ...two 32-bit values, or
	unsigned short ପୂର୍ଣ୍ଣସଂଖ୍ୟା gen64$w_word [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ଅକ୍ଷର	class;
    ଅକ୍ଷର	type;
    short	width;
    ସଂଘ
    {
	ସଂରଚନା
	{
	    ଅକ୍ଷର	_basic[3];
	    ଅକ୍ଷର	length;
	}	y;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	basic;
    }	x;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extended;
}	TT_MODE;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    short	buflen;
    short	itemcode;
    ଅକ୍ଷର	*bufadrs;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*retlen;
}	ITEM;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ITEM	equ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nul;
}	ITMLST1;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ITEM	index;
    ITEM	string;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	nul;
}	ITMLST2;

ସ୍ଥିର TT_MODE	orgmode;
ସ୍ଥିର short	iochan;			// TTY I/O channel
ସ୍ଥିର short	iosb[4];		// IO status block

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vms_match_ଅଙ୍କ = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vms_match_free = 0;
ସ୍ଥିର char_u **vms_fmatch = ଖାଲି;
ସ୍ଥିର ଅକ୍ଷର *Fspec_Rms;		       // rms file spec, passed implicitly between routines



ସ୍ଥିର TT_MODE	get_tty(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ	set_tty(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col);

#define EXPL_ALLOC_INC 64

#define EQN(S1,S2,LN) (strncmp(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) କର { ଯେତେବେଳେ (Str[1] == '/') ++Str; } ଯେତେବେଳେ (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
vul_desc(DESC *des, ଅକ୍ଷର *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_pointer = str;
    des->dsc$w_length = str ? strlen(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    ସ୍ଥିର ଶୂନ୍ୟ
vul_item(ITEM *itm, short len, short cod, ଅକ୍ଷର *adr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    ଶୂନ୍ୟ
mch_settmode(tmode_T tmode)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	status;

    ଯଦି ( tmode == TMODE_RAW )
	set_tty(0, 0);
    ଅନ୍ୟଥା
    {
	ବାଛ (orgmode.width)
	{
	    ସ୍ଥିତି 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	ଭାଙ୍ଗ;
	    ସ୍ଥିତି 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	ଭାଙ୍ଗ;
	}
	out_flush();
	status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, ଆକାର(TT_MODE), 0,0,0,0);
	ଯଦି (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	    ଫେରନ୍ତୁ;
	(ଶୂନ୍ୟ)sys$dassgn(iochan);
	iochan = 0;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
set_tty(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    status;
    TT_MODE	    newmode;		// New TTY mode bits
    ସ୍ଥିର short    first_time = TRUE;

    ଯଦି (first_time)
    {
	orgmode = get_tty();
	first_time = FALSE;
    }
    newmode = get_tty();
    ଯଦି (col)
	newmode.width		 = col;
    ଯଦି (row)
	newmode.x.y.length       = row;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, ଆକାର(newmode), 0, 0, 0, 0);
    ଯଦି (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	ଫେରନ୍ତୁ;
}

    ସ୍ଥିର TT_MODE
get_tty(ଶୂନ୍ୟ)
{

    ସ୍ଥିର $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status;
    TT_MODE	tt_mode;

    ଯଦି (!iochan)
	status = sys$assign(&odsc,&iochan,0,0);

    status = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, ଆକାର(tt_mode), 0, 0, 0, 0);
    ଯଦି (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.length	= 0;
	tt_mode.extended	= 0;
    }
    ଫେରନ୍ତୁ(tt_mode);
}

/*
 * Get the current window size in Rows and Columns.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_get_shellsize(ଶୂନ୍ୟ)
{
    TT_MODE	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    ଫେରନ୍ତୁ OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    ଶୂନ୍ୟ
mch_set_shellsize(ଶୂନ୍ୟ)
{
    set_tty(Rows, Columns);
    ବାଛ (Columns)
    {
	ସ୍ଥିତି 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	ଭାଙ୍ଗ;
	ସ୍ଥିତି 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	ଭାଙ୍ଗ;
    }
    out_flush();
    screen_start();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    ସ୍ଥିର ଅକ୍ଷର		buffer[LNM$C_NAMLENGTH+1];
    char_u		*cp = ଖାଲି;
    unsigned long	attrib;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    ଅକ୍ଷର		*sbuf = ଖାଲି;

    vul_desc(&d_lognam, (ଅକ୍ଷର *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.index, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା), LNM$_INDEX, (ଅକ୍ଷର *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, buffer, &lengte);
    itmlst.nul	= 0;
    ଯଦି (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, ଖାଲି,&itmlst) == SS$_NORMAL)
    {
	buffer[lengte] = '\0';
	ଯଦି (cp = alloc(lengte + 1))
	    strcpy((ଅକ୍ଷର *)cp, buffer);
	ଫେରନ୍ତୁ(cp);
    }
    ଅନ୍ୟଥା ଯଦି ((sbuf = getenv((ଅକ୍ଷର *)lognam)))
    {
	lengte = strlen(sbuf) + 1;
	cp = alloc(lengte);
	ଯଦି (cp)
	    strcpy((ଅକ୍ଷର *)cp, sbuf);
	ଫେରନ୍ତୁ cp;
    }
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ(ଖାଲି);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_setenv(ଅକ୍ଷର *var, ଅକ୍ଷର *ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res, dum;
    long	attrib = 0L;
    ଅକ୍ଷର	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_item(&itmlst.equ, ମୂଲ୍ୟ ? strlen(ମୂଲ୍ୟ) : 0, ମୂଲ୍ୟ ? LNM$_STRING : 0,
	    ମୂଲ୍ୟ, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    ଫେରନ୍ତୁ((res == 1) ? 0 : -1);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vms_sys(ଅକ୍ଷର *cmd, ଅକ୍ଷର *out, ଅକ୍ଷର *inp)
{
    DESC	cdsc, odsc, idsc;
    long	status;

    ଯଦି (cmd)
	vul_desc(&cdsc, cmd);
    ଯଦି (out)
	vul_desc(&odsc, out);
    ଯଦି (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : ଖାଲି,		// command string
	      inp ? &idsc : ଖାଲି,		// input file
	      out ? &odsc : ଖାଲି,		// output file
	      0, 0, 0, &status, 0, 0, 0, 0, 0, 0);
    ଫେରନ୍ତୁ status;
}

/*
 * Convert string to lowercase - most often filename
 */
    ଅକ୍ଷର *
vms_tolower( ଅକ୍ଷର *name )
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i,nlen = strlen(name);
    ପାଇଁ (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    ଫେରନ୍ତୁ name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vms_sys_status(ପୂର୍ଣ୍ଣସଂଖ୍ୟା status)
{
    ଯଦି (status != SS$_NORMAL && (status & STS$M_SUCCESS) == 0)
	ଫେରନ୍ତୁ status;		// Command failed.
    ଫେରନ୍ତୁ 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vms_read(ଅକ୍ଷର *inbuf, size_t nbytes)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status, function, len;
    TT_MODE	tt_mode;
    ITEM	itmlst[2];     // terminates on everything
    ସ୍ଥିର long trm_mask[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    ଯଦି (!iochan)
	tt_mode = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_item(&itmlst[0], 0, TRM$_MODIFIERS,
	 (ଅକ୍ଷର *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_item(&itmlst[1], ଆକାର(trm_mask), TRM$_TERM, (ଅକ୍ଷର *)&trm_mask, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, ଆକାର(itmlst));
    len = strlen(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = strlen(inbuf); // return the total length

    ଫେରନ୍ତୁ len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vms_wproc(ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vms_match_alloced = 0;

    ଯଦି (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	ଫେରନ୍ତୁ 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    ଯଦି (vms_match_ଅଙ୍କ == 0)
    {
	// first time through, setup some things
	ଯଦି (ଖାଲି == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    ଯଦି (!vms_fmatch)
		ଫେରନ୍ତୁ 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	ଅନ୍ୟଥା
	{
	    // re-use existing space
	    vms_match_free = vms_match_alloced;
	}
    }

    // make matches look uniform
    vms_remove_version(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    ପାଇଁ (i = 0; i<vms_match_ଅଙ୍କ; i++)
    {
	ଯଦି (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    ଫେରନ୍ତୁ 1;
    }
    ଯଦି (--vms_match_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		ଆକାର(ଅକ୍ଷର **) * vms_match_alloced);
	ଯଦି (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    ଫେରନ୍ତୁ 0;
	}
	vms_match_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_match_ଅଙ୍କ] = vim_strsave((char_u *)name);

    ++vms_match_ଅଙ୍କ;
    ଫେରନ୍ତୁ 1;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_expand_wildcards(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_pat,
    char_u	**pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_file,
    char_u	***file,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, cnt = 0;
    char_u	buf[MAXPATHL];
    ଅକ୍ଷର       *result;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା files_alloced, files_free;

    *ଅଙ୍କ_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    ଯଦି (*file == ଖାଲି)
    {
	*ଅଙ୍କ_file = 0;
	ଫେରନ୍ତୁ FAIL;
    }
    ପାଇଁ (i = 0; i < ଅଙ୍କ_pat; i++)
    {
	// expand environment var or home dir
	ଯଦି (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	ଅନ୍ୟଥା
	    STRCPY(buf,pat[i]);

	vms_match_ଅଙ୍କ = 0; // reset collection counter
	result = decc$translate_vms(vms_fixfilename(buf));
	ଯଦି ( (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) result == 0 || (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) result == -1  )
	{
	    cnt = 0;
	}
	ଅନ୍ୟଥା
	{
	    cnt = decc$to_vms(result, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	ଯଦି (cnt > 0)
	    cnt = vms_match_ଅଙ୍କ;

	ଯଦି (cnt < 1)
	    ଜାରି;

	ପାଇଁ (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    ଯଦି (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		ଜାରି;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    ଯଦି (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		ଜାରି;

	    // Skip files that are not executable if we check for that.
	    ଯଦି (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], ଖାଲି, !(flags & EW_SHELLCMD)))
		ଜାରି;

	    // allocate memory for pointers
	    ଯଦି (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, ଆକାର(char_u **) * files_alloced);
		ଯଦି (*file == ଖାଲି)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *ଅଙ୍କ_file = 0;
		    ଫେରନ୍ତୁ(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*ଅଙ୍କ_file++] = vms_fmatch[i];
	}
    }
    ଫେରନ୍ତୁ OK;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_expandpath(garray_T *gap, char_u *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i,cnt = 0;
    ଅକ୍ଷର       *result;

    vms_match_ଅଙ୍କ = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    result = decc$translate_vms(vms_fixfilename(path));
    ଯଦି ( (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) result == 0 || (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) result == -1  )
    {
	cnt = 0;
    }
    ଅନ୍ୟଥା
    {
	cnt = decc$to_vms(result, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    ଯଦି (cnt > 0)
	cnt = vms_match_ଅଙ୍କ;
    ପାଇଁ (i = 0; i < cnt; i++)
    {
	ଯଦି (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    ଫେରନ୍ତୁ cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    ସ୍ଥିର ଶୂନ୍ୟ
vms_unix_mixed_filespec(ଅକ୍ଷର *in, ଅକ୍ଷର *out)
{
    ଅକ୍ଷର *lastcolon;
    ଅକ୍ଷର *end_of_dir;
    ଅକ୍ଷର ch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
    ଅକ୍ଷର *out_str=out;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(in, ':');   // find last colon
    ଯଦି (lastcolon != ଖାଲି)
    {
	len = lastcolon - in + 1;
	strncpy(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = ଖାଲି;	// default: no directory

    // start of directory portion
    ch = *in;
    ଯଦି ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    }
    ଅନ୍ୟଥା ଯଦି (EQN(in, "../", 3))      // Unix parent directory?
    {
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    }
    ଅନ୍ୟଥା
    {                                // not a special character
	ଯେତେବେଳେ (EQN(in, "./", 2))     // Ignore Unix "current dir"
	{
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    ଯଦି (strchr(in, '/') == ଖାଲି)     // any more Unix directories ?
    {
	strcpy(out, in);             // No - get rest of the spec
	ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା
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

    ଯେତେବେଳେ (*in != '\0')
    {
	ch = *in;
	ଯଦି ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	ଅନ୍ୟଥା ଯଦି (EQN(in, "../", 3))     // Unix parent directory?
	{
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	ଅନ୍ୟଥା
	{
	    ଯେତେବେଳେ (EQN(in, "./", 2))    // Ignore Unix "current dir"
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

    ଯଦି (end_of_dir != ଖାଲି) // Terminate directory portion
	*end_of_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vms_fspec_proc(ଅକ୍ଷର *fil, ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
    strcpy(Fspec_Rms,fil);
    ଫେରନ୍ତୁ(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    ଶୂନ୍ୟ *
vms_fixfilename(ଶୂନ୍ୟ *instring)
{
    ସ୍ଥିର ଅକ୍ଷର		*buf = ଖାଲି;
    ସ୍ଥିର size_t	buflen = 0;
    size_t		len;

    // get a big-enough buffer
    len = strlen(instring) + 1;
    ଯଦି (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * ଆକାର(ଅକ୍ଷର));
    }

#ifdef DEBUG
    ଅକ୍ଷର		 *tmpbuf = ଖାଲି;
    tmpbuf = ALLOC_MULT(ଅକ୍ଷର, buflen);
    strcpy(tmpbuf, instring);
#endif

    Fspec_Rms = buf;				// for decc$to_vms

    ଯଦି (strchr(instring,'/') == ଖାଲି)
	// It is already a VMS file spec
	strcpy(buf, instring);
    ଅନ୍ୟଥା ଯଦି (strchr(instring,'"') == ଖାଲି)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	ଯଦି ( (strchr(instring,'[') == ଖାଲି) && (strchr(instring,'<') == ଖାଲି) &&
	     (strchr(instring,']') == ଖାଲି) && (strchr(instring,'>') == ଖାଲି) &&
	     (strchr(instring,':') == ଖାଲି) )
	{
	    // It must be a truly unix fspec
	    decc$to_vms(instring, vms_fspec_proc, 0, 0);
	}
	ଅନ୍ୟଥା
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    ଅନ୍ୟଥା
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    ଫେରନ୍ତୁ buf;
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
    ଶୂନ୍ୟ
vms_remove_version(ଶୂନ୍ୟ * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		done = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vdigits = 0;

    rp = (char_u *)fname+ strlen( (ଅକ୍ଷର *)fname)- 1;    // Rightmost char
    ଯେତେବେଳେ ((done == 0) && (rp > (char_u *)fname))       // Count digits
    {
	ଯଦି (isdigit( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	ଅନ୍ୟଥା
	{
	    done = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    ଯଦି (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	ଯଦି (*rp == (char_u)';')
	{
	    ଯଦି ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	ଅନ୍ୟଥା ଯଦି (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    ଯଦି ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		done = 0;
		ଯେତେବେଳେ ((done == 0) && (dp >= (char_u *)fname))
		{
		    ଯଦି ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			ଯଦି ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    done = 1;   // No previous dot found in name
			}
		    }
		    ଅନ୍ୟଥା ଯଦି (*dp == '.')
		    {                   // Possible dot delimiter
			ଯଦି ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
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

    ଫେରନ୍ତୁ ;
}

ସଂରଚନା typeahead_st {
    unsigned short numchars;
    unsigned ଅକ୍ଷର  firstchar;
    unsigned ଅକ୍ଷର  reserved0;
    unsigned long  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
RealWaitForChar(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fd UNUSED, // always read from iochan
    long	msec,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*check_for_gpm UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*interrupted)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା status;
    ସଂରଚନା _generic_64 time_curr;
    ସଂରଚନା _generic_64 time_diff;
    ସଂରଚନା _generic_64 time_out;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା convert_operation = LIB$K_DELTA_SECONDS_F;
    ଦଶମିକ sec =(ଦଶମିକ) msec/1000;

    // make sure the iochan is set
    ଯଦି (!iochan)
	get_tty();

    ଯଦି (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	status = sys$gettim(&time_curr);
	ଯଦି (status != SS$_NORMAL)
	    ଫେରନ୍ତୁ 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#ଯଦି __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_to_internal_time
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvts_to_internal_time // IEEE
# ଅନ୍ୟଥା
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time
# endif
#ଅନ୍ୟଥା
# define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time // VAX
#endif // __IEEE_FLOAT CVTS

	status = LIB_CVTX_TO_INTERNAL_TIME(
		&convert_operation, &sec, &time_diff);
	ଯଦି (status != LIB$_NORMAL)
	    ଫେରନ୍ତୁ 0; // error
	// add them up
	status = lib$add_times(
		&time_curr,
		&time_diff,
		&time_out);
	ଯଦି (status != LIB$_NORMAL)
	    ଫେରନ୍ତୁ 0; // error
    }

    ଯେତେବେଳେ (TRUE)
    {
	// select()
	status = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	ଯଦି (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
	    ଫେରନ୍ତୁ 0; // error

	ଯଦି (typeahead.numchars)
	    ଫେରନ୍ତୁ 1; // ready to read

	// there's nothing to read; what now?
	ଯଦି (msec == 0)
	{
	    // immediate time-out; return impatiently
	    ଫେରନ୍ତୁ 0;
	}
	ଅନ୍ୟଥା ଯଦି (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    ଫେରନ୍ତୁ 1; // fakeout to force a wait in vms_read()
	}
	ଅନ୍ୟଥା
	{
	    // time-out needs to be checked
	    status = sys$gettim(&time_curr);
	    ଯଦି (status != SS$_NORMAL)
		ଫେରନ୍ତୁ 0; // error

	    status = lib$sub_times(
		    &time_out,
		    &time_curr,
		    &time_diff);
	    ଯଦି (status != LIB$_NORMAL)
		ଫେରନ୍ତୁ 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#ଯଦି !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
   {
   ଅକ୍ଷର *name;
   ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ;
   } decc_feat_t;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vms_init_done = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (ଅକ୍ଷର *)ଖାଲି, 0 } };


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

ସ୍ଥିର ଶୂନ୍ୟ
vms_init(ଶୂନ୍ୟ)
{
    // Set the global flag to indicate that LIB$INITIALIZE worked

    vms_init_done = 1;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା feat_index;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା feat_ମୂଲ୍ୟ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା feat_ମୂଲ୍ୟ_max;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା feat_ମୂଲ୍ୟ_min;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sts;

    // Loop through all items in the decc_feat_array[]
    ପାଇଁ (i = 0; decc_feat_array[i].name != ଖାଲି; i++)
    {
	// Get the feature index
	feat_index = decc$feature_get_index(decc_feat_array[i].name);
	ଯଦି (feat_index >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_ମୂଲ୍ୟ = decc$feature_get_ମୂଲ୍ୟ(feat_index, 1);
	    feat_ମୂଲ୍ୟ_min = decc$feature_get_ମୂଲ୍ୟ(feat_index, 2);
	    feat_ମୂଲ୍ୟ_max = decc$feature_get_ମୂଲ୍ୟ(feat_index, 3);

	    ଯଦି ((decc_feat_array[i].ମୂଲ୍ୟ >= feat_ମୂଲ୍ୟ_min) && (decc_feat_array[i].ମୂଲ୍ୟ <= feat_ମୂଲ୍ୟ_max))
	    {
		// Valid value.  Set it if necessary
		ଯଦି (feat_ମୂଲ୍ୟ != decc_feat_array[i].ମୂଲ୍ୟ)
		    sts = decc$feature_set_ମୂଲ୍ୟ(feat_index, 1, decc_feat_array[i].ମୂଲ୍ୟ);
	    }
	    ଅନ୍ୟଥା
		// Invalid DECC feature value
		printf("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_ମୂଲ୍ୟ, feat_ମୂଲ୍ୟ_min, decc_feat_array[i].name, feat_ମୂଲ୍ୟ_max);
	}
	ଅନ୍ୟଥା
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
ଶୂନ୍ୟ (*ଅଚଳ x_vms_init)() = vms_init;

# pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା spare[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
ପୂର୍ଣ୍ଣସଂଖ୍ୟା LIB$INITIALIZE(ଶୂନ୍ୟ);
# pragma extern_model strict_refdef
ପୂର୍ଣ୍ଣସଂଖ୍ୟା dmy_lib$initialize = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#endif // !defined( __VAX) && (__CRTL_VER >= 70301000)
