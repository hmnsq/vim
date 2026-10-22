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

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_SPELL)

#अन्तर्भाव <time.h>	// for time_t

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
#define SN_शब्दगण	13	// common words
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
#define FOR_ALL_गाँठ_SIBLINGS(गाँठ, np) \
    प्रति ((np) = (गाँठ); (np) != रिक्त; (np) = (np)->wn_sibling)

स्थिर पूर्णाङ्क set_spell_finish(spelltab_T	*new_st);
स्थिर पूर्णाङ्क write_spell_prefcond(खाता *fd, garray_T *gap, माप_प्रकार *fwv);
स्थिर पूर्णाङ्क read_region_section(खाता *fd, slang_T *slang, पूर्णाङ्क len);
स्थिर पूर्णाङ्क read_charflags_section(खाता *fd);
स्थिर पूर्णाङ्क read_prefcond_section(खाता *fd, slang_T *lp);
स्थिर पूर्णाङ्क read_rep_section(खाता *fd, garray_T *gap, लघु *first);
स्थिर पूर्णाङ्क read_sal_section(खाता *fd, slang_T *slang);
स्थिर पूर्णाङ्क read_शब्दगण_section(खाता *fd, slang_T *lp, पूर्णाङ्क len);
स्थिर पूर्णाङ्क read_sofo_section(खाता *fd, slang_T *slang);
स्थिर पूर्णाङ्क read_compound(खाता *fd, slang_T *slang, पूर्णाङ्क len);
स्थिर पूर्णाङ्क set_sofo(slang_T *lp, char_u *from, char_u *to);
स्थिर शून्य set_sal_first(slang_T *lp);
स्थिर पूर्णाङ्क *mb_str2wide(char_u *s);
स्थिर पूर्णाङ्क spell_read_वृक्ष(खाता *fd, char_u **bytsp, दीर्घ *bytsp_len, idx_T **idxsp, पूर्णाङ्क prefixtree, पूर्णाङ्क prefixcnt);
स्थिर idx_T read_वृक्ष_गाँठ(खाता *fd, char_u *byts, idx_T *idxs, पूर्णाङ्क maxidx, idx_T startidx, पूर्णाङ्क prefixtree, पूर्णाङ्क maxprefcondnr, पूर्णाङ्क depth);
स्थिर शून्य set_spell_charflags(char_u *flags, पूर्णाङ्क cnt, char_u *upp);
स्थिर पूर्णाङ्क set_spell_chartab(char_u *fol, char_u *निचला, char_u *upp);
स्थिर शून्य set_map_str(slang_T *lp, char_u *map);


स्थिर वर्ण *e_afftrailing = N_("Trailing text in %s line %d: %s");
स्थिर वर्ण *e_affname = N_("Affix name too long in %s line %d: %s");
स्थिर वर्ण *संदेश_compressing = N_("Compressing word tree...");

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
    पूर्णाङ्क		silent)		// no error if file doesn't exist
{
    खाता	*fd;
    char_u	buf[VIMSPELLMAGICL];
    char_u	*p;
    पूर्णाङ्क		i;
    पूर्णाङ्क		n;
    पूर्णाङ्क		len;
    slang_T	*lp = रिक्त;
    पूर्णाङ्क		c = 0;
    पूर्णाङ्क		res;
    पूर्णाङ्क		did_estack_धकेलो = FALSE;
    ESTACK_CHECK_DECLARATION;

    fd = mch_fopen((वर्ण *)fname, "r");
    यदि (fd == रिक्त)
    {
	यदि (!silent)
	    semsg(_(e_cant_open_file_str), fname);
	अन्यथा यदि (p_verbose > 2)
	{
	    verbose_enter();
	    smsg((ध्रुव वर्ण *)e_cant_open_file_str, fname);
	    verbose_leave();
	}
	गच्छ endFAIL;
    }
    यदि (p_verbose > 2)
    {
	verbose_enter();
	smsg(_("Reading spell file \"%s\""), fname);
	verbose_leave();
    }

    यदि (old_lp == रिक्त)
    {
	lp = slang_alloc(lang);
	यदि (lp == रिक्त)
	    गच्छ endFAIL;

	// Remember the file name, used to reload the file when it's updated.
	lp->sl_fname = vim_strsave(fname);
	यदि (lp->sl_fname == रिक्त)
	    गच्छ endFAIL;

	// Check for .add.spl (_add.spl for VMS).
	lp->sl_add = म_माला((वर्ण *)gettail(fname), SPL_FNAME_ADD) != रिक्त;
    }
    अन्यथा
	lp = old_lp;

    // Set sourcing_name, so that error messages mention the file name.
    estack_धकेलो(ETYPE_SPELL, fname, 0);
    ESTACK_CHECK_SETUP;
    did_estack_धकेलो = TRUE;

    /*
     * <HEADER>: <fileID>
     */
    प्रति (i = 0; i < VIMSPELLMAGICL; ++i)
	buf[i] = (c = अ_लो(fd)) == खातापूर्ण ? 0 : c;		// <fileID>
    यदि (STRNCMP(buf, VIMSPELLMAGIC, VIMSPELLMAGICL) != 0)
    {
	emsg(_(e_this_does_not_look_like_spell_file));
	गच्छ endFAIL;
    }
    c = अ_लो(fd);					// <versionnr>
    यदि (c < VIMSPELLVERSION)
    {
	emsg(_(e_old_spell_file_needs_to_be_updated));
	गच्छ endFAIL;
    }
    अन्यथा यदि (c > VIMSPELLVERSION)
    {
	emsg(_(e_spell_file_is_for_newer_version_of_vim));
	गच्छ endFAIL;
    }


    /*
     * <SECTIONS>: <section> ... <sectionend>
     * <section>: <sectionID> <sectionflags> <sectionlen> (section contents)
     */
    प्रति (;;)
    {
	n = अ_लो(fd);			    // <sectionID> or <sectionend>
	यदि (n == SN_END)
	    भङ्ग;
	c = अ_लो(fd);					// <sectionflags>
	len = get4c(fd);				// <sectionlen>
	यदि (len < 0)
	    गच्छ truncerr;

	res = 0;
	विकल्प (n)
	{
	    पक्ष SN_INFO:
		lp->sl_info = read_string(fd, len);	// <infotext>
		यदि (lp->sl_info == रिक्त)
		    गच्छ endFAIL;
		भङ्ग;

	    पक्ष SN_REGION:
		res = read_region_section(fd, lp, len);
		भङ्ग;

	    पक्ष SN_CHARFLAGS:
		res = read_charflags_section(fd);
		भङ्ग;

	    पक्ष SN_MIDWORD:
		lp->sl_midword = read_string(fd, len);	// <midword>
		यदि (lp->sl_midword == रिक्त)
		    गच्छ endFAIL;
		भङ्ग;

	    पक्ष SN_PREFCOND:
		res = read_prefcond_section(fd, lp);
		भङ्ग;

	    पक्ष SN_REP:
		res = read_rep_section(fd, &lp->sl_rep, lp->sl_rep_first);
		भङ्ग;

	    पक्ष SN_REPSAL:
		res = read_rep_section(fd, &lp->sl_repsal, lp->sl_repsal_first);
		भङ्ग;

	    पक्ष SN_SAL:
		res = read_sal_section(fd, lp);
		भङ्ग;

	    पक्ष SN_SOFO:
		res = read_sofo_section(fd, lp);
		भङ्ग;

	    पक्ष SN_MAP:
		p = read_string(fd, len);		// <mapstr>
		यदि (p == रिक्त)
		    गच्छ endFAIL;
		set_map_str(lp, p);
		vim_free(p);
		भङ्ग;

	    पक्ष SN_शब्दगण:
		res = read_शब्दगण_section(fd, lp, len);
		भङ्ग;

	    पक्ष SN_SUGFILE:
		lp->sl_sugtime = get8ctime(fd);		// <timestamp>
		भङ्ग;

	    पक्ष SN_NOSPLITSUGS:
		lp->sl_nosplitsugs = TRUE;
		भङ्ग;

	    पक्ष SN_NOCOMPOUNDSUGS:
		lp->sl_nocompoundsugs = TRUE;
		भङ्ग;

	    पक्ष SN_COMPOUND:
		res = read_compound(fd, lp, len);
		भङ्ग;

	    पक्ष SN_NOBREAK:
		lp->sl_nobreak = TRUE;
		भङ्ग;

	    पक्ष SN_SYLLABLE:
		lp->sl_syllable = read_string(fd, len);	// <syllable>
		यदि (lp->sl_syllable == रिक्त)
		    गच्छ endFAIL;
		यदि (init_syl_tab(lp) != OK)
		    गच्छ endFAIL;
		भङ्ग;

	    स्वभाव:
		// Unsupported section.  When it's required give an error
		// message.  When it's not required skip the contents.
		यदि (c & SNF_REQUIRED)
		{
		    emsg(_(e_unsupported_section_in_spell_file));
		    गच्छ endFAIL;
		}
		यावत् (--len >= 0)
		    यदि (अ_लो(fd) < 0)
			गच्छ truncerr;
		भङ्ग;
	}
someerror:
	यदि (res == SP_FORMERROR)
	{
	    emsg(_(e_format_error_in_spell_file));
	    गच्छ endFAIL;
	}
	यदि (res == SP_TRUNCERROR)
	{
truncerr:
	    emsg(_(e_truncated_spell_file));
	    गच्छ endFAIL;
	}
	यदि (res == SP_OTHERERROR)
	    गच्छ endFAIL;
    }

    // <LWORDTREE>
    res = spell_read_वृक्ष(fd, &lp->sl_fbyts, &lp->sl_fbyts_len,
						      &lp->sl_fidxs, FALSE, 0);
    यदि (res != 0)
	गच्छ someerror;

    // <KWORDTREE>
    res = spell_read_वृक्ष(fd, &lp->sl_kbyts, रिक्त, &lp->sl_kidxs, FALSE, 0);
    यदि (res != 0)
	गच्छ someerror;

    // <PREFIXTREE>
    res = spell_read_वृक्ष(fd, &lp->sl_pbyts, रिक्त, &lp->sl_pidxs, TRUE,
							    lp->sl_prefixcnt);
    यदि (res != 0)
	गच्छ someerror;

    // For a new file link it in the list of spell files.
    यदि (old_lp == रिक्त && lang != रिक्त)
    {
	lp->sl_next = first_lang;
	first_lang = lp;
    }

    गच्छ endOK;

endFAIL:
    यदि (lang != रिक्त)
	// truncating the name signals the error to spell_load_lang()
	*lang = NUL;
    यदि (lp != रिक्त && old_lp == रिक्त)
	slang_free(lp);
    lp = रिक्त;

endOK:
    यदि (fd != रिक्त)
	ख_बंद(fd);
    यदि (did_estack_धकेलो)
    {
	ESTACK_CHECK_NOW;
	estack_निकालो();
    }

    प्रत्यागम lp;
}

/*
 * Fill in the wordcount fields for a trie.
 * Returns the total number of words.
 */
    स्थिर शून्य
वृक्ष_गिनती_शब्दगण(char_u *byts, दीर्घ byts_len, idx_T *idxs)
{
    पूर्णाङ्क		depth;
    idx_T	arridx[MAXWLEN];
    पूर्णाङ्क		curi[MAXWLEN];
    पूर्णाङ्क		c;
    idx_T	n;
    पूर्णाङ्क		wordcount[MAXWLEN];

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;
    depth = 0;
    यावत् (depth >= 0 && !got_int)
    {
	यदि (curi[depth] > byts[arridx[depth]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[depth]] = wordcount[depth];
	    यदि (depth > 0)
		wordcount[depth - 1] += wordcount[depth];

	    --depth;
	    fast_breakcheck();
	}
	अन्यथा
	{
	    // Do one more byte at this node.
	    n = arridx[depth] + curi[depth];
	    ++curi[depth];

	    c = byts[n];
	    यदि (c == 0)
	    {
		// End of word, count it.
		++wordcount[depth];

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		यावत् (n + 1 < byts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[depth];
		}
	    }
	    अन्यथा यदि (depth < MAXWLEN - 1)
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
    शून्य
suggest_load_files(शून्य)
{
    langp_T	*lp;
    पूर्णाङ्क		lpi;
    slang_T	*slang;
    char_u	*dotp;
    खाता	*fd;
    char_u	buf[MAXWLEN];
    पूर्णाङ्क		i;
    समय_प्रकार	timestamp;
    पूर्णाङ्क		wcount;
    पूर्णाङ्क		wordnr;
    garray_T	ga;
    पूर्णाङ्क		c;

    // Do this for all languages that support sound folding.
    प्रति (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	यदि (slang->sl_sugtime != 0 && !slang->sl_sugloaded)
	{
	    // Change ".spl" to ".sug" and open the file.  When the file isn't
	    // found silently skip it.  Do set "sl_sugloaded" so that we
	    // don't try again and again.
	    slang->sl_sugloaded = TRUE;

	    dotp = vim_strrchr(slang->sl_fname, '.');
	    यदि (dotp == रिक्त || fnamecmp(dotp, ".spl") != 0)
		अनुवर्त;
	    STRCPY(dotp, ".sug");
	    fd = mch_fopen((वर्ण *)slang->sl_fname, "r");
	    यदि (fd == रिक्त)
		गच्छ nextone;

	    /*
	     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
	     */
	    प्रति (i = 0; i < VIMSUGMAGICL; ++i)
		buf[i] = (c = अ_लो(fd)) == खातापूर्ण ? 0 : c;	// <fileID>
	    यदि (STRNCMP(buf, VIMSUGMAGIC, VIMSUGMAGICL) != 0)
	    {
		semsg(_(e_this_does_not_look_like_sug_file_str),
							     slang->sl_fname);
		गच्छ nextone;
	    }
	    c = अ_लो(fd);				// <versionnr>
	    यदि (c < VIMSUGVERSION)
	    {
		semsg(_(e_old_sug_file_needs_to_be_updated_str),
							     slang->sl_fname);
		गच्छ nextone;
	    }
	    अन्यथा यदि (c > VIMSUGVERSION)
	    {
		semsg(_(e_sug_file_is_for_newer_version_of_vim_str),
							     slang->sl_fname);
		गच्छ nextone;
	    }

	    // Check the timestamp, it must be exactly the same as the one in
	    // the .spl file.  Otherwise the word numbers won't match.
	    timestamp = get8ctime(fd);			// <timestamp>
	    यदि (timestamp != slang->sl_sugtime)
	    {
		semsg(_(e_sug_file_doesnt_match_spl_file_str),
							     slang->sl_fname);
		गच्छ nextone;
	    }

	    /*
	     * <SUGWORDTREE>: <wordtree>
	     * Read the trie with the soundfolded words.
	     */
	    यदि (spell_read_वृक्ष(fd, &slang->sl_sbyts, &slang->sl_sbyts_len,
			&slang->sl_sidxs, FALSE, 0) != 0)
	    {
someerror:
		semsg(_(e_error_while_reading_sug_file_str),
							     slang->sl_fname);
		slang_clear_sug(slang);
		गच्छ nextone;
	    }

	    /*
	     * <SUGTABLE>: <sugwcount> <sugline> ...
	     *
	     * Read the table with word numbers.  We use a file buffer for
	     * this, because it's so much like a file with lines.  Makes it
	     * possible to swap the info and save on memory use.
	     */
	    slang->sl_sugbuf = open_spellbuf();
	    यदि (slang->sl_sugbuf == रिक्त)
		गच्छ someerror;
							    // <sugwcount>
	    wcount = get4c(fd);
	    यदि (wcount < 0)
		गच्छ someerror;

	    // Read all the wordnr lists into the buffer, one NUL terminated
	    // list per line.
	    ga_init2(&ga, 1, 100);
	    प्रति (wordnr = 0; wordnr < wcount; ++wordnr)
	    {
		ga.ga_len = 0;
		प्रति (;;)
		{
		    c = अ_लो(fd);			    // <sugline>
		    यदि (c < 0 || ga_grow(&ga, 1) == FAIL)
			गच्छ someerror;
		    ((char_u *)ga.ga_data)[ga.ga_len++] = c;
		    यदि (c == NUL)
			भङ्ग;
		}
		यदि (ml_append_buf(slang->sl_sugbuf, (linenr_T)wordnr,
					 ga.ga_data, ga.ga_len, TRUE) == FAIL)
		    गच्छ someerror;
	    }
	    ga_clear(&ga);

	    /*
	     * Need to put word counts in the word tries, so that we can find
	     * a word by its number.
	     */
	    वृक्ष_गिनती_शब्दगण(slang->sl_fbyts, slang->sl_fbyts_len,
		    slang->sl_fidxs);
	    वृक्ष_गिनती_शब्दगण(slang->sl_sbyts, slang->sl_sbyts_len,
		    slang->sl_sidxs);

nextone:
	    यदि (fd != रिक्त)
		ख_बंद(fd);
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
    स्थिर char_u *
read_cnt_string(खाता *fd, पूर्णाङ्क cnt_bytes, पूर्णाङ्क *cntp)
{
    पूर्णाङ्क		cnt = 0;
    पूर्णाङ्क		i;
    char_u	*str;

    // read the length bytes, MSB first
    प्रति (i = 0; i < cnt_bytes; ++i)
    {
	पूर्णाङ्क c = अ_लो(fd);

	यदि (c == खातापूर्ण)
	{
	    *cntp = SP_TRUNCERROR;
	    प्रत्यागम रिक्त;
	}
	cnt = (cnt << 8) + (अचिन्हित)c;
    }
    *cntp = cnt;
    यदि (cnt == 0)
	प्रत्यागम रिक्त;	    // nothing to read, return NULL

    str = read_string(fd, cnt);
    यदि (str == रिक्त)
	*cntp = SP_OTHERERROR;
    प्रत्यागम str;
}

/*
 * Read SN_REGION: <regionname> ...
 * Return SP_*ERROR flags.
 */
    स्थिर पूर्णाङ्क
read_region_section(खाता *fd, slang_T *lp, पूर्णाङ्क len)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		c = 0;

    यदि (len > MAXREGIONS * 2)
	प्रत्यागम SP_FORMERROR;
    प्रति (i = 0; i < len; ++i)
	lp->sl_regions[i] = (c = अ_लो(fd)) == खातापूर्ण ? 0 : c; // <regionname>
    lp->sl_regions[len] = NUL;
    प्रत्यागम c == खातापूर्ण ? SP_TRUNCERROR : 0;
}

/*
 * Read SN_CHARFLAGS section: <charflagslen> <charflags>
 *				<folcharslen> <folchars>
 * Return SP_*ERROR flags.
 */
    स्थिर पूर्णाङ्क
read_charflags_section(खाता *fd)
{
    char_u	*flags;
    char_u	*fol;
    पूर्णाङ्क		flagslen, follen;

    // <charflagslen> <charflags>
    flags = read_cnt_string(fd, 1, &flagslen);
    यदि (flagslen < 0)
	प्रत्यागम flagslen;

    // <folcharslen> <folchars>
    fol = read_cnt_string(fd, 2, &follen);
    यदि (follen < 0)
    {
	vim_free(flags);
	प्रत्यागम follen;
    }

    // Set the word-char flags and fill SPELL_ISUPPER() table.
    यदि (flags != रिक्त && fol != रिक्त)
	set_spell_charflags(flags, flagslen, fol);

    vim_free(flags);
    vim_free(fol);

    // When <charflagslen> is zero then <fcharlen> must also be zero.
    यदि ((flags == रिक्त) != (fol == रिक्त))
	प्रत्यागम SP_FORMERROR;
    प्रत्यागम 0;
}

/*
 * Read SN_PREFCOND section.
 * Return SP_*ERROR flags.
 */
    स्थिर पूर्णाङ्क
read_prefcond_section(खाता *fd, slang_T *lp)
{
    पूर्णाङ्क		cnt;
    पूर्णाङ्क		i;
    पूर्णाङ्क		n;
    पूर्णाङ्क		c;
    char_u	*p;
    char_u	buf[MAXWLEN + 1];

    // <prefcondcnt> <prefcond> ...
    cnt = get2c(fd);					// <prefcondcnt>
    यदि (cnt <= 0)
	प्रत्यागम SP_FORMERROR;

    lp->sl_prefprog = ALLOC_CLEAR_MULT(regprog_T *, cnt);
    यदि (lp->sl_prefprog == रिक्त)
	प्रत्यागम SP_OTHERERROR;
    lp->sl_prefixcnt = cnt;

    प्रति (i = 0; i < cnt; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	n = अ_लो(fd);					// <condlen>
	यदि (n < 0 || n >= MAXWLEN)
	    प्रत्यागम SP_FORMERROR;

	// When <condlen> is zero we have an empty condition.  Otherwise
	// compile the regexp program used to check for the condition.
	यदि (n > 0)
	{
	    buf[0] = '^';	    // always match at one position only
	    p = buf + 1;
	    यावत् (n-- > 0)
		*p++ = (c = अ_लो(fd)) == खातापूर्ण ? 0 : c;	    // <condstr>
	    यदि (c == खातापूर्ण)
		भङ्ग;
	    *p = NUL;
	    lp->sl_prefprog[i] = vim_regcomp(buf, RE_MAGIC + RE_STRING);
	}
    }
    प्रत्यागम 0;
}

/*
 * Read REP or REPSAL items section from "fd": <repcount> <rep> ...
 * Return SP_*ERROR flags.
 */
    स्थिर पूर्णाङ्क
read_rep_section(खाता *fd, garray_T *gap, लघु *first)
{
    पूर्णाङ्क		cnt;
    fromto_T	*ftp;
    पूर्णाङ्क		i;

    cnt = get2c(fd);					// <repcount>
    यदि (cnt < 0)
	प्रत्यागम SP_TRUNCERROR;

    यदि (ga_grow(gap, cnt) == FAIL)
	प्रत्यागम SP_OTHERERROR;

    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
    प्रति (; gap->ga_len < cnt; ++gap->ga_len)
    {
	ftp = &((fromto_T *)gap->ga_data)[gap->ga_len];
	ftp->ft_from = read_cnt_string(fd, 1, &i);
	यदि (i < 0)
	    प्रत्यागम i;
	यदि (i == 0)
	    प्रत्यागम SP_FORMERROR;
	ftp->ft_to = read_cnt_string(fd, 1, &i);
	यदि (i <= 0)
	{
	    vim_free(ftp->ft_from);
	    यदि (i < 0)
		प्रत्यागम i;
	    प्रत्यागम SP_FORMERROR;
	}
    }

    // Fill the first-index table.
    प्रति (i = 0; i < 256; ++i)
	first[i] = -1;
    प्रति (i = 0; i < gap->ga_len; ++i)
    {
	ftp = &((fromto_T *)gap->ga_data)[i];
	यदि (first[*ftp->ft_from] == -1)
	    first[*ftp->ft_from] = i;
    }
    प्रत्यागम 0;
}

/*
 * Read SN_SAL section: <salflags> <salcount> <sal> ...
 * Return SP_*ERROR flags.
 */
    स्थिर पूर्णाङ्क
read_sal_section(खाता *fd, slang_T *slang)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		cnt;
    garray_T	*gap;
    salitem_T	*smp;
    पूर्णाङ्क		ccnt;
    char_u	*p;

    slang->sl_sofo = FALSE;

    i = अ_लो(fd);				// <salflags>
    यदि (i & SAL_F0LLOWUP)
	slang->sl_followup = TRUE;
    यदि (i & SAL_COLLAPSE)
	slang->sl_collapse = TRUE;
    यदि (i & SAL_REM_ACCENTS)
	slang->sl_rem_accents = TRUE;

    cnt = get2c(fd);				// <salcount>
    यदि (cnt < 0)
	प्रत्यागम SP_TRUNCERROR;

    gap = &slang->sl_sal;
    ga_init2(gap, परिमाण(salitem_T), 10);
    यदि (ga_grow(gap, cnt + 1) == FAIL)
	प्रत्यागम SP_OTHERERROR;

    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
    प्रति (; gap->ga_len < cnt; ++gap->ga_len)
    {
	पूर्णाङ्क	c = NUL;

	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	ccnt = अ_लो(fd);			// <salfromlen>
	यदि (ccnt < 0)
	    प्रत्यागम SP_TRUNCERROR;
	यदि ((p = alloc(ccnt + 2)) == रिक्त)
	    प्रत्यागम SP_OTHERERROR;
	smp->sm_lead = p;

	// Read up to the first special char into sm_lead.
	प्रति (i = 0; i < ccnt; ++i)
	{
	    c = अ_लो(fd);			// <salfrom>
	    यदि (vim_strchr((char_u *)"0123456789(-<^$", c) != रिक्त)
		भङ्ग;
	    *p++ = c;
	}
	smp->sm_leadlen = (पूर्णाङ्क)(p - smp->sm_lead);
	*p++ = NUL;

	// Put (abc) chars in sm_oneof, if any.
	यदि (c == '(')
	{
	    smp->sm_oneof = p;
	    प्रति (++i; i < ccnt; ++i)
	    {
		c = अ_लो(fd);			// <salfrom>
		यदि (c == ')')
		    भङ्ग;
		*p++ = c;
	    }
	    *p++ = NUL;
	    यदि (++i < ccnt)
		c = अ_लो(fd);
	}
	अन्यथा
	    smp->sm_oneof = रिक्त;

	// Any following chars go in sm_rules.
	smp->sm_rules = p;
	यदि (i < ccnt)
	    // store the char we got while checking for end of sm_lead
	    *p++ = c;
	प्रति (++i; i < ccnt; ++i)
	    *p++ = (c = अ_लो(fd)) == खातापूर्ण ? 0 : c;	// <salfrom>
	*p++ = NUL;

	// <saltolen> <salto>
	smp->sm_to = read_cnt_string(fd, 1, &ccnt);
	यदि (ccnt < 0)
	{
	    vim_free(smp->sm_lead);
	    प्रत्यागम ccnt;
	}

	यदि (has_mbyte)
	{
	    // convert the multi-byte strings to wide char strings
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = mb_charlen(smp->sm_lead);
	    यदि (smp->sm_oneof == रिक्त)
		smp->sm_oneof_w = रिक्त;
	    अन्यथा
		smp->sm_oneof_w = mb_str2wide(smp->sm_oneof);
	    यदि (smp->sm_to == रिक्त)
		smp->sm_to_w = रिक्त;
	    अन्यथा
		smp->sm_to_w = mb_str2wide(smp->sm_to);
	    यदि (smp->sm_lead_w == रिक्त
		    || (smp->sm_oneof_w == रिक्त && smp->sm_oneof != रिक्त)
		    || (smp->sm_to_w == रिक्त && smp->sm_to != रिक्त))
	    {
		vim_free(smp->sm_lead);
		vim_free(smp->sm_to);
		vim_free(smp->sm_lead_w);
		vim_free(smp->sm_oneof_w);
		vim_free(smp->sm_to_w);
		प्रत्यागम SP_OTHERERROR;
	    }
	}
    }

    यदि (gap->ga_len > 0)
    {
	// Add one extra entry to mark the end with an empty sm_lead.  Avoids
	// that we need to check the index every time.
	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	यदि ((p = alloc(1)) == रिक्त)
	    प्रत्यागम SP_OTHERERROR;
	p[0] = NUL;
	smp->sm_lead = p;
	smp->sm_leadlen = 0;
	smp->sm_oneof = रिक्त;
	smp->sm_rules = p;
	smp->sm_to = रिक्त;
	यदि (has_mbyte)
	{
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = 0;
	    smp->sm_oneof_w = रिक्त;
	    smp->sm_to_w = रिक्त;
	}
	++gap->ga_len;
    }

    // Fill the first-index table.
    set_sal_first(slang);

    प्रत्यागम 0;
}

/*
 * Read SN_WORDS: <word> ...
 * Return SP_*ERROR flags.
 */
    स्थिर पूर्णाङ्क
read_शब्दगण_section(खाता *fd, slang_T *lp, पूर्णाङ्क len)
{
    पूर्णाङ्क		done = 0;
    पूर्णाङ्क		i;
    पूर्णाङ्क		c;
    char_u	word[MAXWLEN];

    यावत् (done < len)
    {
	// Read one word at a time.
	प्रति (i = 0; ; ++i)
	{
	    c = अ_लो(fd);
	    यदि (c == खातापूर्ण)
		प्रत्यागम SP_TRUNCERROR;
	    word[i] = c;
	    यदि (word[i] == NUL)
		भङ्ग;
	    यदि (i == MAXWLEN - 1)
		प्रत्यागम SP_FORMERROR;
	}

	// Init the count to 10.
	गिनती_common_word(lp, word, -1, 10);
	done += i + 1;
    }
    प्रत्यागम 0;
}

/*
 * SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
 * Return SP_*ERROR flags.
 */
    स्थिर पूर्णाङ्क
read_sofo_section(खाता *fd, slang_T *slang)
{
    पूर्णाङ्क		cnt;
    char_u	*from, *to;
    पूर्णाङ्क		res;

    // <sofofromlen> <sofofrom>
    from = read_cnt_string(fd, 2, &cnt);
    यदि (cnt < 0)
	प्रत्यागम cnt;

    // <sofotolen> <sofoto>
    to = read_cnt_string(fd, 2, &cnt);
    यदि (cnt < 0)
    {
	vim_free(from);
	प्रत्यागम cnt;
    }

    // Store the info in slang->sl_sal and/or slang->sl_sal_first.
    यदि (from != रिक्त && to != रिक्त)
	res = set_sofo(slang, from, to);
    अन्यथा यदि (from != रिक्त || to != रिक्त)
	res = SP_FORMERROR;    // only one of two strings is an error
    अन्यथा
	res = 0;

    vim_free(from);
    vim_free(to);
    प्रत्यागम res;
}

/*
 * Read the compound section from the .spl file:
 *	<compmax> <compminlen> <compsylmax> <compoptions> <compflags>
 * Returns SP_*ERROR flags.
 */
    स्थिर पूर्णाङ्क
read_compound(खाता *fd, slang_T *slang, पूर्णाङ्क len)
{
    पूर्णाङ्क		todo = len;
    पूर्णाङ्क		c;
    पूर्णाङ्क		atstart;
    char_u	*pat;
    char_u	*pp;
    char_u	*cp;
    char_u	*ap;
    char_u	*crp;
    पूर्णाङ्क		cnt;
    garray_T	*gap;
    माप_प्रकार	patsize;
    माप_प्रकार	flagsize;

    यदि (todo < 2)
	प्रत्यागम SP_FORMERROR;	// need at least two bytes

    --todo;
    c = अ_लो(fd);					// <compmax>
    यदि (c < 2)
	c = MAXWLEN;
    slang->sl_compmax = c;

    --todo;
    c = अ_लो(fd);					// <compminlen>
    यदि (c < 1)
	c = 0;
    slang->sl_compminlen = c;

    --todo;
    c = अ_लो(fd);					// <compsylmax>
    यदि (c < 1)
	c = MAXWLEN;
    slang->sl_compsylmax = c;

    c = अ_लो(fd);					// <compoptions>
    यदि (c != 0)
	अक्षर_वापस(c, fd);	    // be backwards compatible with Vim 7.0b
    अन्यथा
    {
	--todo;
	c = अ_लो(fd);	    // only use the lower byte for now
	--todo;
	slang->sl_compoptions = c;

	gap = &slang->sl_comppat;
	c = get2c(fd);					// <comppatcount>
	यदि (c < 0)
	    प्रत्यागम SP_TRUNCERROR;
	todo -= 2;
	ga_init2(gap, परिमाण(char_u *), c);
	यदि (ga_grow(gap, c) == OK)
	    यावत् (--c >= 0)
	    {
		((char_u **)(gap->ga_data))[gap->ga_len++] =
						  read_cnt_string(fd, 1, &cnt);
					    // <comppatlen> <comppattext>
		यदि (cnt < 0)
		    प्रत्यागम cnt;
		todo -= cnt + 1;
	    }
    }
    यदि (todo < 0)
	प्रत्यागम SP_FORMERROR;

    // Turn the COMPOUNDRULE items into a regexp pattern:
    // "a[bc]/a*b+" -> "^\(a[bc]\|a*b\+\)$".
    // Inserting backslashes may double the length, "^\(\)$<Nul>" is 7 bytes.
    // Conversion to utf-8 may double the size.
    यदि ((माप_प्रकार)todo > COMPOUND_MAX_LEN)
	प्रत्यागम SP_FORMERROR;
    patsize = (माप_प्रकार)todo * 2 + 7;
    यदि (enc_utf8)
	patsize += (माप_प्रकार)todo * 2;
    flagsize = (माप_प्रकार)todo + 1;
    pat = alloc(patsize);
    यदि (pat == रिक्त)
	प्रत्यागम SP_OTHERERROR;

    // We also need a list of all flags that can appear at the start and one
    // for all flags.
    cp = alloc(flagsize);
    यदि (cp == रिक्त)
    {
	vim_free(pat);
	प्रत्यागम SP_OTHERERROR;
    }
    slang->sl_compstartflags = cp;
    *cp = NUL;

    ap = alloc(flagsize);
    यदि (ap == रिक्त)
    {
	vim_free(pat);
	प्रत्यागम SP_OTHERERROR;
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
    यावत् (todo-- > 0)
    {
	c = अ_लो(fd);					// <compflags>
	यदि (c == खातापूर्ण)
	{
	    vim_free(pat);
	    प्रत्यागम SP_TRUNCERROR;
	}

	// Add all flags to "sl_compallflags".
	यदि (vim_strchr((char_u *)"?*+[]/", c) == रिक्त
		&& !byte_in_str(slang->sl_compallflags, c))
	{
	    *ap++ = c;
	    *ap = NUL;
	}

	यदि (atstart != 0)
	{
	    // At start of item: copy flags to "sl_compstartflags".  For a
	    // [abc] item set "atstart" to 2 and copy up to the ']'.
	    यदि (c == '[')
		atstart = 2;
	    अन्यथा यदि (c == ']')
		atstart = 0;
	    अन्यथा
	    {
		यदि (!byte_in_str(slang->sl_compstartflags, c))
		{
		    *cp++ = c;
		    *cp = NUL;
		}
		यदि (atstart == 1)
		    atstart = 0;
	    }
	}

	// Copy flag to "sl_comprules", unless we run into a wildcard.
	यदि (crp != रिक्त)
	{
	    यदि (c == '?' || c == '+' || c == '*')
	    {
		VIM_CLEAR(slang->sl_comprules);
		crp = रिक्त;
	    }
	    अन्यथा
		*crp++ = c;
	}

	यदि (c == '/')	    // slash separates two items
	{
	    *pp++ = '\\';
	    *pp++ = '|';
	    atstart = 1;
	}
	अन्यथा		    // normal char, "[abc]" and '*' are copied as-is
	{
	    यदि (c == '?' || c == '+' || c == '~')
		*pp++ = '\\';	    // "a?" becomes "a\?", "a+" becomes "a\+"
	    यदि (enc_utf8)
		pp += mb_char2bytes(c, pp);
	    अन्यथा
		*pp++ = c;
	}
    }

    *pp++ = '\\';
    *pp++ = ')';
    *pp++ = '$';
    *pp = NUL;

    यदि (crp != रिक्त)
	*crp = NUL;

    slang->sl_compprog = vim_regcomp(pat, RE_MAGIC + RE_STRING + RE_STRICT);
    vim_free(pat);
    यदि (slang->sl_compprog == रिक्त)
	प्रत्यागम SP_FORMERROR;

    प्रत्यागम 0;
}

/*
 * Set the SOFOFROM and SOFOTO items in language "lp".
 * Returns SP_*ERROR flags when there is something wrong.
 */
    स्थिर पूर्णाङ्क
set_sofo(slang_T *lp, char_u *from, char_u *to)
{
    पूर्णाङ्क		i;

    garray_T	*gap;
    char_u	*s;
    char_u	*p;
    पूर्णाङ्क		c;
    पूर्णाङ्क		*inp;

    यदि (has_mbyte)
    {
	free_sal_वस्तुएँ(&lp->sl_sal);

	// Use "sl_sal" as an array with 256 pointers to a list of wide
	// characters.  The index is the low byte of the character.
	// The list contains from-to pairs with a terminating NUL.
	// sl_sal_first[] is used for latin1 "from" characters.
	gap = &lp->sl_sal;
	ga_init2(gap, परिमाण(पूर्णाङ्क *), 1);
	यदि (ga_grow(gap, 256) == FAIL)
	    प्रत्यागम SP_OTHERERROR;
	vim_memset(gap->ga_data, 0, परिमाण(पूर्णाङ्क *) * 256);
	gap->ga_len = 256;
	lp->sl_sofo = TRUE;

	// First count the number of items for each list.  Temporarily use
	// sl_sal_first[] for this.  Reset it first: a preceding SN_SAL section
	// may have set the entries to -1 via set_sal_first().
	vim_memset(lp->sl_sal_first, 0, परिमाण(salfirst_T) * 256);
	प्रति (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    MB_CPTR_ADV(s);
	    यदि (c >= 256)
		++lp->sl_sal_first[c & 0xff];
	}
	यदि (*p != NUL || *s != NUL)	    // lengths differ
	    प्रत्यागम SP_FORMERROR;

	// Allocate the lists.
	प्रति (i = 0; i < 256; ++i)
	    यदि (lp->sl_sal_first[i] > 0)
	    {
		p = alloc(परिमाण(पूर्णाङ्क) * (lp->sl_sal_first[i] * 2 + 1));
		यदि (p == रिक्त)
		    प्रत्यागम SP_OTHERERROR;
		((पूर्णाङ्क **)gap->ga_data)[i] = (पूर्णाङ्क *)p;
		*(पूर्णाङ्क *)p = 0;
	    }

	// Put the characters up to 255 in sl_sal_first[] the rest in a sl_sal
	// list.
	vim_memset(lp->sl_sal_first, 0, परिमाण(salfirst_T) * 256);
	प्रति (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    i = mb_cptr2char_adv(&s);
	    यदि (c >= 256)
	    {
		// Append the from-to chars at the end of the list with
		// the low byte.
		inp = ((पूर्णाङ्क **)gap->ga_data)[c & 0xff];
		यावत् (*inp != 0)
		    ++inp;
		*inp++ = c;		// from char
		*inp++ = i;		// to char
		*inp++ = NUL;		// NUL at the end
	    }
	    अन्यथा
		// mapping byte to char is done in sl_sal_first[]
		lp->sl_sal_first[c] = i;
	}
    }
    अन्यथा
    {
	// mapping bytes to bytes is done in sl_sal_first[]
	यदि (STRLEN(from) != STRLEN(to))
	    प्रत्यागम SP_FORMERROR;

	प्रति (i = 0; to[i] != NUL; ++i)
	    lp->sl_sal_first[from[i]] = to[i];
	lp->sl_sal.ga_len = 1;		// indicates we have soundfolding
	lp->sl_sofo = TRUE;
    }

    प्रत्यागम 0;
}

/*
 * Fill the first-index table for "lp".
 */
    स्थिर शून्य
set_sal_first(slang_T *lp)
{
    salfirst_T	*sfirst;
    पूर्णाङ्क		i;
    salitem_T	*smp;
    पूर्णाङ्क		c;
    garray_T	*gap = &lp->sl_sal;

    sfirst = lp->sl_sal_first;
    प्रति (i = 0; i < 256; ++i)
	sfirst[i] = -1;
    smp = (salitem_T *)gap->ga_data;
    प्रति (i = 0; i < gap->ga_len; ++i)
    {
	यदि (has_mbyte)
	    // Use the lowest byte of the first character.  For latin1 it's
	    // the character, for other encodings it should differ for most
	    // characters.
	    c = *smp[i].sm_lead_w & 0xff;
	अन्यथा
	    c = *smp[i].sm_lead;
	यदि (sfirst[c] == -1)
	{
	    sfirst[c] = i;
	    यदि (has_mbyte)
	    {
		पूर्णाङ्क		n;

		// Make sure all entries with this byte are following each
		// other.  Move the ones that are in the wrong position.  Do
		// keep the same ordering!
		यावत् (i + 1 < gap->ga_len
				       && (*smp[i + 1].sm_lead_w & 0xff) == c)
		    // Skip over entry with same index byte.
		    ++i;

		प्रति (n = 1; i + n < gap->ga_len; ++n)
		    यदि ((*smp[i + n].sm_lead_w & 0xff) == c)
		    {
			salitem_T  tsal;

			// Move entry with same index byte after the entries
			// we already found.
			++i;
			--n;
			tsal = smp[i + n];
			mch_memmove(smp + i + 1, smp + i,
						       परिमाण(salitem_T) * n);
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
    स्थिर पूर्णाङ्क *
mb_str2wide(char_u *s)
{
    पूर्णाङ्क		*res;
    char_u	*p;
    पूर्णाङ्क		i = 0;

    res = ALLOC_MULT(पूर्णाङ्क, mb_charlen(s) + 1);
    यदि (res == रिक्त)
	प्रत्यागम रिक्त;

    प्रति (p = s; *p != NUL; )
	res[i++] = mb_ptr2char_adv(&p);
    res[i] = NUL;
    प्रत्यागम res;
}

/*
 * Read a tree from the .spl or .sug file.
 * Allocates the memory and stores pointers in "bytsp" and "idxsp".
 * This is skipped when the tree has zero length.
 * Returns zero when OK, SP_ value for an error.
 */
    स्थिर पूर्णाङ्क
spell_read_वृक्ष(
    खाता	*fd,
    char_u	**bytsp,
    दीर्घ	*bytsp_len,
    idx_T	**idxsp,
    पूर्णाङ्क		prefixtree,	// TRUE for the prefix tree
    पूर्णाङ्क		prefixcnt)	// when "prefixtree" is TRUE: prefix count
{
    दीर्घ	len;
    पूर्णाङ्क		idx;
    char_u	*bp;
    idx_T	*ip;

    // The tree size was computed when writing the file, so that we can
    // allocate it as one long block. <nodecount>
    len = get4c(fd);
    यदि (len < 0)
	प्रत्यागम SP_TRUNCERROR;
    यदि ((माप_प्रकार)len > SIZE_MAX / परिमाण(पूर्णाङ्क))
	// Invalid length, multiply with sizeof(int) would overflow.
	प्रत्यागम SP_FORMERROR;
    यदि (len <= 0)
	प्रत्यागम 0;

    // Allocate the byte array.  Zero-initialize so that any position the
    // tree does not visit reads as 0; a stray BY_INDEX shared reference
    // into such a slot then behaves as end-of-word in spellsuggest()
    // instead of consuming an arbitrary heap byte as a siblingcount.
    bp = alloc_clear(len);
    यदि (bp == रिक्त)
	प्रत्यागम SP_OTHERERROR;
    *bytsp = bp;
    यदि (bytsp_len != रिक्त)
	*bytsp_len = len;

    // Allocate the index array.
    ip = lalloc_clear(len * परिमाण(पूर्णाङ्क), TRUE);
    यदि (ip == रिक्त)
	प्रत्यागम SP_OTHERERROR;
    *idxsp = ip;

    // Recursively read the tree and store it in the array.
    idx = read_वृक्ष_गाँठ(fd, bp, ip, len, 0, prefixtree, prefixcnt, 0);
    यदि (idx < 0)
	प्रत्यागम idx;
    यदि (idx != len)
	प्रत्यागम SP_FORMERROR;
    प्रत्यागम 0;
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
    स्थिर idx_T
read_वृक्ष_गाँठ(
    खाता	*fd,
    char_u	*byts,
    idx_T	*idxs,
    पूर्णाङ्क		maxidx,		    // size of arrays
    idx_T	startidx,	    // current index in "byts" and "idxs"
    पूर्णाङ्क		prefixtree,	    // TRUE for reading PREFIXTREE
    पूर्णाङ्क		maxprefcondnr,	    // maximum for <prefcondnr>
    पूर्णाङ्क		depth)		    // recursion level
{
    पूर्णाङ्क		len;
    पूर्णाङ्क		i;
    पूर्णाङ्क		n;
    idx_T	idx = startidx;
    पूर्णाङ्क		c;
    पूर्णाङ्क		c2;
#define SHARED_MASK	0x8000000

    // Bail out on a crafted .spl whose tree recurses beyond the maximum
    // word length: each tree level corresponds to one byte of a word, so
    // any well-formed file has depth <= MAXWLEN.
    यदि (depth > MAXWLEN)
	प्रत्यागम SP_FORMERROR;

    len = अ_लो(fd);					// <siblingcount>
    यदि (len <= 0)
	प्रत्यागम SP_TRUNCERROR;

    यदि (len >= maxidx - startidx)
	प्रत्यागम SP_FORMERROR;
    byts[idx++] = len;

    // Read the byte values, flag/region bytes and shared indexes.
    प्रति (i = 1; i <= len; ++i)
    {
	c = अ_लो(fd);					// <byte>
	यदि (c < 0)
	    प्रत्यागम SP_TRUNCERROR;
	यदि (c <= BY_SPECIAL)
	{
	    यदि (c == BY_NOFLAGS && !prefixtree)
	    {
		// No flags, all regions.
		idxs[idx] = 0;
		c = 0;
	    }
	    अन्यथा यदि (c != BY_INDEX)
	    {
		यदि (prefixtree)
		{
		    // Read the optional pflags byte, the prefix ID and the
		    // condition nr.  In idxs[] store the prefix ID in the low
		    // byte, the condition index shifted up 8 bits, the flags
		    // shifted up 24 bits.
		    यदि (c == BY_FLAGS)
		    {
			n = अ_लो(fd);			// <pflags>
			यदि (n < 0)
			    प्रत्यागम SP_TRUNCERROR;
			c = n << 24;
		    }
		    अन्यथा
			c = 0;

		    n = अ_लो(fd);			// <affixID>
		    यदि (n < 0)
			प्रत्यागम SP_TRUNCERROR;
		    c |= n;

		    n = get2c(fd);			// <prefcondnr>
		    यदि (n < 0 || n >= maxprefcondnr)
			प्रत्यागम SP_FORMERROR;
		    c |= (n << 8);
		}
		अन्यथा // c must be BY_FLAGS or BY_FLAGS2
		{
		    // Read flags and optional region and prefix ID.  In
		    // idxs[] the flags go in the low two bytes, region above
		    // that and prefix ID above the region.
		    c2 = c;
		    c = अ_लो(fd);			// <flags>
		    यदि (c < 0)
			प्रत्यागम SP_TRUNCERROR;
		    यदि (c2 == BY_FLAGS2)
		    {
			n = अ_लो(fd);			// <flags2>
			यदि (n < 0)
			    प्रत्यागम SP_TRUNCERROR;
			c = (n << 8) + c;
		    }
		    यदि (c & WF_REGION)
		    {
			n = अ_लो(fd);			// <region>
			यदि (n < 0)
			    प्रत्यागम SP_TRUNCERROR;
			c = (n << 16) + c;
		    }
		    यदि (c & WF_AFX)
		    {
			n = अ_लो(fd);			// <affixID>
			यदि (n < 0)
			    प्रत्यागम SP_TRUNCERROR;
			c = (n << 24) + c;
		    }
		}

		idxs[idx] = c;
		c = 0;
	    }
	    अन्यथा // c == BY_INDEX
	    {
							// <nodeidx>
		n = get3c(fd);
		यदि (n < 0 || n >= maxidx)
		    प्रत्यागम SP_FORMERROR;
		idxs[idx] = n + SHARED_MASK;
		c = अ_लो(fd);				// <xbyte>
	    }
	}
	byts[idx++] = c;
    }

    // Recursively read the children for non-shared siblings.
    // Skip the end-of-word ones (zero byte value) and the shared ones (and
    // remove SHARED_MASK)
    प्रति (i = 1; i <= len; ++i)
	यदि (byts[startidx + i] != 0)
	{
	    यदि (idxs[startidx + i] & SHARED_MASK)
		idxs[startidx + i] &= ~SHARED_MASK;
	    अन्यथा
	    {
		idxs[startidx + i] = idx;
		idx = read_वृक्ष_गाँठ(fd, byts, idxs, maxidx, idx,
					prefixtree, maxprefcondnr, depth + 1);
		यदि (idx < 0)
		    भङ्ग;
	    }
	}

    प्रत्यागम idx;
}

/*
 * Reload the spell file "fname" if it's loaded.
 */
    स्थिर शून्य
spell_reload_one(
    char_u	*fname,
    पूर्णाङ्क		added_word)	// invoked through "zg"
{
    slang_T	*slang;
    पूर्णाङ्क		didit = FALSE;

    FOR_ALL_SPELL_LANGS(slang)
    {
	यदि (fullpathcmp(fname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	{
	    slang_clear(slang);
	    यदि (spell_load_file(fname, रिक्त, slang, FALSE) == रिक्त)
		// reloading failed, clear the language
		slang_clear(slang);
	    redraw_all_later(UPD_SOME_VALID);
	    didit = TRUE;
	}
    }

    // When "zg" was used and the file wasn't loaded yet, should redo
    // 'spelllang' to load it now.
    यदि (added_word && !didit)
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
प्रकारनिर्देश संरचना afffile_S
{
    char_u	*af_enc;	// "SET", normalized, alloc'ed string or NULL
    पूर्णाङ्क		af_flagtype;	// AFT_CHAR, AFT_LONG, AFT_NUM or AFT_CAPLONG
    अचिन्हित	af_rare;	// RARE ID for rare word
    अचिन्हित	af_keepcase;	// KEEPCASE ID for keep-case word
    अचिन्हित	af_bad;		// BAD ID for banned word
    अचिन्हित	af_needaffix;	// NEEDAFFIX ID
    अचिन्हित	af_circumfix;	// CIRCUMFIX ID
    अचिन्हित	af_needcomp;	// NEEDCOMPOUND ID
    अचिन्हित	af_comproot;	// COMPOUNDROOT ID
    अचिन्हित	af_compforbid;	// COMPOUNDFORBIDFLAG ID
    अचिन्हित	af_comppermit;	// COMPOUNDPERMITFLAG ID
    अचिन्हित	af_nosuggest;	// NOSUGGEST ID
    पूर्णाङ्क		af_pfxpostpone;	// postpone prefixes without chop string and
				// without flags
    पूर्णाङ्क		af_ignoreextra;	// IGNOREEXTRA present
    hashtab_T	af_pref;	// hashtable for prefixes, affheader_T
    hashtab_T	af_suff;	// hashtable for suffixes, affheader_T
    hashtab_T	af_comp;	// hashtable for compound flags, compitem_T
} afffile_T;

#define AFT_CHAR	0	// flags are one character
#define AFT_LONG	1	// flags are two characters
#define AFT_CAPLONG	2	// flags are one or two characters
#define AFT_अंक		3	// flags are numbers, comma separated

प्रकारनिर्देश संरचना affentry_S affentry_T;
// Affix entry from ".aff" file.  Used for prefixes and suffixes.
संरचना affentry_S
{
    affentry_T	*ae_next;	// next affix with same name/number
    char_u	*ae_chop;	// text to chop off basic word (can be NULL)
    char_u	*ae_add;	// text to add to basic word (can be NULL)
    char_u	*ae_flags;	// flags on the affix (can be NULL)
    char_u	*ae_cond;	// condition (NULL for ".")
    regprog_T	*ae_prog;	// regexp program for ae_cond or NULL
    वर्ण	ae_compforbid;	// COMPOUNDFORBIDFLAG found
    वर्ण	ae_comppermit;	// COMPOUNDPERMITFLAG found
};

#define AH_कुंजी_LEN 17		// 2 x 8 bytes + NUL

// Affix header from ".aff" file.  Used for af_pref and af_suff.
प्रकारनिर्देश संरचना affheader_S
{
    char_u	ah_कुंजी[AH_कुंजी_LEN]; // key for hashtab == name of affix
    अचिन्हित	ah_flag;	// affix name as number, uses "af_flagtype"
    पूर्णाङ्क		ah_newID;	// prefix ID after renumbering; 0 if not used
    पूर्णाङ्क		ah_combine;	// suffix may combine with prefix
    पूर्णाङ्क		ah_follows;	// another affix block should be following
    affentry_T	*ah_first;	// first affix entry
} affheader_T;

#define HI2AH(hi)   ((affheader_T *)(hi)->hi_कुंजी)

// Flag used in compound items.
प्रकारनिर्देश संरचना compitem_S
{
    char_u	ci_कुंजी[AH_कुंजी_LEN]; // key for hashtab == name of compound
    अचिन्हित	ci_flag;	// affix name as number, uses "af_flagtype"
    पूर्णाङ्क		ci_newID;	// affix ID after renumbering.
} compitem_T;

#define HI2CI(hi)   ((compitem_T *)(hi)->hi_कुंजी)

/*
 * Structure that is used to store the items in the word tree.  This avoids
 * the need to keep track of each allocated thing, everything is freed all at
 * once after ":mkspell" is done.
 * Note: "sb_next" must be just before "sb_data" to make sure the alignment of
 * "sb_data" is correct for systems where pointers must be aligned on
 * pointer-size boundaries and sizeof(pointer) > sizeof(int) (e.g., Sparc).
 */
#define  SBLOCKSIZE 16000	// size of sb_data
प्रकारनिर्देश संरचना sblock_S sblock_T;
संरचना sblock_S
{
    पूर्णाङ्क		sb_used;	// nr of bytes already in use
    sblock_T	*sb_next;	// next block in list
    char_u	sb_data[1];	// data, actually longer
};

/*
 * A node in the tree.
 */
प्रकारनिर्देश संरचना wordnode_S wordnode_T;
संरचना wordnode_S
{
    सङ्घ   // shared to save space
    {
	char_u	hashkey[6];	// the hash key, only used while compressing
	पूर्णाङ्क	index;		// index in written nodes (valid after first
				// round)
    } wn_u1;
    सङ्घ   // shared to save space
    {
	wordnode_T *next;	// next node with same hash key
	wordnode_T *wnode;	// parent node that will write this node
    } wn_u2;
    wordnode_T	*wn_child;	// child (next byte in word)
    wordnode_T  *wn_sibling;	// next sibling (alternate byte in word,
				// always sorted)
    पूर्णाङ्क		wn_refs;	// Nr. of references to this node.  Only
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
    लघु	wn_region;	// region mask

#ifdef SPELL_PRINTTREE
    पूर्णाङ्क		wn_nr;		// sequence nr for printing
#endif
};

#define WN_MASK	 0xffff		// mask relevant bits of "wn_flags"

#define HI2WN(hi)    (wordnode_T *)((hi)->hi_कुंजी)

/*
 * Info used while reading the spell files.
 */
प्रकारनिर्देश संरचना spellinfo_S
{
    wordnode_T	*si_foldroot;	// tree with case-folded words
    दीर्घ	si_foldwcount;	// nr of words in si_foldroot

    wordnode_T	*si_keeproot;	// tree with keep-case words
    दीर्घ	si_keepwcount;	// nr of words in si_keeproot

    wordnode_T	*si_prefroot;	// tree with postponed prefixes

    दीर्घ	si_sugtree;	// creating the soundfolding trie

    sblock_T	*si_blocks;	// memory blocks used
    दीर्घ	si_blocks_cnt;	// memory blocks allocated
    पूर्णाङ्क		si_did_emsg;	// TRUE when ran out of memory

    दीर्घ	si_compress_cnt;    // words to add before lowering
				    // compression limit
    wordnode_T	*si_first_free; // List of nodes that have been freed during
				// compression, linked by "wn_child" field.
    दीर्घ	si_free_गिनती;	// number of nodes in si_first_free
#ifdef SPELL_PRINTTREE
    पूर्णाङ्क		si_wordnode_nr;	// sequence nr for nodes
#endif
    buf_T	*si_spellbuf;	// buffer used to store soundfold word table

    पूर्णाङ्क		si_ascii;	// handling only ASCII words
    पूर्णाङ्क		si_add;		// addition file
    पूर्णाङ्क		si_clear_chartab;   // when TRUE clear char tables
    पूर्णाङ्क		si_region;	// region mask
    vimconv_T	si_conv;	// for conversion to 'encoding'
    पूर्णाङ्क		si_memtot;	// runtime memory used
    पूर्णाङ्क		si_verbose;	// verbose messages
    पूर्णाङ्क		si_संदेश_गिनती;	// number of words added since last message
    char_u	*si_info;	// info text chars or NULL
    पूर्णाङ्क		si_region_गिनती; // number of regions supported (1 when there
				 // are no regions)
    char_u	si_region_name[MAXREGIONS * 2 + 1];
				// region names; used only if
				// si_region_count > 1)

    garray_T	si_rep;		// list of fromto_T entries from REP lines
    garray_T	si_repsal;	// list of fromto_T entries from REPSAL lines
    garray_T	si_sal;		// list of fromto_T entries from SAL lines
    char_u	*si_sofofr;	// SOFOFROM text
    char_u	*si_sofoto;	// SOFOTO text
    पूर्णाङ्क		si_nosugfile;	// NOSUGFILE item found
    पूर्णाङ्क		si_nosplitsugs;	// NOSPLITSUGS item found
    पूर्णाङ्क		si_nocompoundsugs; // NOCOMPOUNDSUGS item found
    पूर्णाङ्क		si_followup;	// soundsalike: ?
    पूर्णाङ्क		si_collapse;	// soundsalike: ?
    hashtab_T	si_commonwords;	// hashtable for common words
    समय_प्रकार	si_sugtime;	// timestamp for .sug file
    पूर्णाङ्क		si_rem_accents;	// soundsalike: remove accents
    garray_T	si_map;		// MAP info concatenated
    char_u	*si_midword;	// MIDWORD chars or NULL
    पूर्णाङ्क		si_compmax;	// max nr of words for compounding
    पूर्णाङ्क		si_compminlen;	// minimal length for compounding
    पूर्णाङ्क		si_compsylmax;	// max nr of syllables for compounding
    पूर्णाङ्क		si_compoptions;	// COMP_ flags
    garray_T	si_comppat;	// CHECKCOMPOUNDPATTERN items, each stored as
				// a string
    char_u	*si_compflags;	// flags used for compounding
    char_u	si_nobreak;	// NOBREAK
    char_u	*si_syllable;	// syllable string
    garray_T	si_prefcond;	// table with conditions for postponed
				// prefixes, each stored as a string
    पूर्णाङ्क		si_newprefID;	// current value for ah_newID
    पूर्णाङ्क		si_newcompID;	// current value for compound ID
} spellinfo_T;

स्थिर पूर्णाङ्क is_aff_rule(char_u **वस्तुएँ, पूर्णाङ्क itemcnt, वर्ण *rulename, पूर्णाङ्क	 mincount);
स्थिर शून्य aff_process_flags(afffile_T *affile, affentry_T *entry);
स्थिर पूर्णाङ्क spell_info_item(char_u *s);
स्थिर अचिन्हित affitem2flag(पूर्णाङ्क flagtype, char_u *item, char_u	*fname, पूर्णाङ्क lnum);
स्थिर अचिन्हित get_affitem(पूर्णाङ्क flagtype, char_u **pp);
स्थिर शून्य process_compflags(spellinfo_T *spin, afffile_T *aff, char_u *compflags);
स्थिर शून्य check_renumber(spellinfo_T *spin);
स्थिर शून्य aff_check_संख्या(पूर्णाङ्क spinval, पूर्णाङ्क affval, वर्ण *name);
स्थिर शून्य aff_check_string(char_u *spinval, char_u *affval, वर्ण *name);
स्थिर पूर्णाङ्क str_equal(char_u *s1, char_u	*s2);
स्थिर शून्य add_fromto(spellinfo_T *spin, garray_T *gap, char_u	*from, char_u *to);
स्थिर पूर्णाङ्क sal_to_bool(char_u *s);
स्थिर पूर्णाङ्क get_affix_flags(afffile_T *affile, char_u *afflist);
स्थिर पूर्णाङ्क get_pfxlist(afffile_T *affile, char_u *afflist, char_u *store_afflist, पूर्णाङ्क *cntp);
स्थिर पूर्णाङ्क get_compflags(afffile_T *affile, char_u *afflist, char_u *store_afflist, पूर्णाङ्क *cntp);
स्थिर पूर्णाङ्क store_aff_word(spellinfo_T *spin, char_u *word, char_u *afflist, afffile_T *affile, hashtab_T *ht, hashtab_T *xht, पूर्णाङ्क condit, पूर्णाङ्क flags, char_u *pfxlist, पूर्णाङ्क pfxlen);
स्थिर शून्य *getroom(spellinfo_T *spin, माप_प्रकार len, पूर्णाङ्क align);
स्थिर char_u *getroom_save(spellinfo_T *spin, char_u *s);
स्थिर पूर्णाङ्क store_word(spellinfo_T *spin, char_u *word, पूर्णाङ्क flags, पूर्णाङ्क region, char_u *pfxlist, पूर्णाङ्क need_affix);
स्थिर पूर्णाङ्क वृक्ष_add_word(spellinfo_T *spin, char_u *word, wordnode_T *वृक्ष, पूर्णाङ्क flags, पूर्णाङ्क region, पूर्णाङ्क affixID);
स्थिर wordnode_T *get_wordnode(spellinfo_T *spin);
स्थिर शून्य free_wordnode(spellinfo_T *spin, wordnode_T *n);
स्थिर शून्य wordtree_compress(spellinfo_T *spin, wordnode_T *मूल, वर्ण *name);
स्थिर दीर्घ गाँठ_compress(spellinfo_T *spin, wordnode_T *गाँठ, hashtab_T *ht, दीर्घ *tot);
स्थिर पूर्णाङ्क गाँठ_equal(wordnode_T *n1, wordnode_T *n2);
स्थिर शून्य clear_गाँठ(wordnode_T *गाँठ);
स्थिर पूर्णाङ्क put_गाँठ(खाता *fd, wordnode_T *गाँठ, पूर्णाङ्क idx, पूर्णाङ्क regionmask, पूर्णाङ्क prefixtree);
स्थिर पूर्णाङ्क sug_filltree(spellinfo_T *spin, slang_T *slang);
स्थिर पूर्णाङ्क sug_maketable(spellinfo_T *spin);
स्थिर पूर्णाङ्क sug_filltable(spellinfo_T *spin, wordnode_T *गाँठ, पूर्णाङ्क startwordnr, garray_T *gap);
स्थिर पूर्णाङ्क offset2bytes(पूर्णाङ्क nr, char_u *buf);
स्थिर शून्य sug_write(spellinfo_T *spin, char_u *fname);
स्थिर शून्य spell_message(spellinfo_T *spin, char_u *str);
स्थिर शून्य init_spellfile(शून्य);

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
स्थिर दीर्घ compress_start = 30000;	// memory / SBLOCKSIZE
स्थिर दीर्घ compress_inc = 100;		// memory / SBLOCKSIZE
स्थिर दीर्घ compress_added = 500000;	// word count

/*
 * Check the 'mkspellmem' option.  Return FAIL if it's wrong.
 * Sets "sps_flags".
 */
    पूर्णाङ्क
spell_check_msm(शून्य)
{
    char_u	*p = p_msm;
    दीर्घ	start = 0;
    दीर्घ	incr = 0;
    दीर्घ	added = 0;

    यदि (!VIM_ISDIGIT(*p))
	प्रत्यागम FAIL;
    // block count = (value * 1024) / SBLOCKSIZE (but avoid overflow)
    start = (getdigits(&p) * 10) / (SBLOCKSIZE / 102);
    यदि (*p != ',')
	प्रत्यागम FAIL;
    ++p;
    यदि (!VIM_ISDIGIT(*p))
	प्रत्यागम FAIL;
    incr = (getdigits(&p) * 102) / (SBLOCKSIZE / 10);
    यदि (*p != ',')
	प्रत्यागम FAIL;
    ++p;
    यदि (!VIM_ISDIGIT(*p))
	प्रत्यागम FAIL;
    added = getdigits(&p) * 1024;
    यदि (*p != NUL)
	प्रत्यागम FAIL;

    यदि (start == 0 || incr == 0 || added == 0 || incr > start)
	प्रत्यागम FAIL;

    compress_start = start;
    compress_inc = incr;
    compress_added = added;
    प्रत्यागम OK;
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

स्थिर वर्ण line1[PRINTLINESIZE];
स्थिर वर्ण line2[PRINTLINESIZE];
स्थिर वर्ण line3[PRINTLINESIZE];

    स्थिर शून्य
spell_clear_flags(wordnode_T *गाँठ)
{
    wordnode_T	*np;

    FOR_ALL_गाँठ_SIBLINGS(गाँठ, np)
    {
	np->wn_u1.index = FALSE;
	spell_clear_flags(np->wn_child);
    }
}

    स्थिर शून्य
spell_print_गाँठ(wordnode_T *गाँठ, पूर्णाङ्क depth)
{
    यदि (गाँठ->wn_u1.index)
    {
	// Done this node before, print the reference.
	PRINTSOME(line1, depth, "(%d)", गाँठ->wn_nr, 0);
	PRINTSOME(line2, depth, "    ", 0, 0);
	PRINTSOME(line3, depth, "    ", 0, 0);
	संदेश(line1);
	संदेश(line2);
	संदेश(line3);
    }
    अन्यथा
    {
	गाँठ->wn_u1.index = TRUE;

	यदि (गाँठ->wn_byte != NUL)
	{
	    यदि (गाँठ->wn_child != रिक्त)
		PRINTSOME(line1, depth, " %c -> ", गाँठ->wn_byte, 0);
	    अन्यथा
		// Cannot happen?
		PRINTSOME(line1, depth, " %c ???", गाँठ->wn_byte, 0);
	}
	अन्यथा
	    PRINTSOME(line1, depth, " $    ", 0, 0);

	PRINTSOME(line2, depth, "%d/%d    ", गाँठ->wn_nr, गाँठ->wn_refs);

	यदि (गाँठ->wn_sibling != रिक्त)
	    PRINTSOME(line3, depth, " |    ", 0, 0);
	अन्यथा
	    PRINTSOME(line3, depth, "      ", 0, 0);

	यदि (गाँठ->wn_byte == NUL)
	{
	    संदेश(line1);
	    संदेश(line2);
	    संदेश(line3);
	}

	// do the children
	यदि (गाँठ->wn_byte != NUL && गाँठ->wn_child != रिक्त)
	    spell_print_गाँठ(गाँठ->wn_child, depth + 1);

	// do the siblings
	यदि (गाँठ->wn_sibling != रिक्त)
	{
	    // get rid of all parent details except |
	    STRCPY(line1, line3);
	    STRCPY(line2, line3);
	    spell_print_गाँठ(गाँठ->wn_sibling, depth);
	}
    }
}

    स्थिर शून्य
spell_print_वृक्ष(wordnode_T *मूल)
{
    यदि (मूल == रिक्त)
	प्रत्यागम;

    // Clear the "wn_u1.index" fields, used to remember what has been done.
    spell_clear_flags(मूल);

    // Recursively print the tree.
    spell_print_गाँठ(मूल, 0);
}
#endif // SPELL_PRINTTREE

/*
 * Read the affix file "fname".
 * Returns an afffile_T, NULL for complete failure.
 */
    स्थिर afffile_T *
spell_read_aff(spellinfo_T *spin, char_u *fname)
{
    खाता	*fd;
    afffile_T	*aff;
    char_u	rline[MAXLINELEN];
    char_u	*पंक्ति;
    char_u	*pc = रिक्त;
#define MAXITEMCNT  30
    char_u	*(वस्तुएँ[MAXITEMCNT]);
    पूर्णाङ्क		itemcnt;
    char_u	*p;
    पूर्णाङ्क		lnum = 0;
    affheader_T	*cur_aff = रिक्त;
    पूर्णाङ्क		did_postpone_prefix = FALSE;
    पूर्णाङ्क		aff_todo = 0;
    hashtab_T	*tp;
    char_u	*निचला = रिक्त;
    char_u	*fol = रिक्त;
    char_u	*upp = रिक्त;
    पूर्णाङ्क		do_rep;
    पूर्णाङ्क		do_repsal;
    पूर्णाङ्क		do_sal;
    पूर्णाङ्क		do_mapline;
    पूर्णाङ्क		found_map = FALSE;
    hashitem_T	*hi;
    पूर्णाङ्क		l;
    पूर्णाङ्क		compminlen = 0;		// COMPOUNDMIN value
    पूर्णाङ्क		compsylmax = 0;		// COMPOUNDSYLMAX value
    पूर्णाङ्क		compoptions = 0;	// COMP_ flags
    पूर्णाङ्क		compmax = 0;		// COMPOUNDWORDMAX value
    char_u	*compflags = रिक्त;	// COMPOUNDFLAG and COMPOUNDRULE
					// concatenated
    char_u	*midword = रिक्त;	// MIDWORD value
    char_u	*syllable = रिक्त;	// SYLLABLE value
    char_u	*sofofrom = रिक्त;	// SOFOFROM value
    char_u	*sofoto = रिक्त;		// SOFOTO value

    /*
     * Open the file.
     */
    fd = mch_fopen((वर्ण *)fname, "r");
    यदि (fd == रिक्त)
    {
	semsg(_(e_cant_open_file_str), fname);
	प्रत्यागम रिक्त;
    }

    vim_snprintf((वर्ण *)IObuff, IOSIZE, _("Reading affix file %s..."), fname);
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
    aff = (afffile_T *)getroom(spin, परिमाण(afffile_T), TRUE);
    यदि (aff == रिक्त)
    {
	ख_बंद(fd);
	प्रत्यागम रिक्त;
    }
    hash_init(&aff->af_pref);
    hash_init(&aff->af_suff);
    hash_init(&aff->af_comp);

    /*
     * Read all the lines in the file one by one.
     */
    यावत् (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	पंक्ति_breakcheck();
	++lnum;

	// Skip comment lines.
	यदि (*rline == '#')
	    अनुवर्त;

	// Convert from "SET" to 'encoding' when needed.
	vim_free(pc);
	यदि (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, रिक्त);
	    यदि (pc == रिक्त)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
							   fname, lnum, rline);
		अनुवर्त;
	    }
	    पंक्ति = pc;
	}
	अन्यथा
	{
	    pc = रिक्त;
	    पंक्ति = rline;
	}

	// Split the line up in white separated items.  Put a NUL after each
	// item.
	itemcnt = 0;
	प्रति (p = पंक्ति; ; )
	{
	    यावत् (*p != NUL && *p <= ' ')  // skip white space and CR/NL
		++p;
	    यदि (*p == NUL)
		भङ्ग;
	    यदि (itemcnt == MAXITEMCNT)	    // too many items
		भङ्ग;
	    वस्तुएँ[itemcnt++] = p;
	    // A few items have arbitrary text argument, don't split them.
	    यदि (itemcnt == 2 && spell_info_item(वस्तुएँ[0]))
		यावत् (*p >= ' ' || *p == TAB)    // skip until CR/NL
		    ++p;
	    अन्यथा
		यावत् (*p > ' ')    // skip until white space or CR/NL
		    ++p;
	    यदि (*p == NUL)
		भङ्ग;
	    *p++ = NUL;
	}

	// Handle non-empty lines.
	यदि (itemcnt > 0)
	{
	    यदि (is_aff_rule(वस्तुएँ, itemcnt, "SET", 2) && aff->af_enc == रिक्त)
	    {
		// Setup for conversion from "ENC" to 'encoding'.
		aff->af_enc = enc_canonize(वस्तुएँ[1]);
		यदि (aff->af_enc != रिक्त && !spin->si_ascii
			&& convert_setup(&spin->si_conv, aff->af_enc,
							       p_enc) == FAIL)
		    smsg(_("Conversion in %s not supported: from %s to %s"),
					       fname, aff->af_enc, p_enc);
		spin->si_conv.vc_fail = TRUE;
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "FLAG", 2)
					      && aff->af_flagtype == AFT_CHAR)
	    {
		यदि (STRCMP(वस्तुएँ[1], "long") == 0)
		    aff->af_flagtype = AFT_LONG;
		अन्यथा यदि (STRCMP(वस्तुएँ[1], "num") == 0)
		    aff->af_flagtype = AFT_अंक;
		अन्यथा यदि (STRCMP(वस्तुएँ[1], "caplong") == 0)
		    aff->af_flagtype = AFT_CAPLONG;
		अन्यथा
		    smsg(_("Invalid value for FLAG in %s line %d: %s"),
			    fname, lnum, वस्तुएँ[1]);
		यदि (aff->af_rare != 0
			|| aff->af_keepcase != 0
			|| aff->af_bad != 0
			|| aff->af_needaffix != 0
			|| aff->af_circumfix != 0
			|| aff->af_needcomp != 0
			|| aff->af_comproot != 0
			|| aff->af_nosuggest != 0
			|| compflags != रिक्त
			|| aff->af_suff.ht_used > 0
			|| aff->af_pref.ht_used > 0)
		    smsg(_("FLAG after using flags in %s line %d: %s"),
			    fname, lnum, वस्तुएँ[1]);
	    }
	    अन्यथा यदि (spell_info_item(वस्तुएँ[0]))
	    {
		    p = (char_u *)getroom(spin,
			    (spin->si_info == रिक्त ? 0 : STRLEN(spin->si_info))
			    + STRLEN(वस्तुएँ[0])
			    + STRLEN(वस्तुएँ[1]) + 3, FALSE);
		    यदि (p != रिक्त)
		    {
			यदि (spin->si_info != रिक्त)
			{
			    STRCPY(p, spin->si_info);
			    STRCAT(p, "\n");
			}
			STRCAT(p, वस्तुएँ[0]);
			STRCAT(p, " ");
			STRCAT(p, वस्तुएँ[1]);
			spin->si_info = p;
		    }
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "MIDWORD", 2)
							   && midword == रिक्त)
	    {
		midword = getroom_save(spin, वस्तुएँ[1]);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "TRY", 2))
	    {
		// ignored, we look in the tree for what chars may appear
	    }
	    // TODO: remove "RAR" later
	    अन्यथा यदि ((is_aff_rule(वस्तुएँ, itemcnt, "RAR", 2)
			|| is_aff_rule(वस्तुएँ, itemcnt, "RARE", 2))
							 && aff->af_rare == 0)
	    {
		aff->af_rare = affitem2flag(aff->af_flagtype, वस्तुएँ[1],
								 fname, lnum);
	    }
	    // TODO: remove "KEP" later
	    अन्यथा यदि ((is_aff_rule(वस्तुएँ, itemcnt, "KEP", 2)
			|| is_aff_rule(वस्तुएँ, itemcnt, "KEEPCASE", 2))
						     && aff->af_keepcase == 0)
	    {
		aff->af_keepcase = affitem2flag(aff->af_flagtype, वस्तुएँ[1],
								 fname, lnum);
	    }
	    अन्यथा यदि ((is_aff_rule(वस्तुएँ, itemcnt, "BAD", 2)
			|| is_aff_rule(वस्तुएँ, itemcnt, "FORBIDDENWORD", 2))
							  && aff->af_bad == 0)
	    {
		aff->af_bad = affitem2flag(aff->af_flagtype, वस्तुएँ[1],
								 fname, lnum);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "NEEDAFFIX", 2)
						    && aff->af_needaffix == 0)
	    {
		aff->af_needaffix = affitem2flag(aff->af_flagtype, वस्तुएँ[1],
								 fname, lnum);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "CIRCUMFIX", 2)
						    && aff->af_circumfix == 0)
	    {
		aff->af_circumfix = affitem2flag(aff->af_flagtype, वस्तुएँ[1],
								 fname, lnum);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "NOSUGGEST", 2)
						    && aff->af_nosuggest == 0)
	    {
		aff->af_nosuggest = affitem2flag(aff->af_flagtype, वस्तुएँ[1],
								 fname, lnum);
	    }
	    अन्यथा यदि ((is_aff_rule(वस्तुएँ, itemcnt, "NEEDCOMPOUND", 2)
			|| is_aff_rule(वस्तुएँ, itemcnt, "ONLYINCOMPOUND", 2))
						     && aff->af_needcomp == 0)
	    {
		aff->af_needcomp = affitem2flag(aff->af_flagtype, वस्तुएँ[1],
								 fname, lnum);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "COMPOUNDROOT", 2)
						     && aff->af_comproot == 0)
	    {
		aff->af_comproot = affitem2flag(aff->af_flagtype, वस्तुएँ[1],
								 fname, lnum);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "COMPOUNDFORBIDFLAG", 2)
						   && aff->af_compforbid == 0)
	    {
		aff->af_compforbid = affitem2flag(aff->af_flagtype, वस्तुएँ[1],
								 fname, lnum);
		यदि (aff->af_pref.ht_used > 0)
		    smsg(_("Defining COMPOUNDFORBIDFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "COMPOUNDPERMITFLAG", 2)
						   && aff->af_comppermit == 0)
	    {
		aff->af_comppermit = affitem2flag(aff->af_flagtype, वस्तुएँ[1],
								 fname, lnum);
		यदि (aff->af_pref.ht_used > 0)
		    smsg(_("Defining COMPOUNDPERMITFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "COMPOUNDFLAG", 2)
							 && compflags == रिक्त)
	    {
		// Turn flag "c" into COMPOUNDRULE compatible string "c+",
		// "Na" into "Na+", "1234" into "1234+".
		p = getroom(spin, STRLEN(वस्तुएँ[1]) + 2, FALSE);
		यदि (p != रिक्त)
		{
		    STRCPY(p, वस्तुएँ[1]);
		    STRCAT(p, "+");
		    compflags = p;
		}
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "COMPOUNDRULES", 2))
	    {
		// We don't use the count, but do check that it's a number and
		// not COMPOUNDRULE mistyped.
		यदि (म_से_प((वर्ण *)वस्तुएँ[1]) == 0)
		    smsg(_("Wrong COMPOUNDRULES value in %s line %d: %s"),
						       fname, lnum, वस्तुएँ[1]);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "COMPOUNDRULE", 2))
	    {
		// Don't use the first rule if it is a number.
		यदि (compflags != रिक्त || *skipdigits(वस्तुएँ[1]) != NUL)
		{
		    // Concatenate this string to previously defined ones,
		    // using a slash to separate them.
		    l = (पूर्णाङ्क)STRLEN(वस्तुएँ[1]) + 1;
		    यदि (compflags != रिक्त)
			l += (पूर्णाङ्क)STRLEN(compflags) + 1;
		    p = getroom(spin, l, FALSE);
		    यदि (p != रिक्त)
		    {
			यदि (compflags != रिक्त)
			{
			    STRCPY(p, compflags);
			    STRCAT(p, "/");
			}
			STRCAT(p, वस्तुएँ[1]);
			compflags = p;
		    }
		}
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "COMPOUNDWORDMAX", 2)
							      && compmax == 0)
	    {
		compmax = म_से_प((वर्ण *)वस्तुएँ[1]);
		यदि (compmax == 0)
		    smsg(_("Wrong COMPOUNDWORDMAX value in %s line %d: %s"),
						       fname, lnum, वस्तुएँ[1]);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "COMPOUNDMIN", 2)
							   && compminlen == 0)
	    {
		compminlen = म_से_प((वर्ण *)वस्तुएँ[1]);
		यदि (compminlen == 0)
		    smsg(_("Wrong COMPOUNDMIN value in %s line %d: %s"),
						       fname, lnum, वस्तुएँ[1]);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "COMPOUNDSYLMAX", 2)
							   && compsylmax == 0)
	    {
		compsylmax = म_से_प((वर्ण *)वस्तुएँ[1]);
		यदि (compsylmax == 0)
		    smsg(_("Wrong COMPOUNDSYLMAX value in %s line %d: %s"),
						       fname, lnum, वस्तुएँ[1]);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "CHECKCOMPOUNDDUP", 1))
	    {
		compoptions |= COMP_CHECKDUP;
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "CHECKCOMPOUNDREP", 1))
	    {
		compoptions |= COMP_CHECKREP;
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "CHECKCOMPOUNDCASE", 1))
	    {
		compoptions |= COMP_CHECKCASE;
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "CHECKCOMPOUNDTRIPLE", 1))
	    {
		compoptions |= COMP_CHECKTRIPLE;
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "CHECKCOMPOUNDPATTERN", 2))
	    {
		यदि (म_से_प((वर्ण *)वस्तुएँ[1]) == 0)
		    smsg(_("Wrong CHECKCOMPOUNDPATTERN value in %s line %d: %s"),
						       fname, lnum, वस्तुएँ[1]);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "CHECKCOMPOUNDPATTERN", 3))
	    {
		garray_T    *gap = &spin->si_comppat;
		पूर्णाङ्क	    i;

		// Only add the couple if it isn't already there.
		प्रति (i = 0; i < gap->ga_len - 1; i += 2)
		    यदि (STRCMP(((char_u **)(gap->ga_data))[i], वस्तुएँ[1]) == 0
			    && STRCMP(((char_u **)(gap->ga_data))[i + 1],
							       वस्तुएँ[2]) == 0)
			भङ्ग;
		यदि (i >= gap->ga_len && ga_grow(gap, 2) == OK)
		{
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, वस्तुएँ[1]);
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, वस्तुएँ[2]);
		}
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "SYLLABLE", 2)
							  && syllable == रिक्त)
	    {
		syllable = getroom_save(spin, वस्तुएँ[1]);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "NOBREAK", 1))
	    {
		spin->si_nobreak = TRUE;
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "NOSPLITSUGS", 1))
	    {
		spin->si_nosplitsugs = TRUE;
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "NOCOMPOUNDSUGS", 1))
	    {
		spin->si_nocompoundsugs = TRUE;
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "NOSUGFILE", 1))
	    {
		spin->si_nosugfile = TRUE;
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "PFXPOSTPONE", 1))
	    {
		aff->af_pfxpostpone = TRUE;
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "IGNOREEXTRA", 1))
	    {
		aff->af_ignoreextra = TRUE;
	    }
	    अन्यथा यदि ((STRCMP(वस्तुएँ[0], "PFX") == 0
					      || STRCMP(वस्तुएँ[0], "SFX") == 0)
		    && aff_todo == 0
		    && itemcnt >= 4)
	    {
		पूर्णाङ्क	lasti = 4;
		char_u	कुंजी[AH_कुंजी_LEN];

		यदि (*वस्तुएँ[0] == 'P')
		    tp = &aff->af_pref;
		अन्यथा
		    tp = &aff->af_suff;

		// Myspell allows the same affix name to be used multiple
		// times.  The affix files that do this have an undocumented
		// "S" flag on all but the last block, thus we check for that
		// and store it in ah_follows.
		vim_strncpy(कुंजी, वस्तुएँ[1], AH_कुंजी_LEN - 1);
		hi = hash_find(tp, कुंजी);
		यदि (!HASHITEM_EMPTY(hi))
		{
		    cur_aff = HI2AH(hi);
		    यदि (cur_aff->ah_combine != (*वस्तुएँ[2] == 'Y'))
			smsg(_("Different combining flag in continued affix block in %s line %d: %s"),
						   fname, lnum, वस्तुएँ[1]);
		    यदि (!cur_aff->ah_follows)
			smsg(_("Duplicate affix in %s line %d: %s"),
						       fname, lnum, वस्तुएँ[1]);
		}
		अन्यथा
		{
		    // New affix letter.
		    cur_aff = (affheader_T *)getroom(spin,
						   परिमाण(affheader_T), TRUE);
		    यदि (cur_aff == रिक्त)
			भङ्ग;
		    cur_aff->ah_flag = affitem2flag(aff->af_flagtype, वस्तुएँ[1],
								 fname, lnum);
		    यदि (cur_aff->ah_flag == 0 || STRLEN(वस्तुएँ[1]) >= AH_कुंजी_LEN)
			भङ्ग;
		    यदि (cur_aff->ah_flag == aff->af_bad
			    || cur_aff->ah_flag == aff->af_rare
			    || cur_aff->ah_flag == aff->af_keepcase
			    || cur_aff->ah_flag == aff->af_needaffix
			    || cur_aff->ah_flag == aff->af_circumfix
			    || cur_aff->ah_flag == aff->af_nosuggest
			    || cur_aff->ah_flag == aff->af_needcomp
			    || cur_aff->ah_flag == aff->af_comproot)
			smsg(_("Affix also used for BAD/RARE/KEEPCASE/NEEDAFFIX/NEEDCOMPOUND/NOSUGGEST in %s line %d: %s"),
						       fname, lnum, वस्तुएँ[1]);
		    STRCPY(cur_aff->ah_कुंजी, वस्तुएँ[1]);
		    hash_add(tp, cur_aff->ah_कुंजी, "spelling");

		    cur_aff->ah_combine = (*वस्तुएँ[2] == 'Y');
		}

		// Check for the "S" flag, which apparently means that another
		// block with the same affix name is following.
		यदि (itemcnt > lasti && STRCMP(वस्तुएँ[lasti], "S") == 0)
		{
		    ++lasti;
		    cur_aff->ah_follows = TRUE;
		}
		अन्यथा
		    cur_aff->ah_follows = FALSE;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter.
		यदि (itemcnt > lasti && *वस्तुएँ[lasti] != '#')
		    smsg(_(e_afftrailing), fname, lnum, वस्तुएँ[lasti]);

		यदि (STRCMP(वस्तुएँ[2], "Y") != 0 && STRCMP(वस्तुएँ[2], "N") != 0)
		    smsg(_("Expected Y or N in %s line %d: %s"),
						       fname, lnum, वस्तुएँ[2]);

		यदि (*वस्तुएँ[0] == 'P' && aff->af_pfxpostpone)
		{
		    यदि (cur_aff->ah_newID == 0)
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
		    अन्यथा
			// Did use the ID in a previous block.
			did_postpone_prefix = TRUE;
		}

		aff_todo = म_से_प((वर्ण *)वस्तुएँ[3]);
	    }
	    अन्यथा यदि ((STRCMP(वस्तुएँ[0], "PFX") == 0
					      || STRCMP(वस्तुएँ[0], "SFX") == 0)
		    && aff_todo > 0
		    && STRCMP(cur_aff->ah_कुंजी, वस्तुएँ[1]) == 0
		    && itemcnt >= 5)
	    {
		affentry_T	*aff_entry;
		पूर्णाङ्क		upper = FALSE;
		पूर्णाङ्क		lasti = 5;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter,
		// unless IGNOREEXTRA is used.  Hunspell uses a "-" item.
		यदि (itemcnt > lasti
			&& !aff->af_ignoreextra
			&& *वस्तुएँ[lasti] != '#'
			&& (STRCMP(वस्तुएँ[lasti], "-") != 0
						     || itemcnt != lasti + 1))
		    smsg(_(e_afftrailing), fname, lnum, वस्तुएँ[lasti]);

		// New item for an affix letter.
		--aff_todo;
		aff_entry = (affentry_T *)getroom(spin,
						    परिमाण(affentry_T), TRUE);
		यदि (aff_entry == रिक्त)
		    भङ्ग;

		यदि (STRCMP(वस्तुएँ[2], "0") != 0)
		    aff_entry->ae_chop = getroom_save(spin, वस्तुएँ[2]);
		यदि (STRCMP(वस्तुएँ[3], "0") != 0)
		{
		    aff_entry->ae_add = getroom_save(spin, वस्तुएँ[3]);

		    // Recognize flags on the affix: abcd/XYZ
		    aff_entry->ae_flags = vim_strchr(aff_entry->ae_add, '/');
		    यदि (aff_entry->ae_flags != रिक्त)
		    {
			*aff_entry->ae_flags++ = NUL;
			aff_process_flags(aff, aff_entry);
		    }
		}

		// Don't use an affix entry with non-ASCII characters when
		// "spin->si_ascii" is TRUE.
		यदि (!spin->si_ascii || !(has_non_ascii(aff_entry->ae_chop)
					  || has_non_ascii(aff_entry->ae_add)))
		{
		    aff_entry->ae_next = cur_aff->ah_first;
		    cur_aff->ah_first = aff_entry;

		    यदि (STRCMP(वस्तुएँ[4], ".") != 0)
		    {
			char_u	buf[MAXLINELEN];

			aff_entry->ae_cond = getroom_save(spin, वस्तुएँ[4]);
			// Note: this silently truncates the buffer, but this should
			// not happen in practice
			यदि (*वस्तुएँ[0] == 'P')
			    vim_snprintf((वर्ण *)buf, परिमाण(buf), "^%s", वस्तुएँ[4]);
			अन्यथा
			    vim_snprintf((वर्ण *)buf, परिमाण(buf), "%s$", वस्तुएँ[4]);
			aff_entry->ae_prog = vim_regcomp(buf,
					    RE_MAGIC + RE_STRING + RE_STRICT);
			यदि (aff_entry->ae_prog == रिक्त)
			    smsg(_("Broken condition in %s line %d: %s"),
						       fname, lnum, वस्तुएँ[4]);
		    }

		    // For postponed prefixes we need an entry in si_prefcond
		    // for the condition.  Use an existing one if possible.
		    // Can't be done for an affix with flags, ignoring
		    // COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG.
		    यदि (*वस्तुएँ[0] == 'P' && aff->af_pfxpostpone
					       && aff_entry->ae_flags == रिक्त)
		    {
			// When the chop string is one lower-case letter and
			// the add string ends in the upper-case letter we set
			// the "upper" flag, clear "ae_chop" and remove the
			// letters from "ae_add".  The condition must either
			// be empty or start with the same letter.
			यदि (aff_entry->ae_chop != रिक्त
				&& aff_entry->ae_add != रिक्त
				&& aff_entry->ae_chop[(*mb_ptr2len)(
						   aff_entry->ae_chop)] == NUL)
			{
			    पूर्णाङ्क		c, c_up;

			    c = PTR2CHAR(aff_entry->ae_chop);
			    c_up = SPELL_TOUPPER(c);
			    यदि (c_up != c
				    && (aff_entry->ae_cond == रिक्त
					|| PTR2CHAR(aff_entry->ae_cond) == c))
			    {
				p = aff_entry->ae_add
						  + STRLEN(aff_entry->ae_add);
				MB_PTR_BACK(aff_entry->ae_add, p);
				यदि (PTR2CHAR(p) == c_up)
				{
				    upper = TRUE;
				    aff_entry->ae_chop = रिक्त;
				    *p = NUL;

				    // The condition is matched with the
				    // actual word, thus must check for the
				    // upper-case letter.
				    यदि (aff_entry->ae_cond != रिक्त)
				    {
					char_u	buf[MAXLINELEN];

					यदि (has_mbyte)
					{
					    onecap_copy(वस्तुएँ[4], buf, TRUE);
					    aff_entry->ae_cond = getroom_save(
								   spin, buf);
					}
					अन्यथा
					    *aff_entry->ae_cond = c_up;
					यदि (aff_entry->ae_cond != रिक्त)
					{
					    प्र_लिखो((वर्ण *)buf, "^%s",
							  aff_entry->ae_cond);
					    vim_regfree(aff_entry->ae_prog);
					    aff_entry->ae_prog = vim_regcomp(
						    buf, RE_MAGIC + RE_STRING);
					}
				    }
				}
			    }
			}

			यदि (aff_entry->ae_chop == रिक्त
					       && aff_entry->ae_flags == रिक्त)
			{
			    पूर्णाङ्क		idx;
			    char_u	**pp;
			    पूर्णाङ्क		n;

			    // Find a previously used condition.
			    प्रति (idx = spin->si_prefcond.ga_len - 1; idx >= 0;
									--idx)
			    {
				p = ((char_u **)spin->si_prefcond.ga_data)[idx];
				यदि (str_equal(p, aff_entry->ae_cond))
				    भङ्ग;
			    }
			    यदि (idx < 0 && ga_grow(&spin->si_prefcond, 1) == OK)
			    {
				// Not found, add a new condition.
				idx = spin->si_prefcond.ga_len++;
				pp = ((char_u **)spin->si_prefcond.ga_data)
									+ idx;
				यदि (aff_entry->ae_cond == रिक्त)
				    *pp = रिक्त;
				अन्यथा
				    *pp = getroom_save(spin,
							  aff_entry->ae_cond);
			    }

			    // Add the prefix to the prefix tree.
			    यदि (aff_entry->ae_add == रिक्त)
				p = (char_u *)"";
			    अन्यथा
				p = aff_entry->ae_add;

			    // PFX_FLAGS is a negative number, so that
			    // tree_add_word() knows this is the prefix tree.
			    n = PFX_FLAGS;
			    यदि (!cur_aff->ah_combine)
				n |= WFP_NC;
			    यदि (upper)
				n |= WFP_UP;
			    यदि (aff_entry->ae_comppermit)
				n |= WFP_COMPPERMIT;
			    यदि (aff_entry->ae_compforbid)
				n |= WFP_COMPFORBID;
			    वृक्ष_add_word(spin, p, spin->si_prefroot, n,
						      idx, cur_aff->ah_newID);
			    did_postpone_prefix = TRUE;
			}

			// Didn't actually use ah_newID, backup si_newprefID.
			यदि (aff_todo == 0 && !did_postpone_prefix)
			{
			    --spin->si_newprefID;
			    cur_aff->ah_newID = 0;
			}
		    }
		}
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "FOL", 2) && fol == रिक्त)
	    {
		fol = vim_strsave(वस्तुएँ[1]);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "LOW", 2) && निचला == रिक्त)
	    {
		निचला = vim_strsave(वस्तुएँ[1]);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "UPP", 2) && upp == रिक्त)
	    {
		upp = vim_strsave(वस्तुएँ[1]);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "REP", 2)
		     || is_aff_rule(वस्तुएँ, itemcnt, "REPSAL", 2))
	    {
		// Ignore REP/REPSAL count
		यदि (!SAFE_isdigit(*वस्तुएँ[1]))
		    smsg(_("Expected REP(SAL) count in %s line %d"),
								 fname, lnum);
	    }
	    अन्यथा यदि ((STRCMP(वस्तुएँ[0], "REP") == 0
			|| STRCMP(वस्तुएँ[0], "REPSAL") == 0)
		    && itemcnt >= 3)
	    {
		// REP/REPSAL item
		// Myspell ignores extra arguments, we require it starts with
		// # to detect mistakes.
		यदि (itemcnt > 3 && वस्तुएँ[3][0] != '#')
		    smsg(_(e_afftrailing), fname, lnum, वस्तुएँ[3]);
		यदि (वस्तुएँ[0][3] == 'S' ? do_repsal : do_rep)
		{
		    // Replace underscore with space (can't include a space
		    // directly).
		    प्रति (p = वस्तुएँ[1]; *p != NUL; MB_PTR_ADV(p))
			यदि (*p == '_')
			    *p = ' ';
		    प्रति (p = वस्तुएँ[2]; *p != NUL; MB_PTR_ADV(p))
			यदि (*p == '_')
			    *p = ' ';
		    add_fromto(spin, वस्तुएँ[0][3] == 'S'
					 ? &spin->si_repsal
					 : &spin->si_rep, वस्तुएँ[1], वस्तुएँ[2]);
		}
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "MAP", 2))
	    {
		// MAP item or count
		यदि (!found_map)
		{
		    // First line contains the count.
		    found_map = TRUE;
		    यदि (!SAFE_isdigit(*वस्तुएँ[1]))
			smsg(_("Expected MAP count in %s line %d"),
								 fname, lnum);
		}
		अन्यथा यदि (do_mapline)
		{
		    पूर्णाङ्क		c;

		    // Check that every character appears only once.
		    प्रति (p = वस्तुएँ[1]; *p != NUL; )
		    {
			c = mb_ptr2char_adv(&p);
			यदि ((spin->si_map.ga_len > 0
				    && vim_strchr(spin->si_map.ga_data, c)
								      != रिक्त)
				|| vim_strchr(p, c) != रिक्त)
			    smsg(_("Duplicate character in MAP in %s line %d"),
								 fname, lnum);
		    }

		    // We simply concatenate all the MAP strings, separated by
		    // slashes.
		    ga_concat(&spin->si_map, वस्तुएँ[1]);
		    ga_append(&spin->si_map, '/');
		}
	    }
	    // Accept "SAL from to" and "SAL from to  #comment".
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "SAL", 3))
	    {
		यदि (do_sal)
		{
		    // SAL item (sounds-a-like)
		    // Either one of the known keys or a from-to pair.
		    यदि (STRCMP(वस्तुएँ[1], "followup") == 0)
			spin->si_followup = sal_to_bool(वस्तुएँ[2]);
		    अन्यथा यदि (STRCMP(वस्तुएँ[1], "collapse_result") == 0)
			spin->si_collapse = sal_to_bool(वस्तुएँ[2]);
		    अन्यथा यदि (STRCMP(वस्तुएँ[1], "remove_accents") == 0)
			spin->si_rem_accents = sal_to_bool(वस्तुएँ[2]);
		    अन्यथा
			// when "to" is "_" it means empty
			add_fromto(spin, &spin->si_sal, वस्तुएँ[1],
				     STRCMP(वस्तुएँ[2], "_") == 0 ? (char_u *)""
								: वस्तुएँ[2]);
		}
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "SOFOFROM", 2)
							  && sofofrom == रिक्त)
	    {
		sofofrom = getroom_save(spin, वस्तुएँ[1]);
	    }
	    अन्यथा यदि (is_aff_rule(वस्तुएँ, itemcnt, "SOFOTO", 2)
							    && sofoto == रिक्त)
	    {
		sofoto = getroom_save(spin, वस्तुएँ[1]);
	    }
	    अन्यथा यदि (STRCMP(वस्तुएँ[0], "COMMON") == 0)
	    {
		पूर्णाङ्क	i;

		प्रति (i = 1; i < itemcnt; ++i)
		{
		    यदि (HASHITEM_EMPTY(hash_find(&spin->si_commonwords,
								   वस्तुएँ[i])))
		    {
			p = vim_strsave(वस्तुएँ[i]);
			यदि (p == रिक्त)
			    भङ्ग;
			hash_add(&spin->si_commonwords, p, "spelling");
		    }
		}
	    }
	    अन्यथा
		smsg(_("Unrecognized or duplicate item in %s line %d: %s"),
						       fname, lnum, वस्तुएँ[0]);
	}
    }

    यदि (fol != रिक्त || निचला != रिक्त || upp != रिक्त)
    {
	यदि (spin->si_clear_chartab)
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
	यदि (!spin->si_ascii && !enc_utf8)
	{
	    यदि (fol == रिक्त || निचला == रिक्त || upp == रिक्त)
		smsg(_("Missing FOL/LOW/UPP line in %s"), fname);
	    अन्यथा
		(शून्य)set_spell_chartab(fol, निचला, upp);
	}

	vim_free(fol);
	vim_free(निचला);
	vim_free(upp);
    }

    // Use compound specifications of the .aff file for the spell info.
    यदि (compmax != 0)
    {
	aff_check_संख्या(spin->si_compmax, compmax, "COMPOUNDWORDMAX");
	spin->si_compmax = compmax;
    }

    यदि (compminlen != 0)
    {
	aff_check_संख्या(spin->si_compminlen, compminlen, "COMPOUNDMIN");
	spin->si_compminlen = compminlen;
    }

    यदि (compsylmax != 0)
    {
	यदि (syllable == रिक्त)
	    smsg(_("COMPOUNDSYLMAX used without SYLLABLE"));
	aff_check_संख्या(spin->si_compsylmax, compsylmax, "COMPOUNDSYLMAX");
	spin->si_compsylmax = compsylmax;
    }

    यदि (compoptions != 0)
    {
	aff_check_संख्या(spin->si_compoptions, compoptions, "COMPOUND options");
	spin->si_compoptions |= compoptions;
    }

    यदि (compflags != रिक्त)
	process_compflags(spin, aff, compflags);

    // Check that we didn't use too many renumbered flags.
    यदि (spin->si_newcompID < spin->si_newprefID)
    {
	यदि (spin->si_newcompID == 127 || spin->si_newcompID == 255)
	    संदेश(_("Too many postponed prefixes"));
	अन्यथा यदि (spin->si_newprefID == 0 || spin->si_newprefID == 127)
	    संदेश(_("Too many compound flags"));
	अन्यथा
	    संदेश(_("Too many postponed prefixes and/or compound flags"));
    }

    यदि (syllable != रिक्त)
    {
	aff_check_string(spin->si_syllable, syllable, "SYLLABLE");
	spin->si_syllable = syllable;
    }

    यदि (sofofrom != रिक्त || sofoto != रिक्त)
    {
	यदि (sofofrom == रिक्त || sofoto == रिक्त)
	    smsg(_("Missing SOFO%s line in %s"),
				     sofofrom == रिक्त ? "FROM" : "TO", fname);
	अन्यथा यदि (spin->si_sal.ga_len > 0)
	    smsg(_("Both SAL and SOFO lines in %s"), fname);
	अन्यथा
	{
	    aff_check_string(spin->si_sofofr, sofofrom, "SOFOFROM");
	    aff_check_string(spin->si_sofoto, sofoto, "SOFOTO");
	    spin->si_sofofr = sofofrom;
	    spin->si_sofoto = sofoto;
	}
    }

    यदि (midword != रिक्त)
    {
	aff_check_string(spin->si_midword, midword, "MIDWORD");
	spin->si_midword = midword;
    }

    vim_free(pc);
    ख_बंद(fd);
    प्रत्यागम aff;
}

/*
 * Return TRUE when items[0] equals "rulename", there are "mincount" items or
 * a comment is following after item "mincount".
 */
    स्थिर पूर्णाङ्क
is_aff_rule(
    char_u	**वस्तुएँ,
    पूर्णाङ्क		itemcnt,
    वर्ण	*rulename,
    पूर्णाङ्क		mincount)
{
    प्रत्यागम (STRCMP(वस्तुएँ[0], rulename) == 0
	    && (itemcnt == mincount
		|| (itemcnt > mincount && वस्तुएँ[mincount][0] == '#')));
}

/*
 * For affix "entry" move COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG from
 * ae_flags to ae_comppermit and ae_compforbid.
 */
    स्थिर शून्य
aff_process_flags(afffile_T *affile, affentry_T *entry)
{
    char_u	*p;
    char_u	*prevp;
    अचिन्हित	flag;

    यदि (entry->ae_flags != रिक्त
		&& (affile->af_compforbid != 0 || affile->af_comppermit != 0))
    {
	प्रति (p = entry->ae_flags; *p != NUL; )
	{
	    prevp = p;
	    flag = get_affitem(affile->af_flagtype, &p);
	    यदि (flag == affile->af_comppermit || flag == affile->af_compforbid)
	    {
		STRMOVE(prevp, p);
		p = prevp;
		यदि (flag == affile->af_comppermit)
		    entry->ae_comppermit = TRUE;
		अन्यथा
		    entry->ae_compforbid = TRUE;
	    }
	    यदि (affile->af_flagtype == AFT_अंक && *p == ',')
		++p;
	}
	यदि (*entry->ae_flags == NUL)
	    entry->ae_flags = रिक्त;	// nothing left
    }
}

/*
 * Return TRUE if "s" is the name of an info item in the affix file.
 */
    स्थिर पूर्णाङ्क
spell_info_item(char_u *s)
{
    प्रत्यागम STRCMP(s, "NAME") == 0
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
    स्थिर अचिन्हित
affitem2flag(
    पूर्णाङ्क		flagtype,
    char_u	*item,
    char_u	*fname,
    पूर्णाङ्क		lnum)
{
    अचिन्हित	res;
    char_u	*p = item;

    res = get_affitem(flagtype, &p);
    यदि (res == 0)
    {
	यदि (flagtype == AFT_अंक)
	    smsg(_("Flag is not a number in %s line %d: %s"),
							   fname, lnum, item);
	अन्यथा
	    smsg(_("Illegal flag in %s line %d: %s"),
							   fname, lnum, item);
    }
    यदि (*p != NUL)
    {
	smsg(_(e_affname), fname, lnum, item);
	प्रत्यागम 0;
    }

    प्रत्यागम res;
}

/*
 * Get one affix name from "*pp" and advance the pointer.
 * Returns ZERO_FLAG for "0".
 * Returns zero for an error, still advances the pointer then.
 */
    स्थिर अचिन्हित
get_affitem(पूर्णाङ्क flagtype, char_u **pp)
{
    पूर्णाङ्क		res;

    यदि (flagtype == AFT_अंक)
    {
	यदि (!VIM_ISDIGIT(**pp))
	{
	    ++*pp;	// always advance, avoid getting stuck
	    प्रत्यागम 0;
	}
	res = getdigits(pp);
	यदि (res == 0)
	    res = ZERO_FLAG;
    }
    अन्यथा
    {
	res = mb_ptr2char_adv(pp);
	यदि (flagtype == AFT_LONG || (flagtype == AFT_CAPLONG
						 && res >= 'A' && res <= 'Z'))
	{
	    यदि (**pp == NUL)
		प्रत्यागम 0;
	    res = mb_ptr2char_adv(pp) + (res << 16);
	}
    }
    प्रत्यागम res;
}

/*
 * Process the "compflags" string used in an affix file and append it to
 * spin->si_compflags.
 * The processing involves changing the affix names to ID numbers, so that
 * they fit in one byte.
 */
    स्थिर शून्य
process_compflags(
    spellinfo_T	*spin,
    afffile_T	*aff,
    char_u	*compflags)
{
    char_u	*p;
    char_u	*prevp;
    अचिन्हित	flag;
    compitem_T	*ci;
    पूर्णाङ्क		id;
    पूर्णाङ्क		len;
    char_u	*tp;
    char_u	कुंजी[AH_कुंजी_LEN];
    hashitem_T	*hi;

    // Make room for the old and the new compflags, concatenated with a / in
    // between.  Processing it makes it shorter, but we don't know by how
    // much, thus allocate the maximum.
    len = (पूर्णाङ्क)STRLEN(compflags) + 1;
    यदि (spin->si_compflags != रिक्त)
	len += (पूर्णाङ्क)STRLEN(spin->si_compflags) + 1;
    p = getroom(spin, len, FALSE);
    यदि (p == रिक्त)
	प्रत्यागम;
    यदि (spin->si_compflags != रिक्त)
    {
	STRCPY(p, spin->si_compflags);
	STRCAT(p, "/");
    }
    spin->si_compflags = p;
    tp = p + STRLEN(p);

    प्रति (p = compflags; *p != NUL; )
    {
	यदि (vim_strchr((char_u *)"/?*+[]", *p) != रिक्त)
	    // Copy non-flag characters directly.
	    *tp++ = *p++;
	अन्यथा
	{
	    // First get the flag number, also checks validity.
	    prevp = p;
	    flag = get_affitem(aff->af_flagtype, &p);
	    यदि (flag != 0)
	    {
		// Find the flag in the hashtable.  If it was used before, use
		// the existing ID.  Otherwise add a new entry.
		vim_strncpy(कुंजी, prevp, p - prevp);
		hi = hash_find(&aff->af_comp, कुंजी);
		यदि (!HASHITEM_EMPTY(hi))
		    id = HI2CI(hi)->ci_newID;
		अन्यथा
		{
		    ci = (compitem_T *)getroom(spin, परिमाण(compitem_T), TRUE);
		    यदि (ci == रिक्त)
			भङ्ग;
		    STRCPY(ci->ci_कुंजी, कुंजी);
		    ci->ci_flag = flag;
		    // Avoid using a flag ID that has a special meaning in a
		    // regexp (also inside []).
		    कुरु
		    {
			check_renumber(spin);
			id = spin->si_newcompID--;
		    } यावत् (vim_strchr((char_u *)"/?*+[]\\-^", id) != रिक्त);
		    ci->ci_newID = id;
		    hash_add(&aff->af_comp, ci->ci_कुंजी, "spelling");
		}
		*tp++ = id;
	    }
	    यदि (aff->af_flagtype == AFT_अंक && *p == ',')
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
    स्थिर शून्य
check_renumber(spellinfo_T *spin)
{
    यदि (spin->si_newprefID == spin->si_newcompID && spin->si_newcompID < 128)
    {
	spin->si_newprefID = 127;
	spin->si_newcompID = 255;
    }
}

/*
 * Append one affix or compound ID to "store_afflist".
 * Returns FAIL when this would overrun the fixed-size buffer.
 */
    स्थिर पूर्णाङ्क
store_afflist_add(
    char_u	*store_afflist,
    पूर्णाङ्क		*cntp,
    पूर्णाङ्क		id)
{
    यदि (*cntp >= MAXWLEN - 1)
    {
	emsg(_(e_too_many_postponed_prefixes_spell));
	प्रत्यागम FAIL;
    }
    store_afflist[(*cntp)++] = id;
    store_afflist[*cntp] = NUL;
    प्रत्यागम OK;
}

/*
 * Return TRUE if flag "flag" appears in affix list "afflist".
 */
    स्थिर पूर्णाङ्क
flag_in_afflist(पूर्णाङ्क flagtype, char_u *afflist, अचिन्हित flag)
{
    char_u	*p;
    अचिन्हित	n;

    विकल्प (flagtype)
    {
	पक्ष AFT_CHAR:
	    प्रत्यागम vim_strchr(afflist, flag) != रिक्त;

	पक्ष AFT_CAPLONG:
	पक्ष AFT_LONG:
	    प्रति (p = afflist; *p != NUL; )
	    {
		n = mb_ptr2char_adv(&p);
		यदि ((flagtype == AFT_LONG || (n >= 'A' && n <= 'Z'))
								 && *p != NUL)
		    n = mb_ptr2char_adv(&p) + (n << 16);
		यदि (n == flag)
		    प्रत्यागम TRUE;
	    }
	    भङ्ग;

	पक्ष AFT_अंक:
	    प्रति (p = afflist; *p != NUL; )
	    {
		n = getdigits(&p);
		यदि (n == 0)
		    n = ZERO_FLAG;
		यदि (n == flag)
		    प्रत्यागम TRUE;
		यदि (*p != NUL)	// skip over comma
		    ++p;
	    }
	    भङ्ग;
    }
    प्रत्यागम FALSE;
}

/*
 * Give a warning when "spinval" and "affval" numbers are set and not the same.
 */
    स्थिर शून्य
aff_check_संख्या(पूर्णाङ्क spinval, पूर्णाङ्क affval, वर्ण *name)
{
    यदि (spinval != 0 && spinval != affval)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Give a warning when "spinval" and "affval" strings are set and not the same.
 */
    स्थिर शून्य
aff_check_string(char_u *spinval, char_u *affval, वर्ण *name)
{
    यदि (spinval != रिक्त && STRCMP(spinval, affval) != 0)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Return TRUE if strings "s1" and "s2" are equal.  Also consider both being
 * NULL as equal.
 */
    स्थिर पूर्णाङ्क
str_equal(char_u *s1, char_u *s2)
{
    यदि (s1 == रिक्त || s2 == रिक्त)
	प्रत्यागम s1 == s2;
    प्रत्यागम STRCMP(s1, s2) == 0;
}

/*
 * Add a from-to item to "gap".  Used for REP and SAL items.
 * They are stored case-folded.
 */
    स्थिर शून्य
add_fromto(
    spellinfo_T	*spin,
    garray_T	*gap,
    char_u	*from,
    char_u	*to)
{
    fromto_T	*ftp;
    char_u	word[MAXWLEN];

    यदि (ga_grow(gap, 1) == FAIL)
	प्रत्यागम;

    ftp = ((fromto_T *)gap->ga_data) + gap->ga_len;
    (शून्य)spell_casefold(curwin, from, (पूर्णाङ्क)STRLEN(from), word, MAXWLEN);
    ftp->ft_from = getroom_save(spin, word);
    (शून्य)spell_casefold(curwin, to, (पूर्णाङ्क)STRLEN(to), word, MAXWLEN);
    ftp->ft_to = getroom_save(spin, word);
    ++gap->ga_len;
}

/*
 * Convert a boolean argument in a SAL line to TRUE or FALSE;
 */
    स्थिर पूर्णाङ्क
sal_to_bool(char_u *s)
{
    प्रत्यागम STRCMP(s, "1") == 0 || STRCMP(s, "true") == 0;
}

/*
 * Free the structure filled by spell_read_aff().
 */
    स्थिर शून्य
spell_free_aff(afffile_T *aff)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    पूर्णाङ्क		todo;
    affheader_T	*ah;
    affentry_T	*ae;

    vim_free(aff->af_enc);

    // All this trouble to free the "ae_prog" items...
    प्रति (ht = &aff->af_pref; ; ht = &aff->af_suff)
    {
	todo = (पूर्णाङ्क)ht->ht_used;
	FOR_ALL_HASHTAB_वस्तुएँ(ht, hi, todo)
	{
	    यदि (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		ah = HI2AH(hi);
		प्रति (ae = ah->ah_first; ae != रिक्त; ae = ae->ae_next)
		    vim_regfree(ae->ae_prog);
	    }
	}
	यदि (ht == &aff->af_suff)
	    भङ्ग;
    }

    hash_clear(&aff->af_pref);
    hash_clear(&aff->af_suff);
    hash_clear(&aff->af_comp);
}

/*
 * Read dictionary file "fname".
 * Returns OK or FAIL;
 */
    स्थिर पूर्णाङ्क
spell_read_dic(spellinfo_T *spin, char_u *fname, afffile_T *affile)
{
    hashtab_T	ht;
    char_u	पंक्ति[MAXLINELEN];
    char_u	*p;
    char_u	*afflist;
    char_u	store_afflist[MAXWLEN];
    पूर्णाङ्क		pfxlen;
    पूर्णाङ्क		totlen;
    पूर्णाङ्क		need_affix;
    char_u	*dw;
    char_u	*pc;
    char_u	*w;
    पूर्णाङ्क		l;
    hash_T	hash;
    hashitem_T	*hi;
    खाता	*fd;
    पूर्णाङ्क		lnum = 1;
    पूर्णाङ्क		non_ascii = 0;
    पूर्णाङ्क		retval = OK;
    char_u	message[MAXLINELEN + MAXWLEN];
    पूर्णाङ्क		flags;
    पूर्णाङ्क		duplicate = 0;
    time_T	last_संदेश_time = 0;

    /*
     * Open the file.
     */
    fd = mch_fopen((वर्ण *)fname, "r");
    यदि (fd == रिक्त)
    {
	semsg(_(e_cant_open_file_str), fname);
	प्रत्यागम FAIL;
    }

    // The hashtable is only used to detect duplicated words.
    hash_init(&ht);

    vim_snprintf((वर्ण *)IObuff, IOSIZE,
				  _("Reading dictionary file %s..."), fname);
    spell_message(spin, IObuff);

    // start with a message for the first line
    spin->si_संदेश_गिनती = 999999;

    // Read and ignore the first line: word count.
    यदि (vim_fgets(पंक्ति, MAXLINELEN, fd) || !vim_isdigit(*skipwhite(पंक्ति)))
	semsg(_(e_no_word_गिनती_in_str), fname);

    /*
     * Read all the lines in the file one by one.
     * The words are converted to 'encoding' here, before being added to
     * the hashtable.
     */
    यावत् (!vim_fgets(पंक्ति, MAXLINELEN, fd) && !got_int)
    {
	पंक्ति_breakcheck();
	++lnum;
	यदि (पंक्ति[0] == '#' || पंक्ति[0] == '/')
	    अनुवर्त;	// comment line

	// Remove CR, LF and white space from the end.  White space halfway
	// the word is kept to allow e.g., "et al.".
	l = (पूर्णाङ्क)STRLEN(पंक्ति);
	यावत् (l > 0 && पंक्ति[l - 1] <= ' ')
	    --l;
	यदि (l == 0)
	    अनुवर्त;	// empty line
	पंक्ति[l] = NUL;

	// Convert from "SET" to 'encoding' when needed.
	यदि (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, पंक्ति, रिक्त);
	    यदि (pc == रिक्त)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
						       fname, lnum, पंक्ति);
		अनुवर्त;
	    }
	    w = pc;
	}
	अन्यथा
	{
	    pc = रिक्त;
	    w = पंक्ति;
	}

	// Truncate the word at the "/", set "afflist" to what follows.
	// Replace "\/" by "/" and "\\" by "\".
	afflist = रिक्त;
	प्रति (p = w; *p != NUL; MB_PTR_ADV(p))
	{
	    यदि (*p == '\\' && (p[1] == '\\' || p[1] == '/'))
		STRMOVE(p, p + 1);
	    अन्यथा यदि (*p == '/')
	    {
		*p = NUL;
		afflist = p + 1;
		भङ्ग;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	यदि (spin->si_ascii && has_non_ascii(w))
	{
	    ++non_ascii;
	    vim_free(pc);
	    अनुवर्त;
	}

	// This takes time, print a message every 10000 words, but not more
	// often than once per second.
	यदि (spin->si_verbose && spin->si_संदेश_गिनती > 10000)
	{
	    spin->si_संदेश_गिनती = 0;
	    यदि (vim_time() > last_संदेश_time)
	    {
		last_संदेश_time = vim_time();
		vim_snprintf((वर्ण *)message, परिमाण(message),
			_("line %6d, word %6ld - %s"),
			   lnum, spin->si_foldwcount + spin->si_keepwcount, w);
		संदेश_start();
		संदेश_outtrans_long_attr(message, 0);
		संदेश_clr_eos();
		संदेश_didout = FALSE;
		संदेश_col = 0;
		out_flush();
	    }
	}

	// Store the word in the hashtable to be able to find duplicates.
	dw = getroom_save(spin, w);
	यदि (dw == रिक्त)
	{
	    retval = FAIL;
	    vim_free(pc);
	    भङ्ग;
	}

	hash = hash_hash(dw);
	hi = hash_lookup(&ht, dw, hash);
	यदि (!HASHITEM_EMPTY(hi))
	{
	    यदि (p_verbose > 0)
		smsg(_("Duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    अन्यथा यदि (duplicate == 0)
		smsg(_("First duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ++duplicate;
	}
	अन्यथा
	    hash_add_item(&ht, hi, dw, hash);

	flags = 0;
	store_afflist[0] = NUL;
	pfxlen = 0;
	totlen = 0;
	need_affix = FALSE;
	यदि (afflist != रिक्त)
	{
	    // Extract flags from the affix list.
	    flags |= get_affix_flags(affile, afflist);

	    यदि (affile->af_needaffix != 0 && flag_in_afflist(
			  affile->af_flagtype, afflist, affile->af_needaffix))
		need_affix = TRUE;

	    यदि (affile->af_pfxpostpone)
	    {
		// Need to store the list of prefix IDs with the word.
		यदि (get_pfxlist(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    भङ्ग;
		}
		pfxlen = totlen;
	    }

	    यदि (spin->si_compflags != रिक्त)
	    {
		// Need to store the list of compound flags with the word.
		// Concatenate them to the list of prefix IDs.
		यदि (get_compflags(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    भङ्ग;
		}
	    }
	}

	// Add the word to the word tree(s).
	यदि (store_word(spin, dw, flags, spin->si_region,
					   store_afflist, need_affix) == FAIL)
	    retval = FAIL;

	यदि (afflist != रिक्त)
	{
	    // Find all matching suffixes and add the resulting words.
	    // Additionally do matching prefixes that combine.
	    यदि (store_aff_word(spin, dw, afflist, affile,
			   &affile->af_suff, &affile->af_pref,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;

	    // Find all matching prefixes and add the resulting words.
	    यदि (store_aff_word(spin, dw, afflist, affile,
			  &affile->af_pref, रिक्त,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;
	}

	vim_free(pc);
    }

    यदि (duplicate > 0)
	smsg(_("%d duplicate word(s) in %s"), duplicate, fname);
    यदि (spin->si_ascii && non_ascii > 0)
	smsg(_("Ignored %d word(s) with non-ASCII characters in %s"),
							    non_ascii, fname);
    hash_clear(&ht);

    ख_बंद(fd);
    प्रत्यागम retval;
}

/*
 * Check for affix flags in "afflist" that are turned into word flags.
 * Return WF_ flags.
 */
    स्थिर पूर्णाङ्क
get_affix_flags(afffile_T *affile, char_u *afflist)
{
    पूर्णाङ्क		flags = 0;

    यदि (affile->af_keepcase != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_keepcase))
	flags |= WF_KEEPCAP | WF_FIXCAP;
    यदि (affile->af_rare != 0 && flag_in_afflist(
			       affile->af_flagtype, afflist, affile->af_rare))
	flags |= WF_RARE;
    यदि (affile->af_bad != 0 && flag_in_afflist(
				affile->af_flagtype, afflist, affile->af_bad))
	flags |= WF_BANNED;
    यदि (affile->af_needcomp != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_needcomp))
	flags |= WF_NEEDCOMP;
    यदि (affile->af_comproot != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_comproot))
	flags |= WF_COMPROOT;
    यदि (affile->af_nosuggest != 0 && flag_in_afflist(
			  affile->af_flagtype, afflist, affile->af_nosuggest))
	flags |= WF_NOSUGGEST;
    प्रत्यागम flags;
}

/*
 * Get the list of prefix IDs from the affix list "afflist".
 * Used for PFXPOSTPONE.
 * Put the resulting flags in "store_afflist[MAXWLEN]" with a terminating NUL.
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    स्थिर पूर्णाङ्क
get_pfxlist(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    पूर्णाङ्क		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    पूर्णाङ्क		id;
    char_u	कुंजी[AH_कुंजी_LEN];
    hashitem_T	*hi;

    प्रति (p = afflist; *p != NUL; )
    {
	prevp = p;
	यदि (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a postponed prefix flag if it appears in "af_pref"
	    // and its ID is not zero.
	    vim_strncpy(कुंजी, prevp, p - prevp);
	    hi = hash_find(&affile->af_pref, कुंजी);
	    यदि (!HASHITEM_EMPTY(hi))
	    {
		id = HI2AH(hi)->ah_newID;
		यदि (id != 0 && store_afflist_add(store_afflist, cntp, id) == FAIL)
		    प्रत्यागम FAIL;
	    }
	}
	यदि (affile->af_flagtype == AFT_अंक && *p == ',')
	    ++p;
    }

    प्रत्यागम OK;
}

/*
 * Get the list of compound IDs from the affix list "afflist" that are used
 * for compound words.
 * Puts the flags in "store_afflist[]".
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    स्थिर पूर्णाङ्क
get_compflags(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    पूर्णाङ्क		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    char_u	कुंजी[AH_कुंजी_LEN];
    hashitem_T	*hi;

    प्रति (p = afflist; *p != NUL; )
    {
	prevp = p;
	यदि (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a compound flag if it appears in "af_comp".
	    vim_strncpy(कुंजी, prevp, p - prevp);
	    hi = hash_find(&affile->af_comp, कुंजी);
	    यदि (!HASHITEM_EMPTY(hi)
		    && store_afflist_add(store_afflist, cntp,
					      HI2CI(hi)->ci_newID) == FAIL)
		प्रत्यागम FAIL;
	}
	यदि (affile->af_flagtype == AFT_अंक && *p == ',')
	    ++p;
    }

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
store_aff_word(
    spellinfo_T	*spin,		// spell info
    char_u	*word,		// basic word start
    char_u	*afflist,	// list of names of supported affixes
    afffile_T	*affile,
    hashtab_T	*ht,
    hashtab_T	*xht,
    पूर्णाङ्क		condit,		// CONDIT_SUF et al.
    पूर्णाङ्क		flags,		// flags for the word
    char_u	*pfxlist,	// list of prefix IDs
    पूर्णाङ्क		pfxlen)		// nr of flags in "pfxlist" for prefixes, rest
				// is compound flags
{
    पूर्णाङ्क		todo;
    hashitem_T	*hi;
    affheader_T	*ah;
    affentry_T	*ae;
    char_u	newword[MAXWLEN];
    पूर्णाङ्क		retval = OK;
    पूर्णाङ्क		i, j;
    char_u	*p;
    पूर्णाङ्क		use_flags;
    char_u	*use_pfxlist;
    पूर्णाङ्क		use_pfxlen;
    पूर्णाङ्क		need_affix;
    char_u	store_afflist[MAXWLEN];
    char_u	pfx_pfxlist[MAXWLEN];
    माप_प्रकार	wordlen = STRLEN(word);
    पूर्णाङ्क		use_condit;

    todo = (पूर्णाङ्क)ht->ht_used;
    प्रति (hi = ht->ht_array; todo > 0 && retval == OK; ++hi)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ah = HI2AH(hi);

	    // Check that the affix combines, if required, and that the word
	    // supports this affix.
	    यदि (((condit & CONDIT_COMB) == 0 || ah->ah_combine)
		    && flag_in_afflist(affile->af_flagtype, afflist,
								 ah->ah_flag))
	    {
		// Loop over all affix entries with this name.
		प्रति (ae = ah->ah_first; ae != रिक्त; ae = ae->ae_next)
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
		    यदि ((xht != रिक्त || !affile->af_pfxpostpone
				|| ae->ae_chop != रिक्त
				|| ae->ae_flags != रिक्त)
			    && (ae->ae_chop == रिक्त
				|| STRLEN(ae->ae_chop) < wordlen)
			    && (ae->ae_prog == रिक्त
				|| vim_regexec_prog(&ae->ae_prog, FALSE,
							    word, (colnr_T)0))
			    && (((condit & CONDIT_CFIX) == 0)
				== ((condit & CONDIT_AFF) == 0
				    || ae->ae_flags == रिक्त
				    || !flag_in_afflist(affile->af_flagtype,
					ae->ae_flags, affile->af_circumfix))))
		    {
			// Match.  Remove the chop and add the affix.
			यदि (xht == रिक्त)
			{
			    // prefix: chop/add at the start of the word
			    यदि (ae->ae_add == रिक्त)
				*newword = NUL;
			    अन्यथा
				vim_strncpy(newword, ae->ae_add, MAXWLEN - 1);
			    p = word;
			    यदि (ae->ae_chop != रिक्त)
			    {
				// Skip chop string.
				यदि (has_mbyte)
				{
				    i = mb_charlen(ae->ae_chop);
				    प्रति ( ; i > 0; --i)
					MB_PTR_ADV(p);
				}
				अन्यथा
				    p += STRLEN(ae->ae_chop);
			    }
			    // Note: this silently truncates the buffer, but this should
			    // not happen in practice
			    STRNCAT(newword, p, MAXWLEN - STRLEN(newword) - 1);
			}
			अन्यथा
			{
			    // suffix: chop/add at the end of the word
			    vim_strncpy(newword, word, MAXWLEN - 1);
			    यदि (ae->ae_chop != रिक्त)
			    {
				// Remove chop string.
				p = newword + STRLEN(newword);
				i = (पूर्णाङ्क)MB_CHARLEN(ae->ae_chop);
				प्रति ( ; i > 0; --i)
				    MB_PTR_BACK(newword, p);
				*p = NUL;
			    }
			    यदि (ae->ae_add != रिक्त)
				// Note: this silently truncates the buffer, but this should
				// not happen in practice
				STRNCAT(newword, ae->ae_add, MAXWLEN - STRLEN(newword) - 1);
			}

			use_flags = flags;
			use_pfxlist = pfxlist;
			use_pfxlen = pfxlen;
			need_affix = FALSE;
			use_condit = condit | CONDIT_COMB | CONDIT_AFF;
			यदि (ae->ae_flags != रिक्त)
			{
			    // Extract flags from the affix list.
			    use_flags |= get_affix_flags(affile, ae->ae_flags);

			    यदि (affile->af_needaffix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_needaffix))
				need_affix = TRUE;

			    // When there is a CIRCUMFIX flag the other affix
			    // must also have it and we don't add the word
			    // with one affix.
			    यदि (affile->af_circumfix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_circumfix))
			    {
				use_condit |= CONDIT_CFIX;
				यदि ((condit & CONDIT_CFIX) == 0)
				    need_affix = TRUE;
			    }

			    यदि (affile->af_pfxpostpone
						|| spin->si_compflags != रिक्त)
			    {
				पूर्णाङ्क listlen = 0;

				यदि (affile->af_pfxpostpone)
				{
				    // Get prefix IDS from the affix list.
				    यदि (get_pfxlist(affile, ae->ae_flags,
						    store_afflist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					भङ्ग;
				    }
				    use_pfxlen = listlen;
				}
				अन्यथा
				    use_pfxlen = 0;
				use_pfxlist = store_afflist;

				// Combine the prefix IDs. Avoid adding the
				// same ID twice.
				प्रति (i = 0; i < pfxlen; ++i)
				{
				    प्रति (j = 0; j < use_pfxlen; ++j)
					यदि (pfxlist[i] == use_pfxlist[j])
					    भङ्ग;
				    यदि (j == use_pfxlen
					    && store_afflist_add(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					भङ्ग;
				    }
				    use_pfxlen = listlen;
				}
				यदि (retval == FAIL)
				    भङ्ग;

				यदि (spin->si_compflags != रिक्त)
				    // Get compound IDS from the affix list.
				    यदि (get_compflags(affile, ae->ae_flags,
						      use_pfxlist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					भङ्ग;
				    }
				यदि (retval == FAIL)
				    भङ्ग;

				// Combine the list of compound flags.
				// Concatenate them to the prefix IDs list.
				// Avoid adding the same ID twice.
				प्रति (i = pfxlen; pfxlist[i] != NUL; ++i)
				{
				    प्रति (j = use_pfxlen;
						   use_pfxlist[j] != NUL; ++j)
					यदि (pfxlist[i] == use_pfxlist[j])
					    भङ्ग;
				    यदि (use_pfxlist[j] == NUL
					    && store_afflist_add(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					भङ्ग;
				    }
				}
				यदि (retval == FAIL)
				    भङ्ग;
			    }
			}

			// Obey a "COMPOUNDFORBIDFLAG" of the affix: don't
			// use the compound flags.
			यदि (use_pfxlist != रिक्त && ae->ae_compforbid)
			{
			    vim_strncpy(pfx_pfxlist, use_pfxlist, use_pfxlen);
			    use_pfxlist = pfx_pfxlist;
			}

			// When there are postponed prefixes...
			यदि (spin->si_prefroot != रिक्त
				&& spin->si_prefroot->wn_sibling != रिक्त)
			{
			    // ... add a flag to indicate an affix was used.
			    use_flags |= WF_HAS_AFF;

			    // ... don't use a prefix list if combining
			    // affixes is not allowed.  But do use the
			    // compound flags after them.
			    यदि (!ah->ah_combine && use_pfxlist != रिक्त)
				use_pfxlist += use_pfxlen;
			}

			// When compounding is supported and there is no
			// "COMPOUNDPERMITFLAG" then forbid compounding on the
			// side where the affix is applied.
			यदि (spin->si_compflags != रिक्त && !ae->ae_comppermit)
			{
			    यदि (xht != रिक्त)
				use_flags |= WF_NOCOMPAFT;
			    अन्यथा
				use_flags |= WF_NOCOMPBEF;
			}

			// Store the modified word.
			यदि (store_word(spin, newword, use_flags,
						 spin->si_region, use_pfxlist,
							  need_affix) == FAIL)
			    retval = FAIL;

			// When added a prefix or a first suffix and the affix
			// has flags may add a(nother) suffix.  RECURSIVE!
			यदि ((condit & CONDIT_SUF) && ae->ae_flags != रिक्त)
			    यदि (store_aff_word(spin, newword, ae->ae_flags,
					affile, &affile->af_suff, xht,
					   use_condit & (xht == रिक्त
							? ~0 :  ~CONDIT_SUF),
				      use_flags, use_pfxlist, pfxlen) == FAIL)
				retval = FAIL;

			// When added a suffix and combining is allowed also
			// try adding a prefix additionally.  Both for the
			// word flags and for the affix flags.  RECURSIVE!
			यदि (xht != रिक्त && ah->ah_combine)
			{
			    यदि (store_aff_word(spin, newword,
					afflist, affile,
					xht, रिक्त, use_condit,
					use_flags, use_pfxlist,
					pfxlen) == FAIL
				    || (ae->ae_flags != रिक्त
					&& store_aff_word(spin, newword,
					    ae->ae_flags, affile,
					    xht, रिक्त, use_condit,
					    use_flags, use_pfxlist,
					    pfxlen) == FAIL))
				retval = FAIL;
			}
		    }
		}
	    }
	}
    }

    प्रत्यागम retval;
}

/*
 * Read a file with a list of words.
 */
    स्थिर पूर्णाङ्क
spell_read_wordfile(spellinfo_T *spin, char_u *fname)
{
    खाता	*fd;
    दीर्घ	lnum = 0;
    char_u	rline[MAXLINELEN];
    char_u	*पंक्ति;
    char_u	*pc = रिक्त;
    char_u	*p;
    पूर्णाङ्क		l;
    पूर्णाङ्क		retval = OK;
    पूर्णाङ्क		did_word = FALSE;
    पूर्णाङ्क		non_ascii = 0;
    पूर्णाङ्क		flags;
    पूर्णाङ्क		regionmask;

    /*
     * Open the file.
     */
    fd = mch_fopen((वर्ण *)fname, "r");
    यदि (fd == रिक्त)
    {
	semsg(_(e_cant_open_file_str), fname);
	प्रत्यागम FAIL;
    }

    vim_snprintf((वर्ण *)IObuff, IOSIZE, _("Reading word file %s..."), fname);
    spell_message(spin, IObuff);

    /*
     * Read all the lines in the file one by one.
     */
    यावत् (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	पंक्ति_breakcheck();
	++lnum;

	// Skip comment lines.
	यदि (*rline == '#')
	    अनुवर्त;

	// Remove CR, LF and white space from the end.
	l = (पूर्णाङ्क)STRLEN(rline);
	यावत् (l > 0 && rline[l - 1] <= ' ')
	    --l;
	यदि (l == 0)
	    अनुवर्त;	// empty or blank line
	rline[l] = NUL;

	// Convert from "/encoding={encoding}" to 'encoding' when needed.
	vim_free(pc);
	यदि (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, रिक्त);
	    यदि (pc == रिक्त)
	    {
		smsg(_("Conversion failure for word in %s line %ld: %s"),
							   fname, lnum, rline);
		अनुवर्त;
	    }
	    पंक्ति = pc;
	}
	अन्यथा
	{
	    pc = रिक्त;
	    पंक्ति = rline;
	}

	यदि (*पंक्ति == '/')
	{
	    ++पंक्ति;
	    यदि (STRNCMP(पंक्ति, "encoding=", 9) == 0)
	    {
		यदि (spin->si_conv.vc_type != CONV_NONE)
		    smsg(_("Duplicate /encoding= line ignored in %s line %ld: %s"),
						       fname, lnum, पंक्ति - 1);
		अन्यथा यदि (did_word)
		    smsg(_("/encoding= line after word ignored in %s line %ld: %s"),
						       fname, lnum, पंक्ति - 1);
		अन्यथा
		{
		    char_u	*enc;

		    // Setup for conversion to 'encoding'.
		    पंक्ति += 9;
		    enc = enc_canonize(पंक्ति);
		    यदि (enc != रिक्त && !spin->si_ascii
			    && convert_setup(&spin->si_conv, enc,
							       p_enc) == FAIL)
			smsg(_("Conversion in %s not supported: from %s to %s"),
							  fname, पंक्ति, p_enc);
		    vim_free(enc);
		    spin->si_conv.vc_fail = TRUE;
		}
		अनुवर्त;
	    }

	    यदि (STRNCMP(पंक्ति, "regions=", 8) == 0)
	    {
		यदि (spin->si_region_गिनती > 1)
		    smsg(_("Duplicate /regions= line ignored in %s line %ld: %s"),
						       fname, lnum, पंक्ति);
		अन्यथा
		{
		    पंक्ति += 8;
		    यदि (STRLEN(पंक्ति) > MAXREGIONS * 2)
			smsg(_("Too many regions in %s line %ld: %s"),
						       fname, lnum, पंक्ति);
		    अन्यथा
		    {
			spin->si_region_गिनती = (पूर्णाङ्क)STRLEN(पंक्ति) / 2;
			STRCPY(spin->si_region_name, पंक्ति);

			// Adjust the mask for a word valid in all regions.
			spin->si_region = (1 << spin->si_region_गिनती) - 1;
		    }
		}
		अनुवर्त;
	    }

	    smsg(_("/ line ignored in %s line %ld: %s"),
						       fname, lnum, पंक्ति - 1);
	    अनुवर्त;
	}

	flags = 0;
	regionmask = spin->si_region;

	// Check for flags and region after a slash.
	p = vim_strchr(पंक्ति, '/');
	यदि (p != रिक्त)
	{
	    *p++ = NUL;
	    यावत् (*p != NUL)
	    {
		यदि (*p == '=')		// keep-case word
		    flags |= WF_KEEPCAP | WF_FIXCAP;
		अन्यथा यदि (*p == '!')	// Bad, bad, wicked word.
		    flags |= WF_BANNED;
		अन्यथा यदि (*p == '?')	// Rare word.
		    flags |= WF_RARE;
		अन्यथा यदि (VIM_ISDIGIT(*p)) // region number(s)
		{
		    यदि ((flags & WF_REGION) == 0)   // first one
			regionmask = 0;
		    flags |= WF_REGION;

		    l = *p - '0';
		    यदि (l == 0 || l > spin->si_region_गिनती)
		    {
			smsg(_("Invalid region nr in %s line %ld: %s"),
							  fname, lnum, p);
			भङ्ग;
		    }
		    regionmask |= 1 << (l - 1);
		}
		अन्यथा
		{
		    smsg(_("Unrecognized flags in %s line %ld: %s"),
							      fname, lnum, p);
		    भङ्ग;
		}
		++p;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	यदि (spin->si_ascii && has_non_ascii(पंक्ति))
	{
	    ++non_ascii;
	    अनुवर्त;
	}

	// Normal word: store it.
	यदि (store_word(spin, पंक्ति, flags, regionmask, रिक्त, FALSE) == FAIL)
	{
	    retval = FAIL;
	    भङ्ग;
	}
	did_word = TRUE;
    }

    vim_free(pc);
    ख_बंद(fd);

    यदि (spin->si_ascii && non_ascii > 0)
    {
	vim_snprintf((वर्ण *)IObuff, IOSIZE,
		  _("Ignored %d words with non-ASCII characters"), non_ascii);
	spell_message(spin, IObuff);
    }

    प्रत्यागम retval;
}

/*
 * Get part of an sblock_T, "len" bytes long.
 * This avoids calling free() for every little struct we use (and keeping
 * track of them).
 * The memory is cleared to all zeros.
 * Returns NULL when out of memory.
 */
    स्थिर शून्य *
getroom(
    spellinfo_T *spin,
    माप_प्रकार	len,		// length needed
    पूर्णाङ्क		align)		// align for pointer
{
    char_u	*p;
    sblock_T	*bl = spin->si_blocks;

    यदि (align && bl != रिक्त)
	// Round size up for alignment.  On some systems structures need to be
	// aligned to the size of a pointer (e.g., SPARC).
	bl->sb_used = (bl->sb_used + परिमाण(वर्ण *) - 1)
						      & ~(परिमाण(वर्ण *) - 1);

    यदि (bl == रिक्त || bl->sb_used + len > SBLOCKSIZE)
    {
	यदि (len >= SBLOCKSIZE)
	    bl = रिक्त;
	अन्यथा
	    // Allocate a block of memory. It is not freed until much later.
	    bl = alloc_clear(दुरत्व(sblock_T, sb_data) + SBLOCKSIZE + 1);
	यदि (bl == रिक्त)
	{
	    यदि (!spin->si_did_emsg)
	    {
		emsg(_(e_insufficient_memory_word_list_will_be_incomplete));
		spin->si_did_emsg = TRUE;
	    }
	    प्रत्यागम रिक्त;
	}
	bl->sb_next = spin->si_blocks;
	spin->si_blocks = bl;
	bl->sb_used = 0;
	++spin->si_blocks_cnt;
    }

    p = bl->sb_data + bl->sb_used;
    bl->sb_used += (पूर्णाङ्क)len;

    प्रत्यागम p;
}

/*
 * Make a copy of a string into memory allocated with getroom().
 * Returns NULL when out of memory.
 */
    स्थिर char_u *
getroom_save(spellinfo_T *spin, char_u *s)
{
    char_u	*sc;

    sc = (char_u *)getroom(spin, STRLEN(s) + 1, FALSE);
    यदि (sc != रिक्त)
	STRCPY(sc, s);
    प्रत्यागम sc;
}


/*
 * Free the list of allocated sblock_T.
 */
    स्थिर शून्य
free_blocks(sblock_T *bl)
{
    sblock_T	*next;

    यावत् (bl != रिक्त)
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
    स्थिर wordnode_T *
wordtree_alloc(spellinfo_T *spin)
{
    प्रत्यागम (wordnode_T *)getroom(spin, परिमाण(wordnode_T), TRUE);
}

/*
 * Return TRUE if "word" contains valid word characters.
 * Control characters and trailing '/' are invalid.  Space is OK.
 */
    स्थिर पूर्णाङ्क
valid_spell_word(char_u *word, char_u *end)
{
    char_u *p;

    यदि (enc_utf8 && !utf_valid_string(word, end))
	प्रत्यागम FALSE;
    प्रति (p = word; *p != NUL && p < end; p += mb_ptr2len(p))
	यदि (*p < ' ' || (p[0] == '/' && p[1] == NUL))
	    प्रत्यागम FALSE;
    प्रत्यागम TRUE;
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
    स्थिर पूर्णाङ्क
store_word(
    spellinfo_T	*spin,
    char_u	*word,
    पूर्णाङ्क		flags,		// extra flags, WF_BANNED
    पूर्णाङ्क		region,		// supported region(s)
    char_u	*pfxlist,	// list of prefix IDs or NULL
    पूर्णाङ्क		need_affix)	// only store word with affix ID
{
    पूर्णाङ्क		len = (पूर्णाङ्क)STRLEN(word);
    पूर्णाङ्क		ct = captype(word, word + len);
    char_u	foldword[MAXWLEN];
    पूर्णाङ्क		res = OK;
    char_u	*p;

    // Avoid adding illegal bytes to the word tree.
    यदि (!valid_spell_word(word, word + len))
	प्रत्यागम FAIL;

    (शून्य)spell_casefold(curwin, word, len, foldword, MAXWLEN);
    प्रति (p = pfxlist; res == OK; ++p)
    {
	यदि (!need_affix || (p != रिक्त && *p != NUL))
	    res = वृक्ष_add_word(spin, foldword, spin->si_foldroot, ct | flags,
						  region, p == रिक्त ? 0 : *p);
	यदि (p == रिक्त || *p == NUL)
	    भङ्ग;
    }
    ++spin->si_foldwcount;

    यदि (res == OK && (ct == WF_KEEPCAP || (flags & WF_KEEPCAP)))
    {
	प्रति (p = pfxlist; res == OK; ++p)
	{
	    यदि (!need_affix || (p != रिक्त && *p != NUL))
		res = वृक्ष_add_word(spin, word, spin->si_keeproot, flags,
						  region, p == रिक्त ? 0 : *p);
	    यदि (p == रिक्त || *p == NUL)
		भङ्ग;
	}
	++spin->si_keepwcount;
    }
    प्रत्यागम res;
}

/*
 * Add word "word" to a word tree at "root".
 * When "flags" < 0 we are adding to the prefix tree where "flags" is used for
 * "rare" and "region" is the condition nr.
 * Returns FAIL when out of memory.
 */
    स्थिर पूर्णाङ्क
वृक्ष_add_word(
    spellinfo_T	*spin,
    char_u	*word,
    wordnode_T	*मूल,
    पूर्णाङ्क		flags,
    पूर्णाङ्क		region,
    पूर्णाङ्क		affixID)
{
    wordnode_T	*गाँठ = मूल;
    wordnode_T	*np;
    wordnode_T	*copyp, **copyprev;
    wordnode_T	**prev = रिक्त;
    पूर्णाङ्क		i;

    // Add each byte of the word to the tree, including the NUL at the end.
    प्रति (i = 0; ; ++i)
    {
	// When there is more than one reference to this node we need to make
	// a copy, so that we can modify it.  Copy the whole list of siblings
	// (we don't optimize for a partly shared list of siblings).
	यदि (गाँठ != रिक्त && गाँठ->wn_refs > 1)
	{
	    --गाँठ->wn_refs;
	    copyprev = prev;
	    FOR_ALL_गाँठ_SIBLINGS(गाँठ, copyp)
	    {
		// Allocate a new node and copy the info.
		np = get_wordnode(spin);
		यदि (np == रिक्त)
		    प्रत्यागम FAIL;
		np->wn_child = copyp->wn_child;
		यदि (np->wn_child != रिक्त)
		    ++np->wn_child->wn_refs;	// child gets extra ref
		np->wn_byte = copyp->wn_byte;
		यदि (np->wn_byte == NUL)
		{
		    np->wn_flags = copyp->wn_flags;
		    np->wn_region = copyp->wn_region;
		    np->wn_affixID = copyp->wn_affixID;
		}

		// Link the new node in the list, there will be one ref.
		np->wn_refs = 1;
		यदि (copyprev != रिक्त)
		    *copyprev = np;
		copyprev = &np->wn_sibling;

		// Let "node" point to the head of the copied list.
		यदि (copyp == गाँठ)
		    गाँठ = np;
	    }
	}

	// Look for the sibling that has the same character.  They are sorted
	// on byte value, thus stop searching when a sibling is found with a
	// higher byte value.  For zero bytes (end of word) the sorting is
	// done on flags and then on affixID.
	यावत् (गाँठ != रिक्त
		&& (गाँठ->wn_byte < word[i]
		    || (गाँठ->wn_byte == NUL
			&& (flags < 0
			    ? गाँठ->wn_affixID < (अचिन्हित)affixID
			    : (गाँठ->wn_flags < (अचिन्हित)(flags & WN_MASK)
				|| (गाँठ->wn_flags == (flags & WN_MASK)
				    && (spin->si_sugtree
					? (गाँठ->wn_region & 0xffff) < region
					: गाँठ->wn_affixID
						    < (अचिन्हित)affixID)))))))
	{
	    prev = &गाँठ->wn_sibling;
	    गाँठ = *prev;
	}
	यदि (गाँठ == रिक्त
		|| गाँठ->wn_byte != word[i]
		|| (word[i] == NUL
		    && (flags < 0
			|| spin->si_sugtree
			|| गाँठ->wn_flags != (flags & WN_MASK)
			|| गाँठ->wn_affixID != affixID)))
	{
	    // Allocate a new node.
	    np = get_wordnode(spin);
	    यदि (np == रिक्त)
		प्रत्यागम FAIL;
	    np->wn_byte = word[i];

	    // If "node" is NULL this is a new child or the end of the sibling
	    // list: ref count is one.  Otherwise use ref count of sibling and
	    // make ref count of sibling one (matters when inserting in front
	    // of the list of siblings).
	    यदि (गाँठ == रिक्त)
		np->wn_refs = 1;
	    अन्यथा
	    {
		np->wn_refs = गाँठ->wn_refs;
		गाँठ->wn_refs = 1;
	    }
	    यदि (prev != रिक्त)
		*prev = np;
	    np->wn_sibling = गाँठ;
	    गाँठ = np;
	}

	यदि (word[i] == NUL)
	{
	    गाँठ->wn_flags = flags;
	    गाँठ->wn_region |= region;
	    गाँठ->wn_affixID = affixID;
	    भङ्ग;
	}
	prev = &गाँठ->wn_child;
	गाँठ = *prev;
    }
#ifdef SPELL_PRINTTREE
    smsg("Added \"%s\"", word);
    spell_print_वृक्ष(मूल->wn_sibling);
#endif

    // count nr of words added since last message
    ++spin->si_संदेश_गिनती;

    यदि (spin->si_compress_cnt > 1)
    {
	यदि (--spin->si_compress_cnt == 1)
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
    यदि (spin->si_compress_cnt == 1
	    ? spin->si_free_गिनती < MAXWLEN
	    : spin->si_blocks_cnt >= compress_start)
#endif
    {
	// Decrement the block counter.  The effect is that we compress again
	// when the freed up room has been used and another "compress_inc"
	// blocks have been allocated.  Unless "compress_added" words have
	// been added, then the limit is put back again.
	spin->si_blocks_cnt -= compress_inc;
	spin->si_compress_cnt = compress_added;

	यदि (spin->si_verbose)
	{
	    संदेश_start();
	    संदेश_puts(_(संदेश_compressing));
	    संदेश_clr_eos();
	    संदेश_didout = FALSE;
	    संदेश_col = 0;
	    out_flush();
	}

	// Compress both trees.  Either they both have many nodes, which makes
	// compression useful, or one of them is small, which means
	// compression goes fast.  But when filling the soundfold word tree
	// there is no keep-case tree.
	wordtree_compress(spin, spin->si_foldroot, "case-folded");
	यदि (affixID >= 0)
	    wordtree_compress(spin, spin->si_keeproot, "keep-case");
    }

    प्रत्यागम OK;
}

/*
 * Get a wordnode_T, either from the list of previously freed nodes or
 * allocate a new one.
 * Returns NULL when out of memory.
 */
    स्थिर wordnode_T *
get_wordnode(spellinfo_T *spin)
{
    wordnode_T *n;

    यदि (spin->si_first_free == रिक्त)
	n = (wordnode_T *)getroom(spin, परिमाण(wordnode_T), TRUE);
    अन्यथा
    {
	n = spin->si_first_free;
	spin->si_first_free = n->wn_child;
	CLEAR_POINTER(n);
	--spin->si_free_गिनती;
    }
#ifdef SPELL_PRINTTREE
    यदि (n != रिक्त)
	n->wn_nr = ++spin->si_wordnode_nr;
#endif
    प्रत्यागम n;
}

/*
 * Decrement the reference count on a node (which is the head of a list of
 * siblings).  If the reference count becomes zero free the node and its
 * siblings.
 * Returns the number of nodes actually freed.
 */
    स्थिर पूर्णाङ्क
deref_wordnode(spellinfo_T *spin, wordnode_T *गाँठ)
{
    wordnode_T	*np;
    पूर्णाङ्क		cnt = 0;

    यदि (--गाँठ->wn_refs == 0)
    {
	FOR_ALL_गाँठ_SIBLINGS(गाँठ, np)
	{
	    यदि (np->wn_child != रिक्त)
		cnt += deref_wordnode(spin, np->wn_child);
	    free_wordnode(spin, np);
	    ++cnt;
	}
	++cnt;	    // length field
    }
    प्रत्यागम cnt;
}

/*
 * Free a wordnode_T for re-use later.
 * Only the "wn_child" field becomes invalid.
 */
    स्थिर शून्य
free_wordnode(spellinfo_T *spin, wordnode_T *n)
{
    n->wn_child = spin->si_first_free;
    spin->si_first_free = n;
    ++spin->si_free_गिनती;
}

/*
 * Compress a tree: find tails that are identical and can be shared.
 */
    स्थिर शून्य
wordtree_compress(spellinfo_T *spin, wordnode_T *मूल, वर्ण *name)
{
    hashtab_T	    ht;
    दीर्घ	    n;
    दीर्घ	    tot = 0;
    दीर्घ	    perc;

    // Skip the root itself, it's not actually used.  The first sibling is the
    // start of the tree.
    यदि (मूल->wn_sibling == रिक्त)
	प्रत्यागम;

    hash_init(&ht);
    n = गाँठ_compress(spin, मूल->wn_sibling, &ht, &tot);

#ifndef SPELL_PRINTTREE
    यदि (spin->si_verbose || p_verbose > 2)
#endif
    {
	यदि (tot > 1000000)
	    perc = (tot - n) / (tot / 100);
	अन्यथा यदि (tot == 0)
	    perc = 0;
	अन्यथा
	    perc = (tot - n) * 100 / tot;
	vim_snprintf((वर्ण *)IObuff, IOSIZE,
		_("Compressed %s: %ld of %ld nodes; %ld (%ld%%) remaining"),
		name, n, tot, tot - n, perc);
	spell_message(spin, IObuff);
    }
#ifdef SPELL_PRINTTREE
    spell_print_वृक्ष(मूल->wn_sibling);
#endif
    hash_clear(&ht);
}

/*
 * Compress a node, its siblings and its children, depth first.
 * Returns the number of compressed nodes.
 */
    स्थिर दीर्घ
गाँठ_compress(
    spellinfo_T	*spin,
    wordnode_T	*गाँठ,
    hashtab_T	*ht,
    दीर्घ	*tot)	    // total count of nodes before compressing,
			    // incremented while going through the tree
{
    wordnode_T	*np;
    wordnode_T	*tp;
    wordnode_T	*child;
    hash_T	hash;
    hashitem_T	*hi;
    दीर्घ	len = 0;
    अचिन्हित	nr, n;
    दीर्घ	compressed = 0;

    /*
     * Go through the list of siblings.  Compress each child and then try
     * finding an identical child to replace it.
     * Note that with "child" we mean not just the node that is pointed to,
     * but the whole list of siblings of which the child node is the first.
     */
    प्रति (np = गाँठ; np != रिक्त && !got_int; np = np->wn_sibling)
    {
	++len;
	यदि ((child = np->wn_child) != रिक्त)
	{
	    // Compress the child first.  This fills hashkey.
	    compressed += गाँठ_compress(spin, child, ht, tot);

	    // Try to find an identical child.
	    hash = hash_hash(child->wn_u1.hashkey);
	    hi = hash_lookup(ht, child->wn_u1.hashkey, hash);
	    यदि (!HASHITEM_EMPTY(hi))
	    {
		// There are children we encountered before with a hash value
		// identical to the current child.  Now check if there is one
		// that is really identical.
		प्रति (tp = HI2WN(hi); tp != रिक्त; tp = tp->wn_u2.next)
		    यदि (गाँठ_equal(child, tp))
		    {
			// Found one!  Now use that child in place of the
			// current one.  This means the current child and all
			// its siblings is unlinked from the tree.
			++tp->wn_refs;
			compressed += deref_wordnode(spin, child);
			np->wn_child = tp;
			भङ्ग;
		    }
		यदि (tp == रिक्त)
		{
		    // No other child with this hash value equals the child of
		    // the node, add it to the linked list after the first
		    // item.
		    tp = HI2WN(hi);
		    child->wn_u2.next = tp->wn_u2.next;
		    tp->wn_u2.next = child;
		}
	    }
	    अन्यथा
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
    गाँठ->wn_u1.hashkey[0] = len;
    nr = 0;
    FOR_ALL_गाँठ_SIBLINGS(गाँठ, np)
    {
	यदि (np->wn_byte == NUL)
	    // end node: use wn_flags, wn_region and wn_affixID
	    n = np->wn_flags + (np->wn_region << 8) + (np->wn_affixID << 16);
	अन्यथा
	    // byte node: use the byte value and the child pointer
	    n = (अचिन्हित)(np->wn_byte + ((long_u)np->wn_child << 8));
	nr = nr * 101 + n;
    }

    // Avoid NUL bytes, it terminates the hash key.
    n = nr & 0xff;
    गाँठ->wn_u1.hashkey[1] = n == 0 ? 1 : n;
    n = (nr >> 8) & 0xff;
    गाँठ->wn_u1.hashkey[2] = n == 0 ? 1 : n;
    n = (nr >> 16) & 0xff;
    गाँठ->wn_u1.hashkey[3] = n == 0 ? 1 : n;
    n = (nr >> 24) & 0xff;
    गाँठ->wn_u1.hashkey[4] = n == 0 ? 1 : n;
    गाँठ->wn_u1.hashkey[5] = NUL;

    // Check for CTRL-C pressed now and then.
    veryfast_breakcheck();

    प्रत्यागम compressed;
}

/*
 * Return TRUE when two nodes have identical siblings and children.
 */
    स्थिर पूर्णाङ्क
गाँठ_equal(wordnode_T *n1, wordnode_T *n2)
{
    wordnode_T	*p1;
    wordnode_T	*p2;

    प्रति (p1 = n1, p2 = n2; p1 != रिक्त && p2 != रिक्त;
				     p1 = p1->wn_sibling, p2 = p2->wn_sibling)
	यदि (p1->wn_byte != p2->wn_byte
		|| (p1->wn_byte == NUL
		    ? (p1->wn_flags != p2->wn_flags
			|| p1->wn_region != p2->wn_region
			|| p1->wn_affixID != p2->wn_affixID)
		    : (p1->wn_child != p2->wn_child)))
	    भङ्ग;

    प्रत्यागम p1 == रिक्त && p2 == रिक्त;
}

स्थिर पूर्णाङ्क rep_compare(ध्रुव शून्य *s1, ध्रुव शून्य *s2);

/*
 * Function given to qsort() to sort the REP items on "from" string.
 */
    स्थिर पूर्णाङ्क
rep_compare(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    fromto_T	*p1 = (fromto_T *)s1;
    fromto_T	*p2 = (fromto_T *)s2;

    प्रत्यागम STRCMP(p1->ft_from, p2->ft_from);
}

/*
 * Write the Vim .spl file "fname".
 * Return FAIL or OK;
 */
    स्थिर पूर्णाङ्क
write_vim_spell(spellinfo_T *spin, char_u *fname)
{
    खाता	*fd;
    पूर्णाङ्क		regionmask;
    पूर्णाङ्क		round;
    wordnode_T	*वृक्ष;
    पूर्णाङ्क		nodecount;
    पूर्णाङ्क		i;
    पूर्णाङ्क		l;
    garray_T	*gap;
    fromto_T	*ftp;
    char_u	*p;
    पूर्णाङ्क		rr;
    पूर्णाङ्क		retval = OK;
    माप_प्रकार	fwv = 1;  // collect return value of fwrite() to avoid
			  // warnings from picky compiler

    fd = mch_fopen((वर्ण *)fname, "w");
    यदि (fd == रिक्त)
    {
	semsg(_(e_cant_open_file_str), fname);
	प्रत्यागम FAIL;
    }

    // <HEADER>: <fileID> <versionnr>
							    // <fileID>
    fwv &= ख_डालो(VIMSPELLMAGIC, VIMSPELLMAGICL, (माप_प्रकार)1, fd);
    यदि (fwv != (माप_प्रकार)1)
	// Catch first write error, don't try writing more.
	गच्छ theend;

    अ_दो(VIMSPELLVERSION, fd);				    // <versionnr>

    /*
     * <SECTIONS>: <section> ... <sectionend>
     */

    // SN_INFO: <infotext>
    यदि (spin->si_info != रिक्त)
    {
	अ_दो(SN_INFO, fd);				// <sectionID>
	अ_दो(0, fd);					// <sectionflags>

	i = (पूर्णाङ्क)STRLEN(spin->si_info);
	put_bytes(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= ख_डालो(spin->si_info, (माप_प्रकार)i, (माप_प्रकार)1, fd); // <infotext>
    }

    // SN_REGION: <regionname> ...
    // Write the region names only if there is more than one.
    यदि (spin->si_region_गिनती > 1)
    {
	अ_दो(SN_REGION, fd);				// <sectionID>
	अ_दो(SNF_REQUIRED, fd);				// <sectionflags>
	l = spin->si_region_गिनती * 2;
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= ख_डालो(spin->si_region_name, (माप_प्रकार)l, (माप_प्रकार)1, fd);
							// <regionname> ...
	regionmask = (1 << spin->si_region_गिनती) - 1;
    }
    अन्यथा
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
    यदि (!spin->si_ascii && !spin->si_add)
    {
	char_u	folchars[128 * 8];
	पूर्णाङ्क	flags;

	अ_दो(SN_CHARFLAGS, fd);				// <sectionID>
	अ_दो(SNF_REQUIRED, fd);				// <sectionflags>

	// Form the <folchars> string first, we need to know its length.
	l = 0;
	प्रति (i = 128; i < 256; ++i)
	{
	    यदि (has_mbyte)
		l += mb_char2bytes(spelltab.st_fold[i], folchars + l);
	    अन्यथा
		folchars[l++] = spelltab.st_fold[i];
	}
	put_bytes(fd, (long_u)(1 + 128 + 2 + l), 4);	// <sectionlen>

	ख_अक्षर_दो(128, fd);					// <charflagslen>
	प्रति (i = 128; i < 256; ++i)
	{
	    flags = 0;
	    यदि (spelltab.st_isw[i])
		flags |= CF_WORD;
	    यदि (spelltab.st_isu[i])
		flags |= CF_UPPER;
	    ख_अक्षर_दो(flags, fd);				// <charflags>
	}

	put_bytes(fd, (long_u)l, 2);			// <folcharslen>
	fwv &= ख_डालो(folchars, (माप_प्रकार)l, (माप_प्रकार)1, fd); // <folchars>
    }

    // SN_MIDWORD: <midword>
    यदि (spin->si_midword != रिक्त)
    {
	अ_दो(SN_MIDWORD, fd);				// <sectionID>
	अ_दो(SNF_REQUIRED, fd);				// <sectionflags>

	i = (पूर्णाङ्क)STRLEN(spin->si_midword);
	put_bytes(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= ख_डालो(spin->si_midword, (माप_प्रकार)i, (माप_प्रकार)1, fd);
							// <midword>
    }

    // SN_PREFCOND: <prefcondcnt> <prefcond> ...
    यदि (spin->si_prefcond.ga_len > 0)
    {
	अ_दो(SN_PREFCOND, fd);				// <sectionID>
	अ_दो(SNF_REQUIRED, fd);				// <sectionflags>

	l = write_spell_prefcond(रिक्त, &spin->si_prefcond, &fwv);
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>

	write_spell_prefcond(fd, &spin->si_prefcond, &fwv);
    }

    // SN_REP: <repcount> <rep> ...
    // SN_SAL: <salflags> <salcount> <sal> ...
    // SN_REPSAL: <repcount> <rep> ...

    // round 1: SN_REP section
    // round 2: SN_SAL section (unless SN_SOFO is used)
    // round 3: SN_REPSAL section
    प्रति (round = 1; round <= 3; ++round)
    {
	यदि (round == 1)
	    gap = &spin->si_rep;
	अन्यथा यदि (round == 2)
	{
	    // Don't write SN_SAL when using a SN_SOFO section
	    यदि (spin->si_sofofr != रिक्त && spin->si_sofoto != रिक्त)
		अनुवर्त;
	    gap = &spin->si_sal;
	}
	अन्यथा
	    gap = &spin->si_repsal;

	// Don't write the section if there are no items.
	यदि (gap->ga_len == 0)
	    अनुवर्त;

	// Sort the REP/REPSAL items.
	यदि (round != 2)
	    क्विक(gap->ga_data, (माप_प्रकार)gap->ga_len,
					       परिमाण(fromto_T), rep_compare);

	i = round == 1 ? SN_REP : (round == 2 ? SN_SAL : SN_REPSAL);
	अ_दो(i, fd);					// <sectionID>

	// This is for making suggestions, section is not required.
	अ_दो(0, fd);					// <sectionflags>

	// Compute the length of what follows.
	l = 2;	    // count <repcount> or <salcount>
	प्रति (i = 0; i < gap->ga_len; ++i)
	{
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    l += 1 + (पूर्णाङ्क)STRLEN(ftp->ft_from);  // count <*fromlen> and <*from>
	    l += 1 + (पूर्णाङ्क)STRLEN(ftp->ft_to);    // count <*tolen> and <*to>
	}
	यदि (round == 2)
	    ++l;	// count <salflags>
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>

	यदि (round == 2)
	{
	    i = 0;
	    यदि (spin->si_followup)
		i |= SAL_F0LLOWUP;
	    यदि (spin->si_collapse)
		i |= SAL_COLLAPSE;
	    यदि (spin->si_rem_accents)
		i |= SAL_REM_ACCENTS;
	    अ_दो(i, fd);			// <salflags>
	}

	put_bytes(fd, (long_u)gap->ga_len, 2);	// <repcount> or <salcount>
	प्रति (i = 0; i < gap->ga_len; ++i)
	{
	    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
	    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    प्रति (rr = 1; rr <= 2; ++rr)
	    {
		p = rr == 1 ? ftp->ft_from : ftp->ft_to;
		l = (पूर्णाङ्क)STRLEN(p);
		अ_दो(l, fd);
		यदि (l > 0)
		    fwv &= ख_डालो(p, l, (माप_प्रकार)1, fd);
	    }
	}

    }

    // SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
    // This is for making suggestions, section is not required.
    यदि (spin->si_sofofr != रिक्त && spin->si_sofoto != रिक्त)
    {
	अ_दो(SN_SOFO, fd);				// <sectionID>
	अ_दो(0, fd);					// <sectionflags>

	l = (पूर्णाङ्क)STRLEN(spin->si_sofofr);
	put_bytes(fd, (long_u)(l + STRLEN(spin->si_sofoto) + 4), 4);
							// <sectionlen>

	put_bytes(fd, (long_u)l, 2);			// <sofofromlen>
	fwv &= ख_डालो(spin->si_sofofr, l, (माप_प्रकार)1, fd); // <sofofrom>

	l = (पूर्णाङ्क)STRLEN(spin->si_sofoto);
	put_bytes(fd, (long_u)l, 2);			// <sofotolen>
	fwv &= ख_डालो(spin->si_sofoto, l, (माप_प्रकार)1, fd); // <sofoto>
    }

    // SN_WORDS: <word> ...
    // This is for making suggestions, section is not required.
    यदि (spin->si_commonwords.ht_used > 0)
    {
	अ_दो(SN_शब्दगण, fd);				// <sectionID>
	अ_दो(0, fd);					// <sectionflags>

	// round 1: count the bytes
	// round 2: write the bytes
	प्रति (round = 1; round <= 2; ++round)
	{
	    पूर्णाङ्क		todo;
	    पूर्णाङ्क		len = 0;
	    hashitem_T	*hi;

	    todo = (पूर्णाङ्क)spin->si_commonwords.ht_used;
	    FOR_ALL_HASHTAB_वस्तुएँ(&spin->si_commonwords, hi, todo)
		यदि (!HASHITEM_EMPTY(hi))
		{
		    l = (पूर्णाङ्क)STRLEN(hi->hi_कुंजी) + 1;
		    len += l;
		    यदि (round == 2)			// <word>
			fwv &= ख_डालो(hi->hi_कुंजी, (माप_प्रकार)l, (माप_प्रकार)1, fd);
		    --todo;
		}
	    यदि (round == 1)
		put_bytes(fd, (long_u)len, 4);		// <sectionlen>
	}
    }

    // SN_MAP: <mapstr>
    // This is for making suggestions, section is not required.
    यदि (spin->si_map.ga_len > 0)
    {
	अ_दो(SN_MAP, fd);				// <sectionID>
	अ_दो(0, fd);					// <sectionflags>
	l = spin->si_map.ga_len;
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= ख_डालो(spin->si_map.ga_data, (माप_प्रकार)l, (माप_प्रकार)1, fd);
							// <mapstr>
    }

    // SN_SUGFILE: <timestamp>
    // This is used to notify that a .sug file may be available and at the
    // same time allows for checking that a .sug file that is found matches
    // with this .spl file.  That's because the word numbers must be exactly
    // right.
    यदि (!spin->si_nosugfile
	    && (spin->si_sal.ga_len > 0
		     || (spin->si_sofofr != रिक्त && spin->si_sofoto != रिक्त)))
    {
	अ_दो(SN_SUGFILE, fd);				// <sectionID>
	अ_दो(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)8, 4);			// <sectionlen>

	// Set si_sugtime and write it to the file.
	spin->si_sugtime = समय(रिक्त);
	put_time(fd, spin->si_sugtime);			// <timestamp>
    }

    // SN_NOSPLITSUGS: nothing
    // This is used to notify that no suggestions with word splits are to be
    // made.
    यदि (spin->si_nosplitsugs)
    {
	अ_दो(SN_NOSPLITSUGS, fd);			// <sectionID>
	अ_दो(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_NOCOMPUNDSUGS: nothing
    // This is used to notify that no suggestions with compounds are to be
    // made.
    यदि (spin->si_nocompoundsugs)
    {
	अ_दो(SN_NOCOMPOUNDSUGS, fd);			// <sectionID>
	अ_दो(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_COMPOUND: compound info.
    // We don't mark it required, when not supported all compound words will
    // be bad words.
    यदि (spin->si_compflags != रिक्त)
    {
	अ_दो(SN_COMPOUND, fd);				// <sectionID>
	अ_दो(0, fd);					// <sectionflags>

	l = (पूर्णाङ्क)STRLEN(spin->si_compflags);
	प्रति (i = 0; i < spin->si_comppat.ga_len; ++i)
	    l += (पूर्णाङ्क)STRLEN(((char_u **)(spin->si_comppat.ga_data))[i]) + 1;
	put_bytes(fd, (long_u)(l + 7), 4);		// <sectionlen>

	अ_दो(spin->si_compmax, fd);			// <compmax>
	अ_दो(spin->si_compminlen, fd);			// <compminlen>
	अ_दो(spin->si_compsylmax, fd);			// <compsylmax>
	अ_दो(0, fd);		// for Vim 7.0b compatibility
	अ_दो(spin->si_compoptions, fd);			// <compoptions>
	put_bytes(fd, (long_u)spin->si_comppat.ga_len, 2);
							// <comppatcount>
	प्रति (i = 0; i < spin->si_comppat.ga_len; ++i)
	{
	    p = ((char_u **)(spin->si_comppat.ga_data))[i];
	    अ_दो((पूर्णाङ्क)STRLEN(p), fd);			// <comppatlen>
	    fwv &= ख_डालो(p, (माप_प्रकार)STRLEN(p), (माप_प्रकार)1, fd);
							// <comppattext>
	}
							// <compflags>
	fwv &= ख_डालो(spin->si_compflags, (माप_प्रकार)STRLEN(spin->si_compflags),
							       (माप_प्रकार)1, fd);
    }

    // SN_NOBREAK: NOBREAK flag
    यदि (spin->si_nobreak)
    {
	अ_दो(SN_NOBREAK, fd);				// <sectionID>
	अ_दो(0, fd);					// <sectionflags>

	// It's empty, the presence of the section flags the feature.
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_SYLLABLE: syllable info.
    // We don't mark it required, when not supported syllables will not be
    // counted.
    यदि (spin->si_syllable != रिक्त)
    {
	अ_दो(SN_SYLLABLE, fd);				// <sectionID>
	अ_दो(0, fd);					// <sectionflags>

	l = (पूर्णाङ्क)STRLEN(spin->si_syllable);
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= ख_डालो(spin->si_syllable, (माप_प्रकार)l, (माप_प्रकार)1, fd);
							// <syllable>
    }

    // end of <SECTIONS>
    अ_दो(SN_END, fd);					// <sectionend>


    /*
     * <LWORDTREE>  <KWORDTREE>  <PREFIXTREE>
     */
    spin->si_memtot = 0;
    प्रति (round = 1; round <= 3; ++round)
    {
	यदि (round == 1)
	    वृक्ष = spin->si_foldroot->wn_sibling;
	अन्यथा यदि (round == 2)
	    वृक्ष = spin->si_keeproot->wn_sibling;
	अन्यथा
	    वृक्ष = spin->si_prefroot->wn_sibling;

	// Clear the index and wnode fields in the tree.
	clear_गाँठ(वृक्ष);

	// Count the number of nodes.  Needed to be able to allocate the
	// memory when reading the nodes.  Also fills in index for shared
	// nodes.
	nodecount = put_गाँठ(रिक्त, वृक्ष, 0, regionmask, round == 3);

	// number of nodes in 4 bytes
	put_bytes(fd, (long_u)nodecount, 4);	// <nodecount>
	spin->si_memtot += nodecount + nodecount * परिमाण(पूर्णाङ्क);

	// Write the nodes.
	(शून्य)put_गाँठ(fd, वृक्ष, 0, regionmask, round == 3);
    }

    // Write another byte to check for errors (file system full).
    यदि (अ_दो(0, fd) == खातापूर्ण)
	retval = FAIL;
theend:
    यदि (ख_बंद(fd) == खातापूर्ण)
	retval = FAIL;

    यदि (fwv != (माप_प्रकार)1)
	retval = FAIL;
    यदि (retval == FAIL)
	emsg(_(e_error_while_writing));

    प्रत्यागम retval;
}

/*
 * Clear the index and wnode fields of "node", it siblings and its
 * children.  This is needed because they are a union with other items to save
 * space.
 */
    स्थिर शून्य
clear_गाँठ(wordnode_T *गाँठ)
{
    wordnode_T	*np;

    यदि (गाँठ != रिक्त)
	FOR_ALL_गाँठ_SIBLINGS(गाँठ, np)
	{
	    np->wn_u1.index = 0;
	    np->wn_u2.wnode = रिक्त;

	    यदि (np->wn_byte != NUL)
		clear_गाँठ(np->wn_child);
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
    स्थिर पूर्णाङ्क
put_गाँठ(
    खाता	*fd,		// NULL when only counting
    wordnode_T	*गाँठ,
    पूर्णाङ्क		idx,
    पूर्णाङ्क		regionmask,
    पूर्णाङ्क		prefixtree)	// TRUE for PREFIXTREE
{
    पूर्णाङ्क		newindex = idx;
    पूर्णाङ्क		siblingcount = 0;
    wordnode_T	*np;
    पूर्णाङ्क		flags;

    // If "node" is zero the tree is empty.
    यदि (गाँठ == रिक्त)
	प्रत्यागम 0;

    // Store the index where this node is written.
    गाँठ->wn_u1.index = idx;

    // Count the number of siblings.
    FOR_ALL_गाँठ_SIBLINGS(गाँठ, np)
	++siblingcount;

    // Write the sibling count.
    यदि (fd != रिक्त)
	अ_दो(siblingcount, fd);				// <siblingcount>

    // Write each sibling byte and optionally extra info.
    FOR_ALL_गाँठ_SIBLINGS(गाँठ, np)
    {
	यदि (np->wn_byte == 0)
	{
	    यदि (fd != रिक्त)
	    {
		// For a NUL byte (end of word) write the flags etc.
		यदि (prefixtree)
		{
		    // In PREFIXTREE write the required affixID and the
		    // associated condition nr (stored in wn_region).  The
		    // byte value is misused to store the "rare" and "not
		    // combining" flags
		    यदि (np->wn_flags == (short_u)PFX_FLAGS)
			अ_दो(BY_NOFLAGS, fd);		// <byte>
		    अन्यथा
		    {
			अ_दो(BY_FLAGS, fd);		// <byte>
			अ_दो(np->wn_flags, fd);		// <pflags>
		    }
		    अ_दो(np->wn_affixID, fd);		// <affixID>
		    put_bytes(fd, (long_u)np->wn_region, 2); // <prefcondnr>
		}
		अन्यथा
		{
		    // For word trees we write the flag/region items.
		    flags = np->wn_flags;
		    यदि (regionmask != 0 && np->wn_region != regionmask)
			flags |= WF_REGION;
		    यदि (np->wn_affixID != 0)
			flags |= WF_AFX;
		    यदि (flags == 0)
		    {
			// word without flags or region
			अ_दो(BY_NOFLAGS, fd);			// <byte>
		    }
		    अन्यथा
		    {
			यदि (np->wn_flags >= 0x100)
			{
			    अ_दो(BY_FLAGS2, fd);		// <byte>
			    अ_दो(flags, fd);			// <flags>
			    अ_दो((अचिन्हित)flags >> 8, fd);	// <flags2>
			}
			अन्यथा
			{
			    अ_दो(BY_FLAGS, fd);			// <byte>
			    अ_दो(flags, fd);			// <flags>
			}
			यदि (flags & WF_REGION)
			    अ_दो(np->wn_region, fd);		// <region>
			यदि (flags & WF_AFX)
			    अ_दो(np->wn_affixID, fd);		// <affixID>
		    }
		}
	    }
	}
	अन्यथा
	{
	    यदि (np->wn_child->wn_u1.index != 0
					 && np->wn_child->wn_u2.wnode != गाँठ)
	    {
		// The child is written elsewhere, write the reference.
		यदि (fd != रिक्त)
		{
		    अ_दो(BY_INDEX, fd);			// <byte>
							// <nodeidx>
		    put_bytes(fd, (long_u)np->wn_child->wn_u1.index, 3);
		}
	    }
	    अन्यथा यदि (np->wn_child->wn_u2.wnode == रिक्त)
		// We will write the child below and give it an index.
		np->wn_child->wn_u2.wnode = गाँठ;

	    यदि (fd != रिक्त)
		यदि (अ_दो(np->wn_byte, fd) == खातापूर्ण) // <byte> or <xbyte>
		{
		    emsg(_(e_error_while_writing));
		    प्रत्यागम 0;
		}
	}
    }

    // Space used in the array when reading: one for each sibling and one for
    // the count.
    newindex += siblingcount + 1;

    // Recursively dump the children of each sibling.
    FOR_ALL_गाँठ_SIBLINGS(गाँठ, np)
	यदि (np->wn_byte != 0 && np->wn_child->wn_u2.wnode == गाँठ)
	    newindex = put_गाँठ(fd, np->wn_child, newindex, regionmask,
								  prefixtree);

    प्रत्यागम newindex;
}


/*
 * ":mkspell [-ascii] outfile  infile ..."
 * ":mkspell [-ascii] addfile"
 */
    शून्य
ex_mkspell(exarg_T *eap)
{
    पूर्णाङ्क		fcount;
    char_u	**fnames;
    char_u	*arg = eap->arg;
    पूर्णाङ्क		ascii = FALSE;

    यदि (STRNCMP(arg, "-ascii", 6) == 0)
    {
	ascii = TRUE;
	arg = skipwhite(arg + 6);
    }

    // Expand all the remaining arguments (e.g., $VIMRUNTIME).
    यदि (get_arglist_exp(arg, &fcount, &fnames, FALSE) != OK)
	प्रत्यागम;

    mkspell(fcount, fnames, ascii, eap->forceit, FALSE);
    FreeWild(fcount, fnames);
}

/*
 * Create the .sug file.
 * Uses the soundfold info in "spin".
 * Writes the file with the name "wfname", with ".spl" changed to ".sug".
 */
    स्थिर शून्य
spell_make_sugfile(spellinfo_T *spin, char_u *wfname)
{
    char_u	*fname = रिक्त;
    पूर्णाङ्क		len;
    slang_T	*slang;
    पूर्णाङ्क		free_slang = FALSE;

    /*
     * Read back the .spl file that was written.  This fills the required
     * info for soundfolding.  This also uses less memory than the
     * pointer-linked version of the trie.  And it avoids having two versions
     * of the code for the soundfolding stuff.
     * It might have been done already by spell_reload_one().
     */
    FOR_ALL_SPELL_LANGS(slang)
	यदि (fullpathcmp(wfname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	    भङ्ग;
    यदि (slang == रिक्त)
    {
	spell_message(spin, (char_u *)_("Reading back spell file..."));
	slang = spell_load_file(wfname, रिक्त, रिक्त, FALSE);
	यदि (slang == रिक्त)
	    प्रत्यागम;
	free_slang = TRUE;
    }

    /*
     * Clear the info in "spin" that is used.
     */
    spin->si_blocks = रिक्त;
    spin->si_blocks_cnt = 0;
    spin->si_compress_cnt = 0;	    // will stay at 0 all the time
    spin->si_free_गिनती = 0;
    spin->si_first_free = रिक्त;
    spin->si_foldwcount = 0;

    /*
     * Go through the trie of good words, soundfold each word and add it to
     * the soundfold trie.
     */
    spell_message(spin, (char_u *)_("Performing soundfolding..."));
    यदि (sug_filltree(spin, slang) == FAIL)
	गच्छ theend;

    /*
     * Create the table which links each soundfold word with a list of the
     * good words it may come from.  Creates buffer "spin->si_spellbuf".
     * This also removes the wordnr from the NUL byte entries to make
     * compression possible.
     */
    यदि (sug_maketable(spin) == FAIL)
	गच्छ theend;

    smsg(_("Number of words after soundfolding: %ld"),
				 (दीर्घ)spin->si_spellbuf->b_ml.ml_पंक्ति_गिनती);

    /*
     * Compress the soundfold trie.
     */
    spell_message(spin, (char_u *)_(संदेश_compressing));
    wordtree_compress(spin, spin->si_foldroot, "case-folded");

    /*
     * Write the .sug file.
     * Make the file name by changing ".spl" to ".sug".
     */
    fname = alloc(MAXPATHL);
    यदि (fname == रिक्त)
	गच्छ theend;
    vim_strncpy(fname, wfname, MAXPATHL - 1);
    len = (पूर्णाङ्क)STRLEN(fname);
    fname[len - 2] = 'u';
    fname[len - 1] = 'g';
    sug_write(spin, fname);

theend:
    vim_free(fname);
    यदि (free_slang)
	slang_free(slang);
    free_blocks(spin->si_blocks);
    close_spellbuf(spin->si_spellbuf);
}

/*
 * Build the soundfold trie for language "slang".
 */
    स्थिर पूर्णाङ्क
sug_filltree(spellinfo_T *spin, slang_T *slang)
{
    char_u	*byts;
    idx_T	*idxs;
    पूर्णाङ्क		depth;
    idx_T	arridx[MAXWLEN];
    पूर्णाङ्क		curi[MAXWLEN];
    char_u	tword[MAXWLEN];
    char_u	tsalword[MAXWLEN];
    पूर्णाङ्क		c;
    idx_T	n;
    अचिन्हित	शब्दगण_done = 0;
    पूर्णाङ्क		wordcount[MAXWLEN];

    // We use si_foldroot for the soundfolded trie.
    spin->si_foldroot = wordtree_alloc(spin);
    यदि (spin->si_foldroot == रिक्त)
	प्रत्यागम FAIL;

    // let tree_add_word() know we're adding to the soundfolded tree
    spin->si_sugtree = TRUE;

    /*
     * Go through the whole case-folded tree, soundfold each word and put it
     * in the trie.  Bail out if the tree is empty.
     */
    byts = slang->sl_fbyts;
    idxs = slang->sl_fidxs;
    यदि (byts == रिक्त || idxs == रिक्त)
	प्रत्यागम FAIL;

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;

    depth = 0;
    यावत् (depth >= 0 && !got_int)
    {
	यदि (curi[depth] > byts[arridx[depth]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[depth]] = wordcount[depth];
	    यदि (depth > 0)
		wordcount[depth - 1] += wordcount[depth];

	    --depth;
	    पंक्ति_breakcheck();
	}
	अन्यथा
	{

	    // Do one more byte at this node.
	    n = arridx[depth] + curi[depth];
	    ++curi[depth];

	    c = byts[n];
	    यदि (c == 0)
	    {
		// Sound-fold the word.
		tword[depth] = NUL;
		spell_soundfold(slang, tword, TRUE, tsalword);

		// We use the "flags" field for the MSB of the wordnr,
		// "region" for the LSB of the wordnr.
		यदि (वृक्ष_add_word(spin, tsalword, spin->si_foldroot,
				शब्दगण_done >> 16, शब्दगण_done & 0xffff,
							   0) == FAIL)
		    प्रत्यागम FAIL;

		++शब्दगण_done;
		++wordcount[depth];

		// Reset the block count each time to avoid compression
		// kicking in.
		spin->si_blocks_cnt = 0;

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		यावत् (n + 1 < slang->sl_fbyts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[depth];
		}
	    }
	    अन्यथा यदि (depth < MAXWLEN - 1)
	    {
		// Normal char, go one level deeper.
		tword[depth++] = c;
		arridx[depth] = idxs[n];
		curi[depth] = 1;
		wordcount[depth] = 0;
	    }
	}
    }

    smsg(_("Total number of words: %d"), शब्दगण_done);

    प्रत्यागम OK;
}

/*
 * Make the table that links each word in the soundfold trie to the words it
 * can be produced from.
 * This is not unlike lines in a file, thus use a memfile to be able to access
 * the table efficiently.
 * Returns FAIL when out of memory.
 */
    स्थिर पूर्णाङ्क
sug_maketable(spellinfo_T *spin)
{
    garray_T	ga;
    पूर्णाङ्क		res = OK;

    // Allocate a buffer, open a memline for it and create the swap file
    // (uses a temp file, not a .swp file).
    spin->si_spellbuf = open_spellbuf();
    यदि (spin->si_spellbuf == रिक्त)
	प्रत्यागम FAIL;

    // Use a buffer to store the line info, avoids allocating many small
    // pieces of memory.
    ga_init2(&ga, 1, 100);

    // recursively go through the tree
    यदि (sug_filltable(spin, spin->si_foldroot->wn_sibling, 0, &ga) == -1)
	res = FAIL;

    ga_clear(&ga);
    प्रत्यागम res;
}

/*
 * Fill the table for one node and its children.
 * Returns the wordnr at the start of the node.
 * Returns -1 when out of memory.
 */
    स्थिर पूर्णाङ्क
sug_filltable(
    spellinfo_T	*spin,
    wordnode_T	*गाँठ,
    पूर्णाङ्क		startwordnr,
    garray_T	*gap)	    // place to store line of numbers
{
    wordnode_T	*p, *np;
    पूर्णाङ्क		wordnr = startwordnr;
    पूर्णाङ्क		nr;
    पूर्णाङ्क		prev_nr;

    FOR_ALL_गाँठ_SIBLINGS(गाँठ, p)
    {
	यदि (p->wn_byte == NUL)
	{
	    gap->ga_len = 0;
	    prev_nr = 0;
	    प्रति (np = p; np != रिक्त && np->wn_byte == NUL; np = np->wn_sibling)
	    {
		यदि (ga_grow(gap, 10) == FAIL)
		    प्रत्यागम -1;

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

	    यदि (ml_append_buf(spin->si_spellbuf, (linenr_T)wordnr,
				     gap->ga_data, gap->ga_len, TRUE) == FAIL)
		प्रत्यागम -1;
	    ++wordnr;

	    // Remove extra NUL entries, we no longer need them. We don't
	    // bother freeing the nodes, they won't be reused anyway.
	    यावत् (p->wn_sibling != रिक्त && p->wn_sibling->wn_byte == NUL)
		p->wn_sibling = p->wn_sibling->wn_sibling;

	    // Clear the flags on the remaining NUL node, so that compression
	    // works a lot better.
	    p->wn_flags = 0;
	    p->wn_region = 0;
	}
	अन्यथा
	{
	    wordnr = sug_filltable(spin, p->wn_child, wordnr, gap);
	    यदि (wordnr == -1)
		प्रत्यागम -1;
	}
    }
    प्रत्यागम wordnr;
}

/*
 * Convert an offset into a minimal number of bytes.
 * Similar to utf_char2byters, but use 8 bits in followup bytes and avoid NUL
 * bytes.
 */
    स्थिर पूर्णाङ्क
offset2bytes(पूर्णाङ्क nr, char_u *buf)
{
    पूर्णाङ्क	    rem;
    पूर्णाङ्क	    b1, b2, b3, b4;

    // Split the number in parts of base 255.  We need to avoid NUL bytes.
    b1 = nr % 255 + 1;
    rem = nr / 255;
    b2 = rem % 255 + 1;
    rem = rem / 255;
    b3 = rem % 255 + 1;
    b4 = rem / 255 + 1;

    यदि (b4 > 1 || b3 > 0x1f)	// 4 bytes
    {
	buf[0] = 0xe0 + b4;
	buf[1] = b3;
	buf[2] = b2;
	buf[3] = b1;
	प्रत्यागम 4;
    }
    यदि (b3 > 1 || b2 > 0x3f )	// 3 bytes
    {
	buf[0] = 0xc0 + b3;
	buf[1] = b2;
	buf[2] = b1;
	प्रत्यागम 3;
    }
    यदि (b2 > 1 || b1 > 0x7f )	// 2 bytes
    {
	buf[0] = 0x80 + b2;
	buf[1] = b1;
	प्रत्यागम 2;
    }
				// 1 byte
    buf[0] = b1;
    प्रत्यागम 1;
}

/*
 * Write the .sug file in "fname".
 */
    स्थिर शून्य
sug_write(spellinfo_T *spin, char_u *fname)
{
    खाता	*fd;
    wordnode_T	*वृक्ष;
    पूर्णाङ्क		nodecount;
    पूर्णाङ्क		wcount;
    char_u	*पंक्ति;
    linenr_T	lnum;
    पूर्णाङ्क		len;

    // Create the file.  Note that an existing file is silently overwritten!
    fd = mch_fopen((वर्ण *)fname, "w");
    यदि (fd == रिक्त)
    {
	semsg(_(e_cant_open_file_str), fname);
	प्रत्यागम;
    }

    vim_snprintf((वर्ण *)IObuff, IOSIZE,
				  _("Writing suggestion file %s..."), fname);
    spell_message(spin, IObuff);

    /*
     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
     */
    यदि (ख_डालो(VIMSUGMAGIC, VIMSUGMAGICL, (माप_प्रकार)1, fd) != 1) // <fileID>
    {
	emsg(_(e_error_while_writing));
	गच्छ theend;
    }
    अ_दो(VIMSUGVERSION, fd);				// <versionnr>

    // Write si_sugtime to the file.
    put_time(fd, spin->si_sugtime);			// <timestamp>

    /*
     * <SUGWORDTREE>
     */
    spin->si_memtot = 0;
    वृक्ष = spin->si_foldroot->wn_sibling;

    // Clear the index and wnode fields in the tree.
    clear_गाँठ(वृक्ष);

    // Count the number of nodes.  Needed to be able to allocate the
    // memory when reading the nodes.  Also fills in index for shared
    // nodes.
    nodecount = put_गाँठ(रिक्त, वृक्ष, 0, 0, FALSE);

    // number of nodes in 4 bytes
    put_bytes(fd, (long_u)nodecount, 4);	// <nodecount>
    spin->si_memtot += nodecount + nodecount * परिमाण(पूर्णाङ्क);

    // Write the nodes.
    (शून्य)put_गाँठ(fd, वृक्ष, 0, 0, FALSE);

    /*
     * <SUGTABLE>: <sugwcount> <sugline> ...
     */
    wcount = spin->si_spellbuf->b_ml.ml_पंक्ति_गिनती;
    put_bytes(fd, (long_u)wcount, 4);	// <sugwcount>

    प्रति (lnum = 1; lnum <= (linenr_T)wcount; ++lnum)
    {
	// <sugline>: <sugnr> ... NUL
	पंक्ति = ml_get_buf(spin->si_spellbuf, lnum, FALSE);
	len = ml_get_buf_len(spin->si_spellbuf, lnum) + 1;
	यदि (ख_डालो(पंक्ति, (माप_प्रकार)len, (माप_प्रकार)1, fd) == 0)
	{
	    emsg(_(e_error_while_writing));
	    गच्छ theend;
	}
	spin->si_memtot += len;
    }

    // Write another byte to check for errors.
    यदि (अ_दो(0, fd) == खातापूर्ण)
	emsg(_(e_error_while_writing));

    vim_snprintf((वर्ण *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin->si_memtot);
    spell_message(spin, IObuff);

theend:
    // close the file
    ख_बंद(fd);
}


/*
 * Create a Vim spell file from one or more word lists.
 * "fnames[0]" is the output file name.
 * "fnames[fcount - 1]" is the last input file name.
 * Exception: when "fnames[0]" ends in ".add" it's used as the input file name
 * and ".spl" is appended to make the output file name.
 */
    शून्य
mkspell(
    पूर्णाङ्क		fcount,
    char_u	**fnames,
    पूर्णाङ्क		ascii,		    // -ascii argument given
    पूर्णाङ्क		over_write,	    // overwrite existing output file
    पूर्णाङ्क		added_word)	    // invoked through "zg"
{
    char_u	*fname = रिक्त;
    char_u	*wfname;
    char_u	**innames;
    पूर्णाङ्क		incount;
    afffile_T	*(afile[MAXREGIONS]);
    पूर्णाङ्क		i;
    पूर्णाङ्क		len;
    stat_T	st;
    पूर्णाङ्क		error = FALSE;
    spellinfo_T spin;

    CLEAR_FIELD(spin);
    spin.si_verbose = !added_word;
    spin.si_ascii = ascii;
    spin.si_followup = TRUE;
    spin.si_rem_accents = TRUE;
    ga_init2(&spin.si_rep, परिमाण(fromto_T), 20);
    ga_init2(&spin.si_repsal, परिमाण(fromto_T), 20);
    ga_init2(&spin.si_sal, परिमाण(fromto_T), 20);
    ga_init2(&spin.si_map, परिमाण(char_u), 100);
    ga_init2(&spin.si_comppat, परिमाण(char_u *), 20);
    ga_init2(&spin.si_prefcond, परिमाण(char_u *), 50);
    hash_init(&spin.si_commonwords);
    spin.si_newcompID = 127;	// start compound ID at first maximum

    // default: fnames[0] is output file, following are input files
    // When "fcount" is 1 there is only one file.
    innames = &fnames[fcount == 1 ? 0 : 1];
    incount = fcount - 1;

    wfname = alloc(MAXPATHL);
    यदि (wfname == रिक्त)
	प्रत्यागम;

    यदि (fcount >= 1)
    {
	len = (पूर्णाङ्क)STRLEN(fnames[0]);
	यदि (fcount == 1 && len > 4 && STRCMP(fnames[0] + len - 4, ".add") == 0)
	{
	    // For ":mkspell path/en.latin1.add" output file is
	    // "path/en.latin1.add.spl".
	    incount = 1;
	    vim_snprintf((वर्ण *)wfname, MAXPATHL, "%s.spl", fnames[0]);
	}
	अन्यथा यदि (fcount == 1)
	{
	    // For ":mkspell path/vim" output file is "path/vim.latin1.spl".
	    incount = 1;
	    vim_snprintf((वर्ण *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());
	}
	अन्यथा यदि (len > 4 && STRCMP(fnames[0] + len - 4, ".spl") == 0)
	{
	    // Name ends in ".spl", use as the file name.
	    vim_strncpy(wfname, fnames[0], MAXPATHL - 1);
	}
	अन्यथा
	    // Name should be language, make the file name from it.
	    vim_snprintf((वर्ण *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());

	// Check for .ascii.spl.
	यदि (म_माला((वर्ण *)gettail(wfname), SPL_FNAME_ASCII) != रिक्त)
	    spin.si_ascii = TRUE;

	// Check for .add.spl.
	यदि (म_माला((वर्ण *)gettail(wfname), SPL_FNAME_ADD) != रिक्त)
	    spin.si_add = TRUE;
    }

    यदि (incount <= 0)
	emsg(_(e_invalid_argument));	// need at least output and input names
    अन्यथा यदि (vim_strchr(gettail(wfname), '_') != रिक्त)
	emsg(_(e_output_file_name_must_not_have_region_name));
    अन्यथा यदि (incount > MAXREGIONS)
	semsg(_(e_only_up_to_nr_regions_supported), MAXREGIONS);
    अन्यथा
    {
	// Check for overwriting before doing things that may take a lot of
	// time.
	यदि (!over_write && mch_stat((वर्ण *)wfname, &st) >= 0)
	{
	    emsg(_(e_file_exists));
	    गच्छ theend;
	}
	यदि (mch_isdir(wfname))
	{
	    semsg(_(e_str_is_directory), wfname);
	    गच्छ theend;
	}

	fname = alloc(MAXPATHL);
	यदि (fname == रिक्त)
	    गच्छ theend;

	/*
	 * Init the aff and dic pointers.
	 * Get the region names if there are more than 2 arguments.
	 */
	प्रति (i = 0; i < incount; ++i)
	{
	    afile[i] = रिक्त;

	    यदि (incount > 1)
	    {
		len = (पूर्णाङ्क)STRLEN(innames[i]);
		यदि (STRLEN(gettail(innames[i])) < 5
						|| innames[i][len - 3] != '_')
		{
		    semsg(_(e_invalid_region_in_str), innames[i]);
		    गच्छ theend;
		}
		spin.si_region_name[i * 2] = TOLOWER_ASC(innames[i][len - 2]);
		spin.si_region_name[i * 2 + 1] =
					     TOLOWER_ASC(innames[i][len - 1]);
	    }
	}
	spin.si_region_गिनती = incount;

	spin.si_foldroot = wordtree_alloc(&spin);
	spin.si_keeproot = wordtree_alloc(&spin);
	spin.si_prefroot = wordtree_alloc(&spin);
	यदि (spin.si_foldroot == रिक्त
		|| spin.si_keeproot == रिक्त
		|| spin.si_prefroot == रिक्त)
	{
	    free_blocks(spin.si_blocks);
	    गच्छ theend;
	}

	// When not producing a .add.spl file clear the character table when
	// we encounter one in the .aff file.  This means we dump the current
	// one in the .spl file if the .aff file doesn't define one.  That's
	// better than guessing the contents, the table will match a
	// previously loaded spell file.
	यदि (!spin.si_add)
	    spin.si_clear_chartab = TRUE;

	/*
	 * Read all the .aff and .dic files.
	 * Text is converted to 'encoding'.
	 * Words are stored in the case-folded and keep-case trees.
	 */
	प्रति (i = 0; i < incount && !error; ++i)
	{
	    spin.si_conv.vc_type = CONV_NONE;
	    spin.si_region = 1 << i;

	    vim_snprintf((वर्ण *)fname, MAXPATHL, "%s.aff", innames[i]);
	    यदि (mch_stat((वर्ण *)fname, &st) >= 0)
	    {
		// Read the .aff file.  Will init "spin->si_conv" based on the
		// "SET" line.
		afile[i] = spell_read_aff(&spin, fname);
		यदि (afile[i] == रिक्त)
		    error = TRUE;
		अन्यथा
		{
		    // Read the .dic file and store the words in the trees.
		    vim_snprintf((वर्ण *)fname, MAXPATHL, "%s.dic",
								  innames[i]);
		    यदि (spell_read_dic(&spin, fname, afile[i]) == FAIL)
			error = TRUE;
		}
	    }
	    अन्यथा
	    {
		// No .aff file, try reading the file as a word list.  Store
		// the words in the trees.
		यदि (spell_read_wordfile(&spin, innames[i]) == FAIL)
		    error = TRUE;
	    }

	    // Free any conversion stuff.
	    convert_setup(&spin.si_conv, रिक्त, रिक्त);
	}

	यदि (spin.si_compflags != रिक्त && spin.si_nobreak)
	    संदेश(_("Warning: both compounding and NOBREAK specified"));

	यदि (!error && !got_int)
	{
	    /*
	     * Combine tails in the tree.
	     */
	    spell_message(&spin, (char_u *)_(संदेश_compressing));
	    wordtree_compress(&spin, spin.si_foldroot, "case-folded");
	    wordtree_compress(&spin, spin.si_keeproot, "keep-case");
	    wordtree_compress(&spin, spin.si_prefroot, "prefixes");
	}

	यदि (!error && !got_int)
	{
	    /*
	     * Write the info in the spell file.
	     */
	    vim_snprintf((वर्ण *)IObuff, IOSIZE,
				      _("Writing spell file %s..."), wfname);
	    spell_message(&spin, IObuff);

	    error = write_vim_spell(&spin, wfname) == FAIL;

	    spell_message(&spin, (char_u *)_("Done!"));
	    vim_snprintf((वर्ण *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin.si_memtot);
	    spell_message(&spin, IObuff);

	    /*
	     * If the file is loaded need to reload it.
	     */
	    यदि (!error)
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
	प्रति (i = 0; i < incount; ++i)
	    यदि (afile[i] != रिक्त)
		spell_free_aff(afile[i]);

	// Free all the bits and pieces at once.
	free_blocks(spin.si_blocks);

	/*
	 * If there is soundfolding info and no NOSUGFILE item create the
	 * .sug file with the soundfolded word trie.
	 */
	यदि (spin.si_sugtime != 0 && !error && !got_int)
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
    स्थिर शून्य
spell_message(spellinfo_T *spin, char_u *str)
{
    यदि (spin->si_verbose || p_verbose > 2)
    {
	यदि (!spin->si_verbose)
	    verbose_enter();
	संदेश((वर्ण *)str);
	out_flush();
	यदि (!spin->si_verbose)
	    verbose_leave();
    }
}

/*
 * ":[count]spellgood  {word}"
 * ":[count]spellwrong {word}"
 * ":[count]spellundo  {word}"
 * ":[count]spellrare  {word}"
 */
    शून्य
ex_spell(exarg_T *eap)
{
    spell_add_word(eap->arg, (पूर्णाङ्क)STRLEN(eap->arg),
		eap->cmdidx == CMD_spellwrong ? SPELL_ADD_BAD :
		eap->cmdidx == CMD_spellrare ? SPELL_ADD_RARE : SPELL_ADD_GOOD,
				   eap->forceit ? 0 : (पूर्णाङ्क)eap->line2,
				   eap->cmdidx == CMD_spellundo);
}

/*
 * Add "word[len]" to 'spellfile' as a good, rare or bad word.
 */
    शून्य
spell_add_word(
    char_u	*word,
    पूर्णाङ्क		len,
    पूर्णाङ्क		what,	    // SPELL_ADD_ values
    पूर्णाङ्क		idx,	    // "zG" and "zW": zero, otherwise index in
			    // 'spellfile'
    पूर्णाङ्क		undo)	    // TRUE for "zug", "zuG", "zuw" and "zuW"
{
    खाता	*fd = रिक्त;
    buf_T	*buf = रिक्त;
    पूर्णाङ्क		new_spf = FALSE;
    char_u	*fname;
    char_u	*fnamebuf = रिक्त;
    char_u	पंक्ति[MAXWLEN * 2];
    दीर्घ	fpos, fpos_next = 0;
    पूर्णाङ्क		i;
    माप_प्रकार	linelen;
    char_u	*spf;

    यदि (!valid_spell_word(word, word + len))
    {
	emsg(_(e_illegal_character_in_word));
	प्रत्यागम;
    }

    यदि (idx == 0)	    // use internal wordlist
    {
	यदि (int_wordlist == रिक्त)
	{
	    int_wordlist = vim_tempname('s', FALSE);
	    यदि (int_wordlist == रिक्त)
		प्रत्यागम;
	}
	fname = int_wordlist;
    }
    अन्यथा
    {
	// If 'spellfile' isn't set figure out a good default value.
	यदि (*curwin->w_s->b_p_spf == NUL)
	{
	    init_spellfile();
	    new_spf = TRUE;
	}

	यदि (*curwin->w_s->b_p_spf == NUL)
	{
	    semsg(_(e_option_str_is_not_set), "spellfile");
	    प्रत्यागम;
	}
	fnamebuf = alloc(MAXPATHL);
	यदि (fnamebuf == रिक्त)
	    प्रत्यागम;

	प्रति (spf = curwin->w_s->b_p_spf, i = 1; *spf != NUL; ++i)
	{
	    copy_option_part(&spf, fnamebuf, MAXPATHL, ",");
	    यदि (i == idx)
		भङ्ग;
	    यदि (*spf == NUL)
	    {
		semsg(_(e_spellfile_does_not_have_nr_entries), idx);
		vim_free(fnamebuf);
		प्रत्यागम;
	    }
	}

	// Check that the user isn't editing the .add file somewhere.
	buf = buflist_findname_exp(fnamebuf);
	यदि (buf != रिक्त && buf->b_ml.ml_mfp == रिक्त)
	    buf = रिक्त;
	यदि (buf != रिक्त && bufIsChanged(buf))
	{
	    emsg(_(e_file_is_loaded_in_another_buffer));
	    vim_free(fnamebuf);
	    प्रत्यागम;
	}

	fname = fnamebuf;
    }

    यदि (what == SPELL_ADD_BAD || undo)
    {
	// When the word appears as good word we need to remove that one,
	// since its flags sort before the one with WF_BANNED.
	fd = mch_fopen((वर्ण *)fname, "r");
	यदि (fd != रिक्त)
	{
	    यावत् (!vim_fgets(पंक्ति, MAXWLEN * 2, fd))
	    {
		fpos = fpos_next;
		fpos_next = ख_बताओ(fd);
		यदि (fpos_next < 0)
		    भङ्ग;  // should never happen
		linelen = STRLEN(पंक्ति);
		यदि (linelen >= (माप_प्रकार)len
			&& STRNCMP(word, पंक्ति, len) == 0
			&& (पंक्ति[len] == '/' || पंक्ति[len] < ' '))
		{
		    // Found duplicate word.  Remove it by writing a '#' at
		    // the start of the line.  Mixing reading and writing
		    // doesn't work for all systems, close the file first.
		    ख_बंद(fd);
		    fd = mch_fopen((वर्ण *)fname, "r+");
		    यदि (fd == रिक्त)
			भङ्ग;
		    यदि (ख_जाओ(fd, fpos, शुरू_से) == 0)
		    {
			ख_अक्षर_दो('#', fd);
			यदि (undo)
			{
			    home_replace(रिक्त, fname, NameBuff, MAXPATHL, TRUE);
			    smsg(_("Word '%.*s' removed from %s"),
							 len, word, NameBuff);
			}
		    }
		    यदि (ख_जाओ(fd, fpos_next, शुरू_से) != 0)
		    {
			PERROR(_("Seek error in spellfile"));
			भङ्ग;
		    }
		}
	    }
	    यदि (fd != रिक्त)
		ख_बंद(fd);
	}
    }

    यदि (!undo)
    {
	fd = mch_fopen((वर्ण *)fname, "a");
	यदि (fd == रिक्त && new_spf)
	{
	    char_u *p;

	    // We just initialized the 'spellfile' option and can't open the
	    // file.  We may need to create the "spell" directory first.  We
	    // already checked the runtime directory is writable in
	    // init_spellfile().
	    यदि (!dir_of_file_exists(fname) && (p = gettail_sep(fname)) != fname)
	    {
		पूर्णाङ्क c = *p;

		// The directory doesn't exist.  Try creating it and opening
		// the file again.
		*p = NUL;
		vim_mkdir(fname, 0755);
		*p = c;
		fd = mch_fopen((वर्ण *)fname, "a");
	    }
	}

	यदि (fd == रिक्त)
	    semsg(_(e_cant_open_file_str), fname);
	अन्यथा
	{
	    यदि (what == SPELL_ADD_BAD)
		ख_लिखो(fd, "%.*s/!\n", len, word);
	    अन्यथा यदि (what == SPELL_ADD_RARE)
		ख_लिखो(fd, "%.*s/?\n", len, word);
	    अन्यथा
		ख_लिखो(fd, "%.*s\n", len, word);
	    ख_बंद(fd);

	    home_replace(रिक्त, fname, NameBuff, MAXPATHL, TRUE);
	    smsg(_("Word '%.*s' added to %s"), len, word, NameBuff);
	}
    }

    यदि (fd != रिक्त)
    {
	// Update the .add.spl file.
	mkspell(1, &fname, FALSE, TRUE, TRUE);

	// If the .add file is edited somewhere, reload it.
	यदि (buf != रिक्त)
	    buf_reload(buf, buf->b_orig_mode, FALSE);

	redraw_all_later(UPD_SOME_VALID);
    }
    vim_free(fnamebuf);
}

/*
 * Initialize 'spellfile' for the current buffer.
 */
    स्थिर शून्य
init_spellfile(शून्य)
{
    string_T	buf;
    char_u	*fname;
    char_u	*rtp;
    char_u	*lend;
    पूर्णाङ्क		aspath = FALSE;
    char_u	*lstart = curbuf->b_s.b_p_spl;

    यदि (*curwin->w_s->b_p_spl == NUL || curwin->w_s->b_langp.ga_len <= 0)
	प्रत्यागम;

    buf.string = alloc(MAXPATHL);
    यदि (buf.string == रिक्त)
	प्रत्यागम;

    buf.length = 0;

    // Find the end of the language name.  Exclude the region.  If there
    // is a path separator remember the start of the tail.
    प्रति (lend = curwin->w_s->b_p_spl; *lend != NUL
	    && vim_strchr((char_u *)",._", *lend) == रिक्त; ++lend)
	यदि (vim_ispathsep(*lend))
	{
	    aspath = TRUE;
	    lstart = lend + 1;
	}

    // Loop over all entries in 'runtimepath'.  Use the first one where we
    // are allowed to write.
    rtp = p_rtp;
    यावत् (*rtp != NUL)
    {
	यदि (aspath)
	    // Use directory of an entry with path, e.g., for
	    // "/dir/lg.utf-8.spl" use "/dir".
	    vim_strncpy(buf.string, curbuf->b_s.b_p_spl,
		    lstart - curbuf->b_s.b_p_spl - 1);
	अन्यथा
	    // Copy the path from 'runtimepath' to buf[].
	    buf.length = (माप_प्रकार)copy_option_part(&rtp, buf.string, MAXPATHL, ",");
	यदि (filewritable(buf.string) == 2)
	{
	    // Use the first language name from 'spelllang' and the
	    // encoding used in the first loaded .spl file.
	    यदि (aspath)
	    {
		buf.length = (माप_प्रकार)(lend - curbuf->b_s.b_p_spl);
		vim_strncpy(buf.string, curbuf->b_s.b_p_spl, buf.length);
	    }
	    अन्यथा
	    {
		// Create the "spell" directory if it doesn't exist yet.
		buf.length += vim_snprintf_safelen((वर्ण *)buf.string + buf.length,
		    MAXPATHL - buf.length, "/spell");
		यदि (filewritable(buf.string) != 2)
		{
		    यदि (vim_mkdir(buf.string, 0755) != 0)
		    {
			vim_free(buf.string);
			प्रत्यागम;
		    }
		}

		buf.length += vim_snprintf_safelen((वर्ण *)buf.string + buf.length,
		    MAXPATHL - buf.length, "/%.*s", (पूर्णाङ्क)(lend - lstart), lstart);
	    }
	    fname = LANGP_ENTRY(curwin->w_s->b_langp, 0)
		->lp_slang->sl_fname;
	    vim_snprintf((वर्ण *)buf.string + buf.length, MAXPATHL - buf.length,
		".%s.add",
		fname != रिक्त && म_माला((वर्ण *)gettail(fname), ".ascii.") != रिक्त
		    ? (char_u *)"ascii" : spell_enc());
	    set_option_मूल्य_give_err((char_u *)"spellfile",
		    0L, buf.string, OPT_LOCAL);
	    भङ्ग;
	}
	aspath = FALSE;
    }

    vim_free(buf.string);
}



/*
 * Set the spell character tables from strings in the affix file.
 */
    स्थिर पूर्णाङ्क
set_spell_chartab(char_u *fol, char_u *निचला, char_u *upp)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    char_u	*pf = fol, *pl = निचला, *pu = upp;
    पूर्णाङ्क		f, l, u;

    clear_spell_chartab(&new_st);

    यावत् (*pf != NUL)
    {
	यदि (*pl == NUL || *pu == NUL)
	{
	    emsg(_(e_format_error_in_affix_file_fol_निचला_or_upp));
	    प्रत्यागम FAIL;
	}
	f = mb_ptr2char_adv(&pf);
	l = mb_ptr2char_adv(&pl);
	u = mb_ptr2char_adv(&pu);

	// Every character that appears is a word character.
	यदि (f < 256)
	    new_st.st_isw[f] = TRUE;
	यदि (l < 256)
	    new_st.st_isw[l] = TRUE;
	यदि (u < 256)
	    new_st.st_isw[u] = TRUE;

	// if "LOW" and "FOL" are not the same the "LOW" char needs
	// case-folding
	यदि (l < 256 && l != f)
	{
	    यदि (f >= 256)
	    {
		emsg(_(e_character_in_fol_निचला_or_upp_is_out_of_range));
		प्रत्यागम FAIL;
	    }
	    new_st.st_fold[l] = f;
	}

	// if "UPP" and "FOL" are not the same the "UPP" char needs
	// case-folding, it's upper case and the "UPP" is the upper case of
	// "FOL" .
	यदि (u < 256 && u != f)
	{
	    यदि (f >= 256)
	    {
		emsg(_(e_character_in_fol_निचला_or_upp_is_out_of_range));
		प्रत्यागम FAIL;
	    }
	    new_st.st_fold[u] = f;
	    new_st.st_isu[u] = TRUE;
	    new_st.st_upper[f] = u;
	}
    }

    यदि (*pl != NUL || *pu != NUL)
    {
	emsg(_(e_format_error_in_affix_file_fol_निचला_or_upp));
	प्रत्यागम FAIL;
    }

    प्रत्यागम set_spell_finish(&new_st);
}

/*
 * Set the spell character tables from strings in the .spl file.
 */
    स्थिर शून्य
set_spell_charflags(
    char_u	*flags,
    पूर्णाङ्क		cnt,	    // length of "flags"
    char_u	*fol)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    पूर्णाङ्क		i;
    char_u	*p = fol;
    पूर्णाङ्क		c;

    clear_spell_chartab(&new_st);

    प्रति (i = 0; i < 128; ++i)
    {
	यदि (i < cnt)
	{
	    new_st.st_isw[i + 128] = (flags[i] & CF_WORD) != 0;
	    new_st.st_isu[i + 128] = (flags[i] & CF_UPPER) != 0;
	}

	यदि (*p != NUL)
	{
	    c = mb_ptr2char_adv(&p);
	    new_st.st_fold[i + 128] = c;
	    यदि (i + 128 != c && new_st.st_isu[i + 128] && c < 256)
		new_st.st_upper[c] = i + 128;
	}
    }

    (शून्य)set_spell_finish(&new_st);
}

    स्थिर पूर्णाङ्क
set_spell_finish(spelltab_T *new_st)
{
    पूर्णाङ्क		i;

    यदि (did_set_spelltab)
    {
	// check that it's the same table
	प्रति (i = 0; i < 256; ++i)
	{
	    यदि (spelltab.st_isw[i] != new_st->st_isw[i]
		    || spelltab.st_isu[i] != new_st->st_isu[i]
		    || spelltab.st_fold[i] != new_st->st_fold[i]
		    || spelltab.st_upper[i] != new_st->st_upper[i])
	    {
		emsg(_(e_word_characters_differ_between_spell_files));
		प्रत्यागम FAIL;
	    }
	}
    }
    अन्यथा
    {
	// copy the new spelltab into the one being used
	spelltab = *new_st;
	did_set_spelltab = TRUE;
    }

    प्रत्यागम OK;
}

/*
 * Write the table with prefix conditions to the .spl file.
 * When "fd" is NULL only count the length of what is written and return it.
 */
    स्थिर पूर्णाङ्क
write_spell_prefcond(खाता *fd, garray_T *gap, माप_प्रकार *fwv)
{
    पूर्णाङ्क		i;
    char_u	*p;
    पूर्णाङ्क		len;
    पूर्णाङ्क		totlen;

    यदि (fd != रिक्त)
	put_bytes(fd, (long_u)gap->ga_len, 2);	    // <prefcondcnt>

    totlen = 2 + gap->ga_len; // length of <prefcondcnt> and <condlen> bytes

    प्रति (i = 0; i < gap->ga_len; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	p = ((char_u **)gap->ga_data)[i];
	यदि (p != रिक्त)
	{
	    len = (पूर्णाङ्क)STRLEN(p);
	    यदि (fd != रिक्त)
	    {
		ख_अक्षर_दो(len, fd);
		*fwv &= ख_डालो(p, (माप_प्रकार)len, (माप_प्रकार)1, fd);
	    }
	    totlen += len;
	}
	अन्यथा यदि (fd != रिक्त)
	    ख_अक्षर_दो(0, fd);
    }

    प्रत्यागम totlen;
}

/*
 * Use map string "map" for languages "lp".
 */
    स्थिर शून्य
set_map_str(slang_T *lp, char_u *map)
{
    char_u	*p;
    पूर्णाङ्क		headc = 0;
    पूर्णाङ्क		c;
    पूर्णाङ्क		i;

    यदि (*map == NUL)
    {
	lp->sl_has_map = FALSE;
	प्रत्यागम;
    }
    lp->sl_has_map = TRUE;

    // Init the array and hash tables empty.
    प्रति (i = 0; i < 256; ++i)
	lp->sl_map_array[i] = 0;
    hash_init(&lp->sl_map_hash);

    /*
     * The similar characters are stored separated with slashes:
     * "aaa/bbb/ccc/".  Fill sl_map_array[c] with the character before c and
     * before the same slash.  For characters above 255 sl_map_hash is used.
     */
    प्रति (p = map; *p != NUL; )
    {
	c = mb_cptr2char_adv(&p);
	यदि (c == '/')
	    headc = 0;
	अन्यथा
	{
	    यदि (headc == 0)
		 headc = c;

	    // Characters above 255 don't fit in sl_map_array[], put them in
	    // the hash table.  Each entry is the char, a NUL the headchar and
	    // a NUL.
	    यदि (c >= 256)
	    {
		पूर्णाङ्क	    cl = mb_char2len(c);
		पूर्णाङ्क	    headcl = mb_char2len(headc);
		char_u	    *b;
		hash_T	    hash;
		hashitem_T  *hi;

		b = alloc(cl + headcl + 2);
		यदि (b == रिक्त)
		    प्रत्यागम;
		mb_char2bytes(c, b);
		b[cl] = NUL;
		mb_char2bytes(headc, b + cl + 1);
		b[cl + 1 + headcl] = NUL;
		hash = hash_hash(b);
		hi = hash_lookup(&lp->sl_map_hash, b, hash);
		यदि (HASHITEM_EMPTY(hi))
		    hash_add_item(&lp->sl_map_hash, hi, b, hash);
		अन्यथा
		{
		    // This should have been checked when generating the .spl
		    // file.
		    emsg(_(e_duplicate_char_in_map_entry));
		    vim_free(b);
		}
	    }
	    अन्यथा
		lp->sl_map_array[c] = headc;
	}
    }
}

#endif  // FEAT_SPELL
