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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_SPELL)

#ଅନ୍ତର୍ଭାଵ <time.h>	// for time_t

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
#define SN_ଶବ୍ଦଗୁଡ଼ିକ	13	// common words
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
#define FOR_ALL_ଗଣ୍ଠି_SIBLINGS(ଗଣ୍ଠି, np) \
    ପାଇଁ ((np) = (ଗଣ୍ଠି); (np) != ଖାଲି; (np) = (np)->wn_sibling)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_spell_finish(spelltab_T	*new_st);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_region_section(FILE *fd, slang_T *slang, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_charflags_section(FILE *fd);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_prefcond_section(FILE *fd, slang_T *lp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_rep_section(FILE *fd, garray_T *gap, short *first);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_sal_section(FILE *fd, slang_T *slang);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_ଶବ୍ଦଗୁଡ଼ିକ_section(FILE *fd, slang_T *lp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_sofo_section(FILE *fd, slang_T *slang);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_compound(FILE *fd, slang_T *slang, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_sofo(slang_T *lp, char_u *from, char_u *to);
ସ୍ଥିର ଶୂନ୍ୟ set_sal_first(slang_T *lp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା *mb_str2wide(char_u *s);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା spell_read_ବୃକ୍ଷ(FILE *fd, char_u **bytsp, long *bytsp_len, idx_T **idxsp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prefixtree, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prefixcnt);
ସ୍ଥିର idx_T read_ବୃକ୍ଷ_ଗଣ୍ଠି(FILE *fd, char_u *byts, idx_T *idxs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxidx, idx_T startidx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prefixtree, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxprefcondnr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth);
ସ୍ଥିର ଶୂନ୍ୟ set_spell_charflags(char_u *flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt, char_u *upp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_spell_chartab(char_u *fol, char_u *ତଳ, char_u *upp);
ସ୍ଥିର ଶୂନ୍ୟ set_map_str(slang_T *lp, char_u *map);


ସ୍ଥିର ଅକ୍ଷର *e_afftrailing = N_("Trailing text in %s line %d: %s");
ସ୍ଥିର ଅକ୍ଷର *e_affname = N_("Affix name too long in %s line %d: %s");
ସ୍ଥିର ଅକ୍ଷର *ସନ୍ଦେଶ_compressing = N_("Compressing word tree...");

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		silent)		// no error if file doesn't exist
{
    FILE	*fd;
    char_u	buf[VIMSPELLMAGICL];
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    slang_T	*lp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_estack_ଠେଲ = FALSE;
    ESTACK_CHECK_DECLARATION;

    fd = mch_fopen((ଅକ୍ଷର *)fname, "r");
    ଯଦି (fd == ଖାଲି)
    {
	ଯଦି (!silent)
	    semsg(_(e_cant_open_file_str), fname);
	ଅନ୍ୟଥା ଯଦି (p_verbose > 2)
	{
	    verbose_enter();
	    smsg((ଅଚଳ ଅକ୍ଷର *)e_cant_open_file_str, fname);
	    verbose_leave();
	}
	ଯାଅ endFAIL;
    }
    ଯଦି (p_verbose > 2)
    {
	verbose_enter();
	smsg(_("Reading spell file \"%s\""), fname);
	verbose_leave();
    }

    ଯଦି (old_lp == ଖାଲି)
    {
	lp = slang_alloc(lang);
	ଯଦି (lp == ଖାଲି)
	    ଯାଅ endFAIL;

	// Remember the file name, used to reload the file when it's updated.
	lp->sl_fname = vim_strsave(fname);
	ଯଦି (lp->sl_fname == ଖାଲି)
	    ଯାଅ endFAIL;

	// Check for .add.spl (_add.spl for VMS).
	lp->sl_add = strstr((ଅକ୍ଷର *)gettail(fname), SPL_FNAME_ADD) != ଖାଲି;
    }
    ଅନ୍ୟଥା
	lp = old_lp;

    // Set sourcing_name, so that error messages mention the file name.
    estack_ଠେଲ(ETYPE_SPELL, fname, 0);
    ESTACK_CHECK_SETUP;
    did_estack_ଠେଲ = TRUE;

    /*
     * <HEADER>: <fileID>
     */
    ପାଇଁ (i = 0; i < VIMSPELLMAGICL; ++i)
	buf[i] = (c = getc(fd)) == EOF ? 0 : c;		// <fileID>
    ଯଦି (STRNCMP(buf, VIMSPELLMAGIC, VIMSPELLMAGICL) != 0)
    {
	emsg(_(e_this_does_not_look_like_spell_file));
	ଯାଅ endFAIL;
    }
    c = getc(fd);					// <versionnr>
    ଯଦି (c < VIMSPELLVERSION)
    {
	emsg(_(e_old_spell_file_needs_to_be_updated));
	ଯାଅ endFAIL;
    }
    ଅନ୍ୟଥା ଯଦି (c > VIMSPELLVERSION)
    {
	emsg(_(e_spell_file_is_for_newer_version_of_vim));
	ଯାଅ endFAIL;
    }


    /*
     * <SECTIONS>: <section> ... <sectionend>
     * <section>: <sectionID> <sectionflags> <sectionlen> (section contents)
     */
    ପାଇଁ (;;)
    {
	n = getc(fd);			    // <sectionID> or <sectionend>
	ଯଦି (n == SN_END)
	    ଭାଙ୍ଗ;
	c = getc(fd);					// <sectionflags>
	len = get4c(fd);				// <sectionlen>
	ଯଦି (len < 0)
	    ଯାଅ truncerr;

	res = 0;
	ବାଛ (n)
	{
	    ସ୍ଥିତି SN_INFO:
		lp->sl_info = read_string(fd, len);	// <infotext>
		ଯଦି (lp->sl_info == ଖାଲି)
		    ଯାଅ endFAIL;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_REGION:
		res = read_region_section(fd, lp, len);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_CHARFLAGS:
		res = read_charflags_section(fd);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_MIDWORD:
		lp->sl_midword = read_string(fd, len);	// <midword>
		ଯଦି (lp->sl_midword == ଖାଲି)
		    ଯାଅ endFAIL;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_PREFCOND:
		res = read_prefcond_section(fd, lp);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_REP:
		res = read_rep_section(fd, &lp->sl_rep, lp->sl_rep_first);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_REPSAL:
		res = read_rep_section(fd, &lp->sl_repsal, lp->sl_repsal_first);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_SAL:
		res = read_sal_section(fd, lp);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_SOFO:
		res = read_sofo_section(fd, lp);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_MAP:
		p = read_string(fd, len);		// <mapstr>
		ଯଦି (p == ଖାଲି)
		    ଯାଅ endFAIL;
		set_map_str(lp, p);
		vim_free(p);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_ଶବ୍ଦଗୁଡ଼ିକ:
		res = read_ଶବ୍ଦଗୁଡ଼ିକ_section(fd, lp, len);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_SUGFILE:
		lp->sl_sugtime = get8ctime(fd);		// <timestamp>
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_NOSPLITSUGS:
		lp->sl_nosplitsugs = TRUE;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_NOCOMPOUNDSUGS:
		lp->sl_nocompoundsugs = TRUE;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_COMPOUND:
		res = read_compound(fd, lp, len);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_NOBREAK:
		lp->sl_nobreak = TRUE;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SN_SYLLABLE:
		lp->sl_syllable = read_string(fd, len);	// <syllable>
		ଯଦି (lp->sl_syllable == ଖାଲି)
		    ଯାଅ endFAIL;
		ଯଦି (init_syl_tab(lp) != OK)
		    ଯାଅ endFAIL;
		ଭାଙ୍ଗ;

	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		// Unsupported section.  When it's required give an error
		// message.  When it's not required skip the contents.
		ଯଦି (c & SNF_REQUIRED)
		{
		    emsg(_(e_unsupported_section_in_spell_file));
		    ଯାଅ endFAIL;
		}
		ଯେତେବେଳେ (--len >= 0)
		    ଯଦି (getc(fd) < 0)
			ଯାଅ truncerr;
		ଭାଙ୍ଗ;
	}
someerror:
	ଯଦି (res == SP_FORMERROR)
	{
	    emsg(_(e_format_error_in_spell_file));
	    ଯାଅ endFAIL;
	}
	ଯଦି (res == SP_TRUNCERROR)
	{
truncerr:
	    emsg(_(e_truncated_spell_file));
	    ଯାଅ endFAIL;
	}
	ଯଦି (res == SP_OTHERERROR)
	    ଯାଅ endFAIL;
    }

    // <LWORDTREE>
    res = spell_read_ବୃକ୍ଷ(fd, &lp->sl_fbyts, &lp->sl_fbyts_len,
						      &lp->sl_fidxs, FALSE, 0);
    ଯଦି (res != 0)
	ଯାଅ someerror;

    // <KWORDTREE>
    res = spell_read_ବୃକ୍ଷ(fd, &lp->sl_kbyts, ଖାଲି, &lp->sl_kidxs, FALSE, 0);
    ଯଦି (res != 0)
	ଯାଅ someerror;

    // <PREFIXTREE>
    res = spell_read_ବୃକ୍ଷ(fd, &lp->sl_pbyts, ଖାଲି, &lp->sl_pidxs, TRUE,
							    lp->sl_prefixcnt);
    ଯଦି (res != 0)
	ଯାଅ someerror;

    // For a new file link it in the list of spell files.
    ଯଦି (old_lp == ଖାଲି && lang != ଖାଲି)
    {
	lp->sl_next = first_lang;
	first_lang = lp;
    }

    ଯାଅ endOK;

endFAIL:
    ଯଦି (lang != ଖାଲି)
	// truncating the name signals the error to spell_load_lang()
	*lang = NUL;
    ଯଦି (lp != ଖାଲି && old_lp == ଖାଲି)
	slang_free(lp);
    lp = ଖାଲି;

endOK:
    ଯଦି (fd != ଖାଲି)
	fclose(fd);
    ଯଦି (did_estack_ଠେଲ)
    {
	ESTACK_CHECK_NOW;
	estack_କାଢ଼();
    }

    ଫେରନ୍ତୁ lp;
}

/*
 * Fill in the wordcount fields for a trie.
 * Returns the total number of words.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ବୃକ୍ଷ_ଗଣନା_ଶବ୍ଦଗୁଡ଼ିକ(char_u *byts, long byts_len, idx_T *idxs)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		depth;
    idx_T	arridx[MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		curi[MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    idx_T	n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wordcount[MAXWLEN];

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;
    depth = 0;
    ଯେତେବେଳେ (depth >= 0 && !got_int)
    {
	ଯଦି (curi[depth] > byts[arridx[depth]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[depth]] = wordcount[depth];
	    ଯଦି (depth > 0)
		wordcount[depth - 1] += wordcount[depth];

	    --depth;
	    fast_breakcheck();
	}
	ଅନ୍ୟଥା
	{
	    // Do one more byte at this node.
	    n = arridx[depth] + curi[depth];
	    ++curi[depth];

	    c = byts[n];
	    ଯଦି (c == 0)
	    {
		// End of word, count it.
		++wordcount[depth];

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		ଯେତେବେଳେ (n + 1 < byts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[depth];
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (depth < MAXWLEN - 1)
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
    ଶୂନ୍ୟ
suggest_load_files(ଶୂନ୍ୟ)
{
    langp_T	*lp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lpi;
    slang_T	*slang;
    char_u	*dotp;
    FILE	*fd;
    char_u	buf[MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    time_t	timestamp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wcount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wordnr;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    // Do this for all languages that support sound folding.
    ପାଇଁ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ଯଦି (slang->sl_sugtime != 0 && !slang->sl_sugloaded)
	{
	    // Change ".spl" to ".sug" and open the file.  When the file isn't
	    // found silently skip it.  Do set "sl_sugloaded" so that we
	    // don't try again and again.
	    slang->sl_sugloaded = TRUE;

	    dotp = vim_strrchr(slang->sl_fname, '.');
	    ଯଦି (dotp == ଖାଲି || fnamecmp(dotp, ".spl") != 0)
		ଜାରି;
	    STRCPY(dotp, ".sug");
	    fd = mch_fopen((ଅକ୍ଷର *)slang->sl_fname, "r");
	    ଯଦି (fd == ଖାଲି)
		ଯାଅ nextone;

	    /*
	     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
	     */
	    ପାଇଁ (i = 0; i < VIMSUGMAGICL; ++i)
		buf[i] = (c = getc(fd)) == EOF ? 0 : c;	// <fileID>
	    ଯଦି (STRNCMP(buf, VIMSUGMAGIC, VIMSUGMAGICL) != 0)
	    {
		semsg(_(e_this_does_not_look_like_sug_file_str),
							     slang->sl_fname);
		ଯାଅ nextone;
	    }
	    c = getc(fd);				// <versionnr>
	    ଯଦି (c < VIMSUGVERSION)
	    {
		semsg(_(e_old_sug_file_needs_to_be_updated_str),
							     slang->sl_fname);
		ଯାଅ nextone;
	    }
	    ଅନ୍ୟଥା ଯଦି (c > VIMSUGVERSION)
	    {
		semsg(_(e_sug_file_is_for_newer_version_of_vim_str),
							     slang->sl_fname);
		ଯାଅ nextone;
	    }

	    // Check the timestamp, it must be exactly the same as the one in
	    // the .spl file.  Otherwise the word numbers won't match.
	    timestamp = get8ctime(fd);			// <timestamp>
	    ଯଦି (timestamp != slang->sl_sugtime)
	    {
		semsg(_(e_sug_file_doesnt_match_spl_file_str),
							     slang->sl_fname);
		ଯାଅ nextone;
	    }

	    /*
	     * <SUGWORDTREE>: <wordtree>
	     * Read the trie with the soundfolded words.
	     */
	    ଯଦି (spell_read_ବୃକ୍ଷ(fd, &slang->sl_sbyts, &slang->sl_sbyts_len,
			&slang->sl_sidxs, FALSE, 0) != 0)
	    {
someerror:
		semsg(_(e_error_while_reading_sug_file_str),
							     slang->sl_fname);
		slang_clear_sug(slang);
		ଯାଅ nextone;
	    }

	    /*
	     * <SUGTABLE>: <sugwcount> <sugline> ...
	     *
	     * Read the table with word numbers.  We use a file buffer for
	     * this, because it's so much like a file with lines.  Makes it
	     * possible to swap the info and save on memory use.
	     */
	    slang->sl_sugbuf = open_spellbuf();
	    ଯଦି (slang->sl_sugbuf == ଖାଲି)
		ଯାଅ someerror;
							    // <sugwcount>
	    wcount = get4c(fd);
	    ଯଦି (wcount < 0)
		ଯାଅ someerror;

	    // Read all the wordnr lists into the buffer, one NUL terminated
	    // list per line.
	    ga_init2(&ga, 1, 100);
	    ପାଇଁ (wordnr = 0; wordnr < wcount; ++wordnr)
	    {
		ga.ga_len = 0;
		ପାଇଁ (;;)
		{
		    c = getc(fd);			    // <sugline>
		    ଯଦି (c < 0 || ga_grow(&ga, 1) == FAIL)
			ଯାଅ someerror;
		    ((char_u *)ga.ga_data)[ga.ga_len++] = c;
		    ଯଦି (c == NUL)
			ଭାଙ୍ଗ;
		}
		ଯଦି (ml_append_buf(slang->sl_sugbuf, (linenr_T)wordnr,
					 ga.ga_data, ga.ga_len, TRUE) == FAIL)
		    ଯାଅ someerror;
	    }
	    ga_clear(&ga);

	    /*
	     * Need to put word counts in the word tries, so that we can find
	     * a word by its number.
	     */
	    ବୃକ୍ଷ_ଗଣନା_ଶବ୍ଦଗୁଡ଼ିକ(slang->sl_fbyts, slang->sl_fbyts_len,
		    slang->sl_fidxs);
	    ବୃକ୍ଷ_ଗଣନା_ଶବ୍ଦଗୁଡ଼ିକ(slang->sl_sbyts, slang->sl_sbyts_len,
		    slang->sl_sidxs);

nextone:
	    ଯଦି (fd != ଖାଲି)
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
    ସ୍ଥିର char_u *
read_cnt_string(FILE *fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt_bytes, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cntp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cnt = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*str;

    // read the length bytes, MSB first
    ପାଇଁ (i = 0; i < cnt_bytes; ++i)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = getc(fd);

	ଯଦି (c == EOF)
	{
	    *cntp = SP_TRUNCERROR;
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	cnt = (cnt << 8) + (unsigned)c;
    }
    *cntp = cnt;
    ଯଦି (cnt == 0)
	ଫେରନ୍ତୁ ଖାଲି;	    // nothing to read, return NULL

    str = read_string(fd, cnt);
    ଯଦି (str == ଖାଲି)
	*cntp = SP_OTHERERROR;
    ଫେରନ୍ତୁ str;
}

/*
 * Read SN_REGION: <regionname> ...
 * Return SP_*ERROR flags.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_region_section(FILE *fd, slang_T *lp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c = 0;

    ଯଦି (len > MAXREGIONS * 2)
	ଫେରନ୍ତୁ SP_FORMERROR;
    ପାଇଁ (i = 0; i < len; ++i)
	lp->sl_regions[i] = (c = getc(fd)) == EOF ? 0 : c; // <regionname>
    lp->sl_regions[len] = NUL;
    ଫେରନ୍ତୁ c == EOF ? SP_TRUNCERROR : 0;
}

/*
 * Read SN_CHARFLAGS section: <charflagslen> <charflags>
 *				<folcharslen> <folchars>
 * Return SP_*ERROR flags.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_charflags_section(FILE *fd)
{
    char_u	*flags;
    char_u	*fol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flagslen, follen;

    // <charflagslen> <charflags>
    flags = read_cnt_string(fd, 1, &flagslen);
    ଯଦି (flagslen < 0)
	ଫେରନ୍ତୁ flagslen;

    // <folcharslen> <folchars>
    fol = read_cnt_string(fd, 2, &follen);
    ଯଦି (follen < 0)
    {
	vim_free(flags);
	ଫେରନ୍ତୁ follen;
    }

    // Set the word-char flags and fill SPELL_ISUPPER() table.
    ଯଦି (flags != ଖାଲି && fol != ଖାଲି)
	set_spell_charflags(flags, flagslen, fol);

    vim_free(flags);
    vim_free(fol);

    // When <charflagslen> is zero then <fcharlen> must also be zero.
    ଯଦି ((flags == ଖାଲି) != (fol == ଖାଲି))
	ଫେରନ୍ତୁ SP_FORMERROR;
    ଫେରନ୍ତୁ 0;
}

/*
 * Read SN_PREFCOND section.
 * Return SP_*ERROR flags.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_prefcond_section(FILE *fd, slang_T *lp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cnt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    char_u	*p;
    char_u	buf[MAXWLEN + 1];

    // <prefcondcnt> <prefcond> ...
    cnt = get2c(fd);					// <prefcondcnt>
    ଯଦି (cnt <= 0)
	ଫେରନ୍ତୁ SP_FORMERROR;

    lp->sl_prefprog = ALLOC_CLEAR_MULT(regprog_T *, cnt);
    ଯଦି (lp->sl_prefprog == ଖାଲି)
	ଫେରନ୍ତୁ SP_OTHERERROR;
    lp->sl_prefixcnt = cnt;

    ପାଇଁ (i = 0; i < cnt; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	n = getc(fd);					// <condlen>
	ଯଦି (n < 0 || n >= MAXWLEN)
	    ଫେରନ୍ତୁ SP_FORMERROR;

	// When <condlen> is zero we have an empty condition.  Otherwise
	// compile the regexp program used to check for the condition.
	ଯଦି (n > 0)
	{
	    buf[0] = '^';	    // always match at one position only
	    p = buf + 1;
	    ଯେତେବେଳେ (n-- > 0)
		*p++ = (c = getc(fd)) == EOF ? 0 : c;	    // <condstr>
	    ଯଦି (c == EOF)
		ଭାଙ୍ଗ;
	    *p = NUL;
	    lp->sl_prefprog[i] = vim_regcomp(buf, RE_MAGIC + RE_STRING);
	}
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * Read REP or REPSAL items section from "fd": <repcount> <rep> ...
 * Return SP_*ERROR flags.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_rep_section(FILE *fd, garray_T *gap, short *first)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cnt;
    fromto_T	*ftp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    cnt = get2c(fd);					// <repcount>
    ଯଦି (cnt < 0)
	ଫେରନ୍ତୁ SP_TRUNCERROR;

    ଯଦି (ga_grow(gap, cnt) == FAIL)
	ଫେରନ୍ତୁ SP_OTHERERROR;

    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
    ପାଇଁ (; gap->ga_len < cnt; ++gap->ga_len)
    {
	ftp = &((fromto_T *)gap->ga_data)[gap->ga_len];
	ftp->ft_from = read_cnt_string(fd, 1, &i);
	ଯଦି (i < 0)
	    ଫେରନ୍ତୁ i;
	ଯଦି (i == 0)
	    ଫେରନ୍ତୁ SP_FORMERROR;
	ftp->ft_to = read_cnt_string(fd, 1, &i);
	ଯଦି (i <= 0)
	{
	    vim_free(ftp->ft_from);
	    ଯଦି (i < 0)
		ଫେରନ୍ତୁ i;
	    ଫେରନ୍ତୁ SP_FORMERROR;
	}
    }

    // Fill the first-index table.
    ପାଇଁ (i = 0; i < 256; ++i)
	first[i] = -1;
    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
    {
	ftp = &((fromto_T *)gap->ga_data)[i];
	ଯଦି (first[*ftp->ft_from] == -1)
	    first[*ftp->ft_from] = i;
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * Read SN_SAL section: <salflags> <salcount> <sal> ...
 * Return SP_*ERROR flags.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_sal_section(FILE *fd, slang_T *slang)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cnt;
    garray_T	*gap;
    salitem_T	*smp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ccnt;
    char_u	*p;

    slang->sl_sofo = FALSE;

    i = getc(fd);				// <salflags>
    ଯଦି (i & SAL_F0LLOWUP)
	slang->sl_followup = TRUE;
    ଯଦି (i & SAL_COLLAPSE)
	slang->sl_collapse = TRUE;
    ଯଦି (i & SAL_REM_ACCENTS)
	slang->sl_rem_accents = TRUE;

    cnt = get2c(fd);				// <salcount>
    ଯଦି (cnt < 0)
	ଫେରନ୍ତୁ SP_TRUNCERROR;

    gap = &slang->sl_sal;
    ga_init2(gap, ଆକାର(salitem_T), 10);
    ଯଦି (ga_grow(gap, cnt + 1) == FAIL)
	ଫେରନ୍ତୁ SP_OTHERERROR;

    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
    ପାଇଁ (; gap->ga_len < cnt; ++gap->ga_len)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c = NUL;

	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	ccnt = getc(fd);			// <salfromlen>
	ଯଦି (ccnt < 0)
	    ଫେରନ୍ତୁ SP_TRUNCERROR;
	ଯଦି ((p = alloc(ccnt + 2)) == ଖାଲି)
	    ଫେରନ୍ତୁ SP_OTHERERROR;
	smp->sm_lead = p;

	// Read up to the first special char into sm_lead.
	ପାଇଁ (i = 0; i < ccnt; ++i)
	{
	    c = getc(fd);			// <salfrom>
	    ଯଦି (vim_strchr((char_u *)"0123456789(-<^$", c) != ଖାଲି)
		ଭାଙ୍ଗ;
	    *p++ = c;
	}
	smp->sm_leadlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - smp->sm_lead);
	*p++ = NUL;

	// Put (abc) chars in sm_oneof, if any.
	ଯଦି (c == '(')
	{
	    smp->sm_oneof = p;
	    ପାଇଁ (++i; i < ccnt; ++i)
	    {
		c = getc(fd);			// <salfrom>
		ଯଦି (c == ')')
		    ଭାଙ୍ଗ;
		*p++ = c;
	    }
	    *p++ = NUL;
	    ଯଦି (++i < ccnt)
		c = getc(fd);
	}
	ଅନ୍ୟଥା
	    smp->sm_oneof = ଖାଲି;

	// Any following chars go in sm_rules.
	smp->sm_rules = p;
	ଯଦି (i < ccnt)
	    // store the char we got while checking for end of sm_lead
	    *p++ = c;
	ପାଇଁ (++i; i < ccnt; ++i)
	    *p++ = (c = getc(fd)) == EOF ? 0 : c;	// <salfrom>
	*p++ = NUL;

	// <saltolen> <salto>
	smp->sm_to = read_cnt_string(fd, 1, &ccnt);
	ଯଦି (ccnt < 0)
	{
	    vim_free(smp->sm_lead);
	    ଫେରନ୍ତୁ ccnt;
	}

	ଯଦି (has_mbyte)
	{
	    // convert the multi-byte strings to wide char strings
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = mb_charlen(smp->sm_lead);
	    ଯଦି (smp->sm_oneof == ଖାଲି)
		smp->sm_oneof_w = ଖାଲି;
	    ଅନ୍ୟଥା
		smp->sm_oneof_w = mb_str2wide(smp->sm_oneof);
	    ଯଦି (smp->sm_to == ଖାଲି)
		smp->sm_to_w = ଖାଲି;
	    ଅନ୍ୟଥା
		smp->sm_to_w = mb_str2wide(smp->sm_to);
	    ଯଦି (smp->sm_lead_w == ଖାଲି
		    || (smp->sm_oneof_w == ଖାଲି && smp->sm_oneof != ଖାଲି)
		    || (smp->sm_to_w == ଖାଲି && smp->sm_to != ଖାଲି))
	    {
		vim_free(smp->sm_lead);
		vim_free(smp->sm_to);
		vim_free(smp->sm_lead_w);
		vim_free(smp->sm_oneof_w);
		vim_free(smp->sm_to_w);
		ଫେରନ୍ତୁ SP_OTHERERROR;
	    }
	}
    }

    ଯଦି (gap->ga_len > 0)
    {
	// Add one extra entry to mark the end with an empty sm_lead.  Avoids
	// that we need to check the index every time.
	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	ଯଦି ((p = alloc(1)) == ଖାଲି)
	    ଫେରନ୍ତୁ SP_OTHERERROR;
	p[0] = NUL;
	smp->sm_lead = p;
	smp->sm_leadlen = 0;
	smp->sm_oneof = ଖାଲି;
	smp->sm_rules = p;
	smp->sm_to = ଖାଲି;
	ଯଦି (has_mbyte)
	{
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = 0;
	    smp->sm_oneof_w = ଖାଲି;
	    smp->sm_to_w = ଖାଲି;
	}
	++gap->ga_len;
    }

    // Fill the first-index table.
    set_sal_first(slang);

    ଫେରନ୍ତୁ 0;
}

/*
 * Read SN_WORDS: <word> ...
 * Return SP_*ERROR flags.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_ଶବ୍ଦଗୁଡ଼ିକ_section(FILE *fd, slang_T *lp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		done = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    char_u	word[MAXWLEN];

    ଯେତେବେଳେ (done < len)
    {
	// Read one word at a time.
	ପାଇଁ (i = 0; ; ++i)
	{
	    c = getc(fd);
	    ଯଦି (c == EOF)
		ଫେରନ୍ତୁ SP_TRUNCERROR;
	    word[i] = c;
	    ଯଦି (word[i] == NUL)
		ଭାଙ୍ଗ;
	    ଯଦି (i == MAXWLEN - 1)
		ଫେରନ୍ତୁ SP_FORMERROR;
	}

	// Init the count to 10.
	ଗଣନା_common_word(lp, word, -1, 10);
	done += i + 1;
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
 * Return SP_*ERROR flags.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_sofo_section(FILE *fd, slang_T *slang)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cnt;
    char_u	*from, *to;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;

    // <sofofromlen> <sofofrom>
    from = read_cnt_string(fd, 2, &cnt);
    ଯଦି (cnt < 0)
	ଫେରନ୍ତୁ cnt;

    // <sofotolen> <sofoto>
    to = read_cnt_string(fd, 2, &cnt);
    ଯଦି (cnt < 0)
    {
	vim_free(from);
	ଫେରନ୍ତୁ cnt;
    }

    // Store the info in slang->sl_sal and/or slang->sl_sal_first.
    ଯଦି (from != ଖାଲି && to != ଖାଲି)
	res = set_sofo(slang, from, to);
    ଅନ୍ୟଥା ଯଦି (from != ଖାଲି || to != ଖାଲି)
	res = SP_FORMERROR;    // only one of two strings is an error
    ଅନ୍ୟଥା
	res = 0;

    vim_free(from);
    vim_free(to);
    ଫେରନ୍ତୁ res;
}

/*
 * Read the compound section from the .spl file:
 *	<compmax> <compminlen> <compsylmax> <compoptions> <compflags>
 * Returns SP_*ERROR flags.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_compound(FILE *fd, slang_T *slang, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo = len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		atstart;
    char_u	*pat;
    char_u	*pp;
    char_u	*cp;
    char_u	*ap;
    char_u	*crp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cnt;
    garray_T	*gap;
    size_t	patsize;
    size_t	flagsize;

    ଯଦି (todo < 2)
	ଫେରନ୍ତୁ SP_FORMERROR;	// need at least two bytes

    --todo;
    c = getc(fd);					// <compmax>
    ଯଦି (c < 2)
	c = MAXWLEN;
    slang->sl_compmax = c;

    --todo;
    c = getc(fd);					// <compminlen>
    ଯଦି (c < 1)
	c = 0;
    slang->sl_compminlen = c;

    --todo;
    c = getc(fd);					// <compsylmax>
    ଯଦି (c < 1)
	c = MAXWLEN;
    slang->sl_compsylmax = c;

    c = getc(fd);					// <compoptions>
    ଯଦି (c != 0)
	ungetc(c, fd);	    // be backwards compatible with Vim 7.0b
    ଅନ୍ୟଥା
    {
	--todo;
	c = getc(fd);	    // only use the lower byte for now
	--todo;
	slang->sl_compoptions = c;

	gap = &slang->sl_comppat;
	c = get2c(fd);					// <comppatcount>
	ଯଦି (c < 0)
	    ଫେରନ୍ତୁ SP_TRUNCERROR;
	todo -= 2;
	ga_init2(gap, ଆକାର(char_u *), c);
	ଯଦି (ga_grow(gap, c) == OK)
	    ଯେତେବେଳେ (--c >= 0)
	    {
		((char_u **)(gap->ga_data))[gap->ga_len++] =
						  read_cnt_string(fd, 1, &cnt);
					    // <comppatlen> <comppattext>
		ଯଦି (cnt < 0)
		    ଫେରନ୍ତୁ cnt;
		todo -= cnt + 1;
	    }
    }
    ଯଦି (todo < 0)
	ଫେରନ୍ତୁ SP_FORMERROR;

    // Turn the COMPOUNDRULE items into a regexp pattern:
    // "a[bc]/a*b+" -> "^\(a[bc]\|a*b\+\)$".
    // Inserting backslashes may double the length, "^\(\)$<Nul>" is 7 bytes.
    // Conversion to utf-8 may double the size.
    ଯଦି ((size_t)todo > COMPOUND_MAX_LEN)
	ଫେରନ୍ତୁ SP_FORMERROR;
    patsize = (size_t)todo * 2 + 7;
    ଯଦି (enc_utf8)
	patsize += (size_t)todo * 2;
    flagsize = (size_t)todo + 1;
    pat = alloc(patsize);
    ଯଦି (pat == ଖାଲି)
	ଫେରନ୍ତୁ SP_OTHERERROR;

    // We also need a list of all flags that can appear at the start and one
    // for all flags.
    cp = alloc(flagsize);
    ଯଦି (cp == ଖାଲି)
    {
	vim_free(pat);
	ଫେରନ୍ତୁ SP_OTHERERROR;
    }
    slang->sl_compstartflags = cp;
    *cp = NUL;

    ap = alloc(flagsize);
    ଯଦି (ap == ଖାଲି)
    {
	vim_free(pat);
	ଫେରନ୍ତୁ SP_OTHERERROR;
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
    ଯେତେବେଳେ (todo-- > 0)
    {
	c = getc(fd);					// <compflags>
	ଯଦି (c == EOF)
	{
	    vim_free(pat);
	    ଫେରନ୍ତୁ SP_TRUNCERROR;
	}

	// Add all flags to "sl_compallflags".
	ଯଦି (vim_strchr((char_u *)"?*+[]/", c) == ଖାଲି
		&& !byte_in_str(slang->sl_compallflags, c))
	{
	    *ap++ = c;
	    *ap = NUL;
	}

	ଯଦି (atstart != 0)
	{
	    // At start of item: copy flags to "sl_compstartflags".  For a
	    // [abc] item set "atstart" to 2 and copy up to the ']'.
	    ଯଦି (c == '[')
		atstart = 2;
	    ଅନ୍ୟଥା ଯଦି (c == ']')
		atstart = 0;
	    ଅନ୍ୟଥା
	    {
		ଯଦି (!byte_in_str(slang->sl_compstartflags, c))
		{
		    *cp++ = c;
		    *cp = NUL;
		}
		ଯଦି (atstart == 1)
		    atstart = 0;
	    }
	}

	// Copy flag to "sl_comprules", unless we run into a wildcard.
	ଯଦି (crp != ଖାଲି)
	{
	    ଯଦି (c == '?' || c == '+' || c == '*')
	    {
		VIM_CLEAR(slang->sl_comprules);
		crp = ଖାଲି;
	    }
	    ଅନ୍ୟଥା
		*crp++ = c;
	}

	ଯଦି (c == '/')	    // slash separates two items
	{
	    *pp++ = '\\';
	    *pp++ = '|';
	    atstart = 1;
	}
	ଅନ୍ୟଥା		    // normal char, "[abc]" and '*' are copied as-is
	{
	    ଯଦି (c == '?' || c == '+' || c == '~')
		*pp++ = '\\';	    // "a?" becomes "a\?", "a+" becomes "a\+"
	    ଯଦି (enc_utf8)
		pp += mb_char2bytes(c, pp);
	    ଅନ୍ୟଥା
		*pp++ = c;
	}
    }

    *pp++ = '\\';
    *pp++ = ')';
    *pp++ = '$';
    *pp = NUL;

    ଯଦି (crp != ଖାଲି)
	*crp = NUL;

    slang->sl_compprog = vim_regcomp(pat, RE_MAGIC + RE_STRING + RE_STRICT);
    vim_free(pat);
    ଯଦି (slang->sl_compprog == ଖାଲି)
	ଫେରନ୍ତୁ SP_FORMERROR;

    ଫେରନ୍ତୁ 0;
}

/*
 * Set the SOFOFROM and SOFOTO items in language "lp".
 * Returns SP_*ERROR flags when there is something wrong.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_sofo(slang_T *lp, char_u *from, char_u *to)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    garray_T	*gap;
    char_u	*s;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*inp;

    ଯଦି (has_mbyte)
    {
	free_sal_ବସ୍ତୁଗୁଡ଼ିକ(&lp->sl_sal);

	// Use "sl_sal" as an array with 256 pointers to a list of wide
	// characters.  The index is the low byte of the character.
	// The list contains from-to pairs with a terminating NUL.
	// sl_sal_first[] is used for latin1 "from" characters.
	gap = &lp->sl_sal;
	ga_init2(gap, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *), 1);
	ଯଦି (ga_grow(gap, 256) == FAIL)
	    ଫେରନ୍ତୁ SP_OTHERERROR;
	vim_memset(gap->ga_data, 0, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *) * 256);
	gap->ga_len = 256;
	lp->sl_sofo = TRUE;

	// First count the number of items for each list.  Temporarily use
	// sl_sal_first[] for this.  Reset it first: a preceding SN_SAL section
	// may have set the entries to -1 via set_sal_first().
	vim_memset(lp->sl_sal_first, 0, ଆକାର(salfirst_T) * 256);
	ପାଇଁ (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    MB_CPTR_ADV(s);
	    ଯଦି (c >= 256)
		++lp->sl_sal_first[c & 0xff];
	}
	ଯଦି (*p != NUL || *s != NUL)	    // lengths differ
	    ଫେରନ୍ତୁ SP_FORMERROR;

	// Allocate the lists.
	ପାଇଁ (i = 0; i < 256; ++i)
	    ଯଦି (lp->sl_sal_first[i] > 0)
	    {
		p = alloc(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) * (lp->sl_sal_first[i] * 2 + 1));
		ଯଦି (p == ଖାଲି)
		    ଫେରନ୍ତୁ SP_OTHERERROR;
		((ପୂର୍ଣ୍ଣସଂଖ୍ୟା **)gap->ga_data)[i] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)p;
		*(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)p = 0;
	    }

	// Put the characters up to 255 in sl_sal_first[] the rest in a sl_sal
	// list.
	vim_memset(lp->sl_sal_first, 0, ଆକାର(salfirst_T) * 256);
	ପାଇଁ (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    i = mb_cptr2char_adv(&s);
	    ଯଦି (c >= 256)
	    {
		// Append the from-to chars at the end of the list with
		// the low byte.
		inp = ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା **)gap->ga_data)[c & 0xff];
		ଯେତେବେଳେ (*inp != 0)
		    ++inp;
		*inp++ = c;		// from char
		*inp++ = i;		// to char
		*inp++ = NUL;		// NUL at the end
	    }
	    ଅନ୍ୟଥା
		// mapping byte to char is done in sl_sal_first[]
		lp->sl_sal_first[c] = i;
	}
    }
    ଅନ୍ୟଥା
    {
	// mapping bytes to bytes is done in sl_sal_first[]
	ଯଦି (STRLEN(from) != STRLEN(to))
	    ଫେରନ୍ତୁ SP_FORMERROR;

	ପାଇଁ (i = 0; to[i] != NUL; ++i)
	    lp->sl_sal_first[from[i]] = to[i];
	lp->sl_sal.ga_len = 1;		// indicates we have soundfolding
	lp->sl_sofo = TRUE;
    }

    ଫେରନ୍ତୁ 0;
}

/*
 * Fill the first-index table for "lp".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_sal_first(slang_T *lp)
{
    salfirst_T	*sfirst;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    salitem_T	*smp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    garray_T	*gap = &lp->sl_sal;

    sfirst = lp->sl_sal_first;
    ପାଇଁ (i = 0; i < 256; ++i)
	sfirst[i] = -1;
    smp = (salitem_T *)gap->ga_data;
    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
    {
	ଯଦି (has_mbyte)
	    // Use the lowest byte of the first character.  For latin1 it's
	    // the character, for other encodings it should differ for most
	    // characters.
	    c = *smp[i].sm_lead_w & 0xff;
	ଅନ୍ୟଥା
	    c = *smp[i].sm_lead;
	ଯଦି (sfirst[c] == -1)
	{
	    sfirst[c] = i;
	    ଯଦି (has_mbyte)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

		// Make sure all entries with this byte are following each
		// other.  Move the ones that are in the wrong position.  Do
		// keep the same ordering!
		ଯେତେବେଳେ (i + 1 < gap->ga_len
				       && (*smp[i + 1].sm_lead_w & 0xff) == c)
		    // Skip over entry with same index byte.
		    ++i;

		ପାଇଁ (n = 1; i + n < gap->ga_len; ++n)
		    ଯଦି ((*smp[i + n].sm_lead_w & 0xff) == c)
		    {
			salitem_T  tsal;

			// Move entry with same index byte after the entries
			// we already found.
			++i;
			--n;
			tsal = smp[i + n];
			mch_memmove(smp + i + 1, smp + i,
						       ଆକାର(salitem_T) * n);
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା *
mb_str2wide(char_u *s)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*res;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;

    res = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, mb_charlen(s) + 1);
    ଯଦି (res == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (p = s; *p != NUL; )
	res[i++] = mb_ptr2char_adv(&p);
    res[i] = NUL;
    ଫେରନ୍ତୁ res;
}

/*
 * Read a tree from the .spl or .sug file.
 * Allocates the memory and stores pointers in "bytsp" and "idxsp".
 * This is skipped when the tree has zero length.
 * Returns zero when OK, SP_ value for an error.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
spell_read_ବୃକ୍ଷ(
    FILE	*fd,
    char_u	**bytsp,
    long	*bytsp_len,
    idx_T	**idxsp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prefixtree,	// TRUE for the prefix tree
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prefixcnt)	// when "prefixtree" is TRUE: prefix count
{
    long	len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    char_u	*bp;
    idx_T	*ip;

    // The tree size was computed when writing the file, so that we can
    // allocate it as one long block. <nodecount>
    len = get4c(fd);
    ଯଦି (len < 0)
	ଫେରନ୍ତୁ SP_TRUNCERROR;
    ଯଦି ((size_t)len > SIZE_MAX / ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା))
	// Invalid length, multiply with sizeof(int) would overflow.
	ଫେରନ୍ତୁ SP_FORMERROR;
    ଯଦି (len <= 0)
	ଫେରନ୍ତୁ 0;

    // Allocate the byte array.  Zero-initialize so that any position the
    // tree does not visit reads as 0; a stray BY_INDEX shared reference
    // into such a slot then behaves as end-of-word in spellsuggest()
    // instead of consuming an arbitrary heap byte as a siblingcount.
    bp = alloc_clear(len);
    ଯଦି (bp == ଖାଲି)
	ଫେରନ୍ତୁ SP_OTHERERROR;
    *bytsp = bp;
    ଯଦି (bytsp_len != ଖାଲି)
	*bytsp_len = len;

    // Allocate the index array.
    ip = lalloc_clear(len * ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା), TRUE);
    ଯଦି (ip == ଖାଲି)
	ଫେରନ୍ତୁ SP_OTHERERROR;
    *idxsp = ip;

    // Recursively read the tree and store it in the array.
    idx = read_ବୃକ୍ଷ_ଗଣ୍ଠି(fd, bp, ip, len, 0, prefixtree, prefixcnt, 0);
    ଯଦି (idx < 0)
	ଫେରନ୍ତୁ idx;
    ଯଦି (idx != len)
	ଫେରନ୍ତୁ SP_FORMERROR;
    ଫେରନ୍ତୁ 0;
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
    ସ୍ଥିର idx_T
read_ବୃକ୍ଷ_ଗଣ୍ଠି(
    FILE	*fd,
    char_u	*byts,
    idx_T	*idxs,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxidx,		    // size of arrays
    idx_T	startidx,	    // current index in "byts" and "idxs"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prefixtree,	    // TRUE for reading PREFIXTREE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxprefcondnr,	    // maximum for <prefcondnr>
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		depth)		    // recursion level
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    idx_T	idx = startidx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c2;
#define SHARED_MASK	0x8000000

    // Bail out on a crafted .spl whose tree recurses beyond the maximum
    // word length: each tree level corresponds to one byte of a word, so
    // any well-formed file has depth <= MAXWLEN.
    ଯଦି (depth > MAXWLEN)
	ଫେରନ୍ତୁ SP_FORMERROR;

    len = getc(fd);					// <siblingcount>
    ଯଦି (len <= 0)
	ଫେରନ୍ତୁ SP_TRUNCERROR;

    ଯଦି (len >= maxidx - startidx)
	ଫେରନ୍ତୁ SP_FORMERROR;
    byts[idx++] = len;

    // Read the byte values, flag/region bytes and shared indexes.
    ପାଇଁ (i = 1; i <= len; ++i)
    {
	c = getc(fd);					// <byte>
	ଯଦି (c < 0)
	    ଫେରନ୍ତୁ SP_TRUNCERROR;
	ଯଦି (c <= BY_SPECIAL)
	{
	    ଯଦି (c == BY_NOFLAGS && !prefixtree)
	    {
		// No flags, all regions.
		idxs[idx] = 0;
		c = 0;
	    }
	    ଅନ୍ୟଥା ଯଦି (c != BY_INDEX)
	    {
		ଯଦି (prefixtree)
		{
		    // Read the optional pflags byte, the prefix ID and the
		    // condition nr.  In idxs[] store the prefix ID in the low
		    // byte, the condition index shifted up 8 bits, the flags
		    // shifted up 24 bits.
		    ଯଦି (c == BY_FLAGS)
		    {
			n = getc(fd);			// <pflags>
			ଯଦି (n < 0)
			    ଫେରନ୍ତୁ SP_TRUNCERROR;
			c = n << 24;
		    }
		    ଅନ୍ୟଥା
			c = 0;

		    n = getc(fd);			// <affixID>
		    ଯଦି (n < 0)
			ଫେରନ୍ତୁ SP_TRUNCERROR;
		    c |= n;

		    n = get2c(fd);			// <prefcondnr>
		    ଯଦି (n < 0 || n >= maxprefcondnr)
			ଫେରନ୍ତୁ SP_FORMERROR;
		    c |= (n << 8);
		}
		ଅନ୍ୟଥା // c must be BY_FLAGS or BY_FLAGS2
		{
		    // Read flags and optional region and prefix ID.  In
		    // idxs[] the flags go in the low two bytes, region above
		    // that and prefix ID above the region.
		    c2 = c;
		    c = getc(fd);			// <flags>
		    ଯଦି (c < 0)
			ଫେରନ୍ତୁ SP_TRUNCERROR;
		    ଯଦି (c2 == BY_FLAGS2)
		    {
			n = getc(fd);			// <flags2>
			ଯଦି (n < 0)
			    ଫେରନ୍ତୁ SP_TRUNCERROR;
			c = (n << 8) + c;
		    }
		    ଯଦି (c & WF_REGION)
		    {
			n = getc(fd);			// <region>
			ଯଦି (n < 0)
			    ଫେରନ୍ତୁ SP_TRUNCERROR;
			c = (n << 16) + c;
		    }
		    ଯଦି (c & WF_AFX)
		    {
			n = getc(fd);			// <affixID>
			ଯଦି (n < 0)
			    ଫେରନ୍ତୁ SP_TRUNCERROR;
			c = (n << 24) + c;
		    }
		}

		idxs[idx] = c;
		c = 0;
	    }
	    ଅନ୍ୟଥା // c == BY_INDEX
	    {
							// <nodeidx>
		n = get3c(fd);
		ଯଦି (n < 0 || n >= maxidx)
		    ଫେରନ୍ତୁ SP_FORMERROR;
		idxs[idx] = n + SHARED_MASK;
		c = getc(fd);				// <xbyte>
	    }
	}
	byts[idx++] = c;
    }

    // Recursively read the children for non-shared siblings.
    // Skip the end-of-word ones (zero byte value) and the shared ones (and
    // remove SHARED_MASK)
    ପାଇଁ (i = 1; i <= len; ++i)
	ଯଦି (byts[startidx + i] != 0)
	{
	    ଯଦି (idxs[startidx + i] & SHARED_MASK)
		idxs[startidx + i] &= ~SHARED_MASK;
	    ଅନ୍ୟଥା
	    {
		idxs[startidx + i] = idx;
		idx = read_ବୃକ୍ଷ_ଗଣ୍ଠି(fd, byts, idxs, maxidx, idx,
					prefixtree, maxprefcondnr, depth + 1);
		ଯଦି (idx < 0)
		    ଭାଙ୍ଗ;
	    }
	}

    ଫେରନ୍ତୁ idx;
}

/*
 * Reload the spell file "fname" if it's loaded.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
spell_reload_one(
    char_u	*fname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		added_word)	// invoked through "zg"
{
    slang_T	*slang;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		didit = FALSE;

    FOR_ALL_SPELL_LANGS(slang)
    {
	ଯଦି (fullpathcmp(fname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	{
	    slang_clear(slang);
	    ଯଦି (spell_load_file(fname, ଖାଲି, slang, FALSE) == ଖାଲି)
		// reloading failed, clear the language
		slang_clear(slang);
	    redraw_all_later(UPD_SOME_VALID);
	    didit = TRUE;
	}
    }

    // When "zg" was used and the file wasn't loaded yet, should redo
    // 'spelllang' to load it now.
    ଯଦି (added_word && !didit)
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା afffile_S
{
    char_u	*af_enc;	// "SET", normalized, alloc'ed string or NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		af_flagtype;	// AFT_CHAR, AFT_LONG, AFT_NUM or AFT_CAPLONG
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		af_pfxpostpone;	// postpone prefixes without chop string and
				// without flags
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		af_ignoreextra;	// IGNOREEXTRA present
    hashtab_T	af_pref;	// hashtable for prefixes, affheader_T
    hashtab_T	af_suff;	// hashtable for suffixes, affheader_T
    hashtab_T	af_comp;	// hashtable for compound flags, compitem_T
} afffile_T;

#define AFT_CHAR	0	// flags are one character
#define AFT_LONG	1	// flags are two characters
#define AFT_CAPLONG	2	// flags are one or two characters
#define AFT_ଅଙ୍କ		3	// flags are numbers, comma separated

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା affentry_S affentry_T;
// Affix entry from ".aff" file.  Used for prefixes and suffixes.
ସଂରଚନା affentry_S
{
    affentry_T	*ae_next;	// next affix with same name/number
    char_u	*ae_chop;	// text to chop off basic word (can be NULL)
    char_u	*ae_add;	// text to add to basic word (can be NULL)
    char_u	*ae_flags;	// flags on the affix (can be NULL)
    char_u	*ae_cond;	// condition (NULL for ".")
    regprog_T	*ae_prog;	// regexp program for ae_cond or NULL
    ଅକ୍ଷର	ae_compforbid;	// COMPOUNDFORBIDFLAG found
    ଅକ୍ଷର	ae_comppermit;	// COMPOUNDPERMITFLAG found
};

#define AH_ଚାବି_LEN 17		// 2 x 8 bytes + NUL

// Affix header from ".aff" file.  Used for af_pref and af_suff.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା affheader_S
{
    char_u	ah_ଚାବି[AH_ଚାବି_LEN]; // key for hashtab == name of affix
    unsigned	ah_flag;	// affix name as number, uses "af_flagtype"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ah_newID;	// prefix ID after renumbering; 0 if not used
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ah_combine;	// suffix may combine with prefix
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ah_follows;	// another affix block should be following
    affentry_T	*ah_first;	// first affix entry
} affheader_T;

#define HI2AH(hi)   ((affheader_T *)(hi)->hi_ଚାବି)

// Flag used in compound items.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା compitem_S
{
    char_u	ci_ଚାବି[AH_ଚାବି_LEN]; // key for hashtab == name of compound
    unsigned	ci_flag;	// affix name as number, uses "af_flagtype"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ci_newID;	// affix ID after renumbering.
} compitem_T;

#define HI2CI(hi)   ((compitem_T *)(hi)->hi_ଚାବି)

/*
 * Structure that is used to store the items in the word tree.  This avoids
 * the need to keep track of each allocated thing, everything is freed all at
 * once after ":mkspell" is done.
 * Note: "sb_next" must be just before "sb_data" to make sure the alignment of
 * "sb_data" is correct for systems where pointers must be aligned on
 * pointer-size boundaries and sizeof(pointer) > sizeof(int) (e.g., Sparc).
 */
#define  SBLOCKSIZE 16000	// size of sb_data
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା sblock_S sblock_T;
ସଂରଚନା sblock_S
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sb_used;	// nr of bytes already in use
    sblock_T	*sb_next;	// next block in list
    char_u	sb_data[1];	// data, actually longer
};

/*
 * A node in the tree.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା wordnode_S wordnode_T;
ସଂରଚନା wordnode_S
{
    ସଂଘ   // shared to save space
    {
	char_u	hashkey[6];	// the hash key, only used while compressing
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	index;		// index in written nodes (valid after first
				// round)
    } wn_u1;
    ସଂଘ   // shared to save space
    {
	wordnode_T *next;	// next node with same hash key
	wordnode_T *wnode;	// parent node that will write this node
    } wn_u2;
    wordnode_T	*wn_child;	// child (next byte in word)
    wordnode_T  *wn_sibling;	// next sibling (alternate byte in word,
				// always sorted)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wn_refs;	// Nr. of references to this node.  Only
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wn_nr;		// sequence nr for printing
#endif
};

#define WN_MASK	 0xffff		// mask relevant bits of "wn_flags"

#define HI2WN(hi)    (wordnode_T *)((hi)->hi_ଚାବି)

/*
 * Info used while reading the spell files.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା spellinfo_S
{
    wordnode_T	*si_foldroot;	// tree with case-folded words
    long	si_foldwcount;	// nr of words in si_foldroot

    wordnode_T	*si_keeproot;	// tree with keep-case words
    long	si_keepwcount;	// nr of words in si_keeproot

    wordnode_T	*si_prefroot;	// tree with postponed prefixes

    long	si_sugtree;	// creating the soundfolding trie

    sblock_T	*si_blocks;	// memory blocks used
    long	si_blocks_cnt;	// memory blocks allocated
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_did_emsg;	// TRUE when ran out of memory

    long	si_compress_cnt;    // words to add before lowering
				    // compression limit
    wordnode_T	*si_first_free; // List of nodes that have been freed during
				// compression, linked by "wn_child" field.
    long	si_free_ଗଣନା;	// number of nodes in si_first_free
#ifdef SPELL_PRINTTREE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_wordnode_nr;	// sequence nr for nodes
#endif
    buf_T	*si_spellbuf;	// buffer used to store soundfold word table

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_ascii;	// handling only ASCII words
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_add;		// addition file
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_clear_chartab;   // when TRUE clear char tables
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_region;	// region mask
    vimconv_T	si_conv;	// for conversion to 'encoding'
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_memtot;	// runtime memory used
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_verbose;	// verbose messages
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_ସନ୍ଦେଶ_ଗଣନା;	// number of words added since last message
    char_u	*si_info;	// info text chars or NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_region_ଗଣନା; // number of regions supported (1 when there
				 // are no regions)
    char_u	si_region_name[MAXREGIONS * 2 + 1];
				// region names; used only if
				// si_region_count > 1)

    garray_T	si_rep;		// list of fromto_T entries from REP lines
    garray_T	si_repsal;	// list of fromto_T entries from REPSAL lines
    garray_T	si_sal;		// list of fromto_T entries from SAL lines
    char_u	*si_sofofr;	// SOFOFROM text
    char_u	*si_sofoto;	// SOFOTO text
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_nosugfile;	// NOSUGFILE item found
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_nosplitsugs;	// NOSPLITSUGS item found
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_nocompoundsugs; // NOCOMPOUNDSUGS item found
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_followup;	// soundsalike: ?
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_collapse;	// soundsalike: ?
    hashtab_T	si_commonwords;	// hashtable for common words
    time_t	si_sugtime;	// timestamp for .sug file
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_rem_accents;	// soundsalike: remove accents
    garray_T	si_map;		// MAP info concatenated
    char_u	*si_midword;	// MIDWORD chars or NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_compmax;	// max nr of words for compounding
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_compminlen;	// minimal length for compounding
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_compsylmax;	// max nr of syllables for compounding
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_compoptions;	// COMP_ flags
    garray_T	si_comppat;	// CHECKCOMPOUNDPATTERN items, each stored as
				// a string
    char_u	*si_compflags;	// flags used for compounding
    char_u	si_nobreak;	// NOBREAK
    char_u	*si_syllable;	// syllable string
    garray_T	si_prefcond;	// table with conditions for postponed
				// prefixes, each stored as a string
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_newprefID;	// current value for ah_newID
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_newcompID;	// current value for compound ID
} spellinfo_T;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_aff_rule(char_u **ବସ୍ତୁଗୁଡ଼ିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା itemcnt, ଅକ୍ଷର *rulename, ପୂର୍ଣ୍ଣସଂଖ୍ୟା	 mincount);
ସ୍ଥିର ଶୂନ୍ୟ aff_process_flags(afffile_T *affile, affentry_T *entry);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା spell_info_item(char_u *s);
ସ୍ଥିର unsigned affitem2flag(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flagtype, char_u *item, char_u	*fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lnum);
ସ୍ଥିର unsigned get_affitem(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flagtype, char_u **pp);
ସ୍ଥିର ଶୂନ୍ୟ process_compflags(spellinfo_T *spin, afffile_T *aff, char_u *compflags);
ସ୍ଥିର ଶୂନ୍ୟ check_renumber(spellinfo_T *spin);
ସ୍ଥିର ଶୂନ୍ୟ aff_check_ସଂଖ୍ୟା(ପୂର୍ଣ୍ଣସଂଖ୍ୟା spinval, ପୂର୍ଣ୍ଣସଂଖ୍ୟା affval, ଅକ୍ଷର *name);
ସ୍ଥିର ଶୂନ୍ୟ aff_check_string(char_u *spinval, char_u *affval, ଅକ୍ଷର *name);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା str_equal(char_u *s1, char_u	*s2);
ସ୍ଥିର ଶୂନ୍ୟ add_fromto(spellinfo_T *spin, garray_T *gap, char_u	*from, char_u *to);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sal_to_bool(char_u *s);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_affix_flags(afffile_T *affile, char_u *afflist);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_pfxlist(afffile_T *affile, char_u *afflist, char_u *store_afflist, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cntp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_compflags(afffile_T *affile, char_u *afflist, char_u *store_afflist, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cntp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା store_aff_word(spellinfo_T *spin, char_u *word, char_u *afflist, afffile_T *affile, hashtab_T *ht, hashtab_T *xht, ପୂର୍ଣ୍ଣସଂଖ୍ୟା condit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, char_u *pfxlist, ପୂର୍ଣ୍ଣସଂଖ୍ୟା pfxlen);
ସ୍ଥିର ଶୂନ୍ୟ *getroom(spellinfo_T *spin, size_t len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା align);
ସ୍ଥିର char_u *getroom_save(spellinfo_T *spin, char_u *s);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା store_word(spellinfo_T *spin, char_u *word, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା region, char_u *pfxlist, ପୂର୍ଣ୍ଣସଂଖ୍ୟା need_affix);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବୃକ୍ଷ_add_word(spellinfo_T *spin, char_u *word, wordnode_T *ବୃକ୍ଷ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା region, ପୂର୍ଣ୍ଣସଂଖ୍ୟା affixID);
ସ୍ଥିର wordnode_T *get_wordnode(spellinfo_T *spin);
ସ୍ଥିର ଶୂନ୍ୟ free_wordnode(spellinfo_T *spin, wordnode_T *n);
ସ୍ଥିର ଶୂନ୍ୟ wordtree_compress(spellinfo_T *spin, wordnode_T *ମୂଳ, ଅକ୍ଷର *name);
ସ୍ଥିର long ଗଣ୍ଠି_compress(spellinfo_T *spin, wordnode_T *ଗଣ୍ଠି, hashtab_T *ht, long *tot);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି_equal(wordnode_T *n1, wordnode_T *n2);
ସ୍ଥିର ଶୂନ୍ୟ clear_ଗଣ୍ଠି(wordnode_T *ଗଣ୍ଠି);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା put_ଗଣ୍ଠି(FILE *fd, wordnode_T *ଗଣ୍ଠି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା regionmask, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prefixtree);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sug_filltree(spellinfo_T *spin, slang_T *slang);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sug_maketable(spellinfo_T *spin);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sug_filltable(spellinfo_T *spin, wordnode_T *ଗଣ୍ଠି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା startwordnr, garray_T *gap);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset2bytes(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr, char_u *buf);
ସ୍ଥିର ଶୂନ୍ୟ sug_write(spellinfo_T *spin, char_u *fname);
ସ୍ଥିର ଶୂନ୍ୟ spell_message(spellinfo_T *spin, char_u *str);
ସ୍ଥିର ଶୂନ୍ୟ init_spellfile(ଶୂନ୍ୟ);

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
ସ୍ଥିର long compress_start = 30000;	// memory / SBLOCKSIZE
ସ୍ଥିର long compress_inc = 100;		// memory / SBLOCKSIZE
ସ୍ଥିର long compress_added = 500000;	// word count

/*
 * Check the 'mkspellmem' option.  Return FAIL if it's wrong.
 * Sets "sps_flags".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
spell_check_msm(ଶୂନ୍ୟ)
{
    char_u	*p = p_msm;
    long	start = 0;
    long	incr = 0;
    long	added = 0;

    ଯଦି (!VIM_ISDIGIT(*p))
	ଫେରନ୍ତୁ FAIL;
    // block count = (value * 1024) / SBLOCKSIZE (but avoid overflow)
    start = (getdigits(&p) * 10) / (SBLOCKSIZE / 102);
    ଯଦି (*p != ',')
	ଫେରନ୍ତୁ FAIL;
    ++p;
    ଯଦି (!VIM_ISDIGIT(*p))
	ଫେରନ୍ତୁ FAIL;
    incr = (getdigits(&p) * 102) / (SBLOCKSIZE / 10);
    ଯଦି (*p != ',')
	ଫେରନ୍ତୁ FAIL;
    ++p;
    ଯଦି (!VIM_ISDIGIT(*p))
	ଫେରନ୍ତୁ FAIL;
    added = getdigits(&p) * 1024;
    ଯଦି (*p != NUL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (start == 0 || incr == 0 || added == 0 || incr > start)
	ଫେରନ୍ତୁ FAIL;

    compress_start = start;
    compress_inc = incr;
    compress_added = added;
    ଫେରନ୍ତୁ OK;
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

ସ୍ଥିର ଅକ୍ଷର line1[PRINTLINESIZE];
ସ୍ଥିର ଅକ୍ଷର line2[PRINTLINESIZE];
ସ୍ଥିର ଅକ୍ଷର line3[PRINTLINESIZE];

    ସ୍ଥିର ଶୂନ୍ୟ
spell_clear_flags(wordnode_T *ଗଣ୍ଠି)
{
    wordnode_T	*np;

    FOR_ALL_ଗଣ୍ଠି_SIBLINGS(ଗଣ୍ଠି, np)
    {
	np->wn_u1.index = FALSE;
	spell_clear_flags(np->wn_child);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
spell_print_ଗଣ୍ଠି(wordnode_T *ଗଣ୍ଠି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth)
{
    ଯଦି (ଗଣ୍ଠି->wn_u1.index)
    {
	// Done this node before, print the reference.
	PRINTSOME(line1, depth, "(%d)", ଗଣ୍ଠି->wn_nr, 0);
	PRINTSOME(line2, depth, "    ", 0, 0);
	PRINTSOME(line3, depth, "    ", 0, 0);
	ସନ୍ଦେଶ(line1);
	ସନ୍ଦେଶ(line2);
	ସନ୍ଦେଶ(line3);
    }
    ଅନ୍ୟଥା
    {
	ଗଣ୍ଠି->wn_u1.index = TRUE;

	ଯଦି (ଗଣ୍ଠି->wn_byte != NUL)
	{
	    ଯଦି (ଗଣ୍ଠି->wn_child != ଖାଲି)
		PRINTSOME(line1, depth, " %c -> ", ଗଣ୍ଠି->wn_byte, 0);
	    ଅନ୍ୟଥା
		// Cannot happen?
		PRINTSOME(line1, depth, " %c ???", ଗଣ୍ଠି->wn_byte, 0);
	}
	ଅନ୍ୟଥା
	    PRINTSOME(line1, depth, " $    ", 0, 0);

	PRINTSOME(line2, depth, "%d/%d    ", ଗଣ୍ଠି->wn_nr, ଗଣ୍ଠି->wn_refs);

	ଯଦି (ଗଣ୍ଠି->wn_sibling != ଖାଲି)
	    PRINTSOME(line3, depth, " |    ", 0, 0);
	ଅନ୍ୟଥା
	    PRINTSOME(line3, depth, "      ", 0, 0);

	ଯଦି (ଗଣ୍ଠି->wn_byte == NUL)
	{
	    ସନ୍ଦେଶ(line1);
	    ସନ୍ଦେଶ(line2);
	    ସନ୍ଦେଶ(line3);
	}

	// do the children
	ଯଦି (ଗଣ୍ଠି->wn_byte != NUL && ଗଣ୍ଠି->wn_child != ଖାଲି)
	    spell_print_ଗଣ୍ଠି(ଗଣ୍ଠି->wn_child, depth + 1);

	// do the siblings
	ଯଦି (ଗଣ୍ଠି->wn_sibling != ଖାଲି)
	{
	    // get rid of all parent details except |
	    STRCPY(line1, line3);
	    STRCPY(line2, line3);
	    spell_print_ଗଣ୍ଠି(ଗଣ୍ଠି->wn_sibling, depth);
	}
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
spell_print_ବୃକ୍ଷ(wordnode_T *ମୂଳ)
{
    ଯଦି (ମୂଳ == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Clear the "wn_u1.index" fields, used to remember what has been done.
    spell_clear_flags(ମୂଳ);

    // Recursively print the tree.
    spell_print_ଗଣ୍ଠି(ମୂଳ, 0);
}
#endif // SPELL_PRINTTREE

/*
 * Read the affix file "fname".
 * Returns an afffile_T, NULL for complete failure.
 */
    ସ୍ଥିର afffile_T *
spell_read_aff(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    afffile_T	*aff;
    char_u	rline[MAXLINELEN];
    char_u	*ଧାଡ଼ି;
    char_u	*pc = ଖାଲି;
#define MAXITEMCNT  30
    char_u	*(ବସ୍ତୁଗୁଡ଼ିକ[MAXITEMCNT]);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		itemcnt;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lnum = 0;
    affheader_T	*cur_aff = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_postpone_prefix = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		aff_todo = 0;
    hashtab_T	*tp;
    char_u	*ତଳ = ଖାଲି;
    char_u	*fol = ଖାଲି;
    char_u	*upp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_rep;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_repsal;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_sal;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_mapline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_map = FALSE;
    hashitem_T	*hi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compminlen = 0;		// COMPOUNDMIN value
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compsylmax = 0;		// COMPOUNDSYLMAX value
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compoptions = 0;	// COMP_ flags
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		compmax = 0;		// COMPOUNDWORDMAX value
    char_u	*compflags = ଖାଲି;	// COMPOUNDFLAG and COMPOUNDRULE
					// concatenated
    char_u	*midword = ଖାଲି;	// MIDWORD value
    char_u	*syllable = ଖାଲି;	// SYLLABLE value
    char_u	*sofofrom = ଖାଲି;	// SOFOFROM value
    char_u	*sofoto = ଖାଲି;		// SOFOTO value

    /*
     * Open the file.
     */
    fd = mch_fopen((ଅକ୍ଷର *)fname, "r");
    ଯଦି (fd == ଖାଲି)
    {
	semsg(_(e_cant_open_file_str), fname);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, _("Reading affix file %s..."), fname);
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
    aff = (afffile_T *)getroom(spin, ଆକାର(afffile_T), TRUE);
    ଯଦି (aff == ଖାଲି)
    {
	fclose(fd);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    hash_init(&aff->af_pref);
    hash_init(&aff->af_suff);
    hash_init(&aff->af_comp);

    /*
     * Read all the lines in the file one by one.
     */
    ଯେତେବେଳେ (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	ଧାଡ଼ି_breakcheck();
	++lnum;

	// Skip comment lines.
	ଯଦି (*rline == '#')
	    ଜାରି;

	// Convert from "SET" to 'encoding' when needed.
	vim_free(pc);
	ଯଦି (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, ଖାଲି);
	    ଯଦି (pc == ଖାଲି)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
							   fname, lnum, rline);
		ଜାରି;
	    }
	    ଧାଡ଼ି = pc;
	}
	ଅନ୍ୟଥା
	{
	    pc = ଖାଲି;
	    ଧାଡ଼ି = rline;
	}

	// Split the line up in white separated items.  Put a NUL after each
	// item.
	itemcnt = 0;
	ପାଇଁ (p = ଧାଡ଼ି; ; )
	{
	    ଯେତେବେଳେ (*p != NUL && *p <= ' ')  // skip white space and CR/NL
		++p;
	    ଯଦି (*p == NUL)
		ଭାଙ୍ଗ;
	    ଯଦି (itemcnt == MAXITEMCNT)	    // too many items
		ଭାଙ୍ଗ;
	    ବସ୍ତୁଗୁଡ଼ିକ[itemcnt++] = p;
	    // A few items have arbitrary text argument, don't split them.
	    ଯଦି (itemcnt == 2 && spell_info_item(ବସ୍ତୁଗୁଡ଼ିକ[0]))
		ଯେତେବେଳେ (*p >= ' ' || *p == TAB)    // skip until CR/NL
		    ++p;
	    ଅନ୍ୟଥା
		ଯେତେବେଳେ (*p > ' ')    // skip until white space or CR/NL
		    ++p;
	    ଯଦି (*p == NUL)
		ଭାଙ୍ଗ;
	    *p++ = NUL;
	}

	// Handle non-empty lines.
	ଯଦି (itemcnt > 0)
	{
	    ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "SET", 2) && aff->af_enc == ଖାଲି)
	    {
		// Setup for conversion from "ENC" to 'encoding'.
		aff->af_enc = enc_canonize(ବସ୍ତୁଗୁଡ଼ିକ[1]);
		ଯଦି (aff->af_enc != ଖାଲି && !spin->si_ascii
			&& convert_setup(&spin->si_conv, aff->af_enc,
							       p_enc) == FAIL)
		    smsg(_("Conversion in %s not supported: from %s to %s"),
					       fname, aff->af_enc, p_enc);
		spin->si_conv.vc_fail = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "FLAG", 2)
					      && aff->af_flagtype == AFT_CHAR)
	    {
		ଯଦି (STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[1], "long") == 0)
		    aff->af_flagtype = AFT_LONG;
		ଅନ୍ୟଥା ଯଦି (STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[1], "num") == 0)
		    aff->af_flagtype = AFT_ଅଙ୍କ;
		ଅନ୍ୟଥା ଯଦି (STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[1], "caplong") == 0)
		    aff->af_flagtype = AFT_CAPLONG;
		ଅନ୍ୟଥା
		    smsg(_("Invalid value for FLAG in %s line %d: %s"),
			    fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[1]);
		ଯଦି (aff->af_rare != 0
			|| aff->af_keepcase != 0
			|| aff->af_bad != 0
			|| aff->af_needaffix != 0
			|| aff->af_circumfix != 0
			|| aff->af_needcomp != 0
			|| aff->af_comproot != 0
			|| aff->af_nosuggest != 0
			|| compflags != ଖାଲି
			|| aff->af_suff.ht_used > 0
			|| aff->af_pref.ht_used > 0)
		    smsg(_("FLAG after using flags in %s line %d: %s"),
			    fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[1]);
	    }
	    ଅନ୍ୟଥା ଯଦି (spell_info_item(ବସ୍ତୁଗୁଡ଼ିକ[0]))
	    {
		    p = (char_u *)getroom(spin,
			    (spin->si_info == ଖାଲି ? 0 : STRLEN(spin->si_info))
			    + STRLEN(ବସ୍ତୁଗୁଡ଼ିକ[0])
			    + STRLEN(ବସ୍ତୁଗୁଡ଼ିକ[1]) + 3, FALSE);
		    ଯଦି (p != ଖାଲି)
		    {
			ଯଦି (spin->si_info != ଖାଲି)
			{
			    STRCPY(p, spin->si_info);
			    STRCAT(p, "\n");
			}
			STRCAT(p, ବସ୍ତୁଗୁଡ଼ିକ[0]);
			STRCAT(p, " ");
			STRCAT(p, ବସ୍ତୁଗୁଡ଼ିକ[1]);
			spin->si_info = p;
		    }
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "MIDWORD", 2)
							   && midword == ଖାଲି)
	    {
		midword = getroom_save(spin, ବସ୍ତୁଗୁଡ଼ିକ[1]);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "TRY", 2))
	    {
		// ignored, we look in the tree for what chars may appear
	    }
	    // TODO: remove "RAR" later
	    ଅନ୍ୟଥା ଯଦି ((is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "RAR", 2)
			|| is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "RARE", 2))
							 && aff->af_rare == 0)
	    {
		aff->af_rare = affitem2flag(aff->af_flagtype, ବସ୍ତୁଗୁଡ଼ିକ[1],
								 fname, lnum);
	    }
	    // TODO: remove "KEP" later
	    ଅନ୍ୟଥା ଯଦି ((is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "KEP", 2)
			|| is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "KEEPCASE", 2))
						     && aff->af_keepcase == 0)
	    {
		aff->af_keepcase = affitem2flag(aff->af_flagtype, ବସ୍ତୁଗୁଡ଼ିକ[1],
								 fname, lnum);
	    }
	    ଅନ୍ୟଥା ଯଦି ((is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "BAD", 2)
			|| is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "FORBIDDENWORD", 2))
							  && aff->af_bad == 0)
	    {
		aff->af_bad = affitem2flag(aff->af_flagtype, ବସ୍ତୁଗୁଡ଼ିକ[1],
								 fname, lnum);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "NEEDAFFIX", 2)
						    && aff->af_needaffix == 0)
	    {
		aff->af_needaffix = affitem2flag(aff->af_flagtype, ବସ୍ତୁଗୁଡ଼ିକ[1],
								 fname, lnum);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "CIRCUMFIX", 2)
						    && aff->af_circumfix == 0)
	    {
		aff->af_circumfix = affitem2flag(aff->af_flagtype, ବସ୍ତୁଗୁଡ଼ିକ[1],
								 fname, lnum);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "NOSUGGEST", 2)
						    && aff->af_nosuggest == 0)
	    {
		aff->af_nosuggest = affitem2flag(aff->af_flagtype, ବସ୍ତୁଗୁଡ଼ିକ[1],
								 fname, lnum);
	    }
	    ଅନ୍ୟଥା ଯଦି ((is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "NEEDCOMPOUND", 2)
			|| is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "ONLYINCOMPOUND", 2))
						     && aff->af_needcomp == 0)
	    {
		aff->af_needcomp = affitem2flag(aff->af_flagtype, ବସ୍ତୁଗୁଡ଼ିକ[1],
								 fname, lnum);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "COMPOUNDROOT", 2)
						     && aff->af_comproot == 0)
	    {
		aff->af_comproot = affitem2flag(aff->af_flagtype, ବସ୍ତୁଗୁଡ଼ିକ[1],
								 fname, lnum);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "COMPOUNDFORBIDFLAG", 2)
						   && aff->af_compforbid == 0)
	    {
		aff->af_compforbid = affitem2flag(aff->af_flagtype, ବସ୍ତୁଗୁଡ଼ିକ[1],
								 fname, lnum);
		ଯଦି (aff->af_pref.ht_used > 0)
		    smsg(_("Defining COMPOUNDFORBIDFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "COMPOUNDPERMITFLAG", 2)
						   && aff->af_comppermit == 0)
	    {
		aff->af_comppermit = affitem2flag(aff->af_flagtype, ବସ୍ତୁଗୁଡ଼ିକ[1],
								 fname, lnum);
		ଯଦି (aff->af_pref.ht_used > 0)
		    smsg(_("Defining COMPOUNDPERMITFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "COMPOUNDFLAG", 2)
							 && compflags == ଖାଲି)
	    {
		// Turn flag "c" into COMPOUNDRULE compatible string "c+",
		// "Na" into "Na+", "1234" into "1234+".
		p = getroom(spin, STRLEN(ବସ୍ତୁଗୁଡ଼ିକ[1]) + 2, FALSE);
		ଯଦି (p != ଖାଲି)
		{
		    STRCPY(p, ବସ୍ତୁଗୁଡ଼ିକ[1]);
		    STRCAT(p, "+");
		    compflags = p;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "COMPOUNDRULES", 2))
	    {
		// We don't use the count, but do check that it's a number and
		// not COMPOUNDRULE mistyped.
		ଯଦି (atoi((ଅକ୍ଷର *)ବସ୍ତୁଗୁଡ଼ିକ[1]) == 0)
		    smsg(_("Wrong COMPOUNDRULES value in %s line %d: %s"),
						       fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[1]);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "COMPOUNDRULE", 2))
	    {
		// Don't use the first rule if it is a number.
		ଯଦି (compflags != ଖାଲି || *skipdigits(ବସ୍ତୁଗୁଡ଼ିକ[1]) != NUL)
		{
		    // Concatenate this string to previously defined ones,
		    // using a slash to separate them.
		    l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ବସ୍ତୁଗୁଡ଼ିକ[1]) + 1;
		    ଯଦି (compflags != ଖାଲି)
			l += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(compflags) + 1;
		    p = getroom(spin, l, FALSE);
		    ଯଦି (p != ଖାଲି)
		    {
			ଯଦି (compflags != ଖାଲି)
			{
			    STRCPY(p, compflags);
			    STRCAT(p, "/");
			}
			STRCAT(p, ବସ୍ତୁଗୁଡ଼ିକ[1]);
			compflags = p;
		    }
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "COMPOUNDWORDMAX", 2)
							      && compmax == 0)
	    {
		compmax = atoi((ଅକ୍ଷର *)ବସ୍ତୁଗୁଡ଼ିକ[1]);
		ଯଦି (compmax == 0)
		    smsg(_("Wrong COMPOUNDWORDMAX value in %s line %d: %s"),
						       fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[1]);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "COMPOUNDMIN", 2)
							   && compminlen == 0)
	    {
		compminlen = atoi((ଅକ୍ଷର *)ବସ୍ତୁଗୁଡ଼ିକ[1]);
		ଯଦି (compminlen == 0)
		    smsg(_("Wrong COMPOUNDMIN value in %s line %d: %s"),
						       fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[1]);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "COMPOUNDSYLMAX", 2)
							   && compsylmax == 0)
	    {
		compsylmax = atoi((ଅକ୍ଷର *)ବସ୍ତୁଗୁଡ଼ିକ[1]);
		ଯଦି (compsylmax == 0)
		    smsg(_("Wrong COMPOUNDSYLMAX value in %s line %d: %s"),
						       fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[1]);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "CHECKCOMPOUNDDUP", 1))
	    {
		compoptions |= COMP_CHECKDUP;
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "CHECKCOMPOUNDREP", 1))
	    {
		compoptions |= COMP_CHECKREP;
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "CHECKCOMPOUNDCASE", 1))
	    {
		compoptions |= COMP_CHECKCASE;
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "CHECKCOMPOUNDTRIPLE", 1))
	    {
		compoptions |= COMP_CHECKTRIPLE;
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "CHECKCOMPOUNDPATTERN", 2))
	    {
		ଯଦି (atoi((ଅକ୍ଷର *)ବସ୍ତୁଗୁଡ଼ିକ[1]) == 0)
		    smsg(_("Wrong CHECKCOMPOUNDPATTERN value in %s line %d: %s"),
						       fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[1]);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "CHECKCOMPOUNDPATTERN", 3))
	    {
		garray_T    *gap = &spin->si_comppat;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

		// Only add the couple if it isn't already there.
		ପାଇଁ (i = 0; i < gap->ga_len - 1; i += 2)
		    ଯଦି (STRCMP(((char_u **)(gap->ga_data))[i], ବସ୍ତୁଗୁଡ଼ିକ[1]) == 0
			    && STRCMP(((char_u **)(gap->ga_data))[i + 1],
							       ବସ୍ତୁଗୁଡ଼ିକ[2]) == 0)
			ଭାଙ୍ଗ;
		ଯଦି (i >= gap->ga_len && ga_grow(gap, 2) == OK)
		{
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, ବସ୍ତୁଗୁଡ଼ିକ[1]);
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, ବସ୍ତୁଗୁଡ଼ିକ[2]);
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "SYLLABLE", 2)
							  && syllable == ଖାଲି)
	    {
		syllable = getroom_save(spin, ବସ୍ତୁଗୁଡ଼ିକ[1]);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "NOBREAK", 1))
	    {
		spin->si_nobreak = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "NOSPLITSUGS", 1))
	    {
		spin->si_nosplitsugs = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "NOCOMPOUNDSUGS", 1))
	    {
		spin->si_nocompoundsugs = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "NOSUGFILE", 1))
	    {
		spin->si_nosugfile = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "PFXPOSTPONE", 1))
	    {
		aff->af_pfxpostpone = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "IGNOREEXTRA", 1))
	    {
		aff->af_ignoreextra = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି ((STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[0], "PFX") == 0
					      || STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[0], "SFX") == 0)
		    && aff_todo == 0
		    && itemcnt >= 4)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	lasti = 4;
		char_u	ଚାବି[AH_ଚାବି_LEN];

		ଯଦି (*ବସ୍ତୁଗୁଡ଼ିକ[0] == 'P')
		    tp = &aff->af_pref;
		ଅନ୍ୟଥା
		    tp = &aff->af_suff;

		// Myspell allows the same affix name to be used multiple
		// times.  The affix files that do this have an undocumented
		// "S" flag on all but the last block, thus we check for that
		// and store it in ah_follows.
		vim_strncpy(ଚାବି, ବସ୍ତୁଗୁଡ଼ିକ[1], AH_ଚାବି_LEN - 1);
		hi = hash_find(tp, ଚାବି);
		ଯଦି (!HASHITEM_EMPTY(hi))
		{
		    cur_aff = HI2AH(hi);
		    ଯଦି (cur_aff->ah_combine != (*ବସ୍ତୁଗୁଡ଼ିକ[2] == 'Y'))
			smsg(_("Different combining flag in continued affix block in %s line %d: %s"),
						   fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[1]);
		    ଯଦି (!cur_aff->ah_follows)
			smsg(_("Duplicate affix in %s line %d: %s"),
						       fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[1]);
		}
		ଅନ୍ୟଥା
		{
		    // New affix letter.
		    cur_aff = (affheader_T *)getroom(spin,
						   ଆକାର(affheader_T), TRUE);
		    ଯଦି (cur_aff == ଖାଲି)
			ଭାଙ୍ଗ;
		    cur_aff->ah_flag = affitem2flag(aff->af_flagtype, ବସ୍ତୁଗୁଡ଼ିକ[1],
								 fname, lnum);
		    ଯଦି (cur_aff->ah_flag == 0 || STRLEN(ବସ୍ତୁଗୁଡ଼ିକ[1]) >= AH_ଚାବି_LEN)
			ଭାଙ୍ଗ;
		    ଯଦି (cur_aff->ah_flag == aff->af_bad
			    || cur_aff->ah_flag == aff->af_rare
			    || cur_aff->ah_flag == aff->af_keepcase
			    || cur_aff->ah_flag == aff->af_needaffix
			    || cur_aff->ah_flag == aff->af_circumfix
			    || cur_aff->ah_flag == aff->af_nosuggest
			    || cur_aff->ah_flag == aff->af_needcomp
			    || cur_aff->ah_flag == aff->af_comproot)
			smsg(_("Affix also used for BAD/RARE/KEEPCASE/NEEDAFFIX/NEEDCOMPOUND/NOSUGGEST in %s line %d: %s"),
						       fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[1]);
		    STRCPY(cur_aff->ah_ଚାବି, ବସ୍ତୁଗୁଡ଼ିକ[1]);
		    hash_add(tp, cur_aff->ah_ଚାବି, "spelling");

		    cur_aff->ah_combine = (*ବସ୍ତୁଗୁଡ଼ିକ[2] == 'Y');
		}

		// Check for the "S" flag, which apparently means that another
		// block with the same affix name is following.
		ଯଦି (itemcnt > lasti && STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[lasti], "S") == 0)
		{
		    ++lasti;
		    cur_aff->ah_follows = TRUE;
		}
		ଅନ୍ୟଥା
		    cur_aff->ah_follows = FALSE;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter.
		ଯଦି (itemcnt > lasti && *ବସ୍ତୁଗୁଡ଼ିକ[lasti] != '#')
		    smsg(_(e_afftrailing), fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[lasti]);

		ଯଦି (STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[2], "Y") != 0 && STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[2], "N") != 0)
		    smsg(_("Expected Y or N in %s line %d: %s"),
						       fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[2]);

		ଯଦି (*ବସ୍ତୁଗୁଡ଼ିକ[0] == 'P' && aff->af_pfxpostpone)
		{
		    ଯଦି (cur_aff->ah_newID == 0)
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
		    ଅନ୍ୟଥା
			// Did use the ID in a previous block.
			did_postpone_prefix = TRUE;
		}

		aff_todo = atoi((ଅକ୍ଷର *)ବସ୍ତୁଗୁଡ଼ିକ[3]);
	    }
	    ଅନ୍ୟଥା ଯଦି ((STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[0], "PFX") == 0
					      || STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[0], "SFX") == 0)
		    && aff_todo > 0
		    && STRCMP(cur_aff->ah_ଚାବି, ବସ୍ତୁଗୁଡ଼ିକ[1]) == 0
		    && itemcnt >= 5)
	    {
		affentry_T	*aff_entry;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		upper = FALSE;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lasti = 5;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter,
		// unless IGNOREEXTRA is used.  Hunspell uses a "-" item.
		ଯଦି (itemcnt > lasti
			&& !aff->af_ignoreextra
			&& *ବସ୍ତୁଗୁଡ଼ିକ[lasti] != '#'
			&& (STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[lasti], "-") != 0
						     || itemcnt != lasti + 1))
		    smsg(_(e_afftrailing), fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[lasti]);

		// New item for an affix letter.
		--aff_todo;
		aff_entry = (affentry_T *)getroom(spin,
						    ଆକାର(affentry_T), TRUE);
		ଯଦି (aff_entry == ଖାଲି)
		    ଭାଙ୍ଗ;

		ଯଦି (STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[2], "0") != 0)
		    aff_entry->ae_chop = getroom_save(spin, ବସ୍ତୁଗୁଡ଼ିକ[2]);
		ଯଦି (STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[3], "0") != 0)
		{
		    aff_entry->ae_add = getroom_save(spin, ବସ୍ତୁଗୁଡ଼ିକ[3]);

		    // Recognize flags on the affix: abcd/XYZ
		    aff_entry->ae_flags = vim_strchr(aff_entry->ae_add, '/');
		    ଯଦି (aff_entry->ae_flags != ଖାଲି)
		    {
			*aff_entry->ae_flags++ = NUL;
			aff_process_flags(aff, aff_entry);
		    }
		}

		// Don't use an affix entry with non-ASCII characters when
		// "spin->si_ascii" is TRUE.
		ଯଦି (!spin->si_ascii || !(has_non_ascii(aff_entry->ae_chop)
					  || has_non_ascii(aff_entry->ae_add)))
		{
		    aff_entry->ae_next = cur_aff->ah_first;
		    cur_aff->ah_first = aff_entry;

		    ଯଦି (STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[4], ".") != 0)
		    {
			char_u	buf[MAXLINELEN];

			aff_entry->ae_cond = getroom_save(spin, ବସ୍ତୁଗୁଡ଼ିକ[4]);
			// Note: this silently truncates the buffer, but this should
			// not happen in practice
			ଯଦି (*ବସ୍ତୁଗୁଡ଼ିକ[0] == 'P')
			    vim_snprintf((ଅକ୍ଷର *)buf, ଆକାର(buf), "^%s", ବସ୍ତୁଗୁଡ଼ିକ[4]);
			ଅନ୍ୟଥା
			    vim_snprintf((ଅକ୍ଷର *)buf, ଆକାର(buf), "%s$", ବସ୍ତୁଗୁଡ଼ିକ[4]);
			aff_entry->ae_prog = vim_regcomp(buf,
					    RE_MAGIC + RE_STRING + RE_STRICT);
			ଯଦି (aff_entry->ae_prog == ଖାଲି)
			    smsg(_("Broken condition in %s line %d: %s"),
						       fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[4]);
		    }

		    // For postponed prefixes we need an entry in si_prefcond
		    // for the condition.  Use an existing one if possible.
		    // Can't be done for an affix with flags, ignoring
		    // COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG.
		    ଯଦି (*ବସ୍ତୁଗୁଡ଼ିକ[0] == 'P' && aff->af_pfxpostpone
					       && aff_entry->ae_flags == ଖାଲି)
		    {
			// When the chop string is one lower-case letter and
			// the add string ends in the upper-case letter we set
			// the "upper" flag, clear "ae_chop" and remove the
			// letters from "ae_add".  The condition must either
			// be empty or start with the same letter.
			ଯଦି (aff_entry->ae_chop != ଖାଲି
				&& aff_entry->ae_add != ଖାଲି
				&& aff_entry->ae_chop[(*mb_ptr2len)(
						   aff_entry->ae_chop)] == NUL)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c, c_up;

			    c = PTR2CHAR(aff_entry->ae_chop);
			    c_up = SPELL_TOUPPER(c);
			    ଯଦି (c_up != c
				    && (aff_entry->ae_cond == ଖାଲି
					|| PTR2CHAR(aff_entry->ae_cond) == c))
			    {
				p = aff_entry->ae_add
						  + STRLEN(aff_entry->ae_add);
				MB_PTR_BACK(aff_entry->ae_add, p);
				ଯଦି (PTR2CHAR(p) == c_up)
				{
				    upper = TRUE;
				    aff_entry->ae_chop = ଖାଲି;
				    *p = NUL;

				    // The condition is matched with the
				    // actual word, thus must check for the
				    // upper-case letter.
				    ଯଦି (aff_entry->ae_cond != ଖାଲି)
				    {
					char_u	buf[MAXLINELEN];

					ଯଦି (has_mbyte)
					{
					    onecap_copy(ବସ୍ତୁଗୁଡ଼ିକ[4], buf, TRUE);
					    aff_entry->ae_cond = getroom_save(
								   spin, buf);
					}
					ଅନ୍ୟଥା
					    *aff_entry->ae_cond = c_up;
					ଯଦି (aff_entry->ae_cond != ଖାଲି)
					{
					    sprintf((ଅକ୍ଷର *)buf, "^%s",
							  aff_entry->ae_cond);
					    vim_regfree(aff_entry->ae_prog);
					    aff_entry->ae_prog = vim_regcomp(
						    buf, RE_MAGIC + RE_STRING);
					}
				    }
				}
			    }
			}

			ଯଦି (aff_entry->ae_chop == ଖାଲି
					       && aff_entry->ae_flags == ଖାଲି)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
			    char_u	**pp;
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

			    // Find a previously used condition.
			    ପାଇଁ (idx = spin->si_prefcond.ga_len - 1; idx >= 0;
									--idx)
			    {
				p = ((char_u **)spin->si_prefcond.ga_data)[idx];
				ଯଦି (str_equal(p, aff_entry->ae_cond))
				    ଭାଙ୍ଗ;
			    }
			    ଯଦି (idx < 0 && ga_grow(&spin->si_prefcond, 1) == OK)
			    {
				// Not found, add a new condition.
				idx = spin->si_prefcond.ga_len++;
				pp = ((char_u **)spin->si_prefcond.ga_data)
									+ idx;
				ଯଦି (aff_entry->ae_cond == ଖାଲି)
				    *pp = ଖାଲି;
				ଅନ୍ୟଥା
				    *pp = getroom_save(spin,
							  aff_entry->ae_cond);
			    }

			    // Add the prefix to the prefix tree.
			    ଯଦି (aff_entry->ae_add == ଖାଲି)
				p = (char_u *)"";
			    ଅନ୍ୟଥା
				p = aff_entry->ae_add;

			    // PFX_FLAGS is a negative number, so that
			    // tree_add_word() knows this is the prefix tree.
			    n = PFX_FLAGS;
			    ଯଦି (!cur_aff->ah_combine)
				n |= WFP_NC;
			    ଯଦି (upper)
				n |= WFP_UP;
			    ଯଦି (aff_entry->ae_comppermit)
				n |= WFP_COMPPERMIT;
			    ଯଦି (aff_entry->ae_compforbid)
				n |= WFP_COMPFORBID;
			    ବୃକ୍ଷ_add_word(spin, p, spin->si_prefroot, n,
						      idx, cur_aff->ah_newID);
			    did_postpone_prefix = TRUE;
			}

			// Didn't actually use ah_newID, backup si_newprefID.
			ଯଦି (aff_todo == 0 && !did_postpone_prefix)
			{
			    --spin->si_newprefID;
			    cur_aff->ah_newID = 0;
			}
		    }
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "FOL", 2) && fol == ଖାଲି)
	    {
		fol = vim_strsave(ବସ୍ତୁଗୁଡ଼ିକ[1]);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "LOW", 2) && ତଳ == ଖାଲି)
	    {
		ତଳ = vim_strsave(ବସ୍ତୁଗୁଡ଼ିକ[1]);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "UPP", 2) && upp == ଖାଲି)
	    {
		upp = vim_strsave(ବସ୍ତୁଗୁଡ଼ିକ[1]);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "REP", 2)
		     || is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "REPSAL", 2))
	    {
		// Ignore REP/REPSAL count
		ଯଦି (!SAFE_isdigit(*ବସ୍ତୁଗୁଡ଼ିକ[1]))
		    smsg(_("Expected REP(SAL) count in %s line %d"),
								 fname, lnum);
	    }
	    ଅନ୍ୟଥା ଯଦି ((STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[0], "REP") == 0
			|| STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[0], "REPSAL") == 0)
		    && itemcnt >= 3)
	    {
		// REP/REPSAL item
		// Myspell ignores extra arguments, we require it starts with
		// # to detect mistakes.
		ଯଦି (itemcnt > 3 && ବସ୍ତୁଗୁଡ଼ିକ[3][0] != '#')
		    smsg(_(e_afftrailing), fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[3]);
		ଯଦି (ବସ୍ତୁଗୁଡ଼ିକ[0][3] == 'S' ? do_repsal : do_rep)
		{
		    // Replace underscore with space (can't include a space
		    // directly).
		    ପାଇଁ (p = ବସ୍ତୁଗୁଡ଼ିକ[1]; *p != NUL; MB_PTR_ADV(p))
			ଯଦି (*p == '_')
			    *p = ' ';
		    ପାଇଁ (p = ବସ୍ତୁଗୁଡ଼ିକ[2]; *p != NUL; MB_PTR_ADV(p))
			ଯଦି (*p == '_')
			    *p = ' ';
		    add_fromto(spin, ବସ୍ତୁଗୁଡ଼ିକ[0][3] == 'S'
					 ? &spin->si_repsal
					 : &spin->si_rep, ବସ୍ତୁଗୁଡ଼ିକ[1], ବସ୍ତୁଗୁଡ଼ିକ[2]);
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "MAP", 2))
	    {
		// MAP item or count
		ଯଦି (!found_map)
		{
		    // First line contains the count.
		    found_map = TRUE;
		    ଯଦି (!SAFE_isdigit(*ବସ୍ତୁଗୁଡ଼ିକ[1]))
			smsg(_("Expected MAP count in %s line %d"),
								 fname, lnum);
		}
		ଅନ୍ୟଥା ଯଦି (do_mapline)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

		    // Check that every character appears only once.
		    ପାଇଁ (p = ବସ୍ତୁଗୁଡ଼ିକ[1]; *p != NUL; )
		    {
			c = mb_ptr2char_adv(&p);
			ଯଦି ((spin->si_map.ga_len > 0
				    && vim_strchr(spin->si_map.ga_data, c)
								      != ଖାଲି)
				|| vim_strchr(p, c) != ଖାଲି)
			    smsg(_("Duplicate character in MAP in %s line %d"),
								 fname, lnum);
		    }

		    // We simply concatenate all the MAP strings, separated by
		    // slashes.
		    ga_concat(&spin->si_map, ବସ୍ତୁଗୁଡ଼ିକ[1]);
		    ga_append(&spin->si_map, '/');
		}
	    }
	    // Accept "SAL from to" and "SAL from to  #comment".
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "SAL", 3))
	    {
		ଯଦି (do_sal)
		{
		    // SAL item (sounds-a-like)
		    // Either one of the known keys or a from-to pair.
		    ଯଦି (STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[1], "followup") == 0)
			spin->si_followup = sal_to_bool(ବସ୍ତୁଗୁଡ଼ିକ[2]);
		    ଅନ୍ୟଥା ଯଦି (STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[1], "collapse_result") == 0)
			spin->si_collapse = sal_to_bool(ବସ୍ତୁଗୁଡ଼ିକ[2]);
		    ଅନ୍ୟଥା ଯଦି (STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[1], "remove_accents") == 0)
			spin->si_rem_accents = sal_to_bool(ବସ୍ତୁଗୁଡ଼ିକ[2]);
		    ଅନ୍ୟଥା
			// when "to" is "_" it means empty
			add_fromto(spin, &spin->si_sal, ବସ୍ତୁଗୁଡ଼ିକ[1],
				     STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[2], "_") == 0 ? (char_u *)""
								: ବସ୍ତୁଗୁଡ଼ିକ[2]);
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "SOFOFROM", 2)
							  && sofofrom == ଖାଲି)
	    {
		sofofrom = getroom_save(spin, ବସ୍ତୁଗୁଡ଼ିକ[1]);
	    }
	    ଅନ୍ୟଥା ଯଦି (is_aff_rule(ବସ୍ତୁଗୁଡ଼ିକ, itemcnt, "SOFOTO", 2)
							    && sofoto == ଖାଲି)
	    {
		sofoto = getroom_save(spin, ବସ୍ତୁଗୁଡ଼ିକ[1]);
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[0], "COMMON") == 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

		ପାଇଁ (i = 1; i < itemcnt; ++i)
		{
		    ଯଦି (HASHITEM_EMPTY(hash_find(&spin->si_commonwords,
								   ବସ୍ତୁଗୁଡ଼ିକ[i])))
		    {
			p = vim_strsave(ବସ୍ତୁଗୁଡ଼ିକ[i]);
			ଯଦି (p == ଖାଲି)
			    ଭାଙ୍ଗ;
			hash_add(&spin->si_commonwords, p, "spelling");
		    }
		}
	    }
	    ଅନ୍ୟଥା
		smsg(_("Unrecognized or duplicate item in %s line %d: %s"),
						       fname, lnum, ବସ୍ତୁଗୁଡ଼ିକ[0]);
	}
    }

    ଯଦି (fol != ଖାଲି || ତଳ != ଖାଲି || upp != ଖାଲି)
    {
	ଯଦି (spin->si_clear_chartab)
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
	ଯଦି (!spin->si_ascii && !enc_utf8)
	{
	    ଯଦି (fol == ଖାଲି || ତଳ == ଖାଲି || upp == ଖାଲି)
		smsg(_("Missing FOL/LOW/UPP line in %s"), fname);
	    ଅନ୍ୟଥା
		(ଶୂନ୍ୟ)set_spell_chartab(fol, ତଳ, upp);
	}

	vim_free(fol);
	vim_free(ତଳ);
	vim_free(upp);
    }

    // Use compound specifications of the .aff file for the spell info.
    ଯଦି (compmax != 0)
    {
	aff_check_ସଂଖ୍ୟା(spin->si_compmax, compmax, "COMPOUNDWORDMAX");
	spin->si_compmax = compmax;
    }

    ଯଦି (compminlen != 0)
    {
	aff_check_ସଂଖ୍ୟା(spin->si_compminlen, compminlen, "COMPOUNDMIN");
	spin->si_compminlen = compminlen;
    }

    ଯଦି (compsylmax != 0)
    {
	ଯଦି (syllable == ଖାଲି)
	    smsg(_("COMPOUNDSYLMAX used without SYLLABLE"));
	aff_check_ସଂଖ୍ୟା(spin->si_compsylmax, compsylmax, "COMPOUNDSYLMAX");
	spin->si_compsylmax = compsylmax;
    }

    ଯଦି (compoptions != 0)
    {
	aff_check_ସଂଖ୍ୟା(spin->si_compoptions, compoptions, "COMPOUND options");
	spin->si_compoptions |= compoptions;
    }

    ଯଦି (compflags != ଖାଲି)
	process_compflags(spin, aff, compflags);

    // Check that we didn't use too many renumbered flags.
    ଯଦି (spin->si_newcompID < spin->si_newprefID)
    {
	ଯଦି (spin->si_newcompID == 127 || spin->si_newcompID == 255)
	    ସନ୍ଦେଶ(_("Too many postponed prefixes"));
	ଅନ୍ୟଥା ଯଦି (spin->si_newprefID == 0 || spin->si_newprefID == 127)
	    ସନ୍ଦେଶ(_("Too many compound flags"));
	ଅନ୍ୟଥା
	    ସନ୍ଦେଶ(_("Too many postponed prefixes and/or compound flags"));
    }

    ଯଦି (syllable != ଖାଲି)
    {
	aff_check_string(spin->si_syllable, syllable, "SYLLABLE");
	spin->si_syllable = syllable;
    }

    ଯଦି (sofofrom != ଖାଲି || sofoto != ଖାଲି)
    {
	ଯଦି (sofofrom == ଖାଲି || sofoto == ଖାଲି)
	    smsg(_("Missing SOFO%s line in %s"),
				     sofofrom == ଖାଲି ? "FROM" : "TO", fname);
	ଅନ୍ୟଥା ଯଦି (spin->si_sal.ga_len > 0)
	    smsg(_("Both SAL and SOFO lines in %s"), fname);
	ଅନ୍ୟଥା
	{
	    aff_check_string(spin->si_sofofr, sofofrom, "SOFOFROM");
	    aff_check_string(spin->si_sofoto, sofoto, "SOFOTO");
	    spin->si_sofofr = sofofrom;
	    spin->si_sofoto = sofoto;
	}
    }

    ଯଦି (midword != ଖାଲି)
    {
	aff_check_string(spin->si_midword, midword, "MIDWORD");
	spin->si_midword = midword;
    }

    vim_free(pc);
    fclose(fd);
    ଫେରନ୍ତୁ aff;
}

/*
 * Return TRUE when items[0] equals "rulename", there are "mincount" items or
 * a comment is following after item "mincount".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_aff_rule(
    char_u	**ବସ୍ତୁଗୁଡ଼ିକ,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		itemcnt,
    ଅକ୍ଷର	*rulename,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mincount)
{
    ଫେରନ୍ତୁ (STRCMP(ବସ୍ତୁଗୁଡ଼ିକ[0], rulename) == 0
	    && (itemcnt == mincount
		|| (itemcnt > mincount && ବସ୍ତୁଗୁଡ଼ିକ[mincount][0] == '#')));
}

/*
 * For affix "entry" move COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG from
 * ae_flags to ae_comppermit and ae_compforbid.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
aff_process_flags(afffile_T *affile, affentry_T *entry)
{
    char_u	*p;
    char_u	*prevp;
    unsigned	flag;

    ଯଦି (entry->ae_flags != ଖାଲି
		&& (affile->af_compforbid != 0 || affile->af_comppermit != 0))
    {
	ପାଇଁ (p = entry->ae_flags; *p != NUL; )
	{
	    prevp = p;
	    flag = get_affitem(affile->af_flagtype, &p);
	    ଯଦି (flag == affile->af_comppermit || flag == affile->af_compforbid)
	    {
		STRMOVE(prevp, p);
		p = prevp;
		ଯଦି (flag == affile->af_comppermit)
		    entry->ae_comppermit = TRUE;
		ଅନ୍ୟଥା
		    entry->ae_compforbid = TRUE;
	    }
	    ଯଦି (affile->af_flagtype == AFT_ଅଙ୍କ && *p == ',')
		++p;
	}
	ଯଦି (*entry->ae_flags == NUL)
	    entry->ae_flags = ଖାଲି;	// nothing left
    }
}

/*
 * Return TRUE if "s" is the name of an info item in the affix file.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
spell_info_item(char_u *s)
{
    ଫେରନ୍ତୁ STRCMP(s, "NAME") == 0
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
    ସ୍ଥିର unsigned
affitem2flag(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flagtype,
    char_u	*item,
    char_u	*fname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lnum)
{
    unsigned	res;
    char_u	*p = item;

    res = get_affitem(flagtype, &p);
    ଯଦି (res == 0)
    {
	ଯଦି (flagtype == AFT_ଅଙ୍କ)
	    smsg(_("Flag is not a number in %s line %d: %s"),
							   fname, lnum, item);
	ଅନ୍ୟଥା
	    smsg(_("Illegal flag in %s line %d: %s"),
							   fname, lnum, item);
    }
    ଯଦି (*p != NUL)
    {
	smsg(_(e_affname), fname, lnum, item);
	ଫେରନ୍ତୁ 0;
    }

    ଫେରନ୍ତୁ res;
}

/*
 * Get one affix name from "*pp" and advance the pointer.
 * Returns ZERO_FLAG for "0".
 * Returns zero for an error, still advances the pointer then.
 */
    ସ୍ଥିର unsigned
get_affitem(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flagtype, char_u **pp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;

    ଯଦି (flagtype == AFT_ଅଙ୍କ)
    {
	ଯଦି (!VIM_ISDIGIT(**pp))
	{
	    ++*pp;	// always advance, avoid getting stuck
	    ଫେରନ୍ତୁ 0;
	}
	res = getdigits(pp);
	ଯଦି (res == 0)
	    res = ZERO_FLAG;
    }
    ଅନ୍ୟଥା
    {
	res = mb_ptr2char_adv(pp);
	ଯଦି (flagtype == AFT_LONG || (flagtype == AFT_CAPLONG
						 && res >= 'A' && res <= 'Z'))
	{
	    ଯଦି (**pp == NUL)
		ଫେରନ୍ତୁ 0;
	    res = mb_ptr2char_adv(pp) + (res << 16);
	}
    }
    ଫେରନ୍ତୁ res;
}

/*
 * Process the "compflags" string used in an affix file and append it to
 * spin->si_compflags.
 * The processing involves changing the affix names to ID numbers, so that
 * they fit in one byte.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
process_compflags(
    spellinfo_T	*spin,
    afffile_T	*aff,
    char_u	*compflags)
{
    char_u	*p;
    char_u	*prevp;
    unsigned	flag;
    compitem_T	*ci;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*tp;
    char_u	ଚାବି[AH_ଚାବି_LEN];
    hashitem_T	*hi;

    // Make room for the old and the new compflags, concatenated with a / in
    // between.  Processing it makes it shorter, but we don't know by how
    // much, thus allocate the maximum.
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(compflags) + 1;
    ଯଦି (spin->si_compflags != ଖାଲି)
	len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(spin->si_compflags) + 1;
    p = getroom(spin, len, FALSE);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (spin->si_compflags != ଖାଲି)
    {
	STRCPY(p, spin->si_compflags);
	STRCAT(p, "/");
    }
    spin->si_compflags = p;
    tp = p + STRLEN(p);

    ପାଇଁ (p = compflags; *p != NUL; )
    {
	ଯଦି (vim_strchr((char_u *)"/?*+[]", *p) != ଖାଲି)
	    // Copy non-flag characters directly.
	    *tp++ = *p++;
	ଅନ୍ୟଥା
	{
	    // First get the flag number, also checks validity.
	    prevp = p;
	    flag = get_affitem(aff->af_flagtype, &p);
	    ଯଦି (flag != 0)
	    {
		// Find the flag in the hashtable.  If it was used before, use
		// the existing ID.  Otherwise add a new entry.
		vim_strncpy(ଚାବି, prevp, p - prevp);
		hi = hash_find(&aff->af_comp, ଚାବି);
		ଯଦି (!HASHITEM_EMPTY(hi))
		    id = HI2CI(hi)->ci_newID;
		ଅନ୍ୟଥା
		{
		    ci = (compitem_T *)getroom(spin, ଆକାର(compitem_T), TRUE);
		    ଯଦି (ci == ଖାଲି)
			ଭାଙ୍ଗ;
		    STRCPY(ci->ci_ଚାବି, ଚାବି);
		    ci->ci_flag = flag;
		    // Avoid using a flag ID that has a special meaning in a
		    // regexp (also inside []).
		    କର
		    {
			check_renumber(spin);
			id = spin->si_newcompID--;
		    } ଯେତେବେଳେ (vim_strchr((char_u *)"/?*+[]\\-^", id) != ଖାଲି);
		    ci->ci_newID = id;
		    hash_add(&aff->af_comp, ci->ci_ଚାବି, "spelling");
		}
		*tp++ = id;
	    }
	    ଯଦି (aff->af_flagtype == AFT_ଅଙ୍କ && *p == ',')
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
    ସ୍ଥିର ଶୂନ୍ୟ
check_renumber(spellinfo_T *spin)
{
    ଯଦି (spin->si_newprefID == spin->si_newcompID && spin->si_newcompID < 128)
    {
	spin->si_newprefID = 127;
	spin->si_newcompID = 255;
    }
}

/*
 * Append one affix or compound ID to "store_afflist".
 * Returns FAIL when this would overrun the fixed-size buffer.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
store_afflist_add(
    char_u	*store_afflist,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*cntp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id)
{
    ଯଦି (*cntp >= MAXWLEN - 1)
    {
	emsg(_(e_too_many_postponed_prefixes_spell));
	ଫେରନ୍ତୁ FAIL;
    }
    store_afflist[(*cntp)++] = id;
    store_afflist[*cntp] = NUL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Return TRUE if flag "flag" appears in affix list "afflist".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
flag_in_afflist(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flagtype, char_u *afflist, unsigned flag)
{
    char_u	*p;
    unsigned	n;

    ବାଛ (flagtype)
    {
	ସ୍ଥିତି AFT_CHAR:
	    ଫେରନ୍ତୁ vim_strchr(afflist, flag) != ଖାଲି;

	ସ୍ଥିତି AFT_CAPLONG:
	ସ୍ଥିତି AFT_LONG:
	    ପାଇଁ (p = afflist; *p != NUL; )
	    {
		n = mb_ptr2char_adv(&p);
		ଯଦି ((flagtype == AFT_LONG || (n >= 'A' && n <= 'Z'))
								 && *p != NUL)
		    n = mb_ptr2char_adv(&p) + (n << 16);
		ଯଦି (n == flag)
		    ଫେରନ୍ତୁ TRUE;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି AFT_ଅଙ୍କ:
	    ପାଇଁ (p = afflist; *p != NUL; )
	    {
		n = getdigits(&p);
		ଯଦି (n == 0)
		    n = ZERO_FLAG;
		ଯଦି (n == flag)
		    ଫେରନ୍ତୁ TRUE;
		ଯଦି (*p != NUL)	// skip over comma
		    ++p;
	    }
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Give a warning when "spinval" and "affval" numbers are set and not the same.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
aff_check_ସଂଖ୍ୟା(ପୂର୍ଣ୍ଣସଂଖ୍ୟା spinval, ପୂର୍ଣ୍ଣସଂଖ୍ୟା affval, ଅକ୍ଷର *name)
{
    ଯଦି (spinval != 0 && spinval != affval)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Give a warning when "spinval" and "affval" strings are set and not the same.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
aff_check_string(char_u *spinval, char_u *affval, ଅକ୍ଷର *name)
{
    ଯଦି (spinval != ଖାଲି && STRCMP(spinval, affval) != 0)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Return TRUE if strings "s1" and "s2" are equal.  Also consider both being
 * NULL as equal.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
str_equal(char_u *s1, char_u *s2)
{
    ଯଦି (s1 == ଖାଲି || s2 == ଖାଲି)
	ଫେରନ୍ତୁ s1 == s2;
    ଫେରନ୍ତୁ STRCMP(s1, s2) == 0;
}

/*
 * Add a from-to item to "gap".  Used for REP and SAL items.
 * They are stored case-folded.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
add_fromto(
    spellinfo_T	*spin,
    garray_T	*gap,
    char_u	*from,
    char_u	*to)
{
    fromto_T	*ftp;
    char_u	word[MAXWLEN];

    ଯଦି (ga_grow(gap, 1) == FAIL)
	ଫେରନ୍ତୁ;

    ftp = ((fromto_T *)gap->ga_data) + gap->ga_len;
    (ଶୂନ୍ୟ)spell_casefold(curwin, from, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(from), word, MAXWLEN);
    ftp->ft_from = getroom_save(spin, word);
    (ଶୂନ୍ୟ)spell_casefold(curwin, to, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(to), word, MAXWLEN);
    ftp->ft_to = getroom_save(spin, word);
    ++gap->ga_len;
}

/*
 * Convert a boolean argument in a SAL line to TRUE or FALSE;
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sal_to_bool(char_u *s)
{
    ଫେରନ୍ତୁ STRCMP(s, "1") == 0 || STRCMP(s, "true") == 0;
}

/*
 * Free the structure filled by spell_read_aff().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
spell_free_aff(afffile_T *aff)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    affheader_T	*ah;
    affentry_T	*ae;

    vim_free(aff->af_enc);

    // All this trouble to free the "ae_prog" items...
    ପାଇଁ (ht = &aff->af_pref; ; ht = &aff->af_suff)
    {
	todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ht->ht_used;
	FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(ht, hi, todo)
	{
	    ଯଦି (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		ah = HI2AH(hi);
		ପାଇଁ (ae = ah->ah_first; ae != ଖାଲି; ae = ae->ae_next)
		    vim_regfree(ae->ae_prog);
	    }
	}
	ଯଦି (ht == &aff->af_suff)
	    ଭାଙ୍ଗ;
    }

    hash_clear(&aff->af_pref);
    hash_clear(&aff->af_suff);
    hash_clear(&aff->af_comp);
}

/*
 * Read dictionary file "fname".
 * Returns OK or FAIL;
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
spell_read_dic(spellinfo_T *spin, char_u *fname, afffile_T *affile)
{
    hashtab_T	ht;
    char_u	ଧାଡ଼ି[MAXLINELEN];
    char_u	*p;
    char_u	*afflist;
    char_u	store_afflist[MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pfxlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		totlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_affix;
    char_u	*dw;
    char_u	*pc;
    char_u	*w;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
    hash_T	hash;
    hashitem_T	*hi;
    FILE	*fd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lnum = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		non_ascii = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
    char_u	message[MAXLINELEN + MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		duplicate = 0;
    time_T	last_ସନ୍ଦେଶ_time = 0;

    /*
     * Open the file.
     */
    fd = mch_fopen((ଅକ୍ଷର *)fname, "r");
    ଯଦି (fd == ଖାଲି)
    {
	semsg(_(e_cant_open_file_str), fname);
	ଫେରନ୍ତୁ FAIL;
    }

    // The hashtable is only used to detect duplicated words.
    hash_init(&ht);

    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
				  _("Reading dictionary file %s..."), fname);
    spell_message(spin, IObuff);

    // start with a message for the first line
    spin->si_ସନ୍ଦେଶ_ଗଣନା = 999999;

    // Read and ignore the first line: word count.
    ଯଦି (vim_fgets(ଧାଡ଼ି, MAXLINELEN, fd) || !vim_isdigit(*skipwhite(ଧାଡ଼ି)))
	semsg(_(e_no_word_ଗଣନା_in_str), fname);

    /*
     * Read all the lines in the file one by one.
     * The words are converted to 'encoding' here, before being added to
     * the hashtable.
     */
    ଯେତେବେଳେ (!vim_fgets(ଧାଡ଼ି, MAXLINELEN, fd) && !got_int)
    {
	ଧାଡ଼ି_breakcheck();
	++lnum;
	ଯଦି (ଧାଡ଼ି[0] == '#' || ଧାଡ଼ି[0] == '/')
	    ଜାରି;	// comment line

	// Remove CR, LF and white space from the end.  White space halfway
	// the word is kept to allow e.g., "et al.".
	l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ଧାଡ଼ି);
	ଯେତେବେଳେ (l > 0 && ଧାଡ଼ି[l - 1] <= ' ')
	    --l;
	ଯଦି (l == 0)
	    ଜାରି;	// empty line
	ଧାଡ଼ି[l] = NUL;

	// Convert from "SET" to 'encoding' when needed.
	ଯଦି (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, ଧାଡ଼ି, ଖାଲି);
	    ଯଦି (pc == ଖାଲି)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
						       fname, lnum, ଧାଡ଼ି);
		ଜାରି;
	    }
	    w = pc;
	}
	ଅନ୍ୟଥା
	{
	    pc = ଖାଲି;
	    w = ଧାଡ଼ି;
	}

	// Truncate the word at the "/", set "afflist" to what follows.
	// Replace "\/" by "/" and "\\" by "\".
	afflist = ଖାଲି;
	ପାଇଁ (p = w; *p != NUL; MB_PTR_ADV(p))
	{
	    ଯଦି (*p == '\\' && (p[1] == '\\' || p[1] == '/'))
		STRMOVE(p, p + 1);
	    ଅନ୍ୟଥା ଯଦି (*p == '/')
	    {
		*p = NUL;
		afflist = p + 1;
		ଭାଙ୍ଗ;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	ଯଦି (spin->si_ascii && has_non_ascii(w))
	{
	    ++non_ascii;
	    vim_free(pc);
	    ଜାରି;
	}

	// This takes time, print a message every 10000 words, but not more
	// often than once per second.
	ଯଦି (spin->si_verbose && spin->si_ସନ୍ଦେଶ_ଗଣନା > 10000)
	{
	    spin->si_ସନ୍ଦେଶ_ଗଣନା = 0;
	    ଯଦି (vim_time() > last_ସନ୍ଦେଶ_time)
	    {
		last_ସନ୍ଦେଶ_time = vim_time();
		vim_snprintf((ଅକ୍ଷର *)message, ଆକାର(message),
			_("line %6d, word %6ld - %s"),
			   lnum, spin->si_foldwcount + spin->si_keepwcount, w);
		ସନ୍ଦେଶ_start();
		ସନ୍ଦେଶ_outtrans_long_attr(message, 0);
		ସନ୍ଦେଶ_clr_eos();
		ସନ୍ଦେଶ_didout = FALSE;
		ସନ୍ଦେଶ_col = 0;
		out_flush();
	    }
	}

	// Store the word in the hashtable to be able to find duplicates.
	dw = getroom_save(spin, w);
	ଯଦି (dw == ଖାଲି)
	{
	    retval = FAIL;
	    vim_free(pc);
	    ଭାଙ୍ଗ;
	}

	hash = hash_hash(dw);
	hi = hash_lookup(&ht, dw, hash);
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    ଯଦି (p_verbose > 0)
		smsg(_("Duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ଅନ୍ୟଥା ଯଦି (duplicate == 0)
		smsg(_("First duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ++duplicate;
	}
	ଅନ୍ୟଥା
	    hash_add_item(&ht, hi, dw, hash);

	flags = 0;
	store_afflist[0] = NUL;
	pfxlen = 0;
	totlen = 0;
	need_affix = FALSE;
	ଯଦି (afflist != ଖାଲି)
	{
	    // Extract flags from the affix list.
	    flags |= get_affix_flags(affile, afflist);

	    ଯଦି (affile->af_needaffix != 0 && flag_in_afflist(
			  affile->af_flagtype, afflist, affile->af_needaffix))
		need_affix = TRUE;

	    ଯଦି (affile->af_pfxpostpone)
	    {
		// Need to store the list of prefix IDs with the word.
		ଯଦି (get_pfxlist(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    ଭାଙ୍ଗ;
		}
		pfxlen = totlen;
	    }

	    ଯଦି (spin->si_compflags != ଖାଲି)
	    {
		// Need to store the list of compound flags with the word.
		// Concatenate them to the list of prefix IDs.
		ଯଦି (get_compflags(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    ଭାଙ୍ଗ;
		}
	    }
	}

	// Add the word to the word tree(s).
	ଯଦି (store_word(spin, dw, flags, spin->si_region,
					   store_afflist, need_affix) == FAIL)
	    retval = FAIL;

	ଯଦି (afflist != ଖାଲି)
	{
	    // Find all matching suffixes and add the resulting words.
	    // Additionally do matching prefixes that combine.
	    ଯଦି (store_aff_word(spin, dw, afflist, affile,
			   &affile->af_suff, &affile->af_pref,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;

	    // Find all matching prefixes and add the resulting words.
	    ଯଦି (store_aff_word(spin, dw, afflist, affile,
			  &affile->af_pref, ଖାଲି,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;
	}

	vim_free(pc);
    }

    ଯଦି (duplicate > 0)
	smsg(_("%d duplicate word(s) in %s"), duplicate, fname);
    ଯଦି (spin->si_ascii && non_ascii > 0)
	smsg(_("Ignored %d word(s) with non-ASCII characters in %s"),
							    non_ascii, fname);
    hash_clear(&ht);

    fclose(fd);
    ଫେରନ୍ତୁ retval;
}

/*
 * Check for affix flags in "afflist" that are turned into word flags.
 * Return WF_ flags.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_affix_flags(afffile_T *affile, char_u *afflist)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = 0;

    ଯଦି (affile->af_keepcase != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_keepcase))
	flags |= WF_KEEPCAP | WF_FIXCAP;
    ଯଦି (affile->af_rare != 0 && flag_in_afflist(
			       affile->af_flagtype, afflist, affile->af_rare))
	flags |= WF_RARE;
    ଯଦି (affile->af_bad != 0 && flag_in_afflist(
				affile->af_flagtype, afflist, affile->af_bad))
	flags |= WF_BANNED;
    ଯଦି (affile->af_needcomp != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_needcomp))
	flags |= WF_NEEDCOMP;
    ଯଦି (affile->af_comproot != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_comproot))
	flags |= WF_COMPROOT;
    ଯଦି (affile->af_nosuggest != 0 && flag_in_afflist(
			  affile->af_flagtype, afflist, affile->af_nosuggest))
	flags |= WF_NOSUGGEST;
    ଫେରନ୍ତୁ flags;
}

/*
 * Get the list of prefix IDs from the affix list "afflist".
 * Used for PFXPOSTPONE.
 * Put the resulting flags in "store_afflist[MAXWLEN]" with a terminating NUL.
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_pfxlist(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    char_u	ଚାବି[AH_ଚାବି_LEN];
    hashitem_T	*hi;

    ପାଇଁ (p = afflist; *p != NUL; )
    {
	prevp = p;
	ଯଦି (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a postponed prefix flag if it appears in "af_pref"
	    // and its ID is not zero.
	    vim_strncpy(ଚାବି, prevp, p - prevp);
	    hi = hash_find(&affile->af_pref, ଚାବି);
	    ଯଦି (!HASHITEM_EMPTY(hi))
	    {
		id = HI2AH(hi)->ah_newID;
		ଯଦି (id != 0 && store_afflist_add(store_afflist, cntp, id) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	}
	ଯଦି (affile->af_flagtype == AFT_ଅଙ୍କ && *p == ',')
	    ++p;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Get the list of compound IDs from the affix list "afflist" that are used
 * for compound words.
 * Puts the flags in "store_afflist[]".
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_compflags(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    char_u	ଚାବି[AH_ଚାବି_LEN];
    hashitem_T	*hi;

    ପାଇଁ (p = afflist; *p != NUL; )
    {
	prevp = p;
	ଯଦି (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a compound flag if it appears in "af_comp".
	    vim_strncpy(ଚାବି, prevp, p - prevp);
	    hi = hash_find(&affile->af_comp, ଚାବି);
	    ଯଦି (!HASHITEM_EMPTY(hi)
		    && store_afflist_add(store_afflist, cntp,
					      HI2CI(hi)->ci_newID) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (affile->af_flagtype == AFT_ଅଙ୍କ && *p == ',')
	    ++p;
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
store_aff_word(
    spellinfo_T	*spin,		// spell info
    char_u	*word,		// basic word start
    char_u	*afflist,	// list of names of supported affixes
    afffile_T	*affile,
    hashtab_T	*ht,
    hashtab_T	*xht,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		condit,		// CONDIT_SUF et al.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,		// flags for the word
    char_u	*pfxlist,	// list of prefix IDs
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pfxlen)		// nr of flags in "pfxlist" for prefixes, rest
				// is compound flags
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    hashitem_T	*hi;
    affheader_T	*ah;
    affentry_T	*ae;
    char_u	newword[MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_flags;
    char_u	*use_pfxlist;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_pfxlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_affix;
    char_u	store_afflist[MAXWLEN];
    char_u	pfx_pfxlist[MAXWLEN];
    size_t	wordlen = STRLEN(word);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_condit;

    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ht->ht_used;
    ପାଇଁ (hi = ht->ht_array; todo > 0 && retval == OK; ++hi)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ah = HI2AH(hi);

	    // Check that the affix combines, if required, and that the word
	    // supports this affix.
	    ଯଦି (((condit & CONDIT_COMB) == 0 || ah->ah_combine)
		    && flag_in_afflist(affile->af_flagtype, afflist,
								 ah->ah_flag))
	    {
		// Loop over all affix entries with this name.
		ପାଇଁ (ae = ah->ah_first; ae != ଖାଲି; ae = ae->ae_next)
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
		    ଯଦି ((xht != ଖାଲି || !affile->af_pfxpostpone
				|| ae->ae_chop != ଖାଲି
				|| ae->ae_flags != ଖାଲି)
			    && (ae->ae_chop == ଖାଲି
				|| STRLEN(ae->ae_chop) < wordlen)
			    && (ae->ae_prog == ଖାଲି
				|| vim_regexec_prog(&ae->ae_prog, FALSE,
							    word, (colnr_T)0))
			    && (((condit & CONDIT_CFIX) == 0)
				== ((condit & CONDIT_AFF) == 0
				    || ae->ae_flags == ଖାଲି
				    || !flag_in_afflist(affile->af_flagtype,
					ae->ae_flags, affile->af_circumfix))))
		    {
			// Match.  Remove the chop and add the affix.
			ଯଦି (xht == ଖାଲି)
			{
			    // prefix: chop/add at the start of the word
			    ଯଦି (ae->ae_add == ଖାଲି)
				*newword = NUL;
			    ଅନ୍ୟଥା
				vim_strncpy(newword, ae->ae_add, MAXWLEN - 1);
			    p = word;
			    ଯଦି (ae->ae_chop != ଖାଲି)
			    {
				// Skip chop string.
				ଯଦି (has_mbyte)
				{
				    i = mb_charlen(ae->ae_chop);
				    ପାଇଁ ( ; i > 0; --i)
					MB_PTR_ADV(p);
				}
				ଅନ୍ୟଥା
				    p += STRLEN(ae->ae_chop);
			    }
			    // Note: this silently truncates the buffer, but this should
			    // not happen in practice
			    STRNCAT(newword, p, MAXWLEN - STRLEN(newword) - 1);
			}
			ଅନ୍ୟଥା
			{
			    // suffix: chop/add at the end of the word
			    vim_strncpy(newword, word, MAXWLEN - 1);
			    ଯଦି (ae->ae_chop != ଖାଲି)
			    {
				// Remove chop string.
				p = newword + STRLEN(newword);
				i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)MB_CHARLEN(ae->ae_chop);
				ପାଇଁ ( ; i > 0; --i)
				    MB_PTR_BACK(newword, p);
				*p = NUL;
			    }
			    ଯଦି (ae->ae_add != ଖାଲି)
				// Note: this silently truncates the buffer, but this should
				// not happen in practice
				STRNCAT(newword, ae->ae_add, MAXWLEN - STRLEN(newword) - 1);
			}

			use_flags = flags;
			use_pfxlist = pfxlist;
			use_pfxlen = pfxlen;
			need_affix = FALSE;
			use_condit = condit | CONDIT_COMB | CONDIT_AFF;
			ଯଦି (ae->ae_flags != ଖାଲି)
			{
			    // Extract flags from the affix list.
			    use_flags |= get_affix_flags(affile, ae->ae_flags);

			    ଯଦି (affile->af_needaffix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_needaffix))
				need_affix = TRUE;

			    // When there is a CIRCUMFIX flag the other affix
			    // must also have it and we don't add the word
			    // with one affix.
			    ଯଦି (affile->af_circumfix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_circumfix))
			    {
				use_condit |= CONDIT_CFIX;
				ଯଦି ((condit & CONDIT_CFIX) == 0)
				    need_affix = TRUE;
			    }

			    ଯଦି (affile->af_pfxpostpone
						|| spin->si_compflags != ଖାଲି)
			    {
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା listlen = 0;

				ଯଦି (affile->af_pfxpostpone)
				{
				    // Get prefix IDS from the affix list.
				    ଯଦି (get_pfxlist(affile, ae->ae_flags,
						    store_afflist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					ଭାଙ୍ଗ;
				    }
				    use_pfxlen = listlen;
				}
				ଅନ୍ୟଥା
				    use_pfxlen = 0;
				use_pfxlist = store_afflist;

				// Combine the prefix IDs. Avoid adding the
				// same ID twice.
				ପାଇଁ (i = 0; i < pfxlen; ++i)
				{
				    ପାଇଁ (j = 0; j < use_pfxlen; ++j)
					ଯଦି (pfxlist[i] == use_pfxlist[j])
					    ଭାଙ୍ଗ;
				    ଯଦି (j == use_pfxlen
					    && store_afflist_add(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					ଭାଙ୍ଗ;
				    }
				    use_pfxlen = listlen;
				}
				ଯଦି (retval == FAIL)
				    ଭାଙ୍ଗ;

				ଯଦି (spin->si_compflags != ଖାଲି)
				    // Get compound IDS from the affix list.
				    ଯଦି (get_compflags(affile, ae->ae_flags,
						      use_pfxlist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					ଭାଙ୍ଗ;
				    }
				ଯଦି (retval == FAIL)
				    ଭାଙ୍ଗ;

				// Combine the list of compound flags.
				// Concatenate them to the prefix IDs list.
				// Avoid adding the same ID twice.
				ପାଇଁ (i = pfxlen; pfxlist[i] != NUL; ++i)
				{
				    ପାଇଁ (j = use_pfxlen;
						   use_pfxlist[j] != NUL; ++j)
					ଯଦି (pfxlist[i] == use_pfxlist[j])
					    ଭାଙ୍ଗ;
				    ଯଦି (use_pfxlist[j] == NUL
					    && store_afflist_add(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					ଭାଙ୍ଗ;
				    }
				}
				ଯଦି (retval == FAIL)
				    ଭାଙ୍ଗ;
			    }
			}

			// Obey a "COMPOUNDFORBIDFLAG" of the affix: don't
			// use the compound flags.
			ଯଦି (use_pfxlist != ଖାଲି && ae->ae_compforbid)
			{
			    vim_strncpy(pfx_pfxlist, use_pfxlist, use_pfxlen);
			    use_pfxlist = pfx_pfxlist;
			}

			// When there are postponed prefixes...
			ଯଦି (spin->si_prefroot != ଖାଲି
				&& spin->si_prefroot->wn_sibling != ଖାଲି)
			{
			    // ... add a flag to indicate an affix was used.
			    use_flags |= WF_HAS_AFF;

			    // ... don't use a prefix list if combining
			    // affixes is not allowed.  But do use the
			    // compound flags after them.
			    ଯଦି (!ah->ah_combine && use_pfxlist != ଖାଲି)
				use_pfxlist += use_pfxlen;
			}

			// When compounding is supported and there is no
			// "COMPOUNDPERMITFLAG" then forbid compounding on the
			// side where the affix is applied.
			ଯଦି (spin->si_compflags != ଖାଲି && !ae->ae_comppermit)
			{
			    ଯଦି (xht != ଖାଲି)
				use_flags |= WF_NOCOMPAFT;
			    ଅନ୍ୟଥା
				use_flags |= WF_NOCOMPBEF;
			}

			// Store the modified word.
			ଯଦି (store_word(spin, newword, use_flags,
						 spin->si_region, use_pfxlist,
							  need_affix) == FAIL)
			    retval = FAIL;

			// When added a prefix or a first suffix and the affix
			// has flags may add a(nother) suffix.  RECURSIVE!
			ଯଦି ((condit & CONDIT_SUF) && ae->ae_flags != ଖାଲି)
			    ଯଦି (store_aff_word(spin, newword, ae->ae_flags,
					affile, &affile->af_suff, xht,
					   use_condit & (xht == ଖାଲି
							? ~0 :  ~CONDIT_SUF),
				      use_flags, use_pfxlist, pfxlen) == FAIL)
				retval = FAIL;

			// When added a suffix and combining is allowed also
			// try adding a prefix additionally.  Both for the
			// word flags and for the affix flags.  RECURSIVE!
			ଯଦି (xht != ଖାଲି && ah->ah_combine)
			{
			    ଯଦି (store_aff_word(spin, newword,
					afflist, affile,
					xht, ଖାଲି, use_condit,
					use_flags, use_pfxlist,
					pfxlen) == FAIL
				    || (ae->ae_flags != ଖାଲି
					&& store_aff_word(spin, newword,
					    ae->ae_flags, affile,
					    xht, ଖାଲି, use_condit,
					    use_flags, use_pfxlist,
					    pfxlen) == FAIL))
				retval = FAIL;
			}
		    }
		}
	    }
	}
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * Read a file with a list of words.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
spell_read_wordfile(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    long	lnum = 0;
    char_u	rline[MAXLINELEN];
    char_u	*ଧାଡ଼ି;
    char_u	*pc = ଖାଲି;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_word = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		non_ascii = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		regionmask;

    /*
     * Open the file.
     */
    fd = mch_fopen((ଅକ୍ଷର *)fname, "r");
    ଯଦି (fd == ଖାଲି)
    {
	semsg(_(e_cant_open_file_str), fname);
	ଫେରନ୍ତୁ FAIL;
    }

    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, _("Reading word file %s..."), fname);
    spell_message(spin, IObuff);

    /*
     * Read all the lines in the file one by one.
     */
    ଯେତେବେଳେ (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	ଧାଡ଼ି_breakcheck();
	++lnum;

	// Skip comment lines.
	ଯଦି (*rline == '#')
	    ଜାରି;

	// Remove CR, LF and white space from the end.
	l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(rline);
	ଯେତେବେଳେ (l > 0 && rline[l - 1] <= ' ')
	    --l;
	ଯଦି (l == 0)
	    ଜାରି;	// empty or blank line
	rline[l] = NUL;

	// Convert from "/encoding={encoding}" to 'encoding' when needed.
	vim_free(pc);
	ଯଦି (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, ଖାଲି);
	    ଯଦି (pc == ଖାଲି)
	    {
		smsg(_("Conversion failure for word in %s line %ld: %s"),
							   fname, lnum, rline);
		ଜାରି;
	    }
	    ଧାଡ଼ି = pc;
	}
	ଅନ୍ୟଥା
	{
	    pc = ଖାଲି;
	    ଧାଡ଼ି = rline;
	}

	ଯଦି (*ଧାଡ଼ି == '/')
	{
	    ++ଧାଡ଼ି;
	    ଯଦି (STRNCMP(ଧାଡ଼ି, "encoding=", 9) == 0)
	    {
		ଯଦି (spin->si_conv.vc_type != CONV_NONE)
		    smsg(_("Duplicate /encoding= line ignored in %s line %ld: %s"),
						       fname, lnum, ଧାଡ଼ି - 1);
		ଅନ୍ୟଥା ଯଦି (did_word)
		    smsg(_("/encoding= line after word ignored in %s line %ld: %s"),
						       fname, lnum, ଧାଡ଼ି - 1);
		ଅନ୍ୟଥା
		{
		    char_u	*enc;

		    // Setup for conversion to 'encoding'.
		    ଧାଡ଼ି += 9;
		    enc = enc_canonize(ଧାଡ଼ି);
		    ଯଦି (enc != ଖାଲି && !spin->si_ascii
			    && convert_setup(&spin->si_conv, enc,
							       p_enc) == FAIL)
			smsg(_("Conversion in %s not supported: from %s to %s"),
							  fname, ଧାଡ଼ି, p_enc);
		    vim_free(enc);
		    spin->si_conv.vc_fail = TRUE;
		}
		ଜାରି;
	    }

	    ଯଦି (STRNCMP(ଧାଡ଼ି, "regions=", 8) == 0)
	    {
		ଯଦି (spin->si_region_ଗଣନା > 1)
		    smsg(_("Duplicate /regions= line ignored in %s line %ld: %s"),
						       fname, lnum, ଧାଡ଼ି);
		ଅନ୍ୟଥା
		{
		    ଧାଡ଼ି += 8;
		    ଯଦି (STRLEN(ଧାଡ଼ି) > MAXREGIONS * 2)
			smsg(_("Too many regions in %s line %ld: %s"),
						       fname, lnum, ଧାଡ଼ି);
		    ଅନ୍ୟଥା
		    {
			spin->si_region_ଗଣନା = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ଧାଡ଼ି) / 2;
			STRCPY(spin->si_region_name, ଧାଡ଼ି);

			// Adjust the mask for a word valid in all regions.
			spin->si_region = (1 << spin->si_region_ଗଣନା) - 1;
		    }
		}
		ଜାରି;
	    }

	    smsg(_("/ line ignored in %s line %ld: %s"),
						       fname, lnum, ଧାଡ଼ି - 1);
	    ଜାରି;
	}

	flags = 0;
	regionmask = spin->si_region;

	// Check for flags and region after a slash.
	p = vim_strchr(ଧାଡ଼ି, '/');
	ଯଦି (p != ଖାଲି)
	{
	    *p++ = NUL;
	    ଯେତେବେଳେ (*p != NUL)
	    {
		ଯଦି (*p == '=')		// keep-case word
		    flags |= WF_KEEPCAP | WF_FIXCAP;
		ଅନ୍ୟଥା ଯଦି (*p == '!')	// Bad, bad, wicked word.
		    flags |= WF_BANNED;
		ଅନ୍ୟଥା ଯଦି (*p == '?')	// Rare word.
		    flags |= WF_RARE;
		ଅନ୍ୟଥା ଯଦି (VIM_ISDIGIT(*p)) // region number(s)
		{
		    ଯଦି ((flags & WF_REGION) == 0)   // first one
			regionmask = 0;
		    flags |= WF_REGION;

		    l = *p - '0';
		    ଯଦି (l == 0 || l > spin->si_region_ଗଣନା)
		    {
			smsg(_("Invalid region nr in %s line %ld: %s"),
							  fname, lnum, p);
			ଭାଙ୍ଗ;
		    }
		    regionmask |= 1 << (l - 1);
		}
		ଅନ୍ୟଥା
		{
		    smsg(_("Unrecognized flags in %s line %ld: %s"),
							      fname, lnum, p);
		    ଭାଙ୍ଗ;
		}
		++p;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	ଯଦି (spin->si_ascii && has_non_ascii(ଧାଡ଼ି))
	{
	    ++non_ascii;
	    ଜାରି;
	}

	// Normal word: store it.
	ଯଦି (store_word(spin, ଧାଡ଼ି, flags, regionmask, ଖାଲି, FALSE) == FAIL)
	{
	    retval = FAIL;
	    ଭାଙ୍ଗ;
	}
	did_word = TRUE;
    }

    vim_free(pc);
    fclose(fd);

    ଯଦି (spin->si_ascii && non_ascii > 0)
    {
	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
		  _("Ignored %d words with non-ASCII characters"), non_ascii);
	spell_message(spin, IObuff);
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * Get part of an sblock_T, "len" bytes long.
 * This avoids calling free() for every little struct we use (and keeping
 * track of them).
 * The memory is cleared to all zeros.
 * Returns NULL when out of memory.
 */
    ସ୍ଥିର ଶୂନ୍ୟ *
getroom(
    spellinfo_T *spin,
    size_t	len,		// length needed
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		align)		// align for pointer
{
    char_u	*p;
    sblock_T	*bl = spin->si_blocks;

    ଯଦି (align && bl != ଖାଲି)
	// Round size up for alignment.  On some systems structures need to be
	// aligned to the size of a pointer (e.g., SPARC).
	bl->sb_used = (bl->sb_used + ଆକାର(ଅକ୍ଷର *) - 1)
						      & ~(ଆକାର(ଅକ୍ଷର *) - 1);

    ଯଦି (bl == ଖାଲି || bl->sb_used + len > SBLOCKSIZE)
    {
	ଯଦି (len >= SBLOCKSIZE)
	    bl = ଖାଲି;
	ଅନ୍ୟଥା
	    // Allocate a block of memory. It is not freed until much later.
	    bl = alloc_clear(offsetof(sblock_T, sb_data) + SBLOCKSIZE + 1);
	ଯଦି (bl == ଖାଲି)
	{
	    ଯଦି (!spin->si_did_emsg)
	    {
		emsg(_(e_insufficient_memory_word_list_will_be_incomplete));
		spin->si_did_emsg = TRUE;
	    }
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	bl->sb_next = spin->si_blocks;
	spin->si_blocks = bl;
	bl->sb_used = 0;
	++spin->si_blocks_cnt;
    }

    p = bl->sb_data + bl->sb_used;
    bl->sb_used += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len;

    ଫେରନ୍ତୁ p;
}

/*
 * Make a copy of a string into memory allocated with getroom().
 * Returns NULL when out of memory.
 */
    ସ୍ଥିର char_u *
getroom_save(spellinfo_T *spin, char_u *s)
{
    char_u	*sc;

    sc = (char_u *)getroom(spin, STRLEN(s) + 1, FALSE);
    ଯଦି (sc != ଖାଲି)
	STRCPY(sc, s);
    ଫେରନ୍ତୁ sc;
}


/*
 * Free the list of allocated sblock_T.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
free_blocks(sblock_T *bl)
{
    sblock_T	*next;

    ଯେତେବେଳେ (bl != ଖାଲି)
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
    ସ୍ଥିର wordnode_T *
wordtree_alloc(spellinfo_T *spin)
{
    ଫେରନ୍ତୁ (wordnode_T *)getroom(spin, ଆକାର(wordnode_T), TRUE);
}

/*
 * Return TRUE if "word" contains valid word characters.
 * Control characters and trailing '/' are invalid.  Space is OK.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
valid_spell_word(char_u *word, char_u *end)
{
    char_u *p;

    ଯଦି (enc_utf8 && !utf_valid_string(word, end))
	ଫେରନ୍ତୁ FALSE;
    ପାଇଁ (p = word; *p != NUL && p < end; p += mb_ptr2len(p))
	ଯଦି (*p < ' ' || (p[0] == '/' && p[1] == NUL))
	    ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ TRUE;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
store_word(
    spellinfo_T	*spin,
    char_u	*word,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,		// extra flags, WF_BANNED
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		region,		// supported region(s)
    char_u	*pfxlist,	// list of prefix IDs or NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_affix)	// only store word with affix ID
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(word);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ct = captype(word, word + len);
    char_u	foldword[MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = OK;
    char_u	*p;

    // Avoid adding illegal bytes to the word tree.
    ଯଦି (!valid_spell_word(word, word + len))
	ଫେରନ୍ତୁ FAIL;

    (ଶୂନ୍ୟ)spell_casefold(curwin, word, len, foldword, MAXWLEN);
    ପାଇଁ (p = pfxlist; res == OK; ++p)
    {
	ଯଦି (!need_affix || (p != ଖାଲି && *p != NUL))
	    res = ବୃକ୍ଷ_add_word(spin, foldword, spin->si_foldroot, ct | flags,
						  region, p == ଖାଲି ? 0 : *p);
	ଯଦି (p == ଖାଲି || *p == NUL)
	    ଭାଙ୍ଗ;
    }
    ++spin->si_foldwcount;

    ଯଦି (res == OK && (ct == WF_KEEPCAP || (flags & WF_KEEPCAP)))
    {
	ପାଇଁ (p = pfxlist; res == OK; ++p)
	{
	    ଯଦି (!need_affix || (p != ଖାଲି && *p != NUL))
		res = ବୃକ୍ଷ_add_word(spin, word, spin->si_keeproot, flags,
						  region, p == ଖାଲି ? 0 : *p);
	    ଯଦି (p == ଖାଲି || *p == NUL)
		ଭାଙ୍ଗ;
	}
	++spin->si_keepwcount;
    }
    ଫେରନ୍ତୁ res;
}

/*
 * Add word "word" to a word tree at "root".
 * When "flags" < 0 we are adding to the prefix tree where "flags" is used for
 * "rare" and "region" is the condition nr.
 * Returns FAIL when out of memory.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ବୃକ୍ଷ_add_word(
    spellinfo_T	*spin,
    char_u	*word,
    wordnode_T	*ମୂଳ,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		region,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		affixID)
{
    wordnode_T	*ଗଣ୍ଠି = ମୂଳ;
    wordnode_T	*np;
    wordnode_T	*copyp, **copyprev;
    wordnode_T	**prev = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    // Add each byte of the word to the tree, including the NUL at the end.
    ପାଇଁ (i = 0; ; ++i)
    {
	// When there is more than one reference to this node we need to make
	// a copy, so that we can modify it.  Copy the whole list of siblings
	// (we don't optimize for a partly shared list of siblings).
	ଯଦି (ଗଣ୍ଠି != ଖାଲି && ଗଣ୍ଠି->wn_refs > 1)
	{
	    --ଗଣ୍ଠି->wn_refs;
	    copyprev = prev;
	    FOR_ALL_ଗଣ୍ଠି_SIBLINGS(ଗଣ୍ଠି, copyp)
	    {
		// Allocate a new node and copy the info.
		np = get_wordnode(spin);
		ଯଦି (np == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
		np->wn_child = copyp->wn_child;
		ଯଦି (np->wn_child != ଖାଲି)
		    ++np->wn_child->wn_refs;	// child gets extra ref
		np->wn_byte = copyp->wn_byte;
		ଯଦି (np->wn_byte == NUL)
		{
		    np->wn_flags = copyp->wn_flags;
		    np->wn_region = copyp->wn_region;
		    np->wn_affixID = copyp->wn_affixID;
		}

		// Link the new node in the list, there will be one ref.
		np->wn_refs = 1;
		ଯଦି (copyprev != ଖାଲି)
		    *copyprev = np;
		copyprev = &np->wn_sibling;

		// Let "node" point to the head of the copied list.
		ଯଦି (copyp == ଗଣ୍ଠି)
		    ଗଣ୍ଠି = np;
	    }
	}

	// Look for the sibling that has the same character.  They are sorted
	// on byte value, thus stop searching when a sibling is found with a
	// higher byte value.  For zero bytes (end of word) the sorting is
	// done on flags and then on affixID.
	ଯେତେବେଳେ (ଗଣ୍ଠି != ଖାଲି
		&& (ଗଣ୍ଠି->wn_byte < word[i]
		    || (ଗଣ୍ଠି->wn_byte == NUL
			&& (flags < 0
			    ? ଗଣ୍ଠି->wn_affixID < (unsigned)affixID
			    : (ଗଣ୍ଠି->wn_flags < (unsigned)(flags & WN_MASK)
				|| (ଗଣ୍ଠି->wn_flags == (flags & WN_MASK)
				    && (spin->si_sugtree
					? (ଗଣ୍ଠି->wn_region & 0xffff) < region
					: ଗଣ୍ଠି->wn_affixID
						    < (unsigned)affixID)))))))
	{
	    prev = &ଗଣ୍ଠି->wn_sibling;
	    ଗଣ୍ଠି = *prev;
	}
	ଯଦି (ଗଣ୍ଠି == ଖାଲି
		|| ଗଣ୍ଠି->wn_byte != word[i]
		|| (word[i] == NUL
		    && (flags < 0
			|| spin->si_sugtree
			|| ଗଣ୍ଠି->wn_flags != (flags & WN_MASK)
			|| ଗଣ୍ଠି->wn_affixID != affixID)))
	{
	    // Allocate a new node.
	    np = get_wordnode(spin);
	    ଯଦି (np == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	    np->wn_byte = word[i];

	    // If "node" is NULL this is a new child or the end of the sibling
	    // list: ref count is one.  Otherwise use ref count of sibling and
	    // make ref count of sibling one (matters when inserting in front
	    // of the list of siblings).
	    ଯଦି (ଗଣ୍ଠି == ଖାଲି)
		np->wn_refs = 1;
	    ଅନ୍ୟଥା
	    {
		np->wn_refs = ଗଣ୍ଠି->wn_refs;
		ଗଣ୍ଠି->wn_refs = 1;
	    }
	    ଯଦି (prev != ଖାଲି)
		*prev = np;
	    np->wn_sibling = ଗଣ୍ଠି;
	    ଗଣ୍ଠି = np;
	}

	ଯଦି (word[i] == NUL)
	{
	    ଗଣ୍ଠି->wn_flags = flags;
	    ଗଣ୍ଠି->wn_region |= region;
	    ଗଣ୍ଠି->wn_affixID = affixID;
	    ଭାଙ୍ଗ;
	}
	prev = &ଗଣ୍ଠି->wn_child;
	ଗଣ୍ଠି = *prev;
    }
#ifdef SPELL_PRINTTREE
    smsg("Added \"%s\"", word);
    spell_print_ବୃକ୍ଷ(ମୂଳ->wn_sibling);
#endif

    // count nr of words added since last message
    ++spin->si_ସନ୍ଦେଶ_ଗଣନା;

    ଯଦି (spin->si_compress_cnt > 1)
    {
	ଯଦି (--spin->si_compress_cnt == 1)
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
    ଯଦି (spin->si_compress_cnt == 1
	    ? spin->si_free_ଗଣନା < MAXWLEN
	    : spin->si_blocks_cnt >= compress_start)
#endif
    {
	// Decrement the block counter.  The effect is that we compress again
	// when the freed up room has been used and another "compress_inc"
	// blocks have been allocated.  Unless "compress_added" words have
	// been added, then the limit is put back again.
	spin->si_blocks_cnt -= compress_inc;
	spin->si_compress_cnt = compress_added;

	ଯଦି (spin->si_verbose)
	{
	    ସନ୍ଦେଶ_start();
	    ସନ୍ଦେଶ_puts(_(ସନ୍ଦେଶ_compressing));
	    ସନ୍ଦେଶ_clr_eos();
	    ସନ୍ଦେଶ_didout = FALSE;
	    ସନ୍ଦେଶ_col = 0;
	    out_flush();
	}

	// Compress both trees.  Either they both have many nodes, which makes
	// compression useful, or one of them is small, which means
	// compression goes fast.  But when filling the soundfold word tree
	// there is no keep-case tree.
	wordtree_compress(spin, spin->si_foldroot, "case-folded");
	ଯଦି (affixID >= 0)
	    wordtree_compress(spin, spin->si_keeproot, "keep-case");
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Get a wordnode_T, either from the list of previously freed nodes or
 * allocate a new one.
 * Returns NULL when out of memory.
 */
    ସ୍ଥିର wordnode_T *
get_wordnode(spellinfo_T *spin)
{
    wordnode_T *n;

    ଯଦି (spin->si_first_free == ଖାଲି)
	n = (wordnode_T *)getroom(spin, ଆକାର(wordnode_T), TRUE);
    ଅନ୍ୟଥା
    {
	n = spin->si_first_free;
	spin->si_first_free = n->wn_child;
	CLEAR_POINTER(n);
	--spin->si_free_ଗଣନା;
    }
#ifdef SPELL_PRINTTREE
    ଯଦି (n != ଖାଲି)
	n->wn_nr = ++spin->si_wordnode_nr;
#endif
    ଫେରନ୍ତୁ n;
}

/*
 * Decrement the reference count on a node (which is the head of a list of
 * siblings).  If the reference count becomes zero free the node and its
 * siblings.
 * Returns the number of nodes actually freed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
deref_wordnode(spellinfo_T *spin, wordnode_T *ଗଣ୍ଠି)
{
    wordnode_T	*np;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cnt = 0;

    ଯଦି (--ଗଣ୍ଠି->wn_refs == 0)
    {
	FOR_ALL_ଗଣ୍ଠି_SIBLINGS(ଗଣ୍ଠି, np)
	{
	    ଯଦି (np->wn_child != ଖାଲି)
		cnt += deref_wordnode(spin, np->wn_child);
	    free_wordnode(spin, np);
	    ++cnt;
	}
	++cnt;	    // length field
    }
    ଫେରନ୍ତୁ cnt;
}

/*
 * Free a wordnode_T for re-use later.
 * Only the "wn_child" field becomes invalid.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
free_wordnode(spellinfo_T *spin, wordnode_T *n)
{
    n->wn_child = spin->si_first_free;
    spin->si_first_free = n;
    ++spin->si_free_ଗଣନା;
}

/*
 * Compress a tree: find tails that are identical and can be shared.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
wordtree_compress(spellinfo_T *spin, wordnode_T *ମୂଳ, ଅକ୍ଷର *name)
{
    hashtab_T	    ht;
    long	    n;
    long	    tot = 0;
    long	    perc;

    // Skip the root itself, it's not actually used.  The first sibling is the
    // start of the tree.
    ଯଦି (ମୂଳ->wn_sibling == ଖାଲି)
	ଫେରନ୍ତୁ;

    hash_init(&ht);
    n = ଗଣ୍ଠି_compress(spin, ମୂଳ->wn_sibling, &ht, &tot);

#ifndef SPELL_PRINTTREE
    ଯଦି (spin->si_verbose || p_verbose > 2)
#endif
    {
	ଯଦି (tot > 1000000)
	    perc = (tot - n) / (tot / 100);
	ଅନ୍ୟଥା ଯଦି (tot == 0)
	    perc = 0;
	ଅନ୍ୟଥା
	    perc = (tot - n) * 100 / tot;
	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
		_("Compressed %s: %ld of %ld nodes; %ld (%ld%%) remaining"),
		name, n, tot, tot - n, perc);
	spell_message(spin, IObuff);
    }
#ifdef SPELL_PRINTTREE
    spell_print_ବୃକ୍ଷ(ମୂଳ->wn_sibling);
#endif
    hash_clear(&ht);
}

/*
 * Compress a node, its siblings and its children, depth first.
 * Returns the number of compressed nodes.
 */
    ସ୍ଥିର long
ଗଣ୍ଠି_compress(
    spellinfo_T	*spin,
    wordnode_T	*ଗଣ୍ଠି,
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
    ପାଇଁ (np = ଗଣ୍ଠି; np != ଖାଲି && !got_int; np = np->wn_sibling)
    {
	++len;
	ଯଦି ((child = np->wn_child) != ଖାଲି)
	{
	    // Compress the child first.  This fills hashkey.
	    compressed += ଗଣ୍ଠି_compress(spin, child, ht, tot);

	    // Try to find an identical child.
	    hash = hash_hash(child->wn_u1.hashkey);
	    hi = hash_lookup(ht, child->wn_u1.hashkey, hash);
	    ଯଦି (!HASHITEM_EMPTY(hi))
	    {
		// There are children we encountered before with a hash value
		// identical to the current child.  Now check if there is one
		// that is really identical.
		ପାଇଁ (tp = HI2WN(hi); tp != ଖାଲି; tp = tp->wn_u2.next)
		    ଯଦି (ଗଣ୍ଠି_equal(child, tp))
		    {
			// Found one!  Now use that child in place of the
			// current one.  This means the current child and all
			// its siblings is unlinked from the tree.
			++tp->wn_refs;
			compressed += deref_wordnode(spin, child);
			np->wn_child = tp;
			ଭାଙ୍ଗ;
		    }
		ଯଦି (tp == ଖାଲି)
		{
		    // No other child with this hash value equals the child of
		    // the node, add it to the linked list after the first
		    // item.
		    tp = HI2WN(hi);
		    child->wn_u2.next = tp->wn_u2.next;
		    tp->wn_u2.next = child;
		}
	    }
	    ଅନ୍ୟଥା
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
    ଗଣ୍ଠି->wn_u1.hashkey[0] = len;
    nr = 0;
    FOR_ALL_ଗଣ୍ଠି_SIBLINGS(ଗଣ୍ଠି, np)
    {
	ଯଦି (np->wn_byte == NUL)
	    // end node: use wn_flags, wn_region and wn_affixID
	    n = np->wn_flags + (np->wn_region << 8) + (np->wn_affixID << 16);
	ଅନ୍ୟଥା
	    // byte node: use the byte value and the child pointer
	    n = (unsigned)(np->wn_byte + ((long_u)np->wn_child << 8));
	nr = nr * 101 + n;
    }

    // Avoid NUL bytes, it terminates the hash key.
    n = nr & 0xff;
    ଗଣ୍ଠି->wn_u1.hashkey[1] = n == 0 ? 1 : n;
    n = (nr >> 8) & 0xff;
    ଗଣ୍ଠି->wn_u1.hashkey[2] = n == 0 ? 1 : n;
    n = (nr >> 16) & 0xff;
    ଗଣ୍ଠି->wn_u1.hashkey[3] = n == 0 ? 1 : n;
    n = (nr >> 24) & 0xff;
    ଗଣ୍ଠି->wn_u1.hashkey[4] = n == 0 ? 1 : n;
    ଗଣ୍ଠି->wn_u1.hashkey[5] = NUL;

    // Check for CTRL-C pressed now and then.
    veryfast_breakcheck();

    ଫେରନ୍ତୁ compressed;
}

/*
 * Return TRUE when two nodes have identical siblings and children.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ଗଣ୍ଠି_equal(wordnode_T *n1, wordnode_T *n2)
{
    wordnode_T	*p1;
    wordnode_T	*p2;

    ପାଇଁ (p1 = n1, p2 = n2; p1 != ଖାଲି && p2 != ଖାଲି;
				     p1 = p1->wn_sibling, p2 = p2->wn_sibling)
	ଯଦି (p1->wn_byte != p2->wn_byte
		|| (p1->wn_byte == NUL
		    ? (p1->wn_flags != p2->wn_flags
			|| p1->wn_region != p2->wn_region
			|| p1->wn_affixID != p2->wn_affixID)
		    : (p1->wn_child != p2->wn_child)))
	    ଭାଙ୍ଗ;

    ଫେରନ୍ତୁ p1 == ଖାଲି && p2 == ଖାଲି;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rep_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2);

/*
 * Function given to qsort() to sort the REP items on "from" string.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
rep_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    fromto_T	*p1 = (fromto_T *)s1;
    fromto_T	*p2 = (fromto_T *)s2;

    ଫେରନ୍ତୁ STRCMP(p1->ft_from, p2->ft_from);
}

/*
 * Write the Vim .spl file "fname".
 * Return FAIL or OK;
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
write_vim_spell(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		regionmask;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;
    wordnode_T	*ବୃକ୍ଷ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nodecount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
    garray_T	*gap;
    fromto_T	*ftp;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
    size_t	fwv = 1;  // collect return value of fwrite() to avoid
			  // warnings from picky compiler

    fd = mch_fopen((ଅକ୍ଷର *)fname, "w");
    ଯଦି (fd == ଖାଲି)
    {
	semsg(_(e_cant_open_file_str), fname);
	ଫେରନ୍ତୁ FAIL;
    }

    // <HEADER>: <fileID> <versionnr>
							    // <fileID>
    fwv &= fwrite(VIMSPELLMAGIC, VIMSPELLMAGICL, (size_t)1, fd);
    ଯଦି (fwv != (size_t)1)
	// Catch first write error, don't try writing more.
	ଯାଅ theend;

    putc(VIMSPELLVERSION, fd);				    // <versionnr>

    /*
     * <SECTIONS>: <section> ... <sectionend>
     */

    // SN_INFO: <infotext>
    ଯଦି (spin->si_info != ଖାଲି)
    {
	putc(SN_INFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(spin->si_info);
	put_bytes(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_info, (size_t)i, (size_t)1, fd); // <infotext>
    }

    // SN_REGION: <regionname> ...
    // Write the region names only if there is more than one.
    ଯଦି (spin->si_region_ଗଣନା > 1)
    {
	putc(SN_REGION, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>
	l = spin->si_region_ଗଣନା * 2;
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_region_name, (size_t)l, (size_t)1, fd);
							// <regionname> ...
	regionmask = (1 << spin->si_region_ଗଣନା) - 1;
    }
    ଅନ୍ୟଥା
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
    ଯଦି (!spin->si_ascii && !spin->si_add)
    {
	char_u	folchars[128 * 8];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	flags;

	putc(SN_CHARFLAGS, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	// Form the <folchars> string first, we need to know its length.
	l = 0;
	ପାଇଁ (i = 128; i < 256; ++i)
	{
	    ଯଦି (has_mbyte)
		l += mb_char2bytes(spelltab.st_fold[i], folchars + l);
	    ଅନ୍ୟଥା
		folchars[l++] = spelltab.st_fold[i];
	}
	put_bytes(fd, (long_u)(1 + 128 + 2 + l), 4);	// <sectionlen>

	fputc(128, fd);					// <charflagslen>
	ପାଇଁ (i = 128; i < 256; ++i)
	{
	    flags = 0;
	    ଯଦି (spelltab.st_isw[i])
		flags |= CF_WORD;
	    ଯଦି (spelltab.st_isu[i])
		flags |= CF_UPPER;
	    fputc(flags, fd);				// <charflags>
	}

	put_bytes(fd, (long_u)l, 2);			// <folcharslen>
	fwv &= fwrite(folchars, (size_t)l, (size_t)1, fd); // <folchars>
    }

    // SN_MIDWORD: <midword>
    ଯଦି (spin->si_midword != ଖାଲି)
    {
	putc(SN_MIDWORD, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(spin->si_midword);
	put_bytes(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_midword, (size_t)i, (size_t)1, fd);
							// <midword>
    }

    // SN_PREFCOND: <prefcondcnt> <prefcond> ...
    ଯଦି (spin->si_prefcond.ga_len > 0)
    {
	putc(SN_PREFCOND, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	l = write_spell_prefcond(ଖାଲି, &spin->si_prefcond, &fwv);
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>

	write_spell_prefcond(fd, &spin->si_prefcond, &fwv);
    }

    // SN_REP: <repcount> <rep> ...
    // SN_SAL: <salflags> <salcount> <sal> ...
    // SN_REPSAL: <repcount> <rep> ...

    // round 1: SN_REP section
    // round 2: SN_SAL section (unless SN_SOFO is used)
    // round 3: SN_REPSAL section
    ପାଇଁ (round = 1; round <= 3; ++round)
    {
	ଯଦି (round == 1)
	    gap = &spin->si_rep;
	ଅନ୍ୟଥା ଯଦି (round == 2)
	{
	    // Don't write SN_SAL when using a SN_SOFO section
	    ଯଦି (spin->si_sofofr != ଖାଲି && spin->si_sofoto != ଖାଲି)
		ଜାରି;
	    gap = &spin->si_sal;
	}
	ଅନ୍ୟଥା
	    gap = &spin->si_repsal;

	// Don't write the section if there are no items.
	ଯଦି (gap->ga_len == 0)
	    ଜାରି;

	// Sort the REP/REPSAL items.
	ଯଦି (round != 2)
	    qsort(gap->ga_data, (size_t)gap->ga_len,
					       ଆକାର(fromto_T), rep_compare);

	i = round == 1 ? SN_REP : (round == 2 ? SN_SAL : SN_REPSAL);
	putc(i, fd);					// <sectionID>

	// This is for making suggestions, section is not required.
	putc(0, fd);					// <sectionflags>

	// Compute the length of what follows.
	l = 2;	    // count <repcount> or <salcount>
	ପାଇଁ (i = 0; i < gap->ga_len; ++i)
	{
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    l += 1 + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ftp->ft_from);  // count <*fromlen> and <*from>
	    l += 1 + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ftp->ft_to);    // count <*tolen> and <*to>
	}
	ଯଦି (round == 2)
	    ++l;	// count <salflags>
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>

	ଯଦି (round == 2)
	{
	    i = 0;
	    ଯଦି (spin->si_followup)
		i |= SAL_F0LLOWUP;
	    ଯଦି (spin->si_collapse)
		i |= SAL_COLLAPSE;
	    ଯଦି (spin->si_rem_accents)
		i |= SAL_REM_ACCENTS;
	    putc(i, fd);			// <salflags>
	}

	put_bytes(fd, (long_u)gap->ga_len, 2);	// <repcount> or <salcount>
	ପାଇଁ (i = 0; i < gap->ga_len; ++i)
	{
	    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
	    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    ପାଇଁ (rr = 1; rr <= 2; ++rr)
	    {
		p = rr == 1 ? ftp->ft_from : ftp->ft_to;
		l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);
		putc(l, fd);
		ଯଦି (l > 0)
		    fwv &= fwrite(p, l, (size_t)1, fd);
	    }
	}

    }

    // SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
    // This is for making suggestions, section is not required.
    ଯଦି (spin->si_sofofr != ଖାଲି && spin->si_sofoto != ଖାଲି)
    {
	putc(SN_SOFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(spin->si_sofofr);
	put_bytes(fd, (long_u)(l + STRLEN(spin->si_sofoto) + 4), 4);
							// <sectionlen>

	put_bytes(fd, (long_u)l, 2);			// <sofofromlen>
	fwv &= fwrite(spin->si_sofofr, l, (size_t)1, fd); // <sofofrom>

	l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(spin->si_sofoto);
	put_bytes(fd, (long_u)l, 2);			// <sofotolen>
	fwv &= fwrite(spin->si_sofoto, l, (size_t)1, fd); // <sofoto>
    }

    // SN_WORDS: <word> ...
    // This is for making suggestions, section is not required.
    ଯଦି (spin->si_commonwords.ht_used > 0)
    {
	putc(SN_ଶବ୍ଦଗୁଡ଼ିକ, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// round 1: count the bytes
	// round 2: write the bytes
	ପାଇଁ (round = 1; round <= 2; ++round)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
	    hashitem_T	*hi;

	    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)spin->si_commonwords.ht_used;
	    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&spin->si_commonwords, hi, todo)
		ଯଦି (!HASHITEM_EMPTY(hi))
		{
		    l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(hi->hi_ଚାବି) + 1;
		    len += l;
		    ଯଦି (round == 2)			// <word>
			fwv &= fwrite(hi->hi_ଚାବି, (size_t)l, (size_t)1, fd);
		    --todo;
		}
	    ଯଦି (round == 1)
		put_bytes(fd, (long_u)len, 4);		// <sectionlen>
	}
    }

    // SN_MAP: <mapstr>
    // This is for making suggestions, section is not required.
    ଯଦି (spin->si_map.ga_len > 0)
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
    ଯଦି (!spin->si_nosugfile
	    && (spin->si_sal.ga_len > 0
		     || (spin->si_sofofr != ଖାଲି && spin->si_sofoto != ଖାଲି)))
    {
	putc(SN_SUGFILE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)8, 4);			// <sectionlen>

	// Set si_sugtime and write it to the file.
	spin->si_sugtime = time(ଖାଲି);
	put_time(fd, spin->si_sugtime);			// <timestamp>
    }

    // SN_NOSPLITSUGS: nothing
    // This is used to notify that no suggestions with word splits are to be
    // made.
    ଯଦି (spin->si_nosplitsugs)
    {
	putc(SN_NOSPLITSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_NOCOMPUNDSUGS: nothing
    // This is used to notify that no suggestions with compounds are to be
    // made.
    ଯଦି (spin->si_nocompoundsugs)
    {
	putc(SN_NOCOMPOUNDSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_COMPOUND: compound info.
    // We don't mark it required, when not supported all compound words will
    // be bad words.
    ଯଦି (spin->si_compflags != ଖାଲି)
    {
	putc(SN_COMPOUND, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(spin->si_compflags);
	ପାଇଁ (i = 0; i < spin->si_comppat.ga_len; ++i)
	    l += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(((char_u **)(spin->si_comppat.ga_data))[i]) + 1;
	put_bytes(fd, (long_u)(l + 7), 4);		// <sectionlen>

	putc(spin->si_compmax, fd);			// <compmax>
	putc(spin->si_compminlen, fd);			// <compminlen>
	putc(spin->si_compsylmax, fd);			// <compsylmax>
	putc(0, fd);		// for Vim 7.0b compatibility
	putc(spin->si_compoptions, fd);			// <compoptions>
	put_bytes(fd, (long_u)spin->si_comppat.ga_len, 2);
							// <comppatcount>
	ପାଇଁ (i = 0; i < spin->si_comppat.ga_len; ++i)
	{
	    p = ((char_u **)(spin->si_comppat.ga_data))[i];
	    putc((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p), fd);			// <comppatlen>
	    fwv &= fwrite(p, (size_t)STRLEN(p), (size_t)1, fd);
							// <comppattext>
	}
							// <compflags>
	fwv &= fwrite(spin->si_compflags, (size_t)STRLEN(spin->si_compflags),
							       (size_t)1, fd);
    }

    // SN_NOBREAK: NOBREAK flag
    ଯଦି (spin->si_nobreak)
    {
	putc(SN_NOBREAK, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// It's empty, the presence of the section flags the feature.
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_SYLLABLE: syllable info.
    // We don't mark it required, when not supported syllables will not be
    // counted.
    ଯଦି (spin->si_syllable != ଖାଲି)
    {
	putc(SN_SYLLABLE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(spin->si_syllable);
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
    ପାଇଁ (round = 1; round <= 3; ++round)
    {
	ଯଦି (round == 1)
	    ବୃକ୍ଷ = spin->si_foldroot->wn_sibling;
	ଅନ୍ୟଥା ଯଦି (round == 2)
	    ବୃକ୍ଷ = spin->si_keeproot->wn_sibling;
	ଅନ୍ୟଥା
	    ବୃକ୍ଷ = spin->si_prefroot->wn_sibling;

	// Clear the index and wnode fields in the tree.
	clear_ଗଣ୍ଠି(ବୃକ୍ଷ);

	// Count the number of nodes.  Needed to be able to allocate the
	// memory when reading the nodes.  Also fills in index for shared
	// nodes.
	nodecount = put_ଗଣ୍ଠି(ଖାଲି, ବୃକ୍ଷ, 0, regionmask, round == 3);

	// number of nodes in 4 bytes
	put_bytes(fd, (long_u)nodecount, 4);	// <nodecount>
	spin->si_memtot += nodecount + nodecount * ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

	// Write the nodes.
	(ଶୂନ୍ୟ)put_ଗଣ୍ଠି(fd, ବୃକ୍ଷ, 0, regionmask, round == 3);
    }

    // Write another byte to check for errors (file system full).
    ଯଦି (putc(0, fd) == EOF)
	retval = FAIL;
theend:
    ଯଦି (fclose(fd) == EOF)
	retval = FAIL;

    ଯଦି (fwv != (size_t)1)
	retval = FAIL;
    ଯଦି (retval == FAIL)
	emsg(_(e_error_while_writing));

    ଫେରନ୍ତୁ retval;
}

/*
 * Clear the index and wnode fields of "node", it siblings and its
 * children.  This is needed because they are a union with other items to save
 * space.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clear_ଗଣ୍ଠି(wordnode_T *ଗଣ୍ଠି)
{
    wordnode_T	*np;

    ଯଦି (ଗଣ୍ଠି != ଖାଲି)
	FOR_ALL_ଗଣ୍ଠି_SIBLINGS(ଗଣ୍ଠି, np)
	{
	    np->wn_u1.index = 0;
	    np->wn_u2.wnode = ଖାଲି;

	    ଯଦି (np->wn_byte != NUL)
		clear_ଗଣ୍ଠି(np->wn_child);
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
put_ଗଣ୍ଠି(
    FILE	*fd,		// NULL when only counting
    wordnode_T	*ଗଣ୍ଠି,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		regionmask,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prefixtree)	// TRUE for PREFIXTREE
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newindex = idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		siblingcount = 0;
    wordnode_T	*np;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;

    // If "node" is zero the tree is empty.
    ଯଦି (ଗଣ୍ଠି == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    // Store the index where this node is written.
    ଗଣ୍ଠି->wn_u1.index = idx;

    // Count the number of siblings.
    FOR_ALL_ଗଣ୍ଠି_SIBLINGS(ଗଣ୍ଠି, np)
	++siblingcount;

    // Write the sibling count.
    ଯଦି (fd != ଖାଲି)
	putc(siblingcount, fd);				// <siblingcount>

    // Write each sibling byte and optionally extra info.
    FOR_ALL_ଗଣ୍ଠି_SIBLINGS(ଗଣ୍ଠି, np)
    {
	ଯଦି (np->wn_byte == 0)
	{
	    ଯଦି (fd != ଖାଲି)
	    {
		// For a NUL byte (end of word) write the flags etc.
		ଯଦି (prefixtree)
		{
		    // In PREFIXTREE write the required affixID and the
		    // associated condition nr (stored in wn_region).  The
		    // byte value is misused to store the "rare" and "not
		    // combining" flags
		    ଯଦି (np->wn_flags == (short_u)PFX_FLAGS)
			putc(BY_NOFLAGS, fd);		// <byte>
		    ଅନ୍ୟଥା
		    {
			putc(BY_FLAGS, fd);		// <byte>
			putc(np->wn_flags, fd);		// <pflags>
		    }
		    putc(np->wn_affixID, fd);		// <affixID>
		    put_bytes(fd, (long_u)np->wn_region, 2); // <prefcondnr>
		}
		ଅନ୍ୟଥା
		{
		    // For word trees we write the flag/region items.
		    flags = np->wn_flags;
		    ଯଦି (regionmask != 0 && np->wn_region != regionmask)
			flags |= WF_REGION;
		    ଯଦି (np->wn_affixID != 0)
			flags |= WF_AFX;
		    ଯଦି (flags == 0)
		    {
			// word without flags or region
			putc(BY_NOFLAGS, fd);			// <byte>
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (np->wn_flags >= 0x100)
			{
			    putc(BY_FLAGS2, fd);		// <byte>
			    putc(flags, fd);			// <flags>
			    putc((unsigned)flags >> 8, fd);	// <flags2>
			}
			ଅନ୍ୟଥା
			{
			    putc(BY_FLAGS, fd);			// <byte>
			    putc(flags, fd);			// <flags>
			}
			ଯଦି (flags & WF_REGION)
			    putc(np->wn_region, fd);		// <region>
			ଯଦି (flags & WF_AFX)
			    putc(np->wn_affixID, fd);		// <affixID>
		    }
		}
	    }
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (np->wn_child->wn_u1.index != 0
					 && np->wn_child->wn_u2.wnode != ଗଣ୍ଠି)
	    {
		// The child is written elsewhere, write the reference.
		ଯଦି (fd != ଖାଲି)
		{
		    putc(BY_INDEX, fd);			// <byte>
							// <nodeidx>
		    put_bytes(fd, (long_u)np->wn_child->wn_u1.index, 3);
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (np->wn_child->wn_u2.wnode == ଖାଲି)
		// We will write the child below and give it an index.
		np->wn_child->wn_u2.wnode = ଗଣ୍ଠି;

	    ଯଦି (fd != ଖାଲି)
		ଯଦି (putc(np->wn_byte, fd) == EOF) // <byte> or <xbyte>
		{
		    emsg(_(e_error_while_writing));
		    ଫେରନ୍ତୁ 0;
		}
	}
    }

    // Space used in the array when reading: one for each sibling and one for
    // the count.
    newindex += siblingcount + 1;

    // Recursively dump the children of each sibling.
    FOR_ALL_ଗଣ୍ଠି_SIBLINGS(ଗଣ୍ଠି, np)
	ଯଦି (np->wn_byte != 0 && np->wn_child->wn_u2.wnode == ଗଣ୍ଠି)
	    newindex = put_ଗଣ୍ଠି(fd, np->wn_child, newindex, regionmask,
								  prefixtree);

    ଫେରନ୍ତୁ newindex;
}


/*
 * ":mkspell [-ascii] outfile  infile ..."
 * ":mkspell [-ascii] addfile"
 */
    ଶୂନ୍ୟ
ex_mkspell(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fcount;
    char_u	**fnames;
    char_u	*arg = eap->arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ascii = FALSE;

    ଯଦି (STRNCMP(arg, "-ascii", 6) == 0)
    {
	ascii = TRUE;
	arg = skipwhite(arg + 6);
    }

    // Expand all the remaining arguments (e.g., $VIMRUNTIME).
    ଯଦି (get_arglist_exp(arg, &fcount, &fnames, FALSE) != OK)
	ଫେରନ୍ତୁ;

    mkspell(fcount, fnames, ascii, eap->forceit, FALSE);
    FreeWild(fcount, fnames);
}

/*
 * Create the .sug file.
 * Uses the soundfold info in "spin".
 * Writes the file with the name "wfname", with ".spl" changed to ".sug".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
spell_make_sugfile(spellinfo_T *spin, char_u *wfname)
{
    char_u	*fname = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    slang_T	*slang;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		free_slang = FALSE;

    /*
     * Read back the .spl file that was written.  This fills the required
     * info for soundfolding.  This also uses less memory than the
     * pointer-linked version of the trie.  And it avoids having two versions
     * of the code for the soundfolding stuff.
     * It might have been done already by spell_reload_one().
     */
    FOR_ALL_SPELL_LANGS(slang)
	ଯଦି (fullpathcmp(wfname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	    ଭାଙ୍ଗ;
    ଯଦି (slang == ଖାଲି)
    {
	spell_message(spin, (char_u *)_("Reading back spell file..."));
	slang = spell_load_file(wfname, ଖାଲି, ଖାଲି, FALSE);
	ଯଦି (slang == ଖାଲି)
	    ଫେରନ୍ତୁ;
	free_slang = TRUE;
    }

    /*
     * Clear the info in "spin" that is used.
     */
    spin->si_blocks = ଖାଲି;
    spin->si_blocks_cnt = 0;
    spin->si_compress_cnt = 0;	    // will stay at 0 all the time
    spin->si_free_ଗଣନା = 0;
    spin->si_first_free = ଖାଲି;
    spin->si_foldwcount = 0;

    /*
     * Go through the trie of good words, soundfold each word and add it to
     * the soundfold trie.
     */
    spell_message(spin, (char_u *)_("Performing soundfolding..."));
    ଯଦି (sug_filltree(spin, slang) == FAIL)
	ଯାଅ theend;

    /*
     * Create the table which links each soundfold word with a list of the
     * good words it may come from.  Creates buffer "spin->si_spellbuf".
     * This also removes the wordnr from the NUL byte entries to make
     * compression possible.
     */
    ଯଦି (sug_maketable(spin) == FAIL)
	ଯାଅ theend;

    smsg(_("Number of words after soundfolding: %ld"),
				 (long)spin->si_spellbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା);

    /*
     * Compress the soundfold trie.
     */
    spell_message(spin, (char_u *)_(ସନ୍ଦେଶ_compressing));
    wordtree_compress(spin, spin->si_foldroot, "case-folded");

    /*
     * Write the .sug file.
     * Make the file name by changing ".spl" to ".sug".
     */
    fname = alloc(MAXPATHL);
    ଯଦି (fname == ଖାଲି)
	ଯାଅ theend;
    vim_strncpy(fname, wfname, MAXPATHL - 1);
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(fname);
    fname[len - 2] = 'u';
    fname[len - 1] = 'g';
    sug_write(spin, fname);

theend:
    vim_free(fname);
    ଯଦି (free_slang)
	slang_free(slang);
    free_blocks(spin->si_blocks);
    close_spellbuf(spin->si_spellbuf);
}

/*
 * Build the soundfold trie for language "slang".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sug_filltree(spellinfo_T *spin, slang_T *slang)
{
    char_u	*byts;
    idx_T	*idxs;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		depth;
    idx_T	arridx[MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		curi[MAXWLEN];
    char_u	tword[MAXWLEN];
    char_u	tsalword[MAXWLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    idx_T	n;
    unsigned	ଶବ୍ଦଗୁଡ଼ିକ_done = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wordcount[MAXWLEN];

    // We use si_foldroot for the soundfolded trie.
    spin->si_foldroot = wordtree_alloc(spin);
    ଯଦି (spin->si_foldroot == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // let tree_add_word() know we're adding to the soundfolded tree
    spin->si_sugtree = TRUE;

    /*
     * Go through the whole case-folded tree, soundfold each word and put it
     * in the trie.  Bail out if the tree is empty.
     */
    byts = slang->sl_fbyts;
    idxs = slang->sl_fidxs;
    ଯଦି (byts == ଖାଲି || idxs == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;

    depth = 0;
    ଯେତେବେଳେ (depth >= 0 && !got_int)
    {
	ଯଦି (curi[depth] > byts[arridx[depth]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[depth]] = wordcount[depth];
	    ଯଦି (depth > 0)
		wordcount[depth - 1] += wordcount[depth];

	    --depth;
	    ଧାଡ଼ି_breakcheck();
	}
	ଅନ୍ୟଥା
	{

	    // Do one more byte at this node.
	    n = arridx[depth] + curi[depth];
	    ++curi[depth];

	    c = byts[n];
	    ଯଦି (c == 0)
	    {
		// Sound-fold the word.
		tword[depth] = NUL;
		spell_soundfold(slang, tword, TRUE, tsalword);

		// We use the "flags" field for the MSB of the wordnr,
		// "region" for the LSB of the wordnr.
		ଯଦି (ବୃକ୍ଷ_add_word(spin, tsalword, spin->si_foldroot,
				ଶବ୍ଦଗୁଡ଼ିକ_done >> 16, ଶବ୍ଦଗୁଡ଼ିକ_done & 0xffff,
							   0) == FAIL)
		    ଫେରନ୍ତୁ FAIL;

		++ଶବ୍ଦଗୁଡ଼ିକ_done;
		++wordcount[depth];

		// Reset the block count each time to avoid compression
		// kicking in.
		spin->si_blocks_cnt = 0;

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		ଯେତେବେଳେ (n + 1 < slang->sl_fbyts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[depth];
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (depth < MAXWLEN - 1)
	    {
		// Normal char, go one level deeper.
		tword[depth++] = c;
		arridx[depth] = idxs[n];
		curi[depth] = 1;
		wordcount[depth] = 0;
	    }
	}
    }

    smsg(_("Total number of words: %d"), ଶବ୍ଦଗୁଡ଼ିକ_done);

    ଫେରନ୍ତୁ OK;
}

/*
 * Make the table that links each word in the soundfold trie to the words it
 * can be produced from.
 * This is not unlike lines in a file, thus use a memfile to be able to access
 * the table efficiently.
 * Returns FAIL when out of memory.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sug_maketable(spellinfo_T *spin)
{
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = OK;

    // Allocate a buffer, open a memline for it and create the swap file
    // (uses a temp file, not a .swp file).
    spin->si_spellbuf = open_spellbuf();
    ଯଦି (spin->si_spellbuf == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // Use a buffer to store the line info, avoids allocating many small
    // pieces of memory.
    ga_init2(&ga, 1, 100);

    // recursively go through the tree
    ଯଦି (sug_filltable(spin, spin->si_foldroot->wn_sibling, 0, &ga) == -1)
	res = FAIL;

    ga_clear(&ga);
    ଫେରନ୍ତୁ res;
}

/*
 * Fill the table for one node and its children.
 * Returns the wordnr at the start of the node.
 * Returns -1 when out of memory.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sug_filltable(
    spellinfo_T	*spin,
    wordnode_T	*ଗଣ୍ଠି,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startwordnr,
    garray_T	*gap)	    // place to store line of numbers
{
    wordnode_T	*p, *np;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wordnr = startwordnr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_nr;

    FOR_ALL_ଗଣ୍ଠି_SIBLINGS(ଗଣ୍ଠି, p)
    {
	ଯଦି (p->wn_byte == NUL)
	{
	    gap->ga_len = 0;
	    prev_nr = 0;
	    ପାଇଁ (np = p; np != ଖାଲି && np->wn_byte == NUL; np = np->wn_sibling)
	    {
		ଯଦି (ga_grow(gap, 10) == FAIL)
		    ଫେରନ୍ତୁ -1;

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

	    ଯଦି (ml_append_buf(spin->si_spellbuf, (linenr_T)wordnr,
				     gap->ga_data, gap->ga_len, TRUE) == FAIL)
		ଫେରନ୍ତୁ -1;
	    ++wordnr;

	    // Remove extra NUL entries, we no longer need them. We don't
	    // bother freeing the nodes, they won't be reused anyway.
	    ଯେତେବେଳେ (p->wn_sibling != ଖାଲି && p->wn_sibling->wn_byte == NUL)
		p->wn_sibling = p->wn_sibling->wn_sibling;

	    // Clear the flags on the remaining NUL node, so that compression
	    // works a lot better.
	    p->wn_flags = 0;
	    p->wn_region = 0;
	}
	ଅନ୍ୟଥା
	{
	    wordnr = sug_filltable(spin, p->wn_child, wordnr, gap);
	    ଯଦି (wordnr == -1)
		ଫେରନ୍ତୁ -1;
	}
    }
    ଫେରନ୍ତୁ wordnr;
}

/*
 * Convert an offset into a minimal number of bytes.
 * Similar to utf_char2byters, but use 8 bits in followup bytes and avoid NUL
 * bytes.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
offset2bytes(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr, char_u *buf)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    rem;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    b1, b2, b3, b4;

    // Split the number in parts of base 255.  We need to avoid NUL bytes.
    b1 = nr % 255 + 1;
    rem = nr / 255;
    b2 = rem % 255 + 1;
    rem = rem / 255;
    b3 = rem % 255 + 1;
    b4 = rem / 255 + 1;

    ଯଦି (b4 > 1 || b3 > 0x1f)	// 4 bytes
    {
	buf[0] = 0xe0 + b4;
	buf[1] = b3;
	buf[2] = b2;
	buf[3] = b1;
	ଫେରନ୍ତୁ 4;
    }
    ଯଦି (b3 > 1 || b2 > 0x3f )	// 3 bytes
    {
	buf[0] = 0xc0 + b3;
	buf[1] = b2;
	buf[2] = b1;
	ଫେରନ୍ତୁ 3;
    }
    ଯଦି (b2 > 1 || b1 > 0x7f )	// 2 bytes
    {
	buf[0] = 0x80 + b2;
	buf[1] = b1;
	ଫେରନ୍ତୁ 2;
    }
				// 1 byte
    buf[0] = b1;
    ଫେରନ୍ତୁ 1;
}

/*
 * Write the .sug file in "fname".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
sug_write(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    wordnode_T	*ବୃକ୍ଷ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nodecount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wcount;
    char_u	*ଧାଡ଼ି;
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    // Create the file.  Note that an existing file is silently overwritten!
    fd = mch_fopen((ଅକ୍ଷର *)fname, "w");
    ଯଦି (fd == ଖାଲି)
    {
	semsg(_(e_cant_open_file_str), fname);
	ଫେରନ୍ତୁ;
    }

    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
				  _("Writing suggestion file %s..."), fname);
    spell_message(spin, IObuff);

    /*
     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
     */
    ଯଦି (fwrite(VIMSUGMAGIC, VIMSUGMAGICL, (size_t)1, fd) != 1) // <fileID>
    {
	emsg(_(e_error_while_writing));
	ଯାଅ theend;
    }
    putc(VIMSUGVERSION, fd);				// <versionnr>

    // Write si_sugtime to the file.
    put_time(fd, spin->si_sugtime);			// <timestamp>

    /*
     * <SUGWORDTREE>
     */
    spin->si_memtot = 0;
    ବୃକ୍ଷ = spin->si_foldroot->wn_sibling;

    // Clear the index and wnode fields in the tree.
    clear_ଗଣ୍ଠି(ବୃକ୍ଷ);

    // Count the number of nodes.  Needed to be able to allocate the
    // memory when reading the nodes.  Also fills in index for shared
    // nodes.
    nodecount = put_ଗଣ୍ଠି(ଖାଲି, ବୃକ୍ଷ, 0, 0, FALSE);

    // number of nodes in 4 bytes
    put_bytes(fd, (long_u)nodecount, 4);	// <nodecount>
    spin->si_memtot += nodecount + nodecount * ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

    // Write the nodes.
    (ଶୂନ୍ୟ)put_ଗଣ୍ଠି(fd, ବୃକ୍ଷ, 0, 0, FALSE);

    /*
     * <SUGTABLE>: <sugwcount> <sugline> ...
     */
    wcount = spin->si_spellbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    put_bytes(fd, (long_u)wcount, 4);	// <sugwcount>

    ପାଇଁ (lnum = 1; lnum <= (linenr_T)wcount; ++lnum)
    {
	// <sugline>: <sugnr> ... NUL
	ଧାଡ଼ି = ml_get_buf(spin->si_spellbuf, lnum, FALSE);
	len = ml_get_buf_len(spin->si_spellbuf, lnum) + 1;
	ଯଦି (fwrite(ଧାଡ଼ି, (size_t)len, (size_t)1, fd) == 0)
	{
	    emsg(_(e_error_while_writing));
	    ଯାଅ theend;
	}
	spin->si_memtot += len;
    }

    // Write another byte to check for errors.
    ଯଦି (putc(0, fd) == EOF)
	emsg(_(e_error_while_writing));

    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
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
    ଶୂନ୍ୟ
mkspell(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fcount,
    char_u	**fnames,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ascii,		    // -ascii argument given
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		over_write,	    // overwrite existing output file
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		added_word)	    // invoked through "zg"
{
    char_u	*fname = ଖାଲି;
    char_u	*wfname;
    char_u	**innames;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		incount;
    afffile_T	*(afile[MAXREGIONS]);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    stat_T	st;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    spellinfo_T spin;

    CLEAR_FIELD(spin);
    spin.si_verbose = !added_word;
    spin.si_ascii = ascii;
    spin.si_followup = TRUE;
    spin.si_rem_accents = TRUE;
    ga_init2(&spin.si_rep, ଆକାର(fromto_T), 20);
    ga_init2(&spin.si_repsal, ଆକାର(fromto_T), 20);
    ga_init2(&spin.si_sal, ଆକାର(fromto_T), 20);
    ga_init2(&spin.si_map, ଆକାର(char_u), 100);
    ga_init2(&spin.si_comppat, ଆକାର(char_u *), 20);
    ga_init2(&spin.si_prefcond, ଆକାର(char_u *), 50);
    hash_init(&spin.si_commonwords);
    spin.si_newcompID = 127;	// start compound ID at first maximum

    // default: fnames[0] is output file, following are input files
    // When "fcount" is 1 there is only one file.
    innames = &fnames[fcount == 1 ? 0 : 1];
    incount = fcount - 1;

    wfname = alloc(MAXPATHL);
    ଯଦି (wfname == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (fcount >= 1)
    {
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(fnames[0]);
	ଯଦି (fcount == 1 && len > 4 && STRCMP(fnames[0] + len - 4, ".add") == 0)
	{
	    // For ":mkspell path/en.latin1.add" output file is
	    // "path/en.latin1.add.spl".
	    incount = 1;
	    vim_snprintf((ଅକ୍ଷର *)wfname, MAXPATHL, "%s.spl", fnames[0]);
	}
	ଅନ୍ୟଥା ଯଦି (fcount == 1)
	{
	    // For ":mkspell path/vim" output file is "path/vim.latin1.spl".
	    incount = 1;
	    vim_snprintf((ଅକ୍ଷର *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());
	}
	ଅନ୍ୟଥା ଯଦି (len > 4 && STRCMP(fnames[0] + len - 4, ".spl") == 0)
	{
	    // Name ends in ".spl", use as the file name.
	    vim_strncpy(wfname, fnames[0], MAXPATHL - 1);
	}
	ଅନ୍ୟଥା
	    // Name should be language, make the file name from it.
	    vim_snprintf((ଅକ୍ଷର *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());

	// Check for .ascii.spl.
	ଯଦି (strstr((ଅକ୍ଷର *)gettail(wfname), SPL_FNAME_ASCII) != ଖାଲି)
	    spin.si_ascii = TRUE;

	// Check for .add.spl.
	ଯଦି (strstr((ଅକ୍ଷର *)gettail(wfname), SPL_FNAME_ADD) != ଖାଲି)
	    spin.si_add = TRUE;
    }

    ଯଦି (incount <= 0)
	emsg(_(e_invalid_argument));	// need at least output and input names
    ଅନ୍ୟଥା ଯଦି (vim_strchr(gettail(wfname), '_') != ଖାଲି)
	emsg(_(e_output_file_name_must_not_have_region_name));
    ଅନ୍ୟଥା ଯଦି (incount > MAXREGIONS)
	semsg(_(e_only_up_to_nr_regions_supported), MAXREGIONS);
    ଅନ୍ୟଥା
    {
	// Check for overwriting before doing things that may take a lot of
	// time.
	ଯଦି (!over_write && mch_stat((ଅକ୍ଷର *)wfname, &st) >= 0)
	{
	    emsg(_(e_file_exists));
	    ଯାଅ theend;
	}
	ଯଦି (mch_isdir(wfname))
	{
	    semsg(_(e_str_is_directory), wfname);
	    ଯାଅ theend;
	}

	fname = alloc(MAXPATHL);
	ଯଦି (fname == ଖାଲି)
	    ଯାଅ theend;

	/*
	 * Init the aff and dic pointers.
	 * Get the region names if there are more than 2 arguments.
	 */
	ପାଇଁ (i = 0; i < incount; ++i)
	{
	    afile[i] = ଖାଲି;

	    ଯଦି (incount > 1)
	    {
		len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(innames[i]);
		ଯଦି (STRLEN(gettail(innames[i])) < 5
						|| innames[i][len - 3] != '_')
		{
		    semsg(_(e_invalid_region_in_str), innames[i]);
		    ଯାଅ theend;
		}
		spin.si_region_name[i * 2] = TOLOWER_ASC(innames[i][len - 2]);
		spin.si_region_name[i * 2 + 1] =
					     TOLOWER_ASC(innames[i][len - 1]);
	    }
	}
	spin.si_region_ଗଣନା = incount;

	spin.si_foldroot = wordtree_alloc(&spin);
	spin.si_keeproot = wordtree_alloc(&spin);
	spin.si_prefroot = wordtree_alloc(&spin);
	ଯଦି (spin.si_foldroot == ଖାଲି
		|| spin.si_keeproot == ଖାଲି
		|| spin.si_prefroot == ଖାଲି)
	{
	    free_blocks(spin.si_blocks);
	    ଯାଅ theend;
	}

	// When not producing a .add.spl file clear the character table when
	// we encounter one in the .aff file.  This means we dump the current
	// one in the .spl file if the .aff file doesn't define one.  That's
	// better than guessing the contents, the table will match a
	// previously loaded spell file.
	ଯଦି (!spin.si_add)
	    spin.si_clear_chartab = TRUE;

	/*
	 * Read all the .aff and .dic files.
	 * Text is converted to 'encoding'.
	 * Words are stored in the case-folded and keep-case trees.
	 */
	ପାଇଁ (i = 0; i < incount && !error; ++i)
	{
	    spin.si_conv.vc_type = CONV_NONE;
	    spin.si_region = 1 << i;

	    vim_snprintf((ଅକ୍ଷର *)fname, MAXPATHL, "%s.aff", innames[i]);
	    ଯଦି (mch_stat((ଅକ୍ଷର *)fname, &st) >= 0)
	    {
		// Read the .aff file.  Will init "spin->si_conv" based on the
		// "SET" line.
		afile[i] = spell_read_aff(&spin, fname);
		ଯଦି (afile[i] == ଖାଲି)
		    error = TRUE;
		ଅନ୍ୟଥା
		{
		    // Read the .dic file and store the words in the trees.
		    vim_snprintf((ଅକ୍ଷର *)fname, MAXPATHL, "%s.dic",
								  innames[i]);
		    ଯଦି (spell_read_dic(&spin, fname, afile[i]) == FAIL)
			error = TRUE;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		// No .aff file, try reading the file as a word list.  Store
		// the words in the trees.
		ଯଦି (spell_read_wordfile(&spin, innames[i]) == FAIL)
		    error = TRUE;
	    }

	    // Free any conversion stuff.
	    convert_setup(&spin.si_conv, ଖାଲି, ଖାଲି);
	}

	ଯଦି (spin.si_compflags != ଖାଲି && spin.si_nobreak)
	    ସନ୍ଦେଶ(_("Warning: both compounding and NOBREAK specified"));

	ଯଦି (!error && !got_int)
	{
	    /*
	     * Combine tails in the tree.
	     */
	    spell_message(&spin, (char_u *)_(ସନ୍ଦେଶ_compressing));
	    wordtree_compress(&spin, spin.si_foldroot, "case-folded");
	    wordtree_compress(&spin, spin.si_keeproot, "keep-case");
	    wordtree_compress(&spin, spin.si_prefroot, "prefixes");
	}

	ଯଦି (!error && !got_int)
	{
	    /*
	     * Write the info in the spell file.
	     */
	    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
				      _("Writing spell file %s..."), wfname);
	    spell_message(&spin, IObuff);

	    error = write_vim_spell(&spin, wfname) == FAIL;

	    spell_message(&spin, (char_u *)_("Done!"));
	    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin.si_memtot);
	    spell_message(&spin, IObuff);

	    /*
	     * If the file is loaded need to reload it.
	     */
	    ଯଦି (!error)
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
	ପାଇଁ (i = 0; i < incount; ++i)
	    ଯଦି (afile[i] != ଖାଲି)
		spell_free_aff(afile[i]);

	// Free all the bits and pieces at once.
	free_blocks(spin.si_blocks);

	/*
	 * If there is soundfolding info and no NOSUGFILE item create the
	 * .sug file with the soundfolded word trie.
	 */
	ଯଦି (spin.si_sugtime != 0 && !error && !got_int)
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
    ସ୍ଥିର ଶୂନ୍ୟ
spell_message(spellinfo_T *spin, char_u *str)
{
    ଯଦି (spin->si_verbose || p_verbose > 2)
    {
	ଯଦି (!spin->si_verbose)
	    verbose_enter();
	ସନ୍ଦେଶ((ଅକ୍ଷର *)str);
	out_flush();
	ଯଦି (!spin->si_verbose)
	    verbose_leave();
    }
}

/*
 * ":[count]spellgood  {word}"
 * ":[count]spellwrong {word}"
 * ":[count]spellundo  {word}"
 * ":[count]spellrare  {word}"
 */
    ଶୂନ୍ୟ
ex_spell(exarg_T *eap)
{
    spell_add_word(eap->arg, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(eap->arg),
		eap->cmdidx == CMD_spellwrong ? SPELL_ADD_BAD :
		eap->cmdidx == CMD_spellrare ? SPELL_ADD_RARE : SPELL_ADD_GOOD,
				   eap->forceit ? 0 : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line2,
				   eap->cmdidx == CMD_spellundo);
}

/*
 * Add "word[len]" to 'spellfile' as a good, rare or bad word.
 */
    ଶୂନ୍ୟ
spell_add_word(
    char_u	*word,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		what,	    // SPELL_ADD_ values
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,	    // "zG" and "zW": zero, otherwise index in
			    // 'spellfile'
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		undo)	    // TRUE for "zug", "zuG", "zuw" and "zuW"
{
    FILE	*fd = ଖାଲି;
    buf_T	*buf = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_spf = FALSE;
    char_u	*fname;
    char_u	*fnamebuf = ଖାଲି;
    char_u	ଧାଡ଼ି[MAXWLEN * 2];
    long	fpos, fpos_next = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    size_t	linelen;
    char_u	*spf;

    ଯଦି (!valid_spell_word(word, word + len))
    {
	emsg(_(e_illegal_character_in_word));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (idx == 0)	    // use internal wordlist
    {
	ଯଦି (int_wordlist == ଖାଲି)
	{
	    int_wordlist = vim_tempname('s', FALSE);
	    ଯଦି (int_wordlist == ଖାଲି)
		ଫେରନ୍ତୁ;
	}
	fname = int_wordlist;
    }
    ଅନ୍ୟଥା
    {
	// If 'spellfile' isn't set figure out a good default value.
	ଯଦି (*curwin->w_s->b_p_spf == NUL)
	{
	    init_spellfile();
	    new_spf = TRUE;
	}

	ଯଦି (*curwin->w_s->b_p_spf == NUL)
	{
	    semsg(_(e_option_str_is_not_set), "spellfile");
	    ଫେରନ୍ତୁ;
	}
	fnamebuf = alloc(MAXPATHL);
	ଯଦି (fnamebuf == ଖାଲି)
	    ଫେରନ୍ତୁ;

	ପାଇଁ (spf = curwin->w_s->b_p_spf, i = 1; *spf != NUL; ++i)
	{
	    copy_option_part(&spf, fnamebuf, MAXPATHL, ",");
	    ଯଦି (i == idx)
		ଭାଙ୍ଗ;
	    ଯଦି (*spf == NUL)
	    {
		semsg(_(e_spellfile_does_not_have_nr_entries), idx);
		vim_free(fnamebuf);
		ଫେରନ୍ତୁ;
	    }
	}

	// Check that the user isn't editing the .add file somewhere.
	buf = buflist_findname_exp(fnamebuf);
	ଯଦି (buf != ଖାଲି && buf->b_ml.ml_mfp == ଖାଲି)
	    buf = ଖାଲି;
	ଯଦି (buf != ଖାଲି && bufIsChanged(buf))
	{
	    emsg(_(e_file_is_loaded_in_another_buffer));
	    vim_free(fnamebuf);
	    ଫେରନ୍ତୁ;
	}

	fname = fnamebuf;
    }

    ଯଦି (what == SPELL_ADD_BAD || undo)
    {
	// When the word appears as good word we need to remove that one,
	// since its flags sort before the one with WF_BANNED.
	fd = mch_fopen((ଅକ୍ଷର *)fname, "r");
	ଯଦି (fd != ଖାଲି)
	{
	    ଯେତେବେଳେ (!vim_fgets(ଧାଡ଼ି, MAXWLEN * 2, fd))
	    {
		fpos = fpos_next;
		fpos_next = ftell(fd);
		ଯଦି (fpos_next < 0)
		    ଭାଙ୍ଗ;  // should never happen
		linelen = STRLEN(ଧାଡ଼ି);
		ଯଦି (linelen >= (size_t)len
			&& STRNCMP(word, ଧାଡ଼ି, len) == 0
			&& (ଧାଡ଼ି[len] == '/' || ଧାଡ଼ି[len] < ' '))
		{
		    // Found duplicate word.  Remove it by writing a '#' at
		    // the start of the line.  Mixing reading and writing
		    // doesn't work for all systems, close the file first.
		    fclose(fd);
		    fd = mch_fopen((ଅକ୍ଷର *)fname, "r+");
		    ଯଦି (fd == ଖାଲି)
			ଭାଙ୍ଗ;
		    ଯଦି (fseek(fd, fpos, SEEK_SET) == 0)
		    {
			fputc('#', fd);
			ଯଦି (undo)
			{
			    home_replace(ଖାଲି, fname, NameBuff, MAXPATHL, TRUE);
			    smsg(_("Word '%.*s' removed from %s"),
							 len, word, NameBuff);
			}
		    }
		    ଯଦି (fseek(fd, fpos_next, SEEK_SET) != 0)
		    {
			PERROR(_("Seek error in spellfile"));
			ଭାଙ୍ଗ;
		    }
		}
	    }
	    ଯଦି (fd != ଖାଲି)
		fclose(fd);
	}
    }

    ଯଦି (!undo)
    {
	fd = mch_fopen((ଅକ୍ଷର *)fname, "a");
	ଯଦି (fd == ଖାଲି && new_spf)
	{
	    char_u *p;

	    // We just initialized the 'spellfile' option and can't open the
	    // file.  We may need to create the "spell" directory first.  We
	    // already checked the runtime directory is writable in
	    // init_spellfile().
	    ଯଦି (!dir_of_file_exists(fname) && (p = gettail_sep(fname)) != fname)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = *p;

		// The directory doesn't exist.  Try creating it and opening
		// the file again.
		*p = NUL;
		vim_mkdir(fname, 0755);
		*p = c;
		fd = mch_fopen((ଅକ୍ଷର *)fname, "a");
	    }
	}

	ଯଦି (fd == ଖାଲି)
	    semsg(_(e_cant_open_file_str), fname);
	ଅନ୍ୟଥା
	{
	    ଯଦି (what == SPELL_ADD_BAD)
		fprintf(fd, "%.*s/!\n", len, word);
	    ଅନ୍ୟଥା ଯଦି (what == SPELL_ADD_RARE)
		fprintf(fd, "%.*s/?\n", len, word);
	    ଅନ୍ୟଥା
		fprintf(fd, "%.*s\n", len, word);
	    fclose(fd);

	    home_replace(ଖାଲି, fname, NameBuff, MAXPATHL, TRUE);
	    smsg(_("Word '%.*s' added to %s"), len, word, NameBuff);
	}
    }

    ଯଦି (fd != ଖାଲି)
    {
	// Update the .add.spl file.
	mkspell(1, &fname, FALSE, TRUE, TRUE);

	// If the .add file is edited somewhere, reload it.
	ଯଦି (buf != ଖାଲି)
	    buf_reload(buf, buf->b_orig_mode, FALSE);

	redraw_all_later(UPD_SOME_VALID);
    }
    vim_free(fnamebuf);
}

/*
 * Initialize 'spellfile' for the current buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
init_spellfile(ଶୂନ୍ୟ)
{
    string_T	buf;
    char_u	*fname;
    char_u	*rtp;
    char_u	*lend;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		aspath = FALSE;
    char_u	*lstart = curbuf->b_s.b_p_spl;

    ଯଦି (*curwin->w_s->b_p_spl == NUL || curwin->w_s->b_langp.ga_len <= 0)
	ଫେରନ୍ତୁ;

    buf.string = alloc(MAXPATHL);
    ଯଦି (buf.string == ଖାଲି)
	ଫେରନ୍ତୁ;

    buf.length = 0;

    // Find the end of the language name.  Exclude the region.  If there
    // is a path separator remember the start of the tail.
    ପାଇଁ (lend = curwin->w_s->b_p_spl; *lend != NUL
	    && vim_strchr((char_u *)",._", *lend) == ଖାଲି; ++lend)
	ଯଦି (vim_ispathsep(*lend))
	{
	    aspath = TRUE;
	    lstart = lend + 1;
	}

    // Loop over all entries in 'runtimepath'.  Use the first one where we
    // are allowed to write.
    rtp = p_rtp;
    ଯେତେବେଳେ (*rtp != NUL)
    {
	ଯଦି (aspath)
	    // Use directory of an entry with path, e.g., for
	    // "/dir/lg.utf-8.spl" use "/dir".
	    vim_strncpy(buf.string, curbuf->b_s.b_p_spl,
		    lstart - curbuf->b_s.b_p_spl - 1);
	ଅନ୍ୟଥା
	    // Copy the path from 'runtimepath' to buf[].
	    buf.length = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");
	ଯଦି (filewritable(buf.string) == 2)
	{
	    // Use the first language name from 'spelllang' and the
	    // encoding used in the first loaded .spl file.
	    ଯଦି (aspath)
	    {
		buf.length = (size_t)(lend - curbuf->b_s.b_p_spl);
		vim_strncpy(buf.string, curbuf->b_s.b_p_spl, buf.length);
	    }
	    ଅନ୍ୟଥା
	    {
		// Create the "spell" directory if it doesn't exist yet.
		buf.length += vim_snprintf_safelen((ଅକ୍ଷର *)buf.string + buf.length,
		    MAXPATHL - buf.length, "/spell");
		ଯଦି (filewritable(buf.string) != 2)
		{
		    ଯଦି (vim_mkdir(buf.string, 0755) != 0)
		    {
			vim_free(buf.string);
			ଫେରନ୍ତୁ;
		    }
		}

		buf.length += vim_snprintf_safelen((ଅକ୍ଷର *)buf.string + buf.length,
		    MAXPATHL - buf.length, "/%.*s", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(lend - lstart), lstart);
	    }
	    fname = LANGP_ENTRY(curwin->w_s->b_langp, 0)
		->lp_slang->sl_fname;
	    vim_snprintf((ଅକ୍ଷର *)buf.string + buf.length, MAXPATHL - buf.length,
		".%s.add",
		fname != ଖାଲି && strstr((ଅକ୍ଷର *)gettail(fname), ".ascii.") != ଖାଲି
		    ? (char_u *)"ascii" : spell_enc());
	    set_option_ମୂଲ୍ୟ_give_err((char_u *)"spellfile",
		    0L, buf.string, OPT_LOCAL);
	    ଭାଙ୍ଗ;
	}
	aspath = FALSE;
    }

    vim_free(buf.string);
}



/*
 * Set the spell character tables from strings in the affix file.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_spell_chartab(char_u *fol, char_u *ତଳ, char_u *upp)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    char_u	*pf = fol, *pl = ତଳ, *pu = upp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		f, l, u;

    clear_spell_chartab(&new_st);

    ଯେତେବେଳେ (*pf != NUL)
    {
	ଯଦି (*pl == NUL || *pu == NUL)
	{
	    emsg(_(e_format_error_in_affix_file_fol_ତଳ_or_upp));
	    ଫେରନ୍ତୁ FAIL;
	}
	f = mb_ptr2char_adv(&pf);
	l = mb_ptr2char_adv(&pl);
	u = mb_ptr2char_adv(&pu);

	// Every character that appears is a word character.
	ଯଦି (f < 256)
	    new_st.st_isw[f] = TRUE;
	ଯଦି (l < 256)
	    new_st.st_isw[l] = TRUE;
	ଯଦି (u < 256)
	    new_st.st_isw[u] = TRUE;

	// if "LOW" and "FOL" are not the same the "LOW" char needs
	// case-folding
	ଯଦି (l < 256 && l != f)
	{
	    ଯଦି (f >= 256)
	    {
		emsg(_(e_character_in_fol_ତଳ_or_upp_is_out_of_range));
		ଫେରନ୍ତୁ FAIL;
	    }
	    new_st.st_fold[l] = f;
	}

	// if "UPP" and "FOL" are not the same the "UPP" char needs
	// case-folding, it's upper case and the "UPP" is the upper case of
	// "FOL" .
	ଯଦି (u < 256 && u != f)
	{
	    ଯଦି (f >= 256)
	    {
		emsg(_(e_character_in_fol_ତଳ_or_upp_is_out_of_range));
		ଫେରନ୍ତୁ FAIL;
	    }
	    new_st.st_fold[u] = f;
	    new_st.st_isu[u] = TRUE;
	    new_st.st_upper[f] = u;
	}
    }

    ଯଦି (*pl != NUL || *pu != NUL)
    {
	emsg(_(e_format_error_in_affix_file_fol_ତଳ_or_upp));
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ set_spell_finish(&new_st);
}

/*
 * Set the spell character tables from strings in the .spl file.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_spell_charflags(
    char_u	*flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cnt,	    // length of "flags"
    char_u	*fol)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*p = fol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    clear_spell_chartab(&new_st);

    ପାଇଁ (i = 0; i < 128; ++i)
    {
	ଯଦି (i < cnt)
	{
	    new_st.st_isw[i + 128] = (flags[i] & CF_WORD) != 0;
	    new_st.st_isu[i + 128] = (flags[i] & CF_UPPER) != 0;
	}

	ଯଦି (*p != NUL)
	{
	    c = mb_ptr2char_adv(&p);
	    new_st.st_fold[i + 128] = c;
	    ଯଦି (i + 128 != c && new_st.st_isu[i + 128] && c < 256)
		new_st.st_upper[c] = i + 128;
	}
    }

    (ଶୂନ୍ୟ)set_spell_finish(&new_st);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_spell_finish(spelltab_T *new_st)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (did_set_spelltab)
    {
	// check that it's the same table
	ପାଇଁ (i = 0; i < 256; ++i)
	{
	    ଯଦି (spelltab.st_isw[i] != new_st->st_isw[i]
		    || spelltab.st_isu[i] != new_st->st_isu[i]
		    || spelltab.st_fold[i] != new_st->st_fold[i]
		    || spelltab.st_upper[i] != new_st->st_upper[i])
	    {
		emsg(_(e_word_characters_differ_between_spell_files));
		ଫେରନ୍ତୁ FAIL;
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	// copy the new spelltab into the one being used
	spelltab = *new_st;
	did_set_spelltab = TRUE;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Write the table with prefix conditions to the .spl file.
 * When "fd" is NULL only count the length of what is written and return it.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		totlen;

    ଯଦି (fd != ଖାଲି)
	put_bytes(fd, (long_u)gap->ga_len, 2);	    // <prefcondcnt>

    totlen = 2 + gap->ga_len; // length of <prefcondcnt> and <condlen> bytes

    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	p = ((char_u **)gap->ga_data)[i];
	ଯଦି (p != ଖାଲି)
	{
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);
	    ଯଦି (fd != ଖାଲି)
	    {
		fputc(len, fd);
		*fwv &= fwrite(p, (size_t)len, (size_t)1, fd);
	    }
	    totlen += len;
	}
	ଅନ୍ୟଥା ଯଦି (fd != ଖାଲି)
	    fputc(0, fd);
    }

    ଫେରନ୍ତୁ totlen;
}

/*
 * Use map string "map" for languages "lp".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_map_str(slang_T *lp, char_u *map)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		headc = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (*map == NUL)
    {
	lp->sl_has_map = FALSE;
	ଫେରନ୍ତୁ;
    }
    lp->sl_has_map = TRUE;

    // Init the array and hash tables empty.
    ପାଇଁ (i = 0; i < 256; ++i)
	lp->sl_map_array[i] = 0;
    hash_init(&lp->sl_map_hash);

    /*
     * The similar characters are stored separated with slashes:
     * "aaa/bbb/ccc/".  Fill sl_map_array[c] with the character before c and
     * before the same slash.  For characters above 255 sl_map_hash is used.
     */
    ପାଇଁ (p = map; *p != NUL; )
    {
	c = mb_cptr2char_adv(&p);
	ଯଦି (c == '/')
	    headc = 0;
	ଅନ୍ୟଥା
	{
	    ଯଦି (headc == 0)
		 headc = c;

	    // Characters above 255 don't fit in sl_map_array[], put them in
	    // the hash table.  Each entry is the char, a NUL the headchar and
	    // a NUL.
	    ଯଦି (c >= 256)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cl = mb_char2len(c);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    headcl = mb_char2len(headc);
		char_u	    *b;
		hash_T	    hash;
		hashitem_T  *hi;

		b = alloc(cl + headcl + 2);
		ଯଦି (b == ଖାଲି)
		    ଫେରନ୍ତୁ;
		mb_char2bytes(c, b);
		b[cl] = NUL;
		mb_char2bytes(headc, b + cl + 1);
		b[cl + 1 + headcl] = NUL;
		hash = hash_hash(b);
		hi = hash_lookup(&lp->sl_map_hash, b, hash);
		ଯଦି (HASHITEM_EMPTY(hi))
		    hash_add_item(&lp->sl_map_hash, hi, b, hash);
		ଅନ୍ୟଥା
		{
		    // This should have been checked when generating the .spl
		    // file.
		    emsg(_(e_duplicate_char_in_map_entry));
		    vim_free(b);
		}
	    }
	    ଅନ୍ୟଥା
		lp->sl_map_array[c] = headc;
	}
    }
}

#endif  // FEAT_SPELL
