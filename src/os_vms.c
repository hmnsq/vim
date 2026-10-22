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

#अन्तर्भाव "vim.h"

// define _generic_64 for use in time functions
#जइ !defined(VAX)
# include <gen64def.h>
#अण्णहा
// based on Alpha's gen64def.h; the file is absent on VAX
प्रकारनिर्देश संरचना _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	unsigned पूर्णाङ्क gen64$l_longword [2];   // ...two 32-bit values, or
	unsigned short पूर्णाङ्क gen64$w_word [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#endif

प्रकारनिर्देश संरचना
{
    वर्ण	class;
    वर्ण	type;
    short	width;
    सङ्घ
    {
	संरचना
	{
	    वर्ण	_basic[3];
	    वर्ण	length;
	}	y;
	पूर्णाङ्क	basic;
    }	x;
    पूर्णाङ्क		extended;
}	TT_MODE;

प्रकारनिर्देश संरचना
{
    short	buflen;
    short	itemcode;
    वर्ण	*bufadrs;
    पूर्णाङ्क		*retlen;
}	ITEM;

प्रकारनिर्देश संरचना
{
    ITEM	equ;
    पूर्णाङ्क		nul;
}	ITMLST1;

प्रकारनिर्देश संरचना
{
    ITEM	index;
    ITEM	string;
    पूर्णाङ्क	nul;
}	ITMLST2;

स्थिर TT_MODE	orgmode;
स्थिर short	iochan;			// TTY I/O channel
स्थिर short	iosb[4];		// IO status block

स्थिर पूर्णाङ्क vms_match_अङ्कः = 0;
स्थिर पूर्णाङ्क vms_match_free = 0;
स्थिर char_u **vms_fmatch = रिक्त;
स्थिर वर्ण *Fspec_Rms;		       // rms file spec, passed implicitly between routines



स्थिर TT_MODE	get_tty(शून्य);
स्थिर शून्य	set_tty(पूर्णाङ्क row, पूर्णाङ्क col);

#define EXPL_ALLOC_INC 64

#define EQN(S1,S2,LN) (strncmp(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) करेहि { जाव (Str[1] == '/') ++Str; } जाव (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    स्थिर शून्य
vul_desc(DESC *des, वर्ण *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_pointer = str;
    des->dsc$w_length = str ? strlen(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    स्थिर शून्य
vul_item(ITEM *itm, short len, short cod, वर्ण *adr, पूर्णाङ्क *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    शून्य
mch_settmode(tmode_T tmode)
{
    पूर्णाङ्क	status;

    जइ ( tmode == TMODE_RAW )
	set_tty(0, 0);
    अण्णहा
    {
	विकल्प (orgmode.width)
	{
	    पक्ष 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	भङ्ग;
	    पक्ष 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	भङ्ग;
	    स्वभाव:	भङ्ग;
	}
	out_flush();
	status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, परिमाण(TT_MODE), 0,0,0,0);
	जइ (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	    प्रत्यागम;
	(शून्य)sys$dassgn(iochan);
	iochan = 0;
    }
}

    स्थिर शून्य
set_tty(पूर्णाङ्क row, पूर्णाङ्क col)
{
    पूर्णाङ्क		    status;
    TT_MODE	    newmode;		// New TTY mode bits
    स्थिर short    first_time = TRUE;

    जइ (first_time)
    {
	orgmode = get_tty();
	first_time = FALSE;
    }
    newmode = get_tty();
    जइ (col)
	newmode.width		 = col;
    जइ (row)
	newmode.x.y.length       = row;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, परिमाण(newmode), 0, 0, 0, 0);
    जइ (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	प्रत्यागम;
}

    स्थिर TT_MODE
get_tty(शून्य)
{

    स्थिर $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    पूर्णाङ्क		status;
    TT_MODE	tt_mode;

    जइ (!iochan)
	status = sys$assign(&odsc,&iochan,0,0);

    status = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, परिमाण(tt_mode), 0, 0, 0, 0);
    जइ (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.length	= 0;
	tt_mode.extended	= 0;
    }
    प्रत्यागम(tt_mode);
}

/*
 * Get the current window size in Rows and Columns.
 */
    पूर्णाङ्क
mch_get_shellsize(शून्य)
{
    TT_MODE	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    प्रत्यागम OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    शून्य
mch_set_shellsize(शून्य)
{
    set_tty(Rows, Columns);
    विकल्प (Columns)
    {
	पक्ष 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	भङ्ग;
	पक्ष 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	भङ्ग;
	स्वभाव:	भङ्ग;
    }
    out_flush();
    screen_start();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    स्थिर वर्ण		buffer[LNM$C_NAMLENGTH+1];
    char_u		*cp = रिक्त;
    unsigned long	attrib;
    पूर्णाङ्क			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    वर्ण		*sbuf = रिक्त;

    vul_desc(&d_lognam, (वर्ण *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.index, परिमाण(पूर्णाङ्क), LNM$_INDEX, (वर्ण *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, buffer, &lengte);
    itmlst.nul	= 0;
    जइ (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, रिक्त,&itmlst) == SS$_NORMAL)
    {
	buffer[lengte] = '\0';
	जइ (cp = alloc(lengte + 1))
	    strcpy((वर्ण *)cp, buffer);
	प्रत्यागम(cp);
    }
    अण्णहा जइ ((sbuf = getenv((वर्ण *)lognam)))
    {
	lengte = strlen(sbuf) + 1;
	cp = alloc(lengte);
	जइ (cp)
	    strcpy((वर्ण *)cp, sbuf);
	प्रत्यागम cp;
    }
    अण्णहा
	प्रत्यागम(रिक्त);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    पूर्णाङ्क
mch_setenv(वर्ण *var, वर्ण *मानम्, पूर्णाङ्क x)
{
    पूर्णाङ्क		res, dum;
    long	attrib = 0L;
    वर्ण	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_item(&itmlst.equ, मानम् ? strlen(मानम्) : 0, मानम् ? LNM$_STRING : 0,
	    मानम्, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    प्रत्यागम((res == 1) ? 0 : -1);
}

    पूर्णाङ्क
vms_sys(वर्ण *cmd, वर्ण *out, वर्ण *inp)
{
    DESC	cdsc, odsc, idsc;
    long	status;

    जइ (cmd)
	vul_desc(&cdsc, cmd);
    जइ (out)
	vul_desc(&odsc, out);
    जइ (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : रिक्त,		// command string
	      inp ? &idsc : रिक्त,		// input file
	      out ? &odsc : रिक्त,		// output file
	      0, 0, 0, &status, 0, 0, 0, 0, 0, 0);
    प्रत्यागम status;
}

/*
 * Convert string to lowercase - most often filename
 */
    वर्ण *
vms_tolower( वर्ण *name )
{
    पूर्णाङ्क i,nlen = strlen(name);
    प्रति (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    प्रत्यागम name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    पूर्णाङ्क
vms_sys_status(पूर्णाङ्क status)
{
    जइ (status != SS$_NORMAL && (status & STS$M_SUCCESS) == 0)
	प्रत्यागम status;		// Command failed.
    प्रत्यागम 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    पूर्णाङ्क
vms_read(वर्ण *inbuf, size_t nbytes)
{
    पूर्णाङ्क		status, function, len;
    TT_MODE	tt_mode;
    ITEM	itmlst[2];     // terminates on everything
    स्थिर long trm_mask[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    जइ (!iochan)
	tt_mode = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_item(&itmlst[0], 0, TRM$_MODIFIERS,
	 (वर्ण *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_item(&itmlst[1], परिमाण(trm_mask), TRM$_TERM, (वर्ण *)&trm_mask, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, परिमाण(itmlst));
    len = strlen(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = strlen(inbuf); // return the total length

    प्रत्यागम len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    स्थिर पूर्णाङ्क
vms_wproc(वर्ण *name, पूर्णाङ्क val)
{
    पूर्णाङ्क i;
    स्थिर पूर्णाङ्क vms_match_alloced = 0;

    जइ (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	प्रत्यागम 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    जइ (vms_match_अङ्कः == 0)
    {
	// first time through, setup some things
	जइ (रिक्त == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    जइ (!vms_fmatch)
		प्रत्यागम 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	अण्णहा
	{
	    // re-use existing space
	    vms_match_free = vms_match_alloced;
	}
    }

    // make matches look uniform
    vms_remove_version(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    प्रति (i = 0; i<vms_match_अङ्कः; i++)
    {
	जइ (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    प्रत्यागम 1;
    }
    जइ (--vms_match_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		परिमाण(वर्ण **) * vms_match_alloced);
	जइ (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    प्रत्यागम 0;
	}
	vms_match_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_match_अङ्कः] = vim_strsave((char_u *)name);

    ++vms_match_अङ्कः;
    प्रत्यागम 1;
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
    पूर्णाङ्क
mch_expand_wildcards(
    पूर्णाङ्क		अङ्कः_pat,
    char_u	**pat,
    पूर्णाङ्क		*अङ्कः_file,
    char_u	***file,
    पूर्णाङ्क		flags)
{
    पूर्णाङ्क		i, cnt = 0;
    char_u	buf[MAXPATHL];
    वर्ण       *result;
    पूर्णाङ्क		dir;
    पूर्णाङ्क files_alloced, files_free;

    *अङ्कः_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    जइ (*file == रिक्त)
    {
	*अङ्कः_file = 0;
	प्रत्यागम FAIL;
    }
    प्रति (i = 0; i < अङ्कः_pat; i++)
    {
	// expand environment var or home dir
	जइ (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	अण्णहा
	    STRCPY(buf,pat[i]);

	vms_match_अङ्कः = 0; // reset collection counter
	result = decc$translate_vms(vms_fixfilename(buf));
	जइ ( (पूर्णाङ्क) result == 0 || (पूर्णाङ्क) result == -1  )
	{
	    cnt = 0;
	}
	अण्णहा
	{
	    cnt = decc$to_vms(result, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	जइ (cnt > 0)
	    cnt = vms_match_अङ्कः;

	जइ (cnt < 1)
	    अनुवर्त;

	प्रति (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    जइ (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		अनुवर्त;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    जइ (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		अनुवर्त;

	    // Skip files that are not executable if we check for that.
	    जइ (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], रिक्त, !(flags & EW_SHELLCMD)))
		अनुवर्त;

	    // allocate memory for pointers
	    जइ (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, परिमाण(char_u **) * files_alloced);
		जइ (*file == रिक्त)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *अङ्कः_file = 0;
		    प्रत्यागम(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*अङ्कः_file++] = vms_fmatch[i];
	}
    }
    प्रत्यागम OK;
}

    पूर्णाङ्क
mch_expandpath(garray_T *gap, char_u *path, पूर्णाङ्क flags)
{
    पूर्णाङ्क		i,cnt = 0;
    वर्ण       *result;

    vms_match_अङ्कः = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    result = decc$translate_vms(vms_fixfilename(path));
    जइ ( (पूर्णाङ्क) result == 0 || (पूर्णाङ्क) result == -1  )
    {
	cnt = 0;
    }
    अण्णहा
    {
	cnt = decc$to_vms(result, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    जइ (cnt > 0)
	cnt = vms_match_अङ्कः;
    प्रति (i = 0; i < cnt; i++)
    {
	जइ (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    प्रत्यागम cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    स्थिर शून्य
vms_unix_mixed_filespec(वर्ण *in, वर्ण *out)
{
    वर्ण *lastcolon;
    वर्ण *end_of_dir;
    वर्ण ch;
    पूर्णाङ्क len;
    वर्ण *out_str=out;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(in, ':');   // find last colon
    जइ (lastcolon != रिक्त)
    {
	len = lastcolon - in + 1;
	strncpy(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = रिक्त;	// default: no directory

    // start of directory portion
    ch = *in;
    जइ ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    }
    अण्णहा जइ (EQN(in, "../", 3))      // Unix parent directory?
    {
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    }
    अण्णहा
    {                                // not a special character
	जाव (EQN(in, "./", 2))     // Ignore Unix "current dir"
	{
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    जइ (strchr(in, '/') == रिक्त)     // any more Unix directories ?
    {
	strcpy(out, in);             // No - get rest of the spec
	प्रत्यागम;
    }
    अण्णहा
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

    जाव (*in != '\0')
    {
	ch = *in;
	जइ ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	अण्णहा जइ (EQN(in, "../", 3))     // Unix parent directory?
	{
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	अण्णहा
	{
	    जाव (EQN(in, "./", 2))    // Ignore Unix "current dir"
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

    जइ (end_of_dir != रिक्त) // Terminate directory portion
	*end_of_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    स्थिर पूर्णाङ्क
vms_fspec_proc(वर्ण *fil, पूर्णाङ्क val)
{
    strcpy(Fspec_Rms,fil);
    प्रत्यागम(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    शून्य *
vms_fixfilename(शून्य *instring)
{
    स्थिर वर्ण		*buf = रिक्त;
    स्थिर size_t	buflen = 0;
    size_t		len;

    // get a big-enough buffer
    len = strlen(instring) + 1;
    जइ (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * परिमाण(वर्ण));
    }

#ifdef DEBUG
    वर्ण		 *tmpbuf = रिक्त;
    tmpbuf = ALLOC_MULT(वर्ण, buflen);
    strcpy(tmpbuf, instring);
#endif

    Fspec_Rms = buf;				// for decc$to_vms

    जइ (strchr(instring,'/') == रिक्त)
	// It is already a VMS file spec
	strcpy(buf, instring);
    अण्णहा जइ (strchr(instring,'"') == रिक्त)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	जइ ( (strchr(instring,'[') == रिक्त) && (strchr(instring,'<') == रिक्त) &&
	     (strchr(instring,']') == रिक्त) && (strchr(instring,'>') == रिक्त) &&
	     (strchr(instring,':') == रिक्त) )
	{
	    // It must be a truly unix fspec
	    decc$to_vms(instring, vms_fspec_proc, 0, 0);
	}
	अण्णहा
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    अण्णहा
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    प्रत्यागम buf;
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
    शून्य
vms_remove_version(शून्य * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    पूर्णाङ्क		done = 0;
    पूर्णाङ्क		vdigits = 0;

    rp = (char_u *)fname+ strlen( (वर्ण *)fname)- 1;    // Rightmost char
    जाव ((done == 0) && (rp > (char_u *)fname))       // Count digits
    {
	जइ (isdigit( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	अण्णहा
	{
	    done = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    जइ (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	जइ (*rp == (char_u)';')
	{
	    जइ ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	अण्णहा जइ (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    जइ ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		done = 0;
		जाव ((done == 0) && (dp >= (char_u *)fname))
		{
		    जइ ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			जइ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    done = 1;   // No previous dot found in name
			}
		    }
		    अण्णहा जइ (*dp == '.')
		    {                   // Possible dot delimiter
			जइ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
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

    प्रत्यागम ;
}

संरचना typeahead_st {
    unsigned short numchars;
    unsigned वर्ण  firstchar;
    unsigned वर्ण  reserved0;
    unsigned long  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    पूर्णाङ्क
RealWaitForChar(
    पूर्णाङ्क		fd UNUSED, // always read from iochan
    long	msec,
    पूर्णाङ्क		*check_for_gpm UNUSED,
    पूर्णाङ्क		*interrupted)
{
    पूर्णाङ्क status;
    संरचना _generic_64 time_curr;
    संरचना _generic_64 time_diff;
    संरचना _generic_64 time_out;
    unsigned पूर्णाङ्क convert_operation = LIB$K_DELTA_SECONDS_F;
    भिन्न sec =(भिन्न) msec/1000;

    // make sure the iochan is set
    जइ (!iochan)
	get_tty();

    जइ (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	status = sys$gettim(&time_curr);
	जइ (status != SS$_NORMAL)
	    प्रत्यागम 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#जइ __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_to_internal_time
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvts_to_internal_time // IEEE
# अण्णहा
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time
# endif
#अण्णहा
# define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time // VAX
#endif // __IEEE_FLOAT CVTS

	status = LIB_CVTX_TO_INTERNAL_TIME(
		&convert_operation, &sec, &time_diff);
	जइ (status != LIB$_NORMAL)
	    प्रत्यागम 0; // error
	// add them up
	status = lib$add_times(
		&time_curr,
		&time_diff,
		&time_out);
	जइ (status != LIB$_NORMAL)
	    प्रत्यागम 0; // error
    }

    जाव (TRUE)
    {
	// select()
	status = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	जइ (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
	    प्रत्यागम 0; // error

	जइ (typeahead.numchars)
	    प्रत्यागम 1; // ready to read

	// there's nothing to read; what now?
	जइ (msec == 0)
	{
	    // immediate time-out; return impatiently
	    प्रत्यागम 0;
	}
	अण्णहा जइ (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    प्रत्यागम 1; // fakeout to force a wait in vms_read()
	}
	अण्णहा
	{
	    // time-out needs to be checked
	    status = sys$gettim(&time_curr);
	    जइ (status != SS$_NORMAL)
		प्रत्यागम 0; // error

	    status = lib$sub_times(
		    &time_out,
		    &time_curr,
		    &time_diff);
	    जइ (status != LIB$_NORMAL)
		प्रत्यागम 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#जइ !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

प्रकारनिर्देश संरचना
   {
   वर्ण *name;
   पूर्णाङ्क मानम्;
   } decc_feat_t;

पूर्णाङ्क vms_init_done = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (वर्ण *)रिक्त, 0 } };


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

स्थिर शून्य
vms_init(शून्य)
{
    // Set the global flag to indicate that LIB$INITIALIZE worked

    vms_init_done = 1;

    पूर्णाङ्क feat_index;
    पूर्णाङ्क feat_मानम्;
    पूर्णाङ्क feat_मानम्_max;
    पूर्णाङ्क feat_मानम्_min;
    पूर्णाङ्क i;
    पूर्णाङ्क sts;

    // Loop through all items in the decc_feat_array[]
    प्रति (i = 0; decc_feat_array[i].name != रिक्त; i++)
    {
	// Get the feature index
	feat_index = decc$feature_get_index(decc_feat_array[i].name);
	जइ (feat_index >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_मानम् = decc$feature_get_मानम्(feat_index, 1);
	    feat_मानम्_min = decc$feature_get_मानम्(feat_index, 2);
	    feat_मानम्_max = decc$feature_get_मानम्(feat_index, 3);

	    जइ ((decc_feat_array[i].मानम् >= feat_मानम्_min) && (decc_feat_array[i].मानम् <= feat_मानम्_max))
	    {
		// Valid value.  Set it if necessary
		जइ (feat_मानम् != decc_feat_array[i].मानम्)
		    sts = decc$feature_set_मानम्(feat_index, 1, decc_feat_array[i].मानम्);
	    }
	    अण्णहा
		// Invalid DECC feature value
		printf("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_मानम्, feat_मानम्_min, decc_feat_array[i].name, feat_मानम्_max);
	}
	अण्णहा
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
शून्य (*ध्रुव x_vms_init)() = vms_init;

# pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
ध्रुव पूर्णाङ्क spare[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
पूर्णाङ्क LIB$INITIALIZE(शून्य);
# pragma extern_model strict_refdef
पूर्णाङ्क dmy_lib$initialize = (पूर्णाङ्क) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#endif // !defined( __VAX) && (__CRTL_VER >= 70301000)
