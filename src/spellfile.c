/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * spellfile.c: code for reading and writing spell files.
 *
 * See spell.c for information about spell checking.
 */

/*
 * Vim spell file format: <HEADER>
 *			  <SECTIONS>
 *			  <LWORDTREE>
 *			  <KWORDTREE>
 *			  <PREFIXTREE>
 *
 * <HEADER>: <fileID> <versionnr>
 *
 * <fileID>     8 bytes    "VIMspell"
 * <versionnr>  1 byte	    VIMSPELLVERSION
 *
 *
 * Sections make it possible to add information to the .spl file without
 * making it incompatible with previous versions.  There are two kinds of
 * sections:
 * 1. Not essential for correct spell checking.  E.g. for making suggestions.
 *    These are skipped when not supported.
 * 2. Optional information, but essential for spell checking when present.
 *    E.g. conditions for affixes.  When this section is present but not
 *    supported an error message is given.
 *
 * <SECTIONS>: <section> ... <sectionend>
 *
 * <section>: <sectionID> <sectionflags> <sectionlen> (section contents)
 *
 * <sectionID>	  1 byte    number from 0 to 254 identifying the section
 *
 * <sectionflags> 1 byte    SNF_REQUIRED: this section is required for correct
 *					    spell checking
 *
 * <sectionlen>   4 bytes   length of section contents, MSB first
 *
 * <sectionend>	  1 byte    SN_END
 *
 *
 * sectionID == SN_INFO: <infotext>
 * <infotext>	 N bytes    free format text with spell file info (version,
 *			    website, etc)
 *
 * sectionID == SN_REGION: <regionname> ...
 * <regionname>	 2 bytes    Up to MAXREGIONS region names: ca, au, etc.  Lower
 *			    case.  First <regionname> is region 1.
 *
 * sectionID == SN_CHARFLAGS: <charflagslen> <charflags>
 *				<folcharslen> <folchars>
 * <charflagslen> 1 byte    Number of bytes in <charflags> (should be 128).
 * <charflags>  N bytes     List of flags (first one is for character 128):
 *			    0x01  word character	CF_WORD
 *			    0x02  upper-case character	CF_UPPER
 * <folcharslen>  2 bytes   Number of bytes in <folchars>.
 * <folchars>     N bytes   Folded characters, first one is for character 128.
 *
 * sectionID == SN_MIDWORD: <midword>
 * <midword>     N bytes    Characters that are word characters only when used
 *			    in the middle of a word.
 *
 * sectionID == SN_PREFCOND: <prefcondcnt> <prefcond> ...
 * <prefcondcnt> 2 bytes    Number of <prefcond> items following.
 * <prefcond> : <condlen> <condstr>
 * <condlen>	1 byte	    Length of <condstr>.
 * <condstr>	N bytes	    Condition for the prefix.
 *
 * sectionID == SN_REP: <repcount> <rep> ...
 * <repcount>	 2 bytes    number of <rep> items, MSB first.
 * <rep> : <repfromlen> <repfrom> <reptolen> <repto>
 * <repfromlen>	 1 byte	    length of <repfrom>
 * <repfrom>	 N bytes    "from" part of replacement
 * <reptolen>	 1 byte	    length of <repto>
 * <repto>	 N bytes    "to" part of replacement
 *
 * sectionID == SN_REPSAL: <repcount> <rep> ...
 *   just like SN_REP but for soundfolded words
 *
 * sectionID == SN_SAL: <salflags> <salcount> <sal> ...
 * <salflags>	 1 byte	    flags for soundsalike conversion:
 *			    SAL_F0LLOWUP
 *			    SAL_COLLAPSE
 *			    SAL_REM_ACCENTS
 * <salcount>    2 bytes    number of <sal> items following
 * <sal> : <salfromlen> <salfrom> <saltolen> <salto>
 * <salfromlen>	 1 byte	    length of <salfrom>
 * <salfrom>	 N bytes    "from" part of soundsalike
 * <saltolen>	 1 byte	    length of <salto>
 * <salto>	 N bytes    "to" part of soundsalike
 *
 * sectionID == SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
 * <sofofromlen> 2 bytes    length of <sofofrom>
 * <sofofrom>	 N bytes    "from" part of soundfold
 * <sofotolen>	 2 bytes    length of <sofoto>
 * <sofoto>	 N bytes    "to" part of soundfold
 *
 * sectionID == SN_SUGFILE: <timestamp>
 * <timestamp>   8 bytes    time in seconds that must match with .sug file
 *
 * sectionID == SN_NOSPLITSUGS: nothing
	 *
 * sectionID == SN_NOCOMPOUNDSUGS: nothing
 *
 * sectionID == SN_WORDS: <word> ...
 * <word>	 N bytes    NUL terminated common word
 *
 * sectionID == SN_MAP: <mapstr>
 * <mapstr>	 N bytes    String with sequences of similar characters,
 *			    separated by slashes.
 *
 * sectionID == SN_COMPOUND: <compmax> <compminlen> <compsylmax> <compoptions>
 *				<comppatcount> <comppattern> ... <compflags>
 * <compmax>     1 byte	    Maximum nr of words in compound word.
 * <compminlen>  1 byte	    Minimal word length for compounding.
 * <compsylmax>  1 byte	    Maximum nr of syllables in compound word.
 * <compoptions> 2 bytes    COMP_ flags.
 * <comppatcount> 2 bytes   number of <comppattern> following
 * <compflags>   N bytes    Flags from COMPOUNDRULE items, separated by
 *			    slashes.
 *
 * <comppattern>: <comppatlen> <comppattext>
 * <comppatlen>	 1 byte	    length of <comppattext>
 * <comppattext> N bytes    end or begin chars from CHECKCOMPOUNDPATTERN
 *
 * sectionID == SN_NOBREAK: (empty, its presence is what matters)
 *
 * sectionID == SN_SYLLABLE: <syllable>
 * <syllable>    N bytes    String from SYLLABLE item.
 *
 * <LWORDTREE>: <wordtree>
 *
 * <KWORDTREE>: <wordtree>
 *
 * <PREFIXTREE>: <wordtree>
 *
 *
 * <wordtree>: <nodecount> <nodedata> ...
 *
 * <nodecount>	4 bytes	    Number of nodes following.  MSB first.
 *
 * <nodedata>: <siblingcount> <sibling> ...
 *
 * <siblingcount> 1 byte    Number of siblings in this node.  The siblings
 *			    follow in sorted order.
 *
 * <sibling>: <byte> [ <nodeidx> <xbyte>
 *		      | <flags> [<flags2>] [<region>] [<affixID>]
 *		      | [<pflags>] <affixID> <prefcondnr> ]
 *
 * <byte>	1 byte	    Byte value of the sibling.  Special cases:
 *			    BY_NOFLAGS: End of word without flags and for all
 *					regions.
 *					For PREFIXTREE <affixID> and
 *					<prefcondnr> follow.
 *			    BY_FLAGS:   End of word, <flags> follow.
 *					For PREFIXTREE <pflags>, <affixID>
 *					and <prefcondnr> follow.
 *			    BY_FLAGS2:  End of word, <flags> and <flags2>
 *					follow.  Not used in PREFIXTREE.
 *			    BY_INDEX:   Child of sibling is shared, <nodeidx>
 *					and <xbyte> follow.
 *
 * <nodeidx>	3 bytes	    Index of child for this sibling, MSB first.
 *
 * <xbyte>	1 byte	    byte value of the sibling.
 *
 * <flags>	1 byte	    bitmask of:
 *			    WF_ALLCAP	word must have only capitals
 *			    WF_ONECAP   first char of word must be capital
 *			    WF_KEEPCAP	keep-case word
 *			    WF_FIXCAP   keep-case word, all caps not allowed
 *			    WF_RARE	rare word
 *			    WF_BANNED	bad word
 *			    WF_REGION	<region> follows
 *			    WF_AFX	<affixID> follows
 *
 * <flags2>	1 byte	    Bitmask of:
 *			    WF_HAS_AFF >> 8   word includes affix
 *			    WF_NEEDCOMP >> 8  word only valid in compound
 *			    WF_NOSUGGEST >> 8  word not used for suggestions
 *			    WF_COMPROOT >> 8  word already a compound
 *			    WF_NOCOMPBEF >> 8 no compounding before this word
 *			    WF_NOCOMPAFT >> 8 no compounding after this word
 *
 * <pflags>	1 byte	    bitmask of:
 *			    WFP_RARE	rare prefix
 *			    WFP_NC	non-combining prefix
 *			    WFP_UP	letter after prefix made upper case
 *
 * <region>	1 byte	    Bitmask for regions in which word is valid.  When
 *			    omitted it's valid in all regions.
 *			    Lowest bit is for region 1.
 *
 * <affixID>	1 byte	    ID of affix that can be used with this word.  In
 *			    PREFIXTREE used for the required prefix ID.
 *
 * <prefcondnr>	2 bytes	    Prefix condition number, index in <prefcond> list
 *			    from HEADER.
 *
 * All text characters are in 'encoding', but stored as single bytes.
 */

/*
 * Vim .sug file format:  <SUGHEADER>
 *			  <SUGWORDTREE>
 *			  <SUGTABLE>
 *
 * <SUGHEADER>: <fileID> <versionnr> <timestamp>
 *
 * <fileID>     6 bytes     "VIMsug"
 * <versionnr>  1 byte      VIMSUGVERSION
 * <timestamp>  8 bytes     timestamp that must match with .spl file
 *
 *
 * <SUGWORDTREE>: <wordtree>  (see above, no flags or region used)
 *
 *
 * <SUGTABLE>: <sugwcount> <sugline> ...
 *
 * <sugwcount>	4 bytes	    number of <sugline> following
 *
 * <sugline>: <sugnr> ... NUL
 *
 * <sugnr>:     X bytes     word number that results in this soundfolded word,
 *			    stored as an offset to the previous number in as
 *			    few bytes as possible, see offset2bytes())
 */

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_SPELL)

#অন্তর্ভাব <time.h>	// for time_t

// Special byte values for <byte>.  Some are only used in the tree for
// postponed prefixes, some only in the other trees.  This is a bit messy...
#define BY_NOFLAGS	0	// end of word without flags or region; for
				// postponed prefix: no <pflags>
#define BY_INDEX	1	// child is shared, index follows
#define BY_FLAGS	2	// end of word, <flags> byte follows; for
				// postponed prefix: <pflags> follows
#define BY_FLAGS2	3	// end of word, <flags> and <flags2> bytes
				// follow; never used in prefix tree
#define BY_SPECIAL  BY_FLAGS2	// highest special byte value

#define ZERO_FLAG	65009	// used when flag is zero: "0"

// Flags used in .spl file for soundsalike flags.
#define SAL_F0LLOWUP		1
#define SAL_COLLAPSE		2
#define SAL_REM_ACCENTS		4

#define VIMSPELLMAGIC "VIMspell"  // string at start of Vim spell file
#define VIMSPELLMAGICL 8
#define VIMSPELLVERSION 50

// Section IDs.  Only renumber them when VIMSPELLVERSION changes!
#define SN_REGION	0	// <regionname> section
#define SN_CHARFLAGS	1	// charflags section
#define SN_MIDWORD	2	// <midword> section
#define SN_PREFCOND	3	// <prefcond> section
#define SN_REP		4	// REP items section
#define SN_SAL		5	// SAL items section
#define SN_SOFO		6	// soundfolding section
#define SN_MAP		7	// MAP items section
#define SN_COMPOUND	8	// compound words section
#define SN_SYLLABLE	9	// syllable section
#define SN_NOBREAK	10	// NOBREAK section
#define SN_SUGFILE	11	// timestamp for .sug file
#define SN_REPSAL	12	// REPSAL items section
#define SN_শব্দগুলি	13	// common words
#define SN_NOSPLITSUGS	14	// don't split word for suggestions
#define SN_INFO		15	// info section
#define SN_NOCOMPOUNDSUGS 16	// don't compound for suggestions
#define SN_END		255	// end of sections

#define SNF_REQUIRED	1	// <sectionflags>: required section

#define CF_WORD		0x01
#define CF_UPPER	0x02

// Max allowed length for COMPOUND section
#define COMPOUND_MAX_LEN	100000

/*
 * Loop through all the siblings of a node (including the node)
 */
#define FOR_ALL_গ্রন্থি_SIBLINGS(গ্রন্থি, np) \
    প্রতি ((np) = (গ্রন্থি); (np) != রিক্ত; (np) = (np)->wn_sibling)

স্থির পূর্ণাঙ্ক set_spell_finish(spelltab_T	*new_st);
স্থির পূর্ণাঙ্ক write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv);
স্থির পূর্ণাঙ্ক read_region_section(FILE *fd, slang_T *slang, পূর্ণাঙ্ক len);
স্থির পূর্ণাঙ্ক read_charflags_section(FILE *fd);
স্থির পূর্ণাঙ্ক read_prefcond_section(FILE *fd, slang_T *lp);
স্থির পূর্ণাঙ্ক read_rep_section(FILE *fd, garray_T *gap, short *first);
স্থির পূর্ণাঙ্ক read_sal_section(FILE *fd, slang_T *slang);
স্থির পূর্ণাঙ্ক read_শব্দগুলি_section(FILE *fd, slang_T *lp, পূর্ণাঙ্ক len);
স্থির পূর্ণাঙ্ক read_sofo_section(FILE *fd, slang_T *slang);
স্থির পূর্ণাঙ্ক read_compound(FILE *fd, slang_T *slang, পূর্ণাঙ্ক len);
স্থির পূর্ণাঙ্ক set_sofo(slang_T *lp, char_u *from, char_u *to);
স্থির শূন্য set_sal_first(slang_T *lp);
স্থির পূর্ণাঙ্ক *mb_str2wide(char_u *s);
স্থির পূর্ণাঙ্ক spell_read_বৃক্ষ(FILE *fd, char_u **bytsp, long *bytsp_len, idx_T **idxsp, পূর্ণাঙ্ক prefixtree, পূর্ণাঙ্ক prefixcnt);
স্থির idx_T read_বৃক্ষ_গ্রন্থি(FILE *fd, char_u *byts, idx_T *idxs, পূর্ণাঙ্ক maxidx, idx_T startidx, পূর্ণাঙ্ক prefixtree, পূর্ণাঙ্ক maxprefcondnr, পূর্ণাঙ্ক depth);
স্থির শূন্য set_spell_charflags(char_u *flags, পূর্ণাঙ্ক cnt, char_u *upp);
স্থির পূর্ণাঙ্ক set_spell_chartab(char_u *fol, char_u *নিচের, char_u *upp);
স্থির শূন্য set_map_str(slang_T *lp, char_u *map);


স্থির বর্ণ *e_afftrailing = N_("Trailing text in %s line %d: %s");
স্থির বর্ণ *e_affname = N_("Affix name too long in %s line %d: %s");
স্থির বর্ণ *বার্তা_compressing = N_("Compressing word tree...");

/*
 * Load one spell file and store the info into a slang_T.
 *
 * This is invoked in three ways:
 * - From spell_load_cb() to load a spell file for the first time.  "lang" is
 *   the language name, "old_lp" is NULL.  Will allocate an slang_T.
 * - To reload a spell file that was changed.  "lang" is NULL and "old_lp"
 *   points to the existing slang_T.
 * - Just after writing a .spl file; it's read back to produce the .sug file.
 *   "old_lp" is NULL and "lang" is NULL.  Will allocate an slang_T.
 *
 * Returns the slang_T the spell file was loaded into.  NULL for error.
 */
    slang_T *
spell_load_file(
    char_u	*fname,
    char_u	*lang,
    slang_T	*old_lp,
    পূর্ণাঙ্ক		silent)		// no error if file doesn't exist
{
    FILE	*fd;
    char_u	buf[VIMSPELLMAGICL];
    char_u	*p;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		len;
    slang_T	*lp = রিক্ত;
    পূর্ণাঙ্ক		c = 0;
    পূর্ণাঙ্ক		res;
    পূর্ণাঙ্ক		did_estack_ঠেলো = FALSE;
    ESTACK_CHECK_DECLARATION;

    fd = mch_fopen((বর্ণ *)fname, "r");
    যদি (fd == রিক্ত)
    {
	যদি (!silent)
	    semsg(_(e_cant_open_file_str), fname);
	অন্যথা যদি (p_verbose > 2)
	{
	    verbose_enter();
	    smsg((ধ্রুব বর্ণ *)e_cant_open_file_str, fname);
	    verbose_leave();
	}
	গচ্ছ endFAIL;
    }
    যদি (p_verbose > 2)
    {
	verbose_enter();
	smsg(_("Reading spell file \"%s\""), fname);
	verbose_leave();
    }

    যদি (old_lp == রিক্ত)
    {
	lp = slang_alloc(lang);
	যদি (lp == রিক্ত)
	    গচ্ছ endFAIL;

	// Remember the file name, used to reload the file when it's updated.
	lp->sl_fname = vim_strsave(fname);
	যদি (lp->sl_fname == রিক্ত)
	    গচ্ছ endFAIL;

	// Check for .add.spl (_add.spl for VMS).
	lp->sl_add = strstr((বর্ণ *)gettail(fname), SPL_FNAME_ADD) != রিক্ত;
    }
    অন্যথা
	lp = old_lp;

    // Set sourcing_name, so that error messages mention the file name.
    estack_ঠেলো(ETYPE_SPELL, fname, 0);
    ESTACK_CHECK_SETUP;
    did_estack_ঠেলো = TRUE;

    /*
     * <HEADER>: <fileID>
     */
    প্রতি (i = 0; i < VIMSPELLMAGICL; ++i)
	buf[i] = (c = getc(fd)) == EOF ? 0 : c;		// <fileID>
    যদি (STRNCMP(buf, VIMSPELLMAGIC, VIMSPELLMAGICL) != 0)
    {
	emsg(_(e_this_does_not_look_like_spell_file));
	গচ্ছ endFAIL;
    }
    c = getc(fd);					// <versionnr>
    যদি (c < VIMSPELLVERSION)
    {
	emsg(_(e_old_spell_file_needs_to_be_updated));
	গচ্ছ endFAIL;
    }
    অন্যথা যদি (c > VIMSPELLVERSION)
    {
	emsg(_(e_spell_file_is_for_newer_version_of_vim));
	গচ্ছ endFAIL;
    }


    /*
     * <SECTIONS>: <section> ... <sectionend>
     * <section>: <sectionID> <sectionflags> <sectionlen> (section contents)
     */
    প্রতি (;;)
    {
	n = getc(fd);			    // <sectionID> or <sectionend>
	যদি (n == SN_END)
	    ভঙ্গ;
	c = getc(fd);					// <sectionflags>
	len = get4c(fd);				// <sectionlen>
	যদি (len < 0)
	    গচ্ছ truncerr;

	res = 0;
	বিকল্প (n)
	{
	    পক্ষ SN_INFO:
		lp->sl_info = read_string(fd, len);	// <infotext>
		যদি (lp->sl_info == রিক্ত)
		    গচ্ছ endFAIL;
		ভঙ্গ;

	    পক্ষ SN_REGION:
		res = read_region_section(fd, lp, len);
		ভঙ্গ;

	    পক্ষ SN_CHARFLAGS:
		res = read_charflags_section(fd);
		ভঙ্গ;

	    পক্ষ SN_MIDWORD:
		lp->sl_midword = read_string(fd, len);	// <midword>
		যদি (lp->sl_midword == রিক্ত)
		    গচ্ছ endFAIL;
		ভঙ্গ;

	    পক্ষ SN_PREFCOND:
		res = read_prefcond_section(fd, lp);
		ভঙ্গ;

	    পক্ষ SN_REP:
		res = read_rep_section(fd, &lp->sl_rep, lp->sl_rep_first);
		ভঙ্গ;

	    পক্ষ SN_REPSAL:
		res = read_rep_section(fd, &lp->sl_repsal, lp->sl_repsal_first);
		ভঙ্গ;

	    পক্ষ SN_SAL:
		res = read_sal_section(fd, lp);
		ভঙ্গ;

	    পক্ষ SN_SOFO:
		res = read_sofo_section(fd, lp);
		ভঙ্গ;

	    পক্ষ SN_MAP:
		p = read_string(fd, len);		// <mapstr>
		যদি (p == রিক্ত)
		    গচ্ছ endFAIL;
		set_map_str(lp, p);
		vim_free(p);
		ভঙ্গ;

	    পক্ষ SN_শব্দগুলি:
		res = read_শব্দগুলি_section(fd, lp, len);
		ভঙ্গ;

	    পক্ষ SN_SUGFILE:
		lp->sl_sugtime = get8ctime(fd);		// <timestamp>
		ভঙ্গ;

	    পক্ষ SN_NOSPLITSUGS:
		lp->sl_nosplitsugs = TRUE;
		ভঙ্গ;

	    পক্ষ SN_NOCOMPOUNDSUGS:
		lp->sl_nocompoundsugs = TRUE;
		ভঙ্গ;

	    পক্ষ SN_COMPOUND:
		res = read_compound(fd, lp, len);
		ভঙ্গ;

	    পক্ষ SN_NOBREAK:
		lp->sl_nobreak = TRUE;
		ভঙ্গ;

	    পক্ষ SN_SYLLABLE:
		lp->sl_syllable = read_string(fd, len);	// <syllable>
		যদি (lp->sl_syllable == রিক্ত)
		    গচ্ছ endFAIL;
		যদি (init_syl_tab(lp) != OK)
		    গচ্ছ endFAIL;
		ভঙ্গ;

	    স্বভাব:
		// Unsupported section.  When it's required give an error
		// message.  When it's not required skip the contents.
		যদি (c & SNF_REQUIRED)
		{
		    emsg(_(e_unsupported_section_in_spell_file));
		    গচ্ছ endFAIL;
		}
		যাবত্ (--len >= 0)
		    যদি (getc(fd) < 0)
			গচ্ছ truncerr;
		ভঙ্গ;
	}
someerror:
	যদি (res == SP_FORMERROR)
	{
	    emsg(_(e_format_error_in_spell_file));
	    গচ্ছ endFAIL;
	}
	যদি (res == SP_TRUNCERROR)
	{
truncerr:
	    emsg(_(e_truncated_spell_file));
	    গচ্ছ endFAIL;
	}
	যদি (res == SP_OTHERERROR)
	    গচ্ছ endFAIL;
    }

    // <LWORDTREE>
    res = spell_read_বৃক্ষ(fd, &lp->sl_fbyts, &lp->sl_fbyts_len,
						      &lp->sl_fidxs, FALSE, 0);
    যদি (res != 0)
	গচ্ছ someerror;

    // <KWORDTREE>
    res = spell_read_বৃক্ষ(fd, &lp->sl_kbyts, রিক্ত, &lp->sl_kidxs, FALSE, 0);
    যদি (res != 0)
	গচ্ছ someerror;

    // <PREFIXTREE>
    res = spell_read_বৃক্ষ(fd, &lp->sl_pbyts, রিক্ত, &lp->sl_pidxs, TRUE,
							    lp->sl_prefixcnt);
    যদি (res != 0)
	গচ্ছ someerror;

    // For a new file link it in the list of spell files.
    যদি (old_lp == রিক্ত && lang != রিক্ত)
    {
	lp->sl_next = first_lang;
	first_lang = lp;
    }

    গচ্ছ endOK;

endFAIL:
    যদি (lang != রিক্ত)
	// truncating the name signals the error to spell_load_lang()
	*lang = NUL;
    যদি (lp != রিক্ত && old_lp == রিক্ত)
	slang_free(lp);
    lp = রিক্ত;

endOK:
    যদি (fd != রিক্ত)
	fclose(fd);
    যদি (did_estack_ঠেলো)
    {
	ESTACK_CHECK_NOW;
	estack_বের_করো();
    }

    প্রত্যাগম lp;
}

/*
 * Fill in the wordcount fields for a trie.
 * Returns the total number of words.
 */
    স্থির শূন্য
বৃক্ষ_গুনতি_শব্দগুলি(char_u *byts, long byts_len, idx_T *idxs)
{
    পূর্ণাঙ্ক		depth;
    idx_T	arridx[MAXWLEN];
    পূর্ণাঙ্ক		curi[MAXWLEN];
    পূর্ণাঙ্ক		c;
    idx_T	n;
    পূর্ণাঙ্ক		wordcount[MAXWLEN];

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;
    depth = 0;
    যাবত্ (depth >= 0 && !got_int)
    {
	যদি (curi[depth] > byts[arridx[depth]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[depth]] = wordcount[depth];
	    যদি (depth > 0)
		wordcount[depth - 1] += wordcount[depth];

	    --depth;
	    fast_breakcheck();
	}
	অন্যথা
	{
	    // Do one more byte at this node.
	    n = arridx[depth] + curi[depth];
	    ++curi[depth];

	    c = byts[n];
	    যদি (c == 0)
	    {
		// End of word, count it.
		++wordcount[depth];

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		যাবত্ (n + 1 < byts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[depth];
		}
	    }
	    অন্যথা যদি (depth < MAXWLEN - 1)
	    {
		// Normal char, go one level deeper to count the words.
		++depth;
		arridx[depth] = idxs[n];
		curi[depth] = 1;
		wordcount[depth] = 0;
	    }
	}
    }
}

/*
 * Load the .sug files for languages that have one and weren't loaded yet.
 */
    শূন্য
suggest_load_files(শূন্য)
{
    langp_T	*lp;
    পূর্ণাঙ্ক		lpi;
    slang_T	*slang;
    char_u	*dotp;
    FILE	*fd;
    char_u	buf[MAXWLEN];
    পূর্ণাঙ্ক		i;
    time_t	timestamp;
    পূর্ণাঙ্ক		wcount;
    পূর্ণাঙ্ক		wordnr;
    garray_T	ga;
    পূর্ণাঙ্ক		c;

    // Do this for all languages that support sound folding.
    প্রতি (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	যদি (slang->sl_sugtime != 0 && !slang->sl_sugloaded)
	{
	    // Change ".spl" to ".sug" and open the file.  When the file isn't
	    // found silently skip it.  Do set "sl_sugloaded" so that we
	    // don't try again and again.
	    slang->sl_sugloaded = TRUE;

	    dotp = vim_strrchr(slang->sl_fname, '.');
	    যদি (dotp == রিক্ত || fnamecmp(dotp, ".spl") != 0)
		অনুবর্ত;
	    STRCPY(dotp, ".sug");
	    fd = mch_fopen((বর্ণ *)slang->sl_fname, "r");
	    যদি (fd == রিক্ত)
		গচ্ছ nextone;

	    /*
	     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
	     */
	    প্রতি (i = 0; i < VIMSUGMAGICL; ++i)
		buf[i] = (c = getc(fd)) == EOF ? 0 : c;	// <fileID>
	    যদি (STRNCMP(buf, VIMSUGMAGIC, VIMSUGMAGICL) != 0)
	    {
		semsg(_(e_this_does_not_look_like_sug_file_str),
							     slang->sl_fname);
		গচ্ছ nextone;
	    }
	    c = getc(fd);				// <versionnr>
	    যদি (c < VIMSUGVERSION)
	    {
		semsg(_(e_old_sug_file_needs_to_be_updated_str),
							     slang->sl_fname);
		গচ্ছ nextone;
	    }
	    অন্যথা যদি (c > VIMSUGVERSION)
	    {
		semsg(_(e_sug_file_is_for_newer_version_of_vim_str),
							     slang->sl_fname);
		গচ্ছ nextone;
	    }

	    // Check the timestamp, it must be exactly the same as the one in
	    // the .spl file.  Otherwise the word numbers won't match.
	    timestamp = get8ctime(fd);			// <timestamp>
	    যদি (timestamp != slang->sl_sugtime)
	    {
		semsg(_(e_sug_file_doesnt_match_spl_file_str),
							     slang->sl_fname);
		গচ্ছ nextone;
	    }

	    /*
	     * <SUGWORDTREE>: <wordtree>
	     * Read the trie with the soundfolded words.
	     */
	    যদি (spell_read_বৃক্ষ(fd, &slang->sl_sbyts, &slang->sl_sbyts_len,
			&slang->sl_sidxs, FALSE, 0) != 0)
	    {
someerror:
		semsg(_(e_error_while_reading_sug_file_str),
							     slang->sl_fname);
		slang_clear_sug(slang);
		গচ্ছ nextone;
	    }

	    /*
	     * <SUGTABLE>: <sugwcount> <sugline> ...
	     *
	     * Read the table with word numbers.  We use a file buffer for
	     * this, because it's so much like a file with lines.  Makes it
	     * possible to swap the info and save on memory use.
	     */
	    slang->sl_sugbuf = open_spellbuf();
	    যদি (slang->sl_sugbuf == রিক্ত)
		গচ্ছ someerror;
							    // <sugwcount>
	    wcount = get4c(fd);
	    যদি (wcount < 0)
		গচ্ছ someerror;

	    // Read all the wordnr lists into the buffer, one NUL terminated
	    // list per line.
	    ga_init2(&ga, 1, 100);
	    প্রতি (wordnr = 0; wordnr < wcount; ++wordnr)
	    {
		ga.ga_len = 0;
		প্রতি (;;)
		{
		    c = getc(fd);			    // <sugline>
		    যদি (c < 0 || ga_grow(&ga, 1) == FAIL)
			গচ্ছ someerror;
		    ((char_u *)ga.ga_data)[ga.ga_len++] = c;
		    যদি (c == NUL)
			ভঙ্গ;
		}
		যদি (ml_append_buf(slang->sl_sugbuf, (linenr_T)wordnr,
					 ga.ga_data, ga.ga_len, TRUE) == FAIL)
		    গচ্ছ someerror;
	    }
	    ga_clear(&ga);

	    /*
	     * Need to put word counts in the word tries, so that we can find
	     * a word by its number.
	     */
	    বৃক্ষ_গুনতি_শব্দগুলি(slang->sl_fbyts, slang->sl_fbyts_len,
		    slang->sl_fidxs);
	    বৃক্ষ_গুনতি_শব্দগুলি(slang->sl_sbyts, slang->sl_sbyts_len,
		    slang->sl_sidxs);

nextone:
	    যদি (fd != রিক্ত)
		fclose(fd);
	    STRCPY(dotp, ".spl");
	}
    }
}


/*
 * Read a length field from "fd" in "cnt_bytes" bytes.
 * Allocate memory, read the string into it and add a NUL at the end.
 * Returns NULL when the count is zero.
 * Sets "*cntp" to SP_*ERROR when there is an error, length of the result
 * otherwise.
 */
    স্থির char_u *
read_cnt_string(FILE *fd, পূর্ণাঙ্ক cnt_bytes, পূর্ণাঙ্ক *cntp)
{
    পূর্ণাঙ্ক		cnt = 0;
    পূর্ণাঙ্ক		i;
    char_u	*str;

    // read the length bytes, MSB first
    প্রতি (i = 0; i < cnt_bytes; ++i)
    {
	পূর্ণাঙ্ক c = getc(fd);

	যদি (c == EOF)
	{
	    *cntp = SP_TRUNCERROR;
	    প্রত্যাগম রিক্ত;
	}
	cnt = (cnt << 8) + (unsigned)c;
    }
    *cntp = cnt;
    যদি (cnt == 0)
	প্রত্যাগম রিক্ত;	    // nothing to read, return NULL

    str = read_string(fd, cnt);
    যদি (str == রিক্ত)
	*cntp = SP_OTHERERROR;
    প্রত্যাগম str;
}

/*
 * Read SN_REGION: <regionname> ...
 * Return SP_*ERROR flags.
 */
    স্থির পূর্ণাঙ্ক
read_region_section(FILE *fd, slang_T *lp, পূর্ণাঙ্ক len)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		c = 0;

    যদি (len > MAXREGIONS * 2)
	প্রত্যাগম SP_FORMERROR;
    প্রতি (i = 0; i < len; ++i)
	lp->sl_regions[i] = (c = getc(fd)) == EOF ? 0 : c; // <regionname>
    lp->sl_regions[len] = NUL;
    প্রত্যাগম c == EOF ? SP_TRUNCERROR : 0;
}

/*
 * Read SN_CHARFLAGS section: <charflagslen> <charflags>
 *				<folcharslen> <folchars>
 * Return SP_*ERROR flags.
 */
    স্থির পূর্ণাঙ্ক
read_charflags_section(FILE *fd)
{
    char_u	*flags;
    char_u	*fol;
    পূর্ণাঙ্ক		flagslen, follen;

    // <charflagslen> <charflags>
    flags = read_cnt_string(fd, 1, &flagslen);
    যদি (flagslen < 0)
	প্রত্যাগম flagslen;

    // <folcharslen> <folchars>
    fol = read_cnt_string(fd, 2, &follen);
    যদি (follen < 0)
    {
	vim_free(flags);
	প্রত্যাগম follen;
    }

    // Set the word-char flags and fill SPELL_ISUPPER() table.
    যদি (flags != রিক্ত && fol != রিক্ত)
	set_spell_charflags(flags, flagslen, fol);

    vim_free(flags);
    vim_free(fol);

    // When <charflagslen> is zero then <fcharlen> must also be zero.
    যদি ((flags == রিক্ত) != (fol == রিক্ত))
	প্রত্যাগম SP_FORMERROR;
    প্রত্যাগম 0;
}

/*
 * Read SN_PREFCOND section.
 * Return SP_*ERROR flags.
 */
    স্থির পূর্ণাঙ্ক
read_prefcond_section(FILE *fd, slang_T *lp)
{
    পূর্ণাঙ্ক		cnt;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		c;
    char_u	*p;
    char_u	buf[MAXWLEN + 1];

    // <prefcondcnt> <prefcond> ...
    cnt = get2c(fd);					// <prefcondcnt>
    যদি (cnt <= 0)
	প্রত্যাগম SP_FORMERROR;

    lp->sl_prefprog = ALLOC_CLEAR_MULT(regprog_T *, cnt);
    যদি (lp->sl_prefprog == রিক্ত)
	প্রত্যাগম SP_OTHERERROR;
    lp->sl_prefixcnt = cnt;

    প্রতি (i = 0; i < cnt; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	n = getc(fd);					// <condlen>
	যদি (n < 0 || n >= MAXWLEN)
	    প্রত্যাগম SP_FORMERROR;

	// When <condlen> is zero we have an empty condition.  Otherwise
	// compile the regexp program used to check for the condition.
	যদি (n > 0)
	{
	    buf[0] = '^';	    // always match at one position only
	    p = buf + 1;
	    যাবত্ (n-- > 0)
		*p++ = (c = getc(fd)) == EOF ? 0 : c;	    // <condstr>
	    যদি (c == EOF)
		ভঙ্গ;
	    *p = NUL;
	    lp->sl_prefprog[i] = vim_regcomp(buf, RE_MAGIC + RE_STRING);
	}
    }
    প্রত্যাগম 0;
}

/*
 * Read REP or REPSAL items section from "fd": <repcount> <rep> ...
 * Return SP_*ERROR flags.
 */
    স্থির পূর্ণাঙ্ক
read_rep_section(FILE *fd, garray_T *gap, short *first)
{
    পূর্ণাঙ্ক		cnt;
    fromto_T	*ftp;
    পূর্ণাঙ্ক		i;

    cnt = get2c(fd);					// <repcount>
    যদি (cnt < 0)
	প্রত্যাগম SP_TRUNCERROR;

    যদি (ga_grow(gap, cnt) == FAIL)
	প্রত্যাগম SP_OTHERERROR;

    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
    প্রতি (; gap->ga_len < cnt; ++gap->ga_len)
    {
	ftp = &((fromto_T *)gap->ga_data)[gap->ga_len];
	ftp->ft_from = read_cnt_string(fd, 1, &i);
	যদি (i < 0)
	    প্রত্যাগম i;
	যদি (i == 0)
	    প্রত্যাগম SP_FORMERROR;
	ftp->ft_to = read_cnt_string(fd, 1, &i);
	যদি (i <= 0)
	{
	    vim_free(ftp->ft_from);
	    যদি (i < 0)
		প্রত্যাগম i;
	    প্রত্যাগম SP_FORMERROR;
	}
    }

    // Fill the first-index table.
    প্রতি (i = 0; i < 256; ++i)
	first[i] = -1;
    প্রতি (i = 0; i < gap->ga_len; ++i)
    {
	ftp = &((fromto_T *)gap->ga_data)[i];
	যদি (first[*ftp->ft_from] == -1)
	    first[*ftp->ft_from] = i;
    }
    প্রত্যাগম 0;
}

/*
 * Read SN_SAL section: <salflags> <salcount> <sal> ...
 * Return SP_*ERROR flags.
 */
    স্থির পূর্ণাঙ্ক
read_sal_section(FILE *fd, slang_T *slang)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		cnt;
    garray_T	*gap;
    salitem_T	*smp;
    পূর্ণাঙ্ক		ccnt;
    char_u	*p;

    slang->sl_sofo = FALSE;

    i = getc(fd);				// <salflags>
    যদি (i & SAL_F0LLOWUP)
	slang->sl_followup = TRUE;
    যদি (i & SAL_COLLAPSE)
	slang->sl_collapse = TRUE;
    যদি (i & SAL_REM_ACCENTS)
	slang->sl_rem_accents = TRUE;

    cnt = get2c(fd);				// <salcount>
    যদি (cnt < 0)
	প্রত্যাগম SP_TRUNCERROR;

    gap = &slang->sl_sal;
    ga_init2(gap, পরিমাণ(salitem_T), 10);
    যদি (ga_grow(gap, cnt + 1) == FAIL)
	প্রত্যাগম SP_OTHERERROR;

    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
    প্রতি (; gap->ga_len < cnt; ++gap->ga_len)
    {
	পূর্ণাঙ্ক	c = NUL;

	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	ccnt = getc(fd);			// <salfromlen>
	যদি (ccnt < 0)
	    প্রত্যাগম SP_TRUNCERROR;
	যদি ((p = alloc(ccnt + 2)) == রিক্ত)
	    প্রত্যাগম SP_OTHERERROR;
	smp->sm_lead = p;

	// Read up to the first special char into sm_lead.
	প্রতি (i = 0; i < ccnt; ++i)
	{
	    c = getc(fd);			// <salfrom>
	    যদি (vim_strchr((char_u *)"0123456789(-<^$", c) != রিক্ত)
		ভঙ্গ;
	    *p++ = c;
	}
	smp->sm_leadlen = (পূর্ণাঙ্ক)(p - smp->sm_lead);
	*p++ = NUL;

	// Put (abc) chars in sm_oneof, if any.
	যদি (c == '(')
	{
	    smp->sm_oneof = p;
	    প্রতি (++i; i < ccnt; ++i)
	    {
		c = getc(fd);			// <salfrom>
		যদি (c == ')')
		    ভঙ্গ;
		*p++ = c;
	    }
	    *p++ = NUL;
	    যদি (++i < ccnt)
		c = getc(fd);
	}
	অন্যথা
	    smp->sm_oneof = রিক্ত;

	// Any following chars go in sm_rules.
	smp->sm_rules = p;
	যদি (i < ccnt)
	    // store the char we got while checking for end of sm_lead
	    *p++ = c;
	প্রতি (++i; i < ccnt; ++i)
	    *p++ = (c = getc(fd)) == EOF ? 0 : c;	// <salfrom>
	*p++ = NUL;

	// <saltolen> <salto>
	smp->sm_to = read_cnt_string(fd, 1, &ccnt);
	যদি (ccnt < 0)
	{
	    vim_free(smp->sm_lead);
	    প্রত্যাগম ccnt;
	}

	যদি (has_mbyte)
	{
	    // convert the multi-byte strings to wide char strings
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = mb_charlen(smp->sm_lead);
	    যদি (smp->sm_oneof == রিক্ত)
		smp->sm_oneof_w = রিক্ত;
	    অন্যথা
		smp->sm_oneof_w = mb_str2wide(smp->sm_oneof);
	    যদি (smp->sm_to == রিক্ত)
		smp->sm_to_w = রিক্ত;
	    অন্যথা
		smp->sm_to_w = mb_str2wide(smp->sm_to);
	    যদি (smp->sm_lead_w == রিক্ত
		    || (smp->sm_oneof_w == রিক্ত && smp->sm_oneof != রিক্ত)
		    || (smp->sm_to_w == রিক্ত && smp->sm_to != রিক্ত))
	    {
		vim_free(smp->sm_lead);
		vim_free(smp->sm_to);
		vim_free(smp->sm_lead_w);
		vim_free(smp->sm_oneof_w);
		vim_free(smp->sm_to_w);
		প্রত্যাগম SP_OTHERERROR;
	    }
	}
    }

    যদি (gap->ga_len > 0)
    {
	// Add one extra entry to mark the end with an empty sm_lead.  Avoids
	// that we need to check the index every time.
	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	যদি ((p = alloc(1)) == রিক্ত)
	    প্রত্যাগম SP_OTHERERROR;
	p[0] = NUL;
	smp->sm_lead = p;
	smp->sm_leadlen = 0;
	smp->sm_oneof = রিক্ত;
	smp->sm_rules = p;
	smp->sm_to = রিক্ত;
	যদি (has_mbyte)
	{
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = 0;
	    smp->sm_oneof_w = রিক্ত;
	    smp->sm_to_w = রিক্ত;
	}
	++gap->ga_len;
    }

    // Fill the first-index table.
    set_sal_first(slang);

    প্রত্যাগম 0;
}

/*
 * Read SN_WORDS: <word> ...
 * Return SP_*ERROR flags.
 */
    স্থির পূর্ণাঙ্ক
read_শব্দগুলি_section(FILE *fd, slang_T *lp, পূর্ণাঙ্ক len)
{
    পূর্ণাঙ্ক		done = 0;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		c;
    char_u	word[MAXWLEN];

    যাবত্ (done < len)
    {
	// Read one word at a time.
	প্রতি (i = 0; ; ++i)
	{
	    c = getc(fd);
	    যদি (c == EOF)
		প্রত্যাগম SP_TRUNCERROR;
	    word[i] = c;
	    যদি (word[i] == NUL)
		ভঙ্গ;
	    যদি (i == MAXWLEN - 1)
		প্রত্যাগম SP_FORMERROR;
	}

	// Init the count to 10.
	গুনতি_common_word(lp, word, -1, 10);
	done += i + 1;
    }
    প্রত্যাগম 0;
}

/*
 * SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
 * Return SP_*ERROR flags.
 */
    স্থির পূর্ণাঙ্ক
read_sofo_section(FILE *fd, slang_T *slang)
{
    পূর্ণাঙ্ক		cnt;
    char_u	*from, *to;
    পূর্ণাঙ্ক		res;

    // <sofofromlen> <sofofrom>
    from = read_cnt_string(fd, 2, &cnt);
    যদি (cnt < 0)
	প্রত্যাগম cnt;

    // <sofotolen> <sofoto>
    to = read_cnt_string(fd, 2, &cnt);
    যদি (cnt < 0)
    {
	vim_free(from);
	প্রত্যাগম cnt;
    }

    // Store the info in slang->sl_sal and/or slang->sl_sal_first.
    যদি (from != রিক্ত && to != রিক্ত)
	res = set_sofo(slang, from, to);
    অন্যথা যদি (from != রিক্ত || to != রিক্ত)
	res = SP_FORMERROR;    // only one of two strings is an error
    অন্যথা
	res = 0;

    vim_free(from);
    vim_free(to);
    প্রত্যাগম res;
}

/*
 * Read the compound section from the .spl file:
 *	<compmax> <compminlen> <compsylmax> <compoptions> <compflags>
 * Returns SP_*ERROR flags.
 */
    স্থির পূর্ণাঙ্ক
read_compound(FILE *fd, slang_T *slang, পূর্ণাঙ্ক len)
{
    পূর্ণাঙ্ক		todo = len;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		atstart;
    char_u	*pat;
    char_u	*pp;
    char_u	*cp;
    char_u	*ap;
    char_u	*crp;
    পূর্ণাঙ্ক		cnt;
    garray_T	*gap;
    size_t	patsize;
    size_t	flagsize;

    যদি (todo < 2)
	প্রত্যাগম SP_FORMERROR;	// need at least two bytes

    --todo;
    c = getc(fd);					// <compmax>
    যদি (c < 2)
	c = MAXWLEN;
    slang->sl_compmax = c;

    --todo;
    c = getc(fd);					// <compminlen>
    যদি (c < 1)
	c = 0;
    slang->sl_compminlen = c;

    --todo;
    c = getc(fd);					// <compsylmax>
    যদি (c < 1)
	c = MAXWLEN;
    slang->sl_compsylmax = c;

    c = getc(fd);					// <compoptions>
    যদি (c != 0)
	ungetc(c, fd);	    // be backwards compatible with Vim 7.0b
    অন্যথা
    {
	--todo;
	c = getc(fd);	    // only use the lower byte for now
	--todo;
	slang->sl_compoptions = c;

	gap = &slang->sl_comppat;
	c = get2c(fd);					// <comppatcount>
	যদি (c < 0)
	    প্রত্যাগম SP_TRUNCERROR;
	todo -= 2;
	ga_init2(gap, পরিমাণ(char_u *), c);
	যদি (ga_grow(gap, c) == OK)
	    যাবত্ (--c >= 0)
	    {
		((char_u **)(gap->ga_data))[gap->ga_len++] =
						  read_cnt_string(fd, 1, &cnt);
					    // <comppatlen> <comppattext>
		যদি (cnt < 0)
		    প্রত্যাগম cnt;
		todo -= cnt + 1;
	    }
    }
    যদি (todo < 0)
	প্রত্যাগম SP_FORMERROR;

    // Turn the COMPOUNDRULE items into a regexp pattern:
    // "a[bc]/a*b+" -> "^\(a[bc]\|a*b\+\)$".
    // Inserting backslashes may double the length, "^\(\)$<Nul>" is 7 bytes.
    // Conversion to utf-8 may double the size.
    যদি ((size_t)todo > COMPOUND_MAX_LEN)
	প্রত্যাগম SP_FORMERROR;
    patsize = (size_t)todo * 2 + 7;
    যদি (enc_utf8)
	patsize += (size_t)todo * 2;
    flagsize = (size_t)todo + 1;
    pat = alloc(patsize);
    যদি (pat == রিক্ত)
	প্রত্যাগম SP_OTHERERROR;

    // We also need a list of all flags that can appear at the start and one
    // for all flags.
    cp = alloc(flagsize);
    যদি (cp == রিক্ত)
    {
	vim_free(pat);
	প্রত্যাগম SP_OTHERERROR;
    }
    slang->sl_compstartflags = cp;
    *cp = NUL;

    ap = alloc(flagsize);
    যদি (ap == রিক্ত)
    {
	vim_free(pat);
	প্রত্যাগম SP_OTHERERROR;
    }
    slang->sl_compallflags = ap;
    *ap = NUL;

    // And a list of all patterns in their original form, for checking whether
    // compounding may work in match_compoundrule().  This is freed when we
    // encounter a wildcard, the check doesn't work then.
    crp = alloc(flagsize);
    slang->sl_comprules = crp;

    pp = pat;
    *pp++ = '^';
    *pp++ = '\\';
    *pp++ = '(';

    atstart = 1;
    যাবত্ (todo-- > 0)
    {
	c = getc(fd);					// <compflags>
	যদি (c == EOF)
	{
	    vim_free(pat);
	    প্রত্যাগম SP_TRUNCERROR;
	}

	// Add all flags to "sl_compallflags".
	যদি (vim_strchr((char_u *)"?*+[]/", c) == রিক্ত
		&& !byte_in_str(slang->sl_compallflags, c))
	{
	    *ap++ = c;
	    *ap = NUL;
	}

	যদি (atstart != 0)
	{
	    // At start of item: copy flags to "sl_compstartflags".  For a
	    // [abc] item set "atstart" to 2 and copy up to the ']'.
	    যদি (c == '[')
		atstart = 2;
	    অন্যথা যদি (c == ']')
		atstart = 0;
	    অন্যথা
	    {
		যদি (!byte_in_str(slang->sl_compstartflags, c))
		{
		    *cp++ = c;
		    *cp = NUL;
		}
		যদি (atstart == 1)
		    atstart = 0;
	    }
	}

	// Copy flag to "sl_comprules", unless we run into a wildcard.
	যদি (crp != রিক্ত)
	{
	    যদি (c == '?' || c == '+' || c == '*')
	    {
		VIM_CLEAR(slang->sl_comprules);
		crp = রিক্ত;
	    }
	    অন্যথা
		*crp++ = c;
	}

	যদি (c == '/')	    // slash separates two items
	{
	    *pp++ = '\\';
	    *pp++ = '|';
	    atstart = 1;
	}
	অন্যথা		    // normal char, "[abc]" and '*' are copied as-is
	{
	    যদি (c == '?' || c == '+' || c == '~')
		*pp++ = '\\';	    // "a?" becomes "a\?", "a+" becomes "a\+"
	    যদি (enc_utf8)
		pp += mb_char2bytes(c, pp);
	    অন্যথা
		*pp++ = c;
	}
    }

    *pp++ = '\\';
    *pp++ = ')';
    *pp++ = '$';
    *pp = NUL;

    যদি (crp != রিক্ত)
	*crp = NUL;

    slang->sl_compprog = vim_regcomp(pat, RE_MAGIC + RE_STRING + RE_STRICT);
    vim_free(pat);
    যদি (slang->sl_compprog == রিক্ত)
	প্রত্যাগম SP_FORMERROR;

    প্রত্যাগম 0;
}

/*
 * Set the SOFOFROM and SOFOTO items in language "lp".
 * Returns SP_*ERROR flags when there is something wrong.
 */
    স্থির পূর্ণাঙ্ক
set_sofo(slang_T *lp, char_u *from, char_u *to)
{
    পূর্ণাঙ্ক		i;

    garray_T	*gap;
    char_u	*s;
    char_u	*p;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		*inp;

    যদি (has_mbyte)
    {
	free_sal_বস্তুসমূহ(&lp->sl_sal);

	// Use "sl_sal" as an array with 256 pointers to a list of wide
	// characters.  The index is the low byte of the character.
	// The list contains from-to pairs with a terminating NUL.
	// sl_sal_first[] is used for latin1 "from" characters.
	gap = &lp->sl_sal;
	ga_init2(gap, পরিমাণ(পূর্ণাঙ্ক *), 1);
	যদি (ga_grow(gap, 256) == FAIL)
	    প্রত্যাগম SP_OTHERERROR;
	vim_memset(gap->ga_data, 0, পরিমাণ(পূর্ণাঙ্ক *) * 256);
	gap->ga_len = 256;
	lp->sl_sofo = TRUE;

	// First count the number of items for each list.  Temporarily use
	// sl_sal_first[] for this.  Reset it first: a preceding SN_SAL section
	// may have set the entries to -1 via set_sal_first().
	vim_memset(lp->sl_sal_first, 0, পরিমাণ(salfirst_T) * 256);
	প্রতি (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    MB_CPTR_ADV(s);
	    যদি (c >= 256)
		++lp->sl_sal_first[c & 0xff];
	}
	যদি (*p != NUL || *s != NUL)	    // lengths differ
	    প্রত্যাগম SP_FORMERROR;

	// Allocate the lists.
	প্রতি (i = 0; i < 256; ++i)
	    যদি (lp->sl_sal_first[i] > 0)
	    {
		p = alloc(পরিমাণ(পূর্ণাঙ্ক) * (lp->sl_sal_first[i] * 2 + 1));
		যদি (p == রিক্ত)
		    প্রত্যাগম SP_OTHERERROR;
		((পূর্ণাঙ্ক **)gap->ga_data)[i] = (পূর্ণাঙ্ক *)p;
		*(পূর্ণাঙ্ক *)p = 0;
	    }

	// Put the characters up to 255 in sl_sal_first[] the rest in a sl_sal
	// list.
	vim_memset(lp->sl_sal_first, 0, পরিমাণ(salfirst_T) * 256);
	প্রতি (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    i = mb_cptr2char_adv(&s);
	    যদি (c >= 256)
	    {
		// Append the from-to chars at the end of the list with
		// the low byte.
		inp = ((পূর্ণাঙ্ক **)gap->ga_data)[c & 0xff];
		যাবত্ (*inp != 0)
		    ++inp;
		*inp++ = c;		// from char
		*inp++ = i;		// to char
		*inp++ = NUL;		// NUL at the end
	    }
	    অন্যথা
		// mapping byte to char is done in sl_sal_first[]
		lp->sl_sal_first[c] = i;
	}
    }
    অন্যথা
    {
	// mapping bytes to bytes is done in sl_sal_first[]
	যদি (STRLEN(from) != STRLEN(to))
	    প্রত্যাগম SP_FORMERROR;

	প্রতি (i = 0; to[i] != NUL; ++i)
	    lp->sl_sal_first[from[i]] = to[i];
	lp->sl_sal.ga_len = 1;		// indicates we have soundfolding
	lp->sl_sofo = TRUE;
    }

    প্রত্যাগম 0;
}

/*
 * Fill the first-index table for "lp".
 */
    স্থির শূন্য
set_sal_first(slang_T *lp)
{
    salfirst_T	*sfirst;
    পূর্ণাঙ্ক		i;
    salitem_T	*smp;
    পূর্ণাঙ্ক		c;
    garray_T	*gap = &lp->sl_sal;

    sfirst = lp->sl_sal_first;
    প্রতি (i = 0; i < 256; ++i)
	sfirst[i] = -1;
    smp = (salitem_T *)gap->ga_data;
    প্রতি (i = 0; i < gap->ga_len; ++i)
    {
	যদি (has_mbyte)
	    // Use the lowest byte of the first character.  For latin1 it's
	    // the character, for other encodings it should differ for most
	    // characters.
	    c = *smp[i].sm_lead_w & 0xff;
	অন্যথা
	    c = *smp[i].sm_lead;
	যদি (sfirst[c] == -1)
	{
	    sfirst[c] = i;
	    যদি (has_mbyte)
	    {
		পূর্ণাঙ্ক		n;

		// Make sure all entries with this byte are following each
		// other.  Move the ones that are in the wrong position.  Do
		// keep the same ordering!
		যাবত্ (i + 1 < gap->ga_len
				       && (*smp[i + 1].sm_lead_w & 0xff) == c)
		    // Skip over entry with same index byte.
		    ++i;

		প্রতি (n = 1; i + n < gap->ga_len; ++n)
		    যদি ((*smp[i + n].sm_lead_w & 0xff) == c)
		    {
			salitem_T  tsal;

			// Move entry with same index byte after the entries
			// we already found.
			++i;
			--n;
			tsal = smp[i + n];
			mch_memmove(smp + i + 1, smp + i,
						       পরিমাণ(salitem_T) * n);
			smp[i] = tsal;
		    }
	    }
	}
    }
}

/*
 * Turn a multi-byte string into a wide character string.
 * Return it in allocated memory (NULL for out-of-memory)
 */
    স্থির পূর্ণাঙ্ক *
mb_str2wide(char_u *s)
{
    পূর্ণাঙ্ক		*res;
    char_u	*p;
    পূর্ণাঙ্ক		i = 0;

    res = ALLOC_MULT(পূর্ণাঙ্ক, mb_charlen(s) + 1);
    যদি (res == রিক্ত)
	প্রত্যাগম রিক্ত;

    প্রতি (p = s; *p != NUL; )
	res[i++] = mb_ptr2char_adv(&p);
    res[i] = NUL;
    প্রত্যাগম res;
}

/*
 * Read a tree from the .spl or .sug file.
 * Allocates the memory and stores pointers in "bytsp" and "idxsp".
 * This is skipped when the tree has zero length.
 * Returns zero when OK, SP_ value for an error.
 */
    স্থির পূর্ণাঙ্ক
spell_read_বৃক্ষ(
    FILE	*fd,
    char_u	**bytsp,
    long	*bytsp_len,
    idx_T	**idxsp,
    পূর্ণাঙ্ক		prefixtree,	// TRUE for the prefix tree
    পূর্ণাঙ্ক		prefixcnt)	// when "prefixtree" is TRUE: prefix count
{
    long	len;
    পূর্ণাঙ্ক		idx;
    char_u	*bp;
    idx_T	*ip;

    // The tree size was computed when writing the file, so that we can
    // allocate it as one long block. <nodecount>
    len = get4c(fd);
    যদি (len < 0)
	প্রত্যাগম SP_TRUNCERROR;
    যদি ((size_t)len > SIZE_MAX / পরিমাণ(পূর্ণাঙ্ক))
	// Invalid length, multiply with sizeof(int) would overflow.
	প্রত্যাগম SP_FORMERROR;
    যদি (len <= 0)
	প্রত্যাগম 0;

    // Allocate the byte array.  Zero-initialize so that any position the
    // tree does not visit reads as 0; a stray BY_INDEX shared reference
    // into such a slot then behaves as end-of-word in spellsuggest()
    // instead of consuming an arbitrary heap byte as a siblingcount.
    bp = alloc_clear(len);
    যদি (bp == রিক্ত)
	প্রত্যাগম SP_OTHERERROR;
    *bytsp = bp;
    যদি (bytsp_len != রিক্ত)
	*bytsp_len = len;

    // Allocate the index array.
    ip = lalloc_clear(len * পরিমাণ(পূর্ণাঙ্ক), TRUE);
    যদি (ip == রিক্ত)
	প্রত্যাগম SP_OTHERERROR;
    *idxsp = ip;

    // Recursively read the tree and store it in the array.
    idx = read_বৃক্ষ_গ্রন্থি(fd, bp, ip, len, 0, prefixtree, prefixcnt, 0);
    যদি (idx < 0)
	প্রত্যাগম idx;
    যদি (idx != len)
	প্রত্যাগম SP_FORMERROR;
    প্রত্যাগম 0;
}

/*
 * Read one row of siblings from the spell file and store it in the byte array
 * "byts" and index array "idxs".  Recursively read the children.
 *
 * NOTE: The code here must match put_node()!
 *
 * Returns the index (>= 0) following the siblings.
 * Returns SP_TRUNCERROR if the file is shorter than expected.
 * Returns SP_FORMERROR if there is a format error.
 */
    স্থির idx_T
read_বৃক্ষ_গ্রন্থি(
    FILE	*fd,
    char_u	*byts,
    idx_T	*idxs,
    পূর্ণাঙ্ক		maxidx,		    // size of arrays
    idx_T	startidx,	    // current index in "byts" and "idxs"
    পূর্ণাঙ্ক		prefixtree,	    // TRUE for reading PREFIXTREE
    পূর্ণাঙ্ক		maxprefcondnr,	    // maximum for <prefcondnr>
    পূর্ণাঙ্ক		depth)		    // recursion level
{
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		n;
    idx_T	idx = startidx;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		c2;
#define SHARED_MASK	0x8000000

    // Bail out on a crafted .spl whose tree recurses beyond the maximum
    // word length: each tree level corresponds to one byte of a word, so
    // any well-formed file has depth <= MAXWLEN.
    যদি (depth > MAXWLEN)
	প্রত্যাগম SP_FORMERROR;

    len = getc(fd);					// <siblingcount>
    যদি (len <= 0)
	প্রত্যাগম SP_TRUNCERROR;

    যদি (len >= maxidx - startidx)
	প্রত্যাগম SP_FORMERROR;
    byts[idx++] = len;

    // Read the byte values, flag/region bytes and shared indexes.
    প্রতি (i = 1; i <= len; ++i)
    {
	c = getc(fd);					// <byte>
	যদি (c < 0)
	    প্রত্যাগম SP_TRUNCERROR;
	যদি (c <= BY_SPECIAL)
	{
	    যদি (c == BY_NOFLAGS && !prefixtree)
	    {
		// No flags, all regions.
		idxs[idx] = 0;
		c = 0;
	    }
	    অন্যথা যদি (c != BY_INDEX)
	    {
		যদি (prefixtree)
		{
		    // Read the optional pflags byte, the prefix ID and the
		    // condition nr.  In idxs[] store the prefix ID in the low
		    // byte, the condition index shifted up 8 bits, the flags
		    // shifted up 24 bits.
		    যদি (c == BY_FLAGS)
		    {
			n = getc(fd);			// <pflags>
			যদি (n < 0)
			    প্রত্যাগম SP_TRUNCERROR;
			c = n << 24;
		    }
		    অন্যথা
			c = 0;

		    n = getc(fd);			// <affixID>
		    যদি (n < 0)
			প্রত্যাগম SP_TRUNCERROR;
		    c |= n;

		    n = get2c(fd);			// <prefcondnr>
		    যদি (n < 0 || n >= maxprefcondnr)
			প্রত্যাগম SP_FORMERROR;
		    c |= (n << 8);
		}
		অন্যথা // c must be BY_FLAGS or BY_FLAGS2
		{
		    // Read flags and optional region and prefix ID.  In
		    // idxs[] the flags go in the low two bytes, region above
		    // that and prefix ID above the region.
		    c2 = c;
		    c = getc(fd);			// <flags>
		    যদি (c < 0)
			প্রত্যাগম SP_TRUNCERROR;
		    যদি (c2 == BY_FLAGS2)
		    {
			n = getc(fd);			// <flags2>
			যদি (n < 0)
			    প্রত্যাগম SP_TRUNCERROR;
			c = (n << 8) + c;
		    }
		    যদি (c & WF_REGION)
		    {
			n = getc(fd);			// <region>
			যদি (n < 0)
			    প্রত্যাগম SP_TRUNCERROR;
			c = (n << 16) + c;
		    }
		    যদি (c & WF_AFX)
		    {
			n = getc(fd);			// <affixID>
			যদি (n < 0)
			    প্রত্যাগম SP_TRUNCERROR;
			c = (n << 24) + c;
		    }
		}

		idxs[idx] = c;
		c = 0;
	    }
	    অন্যথা // c == BY_INDEX
	    {
							// <nodeidx>
		n = get3c(fd);
		যদি (n < 0 || n >= maxidx)
		    প্রত্যাগম SP_FORMERROR;
		idxs[idx] = n + SHARED_MASK;
		c = getc(fd);				// <xbyte>
	    }
	}
	byts[idx++] = c;
    }

    // Recursively read the children for non-shared siblings.
    // Skip the end-of-word ones (zero byte value) and the shared ones (and
    // remove SHARED_MASK)
    প্রতি (i = 1; i <= len; ++i)
	যদি (byts[startidx + i] != 0)
	{
	    যদি (idxs[startidx + i] & SHARED_MASK)
		idxs[startidx + i] &= ~SHARED_MASK;
	    অন্যথা
	    {
		idxs[startidx + i] = idx;
		idx = read_বৃক্ষ_গ্রন্থি(fd, byts, idxs, maxidx, idx,
					prefixtree, maxprefcondnr, depth + 1);
		যদি (idx < 0)
		    ভঙ্গ;
	    }
	}

    প্রত্যাগম idx;
}

/*
 * Reload the spell file "fname" if it's loaded.
 */
    স্থির শূন্য
spell_reload_one(
    char_u	*fname,
    পূর্ণাঙ্ক		added_word)	// invoked through "zg"
{
    slang_T	*slang;
    পূর্ণাঙ্ক		didit = FALSE;

    FOR_ALL_SPELL_LANGS(slang)
    {
	যদি (fullpathcmp(fname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	{
	    slang_clear(slang);
	    যদি (spell_load_file(fname, রিক্ত, slang, FALSE) == রিক্ত)
		// reloading failed, clear the language
		slang_clear(slang);
	    redraw_all_later(UPD_SOME_VALID);
	    didit = TRUE;
	}
    }

    // When "zg" was used and the file wasn't loaded yet, should redo
    // 'spelllang' to load it now.
    যদি (added_word && !didit)
	parse_spelllang(curwin);
}


/*
 * Functions for ":mkspell".
 */

#define MAXLINELEN  500		// Maximum length in bytes of a line in a .aff
				// and .dic file.
/*
 * Main structure to store the contents of a ".aff" file.
 */
প্রকারনির্দেশ সংরচনা afffile_S
{
    char_u	*af_enc;	// "SET", normalized, alloc'ed string or NULL
    পূর্ণাঙ্ক		af_flagtype;	// AFT_CHAR, AFT_LONG, AFT_NUM or AFT_CAPLONG
    unsigned	af_rare;	// RARE ID for rare word
    unsigned	af_keepcase;	// KEEPCASE ID for keep-case word
    unsigned	af_bad;		// BAD ID for banned word
    unsigned	af_needaffix;	// NEEDAFFIX ID
    unsigned	af_circumfix;	// CIRCUMFIX ID
    unsigned	af_needcomp;	// NEEDCOMPOUND ID
    unsigned	af_comproot;	// COMPOUNDROOT ID
    unsigned	af_compforbid;	// COMPOUNDFORBIDFLAG ID
    unsigned	af_comppermit;	// COMPOUNDPERMITFLAG ID
    unsigned	af_nosuggest;	// NOSUGGEST ID
    পূর্ণাঙ্ক		af_pfxpostpone;	// postpone prefixes without chop string and
				// without flags
    পূর্ণাঙ্ক		af_ignoreextra;	// IGNOREEXTRA present
    hashtab_T	af_pref;	// hashtable for prefixes, affheader_T
    hashtab_T	af_suff;	// hashtable for suffixes, affheader_T
    hashtab_T	af_comp;	// hashtable for compound flags, compitem_T
} afffile_T;

#define AFT_CHAR	0	// flags are one character
#define AFT_LONG	1	// flags are two characters
#define AFT_CAPLONG	2	// flags are one or two characters
#define AFT_অঙ্ক		3	// flags are numbers, comma separated

প্রকারনির্দেশ সংরচনা affentry_S affentry_T;
// Affix entry from ".aff" file.  Used for prefixes and suffixes.
সংরচনা affentry_S
{
    affentry_T	*ae_next;	// next affix with same name/number
    char_u	*ae_chop;	// text to chop off basic word (can be NULL)
    char_u	*ae_add;	// text to add to basic word (can be NULL)
    char_u	*ae_flags;	// flags on the affix (can be NULL)
    char_u	*ae_cond;	// condition (NULL for ".")
    regprog_T	*ae_prog;	// regexp program for ae_cond or NULL
    বর্ণ	ae_compforbid;	// COMPOUNDFORBIDFLAG found
    বর্ণ	ae_comppermit;	// COMPOUNDPERMITFLAG found
};

#define AH_চাবি_LEN 17		// 2 x 8 bytes + NUL

// Affix header from ".aff" file.  Used for af_pref and af_suff.
প্রকারনির্দেশ সংরচনা affheader_S
{
    char_u	ah_চাবি[AH_চাবি_LEN]; // key for hashtab == name of affix
    unsigned	ah_flag;	// affix name as number, uses "af_flagtype"
    পূর্ণাঙ্ক		ah_newID;	// prefix ID after renumbering; 0 if not used
    পূর্ণাঙ্ক		ah_combine;	// suffix may combine with prefix
    পূর্ণাঙ্ক		ah_follows;	// another affix block should be following
    affentry_T	*ah_first;	// first affix entry
} affheader_T;

#define HI2AH(hi)   ((affheader_T *)(hi)->hi_চাবি)

// Flag used in compound items.
প্রকারনির্দেশ সংরচনা compitem_S
{
    char_u	ci_চাবি[AH_চাবি_LEN]; // key for hashtab == name of compound
    unsigned	ci_flag;	// affix name as number, uses "af_flagtype"
    পূর্ণাঙ্ক		ci_newID;	// affix ID after renumbering.
} compitem_T;

#define HI2CI(hi)   ((compitem_T *)(hi)->hi_চাবি)

/*
 * Structure that is used to store the items in the word tree.  This avoids
 * the need to keep track of each allocated thing, everything is freed all at
 * once after ":mkspell" is done.
 * Note: "sb_next" must be just before "sb_data" to make sure the alignment of
 * "sb_data" is correct for systems where pointers must be aligned on
 * pointer-size boundaries and sizeof(pointer) > sizeof(int) (e.g., Sparc).
 */
#define  SBLOCKSIZE 16000	// size of sb_data
প্রকারনির্দেশ সংরচনা sblock_S sblock_T;
সংরচনা sblock_S
{
    পূর্ণাঙ্ক		sb_used;	// nr of bytes already in use
    sblock_T	*sb_next;	// next block in list
    char_u	sb_data[1];	// data, actually longer
};

/*
 * A node in the tree.
 */
প্রকারনির্দেশ সংরচনা wordnode_S wordnode_T;
সংরচনা wordnode_S
{
    সঙ্ঘ   // shared to save space
    {
	char_u	hashkey[6];	// the hash key, only used while compressing
	পূর্ণাঙ্ক	index;		// index in written nodes (valid after first
				// round)
    } wn_u1;
    সঙ্ঘ   // shared to save space
    {
	wordnode_T *next;	// next node with same hash key
	wordnode_T *wnode;	// parent node that will write this node
    } wn_u2;
    wordnode_T	*wn_child;	// child (next byte in word)
    wordnode_T  *wn_sibling;	// next sibling (alternate byte in word,
				// always sorted)
    পূর্ণাঙ্ক		wn_refs;	// Nr. of references to this node.  Only
				// relevant for first node in a list of
				// siblings, in following siblings it is
				// always one.
    char_u	wn_byte;	// Byte for this node. NUL for word end

    // Info for when "wn_byte" is NUL.
    // In PREFIXTREE "wn_region" is used for the prefcondnr.
    // In the soundfolded word tree "wn_flags" has the MSW of the wordnr and
    // "wn_region" the LSW of the wordnr.
    char_u	wn_affixID;	// supported/required prefix ID or 0
    short_u	wn_flags;	// WF_ flags
    short	wn_region;	// region mask

#ifdef SPELL_PRINTTREE
    পূর্ণাঙ্ক		wn_nr;		// sequence nr for printing
#endif
};

#define WN_MASK	 0xffff		// mask relevant bits of "wn_flags"

#define HI2WN(hi)    (wordnode_T *)((hi)->hi_চাবি)

/*
 * Info used while reading the spell files.
 */
প্রকারনির্দেশ সংরচনা spellinfo_S
{
    wordnode_T	*si_foldroot;	// tree with case-folded words
    long	si_foldwcount;	// nr of words in si_foldroot

    wordnode_T	*si_keeproot;	// tree with keep-case words
    long	si_keepwcount;	// nr of words in si_keeproot

    wordnode_T	*si_prefroot;	// tree with postponed prefixes

    long	si_sugtree;	// creating the soundfolding trie

    sblock_T	*si_blocks;	// memory blocks used
    long	si_blocks_cnt;	// memory blocks allocated
    পূর্ণাঙ্ক		si_did_emsg;	// TRUE when ran out of memory

    long	si_compress_cnt;    // words to add before lowering
				    // compression limit
    wordnode_T	*si_first_free; // List of nodes that have been freed during
				// compression, linked by "wn_child" field.
    long	si_free_গুনতি;	// number of nodes in si_first_free
#ifdef SPELL_PRINTTREE
    পূর্ণাঙ্ক		si_wordnode_nr;	// sequence nr for nodes
#endif
    buf_T	*si_spellbuf;	// buffer used to store soundfold word table

    পূর্ণাঙ্ক		si_ascii;	// handling only ASCII words
    পূর্ণাঙ্ক		si_add;		// addition file
    পূর্ণাঙ্ক		si_clear_chartab;   // when TRUE clear char tables
    পূর্ণাঙ্ক		si_region;	// region mask
    vimconv_T	si_conv;	// for conversion to 'encoding'
    পূর্ণাঙ্ক		si_memtot;	// runtime memory used
    পূর্ণাঙ্ক		si_verbose;	// verbose messages
    পূর্ণাঙ্ক		si_বার্তা_গুনতি;	// number of words added since last message
    char_u	*si_info;	// info text chars or NULL
    পূর্ণাঙ্ক		si_region_গুনতি; // number of regions supported (1 when there
				 // are no regions)
    char_u	si_region_name[MAXREGIONS * 2 + 1];
				// region names; used only if
				// si_region_count > 1)

    garray_T	si_rep;		// list of fromto_T entries from REP lines
    garray_T	si_repsal;	// list of fromto_T entries from REPSAL lines
    garray_T	si_sal;		// list of fromto_T entries from SAL lines
    char_u	*si_sofofr;	// SOFOFROM text
    char_u	*si_sofoto;	// SOFOTO text
    পূর্ণাঙ্ক		si_nosugfile;	// NOSUGFILE item found
    পূর্ণাঙ্ক		si_nosplitsugs;	// NOSPLITSUGS item found
    পূর্ণাঙ্ক		si_nocompoundsugs; // NOCOMPOUNDSUGS item found
    পূর্ণাঙ্ক		si_followup;	// soundsalike: ?
    পূর্ণাঙ্ক		si_collapse;	// soundsalike: ?
    hashtab_T	si_commonwords;	// hashtable for common words
    time_t	si_sugtime;	// timestamp for .sug file
    পূর্ণাঙ্ক		si_rem_accents;	// soundsalike: remove accents
    garray_T	si_map;		// MAP info concatenated
    char_u	*si_midword;	// MIDWORD chars or NULL
    পূর্ণাঙ্ক		si_compmax;	// max nr of words for compounding
    পূর্ণাঙ্ক		si_compminlen;	// minimal length for compounding
    পূর্ণাঙ্ক		si_compsylmax;	// max nr of syllables for compounding
    পূর্ণাঙ্ক		si_compoptions;	// COMP_ flags
    garray_T	si_comppat;	// CHECKCOMPOUNDPATTERN items, each stored as
				// a string
    char_u	*si_compflags;	// flags used for compounding
    char_u	si_nobreak;	// NOBREAK
    char_u	*si_syllable;	// syllable string
    garray_T	si_prefcond;	// table with conditions for postponed
				// prefixes, each stored as a string
    পূর্ণাঙ্ক		si_newprefID;	// current value for ah_newID
    পূর্ণাঙ্ক		si_newcompID;	// current value for compound ID
} spellinfo_T;

স্থির পূর্ণাঙ্ক is_aff_rule(char_u **বস্তুসমূহ, পূর্ণাঙ্ক itemcnt, বর্ণ *rulename, পূর্ণাঙ্ক	 mincount);
স্থির শূন্য aff_process_flags(afffile_T *affile, affentry_T *entry);
স্থির পূর্ণাঙ্ক spell_info_item(char_u *s);
স্থির unsigned affitem2flag(পূর্ণাঙ্ক flagtype, char_u *item, char_u	*fname, পূর্ণাঙ্ক lnum);
স্থির unsigned get_affitem(পূর্ণাঙ্ক flagtype, char_u **pp);
স্থির শূন্য process_compflags(spellinfo_T *spin, afffile_T *aff, char_u *compflags);
স্থির শূন্য check_renumber(spellinfo_T *spin);
স্থির শূন্য aff_check_সংখ্যা(পূর্ণাঙ্ক spinval, পূর্ণাঙ্ক affval, বর্ণ *name);
স্থির শূন্য aff_check_string(char_u *spinval, char_u *affval, বর্ণ *name);
স্থির পূর্ণাঙ্ক str_equal(char_u *s1, char_u	*s2);
স্থির শূন্য add_fromto(spellinfo_T *spin, garray_T *gap, char_u	*from, char_u *to);
স্থির পূর্ণাঙ্ক sal_to_bool(char_u *s);
স্থির পূর্ণাঙ্ক get_affix_flags(afffile_T *affile, char_u *afflist);
স্থির পূর্ণাঙ্ক get_pfxlist(afffile_T *affile, char_u *afflist, char_u *store_afflist, পূর্ণাঙ্ক *cntp);
স্থির পূর্ণাঙ্ক get_compflags(afffile_T *affile, char_u *afflist, char_u *store_afflist, পূর্ণাঙ্ক *cntp);
স্থির পূর্ণাঙ্ক store_aff_word(spellinfo_T *spin, char_u *word, char_u *afflist, afffile_T *affile, hashtab_T *ht, hashtab_T *xht, পূর্ণাঙ্ক condit, পূর্ণাঙ্ক flags, char_u *pfxlist, পূর্ণাঙ্ক pfxlen);
স্থির শূন্য *getroom(spellinfo_T *spin, size_t len, পূর্ণাঙ্ক align);
স্থির char_u *getroom_save(spellinfo_T *spin, char_u *s);
স্থির পূর্ণাঙ্ক store_word(spellinfo_T *spin, char_u *word, পূর্ণাঙ্ক flags, পূর্ণাঙ্ক region, char_u *pfxlist, পূর্ণাঙ্ক need_affix);
স্থির পূর্ণাঙ্ক বৃক্ষ_add_word(spellinfo_T *spin, char_u *word, wordnode_T *বৃক্ষ, পূর্ণাঙ্ক flags, পূর্ণাঙ্ক region, পূর্ণাঙ্ক affixID);
স্থির wordnode_T *get_wordnode(spellinfo_T *spin);
স্থির শূন্য free_wordnode(spellinfo_T *spin, wordnode_T *n);
স্থির শূন্য wordtree_compress(spellinfo_T *spin, wordnode_T *মূল, বর্ণ *name);
স্থির long গ্রন্থি_compress(spellinfo_T *spin, wordnode_T *গ্রন্থি, hashtab_T *ht, long *tot);
স্থির পূর্ণাঙ্ক গ্রন্থি_equal(wordnode_T *n1, wordnode_T *n2);
স্থির শূন্য clear_গ্রন্থি(wordnode_T *গ্রন্থি);
স্থির পূর্ণাঙ্ক put_গ্রন্থি(FILE *fd, wordnode_T *গ্রন্থি, পূর্ণাঙ্ক idx, পূর্ণাঙ্ক regionmask, পূর্ণাঙ্ক prefixtree);
স্থির পূর্ণাঙ্ক sug_filltree(spellinfo_T *spin, slang_T *slang);
স্থির পূর্ণাঙ্ক sug_maketable(spellinfo_T *spin);
স্থির পূর্ণাঙ্ক sug_filltable(spellinfo_T *spin, wordnode_T *গ্রন্থি, পূর্ণাঙ্ক startwordnr, garray_T *gap);
স্থির পূর্ণাঙ্ক offset2bytes(পূর্ণাঙ্ক nr, char_u *buf);
স্থির শূন্য sug_write(spellinfo_T *spin, char_u *fname);
স্থির শূন্য spell_message(spellinfo_T *spin, char_u *str);
স্থির শূন্য init_spellfile(শূন্য);

// In the postponed prefixes tree wn_flags is used to store the WFP_ flags,
// but it must be negative to indicate the prefix tree to tree_add_word().
// Use a negative number with the lower 8 bits zero.
#define PFX_FLAGS	(-256)

// flags for "condit" argument of store_aff_word()
#define CONDIT_COMB	1	// affix must combine
#define CONDIT_CFIX	2	// affix must have CIRCUMFIX flag
#define CONDIT_SUF	4	// add a suffix for matching flags
#define CONDIT_AFF	8	// word already has an affix

/*
 * Tunable parameters for when the tree is compressed.  Filled from the
 * 'mkspellmem' option.
 */
স্থির long compress_start = 30000;	// memory / SBLOCKSIZE
স্থির long compress_inc = 100;		// memory / SBLOCKSIZE
স্থির long compress_added = 500000;	// word count

/*
 * Check the 'mkspellmem' option.  Return FAIL if it's wrong.
 * Sets "sps_flags".
 */
    পূর্ণাঙ্ক
spell_check_msm(শূন্য)
{
    char_u	*p = p_msm;
    long	start = 0;
    long	incr = 0;
    long	added = 0;

    যদি (!VIM_ISDIGIT(*p))
	প্রত্যাগম FAIL;
    // block count = (value * 1024) / SBLOCKSIZE (but avoid overflow)
    start = (getdigits(&p) * 10) / (SBLOCKSIZE / 102);
    যদি (*p != ',')
	প্রত্যাগম FAIL;
    ++p;
    যদি (!VIM_ISDIGIT(*p))
	প্রত্যাগম FAIL;
    incr = (getdigits(&p) * 102) / (SBLOCKSIZE / 10);
    যদি (*p != ',')
	প্রত্যাগম FAIL;
    ++p;
    যদি (!VIM_ISDIGIT(*p))
	প্রত্যাগম FAIL;
    added = getdigits(&p) * 1024;
    যদি (*p != NUL)
	প্রত্যাগম FAIL;

    যদি (start == 0 || incr == 0 || added == 0 || incr > start)
	প্রত্যাগম FAIL;

    compress_start = start;
    compress_inc = incr;
    compress_added = added;
    প্রত্যাগম OK;
}

#ifdef SPELL_PRINTTREE
/*
 * For debugging the tree code: print the current tree in a (more or less)
 * readable format, so that we can see what happens when adding a word and/or
 * compressing the tree.
 * Based on code from Olaf Seibert.
 */
# define PRINTLINESIZE	1000
# define PRINTWIDTH	6

# define PRINTSOME(l, depth, fmt, a1, a2) vim_snprintf(l + depth * PRINTWIDTH, \
	    PRINTLINESIZE - PRINTWIDTH * depth, fmt, a1, a2)

স্থির বর্ণ line1[PRINTLINESIZE];
স্থির বর্ণ line2[PRINTLINESIZE];
স্থির বর্ণ line3[PRINTLINESIZE];

    স্থির শূন্য
spell_clear_flags(wordnode_T *গ্রন্থি)
{
    wordnode_T	*np;

    FOR_ALL_গ্রন্থি_SIBLINGS(গ্রন্থি, np)
    {
	np->wn_u1.index = FALSE;
	spell_clear_flags(np->wn_child);
    }
}

    স্থির শূন্য
spell_print_গ্রন্থি(wordnode_T *গ্রন্থি, পূর্ণাঙ্ক depth)
{
    যদি (গ্রন্থি->wn_u1.index)
    {
	// Done this node before, print the reference.
	PRINTSOME(line1, depth, "(%d)", গ্রন্থি->wn_nr, 0);
	PRINTSOME(line2, depth, "    ", 0, 0);
	PRINTSOME(line3, depth, "    ", 0, 0);
	বার্তা(line1);
	বার্তা(line2);
	বার্তা(line3);
    }
    অন্যথা
    {
	গ্রন্থি->wn_u1.index = TRUE;

	যদি (গ্রন্থি->wn_byte != NUL)
	{
	    যদি (গ্রন্থি->wn_child != রিক্ত)
		PRINTSOME(line1, depth, " %c -> ", গ্রন্থি->wn_byte, 0);
	    অন্যথা
		// Cannot happen?
		PRINTSOME(line1, depth, " %c ???", গ্রন্থি->wn_byte, 0);
	}
	অন্যথা
	    PRINTSOME(line1, depth, " $    ", 0, 0);

	PRINTSOME(line2, depth, "%d/%d    ", গ্রন্থি->wn_nr, গ্রন্থি->wn_refs);

	যদি (গ্রন্থি->wn_sibling != রিক্ত)
	    PRINTSOME(line3, depth, " |    ", 0, 0);
	অন্যথা
	    PRINTSOME(line3, depth, "      ", 0, 0);

	যদি (গ্রন্থি->wn_byte == NUL)
	{
	    বার্তা(line1);
	    বার্তা(line2);
	    বার্তা(line3);
	}

	// do the children
	যদি (গ্রন্থি->wn_byte != NUL && গ্রন্থি->wn_child != রিক্ত)
	    spell_print_গ্রন্থি(গ্রন্থি->wn_child, depth + 1);

	// do the siblings
	যদি (গ্রন্থি->wn_sibling != রিক্ত)
	{
	    // get rid of all parent details except |
	    STRCPY(line1, line3);
	    STRCPY(line2, line3);
	    spell_print_গ্রন্থি(গ্রন্থি->wn_sibling, depth);
	}
    }
}

    স্থির শূন্য
spell_print_বৃক্ষ(wordnode_T *মূল)
{
    যদি (মূল == রিক্ত)
	প্রত্যাগম;

    // Clear the "wn_u1.index" fields, used to remember what has been done.
    spell_clear_flags(মূল);

    // Recursively print the tree.
    spell_print_গ্রন্থি(মূল, 0);
}
#endif // SPELL_PRINTTREE

/*
 * Read the affix file "fname".
 * Returns an afffile_T, NULL for complete failure.
 */
    স্থির afffile_T *
spell_read_aff(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    afffile_T	*aff;
    char_u	rline[MAXLINELEN];
    char_u	*সারি;
    char_u	*pc = রিক্ত;
#define MAXITEMCNT  30
    char_u	*(বস্তুসমূহ[MAXITEMCNT]);
    পূর্ণাঙ্ক		itemcnt;
    char_u	*p;
    পূর্ণাঙ্ক		lnum = 0;
    affheader_T	*cur_aff = রিক্ত;
    পূর্ণাঙ্ক		did_postpone_prefix = FALSE;
    পূর্ণাঙ্ক		aff_todo = 0;
    hashtab_T	*tp;
    char_u	*নিচের = রিক্ত;
    char_u	*fol = রিক্ত;
    char_u	*upp = রিক্ত;
    পূর্ণাঙ্ক		do_rep;
    পূর্ণাঙ্ক		do_repsal;
    পূর্ণাঙ্ক		do_sal;
    পূর্ণাঙ্ক		do_mapline;
    পূর্ণাঙ্ক		found_map = FALSE;
    hashitem_T	*hi;
    পূর্ণাঙ্ক		l;
    পূর্ণাঙ্ক		compminlen = 0;		// COMPOUNDMIN value
    পূর্ণাঙ্ক		compsylmax = 0;		// COMPOUNDSYLMAX value
    পূর্ণাঙ্ক		compoptions = 0;	// COMP_ flags
    পূর্ণাঙ্ক		compmax = 0;		// COMPOUNDWORDMAX value
    char_u	*compflags = রিক্ত;	// COMPOUNDFLAG and COMPOUNDRULE
					// concatenated
    char_u	*midword = রিক্ত;	// MIDWORD value
    char_u	*syllable = রিক্ত;	// SYLLABLE value
    char_u	*sofofrom = রিক্ত;	// SOFOFROM value
    char_u	*sofoto = রিক্ত;		// SOFOTO value

    /*
     * Open the file.
     */
    fd = mch_fopen((বর্ণ *)fname, "r");
    যদি (fd == রিক্ত)
    {
	semsg(_(e_cant_open_file_str), fname);
	প্রত্যাগম রিক্ত;
    }

    vim_snprintf((বর্ণ *)IObuff, IOSIZE, _("Reading affix file %s..."), fname);
    spell_message(spin, IObuff);

    // Only do REP lines when not done in another .aff file already.
    do_rep = spin->si_rep.ga_len == 0;

    // Only do REPSAL lines when not done in another .aff file already.
    do_repsal = spin->si_repsal.ga_len == 0;

    // Only do SAL lines when not done in another .aff file already.
    do_sal = spin->si_sal.ga_len == 0;

    // Only do MAP lines when not done in another .aff file already.
    do_mapline = spin->si_map.ga_len == 0;

    /*
     * Allocate and init the afffile_T structure.
     */
    aff = (afffile_T *)getroom(spin, পরিমাণ(afffile_T), TRUE);
    যদি (aff == রিক্ত)
    {
	fclose(fd);
	প্রত্যাগম রিক্ত;
    }
    hash_init(&aff->af_pref);
    hash_init(&aff->af_suff);
    hash_init(&aff->af_comp);

    /*
     * Read all the lines in the file one by one.
     */
    যাবত্ (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	সারি_breakcheck();
	++lnum;

	// Skip comment lines.
	যদি (*rline == '#')
	    অনুবর্ত;

	// Convert from "SET" to 'encoding' when needed.
	vim_free(pc);
	যদি (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, রিক্ত);
	    যদি (pc == রিক্ত)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
							   fname, lnum, rline);
		অনুবর্ত;
	    }
	    সারি = pc;
	}
	অন্যথা
	{
	    pc = রিক্ত;
	    সারি = rline;
	}

	// Split the line up in white separated items.  Put a NUL after each
	// item.
	itemcnt = 0;
	প্রতি (p = সারি; ; )
	{
	    যাবত্ (*p != NUL && *p <= ' ')  // skip white space and CR/NL
		++p;
	    যদি (*p == NUL)
		ভঙ্গ;
	    যদি (itemcnt == MAXITEMCNT)	    // too many items
		ভঙ্গ;
	    বস্তুসমূহ[itemcnt++] = p;
	    // A few items have arbitrary text argument, don't split them.
	    যদি (itemcnt == 2 && spell_info_item(বস্তুসমূহ[0]))
		যাবত্ (*p >= ' ' || *p == TAB)    // skip until CR/NL
		    ++p;
	    অন্যথা
		যাবত্ (*p > ' ')    // skip until white space or CR/NL
		    ++p;
	    যদি (*p == NUL)
		ভঙ্গ;
	    *p++ = NUL;
	}

	// Handle non-empty lines.
	যদি (itemcnt > 0)
	{
	    যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "SET", 2) && aff->af_enc == রিক্ত)
	    {
		// Setup for conversion from "ENC" to 'encoding'.
		aff->af_enc = enc_canonize(বস্তুসমূহ[1]);
		যদি (aff->af_enc != রিক্ত && !spin->si_ascii
			&& convert_setup(&spin->si_conv, aff->af_enc,
							       p_enc) == FAIL)
		    smsg(_("Conversion in %s not supported: from %s to %s"),
					       fname, aff->af_enc, p_enc);
		spin->si_conv.vc_fail = TRUE;
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "FLAG", 2)
					      && aff->af_flagtype == AFT_CHAR)
	    {
		যদি (STRCMP(বস্তুসমূহ[1], "long") == 0)
		    aff->af_flagtype = AFT_LONG;
		অন্যথা যদি (STRCMP(বস্তুসমূহ[1], "num") == 0)
		    aff->af_flagtype = AFT_অঙ্ক;
		অন্যথা যদি (STRCMP(বস্তুসমূহ[1], "caplong") == 0)
		    aff->af_flagtype = AFT_CAPLONG;
		অন্যথা
		    smsg(_("Invalid value for FLAG in %s line %d: %s"),
			    fname, lnum, বস্তুসমূহ[1]);
		যদি (aff->af_rare != 0
			|| aff->af_keepcase != 0
			|| aff->af_bad != 0
			|| aff->af_needaffix != 0
			|| aff->af_circumfix != 0
			|| aff->af_needcomp != 0
			|| aff->af_comproot != 0
			|| aff->af_nosuggest != 0
			|| compflags != রিক্ত
			|| aff->af_suff.ht_used > 0
			|| aff->af_pref.ht_used > 0)
		    smsg(_("FLAG after using flags in %s line %d: %s"),
			    fname, lnum, বস্তুসমূহ[1]);
	    }
	    অন্যথা যদি (spell_info_item(বস্তুসমূহ[0]))
	    {
		    p = (char_u *)getroom(spin,
			    (spin->si_info == রিক্ত ? 0 : STRLEN(spin->si_info))
			    + STRLEN(বস্তুসমূহ[0])
			    + STRLEN(বস্তুসমূহ[1]) + 3, FALSE);
		    যদি (p != রিক্ত)
		    {
			যদি (spin->si_info != রিক্ত)
			{
			    STRCPY(p, spin->si_info);
			    STRCAT(p, "\n");
			}
			STRCAT(p, বস্তুসমূহ[0]);
			STRCAT(p, " ");
			STRCAT(p, বস্তুসমূহ[1]);
			spin->si_info = p;
		    }
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "MIDWORD", 2)
							   && midword == রিক্ত)
	    {
		midword = getroom_save(spin, বস্তুসমূহ[1]);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "TRY", 2))
	    {
		// ignored, we look in the tree for what chars may appear
	    }
	    // TODO: remove "RAR" later
	    অন্যথা যদি ((is_aff_rule(বস্তুসমূহ, itemcnt, "RAR", 2)
			|| is_aff_rule(বস্তুসমূহ, itemcnt, "RARE", 2))
							 && aff->af_rare == 0)
	    {
		aff->af_rare = affitem2flag(aff->af_flagtype, বস্তুসমূহ[1],
								 fname, lnum);
	    }
	    // TODO: remove "KEP" later
	    অন্যথা যদি ((is_aff_rule(বস্তুসমূহ, itemcnt, "KEP", 2)
			|| is_aff_rule(বস্তুসমূহ, itemcnt, "KEEPCASE", 2))
						     && aff->af_keepcase == 0)
	    {
		aff->af_keepcase = affitem2flag(aff->af_flagtype, বস্তুসমূহ[1],
								 fname, lnum);
	    }
	    অন্যথা যদি ((is_aff_rule(বস্তুসমূহ, itemcnt, "BAD", 2)
			|| is_aff_rule(বস্তুসমূহ, itemcnt, "FORBIDDENWORD", 2))
							  && aff->af_bad == 0)
	    {
		aff->af_bad = affitem2flag(aff->af_flagtype, বস্তুসমূহ[1],
								 fname, lnum);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "NEEDAFFIX", 2)
						    && aff->af_needaffix == 0)
	    {
		aff->af_needaffix = affitem2flag(aff->af_flagtype, বস্তুসমূহ[1],
								 fname, lnum);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "CIRCUMFIX", 2)
						    && aff->af_circumfix == 0)
	    {
		aff->af_circumfix = affitem2flag(aff->af_flagtype, বস্তুসমূহ[1],
								 fname, lnum);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "NOSUGGEST", 2)
						    && aff->af_nosuggest == 0)
	    {
		aff->af_nosuggest = affitem2flag(aff->af_flagtype, বস্তুসমূহ[1],
								 fname, lnum);
	    }
	    অন্যথা যদি ((is_aff_rule(বস্তুসমূহ, itemcnt, "NEEDCOMPOUND", 2)
			|| is_aff_rule(বস্তুসমূহ, itemcnt, "ONLYINCOMPOUND", 2))
						     && aff->af_needcomp == 0)
	    {
		aff->af_needcomp = affitem2flag(aff->af_flagtype, বস্তুসমূহ[1],
								 fname, lnum);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "COMPOUNDROOT", 2)
						     && aff->af_comproot == 0)
	    {
		aff->af_comproot = affitem2flag(aff->af_flagtype, বস্তুসমূহ[1],
								 fname, lnum);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "COMPOUNDFORBIDFLAG", 2)
						   && aff->af_compforbid == 0)
	    {
		aff->af_compforbid = affitem2flag(aff->af_flagtype, বস্তুসমূহ[1],
								 fname, lnum);
		যদি (aff->af_pref.ht_used > 0)
		    smsg(_("Defining COMPOUNDFORBIDFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "COMPOUNDPERMITFLAG", 2)
						   && aff->af_comppermit == 0)
	    {
		aff->af_comppermit = affitem2flag(aff->af_flagtype, বস্তুসমূহ[1],
								 fname, lnum);
		যদি (aff->af_pref.ht_used > 0)
		    smsg(_("Defining COMPOUNDPERMITFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "COMPOUNDFLAG", 2)
							 && compflags == রিক্ত)
	    {
		// Turn flag "c" into COMPOUNDRULE compatible string "c+",
		// "Na" into "Na+", "1234" into "1234+".
		p = getroom(spin, STRLEN(বস্তুসমূহ[1]) + 2, FALSE);
		যদি (p != রিক্ত)
		{
		    STRCPY(p, বস্তুসমূহ[1]);
		    STRCAT(p, "+");
		    compflags = p;
		}
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "COMPOUNDRULES", 2))
	    {
		// We don't use the count, but do check that it's a number and
		// not COMPOUNDRULE mistyped.
		যদি (atoi((বর্ণ *)বস্তুসমূহ[1]) == 0)
		    smsg(_("Wrong COMPOUNDRULES value in %s line %d: %s"),
						       fname, lnum, বস্তুসমূহ[1]);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "COMPOUNDRULE", 2))
	    {
		// Don't use the first rule if it is a number.
		যদি (compflags != রিক্ত || *skipdigits(বস্তুসমূহ[1]) != NUL)
		{
		    // Concatenate this string to previously defined ones,
		    // using a slash to separate them.
		    l = (পূর্ণাঙ্ক)STRLEN(বস্তুসমূহ[1]) + 1;
		    যদি (compflags != রিক্ত)
			l += (পূর্ণাঙ্ক)STRLEN(compflags) + 1;
		    p = getroom(spin, l, FALSE);
		    যদি (p != রিক্ত)
		    {
			যদি (compflags != রিক্ত)
			{
			    STRCPY(p, compflags);
			    STRCAT(p, "/");
			}
			STRCAT(p, বস্তুসমূহ[1]);
			compflags = p;
		    }
		}
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "COMPOUNDWORDMAX", 2)
							      && compmax == 0)
	    {
		compmax = atoi((বর্ণ *)বস্তুসমূহ[1]);
		যদি (compmax == 0)
		    smsg(_("Wrong COMPOUNDWORDMAX value in %s line %d: %s"),
						       fname, lnum, বস্তুসমূহ[1]);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "COMPOUNDMIN", 2)
							   && compminlen == 0)
	    {
		compminlen = atoi((বর্ণ *)বস্তুসমূহ[1]);
		যদি (compminlen == 0)
		    smsg(_("Wrong COMPOUNDMIN value in %s line %d: %s"),
						       fname, lnum, বস্তুসমূহ[1]);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "COMPOUNDSYLMAX", 2)
							   && compsylmax == 0)
	    {
		compsylmax = atoi((বর্ণ *)বস্তুসমূহ[1]);
		যদি (compsylmax == 0)
		    smsg(_("Wrong COMPOUNDSYLMAX value in %s line %d: %s"),
						       fname, lnum, বস্তুসমূহ[1]);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "CHECKCOMPOUNDDUP", 1))
	    {
		compoptions |= COMP_CHECKDUP;
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "CHECKCOMPOUNDREP", 1))
	    {
		compoptions |= COMP_CHECKREP;
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "CHECKCOMPOUNDCASE", 1))
	    {
		compoptions |= COMP_CHECKCASE;
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "CHECKCOMPOUNDTRIPLE", 1))
	    {
		compoptions |= COMP_CHECKTRIPLE;
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "CHECKCOMPOUNDPATTERN", 2))
	    {
		যদি (atoi((বর্ণ *)বস্তুসমূহ[1]) == 0)
		    smsg(_("Wrong CHECKCOMPOUNDPATTERN value in %s line %d: %s"),
						       fname, lnum, বস্তুসমূহ[1]);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "CHECKCOMPOUNDPATTERN", 3))
	    {
		garray_T    *gap = &spin->si_comppat;
		পূর্ণাঙ্ক	    i;

		// Only add the couple if it isn't already there.
		প্রতি (i = 0; i < gap->ga_len - 1; i += 2)
		    যদি (STRCMP(((char_u **)(gap->ga_data))[i], বস্তুসমূহ[1]) == 0
			    && STRCMP(((char_u **)(gap->ga_data))[i + 1],
							       বস্তুসমূহ[2]) == 0)
			ভঙ্গ;
		যদি (i >= gap->ga_len && ga_grow(gap, 2) == OK)
		{
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, বস্তুসমূহ[1]);
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, বস্তুসমূহ[2]);
		}
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "SYLLABLE", 2)
							  && syllable == রিক্ত)
	    {
		syllable = getroom_save(spin, বস্তুসমূহ[1]);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "NOBREAK", 1))
	    {
		spin->si_nobreak = TRUE;
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "NOSPLITSUGS", 1))
	    {
		spin->si_nosplitsugs = TRUE;
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "NOCOMPOUNDSUGS", 1))
	    {
		spin->si_nocompoundsugs = TRUE;
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "NOSUGFILE", 1))
	    {
		spin->si_nosugfile = TRUE;
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "PFXPOSTPONE", 1))
	    {
		aff->af_pfxpostpone = TRUE;
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "IGNOREEXTRA", 1))
	    {
		aff->af_ignoreextra = TRUE;
	    }
	    অন্যথা যদি ((STRCMP(বস্তুসমূহ[0], "PFX") == 0
					      || STRCMP(বস্তুসমূহ[0], "SFX") == 0)
		    && aff_todo == 0
		    && itemcnt >= 4)
	    {
		পূর্ণাঙ্ক	lasti = 4;
		char_u	চাবি[AH_চাবি_LEN];

		যদি (*বস্তুসমূহ[0] == 'P')
		    tp = &aff->af_pref;
		অন্যথা
		    tp = &aff->af_suff;

		// Myspell allows the same affix name to be used multiple
		// times.  The affix files that do this have an undocumented
		// "S" flag on all but the last block, thus we check for that
		// and store it in ah_follows.
		vim_strncpy(চাবি, বস্তুসমূহ[1], AH_চাবি_LEN - 1);
		hi = hash_find(tp, চাবি);
		যদি (!HASHITEM_EMPTY(hi))
		{
		    cur_aff = HI2AH(hi);
		    যদি (cur_aff->ah_combine != (*বস্তুসমূহ[2] == 'Y'))
			smsg(_("Different combining flag in continued affix block in %s line %d: %s"),
						   fname, lnum, বস্তুসমূহ[1]);
		    যদি (!cur_aff->ah_follows)
			smsg(_("Duplicate affix in %s line %d: %s"),
						       fname, lnum, বস্তুসমূহ[1]);
		}
		অন্যথা
		{
		    // New affix letter.
		    cur_aff = (affheader_T *)getroom(spin,
						   পরিমাণ(affheader_T), TRUE);
		    যদি (cur_aff == রিক্ত)
			ভঙ্গ;
		    cur_aff->ah_flag = affitem2flag(aff->af_flagtype, বস্তুসমূহ[1],
								 fname, lnum);
		    যদি (cur_aff->ah_flag == 0 || STRLEN(বস্তুসমূহ[1]) >= AH_চাবি_LEN)
			ভঙ্গ;
		    যদি (cur_aff->ah_flag == aff->af_bad
			    || cur_aff->ah_flag == aff->af_rare
			    || cur_aff->ah_flag == aff->af_keepcase
			    || cur_aff->ah_flag == aff->af_needaffix
			    || cur_aff->ah_flag == aff->af_circumfix
			    || cur_aff->ah_flag == aff->af_nosuggest
			    || cur_aff->ah_flag == aff->af_needcomp
			    || cur_aff->ah_flag == aff->af_comproot)
			smsg(_("Affix also used for BAD/RARE/KEEPCASE/NEEDAFFIX/NEEDCOMPOUND/NOSUGGEST in %s line %d: %s"),
						       fname, lnum, বস্তুসমূহ[1]);
		    STRCPY(cur_aff->ah_চাবি, বস্তুসমূহ[1]);
		    hash_add(tp, cur_aff->ah_চাবি, "spelling");

		    cur_aff->ah_combine = (*বস্তুসমূহ[2] == 'Y');
		}

		// Check for the "S" flag, which apparently means that another
		// block with the same affix name is following.
		যদি (itemcnt > lasti && STRCMP(বস্তুসমূহ[lasti], "S") == 0)
		{
		    ++lasti;
		    cur_aff->ah_follows = TRUE;
		}
		অন্যথা
		    cur_aff->ah_follows = FALSE;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter.
		যদি (itemcnt > lasti && *বস্তুসমূহ[lasti] != '#')
		    smsg(_(e_afftrailing), fname, lnum, বস্তুসমূহ[lasti]);

		যদি (STRCMP(বস্তুসমূহ[2], "Y") != 0 && STRCMP(বস্তুসমূহ[2], "N") != 0)
		    smsg(_("Expected Y or N in %s line %d: %s"),
						       fname, lnum, বস্তুসমূহ[2]);

		যদি (*বস্তুসমূহ[0] == 'P' && aff->af_pfxpostpone)
		{
		    যদি (cur_aff->ah_newID == 0)
		    {
			// Use a new number in the .spl file later, to be able
			// to handle multiple .aff files.
			check_renumber(spin);
			cur_aff->ah_newID = ++spin->si_newprefID;

			// We only really use ah_newID if the prefix is
			// postponed.  We know that only after handling all
			// the items.
			did_postpone_prefix = FALSE;
		    }
		    অন্যথা
			// Did use the ID in a previous block.
			did_postpone_prefix = TRUE;
		}

		aff_todo = atoi((বর্ণ *)বস্তুসমূহ[3]);
	    }
	    অন্যথা যদি ((STRCMP(বস্তুসমূহ[0], "PFX") == 0
					      || STRCMP(বস্তুসমূহ[0], "SFX") == 0)
		    && aff_todo > 0
		    && STRCMP(cur_aff->ah_চাবি, বস্তুসমূহ[1]) == 0
		    && itemcnt >= 5)
	    {
		affentry_T	*aff_entry;
		পূর্ণাঙ্ক		upper = FALSE;
		পূর্ণাঙ্ক		lasti = 5;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter,
		// unless IGNOREEXTRA is used.  Hunspell uses a "-" item.
		যদি (itemcnt > lasti
			&& !aff->af_ignoreextra
			&& *বস্তুসমূহ[lasti] != '#'
			&& (STRCMP(বস্তুসমূহ[lasti], "-") != 0
						     || itemcnt != lasti + 1))
		    smsg(_(e_afftrailing), fname, lnum, বস্তুসমূহ[lasti]);

		// New item for an affix letter.
		--aff_todo;
		aff_entry = (affentry_T *)getroom(spin,
						    পরিমাণ(affentry_T), TRUE);
		যদি (aff_entry == রিক্ত)
		    ভঙ্গ;

		যদি (STRCMP(বস্তুসমূহ[2], "0") != 0)
		    aff_entry->ae_chop = getroom_save(spin, বস্তুসমূহ[2]);
		যদি (STRCMP(বস্তুসমূহ[3], "0") != 0)
		{
		    aff_entry->ae_add = getroom_save(spin, বস্তুসমূহ[3]);

		    // Recognize flags on the affix: abcd/XYZ
		    aff_entry->ae_flags = vim_strchr(aff_entry->ae_add, '/');
		    যদি (aff_entry->ae_flags != রিক্ত)
		    {
			*aff_entry->ae_flags++ = NUL;
			aff_process_flags(aff, aff_entry);
		    }
		}

		// Don't use an affix entry with non-ASCII characters when
		// "spin->si_ascii" is TRUE.
		যদি (!spin->si_ascii || !(has_non_ascii(aff_entry->ae_chop)
					  || has_non_ascii(aff_entry->ae_add)))
		{
		    aff_entry->ae_next = cur_aff->ah_first;
		    cur_aff->ah_first = aff_entry;

		    যদি (STRCMP(বস্তুসমূহ[4], ".") != 0)
		    {
			char_u	buf[MAXLINELEN];

			aff_entry->ae_cond = getroom_save(spin, বস্তুসমূহ[4]);
			// Note: this silently truncates the buffer, but this should
			// not happen in practice
			যদি (*বস্তুসমূহ[0] == 'P')
			    vim_snprintf((বর্ণ *)buf, পরিমাণ(buf), "^%s", বস্তুসমূহ[4]);
			অন্যথা
			    vim_snprintf((বর্ণ *)buf, পরিমাণ(buf), "%s$", বস্তুসমূহ[4]);
			aff_entry->ae_prog = vim_regcomp(buf,
					    RE_MAGIC + RE_STRING + RE_STRICT);
			যদি (aff_entry->ae_prog == রিক্ত)
			    smsg(_("Broken condition in %s line %d: %s"),
						       fname, lnum, বস্তুসমূহ[4]);
		    }

		    // For postponed prefixes we need an entry in si_prefcond
		    // for the condition.  Use an existing one if possible.
		    // Can't be done for an affix with flags, ignoring
		    // COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG.
		    যদি (*বস্তুসমূহ[0] == 'P' && aff->af_pfxpostpone
					       && aff_entry->ae_flags == রিক্ত)
		    {
			// When the chop string is one lower-case letter and
			// the add string ends in the upper-case letter we set
			// the "upper" flag, clear "ae_chop" and remove the
			// letters from "ae_add".  The condition must either
			// be empty or start with the same letter.
			যদি (aff_entry->ae_chop != রিক্ত
				&& aff_entry->ae_add != রিক্ত
				&& aff_entry->ae_chop[(*mb_ptr2len)(
						   aff_entry->ae_chop)] == NUL)
			{
			    পূর্ণাঙ্ক		c, c_up;

			    c = PTR2CHAR(aff_entry->ae_chop);
			    c_up = SPELL_TOUPPER(c);
			    যদি (c_up != c
				    && (aff_entry->ae_cond == রিক্ত
					|| PTR2CHAR(aff_entry->ae_cond) == c))
			    {
				p = aff_entry->ae_add
						  + STRLEN(aff_entry->ae_add);
				MB_PTR_BACK(aff_entry->ae_add, p);
				যদি (PTR2CHAR(p) == c_up)
				{
				    upper = TRUE;
				    aff_entry->ae_chop = রিক্ত;
				    *p = NUL;

				    // The condition is matched with the
				    // actual word, thus must check for the
				    // upper-case letter.
				    যদি (aff_entry->ae_cond != রিক্ত)
				    {
					char_u	buf[MAXLINELEN];

					যদি (has_mbyte)
					{
					    onecap_copy(বস্তুসমূহ[4], buf, TRUE);
					    aff_entry->ae_cond = getroom_save(
								   spin, buf);
					}
					অন্যথা
					    *aff_entry->ae_cond = c_up;
					যদি (aff_entry->ae_cond != রিক্ত)
					{
					    sprintf((বর্ণ *)buf, "^%s",
							  aff_entry->ae_cond);
					    vim_regfree(aff_entry->ae_prog);
					    aff_entry->ae_prog = vim_regcomp(
						    buf, RE_MAGIC + RE_STRING);
					}
				    }
				}
			    }
			}

			যদি (aff_entry->ae_chop == রিক্ত
					       && aff_entry->ae_flags == রিক্ত)
			{
			    পূর্ণাঙ্ক		idx;
			    char_u	**pp;
			    পূর্ণাঙ্ক		n;

			    // Find a previously used condition.
			    প্রতি (idx = spin->si_prefcond.ga_len - 1; idx >= 0;
									--idx)
			    {
				p = ((char_u **)spin->si_prefcond.ga_data)[idx];
				যদি (str_equal(p, aff_entry->ae_cond))
				    ভঙ্গ;
			    }
			    যদি (idx < 0 && ga_grow(&spin->si_prefcond, 1) == OK)
			    {
				// Not found, add a new condition.
				idx = spin->si_prefcond.ga_len++;
				pp = ((char_u **)spin->si_prefcond.ga_data)
									+ idx;
				যদি (aff_entry->ae_cond == রিক্ত)
				    *pp = রিক্ত;
				অন্যথা
				    *pp = getroom_save(spin,
							  aff_entry->ae_cond);
			    }

			    // Add the prefix to the prefix tree.
			    যদি (aff_entry->ae_add == রিক্ত)
				p = (char_u *)"";
			    অন্যথা
				p = aff_entry->ae_add;

			    // PFX_FLAGS is a negative number, so that
			    // tree_add_word() knows this is the prefix tree.
			    n = PFX_FLAGS;
			    যদি (!cur_aff->ah_combine)
				n |= WFP_NC;
			    যদি (upper)
				n |= WFP_UP;
			    যদি (aff_entry->ae_comppermit)
				n |= WFP_COMPPERMIT;
			    যদি (aff_entry->ae_compforbid)
				n |= WFP_COMPFORBID;
			    বৃক্ষ_add_word(spin, p, spin->si_prefroot, n,
						      idx, cur_aff->ah_newID);
			    did_postpone_prefix = TRUE;
			}

			// Didn't actually use ah_newID, backup si_newprefID.
			যদি (aff_todo == 0 && !did_postpone_prefix)
			{
			    --spin->si_newprefID;
			    cur_aff->ah_newID = 0;
			}
		    }
		}
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "FOL", 2) && fol == রিক্ত)
	    {
		fol = vim_strsave(বস্তুসমূহ[1]);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "LOW", 2) && নিচের == রিক্ত)
	    {
		নিচের = vim_strsave(বস্তুসমূহ[1]);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "UPP", 2) && upp == রিক্ত)
	    {
		upp = vim_strsave(বস্তুসমূহ[1]);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "REP", 2)
		     || is_aff_rule(বস্তুসমূহ, itemcnt, "REPSAL", 2))
	    {
		// Ignore REP/REPSAL count
		যদি (!SAFE_isdigit(*বস্তুসমূহ[1]))
		    smsg(_("Expected REP(SAL) count in %s line %d"),
								 fname, lnum);
	    }
	    অন্যথা যদি ((STRCMP(বস্তুসমূহ[0], "REP") == 0
			|| STRCMP(বস্তুসমূহ[0], "REPSAL") == 0)
		    && itemcnt >= 3)
	    {
		// REP/REPSAL item
		// Myspell ignores extra arguments, we require it starts with
		// # to detect mistakes.
		যদি (itemcnt > 3 && বস্তুসমূহ[3][0] != '#')
		    smsg(_(e_afftrailing), fname, lnum, বস্তুসমূহ[3]);
		যদি (বস্তুসমূহ[0][3] == 'S' ? do_repsal : do_rep)
		{
		    // Replace underscore with space (can't include a space
		    // directly).
		    প্রতি (p = বস্তুসমূহ[1]; *p != NUL; MB_PTR_ADV(p))
			যদি (*p == '_')
			    *p = ' ';
		    প্রতি (p = বস্তুসমূহ[2]; *p != NUL; MB_PTR_ADV(p))
			যদি (*p == '_')
			    *p = ' ';
		    add_fromto(spin, বস্তুসমূহ[0][3] == 'S'
					 ? &spin->si_repsal
					 : &spin->si_rep, বস্তুসমূহ[1], বস্তুসমূহ[2]);
		}
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "MAP", 2))
	    {
		// MAP item or count
		যদি (!found_map)
		{
		    // First line contains the count.
		    found_map = TRUE;
		    যদি (!SAFE_isdigit(*বস্তুসমূহ[1]))
			smsg(_("Expected MAP count in %s line %d"),
								 fname, lnum);
		}
		অন্যথা যদি (do_mapline)
		{
		    পূর্ণাঙ্ক		c;

		    // Check that every character appears only once.
		    প্রতি (p = বস্তুসমূহ[1]; *p != NUL; )
		    {
			c = mb_ptr2char_adv(&p);
			যদি ((spin->si_map.ga_len > 0
				    && vim_strchr(spin->si_map.ga_data, c)
								      != রিক্ত)
				|| vim_strchr(p, c) != রিক্ত)
			    smsg(_("Duplicate character in MAP in %s line %d"),
								 fname, lnum);
		    }

		    // We simply concatenate all the MAP strings, separated by
		    // slashes.
		    ga_concat(&spin->si_map, বস্তুসমূহ[1]);
		    ga_append(&spin->si_map, '/');
		}
	    }
	    // Accept "SAL from to" and "SAL from to  #comment".
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "SAL", 3))
	    {
		যদি (do_sal)
		{
		    // SAL item (sounds-a-like)
		    // Either one of the known keys or a from-to pair.
		    যদি (STRCMP(বস্তুসমূহ[1], "followup") == 0)
			spin->si_followup = sal_to_bool(বস্তুসমূহ[2]);
		    অন্যথা যদি (STRCMP(বস্তুসমূহ[1], "collapse_result") == 0)
			spin->si_collapse = sal_to_bool(বস্তুসমূহ[2]);
		    অন্যথা যদি (STRCMP(বস্তুসমূহ[1], "remove_accents") == 0)
			spin->si_rem_accents = sal_to_bool(বস্তুসমূহ[2]);
		    অন্যথা
			// when "to" is "_" it means empty
			add_fromto(spin, &spin->si_sal, বস্তুসমূহ[1],
				     STRCMP(বস্তুসমূহ[2], "_") == 0 ? (char_u *)""
								: বস্তুসমূহ[2]);
		}
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "SOFOFROM", 2)
							  && sofofrom == রিক্ত)
	    {
		sofofrom = getroom_save(spin, বস্তুসমূহ[1]);
	    }
	    অন্যথা যদি (is_aff_rule(বস্তুসমূহ, itemcnt, "SOFOTO", 2)
							    && sofoto == রিক্ত)
	    {
		sofoto = getroom_save(spin, বস্তুসমূহ[1]);
	    }
	    অন্যথা যদি (STRCMP(বস্তুসমূহ[0], "COMMON") == 0)
	    {
		পূর্ণাঙ্ক	i;

		প্রতি (i = 1; i < itemcnt; ++i)
		{
		    যদি (HASHITEM_EMPTY(hash_find(&spin->si_commonwords,
								   বস্তুসমূহ[i])))
		    {
			p = vim_strsave(বস্তুসমূহ[i]);
			যদি (p == রিক্ত)
			    ভঙ্গ;
			hash_add(&spin->si_commonwords, p, "spelling");
		    }
		}
	    }
	    অন্যথা
		smsg(_("Unrecognized or duplicate item in %s line %d: %s"),
						       fname, lnum, বস্তুসমূহ[0]);
	}
    }

    যদি (fol != রিক্ত || নিচের != রিক্ত || upp != রিক্ত)
    {
	যদি (spin->si_clear_chartab)
	{
	    // Clear the char type tables, don't want to use any of the
	    // currently used spell properties.
	    init_spell_chartab();
	    spin->si_clear_chartab = FALSE;
	}

	/*
	 * Don't write a word table for an ASCII file, so that we don't check
	 * for conflicts with a word table that matches 'encoding'.
	 * Don't write one for utf-8 either, we use utf_*() and
	 * mb_get_class(), the list of chars in the file will be incomplete.
	 */
	যদি (!spin->si_ascii && !enc_utf8)
	{
	    যদি (fol == রিক্ত || নিচের == রিক্ত || upp == রিক্ত)
		smsg(_("Missing FOL/LOW/UPP line in %s"), fname);
	    অন্যথা
		(শূন্য)set_spell_chartab(fol, নিচের, upp);
	}

	vim_free(fol);
	vim_free(নিচের);
	vim_free(upp);
    }

    // Use compound specifications of the .aff file for the spell info.
    যদি (compmax != 0)
    {
	aff_check_সংখ্যা(spin->si_compmax, compmax, "COMPOUNDWORDMAX");
	spin->si_compmax = compmax;
    }

    যদি (compminlen != 0)
    {
	aff_check_সংখ্যা(spin->si_compminlen, compminlen, "COMPOUNDMIN");
	spin->si_compminlen = compminlen;
    }

    যদি (compsylmax != 0)
    {
	যদি (syllable == রিক্ত)
	    smsg(_("COMPOUNDSYLMAX used without SYLLABLE"));
	aff_check_সংখ্যা(spin->si_compsylmax, compsylmax, "COMPOUNDSYLMAX");
	spin->si_compsylmax = compsylmax;
    }

    যদি (compoptions != 0)
    {
	aff_check_সংখ্যা(spin->si_compoptions, compoptions, "COMPOUND options");
	spin->si_compoptions |= compoptions;
    }

    যদি (compflags != রিক্ত)
	process_compflags(spin, aff, compflags);

    // Check that we didn't use too many renumbered flags.
    যদি (spin->si_newcompID < spin->si_newprefID)
    {
	যদি (spin->si_newcompID == 127 || spin->si_newcompID == 255)
	    বার্তা(_("Too many postponed prefixes"));
	অন্যথা যদি (spin->si_newprefID == 0 || spin->si_newprefID == 127)
	    বার্তা(_("Too many compound flags"));
	অন্যথা
	    বার্তা(_("Too many postponed prefixes and/or compound flags"));
    }

    যদি (syllable != রিক্ত)
    {
	aff_check_string(spin->si_syllable, syllable, "SYLLABLE");
	spin->si_syllable = syllable;
    }

    যদি (sofofrom != রিক্ত || sofoto != রিক্ত)
    {
	যদি (sofofrom == রিক্ত || sofoto == রিক্ত)
	    smsg(_("Missing SOFO%s line in %s"),
				     sofofrom == রিক্ত ? "FROM" : "TO", fname);
	অন্যথা যদি (spin->si_sal.ga_len > 0)
	    smsg(_("Both SAL and SOFO lines in %s"), fname);
	অন্যথা
	{
	    aff_check_string(spin->si_sofofr, sofofrom, "SOFOFROM");
	    aff_check_string(spin->si_sofoto, sofoto, "SOFOTO");
	    spin->si_sofofr = sofofrom;
	    spin->si_sofoto = sofoto;
	}
    }

    যদি (midword != রিক্ত)
    {
	aff_check_string(spin->si_midword, midword, "MIDWORD");
	spin->si_midword = midword;
    }

    vim_free(pc);
    fclose(fd);
    প্রত্যাগম aff;
}

/*
 * Return TRUE when items[0] equals "rulename", there are "mincount" items or
 * a comment is following after item "mincount".
 */
    স্থির পূর্ণাঙ্ক
is_aff_rule(
    char_u	**বস্তুসমূহ,
    পূর্ণাঙ্ক		itemcnt,
    বর্ণ	*rulename,
    পূর্ণাঙ্ক		mincount)
{
    প্রত্যাগম (STRCMP(বস্তুসমূহ[0], rulename) == 0
	    && (itemcnt == mincount
		|| (itemcnt > mincount && বস্তুসমূহ[mincount][0] == '#')));
}

/*
 * For affix "entry" move COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG from
 * ae_flags to ae_comppermit and ae_compforbid.
 */
    স্থির শূন্য
aff_process_flags(afffile_T *affile, affentry_T *entry)
{
    char_u	*p;
    char_u	*prevp;
    unsigned	flag;

    যদি (entry->ae_flags != রিক্ত
		&& (affile->af_compforbid != 0 || affile->af_comppermit != 0))
    {
	প্রতি (p = entry->ae_flags; *p != NUL; )
	{
	    prevp = p;
	    flag = get_affitem(affile->af_flagtype, &p);
	    যদি (flag == affile->af_comppermit || flag == affile->af_compforbid)
	    {
		STRMOVE(prevp, p);
		p = prevp;
		যদি (flag == affile->af_comppermit)
		    entry->ae_comppermit = TRUE;
		অন্যথা
		    entry->ae_compforbid = TRUE;
	    }
	    যদি (affile->af_flagtype == AFT_অঙ্ক && *p == ',')
		++p;
	}
	যদি (*entry->ae_flags == NUL)
	    entry->ae_flags = রিক্ত;	// nothing left
    }
}

/*
 * Return TRUE if "s" is the name of an info item in the affix file.
 */
    স্থির পূর্ণাঙ্ক
spell_info_item(char_u *s)
{
    প্রত্যাগম STRCMP(s, "NAME") == 0
	|| STRCMP(s, "HOME") == 0
	|| STRCMP(s, "VERSION") == 0
	|| STRCMP(s, "AUTHOR") == 0
	|| STRCMP(s, "EMAIL") == 0
	|| STRCMP(s, "COPYRIGHT") == 0;
}

/*
 * Turn an affix flag name into a number, according to the FLAG type.
 * returns zero for failure.
 */
    স্থির unsigned
affitem2flag(
    পূর্ণাঙ্ক		flagtype,
    char_u	*item,
    char_u	*fname,
    পূর্ণাঙ্ক		lnum)
{
    unsigned	res;
    char_u	*p = item;

    res = get_affitem(flagtype, &p);
    যদি (res == 0)
    {
	যদি (flagtype == AFT_অঙ্ক)
	    smsg(_("Flag is not a number in %s line %d: %s"),
							   fname, lnum, item);
	অন্যথা
	    smsg(_("Illegal flag in %s line %d: %s"),
							   fname, lnum, item);
    }
    যদি (*p != NUL)
    {
	smsg(_(e_affname), fname, lnum, item);
	প্রত্যাগম 0;
    }

    প্রত্যাগম res;
}

/*
 * Get one affix name from "*pp" and advance the pointer.
 * Returns ZERO_FLAG for "0".
 * Returns zero for an error, still advances the pointer then.
 */
    স্থির unsigned
get_affitem(পূর্ণাঙ্ক flagtype, char_u **pp)
{
    পূর্ণাঙ্ক		res;

    যদি (flagtype == AFT_অঙ্ক)
    {
	যদি (!VIM_ISDIGIT(**pp))
	{
	    ++*pp;	// always advance, avoid getting stuck
	    প্রত্যাগম 0;
	}
	res = getdigits(pp);
	যদি (res == 0)
	    res = ZERO_FLAG;
    }
    অন্যথা
    {
	res = mb_ptr2char_adv(pp);
	যদি (flagtype == AFT_LONG || (flagtype == AFT_CAPLONG
						 && res >= 'A' && res <= 'Z'))
	{
	    যদি (**pp == NUL)
		প্রত্যাগম 0;
	    res = mb_ptr2char_adv(pp) + (res << 16);
	}
    }
    প্রত্যাগম res;
}

/*
 * Process the "compflags" string used in an affix file and append it to
 * spin->si_compflags.
 * The processing involves changing the affix names to ID numbers, so that
 * they fit in one byte.
 */
    স্থির শূন্য
process_compflags(
    spellinfo_T	*spin,
    afffile_T	*aff,
    char_u	*compflags)
{
    char_u	*p;
    char_u	*prevp;
    unsigned	flag;
    compitem_T	*ci;
    পূর্ণাঙ্ক		id;
    পূর্ণাঙ্ক		len;
    char_u	*tp;
    char_u	চাবি[AH_চাবি_LEN];
    hashitem_T	*hi;

    // Make room for the old and the new compflags, concatenated with a / in
    // between.  Processing it makes it shorter, but we don't know by how
    // much, thus allocate the maximum.
    len = (পূর্ণাঙ্ক)STRLEN(compflags) + 1;
    যদি (spin->si_compflags != রিক্ত)
	len += (পূর্ণাঙ্ক)STRLEN(spin->si_compflags) + 1;
    p = getroom(spin, len, FALSE);
    যদি (p == রিক্ত)
	প্রত্যাগম;
    যদি (spin->si_compflags != রিক্ত)
    {
	STRCPY(p, spin->si_compflags);
	STRCAT(p, "/");
    }
    spin->si_compflags = p;
    tp = p + STRLEN(p);

    প্রতি (p = compflags; *p != NUL; )
    {
	যদি (vim_strchr((char_u *)"/?*+[]", *p) != রিক্ত)
	    // Copy non-flag characters directly.
	    *tp++ = *p++;
	অন্যথা
	{
	    // First get the flag number, also checks validity.
	    prevp = p;
	    flag = get_affitem(aff->af_flagtype, &p);
	    যদি (flag != 0)
	    {
		// Find the flag in the hashtable.  If it was used before, use
		// the existing ID.  Otherwise add a new entry.
		vim_strncpy(চাবি, prevp, p - prevp);
		hi = hash_find(&aff->af_comp, চাবি);
		যদি (!HASHITEM_EMPTY(hi))
		    id = HI2CI(hi)->ci_newID;
		অন্যথা
		{
		    ci = (compitem_T *)getroom(spin, পরিমাণ(compitem_T), TRUE);
		    যদি (ci == রিক্ত)
			ভঙ্গ;
		    STRCPY(ci->ci_চাবি, চাবি);
		    ci->ci_flag = flag;
		    // Avoid using a flag ID that has a special meaning in a
		    // regexp (also inside []).
		    কুরু
		    {
			check_renumber(spin);
			id = spin->si_newcompID--;
		    } যাবত্ (vim_strchr((char_u *)"/?*+[]\\-^", id) != রিক্ত);
		    ci->ci_newID = id;
		    hash_add(&aff->af_comp, ci->ci_চাবি, "spelling");
		}
		*tp++ = id;
	    }
	    যদি (aff->af_flagtype == AFT_অঙ্ক && *p == ',')
		++p;
	}
    }

    *tp = NUL;
}

/*
 * Check that the new IDs for postponed affixes and compounding don't overrun
 * each other.  We have almost 255 available, but start at 0-127 to avoid
 * using two bytes for utf-8.  When the 0-127 range is used up go to 128-255.
 * When that is used up an error message is given.
 */
    স্থির শূন্য
check_renumber(spellinfo_T *spin)
{
    যদি (spin->si_newprefID == spin->si_newcompID && spin->si_newcompID < 128)
    {
	spin->si_newprefID = 127;
	spin->si_newcompID = 255;
    }
}

/*
 * Append one affix or compound ID to "store_afflist".
 * Returns FAIL when this would overrun the fixed-size buffer.
 */
    স্থির পূর্ণাঙ্ক
store_afflist_add(
    char_u	*store_afflist,
    পূর্ণাঙ্ক		*cntp,
    পূর্ণাঙ্ক		id)
{
    যদি (*cntp >= MAXWLEN - 1)
    {
	emsg(_(e_too_many_postponed_prefixes_spell));
	প্রত্যাগম FAIL;
    }
    store_afflist[(*cntp)++] = id;
    store_afflist[*cntp] = NUL;
    প্রত্যাগম OK;
}

/*
 * Return TRUE if flag "flag" appears in affix list "afflist".
 */
    স্থির পূর্ণাঙ্ক
flag_in_afflist(পূর্ণাঙ্ক flagtype, char_u *afflist, unsigned flag)
{
    char_u	*p;
    unsigned	n;

    বিকল্প (flagtype)
    {
	পক্ষ AFT_CHAR:
	    প্রত্যাগম vim_strchr(afflist, flag) != রিক্ত;

	পক্ষ AFT_CAPLONG:
	পক্ষ AFT_LONG:
	    প্রতি (p = afflist; *p != NUL; )
	    {
		n = mb_ptr2char_adv(&p);
		যদি ((flagtype == AFT_LONG || (n >= 'A' && n <= 'Z'))
								 && *p != NUL)
		    n = mb_ptr2char_adv(&p) + (n << 16);
		যদি (n == flag)
		    প্রত্যাগম TRUE;
	    }
	    ভঙ্গ;

	পক্ষ AFT_অঙ্ক:
	    প্রতি (p = afflist; *p != NUL; )
	    {
		n = getdigits(&p);
		যদি (n == 0)
		    n = ZERO_FLAG;
		যদি (n == flag)
		    প্রত্যাগম TRUE;
		যদি (*p != NUL)	// skip over comma
		    ++p;
	    }
	    ভঙ্গ;
    }
    প্রত্যাগম FALSE;
}

/*
 * Give a warning when "spinval" and "affval" numbers are set and not the same.
 */
    স্থির শূন্য
aff_check_সংখ্যা(পূর্ণাঙ্ক spinval, পূর্ণাঙ্ক affval, বর্ণ *name)
{
    যদি (spinval != 0 && spinval != affval)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Give a warning when "spinval" and "affval" strings are set and not the same.
 */
    স্থির শূন্য
aff_check_string(char_u *spinval, char_u *affval, বর্ণ *name)
{
    যদি (spinval != রিক্ত && STRCMP(spinval, affval) != 0)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Return TRUE if strings "s1" and "s2" are equal.  Also consider both being
 * NULL as equal.
 */
    স্থির পূর্ণাঙ্ক
str_equal(char_u *s1, char_u *s2)
{
    যদি (s1 == রিক্ত || s2 == রিক্ত)
	প্রত্যাগম s1 == s2;
    প্রত্যাগম STRCMP(s1, s2) == 0;
}

/*
 * Add a from-to item to "gap".  Used for REP and SAL items.
 * They are stored case-folded.
 */
    স্থির শূন্য
add_fromto(
    spellinfo_T	*spin,
    garray_T	*gap,
    char_u	*from,
    char_u	*to)
{
    fromto_T	*ftp;
    char_u	word[MAXWLEN];

    যদি (ga_grow(gap, 1) == FAIL)
	প্রত্যাগম;

    ftp = ((fromto_T *)gap->ga_data) + gap->ga_len;
    (শূন্য)spell_casefold(curwin, from, (পূর্ণাঙ্ক)STRLEN(from), word, MAXWLEN);
    ftp->ft_from = getroom_save(spin, word);
    (শূন্য)spell_casefold(curwin, to, (পূর্ণাঙ্ক)STRLEN(to), word, MAXWLEN);
    ftp->ft_to = getroom_save(spin, word);
    ++gap->ga_len;
}

/*
 * Convert a boolean argument in a SAL line to TRUE or FALSE;
 */
    স্থির পূর্ণাঙ্ক
sal_to_bool(char_u *s)
{
    প্রত্যাগম STRCMP(s, "1") == 0 || STRCMP(s, "true") == 0;
}

/*
 * Free the structure filled by spell_read_aff().
 */
    স্থির শূন্য
spell_free_aff(afffile_T *aff)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    পূর্ণাঙ্ক		todo;
    affheader_T	*ah;
    affentry_T	*ae;

    vim_free(aff->af_enc);

    // All this trouble to free the "ae_prog" items...
    প্রতি (ht = &aff->af_pref; ; ht = &aff->af_suff)
    {
	todo = (পূর্ণাঙ্ক)ht->ht_used;
	FOR_ALL_HASHTAB_বস্তুসমূহ(ht, hi, todo)
	{
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		ah = HI2AH(hi);
		প্রতি (ae = ah->ah_first; ae != রিক্ত; ae = ae->ae_next)
		    vim_regfree(ae->ae_prog);
	    }
	}
	যদি (ht == &aff->af_suff)
	    ভঙ্গ;
    }

    hash_clear(&aff->af_pref);
    hash_clear(&aff->af_suff);
    hash_clear(&aff->af_comp);
}

/*
 * Read dictionary file "fname".
 * Returns OK or FAIL;
 */
    স্থির পূর্ণাঙ্ক
spell_read_dic(spellinfo_T *spin, char_u *fname, afffile_T *affile)
{
    hashtab_T	ht;
    char_u	সারি[MAXLINELEN];
    char_u	*p;
    char_u	*afflist;
    char_u	store_afflist[MAXWLEN];
    পূর্ণাঙ্ক		pfxlen;
    পূর্ণাঙ্ক		totlen;
    পূর্ণাঙ্ক		need_affix;
    char_u	*dw;
    char_u	*pc;
    char_u	*w;
    পূর্ণাঙ্ক		l;
    hash_T	hash;
    hashitem_T	*hi;
    FILE	*fd;
    পূর্ণাঙ্ক		lnum = 1;
    পূর্ণাঙ্ক		non_ascii = 0;
    পূর্ণাঙ্ক		retval = OK;
    char_u	message[MAXLINELEN + MAXWLEN];
    পূর্ণাঙ্ক		flags;
    পূর্ণাঙ্ক		duplicate = 0;
    time_T	last_বার্তা_time = 0;

    /*
     * Open the file.
     */
    fd = mch_fopen((বর্ণ *)fname, "r");
    যদি (fd == রিক্ত)
    {
	semsg(_(e_cant_open_file_str), fname);
	প্রত্যাগম FAIL;
    }

    // The hashtable is only used to detect duplicated words.
    hash_init(&ht);

    vim_snprintf((বর্ণ *)IObuff, IOSIZE,
				  _("Reading dictionary file %s..."), fname);
    spell_message(spin, IObuff);

    // start with a message for the first line
    spin->si_বার্তা_গুনতি = 999999;

    // Read and ignore the first line: word count.
    যদি (vim_fgets(সারি, MAXLINELEN, fd) || !vim_isdigit(*skipwhite(সারি)))
	semsg(_(e_no_word_গুনতি_in_str), fname);

    /*
     * Read all the lines in the file one by one.
     * The words are converted to 'encoding' here, before being added to
     * the hashtable.
     */
    যাবত্ (!vim_fgets(সারি, MAXLINELEN, fd) && !got_int)
    {
	সারি_breakcheck();
	++lnum;
	যদি (সারি[0] == '#' || সারি[0] == '/')
	    অনুবর্ত;	// comment line

	// Remove CR, LF and white space from the end.  White space halfway
	// the word is kept to allow e.g., "et al.".
	l = (পূর্ণাঙ্ক)STRLEN(সারি);
	যাবত্ (l > 0 && সারি[l - 1] <= ' ')
	    --l;
	যদি (l == 0)
	    অনুবর্ত;	// empty line
	সারি[l] = NUL;

	// Convert from "SET" to 'encoding' when needed.
	যদি (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, সারি, রিক্ত);
	    যদি (pc == রিক্ত)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
						       fname, lnum, সারি);
		অনুবর্ত;
	    }
	    w = pc;
	}
	অন্যথা
	{
	    pc = রিক্ত;
	    w = সারি;
	}

	// Truncate the word at the "/", set "afflist" to what follows.
	// Replace "\/" by "/" and "\\" by "\".
	afflist = রিক্ত;
	প্রতি (p = w; *p != NUL; MB_PTR_ADV(p))
	{
	    যদি (*p == '\\' && (p[1] == '\\' || p[1] == '/'))
		STRMOVE(p, p + 1);
	    অন্যথা যদি (*p == '/')
	    {
		*p = NUL;
		afflist = p + 1;
		ভঙ্গ;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	যদি (spin->si_ascii && has_non_ascii(w))
	{
	    ++non_ascii;
	    vim_free(pc);
	    অনুবর্ত;
	}

	// This takes time, print a message every 10000 words, but not more
	// often than once per second.
	যদি (spin->si_verbose && spin->si_বার্তা_গুনতি > 10000)
	{
	    spin->si_বার্তা_গুনতি = 0;
	    যদি (vim_time() > last_বার্তা_time)
	    {
		last_বার্তা_time = vim_time();
		vim_snprintf((বর্ণ *)message, পরিমাণ(message),
			_("line %6d, word %6ld - %s"),
			   lnum, spin->si_foldwcount + spin->si_keepwcount, w);
		বার্তা_start();
		বার্তা_outtrans_long_attr(message, 0);
		বার্তা_clr_eos();
		বার্তা_didout = FALSE;
		বার্তা_col = 0;
		out_flush();
	    }
	}

	// Store the word in the hashtable to be able to find duplicates.
	dw = getroom_save(spin, w);
	যদি (dw == রিক্ত)
	{
	    retval = FAIL;
	    vim_free(pc);
	    ভঙ্গ;
	}

	hash = hash_hash(dw);
	hi = hash_lookup(&ht, dw, hash);
	যদি (!HASHITEM_EMPTY(hi))
	{
	    যদি (p_verbose > 0)
		smsg(_("Duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    অন্যথা যদি (duplicate == 0)
		smsg(_("First duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ++duplicate;
	}
	অন্যথা
	    hash_add_item(&ht, hi, dw, hash);

	flags = 0;
	store_afflist[0] = NUL;
	pfxlen = 0;
	totlen = 0;
	need_affix = FALSE;
	যদি (afflist != রিক্ত)
	{
	    // Extract flags from the affix list.
	    flags |= get_affix_flags(affile, afflist);

	    যদি (affile->af_needaffix != 0 && flag_in_afflist(
			  affile->af_flagtype, afflist, affile->af_needaffix))
		need_affix = TRUE;

	    যদি (affile->af_pfxpostpone)
	    {
		// Need to store the list of prefix IDs with the word.
		যদি (get_pfxlist(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    ভঙ্গ;
		}
		pfxlen = totlen;
	    }

	    যদি (spin->si_compflags != রিক্ত)
	    {
		// Need to store the list of compound flags with the word.
		// Concatenate them to the list of prefix IDs.
		যদি (get_compflags(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    ভঙ্গ;
		}
	    }
	}

	// Add the word to the word tree(s).
	যদি (store_word(spin, dw, flags, spin->si_region,
					   store_afflist, need_affix) == FAIL)
	    retval = FAIL;

	যদি (afflist != রিক্ত)
	{
	    // Find all matching suffixes and add the resulting words.
	    // Additionally do matching prefixes that combine.
	    যদি (store_aff_word(spin, dw, afflist, affile,
			   &affile->af_suff, &affile->af_pref,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;

	    // Find all matching prefixes and add the resulting words.
	    যদি (store_aff_word(spin, dw, afflist, affile,
			  &affile->af_pref, রিক্ত,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;
	}

	vim_free(pc);
    }

    যদি (duplicate > 0)
	smsg(_("%d duplicate word(s) in %s"), duplicate, fname);
    যদি (spin->si_ascii && non_ascii > 0)
	smsg(_("Ignored %d word(s) with non-ASCII characters in %s"),
							    non_ascii, fname);
    hash_clear(&ht);

    fclose(fd);
    প্রত্যাগম retval;
}

/*
 * Check for affix flags in "afflist" that are turned into word flags.
 * Return WF_ flags.
 */
    স্থির পূর্ণাঙ্ক
get_affix_flags(afffile_T *affile, char_u *afflist)
{
    পূর্ণাঙ্ক		flags = 0;

    যদি (affile->af_keepcase != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_keepcase))
	flags |= WF_KEEPCAP | WF_FIXCAP;
    যদি (affile->af_rare != 0 && flag_in_afflist(
			       affile->af_flagtype, afflist, affile->af_rare))
	flags |= WF_RARE;
    যদি (affile->af_bad != 0 && flag_in_afflist(
				affile->af_flagtype, afflist, affile->af_bad))
	flags |= WF_BANNED;
    যদি (affile->af_needcomp != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_needcomp))
	flags |= WF_NEEDCOMP;
    যদি (affile->af_comproot != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_comproot))
	flags |= WF_COMPROOT;
    যদি (affile->af_nosuggest != 0 && flag_in_afflist(
			  affile->af_flagtype, afflist, affile->af_nosuggest))
	flags |= WF_NOSUGGEST;
    প্রত্যাগম flags;
}

/*
 * Get the list of prefix IDs from the affix list "afflist".
 * Used for PFXPOSTPONE.
 * Put the resulting flags in "store_afflist[MAXWLEN]" with a terminating NUL.
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    স্থির পূর্ণাঙ্ক
get_pfxlist(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    পূর্ণাঙ্ক		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    পূর্ণাঙ্ক		id;
    char_u	চাবি[AH_চাবি_LEN];
    hashitem_T	*hi;

    প্রতি (p = afflist; *p != NUL; )
    {
	prevp = p;
	যদি (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a postponed prefix flag if it appears in "af_pref"
	    // and its ID is not zero.
	    vim_strncpy(চাবি, prevp, p - prevp);
	    hi = hash_find(&affile->af_pref, চাবি);
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		id = HI2AH(hi)->ah_newID;
		যদি (id != 0 && store_afflist_add(store_afflist, cntp, id) == FAIL)
		    প্রত্যাগম FAIL;
	    }
	}
	যদি (affile->af_flagtype == AFT_অঙ্ক && *p == ',')
	    ++p;
    }

    প্রত্যাগম OK;
}

/*
 * Get the list of compound IDs from the affix list "afflist" that are used
 * for compound words.
 * Puts the flags in "store_afflist[]".
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    স্থির পূর্ণাঙ্ক
get_compflags(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    পূর্ণাঙ্ক		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    char_u	চাবি[AH_চাবি_LEN];
    hashitem_T	*hi;

    প্রতি (p = afflist; *p != NUL; )
    {
	prevp = p;
	যদি (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a compound flag if it appears in "af_comp".
	    vim_strncpy(চাবি, prevp, p - prevp);
	    hi = hash_find(&affile->af_comp, চাবি);
	    যদি (!HASHITEM_EMPTY(hi)
		    && store_afflist_add(store_afflist, cntp,
					      HI2CI(hi)->ci_newID) == FAIL)
		প্রত্যাগম FAIL;
	}
	যদি (affile->af_flagtype == AFT_অঙ্ক && *p == ',')
	    ++p;
    }

    প্রত্যাগম OK;
}

/*
 * Apply affixes to a word and store the resulting words.
 * "ht" is the hashtable with affentry_T that need to be applied, either
 * prefixes or suffixes.
 * "xht", when not NULL, is the prefix hashtable, to be used additionally on
 * the resulting words for combining affixes.
 *
 * Returns FAIL when out of memory.
 */
    স্থির পূর্ণাঙ্ক
store_aff_word(
    spellinfo_T	*spin,		// spell info
    char_u	*word,		// basic word start
    char_u	*afflist,	// list of names of supported affixes
    afffile_T	*affile,
    hashtab_T	*ht,
    hashtab_T	*xht,
    পূর্ণাঙ্ক		condit,		// CONDIT_SUF et al.
    পূর্ণাঙ্ক		flags,		// flags for the word
    char_u	*pfxlist,	// list of prefix IDs
    পূর্ণাঙ্ক		pfxlen)		// nr of flags in "pfxlist" for prefixes, rest
				// is compound flags
{
    পূর্ণাঙ্ক		todo;
    hashitem_T	*hi;
    affheader_T	*ah;
    affentry_T	*ae;
    char_u	newword[MAXWLEN];
    পূর্ণাঙ্ক		retval = OK;
    পূর্ণাঙ্ক		i, j;
    char_u	*p;
    পূর্ণাঙ্ক		use_flags;
    char_u	*use_pfxlist;
    পূর্ণাঙ্ক		use_pfxlen;
    পূর্ণাঙ্ক		need_affix;
    char_u	store_afflist[MAXWLEN];
    char_u	pfx_pfxlist[MAXWLEN];
    size_t	wordlen = STRLEN(word);
    পূর্ণাঙ্ক		use_condit;

    todo = (পূর্ণাঙ্ক)ht->ht_used;
    প্রতি (hi = ht->ht_array; todo > 0 && retval == OK; ++hi)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ah = HI2AH(hi);

	    // Check that the affix combines, if required, and that the word
	    // supports this affix.
	    যদি (((condit & CONDIT_COMB) == 0 || ah->ah_combine)
		    && flag_in_afflist(affile->af_flagtype, afflist,
								 ah->ah_flag))
	    {
		// Loop over all affix entries with this name.
		প্রতি (ae = ah->ah_first; ae != রিক্ত; ae = ae->ae_next)
		{
		    // Check the condition.  It's not logical to match case
		    // here, but it is required for compatibility with
		    // Myspell.
		    // Another requirement from Myspell is that the chop
		    // string is shorter than the word itself.
		    // For prefixes, when "PFXPOSTPONE" was used, only do
		    // prefixes with a chop string and/or flags.
		    // When a previously added affix had CIRCUMFIX this one
		    // must have it too, if it had not then this one must not
		    // have one either.
		    যদি ((xht != রিক্ত || !affile->af_pfxpostpone
				|| ae->ae_chop != রিক্ত
				|| ae->ae_flags != রিক্ত)
			    && (ae->ae_chop == রিক্ত
				|| STRLEN(ae->ae_chop) < wordlen)
			    && (ae->ae_prog == রিক্ত
				|| vim_regexec_prog(&ae->ae_prog, FALSE,
							    word, (colnr_T)0))
			    && (((condit & CONDIT_CFIX) == 0)
				== ((condit & CONDIT_AFF) == 0
				    || ae->ae_flags == রিক্ত
				    || !flag_in_afflist(affile->af_flagtype,
					ae->ae_flags, affile->af_circumfix))))
		    {
			// Match.  Remove the chop and add the affix.
			যদি (xht == রিক্ত)
			{
			    // prefix: chop/add at the start of the word
			    যদি (ae->ae_add == রিক্ত)
				*newword = NUL;
			    অন্যথা
				vim_strncpy(newword, ae->ae_add, MAXWLEN - 1);
			    p = word;
			    যদি (ae->ae_chop != রিক্ত)
			    {
				// Skip chop string.
				যদি (has_mbyte)
				{
				    i = mb_charlen(ae->ae_chop);
				    প্রতি ( ; i > 0; --i)
					MB_PTR_ADV(p);
				}
				অন্যথা
				    p += STRLEN(ae->ae_chop);
			    }
			    // Note: this silently truncates the buffer, but this should
			    // not happen in practice
			    STRNCAT(newword, p, MAXWLEN - STRLEN(newword) - 1);
			}
			অন্যথা
			{
			    // suffix: chop/add at the end of the word
			    vim_strncpy(newword, word, MAXWLEN - 1);
			    যদি (ae->ae_chop != রিক্ত)
			    {
				// Remove chop string.
				p = newword + STRLEN(newword);
				i = (পূর্ণাঙ্ক)MB_CHARLEN(ae->ae_chop);
				প্রতি ( ; i > 0; --i)
				    MB_PTR_BACK(newword, p);
				*p = NUL;
			    }
			    যদি (ae->ae_add != রিক্ত)
				// Note: this silently truncates the buffer, but this should
				// not happen in practice
				STRNCAT(newword, ae->ae_add, MAXWLEN - STRLEN(newword) - 1);
			}

			use_flags = flags;
			use_pfxlist = pfxlist;
			use_pfxlen = pfxlen;
			need_affix = FALSE;
			use_condit = condit | CONDIT_COMB | CONDIT_AFF;
			যদি (ae->ae_flags != রিক্ত)
			{
			    // Extract flags from the affix list.
			    use_flags |= get_affix_flags(affile, ae->ae_flags);

			    যদি (affile->af_needaffix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_needaffix))
				need_affix = TRUE;

			    // When there is a CIRCUMFIX flag the other affix
			    // must also have it and we don't add the word
			    // with one affix.
			    যদি (affile->af_circumfix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_circumfix))
			    {
				use_condit |= CONDIT_CFIX;
				যদি ((condit & CONDIT_CFIX) == 0)
				    need_affix = TRUE;
			    }

			    যদি (affile->af_pfxpostpone
						|| spin->si_compflags != রিক্ত)
			    {
				পূর্ণাঙ্ক listlen = 0;

				যদি (affile->af_pfxpostpone)
				{
				    // Get prefix IDS from the affix list.
				    যদি (get_pfxlist(affile, ae->ae_flags,
						    store_afflist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					ভঙ্গ;
				    }
				    use_pfxlen = listlen;
				}
				অন্যথা
				    use_pfxlen = 0;
				use_pfxlist = store_afflist;

				// Combine the prefix IDs. Avoid adding the
				// same ID twice.
				প্রতি (i = 0; i < pfxlen; ++i)
				{
				    প্রতি (j = 0; j < use_pfxlen; ++j)
					যদি (pfxlist[i] == use_pfxlist[j])
					    ভঙ্গ;
				    যদি (j == use_pfxlen
					    && store_afflist_add(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					ভঙ্গ;
				    }
				    use_pfxlen = listlen;
				}
				যদি (retval == FAIL)
				    ভঙ্গ;

				যদি (spin->si_compflags != রিক্ত)
				    // Get compound IDS from the affix list.
				    যদি (get_compflags(affile, ae->ae_flags,
						      use_pfxlist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					ভঙ্গ;
				    }
				যদি (retval == FAIL)
				    ভঙ্গ;

				// Combine the list of compound flags.
				// Concatenate them to the prefix IDs list.
				// Avoid adding the same ID twice.
				প্রতি (i = pfxlen; pfxlist[i] != NUL; ++i)
				{
				    প্রতি (j = use_pfxlen;
						   use_pfxlist[j] != NUL; ++j)
					যদি (pfxlist[i] == use_pfxlist[j])
					    ভঙ্গ;
				    যদি (use_pfxlist[j] == NUL
					    && store_afflist_add(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					ভঙ্গ;
				    }
				}
				যদি (retval == FAIL)
				    ভঙ্গ;
			    }
			}

			// Obey a "COMPOUNDFORBIDFLAG" of the affix: don't
			// use the compound flags.
			যদি (use_pfxlist != রিক্ত && ae->ae_compforbid)
			{
			    vim_strncpy(pfx_pfxlist, use_pfxlist, use_pfxlen);
			    use_pfxlist = pfx_pfxlist;
			}

			// When there are postponed prefixes...
			যদি (spin->si_prefroot != রিক্ত
				&& spin->si_prefroot->wn_sibling != রিক্ত)
			{
			    // ... add a flag to indicate an affix was used.
			    use_flags |= WF_HAS_AFF;

			    // ... don't use a prefix list if combining
			    // affixes is not allowed.  But do use the
			    // compound flags after them.
			    যদি (!ah->ah_combine && use_pfxlist != রিক্ত)
				use_pfxlist += use_pfxlen;
			}

			// When compounding is supported and there is no
			// "COMPOUNDPERMITFLAG" then forbid compounding on the
			// side where the affix is applied.
			যদি (spin->si_compflags != রিক্ত && !ae->ae_comppermit)
			{
			    যদি (xht != রিক্ত)
				use_flags |= WF_NOCOMPAFT;
			    অন্যথা
				use_flags |= WF_NOCOMPBEF;
			}

			// Store the modified word.
			যদি (store_word(spin, newword, use_flags,
						 spin->si_region, use_pfxlist,
							  need_affix) == FAIL)
			    retval = FAIL;

			// When added a prefix or a first suffix and the affix
			// has flags may add a(nother) suffix.  RECURSIVE!
			যদি ((condit & CONDIT_SUF) && ae->ae_flags != রিক্ত)
			    যদি (store_aff_word(spin, newword, ae->ae_flags,
					affile, &affile->af_suff, xht,
					   use_condit & (xht == রিক্ত
							? ~0 :  ~CONDIT_SUF),
				      use_flags, use_pfxlist, pfxlen) == FAIL)
				retval = FAIL;

			// When added a suffix and combining is allowed also
			// try adding a prefix additionally.  Both for the
			// word flags and for the affix flags.  RECURSIVE!
			যদি (xht != রিক্ত && ah->ah_combine)
			{
			    যদি (store_aff_word(spin, newword,
					afflist, affile,
					xht, রিক্ত, use_condit,
					use_flags, use_pfxlist,
					pfxlen) == FAIL
				    || (ae->ae_flags != রিক্ত
					&& store_aff_word(spin, newword,
					    ae->ae_flags, affile,
					    xht, রিক্ত, use_condit,
					    use_flags, use_pfxlist,
					    pfxlen) == FAIL))
				retval = FAIL;
			}
		    }
		}
	    }
	}
    }

    প্রত্যাগম retval;
}

/*
 * Read a file with a list of words.
 */
    স্থির পূর্ণাঙ্ক
spell_read_wordfile(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    long	lnum = 0;
    char_u	rline[MAXLINELEN];
    char_u	*সারি;
    char_u	*pc = রিক্ত;
    char_u	*p;
    পূর্ণাঙ্ক		l;
    পূর্ণাঙ্ক		retval = OK;
    পূর্ণাঙ্ক		did_word = FALSE;
    পূর্ণাঙ্ক		non_ascii = 0;
    পূর্ণাঙ্ক		flags;
    পূর্ণাঙ্ক		regionmask;

    /*
     * Open the file.
     */
    fd = mch_fopen((বর্ণ *)fname, "r");
    যদি (fd == রিক্ত)
    {
	semsg(_(e_cant_open_file_str), fname);
	প্রত্যাগম FAIL;
    }

    vim_snprintf((বর্ণ *)IObuff, IOSIZE, _("Reading word file %s..."), fname);
    spell_message(spin, IObuff);

    /*
     * Read all the lines in the file one by one.
     */
    যাবত্ (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	সারি_breakcheck();
	++lnum;

	// Skip comment lines.
	যদি (*rline == '#')
	    অনুবর্ত;

	// Remove CR, LF and white space from the end.
	l = (পূর্ণাঙ্ক)STRLEN(rline);
	যাবত্ (l > 0 && rline[l - 1] <= ' ')
	    --l;
	যদি (l == 0)
	    অনুবর্ত;	// empty or blank line
	rline[l] = NUL;

	// Convert from "/encoding={encoding}" to 'encoding' when needed.
	vim_free(pc);
	যদি (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, রিক্ত);
	    যদি (pc == রিক্ত)
	    {
		smsg(_("Conversion failure for word in %s line %ld: %s"),
							   fname, lnum, rline);
		অনুবর্ত;
	    }
	    সারি = pc;
	}
	অন্যথা
	{
	    pc = রিক্ত;
	    সারি = rline;
	}

	যদি (*সারি == '/')
	{
	    ++সারি;
	    যদি (STRNCMP(সারি, "encoding=", 9) == 0)
	    {
		যদি (spin->si_conv.vc_type != CONV_NONE)
		    smsg(_("Duplicate /encoding= line ignored in %s line %ld: %s"),
						       fname, lnum, সারি - 1);
		অন্যথা যদি (did_word)
		    smsg(_("/encoding= line after word ignored in %s line %ld: %s"),
						       fname, lnum, সারি - 1);
		অন্যথা
		{
		    char_u	*enc;

		    // Setup for conversion to 'encoding'.
		    সারি += 9;
		    enc = enc_canonize(সারি);
		    যদি (enc != রিক্ত && !spin->si_ascii
			    && convert_setup(&spin->si_conv, enc,
							       p_enc) == FAIL)
			smsg(_("Conversion in %s not supported: from %s to %s"),
							  fname, সারি, p_enc);
		    vim_free(enc);
		    spin->si_conv.vc_fail = TRUE;
		}
		অনুবর্ত;
	    }

	    যদি (STRNCMP(সারি, "regions=", 8) == 0)
	    {
		যদি (spin->si_region_গুনতি > 1)
		    smsg(_("Duplicate /regions= line ignored in %s line %ld: %s"),
						       fname, lnum, সারি);
		অন্যথা
		{
		    সারি += 8;
		    যদি (STRLEN(সারি) > MAXREGIONS * 2)
			smsg(_("Too many regions in %s line %ld: %s"),
						       fname, lnum, সারি);
		    অন্যথা
		    {
			spin->si_region_গুনতি = (পূর্ণাঙ্ক)STRLEN(সারি) / 2;
			STRCPY(spin->si_region_name, সারি);

			// Adjust the mask for a word valid in all regions.
			spin->si_region = (1 << spin->si_region_গুনতি) - 1;
		    }
		}
		অনুবর্ত;
	    }

	    smsg(_("/ line ignored in %s line %ld: %s"),
						       fname, lnum, সারি - 1);
	    অনুবর্ত;
	}

	flags = 0;
	regionmask = spin->si_region;

	// Check for flags and region after a slash.
	p = vim_strchr(সারি, '/');
	যদি (p != রিক্ত)
	{
	    *p++ = NUL;
	    যাবত্ (*p != NUL)
	    {
		যদি (*p == '=')		// keep-case word
		    flags |= WF_KEEPCAP | WF_FIXCAP;
		অন্যথা যদি (*p == '!')	// Bad, bad, wicked word.
		    flags |= WF_BANNED;
		অন্যথা যদি (*p == '?')	// Rare word.
		    flags |= WF_RARE;
		অন্যথা যদি (VIM_ISDIGIT(*p)) // region number(s)
		{
		    যদি ((flags & WF_REGION) == 0)   // first one
			regionmask = 0;
		    flags |= WF_REGION;

		    l = *p - '0';
		    যদি (l == 0 || l > spin->si_region_গুনতি)
		    {
			smsg(_("Invalid region nr in %s line %ld: %s"),
							  fname, lnum, p);
			ভঙ্গ;
		    }
		    regionmask |= 1 << (l - 1);
		}
		অন্যথা
		{
		    smsg(_("Unrecognized flags in %s line %ld: %s"),
							      fname, lnum, p);
		    ভঙ্গ;
		}
		++p;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	যদি (spin->si_ascii && has_non_ascii(সারি))
	{
	    ++non_ascii;
	    অনুবর্ত;
	}

	// Normal word: store it.
	যদি (store_word(spin, সারি, flags, regionmask, রিক্ত, FALSE) == FAIL)
	{
	    retval = FAIL;
	    ভঙ্গ;
	}
	did_word = TRUE;
    }

    vim_free(pc);
    fclose(fd);

    যদি (spin->si_ascii && non_ascii > 0)
    {
	vim_snprintf((বর্ণ *)IObuff, IOSIZE,
		  _("Ignored %d words with non-ASCII characters"), non_ascii);
	spell_message(spin, IObuff);
    }

    প্রত্যাগম retval;
}

/*
 * Get part of an sblock_T, "len" bytes long.
 * This avoids calling free() for every little struct we use (and keeping
 * track of them).
 * The memory is cleared to all zeros.
 * Returns NULL when out of memory.
 */
    স্থির শূন্য *
getroom(
    spellinfo_T *spin,
    size_t	len,		// length needed
    পূর্ণাঙ্ক		align)		// align for pointer
{
    char_u	*p;
    sblock_T	*bl = spin->si_blocks;

    যদি (align && bl != রিক্ত)
	// Round size up for alignment.  On some systems structures need to be
	// aligned to the size of a pointer (e.g., SPARC).
	bl->sb_used = (bl->sb_used + পরিমাণ(বর্ণ *) - 1)
						      & ~(পরিমাণ(বর্ণ *) - 1);

    যদি (bl == রিক্ত || bl->sb_used + len > SBLOCKSIZE)
    {
	যদি (len >= SBLOCKSIZE)
	    bl = রিক্ত;
	অন্যথা
	    // Allocate a block of memory. It is not freed until much later.
	    bl = alloc_clear(offsetof(sblock_T, sb_data) + SBLOCKSIZE + 1);
	যদি (bl == রিক্ত)
	{
	    যদি (!spin->si_did_emsg)
	    {
		emsg(_(e_insufficient_memory_word_list_will_be_incomplete));
		spin->si_did_emsg = TRUE;
	    }
	    প্রত্যাগম রিক্ত;
	}
	bl->sb_next = spin->si_blocks;
	spin->si_blocks = bl;
	bl->sb_used = 0;
	++spin->si_blocks_cnt;
    }

    p = bl->sb_data + bl->sb_used;
    bl->sb_used += (পূর্ণাঙ্ক)len;

    প্রত্যাগম p;
}

/*
 * Make a copy of a string into memory allocated with getroom().
 * Returns NULL when out of memory.
 */
    স্থির char_u *
getroom_save(spellinfo_T *spin, char_u *s)
{
    char_u	*sc;

    sc = (char_u *)getroom(spin, STRLEN(s) + 1, FALSE);
    যদি (sc != রিক্ত)
	STRCPY(sc, s);
    প্রত্যাগম sc;
}


/*
 * Free the list of allocated sblock_T.
 */
    স্থির শূন্য
free_blocks(sblock_T *bl)
{
    sblock_T	*next;

    যাবত্ (bl != রিক্ত)
    {
	next = bl->sb_next;
	vim_free(bl);
	bl = next;
    }
}

/*
 * Allocate the root of a word tree.
 * Returns NULL when out of memory.
 */
    স্থির wordnode_T *
wordtree_alloc(spellinfo_T *spin)
{
    প্রত্যাগম (wordnode_T *)getroom(spin, পরিমাণ(wordnode_T), TRUE);
}

/*
 * Return TRUE if "word" contains valid word characters.
 * Control characters and trailing '/' are invalid.  Space is OK.
 */
    স্থির পূর্ণাঙ্ক
valid_spell_word(char_u *word, char_u *end)
{
    char_u *p;

    যদি (enc_utf8 && !utf_valid_string(word, end))
	প্রত্যাগম FALSE;
    প্রতি (p = word; *p != NUL && p < end; p += mb_ptr2len(p))
	যদি (*p < ' ' || (p[0] == '/' && p[1] == NUL))
	    প্রত্যাগম FALSE;
    প্রত্যাগম TRUE;
}

/*
 * Store a word in the tree(s).
 * Always store it in the case-folded tree.  For a keep-case word this is
 * useful when the word can also be used with all caps (no WF_FIXCAP flag) and
 * used to find suggestions.
 * For a keep-case word also store it in the keep-case tree.
 * When "pfxlist" is not NULL store the word for each postponed prefix ID and
 * compound flag.
 */
    স্থির পূর্ণাঙ্ক
store_word(
    spellinfo_T	*spin,
    char_u	*word,
    পূর্ণাঙ্ক		flags,		// extra flags, WF_BANNED
    পূর্ণাঙ্ক		region,		// supported region(s)
    char_u	*pfxlist,	// list of prefix IDs or NULL
    পূর্ণাঙ্ক		need_affix)	// only store word with affix ID
{
    পূর্ণাঙ্ক		len = (পূর্ণাঙ্ক)STRLEN(word);
    পূর্ণাঙ্ক		ct = captype(word, word + len);
    char_u	foldword[MAXWLEN];
    পূর্ণাঙ্ক		res = OK;
    char_u	*p;

    // Avoid adding illegal bytes to the word tree.
    যদি (!valid_spell_word(word, word + len))
	প্রত্যাগম FAIL;

    (শূন্য)spell_casefold(curwin, word, len, foldword, MAXWLEN);
    প্রতি (p = pfxlist; res == OK; ++p)
    {
	যদি (!need_affix || (p != রিক্ত && *p != NUL))
	    res = বৃক্ষ_add_word(spin, foldword, spin->si_foldroot, ct | flags,
						  region, p == রিক্ত ? 0 : *p);
	যদি (p == রিক্ত || *p == NUL)
	    ভঙ্গ;
    }
    ++spin->si_foldwcount;

    যদি (res == OK && (ct == WF_KEEPCAP || (flags & WF_KEEPCAP)))
    {
	প্রতি (p = pfxlist; res == OK; ++p)
	{
	    যদি (!need_affix || (p != রিক্ত && *p != NUL))
		res = বৃক্ষ_add_word(spin, word, spin->si_keeproot, flags,
						  region, p == রিক্ত ? 0 : *p);
	    যদি (p == রিক্ত || *p == NUL)
		ভঙ্গ;
	}
	++spin->si_keepwcount;
    }
    প্রত্যাগম res;
}

/*
 * Add word "word" to a word tree at "root".
 * When "flags" < 0 we are adding to the prefix tree where "flags" is used for
 * "rare" and "region" is the condition nr.
 * Returns FAIL when out of memory.
 */
    স্থির পূর্ণাঙ্ক
বৃক্ষ_add_word(
    spellinfo_T	*spin,
    char_u	*word,
    wordnode_T	*মূল,
    পূর্ণাঙ্ক		flags,
    পূর্ণাঙ্ক		region,
    পূর্ণাঙ্ক		affixID)
{
    wordnode_T	*গ্রন্থি = মূল;
    wordnode_T	*np;
    wordnode_T	*copyp, **copyprev;
    wordnode_T	**prev = রিক্ত;
    পূর্ণাঙ্ক		i;

    // Add each byte of the word to the tree, including the NUL at the end.
    প্রতি (i = 0; ; ++i)
    {
	// When there is more than one reference to this node we need to make
	// a copy, so that we can modify it.  Copy the whole list of siblings
	// (we don't optimize for a partly shared list of siblings).
	যদি (গ্রন্থি != রিক্ত && গ্রন্থি->wn_refs > 1)
	{
	    --গ্রন্থি->wn_refs;
	    copyprev = prev;
	    FOR_ALL_গ্রন্থি_SIBLINGS(গ্রন্থি, copyp)
	    {
		// Allocate a new node and copy the info.
		np = get_wordnode(spin);
		যদি (np == রিক্ত)
		    প্রত্যাগম FAIL;
		np->wn_child = copyp->wn_child;
		যদি (np->wn_child != রিক্ত)
		    ++np->wn_child->wn_refs;	// child gets extra ref
		np->wn_byte = copyp->wn_byte;
		যদি (np->wn_byte == NUL)
		{
		    np->wn_flags = copyp->wn_flags;
		    np->wn_region = copyp->wn_region;
		    np->wn_affixID = copyp->wn_affixID;
		}

		// Link the new node in the list, there will be one ref.
		np->wn_refs = 1;
		যদি (copyprev != রিক্ত)
		    *copyprev = np;
		copyprev = &np->wn_sibling;

		// Let "node" point to the head of the copied list.
		যদি (copyp == গ্রন্থি)
		    গ্রন্থি = np;
	    }
	}

	// Look for the sibling that has the same character.  They are sorted
	// on byte value, thus stop searching when a sibling is found with a
	// higher byte value.  For zero bytes (end of word) the sorting is
	// done on flags and then on affixID.
	যাবত্ (গ্রন্থি != রিক্ত
		&& (গ্রন্থি->wn_byte < word[i]
		    || (গ্রন্থি->wn_byte == NUL
			&& (flags < 0
			    ? গ্রন্থি->wn_affixID < (unsigned)affixID
			    : (গ্রন্থি->wn_flags < (unsigned)(flags & WN_MASK)
				|| (গ্রন্থি->wn_flags == (flags & WN_MASK)
				    && (spin->si_sugtree
					? (গ্রন্থি->wn_region & 0xffff) < region
					: গ্রন্থি->wn_affixID
						    < (unsigned)affixID)))))))
	{
	    prev = &গ্রন্থি->wn_sibling;
	    গ্রন্থি = *prev;
	}
	যদি (গ্রন্থি == রিক্ত
		|| গ্রন্থি->wn_byte != word[i]
		|| (word[i] == NUL
		    && (flags < 0
			|| spin->si_sugtree
			|| গ্রন্থি->wn_flags != (flags & WN_MASK)
			|| গ্রন্থি->wn_affixID != affixID)))
	{
	    // Allocate a new node.
	    np = get_wordnode(spin);
	    যদি (np == রিক্ত)
		প্রত্যাগম FAIL;
	    np->wn_byte = word[i];

	    // If "node" is NULL this is a new child or the end of the sibling
	    // list: ref count is one.  Otherwise use ref count of sibling and
	    // make ref count of sibling one (matters when inserting in front
	    // of the list of siblings).
	    যদি (গ্রন্থি == রিক্ত)
		np->wn_refs = 1;
	    অন্যথা
	    {
		np->wn_refs = গ্রন্থি->wn_refs;
		গ্রন্থি->wn_refs = 1;
	    }
	    যদি (prev != রিক্ত)
		*prev = np;
	    np->wn_sibling = গ্রন্থি;
	    গ্রন্থি = np;
	}

	যদি (word[i] == NUL)
	{
	    গ্রন্থি->wn_flags = flags;
	    গ্রন্থি->wn_region |= region;
	    গ্রন্থি->wn_affixID = affixID;
	    ভঙ্গ;
	}
	prev = &গ্রন্থি->wn_child;
	গ্রন্থি = *prev;
    }
#ifdef SPELL_PRINTTREE
    smsg("Added \"%s\"", word);
    spell_print_বৃক্ষ(মূল->wn_sibling);
#endif

    // count nr of words added since last message
    ++spin->si_বার্তা_গুনতি;

    যদি (spin->si_compress_cnt > 1)
    {
	যদি (--spin->si_compress_cnt == 1)
	    // Did enough words to lower the block count limit.
	    spin->si_blocks_cnt += compress_inc;
    }

    /*
     * When we have allocated lots of memory we need to compress the word tree
     * to free up some room.  But compression is slow, and we might actually
     * need that room, thus only compress in the following situations:
     * 1. When not compressed before (si_compress_cnt == 0): when using
     *    "compress_start" blocks.
     * 2. When compressed before and used "compress_inc" blocks before
     *    adding "compress_added" words (si_compress_cnt > 1).
     * 3. When compressed before, added "compress_added" words
     *    (si_compress_cnt == 1) and the number of free nodes drops below the
     *    maximum word length.
     */
#ifndef SPELL_COMPRESS_ALWAYS
    যদি (spin->si_compress_cnt == 1
	    ? spin->si_free_গুনতি < MAXWLEN
	    : spin->si_blocks_cnt >= compress_start)
#endif
    {
	// Decrement the block counter.  The effect is that we compress again
	// when the freed up room has been used and another "compress_inc"
	// blocks have been allocated.  Unless "compress_added" words have
	// been added, then the limit is put back again.
	spin->si_blocks_cnt -= compress_inc;
	spin->si_compress_cnt = compress_added;

	যদি (spin->si_verbose)
	{
	    বার্তা_start();
	    বার্তা_puts(_(বার্তা_compressing));
	    বার্তা_clr_eos();
	    বার্তা_didout = FALSE;
	    বার্তা_col = 0;
	    out_flush();
	}

	// Compress both trees.  Either they both have many nodes, which makes
	// compression useful, or one of them is small, which means
	// compression goes fast.  But when filling the soundfold word tree
	// there is no keep-case tree.
	wordtree_compress(spin, spin->si_foldroot, "case-folded");
	যদি (affixID >= 0)
	    wordtree_compress(spin, spin->si_keeproot, "keep-case");
    }

    প্রত্যাগম OK;
}

/*
 * Get a wordnode_T, either from the list of previously freed nodes or
 * allocate a new one.
 * Returns NULL when out of memory.
 */
    স্থির wordnode_T *
get_wordnode(spellinfo_T *spin)
{
    wordnode_T *n;

    যদি (spin->si_first_free == রিক্ত)
	n = (wordnode_T *)getroom(spin, পরিমাণ(wordnode_T), TRUE);
    অন্যথা
    {
	n = spin->si_first_free;
	spin->si_first_free = n->wn_child;
	CLEAR_POINTER(n);
	--spin->si_free_গুনতি;
    }
#ifdef SPELL_PRINTTREE
    যদি (n != রিক্ত)
	n->wn_nr = ++spin->si_wordnode_nr;
#endif
    প্রত্যাগম n;
}

/*
 * Decrement the reference count on a node (which is the head of a list of
 * siblings).  If the reference count becomes zero free the node and its
 * siblings.
 * Returns the number of nodes actually freed.
 */
    স্থির পূর্ণাঙ্ক
deref_wordnode(spellinfo_T *spin, wordnode_T *গ্রন্থি)
{
    wordnode_T	*np;
    পূর্ণাঙ্ক		cnt = 0;

    যদি (--গ্রন্থি->wn_refs == 0)
    {
	FOR_ALL_গ্রন্থি_SIBLINGS(গ্রন্থি, np)
	{
	    যদি (np->wn_child != রিক্ত)
		cnt += deref_wordnode(spin, np->wn_child);
	    free_wordnode(spin, np);
	    ++cnt;
	}
	++cnt;	    // length field
    }
    প্রত্যাগম cnt;
}

/*
 * Free a wordnode_T for re-use later.
 * Only the "wn_child" field becomes invalid.
 */
    স্থির শূন্য
free_wordnode(spellinfo_T *spin, wordnode_T *n)
{
    n->wn_child = spin->si_first_free;
    spin->si_first_free = n;
    ++spin->si_free_গুনতি;
}

/*
 * Compress a tree: find tails that are identical and can be shared.
 */
    স্থির শূন্য
wordtree_compress(spellinfo_T *spin, wordnode_T *মূল, বর্ণ *name)
{
    hashtab_T	    ht;
    long	    n;
    long	    tot = 0;
    long	    perc;

    // Skip the root itself, it's not actually used.  The first sibling is the
    // start of the tree.
    যদি (মূল->wn_sibling == রিক্ত)
	প্রত্যাগম;

    hash_init(&ht);
    n = গ্রন্থি_compress(spin, মূল->wn_sibling, &ht, &tot);

#ifndef SPELL_PRINTTREE
    যদি (spin->si_verbose || p_verbose > 2)
#endif
    {
	যদি (tot > 1000000)
	    perc = (tot - n) / (tot / 100);
	অন্যথা যদি (tot == 0)
	    perc = 0;
	অন্যথা
	    perc = (tot - n) * 100 / tot;
	vim_snprintf((বর্ণ *)IObuff, IOSIZE,
		_("Compressed %s: %ld of %ld nodes; %ld (%ld%%) remaining"),
		name, n, tot, tot - n, perc);
	spell_message(spin, IObuff);
    }
#ifdef SPELL_PRINTTREE
    spell_print_বৃক্ষ(মূল->wn_sibling);
#endif
    hash_clear(&ht);
}

/*
 * Compress a node, its siblings and its children, depth first.
 * Returns the number of compressed nodes.
 */
    স্থির long
গ্রন্থি_compress(
    spellinfo_T	*spin,
    wordnode_T	*গ্রন্থি,
    hashtab_T	*ht,
    long	*tot)	    // total count of nodes before compressing,
			    // incremented while going through the tree
{
    wordnode_T	*np;
    wordnode_T	*tp;
    wordnode_T	*child;
    hash_T	hash;
    hashitem_T	*hi;
    long	len = 0;
    unsigned	nr, n;
    long	compressed = 0;

    /*
     * Go through the list of siblings.  Compress each child and then try
     * finding an identical child to replace it.
     * Note that with "child" we mean not just the node that is pointed to,
     * but the whole list of siblings of which the child node is the first.
     */
    প্রতি (np = গ্রন্থি; np != রিক্ত && !got_int; np = np->wn_sibling)
    {
	++len;
	যদি ((child = np->wn_child) != রিক্ত)
	{
	    // Compress the child first.  This fills hashkey.
	    compressed += গ্রন্থি_compress(spin, child, ht, tot);

	    // Try to find an identical child.
	    hash = hash_hash(child->wn_u1.hashkey);
	    hi = hash_lookup(ht, child->wn_u1.hashkey, hash);
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		// There are children we encountered before with a hash value
		// identical to the current child.  Now check if there is one
		// that is really identical.
		প্রতি (tp = HI2WN(hi); tp != রিক্ত; tp = tp->wn_u2.next)
		    যদি (গ্রন্থি_equal(child, tp))
		    {
			// Found one!  Now use that child in place of the
			// current one.  This means the current child and all
			// its siblings is unlinked from the tree.
			++tp->wn_refs;
			compressed += deref_wordnode(spin, child);
			np->wn_child = tp;
			ভঙ্গ;
		    }
		যদি (tp == রিক্ত)
		{
		    // No other child with this hash value equals the child of
		    // the node, add it to the linked list after the first
		    // item.
		    tp = HI2WN(hi);
		    child->wn_u2.next = tp->wn_u2.next;
		    tp->wn_u2.next = child;
		}
	    }
	    অন্যথা
		// No other child has this hash value, add it to the
		// hashtable.
		hash_add_item(ht, hi, child->wn_u1.hashkey, hash);
	}
    }
    *tot += len + 1;	// add one for the node that stores the length

    /*
     * Make a hash key for the node and its siblings, so that we can quickly
     * find a lookalike node.  This must be done after compressing the sibling
     * list, otherwise the hash key would become invalid by the compression.
     */
    গ্রন্থি->wn_u1.hashkey[0] = len;
    nr = 0;
    FOR_ALL_গ্রন্থি_SIBLINGS(গ্রন্থি, np)
    {
	যদি (np->wn_byte == NUL)
	    // end node: use wn_flags, wn_region and wn_affixID
	    n = np->wn_flags + (np->wn_region << 8) + (np->wn_affixID << 16);
	অন্যথা
	    // byte node: use the byte value and the child pointer
	    n = (unsigned)(np->wn_byte + ((long_u)np->wn_child << 8));
	nr = nr * 101 + n;
    }

    // Avoid NUL bytes, it terminates the hash key.
    n = nr & 0xff;
    গ্রন্থি->wn_u1.hashkey[1] = n == 0 ? 1 : n;
    n = (nr >> 8) & 0xff;
    গ্রন্থি->wn_u1.hashkey[2] = n == 0 ? 1 : n;
    n = (nr >> 16) & 0xff;
    গ্রন্থি->wn_u1.hashkey[3] = n == 0 ? 1 : n;
    n = (nr >> 24) & 0xff;
    গ্রন্থি->wn_u1.hashkey[4] = n == 0 ? 1 : n;
    গ্রন্থি->wn_u1.hashkey[5] = NUL;

    // Check for CTRL-C pressed now and then.
    veryfast_breakcheck();

    প্রত্যাগম compressed;
}

/*
 * Return TRUE when two nodes have identical siblings and children.
 */
    স্থির পূর্ণাঙ্ক
গ্রন্থি_equal(wordnode_T *n1, wordnode_T *n2)
{
    wordnode_T	*p1;
    wordnode_T	*p2;

    প্রতি (p1 = n1, p2 = n2; p1 != রিক্ত && p2 != রিক্ত;
				     p1 = p1->wn_sibling, p2 = p2->wn_sibling)
	যদি (p1->wn_byte != p2->wn_byte
		|| (p1->wn_byte == NUL
		    ? (p1->wn_flags != p2->wn_flags
			|| p1->wn_region != p2->wn_region
			|| p1->wn_affixID != p2->wn_affixID)
		    : (p1->wn_child != p2->wn_child)))
	    ভঙ্গ;

    প্রত্যাগম p1 == রিক্ত && p2 == রিক্ত;
}

স্থির পূর্ণাঙ্ক rep_compare(ধ্রুব শূন্য *s1, ধ্রুব শূন্য *s2);

/*
 * Function given to qsort() to sort the REP items on "from" string.
 */
    স্থির পূর্ণাঙ্ক
rep_compare(ধ্রুব শূন্য *s1, ধ্রুব শূন্য *s2)
{
    fromto_T	*p1 = (fromto_T *)s1;
    fromto_T	*p2 = (fromto_T *)s2;

    প্রত্যাগম STRCMP(p1->ft_from, p2->ft_from);
}

/*
 * Write the Vim .spl file "fname".
 * Return FAIL or OK;
 */
    স্থির পূর্ণাঙ্ক
write_vim_spell(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    পূর্ণাঙ্ক		regionmask;
    পূর্ণাঙ্ক		round;
    wordnode_T	*বৃক্ষ;
    পূর্ণাঙ্ক		nodecount;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		l;
    garray_T	*gap;
    fromto_T	*ftp;
    char_u	*p;
    পূর্ণাঙ্ক		rr;
    পূর্ণাঙ্ক		retval = OK;
    size_t	fwv = 1;  // collect return value of fwrite() to avoid
			  // warnings from picky compiler

    fd = mch_fopen((বর্ণ *)fname, "w");
    যদি (fd == রিক্ত)
    {
	semsg(_(e_cant_open_file_str), fname);
	প্রত্যাগম FAIL;
    }

    // <HEADER>: <fileID> <versionnr>
							    // <fileID>
    fwv &= fwrite(VIMSPELLMAGIC, VIMSPELLMAGICL, (size_t)1, fd);
    যদি (fwv != (size_t)1)
	// Catch first write error, don't try writing more.
	গচ্ছ theend;

    putc(VIMSPELLVERSION, fd);				    // <versionnr>

    /*
     * <SECTIONS>: <section> ... <sectionend>
     */

    // SN_INFO: <infotext>
    যদি (spin->si_info != রিক্ত)
    {
	putc(SN_INFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	i = (পূর্ণাঙ্ক)STRLEN(spin->si_info);
	put_bytes(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_info, (size_t)i, (size_t)1, fd); // <infotext>
    }

    // SN_REGION: <regionname> ...
    // Write the region names only if there is more than one.
    যদি (spin->si_region_গুনতি > 1)
    {
	putc(SN_REGION, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>
	l = spin->si_region_গুনতি * 2;
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_region_name, (size_t)l, (size_t)1, fd);
							// <regionname> ...
	regionmask = (1 << spin->si_region_গুনতি) - 1;
    }
    অন্যথা
	regionmask = 0;

    // SN_CHARFLAGS: <charflagslen> <charflags> <folcharslen> <folchars>
    //
    // The table with character flags and the table for case folding.
    // This makes sure the same characters are recognized as word characters
    // when generating and when using a spell file.
    // Skip this for ASCII, the table may conflict with the one used for
    // 'encoding'.
    // Also skip this for an .add.spl file, the main spell file must contain
    // the table (avoids that it conflicts).  File is shorter too.
    যদি (!spin->si_ascii && !spin->si_add)
    {
	char_u	folchars[128 * 8];
	পূর্ণাঙ্ক	flags;

	putc(SN_CHARFLAGS, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	// Form the <folchars> string first, we need to know its length.
	l = 0;
	প্রতি (i = 128; i < 256; ++i)
	{
	    যদি (has_mbyte)
		l += mb_char2bytes(spelltab.st_fold[i], folchars + l);
	    অন্যথা
		folchars[l++] = spelltab.st_fold[i];
	}
	put_bytes(fd, (long_u)(1 + 128 + 2 + l), 4);	// <sectionlen>

	fputc(128, fd);					// <charflagslen>
	প্রতি (i = 128; i < 256; ++i)
	{
	    flags = 0;
	    যদি (spelltab.st_isw[i])
		flags |= CF_WORD;
	    যদি (spelltab.st_isu[i])
		flags |= CF_UPPER;
	    fputc(flags, fd);				// <charflags>
	}

	put_bytes(fd, (long_u)l, 2);			// <folcharslen>
	fwv &= fwrite(folchars, (size_t)l, (size_t)1, fd); // <folchars>
    }

    // SN_MIDWORD: <midword>
    যদি (spin->si_midword != রিক্ত)
    {
	putc(SN_MIDWORD, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	i = (পূর্ণাঙ্ক)STRLEN(spin->si_midword);
	put_bytes(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_midword, (size_t)i, (size_t)1, fd);
							// <midword>
    }

    // SN_PREFCOND: <prefcondcnt> <prefcond> ...
    যদি (spin->si_prefcond.ga_len > 0)
    {
	putc(SN_PREFCOND, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	l = write_spell_prefcond(রিক্ত, &spin->si_prefcond, &fwv);
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>

	write_spell_prefcond(fd, &spin->si_prefcond, &fwv);
    }

    // SN_REP: <repcount> <rep> ...
    // SN_SAL: <salflags> <salcount> <sal> ...
    // SN_REPSAL: <repcount> <rep> ...

    // round 1: SN_REP section
    // round 2: SN_SAL section (unless SN_SOFO is used)
    // round 3: SN_REPSAL section
    প্রতি (round = 1; round <= 3; ++round)
    {
	যদি (round == 1)
	    gap = &spin->si_rep;
	অন্যথা যদি (round == 2)
	{
	    // Don't write SN_SAL when using a SN_SOFO section
	    যদি (spin->si_sofofr != রিক্ত && spin->si_sofoto != রিক্ত)
		অনুবর্ত;
	    gap = &spin->si_sal;
	}
	অন্যথা
	    gap = &spin->si_repsal;

	// Don't write the section if there are no items.
	যদি (gap->ga_len == 0)
	    অনুবর্ত;

	// Sort the REP/REPSAL items.
	যদি (round != 2)
	    qsort(gap->ga_data, (size_t)gap->ga_len,
					       পরিমাণ(fromto_T), rep_compare);

	i = round == 1 ? SN_REP : (round == 2 ? SN_SAL : SN_REPSAL);
	putc(i, fd);					// <sectionID>

	// This is for making suggestions, section is not required.
	putc(0, fd);					// <sectionflags>

	// Compute the length of what follows.
	l = 2;	    // count <repcount> or <salcount>
	প্রতি (i = 0; i < gap->ga_len; ++i)
	{
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    l += 1 + (পূর্ণাঙ্ক)STRLEN(ftp->ft_from);  // count <*fromlen> and <*from>
	    l += 1 + (পূর্ণাঙ্ক)STRLEN(ftp->ft_to);    // count <*tolen> and <*to>
	}
	যদি (round == 2)
	    ++l;	// count <salflags>
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>

	যদি (round == 2)
	{
	    i = 0;
	    যদি (spin->si_followup)
		i |= SAL_F0LLOWUP;
	    যদি (spin->si_collapse)
		i |= SAL_COLLAPSE;
	    যদি (spin->si_rem_accents)
		i |= SAL_REM_ACCENTS;
	    putc(i, fd);			// <salflags>
	}

	put_bytes(fd, (long_u)gap->ga_len, 2);	// <repcount> or <salcount>
	প্রতি (i = 0; i < gap->ga_len; ++i)
	{
	    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
	    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    প্রতি (rr = 1; rr <= 2; ++rr)
	    {
		p = rr == 1 ? ftp->ft_from : ftp->ft_to;
		l = (পূর্ণাঙ্ক)STRLEN(p);
		putc(l, fd);
		যদি (l > 0)
		    fwv &= fwrite(p, l, (size_t)1, fd);
	    }
	}

    }

    // SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
    // This is for making suggestions, section is not required.
    যদি (spin->si_sofofr != রিক্ত && spin->si_sofoto != রিক্ত)
    {
	putc(SN_SOFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (পূর্ণাঙ্ক)STRLEN(spin->si_sofofr);
	put_bytes(fd, (long_u)(l + STRLEN(spin->si_sofoto) + 4), 4);
							// <sectionlen>

	put_bytes(fd, (long_u)l, 2);			// <sofofromlen>
	fwv &= fwrite(spin->si_sofofr, l, (size_t)1, fd); // <sofofrom>

	l = (পূর্ণাঙ্ক)STRLEN(spin->si_sofoto);
	put_bytes(fd, (long_u)l, 2);			// <sofotolen>
	fwv &= fwrite(spin->si_sofoto, l, (size_t)1, fd); // <sofoto>
    }

    // SN_WORDS: <word> ...
    // This is for making suggestions, section is not required.
    যদি (spin->si_commonwords.ht_used > 0)
    {
	putc(SN_শব্দগুলি, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// round 1: count the bytes
	// round 2: write the bytes
	প্রতি (round = 1; round <= 2; ++round)
	{
	    পূর্ণাঙ্ক		todo;
	    পূর্ণাঙ্ক		len = 0;
	    hashitem_T	*hi;

	    todo = (পূর্ণাঙ্ক)spin->si_commonwords.ht_used;
	    FOR_ALL_HASHTAB_বস্তুসমূহ(&spin->si_commonwords, hi, todo)
		যদি (!HASHITEM_EMPTY(hi))
		{
		    l = (পূর্ণাঙ্ক)STRLEN(hi->hi_চাবি) + 1;
		    len += l;
		    যদি (round == 2)			// <word>
			fwv &= fwrite(hi->hi_চাবি, (size_t)l, (size_t)1, fd);
		    --todo;
		}
	    যদি (round == 1)
		put_bytes(fd, (long_u)len, 4);		// <sectionlen>
	}
    }

    // SN_MAP: <mapstr>
    // This is for making suggestions, section is not required.
    যদি (spin->si_map.ga_len > 0)
    {
	putc(SN_MAP, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>
	l = spin->si_map.ga_len;
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_map.ga_data, (size_t)l, (size_t)1, fd);
							// <mapstr>
    }

    // SN_SUGFILE: <timestamp>
    // This is used to notify that a .sug file may be available and at the
    // same time allows for checking that a .sug file that is found matches
    // with this .spl file.  That's because the word numbers must be exactly
    // right.
    যদি (!spin->si_nosugfile
	    && (spin->si_sal.ga_len > 0
		     || (spin->si_sofofr != রিক্ত && spin->si_sofoto != রিক্ত)))
    {
	putc(SN_SUGFILE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)8, 4);			// <sectionlen>

	// Set si_sugtime and write it to the file.
	spin->si_sugtime = time(রিক্ত);
	put_time(fd, spin->si_sugtime);			// <timestamp>
    }

    // SN_NOSPLITSUGS: nothing
    // This is used to notify that no suggestions with word splits are to be
    // made.
    যদি (spin->si_nosplitsugs)
    {
	putc(SN_NOSPLITSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_NOCOMPUNDSUGS: nothing
    // This is used to notify that no suggestions with compounds are to be
    // made.
    যদি (spin->si_nocompoundsugs)
    {
	putc(SN_NOCOMPOUNDSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_COMPOUND: compound info.
    // We don't mark it required, when not supported all compound words will
    // be bad words.
    যদি (spin->si_compflags != রিক্ত)
    {
	putc(SN_COMPOUND, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (পূর্ণাঙ্ক)STRLEN(spin->si_compflags);
	প্রতি (i = 0; i < spin->si_comppat.ga_len; ++i)
	    l += (পূর্ণাঙ্ক)STRLEN(((char_u **)(spin->si_comppat.ga_data))[i]) + 1;
	put_bytes(fd, (long_u)(l + 7), 4);		// <sectionlen>

	putc(spin->si_compmax, fd);			// <compmax>
	putc(spin->si_compminlen, fd);			// <compminlen>
	putc(spin->si_compsylmax, fd);			// <compsylmax>
	putc(0, fd);		// for Vim 7.0b compatibility
	putc(spin->si_compoptions, fd);			// <compoptions>
	put_bytes(fd, (long_u)spin->si_comppat.ga_len, 2);
							// <comppatcount>
	প্রতি (i = 0; i < spin->si_comppat.ga_len; ++i)
	{
	    p = ((char_u **)(spin->si_comppat.ga_data))[i];
	    putc((পূর্ণাঙ্ক)STRLEN(p), fd);			// <comppatlen>
	    fwv &= fwrite(p, (size_t)STRLEN(p), (size_t)1, fd);
							// <comppattext>
	}
							// <compflags>
	fwv &= fwrite(spin->si_compflags, (size_t)STRLEN(spin->si_compflags),
							       (size_t)1, fd);
    }

    // SN_NOBREAK: NOBREAK flag
    যদি (spin->si_nobreak)
    {
	putc(SN_NOBREAK, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// It's empty, the presence of the section flags the feature.
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_SYLLABLE: syllable info.
    // We don't mark it required, when not supported syllables will not be
    // counted.
    যদি (spin->si_syllable != রিক্ত)
    {
	putc(SN_SYLLABLE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (পূর্ণাঙ্ক)STRLEN(spin->si_syllable);
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_syllable, (size_t)l, (size_t)1, fd);
							// <syllable>
    }

    // end of <SECTIONS>
    putc(SN_END, fd);					// <sectionend>


    /*
     * <LWORDTREE>  <KWORDTREE>  <PREFIXTREE>
     */
    spin->si_memtot = 0;
    প্রতি (round = 1; round <= 3; ++round)
    {
	যদি (round == 1)
	    বৃক্ষ = spin->si_foldroot->wn_sibling;
	অন্যথা যদি (round == 2)
	    বৃক্ষ = spin->si_keeproot->wn_sibling;
	অন্যথা
	    বৃক্ষ = spin->si_prefroot->wn_sibling;

	// Clear the index and wnode fields in the tree.
	clear_গ্রন্থি(বৃক্ষ);

	// Count the number of nodes.  Needed to be able to allocate the
	// memory when reading the nodes.  Also fills in index for shared
	// nodes.
	nodecount = put_গ্রন্থি(রিক্ত, বৃক্ষ, 0, regionmask, round == 3);

	// number of nodes in 4 bytes
	put_bytes(fd, (long_u)nodecount, 4);	// <nodecount>
	spin->si_memtot += nodecount + nodecount * পরিমাণ(পূর্ণাঙ্ক);

	// Write the nodes.
	(শূন্য)put_গ্রন্থি(fd, বৃক্ষ, 0, regionmask, round == 3);
    }

    // Write another byte to check for errors (file system full).
    যদি (putc(0, fd) == EOF)
	retval = FAIL;
theend:
    যদি (fclose(fd) == EOF)
	retval = FAIL;

    যদি (fwv != (size_t)1)
	retval = FAIL;
    যদি (retval == FAIL)
	emsg(_(e_error_while_writing));

    প্রত্যাগম retval;
}

/*
 * Clear the index and wnode fields of "node", it siblings and its
 * children.  This is needed because they are a union with other items to save
 * space.
 */
    স্থির শূন্য
clear_গ্রন্থি(wordnode_T *গ্রন্থি)
{
    wordnode_T	*np;

    যদি (গ্রন্থি != রিক্ত)
	FOR_ALL_গ্রন্থি_SIBLINGS(গ্রন্থি, np)
	{
	    np->wn_u1.index = 0;
	    np->wn_u2.wnode = রিক্ত;

	    যদি (np->wn_byte != NUL)
		clear_গ্রন্থি(np->wn_child);
	}
}


/*
 * Dump a word tree at node "node".
 *
 * This first writes the list of possible bytes (siblings).  Then for each
 * byte recursively write the children.
 *
 * NOTE: The code here must match the code in read_tree_node(), since
 * assumptions are made about the indexes (so that we don't have to write them
 * in the file).
 *
 * Returns the number of nodes used.
 */
    স্থির পূর্ণাঙ্ক
put_গ্রন্থি(
    FILE	*fd,		// NULL when only counting
    wordnode_T	*গ্রন্থি,
    পূর্ণাঙ্ক		idx,
    পূর্ণাঙ্ক		regionmask,
    পূর্ণাঙ্ক		prefixtree)	// TRUE for PREFIXTREE
{
    পূর্ণাঙ্ক		newindex = idx;
    পূর্ণাঙ্ক		siblingcount = 0;
    wordnode_T	*np;
    পূর্ণাঙ্ক		flags;

    // If "node" is zero the tree is empty.
    যদি (গ্রন্থি == রিক্ত)
	প্রত্যাগম 0;

    // Store the index where this node is written.
    গ্রন্থি->wn_u1.index = idx;

    // Count the number of siblings.
    FOR_ALL_গ্রন্থি_SIBLINGS(গ্রন্থি, np)
	++siblingcount;

    // Write the sibling count.
    যদি (fd != রিক্ত)
	putc(siblingcount, fd);				// <siblingcount>

    // Write each sibling byte and optionally extra info.
    FOR_ALL_গ্রন্থি_SIBLINGS(গ্রন্থি, np)
    {
	যদি (np->wn_byte == 0)
	{
	    যদি (fd != রিক্ত)
	    {
		// For a NUL byte (end of word) write the flags etc.
		যদি (prefixtree)
		{
		    // In PREFIXTREE write the required affixID and the
		    // associated condition nr (stored in wn_region).  The
		    // byte value is misused to store the "rare" and "not
		    // combining" flags
		    যদি (np->wn_flags == (short_u)PFX_FLAGS)
			putc(BY_NOFLAGS, fd);		// <byte>
		    অন্যথা
		    {
			putc(BY_FLAGS, fd);		// <byte>
			putc(np->wn_flags, fd);		// <pflags>
		    }
		    putc(np->wn_affixID, fd);		// <affixID>
		    put_bytes(fd, (long_u)np->wn_region, 2); // <prefcondnr>
		}
		অন্যথা
		{
		    // For word trees we write the flag/region items.
		    flags = np->wn_flags;
		    যদি (regionmask != 0 && np->wn_region != regionmask)
			flags |= WF_REGION;
		    যদি (np->wn_affixID != 0)
			flags |= WF_AFX;
		    যদি (flags == 0)
		    {
			// word without flags or region
			putc(BY_NOFLAGS, fd);			// <byte>
		    }
		    অন্যথা
		    {
			যদি (np->wn_flags >= 0x100)
			{
			    putc(BY_FLAGS2, fd);		// <byte>
			    putc(flags, fd);			// <flags>
			    putc((unsigned)flags >> 8, fd);	// <flags2>
			}
			অন্যথা
			{
			    putc(BY_FLAGS, fd);			// <byte>
			    putc(flags, fd);			// <flags>
			}
			যদি (flags & WF_REGION)
			    putc(np->wn_region, fd);		// <region>
			যদি (flags & WF_AFX)
			    putc(np->wn_affixID, fd);		// <affixID>
		    }
		}
	    }
	}
	অন্যথা
	{
	    যদি (np->wn_child->wn_u1.index != 0
					 && np->wn_child->wn_u2.wnode != গ্রন্থি)
	    {
		// The child is written elsewhere, write the reference.
		যদি (fd != রিক্ত)
		{
		    putc(BY_INDEX, fd);			// <byte>
							// <nodeidx>
		    put_bytes(fd, (long_u)np->wn_child->wn_u1.index, 3);
		}
	    }
	    অন্যথা যদি (np->wn_child->wn_u2.wnode == রিক্ত)
		// We will write the child below and give it an index.
		np->wn_child->wn_u2.wnode = গ্রন্থি;

	    যদি (fd != রিক্ত)
		যদি (putc(np->wn_byte, fd) == EOF) // <byte> or <xbyte>
		{
		    emsg(_(e_error_while_writing));
		    প্রত্যাগম 0;
		}
	}
    }

    // Space used in the array when reading: one for each sibling and one for
    // the count.
    newindex += siblingcount + 1;

    // Recursively dump the children of each sibling.
    FOR_ALL_গ্রন্থি_SIBLINGS(গ্রন্থি, np)
	যদি (np->wn_byte != 0 && np->wn_child->wn_u2.wnode == গ্রন্থি)
	    newindex = put_গ্রন্থি(fd, np->wn_child, newindex, regionmask,
								  prefixtree);

    প্রত্যাগম newindex;
}


/*
 * ":mkspell [-ascii] outfile  infile ..."
 * ":mkspell [-ascii] addfile"
 */
    শূন্য
ex_mkspell(exarg_T *eap)
{
    পূর্ণাঙ্ক		fcount;
    char_u	**fnames;
    char_u	*arg = eap->arg;
    পূর্ণাঙ্ক		ascii = FALSE;

    যদি (STRNCMP(arg, "-ascii", 6) == 0)
    {
	ascii = TRUE;
	arg = skipwhite(arg + 6);
    }

    // Expand all the remaining arguments (e.g., $VIMRUNTIME).
    যদি (get_arglist_exp(arg, &fcount, &fnames, FALSE) != OK)
	প্রত্যাগম;

    mkspell(fcount, fnames, ascii, eap->forceit, FALSE);
    FreeWild(fcount, fnames);
}

/*
 * Create the .sug file.
 * Uses the soundfold info in "spin".
 * Writes the file with the name "wfname", with ".spl" changed to ".sug".
 */
    স্থির শূন্য
spell_make_sugfile(spellinfo_T *spin, char_u *wfname)
{
    char_u	*fname = রিক্ত;
    পূর্ণাঙ্ক		len;
    slang_T	*slang;
    পূর্ণাঙ্ক		free_slang = FALSE;

    /*
     * Read back the .spl file that was written.  This fills the required
     * info for soundfolding.  This also uses less memory than the
     * pointer-linked version of the trie.  And it avoids having two versions
     * of the code for the soundfolding stuff.
     * It might have been done already by spell_reload_one().
     */
    FOR_ALL_SPELL_LANGS(slang)
	যদি (fullpathcmp(wfname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	    ভঙ্গ;
    যদি (slang == রিক্ত)
    {
	spell_message(spin, (char_u *)_("Reading back spell file..."));
	slang = spell_load_file(wfname, রিক্ত, রিক্ত, FALSE);
	যদি (slang == রিক্ত)
	    প্রত্যাগম;
	free_slang = TRUE;
    }

    /*
     * Clear the info in "spin" that is used.
     */
    spin->si_blocks = রিক্ত;
    spin->si_blocks_cnt = 0;
    spin->si_compress_cnt = 0;	    // will stay at 0 all the time
    spin->si_free_গুনতি = 0;
    spin->si_first_free = রিক্ত;
    spin->si_foldwcount = 0;

    /*
     * Go through the trie of good words, soundfold each word and add it to
     * the soundfold trie.
     */
    spell_message(spin, (char_u *)_("Performing soundfolding..."));
    যদি (sug_filltree(spin, slang) == FAIL)
	গচ্ছ theend;

    /*
     * Create the table which links each soundfold word with a list of the
     * good words it may come from.  Creates buffer "spin->si_spellbuf".
     * This also removes the wordnr from the NUL byte entries to make
     * compression possible.
     */
    যদি (sug_maketable(spin) == FAIL)
	গচ্ছ theend;

    smsg(_("Number of words after soundfolding: %ld"),
				 (long)spin->si_spellbuf->b_ml.ml_সারি_গুনতি);

    /*
     * Compress the soundfold trie.
     */
    spell_message(spin, (char_u *)_(বার্তা_compressing));
    wordtree_compress(spin, spin->si_foldroot, "case-folded");

    /*
     * Write the .sug file.
     * Make the file name by changing ".spl" to ".sug".
     */
    fname = alloc(MAXPATHL);
    যদি (fname == রিক্ত)
	গচ্ছ theend;
    vim_strncpy(fname, wfname, MAXPATHL - 1);
    len = (পূর্ণাঙ্ক)STRLEN(fname);
    fname[len - 2] = 'u';
    fname[len - 1] = 'g';
    sug_write(spin, fname);

theend:
    vim_free(fname);
    যদি (free_slang)
	slang_free(slang);
    free_blocks(spin->si_blocks);
    close_spellbuf(spin->si_spellbuf);
}

/*
 * Build the soundfold trie for language "slang".
 */
    স্থির পূর্ণাঙ্ক
sug_filltree(spellinfo_T *spin, slang_T *slang)
{
    char_u	*byts;
    idx_T	*idxs;
    পূর্ণাঙ্ক		depth;
    idx_T	arridx[MAXWLEN];
    পূর্ণাঙ্ক		curi[MAXWLEN];
    char_u	tword[MAXWLEN];
    char_u	tsalword[MAXWLEN];
    পূর্ণাঙ্ক		c;
    idx_T	n;
    unsigned	শব্দগুলি_done = 0;
    পূর্ণাঙ্ক		wordcount[MAXWLEN];

    // We use si_foldroot for the soundfolded trie.
    spin->si_foldroot = wordtree_alloc(spin);
    যদি (spin->si_foldroot == রিক্ত)
	প্রত্যাগম FAIL;

    // let tree_add_word() know we're adding to the soundfolded tree
    spin->si_sugtree = TRUE;

    /*
     * Go through the whole case-folded tree, soundfold each word and put it
     * in the trie.  Bail out if the tree is empty.
     */
    byts = slang->sl_fbyts;
    idxs = slang->sl_fidxs;
    যদি (byts == রিক্ত || idxs == রিক্ত)
	প্রত্যাগম FAIL;

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;

    depth = 0;
    যাবত্ (depth >= 0 && !got_int)
    {
	যদি (curi[depth] > byts[arridx[depth]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[depth]] = wordcount[depth];
	    যদি (depth > 0)
		wordcount[depth - 1] += wordcount[depth];

	    --depth;
	    সারি_breakcheck();
	}
	অন্যথা
	{

	    // Do one more byte at this node.
	    n = arridx[depth] + curi[depth];
	    ++curi[depth];

	    c = byts[n];
	    যদি (c == 0)
	    {
		// Sound-fold the word.
		tword[depth] = NUL;
		spell_soundfold(slang, tword, TRUE, tsalword);

		// We use the "flags" field for the MSB of the wordnr,
		// "region" for the LSB of the wordnr.
		যদি (বৃক্ষ_add_word(spin, tsalword, spin->si_foldroot,
				শব্দগুলি_done >> 16, শব্দগুলি_done & 0xffff,
							   0) == FAIL)
		    প্রত্যাগম FAIL;

		++শব্দগুলি_done;
		++wordcount[depth];

		// Reset the block count each time to avoid compression
		// kicking in.
		spin->si_blocks_cnt = 0;

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		যাবত্ (n + 1 < slang->sl_fbyts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[depth];
		}
	    }
	    অন্যথা যদি (depth < MAXWLEN - 1)
	    {
		// Normal char, go one level deeper.
		tword[depth++] = c;
		arridx[depth] = idxs[n];
		curi[depth] = 1;
		wordcount[depth] = 0;
	    }
	}
    }

    smsg(_("Total number of words: %d"), শব্দগুলি_done);

    প্রত্যাগম OK;
}

/*
 * Make the table that links each word in the soundfold trie to the words it
 * can be produced from.
 * This is not unlike lines in a file, thus use a memfile to be able to access
 * the table efficiently.
 * Returns FAIL when out of memory.
 */
    স্থির পূর্ণাঙ্ক
sug_maketable(spellinfo_T *spin)
{
    garray_T	ga;
    পূর্ণাঙ্ক		res = OK;

    // Allocate a buffer, open a memline for it and create the swap file
    // (uses a temp file, not a .swp file).
    spin->si_spellbuf = open_spellbuf();
    যদি (spin->si_spellbuf == রিক্ত)
	প্রত্যাগম FAIL;

    // Use a buffer to store the line info, avoids allocating many small
    // pieces of memory.
    ga_init2(&ga, 1, 100);

    // recursively go through the tree
    যদি (sug_filltable(spin, spin->si_foldroot->wn_sibling, 0, &ga) == -1)
	res = FAIL;

    ga_clear(&ga);
    প্রত্যাগম res;
}

/*
 * Fill the table for one node and its children.
 * Returns the wordnr at the start of the node.
 * Returns -1 when out of memory.
 */
    স্থির পূর্ণাঙ্ক
sug_filltable(
    spellinfo_T	*spin,
    wordnode_T	*গ্রন্থি,
    পূর্ণাঙ্ক		startwordnr,
    garray_T	*gap)	    // place to store line of numbers
{
    wordnode_T	*p, *np;
    পূর্ণাঙ্ক		wordnr = startwordnr;
    পূর্ণাঙ্ক		nr;
    পূর্ণাঙ্ক		prev_nr;

    FOR_ALL_গ্রন্থি_SIBLINGS(গ্রন্থি, p)
    {
	যদি (p->wn_byte == NUL)
	{
	    gap->ga_len = 0;
	    prev_nr = 0;
	    প্রতি (np = p; np != রিক্ত && np->wn_byte == NUL; np = np->wn_sibling)
	    {
		যদি (ga_grow(gap, 10) == FAIL)
		    প্রত্যাগম -1;

		nr = (np->wn_flags << 16) + (np->wn_region & 0xffff);
		// Compute the offset from the previous nr and store the
		// offset in a way that it takes a minimum number of bytes.
		// It's a bit like utf-8, but without the need to mark
		// following bytes.
		nr -= prev_nr;
		prev_nr += nr;
		gap->ga_len += offset2bytes(nr,
					 (char_u *)gap->ga_data + gap->ga_len);
	    }

	    // add the NUL byte
	    ((char_u *)gap->ga_data)[gap->ga_len++] = NUL;

	    যদি (ml_append_buf(spin->si_spellbuf, (linenr_T)wordnr,
				     gap->ga_data, gap->ga_len, TRUE) == FAIL)
		প্রত্যাগম -1;
	    ++wordnr;

	    // Remove extra NUL entries, we no longer need them. We don't
	    // bother freeing the nodes, they won't be reused anyway.
	    যাবত্ (p->wn_sibling != রিক্ত && p->wn_sibling->wn_byte == NUL)
		p->wn_sibling = p->wn_sibling->wn_sibling;

	    // Clear the flags on the remaining NUL node, so that compression
	    // works a lot better.
	    p->wn_flags = 0;
	    p->wn_region = 0;
	}
	অন্যথা
	{
	    wordnr = sug_filltable(spin, p->wn_child, wordnr, gap);
	    যদি (wordnr == -1)
		প্রত্যাগম -1;
	}
    }
    প্রত্যাগম wordnr;
}

/*
 * Convert an offset into a minimal number of bytes.
 * Similar to utf_char2byters, but use 8 bits in followup bytes and avoid NUL
 * bytes.
 */
    স্থির পূর্ণাঙ্ক
offset2bytes(পূর্ণাঙ্ক nr, char_u *buf)
{
    পূর্ণাঙ্ক	    rem;
    পূর্ণাঙ্ক	    b1, b2, b3, b4;

    // Split the number in parts of base 255.  We need to avoid NUL bytes.
    b1 = nr % 255 + 1;
    rem = nr / 255;
    b2 = rem % 255 + 1;
    rem = rem / 255;
    b3 = rem % 255 + 1;
    b4 = rem / 255 + 1;

    যদি (b4 > 1 || b3 > 0x1f)	// 4 bytes
    {
	buf[0] = 0xe0 + b4;
	buf[1] = b3;
	buf[2] = b2;
	buf[3] = b1;
	প্রত্যাগম 4;
    }
    যদি (b3 > 1 || b2 > 0x3f )	// 3 bytes
    {
	buf[0] = 0xc0 + b3;
	buf[1] = b2;
	buf[2] = b1;
	প্রত্যাগম 3;
    }
    যদি (b2 > 1 || b1 > 0x7f )	// 2 bytes
    {
	buf[0] = 0x80 + b2;
	buf[1] = b1;
	প্রত্যাগম 2;
    }
				// 1 byte
    buf[0] = b1;
    প্রত্যাগম 1;
}

/*
 * Write the .sug file in "fname".
 */
    স্থির শূন্য
sug_write(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    wordnode_T	*বৃক্ষ;
    পূর্ণাঙ্ক		nodecount;
    পূর্ণাঙ্ক		wcount;
    char_u	*সারি;
    linenr_T	lnum;
    পূর্ণাঙ্ক		len;

    // Create the file.  Note that an existing file is silently overwritten!
    fd = mch_fopen((বর্ণ *)fname, "w");
    যদি (fd == রিক্ত)
    {
	semsg(_(e_cant_open_file_str), fname);
	প্রত্যাগম;
    }

    vim_snprintf((বর্ণ *)IObuff, IOSIZE,
				  _("Writing suggestion file %s..."), fname);
    spell_message(spin, IObuff);

    /*
     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
     */
    যদি (fwrite(VIMSUGMAGIC, VIMSUGMAGICL, (size_t)1, fd) != 1) // <fileID>
    {
	emsg(_(e_error_while_writing));
	গচ্ছ theend;
    }
    putc(VIMSUGVERSION, fd);				// <versionnr>

    // Write si_sugtime to the file.
    put_time(fd, spin->si_sugtime);			// <timestamp>

    /*
     * <SUGWORDTREE>
     */
    spin->si_memtot = 0;
    বৃক্ষ = spin->si_foldroot->wn_sibling;

    // Clear the index and wnode fields in the tree.
    clear_গ্রন্থি(বৃক্ষ);

    // Count the number of nodes.  Needed to be able to allocate the
    // memory when reading the nodes.  Also fills in index for shared
    // nodes.
    nodecount = put_গ্রন্থি(রিক্ত, বৃক্ষ, 0, 0, FALSE);

    // number of nodes in 4 bytes
    put_bytes(fd, (long_u)nodecount, 4);	// <nodecount>
    spin->si_memtot += nodecount + nodecount * পরিমাণ(পূর্ণাঙ্ক);

    // Write the nodes.
    (শূন্য)put_গ্রন্থি(fd, বৃক্ষ, 0, 0, FALSE);

    /*
     * <SUGTABLE>: <sugwcount> <sugline> ...
     */
    wcount = spin->si_spellbuf->b_ml.ml_সারি_গুনতি;
    put_bytes(fd, (long_u)wcount, 4);	// <sugwcount>

    প্রতি (lnum = 1; lnum <= (linenr_T)wcount; ++lnum)
    {
	// <sugline>: <sugnr> ... NUL
	সারি = ml_get_buf(spin->si_spellbuf, lnum, FALSE);
	len = ml_get_buf_len(spin->si_spellbuf, lnum) + 1;
	যদি (fwrite(সারি, (size_t)len, (size_t)1, fd) == 0)
	{
	    emsg(_(e_error_while_writing));
	    গচ্ছ theend;
	}
	spin->si_memtot += len;
    }

    // Write another byte to check for errors.
    যদি (putc(0, fd) == EOF)
	emsg(_(e_error_while_writing));

    vim_snprintf((বর্ণ *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin->si_memtot);
    spell_message(spin, IObuff);

theend:
    // close the file
    fclose(fd);
}


/*
 * Create a Vim spell file from one or more word lists.
 * "fnames[0]" is the output file name.
 * "fnames[fcount - 1]" is the last input file name.
 * Exception: when "fnames[0]" ends in ".add" it's used as the input file name
 * and ".spl" is appended to make the output file name.
 */
    শূন্য
mkspell(
    পূর্ণাঙ্ক		fcount,
    char_u	**fnames,
    পূর্ণাঙ্ক		ascii,		    // -ascii argument given
    পূর্ণাঙ্ক		over_write,	    // overwrite existing output file
    পূর্ণাঙ্ক		added_word)	    // invoked through "zg"
{
    char_u	*fname = রিক্ত;
    char_u	*wfname;
    char_u	**innames;
    পূর্ণাঙ্ক		incount;
    afffile_T	*(afile[MAXREGIONS]);
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		len;
    stat_T	st;
    পূর্ণাঙ্ক		error = FALSE;
    spellinfo_T spin;

    CLEAR_FIELD(spin);
    spin.si_verbose = !added_word;
    spin.si_ascii = ascii;
    spin.si_followup = TRUE;
    spin.si_rem_accents = TRUE;
    ga_init2(&spin.si_rep, পরিমাণ(fromto_T), 20);
    ga_init2(&spin.si_repsal, পরিমাণ(fromto_T), 20);
    ga_init2(&spin.si_sal, পরিমাণ(fromto_T), 20);
    ga_init2(&spin.si_map, পরিমাণ(char_u), 100);
    ga_init2(&spin.si_comppat, পরিমাণ(char_u *), 20);
    ga_init2(&spin.si_prefcond, পরিমাণ(char_u *), 50);
    hash_init(&spin.si_commonwords);
    spin.si_newcompID = 127;	// start compound ID at first maximum

    // default: fnames[0] is output file, following are input files
    // When "fcount" is 1 there is only one file.
    innames = &fnames[fcount == 1 ? 0 : 1];
    incount = fcount - 1;

    wfname = alloc(MAXPATHL);
    যদি (wfname == রিক্ত)
	প্রত্যাগম;

    যদি (fcount >= 1)
    {
	len = (পূর্ণাঙ্ক)STRLEN(fnames[0]);
	যদি (fcount == 1 && len > 4 && STRCMP(fnames[0] + len - 4, ".add") == 0)
	{
	    // For ":mkspell path/en.latin1.add" output file is
	    // "path/en.latin1.add.spl".
	    incount = 1;
	    vim_snprintf((বর্ণ *)wfname, MAXPATHL, "%s.spl", fnames[0]);
	}
	অন্যথা যদি (fcount == 1)
	{
	    // For ":mkspell path/vim" output file is "path/vim.latin1.spl".
	    incount = 1;
	    vim_snprintf((বর্ণ *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());
	}
	অন্যথা যদি (len > 4 && STRCMP(fnames[0] + len - 4, ".spl") == 0)
	{
	    // Name ends in ".spl", use as the file name.
	    vim_strncpy(wfname, fnames[0], MAXPATHL - 1);
	}
	অন্যথা
	    // Name should be language, make the file name from it.
	    vim_snprintf((বর্ণ *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());

	// Check for .ascii.spl.
	যদি (strstr((বর্ণ *)gettail(wfname), SPL_FNAME_ASCII) != রিক্ত)
	    spin.si_ascii = TRUE;

	// Check for .add.spl.
	যদি (strstr((বর্ণ *)gettail(wfname), SPL_FNAME_ADD) != রিক্ত)
	    spin.si_add = TRUE;
    }

    যদি (incount <= 0)
	emsg(_(e_invalid_argument));	// need at least output and input names
    অন্যথা যদি (vim_strchr(gettail(wfname), '_') != রিক্ত)
	emsg(_(e_output_file_name_must_not_have_region_name));
    অন্যথা যদি (incount > MAXREGIONS)
	semsg(_(e_only_up_to_nr_regions_supported), MAXREGIONS);
    অন্যথা
    {
	// Check for overwriting before doing things that may take a lot of
	// time.
	যদি (!over_write && mch_stat((বর্ণ *)wfname, &st) >= 0)
	{
	    emsg(_(e_file_exists));
	    গচ্ছ theend;
	}
	যদি (mch_isdir(wfname))
	{
	    semsg(_(e_str_is_directory), wfname);
	    গচ্ছ theend;
	}

	fname = alloc(MAXPATHL);
	যদি (fname == রিক্ত)
	    গচ্ছ theend;

	/*
	 * Init the aff and dic pointers.
	 * Get the region names if there are more than 2 arguments.
	 */
	প্রতি (i = 0; i < incount; ++i)
	{
	    afile[i] = রিক্ত;

	    যদি (incount > 1)
	    {
		len = (পূর্ণাঙ্ক)STRLEN(innames[i]);
		যদি (STRLEN(gettail(innames[i])) < 5
						|| innames[i][len - 3] != '_')
		{
		    semsg(_(e_invalid_region_in_str), innames[i]);
		    গচ্ছ theend;
		}
		spin.si_region_name[i * 2] = TOLOWER_ASC(innames[i][len - 2]);
		spin.si_region_name[i * 2 + 1] =
					     TOLOWER_ASC(innames[i][len - 1]);
	    }
	}
	spin.si_region_গুনতি = incount;

	spin.si_foldroot = wordtree_alloc(&spin);
	spin.si_keeproot = wordtree_alloc(&spin);
	spin.si_prefroot = wordtree_alloc(&spin);
	যদি (spin.si_foldroot == রিক্ত
		|| spin.si_keeproot == রিক্ত
		|| spin.si_prefroot == রিক্ত)
	{
	    free_blocks(spin.si_blocks);
	    গচ্ছ theend;
	}

	// When not producing a .add.spl file clear the character table when
	// we encounter one in the .aff file.  This means we dump the current
	// one in the .spl file if the .aff file doesn't define one.  That's
	// better than guessing the contents, the table will match a
	// previously loaded spell file.
	যদি (!spin.si_add)
	    spin.si_clear_chartab = TRUE;

	/*
	 * Read all the .aff and .dic files.
	 * Text is converted to 'encoding'.
	 * Words are stored in the case-folded and keep-case trees.
	 */
	প্রতি (i = 0; i < incount && !error; ++i)
	{
	    spin.si_conv.vc_type = CONV_NONE;
	    spin.si_region = 1 << i;

	    vim_snprintf((বর্ণ *)fname, MAXPATHL, "%s.aff", innames[i]);
	    যদি (mch_stat((বর্ণ *)fname, &st) >= 0)
	    {
		// Read the .aff file.  Will init "spin->si_conv" based on the
		// "SET" line.
		afile[i] = spell_read_aff(&spin, fname);
		যদি (afile[i] == রিক্ত)
		    error = TRUE;
		অন্যথা
		{
		    // Read the .dic file and store the words in the trees.
		    vim_snprintf((বর্ণ *)fname, MAXPATHL, "%s.dic",
								  innames[i]);
		    যদি (spell_read_dic(&spin, fname, afile[i]) == FAIL)
			error = TRUE;
		}
	    }
	    অন্যথা
	    {
		// No .aff file, try reading the file as a word list.  Store
		// the words in the trees.
		যদি (spell_read_wordfile(&spin, innames[i]) == FAIL)
		    error = TRUE;
	    }

	    // Free any conversion stuff.
	    convert_setup(&spin.si_conv, রিক্ত, রিক্ত);
	}

	যদি (spin.si_compflags != রিক্ত && spin.si_nobreak)
	    বার্তা(_("Warning: both compounding and NOBREAK specified"));

	যদি (!error && !got_int)
	{
	    /*
	     * Combine tails in the tree.
	     */
	    spell_message(&spin, (char_u *)_(বার্তা_compressing));
	    wordtree_compress(&spin, spin.si_foldroot, "case-folded");
	    wordtree_compress(&spin, spin.si_keeproot, "keep-case");
	    wordtree_compress(&spin, spin.si_prefroot, "prefixes");
	}

	যদি (!error && !got_int)
	{
	    /*
	     * Write the info in the spell file.
	     */
	    vim_snprintf((বর্ণ *)IObuff, IOSIZE,
				      _("Writing spell file %s..."), wfname);
	    spell_message(&spin, IObuff);

	    error = write_vim_spell(&spin, wfname) == FAIL;

	    spell_message(&spin, (char_u *)_("Done!"));
	    vim_snprintf((বর্ণ *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin.si_memtot);
	    spell_message(&spin, IObuff);

	    /*
	     * If the file is loaded need to reload it.
	     */
	    যদি (!error)
		spell_reload_one(wfname, added_word);
	}

	// Free the allocated memory.
	ga_clear(&spin.si_rep);
	ga_clear(&spin.si_repsal);
	ga_clear(&spin.si_sal);
	ga_clear(&spin.si_map);
	ga_clear(&spin.si_comppat);
	ga_clear(&spin.si_prefcond);
	hash_clear_all(&spin.si_commonwords, 0);

	// Free the .aff file structures.
	প্রতি (i = 0; i < incount; ++i)
	    যদি (afile[i] != রিক্ত)
		spell_free_aff(afile[i]);

	// Free all the bits and pieces at once.
	free_blocks(spin.si_blocks);

	/*
	 * If there is soundfolding info and no NOSUGFILE item create the
	 * .sug file with the soundfolded word trie.
	 */
	যদি (spin.si_sugtime != 0 && !error && !got_int)
	    spell_make_sugfile(&spin, wfname);

    }

theend:
    vim_free(fname);
    vim_free(wfname);
}

/*
 * Display a message for spell file processing when 'verbose' is set or using
 * ":mkspell".  "str" can be IObuff.
 */
    স্থির শূন্য
spell_message(spellinfo_T *spin, char_u *str)
{
    যদি (spin->si_verbose || p_verbose > 2)
    {
	যদি (!spin->si_verbose)
	    verbose_enter();
	বার্তা((বর্ণ *)str);
	out_flush();
	যদি (!spin->si_verbose)
	    verbose_leave();
    }
}

/*
 * ":[count]spellgood  {word}"
 * ":[count]spellwrong {word}"
 * ":[count]spellundo  {word}"
 * ":[count]spellrare  {word}"
 */
    শূন্য
ex_spell(exarg_T *eap)
{
    spell_add_word(eap->arg, (পূর্ণাঙ্ক)STRLEN(eap->arg),
		eap->cmdidx == CMD_spellwrong ? SPELL_ADD_BAD :
		eap->cmdidx == CMD_spellrare ? SPELL_ADD_RARE : SPELL_ADD_GOOD,
				   eap->forceit ? 0 : (পূর্ণাঙ্ক)eap->line2,
				   eap->cmdidx == CMD_spellundo);
}

/*
 * Add "word[len]" to 'spellfile' as a good, rare or bad word.
 */
    শূন্য
spell_add_word(
    char_u	*word,
    পূর্ণাঙ্ক		len,
    পূর্ণাঙ্ক		what,	    // SPELL_ADD_ values
    পূর্ণাঙ্ক		idx,	    // "zG" and "zW": zero, otherwise index in
			    // 'spellfile'
    পূর্ণাঙ্ক		undo)	    // TRUE for "zug", "zuG", "zuw" and "zuW"
{
    FILE	*fd = রিক্ত;
    buf_T	*buf = রিক্ত;
    পূর্ণাঙ্ক		new_spf = FALSE;
    char_u	*fname;
    char_u	*fnamebuf = রিক্ত;
    char_u	সারি[MAXWLEN * 2];
    long	fpos, fpos_next = 0;
    পূর্ণাঙ্ক		i;
    size_t	linelen;
    char_u	*spf;

    যদি (!valid_spell_word(word, word + len))
    {
	emsg(_(e_illegal_character_in_word));
	প্রত্যাগম;
    }

    যদি (idx == 0)	    // use internal wordlist
    {
	যদি (int_wordlist == রিক্ত)
	{
	    int_wordlist = vim_tempname('s', FALSE);
	    যদি (int_wordlist == রিক্ত)
		প্রত্যাগম;
	}
	fname = int_wordlist;
    }
    অন্যথা
    {
	// If 'spellfile' isn't set figure out a good default value.
	যদি (*curwin->w_s->b_p_spf == NUL)
	{
	    init_spellfile();
	    new_spf = TRUE;
	}

	যদি (*curwin->w_s->b_p_spf == NUL)
	{
	    semsg(_(e_option_str_is_not_set), "spellfile");
	    প্রত্যাগম;
	}
	fnamebuf = alloc(MAXPATHL);
	যদি (fnamebuf == রিক্ত)
	    প্রত্যাগম;

	প্রতি (spf = curwin->w_s->b_p_spf, i = 1; *spf != NUL; ++i)
	{
	    copy_option_part(&spf, fnamebuf, MAXPATHL, ",");
	    যদি (i == idx)
		ভঙ্গ;
	    যদি (*spf == NUL)
	    {
		semsg(_(e_spellfile_does_not_have_nr_entries), idx);
		vim_free(fnamebuf);
		প্রত্যাগম;
	    }
	}

	// Check that the user isn't editing the .add file somewhere.
	buf = buflist_findname_exp(fnamebuf);
	যদি (buf != রিক্ত && buf->b_ml.ml_mfp == রিক্ত)
	    buf = রিক্ত;
	যদি (buf != রিক্ত && bufIsChanged(buf))
	{
	    emsg(_(e_file_is_loaded_in_another_buffer));
	    vim_free(fnamebuf);
	    প্রত্যাগম;
	}

	fname = fnamebuf;
    }

    যদি (what == SPELL_ADD_BAD || undo)
    {
	// When the word appears as good word we need to remove that one,
	// since its flags sort before the one with WF_BANNED.
	fd = mch_fopen((বর্ণ *)fname, "r");
	যদি (fd != রিক্ত)
	{
	    যাবত্ (!vim_fgets(সারি, MAXWLEN * 2, fd))
	    {
		fpos = fpos_next;
		fpos_next = ftell(fd);
		যদি (fpos_next < 0)
		    ভঙ্গ;  // should never happen
		linelen = STRLEN(সারি);
		যদি (linelen >= (size_t)len
			&& STRNCMP(word, সারি, len) == 0
			&& (সারি[len] == '/' || সারি[len] < ' '))
		{
		    // Found duplicate word.  Remove it by writing a '#' at
		    // the start of the line.  Mixing reading and writing
		    // doesn't work for all systems, close the file first.
		    fclose(fd);
		    fd = mch_fopen((বর্ণ *)fname, "r+");
		    যদি (fd == রিক্ত)
			ভঙ্গ;
		    যদি (fseek(fd, fpos, SEEK_SET) == 0)
		    {
			fputc('#', fd);
			যদি (undo)
			{
			    home_replace(রিক্ত, fname, NameBuff, MAXPATHL, TRUE);
			    smsg(_("Word '%.*s' removed from %s"),
							 len, word, NameBuff);
			}
		    }
		    যদি (fseek(fd, fpos_next, SEEK_SET) != 0)
		    {
			PERROR(_("Seek error in spellfile"));
			ভঙ্গ;
		    }
		}
	    }
	    যদি (fd != রিক্ত)
		fclose(fd);
	}
    }

    যদি (!undo)
    {
	fd = mch_fopen((বর্ণ *)fname, "a");
	যদি (fd == রিক্ত && new_spf)
	{
	    char_u *p;

	    // We just initialized the 'spellfile' option and can't open the
	    // file.  We may need to create the "spell" directory first.  We
	    // already checked the runtime directory is writable in
	    // init_spellfile().
	    যদি (!dir_of_file_exists(fname) && (p = gettail_sep(fname)) != fname)
	    {
		পূর্ণাঙ্ক c = *p;

		// The directory doesn't exist.  Try creating it and opening
		// the file again.
		*p = NUL;
		vim_mkdir(fname, 0755);
		*p = c;
		fd = mch_fopen((বর্ণ *)fname, "a");
	    }
	}

	যদি (fd == রিক্ত)
	    semsg(_(e_cant_open_file_str), fname);
	অন্যথা
	{
	    যদি (what == SPELL_ADD_BAD)
		fprintf(fd, "%.*s/!\n", len, word);
	    অন্যথা যদি (what == SPELL_ADD_RARE)
		fprintf(fd, "%.*s/?\n", len, word);
	    অন্যথা
		fprintf(fd, "%.*s\n", len, word);
	    fclose(fd);

	    home_replace(রিক্ত, fname, NameBuff, MAXPATHL, TRUE);
	    smsg(_("Word '%.*s' added to %s"), len, word, NameBuff);
	}
    }

    যদি (fd != রিক্ত)
    {
	// Update the .add.spl file.
	mkspell(1, &fname, FALSE, TRUE, TRUE);

	// If the .add file is edited somewhere, reload it.
	যদি (buf != রিক্ত)
	    buf_reload(buf, buf->b_orig_mode, FALSE);

	redraw_all_later(UPD_SOME_VALID);
    }
    vim_free(fnamebuf);
}

/*
 * Initialize 'spellfile' for the current buffer.
 */
    স্থির শূন্য
init_spellfile(শূন্য)
{
    string_T	buf;
    char_u	*fname;
    char_u	*rtp;
    char_u	*lend;
    পূর্ণাঙ্ক		aspath = FALSE;
    char_u	*lstart = curbuf->b_s.b_p_spl;

    যদি (*curwin->w_s->b_p_spl == NUL || curwin->w_s->b_langp.ga_len <= 0)
	প্রত্যাগম;

    buf.string = alloc(MAXPATHL);
    যদি (buf.string == রিক্ত)
	প্রত্যাগম;

    buf.length = 0;

    // Find the end of the language name.  Exclude the region.  If there
    // is a path separator remember the start of the tail.
    প্রতি (lend = curwin->w_s->b_p_spl; *lend != NUL
	    && vim_strchr((char_u *)",._", *lend) == রিক্ত; ++lend)
	যদি (vim_ispathsep(*lend))
	{
	    aspath = TRUE;
	    lstart = lend + 1;
	}

    // Loop over all entries in 'runtimepath'.  Use the first one where we
    // are allowed to write.
    rtp = p_rtp;
    যাবত্ (*rtp != NUL)
    {
	যদি (aspath)
	    // Use directory of an entry with path, e.g., for
	    // "/dir/lg.utf-8.spl" use "/dir".
	    vim_strncpy(buf.string, curbuf->b_s.b_p_spl,
		    lstart - curbuf->b_s.b_p_spl - 1);
	অন্যথা
	    // Copy the path from 'runtimepath' to buf[].
	    buf.length = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");
	যদি (filewritable(buf.string) == 2)
	{
	    // Use the first language name from 'spelllang' and the
	    // encoding used in the first loaded .spl file.
	    যদি (aspath)
	    {
		buf.length = (size_t)(lend - curbuf->b_s.b_p_spl);
		vim_strncpy(buf.string, curbuf->b_s.b_p_spl, buf.length);
	    }
	    অন্যথা
	    {
		// Create the "spell" directory if it doesn't exist yet.
		buf.length += vim_snprintf_safelen((বর্ণ *)buf.string + buf.length,
		    MAXPATHL - buf.length, "/spell");
		যদি (filewritable(buf.string) != 2)
		{
		    যদি (vim_mkdir(buf.string, 0755) != 0)
		    {
			vim_free(buf.string);
			প্রত্যাগম;
		    }
		}

		buf.length += vim_snprintf_safelen((বর্ণ *)buf.string + buf.length,
		    MAXPATHL - buf.length, "/%.*s", (পূর্ণাঙ্ক)(lend - lstart), lstart);
	    }
	    fname = LANGP_ENTRY(curwin->w_s->b_langp, 0)
		->lp_slang->sl_fname;
	    vim_snprintf((বর্ণ *)buf.string + buf.length, MAXPATHL - buf.length,
		".%s.add",
		fname != রিক্ত && strstr((বর্ণ *)gettail(fname), ".ascii.") != রিক্ত
		    ? (char_u *)"ascii" : spell_enc());
	    set_option_মান_give_err((char_u *)"spellfile",
		    0L, buf.string, OPT_LOCAL);
	    ভঙ্গ;
	}
	aspath = FALSE;
    }

    vim_free(buf.string);
}



/*
 * Set the spell character tables from strings in the affix file.
 */
    স্থির পূর্ণাঙ্ক
set_spell_chartab(char_u *fol, char_u *নিচের, char_u *upp)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    char_u	*pf = fol, *pl = নিচের, *pu = upp;
    পূর্ণাঙ্ক		f, l, u;

    clear_spell_chartab(&new_st);

    যাবত্ (*pf != NUL)
    {
	যদি (*pl == NUL || *pu == NUL)
	{
	    emsg(_(e_format_error_in_affix_file_fol_নিচের_or_upp));
	    প্রত্যাগম FAIL;
	}
	f = mb_ptr2char_adv(&pf);
	l = mb_ptr2char_adv(&pl);
	u = mb_ptr2char_adv(&pu);

	// Every character that appears is a word character.
	যদি (f < 256)
	    new_st.st_isw[f] = TRUE;
	যদি (l < 256)
	    new_st.st_isw[l] = TRUE;
	যদি (u < 256)
	    new_st.st_isw[u] = TRUE;

	// if "LOW" and "FOL" are not the same the "LOW" char needs
	// case-folding
	যদি (l < 256 && l != f)
	{
	    যদি (f >= 256)
	    {
		emsg(_(e_character_in_fol_নিচের_or_upp_is_out_of_range));
		প্রত্যাগম FAIL;
	    }
	    new_st.st_fold[l] = f;
	}

	// if "UPP" and "FOL" are not the same the "UPP" char needs
	// case-folding, it's upper case and the "UPP" is the upper case of
	// "FOL" .
	যদি (u < 256 && u != f)
	{
	    যদি (f >= 256)
	    {
		emsg(_(e_character_in_fol_নিচের_or_upp_is_out_of_range));
		প্রত্যাগম FAIL;
	    }
	    new_st.st_fold[u] = f;
	    new_st.st_isu[u] = TRUE;
	    new_st.st_upper[f] = u;
	}
    }

    যদি (*pl != NUL || *pu != NUL)
    {
	emsg(_(e_format_error_in_affix_file_fol_নিচের_or_upp));
	প্রত্যাগম FAIL;
    }

    প্রত্যাগম set_spell_finish(&new_st);
}

/*
 * Set the spell character tables from strings in the .spl file.
 */
    স্থির শূন্য
set_spell_charflags(
    char_u	*flags,
    পূর্ণাঙ্ক		cnt,	    // length of "flags"
    char_u	*fol)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    পূর্ণাঙ্ক		i;
    char_u	*p = fol;
    পূর্ণাঙ্ক		c;

    clear_spell_chartab(&new_st);

    প্রতি (i = 0; i < 128; ++i)
    {
	যদি (i < cnt)
	{
	    new_st.st_isw[i + 128] = (flags[i] & CF_WORD) != 0;
	    new_st.st_isu[i + 128] = (flags[i] & CF_UPPER) != 0;
	}

	যদি (*p != NUL)
	{
	    c = mb_ptr2char_adv(&p);
	    new_st.st_fold[i + 128] = c;
	    যদি (i + 128 != c && new_st.st_isu[i + 128] && c < 256)
		new_st.st_upper[c] = i + 128;
	}
    }

    (শূন্য)set_spell_finish(&new_st);
}

    স্থির পূর্ণাঙ্ক
set_spell_finish(spelltab_T *new_st)
{
    পূর্ণাঙ্ক		i;

    যদি (did_set_spelltab)
    {
	// check that it's the same table
	প্রতি (i = 0; i < 256; ++i)
	{
	    যদি (spelltab.st_isw[i] != new_st->st_isw[i]
		    || spelltab.st_isu[i] != new_st->st_isu[i]
		    || spelltab.st_fold[i] != new_st->st_fold[i]
		    || spelltab.st_upper[i] != new_st->st_upper[i])
	    {
		emsg(_(e_word_characters_differ_between_spell_files));
		প্রত্যাগম FAIL;
	    }
	}
    }
    অন্যথা
    {
	// copy the new spelltab into the one being used
	spelltab = *new_st;
	did_set_spelltab = TRUE;
    }

    প্রত্যাগম OK;
}

/*
 * Write the table with prefix conditions to the .spl file.
 * When "fd" is NULL only count the length of what is written and return it.
 */
    স্থির পূর্ণাঙ্ক
write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv)
{
    পূর্ণাঙ্ক		i;
    char_u	*p;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		totlen;

    যদি (fd != রিক্ত)
	put_bytes(fd, (long_u)gap->ga_len, 2);	    // <prefcondcnt>

    totlen = 2 + gap->ga_len; // length of <prefcondcnt> and <condlen> bytes

    প্রতি (i = 0; i < gap->ga_len; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	p = ((char_u **)gap->ga_data)[i];
	যদি (p != রিক্ত)
	{
	    len = (পূর্ণাঙ্ক)STRLEN(p);
	    যদি (fd != রিক্ত)
	    {
		fputc(len, fd);
		*fwv &= fwrite(p, (size_t)len, (size_t)1, fd);
	    }
	    totlen += len;
	}
	অন্যথা যদি (fd != রিক্ত)
	    fputc(0, fd);
    }

    প্রত্যাগম totlen;
}

/*
 * Use map string "map" for languages "lp".
 */
    স্থির শূন্য
set_map_str(slang_T *lp, char_u *map)
{
    char_u	*p;
    পূর্ণাঙ্ক		headc = 0;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		i;

    যদি (*map == NUL)
    {
	lp->sl_has_map = FALSE;
	প্রত্যাগম;
    }
    lp->sl_has_map = TRUE;

    // Init the array and hash tables empty.
    প্রতি (i = 0; i < 256; ++i)
	lp->sl_map_array[i] = 0;
    hash_init(&lp->sl_map_hash);

    /*
     * The similar characters are stored separated with slashes:
     * "aaa/bbb/ccc/".  Fill sl_map_array[c] with the character before c and
     * before the same slash.  For characters above 255 sl_map_hash is used.
     */
    প্রতি (p = map; *p != NUL; )
    {
	c = mb_cptr2char_adv(&p);
	যদি (c == '/')
	    headc = 0;
	অন্যথা
	{
	    যদি (headc == 0)
		 headc = c;

	    // Characters above 255 don't fit in sl_map_array[], put them in
	    // the hash table.  Each entry is the char, a NUL the headchar and
	    // a NUL.
	    যদি (c >= 256)
	    {
		পূর্ণাঙ্ক	    cl = mb_char2len(c);
		পূর্ণাঙ্ক	    headcl = mb_char2len(headc);
		char_u	    *b;
		hash_T	    hash;
		hashitem_T  *hi;

		b = alloc(cl + headcl + 2);
		যদি (b == রিক্ত)
		    প্রত্যাগম;
		mb_char2bytes(c, b);
		b[cl] = NUL;
		mb_char2bytes(headc, b + cl + 1);
		b[cl + 1 + headcl] = NUL;
		hash = hash_hash(b);
		hi = hash_lookup(&lp->sl_map_hash, b, hash);
		যদি (HASHITEM_EMPTY(hi))
		    hash_add_item(&lp->sl_map_hash, hi, b, hash);
		অন্যথা
		{
		    // This should have been checked when generating the .spl
		    // file.
		    emsg(_(e_duplicate_char_in_map_entry));
		    vim_free(b);
		}
	    }
	    অন্যথা
		lp->sl_map_array[c] = headc;
	}
    }
}

#endif  // FEAT_SPELL
