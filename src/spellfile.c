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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_SPELL)

#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>	// for time_t

// Special byte values for <byte>.  Some are only used in the tree for
// postponed prefixes, some only in the other trees.  This is a bit messy...
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BY_NOFLAGS	0	// end of word without flags or region; for
				// postponed prefix: no <pflags>
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BY_ਸੂਚਕਾਂਕ	1	// child is shared, index follows
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BY_FLAGS	2	// end of word, <flags> byte follows; for
				// postponed prefix: <pflags> follows
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BY_FLAGS2	3	// end of word, <flags> and <flags2> bytes
				// follow; never used in prefix tree
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BY_SPECIAL  BY_FLAGS2	// highest special byte value

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ZERO_FLAG	65009	// used when flag is zero: "0"

// Flags used in .spl file for soundsalike flags.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAL_F0LLOWUP		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAL_COLLAPSE		2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAL_REM_ACCENTS		4

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIMSPELLMAGIC "VIMspell"  // string at start of Vim spell file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIMSPELLMAGICL 8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIMSPELLVERSION 50

// Section IDs.  Only renumber them when VIMSPELLVERSION changes!
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_REGION	0	// <regionname> section
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_CHARFLAGS	1	// charflags section
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_MIDWORD	2	// <midword> section
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_PREFCOND	3	// <prefcond> section
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_REP		4	// REP items section
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_SAL		5	// SAL items section
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_SOFO		6	// soundfolding section
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_MAP		7	// MAP items section
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_COMPOUND	8	// compound words section
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_SYLLABLE	9	// syllable section
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_NOBREAK	10	// NOBREAK section
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_SUGFILE	11	// timestamp for .sug file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_REPSAL	12	// REPSAL items section
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_ਸ਼ਬਦ	13	// common words
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_NOSPLITSUGS	14	// don't split word for suggestions
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_INFO		15	// info section
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_NOCOMPOUNDSUGS 16	// don't compound for suggestions
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SN_ਸਿਰਾ		255	// end of sections

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SNF_REQUIRED	1	// <sectionflags>: required section

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CF_ਸ਼ਬਦ		0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CF_ਉੱਤਲਾ	0x02

// Max allowed length for COMPOUND section
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMPOUND_MAX_LEN	100000

/*
 * Loop through all the siblings of a node (including the node)
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_ਗੰਢ_SIBLINGS(ਗੰਢ, np) \
    ਲਈ ((np) = (ਗੰਢ); (np) != ਨਿਰਾ; (np) = (np)->wn_sibling)

ਸਥਿਰ ਪੂਰਨ ਸੈੱਟ_spell_ਅੰਤ(spelltab_T	*new_st);
ਸਥਿਰ ਪੂਰਨ write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv);
ਸਥਿਰ ਪੂਰਨ read_region_section(FILE *fd, slang_T *slang, ਪੂਰਨ len);
ਸਥਿਰ ਪੂਰਨ read_charflags_section(FILE *fd);
ਸਥਿਰ ਪੂਰਨ read_prefcond_section(FILE *fd, slang_T *lp);
ਸਥਿਰ ਪੂਰਨ read_rep_section(FILE *fd, garray_T *gap, ਛੋਟਾ *ਪਹਿਲਾ);
ਸਥਿਰ ਪੂਰਨ read_sal_section(FILE *fd, slang_T *slang);
ਸਥਿਰ ਪੂਰਨ read_ਸ਼ਬਦ_section(FILE *fd, slang_T *lp, ਪੂਰਨ len);
ਸਥਿਰ ਪੂਰਨ read_sofo_section(FILE *fd, slang_T *slang);
ਸਥਿਰ ਪੂਰਨ read_compound(FILE *fd, slang_T *slang, ਪੂਰਨ len);
ਸਥਿਰ ਪੂਰਨ ਸੈੱਟ_sofo(slang_T *lp, char_u *from, char_u *ਮੰਜ਼ਿਲ);
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_sal_ਪਹਿਲਾ(slang_T *lp);
ਸਥਿਰ ਪੂਰਨ *mb_str2wide(char_u *s);
ਸਥਿਰ ਪੂਰਨ spell_read_ਰੁੱਖ(FILE *fd, char_u **bytsp, ਲੰਮਾ *bytsp_len, idx_T **idxsp, ਪੂਰਨ prefixtree, ਪੂਰਨ prefixcnt);
ਸਥਿਰ idx_T read_ਰੁੱਖ_ਗੰਢ(FILE *fd, char_u *byts, idx_T *idxs, ਪੂਰਨ maxidx, idx_T startidx, ਪੂਰਨ prefixtree, ਪੂਰਨ maxprefcondnr, ਪੂਰਨ ਡੂੰਘਾਈ);
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_spell_charflags(char_u *flags, ਪੂਰਨ cnt, char_u *upp);
ਸਥਿਰ ਪੂਰਨ ਸੈੱਟ_spell_chartab(char_u *fol, char_u *ਨੀਵਾਂ, char_u *upp);
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_map_str(slang_T *lp, char_u *map);


ਸਥਿਰ ਅੱਖਰ *e_afftrailing = N_("Trailing text in %s line %d: %s");
ਸਥਿਰ ਅੱਖਰ *e_affname = N_("Affix name too long in %s line %d: %s");
ਸਥਿਰ ਅੱਖਰ *ਸੁਨੇਹਾ_compressing = N_("Compressing word tree...");

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
spell_ਲੱਦੋ_file(
    char_u	*fname,
    char_u	*lang,
    slang_T	*old_lp,
    ਪੂਰਨ		silent)		// no error if file doesn't exist
{
    FILE	*fd;
    char_u	buf[VIMSPELLMAGICL];
    char_u	*p;
    ਪੂਰਨ		i;
    ਪੂਰਨ		n;
    ਪੂਰਨ		len;
    slang_T	*lp = ਨਿਰਾ;
    ਪੂਰਨ		c = 0;
    ਪੂਰਨ		res;
    ਪੂਰਨ		did_estack_ਧੱਕੋ = FALSE;
    ESTACK_CHECK_DECLARATION;

    fd = mch_fopen((ਅੱਖਰ *)fname, "r");
    ਜੇ (fd == ਨਿਰਾ)
    {
	ਜੇ (!silent)
	    semsg(_(e_cant_open_file_str), fname);
	ਨਹੀਂਤਾਂ ਜੇ (p_verbose > 2)
	{
	    verbose_enter();
	    smsg((ਅਚਲ ਅੱਖਰ *)e_cant_open_file_str, fname);
	    verbose_leave();
	}
	ਜਾਓ ਸਿਰਾ_FAIL;
    }
    ਜੇ (p_verbose > 2)
    {
	verbose_enter();
	smsg(_("Reading spell file \"%s\""), fname);
	verbose_leave();
    }

    ਜੇ (old_lp == ਨਿਰਾ)
    {
	lp = slang_alloc(lang);
	ਜੇ (lp == ਨਿਰਾ)
	    ਜਾਓ ਸਿਰਾ_FAIL;

	// Remember the file name, used to reload the file when it's updated.
	lp->sl_fname = vim_strsave(fname);
	ਜੇ (lp->sl_fname == ਨਿਰਾ)
	    ਜਾਓ ਸਿਰਾ_FAIL;

	// Check for .add.spl (_add.spl for VMS).
	lp->sl_ਜੋੜੋ = ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)gettail(fname), SPL_FNAME_ਜੋੜੋ) != ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
	lp = old_lp;

    // Set sourcing_name, so that error messages mention the file name.
    estack_ਧੱਕੋ(ETYPE_SPELL, fname, 0);
    ESTACK_CHECK_ਤਿਆਰੀ;
    did_estack_ਧੱਕੋ = TRUE;

    /*
     * <HEADER>: <fileID>
     */
    ਲਈ (i = 0; i < VIMSPELLMAGICL; ++i)
	buf[i] = (c = getc(fd)) == EOF ? 0 : c;		// <fileID>
    ਜੇ (STRNCMP(buf, VIMSPELLMAGIC, VIMSPELLMAGICL) != 0)
    {
	emsg(_(e_this_does_not_look_like_spell_file));
	ਜਾਓ ਸਿਰਾ_FAIL;
    }
    c = getc(fd);					// <versionnr>
    ਜੇ (c < VIMSPELLVERSION)
    {
	emsg(_(e_old_spell_file_needs_ਮੰਜ਼ਿਲ_be_updated));
	ਜਾਓ ਸਿਰਾ_FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (c > VIMSPELLVERSION)
    {
	emsg(_(e_spell_file_is_for_newer_ਸੰਸਕਰਣ_ਦਾ_vim));
	ਜਾਓ ਸਿਰਾ_FAIL;
    }


    /*
     * <SECTIONS>: <section> ... <sectionend>
     * <section>: <sectionID> <sectionflags> <sectionlen> (section contents)
     */
    ਲਈ (;;)
    {
	n = getc(fd);			    // <sectionID> or <sectionend>
	ਜੇ (n == SN_ਸਿਰਾ)
	    ਰੋਕੋ;
	c = getc(fd);					// <sectionflags>
	len = get4c(fd);				// <sectionlen>
	ਜੇ (len < 0)
	    ਜਾਓ truncerr;

	res = 0;
	ਚੋਣ (n)
	{
	    ਹਾਲਤ SN_INFO:
		lp->sl_info = read_string(fd, len);	// <infotext>
		ਜੇ (lp->sl_info == ਨਿਰਾ)
		    ਜਾਓ ਸਿਰਾ_FAIL;
		ਰੋਕੋ;

	    ਹਾਲਤ SN_REGION:
		res = read_region_section(fd, lp, len);
		ਰੋਕੋ;

	    ਹਾਲਤ SN_CHARFLAGS:
		res = read_charflags_section(fd);
		ਰੋਕੋ;

	    ਹਾਲਤ SN_MIDWORD:
		lp->sl_midword = read_string(fd, len);	// <midword>
		ਜੇ (lp->sl_midword == ਨਿਰਾ)
		    ਜਾਓ ਸਿਰਾ_FAIL;
		ਰੋਕੋ;

	    ਹਾਲਤ SN_PREFCOND:
		res = read_prefcond_section(fd, lp);
		ਰੋਕੋ;

	    ਹਾਲਤ SN_REP:
		res = read_rep_section(fd, &lp->sl_rep, lp->sl_rep_ਪਹਿਲਾ);
		ਰੋਕੋ;

	    ਹਾਲਤ SN_REPSAL:
		res = read_rep_section(fd, &lp->sl_repsal, lp->sl_repsal_ਪਹਿਲਾ);
		ਰੋਕੋ;

	    ਹਾਲਤ SN_SAL:
		res = read_sal_section(fd, lp);
		ਰੋਕੋ;

	    ਹਾਲਤ SN_SOFO:
		res = read_sofo_section(fd, lp);
		ਰੋਕੋ;

	    ਹਾਲਤ SN_MAP:
		p = read_string(fd, len);		// <mapstr>
		ਜੇ (p == ਨਿਰਾ)
		    ਜਾਓ ਸਿਰਾ_FAIL;
		ਸੈੱਟ_map_str(lp, p);
		vim_free(p);
		ਰੋਕੋ;

	    ਹਾਲਤ SN_ਸ਼ਬਦ:
		res = read_ਸ਼ਬਦ_section(fd, lp, len);
		ਰੋਕੋ;

	    ਹਾਲਤ SN_SUGFILE:
		lp->sl_sugtime = get8ctime(fd);		// <timestamp>
		ਰੋਕੋ;

	    ਹਾਲਤ SN_NOSPLITSUGS:
		lp->sl_nosplitsugs = TRUE;
		ਰੋਕੋ;

	    ਹਾਲਤ SN_NOCOMPOUNDSUGS:
		lp->sl_nocompoundsugs = TRUE;
		ਰੋਕੋ;

	    ਹਾਲਤ SN_COMPOUND:
		res = read_compound(fd, lp, len);
		ਰੋਕੋ;

	    ਹਾਲਤ SN_NOBREAK:
		lp->sl_nobreak = TRUE;
		ਰੋਕੋ;

	    ਹਾਲਤ SN_SYLLABLE:
		lp->sl_syllable = read_string(fd, len);	// <syllable>
		ਜੇ (lp->sl_syllable == ਨਿਰਾ)
		    ਜਾਓ ਸਿਰਾ_FAIL;
		ਜੇ (init_syl_tab(lp) != OK)
		    ਜਾਓ ਸਿਰਾ_FAIL;
		ਰੋਕੋ;

	    ਮੂਲ:
		// Unsupported section.  When it's required give an error
		// message.  When it's not required skip the contents.
		ਜੇ (c & SNF_REQUIRED)
		{
		    emsg(_(e_unsupported_section_ਵਿੱਚ_spell_file));
		    ਜਾਓ ਸਿਰਾ_FAIL;
		}
		ਜਦੋਂ (--len >= 0)
		    ਜੇ (getc(fd) < 0)
			ਜਾਓ truncerr;
		ਰੋਕੋ;
	}
someerror:
	ਜੇ (res == SP_FORMERROR)
	{
	    emsg(_(e_format_error_ਵਿੱਚ_spell_file));
	    ਜਾਓ ਸਿਰਾ_FAIL;
	}
	ਜੇ (res == SP_TRUNCERROR)
	{
truncerr:
	    emsg(_(e_truncated_spell_file));
	    ਜਾਓ ਸਿਰਾ_FAIL;
	}
	ਜੇ (res == SP_OTHERERROR)
	    ਜਾਓ ਸਿਰਾ_FAIL;
    }

    // <LWORDTREE>
    res = spell_read_ਰੁੱਖ(fd, &lp->sl_fbyts, &lp->sl_fbyts_len,
						      &lp->sl_fidxs, FALSE, 0);
    ਜੇ (res != 0)
	ਜਾਓ someerror;

    // <KWORDTREE>
    res = spell_read_ਰੁੱਖ(fd, &lp->sl_kbyts, ਨਿਰਾ, &lp->sl_kidxs, FALSE, 0);
    ਜੇ (res != 0)
	ਜਾਓ someerror;

    // <PREFIXTREE>
    res = spell_read_ਰੁੱਖ(fd, &lp->sl_pbyts, ਨਿਰਾ, &lp->sl_pidxs, TRUE,
							    lp->sl_prefixcnt);
    ਜੇ (res != 0)
	ਜਾਓ someerror;

    // For a new file link it in the list of spell files.
    ਜੇ (old_lp == ਨਿਰਾ && lang != ਨਿਰਾ)
    {
	lp->sl_ਅਗਲਾ = ਪਹਿਲਾ_lang;
	ਪਹਿਲਾ_lang = lp;
    }

    ਜਾਓ ਸਿਰਾ_OK;

ਸਿਰਾ_FAIL:
    ਜੇ (lang != ਨਿਰਾ)
	// truncating the name signals the error to spell_load_lang()
	*lang = NUL;
    ਜੇ (lp != ਨਿਰਾ && old_lp == ਨਿਰਾ)
	slang_free(lp);
    lp = ਨਿਰਾ;

ਸਿਰਾ_OK:
    ਜੇ (fd != ਨਿਰਾ)
	fclose(fd);
    ਜੇ (did_estack_ਧੱਕੋ)
    {
	ESTACK_CHECK_NOW;
	estack_ਕੱਢੋ();
    }

    ਵਾਪਸ lp;
}

/*
 * Fill in the wordcount fields for a trie.
 * Returns the total number of words.
 */
    ਸਥਿਰ ਖਾਲੀ
ਰੁੱਖ_ਗਿਣਤੀ_ਸ਼ਬਦ(char_u *byts, ਲੰਮਾ byts_len, idx_T *idxs)
{
    ਪੂਰਨ		ਡੂੰਘਾਈ;
    idx_T	arridx[MAXWLEN];
    ਪੂਰਨ		curi[MAXWLEN];
    ਪੂਰਨ		c;
    idx_T	n;
    ਪੂਰਨ		wordcount[MAXWLEN];

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;
    ਡੂੰਘਾਈ = 0;
    ਜਦੋਂ (ਡੂੰਘਾਈ >= 0 && !got_int)
    {
	ਜੇ (curi[ਡੂੰਘਾਈ] > byts[arridx[ਡੂੰਘਾਈ]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[ਡੂੰਘਾਈ]] = wordcount[ਡੂੰਘਾਈ];
	    ਜੇ (ਡੂੰਘਾਈ > 0)
		wordcount[ਡੂੰਘਾਈ - 1] += wordcount[ਡੂੰਘਾਈ];

	    --ਡੂੰਘਾਈ;
	    fast_breakcheck();
	}
	ਨਹੀਂਤਾਂ
	{
	    // Do one more byte at this node.
	    n = arridx[ਡੂੰਘਾਈ] + curi[ਡੂੰਘਾਈ];
	    ++curi[ਡੂੰਘਾਈ];

	    c = byts[n];
	    ਜੇ (c == 0)
	    {
		// End of word, count it.
		++wordcount[ਡੂੰਘਾਈ];

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		ਜਦੋਂ (n + 1 < byts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[ਡੂੰਘਾਈ];
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਡੂੰਘਾਈ < MAXWLEN - 1)
	    {
		// Normal char, go one level deeper to count the words.
		++ਡੂੰਘਾਈ;
		arridx[ਡੂੰਘਾਈ] = idxs[n];
		curi[ਡੂੰਘਾਈ] = 1;
		wordcount[ਡੂੰਘਾਈ] = 0;
	    }
	}
    }
}

/*
 * Load the .sug files for languages that have one and weren't loaded yet.
 */
    ਖਾਲੀ
suggest_ਲੱਦੋ_files(ਖਾਲੀ)
{
    langp_T	*lp;
    ਪੂਰਨ		lpi;
    slang_T	*slang;
    char_u	*dotp;
    FILE	*fd;
    char_u	buf[MAXWLEN];
    ਪੂਰਨ		i;
    time_t	timestamp;
    ਪੂਰਨ		wcount;
    ਪੂਰਨ		wordnr;
    garray_T	ga;
    ਪੂਰਨ		c;

    // Do this for all languages that support sound folding.
    ਲਈ (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ਇੰਦਰਾਜ(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ਜੇ (slang->sl_sugtime != 0 && !slang->sl_sugloaded)
	{
	    // Change ".spl" to ".sug" and open the file.  When the file isn't
	    // found silently skip it.  Do set "sl_sugloaded" so that we
	    // don't try again and again.
	    slang->sl_sugloaded = TRUE;

	    dotp = vim_strrchr(slang->sl_fname, '.');
	    ਜੇ (dotp == ਨਿਰਾ || fnamecmp(dotp, ".spl") != 0)
		ਜਾਰੀ;
	    STRCPY(dotp, ".sug");
	    fd = mch_fopen((ਅੱਖਰ *)slang->sl_fname, "r");
	    ਜੇ (fd == ਨਿਰਾ)
		ਜਾਓ nextone;

	    /*
	     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
	     */
	    ਲਈ (i = 0; i < VIMSUGMAGICL; ++i)
		buf[i] = (c = getc(fd)) == EOF ? 0 : c;	// <fileID>
	    ਜੇ (STRNCMP(buf, VIMSUGMAGIC, VIMSUGMAGICL) != 0)
	    {
		semsg(_(e_this_does_not_look_like_sug_file_str),
							     slang->sl_fname);
		ਜਾਓ nextone;
	    }
	    c = getc(fd);				// <versionnr>
	    ਜੇ (c < VIMSUGVERSION)
	    {
		semsg(_(e_old_sug_file_needs_ਮੰਜ਼ਿਲ_be_updated_str),
							     slang->sl_fname);
		ਜਾਓ nextone;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c > VIMSUGVERSION)
	    {
		semsg(_(e_sug_file_is_for_newer_ਸੰਸਕਰਣ_ਦਾ_vim_str),
							     slang->sl_fname);
		ਜਾਓ nextone;
	    }

	    // Check the timestamp, it must be exactly the same as the one in
	    // the .spl file.  Otherwise the word numbers won't match.
	    timestamp = get8ctime(fd);			// <timestamp>
	    ਜੇ (timestamp != slang->sl_sugtime)
	    {
		semsg(_(e_sug_file_doesnt_ਮੇਲ_spl_file_str),
							     slang->sl_fname);
		ਜਾਓ nextone;
	    }

	    /*
	     * <SUGWORDTREE>: <wordtree>
	     * Read the trie with the soundfolded words.
	     */
	    ਜੇ (spell_read_ਰੁੱਖ(fd, &slang->sl_sbyts, &slang->sl_sbyts_len,
			&slang->sl_sidxs, FALSE, 0) != 0)
	    {
someerror:
		semsg(_(e_error_while_reading_sug_file_str),
							     slang->sl_fname);
		slang_clear_sug(slang);
		ਜਾਓ nextone;
	    }

	    /*
	     * <SUGTABLE>: <sugwcount> <sugline> ...
	     *
	     * Read the table with word numbers.  We use a file buffer for
	     * this, because it's so much like a file with lines.  Makes it
	     * possible to swap the info and save on memory use.
	     */
	    slang->sl_sugbuf = open_spellbuf();
	    ਜੇ (slang->sl_sugbuf == ਨਿਰਾ)
		ਜਾਓ someerror;
							    // <sugwcount>
	    wcount = get4c(fd);
	    ਜੇ (wcount < 0)
		ਜਾਓ someerror;

	    // Read all the wordnr lists into the buffer, one NUL terminated
	    // list per line.
	    ga_init2(&ga, 1, 100);
	    ਲਈ (wordnr = 0; wordnr < wcount; ++wordnr)
	    {
		ga.ga_len = 0;
		ਲਈ (;;)
		{
		    c = getc(fd);			    // <sugline>
		    ਜੇ (c < 0 || ga_grow(&ga, 1) == FAIL)
			ਜਾਓ someerror;
		    ((char_u *)ga.ga_ਅੰਕੜੇ)[ga.ga_len++] = c;
		    ਜੇ (c == NUL)
			ਰੋਕੋ;
		}
		ਜੇ (ml_append_buf(slang->sl_sugbuf, (linenr_T)wordnr,
					 ga.ga_ਅੰਕੜੇ, ga.ga_len, TRUE) == FAIL)
		    ਜਾਓ someerror;
	    }
	    ga_clear(&ga);

	    /*
	     * Need to put word counts in the word tries, so that we can find
	     * a word by its number.
	     */
	    ਰੁੱਖ_ਗਿਣਤੀ_ਸ਼ਬਦ(slang->sl_fbyts, slang->sl_fbyts_len,
		    slang->sl_fidxs);
	    ਰੁੱਖ_ਗਿਣਤੀ_ਸ਼ਬਦ(slang->sl_sbyts, slang->sl_sbyts_len,
		    slang->sl_sidxs);

nextone:
	    ਜੇ (fd != ਨਿਰਾ)
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
    ਸਥਿਰ char_u *
read_cnt_string(FILE *fd, ਪੂਰਨ cnt_ਬਾਈਟਾਂ, ਪੂਰਨ *cntp)
{
    ਪੂਰਨ		cnt = 0;
    ਪੂਰਨ		i;
    char_u	*str;

    // read the length bytes, MSB first
    ਲਈ (i = 0; i < cnt_ਬਾਈਟਾਂ; ++i)
    {
	ਪੂਰਨ c = getc(fd);

	ਜੇ (c == EOF)
	{
	    *cntp = SP_TRUNCERROR;
	    ਵਾਪਸ ਨਿਰਾ;
	}
	cnt = (cnt << 8) + (ਬਿਨਾਨਿਸ਼ਾਨੀ)c;
    }
    *cntp = cnt;
    ਜੇ (cnt == 0)
	ਵਾਪਸ ਨਿਰਾ;	    // nothing to read, return NULL

    str = read_string(fd, cnt);
    ਜੇ (str == ਨਿਰਾ)
	*cntp = SP_OTHERERROR;
    ਵਾਪਸ str;
}

/*
 * Read SN_REGION: <regionname> ...
 * Return SP_*ERROR flags.
 */
    ਸਥਿਰ ਪੂਰਨ
read_region_section(FILE *fd, slang_T *lp, ਪੂਰਨ len)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		c = 0;

    ਜੇ (len > MAXREGIONS * 2)
	ਵਾਪਸ SP_FORMERROR;
    ਲਈ (i = 0; i < len; ++i)
	lp->sl_regions[i] = (c = getc(fd)) == EOF ? 0 : c; // <regionname>
    lp->sl_regions[len] = NUL;
    ਵਾਪਸ c == EOF ? SP_TRUNCERROR : 0;
}

/*
 * Read SN_CHARFLAGS section: <charflagslen> <charflags>
 *				<folcharslen> <folchars>
 * Return SP_*ERROR flags.
 */
    ਸਥਿਰ ਪੂਰਨ
read_charflags_section(FILE *fd)
{
    char_u	*flags;
    char_u	*fol;
    ਪੂਰਨ		flagslen, follen;

    // <charflagslen> <charflags>
    flags = read_cnt_string(fd, 1, &flagslen);
    ਜੇ (flagslen < 0)
	ਵਾਪਸ flagslen;

    // <folcharslen> <folchars>
    fol = read_cnt_string(fd, 2, &follen);
    ਜੇ (follen < 0)
    {
	vim_free(flags);
	ਵਾਪਸ follen;
    }

    // Set the word-char flags and fill SPELL_ISUPPER() table.
    ਜੇ (flags != ਨਿਰਾ && fol != ਨਿਰਾ)
	ਸੈੱਟ_spell_charflags(flags, flagslen, fol);

    vim_free(flags);
    vim_free(fol);

    // When <charflagslen> is zero then <fcharlen> must also be zero.
    ਜੇ ((flags == ਨਿਰਾ) != (fol == ਨਿਰਾ))
	ਵਾਪਸ SP_FORMERROR;
    ਵਾਪਸ 0;
}

/*
 * Read SN_PREFCOND section.
 * Return SP_*ERROR flags.
 */
    ਸਥਿਰ ਪੂਰਨ
read_prefcond_section(FILE *fd, slang_T *lp)
{
    ਪੂਰਨ		cnt;
    ਪੂਰਨ		i;
    ਪੂਰਨ		n;
    ਪੂਰਨ		c;
    char_u	*p;
    char_u	buf[MAXWLEN + 1];

    // <prefcondcnt> <prefcond> ...
    cnt = get2c(fd);					// <prefcondcnt>
    ਜੇ (cnt <= 0)
	ਵਾਪਸ SP_FORMERROR;

    lp->sl_prefprog = ALLOC_CLEAR_MULT(regprog_T *, cnt);
    ਜੇ (lp->sl_prefprog == ਨਿਰਾ)
	ਵਾਪਸ SP_OTHERERROR;
    lp->sl_prefixcnt = cnt;

    ਲਈ (i = 0; i < cnt; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	n = getc(fd);					// <condlen>
	ਜੇ (n < 0 || n >= MAXWLEN)
	    ਵਾਪਸ SP_FORMERROR;

	// When <condlen> is zero we have an empty condition.  Otherwise
	// compile the regexp program used to check for the condition.
	ਜੇ (n > 0)
	{
	    buf[0] = '^';	    // always match at one position only
	    p = buf + 1;
	    ਜਦੋਂ (n-- > 0)
		*p++ = (c = getc(fd)) == EOF ? 0 : c;	    // <condstr>
	    ਜੇ (c == EOF)
		ਰੋਕੋ;
	    *p = NUL;
	    lp->sl_prefprog[i] = vim_regcomp(buf, RE_MAGIC + RE_STRING);
	}
    }
    ਵਾਪਸ 0;
}

/*
 * Read REP or REPSAL items section from "fd": <repcount> <rep> ...
 * Return SP_*ERROR flags.
 */
    ਸਥਿਰ ਪੂਰਨ
read_rep_section(FILE *fd, garray_T *gap, ਛੋਟਾ *ਪਹਿਲਾ)
{
    ਪੂਰਨ		cnt;
    fromto_T	*ftp;
    ਪੂਰਨ		i;

    cnt = get2c(fd);					// <repcount>
    ਜੇ (cnt < 0)
	ਵਾਪਸ SP_TRUNCERROR;

    ਜੇ (ga_grow(gap, cnt) == FAIL)
	ਵਾਪਸ SP_OTHERERROR;

    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
    ਲਈ (; gap->ga_len < cnt; ++gap->ga_len)
    {
	ftp = &((fromto_T *)gap->ga_ਅੰਕੜੇ)[gap->ga_len];
	ftp->ft_from = read_cnt_string(fd, 1, &i);
	ਜੇ (i < 0)
	    ਵਾਪਸ i;
	ਜੇ (i == 0)
	    ਵਾਪਸ SP_FORMERROR;
	ftp->ft_ਮੰਜ਼ਿਲ = read_cnt_string(fd, 1, &i);
	ਜੇ (i <= 0)
	{
	    vim_free(ftp->ft_from);
	    ਜੇ (i < 0)
		ਵਾਪਸ i;
	    ਵਾਪਸ SP_FORMERROR;
	}
    }

    // Fill the first-index table.
    ਲਈ (i = 0; i < 256; ++i)
	ਪਹਿਲਾ[i] = -1;
    ਲਈ (i = 0; i < gap->ga_len; ++i)
    {
	ftp = &((fromto_T *)gap->ga_ਅੰਕੜੇ)[i];
	ਜੇ (ਪਹਿਲਾ[*ftp->ft_from] == -1)
	    ਪਹਿਲਾ[*ftp->ft_from] = i;
    }
    ਵਾਪਸ 0;
}

/*
 * Read SN_SAL section: <salflags> <salcount> <sal> ...
 * Return SP_*ERROR flags.
 */
    ਸਥਿਰ ਪੂਰਨ
read_sal_section(FILE *fd, slang_T *slang)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		cnt;
    garray_T	*gap;
    salitem_T	*smp;
    ਪੂਰਨ		ccnt;
    char_u	*p;

    slang->sl_sofo = FALSE;

    i = getc(fd);				// <salflags>
    ਜੇ (i & SAL_F0LLOWUP)
	slang->sl_followup = TRUE;
    ਜੇ (i & SAL_COLLAPSE)
	slang->sl_collapse = TRUE;
    ਜੇ (i & SAL_REM_ACCENTS)
	slang->sl_rem_accents = TRUE;

    cnt = get2c(fd);				// <salcount>
    ਜੇ (cnt < 0)
	ਵਾਪਸ SP_TRUNCERROR;

    gap = &slang->sl_sal;
    ga_init2(gap, ਆਕਾਰ(salitem_T), 10);
    ਜੇ (ga_grow(gap, cnt + 1) == FAIL)
	ਵਾਪਸ SP_OTHERERROR;

    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
    ਲਈ (; gap->ga_len < cnt; ++gap->ga_len)
    {
	ਪੂਰਨ	c = NUL;

	smp = &((salitem_T *)gap->ga_ਅੰਕੜੇ)[gap->ga_len];
	ccnt = getc(fd);			// <salfromlen>
	ਜੇ (ccnt < 0)
	    ਵਾਪਸ SP_TRUNCERROR;
	ਜੇ ((p = alloc(ccnt + 2)) == ਨਿਰਾ)
	    ਵਾਪਸ SP_OTHERERROR;
	smp->sm_lead = p;

	// Read up to the first special char into sm_lead.
	ਲਈ (i = 0; i < ccnt; ++i)
	{
	    c = getc(fd);			// <salfrom>
	    ਜੇ (vim_strchr((char_u *)"0123456789(-<^$", c) != ਨਿਰਾ)
		ਰੋਕੋ;
	    *p++ = c;
	}
	smp->sm_leadlen = (ਪੂਰਨ)(p - smp->sm_lead);
	*p++ = NUL;

	// Put (abc) chars in sm_oneof, if any.
	ਜੇ (c == '(')
	{
	    smp->sm_oneof = p;
	    ਲਈ (++i; i < ccnt; ++i)
	    {
		c = getc(fd);			// <salfrom>
		ਜੇ (c == ')')
		    ਰੋਕੋ;
		*p++ = c;
	    }
	    *p++ = NUL;
	    ਜੇ (++i < ccnt)
		c = getc(fd);
	}
	ਨਹੀਂਤਾਂ
	    smp->sm_oneof = ਨਿਰਾ;

	// Any following chars go in sm_rules.
	smp->sm_ਨਿਯਮਾਂ = p;
	ਜੇ (i < ccnt)
	    // store the char we got while checking for end of sm_lead
	    *p++ = c;
	ਲਈ (++i; i < ccnt; ++i)
	    *p++ = (c = getc(fd)) == EOF ? 0 : c;	// <salfrom>
	*p++ = NUL;

	// <saltolen> <salto>
	smp->sm_ਮੰਜ਼ਿਲ = read_cnt_string(fd, 1, &ccnt);
	ਜੇ (ccnt < 0)
	{
	    vim_free(smp->sm_lead);
	    ਵਾਪਸ ccnt;
	}

	ਜੇ (has_mbyte)
	{
	    // convert the multi-byte strings to wide char strings
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = mb_charlen(smp->sm_lead);
	    ਜੇ (smp->sm_oneof == ਨਿਰਾ)
		smp->sm_oneof_w = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
		smp->sm_oneof_w = mb_str2wide(smp->sm_oneof);
	    ਜੇ (smp->sm_ਮੰਜ਼ਿਲ == ਨਿਰਾ)
		smp->sm_ਮੰਜ਼ਿਲ_w = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
		smp->sm_ਮੰਜ਼ਿਲ_w = mb_str2wide(smp->sm_ਮੰਜ਼ਿਲ);
	    ਜੇ (smp->sm_lead_w == ਨਿਰਾ
		    || (smp->sm_oneof_w == ਨਿਰਾ && smp->sm_oneof != ਨਿਰਾ)
		    || (smp->sm_ਮੰਜ਼ਿਲ_w == ਨਿਰਾ && smp->sm_ਮੰਜ਼ਿਲ != ਨਿਰਾ))
	    {
		vim_free(smp->sm_lead);
		vim_free(smp->sm_ਮੰਜ਼ਿਲ);
		vim_free(smp->sm_lead_w);
		vim_free(smp->sm_oneof_w);
		vim_free(smp->sm_ਮੰਜ਼ਿਲ_w);
		ਵਾਪਸ SP_OTHERERROR;
	    }
	}
    }

    ਜੇ (gap->ga_len > 0)
    {
	// Add one extra entry to mark the end with an empty sm_lead.  Avoids
	// that we need to check the index every time.
	smp = &((salitem_T *)gap->ga_ਅੰਕੜੇ)[gap->ga_len];
	ਜੇ ((p = alloc(1)) == ਨਿਰਾ)
	    ਵਾਪਸ SP_OTHERERROR;
	p[0] = NUL;
	smp->sm_lead = p;
	smp->sm_leadlen = 0;
	smp->sm_oneof = ਨਿਰਾ;
	smp->sm_ਨਿਯਮਾਂ = p;
	smp->sm_ਮੰਜ਼ਿਲ = ਨਿਰਾ;
	ਜੇ (has_mbyte)
	{
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = 0;
	    smp->sm_oneof_w = ਨਿਰਾ;
	    smp->sm_ਮੰਜ਼ਿਲ_w = ਨਿਰਾ;
	}
	++gap->ga_len;
    }

    // Fill the first-index table.
    ਸੈੱਟ_sal_ਪਹਿਲਾ(slang);

    ਵਾਪਸ 0;
}

/*
 * Read SN_WORDS: <word> ...
 * Return SP_*ERROR flags.
 */
    ਸਥਿਰ ਪੂਰਨ
read_ਸ਼ਬਦ_section(FILE *fd, slang_T *lp, ਪੂਰਨ len)
{
    ਪੂਰਨ		ਪੂਰਾ = 0;
    ਪੂਰਨ		i;
    ਪੂਰਨ		c;
    char_u	ਸ਼ਬਦ[MAXWLEN];

    ਜਦੋਂ (ਪੂਰਾ < len)
    {
	// Read one word at a time.
	ਲਈ (i = 0; ; ++i)
	{
	    c = getc(fd);
	    ਜੇ (c == EOF)
		ਵਾਪਸ SP_TRUNCERROR;
	    ਸ਼ਬਦ[i] = c;
	    ਜੇ (ਸ਼ਬਦ[i] == NUL)
		ਰੋਕੋ;
	    ਜੇ (i == MAXWLEN - 1)
		ਵਾਪਸ SP_FORMERROR;
	}

	// Init the count to 10.
	ਗਿਣਤੀ_common_ਸ਼ਬਦ(lp, ਸ਼ਬਦ, -1, 10);
	ਪੂਰਾ += i + 1;
    }
    ਵਾਪਸ 0;
}

/*
 * SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
 * Return SP_*ERROR flags.
 */
    ਸਥਿਰ ਪੂਰਨ
read_sofo_section(FILE *fd, slang_T *slang)
{
    ਪੂਰਨ		cnt;
    char_u	*from, *ਮੰਜ਼ਿਲ;
    ਪੂਰਨ		res;

    // <sofofromlen> <sofofrom>
    from = read_cnt_string(fd, 2, &cnt);
    ਜੇ (cnt < 0)
	ਵਾਪਸ cnt;

    // <sofotolen> <sofoto>
    ਮੰਜ਼ਿਲ = read_cnt_string(fd, 2, &cnt);
    ਜੇ (cnt < 0)
    {
	vim_free(from);
	ਵਾਪਸ cnt;
    }

    // Store the info in slang->sl_sal and/or slang->sl_sal_first.
    ਜੇ (from != ਨਿਰਾ && ਮੰਜ਼ਿਲ != ਨਿਰਾ)
	res = ਸੈੱਟ_sofo(slang, from, ਮੰਜ਼ਿਲ);
    ਨਹੀਂਤਾਂ ਜੇ (from != ਨਿਰਾ || ਮੰਜ਼ਿਲ != ਨਿਰਾ)
	res = SP_FORMERROR;    // only one of two strings is an error
    ਨਹੀਂਤਾਂ
	res = 0;

    vim_free(from);
    vim_free(ਮੰਜ਼ਿਲ);
    ਵਾਪਸ res;
}

/*
 * Read the compound section from the .spl file:
 *	<compmax> <compminlen> <compsylmax> <compoptions> <compflags>
 * Returns SP_*ERROR flags.
 */
    ਸਥਿਰ ਪੂਰਨ
read_compound(FILE *fd, slang_T *slang, ਪੂਰਨ len)
{
    ਪੂਰਨ		todo = len;
    ਪੂਰਨ		c;
    ਪੂਰਨ		atstart;
    char_u	*pat;
    char_u	*pp;
    char_u	*cp;
    char_u	*ap;
    char_u	*crp;
    ਪੂਰਨ		cnt;
    garray_T	*gap;
    size_t	patsize;
    size_t	flagsize;

    ਜੇ (todo < 2)
	ਵਾਪਸ SP_FORMERROR;	// need at least two bytes

    --todo;
    c = getc(fd);					// <compmax>
    ਜੇ (c < 2)
	c = MAXWLEN;
    slang->sl_compmax = c;

    --todo;
    c = getc(fd);					// <compminlen>
    ਜੇ (c < 1)
	c = 0;
    slang->sl_compminlen = c;

    --todo;
    c = getc(fd);					// <compsylmax>
    ਜੇ (c < 1)
	c = MAXWLEN;
    slang->sl_compsylmax = c;

    c = getc(fd);					// <compoptions>
    ਜੇ (c != 0)
	ungetc(c, fd);	    // be backwards compatible with Vim 7.0b
    ਨਹੀਂਤਾਂ
    {
	--todo;
	c = getc(fd);	    // only use the lower byte for now
	--todo;
	slang->sl_compoptions = c;

	gap = &slang->sl_comppat;
	c = get2c(fd);					// <comppatcount>
	ਜੇ (c < 0)
	    ਵਾਪਸ SP_TRUNCERROR;
	todo -= 2;
	ga_init2(gap, ਆਕਾਰ(char_u *), c);
	ਜੇ (ga_grow(gap, c) == OK)
	    ਜਦੋਂ (--c >= 0)
	    {
		((char_u **)(gap->ga_ਅੰਕੜੇ))[gap->ga_len++] =
						  read_cnt_string(fd, 1, &cnt);
					    // <comppatlen> <comppattext>
		ਜੇ (cnt < 0)
		    ਵਾਪਸ cnt;
		todo -= cnt + 1;
	    }
    }
    ਜੇ (todo < 0)
	ਵਾਪਸ SP_FORMERROR;

    // Turn the COMPOUNDRULE items into a regexp pattern:
    // "a[bc]/a*b+" -> "^\(a[bc]\|a*b\+\)$".
    // Inserting backslashes may double the length, "^\(\)$<Nul>" is 7 bytes.
    // Conversion to utf-8 may double the size.
    ਜੇ ((size_t)todo > COMPOUND_MAX_LEN)
	ਵਾਪਸ SP_FORMERROR;
    patsize = (size_t)todo * 2 + 7;
    ਜੇ (enc_utf8)
	patsize += (size_t)todo * 2;
    flagsize = (size_t)todo + 1;
    pat = alloc(patsize);
    ਜੇ (pat == ਨਿਰਾ)
	ਵਾਪਸ SP_OTHERERROR;

    // We also need a list of all flags that can appear at the start and one
    // for all flags.
    cp = alloc(flagsize);
    ਜੇ (cp == ਨਿਰਾ)
    {
	vim_free(pat);
	ਵਾਪਸ SP_OTHERERROR;
    }
    slang->sl_compstartflags = cp;
    *cp = NUL;

    ap = alloc(flagsize);
    ਜੇ (ap == ਨਿਰਾ)
    {
	vim_free(pat);
	ਵਾਪਸ SP_OTHERERROR;
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
    ਜਦੋਂ (todo-- > 0)
    {
	c = getc(fd);					// <compflags>
	ਜੇ (c == EOF)
	{
	    vim_free(pat);
	    ਵਾਪਸ SP_TRUNCERROR;
	}

	// Add all flags to "sl_compallflags".
	ਜੇ (vim_strchr((char_u *)"?*+[]/", c) == ਨਿਰਾ
		&& !byte_ਵਿੱਚ_str(slang->sl_compallflags, c))
	{
	    *ap++ = c;
	    *ap = NUL;
	}

	ਜੇ (atstart != 0)
	{
	    // At start of item: copy flags to "sl_compstartflags".  For a
	    // [abc] item set "atstart" to 2 and copy up to the ']'.
	    ਜੇ (c == '[')
		atstart = 2;
	    ਨਹੀਂਤਾਂ ਜੇ (c == ']')
		atstart = 0;
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (!byte_ਵਿੱਚ_str(slang->sl_compstartflags, c))
		{
		    *cp++ = c;
		    *cp = NUL;
		}
		ਜੇ (atstart == 1)
		    atstart = 0;
	    }
	}

	// Copy flag to "sl_comprules", unless we run into a wildcard.
	ਜੇ (crp != ਨਿਰਾ)
	{
	    ਜੇ (c == '?' || c == '+' || c == '*')
	    {
		VIM_CLEAR(slang->sl_comprules);
		crp = ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ
		*crp++ = c;
	}

	ਜੇ (c == '/')	    // slash separates two items
	{
	    *pp++ = '\\';
	    *pp++ = '|';
	    atstart = 1;
	}
	ਨਹੀਂਤਾਂ		    // normal char, "[abc]" and '*' are copied as-is
	{
	    ਜੇ (c == '?' || c == '+' || c == '~')
		*pp++ = '\\';	    // "a?" becomes "a\?", "a+" becomes "a\+"
	    ਜੇ (enc_utf8)
		pp += mb_char2bytes(c, pp);
	    ਨਹੀਂਤਾਂ
		*pp++ = c;
	}
    }

    *pp++ = '\\';
    *pp++ = ')';
    *pp++ = '$';
    *pp = NUL;

    ਜੇ (crp != ਨਿਰਾ)
	*crp = NUL;

    slang->sl_compprog = vim_regcomp(pat, RE_MAGIC + RE_STRING + RE_STRICT);
    vim_free(pat);
    ਜੇ (slang->sl_compprog == ਨਿਰਾ)
	ਵਾਪਸ SP_FORMERROR;

    ਵਾਪਸ 0;
}

/*
 * Set the SOFOFROM and SOFOTO items in language "lp".
 * Returns SP_*ERROR flags when there is something wrong.
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_sofo(slang_T *lp, char_u *from, char_u *ਮੰਜ਼ਿਲ)
{
    ਪੂਰਨ		i;

    garray_T	*gap;
    char_u	*s;
    char_u	*p;
    ਪੂਰਨ		c;
    ਪੂਰਨ		*inp;

    ਜੇ (has_mbyte)
    {
	free_sal_ਚੀਜ਼ਾਂ(&lp->sl_sal);

	// Use "sl_sal" as an array with 256 pointers to a list of wide
	// characters.  The index is the low byte of the character.
	// The list contains from-to pairs with a terminating NUL.
	// sl_sal_first[] is used for latin1 "from" characters.
	gap = &lp->sl_sal;
	ga_init2(gap, ਆਕਾਰ(ਪੂਰਨ *), 1);
	ਜੇ (ga_grow(gap, 256) == FAIL)
	    ਵਾਪਸ SP_OTHERERROR;
	vim_memset(gap->ga_ਅੰਕੜੇ, 0, ਆਕਾਰ(ਪੂਰਨ *) * 256);
	gap->ga_len = 256;
	lp->sl_sofo = TRUE;

	// First count the number of items for each list.  Temporarily use
	// sl_sal_first[] for this.  Reset it first: a preceding SN_SAL section
	// may have set the entries to -1 via set_sal_first().
	vim_memset(lp->sl_sal_ਪਹਿਲਾ, 0, ਆਕਾਰ(salfirst_T) * 256);
	ਲਈ (p = from, s = ਮੰਜ਼ਿਲ; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    MB_CPTR_ADV(s);
	    ਜੇ (c >= 256)
		++lp->sl_sal_ਪਹਿਲਾ[c & 0xff];
	}
	ਜੇ (*p != NUL || *s != NUL)	    // lengths differ
	    ਵਾਪਸ SP_FORMERROR;

	// Allocate the lists.
	ਲਈ (i = 0; i < 256; ++i)
	    ਜੇ (lp->sl_sal_ਪਹਿਲਾ[i] > 0)
	    {
		p = alloc(ਆਕਾਰ(ਪੂਰਨ) * (lp->sl_sal_ਪਹਿਲਾ[i] * 2 + 1));
		ਜੇ (p == ਨਿਰਾ)
		    ਵਾਪਸ SP_OTHERERROR;
		((ਪੂਰਨ **)gap->ga_ਅੰਕੜੇ)[i] = (ਪੂਰਨ *)p;
		*(ਪੂਰਨ *)p = 0;
	    }

	// Put the characters up to 255 in sl_sal_first[] the rest in a sl_sal
	// list.
	vim_memset(lp->sl_sal_ਪਹਿਲਾ, 0, ਆਕਾਰ(salfirst_T) * 256);
	ਲਈ (p = from, s = ਮੰਜ਼ਿਲ; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    i = mb_cptr2char_adv(&s);
	    ਜੇ (c >= 256)
	    {
		// Append the from-to chars at the end of the list with
		// the low byte.
		inp = ((ਪੂਰਨ **)gap->ga_ਅੰਕੜੇ)[c & 0xff];
		ਜਦੋਂ (*inp != 0)
		    ++inp;
		*inp++ = c;		// from char
		*inp++ = i;		// to char
		*inp++ = NUL;		// NUL at the end
	    }
	    ਨਹੀਂਤਾਂ
		// mapping byte to char is done in sl_sal_first[]
		lp->sl_sal_ਪਹਿਲਾ[c] = i;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// mapping bytes to bytes is done in sl_sal_first[]
	ਜੇ (STRLEN(from) != STRLEN(ਮੰਜ਼ਿਲ))
	    ਵਾਪਸ SP_FORMERROR;

	ਲਈ (i = 0; ਮੰਜ਼ਿਲ[i] != NUL; ++i)
	    lp->sl_sal_ਪਹਿਲਾ[from[i]] = ਮੰਜ਼ਿਲ[i];
	lp->sl_sal.ga_len = 1;		// indicates we have soundfolding
	lp->sl_sofo = TRUE;
    }

    ਵਾਪਸ 0;
}

/*
 * Fill the first-index table for "lp".
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_sal_ਪਹਿਲਾ(slang_T *lp)
{
    salfirst_T	*sfirst;
    ਪੂਰਨ		i;
    salitem_T	*smp;
    ਪੂਰਨ		c;
    garray_T	*gap = &lp->sl_sal;

    sfirst = lp->sl_sal_ਪਹਿਲਾ;
    ਲਈ (i = 0; i < 256; ++i)
	sfirst[i] = -1;
    smp = (salitem_T *)gap->ga_ਅੰਕੜੇ;
    ਲਈ (i = 0; i < gap->ga_len; ++i)
    {
	ਜੇ (has_mbyte)
	    // Use the lowest byte of the first character.  For latin1 it's
	    // the character, for other encodings it should differ for most
	    // characters.
	    c = *smp[i].sm_lead_w & 0xff;
	ਨਹੀਂਤਾਂ
	    c = *smp[i].sm_lead;
	ਜੇ (sfirst[c] == -1)
	{
	    sfirst[c] = i;
	    ਜੇ (has_mbyte)
	    {
		ਪੂਰਨ		n;

		// Make sure all entries with this byte are following each
		// other.  Move the ones that are in the wrong position.  Do
		// keep the same ordering!
		ਜਦੋਂ (i + 1 < gap->ga_len
				       && (*smp[i + 1].sm_lead_w & 0xff) == c)
		    // Skip over entry with same index byte.
		    ++i;

		ਲਈ (n = 1; i + n < gap->ga_len; ++n)
		    ਜੇ ((*smp[i + n].sm_lead_w & 0xff) == c)
		    {
			salitem_T  tsal;

			// Move entry with same index byte after the entries
			// we already found.
			++i;
			--n;
			tsal = smp[i + n];
			mch_memmove(smp + i + 1, smp + i,
						       ਆਕਾਰ(salitem_T) * n);
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
    ਸਥਿਰ ਪੂਰਨ *
mb_str2wide(char_u *s)
{
    ਪੂਰਨ		*res;
    char_u	*p;
    ਪੂਰਨ		i = 0;

    res = ALLOC_MULT(ਪੂਰਨ, mb_charlen(s) + 1);
    ਜੇ (res == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (p = s; *p != NUL; )
	res[i++] = mb_ptr2char_adv(&p);
    res[i] = NUL;
    ਵਾਪਸ res;
}

/*
 * Read a tree from the .spl or .sug file.
 * Allocates the memory and stores pointers in "bytsp" and "idxsp".
 * This is skipped when the tree has zero length.
 * Returns zero when OK, SP_ value for an error.
 */
    ਸਥਿਰ ਪੂਰਨ
spell_read_ਰੁੱਖ(
    FILE	*fd,
    char_u	**bytsp,
    ਲੰਮਾ	*bytsp_len,
    idx_T	**idxsp,
    ਪੂਰਨ		prefixtree,	// TRUE for the prefix tree
    ਪੂਰਨ		prefixcnt)	// when "prefixtree" is TRUE: prefix count
{
    ਲੰਮਾ	len;
    ਪੂਰਨ		idx;
    char_u	*bp;
    idx_T	*ip;

    // The tree size was computed when writing the file, so that we can
    // allocate it as one long block. <nodecount>
    len = get4c(fd);
    ਜੇ (len < 0)
	ਵਾਪਸ SP_TRUNCERROR;
    ਜੇ ((size_t)len > ਮਾਪ_MAX / ਆਕਾਰ(ਪੂਰਨ))
	// Invalid length, multiply with sizeof(int) would overflow.
	ਵਾਪਸ SP_FORMERROR;
    ਜੇ (len <= 0)
	ਵਾਪਸ 0;

    // Allocate the byte array.  Zero-initialize so that any position the
    // tree does not visit reads as 0; a stray BY_INDEX shared reference
    // into such a slot then behaves as end-of-word in spellsuggest()
    // instead of consuming an arbitrary heap byte as a siblingcount.
    bp = alloc_clear(len);
    ਜੇ (bp == ਨਿਰਾ)
	ਵਾਪਸ SP_OTHERERROR;
    *bytsp = bp;
    ਜੇ (bytsp_len != ਨਿਰਾ)
	*bytsp_len = len;

    // Allocate the index array.
    ip = lalloc_clear(len * ਆਕਾਰ(ਪੂਰਨ), TRUE);
    ਜੇ (ip == ਨਿਰਾ)
	ਵਾਪਸ SP_OTHERERROR;
    *idxsp = ip;

    // Recursively read the tree and store it in the array.
    idx = read_ਰੁੱਖ_ਗੰਢ(fd, bp, ip, len, 0, prefixtree, prefixcnt, 0);
    ਜੇ (idx < 0)
	ਵਾਪਸ idx;
    ਜੇ (idx != len)
	ਵਾਪਸ SP_FORMERROR;
    ਵਾਪਸ 0;
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
    ਸਥਿਰ idx_T
read_ਰੁੱਖ_ਗੰਢ(
    FILE	*fd,
    char_u	*byts,
    idx_T	*idxs,
    ਪੂਰਨ		maxidx,		    // size of arrays
    idx_T	startidx,	    // current index in "byts" and "idxs"
    ਪੂਰਨ		prefixtree,	    // TRUE for reading PREFIXTREE
    ਪੂਰਨ		maxprefcondnr,	    // maximum for <prefcondnr>
    ਪੂਰਨ		ਡੂੰਘਾਈ)		    // recursion level
{
    ਪੂਰਨ		len;
    ਪੂਰਨ		i;
    ਪੂਰਨ		n;
    idx_T	idx = startidx;
    ਪੂਰਨ		c;
    ਪੂਰਨ		c2;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHARED_ਨਕਾਬ	0x8000000

    // Bail out on a crafted .spl whose tree recurses beyond the maximum
    // word length: each tree level corresponds to one byte of a word, so
    // any well-formed file has depth <= MAXWLEN.
    ਜੇ (ਡੂੰਘਾਈ > MAXWLEN)
	ਵਾਪਸ SP_FORMERROR;

    len = getc(fd);					// <siblingcount>
    ਜੇ (len <= 0)
	ਵਾਪਸ SP_TRUNCERROR;

    ਜੇ (len >= maxidx - startidx)
	ਵਾਪਸ SP_FORMERROR;
    byts[idx++] = len;

    // Read the byte values, flag/region bytes and shared indexes.
    ਲਈ (i = 1; i <= len; ++i)
    {
	c = getc(fd);					// <byte>
	ਜੇ (c < 0)
	    ਵਾਪਸ SP_TRUNCERROR;
	ਜੇ (c <= BY_SPECIAL)
	{
	    ਜੇ (c == BY_NOFLAGS && !prefixtree)
	    {
		// No flags, all regions.
		idxs[idx] = 0;
		c = 0;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c != BY_ਸੂਚਕਾਂਕ)
	    {
		ਜੇ (prefixtree)
		{
		    // Read the optional pflags byte, the prefix ID and the
		    // condition nr.  In idxs[] store the prefix ID in the low
		    // byte, the condition index shifted up 8 bits, the flags
		    // shifted up 24 bits.
		    ਜੇ (c == BY_FLAGS)
		    {
			n = getc(fd);			// <pflags>
			ਜੇ (n < 0)
			    ਵਾਪਸ SP_TRUNCERROR;
			c = n << 24;
		    }
		    ਨਹੀਂਤਾਂ
			c = 0;

		    n = getc(fd);			// <affixID>
		    ਜੇ (n < 0)
			ਵਾਪਸ SP_TRUNCERROR;
		    c |= n;

		    n = get2c(fd);			// <prefcondnr>
		    ਜੇ (n < 0 || n >= maxprefcondnr)
			ਵਾਪਸ SP_FORMERROR;
		    c |= (n << 8);
		}
		ਨਹੀਂਤਾਂ // c must be BY_FLAGS or BY_FLAGS2
		{
		    // Read flags and optional region and prefix ID.  In
		    // idxs[] the flags go in the low two bytes, region above
		    // that and prefix ID above the region.
		    c2 = c;
		    c = getc(fd);			// <flags>
		    ਜੇ (c < 0)
			ਵਾਪਸ SP_TRUNCERROR;
		    ਜੇ (c2 == BY_FLAGS2)
		    {
			n = getc(fd);			// <flags2>
			ਜੇ (n < 0)
			    ਵਾਪਸ SP_TRUNCERROR;
			c = (n << 8) + c;
		    }
		    ਜੇ (c & WF_REGION)
		    {
			n = getc(fd);			// <region>
			ਜੇ (n < 0)
			    ਵਾਪਸ SP_TRUNCERROR;
			c = (n << 16) + c;
		    }
		    ਜੇ (c & WF_AFX)
		    {
			n = getc(fd);			// <affixID>
			ਜੇ (n < 0)
			    ਵਾਪਸ SP_TRUNCERROR;
			c = (n << 24) + c;
		    }
		}

		idxs[idx] = c;
		c = 0;
	    }
	    ਨਹੀਂਤਾਂ // c == BY_INDEX
	    {
							// <nodeidx>
		n = get3c(fd);
		ਜੇ (n < 0 || n >= maxidx)
		    ਵਾਪਸ SP_FORMERROR;
		idxs[idx] = n + SHARED_ਨਕਾਬ;
		c = getc(fd);				// <xbyte>
	    }
	}
	byts[idx++] = c;
    }

    // Recursively read the children for non-shared siblings.
    // Skip the end-of-word ones (zero byte value) and the shared ones (and
    // remove SHARED_MASK)
    ਲਈ (i = 1; i <= len; ++i)
	ਜੇ (byts[startidx + i] != 0)
	{
	    ਜੇ (idxs[startidx + i] & SHARED_ਨਕਾਬ)
		idxs[startidx + i] &= ~SHARED_ਨਕਾਬ;
	    ਨਹੀਂਤਾਂ
	    {
		idxs[startidx + i] = idx;
		idx = read_ਰੁੱਖ_ਗੰਢ(fd, byts, idxs, maxidx, idx,
					prefixtree, maxprefcondnr, ਡੂੰਘਾਈ + 1);
		ਜੇ (idx < 0)
		    ਰੋਕੋ;
	    }
	}

    ਵਾਪਸ idx;
}

/*
 * Reload the spell file "fname" if it's loaded.
 */
    ਸਥਿਰ ਖਾਲੀ
spell_reload_one(
    char_u	*fname,
    ਪੂਰਨ		added_ਸ਼ਬਦ)	// invoked through "zg"
{
    slang_T	*slang;
    ਪੂਰਨ		didit = FALSE;

    FOR_ALL_SPELL_LANGS(slang)
    {
	ਜੇ (fullpathcmp(fname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	{
	    slang_clear(slang);
	    ਜੇ (spell_ਲੱਦੋ_file(fname, ਨਿਰਾ, slang, FALSE) == ਨਿਰਾ)
		// reloading failed, clear the language
		slang_clear(slang);
	    redraw_all_later(UPD_SOME_VALID);
	    didit = TRUE;
	}
    }

    // When "zg" was used and the file wasn't loaded yet, should redo
    // 'spelllang' to load it now.
    ਜੇ (added_ਸ਼ਬਦ && !didit)
	parse_spelllang(curwin);
}


/*
 * Functions for ":mkspell".
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXLINELEN  500		// Maximum length in bytes of a line in a .aff
				// and .dic file.
/*
 * Main structure to store the contents of a ".aff" file.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ afffile_S
{
    char_u	*af_enc;	// "SET", normalized, alloc'ed string or NULL
    ਪੂਰਨ		af_flagtype;	// AFT_CHAR, AFT_LONG, AFT_NUM or AFT_CAPLONG
    ਬਿਨਾਨਿਸ਼ਾਨੀ	af_rare;	// RARE ID for rare word
    ਬਿਨਾਨਿਸ਼ਾਨੀ	af_keepcase;	// KEEPCASE ID for keep-case word
    ਬਿਨਾਨਿਸ਼ਾਨੀ	af_bad;		// BAD ID for banned word
    ਬਿਨਾਨਿਸ਼ਾਨੀ	af_needaffix;	// NEEDAFFIX ID
    ਬਿਨਾਨਿਸ਼ਾਨੀ	af_circumfix;	// CIRCUMFIX ID
    ਬਿਨਾਨਿਸ਼ਾਨੀ	af_needcomp;	// NEEDCOMPOUND ID
    ਬਿਨਾਨਿਸ਼ਾਨੀ	af_comproot;	// COMPOUNDROOT ID
    ਬਿਨਾਨਿਸ਼ਾਨੀ	af_compforbid;	// COMPOUNDFORBIDFLAG ID
    ਬਿਨਾਨਿਸ਼ਾਨੀ	af_comppermit;	// COMPOUNDPERMITFLAG ID
    ਬਿਨਾਨਿਸ਼ਾਨੀ	af_nosuggest;	// NOSUGGEST ID
    ਪੂਰਨ		af_pfxpostpone;	// postpone prefixes without chop string and
				// without flags
    ਪੂਰਨ		af_ignoreextra;	// IGNOREEXTRA present
    hashtab_T	af_pref;	// hashtable for prefixes, affheader_T
    hashtab_T	af_suff;	// hashtable for suffixes, affheader_T
    hashtab_T	af_comp;	// hashtable for compound flags, compitem_T
} afffile_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AFT_CHAR	0	// flags are one character
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AFT_LONG	1	// flags are two characters
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AFT_CAPLONG	2	// flags are one or two characters
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AFT_ਅੰਕ		3	// flags are numbers, comma separated

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ affentry_S affentry_T;
// Affix entry from ".aff" file.  Used for prefixes and suffixes.
ਬਣਤਰ affentry_S
{
    affentry_T	*ae_ਅਗਲਾ;	// next affix with same name/number
    char_u	*ae_chop;	// text to chop off basic word (can be NULL)
    char_u	*ae_ਜੋੜੋ;	// text to add to basic word (can be NULL)
    char_u	*ae_flags;	// flags on the affix (can be NULL)
    char_u	*ae_cond;	// condition (NULL for ".")
    regprog_T	*ae_prog;	// regexp program for ae_cond or NULL
    ਅੱਖਰ	ae_compforbid;	// COMPOUNDFORBIDFLAG found
    ਅੱਖਰ	ae_comppermit;	// COMPOUNDPERMITFLAG found
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AH_ਕੁੰਜੀ_LEN 17		// 2 x 8 bytes + NUL

// Affix header from ".aff" file.  Used for af_pref and af_suff.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ affheader_S
{
    char_u	ah_ਕੁੰਜੀ[AH_ਕੁੰਜੀ_LEN]; // key for hashtab == name of affix
    ਬਿਨਾਨਿਸ਼ਾਨੀ	ah_flag;	// affix name as number, uses "af_flagtype"
    ਪੂਰਨ		ah_newID;	// prefix ID after renumbering; 0 if not used
    ਪੂਰਨ		ah_combine;	// suffix may combine with prefix
    ਪੂਰਨ		ah_follows;	// another affix block should be following
    affentry_T	*ah_ਪਹਿਲਾ;	// first affix entry
} affheader_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HI2AH(hi)   ((affheader_T *)(hi)->hi_ਕੁੰਜੀ)

// Flag used in compound items.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ compitem_S
{
    char_u	ci_ਕੁੰਜੀ[AH_ਕੁੰਜੀ_LEN]; // key for hashtab == name of compound
    ਬਿਨਾਨਿਸ਼ਾਨੀ	ci_flag;	// affix name as number, uses "af_flagtype"
    ਪੂਰਨ		ci_newID;	// affix ID after renumbering.
} compitem_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HI2CI(hi)   ((compitem_T *)(hi)->hi_ਕੁੰਜੀ)

/*
 * Structure that is used to store the items in the word tree.  This avoids
 * the need to keep track of each allocated thing, everything is freed all at
 * once after ":mkspell" is done.
 * Note: "sb_next" must be just before "sb_data" to make sure the alignment of
 * "sb_data" is correct for systems where pointers must be aligned on
 * pointer-size boundaries and sizeof(pointer) > sizeof(int) (e.g., Sparc).
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ  SBLOCKSIZE 16000	// size of sb_data
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ sblock_S sblock_T;
ਬਣਤਰ sblock_S
{
    ਪੂਰਨ		sb_ਵਰਤਿਆ;	// nr of bytes already in use
    sblock_T	*sb_ਅਗਲਾ;	// next block in list
    char_u	sb_ਅੰਕੜੇ[1];	// data, actually longer
};

/*
 * A node in the tree.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ wordnode_S wordnode_T;
ਬਣਤਰ wordnode_S
{
    ਸੰਘ   // shared to save space
    {
	char_u	hashkey[6];	// the hash key, only used while compressing
	ਪੂਰਨ	ਸੂਚਕਾਂਕ;		// index in written nodes (valid after first
				// round)
    } wn_u1;
    ਸੰਘ   // shared to save space
    {
	wordnode_T *ਅਗਲਾ;	// next node with same hash key
	wordnode_T *wnode;	// parent node that will write this node
    } wn_u2;
    wordnode_T	*wn_ਬੱਚਾ;	// child (next byte in word)
    wordnode_T  *wn_sibling;	// next sibling (alternate byte in word,
				// always sorted)
    ਪੂਰਨ		wn_refs;	// Nr. of references to this node.  Only
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
    ਛੋਟਾ	wn_region;	// region mask

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SPELL_PRINTTREE
    ਪੂਰਨ		wn_nr;		// sequence nr for printing
#ਜੇ_ਅੰਤ
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WN_ਨਕਾਬ	 0xffff		// mask relevant bits of "wn_flags"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HI2WN(hi)    (wordnode_T *)((hi)->hi_ਕੁੰਜੀ)

/*
 * Info used while reading the spell files.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ spellinfo_S
{
    wordnode_T	*si_foldroot;	// tree with case-folded words
    ਲੰਮਾ	si_foldwcount;	// nr of words in si_foldroot

    wordnode_T	*si_keeproot;	// tree with keep-case words
    ਲੰਮਾ	si_keepwcount;	// nr of words in si_keeproot

    wordnode_T	*si_prefroot;	// tree with postponed prefixes

    ਲੰਮਾ	si_sugtree;	// creating the soundfolding trie

    sblock_T	*si_ਖੰਡਾਂ;	// memory blocks used
    ਲੰਮਾ	si_ਖੰਡਾਂ_cnt;	// memory blocks allocated
    ਪੂਰਨ		si_did_emsg;	// TRUE when ran out of memory

    ਲੰਮਾ	si_compress_cnt;    // words to add before lowering
				    // compression limit
    wordnode_T	*si_ਪਹਿਲਾ_free; // List of nodes that have been freed during
				// compression, linked by "wn_child" field.
    ਲੰਮਾ	si_free_ਗਿਣਤੀ;	// number of nodes in si_first_free
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SPELL_PRINTTREE
    ਪੂਰਨ		si_wordnode_nr;	// sequence nr for nodes
#ਜੇ_ਅੰਤ
    buf_T	*si_spellbuf;	// buffer used to store soundfold word table

    ਪੂਰਨ		si_ascii;	// handling only ASCII words
    ਪੂਰਨ		si_ਜੋੜੋ;		// addition file
    ਪੂਰਨ		si_clear_chartab;   // when TRUE clear char tables
    ਪੂਰਨ		si_region;	// region mask
    vimconv_T	si_conv;	// for conversion to 'encoding'
    ਪੂਰਨ		si_memtot;	// runtime memory used
    ਪੂਰਨ		si_verbose;	// verbose messages
    ਪੂਰਨ		si_ਸੁਨੇਹਾ_ਗਿਣਤੀ;	// number of words added since last message
    char_u	*si_info;	// info text chars or NULL
    ਪੂਰਨ		si_region_ਗਿਣਤੀ; // number of regions supported (1 when there
				 // are no regions)
    char_u	si_region_name[MAXREGIONS * 2 + 1];
				// region names; used only if
				// si_region_count > 1)

    garray_T	si_rep;		// list of fromto_T entries from REP lines
    garray_T	si_repsal;	// list of fromto_T entries from REPSAL lines
    garray_T	si_sal;		// list of fromto_T entries from SAL lines
    char_u	*si_sofofr;	// SOFOFROM text
    char_u	*si_sofoto;	// SOFOTO text
    ਪੂਰਨ		si_nosugfile;	// NOSUGFILE item found
    ਪੂਰਨ		si_nosplitsugs;	// NOSPLITSUGS item found
    ਪੂਰਨ		si_nocompoundsugs; // NOCOMPOUNDSUGS item found
    ਪੂਰਨ		si_followup;	// soundsalike: ?
    ਪੂਰਨ		si_collapse;	// soundsalike: ?
    hashtab_T	si_commonwords;	// hashtable for common words
    time_t	si_sugtime;	// timestamp for .sug file
    ਪੂਰਨ		si_rem_accents;	// soundsalike: remove accents
    garray_T	si_map;		// MAP info concatenated
    char_u	*si_midword;	// MIDWORD chars or NULL
    ਪੂਰਨ		si_compmax;	// max nr of words for compounding
    ਪੂਰਨ		si_compminlen;	// minimal length for compounding
    ਪੂਰਨ		si_compsylmax;	// max nr of syllables for compounding
    ਪੂਰਨ		si_compoptions;	// COMP_ flags
    garray_T	si_comppat;	// CHECKCOMPOUNDPATTERN items, each stored as
				// a string
    char_u	*si_compflags;	// flags used for compounding
    char_u	si_nobreak;	// NOBREAK
    char_u	*si_syllable;	// syllable string
    garray_T	si_prefcond;	// table with conditions for postponed
				// prefixes, each stored as a string
    ਪੂਰਨ		si_newprefID;	// current value for ah_newID
    ਪੂਰਨ		si_newcompID;	// current value for compound ID
} spellinfo_T;

ਸਥਿਰ ਪੂਰਨ is_aff_ਨਿਯਮ(char_u **ਚੀਜ਼ਾਂ, ਪੂਰਨ itemcnt, ਅੱਖਰ *rulename, ਪੂਰਨ	 mincount);
ਸਥਿਰ ਖਾਲੀ aff_process_flags(afffile_T *affile, affentry_T *ਇੰਦਰਾਜ);
ਸਥਿਰ ਪੂਰਨ spell_info_ਵਸਤੂ(char_u *s);
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ affitem2flag(ਪੂਰਨ flagtype, char_u *ਵਸਤੂ, char_u	*fname, ਪੂਰਨ lnum);
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ get_affitem(ਪੂਰਨ flagtype, char_u **pp);
ਸਥਿਰ ਖਾਲੀ process_compflags(spellinfo_T *spin, afffile_T *aff, char_u *compflags);
ਸਥਿਰ ਖਾਲੀ check_renumber(spellinfo_T *spin);
ਸਥਿਰ ਖਾਲੀ aff_check_ਸੰਖਿਆ(ਪੂਰਨ spinval, ਪੂਰਨ affval, ਅੱਖਰ *name);
ਸਥਿਰ ਖਾਲੀ aff_check_string(char_u *spinval, char_u *affval, ਅੱਖਰ *name);
ਸਥਿਰ ਪੂਰਨ str_equal(char_u *s1, char_u	*s2);
ਸਥਿਰ ਖਾਲੀ ਜੋੜੋ_fromto(spellinfo_T *spin, garray_T *gap, char_u	*from, char_u *ਮੰਜ਼ਿਲ);
ਸਥਿਰ ਪੂਰਨ sal_ਮੰਜ਼ਿਲ_bool(char_u *s);
ਸਥਿਰ ਪੂਰਨ get_affix_flags(afffile_T *affile, char_u *afflist);
ਸਥਿਰ ਪੂਰਨ get_pfxlist(afffile_T *affile, char_u *afflist, char_u *store_afflist, ਪੂਰਨ *cntp);
ਸਥਿਰ ਪੂਰਨ get_compflags(afffile_T *affile, char_u *afflist, char_u *store_afflist, ਪੂਰਨ *cntp);
ਸਥਿਰ ਪੂਰਨ store_aff_ਸ਼ਬਦ(spellinfo_T *spin, char_u *ਸ਼ਬਦ, char_u *afflist, afffile_T *affile, hashtab_T *ht, hashtab_T *xht, ਪੂਰਨ condit, ਪੂਰਨ flags, char_u *pfxlist, ਪੂਰਨ pfxlen);
ਸਥਿਰ ਖਾਲੀ *getroom(spellinfo_T *spin, size_t len, ਪੂਰਨ align);
ਸਥਿਰ char_u *getroom_save(spellinfo_T *spin, char_u *s);
ਸਥਿਰ ਪੂਰਨ store_ਸ਼ਬਦ(spellinfo_T *spin, char_u *ਸ਼ਬਦ, ਪੂਰਨ flags, ਪੂਰਨ region, char_u *pfxlist, ਪੂਰਨ need_affix);
ਸਥਿਰ ਪੂਰਨ ਰੁੱਖ_ਜੋੜੋ_ਸ਼ਬਦ(spellinfo_T *spin, char_u *ਸ਼ਬਦ, wordnode_T *ਰੁੱਖ, ਪੂਰਨ flags, ਪੂਰਨ region, ਪੂਰਨ affixID);
ਸਥਿਰ wordnode_T *get_wordnode(spellinfo_T *spin);
ਸਥਿਰ ਖਾਲੀ free_wordnode(spellinfo_T *spin, wordnode_T *n);
ਸਥਿਰ ਖਾਲੀ wordtree_compress(spellinfo_T *spin, wordnode_T *ਜੜ੍ਹ, ਅੱਖਰ *name);
ਸਥਿਰ ਲੰਮਾ ਗੰਢ_compress(spellinfo_T *spin, wordnode_T *ਗੰਢ, hashtab_T *ht, ਲੰਮਾ *tot);
ਸਥਿਰ ਪੂਰਨ ਗੰਢ_equal(wordnode_T *n1, wordnode_T *n2);
ਸਥਿਰ ਖਾਲੀ clear_ਗੰਢ(wordnode_T *ਗੰਢ);
ਸਥਿਰ ਪੂਰਨ put_ਗੰਢ(FILE *fd, wordnode_T *ਗੰਢ, ਪੂਰਨ idx, ਪੂਰਨ regionmask, ਪੂਰਨ prefixtree);
ਸਥਿਰ ਪੂਰਨ sug_filltree(spellinfo_T *spin, slang_T *slang);
ਸਥਿਰ ਪੂਰਨ sug_maketable(spellinfo_T *spin);
ਸਥਿਰ ਪੂਰਨ sug_filltable(spellinfo_T *spin, wordnode_T *ਗੰਢ, ਪੂਰਨ startwordnr, garray_T *gap);
ਸਥਿਰ ਪੂਰਨ offset2bytes(ਪੂਰਨ nr, char_u *buf);
ਸਥਿਰ ਖਾਲੀ sug_write(spellinfo_T *spin, char_u *fname);
ਸਥਿਰ ਖਾਲੀ spell_ਸੁਨੇਹਾ(spellinfo_T *spin, char_u *str);
ਸਥਿਰ ਖਾਲੀ init_spellfile(ਖਾਲੀ);

// In the postponed prefixes tree wn_flags is used to store the WFP_ flags,
// but it must be negative to indicate the prefix tree to tree_add_word().
// Use a negative number with the lower 8 bits zero.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PFX_FLAGS	(-256)

// flags for "condit" argument of store_aff_word()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONDIT_COMB	1	// affix must combine
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONDIT_CFIX	2	// affix must have CIRCUMFIX flag
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONDIT_SUF	4	// add a suffix for matching flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONDIT_AFF	8	// word already has an affix

/*
 * Tunable parameters for when the tree is compressed.  Filled from the
 * 'mkspellmem' option.
 */
ਸਥਿਰ ਲੰਮਾ compress_ਸ਼ੁਰੂ = 30000;	// memory / SBLOCKSIZE
ਸਥਿਰ ਲੰਮਾ compress_inc = 100;		// memory / SBLOCKSIZE
ਸਥਿਰ ਲੰਮਾ compress_added = 500000;	// word count

/*
 * Check the 'mkspellmem' option.  Return FAIL if it's wrong.
 * Sets "sps_flags".
 */
    ਪੂਰਨ
spell_check_msm(ਖਾਲੀ)
{
    char_u	*p = p_msm;
    ਲੰਮਾ	ਸ਼ੁਰੂ = 0;
    ਲੰਮਾ	incr = 0;
    ਲੰਮਾ	added = 0;

    ਜੇ (!VIM_ISDIGIT(*p))
	ਵਾਪਸ FAIL;
    // block count = (value * 1024) / SBLOCKSIZE (but avoid overflow)
    ਸ਼ੁਰੂ = (getdigits(&p) * 10) / (SBLOCKSIZE / 102);
    ਜੇ (*p != ',')
	ਵਾਪਸ FAIL;
    ++p;
    ਜੇ (!VIM_ISDIGIT(*p))
	ਵਾਪਸ FAIL;
    incr = (getdigits(&p) * 102) / (SBLOCKSIZE / 10);
    ਜੇ (*p != ',')
	ਵਾਪਸ FAIL;
    ++p;
    ਜੇ (!VIM_ISDIGIT(*p))
	ਵਾਪਸ FAIL;
    added = getdigits(&p) * 1024;
    ਜੇ (*p != NUL)
	ਵਾਪਸ FAIL;

    ਜੇ (ਸ਼ੁਰੂ == 0 || incr == 0 || added == 0 || incr > ਸ਼ੁਰੂ)
	ਵਾਪਸ FAIL;

    compress_ਸ਼ੁਰੂ = ਸ਼ੁਰੂ;
    compress_inc = incr;
    compress_added = added;
    ਵਾਪਸ OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SPELL_PRINTTREE
/*
 * For debugging the tree code: print the current tree in a (more or less)
 * readable format, so that we can see what happens when adding a word and/or
 * compressing the tree.
 * Based on code from Olaf Seibert.
 */
# define PRINTLINESIZE	1000
# define PRINTWIDTH	6

# define PRINTSOME(l, ਡੂੰਘਾਈ, fmt, a1, a2) vim_snprintf(l + ਡੂੰਘਾਈ * PRINTWIDTH, \
	    PRINTLINESIZE - PRINTWIDTH * ਡੂੰਘਾਈ, fmt, a1, a2)

ਸਥਿਰ ਅੱਖਰ line1[PRINTLINESIZE];
ਸਥਿਰ ਅੱਖਰ line2[PRINTLINESIZE];
ਸਥਿਰ ਅੱਖਰ line3[PRINTLINESIZE];

    ਸਥਿਰ ਖਾਲੀ
spell_clear_flags(wordnode_T *ਗੰਢ)
{
    wordnode_T	*np;

    FOR_ALL_ਗੰਢ_SIBLINGS(ਗੰਢ, np)
    {
	np->wn_u1.ਸੂਚਕਾਂਕ = FALSE;
	spell_clear_flags(np->wn_ਬੱਚਾ);
    }
}

    ਸਥਿਰ ਖਾਲੀ
spell_print_ਗੰਢ(wordnode_T *ਗੰਢ, ਪੂਰਨ ਡੂੰਘਾਈ)
{
    ਜੇ (ਗੰਢ->wn_u1.ਸੂਚਕਾਂਕ)
    {
	// Done this node before, print the reference.
	PRINTSOME(line1, ਡੂੰਘਾਈ, "(%d)", ਗੰਢ->wn_nr, 0);
	PRINTSOME(line2, ਡੂੰਘਾਈ, "    ", 0, 0);
	PRINTSOME(line3, ਡੂੰਘਾਈ, "    ", 0, 0);
	ਸੁਨੇਹਾ(line1);
	ਸੁਨੇਹਾ(line2);
	ਸੁਨੇਹਾ(line3);
    }
    ਨਹੀਂਤਾਂ
    {
	ਗੰਢ->wn_u1.ਸੂਚਕਾਂਕ = TRUE;

	ਜੇ (ਗੰਢ->wn_byte != NUL)
	{
	    ਜੇ (ਗੰਢ->wn_ਬੱਚਾ != ਨਿਰਾ)
		PRINTSOME(line1, ਡੂੰਘਾਈ, " %c -> ", ਗੰਢ->wn_byte, 0);
	    ਨਹੀਂਤਾਂ
		// Cannot happen?
		PRINTSOME(line1, ਡੂੰਘਾਈ, " %c ???", ਗੰਢ->wn_byte, 0);
	}
	ਨਹੀਂਤਾਂ
	    PRINTSOME(line1, ਡੂੰਘਾਈ, " $    ", 0, 0);

	PRINTSOME(line2, ਡੂੰਘਾਈ, "%d/%d    ", ਗੰਢ->wn_nr, ਗੰਢ->wn_refs);

	ਜੇ (ਗੰਢ->wn_sibling != ਨਿਰਾ)
	    PRINTSOME(line3, ਡੂੰਘਾਈ, " |    ", 0, 0);
	ਨਹੀਂਤਾਂ
	    PRINTSOME(line3, ਡੂੰਘਾਈ, "      ", 0, 0);

	ਜੇ (ਗੰਢ->wn_byte == NUL)
	{
	    ਸੁਨੇਹਾ(line1);
	    ਸੁਨੇਹਾ(line2);
	    ਸੁਨੇਹਾ(line3);
	}

	// do the children
	ਜੇ (ਗੰਢ->wn_byte != NUL && ਗੰਢ->wn_ਬੱਚਾ != ਨਿਰਾ)
	    spell_print_ਗੰਢ(ਗੰਢ->wn_ਬੱਚਾ, ਡੂੰਘਾਈ + 1);

	// do the siblings
	ਜੇ (ਗੰਢ->wn_sibling != ਨਿਰਾ)
	{
	    // get rid of all parent details except |
	    STRCPY(line1, line3);
	    STRCPY(line2, line3);
	    spell_print_ਗੰਢ(ਗੰਢ->wn_sibling, ਡੂੰਘਾਈ);
	}
    }
}

    ਸਥਿਰ ਖਾਲੀ
spell_print_ਰੁੱਖ(wordnode_T *ਜੜ੍ਹ)
{
    ਜੇ (ਜੜ੍ਹ == ਨਿਰਾ)
	ਵਾਪਸ;

    // Clear the "wn_u1.index" fields, used to remember what has been done.
    spell_clear_flags(ਜੜ੍ਹ);

    // Recursively print the tree.
    spell_print_ਗੰਢ(ਜੜ੍ਹ, 0);
}
#ਜੇ_ਅੰਤ // SPELL_PRINTTREE

/*
 * Read the affix file "fname".
 * Returns an afffile_T, NULL for complete failure.
 */
    ਸਥਿਰ afffile_T *
spell_read_aff(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    afffile_T	*aff;
    char_u	rline[MAXLINELEN];
    char_u	*ਸਤਰ;
    char_u	*pc = ਨਿਰਾ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXITEMCNT  30
    char_u	*(ਚੀਜ਼ਾਂ[MAXITEMCNT]);
    ਪੂਰਨ		itemcnt;
    char_u	*p;
    ਪੂਰਨ		lnum = 0;
    affheader_T	*cur_aff = ਨਿਰਾ;
    ਪੂਰਨ		did_postpone_ਅਗੇਤਰ = FALSE;
    ਪੂਰਨ		aff_todo = 0;
    hashtab_T	*tp;
    char_u	*ਨੀਵਾਂ = ਨਿਰਾ;
    char_u	*fol = ਨਿਰਾ;
    char_u	*upp = ਨਿਰਾ;
    ਪੂਰਨ		do_rep;
    ਪੂਰਨ		do_repsal;
    ਪੂਰਨ		do_sal;
    ਪੂਰਨ		do_mapline;
    ਪੂਰਨ		ਮਿਲਿਆ_map = FALSE;
    hashitem_T	*hi;
    ਪੂਰਨ		l;
    ਪੂਰਨ		compminlen = 0;		// COMPOUNDMIN value
    ਪੂਰਨ		compsylmax = 0;		// COMPOUNDSYLMAX value
    ਪੂਰਨ		compoptions = 0;	// COMP_ flags
    ਪੂਰਨ		compmax = 0;		// COMPOUNDWORDMAX value
    char_u	*compflags = ਨਿਰਾ;	// COMPOUNDFLAG and COMPOUNDRULE
					// concatenated
    char_u	*midword = ਨਿਰਾ;	// MIDWORD value
    char_u	*syllable = ਨਿਰਾ;	// SYLLABLE value
    char_u	*sofofrom = ਨਿਰਾ;	// SOFOFROM value
    char_u	*sofoto = ਨਿਰਾ;		// SOFOTO value

    /*
     * Open the file.
     */
    fd = mch_fopen((ਅੱਖਰ *)fname, "r");
    ਜੇ (fd == ਨਿਰਾ)
    {
	semsg(_(e_cant_open_file_str), fname);
	ਵਾਪਸ ਨਿਰਾ;
    }

    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, _("Reading affix file %s..."), fname);
    spell_ਸੁਨੇਹਾ(spin, IObuff);

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
    aff = (afffile_T *)getroom(spin, ਆਕਾਰ(afffile_T), TRUE);
    ਜੇ (aff == ਨਿਰਾ)
    {
	fclose(fd);
	ਵਾਪਸ ਨਿਰਾ;
    }
    hash_init(&aff->af_pref);
    hash_init(&aff->af_suff);
    hash_init(&aff->af_comp);

    /*
     * Read all the lines in the file one by one.
     */
    ਜਦੋਂ (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	ਸਤਰ_breakcheck();
	++lnum;

	// Skip comment lines.
	ਜੇ (*rline == '#')
	    ਜਾਰੀ;

	// Convert from "SET" to 'encoding' when needed.
	vim_free(pc);
	ਜੇ (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, ਨਿਰਾ);
	    ਜੇ (pc == ਨਿਰਾ)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
							   fname, lnum, rline);
		ਜਾਰੀ;
	    }
	    ਸਤਰ = pc;
	}
	ਨਹੀਂਤਾਂ
	{
	    pc = ਨਿਰਾ;
	    ਸਤਰ = rline;
	}

	// Split the line up in white separated items.  Put a NUL after each
	// item.
	itemcnt = 0;
	ਲਈ (p = ਸਤਰ; ; )
	{
	    ਜਦੋਂ (*p != NUL && *p <= ' ')  // skip white space and CR/NL
		++p;
	    ਜੇ (*p == NUL)
		ਰੋਕੋ;
	    ਜੇ (itemcnt == MAXITEMCNT)	    // too many items
		ਰੋਕੋ;
	    ਚੀਜ਼ਾਂ[itemcnt++] = p;
	    // A few items have arbitrary text argument, don't split them.
	    ਜੇ (itemcnt == 2 && spell_info_ਵਸਤੂ(ਚੀਜ਼ਾਂ[0]))
		ਜਦੋਂ (*p >= ' ' || *p == TAB)    // skip until CR/NL
		    ++p;
	    ਨਹੀਂਤਾਂ
		ਜਦੋਂ (*p > ' ')    // skip until white space or CR/NL
		    ++p;
	    ਜੇ (*p == NUL)
		ਰੋਕੋ;
	    *p++ = NUL;
	}

	// Handle non-empty lines.
	ਜੇ (itemcnt > 0)
	{
	    ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "SET", 2) && aff->af_enc == ਨਿਰਾ)
	    {
		// Setup for conversion from "ENC" to 'encoding'.
		aff->af_enc = enc_canonize(ਚੀਜ਼ਾਂ[1]);
		ਜੇ (aff->af_enc != ਨਿਰਾ && !spin->si_ascii
			&& convert_ਤਿਆਰੀ(&spin->si_conv, aff->af_enc,
							       p_enc) == FAIL)
		    smsg(_("Conversion in %s not supported: from %s to %s"),
					       fname, aff->af_enc, p_enc);
		spin->si_conv.vc_fail = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "FLAG", 2)
					      && aff->af_flagtype == AFT_CHAR)
	    {
		ਜੇ (STRCMP(ਚੀਜ਼ਾਂ[1], "long") == 0)
		    aff->af_flagtype = AFT_LONG;
		ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਚੀਜ਼ਾਂ[1], "num") == 0)
		    aff->af_flagtype = AFT_ਅੰਕ;
		ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਚੀਜ਼ਾਂ[1], "caplong") == 0)
		    aff->af_flagtype = AFT_CAPLONG;
		ਨਹੀਂਤਾਂ
		    smsg(_("Invalid value for FLAG in %s line %d: %s"),
			    fname, lnum, ਚੀਜ਼ਾਂ[1]);
		ਜੇ (aff->af_rare != 0
			|| aff->af_keepcase != 0
			|| aff->af_bad != 0
			|| aff->af_needaffix != 0
			|| aff->af_circumfix != 0
			|| aff->af_needcomp != 0
			|| aff->af_comproot != 0
			|| aff->af_nosuggest != 0
			|| compflags != ਨਿਰਾ
			|| aff->af_suff.ht_ਵਰਤਿਆ > 0
			|| aff->af_pref.ht_ਵਰਤਿਆ > 0)
		    smsg(_("FLAG after using flags in %s line %d: %s"),
			    fname, lnum, ਚੀਜ਼ਾਂ[1]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (spell_info_ਵਸਤੂ(ਚੀਜ਼ਾਂ[0]))
	    {
		    p = (char_u *)getroom(spin,
			    (spin->si_info == ਨਿਰਾ ? 0 : STRLEN(spin->si_info))
			    + STRLEN(ਚੀਜ਼ਾਂ[0])
			    + STRLEN(ਚੀਜ਼ਾਂ[1]) + 3, FALSE);
		    ਜੇ (p != ਨਿਰਾ)
		    {
			ਜੇ (spin->si_info != ਨਿਰਾ)
			{
			    STRCPY(p, spin->si_info);
			    STRCAT(p, "\n");
			}
			STRCAT(p, ਚੀਜ਼ਾਂ[0]);
			STRCAT(p, " ");
			STRCAT(p, ਚੀਜ਼ਾਂ[1]);
			spin->si_info = p;
		    }
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "MIDWORD", 2)
							   && midword == ਨਿਰਾ)
	    {
		midword = getroom_save(spin, ਚੀਜ਼ਾਂ[1]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "TRY", 2))
	    {
		// ignored, we look in the tree for what chars may appear
	    }
	    // TODO: remove "RAR" later
	    ਨਹੀਂਤਾਂ ਜੇ ((is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "RAR", 2)
			|| is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "RARE", 2))
							 && aff->af_rare == 0)
	    {
		aff->af_rare = affitem2flag(aff->af_flagtype, ਚੀਜ਼ਾਂ[1],
								 fname, lnum);
	    }
	    // TODO: remove "KEP" later
	    ਨਹੀਂਤਾਂ ਜੇ ((is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "KEP", 2)
			|| is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "KEEPCASE", 2))
						     && aff->af_keepcase == 0)
	    {
		aff->af_keepcase = affitem2flag(aff->af_flagtype, ਚੀਜ਼ਾਂ[1],
								 fname, lnum);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ((is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "BAD", 2)
			|| is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "FORBIDDENWORD", 2))
							  && aff->af_bad == 0)
	    {
		aff->af_bad = affitem2flag(aff->af_flagtype, ਚੀਜ਼ਾਂ[1],
								 fname, lnum);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "NEEDAFFIX", 2)
						    && aff->af_needaffix == 0)
	    {
		aff->af_needaffix = affitem2flag(aff->af_flagtype, ਚੀਜ਼ਾਂ[1],
								 fname, lnum);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "CIRCUMFIX", 2)
						    && aff->af_circumfix == 0)
	    {
		aff->af_circumfix = affitem2flag(aff->af_flagtype, ਚੀਜ਼ਾਂ[1],
								 fname, lnum);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "NOSUGGEST", 2)
						    && aff->af_nosuggest == 0)
	    {
		aff->af_nosuggest = affitem2flag(aff->af_flagtype, ਚੀਜ਼ਾਂ[1],
								 fname, lnum);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ((is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "NEEDCOMPOUND", 2)
			|| is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "ONLYINCOMPOUND", 2))
						     && aff->af_needcomp == 0)
	    {
		aff->af_needcomp = affitem2flag(aff->af_flagtype, ਚੀਜ਼ਾਂ[1],
								 fname, lnum);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "COMPOUNDROOT", 2)
						     && aff->af_comproot == 0)
	    {
		aff->af_comproot = affitem2flag(aff->af_flagtype, ਚੀਜ਼ਾਂ[1],
								 fname, lnum);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "COMPOUNDFORBIDFLAG", 2)
						   && aff->af_compforbid == 0)
	    {
		aff->af_compforbid = affitem2flag(aff->af_flagtype, ਚੀਜ਼ਾਂ[1],
								 fname, lnum);
		ਜੇ (aff->af_pref.ht_ਵਰਤਿਆ > 0)
		    smsg(_("Defining COMPOUNDFORBIDFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "COMPOUNDPERMITFLAG", 2)
						   && aff->af_comppermit == 0)
	    {
		aff->af_comppermit = affitem2flag(aff->af_flagtype, ਚੀਜ਼ਾਂ[1],
								 fname, lnum);
		ਜੇ (aff->af_pref.ht_ਵਰਤਿਆ > 0)
		    smsg(_("Defining COMPOUNDPERMITFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "COMPOUNDFLAG", 2)
							 && compflags == ਨਿਰਾ)
	    {
		// Turn flag "c" into COMPOUNDRULE compatible string "c+",
		// "Na" into "Na+", "1234" into "1234+".
		p = getroom(spin, STRLEN(ਚੀਜ਼ਾਂ[1]) + 2, FALSE);
		ਜੇ (p != ਨਿਰਾ)
		{
		    STRCPY(p, ਚੀਜ਼ਾਂ[1]);
		    STRCAT(p, "+");
		    compflags = p;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "COMPOUNDRULES", 2))
	    {
		// We don't use the count, but do check that it's a number and
		// not COMPOUNDRULE mistyped.
		ਜੇ (atoi((ਅੱਖਰ *)ਚੀਜ਼ਾਂ[1]) == 0)
		    smsg(_("Wrong COMPOUNDRULES value in %s line %d: %s"),
						       fname, lnum, ਚੀਜ਼ਾਂ[1]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "COMPOUNDRULE", 2))
	    {
		// Don't use the first rule if it is a number.
		ਜੇ (compflags != ਨਿਰਾ || *skipdigits(ਚੀਜ਼ਾਂ[1]) != NUL)
		{
		    // Concatenate this string to previously defined ones,
		    // using a slash to separate them.
		    l = (ਪੂਰਨ)STRLEN(ਚੀਜ਼ਾਂ[1]) + 1;
		    ਜੇ (compflags != ਨਿਰਾ)
			l += (ਪੂਰਨ)STRLEN(compflags) + 1;
		    p = getroom(spin, l, FALSE);
		    ਜੇ (p != ਨਿਰਾ)
		    {
			ਜੇ (compflags != ਨਿਰਾ)
			{
			    STRCPY(p, compflags);
			    STRCAT(p, "/");
			}
			STRCAT(p, ਚੀਜ਼ਾਂ[1]);
			compflags = p;
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "COMPOUNDWORDMAX", 2)
							      && compmax == 0)
	    {
		compmax = atoi((ਅੱਖਰ *)ਚੀਜ਼ਾਂ[1]);
		ਜੇ (compmax == 0)
		    smsg(_("Wrong COMPOUNDWORDMAX value in %s line %d: %s"),
						       fname, lnum, ਚੀਜ਼ਾਂ[1]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "COMPOUNDMIN", 2)
							   && compminlen == 0)
	    {
		compminlen = atoi((ਅੱਖਰ *)ਚੀਜ਼ਾਂ[1]);
		ਜੇ (compminlen == 0)
		    smsg(_("Wrong COMPOUNDMIN value in %s line %d: %s"),
						       fname, lnum, ਚੀਜ਼ਾਂ[1]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "COMPOUNDSYLMAX", 2)
							   && compsylmax == 0)
	    {
		compsylmax = atoi((ਅੱਖਰ *)ਚੀਜ਼ਾਂ[1]);
		ਜੇ (compsylmax == 0)
		    smsg(_("Wrong COMPOUNDSYLMAX value in %s line %d: %s"),
						       fname, lnum, ਚੀਜ਼ਾਂ[1]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "CHECKCOMPOUNDDUP", 1))
	    {
		compoptions |= COMP_CHECKDUP;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "CHECKCOMPOUNDREP", 1))
	    {
		compoptions |= COMP_CHECKREP;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "CHECKCOMPOUNDCASE", 1))
	    {
		compoptions |= COMP_CHECKCASE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "CHECKCOMPOUNDTRIPLE", 1))
	    {
		compoptions |= COMP_CHECKTRIPLE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "CHECKCOMPOUNDPATTERN", 2))
	    {
		ਜੇ (atoi((ਅੱਖਰ *)ਚੀਜ਼ਾਂ[1]) == 0)
		    smsg(_("Wrong CHECKCOMPOUNDPATTERN value in %s line %d: %s"),
						       fname, lnum, ਚੀਜ਼ਾਂ[1]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "CHECKCOMPOUNDPATTERN", 3))
	    {
		garray_T    *gap = &spin->si_comppat;
		ਪੂਰਨ	    i;

		// Only add the couple if it isn't already there.
		ਲਈ (i = 0; i < gap->ga_len - 1; i += 2)
		    ਜੇ (STRCMP(((char_u **)(gap->ga_ਅੰਕੜੇ))[i], ਚੀਜ਼ਾਂ[1]) == 0
			    && STRCMP(((char_u **)(gap->ga_ਅੰਕੜੇ))[i + 1],
							       ਚੀਜ਼ਾਂ[2]) == 0)
			ਰੋਕੋ;
		ਜੇ (i >= gap->ga_len && ga_grow(gap, 2) == OK)
		{
		    ((char_u **)(gap->ga_ਅੰਕੜੇ))[gap->ga_len++]
					       = getroom_save(spin, ਚੀਜ਼ਾਂ[1]);
		    ((char_u **)(gap->ga_ਅੰਕੜੇ))[gap->ga_len++]
					       = getroom_save(spin, ਚੀਜ਼ਾਂ[2]);
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "SYLLABLE", 2)
							  && syllable == ਨਿਰਾ)
	    {
		syllable = getroom_save(spin, ਚੀਜ਼ਾਂ[1]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "NOBREAK", 1))
	    {
		spin->si_nobreak = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "NOSPLITSUGS", 1))
	    {
		spin->si_nosplitsugs = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "NOCOMPOUNDSUGS", 1))
	    {
		spin->si_nocompoundsugs = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "NOSUGFILE", 1))
	    {
		spin->si_nosugfile = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "PFXPOSTPONE", 1))
	    {
		aff->af_pfxpostpone = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "IGNOREEXTRA", 1))
	    {
		aff->af_ignoreextra = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ((STRCMP(ਚੀਜ਼ਾਂ[0], "PFX") == 0
					      || STRCMP(ਚੀਜ਼ਾਂ[0], "SFX") == 0)
		    && aff_todo == 0
		    && itemcnt >= 4)
	    {
		ਪੂਰਨ	lasti = 4;
		char_u	ਕੁੰਜੀ[AH_ਕੁੰਜੀ_LEN];

		ਜੇ (*ਚੀਜ਼ਾਂ[0] == 'P')
		    tp = &aff->af_pref;
		ਨਹੀਂਤਾਂ
		    tp = &aff->af_suff;

		// Myspell allows the same affix name to be used multiple
		// times.  The affix files that do this have an undocumented
		// "S" flag on all but the last block, thus we check for that
		// and store it in ah_follows.
		vim_strncpy(ਕੁੰਜੀ, ਚੀਜ਼ਾਂ[1], AH_ਕੁੰਜੀ_LEN - 1);
		hi = hash_ਲੱਭੋ(tp, ਕੁੰਜੀ);
		ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		{
		    cur_aff = HI2AH(hi);
		    ਜੇ (cur_aff->ah_combine != (*ਚੀਜ਼ਾਂ[2] == 'Y'))
			smsg(_("Different combining flag in continued affix block in %s line %d: %s"),
						   fname, lnum, ਚੀਜ਼ਾਂ[1]);
		    ਜੇ (!cur_aff->ah_follows)
			smsg(_("Duplicate affix in %s line %d: %s"),
						       fname, lnum, ਚੀਜ਼ਾਂ[1]);
		}
		ਨਹੀਂਤਾਂ
		{
		    // New affix letter.
		    cur_aff = (affheader_T *)getroom(spin,
						   ਆਕਾਰ(affheader_T), TRUE);
		    ਜੇ (cur_aff == ਨਿਰਾ)
			ਰੋਕੋ;
		    cur_aff->ah_flag = affitem2flag(aff->af_flagtype, ਚੀਜ਼ਾਂ[1],
								 fname, lnum);
		    ਜੇ (cur_aff->ah_flag == 0 || STRLEN(ਚੀਜ਼ਾਂ[1]) >= AH_ਕੁੰਜੀ_LEN)
			ਰੋਕੋ;
		    ਜੇ (cur_aff->ah_flag == aff->af_bad
			    || cur_aff->ah_flag == aff->af_rare
			    || cur_aff->ah_flag == aff->af_keepcase
			    || cur_aff->ah_flag == aff->af_needaffix
			    || cur_aff->ah_flag == aff->af_circumfix
			    || cur_aff->ah_flag == aff->af_nosuggest
			    || cur_aff->ah_flag == aff->af_needcomp
			    || cur_aff->ah_flag == aff->af_comproot)
			smsg(_("Affix also used for BAD/RARE/KEEPCASE/NEEDAFFIX/NEEDCOMPOUND/NOSUGGEST in %s line %d: %s"),
						       fname, lnum, ਚੀਜ਼ਾਂ[1]);
		    STRCPY(cur_aff->ah_ਕੁੰਜੀ, ਚੀਜ਼ਾਂ[1]);
		    hash_ਜੋੜੋ(tp, cur_aff->ah_ਕੁੰਜੀ, "spelling");

		    cur_aff->ah_combine = (*ਚੀਜ਼ਾਂ[2] == 'Y');
		}

		// Check for the "S" flag, which apparently means that another
		// block with the same affix name is following.
		ਜੇ (itemcnt > lasti && STRCMP(ਚੀਜ਼ਾਂ[lasti], "S") == 0)
		{
		    ++lasti;
		    cur_aff->ah_follows = TRUE;
		}
		ਨਹੀਂਤਾਂ
		    cur_aff->ah_follows = FALSE;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter.
		ਜੇ (itemcnt > lasti && *ਚੀਜ਼ਾਂ[lasti] != '#')
		    smsg(_(e_afftrailing), fname, lnum, ਚੀਜ਼ਾਂ[lasti]);

		ਜੇ (STRCMP(ਚੀਜ਼ਾਂ[2], "Y") != 0 && STRCMP(ਚੀਜ਼ਾਂ[2], "N") != 0)
		    smsg(_("Expected Y or N in %s line %d: %s"),
						       fname, lnum, ਚੀਜ਼ਾਂ[2]);

		ਜੇ (*ਚੀਜ਼ਾਂ[0] == 'P' && aff->af_pfxpostpone)
		{
		    ਜੇ (cur_aff->ah_newID == 0)
		    {
			// Use a new number in the .spl file later, to be able
			// to handle multiple .aff files.
			check_renumber(spin);
			cur_aff->ah_newID = ++spin->si_newprefID;

			// We only really use ah_newID if the prefix is
			// postponed.  We know that only after handling all
			// the items.
			did_postpone_ਅਗੇਤਰ = FALSE;
		    }
		    ਨਹੀਂਤਾਂ
			// Did use the ID in a previous block.
			did_postpone_ਅਗੇਤਰ = TRUE;
		}

		aff_todo = atoi((ਅੱਖਰ *)ਚੀਜ਼ਾਂ[3]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ((STRCMP(ਚੀਜ਼ਾਂ[0], "PFX") == 0
					      || STRCMP(ਚੀਜ਼ਾਂ[0], "SFX") == 0)
		    && aff_todo > 0
		    && STRCMP(cur_aff->ah_ਕੁੰਜੀ, ਚੀਜ਼ਾਂ[1]) == 0
		    && itemcnt >= 5)
	    {
		affentry_T	*aff_ਇੰਦਰਾਜ;
		ਪੂਰਨ		ਉੱਤਲਾ_2 = FALSE;
		ਪੂਰਨ		lasti = 5;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter,
		// unless IGNOREEXTRA is used.  Hunspell uses a "-" item.
		ਜੇ (itemcnt > lasti
			&& !aff->af_ignoreextra
			&& *ਚੀਜ਼ਾਂ[lasti] != '#'
			&& (STRCMP(ਚੀਜ਼ਾਂ[lasti], "-") != 0
						     || itemcnt != lasti + 1))
		    smsg(_(e_afftrailing), fname, lnum, ਚੀਜ਼ਾਂ[lasti]);

		// New item for an affix letter.
		--aff_todo;
		aff_ਇੰਦਰਾਜ = (affentry_T *)getroom(spin,
						    ਆਕਾਰ(affentry_T), TRUE);
		ਜੇ (aff_ਇੰਦਰਾਜ == ਨਿਰਾ)
		    ਰੋਕੋ;

		ਜੇ (STRCMP(ਚੀਜ਼ਾਂ[2], "0") != 0)
		    aff_ਇੰਦਰਾਜ->ae_chop = getroom_save(spin, ਚੀਜ਼ਾਂ[2]);
		ਜੇ (STRCMP(ਚੀਜ਼ਾਂ[3], "0") != 0)
		{
		    aff_ਇੰਦਰਾਜ->ae_ਜੋੜੋ = getroom_save(spin, ਚੀਜ਼ਾਂ[3]);

		    // Recognize flags on the affix: abcd/XYZ
		    aff_ਇੰਦਰਾਜ->ae_flags = vim_strchr(aff_ਇੰਦਰਾਜ->ae_ਜੋੜੋ, '/');
		    ਜੇ (aff_ਇੰਦਰਾਜ->ae_flags != ਨਿਰਾ)
		    {
			*aff_ਇੰਦਰਾਜ->ae_flags++ = NUL;
			aff_process_flags(aff, aff_ਇੰਦਰਾਜ);
		    }
		}

		// Don't use an affix entry with non-ASCII characters when
		// "spin->si_ascii" is TRUE.
		ਜੇ (!spin->si_ascii || !(has_non_ascii(aff_ਇੰਦਰਾਜ->ae_chop)
					  || has_non_ascii(aff_ਇੰਦਰਾਜ->ae_ਜੋੜੋ)))
		{
		    aff_ਇੰਦਰਾਜ->ae_ਅਗਲਾ = cur_aff->ah_ਪਹਿਲਾ;
		    cur_aff->ah_ਪਹਿਲਾ = aff_ਇੰਦਰਾਜ;

		    ਜੇ (STRCMP(ਚੀਜ਼ਾਂ[4], ".") != 0)
		    {
			char_u	buf[MAXLINELEN];

			aff_ਇੰਦਰਾਜ->ae_cond = getroom_save(spin, ਚੀਜ਼ਾਂ[4]);
			// Note: this silently truncates the buffer, but this should
			// not happen in practice
			ਜੇ (*ਚੀਜ਼ਾਂ[0] == 'P')
			    vim_snprintf((ਅੱਖਰ *)buf, ਆਕਾਰ(buf), "^%s", ਚੀਜ਼ਾਂ[4]);
			ਨਹੀਂਤਾਂ
			    vim_snprintf((ਅੱਖਰ *)buf, ਆਕਾਰ(buf), "%s$", ਚੀਜ਼ਾਂ[4]);
			aff_ਇੰਦਰਾਜ->ae_prog = vim_regcomp(buf,
					    RE_MAGIC + RE_STRING + RE_STRICT);
			ਜੇ (aff_ਇੰਦਰਾਜ->ae_prog == ਨਿਰਾ)
			    smsg(_("Broken condition in %s line %d: %s"),
						       fname, lnum, ਚੀਜ਼ਾਂ[4]);
		    }

		    // For postponed prefixes we need an entry in si_prefcond
		    // for the condition.  Use an existing one if possible.
		    // Can't be done for an affix with flags, ignoring
		    // COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG.
		    ਜੇ (*ਚੀਜ਼ਾਂ[0] == 'P' && aff->af_pfxpostpone
					       && aff_ਇੰਦਰਾਜ->ae_flags == ਨਿਰਾ)
		    {
			// When the chop string is one lower-case letter and
			// the add string ends in the upper-case letter we set
			// the "upper" flag, clear "ae_chop" and remove the
			// letters from "ae_add".  The condition must either
			// be empty or start with the same letter.
			ਜੇ (aff_ਇੰਦਰਾਜ->ae_chop != ਨਿਰਾ
				&& aff_ਇੰਦਰਾਜ->ae_ਜੋੜੋ != ਨਿਰਾ
				&& aff_ਇੰਦਰਾਜ->ae_chop[(*mb_ptr2len)(
						   aff_ਇੰਦਰਾਜ->ae_chop)] == NUL)
			{
			    ਪੂਰਨ		c, c_ਉੱਪਰ;

			    c = PTR2CHAR(aff_ਇੰਦਰਾਜ->ae_chop);
			    c_ਉੱਪਰ = SPELL_TOUPPER(c);
			    ਜੇ (c_ਉੱਪਰ != c
				    && (aff_ਇੰਦਰਾਜ->ae_cond == ਨਿਰਾ
					|| PTR2CHAR(aff_ਇੰਦਰਾਜ->ae_cond) == c))
			    {
				p = aff_ਇੰਦਰਾਜ->ae_ਜੋੜੋ
						  + STRLEN(aff_ਇੰਦਰਾਜ->ae_ਜੋੜੋ);
				MB_PTR_ਪਿੱਠ(aff_ਇੰਦਰਾਜ->ae_ਜੋੜੋ, p);
				ਜੇ (PTR2CHAR(p) == c_ਉੱਪਰ)
				{
				    ਉੱਤਲਾ_2 = TRUE;
				    aff_ਇੰਦਰਾਜ->ae_chop = ਨਿਰਾ;
				    *p = NUL;

				    // The condition is matched with the
				    // actual word, thus must check for the
				    // upper-case letter.
				    ਜੇ (aff_ਇੰਦਰਾਜ->ae_cond != ਨਿਰਾ)
				    {
					char_u	buf[MAXLINELEN];

					ਜੇ (has_mbyte)
					{
					    onecap_copy(ਚੀਜ਼ਾਂ[4], buf, TRUE);
					    aff_ਇੰਦਰਾਜ->ae_cond = getroom_save(
								   spin, buf);
					}
					ਨਹੀਂਤਾਂ
					    *aff_ਇੰਦਰਾਜ->ae_cond = c_ਉੱਪਰ;
					ਜੇ (aff_ਇੰਦਰਾਜ->ae_cond != ਨਿਰਾ)
					{
					    sprintf((ਅੱਖਰ *)buf, "^%s",
							  aff_ਇੰਦਰਾਜ->ae_cond);
					    vim_regfree(aff_ਇੰਦਰਾਜ->ae_prog);
					    aff_ਇੰਦਰਾਜ->ae_prog = vim_regcomp(
						    buf, RE_MAGIC + RE_STRING);
					}
				    }
				}
			    }
			}

			ਜੇ (aff_ਇੰਦਰਾਜ->ae_chop == ਨਿਰਾ
					       && aff_ਇੰਦਰਾਜ->ae_flags == ਨਿਰਾ)
			{
			    ਪੂਰਨ		idx;
			    char_u	**pp;
			    ਪੂਰਨ		n;

			    // Find a previously used condition.
			    ਲਈ (idx = spin->si_prefcond.ga_len - 1; idx >= 0;
									--idx)
			    {
				p = ((char_u **)spin->si_prefcond.ga_ਅੰਕੜੇ)[idx];
				ਜੇ (str_equal(p, aff_ਇੰਦਰਾਜ->ae_cond))
				    ਰੋਕੋ;
			    }
			    ਜੇ (idx < 0 && ga_grow(&spin->si_prefcond, 1) == OK)
			    {
				// Not found, add a new condition.
				idx = spin->si_prefcond.ga_len++;
				pp = ((char_u **)spin->si_prefcond.ga_ਅੰਕੜੇ)
									+ idx;
				ਜੇ (aff_ਇੰਦਰਾਜ->ae_cond == ਨਿਰਾ)
				    *pp = ਨਿਰਾ;
				ਨਹੀਂਤਾਂ
				    *pp = getroom_save(spin,
							  aff_ਇੰਦਰਾਜ->ae_cond);
			    }

			    // Add the prefix to the prefix tree.
			    ਜੇ (aff_ਇੰਦਰਾਜ->ae_ਜੋੜੋ == ਨਿਰਾ)
				p = (char_u *)"";
			    ਨਹੀਂਤਾਂ
				p = aff_ਇੰਦਰਾਜ->ae_ਜੋੜੋ;

			    // PFX_FLAGS is a negative number, so that
			    // tree_add_word() knows this is the prefix tree.
			    n = PFX_FLAGS;
			    ਜੇ (!cur_aff->ah_combine)
				n |= WFP_NC;
			    ਜੇ (ਉੱਤਲਾ_2)
				n |= WFP_ਉੱਪਰ;
			    ਜੇ (aff_ਇੰਦਰਾਜ->ae_comppermit)
				n |= WFP_COMPPERMIT;
			    ਜੇ (aff_ਇੰਦਰਾਜ->ae_compforbid)
				n |= WFP_COMPFORBID;
			    ਰੁੱਖ_ਜੋੜੋ_ਸ਼ਬਦ(spin, p, spin->si_prefroot, n,
						      idx, cur_aff->ah_newID);
			    did_postpone_ਅਗੇਤਰ = TRUE;
			}

			// Didn't actually use ah_newID, backup si_newprefID.
			ਜੇ (aff_todo == 0 && !did_postpone_ਅਗੇਤਰ)
			{
			    --spin->si_newprefID;
			    cur_aff->ah_newID = 0;
			}
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "FOL", 2) && fol == ਨਿਰਾ)
	    {
		fol = vim_strsave(ਚੀਜ਼ਾਂ[1]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "LOW", 2) && ਨੀਵਾਂ == ਨਿਰਾ)
	    {
		ਨੀਵਾਂ = vim_strsave(ਚੀਜ਼ਾਂ[1]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "UPP", 2) && upp == ਨਿਰਾ)
	    {
		upp = vim_strsave(ਚੀਜ਼ਾਂ[1]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "REP", 2)
		     || is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "REPSAL", 2))
	    {
		// Ignore REP/REPSAL count
		ਜੇ (!ਸੁਰੱਖਿਅਤ_isdigit(*ਚੀਜ਼ਾਂ[1]))
		    smsg(_("Expected REP(SAL) count in %s line %d"),
								 fname, lnum);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ((STRCMP(ਚੀਜ਼ਾਂ[0], "REP") == 0
			|| STRCMP(ਚੀਜ਼ਾਂ[0], "REPSAL") == 0)
		    && itemcnt >= 3)
	    {
		// REP/REPSAL item
		// Myspell ignores extra arguments, we require it starts with
		// # to detect mistakes.
		ਜੇ (itemcnt > 3 && ਚੀਜ਼ਾਂ[3][0] != '#')
		    smsg(_(e_afftrailing), fname, lnum, ਚੀਜ਼ਾਂ[3]);
		ਜੇ (ਚੀਜ਼ਾਂ[0][3] == 'S' ? do_repsal : do_rep)
		{
		    // Replace underscore with space (can't include a space
		    // directly).
		    ਲਈ (p = ਚੀਜ਼ਾਂ[1]; *p != NUL; MB_PTR_ADV(p))
			ਜੇ (*p == '_')
			    *p = ' ';
		    ਲਈ (p = ਚੀਜ਼ਾਂ[2]; *p != NUL; MB_PTR_ADV(p))
			ਜੇ (*p == '_')
			    *p = ' ';
		    ਜੋੜੋ_fromto(spin, ਚੀਜ਼ਾਂ[0][3] == 'S'
					 ? &spin->si_repsal
					 : &spin->si_rep, ਚੀਜ਼ਾਂ[1], ਚੀਜ਼ਾਂ[2]);
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "MAP", 2))
	    {
		// MAP item or count
		ਜੇ (!ਮਿਲਿਆ_map)
		{
		    // First line contains the count.
		    ਮਿਲਿਆ_map = TRUE;
		    ਜੇ (!ਸੁਰੱਖਿਅਤ_isdigit(*ਚੀਜ਼ਾਂ[1]))
			smsg(_("Expected MAP count in %s line %d"),
								 fname, lnum);
		}
		ਨਹੀਂਤਾਂ ਜੇ (do_mapline)
		{
		    ਪੂਰਨ		c;

		    // Check that every character appears only once.
		    ਲਈ (p = ਚੀਜ਼ਾਂ[1]; *p != NUL; )
		    {
			c = mb_ptr2char_adv(&p);
			ਜੇ ((spin->si_map.ga_len > 0
				    && vim_strchr(spin->si_map.ga_ਅੰਕੜੇ, c)
								      != ਨਿਰਾ)
				|| vim_strchr(p, c) != ਨਿਰਾ)
			    smsg(_("Duplicate character in MAP in %s line %d"),
								 fname, lnum);
		    }

		    // We simply concatenate all the MAP strings, separated by
		    // slashes.
		    ga_concat(&spin->si_map, ਚੀਜ਼ਾਂ[1]);
		    ga_append(&spin->si_map, '/');
		}
	    }
	    // Accept "SAL from to" and "SAL from to  #comment".
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "SAL", 3))
	    {
		ਜੇ (do_sal)
		{
		    // SAL item (sounds-a-like)
		    // Either one of the known keys or a from-to pair.
		    ਜੇ (STRCMP(ਚੀਜ਼ਾਂ[1], "followup") == 0)
			spin->si_followup = sal_ਮੰਜ਼ਿਲ_bool(ਚੀਜ਼ਾਂ[2]);
		    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਚੀਜ਼ਾਂ[1], "collapse_result") == 0)
			spin->si_collapse = sal_ਮੰਜ਼ਿਲ_bool(ਚੀਜ਼ਾਂ[2]);
		    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਚੀਜ਼ਾਂ[1], "remove_accents") == 0)
			spin->si_rem_accents = sal_ਮੰਜ਼ਿਲ_bool(ਚੀਜ਼ਾਂ[2]);
		    ਨਹੀਂਤਾਂ
			// when "to" is "_" it means empty
			ਜੋੜੋ_fromto(spin, &spin->si_sal, ਚੀਜ਼ਾਂ[1],
				     STRCMP(ਚੀਜ਼ਾਂ[2], "_") == 0 ? (char_u *)""
								: ਚੀਜ਼ਾਂ[2]);
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "SOFOFROM", 2)
							  && sofofrom == ਨਿਰਾ)
	    {
		sofofrom = getroom_save(spin, ਚੀਜ਼ਾਂ[1]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_aff_ਨਿਯਮ(ਚੀਜ਼ਾਂ, itemcnt, "SOFOTO", 2)
							    && sofoto == ਨਿਰਾ)
	    {
		sofoto = getroom_save(spin, ਚੀਜ਼ਾਂ[1]);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਚੀਜ਼ਾਂ[0], "COMMON") == 0)
	    {
		ਪੂਰਨ	i;

		ਲਈ (i = 1; i < itemcnt; ++i)
		{
		    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hash_ਲੱਭੋ(&spin->si_commonwords,
								   ਚੀਜ਼ਾਂ[i])))
		    {
			p = vim_strsave(ਚੀਜ਼ਾਂ[i]);
			ਜੇ (p == ਨਿਰਾ)
			    ਰੋਕੋ;
			hash_ਜੋੜੋ(&spin->si_commonwords, p, "spelling");
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ
		smsg(_("Unrecognized or duplicate item in %s line %d: %s"),
						       fname, lnum, ਚੀਜ਼ਾਂ[0]);
	}
    }

    ਜੇ (fol != ਨਿਰਾ || ਨੀਵਾਂ != ਨਿਰਾ || upp != ਨਿਰਾ)
    {
	ਜੇ (spin->si_clear_chartab)
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
	ਜੇ (!spin->si_ascii && !enc_utf8)
	{
	    ਜੇ (fol == ਨਿਰਾ || ਨੀਵਾਂ == ਨਿਰਾ || upp == ਨਿਰਾ)
		smsg(_("Missing FOL/LOW/UPP line in %s"), fname);
	    ਨਹੀਂਤਾਂ
		(ਖਾਲੀ)ਸੈੱਟ_spell_chartab(fol, ਨੀਵਾਂ, upp);
	}

	vim_free(fol);
	vim_free(ਨੀਵਾਂ);
	vim_free(upp);
    }

    // Use compound specifications of the .aff file for the spell info.
    ਜੇ (compmax != 0)
    {
	aff_check_ਸੰਖਿਆ(spin->si_compmax, compmax, "COMPOUNDWORDMAX");
	spin->si_compmax = compmax;
    }

    ਜੇ (compminlen != 0)
    {
	aff_check_ਸੰਖਿਆ(spin->si_compminlen, compminlen, "COMPOUNDMIN");
	spin->si_compminlen = compminlen;
    }

    ਜੇ (compsylmax != 0)
    {
	ਜੇ (syllable == ਨਿਰਾ)
	    smsg(_("COMPOUNDSYLMAX used without SYLLABLE"));
	aff_check_ਸੰਖਿਆ(spin->si_compsylmax, compsylmax, "COMPOUNDSYLMAX");
	spin->si_compsylmax = compsylmax;
    }

    ਜੇ (compoptions != 0)
    {
	aff_check_ਸੰਖਿਆ(spin->si_compoptions, compoptions, "COMPOUND options");
	spin->si_compoptions |= compoptions;
    }

    ਜੇ (compflags != ਨਿਰਾ)
	process_compflags(spin, aff, compflags);

    // Check that we didn't use too many renumbered flags.
    ਜੇ (spin->si_newcompID < spin->si_newprefID)
    {
	ਜੇ (spin->si_newcompID == 127 || spin->si_newcompID == 255)
	    ਸੁਨੇਹਾ(_("Too many postponed prefixes"));
	ਨਹੀਂਤਾਂ ਜੇ (spin->si_newprefID == 0 || spin->si_newprefID == 127)
	    ਸੁਨੇਹਾ(_("Too many compound flags"));
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ(_("Too many postponed prefixes and/or compound flags"));
    }

    ਜੇ (syllable != ਨਿਰਾ)
    {
	aff_check_string(spin->si_syllable, syllable, "SYLLABLE");
	spin->si_syllable = syllable;
    }

    ਜੇ (sofofrom != ਨਿਰਾ || sofoto != ਨਿਰਾ)
    {
	ਜੇ (sofofrom == ਨਿਰਾ || sofoto == ਨਿਰਾ)
	    smsg(_("Missing SOFO%s line in %s"),
				     sofofrom == ਨਿਰਾ ? "FROM" : "TO", fname);
	ਨਹੀਂਤਾਂ ਜੇ (spin->si_sal.ga_len > 0)
	    smsg(_("Both SAL and SOFO lines in %s"), fname);
	ਨਹੀਂਤਾਂ
	{
	    aff_check_string(spin->si_sofofr, sofofrom, "SOFOFROM");
	    aff_check_string(spin->si_sofoto, sofoto, "SOFOTO");
	    spin->si_sofofr = sofofrom;
	    spin->si_sofoto = sofoto;
	}
    }

    ਜੇ (midword != ਨਿਰਾ)
    {
	aff_check_string(spin->si_midword, midword, "MIDWORD");
	spin->si_midword = midword;
    }

    vim_free(pc);
    fclose(fd);
    ਵਾਪਸ aff;
}

/*
 * Return TRUE when items[0] equals "rulename", there are "mincount" items or
 * a comment is following after item "mincount".
 */
    ਸਥਿਰ ਪੂਰਨ
is_aff_ਨਿਯਮ(
    char_u	**ਚੀਜ਼ਾਂ,
    ਪੂਰਨ		itemcnt,
    ਅੱਖਰ	*rulename,
    ਪੂਰਨ		mincount)
{
    ਵਾਪਸ (STRCMP(ਚੀਜ਼ਾਂ[0], rulename) == 0
	    && (itemcnt == mincount
		|| (itemcnt > mincount && ਚੀਜ਼ਾਂ[mincount][0] == '#')));
}

/*
 * For affix "entry" move COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG from
 * ae_flags to ae_comppermit and ae_compforbid.
 */
    ਸਥਿਰ ਖਾਲੀ
aff_process_flags(afffile_T *affile, affentry_T *ਇੰਦਰਾਜ)
{
    char_u	*p;
    char_u	*prevp;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	flag;

    ਜੇ (ਇੰਦਰਾਜ->ae_flags != ਨਿਰਾ
		&& (affile->af_compforbid != 0 || affile->af_comppermit != 0))
    {
	ਲਈ (p = ਇੰਦਰਾਜ->ae_flags; *p != NUL; )
	{
	    prevp = p;
	    flag = get_affitem(affile->af_flagtype, &p);
	    ਜੇ (flag == affile->af_comppermit || flag == affile->af_compforbid)
	    {
		STRMOVE(prevp, p);
		p = prevp;
		ਜੇ (flag == affile->af_comppermit)
		    ਇੰਦਰਾਜ->ae_comppermit = TRUE;
		ਨਹੀਂਤਾਂ
		    ਇੰਦਰਾਜ->ae_compforbid = TRUE;
	    }
	    ਜੇ (affile->af_flagtype == AFT_ਅੰਕ && *p == ',')
		++p;
	}
	ਜੇ (*ਇੰਦਰਾਜ->ae_flags == NUL)
	    ਇੰਦਰਾਜ->ae_flags = ਨਿਰਾ;	// nothing left
    }
}

/*
 * Return TRUE if "s" is the name of an info item in the affix file.
 */
    ਸਥਿਰ ਪੂਰਨ
spell_info_ਵਸਤੂ(char_u *s)
{
    ਵਾਪਸ STRCMP(s, "NAME") == 0
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
    ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ
affitem2flag(
    ਪੂਰਨ		flagtype,
    char_u	*ਵਸਤੂ,
    char_u	*fname,
    ਪੂਰਨ		lnum)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ	res;
    char_u	*p = ਵਸਤੂ;

    res = get_affitem(flagtype, &p);
    ਜੇ (res == 0)
    {
	ਜੇ (flagtype == AFT_ਅੰਕ)
	    smsg(_("Flag is not a number in %s line %d: %s"),
							   fname, lnum, ਵਸਤੂ);
	ਨਹੀਂਤਾਂ
	    smsg(_("Illegal flag in %s line %d: %s"),
							   fname, lnum, ਵਸਤੂ);
    }
    ਜੇ (*p != NUL)
    {
	smsg(_(e_affname), fname, lnum, ਵਸਤੂ);
	ਵਾਪਸ 0;
    }

    ਵਾਪਸ res;
}

/*
 * Get one affix name from "*pp" and advance the pointer.
 * Returns ZERO_FLAG for "0".
 * Returns zero for an error, still advances the pointer then.
 */
    ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ
get_affitem(ਪੂਰਨ flagtype, char_u **pp)
{
    ਪੂਰਨ		res;

    ਜੇ (flagtype == AFT_ਅੰਕ)
    {
	ਜੇ (!VIM_ISDIGIT(**pp))
	{
	    ++*pp;	// always advance, avoid getting stuck
	    ਵਾਪਸ 0;
	}
	res = getdigits(pp);
	ਜੇ (res == 0)
	    res = ZERO_FLAG;
    }
    ਨਹੀਂਤਾਂ
    {
	res = mb_ptr2char_adv(pp);
	ਜੇ (flagtype == AFT_LONG || (flagtype == AFT_CAPLONG
						 && res >= 'A' && res <= 'Z'))
	{
	    ਜੇ (**pp == NUL)
		ਵਾਪਸ 0;
	    res = mb_ptr2char_adv(pp) + (res << 16);
	}
    }
    ਵਾਪਸ res;
}

/*
 * Process the "compflags" string used in an affix file and append it to
 * spin->si_compflags.
 * The processing involves changing the affix names to ID numbers, so that
 * they fit in one byte.
 */
    ਸਥਿਰ ਖਾਲੀ
process_compflags(
    spellinfo_T	*spin,
    afffile_T	*aff,
    char_u	*compflags)
{
    char_u	*p;
    char_u	*prevp;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	flag;
    compitem_T	*ci;
    ਪੂਰਨ		id;
    ਪੂਰਨ		len;
    char_u	*tp;
    char_u	ਕੁੰਜੀ[AH_ਕੁੰਜੀ_LEN];
    hashitem_T	*hi;

    // Make room for the old and the new compflags, concatenated with a / in
    // between.  Processing it makes it shorter, but we don't know by how
    // much, thus allocate the maximum.
    len = (ਪੂਰਨ)STRLEN(compflags) + 1;
    ਜੇ (spin->si_compflags != ਨਿਰਾ)
	len += (ਪੂਰਨ)STRLEN(spin->si_compflags) + 1;
    p = getroom(spin, len, FALSE);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (spin->si_compflags != ਨਿਰਾ)
    {
	STRCPY(p, spin->si_compflags);
	STRCAT(p, "/");
    }
    spin->si_compflags = p;
    tp = p + STRLEN(p);

    ਲਈ (p = compflags; *p != NUL; )
    {
	ਜੇ (vim_strchr((char_u *)"/?*+[]", *p) != ਨਿਰਾ)
	    // Copy non-flag characters directly.
	    *tp++ = *p++;
	ਨਹੀਂਤਾਂ
	{
	    // First get the flag number, also checks validity.
	    prevp = p;
	    flag = get_affitem(aff->af_flagtype, &p);
	    ਜੇ (flag != 0)
	    {
		// Find the flag in the hashtable.  If it was used before, use
		// the existing ID.  Otherwise add a new entry.
		vim_strncpy(ਕੁੰਜੀ, prevp, p - prevp);
		hi = hash_ਲੱਭੋ(&aff->af_comp, ਕੁੰਜੀ);
		ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		    id = HI2CI(hi)->ci_newID;
		ਨਹੀਂਤਾਂ
		{
		    ci = (compitem_T *)getroom(spin, ਆਕਾਰ(compitem_T), TRUE);
		    ਜੇ (ci == ਨਿਰਾ)
			ਰੋਕੋ;
		    STRCPY(ci->ci_ਕੁੰਜੀ, ਕੁੰਜੀ);
		    ci->ci_flag = flag;
		    // Avoid using a flag ID that has a special meaning in a
		    // regexp (also inside []).
		    ਕਰੋ
		    {
			check_renumber(spin);
			id = spin->si_newcompID--;
		    } ਜਦੋਂ (vim_strchr((char_u *)"/?*+[]\\-^", id) != ਨਿਰਾ);
		    ci->ci_newID = id;
		    hash_ਜੋੜੋ(&aff->af_comp, ci->ci_ਕੁੰਜੀ, "spelling");
		}
		*tp++ = id;
	    }
	    ਜੇ (aff->af_flagtype == AFT_ਅੰਕ && *p == ',')
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
    ਸਥਿਰ ਖਾਲੀ
check_renumber(spellinfo_T *spin)
{
    ਜੇ (spin->si_newprefID == spin->si_newcompID && spin->si_newcompID < 128)
    {
	spin->si_newprefID = 127;
	spin->si_newcompID = 255;
    }
}

/*
 * Append one affix or compound ID to "store_afflist".
 * Returns FAIL when this would overrun the fixed-size buffer.
 */
    ਸਥਿਰ ਪੂਰਨ
store_afflist_ਜੋੜੋ(
    char_u	*store_afflist,
    ਪੂਰਨ		*cntp,
    ਪੂਰਨ		id)
{
    ਜੇ (*cntp >= MAXWLEN - 1)
    {
	emsg(_(e_too_many_postponed_prefixes_spell));
	ਵਾਪਸ FAIL;
    }
    store_afflist[(*cntp)++] = id;
    store_afflist[*cntp] = NUL;
    ਵਾਪਸ OK;
}

/*
 * Return TRUE if flag "flag" appears in affix list "afflist".
 */
    ਸਥਿਰ ਪੂਰਨ
flag_ਵਿੱਚ_afflist(ਪੂਰਨ flagtype, char_u *afflist, ਬਿਨਾਨਿਸ਼ਾਨੀ flag)
{
    char_u	*p;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	n;

    ਚੋਣ (flagtype)
    {
	ਹਾਲਤ AFT_CHAR:
	    ਵਾਪਸ vim_strchr(afflist, flag) != ਨਿਰਾ;

	ਹਾਲਤ AFT_CAPLONG:
	ਹਾਲਤ AFT_LONG:
	    ਲਈ (p = afflist; *p != NUL; )
	    {
		n = mb_ptr2char_adv(&p);
		ਜੇ ((flagtype == AFT_LONG || (n >= 'A' && n <= 'Z'))
								 && *p != NUL)
		    n = mb_ptr2char_adv(&p) + (n << 16);
		ਜੇ (n == flag)
		    ਵਾਪਸ TRUE;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ AFT_ਅੰਕ:
	    ਲਈ (p = afflist; *p != NUL; )
	    {
		n = getdigits(&p);
		ਜੇ (n == 0)
		    n = ZERO_FLAG;
		ਜੇ (n == flag)
		    ਵਾਪਸ TRUE;
		ਜੇ (*p != NUL)	// skip over comma
		    ++p;
	    }
	    ਰੋਕੋ;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Give a warning when "spinval" and "affval" numbers are set and not the same.
 */
    ਸਥਿਰ ਖਾਲੀ
aff_check_ਸੰਖਿਆ(ਪੂਰਨ spinval, ਪੂਰਨ affval, ਅੱਖਰ *name)
{
    ਜੇ (spinval != 0 && spinval != affval)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Give a warning when "spinval" and "affval" strings are set and not the same.
 */
    ਸਥਿਰ ਖਾਲੀ
aff_check_string(char_u *spinval, char_u *affval, ਅੱਖਰ *name)
{
    ਜੇ (spinval != ਨਿਰਾ && STRCMP(spinval, affval) != 0)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Return TRUE if strings "s1" and "s2" are equal.  Also consider both being
 * NULL as equal.
 */
    ਸਥਿਰ ਪੂਰਨ
str_equal(char_u *s1, char_u *s2)
{
    ਜੇ (s1 == ਨਿਰਾ || s2 == ਨਿਰਾ)
	ਵਾਪਸ s1 == s2;
    ਵਾਪਸ STRCMP(s1, s2) == 0;
}

/*
 * Add a from-to item to "gap".  Used for REP and SAL items.
 * They are stored case-folded.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_fromto(
    spellinfo_T	*spin,
    garray_T	*gap,
    char_u	*from,
    char_u	*ਮੰਜ਼ਿਲ)
{
    fromto_T	*ftp;
    char_u	ਸ਼ਬਦ[MAXWLEN];

    ਜੇ (ga_grow(gap, 1) == FAIL)
	ਵਾਪਸ;

    ftp = ((fromto_T *)gap->ga_ਅੰਕੜੇ) + gap->ga_len;
    (ਖਾਲੀ)spell_casefold(curwin, from, (ਪੂਰਨ)STRLEN(from), ਸ਼ਬਦ, MAXWLEN);
    ftp->ft_from = getroom_save(spin, ਸ਼ਬਦ);
    (ਖਾਲੀ)spell_casefold(curwin, ਮੰਜ਼ਿਲ, (ਪੂਰਨ)STRLEN(ਮੰਜ਼ਿਲ), ਸ਼ਬਦ, MAXWLEN);
    ftp->ft_ਮੰਜ਼ਿਲ = getroom_save(spin, ਸ਼ਬਦ);
    ++gap->ga_len;
}

/*
 * Convert a boolean argument in a SAL line to TRUE or FALSE;
 */
    ਸਥਿਰ ਪੂਰਨ
sal_ਮੰਜ਼ਿਲ_bool(char_u *s)
{
    ਵਾਪਸ STRCMP(s, "1") == 0 || STRCMP(s, "true") == 0;
}

/*
 * Free the structure filled by spell_read_aff().
 */
    ਸਥਿਰ ਖਾਲੀ
spell_free_aff(afffile_T *aff)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    ਪੂਰਨ		todo;
    affheader_T	*ah;
    affentry_T	*ae;

    vim_free(aff->af_enc);

    // All this trouble to free the "ae_prog" items...
    ਲਈ (ht = &aff->af_pref; ; ht = &aff->af_suff)
    {
	todo = (ਪੂਰਨ)ht->ht_ਵਰਤਿਆ;
	FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(ht, hi, todo)
	{
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    {
		--todo;
		ah = HI2AH(hi);
		ਲਈ (ae = ah->ah_ਪਹਿਲਾ; ae != ਨਿਰਾ; ae = ae->ae_ਅਗਲਾ)
		    vim_regfree(ae->ae_prog);
	    }
	}
	ਜੇ (ht == &aff->af_suff)
	    ਰੋਕੋ;
    }

    hash_clear(&aff->af_pref);
    hash_clear(&aff->af_suff);
    hash_clear(&aff->af_comp);
}

/*
 * Read dictionary file "fname".
 * Returns OK or FAIL;
 */
    ਸਥਿਰ ਪੂਰਨ
spell_read_dic(spellinfo_T *spin, char_u *fname, afffile_T *affile)
{
    hashtab_T	ht;
    char_u	ਸਤਰ[MAXLINELEN];
    char_u	*p;
    char_u	*afflist;
    char_u	store_afflist[MAXWLEN];
    ਪੂਰਨ		pfxlen;
    ਪੂਰਨ		totlen;
    ਪੂਰਨ		need_affix;
    char_u	*dw;
    char_u	*pc;
    char_u	*w;
    ਪੂਰਨ		l;
    hash_T	hash;
    hashitem_T	*hi;
    FILE	*fd;
    ਪੂਰਨ		lnum = 1;
    ਪੂਰਨ		non_ascii = 0;
    ਪੂਰਨ		retval = OK;
    char_u	ਸੁਨੇਹਾ_2[MAXLINELEN + MAXWLEN];
    ਪੂਰਨ		flags;
    ਪੂਰਨ		duplicate = 0;
    ਸਮਾਂ_T	ਆਖਰੀ_ਸੁਨੇਹਾ_ਸਮਾਂ = 0;

    /*
     * Open the file.
     */
    fd = mch_fopen((ਅੱਖਰ *)fname, "r");
    ਜੇ (fd == ਨਿਰਾ)
    {
	semsg(_(e_cant_open_file_str), fname);
	ਵਾਪਸ FAIL;
    }

    // The hashtable is only used to detect duplicated words.
    hash_init(&ht);

    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
				  _("Reading dictionary file %s..."), fname);
    spell_ਸੁਨੇਹਾ(spin, IObuff);

    // start with a message for the first line
    spin->si_ਸੁਨੇਹਾ_ਗਿਣਤੀ = 999999;

    // Read and ignore the first line: word count.
    ਜੇ (vim_fgets(ਸਤਰ, MAXLINELEN, fd) || !vim_isdigit(*skipwhite(ਸਤਰ)))
	semsg(_(e_no_ਸ਼ਬਦ_ਗਿਣਤੀ_ਵਿੱਚ_str), fname);

    /*
     * Read all the lines in the file one by one.
     * The words are converted to 'encoding' here, before being added to
     * the hashtable.
     */
    ਜਦੋਂ (!vim_fgets(ਸਤਰ, MAXLINELEN, fd) && !got_int)
    {
	ਸਤਰ_breakcheck();
	++lnum;
	ਜੇ (ਸਤਰ[0] == '#' || ਸਤਰ[0] == '/')
	    ਜਾਰੀ;	// comment line

	// Remove CR, LF and white space from the end.  White space halfway
	// the word is kept to allow e.g., "et al.".
	l = (ਪੂਰਨ)STRLEN(ਸਤਰ);
	ਜਦੋਂ (l > 0 && ਸਤਰ[l - 1] <= ' ')
	    --l;
	ਜੇ (l == 0)
	    ਜਾਰੀ;	// empty line
	ਸਤਰ[l] = NUL;

	// Convert from "SET" to 'encoding' when needed.
	ਜੇ (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, ਸਤਰ, ਨਿਰਾ);
	    ਜੇ (pc == ਨਿਰਾ)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
						       fname, lnum, ਸਤਰ);
		ਜਾਰੀ;
	    }
	    w = pc;
	}
	ਨਹੀਂਤਾਂ
	{
	    pc = ਨਿਰਾ;
	    w = ਸਤਰ;
	}

	// Truncate the word at the "/", set "afflist" to what follows.
	// Replace "\/" by "/" and "\\" by "\".
	afflist = ਨਿਰਾ;
	ਲਈ (p = w; *p != NUL; MB_PTR_ADV(p))
	{
	    ਜੇ (*p == '\\' && (p[1] == '\\' || p[1] == '/'))
		STRMOVE(p, p + 1);
	    ਨਹੀਂਤਾਂ ਜੇ (*p == '/')
	    {
		*p = NUL;
		afflist = p + 1;
		ਰੋਕੋ;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	ਜੇ (spin->si_ascii && has_non_ascii(w))
	{
	    ++non_ascii;
	    vim_free(pc);
	    ਜਾਰੀ;
	}

	// This takes time, print a message every 10000 words, but not more
	// often than once per second.
	ਜੇ (spin->si_verbose && spin->si_ਸੁਨੇਹਾ_ਗਿਣਤੀ > 10000)
	{
	    spin->si_ਸੁਨੇਹਾ_ਗਿਣਤੀ = 0;
	    ਜੇ (vim_ਸਮਾਂ() > ਆਖਰੀ_ਸੁਨੇਹਾ_ਸਮਾਂ)
	    {
		ਆਖਰੀ_ਸੁਨੇਹਾ_ਸਮਾਂ = vim_ਸਮਾਂ();
		vim_snprintf((ਅੱਖਰ *)ਸੁਨੇਹਾ_2, ਆਕਾਰ(ਸੁਨੇਹਾ_2),
			_("line %6d, word %6ld - %s"),
			   lnum, spin->si_foldwcount + spin->si_keepwcount, w);
		ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
		ਸੁਨੇਹਾ_outtrans_long_attr(ਸੁਨੇਹਾ_2, 0);
		ਸੁਨੇਹਾ_clr_eos();
		ਸੁਨੇਹਾ_didout = FALSE;
		ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
		ਬਾਹਰਲਾ_flush();
	    }
	}

	// Store the word in the hashtable to be able to find duplicates.
	dw = getroom_save(spin, w);
	ਜੇ (dw == ਨਿਰਾ)
	{
	    retval = FAIL;
	    vim_free(pc);
	    ਰੋਕੋ;
	}

	hash = hash_hash(dw);
	hi = hash_lookup(&ht, dw, hash);
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    ਜੇ (p_verbose > 0)
		smsg(_("Duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ਨਹੀਂਤਾਂ ਜੇ (duplicate == 0)
		smsg(_("First duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ++duplicate;
	}
	ਨਹੀਂਤਾਂ
	    hash_ਜੋੜੋ_ਵਸਤੂ(&ht, hi, dw, hash);

	flags = 0;
	store_afflist[0] = NUL;
	pfxlen = 0;
	totlen = 0;
	need_affix = FALSE;
	ਜੇ (afflist != ਨਿਰਾ)
	{
	    // Extract flags from the affix list.
	    flags |= get_affix_flags(affile, afflist);

	    ਜੇ (affile->af_needaffix != 0 && flag_ਵਿੱਚ_afflist(
			  affile->af_flagtype, afflist, affile->af_needaffix))
		need_affix = TRUE;

	    ਜੇ (affile->af_pfxpostpone)
	    {
		// Need to store the list of prefix IDs with the word.
		ਜੇ (get_pfxlist(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    ਰੋਕੋ;
		}
		pfxlen = totlen;
	    }

	    ਜੇ (spin->si_compflags != ਨਿਰਾ)
	    {
		// Need to store the list of compound flags with the word.
		// Concatenate them to the list of prefix IDs.
		ਜੇ (get_compflags(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    ਰੋਕੋ;
		}
	    }
	}

	// Add the word to the word tree(s).
	ਜੇ (store_ਸ਼ਬਦ(spin, dw, flags, spin->si_region,
					   store_afflist, need_affix) == FAIL)
	    retval = FAIL;

	ਜੇ (afflist != ਨਿਰਾ)
	{
	    // Find all matching suffixes and add the resulting words.
	    // Additionally do matching prefixes that combine.
	    ਜੇ (store_aff_ਸ਼ਬਦ(spin, dw, afflist, affile,
			   &affile->af_suff, &affile->af_pref,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;

	    // Find all matching prefixes and add the resulting words.
	    ਜੇ (store_aff_ਸ਼ਬਦ(spin, dw, afflist, affile,
			  &affile->af_pref, ਨਿਰਾ,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;
	}

	vim_free(pc);
    }

    ਜੇ (duplicate > 0)
	smsg(_("%d duplicate word(s) in %s"), duplicate, fname);
    ਜੇ (spin->si_ascii && non_ascii > 0)
	smsg(_("Ignored %d word(s) with non-ASCII characters in %s"),
							    non_ascii, fname);
    hash_clear(&ht);

    fclose(fd);
    ਵਾਪਸ retval;
}

/*
 * Check for affix flags in "afflist" that are turned into word flags.
 * Return WF_ flags.
 */
    ਸਥਿਰ ਪੂਰਨ
get_affix_flags(afffile_T *affile, char_u *afflist)
{
    ਪੂਰਨ		flags = 0;

    ਜੇ (affile->af_keepcase != 0 && flag_ਵਿੱਚ_afflist(
			   affile->af_flagtype, afflist, affile->af_keepcase))
	flags |= WF_KEEPCAP | WF_FIXCAP;
    ਜੇ (affile->af_rare != 0 && flag_ਵਿੱਚ_afflist(
			       affile->af_flagtype, afflist, affile->af_rare))
	flags |= WF_RARE;
    ਜੇ (affile->af_bad != 0 && flag_ਵਿੱਚ_afflist(
				affile->af_flagtype, afflist, affile->af_bad))
	flags |= WF_BANNED;
    ਜੇ (affile->af_needcomp != 0 && flag_ਵਿੱਚ_afflist(
			   affile->af_flagtype, afflist, affile->af_needcomp))
	flags |= WF_NEEDCOMP;
    ਜੇ (affile->af_comproot != 0 && flag_ਵਿੱਚ_afflist(
			   affile->af_flagtype, afflist, affile->af_comproot))
	flags |= WF_COMPROOT;
    ਜੇ (affile->af_nosuggest != 0 && flag_ਵਿੱਚ_afflist(
			  affile->af_flagtype, afflist, affile->af_nosuggest))
	flags |= WF_NOSUGGEST;
    ਵਾਪਸ flags;
}

/*
 * Get the list of prefix IDs from the affix list "afflist".
 * Used for PFXPOSTPONE.
 * Put the resulting flags in "store_afflist[MAXWLEN]" with a terminating NUL.
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    ਸਥਿਰ ਪੂਰਨ
get_pfxlist(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    ਪੂਰਨ		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    ਪੂਰਨ		id;
    char_u	ਕੁੰਜੀ[AH_ਕੁੰਜੀ_LEN];
    hashitem_T	*hi;

    ਲਈ (p = afflist; *p != NUL; )
    {
	prevp = p;
	ਜੇ (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a postponed prefix flag if it appears in "af_pref"
	    // and its ID is not zero.
	    vim_strncpy(ਕੁੰਜੀ, prevp, p - prevp);
	    hi = hash_ਲੱਭੋ(&affile->af_pref, ਕੁੰਜੀ);
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    {
		id = HI2AH(hi)->ah_newID;
		ਜੇ (id != 0 && store_afflist_ਜੋੜੋ(store_afflist, cntp, id) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	}
	ਜੇ (affile->af_flagtype == AFT_ਅੰਕ && *p == ',')
	    ++p;
    }

    ਵਾਪਸ OK;
}

/*
 * Get the list of compound IDs from the affix list "afflist" that are used
 * for compound words.
 * Puts the flags in "store_afflist[]".
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    ਸਥਿਰ ਪੂਰਨ
get_compflags(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    ਪੂਰਨ		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    char_u	ਕੁੰਜੀ[AH_ਕੁੰਜੀ_LEN];
    hashitem_T	*hi;

    ਲਈ (p = afflist; *p != NUL; )
    {
	prevp = p;
	ਜੇ (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a compound flag if it appears in "af_comp".
	    vim_strncpy(ਕੁੰਜੀ, prevp, p - prevp);
	    hi = hash_ਲੱਭੋ(&affile->af_comp, ਕੁੰਜੀ);
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi)
		    && store_afflist_ਜੋੜੋ(store_afflist, cntp,
					      HI2CI(hi)->ci_newID) == FAIL)
		ਵਾਪਸ FAIL;
	}
	ਜੇ (affile->af_flagtype == AFT_ਅੰਕ && *p == ',')
	    ++p;
    }

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
store_aff_ਸ਼ਬਦ(
    spellinfo_T	*spin,		// spell info
    char_u	*ਸ਼ਬਦ,		// basic word start
    char_u	*afflist,	// list of names of supported affixes
    afffile_T	*affile,
    hashtab_T	*ht,
    hashtab_T	*xht,
    ਪੂਰਨ		condit,		// CONDIT_SUF et al.
    ਪੂਰਨ		flags,		// flags for the word
    char_u	*pfxlist,	// list of prefix IDs
    ਪੂਰਨ		pfxlen)		// nr of flags in "pfxlist" for prefixes, rest
				// is compound flags
{
    ਪੂਰਨ		todo;
    hashitem_T	*hi;
    affheader_T	*ah;
    affentry_T	*ae;
    char_u	newword[MAXWLEN];
    ਪੂਰਨ		retval = OK;
    ਪੂਰਨ		i, j;
    char_u	*p;
    ਪੂਰਨ		use_flags;
    char_u	*use_pfxlist;
    ਪੂਰਨ		use_pfxlen;
    ਪੂਰਨ		need_affix;
    char_u	store_afflist[MAXWLEN];
    char_u	pfx_pfxlist[MAXWLEN];
    size_t	wordlen = STRLEN(ਸ਼ਬਦ);
    ਪੂਰਨ		use_condit;

    todo = (ਪੂਰਨ)ht->ht_ਵਰਤਿਆ;
    ਲਈ (hi = ht->ht_array; todo > 0 && retval == OK; ++hi)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;
	    ah = HI2AH(hi);

	    // Check that the affix combines, if required, and that the word
	    // supports this affix.
	    ਜੇ (((condit & CONDIT_COMB) == 0 || ah->ah_combine)
		    && flag_ਵਿੱਚ_afflist(affile->af_flagtype, afflist,
								 ah->ah_flag))
	    {
		// Loop over all affix entries with this name.
		ਲਈ (ae = ah->ah_ਪਹਿਲਾ; ae != ਨਿਰਾ; ae = ae->ae_ਅਗਲਾ)
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
		    ਜੇ ((xht != ਨਿਰਾ || !affile->af_pfxpostpone
				|| ae->ae_chop != ਨਿਰਾ
				|| ae->ae_flags != ਨਿਰਾ)
			    && (ae->ae_chop == ਨਿਰਾ
				|| STRLEN(ae->ae_chop) < wordlen)
			    && (ae->ae_prog == ਨਿਰਾ
				|| vim_regexec_prog(&ae->ae_prog, FALSE,
							    ਸ਼ਬਦ, (colnr_T)0))
			    && (((condit & CONDIT_CFIX) == 0)
				== ((condit & CONDIT_AFF) == 0
				    || ae->ae_flags == ਨਿਰਾ
				    || !flag_ਵਿੱਚ_afflist(affile->af_flagtype,
					ae->ae_flags, affile->af_circumfix))))
		    {
			// Match.  Remove the chop and add the affix.
			ਜੇ (xht == ਨਿਰਾ)
			{
			    // prefix: chop/add at the start of the word
			    ਜੇ (ae->ae_ਜੋੜੋ == ਨਿਰਾ)
				*newword = NUL;
			    ਨਹੀਂਤਾਂ
				vim_strncpy(newword, ae->ae_ਜੋੜੋ, MAXWLEN - 1);
			    p = ਸ਼ਬਦ;
			    ਜੇ (ae->ae_chop != ਨਿਰਾ)
			    {
				// Skip chop string.
				ਜੇ (has_mbyte)
				{
				    i = mb_charlen(ae->ae_chop);
				    ਲਈ ( ; i > 0; --i)
					MB_PTR_ADV(p);
				}
				ਨਹੀਂਤਾਂ
				    p += STRLEN(ae->ae_chop);
			    }
			    // Note: this silently truncates the buffer, but this should
			    // not happen in practice
			    STRNCAT(newword, p, MAXWLEN - STRLEN(newword) - 1);
			}
			ਨਹੀਂਤਾਂ
			{
			    // suffix: chop/add at the end of the word
			    vim_strncpy(newword, ਸ਼ਬਦ, MAXWLEN - 1);
			    ਜੇ (ae->ae_chop != ਨਿਰਾ)
			    {
				// Remove chop string.
				p = newword + STRLEN(newword);
				i = (ਪੂਰਨ)MB_CHARLEN(ae->ae_chop);
				ਲਈ ( ; i > 0; --i)
				    MB_PTR_ਪਿੱਠ(newword, p);
				*p = NUL;
			    }
			    ਜੇ (ae->ae_ਜੋੜੋ != ਨਿਰਾ)
				// Note: this silently truncates the buffer, but this should
				// not happen in practice
				STRNCAT(newword, ae->ae_ਜੋੜੋ, MAXWLEN - STRLEN(newword) - 1);
			}

			use_flags = flags;
			use_pfxlist = pfxlist;
			use_pfxlen = pfxlen;
			need_affix = FALSE;
			use_condit = condit | CONDIT_COMB | CONDIT_AFF;
			ਜੇ (ae->ae_flags != ਨਿਰਾ)
			{
			    // Extract flags from the affix list.
			    use_flags |= get_affix_flags(affile, ae->ae_flags);

			    ਜੇ (affile->af_needaffix != 0 && flag_ਵਿੱਚ_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_needaffix))
				need_affix = TRUE;

			    // When there is a CIRCUMFIX flag the other affix
			    // must also have it and we don't add the word
			    // with one affix.
			    ਜੇ (affile->af_circumfix != 0 && flag_ਵਿੱਚ_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_circumfix))
			    {
				use_condit |= CONDIT_CFIX;
				ਜੇ ((condit & CONDIT_CFIX) == 0)
				    need_affix = TRUE;
			    }

			    ਜੇ (affile->af_pfxpostpone
						|| spin->si_compflags != ਨਿਰਾ)
			    {
				ਪੂਰਨ listlen = 0;

				ਜੇ (affile->af_pfxpostpone)
				{
				    // Get prefix IDS from the affix list.
				    ਜੇ (get_pfxlist(affile, ae->ae_flags,
						    store_afflist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					ਰੋਕੋ;
				    }
				    use_pfxlen = listlen;
				}
				ਨਹੀਂਤਾਂ
				    use_pfxlen = 0;
				use_pfxlist = store_afflist;

				// Combine the prefix IDs. Avoid adding the
				// same ID twice.
				ਲਈ (i = 0; i < pfxlen; ++i)
				{
				    ਲਈ (j = 0; j < use_pfxlen; ++j)
					ਜੇ (pfxlist[i] == use_pfxlist[j])
					    ਰੋਕੋ;
				    ਜੇ (j == use_pfxlen
					    && store_afflist_ਜੋੜੋ(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					ਰੋਕੋ;
				    }
				    use_pfxlen = listlen;
				}
				ਜੇ (retval == FAIL)
				    ਰੋਕੋ;

				ਜੇ (spin->si_compflags != ਨਿਰਾ)
				    // Get compound IDS from the affix list.
				    ਜੇ (get_compflags(affile, ae->ae_flags,
						      use_pfxlist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					ਰੋਕੋ;
				    }
				ਜੇ (retval == FAIL)
				    ਰੋਕੋ;

				// Combine the list of compound flags.
				// Concatenate them to the prefix IDs list.
				// Avoid adding the same ID twice.
				ਲਈ (i = pfxlen; pfxlist[i] != NUL; ++i)
				{
				    ਲਈ (j = use_pfxlen;
						   use_pfxlist[j] != NUL; ++j)
					ਜੇ (pfxlist[i] == use_pfxlist[j])
					    ਰੋਕੋ;
				    ਜੇ (use_pfxlist[j] == NUL
					    && store_afflist_ਜੋੜੋ(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					ਰੋਕੋ;
				    }
				}
				ਜੇ (retval == FAIL)
				    ਰੋਕੋ;
			    }
			}

			// Obey a "COMPOUNDFORBIDFLAG" of the affix: don't
			// use the compound flags.
			ਜੇ (use_pfxlist != ਨਿਰਾ && ae->ae_compforbid)
			{
			    vim_strncpy(pfx_pfxlist, use_pfxlist, use_pfxlen);
			    use_pfxlist = pfx_pfxlist;
			}

			// When there are postponed prefixes...
			ਜੇ (spin->si_prefroot != ਨਿਰਾ
				&& spin->si_prefroot->wn_sibling != ਨਿਰਾ)
			{
			    // ... add a flag to indicate an affix was used.
			    use_flags |= WF_HAS_AFF;

			    // ... don't use a prefix list if combining
			    // affixes is not allowed.  But do use the
			    // compound flags after them.
			    ਜੇ (!ah->ah_combine && use_pfxlist != ਨਿਰਾ)
				use_pfxlist += use_pfxlen;
			}

			// When compounding is supported and there is no
			// "COMPOUNDPERMITFLAG" then forbid compounding on the
			// side where the affix is applied.
			ਜੇ (spin->si_compflags != ਨਿਰਾ && !ae->ae_comppermit)
			{
			    ਜੇ (xht != ਨਿਰਾ)
				use_flags |= WF_NOCOMPAFT;
			    ਨਹੀਂਤਾਂ
				use_flags |= WF_NOCOMPBEF;
			}

			// Store the modified word.
			ਜੇ (store_ਸ਼ਬਦ(spin, newword, use_flags,
						 spin->si_region, use_pfxlist,
							  need_affix) == FAIL)
			    retval = FAIL;

			// When added a prefix or a first suffix and the affix
			// has flags may add a(nother) suffix.  RECURSIVE!
			ਜੇ ((condit & CONDIT_SUF) && ae->ae_flags != ਨਿਰਾ)
			    ਜੇ (store_aff_ਸ਼ਬਦ(spin, newword, ae->ae_flags,
					affile, &affile->af_suff, xht,
					   use_condit & (xht == ਨਿਰਾ
							? ~0 :  ~CONDIT_SUF),
				      use_flags, use_pfxlist, pfxlen) == FAIL)
				retval = FAIL;

			// When added a suffix and combining is allowed also
			// try adding a prefix additionally.  Both for the
			// word flags and for the affix flags.  RECURSIVE!
			ਜੇ (xht != ਨਿਰਾ && ah->ah_combine)
			{
			    ਜੇ (store_aff_ਸ਼ਬਦ(spin, newword,
					afflist, affile,
					xht, ਨਿਰਾ, use_condit,
					use_flags, use_pfxlist,
					pfxlen) == FAIL
				    || (ae->ae_flags != ਨਿਰਾ
					&& store_aff_ਸ਼ਬਦ(spin, newword,
					    ae->ae_flags, affile,
					    xht, ਨਿਰਾ, use_condit,
					    use_flags, use_pfxlist,
					    pfxlen) == FAIL))
				retval = FAIL;
			}
		    }
		}
	    }
	}
    }

    ਵਾਪਸ retval;
}

/*
 * Read a file with a list of words.
 */
    ਸਥਿਰ ਪੂਰਨ
spell_read_wordfile(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    ਲੰਮਾ	lnum = 0;
    char_u	rline[MAXLINELEN];
    char_u	*ਸਤਰ;
    char_u	*pc = ਨਿਰਾ;
    char_u	*p;
    ਪੂਰਨ		l;
    ਪੂਰਨ		retval = OK;
    ਪੂਰਨ		did_ਸ਼ਬਦ = FALSE;
    ਪੂਰਨ		non_ascii = 0;
    ਪੂਰਨ		flags;
    ਪੂਰਨ		regionmask;

    /*
     * Open the file.
     */
    fd = mch_fopen((ਅੱਖਰ *)fname, "r");
    ਜੇ (fd == ਨਿਰਾ)
    {
	semsg(_(e_cant_open_file_str), fname);
	ਵਾਪਸ FAIL;
    }

    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, _("Reading word file %s..."), fname);
    spell_ਸੁਨੇਹਾ(spin, IObuff);

    /*
     * Read all the lines in the file one by one.
     */
    ਜਦੋਂ (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	ਸਤਰ_breakcheck();
	++lnum;

	// Skip comment lines.
	ਜੇ (*rline == '#')
	    ਜਾਰੀ;

	// Remove CR, LF and white space from the end.
	l = (ਪੂਰਨ)STRLEN(rline);
	ਜਦੋਂ (l > 0 && rline[l - 1] <= ' ')
	    --l;
	ਜੇ (l == 0)
	    ਜਾਰੀ;	// empty or blank line
	rline[l] = NUL;

	// Convert from "/encoding={encoding}" to 'encoding' when needed.
	vim_free(pc);
	ਜੇ (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, ਨਿਰਾ);
	    ਜੇ (pc == ਨਿਰਾ)
	    {
		smsg(_("Conversion failure for word in %s line %ld: %s"),
							   fname, lnum, rline);
		ਜਾਰੀ;
	    }
	    ਸਤਰ = pc;
	}
	ਨਹੀਂਤਾਂ
	{
	    pc = ਨਿਰਾ;
	    ਸਤਰ = rline;
	}

	ਜੇ (*ਸਤਰ == '/')
	{
	    ++ਸਤਰ;
	    ਜੇ (STRNCMP(ਸਤਰ, "encoding=", 9) == 0)
	    {
		ਜੇ (spin->si_conv.vc_type != CONV_NONE)
		    smsg(_("Duplicate /encoding= line ignored in %s line %ld: %s"),
						       fname, lnum, ਸਤਰ - 1);
		ਨਹੀਂਤਾਂ ਜੇ (did_ਸ਼ਬਦ)
		    smsg(_("/encoding= line after word ignored in %s line %ld: %s"),
						       fname, lnum, ਸਤਰ - 1);
		ਨਹੀਂਤਾਂ
		{
		    char_u	*enc;

		    // Setup for conversion to 'encoding'.
		    ਸਤਰ += 9;
		    enc = enc_canonize(ਸਤਰ);
		    ਜੇ (enc != ਨਿਰਾ && !spin->si_ascii
			    && convert_ਤਿਆਰੀ(&spin->si_conv, enc,
							       p_enc) == FAIL)
			smsg(_("Conversion in %s not supported: from %s to %s"),
							  fname, ਸਤਰ, p_enc);
		    vim_free(enc);
		    spin->si_conv.vc_fail = TRUE;
		}
		ਜਾਰੀ;
	    }

	    ਜੇ (STRNCMP(ਸਤਰ, "regions=", 8) == 0)
	    {
		ਜੇ (spin->si_region_ਗਿਣਤੀ > 1)
		    smsg(_("Duplicate /regions= line ignored in %s line %ld: %s"),
						       fname, lnum, ਸਤਰ);
		ਨਹੀਂਤਾਂ
		{
		    ਸਤਰ += 8;
		    ਜੇ (STRLEN(ਸਤਰ) > MAXREGIONS * 2)
			smsg(_("Too many regions in %s line %ld: %s"),
						       fname, lnum, ਸਤਰ);
		    ਨਹੀਂਤਾਂ
		    {
			spin->si_region_ਗਿਣਤੀ = (ਪੂਰਨ)STRLEN(ਸਤਰ) / 2;
			STRCPY(spin->si_region_name, ਸਤਰ);

			// Adjust the mask for a word valid in all regions.
			spin->si_region = (1 << spin->si_region_ਗਿਣਤੀ) - 1;
		    }
		}
		ਜਾਰੀ;
	    }

	    smsg(_("/ line ignored in %s line %ld: %s"),
						       fname, lnum, ਸਤਰ - 1);
	    ਜਾਰੀ;
	}

	flags = 0;
	regionmask = spin->si_region;

	// Check for flags and region after a slash.
	p = vim_strchr(ਸਤਰ, '/');
	ਜੇ (p != ਨਿਰਾ)
	{
	    *p++ = NUL;
	    ਜਦੋਂ (*p != NUL)
	    {
		ਜੇ (*p == '=')		// keep-case word
		    flags |= WF_KEEPCAP | WF_FIXCAP;
		ਨਹੀਂਤਾਂ ਜੇ (*p == '!')	// Bad, bad, wicked word.
		    flags |= WF_BANNED;
		ਨਹੀਂਤਾਂ ਜੇ (*p == '?')	// Rare word.
		    flags |= WF_RARE;
		ਨਹੀਂਤਾਂ ਜੇ (VIM_ISDIGIT(*p)) // region number(s)
		{
		    ਜੇ ((flags & WF_REGION) == 0)   // first one
			regionmask = 0;
		    flags |= WF_REGION;

		    l = *p - '0';
		    ਜੇ (l == 0 || l > spin->si_region_ਗਿਣਤੀ)
		    {
			smsg(_("Invalid region nr in %s line %ld: %s"),
							  fname, lnum, p);
			ਰੋਕੋ;
		    }
		    regionmask |= 1 << (l - 1);
		}
		ਨਹੀਂਤਾਂ
		{
		    smsg(_("Unrecognized flags in %s line %ld: %s"),
							      fname, lnum, p);
		    ਰੋਕੋ;
		}
		++p;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	ਜੇ (spin->si_ascii && has_non_ascii(ਸਤਰ))
	{
	    ++non_ascii;
	    ਜਾਰੀ;
	}

	// Normal word: store it.
	ਜੇ (store_ਸ਼ਬਦ(spin, ਸਤਰ, flags, regionmask, ਨਿਰਾ, FALSE) == FAIL)
	{
	    retval = FAIL;
	    ਰੋਕੋ;
	}
	did_ਸ਼ਬਦ = TRUE;
    }

    vim_free(pc);
    fclose(fd);

    ਜੇ (spin->si_ascii && non_ascii > 0)
    {
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
		  _("Ignored %d words with non-ASCII characters"), non_ascii);
	spell_ਸੁਨੇਹਾ(spin, IObuff);
    }

    ਵਾਪਸ retval;
}

/*
 * Get part of an sblock_T, "len" bytes long.
 * This avoids calling free() for every little struct we use (and keeping
 * track of them).
 * The memory is cleared to all zeros.
 * Returns NULL when out of memory.
 */
    ਸਥਿਰ ਖਾਲੀ *
getroom(
    spellinfo_T *spin,
    size_t	len,		// length needed
    ਪੂਰਨ		align)		// align for pointer
{
    char_u	*p;
    sblock_T	*bl = spin->si_ਖੰਡਾਂ;

    ਜੇ (align && bl != ਨਿਰਾ)
	// Round size up for alignment.  On some systems structures need to be
	// aligned to the size of a pointer (e.g., SPARC).
	bl->sb_ਵਰਤਿਆ = (bl->sb_ਵਰਤਿਆ + ਆਕਾਰ(ਅੱਖਰ *) - 1)
						      & ~(ਆਕਾਰ(ਅੱਖਰ *) - 1);

    ਜੇ (bl == ਨਿਰਾ || bl->sb_ਵਰਤਿਆ + len > SBLOCKSIZE)
    {
	ਜੇ (len >= SBLOCKSIZE)
	    bl = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    // Allocate a block of memory. It is not freed until much later.
	    bl = alloc_clear(offsetof(sblock_T, sb_ਅੰਕੜੇ) + SBLOCKSIZE + 1);
	ਜੇ (bl == ਨਿਰਾ)
	{
	    ਜੇ (!spin->si_did_emsg)
	    {
		emsg(_(e_insufficient_ਸਮ੍ਰਿਤੀ_ਸ਼ਬਦ_list_will_be_incomplete));
		spin->si_did_emsg = TRUE;
	    }
	    ਵਾਪਸ ਨਿਰਾ;
	}
	bl->sb_ਅਗਲਾ = spin->si_ਖੰਡਾਂ;
	spin->si_ਖੰਡਾਂ = bl;
	bl->sb_ਵਰਤਿਆ = 0;
	++spin->si_ਖੰਡਾਂ_cnt;
    }

    p = bl->sb_ਅੰਕੜੇ + bl->sb_ਵਰਤਿਆ;
    bl->sb_ਵਰਤਿਆ += (ਪੂਰਨ)len;

    ਵਾਪਸ p;
}

/*
 * Make a copy of a string into memory allocated with getroom().
 * Returns NULL when out of memory.
 */
    ਸਥਿਰ char_u *
getroom_save(spellinfo_T *spin, char_u *s)
{
    char_u	*sc;

    sc = (char_u *)getroom(spin, STRLEN(s) + 1, FALSE);
    ਜੇ (sc != ਨਿਰਾ)
	STRCPY(sc, s);
    ਵਾਪਸ sc;
}


/*
 * Free the list of allocated sblock_T.
 */
    ਸਥਿਰ ਖਾਲੀ
free_ਖੰਡਾਂ(sblock_T *bl)
{
    sblock_T	*ਅਗਲਾ;

    ਜਦੋਂ (bl != ਨਿਰਾ)
    {
	ਅਗਲਾ = bl->sb_ਅਗਲਾ;
	vim_free(bl);
	bl = ਅਗਲਾ;
    }
}

/*
 * Allocate the root of a word tree.
 * Returns NULL when out of memory.
 */
    ਸਥਿਰ wordnode_T *
wordtree_alloc(spellinfo_T *spin)
{
    ਵਾਪਸ (wordnode_T *)getroom(spin, ਆਕਾਰ(wordnode_T), TRUE);
}

/*
 * Return TRUE if "word" contains valid word characters.
 * Control characters and trailing '/' are invalid.  Space is OK.
 */
    ਸਥਿਰ ਪੂਰਨ
valid_spell_ਸ਼ਬਦ(char_u *ਸ਼ਬਦ, char_u *ਸਿਰਾ)
{
    char_u *p;

    ਜੇ (enc_utf8 && !utf_valid_string(ਸ਼ਬਦ, ਸਿਰਾ))
	ਵਾਪਸ FALSE;
    ਲਈ (p = ਸ਼ਬਦ; *p != NUL && p < ਸਿਰਾ; p += mb_ptr2len(p))
	ਜੇ (*p < ' ' || (p[0] == '/' && p[1] == NUL))
	    ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
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
    ਸਥਿਰ ਪੂਰਨ
store_ਸ਼ਬਦ(
    spellinfo_T	*spin,
    char_u	*ਸ਼ਬਦ,
    ਪੂਰਨ		flags,		// extra flags, WF_BANNED
    ਪੂਰਨ		region,		// supported region(s)
    char_u	*pfxlist,	// list of prefix IDs or NULL
    ਪੂਰਨ		need_affix)	// only store word with affix ID
{
    ਪੂਰਨ		len = (ਪੂਰਨ)STRLEN(ਸ਼ਬਦ);
    ਪੂਰਨ		ct = captype(ਸ਼ਬਦ, ਸ਼ਬਦ + len);
    char_u	foldword[MAXWLEN];
    ਪੂਰਨ		res = OK;
    char_u	*p;

    // Avoid adding illegal bytes to the word tree.
    ਜੇ (!valid_spell_ਸ਼ਬਦ(ਸ਼ਬਦ, ਸ਼ਬਦ + len))
	ਵਾਪਸ FAIL;

    (ਖਾਲੀ)spell_casefold(curwin, ਸ਼ਬਦ, len, foldword, MAXWLEN);
    ਲਈ (p = pfxlist; res == OK; ++p)
    {
	ਜੇ (!need_affix || (p != ਨਿਰਾ && *p != NUL))
	    res = ਰੁੱਖ_ਜੋੜੋ_ਸ਼ਬਦ(spin, foldword, spin->si_foldroot, ct | flags,
						  region, p == ਨਿਰਾ ? 0 : *p);
	ਜੇ (p == ਨਿਰਾ || *p == NUL)
	    ਰੋਕੋ;
    }
    ++spin->si_foldwcount;

    ਜੇ (res == OK && (ct == WF_KEEPCAP || (flags & WF_KEEPCAP)))
    {
	ਲਈ (p = pfxlist; res == OK; ++p)
	{
	    ਜੇ (!need_affix || (p != ਨਿਰਾ && *p != NUL))
		res = ਰੁੱਖ_ਜੋੜੋ_ਸ਼ਬਦ(spin, ਸ਼ਬਦ, spin->si_keeproot, flags,
						  region, p == ਨਿਰਾ ? 0 : *p);
	    ਜੇ (p == ਨਿਰਾ || *p == NUL)
		ਰੋਕੋ;
	}
	++spin->si_keepwcount;
    }
    ਵਾਪਸ res;
}

/*
 * Add word "word" to a word tree at "root".
 * When "flags" < 0 we are adding to the prefix tree where "flags" is used for
 * "rare" and "region" is the condition nr.
 * Returns FAIL when out of memory.
 */
    ਸਥਿਰ ਪੂਰਨ
ਰੁੱਖ_ਜੋੜੋ_ਸ਼ਬਦ(
    spellinfo_T	*spin,
    char_u	*ਸ਼ਬਦ,
    wordnode_T	*ਜੜ੍ਹ,
    ਪੂਰਨ		flags,
    ਪੂਰਨ		region,
    ਪੂਰਨ		affixID)
{
    wordnode_T	*ਗੰਢ = ਜੜ੍ਹ;
    wordnode_T	*np;
    wordnode_T	*copyp, **copyprev;
    wordnode_T	**prev = ਨਿਰਾ;
    ਪੂਰਨ		i;

    // Add each byte of the word to the tree, including the NUL at the end.
    ਲਈ (i = 0; ; ++i)
    {
	// When there is more than one reference to this node we need to make
	// a copy, so that we can modify it.  Copy the whole list of siblings
	// (we don't optimize for a partly shared list of siblings).
	ਜੇ (ਗੰਢ != ਨਿਰਾ && ਗੰਢ->wn_refs > 1)
	{
	    --ਗੰਢ->wn_refs;
	    copyprev = prev;
	    FOR_ALL_ਗੰਢ_SIBLINGS(ਗੰਢ, copyp)
	    {
		// Allocate a new node and copy the info.
		np = get_wordnode(spin);
		ਜੇ (np == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
		np->wn_ਬੱਚਾ = copyp->wn_ਬੱਚਾ;
		ਜੇ (np->wn_ਬੱਚਾ != ਨਿਰਾ)
		    ++np->wn_ਬੱਚਾ->wn_refs;	// child gets extra ref
		np->wn_byte = copyp->wn_byte;
		ਜੇ (np->wn_byte == NUL)
		{
		    np->wn_flags = copyp->wn_flags;
		    np->wn_region = copyp->wn_region;
		    np->wn_affixID = copyp->wn_affixID;
		}

		// Link the new node in the list, there will be one ref.
		np->wn_refs = 1;
		ਜੇ (copyprev != ਨਿਰਾ)
		    *copyprev = np;
		copyprev = &np->wn_sibling;

		// Let "node" point to the head of the copied list.
		ਜੇ (copyp == ਗੰਢ)
		    ਗੰਢ = np;
	    }
	}

	// Look for the sibling that has the same character.  They are sorted
	// on byte value, thus stop searching when a sibling is found with a
	// higher byte value.  For zero bytes (end of word) the sorting is
	// done on flags and then on affixID.
	ਜਦੋਂ (ਗੰਢ != ਨਿਰਾ
		&& (ਗੰਢ->wn_byte < ਸ਼ਬਦ[i]
		    || (ਗੰਢ->wn_byte == NUL
			&& (flags < 0
			    ? ਗੰਢ->wn_affixID < (ਬਿਨਾਨਿਸ਼ਾਨੀ)affixID
			    : (ਗੰਢ->wn_flags < (ਬਿਨਾਨਿਸ਼ਾਨੀ)(flags & WN_ਨਕਾਬ)
				|| (ਗੰਢ->wn_flags == (flags & WN_ਨਕਾਬ)
				    && (spin->si_sugtree
					? (ਗੰਢ->wn_region & 0xffff) < region
					: ਗੰਢ->wn_affixID
						    < (ਬਿਨਾਨਿਸ਼ਾਨੀ)affixID)))))))
	{
	    prev = &ਗੰਢ->wn_sibling;
	    ਗੰਢ = *prev;
	}
	ਜੇ (ਗੰਢ == ਨਿਰਾ
		|| ਗੰਢ->wn_byte != ਸ਼ਬਦ[i]
		|| (ਸ਼ਬਦ[i] == NUL
		    && (flags < 0
			|| spin->si_sugtree
			|| ਗੰਢ->wn_flags != (flags & WN_ਨਕਾਬ)
			|| ਗੰਢ->wn_affixID != affixID)))
	{
	    // Allocate a new node.
	    np = get_wordnode(spin);
	    ਜੇ (np == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	    np->wn_byte = ਸ਼ਬਦ[i];

	    // If "node" is NULL this is a new child or the end of the sibling
	    // list: ref count is one.  Otherwise use ref count of sibling and
	    // make ref count of sibling one (matters when inserting in front
	    // of the list of siblings).
	    ਜੇ (ਗੰਢ == ਨਿਰਾ)
		np->wn_refs = 1;
	    ਨਹੀਂਤਾਂ
	    {
		np->wn_refs = ਗੰਢ->wn_refs;
		ਗੰਢ->wn_refs = 1;
	    }
	    ਜੇ (prev != ਨਿਰਾ)
		*prev = np;
	    np->wn_sibling = ਗੰਢ;
	    ਗੰਢ = np;
	}

	ਜੇ (ਸ਼ਬਦ[i] == NUL)
	{
	    ਗੰਢ->wn_flags = flags;
	    ਗੰਢ->wn_region |= region;
	    ਗੰਢ->wn_affixID = affixID;
	    ਰੋਕੋ;
	}
	prev = &ਗੰਢ->wn_ਬੱਚਾ;
	ਗੰਢ = *prev;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SPELL_PRINTTREE
    smsg("Added \"%s\"", ਸ਼ਬਦ);
    spell_print_ਰੁੱਖ(ਜੜ੍ਹ->wn_sibling);
#ਜੇ_ਅੰਤ

    // count nr of words added since last message
    ++spin->si_ਸੁਨੇਹਾ_ਗਿਣਤੀ;

    ਜੇ (spin->si_compress_cnt > 1)
    {
	ਜੇ (--spin->si_compress_cnt == 1)
	    // Did enough words to lower the block count limit.
	    spin->si_ਖੰਡਾਂ_cnt += compress_inc;
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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SPELL_COMPRESS_ALWAYS
    ਜੇ (spin->si_compress_cnt == 1
	    ? spin->si_free_ਗਿਣਤੀ < MAXWLEN
	    : spin->si_ਖੰਡਾਂ_cnt >= compress_ਸ਼ੁਰੂ)
#ਜੇ_ਅੰਤ
    {
	// Decrement the block counter.  The effect is that we compress again
	// when the freed up room has been used and another "compress_inc"
	// blocks have been allocated.  Unless "compress_added" words have
	// been added, then the limit is put back again.
	spin->si_ਖੰਡਾਂ_cnt -= compress_inc;
	spin->si_compress_cnt = compress_added;

	ਜੇ (spin->si_verbose)
	{
	    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
	    ਸੁਨੇਹਾ_puts(_(ਸੁਨੇਹਾ_compressing));
	    ਸੁਨੇਹਾ_clr_eos();
	    ਸੁਨੇਹਾ_didout = FALSE;
	    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
	    ਬਾਹਰਲਾ_flush();
	}

	// Compress both trees.  Either they both have many nodes, which makes
	// compression useful, or one of them is small, which means
	// compression goes fast.  But when filling the soundfold word tree
	// there is no keep-case tree.
	wordtree_compress(spin, spin->si_foldroot, "case-folded");
	ਜੇ (affixID >= 0)
	    wordtree_compress(spin, spin->si_keeproot, "keep-case");
    }

    ਵਾਪਸ OK;
}

/*
 * Get a wordnode_T, either from the list of previously freed nodes or
 * allocate a new one.
 * Returns NULL when out of memory.
 */
    ਸਥਿਰ wordnode_T *
get_wordnode(spellinfo_T *spin)
{
    wordnode_T *n;

    ਜੇ (spin->si_ਪਹਿਲਾ_free == ਨਿਰਾ)
	n = (wordnode_T *)getroom(spin, ਆਕਾਰ(wordnode_T), TRUE);
    ਨਹੀਂਤਾਂ
    {
	n = spin->si_ਪਹਿਲਾ_free;
	spin->si_ਪਹਿਲਾ_free = n->wn_ਬੱਚਾ;
	CLEAR_ਸੂਚਕ(n);
	--spin->si_free_ਗਿਣਤੀ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SPELL_PRINTTREE
    ਜੇ (n != ਨਿਰਾ)
	n->wn_nr = ++spin->si_wordnode_nr;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ n;
}

/*
 * Decrement the reference count on a node (which is the head of a list of
 * siblings).  If the reference count becomes zero free the node and its
 * siblings.
 * Returns the number of nodes actually freed.
 */
    ਸਥਿਰ ਪੂਰਨ
deref_wordnode(spellinfo_T *spin, wordnode_T *ਗੰਢ)
{
    wordnode_T	*np;
    ਪੂਰਨ		cnt = 0;

    ਜੇ (--ਗੰਢ->wn_refs == 0)
    {
	FOR_ALL_ਗੰਢ_SIBLINGS(ਗੰਢ, np)
	{
	    ਜੇ (np->wn_ਬੱਚਾ != ਨਿਰਾ)
		cnt += deref_wordnode(spin, np->wn_ਬੱਚਾ);
	    free_wordnode(spin, np);
	    ++cnt;
	}
	++cnt;	    // length field
    }
    ਵਾਪਸ cnt;
}

/*
 * Free a wordnode_T for re-use later.
 * Only the "wn_child" field becomes invalid.
 */
    ਸਥਿਰ ਖਾਲੀ
free_wordnode(spellinfo_T *spin, wordnode_T *n)
{
    n->wn_ਬੱਚਾ = spin->si_ਪਹਿਲਾ_free;
    spin->si_ਪਹਿਲਾ_free = n;
    ++spin->si_free_ਗਿਣਤੀ;
}

/*
 * Compress a tree: find tails that are identical and can be shared.
 */
    ਸਥਿਰ ਖਾਲੀ
wordtree_compress(spellinfo_T *spin, wordnode_T *ਜੜ੍ਹ, ਅੱਖਰ *name)
{
    hashtab_T	    ht;
    ਲੰਮਾ	    n;
    ਲੰਮਾ	    tot = 0;
    ਲੰਮਾ	    perc;

    // Skip the root itself, it's not actually used.  The first sibling is the
    // start of the tree.
    ਜੇ (ਜੜ੍ਹ->wn_sibling == ਨਿਰਾ)
	ਵਾਪਸ;

    hash_init(&ht);
    n = ਗੰਢ_compress(spin, ਜੜ੍ਹ->wn_sibling, &ht, &tot);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SPELL_PRINTTREE
    ਜੇ (spin->si_verbose || p_verbose > 2)
#ਜੇ_ਅੰਤ
    {
	ਜੇ (tot > 1000000)
	    perc = (tot - n) / (tot / 100);
	ਨਹੀਂਤਾਂ ਜੇ (tot == 0)
	    perc = 0;
	ਨਹੀਂਤਾਂ
	    perc = (tot - n) * 100 / tot;
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
		_("Compressed %s: %ld of %ld nodes; %ld (%ld%%) remaining"),
		name, n, tot, tot - n, perc);
	spell_ਸੁਨੇਹਾ(spin, IObuff);
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SPELL_PRINTTREE
    spell_print_ਰੁੱਖ(ਜੜ੍ਹ->wn_sibling);
#ਜੇ_ਅੰਤ
    hash_clear(&ht);
}

/*
 * Compress a node, its siblings and its children, depth first.
 * Returns the number of compressed nodes.
 */
    ਸਥਿਰ ਲੰਮਾ
ਗੰਢ_compress(
    spellinfo_T	*spin,
    wordnode_T	*ਗੰਢ,
    hashtab_T	*ht,
    ਲੰਮਾ	*tot)	    // total count of nodes before compressing,
			    // incremented while going through the tree
{
    wordnode_T	*np;
    wordnode_T	*tp;
    wordnode_T	*ਬੱਚਾ;
    hash_T	hash;
    hashitem_T	*hi;
    ਲੰਮਾ	len = 0;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	nr, n;
    ਲੰਮਾ	compressed = 0;

    /*
     * Go through the list of siblings.  Compress each child and then try
     * finding an identical child to replace it.
     * Note that with "child" we mean not just the node that is pointed to,
     * but the whole list of siblings of which the child node is the first.
     */
    ਲਈ (np = ਗੰਢ; np != ਨਿਰਾ && !got_int; np = np->wn_sibling)
    {
	++len;
	ਜੇ ((ਬੱਚਾ = np->wn_ਬੱਚਾ) != ਨਿਰਾ)
	{
	    // Compress the child first.  This fills hashkey.
	    compressed += ਗੰਢ_compress(spin, ਬੱਚਾ, ht, tot);

	    // Try to find an identical child.
	    hash = hash_hash(ਬੱਚਾ->wn_u1.hashkey);
	    hi = hash_lookup(ht, ਬੱਚਾ->wn_u1.hashkey, hash);
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    {
		// There are children we encountered before with a hash value
		// identical to the current child.  Now check if there is one
		// that is really identical.
		ਲਈ (tp = HI2WN(hi); tp != ਨਿਰਾ; tp = tp->wn_u2.ਅਗਲਾ)
		    ਜੇ (ਗੰਢ_equal(ਬੱਚਾ, tp))
		    {
			// Found one!  Now use that child in place of the
			// current one.  This means the current child and all
			// its siblings is unlinked from the tree.
			++tp->wn_refs;
			compressed += deref_wordnode(spin, ਬੱਚਾ);
			np->wn_ਬੱਚਾ = tp;
			ਰੋਕੋ;
		    }
		ਜੇ (tp == ਨਿਰਾ)
		{
		    // No other child with this hash value equals the child of
		    // the node, add it to the linked list after the first
		    // item.
		    tp = HI2WN(hi);
		    ਬੱਚਾ->wn_u2.ਅਗਲਾ = tp->wn_u2.ਅਗਲਾ;
		    tp->wn_u2.ਅਗਲਾ = ਬੱਚਾ;
		}
	    }
	    ਨਹੀਂਤਾਂ
		// No other child has this hash value, add it to the
		// hashtable.
		hash_ਜੋੜੋ_ਵਸਤੂ(ht, hi, ਬੱਚਾ->wn_u1.hashkey, hash);
	}
    }
    *tot += len + 1;	// add one for the node that stores the length

    /*
     * Make a hash key for the node and its siblings, so that we can quickly
     * find a lookalike node.  This must be done after compressing the sibling
     * list, otherwise the hash key would become invalid by the compression.
     */
    ਗੰਢ->wn_u1.hashkey[0] = len;
    nr = 0;
    FOR_ALL_ਗੰਢ_SIBLINGS(ਗੰਢ, np)
    {
	ਜੇ (np->wn_byte == NUL)
	    // end node: use wn_flags, wn_region and wn_affixID
	    n = np->wn_flags + (np->wn_region << 8) + (np->wn_affixID << 16);
	ਨਹੀਂਤਾਂ
	    // byte node: use the byte value and the child pointer
	    n = (ਬਿਨਾਨਿਸ਼ਾਨੀ)(np->wn_byte + ((long_u)np->wn_ਬੱਚਾ << 8));
	nr = nr * 101 + n;
    }

    // Avoid NUL bytes, it terminates the hash key.
    n = nr & 0xff;
    ਗੰਢ->wn_u1.hashkey[1] = n == 0 ? 1 : n;
    n = (nr >> 8) & 0xff;
    ਗੰਢ->wn_u1.hashkey[2] = n == 0 ? 1 : n;
    n = (nr >> 16) & 0xff;
    ਗੰਢ->wn_u1.hashkey[3] = n == 0 ? 1 : n;
    n = (nr >> 24) & 0xff;
    ਗੰਢ->wn_u1.hashkey[4] = n == 0 ? 1 : n;
    ਗੰਢ->wn_u1.hashkey[5] = NUL;

    // Check for CTRL-C pressed now and then.
    veryfast_breakcheck();

    ਵਾਪਸ compressed;
}

/*
 * Return TRUE when two nodes have identical siblings and children.
 */
    ਸਥਿਰ ਪੂਰਨ
ਗੰਢ_equal(wordnode_T *n1, wordnode_T *n2)
{
    wordnode_T	*p1;
    wordnode_T	*p2;

    ਲਈ (p1 = n1, p2 = n2; p1 != ਨਿਰਾ && p2 != ਨਿਰਾ;
				     p1 = p1->wn_sibling, p2 = p2->wn_sibling)
	ਜੇ (p1->wn_byte != p2->wn_byte
		|| (p1->wn_byte == NUL
		    ? (p1->wn_flags != p2->wn_flags
			|| p1->wn_region != p2->wn_region
			|| p1->wn_affixID != p2->wn_affixID)
		    : (p1->wn_ਬੱਚਾ != p2->wn_ਬੱਚਾ)))
	    ਰੋਕੋ;

    ਵਾਪਸ p1 == ਨਿਰਾ && p2 == ਨਿਰਾ;
}

ਸਥਿਰ ਪੂਰਨ rep_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2);

/*
 * Function given to qsort() to sort the REP items on "from" string.
 */
    ਸਥਿਰ ਪੂਰਨ
rep_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    fromto_T	*p1 = (fromto_T *)s1;
    fromto_T	*p2 = (fromto_T *)s2;

    ਵਾਪਸ STRCMP(p1->ft_from, p2->ft_from);
}

/*
 * Write the Vim .spl file "fname".
 * Return FAIL or OK;
 */
    ਸਥਿਰ ਪੂਰਨ
write_vim_spell(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    ਪੂਰਨ		regionmask;
    ਪੂਰਨ		ਗੇੜਾ;
    wordnode_T	*ਰੁੱਖ;
    ਪੂਰਨ		nodecount;
    ਪੂਰਨ		i;
    ਪੂਰਨ		l;
    garray_T	*gap;
    fromto_T	*ftp;
    char_u	*p;
    ਪੂਰਨ		rr;
    ਪੂਰਨ		retval = OK;
    size_t	fwv = 1;  // collect return value of fwrite() to avoid
			  // warnings from picky compiler

    fd = mch_fopen((ਅੱਖਰ *)fname, "w");
    ਜੇ (fd == ਨਿਰਾ)
    {
	semsg(_(e_cant_open_file_str), fname);
	ਵਾਪਸ FAIL;
    }

    // <HEADER>: <fileID> <versionnr>
							    // <fileID>
    fwv &= fwrite(VIMSPELLMAGIC, VIMSPELLMAGICL, (size_t)1, fd);
    ਜੇ (fwv != (size_t)1)
	// Catch first write error, don't try writing more.
	ਜਾਓ theend;

    putc(VIMSPELLVERSION, fd);				    // <versionnr>

    /*
     * <SECTIONS>: <section> ... <sectionend>
     */

    // SN_INFO: <infotext>
    ਜੇ (spin->si_info != ਨਿਰਾ)
    {
	putc(SN_INFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	i = (ਪੂਰਨ)STRLEN(spin->si_info);
	put_ਬਾਈਟਾਂ(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_info, (size_t)i, (size_t)1, fd); // <infotext>
    }

    // SN_REGION: <regionname> ...
    // Write the region names only if there is more than one.
    ਜੇ (spin->si_region_ਗਿਣਤੀ > 1)
    {
	putc(SN_REGION, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>
	l = spin->si_region_ਗਿਣਤੀ * 2;
	put_ਬਾਈਟਾਂ(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_region_name, (size_t)l, (size_t)1, fd);
							// <regionname> ...
	regionmask = (1 << spin->si_region_ਗਿਣਤੀ) - 1;
    }
    ਨਹੀਂਤਾਂ
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
    ਜੇ (!spin->si_ascii && !spin->si_ਜੋੜੋ)
    {
	char_u	folchars[128 * 8];
	ਪੂਰਨ	flags;

	putc(SN_CHARFLAGS, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	// Form the <folchars> string first, we need to know its length.
	l = 0;
	ਲਈ (i = 128; i < 256; ++i)
	{
	    ਜੇ (has_mbyte)
		l += mb_char2bytes(spelltab.st_fold[i], folchars + l);
	    ਨਹੀਂਤਾਂ
		folchars[l++] = spelltab.st_fold[i];
	}
	put_ਬਾਈਟਾਂ(fd, (long_u)(1 + 128 + 2 + l), 4);	// <sectionlen>

	fputc(128, fd);					// <charflagslen>
	ਲਈ (i = 128; i < 256; ++i)
	{
	    flags = 0;
	    ਜੇ (spelltab.st_isw[i])
		flags |= CF_ਸ਼ਬਦ;
	    ਜੇ (spelltab.st_isu[i])
		flags |= CF_ਉੱਤਲਾ;
	    fputc(flags, fd);				// <charflags>
	}

	put_ਬਾਈਟਾਂ(fd, (long_u)l, 2);			// <folcharslen>
	fwv &= fwrite(folchars, (size_t)l, (size_t)1, fd); // <folchars>
    }

    // SN_MIDWORD: <midword>
    ਜੇ (spin->si_midword != ਨਿਰਾ)
    {
	putc(SN_MIDWORD, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	i = (ਪੂਰਨ)STRLEN(spin->si_midword);
	put_ਬਾਈਟਾਂ(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_midword, (size_t)i, (size_t)1, fd);
							// <midword>
    }

    // SN_PREFCOND: <prefcondcnt> <prefcond> ...
    ਜੇ (spin->si_prefcond.ga_len > 0)
    {
	putc(SN_PREFCOND, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	l = write_spell_prefcond(ਨਿਰਾ, &spin->si_prefcond, &fwv);
	put_ਬਾਈਟਾਂ(fd, (long_u)l, 4);			// <sectionlen>

	write_spell_prefcond(fd, &spin->si_prefcond, &fwv);
    }

    // SN_REP: <repcount> <rep> ...
    // SN_SAL: <salflags> <salcount> <sal> ...
    // SN_REPSAL: <repcount> <rep> ...

    // round 1: SN_REP section
    // round 2: SN_SAL section (unless SN_SOFO is used)
    // round 3: SN_REPSAL section
    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 3; ++ਗੇੜਾ)
    {
	ਜੇ (ਗੇੜਾ == 1)
	    gap = &spin->si_rep;
	ਨਹੀਂਤਾਂ ਜੇ (ਗੇੜਾ == 2)
	{
	    // Don't write SN_SAL when using a SN_SOFO section
	    ਜੇ (spin->si_sofofr != ਨਿਰਾ && spin->si_sofoto != ਨਿਰਾ)
		ਜਾਰੀ;
	    gap = &spin->si_sal;
	}
	ਨਹੀਂਤਾਂ
	    gap = &spin->si_repsal;

	// Don't write the section if there are no items.
	ਜੇ (gap->ga_len == 0)
	    ਜਾਰੀ;

	// Sort the REP/REPSAL items.
	ਜੇ (ਗੇੜਾ != 2)
	    qsort(gap->ga_ਅੰਕੜੇ, (size_t)gap->ga_len,
					       ਆਕਾਰ(fromto_T), rep_compare);

	i = ਗੇੜਾ == 1 ? SN_REP : (ਗੇੜਾ == 2 ? SN_SAL : SN_REPSAL);
	putc(i, fd);					// <sectionID>

	// This is for making suggestions, section is not required.
	putc(0, fd);					// <sectionflags>

	// Compute the length of what follows.
	l = 2;	    // count <repcount> or <salcount>
	ਲਈ (i = 0; i < gap->ga_len; ++i)
	{
	    ftp = &((fromto_T *)gap->ga_ਅੰਕੜੇ)[i];
	    l += 1 + (ਪੂਰਨ)STRLEN(ftp->ft_from);  // count <*fromlen> and <*from>
	    l += 1 + (ਪੂਰਨ)STRLEN(ftp->ft_ਮੰਜ਼ਿਲ);    // count <*tolen> and <*to>
	}
	ਜੇ (ਗੇੜਾ == 2)
	    ++l;	// count <salflags>
	put_ਬਾਈਟਾਂ(fd, (long_u)l, 4);			// <sectionlen>

	ਜੇ (ਗੇੜਾ == 2)
	{
	    i = 0;
	    ਜੇ (spin->si_followup)
		i |= SAL_F0LLOWUP;
	    ਜੇ (spin->si_collapse)
		i |= SAL_COLLAPSE;
	    ਜੇ (spin->si_rem_accents)
		i |= SAL_REM_ACCENTS;
	    putc(i, fd);			// <salflags>
	}

	put_ਬਾਈਟਾਂ(fd, (long_u)gap->ga_len, 2);	// <repcount> or <salcount>
	ਲਈ (i = 0; i < gap->ga_len; ++i)
	{
	    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
	    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
	    ftp = &((fromto_T *)gap->ga_ਅੰਕੜੇ)[i];
	    ਲਈ (rr = 1; rr <= 2; ++rr)
	    {
		p = rr == 1 ? ftp->ft_from : ftp->ft_ਮੰਜ਼ਿਲ;
		l = (ਪੂਰਨ)STRLEN(p);
		putc(l, fd);
		ਜੇ (l > 0)
		    fwv &= fwrite(p, l, (size_t)1, fd);
	    }
	}

    }

    // SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
    // This is for making suggestions, section is not required.
    ਜੇ (spin->si_sofofr != ਨਿਰਾ && spin->si_sofoto != ਨਿਰਾ)
    {
	putc(SN_SOFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (ਪੂਰਨ)STRLEN(spin->si_sofofr);
	put_ਬਾਈਟਾਂ(fd, (long_u)(l + STRLEN(spin->si_sofoto) + 4), 4);
							// <sectionlen>

	put_ਬਾਈਟਾਂ(fd, (long_u)l, 2);			// <sofofromlen>
	fwv &= fwrite(spin->si_sofofr, l, (size_t)1, fd); // <sofofrom>

	l = (ਪੂਰਨ)STRLEN(spin->si_sofoto);
	put_ਬਾਈਟਾਂ(fd, (long_u)l, 2);			// <sofotolen>
	fwv &= fwrite(spin->si_sofoto, l, (size_t)1, fd); // <sofoto>
    }

    // SN_WORDS: <word> ...
    // This is for making suggestions, section is not required.
    ਜੇ (spin->si_commonwords.ht_ਵਰਤਿਆ > 0)
    {
	putc(SN_ਸ਼ਬਦ, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// round 1: count the bytes
	// round 2: write the bytes
	ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
	{
	    ਪੂਰਨ		todo;
	    ਪੂਰਨ		len = 0;
	    hashitem_T	*hi;

	    todo = (ਪੂਰਨ)spin->si_commonwords.ht_ਵਰਤਿਆ;
	    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&spin->si_commonwords, hi, todo)
		ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		{
		    l = (ਪੂਰਨ)STRLEN(hi->hi_ਕੁੰਜੀ) + 1;
		    len += l;
		    ਜੇ (ਗੇੜਾ == 2)			// <word>
			fwv &= fwrite(hi->hi_ਕੁੰਜੀ, (size_t)l, (size_t)1, fd);
		    --todo;
		}
	    ਜੇ (ਗੇੜਾ == 1)
		put_ਬਾਈਟਾਂ(fd, (long_u)len, 4);		// <sectionlen>
	}
    }

    // SN_MAP: <mapstr>
    // This is for making suggestions, section is not required.
    ਜੇ (spin->si_map.ga_len > 0)
    {
	putc(SN_MAP, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>
	l = spin->si_map.ga_len;
	put_ਬਾਈਟਾਂ(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_map.ga_ਅੰਕੜੇ, (size_t)l, (size_t)1, fd);
							// <mapstr>
    }

    // SN_SUGFILE: <timestamp>
    // This is used to notify that a .sug file may be available and at the
    // same time allows for checking that a .sug file that is found matches
    // with this .spl file.  That's because the word numbers must be exactly
    // right.
    ਜੇ (!spin->si_nosugfile
	    && (spin->si_sal.ga_len > 0
		     || (spin->si_sofofr != ਨਿਰਾ && spin->si_sofoto != ਨਿਰਾ)))
    {
	putc(SN_SUGFILE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_ਬਾਈਟਾਂ(fd, (long_u)8, 4);			// <sectionlen>

	// Set si_sugtime and write it to the file.
	spin->si_sugtime = ਸਮਾਂ(ਨਿਰਾ);
	put_ਸਮਾਂ(fd, spin->si_sugtime);			// <timestamp>
    }

    // SN_NOSPLITSUGS: nothing
    // This is used to notify that no suggestions with word splits are to be
    // made.
    ਜੇ (spin->si_nosplitsugs)
    {
	putc(SN_NOSPLITSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_ਬਾਈਟਾਂ(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_NOCOMPUNDSUGS: nothing
    // This is used to notify that no suggestions with compounds are to be
    // made.
    ਜੇ (spin->si_nocompoundsugs)
    {
	putc(SN_NOCOMPOUNDSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_ਬਾਈਟਾਂ(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_COMPOUND: compound info.
    // We don't mark it required, when not supported all compound words will
    // be bad words.
    ਜੇ (spin->si_compflags != ਨਿਰਾ)
    {
	putc(SN_COMPOUND, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (ਪੂਰਨ)STRLEN(spin->si_compflags);
	ਲਈ (i = 0; i < spin->si_comppat.ga_len; ++i)
	    l += (ਪੂਰਨ)STRLEN(((char_u **)(spin->si_comppat.ga_ਅੰਕੜੇ))[i]) + 1;
	put_ਬਾਈਟਾਂ(fd, (long_u)(l + 7), 4);		// <sectionlen>

	putc(spin->si_compmax, fd);			// <compmax>
	putc(spin->si_compminlen, fd);			// <compminlen>
	putc(spin->si_compsylmax, fd);			// <compsylmax>
	putc(0, fd);		// for Vim 7.0b compatibility
	putc(spin->si_compoptions, fd);			// <compoptions>
	put_ਬਾਈਟਾਂ(fd, (long_u)spin->si_comppat.ga_len, 2);
							// <comppatcount>
	ਲਈ (i = 0; i < spin->si_comppat.ga_len; ++i)
	{
	    p = ((char_u **)(spin->si_comppat.ga_ਅੰਕੜੇ))[i];
	    putc((ਪੂਰਨ)STRLEN(p), fd);			// <comppatlen>
	    fwv &= fwrite(p, (size_t)STRLEN(p), (size_t)1, fd);
							// <comppattext>
	}
							// <compflags>
	fwv &= fwrite(spin->si_compflags, (size_t)STRLEN(spin->si_compflags),
							       (size_t)1, fd);
    }

    // SN_NOBREAK: NOBREAK flag
    ਜੇ (spin->si_nobreak)
    {
	putc(SN_NOBREAK, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// It's empty, the presence of the section flags the feature.
	put_ਬਾਈਟਾਂ(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_SYLLABLE: syllable info.
    // We don't mark it required, when not supported syllables will not be
    // counted.
    ਜੇ (spin->si_syllable != ਨਿਰਾ)
    {
	putc(SN_SYLLABLE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (ਪੂਰਨ)STRLEN(spin->si_syllable);
	put_ਬਾਈਟਾਂ(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_syllable, (size_t)l, (size_t)1, fd);
							// <syllable>
    }

    // end of <SECTIONS>
    putc(SN_ਸਿਰਾ, fd);					// <sectionend>


    /*
     * <LWORDTREE>  <KWORDTREE>  <PREFIXTREE>
     */
    spin->si_memtot = 0;
    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 3; ++ਗੇੜਾ)
    {
	ਜੇ (ਗੇੜਾ == 1)
	    ਰੁੱਖ = spin->si_foldroot->wn_sibling;
	ਨਹੀਂਤਾਂ ਜੇ (ਗੇੜਾ == 2)
	    ਰੁੱਖ = spin->si_keeproot->wn_sibling;
	ਨਹੀਂਤਾਂ
	    ਰੁੱਖ = spin->si_prefroot->wn_sibling;

	// Clear the index and wnode fields in the tree.
	clear_ਗੰਢ(ਰੁੱਖ);

	// Count the number of nodes.  Needed to be able to allocate the
	// memory when reading the nodes.  Also fills in index for shared
	// nodes.
	nodecount = put_ਗੰਢ(ਨਿਰਾ, ਰੁੱਖ, 0, regionmask, ਗੇੜਾ == 3);

	// number of nodes in 4 bytes
	put_ਬਾਈਟਾਂ(fd, (long_u)nodecount, 4);	// <nodecount>
	spin->si_memtot += nodecount + nodecount * ਆਕਾਰ(ਪੂਰਨ);

	// Write the nodes.
	(ਖਾਲੀ)put_ਗੰਢ(fd, ਰੁੱਖ, 0, regionmask, ਗੇੜਾ == 3);
    }

    // Write another byte to check for errors (file system full).
    ਜੇ (putc(0, fd) == EOF)
	retval = FAIL;
theend:
    ਜੇ (fclose(fd) == EOF)
	retval = FAIL;

    ਜੇ (fwv != (size_t)1)
	retval = FAIL;
    ਜੇ (retval == FAIL)
	emsg(_(e_error_while_writing));

    ਵਾਪਸ retval;
}

/*
 * Clear the index and wnode fields of "node", it siblings and its
 * children.  This is needed because they are a union with other items to save
 * space.
 */
    ਸਥਿਰ ਖਾਲੀ
clear_ਗੰਢ(wordnode_T *ਗੰਢ)
{
    wordnode_T	*np;

    ਜੇ (ਗੰਢ != ਨਿਰਾ)
	FOR_ALL_ਗੰਢ_SIBLINGS(ਗੰਢ, np)
	{
	    np->wn_u1.ਸੂਚਕਾਂਕ = 0;
	    np->wn_u2.wnode = ਨਿਰਾ;

	    ਜੇ (np->wn_byte != NUL)
		clear_ਗੰਢ(np->wn_ਬੱਚਾ);
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
    ਸਥਿਰ ਪੂਰਨ
put_ਗੰਢ(
    FILE	*fd,		// NULL when only counting
    wordnode_T	*ਗੰਢ,
    ਪੂਰਨ		idx,
    ਪੂਰਨ		regionmask,
    ਪੂਰਨ		prefixtree)	// TRUE for PREFIXTREE
{
    ਪੂਰਨ		newindex = idx;
    ਪੂਰਨ		siblingcount = 0;
    wordnode_T	*np;
    ਪੂਰਨ		flags;

    // If "node" is zero the tree is empty.
    ਜੇ (ਗੰਢ == ਨਿਰਾ)
	ਵਾਪਸ 0;

    // Store the index where this node is written.
    ਗੰਢ->wn_u1.ਸੂਚਕਾਂਕ = idx;

    // Count the number of siblings.
    FOR_ALL_ਗੰਢ_SIBLINGS(ਗੰਢ, np)
	++siblingcount;

    // Write the sibling count.
    ਜੇ (fd != ਨਿਰਾ)
	putc(siblingcount, fd);				// <siblingcount>

    // Write each sibling byte and optionally extra info.
    FOR_ALL_ਗੰਢ_SIBLINGS(ਗੰਢ, np)
    {
	ਜੇ (np->wn_byte == 0)
	{
	    ਜੇ (fd != ਨਿਰਾ)
	    {
		// For a NUL byte (end of word) write the flags etc.
		ਜੇ (prefixtree)
		{
		    // In PREFIXTREE write the required affixID and the
		    // associated condition nr (stored in wn_region).  The
		    // byte value is misused to store the "rare" and "not
		    // combining" flags
		    ਜੇ (np->wn_flags == (short_u)PFX_FLAGS)
			putc(BY_NOFLAGS, fd);		// <byte>
		    ਨਹੀਂਤਾਂ
		    {
			putc(BY_FLAGS, fd);		// <byte>
			putc(np->wn_flags, fd);		// <pflags>
		    }
		    putc(np->wn_affixID, fd);		// <affixID>
		    put_ਬਾਈਟਾਂ(fd, (long_u)np->wn_region, 2); // <prefcondnr>
		}
		ਨਹੀਂਤਾਂ
		{
		    // For word trees we write the flag/region items.
		    flags = np->wn_flags;
		    ਜੇ (regionmask != 0 && np->wn_region != regionmask)
			flags |= WF_REGION;
		    ਜੇ (np->wn_affixID != 0)
			flags |= WF_AFX;
		    ਜੇ (flags == 0)
		    {
			// word without flags or region
			putc(BY_NOFLAGS, fd);			// <byte>
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (np->wn_flags >= 0x100)
			{
			    putc(BY_FLAGS2, fd);		// <byte>
			    putc(flags, fd);			// <flags>
			    putc((ਬਿਨਾਨਿਸ਼ਾਨੀ)flags >> 8, fd);	// <flags2>
			}
			ਨਹੀਂਤਾਂ
			{
			    putc(BY_FLAGS, fd);			// <byte>
			    putc(flags, fd);			// <flags>
			}
			ਜੇ (flags & WF_REGION)
			    putc(np->wn_region, fd);		// <region>
			ਜੇ (flags & WF_AFX)
			    putc(np->wn_affixID, fd);		// <affixID>
		    }
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (np->wn_ਬੱਚਾ->wn_u1.ਸੂਚਕਾਂਕ != 0
					 && np->wn_ਬੱਚਾ->wn_u2.wnode != ਗੰਢ)
	    {
		// The child is written elsewhere, write the reference.
		ਜੇ (fd != ਨਿਰਾ)
		{
		    putc(BY_ਸੂਚਕਾਂਕ, fd);			// <byte>
							// <nodeidx>
		    put_ਬਾਈਟਾਂ(fd, (long_u)np->wn_ਬੱਚਾ->wn_u1.ਸੂਚਕਾਂਕ, 3);
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (np->wn_ਬੱਚਾ->wn_u2.wnode == ਨਿਰਾ)
		// We will write the child below and give it an index.
		np->wn_ਬੱਚਾ->wn_u2.wnode = ਗੰਢ;

	    ਜੇ (fd != ਨਿਰਾ)
		ਜੇ (putc(np->wn_byte, fd) == EOF) // <byte> or <xbyte>
		{
		    emsg(_(e_error_while_writing));
		    ਵਾਪਸ 0;
		}
	}
    }

    // Space used in the array when reading: one for each sibling and one for
    // the count.
    newindex += siblingcount + 1;

    // Recursively dump the children of each sibling.
    FOR_ALL_ਗੰਢ_SIBLINGS(ਗੰਢ, np)
	ਜੇ (np->wn_byte != 0 && np->wn_ਬੱਚਾ->wn_u2.wnode == ਗੰਢ)
	    newindex = put_ਗੰਢ(fd, np->wn_ਬੱਚਾ, newindex, regionmask,
								  prefixtree);

    ਵਾਪਸ newindex;
}


/*
 * ":mkspell [-ascii] outfile  infile ..."
 * ":mkspell [-ascii] addfile"
 */
    ਖਾਲੀ
ex_mkspell(exarg_T *eap)
{
    ਪੂਰਨ		fcount;
    char_u	**fnames;
    char_u	*arg = eap->arg;
    ਪੂਰਨ		ascii = FALSE;

    ਜੇ (STRNCMP(arg, "-ascii", 6) == 0)
    {
	ascii = TRUE;
	arg = skipwhite(arg + 6);
    }

    // Expand all the remaining arguments (e.g., $VIMRUNTIME).
    ਜੇ (get_arglist_exp(arg, &fcount, &fnames, FALSE) != OK)
	ਵਾਪਸ;

    mkspell(fcount, fnames, ascii, eap->forceit, FALSE);
    FreeWild(fcount, fnames);
}

/*
 * Create the .sug file.
 * Uses the soundfold info in "spin".
 * Writes the file with the name "wfname", with ".spl" changed to ".sug".
 */
    ਸਥਿਰ ਖਾਲੀ
spell_make_sugfile(spellinfo_T *spin, char_u *wfname)
{
    char_u	*fname = ਨਿਰਾ;
    ਪੂਰਨ		len;
    slang_T	*slang;
    ਪੂਰਨ		free_slang = FALSE;

    /*
     * Read back the .spl file that was written.  This fills the required
     * info for soundfolding.  This also uses less memory than the
     * pointer-linked version of the trie.  And it avoids having two versions
     * of the code for the soundfolding stuff.
     * It might have been done already by spell_reload_one().
     */
    FOR_ALL_SPELL_LANGS(slang)
	ਜੇ (fullpathcmp(wfname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	    ਰੋਕੋ;
    ਜੇ (slang == ਨਿਰਾ)
    {
	spell_ਸੁਨੇਹਾ(spin, (char_u *)_("Reading back spell file..."));
	slang = spell_ਲੱਦੋ_file(wfname, ਨਿਰਾ, ਨਿਰਾ, FALSE);
	ਜੇ (slang == ਨਿਰਾ)
	    ਵਾਪਸ;
	free_slang = TRUE;
    }

    /*
     * Clear the info in "spin" that is used.
     */
    spin->si_ਖੰਡਾਂ = ਨਿਰਾ;
    spin->si_ਖੰਡਾਂ_cnt = 0;
    spin->si_compress_cnt = 0;	    // will stay at 0 all the time
    spin->si_free_ਗਿਣਤੀ = 0;
    spin->si_ਪਹਿਲਾ_free = ਨਿਰਾ;
    spin->si_foldwcount = 0;

    /*
     * Go through the trie of good words, soundfold each word and add it to
     * the soundfold trie.
     */
    spell_ਸੁਨੇਹਾ(spin, (char_u *)_("Performing soundfolding..."));
    ਜੇ (sug_filltree(spin, slang) == FAIL)
	ਜਾਓ theend;

    /*
     * Create the table which links each soundfold word with a list of the
     * good words it may come from.  Creates buffer "spin->si_spellbuf".
     * This also removes the wordnr from the NUL byte entries to make
     * compression possible.
     */
    ਜੇ (sug_maketable(spin) == FAIL)
	ਜਾਓ theend;

    smsg(_("Number of words after soundfolding: %ld"),
				 (ਲੰਮਾ)spin->si_spellbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);

    /*
     * Compress the soundfold trie.
     */
    spell_ਸੁਨੇਹਾ(spin, (char_u *)_(ਸੁਨੇਹਾ_compressing));
    wordtree_compress(spin, spin->si_foldroot, "case-folded");

    /*
     * Write the .sug file.
     * Make the file name by changing ".spl" to ".sug".
     */
    fname = alloc(MAXPATHL);
    ਜੇ (fname == ਨਿਰਾ)
	ਜਾਓ theend;
    vim_strncpy(fname, wfname, MAXPATHL - 1);
    len = (ਪੂਰਨ)STRLEN(fname);
    fname[len - 2] = 'u';
    fname[len - 1] = 'g';
    sug_write(spin, fname);

theend:
    vim_free(fname);
    ਜੇ (free_slang)
	slang_free(slang);
    free_ਖੰਡਾਂ(spin->si_ਖੰਡਾਂ);
    close_spellbuf(spin->si_spellbuf);
}

/*
 * Build the soundfold trie for language "slang".
 */
    ਸਥਿਰ ਪੂਰਨ
sug_filltree(spellinfo_T *spin, slang_T *slang)
{
    char_u	*byts;
    idx_T	*idxs;
    ਪੂਰਨ		ਡੂੰਘਾਈ;
    idx_T	arridx[MAXWLEN];
    ਪੂਰਨ		curi[MAXWLEN];
    char_u	tword[MAXWLEN];
    char_u	tsalword[MAXWLEN];
    ਪੂਰਨ		c;
    idx_T	n;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	ਸ਼ਬਦ_ਪੂਰਾ = 0;
    ਪੂਰਨ		wordcount[MAXWLEN];

    // We use si_foldroot for the soundfolded trie.
    spin->si_foldroot = wordtree_alloc(spin);
    ਜੇ (spin->si_foldroot == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // let tree_add_word() know we're adding to the soundfolded tree
    spin->si_sugtree = TRUE;

    /*
     * Go through the whole case-folded tree, soundfold each word and put it
     * in the trie.  Bail out if the tree is empty.
     */
    byts = slang->sl_fbyts;
    idxs = slang->sl_fidxs;
    ਜੇ (byts == ਨਿਰਾ || idxs == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;

    ਡੂੰਘਾਈ = 0;
    ਜਦੋਂ (ਡੂੰਘਾਈ >= 0 && !got_int)
    {
	ਜੇ (curi[ਡੂੰਘਾਈ] > byts[arridx[ਡੂੰਘਾਈ]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[ਡੂੰਘਾਈ]] = wordcount[ਡੂੰਘਾਈ];
	    ਜੇ (ਡੂੰਘਾਈ > 0)
		wordcount[ਡੂੰਘਾਈ - 1] += wordcount[ਡੂੰਘਾਈ];

	    --ਡੂੰਘਾਈ;
	    ਸਤਰ_breakcheck();
	}
	ਨਹੀਂਤਾਂ
	{

	    // Do one more byte at this node.
	    n = arridx[ਡੂੰਘਾਈ] + curi[ਡੂੰਘਾਈ];
	    ++curi[ਡੂੰਘਾਈ];

	    c = byts[n];
	    ਜੇ (c == 0)
	    {
		// Sound-fold the word.
		tword[ਡੂੰਘਾਈ] = NUL;
		spell_soundfold(slang, tword, TRUE, tsalword);

		// We use the "flags" field for the MSB of the wordnr,
		// "region" for the LSB of the wordnr.
		ਜੇ (ਰੁੱਖ_ਜੋੜੋ_ਸ਼ਬਦ(spin, tsalword, spin->si_foldroot,
				ਸ਼ਬਦ_ਪੂਰਾ >> 16, ਸ਼ਬਦ_ਪੂਰਾ & 0xffff,
							   0) == FAIL)
		    ਵਾਪਸ FAIL;

		++ਸ਼ਬਦ_ਪੂਰਾ;
		++wordcount[ਡੂੰਘਾਈ];

		// Reset the block count each time to avoid compression
		// kicking in.
		spin->si_ਖੰਡਾਂ_cnt = 0;

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		ਜਦੋਂ (n + 1 < slang->sl_fbyts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[ਡੂੰਘਾਈ];
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਡੂੰਘਾਈ < MAXWLEN - 1)
	    {
		// Normal char, go one level deeper.
		tword[ਡੂੰਘਾਈ++] = c;
		arridx[ਡੂੰਘਾਈ] = idxs[n];
		curi[ਡੂੰਘਾਈ] = 1;
		wordcount[ਡੂੰਘਾਈ] = 0;
	    }
	}
    }

    smsg(_("Total number of words: %d"), ਸ਼ਬਦ_ਪੂਰਾ);

    ਵਾਪਸ OK;
}

/*
 * Make the table that links each word in the soundfold trie to the words it
 * can be produced from.
 * This is not unlike lines in a file, thus use a memfile to be able to access
 * the table efficiently.
 * Returns FAIL when out of memory.
 */
    ਸਥਿਰ ਪੂਰਨ
sug_maketable(spellinfo_T *spin)
{
    garray_T	ga;
    ਪੂਰਨ		res = OK;

    // Allocate a buffer, open a memline for it and create the swap file
    // (uses a temp file, not a .swp file).
    spin->si_spellbuf = open_spellbuf();
    ਜੇ (spin->si_spellbuf == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // Use a buffer to store the line info, avoids allocating many small
    // pieces of memory.
    ga_init2(&ga, 1, 100);

    // recursively go through the tree
    ਜੇ (sug_filltable(spin, spin->si_foldroot->wn_sibling, 0, &ga) == -1)
	res = FAIL;

    ga_clear(&ga);
    ਵਾਪਸ res;
}

/*
 * Fill the table for one node and its children.
 * Returns the wordnr at the start of the node.
 * Returns -1 when out of memory.
 */
    ਸਥਿਰ ਪੂਰਨ
sug_filltable(
    spellinfo_T	*spin,
    wordnode_T	*ਗੰਢ,
    ਪੂਰਨ		startwordnr,
    garray_T	*gap)	    // place to store line of numbers
{
    wordnode_T	*p, *np;
    ਪੂਰਨ		wordnr = startwordnr;
    ਪੂਰਨ		nr;
    ਪੂਰਨ		prev_nr;

    FOR_ALL_ਗੰਢ_SIBLINGS(ਗੰਢ, p)
    {
	ਜੇ (p->wn_byte == NUL)
	{
	    gap->ga_len = 0;
	    prev_nr = 0;
	    ਲਈ (np = p; np != ਨਿਰਾ && np->wn_byte == NUL; np = np->wn_sibling)
	    {
		ਜੇ (ga_grow(gap, 10) == FAIL)
		    ਵਾਪਸ -1;

		nr = (np->wn_flags << 16) + (np->wn_region & 0xffff);
		// Compute the offset from the previous nr and store the
		// offset in a way that it takes a minimum number of bytes.
		// It's a bit like utf-8, but without the need to mark
		// following bytes.
		nr -= prev_nr;
		prev_nr += nr;
		gap->ga_len += offset2bytes(nr,
					 (char_u *)gap->ga_ਅੰਕੜੇ + gap->ga_len);
	    }

	    // add the NUL byte
	    ((char_u *)gap->ga_ਅੰਕੜੇ)[gap->ga_len++] = NUL;

	    ਜੇ (ml_append_buf(spin->si_spellbuf, (linenr_T)wordnr,
				     gap->ga_ਅੰਕੜੇ, gap->ga_len, TRUE) == FAIL)
		ਵਾਪਸ -1;
	    ++wordnr;

	    // Remove extra NUL entries, we no longer need them. We don't
	    // bother freeing the nodes, they won't be reused anyway.
	    ਜਦੋਂ (p->wn_sibling != ਨਿਰਾ && p->wn_sibling->wn_byte == NUL)
		p->wn_sibling = p->wn_sibling->wn_sibling;

	    // Clear the flags on the remaining NUL node, so that compression
	    // works a lot better.
	    p->wn_flags = 0;
	    p->wn_region = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    wordnr = sug_filltable(spin, p->wn_ਬੱਚਾ, wordnr, gap);
	    ਜੇ (wordnr == -1)
		ਵਾਪਸ -1;
	}
    }
    ਵਾਪਸ wordnr;
}

/*
 * Convert an offset into a minimal number of bytes.
 * Similar to utf_char2byters, but use 8 bits in followup bytes and avoid NUL
 * bytes.
 */
    ਸਥਿਰ ਪੂਰਨ
offset2bytes(ਪੂਰਨ nr, char_u *buf)
{
    ਪੂਰਨ	    rem;
    ਪੂਰਨ	    b1, b2, b3, b4;

    // Split the number in parts of base 255.  We need to avoid NUL bytes.
    b1 = nr % 255 + 1;
    rem = nr / 255;
    b2 = rem % 255 + 1;
    rem = rem / 255;
    b3 = rem % 255 + 1;
    b4 = rem / 255 + 1;

    ਜੇ (b4 > 1 || b3 > 0x1f)	// 4 bytes
    {
	buf[0] = 0xe0 + b4;
	buf[1] = b3;
	buf[2] = b2;
	buf[3] = b1;
	ਵਾਪਸ 4;
    }
    ਜੇ (b3 > 1 || b2 > 0x3f )	// 3 bytes
    {
	buf[0] = 0xc0 + b3;
	buf[1] = b2;
	buf[2] = b1;
	ਵਾਪਸ 3;
    }
    ਜੇ (b2 > 1 || b1 > 0x7f )	// 2 bytes
    {
	buf[0] = 0x80 + b2;
	buf[1] = b1;
	ਵਾਪਸ 2;
    }
				// 1 byte
    buf[0] = b1;
    ਵਾਪਸ 1;
}

/*
 * Write the .sug file in "fname".
 */
    ਸਥਿਰ ਖਾਲੀ
sug_write(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    wordnode_T	*ਰੁੱਖ;
    ਪੂਰਨ		nodecount;
    ਪੂਰਨ		wcount;
    char_u	*ਸਤਰ;
    linenr_T	lnum;
    ਪੂਰਨ		len;

    // Create the file.  Note that an existing file is silently overwritten!
    fd = mch_fopen((ਅੱਖਰ *)fname, "w");
    ਜੇ (fd == ਨਿਰਾ)
    {
	semsg(_(e_cant_open_file_str), fname);
	ਵਾਪਸ;
    }

    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
				  _("Writing suggestion file %s..."), fname);
    spell_ਸੁਨੇਹਾ(spin, IObuff);

    /*
     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
     */
    ਜੇ (fwrite(VIMSUGMAGIC, VIMSUGMAGICL, (size_t)1, fd) != 1) // <fileID>
    {
	emsg(_(e_error_while_writing));
	ਜਾਓ theend;
    }
    putc(VIMSUGVERSION, fd);				// <versionnr>

    // Write si_sugtime to the file.
    put_ਸਮਾਂ(fd, spin->si_sugtime);			// <timestamp>

    /*
     * <SUGWORDTREE>
     */
    spin->si_memtot = 0;
    ਰੁੱਖ = spin->si_foldroot->wn_sibling;

    // Clear the index and wnode fields in the tree.
    clear_ਗੰਢ(ਰੁੱਖ);

    // Count the number of nodes.  Needed to be able to allocate the
    // memory when reading the nodes.  Also fills in index for shared
    // nodes.
    nodecount = put_ਗੰਢ(ਨਿਰਾ, ਰੁੱਖ, 0, 0, FALSE);

    // number of nodes in 4 bytes
    put_ਬਾਈਟਾਂ(fd, (long_u)nodecount, 4);	// <nodecount>
    spin->si_memtot += nodecount + nodecount * ਆਕਾਰ(ਪੂਰਨ);

    // Write the nodes.
    (ਖਾਲੀ)put_ਗੰਢ(fd, ਰੁੱਖ, 0, 0, FALSE);

    /*
     * <SUGTABLE>: <sugwcount> <sugline> ...
     */
    wcount = spin->si_spellbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    put_ਬਾਈਟਾਂ(fd, (long_u)wcount, 4);	// <sugwcount>

    ਲਈ (lnum = 1; lnum <= (linenr_T)wcount; ++lnum)
    {
	// <sugline>: <sugnr> ... NUL
	ਸਤਰ = ml_get_buf(spin->si_spellbuf, lnum, FALSE);
	len = ml_get_buf_len(spin->si_spellbuf, lnum) + 1;
	ਜੇ (fwrite(ਸਤਰ, (size_t)len, (size_t)1, fd) == 0)
	{
	    emsg(_(e_error_while_writing));
	    ਜਾਓ theend;
	}
	spin->si_memtot += len;
    }

    // Write another byte to check for errors.
    ਜੇ (putc(0, fd) == EOF)
	emsg(_(e_error_while_writing));

    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin->si_memtot);
    spell_ਸੁਨੇਹਾ(spin, IObuff);

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
    ਖਾਲੀ
mkspell(
    ਪੂਰਨ		fcount,
    char_u	**fnames,
    ਪੂਰਨ		ascii,		    // -ascii argument given
    ਪੂਰਨ		over_write,	    // overwrite existing output file
    ਪੂਰਨ		added_ਸ਼ਬਦ)	    // invoked through "zg"
{
    char_u	*fname = ਨਿਰਾ;
    char_u	*wfname;
    char_u	**innames;
    ਪੂਰਨ		incount;
    afffile_T	*(afile[MAXREGIONS]);
    ਪੂਰਨ		i;
    ਪੂਰਨ		len;
    stat_T	st;
    ਪੂਰਨ		error = FALSE;
    spellinfo_T spin;

    CLEAR_ਖੇਤਰ(spin);
    spin.si_verbose = !added_ਸ਼ਬਦ;
    spin.si_ascii = ascii;
    spin.si_followup = TRUE;
    spin.si_rem_accents = TRUE;
    ga_init2(&spin.si_rep, ਆਕਾਰ(fromto_T), 20);
    ga_init2(&spin.si_repsal, ਆਕਾਰ(fromto_T), 20);
    ga_init2(&spin.si_sal, ਆਕਾਰ(fromto_T), 20);
    ga_init2(&spin.si_map, ਆਕਾਰ(char_u), 100);
    ga_init2(&spin.si_comppat, ਆਕਾਰ(char_u *), 20);
    ga_init2(&spin.si_prefcond, ਆਕਾਰ(char_u *), 50);
    hash_init(&spin.si_commonwords);
    spin.si_newcompID = 127;	// start compound ID at first maximum

    // default: fnames[0] is output file, following are input files
    // When "fcount" is 1 there is only one file.
    innames = &fnames[fcount == 1 ? 0 : 1];
    incount = fcount - 1;

    wfname = alloc(MAXPATHL);
    ਜੇ (wfname == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (fcount >= 1)
    {
	len = (ਪੂਰਨ)STRLEN(fnames[0]);
	ਜੇ (fcount == 1 && len > 4 && STRCMP(fnames[0] + len - 4, ".add") == 0)
	{
	    // For ":mkspell path/en.latin1.add" output file is
	    // "path/en.latin1.add.spl".
	    incount = 1;
	    vim_snprintf((ਅੱਖਰ *)wfname, MAXPATHL, "%s.spl", fnames[0]);
	}
	ਨਹੀਂਤਾਂ ਜੇ (fcount == 1)
	{
	    // For ":mkspell path/vim" output file is "path/vim.latin1.spl".
	    incount = 1;
	    vim_snprintf((ਅੱਖਰ *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());
	}
	ਨਹੀਂਤਾਂ ਜੇ (len > 4 && STRCMP(fnames[0] + len - 4, ".spl") == 0)
	{
	    // Name ends in ".spl", use as the file name.
	    vim_strncpy(wfname, fnames[0], MAXPATHL - 1);
	}
	ਨਹੀਂਤਾਂ
	    // Name should be language, make the file name from it.
	    vim_snprintf((ਅੱਖਰ *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());

	// Check for .ascii.spl.
	ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)gettail(wfname), SPL_FNAME_ASCII) != ਨਿਰਾ)
	    spin.si_ascii = TRUE;

	// Check for .add.spl.
	ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)gettail(wfname), SPL_FNAME_ਜੋੜੋ) != ਨਿਰਾ)
	    spin.si_ਜੋੜੋ = TRUE;
    }

    ਜੇ (incount <= 0)
	emsg(_(e_invalid_ਦਲੀਲ));	// need at least output and input names
    ਨਹੀਂਤਾਂ ਜੇ (vim_strchr(gettail(wfname), '_') != ਨਿਰਾ)
	emsg(_(e_output_file_name_must_not_have_region_name));
    ਨਹੀਂਤਾਂ ਜੇ (incount > MAXREGIONS)
	semsg(_(e_only_ਉੱਪਰ_ਮੰਜ਼ਿਲ_nr_regions_supported), MAXREGIONS);
    ਨਹੀਂਤਾਂ
    {
	// Check for overwriting before doing things that may take a lot of
	// time.
	ਜੇ (!over_write && mch_stat((ਅੱਖਰ *)wfname, &st) >= 0)
	{
	    emsg(_(e_file_exists));
	    ਜਾਓ theend;
	}
	ਜੇ (mch_isdir(wfname))
	{
	    semsg(_(e_str_is_directory), wfname);
	    ਜਾਓ theend;
	}

	fname = alloc(MAXPATHL);
	ਜੇ (fname == ਨਿਰਾ)
	    ਜਾਓ theend;

	/*
	 * Init the aff and dic pointers.
	 * Get the region names if there are more than 2 arguments.
	 */
	ਲਈ (i = 0; i < incount; ++i)
	{
	    afile[i] = ਨਿਰਾ;

	    ਜੇ (incount > 1)
	    {
		len = (ਪੂਰਨ)STRLEN(innames[i]);
		ਜੇ (STRLEN(gettail(innames[i])) < 5
						|| innames[i][len - 3] != '_')
		{
		    semsg(_(e_invalid_region_ਵਿੱਚ_str), innames[i]);
		    ਜਾਓ theend;
		}
		spin.si_region_name[i * 2] = TOLOWER_ASC(innames[i][len - 2]);
		spin.si_region_name[i * 2 + 1] =
					     TOLOWER_ASC(innames[i][len - 1]);
	    }
	}
	spin.si_region_ਗਿਣਤੀ = incount;

	spin.si_foldroot = wordtree_alloc(&spin);
	spin.si_keeproot = wordtree_alloc(&spin);
	spin.si_prefroot = wordtree_alloc(&spin);
	ਜੇ (spin.si_foldroot == ਨਿਰਾ
		|| spin.si_keeproot == ਨਿਰਾ
		|| spin.si_prefroot == ਨਿਰਾ)
	{
	    free_ਖੰਡਾਂ(spin.si_ਖੰਡਾਂ);
	    ਜਾਓ theend;
	}

	// When not producing a .add.spl file clear the character table when
	// we encounter one in the .aff file.  This means we dump the current
	// one in the .spl file if the .aff file doesn't define one.  That's
	// better than guessing the contents, the table will match a
	// previously loaded spell file.
	ਜੇ (!spin.si_ਜੋੜੋ)
	    spin.si_clear_chartab = TRUE;

	/*
	 * Read all the .aff and .dic files.
	 * Text is converted to 'encoding'.
	 * Words are stored in the case-folded and keep-case trees.
	 */
	ਲਈ (i = 0; i < incount && !error; ++i)
	{
	    spin.si_conv.vc_type = CONV_NONE;
	    spin.si_region = 1 << i;

	    vim_snprintf((ਅੱਖਰ *)fname, MAXPATHL, "%s.aff", innames[i]);
	    ਜੇ (mch_stat((ਅੱਖਰ *)fname, &st) >= 0)
	    {
		// Read the .aff file.  Will init "spin->si_conv" based on the
		// "SET" line.
		afile[i] = spell_read_aff(&spin, fname);
		ਜੇ (afile[i] == ਨਿਰਾ)
		    error = TRUE;
		ਨਹੀਂਤਾਂ
		{
		    // Read the .dic file and store the words in the trees.
		    vim_snprintf((ਅੱਖਰ *)fname, MAXPATHL, "%s.dic",
								  innames[i]);
		    ਜੇ (spell_read_dic(&spin, fname, afile[i]) == FAIL)
			error = TRUE;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// No .aff file, try reading the file as a word list.  Store
		// the words in the trees.
		ਜੇ (spell_read_wordfile(&spin, innames[i]) == FAIL)
		    error = TRUE;
	    }

	    // Free any conversion stuff.
	    convert_ਤਿਆਰੀ(&spin.si_conv, ਨਿਰਾ, ਨਿਰਾ);
	}

	ਜੇ (spin.si_compflags != ਨਿਰਾ && spin.si_nobreak)
	    ਸੁਨੇਹਾ(_("Warning: both compounding and NOBREAK specified"));

	ਜੇ (!error && !got_int)
	{
	    /*
	     * Combine tails in the tree.
	     */
	    spell_ਸੁਨੇਹਾ(&spin, (char_u *)_(ਸੁਨੇਹਾ_compressing));
	    wordtree_compress(&spin, spin.si_foldroot, "case-folded");
	    wordtree_compress(&spin, spin.si_keeproot, "keep-case");
	    wordtree_compress(&spin, spin.si_prefroot, "prefixes");
	}

	ਜੇ (!error && !got_int)
	{
	    /*
	     * Write the info in the spell file.
	     */
	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
				      _("Writing spell file %s..."), wfname);
	    spell_ਸੁਨੇਹਾ(&spin, IObuff);

	    error = write_vim_spell(&spin, wfname) == FAIL;

	    spell_ਸੁਨੇਹਾ(&spin, (char_u *)_("Done!"));
	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin.si_memtot);
	    spell_ਸੁਨੇਹਾ(&spin, IObuff);

	    /*
	     * If the file is loaded need to reload it.
	     */
	    ਜੇ (!error)
		spell_reload_one(wfname, added_ਸ਼ਬਦ);
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
	ਲਈ (i = 0; i < incount; ++i)
	    ਜੇ (afile[i] != ਨਿਰਾ)
		spell_free_aff(afile[i]);

	// Free all the bits and pieces at once.
	free_ਖੰਡਾਂ(spin.si_ਖੰਡਾਂ);

	/*
	 * If there is soundfolding info and no NOSUGFILE item create the
	 * .sug file with the soundfolded word trie.
	 */
	ਜੇ (spin.si_sugtime != 0 && !error && !got_int)
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
    ਸਥਿਰ ਖਾਲੀ
spell_ਸੁਨੇਹਾ(spellinfo_T *spin, char_u *str)
{
    ਜੇ (spin->si_verbose || p_verbose > 2)
    {
	ਜੇ (!spin->si_verbose)
	    verbose_enter();
	ਸੁਨੇਹਾ((ਅੱਖਰ *)str);
	ਬਾਹਰਲਾ_flush();
	ਜੇ (!spin->si_verbose)
	    verbose_leave();
    }
}

/*
 * ":[count]spellgood  {word}"
 * ":[count]spellwrong {word}"
 * ":[count]spellundo  {word}"
 * ":[count]spellrare  {word}"
 */
    ਖਾਲੀ
ex_spell(exarg_T *eap)
{
    spell_ਜੋੜੋ_ਸ਼ਬਦ(eap->arg, (ਪੂਰਨ)STRLEN(eap->arg),
		eap->cmdidx == CMD_spellwrong ? SPELL_ਜੋੜੋ_BAD :
		eap->cmdidx == CMD_spellrare ? SPELL_ਜੋੜੋ_RARE : SPELL_ਜੋੜੋ_GOOD,
				   eap->forceit ? 0 : (ਪੂਰਨ)eap->line2,
				   eap->cmdidx == CMD_spellundo);
}

/*
 * Add "word[len]" to 'spellfile' as a good, rare or bad word.
 */
    ਖਾਲੀ
spell_ਜੋੜੋ_ਸ਼ਬਦ(
    char_u	*ਸ਼ਬਦ,
    ਪੂਰਨ		len,
    ਪੂਰਨ		what,	    // SPELL_ADD_ values
    ਪੂਰਨ		idx,	    // "zG" and "zW": zero, otherwise index in
			    // 'spellfile'
    ਪੂਰਨ		undo)	    // TRUE for "zug", "zuG", "zuw" and "zuW"
{
    FILE	*fd = ਨਿਰਾ;
    buf_T	*buf = ਨਿਰਾ;
    ਪੂਰਨ		new_spf = FALSE;
    char_u	*fname;
    char_u	*fnamebuf = ਨਿਰਾ;
    char_u	ਸਤਰ[MAXWLEN * 2];
    ਲੰਮਾ	fpos, fpos_ਅਗਲਾ = 0;
    ਪੂਰਨ		i;
    size_t	linelen;
    char_u	*spf;

    ਜੇ (!valid_spell_ਸ਼ਬਦ(ਸ਼ਬਦ, ਸ਼ਬਦ + len))
    {
	emsg(_(e_illegal_character_ਵਿੱਚ_ਸ਼ਬਦ));
	ਵਾਪਸ;
    }

    ਜੇ (idx == 0)	    // use internal wordlist
    {
	ਜੇ (int_wordlist == ਨਿਰਾ)
	{
	    int_wordlist = vim_tempname('s', FALSE);
	    ਜੇ (int_wordlist == ਨਿਰਾ)
		ਵਾਪਸ;
	}
	fname = int_wordlist;
    }
    ਨਹੀਂਤਾਂ
    {
	// If 'spellfile' isn't set figure out a good default value.
	ਜੇ (*curwin->w_s->b_p_spf == NUL)
	{
	    init_spellfile();
	    new_spf = TRUE;
	}

	ਜੇ (*curwin->w_s->b_p_spf == NUL)
	{
	    semsg(_(e_option_str_is_not_ਸੈੱਟ), "spellfile");
	    ਵਾਪਸ;
	}
	fnamebuf = alloc(MAXPATHL);
	ਜੇ (fnamebuf == ਨਿਰਾ)
	    ਵਾਪਸ;

	ਲਈ (spf = curwin->w_s->b_p_spf, i = 1; *spf != NUL; ++i)
	{
	    copy_option_part(&spf, fnamebuf, MAXPATHL, ",");
	    ਜੇ (i == idx)
		ਰੋਕੋ;
	    ਜੇ (*spf == NUL)
	    {
		semsg(_(e_spellfile_does_not_have_nr_entries), idx);
		vim_free(fnamebuf);
		ਵਾਪਸ;
	    }
	}

	// Check that the user isn't editing the .add file somewhere.
	buf = buflist_findname_exp(fnamebuf);
	ਜੇ (buf != ਨਿਰਾ && buf->b_ml.ml_mfp == ਨਿਰਾ)
	    buf = ਨਿਰਾ;
	ਜੇ (buf != ਨਿਰਾ && buf_Is_ਬਦਲਿਆ(buf))
	{
	    emsg(_(e_file_is_loaded_ਵਿੱਚ_another_ਬਫਰ));
	    vim_free(fnamebuf);
	    ਵਾਪਸ;
	}

	fname = fnamebuf;
    }

    ਜੇ (what == SPELL_ਜੋੜੋ_BAD || undo)
    {
	// When the word appears as good word we need to remove that one,
	// since its flags sort before the one with WF_BANNED.
	fd = mch_fopen((ਅੱਖਰ *)fname, "r");
	ਜੇ (fd != ਨਿਰਾ)
	{
	    ਜਦੋਂ (!vim_fgets(ਸਤਰ, MAXWLEN * 2, fd))
	    {
		fpos = fpos_ਅਗਲਾ;
		fpos_ਅਗਲਾ = ftell(fd);
		ਜੇ (fpos_ਅਗਲਾ < 0)
		    ਰੋਕੋ;  // should never happen
		linelen = STRLEN(ਸਤਰ);
		ਜੇ (linelen >= (size_t)len
			&& STRNCMP(ਸ਼ਬਦ, ਸਤਰ, len) == 0
			&& (ਸਤਰ[len] == '/' || ਸਤਰ[len] < ' '))
		{
		    // Found duplicate word.  Remove it by writing a '#' at
		    // the start of the line.  Mixing reading and writing
		    // doesn't work for all systems, close the file first.
		    fclose(fd);
		    fd = mch_fopen((ਅੱਖਰ *)fname, "r+");
		    ਜੇ (fd == ਨਿਰਾ)
			ਰੋਕੋ;
		    ਜੇ (fseek(fd, fpos, SEEK_SET) == 0)
		    {
			fputc('#', fd);
			ਜੇ (undo)
			{
			    home_replace(ਨਿਰਾ, fname, NameBuff, MAXPATHL, TRUE);
			    smsg(_("Word '%.*s' removed from %s"),
							 len, ਸ਼ਬਦ, NameBuff);
			}
		    }
		    ਜੇ (fseek(fd, fpos_ਅਗਲਾ, SEEK_SET) != 0)
		    {
			PERROR(_("Seek error in spellfile"));
			ਰੋਕੋ;
		    }
		}
	    }
	    ਜੇ (fd != ਨਿਰਾ)
		fclose(fd);
	}
    }

    ਜੇ (!undo)
    {
	fd = mch_fopen((ਅੱਖਰ *)fname, "a");
	ਜੇ (fd == ਨਿਰਾ && new_spf)
	{
	    char_u *p;

	    // We just initialized the 'spellfile' option and can't open the
	    // file.  We may need to create the "spell" directory first.  We
	    // already checked the runtime directory is writable in
	    // init_spellfile().
	    ਜੇ (!dir_ਦਾ_file_exists(fname) && (p = gettail_sep(fname)) != fname)
	    {
		ਪੂਰਨ c = *p;

		// The directory doesn't exist.  Try creating it and opening
		// the file again.
		*p = NUL;
		vim_mkdir(fname, 0755);
		*p = c;
		fd = mch_fopen((ਅੱਖਰ *)fname, "a");
	    }
	}

	ਜੇ (fd == ਨਿਰਾ)
	    semsg(_(e_cant_open_file_str), fname);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (what == SPELL_ਜੋੜੋ_BAD)
		fprintf(fd, "%.*s/!\n", len, ਸ਼ਬਦ);
	    ਨਹੀਂਤਾਂ ਜੇ (what == SPELL_ਜੋੜੋ_RARE)
		fprintf(fd, "%.*s/?\n", len, ਸ਼ਬਦ);
	    ਨਹੀਂਤਾਂ
		fprintf(fd, "%.*s\n", len, ਸ਼ਬਦ);
	    fclose(fd);

	    home_replace(ਨਿਰਾ, fname, NameBuff, MAXPATHL, TRUE);
	    smsg(_("Word '%.*s' added to %s"), len, ਸ਼ਬਦ, NameBuff);
	}
    }

    ਜੇ (fd != ਨਿਰਾ)
    {
	// Update the .add.spl file.
	mkspell(1, &fname, FALSE, TRUE, TRUE);

	// If the .add file is edited somewhere, reload it.
	ਜੇ (buf != ਨਿਰਾ)
	    buf_reload(buf, buf->b_orig_ਬਹੁਲਕ, FALSE);

	redraw_all_later(UPD_SOME_VALID);
    }
    vim_free(fnamebuf);
}

/*
 * Initialize 'spellfile' for the current buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
init_spellfile(ਖਾਲੀ)
{
    string_T	buf;
    char_u	*fname;
    char_u	*rtp;
    char_u	*lend;
    ਪੂਰਨ		aspath = FALSE;
    char_u	*lstart = curbuf->b_s.b_p_spl;

    ਜੇ (*curwin->w_s->b_p_spl == NUL || curwin->w_s->b_langp.ga_len <= 0)
	ਵਾਪਸ;

    buf.string = alloc(MAXPATHL);
    ਜੇ (buf.string == ਨਿਰਾ)
	ਵਾਪਸ;

    buf.ਲੰਮਾਈ = 0;

    // Find the end of the language name.  Exclude the region.  If there
    // is a path separator remember the start of the tail.
    ਲਈ (lend = curwin->w_s->b_p_spl; *lend != NUL
	    && vim_strchr((char_u *)",._", *lend) == ਨਿਰਾ; ++lend)
	ਜੇ (vim_ispathsep(*lend))
	{
	    aspath = TRUE;
	    lstart = lend + 1;
	}

    // Loop over all entries in 'runtimepath'.  Use the first one where we
    // are allowed to write.
    rtp = p_rtp;
    ਜਦੋਂ (*rtp != NUL)
    {
	ਜੇ (aspath)
	    // Use directory of an entry with path, e.g., for
	    // "/dir/lg.utf-8.spl" use "/dir".
	    vim_strncpy(buf.string, curbuf->b_s.b_p_spl,
		    lstart - curbuf->b_s.b_p_spl - 1);
	ਨਹੀਂਤਾਂ
	    // Copy the path from 'runtimepath' to buf[].
	    buf.ਲੰਮਾਈ = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");
	ਜੇ (filewritable(buf.string) == 2)
	{
	    // Use the first language name from 'spelllang' and the
	    // encoding used in the first loaded .spl file.
	    ਜੇ (aspath)
	    {
		buf.ਲੰਮਾਈ = (size_t)(lend - curbuf->b_s.b_p_spl);
		vim_strncpy(buf.string, curbuf->b_s.b_p_spl, buf.ਲੰਮਾਈ);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Create the "spell" directory if it doesn't exist yet.
		buf.ਲੰਮਾਈ += vim_snprintf_safelen((ਅੱਖਰ *)buf.string + buf.ਲੰਮਾਈ,
		    MAXPATHL - buf.ਲੰਮਾਈ, "/spell");
		ਜੇ (filewritable(buf.string) != 2)
		{
		    ਜੇ (vim_mkdir(buf.string, 0755) != 0)
		    {
			vim_free(buf.string);
			ਵਾਪਸ;
		    }
		}

		buf.ਲੰਮਾਈ += vim_snprintf_safelen((ਅੱਖਰ *)buf.string + buf.ਲੰਮਾਈ,
		    MAXPATHL - buf.ਲੰਮਾਈ, "/%.*s", (ਪੂਰਨ)(lend - lstart), lstart);
	    }
	    fname = LANGP_ਇੰਦਰਾਜ(curwin->w_s->b_langp, 0)
		->lp_slang->sl_fname;
	    vim_snprintf((ਅੱਖਰ *)buf.string + buf.ਲੰਮਾਈ, MAXPATHL - buf.ਲੰਮਾਈ,
		".%s.add",
		fname != ਨਿਰਾ && ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)gettail(fname), ".ascii.") != ਨਿਰਾ
		    ? (char_u *)"ascii" : spell_enc());
	    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"spellfile",
		    0L, buf.string, OPT_LOCAL);
	    ਰੋਕੋ;
	}
	aspath = FALSE;
    }

    vim_free(buf.string);
}



/*
 * Set the spell character tables from strings in the affix file.
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_spell_chartab(char_u *fol, char_u *ਨੀਵਾਂ, char_u *upp)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    char_u	*pf = fol, *pl = ਨੀਵਾਂ, *pu = upp;
    ਪੂਰਨ		f, l, u;

    clear_spell_chartab(&new_st);

    ਜਦੋਂ (*pf != NUL)
    {
	ਜੇ (*pl == NUL || *pu == NUL)
	{
	    emsg(_(e_format_error_ਵਿੱਚ_affix_file_fol_ਨੀਵਾਂ_or_upp));
	    ਵਾਪਸ FAIL;
	}
	f = mb_ptr2char_adv(&pf);
	l = mb_ptr2char_adv(&pl);
	u = mb_ptr2char_adv(&pu);

	// Every character that appears is a word character.
	ਜੇ (f < 256)
	    new_st.st_isw[f] = TRUE;
	ਜੇ (l < 256)
	    new_st.st_isw[l] = TRUE;
	ਜੇ (u < 256)
	    new_st.st_isw[u] = TRUE;

	// if "LOW" and "FOL" are not the same the "LOW" char needs
	// case-folding
	ਜੇ (l < 256 && l != f)
	{
	    ਜੇ (f >= 256)
	    {
		emsg(_(e_character_ਵਿੱਚ_fol_ਨੀਵਾਂ_or_upp_is_ਬਾਹਰਲਾ_ਦਾ_range));
		ਵਾਪਸ FAIL;
	    }
	    new_st.st_fold[l] = f;
	}

	// if "UPP" and "FOL" are not the same the "UPP" char needs
	// case-folding, it's upper case and the "UPP" is the upper case of
	// "FOL" .
	ਜੇ (u < 256 && u != f)
	{
	    ਜੇ (f >= 256)
	    {
		emsg(_(e_character_ਵਿੱਚ_fol_ਨੀਵਾਂ_or_upp_is_ਬਾਹਰਲਾ_ਦਾ_range));
		ਵਾਪਸ FAIL;
	    }
	    new_st.st_fold[u] = f;
	    new_st.st_isu[u] = TRUE;
	    new_st.st_ਉੱਤਲਾ[f] = u;
	}
    }

    ਜੇ (*pl != NUL || *pu != NUL)
    {
	emsg(_(e_format_error_ਵਿੱਚ_affix_file_fol_ਨੀਵਾਂ_or_upp));
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ ਸੈੱਟ_spell_ਅੰਤ(&new_st);
}

/*
 * Set the spell character tables from strings in the .spl file.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_spell_charflags(
    char_u	*flags,
    ਪੂਰਨ		cnt,	    // length of "flags"
    char_u	*fol)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    ਪੂਰਨ		i;
    char_u	*p = fol;
    ਪੂਰਨ		c;

    clear_spell_chartab(&new_st);

    ਲਈ (i = 0; i < 128; ++i)
    {
	ਜੇ (i < cnt)
	{
	    new_st.st_isw[i + 128] = (flags[i] & CF_ਸ਼ਬਦ) != 0;
	    new_st.st_isu[i + 128] = (flags[i] & CF_ਉੱਤਲਾ) != 0;
	}

	ਜੇ (*p != NUL)
	{
	    c = mb_ptr2char_adv(&p);
	    new_st.st_fold[i + 128] = c;
	    ਜੇ (i + 128 != c && new_st.st_isu[i + 128] && c < 256)
		new_st.st_ਉੱਤਲਾ[c] = i + 128;
	}
    }

    (ਖਾਲੀ)ਸੈੱਟ_spell_ਅੰਤ(&new_st);
}

    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_spell_ਅੰਤ(spelltab_T *new_st)
{
    ਪੂਰਨ		i;

    ਜੇ (did_ਸੈੱਟ_spelltab)
    {
	// check that it's the same table
	ਲਈ (i = 0; i < 256; ++i)
	{
	    ਜੇ (spelltab.st_isw[i] != new_st->st_isw[i]
		    || spelltab.st_isu[i] != new_st->st_isu[i]
		    || spelltab.st_fold[i] != new_st->st_fold[i]
		    || spelltab.st_ਉੱਤਲਾ[i] != new_st->st_ਉੱਤਲਾ[i])
	    {
		emsg(_(e_ਸ਼ਬਦ_characters_ਫਰਕ_between_spell_files));
		ਵਾਪਸ FAIL;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// copy the new spelltab into the one being used
	spelltab = *new_st;
	did_ਸੈੱਟ_spelltab = TRUE;
    }

    ਵਾਪਸ OK;
}

/*
 * Write the table with prefix conditions to the .spl file.
 * When "fd" is NULL only count the length of what is written and return it.
 */
    ਸਥਿਰ ਪੂਰਨ
write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv)
{
    ਪੂਰਨ		i;
    char_u	*p;
    ਪੂਰਨ		len;
    ਪੂਰਨ		totlen;

    ਜੇ (fd != ਨਿਰਾ)
	put_ਬਾਈਟਾਂ(fd, (long_u)gap->ga_len, 2);	    // <prefcondcnt>

    totlen = 2 + gap->ga_len; // length of <prefcondcnt> and <condlen> bytes

    ਲਈ (i = 0; i < gap->ga_len; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	p = ((char_u **)gap->ga_ਅੰਕੜੇ)[i];
	ਜੇ (p != ਨਿਰਾ)
	{
	    len = (ਪੂਰਨ)STRLEN(p);
	    ਜੇ (fd != ਨਿਰਾ)
	    {
		fputc(len, fd);
		*fwv &= fwrite(p, (size_t)len, (size_t)1, fd);
	    }
	    totlen += len;
	}
	ਨਹੀਂਤਾਂ ਜੇ (fd != ਨਿਰਾ)
	    fputc(0, fd);
    }

    ਵਾਪਸ totlen;
}

/*
 * Use map string "map" for languages "lp".
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_map_str(slang_T *lp, char_u *map)
{
    char_u	*p;
    ਪੂਰਨ		headc = 0;
    ਪੂਰਨ		c;
    ਪੂਰਨ		i;

    ਜੇ (*map == NUL)
    {
	lp->sl_has_map = FALSE;
	ਵਾਪਸ;
    }
    lp->sl_has_map = TRUE;

    // Init the array and hash tables empty.
    ਲਈ (i = 0; i < 256; ++i)
	lp->sl_map_array[i] = 0;
    hash_init(&lp->sl_map_hash);

    /*
     * The similar characters are stored separated with slashes:
     * "aaa/bbb/ccc/".  Fill sl_map_array[c] with the character before c and
     * before the same slash.  For characters above 255 sl_map_hash is used.
     */
    ਲਈ (p = map; *p != NUL; )
    {
	c = mb_cptr2char_adv(&p);
	ਜੇ (c == '/')
	    headc = 0;
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (headc == 0)
		 headc = c;

	    // Characters above 255 don't fit in sl_map_array[], put them in
	    // the hash table.  Each entry is the char, a NUL the headchar and
	    // a NUL.
	    ਜੇ (c >= 256)
	    {
		ਪੂਰਨ	    cl = mb_char2len(c);
		ਪੂਰਨ	    headcl = mb_char2len(headc);
		char_u	    *b;
		hash_T	    hash;
		hashitem_T  *hi;

		b = alloc(cl + headcl + 2);
		ਜੇ (b == ਨਿਰਾ)
		    ਵਾਪਸ;
		mb_char2bytes(c, b);
		b[cl] = NUL;
		mb_char2bytes(headc, b + cl + 1);
		b[cl + 1 + headcl] = NUL;
		hash = hash_hash(b);
		hi = hash_lookup(&lp->sl_map_hash, b, hash);
		ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		    hash_ਜੋੜੋ_ਵਸਤੂ(&lp->sl_map_hash, hi, b, hash);
		ਨਹੀਂਤਾਂ
		{
		    // This should have been checked when generating the .spl
		    // file.
		    emsg(_(e_duplicate_char_ਵਿੱਚ_map_ਇੰਦਰਾਜ));
		    vim_free(b);
		}
	    }
	    ਨਹੀਂਤਾਂ
		lp->sl_map_array[c] = headc;
	}
    }
}

#ਜੇ_ਅੰਤ  // FEAT_SPELL
