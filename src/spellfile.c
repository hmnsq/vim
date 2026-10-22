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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_SPELL)

#అన్తర్భావ <time.h>	// for time_t

// Special byte values for <byte>.  Some are only used in the tree for
// postponed prefixes, some only in the other trees.  This is a bit messy...
#నిర్వచించు BY_NOFLAGS	0	// end of word without flags or region; for
				// postponed prefix: no <pflags>
#నిర్వచించు BY_సూచీ	1	// child is shared, index follows
#నిర్వచించు BY_FLAGS	2	// end of word, <flags> byte follows; for
				// postponed prefix: <pflags> follows
#నిర్వచించు BY_FLAGS2	3	// end of word, <flags> and <flags2> bytes
				// follow; never used in prefix tree
#నిర్వచించు BY_SPECIAL  BY_FLAGS2	// highest special byte value

#నిర్వచించు ZERO_FLAG	65009	// used when flag is zero: "0"

// Flags used in .spl file for soundsalike flags.
#నిర్వచించు SAL_F0LLOWUP		1
#నిర్వచించు SAL_COLLAPSE		2
#నిర్వచించు SAL_REM_ACCENTS		4

#నిర్వచించు VIMSPELLMAGIC "VIMspell"  // string at start of Vim spell file
#నిర్వచించు VIMSPELLMAGICL 8
#నిర్వచించు VIMSPELLVERSION 50

// Section IDs.  Only renumber them when VIMSPELLVERSION changes!
#నిర్వచించు SN_REGION	0	// <regionname> section
#నిర్వచించు SN_CHARFLAGS	1	// charflags section
#నిర్వచించు SN_MIDWORD	2	// <midword> section
#నిర్వచించు SN_PREFCOND	3	// <prefcond> section
#నిర్వచించు SN_REP		4	// REP items section
#నిర్వచించు SN_SAL		5	// SAL items section
#నిర్వచించు SN_SOFO		6	// soundfolding section
#నిర్వచించు SN_MAP		7	// MAP items section
#నిర్వచించు SN_COMPOUND	8	// compound words section
#నిర్వచించు SN_SYLLABLE	9	// syllable section
#నిర్వచించు SN_NOBREAK	10	// NOBREAK section
#నిర్వచించు SN_SUGFILE	11	// timestamp for .sug file
#నిర్వచించు SN_REPSAL	12	// REPSAL items section
#నిర్వచించు SN_పదాలు	13	// common words
#నిర్వచించు SN_NOSPLITSUGS	14	// don't split word for suggestions
#నిర్వచించు SN_INFO		15	// info section
#నిర్వచించు SN_NOCOMPOUNDSUGS 16	// don't compound for suggestions
#నిర్వచించు SN_చివర		255	// end of sections

#నిర్వచించు SNF_REQUIRED	1	// <sectionflags>: required section

#నిర్వచించు CF_మాట		0x01
#నిర్వచించు CF_పై	0x02

// Max allowed length for COMPOUND section
#నిర్వచించు COMPOUND_MAX_LEN	100000

/*
 * Loop through all the siblings of a node (including the node)
 */
#నిర్వచించు FOR_ALL_కణుపు_SIBLINGS(కణుపు, np) \
    కొరకు ((np) = (కణుపు); (np) != ఖాళీ; (np) = (np)->wn_sibling)

స్థిర పూర్ణాంకం సమితి_spell_ముగింపు(spelltab_T	*new_st);
స్థిర పూర్ణాంకం write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv);
స్థిర పూర్ణాంకం read_region_section(FILE *fd, slang_T *slang, పూర్ణాంకం len);
స్థిర పూర్ణాంకం read_charflags_section(FILE *fd);
స్థిర పూర్ణాంకం read_prefcond_section(FILE *fd, slang_T *lp);
స్థిర పూర్ణాంకం read_rep_section(FILE *fd, garray_T *gap, పొట్టి *మొదటి);
స్థిర పూర్ణాంకం read_sal_section(FILE *fd, slang_T *slang);
స్థిర పూర్ణాంకం read_పదాలు_section(FILE *fd, slang_T *lp, పూర్ణాంకం len);
స్థిర పూర్ణాంకం read_sofo_section(FILE *fd, slang_T *slang);
స్థిర పూర్ణాంకం read_compound(FILE *fd, slang_T *slang, పూర్ణాంకం len);
స్థిర పూర్ణాంకం సమితి_sofo(slang_T *lp, char_u *from, char_u *గమ్యం);
స్థిర శూన్య సమితి_sal_మొదటి(slang_T *lp);
స్థిర పూర్ణాంకం *mb_str2wide(char_u *s);
స్థిర పూర్ణాంకం spell_read_వృక్షం(FILE *fd, char_u **bytsp, దీర్ఘ *bytsp_len, idx_T **idxsp, పూర్ణాంకం prefixtree, పూర్ణాంకం prefixcnt);
స్థిర idx_T read_వృక్షం_కణుపు(FILE *fd, char_u *byts, idx_T *idxs, పూర్ణాంకం maxidx, idx_T startidx, పూర్ణాంకం prefixtree, పూర్ణాంకం maxprefcondnr, పూర్ణాంకం లోతు);
స్థిర శూన్య సమితి_spell_charflags(char_u *flags, పూర్ణాంకం cnt, char_u *upp);
స్థిర పూర్ణాంకం సమితి_spell_chartab(char_u *fol, char_u *దిగువ, char_u *upp);
స్థిర శూన్య సమితి_map_str(slang_T *lp, char_u *map);


స్థిర అక్షరం *e_afftrailing = N_("Trailing text in %s line %d: %s");
స్థిర అక్షరం *e_affname = N_("Affix name too long in %s line %d: %s");
స్థిర అక్షరం *సందేశం_compressing = N_("Compressing word tree...");

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
spell_ఎక్కించు_file(
    char_u	*fname,
    char_u	*lang,
    slang_T	*old_lp,
    పూర్ణాంకం		silent)		// no error if file doesn't exist
{
    FILE	*fd;
    char_u	buf[VIMSPELLMAGICL];
    char_u	*p;
    పూర్ణాంకం		i;
    పూర్ణాంకం		n;
    పూర్ణాంకం		len;
    slang_T	*lp = ఖాళీ;
    పూర్ణాంకం		c = 0;
    పూర్ణాంకం		res;
    పూర్ణాంకం		did_estack_నెట్టు = FALSE;
    ESTACK_CHECK_DECLARATION;

    fd = mch_fopen((అక్షరం *)fname, "r");
    ఒకవేళ (fd == ఖాళీ)
    {
	ఒకవేళ (!silent)
	    semsg(_(e_cant_open_file_str), fname);
	లేకపోతే ఒకవేళ (p_verbose > 2)
	{
	    verbose_enter();
	    smsg((స్థిరరాశి అక్షరం *)e_cant_open_file_str, fname);
	    verbose_leave();
	}
	వెళ్ళు చివర_FAIL;
    }
    ఒకవేళ (p_verbose > 2)
    {
	verbose_enter();
	smsg(_("Reading spell file \"%s\""), fname);
	verbose_leave();
    }

    ఒకవేళ (old_lp == ఖాళీ)
    {
	lp = slang_alloc(lang);
	ఒకవేళ (lp == ఖాళీ)
	    వెళ్ళు చివర_FAIL;

	// Remember the file name, used to reload the file when it's updated.
	lp->sl_fname = vim_strsave(fname);
	ఒకవేళ (lp->sl_fname == ఖాళీ)
	    వెళ్ళు చివర_FAIL;

	// Check for .add.spl (_add.spl for VMS).
	lp->sl_కూడు = అక్షరశ్రేణి_శోధన((అక్షరం *)gettail(fname), SPL_FNAME_కూడు) != ఖాళీ;
    }
    లేకపోతే
	lp = old_lp;

    // Set sourcing_name, so that error messages mention the file name.
    estack_నెట్టు(ETYPE_SPELL, fname, 0);
    ESTACK_CHECK_అమరిక;
    did_estack_నెట్టు = TRUE;

    /*
     * <HEADER>: <fileID>
     */
    కొరకు (i = 0; i < VIMSPELLMAGICL; ++i)
	buf[i] = (c = getc(fd)) == EOF ? 0 : c;		// <fileID>
    ఒకవేళ (STRNCMP(buf, VIMSPELLMAGIC, VIMSPELLMAGICL) != 0)
    {
	emsg(_(e_this_does_not_look_like_spell_file));
	వెళ్ళు చివర_FAIL;
    }
    c = getc(fd);					// <versionnr>
    ఒకవేళ (c < VIMSPELLVERSION)
    {
	emsg(_(e_old_spell_file_needs_గమ్యం_be_updated));
	వెళ్ళు చివర_FAIL;
    }
    లేకపోతే ఒకవేళ (c > VIMSPELLVERSION)
    {
	emsg(_(e_spell_file_is_for_newer_సంస్కరణ_యొక్క_vim));
	వెళ్ళు చివర_FAIL;
    }


    /*
     * <SECTIONS>: <section> ... <sectionend>
     * <section>: <sectionID> <sectionflags> <sectionlen> (section contents)
     */
    కొరకు (;;)
    {
	n = getc(fd);			    // <sectionID> or <sectionend>
	ఒకవేళ (n == SN_చివర)
	    ఆపు;
	c = getc(fd);					// <sectionflags>
	len = get4c(fd);				// <sectionlen>
	ఒకవేళ (len < 0)
	    వెళ్ళు truncerr;

	res = 0;
	ఎంపిక (n)
	{
	    స్థితి SN_INFO:
		lp->sl_info = read_string(fd, len);	// <infotext>
		ఒకవేళ (lp->sl_info == ఖాళీ)
		    వెళ్ళు చివర_FAIL;
		ఆపు;

	    స్థితి SN_REGION:
		res = read_region_section(fd, lp, len);
		ఆపు;

	    స్థితి SN_CHARFLAGS:
		res = read_charflags_section(fd);
		ఆపు;

	    స్థితి SN_MIDWORD:
		lp->sl_midword = read_string(fd, len);	// <midword>
		ఒకవేళ (lp->sl_midword == ఖాళీ)
		    వెళ్ళు చివర_FAIL;
		ఆపు;

	    స్థితి SN_PREFCOND:
		res = read_prefcond_section(fd, lp);
		ఆపు;

	    స్థితి SN_REP:
		res = read_rep_section(fd, &lp->sl_rep, lp->sl_rep_మొదటి);
		ఆపు;

	    స్థితి SN_REPSAL:
		res = read_rep_section(fd, &lp->sl_repsal, lp->sl_repsal_మొదటి);
		ఆపు;

	    స్థితి SN_SAL:
		res = read_sal_section(fd, lp);
		ఆపు;

	    స్థితి SN_SOFO:
		res = read_sofo_section(fd, lp);
		ఆపు;

	    స్థితి SN_MAP:
		p = read_string(fd, len);		// <mapstr>
		ఒకవేళ (p == ఖాళీ)
		    వెళ్ళు చివర_FAIL;
		సమితి_map_str(lp, p);
		vim_free(p);
		ఆపు;

	    స్థితి SN_పదాలు:
		res = read_పదాలు_section(fd, lp, len);
		ఆపు;

	    స్థితి SN_SUGFILE:
		lp->sl_sugtime = get8ctime(fd);		// <timestamp>
		ఆపు;

	    స్థితి SN_NOSPLITSUGS:
		lp->sl_nosplitsugs = TRUE;
		ఆపు;

	    స్థితి SN_NOCOMPOUNDSUGS:
		lp->sl_nocompoundsugs = TRUE;
		ఆపు;

	    స్థితి SN_COMPOUND:
		res = read_compound(fd, lp, len);
		ఆపు;

	    స్థితి SN_NOBREAK:
		lp->sl_nobreak = TRUE;
		ఆపు;

	    స్థితి SN_SYLLABLE:
		lp->sl_syllable = read_string(fd, len);	// <syllable>
		ఒకవేళ (lp->sl_syllable == ఖాళీ)
		    వెళ్ళు చివర_FAIL;
		ఒకవేళ (init_syl_tab(lp) != OK)
		    వెళ్ళు చివర_FAIL;
		ఆపు;

	    అప్రమేయ:
		// Unsupported section.  When it's required give an error
		// message.  When it's not required skip the contents.
		ఒకవేళ (c & SNF_REQUIRED)
		{
		    emsg(_(e_unsupported_section_లోపల_spell_file));
		    వెళ్ళు చివర_FAIL;
		}
		అయితే (--len >= 0)
		    ఒకవేళ (getc(fd) < 0)
			వెళ్ళు truncerr;
		ఆపు;
	}
someerror:
	ఒకవేళ (res == SP_FORMERROR)
	{
	    emsg(_(e_format_error_లోపల_spell_file));
	    వెళ్ళు చివర_FAIL;
	}
	ఒకవేళ (res == SP_TRUNCERROR)
	{
truncerr:
	    emsg(_(e_truncated_spell_file));
	    వెళ్ళు చివర_FAIL;
	}
	ఒకవేళ (res == SP_OTHERERROR)
	    వెళ్ళు చివర_FAIL;
    }

    // <LWORDTREE>
    res = spell_read_వృక్షం(fd, &lp->sl_fbyts, &lp->sl_fbyts_len,
						      &lp->sl_fidxs, FALSE, 0);
    ఒకవేళ (res != 0)
	వెళ్ళు someerror;

    // <KWORDTREE>
    res = spell_read_వృక్షం(fd, &lp->sl_kbyts, ఖాళీ, &lp->sl_kidxs, FALSE, 0);
    ఒకవేళ (res != 0)
	వెళ్ళు someerror;

    // <PREFIXTREE>
    res = spell_read_వృక్షం(fd, &lp->sl_pbyts, ఖాళీ, &lp->sl_pidxs, TRUE,
							    lp->sl_prefixcnt);
    ఒకవేళ (res != 0)
	వెళ్ళు someerror;

    // For a new file link it in the list of spell files.
    ఒకవేళ (old_lp == ఖాళీ && lang != ఖాళీ)
    {
	lp->sl_తదుపరి = మొదటి_lang;
	మొదటి_lang = lp;
    }

    వెళ్ళు చివర_OK;

చివర_FAIL:
    ఒకవేళ (lang != ఖాళీ)
	// truncating the name signals the error to spell_load_lang()
	*lang = NUL;
    ఒకవేళ (lp != ఖాళీ && old_lp == ఖాళీ)
	slang_free(lp);
    lp = ఖాళీ;

చివర_OK:
    ఒకవేళ (fd != ఖాళీ)
	fclose(fd);
    ఒకవేళ (did_estack_నెట్టు)
    {
	ESTACK_CHECK_NOW;
	estack_తీయి();
    }

    తిరిగి lp;
}

/*
 * Fill in the wordcount fields for a trie.
 * Returns the total number of words.
 */
    స్థిర శూన్య
వృక్షం_లెక్క_పదాలు(char_u *byts, దీర్ఘ byts_len, idx_T *idxs)
{
    పూర్ణాంకం		లోతు;
    idx_T	arridx[MAXWLEN];
    పూర్ణాంకం		curi[MAXWLEN];
    పూర్ణాంకం		c;
    idx_T	n;
    పూర్ణాంకం		wordcount[MAXWLEN];

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;
    లోతు = 0;
    అయితే (లోతు >= 0 && !got_int)
    {
	ఒకవేళ (curi[లోతు] > byts[arridx[లోతు]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[లోతు]] = wordcount[లోతు];
	    ఒకవేళ (లోతు > 0)
		wordcount[లోతు - 1] += wordcount[లోతు];

	    --లోతు;
	    fast_breakcheck();
	}
	లేకపోతే
	{
	    // Do one more byte at this node.
	    n = arridx[లోతు] + curi[లోతు];
	    ++curi[లోతు];

	    c = byts[n];
	    ఒకవేళ (c == 0)
	    {
		// End of word, count it.
		++wordcount[లోతు];

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		అయితే (n + 1 < byts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[లోతు];
		}
	    }
	    లేకపోతే ఒకవేళ (లోతు < MAXWLEN - 1)
	    {
		// Normal char, go one level deeper to count the words.
		++లోతు;
		arridx[లోతు] = idxs[n];
		curi[లోతు] = 1;
		wordcount[లోతు] = 0;
	    }
	}
    }
}

/*
 * Load the .sug files for languages that have one and weren't loaded yet.
 */
    శూన్య
suggest_ఎక్కించు_files(శూన్య)
{
    langp_T	*lp;
    పూర్ణాంకం		lpi;
    slang_T	*slang;
    char_u	*dotp;
    FILE	*fd;
    char_u	buf[MAXWLEN];
    పూర్ణాంకం		i;
    time_t	timestamp;
    పూర్ణాంకం		wcount;
    పూర్ణాంకం		wordnr;
    garray_T	ga;
    పూర్ణాంకం		c;

    // Do this for all languages that support sound folding.
    కొరకు (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_నమోదు(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ఒకవేళ (slang->sl_sugtime != 0 && !slang->sl_sugloaded)
	{
	    // Change ".spl" to ".sug" and open the file.  When the file isn't
	    // found silently skip it.  Do set "sl_sugloaded" so that we
	    // don't try again and again.
	    slang->sl_sugloaded = TRUE;

	    dotp = vim_strrchr(slang->sl_fname, '.');
	    ఒకవేళ (dotp == ఖాళీ || fnamecmp(dotp, ".spl") != 0)
		కొనసాగు;
	    STRCPY(dotp, ".sug");
	    fd = mch_fopen((అక్షరం *)slang->sl_fname, "r");
	    ఒకవేళ (fd == ఖాళీ)
		వెళ్ళు nextone;

	    /*
	     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
	     */
	    కొరకు (i = 0; i < VIMSUGMAGICL; ++i)
		buf[i] = (c = getc(fd)) == EOF ? 0 : c;	// <fileID>
	    ఒకవేళ (STRNCMP(buf, VIMSUGMAGIC, VIMSUGMAGICL) != 0)
	    {
		semsg(_(e_this_does_not_look_like_sug_file_str),
							     slang->sl_fname);
		వెళ్ళు nextone;
	    }
	    c = getc(fd);				// <versionnr>
	    ఒకవేళ (c < VIMSUGVERSION)
	    {
		semsg(_(e_old_sug_file_needs_గమ్యం_be_updated_str),
							     slang->sl_fname);
		వెళ్ళు nextone;
	    }
	    లేకపోతే ఒకవేళ (c > VIMSUGVERSION)
	    {
		semsg(_(e_sug_file_is_for_newer_సంస్కరణ_యొక్క_vim_str),
							     slang->sl_fname);
		వెళ్ళు nextone;
	    }

	    // Check the timestamp, it must be exactly the same as the one in
	    // the .spl file.  Otherwise the word numbers won't match.
	    timestamp = get8ctime(fd);			// <timestamp>
	    ఒకవేళ (timestamp != slang->sl_sugtime)
	    {
		semsg(_(e_sug_file_doesnt_సరిపోలిక_spl_file_str),
							     slang->sl_fname);
		వెళ్ళు nextone;
	    }

	    /*
	     * <SUGWORDTREE>: <wordtree>
	     * Read the trie with the soundfolded words.
	     */
	    ఒకవేళ (spell_read_వృక్షం(fd, &slang->sl_sbyts, &slang->sl_sbyts_len,
			&slang->sl_sidxs, FALSE, 0) != 0)
	    {
someerror:
		semsg(_(e_error_while_reading_sug_file_str),
							     slang->sl_fname);
		slang_clear_sug(slang);
		వెళ్ళు nextone;
	    }

	    /*
	     * <SUGTABLE>: <sugwcount> <sugline> ...
	     *
	     * Read the table with word numbers.  We use a file buffer for
	     * this, because it's so much like a file with lines.  Makes it
	     * possible to swap the info and save on memory use.
	     */
	    slang->sl_sugbuf = open_spellbuf();
	    ఒకవేళ (slang->sl_sugbuf == ఖాళీ)
		వెళ్ళు someerror;
							    // <sugwcount>
	    wcount = get4c(fd);
	    ఒకవేళ (wcount < 0)
		వెళ్ళు someerror;

	    // Read all the wordnr lists into the buffer, one NUL terminated
	    // list per line.
	    ga_init2(&ga, 1, 100);
	    కొరకు (wordnr = 0; wordnr < wcount; ++wordnr)
	    {
		ga.ga_len = 0;
		కొరకు (;;)
		{
		    c = getc(fd);			    // <sugline>
		    ఒకవేళ (c < 0 || ga_grow(&ga, 1) == FAIL)
			వెళ్ళు someerror;
		    ((char_u *)ga.ga_దత్తాంశం)[ga.ga_len++] = c;
		    ఒకవేళ (c == NUL)
			ఆపు;
		}
		ఒకవేళ (ml_append_buf(slang->sl_sugbuf, (linenr_T)wordnr,
					 ga.ga_దత్తాంశం, ga.ga_len, TRUE) == FAIL)
		    వెళ్ళు someerror;
	    }
	    ga_clear(&ga);

	    /*
	     * Need to put word counts in the word tries, so that we can find
	     * a word by its number.
	     */
	    వృక్షం_లెక్క_పదాలు(slang->sl_fbyts, slang->sl_fbyts_len,
		    slang->sl_fidxs);
	    వృక్షం_లెక్క_పదాలు(slang->sl_sbyts, slang->sl_sbyts_len,
		    slang->sl_sidxs);

nextone:
	    ఒకవేళ (fd != ఖాళీ)
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
    స్థిర char_u *
read_cnt_string(FILE *fd, పూర్ణాంకం cnt_బైట్లు, పూర్ణాంకం *cntp)
{
    పూర్ణాంకం		cnt = 0;
    పూర్ణాంకం		i;
    char_u	*str;

    // read the length bytes, MSB first
    కొరకు (i = 0; i < cnt_బైట్లు; ++i)
    {
	పూర్ణాంకం c = getc(fd);

	ఒకవేళ (c == EOF)
	{
	    *cntp = SP_TRUNCERROR;
	    తిరిగి ఖాళీ;
	}
	cnt = (cnt << 8) + (గుర్తులేని)c;
    }
    *cntp = cnt;
    ఒకవేళ (cnt == 0)
	తిరిగి ఖాళీ;	    // nothing to read, return NULL

    str = read_string(fd, cnt);
    ఒకవేళ (str == ఖాళీ)
	*cntp = SP_OTHERERROR;
    తిరిగి str;
}

/*
 * Read SN_REGION: <regionname> ...
 * Return SP_*ERROR flags.
 */
    స్థిర పూర్ణాంకం
read_region_section(FILE *fd, slang_T *lp, పూర్ణాంకం len)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		c = 0;

    ఒకవేళ (len > MAXREGIONS * 2)
	తిరిగి SP_FORMERROR;
    కొరకు (i = 0; i < len; ++i)
	lp->sl_regions[i] = (c = getc(fd)) == EOF ? 0 : c; // <regionname>
    lp->sl_regions[len] = NUL;
    తిరిగి c == EOF ? SP_TRUNCERROR : 0;
}

/*
 * Read SN_CHARFLAGS section: <charflagslen> <charflags>
 *				<folcharslen> <folchars>
 * Return SP_*ERROR flags.
 */
    స్థిర పూర్ణాంకం
read_charflags_section(FILE *fd)
{
    char_u	*flags;
    char_u	*fol;
    పూర్ణాంకం		flagslen, follen;

    // <charflagslen> <charflags>
    flags = read_cnt_string(fd, 1, &flagslen);
    ఒకవేళ (flagslen < 0)
	తిరిగి flagslen;

    // <folcharslen> <folchars>
    fol = read_cnt_string(fd, 2, &follen);
    ఒకవేళ (follen < 0)
    {
	vim_free(flags);
	తిరిగి follen;
    }

    // Set the word-char flags and fill SPELL_ISUPPER() table.
    ఒకవేళ (flags != ఖాళీ && fol != ఖాళీ)
	సమితి_spell_charflags(flags, flagslen, fol);

    vim_free(flags);
    vim_free(fol);

    // When <charflagslen> is zero then <fcharlen> must also be zero.
    ఒకవేళ ((flags == ఖాళీ) != (fol == ఖాళీ))
	తిరిగి SP_FORMERROR;
    తిరిగి 0;
}

/*
 * Read SN_PREFCOND section.
 * Return SP_*ERROR flags.
 */
    స్థిర పూర్ణాంకం
read_prefcond_section(FILE *fd, slang_T *lp)
{
    పూర్ణాంకం		cnt;
    పూర్ణాంకం		i;
    పూర్ణాంకం		n;
    పూర్ణాంకం		c;
    char_u	*p;
    char_u	buf[MAXWLEN + 1];

    // <prefcondcnt> <prefcond> ...
    cnt = get2c(fd);					// <prefcondcnt>
    ఒకవేళ (cnt <= 0)
	తిరిగి SP_FORMERROR;

    lp->sl_prefprog = ALLOC_CLEAR_MULT(regprog_T *, cnt);
    ఒకవేళ (lp->sl_prefprog == ఖాళీ)
	తిరిగి SP_OTHERERROR;
    lp->sl_prefixcnt = cnt;

    కొరకు (i = 0; i < cnt; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	n = getc(fd);					// <condlen>
	ఒకవేళ (n < 0 || n >= MAXWLEN)
	    తిరిగి SP_FORMERROR;

	// When <condlen> is zero we have an empty condition.  Otherwise
	// compile the regexp program used to check for the condition.
	ఒకవేళ (n > 0)
	{
	    buf[0] = '^';	    // always match at one position only
	    p = buf + 1;
	    అయితే (n-- > 0)
		*p++ = (c = getc(fd)) == EOF ? 0 : c;	    // <condstr>
	    ఒకవేళ (c == EOF)
		ఆపు;
	    *p = NUL;
	    lp->sl_prefprog[i] = vim_regcomp(buf, RE_MAGIC + RE_STRING);
	}
    }
    తిరిగి 0;
}

/*
 * Read REP or REPSAL items section from "fd": <repcount> <rep> ...
 * Return SP_*ERROR flags.
 */
    స్థిర పూర్ణాంకం
read_rep_section(FILE *fd, garray_T *gap, పొట్టి *మొదటి)
{
    పూర్ణాంకం		cnt;
    fromto_T	*ftp;
    పూర్ణాంకం		i;

    cnt = get2c(fd);					// <repcount>
    ఒకవేళ (cnt < 0)
	తిరిగి SP_TRUNCERROR;

    ఒకవేళ (ga_grow(gap, cnt) == FAIL)
	తిరిగి SP_OTHERERROR;

    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
    కొరకు (; gap->ga_len < cnt; ++gap->ga_len)
    {
	ftp = &((fromto_T *)gap->ga_దత్తాంశం)[gap->ga_len];
	ftp->ft_from = read_cnt_string(fd, 1, &i);
	ఒకవేళ (i < 0)
	    తిరిగి i;
	ఒకవేళ (i == 0)
	    తిరిగి SP_FORMERROR;
	ftp->ft_గమ్యం = read_cnt_string(fd, 1, &i);
	ఒకవేళ (i <= 0)
	{
	    vim_free(ftp->ft_from);
	    ఒకవేళ (i < 0)
		తిరిగి i;
	    తిరిగి SP_FORMERROR;
	}
    }

    // Fill the first-index table.
    కొరకు (i = 0; i < 256; ++i)
	మొదటి[i] = -1;
    కొరకు (i = 0; i < gap->ga_len; ++i)
    {
	ftp = &((fromto_T *)gap->ga_దత్తాంశం)[i];
	ఒకవేళ (మొదటి[*ftp->ft_from] == -1)
	    మొదటి[*ftp->ft_from] = i;
    }
    తిరిగి 0;
}

/*
 * Read SN_SAL section: <salflags> <salcount> <sal> ...
 * Return SP_*ERROR flags.
 */
    స్థిర పూర్ణాంకం
read_sal_section(FILE *fd, slang_T *slang)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		cnt;
    garray_T	*gap;
    salitem_T	*smp;
    పూర్ణాంకం		ccnt;
    char_u	*p;

    slang->sl_sofo = FALSE;

    i = getc(fd);				// <salflags>
    ఒకవేళ (i & SAL_F0LLOWUP)
	slang->sl_followup = TRUE;
    ఒకవేళ (i & SAL_COLLAPSE)
	slang->sl_collapse = TRUE;
    ఒకవేళ (i & SAL_REM_ACCENTS)
	slang->sl_rem_accents = TRUE;

    cnt = get2c(fd);				// <salcount>
    ఒకవేళ (cnt < 0)
	తిరిగి SP_TRUNCERROR;

    gap = &slang->sl_sal;
    ga_init2(gap, పరిమాణం(salitem_T), 10);
    ఒకవేళ (ga_grow(gap, cnt + 1) == FAIL)
	తిరిగి SP_OTHERERROR;

    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
    కొరకు (; gap->ga_len < cnt; ++gap->ga_len)
    {
	పూర్ణాంకం	c = NUL;

	smp = &((salitem_T *)gap->ga_దత్తాంశం)[gap->ga_len];
	ccnt = getc(fd);			// <salfromlen>
	ఒకవేళ (ccnt < 0)
	    తిరిగి SP_TRUNCERROR;
	ఒకవేళ ((p = alloc(ccnt + 2)) == ఖాళీ)
	    తిరిగి SP_OTHERERROR;
	smp->sm_lead = p;

	// Read up to the first special char into sm_lead.
	కొరకు (i = 0; i < ccnt; ++i)
	{
	    c = getc(fd);			// <salfrom>
	    ఒకవేళ (vim_strchr((char_u *)"0123456789(-<^$", c) != ఖాళీ)
		ఆపు;
	    *p++ = c;
	}
	smp->sm_leadlen = (పూర్ణాంకం)(p - smp->sm_lead);
	*p++ = NUL;

	// Put (abc) chars in sm_oneof, if any.
	ఒకవేళ (c == '(')
	{
	    smp->sm_oneof = p;
	    కొరకు (++i; i < ccnt; ++i)
	    {
		c = getc(fd);			// <salfrom>
		ఒకవేళ (c == ')')
		    ఆపు;
		*p++ = c;
	    }
	    *p++ = NUL;
	    ఒకవేళ (++i < ccnt)
		c = getc(fd);
	}
	లేకపోతే
	    smp->sm_oneof = ఖాళీ;

	// Any following chars go in sm_rules.
	smp->sm_నియమాలు = p;
	ఒకవేళ (i < ccnt)
	    // store the char we got while checking for end of sm_lead
	    *p++ = c;
	కొరకు (++i; i < ccnt; ++i)
	    *p++ = (c = getc(fd)) == EOF ? 0 : c;	// <salfrom>
	*p++ = NUL;

	// <saltolen> <salto>
	smp->sm_గమ్యం = read_cnt_string(fd, 1, &ccnt);
	ఒకవేళ (ccnt < 0)
	{
	    vim_free(smp->sm_lead);
	    తిరిగి ccnt;
	}

	ఒకవేళ (has_mbyte)
	{
	    // convert the multi-byte strings to wide char strings
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = mb_charlen(smp->sm_lead);
	    ఒకవేళ (smp->sm_oneof == ఖాళీ)
		smp->sm_oneof_w = ఖాళీ;
	    లేకపోతే
		smp->sm_oneof_w = mb_str2wide(smp->sm_oneof);
	    ఒకవేళ (smp->sm_గమ్యం == ఖాళీ)
		smp->sm_గమ్యం_w = ఖాళీ;
	    లేకపోతే
		smp->sm_గమ్యం_w = mb_str2wide(smp->sm_గమ్యం);
	    ఒకవేళ (smp->sm_lead_w == ఖాళీ
		    || (smp->sm_oneof_w == ఖాళీ && smp->sm_oneof != ఖాళీ)
		    || (smp->sm_గమ్యం_w == ఖాళీ && smp->sm_గమ్యం != ఖాళీ))
	    {
		vim_free(smp->sm_lead);
		vim_free(smp->sm_గమ్యం);
		vim_free(smp->sm_lead_w);
		vim_free(smp->sm_oneof_w);
		vim_free(smp->sm_గమ్యం_w);
		తిరిగి SP_OTHERERROR;
	    }
	}
    }

    ఒకవేళ (gap->ga_len > 0)
    {
	// Add one extra entry to mark the end with an empty sm_lead.  Avoids
	// that we need to check the index every time.
	smp = &((salitem_T *)gap->ga_దత్తాంశం)[gap->ga_len];
	ఒకవేళ ((p = alloc(1)) == ఖాళీ)
	    తిరిగి SP_OTHERERROR;
	p[0] = NUL;
	smp->sm_lead = p;
	smp->sm_leadlen = 0;
	smp->sm_oneof = ఖాళీ;
	smp->sm_నియమాలు = p;
	smp->sm_గమ్యం = ఖాళీ;
	ఒకవేళ (has_mbyte)
	{
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = 0;
	    smp->sm_oneof_w = ఖాళీ;
	    smp->sm_గమ్యం_w = ఖాళీ;
	}
	++gap->ga_len;
    }

    // Fill the first-index table.
    సమితి_sal_మొదటి(slang);

    తిరిగి 0;
}

/*
 * Read SN_WORDS: <word> ...
 * Return SP_*ERROR flags.
 */
    స్థిర పూర్ణాంకం
read_పదాలు_section(FILE *fd, slang_T *lp, పూర్ణాంకం len)
{
    పూర్ణాంకం		పూర్తి = 0;
    పూర్ణాంకం		i;
    పూర్ణాంకం		c;
    char_u	మాట[MAXWLEN];

    అయితే (పూర్తి < len)
    {
	// Read one word at a time.
	కొరకు (i = 0; ; ++i)
	{
	    c = getc(fd);
	    ఒకవేళ (c == EOF)
		తిరిగి SP_TRUNCERROR;
	    మాట[i] = c;
	    ఒకవేళ (మాట[i] == NUL)
		ఆపు;
	    ఒకవేళ (i == MAXWLEN - 1)
		తిరిగి SP_FORMERROR;
	}

	// Init the count to 10.
	లెక్క_common_మాట(lp, మాట, -1, 10);
	పూర్తి += i + 1;
    }
    తిరిగి 0;
}

/*
 * SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
 * Return SP_*ERROR flags.
 */
    స్థిర పూర్ణాంకం
read_sofo_section(FILE *fd, slang_T *slang)
{
    పూర్ణాంకం		cnt;
    char_u	*from, *గమ్యం;
    పూర్ణాంకం		res;

    // <sofofromlen> <sofofrom>
    from = read_cnt_string(fd, 2, &cnt);
    ఒకవేళ (cnt < 0)
	తిరిగి cnt;

    // <sofotolen> <sofoto>
    గమ్యం = read_cnt_string(fd, 2, &cnt);
    ఒకవేళ (cnt < 0)
    {
	vim_free(from);
	తిరిగి cnt;
    }

    // Store the info in slang->sl_sal and/or slang->sl_sal_first.
    ఒకవేళ (from != ఖాళీ && గమ్యం != ఖాళీ)
	res = సమితి_sofo(slang, from, గమ్యం);
    లేకపోతే ఒకవేళ (from != ఖాళీ || గమ్యం != ఖాళీ)
	res = SP_FORMERROR;    // only one of two strings is an error
    లేకపోతే
	res = 0;

    vim_free(from);
    vim_free(గమ్యం);
    తిరిగి res;
}

/*
 * Read the compound section from the .spl file:
 *	<compmax> <compminlen> <compsylmax> <compoptions> <compflags>
 * Returns SP_*ERROR flags.
 */
    స్థిర పూర్ణాంకం
read_compound(FILE *fd, slang_T *slang, పూర్ణాంకం len)
{
    పూర్ణాంకం		todo = len;
    పూర్ణాంకం		c;
    పూర్ణాంకం		atstart;
    char_u	*pat;
    char_u	*pp;
    char_u	*cp;
    char_u	*ap;
    char_u	*crp;
    పూర్ణాంకం		cnt;
    garray_T	*gap;
    size_t	patsize;
    size_t	flagsize;

    ఒకవేళ (todo < 2)
	తిరిగి SP_FORMERROR;	// need at least two bytes

    --todo;
    c = getc(fd);					// <compmax>
    ఒకవేళ (c < 2)
	c = MAXWLEN;
    slang->sl_compmax = c;

    --todo;
    c = getc(fd);					// <compminlen>
    ఒకవేళ (c < 1)
	c = 0;
    slang->sl_compminlen = c;

    --todo;
    c = getc(fd);					// <compsylmax>
    ఒకవేళ (c < 1)
	c = MAXWLEN;
    slang->sl_compsylmax = c;

    c = getc(fd);					// <compoptions>
    ఒకవేళ (c != 0)
	ungetc(c, fd);	    // be backwards compatible with Vim 7.0b
    లేకపోతే
    {
	--todo;
	c = getc(fd);	    // only use the lower byte for now
	--todo;
	slang->sl_compoptions = c;

	gap = &slang->sl_comppat;
	c = get2c(fd);					// <comppatcount>
	ఒకవేళ (c < 0)
	    తిరిగి SP_TRUNCERROR;
	todo -= 2;
	ga_init2(gap, పరిమాణం(char_u *), c);
	ఒకవేళ (ga_grow(gap, c) == OK)
	    అయితే (--c >= 0)
	    {
		((char_u **)(gap->ga_దత్తాంశం))[gap->ga_len++] =
						  read_cnt_string(fd, 1, &cnt);
					    // <comppatlen> <comppattext>
		ఒకవేళ (cnt < 0)
		    తిరిగి cnt;
		todo -= cnt + 1;
	    }
    }
    ఒకవేళ (todo < 0)
	తిరిగి SP_FORMERROR;

    // Turn the COMPOUNDRULE items into a regexp pattern:
    // "a[bc]/a*b+" -> "^\(a[bc]\|a*b\+\)$".
    // Inserting backslashes may double the length, "^\(\)$<Nul>" is 7 bytes.
    // Conversion to utf-8 may double the size.
    ఒకవేళ ((size_t)todo > COMPOUND_MAX_LEN)
	తిరిగి SP_FORMERROR;
    patsize = (size_t)todo * 2 + 7;
    ఒకవేళ (enc_utf8)
	patsize += (size_t)todo * 2;
    flagsize = (size_t)todo + 1;
    pat = alloc(patsize);
    ఒకవేళ (pat == ఖాళీ)
	తిరిగి SP_OTHERERROR;

    // We also need a list of all flags that can appear at the start and one
    // for all flags.
    cp = alloc(flagsize);
    ఒకవేళ (cp == ఖాళీ)
    {
	vim_free(pat);
	తిరిగి SP_OTHERERROR;
    }
    slang->sl_compstartflags = cp;
    *cp = NUL;

    ap = alloc(flagsize);
    ఒకవేళ (ap == ఖాళీ)
    {
	vim_free(pat);
	తిరిగి SP_OTHERERROR;
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
    అయితే (todo-- > 0)
    {
	c = getc(fd);					// <compflags>
	ఒకవేళ (c == EOF)
	{
	    vim_free(pat);
	    తిరిగి SP_TRUNCERROR;
	}

	// Add all flags to "sl_compallflags".
	ఒకవేళ (vim_strchr((char_u *)"?*+[]/", c) == ఖాళీ
		&& !byte_లోపల_str(slang->sl_compallflags, c))
	{
	    *ap++ = c;
	    *ap = NUL;
	}

	ఒకవేళ (atstart != 0)
	{
	    // At start of item: copy flags to "sl_compstartflags".  For a
	    // [abc] item set "atstart" to 2 and copy up to the ']'.
	    ఒకవేళ (c == '[')
		atstart = 2;
	    లేకపోతే ఒకవేళ (c == ']')
		atstart = 0;
	    లేకపోతే
	    {
		ఒకవేళ (!byte_లోపల_str(slang->sl_compstartflags, c))
		{
		    *cp++ = c;
		    *cp = NUL;
		}
		ఒకవేళ (atstart == 1)
		    atstart = 0;
	    }
	}

	// Copy flag to "sl_comprules", unless we run into a wildcard.
	ఒకవేళ (crp != ఖాళీ)
	{
	    ఒకవేళ (c == '?' || c == '+' || c == '*')
	    {
		VIM_CLEAR(slang->sl_comprules);
		crp = ఖాళీ;
	    }
	    లేకపోతే
		*crp++ = c;
	}

	ఒకవేళ (c == '/')	    // slash separates two items
	{
	    *pp++ = '\\';
	    *pp++ = '|';
	    atstart = 1;
	}
	లేకపోతే		    // normal char, "[abc]" and '*' are copied as-is
	{
	    ఒకవేళ (c == '?' || c == '+' || c == '~')
		*pp++ = '\\';	    // "a?" becomes "a\?", "a+" becomes "a\+"
	    ఒకవేళ (enc_utf8)
		pp += mb_char2bytes(c, pp);
	    లేకపోతే
		*pp++ = c;
	}
    }

    *pp++ = '\\';
    *pp++ = ')';
    *pp++ = '$';
    *pp = NUL;

    ఒకవేళ (crp != ఖాళీ)
	*crp = NUL;

    slang->sl_compprog = vim_regcomp(pat, RE_MAGIC + RE_STRING + RE_STRICT);
    vim_free(pat);
    ఒకవేళ (slang->sl_compprog == ఖాళీ)
	తిరిగి SP_FORMERROR;

    తిరిగి 0;
}

/*
 * Set the SOFOFROM and SOFOTO items in language "lp".
 * Returns SP_*ERROR flags when there is something wrong.
 */
    స్థిర పూర్ణాంకం
సమితి_sofo(slang_T *lp, char_u *from, char_u *గమ్యం)
{
    పూర్ణాంకం		i;

    garray_T	*gap;
    char_u	*s;
    char_u	*p;
    పూర్ణాంకం		c;
    పూర్ణాంకం		*inp;

    ఒకవేళ (has_mbyte)
    {
	free_sal_వస్తువులు(&lp->sl_sal);

	// Use "sl_sal" as an array with 256 pointers to a list of wide
	// characters.  The index is the low byte of the character.
	// The list contains from-to pairs with a terminating NUL.
	// sl_sal_first[] is used for latin1 "from" characters.
	gap = &lp->sl_sal;
	ga_init2(gap, పరిమాణం(పూర్ణాంకం *), 1);
	ఒకవేళ (ga_grow(gap, 256) == FAIL)
	    తిరిగి SP_OTHERERROR;
	vim_memset(gap->ga_దత్తాంశం, 0, పరిమాణం(పూర్ణాంకం *) * 256);
	gap->ga_len = 256;
	lp->sl_sofo = TRUE;

	// First count the number of items for each list.  Temporarily use
	// sl_sal_first[] for this.  Reset it first: a preceding SN_SAL section
	// may have set the entries to -1 via set_sal_first().
	vim_memset(lp->sl_sal_మొదటి, 0, పరిమాణం(salfirst_T) * 256);
	కొరకు (p = from, s = గమ్యం; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    MB_CPTR_ADV(s);
	    ఒకవేళ (c >= 256)
		++lp->sl_sal_మొదటి[c & 0xff];
	}
	ఒకవేళ (*p != NUL || *s != NUL)	    // lengths differ
	    తిరిగి SP_FORMERROR;

	// Allocate the lists.
	కొరకు (i = 0; i < 256; ++i)
	    ఒకవేళ (lp->sl_sal_మొదటి[i] > 0)
	    {
		p = alloc(పరిమాణం(పూర్ణాంకం) * (lp->sl_sal_మొదటి[i] * 2 + 1));
		ఒకవేళ (p == ఖాళీ)
		    తిరిగి SP_OTHERERROR;
		((పూర్ణాంకం **)gap->ga_దత్తాంశం)[i] = (పూర్ణాంకం *)p;
		*(పూర్ణాంకం *)p = 0;
	    }

	// Put the characters up to 255 in sl_sal_first[] the rest in a sl_sal
	// list.
	vim_memset(lp->sl_sal_మొదటి, 0, పరిమాణం(salfirst_T) * 256);
	కొరకు (p = from, s = గమ్యం; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    i = mb_cptr2char_adv(&s);
	    ఒకవేళ (c >= 256)
	    {
		// Append the from-to chars at the end of the list with
		// the low byte.
		inp = ((పూర్ణాంకం **)gap->ga_దత్తాంశం)[c & 0xff];
		అయితే (*inp != 0)
		    ++inp;
		*inp++ = c;		// from char
		*inp++ = i;		// to char
		*inp++ = NUL;		// NUL at the end
	    }
	    లేకపోతే
		// mapping byte to char is done in sl_sal_first[]
		lp->sl_sal_మొదటి[c] = i;
	}
    }
    లేకపోతే
    {
	// mapping bytes to bytes is done in sl_sal_first[]
	ఒకవేళ (STRLEN(from) != STRLEN(గమ్యం))
	    తిరిగి SP_FORMERROR;

	కొరకు (i = 0; గమ్యం[i] != NUL; ++i)
	    lp->sl_sal_మొదటి[from[i]] = గమ్యం[i];
	lp->sl_sal.ga_len = 1;		// indicates we have soundfolding
	lp->sl_sofo = TRUE;
    }

    తిరిగి 0;
}

/*
 * Fill the first-index table for "lp".
 */
    స్థిర శూన్య
సమితి_sal_మొదటి(slang_T *lp)
{
    salfirst_T	*sfirst;
    పూర్ణాంకం		i;
    salitem_T	*smp;
    పూర్ణాంకం		c;
    garray_T	*gap = &lp->sl_sal;

    sfirst = lp->sl_sal_మొదటి;
    కొరకు (i = 0; i < 256; ++i)
	sfirst[i] = -1;
    smp = (salitem_T *)gap->ga_దత్తాంశం;
    కొరకు (i = 0; i < gap->ga_len; ++i)
    {
	ఒకవేళ (has_mbyte)
	    // Use the lowest byte of the first character.  For latin1 it's
	    // the character, for other encodings it should differ for most
	    // characters.
	    c = *smp[i].sm_lead_w & 0xff;
	లేకపోతే
	    c = *smp[i].sm_lead;
	ఒకవేళ (sfirst[c] == -1)
	{
	    sfirst[c] = i;
	    ఒకవేళ (has_mbyte)
	    {
		పూర్ణాంకం		n;

		// Make sure all entries with this byte are following each
		// other.  Move the ones that are in the wrong position.  Do
		// keep the same ordering!
		అయితే (i + 1 < gap->ga_len
				       && (*smp[i + 1].sm_lead_w & 0xff) == c)
		    // Skip over entry with same index byte.
		    ++i;

		కొరకు (n = 1; i + n < gap->ga_len; ++n)
		    ఒకవేళ ((*smp[i + n].sm_lead_w & 0xff) == c)
		    {
			salitem_T  tsal;

			// Move entry with same index byte after the entries
			// we already found.
			++i;
			--n;
			tsal = smp[i + n];
			mch_memmove(smp + i + 1, smp + i,
						       పరిమాణం(salitem_T) * n);
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
    స్థిర పూర్ణాంకం *
mb_str2wide(char_u *s)
{
    పూర్ణాంకం		*res;
    char_u	*p;
    పూర్ణాంకం		i = 0;

    res = ALLOC_MULT(పూర్ణాంకం, mb_charlen(s) + 1);
    ఒకవేళ (res == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (p = s; *p != NUL; )
	res[i++] = mb_ptr2char_adv(&p);
    res[i] = NUL;
    తిరిగి res;
}

/*
 * Read a tree from the .spl or .sug file.
 * Allocates the memory and stores pointers in "bytsp" and "idxsp".
 * This is skipped when the tree has zero length.
 * Returns zero when OK, SP_ value for an error.
 */
    స్థిర పూర్ణాంకం
spell_read_వృక్షం(
    FILE	*fd,
    char_u	**bytsp,
    దీర్ఘ	*bytsp_len,
    idx_T	**idxsp,
    పూర్ణాంకం		prefixtree,	// TRUE for the prefix tree
    పూర్ణాంకం		prefixcnt)	// when "prefixtree" is TRUE: prefix count
{
    దీర్ఘ	len;
    పూర్ణాంకం		idx;
    char_u	*bp;
    idx_T	*ip;

    // The tree size was computed when writing the file, so that we can
    // allocate it as one long block. <nodecount>
    len = get4c(fd);
    ఒకవేళ (len < 0)
	తిరిగి SP_TRUNCERROR;
    ఒకవేళ ((size_t)len > కొలత_MAX / పరిమాణం(పూర్ణాంకం))
	// Invalid length, multiply with sizeof(int) would overflow.
	తిరిగి SP_FORMERROR;
    ఒకవేళ (len <= 0)
	తిరిగి 0;

    // Allocate the byte array.  Zero-initialize so that any position the
    // tree does not visit reads as 0; a stray BY_INDEX shared reference
    // into such a slot then behaves as end-of-word in spellsuggest()
    // instead of consuming an arbitrary heap byte as a siblingcount.
    bp = alloc_clear(len);
    ఒకవేళ (bp == ఖాళీ)
	తిరిగి SP_OTHERERROR;
    *bytsp = bp;
    ఒకవేళ (bytsp_len != ఖాళీ)
	*bytsp_len = len;

    // Allocate the index array.
    ip = lalloc_clear(len * పరిమాణం(పూర్ణాంకం), TRUE);
    ఒకవేళ (ip == ఖాళీ)
	తిరిగి SP_OTHERERROR;
    *idxsp = ip;

    // Recursively read the tree and store it in the array.
    idx = read_వృక్షం_కణుపు(fd, bp, ip, len, 0, prefixtree, prefixcnt, 0);
    ఒకవేళ (idx < 0)
	తిరిగి idx;
    ఒకవేళ (idx != len)
	తిరిగి SP_FORMERROR;
    తిరిగి 0;
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
    స్థిర idx_T
read_వృక్షం_కణుపు(
    FILE	*fd,
    char_u	*byts,
    idx_T	*idxs,
    పూర్ణాంకం		maxidx,		    // size of arrays
    idx_T	startidx,	    // current index in "byts" and "idxs"
    పూర్ణాంకం		prefixtree,	    // TRUE for reading PREFIXTREE
    పూర్ణాంకం		maxprefcondnr,	    // maximum for <prefcondnr>
    పూర్ణాంకం		లోతు)		    // recursion level
{
    పూర్ణాంకం		len;
    పూర్ణాంకం		i;
    పూర్ణాంకం		n;
    idx_T	idx = startidx;
    పూర్ణాంకం		c;
    పూర్ణాంకం		c2;
#నిర్వచించు SHARED_ముసుగు	0x8000000

    // Bail out on a crafted .spl whose tree recurses beyond the maximum
    // word length: each tree level corresponds to one byte of a word, so
    // any well-formed file has depth <= MAXWLEN.
    ఒకవేళ (లోతు > MAXWLEN)
	తిరిగి SP_FORMERROR;

    len = getc(fd);					// <siblingcount>
    ఒకవేళ (len <= 0)
	తిరిగి SP_TRUNCERROR;

    ఒకవేళ (len >= maxidx - startidx)
	తిరిగి SP_FORMERROR;
    byts[idx++] = len;

    // Read the byte values, flag/region bytes and shared indexes.
    కొరకు (i = 1; i <= len; ++i)
    {
	c = getc(fd);					// <byte>
	ఒకవేళ (c < 0)
	    తిరిగి SP_TRUNCERROR;
	ఒకవేళ (c <= BY_SPECIAL)
	{
	    ఒకవేళ (c == BY_NOFLAGS && !prefixtree)
	    {
		// No flags, all regions.
		idxs[idx] = 0;
		c = 0;
	    }
	    లేకపోతే ఒకవేళ (c != BY_సూచీ)
	    {
		ఒకవేళ (prefixtree)
		{
		    // Read the optional pflags byte, the prefix ID and the
		    // condition nr.  In idxs[] store the prefix ID in the low
		    // byte, the condition index shifted up 8 bits, the flags
		    // shifted up 24 bits.
		    ఒకవేళ (c == BY_FLAGS)
		    {
			n = getc(fd);			// <pflags>
			ఒకవేళ (n < 0)
			    తిరిగి SP_TRUNCERROR;
			c = n << 24;
		    }
		    లేకపోతే
			c = 0;

		    n = getc(fd);			// <affixID>
		    ఒకవేళ (n < 0)
			తిరిగి SP_TRUNCERROR;
		    c |= n;

		    n = get2c(fd);			// <prefcondnr>
		    ఒకవేళ (n < 0 || n >= maxprefcondnr)
			తిరిగి SP_FORMERROR;
		    c |= (n << 8);
		}
		లేకపోతే // c must be BY_FLAGS or BY_FLAGS2
		{
		    // Read flags and optional region and prefix ID.  In
		    // idxs[] the flags go in the low two bytes, region above
		    // that and prefix ID above the region.
		    c2 = c;
		    c = getc(fd);			// <flags>
		    ఒకవేళ (c < 0)
			తిరిగి SP_TRUNCERROR;
		    ఒకవేళ (c2 == BY_FLAGS2)
		    {
			n = getc(fd);			// <flags2>
			ఒకవేళ (n < 0)
			    తిరిగి SP_TRUNCERROR;
			c = (n << 8) + c;
		    }
		    ఒకవేళ (c & WF_REGION)
		    {
			n = getc(fd);			// <region>
			ఒకవేళ (n < 0)
			    తిరిగి SP_TRUNCERROR;
			c = (n << 16) + c;
		    }
		    ఒకవేళ (c & WF_AFX)
		    {
			n = getc(fd);			// <affixID>
			ఒకవేళ (n < 0)
			    తిరిగి SP_TRUNCERROR;
			c = (n << 24) + c;
		    }
		}

		idxs[idx] = c;
		c = 0;
	    }
	    లేకపోతే // c == BY_INDEX
	    {
							// <nodeidx>
		n = get3c(fd);
		ఒకవేళ (n < 0 || n >= maxidx)
		    తిరిగి SP_FORMERROR;
		idxs[idx] = n + SHARED_ముసుగు;
		c = getc(fd);				// <xbyte>
	    }
	}
	byts[idx++] = c;
    }

    // Recursively read the children for non-shared siblings.
    // Skip the end-of-word ones (zero byte value) and the shared ones (and
    // remove SHARED_MASK)
    కొరకు (i = 1; i <= len; ++i)
	ఒకవేళ (byts[startidx + i] != 0)
	{
	    ఒకవేళ (idxs[startidx + i] & SHARED_ముసుగు)
		idxs[startidx + i] &= ~SHARED_ముసుగు;
	    లేకపోతే
	    {
		idxs[startidx + i] = idx;
		idx = read_వృక్షం_కణుపు(fd, byts, idxs, maxidx, idx,
					prefixtree, maxprefcondnr, లోతు + 1);
		ఒకవేళ (idx < 0)
		    ఆపు;
	    }
	}

    తిరిగి idx;
}

/*
 * Reload the spell file "fname" if it's loaded.
 */
    స్థిర శూన్య
spell_reload_one(
    char_u	*fname,
    పూర్ణాంకం		added_మాట)	// invoked through "zg"
{
    slang_T	*slang;
    పూర్ణాంకం		didit = FALSE;

    FOR_ALL_SPELL_LANGS(slang)
    {
	ఒకవేళ (fullpathcmp(fname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	{
	    slang_clear(slang);
	    ఒకవేళ (spell_ఎక్కించు_file(fname, ఖాళీ, slang, FALSE) == ఖాళీ)
		// reloading failed, clear the language
		slang_clear(slang);
	    redraw_all_later(UPD_SOME_VALID);
	    didit = TRUE;
	}
    }

    // When "zg" was used and the file wasn't loaded yet, should redo
    // 'spelllang' to load it now.
    ఒకవేళ (added_మాట && !didit)
	parse_spelllang(curwin);
}


/*
 * Functions for ":mkspell".
 */

#నిర్వచించు MAXLINELEN  500		// Maximum length in bytes of a line in a .aff
				// and .dic file.
/*
 * Main structure to store the contents of a ".aff" file.
 */
రకనిర్వచనం నిర్మాణం afffile_S
{
    char_u	*af_enc;	// "SET", normalized, alloc'ed string or NULL
    పూర్ణాంకం		af_flagtype;	// AFT_CHAR, AFT_LONG, AFT_NUM or AFT_CAPLONG
    గుర్తులేని	af_rare;	// RARE ID for rare word
    గుర్తులేని	af_keepcase;	// KEEPCASE ID for keep-case word
    గుర్తులేని	af_bad;		// BAD ID for banned word
    గుర్తులేని	af_needaffix;	// NEEDAFFIX ID
    గుర్తులేని	af_circumfix;	// CIRCUMFIX ID
    గుర్తులేని	af_needcomp;	// NEEDCOMPOUND ID
    గుర్తులేని	af_comproot;	// COMPOUNDROOT ID
    గుర్తులేని	af_compforbid;	// COMPOUNDFORBIDFLAG ID
    గుర్తులేని	af_comppermit;	// COMPOUNDPERMITFLAG ID
    గుర్తులేని	af_nosuggest;	// NOSUGGEST ID
    పూర్ణాంకం		af_pfxpostpone;	// postpone prefixes without chop string and
				// without flags
    పూర్ణాంకం		af_ignoreextra;	// IGNOREEXTRA present
    hashtab_T	af_pref;	// hashtable for prefixes, affheader_T
    hashtab_T	af_suff;	// hashtable for suffixes, affheader_T
    hashtab_T	af_comp;	// hashtable for compound flags, compitem_T
} afffile_T;

#నిర్వచించు AFT_CHAR	0	// flags are one character
#నిర్వచించు AFT_LONG	1	// flags are two characters
#నిర్వచించు AFT_CAPLONG	2	// flags are one or two characters
#నిర్వచించు AFT_అంకె		3	// flags are numbers, comma separated

రకనిర్వచనం నిర్మాణం affentry_S affentry_T;
// Affix entry from ".aff" file.  Used for prefixes and suffixes.
నిర్మాణం affentry_S
{
    affentry_T	*ae_తదుపరి;	// next affix with same name/number
    char_u	*ae_chop;	// text to chop off basic word (can be NULL)
    char_u	*ae_కూడు;	// text to add to basic word (can be NULL)
    char_u	*ae_flags;	// flags on the affix (can be NULL)
    char_u	*ae_cond;	// condition (NULL for ".")
    regprog_T	*ae_prog;	// regexp program for ae_cond or NULL
    అక్షరం	ae_compforbid;	// COMPOUNDFORBIDFLAG found
    అక్షరం	ae_comppermit;	// COMPOUNDPERMITFLAG found
};

#నిర్వచించు AH_కీలకం_LEN 17		// 2 x 8 bytes + NUL

// Affix header from ".aff" file.  Used for af_pref and af_suff.
రకనిర్వచనం నిర్మాణం affheader_S
{
    char_u	ah_కీలకం[AH_కీలకం_LEN]; // key for hashtab == name of affix
    గుర్తులేని	ah_flag;	// affix name as number, uses "af_flagtype"
    పూర్ణాంకం		ah_newID;	// prefix ID after renumbering; 0 if not used
    పూర్ణాంకం		ah_combine;	// suffix may combine with prefix
    పూర్ణాంకం		ah_follows;	// another affix block should be following
    affentry_T	*ah_మొదటి;	// first affix entry
} affheader_T;

#నిర్వచించు HI2AH(hi)   ((affheader_T *)(hi)->hi_కీలకం)

// Flag used in compound items.
రకనిర్వచనం నిర్మాణం compitem_S
{
    char_u	ci_కీలకం[AH_కీలకం_LEN]; // key for hashtab == name of compound
    గుర్తులేని	ci_flag;	// affix name as number, uses "af_flagtype"
    పూర్ణాంకం		ci_newID;	// affix ID after renumbering.
} compitem_T;

#నిర్వచించు HI2CI(hi)   ((compitem_T *)(hi)->hi_కీలకం)

/*
 * Structure that is used to store the items in the word tree.  This avoids
 * the need to keep track of each allocated thing, everything is freed all at
 * once after ":mkspell" is done.
 * Note: "sb_next" must be just before "sb_data" to make sure the alignment of
 * "sb_data" is correct for systems where pointers must be aligned on
 * pointer-size boundaries and sizeof(pointer) > sizeof(int) (e.g., Sparc).
 */
#నిర్వచించు  SBLOCKSIZE 16000	// size of sb_data
రకనిర్వచనం నిర్మాణం sblock_S sblock_T;
నిర్మాణం sblock_S
{
    పూర్ణాంకం		sb_వాడిన;	// nr of bytes already in use
    sblock_T	*sb_తదుపరి;	// next block in list
    char_u	sb_దత్తాంశం[1];	// data, actually longer
};

/*
 * A node in the tree.
 */
రకనిర్వచనం నిర్మాణం wordnode_S wordnode_T;
నిర్మాణం wordnode_S
{
    సంఘం   // shared to save space
    {
	char_u	hashkey[6];	// the hash key, only used while compressing
	పూర్ణాంకం	సూచీ;		// index in written nodes (valid after first
				// round)
    } wn_u1;
    సంఘం   // shared to save space
    {
	wordnode_T *తదుపరి;	// next node with same hash key
	wordnode_T *wnode;	// parent node that will write this node
    } wn_u2;
    wordnode_T	*wn_బిడ్డ;	// child (next byte in word)
    wordnode_T  *wn_sibling;	// next sibling (alternate byte in word,
				// always sorted)
    పూర్ణాంకం		wn_refs;	// Nr. of references to this node.  Only
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
    పొట్టి	wn_region;	// region mask

#నిర్వచితమైతే SPELL_PRINTTREE
    పూర్ణాంకం		wn_nr;		// sequence nr for printing
#అయితే_ముగింపు
};

#నిర్వచించు WN_ముసుగు	 0xffff		// mask relevant bits of "wn_flags"

#నిర్వచించు HI2WN(hi)    (wordnode_T *)((hi)->hi_కీలకం)

/*
 * Info used while reading the spell files.
 */
రకనిర్వచనం నిర్మాణం spellinfo_S
{
    wordnode_T	*si_foldroot;	// tree with case-folded words
    దీర్ఘ	si_foldwcount;	// nr of words in si_foldroot

    wordnode_T	*si_keeproot;	// tree with keep-case words
    దీర్ఘ	si_keepwcount;	// nr of words in si_keeproot

    wordnode_T	*si_prefroot;	// tree with postponed prefixes

    దీర్ఘ	si_sugtree;	// creating the soundfolding trie

    sblock_T	*si_ఖండాలు;	// memory blocks used
    దీర్ఘ	si_ఖండాలు_cnt;	// memory blocks allocated
    పూర్ణాంకం		si_did_emsg;	// TRUE when ran out of memory

    దీర్ఘ	si_compress_cnt;    // words to add before lowering
				    // compression limit
    wordnode_T	*si_మొదటి_free; // List of nodes that have been freed during
				// compression, linked by "wn_child" field.
    దీర్ఘ	si_free_లెక్క;	// number of nodes in si_first_free
#నిర్వచితమైతే SPELL_PRINTTREE
    పూర్ణాంకం		si_wordnode_nr;	// sequence nr for nodes
#అయితే_ముగింపు
    buf_T	*si_spellbuf;	// buffer used to store soundfold word table

    పూర్ణాంకం		si_ascii;	// handling only ASCII words
    పూర్ణాంకం		si_కూడు;		// addition file
    పూర్ణాంకం		si_clear_chartab;   // when TRUE clear char tables
    పూర్ణాంకం		si_region;	// region mask
    vimconv_T	si_conv;	// for conversion to 'encoding'
    పూర్ణాంకం		si_memtot;	// runtime memory used
    పూర్ణాంకం		si_verbose;	// verbose messages
    పూర్ణాంకం		si_సందేశం_లెక్క;	// number of words added since last message
    char_u	*si_info;	// info text chars or NULL
    పూర్ణాంకం		si_region_లెక్క; // number of regions supported (1 when there
				 // are no regions)
    char_u	si_region_name[MAXREGIONS * 2 + 1];
				// region names; used only if
				// si_region_count > 1)

    garray_T	si_rep;		// list of fromto_T entries from REP lines
    garray_T	si_repsal;	// list of fromto_T entries from REPSAL lines
    garray_T	si_sal;		// list of fromto_T entries from SAL lines
    char_u	*si_sofofr;	// SOFOFROM text
    char_u	*si_sofoto;	// SOFOTO text
    పూర్ణాంకం		si_nosugfile;	// NOSUGFILE item found
    పూర్ణాంకం		si_nosplitsugs;	// NOSPLITSUGS item found
    పూర్ణాంకం		si_nocompoundsugs; // NOCOMPOUNDSUGS item found
    పూర్ణాంకం		si_followup;	// soundsalike: ?
    పూర్ణాంకం		si_collapse;	// soundsalike: ?
    hashtab_T	si_commonwords;	// hashtable for common words
    time_t	si_sugtime;	// timestamp for .sug file
    పూర్ణాంకం		si_rem_accents;	// soundsalike: remove accents
    garray_T	si_map;		// MAP info concatenated
    char_u	*si_midword;	// MIDWORD chars or NULL
    పూర్ణాంకం		si_compmax;	// max nr of words for compounding
    పూర్ణాంకం		si_compminlen;	// minimal length for compounding
    పూర్ణాంకం		si_compsylmax;	// max nr of syllables for compounding
    పూర్ణాంకం		si_compoptions;	// COMP_ flags
    garray_T	si_comppat;	// CHECKCOMPOUNDPATTERN items, each stored as
				// a string
    char_u	*si_compflags;	// flags used for compounding
    char_u	si_nobreak;	// NOBREAK
    char_u	*si_syllable;	// syllable string
    garray_T	si_prefcond;	// table with conditions for postponed
				// prefixes, each stored as a string
    పూర్ణాంకం		si_newprefID;	// current value for ah_newID
    పూర్ణాంకం		si_newcompID;	// current value for compound ID
} spellinfo_T;

స్థిర పూర్ణాంకం is_aff_నియమం(char_u **వస్తువులు, పూర్ణాంకం itemcnt, అక్షరం *rulename, పూర్ణాంకం	 mincount);
స్థిర శూన్య aff_process_flags(afffile_T *affile, affentry_T *నమోదు_2);
స్థిర పూర్ణాంకం spell_info_అంశం(char_u *s);
స్థిర గుర్తులేని affitem2flag(పూర్ణాంకం flagtype, char_u *అంశం, char_u	*fname, పూర్ణాంకం lnum);
స్థిర గుర్తులేని get_affitem(పూర్ణాంకం flagtype, char_u **pp);
స్థిర శూన్య process_compflags(spellinfo_T *spin, afffile_T *aff, char_u *compflags);
స్థిర శూన్య check_renumber(spellinfo_T *spin);
స్థిర శూన్య aff_check_సంఖ్య(పూర్ణాంకం spinval, పూర్ణాంకం affval, అక్షరం *name);
స్థిర శూన్య aff_check_string(char_u *spinval, char_u *affval, అక్షరం *name);
స్థిర పూర్ణాంకం str_equal(char_u *s1, char_u	*s2);
స్థిర శూన్య కూడు_fromto(spellinfo_T *spin, garray_T *gap, char_u	*from, char_u *గమ్యం);
స్థిర పూర్ణాంకం sal_గమ్యం_bool(char_u *s);
స్థిర పూర్ణాంకం get_affix_flags(afffile_T *affile, char_u *afflist);
స్థిర పూర్ణాంకం get_pfxlist(afffile_T *affile, char_u *afflist, char_u *store_afflist, పూర్ణాంకం *cntp);
స్థిర పూర్ణాంకం get_compflags(afffile_T *affile, char_u *afflist, char_u *store_afflist, పూర్ణాంకం *cntp);
స్థిర పూర్ణాంకం store_aff_మాట(spellinfo_T *spin, char_u *మాట, char_u *afflist, afffile_T *affile, hashtab_T *ht, hashtab_T *xht, పూర్ణాంకం condit, పూర్ణాంకం flags, char_u *pfxlist, పూర్ణాంకం pfxlen);
స్థిర శూన్య *getroom(spellinfo_T *spin, size_t len, పూర్ణాంకం align);
స్థిర char_u *getroom_save(spellinfo_T *spin, char_u *s);
స్థిర పూర్ణాంకం store_మాట(spellinfo_T *spin, char_u *మాట, పూర్ణాంకం flags, పూర్ణాంకం region, char_u *pfxlist, పూర్ణాంకం need_affix);
స్థిర పూర్ణాంకం వృక్షం_కూడు_మాట(spellinfo_T *spin, char_u *మాట, wordnode_T *వృక్షం, పూర్ణాంకం flags, పూర్ణాంకం region, పూర్ణాంకం affixID);
స్థిర wordnode_T *get_wordnode(spellinfo_T *spin);
స్థిర శూన్య free_wordnode(spellinfo_T *spin, wordnode_T *n);
స్థిర శూన్య wordtree_compress(spellinfo_T *spin, wordnode_T *మూలం, అక్షరం *name);
స్థిర దీర్ఘ కణుపు_compress(spellinfo_T *spin, wordnode_T *కణుపు, hashtab_T *ht, దీర్ఘ *tot);
స్థిర పూర్ణాంకం కణుపు_equal(wordnode_T *n1, wordnode_T *n2);
స్థిర శూన్య clear_కణుపు(wordnode_T *కణుపు);
స్థిర పూర్ణాంకం put_కణుపు(FILE *fd, wordnode_T *కణుపు, పూర్ణాంకం idx, పూర్ణాంకం regionmask, పూర్ణాంకం prefixtree);
స్థిర పూర్ణాంకం sug_filltree(spellinfo_T *spin, slang_T *slang);
స్థిర పూర్ణాంకం sug_maketable(spellinfo_T *spin);
స్థిర పూర్ణాంకం sug_filltable(spellinfo_T *spin, wordnode_T *కణుపు, పూర్ణాంకం startwordnr, garray_T *gap);
స్థిర పూర్ణాంకం offset2bytes(పూర్ణాంకం nr, char_u *buf);
స్థిర శూన్య sug_write(spellinfo_T *spin, char_u *fname);
స్థిర శూన్య spell_సందేశం(spellinfo_T *spin, char_u *str);
స్థిర శూన్య init_spellfile(శూన్య);

// In the postponed prefixes tree wn_flags is used to store the WFP_ flags,
// but it must be negative to indicate the prefix tree to tree_add_word().
// Use a negative number with the lower 8 bits zero.
#నిర్వచించు PFX_FLAGS	(-256)

// flags for "condit" argument of store_aff_word()
#నిర్వచించు CONDIT_COMB	1	// affix must combine
#నిర్వచించు CONDIT_CFIX	2	// affix must have CIRCUMFIX flag
#నిర్వచించు CONDIT_SUF	4	// add a suffix for matching flags
#నిర్వచించు CONDIT_AFF	8	// word already has an affix

/*
 * Tunable parameters for when the tree is compressed.  Filled from the
 * 'mkspellmem' option.
 */
స్థిర దీర్ఘ compress_ప్రారంభం = 30000;	// memory / SBLOCKSIZE
స్థిర దీర్ఘ compress_inc = 100;		// memory / SBLOCKSIZE
స్థిర దీర్ఘ compress_added = 500000;	// word count

/*
 * Check the 'mkspellmem' option.  Return FAIL if it's wrong.
 * Sets "sps_flags".
 */
    పూర్ణాంకం
spell_check_msm(శూన్య)
{
    char_u	*p = p_msm;
    దీర్ఘ	ప్రారంభం = 0;
    దీర్ఘ	incr = 0;
    దీర్ఘ	added = 0;

    ఒకవేళ (!VIM_ISDIGIT(*p))
	తిరిగి FAIL;
    // block count = (value * 1024) / SBLOCKSIZE (but avoid overflow)
    ప్రారంభం = (getdigits(&p) * 10) / (SBLOCKSIZE / 102);
    ఒకవేళ (*p != ',')
	తిరిగి FAIL;
    ++p;
    ఒకవేళ (!VIM_ISDIGIT(*p))
	తిరిగి FAIL;
    incr = (getdigits(&p) * 102) / (SBLOCKSIZE / 10);
    ఒకవేళ (*p != ',')
	తిరిగి FAIL;
    ++p;
    ఒకవేళ (!VIM_ISDIGIT(*p))
	తిరిగి FAIL;
    added = getdigits(&p) * 1024;
    ఒకవేళ (*p != NUL)
	తిరిగి FAIL;

    ఒకవేళ (ప్రారంభం == 0 || incr == 0 || added == 0 || incr > ప్రారంభం)
	తిరిగి FAIL;

    compress_ప్రారంభం = ప్రారంభం;
    compress_inc = incr;
    compress_added = added;
    తిరిగి OK;
}

#నిర్వచితమైతే SPELL_PRINTTREE
/*
 * For debugging the tree code: print the current tree in a (more or less)
 * readable format, so that we can see what happens when adding a word and/or
 * compressing the tree.
 * Based on code from Olaf Seibert.
 */
# define PRINTLINESIZE	1000
# define PRINTWIDTH	6

# define PRINTSOME(l, లోతు, fmt, a1, a2) vim_snprintf(l + లోతు * PRINTWIDTH, \
	    PRINTLINESIZE - PRINTWIDTH * లోతు, fmt, a1, a2)

స్థిర అక్షరం line1[PRINTLINESIZE];
స్థిర అక్షరం line2[PRINTLINESIZE];
స్థిర అక్షరం line3[PRINTLINESIZE];

    స్థిర శూన్య
spell_clear_flags(wordnode_T *కణుపు)
{
    wordnode_T	*np;

    FOR_ALL_కణుపు_SIBLINGS(కణుపు, np)
    {
	np->wn_u1.సూచీ = FALSE;
	spell_clear_flags(np->wn_బిడ్డ);
    }
}

    స్థిర శూన్య
spell_print_కణుపు(wordnode_T *కణుపు, పూర్ణాంకం లోతు)
{
    ఒకవేళ (కణుపు->wn_u1.సూచీ)
    {
	// Done this node before, print the reference.
	PRINTSOME(line1, లోతు, "(%d)", కణుపు->wn_nr, 0);
	PRINTSOME(line2, లోతు, "    ", 0, 0);
	PRINTSOME(line3, లోతు, "    ", 0, 0);
	సందేశం(line1);
	సందేశం(line2);
	సందేశం(line3);
    }
    లేకపోతే
    {
	కణుపు->wn_u1.సూచీ = TRUE;

	ఒకవేళ (కణుపు->wn_byte != NUL)
	{
	    ఒకవేళ (కణుపు->wn_బిడ్డ != ఖాళీ)
		PRINTSOME(line1, లోతు, " %c -> ", కణుపు->wn_byte, 0);
	    లేకపోతే
		// Cannot happen?
		PRINTSOME(line1, లోతు, " %c ???", కణుపు->wn_byte, 0);
	}
	లేకపోతే
	    PRINTSOME(line1, లోతు, " $    ", 0, 0);

	PRINTSOME(line2, లోతు, "%d/%d    ", కణుపు->wn_nr, కణుపు->wn_refs);

	ఒకవేళ (కణుపు->wn_sibling != ఖాళీ)
	    PRINTSOME(line3, లోతు, " |    ", 0, 0);
	లేకపోతే
	    PRINTSOME(line3, లోతు, "      ", 0, 0);

	ఒకవేళ (కణుపు->wn_byte == NUL)
	{
	    సందేశం(line1);
	    సందేశం(line2);
	    సందేశం(line3);
	}

	// do the children
	ఒకవేళ (కణుపు->wn_byte != NUL && కణుపు->wn_బిడ్డ != ఖాళీ)
	    spell_print_కణుపు(కణుపు->wn_బిడ్డ, లోతు + 1);

	// do the siblings
	ఒకవేళ (కణుపు->wn_sibling != ఖాళీ)
	{
	    // get rid of all parent details except |
	    STRCPY(line1, line3);
	    STRCPY(line2, line3);
	    spell_print_కణుపు(కణుపు->wn_sibling, లోతు);
	}
    }
}

    స్థిర శూన్య
spell_print_వృక్షం(wordnode_T *మూలం)
{
    ఒకవేళ (మూలం == ఖాళీ)
	తిరిగి;

    // Clear the "wn_u1.index" fields, used to remember what has been done.
    spell_clear_flags(మూలం);

    // Recursively print the tree.
    spell_print_కణుపు(మూలం, 0);
}
#అయితే_ముగింపు // SPELL_PRINTTREE

/*
 * Read the affix file "fname".
 * Returns an afffile_T, NULL for complete failure.
 */
    స్థిర afffile_T *
spell_read_aff(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    afffile_T	*aff;
    char_u	rline[MAXLINELEN];
    char_u	*పంక్తి;
    char_u	*pc = ఖాళీ;
#నిర్వచించు MAXITEMCNT  30
    char_u	*(వస్తువులు[MAXITEMCNT]);
    పూర్ణాంకం		itemcnt;
    char_u	*p;
    పూర్ణాంకం		lnum = 0;
    affheader_T	*cur_aff = ఖాళీ;
    పూర్ణాంకం		did_postpone_ఉపసర్గ = FALSE;
    పూర్ణాంకం		aff_todo = 0;
    hashtab_T	*tp;
    char_u	*దిగువ = ఖాళీ;
    char_u	*fol = ఖాళీ;
    char_u	*upp = ఖాళీ;
    పూర్ణాంకం		do_rep;
    పూర్ణాంకం		do_repsal;
    పూర్ణాంకం		do_sal;
    పూర్ణాంకం		do_mapline;
    పూర్ణాంకం		దొరికింది_map = FALSE;
    hashitem_T	*hi;
    పూర్ణాంకం		l;
    పూర్ణాంకం		compminlen = 0;		// COMPOUNDMIN value
    పూర్ణాంకం		compsylmax = 0;		// COMPOUNDSYLMAX value
    పూర్ణాంకం		compoptions = 0;	// COMP_ flags
    పూర్ణాంకం		compmax = 0;		// COMPOUNDWORDMAX value
    char_u	*compflags = ఖాళీ;	// COMPOUNDFLAG and COMPOUNDRULE
					// concatenated
    char_u	*midword = ఖాళీ;	// MIDWORD value
    char_u	*syllable = ఖాళీ;	// SYLLABLE value
    char_u	*sofofrom = ఖాళీ;	// SOFOFROM value
    char_u	*sofoto = ఖాళీ;		// SOFOTO value

    /*
     * Open the file.
     */
    fd = mch_fopen((అక్షరం *)fname, "r");
    ఒకవేళ (fd == ఖాళీ)
    {
	semsg(_(e_cant_open_file_str), fname);
	తిరిగి ఖాళీ;
    }

    vim_snprintf((అక్షరం *)IObuff, IOSIZE, _("Reading affix file %s..."), fname);
    spell_సందేశం(spin, IObuff);

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
    aff = (afffile_T *)getroom(spin, పరిమాణం(afffile_T), TRUE);
    ఒకవేళ (aff == ఖాళీ)
    {
	fclose(fd);
	తిరిగి ఖాళీ;
    }
    hash_init(&aff->af_pref);
    hash_init(&aff->af_suff);
    hash_init(&aff->af_comp);

    /*
     * Read all the lines in the file one by one.
     */
    అయితే (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	పంక్తి_breakcheck();
	++lnum;

	// Skip comment lines.
	ఒకవేళ (*rline == '#')
	    కొనసాగు;

	// Convert from "SET" to 'encoding' when needed.
	vim_free(pc);
	ఒకవేళ (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, ఖాళీ);
	    ఒకవేళ (pc == ఖాళీ)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
							   fname, lnum, rline);
		కొనసాగు;
	    }
	    పంక్తి = pc;
	}
	లేకపోతే
	{
	    pc = ఖాళీ;
	    పంక్తి = rline;
	}

	// Split the line up in white separated items.  Put a NUL after each
	// item.
	itemcnt = 0;
	కొరకు (p = పంక్తి; ; )
	{
	    అయితే (*p != NUL && *p <= ' ')  // skip white space and CR/NL
		++p;
	    ఒకవేళ (*p == NUL)
		ఆపు;
	    ఒకవేళ (itemcnt == MAXITEMCNT)	    // too many items
		ఆపు;
	    వస్తువులు[itemcnt++] = p;
	    // A few items have arbitrary text argument, don't split them.
	    ఒకవేళ (itemcnt == 2 && spell_info_అంశం(వస్తువులు[0]))
		అయితే (*p >= ' ' || *p == TAB)    // skip until CR/NL
		    ++p;
	    లేకపోతే
		అయితే (*p > ' ')    // skip until white space or CR/NL
		    ++p;
	    ఒకవేళ (*p == NUL)
		ఆపు;
	    *p++ = NUL;
	}

	// Handle non-empty lines.
	ఒకవేళ (itemcnt > 0)
	{
	    ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "SET", 2) && aff->af_enc == ఖాళీ)
	    {
		// Setup for conversion from "ENC" to 'encoding'.
		aff->af_enc = enc_canonize(వస్తువులు[1]);
		ఒకవేళ (aff->af_enc != ఖాళీ && !spin->si_ascii
			&& convert_అమరిక(&spin->si_conv, aff->af_enc,
							       p_enc) == FAIL)
		    smsg(_("Conversion in %s not supported: from %s to %s"),
					       fname, aff->af_enc, p_enc);
		spin->si_conv.vc_fail = TRUE;
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "FLAG", 2)
					      && aff->af_flagtype == AFT_CHAR)
	    {
		ఒకవేళ (STRCMP(వస్తువులు[1], "long") == 0)
		    aff->af_flagtype = AFT_LONG;
		లేకపోతే ఒకవేళ (STRCMP(వస్తువులు[1], "num") == 0)
		    aff->af_flagtype = AFT_అంకె;
		లేకపోతే ఒకవేళ (STRCMP(వస్తువులు[1], "caplong") == 0)
		    aff->af_flagtype = AFT_CAPLONG;
		లేకపోతే
		    smsg(_("Invalid value for FLAG in %s line %d: %s"),
			    fname, lnum, వస్తువులు[1]);
		ఒకవేళ (aff->af_rare != 0
			|| aff->af_keepcase != 0
			|| aff->af_bad != 0
			|| aff->af_needaffix != 0
			|| aff->af_circumfix != 0
			|| aff->af_needcomp != 0
			|| aff->af_comproot != 0
			|| aff->af_nosuggest != 0
			|| compflags != ఖాళీ
			|| aff->af_suff.ht_వాడిన > 0
			|| aff->af_pref.ht_వాడిన > 0)
		    smsg(_("FLAG after using flags in %s line %d: %s"),
			    fname, lnum, వస్తువులు[1]);
	    }
	    లేకపోతే ఒకవేళ (spell_info_అంశం(వస్తువులు[0]))
	    {
		    p = (char_u *)getroom(spin,
			    (spin->si_info == ఖాళీ ? 0 : STRLEN(spin->si_info))
			    + STRLEN(వస్తువులు[0])
			    + STRLEN(వస్తువులు[1]) + 3, FALSE);
		    ఒకవేళ (p != ఖాళీ)
		    {
			ఒకవేళ (spin->si_info != ఖాళీ)
			{
			    STRCPY(p, spin->si_info);
			    STRCAT(p, "\n");
			}
			STRCAT(p, వస్తువులు[0]);
			STRCAT(p, " ");
			STRCAT(p, వస్తువులు[1]);
			spin->si_info = p;
		    }
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "MIDWORD", 2)
							   && midword == ఖాళీ)
	    {
		midword = getroom_save(spin, వస్తువులు[1]);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "TRY", 2))
	    {
		// ignored, we look in the tree for what chars may appear
	    }
	    // TODO: remove "RAR" later
	    లేకపోతే ఒకవేళ ((is_aff_నియమం(వస్తువులు, itemcnt, "RAR", 2)
			|| is_aff_నియమం(వస్తువులు, itemcnt, "RARE", 2))
							 && aff->af_rare == 0)
	    {
		aff->af_rare = affitem2flag(aff->af_flagtype, వస్తువులు[1],
								 fname, lnum);
	    }
	    // TODO: remove "KEP" later
	    లేకపోతే ఒకవేళ ((is_aff_నియమం(వస్తువులు, itemcnt, "KEP", 2)
			|| is_aff_నియమం(వస్తువులు, itemcnt, "KEEPCASE", 2))
						     && aff->af_keepcase == 0)
	    {
		aff->af_keepcase = affitem2flag(aff->af_flagtype, వస్తువులు[1],
								 fname, lnum);
	    }
	    లేకపోతే ఒకవేళ ((is_aff_నియమం(వస్తువులు, itemcnt, "BAD", 2)
			|| is_aff_నియమం(వస్తువులు, itemcnt, "FORBIDDENWORD", 2))
							  && aff->af_bad == 0)
	    {
		aff->af_bad = affitem2flag(aff->af_flagtype, వస్తువులు[1],
								 fname, lnum);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "NEEDAFFIX", 2)
						    && aff->af_needaffix == 0)
	    {
		aff->af_needaffix = affitem2flag(aff->af_flagtype, వస్తువులు[1],
								 fname, lnum);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "CIRCUMFIX", 2)
						    && aff->af_circumfix == 0)
	    {
		aff->af_circumfix = affitem2flag(aff->af_flagtype, వస్తువులు[1],
								 fname, lnum);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "NOSUGGEST", 2)
						    && aff->af_nosuggest == 0)
	    {
		aff->af_nosuggest = affitem2flag(aff->af_flagtype, వస్తువులు[1],
								 fname, lnum);
	    }
	    లేకపోతే ఒకవేళ ((is_aff_నియమం(వస్తువులు, itemcnt, "NEEDCOMPOUND", 2)
			|| is_aff_నియమం(వస్తువులు, itemcnt, "ONLYINCOMPOUND", 2))
						     && aff->af_needcomp == 0)
	    {
		aff->af_needcomp = affitem2flag(aff->af_flagtype, వస్తువులు[1],
								 fname, lnum);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "COMPOUNDROOT", 2)
						     && aff->af_comproot == 0)
	    {
		aff->af_comproot = affitem2flag(aff->af_flagtype, వస్తువులు[1],
								 fname, lnum);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "COMPOUNDFORBIDFLAG", 2)
						   && aff->af_compforbid == 0)
	    {
		aff->af_compforbid = affitem2flag(aff->af_flagtype, వస్తువులు[1],
								 fname, lnum);
		ఒకవేళ (aff->af_pref.ht_వాడిన > 0)
		    smsg(_("Defining COMPOUNDFORBIDFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "COMPOUNDPERMITFLAG", 2)
						   && aff->af_comppermit == 0)
	    {
		aff->af_comppermit = affitem2flag(aff->af_flagtype, వస్తువులు[1],
								 fname, lnum);
		ఒకవేళ (aff->af_pref.ht_వాడిన > 0)
		    smsg(_("Defining COMPOUNDPERMITFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "COMPOUNDFLAG", 2)
							 && compflags == ఖాళీ)
	    {
		// Turn flag "c" into COMPOUNDRULE compatible string "c+",
		// "Na" into "Na+", "1234" into "1234+".
		p = getroom(spin, STRLEN(వస్తువులు[1]) + 2, FALSE);
		ఒకవేళ (p != ఖాళీ)
		{
		    STRCPY(p, వస్తువులు[1]);
		    STRCAT(p, "+");
		    compflags = p;
		}
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "COMPOUNDRULES", 2))
	    {
		// We don't use the count, but do check that it's a number and
		// not COMPOUNDRULE mistyped.
		ఒకవేళ (atoi((అక్షరం *)వస్తువులు[1]) == 0)
		    smsg(_("Wrong COMPOUNDRULES value in %s line %d: %s"),
						       fname, lnum, వస్తువులు[1]);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "COMPOUNDRULE", 2))
	    {
		// Don't use the first rule if it is a number.
		ఒకవేళ (compflags != ఖాళీ || *skipdigits(వస్తువులు[1]) != NUL)
		{
		    // Concatenate this string to previously defined ones,
		    // using a slash to separate them.
		    l = (పూర్ణాంకం)STRLEN(వస్తువులు[1]) + 1;
		    ఒకవేళ (compflags != ఖాళీ)
			l += (పూర్ణాంకం)STRLEN(compflags) + 1;
		    p = getroom(spin, l, FALSE);
		    ఒకవేళ (p != ఖాళీ)
		    {
			ఒకవేళ (compflags != ఖాళీ)
			{
			    STRCPY(p, compflags);
			    STRCAT(p, "/");
			}
			STRCAT(p, వస్తువులు[1]);
			compflags = p;
		    }
		}
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "COMPOUNDWORDMAX", 2)
							      && compmax == 0)
	    {
		compmax = atoi((అక్షరం *)వస్తువులు[1]);
		ఒకవేళ (compmax == 0)
		    smsg(_("Wrong COMPOUNDWORDMAX value in %s line %d: %s"),
						       fname, lnum, వస్తువులు[1]);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "COMPOUNDMIN", 2)
							   && compminlen == 0)
	    {
		compminlen = atoi((అక్షరం *)వస్తువులు[1]);
		ఒకవేళ (compminlen == 0)
		    smsg(_("Wrong COMPOUNDMIN value in %s line %d: %s"),
						       fname, lnum, వస్తువులు[1]);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "COMPOUNDSYLMAX", 2)
							   && compsylmax == 0)
	    {
		compsylmax = atoi((అక్షరం *)వస్తువులు[1]);
		ఒకవేళ (compsylmax == 0)
		    smsg(_("Wrong COMPOUNDSYLMAX value in %s line %d: %s"),
						       fname, lnum, వస్తువులు[1]);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "CHECKCOMPOUNDDUP", 1))
	    {
		compoptions |= COMP_CHECKDUP;
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "CHECKCOMPOUNDREP", 1))
	    {
		compoptions |= COMP_CHECKREP;
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "CHECKCOMPOUNDCASE", 1))
	    {
		compoptions |= COMP_CHECKCASE;
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "CHECKCOMPOUNDTRIPLE", 1))
	    {
		compoptions |= COMP_CHECKTRIPLE;
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "CHECKCOMPOUNDPATTERN", 2))
	    {
		ఒకవేళ (atoi((అక్షరం *)వస్తువులు[1]) == 0)
		    smsg(_("Wrong CHECKCOMPOUNDPATTERN value in %s line %d: %s"),
						       fname, lnum, వస్తువులు[1]);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "CHECKCOMPOUNDPATTERN", 3))
	    {
		garray_T    *gap = &spin->si_comppat;
		పూర్ణాంకం	    i;

		// Only add the couple if it isn't already there.
		కొరకు (i = 0; i < gap->ga_len - 1; i += 2)
		    ఒకవేళ (STRCMP(((char_u **)(gap->ga_దత్తాంశం))[i], వస్తువులు[1]) == 0
			    && STRCMP(((char_u **)(gap->ga_దత్తాంశం))[i + 1],
							       వస్తువులు[2]) == 0)
			ఆపు;
		ఒకవేళ (i >= gap->ga_len && ga_grow(gap, 2) == OK)
		{
		    ((char_u **)(gap->ga_దత్తాంశం))[gap->ga_len++]
					       = getroom_save(spin, వస్తువులు[1]);
		    ((char_u **)(gap->ga_దత్తాంశం))[gap->ga_len++]
					       = getroom_save(spin, వస్తువులు[2]);
		}
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "SYLLABLE", 2)
							  && syllable == ఖాళీ)
	    {
		syllable = getroom_save(spin, వస్తువులు[1]);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "NOBREAK", 1))
	    {
		spin->si_nobreak = TRUE;
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "NOSPLITSUGS", 1))
	    {
		spin->si_nosplitsugs = TRUE;
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "NOCOMPOUNDSUGS", 1))
	    {
		spin->si_nocompoundsugs = TRUE;
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "NOSUGFILE", 1))
	    {
		spin->si_nosugfile = TRUE;
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "PFXPOSTPONE", 1))
	    {
		aff->af_pfxpostpone = TRUE;
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "IGNOREEXTRA", 1))
	    {
		aff->af_ignoreextra = TRUE;
	    }
	    లేకపోతే ఒకవేళ ((STRCMP(వస్తువులు[0], "PFX") == 0
					      || STRCMP(వస్తువులు[0], "SFX") == 0)
		    && aff_todo == 0
		    && itemcnt >= 4)
	    {
		పూర్ణాంకం	lasti = 4;
		char_u	కీలకం[AH_కీలకం_LEN];

		ఒకవేళ (*వస్తువులు[0] == 'P')
		    tp = &aff->af_pref;
		లేకపోతే
		    tp = &aff->af_suff;

		// Myspell allows the same affix name to be used multiple
		// times.  The affix files that do this have an undocumented
		// "S" flag on all but the last block, thus we check for that
		// and store it in ah_follows.
		vim_strncpy(కీలకం, వస్తువులు[1], AH_కీలకం_LEN - 1);
		hi = hash_కనుగొను(tp, కీలకం);
		ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		{
		    cur_aff = HI2AH(hi);
		    ఒకవేళ (cur_aff->ah_combine != (*వస్తువులు[2] == 'Y'))
			smsg(_("Different combining flag in continued affix block in %s line %d: %s"),
						   fname, lnum, వస్తువులు[1]);
		    ఒకవేళ (!cur_aff->ah_follows)
			smsg(_("Duplicate affix in %s line %d: %s"),
						       fname, lnum, వస్తువులు[1]);
		}
		లేకపోతే
		{
		    // New affix letter.
		    cur_aff = (affheader_T *)getroom(spin,
						   పరిమాణం(affheader_T), TRUE);
		    ఒకవేళ (cur_aff == ఖాళీ)
			ఆపు;
		    cur_aff->ah_flag = affitem2flag(aff->af_flagtype, వస్తువులు[1],
								 fname, lnum);
		    ఒకవేళ (cur_aff->ah_flag == 0 || STRLEN(వస్తువులు[1]) >= AH_కీలకం_LEN)
			ఆపు;
		    ఒకవేళ (cur_aff->ah_flag == aff->af_bad
			    || cur_aff->ah_flag == aff->af_rare
			    || cur_aff->ah_flag == aff->af_keepcase
			    || cur_aff->ah_flag == aff->af_needaffix
			    || cur_aff->ah_flag == aff->af_circumfix
			    || cur_aff->ah_flag == aff->af_nosuggest
			    || cur_aff->ah_flag == aff->af_needcomp
			    || cur_aff->ah_flag == aff->af_comproot)
			smsg(_("Affix also used for BAD/RARE/KEEPCASE/NEEDAFFIX/NEEDCOMPOUND/NOSUGGEST in %s line %d: %s"),
						       fname, lnum, వస్తువులు[1]);
		    STRCPY(cur_aff->ah_కీలకం, వస్తువులు[1]);
		    hash_కూడు(tp, cur_aff->ah_కీలకం, "spelling");

		    cur_aff->ah_combine = (*వస్తువులు[2] == 'Y');
		}

		// Check for the "S" flag, which apparently means that another
		// block with the same affix name is following.
		ఒకవేళ (itemcnt > lasti && STRCMP(వస్తువులు[lasti], "S") == 0)
		{
		    ++lasti;
		    cur_aff->ah_follows = TRUE;
		}
		లేకపోతే
		    cur_aff->ah_follows = FALSE;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter.
		ఒకవేళ (itemcnt > lasti && *వస్తువులు[lasti] != '#')
		    smsg(_(e_afftrailing), fname, lnum, వస్తువులు[lasti]);

		ఒకవేళ (STRCMP(వస్తువులు[2], "Y") != 0 && STRCMP(వస్తువులు[2], "N") != 0)
		    smsg(_("Expected Y or N in %s line %d: %s"),
						       fname, lnum, వస్తువులు[2]);

		ఒకవేళ (*వస్తువులు[0] == 'P' && aff->af_pfxpostpone)
		{
		    ఒకవేళ (cur_aff->ah_newID == 0)
		    {
			// Use a new number in the .spl file later, to be able
			// to handle multiple .aff files.
			check_renumber(spin);
			cur_aff->ah_newID = ++spin->si_newprefID;

			// We only really use ah_newID if the prefix is
			// postponed.  We know that only after handling all
			// the items.
			did_postpone_ఉపసర్గ = FALSE;
		    }
		    లేకపోతే
			// Did use the ID in a previous block.
			did_postpone_ఉపసర్గ = TRUE;
		}

		aff_todo = atoi((అక్షరం *)వస్తువులు[3]);
	    }
	    లేకపోతే ఒకవేళ ((STRCMP(వస్తువులు[0], "PFX") == 0
					      || STRCMP(వస్తువులు[0], "SFX") == 0)
		    && aff_todo > 0
		    && STRCMP(cur_aff->ah_కీలకం, వస్తువులు[1]) == 0
		    && itemcnt >= 5)
	    {
		affentry_T	*aff_నమోదు;
		పూర్ణాంకం		పై_2 = FALSE;
		పూర్ణాంకం		lasti = 5;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter,
		// unless IGNOREEXTRA is used.  Hunspell uses a "-" item.
		ఒకవేళ (itemcnt > lasti
			&& !aff->af_ignoreextra
			&& *వస్తువులు[lasti] != '#'
			&& (STRCMP(వస్తువులు[lasti], "-") != 0
						     || itemcnt != lasti + 1))
		    smsg(_(e_afftrailing), fname, lnum, వస్తువులు[lasti]);

		// New item for an affix letter.
		--aff_todo;
		aff_నమోదు = (affentry_T *)getroom(spin,
						    పరిమాణం(affentry_T), TRUE);
		ఒకవేళ (aff_నమోదు == ఖాళీ)
		    ఆపు;

		ఒకవేళ (STRCMP(వస్తువులు[2], "0") != 0)
		    aff_నమోదు->ae_chop = getroom_save(spin, వస్తువులు[2]);
		ఒకవేళ (STRCMP(వస్తువులు[3], "0") != 0)
		{
		    aff_నమోదు->ae_కూడు = getroom_save(spin, వస్తువులు[3]);

		    // Recognize flags on the affix: abcd/XYZ
		    aff_నమోదు->ae_flags = vim_strchr(aff_నమోదు->ae_కూడు, '/');
		    ఒకవేళ (aff_నమోదు->ae_flags != ఖాళీ)
		    {
			*aff_నమోదు->ae_flags++ = NUL;
			aff_process_flags(aff, aff_నమోదు);
		    }
		}

		// Don't use an affix entry with non-ASCII characters when
		// "spin->si_ascii" is TRUE.
		ఒకవేళ (!spin->si_ascii || !(has_non_ascii(aff_నమోదు->ae_chop)
					  || has_non_ascii(aff_నమోదు->ae_కూడు)))
		{
		    aff_నమోదు->ae_తదుపరి = cur_aff->ah_మొదటి;
		    cur_aff->ah_మొదటి = aff_నమోదు;

		    ఒకవేళ (STRCMP(వస్తువులు[4], ".") != 0)
		    {
			char_u	buf[MAXLINELEN];

			aff_నమోదు->ae_cond = getroom_save(spin, వస్తువులు[4]);
			// Note: this silently truncates the buffer, but this should
			// not happen in practice
			ఒకవేళ (*వస్తువులు[0] == 'P')
			    vim_snprintf((అక్షరం *)buf, పరిమాణం(buf), "^%s", వస్తువులు[4]);
			లేకపోతే
			    vim_snprintf((అక్షరం *)buf, పరిమాణం(buf), "%s$", వస్తువులు[4]);
			aff_నమోదు->ae_prog = vim_regcomp(buf,
					    RE_MAGIC + RE_STRING + RE_STRICT);
			ఒకవేళ (aff_నమోదు->ae_prog == ఖాళీ)
			    smsg(_("Broken condition in %s line %d: %s"),
						       fname, lnum, వస్తువులు[4]);
		    }

		    // For postponed prefixes we need an entry in si_prefcond
		    // for the condition.  Use an existing one if possible.
		    // Can't be done for an affix with flags, ignoring
		    // COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG.
		    ఒకవేళ (*వస్తువులు[0] == 'P' && aff->af_pfxpostpone
					       && aff_నమోదు->ae_flags == ఖాళీ)
		    {
			// When the chop string is one lower-case letter and
			// the add string ends in the upper-case letter we set
			// the "upper" flag, clear "ae_chop" and remove the
			// letters from "ae_add".  The condition must either
			// be empty or start with the same letter.
			ఒకవేళ (aff_నమోదు->ae_chop != ఖాళీ
				&& aff_నమోదు->ae_కూడు != ఖాళీ
				&& aff_నమోదు->ae_chop[(*mb_ptr2len)(
						   aff_నమోదు->ae_chop)] == NUL)
			{
			    పూర్ణాంకం		c, c_పైకి;

			    c = PTR2CHAR(aff_నమోదు->ae_chop);
			    c_పైకి = SPELL_TOUPPER(c);
			    ఒకవేళ (c_పైకి != c
				    && (aff_నమోదు->ae_cond == ఖాళీ
					|| PTR2CHAR(aff_నమోదు->ae_cond) == c))
			    {
				p = aff_నమోదు->ae_కూడు
						  + STRLEN(aff_నమోదు->ae_కూడు);
				MB_PTR_వెనుక(aff_నమోదు->ae_కూడు, p);
				ఒకవేళ (PTR2CHAR(p) == c_పైకి)
				{
				    పై_2 = TRUE;
				    aff_నమోదు->ae_chop = ఖాళీ;
				    *p = NUL;

				    // The condition is matched with the
				    // actual word, thus must check for the
				    // upper-case letter.
				    ఒకవేళ (aff_నమోదు->ae_cond != ఖాళీ)
				    {
					char_u	buf[MAXLINELEN];

					ఒకవేళ (has_mbyte)
					{
					    onecap_copy(వస్తువులు[4], buf, TRUE);
					    aff_నమోదు->ae_cond = getroom_save(
								   spin, buf);
					}
					లేకపోతే
					    *aff_నమోదు->ae_cond = c_పైకి;
					ఒకవేళ (aff_నమోదు->ae_cond != ఖాళీ)
					{
					    sprintf((అక్షరం *)buf, "^%s",
							  aff_నమోదు->ae_cond);
					    vim_regfree(aff_నమోదు->ae_prog);
					    aff_నమోదు->ae_prog = vim_regcomp(
						    buf, RE_MAGIC + RE_STRING);
					}
				    }
				}
			    }
			}

			ఒకవేళ (aff_నమోదు->ae_chop == ఖాళీ
					       && aff_నమోదు->ae_flags == ఖాళీ)
			{
			    పూర్ణాంకం		idx;
			    char_u	**pp;
			    పూర్ణాంకం		n;

			    // Find a previously used condition.
			    కొరకు (idx = spin->si_prefcond.ga_len - 1; idx >= 0;
									--idx)
			    {
				p = ((char_u **)spin->si_prefcond.ga_దత్తాంశం)[idx];
				ఒకవేళ (str_equal(p, aff_నమోదు->ae_cond))
				    ఆపు;
			    }
			    ఒకవేళ (idx < 0 && ga_grow(&spin->si_prefcond, 1) == OK)
			    {
				// Not found, add a new condition.
				idx = spin->si_prefcond.ga_len++;
				pp = ((char_u **)spin->si_prefcond.ga_దత్తాంశం)
									+ idx;
				ఒకవేళ (aff_నమోదు->ae_cond == ఖాళీ)
				    *pp = ఖాళీ;
				లేకపోతే
				    *pp = getroom_save(spin,
							  aff_నమోదు->ae_cond);
			    }

			    // Add the prefix to the prefix tree.
			    ఒకవేళ (aff_నమోదు->ae_కూడు == ఖాళీ)
				p = (char_u *)"";
			    లేకపోతే
				p = aff_నమోదు->ae_కూడు;

			    // PFX_FLAGS is a negative number, so that
			    // tree_add_word() knows this is the prefix tree.
			    n = PFX_FLAGS;
			    ఒకవేళ (!cur_aff->ah_combine)
				n |= WFP_NC;
			    ఒకవేళ (పై_2)
				n |= WFP_పైకి;
			    ఒకవేళ (aff_నమోదు->ae_comppermit)
				n |= WFP_COMPPERMIT;
			    ఒకవేళ (aff_నమోదు->ae_compforbid)
				n |= WFP_COMPFORBID;
			    వృక్షం_కూడు_మాట(spin, p, spin->si_prefroot, n,
						      idx, cur_aff->ah_newID);
			    did_postpone_ఉపసర్గ = TRUE;
			}

			// Didn't actually use ah_newID, backup si_newprefID.
			ఒకవేళ (aff_todo == 0 && !did_postpone_ఉపసర్గ)
			{
			    --spin->si_newprefID;
			    cur_aff->ah_newID = 0;
			}
		    }
		}
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "FOL", 2) && fol == ఖాళీ)
	    {
		fol = vim_strsave(వస్తువులు[1]);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "LOW", 2) && దిగువ == ఖాళీ)
	    {
		దిగువ = vim_strsave(వస్తువులు[1]);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "UPP", 2) && upp == ఖాళీ)
	    {
		upp = vim_strsave(వస్తువులు[1]);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "REP", 2)
		     || is_aff_నియమం(వస్తువులు, itemcnt, "REPSAL", 2))
	    {
		// Ignore REP/REPSAL count
		ఒకవేళ (!సురక్షిత_isdigit(*వస్తువులు[1]))
		    smsg(_("Expected REP(SAL) count in %s line %d"),
								 fname, lnum);
	    }
	    లేకపోతే ఒకవేళ ((STRCMP(వస్తువులు[0], "REP") == 0
			|| STRCMP(వస్తువులు[0], "REPSAL") == 0)
		    && itemcnt >= 3)
	    {
		// REP/REPSAL item
		// Myspell ignores extra arguments, we require it starts with
		// # to detect mistakes.
		ఒకవేళ (itemcnt > 3 && వస్తువులు[3][0] != '#')
		    smsg(_(e_afftrailing), fname, lnum, వస్తువులు[3]);
		ఒకవేళ (వస్తువులు[0][3] == 'S' ? do_repsal : do_rep)
		{
		    // Replace underscore with space (can't include a space
		    // directly).
		    కొరకు (p = వస్తువులు[1]; *p != NUL; MB_PTR_ADV(p))
			ఒకవేళ (*p == '_')
			    *p = ' ';
		    కొరకు (p = వస్తువులు[2]; *p != NUL; MB_PTR_ADV(p))
			ఒకవేళ (*p == '_')
			    *p = ' ';
		    కూడు_fromto(spin, వస్తువులు[0][3] == 'S'
					 ? &spin->si_repsal
					 : &spin->si_rep, వస్తువులు[1], వస్తువులు[2]);
		}
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "MAP", 2))
	    {
		// MAP item or count
		ఒకవేళ (!దొరికింది_map)
		{
		    // First line contains the count.
		    దొరికింది_map = TRUE;
		    ఒకవేళ (!సురక్షిత_isdigit(*వస్తువులు[1]))
			smsg(_("Expected MAP count in %s line %d"),
								 fname, lnum);
		}
		లేకపోతే ఒకవేళ (do_mapline)
		{
		    పూర్ణాంకం		c;

		    // Check that every character appears only once.
		    కొరకు (p = వస్తువులు[1]; *p != NUL; )
		    {
			c = mb_ptr2char_adv(&p);
			ఒకవేళ ((spin->si_map.ga_len > 0
				    && vim_strchr(spin->si_map.ga_దత్తాంశం, c)
								      != ఖాళీ)
				|| vim_strchr(p, c) != ఖాళీ)
			    smsg(_("Duplicate character in MAP in %s line %d"),
								 fname, lnum);
		    }

		    // We simply concatenate all the MAP strings, separated by
		    // slashes.
		    ga_concat(&spin->si_map, వస్తువులు[1]);
		    ga_append(&spin->si_map, '/');
		}
	    }
	    // Accept "SAL from to" and "SAL from to  #comment".
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "SAL", 3))
	    {
		ఒకవేళ (do_sal)
		{
		    // SAL item (sounds-a-like)
		    // Either one of the known keys or a from-to pair.
		    ఒకవేళ (STRCMP(వస్తువులు[1], "followup") == 0)
			spin->si_followup = sal_గమ్యం_bool(వస్తువులు[2]);
		    లేకపోతే ఒకవేళ (STRCMP(వస్తువులు[1], "collapse_result") == 0)
			spin->si_collapse = sal_గమ్యం_bool(వస్తువులు[2]);
		    లేకపోతే ఒకవేళ (STRCMP(వస్తువులు[1], "remove_accents") == 0)
			spin->si_rem_accents = sal_గమ్యం_bool(వస్తువులు[2]);
		    లేకపోతే
			// when "to" is "_" it means empty
			కూడు_fromto(spin, &spin->si_sal, వస్తువులు[1],
				     STRCMP(వస్తువులు[2], "_") == 0 ? (char_u *)""
								: వస్తువులు[2]);
		}
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "SOFOFROM", 2)
							  && sofofrom == ఖాళీ)
	    {
		sofofrom = getroom_save(spin, వస్తువులు[1]);
	    }
	    లేకపోతే ఒకవేళ (is_aff_నియమం(వస్తువులు, itemcnt, "SOFOTO", 2)
							    && sofoto == ఖాళీ)
	    {
		sofoto = getroom_save(spin, వస్తువులు[1]);
	    }
	    లేకపోతే ఒకవేళ (STRCMP(వస్తువులు[0], "COMMON") == 0)
	    {
		పూర్ణాంకం	i;

		కొరకు (i = 1; i < itemcnt; ++i)
		{
		    ఒకవేళ (HASHITEM_ఖాళీలు(hash_కనుగొను(&spin->si_commonwords,
								   వస్తువులు[i])))
		    {
			p = vim_strsave(వస్తువులు[i]);
			ఒకవేళ (p == ఖాళీ)
			    ఆపు;
			hash_కూడు(&spin->si_commonwords, p, "spelling");
		    }
		}
	    }
	    లేకపోతే
		smsg(_("Unrecognized or duplicate item in %s line %d: %s"),
						       fname, lnum, వస్తువులు[0]);
	}
    }

    ఒకవేళ (fol != ఖాళీ || దిగువ != ఖాళీ || upp != ఖాళీ)
    {
	ఒకవేళ (spin->si_clear_chartab)
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
	ఒకవేళ (!spin->si_ascii && !enc_utf8)
	{
	    ఒకవేళ (fol == ఖాళీ || దిగువ == ఖాళీ || upp == ఖాళీ)
		smsg(_("Missing FOL/LOW/UPP line in %s"), fname);
	    లేకపోతే
		(శూన్య)సమితి_spell_chartab(fol, దిగువ, upp);
	}

	vim_free(fol);
	vim_free(దిగువ);
	vim_free(upp);
    }

    // Use compound specifications of the .aff file for the spell info.
    ఒకవేళ (compmax != 0)
    {
	aff_check_సంఖ్య(spin->si_compmax, compmax, "COMPOUNDWORDMAX");
	spin->si_compmax = compmax;
    }

    ఒకవేళ (compminlen != 0)
    {
	aff_check_సంఖ్య(spin->si_compminlen, compminlen, "COMPOUNDMIN");
	spin->si_compminlen = compminlen;
    }

    ఒకవేళ (compsylmax != 0)
    {
	ఒకవేళ (syllable == ఖాళీ)
	    smsg(_("COMPOUNDSYLMAX used without SYLLABLE"));
	aff_check_సంఖ్య(spin->si_compsylmax, compsylmax, "COMPOUNDSYLMAX");
	spin->si_compsylmax = compsylmax;
    }

    ఒకవేళ (compoptions != 0)
    {
	aff_check_సంఖ్య(spin->si_compoptions, compoptions, "COMPOUND options");
	spin->si_compoptions |= compoptions;
    }

    ఒకవేళ (compflags != ఖాళీ)
	process_compflags(spin, aff, compflags);

    // Check that we didn't use too many renumbered flags.
    ఒకవేళ (spin->si_newcompID < spin->si_newprefID)
    {
	ఒకవేళ (spin->si_newcompID == 127 || spin->si_newcompID == 255)
	    సందేశం(_("Too many postponed prefixes"));
	లేకపోతే ఒకవేళ (spin->si_newprefID == 0 || spin->si_newprefID == 127)
	    సందేశం(_("Too many compound flags"));
	లేకపోతే
	    సందేశం(_("Too many postponed prefixes and/or compound flags"));
    }

    ఒకవేళ (syllable != ఖాళీ)
    {
	aff_check_string(spin->si_syllable, syllable, "SYLLABLE");
	spin->si_syllable = syllable;
    }

    ఒకవేళ (sofofrom != ఖాళీ || sofoto != ఖాళీ)
    {
	ఒకవేళ (sofofrom == ఖాళీ || sofoto == ఖాళీ)
	    smsg(_("Missing SOFO%s line in %s"),
				     sofofrom == ఖాళీ ? "FROM" : "TO", fname);
	లేకపోతే ఒకవేళ (spin->si_sal.ga_len > 0)
	    smsg(_("Both SAL and SOFO lines in %s"), fname);
	లేకపోతే
	{
	    aff_check_string(spin->si_sofofr, sofofrom, "SOFOFROM");
	    aff_check_string(spin->si_sofoto, sofoto, "SOFOTO");
	    spin->si_sofofr = sofofrom;
	    spin->si_sofoto = sofoto;
	}
    }

    ఒకవేళ (midword != ఖాళీ)
    {
	aff_check_string(spin->si_midword, midword, "MIDWORD");
	spin->si_midword = midword;
    }

    vim_free(pc);
    fclose(fd);
    తిరిగి aff;
}

/*
 * Return TRUE when items[0] equals "rulename", there are "mincount" items or
 * a comment is following after item "mincount".
 */
    స్థిర పూర్ణాంకం
is_aff_నియమం(
    char_u	**వస్తువులు,
    పూర్ణాంకం		itemcnt,
    అక్షరం	*rulename,
    పూర్ణాంకం		mincount)
{
    తిరిగి (STRCMP(వస్తువులు[0], rulename) == 0
	    && (itemcnt == mincount
		|| (itemcnt > mincount && వస్తువులు[mincount][0] == '#')));
}

/*
 * For affix "entry" move COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG from
 * ae_flags to ae_comppermit and ae_compforbid.
 */
    స్థిర శూన్య
aff_process_flags(afffile_T *affile, affentry_T *నమోదు_2)
{
    char_u	*p;
    char_u	*prevp;
    గుర్తులేని	flag;

    ఒకవేళ (నమోదు_2->ae_flags != ఖాళీ
		&& (affile->af_compforbid != 0 || affile->af_comppermit != 0))
    {
	కొరకు (p = నమోదు_2->ae_flags; *p != NUL; )
	{
	    prevp = p;
	    flag = get_affitem(affile->af_flagtype, &p);
	    ఒకవేళ (flag == affile->af_comppermit || flag == affile->af_compforbid)
	    {
		STRMOVE(prevp, p);
		p = prevp;
		ఒకవేళ (flag == affile->af_comppermit)
		    నమోదు_2->ae_comppermit = TRUE;
		లేకపోతే
		    నమోదు_2->ae_compforbid = TRUE;
	    }
	    ఒకవేళ (affile->af_flagtype == AFT_అంకె && *p == ',')
		++p;
	}
	ఒకవేళ (*నమోదు_2->ae_flags == NUL)
	    నమోదు_2->ae_flags = ఖాళీ;	// nothing left
    }
}

/*
 * Return TRUE if "s" is the name of an info item in the affix file.
 */
    స్థిర పూర్ణాంకం
spell_info_అంశం(char_u *s)
{
    తిరిగి STRCMP(s, "NAME") == 0
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
    స్థిర గుర్తులేని
affitem2flag(
    పూర్ణాంకం		flagtype,
    char_u	*అంశం,
    char_u	*fname,
    పూర్ణాంకం		lnum)
{
    గుర్తులేని	res;
    char_u	*p = అంశం;

    res = get_affitem(flagtype, &p);
    ఒకవేళ (res == 0)
    {
	ఒకవేళ (flagtype == AFT_అంకె)
	    smsg(_("Flag is not a number in %s line %d: %s"),
							   fname, lnum, అంశం);
	లేకపోతే
	    smsg(_("Illegal flag in %s line %d: %s"),
							   fname, lnum, అంశం);
    }
    ఒకవేళ (*p != NUL)
    {
	smsg(_(e_affname), fname, lnum, అంశం);
	తిరిగి 0;
    }

    తిరిగి res;
}

/*
 * Get one affix name from "*pp" and advance the pointer.
 * Returns ZERO_FLAG for "0".
 * Returns zero for an error, still advances the pointer then.
 */
    స్థిర గుర్తులేని
get_affitem(పూర్ణాంకం flagtype, char_u **pp)
{
    పూర్ణాంకం		res;

    ఒకవేళ (flagtype == AFT_అంకె)
    {
	ఒకవేళ (!VIM_ISDIGIT(**pp))
	{
	    ++*pp;	// always advance, avoid getting stuck
	    తిరిగి 0;
	}
	res = getdigits(pp);
	ఒకవేళ (res == 0)
	    res = ZERO_FLAG;
    }
    లేకపోతే
    {
	res = mb_ptr2char_adv(pp);
	ఒకవేళ (flagtype == AFT_LONG || (flagtype == AFT_CAPLONG
						 && res >= 'A' && res <= 'Z'))
	{
	    ఒకవేళ (**pp == NUL)
		తిరిగి 0;
	    res = mb_ptr2char_adv(pp) + (res << 16);
	}
    }
    తిరిగి res;
}

/*
 * Process the "compflags" string used in an affix file and append it to
 * spin->si_compflags.
 * The processing involves changing the affix names to ID numbers, so that
 * they fit in one byte.
 */
    స్థిర శూన్య
process_compflags(
    spellinfo_T	*spin,
    afffile_T	*aff,
    char_u	*compflags)
{
    char_u	*p;
    char_u	*prevp;
    గుర్తులేని	flag;
    compitem_T	*ci;
    పూర్ణాంకం		id;
    పూర్ణాంకం		len;
    char_u	*tp;
    char_u	కీలకం[AH_కీలకం_LEN];
    hashitem_T	*hi;

    // Make room for the old and the new compflags, concatenated with a / in
    // between.  Processing it makes it shorter, but we don't know by how
    // much, thus allocate the maximum.
    len = (పూర్ణాంకం)STRLEN(compflags) + 1;
    ఒకవేళ (spin->si_compflags != ఖాళీ)
	len += (పూర్ణాంకం)STRLEN(spin->si_compflags) + 1;
    p = getroom(spin, len, FALSE);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;
    ఒకవేళ (spin->si_compflags != ఖాళీ)
    {
	STRCPY(p, spin->si_compflags);
	STRCAT(p, "/");
    }
    spin->si_compflags = p;
    tp = p + STRLEN(p);

    కొరకు (p = compflags; *p != NUL; )
    {
	ఒకవేళ (vim_strchr((char_u *)"/?*+[]", *p) != ఖాళీ)
	    // Copy non-flag characters directly.
	    *tp++ = *p++;
	లేకపోతే
	{
	    // First get the flag number, also checks validity.
	    prevp = p;
	    flag = get_affitem(aff->af_flagtype, &p);
	    ఒకవేళ (flag != 0)
	    {
		// Find the flag in the hashtable.  If it was used before, use
		// the existing ID.  Otherwise add a new entry.
		vim_strncpy(కీలకం, prevp, p - prevp);
		hi = hash_కనుగొను(&aff->af_comp, కీలకం);
		ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		    id = HI2CI(hi)->ci_newID;
		లేకపోతే
		{
		    ci = (compitem_T *)getroom(spin, పరిమాణం(compitem_T), TRUE);
		    ఒకవేళ (ci == ఖాళీ)
			ఆపు;
		    STRCPY(ci->ci_కీలకం, కీలకం);
		    ci->ci_flag = flag;
		    // Avoid using a flag ID that has a special meaning in a
		    // regexp (also inside []).
		    చేయి
		    {
			check_renumber(spin);
			id = spin->si_newcompID--;
		    } అయితే (vim_strchr((char_u *)"/?*+[]\\-^", id) != ఖాళీ);
		    ci->ci_newID = id;
		    hash_కూడు(&aff->af_comp, ci->ci_కీలకం, "spelling");
		}
		*tp++ = id;
	    }
	    ఒకవేళ (aff->af_flagtype == AFT_అంకె && *p == ',')
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
    స్థిర శూన్య
check_renumber(spellinfo_T *spin)
{
    ఒకవేళ (spin->si_newprefID == spin->si_newcompID && spin->si_newcompID < 128)
    {
	spin->si_newprefID = 127;
	spin->si_newcompID = 255;
    }
}

/*
 * Append one affix or compound ID to "store_afflist".
 * Returns FAIL when this would overrun the fixed-size buffer.
 */
    స్థిర పూర్ణాంకం
store_afflist_కూడు(
    char_u	*store_afflist,
    పూర్ణాంకం		*cntp,
    పూర్ణాంకం		id)
{
    ఒకవేళ (*cntp >= MAXWLEN - 1)
    {
	emsg(_(e_too_many_postponed_prefixes_spell));
	తిరిగి FAIL;
    }
    store_afflist[(*cntp)++] = id;
    store_afflist[*cntp] = NUL;
    తిరిగి OK;
}

/*
 * Return TRUE if flag "flag" appears in affix list "afflist".
 */
    స్థిర పూర్ణాంకం
flag_లోపల_afflist(పూర్ణాంకం flagtype, char_u *afflist, గుర్తులేని flag)
{
    char_u	*p;
    గుర్తులేని	n;

    ఎంపిక (flagtype)
    {
	స్థితి AFT_CHAR:
	    తిరిగి vim_strchr(afflist, flag) != ఖాళీ;

	స్థితి AFT_CAPLONG:
	స్థితి AFT_LONG:
	    కొరకు (p = afflist; *p != NUL; )
	    {
		n = mb_ptr2char_adv(&p);
		ఒకవేళ ((flagtype == AFT_LONG || (n >= 'A' && n <= 'Z'))
								 && *p != NUL)
		    n = mb_ptr2char_adv(&p) + (n << 16);
		ఒకవేళ (n == flag)
		    తిరిగి TRUE;
	    }
	    ఆపు;

	స్థితి AFT_అంకె:
	    కొరకు (p = afflist; *p != NUL; )
	    {
		n = getdigits(&p);
		ఒకవేళ (n == 0)
		    n = ZERO_FLAG;
		ఒకవేళ (n == flag)
		    తిరిగి TRUE;
		ఒకవేళ (*p != NUL)	// skip over comma
		    ++p;
	    }
	    ఆపు;
    }
    తిరిగి FALSE;
}

/*
 * Give a warning when "spinval" and "affval" numbers are set and not the same.
 */
    స్థిర శూన్య
aff_check_సంఖ్య(పూర్ణాంకం spinval, పూర్ణాంకం affval, అక్షరం *name)
{
    ఒకవేళ (spinval != 0 && spinval != affval)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Give a warning when "spinval" and "affval" strings are set and not the same.
 */
    స్థిర శూన్య
aff_check_string(char_u *spinval, char_u *affval, అక్షరం *name)
{
    ఒకవేళ (spinval != ఖాళీ && STRCMP(spinval, affval) != 0)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Return TRUE if strings "s1" and "s2" are equal.  Also consider both being
 * NULL as equal.
 */
    స్థిర పూర్ణాంకం
str_equal(char_u *s1, char_u *s2)
{
    ఒకవేళ (s1 == ఖాళీ || s2 == ఖాళీ)
	తిరిగి s1 == s2;
    తిరిగి STRCMP(s1, s2) == 0;
}

/*
 * Add a from-to item to "gap".  Used for REP and SAL items.
 * They are stored case-folded.
 */
    స్థిర శూన్య
కూడు_fromto(
    spellinfo_T	*spin,
    garray_T	*gap,
    char_u	*from,
    char_u	*గమ్యం)
{
    fromto_T	*ftp;
    char_u	మాట[MAXWLEN];

    ఒకవేళ (ga_grow(gap, 1) == FAIL)
	తిరిగి;

    ftp = ((fromto_T *)gap->ga_దత్తాంశం) + gap->ga_len;
    (శూన్య)spell_casefold(curwin, from, (పూర్ణాంకం)STRLEN(from), మాట, MAXWLEN);
    ftp->ft_from = getroom_save(spin, మాట);
    (శూన్య)spell_casefold(curwin, గమ్యం, (పూర్ణాంకం)STRLEN(గమ్యం), మాట, MAXWLEN);
    ftp->ft_గమ్యం = getroom_save(spin, మాట);
    ++gap->ga_len;
}

/*
 * Convert a boolean argument in a SAL line to TRUE or FALSE;
 */
    స్థిర పూర్ణాంకం
sal_గమ్యం_bool(char_u *s)
{
    తిరిగి STRCMP(s, "1") == 0 || STRCMP(s, "true") == 0;
}

/*
 * Free the structure filled by spell_read_aff().
 */
    స్థిర శూన్య
spell_free_aff(afffile_T *aff)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    పూర్ణాంకం		todo;
    affheader_T	*ah;
    affentry_T	*ae;

    vim_free(aff->af_enc);

    // All this trouble to free the "ae_prog" items...
    కొరకు (ht = &aff->af_pref; ; ht = &aff->af_suff)
    {
	todo = (పూర్ణాంకం)ht->ht_వాడిన;
	FOR_ALL_HASHTAB_వస్తువులు(ht, hi, todo)
	{
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	    {
		--todo;
		ah = HI2AH(hi);
		కొరకు (ae = ah->ah_మొదటి; ae != ఖాళీ; ae = ae->ae_తదుపరి)
		    vim_regfree(ae->ae_prog);
	    }
	}
	ఒకవేళ (ht == &aff->af_suff)
	    ఆపు;
    }

    hash_clear(&aff->af_pref);
    hash_clear(&aff->af_suff);
    hash_clear(&aff->af_comp);
}

/*
 * Read dictionary file "fname".
 * Returns OK or FAIL;
 */
    స్థిర పూర్ణాంకం
spell_read_dic(spellinfo_T *spin, char_u *fname, afffile_T *affile)
{
    hashtab_T	ht;
    char_u	పంక్తి[MAXLINELEN];
    char_u	*p;
    char_u	*afflist;
    char_u	store_afflist[MAXWLEN];
    పూర్ణాంకం		pfxlen;
    పూర్ణాంకం		totlen;
    పూర్ణాంకం		need_affix;
    char_u	*dw;
    char_u	*pc;
    char_u	*w;
    పూర్ణాంకం		l;
    hash_T	hash;
    hashitem_T	*hi;
    FILE	*fd;
    పూర్ణాంకం		lnum = 1;
    పూర్ణాంకం		non_ascii = 0;
    పూర్ణాంకం		retval = OK;
    char_u	సందేశం_2[MAXLINELEN + MAXWLEN];
    పూర్ణాంకం		flags;
    పూర్ణాంకం		duplicate = 0;
    సమయం_T	చివరి_సందేశం_సమయం = 0;

    /*
     * Open the file.
     */
    fd = mch_fopen((అక్షరం *)fname, "r");
    ఒకవేళ (fd == ఖాళీ)
    {
	semsg(_(e_cant_open_file_str), fname);
	తిరిగి FAIL;
    }

    // The hashtable is only used to detect duplicated words.
    hash_init(&ht);

    vim_snprintf((అక్షరం *)IObuff, IOSIZE,
				  _("Reading dictionary file %s..."), fname);
    spell_సందేశం(spin, IObuff);

    // start with a message for the first line
    spin->si_సందేశం_లెక్క = 999999;

    // Read and ignore the first line: word count.
    ఒకవేళ (vim_fgets(పంక్తి, MAXLINELEN, fd) || !vim_isdigit(*skipwhite(పంక్తి)))
	semsg(_(e_no_మాట_లెక్క_లోపల_str), fname);

    /*
     * Read all the lines in the file one by one.
     * The words are converted to 'encoding' here, before being added to
     * the hashtable.
     */
    అయితే (!vim_fgets(పంక్తి, MAXLINELEN, fd) && !got_int)
    {
	పంక్తి_breakcheck();
	++lnum;
	ఒకవేళ (పంక్తి[0] == '#' || పంక్తి[0] == '/')
	    కొనసాగు;	// comment line

	// Remove CR, LF and white space from the end.  White space halfway
	// the word is kept to allow e.g., "et al.".
	l = (పూర్ణాంకం)STRLEN(పంక్తి);
	అయితే (l > 0 && పంక్తి[l - 1] <= ' ')
	    --l;
	ఒకవేళ (l == 0)
	    కొనసాగు;	// empty line
	పంక్తి[l] = NUL;

	// Convert from "SET" to 'encoding' when needed.
	ఒకవేళ (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, పంక్తి, ఖాళీ);
	    ఒకవేళ (pc == ఖాళీ)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
						       fname, lnum, పంక్తి);
		కొనసాగు;
	    }
	    w = pc;
	}
	లేకపోతే
	{
	    pc = ఖాళీ;
	    w = పంక్తి;
	}

	// Truncate the word at the "/", set "afflist" to what follows.
	// Replace "\/" by "/" and "\\" by "\".
	afflist = ఖాళీ;
	కొరకు (p = w; *p != NUL; MB_PTR_ADV(p))
	{
	    ఒకవేళ (*p == '\\' && (p[1] == '\\' || p[1] == '/'))
		STRMOVE(p, p + 1);
	    లేకపోతే ఒకవేళ (*p == '/')
	    {
		*p = NUL;
		afflist = p + 1;
		ఆపు;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	ఒకవేళ (spin->si_ascii && has_non_ascii(w))
	{
	    ++non_ascii;
	    vim_free(pc);
	    కొనసాగు;
	}

	// This takes time, print a message every 10000 words, but not more
	// often than once per second.
	ఒకవేళ (spin->si_verbose && spin->si_సందేశం_లెక్క > 10000)
	{
	    spin->si_సందేశం_లెక్క = 0;
	    ఒకవేళ (vim_సమయం() > చివరి_సందేశం_సమయం)
	    {
		చివరి_సందేశం_సమయం = vim_సమయం();
		vim_snprintf((అక్షరం *)సందేశం_2, పరిమాణం(సందేశం_2),
			_("line %6d, word %6ld - %s"),
			   lnum, spin->si_foldwcount + spin->si_keepwcount, w);
		సందేశం_ప్రారంభం();
		సందేశం_outtrans_long_attr(సందేశం_2, 0);
		సందేశం_clr_eos();
		సందేశం_didout = FALSE;
		సందేశం_నిలువు = 0;
		బహిర్గతం_flush();
	    }
	}

	// Store the word in the hashtable to be able to find duplicates.
	dw = getroom_save(spin, w);
	ఒకవేళ (dw == ఖాళీ)
	{
	    retval = FAIL;
	    vim_free(pc);
	    ఆపు;
	}

	hash = hash_hash(dw);
	hi = hash_lookup(&ht, dw, hash);
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    ఒకవేళ (p_verbose > 0)
		smsg(_("Duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    లేకపోతే ఒకవేళ (duplicate == 0)
		smsg(_("First duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ++duplicate;
	}
	లేకపోతే
	    hash_కూడు_అంశం(&ht, hi, dw, hash);

	flags = 0;
	store_afflist[0] = NUL;
	pfxlen = 0;
	totlen = 0;
	need_affix = FALSE;
	ఒకవేళ (afflist != ఖాళీ)
	{
	    // Extract flags from the affix list.
	    flags |= get_affix_flags(affile, afflist);

	    ఒకవేళ (affile->af_needaffix != 0 && flag_లోపల_afflist(
			  affile->af_flagtype, afflist, affile->af_needaffix))
		need_affix = TRUE;

	    ఒకవేళ (affile->af_pfxpostpone)
	    {
		// Need to store the list of prefix IDs with the word.
		ఒకవేళ (get_pfxlist(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    ఆపు;
		}
		pfxlen = totlen;
	    }

	    ఒకవేళ (spin->si_compflags != ఖాళీ)
	    {
		// Need to store the list of compound flags with the word.
		// Concatenate them to the list of prefix IDs.
		ఒకవేళ (get_compflags(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    ఆపు;
		}
	    }
	}

	// Add the word to the word tree(s).
	ఒకవేళ (store_మాట(spin, dw, flags, spin->si_region,
					   store_afflist, need_affix) == FAIL)
	    retval = FAIL;

	ఒకవేళ (afflist != ఖాళీ)
	{
	    // Find all matching suffixes and add the resulting words.
	    // Additionally do matching prefixes that combine.
	    ఒకవేళ (store_aff_మాట(spin, dw, afflist, affile,
			   &affile->af_suff, &affile->af_pref,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;

	    // Find all matching prefixes and add the resulting words.
	    ఒకవేళ (store_aff_మాట(spin, dw, afflist, affile,
			  &affile->af_pref, ఖాళీ,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;
	}

	vim_free(pc);
    }

    ఒకవేళ (duplicate > 0)
	smsg(_("%d duplicate word(s) in %s"), duplicate, fname);
    ఒకవేళ (spin->si_ascii && non_ascii > 0)
	smsg(_("Ignored %d word(s) with non-ASCII characters in %s"),
							    non_ascii, fname);
    hash_clear(&ht);

    fclose(fd);
    తిరిగి retval;
}

/*
 * Check for affix flags in "afflist" that are turned into word flags.
 * Return WF_ flags.
 */
    స్థిర పూర్ణాంకం
get_affix_flags(afffile_T *affile, char_u *afflist)
{
    పూర్ణాంకం		flags = 0;

    ఒకవేళ (affile->af_keepcase != 0 && flag_లోపల_afflist(
			   affile->af_flagtype, afflist, affile->af_keepcase))
	flags |= WF_KEEPCAP | WF_FIXCAP;
    ఒకవేళ (affile->af_rare != 0 && flag_లోపల_afflist(
			       affile->af_flagtype, afflist, affile->af_rare))
	flags |= WF_RARE;
    ఒకవేళ (affile->af_bad != 0 && flag_లోపల_afflist(
				affile->af_flagtype, afflist, affile->af_bad))
	flags |= WF_BANNED;
    ఒకవేళ (affile->af_needcomp != 0 && flag_లోపల_afflist(
			   affile->af_flagtype, afflist, affile->af_needcomp))
	flags |= WF_NEEDCOMP;
    ఒకవేళ (affile->af_comproot != 0 && flag_లోపల_afflist(
			   affile->af_flagtype, afflist, affile->af_comproot))
	flags |= WF_COMPROOT;
    ఒకవేళ (affile->af_nosuggest != 0 && flag_లోపల_afflist(
			  affile->af_flagtype, afflist, affile->af_nosuggest))
	flags |= WF_NOSUGGEST;
    తిరిగి flags;
}

/*
 * Get the list of prefix IDs from the affix list "afflist".
 * Used for PFXPOSTPONE.
 * Put the resulting flags in "store_afflist[MAXWLEN]" with a terminating NUL.
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    స్థిర పూర్ణాంకం
get_pfxlist(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    పూర్ణాంకం		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    పూర్ణాంకం		id;
    char_u	కీలకం[AH_కీలకం_LEN];
    hashitem_T	*hi;

    కొరకు (p = afflist; *p != NUL; )
    {
	prevp = p;
	ఒకవేళ (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a postponed prefix flag if it appears in "af_pref"
	    // and its ID is not zero.
	    vim_strncpy(కీలకం, prevp, p - prevp);
	    hi = hash_కనుగొను(&affile->af_pref, కీలకం);
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	    {
		id = HI2AH(hi)->ah_newID;
		ఒకవేళ (id != 0 && store_afflist_కూడు(store_afflist, cntp, id) == FAIL)
		    తిరిగి FAIL;
	    }
	}
	ఒకవేళ (affile->af_flagtype == AFT_అంకె && *p == ',')
	    ++p;
    }

    తిరిగి OK;
}

/*
 * Get the list of compound IDs from the affix list "afflist" that are used
 * for compound words.
 * Puts the flags in "store_afflist[]".
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    స్థిర పూర్ణాంకం
get_compflags(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    పూర్ణాంకం		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    char_u	కీలకం[AH_కీలకం_LEN];
    hashitem_T	*hi;

    కొరకు (p = afflist; *p != NUL; )
    {
	prevp = p;
	ఒకవేళ (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a compound flag if it appears in "af_comp".
	    vim_strncpy(కీలకం, prevp, p - prevp);
	    hi = hash_కనుగొను(&affile->af_comp, కీలకం);
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi)
		    && store_afflist_కూడు(store_afflist, cntp,
					      HI2CI(hi)->ci_newID) == FAIL)
		తిరిగి FAIL;
	}
	ఒకవేళ (affile->af_flagtype == AFT_అంకె && *p == ',')
	    ++p;
    }

    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
store_aff_మాట(
    spellinfo_T	*spin,		// spell info
    char_u	*మాట,		// basic word start
    char_u	*afflist,	// list of names of supported affixes
    afffile_T	*affile,
    hashtab_T	*ht,
    hashtab_T	*xht,
    పూర్ణాంకం		condit,		// CONDIT_SUF et al.
    పూర్ణాంకం		flags,		// flags for the word
    char_u	*pfxlist,	// list of prefix IDs
    పూర్ణాంకం		pfxlen)		// nr of flags in "pfxlist" for prefixes, rest
				// is compound flags
{
    పూర్ణాంకం		todo;
    hashitem_T	*hi;
    affheader_T	*ah;
    affentry_T	*ae;
    char_u	newword[MAXWLEN];
    పూర్ణాంకం		retval = OK;
    పూర్ణాంకం		i, j;
    char_u	*p;
    పూర్ణాంకం		use_flags;
    char_u	*use_pfxlist;
    పూర్ణాంకం		use_pfxlen;
    పూర్ణాంకం		need_affix;
    char_u	store_afflist[MAXWLEN];
    char_u	pfx_pfxlist[MAXWLEN];
    size_t	wordlen = STRLEN(మాట);
    పూర్ణాంకం		use_condit;

    todo = (పూర్ణాంకం)ht->ht_వాడిన;
    కొరకు (hi = ht->ht_array; todo > 0 && retval == OK; ++hi)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;
	    ah = HI2AH(hi);

	    // Check that the affix combines, if required, and that the word
	    // supports this affix.
	    ఒకవేళ (((condit & CONDIT_COMB) == 0 || ah->ah_combine)
		    && flag_లోపల_afflist(affile->af_flagtype, afflist,
								 ah->ah_flag))
	    {
		// Loop over all affix entries with this name.
		కొరకు (ae = ah->ah_మొదటి; ae != ఖాళీ; ae = ae->ae_తదుపరి)
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
		    ఒకవేళ ((xht != ఖాళీ || !affile->af_pfxpostpone
				|| ae->ae_chop != ఖాళీ
				|| ae->ae_flags != ఖాళీ)
			    && (ae->ae_chop == ఖాళీ
				|| STRLEN(ae->ae_chop) < wordlen)
			    && (ae->ae_prog == ఖాళీ
				|| vim_regexec_prog(&ae->ae_prog, FALSE,
							    మాట, (colnr_T)0))
			    && (((condit & CONDIT_CFIX) == 0)
				== ((condit & CONDIT_AFF) == 0
				    || ae->ae_flags == ఖాళీ
				    || !flag_లోపల_afflist(affile->af_flagtype,
					ae->ae_flags, affile->af_circumfix))))
		    {
			// Match.  Remove the chop and add the affix.
			ఒకవేళ (xht == ఖాళీ)
			{
			    // prefix: chop/add at the start of the word
			    ఒకవేళ (ae->ae_కూడు == ఖాళీ)
				*newword = NUL;
			    లేకపోతే
				vim_strncpy(newword, ae->ae_కూడు, MAXWLEN - 1);
			    p = మాట;
			    ఒకవేళ (ae->ae_chop != ఖాళీ)
			    {
				// Skip chop string.
				ఒకవేళ (has_mbyte)
				{
				    i = mb_charlen(ae->ae_chop);
				    కొరకు ( ; i > 0; --i)
					MB_PTR_ADV(p);
				}
				లేకపోతే
				    p += STRLEN(ae->ae_chop);
			    }
			    // Note: this silently truncates the buffer, but this should
			    // not happen in practice
			    STRNCAT(newword, p, MAXWLEN - STRLEN(newword) - 1);
			}
			లేకపోతే
			{
			    // suffix: chop/add at the end of the word
			    vim_strncpy(newword, మాట, MAXWLEN - 1);
			    ఒకవేళ (ae->ae_chop != ఖాళీ)
			    {
				// Remove chop string.
				p = newword + STRLEN(newword);
				i = (పూర్ణాంకం)MB_CHARLEN(ae->ae_chop);
				కొరకు ( ; i > 0; --i)
				    MB_PTR_వెనుక(newword, p);
				*p = NUL;
			    }
			    ఒకవేళ (ae->ae_కూడు != ఖాళీ)
				// Note: this silently truncates the buffer, but this should
				// not happen in practice
				STRNCAT(newword, ae->ae_కూడు, MAXWLEN - STRLEN(newword) - 1);
			}

			use_flags = flags;
			use_pfxlist = pfxlist;
			use_pfxlen = pfxlen;
			need_affix = FALSE;
			use_condit = condit | CONDIT_COMB | CONDIT_AFF;
			ఒకవేళ (ae->ae_flags != ఖాళీ)
			{
			    // Extract flags from the affix list.
			    use_flags |= get_affix_flags(affile, ae->ae_flags);

			    ఒకవేళ (affile->af_needaffix != 0 && flag_లోపల_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_needaffix))
				need_affix = TRUE;

			    // When there is a CIRCUMFIX flag the other affix
			    // must also have it and we don't add the word
			    // with one affix.
			    ఒకవేళ (affile->af_circumfix != 0 && flag_లోపల_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_circumfix))
			    {
				use_condit |= CONDIT_CFIX;
				ఒకవేళ ((condit & CONDIT_CFIX) == 0)
				    need_affix = TRUE;
			    }

			    ఒకవేళ (affile->af_pfxpostpone
						|| spin->si_compflags != ఖాళీ)
			    {
				పూర్ణాంకం listlen = 0;

				ఒకవేళ (affile->af_pfxpostpone)
				{
				    // Get prefix IDS from the affix list.
				    ఒకవేళ (get_pfxlist(affile, ae->ae_flags,
						    store_afflist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					ఆపు;
				    }
				    use_pfxlen = listlen;
				}
				లేకపోతే
				    use_pfxlen = 0;
				use_pfxlist = store_afflist;

				// Combine the prefix IDs. Avoid adding the
				// same ID twice.
				కొరకు (i = 0; i < pfxlen; ++i)
				{
				    కొరకు (j = 0; j < use_pfxlen; ++j)
					ఒకవేళ (pfxlist[i] == use_pfxlist[j])
					    ఆపు;
				    ఒకవేళ (j == use_pfxlen
					    && store_afflist_కూడు(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					ఆపు;
				    }
				    use_pfxlen = listlen;
				}
				ఒకవేళ (retval == FAIL)
				    ఆపు;

				ఒకవేళ (spin->si_compflags != ఖాళీ)
				    // Get compound IDS from the affix list.
				    ఒకవేళ (get_compflags(affile, ae->ae_flags,
						      use_pfxlist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					ఆపు;
				    }
				ఒకవేళ (retval == FAIL)
				    ఆపు;

				// Combine the list of compound flags.
				// Concatenate them to the prefix IDs list.
				// Avoid adding the same ID twice.
				కొరకు (i = pfxlen; pfxlist[i] != NUL; ++i)
				{
				    కొరకు (j = use_pfxlen;
						   use_pfxlist[j] != NUL; ++j)
					ఒకవేళ (pfxlist[i] == use_pfxlist[j])
					    ఆపు;
				    ఒకవేళ (use_pfxlist[j] == NUL
					    && store_afflist_కూడు(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					ఆపు;
				    }
				}
				ఒకవేళ (retval == FAIL)
				    ఆపు;
			    }
			}

			// Obey a "COMPOUNDFORBIDFLAG" of the affix: don't
			// use the compound flags.
			ఒకవేళ (use_pfxlist != ఖాళీ && ae->ae_compforbid)
			{
			    vim_strncpy(pfx_pfxlist, use_pfxlist, use_pfxlen);
			    use_pfxlist = pfx_pfxlist;
			}

			// When there are postponed prefixes...
			ఒకవేళ (spin->si_prefroot != ఖాళీ
				&& spin->si_prefroot->wn_sibling != ఖాళీ)
			{
			    // ... add a flag to indicate an affix was used.
			    use_flags |= WF_HAS_AFF;

			    // ... don't use a prefix list if combining
			    // affixes is not allowed.  But do use the
			    // compound flags after them.
			    ఒకవేళ (!ah->ah_combine && use_pfxlist != ఖాళీ)
				use_pfxlist += use_pfxlen;
			}

			// When compounding is supported and there is no
			// "COMPOUNDPERMITFLAG" then forbid compounding on the
			// side where the affix is applied.
			ఒకవేళ (spin->si_compflags != ఖాళీ && !ae->ae_comppermit)
			{
			    ఒకవేళ (xht != ఖాళీ)
				use_flags |= WF_NOCOMPAFT;
			    లేకపోతే
				use_flags |= WF_NOCOMPBEF;
			}

			// Store the modified word.
			ఒకవేళ (store_మాట(spin, newword, use_flags,
						 spin->si_region, use_pfxlist,
							  need_affix) == FAIL)
			    retval = FAIL;

			// When added a prefix or a first suffix and the affix
			// has flags may add a(nother) suffix.  RECURSIVE!
			ఒకవేళ ((condit & CONDIT_SUF) && ae->ae_flags != ఖాళీ)
			    ఒకవేళ (store_aff_మాట(spin, newword, ae->ae_flags,
					affile, &affile->af_suff, xht,
					   use_condit & (xht == ఖాళీ
							? ~0 :  ~CONDIT_SUF),
				      use_flags, use_pfxlist, pfxlen) == FAIL)
				retval = FAIL;

			// When added a suffix and combining is allowed also
			// try adding a prefix additionally.  Both for the
			// word flags and for the affix flags.  RECURSIVE!
			ఒకవేళ (xht != ఖాళీ && ah->ah_combine)
			{
			    ఒకవేళ (store_aff_మాట(spin, newword,
					afflist, affile,
					xht, ఖాళీ, use_condit,
					use_flags, use_pfxlist,
					pfxlen) == FAIL
				    || (ae->ae_flags != ఖాళీ
					&& store_aff_మాట(spin, newword,
					    ae->ae_flags, affile,
					    xht, ఖాళీ, use_condit,
					    use_flags, use_pfxlist,
					    pfxlen) == FAIL))
				retval = FAIL;
			}
		    }
		}
	    }
	}
    }

    తిరిగి retval;
}

/*
 * Read a file with a list of words.
 */
    స్థిర పూర్ణాంకం
spell_read_wordfile(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    దీర్ఘ	lnum = 0;
    char_u	rline[MAXLINELEN];
    char_u	*పంక్తి;
    char_u	*pc = ఖాళీ;
    char_u	*p;
    పూర్ణాంకం		l;
    పూర్ణాంకం		retval = OK;
    పూర్ణాంకం		did_మాట = FALSE;
    పూర్ణాంకం		non_ascii = 0;
    పూర్ణాంకం		flags;
    పూర్ణాంకం		regionmask;

    /*
     * Open the file.
     */
    fd = mch_fopen((అక్షరం *)fname, "r");
    ఒకవేళ (fd == ఖాళీ)
    {
	semsg(_(e_cant_open_file_str), fname);
	తిరిగి FAIL;
    }

    vim_snprintf((అక్షరం *)IObuff, IOSIZE, _("Reading word file %s..."), fname);
    spell_సందేశం(spin, IObuff);

    /*
     * Read all the lines in the file one by one.
     */
    అయితే (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	పంక్తి_breakcheck();
	++lnum;

	// Skip comment lines.
	ఒకవేళ (*rline == '#')
	    కొనసాగు;

	// Remove CR, LF and white space from the end.
	l = (పూర్ణాంకం)STRLEN(rline);
	అయితే (l > 0 && rline[l - 1] <= ' ')
	    --l;
	ఒకవేళ (l == 0)
	    కొనసాగు;	// empty or blank line
	rline[l] = NUL;

	// Convert from "/encoding={encoding}" to 'encoding' when needed.
	vim_free(pc);
	ఒకవేళ (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, ఖాళీ);
	    ఒకవేళ (pc == ఖాళీ)
	    {
		smsg(_("Conversion failure for word in %s line %ld: %s"),
							   fname, lnum, rline);
		కొనసాగు;
	    }
	    పంక్తి = pc;
	}
	లేకపోతే
	{
	    pc = ఖాళీ;
	    పంక్తి = rline;
	}

	ఒకవేళ (*పంక్తి == '/')
	{
	    ++పంక్తి;
	    ఒకవేళ (STRNCMP(పంక్తి, "encoding=", 9) == 0)
	    {
		ఒకవేళ (spin->si_conv.vc_type != CONV_NONE)
		    smsg(_("Duplicate /encoding= line ignored in %s line %ld: %s"),
						       fname, lnum, పంక్తి - 1);
		లేకపోతే ఒకవేళ (did_మాట)
		    smsg(_("/encoding= line after word ignored in %s line %ld: %s"),
						       fname, lnum, పంక్తి - 1);
		లేకపోతే
		{
		    char_u	*enc;

		    // Setup for conversion to 'encoding'.
		    పంక్తి += 9;
		    enc = enc_canonize(పంక్తి);
		    ఒకవేళ (enc != ఖాళీ && !spin->si_ascii
			    && convert_అమరిక(&spin->si_conv, enc,
							       p_enc) == FAIL)
			smsg(_("Conversion in %s not supported: from %s to %s"),
							  fname, పంక్తి, p_enc);
		    vim_free(enc);
		    spin->si_conv.vc_fail = TRUE;
		}
		కొనసాగు;
	    }

	    ఒకవేళ (STRNCMP(పంక్తి, "regions=", 8) == 0)
	    {
		ఒకవేళ (spin->si_region_లెక్క > 1)
		    smsg(_("Duplicate /regions= line ignored in %s line %ld: %s"),
						       fname, lnum, పంక్తి);
		లేకపోతే
		{
		    పంక్తి += 8;
		    ఒకవేళ (STRLEN(పంక్తి) > MAXREGIONS * 2)
			smsg(_("Too many regions in %s line %ld: %s"),
						       fname, lnum, పంక్తి);
		    లేకపోతే
		    {
			spin->si_region_లెక్క = (పూర్ణాంకం)STRLEN(పంక్తి) / 2;
			STRCPY(spin->si_region_name, పంక్తి);

			// Adjust the mask for a word valid in all regions.
			spin->si_region = (1 << spin->si_region_లెక్క) - 1;
		    }
		}
		కొనసాగు;
	    }

	    smsg(_("/ line ignored in %s line %ld: %s"),
						       fname, lnum, పంక్తి - 1);
	    కొనసాగు;
	}

	flags = 0;
	regionmask = spin->si_region;

	// Check for flags and region after a slash.
	p = vim_strchr(పంక్తి, '/');
	ఒకవేళ (p != ఖాళీ)
	{
	    *p++ = NUL;
	    అయితే (*p != NUL)
	    {
		ఒకవేళ (*p == '=')		// keep-case word
		    flags |= WF_KEEPCAP | WF_FIXCAP;
		లేకపోతే ఒకవేళ (*p == '!')	// Bad, bad, wicked word.
		    flags |= WF_BANNED;
		లేకపోతే ఒకవేళ (*p == '?')	// Rare word.
		    flags |= WF_RARE;
		లేకపోతే ఒకవేళ (VIM_ISDIGIT(*p)) // region number(s)
		{
		    ఒకవేళ ((flags & WF_REGION) == 0)   // first one
			regionmask = 0;
		    flags |= WF_REGION;

		    l = *p - '0';
		    ఒకవేళ (l == 0 || l > spin->si_region_లెక్క)
		    {
			smsg(_("Invalid region nr in %s line %ld: %s"),
							  fname, lnum, p);
			ఆపు;
		    }
		    regionmask |= 1 << (l - 1);
		}
		లేకపోతే
		{
		    smsg(_("Unrecognized flags in %s line %ld: %s"),
							      fname, lnum, p);
		    ఆపు;
		}
		++p;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	ఒకవేళ (spin->si_ascii && has_non_ascii(పంక్తి))
	{
	    ++non_ascii;
	    కొనసాగు;
	}

	// Normal word: store it.
	ఒకవేళ (store_మాట(spin, పంక్తి, flags, regionmask, ఖాళీ, FALSE) == FAIL)
	{
	    retval = FAIL;
	    ఆపు;
	}
	did_మాట = TRUE;
    }

    vim_free(pc);
    fclose(fd);

    ఒకవేళ (spin->si_ascii && non_ascii > 0)
    {
	vim_snprintf((అక్షరం *)IObuff, IOSIZE,
		  _("Ignored %d words with non-ASCII characters"), non_ascii);
	spell_సందేశం(spin, IObuff);
    }

    తిరిగి retval;
}

/*
 * Get part of an sblock_T, "len" bytes long.
 * This avoids calling free() for every little struct we use (and keeping
 * track of them).
 * The memory is cleared to all zeros.
 * Returns NULL when out of memory.
 */
    స్థిర శూన్య *
getroom(
    spellinfo_T *spin,
    size_t	len,		// length needed
    పూర్ణాంకం		align)		// align for pointer
{
    char_u	*p;
    sblock_T	*bl = spin->si_ఖండాలు;

    ఒకవేళ (align && bl != ఖాళీ)
	// Round size up for alignment.  On some systems structures need to be
	// aligned to the size of a pointer (e.g., SPARC).
	bl->sb_వాడిన = (bl->sb_వాడిన + పరిమాణం(అక్షరం *) - 1)
						      & ~(పరిమాణం(అక్షరం *) - 1);

    ఒకవేళ (bl == ఖాళీ || bl->sb_వాడిన + len > SBLOCKSIZE)
    {
	ఒకవేళ (len >= SBLOCKSIZE)
	    bl = ఖాళీ;
	లేకపోతే
	    // Allocate a block of memory. It is not freed until much later.
	    bl = alloc_clear(offsetof(sblock_T, sb_దత్తాంశం) + SBLOCKSIZE + 1);
	ఒకవేళ (bl == ఖాళీ)
	{
	    ఒకవేళ (!spin->si_did_emsg)
	    {
		emsg(_(e_insufficient_జ్ఞాపకం_మాట_list_will_be_incomplete));
		spin->si_did_emsg = TRUE;
	    }
	    తిరిగి ఖాళీ;
	}
	bl->sb_తదుపరి = spin->si_ఖండాలు;
	spin->si_ఖండాలు = bl;
	bl->sb_వాడిన = 0;
	++spin->si_ఖండాలు_cnt;
    }

    p = bl->sb_దత్తాంశం + bl->sb_వాడిన;
    bl->sb_వాడిన += (పూర్ణాంకం)len;

    తిరిగి p;
}

/*
 * Make a copy of a string into memory allocated with getroom().
 * Returns NULL when out of memory.
 */
    స్థిర char_u *
getroom_save(spellinfo_T *spin, char_u *s)
{
    char_u	*sc;

    sc = (char_u *)getroom(spin, STRLEN(s) + 1, FALSE);
    ఒకవేళ (sc != ఖాళీ)
	STRCPY(sc, s);
    తిరిగి sc;
}


/*
 * Free the list of allocated sblock_T.
 */
    స్థిర శూన్య
free_ఖండాలు(sblock_T *bl)
{
    sblock_T	*తదుపరి;

    అయితే (bl != ఖాళీ)
    {
	తదుపరి = bl->sb_తదుపరి;
	vim_free(bl);
	bl = తదుపరి;
    }
}

/*
 * Allocate the root of a word tree.
 * Returns NULL when out of memory.
 */
    స్థిర wordnode_T *
wordtree_alloc(spellinfo_T *spin)
{
    తిరిగి (wordnode_T *)getroom(spin, పరిమాణం(wordnode_T), TRUE);
}

/*
 * Return TRUE if "word" contains valid word characters.
 * Control characters and trailing '/' are invalid.  Space is OK.
 */
    స్థిర పూర్ణాంకం
valid_spell_మాట(char_u *మాట, char_u *చివర)
{
    char_u *p;

    ఒకవేళ (enc_utf8 && !utf_valid_string(మాట, చివర))
	తిరిగి FALSE;
    కొరకు (p = మాట; *p != NUL && p < చివర; p += mb_ptr2len(p))
	ఒకవేళ (*p < ' ' || (p[0] == '/' && p[1] == NUL))
	    తిరిగి FALSE;
    తిరిగి TRUE;
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
    స్థిర పూర్ణాంకం
store_మాట(
    spellinfo_T	*spin,
    char_u	*మాట,
    పూర్ణాంకం		flags,		// extra flags, WF_BANNED
    పూర్ణాంకం		region,		// supported region(s)
    char_u	*pfxlist,	// list of prefix IDs or NULL
    పూర్ణాంకం		need_affix)	// only store word with affix ID
{
    పూర్ణాంకం		len = (పూర్ణాంకం)STRLEN(మాట);
    పూర్ణాంకం		ct = captype(మాట, మాట + len);
    char_u	foldword[MAXWLEN];
    పూర్ణాంకం		res = OK;
    char_u	*p;

    // Avoid adding illegal bytes to the word tree.
    ఒకవేళ (!valid_spell_మాట(మాట, మాట + len))
	తిరిగి FAIL;

    (శూన్య)spell_casefold(curwin, మాట, len, foldword, MAXWLEN);
    కొరకు (p = pfxlist; res == OK; ++p)
    {
	ఒకవేళ (!need_affix || (p != ఖాళీ && *p != NUL))
	    res = వృక్షం_కూడు_మాట(spin, foldword, spin->si_foldroot, ct | flags,
						  region, p == ఖాళీ ? 0 : *p);
	ఒకవేళ (p == ఖాళీ || *p == NUL)
	    ఆపు;
    }
    ++spin->si_foldwcount;

    ఒకవేళ (res == OK && (ct == WF_KEEPCAP || (flags & WF_KEEPCAP)))
    {
	కొరకు (p = pfxlist; res == OK; ++p)
	{
	    ఒకవేళ (!need_affix || (p != ఖాళీ && *p != NUL))
		res = వృక్షం_కూడు_మాట(spin, మాట, spin->si_keeproot, flags,
						  region, p == ఖాళీ ? 0 : *p);
	    ఒకవేళ (p == ఖాళీ || *p == NUL)
		ఆపు;
	}
	++spin->si_keepwcount;
    }
    తిరిగి res;
}

/*
 * Add word "word" to a word tree at "root".
 * When "flags" < 0 we are adding to the prefix tree where "flags" is used for
 * "rare" and "region" is the condition nr.
 * Returns FAIL when out of memory.
 */
    స్థిర పూర్ణాంకం
వృక్షం_కూడు_మాట(
    spellinfo_T	*spin,
    char_u	*మాట,
    wordnode_T	*మూలం,
    పూర్ణాంకం		flags,
    పూర్ణాంకం		region,
    పూర్ణాంకం		affixID)
{
    wordnode_T	*కణుపు = మూలం;
    wordnode_T	*np;
    wordnode_T	*copyp, **copyprev;
    wordnode_T	**prev = ఖాళీ;
    పూర్ణాంకం		i;

    // Add each byte of the word to the tree, including the NUL at the end.
    కొరకు (i = 0; ; ++i)
    {
	// When there is more than one reference to this node we need to make
	// a copy, so that we can modify it.  Copy the whole list of siblings
	// (we don't optimize for a partly shared list of siblings).
	ఒకవేళ (కణుపు != ఖాళీ && కణుపు->wn_refs > 1)
	{
	    --కణుపు->wn_refs;
	    copyprev = prev;
	    FOR_ALL_కణుపు_SIBLINGS(కణుపు, copyp)
	    {
		// Allocate a new node and copy the info.
		np = get_wordnode(spin);
		ఒకవేళ (np == ఖాళీ)
		    తిరిగి FAIL;
		np->wn_బిడ్డ = copyp->wn_బిడ్డ;
		ఒకవేళ (np->wn_బిడ్డ != ఖాళీ)
		    ++np->wn_బిడ్డ->wn_refs;	// child gets extra ref
		np->wn_byte = copyp->wn_byte;
		ఒకవేళ (np->wn_byte == NUL)
		{
		    np->wn_flags = copyp->wn_flags;
		    np->wn_region = copyp->wn_region;
		    np->wn_affixID = copyp->wn_affixID;
		}

		// Link the new node in the list, there will be one ref.
		np->wn_refs = 1;
		ఒకవేళ (copyprev != ఖాళీ)
		    *copyprev = np;
		copyprev = &np->wn_sibling;

		// Let "node" point to the head of the copied list.
		ఒకవేళ (copyp == కణుపు)
		    కణుపు = np;
	    }
	}

	// Look for the sibling that has the same character.  They are sorted
	// on byte value, thus stop searching when a sibling is found with a
	// higher byte value.  For zero bytes (end of word) the sorting is
	// done on flags and then on affixID.
	అయితే (కణుపు != ఖాళీ
		&& (కణుపు->wn_byte < మాట[i]
		    || (కణుపు->wn_byte == NUL
			&& (flags < 0
			    ? కణుపు->wn_affixID < (గుర్తులేని)affixID
			    : (కణుపు->wn_flags < (గుర్తులేని)(flags & WN_ముసుగు)
				|| (కణుపు->wn_flags == (flags & WN_ముసుగు)
				    && (spin->si_sugtree
					? (కణుపు->wn_region & 0xffff) < region
					: కణుపు->wn_affixID
						    < (గుర్తులేని)affixID)))))))
	{
	    prev = &కణుపు->wn_sibling;
	    కణుపు = *prev;
	}
	ఒకవేళ (కణుపు == ఖాళీ
		|| కణుపు->wn_byte != మాట[i]
		|| (మాట[i] == NUL
		    && (flags < 0
			|| spin->si_sugtree
			|| కణుపు->wn_flags != (flags & WN_ముసుగు)
			|| కణుపు->wn_affixID != affixID)))
	{
	    // Allocate a new node.
	    np = get_wordnode(spin);
	    ఒకవేళ (np == ఖాళీ)
		తిరిగి FAIL;
	    np->wn_byte = మాట[i];

	    // If "node" is NULL this is a new child or the end of the sibling
	    // list: ref count is one.  Otherwise use ref count of sibling and
	    // make ref count of sibling one (matters when inserting in front
	    // of the list of siblings).
	    ఒకవేళ (కణుపు == ఖాళీ)
		np->wn_refs = 1;
	    లేకపోతే
	    {
		np->wn_refs = కణుపు->wn_refs;
		కణుపు->wn_refs = 1;
	    }
	    ఒకవేళ (prev != ఖాళీ)
		*prev = np;
	    np->wn_sibling = కణుపు;
	    కణుపు = np;
	}

	ఒకవేళ (మాట[i] == NUL)
	{
	    కణుపు->wn_flags = flags;
	    కణుపు->wn_region |= region;
	    కణుపు->wn_affixID = affixID;
	    ఆపు;
	}
	prev = &కణుపు->wn_బిడ్డ;
	కణుపు = *prev;
    }
#నిర్వచితమైతే SPELL_PRINTTREE
    smsg("Added \"%s\"", మాట);
    spell_print_వృక్షం(మూలం->wn_sibling);
#అయితే_ముగింపు

    // count nr of words added since last message
    ++spin->si_సందేశం_లెక్క;

    ఒకవేళ (spin->si_compress_cnt > 1)
    {
	ఒకవేళ (--spin->si_compress_cnt == 1)
	    // Did enough words to lower the block count limit.
	    spin->si_ఖండాలు_cnt += compress_inc;
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
#నిర్వచితం_కాకపోతే SPELL_COMPRESS_ALWAYS
    ఒకవేళ (spin->si_compress_cnt == 1
	    ? spin->si_free_లెక్క < MAXWLEN
	    : spin->si_ఖండాలు_cnt >= compress_ప్రారంభం)
#అయితే_ముగింపు
    {
	// Decrement the block counter.  The effect is that we compress again
	// when the freed up room has been used and another "compress_inc"
	// blocks have been allocated.  Unless "compress_added" words have
	// been added, then the limit is put back again.
	spin->si_ఖండాలు_cnt -= compress_inc;
	spin->si_compress_cnt = compress_added;

	ఒకవేళ (spin->si_verbose)
	{
	    సందేశం_ప్రారంభం();
	    సందేశం_puts(_(సందేశం_compressing));
	    సందేశం_clr_eos();
	    సందేశం_didout = FALSE;
	    సందేశం_నిలువు = 0;
	    బహిర్గతం_flush();
	}

	// Compress both trees.  Either they both have many nodes, which makes
	// compression useful, or one of them is small, which means
	// compression goes fast.  But when filling the soundfold word tree
	// there is no keep-case tree.
	wordtree_compress(spin, spin->si_foldroot, "case-folded");
	ఒకవేళ (affixID >= 0)
	    wordtree_compress(spin, spin->si_keeproot, "keep-case");
    }

    తిరిగి OK;
}

/*
 * Get a wordnode_T, either from the list of previously freed nodes or
 * allocate a new one.
 * Returns NULL when out of memory.
 */
    స్థిర wordnode_T *
get_wordnode(spellinfo_T *spin)
{
    wordnode_T *n;

    ఒకవేళ (spin->si_మొదటి_free == ఖాళీ)
	n = (wordnode_T *)getroom(spin, పరిమాణం(wordnode_T), TRUE);
    లేకపోతే
    {
	n = spin->si_మొదటి_free;
	spin->si_మొదటి_free = n->wn_బిడ్డ;
	CLEAR_సూచిక(n);
	--spin->si_free_లెక్క;
    }
#నిర్వచితమైతే SPELL_PRINTTREE
    ఒకవేళ (n != ఖాళీ)
	n->wn_nr = ++spin->si_wordnode_nr;
#అయితే_ముగింపు
    తిరిగి n;
}

/*
 * Decrement the reference count on a node (which is the head of a list of
 * siblings).  If the reference count becomes zero free the node and its
 * siblings.
 * Returns the number of nodes actually freed.
 */
    స్థిర పూర్ణాంకం
deref_wordnode(spellinfo_T *spin, wordnode_T *కణుపు)
{
    wordnode_T	*np;
    పూర్ణాంకం		cnt = 0;

    ఒకవేళ (--కణుపు->wn_refs == 0)
    {
	FOR_ALL_కణుపు_SIBLINGS(కణుపు, np)
	{
	    ఒకవేళ (np->wn_బిడ్డ != ఖాళీ)
		cnt += deref_wordnode(spin, np->wn_బిడ్డ);
	    free_wordnode(spin, np);
	    ++cnt;
	}
	++cnt;	    // length field
    }
    తిరిగి cnt;
}

/*
 * Free a wordnode_T for re-use later.
 * Only the "wn_child" field becomes invalid.
 */
    స్థిర శూన్య
free_wordnode(spellinfo_T *spin, wordnode_T *n)
{
    n->wn_బిడ్డ = spin->si_మొదటి_free;
    spin->si_మొదటి_free = n;
    ++spin->si_free_లెక్క;
}

/*
 * Compress a tree: find tails that are identical and can be shared.
 */
    స్థిర శూన్య
wordtree_compress(spellinfo_T *spin, wordnode_T *మూలం, అక్షరం *name)
{
    hashtab_T	    ht;
    దీర్ఘ	    n;
    దీర్ఘ	    tot = 0;
    దీర్ఘ	    perc;

    // Skip the root itself, it's not actually used.  The first sibling is the
    // start of the tree.
    ఒకవేళ (మూలం->wn_sibling == ఖాళీ)
	తిరిగి;

    hash_init(&ht);
    n = కణుపు_compress(spin, మూలం->wn_sibling, &ht, &tot);

#నిర్వచితం_కాకపోతే SPELL_PRINTTREE
    ఒకవేళ (spin->si_verbose || p_verbose > 2)
#అయితే_ముగింపు
    {
	ఒకవేళ (tot > 1000000)
	    perc = (tot - n) / (tot / 100);
	లేకపోతే ఒకవేళ (tot == 0)
	    perc = 0;
	లేకపోతే
	    perc = (tot - n) * 100 / tot;
	vim_snprintf((అక్షరం *)IObuff, IOSIZE,
		_("Compressed %s: %ld of %ld nodes; %ld (%ld%%) remaining"),
		name, n, tot, tot - n, perc);
	spell_సందేశం(spin, IObuff);
    }
#నిర్వచితమైతే SPELL_PRINTTREE
    spell_print_వృక్షం(మూలం->wn_sibling);
#అయితే_ముగింపు
    hash_clear(&ht);
}

/*
 * Compress a node, its siblings and its children, depth first.
 * Returns the number of compressed nodes.
 */
    స్థిర దీర్ఘ
కణుపు_compress(
    spellinfo_T	*spin,
    wordnode_T	*కణుపు,
    hashtab_T	*ht,
    దీర్ఘ	*tot)	    // total count of nodes before compressing,
			    // incremented while going through the tree
{
    wordnode_T	*np;
    wordnode_T	*tp;
    wordnode_T	*బిడ్డ;
    hash_T	hash;
    hashitem_T	*hi;
    దీర్ఘ	len = 0;
    గుర్తులేని	nr, n;
    దీర్ఘ	compressed = 0;

    /*
     * Go through the list of siblings.  Compress each child and then try
     * finding an identical child to replace it.
     * Note that with "child" we mean not just the node that is pointed to,
     * but the whole list of siblings of which the child node is the first.
     */
    కొరకు (np = కణుపు; np != ఖాళీ && !got_int; np = np->wn_sibling)
    {
	++len;
	ఒకవేళ ((బిడ్డ = np->wn_బిడ్డ) != ఖాళీ)
	{
	    // Compress the child first.  This fills hashkey.
	    compressed += కణుపు_compress(spin, బిడ్డ, ht, tot);

	    // Try to find an identical child.
	    hash = hash_hash(బిడ్డ->wn_u1.hashkey);
	    hi = hash_lookup(ht, బిడ్డ->wn_u1.hashkey, hash);
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	    {
		// There are children we encountered before with a hash value
		// identical to the current child.  Now check if there is one
		// that is really identical.
		కొరకు (tp = HI2WN(hi); tp != ఖాళీ; tp = tp->wn_u2.తదుపరి)
		    ఒకవేళ (కణుపు_equal(బిడ్డ, tp))
		    {
			// Found one!  Now use that child in place of the
			// current one.  This means the current child and all
			// its siblings is unlinked from the tree.
			++tp->wn_refs;
			compressed += deref_wordnode(spin, బిడ్డ);
			np->wn_బిడ్డ = tp;
			ఆపు;
		    }
		ఒకవేళ (tp == ఖాళీ)
		{
		    // No other child with this hash value equals the child of
		    // the node, add it to the linked list after the first
		    // item.
		    tp = HI2WN(hi);
		    బిడ్డ->wn_u2.తదుపరి = tp->wn_u2.తదుపరి;
		    tp->wn_u2.తదుపరి = బిడ్డ;
		}
	    }
	    లేకపోతే
		// No other child has this hash value, add it to the
		// hashtable.
		hash_కూడు_అంశం(ht, hi, బిడ్డ->wn_u1.hashkey, hash);
	}
    }
    *tot += len + 1;	// add one for the node that stores the length

    /*
     * Make a hash key for the node and its siblings, so that we can quickly
     * find a lookalike node.  This must be done after compressing the sibling
     * list, otherwise the hash key would become invalid by the compression.
     */
    కణుపు->wn_u1.hashkey[0] = len;
    nr = 0;
    FOR_ALL_కణుపు_SIBLINGS(కణుపు, np)
    {
	ఒకవేళ (np->wn_byte == NUL)
	    // end node: use wn_flags, wn_region and wn_affixID
	    n = np->wn_flags + (np->wn_region << 8) + (np->wn_affixID << 16);
	లేకపోతే
	    // byte node: use the byte value and the child pointer
	    n = (గుర్తులేని)(np->wn_byte + ((long_u)np->wn_బిడ్డ << 8));
	nr = nr * 101 + n;
    }

    // Avoid NUL bytes, it terminates the hash key.
    n = nr & 0xff;
    కణుపు->wn_u1.hashkey[1] = n == 0 ? 1 : n;
    n = (nr >> 8) & 0xff;
    కణుపు->wn_u1.hashkey[2] = n == 0 ? 1 : n;
    n = (nr >> 16) & 0xff;
    కణుపు->wn_u1.hashkey[3] = n == 0 ? 1 : n;
    n = (nr >> 24) & 0xff;
    కణుపు->wn_u1.hashkey[4] = n == 0 ? 1 : n;
    కణుపు->wn_u1.hashkey[5] = NUL;

    // Check for CTRL-C pressed now and then.
    veryfast_breakcheck();

    తిరిగి compressed;
}

/*
 * Return TRUE when two nodes have identical siblings and children.
 */
    స్థిర పూర్ణాంకం
కణుపు_equal(wordnode_T *n1, wordnode_T *n2)
{
    wordnode_T	*p1;
    wordnode_T	*p2;

    కొరకు (p1 = n1, p2 = n2; p1 != ఖాళీ && p2 != ఖాళీ;
				     p1 = p1->wn_sibling, p2 = p2->wn_sibling)
	ఒకవేళ (p1->wn_byte != p2->wn_byte
		|| (p1->wn_byte == NUL
		    ? (p1->wn_flags != p2->wn_flags
			|| p1->wn_region != p2->wn_region
			|| p1->wn_affixID != p2->wn_affixID)
		    : (p1->wn_బిడ్డ != p2->wn_బిడ్డ)))
	    ఆపు;

    తిరిగి p1 == ఖాళీ && p2 == ఖాళీ;
}

స్థిర పూర్ణాంకం rep_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2);

/*
 * Function given to qsort() to sort the REP items on "from" string.
 */
    స్థిర పూర్ణాంకం
rep_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    fromto_T	*p1 = (fromto_T *)s1;
    fromto_T	*p2 = (fromto_T *)s2;

    తిరిగి STRCMP(p1->ft_from, p2->ft_from);
}

/*
 * Write the Vim .spl file "fname".
 * Return FAIL or OK;
 */
    స్థిర పూర్ణాంకం
write_vim_spell(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    పూర్ణాంకం		regionmask;
    పూర్ణాంకం		విడత;
    wordnode_T	*వృక్షం;
    పూర్ణాంకం		nodecount;
    పూర్ణాంకం		i;
    పూర్ణాంకం		l;
    garray_T	*gap;
    fromto_T	*ftp;
    char_u	*p;
    పూర్ణాంకం		rr;
    పూర్ణాంకం		retval = OK;
    size_t	fwv = 1;  // collect return value of fwrite() to avoid
			  // warnings from picky compiler

    fd = mch_fopen((అక్షరం *)fname, "w");
    ఒకవేళ (fd == ఖాళీ)
    {
	semsg(_(e_cant_open_file_str), fname);
	తిరిగి FAIL;
    }

    // <HEADER>: <fileID> <versionnr>
							    // <fileID>
    fwv &= fwrite(VIMSPELLMAGIC, VIMSPELLMAGICL, (size_t)1, fd);
    ఒకవేళ (fwv != (size_t)1)
	// Catch first write error, don't try writing more.
	వెళ్ళు theend;

    putc(VIMSPELLVERSION, fd);				    // <versionnr>

    /*
     * <SECTIONS>: <section> ... <sectionend>
     */

    // SN_INFO: <infotext>
    ఒకవేళ (spin->si_info != ఖాళీ)
    {
	putc(SN_INFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	i = (పూర్ణాంకం)STRLEN(spin->si_info);
	put_బైట్లు(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_info, (size_t)i, (size_t)1, fd); // <infotext>
    }

    // SN_REGION: <regionname> ...
    // Write the region names only if there is more than one.
    ఒకవేళ (spin->si_region_లెక్క > 1)
    {
	putc(SN_REGION, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>
	l = spin->si_region_లెక్క * 2;
	put_బైట్లు(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_region_name, (size_t)l, (size_t)1, fd);
							// <regionname> ...
	regionmask = (1 << spin->si_region_లెక్క) - 1;
    }
    లేకపోతే
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
    ఒకవేళ (!spin->si_ascii && !spin->si_కూడు)
    {
	char_u	folchars[128 * 8];
	పూర్ణాంకం	flags;

	putc(SN_CHARFLAGS, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	// Form the <folchars> string first, we need to know its length.
	l = 0;
	కొరకు (i = 128; i < 256; ++i)
	{
	    ఒకవేళ (has_mbyte)
		l += mb_char2bytes(spelltab.st_fold[i], folchars + l);
	    లేకపోతే
		folchars[l++] = spelltab.st_fold[i];
	}
	put_బైట్లు(fd, (long_u)(1 + 128 + 2 + l), 4);	// <sectionlen>

	fputc(128, fd);					// <charflagslen>
	కొరకు (i = 128; i < 256; ++i)
	{
	    flags = 0;
	    ఒకవేళ (spelltab.st_isw[i])
		flags |= CF_మాట;
	    ఒకవేళ (spelltab.st_isu[i])
		flags |= CF_పై;
	    fputc(flags, fd);				// <charflags>
	}

	put_బైట్లు(fd, (long_u)l, 2);			// <folcharslen>
	fwv &= fwrite(folchars, (size_t)l, (size_t)1, fd); // <folchars>
    }

    // SN_MIDWORD: <midword>
    ఒకవేళ (spin->si_midword != ఖాళీ)
    {
	putc(SN_MIDWORD, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	i = (పూర్ణాంకం)STRLEN(spin->si_midword);
	put_బైట్లు(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_midword, (size_t)i, (size_t)1, fd);
							// <midword>
    }

    // SN_PREFCOND: <prefcondcnt> <prefcond> ...
    ఒకవేళ (spin->si_prefcond.ga_len > 0)
    {
	putc(SN_PREFCOND, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	l = write_spell_prefcond(ఖాళీ, &spin->si_prefcond, &fwv);
	put_బైట్లు(fd, (long_u)l, 4);			// <sectionlen>

	write_spell_prefcond(fd, &spin->si_prefcond, &fwv);
    }

    // SN_REP: <repcount> <rep> ...
    // SN_SAL: <salflags> <salcount> <sal> ...
    // SN_REPSAL: <repcount> <rep> ...

    // round 1: SN_REP section
    // round 2: SN_SAL section (unless SN_SOFO is used)
    // round 3: SN_REPSAL section
    కొరకు (విడత = 1; విడత <= 3; ++విడత)
    {
	ఒకవేళ (విడత == 1)
	    gap = &spin->si_rep;
	లేకపోతే ఒకవేళ (విడత == 2)
	{
	    // Don't write SN_SAL when using a SN_SOFO section
	    ఒకవేళ (spin->si_sofofr != ఖాళీ && spin->si_sofoto != ఖాళీ)
		కొనసాగు;
	    gap = &spin->si_sal;
	}
	లేకపోతే
	    gap = &spin->si_repsal;

	// Don't write the section if there are no items.
	ఒకవేళ (gap->ga_len == 0)
	    కొనసాగు;

	// Sort the REP/REPSAL items.
	ఒకవేళ (విడత != 2)
	    qsort(gap->ga_దత్తాంశం, (size_t)gap->ga_len,
					       పరిమాణం(fromto_T), rep_compare);

	i = విడత == 1 ? SN_REP : (విడత == 2 ? SN_SAL : SN_REPSAL);
	putc(i, fd);					// <sectionID>

	// This is for making suggestions, section is not required.
	putc(0, fd);					// <sectionflags>

	// Compute the length of what follows.
	l = 2;	    // count <repcount> or <salcount>
	కొరకు (i = 0; i < gap->ga_len; ++i)
	{
	    ftp = &((fromto_T *)gap->ga_దత్తాంశం)[i];
	    l += 1 + (పూర్ణాంకం)STRLEN(ftp->ft_from);  // count <*fromlen> and <*from>
	    l += 1 + (పూర్ణాంకం)STRLEN(ftp->ft_గమ్యం);    // count <*tolen> and <*to>
	}
	ఒకవేళ (విడత == 2)
	    ++l;	// count <salflags>
	put_బైట్లు(fd, (long_u)l, 4);			// <sectionlen>

	ఒకవేళ (విడత == 2)
	{
	    i = 0;
	    ఒకవేళ (spin->si_followup)
		i |= SAL_F0LLOWUP;
	    ఒకవేళ (spin->si_collapse)
		i |= SAL_COLLAPSE;
	    ఒకవేళ (spin->si_rem_accents)
		i |= SAL_REM_ACCENTS;
	    putc(i, fd);			// <salflags>
	}

	put_బైట్లు(fd, (long_u)gap->ga_len, 2);	// <repcount> or <salcount>
	కొరకు (i = 0; i < gap->ga_len; ++i)
	{
	    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
	    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
	    ftp = &((fromto_T *)gap->ga_దత్తాంశం)[i];
	    కొరకు (rr = 1; rr <= 2; ++rr)
	    {
		p = rr == 1 ? ftp->ft_from : ftp->ft_గమ్యం;
		l = (పూర్ణాంకం)STRLEN(p);
		putc(l, fd);
		ఒకవేళ (l > 0)
		    fwv &= fwrite(p, l, (size_t)1, fd);
	    }
	}

    }

    // SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
    // This is for making suggestions, section is not required.
    ఒకవేళ (spin->si_sofofr != ఖాళీ && spin->si_sofoto != ఖాళీ)
    {
	putc(SN_SOFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (పూర్ణాంకం)STRLEN(spin->si_sofofr);
	put_బైట్లు(fd, (long_u)(l + STRLEN(spin->si_sofoto) + 4), 4);
							// <sectionlen>

	put_బైట్లు(fd, (long_u)l, 2);			// <sofofromlen>
	fwv &= fwrite(spin->si_sofofr, l, (size_t)1, fd); // <sofofrom>

	l = (పూర్ణాంకం)STRLEN(spin->si_sofoto);
	put_బైట్లు(fd, (long_u)l, 2);			// <sofotolen>
	fwv &= fwrite(spin->si_sofoto, l, (size_t)1, fd); // <sofoto>
    }

    // SN_WORDS: <word> ...
    // This is for making suggestions, section is not required.
    ఒకవేళ (spin->si_commonwords.ht_వాడిన > 0)
    {
	putc(SN_పదాలు, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// round 1: count the bytes
	// round 2: write the bytes
	కొరకు (విడత = 1; విడత <= 2; ++విడత)
	{
	    పూర్ణాంకం		todo;
	    పూర్ణాంకం		len = 0;
	    hashitem_T	*hi;

	    todo = (పూర్ణాంకం)spin->si_commonwords.ht_వాడిన;
	    FOR_ALL_HASHTAB_వస్తువులు(&spin->si_commonwords, hi, todo)
		ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		{
		    l = (పూర్ణాంకం)STRLEN(hi->hi_కీలకం) + 1;
		    len += l;
		    ఒకవేళ (విడత == 2)			// <word>
			fwv &= fwrite(hi->hi_కీలకం, (size_t)l, (size_t)1, fd);
		    --todo;
		}
	    ఒకవేళ (విడత == 1)
		put_బైట్లు(fd, (long_u)len, 4);		// <sectionlen>
	}
    }

    // SN_MAP: <mapstr>
    // This is for making suggestions, section is not required.
    ఒకవేళ (spin->si_map.ga_len > 0)
    {
	putc(SN_MAP, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>
	l = spin->si_map.ga_len;
	put_బైట్లు(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_map.ga_దత్తాంశం, (size_t)l, (size_t)1, fd);
							// <mapstr>
    }

    // SN_SUGFILE: <timestamp>
    // This is used to notify that a .sug file may be available and at the
    // same time allows for checking that a .sug file that is found matches
    // with this .spl file.  That's because the word numbers must be exactly
    // right.
    ఒకవేళ (!spin->si_nosugfile
	    && (spin->si_sal.ga_len > 0
		     || (spin->si_sofofr != ఖాళీ && spin->si_sofoto != ఖాళీ)))
    {
	putc(SN_SUGFILE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_బైట్లు(fd, (long_u)8, 4);			// <sectionlen>

	// Set si_sugtime and write it to the file.
	spin->si_sugtime = సమయం(ఖాళీ);
	put_సమయం(fd, spin->si_sugtime);			// <timestamp>
    }

    // SN_NOSPLITSUGS: nothing
    // This is used to notify that no suggestions with word splits are to be
    // made.
    ఒకవేళ (spin->si_nosplitsugs)
    {
	putc(SN_NOSPLITSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_బైట్లు(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_NOCOMPUNDSUGS: nothing
    // This is used to notify that no suggestions with compounds are to be
    // made.
    ఒకవేళ (spin->si_nocompoundsugs)
    {
	putc(SN_NOCOMPOUNDSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_బైట్లు(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_COMPOUND: compound info.
    // We don't mark it required, when not supported all compound words will
    // be bad words.
    ఒకవేళ (spin->si_compflags != ఖాళీ)
    {
	putc(SN_COMPOUND, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (పూర్ణాంకం)STRLEN(spin->si_compflags);
	కొరకు (i = 0; i < spin->si_comppat.ga_len; ++i)
	    l += (పూర్ణాంకం)STRLEN(((char_u **)(spin->si_comppat.ga_దత్తాంశం))[i]) + 1;
	put_బైట్లు(fd, (long_u)(l + 7), 4);		// <sectionlen>

	putc(spin->si_compmax, fd);			// <compmax>
	putc(spin->si_compminlen, fd);			// <compminlen>
	putc(spin->si_compsylmax, fd);			// <compsylmax>
	putc(0, fd);		// for Vim 7.0b compatibility
	putc(spin->si_compoptions, fd);			// <compoptions>
	put_బైట్లు(fd, (long_u)spin->si_comppat.ga_len, 2);
							// <comppatcount>
	కొరకు (i = 0; i < spin->si_comppat.ga_len; ++i)
	{
	    p = ((char_u **)(spin->si_comppat.ga_దత్తాంశం))[i];
	    putc((పూర్ణాంకం)STRLEN(p), fd);			// <comppatlen>
	    fwv &= fwrite(p, (size_t)STRLEN(p), (size_t)1, fd);
							// <comppattext>
	}
							// <compflags>
	fwv &= fwrite(spin->si_compflags, (size_t)STRLEN(spin->si_compflags),
							       (size_t)1, fd);
    }

    // SN_NOBREAK: NOBREAK flag
    ఒకవేళ (spin->si_nobreak)
    {
	putc(SN_NOBREAK, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// It's empty, the presence of the section flags the feature.
	put_బైట్లు(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_SYLLABLE: syllable info.
    // We don't mark it required, when not supported syllables will not be
    // counted.
    ఒకవేళ (spin->si_syllable != ఖాళీ)
    {
	putc(SN_SYLLABLE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (పూర్ణాంకం)STRLEN(spin->si_syllable);
	put_బైట్లు(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_syllable, (size_t)l, (size_t)1, fd);
							// <syllable>
    }

    // end of <SECTIONS>
    putc(SN_చివర, fd);					// <sectionend>


    /*
     * <LWORDTREE>  <KWORDTREE>  <PREFIXTREE>
     */
    spin->si_memtot = 0;
    కొరకు (విడత = 1; విడత <= 3; ++విడత)
    {
	ఒకవేళ (విడత == 1)
	    వృక్షం = spin->si_foldroot->wn_sibling;
	లేకపోతే ఒకవేళ (విడత == 2)
	    వృక్షం = spin->si_keeproot->wn_sibling;
	లేకపోతే
	    వృక్షం = spin->si_prefroot->wn_sibling;

	// Clear the index and wnode fields in the tree.
	clear_కణుపు(వృక్షం);

	// Count the number of nodes.  Needed to be able to allocate the
	// memory when reading the nodes.  Also fills in index for shared
	// nodes.
	nodecount = put_కణుపు(ఖాళీ, వృక్షం, 0, regionmask, విడత == 3);

	// number of nodes in 4 bytes
	put_బైట్లు(fd, (long_u)nodecount, 4);	// <nodecount>
	spin->si_memtot += nodecount + nodecount * పరిమాణం(పూర్ణాంకం);

	// Write the nodes.
	(శూన్య)put_కణుపు(fd, వృక్షం, 0, regionmask, విడత == 3);
    }

    // Write another byte to check for errors (file system full).
    ఒకవేళ (putc(0, fd) == EOF)
	retval = FAIL;
theend:
    ఒకవేళ (fclose(fd) == EOF)
	retval = FAIL;

    ఒకవేళ (fwv != (size_t)1)
	retval = FAIL;
    ఒకవేళ (retval == FAIL)
	emsg(_(e_error_while_writing));

    తిరిగి retval;
}

/*
 * Clear the index and wnode fields of "node", it siblings and its
 * children.  This is needed because they are a union with other items to save
 * space.
 */
    స్థిర శూన్య
clear_కణుపు(wordnode_T *కణుపు)
{
    wordnode_T	*np;

    ఒకవేళ (కణుపు != ఖాళీ)
	FOR_ALL_కణుపు_SIBLINGS(కణుపు, np)
	{
	    np->wn_u1.సూచీ = 0;
	    np->wn_u2.wnode = ఖాళీ;

	    ఒకవేళ (np->wn_byte != NUL)
		clear_కణుపు(np->wn_బిడ్డ);
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
    స్థిర పూర్ణాంకం
put_కణుపు(
    FILE	*fd,		// NULL when only counting
    wordnode_T	*కణుపు,
    పూర్ణాంకం		idx,
    పూర్ణాంకం		regionmask,
    పూర్ణాంకం		prefixtree)	// TRUE for PREFIXTREE
{
    పూర్ణాంకం		newindex = idx;
    పూర్ణాంకం		siblingcount = 0;
    wordnode_T	*np;
    పూర్ణాంకం		flags;

    // If "node" is zero the tree is empty.
    ఒకవేళ (కణుపు == ఖాళీ)
	తిరిగి 0;

    // Store the index where this node is written.
    కణుపు->wn_u1.సూచీ = idx;

    // Count the number of siblings.
    FOR_ALL_కణుపు_SIBLINGS(కణుపు, np)
	++siblingcount;

    // Write the sibling count.
    ఒకవేళ (fd != ఖాళీ)
	putc(siblingcount, fd);				// <siblingcount>

    // Write each sibling byte and optionally extra info.
    FOR_ALL_కణుపు_SIBLINGS(కణుపు, np)
    {
	ఒకవేళ (np->wn_byte == 0)
	{
	    ఒకవేళ (fd != ఖాళీ)
	    {
		// For a NUL byte (end of word) write the flags etc.
		ఒకవేళ (prefixtree)
		{
		    // In PREFIXTREE write the required affixID and the
		    // associated condition nr (stored in wn_region).  The
		    // byte value is misused to store the "rare" and "not
		    // combining" flags
		    ఒకవేళ (np->wn_flags == (short_u)PFX_FLAGS)
			putc(BY_NOFLAGS, fd);		// <byte>
		    లేకపోతే
		    {
			putc(BY_FLAGS, fd);		// <byte>
			putc(np->wn_flags, fd);		// <pflags>
		    }
		    putc(np->wn_affixID, fd);		// <affixID>
		    put_బైట్లు(fd, (long_u)np->wn_region, 2); // <prefcondnr>
		}
		లేకపోతే
		{
		    // For word trees we write the flag/region items.
		    flags = np->wn_flags;
		    ఒకవేళ (regionmask != 0 && np->wn_region != regionmask)
			flags |= WF_REGION;
		    ఒకవేళ (np->wn_affixID != 0)
			flags |= WF_AFX;
		    ఒకవేళ (flags == 0)
		    {
			// word without flags or region
			putc(BY_NOFLAGS, fd);			// <byte>
		    }
		    లేకపోతే
		    {
			ఒకవేళ (np->wn_flags >= 0x100)
			{
			    putc(BY_FLAGS2, fd);		// <byte>
			    putc(flags, fd);			// <flags>
			    putc((గుర్తులేని)flags >> 8, fd);	// <flags2>
			}
			లేకపోతే
			{
			    putc(BY_FLAGS, fd);			// <byte>
			    putc(flags, fd);			// <flags>
			}
			ఒకవేళ (flags & WF_REGION)
			    putc(np->wn_region, fd);		// <region>
			ఒకవేళ (flags & WF_AFX)
			    putc(np->wn_affixID, fd);		// <affixID>
		    }
		}
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ (np->wn_బిడ్డ->wn_u1.సూచీ != 0
					 && np->wn_బిడ్డ->wn_u2.wnode != కణుపు)
	    {
		// The child is written elsewhere, write the reference.
		ఒకవేళ (fd != ఖాళీ)
		{
		    putc(BY_సూచీ, fd);			// <byte>
							// <nodeidx>
		    put_బైట్లు(fd, (long_u)np->wn_బిడ్డ->wn_u1.సూచీ, 3);
		}
	    }
	    లేకపోతే ఒకవేళ (np->wn_బిడ్డ->wn_u2.wnode == ఖాళీ)
		// We will write the child below and give it an index.
		np->wn_బిడ్డ->wn_u2.wnode = కణుపు;

	    ఒకవేళ (fd != ఖాళీ)
		ఒకవేళ (putc(np->wn_byte, fd) == EOF) // <byte> or <xbyte>
		{
		    emsg(_(e_error_while_writing));
		    తిరిగి 0;
		}
	}
    }

    // Space used in the array when reading: one for each sibling and one for
    // the count.
    newindex += siblingcount + 1;

    // Recursively dump the children of each sibling.
    FOR_ALL_కణుపు_SIBLINGS(కణుపు, np)
	ఒకవేళ (np->wn_byte != 0 && np->wn_బిడ్డ->wn_u2.wnode == కణుపు)
	    newindex = put_కణుపు(fd, np->wn_బిడ్డ, newindex, regionmask,
								  prefixtree);

    తిరిగి newindex;
}


/*
 * ":mkspell [-ascii] outfile  infile ..."
 * ":mkspell [-ascii] addfile"
 */
    శూన్య
ex_mkspell(exarg_T *eap)
{
    పూర్ణాంకం		fcount;
    char_u	**fnames;
    char_u	*arg = eap->arg;
    పూర్ణాంకం		ascii = FALSE;

    ఒకవేళ (STRNCMP(arg, "-ascii", 6) == 0)
    {
	ascii = TRUE;
	arg = skipwhite(arg + 6);
    }

    // Expand all the remaining arguments (e.g., $VIMRUNTIME).
    ఒకవేళ (get_arglist_exp(arg, &fcount, &fnames, FALSE) != OK)
	తిరిగి;

    mkspell(fcount, fnames, ascii, eap->forceit, FALSE);
    FreeWild(fcount, fnames);
}

/*
 * Create the .sug file.
 * Uses the soundfold info in "spin".
 * Writes the file with the name "wfname", with ".spl" changed to ".sug".
 */
    స్థిర శూన్య
spell_make_sugfile(spellinfo_T *spin, char_u *wfname)
{
    char_u	*fname = ఖాళీ;
    పూర్ణాంకం		len;
    slang_T	*slang;
    పూర్ణాంకం		free_slang = FALSE;

    /*
     * Read back the .spl file that was written.  This fills the required
     * info for soundfolding.  This also uses less memory than the
     * pointer-linked version of the trie.  And it avoids having two versions
     * of the code for the soundfolding stuff.
     * It might have been done already by spell_reload_one().
     */
    FOR_ALL_SPELL_LANGS(slang)
	ఒకవేళ (fullpathcmp(wfname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	    ఆపు;
    ఒకవేళ (slang == ఖాళీ)
    {
	spell_సందేశం(spin, (char_u *)_("Reading back spell file..."));
	slang = spell_ఎక్కించు_file(wfname, ఖాళీ, ఖాళీ, FALSE);
	ఒకవేళ (slang == ఖాళీ)
	    తిరిగి;
	free_slang = TRUE;
    }

    /*
     * Clear the info in "spin" that is used.
     */
    spin->si_ఖండాలు = ఖాళీ;
    spin->si_ఖండాలు_cnt = 0;
    spin->si_compress_cnt = 0;	    // will stay at 0 all the time
    spin->si_free_లెక్క = 0;
    spin->si_మొదటి_free = ఖాళీ;
    spin->si_foldwcount = 0;

    /*
     * Go through the trie of good words, soundfold each word and add it to
     * the soundfold trie.
     */
    spell_సందేశం(spin, (char_u *)_("Performing soundfolding..."));
    ఒకవేళ (sug_filltree(spin, slang) == FAIL)
	వెళ్ళు theend;

    /*
     * Create the table which links each soundfold word with a list of the
     * good words it may come from.  Creates buffer "spin->si_spellbuf".
     * This also removes the wordnr from the NUL byte entries to make
     * compression possible.
     */
    ఒకవేళ (sug_maketable(spin) == FAIL)
	వెళ్ళు theend;

    smsg(_("Number of words after soundfolding: %ld"),
				 (దీర్ఘ)spin->si_spellbuf->b_ml.ml_పంక్తి_లెక్క);

    /*
     * Compress the soundfold trie.
     */
    spell_సందేశం(spin, (char_u *)_(సందేశం_compressing));
    wordtree_compress(spin, spin->si_foldroot, "case-folded");

    /*
     * Write the .sug file.
     * Make the file name by changing ".spl" to ".sug".
     */
    fname = alloc(MAXPATHL);
    ఒకవేళ (fname == ఖాళీ)
	వెళ్ళు theend;
    vim_strncpy(fname, wfname, MAXPATHL - 1);
    len = (పూర్ణాంకం)STRLEN(fname);
    fname[len - 2] = 'u';
    fname[len - 1] = 'g';
    sug_write(spin, fname);

theend:
    vim_free(fname);
    ఒకవేళ (free_slang)
	slang_free(slang);
    free_ఖండాలు(spin->si_ఖండాలు);
    close_spellbuf(spin->si_spellbuf);
}

/*
 * Build the soundfold trie for language "slang".
 */
    స్థిర పూర్ణాంకం
sug_filltree(spellinfo_T *spin, slang_T *slang)
{
    char_u	*byts;
    idx_T	*idxs;
    పూర్ణాంకం		లోతు;
    idx_T	arridx[MAXWLEN];
    పూర్ణాంకం		curi[MAXWLEN];
    char_u	tword[MAXWLEN];
    char_u	tsalword[MAXWLEN];
    పూర్ణాంకం		c;
    idx_T	n;
    గుర్తులేని	పదాలు_పూర్తి = 0;
    పూర్ణాంకం		wordcount[MAXWLEN];

    // We use si_foldroot for the soundfolded trie.
    spin->si_foldroot = wordtree_alloc(spin);
    ఒకవేళ (spin->si_foldroot == ఖాళీ)
	తిరిగి FAIL;

    // let tree_add_word() know we're adding to the soundfolded tree
    spin->si_sugtree = TRUE;

    /*
     * Go through the whole case-folded tree, soundfold each word and put it
     * in the trie.  Bail out if the tree is empty.
     */
    byts = slang->sl_fbyts;
    idxs = slang->sl_fidxs;
    ఒకవేళ (byts == ఖాళీ || idxs == ఖాళీ)
	తిరిగి FAIL;

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;

    లోతు = 0;
    అయితే (లోతు >= 0 && !got_int)
    {
	ఒకవేళ (curi[లోతు] > byts[arridx[లోతు]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[లోతు]] = wordcount[లోతు];
	    ఒకవేళ (లోతు > 0)
		wordcount[లోతు - 1] += wordcount[లోతు];

	    --లోతు;
	    పంక్తి_breakcheck();
	}
	లేకపోతే
	{

	    // Do one more byte at this node.
	    n = arridx[లోతు] + curi[లోతు];
	    ++curi[లోతు];

	    c = byts[n];
	    ఒకవేళ (c == 0)
	    {
		// Sound-fold the word.
		tword[లోతు] = NUL;
		spell_soundfold(slang, tword, TRUE, tsalword);

		// We use the "flags" field for the MSB of the wordnr,
		// "region" for the LSB of the wordnr.
		ఒకవేళ (వృక్షం_కూడు_మాట(spin, tsalword, spin->si_foldroot,
				పదాలు_పూర్తి >> 16, పదాలు_పూర్తి & 0xffff,
							   0) == FAIL)
		    తిరిగి FAIL;

		++పదాలు_పూర్తి;
		++wordcount[లోతు];

		// Reset the block count each time to avoid compression
		// kicking in.
		spin->si_ఖండాలు_cnt = 0;

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		అయితే (n + 1 < slang->sl_fbyts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[లోతు];
		}
	    }
	    లేకపోతే ఒకవేళ (లోతు < MAXWLEN - 1)
	    {
		// Normal char, go one level deeper.
		tword[లోతు++] = c;
		arridx[లోతు] = idxs[n];
		curi[లోతు] = 1;
		wordcount[లోతు] = 0;
	    }
	}
    }

    smsg(_("Total number of words: %d"), పదాలు_పూర్తి);

    తిరిగి OK;
}

/*
 * Make the table that links each word in the soundfold trie to the words it
 * can be produced from.
 * This is not unlike lines in a file, thus use a memfile to be able to access
 * the table efficiently.
 * Returns FAIL when out of memory.
 */
    స్థిర పూర్ణాంకం
sug_maketable(spellinfo_T *spin)
{
    garray_T	ga;
    పూర్ణాంకం		res = OK;

    // Allocate a buffer, open a memline for it and create the swap file
    // (uses a temp file, not a .swp file).
    spin->si_spellbuf = open_spellbuf();
    ఒకవేళ (spin->si_spellbuf == ఖాళీ)
	తిరిగి FAIL;

    // Use a buffer to store the line info, avoids allocating many small
    // pieces of memory.
    ga_init2(&ga, 1, 100);

    // recursively go through the tree
    ఒకవేళ (sug_filltable(spin, spin->si_foldroot->wn_sibling, 0, &ga) == -1)
	res = FAIL;

    ga_clear(&ga);
    తిరిగి res;
}

/*
 * Fill the table for one node and its children.
 * Returns the wordnr at the start of the node.
 * Returns -1 when out of memory.
 */
    స్థిర పూర్ణాంకం
sug_filltable(
    spellinfo_T	*spin,
    wordnode_T	*కణుపు,
    పూర్ణాంకం		startwordnr,
    garray_T	*gap)	    // place to store line of numbers
{
    wordnode_T	*p, *np;
    పూర్ణాంకం		wordnr = startwordnr;
    పూర్ణాంకం		nr;
    పూర్ణాంకం		prev_nr;

    FOR_ALL_కణుపు_SIBLINGS(కణుపు, p)
    {
	ఒకవేళ (p->wn_byte == NUL)
	{
	    gap->ga_len = 0;
	    prev_nr = 0;
	    కొరకు (np = p; np != ఖాళీ && np->wn_byte == NUL; np = np->wn_sibling)
	    {
		ఒకవేళ (ga_grow(gap, 10) == FAIL)
		    తిరిగి -1;

		nr = (np->wn_flags << 16) + (np->wn_region & 0xffff);
		// Compute the offset from the previous nr and store the
		// offset in a way that it takes a minimum number of bytes.
		// It's a bit like utf-8, but without the need to mark
		// following bytes.
		nr -= prev_nr;
		prev_nr += nr;
		gap->ga_len += offset2bytes(nr,
					 (char_u *)gap->ga_దత్తాంశం + gap->ga_len);
	    }

	    // add the NUL byte
	    ((char_u *)gap->ga_దత్తాంశం)[gap->ga_len++] = NUL;

	    ఒకవేళ (ml_append_buf(spin->si_spellbuf, (linenr_T)wordnr,
				     gap->ga_దత్తాంశం, gap->ga_len, TRUE) == FAIL)
		తిరిగి -1;
	    ++wordnr;

	    // Remove extra NUL entries, we no longer need them. We don't
	    // bother freeing the nodes, they won't be reused anyway.
	    అయితే (p->wn_sibling != ఖాళీ && p->wn_sibling->wn_byte == NUL)
		p->wn_sibling = p->wn_sibling->wn_sibling;

	    // Clear the flags on the remaining NUL node, so that compression
	    // works a lot better.
	    p->wn_flags = 0;
	    p->wn_region = 0;
	}
	లేకపోతే
	{
	    wordnr = sug_filltable(spin, p->wn_బిడ్డ, wordnr, gap);
	    ఒకవేళ (wordnr == -1)
		తిరిగి -1;
	}
    }
    తిరిగి wordnr;
}

/*
 * Convert an offset into a minimal number of bytes.
 * Similar to utf_char2byters, but use 8 bits in followup bytes and avoid NUL
 * bytes.
 */
    స్థిర పూర్ణాంకం
offset2bytes(పూర్ణాంకం nr, char_u *buf)
{
    పూర్ణాంకం	    rem;
    పూర్ణాంకం	    b1, b2, b3, b4;

    // Split the number in parts of base 255.  We need to avoid NUL bytes.
    b1 = nr % 255 + 1;
    rem = nr / 255;
    b2 = rem % 255 + 1;
    rem = rem / 255;
    b3 = rem % 255 + 1;
    b4 = rem / 255 + 1;

    ఒకవేళ (b4 > 1 || b3 > 0x1f)	// 4 bytes
    {
	buf[0] = 0xe0 + b4;
	buf[1] = b3;
	buf[2] = b2;
	buf[3] = b1;
	తిరిగి 4;
    }
    ఒకవేళ (b3 > 1 || b2 > 0x3f )	// 3 bytes
    {
	buf[0] = 0xc0 + b3;
	buf[1] = b2;
	buf[2] = b1;
	తిరిగి 3;
    }
    ఒకవేళ (b2 > 1 || b1 > 0x7f )	// 2 bytes
    {
	buf[0] = 0x80 + b2;
	buf[1] = b1;
	తిరిగి 2;
    }
				// 1 byte
    buf[0] = b1;
    తిరిగి 1;
}

/*
 * Write the .sug file in "fname".
 */
    స్థిర శూన్య
sug_write(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    wordnode_T	*వృక్షం;
    పూర్ణాంకం		nodecount;
    పూర్ణాంకం		wcount;
    char_u	*పంక్తి;
    linenr_T	lnum;
    పూర్ణాంకం		len;

    // Create the file.  Note that an existing file is silently overwritten!
    fd = mch_fopen((అక్షరం *)fname, "w");
    ఒకవేళ (fd == ఖాళీ)
    {
	semsg(_(e_cant_open_file_str), fname);
	తిరిగి;
    }

    vim_snprintf((అక్షరం *)IObuff, IOSIZE,
				  _("Writing suggestion file %s..."), fname);
    spell_సందేశం(spin, IObuff);

    /*
     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
     */
    ఒకవేళ (fwrite(VIMSUGMAGIC, VIMSUGMAGICL, (size_t)1, fd) != 1) // <fileID>
    {
	emsg(_(e_error_while_writing));
	వెళ్ళు theend;
    }
    putc(VIMSUGVERSION, fd);				// <versionnr>

    // Write si_sugtime to the file.
    put_సమయం(fd, spin->si_sugtime);			// <timestamp>

    /*
     * <SUGWORDTREE>
     */
    spin->si_memtot = 0;
    వృక్షం = spin->si_foldroot->wn_sibling;

    // Clear the index and wnode fields in the tree.
    clear_కణుపు(వృక్షం);

    // Count the number of nodes.  Needed to be able to allocate the
    // memory when reading the nodes.  Also fills in index for shared
    // nodes.
    nodecount = put_కణుపు(ఖాళీ, వృక్షం, 0, 0, FALSE);

    // number of nodes in 4 bytes
    put_బైట్లు(fd, (long_u)nodecount, 4);	// <nodecount>
    spin->si_memtot += nodecount + nodecount * పరిమాణం(పూర్ణాంకం);

    // Write the nodes.
    (శూన్య)put_కణుపు(fd, వృక్షం, 0, 0, FALSE);

    /*
     * <SUGTABLE>: <sugwcount> <sugline> ...
     */
    wcount = spin->si_spellbuf->b_ml.ml_పంక్తి_లెక్క;
    put_బైట్లు(fd, (long_u)wcount, 4);	// <sugwcount>

    కొరకు (lnum = 1; lnum <= (linenr_T)wcount; ++lnum)
    {
	// <sugline>: <sugnr> ... NUL
	పంక్తి = ml_get_buf(spin->si_spellbuf, lnum, FALSE);
	len = ml_get_buf_len(spin->si_spellbuf, lnum) + 1;
	ఒకవేళ (fwrite(పంక్తి, (size_t)len, (size_t)1, fd) == 0)
	{
	    emsg(_(e_error_while_writing));
	    వెళ్ళు theend;
	}
	spin->si_memtot += len;
    }

    // Write another byte to check for errors.
    ఒకవేళ (putc(0, fd) == EOF)
	emsg(_(e_error_while_writing));

    vim_snprintf((అక్షరం *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin->si_memtot);
    spell_సందేశం(spin, IObuff);

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
    శూన్య
mkspell(
    పూర్ణాంకం		fcount,
    char_u	**fnames,
    పూర్ణాంకం		ascii,		    // -ascii argument given
    పూర్ణాంకం		over_write,	    // overwrite existing output file
    పూర్ణాంకం		added_మాట)	    // invoked through "zg"
{
    char_u	*fname = ఖాళీ;
    char_u	*wfname;
    char_u	**innames;
    పూర్ణాంకం		incount;
    afffile_T	*(afile[MAXREGIONS]);
    పూర్ణాంకం		i;
    పూర్ణాంకం		len;
    stat_T	st;
    పూర్ణాంకం		error = FALSE;
    spellinfo_T spin;

    CLEAR_క్షేత్రం(spin);
    spin.si_verbose = !added_మాట;
    spin.si_ascii = ascii;
    spin.si_followup = TRUE;
    spin.si_rem_accents = TRUE;
    ga_init2(&spin.si_rep, పరిమాణం(fromto_T), 20);
    ga_init2(&spin.si_repsal, పరిమాణం(fromto_T), 20);
    ga_init2(&spin.si_sal, పరిమాణం(fromto_T), 20);
    ga_init2(&spin.si_map, పరిమాణం(char_u), 100);
    ga_init2(&spin.si_comppat, పరిమాణం(char_u *), 20);
    ga_init2(&spin.si_prefcond, పరిమాణం(char_u *), 50);
    hash_init(&spin.si_commonwords);
    spin.si_newcompID = 127;	// start compound ID at first maximum

    // default: fnames[0] is output file, following are input files
    // When "fcount" is 1 there is only one file.
    innames = &fnames[fcount == 1 ? 0 : 1];
    incount = fcount - 1;

    wfname = alloc(MAXPATHL);
    ఒకవేళ (wfname == ఖాళీ)
	తిరిగి;

    ఒకవేళ (fcount >= 1)
    {
	len = (పూర్ణాంకం)STRLEN(fnames[0]);
	ఒకవేళ (fcount == 1 && len > 4 && STRCMP(fnames[0] + len - 4, ".add") == 0)
	{
	    // For ":mkspell path/en.latin1.add" output file is
	    // "path/en.latin1.add.spl".
	    incount = 1;
	    vim_snprintf((అక్షరం *)wfname, MAXPATHL, "%s.spl", fnames[0]);
	}
	లేకపోతే ఒకవేళ (fcount == 1)
	{
	    // For ":mkspell path/vim" output file is "path/vim.latin1.spl".
	    incount = 1;
	    vim_snprintf((అక్షరం *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());
	}
	లేకపోతే ఒకవేళ (len > 4 && STRCMP(fnames[0] + len - 4, ".spl") == 0)
	{
	    // Name ends in ".spl", use as the file name.
	    vim_strncpy(wfname, fnames[0], MAXPATHL - 1);
	}
	లేకపోతే
	    // Name should be language, make the file name from it.
	    vim_snprintf((అక్షరం *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());

	// Check for .ascii.spl.
	ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)gettail(wfname), SPL_FNAME_ASCII) != ఖాళీ)
	    spin.si_ascii = TRUE;

	// Check for .add.spl.
	ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)gettail(wfname), SPL_FNAME_కూడు) != ఖాళీ)
	    spin.si_కూడు = TRUE;
    }

    ఒకవేళ (incount <= 0)
	emsg(_(e_invalid_వాదం));	// need at least output and input names
    లేకపోతే ఒకవేళ (vim_strchr(gettail(wfname), '_') != ఖాళీ)
	emsg(_(e_output_file_name_must_not_have_region_name));
    లేకపోతే ఒకవేళ (incount > MAXREGIONS)
	semsg(_(e_only_పైకి_గమ్యం_nr_regions_supported), MAXREGIONS);
    లేకపోతే
    {
	// Check for overwriting before doing things that may take a lot of
	// time.
	ఒకవేళ (!over_write && mch_stat((అక్షరం *)wfname, &st) >= 0)
	{
	    emsg(_(e_file_exists));
	    వెళ్ళు theend;
	}
	ఒకవేళ (mch_isdir(wfname))
	{
	    semsg(_(e_str_is_directory), wfname);
	    వెళ్ళు theend;
	}

	fname = alloc(MAXPATHL);
	ఒకవేళ (fname == ఖాళీ)
	    వెళ్ళు theend;

	/*
	 * Init the aff and dic pointers.
	 * Get the region names if there are more than 2 arguments.
	 */
	కొరకు (i = 0; i < incount; ++i)
	{
	    afile[i] = ఖాళీ;

	    ఒకవేళ (incount > 1)
	    {
		len = (పూర్ణాంకం)STRLEN(innames[i]);
		ఒకవేళ (STRLEN(gettail(innames[i])) < 5
						|| innames[i][len - 3] != '_')
		{
		    semsg(_(e_invalid_region_లోపల_str), innames[i]);
		    వెళ్ళు theend;
		}
		spin.si_region_name[i * 2] = TOLOWER_ASC(innames[i][len - 2]);
		spin.si_region_name[i * 2 + 1] =
					     TOLOWER_ASC(innames[i][len - 1]);
	    }
	}
	spin.si_region_లెక్క = incount;

	spin.si_foldroot = wordtree_alloc(&spin);
	spin.si_keeproot = wordtree_alloc(&spin);
	spin.si_prefroot = wordtree_alloc(&spin);
	ఒకవేళ (spin.si_foldroot == ఖాళీ
		|| spin.si_keeproot == ఖాళీ
		|| spin.si_prefroot == ఖాళీ)
	{
	    free_ఖండాలు(spin.si_ఖండాలు);
	    వెళ్ళు theend;
	}

	// When not producing a .add.spl file clear the character table when
	// we encounter one in the .aff file.  This means we dump the current
	// one in the .spl file if the .aff file doesn't define one.  That's
	// better than guessing the contents, the table will match a
	// previously loaded spell file.
	ఒకవేళ (!spin.si_కూడు)
	    spin.si_clear_chartab = TRUE;

	/*
	 * Read all the .aff and .dic files.
	 * Text is converted to 'encoding'.
	 * Words are stored in the case-folded and keep-case trees.
	 */
	కొరకు (i = 0; i < incount && !error; ++i)
	{
	    spin.si_conv.vc_type = CONV_NONE;
	    spin.si_region = 1 << i;

	    vim_snprintf((అక్షరం *)fname, MAXPATHL, "%s.aff", innames[i]);
	    ఒకవేళ (mch_stat((అక్షరం *)fname, &st) >= 0)
	    {
		// Read the .aff file.  Will init "spin->si_conv" based on the
		// "SET" line.
		afile[i] = spell_read_aff(&spin, fname);
		ఒకవేళ (afile[i] == ఖాళీ)
		    error = TRUE;
		లేకపోతే
		{
		    // Read the .dic file and store the words in the trees.
		    vim_snprintf((అక్షరం *)fname, MAXPATHL, "%s.dic",
								  innames[i]);
		    ఒకవేళ (spell_read_dic(&spin, fname, afile[i]) == FAIL)
			error = TRUE;
		}
	    }
	    లేకపోతే
	    {
		// No .aff file, try reading the file as a word list.  Store
		// the words in the trees.
		ఒకవేళ (spell_read_wordfile(&spin, innames[i]) == FAIL)
		    error = TRUE;
	    }

	    // Free any conversion stuff.
	    convert_అమరిక(&spin.si_conv, ఖాళీ, ఖాళీ);
	}

	ఒకవేళ (spin.si_compflags != ఖాళీ && spin.si_nobreak)
	    సందేశం(_("Warning: both compounding and NOBREAK specified"));

	ఒకవేళ (!error && !got_int)
	{
	    /*
	     * Combine tails in the tree.
	     */
	    spell_సందేశం(&spin, (char_u *)_(సందేశం_compressing));
	    wordtree_compress(&spin, spin.si_foldroot, "case-folded");
	    wordtree_compress(&spin, spin.si_keeproot, "keep-case");
	    wordtree_compress(&spin, spin.si_prefroot, "prefixes");
	}

	ఒకవేళ (!error && !got_int)
	{
	    /*
	     * Write the info in the spell file.
	     */
	    vim_snprintf((అక్షరం *)IObuff, IOSIZE,
				      _("Writing spell file %s..."), wfname);
	    spell_సందేశం(&spin, IObuff);

	    error = write_vim_spell(&spin, wfname) == FAIL;

	    spell_సందేశం(&spin, (char_u *)_("Done!"));
	    vim_snprintf((అక్షరం *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin.si_memtot);
	    spell_సందేశం(&spin, IObuff);

	    /*
	     * If the file is loaded need to reload it.
	     */
	    ఒకవేళ (!error)
		spell_reload_one(wfname, added_మాట);
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
	కొరకు (i = 0; i < incount; ++i)
	    ఒకవేళ (afile[i] != ఖాళీ)
		spell_free_aff(afile[i]);

	// Free all the bits and pieces at once.
	free_ఖండాలు(spin.si_ఖండాలు);

	/*
	 * If there is soundfolding info and no NOSUGFILE item create the
	 * .sug file with the soundfolded word trie.
	 */
	ఒకవేళ (spin.si_sugtime != 0 && !error && !got_int)
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
    స్థిర శూన్య
spell_సందేశం(spellinfo_T *spin, char_u *str)
{
    ఒకవేళ (spin->si_verbose || p_verbose > 2)
    {
	ఒకవేళ (!spin->si_verbose)
	    verbose_enter();
	సందేశం((అక్షరం *)str);
	బహిర్గతం_flush();
	ఒకవేళ (!spin->si_verbose)
	    verbose_leave();
    }
}

/*
 * ":[count]spellgood  {word}"
 * ":[count]spellwrong {word}"
 * ":[count]spellundo  {word}"
 * ":[count]spellrare  {word}"
 */
    శూన్య
ex_spell(exarg_T *eap)
{
    spell_కూడు_మాట(eap->arg, (పూర్ణాంకం)STRLEN(eap->arg),
		eap->cmdidx == CMD_spellwrong ? SPELL_కూడు_BAD :
		eap->cmdidx == CMD_spellrare ? SPELL_కూడు_RARE : SPELL_కూడు_GOOD,
				   eap->forceit ? 0 : (పూర్ణాంకం)eap->line2,
				   eap->cmdidx == CMD_spellundo);
}

/*
 * Add "word[len]" to 'spellfile' as a good, rare or bad word.
 */
    శూన్య
spell_కూడు_మాట(
    char_u	*మాట,
    పూర్ణాంకం		len,
    పూర్ణాంకం		what,	    // SPELL_ADD_ values
    పూర్ణాంకం		idx,	    // "zG" and "zW": zero, otherwise index in
			    // 'spellfile'
    పూర్ణాంకం		undo)	    // TRUE for "zug", "zuG", "zuw" and "zuW"
{
    FILE	*fd = ఖాళీ;
    buf_T	*buf = ఖాళీ;
    పూర్ణాంకం		new_spf = FALSE;
    char_u	*fname;
    char_u	*fnamebuf = ఖాళీ;
    char_u	పంక్తి[MAXWLEN * 2];
    దీర్ఘ	fpos, fpos_తదుపరి = 0;
    పూర్ణాంకం		i;
    size_t	linelen;
    char_u	*spf;

    ఒకవేళ (!valid_spell_మాట(మాట, మాట + len))
    {
	emsg(_(e_illegal_character_లోపల_మాట));
	తిరిగి;
    }

    ఒకవేళ (idx == 0)	    // use internal wordlist
    {
	ఒకవేళ (int_wordlist == ఖాళీ)
	{
	    int_wordlist = vim_tempname('s', FALSE);
	    ఒకవేళ (int_wordlist == ఖాళీ)
		తిరిగి;
	}
	fname = int_wordlist;
    }
    లేకపోతే
    {
	// If 'spellfile' isn't set figure out a good default value.
	ఒకవేళ (*curwin->w_s->b_p_spf == NUL)
	{
	    init_spellfile();
	    new_spf = TRUE;
	}

	ఒకవేళ (*curwin->w_s->b_p_spf == NUL)
	{
	    semsg(_(e_option_str_is_not_సమితి), "spellfile");
	    తిరిగి;
	}
	fnamebuf = alloc(MAXPATHL);
	ఒకవేళ (fnamebuf == ఖాళీ)
	    తిరిగి;

	కొరకు (spf = curwin->w_s->b_p_spf, i = 1; *spf != NUL; ++i)
	{
	    copy_option_part(&spf, fnamebuf, MAXPATHL, ",");
	    ఒకవేళ (i == idx)
		ఆపు;
	    ఒకవేళ (*spf == NUL)
	    {
		semsg(_(e_spellfile_does_not_have_nr_entries), idx);
		vim_free(fnamebuf);
		తిరిగి;
	    }
	}

	// Check that the user isn't editing the .add file somewhere.
	buf = buflist_findname_exp(fnamebuf);
	ఒకవేళ (buf != ఖాళీ && buf->b_ml.ml_mfp == ఖాళీ)
	    buf = ఖాళీ;
	ఒకవేళ (buf != ఖాళీ && buf_Is_మారింది(buf))
	{
	    emsg(_(e_file_is_loaded_లోపల_another_బఫరు));
	    vim_free(fnamebuf);
	    తిరిగి;
	}

	fname = fnamebuf;
    }

    ఒకవేళ (what == SPELL_కూడు_BAD || undo)
    {
	// When the word appears as good word we need to remove that one,
	// since its flags sort before the one with WF_BANNED.
	fd = mch_fopen((అక్షరం *)fname, "r");
	ఒకవేళ (fd != ఖాళీ)
	{
	    అయితే (!vim_fgets(పంక్తి, MAXWLEN * 2, fd))
	    {
		fpos = fpos_తదుపరి;
		fpos_తదుపరి = ftell(fd);
		ఒకవేళ (fpos_తదుపరి < 0)
		    ఆపు;  // should never happen
		linelen = STRLEN(పంక్తి);
		ఒకవేళ (linelen >= (size_t)len
			&& STRNCMP(మాట, పంక్తి, len) == 0
			&& (పంక్తి[len] == '/' || పంక్తి[len] < ' '))
		{
		    // Found duplicate word.  Remove it by writing a '#' at
		    // the start of the line.  Mixing reading and writing
		    // doesn't work for all systems, close the file first.
		    fclose(fd);
		    fd = mch_fopen((అక్షరం *)fname, "r+");
		    ఒకవేళ (fd == ఖాళీ)
			ఆపు;
		    ఒకవేళ (fseek(fd, fpos, SEEK_SET) == 0)
		    {
			fputc('#', fd);
			ఒకవేళ (undo)
			{
			    home_replace(ఖాళీ, fname, NameBuff, MAXPATHL, TRUE);
			    smsg(_("Word '%.*s' removed from %s"),
							 len, మాట, NameBuff);
			}
		    }
		    ఒకవేళ (fseek(fd, fpos_తదుపరి, SEEK_SET) != 0)
		    {
			PERROR(_("Seek error in spellfile"));
			ఆపు;
		    }
		}
	    }
	    ఒకవేళ (fd != ఖాళీ)
		fclose(fd);
	}
    }

    ఒకవేళ (!undo)
    {
	fd = mch_fopen((అక్షరం *)fname, "a");
	ఒకవేళ (fd == ఖాళీ && new_spf)
	{
	    char_u *p;

	    // We just initialized the 'spellfile' option and can't open the
	    // file.  We may need to create the "spell" directory first.  We
	    // already checked the runtime directory is writable in
	    // init_spellfile().
	    ఒకవేళ (!dir_యొక్క_file_exists(fname) && (p = gettail_sep(fname)) != fname)
	    {
		పూర్ణాంకం c = *p;

		// The directory doesn't exist.  Try creating it and opening
		// the file again.
		*p = NUL;
		vim_mkdir(fname, 0755);
		*p = c;
		fd = mch_fopen((అక్షరం *)fname, "a");
	    }
	}

	ఒకవేళ (fd == ఖాళీ)
	    semsg(_(e_cant_open_file_str), fname);
	లేకపోతే
	{
	    ఒకవేళ (what == SPELL_కూడు_BAD)
		fprintf(fd, "%.*s/!\n", len, మాట);
	    లేకపోతే ఒకవేళ (what == SPELL_కూడు_RARE)
		fprintf(fd, "%.*s/?\n", len, మాట);
	    లేకపోతే
		fprintf(fd, "%.*s\n", len, మాట);
	    fclose(fd);

	    home_replace(ఖాళీ, fname, NameBuff, MAXPATHL, TRUE);
	    smsg(_("Word '%.*s' added to %s"), len, మాట, NameBuff);
	}
    }

    ఒకవేళ (fd != ఖాళీ)
    {
	// Update the .add.spl file.
	mkspell(1, &fname, FALSE, TRUE, TRUE);

	// If the .add file is edited somewhere, reload it.
	ఒకవేళ (buf != ఖాళీ)
	    buf_reload(buf, buf->b_orig_బహుళకం, FALSE);

	redraw_all_later(UPD_SOME_VALID);
    }
    vim_free(fnamebuf);
}

/*
 * Initialize 'spellfile' for the current buffer.
 */
    స్థిర శూన్య
init_spellfile(శూన్య)
{
    string_T	buf;
    char_u	*fname;
    char_u	*rtp;
    char_u	*lend;
    పూర్ణాంకం		aspath = FALSE;
    char_u	*lstart = curbuf->b_s.b_p_spl;

    ఒకవేళ (*curwin->w_s->b_p_spl == NUL || curwin->w_s->b_langp.ga_len <= 0)
	తిరిగి;

    buf.string = alloc(MAXPATHL);
    ఒకవేళ (buf.string == ఖాళీ)
	తిరిగి;

    buf.నిడివి = 0;

    // Find the end of the language name.  Exclude the region.  If there
    // is a path separator remember the start of the tail.
    కొరకు (lend = curwin->w_s->b_p_spl; *lend != NUL
	    && vim_strchr((char_u *)",._", *lend) == ఖాళీ; ++lend)
	ఒకవేళ (vim_ispathsep(*lend))
	{
	    aspath = TRUE;
	    lstart = lend + 1;
	}

    // Loop over all entries in 'runtimepath'.  Use the first one where we
    // are allowed to write.
    rtp = p_rtp;
    అయితే (*rtp != NUL)
    {
	ఒకవేళ (aspath)
	    // Use directory of an entry with path, e.g., for
	    // "/dir/lg.utf-8.spl" use "/dir".
	    vim_strncpy(buf.string, curbuf->b_s.b_p_spl,
		    lstart - curbuf->b_s.b_p_spl - 1);
	లేకపోతే
	    // Copy the path from 'runtimepath' to buf[].
	    buf.నిడివి = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");
	ఒకవేళ (filewritable(buf.string) == 2)
	{
	    // Use the first language name from 'spelllang' and the
	    // encoding used in the first loaded .spl file.
	    ఒకవేళ (aspath)
	    {
		buf.నిడివి = (size_t)(lend - curbuf->b_s.b_p_spl);
		vim_strncpy(buf.string, curbuf->b_s.b_p_spl, buf.నిడివి);
	    }
	    లేకపోతే
	    {
		// Create the "spell" directory if it doesn't exist yet.
		buf.నిడివి += vim_snprintf_safelen((అక్షరం *)buf.string + buf.నిడివి,
		    MAXPATHL - buf.నిడివి, "/spell");
		ఒకవేళ (filewritable(buf.string) != 2)
		{
		    ఒకవేళ (vim_mkdir(buf.string, 0755) != 0)
		    {
			vim_free(buf.string);
			తిరిగి;
		    }
		}

		buf.నిడివి += vim_snprintf_safelen((అక్షరం *)buf.string + buf.నిడివి,
		    MAXPATHL - buf.నిడివి, "/%.*s", (పూర్ణాంకం)(lend - lstart), lstart);
	    }
	    fname = LANGP_నమోదు(curwin->w_s->b_langp, 0)
		->lp_slang->sl_fname;
	    vim_snprintf((అక్షరం *)buf.string + buf.నిడివి, MAXPATHL - buf.నిడివి,
		".%s.add",
		fname != ఖాళీ && అక్షరశ్రేణి_శోధన((అక్షరం *)gettail(fname), ".ascii.") != ఖాళీ
		    ? (char_u *)"ascii" : spell_enc());
	    సమితి_option_విలువ_give_err((char_u *)"spellfile",
		    0L, buf.string, OPT_LOCAL);
	    ఆపు;
	}
	aspath = FALSE;
    }

    vim_free(buf.string);
}



/*
 * Set the spell character tables from strings in the affix file.
 */
    స్థిర పూర్ణాంకం
సమితి_spell_chartab(char_u *fol, char_u *దిగువ, char_u *upp)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    char_u	*pf = fol, *pl = దిగువ, *pu = upp;
    పూర్ణాంకం		f, l, u;

    clear_spell_chartab(&new_st);

    అయితే (*pf != NUL)
    {
	ఒకవేళ (*pl == NUL || *pu == NUL)
	{
	    emsg(_(e_format_error_లోపల_affix_file_fol_దిగువ_or_upp));
	    తిరిగి FAIL;
	}
	f = mb_ptr2char_adv(&pf);
	l = mb_ptr2char_adv(&pl);
	u = mb_ptr2char_adv(&pu);

	// Every character that appears is a word character.
	ఒకవేళ (f < 256)
	    new_st.st_isw[f] = TRUE;
	ఒకవేళ (l < 256)
	    new_st.st_isw[l] = TRUE;
	ఒకవేళ (u < 256)
	    new_st.st_isw[u] = TRUE;

	// if "LOW" and "FOL" are not the same the "LOW" char needs
	// case-folding
	ఒకవేళ (l < 256 && l != f)
	{
	    ఒకవేళ (f >= 256)
	    {
		emsg(_(e_character_లోపల_fol_దిగువ_or_upp_is_బహిర్గతం_యొక్క_range));
		తిరిగి FAIL;
	    }
	    new_st.st_fold[l] = f;
	}

	// if "UPP" and "FOL" are not the same the "UPP" char needs
	// case-folding, it's upper case and the "UPP" is the upper case of
	// "FOL" .
	ఒకవేళ (u < 256 && u != f)
	{
	    ఒకవేళ (f >= 256)
	    {
		emsg(_(e_character_లోపల_fol_దిగువ_or_upp_is_బహిర్గతం_యొక్క_range));
		తిరిగి FAIL;
	    }
	    new_st.st_fold[u] = f;
	    new_st.st_isu[u] = TRUE;
	    new_st.st_పై[f] = u;
	}
    }

    ఒకవేళ (*pl != NUL || *pu != NUL)
    {
	emsg(_(e_format_error_లోపల_affix_file_fol_దిగువ_or_upp));
	తిరిగి FAIL;
    }

    తిరిగి సమితి_spell_ముగింపు(&new_st);
}

/*
 * Set the spell character tables from strings in the .spl file.
 */
    స్థిర శూన్య
సమితి_spell_charflags(
    char_u	*flags,
    పూర్ణాంకం		cnt,	    // length of "flags"
    char_u	*fol)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    పూర్ణాంకం		i;
    char_u	*p = fol;
    పూర్ణాంకం		c;

    clear_spell_chartab(&new_st);

    కొరకు (i = 0; i < 128; ++i)
    {
	ఒకవేళ (i < cnt)
	{
	    new_st.st_isw[i + 128] = (flags[i] & CF_మాట) != 0;
	    new_st.st_isu[i + 128] = (flags[i] & CF_పై) != 0;
	}

	ఒకవేళ (*p != NUL)
	{
	    c = mb_ptr2char_adv(&p);
	    new_st.st_fold[i + 128] = c;
	    ఒకవేళ (i + 128 != c && new_st.st_isu[i + 128] && c < 256)
		new_st.st_పై[c] = i + 128;
	}
    }

    (శూన్య)సమితి_spell_ముగింపు(&new_st);
}

    స్థిర పూర్ణాంకం
సమితి_spell_ముగింపు(spelltab_T *new_st)
{
    పూర్ణాంకం		i;

    ఒకవేళ (did_సమితి_spelltab)
    {
	// check that it's the same table
	కొరకు (i = 0; i < 256; ++i)
	{
	    ఒకవేళ (spelltab.st_isw[i] != new_st->st_isw[i]
		    || spelltab.st_isu[i] != new_st->st_isu[i]
		    || spelltab.st_fold[i] != new_st->st_fold[i]
		    || spelltab.st_పై[i] != new_st->st_పై[i])
	    {
		emsg(_(e_మాట_characters_తేడా_between_spell_files));
		తిరిగి FAIL;
	    }
	}
    }
    లేకపోతే
    {
	// copy the new spelltab into the one being used
	spelltab = *new_st;
	did_సమితి_spelltab = TRUE;
    }

    తిరిగి OK;
}

/*
 * Write the table with prefix conditions to the .spl file.
 * When "fd" is NULL only count the length of what is written and return it.
 */
    స్థిర పూర్ణాంకం
write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv)
{
    పూర్ణాంకం		i;
    char_u	*p;
    పూర్ణాంకం		len;
    పూర్ణాంకం		totlen;

    ఒకవేళ (fd != ఖాళీ)
	put_బైట్లు(fd, (long_u)gap->ga_len, 2);	    // <prefcondcnt>

    totlen = 2 + gap->ga_len; // length of <prefcondcnt> and <condlen> bytes

    కొరకు (i = 0; i < gap->ga_len; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	p = ((char_u **)gap->ga_దత్తాంశం)[i];
	ఒకవేళ (p != ఖాళీ)
	{
	    len = (పూర్ణాంకం)STRLEN(p);
	    ఒకవేళ (fd != ఖాళీ)
	    {
		fputc(len, fd);
		*fwv &= fwrite(p, (size_t)len, (size_t)1, fd);
	    }
	    totlen += len;
	}
	లేకపోతే ఒకవేళ (fd != ఖాళీ)
	    fputc(0, fd);
    }

    తిరిగి totlen;
}

/*
 * Use map string "map" for languages "lp".
 */
    స్థిర శూన్య
సమితి_map_str(slang_T *lp, char_u *map)
{
    char_u	*p;
    పూర్ణాంకం		headc = 0;
    పూర్ణాంకం		c;
    పూర్ణాంకం		i;

    ఒకవేళ (*map == NUL)
    {
	lp->sl_has_map = FALSE;
	తిరిగి;
    }
    lp->sl_has_map = TRUE;

    // Init the array and hash tables empty.
    కొరకు (i = 0; i < 256; ++i)
	lp->sl_map_array[i] = 0;
    hash_init(&lp->sl_map_hash);

    /*
     * The similar characters are stored separated with slashes:
     * "aaa/bbb/ccc/".  Fill sl_map_array[c] with the character before c and
     * before the same slash.  For characters above 255 sl_map_hash is used.
     */
    కొరకు (p = map; *p != NUL; )
    {
	c = mb_cptr2char_adv(&p);
	ఒకవేళ (c == '/')
	    headc = 0;
	లేకపోతే
	{
	    ఒకవేళ (headc == 0)
		 headc = c;

	    // Characters above 255 don't fit in sl_map_array[], put them in
	    // the hash table.  Each entry is the char, a NUL the headchar and
	    // a NUL.
	    ఒకవేళ (c >= 256)
	    {
		పూర్ణాంకం	    cl = mb_char2len(c);
		పూర్ణాంకం	    headcl = mb_char2len(headc);
		char_u	    *b;
		hash_T	    hash;
		hashitem_T  *hi;

		b = alloc(cl + headcl + 2);
		ఒకవేళ (b == ఖాళీ)
		    తిరిగి;
		mb_char2bytes(c, b);
		b[cl] = NUL;
		mb_char2bytes(headc, b + cl + 1);
		b[cl + 1 + headcl] = NUL;
		hash = hash_hash(b);
		hi = hash_lookup(&lp->sl_map_hash, b, hash);
		ఒకవేళ (HASHITEM_ఖాళీలు(hi))
		    hash_కూడు_అంశం(&lp->sl_map_hash, hi, b, hash);
		లేకపోతే
		{
		    // This should have been checked when generating the .spl
		    // file.
		    emsg(_(e_duplicate_char_లోపల_map_నమోదు));
		    vim_free(b);
		}
	    }
	    లేకపోతే
		lp->sl_map_array[c] = headc;
	}
    }
}

#అయితే_ముగింపు  // FEAT_SPELL
