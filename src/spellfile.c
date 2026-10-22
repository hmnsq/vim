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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_SPELL)

#અન્તર્ભાવ <time.h>	// for time_t

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
#define SN_શબ્દો	13	// common words
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
#define FOR_ALL_ગાંઠ_SIBLINGS(ગાંઠ, np) \
    માટે ((np) = (ગાંઠ); (np) != ખાલી; (np) = (np)->wn_sibling)

સ્થિર પૂર્ણાંક set_spell_finish(spelltab_T	*new_st);
સ્થિર પૂર્ણાંક write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv);
સ્થિર પૂર્ણાંક read_region_section(FILE *fd, slang_T *slang, પૂર્ણાંક len);
સ્થિર પૂર્ણાંક read_charflags_section(FILE *fd);
સ્થિર પૂર્ણાંક read_prefcond_section(FILE *fd, slang_T *lp);
સ્થિર પૂર્ણાંક read_rep_section(FILE *fd, garray_T *gap, short *first);
સ્થિર પૂર્ણાંક read_sal_section(FILE *fd, slang_T *slang);
સ્થિર પૂર્ણાંક read_શબ્દો_section(FILE *fd, slang_T *lp, પૂર્ણાંક len);
સ્થિર પૂર્ણાંક read_sofo_section(FILE *fd, slang_T *slang);
સ્થિર પૂર્ણાંક read_compound(FILE *fd, slang_T *slang, પૂર્ણાંક len);
સ્થિર પૂર્ણાંક set_sofo(slang_T *lp, char_u *from, char_u *to);
સ્થિર શૂન્ય set_sal_first(slang_T *lp);
સ્થિર પૂર્ણાંક *mb_str2wide(char_u *s);
સ્થિર પૂર્ણાંક spell_read_વૃક્ષ(FILE *fd, char_u **bytsp, long *bytsp_len, idx_T **idxsp, પૂર્ણાંક prefixtree, પૂર્ણાંક prefixcnt);
સ્થિર idx_T read_વૃક્ષ_ગાંઠ(FILE *fd, char_u *byts, idx_T *idxs, પૂર્ણાંક maxidx, idx_T startidx, પૂર્ણાંક prefixtree, પૂર્ણાંક maxprefcondnr, પૂર્ણાંક depth);
સ્થિર શૂન્ય set_spell_charflags(char_u *flags, પૂર્ણાંક cnt, char_u *upp);
સ્થિર પૂર્ણાંક set_spell_chartab(char_u *fol, char_u *નીચલું, char_u *upp);
સ્થિર શૂન્ય set_map_str(slang_T *lp, char_u *map);


સ્થિર અક્ષર *e_afftrailing = N_("Trailing text in %s line %d: %s");
સ્થિર અક્ષર *e_affname = N_("Affix name too long in %s line %d: %s");
સ્થિર અક્ષર *સંદેશ_compressing = N_("Compressing word tree...");

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
    પૂર્ણાંક		silent)		// no error if file doesn't exist
{
    FILE	*fd;
    char_u	buf[VIMSPELLMAGICL];
    char_u	*p;
    પૂર્ણાંક		i;
    પૂર્ણાંક		n;
    પૂર્ણાંક		len;
    slang_T	*lp = ખાલી;
    પૂર્ણાંક		c = 0;
    પૂર્ણાંક		res;
    પૂર્ણાંક		did_estack_ધકેલો = FALSE;
    ESTACK_CHECK_DECLARATION;

    fd = mch_fopen((અક્ષર *)fname, "r");
    જો (fd == ખાલી)
    {
	જો (!silent)
	    semsg(_(e_cant_open_file_str), fname);
	નહીંતર જો (p_verbose > 2)
	{
	    verbose_enter();
	    smsg((અચલ અક્ષર *)e_cant_open_file_str, fname);
	    verbose_leave();
	}
	જાઓ endFAIL;
    }
    જો (p_verbose > 2)
    {
	verbose_enter();
	smsg(_("Reading spell file \"%s\""), fname);
	verbose_leave();
    }

    જો (old_lp == ખાલી)
    {
	lp = slang_alloc(lang);
	જો (lp == ખાલી)
	    જાઓ endFAIL;

	// Remember the file name, used to reload the file when it's updated.
	lp->sl_fname = vim_strsave(fname);
	જો (lp->sl_fname == ખાલી)
	    જાઓ endFAIL;

	// Check for .add.spl (_add.spl for VMS).
	lp->sl_add = strstr((અક્ષર *)gettail(fname), SPL_FNAME_ADD) != ખાલી;
    }
    નહીંતર
	lp = old_lp;

    // Set sourcing_name, so that error messages mention the file name.
    estack_ધકેલો(ETYPE_SPELL, fname, 0);
    ESTACK_CHECK_SETUP;
    did_estack_ધકેલો = TRUE;

    /*
     * <HEADER>: <fileID>
     */
    માટે (i = 0; i < VIMSPELLMAGICL; ++i)
	buf[i] = (c = getc(fd)) == EOF ? 0 : c;		// <fileID>
    જો (STRNCMP(buf, VIMSPELLMAGIC, VIMSPELLMAGICL) != 0)
    {
	emsg(_(e_this_does_not_look_like_spell_file));
	જાઓ endFAIL;
    }
    c = getc(fd);					// <versionnr>
    જો (c < VIMSPELLVERSION)
    {
	emsg(_(e_old_spell_file_needs_to_be_updated));
	જાઓ endFAIL;
    }
    નહીંતર જો (c > VIMSPELLVERSION)
    {
	emsg(_(e_spell_file_is_for_newer_version_of_vim));
	જાઓ endFAIL;
    }


    /*
     * <SECTIONS>: <section> ... <sectionend>
     * <section>: <sectionID> <sectionflags> <sectionlen> (section contents)
     */
    માટે (;;)
    {
	n = getc(fd);			    // <sectionID> or <sectionend>
	જો (n == SN_END)
	    તોડો;
	c = getc(fd);					// <sectionflags>
	len = get4c(fd);				// <sectionlen>
	જો (len < 0)
	    જાઓ truncerr;

	res = 0;
	પસંદગી (n)
	{
	    સ્થિતિ SN_INFO:
		lp->sl_info = read_string(fd, len);	// <infotext>
		જો (lp->sl_info == ખાલી)
		    જાઓ endFAIL;
		તોડો;

	    સ્થિતિ SN_REGION:
		res = read_region_section(fd, lp, len);
		તોડો;

	    સ્થિતિ SN_CHARFLAGS:
		res = read_charflags_section(fd);
		તોડો;

	    સ્થિતિ SN_MIDWORD:
		lp->sl_midword = read_string(fd, len);	// <midword>
		જો (lp->sl_midword == ખાલી)
		    જાઓ endFAIL;
		તોડો;

	    સ્થિતિ SN_PREFCOND:
		res = read_prefcond_section(fd, lp);
		તોડો;

	    સ્થિતિ SN_REP:
		res = read_rep_section(fd, &lp->sl_rep, lp->sl_rep_first);
		તોડો;

	    સ્થિતિ SN_REPSAL:
		res = read_rep_section(fd, &lp->sl_repsal, lp->sl_repsal_first);
		તોડો;

	    સ્થિતિ SN_SAL:
		res = read_sal_section(fd, lp);
		તોડો;

	    સ્થિતિ SN_SOFO:
		res = read_sofo_section(fd, lp);
		તોડો;

	    સ્થિતિ SN_MAP:
		p = read_string(fd, len);		// <mapstr>
		જો (p == ખાલી)
		    જાઓ endFAIL;
		set_map_str(lp, p);
		vim_free(p);
		તોડો;

	    સ્થિતિ SN_શબ્દો:
		res = read_શબ્દો_section(fd, lp, len);
		તોડો;

	    સ્થિતિ SN_SUGFILE:
		lp->sl_sugtime = get8ctime(fd);		// <timestamp>
		તોડો;

	    સ્થિતિ SN_NOSPLITSUGS:
		lp->sl_nosplitsugs = TRUE;
		તોડો;

	    સ્થિતિ SN_NOCOMPOUNDSUGS:
		lp->sl_nocompoundsugs = TRUE;
		તોડો;

	    સ્થિતિ SN_COMPOUND:
		res = read_compound(fd, lp, len);
		તોડો;

	    સ્થિતિ SN_NOBREAK:
		lp->sl_nobreak = TRUE;
		તોડો;

	    સ્થિતિ SN_SYLLABLE:
		lp->sl_syllable = read_string(fd, len);	// <syllable>
		જો (lp->sl_syllable == ખાલી)
		    જાઓ endFAIL;
		જો (init_syl_tab(lp) != OK)
		    જાઓ endFAIL;
		તોડો;

	    મૂળભૂત:
		// Unsupported section.  When it's required give an error
		// message.  When it's not required skip the contents.
		જો (c & SNF_REQUIRED)
		{
		    emsg(_(e_unsupported_section_in_spell_file));
		    જાઓ endFAIL;
		}
		જ્યારે (--len >= 0)
		    જો (getc(fd) < 0)
			જાઓ truncerr;
		તોડો;
	}
someerror:
	જો (res == SP_FORMERROR)
	{
	    emsg(_(e_format_error_in_spell_file));
	    જાઓ endFAIL;
	}
	જો (res == SP_TRUNCERROR)
	{
truncerr:
	    emsg(_(e_truncated_spell_file));
	    જાઓ endFAIL;
	}
	જો (res == SP_OTHERERROR)
	    જાઓ endFAIL;
    }

    // <LWORDTREE>
    res = spell_read_વૃક્ષ(fd, &lp->sl_fbyts, &lp->sl_fbyts_len,
						      &lp->sl_fidxs, FALSE, 0);
    જો (res != 0)
	જાઓ someerror;

    // <KWORDTREE>
    res = spell_read_વૃક્ષ(fd, &lp->sl_kbyts, ખાલી, &lp->sl_kidxs, FALSE, 0);
    જો (res != 0)
	જાઓ someerror;

    // <PREFIXTREE>
    res = spell_read_વૃક્ષ(fd, &lp->sl_pbyts, ખાલી, &lp->sl_pidxs, TRUE,
							    lp->sl_prefixcnt);
    જો (res != 0)
	જાઓ someerror;

    // For a new file link it in the list of spell files.
    જો (old_lp == ખાલી && lang != ખાલી)
    {
	lp->sl_next = first_lang;
	first_lang = lp;
    }

    જાઓ endOK;

endFAIL:
    જો (lang != ખાલી)
	// truncating the name signals the error to spell_load_lang()
	*lang = NUL;
    જો (lp != ખાલી && old_lp == ખાલી)
	slang_free(lp);
    lp = ખાલી;

endOK:
    જો (fd != ખાલી)
	fclose(fd);
    જો (did_estack_ધકેલો)
    {
	ESTACK_CHECK_NOW;
	estack_કાઢો();
    }

    પાછા lp;
}

/*
 * Fill in the wordcount fields for a trie.
 * Returns the total number of words.
 */
    સ્થિર શૂન્ય
વૃક્ષ_ગણતરી_શબ્દો(char_u *byts, long byts_len, idx_T *idxs)
{
    પૂર્ણાંક		depth;
    idx_T	arridx[MAXWLEN];
    પૂર્ણાંક		curi[MAXWLEN];
    પૂર્ણાંક		c;
    idx_T	n;
    પૂર્ણાંક		wordcount[MAXWLEN];

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;
    depth = 0;
    જ્યારે (depth >= 0 && !got_int)
    {
	જો (curi[depth] > byts[arridx[depth]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[depth]] = wordcount[depth];
	    જો (depth > 0)
		wordcount[depth - 1] += wordcount[depth];

	    --depth;
	    fast_breakcheck();
	}
	નહીંતર
	{
	    // Do one more byte at this node.
	    n = arridx[depth] + curi[depth];
	    ++curi[depth];

	    c = byts[n];
	    જો (c == 0)
	    {
		// End of word, count it.
		++wordcount[depth];

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		જ્યારે (n + 1 < byts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[depth];
		}
	    }
	    નહીંતર જો (depth < MAXWLEN - 1)
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
    શૂન્ય
suggest_load_files(શૂન્ય)
{
    langp_T	*lp;
    પૂર્ણાંક		lpi;
    slang_T	*slang;
    char_u	*dotp;
    FILE	*fd;
    char_u	buf[MAXWLEN];
    પૂર્ણાંક		i;
    time_t	timestamp;
    પૂર્ણાંક		wcount;
    પૂર્ણાંક		wordnr;
    garray_T	ga;
    પૂર્ણાંક		c;

    // Do this for all languages that support sound folding.
    માટે (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	જો (slang->sl_sugtime != 0 && !slang->sl_sugloaded)
	{
	    // Change ".spl" to ".sug" and open the file.  When the file isn't
	    // found silently skip it.  Do set "sl_sugloaded" so that we
	    // don't try again and again.
	    slang->sl_sugloaded = TRUE;

	    dotp = vim_strrchr(slang->sl_fname, '.');
	    જો (dotp == ખાલી || fnamecmp(dotp, ".spl") != 0)
		ચાલુ;
	    STRCPY(dotp, ".sug");
	    fd = mch_fopen((અક્ષર *)slang->sl_fname, "r");
	    જો (fd == ખાલી)
		જાઓ nextone;

	    /*
	     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
	     */
	    માટે (i = 0; i < VIMSUGMAGICL; ++i)
		buf[i] = (c = getc(fd)) == EOF ? 0 : c;	// <fileID>
	    જો (STRNCMP(buf, VIMSUGMAGIC, VIMSUGMAGICL) != 0)
	    {
		semsg(_(e_this_does_not_look_like_sug_file_str),
							     slang->sl_fname);
		જાઓ nextone;
	    }
	    c = getc(fd);				// <versionnr>
	    જો (c < VIMSUGVERSION)
	    {
		semsg(_(e_old_sug_file_needs_to_be_updated_str),
							     slang->sl_fname);
		જાઓ nextone;
	    }
	    નહીંતર જો (c > VIMSUGVERSION)
	    {
		semsg(_(e_sug_file_is_for_newer_version_of_vim_str),
							     slang->sl_fname);
		જાઓ nextone;
	    }

	    // Check the timestamp, it must be exactly the same as the one in
	    // the .spl file.  Otherwise the word numbers won't match.
	    timestamp = get8ctime(fd);			// <timestamp>
	    જો (timestamp != slang->sl_sugtime)
	    {
		semsg(_(e_sug_file_doesnt_match_spl_file_str),
							     slang->sl_fname);
		જાઓ nextone;
	    }

	    /*
	     * <SUGWORDTREE>: <wordtree>
	     * Read the trie with the soundfolded words.
	     */
	    જો (spell_read_વૃક્ષ(fd, &slang->sl_sbyts, &slang->sl_sbyts_len,
			&slang->sl_sidxs, FALSE, 0) != 0)
	    {
someerror:
		semsg(_(e_error_while_reading_sug_file_str),
							     slang->sl_fname);
		slang_clear_sug(slang);
		જાઓ nextone;
	    }

	    /*
	     * <SUGTABLE>: <sugwcount> <sugline> ...
	     *
	     * Read the table with word numbers.  We use a file buffer for
	     * this, because it's so much like a file with lines.  Makes it
	     * possible to swap the info and save on memory use.
	     */
	    slang->sl_sugbuf = open_spellbuf();
	    જો (slang->sl_sugbuf == ખાલી)
		જાઓ someerror;
							    // <sugwcount>
	    wcount = get4c(fd);
	    જો (wcount < 0)
		જાઓ someerror;

	    // Read all the wordnr lists into the buffer, one NUL terminated
	    // list per line.
	    ga_init2(&ga, 1, 100);
	    માટે (wordnr = 0; wordnr < wcount; ++wordnr)
	    {
		ga.ga_len = 0;
		માટે (;;)
		{
		    c = getc(fd);			    // <sugline>
		    જો (c < 0 || ga_grow(&ga, 1) == FAIL)
			જાઓ someerror;
		    ((char_u *)ga.ga_data)[ga.ga_len++] = c;
		    જો (c == NUL)
			તોડો;
		}
		જો (ml_append_buf(slang->sl_sugbuf, (linenr_T)wordnr,
					 ga.ga_data, ga.ga_len, TRUE) == FAIL)
		    જાઓ someerror;
	    }
	    ga_clear(&ga);

	    /*
	     * Need to put word counts in the word tries, so that we can find
	     * a word by its number.
	     */
	    વૃક્ષ_ગણતરી_શબ્દો(slang->sl_fbyts, slang->sl_fbyts_len,
		    slang->sl_fidxs);
	    વૃક્ષ_ગણતરી_શબ્દો(slang->sl_sbyts, slang->sl_sbyts_len,
		    slang->sl_sidxs);

nextone:
	    જો (fd != ખાલી)
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
    સ્થિર char_u *
read_cnt_string(FILE *fd, પૂર્ણાંક cnt_bytes, પૂર્ણાંક *cntp)
{
    પૂર્ણાંક		cnt = 0;
    પૂર્ણાંક		i;
    char_u	*str;

    // read the length bytes, MSB first
    માટે (i = 0; i < cnt_bytes; ++i)
    {
	પૂર્ણાંક c = getc(fd);

	જો (c == EOF)
	{
	    *cntp = SP_TRUNCERROR;
	    પાછા ખાલી;
	}
	cnt = (cnt << 8) + (unsigned)c;
    }
    *cntp = cnt;
    જો (cnt == 0)
	પાછા ખાલી;	    // nothing to read, return NULL

    str = read_string(fd, cnt);
    જો (str == ખાલી)
	*cntp = SP_OTHERERROR;
    પાછા str;
}

/*
 * Read SN_REGION: <regionname> ...
 * Return SP_*ERROR flags.
 */
    સ્થિર પૂર્ણાંક
read_region_section(FILE *fd, slang_T *lp, પૂર્ણાંક len)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		c = 0;

    જો (len > MAXREGIONS * 2)
	પાછા SP_FORMERROR;
    માટે (i = 0; i < len; ++i)
	lp->sl_regions[i] = (c = getc(fd)) == EOF ? 0 : c; // <regionname>
    lp->sl_regions[len] = NUL;
    પાછા c == EOF ? SP_TRUNCERROR : 0;
}

/*
 * Read SN_CHARFLAGS section: <charflagslen> <charflags>
 *				<folcharslen> <folchars>
 * Return SP_*ERROR flags.
 */
    સ્થિર પૂર્ણાંક
read_charflags_section(FILE *fd)
{
    char_u	*flags;
    char_u	*fol;
    પૂર્ણાંક		flagslen, follen;

    // <charflagslen> <charflags>
    flags = read_cnt_string(fd, 1, &flagslen);
    જો (flagslen < 0)
	પાછા flagslen;

    // <folcharslen> <folchars>
    fol = read_cnt_string(fd, 2, &follen);
    જો (follen < 0)
    {
	vim_free(flags);
	પાછા follen;
    }

    // Set the word-char flags and fill SPELL_ISUPPER() table.
    જો (flags != ખાલી && fol != ખાલી)
	set_spell_charflags(flags, flagslen, fol);

    vim_free(flags);
    vim_free(fol);

    // When <charflagslen> is zero then <fcharlen> must also be zero.
    જો ((flags == ખાલી) != (fol == ખાલી))
	પાછા SP_FORMERROR;
    પાછા 0;
}

/*
 * Read SN_PREFCOND section.
 * Return SP_*ERROR flags.
 */
    સ્થિર પૂર્ણાંક
read_prefcond_section(FILE *fd, slang_T *lp)
{
    પૂર્ણાંક		cnt;
    પૂર્ણાંક		i;
    પૂર્ણાંક		n;
    પૂર્ણાંક		c;
    char_u	*p;
    char_u	buf[MAXWLEN + 1];

    // <prefcondcnt> <prefcond> ...
    cnt = get2c(fd);					// <prefcondcnt>
    જો (cnt <= 0)
	પાછા SP_FORMERROR;

    lp->sl_prefprog = ALLOC_CLEAR_MULT(regprog_T *, cnt);
    જો (lp->sl_prefprog == ખાલી)
	પાછા SP_OTHERERROR;
    lp->sl_prefixcnt = cnt;

    માટે (i = 0; i < cnt; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	n = getc(fd);					// <condlen>
	જો (n < 0 || n >= MAXWLEN)
	    પાછા SP_FORMERROR;

	// When <condlen> is zero we have an empty condition.  Otherwise
	// compile the regexp program used to check for the condition.
	જો (n > 0)
	{
	    buf[0] = '^';	    // always match at one position only
	    p = buf + 1;
	    જ્યારે (n-- > 0)
		*p++ = (c = getc(fd)) == EOF ? 0 : c;	    // <condstr>
	    જો (c == EOF)
		તોડો;
	    *p = NUL;
	    lp->sl_prefprog[i] = vim_regcomp(buf, RE_MAGIC + RE_STRING);
	}
    }
    પાછા 0;
}

/*
 * Read REP or REPSAL items section from "fd": <repcount> <rep> ...
 * Return SP_*ERROR flags.
 */
    સ્થિર પૂર્ણાંક
read_rep_section(FILE *fd, garray_T *gap, short *first)
{
    પૂર્ણાંક		cnt;
    fromto_T	*ftp;
    પૂર્ણાંક		i;

    cnt = get2c(fd);					// <repcount>
    જો (cnt < 0)
	પાછા SP_TRUNCERROR;

    જો (ga_grow(gap, cnt) == FAIL)
	પાછા SP_OTHERERROR;

    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
    માટે (; gap->ga_len < cnt; ++gap->ga_len)
    {
	ftp = &((fromto_T *)gap->ga_data)[gap->ga_len];
	ftp->ft_from = read_cnt_string(fd, 1, &i);
	જો (i < 0)
	    પાછા i;
	જો (i == 0)
	    પાછા SP_FORMERROR;
	ftp->ft_to = read_cnt_string(fd, 1, &i);
	જો (i <= 0)
	{
	    vim_free(ftp->ft_from);
	    જો (i < 0)
		પાછા i;
	    પાછા SP_FORMERROR;
	}
    }

    // Fill the first-index table.
    માટે (i = 0; i < 256; ++i)
	first[i] = -1;
    માટે (i = 0; i < gap->ga_len; ++i)
    {
	ftp = &((fromto_T *)gap->ga_data)[i];
	જો (first[*ftp->ft_from] == -1)
	    first[*ftp->ft_from] = i;
    }
    પાછા 0;
}

/*
 * Read SN_SAL section: <salflags> <salcount> <sal> ...
 * Return SP_*ERROR flags.
 */
    સ્થિર પૂર્ણાંક
read_sal_section(FILE *fd, slang_T *slang)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		cnt;
    garray_T	*gap;
    salitem_T	*smp;
    પૂર્ણાંક		ccnt;
    char_u	*p;

    slang->sl_sofo = FALSE;

    i = getc(fd);				// <salflags>
    જો (i & SAL_F0LLOWUP)
	slang->sl_followup = TRUE;
    જો (i & SAL_COLLAPSE)
	slang->sl_collapse = TRUE;
    જો (i & SAL_REM_ACCENTS)
	slang->sl_rem_accents = TRUE;

    cnt = get2c(fd);				// <salcount>
    જો (cnt < 0)
	પાછા SP_TRUNCERROR;

    gap = &slang->sl_sal;
    ga_init2(gap, કદ(salitem_T), 10);
    જો (ga_grow(gap, cnt + 1) == FAIL)
	પાછા SP_OTHERERROR;

    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
    માટે (; gap->ga_len < cnt; ++gap->ga_len)
    {
	પૂર્ણાંક	c = NUL;

	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	ccnt = getc(fd);			// <salfromlen>
	જો (ccnt < 0)
	    પાછા SP_TRUNCERROR;
	જો ((p = alloc(ccnt + 2)) == ખાલી)
	    પાછા SP_OTHERERROR;
	smp->sm_lead = p;

	// Read up to the first special char into sm_lead.
	માટે (i = 0; i < ccnt; ++i)
	{
	    c = getc(fd);			// <salfrom>
	    જો (vim_strchr((char_u *)"0123456789(-<^$", c) != ખાલી)
		તોડો;
	    *p++ = c;
	}
	smp->sm_leadlen = (પૂર્ણાંક)(p - smp->sm_lead);
	*p++ = NUL;

	// Put (abc) chars in sm_oneof, if any.
	જો (c == '(')
	{
	    smp->sm_oneof = p;
	    માટે (++i; i < ccnt; ++i)
	    {
		c = getc(fd);			// <salfrom>
		જો (c == ')')
		    તોડો;
		*p++ = c;
	    }
	    *p++ = NUL;
	    જો (++i < ccnt)
		c = getc(fd);
	}
	નહીંતર
	    smp->sm_oneof = ખાલી;

	// Any following chars go in sm_rules.
	smp->sm_rules = p;
	જો (i < ccnt)
	    // store the char we got while checking for end of sm_lead
	    *p++ = c;
	માટે (++i; i < ccnt; ++i)
	    *p++ = (c = getc(fd)) == EOF ? 0 : c;	// <salfrom>
	*p++ = NUL;

	// <saltolen> <salto>
	smp->sm_to = read_cnt_string(fd, 1, &ccnt);
	જો (ccnt < 0)
	{
	    vim_free(smp->sm_lead);
	    પાછા ccnt;
	}

	જો (has_mbyte)
	{
	    // convert the multi-byte strings to wide char strings
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = mb_charlen(smp->sm_lead);
	    જો (smp->sm_oneof == ખાલી)
		smp->sm_oneof_w = ખાલી;
	    નહીંતર
		smp->sm_oneof_w = mb_str2wide(smp->sm_oneof);
	    જો (smp->sm_to == ખાલી)
		smp->sm_to_w = ખાલી;
	    નહીંતર
		smp->sm_to_w = mb_str2wide(smp->sm_to);
	    જો (smp->sm_lead_w == ખાલી
		    || (smp->sm_oneof_w == ખાલી && smp->sm_oneof != ખાલી)
		    || (smp->sm_to_w == ખાલી && smp->sm_to != ખાલી))
	    {
		vim_free(smp->sm_lead);
		vim_free(smp->sm_to);
		vim_free(smp->sm_lead_w);
		vim_free(smp->sm_oneof_w);
		vim_free(smp->sm_to_w);
		પાછા SP_OTHERERROR;
	    }
	}
    }

    જો (gap->ga_len > 0)
    {
	// Add one extra entry to mark the end with an empty sm_lead.  Avoids
	// that we need to check the index every time.
	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	જો ((p = alloc(1)) == ખાલી)
	    પાછા SP_OTHERERROR;
	p[0] = NUL;
	smp->sm_lead = p;
	smp->sm_leadlen = 0;
	smp->sm_oneof = ખાલી;
	smp->sm_rules = p;
	smp->sm_to = ખાલી;
	જો (has_mbyte)
	{
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = 0;
	    smp->sm_oneof_w = ખાલી;
	    smp->sm_to_w = ખાલી;
	}
	++gap->ga_len;
    }

    // Fill the first-index table.
    set_sal_first(slang);

    પાછા 0;
}

/*
 * Read SN_WORDS: <word> ...
 * Return SP_*ERROR flags.
 */
    સ્થિર પૂર્ણાંક
read_શબ્દો_section(FILE *fd, slang_T *lp, પૂર્ણાંક len)
{
    પૂર્ણાંક		done = 0;
    પૂર્ણાંક		i;
    પૂર્ણાંક		c;
    char_u	word[MAXWLEN];

    જ્યારે (done < len)
    {
	// Read one word at a time.
	માટે (i = 0; ; ++i)
	{
	    c = getc(fd);
	    જો (c == EOF)
		પાછા SP_TRUNCERROR;
	    word[i] = c;
	    જો (word[i] == NUL)
		તોડો;
	    જો (i == MAXWLEN - 1)
		પાછા SP_FORMERROR;
	}

	// Init the count to 10.
	ગણતરી_common_word(lp, word, -1, 10);
	done += i + 1;
    }
    પાછા 0;
}

/*
 * SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
 * Return SP_*ERROR flags.
 */
    સ્થિર પૂર્ણાંક
read_sofo_section(FILE *fd, slang_T *slang)
{
    પૂર્ણાંક		cnt;
    char_u	*from, *to;
    પૂર્ણાંક		res;

    // <sofofromlen> <sofofrom>
    from = read_cnt_string(fd, 2, &cnt);
    જો (cnt < 0)
	પાછા cnt;

    // <sofotolen> <sofoto>
    to = read_cnt_string(fd, 2, &cnt);
    જો (cnt < 0)
    {
	vim_free(from);
	પાછા cnt;
    }

    // Store the info in slang->sl_sal and/or slang->sl_sal_first.
    જો (from != ખાલી && to != ખાલી)
	res = set_sofo(slang, from, to);
    નહીંતર જો (from != ખાલી || to != ખાલી)
	res = SP_FORMERROR;    // only one of two strings is an error
    નહીંતર
	res = 0;

    vim_free(from);
    vim_free(to);
    પાછા res;
}

/*
 * Read the compound section from the .spl file:
 *	<compmax> <compminlen> <compsylmax> <compoptions> <compflags>
 * Returns SP_*ERROR flags.
 */
    સ્થિર પૂર્ણાંક
read_compound(FILE *fd, slang_T *slang, પૂર્ણાંક len)
{
    પૂર્ણાંક		todo = len;
    પૂર્ણાંક		c;
    પૂર્ણાંક		atstart;
    char_u	*pat;
    char_u	*pp;
    char_u	*cp;
    char_u	*ap;
    char_u	*crp;
    પૂર્ણાંક		cnt;
    garray_T	*gap;
    size_t	patsize;
    size_t	flagsize;

    જો (todo < 2)
	પાછા SP_FORMERROR;	// need at least two bytes

    --todo;
    c = getc(fd);					// <compmax>
    જો (c < 2)
	c = MAXWLEN;
    slang->sl_compmax = c;

    --todo;
    c = getc(fd);					// <compminlen>
    જો (c < 1)
	c = 0;
    slang->sl_compminlen = c;

    --todo;
    c = getc(fd);					// <compsylmax>
    જો (c < 1)
	c = MAXWLEN;
    slang->sl_compsylmax = c;

    c = getc(fd);					// <compoptions>
    જો (c != 0)
	ungetc(c, fd);	    // be backwards compatible with Vim 7.0b
    નહીંતર
    {
	--todo;
	c = getc(fd);	    // only use the lower byte for now
	--todo;
	slang->sl_compoptions = c;

	gap = &slang->sl_comppat;
	c = get2c(fd);					// <comppatcount>
	જો (c < 0)
	    પાછા SP_TRUNCERROR;
	todo -= 2;
	ga_init2(gap, કદ(char_u *), c);
	જો (ga_grow(gap, c) == OK)
	    જ્યારે (--c >= 0)
	    {
		((char_u **)(gap->ga_data))[gap->ga_len++] =
						  read_cnt_string(fd, 1, &cnt);
					    // <comppatlen> <comppattext>
		જો (cnt < 0)
		    પાછા cnt;
		todo -= cnt + 1;
	    }
    }
    જો (todo < 0)
	પાછા SP_FORMERROR;

    // Turn the COMPOUNDRULE items into a regexp pattern:
    // "a[bc]/a*b+" -> "^\(a[bc]\|a*b\+\)$".
    // Inserting backslashes may double the length, "^\(\)$<Nul>" is 7 bytes.
    // Conversion to utf-8 may double the size.
    જો ((size_t)todo > COMPOUND_MAX_LEN)
	પાછા SP_FORMERROR;
    patsize = (size_t)todo * 2 + 7;
    જો (enc_utf8)
	patsize += (size_t)todo * 2;
    flagsize = (size_t)todo + 1;
    pat = alloc(patsize);
    જો (pat == ખાલી)
	પાછા SP_OTHERERROR;

    // We also need a list of all flags that can appear at the start and one
    // for all flags.
    cp = alloc(flagsize);
    જો (cp == ખાલી)
    {
	vim_free(pat);
	પાછા SP_OTHERERROR;
    }
    slang->sl_compstartflags = cp;
    *cp = NUL;

    ap = alloc(flagsize);
    જો (ap == ખાલી)
    {
	vim_free(pat);
	પાછા SP_OTHERERROR;
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
    જ્યારે (todo-- > 0)
    {
	c = getc(fd);					// <compflags>
	જો (c == EOF)
	{
	    vim_free(pat);
	    પાછા SP_TRUNCERROR;
	}

	// Add all flags to "sl_compallflags".
	જો (vim_strchr((char_u *)"?*+[]/", c) == ખાલી
		&& !byte_in_str(slang->sl_compallflags, c))
	{
	    *ap++ = c;
	    *ap = NUL;
	}

	જો (atstart != 0)
	{
	    // At start of item: copy flags to "sl_compstartflags".  For a
	    // [abc] item set "atstart" to 2 and copy up to the ']'.
	    જો (c == '[')
		atstart = 2;
	    નહીંતર જો (c == ']')
		atstart = 0;
	    નહીંતર
	    {
		જો (!byte_in_str(slang->sl_compstartflags, c))
		{
		    *cp++ = c;
		    *cp = NUL;
		}
		જો (atstart == 1)
		    atstart = 0;
	    }
	}

	// Copy flag to "sl_comprules", unless we run into a wildcard.
	જો (crp != ખાલી)
	{
	    જો (c == '?' || c == '+' || c == '*')
	    {
		VIM_CLEAR(slang->sl_comprules);
		crp = ખાલી;
	    }
	    નહીંતર
		*crp++ = c;
	}

	જો (c == '/')	    // slash separates two items
	{
	    *pp++ = '\\';
	    *pp++ = '|';
	    atstart = 1;
	}
	નહીંતર		    // normal char, "[abc]" and '*' are copied as-is
	{
	    જો (c == '?' || c == '+' || c == '~')
		*pp++ = '\\';	    // "a?" becomes "a\?", "a+" becomes "a\+"
	    જો (enc_utf8)
		pp += mb_char2bytes(c, pp);
	    નહીંતર
		*pp++ = c;
	}
    }

    *pp++ = '\\';
    *pp++ = ')';
    *pp++ = '$';
    *pp = NUL;

    જો (crp != ખાલી)
	*crp = NUL;

    slang->sl_compprog = vim_regcomp(pat, RE_MAGIC + RE_STRING + RE_STRICT);
    vim_free(pat);
    જો (slang->sl_compprog == ખાલી)
	પાછા SP_FORMERROR;

    પાછા 0;
}

/*
 * Set the SOFOFROM and SOFOTO items in language "lp".
 * Returns SP_*ERROR flags when there is something wrong.
 */
    સ્થિર પૂર્ણાંક
set_sofo(slang_T *lp, char_u *from, char_u *to)
{
    પૂર્ણાંક		i;

    garray_T	*gap;
    char_u	*s;
    char_u	*p;
    પૂર્ણાંક		c;
    પૂર્ણાંક		*inp;

    જો (has_mbyte)
    {
	free_sal_વસ્તુઓ(&lp->sl_sal);

	// Use "sl_sal" as an array with 256 pointers to a list of wide
	// characters.  The index is the low byte of the character.
	// The list contains from-to pairs with a terminating NUL.
	// sl_sal_first[] is used for latin1 "from" characters.
	gap = &lp->sl_sal;
	ga_init2(gap, કદ(પૂર્ણાંક *), 1);
	જો (ga_grow(gap, 256) == FAIL)
	    પાછા SP_OTHERERROR;
	vim_memset(gap->ga_data, 0, કદ(પૂર્ણાંક *) * 256);
	gap->ga_len = 256;
	lp->sl_sofo = TRUE;

	// First count the number of items for each list.  Temporarily use
	// sl_sal_first[] for this.  Reset it first: a preceding SN_SAL section
	// may have set the entries to -1 via set_sal_first().
	vim_memset(lp->sl_sal_first, 0, કદ(salfirst_T) * 256);
	માટે (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    MB_CPTR_ADV(s);
	    જો (c >= 256)
		++lp->sl_sal_first[c & 0xff];
	}
	જો (*p != NUL || *s != NUL)	    // lengths differ
	    પાછા SP_FORMERROR;

	// Allocate the lists.
	માટે (i = 0; i < 256; ++i)
	    જો (lp->sl_sal_first[i] > 0)
	    {
		p = alloc(કદ(પૂર્ણાંક) * (lp->sl_sal_first[i] * 2 + 1));
		જો (p == ખાલી)
		    પાછા SP_OTHERERROR;
		((પૂર્ણાંક **)gap->ga_data)[i] = (પૂર્ણાંક *)p;
		*(પૂર્ણાંક *)p = 0;
	    }

	// Put the characters up to 255 in sl_sal_first[] the rest in a sl_sal
	// list.
	vim_memset(lp->sl_sal_first, 0, કદ(salfirst_T) * 256);
	માટે (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    i = mb_cptr2char_adv(&s);
	    જો (c >= 256)
	    {
		// Append the from-to chars at the end of the list with
		// the low byte.
		inp = ((પૂર્ણાંક **)gap->ga_data)[c & 0xff];
		જ્યારે (*inp != 0)
		    ++inp;
		*inp++ = c;		// from char
		*inp++ = i;		// to char
		*inp++ = NUL;		// NUL at the end
	    }
	    નહીંતર
		// mapping byte to char is done in sl_sal_first[]
		lp->sl_sal_first[c] = i;
	}
    }
    નહીંતર
    {
	// mapping bytes to bytes is done in sl_sal_first[]
	જો (STRLEN(from) != STRLEN(to))
	    પાછા SP_FORMERROR;

	માટે (i = 0; to[i] != NUL; ++i)
	    lp->sl_sal_first[from[i]] = to[i];
	lp->sl_sal.ga_len = 1;		// indicates we have soundfolding
	lp->sl_sofo = TRUE;
    }

    પાછા 0;
}

/*
 * Fill the first-index table for "lp".
 */
    સ્થિર શૂન્ય
set_sal_first(slang_T *lp)
{
    salfirst_T	*sfirst;
    પૂર્ણાંક		i;
    salitem_T	*smp;
    પૂર્ણાંક		c;
    garray_T	*gap = &lp->sl_sal;

    sfirst = lp->sl_sal_first;
    માટે (i = 0; i < 256; ++i)
	sfirst[i] = -1;
    smp = (salitem_T *)gap->ga_data;
    માટે (i = 0; i < gap->ga_len; ++i)
    {
	જો (has_mbyte)
	    // Use the lowest byte of the first character.  For latin1 it's
	    // the character, for other encodings it should differ for most
	    // characters.
	    c = *smp[i].sm_lead_w & 0xff;
	નહીંતર
	    c = *smp[i].sm_lead;
	જો (sfirst[c] == -1)
	{
	    sfirst[c] = i;
	    જો (has_mbyte)
	    {
		પૂર્ણાંક		n;

		// Make sure all entries with this byte are following each
		// other.  Move the ones that are in the wrong position.  Do
		// keep the same ordering!
		જ્યારે (i + 1 < gap->ga_len
				       && (*smp[i + 1].sm_lead_w & 0xff) == c)
		    // Skip over entry with same index byte.
		    ++i;

		માટે (n = 1; i + n < gap->ga_len; ++n)
		    જો ((*smp[i + n].sm_lead_w & 0xff) == c)
		    {
			salitem_T  tsal;

			// Move entry with same index byte after the entries
			// we already found.
			++i;
			--n;
			tsal = smp[i + n];
			mch_memmove(smp + i + 1, smp + i,
						       કદ(salitem_T) * n);
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
    સ્થિર પૂર્ણાંક *
mb_str2wide(char_u *s)
{
    પૂર્ણાંક		*res;
    char_u	*p;
    પૂર્ણાંક		i = 0;

    res = ALLOC_MULT(પૂર્ણાંક, mb_charlen(s) + 1);
    જો (res == ખાલી)
	પાછા ખાલી;

    માટે (p = s; *p != NUL; )
	res[i++] = mb_ptr2char_adv(&p);
    res[i] = NUL;
    પાછા res;
}

/*
 * Read a tree from the .spl or .sug file.
 * Allocates the memory and stores pointers in "bytsp" and "idxsp".
 * This is skipped when the tree has zero length.
 * Returns zero when OK, SP_ value for an error.
 */
    સ્થિર પૂર્ણાંક
spell_read_વૃક્ષ(
    FILE	*fd,
    char_u	**bytsp,
    long	*bytsp_len,
    idx_T	**idxsp,
    પૂર્ણાંક		prefixtree,	// TRUE for the prefix tree
    પૂર્ણાંક		prefixcnt)	// when "prefixtree" is TRUE: prefix count
{
    long	len;
    પૂર્ણાંક		idx;
    char_u	*bp;
    idx_T	*ip;

    // The tree size was computed when writing the file, so that we can
    // allocate it as one long block. <nodecount>
    len = get4c(fd);
    જો (len < 0)
	પાછા SP_TRUNCERROR;
    જો ((size_t)len > SIZE_MAX / કદ(પૂર્ણાંક))
	// Invalid length, multiply with sizeof(int) would overflow.
	પાછા SP_FORMERROR;
    જો (len <= 0)
	પાછા 0;

    // Allocate the byte array.  Zero-initialize so that any position the
    // tree does not visit reads as 0; a stray BY_INDEX shared reference
    // into such a slot then behaves as end-of-word in spellsuggest()
    // instead of consuming an arbitrary heap byte as a siblingcount.
    bp = alloc_clear(len);
    જો (bp == ખાલી)
	પાછા SP_OTHERERROR;
    *bytsp = bp;
    જો (bytsp_len != ખાલી)
	*bytsp_len = len;

    // Allocate the index array.
    ip = lalloc_clear(len * કદ(પૂર્ણાંક), TRUE);
    જો (ip == ખાલી)
	પાછા SP_OTHERERROR;
    *idxsp = ip;

    // Recursively read the tree and store it in the array.
    idx = read_વૃક્ષ_ગાંઠ(fd, bp, ip, len, 0, prefixtree, prefixcnt, 0);
    જો (idx < 0)
	પાછા idx;
    જો (idx != len)
	પાછા SP_FORMERROR;
    પાછા 0;
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
    સ્થિર idx_T
read_વૃક્ષ_ગાંઠ(
    FILE	*fd,
    char_u	*byts,
    idx_T	*idxs,
    પૂર્ણાંક		maxidx,		    // size of arrays
    idx_T	startidx,	    // current index in "byts" and "idxs"
    પૂર્ણાંક		prefixtree,	    // TRUE for reading PREFIXTREE
    પૂર્ણાંક		maxprefcondnr,	    // maximum for <prefcondnr>
    પૂર્ણાંક		depth)		    // recursion level
{
    પૂર્ણાંક		len;
    પૂર્ણાંક		i;
    પૂર્ણાંક		n;
    idx_T	idx = startidx;
    પૂર્ણાંક		c;
    પૂર્ણાંક		c2;
#define SHARED_MASK	0x8000000

    // Bail out on a crafted .spl whose tree recurses beyond the maximum
    // word length: each tree level corresponds to one byte of a word, so
    // any well-formed file has depth <= MAXWLEN.
    જો (depth > MAXWLEN)
	પાછા SP_FORMERROR;

    len = getc(fd);					// <siblingcount>
    જો (len <= 0)
	પાછા SP_TRUNCERROR;

    જો (len >= maxidx - startidx)
	પાછા SP_FORMERROR;
    byts[idx++] = len;

    // Read the byte values, flag/region bytes and shared indexes.
    માટે (i = 1; i <= len; ++i)
    {
	c = getc(fd);					// <byte>
	જો (c < 0)
	    પાછા SP_TRUNCERROR;
	જો (c <= BY_SPECIAL)
	{
	    જો (c == BY_NOFLAGS && !prefixtree)
	    {
		// No flags, all regions.
		idxs[idx] = 0;
		c = 0;
	    }
	    નહીંતર જો (c != BY_INDEX)
	    {
		જો (prefixtree)
		{
		    // Read the optional pflags byte, the prefix ID and the
		    // condition nr.  In idxs[] store the prefix ID in the low
		    // byte, the condition index shifted up 8 bits, the flags
		    // shifted up 24 bits.
		    જો (c == BY_FLAGS)
		    {
			n = getc(fd);			// <pflags>
			જો (n < 0)
			    પાછા SP_TRUNCERROR;
			c = n << 24;
		    }
		    નહીંતર
			c = 0;

		    n = getc(fd);			// <affixID>
		    જો (n < 0)
			પાછા SP_TRUNCERROR;
		    c |= n;

		    n = get2c(fd);			// <prefcondnr>
		    જો (n < 0 || n >= maxprefcondnr)
			પાછા SP_FORMERROR;
		    c |= (n << 8);
		}
		નહીંતર // c must be BY_FLAGS or BY_FLAGS2
		{
		    // Read flags and optional region and prefix ID.  In
		    // idxs[] the flags go in the low two bytes, region above
		    // that and prefix ID above the region.
		    c2 = c;
		    c = getc(fd);			// <flags>
		    જો (c < 0)
			પાછા SP_TRUNCERROR;
		    જો (c2 == BY_FLAGS2)
		    {
			n = getc(fd);			// <flags2>
			જો (n < 0)
			    પાછા SP_TRUNCERROR;
			c = (n << 8) + c;
		    }
		    જો (c & WF_REGION)
		    {
			n = getc(fd);			// <region>
			જો (n < 0)
			    પાછા SP_TRUNCERROR;
			c = (n << 16) + c;
		    }
		    જો (c & WF_AFX)
		    {
			n = getc(fd);			// <affixID>
			જો (n < 0)
			    પાછા SP_TRUNCERROR;
			c = (n << 24) + c;
		    }
		}

		idxs[idx] = c;
		c = 0;
	    }
	    નહીંતર // c == BY_INDEX
	    {
							// <nodeidx>
		n = get3c(fd);
		જો (n < 0 || n >= maxidx)
		    પાછા SP_FORMERROR;
		idxs[idx] = n + SHARED_MASK;
		c = getc(fd);				// <xbyte>
	    }
	}
	byts[idx++] = c;
    }

    // Recursively read the children for non-shared siblings.
    // Skip the end-of-word ones (zero byte value) and the shared ones (and
    // remove SHARED_MASK)
    માટે (i = 1; i <= len; ++i)
	જો (byts[startidx + i] != 0)
	{
	    જો (idxs[startidx + i] & SHARED_MASK)
		idxs[startidx + i] &= ~SHARED_MASK;
	    નહીંતર
	    {
		idxs[startidx + i] = idx;
		idx = read_વૃક્ષ_ગાંઠ(fd, byts, idxs, maxidx, idx,
					prefixtree, maxprefcondnr, depth + 1);
		જો (idx < 0)
		    તોડો;
	    }
	}

    પાછા idx;
}

/*
 * Reload the spell file "fname" if it's loaded.
 */
    સ્થિર શૂન્ય
spell_reload_one(
    char_u	*fname,
    પૂર્ણાંક		added_word)	// invoked through "zg"
{
    slang_T	*slang;
    પૂર્ણાંક		didit = FALSE;

    FOR_ALL_SPELL_LANGS(slang)
    {
	જો (fullpathcmp(fname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	{
	    slang_clear(slang);
	    જો (spell_load_file(fname, ખાલી, slang, FALSE) == ખાલી)
		// reloading failed, clear the language
		slang_clear(slang);
	    redraw_all_later(UPD_SOME_VALID);
	    didit = TRUE;
	}
    }

    // When "zg" was used and the file wasn't loaded yet, should redo
    // 'spelllang' to load it now.
    જો (added_word && !didit)
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
પ્રકારવ્યાખ્યા સંરચના afffile_S
{
    char_u	*af_enc;	// "SET", normalized, alloc'ed string or NULL
    પૂર્ણાંક		af_flagtype;	// AFT_CHAR, AFT_LONG, AFT_NUM or AFT_CAPLONG
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
    પૂર્ણાંક		af_pfxpostpone;	// postpone prefixes without chop string and
				// without flags
    પૂર્ણાંક		af_ignoreextra;	// IGNOREEXTRA present
    hashtab_T	af_pref;	// hashtable for prefixes, affheader_T
    hashtab_T	af_suff;	// hashtable for suffixes, affheader_T
    hashtab_T	af_comp;	// hashtable for compound flags, compitem_T
} afffile_T;

#define AFT_CHAR	0	// flags are one character
#define AFT_LONG	1	// flags are two characters
#define AFT_CAPLONG	2	// flags are one or two characters
#define AFT_અંક		3	// flags are numbers, comma separated

પ્રકારવ્યાખ્યા સંરચના affentry_S affentry_T;
// Affix entry from ".aff" file.  Used for prefixes and suffixes.
સંરચના affentry_S
{
    affentry_T	*ae_next;	// next affix with same name/number
    char_u	*ae_chop;	// text to chop off basic word (can be NULL)
    char_u	*ae_add;	// text to add to basic word (can be NULL)
    char_u	*ae_flags;	// flags on the affix (can be NULL)
    char_u	*ae_cond;	// condition (NULL for ".")
    regprog_T	*ae_prog;	// regexp program for ae_cond or NULL
    અક્ષર	ae_compforbid;	// COMPOUNDFORBIDFLAG found
    અક્ષર	ae_comppermit;	// COMPOUNDPERMITFLAG found
};

#define AH_ચાવી_LEN 17		// 2 x 8 bytes + NUL

// Affix header from ".aff" file.  Used for af_pref and af_suff.
પ્રકારવ્યાખ્યા સંરચના affheader_S
{
    char_u	ah_ચાવી[AH_ચાવી_LEN]; // key for hashtab == name of affix
    unsigned	ah_flag;	// affix name as number, uses "af_flagtype"
    પૂર્ણાંક		ah_newID;	// prefix ID after renumbering; 0 if not used
    પૂર્ણાંક		ah_combine;	// suffix may combine with prefix
    પૂર્ણાંક		ah_follows;	// another affix block should be following
    affentry_T	*ah_first;	// first affix entry
} affheader_T;

#define HI2AH(hi)   ((affheader_T *)(hi)->hi_ચાવી)

// Flag used in compound items.
પ્રકારવ્યાખ્યા સંરચના compitem_S
{
    char_u	ci_ચાવી[AH_ચાવી_LEN]; // key for hashtab == name of compound
    unsigned	ci_flag;	// affix name as number, uses "af_flagtype"
    પૂર્ણાંક		ci_newID;	// affix ID after renumbering.
} compitem_T;

#define HI2CI(hi)   ((compitem_T *)(hi)->hi_ચાવી)

/*
 * Structure that is used to store the items in the word tree.  This avoids
 * the need to keep track of each allocated thing, everything is freed all at
 * once after ":mkspell" is done.
 * Note: "sb_next" must be just before "sb_data" to make sure the alignment of
 * "sb_data" is correct for systems where pointers must be aligned on
 * pointer-size boundaries and sizeof(pointer) > sizeof(int) (e.g., Sparc).
 */
#define  SBLOCKSIZE 16000	// size of sb_data
પ્રકારવ્યાખ્યા સંરચના sblock_S sblock_T;
સંરચના sblock_S
{
    પૂર્ણાંક		sb_used;	// nr of bytes already in use
    sblock_T	*sb_next;	// next block in list
    char_u	sb_data[1];	// data, actually longer
};

/*
 * A node in the tree.
 */
પ્રકારવ્યાખ્યા સંરચના wordnode_S wordnode_T;
સંરચના wordnode_S
{
    સંઘ   // shared to save space
    {
	char_u	hashkey[6];	// the hash key, only used while compressing
	પૂર્ણાંક	index;		// index in written nodes (valid after first
				// round)
    } wn_u1;
    સંઘ   // shared to save space
    {
	wordnode_T *next;	// next node with same hash key
	wordnode_T *wnode;	// parent node that will write this node
    } wn_u2;
    wordnode_T	*wn_child;	// child (next byte in word)
    wordnode_T  *wn_sibling;	// next sibling (alternate byte in word,
				// always sorted)
    પૂર્ણાંક		wn_refs;	// Nr. of references to this node.  Only
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
    પૂર્ણાંક		wn_nr;		// sequence nr for printing
#endif
};

#define WN_MASK	 0xffff		// mask relevant bits of "wn_flags"

#define HI2WN(hi)    (wordnode_T *)((hi)->hi_ચાવી)

/*
 * Info used while reading the spell files.
 */
પ્રકારવ્યાખ્યા સંરચના spellinfo_S
{
    wordnode_T	*si_foldroot;	// tree with case-folded words
    long	si_foldwcount;	// nr of words in si_foldroot

    wordnode_T	*si_keeproot;	// tree with keep-case words
    long	si_keepwcount;	// nr of words in si_keeproot

    wordnode_T	*si_prefroot;	// tree with postponed prefixes

    long	si_sugtree;	// creating the soundfolding trie

    sblock_T	*si_blocks;	// memory blocks used
    long	si_blocks_cnt;	// memory blocks allocated
    પૂર્ણાંક		si_did_emsg;	// TRUE when ran out of memory

    long	si_compress_cnt;    // words to add before lowering
				    // compression limit
    wordnode_T	*si_first_free; // List of nodes that have been freed during
				// compression, linked by "wn_child" field.
    long	si_free_ગણતરી;	// number of nodes in si_first_free
#ifdef SPELL_PRINTTREE
    પૂર્ણાંક		si_wordnode_nr;	// sequence nr for nodes
#endif
    buf_T	*si_spellbuf;	// buffer used to store soundfold word table

    પૂર્ણાંક		si_ascii;	// handling only ASCII words
    પૂર્ણાંક		si_add;		// addition file
    પૂર્ણાંક		si_clear_chartab;   // when TRUE clear char tables
    પૂર્ણાંક		si_region;	// region mask
    vimconv_T	si_conv;	// for conversion to 'encoding'
    પૂર્ણાંક		si_memtot;	// runtime memory used
    પૂર્ણાંક		si_verbose;	// verbose messages
    પૂર્ણાંક		si_સંદેશ_ગણતરી;	// number of words added since last message
    char_u	*si_info;	// info text chars or NULL
    પૂર્ણાંક		si_region_ગણતરી; // number of regions supported (1 when there
				 // are no regions)
    char_u	si_region_name[MAXREGIONS * 2 + 1];
				// region names; used only if
				// si_region_count > 1)

    garray_T	si_rep;		// list of fromto_T entries from REP lines
    garray_T	si_repsal;	// list of fromto_T entries from REPSAL lines
    garray_T	si_sal;		// list of fromto_T entries from SAL lines
    char_u	*si_sofofr;	// SOFOFROM text
    char_u	*si_sofoto;	// SOFOTO text
    પૂર્ણાંક		si_nosugfile;	// NOSUGFILE item found
    પૂર્ણાંક		si_nosplitsugs;	// NOSPLITSUGS item found
    પૂર્ણાંક		si_nocompoundsugs; // NOCOMPOUNDSUGS item found
    પૂર્ણાંક		si_followup;	// soundsalike: ?
    પૂર્ણાંક		si_collapse;	// soundsalike: ?
    hashtab_T	si_commonwords;	// hashtable for common words
    time_t	si_sugtime;	// timestamp for .sug file
    પૂર્ણાંક		si_rem_accents;	// soundsalike: remove accents
    garray_T	si_map;		// MAP info concatenated
    char_u	*si_midword;	// MIDWORD chars or NULL
    પૂર્ણાંક		si_compmax;	// max nr of words for compounding
    પૂર્ણાંક		si_compminlen;	// minimal length for compounding
    પૂર્ણાંક		si_compsylmax;	// max nr of syllables for compounding
    પૂર્ણાંક		si_compoptions;	// COMP_ flags
    garray_T	si_comppat;	// CHECKCOMPOUNDPATTERN items, each stored as
				// a string
    char_u	*si_compflags;	// flags used for compounding
    char_u	si_nobreak;	// NOBREAK
    char_u	*si_syllable;	// syllable string
    garray_T	si_prefcond;	// table with conditions for postponed
				// prefixes, each stored as a string
    પૂર્ણાંક		si_newprefID;	// current value for ah_newID
    પૂર્ણાંક		si_newcompID;	// current value for compound ID
} spellinfo_T;

સ્થિર પૂર્ણાંક is_aff_rule(char_u **વસ્તુઓ, પૂર્ણાંક itemcnt, અક્ષર *rulename, પૂર્ણાંક	 mincount);
સ્થિર શૂન્ય aff_process_flags(afffile_T *affile, affentry_T *entry);
સ્થિર પૂર્ણાંક spell_info_item(char_u *s);
સ્થિર unsigned affitem2flag(પૂર્ણાંક flagtype, char_u *item, char_u	*fname, પૂર્ણાંક lnum);
સ્થિર unsigned get_affitem(પૂર્ણાંક flagtype, char_u **pp);
સ્થિર શૂન્ય process_compflags(spellinfo_T *spin, afffile_T *aff, char_u *compflags);
સ્થિર શૂન્ય check_renumber(spellinfo_T *spin);
સ્થિર શૂન્ય aff_check_સંખ્યા(પૂર્ણાંક spinval, પૂર્ણાંક affval, અક્ષર *name);
સ્થિર શૂન્ય aff_check_string(char_u *spinval, char_u *affval, અક્ષર *name);
સ્થિર પૂર્ણાંક str_equal(char_u *s1, char_u	*s2);
સ્થિર શૂન્ય add_fromto(spellinfo_T *spin, garray_T *gap, char_u	*from, char_u *to);
સ્થિર પૂર્ણાંક sal_to_bool(char_u *s);
સ્થિર પૂર્ણાંક get_affix_flags(afffile_T *affile, char_u *afflist);
સ્થિર પૂર્ણાંક get_pfxlist(afffile_T *affile, char_u *afflist, char_u *store_afflist, પૂર્ણાંક *cntp);
સ્થિર પૂર્ણાંક get_compflags(afffile_T *affile, char_u *afflist, char_u *store_afflist, પૂર્ણાંક *cntp);
સ્થિર પૂર્ણાંક store_aff_word(spellinfo_T *spin, char_u *word, char_u *afflist, afffile_T *affile, hashtab_T *ht, hashtab_T *xht, પૂર્ણાંક condit, પૂર્ણાંક flags, char_u *pfxlist, પૂર્ણાંક pfxlen);
સ્થિર શૂન્ય *getroom(spellinfo_T *spin, size_t len, પૂર્ણાંક align);
સ્થિર char_u *getroom_save(spellinfo_T *spin, char_u *s);
સ્થિર પૂર્ણાંક store_word(spellinfo_T *spin, char_u *word, પૂર્ણાંક flags, પૂર્ણાંક region, char_u *pfxlist, પૂર્ણાંક need_affix);
સ્થિર પૂર્ણાંક વૃક્ષ_add_word(spellinfo_T *spin, char_u *word, wordnode_T *વૃક્ષ, પૂર્ણાંક flags, પૂર્ણાંક region, પૂર્ણાંક affixID);
સ્થિર wordnode_T *get_wordnode(spellinfo_T *spin);
સ્થિર શૂન્ય free_wordnode(spellinfo_T *spin, wordnode_T *n);
સ્થિર શૂન્ય wordtree_compress(spellinfo_T *spin, wordnode_T *મૂળ, અક્ષર *name);
સ્થિર long ગાંઠ_compress(spellinfo_T *spin, wordnode_T *ગાંઠ, hashtab_T *ht, long *tot);
સ્થિર પૂર્ણાંક ગાંઠ_equal(wordnode_T *n1, wordnode_T *n2);
સ્થિર શૂન્ય clear_ગાંઠ(wordnode_T *ગાંઠ);
સ્થિર પૂર્ણાંક put_ગાંઠ(FILE *fd, wordnode_T *ગાંઠ, પૂર્ણાંક idx, પૂર્ણાંક regionmask, પૂર્ણાંક prefixtree);
સ્થિર પૂર્ણાંક sug_filltree(spellinfo_T *spin, slang_T *slang);
સ્થિર પૂર્ણાંક sug_maketable(spellinfo_T *spin);
સ્થિર પૂર્ણાંક sug_filltable(spellinfo_T *spin, wordnode_T *ગાંઠ, પૂર્ણાંક startwordnr, garray_T *gap);
સ્થિર પૂર્ણાંક offset2bytes(પૂર્ણાંક nr, char_u *buf);
સ્થિર શૂન્ય sug_write(spellinfo_T *spin, char_u *fname);
સ્થિર શૂન્ય spell_message(spellinfo_T *spin, char_u *str);
સ્થિર શૂન્ય init_spellfile(શૂન્ય);

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
સ્થિર long compress_start = 30000;	// memory / SBLOCKSIZE
સ્થિર long compress_inc = 100;		// memory / SBLOCKSIZE
સ્થિર long compress_added = 500000;	// word count

/*
 * Check the 'mkspellmem' option.  Return FAIL if it's wrong.
 * Sets "sps_flags".
 */
    પૂર્ણાંક
spell_check_msm(શૂન્ય)
{
    char_u	*p = p_msm;
    long	start = 0;
    long	incr = 0;
    long	added = 0;

    જો (!VIM_ISDIGIT(*p))
	પાછા FAIL;
    // block count = (value * 1024) / SBLOCKSIZE (but avoid overflow)
    start = (getdigits(&p) * 10) / (SBLOCKSIZE / 102);
    જો (*p != ',')
	પાછા FAIL;
    ++p;
    જો (!VIM_ISDIGIT(*p))
	પાછા FAIL;
    incr = (getdigits(&p) * 102) / (SBLOCKSIZE / 10);
    જો (*p != ',')
	પાછા FAIL;
    ++p;
    જો (!VIM_ISDIGIT(*p))
	પાછા FAIL;
    added = getdigits(&p) * 1024;
    જો (*p != NUL)
	પાછા FAIL;

    જો (start == 0 || incr == 0 || added == 0 || incr > start)
	પાછા FAIL;

    compress_start = start;
    compress_inc = incr;
    compress_added = added;
    પાછા OK;
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

સ્થિર અક્ષર line1[PRINTLINESIZE];
સ્થિર અક્ષર line2[PRINTLINESIZE];
સ્થિર અક્ષર line3[PRINTLINESIZE];

    સ્થિર શૂન્ય
spell_clear_flags(wordnode_T *ગાંઠ)
{
    wordnode_T	*np;

    FOR_ALL_ગાંઠ_SIBLINGS(ગાંઠ, np)
    {
	np->wn_u1.index = FALSE;
	spell_clear_flags(np->wn_child);
    }
}

    સ્થિર શૂન્ય
spell_print_ગાંઠ(wordnode_T *ગાંઠ, પૂર્ણાંક depth)
{
    જો (ગાંઠ->wn_u1.index)
    {
	// Done this node before, print the reference.
	PRINTSOME(line1, depth, "(%d)", ગાંઠ->wn_nr, 0);
	PRINTSOME(line2, depth, "    ", 0, 0);
	PRINTSOME(line3, depth, "    ", 0, 0);
	સંદેશ(line1);
	સંદેશ(line2);
	સંદેશ(line3);
    }
    નહીંતર
    {
	ગાંઠ->wn_u1.index = TRUE;

	જો (ગાંઠ->wn_byte != NUL)
	{
	    જો (ગાંઠ->wn_child != ખાલી)
		PRINTSOME(line1, depth, " %c -> ", ગાંઠ->wn_byte, 0);
	    નહીંતર
		// Cannot happen?
		PRINTSOME(line1, depth, " %c ???", ગાંઠ->wn_byte, 0);
	}
	નહીંતર
	    PRINTSOME(line1, depth, " $    ", 0, 0);

	PRINTSOME(line2, depth, "%d/%d    ", ગાંઠ->wn_nr, ગાંઠ->wn_refs);

	જો (ગાંઠ->wn_sibling != ખાલી)
	    PRINTSOME(line3, depth, " |    ", 0, 0);
	નહીંતર
	    PRINTSOME(line3, depth, "      ", 0, 0);

	જો (ગાંઠ->wn_byte == NUL)
	{
	    સંદેશ(line1);
	    સંદેશ(line2);
	    સંદેશ(line3);
	}

	// do the children
	જો (ગાંઠ->wn_byte != NUL && ગાંઠ->wn_child != ખાલી)
	    spell_print_ગાંઠ(ગાંઠ->wn_child, depth + 1);

	// do the siblings
	જો (ગાંઠ->wn_sibling != ખાલી)
	{
	    // get rid of all parent details except |
	    STRCPY(line1, line3);
	    STRCPY(line2, line3);
	    spell_print_ગાંઠ(ગાંઠ->wn_sibling, depth);
	}
    }
}

    સ્થિર શૂન્ય
spell_print_વૃક્ષ(wordnode_T *મૂળ)
{
    જો (મૂળ == ખાલી)
	પાછા;

    // Clear the "wn_u1.index" fields, used to remember what has been done.
    spell_clear_flags(મૂળ);

    // Recursively print the tree.
    spell_print_ગાંઠ(મૂળ, 0);
}
#endif // SPELL_PRINTTREE

/*
 * Read the affix file "fname".
 * Returns an afffile_T, NULL for complete failure.
 */
    સ્થિર afffile_T *
spell_read_aff(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    afffile_T	*aff;
    char_u	rline[MAXLINELEN];
    char_u	*લીટી;
    char_u	*pc = ખાલી;
#define MAXITEMCNT  30
    char_u	*(વસ્તુઓ[MAXITEMCNT]);
    પૂર્ણાંક		itemcnt;
    char_u	*p;
    પૂર્ણાંક		lnum = 0;
    affheader_T	*cur_aff = ખાલી;
    પૂર્ણાંક		did_postpone_prefix = FALSE;
    પૂર્ણાંક		aff_todo = 0;
    hashtab_T	*tp;
    char_u	*નીચલું = ખાલી;
    char_u	*fol = ખાલી;
    char_u	*upp = ખાલી;
    પૂર્ણાંક		do_rep;
    પૂર્ણાંક		do_repsal;
    પૂર્ણાંક		do_sal;
    પૂર્ણાંક		do_mapline;
    પૂર્ણાંક		found_map = FALSE;
    hashitem_T	*hi;
    પૂર્ણાંક		l;
    પૂર્ણાંક		compminlen = 0;		// COMPOUNDMIN value
    પૂર્ણાંક		compsylmax = 0;		// COMPOUNDSYLMAX value
    પૂર્ણાંક		compoptions = 0;	// COMP_ flags
    પૂર્ણાંક		compmax = 0;		// COMPOUNDWORDMAX value
    char_u	*compflags = ખાલી;	// COMPOUNDFLAG and COMPOUNDRULE
					// concatenated
    char_u	*midword = ખાલી;	// MIDWORD value
    char_u	*syllable = ખાલી;	// SYLLABLE value
    char_u	*sofofrom = ખાલી;	// SOFOFROM value
    char_u	*sofoto = ખાલી;		// SOFOTO value

    /*
     * Open the file.
     */
    fd = mch_fopen((અક્ષર *)fname, "r");
    જો (fd == ખાલી)
    {
	semsg(_(e_cant_open_file_str), fname);
	પાછા ખાલી;
    }

    vim_snprintf((અક્ષર *)IObuff, IOSIZE, _("Reading affix file %s..."), fname);
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
    aff = (afffile_T *)getroom(spin, કદ(afffile_T), TRUE);
    જો (aff == ખાલી)
    {
	fclose(fd);
	પાછા ખાલી;
    }
    hash_init(&aff->af_pref);
    hash_init(&aff->af_suff);
    hash_init(&aff->af_comp);

    /*
     * Read all the lines in the file one by one.
     */
    જ્યારે (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	લીટી_breakcheck();
	++lnum;

	// Skip comment lines.
	જો (*rline == '#')
	    ચાલુ;

	// Convert from "SET" to 'encoding' when needed.
	vim_free(pc);
	જો (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, ખાલી);
	    જો (pc == ખાલી)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
							   fname, lnum, rline);
		ચાલુ;
	    }
	    લીટી = pc;
	}
	નહીંતર
	{
	    pc = ખાલી;
	    લીટી = rline;
	}

	// Split the line up in white separated items.  Put a NUL after each
	// item.
	itemcnt = 0;
	માટે (p = લીટી; ; )
	{
	    જ્યારે (*p != NUL && *p <= ' ')  // skip white space and CR/NL
		++p;
	    જો (*p == NUL)
		તોડો;
	    જો (itemcnt == MAXITEMCNT)	    // too many items
		તોડો;
	    વસ્તુઓ[itemcnt++] = p;
	    // A few items have arbitrary text argument, don't split them.
	    જો (itemcnt == 2 && spell_info_item(વસ્તુઓ[0]))
		જ્યારે (*p >= ' ' || *p == TAB)    // skip until CR/NL
		    ++p;
	    નહીંતર
		જ્યારે (*p > ' ')    // skip until white space or CR/NL
		    ++p;
	    જો (*p == NUL)
		તોડો;
	    *p++ = NUL;
	}

	// Handle non-empty lines.
	જો (itemcnt > 0)
	{
	    જો (is_aff_rule(વસ્તુઓ, itemcnt, "SET", 2) && aff->af_enc == ખાલી)
	    {
		// Setup for conversion from "ENC" to 'encoding'.
		aff->af_enc = enc_canonize(વસ્તુઓ[1]);
		જો (aff->af_enc != ખાલી && !spin->si_ascii
			&& convert_setup(&spin->si_conv, aff->af_enc,
							       p_enc) == FAIL)
		    smsg(_("Conversion in %s not supported: from %s to %s"),
					       fname, aff->af_enc, p_enc);
		spin->si_conv.vc_fail = TRUE;
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "FLAG", 2)
					      && aff->af_flagtype == AFT_CHAR)
	    {
		જો (STRCMP(વસ્તુઓ[1], "long") == 0)
		    aff->af_flagtype = AFT_LONG;
		નહીંતર જો (STRCMP(વસ્તુઓ[1], "num") == 0)
		    aff->af_flagtype = AFT_અંક;
		નહીંતર જો (STRCMP(વસ્તુઓ[1], "caplong") == 0)
		    aff->af_flagtype = AFT_CAPLONG;
		નહીંતર
		    smsg(_("Invalid value for FLAG in %s line %d: %s"),
			    fname, lnum, વસ્તુઓ[1]);
		જો (aff->af_rare != 0
			|| aff->af_keepcase != 0
			|| aff->af_bad != 0
			|| aff->af_needaffix != 0
			|| aff->af_circumfix != 0
			|| aff->af_needcomp != 0
			|| aff->af_comproot != 0
			|| aff->af_nosuggest != 0
			|| compflags != ખાલી
			|| aff->af_suff.ht_used > 0
			|| aff->af_pref.ht_used > 0)
		    smsg(_("FLAG after using flags in %s line %d: %s"),
			    fname, lnum, વસ્તુઓ[1]);
	    }
	    નહીંતર જો (spell_info_item(વસ્તુઓ[0]))
	    {
		    p = (char_u *)getroom(spin,
			    (spin->si_info == ખાલી ? 0 : STRLEN(spin->si_info))
			    + STRLEN(વસ્તુઓ[0])
			    + STRLEN(વસ્તુઓ[1]) + 3, FALSE);
		    જો (p != ખાલી)
		    {
			જો (spin->si_info != ખાલી)
			{
			    STRCPY(p, spin->si_info);
			    STRCAT(p, "\n");
			}
			STRCAT(p, વસ્તુઓ[0]);
			STRCAT(p, " ");
			STRCAT(p, વસ્તુઓ[1]);
			spin->si_info = p;
		    }
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "MIDWORD", 2)
							   && midword == ખાલી)
	    {
		midword = getroom_save(spin, વસ્તુઓ[1]);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "TRY", 2))
	    {
		// ignored, we look in the tree for what chars may appear
	    }
	    // TODO: remove "RAR" later
	    નહીંતર જો ((is_aff_rule(વસ્તુઓ, itemcnt, "RAR", 2)
			|| is_aff_rule(વસ્તુઓ, itemcnt, "RARE", 2))
							 && aff->af_rare == 0)
	    {
		aff->af_rare = affitem2flag(aff->af_flagtype, વસ્તુઓ[1],
								 fname, lnum);
	    }
	    // TODO: remove "KEP" later
	    નહીંતર જો ((is_aff_rule(વસ્તુઓ, itemcnt, "KEP", 2)
			|| is_aff_rule(વસ્તુઓ, itemcnt, "KEEPCASE", 2))
						     && aff->af_keepcase == 0)
	    {
		aff->af_keepcase = affitem2flag(aff->af_flagtype, વસ્તુઓ[1],
								 fname, lnum);
	    }
	    નહીંતર જો ((is_aff_rule(વસ્તુઓ, itemcnt, "BAD", 2)
			|| is_aff_rule(વસ્તુઓ, itemcnt, "FORBIDDENWORD", 2))
							  && aff->af_bad == 0)
	    {
		aff->af_bad = affitem2flag(aff->af_flagtype, વસ્તુઓ[1],
								 fname, lnum);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "NEEDAFFIX", 2)
						    && aff->af_needaffix == 0)
	    {
		aff->af_needaffix = affitem2flag(aff->af_flagtype, વસ્તુઓ[1],
								 fname, lnum);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "CIRCUMFIX", 2)
						    && aff->af_circumfix == 0)
	    {
		aff->af_circumfix = affitem2flag(aff->af_flagtype, વસ્તુઓ[1],
								 fname, lnum);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "NOSUGGEST", 2)
						    && aff->af_nosuggest == 0)
	    {
		aff->af_nosuggest = affitem2flag(aff->af_flagtype, વસ્તુઓ[1],
								 fname, lnum);
	    }
	    નહીંતર જો ((is_aff_rule(વસ્તુઓ, itemcnt, "NEEDCOMPOUND", 2)
			|| is_aff_rule(વસ્તુઓ, itemcnt, "ONLYINCOMPOUND", 2))
						     && aff->af_needcomp == 0)
	    {
		aff->af_needcomp = affitem2flag(aff->af_flagtype, વસ્તુઓ[1],
								 fname, lnum);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "COMPOUNDROOT", 2)
						     && aff->af_comproot == 0)
	    {
		aff->af_comproot = affitem2flag(aff->af_flagtype, વસ્તુઓ[1],
								 fname, lnum);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "COMPOUNDFORBIDFLAG", 2)
						   && aff->af_compforbid == 0)
	    {
		aff->af_compforbid = affitem2flag(aff->af_flagtype, વસ્તુઓ[1],
								 fname, lnum);
		જો (aff->af_pref.ht_used > 0)
		    smsg(_("Defining COMPOUNDFORBIDFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "COMPOUNDPERMITFLAG", 2)
						   && aff->af_comppermit == 0)
	    {
		aff->af_comppermit = affitem2flag(aff->af_flagtype, વસ્તુઓ[1],
								 fname, lnum);
		જો (aff->af_pref.ht_used > 0)
		    smsg(_("Defining COMPOUNDPERMITFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "COMPOUNDFLAG", 2)
							 && compflags == ખાલી)
	    {
		// Turn flag "c" into COMPOUNDRULE compatible string "c+",
		// "Na" into "Na+", "1234" into "1234+".
		p = getroom(spin, STRLEN(વસ્તુઓ[1]) + 2, FALSE);
		જો (p != ખાલી)
		{
		    STRCPY(p, વસ્તુઓ[1]);
		    STRCAT(p, "+");
		    compflags = p;
		}
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "COMPOUNDRULES", 2))
	    {
		// We don't use the count, but do check that it's a number and
		// not COMPOUNDRULE mistyped.
		જો (atoi((અક્ષર *)વસ્તુઓ[1]) == 0)
		    smsg(_("Wrong COMPOUNDRULES value in %s line %d: %s"),
						       fname, lnum, વસ્તુઓ[1]);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "COMPOUNDRULE", 2))
	    {
		// Don't use the first rule if it is a number.
		જો (compflags != ખાલી || *skipdigits(વસ્તુઓ[1]) != NUL)
		{
		    // Concatenate this string to previously defined ones,
		    // using a slash to separate them.
		    l = (પૂર્ણાંક)STRLEN(વસ્તુઓ[1]) + 1;
		    જો (compflags != ખાલી)
			l += (પૂર્ણાંક)STRLEN(compflags) + 1;
		    p = getroom(spin, l, FALSE);
		    જો (p != ખાલી)
		    {
			જો (compflags != ખાલી)
			{
			    STRCPY(p, compflags);
			    STRCAT(p, "/");
			}
			STRCAT(p, વસ્તુઓ[1]);
			compflags = p;
		    }
		}
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "COMPOUNDWORDMAX", 2)
							      && compmax == 0)
	    {
		compmax = atoi((અક્ષર *)વસ્તુઓ[1]);
		જો (compmax == 0)
		    smsg(_("Wrong COMPOUNDWORDMAX value in %s line %d: %s"),
						       fname, lnum, વસ્તુઓ[1]);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "COMPOUNDMIN", 2)
							   && compminlen == 0)
	    {
		compminlen = atoi((અક્ષર *)વસ્તુઓ[1]);
		જો (compminlen == 0)
		    smsg(_("Wrong COMPOUNDMIN value in %s line %d: %s"),
						       fname, lnum, વસ્તુઓ[1]);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "COMPOUNDSYLMAX", 2)
							   && compsylmax == 0)
	    {
		compsylmax = atoi((અક્ષર *)વસ્તુઓ[1]);
		જો (compsylmax == 0)
		    smsg(_("Wrong COMPOUNDSYLMAX value in %s line %d: %s"),
						       fname, lnum, વસ્તુઓ[1]);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "CHECKCOMPOUNDDUP", 1))
	    {
		compoptions |= COMP_CHECKDUP;
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "CHECKCOMPOUNDREP", 1))
	    {
		compoptions |= COMP_CHECKREP;
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "CHECKCOMPOUNDCASE", 1))
	    {
		compoptions |= COMP_CHECKCASE;
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "CHECKCOMPOUNDTRIPLE", 1))
	    {
		compoptions |= COMP_CHECKTRIPLE;
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "CHECKCOMPOUNDPATTERN", 2))
	    {
		જો (atoi((અક્ષર *)વસ્તુઓ[1]) == 0)
		    smsg(_("Wrong CHECKCOMPOUNDPATTERN value in %s line %d: %s"),
						       fname, lnum, વસ્તુઓ[1]);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "CHECKCOMPOUNDPATTERN", 3))
	    {
		garray_T    *gap = &spin->si_comppat;
		પૂર્ણાંક	    i;

		// Only add the couple if it isn't already there.
		માટે (i = 0; i < gap->ga_len - 1; i += 2)
		    જો (STRCMP(((char_u **)(gap->ga_data))[i], વસ્તુઓ[1]) == 0
			    && STRCMP(((char_u **)(gap->ga_data))[i + 1],
							       વસ્તુઓ[2]) == 0)
			તોડો;
		જો (i >= gap->ga_len && ga_grow(gap, 2) == OK)
		{
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, વસ્તુઓ[1]);
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, વસ્તુઓ[2]);
		}
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "SYLLABLE", 2)
							  && syllable == ખાલી)
	    {
		syllable = getroom_save(spin, વસ્તુઓ[1]);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "NOBREAK", 1))
	    {
		spin->si_nobreak = TRUE;
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "NOSPLITSUGS", 1))
	    {
		spin->si_nosplitsugs = TRUE;
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "NOCOMPOUNDSUGS", 1))
	    {
		spin->si_nocompoundsugs = TRUE;
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "NOSUGFILE", 1))
	    {
		spin->si_nosugfile = TRUE;
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "PFXPOSTPONE", 1))
	    {
		aff->af_pfxpostpone = TRUE;
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "IGNOREEXTRA", 1))
	    {
		aff->af_ignoreextra = TRUE;
	    }
	    નહીંતર જો ((STRCMP(વસ્તુઓ[0], "PFX") == 0
					      || STRCMP(વસ્તુઓ[0], "SFX") == 0)
		    && aff_todo == 0
		    && itemcnt >= 4)
	    {
		પૂર્ણાંક	lasti = 4;
		char_u	ચાવી[AH_ચાવી_LEN];

		જો (*વસ્તુઓ[0] == 'P')
		    tp = &aff->af_pref;
		નહીંતર
		    tp = &aff->af_suff;

		// Myspell allows the same affix name to be used multiple
		// times.  The affix files that do this have an undocumented
		// "S" flag on all but the last block, thus we check for that
		// and store it in ah_follows.
		vim_strncpy(ચાવી, વસ્તુઓ[1], AH_ચાવી_LEN - 1);
		hi = hash_find(tp, ચાવી);
		જો (!HASHITEM_EMPTY(hi))
		{
		    cur_aff = HI2AH(hi);
		    જો (cur_aff->ah_combine != (*વસ્તુઓ[2] == 'Y'))
			smsg(_("Different combining flag in continued affix block in %s line %d: %s"),
						   fname, lnum, વસ્તુઓ[1]);
		    જો (!cur_aff->ah_follows)
			smsg(_("Duplicate affix in %s line %d: %s"),
						       fname, lnum, વસ્તુઓ[1]);
		}
		નહીંતર
		{
		    // New affix letter.
		    cur_aff = (affheader_T *)getroom(spin,
						   કદ(affheader_T), TRUE);
		    જો (cur_aff == ખાલી)
			તોડો;
		    cur_aff->ah_flag = affitem2flag(aff->af_flagtype, વસ્તુઓ[1],
								 fname, lnum);
		    જો (cur_aff->ah_flag == 0 || STRLEN(વસ્તુઓ[1]) >= AH_ચાવી_LEN)
			તોડો;
		    જો (cur_aff->ah_flag == aff->af_bad
			    || cur_aff->ah_flag == aff->af_rare
			    || cur_aff->ah_flag == aff->af_keepcase
			    || cur_aff->ah_flag == aff->af_needaffix
			    || cur_aff->ah_flag == aff->af_circumfix
			    || cur_aff->ah_flag == aff->af_nosuggest
			    || cur_aff->ah_flag == aff->af_needcomp
			    || cur_aff->ah_flag == aff->af_comproot)
			smsg(_("Affix also used for BAD/RARE/KEEPCASE/NEEDAFFIX/NEEDCOMPOUND/NOSUGGEST in %s line %d: %s"),
						       fname, lnum, વસ્તુઓ[1]);
		    STRCPY(cur_aff->ah_ચાવી, વસ્તુઓ[1]);
		    hash_add(tp, cur_aff->ah_ચાવી, "spelling");

		    cur_aff->ah_combine = (*વસ્તુઓ[2] == 'Y');
		}

		// Check for the "S" flag, which apparently means that another
		// block with the same affix name is following.
		જો (itemcnt > lasti && STRCMP(વસ્તુઓ[lasti], "S") == 0)
		{
		    ++lasti;
		    cur_aff->ah_follows = TRUE;
		}
		નહીંતર
		    cur_aff->ah_follows = FALSE;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter.
		જો (itemcnt > lasti && *વસ્તુઓ[lasti] != '#')
		    smsg(_(e_afftrailing), fname, lnum, વસ્તુઓ[lasti]);

		જો (STRCMP(વસ્તુઓ[2], "Y") != 0 && STRCMP(વસ્તુઓ[2], "N") != 0)
		    smsg(_("Expected Y or N in %s line %d: %s"),
						       fname, lnum, વસ્તુઓ[2]);

		જો (*વસ્તુઓ[0] == 'P' && aff->af_pfxpostpone)
		{
		    જો (cur_aff->ah_newID == 0)
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
		    નહીંતર
			// Did use the ID in a previous block.
			did_postpone_prefix = TRUE;
		}

		aff_todo = atoi((અક્ષર *)વસ્તુઓ[3]);
	    }
	    નહીંતર જો ((STRCMP(વસ્તુઓ[0], "PFX") == 0
					      || STRCMP(વસ્તુઓ[0], "SFX") == 0)
		    && aff_todo > 0
		    && STRCMP(cur_aff->ah_ચાવી, વસ્તુઓ[1]) == 0
		    && itemcnt >= 5)
	    {
		affentry_T	*aff_entry;
		પૂર્ણાંક		upper = FALSE;
		પૂર્ણાંક		lasti = 5;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter,
		// unless IGNOREEXTRA is used.  Hunspell uses a "-" item.
		જો (itemcnt > lasti
			&& !aff->af_ignoreextra
			&& *વસ્તુઓ[lasti] != '#'
			&& (STRCMP(વસ્તુઓ[lasti], "-") != 0
						     || itemcnt != lasti + 1))
		    smsg(_(e_afftrailing), fname, lnum, વસ્તુઓ[lasti]);

		// New item for an affix letter.
		--aff_todo;
		aff_entry = (affentry_T *)getroom(spin,
						    કદ(affentry_T), TRUE);
		જો (aff_entry == ખાલી)
		    તોડો;

		જો (STRCMP(વસ્તુઓ[2], "0") != 0)
		    aff_entry->ae_chop = getroom_save(spin, વસ્તુઓ[2]);
		જો (STRCMP(વસ્તુઓ[3], "0") != 0)
		{
		    aff_entry->ae_add = getroom_save(spin, વસ્તુઓ[3]);

		    // Recognize flags on the affix: abcd/XYZ
		    aff_entry->ae_flags = vim_strchr(aff_entry->ae_add, '/');
		    જો (aff_entry->ae_flags != ખાલી)
		    {
			*aff_entry->ae_flags++ = NUL;
			aff_process_flags(aff, aff_entry);
		    }
		}

		// Don't use an affix entry with non-ASCII characters when
		// "spin->si_ascii" is TRUE.
		જો (!spin->si_ascii || !(has_non_ascii(aff_entry->ae_chop)
					  || has_non_ascii(aff_entry->ae_add)))
		{
		    aff_entry->ae_next = cur_aff->ah_first;
		    cur_aff->ah_first = aff_entry;

		    જો (STRCMP(વસ્તુઓ[4], ".") != 0)
		    {
			char_u	buf[MAXLINELEN];

			aff_entry->ae_cond = getroom_save(spin, વસ્તુઓ[4]);
			// Note: this silently truncates the buffer, but this should
			// not happen in practice
			જો (*વસ્તુઓ[0] == 'P')
			    vim_snprintf((અક્ષર *)buf, કદ(buf), "^%s", વસ્તુઓ[4]);
			નહીંતર
			    vim_snprintf((અક્ષર *)buf, કદ(buf), "%s$", વસ્તુઓ[4]);
			aff_entry->ae_prog = vim_regcomp(buf,
					    RE_MAGIC + RE_STRING + RE_STRICT);
			જો (aff_entry->ae_prog == ખાલી)
			    smsg(_("Broken condition in %s line %d: %s"),
						       fname, lnum, વસ્તુઓ[4]);
		    }

		    // For postponed prefixes we need an entry in si_prefcond
		    // for the condition.  Use an existing one if possible.
		    // Can't be done for an affix with flags, ignoring
		    // COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG.
		    જો (*વસ્તુઓ[0] == 'P' && aff->af_pfxpostpone
					       && aff_entry->ae_flags == ખાલી)
		    {
			// When the chop string is one lower-case letter and
			// the add string ends in the upper-case letter we set
			// the "upper" flag, clear "ae_chop" and remove the
			// letters from "ae_add".  The condition must either
			// be empty or start with the same letter.
			જો (aff_entry->ae_chop != ખાલી
				&& aff_entry->ae_add != ખાલી
				&& aff_entry->ae_chop[(*mb_ptr2len)(
						   aff_entry->ae_chop)] == NUL)
			{
			    પૂર્ણાંક		c, c_up;

			    c = PTR2CHAR(aff_entry->ae_chop);
			    c_up = SPELL_TOUPPER(c);
			    જો (c_up != c
				    && (aff_entry->ae_cond == ખાલી
					|| PTR2CHAR(aff_entry->ae_cond) == c))
			    {
				p = aff_entry->ae_add
						  + STRLEN(aff_entry->ae_add);
				MB_PTR_BACK(aff_entry->ae_add, p);
				જો (PTR2CHAR(p) == c_up)
				{
				    upper = TRUE;
				    aff_entry->ae_chop = ખાલી;
				    *p = NUL;

				    // The condition is matched with the
				    // actual word, thus must check for the
				    // upper-case letter.
				    જો (aff_entry->ae_cond != ખાલી)
				    {
					char_u	buf[MAXLINELEN];

					જો (has_mbyte)
					{
					    onecap_copy(વસ્તુઓ[4], buf, TRUE);
					    aff_entry->ae_cond = getroom_save(
								   spin, buf);
					}
					નહીંતર
					    *aff_entry->ae_cond = c_up;
					જો (aff_entry->ae_cond != ખાલી)
					{
					    sprintf((અક્ષર *)buf, "^%s",
							  aff_entry->ae_cond);
					    vim_regfree(aff_entry->ae_prog);
					    aff_entry->ae_prog = vim_regcomp(
						    buf, RE_MAGIC + RE_STRING);
					}
				    }
				}
			    }
			}

			જો (aff_entry->ae_chop == ખાલી
					       && aff_entry->ae_flags == ખાલી)
			{
			    પૂર્ણાંક		idx;
			    char_u	**pp;
			    પૂર્ણાંક		n;

			    // Find a previously used condition.
			    માટે (idx = spin->si_prefcond.ga_len - 1; idx >= 0;
									--idx)
			    {
				p = ((char_u **)spin->si_prefcond.ga_data)[idx];
				જો (str_equal(p, aff_entry->ae_cond))
				    તોડો;
			    }
			    જો (idx < 0 && ga_grow(&spin->si_prefcond, 1) == OK)
			    {
				// Not found, add a new condition.
				idx = spin->si_prefcond.ga_len++;
				pp = ((char_u **)spin->si_prefcond.ga_data)
									+ idx;
				જો (aff_entry->ae_cond == ખાલી)
				    *pp = ખાલી;
				નહીંતર
				    *pp = getroom_save(spin,
							  aff_entry->ae_cond);
			    }

			    // Add the prefix to the prefix tree.
			    જો (aff_entry->ae_add == ખાલી)
				p = (char_u *)"";
			    નહીંતર
				p = aff_entry->ae_add;

			    // PFX_FLAGS is a negative number, so that
			    // tree_add_word() knows this is the prefix tree.
			    n = PFX_FLAGS;
			    જો (!cur_aff->ah_combine)
				n |= WFP_NC;
			    જો (upper)
				n |= WFP_UP;
			    જો (aff_entry->ae_comppermit)
				n |= WFP_COMPPERMIT;
			    જો (aff_entry->ae_compforbid)
				n |= WFP_COMPFORBID;
			    વૃક્ષ_add_word(spin, p, spin->si_prefroot, n,
						      idx, cur_aff->ah_newID);
			    did_postpone_prefix = TRUE;
			}

			// Didn't actually use ah_newID, backup si_newprefID.
			જો (aff_todo == 0 && !did_postpone_prefix)
			{
			    --spin->si_newprefID;
			    cur_aff->ah_newID = 0;
			}
		    }
		}
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "FOL", 2) && fol == ખાલી)
	    {
		fol = vim_strsave(વસ્તુઓ[1]);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "LOW", 2) && નીચલું == ખાલી)
	    {
		નીચલું = vim_strsave(વસ્તુઓ[1]);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "UPP", 2) && upp == ખાલી)
	    {
		upp = vim_strsave(વસ્તુઓ[1]);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "REP", 2)
		     || is_aff_rule(વસ્તુઓ, itemcnt, "REPSAL", 2))
	    {
		// Ignore REP/REPSAL count
		જો (!SAFE_isdigit(*વસ્તુઓ[1]))
		    smsg(_("Expected REP(SAL) count in %s line %d"),
								 fname, lnum);
	    }
	    નહીંતર જો ((STRCMP(વસ્તુઓ[0], "REP") == 0
			|| STRCMP(વસ્તુઓ[0], "REPSAL") == 0)
		    && itemcnt >= 3)
	    {
		// REP/REPSAL item
		// Myspell ignores extra arguments, we require it starts with
		// # to detect mistakes.
		જો (itemcnt > 3 && વસ્તુઓ[3][0] != '#')
		    smsg(_(e_afftrailing), fname, lnum, વસ્તુઓ[3]);
		જો (વસ્તુઓ[0][3] == 'S' ? do_repsal : do_rep)
		{
		    // Replace underscore with space (can't include a space
		    // directly).
		    માટે (p = વસ્તુઓ[1]; *p != NUL; MB_PTR_ADV(p))
			જો (*p == '_')
			    *p = ' ';
		    માટે (p = વસ્તુઓ[2]; *p != NUL; MB_PTR_ADV(p))
			જો (*p == '_')
			    *p = ' ';
		    add_fromto(spin, વસ્તુઓ[0][3] == 'S'
					 ? &spin->si_repsal
					 : &spin->si_rep, વસ્તુઓ[1], વસ્તુઓ[2]);
		}
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "MAP", 2))
	    {
		// MAP item or count
		જો (!found_map)
		{
		    // First line contains the count.
		    found_map = TRUE;
		    જો (!SAFE_isdigit(*વસ્તુઓ[1]))
			smsg(_("Expected MAP count in %s line %d"),
								 fname, lnum);
		}
		નહીંતર જો (do_mapline)
		{
		    પૂર્ણાંક		c;

		    // Check that every character appears only once.
		    માટે (p = વસ્તુઓ[1]; *p != NUL; )
		    {
			c = mb_ptr2char_adv(&p);
			જો ((spin->si_map.ga_len > 0
				    && vim_strchr(spin->si_map.ga_data, c)
								      != ખાલી)
				|| vim_strchr(p, c) != ખાલી)
			    smsg(_("Duplicate character in MAP in %s line %d"),
								 fname, lnum);
		    }

		    // We simply concatenate all the MAP strings, separated by
		    // slashes.
		    ga_concat(&spin->si_map, વસ્તુઓ[1]);
		    ga_append(&spin->si_map, '/');
		}
	    }
	    // Accept "SAL from to" and "SAL from to  #comment".
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "SAL", 3))
	    {
		જો (do_sal)
		{
		    // SAL item (sounds-a-like)
		    // Either one of the known keys or a from-to pair.
		    જો (STRCMP(વસ્તુઓ[1], "followup") == 0)
			spin->si_followup = sal_to_bool(વસ્તુઓ[2]);
		    નહીંતર જો (STRCMP(વસ્તુઓ[1], "collapse_result") == 0)
			spin->si_collapse = sal_to_bool(વસ્તુઓ[2]);
		    નહીંતર જો (STRCMP(વસ્તુઓ[1], "remove_accents") == 0)
			spin->si_rem_accents = sal_to_bool(વસ્તુઓ[2]);
		    નહીંતર
			// when "to" is "_" it means empty
			add_fromto(spin, &spin->si_sal, વસ્તુઓ[1],
				     STRCMP(વસ્તુઓ[2], "_") == 0 ? (char_u *)""
								: વસ્તુઓ[2]);
		}
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "SOFOFROM", 2)
							  && sofofrom == ખાલી)
	    {
		sofofrom = getroom_save(spin, વસ્તુઓ[1]);
	    }
	    નહીંતર જો (is_aff_rule(વસ્તુઓ, itemcnt, "SOFOTO", 2)
							    && sofoto == ખાલી)
	    {
		sofoto = getroom_save(spin, વસ્તુઓ[1]);
	    }
	    નહીંતર જો (STRCMP(વસ્તુઓ[0], "COMMON") == 0)
	    {
		પૂર્ણાંક	i;

		માટે (i = 1; i < itemcnt; ++i)
		{
		    જો (HASHITEM_EMPTY(hash_find(&spin->si_commonwords,
								   વસ્તુઓ[i])))
		    {
			p = vim_strsave(વસ્તુઓ[i]);
			જો (p == ખાલી)
			    તોડો;
			hash_add(&spin->si_commonwords, p, "spelling");
		    }
		}
	    }
	    નહીંતર
		smsg(_("Unrecognized or duplicate item in %s line %d: %s"),
						       fname, lnum, વસ્તુઓ[0]);
	}
    }

    જો (fol != ખાલી || નીચલું != ખાલી || upp != ખાલી)
    {
	જો (spin->si_clear_chartab)
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
	જો (!spin->si_ascii && !enc_utf8)
	{
	    જો (fol == ખાલી || નીચલું == ખાલી || upp == ખાલી)
		smsg(_("Missing FOL/LOW/UPP line in %s"), fname);
	    નહીંતર
		(શૂન્ય)set_spell_chartab(fol, નીચલું, upp);
	}

	vim_free(fol);
	vim_free(નીચલું);
	vim_free(upp);
    }

    // Use compound specifications of the .aff file for the spell info.
    જો (compmax != 0)
    {
	aff_check_સંખ્યા(spin->si_compmax, compmax, "COMPOUNDWORDMAX");
	spin->si_compmax = compmax;
    }

    જો (compminlen != 0)
    {
	aff_check_સંખ્યા(spin->si_compminlen, compminlen, "COMPOUNDMIN");
	spin->si_compminlen = compminlen;
    }

    જો (compsylmax != 0)
    {
	જો (syllable == ખાલી)
	    smsg(_("COMPOUNDSYLMAX used without SYLLABLE"));
	aff_check_સંખ્યા(spin->si_compsylmax, compsylmax, "COMPOUNDSYLMAX");
	spin->si_compsylmax = compsylmax;
    }

    જો (compoptions != 0)
    {
	aff_check_સંખ્યા(spin->si_compoptions, compoptions, "COMPOUND options");
	spin->si_compoptions |= compoptions;
    }

    જો (compflags != ખાલી)
	process_compflags(spin, aff, compflags);

    // Check that we didn't use too many renumbered flags.
    જો (spin->si_newcompID < spin->si_newprefID)
    {
	જો (spin->si_newcompID == 127 || spin->si_newcompID == 255)
	    સંદેશ(_("Too many postponed prefixes"));
	નહીંતર જો (spin->si_newprefID == 0 || spin->si_newprefID == 127)
	    સંદેશ(_("Too many compound flags"));
	નહીંતર
	    સંદેશ(_("Too many postponed prefixes and/or compound flags"));
    }

    જો (syllable != ખાલી)
    {
	aff_check_string(spin->si_syllable, syllable, "SYLLABLE");
	spin->si_syllable = syllable;
    }

    જો (sofofrom != ખાલી || sofoto != ખાલી)
    {
	જો (sofofrom == ખાલી || sofoto == ખાલી)
	    smsg(_("Missing SOFO%s line in %s"),
				     sofofrom == ખાલી ? "FROM" : "TO", fname);
	નહીંતર જો (spin->si_sal.ga_len > 0)
	    smsg(_("Both SAL and SOFO lines in %s"), fname);
	નહીંતર
	{
	    aff_check_string(spin->si_sofofr, sofofrom, "SOFOFROM");
	    aff_check_string(spin->si_sofoto, sofoto, "SOFOTO");
	    spin->si_sofofr = sofofrom;
	    spin->si_sofoto = sofoto;
	}
    }

    જો (midword != ખાલી)
    {
	aff_check_string(spin->si_midword, midword, "MIDWORD");
	spin->si_midword = midword;
    }

    vim_free(pc);
    fclose(fd);
    પાછા aff;
}

/*
 * Return TRUE when items[0] equals "rulename", there are "mincount" items or
 * a comment is following after item "mincount".
 */
    સ્થિર પૂર્ણાંક
is_aff_rule(
    char_u	**વસ્તુઓ,
    પૂર્ણાંક		itemcnt,
    અક્ષર	*rulename,
    પૂર્ણાંક		mincount)
{
    પાછા (STRCMP(વસ્તુઓ[0], rulename) == 0
	    && (itemcnt == mincount
		|| (itemcnt > mincount && વસ્તુઓ[mincount][0] == '#')));
}

/*
 * For affix "entry" move COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG from
 * ae_flags to ae_comppermit and ae_compforbid.
 */
    સ્થિર શૂન્ય
aff_process_flags(afffile_T *affile, affentry_T *entry)
{
    char_u	*p;
    char_u	*prevp;
    unsigned	flag;

    જો (entry->ae_flags != ખાલી
		&& (affile->af_compforbid != 0 || affile->af_comppermit != 0))
    {
	માટે (p = entry->ae_flags; *p != NUL; )
	{
	    prevp = p;
	    flag = get_affitem(affile->af_flagtype, &p);
	    જો (flag == affile->af_comppermit || flag == affile->af_compforbid)
	    {
		STRMOVE(prevp, p);
		p = prevp;
		જો (flag == affile->af_comppermit)
		    entry->ae_comppermit = TRUE;
		નહીંતર
		    entry->ae_compforbid = TRUE;
	    }
	    જો (affile->af_flagtype == AFT_અંક && *p == ',')
		++p;
	}
	જો (*entry->ae_flags == NUL)
	    entry->ae_flags = ખાલી;	// nothing left
    }
}

/*
 * Return TRUE if "s" is the name of an info item in the affix file.
 */
    સ્થિર પૂર્ણાંક
spell_info_item(char_u *s)
{
    પાછા STRCMP(s, "NAME") == 0
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
    સ્થિર unsigned
affitem2flag(
    પૂર્ણાંક		flagtype,
    char_u	*item,
    char_u	*fname,
    પૂર્ણાંક		lnum)
{
    unsigned	res;
    char_u	*p = item;

    res = get_affitem(flagtype, &p);
    જો (res == 0)
    {
	જો (flagtype == AFT_અંક)
	    smsg(_("Flag is not a number in %s line %d: %s"),
							   fname, lnum, item);
	નહીંતર
	    smsg(_("Illegal flag in %s line %d: %s"),
							   fname, lnum, item);
    }
    જો (*p != NUL)
    {
	smsg(_(e_affname), fname, lnum, item);
	પાછા 0;
    }

    પાછા res;
}

/*
 * Get one affix name from "*pp" and advance the pointer.
 * Returns ZERO_FLAG for "0".
 * Returns zero for an error, still advances the pointer then.
 */
    સ્થિર unsigned
get_affitem(પૂર્ણાંક flagtype, char_u **pp)
{
    પૂર્ણાંક		res;

    જો (flagtype == AFT_અંક)
    {
	જો (!VIM_ISDIGIT(**pp))
	{
	    ++*pp;	// always advance, avoid getting stuck
	    પાછા 0;
	}
	res = getdigits(pp);
	જો (res == 0)
	    res = ZERO_FLAG;
    }
    નહીંતર
    {
	res = mb_ptr2char_adv(pp);
	જો (flagtype == AFT_LONG || (flagtype == AFT_CAPLONG
						 && res >= 'A' && res <= 'Z'))
	{
	    જો (**pp == NUL)
		પાછા 0;
	    res = mb_ptr2char_adv(pp) + (res << 16);
	}
    }
    પાછા res;
}

/*
 * Process the "compflags" string used in an affix file and append it to
 * spin->si_compflags.
 * The processing involves changing the affix names to ID numbers, so that
 * they fit in one byte.
 */
    સ્થિર શૂન્ય
process_compflags(
    spellinfo_T	*spin,
    afffile_T	*aff,
    char_u	*compflags)
{
    char_u	*p;
    char_u	*prevp;
    unsigned	flag;
    compitem_T	*ci;
    પૂર્ણાંક		id;
    પૂર્ણાંક		len;
    char_u	*tp;
    char_u	ચાવી[AH_ચાવી_LEN];
    hashitem_T	*hi;

    // Make room for the old and the new compflags, concatenated with a / in
    // between.  Processing it makes it shorter, but we don't know by how
    // much, thus allocate the maximum.
    len = (પૂર્ણાંક)STRLEN(compflags) + 1;
    જો (spin->si_compflags != ખાલી)
	len += (પૂર્ણાંક)STRLEN(spin->si_compflags) + 1;
    p = getroom(spin, len, FALSE);
    જો (p == ખાલી)
	પાછા;
    જો (spin->si_compflags != ખાલી)
    {
	STRCPY(p, spin->si_compflags);
	STRCAT(p, "/");
    }
    spin->si_compflags = p;
    tp = p + STRLEN(p);

    માટે (p = compflags; *p != NUL; )
    {
	જો (vim_strchr((char_u *)"/?*+[]", *p) != ખાલી)
	    // Copy non-flag characters directly.
	    *tp++ = *p++;
	નહીંતર
	{
	    // First get the flag number, also checks validity.
	    prevp = p;
	    flag = get_affitem(aff->af_flagtype, &p);
	    જો (flag != 0)
	    {
		// Find the flag in the hashtable.  If it was used before, use
		// the existing ID.  Otherwise add a new entry.
		vim_strncpy(ચાવી, prevp, p - prevp);
		hi = hash_find(&aff->af_comp, ચાવી);
		જો (!HASHITEM_EMPTY(hi))
		    id = HI2CI(hi)->ci_newID;
		નહીંતર
		{
		    ci = (compitem_T *)getroom(spin, કદ(compitem_T), TRUE);
		    જો (ci == ખાલી)
			તોડો;
		    STRCPY(ci->ci_ચાવી, ચાવી);
		    ci->ci_flag = flag;
		    // Avoid using a flag ID that has a special meaning in a
		    // regexp (also inside []).
		    કરો
		    {
			check_renumber(spin);
			id = spin->si_newcompID--;
		    } જ્યારે (vim_strchr((char_u *)"/?*+[]\\-^", id) != ખાલી);
		    ci->ci_newID = id;
		    hash_add(&aff->af_comp, ci->ci_ચાવી, "spelling");
		}
		*tp++ = id;
	    }
	    જો (aff->af_flagtype == AFT_અંક && *p == ',')
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
    સ્થિર શૂન્ય
check_renumber(spellinfo_T *spin)
{
    જો (spin->si_newprefID == spin->si_newcompID && spin->si_newcompID < 128)
    {
	spin->si_newprefID = 127;
	spin->si_newcompID = 255;
    }
}

/*
 * Append one affix or compound ID to "store_afflist".
 * Returns FAIL when this would overrun the fixed-size buffer.
 */
    સ્થિર પૂર્ણાંક
store_afflist_add(
    char_u	*store_afflist,
    પૂર્ણાંક		*cntp,
    પૂર્ણાંક		id)
{
    જો (*cntp >= MAXWLEN - 1)
    {
	emsg(_(e_too_many_postponed_prefixes_spell));
	પાછા FAIL;
    }
    store_afflist[(*cntp)++] = id;
    store_afflist[*cntp] = NUL;
    પાછા OK;
}

/*
 * Return TRUE if flag "flag" appears in affix list "afflist".
 */
    સ્થિર પૂર્ણાંક
flag_in_afflist(પૂર્ણાંક flagtype, char_u *afflist, unsigned flag)
{
    char_u	*p;
    unsigned	n;

    પસંદગી (flagtype)
    {
	સ્થિતિ AFT_CHAR:
	    પાછા vim_strchr(afflist, flag) != ખાલી;

	સ્થિતિ AFT_CAPLONG:
	સ્થિતિ AFT_LONG:
	    માટે (p = afflist; *p != NUL; )
	    {
		n = mb_ptr2char_adv(&p);
		જો ((flagtype == AFT_LONG || (n >= 'A' && n <= 'Z'))
								 && *p != NUL)
		    n = mb_ptr2char_adv(&p) + (n << 16);
		જો (n == flag)
		    પાછા TRUE;
	    }
	    તોડો;

	સ્થિતિ AFT_અંક:
	    માટે (p = afflist; *p != NUL; )
	    {
		n = getdigits(&p);
		જો (n == 0)
		    n = ZERO_FLAG;
		જો (n == flag)
		    પાછા TRUE;
		જો (*p != NUL)	// skip over comma
		    ++p;
	    }
	    તોડો;
    }
    પાછા FALSE;
}

/*
 * Give a warning when "spinval" and "affval" numbers are set and not the same.
 */
    સ્થિર શૂન્ય
aff_check_સંખ્યા(પૂર્ણાંક spinval, પૂર્ણાંક affval, અક્ષર *name)
{
    જો (spinval != 0 && spinval != affval)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Give a warning when "spinval" and "affval" strings are set and not the same.
 */
    સ્થિર શૂન્ય
aff_check_string(char_u *spinval, char_u *affval, અક્ષર *name)
{
    જો (spinval != ખાલી && STRCMP(spinval, affval) != 0)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Return TRUE if strings "s1" and "s2" are equal.  Also consider both being
 * NULL as equal.
 */
    સ્થિર પૂર્ણાંક
str_equal(char_u *s1, char_u *s2)
{
    જો (s1 == ખાલી || s2 == ખાલી)
	પાછા s1 == s2;
    પાછા STRCMP(s1, s2) == 0;
}

/*
 * Add a from-to item to "gap".  Used for REP and SAL items.
 * They are stored case-folded.
 */
    સ્થિર શૂન્ય
add_fromto(
    spellinfo_T	*spin,
    garray_T	*gap,
    char_u	*from,
    char_u	*to)
{
    fromto_T	*ftp;
    char_u	word[MAXWLEN];

    જો (ga_grow(gap, 1) == FAIL)
	પાછા;

    ftp = ((fromto_T *)gap->ga_data) + gap->ga_len;
    (શૂન્ય)spell_casefold(curwin, from, (પૂર્ણાંક)STRLEN(from), word, MAXWLEN);
    ftp->ft_from = getroom_save(spin, word);
    (શૂન્ય)spell_casefold(curwin, to, (પૂર્ણાંક)STRLEN(to), word, MAXWLEN);
    ftp->ft_to = getroom_save(spin, word);
    ++gap->ga_len;
}

/*
 * Convert a boolean argument in a SAL line to TRUE or FALSE;
 */
    સ્થિર પૂર્ણાંક
sal_to_bool(char_u *s)
{
    પાછા STRCMP(s, "1") == 0 || STRCMP(s, "true") == 0;
}

/*
 * Free the structure filled by spell_read_aff().
 */
    સ્થિર શૂન્ય
spell_free_aff(afffile_T *aff)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    પૂર્ણાંક		todo;
    affheader_T	*ah;
    affentry_T	*ae;

    vim_free(aff->af_enc);

    // All this trouble to free the "ae_prog" items...
    માટે (ht = &aff->af_pref; ; ht = &aff->af_suff)
    {
	todo = (પૂર્ણાંક)ht->ht_used;
	FOR_ALL_HASHTAB_વસ્તુઓ(ht, hi, todo)
	{
	    જો (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		ah = HI2AH(hi);
		માટે (ae = ah->ah_first; ae != ખાલી; ae = ae->ae_next)
		    vim_regfree(ae->ae_prog);
	    }
	}
	જો (ht == &aff->af_suff)
	    તોડો;
    }

    hash_clear(&aff->af_pref);
    hash_clear(&aff->af_suff);
    hash_clear(&aff->af_comp);
}

/*
 * Read dictionary file "fname".
 * Returns OK or FAIL;
 */
    સ્થિર પૂર્ણાંક
spell_read_dic(spellinfo_T *spin, char_u *fname, afffile_T *affile)
{
    hashtab_T	ht;
    char_u	લીટી[MAXLINELEN];
    char_u	*p;
    char_u	*afflist;
    char_u	store_afflist[MAXWLEN];
    પૂર્ણાંક		pfxlen;
    પૂર્ણાંક		totlen;
    પૂર્ણાંક		need_affix;
    char_u	*dw;
    char_u	*pc;
    char_u	*w;
    પૂર્ણાંક		l;
    hash_T	hash;
    hashitem_T	*hi;
    FILE	*fd;
    પૂર્ણાંક		lnum = 1;
    પૂર્ણાંક		non_ascii = 0;
    પૂર્ણાંક		retval = OK;
    char_u	message[MAXLINELEN + MAXWLEN];
    પૂર્ણાંક		flags;
    પૂર્ણાંક		duplicate = 0;
    time_T	last_સંદેશ_time = 0;

    /*
     * Open the file.
     */
    fd = mch_fopen((અક્ષર *)fname, "r");
    જો (fd == ખાલી)
    {
	semsg(_(e_cant_open_file_str), fname);
	પાછા FAIL;
    }

    // The hashtable is only used to detect duplicated words.
    hash_init(&ht);

    vim_snprintf((અક્ષર *)IObuff, IOSIZE,
				  _("Reading dictionary file %s..."), fname);
    spell_message(spin, IObuff);

    // start with a message for the first line
    spin->si_સંદેશ_ગણતરી = 999999;

    // Read and ignore the first line: word count.
    જો (vim_fgets(લીટી, MAXLINELEN, fd) || !vim_isdigit(*skipwhite(લીટી)))
	semsg(_(e_no_word_ગણતરી_in_str), fname);

    /*
     * Read all the lines in the file one by one.
     * The words are converted to 'encoding' here, before being added to
     * the hashtable.
     */
    જ્યારે (!vim_fgets(લીટી, MAXLINELEN, fd) && !got_int)
    {
	લીટી_breakcheck();
	++lnum;
	જો (લીટી[0] == '#' || લીટી[0] == '/')
	    ચાલુ;	// comment line

	// Remove CR, LF and white space from the end.  White space halfway
	// the word is kept to allow e.g., "et al.".
	l = (પૂર્ણાંક)STRLEN(લીટી);
	જ્યારે (l > 0 && લીટી[l - 1] <= ' ')
	    --l;
	જો (l == 0)
	    ચાલુ;	// empty line
	લીટી[l] = NUL;

	// Convert from "SET" to 'encoding' when needed.
	જો (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, લીટી, ખાલી);
	    જો (pc == ખાલી)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
						       fname, lnum, લીટી);
		ચાલુ;
	    }
	    w = pc;
	}
	નહીંતર
	{
	    pc = ખાલી;
	    w = લીટી;
	}

	// Truncate the word at the "/", set "afflist" to what follows.
	// Replace "\/" by "/" and "\\" by "\".
	afflist = ખાલી;
	માટે (p = w; *p != NUL; MB_PTR_ADV(p))
	{
	    જો (*p == '\\' && (p[1] == '\\' || p[1] == '/'))
		STRMOVE(p, p + 1);
	    નહીંતર જો (*p == '/')
	    {
		*p = NUL;
		afflist = p + 1;
		તોડો;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	જો (spin->si_ascii && has_non_ascii(w))
	{
	    ++non_ascii;
	    vim_free(pc);
	    ચાલુ;
	}

	// This takes time, print a message every 10000 words, but not more
	// often than once per second.
	જો (spin->si_verbose && spin->si_સંદેશ_ગણતરી > 10000)
	{
	    spin->si_સંદેશ_ગણતરી = 0;
	    જો (vim_time() > last_સંદેશ_time)
	    {
		last_સંદેશ_time = vim_time();
		vim_snprintf((અક્ષર *)message, કદ(message),
			_("line %6d, word %6ld - %s"),
			   lnum, spin->si_foldwcount + spin->si_keepwcount, w);
		સંદેશ_start();
		સંદેશ_outtrans_long_attr(message, 0);
		સંદેશ_clr_eos();
		સંદેશ_didout = FALSE;
		સંદેશ_col = 0;
		out_flush();
	    }
	}

	// Store the word in the hashtable to be able to find duplicates.
	dw = getroom_save(spin, w);
	જો (dw == ખાલી)
	{
	    retval = FAIL;
	    vim_free(pc);
	    તોડો;
	}

	hash = hash_hash(dw);
	hi = hash_lookup(&ht, dw, hash);
	જો (!HASHITEM_EMPTY(hi))
	{
	    જો (p_verbose > 0)
		smsg(_("Duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    નહીંતર જો (duplicate == 0)
		smsg(_("First duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ++duplicate;
	}
	નહીંતર
	    hash_add_item(&ht, hi, dw, hash);

	flags = 0;
	store_afflist[0] = NUL;
	pfxlen = 0;
	totlen = 0;
	need_affix = FALSE;
	જો (afflist != ખાલી)
	{
	    // Extract flags from the affix list.
	    flags |= get_affix_flags(affile, afflist);

	    જો (affile->af_needaffix != 0 && flag_in_afflist(
			  affile->af_flagtype, afflist, affile->af_needaffix))
		need_affix = TRUE;

	    જો (affile->af_pfxpostpone)
	    {
		// Need to store the list of prefix IDs with the word.
		જો (get_pfxlist(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    તોડો;
		}
		pfxlen = totlen;
	    }

	    જો (spin->si_compflags != ખાલી)
	    {
		// Need to store the list of compound flags with the word.
		// Concatenate them to the list of prefix IDs.
		જો (get_compflags(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    તોડો;
		}
	    }
	}

	// Add the word to the word tree(s).
	જો (store_word(spin, dw, flags, spin->si_region,
					   store_afflist, need_affix) == FAIL)
	    retval = FAIL;

	જો (afflist != ખાલી)
	{
	    // Find all matching suffixes and add the resulting words.
	    // Additionally do matching prefixes that combine.
	    જો (store_aff_word(spin, dw, afflist, affile,
			   &affile->af_suff, &affile->af_pref,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;

	    // Find all matching prefixes and add the resulting words.
	    જો (store_aff_word(spin, dw, afflist, affile,
			  &affile->af_pref, ખાલી,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;
	}

	vim_free(pc);
    }

    જો (duplicate > 0)
	smsg(_("%d duplicate word(s) in %s"), duplicate, fname);
    જો (spin->si_ascii && non_ascii > 0)
	smsg(_("Ignored %d word(s) with non-ASCII characters in %s"),
							    non_ascii, fname);
    hash_clear(&ht);

    fclose(fd);
    પાછા retval;
}

/*
 * Check for affix flags in "afflist" that are turned into word flags.
 * Return WF_ flags.
 */
    સ્થિર પૂર્ણાંક
get_affix_flags(afffile_T *affile, char_u *afflist)
{
    પૂર્ણાંક		flags = 0;

    જો (affile->af_keepcase != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_keepcase))
	flags |= WF_KEEPCAP | WF_FIXCAP;
    જો (affile->af_rare != 0 && flag_in_afflist(
			       affile->af_flagtype, afflist, affile->af_rare))
	flags |= WF_RARE;
    જો (affile->af_bad != 0 && flag_in_afflist(
				affile->af_flagtype, afflist, affile->af_bad))
	flags |= WF_BANNED;
    જો (affile->af_needcomp != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_needcomp))
	flags |= WF_NEEDCOMP;
    જો (affile->af_comproot != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_comproot))
	flags |= WF_COMPROOT;
    જો (affile->af_nosuggest != 0 && flag_in_afflist(
			  affile->af_flagtype, afflist, affile->af_nosuggest))
	flags |= WF_NOSUGGEST;
    પાછા flags;
}

/*
 * Get the list of prefix IDs from the affix list "afflist".
 * Used for PFXPOSTPONE.
 * Put the resulting flags in "store_afflist[MAXWLEN]" with a terminating NUL.
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    સ્થિર પૂર્ણાંક
get_pfxlist(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    પૂર્ણાંક		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    પૂર્ણાંક		id;
    char_u	ચાવી[AH_ચાવી_LEN];
    hashitem_T	*hi;

    માટે (p = afflist; *p != NUL; )
    {
	prevp = p;
	જો (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a postponed prefix flag if it appears in "af_pref"
	    // and its ID is not zero.
	    vim_strncpy(ચાવી, prevp, p - prevp);
	    hi = hash_find(&affile->af_pref, ચાવી);
	    જો (!HASHITEM_EMPTY(hi))
	    {
		id = HI2AH(hi)->ah_newID;
		જો (id != 0 && store_afflist_add(store_afflist, cntp, id) == FAIL)
		    પાછા FAIL;
	    }
	}
	જો (affile->af_flagtype == AFT_અંક && *p == ',')
	    ++p;
    }

    પાછા OK;
}

/*
 * Get the list of compound IDs from the affix list "afflist" that are used
 * for compound words.
 * Puts the flags in "store_afflist[]".
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    સ્થિર પૂર્ણાંક
get_compflags(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    પૂર્ણાંક		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    char_u	ચાવી[AH_ચાવી_LEN];
    hashitem_T	*hi;

    માટે (p = afflist; *p != NUL; )
    {
	prevp = p;
	જો (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a compound flag if it appears in "af_comp".
	    vim_strncpy(ચાવી, prevp, p - prevp);
	    hi = hash_find(&affile->af_comp, ચાવી);
	    જો (!HASHITEM_EMPTY(hi)
		    && store_afflist_add(store_afflist, cntp,
					      HI2CI(hi)->ci_newID) == FAIL)
		પાછા FAIL;
	}
	જો (affile->af_flagtype == AFT_અંક && *p == ',')
	    ++p;
    }

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
store_aff_word(
    spellinfo_T	*spin,		// spell info
    char_u	*word,		// basic word start
    char_u	*afflist,	// list of names of supported affixes
    afffile_T	*affile,
    hashtab_T	*ht,
    hashtab_T	*xht,
    પૂર્ણાંક		condit,		// CONDIT_SUF et al.
    પૂર્ણાંક		flags,		// flags for the word
    char_u	*pfxlist,	// list of prefix IDs
    પૂર્ણાંક		pfxlen)		// nr of flags in "pfxlist" for prefixes, rest
				// is compound flags
{
    પૂર્ણાંક		todo;
    hashitem_T	*hi;
    affheader_T	*ah;
    affentry_T	*ae;
    char_u	newword[MAXWLEN];
    પૂર્ણાંક		retval = OK;
    પૂર્ણાંક		i, j;
    char_u	*p;
    પૂર્ણાંક		use_flags;
    char_u	*use_pfxlist;
    પૂર્ણાંક		use_pfxlen;
    પૂર્ણાંક		need_affix;
    char_u	store_afflist[MAXWLEN];
    char_u	pfx_pfxlist[MAXWLEN];
    size_t	wordlen = STRLEN(word);
    પૂર્ણાંક		use_condit;

    todo = (પૂર્ણાંક)ht->ht_used;
    માટે (hi = ht->ht_array; todo > 0 && retval == OK; ++hi)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ah = HI2AH(hi);

	    // Check that the affix combines, if required, and that the word
	    // supports this affix.
	    જો (((condit & CONDIT_COMB) == 0 || ah->ah_combine)
		    && flag_in_afflist(affile->af_flagtype, afflist,
								 ah->ah_flag))
	    {
		// Loop over all affix entries with this name.
		માટે (ae = ah->ah_first; ae != ખાલી; ae = ae->ae_next)
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
		    જો ((xht != ખાલી || !affile->af_pfxpostpone
				|| ae->ae_chop != ખાલી
				|| ae->ae_flags != ખાલી)
			    && (ae->ae_chop == ખાલી
				|| STRLEN(ae->ae_chop) < wordlen)
			    && (ae->ae_prog == ખાલી
				|| vim_regexec_prog(&ae->ae_prog, FALSE,
							    word, (colnr_T)0))
			    && (((condit & CONDIT_CFIX) == 0)
				== ((condit & CONDIT_AFF) == 0
				    || ae->ae_flags == ખાલી
				    || !flag_in_afflist(affile->af_flagtype,
					ae->ae_flags, affile->af_circumfix))))
		    {
			// Match.  Remove the chop and add the affix.
			જો (xht == ખાલી)
			{
			    // prefix: chop/add at the start of the word
			    જો (ae->ae_add == ખાલી)
				*newword = NUL;
			    નહીંતર
				vim_strncpy(newword, ae->ae_add, MAXWLEN - 1);
			    p = word;
			    જો (ae->ae_chop != ખાલી)
			    {
				// Skip chop string.
				જો (has_mbyte)
				{
				    i = mb_charlen(ae->ae_chop);
				    માટે ( ; i > 0; --i)
					MB_PTR_ADV(p);
				}
				નહીંતર
				    p += STRLEN(ae->ae_chop);
			    }
			    // Note: this silently truncates the buffer, but this should
			    // not happen in practice
			    STRNCAT(newword, p, MAXWLEN - STRLEN(newword) - 1);
			}
			નહીંતર
			{
			    // suffix: chop/add at the end of the word
			    vim_strncpy(newword, word, MAXWLEN - 1);
			    જો (ae->ae_chop != ખાલી)
			    {
				// Remove chop string.
				p = newword + STRLEN(newword);
				i = (પૂર્ણાંક)MB_CHARLEN(ae->ae_chop);
				માટે ( ; i > 0; --i)
				    MB_PTR_BACK(newword, p);
				*p = NUL;
			    }
			    જો (ae->ae_add != ખાલી)
				// Note: this silently truncates the buffer, but this should
				// not happen in practice
				STRNCAT(newword, ae->ae_add, MAXWLEN - STRLEN(newword) - 1);
			}

			use_flags = flags;
			use_pfxlist = pfxlist;
			use_pfxlen = pfxlen;
			need_affix = FALSE;
			use_condit = condit | CONDIT_COMB | CONDIT_AFF;
			જો (ae->ae_flags != ખાલી)
			{
			    // Extract flags from the affix list.
			    use_flags |= get_affix_flags(affile, ae->ae_flags);

			    જો (affile->af_needaffix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_needaffix))
				need_affix = TRUE;

			    // When there is a CIRCUMFIX flag the other affix
			    // must also have it and we don't add the word
			    // with one affix.
			    જો (affile->af_circumfix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_circumfix))
			    {
				use_condit |= CONDIT_CFIX;
				જો ((condit & CONDIT_CFIX) == 0)
				    need_affix = TRUE;
			    }

			    જો (affile->af_pfxpostpone
						|| spin->si_compflags != ખાલી)
			    {
				પૂર્ણાંક listlen = 0;

				જો (affile->af_pfxpostpone)
				{
				    // Get prefix IDS from the affix list.
				    જો (get_pfxlist(affile, ae->ae_flags,
						    store_afflist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					તોડો;
				    }
				    use_pfxlen = listlen;
				}
				નહીંતર
				    use_pfxlen = 0;
				use_pfxlist = store_afflist;

				// Combine the prefix IDs. Avoid adding the
				// same ID twice.
				માટે (i = 0; i < pfxlen; ++i)
				{
				    માટે (j = 0; j < use_pfxlen; ++j)
					જો (pfxlist[i] == use_pfxlist[j])
					    તોડો;
				    જો (j == use_pfxlen
					    && store_afflist_add(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					તોડો;
				    }
				    use_pfxlen = listlen;
				}
				જો (retval == FAIL)
				    તોડો;

				જો (spin->si_compflags != ખાલી)
				    // Get compound IDS from the affix list.
				    જો (get_compflags(affile, ae->ae_flags,
						      use_pfxlist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					તોડો;
				    }
				જો (retval == FAIL)
				    તોડો;

				// Combine the list of compound flags.
				// Concatenate them to the prefix IDs list.
				// Avoid adding the same ID twice.
				માટે (i = pfxlen; pfxlist[i] != NUL; ++i)
				{
				    માટે (j = use_pfxlen;
						   use_pfxlist[j] != NUL; ++j)
					જો (pfxlist[i] == use_pfxlist[j])
					    તોડો;
				    જો (use_pfxlist[j] == NUL
					    && store_afflist_add(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					તોડો;
				    }
				}
				જો (retval == FAIL)
				    તોડો;
			    }
			}

			// Obey a "COMPOUNDFORBIDFLAG" of the affix: don't
			// use the compound flags.
			જો (use_pfxlist != ખાલી && ae->ae_compforbid)
			{
			    vim_strncpy(pfx_pfxlist, use_pfxlist, use_pfxlen);
			    use_pfxlist = pfx_pfxlist;
			}

			// When there are postponed prefixes...
			જો (spin->si_prefroot != ખાલી
				&& spin->si_prefroot->wn_sibling != ખાલી)
			{
			    // ... add a flag to indicate an affix was used.
			    use_flags |= WF_HAS_AFF;

			    // ... don't use a prefix list if combining
			    // affixes is not allowed.  But do use the
			    // compound flags after them.
			    જો (!ah->ah_combine && use_pfxlist != ખાલી)
				use_pfxlist += use_pfxlen;
			}

			// When compounding is supported and there is no
			// "COMPOUNDPERMITFLAG" then forbid compounding on the
			// side where the affix is applied.
			જો (spin->si_compflags != ખાલી && !ae->ae_comppermit)
			{
			    જો (xht != ખાલી)
				use_flags |= WF_NOCOMPAFT;
			    નહીંતર
				use_flags |= WF_NOCOMPBEF;
			}

			// Store the modified word.
			જો (store_word(spin, newword, use_flags,
						 spin->si_region, use_pfxlist,
							  need_affix) == FAIL)
			    retval = FAIL;

			// When added a prefix or a first suffix and the affix
			// has flags may add a(nother) suffix.  RECURSIVE!
			જો ((condit & CONDIT_SUF) && ae->ae_flags != ખાલી)
			    જો (store_aff_word(spin, newword, ae->ae_flags,
					affile, &affile->af_suff, xht,
					   use_condit & (xht == ખાલી
							? ~0 :  ~CONDIT_SUF),
				      use_flags, use_pfxlist, pfxlen) == FAIL)
				retval = FAIL;

			// When added a suffix and combining is allowed also
			// try adding a prefix additionally.  Both for the
			// word flags and for the affix flags.  RECURSIVE!
			જો (xht != ખાલી && ah->ah_combine)
			{
			    જો (store_aff_word(spin, newword,
					afflist, affile,
					xht, ખાલી, use_condit,
					use_flags, use_pfxlist,
					pfxlen) == FAIL
				    || (ae->ae_flags != ખાલી
					&& store_aff_word(spin, newword,
					    ae->ae_flags, affile,
					    xht, ખાલી, use_condit,
					    use_flags, use_pfxlist,
					    pfxlen) == FAIL))
				retval = FAIL;
			}
		    }
		}
	    }
	}
    }

    પાછા retval;
}

/*
 * Read a file with a list of words.
 */
    સ્થિર પૂર્ણાંક
spell_read_wordfile(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    long	lnum = 0;
    char_u	rline[MAXLINELEN];
    char_u	*લીટી;
    char_u	*pc = ખાલી;
    char_u	*p;
    પૂર્ણાંક		l;
    પૂર્ણાંક		retval = OK;
    પૂર્ણાંક		did_word = FALSE;
    પૂર્ણાંક		non_ascii = 0;
    પૂર્ણાંક		flags;
    પૂર્ણાંક		regionmask;

    /*
     * Open the file.
     */
    fd = mch_fopen((અક્ષર *)fname, "r");
    જો (fd == ખાલી)
    {
	semsg(_(e_cant_open_file_str), fname);
	પાછા FAIL;
    }

    vim_snprintf((અક્ષર *)IObuff, IOSIZE, _("Reading word file %s..."), fname);
    spell_message(spin, IObuff);

    /*
     * Read all the lines in the file one by one.
     */
    જ્યારે (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	લીટી_breakcheck();
	++lnum;

	// Skip comment lines.
	જો (*rline == '#')
	    ચાલુ;

	// Remove CR, LF and white space from the end.
	l = (પૂર્ણાંક)STRLEN(rline);
	જ્યારે (l > 0 && rline[l - 1] <= ' ')
	    --l;
	જો (l == 0)
	    ચાલુ;	// empty or blank line
	rline[l] = NUL;

	// Convert from "/encoding={encoding}" to 'encoding' when needed.
	vim_free(pc);
	જો (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, ખાલી);
	    જો (pc == ખાલી)
	    {
		smsg(_("Conversion failure for word in %s line %ld: %s"),
							   fname, lnum, rline);
		ચાલુ;
	    }
	    લીટી = pc;
	}
	નહીંતર
	{
	    pc = ખાલી;
	    લીટી = rline;
	}

	જો (*લીટી == '/')
	{
	    ++લીટી;
	    જો (STRNCMP(લીટી, "encoding=", 9) == 0)
	    {
		જો (spin->si_conv.vc_type != CONV_NONE)
		    smsg(_("Duplicate /encoding= line ignored in %s line %ld: %s"),
						       fname, lnum, લીટી - 1);
		નહીંતર જો (did_word)
		    smsg(_("/encoding= line after word ignored in %s line %ld: %s"),
						       fname, lnum, લીટી - 1);
		નહીંતર
		{
		    char_u	*enc;

		    // Setup for conversion to 'encoding'.
		    લીટી += 9;
		    enc = enc_canonize(લીટી);
		    જો (enc != ખાલી && !spin->si_ascii
			    && convert_setup(&spin->si_conv, enc,
							       p_enc) == FAIL)
			smsg(_("Conversion in %s not supported: from %s to %s"),
							  fname, લીટી, p_enc);
		    vim_free(enc);
		    spin->si_conv.vc_fail = TRUE;
		}
		ચાલુ;
	    }

	    જો (STRNCMP(લીટી, "regions=", 8) == 0)
	    {
		જો (spin->si_region_ગણતરી > 1)
		    smsg(_("Duplicate /regions= line ignored in %s line %ld: %s"),
						       fname, lnum, લીટી);
		નહીંતર
		{
		    લીટી += 8;
		    જો (STRLEN(લીટી) > MAXREGIONS * 2)
			smsg(_("Too many regions in %s line %ld: %s"),
						       fname, lnum, લીટી);
		    નહીંતર
		    {
			spin->si_region_ગણતરી = (પૂર્ણાંક)STRLEN(લીટી) / 2;
			STRCPY(spin->si_region_name, લીટી);

			// Adjust the mask for a word valid in all regions.
			spin->si_region = (1 << spin->si_region_ગણતરી) - 1;
		    }
		}
		ચાલુ;
	    }

	    smsg(_("/ line ignored in %s line %ld: %s"),
						       fname, lnum, લીટી - 1);
	    ચાલુ;
	}

	flags = 0;
	regionmask = spin->si_region;

	// Check for flags and region after a slash.
	p = vim_strchr(લીટી, '/');
	જો (p != ખાલી)
	{
	    *p++ = NUL;
	    જ્યારે (*p != NUL)
	    {
		જો (*p == '=')		// keep-case word
		    flags |= WF_KEEPCAP | WF_FIXCAP;
		નહીંતર જો (*p == '!')	// Bad, bad, wicked word.
		    flags |= WF_BANNED;
		નહીંતર જો (*p == '?')	// Rare word.
		    flags |= WF_RARE;
		નહીંતર જો (VIM_ISDIGIT(*p)) // region number(s)
		{
		    જો ((flags & WF_REGION) == 0)   // first one
			regionmask = 0;
		    flags |= WF_REGION;

		    l = *p - '0';
		    જો (l == 0 || l > spin->si_region_ગણતરી)
		    {
			smsg(_("Invalid region nr in %s line %ld: %s"),
							  fname, lnum, p);
			તોડો;
		    }
		    regionmask |= 1 << (l - 1);
		}
		નહીંતર
		{
		    smsg(_("Unrecognized flags in %s line %ld: %s"),
							      fname, lnum, p);
		    તોડો;
		}
		++p;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	જો (spin->si_ascii && has_non_ascii(લીટી))
	{
	    ++non_ascii;
	    ચાલુ;
	}

	// Normal word: store it.
	જો (store_word(spin, લીટી, flags, regionmask, ખાલી, FALSE) == FAIL)
	{
	    retval = FAIL;
	    તોડો;
	}
	did_word = TRUE;
    }

    vim_free(pc);
    fclose(fd);

    જો (spin->si_ascii && non_ascii > 0)
    {
	vim_snprintf((અક્ષર *)IObuff, IOSIZE,
		  _("Ignored %d words with non-ASCII characters"), non_ascii);
	spell_message(spin, IObuff);
    }

    પાછા retval;
}

/*
 * Get part of an sblock_T, "len" bytes long.
 * This avoids calling free() for every little struct we use (and keeping
 * track of them).
 * The memory is cleared to all zeros.
 * Returns NULL when out of memory.
 */
    સ્થિર શૂન્ય *
getroom(
    spellinfo_T *spin,
    size_t	len,		// length needed
    પૂર્ણાંક		align)		// align for pointer
{
    char_u	*p;
    sblock_T	*bl = spin->si_blocks;

    જો (align && bl != ખાલી)
	// Round size up for alignment.  On some systems structures need to be
	// aligned to the size of a pointer (e.g., SPARC).
	bl->sb_used = (bl->sb_used + કદ(અક્ષર *) - 1)
						      & ~(કદ(અક્ષર *) - 1);

    જો (bl == ખાલી || bl->sb_used + len > SBLOCKSIZE)
    {
	જો (len >= SBLOCKSIZE)
	    bl = ખાલી;
	નહીંતર
	    // Allocate a block of memory. It is not freed until much later.
	    bl = alloc_clear(offsetof(sblock_T, sb_data) + SBLOCKSIZE + 1);
	જો (bl == ખાલી)
	{
	    જો (!spin->si_did_emsg)
	    {
		emsg(_(e_insufficient_memory_word_list_will_be_incomplete));
		spin->si_did_emsg = TRUE;
	    }
	    પાછા ખાલી;
	}
	bl->sb_next = spin->si_blocks;
	spin->si_blocks = bl;
	bl->sb_used = 0;
	++spin->si_blocks_cnt;
    }

    p = bl->sb_data + bl->sb_used;
    bl->sb_used += (પૂર્ણાંક)len;

    પાછા p;
}

/*
 * Make a copy of a string into memory allocated with getroom().
 * Returns NULL when out of memory.
 */
    સ્થિર char_u *
getroom_save(spellinfo_T *spin, char_u *s)
{
    char_u	*sc;

    sc = (char_u *)getroom(spin, STRLEN(s) + 1, FALSE);
    જો (sc != ખાલી)
	STRCPY(sc, s);
    પાછા sc;
}


/*
 * Free the list of allocated sblock_T.
 */
    સ્થિર શૂન્ય
free_blocks(sblock_T *bl)
{
    sblock_T	*next;

    જ્યારે (bl != ખાલી)
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
    સ્થિર wordnode_T *
wordtree_alloc(spellinfo_T *spin)
{
    પાછા (wordnode_T *)getroom(spin, કદ(wordnode_T), TRUE);
}

/*
 * Return TRUE if "word" contains valid word characters.
 * Control characters and trailing '/' are invalid.  Space is OK.
 */
    સ્થિર પૂર્ણાંક
valid_spell_word(char_u *word, char_u *end)
{
    char_u *p;

    જો (enc_utf8 && !utf_valid_string(word, end))
	પાછા FALSE;
    માટે (p = word; *p != NUL && p < end; p += mb_ptr2len(p))
	જો (*p < ' ' || (p[0] == '/' && p[1] == NUL))
	    પાછા FALSE;
    પાછા TRUE;
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
    સ્થિર પૂર્ણાંક
store_word(
    spellinfo_T	*spin,
    char_u	*word,
    પૂર્ણાંક		flags,		// extra flags, WF_BANNED
    પૂર્ણાંક		region,		// supported region(s)
    char_u	*pfxlist,	// list of prefix IDs or NULL
    પૂર્ણાંક		need_affix)	// only store word with affix ID
{
    પૂર્ણાંક		len = (પૂર્ણાંક)STRLEN(word);
    પૂર્ણાંક		ct = captype(word, word + len);
    char_u	foldword[MAXWLEN];
    પૂર્ણાંક		res = OK;
    char_u	*p;

    // Avoid adding illegal bytes to the word tree.
    જો (!valid_spell_word(word, word + len))
	પાછા FAIL;

    (શૂન્ય)spell_casefold(curwin, word, len, foldword, MAXWLEN);
    માટે (p = pfxlist; res == OK; ++p)
    {
	જો (!need_affix || (p != ખાલી && *p != NUL))
	    res = વૃક્ષ_add_word(spin, foldword, spin->si_foldroot, ct | flags,
						  region, p == ખાલી ? 0 : *p);
	જો (p == ખાલી || *p == NUL)
	    તોડો;
    }
    ++spin->si_foldwcount;

    જો (res == OK && (ct == WF_KEEPCAP || (flags & WF_KEEPCAP)))
    {
	માટે (p = pfxlist; res == OK; ++p)
	{
	    જો (!need_affix || (p != ખાલી && *p != NUL))
		res = વૃક્ષ_add_word(spin, word, spin->si_keeproot, flags,
						  region, p == ખાલી ? 0 : *p);
	    જો (p == ખાલી || *p == NUL)
		તોડો;
	}
	++spin->si_keepwcount;
    }
    પાછા res;
}

/*
 * Add word "word" to a word tree at "root".
 * When "flags" < 0 we are adding to the prefix tree where "flags" is used for
 * "rare" and "region" is the condition nr.
 * Returns FAIL when out of memory.
 */
    સ્થિર પૂર્ણાંક
વૃક્ષ_add_word(
    spellinfo_T	*spin,
    char_u	*word,
    wordnode_T	*મૂળ,
    પૂર્ણાંક		flags,
    પૂર્ણાંક		region,
    પૂર્ણાંક		affixID)
{
    wordnode_T	*ગાંઠ = મૂળ;
    wordnode_T	*np;
    wordnode_T	*copyp, **copyprev;
    wordnode_T	**prev = ખાલી;
    પૂર્ણાંક		i;

    // Add each byte of the word to the tree, including the NUL at the end.
    માટે (i = 0; ; ++i)
    {
	// When there is more than one reference to this node we need to make
	// a copy, so that we can modify it.  Copy the whole list of siblings
	// (we don't optimize for a partly shared list of siblings).
	જો (ગાંઠ != ખાલી && ગાંઠ->wn_refs > 1)
	{
	    --ગાંઠ->wn_refs;
	    copyprev = prev;
	    FOR_ALL_ગાંઠ_SIBLINGS(ગાંઠ, copyp)
	    {
		// Allocate a new node and copy the info.
		np = get_wordnode(spin);
		જો (np == ખાલી)
		    પાછા FAIL;
		np->wn_child = copyp->wn_child;
		જો (np->wn_child != ખાલી)
		    ++np->wn_child->wn_refs;	// child gets extra ref
		np->wn_byte = copyp->wn_byte;
		જો (np->wn_byte == NUL)
		{
		    np->wn_flags = copyp->wn_flags;
		    np->wn_region = copyp->wn_region;
		    np->wn_affixID = copyp->wn_affixID;
		}

		// Link the new node in the list, there will be one ref.
		np->wn_refs = 1;
		જો (copyprev != ખાલી)
		    *copyprev = np;
		copyprev = &np->wn_sibling;

		// Let "node" point to the head of the copied list.
		જો (copyp == ગાંઠ)
		    ગાંઠ = np;
	    }
	}

	// Look for the sibling that has the same character.  They are sorted
	// on byte value, thus stop searching when a sibling is found with a
	// higher byte value.  For zero bytes (end of word) the sorting is
	// done on flags and then on affixID.
	જ્યારે (ગાંઠ != ખાલી
		&& (ગાંઠ->wn_byte < word[i]
		    || (ગાંઠ->wn_byte == NUL
			&& (flags < 0
			    ? ગાંઠ->wn_affixID < (unsigned)affixID
			    : (ગાંઠ->wn_flags < (unsigned)(flags & WN_MASK)
				|| (ગાંઠ->wn_flags == (flags & WN_MASK)
				    && (spin->si_sugtree
					? (ગાંઠ->wn_region & 0xffff) < region
					: ગાંઠ->wn_affixID
						    < (unsigned)affixID)))))))
	{
	    prev = &ગાંઠ->wn_sibling;
	    ગાંઠ = *prev;
	}
	જો (ગાંઠ == ખાલી
		|| ગાંઠ->wn_byte != word[i]
		|| (word[i] == NUL
		    && (flags < 0
			|| spin->si_sugtree
			|| ગાંઠ->wn_flags != (flags & WN_MASK)
			|| ગાંઠ->wn_affixID != affixID)))
	{
	    // Allocate a new node.
	    np = get_wordnode(spin);
	    જો (np == ખાલી)
		પાછા FAIL;
	    np->wn_byte = word[i];

	    // If "node" is NULL this is a new child or the end of the sibling
	    // list: ref count is one.  Otherwise use ref count of sibling and
	    // make ref count of sibling one (matters when inserting in front
	    // of the list of siblings).
	    જો (ગાંઠ == ખાલી)
		np->wn_refs = 1;
	    નહીંતર
	    {
		np->wn_refs = ગાંઠ->wn_refs;
		ગાંઠ->wn_refs = 1;
	    }
	    જો (prev != ખાલી)
		*prev = np;
	    np->wn_sibling = ગાંઠ;
	    ગાંઠ = np;
	}

	જો (word[i] == NUL)
	{
	    ગાંઠ->wn_flags = flags;
	    ગાંઠ->wn_region |= region;
	    ગાંઠ->wn_affixID = affixID;
	    તોડો;
	}
	prev = &ગાંઠ->wn_child;
	ગાંઠ = *prev;
    }
#ifdef SPELL_PRINTTREE
    smsg("Added \"%s\"", word);
    spell_print_વૃક્ષ(મૂળ->wn_sibling);
#endif

    // count nr of words added since last message
    ++spin->si_સંદેશ_ગણતરી;

    જો (spin->si_compress_cnt > 1)
    {
	જો (--spin->si_compress_cnt == 1)
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
    જો (spin->si_compress_cnt == 1
	    ? spin->si_free_ગણતરી < MAXWLEN
	    : spin->si_blocks_cnt >= compress_start)
#endif
    {
	// Decrement the block counter.  The effect is that we compress again
	// when the freed up room has been used and another "compress_inc"
	// blocks have been allocated.  Unless "compress_added" words have
	// been added, then the limit is put back again.
	spin->si_blocks_cnt -= compress_inc;
	spin->si_compress_cnt = compress_added;

	જો (spin->si_verbose)
	{
	    સંદેશ_start();
	    સંદેશ_puts(_(સંદેશ_compressing));
	    સંદેશ_clr_eos();
	    સંદેશ_didout = FALSE;
	    સંદેશ_col = 0;
	    out_flush();
	}

	// Compress both trees.  Either they both have many nodes, which makes
	// compression useful, or one of them is small, which means
	// compression goes fast.  But when filling the soundfold word tree
	// there is no keep-case tree.
	wordtree_compress(spin, spin->si_foldroot, "case-folded");
	જો (affixID >= 0)
	    wordtree_compress(spin, spin->si_keeproot, "keep-case");
    }

    પાછા OK;
}

/*
 * Get a wordnode_T, either from the list of previously freed nodes or
 * allocate a new one.
 * Returns NULL when out of memory.
 */
    સ્થિર wordnode_T *
get_wordnode(spellinfo_T *spin)
{
    wordnode_T *n;

    જો (spin->si_first_free == ખાલી)
	n = (wordnode_T *)getroom(spin, કદ(wordnode_T), TRUE);
    નહીંતર
    {
	n = spin->si_first_free;
	spin->si_first_free = n->wn_child;
	CLEAR_POINTER(n);
	--spin->si_free_ગણતરી;
    }
#ifdef SPELL_PRINTTREE
    જો (n != ખાલી)
	n->wn_nr = ++spin->si_wordnode_nr;
#endif
    પાછા n;
}

/*
 * Decrement the reference count on a node (which is the head of a list of
 * siblings).  If the reference count becomes zero free the node and its
 * siblings.
 * Returns the number of nodes actually freed.
 */
    સ્થિર પૂર્ણાંક
deref_wordnode(spellinfo_T *spin, wordnode_T *ગાંઠ)
{
    wordnode_T	*np;
    પૂર્ણાંક		cnt = 0;

    જો (--ગાંઠ->wn_refs == 0)
    {
	FOR_ALL_ગાંઠ_SIBLINGS(ગાંઠ, np)
	{
	    જો (np->wn_child != ખાલી)
		cnt += deref_wordnode(spin, np->wn_child);
	    free_wordnode(spin, np);
	    ++cnt;
	}
	++cnt;	    // length field
    }
    પાછા cnt;
}

/*
 * Free a wordnode_T for re-use later.
 * Only the "wn_child" field becomes invalid.
 */
    સ્થિર શૂન્ય
free_wordnode(spellinfo_T *spin, wordnode_T *n)
{
    n->wn_child = spin->si_first_free;
    spin->si_first_free = n;
    ++spin->si_free_ગણતરી;
}

/*
 * Compress a tree: find tails that are identical and can be shared.
 */
    સ્થિર શૂન્ય
wordtree_compress(spellinfo_T *spin, wordnode_T *મૂળ, અક્ષર *name)
{
    hashtab_T	    ht;
    long	    n;
    long	    tot = 0;
    long	    perc;

    // Skip the root itself, it's not actually used.  The first sibling is the
    // start of the tree.
    જો (મૂળ->wn_sibling == ખાલી)
	પાછા;

    hash_init(&ht);
    n = ગાંઠ_compress(spin, મૂળ->wn_sibling, &ht, &tot);

#ifndef SPELL_PRINTTREE
    જો (spin->si_verbose || p_verbose > 2)
#endif
    {
	જો (tot > 1000000)
	    perc = (tot - n) / (tot / 100);
	નહીંતર જો (tot == 0)
	    perc = 0;
	નહીંતર
	    perc = (tot - n) * 100 / tot;
	vim_snprintf((અક્ષર *)IObuff, IOSIZE,
		_("Compressed %s: %ld of %ld nodes; %ld (%ld%%) remaining"),
		name, n, tot, tot - n, perc);
	spell_message(spin, IObuff);
    }
#ifdef SPELL_PRINTTREE
    spell_print_વૃક્ષ(મૂળ->wn_sibling);
#endif
    hash_clear(&ht);
}

/*
 * Compress a node, its siblings and its children, depth first.
 * Returns the number of compressed nodes.
 */
    સ્થિર long
ગાંઠ_compress(
    spellinfo_T	*spin,
    wordnode_T	*ગાંઠ,
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
    માટે (np = ગાંઠ; np != ખાલી && !got_int; np = np->wn_sibling)
    {
	++len;
	જો ((child = np->wn_child) != ખાલી)
	{
	    // Compress the child first.  This fills hashkey.
	    compressed += ગાંઠ_compress(spin, child, ht, tot);

	    // Try to find an identical child.
	    hash = hash_hash(child->wn_u1.hashkey);
	    hi = hash_lookup(ht, child->wn_u1.hashkey, hash);
	    જો (!HASHITEM_EMPTY(hi))
	    {
		// There are children we encountered before with a hash value
		// identical to the current child.  Now check if there is one
		// that is really identical.
		માટે (tp = HI2WN(hi); tp != ખાલી; tp = tp->wn_u2.next)
		    જો (ગાંઠ_equal(child, tp))
		    {
			// Found one!  Now use that child in place of the
			// current one.  This means the current child and all
			// its siblings is unlinked from the tree.
			++tp->wn_refs;
			compressed += deref_wordnode(spin, child);
			np->wn_child = tp;
			તોડો;
		    }
		જો (tp == ખાલી)
		{
		    // No other child with this hash value equals the child of
		    // the node, add it to the linked list after the first
		    // item.
		    tp = HI2WN(hi);
		    child->wn_u2.next = tp->wn_u2.next;
		    tp->wn_u2.next = child;
		}
	    }
	    નહીંતર
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
    ગાંઠ->wn_u1.hashkey[0] = len;
    nr = 0;
    FOR_ALL_ગાંઠ_SIBLINGS(ગાંઠ, np)
    {
	જો (np->wn_byte == NUL)
	    // end node: use wn_flags, wn_region and wn_affixID
	    n = np->wn_flags + (np->wn_region << 8) + (np->wn_affixID << 16);
	નહીંતર
	    // byte node: use the byte value and the child pointer
	    n = (unsigned)(np->wn_byte + ((long_u)np->wn_child << 8));
	nr = nr * 101 + n;
    }

    // Avoid NUL bytes, it terminates the hash key.
    n = nr & 0xff;
    ગાંઠ->wn_u1.hashkey[1] = n == 0 ? 1 : n;
    n = (nr >> 8) & 0xff;
    ગાંઠ->wn_u1.hashkey[2] = n == 0 ? 1 : n;
    n = (nr >> 16) & 0xff;
    ગાંઠ->wn_u1.hashkey[3] = n == 0 ? 1 : n;
    n = (nr >> 24) & 0xff;
    ગાંઠ->wn_u1.hashkey[4] = n == 0 ? 1 : n;
    ગાંઠ->wn_u1.hashkey[5] = NUL;

    // Check for CTRL-C pressed now and then.
    veryfast_breakcheck();

    પાછા compressed;
}

/*
 * Return TRUE when two nodes have identical siblings and children.
 */
    સ્થિર પૂર્ણાંક
ગાંઠ_equal(wordnode_T *n1, wordnode_T *n2)
{
    wordnode_T	*p1;
    wordnode_T	*p2;

    માટે (p1 = n1, p2 = n2; p1 != ખાલી && p2 != ખાલી;
				     p1 = p1->wn_sibling, p2 = p2->wn_sibling)
	જો (p1->wn_byte != p2->wn_byte
		|| (p1->wn_byte == NUL
		    ? (p1->wn_flags != p2->wn_flags
			|| p1->wn_region != p2->wn_region
			|| p1->wn_affixID != p2->wn_affixID)
		    : (p1->wn_child != p2->wn_child)))
	    તોડો;

    પાછા p1 == ખાલી && p2 == ખાલી;
}

સ્થિર પૂર્ણાંક rep_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2);

/*
 * Function given to qsort() to sort the REP items on "from" string.
 */
    સ્થિર પૂર્ણાંક
rep_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    fromto_T	*p1 = (fromto_T *)s1;
    fromto_T	*p2 = (fromto_T *)s2;

    પાછા STRCMP(p1->ft_from, p2->ft_from);
}

/*
 * Write the Vim .spl file "fname".
 * Return FAIL or OK;
 */
    સ્થિર પૂર્ણાંક
write_vim_spell(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    પૂર્ણાંક		regionmask;
    પૂર્ણાંક		round;
    wordnode_T	*વૃક્ષ;
    પૂર્ણાંક		nodecount;
    પૂર્ણાંક		i;
    પૂર્ણાંક		l;
    garray_T	*gap;
    fromto_T	*ftp;
    char_u	*p;
    પૂર્ણાંક		rr;
    પૂર્ણાંક		retval = OK;
    size_t	fwv = 1;  // collect return value of fwrite() to avoid
			  // warnings from picky compiler

    fd = mch_fopen((અક્ષર *)fname, "w");
    જો (fd == ખાલી)
    {
	semsg(_(e_cant_open_file_str), fname);
	પાછા FAIL;
    }

    // <HEADER>: <fileID> <versionnr>
							    // <fileID>
    fwv &= fwrite(VIMSPELLMAGIC, VIMSPELLMAGICL, (size_t)1, fd);
    જો (fwv != (size_t)1)
	// Catch first write error, don't try writing more.
	જાઓ theend;

    putc(VIMSPELLVERSION, fd);				    // <versionnr>

    /*
     * <SECTIONS>: <section> ... <sectionend>
     */

    // SN_INFO: <infotext>
    જો (spin->si_info != ખાલી)
    {
	putc(SN_INFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	i = (પૂર્ણાંક)STRLEN(spin->si_info);
	put_bytes(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_info, (size_t)i, (size_t)1, fd); // <infotext>
    }

    // SN_REGION: <regionname> ...
    // Write the region names only if there is more than one.
    જો (spin->si_region_ગણતરી > 1)
    {
	putc(SN_REGION, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>
	l = spin->si_region_ગણતરી * 2;
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_region_name, (size_t)l, (size_t)1, fd);
							// <regionname> ...
	regionmask = (1 << spin->si_region_ગણતરી) - 1;
    }
    નહીંતર
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
    જો (!spin->si_ascii && !spin->si_add)
    {
	char_u	folchars[128 * 8];
	પૂર્ણાંક	flags;

	putc(SN_CHARFLAGS, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	// Form the <folchars> string first, we need to know its length.
	l = 0;
	માટે (i = 128; i < 256; ++i)
	{
	    જો (has_mbyte)
		l += mb_char2bytes(spelltab.st_fold[i], folchars + l);
	    નહીંતર
		folchars[l++] = spelltab.st_fold[i];
	}
	put_bytes(fd, (long_u)(1 + 128 + 2 + l), 4);	// <sectionlen>

	fputc(128, fd);					// <charflagslen>
	માટે (i = 128; i < 256; ++i)
	{
	    flags = 0;
	    જો (spelltab.st_isw[i])
		flags |= CF_WORD;
	    જો (spelltab.st_isu[i])
		flags |= CF_UPPER;
	    fputc(flags, fd);				// <charflags>
	}

	put_bytes(fd, (long_u)l, 2);			// <folcharslen>
	fwv &= fwrite(folchars, (size_t)l, (size_t)1, fd); // <folchars>
    }

    // SN_MIDWORD: <midword>
    જો (spin->si_midword != ખાલી)
    {
	putc(SN_MIDWORD, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	i = (પૂર્ણાંક)STRLEN(spin->si_midword);
	put_bytes(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_midword, (size_t)i, (size_t)1, fd);
							// <midword>
    }

    // SN_PREFCOND: <prefcondcnt> <prefcond> ...
    જો (spin->si_prefcond.ga_len > 0)
    {
	putc(SN_PREFCOND, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	l = write_spell_prefcond(ખાલી, &spin->si_prefcond, &fwv);
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>

	write_spell_prefcond(fd, &spin->si_prefcond, &fwv);
    }

    // SN_REP: <repcount> <rep> ...
    // SN_SAL: <salflags> <salcount> <sal> ...
    // SN_REPSAL: <repcount> <rep> ...

    // round 1: SN_REP section
    // round 2: SN_SAL section (unless SN_SOFO is used)
    // round 3: SN_REPSAL section
    માટે (round = 1; round <= 3; ++round)
    {
	જો (round == 1)
	    gap = &spin->si_rep;
	નહીંતર જો (round == 2)
	{
	    // Don't write SN_SAL when using a SN_SOFO section
	    જો (spin->si_sofofr != ખાલી && spin->si_sofoto != ખાલી)
		ચાલુ;
	    gap = &spin->si_sal;
	}
	નહીંતર
	    gap = &spin->si_repsal;

	// Don't write the section if there are no items.
	જો (gap->ga_len == 0)
	    ચાલુ;

	// Sort the REP/REPSAL items.
	જો (round != 2)
	    qsort(gap->ga_data, (size_t)gap->ga_len,
					       કદ(fromto_T), rep_compare);

	i = round == 1 ? SN_REP : (round == 2 ? SN_SAL : SN_REPSAL);
	putc(i, fd);					// <sectionID>

	// This is for making suggestions, section is not required.
	putc(0, fd);					// <sectionflags>

	// Compute the length of what follows.
	l = 2;	    // count <repcount> or <salcount>
	માટે (i = 0; i < gap->ga_len; ++i)
	{
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    l += 1 + (પૂર્ણાંક)STRLEN(ftp->ft_from);  // count <*fromlen> and <*from>
	    l += 1 + (પૂર્ણાંક)STRLEN(ftp->ft_to);    // count <*tolen> and <*to>
	}
	જો (round == 2)
	    ++l;	// count <salflags>
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>

	જો (round == 2)
	{
	    i = 0;
	    જો (spin->si_followup)
		i |= SAL_F0LLOWUP;
	    જો (spin->si_collapse)
		i |= SAL_COLLAPSE;
	    જો (spin->si_rem_accents)
		i |= SAL_REM_ACCENTS;
	    putc(i, fd);			// <salflags>
	}

	put_bytes(fd, (long_u)gap->ga_len, 2);	// <repcount> or <salcount>
	માટે (i = 0; i < gap->ga_len; ++i)
	{
	    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
	    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    માટે (rr = 1; rr <= 2; ++rr)
	    {
		p = rr == 1 ? ftp->ft_from : ftp->ft_to;
		l = (પૂર્ણાંક)STRLEN(p);
		putc(l, fd);
		જો (l > 0)
		    fwv &= fwrite(p, l, (size_t)1, fd);
	    }
	}

    }

    // SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
    // This is for making suggestions, section is not required.
    જો (spin->si_sofofr != ખાલી && spin->si_sofoto != ખાલી)
    {
	putc(SN_SOFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (પૂર્ણાંક)STRLEN(spin->si_sofofr);
	put_bytes(fd, (long_u)(l + STRLEN(spin->si_sofoto) + 4), 4);
							// <sectionlen>

	put_bytes(fd, (long_u)l, 2);			// <sofofromlen>
	fwv &= fwrite(spin->si_sofofr, l, (size_t)1, fd); // <sofofrom>

	l = (પૂર્ણાંક)STRLEN(spin->si_sofoto);
	put_bytes(fd, (long_u)l, 2);			// <sofotolen>
	fwv &= fwrite(spin->si_sofoto, l, (size_t)1, fd); // <sofoto>
    }

    // SN_WORDS: <word> ...
    // This is for making suggestions, section is not required.
    જો (spin->si_commonwords.ht_used > 0)
    {
	putc(SN_શબ્દો, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// round 1: count the bytes
	// round 2: write the bytes
	માટે (round = 1; round <= 2; ++round)
	{
	    પૂર્ણાંક		todo;
	    પૂર્ણાંક		len = 0;
	    hashitem_T	*hi;

	    todo = (પૂર્ણાંક)spin->si_commonwords.ht_used;
	    FOR_ALL_HASHTAB_વસ્તુઓ(&spin->si_commonwords, hi, todo)
		જો (!HASHITEM_EMPTY(hi))
		{
		    l = (પૂર્ણાંક)STRLEN(hi->hi_ચાવી) + 1;
		    len += l;
		    જો (round == 2)			// <word>
			fwv &= fwrite(hi->hi_ચાવી, (size_t)l, (size_t)1, fd);
		    --todo;
		}
	    જો (round == 1)
		put_bytes(fd, (long_u)len, 4);		// <sectionlen>
	}
    }

    // SN_MAP: <mapstr>
    // This is for making suggestions, section is not required.
    જો (spin->si_map.ga_len > 0)
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
    જો (!spin->si_nosugfile
	    && (spin->si_sal.ga_len > 0
		     || (spin->si_sofofr != ખાલી && spin->si_sofoto != ખાલી)))
    {
	putc(SN_SUGFILE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)8, 4);			// <sectionlen>

	// Set si_sugtime and write it to the file.
	spin->si_sugtime = time(ખાલી);
	put_time(fd, spin->si_sugtime);			// <timestamp>
    }

    // SN_NOSPLITSUGS: nothing
    // This is used to notify that no suggestions with word splits are to be
    // made.
    જો (spin->si_nosplitsugs)
    {
	putc(SN_NOSPLITSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_NOCOMPUNDSUGS: nothing
    // This is used to notify that no suggestions with compounds are to be
    // made.
    જો (spin->si_nocompoundsugs)
    {
	putc(SN_NOCOMPOUNDSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_COMPOUND: compound info.
    // We don't mark it required, when not supported all compound words will
    // be bad words.
    જો (spin->si_compflags != ખાલી)
    {
	putc(SN_COMPOUND, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (પૂર્ણાંક)STRLEN(spin->si_compflags);
	માટે (i = 0; i < spin->si_comppat.ga_len; ++i)
	    l += (પૂર્ણાંક)STRLEN(((char_u **)(spin->si_comppat.ga_data))[i]) + 1;
	put_bytes(fd, (long_u)(l + 7), 4);		// <sectionlen>

	putc(spin->si_compmax, fd);			// <compmax>
	putc(spin->si_compminlen, fd);			// <compminlen>
	putc(spin->si_compsylmax, fd);			// <compsylmax>
	putc(0, fd);		// for Vim 7.0b compatibility
	putc(spin->si_compoptions, fd);			// <compoptions>
	put_bytes(fd, (long_u)spin->si_comppat.ga_len, 2);
							// <comppatcount>
	માટે (i = 0; i < spin->si_comppat.ga_len; ++i)
	{
	    p = ((char_u **)(spin->si_comppat.ga_data))[i];
	    putc((પૂર્ણાંક)STRLEN(p), fd);			// <comppatlen>
	    fwv &= fwrite(p, (size_t)STRLEN(p), (size_t)1, fd);
							// <comppattext>
	}
							// <compflags>
	fwv &= fwrite(spin->si_compflags, (size_t)STRLEN(spin->si_compflags),
							       (size_t)1, fd);
    }

    // SN_NOBREAK: NOBREAK flag
    જો (spin->si_nobreak)
    {
	putc(SN_NOBREAK, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// It's empty, the presence of the section flags the feature.
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_SYLLABLE: syllable info.
    // We don't mark it required, when not supported syllables will not be
    // counted.
    જો (spin->si_syllable != ખાલી)
    {
	putc(SN_SYLLABLE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (પૂર્ણાંક)STRLEN(spin->si_syllable);
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
    માટે (round = 1; round <= 3; ++round)
    {
	જો (round == 1)
	    વૃક્ષ = spin->si_foldroot->wn_sibling;
	નહીંતર જો (round == 2)
	    વૃક્ષ = spin->si_keeproot->wn_sibling;
	નહીંતર
	    વૃક્ષ = spin->si_prefroot->wn_sibling;

	// Clear the index and wnode fields in the tree.
	clear_ગાંઠ(વૃક્ષ);

	// Count the number of nodes.  Needed to be able to allocate the
	// memory when reading the nodes.  Also fills in index for shared
	// nodes.
	nodecount = put_ગાંઠ(ખાલી, વૃક્ષ, 0, regionmask, round == 3);

	// number of nodes in 4 bytes
	put_bytes(fd, (long_u)nodecount, 4);	// <nodecount>
	spin->si_memtot += nodecount + nodecount * કદ(પૂર્ણાંક);

	// Write the nodes.
	(શૂન્ય)put_ગાંઠ(fd, વૃક્ષ, 0, regionmask, round == 3);
    }

    // Write another byte to check for errors (file system full).
    જો (putc(0, fd) == EOF)
	retval = FAIL;
theend:
    જો (fclose(fd) == EOF)
	retval = FAIL;

    જો (fwv != (size_t)1)
	retval = FAIL;
    જો (retval == FAIL)
	emsg(_(e_error_while_writing));

    પાછા retval;
}

/*
 * Clear the index and wnode fields of "node", it siblings and its
 * children.  This is needed because they are a union with other items to save
 * space.
 */
    સ્થિર શૂન્ય
clear_ગાંઠ(wordnode_T *ગાંઠ)
{
    wordnode_T	*np;

    જો (ગાંઠ != ખાલી)
	FOR_ALL_ગાંઠ_SIBLINGS(ગાંઠ, np)
	{
	    np->wn_u1.index = 0;
	    np->wn_u2.wnode = ખાલી;

	    જો (np->wn_byte != NUL)
		clear_ગાંઠ(np->wn_child);
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
    સ્થિર પૂર્ણાંક
put_ગાંઠ(
    FILE	*fd,		// NULL when only counting
    wordnode_T	*ગાંઠ,
    પૂર્ણાંક		idx,
    પૂર્ણાંક		regionmask,
    પૂર્ણાંક		prefixtree)	// TRUE for PREFIXTREE
{
    પૂર્ણાંક		newindex = idx;
    પૂર્ણાંક		siblingcount = 0;
    wordnode_T	*np;
    પૂર્ણાંક		flags;

    // If "node" is zero the tree is empty.
    જો (ગાંઠ == ખાલી)
	પાછા 0;

    // Store the index where this node is written.
    ગાંઠ->wn_u1.index = idx;

    // Count the number of siblings.
    FOR_ALL_ગાંઠ_SIBLINGS(ગાંઠ, np)
	++siblingcount;

    // Write the sibling count.
    જો (fd != ખાલી)
	putc(siblingcount, fd);				// <siblingcount>

    // Write each sibling byte and optionally extra info.
    FOR_ALL_ગાંઠ_SIBLINGS(ગાંઠ, np)
    {
	જો (np->wn_byte == 0)
	{
	    જો (fd != ખાલી)
	    {
		// For a NUL byte (end of word) write the flags etc.
		જો (prefixtree)
		{
		    // In PREFIXTREE write the required affixID and the
		    // associated condition nr (stored in wn_region).  The
		    // byte value is misused to store the "rare" and "not
		    // combining" flags
		    જો (np->wn_flags == (short_u)PFX_FLAGS)
			putc(BY_NOFLAGS, fd);		// <byte>
		    નહીંતર
		    {
			putc(BY_FLAGS, fd);		// <byte>
			putc(np->wn_flags, fd);		// <pflags>
		    }
		    putc(np->wn_affixID, fd);		// <affixID>
		    put_bytes(fd, (long_u)np->wn_region, 2); // <prefcondnr>
		}
		નહીંતર
		{
		    // For word trees we write the flag/region items.
		    flags = np->wn_flags;
		    જો (regionmask != 0 && np->wn_region != regionmask)
			flags |= WF_REGION;
		    જો (np->wn_affixID != 0)
			flags |= WF_AFX;
		    જો (flags == 0)
		    {
			// word without flags or region
			putc(BY_NOFLAGS, fd);			// <byte>
		    }
		    નહીંતર
		    {
			જો (np->wn_flags >= 0x100)
			{
			    putc(BY_FLAGS2, fd);		// <byte>
			    putc(flags, fd);			// <flags>
			    putc((unsigned)flags >> 8, fd);	// <flags2>
			}
			નહીંતર
			{
			    putc(BY_FLAGS, fd);			// <byte>
			    putc(flags, fd);			// <flags>
			}
			જો (flags & WF_REGION)
			    putc(np->wn_region, fd);		// <region>
			જો (flags & WF_AFX)
			    putc(np->wn_affixID, fd);		// <affixID>
		    }
		}
	    }
	}
	નહીંતર
	{
	    જો (np->wn_child->wn_u1.index != 0
					 && np->wn_child->wn_u2.wnode != ગાંઠ)
	    {
		// The child is written elsewhere, write the reference.
		જો (fd != ખાલી)
		{
		    putc(BY_INDEX, fd);			// <byte>
							// <nodeidx>
		    put_bytes(fd, (long_u)np->wn_child->wn_u1.index, 3);
		}
	    }
	    નહીંતર જો (np->wn_child->wn_u2.wnode == ખાલી)
		// We will write the child below and give it an index.
		np->wn_child->wn_u2.wnode = ગાંઠ;

	    જો (fd != ખાલી)
		જો (putc(np->wn_byte, fd) == EOF) // <byte> or <xbyte>
		{
		    emsg(_(e_error_while_writing));
		    પાછા 0;
		}
	}
    }

    // Space used in the array when reading: one for each sibling and one for
    // the count.
    newindex += siblingcount + 1;

    // Recursively dump the children of each sibling.
    FOR_ALL_ગાંઠ_SIBLINGS(ગાંઠ, np)
	જો (np->wn_byte != 0 && np->wn_child->wn_u2.wnode == ગાંઠ)
	    newindex = put_ગાંઠ(fd, np->wn_child, newindex, regionmask,
								  prefixtree);

    પાછા newindex;
}


/*
 * ":mkspell [-ascii] outfile  infile ..."
 * ":mkspell [-ascii] addfile"
 */
    શૂન્ય
ex_mkspell(exarg_T *eap)
{
    પૂર્ણાંક		fcount;
    char_u	**fnames;
    char_u	*arg = eap->arg;
    પૂર્ણાંક		ascii = FALSE;

    જો (STRNCMP(arg, "-ascii", 6) == 0)
    {
	ascii = TRUE;
	arg = skipwhite(arg + 6);
    }

    // Expand all the remaining arguments (e.g., $VIMRUNTIME).
    જો (get_arglist_exp(arg, &fcount, &fnames, FALSE) != OK)
	પાછા;

    mkspell(fcount, fnames, ascii, eap->forceit, FALSE);
    FreeWild(fcount, fnames);
}

/*
 * Create the .sug file.
 * Uses the soundfold info in "spin".
 * Writes the file with the name "wfname", with ".spl" changed to ".sug".
 */
    સ્થિર શૂન્ય
spell_make_sugfile(spellinfo_T *spin, char_u *wfname)
{
    char_u	*fname = ખાલી;
    પૂર્ણાંક		len;
    slang_T	*slang;
    પૂર્ણાંક		free_slang = FALSE;

    /*
     * Read back the .spl file that was written.  This fills the required
     * info for soundfolding.  This also uses less memory than the
     * pointer-linked version of the trie.  And it avoids having two versions
     * of the code for the soundfolding stuff.
     * It might have been done already by spell_reload_one().
     */
    FOR_ALL_SPELL_LANGS(slang)
	જો (fullpathcmp(wfname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	    તોડો;
    જો (slang == ખાલી)
    {
	spell_message(spin, (char_u *)_("Reading back spell file..."));
	slang = spell_load_file(wfname, ખાલી, ખાલી, FALSE);
	જો (slang == ખાલી)
	    પાછા;
	free_slang = TRUE;
    }

    /*
     * Clear the info in "spin" that is used.
     */
    spin->si_blocks = ખાલી;
    spin->si_blocks_cnt = 0;
    spin->si_compress_cnt = 0;	    // will stay at 0 all the time
    spin->si_free_ગણતરી = 0;
    spin->si_first_free = ખાલી;
    spin->si_foldwcount = 0;

    /*
     * Go through the trie of good words, soundfold each word and add it to
     * the soundfold trie.
     */
    spell_message(spin, (char_u *)_("Performing soundfolding..."));
    જો (sug_filltree(spin, slang) == FAIL)
	જાઓ theend;

    /*
     * Create the table which links each soundfold word with a list of the
     * good words it may come from.  Creates buffer "spin->si_spellbuf".
     * This also removes the wordnr from the NUL byte entries to make
     * compression possible.
     */
    જો (sug_maketable(spin) == FAIL)
	જાઓ theend;

    smsg(_("Number of words after soundfolding: %ld"),
				 (long)spin->si_spellbuf->b_ml.ml_લીટી_ગણતરી);

    /*
     * Compress the soundfold trie.
     */
    spell_message(spin, (char_u *)_(સંદેશ_compressing));
    wordtree_compress(spin, spin->si_foldroot, "case-folded");

    /*
     * Write the .sug file.
     * Make the file name by changing ".spl" to ".sug".
     */
    fname = alloc(MAXPATHL);
    જો (fname == ખાલી)
	જાઓ theend;
    vim_strncpy(fname, wfname, MAXPATHL - 1);
    len = (પૂર્ણાંક)STRLEN(fname);
    fname[len - 2] = 'u';
    fname[len - 1] = 'g';
    sug_write(spin, fname);

theend:
    vim_free(fname);
    જો (free_slang)
	slang_free(slang);
    free_blocks(spin->si_blocks);
    close_spellbuf(spin->si_spellbuf);
}

/*
 * Build the soundfold trie for language "slang".
 */
    સ્થિર પૂર્ણાંક
sug_filltree(spellinfo_T *spin, slang_T *slang)
{
    char_u	*byts;
    idx_T	*idxs;
    પૂર્ણાંક		depth;
    idx_T	arridx[MAXWLEN];
    પૂર્ણાંક		curi[MAXWLEN];
    char_u	tword[MAXWLEN];
    char_u	tsalword[MAXWLEN];
    પૂર્ણાંક		c;
    idx_T	n;
    unsigned	શબ્દો_done = 0;
    પૂર્ણાંક		wordcount[MAXWLEN];

    // We use si_foldroot for the soundfolded trie.
    spin->si_foldroot = wordtree_alloc(spin);
    જો (spin->si_foldroot == ખાલી)
	પાછા FAIL;

    // let tree_add_word() know we're adding to the soundfolded tree
    spin->si_sugtree = TRUE;

    /*
     * Go through the whole case-folded tree, soundfold each word and put it
     * in the trie.  Bail out if the tree is empty.
     */
    byts = slang->sl_fbyts;
    idxs = slang->sl_fidxs;
    જો (byts == ખાલી || idxs == ખાલી)
	પાછા FAIL;

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;

    depth = 0;
    જ્યારે (depth >= 0 && !got_int)
    {
	જો (curi[depth] > byts[arridx[depth]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[depth]] = wordcount[depth];
	    જો (depth > 0)
		wordcount[depth - 1] += wordcount[depth];

	    --depth;
	    લીટી_breakcheck();
	}
	નહીંતર
	{

	    // Do one more byte at this node.
	    n = arridx[depth] + curi[depth];
	    ++curi[depth];

	    c = byts[n];
	    જો (c == 0)
	    {
		// Sound-fold the word.
		tword[depth] = NUL;
		spell_soundfold(slang, tword, TRUE, tsalword);

		// We use the "flags" field for the MSB of the wordnr,
		// "region" for the LSB of the wordnr.
		જો (વૃક્ષ_add_word(spin, tsalword, spin->si_foldroot,
				શબ્દો_done >> 16, શબ્દો_done & 0xffff,
							   0) == FAIL)
		    પાછા FAIL;

		++શબ્દો_done;
		++wordcount[depth];

		// Reset the block count each time to avoid compression
		// kicking in.
		spin->si_blocks_cnt = 0;

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		જ્યારે (n + 1 < slang->sl_fbyts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[depth];
		}
	    }
	    નહીંતર જો (depth < MAXWLEN - 1)
	    {
		// Normal char, go one level deeper.
		tword[depth++] = c;
		arridx[depth] = idxs[n];
		curi[depth] = 1;
		wordcount[depth] = 0;
	    }
	}
    }

    smsg(_("Total number of words: %d"), શબ્દો_done);

    પાછા OK;
}

/*
 * Make the table that links each word in the soundfold trie to the words it
 * can be produced from.
 * This is not unlike lines in a file, thus use a memfile to be able to access
 * the table efficiently.
 * Returns FAIL when out of memory.
 */
    સ્થિર પૂર્ણાંક
sug_maketable(spellinfo_T *spin)
{
    garray_T	ga;
    પૂર્ણાંક		res = OK;

    // Allocate a buffer, open a memline for it and create the swap file
    // (uses a temp file, not a .swp file).
    spin->si_spellbuf = open_spellbuf();
    જો (spin->si_spellbuf == ખાલી)
	પાછા FAIL;

    // Use a buffer to store the line info, avoids allocating many small
    // pieces of memory.
    ga_init2(&ga, 1, 100);

    // recursively go through the tree
    જો (sug_filltable(spin, spin->si_foldroot->wn_sibling, 0, &ga) == -1)
	res = FAIL;

    ga_clear(&ga);
    પાછા res;
}

/*
 * Fill the table for one node and its children.
 * Returns the wordnr at the start of the node.
 * Returns -1 when out of memory.
 */
    સ્થિર પૂર્ણાંક
sug_filltable(
    spellinfo_T	*spin,
    wordnode_T	*ગાંઠ,
    પૂર્ણાંક		startwordnr,
    garray_T	*gap)	    // place to store line of numbers
{
    wordnode_T	*p, *np;
    પૂર્ણાંક		wordnr = startwordnr;
    પૂર્ણાંક		nr;
    પૂર્ણાંક		prev_nr;

    FOR_ALL_ગાંઠ_SIBLINGS(ગાંઠ, p)
    {
	જો (p->wn_byte == NUL)
	{
	    gap->ga_len = 0;
	    prev_nr = 0;
	    માટે (np = p; np != ખાલી && np->wn_byte == NUL; np = np->wn_sibling)
	    {
		જો (ga_grow(gap, 10) == FAIL)
		    પાછા -1;

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

	    જો (ml_append_buf(spin->si_spellbuf, (linenr_T)wordnr,
				     gap->ga_data, gap->ga_len, TRUE) == FAIL)
		પાછા -1;
	    ++wordnr;

	    // Remove extra NUL entries, we no longer need them. We don't
	    // bother freeing the nodes, they won't be reused anyway.
	    જ્યારે (p->wn_sibling != ખાલી && p->wn_sibling->wn_byte == NUL)
		p->wn_sibling = p->wn_sibling->wn_sibling;

	    // Clear the flags on the remaining NUL node, so that compression
	    // works a lot better.
	    p->wn_flags = 0;
	    p->wn_region = 0;
	}
	નહીંતર
	{
	    wordnr = sug_filltable(spin, p->wn_child, wordnr, gap);
	    જો (wordnr == -1)
		પાછા -1;
	}
    }
    પાછા wordnr;
}

/*
 * Convert an offset into a minimal number of bytes.
 * Similar to utf_char2byters, but use 8 bits in followup bytes and avoid NUL
 * bytes.
 */
    સ્થિર પૂર્ણાંક
offset2bytes(પૂર્ણાંક nr, char_u *buf)
{
    પૂર્ણાંક	    rem;
    પૂર્ણાંક	    b1, b2, b3, b4;

    // Split the number in parts of base 255.  We need to avoid NUL bytes.
    b1 = nr % 255 + 1;
    rem = nr / 255;
    b2 = rem % 255 + 1;
    rem = rem / 255;
    b3 = rem % 255 + 1;
    b4 = rem / 255 + 1;

    જો (b4 > 1 || b3 > 0x1f)	// 4 bytes
    {
	buf[0] = 0xe0 + b4;
	buf[1] = b3;
	buf[2] = b2;
	buf[3] = b1;
	પાછા 4;
    }
    જો (b3 > 1 || b2 > 0x3f )	// 3 bytes
    {
	buf[0] = 0xc0 + b3;
	buf[1] = b2;
	buf[2] = b1;
	પાછા 3;
    }
    જો (b2 > 1 || b1 > 0x7f )	// 2 bytes
    {
	buf[0] = 0x80 + b2;
	buf[1] = b1;
	પાછા 2;
    }
				// 1 byte
    buf[0] = b1;
    પાછા 1;
}

/*
 * Write the .sug file in "fname".
 */
    સ્થિર શૂન્ય
sug_write(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    wordnode_T	*વૃક્ષ;
    પૂર્ણાંક		nodecount;
    પૂર્ણાંક		wcount;
    char_u	*લીટી;
    linenr_T	lnum;
    પૂર્ણાંક		len;

    // Create the file.  Note that an existing file is silently overwritten!
    fd = mch_fopen((અક્ષર *)fname, "w");
    જો (fd == ખાલી)
    {
	semsg(_(e_cant_open_file_str), fname);
	પાછા;
    }

    vim_snprintf((અક્ષર *)IObuff, IOSIZE,
				  _("Writing suggestion file %s..."), fname);
    spell_message(spin, IObuff);

    /*
     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
     */
    જો (fwrite(VIMSUGMAGIC, VIMSUGMAGICL, (size_t)1, fd) != 1) // <fileID>
    {
	emsg(_(e_error_while_writing));
	જાઓ theend;
    }
    putc(VIMSUGVERSION, fd);				// <versionnr>

    // Write si_sugtime to the file.
    put_time(fd, spin->si_sugtime);			// <timestamp>

    /*
     * <SUGWORDTREE>
     */
    spin->si_memtot = 0;
    વૃક્ષ = spin->si_foldroot->wn_sibling;

    // Clear the index and wnode fields in the tree.
    clear_ગાંઠ(વૃક્ષ);

    // Count the number of nodes.  Needed to be able to allocate the
    // memory when reading the nodes.  Also fills in index for shared
    // nodes.
    nodecount = put_ગાંઠ(ખાલી, વૃક્ષ, 0, 0, FALSE);

    // number of nodes in 4 bytes
    put_bytes(fd, (long_u)nodecount, 4);	// <nodecount>
    spin->si_memtot += nodecount + nodecount * કદ(પૂર્ણાંક);

    // Write the nodes.
    (શૂન્ય)put_ગાંઠ(fd, વૃક્ષ, 0, 0, FALSE);

    /*
     * <SUGTABLE>: <sugwcount> <sugline> ...
     */
    wcount = spin->si_spellbuf->b_ml.ml_લીટી_ગણતરી;
    put_bytes(fd, (long_u)wcount, 4);	// <sugwcount>

    માટે (lnum = 1; lnum <= (linenr_T)wcount; ++lnum)
    {
	// <sugline>: <sugnr> ... NUL
	લીટી = ml_get_buf(spin->si_spellbuf, lnum, FALSE);
	len = ml_get_buf_len(spin->si_spellbuf, lnum) + 1;
	જો (fwrite(લીટી, (size_t)len, (size_t)1, fd) == 0)
	{
	    emsg(_(e_error_while_writing));
	    જાઓ theend;
	}
	spin->si_memtot += len;
    }

    // Write another byte to check for errors.
    જો (putc(0, fd) == EOF)
	emsg(_(e_error_while_writing));

    vim_snprintf((અક્ષર *)IObuff, IOSIZE,
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
    શૂન્ય
mkspell(
    પૂર્ણાંક		fcount,
    char_u	**fnames,
    પૂર્ણાંક		ascii,		    // -ascii argument given
    પૂર્ણાંક		over_write,	    // overwrite existing output file
    પૂર્ણાંક		added_word)	    // invoked through "zg"
{
    char_u	*fname = ખાલી;
    char_u	*wfname;
    char_u	**innames;
    પૂર્ણાંક		incount;
    afffile_T	*(afile[MAXREGIONS]);
    પૂર્ણાંક		i;
    પૂર્ણાંક		len;
    stat_T	st;
    પૂર્ણાંક		error = FALSE;
    spellinfo_T spin;

    CLEAR_FIELD(spin);
    spin.si_verbose = !added_word;
    spin.si_ascii = ascii;
    spin.si_followup = TRUE;
    spin.si_rem_accents = TRUE;
    ga_init2(&spin.si_rep, કદ(fromto_T), 20);
    ga_init2(&spin.si_repsal, કદ(fromto_T), 20);
    ga_init2(&spin.si_sal, કદ(fromto_T), 20);
    ga_init2(&spin.si_map, કદ(char_u), 100);
    ga_init2(&spin.si_comppat, કદ(char_u *), 20);
    ga_init2(&spin.si_prefcond, કદ(char_u *), 50);
    hash_init(&spin.si_commonwords);
    spin.si_newcompID = 127;	// start compound ID at first maximum

    // default: fnames[0] is output file, following are input files
    // When "fcount" is 1 there is only one file.
    innames = &fnames[fcount == 1 ? 0 : 1];
    incount = fcount - 1;

    wfname = alloc(MAXPATHL);
    જો (wfname == ખાલી)
	પાછા;

    જો (fcount >= 1)
    {
	len = (પૂર્ણાંક)STRLEN(fnames[0]);
	જો (fcount == 1 && len > 4 && STRCMP(fnames[0] + len - 4, ".add") == 0)
	{
	    // For ":mkspell path/en.latin1.add" output file is
	    // "path/en.latin1.add.spl".
	    incount = 1;
	    vim_snprintf((અક્ષર *)wfname, MAXPATHL, "%s.spl", fnames[0]);
	}
	નહીંતર જો (fcount == 1)
	{
	    // For ":mkspell path/vim" output file is "path/vim.latin1.spl".
	    incount = 1;
	    vim_snprintf((અક્ષર *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());
	}
	નહીંતર જો (len > 4 && STRCMP(fnames[0] + len - 4, ".spl") == 0)
	{
	    // Name ends in ".spl", use as the file name.
	    vim_strncpy(wfname, fnames[0], MAXPATHL - 1);
	}
	નહીંતર
	    // Name should be language, make the file name from it.
	    vim_snprintf((અક્ષર *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());

	// Check for .ascii.spl.
	જો (strstr((અક્ષર *)gettail(wfname), SPL_FNAME_ASCII) != ખાલી)
	    spin.si_ascii = TRUE;

	// Check for .add.spl.
	જો (strstr((અક્ષર *)gettail(wfname), SPL_FNAME_ADD) != ખાલી)
	    spin.si_add = TRUE;
    }

    જો (incount <= 0)
	emsg(_(e_invalid_argument));	// need at least output and input names
    નહીંતર જો (vim_strchr(gettail(wfname), '_') != ખાલી)
	emsg(_(e_output_file_name_must_not_have_region_name));
    નહીંતર જો (incount > MAXREGIONS)
	semsg(_(e_only_up_to_nr_regions_supported), MAXREGIONS);
    નહીંતર
    {
	// Check for overwriting before doing things that may take a lot of
	// time.
	જો (!over_write && mch_stat((અક્ષર *)wfname, &st) >= 0)
	{
	    emsg(_(e_file_exists));
	    જાઓ theend;
	}
	જો (mch_isdir(wfname))
	{
	    semsg(_(e_str_is_directory), wfname);
	    જાઓ theend;
	}

	fname = alloc(MAXPATHL);
	જો (fname == ખાલી)
	    જાઓ theend;

	/*
	 * Init the aff and dic pointers.
	 * Get the region names if there are more than 2 arguments.
	 */
	માટે (i = 0; i < incount; ++i)
	{
	    afile[i] = ખાલી;

	    જો (incount > 1)
	    {
		len = (પૂર્ણાંક)STRLEN(innames[i]);
		જો (STRLEN(gettail(innames[i])) < 5
						|| innames[i][len - 3] != '_')
		{
		    semsg(_(e_invalid_region_in_str), innames[i]);
		    જાઓ theend;
		}
		spin.si_region_name[i * 2] = TOLOWER_ASC(innames[i][len - 2]);
		spin.si_region_name[i * 2 + 1] =
					     TOLOWER_ASC(innames[i][len - 1]);
	    }
	}
	spin.si_region_ગણતરી = incount;

	spin.si_foldroot = wordtree_alloc(&spin);
	spin.si_keeproot = wordtree_alloc(&spin);
	spin.si_prefroot = wordtree_alloc(&spin);
	જો (spin.si_foldroot == ખાલી
		|| spin.si_keeproot == ખાલી
		|| spin.si_prefroot == ખાલી)
	{
	    free_blocks(spin.si_blocks);
	    જાઓ theend;
	}

	// When not producing a .add.spl file clear the character table when
	// we encounter one in the .aff file.  This means we dump the current
	// one in the .spl file if the .aff file doesn't define one.  That's
	// better than guessing the contents, the table will match a
	// previously loaded spell file.
	જો (!spin.si_add)
	    spin.si_clear_chartab = TRUE;

	/*
	 * Read all the .aff and .dic files.
	 * Text is converted to 'encoding'.
	 * Words are stored in the case-folded and keep-case trees.
	 */
	માટે (i = 0; i < incount && !error; ++i)
	{
	    spin.si_conv.vc_type = CONV_NONE;
	    spin.si_region = 1 << i;

	    vim_snprintf((અક્ષર *)fname, MAXPATHL, "%s.aff", innames[i]);
	    જો (mch_stat((અક્ષર *)fname, &st) >= 0)
	    {
		// Read the .aff file.  Will init "spin->si_conv" based on the
		// "SET" line.
		afile[i] = spell_read_aff(&spin, fname);
		જો (afile[i] == ખાલી)
		    error = TRUE;
		નહીંતર
		{
		    // Read the .dic file and store the words in the trees.
		    vim_snprintf((અક્ષર *)fname, MAXPATHL, "%s.dic",
								  innames[i]);
		    જો (spell_read_dic(&spin, fname, afile[i]) == FAIL)
			error = TRUE;
		}
	    }
	    નહીંતર
	    {
		// No .aff file, try reading the file as a word list.  Store
		// the words in the trees.
		જો (spell_read_wordfile(&spin, innames[i]) == FAIL)
		    error = TRUE;
	    }

	    // Free any conversion stuff.
	    convert_setup(&spin.si_conv, ખાલી, ખાલી);
	}

	જો (spin.si_compflags != ખાલી && spin.si_nobreak)
	    સંદેશ(_("Warning: both compounding and NOBREAK specified"));

	જો (!error && !got_int)
	{
	    /*
	     * Combine tails in the tree.
	     */
	    spell_message(&spin, (char_u *)_(સંદેશ_compressing));
	    wordtree_compress(&spin, spin.si_foldroot, "case-folded");
	    wordtree_compress(&spin, spin.si_keeproot, "keep-case");
	    wordtree_compress(&spin, spin.si_prefroot, "prefixes");
	}

	જો (!error && !got_int)
	{
	    /*
	     * Write the info in the spell file.
	     */
	    vim_snprintf((અક્ષર *)IObuff, IOSIZE,
				      _("Writing spell file %s..."), wfname);
	    spell_message(&spin, IObuff);

	    error = write_vim_spell(&spin, wfname) == FAIL;

	    spell_message(&spin, (char_u *)_("Done!"));
	    vim_snprintf((અક્ષર *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin.si_memtot);
	    spell_message(&spin, IObuff);

	    /*
	     * If the file is loaded need to reload it.
	     */
	    જો (!error)
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
	માટે (i = 0; i < incount; ++i)
	    જો (afile[i] != ખાલી)
		spell_free_aff(afile[i]);

	// Free all the bits and pieces at once.
	free_blocks(spin.si_blocks);

	/*
	 * If there is soundfolding info and no NOSUGFILE item create the
	 * .sug file with the soundfolded word trie.
	 */
	જો (spin.si_sugtime != 0 && !error && !got_int)
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
    સ્થિર શૂન્ય
spell_message(spellinfo_T *spin, char_u *str)
{
    જો (spin->si_verbose || p_verbose > 2)
    {
	જો (!spin->si_verbose)
	    verbose_enter();
	સંદેશ((અક્ષર *)str);
	out_flush();
	જો (!spin->si_verbose)
	    verbose_leave();
    }
}

/*
 * ":[count]spellgood  {word}"
 * ":[count]spellwrong {word}"
 * ":[count]spellundo  {word}"
 * ":[count]spellrare  {word}"
 */
    શૂન્ય
ex_spell(exarg_T *eap)
{
    spell_add_word(eap->arg, (પૂર્ણાંક)STRLEN(eap->arg),
		eap->cmdidx == CMD_spellwrong ? SPELL_ADD_BAD :
		eap->cmdidx == CMD_spellrare ? SPELL_ADD_RARE : SPELL_ADD_GOOD,
				   eap->forceit ? 0 : (પૂર્ણાંક)eap->line2,
				   eap->cmdidx == CMD_spellundo);
}

/*
 * Add "word[len]" to 'spellfile' as a good, rare or bad word.
 */
    શૂન્ય
spell_add_word(
    char_u	*word,
    પૂર્ણાંક		len,
    પૂર્ણાંક		what,	    // SPELL_ADD_ values
    પૂર્ણાંક		idx,	    // "zG" and "zW": zero, otherwise index in
			    // 'spellfile'
    પૂર્ણાંક		undo)	    // TRUE for "zug", "zuG", "zuw" and "zuW"
{
    FILE	*fd = ખાલી;
    buf_T	*buf = ખાલી;
    પૂર્ણાંક		new_spf = FALSE;
    char_u	*fname;
    char_u	*fnamebuf = ખાલી;
    char_u	લીટી[MAXWLEN * 2];
    long	fpos, fpos_next = 0;
    પૂર્ણાંક		i;
    size_t	linelen;
    char_u	*spf;

    જો (!valid_spell_word(word, word + len))
    {
	emsg(_(e_illegal_character_in_word));
	પાછા;
    }

    જો (idx == 0)	    // use internal wordlist
    {
	જો (int_wordlist == ખાલી)
	{
	    int_wordlist = vim_tempname('s', FALSE);
	    જો (int_wordlist == ખાલી)
		પાછા;
	}
	fname = int_wordlist;
    }
    નહીંતર
    {
	// If 'spellfile' isn't set figure out a good default value.
	જો (*curwin->w_s->b_p_spf == NUL)
	{
	    init_spellfile();
	    new_spf = TRUE;
	}

	જો (*curwin->w_s->b_p_spf == NUL)
	{
	    semsg(_(e_option_str_is_not_set), "spellfile");
	    પાછા;
	}
	fnamebuf = alloc(MAXPATHL);
	જો (fnamebuf == ખાલી)
	    પાછા;

	માટે (spf = curwin->w_s->b_p_spf, i = 1; *spf != NUL; ++i)
	{
	    copy_option_part(&spf, fnamebuf, MAXPATHL, ",");
	    જો (i == idx)
		તોડો;
	    જો (*spf == NUL)
	    {
		semsg(_(e_spellfile_does_not_have_nr_entries), idx);
		vim_free(fnamebuf);
		પાછા;
	    }
	}

	// Check that the user isn't editing the .add file somewhere.
	buf = buflist_findname_exp(fnamebuf);
	જો (buf != ખાલી && buf->b_ml.ml_mfp == ખાલી)
	    buf = ખાલી;
	જો (buf != ખાલી && bufIsChanged(buf))
	{
	    emsg(_(e_file_is_loaded_in_another_buffer));
	    vim_free(fnamebuf);
	    પાછા;
	}

	fname = fnamebuf;
    }

    જો (what == SPELL_ADD_BAD || undo)
    {
	// When the word appears as good word we need to remove that one,
	// since its flags sort before the one with WF_BANNED.
	fd = mch_fopen((અક્ષર *)fname, "r");
	જો (fd != ખાલી)
	{
	    જ્યારે (!vim_fgets(લીટી, MAXWLEN * 2, fd))
	    {
		fpos = fpos_next;
		fpos_next = ftell(fd);
		જો (fpos_next < 0)
		    તોડો;  // should never happen
		linelen = STRLEN(લીટી);
		જો (linelen >= (size_t)len
			&& STRNCMP(word, લીટી, len) == 0
			&& (લીટી[len] == '/' || લીટી[len] < ' '))
		{
		    // Found duplicate word.  Remove it by writing a '#' at
		    // the start of the line.  Mixing reading and writing
		    // doesn't work for all systems, close the file first.
		    fclose(fd);
		    fd = mch_fopen((અક્ષર *)fname, "r+");
		    જો (fd == ખાલી)
			તોડો;
		    જો (fseek(fd, fpos, SEEK_SET) == 0)
		    {
			fputc('#', fd);
			જો (undo)
			{
			    home_replace(ખાલી, fname, NameBuff, MAXPATHL, TRUE);
			    smsg(_("Word '%.*s' removed from %s"),
							 len, word, NameBuff);
			}
		    }
		    જો (fseek(fd, fpos_next, SEEK_SET) != 0)
		    {
			PERROR(_("Seek error in spellfile"));
			તોડો;
		    }
		}
	    }
	    જો (fd != ખાલી)
		fclose(fd);
	}
    }

    જો (!undo)
    {
	fd = mch_fopen((અક્ષર *)fname, "a");
	જો (fd == ખાલી && new_spf)
	{
	    char_u *p;

	    // We just initialized the 'spellfile' option and can't open the
	    // file.  We may need to create the "spell" directory first.  We
	    // already checked the runtime directory is writable in
	    // init_spellfile().
	    જો (!dir_of_file_exists(fname) && (p = gettail_sep(fname)) != fname)
	    {
		પૂર્ણાંક c = *p;

		// The directory doesn't exist.  Try creating it and opening
		// the file again.
		*p = NUL;
		vim_mkdir(fname, 0755);
		*p = c;
		fd = mch_fopen((અક્ષર *)fname, "a");
	    }
	}

	જો (fd == ખાલી)
	    semsg(_(e_cant_open_file_str), fname);
	નહીંતર
	{
	    જો (what == SPELL_ADD_BAD)
		fprintf(fd, "%.*s/!\n", len, word);
	    નહીંતર જો (what == SPELL_ADD_RARE)
		fprintf(fd, "%.*s/?\n", len, word);
	    નહીંતર
		fprintf(fd, "%.*s\n", len, word);
	    fclose(fd);

	    home_replace(ખાલી, fname, NameBuff, MAXPATHL, TRUE);
	    smsg(_("Word '%.*s' added to %s"), len, word, NameBuff);
	}
    }

    જો (fd != ખાલી)
    {
	// Update the .add.spl file.
	mkspell(1, &fname, FALSE, TRUE, TRUE);

	// If the .add file is edited somewhere, reload it.
	જો (buf != ખાલી)
	    buf_reload(buf, buf->b_orig_mode, FALSE);

	redraw_all_later(UPD_SOME_VALID);
    }
    vim_free(fnamebuf);
}

/*
 * Initialize 'spellfile' for the current buffer.
 */
    સ્થિર શૂન્ય
init_spellfile(શૂન્ય)
{
    string_T	buf;
    char_u	*fname;
    char_u	*rtp;
    char_u	*lend;
    પૂર્ણાંક		aspath = FALSE;
    char_u	*lstart = curbuf->b_s.b_p_spl;

    જો (*curwin->w_s->b_p_spl == NUL || curwin->w_s->b_langp.ga_len <= 0)
	પાછા;

    buf.string = alloc(MAXPATHL);
    જો (buf.string == ખાલી)
	પાછા;

    buf.length = 0;

    // Find the end of the language name.  Exclude the region.  If there
    // is a path separator remember the start of the tail.
    માટે (lend = curwin->w_s->b_p_spl; *lend != NUL
	    && vim_strchr((char_u *)",._", *lend) == ખાલી; ++lend)
	જો (vim_ispathsep(*lend))
	{
	    aspath = TRUE;
	    lstart = lend + 1;
	}

    // Loop over all entries in 'runtimepath'.  Use the first one where we
    // are allowed to write.
    rtp = p_rtp;
    જ્યારે (*rtp != NUL)
    {
	જો (aspath)
	    // Use directory of an entry with path, e.g., for
	    // "/dir/lg.utf-8.spl" use "/dir".
	    vim_strncpy(buf.string, curbuf->b_s.b_p_spl,
		    lstart - curbuf->b_s.b_p_spl - 1);
	નહીંતર
	    // Copy the path from 'runtimepath' to buf[].
	    buf.length = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");
	જો (filewritable(buf.string) == 2)
	{
	    // Use the first language name from 'spelllang' and the
	    // encoding used in the first loaded .spl file.
	    જો (aspath)
	    {
		buf.length = (size_t)(lend - curbuf->b_s.b_p_spl);
		vim_strncpy(buf.string, curbuf->b_s.b_p_spl, buf.length);
	    }
	    નહીંતર
	    {
		// Create the "spell" directory if it doesn't exist yet.
		buf.length += vim_snprintf_safelen((અક્ષર *)buf.string + buf.length,
		    MAXPATHL - buf.length, "/spell");
		જો (filewritable(buf.string) != 2)
		{
		    જો (vim_mkdir(buf.string, 0755) != 0)
		    {
			vim_free(buf.string);
			પાછા;
		    }
		}

		buf.length += vim_snprintf_safelen((અક્ષર *)buf.string + buf.length,
		    MAXPATHL - buf.length, "/%.*s", (પૂર્ણાંક)(lend - lstart), lstart);
	    }
	    fname = LANGP_ENTRY(curwin->w_s->b_langp, 0)
		->lp_slang->sl_fname;
	    vim_snprintf((અક્ષર *)buf.string + buf.length, MAXPATHL - buf.length,
		".%s.add",
		fname != ખાલી && strstr((અક્ષર *)gettail(fname), ".ascii.") != ખાલી
		    ? (char_u *)"ascii" : spell_enc());
	    set_option_કિંમત_give_err((char_u *)"spellfile",
		    0L, buf.string, OPT_LOCAL);
	    તોડો;
	}
	aspath = FALSE;
    }

    vim_free(buf.string);
}



/*
 * Set the spell character tables from strings in the affix file.
 */
    સ્થિર પૂર્ણાંક
set_spell_chartab(char_u *fol, char_u *નીચલું, char_u *upp)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    char_u	*pf = fol, *pl = નીચલું, *pu = upp;
    પૂર્ણાંક		f, l, u;

    clear_spell_chartab(&new_st);

    જ્યારે (*pf != NUL)
    {
	જો (*pl == NUL || *pu == NUL)
	{
	    emsg(_(e_format_error_in_affix_file_fol_નીચલું_or_upp));
	    પાછા FAIL;
	}
	f = mb_ptr2char_adv(&pf);
	l = mb_ptr2char_adv(&pl);
	u = mb_ptr2char_adv(&pu);

	// Every character that appears is a word character.
	જો (f < 256)
	    new_st.st_isw[f] = TRUE;
	જો (l < 256)
	    new_st.st_isw[l] = TRUE;
	જો (u < 256)
	    new_st.st_isw[u] = TRUE;

	// if "LOW" and "FOL" are not the same the "LOW" char needs
	// case-folding
	જો (l < 256 && l != f)
	{
	    જો (f >= 256)
	    {
		emsg(_(e_character_in_fol_નીચલું_or_upp_is_out_of_range));
		પાછા FAIL;
	    }
	    new_st.st_fold[l] = f;
	}

	// if "UPP" and "FOL" are not the same the "UPP" char needs
	// case-folding, it's upper case and the "UPP" is the upper case of
	// "FOL" .
	જો (u < 256 && u != f)
	{
	    જો (f >= 256)
	    {
		emsg(_(e_character_in_fol_નીચલું_or_upp_is_out_of_range));
		પાછા FAIL;
	    }
	    new_st.st_fold[u] = f;
	    new_st.st_isu[u] = TRUE;
	    new_st.st_upper[f] = u;
	}
    }

    જો (*pl != NUL || *pu != NUL)
    {
	emsg(_(e_format_error_in_affix_file_fol_નીચલું_or_upp));
	પાછા FAIL;
    }

    પાછા set_spell_finish(&new_st);
}

/*
 * Set the spell character tables from strings in the .spl file.
 */
    સ્થિર શૂન્ય
set_spell_charflags(
    char_u	*flags,
    પૂર્ણાંક		cnt,	    // length of "flags"
    char_u	*fol)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    પૂર્ણાંક		i;
    char_u	*p = fol;
    પૂર્ણાંક		c;

    clear_spell_chartab(&new_st);

    માટે (i = 0; i < 128; ++i)
    {
	જો (i < cnt)
	{
	    new_st.st_isw[i + 128] = (flags[i] & CF_WORD) != 0;
	    new_st.st_isu[i + 128] = (flags[i] & CF_UPPER) != 0;
	}

	જો (*p != NUL)
	{
	    c = mb_ptr2char_adv(&p);
	    new_st.st_fold[i + 128] = c;
	    જો (i + 128 != c && new_st.st_isu[i + 128] && c < 256)
		new_st.st_upper[c] = i + 128;
	}
    }

    (શૂન્ય)set_spell_finish(&new_st);
}

    સ્થિર પૂર્ણાંક
set_spell_finish(spelltab_T *new_st)
{
    પૂર્ણાંક		i;

    જો (did_set_spelltab)
    {
	// check that it's the same table
	માટે (i = 0; i < 256; ++i)
	{
	    જો (spelltab.st_isw[i] != new_st->st_isw[i]
		    || spelltab.st_isu[i] != new_st->st_isu[i]
		    || spelltab.st_fold[i] != new_st->st_fold[i]
		    || spelltab.st_upper[i] != new_st->st_upper[i])
	    {
		emsg(_(e_word_characters_differ_between_spell_files));
		પાછા FAIL;
	    }
	}
    }
    નહીંતર
    {
	// copy the new spelltab into the one being used
	spelltab = *new_st;
	did_set_spelltab = TRUE;
    }

    પાછા OK;
}

/*
 * Write the table with prefix conditions to the .spl file.
 * When "fd" is NULL only count the length of what is written and return it.
 */
    સ્થિર પૂર્ણાંક
write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv)
{
    પૂર્ણાંક		i;
    char_u	*p;
    પૂર્ણાંક		len;
    પૂર્ણાંક		totlen;

    જો (fd != ખાલી)
	put_bytes(fd, (long_u)gap->ga_len, 2);	    // <prefcondcnt>

    totlen = 2 + gap->ga_len; // length of <prefcondcnt> and <condlen> bytes

    માટે (i = 0; i < gap->ga_len; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	p = ((char_u **)gap->ga_data)[i];
	જો (p != ખાલી)
	{
	    len = (પૂર્ણાંક)STRLEN(p);
	    જો (fd != ખાલી)
	    {
		fputc(len, fd);
		*fwv &= fwrite(p, (size_t)len, (size_t)1, fd);
	    }
	    totlen += len;
	}
	નહીંતર જો (fd != ખાલી)
	    fputc(0, fd);
    }

    પાછા totlen;
}

/*
 * Use map string "map" for languages "lp".
 */
    સ્થિર શૂન્ય
set_map_str(slang_T *lp, char_u *map)
{
    char_u	*p;
    પૂર્ણાંક		headc = 0;
    પૂર્ણાંક		c;
    પૂર્ણાંક		i;

    જો (*map == NUL)
    {
	lp->sl_has_map = FALSE;
	પાછા;
    }
    lp->sl_has_map = TRUE;

    // Init the array and hash tables empty.
    માટે (i = 0; i < 256; ++i)
	lp->sl_map_array[i] = 0;
    hash_init(&lp->sl_map_hash);

    /*
     * The similar characters are stored separated with slashes:
     * "aaa/bbb/ccc/".  Fill sl_map_array[c] with the character before c and
     * before the same slash.  For characters above 255 sl_map_hash is used.
     */
    માટે (p = map; *p != NUL; )
    {
	c = mb_cptr2char_adv(&p);
	જો (c == '/')
	    headc = 0;
	નહીંતર
	{
	    જો (headc == 0)
		 headc = c;

	    // Characters above 255 don't fit in sl_map_array[], put them in
	    // the hash table.  Each entry is the char, a NUL the headchar and
	    // a NUL.
	    જો (c >= 256)
	    {
		પૂર્ણાંક	    cl = mb_char2len(c);
		પૂર્ણાંક	    headcl = mb_char2len(headc);
		char_u	    *b;
		hash_T	    hash;
		hashitem_T  *hi;

		b = alloc(cl + headcl + 2);
		જો (b == ખાલી)
		    પાછા;
		mb_char2bytes(c, b);
		b[cl] = NUL;
		mb_char2bytes(headc, b + cl + 1);
		b[cl + 1 + headcl] = NUL;
		hash = hash_hash(b);
		hi = hash_lookup(&lp->sl_map_hash, b, hash);
		જો (HASHITEM_EMPTY(hi))
		    hash_add_item(&lp->sl_map_hash, hi, b, hash);
		નહીંતર
		{
		    // This should have been checked when generating the .spl
		    // file.
		    emsg(_(e_duplicate_char_in_map_entry));
		    vim_free(b);
		}
	    }
	    નહીંતર
		lp->sl_map_array[c] = headc;
	}
    }
}

#endif  // FEAT_SPELL
