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
#ورنہ
// based on Alpha's gen64def.h; the file is absent on VAX
قسمتعریف ساخت _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	غیرعلامتی صحیح gen64$l_longword [2];   // ...two 32-bit values, or
	غیرعلامتی مختصر صحیح gen64$w_لفظ [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#اگر_ختم

قسمتعریف ساخت
{
    حرف	class;
    حرف	type;
    مختصر	چوڑائی;
    اتحاد
    {
	ساخت
	{
	    حرف	_basic[3];
	    حرف	لمبائی;
	}	y;
	صحیح	basic;
    }	x;
    صحیح		extended;
}	TT_منوال;

قسمتعریف ساخت
{
    مختصر	buflen;
    مختصر	itemcode;
    حرف	*bufadrs;
    صحیح		*retlen;
}	شے_3;

قسمتعریف ساخت
{
    شے_3	equ;
    صحیح		nul;
}	ITMLST1;

قسمتعریف ساخت
{
    شے_3	اشاریہ;
    شے_3	string;
    صحیح	nul;
}	ITMLST2;

ساکن TT_منوال	orgmode;
ساکن مختصر	iochan;			// TTY I/O channel
ساکن مختصر	iosb[4];		// IO status block

ساکن صحیح vms_مطابقت_عدد = 0;
ساکن صحیح vms_مطابقت_free = 0;
ساکن char_u **vms_fmatch = تہی;
ساکن حرف *Fspec_Rms;		       // rms file spec, passed implicitly between routines



ساکن TT_منوال	get_tty(خالی);
ساکن خالی	سیٹ_tty(صحیح صف, صحیح ستون);

#تعریف_کرو EXPL_ALLOC_INC 64

#تعریف_کرو EQN(S1,S2,LN) (لڑی_موازنہ(S1,S2,LN) == 0)
#تعریف_کرو SKIP_FOLLOWING_SLASHES(Str) کرو { جب (Str[1] == '/') ++Str; } جب (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    ساکن خالی
vul_desc(DESC *des, حرف *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_اشارہ_گر = str;
    des->dsc$w_لمبائی = str ? لڑی_لمبائی(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    ساکن خالی
vul_شے(شے_3 *itm, مختصر len, مختصر cod, حرف *adr, صحیح *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    خالی
mch_settmode(tmode_T tmode)
{
    صحیح	حالت_2;

    اگر ( tmode == TMODE_RAW )
	سیٹ_tty(0, 0);
    ورنہ
    {
	انتخاب (orgmode.چوڑائی)
	{
	    حالت 132:	برآمد_STR_NF((char_u *)"\033[?3h\033>");	توڑو;
	    حالت 80:	برآمد_STR_NF((char_u *)"\033[?3l\033>");	توڑو;
	    طےشدہ:	توڑو;
	}
	برآمد_flush();
	حالت_2 = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, حجم(TT_منوال), 0,0,0,0);
	اگر (حالت_2!=SS$معمول_3 || (iosb[0]&0xFFFF)!=SS$معمول_3)
	    واپس;
	(خالی)sys$dassgn(iochan);
	iochan = 0;
    }
}

    ساکن خالی
سیٹ_tty(صحیح صف, صحیح ستون)
{
    صحیح		    حالت_2;
    TT_منوال	    newmode;		// New TTY mode bits
    ساکن مختصر    پہلا_وقت = TRUE;

    اگر (پہلا_وقت)
    {
	orgmode = get_tty();
	پہلا_وقت = FALSE;
    }
    newmode = get_tty();
    اگر (ستون)
	newmode.چوڑائی		 = ستون;
    اگر (صف)
	newmode.x.y.لمبائی       = صف;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    حالت_2 = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, حجم(newmode), 0, 0, 0, 0);
    اگر (حالت_2!=SS$معمول_3 || (iosb[0]&0xFFFF)!=SS$معمول_3)
	واپس;
}

    ساکن TT_منوال
get_tty(خالی)
{

    ساکن $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    صحیح		حالت_2;
    TT_منوال	tt_منوال;

    اگر (!iochan)
	حالت_2 = sys$تفویض(&odsc,&iochan,0,0);

    حالت_2 = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_منوال, حجم(tt_منوال), 0, 0, 0, 0);
    اگر (حالت_2 != SS$معمول_3 || (iosb[0] & 0xFFFF) != SS$معمول_3)
    {
	tt_منوال.چوڑائی		= 0;
	tt_منوال.type		= 0;
	tt_منوال.class		= 0;
	tt_منوال.x.basic		= 0;
	tt_منوال.x.y.لمبائی	= 0;
	tt_منوال.extended	= 0;
    }
    واپس(tt_منوال);
}

/*
 * Get the current window size in Rows and Columns.
 */
    صحیح
mch_get_shellsize(خالی)
{
    TT_منوال	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.چوڑائی;
    صفیں = tmode.x.y.لمبائی;
    واپس OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    خالی
mch_سیٹ_shellsize(خالی)
{
    سیٹ_tty(صفیں, Columns);
    انتخاب (Columns)
    {
	حالت 132:	برآمد_STR_NF((char_u *)"\033[?3h\033>");	توڑو;
	حالت 80:	برآمد_STR_NF((char_u *)"\033[?3l\033>");	توڑو;
	طےشدہ:	توڑو;
    }
    برآمد_flush();
    screen_آغاز();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    ساکن حرف		بفر[LNM$C_NAMLENGTH+1];
    char_u		*cp = تہی;
    غیرعلامتی طویل	attrib;
    صحیح			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    حرف		*sbuf = تہی;

    vul_desc(&d_lognam, (حرف *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_شے(&itmlst.اشاریہ, حجم(صحیح), LNM$اشاریہ_2, (حرف *)&idx, &dum);
    vul_شے(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, بفر, &lengte);
    itmlst.nul	= 0;
    اگر (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, تہی,&itmlst) == SS$معمول_3)
    {
	بفر[lengte] = '\0';
	اگر (cp = alloc(lengte + 1))
	    strcpy((حرف *)cp, بفر);
	واپس(cp);
    }
    ورنہ اگر ((sbuf = getenv((حرف *)lognam)))
    {
	lengte = لڑی_لمبائی(sbuf) + 1;
	cp = alloc(lengte);
	اگر (cp)
	    strcpy((حرف *)cp, sbuf);
	واپس cp;
    }
    ورنہ
	واپس(تہی);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    صحیح
mch_setenv(حرف *var, حرف *قدر, صحیح x)
{
    صحیح		res, dum;
    طویل	attrib = 0L;
    حرف	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_شے(&itmlst.equ, قدر ? لڑی_لمبائی(قدر) : 0, قدر ? LNM$_STRING : 0,
	    قدر, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    واپس((res == 1) ? 0 : -1);
}

    صحیح
vms_sys(حرف *cmd, حرف *برآمد, حرف *inp)
{
    DESC	cdsc, odsc, idsc;
    طویل	حالت_2;

    اگر (cmd)
	vul_desc(&cdsc, cmd);
    اگر (برآمد)
	vul_desc(&odsc, برآمد);
    اگر (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : تہی,		// command string
	      inp ? &idsc : تہی,		// input file
	      برآمد ? &odsc : تہی,		// output file
	      0, 0, 0, &حالت_2, 0, 0, 0, 0, 0, 0);
    واپس حالت_2;
}

/*
 * Convert string to lowercase - most often filename
 */
    حرف *
vms_tolower( حرف *name )
{
    صحیح i,nlen = لڑی_لمبائی(name);
    برائے (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    واپس name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    صحیح
vms_sys_حالت(صحیح حالت_2)
{
    اگر (حالت_2 != SS$معمول_3 && (حالت_2 & STS$M_SUCCESS) == 0)
	واپس حالت_2;		// Command failed.
    واپس 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    صحیح
vms_read(حرف *inbuf, size_t nbytes)
{
    صحیح		حالت_2, function, len;
    TT_منوال	tt_منوال;
    شے_3	itmlst[2];     // terminates on everything
    ساکن طویل trm_نقاب[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    اگر (!iochan)
	tt_منوال = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_شے(&itmlst[0], 0, TRM$_MODIFIERS,
	 (حرف *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_شے(&itmlst[1], حجم(trm_نقاب), TRM$رکن_2, (حرف *)&trm_نقاب, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    حالت_2 = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, حجم(itmlst));
    len = لڑی_لمبائی(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    حالت_2 = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = لڑی_لمبائی(inbuf); // return the total length

    واپس len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    ساکن صحیح
vms_wproc(حرف *name, صحیح val)
{
    صحیح i;
    ساکن صحیح vms_مطابقت_alloced = 0;

    اگر (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	واپس 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    اگر (vms_مطابقت_عدد == 0)
    {
	// first time through, setup some things
	اگر (تہی == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    اگر (!vms_fmatch)
		واپس 0;
	    vms_مطابقت_alloced = EXPL_ALLOC_INC;
	    vms_مطابقت_free = EXPL_ALLOC_INC;
	}
	ورنہ
	{
	    // re-use existing space
	    vms_مطابقت_free = vms_مطابقت_alloced;
	}
    }

    // make matches look uniform
    vms_remove_نسخہ(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    برائے (i = 0; i<vms_مطابقت_عدد; i++)
    {
	اگر (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    واپس 1;
    }
    اگر (--vms_مطابقت_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_مطابقت_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		حجم(حرف **) * vms_مطابقت_alloced);
	اگر (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    واپس 0;
	}
	vms_مطابقت_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_مطابقت_عدد] = vim_strsave((char_u *)name);

    ++vms_مطابقت_عدد;
    واپس 1;
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
    صحیح		عدد_pat,
    char_u	**pat,
    صحیح		*عدد_file,
    char_u	***file,
    صحیح		flags)
{
    صحیح		i, cnt = 0;
    char_u	buf[MAXPATHL];
    حرف       *نتیجہ;
    صحیح		dir;
    صحیح files_alloced, files_free;

    *عدد_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    اگر (*file == تہی)
    {
	*عدد_file = 0;
	واپس FAIL;
    }
    برائے (i = 0; i < عدد_pat; i++)
    {
	// expand environment var or home dir
	اگر (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	ورنہ
	    STRCPY(buf,pat[i]);

	vms_مطابقت_عدد = 0; // reset collection counter
	نتیجہ = decc$translate_vms(vms_fixfilename(buf));
	اگر ( (صحیح) نتیجہ == 0 || (صحیح) نتیجہ == -1  )
	{
	    cnt = 0;
	}
	ورنہ
	{
	    cnt = decc$مقصد_vms(نتیجہ, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	اگر (cnt > 0)
	    cnt = vms_مطابقت_عدد;

	اگر (cnt < 1)
	    جاری;

	برائے (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    اگر (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		جاری;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    اگر (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		جاری;

	    // Skip files that are not executable if we check for that.
	    اگر (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], تہی, !(flags & EW_SHELLCMD)))
		جاری;

	    // allocate memory for pointers
	    اگر (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, حجم(char_u **) * files_alloced);
		اگر (*file == تہی)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *عدد_file = 0;
		    واپس(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*عدد_file++] = vms_fmatch[i];
	}
    }
    واپس OK;
}

    صحیح
mch_expandpath(garray_T *gap, char_u *راستہ, صحیح flags)
{
    صحیح		i,cnt = 0;
    حرف       *نتیجہ;

    vms_مطابقت_عدد = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    نتیجہ = decc$translate_vms(vms_fixfilename(راستہ));
    اگر ( (صحیح) نتیجہ == 0 || (صحیح) نتیجہ == -1  )
    {
	cnt = 0;
    }
    ورنہ
    {
	cnt = decc$مقصد_vms(نتیجہ, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    اگر (cnt > 0)
	cnt = vms_مطابقت_عدد;
    برائے (i = 0; i < cnt; i++)
    {
	اگر (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    واپس cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    ساکن خالی
vms_unix_mixed_filespec(حرف *اندر, حرف *برآمد)
{
    حرف *lastcolon;
    حرف *انتہا_کا_dir;
    حرف ch;
    صحیح len;
    حرف *برآمد_str=برآمد;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(اندر, ':');   // find last colon
    اگر (lastcolon != تہی)
    {
	len = lastcolon - اندر + 1;
	strncpy(برآمد, اندر, len);
	برآمد += len;
	اندر += len;
    }

    انتہا_کا_dir = تہی;	// default: no directory

    // start of directory portion
    ch = *اندر;
    اگر ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(اندر);
    }
    ورنہ اگر (EQN(اندر, "../", 3))      // Unix parent directory?
    {
	*برآمد++ = '[';
	*برآمد++ = '-';
	انتہا_کا_dir = برآمد;
	ch = '.';
	اندر += 2;
	SKIP_FOLLOWING_SLASHES(اندر);
    }
    ورنہ
    {                                // not a special character
	جب (EQN(اندر, "./", 2))     // Ignore Unix "current dir"
	{
	    اندر += 2;
	    SKIP_FOLLOWING_SLASHES(اندر);
    }
    اگر (strchr(اندر, '/') == تہی)     // any more Unix directories ?
    {
	strcpy(برآمد, اندر);             // No - get rest of the spec
	واپس;
    }
    ورنہ
    {
	*برآمد++ = '[';                // Yes, denote a Vms subdirectory
	ch = '.';
	--اندر;
	}
    }

    // if we get here, there is a directory part of the filename

    // initialize output file spec
    *برآمد++ = ch;
    ++اندر;

    جب (*اندر != '\0')
    {
	ch = *اندر;
	اگر ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    انتہا_کا_dir = برآمد;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(اندر);
	    }
	ورنہ اگر (EQN(اندر, "../", 3))     // Unix parent directory?
	{
	    *برآمد++ = '-';
	    انتہا_کا_dir = برآمد;
	    ch = '.';
	    اندر += 2;
	    SKIP_FOLLOWING_SLASHES(اندر);
	    }
	ورنہ
	{
	    جب (EQN(اندر, "./", 2))    // Ignore Unix "current dir"
	    {
		انتہا_کا_dir = برآمد;
		اندر += 2;
		SKIP_FOLLOWING_SLASHES(اندر);
		ch = *اندر;
	    }
	}

    // Place next character into output file spec
	*برآمد++ = ch;
	++اندر;
    }

    *برآمد = '\0';            // Terminate output file spec

    اگر (انتہا_کا_dir != تہی) // Terminate directory portion
	*انتہا_کا_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    ساکن صحیح
vms_fspec_proc(حرف *fil, صحیح val)
{
    strcpy(Fspec_Rms,fil);
    واپس(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    خالی *
vms_fixfilename(خالی *instring)
{
    ساکن حرف		*buf = تہی;
    ساکن size_t	buflen = 0;
    size_t		len;

    // get a big-enough buffer
    len = لڑی_لمبائی(instring) + 1;
    اگر (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * حجم(حرف));
    }

#اگر_تعریف_شدہ DEBUG
    حرف		 *tmpbuf = تہی;
    tmpbuf = ALLOC_MULT(حرف, buflen);
    strcpy(tmpbuf, instring);
#اگر_ختم

    Fspec_Rms = buf;				// for decc$to_vms

    اگر (strchr(instring,'/') == تہی)
	// It is already a VMS file spec
	strcpy(buf, instring);
    ورنہ اگر (strchr(instring,'"') == تہی)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	اگر ( (strchr(instring,'[') == تہی) && (strchr(instring,'<') == تہی) &&
	     (strchr(instring,']') == تہی) && (strchr(instring,'>') == تہی) &&
	     (strchr(instring,':') == تہی) )
	{
	    // It must be a truly unix fspec
	    decc$مقصد_vms(instring, vms_fspec_proc, 0, 0);
	}
	ورنہ
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    ورنہ
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    واپس buf;
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
    خالی
vms_remove_نسخہ(خالی * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    صحیح		مکمل = 0;
    صحیح		vdigits = 0;

    rp = (char_u *)fname+ لڑی_لمبائی( (حرف *)fname)- 1;    // Rightmost char
    جب ((مکمل == 0) && (rp > (char_u *)fname))       // Count digits
    {
	اگر (ہندسہ_ہے( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	ورنہ
	{
	    مکمل = 1;                                   // Quit at non-digit
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
	ورنہ اگر (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    اگر ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		مکمل = 0;
		جب ((مکمل == 0) && (dp >= (char_u *)fname))
		{
		    اگر ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			اگر ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    مکمل = 1;   // No previous dot found in name
			}
		    }
		    ورنہ اگر (*dp == '.')
		    {                   // Possible dot delimiter
			اگر ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dot delimiter
			    مکمل = 1;   // Previous dot found in name
			    *rp = '\0'; // Trim off ".nnn"
			}
		    }
		    dp--;               // Next char to right
		} // while
	    }
	}
	// Else no version found to remove
    } // if (vdigits  <= 5)

    واپس ;
}

ساخت typeahead_st {
    غیرعلامتی مختصر numchars;
    غیرعلامتی حرف  firstchar;
    غیرعلامتی حرف  reserved0;
    غیرعلامتی طویل  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    صحیح
RealWaitForChar(
    صحیح		fd غیر_مستعمل, // always read from iochan
    طویل	msec,
    صحیح		*check_for_gpm غیر_مستعمل,
    صحیح		*interrupted)
{
    صحیح حالت_2;
    ساخت _generic_64 وقت_curr;
    ساخت _generic_64 وقت_diff;
    ساخت _generic_64 وقت_برآمد;
    غیرعلامتی صحیح convert_operation = LIB$K_DELTA_SECONDS_F;
    اعشاری sec =(اعشاری) msec/1000;

    // make sure the iochan is set
    اگر (!iochan)
	get_tty();

    اگر (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	حالت_2 = sys$gettim(&وقت_curr);
	اگر (حالت_2 != SS$معمول_3)
	    واپس 0; // error
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
# ifdef lib$cvts_مقصد_اندرونی_وقت
#  define LIB_CVTX_مقصد_اندرونی_وقت lib$cvts_مقصد_اندرونی_وقت // IEEE
# ورنہ
#  define LIB_CVTX_مقصد_اندرونی_وقت lib$cvtf_مقصد_اندرونی_وقت
# endif
#ورنہ
# define LIB_CVTX_مقصد_اندرونی_وقت lib$cvtf_مقصد_اندرونی_وقت // VAX
#اگر_ختم // __IEEE_FLOAT CVTS

	حالت_2 = LIB_CVTX_مقصد_اندرونی_وقت(
		&convert_operation, &sec, &وقت_diff);
	اگر (حالت_2 != LIB$معمول_3)
	    واپس 0; // error
	// add them up
	حالت_2 = lib$جوڑو_times(
		&وقت_curr,
		&وقت_diff,
		&وقت_برآمد);
	اگر (حالت_2 != LIB$معمول_3)
	    واپس 0; // error
    }

    جب (TRUE)
    {
	// select()
	حالت_2 = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	اگر (حالت_2 != SS$معمول_3 || (iosb[0] & 0xFFFF) != SS$معمول_3)
	    واپس 0; // error

	اگر (typeahead.numchars)
	    واپس 1; // ready to read

	// there's nothing to read; what now?
	اگر (msec == 0)
	{
	    // immediate time-out; return impatiently
	    واپس 0;
	}
	ورنہ اگر (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    واپس 1; // fakeout to force a wait in vms_read()
	}
	ورنہ
	{
	    // time-out needs to be checked
	    حالت_2 = sys$gettim(&وقت_curr);
	    اگر (حالت_2 != SS$معمول_3)
		واپس 0; // error

	    حالت_2 = lib$sub_times(
		    &وقت_برآمد,
		    &وقت_curr,
		    &وقت_diff);
	    اگر (حالت_2 != LIB$معمول_3)
		واپس 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#اگر !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

قسمتعریف ساخت
   {
   حرف *name;
   صحیح قدر;
   } decc_feat_t;

صحیح vms_init_مکمل = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (حرف *)تہی, 0 } };


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

ساکن خالی
vms_init(خالی)
{
    // Set the global flag to indicate that LIB$INITIALIZE worked

    vms_init_مکمل = 1;

    صحیح feat_اشاریہ;
    صحیح feat_قدر;
    صحیح feat_قدر_max;
    صحیح feat_قدر_min;
    صحیح i;
    صحیح sts;

    // Loop through all items in the decc_feat_array[]
    برائے (i = 0; decc_feat_array[i].name != تہی; i++)
    {
	// Get the feature index
	feat_اشاریہ = decc$feature_get_اشاریہ(decc_feat_array[i].name);
	اگر (feat_اشاریہ >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_قدر = decc$feature_get_قدر(feat_اشاریہ, 1);
	    feat_قدر_min = decc$feature_get_قدر(feat_اشاریہ, 2);
	    feat_قدر_max = decc$feature_get_قدر(feat_اشاریہ, 3);

	    اگر ((decc_feat_array[i].قدر >= feat_قدر_min) && (decc_feat_array[i].قدر <= feat_قدر_max))
	    {
		// Valid value.  Set it if necessary
		اگر (feat_قدر != decc_feat_array[i].قدر)
		    sts = decc$feature_سیٹ_قدر(feat_اشاریہ, 1, decc_feat_array[i].قدر);
	    }
	    ورنہ
		// Invalid DECC feature value
		لکھو("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_قدر, feat_قدر_min, decc_feat_array[i].name, feat_قدر_max);
	}
	ورنہ
	    // Invalid DECC feature name
	    لکھو("UNKNOWN DECC FEATURE: %s.\n", decc_feat_array[i].name);
    }
}


/* Get "vms_init()" into a valid, loaded LIB$INITIALIZE PSECT. */

# pragma nostandard

/* Establish the LIB$INITIALIZE PSECTs, with proper alignment and
 * other attributes.  Note that "nopic" is significant only on VAX.
 */
# pragma extern_model save

# pragma extern_model strict_refdef "LIB$INITIALIZE" 2, nopic, nowrt
خالی (*ثابت x_vms_init)() = vms_init;

# pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
ثابت صحیح فالتو[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
صحیح LIB$INITIALIZE(خالی);
# pragma extern_model strict_refdef
صحیح dmy_lib$initialize = (صحیح) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#اگر_ختم // !defined( __VAX) && (__CRTL_VER >= 70301000)
