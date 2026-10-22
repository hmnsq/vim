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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_SPELL)

#ಅನ್ತರ್ಭಾವ <time.h>	// for time_t

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
#define SN_ಪದಗಳು	13	// common words
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
#define FOR_ALL_ಗಂಟು_SIBLINGS(ಗಂಟು, np) \
    ಪ್ರತಿ ((np) = (ಗಂಟು); (np) != ರಿಕ್ತ; (np) = (np)->wn_sibling)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ set_spell_finish(spelltab_T	*new_st);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_region_section(FILE *fd, slang_T *slang, ಪೂರ್ಣಾಙ್ಕ len);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_charflags_section(FILE *fd);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_prefcond_section(FILE *fd, slang_T *lp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_rep_section(FILE *fd, garray_T *gap, short *first);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_sal_section(FILE *fd, slang_T *slang);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_ಪದಗಳು_section(FILE *fd, slang_T *lp, ಪೂರ್ಣಾಙ್ಕ len);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_sofo_section(FILE *fd, slang_T *slang);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_compound(FILE *fd, slang_T *slang, ಪೂರ್ಣಾಙ್ಕ len);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ set_sofo(slang_T *lp, char_u *from, char_u *to);
ಸ್ಥಿರ ಶೂನ್ಯ set_sal_first(slang_T *lp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ *mb_str2wide(char_u *s);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ spell_read_ವೃಕ್ಷ(FILE *fd, char_u **bytsp, long *bytsp_len, idx_T **idxsp, ಪೂರ್ಣಾಙ್ಕ prefixtree, ಪೂರ್ಣಾಙ್ಕ prefixcnt);
ಸ್ಥಿರ idx_T read_ವೃಕ್ಷ_ಗಂಟು(FILE *fd, char_u *byts, idx_T *idxs, ಪೂರ್ಣಾಙ್ಕ maxidx, idx_T startidx, ಪೂರ್ಣಾಙ್ಕ prefixtree, ಪೂರ್ಣಾಙ್ಕ maxprefcondnr, ಪೂರ್ಣಾಙ್ಕ depth);
ಸ್ಥಿರ ಶೂನ್ಯ set_spell_charflags(char_u *flags, ಪೂರ್ಣಾಙ್ಕ cnt, char_u *upp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ set_spell_chartab(char_u *fol, char_u *ಕೆಳಗಿನ, char_u *upp);
ಸ್ಥಿರ ಶೂನ್ಯ set_map_str(slang_T *lp, char_u *map);


ಸ್ಥಿರ ವರ್ಣ *e_afftrailing = N_("Trailing text in %s line %d: %s");
ಸ್ಥಿರ ವರ್ಣ *e_affname = N_("Affix name too long in %s line %d: %s");
ಸ್ಥಿರ ವರ್ಣ *ಸಂದೇಶ_compressing = N_("Compressing word tree...");

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
    ಪೂರ್ಣಾಙ್ಕ		silent)		// no error if file doesn't exist
{
    FILE	*fd;
    char_u	buf[VIMSPELLMAGICL];
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		len;
    slang_T	*lp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		c = 0;
    ಪೂರ್ಣಾಙ್ಕ		res;
    ಪೂರ್ಣಾಙ್ಕ		did_estack_ತಳ್ಳು = FALSE;
    ESTACK_CHECK_DECLARATION;

    fd = mch_fopen((ವರ್ಣ *)fname, "r");
    ಯದಿ (fd == ರಿಕ್ತ)
    {
	ಯದಿ (!silent)
	    semsg(_(e_cant_open_file_str), fname);
	ಅನ್ಯಥಾ ಯದಿ (p_verbose > 2)
	{
	    verbose_enter();
	    smsg((ಧ್ರುವ ವರ್ಣ *)e_cant_open_file_str, fname);
	    verbose_leave();
	}
	ಗಚ್ಛ endFAIL;
    }
    ಯದಿ (p_verbose > 2)
    {
	verbose_enter();
	smsg(_("Reading spell file \"%s\""), fname);
	verbose_leave();
    }

    ಯದಿ (old_lp == ರಿಕ್ತ)
    {
	lp = slang_alloc(lang);
	ಯದಿ (lp == ರಿಕ್ತ)
	    ಗಚ್ಛ endFAIL;

	// Remember the file name, used to reload the file when it's updated.
	lp->sl_fname = vim_strsave(fname);
	ಯದಿ (lp->sl_fname == ರಿಕ್ತ)
	    ಗಚ್ಛ endFAIL;

	// Check for .add.spl (_add.spl for VMS).
	lp->sl_add = strstr((ವರ್ಣ *)gettail(fname), SPL_FNAME_ADD) != ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
	lp = old_lp;

    // Set sourcing_name, so that error messages mention the file name.
    estack_ತಳ್ಳು(ETYPE_SPELL, fname, 0);
    ESTACK_CHECK_SETUP;
    did_estack_ತಳ್ಳು = TRUE;

    /*
     * <HEADER>: <fileID>
     */
    ಪ್ರತಿ (i = 0; i < VIMSPELLMAGICL; ++i)
	buf[i] = (c = getc(fd)) == EOF ? 0 : c;		// <fileID>
    ಯದಿ (STRNCMP(buf, VIMSPELLMAGIC, VIMSPELLMAGICL) != 0)
    {
	emsg(_(e_this_does_not_look_like_spell_file));
	ಗಚ್ಛ endFAIL;
    }
    c = getc(fd);					// <versionnr>
    ಯದಿ (c < VIMSPELLVERSION)
    {
	emsg(_(e_old_spell_file_needs_to_be_updated));
	ಗಚ್ಛ endFAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (c > VIMSPELLVERSION)
    {
	emsg(_(e_spell_file_is_for_newer_version_of_vim));
	ಗಚ್ಛ endFAIL;
    }


    /*
     * <SECTIONS>: <section> ... <sectionend>
     * <section>: <sectionID> <sectionflags> <sectionlen> (section contents)
     */
    ಪ್ರತಿ (;;)
    {
	n = getc(fd);			    // <sectionID> or <sectionend>
	ಯದಿ (n == SN_END)
	    ಭಙ್ಗ;
	c = getc(fd);					// <sectionflags>
	len = get4c(fd);				// <sectionlen>
	ಯದಿ (len < 0)
	    ಗಚ್ಛ truncerr;

	res = 0;
	ವಿಕಲ್ಪ (n)
	{
	    ಪಕ್ಷ SN_INFO:
		lp->sl_info = read_string(fd, len);	// <infotext>
		ಯದಿ (lp->sl_info == ರಿಕ್ತ)
		    ಗಚ್ಛ endFAIL;
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_REGION:
		res = read_region_section(fd, lp, len);
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_CHARFLAGS:
		res = read_charflags_section(fd);
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_MIDWORD:
		lp->sl_midword = read_string(fd, len);	// <midword>
		ಯದಿ (lp->sl_midword == ರಿಕ್ತ)
		    ಗಚ್ಛ endFAIL;
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_PREFCOND:
		res = read_prefcond_section(fd, lp);
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_REP:
		res = read_rep_section(fd, &lp->sl_rep, lp->sl_rep_first);
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_REPSAL:
		res = read_rep_section(fd, &lp->sl_repsal, lp->sl_repsal_first);
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_SAL:
		res = read_sal_section(fd, lp);
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_SOFO:
		res = read_sofo_section(fd, lp);
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_MAP:
		p = read_string(fd, len);		// <mapstr>
		ಯದಿ (p == ರಿಕ್ತ)
		    ಗಚ್ಛ endFAIL;
		set_map_str(lp, p);
		vim_free(p);
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_ಪದಗಳು:
		res = read_ಪದಗಳು_section(fd, lp, len);
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_SUGFILE:
		lp->sl_sugtime = get8ctime(fd);		// <timestamp>
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_NOSPLITSUGS:
		lp->sl_nosplitsugs = TRUE;
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_NOCOMPOUNDSUGS:
		lp->sl_nocompoundsugs = TRUE;
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_COMPOUND:
		res = read_compound(fd, lp, len);
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_NOBREAK:
		lp->sl_nobreak = TRUE;
		ಭಙ್ಗ;

	    ಪಕ್ಷ SN_SYLLABLE:
		lp->sl_syllable = read_string(fd, len);	// <syllable>
		ಯದಿ (lp->sl_syllable == ರಿಕ್ತ)
		    ಗಚ್ಛ endFAIL;
		ಯದಿ (init_syl_tab(lp) != OK)
		    ಗಚ್ಛ endFAIL;
		ಭಙ್ಗ;

	    ಸ್ವಭಾವ:
		// Unsupported section.  When it's required give an error
		// message.  When it's not required skip the contents.
		ಯದಿ (c & SNF_REQUIRED)
		{
		    emsg(_(e_unsupported_section_in_spell_file));
		    ಗಚ್ಛ endFAIL;
		}
		ಯಾವತ್ (--len >= 0)
		    ಯದಿ (getc(fd) < 0)
			ಗಚ್ಛ truncerr;
		ಭಙ್ಗ;
	}
someerror:
	ಯದಿ (res == SP_FORMERROR)
	{
	    emsg(_(e_format_error_in_spell_file));
	    ಗಚ್ಛ endFAIL;
	}
	ಯದಿ (res == SP_TRUNCERROR)
	{
truncerr:
	    emsg(_(e_truncated_spell_file));
	    ಗಚ್ಛ endFAIL;
	}
	ಯದಿ (res == SP_OTHERERROR)
	    ಗಚ್ಛ endFAIL;
    }

    // <LWORDTREE>
    res = spell_read_ವೃಕ್ಷ(fd, &lp->sl_fbyts, &lp->sl_fbyts_len,
						      &lp->sl_fidxs, FALSE, 0);
    ಯದಿ (res != 0)
	ಗಚ್ಛ someerror;

    // <KWORDTREE>
    res = spell_read_ವೃಕ್ಷ(fd, &lp->sl_kbyts, ರಿಕ್ತ, &lp->sl_kidxs, FALSE, 0);
    ಯದಿ (res != 0)
	ಗಚ್ಛ someerror;

    // <PREFIXTREE>
    res = spell_read_ವೃಕ್ಷ(fd, &lp->sl_pbyts, ರಿಕ್ತ, &lp->sl_pidxs, TRUE,
							    lp->sl_prefixcnt);
    ಯದಿ (res != 0)
	ಗಚ್ಛ someerror;

    // For a new file link it in the list of spell files.
    ಯದಿ (old_lp == ರಿಕ್ತ && lang != ರಿಕ್ತ)
    {
	lp->sl_next = first_lang;
	first_lang = lp;
    }

    ಗಚ್ಛ endOK;

endFAIL:
    ಯದಿ (lang != ರಿಕ್ತ)
	// truncating the name signals the error to spell_load_lang()
	*lang = NUL;
    ಯದಿ (lp != ರಿಕ್ತ && old_lp == ರಿಕ್ತ)
	slang_free(lp);
    lp = ರಿಕ್ತ;

endOK:
    ಯದಿ (fd != ರಿಕ್ತ)
	fclose(fd);
    ಯದಿ (did_estack_ತಳ್ಳು)
    {
	ESTACK_CHECK_NOW;
	estack_ತೆಗೆ();
    }

    ಪ್ರತ್ಯಾಗಮ lp;
}

/*
 * Fill in the wordcount fields for a trie.
 * Returns the total number of words.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ವೃಕ್ಷ_ಎಣಿಕೆ_ಪದಗಳು(char_u *byts, long byts_len, idx_T *idxs)
{
    ಪೂರ್ಣಾಙ್ಕ		depth;
    idx_T	arridx[MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		curi[MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		c;
    idx_T	n;
    ಪೂರ್ಣಾಙ್ಕ		wordcount[MAXWLEN];

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;
    depth = 0;
    ಯಾವತ್ (depth >= 0 && !got_int)
    {
	ಯದಿ (curi[depth] > byts[arridx[depth]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[depth]] = wordcount[depth];
	    ಯದಿ (depth > 0)
		wordcount[depth - 1] += wordcount[depth];

	    --depth;
	    fast_breakcheck();
	}
	ಅನ್ಯಥಾ
	{
	    // Do one more byte at this node.
	    n = arridx[depth] + curi[depth];
	    ++curi[depth];

	    c = byts[n];
	    ಯದಿ (c == 0)
	    {
		// End of word, count it.
		++wordcount[depth];

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		ಯಾವತ್ (n + 1 < byts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[depth];
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (depth < MAXWLEN - 1)
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
    ಶೂನ್ಯ
suggest_load_files(ಶೂನ್ಯ)
{
    langp_T	*lp;
    ಪೂರ್ಣಾಙ್ಕ		lpi;
    slang_T	*slang;
    char_u	*dotp;
    FILE	*fd;
    char_u	buf[MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		i;
    time_t	timestamp;
    ಪೂರ್ಣಾಙ್ಕ		wcount;
    ಪೂರ್ಣಾಙ್ಕ		wordnr;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		c;

    // Do this for all languages that support sound folding.
    ಪ್ರತಿ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ಯದಿ (slang->sl_sugtime != 0 && !slang->sl_sugloaded)
	{
	    // Change ".spl" to ".sug" and open the file.  When the file isn't
	    // found silently skip it.  Do set "sl_sugloaded" so that we
	    // don't try again and again.
	    slang->sl_sugloaded = TRUE;

	    dotp = vim_strrchr(slang->sl_fname, '.');
	    ಯದಿ (dotp == ರಿಕ್ತ || fnamecmp(dotp, ".spl") != 0)
		ಅನುವರ್ತ;
	    STRCPY(dotp, ".sug");
	    fd = mch_fopen((ವರ್ಣ *)slang->sl_fname, "r");
	    ಯದಿ (fd == ರಿಕ್ತ)
		ಗಚ್ಛ nextone;

	    /*
	     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
	     */
	    ಪ್ರತಿ (i = 0; i < VIMSUGMAGICL; ++i)
		buf[i] = (c = getc(fd)) == EOF ? 0 : c;	// <fileID>
	    ಯದಿ (STRNCMP(buf, VIMSUGMAGIC, VIMSUGMAGICL) != 0)
	    {
		semsg(_(e_this_does_not_look_like_sug_file_str),
							     slang->sl_fname);
		ಗಚ್ಛ nextone;
	    }
	    c = getc(fd);				// <versionnr>
	    ಯದಿ (c < VIMSUGVERSION)
	    {
		semsg(_(e_old_sug_file_needs_to_be_updated_str),
							     slang->sl_fname);
		ಗಚ್ಛ nextone;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c > VIMSUGVERSION)
	    {
		semsg(_(e_sug_file_is_for_newer_version_of_vim_str),
							     slang->sl_fname);
		ಗಚ್ಛ nextone;
	    }

	    // Check the timestamp, it must be exactly the same as the one in
	    // the .spl file.  Otherwise the word numbers won't match.
	    timestamp = get8ctime(fd);			// <timestamp>
	    ಯದಿ (timestamp != slang->sl_sugtime)
	    {
		semsg(_(e_sug_file_doesnt_match_spl_file_str),
							     slang->sl_fname);
		ಗಚ್ಛ nextone;
	    }

	    /*
	     * <SUGWORDTREE>: <wordtree>
	     * Read the trie with the soundfolded words.
	     */
	    ಯದಿ (spell_read_ವೃಕ್ಷ(fd, &slang->sl_sbyts, &slang->sl_sbyts_len,
			&slang->sl_sidxs, FALSE, 0) != 0)
	    {
someerror:
		semsg(_(e_error_while_reading_sug_file_str),
							     slang->sl_fname);
		slang_clear_sug(slang);
		ಗಚ್ಛ nextone;
	    }

	    /*
	     * <SUGTABLE>: <sugwcount> <sugline> ...
	     *
	     * Read the table with word numbers.  We use a file buffer for
	     * this, because it's so much like a file with lines.  Makes it
	     * possible to swap the info and save on memory use.
	     */
	    slang->sl_sugbuf = open_spellbuf();
	    ಯದಿ (slang->sl_sugbuf == ರಿಕ್ತ)
		ಗಚ್ಛ someerror;
							    // <sugwcount>
	    wcount = get4c(fd);
	    ಯದಿ (wcount < 0)
		ಗಚ್ಛ someerror;

	    // Read all the wordnr lists into the buffer, one NUL terminated
	    // list per line.
	    ga_init2(&ga, 1, 100);
	    ಪ್ರತಿ (wordnr = 0; wordnr < wcount; ++wordnr)
	    {
		ga.ga_len = 0;
		ಪ್ರತಿ (;;)
		{
		    c = getc(fd);			    // <sugline>
		    ಯದಿ (c < 0 || ga_grow(&ga, 1) == FAIL)
			ಗಚ್ಛ someerror;
		    ((char_u *)ga.ga_data)[ga.ga_len++] = c;
		    ಯದಿ (c == NUL)
			ಭಙ್ಗ;
		}
		ಯದಿ (ml_append_buf(slang->sl_sugbuf, (linenr_T)wordnr,
					 ga.ga_data, ga.ga_len, TRUE) == FAIL)
		    ಗಚ್ಛ someerror;
	    }
	    ga_clear(&ga);

	    /*
	     * Need to put word counts in the word tries, so that we can find
	     * a word by its number.
	     */
	    ವೃಕ್ಷ_ಎಣಿಕೆ_ಪದಗಳು(slang->sl_fbyts, slang->sl_fbyts_len,
		    slang->sl_fidxs);
	    ವೃಕ್ಷ_ಎಣಿಕೆ_ಪದಗಳು(slang->sl_sbyts, slang->sl_sbyts_len,
		    slang->sl_sidxs);

nextone:
	    ಯದಿ (fd != ರಿಕ್ತ)
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
    ಸ್ಥಿರ char_u *
read_cnt_string(FILE *fd, ಪೂರ್ಣಾಙ್ಕ cnt_bytes, ಪೂರ್ಣಾಙ್ಕ *cntp)
{
    ಪೂರ್ಣಾಙ್ಕ		cnt = 0;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*str;

    // read the length bytes, MSB first
    ಪ್ರತಿ (i = 0; i < cnt_bytes; ++i)
    {
	ಪೂರ್ಣಾಙ್ಕ c = getc(fd);

	ಯದಿ (c == EOF)
	{
	    *cntp = SP_TRUNCERROR;
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	cnt = (cnt << 8) + (unsigned)c;
    }
    *cntp = cnt;
    ಯದಿ (cnt == 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;	    // nothing to read, return NULL

    str = read_string(fd, cnt);
    ಯದಿ (str == ರಿಕ್ತ)
	*cntp = SP_OTHERERROR;
    ಪ್ರತ್ಯಾಗಮ str;
}

/*
 * Read SN_REGION: <regionname> ...
 * Return SP_*ERROR flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_region_section(FILE *fd, slang_T *lp, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		c = 0;

    ಯದಿ (len > MAXREGIONS * 2)
	ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;
    ಪ್ರತಿ (i = 0; i < len; ++i)
	lp->sl_regions[i] = (c = getc(fd)) == EOF ? 0 : c; // <regionname>
    lp->sl_regions[len] = NUL;
    ಪ್ರತ್ಯಾಗಮ c == EOF ? SP_TRUNCERROR : 0;
}

/*
 * Read SN_CHARFLAGS section: <charflagslen> <charflags>
 *				<folcharslen> <folchars>
 * Return SP_*ERROR flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_charflags_section(FILE *fd)
{
    char_u	*flags;
    char_u	*fol;
    ಪೂರ್ಣಾಙ್ಕ		flagslen, follen;

    // <charflagslen> <charflags>
    flags = read_cnt_string(fd, 1, &flagslen);
    ಯದಿ (flagslen < 0)
	ಪ್ರತ್ಯಾಗಮ flagslen;

    // <folcharslen> <folchars>
    fol = read_cnt_string(fd, 2, &follen);
    ಯದಿ (follen < 0)
    {
	vim_free(flags);
	ಪ್ರತ್ಯಾಗಮ follen;
    }

    // Set the word-char flags and fill SPELL_ISUPPER() table.
    ಯದಿ (flags != ರಿಕ್ತ && fol != ರಿಕ್ತ)
	set_spell_charflags(flags, flagslen, fol);

    vim_free(flags);
    vim_free(fol);

    // When <charflagslen> is zero then <fcharlen> must also be zero.
    ಯದಿ ((flags == ರಿಕ್ತ) != (fol == ರಿಕ್ತ))
	ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Read SN_PREFCOND section.
 * Return SP_*ERROR flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_prefcond_section(FILE *fd, slang_T *lp)
{
    ಪೂರ್ಣಾಙ್ಕ		cnt;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		c;
    char_u	*p;
    char_u	buf[MAXWLEN + 1];

    // <prefcondcnt> <prefcond> ...
    cnt = get2c(fd);					// <prefcondcnt>
    ಯದಿ (cnt <= 0)
	ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;

    lp->sl_prefprog = ALLOC_CLEAR_MULT(regprog_T *, cnt);
    ಯದಿ (lp->sl_prefprog == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ SP_OTHERERROR;
    lp->sl_prefixcnt = cnt;

    ಪ್ರತಿ (i = 0; i < cnt; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	n = getc(fd);					// <condlen>
	ಯದಿ (n < 0 || n >= MAXWLEN)
	    ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;

	// When <condlen> is zero we have an empty condition.  Otherwise
	// compile the regexp program used to check for the condition.
	ಯದಿ (n > 0)
	{
	    buf[0] = '^';	    // always match at one position only
	    p = buf + 1;
	    ಯಾವತ್ (n-- > 0)
		*p++ = (c = getc(fd)) == EOF ? 0 : c;	    // <condstr>
	    ಯದಿ (c == EOF)
		ಭಙ್ಗ;
	    *p = NUL;
	    lp->sl_prefprog[i] = vim_regcomp(buf, RE_MAGIC + RE_STRING);
	}
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Read REP or REPSAL items section from "fd": <repcount> <rep> ...
 * Return SP_*ERROR flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_rep_section(FILE *fd, garray_T *gap, short *first)
{
    ಪೂರ್ಣಾಙ್ಕ		cnt;
    fromto_T	*ftp;
    ಪೂರ್ಣಾಙ್ಕ		i;

    cnt = get2c(fd);					// <repcount>
    ಯದಿ (cnt < 0)
	ಪ್ರತ್ಯಾಗಮ SP_TRUNCERROR;

    ಯದಿ (ga_grow(gap, cnt) == FAIL)
	ಪ್ರತ್ಯಾಗಮ SP_OTHERERROR;

    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
    ಪ್ರತಿ (; gap->ga_len < cnt; ++gap->ga_len)
    {
	ftp = &((fromto_T *)gap->ga_data)[gap->ga_len];
	ftp->ft_from = read_cnt_string(fd, 1, &i);
	ಯದಿ (i < 0)
	    ಪ್ರತ್ಯಾಗಮ i;
	ಯದಿ (i == 0)
	    ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;
	ftp->ft_to = read_cnt_string(fd, 1, &i);
	ಯದಿ (i <= 0)
	{
	    vim_free(ftp->ft_from);
	    ಯದಿ (i < 0)
		ಪ್ರತ್ಯಾಗಮ i;
	    ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;
	}
    }

    // Fill the first-index table.
    ಪ್ರತಿ (i = 0; i < 256; ++i)
	first[i] = -1;
    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
    {
	ftp = &((fromto_T *)gap->ga_data)[i];
	ಯದಿ (first[*ftp->ft_from] == -1)
	    first[*ftp->ft_from] = i;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Read SN_SAL section: <salflags> <salcount> <sal> ...
 * Return SP_*ERROR flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_sal_section(FILE *fd, slang_T *slang)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		cnt;
    garray_T	*gap;
    salitem_T	*smp;
    ಪೂರ್ಣಾಙ್ಕ		ccnt;
    char_u	*p;

    slang->sl_sofo = FALSE;

    i = getc(fd);				// <salflags>
    ಯದಿ (i & SAL_F0LLOWUP)
	slang->sl_followup = TRUE;
    ಯದಿ (i & SAL_COLLAPSE)
	slang->sl_collapse = TRUE;
    ಯದಿ (i & SAL_REM_ACCENTS)
	slang->sl_rem_accents = TRUE;

    cnt = get2c(fd);				// <salcount>
    ಯದಿ (cnt < 0)
	ಪ್ರತ್ಯಾಗಮ SP_TRUNCERROR;

    gap = &slang->sl_sal;
    ga_init2(gap, ಪರಿಮಾಣ(salitem_T), 10);
    ಯದಿ (ga_grow(gap, cnt + 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ SP_OTHERERROR;

    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
    ಪ್ರತಿ (; gap->ga_len < cnt; ++gap->ga_len)
    {
	ಪೂರ್ಣಾಙ್ಕ	c = NUL;

	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	ccnt = getc(fd);			// <salfromlen>
	ಯದಿ (ccnt < 0)
	    ಪ್ರತ್ಯಾಗಮ SP_TRUNCERROR;
	ಯದಿ ((p = alloc(ccnt + 2)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ SP_OTHERERROR;
	smp->sm_lead = p;

	// Read up to the first special char into sm_lead.
	ಪ್ರತಿ (i = 0; i < ccnt; ++i)
	{
	    c = getc(fd);			// <salfrom>
	    ಯದಿ (vim_strchr((char_u *)"0123456789(-<^$", c) != ರಿಕ್ತ)
		ಭಙ್ಗ;
	    *p++ = c;
	}
	smp->sm_leadlen = (ಪೂರ್ಣಾಙ್ಕ)(p - smp->sm_lead);
	*p++ = NUL;

	// Put (abc) chars in sm_oneof, if any.
	ಯದಿ (c == '(')
	{
	    smp->sm_oneof = p;
	    ಪ್ರತಿ (++i; i < ccnt; ++i)
	    {
		c = getc(fd);			// <salfrom>
		ಯದಿ (c == ')')
		    ಭಙ್ಗ;
		*p++ = c;
	    }
	    *p++ = NUL;
	    ಯದಿ (++i < ccnt)
		c = getc(fd);
	}
	ಅನ್ಯಥಾ
	    smp->sm_oneof = ರಿಕ್ತ;

	// Any following chars go in sm_rules.
	smp->sm_rules = p;
	ಯದಿ (i < ccnt)
	    // store the char we got while checking for end of sm_lead
	    *p++ = c;
	ಪ್ರತಿ (++i; i < ccnt; ++i)
	    *p++ = (c = getc(fd)) == EOF ? 0 : c;	// <salfrom>
	*p++ = NUL;

	// <saltolen> <salto>
	smp->sm_to = read_cnt_string(fd, 1, &ccnt);
	ಯದಿ (ccnt < 0)
	{
	    vim_free(smp->sm_lead);
	    ಪ್ರತ್ಯಾಗಮ ccnt;
	}

	ಯದಿ (has_mbyte)
	{
	    // convert the multi-byte strings to wide char strings
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = mb_charlen(smp->sm_lead);
	    ಯದಿ (smp->sm_oneof == ರಿಕ್ತ)
		smp->sm_oneof_w = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
		smp->sm_oneof_w = mb_str2wide(smp->sm_oneof);
	    ಯದಿ (smp->sm_to == ರಿಕ್ತ)
		smp->sm_to_w = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
		smp->sm_to_w = mb_str2wide(smp->sm_to);
	    ಯದಿ (smp->sm_lead_w == ರಿಕ್ತ
		    || (smp->sm_oneof_w == ರಿಕ್ತ && smp->sm_oneof != ರಿಕ್ತ)
		    || (smp->sm_to_w == ರಿಕ್ತ && smp->sm_to != ರಿಕ್ತ))
	    {
		vim_free(smp->sm_lead);
		vim_free(smp->sm_to);
		vim_free(smp->sm_lead_w);
		vim_free(smp->sm_oneof_w);
		vim_free(smp->sm_to_w);
		ಪ್ರತ್ಯಾಗಮ SP_OTHERERROR;
	    }
	}
    }

    ಯದಿ (gap->ga_len > 0)
    {
	// Add one extra entry to mark the end with an empty sm_lead.  Avoids
	// that we need to check the index every time.
	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	ಯದಿ ((p = alloc(1)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ SP_OTHERERROR;
	p[0] = NUL;
	smp->sm_lead = p;
	smp->sm_leadlen = 0;
	smp->sm_oneof = ರಿಕ್ತ;
	smp->sm_rules = p;
	smp->sm_to = ರಿಕ್ತ;
	ಯದಿ (has_mbyte)
	{
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = 0;
	    smp->sm_oneof_w = ರಿಕ್ತ;
	    smp->sm_to_w = ರಿಕ್ತ;
	}
	++gap->ga_len;
    }

    // Fill the first-index table.
    set_sal_first(slang);

    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Read SN_WORDS: <word> ...
 * Return SP_*ERROR flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_ಪದಗಳು_section(FILE *fd, slang_T *lp, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪೂರ್ಣಾಙ್ಕ		done = 0;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		c;
    char_u	word[MAXWLEN];

    ಯಾವತ್ (done < len)
    {
	// Read one word at a time.
	ಪ್ರತಿ (i = 0; ; ++i)
	{
	    c = getc(fd);
	    ಯದಿ (c == EOF)
		ಪ್ರತ್ಯಾಗಮ SP_TRUNCERROR;
	    word[i] = c;
	    ಯದಿ (word[i] == NUL)
		ಭಙ್ಗ;
	    ಯದಿ (i == MAXWLEN - 1)
		ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;
	}

	// Init the count to 10.
	ಎಣಿಕೆ_common_word(lp, word, -1, 10);
	done += i + 1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
 * Return SP_*ERROR flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_sofo_section(FILE *fd, slang_T *slang)
{
    ಪೂರ್ಣಾಙ್ಕ		cnt;
    char_u	*from, *to;
    ಪೂರ್ಣಾಙ್ಕ		res;

    // <sofofromlen> <sofofrom>
    from = read_cnt_string(fd, 2, &cnt);
    ಯದಿ (cnt < 0)
	ಪ್ರತ್ಯಾಗಮ cnt;

    // <sofotolen> <sofoto>
    to = read_cnt_string(fd, 2, &cnt);
    ಯದಿ (cnt < 0)
    {
	vim_free(from);
	ಪ್ರತ್ಯಾಗಮ cnt;
    }

    // Store the info in slang->sl_sal and/or slang->sl_sal_first.
    ಯದಿ (from != ರಿಕ್ತ && to != ರಿಕ್ತ)
	res = set_sofo(slang, from, to);
    ಅನ್ಯಥಾ ಯದಿ (from != ರಿಕ್ತ || to != ರಿಕ್ತ)
	res = SP_FORMERROR;    // only one of two strings is an error
    ಅನ್ಯಥಾ
	res = 0;

    vim_free(from);
    vim_free(to);
    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Read the compound section from the .spl file:
 *	<compmax> <compminlen> <compsylmax> <compoptions> <compflags>
 * Returns SP_*ERROR flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_compound(FILE *fd, slang_T *slang, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪೂರ್ಣಾಙ್ಕ		todo = len;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		atstart;
    char_u	*pat;
    char_u	*pp;
    char_u	*cp;
    char_u	*ap;
    char_u	*crp;
    ಪೂರ್ಣಾಙ್ಕ		cnt;
    garray_T	*gap;
    size_t	patsize;
    size_t	flagsize;

    ಯದಿ (todo < 2)
	ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;	// need at least two bytes

    --todo;
    c = getc(fd);					// <compmax>
    ಯದಿ (c < 2)
	c = MAXWLEN;
    slang->sl_compmax = c;

    --todo;
    c = getc(fd);					// <compminlen>
    ಯದಿ (c < 1)
	c = 0;
    slang->sl_compminlen = c;

    --todo;
    c = getc(fd);					// <compsylmax>
    ಯದಿ (c < 1)
	c = MAXWLEN;
    slang->sl_compsylmax = c;

    c = getc(fd);					// <compoptions>
    ಯದಿ (c != 0)
	ungetc(c, fd);	    // be backwards compatible with Vim 7.0b
    ಅನ್ಯಥಾ
    {
	--todo;
	c = getc(fd);	    // only use the lower byte for now
	--todo;
	slang->sl_compoptions = c;

	gap = &slang->sl_comppat;
	c = get2c(fd);					// <comppatcount>
	ಯದಿ (c < 0)
	    ಪ್ರತ್ಯಾಗಮ SP_TRUNCERROR;
	todo -= 2;
	ga_init2(gap, ಪರಿಮಾಣ(char_u *), c);
	ಯದಿ (ga_grow(gap, c) == OK)
	    ಯಾವತ್ (--c >= 0)
	    {
		((char_u **)(gap->ga_data))[gap->ga_len++] =
						  read_cnt_string(fd, 1, &cnt);
					    // <comppatlen> <comppattext>
		ಯದಿ (cnt < 0)
		    ಪ್ರತ್ಯಾಗಮ cnt;
		todo -= cnt + 1;
	    }
    }
    ಯದಿ (todo < 0)
	ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;

    // Turn the COMPOUNDRULE items into a regexp pattern:
    // "a[bc]/a*b+" -> "^\(a[bc]\|a*b\+\)$".
    // Inserting backslashes may double the length, "^\(\)$<Nul>" is 7 bytes.
    // Conversion to utf-8 may double the size.
    ಯದಿ ((size_t)todo > COMPOUND_MAX_LEN)
	ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;
    patsize = (size_t)todo * 2 + 7;
    ಯದಿ (enc_utf8)
	patsize += (size_t)todo * 2;
    flagsize = (size_t)todo + 1;
    pat = alloc(patsize);
    ಯದಿ (pat == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ SP_OTHERERROR;

    // We also need a list of all flags that can appear at the start and one
    // for all flags.
    cp = alloc(flagsize);
    ಯದಿ (cp == ರಿಕ್ತ)
    {
	vim_free(pat);
	ಪ್ರತ್ಯಾಗಮ SP_OTHERERROR;
    }
    slang->sl_compstartflags = cp;
    *cp = NUL;

    ap = alloc(flagsize);
    ಯದಿ (ap == ರಿಕ್ತ)
    {
	vim_free(pat);
	ಪ್ರತ್ಯಾಗಮ SP_OTHERERROR;
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
    ಯಾವತ್ (todo-- > 0)
    {
	c = getc(fd);					// <compflags>
	ಯದಿ (c == EOF)
	{
	    vim_free(pat);
	    ಪ್ರತ್ಯಾಗಮ SP_TRUNCERROR;
	}

	// Add all flags to "sl_compallflags".
	ಯದಿ (vim_strchr((char_u *)"?*+[]/", c) == ರಿಕ್ತ
		&& !byte_in_str(slang->sl_compallflags, c))
	{
	    *ap++ = c;
	    *ap = NUL;
	}

	ಯದಿ (atstart != 0)
	{
	    // At start of item: copy flags to "sl_compstartflags".  For a
	    // [abc] item set "atstart" to 2 and copy up to the ']'.
	    ಯದಿ (c == '[')
		atstart = 2;
	    ಅನ್ಯಥಾ ಯದಿ (c == ']')
		atstart = 0;
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (!byte_in_str(slang->sl_compstartflags, c))
		{
		    *cp++ = c;
		    *cp = NUL;
		}
		ಯದಿ (atstart == 1)
		    atstart = 0;
	    }
	}

	// Copy flag to "sl_comprules", unless we run into a wildcard.
	ಯದಿ (crp != ರಿಕ್ತ)
	{
	    ಯದಿ (c == '?' || c == '+' || c == '*')
	    {
		VIM_CLEAR(slang->sl_comprules);
		crp = ರಿಕ್ತ;
	    }
	    ಅನ್ಯಥಾ
		*crp++ = c;
	}

	ಯದಿ (c == '/')	    // slash separates two items
	{
	    *pp++ = '\\';
	    *pp++ = '|';
	    atstart = 1;
	}
	ಅನ್ಯಥಾ		    // normal char, "[abc]" and '*' are copied as-is
	{
	    ಯದಿ (c == '?' || c == '+' || c == '~')
		*pp++ = '\\';	    // "a?" becomes "a\?", "a+" becomes "a\+"
	    ಯದಿ (enc_utf8)
		pp += mb_char2bytes(c, pp);
	    ಅನ್ಯಥಾ
		*pp++ = c;
	}
    }

    *pp++ = '\\';
    *pp++ = ')';
    *pp++ = '$';
    *pp = NUL;

    ಯದಿ (crp != ರಿಕ್ತ)
	*crp = NUL;

    slang->sl_compprog = vim_regcomp(pat, RE_MAGIC + RE_STRING + RE_STRICT);
    vim_free(pat);
    ಯದಿ (slang->sl_compprog == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;

    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Set the SOFOFROM and SOFOTO items in language "lp".
 * Returns SP_*ERROR flags when there is something wrong.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_sofo(slang_T *lp, char_u *from, char_u *to)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    garray_T	*gap;
    char_u	*s;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		*inp;

    ಯದಿ (has_mbyte)
    {
	free_sal_ವಸ್ತುಗಳು(&lp->sl_sal);

	// Use "sl_sal" as an array with 256 pointers to a list of wide
	// characters.  The index is the low byte of the character.
	// The list contains from-to pairs with a terminating NUL.
	// sl_sal_first[] is used for latin1 "from" characters.
	gap = &lp->sl_sal;
	ga_init2(gap, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ *), 1);
	ಯದಿ (ga_grow(gap, 256) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ SP_OTHERERROR;
	vim_memset(gap->ga_data, 0, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ *) * 256);
	gap->ga_len = 256;
	lp->sl_sofo = TRUE;

	// First count the number of items for each list.  Temporarily use
	// sl_sal_first[] for this.  Reset it first: a preceding SN_SAL section
	// may have set the entries to -1 via set_sal_first().
	vim_memset(lp->sl_sal_first, 0, ಪರಿಮಾಣ(salfirst_T) * 256);
	ಪ್ರತಿ (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    MB_CPTR_ADV(s);
	    ಯದಿ (c >= 256)
		++lp->sl_sal_first[c & 0xff];
	}
	ಯದಿ (*p != NUL || *s != NUL)	    // lengths differ
	    ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;

	// Allocate the lists.
	ಪ್ರತಿ (i = 0; i < 256; ++i)
	    ಯದಿ (lp->sl_sal_first[i] > 0)
	    {
		p = alloc(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) * (lp->sl_sal_first[i] * 2 + 1));
		ಯದಿ (p == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ SP_OTHERERROR;
		((ಪೂರ್ಣಾಙ್ಕ **)gap->ga_data)[i] = (ಪೂರ್ಣಾಙ್ಕ *)p;
		*(ಪೂರ್ಣಾಙ್ಕ *)p = 0;
	    }

	// Put the characters up to 255 in sl_sal_first[] the rest in a sl_sal
	// list.
	vim_memset(lp->sl_sal_first, 0, ಪರಿಮಾಣ(salfirst_T) * 256);
	ಪ್ರತಿ (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    i = mb_cptr2char_adv(&s);
	    ಯದಿ (c >= 256)
	    {
		// Append the from-to chars at the end of the list with
		// the low byte.
		inp = ((ಪೂರ್ಣಾಙ್ಕ **)gap->ga_data)[c & 0xff];
		ಯಾವತ್ (*inp != 0)
		    ++inp;
		*inp++ = c;		// from char
		*inp++ = i;		// to char
		*inp++ = NUL;		// NUL at the end
	    }
	    ಅನ್ಯಥಾ
		// mapping byte to char is done in sl_sal_first[]
		lp->sl_sal_first[c] = i;
	}
    }
    ಅನ್ಯಥಾ
    {
	// mapping bytes to bytes is done in sl_sal_first[]
	ಯದಿ (STRLEN(from) != STRLEN(to))
	    ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;

	ಪ್ರತಿ (i = 0; to[i] != NUL; ++i)
	    lp->sl_sal_first[from[i]] = to[i];
	lp->sl_sal.ga_len = 1;		// indicates we have soundfolding
	lp->sl_sofo = TRUE;
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Fill the first-index table for "lp".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_sal_first(slang_T *lp)
{
    salfirst_T	*sfirst;
    ಪೂರ್ಣಾಙ್ಕ		i;
    salitem_T	*smp;
    ಪೂರ್ಣಾಙ್ಕ		c;
    garray_T	*gap = &lp->sl_sal;

    sfirst = lp->sl_sal_first;
    ಪ್ರತಿ (i = 0; i < 256; ++i)
	sfirst[i] = -1;
    smp = (salitem_T *)gap->ga_data;
    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
    {
	ಯದಿ (has_mbyte)
	    // Use the lowest byte of the first character.  For latin1 it's
	    // the character, for other encodings it should differ for most
	    // characters.
	    c = *smp[i].sm_lead_w & 0xff;
	ಅನ್ಯಥಾ
	    c = *smp[i].sm_lead;
	ಯದಿ (sfirst[c] == -1)
	{
	    sfirst[c] = i;
	    ಯದಿ (has_mbyte)
	    {
		ಪೂರ್ಣಾಙ್ಕ		n;

		// Make sure all entries with this byte are following each
		// other.  Move the ones that are in the wrong position.  Do
		// keep the same ordering!
		ಯಾವತ್ (i + 1 < gap->ga_len
				       && (*smp[i + 1].sm_lead_w & 0xff) == c)
		    // Skip over entry with same index byte.
		    ++i;

		ಪ್ರತಿ (n = 1; i + n < gap->ga_len; ++n)
		    ಯದಿ ((*smp[i + n].sm_lead_w & 0xff) == c)
		    {
			salitem_T  tsal;

			// Move entry with same index byte after the entries
			// we already found.
			++i;
			--n;
			tsal = smp[i + n];
			mch_memmove(smp + i + 1, smp + i,
						       ಪರಿಮಾಣ(salitem_T) * n);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ *
mb_str2wide(char_u *s)
{
    ಪೂರ್ಣಾಙ್ಕ		*res;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i = 0;

    res = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, mb_charlen(s) + 1);
    ಯದಿ (res == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (p = s; *p != NUL; )
	res[i++] = mb_ptr2char_adv(&p);
    res[i] = NUL;
    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Read a tree from the .spl or .sug file.
 * Allocates the memory and stores pointers in "bytsp" and "idxsp".
 * This is skipped when the tree has zero length.
 * Returns zero when OK, SP_ value for an error.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
spell_read_ವೃಕ್ಷ(
    FILE	*fd,
    char_u	**bytsp,
    long	*bytsp_len,
    idx_T	**idxsp,
    ಪೂರ್ಣಾಙ್ಕ		prefixtree,	// TRUE for the prefix tree
    ಪೂರ್ಣಾಙ್ಕ		prefixcnt)	// when "prefixtree" is TRUE: prefix count
{
    long	len;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    char_u	*bp;
    idx_T	*ip;

    // The tree size was computed when writing the file, so that we can
    // allocate it as one long block. <nodecount>
    len = get4c(fd);
    ಯದಿ (len < 0)
	ಪ್ರತ್ಯಾಗಮ SP_TRUNCERROR;
    ಯದಿ ((size_t)len > SIZE_MAX / ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ))
	// Invalid length, multiply with sizeof(int) would overflow.
	ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;
    ಯದಿ (len <= 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    // Allocate the byte array.  Zero-initialize so that any position the
    // tree does not visit reads as 0; a stray BY_INDEX shared reference
    // into such a slot then behaves as end-of-word in spellsuggest()
    // instead of consuming an arbitrary heap byte as a siblingcount.
    bp = alloc_clear(len);
    ಯದಿ (bp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ SP_OTHERERROR;
    *bytsp = bp;
    ಯದಿ (bytsp_len != ರಿಕ್ತ)
	*bytsp_len = len;

    // Allocate the index array.
    ip = lalloc_clear(len * ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ), TRUE);
    ಯದಿ (ip == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ SP_OTHERERROR;
    *idxsp = ip;

    // Recursively read the tree and store it in the array.
    idx = read_ವೃಕ್ಷ_ಗಂಟು(fd, bp, ip, len, 0, prefixtree, prefixcnt, 0);
    ಯದಿ (idx < 0)
	ಪ್ರತ್ಯಾಗಮ idx;
    ಯದಿ (idx != len)
	ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
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
    ಸ್ಥಿರ idx_T
read_ವೃಕ್ಷ_ಗಂಟು(
    FILE	*fd,
    char_u	*byts,
    idx_T	*idxs,
    ಪೂರ್ಣಾಙ್ಕ		maxidx,		    // size of arrays
    idx_T	startidx,	    // current index in "byts" and "idxs"
    ಪೂರ್ಣಾಙ್ಕ		prefixtree,	    // TRUE for reading PREFIXTREE
    ಪೂರ್ಣಾಙ್ಕ		maxprefcondnr,	    // maximum for <prefcondnr>
    ಪೂರ್ಣಾಙ್ಕ		depth)		    // recursion level
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		n;
    idx_T	idx = startidx;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		c2;
#define SHARED_MASK	0x8000000

    // Bail out on a crafted .spl whose tree recurses beyond the maximum
    // word length: each tree level corresponds to one byte of a word, so
    // any well-formed file has depth <= MAXWLEN.
    ಯದಿ (depth > MAXWLEN)
	ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;

    len = getc(fd);					// <siblingcount>
    ಯದಿ (len <= 0)
	ಪ್ರತ್ಯಾಗಮ SP_TRUNCERROR;

    ಯದಿ (len >= maxidx - startidx)
	ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;
    byts[idx++] = len;

    // Read the byte values, flag/region bytes and shared indexes.
    ಪ್ರತಿ (i = 1; i <= len; ++i)
    {
	c = getc(fd);					// <byte>
	ಯದಿ (c < 0)
	    ಪ್ರತ್ಯಾಗಮ SP_TRUNCERROR;
	ಯದಿ (c <= BY_SPECIAL)
	{
	    ಯದಿ (c == BY_NOFLAGS && !prefixtree)
	    {
		// No flags, all regions.
		idxs[idx] = 0;
		c = 0;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c != BY_INDEX)
	    {
		ಯದಿ (prefixtree)
		{
		    // Read the optional pflags byte, the prefix ID and the
		    // condition nr.  In idxs[] store the prefix ID in the low
		    // byte, the condition index shifted up 8 bits, the flags
		    // shifted up 24 bits.
		    ಯದಿ (c == BY_FLAGS)
		    {
			n = getc(fd);			// <pflags>
			ಯದಿ (n < 0)
			    ಪ್ರತ್ಯಾಗಮ SP_TRUNCERROR;
			c = n << 24;
		    }
		    ಅನ್ಯಥಾ
			c = 0;

		    n = getc(fd);			// <affixID>
		    ಯದಿ (n < 0)
			ಪ್ರತ್ಯಾಗಮ SP_TRUNCERROR;
		    c |= n;

		    n = get2c(fd);			// <prefcondnr>
		    ಯದಿ (n < 0 || n >= maxprefcondnr)
			ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;
		    c |= (n << 8);
		}
		ಅನ್ಯಥಾ // c must be BY_FLAGS or BY_FLAGS2
		{
		    // Read flags and optional region and prefix ID.  In
		    // idxs[] the flags go in the low two bytes, region above
		    // that and prefix ID above the region.
		    c2 = c;
		    c = getc(fd);			// <flags>
		    ಯದಿ (c < 0)
			ಪ್ರತ್ಯಾಗಮ SP_TRUNCERROR;
		    ಯದಿ (c2 == BY_FLAGS2)
		    {
			n = getc(fd);			// <flags2>
			ಯದಿ (n < 0)
			    ಪ್ರತ್ಯಾಗಮ SP_TRUNCERROR;
			c = (n << 8) + c;
		    }
		    ಯದಿ (c & WF_REGION)
		    {
			n = getc(fd);			// <region>
			ಯದಿ (n < 0)
			    ಪ್ರತ್ಯಾಗಮ SP_TRUNCERROR;
			c = (n << 16) + c;
		    }
		    ಯದಿ (c & WF_AFX)
		    {
			n = getc(fd);			// <affixID>
			ಯದಿ (n < 0)
			    ಪ್ರತ್ಯಾಗಮ SP_TRUNCERROR;
			c = (n << 24) + c;
		    }
		}

		idxs[idx] = c;
		c = 0;
	    }
	    ಅನ್ಯಥಾ // c == BY_INDEX
	    {
							// <nodeidx>
		n = get3c(fd);
		ಯದಿ (n < 0 || n >= maxidx)
		    ಪ್ರತ್ಯಾಗಮ SP_FORMERROR;
		idxs[idx] = n + SHARED_MASK;
		c = getc(fd);				// <xbyte>
	    }
	}
	byts[idx++] = c;
    }

    // Recursively read the children for non-shared siblings.
    // Skip the end-of-word ones (zero byte value) and the shared ones (and
    // remove SHARED_MASK)
    ಪ್ರತಿ (i = 1; i <= len; ++i)
	ಯದಿ (byts[startidx + i] != 0)
	{
	    ಯದಿ (idxs[startidx + i] & SHARED_MASK)
		idxs[startidx + i] &= ~SHARED_MASK;
	    ಅನ್ಯಥಾ
	    {
		idxs[startidx + i] = idx;
		idx = read_ವೃಕ್ಷ_ಗಂಟು(fd, byts, idxs, maxidx, idx,
					prefixtree, maxprefcondnr, depth + 1);
		ಯದಿ (idx < 0)
		    ಭಙ್ಗ;
	    }
	}

    ಪ್ರತ್ಯಾಗಮ idx;
}

/*
 * Reload the spell file "fname" if it's loaded.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_reload_one(
    char_u	*fname,
    ಪೂರ್ಣಾಙ್ಕ		added_word)	// invoked through "zg"
{
    slang_T	*slang;
    ಪೂರ್ಣಾಙ್ಕ		didit = FALSE;

    FOR_ALL_SPELL_LANGS(slang)
    {
	ಯದಿ (fullpathcmp(fname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	{
	    slang_clear(slang);
	    ಯದಿ (spell_load_file(fname, ರಿಕ್ತ, slang, FALSE) == ರಿಕ್ತ)
		// reloading failed, clear the language
		slang_clear(slang);
	    redraw_all_later(UPD_SOME_VALID);
	    didit = TRUE;
	}
    }

    // When "zg" was used and the file wasn't loaded yet, should redo
    // 'spelllang' to load it now.
    ಯದಿ (added_word && !didit)
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ afffile_S
{
    char_u	*af_enc;	// "SET", normalized, alloc'ed string or NULL
    ಪೂರ್ಣಾಙ್ಕ		af_flagtype;	// AFT_CHAR, AFT_LONG, AFT_NUM or AFT_CAPLONG
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
    ಪೂರ್ಣಾಙ್ಕ		af_pfxpostpone;	// postpone prefixes without chop string and
				// without flags
    ಪೂರ್ಣಾಙ್ಕ		af_ignoreextra;	// IGNOREEXTRA present
    hashtab_T	af_pref;	// hashtable for prefixes, affheader_T
    hashtab_T	af_suff;	// hashtable for suffixes, affheader_T
    hashtab_T	af_comp;	// hashtable for compound flags, compitem_T
} afffile_T;

#define AFT_CHAR	0	// flags are one character
#define AFT_LONG	1	// flags are two characters
#define AFT_CAPLONG	2	// flags are one or two characters
#define AFT_ಅಂಕೆ		3	// flags are numbers, comma separated

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ affentry_S affentry_T;
// Affix entry from ".aff" file.  Used for prefixes and suffixes.
ಸಂರಚನಾ affentry_S
{
    affentry_T	*ae_next;	// next affix with same name/number
    char_u	*ae_chop;	// text to chop off basic word (can be NULL)
    char_u	*ae_add;	// text to add to basic word (can be NULL)
    char_u	*ae_flags;	// flags on the affix (can be NULL)
    char_u	*ae_cond;	// condition (NULL for ".")
    regprog_T	*ae_prog;	// regexp program for ae_cond or NULL
    ವರ್ಣ	ae_compforbid;	// COMPOUNDFORBIDFLAG found
    ವರ್ಣ	ae_comppermit;	// COMPOUNDPERMITFLAG found
};

#define AH_ಕೀಲಿ_LEN 17		// 2 x 8 bytes + NUL

// Affix header from ".aff" file.  Used for af_pref and af_suff.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ affheader_S
{
    char_u	ah_ಕೀಲಿ[AH_ಕೀಲಿ_LEN]; // key for hashtab == name of affix
    unsigned	ah_flag;	// affix name as number, uses "af_flagtype"
    ಪೂರ್ಣಾಙ್ಕ		ah_newID;	// prefix ID after renumbering; 0 if not used
    ಪೂರ್ಣಾಙ್ಕ		ah_combine;	// suffix may combine with prefix
    ಪೂರ್ಣಾಙ್ಕ		ah_follows;	// another affix block should be following
    affentry_T	*ah_first;	// first affix entry
} affheader_T;

#define HI2AH(hi)   ((affheader_T *)(hi)->hi_ಕೀಲಿ)

// Flag used in compound items.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ compitem_S
{
    char_u	ci_ಕೀಲಿ[AH_ಕೀಲಿ_LEN]; // key for hashtab == name of compound
    unsigned	ci_flag;	// affix name as number, uses "af_flagtype"
    ಪೂರ್ಣಾಙ್ಕ		ci_newID;	// affix ID after renumbering.
} compitem_T;

#define HI2CI(hi)   ((compitem_T *)(hi)->hi_ಕೀಲಿ)

/*
 * Structure that is used to store the items in the word tree.  This avoids
 * the need to keep track of each allocated thing, everything is freed all at
 * once after ":mkspell" is done.
 * Note: "sb_next" must be just before "sb_data" to make sure the alignment of
 * "sb_data" is correct for systems where pointers must be aligned on
 * pointer-size boundaries and sizeof(pointer) > sizeof(int) (e.g., Sparc).
 */
#define  SBLOCKSIZE 16000	// size of sb_data
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ sblock_S sblock_T;
ಸಂರಚನಾ sblock_S
{
    ಪೂರ್ಣಾಙ್ಕ		sb_used;	// nr of bytes already in use
    sblock_T	*sb_next;	// next block in list
    char_u	sb_data[1];	// data, actually longer
};

/*
 * A node in the tree.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ wordnode_S wordnode_T;
ಸಂರಚನಾ wordnode_S
{
    ಸಙ್ಘ   // shared to save space
    {
	char_u	hashkey[6];	// the hash key, only used while compressing
	ಪೂರ್ಣಾಙ್ಕ	index;		// index in written nodes (valid after first
				// round)
    } wn_u1;
    ಸಙ್ಘ   // shared to save space
    {
	wordnode_T *next;	// next node with same hash key
	wordnode_T *wnode;	// parent node that will write this node
    } wn_u2;
    wordnode_T	*wn_child;	// child (next byte in word)
    wordnode_T  *wn_sibling;	// next sibling (alternate byte in word,
				// always sorted)
    ಪೂರ್ಣಾಙ್ಕ		wn_refs;	// Nr. of references to this node.  Only
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
    ಪೂರ್ಣಾಙ್ಕ		wn_nr;		// sequence nr for printing
#endif
};

#define WN_MASK	 0xffff		// mask relevant bits of "wn_flags"

#define HI2WN(hi)    (wordnode_T *)((hi)->hi_ಕೀಲಿ)

/*
 * Info used while reading the spell files.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ spellinfo_S
{
    wordnode_T	*si_foldroot;	// tree with case-folded words
    long	si_foldwcount;	// nr of words in si_foldroot

    wordnode_T	*si_keeproot;	// tree with keep-case words
    long	si_keepwcount;	// nr of words in si_keeproot

    wordnode_T	*si_prefroot;	// tree with postponed prefixes

    long	si_sugtree;	// creating the soundfolding trie

    sblock_T	*si_blocks;	// memory blocks used
    long	si_blocks_cnt;	// memory blocks allocated
    ಪೂರ್ಣಾಙ್ಕ		si_did_emsg;	// TRUE when ran out of memory

    long	si_compress_cnt;    // words to add before lowering
				    // compression limit
    wordnode_T	*si_first_free; // List of nodes that have been freed during
				// compression, linked by "wn_child" field.
    long	si_free_ಎಣಿಕೆ;	// number of nodes in si_first_free
#ifdef SPELL_PRINTTREE
    ಪೂರ್ಣಾಙ್ಕ		si_wordnode_nr;	// sequence nr for nodes
#endif
    buf_T	*si_spellbuf;	// buffer used to store soundfold word table

    ಪೂರ್ಣಾಙ್ಕ		si_ascii;	// handling only ASCII words
    ಪೂರ್ಣಾಙ್ಕ		si_add;		// addition file
    ಪೂರ್ಣಾಙ್ಕ		si_clear_chartab;   // when TRUE clear char tables
    ಪೂರ್ಣಾಙ್ಕ		si_region;	// region mask
    vimconv_T	si_conv;	// for conversion to 'encoding'
    ಪೂರ್ಣಾಙ್ಕ		si_memtot;	// runtime memory used
    ಪೂರ್ಣಾಙ್ಕ		si_verbose;	// verbose messages
    ಪೂರ್ಣಾಙ್ಕ		si_ಸಂದೇಶ_ಎಣಿಕೆ;	// number of words added since last message
    char_u	*si_info;	// info text chars or NULL
    ಪೂರ್ಣಾಙ್ಕ		si_region_ಎಣಿಕೆ; // number of regions supported (1 when there
				 // are no regions)
    char_u	si_region_name[MAXREGIONS * 2 + 1];
				// region names; used only if
				// si_region_count > 1)

    garray_T	si_rep;		// list of fromto_T entries from REP lines
    garray_T	si_repsal;	// list of fromto_T entries from REPSAL lines
    garray_T	si_sal;		// list of fromto_T entries from SAL lines
    char_u	*si_sofofr;	// SOFOFROM text
    char_u	*si_sofoto;	// SOFOTO text
    ಪೂರ್ಣಾಙ್ಕ		si_nosugfile;	// NOSUGFILE item found
    ಪೂರ್ಣಾಙ್ಕ		si_nosplitsugs;	// NOSPLITSUGS item found
    ಪೂರ್ಣಾಙ್ಕ		si_nocompoundsugs; // NOCOMPOUNDSUGS item found
    ಪೂರ್ಣಾಙ್ಕ		si_followup;	// soundsalike: ?
    ಪೂರ್ಣಾಙ್ಕ		si_collapse;	// soundsalike: ?
    hashtab_T	si_commonwords;	// hashtable for common words
    time_t	si_sugtime;	// timestamp for .sug file
    ಪೂರ್ಣಾಙ್ಕ		si_rem_accents;	// soundsalike: remove accents
    garray_T	si_map;		// MAP info concatenated
    char_u	*si_midword;	// MIDWORD chars or NULL
    ಪೂರ್ಣಾಙ್ಕ		si_compmax;	// max nr of words for compounding
    ಪೂರ್ಣಾಙ್ಕ		si_compminlen;	// minimal length for compounding
    ಪೂರ್ಣಾಙ್ಕ		si_compsylmax;	// max nr of syllables for compounding
    ಪೂರ್ಣಾಙ್ಕ		si_compoptions;	// COMP_ flags
    garray_T	si_comppat;	// CHECKCOMPOUNDPATTERN items, each stored as
				// a string
    char_u	*si_compflags;	// flags used for compounding
    char_u	si_nobreak;	// NOBREAK
    char_u	*si_syllable;	// syllable string
    garray_T	si_prefcond;	// table with conditions for postponed
				// prefixes, each stored as a string
    ಪೂರ್ಣಾಙ್ಕ		si_newprefID;	// current value for ah_newID
    ಪೂರ್ಣಾಙ್ಕ		si_newcompID;	// current value for compound ID
} spellinfo_T;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ is_aff_rule(char_u **ವಸ್ತುಗಳು, ಪೂರ್ಣಾಙ್ಕ itemcnt, ವರ್ಣ *rulename, ಪೂರ್ಣಾಙ್ಕ	 mincount);
ಸ್ಥಿರ ಶೂನ್ಯ aff_process_flags(afffile_T *affile, affentry_T *entry);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ spell_info_item(char_u *s);
ಸ್ಥಿರ unsigned affitem2flag(ಪೂರ್ಣಾಙ್ಕ flagtype, char_u *item, char_u	*fname, ಪೂರ್ಣಾಙ್ಕ lnum);
ಸ್ಥಿರ unsigned get_affitem(ಪೂರ್ಣಾಙ್ಕ flagtype, char_u **pp);
ಸ್ಥಿರ ಶೂನ್ಯ process_compflags(spellinfo_T *spin, afffile_T *aff, char_u *compflags);
ಸ್ಥಿರ ಶೂನ್ಯ check_renumber(spellinfo_T *spin);
ಸ್ಥಿರ ಶೂನ್ಯ aff_check_ಸಂಖ್ಯೆ(ಪೂರ್ಣಾಙ್ಕ spinval, ಪೂರ್ಣಾಙ್ಕ affval, ವರ್ಣ *name);
ಸ್ಥಿರ ಶೂನ್ಯ aff_check_string(char_u *spinval, char_u *affval, ವರ್ಣ *name);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ str_equal(char_u *s1, char_u	*s2);
ಸ್ಥಿರ ಶೂನ್ಯ add_fromto(spellinfo_T *spin, garray_T *gap, char_u	*from, char_u *to);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sal_to_bool(char_u *s);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_affix_flags(afffile_T *affile, char_u *afflist);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_pfxlist(afffile_T *affile, char_u *afflist, char_u *store_afflist, ಪೂರ್ಣಾಙ್ಕ *cntp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_compflags(afffile_T *affile, char_u *afflist, char_u *store_afflist, ಪೂರ್ಣಾಙ್ಕ *cntp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ store_aff_word(spellinfo_T *spin, char_u *word, char_u *afflist, afffile_T *affile, hashtab_T *ht, hashtab_T *xht, ಪೂರ್ಣಾಙ್ಕ condit, ಪೂರ್ಣಾಙ್ಕ flags, char_u *pfxlist, ಪೂರ್ಣಾಙ್ಕ pfxlen);
ಸ್ಥಿರ ಶೂನ್ಯ *getroom(spellinfo_T *spin, size_t len, ಪೂರ್ಣಾಙ್ಕ align);
ಸ್ಥಿರ char_u *getroom_save(spellinfo_T *spin, char_u *s);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ store_word(spellinfo_T *spin, char_u *word, ಪೂರ್ಣಾಙ್ಕ flags, ಪೂರ್ಣಾಙ್ಕ region, char_u *pfxlist, ಪೂರ್ಣಾಙ್ಕ need_affix);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ವೃಕ್ಷ_add_word(spellinfo_T *spin, char_u *word, wordnode_T *ವೃಕ್ಷ, ಪೂರ್ಣಾಙ್ಕ flags, ಪೂರ್ಣಾಙ್ಕ region, ಪೂರ್ಣಾಙ್ಕ affixID);
ಸ್ಥಿರ wordnode_T *get_wordnode(spellinfo_T *spin);
ಸ್ಥಿರ ಶೂನ್ಯ free_wordnode(spellinfo_T *spin, wordnode_T *n);
ಸ್ಥಿರ ಶೂನ್ಯ wordtree_compress(spellinfo_T *spin, wordnode_T *ಬೇರು, ವರ್ಣ *name);
ಸ್ಥಿರ long ಗಂಟು_compress(spellinfo_T *spin, wordnode_T *ಗಂಟು, hashtab_T *ht, long *tot);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಗಂಟು_equal(wordnode_T *n1, wordnode_T *n2);
ಸ್ಥಿರ ಶೂನ್ಯ clear_ಗಂಟು(wordnode_T *ಗಂಟು);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ put_ಗಂಟು(FILE *fd, wordnode_T *ಗಂಟು, ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ regionmask, ಪೂರ್ಣಾಙ್ಕ prefixtree);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sug_filltree(spellinfo_T *spin, slang_T *slang);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sug_maketable(spellinfo_T *spin);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sug_filltable(spellinfo_T *spin, wordnode_T *ಗಂಟು, ಪೂರ್ಣಾಙ್ಕ startwordnr, garray_T *gap);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ offset2bytes(ಪೂರ್ಣಾಙ್ಕ nr, char_u *buf);
ಸ್ಥಿರ ಶೂನ್ಯ sug_write(spellinfo_T *spin, char_u *fname);
ಸ್ಥಿರ ಶೂನ್ಯ spell_message(spellinfo_T *spin, char_u *str);
ಸ್ಥಿರ ಶೂನ್ಯ init_spellfile(ಶೂನ್ಯ);

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
ಸ್ಥಿರ long compress_start = 30000;	// memory / SBLOCKSIZE
ಸ್ಥಿರ long compress_inc = 100;		// memory / SBLOCKSIZE
ಸ್ಥಿರ long compress_added = 500000;	// word count

/*
 * Check the 'mkspellmem' option.  Return FAIL if it's wrong.
 * Sets "sps_flags".
 */
    ಪೂರ್ಣಾಙ್ಕ
spell_check_msm(ಶೂನ್ಯ)
{
    char_u	*p = p_msm;
    long	start = 0;
    long	incr = 0;
    long	added = 0;

    ಯದಿ (!VIM_ISDIGIT(*p))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    // block count = (value * 1024) / SBLOCKSIZE (but avoid overflow)
    start = (getdigits(&p) * 10) / (SBLOCKSIZE / 102);
    ಯದಿ (*p != ',')
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ++p;
    ಯದಿ (!VIM_ISDIGIT(*p))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    incr = (getdigits(&p) * 102) / (SBLOCKSIZE / 10);
    ಯದಿ (*p != ',')
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ++p;
    ಯದಿ (!VIM_ISDIGIT(*p))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    added = getdigits(&p) * 1024;
    ಯದಿ (*p != NUL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (start == 0 || incr == 0 || added == 0 || incr > start)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    compress_start = start;
    compress_inc = incr;
    compress_added = added;
    ಪ್ರತ್ಯಾಗಮ OK;
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

ಸ್ಥಿರ ವರ್ಣ line1[PRINTLINESIZE];
ಸ್ಥಿರ ವರ್ಣ line2[PRINTLINESIZE];
ಸ್ಥಿರ ವರ್ಣ line3[PRINTLINESIZE];

    ಸ್ಥಿರ ಶೂನ್ಯ
spell_clear_flags(wordnode_T *ಗಂಟು)
{
    wordnode_T	*np;

    FOR_ALL_ಗಂಟು_SIBLINGS(ಗಂಟು, np)
    {
	np->wn_u1.index = FALSE;
	spell_clear_flags(np->wn_child);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
spell_print_ಗಂಟು(wordnode_T *ಗಂಟು, ಪೂರ್ಣಾಙ್ಕ depth)
{
    ಯದಿ (ಗಂಟು->wn_u1.index)
    {
	// Done this node before, print the reference.
	PRINTSOME(line1, depth, "(%d)", ಗಂಟು->wn_nr, 0);
	PRINTSOME(line2, depth, "    ", 0, 0);
	PRINTSOME(line3, depth, "    ", 0, 0);
	ಸಂದೇಶ(line1);
	ಸಂದೇಶ(line2);
	ಸಂದೇಶ(line3);
    }
    ಅನ್ಯಥಾ
    {
	ಗಂಟು->wn_u1.index = TRUE;

	ಯದಿ (ಗಂಟು->wn_byte != NUL)
	{
	    ಯದಿ (ಗಂಟು->wn_child != ರಿಕ್ತ)
		PRINTSOME(line1, depth, " %c -> ", ಗಂಟು->wn_byte, 0);
	    ಅನ್ಯಥಾ
		// Cannot happen?
		PRINTSOME(line1, depth, " %c ???", ಗಂಟು->wn_byte, 0);
	}
	ಅನ್ಯಥಾ
	    PRINTSOME(line1, depth, " $    ", 0, 0);

	PRINTSOME(line2, depth, "%d/%d    ", ಗಂಟು->wn_nr, ಗಂಟು->wn_refs);

	ಯದಿ (ಗಂಟು->wn_sibling != ರಿಕ್ತ)
	    PRINTSOME(line3, depth, " |    ", 0, 0);
	ಅನ್ಯಥಾ
	    PRINTSOME(line3, depth, "      ", 0, 0);

	ಯದಿ (ಗಂಟು->wn_byte == NUL)
	{
	    ಸಂದೇಶ(line1);
	    ಸಂದೇಶ(line2);
	    ಸಂದೇಶ(line3);
	}

	// do the children
	ಯದಿ (ಗಂಟು->wn_byte != NUL && ಗಂಟು->wn_child != ರಿಕ್ತ)
	    spell_print_ಗಂಟು(ಗಂಟು->wn_child, depth + 1);

	// do the siblings
	ಯದಿ (ಗಂಟು->wn_sibling != ರಿಕ್ತ)
	{
	    // get rid of all parent details except |
	    STRCPY(line1, line3);
	    STRCPY(line2, line3);
	    spell_print_ಗಂಟು(ಗಂಟು->wn_sibling, depth);
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
spell_print_ವೃಕ್ಷ(wordnode_T *ಬೇರು)
{
    ಯದಿ (ಬೇರು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Clear the "wn_u1.index" fields, used to remember what has been done.
    spell_clear_flags(ಬೇರು);

    // Recursively print the tree.
    spell_print_ಗಂಟು(ಬೇರು, 0);
}
#endif // SPELL_PRINTTREE

/*
 * Read the affix file "fname".
 * Returns an afffile_T, NULL for complete failure.
 */
    ಸ್ಥಿರ afffile_T *
spell_read_aff(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    afffile_T	*aff;
    char_u	rline[MAXLINELEN];
    char_u	*ಸಾಲು;
    char_u	*pc = ರಿಕ್ತ;
#define MAXITEMCNT  30
    char_u	*(ವಸ್ತುಗಳು[MAXITEMCNT]);
    ಪೂರ್ಣಾಙ್ಕ		itemcnt;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		lnum = 0;
    affheader_T	*cur_aff = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		did_postpone_prefix = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		aff_todo = 0;
    hashtab_T	*tp;
    char_u	*ಕೆಳಗಿನ = ರಿಕ್ತ;
    char_u	*fol = ರಿಕ್ತ;
    char_u	*upp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		do_rep;
    ಪೂರ್ಣಾಙ್ಕ		do_repsal;
    ಪೂರ್ಣಾಙ್ಕ		do_sal;
    ಪೂರ್ಣಾಙ್ಕ		do_mapline;
    ಪೂರ್ಣಾಙ್ಕ		found_map = FALSE;
    hashitem_T	*hi;
    ಪೂರ್ಣಾಙ್ಕ		l;
    ಪೂರ್ಣಾಙ್ಕ		compminlen = 0;		// COMPOUNDMIN value
    ಪೂರ್ಣಾಙ್ಕ		compsylmax = 0;		// COMPOUNDSYLMAX value
    ಪೂರ್ಣಾಙ್ಕ		compoptions = 0;	// COMP_ flags
    ಪೂರ್ಣಾಙ್ಕ		compmax = 0;		// COMPOUNDWORDMAX value
    char_u	*compflags = ರಿಕ್ತ;	// COMPOUNDFLAG and COMPOUNDRULE
					// concatenated
    char_u	*midword = ರಿಕ್ತ;	// MIDWORD value
    char_u	*syllable = ರಿಕ್ತ;	// SYLLABLE value
    char_u	*sofofrom = ರಿಕ್ತ;	// SOFOFROM value
    char_u	*sofoto = ರಿಕ್ತ;		// SOFOTO value

    /*
     * Open the file.
     */
    fd = mch_fopen((ವರ್ಣ *)fname, "r");
    ಯದಿ (fd == ರಿಕ್ತ)
    {
	semsg(_(e_cant_open_file_str), fname);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, _("Reading affix file %s..."), fname);
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
    aff = (afffile_T *)getroom(spin, ಪರಿಮಾಣ(afffile_T), TRUE);
    ಯದಿ (aff == ರಿಕ್ತ)
    {
	fclose(fd);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    hash_init(&aff->af_pref);
    hash_init(&aff->af_suff);
    hash_init(&aff->af_comp);

    /*
     * Read all the lines in the file one by one.
     */
    ಯಾವತ್ (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	ಸಾಲು_breakcheck();
	++lnum;

	// Skip comment lines.
	ಯದಿ (*rline == '#')
	    ಅನುವರ್ತ;

	// Convert from "SET" to 'encoding' when needed.
	vim_free(pc);
	ಯದಿ (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, ರಿಕ್ತ);
	    ಯದಿ (pc == ರಿಕ್ತ)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
							   fname, lnum, rline);
		ಅನುವರ್ತ;
	    }
	    ಸಾಲು = pc;
	}
	ಅನ್ಯಥಾ
	{
	    pc = ರಿಕ್ತ;
	    ಸಾಲು = rline;
	}

	// Split the line up in white separated items.  Put a NUL after each
	// item.
	itemcnt = 0;
	ಪ್ರತಿ (p = ಸಾಲು; ; )
	{
	    ಯಾವತ್ (*p != NUL && *p <= ' ')  // skip white space and CR/NL
		++p;
	    ಯದಿ (*p == NUL)
		ಭಙ್ಗ;
	    ಯದಿ (itemcnt == MAXITEMCNT)	    // too many items
		ಭಙ್ಗ;
	    ವಸ್ತುಗಳು[itemcnt++] = p;
	    // A few items have arbitrary text argument, don't split them.
	    ಯದಿ (itemcnt == 2 && spell_info_item(ವಸ್ತುಗಳು[0]))
		ಯಾವತ್ (*p >= ' ' || *p == TAB)    // skip until CR/NL
		    ++p;
	    ಅನ್ಯಥಾ
		ಯಾವತ್ (*p > ' ')    // skip until white space or CR/NL
		    ++p;
	    ಯದಿ (*p == NUL)
		ಭಙ್ಗ;
	    *p++ = NUL;
	}

	// Handle non-empty lines.
	ಯದಿ (itemcnt > 0)
	{
	    ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "SET", 2) && aff->af_enc == ರಿಕ್ತ)
	    {
		// Setup for conversion from "ENC" to 'encoding'.
		aff->af_enc = enc_canonize(ವಸ್ತುಗಳು[1]);
		ಯದಿ (aff->af_enc != ರಿಕ್ತ && !spin->si_ascii
			&& convert_setup(&spin->si_conv, aff->af_enc,
							       p_enc) == FAIL)
		    smsg(_("Conversion in %s not supported: from %s to %s"),
					       fname, aff->af_enc, p_enc);
		spin->si_conv.vc_fail = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "FLAG", 2)
					      && aff->af_flagtype == AFT_CHAR)
	    {
		ಯದಿ (STRCMP(ವಸ್ತುಗಳು[1], "long") == 0)
		    aff->af_flagtype = AFT_LONG;
		ಅನ್ಯಥಾ ಯದಿ (STRCMP(ವಸ್ತುಗಳು[1], "num") == 0)
		    aff->af_flagtype = AFT_ಅಂಕೆ;
		ಅನ್ಯಥಾ ಯದಿ (STRCMP(ವಸ್ತುಗಳು[1], "caplong") == 0)
		    aff->af_flagtype = AFT_CAPLONG;
		ಅನ್ಯಥಾ
		    smsg(_("Invalid value for FLAG in %s line %d: %s"),
			    fname, lnum, ವಸ್ತುಗಳು[1]);
		ಯದಿ (aff->af_rare != 0
			|| aff->af_keepcase != 0
			|| aff->af_bad != 0
			|| aff->af_needaffix != 0
			|| aff->af_circumfix != 0
			|| aff->af_needcomp != 0
			|| aff->af_comproot != 0
			|| aff->af_nosuggest != 0
			|| compflags != ರಿಕ್ತ
			|| aff->af_suff.ht_used > 0
			|| aff->af_pref.ht_used > 0)
		    smsg(_("FLAG after using flags in %s line %d: %s"),
			    fname, lnum, ವಸ್ತುಗಳು[1]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (spell_info_item(ವಸ್ತುಗಳು[0]))
	    {
		    p = (char_u *)getroom(spin,
			    (spin->si_info == ರಿಕ್ತ ? 0 : STRLEN(spin->si_info))
			    + STRLEN(ವಸ್ತುಗಳು[0])
			    + STRLEN(ವಸ್ತುಗಳು[1]) + 3, FALSE);
		    ಯದಿ (p != ರಿಕ್ತ)
		    {
			ಯದಿ (spin->si_info != ರಿಕ್ತ)
			{
			    STRCPY(p, spin->si_info);
			    STRCAT(p, "\n");
			}
			STRCAT(p, ವಸ್ತುಗಳು[0]);
			STRCAT(p, " ");
			STRCAT(p, ವಸ್ತುಗಳು[1]);
			spin->si_info = p;
		    }
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "MIDWORD", 2)
							   && midword == ರಿಕ್ತ)
	    {
		midword = getroom_save(spin, ವಸ್ತುಗಳು[1]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "TRY", 2))
	    {
		// ignored, we look in the tree for what chars may appear
	    }
	    // TODO: remove "RAR" later
	    ಅನ್ಯಥಾ ಯದಿ ((is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "RAR", 2)
			|| is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "RARE", 2))
							 && aff->af_rare == 0)
	    {
		aff->af_rare = affitem2flag(aff->af_flagtype, ವಸ್ತುಗಳು[1],
								 fname, lnum);
	    }
	    // TODO: remove "KEP" later
	    ಅನ್ಯಥಾ ಯದಿ ((is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "KEP", 2)
			|| is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "KEEPCASE", 2))
						     && aff->af_keepcase == 0)
	    {
		aff->af_keepcase = affitem2flag(aff->af_flagtype, ವಸ್ತುಗಳು[1],
								 fname, lnum);
	    }
	    ಅನ್ಯಥಾ ಯದಿ ((is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "BAD", 2)
			|| is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "FORBIDDENWORD", 2))
							  && aff->af_bad == 0)
	    {
		aff->af_bad = affitem2flag(aff->af_flagtype, ವಸ್ತುಗಳು[1],
								 fname, lnum);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "NEEDAFFIX", 2)
						    && aff->af_needaffix == 0)
	    {
		aff->af_needaffix = affitem2flag(aff->af_flagtype, ವಸ್ತುಗಳು[1],
								 fname, lnum);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "CIRCUMFIX", 2)
						    && aff->af_circumfix == 0)
	    {
		aff->af_circumfix = affitem2flag(aff->af_flagtype, ವಸ್ತುಗಳು[1],
								 fname, lnum);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "NOSUGGEST", 2)
						    && aff->af_nosuggest == 0)
	    {
		aff->af_nosuggest = affitem2flag(aff->af_flagtype, ವಸ್ತುಗಳು[1],
								 fname, lnum);
	    }
	    ಅನ್ಯಥಾ ಯದಿ ((is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "NEEDCOMPOUND", 2)
			|| is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "ONLYINCOMPOUND", 2))
						     && aff->af_needcomp == 0)
	    {
		aff->af_needcomp = affitem2flag(aff->af_flagtype, ವಸ್ತುಗಳು[1],
								 fname, lnum);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "COMPOUNDROOT", 2)
						     && aff->af_comproot == 0)
	    {
		aff->af_comproot = affitem2flag(aff->af_flagtype, ವಸ್ತುಗಳು[1],
								 fname, lnum);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "COMPOUNDFORBIDFLAG", 2)
						   && aff->af_compforbid == 0)
	    {
		aff->af_compforbid = affitem2flag(aff->af_flagtype, ವಸ್ತುಗಳು[1],
								 fname, lnum);
		ಯದಿ (aff->af_pref.ht_used > 0)
		    smsg(_("Defining COMPOUNDFORBIDFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "COMPOUNDPERMITFLAG", 2)
						   && aff->af_comppermit == 0)
	    {
		aff->af_comppermit = affitem2flag(aff->af_flagtype, ವಸ್ತುಗಳು[1],
								 fname, lnum);
		ಯದಿ (aff->af_pref.ht_used > 0)
		    smsg(_("Defining COMPOUNDPERMITFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "COMPOUNDFLAG", 2)
							 && compflags == ರಿಕ್ತ)
	    {
		// Turn flag "c" into COMPOUNDRULE compatible string "c+",
		// "Na" into "Na+", "1234" into "1234+".
		p = getroom(spin, STRLEN(ವಸ್ತುಗಳು[1]) + 2, FALSE);
		ಯದಿ (p != ರಿಕ್ತ)
		{
		    STRCPY(p, ವಸ್ತುಗಳು[1]);
		    STRCAT(p, "+");
		    compflags = p;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "COMPOUNDRULES", 2))
	    {
		// We don't use the count, but do check that it's a number and
		// not COMPOUNDRULE mistyped.
		ಯದಿ (atoi((ವರ್ಣ *)ವಸ್ತುಗಳು[1]) == 0)
		    smsg(_("Wrong COMPOUNDRULES value in %s line %d: %s"),
						       fname, lnum, ವಸ್ತುಗಳು[1]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "COMPOUNDRULE", 2))
	    {
		// Don't use the first rule if it is a number.
		ಯದಿ (compflags != ರಿಕ್ತ || *skipdigits(ವಸ್ತುಗಳು[1]) != NUL)
		{
		    // Concatenate this string to previously defined ones,
		    // using a slash to separate them.
		    l = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ವಸ್ತುಗಳು[1]) + 1;
		    ಯದಿ (compflags != ರಿಕ್ತ)
			l += (ಪೂರ್ಣಾಙ್ಕ)STRLEN(compflags) + 1;
		    p = getroom(spin, l, FALSE);
		    ಯದಿ (p != ರಿಕ್ತ)
		    {
			ಯದಿ (compflags != ರಿಕ್ತ)
			{
			    STRCPY(p, compflags);
			    STRCAT(p, "/");
			}
			STRCAT(p, ವಸ್ತುಗಳು[1]);
			compflags = p;
		    }
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "COMPOUNDWORDMAX", 2)
							      && compmax == 0)
	    {
		compmax = atoi((ವರ್ಣ *)ವಸ್ತುಗಳು[1]);
		ಯದಿ (compmax == 0)
		    smsg(_("Wrong COMPOUNDWORDMAX value in %s line %d: %s"),
						       fname, lnum, ವಸ್ತುಗಳು[1]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "COMPOUNDMIN", 2)
							   && compminlen == 0)
	    {
		compminlen = atoi((ವರ್ಣ *)ವಸ್ತುಗಳು[1]);
		ಯದಿ (compminlen == 0)
		    smsg(_("Wrong COMPOUNDMIN value in %s line %d: %s"),
						       fname, lnum, ವಸ್ತುಗಳು[1]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "COMPOUNDSYLMAX", 2)
							   && compsylmax == 0)
	    {
		compsylmax = atoi((ವರ್ಣ *)ವಸ್ತುಗಳು[1]);
		ಯದಿ (compsylmax == 0)
		    smsg(_("Wrong COMPOUNDSYLMAX value in %s line %d: %s"),
						       fname, lnum, ವಸ್ತುಗಳು[1]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "CHECKCOMPOUNDDUP", 1))
	    {
		compoptions |= COMP_CHECKDUP;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "CHECKCOMPOUNDREP", 1))
	    {
		compoptions |= COMP_CHECKREP;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "CHECKCOMPOUNDCASE", 1))
	    {
		compoptions |= COMP_CHECKCASE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "CHECKCOMPOUNDTRIPLE", 1))
	    {
		compoptions |= COMP_CHECKTRIPLE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "CHECKCOMPOUNDPATTERN", 2))
	    {
		ಯದಿ (atoi((ವರ್ಣ *)ವಸ್ತುಗಳು[1]) == 0)
		    smsg(_("Wrong CHECKCOMPOUNDPATTERN value in %s line %d: %s"),
						       fname, lnum, ವಸ್ತುಗಳು[1]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "CHECKCOMPOUNDPATTERN", 3))
	    {
		garray_T    *gap = &spin->si_comppat;
		ಪೂರ್ಣಾಙ್ಕ	    i;

		// Only add the couple if it isn't already there.
		ಪ್ರತಿ (i = 0; i < gap->ga_len - 1; i += 2)
		    ಯದಿ (STRCMP(((char_u **)(gap->ga_data))[i], ವಸ್ತುಗಳು[1]) == 0
			    && STRCMP(((char_u **)(gap->ga_data))[i + 1],
							       ವಸ್ತುಗಳು[2]) == 0)
			ಭಙ್ಗ;
		ಯದಿ (i >= gap->ga_len && ga_grow(gap, 2) == OK)
		{
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, ವಸ್ತುಗಳು[1]);
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, ವಸ್ತುಗಳು[2]);
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "SYLLABLE", 2)
							  && syllable == ರಿಕ್ತ)
	    {
		syllable = getroom_save(spin, ವಸ್ತುಗಳು[1]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "NOBREAK", 1))
	    {
		spin->si_nobreak = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "NOSPLITSUGS", 1))
	    {
		spin->si_nosplitsugs = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "NOCOMPOUNDSUGS", 1))
	    {
		spin->si_nocompoundsugs = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "NOSUGFILE", 1))
	    {
		spin->si_nosugfile = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "PFXPOSTPONE", 1))
	    {
		aff->af_pfxpostpone = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "IGNOREEXTRA", 1))
	    {
		aff->af_ignoreextra = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ ((STRCMP(ವಸ್ತುಗಳು[0], "PFX") == 0
					      || STRCMP(ವಸ್ತುಗಳು[0], "SFX") == 0)
		    && aff_todo == 0
		    && itemcnt >= 4)
	    {
		ಪೂರ್ಣಾಙ್ಕ	lasti = 4;
		char_u	ಕೀಲಿ[AH_ಕೀಲಿ_LEN];

		ಯದಿ (*ವಸ್ತುಗಳು[0] == 'P')
		    tp = &aff->af_pref;
		ಅನ್ಯಥಾ
		    tp = &aff->af_suff;

		// Myspell allows the same affix name to be used multiple
		// times.  The affix files that do this have an undocumented
		// "S" flag on all but the last block, thus we check for that
		// and store it in ah_follows.
		vim_strncpy(ಕೀಲಿ, ವಸ್ತುಗಳು[1], AH_ಕೀಲಿ_LEN - 1);
		hi = hash_find(tp, ಕೀಲಿ);
		ಯದಿ (!HASHITEM_EMPTY(hi))
		{
		    cur_aff = HI2AH(hi);
		    ಯದಿ (cur_aff->ah_combine != (*ವಸ್ತುಗಳು[2] == 'Y'))
			smsg(_("Different combining flag in continued affix block in %s line %d: %s"),
						   fname, lnum, ವಸ್ತುಗಳು[1]);
		    ಯದಿ (!cur_aff->ah_follows)
			smsg(_("Duplicate affix in %s line %d: %s"),
						       fname, lnum, ವಸ್ತುಗಳು[1]);
		}
		ಅನ್ಯಥಾ
		{
		    // New affix letter.
		    cur_aff = (affheader_T *)getroom(spin,
						   ಪರಿಮಾಣ(affheader_T), TRUE);
		    ಯದಿ (cur_aff == ರಿಕ್ತ)
			ಭಙ್ಗ;
		    cur_aff->ah_flag = affitem2flag(aff->af_flagtype, ವಸ್ತುಗಳು[1],
								 fname, lnum);
		    ಯದಿ (cur_aff->ah_flag == 0 || STRLEN(ವಸ್ತುಗಳು[1]) >= AH_ಕೀಲಿ_LEN)
			ಭಙ್ಗ;
		    ಯದಿ (cur_aff->ah_flag == aff->af_bad
			    || cur_aff->ah_flag == aff->af_rare
			    || cur_aff->ah_flag == aff->af_keepcase
			    || cur_aff->ah_flag == aff->af_needaffix
			    || cur_aff->ah_flag == aff->af_circumfix
			    || cur_aff->ah_flag == aff->af_nosuggest
			    || cur_aff->ah_flag == aff->af_needcomp
			    || cur_aff->ah_flag == aff->af_comproot)
			smsg(_("Affix also used for BAD/RARE/KEEPCASE/NEEDAFFIX/NEEDCOMPOUND/NOSUGGEST in %s line %d: %s"),
						       fname, lnum, ವಸ್ತುಗಳು[1]);
		    STRCPY(cur_aff->ah_ಕೀಲಿ, ವಸ್ತುಗಳು[1]);
		    hash_add(tp, cur_aff->ah_ಕೀಲಿ, "spelling");

		    cur_aff->ah_combine = (*ವಸ್ತುಗಳು[2] == 'Y');
		}

		// Check for the "S" flag, which apparently means that another
		// block with the same affix name is following.
		ಯದಿ (itemcnt > lasti && STRCMP(ವಸ್ತುಗಳು[lasti], "S") == 0)
		{
		    ++lasti;
		    cur_aff->ah_follows = TRUE;
		}
		ಅನ್ಯಥಾ
		    cur_aff->ah_follows = FALSE;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter.
		ಯದಿ (itemcnt > lasti && *ವಸ್ತುಗಳು[lasti] != '#')
		    smsg(_(e_afftrailing), fname, lnum, ವಸ್ತುಗಳು[lasti]);

		ಯದಿ (STRCMP(ವಸ್ತುಗಳು[2], "Y") != 0 && STRCMP(ವಸ್ತುಗಳು[2], "N") != 0)
		    smsg(_("Expected Y or N in %s line %d: %s"),
						       fname, lnum, ವಸ್ತುಗಳು[2]);

		ಯದಿ (*ವಸ್ತುಗಳು[0] == 'P' && aff->af_pfxpostpone)
		{
		    ಯದಿ (cur_aff->ah_newID == 0)
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
		    ಅನ್ಯಥಾ
			// Did use the ID in a previous block.
			did_postpone_prefix = TRUE;
		}

		aff_todo = atoi((ವರ್ಣ *)ವಸ್ತುಗಳು[3]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ ((STRCMP(ವಸ್ತುಗಳು[0], "PFX") == 0
					      || STRCMP(ವಸ್ತುಗಳು[0], "SFX") == 0)
		    && aff_todo > 0
		    && STRCMP(cur_aff->ah_ಕೀಲಿ, ವಸ್ತುಗಳು[1]) == 0
		    && itemcnt >= 5)
	    {
		affentry_T	*aff_entry;
		ಪೂರ್ಣಾಙ್ಕ		upper = FALSE;
		ಪೂರ್ಣಾಙ್ಕ		lasti = 5;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter,
		// unless IGNOREEXTRA is used.  Hunspell uses a "-" item.
		ಯದಿ (itemcnt > lasti
			&& !aff->af_ignoreextra
			&& *ವಸ್ತುಗಳು[lasti] != '#'
			&& (STRCMP(ವಸ್ತುಗಳು[lasti], "-") != 0
						     || itemcnt != lasti + 1))
		    smsg(_(e_afftrailing), fname, lnum, ವಸ್ತುಗಳು[lasti]);

		// New item for an affix letter.
		--aff_todo;
		aff_entry = (affentry_T *)getroom(spin,
						    ಪರಿಮಾಣ(affentry_T), TRUE);
		ಯದಿ (aff_entry == ರಿಕ್ತ)
		    ಭಙ್ಗ;

		ಯದಿ (STRCMP(ವಸ್ತುಗಳು[2], "0") != 0)
		    aff_entry->ae_chop = getroom_save(spin, ವಸ್ತುಗಳು[2]);
		ಯದಿ (STRCMP(ವಸ್ತುಗಳು[3], "0") != 0)
		{
		    aff_entry->ae_add = getroom_save(spin, ವಸ್ತುಗಳು[3]);

		    // Recognize flags on the affix: abcd/XYZ
		    aff_entry->ae_flags = vim_strchr(aff_entry->ae_add, '/');
		    ಯದಿ (aff_entry->ae_flags != ರಿಕ್ತ)
		    {
			*aff_entry->ae_flags++ = NUL;
			aff_process_flags(aff, aff_entry);
		    }
		}

		// Don't use an affix entry with non-ASCII characters when
		// "spin->si_ascii" is TRUE.
		ಯದಿ (!spin->si_ascii || !(has_non_ascii(aff_entry->ae_chop)
					  || has_non_ascii(aff_entry->ae_add)))
		{
		    aff_entry->ae_next = cur_aff->ah_first;
		    cur_aff->ah_first = aff_entry;

		    ಯದಿ (STRCMP(ವಸ್ತುಗಳು[4], ".") != 0)
		    {
			char_u	buf[MAXLINELEN];

			aff_entry->ae_cond = getroom_save(spin, ವಸ್ತುಗಳು[4]);
			// Note: this silently truncates the buffer, but this should
			// not happen in practice
			ಯದಿ (*ವಸ್ತುಗಳು[0] == 'P')
			    vim_snprintf((ವರ್ಣ *)buf, ಪರಿಮಾಣ(buf), "^%s", ವಸ್ತುಗಳು[4]);
			ಅನ್ಯಥಾ
			    vim_snprintf((ವರ್ಣ *)buf, ಪರಿಮಾಣ(buf), "%s$", ವಸ್ತುಗಳು[4]);
			aff_entry->ae_prog = vim_regcomp(buf,
					    RE_MAGIC + RE_STRING + RE_STRICT);
			ಯದಿ (aff_entry->ae_prog == ರಿಕ್ತ)
			    smsg(_("Broken condition in %s line %d: %s"),
						       fname, lnum, ವಸ್ತುಗಳು[4]);
		    }

		    // For postponed prefixes we need an entry in si_prefcond
		    // for the condition.  Use an existing one if possible.
		    // Can't be done for an affix with flags, ignoring
		    // COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG.
		    ಯದಿ (*ವಸ್ತುಗಳು[0] == 'P' && aff->af_pfxpostpone
					       && aff_entry->ae_flags == ರಿಕ್ತ)
		    {
			// When the chop string is one lower-case letter and
			// the add string ends in the upper-case letter we set
			// the "upper" flag, clear "ae_chop" and remove the
			// letters from "ae_add".  The condition must either
			// be empty or start with the same letter.
			ಯದಿ (aff_entry->ae_chop != ರಿಕ್ತ
				&& aff_entry->ae_add != ರಿಕ್ತ
				&& aff_entry->ae_chop[(*mb_ptr2len)(
						   aff_entry->ae_chop)] == NUL)
			{
			    ಪೂರ್ಣಾಙ್ಕ		c, c_up;

			    c = PTR2CHAR(aff_entry->ae_chop);
			    c_up = SPELL_TOUPPER(c);
			    ಯದಿ (c_up != c
				    && (aff_entry->ae_cond == ರಿಕ್ತ
					|| PTR2CHAR(aff_entry->ae_cond) == c))
			    {
				p = aff_entry->ae_add
						  + STRLEN(aff_entry->ae_add);
				MB_PTR_BACK(aff_entry->ae_add, p);
				ಯದಿ (PTR2CHAR(p) == c_up)
				{
				    upper = TRUE;
				    aff_entry->ae_chop = ರಿಕ್ತ;
				    *p = NUL;

				    // The condition is matched with the
				    // actual word, thus must check for the
				    // upper-case letter.
				    ಯದಿ (aff_entry->ae_cond != ರಿಕ್ತ)
				    {
					char_u	buf[MAXLINELEN];

					ಯದಿ (has_mbyte)
					{
					    onecap_copy(ವಸ್ತುಗಳು[4], buf, TRUE);
					    aff_entry->ae_cond = getroom_save(
								   spin, buf);
					}
					ಅನ್ಯಥಾ
					    *aff_entry->ae_cond = c_up;
					ಯದಿ (aff_entry->ae_cond != ರಿಕ್ತ)
					{
					    sprintf((ವರ್ಣ *)buf, "^%s",
							  aff_entry->ae_cond);
					    vim_regfree(aff_entry->ae_prog);
					    aff_entry->ae_prog = vim_regcomp(
						    buf, RE_MAGIC + RE_STRING);
					}
				    }
				}
			    }
			}

			ಯದಿ (aff_entry->ae_chop == ರಿಕ್ತ
					       && aff_entry->ae_flags == ರಿಕ್ತ)
			{
			    ಪೂರ್ಣಾಙ್ಕ		idx;
			    char_u	**pp;
			    ಪೂರ್ಣಾಙ್ಕ		n;

			    // Find a previously used condition.
			    ಪ್ರತಿ (idx = spin->si_prefcond.ga_len - 1; idx >= 0;
									--idx)
			    {
				p = ((char_u **)spin->si_prefcond.ga_data)[idx];
				ಯದಿ (str_equal(p, aff_entry->ae_cond))
				    ಭಙ್ಗ;
			    }
			    ಯದಿ (idx < 0 && ga_grow(&spin->si_prefcond, 1) == OK)
			    {
				// Not found, add a new condition.
				idx = spin->si_prefcond.ga_len++;
				pp = ((char_u **)spin->si_prefcond.ga_data)
									+ idx;
				ಯದಿ (aff_entry->ae_cond == ರಿಕ್ತ)
				    *pp = ರಿಕ್ತ;
				ಅನ್ಯಥಾ
				    *pp = getroom_save(spin,
							  aff_entry->ae_cond);
			    }

			    // Add the prefix to the prefix tree.
			    ಯದಿ (aff_entry->ae_add == ರಿಕ್ತ)
				p = (char_u *)"";
			    ಅನ್ಯಥಾ
				p = aff_entry->ae_add;

			    // PFX_FLAGS is a negative number, so that
			    // tree_add_word() knows this is the prefix tree.
			    n = PFX_FLAGS;
			    ಯದಿ (!cur_aff->ah_combine)
				n |= WFP_NC;
			    ಯದಿ (upper)
				n |= WFP_UP;
			    ಯದಿ (aff_entry->ae_comppermit)
				n |= WFP_COMPPERMIT;
			    ಯದಿ (aff_entry->ae_compforbid)
				n |= WFP_COMPFORBID;
			    ವೃಕ್ಷ_add_word(spin, p, spin->si_prefroot, n,
						      idx, cur_aff->ah_newID);
			    did_postpone_prefix = TRUE;
			}

			// Didn't actually use ah_newID, backup si_newprefID.
			ಯದಿ (aff_todo == 0 && !did_postpone_prefix)
			{
			    --spin->si_newprefID;
			    cur_aff->ah_newID = 0;
			}
		    }
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "FOL", 2) && fol == ರಿಕ್ತ)
	    {
		fol = vim_strsave(ವಸ್ತುಗಳು[1]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "LOW", 2) && ಕೆಳಗಿನ == ರಿಕ್ತ)
	    {
		ಕೆಳಗಿನ = vim_strsave(ವಸ್ತುಗಳು[1]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "UPP", 2) && upp == ರಿಕ್ತ)
	    {
		upp = vim_strsave(ವಸ್ತುಗಳು[1]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "REP", 2)
		     || is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "REPSAL", 2))
	    {
		// Ignore REP/REPSAL count
		ಯದಿ (!SAFE_isdigit(*ವಸ್ತುಗಳು[1]))
		    smsg(_("Expected REP(SAL) count in %s line %d"),
								 fname, lnum);
	    }
	    ಅನ್ಯಥಾ ಯದಿ ((STRCMP(ವಸ್ತುಗಳು[0], "REP") == 0
			|| STRCMP(ವಸ್ತುಗಳು[0], "REPSAL") == 0)
		    && itemcnt >= 3)
	    {
		// REP/REPSAL item
		// Myspell ignores extra arguments, we require it starts with
		// # to detect mistakes.
		ಯದಿ (itemcnt > 3 && ವಸ್ತುಗಳು[3][0] != '#')
		    smsg(_(e_afftrailing), fname, lnum, ವಸ್ತುಗಳು[3]);
		ಯದಿ (ವಸ್ತುಗಳು[0][3] == 'S' ? do_repsal : do_rep)
		{
		    // Replace underscore with space (can't include a space
		    // directly).
		    ಪ್ರತಿ (p = ವಸ್ತುಗಳು[1]; *p != NUL; MB_PTR_ADV(p))
			ಯದಿ (*p == '_')
			    *p = ' ';
		    ಪ್ರತಿ (p = ವಸ್ತುಗಳು[2]; *p != NUL; MB_PTR_ADV(p))
			ಯದಿ (*p == '_')
			    *p = ' ';
		    add_fromto(spin, ವಸ್ತುಗಳು[0][3] == 'S'
					 ? &spin->si_repsal
					 : &spin->si_rep, ವಸ್ತುಗಳು[1], ವಸ್ತುಗಳು[2]);
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "MAP", 2))
	    {
		// MAP item or count
		ಯದಿ (!found_map)
		{
		    // First line contains the count.
		    found_map = TRUE;
		    ಯದಿ (!SAFE_isdigit(*ವಸ್ತುಗಳು[1]))
			smsg(_("Expected MAP count in %s line %d"),
								 fname, lnum);
		}
		ಅನ್ಯಥಾ ಯದಿ (do_mapline)
		{
		    ಪೂರ್ಣಾಙ್ಕ		c;

		    // Check that every character appears only once.
		    ಪ್ರತಿ (p = ವಸ್ತುಗಳು[1]; *p != NUL; )
		    {
			c = mb_ptr2char_adv(&p);
			ಯದಿ ((spin->si_map.ga_len > 0
				    && vim_strchr(spin->si_map.ga_data, c)
								      != ರಿಕ್ತ)
				|| vim_strchr(p, c) != ರಿಕ್ತ)
			    smsg(_("Duplicate character in MAP in %s line %d"),
								 fname, lnum);
		    }

		    // We simply concatenate all the MAP strings, separated by
		    // slashes.
		    ga_concat(&spin->si_map, ವಸ್ತುಗಳು[1]);
		    ga_append(&spin->si_map, '/');
		}
	    }
	    // Accept "SAL from to" and "SAL from to  #comment".
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "SAL", 3))
	    {
		ಯದಿ (do_sal)
		{
		    // SAL item (sounds-a-like)
		    // Either one of the known keys or a from-to pair.
		    ಯದಿ (STRCMP(ವಸ್ತುಗಳು[1], "followup") == 0)
			spin->si_followup = sal_to_bool(ವಸ್ತುಗಳು[2]);
		    ಅನ್ಯಥಾ ಯದಿ (STRCMP(ವಸ್ತುಗಳು[1], "collapse_result") == 0)
			spin->si_collapse = sal_to_bool(ವಸ್ತುಗಳು[2]);
		    ಅನ್ಯಥಾ ಯದಿ (STRCMP(ವಸ್ತುಗಳು[1], "remove_accents") == 0)
			spin->si_rem_accents = sal_to_bool(ವಸ್ತುಗಳು[2]);
		    ಅನ್ಯಥಾ
			// when "to" is "_" it means empty
			add_fromto(spin, &spin->si_sal, ವಸ್ತುಗಳು[1],
				     STRCMP(ವಸ್ತುಗಳು[2], "_") == 0 ? (char_u *)""
								: ವಸ್ತುಗಳು[2]);
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "SOFOFROM", 2)
							  && sofofrom == ರಿಕ್ತ)
	    {
		sofofrom = getroom_save(spin, ವಸ್ತುಗಳು[1]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (is_aff_rule(ವಸ್ತುಗಳು, itemcnt, "SOFOTO", 2)
							    && sofoto == ರಿಕ್ತ)
	    {
		sofoto = getroom_save(spin, ವಸ್ತುಗಳು[1]);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(ವಸ್ತುಗಳು[0], "COMMON") == 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ	i;

		ಪ್ರತಿ (i = 1; i < itemcnt; ++i)
		{
		    ಯದಿ (HASHITEM_EMPTY(hash_find(&spin->si_commonwords,
								   ವಸ್ತುಗಳು[i])))
		    {
			p = vim_strsave(ವಸ್ತುಗಳು[i]);
			ಯದಿ (p == ರಿಕ್ತ)
			    ಭಙ್ಗ;
			hash_add(&spin->si_commonwords, p, "spelling");
		    }
		}
	    }
	    ಅನ್ಯಥಾ
		smsg(_("Unrecognized or duplicate item in %s line %d: %s"),
						       fname, lnum, ವಸ್ತುಗಳು[0]);
	}
    }

    ಯದಿ (fol != ರಿಕ್ತ || ಕೆಳಗಿನ != ರಿಕ್ತ || upp != ರಿಕ್ತ)
    {
	ಯದಿ (spin->si_clear_chartab)
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
	ಯದಿ (!spin->si_ascii && !enc_utf8)
	{
	    ಯದಿ (fol == ರಿಕ್ತ || ಕೆಳಗಿನ == ರಿಕ್ತ || upp == ರಿಕ್ತ)
		smsg(_("Missing FOL/LOW/UPP line in %s"), fname);
	    ಅನ್ಯಥಾ
		(ಶೂನ್ಯ)set_spell_chartab(fol, ಕೆಳಗಿನ, upp);
	}

	vim_free(fol);
	vim_free(ಕೆಳಗಿನ);
	vim_free(upp);
    }

    // Use compound specifications of the .aff file for the spell info.
    ಯದಿ (compmax != 0)
    {
	aff_check_ಸಂಖ್ಯೆ(spin->si_compmax, compmax, "COMPOUNDWORDMAX");
	spin->si_compmax = compmax;
    }

    ಯದಿ (compminlen != 0)
    {
	aff_check_ಸಂಖ್ಯೆ(spin->si_compminlen, compminlen, "COMPOUNDMIN");
	spin->si_compminlen = compminlen;
    }

    ಯದಿ (compsylmax != 0)
    {
	ಯದಿ (syllable == ರಿಕ್ತ)
	    smsg(_("COMPOUNDSYLMAX used without SYLLABLE"));
	aff_check_ಸಂಖ್ಯೆ(spin->si_compsylmax, compsylmax, "COMPOUNDSYLMAX");
	spin->si_compsylmax = compsylmax;
    }

    ಯದಿ (compoptions != 0)
    {
	aff_check_ಸಂಖ್ಯೆ(spin->si_compoptions, compoptions, "COMPOUND options");
	spin->si_compoptions |= compoptions;
    }

    ಯದಿ (compflags != ರಿಕ್ತ)
	process_compflags(spin, aff, compflags);

    // Check that we didn't use too many renumbered flags.
    ಯದಿ (spin->si_newcompID < spin->si_newprefID)
    {
	ಯದಿ (spin->si_newcompID == 127 || spin->si_newcompID == 255)
	    ಸಂದೇಶ(_("Too many postponed prefixes"));
	ಅನ್ಯಥಾ ಯದಿ (spin->si_newprefID == 0 || spin->si_newprefID == 127)
	    ಸಂದೇಶ(_("Too many compound flags"));
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ(_("Too many postponed prefixes and/or compound flags"));
    }

    ಯದಿ (syllable != ರಿಕ್ತ)
    {
	aff_check_string(spin->si_syllable, syllable, "SYLLABLE");
	spin->si_syllable = syllable;
    }

    ಯದಿ (sofofrom != ರಿಕ್ತ || sofoto != ರಿಕ್ತ)
    {
	ಯದಿ (sofofrom == ರಿಕ್ತ || sofoto == ರಿಕ್ತ)
	    smsg(_("Missing SOFO%s line in %s"),
				     sofofrom == ರಿಕ್ತ ? "FROM" : "TO", fname);
	ಅನ್ಯಥಾ ಯದಿ (spin->si_sal.ga_len > 0)
	    smsg(_("Both SAL and SOFO lines in %s"), fname);
	ಅನ್ಯಥಾ
	{
	    aff_check_string(spin->si_sofofr, sofofrom, "SOFOFROM");
	    aff_check_string(spin->si_sofoto, sofoto, "SOFOTO");
	    spin->si_sofofr = sofofrom;
	    spin->si_sofoto = sofoto;
	}
    }

    ಯದಿ (midword != ರಿಕ್ತ)
    {
	aff_check_string(spin->si_midword, midword, "MIDWORD");
	spin->si_midword = midword;
    }

    vim_free(pc);
    fclose(fd);
    ಪ್ರತ್ಯಾಗಮ aff;
}

/*
 * Return TRUE when items[0] equals "rulename", there are "mincount" items or
 * a comment is following after item "mincount".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_aff_rule(
    char_u	**ವಸ್ತುಗಳು,
    ಪೂರ್ಣಾಙ್ಕ		itemcnt,
    ವರ್ಣ	*rulename,
    ಪೂರ್ಣಾಙ್ಕ		mincount)
{
    ಪ್ರತ್ಯಾಗಮ (STRCMP(ವಸ್ತುಗಳು[0], rulename) == 0
	    && (itemcnt == mincount
		|| (itemcnt > mincount && ವಸ್ತುಗಳು[mincount][0] == '#')));
}

/*
 * For affix "entry" move COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG from
 * ae_flags to ae_comppermit and ae_compforbid.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
aff_process_flags(afffile_T *affile, affentry_T *entry)
{
    char_u	*p;
    char_u	*prevp;
    unsigned	flag;

    ಯದಿ (entry->ae_flags != ರಿಕ್ತ
		&& (affile->af_compforbid != 0 || affile->af_comppermit != 0))
    {
	ಪ್ರತಿ (p = entry->ae_flags; *p != NUL; )
	{
	    prevp = p;
	    flag = get_affitem(affile->af_flagtype, &p);
	    ಯದಿ (flag == affile->af_comppermit || flag == affile->af_compforbid)
	    {
		STRMOVE(prevp, p);
		p = prevp;
		ಯದಿ (flag == affile->af_comppermit)
		    entry->ae_comppermit = TRUE;
		ಅನ್ಯಥಾ
		    entry->ae_compforbid = TRUE;
	    }
	    ಯದಿ (affile->af_flagtype == AFT_ಅಂಕೆ && *p == ',')
		++p;
	}
	ಯದಿ (*entry->ae_flags == NUL)
	    entry->ae_flags = ರಿಕ್ತ;	// nothing left
    }
}

/*
 * Return TRUE if "s" is the name of an info item in the affix file.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
spell_info_item(char_u *s)
{
    ಪ್ರತ್ಯಾಗಮ STRCMP(s, "NAME") == 0
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
    ಸ್ಥಿರ unsigned
affitem2flag(
    ಪೂರ್ಣಾಙ್ಕ		flagtype,
    char_u	*item,
    char_u	*fname,
    ಪೂರ್ಣಾಙ್ಕ		lnum)
{
    unsigned	res;
    char_u	*p = item;

    res = get_affitem(flagtype, &p);
    ಯದಿ (res == 0)
    {
	ಯದಿ (flagtype == AFT_ಅಂಕೆ)
	    smsg(_("Flag is not a number in %s line %d: %s"),
							   fname, lnum, item);
	ಅನ್ಯಥಾ
	    smsg(_("Illegal flag in %s line %d: %s"),
							   fname, lnum, item);
    }
    ಯದಿ (*p != NUL)
    {
	smsg(_(e_affname), fname, lnum, item);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Get one affix name from "*pp" and advance the pointer.
 * Returns ZERO_FLAG for "0".
 * Returns zero for an error, still advances the pointer then.
 */
    ಸ್ಥಿರ unsigned
get_affitem(ಪೂರ್ಣಾಙ್ಕ flagtype, char_u **pp)
{
    ಪೂರ್ಣಾಙ್ಕ		res;

    ಯದಿ (flagtype == AFT_ಅಂಕೆ)
    {
	ಯದಿ (!VIM_ISDIGIT(**pp))
	{
	    ++*pp;	// always advance, avoid getting stuck
	    ಪ್ರತ್ಯಾಗಮ 0;
	}
	res = getdigits(pp);
	ಯದಿ (res == 0)
	    res = ZERO_FLAG;
    }
    ಅನ್ಯಥಾ
    {
	res = mb_ptr2char_adv(pp);
	ಯದಿ (flagtype == AFT_LONG || (flagtype == AFT_CAPLONG
						 && res >= 'A' && res <= 'Z'))
	{
	    ಯದಿ (**pp == NUL)
		ಪ್ರತ್ಯಾಗಮ 0;
	    res = mb_ptr2char_adv(pp) + (res << 16);
	}
    }
    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Process the "compflags" string used in an affix file and append it to
 * spin->si_compflags.
 * The processing involves changing the affix names to ID numbers, so that
 * they fit in one byte.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
process_compflags(
    spellinfo_T	*spin,
    afffile_T	*aff,
    char_u	*compflags)
{
    char_u	*p;
    char_u	*prevp;
    unsigned	flag;
    compitem_T	*ci;
    ಪೂರ್ಣಾಙ್ಕ		id;
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*tp;
    char_u	ಕೀಲಿ[AH_ಕೀಲಿ_LEN];
    hashitem_T	*hi;

    // Make room for the old and the new compflags, concatenated with a / in
    // between.  Processing it makes it shorter, but we don't know by how
    // much, thus allocate the maximum.
    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(compflags) + 1;
    ಯದಿ (spin->si_compflags != ರಿಕ್ತ)
	len += (ಪೂರ್ಣಾಙ್ಕ)STRLEN(spin->si_compflags) + 1;
    p = getroom(spin, len, FALSE);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (spin->si_compflags != ರಿಕ್ತ)
    {
	STRCPY(p, spin->si_compflags);
	STRCAT(p, "/");
    }
    spin->si_compflags = p;
    tp = p + STRLEN(p);

    ಪ್ರತಿ (p = compflags; *p != NUL; )
    {
	ಯದಿ (vim_strchr((char_u *)"/?*+[]", *p) != ರಿಕ್ತ)
	    // Copy non-flag characters directly.
	    *tp++ = *p++;
	ಅನ್ಯಥಾ
	{
	    // First get the flag number, also checks validity.
	    prevp = p;
	    flag = get_affitem(aff->af_flagtype, &p);
	    ಯದಿ (flag != 0)
	    {
		// Find the flag in the hashtable.  If it was used before, use
		// the existing ID.  Otherwise add a new entry.
		vim_strncpy(ಕೀಲಿ, prevp, p - prevp);
		hi = hash_find(&aff->af_comp, ಕೀಲಿ);
		ಯದಿ (!HASHITEM_EMPTY(hi))
		    id = HI2CI(hi)->ci_newID;
		ಅನ್ಯಥಾ
		{
		    ci = (compitem_T *)getroom(spin, ಪರಿಮಾಣ(compitem_T), TRUE);
		    ಯದಿ (ci == ರಿಕ್ತ)
			ಭಙ್ಗ;
		    STRCPY(ci->ci_ಕೀಲಿ, ಕೀಲಿ);
		    ci->ci_flag = flag;
		    // Avoid using a flag ID that has a special meaning in a
		    // regexp (also inside []).
		    ಕುರು
		    {
			check_renumber(spin);
			id = spin->si_newcompID--;
		    } ಯಾವತ್ (vim_strchr((char_u *)"/?*+[]\\-^", id) != ರಿಕ್ತ);
		    ci->ci_newID = id;
		    hash_add(&aff->af_comp, ci->ci_ಕೀಲಿ, "spelling");
		}
		*tp++ = id;
	    }
	    ಯದಿ (aff->af_flagtype == AFT_ಅಂಕೆ && *p == ',')
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
    ಸ್ಥಿರ ಶೂನ್ಯ
check_renumber(spellinfo_T *spin)
{
    ಯದಿ (spin->si_newprefID == spin->si_newcompID && spin->si_newcompID < 128)
    {
	spin->si_newprefID = 127;
	spin->si_newcompID = 255;
    }
}

/*
 * Append one affix or compound ID to "store_afflist".
 * Returns FAIL when this would overrun the fixed-size buffer.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
store_afflist_add(
    char_u	*store_afflist,
    ಪೂರ್ಣಾಙ್ಕ		*cntp,
    ಪೂರ್ಣಾಙ್ಕ		id)
{
    ಯದಿ (*cntp >= MAXWLEN - 1)
    {
	emsg(_(e_too_many_postponed_prefixes_spell));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    store_afflist[(*cntp)++] = id;
    store_afflist[*cntp] = NUL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return TRUE if flag "flag" appears in affix list "afflist".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
flag_in_afflist(ಪೂರ್ಣಾಙ್ಕ flagtype, char_u *afflist, unsigned flag)
{
    char_u	*p;
    unsigned	n;

    ವಿಕಲ್ಪ (flagtype)
    {
	ಪಕ್ಷ AFT_CHAR:
	    ಪ್ರತ್ಯಾಗಮ vim_strchr(afflist, flag) != ರಿಕ್ತ;

	ಪಕ್ಷ AFT_CAPLONG:
	ಪಕ್ಷ AFT_LONG:
	    ಪ್ರತಿ (p = afflist; *p != NUL; )
	    {
		n = mb_ptr2char_adv(&p);
		ಯದಿ ((flagtype == AFT_LONG || (n >= 'A' && n <= 'Z'))
								 && *p != NUL)
		    n = mb_ptr2char_adv(&p) + (n << 16);
		ಯದಿ (n == flag)
		    ಪ್ರತ್ಯಾಗಮ TRUE;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ AFT_ಅಂಕೆ:
	    ಪ್ರತಿ (p = afflist; *p != NUL; )
	    {
		n = getdigits(&p);
		ಯದಿ (n == 0)
		    n = ZERO_FLAG;
		ಯದಿ (n == flag)
		    ಪ್ರತ್ಯಾಗಮ TRUE;
		ಯದಿ (*p != NUL)	// skip over comma
		    ++p;
	    }
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Give a warning when "spinval" and "affval" numbers are set and not the same.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
aff_check_ಸಂಖ್ಯೆ(ಪೂರ್ಣಾಙ್ಕ spinval, ಪೂರ್ಣಾಙ್ಕ affval, ವರ್ಣ *name)
{
    ಯದಿ (spinval != 0 && spinval != affval)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Give a warning when "spinval" and "affval" strings are set and not the same.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
aff_check_string(char_u *spinval, char_u *affval, ವರ್ಣ *name)
{
    ಯದಿ (spinval != ರಿಕ್ತ && STRCMP(spinval, affval) != 0)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Return TRUE if strings "s1" and "s2" are equal.  Also consider both being
 * NULL as equal.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
str_equal(char_u *s1, char_u *s2)
{
    ಯದಿ (s1 == ರಿಕ್ತ || s2 == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ s1 == s2;
    ಪ್ರತ್ಯಾಗಮ STRCMP(s1, s2) == 0;
}

/*
 * Add a from-to item to "gap".  Used for REP and SAL items.
 * They are stored case-folded.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
add_fromto(
    spellinfo_T	*spin,
    garray_T	*gap,
    char_u	*from,
    char_u	*to)
{
    fromto_T	*ftp;
    char_u	word[MAXWLEN];

    ಯದಿ (ga_grow(gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ftp = ((fromto_T *)gap->ga_data) + gap->ga_len;
    (ಶೂನ್ಯ)spell_casefold(curwin, from, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(from), word, MAXWLEN);
    ftp->ft_from = getroom_save(spin, word);
    (ಶೂನ್ಯ)spell_casefold(curwin, to, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(to), word, MAXWLEN);
    ftp->ft_to = getroom_save(spin, word);
    ++gap->ga_len;
}

/*
 * Convert a boolean argument in a SAL line to TRUE or FALSE;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sal_to_bool(char_u *s)
{
    ಪ್ರತ್ಯಾಗಮ STRCMP(s, "1") == 0 || STRCMP(s, "true") == 0;
}

/*
 * Free the structure filled by spell_read_aff().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_free_aff(afffile_T *aff)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    affheader_T	*ah;
    affentry_T	*ae;

    vim_free(aff->af_enc);

    // All this trouble to free the "ae_prog" items...
    ಪ್ರತಿ (ht = &aff->af_pref; ; ht = &aff->af_suff)
    {
	todo = (ಪೂರ್ಣಾಙ್ಕ)ht->ht_used;
	FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
	{
	    ಯದಿ (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		ah = HI2AH(hi);
		ಪ್ರತಿ (ae = ah->ah_first; ae != ರಿಕ್ತ; ae = ae->ae_next)
		    vim_regfree(ae->ae_prog);
	    }
	}
	ಯದಿ (ht == &aff->af_suff)
	    ಭಙ್ಗ;
    }

    hash_clear(&aff->af_pref);
    hash_clear(&aff->af_suff);
    hash_clear(&aff->af_comp);
}

/*
 * Read dictionary file "fname".
 * Returns OK or FAIL;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
spell_read_dic(spellinfo_T *spin, char_u *fname, afffile_T *affile)
{
    hashtab_T	ht;
    char_u	ಸಾಲು[MAXLINELEN];
    char_u	*p;
    char_u	*afflist;
    char_u	store_afflist[MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		pfxlen;
    ಪೂರ್ಣಾಙ್ಕ		totlen;
    ಪೂರ್ಣಾಙ್ಕ		need_affix;
    char_u	*dw;
    char_u	*pc;
    char_u	*w;
    ಪೂರ್ಣಾಙ್ಕ		l;
    hash_T	hash;
    hashitem_T	*hi;
    FILE	*fd;
    ಪೂರ್ಣಾಙ್ಕ		lnum = 1;
    ಪೂರ್ಣಾಙ್ಕ		non_ascii = 0;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
    char_u	message[MAXLINELEN + MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		flags;
    ಪೂರ್ಣಾಙ್ಕ		duplicate = 0;
    time_T	last_ಸಂದೇಶ_time = 0;

    /*
     * Open the file.
     */
    fd = mch_fopen((ವರ್ಣ *)fname, "r");
    ಯದಿ (fd == ರಿಕ್ತ)
    {
	semsg(_(e_cant_open_file_str), fname);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // The hashtable is only used to detect duplicated words.
    hash_init(&ht);

    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE,
				  _("Reading dictionary file %s..."), fname);
    spell_message(spin, IObuff);

    // start with a message for the first line
    spin->si_ಸಂದೇಶ_ಎಣಿಕೆ = 999999;

    // Read and ignore the first line: word count.
    ಯದಿ (vim_fgets(ಸಾಲು, MAXLINELEN, fd) || !vim_isdigit(*skipwhite(ಸಾಲು)))
	semsg(_(e_no_word_ಎಣಿಕೆ_in_str), fname);

    /*
     * Read all the lines in the file one by one.
     * The words are converted to 'encoding' here, before being added to
     * the hashtable.
     */
    ಯಾವತ್ (!vim_fgets(ಸಾಲು, MAXLINELEN, fd) && !got_int)
    {
	ಸಾಲು_breakcheck();
	++lnum;
	ಯದಿ (ಸಾಲು[0] == '#' || ಸಾಲು[0] == '/')
	    ಅನುವರ್ತ;	// comment line

	// Remove CR, LF and white space from the end.  White space halfway
	// the word is kept to allow e.g., "et al.".
	l = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಸಾಲು);
	ಯಾವತ್ (l > 0 && ಸಾಲು[l - 1] <= ' ')
	    --l;
	ಯದಿ (l == 0)
	    ಅನುವರ್ತ;	// empty line
	ಸಾಲು[l] = NUL;

	// Convert from "SET" to 'encoding' when needed.
	ಯದಿ (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, ಸಾಲು, ರಿಕ್ತ);
	    ಯದಿ (pc == ರಿಕ್ತ)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
						       fname, lnum, ಸಾಲು);
		ಅನುವರ್ತ;
	    }
	    w = pc;
	}
	ಅನ್ಯಥಾ
	{
	    pc = ರಿಕ್ತ;
	    w = ಸಾಲು;
	}

	// Truncate the word at the "/", set "afflist" to what follows.
	// Replace "\/" by "/" and "\\" by "\".
	afflist = ರಿಕ್ತ;
	ಪ್ರತಿ (p = w; *p != NUL; MB_PTR_ADV(p))
	{
	    ಯದಿ (*p == '\\' && (p[1] == '\\' || p[1] == '/'))
		STRMOVE(p, p + 1);
	    ಅನ್ಯಥಾ ಯದಿ (*p == '/')
	    {
		*p = NUL;
		afflist = p + 1;
		ಭಙ್ಗ;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	ಯದಿ (spin->si_ascii && has_non_ascii(w))
	{
	    ++non_ascii;
	    vim_free(pc);
	    ಅನುವರ್ತ;
	}

	// This takes time, print a message every 10000 words, but not more
	// often than once per second.
	ಯದಿ (spin->si_verbose && spin->si_ಸಂದೇಶ_ಎಣಿಕೆ > 10000)
	{
	    spin->si_ಸಂದೇಶ_ಎಣಿಕೆ = 0;
	    ಯದಿ (vim_time() > last_ಸಂದೇಶ_time)
	    {
		last_ಸಂದೇಶ_time = vim_time();
		vim_snprintf((ವರ್ಣ *)message, ಪರಿಮಾಣ(message),
			_("line %6d, word %6ld - %s"),
			   lnum, spin->si_foldwcount + spin->si_keepwcount, w);
		ಸಂದೇಶ_start();
		ಸಂದೇಶ_outtrans_long_attr(message, 0);
		ಸಂದೇಶ_clr_eos();
		ಸಂದೇಶ_didout = FALSE;
		ಸಂದೇಶ_col = 0;
		out_flush();
	    }
	}

	// Store the word in the hashtable to be able to find duplicates.
	dw = getroom_save(spin, w);
	ಯದಿ (dw == ರಿಕ್ತ)
	{
	    retval = FAIL;
	    vim_free(pc);
	    ಭಙ್ಗ;
	}

	hash = hash_hash(dw);
	hi = hash_lookup(&ht, dw, hash);
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    ಯದಿ (p_verbose > 0)
		smsg(_("Duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ಅನ್ಯಥಾ ಯದಿ (duplicate == 0)
		smsg(_("First duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ++duplicate;
	}
	ಅನ್ಯಥಾ
	    hash_add_item(&ht, hi, dw, hash);

	flags = 0;
	store_afflist[0] = NUL;
	pfxlen = 0;
	totlen = 0;
	need_affix = FALSE;
	ಯದಿ (afflist != ರಿಕ್ತ)
	{
	    // Extract flags from the affix list.
	    flags |= get_affix_flags(affile, afflist);

	    ಯದಿ (affile->af_needaffix != 0 && flag_in_afflist(
			  affile->af_flagtype, afflist, affile->af_needaffix))
		need_affix = TRUE;

	    ಯದಿ (affile->af_pfxpostpone)
	    {
		// Need to store the list of prefix IDs with the word.
		ಯದಿ (get_pfxlist(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    ಭಙ್ಗ;
		}
		pfxlen = totlen;
	    }

	    ಯದಿ (spin->si_compflags != ರಿಕ್ತ)
	    {
		// Need to store the list of compound flags with the word.
		// Concatenate them to the list of prefix IDs.
		ಯದಿ (get_compflags(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    ಭಙ್ಗ;
		}
	    }
	}

	// Add the word to the word tree(s).
	ಯದಿ (store_word(spin, dw, flags, spin->si_region,
					   store_afflist, need_affix) == FAIL)
	    retval = FAIL;

	ಯದಿ (afflist != ರಿಕ್ತ)
	{
	    // Find all matching suffixes and add the resulting words.
	    // Additionally do matching prefixes that combine.
	    ಯದಿ (store_aff_word(spin, dw, afflist, affile,
			   &affile->af_suff, &affile->af_pref,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;

	    // Find all matching prefixes and add the resulting words.
	    ಯದಿ (store_aff_word(spin, dw, afflist, affile,
			  &affile->af_pref, ರಿಕ್ತ,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;
	}

	vim_free(pc);
    }

    ಯದಿ (duplicate > 0)
	smsg(_("%d duplicate word(s) in %s"), duplicate, fname);
    ಯದಿ (spin->si_ascii && non_ascii > 0)
	smsg(_("Ignored %d word(s) with non-ASCII characters in %s"),
							    non_ascii, fname);
    hash_clear(&ht);

    fclose(fd);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Check for affix flags in "afflist" that are turned into word flags.
 * Return WF_ flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_affix_flags(afffile_T *affile, char_u *afflist)
{
    ಪೂರ್ಣಾಙ್ಕ		flags = 0;

    ಯದಿ (affile->af_keepcase != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_keepcase))
	flags |= WF_KEEPCAP | WF_FIXCAP;
    ಯದಿ (affile->af_rare != 0 && flag_in_afflist(
			       affile->af_flagtype, afflist, affile->af_rare))
	flags |= WF_RARE;
    ಯದಿ (affile->af_bad != 0 && flag_in_afflist(
				affile->af_flagtype, afflist, affile->af_bad))
	flags |= WF_BANNED;
    ಯದಿ (affile->af_needcomp != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_needcomp))
	flags |= WF_NEEDCOMP;
    ಯದಿ (affile->af_comproot != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_comproot))
	flags |= WF_COMPROOT;
    ಯದಿ (affile->af_nosuggest != 0 && flag_in_afflist(
			  affile->af_flagtype, afflist, affile->af_nosuggest))
	flags |= WF_NOSUGGEST;
    ಪ್ರತ್ಯಾಗಮ flags;
}

/*
 * Get the list of prefix IDs from the affix list "afflist".
 * Used for PFXPOSTPONE.
 * Put the resulting flags in "store_afflist[MAXWLEN]" with a terminating NUL.
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_pfxlist(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    ಪೂರ್ಣಾಙ್ಕ		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    ಪೂರ್ಣಾಙ್ಕ		id;
    char_u	ಕೀಲಿ[AH_ಕೀಲಿ_LEN];
    hashitem_T	*hi;

    ಪ್ರತಿ (p = afflist; *p != NUL; )
    {
	prevp = p;
	ಯದಿ (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a postponed prefix flag if it appears in "af_pref"
	    // and its ID is not zero.
	    vim_strncpy(ಕೀಲಿ, prevp, p - prevp);
	    hi = hash_find(&affile->af_pref, ಕೀಲಿ);
	    ಯದಿ (!HASHITEM_EMPTY(hi))
	    {
		id = HI2AH(hi)->ah_newID;
		ಯದಿ (id != 0 && store_afflist_add(store_afflist, cntp, id) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	ಯದಿ (affile->af_flagtype == AFT_ಅಂಕೆ && *p == ',')
	    ++p;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get the list of compound IDs from the affix list "afflist" that are used
 * for compound words.
 * Puts the flags in "store_afflist[]".
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_compflags(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    ಪೂರ್ಣಾಙ್ಕ		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    char_u	ಕೀಲಿ[AH_ಕೀಲಿ_LEN];
    hashitem_T	*hi;

    ಪ್ರತಿ (p = afflist; *p != NUL; )
    {
	prevp = p;
	ಯದಿ (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a compound flag if it appears in "af_comp".
	    vim_strncpy(ಕೀಲಿ, prevp, p - prevp);
	    hi = hash_find(&affile->af_comp, ಕೀಲಿ);
	    ಯದಿ (!HASHITEM_EMPTY(hi)
		    && store_afflist_add(store_afflist, cntp,
					      HI2CI(hi)->ci_newID) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (affile->af_flagtype == AFT_ಅಂಕೆ && *p == ',')
	    ++p;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
store_aff_word(
    spellinfo_T	*spin,		// spell info
    char_u	*word,		// basic word start
    char_u	*afflist,	// list of names of supported affixes
    afffile_T	*affile,
    hashtab_T	*ht,
    hashtab_T	*xht,
    ಪೂರ್ಣಾಙ್ಕ		condit,		// CONDIT_SUF et al.
    ಪೂರ್ಣಾಙ್ಕ		flags,		// flags for the word
    char_u	*pfxlist,	// list of prefix IDs
    ಪೂರ್ಣಾಙ್ಕ		pfxlen)		// nr of flags in "pfxlist" for prefixes, rest
				// is compound flags
{
    ಪೂರ್ಣಾಙ್ಕ		todo;
    hashitem_T	*hi;
    affheader_T	*ah;
    affentry_T	*ae;
    char_u	newword[MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		use_flags;
    char_u	*use_pfxlist;
    ಪೂರ್ಣಾಙ್ಕ		use_pfxlen;
    ಪೂರ್ಣಾಙ್ಕ		need_affix;
    char_u	store_afflist[MAXWLEN];
    char_u	pfx_pfxlist[MAXWLEN];
    size_t	wordlen = STRLEN(word);
    ಪೂರ್ಣಾಙ್ಕ		use_condit;

    todo = (ಪೂರ್ಣಾಙ್ಕ)ht->ht_used;
    ಪ್ರತಿ (hi = ht->ht_array; todo > 0 && retval == OK; ++hi)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ah = HI2AH(hi);

	    // Check that the affix combines, if required, and that the word
	    // supports this affix.
	    ಯದಿ (((condit & CONDIT_COMB) == 0 || ah->ah_combine)
		    && flag_in_afflist(affile->af_flagtype, afflist,
								 ah->ah_flag))
	    {
		// Loop over all affix entries with this name.
		ಪ್ರತಿ (ae = ah->ah_first; ae != ರಿಕ್ತ; ae = ae->ae_next)
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
		    ಯದಿ ((xht != ರಿಕ್ತ || !affile->af_pfxpostpone
				|| ae->ae_chop != ರಿಕ್ತ
				|| ae->ae_flags != ರಿಕ್ತ)
			    && (ae->ae_chop == ರಿಕ್ತ
				|| STRLEN(ae->ae_chop) < wordlen)
			    && (ae->ae_prog == ರಿಕ್ತ
				|| vim_regexec_prog(&ae->ae_prog, FALSE,
							    word, (colnr_T)0))
			    && (((condit & CONDIT_CFIX) == 0)
				== ((condit & CONDIT_AFF) == 0
				    || ae->ae_flags == ರಿಕ್ತ
				    || !flag_in_afflist(affile->af_flagtype,
					ae->ae_flags, affile->af_circumfix))))
		    {
			// Match.  Remove the chop and add the affix.
			ಯದಿ (xht == ರಿಕ್ತ)
			{
			    // prefix: chop/add at the start of the word
			    ಯದಿ (ae->ae_add == ರಿಕ್ತ)
				*newword = NUL;
			    ಅನ್ಯಥಾ
				vim_strncpy(newword, ae->ae_add, MAXWLEN - 1);
			    p = word;
			    ಯದಿ (ae->ae_chop != ರಿಕ್ತ)
			    {
				// Skip chop string.
				ಯದಿ (has_mbyte)
				{
				    i = mb_charlen(ae->ae_chop);
				    ಪ್ರತಿ ( ; i > 0; --i)
					MB_PTR_ADV(p);
				}
				ಅನ್ಯಥಾ
				    p += STRLEN(ae->ae_chop);
			    }
			    // Note: this silently truncates the buffer, but this should
			    // not happen in practice
			    STRNCAT(newword, p, MAXWLEN - STRLEN(newword) - 1);
			}
			ಅನ್ಯಥಾ
			{
			    // suffix: chop/add at the end of the word
			    vim_strncpy(newword, word, MAXWLEN - 1);
			    ಯದಿ (ae->ae_chop != ರಿಕ್ತ)
			    {
				// Remove chop string.
				p = newword + STRLEN(newword);
				i = (ಪೂರ್ಣಾಙ್ಕ)MB_CHARLEN(ae->ae_chop);
				ಪ್ರತಿ ( ; i > 0; --i)
				    MB_PTR_BACK(newword, p);
				*p = NUL;
			    }
			    ಯದಿ (ae->ae_add != ರಿಕ್ತ)
				// Note: this silently truncates the buffer, but this should
				// not happen in practice
				STRNCAT(newword, ae->ae_add, MAXWLEN - STRLEN(newword) - 1);
			}

			use_flags = flags;
			use_pfxlist = pfxlist;
			use_pfxlen = pfxlen;
			need_affix = FALSE;
			use_condit = condit | CONDIT_COMB | CONDIT_AFF;
			ಯದಿ (ae->ae_flags != ರಿಕ್ತ)
			{
			    // Extract flags from the affix list.
			    use_flags |= get_affix_flags(affile, ae->ae_flags);

			    ಯದಿ (affile->af_needaffix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_needaffix))
				need_affix = TRUE;

			    // When there is a CIRCUMFIX flag the other affix
			    // must also have it and we don't add the word
			    // with one affix.
			    ಯದಿ (affile->af_circumfix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_circumfix))
			    {
				use_condit |= CONDIT_CFIX;
				ಯದಿ ((condit & CONDIT_CFIX) == 0)
				    need_affix = TRUE;
			    }

			    ಯದಿ (affile->af_pfxpostpone
						|| spin->si_compflags != ರಿಕ್ತ)
			    {
				ಪೂರ್ಣಾಙ್ಕ listlen = 0;

				ಯದಿ (affile->af_pfxpostpone)
				{
				    // Get prefix IDS from the affix list.
				    ಯದಿ (get_pfxlist(affile, ae->ae_flags,
						    store_afflist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					ಭಙ್ಗ;
				    }
				    use_pfxlen = listlen;
				}
				ಅನ್ಯಥಾ
				    use_pfxlen = 0;
				use_pfxlist = store_afflist;

				// Combine the prefix IDs. Avoid adding the
				// same ID twice.
				ಪ್ರತಿ (i = 0; i < pfxlen; ++i)
				{
				    ಪ್ರತಿ (j = 0; j < use_pfxlen; ++j)
					ಯದಿ (pfxlist[i] == use_pfxlist[j])
					    ಭಙ್ಗ;
				    ಯದಿ (j == use_pfxlen
					    && store_afflist_add(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					ಭಙ್ಗ;
				    }
				    use_pfxlen = listlen;
				}
				ಯದಿ (retval == FAIL)
				    ಭಙ್ಗ;

				ಯದಿ (spin->si_compflags != ರಿಕ್ತ)
				    // Get compound IDS from the affix list.
				    ಯದಿ (get_compflags(affile, ae->ae_flags,
						      use_pfxlist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					ಭಙ್ಗ;
				    }
				ಯದಿ (retval == FAIL)
				    ಭಙ್ಗ;

				// Combine the list of compound flags.
				// Concatenate them to the prefix IDs list.
				// Avoid adding the same ID twice.
				ಪ್ರತಿ (i = pfxlen; pfxlist[i] != NUL; ++i)
				{
				    ಪ್ರತಿ (j = use_pfxlen;
						   use_pfxlist[j] != NUL; ++j)
					ಯದಿ (pfxlist[i] == use_pfxlist[j])
					    ಭಙ್ಗ;
				    ಯದಿ (use_pfxlist[j] == NUL
					    && store_afflist_add(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					ಭಙ್ಗ;
				    }
				}
				ಯದಿ (retval == FAIL)
				    ಭಙ್ಗ;
			    }
			}

			// Obey a "COMPOUNDFORBIDFLAG" of the affix: don't
			// use the compound flags.
			ಯದಿ (use_pfxlist != ರಿಕ್ತ && ae->ae_compforbid)
			{
			    vim_strncpy(pfx_pfxlist, use_pfxlist, use_pfxlen);
			    use_pfxlist = pfx_pfxlist;
			}

			// When there are postponed prefixes...
			ಯದಿ (spin->si_prefroot != ರಿಕ್ತ
				&& spin->si_prefroot->wn_sibling != ರಿಕ್ತ)
			{
			    // ... add a flag to indicate an affix was used.
			    use_flags |= WF_HAS_AFF;

			    // ... don't use a prefix list if combining
			    // affixes is not allowed.  But do use the
			    // compound flags after them.
			    ಯದಿ (!ah->ah_combine && use_pfxlist != ರಿಕ್ತ)
				use_pfxlist += use_pfxlen;
			}

			// When compounding is supported and there is no
			// "COMPOUNDPERMITFLAG" then forbid compounding on the
			// side where the affix is applied.
			ಯದಿ (spin->si_compflags != ರಿಕ್ತ && !ae->ae_comppermit)
			{
			    ಯದಿ (xht != ರಿಕ್ತ)
				use_flags |= WF_NOCOMPAFT;
			    ಅನ್ಯಥಾ
				use_flags |= WF_NOCOMPBEF;
			}

			// Store the modified word.
			ಯದಿ (store_word(spin, newword, use_flags,
						 spin->si_region, use_pfxlist,
							  need_affix) == FAIL)
			    retval = FAIL;

			// When added a prefix or a first suffix and the affix
			// has flags may add a(nother) suffix.  RECURSIVE!
			ಯದಿ ((condit & CONDIT_SUF) && ae->ae_flags != ರಿಕ್ತ)
			    ಯದಿ (store_aff_word(spin, newword, ae->ae_flags,
					affile, &affile->af_suff, xht,
					   use_condit & (xht == ರಿಕ್ತ
							? ~0 :  ~CONDIT_SUF),
				      use_flags, use_pfxlist, pfxlen) == FAIL)
				retval = FAIL;

			// When added a suffix and combining is allowed also
			// try adding a prefix additionally.  Both for the
			// word flags and for the affix flags.  RECURSIVE!
			ಯದಿ (xht != ರಿಕ್ತ && ah->ah_combine)
			{
			    ಯದಿ (store_aff_word(spin, newword,
					afflist, affile,
					xht, ರಿಕ್ತ, use_condit,
					use_flags, use_pfxlist,
					pfxlen) == FAIL
				    || (ae->ae_flags != ರಿಕ್ತ
					&& store_aff_word(spin, newword,
					    ae->ae_flags, affile,
					    xht, ರಿಕ್ತ, use_condit,
					    use_flags, use_pfxlist,
					    pfxlen) == FAIL))
				retval = FAIL;
			}
		    }
		}
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Read a file with a list of words.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
spell_read_wordfile(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    long	lnum = 0;
    char_u	rline[MAXLINELEN];
    char_u	*ಸಾಲು;
    char_u	*pc = ರಿಕ್ತ;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		l;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
    ಪೂರ್ಣಾಙ್ಕ		did_word = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		non_ascii = 0;
    ಪೂರ್ಣಾಙ್ಕ		flags;
    ಪೂರ್ಣಾಙ್ಕ		regionmask;

    /*
     * Open the file.
     */
    fd = mch_fopen((ವರ್ಣ *)fname, "r");
    ಯದಿ (fd == ರಿಕ್ತ)
    {
	semsg(_(e_cant_open_file_str), fname);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, _("Reading word file %s..."), fname);
    spell_message(spin, IObuff);

    /*
     * Read all the lines in the file one by one.
     */
    ಯಾವತ್ (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	ಸಾಲು_breakcheck();
	++lnum;

	// Skip comment lines.
	ಯದಿ (*rline == '#')
	    ಅನುವರ್ತ;

	// Remove CR, LF and white space from the end.
	l = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(rline);
	ಯಾವತ್ (l > 0 && rline[l - 1] <= ' ')
	    --l;
	ಯದಿ (l == 0)
	    ಅನುವರ್ತ;	// empty or blank line
	rline[l] = NUL;

	// Convert from "/encoding={encoding}" to 'encoding' when needed.
	vim_free(pc);
	ಯದಿ (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, ರಿಕ್ತ);
	    ಯದಿ (pc == ರಿಕ್ತ)
	    {
		smsg(_("Conversion failure for word in %s line %ld: %s"),
							   fname, lnum, rline);
		ಅನುವರ್ತ;
	    }
	    ಸಾಲು = pc;
	}
	ಅನ್ಯಥಾ
	{
	    pc = ರಿಕ್ತ;
	    ಸಾಲು = rline;
	}

	ಯದಿ (*ಸಾಲು == '/')
	{
	    ++ಸಾಲು;
	    ಯದಿ (STRNCMP(ಸಾಲು, "encoding=", 9) == 0)
	    {
		ಯದಿ (spin->si_conv.vc_type != CONV_NONE)
		    smsg(_("Duplicate /encoding= line ignored in %s line %ld: %s"),
						       fname, lnum, ಸಾಲು - 1);
		ಅನ್ಯಥಾ ಯದಿ (did_word)
		    smsg(_("/encoding= line after word ignored in %s line %ld: %s"),
						       fname, lnum, ಸಾಲು - 1);
		ಅನ್ಯಥಾ
		{
		    char_u	*enc;

		    // Setup for conversion to 'encoding'.
		    ಸಾಲು += 9;
		    enc = enc_canonize(ಸಾಲು);
		    ಯದಿ (enc != ರಿಕ್ತ && !spin->si_ascii
			    && convert_setup(&spin->si_conv, enc,
							       p_enc) == FAIL)
			smsg(_("Conversion in %s not supported: from %s to %s"),
							  fname, ಸಾಲು, p_enc);
		    vim_free(enc);
		    spin->si_conv.vc_fail = TRUE;
		}
		ಅನುವರ್ತ;
	    }

	    ಯದಿ (STRNCMP(ಸಾಲು, "regions=", 8) == 0)
	    {
		ಯದಿ (spin->si_region_ಎಣಿಕೆ > 1)
		    smsg(_("Duplicate /regions= line ignored in %s line %ld: %s"),
						       fname, lnum, ಸಾಲು);
		ಅನ್ಯಥಾ
		{
		    ಸಾಲು += 8;
		    ಯದಿ (STRLEN(ಸಾಲು) > MAXREGIONS * 2)
			smsg(_("Too many regions in %s line %ld: %s"),
						       fname, lnum, ಸಾಲು);
		    ಅನ್ಯಥಾ
		    {
			spin->si_region_ಎಣಿಕೆ = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಸಾಲು) / 2;
			STRCPY(spin->si_region_name, ಸಾಲು);

			// Adjust the mask for a word valid in all regions.
			spin->si_region = (1 << spin->si_region_ಎಣಿಕೆ) - 1;
		    }
		}
		ಅನುವರ್ತ;
	    }

	    smsg(_("/ line ignored in %s line %ld: %s"),
						       fname, lnum, ಸಾಲು - 1);
	    ಅನುವರ್ತ;
	}

	flags = 0;
	regionmask = spin->si_region;

	// Check for flags and region after a slash.
	p = vim_strchr(ಸಾಲು, '/');
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    *p++ = NUL;
	    ಯಾವತ್ (*p != NUL)
	    {
		ಯದಿ (*p == '=')		// keep-case word
		    flags |= WF_KEEPCAP | WF_FIXCAP;
		ಅನ್ಯಥಾ ಯದಿ (*p == '!')	// Bad, bad, wicked word.
		    flags |= WF_BANNED;
		ಅನ್ಯಥಾ ಯದಿ (*p == '?')	// Rare word.
		    flags |= WF_RARE;
		ಅನ್ಯಥಾ ಯದಿ (VIM_ISDIGIT(*p)) // region number(s)
		{
		    ಯದಿ ((flags & WF_REGION) == 0)   // first one
			regionmask = 0;
		    flags |= WF_REGION;

		    l = *p - '0';
		    ಯದಿ (l == 0 || l > spin->si_region_ಎಣಿಕೆ)
		    {
			smsg(_("Invalid region nr in %s line %ld: %s"),
							  fname, lnum, p);
			ಭಙ್ಗ;
		    }
		    regionmask |= 1 << (l - 1);
		}
		ಅನ್ಯಥಾ
		{
		    smsg(_("Unrecognized flags in %s line %ld: %s"),
							      fname, lnum, p);
		    ಭಙ್ಗ;
		}
		++p;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	ಯದಿ (spin->si_ascii && has_non_ascii(ಸಾಲು))
	{
	    ++non_ascii;
	    ಅನುವರ್ತ;
	}

	// Normal word: store it.
	ಯದಿ (store_word(spin, ಸಾಲು, flags, regionmask, ರಿಕ್ತ, FALSE) == FAIL)
	{
	    retval = FAIL;
	    ಭಙ್ಗ;
	}
	did_word = TRUE;
    }

    vim_free(pc);
    fclose(fd);

    ಯದಿ (spin->si_ascii && non_ascii > 0)
    {
	vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE,
		  _("Ignored %d words with non-ASCII characters"), non_ascii);
	spell_message(spin, IObuff);
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Get part of an sblock_T, "len" bytes long.
 * This avoids calling free() for every little struct we use (and keeping
 * track of them).
 * The memory is cleared to all zeros.
 * Returns NULL when out of memory.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ *
getroom(
    spellinfo_T *spin,
    size_t	len,		// length needed
    ಪೂರ್ಣಾಙ್ಕ		align)		// align for pointer
{
    char_u	*p;
    sblock_T	*bl = spin->si_blocks;

    ಯದಿ (align && bl != ರಿಕ್ತ)
	// Round size up for alignment.  On some systems structures need to be
	// aligned to the size of a pointer (e.g., SPARC).
	bl->sb_used = (bl->sb_used + ಪರಿಮಾಣ(ವರ್ಣ *) - 1)
						      & ~(ಪರಿಮಾಣ(ವರ್ಣ *) - 1);

    ಯದಿ (bl == ರಿಕ್ತ || bl->sb_used + len > SBLOCKSIZE)
    {
	ಯದಿ (len >= SBLOCKSIZE)
	    bl = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	    // Allocate a block of memory. It is not freed until much later.
	    bl = alloc_clear(offsetof(sblock_T, sb_data) + SBLOCKSIZE + 1);
	ಯದಿ (bl == ರಿಕ್ತ)
	{
	    ಯದಿ (!spin->si_did_emsg)
	    {
		emsg(_(e_insufficient_memory_word_list_will_be_incomplete));
		spin->si_did_emsg = TRUE;
	    }
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	bl->sb_next = spin->si_blocks;
	spin->si_blocks = bl;
	bl->sb_used = 0;
	++spin->si_blocks_cnt;
    }

    p = bl->sb_data + bl->sb_used;
    bl->sb_used += (ಪೂರ್ಣಾಙ್ಕ)len;

    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Make a copy of a string into memory allocated with getroom().
 * Returns NULL when out of memory.
 */
    ಸ್ಥಿರ char_u *
getroom_save(spellinfo_T *spin, char_u *s)
{
    char_u	*sc;

    sc = (char_u *)getroom(spin, STRLEN(s) + 1, FALSE);
    ಯದಿ (sc != ರಿಕ್ತ)
	STRCPY(sc, s);
    ಪ್ರತ್ಯಾಗಮ sc;
}


/*
 * Free the list of allocated sblock_T.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_blocks(sblock_T *bl)
{
    sblock_T	*next;

    ಯಾವತ್ (bl != ರಿಕ್ತ)
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
    ಸ್ಥಿರ wordnode_T *
wordtree_alloc(spellinfo_T *spin)
{
    ಪ್ರತ್ಯಾಗಮ (wordnode_T *)getroom(spin, ಪರಿಮಾಣ(wordnode_T), TRUE);
}

/*
 * Return TRUE if "word" contains valid word characters.
 * Control characters and trailing '/' are invalid.  Space is OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
valid_spell_word(char_u *word, char_u *end)
{
    char_u *p;

    ಯದಿ (enc_utf8 && !utf_valid_string(word, end))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತಿ (p = word; *p != NUL && p < end; p += mb_ptr2len(p))
	ಯದಿ (*p < ' ' || (p[0] == '/' && p[1] == NUL))
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ TRUE;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
store_word(
    spellinfo_T	*spin,
    char_u	*word,
    ಪೂರ್ಣಾಙ್ಕ		flags,		// extra flags, WF_BANNED
    ಪೂರ್ಣಾಙ್ಕ		region,		// supported region(s)
    char_u	*pfxlist,	// list of prefix IDs or NULL
    ಪೂರ್ಣಾಙ್ಕ		need_affix)	// only store word with affix ID
{
    ಪೂರ್ಣಾಙ್ಕ		len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(word);
    ಪೂರ್ಣಾಙ್ಕ		ct = captype(word, word + len);
    char_u	foldword[MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		res = OK;
    char_u	*p;

    // Avoid adding illegal bytes to the word tree.
    ಯದಿ (!valid_spell_word(word, word + len))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    (ಶೂನ್ಯ)spell_casefold(curwin, word, len, foldword, MAXWLEN);
    ಪ್ರತಿ (p = pfxlist; res == OK; ++p)
    {
	ಯದಿ (!need_affix || (p != ರಿಕ್ತ && *p != NUL))
	    res = ವೃಕ್ಷ_add_word(spin, foldword, spin->si_foldroot, ct | flags,
						  region, p == ರಿಕ್ತ ? 0 : *p);
	ಯದಿ (p == ರಿಕ್ತ || *p == NUL)
	    ಭಙ್ಗ;
    }
    ++spin->si_foldwcount;

    ಯದಿ (res == OK && (ct == WF_KEEPCAP || (flags & WF_KEEPCAP)))
    {
	ಪ್ರತಿ (p = pfxlist; res == OK; ++p)
	{
	    ಯದಿ (!need_affix || (p != ರಿಕ್ತ && *p != NUL))
		res = ವೃಕ್ಷ_add_word(spin, word, spin->si_keeproot, flags,
						  region, p == ರಿಕ್ತ ? 0 : *p);
	    ಯದಿ (p == ರಿಕ್ತ || *p == NUL)
		ಭಙ್ಗ;
	}
	++spin->si_keepwcount;
    }
    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Add word "word" to a word tree at "root".
 * When "flags" < 0 we are adding to the prefix tree where "flags" is used for
 * "rare" and "region" is the condition nr.
 * Returns FAIL when out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ವೃಕ್ಷ_add_word(
    spellinfo_T	*spin,
    char_u	*word,
    wordnode_T	*ಬೇರು,
    ಪೂರ್ಣಾಙ್ಕ		flags,
    ಪೂರ್ಣಾಙ್ಕ		region,
    ಪೂರ್ಣಾಙ್ಕ		affixID)
{
    wordnode_T	*ಗಂಟು = ಬೇರು;
    wordnode_T	*np;
    wordnode_T	*copyp, **copyprev;
    wordnode_T	**prev = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		i;

    // Add each byte of the word to the tree, including the NUL at the end.
    ಪ್ರತಿ (i = 0; ; ++i)
    {
	// When there is more than one reference to this node we need to make
	// a copy, so that we can modify it.  Copy the whole list of siblings
	// (we don't optimize for a partly shared list of siblings).
	ಯದಿ (ಗಂಟು != ರಿಕ್ತ && ಗಂಟು->wn_refs > 1)
	{
	    --ಗಂಟು->wn_refs;
	    copyprev = prev;
	    FOR_ALL_ಗಂಟು_SIBLINGS(ಗಂಟು, copyp)
	    {
		// Allocate a new node and copy the info.
		np = get_wordnode(spin);
		ಯದಿ (np == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		np->wn_child = copyp->wn_child;
		ಯದಿ (np->wn_child != ರಿಕ್ತ)
		    ++np->wn_child->wn_refs;	// child gets extra ref
		np->wn_byte = copyp->wn_byte;
		ಯದಿ (np->wn_byte == NUL)
		{
		    np->wn_flags = copyp->wn_flags;
		    np->wn_region = copyp->wn_region;
		    np->wn_affixID = copyp->wn_affixID;
		}

		// Link the new node in the list, there will be one ref.
		np->wn_refs = 1;
		ಯದಿ (copyprev != ರಿಕ್ತ)
		    *copyprev = np;
		copyprev = &np->wn_sibling;

		// Let "node" point to the head of the copied list.
		ಯದಿ (copyp == ಗಂಟು)
		    ಗಂಟು = np;
	    }
	}

	// Look for the sibling that has the same character.  They are sorted
	// on byte value, thus stop searching when a sibling is found with a
	// higher byte value.  For zero bytes (end of word) the sorting is
	// done on flags and then on affixID.
	ಯಾವತ್ (ಗಂಟು != ರಿಕ್ತ
		&& (ಗಂಟು->wn_byte < word[i]
		    || (ಗಂಟು->wn_byte == NUL
			&& (flags < 0
			    ? ಗಂಟು->wn_affixID < (unsigned)affixID
			    : (ಗಂಟು->wn_flags < (unsigned)(flags & WN_MASK)
				|| (ಗಂಟು->wn_flags == (flags & WN_MASK)
				    && (spin->si_sugtree
					? (ಗಂಟು->wn_region & 0xffff) < region
					: ಗಂಟು->wn_affixID
						    < (unsigned)affixID)))))))
	{
	    prev = &ಗಂಟು->wn_sibling;
	    ಗಂಟು = *prev;
	}
	ಯದಿ (ಗಂಟು == ರಿಕ್ತ
		|| ಗಂಟು->wn_byte != word[i]
		|| (word[i] == NUL
		    && (flags < 0
			|| spin->si_sugtree
			|| ಗಂಟು->wn_flags != (flags & WN_MASK)
			|| ಗಂಟು->wn_affixID != affixID)))
	{
	    // Allocate a new node.
	    np = get_wordnode(spin);
	    ಯದಿ (np == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    np->wn_byte = word[i];

	    // If "node" is NULL this is a new child or the end of the sibling
	    // list: ref count is one.  Otherwise use ref count of sibling and
	    // make ref count of sibling one (matters when inserting in front
	    // of the list of siblings).
	    ಯದಿ (ಗಂಟು == ರಿಕ್ತ)
		np->wn_refs = 1;
	    ಅನ್ಯಥಾ
	    {
		np->wn_refs = ಗಂಟು->wn_refs;
		ಗಂಟು->wn_refs = 1;
	    }
	    ಯದಿ (prev != ರಿಕ್ತ)
		*prev = np;
	    np->wn_sibling = ಗಂಟು;
	    ಗಂಟು = np;
	}

	ಯದಿ (word[i] == NUL)
	{
	    ಗಂಟು->wn_flags = flags;
	    ಗಂಟು->wn_region |= region;
	    ಗಂಟು->wn_affixID = affixID;
	    ಭಙ್ಗ;
	}
	prev = &ಗಂಟು->wn_child;
	ಗಂಟು = *prev;
    }
#ifdef SPELL_PRINTTREE
    smsg("Added \"%s\"", word);
    spell_print_ವೃಕ್ಷ(ಬೇರು->wn_sibling);
#endif

    // count nr of words added since last message
    ++spin->si_ಸಂದೇಶ_ಎಣಿಕೆ;

    ಯದಿ (spin->si_compress_cnt > 1)
    {
	ಯದಿ (--spin->si_compress_cnt == 1)
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
    ಯದಿ (spin->si_compress_cnt == 1
	    ? spin->si_free_ಎಣಿಕೆ < MAXWLEN
	    : spin->si_blocks_cnt >= compress_start)
#endif
    {
	// Decrement the block counter.  The effect is that we compress again
	// when the freed up room has been used and another "compress_inc"
	// blocks have been allocated.  Unless "compress_added" words have
	// been added, then the limit is put back again.
	spin->si_blocks_cnt -= compress_inc;
	spin->si_compress_cnt = compress_added;

	ಯದಿ (spin->si_verbose)
	{
	    ಸಂದೇಶ_start();
	    ಸಂದೇಶ_puts(_(ಸಂದೇಶ_compressing));
	    ಸಂದೇಶ_clr_eos();
	    ಸಂದೇಶ_didout = FALSE;
	    ಸಂದೇಶ_col = 0;
	    out_flush();
	}

	// Compress both trees.  Either they both have many nodes, which makes
	// compression useful, or one of them is small, which means
	// compression goes fast.  But when filling the soundfold word tree
	// there is no keep-case tree.
	wordtree_compress(spin, spin->si_foldroot, "case-folded");
	ಯದಿ (affixID >= 0)
	    wordtree_compress(spin, spin->si_keeproot, "keep-case");
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get a wordnode_T, either from the list of previously freed nodes or
 * allocate a new one.
 * Returns NULL when out of memory.
 */
    ಸ್ಥಿರ wordnode_T *
get_wordnode(spellinfo_T *spin)
{
    wordnode_T *n;

    ಯದಿ (spin->si_first_free == ರಿಕ್ತ)
	n = (wordnode_T *)getroom(spin, ಪರಿಮಾಣ(wordnode_T), TRUE);
    ಅನ್ಯಥಾ
    {
	n = spin->si_first_free;
	spin->si_first_free = n->wn_child;
	CLEAR_POINTER(n);
	--spin->si_free_ಎಣಿಕೆ;
    }
#ifdef SPELL_PRINTTREE
    ಯದಿ (n != ರಿಕ್ತ)
	n->wn_nr = ++spin->si_wordnode_nr;
#endif
    ಪ್ರತ್ಯಾಗಮ n;
}

/*
 * Decrement the reference count on a node (which is the head of a list of
 * siblings).  If the reference count becomes zero free the node and its
 * siblings.
 * Returns the number of nodes actually freed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
deref_wordnode(spellinfo_T *spin, wordnode_T *ಗಂಟು)
{
    wordnode_T	*np;
    ಪೂರ್ಣಾಙ್ಕ		cnt = 0;

    ಯದಿ (--ಗಂಟು->wn_refs == 0)
    {
	FOR_ALL_ಗಂಟು_SIBLINGS(ಗಂಟು, np)
	{
	    ಯದಿ (np->wn_child != ರಿಕ್ತ)
		cnt += deref_wordnode(spin, np->wn_child);
	    free_wordnode(spin, np);
	    ++cnt;
	}
	++cnt;	    // length field
    }
    ಪ್ರತ್ಯಾಗಮ cnt;
}

/*
 * Free a wordnode_T for re-use later.
 * Only the "wn_child" field becomes invalid.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_wordnode(spellinfo_T *spin, wordnode_T *n)
{
    n->wn_child = spin->si_first_free;
    spin->si_first_free = n;
    ++spin->si_free_ಎಣಿಕೆ;
}

/*
 * Compress a tree: find tails that are identical and can be shared.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wordtree_compress(spellinfo_T *spin, wordnode_T *ಬೇರು, ವರ್ಣ *name)
{
    hashtab_T	    ht;
    long	    n;
    long	    tot = 0;
    long	    perc;

    // Skip the root itself, it's not actually used.  The first sibling is the
    // start of the tree.
    ಯದಿ (ಬೇರು->wn_sibling == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    hash_init(&ht);
    n = ಗಂಟು_compress(spin, ಬೇರು->wn_sibling, &ht, &tot);

#ifndef SPELL_PRINTTREE
    ಯದಿ (spin->si_verbose || p_verbose > 2)
#endif
    {
	ಯದಿ (tot > 1000000)
	    perc = (tot - n) / (tot / 100);
	ಅನ್ಯಥಾ ಯದಿ (tot == 0)
	    perc = 0;
	ಅನ್ಯಥಾ
	    perc = (tot - n) * 100 / tot;
	vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE,
		_("Compressed %s: %ld of %ld nodes; %ld (%ld%%) remaining"),
		name, n, tot, tot - n, perc);
	spell_message(spin, IObuff);
    }
#ifdef SPELL_PRINTTREE
    spell_print_ವೃಕ್ಷ(ಬೇರು->wn_sibling);
#endif
    hash_clear(&ht);
}

/*
 * Compress a node, its siblings and its children, depth first.
 * Returns the number of compressed nodes.
 */
    ಸ್ಥಿರ long
ಗಂಟು_compress(
    spellinfo_T	*spin,
    wordnode_T	*ಗಂಟು,
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
    ಪ್ರತಿ (np = ಗಂಟು; np != ರಿಕ್ತ && !got_int; np = np->wn_sibling)
    {
	++len;
	ಯದಿ ((child = np->wn_child) != ರಿಕ್ತ)
	{
	    // Compress the child first.  This fills hashkey.
	    compressed += ಗಂಟು_compress(spin, child, ht, tot);

	    // Try to find an identical child.
	    hash = hash_hash(child->wn_u1.hashkey);
	    hi = hash_lookup(ht, child->wn_u1.hashkey, hash);
	    ಯದಿ (!HASHITEM_EMPTY(hi))
	    {
		// There are children we encountered before with a hash value
		// identical to the current child.  Now check if there is one
		// that is really identical.
		ಪ್ರತಿ (tp = HI2WN(hi); tp != ರಿಕ್ತ; tp = tp->wn_u2.next)
		    ಯದಿ (ಗಂಟು_equal(child, tp))
		    {
			// Found one!  Now use that child in place of the
			// current one.  This means the current child and all
			// its siblings is unlinked from the tree.
			++tp->wn_refs;
			compressed += deref_wordnode(spin, child);
			np->wn_child = tp;
			ಭಙ್ಗ;
		    }
		ಯದಿ (tp == ರಿಕ್ತ)
		{
		    // No other child with this hash value equals the child of
		    // the node, add it to the linked list after the first
		    // item.
		    tp = HI2WN(hi);
		    child->wn_u2.next = tp->wn_u2.next;
		    tp->wn_u2.next = child;
		}
	    }
	    ಅನ್ಯಥಾ
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
    ಗಂಟು->wn_u1.hashkey[0] = len;
    nr = 0;
    FOR_ALL_ಗಂಟು_SIBLINGS(ಗಂಟು, np)
    {
	ಯದಿ (np->wn_byte == NUL)
	    // end node: use wn_flags, wn_region and wn_affixID
	    n = np->wn_flags + (np->wn_region << 8) + (np->wn_affixID << 16);
	ಅನ್ಯಥಾ
	    // byte node: use the byte value and the child pointer
	    n = (unsigned)(np->wn_byte + ((long_u)np->wn_child << 8));
	nr = nr * 101 + n;
    }

    // Avoid NUL bytes, it terminates the hash key.
    n = nr & 0xff;
    ಗಂಟು->wn_u1.hashkey[1] = n == 0 ? 1 : n;
    n = (nr >> 8) & 0xff;
    ಗಂಟು->wn_u1.hashkey[2] = n == 0 ? 1 : n;
    n = (nr >> 16) & 0xff;
    ಗಂಟು->wn_u1.hashkey[3] = n == 0 ? 1 : n;
    n = (nr >> 24) & 0xff;
    ಗಂಟು->wn_u1.hashkey[4] = n == 0 ? 1 : n;
    ಗಂಟು->wn_u1.hashkey[5] = NUL;

    // Check for CTRL-C pressed now and then.
    veryfast_breakcheck();

    ಪ್ರತ್ಯಾಗಮ compressed;
}

/*
 * Return TRUE when two nodes have identical siblings and children.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ಗಂಟು_equal(wordnode_T *n1, wordnode_T *n2)
{
    wordnode_T	*p1;
    wordnode_T	*p2;

    ಪ್ರತಿ (p1 = n1, p2 = n2; p1 != ರಿಕ್ತ && p2 != ರಿಕ್ತ;
				     p1 = p1->wn_sibling, p2 = p2->wn_sibling)
	ಯದಿ (p1->wn_byte != p2->wn_byte
		|| (p1->wn_byte == NUL
		    ? (p1->wn_flags != p2->wn_flags
			|| p1->wn_region != p2->wn_region
			|| p1->wn_affixID != p2->wn_affixID)
		    : (p1->wn_child != p2->wn_child)))
	    ಭಙ್ಗ;

    ಪ್ರತ್ಯಾಗಮ p1 == ರಿಕ್ತ && p2 == ರಿಕ್ತ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rep_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2);

/*
 * Function given to qsort() to sort the REP items on "from" string.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
rep_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    fromto_T	*p1 = (fromto_T *)s1;
    fromto_T	*p2 = (fromto_T *)s2;

    ಪ್ರತ್ಯಾಗಮ STRCMP(p1->ft_from, p2->ft_from);
}

/*
 * Write the Vim .spl file "fname".
 * Return FAIL or OK;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
write_vim_spell(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    ಪೂರ್ಣಾಙ್ಕ		regionmask;
    ಪೂರ್ಣಾಙ್ಕ		round;
    wordnode_T	*ವೃಕ್ಷ;
    ಪೂರ್ಣಾಙ್ಕ		nodecount;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		l;
    garray_T	*gap;
    fromto_T	*ftp;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		rr;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
    size_t	fwv = 1;  // collect return value of fwrite() to avoid
			  // warnings from picky compiler

    fd = mch_fopen((ವರ್ಣ *)fname, "w");
    ಯದಿ (fd == ರಿಕ್ತ)
    {
	semsg(_(e_cant_open_file_str), fname);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // <HEADER>: <fileID> <versionnr>
							    // <fileID>
    fwv &= fwrite(VIMSPELLMAGIC, VIMSPELLMAGICL, (size_t)1, fd);
    ಯದಿ (fwv != (size_t)1)
	// Catch first write error, don't try writing more.
	ಗಚ್ಛ theend;

    putc(VIMSPELLVERSION, fd);				    // <versionnr>

    /*
     * <SECTIONS>: <section> ... <sectionend>
     */

    // SN_INFO: <infotext>
    ಯದಿ (spin->si_info != ರಿಕ್ತ)
    {
	putc(SN_INFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	i = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(spin->si_info);
	put_bytes(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_info, (size_t)i, (size_t)1, fd); // <infotext>
    }

    // SN_REGION: <regionname> ...
    // Write the region names only if there is more than one.
    ಯದಿ (spin->si_region_ಎಣಿಕೆ > 1)
    {
	putc(SN_REGION, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>
	l = spin->si_region_ಎಣಿಕೆ * 2;
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_region_name, (size_t)l, (size_t)1, fd);
							// <regionname> ...
	regionmask = (1 << spin->si_region_ಎಣಿಕೆ) - 1;
    }
    ಅನ್ಯಥಾ
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
    ಯದಿ (!spin->si_ascii && !spin->si_add)
    {
	char_u	folchars[128 * 8];
	ಪೂರ್ಣಾಙ್ಕ	flags;

	putc(SN_CHARFLAGS, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	// Form the <folchars> string first, we need to know its length.
	l = 0;
	ಪ್ರತಿ (i = 128; i < 256; ++i)
	{
	    ಯದಿ (has_mbyte)
		l += mb_char2bytes(spelltab.st_fold[i], folchars + l);
	    ಅನ್ಯಥಾ
		folchars[l++] = spelltab.st_fold[i];
	}
	put_bytes(fd, (long_u)(1 + 128 + 2 + l), 4);	// <sectionlen>

	fputc(128, fd);					// <charflagslen>
	ಪ್ರತಿ (i = 128; i < 256; ++i)
	{
	    flags = 0;
	    ಯದಿ (spelltab.st_isw[i])
		flags |= CF_WORD;
	    ಯದಿ (spelltab.st_isu[i])
		flags |= CF_UPPER;
	    fputc(flags, fd);				// <charflags>
	}

	put_bytes(fd, (long_u)l, 2);			// <folcharslen>
	fwv &= fwrite(folchars, (size_t)l, (size_t)1, fd); // <folchars>
    }

    // SN_MIDWORD: <midword>
    ಯದಿ (spin->si_midword != ರಿಕ್ತ)
    {
	putc(SN_MIDWORD, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	i = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(spin->si_midword);
	put_bytes(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_midword, (size_t)i, (size_t)1, fd);
							// <midword>
    }

    // SN_PREFCOND: <prefcondcnt> <prefcond> ...
    ಯದಿ (spin->si_prefcond.ga_len > 0)
    {
	putc(SN_PREFCOND, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	l = write_spell_prefcond(ರಿಕ್ತ, &spin->si_prefcond, &fwv);
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>

	write_spell_prefcond(fd, &spin->si_prefcond, &fwv);
    }

    // SN_REP: <repcount> <rep> ...
    // SN_SAL: <salflags> <salcount> <sal> ...
    // SN_REPSAL: <repcount> <rep> ...

    // round 1: SN_REP section
    // round 2: SN_SAL section (unless SN_SOFO is used)
    // round 3: SN_REPSAL section
    ಪ್ರತಿ (round = 1; round <= 3; ++round)
    {
	ಯದಿ (round == 1)
	    gap = &spin->si_rep;
	ಅನ್ಯಥಾ ಯದಿ (round == 2)
	{
	    // Don't write SN_SAL when using a SN_SOFO section
	    ಯದಿ (spin->si_sofofr != ರಿಕ್ತ && spin->si_sofoto != ರಿಕ್ತ)
		ಅನುವರ್ತ;
	    gap = &spin->si_sal;
	}
	ಅನ್ಯಥಾ
	    gap = &spin->si_repsal;

	// Don't write the section if there are no items.
	ಯದಿ (gap->ga_len == 0)
	    ಅನುವರ್ತ;

	// Sort the REP/REPSAL items.
	ಯದಿ (round != 2)
	    qsort(gap->ga_data, (size_t)gap->ga_len,
					       ಪರಿಮಾಣ(fromto_T), rep_compare);

	i = round == 1 ? SN_REP : (round == 2 ? SN_SAL : SN_REPSAL);
	putc(i, fd);					// <sectionID>

	// This is for making suggestions, section is not required.
	putc(0, fd);					// <sectionflags>

	// Compute the length of what follows.
	l = 2;	    // count <repcount> or <salcount>
	ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
	{
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    l += 1 + (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ftp->ft_from);  // count <*fromlen> and <*from>
	    l += 1 + (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ftp->ft_to);    // count <*tolen> and <*to>
	}
	ಯದಿ (round == 2)
	    ++l;	// count <salflags>
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>

	ಯದಿ (round == 2)
	{
	    i = 0;
	    ಯದಿ (spin->si_followup)
		i |= SAL_F0LLOWUP;
	    ಯದಿ (spin->si_collapse)
		i |= SAL_COLLAPSE;
	    ಯದಿ (spin->si_rem_accents)
		i |= SAL_REM_ACCENTS;
	    putc(i, fd);			// <salflags>
	}

	put_bytes(fd, (long_u)gap->ga_len, 2);	// <repcount> or <salcount>
	ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
	{
	    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
	    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    ಪ್ರತಿ (rr = 1; rr <= 2; ++rr)
	    {
		p = rr == 1 ? ftp->ft_from : ftp->ft_to;
		l = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);
		putc(l, fd);
		ಯದಿ (l > 0)
		    fwv &= fwrite(p, l, (size_t)1, fd);
	    }
	}

    }

    // SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
    // This is for making suggestions, section is not required.
    ಯದಿ (spin->si_sofofr != ರಿಕ್ತ && spin->si_sofoto != ರಿಕ್ತ)
    {
	putc(SN_SOFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(spin->si_sofofr);
	put_bytes(fd, (long_u)(l + STRLEN(spin->si_sofoto) + 4), 4);
							// <sectionlen>

	put_bytes(fd, (long_u)l, 2);			// <sofofromlen>
	fwv &= fwrite(spin->si_sofofr, l, (size_t)1, fd); // <sofofrom>

	l = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(spin->si_sofoto);
	put_bytes(fd, (long_u)l, 2);			// <sofotolen>
	fwv &= fwrite(spin->si_sofoto, l, (size_t)1, fd); // <sofoto>
    }

    // SN_WORDS: <word> ...
    // This is for making suggestions, section is not required.
    ಯದಿ (spin->si_commonwords.ht_used > 0)
    {
	putc(SN_ಪದಗಳು, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// round 1: count the bytes
	// round 2: write the bytes
	ಪ್ರತಿ (round = 1; round <= 2; ++round)
	{
	    ಪೂರ್ಣಾಙ್ಕ		todo;
	    ಪೂರ್ಣಾಙ್ಕ		len = 0;
	    hashitem_T	*hi;

	    todo = (ಪೂರ್ಣಾಙ್ಕ)spin->si_commonwords.ht_used;
	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&spin->si_commonwords, hi, todo)
		ಯದಿ (!HASHITEM_EMPTY(hi))
		{
		    l = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(hi->hi_ಕೀಲಿ) + 1;
		    len += l;
		    ಯದಿ (round == 2)			// <word>
			fwv &= fwrite(hi->hi_ಕೀಲಿ, (size_t)l, (size_t)1, fd);
		    --todo;
		}
	    ಯದಿ (round == 1)
		put_bytes(fd, (long_u)len, 4);		// <sectionlen>
	}
    }

    // SN_MAP: <mapstr>
    // This is for making suggestions, section is not required.
    ಯದಿ (spin->si_map.ga_len > 0)
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
    ಯದಿ (!spin->si_nosugfile
	    && (spin->si_sal.ga_len > 0
		     || (spin->si_sofofr != ರಿಕ್ತ && spin->si_sofoto != ರಿಕ್ತ)))
    {
	putc(SN_SUGFILE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)8, 4);			// <sectionlen>

	// Set si_sugtime and write it to the file.
	spin->si_sugtime = time(ರಿಕ್ತ);
	put_time(fd, spin->si_sugtime);			// <timestamp>
    }

    // SN_NOSPLITSUGS: nothing
    // This is used to notify that no suggestions with word splits are to be
    // made.
    ಯದಿ (spin->si_nosplitsugs)
    {
	putc(SN_NOSPLITSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_NOCOMPUNDSUGS: nothing
    // This is used to notify that no suggestions with compounds are to be
    // made.
    ಯದಿ (spin->si_nocompoundsugs)
    {
	putc(SN_NOCOMPOUNDSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_COMPOUND: compound info.
    // We don't mark it required, when not supported all compound words will
    // be bad words.
    ಯದಿ (spin->si_compflags != ರಿಕ್ತ)
    {
	putc(SN_COMPOUND, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(spin->si_compflags);
	ಪ್ರತಿ (i = 0; i < spin->si_comppat.ga_len; ++i)
	    l += (ಪೂರ್ಣಾಙ್ಕ)STRLEN(((char_u **)(spin->si_comppat.ga_data))[i]) + 1;
	put_bytes(fd, (long_u)(l + 7), 4);		// <sectionlen>

	putc(spin->si_compmax, fd);			// <compmax>
	putc(spin->si_compminlen, fd);			// <compminlen>
	putc(spin->si_compsylmax, fd);			// <compsylmax>
	putc(0, fd);		// for Vim 7.0b compatibility
	putc(spin->si_compoptions, fd);			// <compoptions>
	put_bytes(fd, (long_u)spin->si_comppat.ga_len, 2);
							// <comppatcount>
	ಪ್ರತಿ (i = 0; i < spin->si_comppat.ga_len; ++i)
	{
	    p = ((char_u **)(spin->si_comppat.ga_data))[i];
	    putc((ಪೂರ್ಣಾಙ್ಕ)STRLEN(p), fd);			// <comppatlen>
	    fwv &= fwrite(p, (size_t)STRLEN(p), (size_t)1, fd);
							// <comppattext>
	}
							// <compflags>
	fwv &= fwrite(spin->si_compflags, (size_t)STRLEN(spin->si_compflags),
							       (size_t)1, fd);
    }

    // SN_NOBREAK: NOBREAK flag
    ಯದಿ (spin->si_nobreak)
    {
	putc(SN_NOBREAK, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// It's empty, the presence of the section flags the feature.
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_SYLLABLE: syllable info.
    // We don't mark it required, when not supported syllables will not be
    // counted.
    ಯದಿ (spin->si_syllable != ರಿಕ್ತ)
    {
	putc(SN_SYLLABLE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(spin->si_syllable);
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
    ಪ್ರತಿ (round = 1; round <= 3; ++round)
    {
	ಯದಿ (round == 1)
	    ವೃಕ್ಷ = spin->si_foldroot->wn_sibling;
	ಅನ್ಯಥಾ ಯದಿ (round == 2)
	    ವೃಕ್ಷ = spin->si_keeproot->wn_sibling;
	ಅನ್ಯಥಾ
	    ವೃಕ್ಷ = spin->si_prefroot->wn_sibling;

	// Clear the index and wnode fields in the tree.
	clear_ಗಂಟು(ವೃಕ್ಷ);

	// Count the number of nodes.  Needed to be able to allocate the
	// memory when reading the nodes.  Also fills in index for shared
	// nodes.
	nodecount = put_ಗಂಟು(ರಿಕ್ತ, ವೃಕ್ಷ, 0, regionmask, round == 3);

	// number of nodes in 4 bytes
	put_bytes(fd, (long_u)nodecount, 4);	// <nodecount>
	spin->si_memtot += nodecount + nodecount * ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ);

	// Write the nodes.
	(ಶೂನ್ಯ)put_ಗಂಟು(fd, ವೃಕ್ಷ, 0, regionmask, round == 3);
    }

    // Write another byte to check for errors (file system full).
    ಯದಿ (putc(0, fd) == EOF)
	retval = FAIL;
theend:
    ಯದಿ (fclose(fd) == EOF)
	retval = FAIL;

    ಯದಿ (fwv != (size_t)1)
	retval = FAIL;
    ಯದಿ (retval == FAIL)
	emsg(_(e_error_while_writing));

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Clear the index and wnode fields of "node", it siblings and its
 * children.  This is needed because they are a union with other items to save
 * space.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_ಗಂಟು(wordnode_T *ಗಂಟು)
{
    wordnode_T	*np;

    ಯದಿ (ಗಂಟು != ರಿಕ್ತ)
	FOR_ALL_ಗಂಟು_SIBLINGS(ಗಂಟು, np)
	{
	    np->wn_u1.index = 0;
	    np->wn_u2.wnode = ರಿಕ್ತ;

	    ಯದಿ (np->wn_byte != NUL)
		clear_ಗಂಟು(np->wn_child);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
put_ಗಂಟು(
    FILE	*fd,		// NULL when only counting
    wordnode_T	*ಗಂಟು,
    ಪೂರ್ಣಾಙ್ಕ		idx,
    ಪೂರ್ಣಾಙ್ಕ		regionmask,
    ಪೂರ್ಣಾಙ್ಕ		prefixtree)	// TRUE for PREFIXTREE
{
    ಪೂರ್ಣಾಙ್ಕ		newindex = idx;
    ಪೂರ್ಣಾಙ್ಕ		siblingcount = 0;
    wordnode_T	*np;
    ಪೂರ್ಣಾಙ್ಕ		flags;

    // If "node" is zero the tree is empty.
    ಯದಿ (ಗಂಟು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    // Store the index where this node is written.
    ಗಂಟು->wn_u1.index = idx;

    // Count the number of siblings.
    FOR_ALL_ಗಂಟು_SIBLINGS(ಗಂಟು, np)
	++siblingcount;

    // Write the sibling count.
    ಯದಿ (fd != ರಿಕ್ತ)
	putc(siblingcount, fd);				// <siblingcount>

    // Write each sibling byte and optionally extra info.
    FOR_ALL_ಗಂಟು_SIBLINGS(ಗಂಟು, np)
    {
	ಯದಿ (np->wn_byte == 0)
	{
	    ಯದಿ (fd != ರಿಕ್ತ)
	    {
		// For a NUL byte (end of word) write the flags etc.
		ಯದಿ (prefixtree)
		{
		    // In PREFIXTREE write the required affixID and the
		    // associated condition nr (stored in wn_region).  The
		    // byte value is misused to store the "rare" and "not
		    // combining" flags
		    ಯದಿ (np->wn_flags == (short_u)PFX_FLAGS)
			putc(BY_NOFLAGS, fd);		// <byte>
		    ಅನ್ಯಥಾ
		    {
			putc(BY_FLAGS, fd);		// <byte>
			putc(np->wn_flags, fd);		// <pflags>
		    }
		    putc(np->wn_affixID, fd);		// <affixID>
		    put_bytes(fd, (long_u)np->wn_region, 2); // <prefcondnr>
		}
		ಅನ್ಯಥಾ
		{
		    // For word trees we write the flag/region items.
		    flags = np->wn_flags;
		    ಯದಿ (regionmask != 0 && np->wn_region != regionmask)
			flags |= WF_REGION;
		    ಯದಿ (np->wn_affixID != 0)
			flags |= WF_AFX;
		    ಯದಿ (flags == 0)
		    {
			// word without flags or region
			putc(BY_NOFLAGS, fd);			// <byte>
		    }
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (np->wn_flags >= 0x100)
			{
			    putc(BY_FLAGS2, fd);		// <byte>
			    putc(flags, fd);			// <flags>
			    putc((unsigned)flags >> 8, fd);	// <flags2>
			}
			ಅನ್ಯಥಾ
			{
			    putc(BY_FLAGS, fd);			// <byte>
			    putc(flags, fd);			// <flags>
			}
			ಯದಿ (flags & WF_REGION)
			    putc(np->wn_region, fd);		// <region>
			ಯದಿ (flags & WF_AFX)
			    putc(np->wn_affixID, fd);		// <affixID>
		    }
		}
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (np->wn_child->wn_u1.index != 0
					 && np->wn_child->wn_u2.wnode != ಗಂಟು)
	    {
		// The child is written elsewhere, write the reference.
		ಯದಿ (fd != ರಿಕ್ತ)
		{
		    putc(BY_INDEX, fd);			// <byte>
							// <nodeidx>
		    put_bytes(fd, (long_u)np->wn_child->wn_u1.index, 3);
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (np->wn_child->wn_u2.wnode == ರಿಕ್ತ)
		// We will write the child below and give it an index.
		np->wn_child->wn_u2.wnode = ಗಂಟು;

	    ಯದಿ (fd != ರಿಕ್ತ)
		ಯದಿ (putc(np->wn_byte, fd) == EOF) // <byte> or <xbyte>
		{
		    emsg(_(e_error_while_writing));
		    ಪ್ರತ್ಯಾಗಮ 0;
		}
	}
    }

    // Space used in the array when reading: one for each sibling and one for
    // the count.
    newindex += siblingcount + 1;

    // Recursively dump the children of each sibling.
    FOR_ALL_ಗಂಟು_SIBLINGS(ಗಂಟು, np)
	ಯದಿ (np->wn_byte != 0 && np->wn_child->wn_u2.wnode == ಗಂಟು)
	    newindex = put_ಗಂಟು(fd, np->wn_child, newindex, regionmask,
								  prefixtree);

    ಪ್ರತ್ಯಾಗಮ newindex;
}


/*
 * ":mkspell [-ascii] outfile  infile ..."
 * ":mkspell [-ascii] addfile"
 */
    ಶೂನ್ಯ
ex_mkspell(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		fcount;
    char_u	**fnames;
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಙ್ಕ		ascii = FALSE;

    ಯದಿ (STRNCMP(arg, "-ascii", 6) == 0)
    {
	ascii = TRUE;
	arg = skipwhite(arg + 6);
    }

    // Expand all the remaining arguments (e.g., $VIMRUNTIME).
    ಯದಿ (get_arglist_exp(arg, &fcount, &fnames, FALSE) != OK)
	ಪ್ರತ್ಯಾಗಮ;

    mkspell(fcount, fnames, ascii, eap->forceit, FALSE);
    FreeWild(fcount, fnames);
}

/*
 * Create the .sug file.
 * Uses the soundfold info in "spin".
 * Writes the file with the name "wfname", with ".spl" changed to ".sug".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_make_sugfile(spellinfo_T *spin, char_u *wfname)
{
    char_u	*fname = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		len;
    slang_T	*slang;
    ಪೂರ್ಣಾಙ್ಕ		free_slang = FALSE;

    /*
     * Read back the .spl file that was written.  This fills the required
     * info for soundfolding.  This also uses less memory than the
     * pointer-linked version of the trie.  And it avoids having two versions
     * of the code for the soundfolding stuff.
     * It might have been done already by spell_reload_one().
     */
    FOR_ALL_SPELL_LANGS(slang)
	ಯದಿ (fullpathcmp(wfname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	    ಭಙ್ಗ;
    ಯದಿ (slang == ರಿಕ್ತ)
    {
	spell_message(spin, (char_u *)_("Reading back spell file..."));
	slang = spell_load_file(wfname, ರಿಕ್ತ, ರಿಕ್ತ, FALSE);
	ಯದಿ (slang == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	free_slang = TRUE;
    }

    /*
     * Clear the info in "spin" that is used.
     */
    spin->si_blocks = ರಿಕ್ತ;
    spin->si_blocks_cnt = 0;
    spin->si_compress_cnt = 0;	    // will stay at 0 all the time
    spin->si_free_ಎಣಿಕೆ = 0;
    spin->si_first_free = ರಿಕ್ತ;
    spin->si_foldwcount = 0;

    /*
     * Go through the trie of good words, soundfold each word and add it to
     * the soundfold trie.
     */
    spell_message(spin, (char_u *)_("Performing soundfolding..."));
    ಯದಿ (sug_filltree(spin, slang) == FAIL)
	ಗಚ್ಛ theend;

    /*
     * Create the table which links each soundfold word with a list of the
     * good words it may come from.  Creates buffer "spin->si_spellbuf".
     * This also removes the wordnr from the NUL byte entries to make
     * compression possible.
     */
    ಯದಿ (sug_maketable(spin) == FAIL)
	ಗಚ್ಛ theend;

    smsg(_("Number of words after soundfolding: %ld"),
				 (long)spin->si_spellbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);

    /*
     * Compress the soundfold trie.
     */
    spell_message(spin, (char_u *)_(ಸಂದೇಶ_compressing));
    wordtree_compress(spin, spin->si_foldroot, "case-folded");

    /*
     * Write the .sug file.
     * Make the file name by changing ".spl" to ".sug".
     */
    fname = alloc(MAXPATHL);
    ಯದಿ (fname == ರಿಕ್ತ)
	ಗಚ್ಛ theend;
    vim_strncpy(fname, wfname, MAXPATHL - 1);
    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(fname);
    fname[len - 2] = 'u';
    fname[len - 1] = 'g';
    sug_write(spin, fname);

theend:
    vim_free(fname);
    ಯದಿ (free_slang)
	slang_free(slang);
    free_blocks(spin->si_blocks);
    close_spellbuf(spin->si_spellbuf);
}

/*
 * Build the soundfold trie for language "slang".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sug_filltree(spellinfo_T *spin, slang_T *slang)
{
    char_u	*byts;
    idx_T	*idxs;
    ಪೂರ್ಣಾಙ್ಕ		depth;
    idx_T	arridx[MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		curi[MAXWLEN];
    char_u	tword[MAXWLEN];
    char_u	tsalword[MAXWLEN];
    ಪೂರ್ಣಾಙ್ಕ		c;
    idx_T	n;
    unsigned	ಪದಗಳು_done = 0;
    ಪೂರ್ಣಾಙ್ಕ		wordcount[MAXWLEN];

    // We use si_foldroot for the soundfolded trie.
    spin->si_foldroot = wordtree_alloc(spin);
    ಯದಿ (spin->si_foldroot == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // let tree_add_word() know we're adding to the soundfolded tree
    spin->si_sugtree = TRUE;

    /*
     * Go through the whole case-folded tree, soundfold each word and put it
     * in the trie.  Bail out if the tree is empty.
     */
    byts = slang->sl_fbyts;
    idxs = slang->sl_fidxs;
    ಯದಿ (byts == ರಿಕ್ತ || idxs == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;

    depth = 0;
    ಯಾವತ್ (depth >= 0 && !got_int)
    {
	ಯದಿ (curi[depth] > byts[arridx[depth]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[depth]] = wordcount[depth];
	    ಯದಿ (depth > 0)
		wordcount[depth - 1] += wordcount[depth];

	    --depth;
	    ಸಾಲು_breakcheck();
	}
	ಅನ್ಯಥಾ
	{

	    // Do one more byte at this node.
	    n = arridx[depth] + curi[depth];
	    ++curi[depth];

	    c = byts[n];
	    ಯದಿ (c == 0)
	    {
		// Sound-fold the word.
		tword[depth] = NUL;
		spell_soundfold(slang, tword, TRUE, tsalword);

		// We use the "flags" field for the MSB of the wordnr,
		// "region" for the LSB of the wordnr.
		ಯದಿ (ವೃಕ್ಷ_add_word(spin, tsalword, spin->si_foldroot,
				ಪದಗಳು_done >> 16, ಪದಗಳು_done & 0xffff,
							   0) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;

		++ಪದಗಳು_done;
		++wordcount[depth];

		// Reset the block count each time to avoid compression
		// kicking in.
		spin->si_blocks_cnt = 0;

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		ಯಾವತ್ (n + 1 < slang->sl_fbyts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[depth];
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (depth < MAXWLEN - 1)
	    {
		// Normal char, go one level deeper.
		tword[depth++] = c;
		arridx[depth] = idxs[n];
		curi[depth] = 1;
		wordcount[depth] = 0;
	    }
	}
    }

    smsg(_("Total number of words: %d"), ಪದಗಳು_done);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Make the table that links each word in the soundfold trie to the words it
 * can be produced from.
 * This is not unlike lines in a file, thus use a memfile to be able to access
 * the table efficiently.
 * Returns FAIL when out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sug_maketable(spellinfo_T *spin)
{
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		res = OK;

    // Allocate a buffer, open a memline for it and create the swap file
    // (uses a temp file, not a .swp file).
    spin->si_spellbuf = open_spellbuf();
    ಯದಿ (spin->si_spellbuf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Use a buffer to store the line info, avoids allocating many small
    // pieces of memory.
    ga_init2(&ga, 1, 100);

    // recursively go through the tree
    ಯದಿ (sug_filltable(spin, spin->si_foldroot->wn_sibling, 0, &ga) == -1)
	res = FAIL;

    ga_clear(&ga);
    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Fill the table for one node and its children.
 * Returns the wordnr at the start of the node.
 * Returns -1 when out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sug_filltable(
    spellinfo_T	*spin,
    wordnode_T	*ಗಂಟು,
    ಪೂರ್ಣಾಙ್ಕ		startwordnr,
    garray_T	*gap)	    // place to store line of numbers
{
    wordnode_T	*p, *np;
    ಪೂರ್ಣಾಙ್ಕ		wordnr = startwordnr;
    ಪೂರ್ಣಾಙ್ಕ		nr;
    ಪೂರ್ಣಾಙ್ಕ		prev_nr;

    FOR_ALL_ಗಂಟು_SIBLINGS(ಗಂಟು, p)
    {
	ಯದಿ (p->wn_byte == NUL)
	{
	    gap->ga_len = 0;
	    prev_nr = 0;
	    ಪ್ರತಿ (np = p; np != ರಿಕ್ತ && np->wn_byte == NUL; np = np->wn_sibling)
	    {
		ಯದಿ (ga_grow(gap, 10) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ -1;

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

	    ಯದಿ (ml_append_buf(spin->si_spellbuf, (linenr_T)wordnr,
				     gap->ga_data, gap->ga_len, TRUE) == FAIL)
		ಪ್ರತ್ಯಾಗಮ -1;
	    ++wordnr;

	    // Remove extra NUL entries, we no longer need them. We don't
	    // bother freeing the nodes, they won't be reused anyway.
	    ಯಾವತ್ (p->wn_sibling != ರಿಕ್ತ && p->wn_sibling->wn_byte == NUL)
		p->wn_sibling = p->wn_sibling->wn_sibling;

	    // Clear the flags on the remaining NUL node, so that compression
	    // works a lot better.
	    p->wn_flags = 0;
	    p->wn_region = 0;
	}
	ಅನ್ಯಥಾ
	{
	    wordnr = sug_filltable(spin, p->wn_child, wordnr, gap);
	    ಯದಿ (wordnr == -1)
		ಪ್ರತ್ಯಾಗಮ -1;
	}
    }
    ಪ್ರತ್ಯಾಗಮ wordnr;
}

/*
 * Convert an offset into a minimal number of bytes.
 * Similar to utf_char2byters, but use 8 bits in followup bytes and avoid NUL
 * bytes.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
offset2bytes(ಪೂರ್ಣಾಙ್ಕ nr, char_u *buf)
{
    ಪೂರ್ಣಾಙ್ಕ	    rem;
    ಪೂರ್ಣಾಙ್ಕ	    b1, b2, b3, b4;

    // Split the number in parts of base 255.  We need to avoid NUL bytes.
    b1 = nr % 255 + 1;
    rem = nr / 255;
    b2 = rem % 255 + 1;
    rem = rem / 255;
    b3 = rem % 255 + 1;
    b4 = rem / 255 + 1;

    ಯದಿ (b4 > 1 || b3 > 0x1f)	// 4 bytes
    {
	buf[0] = 0xe0 + b4;
	buf[1] = b3;
	buf[2] = b2;
	buf[3] = b1;
	ಪ್ರತ್ಯಾಗಮ 4;
    }
    ಯದಿ (b3 > 1 || b2 > 0x3f )	// 3 bytes
    {
	buf[0] = 0xc0 + b3;
	buf[1] = b2;
	buf[2] = b1;
	ಪ್ರತ್ಯಾಗಮ 3;
    }
    ಯದಿ (b2 > 1 || b1 > 0x7f )	// 2 bytes
    {
	buf[0] = 0x80 + b2;
	buf[1] = b1;
	ಪ್ರತ್ಯಾಗಮ 2;
    }
				// 1 byte
    buf[0] = b1;
    ಪ್ರತ್ಯಾಗಮ 1;
}

/*
 * Write the .sug file in "fname".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sug_write(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    wordnode_T	*ವೃಕ್ಷ;
    ಪೂರ್ಣಾಙ್ಕ		nodecount;
    ಪೂರ್ಣಾಙ್ಕ		wcount;
    char_u	*ಸಾಲು;
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		len;

    // Create the file.  Note that an existing file is silently overwritten!
    fd = mch_fopen((ವರ್ಣ *)fname, "w");
    ಯದಿ (fd == ರಿಕ್ತ)
    {
	semsg(_(e_cant_open_file_str), fname);
	ಪ್ರತ್ಯಾಗಮ;
    }

    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE,
				  _("Writing suggestion file %s..."), fname);
    spell_message(spin, IObuff);

    /*
     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
     */
    ಯದಿ (fwrite(VIMSUGMAGIC, VIMSUGMAGICL, (size_t)1, fd) != 1) // <fileID>
    {
	emsg(_(e_error_while_writing));
	ಗಚ್ಛ theend;
    }
    putc(VIMSUGVERSION, fd);				// <versionnr>

    // Write si_sugtime to the file.
    put_time(fd, spin->si_sugtime);			// <timestamp>

    /*
     * <SUGWORDTREE>
     */
    spin->si_memtot = 0;
    ವೃಕ್ಷ = spin->si_foldroot->wn_sibling;

    // Clear the index and wnode fields in the tree.
    clear_ಗಂಟು(ವೃಕ್ಷ);

    // Count the number of nodes.  Needed to be able to allocate the
    // memory when reading the nodes.  Also fills in index for shared
    // nodes.
    nodecount = put_ಗಂಟು(ರಿಕ್ತ, ವೃಕ್ಷ, 0, 0, FALSE);

    // number of nodes in 4 bytes
    put_bytes(fd, (long_u)nodecount, 4);	// <nodecount>
    spin->si_memtot += nodecount + nodecount * ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ);

    // Write the nodes.
    (ಶೂನ್ಯ)put_ಗಂಟು(fd, ವೃಕ್ಷ, 0, 0, FALSE);

    /*
     * <SUGTABLE>: <sugwcount> <sugline> ...
     */
    wcount = spin->si_spellbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    put_bytes(fd, (long_u)wcount, 4);	// <sugwcount>

    ಪ್ರತಿ (lnum = 1; lnum <= (linenr_T)wcount; ++lnum)
    {
	// <sugline>: <sugnr> ... NUL
	ಸಾಲು = ml_get_buf(spin->si_spellbuf, lnum, FALSE);
	len = ml_get_buf_len(spin->si_spellbuf, lnum) + 1;
	ಯದಿ (fwrite(ಸಾಲು, (size_t)len, (size_t)1, fd) == 0)
	{
	    emsg(_(e_error_while_writing));
	    ಗಚ್ಛ theend;
	}
	spin->si_memtot += len;
    }

    // Write another byte to check for errors.
    ಯದಿ (putc(0, fd) == EOF)
	emsg(_(e_error_while_writing));

    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE,
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
    ಶೂನ್ಯ
mkspell(
    ಪೂರ್ಣಾಙ್ಕ		fcount,
    char_u	**fnames,
    ಪೂರ್ಣಾಙ್ಕ		ascii,		    // -ascii argument given
    ಪೂರ್ಣಾಙ್ಕ		over_write,	    // overwrite existing output file
    ಪೂರ್ಣಾಙ್ಕ		added_word)	    // invoked through "zg"
{
    char_u	*fname = ರಿಕ್ತ;
    char_u	*wfname;
    char_u	**innames;
    ಪೂರ್ಣಾಙ್ಕ		incount;
    afffile_T	*(afile[MAXREGIONS]);
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		len;
    stat_T	st;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    spellinfo_T spin;

    CLEAR_FIELD(spin);
    spin.si_verbose = !added_word;
    spin.si_ascii = ascii;
    spin.si_followup = TRUE;
    spin.si_rem_accents = TRUE;
    ga_init2(&spin.si_rep, ಪರಿಮಾಣ(fromto_T), 20);
    ga_init2(&spin.si_repsal, ಪರಿಮಾಣ(fromto_T), 20);
    ga_init2(&spin.si_sal, ಪರಿಮಾಣ(fromto_T), 20);
    ga_init2(&spin.si_map, ಪರಿಮಾಣ(char_u), 100);
    ga_init2(&spin.si_comppat, ಪರಿಮಾಣ(char_u *), 20);
    ga_init2(&spin.si_prefcond, ಪರಿಮಾಣ(char_u *), 50);
    hash_init(&spin.si_commonwords);
    spin.si_newcompID = 127;	// start compound ID at first maximum

    // default: fnames[0] is output file, following are input files
    // When "fcount" is 1 there is only one file.
    innames = &fnames[fcount == 1 ? 0 : 1];
    incount = fcount - 1;

    wfname = alloc(MAXPATHL);
    ಯದಿ (wfname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (fcount >= 1)
    {
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(fnames[0]);
	ಯದಿ (fcount == 1 && len > 4 && STRCMP(fnames[0] + len - 4, ".add") == 0)
	{
	    // For ":mkspell path/en.latin1.add" output file is
	    // "path/en.latin1.add.spl".
	    incount = 1;
	    vim_snprintf((ವರ್ಣ *)wfname, MAXPATHL, "%s.spl", fnames[0]);
	}
	ಅನ್ಯಥಾ ಯದಿ (fcount == 1)
	{
	    // For ":mkspell path/vim" output file is "path/vim.latin1.spl".
	    incount = 1;
	    vim_snprintf((ವರ್ಣ *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());
	}
	ಅನ್ಯಥಾ ಯದಿ (len > 4 && STRCMP(fnames[0] + len - 4, ".spl") == 0)
	{
	    // Name ends in ".spl", use as the file name.
	    vim_strncpy(wfname, fnames[0], MAXPATHL - 1);
	}
	ಅನ್ಯಥಾ
	    // Name should be language, make the file name from it.
	    vim_snprintf((ವರ್ಣ *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());

	// Check for .ascii.spl.
	ಯದಿ (strstr((ವರ್ಣ *)gettail(wfname), SPL_FNAME_ASCII) != ರಿಕ್ತ)
	    spin.si_ascii = TRUE;

	// Check for .add.spl.
	ಯದಿ (strstr((ವರ್ಣ *)gettail(wfname), SPL_FNAME_ADD) != ರಿಕ್ತ)
	    spin.si_add = TRUE;
    }

    ಯದಿ (incount <= 0)
	emsg(_(e_invalid_argument));	// need at least output and input names
    ಅನ್ಯಥಾ ಯದಿ (vim_strchr(gettail(wfname), '_') != ರಿಕ್ತ)
	emsg(_(e_output_file_name_must_not_have_region_name));
    ಅನ್ಯಥಾ ಯದಿ (incount > MAXREGIONS)
	semsg(_(e_only_up_to_nr_regions_supported), MAXREGIONS);
    ಅನ್ಯಥಾ
    {
	// Check for overwriting before doing things that may take a lot of
	// time.
	ಯದಿ (!over_write && mch_stat((ವರ್ಣ *)wfname, &st) >= 0)
	{
	    emsg(_(e_file_exists));
	    ಗಚ್ಛ theend;
	}
	ಯದಿ (mch_isdir(wfname))
	{
	    semsg(_(e_str_is_directory), wfname);
	    ಗಚ್ಛ theend;
	}

	fname = alloc(MAXPATHL);
	ಯದಿ (fname == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;

	/*
	 * Init the aff and dic pointers.
	 * Get the region names if there are more than 2 arguments.
	 */
	ಪ್ರತಿ (i = 0; i < incount; ++i)
	{
	    afile[i] = ರಿಕ್ತ;

	    ಯದಿ (incount > 1)
	    {
		len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(innames[i]);
		ಯದಿ (STRLEN(gettail(innames[i])) < 5
						|| innames[i][len - 3] != '_')
		{
		    semsg(_(e_invalid_region_in_str), innames[i]);
		    ಗಚ್ಛ theend;
		}
		spin.si_region_name[i * 2] = TOLOWER_ASC(innames[i][len - 2]);
		spin.si_region_name[i * 2 + 1] =
					     TOLOWER_ASC(innames[i][len - 1]);
	    }
	}
	spin.si_region_ಎಣಿಕೆ = incount;

	spin.si_foldroot = wordtree_alloc(&spin);
	spin.si_keeproot = wordtree_alloc(&spin);
	spin.si_prefroot = wordtree_alloc(&spin);
	ಯದಿ (spin.si_foldroot == ರಿಕ್ತ
		|| spin.si_keeproot == ರಿಕ್ತ
		|| spin.si_prefroot == ರಿಕ್ತ)
	{
	    free_blocks(spin.si_blocks);
	    ಗಚ್ಛ theend;
	}

	// When not producing a .add.spl file clear the character table when
	// we encounter one in the .aff file.  This means we dump the current
	// one in the .spl file if the .aff file doesn't define one.  That's
	// better than guessing the contents, the table will match a
	// previously loaded spell file.
	ಯದಿ (!spin.si_add)
	    spin.si_clear_chartab = TRUE;

	/*
	 * Read all the .aff and .dic files.
	 * Text is converted to 'encoding'.
	 * Words are stored in the case-folded and keep-case trees.
	 */
	ಪ್ರತಿ (i = 0; i < incount && !error; ++i)
	{
	    spin.si_conv.vc_type = CONV_NONE;
	    spin.si_region = 1 << i;

	    vim_snprintf((ವರ್ಣ *)fname, MAXPATHL, "%s.aff", innames[i]);
	    ಯದಿ (mch_stat((ವರ್ಣ *)fname, &st) >= 0)
	    {
		// Read the .aff file.  Will init "spin->si_conv" based on the
		// "SET" line.
		afile[i] = spell_read_aff(&spin, fname);
		ಯದಿ (afile[i] == ರಿಕ್ತ)
		    error = TRUE;
		ಅನ್ಯಥಾ
		{
		    // Read the .dic file and store the words in the trees.
		    vim_snprintf((ವರ್ಣ *)fname, MAXPATHL, "%s.dic",
								  innames[i]);
		    ಯದಿ (spell_read_dic(&spin, fname, afile[i]) == FAIL)
			error = TRUE;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		// No .aff file, try reading the file as a word list.  Store
		// the words in the trees.
		ಯದಿ (spell_read_wordfile(&spin, innames[i]) == FAIL)
		    error = TRUE;
	    }

	    // Free any conversion stuff.
	    convert_setup(&spin.si_conv, ರಿಕ್ತ, ರಿಕ್ತ);
	}

	ಯದಿ (spin.si_compflags != ರಿಕ್ತ && spin.si_nobreak)
	    ಸಂದೇಶ(_("Warning: both compounding and NOBREAK specified"));

	ಯದಿ (!error && !got_int)
	{
	    /*
	     * Combine tails in the tree.
	     */
	    spell_message(&spin, (char_u *)_(ಸಂದೇಶ_compressing));
	    wordtree_compress(&spin, spin.si_foldroot, "case-folded");
	    wordtree_compress(&spin, spin.si_keeproot, "keep-case");
	    wordtree_compress(&spin, spin.si_prefroot, "prefixes");
	}

	ಯದಿ (!error && !got_int)
	{
	    /*
	     * Write the info in the spell file.
	     */
	    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE,
				      _("Writing spell file %s..."), wfname);
	    spell_message(&spin, IObuff);

	    error = write_vim_spell(&spin, wfname) == FAIL;

	    spell_message(&spin, (char_u *)_("Done!"));
	    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin.si_memtot);
	    spell_message(&spin, IObuff);

	    /*
	     * If the file is loaded need to reload it.
	     */
	    ಯದಿ (!error)
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
	ಪ್ರತಿ (i = 0; i < incount; ++i)
	    ಯದಿ (afile[i] != ರಿಕ್ತ)
		spell_free_aff(afile[i]);

	// Free all the bits and pieces at once.
	free_blocks(spin.si_blocks);

	/*
	 * If there is soundfolding info and no NOSUGFILE item create the
	 * .sug file with the soundfolded word trie.
	 */
	ಯದಿ (spin.si_sugtime != 0 && !error && !got_int)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
spell_message(spellinfo_T *spin, char_u *str)
{
    ಯದಿ (spin->si_verbose || p_verbose > 2)
    {
	ಯದಿ (!spin->si_verbose)
	    verbose_enter();
	ಸಂದೇಶ((ವರ್ಣ *)str);
	out_flush();
	ಯದಿ (!spin->si_verbose)
	    verbose_leave();
    }
}

/*
 * ":[count]spellgood  {word}"
 * ":[count]spellwrong {word}"
 * ":[count]spellundo  {word}"
 * ":[count]spellrare  {word}"
 */
    ಶೂನ್ಯ
ex_spell(exarg_T *eap)
{
    spell_add_word(eap->arg, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(eap->arg),
		eap->cmdidx == CMD_spellwrong ? SPELL_ADD_BAD :
		eap->cmdidx == CMD_spellrare ? SPELL_ADD_RARE : SPELL_ADD_GOOD,
				   eap->forceit ? 0 : (ಪೂರ್ಣಾಙ್ಕ)eap->line2,
				   eap->cmdidx == CMD_spellundo);
}

/*
 * Add "word[len]" to 'spellfile' as a good, rare or bad word.
 */
    ಶೂನ್ಯ
spell_add_word(
    char_u	*word,
    ಪೂರ್ಣಾಙ್ಕ		len,
    ಪೂರ್ಣಾಙ್ಕ		what,	    // SPELL_ADD_ values
    ಪೂರ್ಣಾಙ್ಕ		idx,	    // "zG" and "zW": zero, otherwise index in
			    // 'spellfile'
    ಪೂರ್ಣಾಙ್ಕ		undo)	    // TRUE for "zug", "zuG", "zuw" and "zuW"
{
    FILE	*fd = ರಿಕ್ತ;
    buf_T	*buf = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		new_spf = FALSE;
    char_u	*fname;
    char_u	*fnamebuf = ರಿಕ್ತ;
    char_u	ಸಾಲು[MAXWLEN * 2];
    long	fpos, fpos_next = 0;
    ಪೂರ್ಣಾಙ್ಕ		i;
    size_t	linelen;
    char_u	*spf;

    ಯದಿ (!valid_spell_word(word, word + len))
    {
	emsg(_(e_illegal_character_in_word));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (idx == 0)	    // use internal wordlist
    {
	ಯದಿ (int_wordlist == ರಿಕ್ತ)
	{
	    int_wordlist = vim_tempname('s', FALSE);
	    ಯದಿ (int_wordlist == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	}
	fname = int_wordlist;
    }
    ಅನ್ಯಥಾ
    {
	// If 'spellfile' isn't set figure out a good default value.
	ಯದಿ (*curwin->w_s->b_p_spf == NUL)
	{
	    init_spellfile();
	    new_spf = TRUE;
	}

	ಯದಿ (*curwin->w_s->b_p_spf == NUL)
	{
	    semsg(_(e_option_str_is_not_set), "spellfile");
	    ಪ್ರತ್ಯಾಗಮ;
	}
	fnamebuf = alloc(MAXPATHL);
	ಯದಿ (fnamebuf == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

	ಪ್ರತಿ (spf = curwin->w_s->b_p_spf, i = 1; *spf != NUL; ++i)
	{
	    copy_option_part(&spf, fnamebuf, MAXPATHL, ",");
	    ಯದಿ (i == idx)
		ಭಙ್ಗ;
	    ಯದಿ (*spf == NUL)
	    {
		semsg(_(e_spellfile_does_not_have_nr_entries), idx);
		vim_free(fnamebuf);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}

	// Check that the user isn't editing the .add file somewhere.
	buf = buflist_findname_exp(fnamebuf);
	ಯದಿ (buf != ರಿಕ್ತ && buf->b_ml.ml_mfp == ರಿಕ್ತ)
	    buf = ರಿಕ್ತ;
	ಯದಿ (buf != ರಿಕ್ತ && bufIsChanged(buf))
	{
	    emsg(_(e_file_is_loaded_in_another_buffer));
	    vim_free(fnamebuf);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	fname = fnamebuf;
    }

    ಯದಿ (what == SPELL_ADD_BAD || undo)
    {
	// When the word appears as good word we need to remove that one,
	// since its flags sort before the one with WF_BANNED.
	fd = mch_fopen((ವರ್ಣ *)fname, "r");
	ಯದಿ (fd != ರಿಕ್ತ)
	{
	    ಯಾವತ್ (!vim_fgets(ಸಾಲು, MAXWLEN * 2, fd))
	    {
		fpos = fpos_next;
		fpos_next = ftell(fd);
		ಯದಿ (fpos_next < 0)
		    ಭಙ್ಗ;  // should never happen
		linelen = STRLEN(ಸಾಲು);
		ಯದಿ (linelen >= (size_t)len
			&& STRNCMP(word, ಸಾಲು, len) == 0
			&& (ಸಾಲು[len] == '/' || ಸಾಲು[len] < ' '))
		{
		    // Found duplicate word.  Remove it by writing a '#' at
		    // the start of the line.  Mixing reading and writing
		    // doesn't work for all systems, close the file first.
		    fclose(fd);
		    fd = mch_fopen((ವರ್ಣ *)fname, "r+");
		    ಯದಿ (fd == ರಿಕ್ತ)
			ಭಙ್ಗ;
		    ಯದಿ (fseek(fd, fpos, SEEK_SET) == 0)
		    {
			fputc('#', fd);
			ಯದಿ (undo)
			{
			    home_replace(ರಿಕ್ತ, fname, NameBuff, MAXPATHL, TRUE);
			    smsg(_("Word '%.*s' removed from %s"),
							 len, word, NameBuff);
			}
		    }
		    ಯದಿ (fseek(fd, fpos_next, SEEK_SET) != 0)
		    {
			PERROR(_("Seek error in spellfile"));
			ಭಙ್ಗ;
		    }
		}
	    }
	    ಯದಿ (fd != ರಿಕ್ತ)
		fclose(fd);
	}
    }

    ಯದಿ (!undo)
    {
	fd = mch_fopen((ವರ್ಣ *)fname, "a");
	ಯದಿ (fd == ರಿಕ್ತ && new_spf)
	{
	    char_u *p;

	    // We just initialized the 'spellfile' option and can't open the
	    // file.  We may need to create the "spell" directory first.  We
	    // already checked the runtime directory is writable in
	    // init_spellfile().
	    ಯದಿ (!dir_of_file_exists(fname) && (p = gettail_sep(fname)) != fname)
	    {
		ಪೂರ್ಣಾಙ್ಕ c = *p;

		// The directory doesn't exist.  Try creating it and opening
		// the file again.
		*p = NUL;
		vim_mkdir(fname, 0755);
		*p = c;
		fd = mch_fopen((ವರ್ಣ *)fname, "a");
	    }
	}

	ಯದಿ (fd == ರಿಕ್ತ)
	    semsg(_(e_cant_open_file_str), fname);
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (what == SPELL_ADD_BAD)
		fprintf(fd, "%.*s/!\n", len, word);
	    ಅನ್ಯಥಾ ಯದಿ (what == SPELL_ADD_RARE)
		fprintf(fd, "%.*s/?\n", len, word);
	    ಅನ್ಯಥಾ
		fprintf(fd, "%.*s\n", len, word);
	    fclose(fd);

	    home_replace(ರಿಕ್ತ, fname, NameBuff, MAXPATHL, TRUE);
	    smsg(_("Word '%.*s' added to %s"), len, word, NameBuff);
	}
    }

    ಯದಿ (fd != ರಿಕ್ತ)
    {
	// Update the .add.spl file.
	mkspell(1, &fname, FALSE, TRUE, TRUE);

	// If the .add file is edited somewhere, reload it.
	ಯದಿ (buf != ರಿಕ್ತ)
	    buf_reload(buf, buf->b_orig_mode, FALSE);

	redraw_all_later(UPD_SOME_VALID);
    }
    vim_free(fnamebuf);
}

/*
 * Initialize 'spellfile' for the current buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_spellfile(ಶೂನ್ಯ)
{
    string_T	buf;
    char_u	*fname;
    char_u	*rtp;
    char_u	*lend;
    ಪೂರ್ಣಾಙ್ಕ		aspath = FALSE;
    char_u	*lstart = curbuf->b_s.b_p_spl;

    ಯದಿ (*curwin->w_s->b_p_spl == NUL || curwin->w_s->b_langp.ga_len <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    buf.string = alloc(MAXPATHL);
    ಯದಿ (buf.string == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    buf.length = 0;

    // Find the end of the language name.  Exclude the region.  If there
    // is a path separator remember the start of the tail.
    ಪ್ರತಿ (lend = curwin->w_s->b_p_spl; *lend != NUL
	    && vim_strchr((char_u *)",._", *lend) == ರಿಕ್ತ; ++lend)
	ಯದಿ (vim_ispathsep(*lend))
	{
	    aspath = TRUE;
	    lstart = lend + 1;
	}

    // Loop over all entries in 'runtimepath'.  Use the first one where we
    // are allowed to write.
    rtp = p_rtp;
    ಯಾವತ್ (*rtp != NUL)
    {
	ಯದಿ (aspath)
	    // Use directory of an entry with path, e.g., for
	    // "/dir/lg.utf-8.spl" use "/dir".
	    vim_strncpy(buf.string, curbuf->b_s.b_p_spl,
		    lstart - curbuf->b_s.b_p_spl - 1);
	ಅನ್ಯಥಾ
	    // Copy the path from 'runtimepath' to buf[].
	    buf.length = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");
	ಯದಿ (filewritable(buf.string) == 2)
	{
	    // Use the first language name from 'spelllang' and the
	    // encoding used in the first loaded .spl file.
	    ಯದಿ (aspath)
	    {
		buf.length = (size_t)(lend - curbuf->b_s.b_p_spl);
		vim_strncpy(buf.string, curbuf->b_s.b_p_spl, buf.length);
	    }
	    ಅನ್ಯಥಾ
	    {
		// Create the "spell" directory if it doesn't exist yet.
		buf.length += vim_snprintf_safelen((ವರ್ಣ *)buf.string + buf.length,
		    MAXPATHL - buf.length, "/spell");
		ಯದಿ (filewritable(buf.string) != 2)
		{
		    ಯದಿ (vim_mkdir(buf.string, 0755) != 0)
		    {
			vim_free(buf.string);
			ಪ್ರತ್ಯಾಗಮ;
		    }
		}

		buf.length += vim_snprintf_safelen((ವರ್ಣ *)buf.string + buf.length,
		    MAXPATHL - buf.length, "/%.*s", (ಪೂರ್ಣಾಙ್ಕ)(lend - lstart), lstart);
	    }
	    fname = LANGP_ENTRY(curwin->w_s->b_langp, 0)
		->lp_slang->sl_fname;
	    vim_snprintf((ವರ್ಣ *)buf.string + buf.length, MAXPATHL - buf.length,
		".%s.add",
		fname != ರಿಕ್ತ && strstr((ವರ್ಣ *)gettail(fname), ".ascii.") != ರಿಕ್ತ
		    ? (char_u *)"ascii" : spell_enc());
	    set_option_ಮೌಲ್ಯ_give_err((char_u *)"spellfile",
		    0L, buf.string, OPT_LOCAL);
	    ಭಙ್ಗ;
	}
	aspath = FALSE;
    }

    vim_free(buf.string);
}



/*
 * Set the spell character tables from strings in the affix file.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_spell_chartab(char_u *fol, char_u *ಕೆಳಗಿನ, char_u *upp)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    char_u	*pf = fol, *pl = ಕೆಳಗಿನ, *pu = upp;
    ಪೂರ್ಣಾಙ್ಕ		f, l, u;

    clear_spell_chartab(&new_st);

    ಯಾವತ್ (*pf != NUL)
    {
	ಯದಿ (*pl == NUL || *pu == NUL)
	{
	    emsg(_(e_format_error_in_affix_file_fol_ಕೆಳಗಿನ_or_upp));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	f = mb_ptr2char_adv(&pf);
	l = mb_ptr2char_adv(&pl);
	u = mb_ptr2char_adv(&pu);

	// Every character that appears is a word character.
	ಯದಿ (f < 256)
	    new_st.st_isw[f] = TRUE;
	ಯದಿ (l < 256)
	    new_st.st_isw[l] = TRUE;
	ಯದಿ (u < 256)
	    new_st.st_isw[u] = TRUE;

	// if "LOW" and "FOL" are not the same the "LOW" char needs
	// case-folding
	ಯದಿ (l < 256 && l != f)
	{
	    ಯದಿ (f >= 256)
	    {
		emsg(_(e_character_in_fol_ಕೆಳಗಿನ_or_upp_is_out_of_range));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    new_st.st_fold[l] = f;
	}

	// if "UPP" and "FOL" are not the same the "UPP" char needs
	// case-folding, it's upper case and the "UPP" is the upper case of
	// "FOL" .
	ಯದಿ (u < 256 && u != f)
	{
	    ಯದಿ (f >= 256)
	    {
		emsg(_(e_character_in_fol_ಕೆಳಗಿನ_or_upp_is_out_of_range));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    new_st.st_fold[u] = f;
	    new_st.st_isu[u] = TRUE;
	    new_st.st_upper[f] = u;
	}
    }

    ಯದಿ (*pl != NUL || *pu != NUL)
    {
	emsg(_(e_format_error_in_affix_file_fol_ಕೆಳಗಿನ_or_upp));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ set_spell_finish(&new_st);
}

/*
 * Set the spell character tables from strings in the .spl file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_spell_charflags(
    char_u	*flags,
    ಪೂರ್ಣಾಙ್ಕ		cnt,	    // length of "flags"
    char_u	*fol)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*p = fol;
    ಪೂರ್ಣಾಙ್ಕ		c;

    clear_spell_chartab(&new_st);

    ಪ್ರತಿ (i = 0; i < 128; ++i)
    {
	ಯದಿ (i < cnt)
	{
	    new_st.st_isw[i + 128] = (flags[i] & CF_WORD) != 0;
	    new_st.st_isu[i + 128] = (flags[i] & CF_UPPER) != 0;
	}

	ಯದಿ (*p != NUL)
	{
	    c = mb_ptr2char_adv(&p);
	    new_st.st_fold[i + 128] = c;
	    ಯದಿ (i + 128 != c && new_st.st_isu[i + 128] && c < 256)
		new_st.st_upper[c] = i + 128;
	}
    }

    (ಶೂನ್ಯ)set_spell_finish(&new_st);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_spell_finish(spelltab_T *new_st)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (did_set_spelltab)
    {
	// check that it's the same table
	ಪ್ರತಿ (i = 0; i < 256; ++i)
	{
	    ಯದಿ (spelltab.st_isw[i] != new_st->st_isw[i]
		    || spelltab.st_isu[i] != new_st->st_isu[i]
		    || spelltab.st_fold[i] != new_st->st_fold[i]
		    || spelltab.st_upper[i] != new_st->st_upper[i])
	    {
		emsg(_(e_word_characters_differ_between_spell_files));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	// copy the new spelltab into the one being used
	spelltab = *new_st;
	did_set_spelltab = TRUE;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Write the table with prefix conditions to the .spl file.
 * When "fd" is NULL only count the length of what is written and return it.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		totlen;

    ಯದಿ (fd != ರಿಕ್ತ)
	put_bytes(fd, (long_u)gap->ga_len, 2);	    // <prefcondcnt>

    totlen = 2 + gap->ga_len; // length of <prefcondcnt> and <condlen> bytes

    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	p = ((char_u **)gap->ga_data)[i];
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);
	    ಯದಿ (fd != ರಿಕ್ತ)
	    {
		fputc(len, fd);
		*fwv &= fwrite(p, (size_t)len, (size_t)1, fd);
	    }
	    totlen += len;
	}
	ಅನ್ಯಥಾ ಯದಿ (fd != ರಿಕ್ತ)
	    fputc(0, fd);
    }

    ಪ್ರತ್ಯಾಗಮ totlen;
}

/*
 * Use map string "map" for languages "lp".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_map_str(slang_T *lp, char_u *map)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		headc = 0;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (*map == NUL)
    {
	lp->sl_has_map = FALSE;
	ಪ್ರತ್ಯಾಗಮ;
    }
    lp->sl_has_map = TRUE;

    // Init the array and hash tables empty.
    ಪ್ರತಿ (i = 0; i < 256; ++i)
	lp->sl_map_array[i] = 0;
    hash_init(&lp->sl_map_hash);

    /*
     * The similar characters are stored separated with slashes:
     * "aaa/bbb/ccc/".  Fill sl_map_array[c] with the character before c and
     * before the same slash.  For characters above 255 sl_map_hash is used.
     */
    ಪ್ರತಿ (p = map; *p != NUL; )
    {
	c = mb_cptr2char_adv(&p);
	ಯದಿ (c == '/')
	    headc = 0;
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (headc == 0)
		 headc = c;

	    // Characters above 255 don't fit in sl_map_array[], put them in
	    // the hash table.  Each entry is the char, a NUL the headchar and
	    // a NUL.
	    ಯದಿ (c >= 256)
	    {
		ಪೂರ್ಣಾಙ್ಕ	    cl = mb_char2len(c);
		ಪೂರ್ಣಾಙ್ಕ	    headcl = mb_char2len(headc);
		char_u	    *b;
		hash_T	    hash;
		hashitem_T  *hi;

		b = alloc(cl + headcl + 2);
		ಯದಿ (b == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ;
		mb_char2bytes(c, b);
		b[cl] = NUL;
		mb_char2bytes(headc, b + cl + 1);
		b[cl + 1 + headcl] = NUL;
		hash = hash_hash(b);
		hi = hash_lookup(&lp->sl_map_hash, b, hash);
		ಯದಿ (HASHITEM_EMPTY(hi))
		    hash_add_item(&lp->sl_map_hash, hi, b, hash);
		ಅನ್ಯಥಾ
		{
		    // This should have been checked when generating the .spl
		    // file.
		    emsg(_(e_duplicate_char_in_map_entry));
		    vim_free(b);
		}
	    }
	    ಅನ್ಯಥಾ
		lp->sl_map_array[c] = headc;
	}
    }
}

#endif  // FEAT_SPELL
