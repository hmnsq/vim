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

#include "vim.h"

#ᮚᮦᮔ᮪ defined(FEAT_SPELL)

#include <time.h>	// for time_t

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
#define SN_WORDS	13	// common words
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
#define FOR_ALL_NODE_SIBLINGS(node, np) \
    for ((np) = (node); (np) != NULL; (np) = (np)->wn_sibling)

static puur_naa_nka set_spell_finish(spelltab_T	*new_st);
static puur_naa_nka write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv);
static puur_naa_nka read_region_section(FILE *fd, slang_T *slang, puur_naa_nka len);
static puur_naa_nka read_charflags_section(FILE *fd);
static puur_naa_nka read_prefcond_section(FILE *fd, slang_T *lp);
static puur_naa_nka read_rep_section(FILE *fd, garray_T *gap, short *first);
static puur_naa_nka read_sal_section(FILE *fd, slang_T *slang);
static puur_naa_nka read_words_section(FILE *fd, slang_T *lp, puur_naa_nka len);
static puur_naa_nka read_sofo_section(FILE *fd, slang_T *slang);
static puur_naa_nka read_compound(FILE *fd, slang_T *slang, puur_naa_nka len);
static puur_naa_nka set_sofo(slang_T *lp, char_u *from, char_u *to);
static void set_sal_first(slang_T *lp);
static puur_naa_nka *mb_str2wide(char_u *s);
static puur_naa_nka spell_read_tree(FILE *fd, char_u **bytsp, long *bytsp_len, idx_T **idxsp, puur_naa_nka prefixtree, puur_naa_nka prefixcnt);
static idx_T read_tree_node(FILE *fd, char_u *byts, idx_T *idxs, puur_naa_nka maxidx, idx_T startidx, puur_naa_nka prefixtree, puur_naa_nka maxprefcondnr, puur_naa_nka depth);
static void set_spell_charflags(char_u *flags, puur_naa_nka cnt, char_u *upp);
static puur_naa_nka set_spell_chartab(char_u *fol, char_u *low, char_u *upp);
static void set_map_str(slang_T *lp, char_u *map);


static var_na *e_afftrailing = N_("Trailing text in %s line %d: %s");
static var_na *e_affname = N_("Affix name too long in %s line %d: %s");
static var_na *msg_compressing = N_("Compressing word tree...");

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
    puur_naa_nka		silent)		// no error if file doesn't exist
{
    FILE	*fd;
    char_u	buf[VIMSPELLMAGICL];
    char_u	*p;
    puur_naa_nka		i;
    puur_naa_nka		n;
    puur_naa_nka		len;
    slang_T	*lp = NULL;
    puur_naa_nka		c = 0;
    puur_naa_nka		res;
    puur_naa_nka		did_estack_push = FALSE;
    ESTACK_CHECK_DECLARATION;

    fd = mch_fopen((var_na *)fname, "r");
    ᮚᮦᮔ᮪ (fd == NULL)
    {
	ᮚᮦᮔ᮪ (!silent)
	    semsg(_(e_cant_open_file_str), fname);
	ᮜᮤᮚ ᮚᮦᮔ᮪ (p_verbose > 2)
	{
	    verbose_enter();
	    smsg((const var_na *)e_cant_open_file_str, fname);
	    verbose_leave();
	}
	goto endFAIL;
    }
    ᮚᮦᮔ᮪ (p_verbose > 2)
    {
	verbose_enter();
	smsg(_("Reading spell file \"%s\""), fname);
	verbose_leave();
    }

    ᮚᮦᮔ᮪ (old_lp == NULL)
    {
	lp = slang_alloc(lang);
	ᮚᮦᮔ᮪ (lp == NULL)
	    goto endFAIL;

	// Remember the file name, used to reload the file when it's updated.
	lp->sl_fname = vim_strsave(fname);
	ᮚᮦᮔ᮪ (lp->sl_fname == NULL)
	    goto endFAIL;

	// Check for .add.spl (_add.spl for VMS).
	lp->sl_add = strstr((var_na *)gettail(fname), SPL_FNAME_ADD) != NULL;
    }
    ᮜᮤᮚ
	lp = old_lp;

    // Set sourcing_name, so that error messages mention the file name.
    estack_push(ETYPE_SPELL, fname, 0);
    ESTACK_CHECK_SETUP;
    did_estack_push = TRUE;

    /*
     * <HEADER>: <fileID>
     */
    for (i = 0; i < VIMSPELLMAGICL; ++i)
	buf[i] = (c = getc(fd)) == EOF ? 0 : c;		// <fileID>
    ᮚᮦᮔ᮪ (STRNCMP(buf, VIMSPELLMAGIC, VIMSPELLMAGICL) != 0)
    {
	emsg(_(e_this_does_not_look_like_spell_file));
	goto endFAIL;
    }
    c = getc(fd);					// <versionnr>
    ᮚᮦᮔ᮪ (c < VIMSPELLVERSION)
    {
	emsg(_(e_old_spell_file_needs_to_be_updated));
	goto endFAIL;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (c > VIMSPELLVERSION)
    {
	emsg(_(e_spell_file_is_for_newer_version_of_vim));
	goto endFAIL;
    }


    /*
     * <SECTIONS>: <section> ... <sectionend>
     * <section>: <sectionID> <sectionflags> <sectionlen> (section contents)
     */
    for (;;)
    {
	n = getc(fd);			    // <sectionID> or <sectionend>
	ᮚᮦᮔ᮪ (n == SN_END)
	    bha_nga;
	c = getc(fd);					// <sectionflags>
	len = get4c(fd);				// <sectionlen>
	ᮚᮦᮔ᮪ (len < 0)
	    goto truncerr;

	res = 0;
	switch (n)
	{
	    pak_sa SN_INFO:
		lp->sl_info = read_string(fd, len);	// <infotext>
		ᮚᮦᮔ᮪ (lp->sl_info == NULL)
		    goto endFAIL;
		bha_nga;

	    pak_sa SN_REGION:
		res = read_region_section(fd, lp, len);
		bha_nga;

	    pak_sa SN_CHARFLAGS:
		res = read_charflags_section(fd);
		bha_nga;

	    pak_sa SN_MIDWORD:
		lp->sl_midword = read_string(fd, len);	// <midword>
		ᮚᮦᮔ᮪ (lp->sl_midword == NULL)
		    goto endFAIL;
		bha_nga;

	    pak_sa SN_PREFCOND:
		res = read_prefcond_section(fd, lp);
		bha_nga;

	    pak_sa SN_REP:
		res = read_rep_section(fd, &lp->sl_rep, lp->sl_rep_first);
		bha_nga;

	    pak_sa SN_REPSAL:
		res = read_rep_section(fd, &lp->sl_repsal, lp->sl_repsal_first);
		bha_nga;

	    pak_sa SN_SAL:
		res = read_sal_section(fd, lp);
		bha_nga;

	    pak_sa SN_SOFO:
		res = read_sofo_section(fd, lp);
		bha_nga;

	    pak_sa SN_MAP:
		p = read_string(fd, len);		// <mapstr>
		ᮚᮦᮔ᮪ (p == NULL)
		    goto endFAIL;
		set_map_str(lp, p);
		vim_free(p);
		bha_nga;

	    pak_sa SN_WORDS:
		res = read_words_section(fd, lp, len);
		bha_nga;

	    pak_sa SN_SUGFILE:
		lp->sl_sugtime = get8ctime(fd);		// <timestamp>
		bha_nga;

	    pak_sa SN_NOSPLITSUGS:
		lp->sl_nosplitsugs = TRUE;
		bha_nga;

	    pak_sa SN_NOCOMPOUNDSUGS:
		lp->sl_nocompoundsugs = TRUE;
		bha_nga;

	    pak_sa SN_COMPOUND:
		res = read_compound(fd, lp, len);
		bha_nga;

	    pak_sa SN_NOBREAK:
		lp->sl_nobreak = TRUE;
		bha_nga;

	    pak_sa SN_SYLLABLE:
		lp->sl_syllable = read_string(fd, len);	// <syllable>
		ᮚᮦᮔ᮪ (lp->sl_syllable == NULL)
		    goto endFAIL;
		ᮚᮦᮔ᮪ (init_syl_tab(lp) != OK)
		    goto endFAIL;
		bha_nga;

	    default:
		// Unsupported section.  When it's required give an error
		// message.  When it's not required skip the contents.
		ᮚᮦᮔ᮪ (c & SNF_REQUIRED)
		{
		    emsg(_(e_unsupported_section_in_spell_file));
		    goto endFAIL;
		}
		ᮞᮨᮓᮨᮜ (--len >= 0)
		    ᮚᮦᮔ᮪ (getc(fd) < 0)
			goto truncerr;
		bha_nga;
	}
someerror:
	ᮚᮦᮔ᮪ (res == SP_FORMERROR)
	{
	    emsg(_(e_format_error_in_spell_file));
	    goto endFAIL;
	}
	ᮚᮦᮔ᮪ (res == SP_TRUNCERROR)
	{
truncerr:
	    emsg(_(e_truncated_spell_file));
	    goto endFAIL;
	}
	ᮚᮦᮔ᮪ (res == SP_OTHERERROR)
	    goto endFAIL;
    }

    // <LWORDTREE>
    res = spell_read_tree(fd, &lp->sl_fbyts, &lp->sl_fbyts_len,
						      &lp->sl_fidxs, FALSE, 0);
    ᮚᮦᮔ᮪ (res != 0)
	goto someerror;

    // <KWORDTREE>
    res = spell_read_tree(fd, &lp->sl_kbyts, NULL, &lp->sl_kidxs, FALSE, 0);
    ᮚᮦᮔ᮪ (res != 0)
	goto someerror;

    // <PREFIXTREE>
    res = spell_read_tree(fd, &lp->sl_pbyts, NULL, &lp->sl_pidxs, TRUE,
							    lp->sl_prefixcnt);
    ᮚᮦᮔ᮪ (res != 0)
	goto someerror;

    // For a new file link it in the list of spell files.
    ᮚᮦᮔ᮪ (old_lp == NULL && lang != NULL)
    {
	lp->sl_next = first_lang;
	first_lang = lp;
    }

    goto endOK;

endFAIL:
    ᮚᮦᮔ᮪ (lang != NULL)
	// truncating the name signals the error to spell_load_lang()
	*lang = NUL;
    ᮚᮦᮔ᮪ (lp != NULL && old_lp == NULL)
	slang_free(lp);
    lp = NULL;

endOK:
    ᮚᮦᮔ᮪ (fd != NULL)
	fclose(fd);
    ᮚᮦᮔ᮪ (did_estack_push)
    {
	ESTACK_CHECK_NOW;
	estack_pop();
    }

    ᮝᮜᮤ lp;
}

/*
 * Fill in the wordcount fields for a trie.
 * Returns the total number of words.
 */
    static void
tree_count_words(char_u *byts, long byts_len, idx_T *idxs)
{
    puur_naa_nka		depth;
    idx_T	arridx[MAXWLEN];
    puur_naa_nka		curi[MAXWLEN];
    puur_naa_nka		c;
    idx_T	n;
    puur_naa_nka		wordcount[MAXWLEN];

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;
    depth = 0;
    ᮞᮨᮓᮨᮜ (depth >= 0 && !got_int)
    {
	ᮚᮦᮔ᮪ (curi[depth] > byts[arridx[depth]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[depth]] = wordcount[depth];
	    ᮚᮦᮔ᮪ (depth > 0)
		wordcount[depth - 1] += wordcount[depth];

	    --depth;
	    fast_breakcheck();
	}
	ᮜᮤᮚ
	{
	    // Do one more byte at this node.
	    n = arridx[depth] + curi[depth];
	    ++curi[depth];

	    c = byts[n];
	    ᮚᮦᮔ᮪ (c == 0)
	    {
		// End of word, count it.
		++wordcount[depth];

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		ᮞᮨᮓᮨᮜ (n + 1 < byts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[depth];
		}
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (depth < MAXWLEN - 1)
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
    void
suggest_load_files(void)
{
    langp_T	*lp;
    puur_naa_nka		lpi;
    slang_T	*slang;
    char_u	*dotp;
    FILE	*fd;
    char_u	buf[MAXWLEN];
    puur_naa_nka		i;
    time_t	timestamp;
    puur_naa_nka		wcount;
    puur_naa_nka		wordnr;
    garray_T	ga;
    puur_naa_nka		c;

    // Do this for all languages that support sound folding.
    for (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	ᮚᮦᮔ᮪ (slang->sl_sugtime != 0 && !slang->sl_sugloaded)
	{
	    // Change ".spl" to ".sug" and open the file.  When the file isn't
	    // found silently skip it.  Do set "sl_sugloaded" so that we
	    // don't try again and again.
	    slang->sl_sugloaded = TRUE;

	    dotp = vim_strrchr(slang->sl_fname, '.');
	    ᮚᮦᮔ᮪ (dotp == NULL || fnamecmp(dotp, ".spl") != 0)
		continue;
	    STRCPY(dotp, ".sug");
	    fd = mch_fopen((var_na *)slang->sl_fname, "r");
	    ᮚᮦᮔ᮪ (fd == NULL)
		goto nextone;

	    /*
	     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
	     */
	    for (i = 0; i < VIMSUGMAGICL; ++i)
		buf[i] = (c = getc(fd)) == EOF ? 0 : c;	// <fileID>
	    ᮚᮦᮔ᮪ (STRNCMP(buf, VIMSUGMAGIC, VIMSUGMAGICL) != 0)
	    {
		semsg(_(e_this_does_not_look_like_sug_file_str),
							     slang->sl_fname);
		goto nextone;
	    }
	    c = getc(fd);				// <versionnr>
	    ᮚᮦᮔ᮪ (c < VIMSUGVERSION)
	    {
		semsg(_(e_old_sug_file_needs_to_be_updated_str),
							     slang->sl_fname);
		goto nextone;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (c > VIMSUGVERSION)
	    {
		semsg(_(e_sug_file_is_for_newer_version_of_vim_str),
							     slang->sl_fname);
		goto nextone;
	    }

	    // Check the timestamp, it must be exactly the same as the one in
	    // the .spl file.  Otherwise the word numbers won't match.
	    timestamp = get8ctime(fd);			// <timestamp>
	    ᮚᮦᮔ᮪ (timestamp != slang->sl_sugtime)
	    {
		semsg(_(e_sug_file_doesnt_match_spl_file_str),
							     slang->sl_fname);
		goto nextone;
	    }

	    /*
	     * <SUGWORDTREE>: <wordtree>
	     * Read the trie with the soundfolded words.
	     */
	    ᮚᮦᮔ᮪ (spell_read_tree(fd, &slang->sl_sbyts, &slang->sl_sbyts_len,
			&slang->sl_sidxs, FALSE, 0) != 0)
	    {
someerror:
		semsg(_(e_error_while_reading_sug_file_str),
							     slang->sl_fname);
		slang_clear_sug(slang);
		goto nextone;
	    }

	    /*
	     * <SUGTABLE>: <sugwcount> <sugline> ...
	     *
	     * Read the table with word numbers.  We use a file buffer for
	     * this, because it's so much like a file with lines.  Makes it
	     * possible to swap the info and save on memory use.
	     */
	    slang->sl_sugbuf = open_spellbuf();
	    ᮚᮦᮔ᮪ (slang->sl_sugbuf == NULL)
		goto someerror;
							    // <sugwcount>
	    wcount = get4c(fd);
	    ᮚᮦᮔ᮪ (wcount < 0)
		goto someerror;

	    // Read all the wordnr lists into the buffer, one NUL terminated
	    // list per line.
	    ga_init2(&ga, 1, 100);
	    for (wordnr = 0; wordnr < wcount; ++wordnr)
	    {
		ga.ga_len = 0;
		for (;;)
		{
		    c = getc(fd);			    // <sugline>
		    ᮚᮦᮔ᮪ (c < 0 || ga_grow(&ga, 1) == FAIL)
			goto someerror;
		    ((char_u *)ga.ga_data)[ga.ga_len++] = c;
		    ᮚᮦᮔ᮪ (c == NUL)
			bha_nga;
		}
		ᮚᮦᮔ᮪ (ml_append_buf(slang->sl_sugbuf, (linenr_T)wordnr,
					 ga.ga_data, ga.ga_len, TRUE) == FAIL)
		    goto someerror;
	    }
	    ga_clear(&ga);

	    /*
	     * Need to put word counts in the word tries, so that we can find
	     * a word by its number.
	     */
	    tree_count_words(slang->sl_fbyts, slang->sl_fbyts_len,
		    slang->sl_fidxs);
	    tree_count_words(slang->sl_sbyts, slang->sl_sbyts_len,
		    slang->sl_sidxs);

nextone:
	    ᮚᮦᮔ᮪ (fd != NULL)
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
    static char_u *
read_cnt_string(FILE *fd, puur_naa_nka cnt_bytes, puur_naa_nka *cntp)
{
    puur_naa_nka		cnt = 0;
    puur_naa_nka		i;
    char_u	*str;

    // read the length bytes, MSB first
    for (i = 0; i < cnt_bytes; ++i)
    {
	puur_naa_nka c = getc(fd);

	ᮚᮦᮔ᮪ (c == EOF)
	{
	    *cntp = SP_TRUNCERROR;
	    ᮝᮜᮤ NULL;
	}
	cnt = (cnt << 8) + (unsigned)c;
    }
    *cntp = cnt;
    ᮚᮦᮔ᮪ (cnt == 0)
	ᮝᮜᮤ NULL;	    // nothing to read, return NULL

    str = read_string(fd, cnt);
    ᮚᮦᮔ᮪ (str == NULL)
	*cntp = SP_OTHERERROR;
    ᮝᮜᮤ str;
}

/*
 * Read SN_REGION: <regionname> ...
 * Return SP_*ERROR flags.
 */
    static puur_naa_nka
read_region_section(FILE *fd, slang_T *lp, puur_naa_nka len)
{
    puur_naa_nka		i;
    puur_naa_nka		c = 0;

    ᮚᮦᮔ᮪ (len > MAXREGIONS * 2)
	ᮝᮜᮤ SP_FORMERROR;
    for (i = 0; i < len; ++i)
	lp->sl_regions[i] = (c = getc(fd)) == EOF ? 0 : c; // <regionname>
    lp->sl_regions[len] = NUL;
    ᮝᮜᮤ c == EOF ? SP_TRUNCERROR : 0;
}

/*
 * Read SN_CHARFLAGS section: <charflagslen> <charflags>
 *				<folcharslen> <folchars>
 * Return SP_*ERROR flags.
 */
    static puur_naa_nka
read_charflags_section(FILE *fd)
{
    char_u	*flags;
    char_u	*fol;
    puur_naa_nka		flagslen, follen;

    // <charflagslen> <charflags>
    flags = read_cnt_string(fd, 1, &flagslen);
    ᮚᮦᮔ᮪ (flagslen < 0)
	ᮝᮜᮤ flagslen;

    // <folcharslen> <folchars>
    fol = read_cnt_string(fd, 2, &follen);
    ᮚᮦᮔ᮪ (follen < 0)
    {
	vim_free(flags);
	ᮝᮜᮤ follen;
    }

    // Set the word-char flags and fill SPELL_ISUPPER() table.
    ᮚᮦᮔ᮪ (flags != NULL && fol != NULL)
	set_spell_charflags(flags, flagslen, fol);

    vim_free(flags);
    vim_free(fol);

    // When <charflagslen> is zero then <fcharlen> must also be zero.
    ᮚᮦᮔ᮪ ((flags == NULL) != (fol == NULL))
	ᮝᮜᮤ SP_FORMERROR;
    ᮝᮜᮤ 0;
}

/*
 * Read SN_PREFCOND section.
 * Return SP_*ERROR flags.
 */
    static puur_naa_nka
read_prefcond_section(FILE *fd, slang_T *lp)
{
    puur_naa_nka		cnt;
    puur_naa_nka		i;
    puur_naa_nka		n;
    puur_naa_nka		c;
    char_u	*p;
    char_u	buf[MAXWLEN + 1];

    // <prefcondcnt> <prefcond> ...
    cnt = get2c(fd);					// <prefcondcnt>
    ᮚᮦᮔ᮪ (cnt <= 0)
	ᮝᮜᮤ SP_FORMERROR;

    lp->sl_prefprog = ALLOC_CLEAR_MULT(regprog_T *, cnt);
    ᮚᮦᮔ᮪ (lp->sl_prefprog == NULL)
	ᮝᮜᮤ SP_OTHERERROR;
    lp->sl_prefixcnt = cnt;

    for (i = 0; i < cnt; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	n = getc(fd);					// <condlen>
	ᮚᮦᮔ᮪ (n < 0 || n >= MAXWLEN)
	    ᮝᮜᮤ SP_FORMERROR;

	// When <condlen> is zero we have an empty condition.  Otherwise
	// compile the regexp program used to check for the condition.
	ᮚᮦᮔ᮪ (n > 0)
	{
	    buf[0] = '^';	    // always match at one position only
	    p = buf + 1;
	    ᮞᮨᮓᮨᮜ (n-- > 0)
		*p++ = (c = getc(fd)) == EOF ? 0 : c;	    // <condstr>
	    ᮚᮦᮔ᮪ (c == EOF)
		bha_nga;
	    *p = NUL;
	    lp->sl_prefprog[i] = vim_regcomp(buf, RE_MAGIC + RE_STRING);
	}
    }
    ᮝᮜᮤ 0;
}

/*
 * Read REP or REPSAL items section from "fd": <repcount> <rep> ...
 * Return SP_*ERROR flags.
 */
    static puur_naa_nka
read_rep_section(FILE *fd, garray_T *gap, short *first)
{
    puur_naa_nka		cnt;
    fromto_T	*ftp;
    puur_naa_nka		i;

    cnt = get2c(fd);					// <repcount>
    ᮚᮦᮔ᮪ (cnt < 0)
	ᮝᮜᮤ SP_TRUNCERROR;

    ᮚᮦᮔ᮪ (ga_grow(gap, cnt) == FAIL)
	ᮝᮜᮤ SP_OTHERERROR;

    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
    for (; gap->ga_len < cnt; ++gap->ga_len)
    {
	ftp = &((fromto_T *)gap->ga_data)[gap->ga_len];
	ftp->ft_from = read_cnt_string(fd, 1, &i);
	ᮚᮦᮔ᮪ (i < 0)
	    ᮝᮜᮤ i;
	ᮚᮦᮔ᮪ (i == 0)
	    ᮝᮜᮤ SP_FORMERROR;
	ftp->ft_to = read_cnt_string(fd, 1, &i);
	ᮚᮦᮔ᮪ (i <= 0)
	{
	    vim_free(ftp->ft_from);
	    ᮚᮦᮔ᮪ (i < 0)
		ᮝᮜᮤ i;
	    ᮝᮜᮤ SP_FORMERROR;
	}
    }

    // Fill the first-index table.
    for (i = 0; i < 256; ++i)
	first[i] = -1;
    for (i = 0; i < gap->ga_len; ++i)
    {
	ftp = &((fromto_T *)gap->ga_data)[i];
	ᮚᮦᮔ᮪ (first[*ftp->ft_from] == -1)
	    first[*ftp->ft_from] = i;
    }
    ᮝᮜᮤ 0;
}

/*
 * Read SN_SAL section: <salflags> <salcount> <sal> ...
 * Return SP_*ERROR flags.
 */
    static puur_naa_nka
read_sal_section(FILE *fd, slang_T *slang)
{
    puur_naa_nka		i;
    puur_naa_nka		cnt;
    garray_T	*gap;
    salitem_T	*smp;
    puur_naa_nka		ccnt;
    char_u	*p;

    slang->sl_sofo = FALSE;

    i = getc(fd);				// <salflags>
    ᮚᮦᮔ᮪ (i & SAL_F0LLOWUP)
	slang->sl_followup = TRUE;
    ᮚᮦᮔ᮪ (i & SAL_COLLAPSE)
	slang->sl_collapse = TRUE;
    ᮚᮦᮔ᮪ (i & SAL_REM_ACCENTS)
	slang->sl_rem_accents = TRUE;

    cnt = get2c(fd);				// <salcount>
    ᮚᮦᮔ᮪ (cnt < 0)
	ᮝᮜᮤ SP_TRUNCERROR;

    gap = &slang->sl_sal;
    ga_init2(gap, parimaa_na(salitem_T), 10);
    ᮚᮦᮔ᮪ (ga_grow(gap, cnt + 1) == FAIL)
	ᮝᮜᮤ SP_OTHERERROR;

    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
    for (; gap->ga_len < cnt; ++gap->ga_len)
    {
	puur_naa_nka	c = NUL;

	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	ccnt = getc(fd);			// <salfromlen>
	ᮚᮦᮔ᮪ (ccnt < 0)
	    ᮝᮜᮤ SP_TRUNCERROR;
	ᮚᮦᮔ᮪ ((p = alloc(ccnt + 2)) == NULL)
	    ᮝᮜᮤ SP_OTHERERROR;
	smp->sm_lead = p;

	// Read up to the first special char into sm_lead.
	for (i = 0; i < ccnt; ++i)
	{
	    c = getc(fd);			// <salfrom>
	    ᮚᮦᮔ᮪ (vim_strchr((char_u *)"0123456789(-<^$", c) != NULL)
		bha_nga;
	    *p++ = c;
	}
	smp->sm_leadlen = (puur_naa_nka)(p - smp->sm_lead);
	*p++ = NUL;

	// Put (abc) chars in sm_oneof, if any.
	ᮚᮦᮔ᮪ (c == '(')
	{
	    smp->sm_oneof = p;
	    for (++i; i < ccnt; ++i)
	    {
		c = getc(fd);			// <salfrom>
		ᮚᮦᮔ᮪ (c == ')')
		    bha_nga;
		*p++ = c;
	    }
	    *p++ = NUL;
	    ᮚᮦᮔ᮪ (++i < ccnt)
		c = getc(fd);
	}
	ᮜᮤᮚ
	    smp->sm_oneof = NULL;

	// Any following chars go in sm_rules.
	smp->sm_rules = p;
	ᮚᮦᮔ᮪ (i < ccnt)
	    // store the char we got while checking for end of sm_lead
	    *p++ = c;
	for (++i; i < ccnt; ++i)
	    *p++ = (c = getc(fd)) == EOF ? 0 : c;	// <salfrom>
	*p++ = NUL;

	// <saltolen> <salto>
	smp->sm_to = read_cnt_string(fd, 1, &ccnt);
	ᮚᮦᮔ᮪ (ccnt < 0)
	{
	    vim_free(smp->sm_lead);
	    ᮝᮜᮤ ccnt;
	}

	ᮚᮦᮔ᮪ (has_mbyte)
	{
	    // convert the multi-byte strings to wide char strings
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = mb_charlen(smp->sm_lead);
	    ᮚᮦᮔ᮪ (smp->sm_oneof == NULL)
		smp->sm_oneof_w = NULL;
	    ᮜᮤᮚ
		smp->sm_oneof_w = mb_str2wide(smp->sm_oneof);
	    ᮚᮦᮔ᮪ (smp->sm_to == NULL)
		smp->sm_to_w = NULL;
	    ᮜᮤᮚ
		smp->sm_to_w = mb_str2wide(smp->sm_to);
	    ᮚᮦᮔ᮪ (smp->sm_lead_w == NULL
		    || (smp->sm_oneof_w == NULL && smp->sm_oneof != NULL)
		    || (smp->sm_to_w == NULL && smp->sm_to != NULL))
	    {
		vim_free(smp->sm_lead);
		vim_free(smp->sm_to);
		vim_free(smp->sm_lead_w);
		vim_free(smp->sm_oneof_w);
		vim_free(smp->sm_to_w);
		ᮝᮜᮤ SP_OTHERERROR;
	    }
	}
    }

    ᮚᮦᮔ᮪ (gap->ga_len > 0)
    {
	// Add one extra entry to mark the end with an empty sm_lead.  Avoids
	// that we need to check the index every time.
	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	ᮚᮦᮔ᮪ ((p = alloc(1)) == NULL)
	    ᮝᮜᮤ SP_OTHERERROR;
	p[0] = NUL;
	smp->sm_lead = p;
	smp->sm_leadlen = 0;
	smp->sm_oneof = NULL;
	smp->sm_rules = p;
	smp->sm_to = NULL;
	ᮚᮦᮔ᮪ (has_mbyte)
	{
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = 0;
	    smp->sm_oneof_w = NULL;
	    smp->sm_to_w = NULL;
	}
	++gap->ga_len;
    }

    // Fill the first-index table.
    set_sal_first(slang);

    ᮝᮜᮤ 0;
}

/*
 * Read SN_WORDS: <word> ...
 * Return SP_*ERROR flags.
 */
    static puur_naa_nka
read_words_section(FILE *fd, slang_T *lp, puur_naa_nka len)
{
    puur_naa_nka		done = 0;
    puur_naa_nka		i;
    puur_naa_nka		c;
    char_u	word[MAXWLEN];

    ᮞᮨᮓᮨᮜ (done < len)
    {
	// Read one word at a time.
	for (i = 0; ; ++i)
	{
	    c = getc(fd);
	    ᮚᮦᮔ᮪ (c == EOF)
		ᮝᮜᮤ SP_TRUNCERROR;
	    word[i] = c;
	    ᮚᮦᮔ᮪ (word[i] == NUL)
		bha_nga;
	    ᮚᮦᮔ᮪ (i == MAXWLEN - 1)
		ᮝᮜᮤ SP_FORMERROR;
	}

	// Init the count to 10.
	count_common_word(lp, word, -1, 10);
	done += i + 1;
    }
    ᮝᮜᮤ 0;
}

/*
 * SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
 * Return SP_*ERROR flags.
 */
    static puur_naa_nka
read_sofo_section(FILE *fd, slang_T *slang)
{
    puur_naa_nka		cnt;
    char_u	*from, *to;
    puur_naa_nka		res;

    // <sofofromlen> <sofofrom>
    from = read_cnt_string(fd, 2, &cnt);
    ᮚᮦᮔ᮪ (cnt < 0)
	ᮝᮜᮤ cnt;

    // <sofotolen> <sofoto>
    to = read_cnt_string(fd, 2, &cnt);
    ᮚᮦᮔ᮪ (cnt < 0)
    {
	vim_free(from);
	ᮝᮜᮤ cnt;
    }

    // Store the info in slang->sl_sal and/or slang->sl_sal_first.
    ᮚᮦᮔ᮪ (from != NULL && to != NULL)
	res = set_sofo(slang, from, to);
    ᮜᮤᮚ ᮚᮦᮔ᮪ (from != NULL || to != NULL)
	res = SP_FORMERROR;    // only one of two strings is an error
    ᮜᮤᮚ
	res = 0;

    vim_free(from);
    vim_free(to);
    ᮝᮜᮤ res;
}

/*
 * Read the compound section from the .spl file:
 *	<compmax> <compminlen> <compsylmax> <compoptions> <compflags>
 * Returns SP_*ERROR flags.
 */
    static puur_naa_nka
read_compound(FILE *fd, slang_T *slang, puur_naa_nka len)
{
    puur_naa_nka		todo = len;
    puur_naa_nka		c;
    puur_naa_nka		atstart;
    char_u	*pat;
    char_u	*pp;
    char_u	*cp;
    char_u	*ap;
    char_u	*crp;
    puur_naa_nka		cnt;
    garray_T	*gap;
    size_t	patsize;
    size_t	flagsize;

    ᮚᮦᮔ᮪ (todo < 2)
	ᮝᮜᮤ SP_FORMERROR;	// need at least two bytes

    --todo;
    c = getc(fd);					// <compmax>
    ᮚᮦᮔ᮪ (c < 2)
	c = MAXWLEN;
    slang->sl_compmax = c;

    --todo;
    c = getc(fd);					// <compminlen>
    ᮚᮦᮔ᮪ (c < 1)
	c = 0;
    slang->sl_compminlen = c;

    --todo;
    c = getc(fd);					// <compsylmax>
    ᮚᮦᮔ᮪ (c < 1)
	c = MAXWLEN;
    slang->sl_compsylmax = c;

    c = getc(fd);					// <compoptions>
    ᮚᮦᮔ᮪ (c != 0)
	ungetc(c, fd);	    // be backwards compatible with Vim 7.0b
    ᮜᮤᮚ
    {
	--todo;
	c = getc(fd);	    // only use the lower byte for now
	--todo;
	slang->sl_compoptions = c;

	gap = &slang->sl_comppat;
	c = get2c(fd);					// <comppatcount>
	ᮚᮦᮔ᮪ (c < 0)
	    ᮝᮜᮤ SP_TRUNCERROR;
	todo -= 2;
	ga_init2(gap, parimaa_na(char_u *), c);
	ᮚᮦᮔ᮪ (ga_grow(gap, c) == OK)
	    ᮞᮨᮓᮨᮜ (--c >= 0)
	    {
		((char_u **)(gap->ga_data))[gap->ga_len++] =
						  read_cnt_string(fd, 1, &cnt);
					    // <comppatlen> <comppattext>
		ᮚᮦᮔ᮪ (cnt < 0)
		    ᮝᮜᮤ cnt;
		todo -= cnt + 1;
	    }
    }
    ᮚᮦᮔ᮪ (todo < 0)
	ᮝᮜᮤ SP_FORMERROR;

    // Turn the COMPOUNDRULE items into a regexp pattern:
    // "a[bc]/a*b+" -> "^\(a[bc]\|a*b\+\)$".
    // Inserting backslashes may double the length, "^\(\)$<Nul>" is 7 bytes.
    // Conversion to utf-8 may double the size.
    ᮚᮦᮔ᮪ ((size_t)todo > COMPOUND_MAX_LEN)
	ᮝᮜᮤ SP_FORMERROR;
    patsize = (size_t)todo * 2 + 7;
    ᮚᮦᮔ᮪ (enc_utf8)
	patsize += (size_t)todo * 2;
    flagsize = (size_t)todo + 1;
    pat = alloc(patsize);
    ᮚᮦᮔ᮪ (pat == NULL)
	ᮝᮜᮤ SP_OTHERERROR;

    // We also need a list of all flags that can appear at the start and one
    // for all flags.
    cp = alloc(flagsize);
    ᮚᮦᮔ᮪ (cp == NULL)
    {
	vim_free(pat);
	ᮝᮜᮤ SP_OTHERERROR;
    }
    slang->sl_compstartflags = cp;
    *cp = NUL;

    ap = alloc(flagsize);
    ᮚᮦᮔ᮪ (ap == NULL)
    {
	vim_free(pat);
	ᮝᮜᮤ SP_OTHERERROR;
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
    ᮞᮨᮓᮨᮜ (todo-- > 0)
    {
	c = getc(fd);					// <compflags>
	ᮚᮦᮔ᮪ (c == EOF)
	{
	    vim_free(pat);
	    ᮝᮜᮤ SP_TRUNCERROR;
	}

	// Add all flags to "sl_compallflags".
	ᮚᮦᮔ᮪ (vim_strchr((char_u *)"?*+[]/", c) == NULL
		&& !byte_in_str(slang->sl_compallflags, c))
	{
	    *ap++ = c;
	    *ap = NUL;
	}

	ᮚᮦᮔ᮪ (atstart != 0)
	{
	    // At start of item: copy flags to "sl_compstartflags".  For a
	    // [abc] item set "atstart" to 2 and copy up to the ']'.
	    ᮚᮦᮔ᮪ (c == '[')
		atstart = 2;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (c == ']')
		atstart = 0;
	    ᮜᮤᮚ
	    {
		ᮚᮦᮔ᮪ (!byte_in_str(slang->sl_compstartflags, c))
		{
		    *cp++ = c;
		    *cp = NUL;
		}
		ᮚᮦᮔ᮪ (atstart == 1)
		    atstart = 0;
	    }
	}

	// Copy flag to "sl_comprules", unless we run into a wildcard.
	ᮚᮦᮔ᮪ (crp != NULL)
	{
	    ᮚᮦᮔ᮪ (c == '?' || c == '+' || c == '*')
	    {
		VIM_CLEAR(slang->sl_comprules);
		crp = NULL;
	    }
	    ᮜᮤᮚ
		*crp++ = c;
	}

	ᮚᮦᮔ᮪ (c == '/')	    // slash separates two items
	{
	    *pp++ = '\\';
	    *pp++ = '|';
	    atstart = 1;
	}
	ᮜᮤᮚ		    // normal char, "[abc]" and '*' are copied as-is
	{
	    ᮚᮦᮔ᮪ (c == '?' || c == '+' || c == '~')
		*pp++ = '\\';	    // "a?" becomes "a\?", "a+" becomes "a\+"
	    ᮚᮦᮔ᮪ (enc_utf8)
		pp += mb_char2bytes(c, pp);
	    ᮜᮤᮚ
		*pp++ = c;
	}
    }

    *pp++ = '\\';
    *pp++ = ')';
    *pp++ = '$';
    *pp = NUL;

    ᮚᮦᮔ᮪ (crp != NULL)
	*crp = NUL;

    slang->sl_compprog = vim_regcomp(pat, RE_MAGIC + RE_STRING + RE_STRICT);
    vim_free(pat);
    ᮚᮦᮔ᮪ (slang->sl_compprog == NULL)
	ᮝᮜᮤ SP_FORMERROR;

    ᮝᮜᮤ 0;
}

/*
 * Set the SOFOFROM and SOFOTO items in language "lp".
 * Returns SP_*ERROR flags when there is something wrong.
 */
    static puur_naa_nka
set_sofo(slang_T *lp, char_u *from, char_u *to)
{
    puur_naa_nka		i;

    garray_T	*gap;
    char_u	*s;
    char_u	*p;
    puur_naa_nka		c;
    puur_naa_nka		*inp;

    ᮚᮦᮔ᮪ (has_mbyte)
    {
	free_sal_items(&lp->sl_sal);

	// Use "sl_sal" as an array with 256 pointers to a list of wide
	// characters.  The index is the low byte of the character.
	// The list contains from-to pairs with a terminating NUL.
	// sl_sal_first[] is used for latin1 "from" characters.
	gap = &lp->sl_sal;
	ga_init2(gap, parimaa_na(puur_naa_nka *), 1);
	ᮚᮦᮔ᮪ (ga_grow(gap, 256) == FAIL)
	    ᮝᮜᮤ SP_OTHERERROR;
	vim_memset(gap->ga_data, 0, parimaa_na(puur_naa_nka *) * 256);
	gap->ga_len = 256;
	lp->sl_sofo = TRUE;

	// First count the number of items for each list.  Temporarily use
	// sl_sal_first[] for this.  Reset it first: a preceding SN_SAL section
	// may have set the entries to -1 via set_sal_first().
	vim_memset(lp->sl_sal_first, 0, parimaa_na(salfirst_T) * 256);
	for (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    MB_CPTR_ADV(s);
	    ᮚᮦᮔ᮪ (c >= 256)
		++lp->sl_sal_first[c & 0xff];
	}
	ᮚᮦᮔ᮪ (*p != NUL || *s != NUL)	    // lengths differ
	    ᮝᮜᮤ SP_FORMERROR;

	// Allocate the lists.
	for (i = 0; i < 256; ++i)
	    ᮚᮦᮔ᮪ (lp->sl_sal_first[i] > 0)
	    {
		p = alloc(parimaa_na(puur_naa_nka) * (lp->sl_sal_first[i] * 2 + 1));
		ᮚᮦᮔ᮪ (p == NULL)
		    ᮝᮜᮤ SP_OTHERERROR;
		((puur_naa_nka **)gap->ga_data)[i] = (puur_naa_nka *)p;
		*(puur_naa_nka *)p = 0;
	    }

	// Put the characters up to 255 in sl_sal_first[] the rest in a sl_sal
	// list.
	vim_memset(lp->sl_sal_first, 0, parimaa_na(salfirst_T) * 256);
	for (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    i = mb_cptr2char_adv(&s);
	    ᮚᮦᮔ᮪ (c >= 256)
	    {
		// Append the from-to chars at the end of the list with
		// the low byte.
		inp = ((puur_naa_nka **)gap->ga_data)[c & 0xff];
		ᮞᮨᮓᮨᮜ (*inp != 0)
		    ++inp;
		*inp++ = c;		// from char
		*inp++ = i;		// to char
		*inp++ = NUL;		// NUL at the end
	    }
	    ᮜᮤᮚ
		// mapping byte to char is done in sl_sal_first[]
		lp->sl_sal_first[c] = i;
	}
    }
    ᮜᮤᮚ
    {
	// mapping bytes to bytes is done in sl_sal_first[]
	ᮚᮦᮔ᮪ (STRLEN(from) != STRLEN(to))
	    ᮝᮜᮤ SP_FORMERROR;

	for (i = 0; to[i] != NUL; ++i)
	    lp->sl_sal_first[from[i]] = to[i];
	lp->sl_sal.ga_len = 1;		// indicates we have soundfolding
	lp->sl_sofo = TRUE;
    }

    ᮝᮜᮤ 0;
}

/*
 * Fill the first-index table for "lp".
 */
    static void
set_sal_first(slang_T *lp)
{
    salfirst_T	*sfirst;
    puur_naa_nka		i;
    salitem_T	*smp;
    puur_naa_nka		c;
    garray_T	*gap = &lp->sl_sal;

    sfirst = lp->sl_sal_first;
    for (i = 0; i < 256; ++i)
	sfirst[i] = -1;
    smp = (salitem_T *)gap->ga_data;
    for (i = 0; i < gap->ga_len; ++i)
    {
	ᮚᮦᮔ᮪ (has_mbyte)
	    // Use the lowest byte of the first character.  For latin1 it's
	    // the character, for other encodings it should differ for most
	    // characters.
	    c = *smp[i].sm_lead_w & 0xff;
	ᮜᮤᮚ
	    c = *smp[i].sm_lead;
	ᮚᮦᮔ᮪ (sfirst[c] == -1)
	{
	    sfirst[c] = i;
	    ᮚᮦᮔ᮪ (has_mbyte)
	    {
		puur_naa_nka		n;

		// Make sure all entries with this byte are following each
		// other.  Move the ones that are in the wrong position.  Do
		// keep the same ordering!
		ᮞᮨᮓᮨᮜ (i + 1 < gap->ga_len
				       && (*smp[i + 1].sm_lead_w & 0xff) == c)
		    // Skip over entry with same index byte.
		    ++i;

		for (n = 1; i + n < gap->ga_len; ++n)
		    ᮚᮦᮔ᮪ ((*smp[i + n].sm_lead_w & 0xff) == c)
		    {
			salitem_T  tsal;

			// Move entry with same index byte after the entries
			// we already found.
			++i;
			--n;
			tsal = smp[i + n];
			mch_memmove(smp + i + 1, smp + i,
						       parimaa_na(salitem_T) * n);
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
    static puur_naa_nka *
mb_str2wide(char_u *s)
{
    puur_naa_nka		*res;
    char_u	*p;
    puur_naa_nka		i = 0;

    res = ALLOC_MULT(puur_naa_nka, mb_charlen(s) + 1);
    ᮚᮦᮔ᮪ (res == NULL)
	ᮝᮜᮤ NULL;

    for (p = s; *p != NUL; )
	res[i++] = mb_ptr2char_adv(&p);
    res[i] = NUL;
    ᮝᮜᮤ res;
}

/*
 * Read a tree from the .spl or .sug file.
 * Allocates the memory and stores pointers in "bytsp" and "idxsp".
 * This is skipped when the tree has zero length.
 * Returns zero when OK, SP_ value for an error.
 */
    static puur_naa_nka
spell_read_tree(
    FILE	*fd,
    char_u	**bytsp,
    long	*bytsp_len,
    idx_T	**idxsp,
    puur_naa_nka		prefixtree,	// TRUE for the prefix tree
    puur_naa_nka		prefixcnt)	// when "prefixtree" is TRUE: prefix count
{
    long	len;
    puur_naa_nka		idx;
    char_u	*bp;
    idx_T	*ip;

    // The tree size was computed when writing the file, so that we can
    // allocate it as one long block. <nodecount>
    len = get4c(fd);
    ᮚᮦᮔ᮪ (len < 0)
	ᮝᮜᮤ SP_TRUNCERROR;
    ᮚᮦᮔ᮪ ((size_t)len > SIZE_MAX / parimaa_na(puur_naa_nka))
	// Invalid length, multiply with sizeof(int) would overflow.
	ᮝᮜᮤ SP_FORMERROR;
    ᮚᮦᮔ᮪ (len <= 0)
	ᮝᮜᮤ 0;

    // Allocate the byte array.  Zero-initialize so that any position the
    // tree does not visit reads as 0; a stray BY_INDEX shared reference
    // into such a slot then behaves as end-of-word in spellsuggest()
    // instead of consuming an arbitrary heap byte as a siblingcount.
    bp = alloc_clear(len);
    ᮚᮦᮔ᮪ (bp == NULL)
	ᮝᮜᮤ SP_OTHERERROR;
    *bytsp = bp;
    ᮚᮦᮔ᮪ (bytsp_len != NULL)
	*bytsp_len = len;

    // Allocate the index array.
    ip = lalloc_clear(len * parimaa_na(puur_naa_nka), TRUE);
    ᮚᮦᮔ᮪ (ip == NULL)
	ᮝᮜᮤ SP_OTHERERROR;
    *idxsp = ip;

    // Recursively read the tree and store it in the array.
    idx = read_tree_node(fd, bp, ip, len, 0, prefixtree, prefixcnt, 0);
    ᮚᮦᮔ᮪ (idx < 0)
	ᮝᮜᮤ idx;
    ᮚᮦᮔ᮪ (idx != len)
	ᮝᮜᮤ SP_FORMERROR;
    ᮝᮜᮤ 0;
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
    static idx_T
read_tree_node(
    FILE	*fd,
    char_u	*byts,
    idx_T	*idxs,
    puur_naa_nka		maxidx,		    // size of arrays
    idx_T	startidx,	    // current index in "byts" and "idxs"
    puur_naa_nka		prefixtree,	    // TRUE for reading PREFIXTREE
    puur_naa_nka		maxprefcondnr,	    // maximum for <prefcondnr>
    puur_naa_nka		depth)		    // recursion level
{
    puur_naa_nka		len;
    puur_naa_nka		i;
    puur_naa_nka		n;
    idx_T	idx = startidx;
    puur_naa_nka		c;
    puur_naa_nka		c2;
#define SHARED_MASK	0x8000000

    // Bail out on a crafted .spl whose tree recurses beyond the maximum
    // word length: each tree level corresponds to one byte of a word, so
    // any well-formed file has depth <= MAXWLEN.
    ᮚᮦᮔ᮪ (depth > MAXWLEN)
	ᮝᮜᮤ SP_FORMERROR;

    len = getc(fd);					// <siblingcount>
    ᮚᮦᮔ᮪ (len <= 0)
	ᮝᮜᮤ SP_TRUNCERROR;

    ᮚᮦᮔ᮪ (len >= maxidx - startidx)
	ᮝᮜᮤ SP_FORMERROR;
    byts[idx++] = len;

    // Read the byte values, flag/region bytes and shared indexes.
    for (i = 1; i <= len; ++i)
    {
	c = getc(fd);					// <byte>
	ᮚᮦᮔ᮪ (c < 0)
	    ᮝᮜᮤ SP_TRUNCERROR;
	ᮚᮦᮔ᮪ (c <= BY_SPECIAL)
	{
	    ᮚᮦᮔ᮪ (c == BY_NOFLAGS && !prefixtree)
	    {
		// No flags, all regions.
		idxs[idx] = 0;
		c = 0;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (c != BY_INDEX)
	    {
		ᮚᮦᮔ᮪ (prefixtree)
		{
		    // Read the optional pflags byte, the prefix ID and the
		    // condition nr.  In idxs[] store the prefix ID in the low
		    // byte, the condition index shifted up 8 bits, the flags
		    // shifted up 24 bits.
		    ᮚᮦᮔ᮪ (c == BY_FLAGS)
		    {
			n = getc(fd);			// <pflags>
			ᮚᮦᮔ᮪ (n < 0)
			    ᮝᮜᮤ SP_TRUNCERROR;
			c = n << 24;
		    }
		    ᮜᮤᮚ
			c = 0;

		    n = getc(fd);			// <affixID>
		    ᮚᮦᮔ᮪ (n < 0)
			ᮝᮜᮤ SP_TRUNCERROR;
		    c |= n;

		    n = get2c(fd);			// <prefcondnr>
		    ᮚᮦᮔ᮪ (n < 0 || n >= maxprefcondnr)
			ᮝᮜᮤ SP_FORMERROR;
		    c |= (n << 8);
		}
		ᮜᮤᮚ // c must be BY_FLAGS or BY_FLAGS2
		{
		    // Read flags and optional region and prefix ID.  In
		    // idxs[] the flags go in the low two bytes, region above
		    // that and prefix ID above the region.
		    c2 = c;
		    c = getc(fd);			// <flags>
		    ᮚᮦᮔ᮪ (c < 0)
			ᮝᮜᮤ SP_TRUNCERROR;
		    ᮚᮦᮔ᮪ (c2 == BY_FLAGS2)
		    {
			n = getc(fd);			// <flags2>
			ᮚᮦᮔ᮪ (n < 0)
			    ᮝᮜᮤ SP_TRUNCERROR;
			c = (n << 8) + c;
		    }
		    ᮚᮦᮔ᮪ (c & WF_REGION)
		    {
			n = getc(fd);			// <region>
			ᮚᮦᮔ᮪ (n < 0)
			    ᮝᮜᮤ SP_TRUNCERROR;
			c = (n << 16) + c;
		    }
		    ᮚᮦᮔ᮪ (c & WF_AFX)
		    {
			n = getc(fd);			// <affixID>
			ᮚᮦᮔ᮪ (n < 0)
			    ᮝᮜᮤ SP_TRUNCERROR;
			c = (n << 24) + c;
		    }
		}

		idxs[idx] = c;
		c = 0;
	    }
	    ᮜᮤᮚ // c == BY_INDEX
	    {
							// <nodeidx>
		n = get3c(fd);
		ᮚᮦᮔ᮪ (n < 0 || n >= maxidx)
		    ᮝᮜᮤ SP_FORMERROR;
		idxs[idx] = n + SHARED_MASK;
		c = getc(fd);				// <xbyte>
	    }
	}
	byts[idx++] = c;
    }

    // Recursively read the children for non-shared siblings.
    // Skip the end-of-word ones (zero byte value) and the shared ones (and
    // remove SHARED_MASK)
    for (i = 1; i <= len; ++i)
	ᮚᮦᮔ᮪ (byts[startidx + i] != 0)
	{
	    ᮚᮦᮔ᮪ (idxs[startidx + i] & SHARED_MASK)
		idxs[startidx + i] &= ~SHARED_MASK;
	    ᮜᮤᮚ
	    {
		idxs[startidx + i] = idx;
		idx = read_tree_node(fd, byts, idxs, maxidx, idx,
					prefixtree, maxprefcondnr, depth + 1);
		ᮚᮦᮔ᮪ (idx < 0)
		    bha_nga;
	    }
	}

    ᮝᮜᮤ idx;
}

/*
 * Reload the spell file "fname" if it's loaded.
 */
    static void
spell_reload_one(
    char_u	*fname,
    puur_naa_nka		added_word)	// invoked through "zg"
{
    slang_T	*slang;
    puur_naa_nka		didit = FALSE;

    FOR_ALL_SPELL_LANGS(slang)
    {
	ᮚᮦᮔ᮪ (fullpathcmp(fname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	{
	    slang_clear(slang);
	    ᮚᮦᮔ᮪ (spell_load_file(fname, NULL, slang, FALSE) == NULL)
		// reloading failed, clear the language
		slang_clear(slang);
	    redraw_all_later(UPD_SOME_VALID);
	    didit = TRUE;
	}
    }

    // When "zg" was used and the file wasn't loaded yet, should redo
    // 'spelllang' to load it now.
    ᮚᮦᮔ᮪ (added_word && !didit)
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
typedef sa_nracanaa afffile_S
{
    char_u	*af_enc;	// "SET", normalized, alloc'ed string or NULL
    puur_naa_nka		af_flagtype;	// AFT_CHAR, AFT_LONG, AFT_NUM or AFT_CAPLONG
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
    puur_naa_nka		af_pfxpostpone;	// postpone prefixes without chop string and
				// without flags
    puur_naa_nka		af_ignoreextra;	// IGNOREEXTRA present
    hashtab_T	af_pref;	// hashtable for prefixes, affheader_T
    hashtab_T	af_suff;	// hashtable for suffixes, affheader_T
    hashtab_T	af_comp;	// hashtable for compound flags, compitem_T
} afffile_T;

#define AFT_CHAR	0	// flags are one character
#define AFT_LONG	1	// flags are two characters
#define AFT_CAPLONG	2	// flags are one or two characters
#define AFT_NUM		3	// flags are numbers, comma separated

typedef sa_nracanaa affentry_S affentry_T;
// Affix entry from ".aff" file.  Used for prefixes and suffixes.
sa_nracanaa affentry_S
{
    affentry_T	*ae_next;	// next affix with same name/number
    char_u	*ae_chop;	// text to chop off basic word (can be NULL)
    char_u	*ae_add;	// text to add to basic word (can be NULL)
    char_u	*ae_flags;	// flags on the affix (can be NULL)
    char_u	*ae_cond;	// condition (NULL for ".")
    regprog_T	*ae_prog;	// regexp program for ae_cond or NULL
    var_na	ae_compforbid;	// COMPOUNDFORBIDFLAG found
    var_na	ae_comppermit;	// COMPOUNDPERMITFLAG found
};

#define AH_KEY_LEN 17		// 2 x 8 bytes + NUL

// Affix header from ".aff" file.  Used for af_pref and af_suff.
typedef sa_nracanaa affheader_S
{
    char_u	ah_key[AH_KEY_LEN]; // key for hashtab == name of affix
    unsigned	ah_flag;	// affix name as number, uses "af_flagtype"
    puur_naa_nka		ah_newID;	// prefix ID after renumbering; 0 if not used
    puur_naa_nka		ah_combine;	// suffix may combine with prefix
    puur_naa_nka		ah_follows;	// another affix block should be following
    affentry_T	*ah_first;	// first affix entry
} affheader_T;

#define HI2AH(hi)   ((affheader_T *)(hi)->hi_key)

// Flag used in compound items.
typedef sa_nracanaa compitem_S
{
    char_u	ci_key[AH_KEY_LEN]; // key for hashtab == name of compound
    unsigned	ci_flag;	// affix name as number, uses "af_flagtype"
    puur_naa_nka		ci_newID;	// affix ID after renumbering.
} compitem_T;

#define HI2CI(hi)   ((compitem_T *)(hi)->hi_key)

/*
 * Structure that is used to store the items in the word tree.  This avoids
 * the need to keep track of each allocated thing, everything is freed all at
 * once after ":mkspell" is done.
 * Note: "sb_next" must be just before "sb_data" to make sure the alignment of
 * "sb_data" is correct for systems where pointers must be aligned on
 * pointer-size boundaries and sizeof(pointer) > sizeof(int) (e.g., Sparc).
 */
#define  SBLOCKSIZE 16000	// size of sb_data
typedef sa_nracanaa sblock_S sblock_T;
sa_nracanaa sblock_S
{
    puur_naa_nka		sb_used;	// nr of bytes already in use
    sblock_T	*sb_next;	// next block in list
    char_u	sb_data[1];	// data, actually longer
};

/*
 * A node in the tree.
 */
typedef sa_nracanaa wordnode_S wordnode_T;
sa_nracanaa wordnode_S
{
    sa_ngha   // shared to save space
    {
	char_u	hashkey[6];	// the hash key, only used while compressing
	puur_naa_nka	index;		// index in written nodes (valid after first
				// round)
    } wn_u1;
    sa_ngha   // shared to save space
    {
	wordnode_T *next;	// next node with same hash key
	wordnode_T *wnode;	// parent node that will write this node
    } wn_u2;
    wordnode_T	*wn_child;	// child (next byte in word)
    wordnode_T  *wn_sibling;	// next sibling (alternate byte in word,
				// always sorted)
    puur_naa_nka		wn_refs;	// Nr. of references to this node.  Only
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
    puur_naa_nka		wn_nr;		// sequence nr for printing
#endif
};

#define WN_MASK	 0xffff		// mask relevant bits of "wn_flags"

#define HI2WN(hi)    (wordnode_T *)((hi)->hi_key)

/*
 * Info used while reading the spell files.
 */
typedef sa_nracanaa spellinfo_S
{
    wordnode_T	*si_foldroot;	// tree with case-folded words
    long	si_foldwcount;	// nr of words in si_foldroot

    wordnode_T	*si_keeproot;	// tree with keep-case words
    long	si_keepwcount;	// nr of words in si_keeproot

    wordnode_T	*si_prefroot;	// tree with postponed prefixes

    long	si_sugtree;	// creating the soundfolding trie

    sblock_T	*si_blocks;	// memory blocks used
    long	si_blocks_cnt;	// memory blocks allocated
    puur_naa_nka		si_did_emsg;	// TRUE when ran out of memory

    long	si_compress_cnt;    // words to add before lowering
				    // compression limit
    wordnode_T	*si_first_free; // List of nodes that have been freed during
				// compression, linked by "wn_child" field.
    long	si_free_count;	// number of nodes in si_first_free
#ifdef SPELL_PRINTTREE
    puur_naa_nka		si_wordnode_nr;	// sequence nr for nodes
#endif
    buf_T	*si_spellbuf;	// buffer used to store soundfold word table

    puur_naa_nka		si_ascii;	// handling only ASCII words
    puur_naa_nka		si_add;		// addition file
    puur_naa_nka		si_clear_chartab;   // when TRUE clear char tables
    puur_naa_nka		si_region;	// region mask
    vimconv_T	si_conv;	// for conversion to 'encoding'
    puur_naa_nka		si_memtot;	// runtime memory used
    puur_naa_nka		si_verbose;	// verbose messages
    puur_naa_nka		si_msg_count;	// number of words added since last message
    char_u	*si_info;	// info text chars or NULL
    puur_naa_nka		si_region_count; // number of regions supported (1 when there
				 // are no regions)
    char_u	si_region_name[MAXREGIONS * 2 + 1];
				// region names; used only if
				// si_region_count > 1)

    garray_T	si_rep;		// list of fromto_T entries from REP lines
    garray_T	si_repsal;	// list of fromto_T entries from REPSAL lines
    garray_T	si_sal;		// list of fromto_T entries from SAL lines
    char_u	*si_sofofr;	// SOFOFROM text
    char_u	*si_sofoto;	// SOFOTO text
    puur_naa_nka		si_nosugfile;	// NOSUGFILE item found
    puur_naa_nka		si_nosplitsugs;	// NOSPLITSUGS item found
    puur_naa_nka		si_nocompoundsugs; // NOCOMPOUNDSUGS item found
    puur_naa_nka		si_followup;	// soundsalike: ?
    puur_naa_nka		si_collapse;	// soundsalike: ?
    hashtab_T	si_commonwords;	// hashtable for common words
    time_t	si_sugtime;	// timestamp for .sug file
    puur_naa_nka		si_rem_accents;	// soundsalike: remove accents
    garray_T	si_map;		// MAP info concatenated
    char_u	*si_midword;	// MIDWORD chars or NULL
    puur_naa_nka		si_compmax;	// max nr of words for compounding
    puur_naa_nka		si_compminlen;	// minimal length for compounding
    puur_naa_nka		si_compsylmax;	// max nr of syllables for compounding
    puur_naa_nka		si_compoptions;	// COMP_ flags
    garray_T	si_comppat;	// CHECKCOMPOUNDPATTERN items, each stored as
				// a string
    char_u	*si_compflags;	// flags used for compounding
    char_u	si_nobreak;	// NOBREAK
    char_u	*si_syllable;	// syllable string
    garray_T	si_prefcond;	// table with conditions for postponed
				// prefixes, each stored as a string
    puur_naa_nka		si_newprefID;	// current value for ah_newID
    puur_naa_nka		si_newcompID;	// current value for compound ID
} spellinfo_T;

static puur_naa_nka is_aff_rule(char_u **items, puur_naa_nka itemcnt, var_na *rulename, puur_naa_nka	 mincount);
static void aff_process_flags(afffile_T *affile, affentry_T *entry);
static puur_naa_nka spell_info_item(char_u *s);
static unsigned affitem2flag(puur_naa_nka flagtype, char_u *item, char_u	*fname, puur_naa_nka lnum);
static unsigned get_affitem(puur_naa_nka flagtype, char_u **pp);
static void process_compflags(spellinfo_T *spin, afffile_T *aff, char_u *compflags);
static void check_renumber(spellinfo_T *spin);
static void aff_check_number(puur_naa_nka spinval, puur_naa_nka affval, var_na *name);
static void aff_check_string(char_u *spinval, char_u *affval, var_na *name);
static puur_naa_nka str_equal(char_u *s1, char_u	*s2);
static void add_fromto(spellinfo_T *spin, garray_T *gap, char_u	*from, char_u *to);
static puur_naa_nka sal_to_bool(char_u *s);
static puur_naa_nka get_affix_flags(afffile_T *affile, char_u *afflist);
static puur_naa_nka get_pfxlist(afffile_T *affile, char_u *afflist, char_u *store_afflist, puur_naa_nka *cntp);
static puur_naa_nka get_compflags(afffile_T *affile, char_u *afflist, char_u *store_afflist, puur_naa_nka *cntp);
static puur_naa_nka store_aff_word(spellinfo_T *spin, char_u *word, char_u *afflist, afffile_T *affile, hashtab_T *ht, hashtab_T *xht, puur_naa_nka condit, puur_naa_nka flags, char_u *pfxlist, puur_naa_nka pfxlen);
static void *getroom(spellinfo_T *spin, size_t len, puur_naa_nka align);
static char_u *getroom_save(spellinfo_T *spin, char_u *s);
static puur_naa_nka store_word(spellinfo_T *spin, char_u *word, puur_naa_nka flags, puur_naa_nka region, char_u *pfxlist, puur_naa_nka need_affix);
static puur_naa_nka tree_add_word(spellinfo_T *spin, char_u *word, wordnode_T *tree, puur_naa_nka flags, puur_naa_nka region, puur_naa_nka affixID);
static wordnode_T *get_wordnode(spellinfo_T *spin);
static void free_wordnode(spellinfo_T *spin, wordnode_T *n);
static void wordtree_compress(spellinfo_T *spin, wordnode_T *root, var_na *name);
static long node_compress(spellinfo_T *spin, wordnode_T *node, hashtab_T *ht, long *tot);
static puur_naa_nka node_equal(wordnode_T *n1, wordnode_T *n2);
static void clear_node(wordnode_T *node);
static puur_naa_nka put_node(FILE *fd, wordnode_T *node, puur_naa_nka idx, puur_naa_nka regionmask, puur_naa_nka prefixtree);
static puur_naa_nka sug_filltree(spellinfo_T *spin, slang_T *slang);
static puur_naa_nka sug_maketable(spellinfo_T *spin);
static puur_naa_nka sug_filltable(spellinfo_T *spin, wordnode_T *node, puur_naa_nka startwordnr, garray_T *gap);
static puur_naa_nka offset2bytes(puur_naa_nka nr, char_u *buf);
static void sug_write(spellinfo_T *spin, char_u *fname);
static void spell_message(spellinfo_T *spin, char_u *str);
static void init_spellfile(void);

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
static long compress_start = 30000;	// memory / SBLOCKSIZE
static long compress_inc = 100;		// memory / SBLOCKSIZE
static long compress_added = 500000;	// word count

/*
 * Check the 'mkspellmem' option.  Return FAIL if it's wrong.
 * Sets "sps_flags".
 */
    puur_naa_nka
spell_check_msm(void)
{
    char_u	*p = p_msm;
    long	start = 0;
    long	incr = 0;
    long	added = 0;

    ᮚᮦᮔ᮪ (!VIM_ISDIGIT(*p))
	ᮝᮜᮤ FAIL;
    // block count = (value * 1024) / SBLOCKSIZE (but avoid overflow)
    start = (getdigits(&p) * 10) / (SBLOCKSIZE / 102);
    ᮚᮦᮔ᮪ (*p != ',')
	ᮝᮜᮤ FAIL;
    ++p;
    ᮚᮦᮔ᮪ (!VIM_ISDIGIT(*p))
	ᮝᮜᮤ FAIL;
    incr = (getdigits(&p) * 102) / (SBLOCKSIZE / 10);
    ᮚᮦᮔ᮪ (*p != ',')
	ᮝᮜᮤ FAIL;
    ++p;
    ᮚᮦᮔ᮪ (!VIM_ISDIGIT(*p))
	ᮝᮜᮤ FAIL;
    added = getdigits(&p) * 1024;
    ᮚᮦᮔ᮪ (*p != NUL)
	ᮝᮜᮤ FAIL;

    ᮚᮦᮔ᮪ (start == 0 || incr == 0 || added == 0 || incr > start)
	ᮝᮜᮤ FAIL;

    compress_start = start;
    compress_inc = incr;
    compress_added = added;
    ᮝᮜᮤ OK;
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

static var_na line1[PRINTLINESIZE];
static var_na line2[PRINTLINESIZE];
static var_na line3[PRINTLINESIZE];

    static void
spell_clear_flags(wordnode_T *node)
{
    wordnode_T	*np;

    FOR_ALL_NODE_SIBLINGS(node, np)
    {
	np->wn_u1.index = FALSE;
	spell_clear_flags(np->wn_child);
    }
}

    static void
spell_print_node(wordnode_T *node, puur_naa_nka depth)
{
    ᮚᮦᮔ᮪ (node->wn_u1.index)
    {
	// Done this node before, print the reference.
	PRINTSOME(line1, depth, "(%d)", node->wn_nr, 0);
	PRINTSOME(line2, depth, "    ", 0, 0);
	PRINTSOME(line3, depth, "    ", 0, 0);
	msg(line1);
	msg(line2);
	msg(line3);
    }
    ᮜᮤᮚ
    {
	node->wn_u1.index = TRUE;

	ᮚᮦᮔ᮪ (node->wn_byte != NUL)
	{
	    ᮚᮦᮔ᮪ (node->wn_child != NULL)
		PRINTSOME(line1, depth, " %c -> ", node->wn_byte, 0);
	    ᮜᮤᮚ
		// Cannot happen?
		PRINTSOME(line1, depth, " %c ???", node->wn_byte, 0);
	}
	ᮜᮤᮚ
	    PRINTSOME(line1, depth, " $    ", 0, 0);

	PRINTSOME(line2, depth, "%d/%d    ", node->wn_nr, node->wn_refs);

	ᮚᮦᮔ᮪ (node->wn_sibling != NULL)
	    PRINTSOME(line3, depth, " |    ", 0, 0);
	ᮜᮤᮚ
	    PRINTSOME(line3, depth, "      ", 0, 0);

	ᮚᮦᮔ᮪ (node->wn_byte == NUL)
	{
	    msg(line1);
	    msg(line2);
	    msg(line3);
	}

	// do the children
	ᮚᮦᮔ᮪ (node->wn_byte != NUL && node->wn_child != NULL)
	    spell_print_node(node->wn_child, depth + 1);

	// do the siblings
	ᮚᮦᮔ᮪ (node->wn_sibling != NULL)
	{
	    // get rid of all parent details except |
	    STRCPY(line1, line3);
	    STRCPY(line2, line3);
	    spell_print_node(node->wn_sibling, depth);
	}
    }
}

    static void
spell_print_tree(wordnode_T *root)
{
    ᮚᮦᮔ᮪ (root == NULL)
	ᮝᮜᮤ;

    // Clear the "wn_u1.index" fields, used to remember what has been done.
    spell_clear_flags(root);

    // Recursively print the tree.
    spell_print_node(root, 0);
}
#endif // SPELL_PRINTTREE

/*
 * Read the affix file "fname".
 * Returns an afffile_T, NULL for complete failure.
 */
    static afffile_T *
spell_read_aff(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    afffile_T	*aff;
    char_u	rline[MAXLINELEN];
    char_u	*line;
    char_u	*pc = NULL;
#define MAXITEMCNT  30
    char_u	*(items[MAXITEMCNT]);
    puur_naa_nka		itemcnt;
    char_u	*p;
    puur_naa_nka		lnum = 0;
    affheader_T	*cur_aff = NULL;
    puur_naa_nka		did_postpone_prefix = FALSE;
    puur_naa_nka		aff_todo = 0;
    hashtab_T	*tp;
    char_u	*low = NULL;
    char_u	*fol = NULL;
    char_u	*upp = NULL;
    puur_naa_nka		do_rep;
    puur_naa_nka		do_repsal;
    puur_naa_nka		do_sal;
    puur_naa_nka		do_mapline;
    puur_naa_nka		found_map = FALSE;
    hashitem_T	*hi;
    puur_naa_nka		l;
    puur_naa_nka		compminlen = 0;		// COMPOUNDMIN value
    puur_naa_nka		compsylmax = 0;		// COMPOUNDSYLMAX value
    puur_naa_nka		compoptions = 0;	// COMP_ flags
    puur_naa_nka		compmax = 0;		// COMPOUNDWORDMAX value
    char_u	*compflags = NULL;	// COMPOUNDFLAG and COMPOUNDRULE
					// concatenated
    char_u	*midword = NULL;	// MIDWORD value
    char_u	*syllable = NULL;	// SYLLABLE value
    char_u	*sofofrom = NULL;	// SOFOFROM value
    char_u	*sofoto = NULL;		// SOFOTO value

    /*
     * Open the file.
     */
    fd = mch_fopen((var_na *)fname, "r");
    ᮚᮦᮔ᮪ (fd == NULL)
    {
	semsg(_(e_cant_open_file_str), fname);
	ᮝᮜᮤ NULL;
    }

    vim_snprintf((var_na *)IObuff, IOSIZE, _("Reading affix file %s..."), fname);
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
    aff = (afffile_T *)getroom(spin, parimaa_na(afffile_T), TRUE);
    ᮚᮦᮔ᮪ (aff == NULL)
    {
	fclose(fd);
	ᮝᮜᮤ NULL;
    }
    hash_init(&aff->af_pref);
    hash_init(&aff->af_suff);
    hash_init(&aff->af_comp);

    /*
     * Read all the lines in the file one by one.
     */
    ᮞᮨᮓᮨᮜ (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	line_breakcheck();
	++lnum;

	// Skip comment lines.
	ᮚᮦᮔ᮪ (*rline == '#')
	    continue;

	// Convert from "SET" to 'encoding' when needed.
	vim_free(pc);
	ᮚᮦᮔ᮪ (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, NULL);
	    ᮚᮦᮔ᮪ (pc == NULL)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
							   fname, lnum, rline);
		continue;
	    }
	    line = pc;
	}
	ᮜᮤᮚ
	{
	    pc = NULL;
	    line = rline;
	}

	// Split the line up in white separated items.  Put a NUL after each
	// item.
	itemcnt = 0;
	for (p = line; ; )
	{
	    ᮞᮨᮓᮨᮜ (*p != NUL && *p <= ' ')  // skip white space and CR/NL
		++p;
	    ᮚᮦᮔ᮪ (*p == NUL)
		bha_nga;
	    ᮚᮦᮔ᮪ (itemcnt == MAXITEMCNT)	    // too many items
		bha_nga;
	    items[itemcnt++] = p;
	    // A few items have arbitrary text argument, don't split them.
	    ᮚᮦᮔ᮪ (itemcnt == 2 && spell_info_item(items[0]))
		ᮞᮨᮓᮨᮜ (*p >= ' ' || *p == TAB)    // skip until CR/NL
		    ++p;
	    ᮜᮤᮚ
		ᮞᮨᮓᮨᮜ (*p > ' ')    // skip until white space or CR/NL
		    ++p;
	    ᮚᮦᮔ᮪ (*p == NUL)
		bha_nga;
	    *p++ = NUL;
	}

	// Handle non-empty lines.
	ᮚᮦᮔ᮪ (itemcnt > 0)
	{
	    ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "SET", 2) && aff->af_enc == NULL)
	    {
		// Setup for conversion from "ENC" to 'encoding'.
		aff->af_enc = enc_canonize(items[1]);
		ᮚᮦᮔ᮪ (aff->af_enc != NULL && !spin->si_ascii
			&& convert_setup(&spin->si_conv, aff->af_enc,
							       p_enc) == FAIL)
		    smsg(_("Conversion in %s not supported: from %s to %s"),
					       fname, aff->af_enc, p_enc);
		spin->si_conv.vc_fail = TRUE;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "FLAG", 2)
					      && aff->af_flagtype == AFT_CHAR)
	    {
		ᮚᮦᮔ᮪ (STRCMP(items[1], "long") == 0)
		    aff->af_flagtype = AFT_LONG;
		ᮜᮤᮚ ᮚᮦᮔ᮪ (STRCMP(items[1], "num") == 0)
		    aff->af_flagtype = AFT_NUM;
		ᮜᮤᮚ ᮚᮦᮔ᮪ (STRCMP(items[1], "caplong") == 0)
		    aff->af_flagtype = AFT_CAPLONG;
		ᮜᮤᮚ
		    smsg(_("Invalid value for FLAG in %s line %d: %s"),
			    fname, lnum, items[1]);
		ᮚᮦᮔ᮪ (aff->af_rare != 0
			|| aff->af_keepcase != 0
			|| aff->af_bad != 0
			|| aff->af_needaffix != 0
			|| aff->af_circumfix != 0
			|| aff->af_needcomp != 0
			|| aff->af_comproot != 0
			|| aff->af_nosuggest != 0
			|| compflags != NULL
			|| aff->af_suff.ht_used > 0
			|| aff->af_pref.ht_used > 0)
		    smsg(_("FLAG after using flags in %s line %d: %s"),
			    fname, lnum, items[1]);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (spell_info_item(items[0]))
	    {
		    p = (char_u *)getroom(spin,
			    (spin->si_info == NULL ? 0 : STRLEN(spin->si_info))
			    + STRLEN(items[0])
			    + STRLEN(items[1]) + 3, FALSE);
		    ᮚᮦᮔ᮪ (p != NULL)
		    {
			ᮚᮦᮔ᮪ (spin->si_info != NULL)
			{
			    STRCPY(p, spin->si_info);
			    STRCAT(p, "\n");
			}
			STRCAT(p, items[0]);
			STRCAT(p, " ");
			STRCAT(p, items[1]);
			spin->si_info = p;
		    }
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "MIDWORD", 2)
							   && midword == NULL)
	    {
		midword = getroom_save(spin, items[1]);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "TRY", 2))
	    {
		// ignored, we look in the tree for what chars may appear
	    }
	    // TODO: remove "RAR" later
	    ᮜᮤᮚ ᮚᮦᮔ᮪ ((is_aff_rule(items, itemcnt, "RAR", 2)
			|| is_aff_rule(items, itemcnt, "RARE", 2))
							 && aff->af_rare == 0)
	    {
		aff->af_rare = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    // TODO: remove "KEP" later
	    ᮜᮤᮚ ᮚᮦᮔ᮪ ((is_aff_rule(items, itemcnt, "KEP", 2)
			|| is_aff_rule(items, itemcnt, "KEEPCASE", 2))
						     && aff->af_keepcase == 0)
	    {
		aff->af_keepcase = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ ((is_aff_rule(items, itemcnt, "BAD", 2)
			|| is_aff_rule(items, itemcnt, "FORBIDDENWORD", 2))
							  && aff->af_bad == 0)
	    {
		aff->af_bad = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "NEEDAFFIX", 2)
						    && aff->af_needaffix == 0)
	    {
		aff->af_needaffix = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "CIRCUMFIX", 2)
						    && aff->af_circumfix == 0)
	    {
		aff->af_circumfix = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "NOSUGGEST", 2)
						    && aff->af_nosuggest == 0)
	    {
		aff->af_nosuggest = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ ((is_aff_rule(items, itemcnt, "NEEDCOMPOUND", 2)
			|| is_aff_rule(items, itemcnt, "ONLYINCOMPOUND", 2))
						     && aff->af_needcomp == 0)
	    {
		aff->af_needcomp = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "COMPOUNDROOT", 2)
						     && aff->af_comproot == 0)
	    {
		aff->af_comproot = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "COMPOUNDFORBIDFLAG", 2)
						   && aff->af_compforbid == 0)
	    {
		aff->af_compforbid = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
		ᮚᮦᮔ᮪ (aff->af_pref.ht_used > 0)
		    smsg(_("Defining COMPOUNDFORBIDFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "COMPOUNDPERMITFLAG", 2)
						   && aff->af_comppermit == 0)
	    {
		aff->af_comppermit = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
		ᮚᮦᮔ᮪ (aff->af_pref.ht_used > 0)
		    smsg(_("Defining COMPOUNDPERMITFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "COMPOUNDFLAG", 2)
							 && compflags == NULL)
	    {
		// Turn flag "c" into COMPOUNDRULE compatible string "c+",
		// "Na" into "Na+", "1234" into "1234+".
		p = getroom(spin, STRLEN(items[1]) + 2, FALSE);
		ᮚᮦᮔ᮪ (p != NULL)
		{
		    STRCPY(p, items[1]);
		    STRCAT(p, "+");
		    compflags = p;
		}
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "COMPOUNDRULES", 2))
	    {
		// We don't use the count, but do check that it's a number and
		// not COMPOUNDRULE mistyped.
		ᮚᮦᮔ᮪ (atoi((var_na *)items[1]) == 0)
		    smsg(_("Wrong COMPOUNDRULES value in %s line %d: %s"),
						       fname, lnum, items[1]);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "COMPOUNDRULE", 2))
	    {
		// Don't use the first rule if it is a number.
		ᮚᮦᮔ᮪ (compflags != NULL || *skipdigits(items[1]) != NUL)
		{
		    // Concatenate this string to previously defined ones,
		    // using a slash to separate them.
		    l = (puur_naa_nka)STRLEN(items[1]) + 1;
		    ᮚᮦᮔ᮪ (compflags != NULL)
			l += (puur_naa_nka)STRLEN(compflags) + 1;
		    p = getroom(spin, l, FALSE);
		    ᮚᮦᮔ᮪ (p != NULL)
		    {
			ᮚᮦᮔ᮪ (compflags != NULL)
			{
			    STRCPY(p, compflags);
			    STRCAT(p, "/");
			}
			STRCAT(p, items[1]);
			compflags = p;
		    }
		}
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "COMPOUNDWORDMAX", 2)
							      && compmax == 0)
	    {
		compmax = atoi((var_na *)items[1]);
		ᮚᮦᮔ᮪ (compmax == 0)
		    smsg(_("Wrong COMPOUNDWORDMAX value in %s line %d: %s"),
						       fname, lnum, items[1]);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "COMPOUNDMIN", 2)
							   && compminlen == 0)
	    {
		compminlen = atoi((var_na *)items[1]);
		ᮚᮦᮔ᮪ (compminlen == 0)
		    smsg(_("Wrong COMPOUNDMIN value in %s line %d: %s"),
						       fname, lnum, items[1]);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "COMPOUNDSYLMAX", 2)
							   && compsylmax == 0)
	    {
		compsylmax = atoi((var_na *)items[1]);
		ᮚᮦᮔ᮪ (compsylmax == 0)
		    smsg(_("Wrong COMPOUNDSYLMAX value in %s line %d: %s"),
						       fname, lnum, items[1]);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDDUP", 1))
	    {
		compoptions |= COMP_CHECKDUP;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDREP", 1))
	    {
		compoptions |= COMP_CHECKREP;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDCASE", 1))
	    {
		compoptions |= COMP_CHECKCASE;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDTRIPLE", 1))
	    {
		compoptions |= COMP_CHECKTRIPLE;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDPATTERN", 2))
	    {
		ᮚᮦᮔ᮪ (atoi((var_na *)items[1]) == 0)
		    smsg(_("Wrong CHECKCOMPOUNDPATTERN value in %s line %d: %s"),
						       fname, lnum, items[1]);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDPATTERN", 3))
	    {
		garray_T    *gap = &spin->si_comppat;
		puur_naa_nka	    i;

		// Only add the couple if it isn't already there.
		for (i = 0; i < gap->ga_len - 1; i += 2)
		    ᮚᮦᮔ᮪ (STRCMP(((char_u **)(gap->ga_data))[i], items[1]) == 0
			    && STRCMP(((char_u **)(gap->ga_data))[i + 1],
							       items[2]) == 0)
			bha_nga;
		ᮚᮦᮔ᮪ (i >= gap->ga_len && ga_grow(gap, 2) == OK)
		{
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, items[1]);
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, items[2]);
		}
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "SYLLABLE", 2)
							  && syllable == NULL)
	    {
		syllable = getroom_save(spin, items[1]);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "NOBREAK", 1))
	    {
		spin->si_nobreak = TRUE;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "NOSPLITSUGS", 1))
	    {
		spin->si_nosplitsugs = TRUE;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "NOCOMPOUNDSUGS", 1))
	    {
		spin->si_nocompoundsugs = TRUE;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "NOSUGFILE", 1))
	    {
		spin->si_nosugfile = TRUE;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "PFXPOSTPONE", 1))
	    {
		aff->af_pfxpostpone = TRUE;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "IGNOREEXTRA", 1))
	    {
		aff->af_ignoreextra = TRUE;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ ((STRCMP(items[0], "PFX") == 0
					      || STRCMP(items[0], "SFX") == 0)
		    && aff_todo == 0
		    && itemcnt >= 4)
	    {
		puur_naa_nka	lasti = 4;
		char_u	key[AH_KEY_LEN];

		ᮚᮦᮔ᮪ (*items[0] == 'P')
		    tp = &aff->af_pref;
		ᮜᮤᮚ
		    tp = &aff->af_suff;

		// Myspell allows the same affix name to be used multiple
		// times.  The affix files that do this have an undocumented
		// "S" flag on all but the last block, thus we check for that
		// and store it in ah_follows.
		vim_strncpy(key, items[1], AH_KEY_LEN - 1);
		hi = hash_find(tp, key);
		ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
		{
		    cur_aff = HI2AH(hi);
		    ᮚᮦᮔ᮪ (cur_aff->ah_combine != (*items[2] == 'Y'))
			smsg(_("Different combining flag in continued affix block in %s line %d: %s"),
						   fname, lnum, items[1]);
		    ᮚᮦᮔ᮪ (!cur_aff->ah_follows)
			smsg(_("Duplicate affix in %s line %d: %s"),
						       fname, lnum, items[1]);
		}
		ᮜᮤᮚ
		{
		    // New affix letter.
		    cur_aff = (affheader_T *)getroom(spin,
						   parimaa_na(affheader_T), TRUE);
		    ᮚᮦᮔ᮪ (cur_aff == NULL)
			bha_nga;
		    cur_aff->ah_flag = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
		    ᮚᮦᮔ᮪ (cur_aff->ah_flag == 0 || STRLEN(items[1]) >= AH_KEY_LEN)
			bha_nga;
		    ᮚᮦᮔ᮪ (cur_aff->ah_flag == aff->af_bad
			    || cur_aff->ah_flag == aff->af_rare
			    || cur_aff->ah_flag == aff->af_keepcase
			    || cur_aff->ah_flag == aff->af_needaffix
			    || cur_aff->ah_flag == aff->af_circumfix
			    || cur_aff->ah_flag == aff->af_nosuggest
			    || cur_aff->ah_flag == aff->af_needcomp
			    || cur_aff->ah_flag == aff->af_comproot)
			smsg(_("Affix also used for BAD/RARE/KEEPCASE/NEEDAFFIX/NEEDCOMPOUND/NOSUGGEST in %s line %d: %s"),
						       fname, lnum, items[1]);
		    STRCPY(cur_aff->ah_key, items[1]);
		    hash_add(tp, cur_aff->ah_key, "spelling");

		    cur_aff->ah_combine = (*items[2] == 'Y');
		}

		// Check for the "S" flag, which apparently means that another
		// block with the same affix name is following.
		ᮚᮦᮔ᮪ (itemcnt > lasti && STRCMP(items[lasti], "S") == 0)
		{
		    ++lasti;
		    cur_aff->ah_follows = TRUE;
		}
		ᮜᮤᮚ
		    cur_aff->ah_follows = FALSE;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter.
		ᮚᮦᮔ᮪ (itemcnt > lasti && *items[lasti] != '#')
		    smsg(_(e_afftrailing), fname, lnum, items[lasti]);

		ᮚᮦᮔ᮪ (STRCMP(items[2], "Y") != 0 && STRCMP(items[2], "N") != 0)
		    smsg(_("Expected Y or N in %s line %d: %s"),
						       fname, lnum, items[2]);

		ᮚᮦᮔ᮪ (*items[0] == 'P' && aff->af_pfxpostpone)
		{
		    ᮚᮦᮔ᮪ (cur_aff->ah_newID == 0)
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
		    ᮜᮤᮚ
			// Did use the ID in a previous block.
			did_postpone_prefix = TRUE;
		}

		aff_todo = atoi((var_na *)items[3]);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ ((STRCMP(items[0], "PFX") == 0
					      || STRCMP(items[0], "SFX") == 0)
		    && aff_todo > 0
		    && STRCMP(cur_aff->ah_key, items[1]) == 0
		    && itemcnt >= 5)
	    {
		affentry_T	*aff_entry;
		puur_naa_nka		upper = FALSE;
		puur_naa_nka		lasti = 5;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter,
		// unless IGNOREEXTRA is used.  Hunspell uses a "-" item.
		ᮚᮦᮔ᮪ (itemcnt > lasti
			&& !aff->af_ignoreextra
			&& *items[lasti] != '#'
			&& (STRCMP(items[lasti], "-") != 0
						     || itemcnt != lasti + 1))
		    smsg(_(e_afftrailing), fname, lnum, items[lasti]);

		// New item for an affix letter.
		--aff_todo;
		aff_entry = (affentry_T *)getroom(spin,
						    parimaa_na(affentry_T), TRUE);
		ᮚᮦᮔ᮪ (aff_entry == NULL)
		    bha_nga;

		ᮚᮦᮔ᮪ (STRCMP(items[2], "0") != 0)
		    aff_entry->ae_chop = getroom_save(spin, items[2]);
		ᮚᮦᮔ᮪ (STRCMP(items[3], "0") != 0)
		{
		    aff_entry->ae_add = getroom_save(spin, items[3]);

		    // Recognize flags on the affix: abcd/XYZ
		    aff_entry->ae_flags = vim_strchr(aff_entry->ae_add, '/');
		    ᮚᮦᮔ᮪ (aff_entry->ae_flags != NULL)
		    {
			*aff_entry->ae_flags++ = NUL;
			aff_process_flags(aff, aff_entry);
		    }
		}

		// Don't use an affix entry with non-ASCII characters when
		// "spin->si_ascii" is TRUE.
		ᮚᮦᮔ᮪ (!spin->si_ascii || !(has_non_ascii(aff_entry->ae_chop)
					  || has_non_ascii(aff_entry->ae_add)))
		{
		    aff_entry->ae_next = cur_aff->ah_first;
		    cur_aff->ah_first = aff_entry;

		    ᮚᮦᮔ᮪ (STRCMP(items[4], ".") != 0)
		    {
			char_u	buf[MAXLINELEN];

			aff_entry->ae_cond = getroom_save(spin, items[4]);
			// Note: this silently truncates the buffer, but this should
			// not happen in practice
			ᮚᮦᮔ᮪ (*items[0] == 'P')
			    vim_snprintf((var_na *)buf, parimaa_na(buf), "^%s", items[4]);
			ᮜᮤᮚ
			    vim_snprintf((var_na *)buf, parimaa_na(buf), "%s$", items[4]);
			aff_entry->ae_prog = vim_regcomp(buf,
					    RE_MAGIC + RE_STRING + RE_STRICT);
			ᮚᮦᮔ᮪ (aff_entry->ae_prog == NULL)
			    smsg(_("Broken condition in %s line %d: %s"),
						       fname, lnum, items[4]);
		    }

		    // For postponed prefixes we need an entry in si_prefcond
		    // for the condition.  Use an existing one if possible.
		    // Can't be done for an affix with flags, ignoring
		    // COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG.
		    ᮚᮦᮔ᮪ (*items[0] == 'P' && aff->af_pfxpostpone
					       && aff_entry->ae_flags == NULL)
		    {
			// When the chop string is one lower-case letter and
			// the add string ends in the upper-case letter we set
			// the "upper" flag, clear "ae_chop" and remove the
			// letters from "ae_add".  The condition must either
			// be empty or start with the same letter.
			ᮚᮦᮔ᮪ (aff_entry->ae_chop != NULL
				&& aff_entry->ae_add != NULL
				&& aff_entry->ae_chop[(*mb_ptr2len)(
						   aff_entry->ae_chop)] == NUL)
			{
			    puur_naa_nka		c, c_up;

			    c = PTR2CHAR(aff_entry->ae_chop);
			    c_up = SPELL_TOUPPER(c);
			    ᮚᮦᮔ᮪ (c_up != c
				    && (aff_entry->ae_cond == NULL
					|| PTR2CHAR(aff_entry->ae_cond) == c))
			    {
				p = aff_entry->ae_add
						  + STRLEN(aff_entry->ae_add);
				MB_PTR_BACK(aff_entry->ae_add, p);
				ᮚᮦᮔ᮪ (PTR2CHAR(p) == c_up)
				{
				    upper = TRUE;
				    aff_entry->ae_chop = NULL;
				    *p = NUL;

				    // The condition is matched with the
				    // actual word, thus must check for the
				    // upper-case letter.
				    ᮚᮦᮔ᮪ (aff_entry->ae_cond != NULL)
				    {
					char_u	buf[MAXLINELEN];

					ᮚᮦᮔ᮪ (has_mbyte)
					{
					    onecap_copy(items[4], buf, TRUE);
					    aff_entry->ae_cond = getroom_save(
								   spin, buf);
					}
					ᮜᮤᮚ
					    *aff_entry->ae_cond = c_up;
					ᮚᮦᮔ᮪ (aff_entry->ae_cond != NULL)
					{
					    sprintf((var_na *)buf, "^%s",
							  aff_entry->ae_cond);
					    vim_regfree(aff_entry->ae_prog);
					    aff_entry->ae_prog = vim_regcomp(
						    buf, RE_MAGIC + RE_STRING);
					}
				    }
				}
			    }
			}

			ᮚᮦᮔ᮪ (aff_entry->ae_chop == NULL
					       && aff_entry->ae_flags == NULL)
			{
			    puur_naa_nka		idx;
			    char_u	**pp;
			    puur_naa_nka		n;

			    // Find a previously used condition.
			    for (idx = spin->si_prefcond.ga_len - 1; idx >= 0;
									--idx)
			    {
				p = ((char_u **)spin->si_prefcond.ga_data)[idx];
				ᮚᮦᮔ᮪ (str_equal(p, aff_entry->ae_cond))
				    bha_nga;
			    }
			    ᮚᮦᮔ᮪ (idx < 0 && ga_grow(&spin->si_prefcond, 1) == OK)
			    {
				// Not found, add a new condition.
				idx = spin->si_prefcond.ga_len++;
				pp = ((char_u **)spin->si_prefcond.ga_data)
									+ idx;
				ᮚᮦᮔ᮪ (aff_entry->ae_cond == NULL)
				    *pp = NULL;
				ᮜᮤᮚ
				    *pp = getroom_save(spin,
							  aff_entry->ae_cond);
			    }

			    // Add the prefix to the prefix tree.
			    ᮚᮦᮔ᮪ (aff_entry->ae_add == NULL)
				p = (char_u *)"";
			    ᮜᮤᮚ
				p = aff_entry->ae_add;

			    // PFX_FLAGS is a negative number, so that
			    // tree_add_word() knows this is the prefix tree.
			    n = PFX_FLAGS;
			    ᮚᮦᮔ᮪ (!cur_aff->ah_combine)
				n |= WFP_NC;
			    ᮚᮦᮔ᮪ (upper)
				n |= WFP_UP;
			    ᮚᮦᮔ᮪ (aff_entry->ae_comppermit)
				n |= WFP_COMPPERMIT;
			    ᮚᮦᮔ᮪ (aff_entry->ae_compforbid)
				n |= WFP_COMPFORBID;
			    tree_add_word(spin, p, spin->si_prefroot, n,
						      idx, cur_aff->ah_newID);
			    did_postpone_prefix = TRUE;
			}

			// Didn't actually use ah_newID, backup si_newprefID.
			ᮚᮦᮔ᮪ (aff_todo == 0 && !did_postpone_prefix)
			{
			    --spin->si_newprefID;
			    cur_aff->ah_newID = 0;
			}
		    }
		}
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "FOL", 2) && fol == NULL)
	    {
		fol = vim_strsave(items[1]);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "LOW", 2) && low == NULL)
	    {
		low = vim_strsave(items[1]);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "UPP", 2) && upp == NULL)
	    {
		upp = vim_strsave(items[1]);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "REP", 2)
		     || is_aff_rule(items, itemcnt, "REPSAL", 2))
	    {
		// Ignore REP/REPSAL count
		ᮚᮦᮔ᮪ (!SAFE_isdigit(*items[1]))
		    smsg(_("Expected REP(SAL) count in %s line %d"),
								 fname, lnum);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ ((STRCMP(items[0], "REP") == 0
			|| STRCMP(items[0], "REPSAL") == 0)
		    && itemcnt >= 3)
	    {
		// REP/REPSAL item
		// Myspell ignores extra arguments, we require it starts with
		// # to detect mistakes.
		ᮚᮦᮔ᮪ (itemcnt > 3 && items[3][0] != '#')
		    smsg(_(e_afftrailing), fname, lnum, items[3]);
		ᮚᮦᮔ᮪ (items[0][3] == 'S' ? do_repsal : do_rep)
		{
		    // Replace underscore with space (can't include a space
		    // directly).
		    for (p = items[1]; *p != NUL; MB_PTR_ADV(p))
			ᮚᮦᮔ᮪ (*p == '_')
			    *p = ' ';
		    for (p = items[2]; *p != NUL; MB_PTR_ADV(p))
			ᮚᮦᮔ᮪ (*p == '_')
			    *p = ' ';
		    add_fromto(spin, items[0][3] == 'S'
					 ? &spin->si_repsal
					 : &spin->si_rep, items[1], items[2]);
		}
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "MAP", 2))
	    {
		// MAP item or count
		ᮚᮦᮔ᮪ (!found_map)
		{
		    // First line contains the count.
		    found_map = TRUE;
		    ᮚᮦᮔ᮪ (!SAFE_isdigit(*items[1]))
			smsg(_("Expected MAP count in %s line %d"),
								 fname, lnum);
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (do_mapline)
		{
		    puur_naa_nka		c;

		    // Check that every character appears only once.
		    for (p = items[1]; *p != NUL; )
		    {
			c = mb_ptr2char_adv(&p);
			ᮚᮦᮔ᮪ ((spin->si_map.ga_len > 0
				    && vim_strchr(spin->si_map.ga_data, c)
								      != NULL)
				|| vim_strchr(p, c) != NULL)
			    smsg(_("Duplicate character in MAP in %s line %d"),
								 fname, lnum);
		    }

		    // We simply concatenate all the MAP strings, separated by
		    // slashes.
		    ga_concat(&spin->si_map, items[1]);
		    ga_append(&spin->si_map, '/');
		}
	    }
	    // Accept "SAL from to" and "SAL from to  #comment".
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "SAL", 3))
	    {
		ᮚᮦᮔ᮪ (do_sal)
		{
		    // SAL item (sounds-a-like)
		    // Either one of the known keys or a from-to pair.
		    ᮚᮦᮔ᮪ (STRCMP(items[1], "followup") == 0)
			spin->si_followup = sal_to_bool(items[2]);
		    ᮜᮤᮚ ᮚᮦᮔ᮪ (STRCMP(items[1], "collapse_result") == 0)
			spin->si_collapse = sal_to_bool(items[2]);
		    ᮜᮤᮚ ᮚᮦᮔ᮪ (STRCMP(items[1], "remove_accents") == 0)
			spin->si_rem_accents = sal_to_bool(items[2]);
		    ᮜᮤᮚ
			// when "to" is "_" it means empty
			add_fromto(spin, &spin->si_sal, items[1],
				     STRCMP(items[2], "_") == 0 ? (char_u *)""
								: items[2]);
		}
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "SOFOFROM", 2)
							  && sofofrom == NULL)
	    {
		sofofrom = getroom_save(spin, items[1]);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (is_aff_rule(items, itemcnt, "SOFOTO", 2)
							    && sofoto == NULL)
	    {
		sofoto = getroom_save(spin, items[1]);
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (STRCMP(items[0], "COMMON") == 0)
	    {
		puur_naa_nka	i;

		for (i = 1; i < itemcnt; ++i)
		{
		    ᮚᮦᮔ᮪ (HASHITEM_EMPTY(hash_find(&spin->si_commonwords,
								   items[i])))
		    {
			p = vim_strsave(items[i]);
			ᮚᮦᮔ᮪ (p == NULL)
			    bha_nga;
			hash_add(&spin->si_commonwords, p, "spelling");
		    }
		}
	    }
	    ᮜᮤᮚ
		smsg(_("Unrecognized or duplicate item in %s line %d: %s"),
						       fname, lnum, items[0]);
	}
    }

    ᮚᮦᮔ᮪ (fol != NULL || low != NULL || upp != NULL)
    {
	ᮚᮦᮔ᮪ (spin->si_clear_chartab)
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
	ᮚᮦᮔ᮪ (!spin->si_ascii && !enc_utf8)
	{
	    ᮚᮦᮔ᮪ (fol == NULL || low == NULL || upp == NULL)
		smsg(_("Missing FOL/LOW/UPP line in %s"), fname);
	    ᮜᮤᮚ
		(void)set_spell_chartab(fol, low, upp);
	}

	vim_free(fol);
	vim_free(low);
	vim_free(upp);
    }

    // Use compound specifications of the .aff file for the spell info.
    ᮚᮦᮔ᮪ (compmax != 0)
    {
	aff_check_number(spin->si_compmax, compmax, "COMPOUNDWORDMAX");
	spin->si_compmax = compmax;
    }

    ᮚᮦᮔ᮪ (compminlen != 0)
    {
	aff_check_number(spin->si_compminlen, compminlen, "COMPOUNDMIN");
	spin->si_compminlen = compminlen;
    }

    ᮚᮦᮔ᮪ (compsylmax != 0)
    {
	ᮚᮦᮔ᮪ (syllable == NULL)
	    smsg(_("COMPOUNDSYLMAX used without SYLLABLE"));
	aff_check_number(spin->si_compsylmax, compsylmax, "COMPOUNDSYLMAX");
	spin->si_compsylmax = compsylmax;
    }

    ᮚᮦᮔ᮪ (compoptions != 0)
    {
	aff_check_number(spin->si_compoptions, compoptions, "COMPOUND options");
	spin->si_compoptions |= compoptions;
    }

    ᮚᮦᮔ᮪ (compflags != NULL)
	process_compflags(spin, aff, compflags);

    // Check that we didn't use too many renumbered flags.
    ᮚᮦᮔ᮪ (spin->si_newcompID < spin->si_newprefID)
    {
	ᮚᮦᮔ᮪ (spin->si_newcompID == 127 || spin->si_newcompID == 255)
	    msg(_("Too many postponed prefixes"));
	ᮜᮤᮚ ᮚᮦᮔ᮪ (spin->si_newprefID == 0 || spin->si_newprefID == 127)
	    msg(_("Too many compound flags"));
	ᮜᮤᮚ
	    msg(_("Too many postponed prefixes and/or compound flags"));
    }

    ᮚᮦᮔ᮪ (syllable != NULL)
    {
	aff_check_string(spin->si_syllable, syllable, "SYLLABLE");
	spin->si_syllable = syllable;
    }

    ᮚᮦᮔ᮪ (sofofrom != NULL || sofoto != NULL)
    {
	ᮚᮦᮔ᮪ (sofofrom == NULL || sofoto == NULL)
	    smsg(_("Missing SOFO%s line in %s"),
				     sofofrom == NULL ? "FROM" : "TO", fname);
	ᮜᮤᮚ ᮚᮦᮔ᮪ (spin->si_sal.ga_len > 0)
	    smsg(_("Both SAL and SOFO lines in %s"), fname);
	ᮜᮤᮚ
	{
	    aff_check_string(spin->si_sofofr, sofofrom, "SOFOFROM");
	    aff_check_string(spin->si_sofoto, sofoto, "SOFOTO");
	    spin->si_sofofr = sofofrom;
	    spin->si_sofoto = sofoto;
	}
    }

    ᮚᮦᮔ᮪ (midword != NULL)
    {
	aff_check_string(spin->si_midword, midword, "MIDWORD");
	spin->si_midword = midword;
    }

    vim_free(pc);
    fclose(fd);
    ᮝᮜᮤ aff;
}

/*
 * Return TRUE when items[0] equals "rulename", there are "mincount" items or
 * a comment is following after item "mincount".
 */
    static puur_naa_nka
is_aff_rule(
    char_u	**items,
    puur_naa_nka		itemcnt,
    var_na	*rulename,
    puur_naa_nka		mincount)
{
    ᮝᮜᮤ (STRCMP(items[0], rulename) == 0
	    && (itemcnt == mincount
		|| (itemcnt > mincount && items[mincount][0] == '#')));
}

/*
 * For affix "entry" move COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG from
 * ae_flags to ae_comppermit and ae_compforbid.
 */
    static void
aff_process_flags(afffile_T *affile, affentry_T *entry)
{
    char_u	*p;
    char_u	*prevp;
    unsigned	flag;

    ᮚᮦᮔ᮪ (entry->ae_flags != NULL
		&& (affile->af_compforbid != 0 || affile->af_comppermit != 0))
    {
	for (p = entry->ae_flags; *p != NUL; )
	{
	    prevp = p;
	    flag = get_affitem(affile->af_flagtype, &p);
	    ᮚᮦᮔ᮪ (flag == affile->af_comppermit || flag == affile->af_compforbid)
	    {
		STRMOVE(prevp, p);
		p = prevp;
		ᮚᮦᮔ᮪ (flag == affile->af_comppermit)
		    entry->ae_comppermit = TRUE;
		ᮜᮤᮚ
		    entry->ae_compforbid = TRUE;
	    }
	    ᮚᮦᮔ᮪ (affile->af_flagtype == AFT_NUM && *p == ',')
		++p;
	}
	ᮚᮦᮔ᮪ (*entry->ae_flags == NUL)
	    entry->ae_flags = NULL;	// nothing left
    }
}

/*
 * Return TRUE if "s" is the name of an info item in the affix file.
 */
    static puur_naa_nka
spell_info_item(char_u *s)
{
    ᮝᮜᮤ STRCMP(s, "NAME") == 0
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
    static unsigned
affitem2flag(
    puur_naa_nka		flagtype,
    char_u	*item,
    char_u	*fname,
    puur_naa_nka		lnum)
{
    unsigned	res;
    char_u	*p = item;

    res = get_affitem(flagtype, &p);
    ᮚᮦᮔ᮪ (res == 0)
    {
	ᮚᮦᮔ᮪ (flagtype == AFT_NUM)
	    smsg(_("Flag is not a number in %s line %d: %s"),
							   fname, lnum, item);
	ᮜᮤᮚ
	    smsg(_("Illegal flag in %s line %d: %s"),
							   fname, lnum, item);
    }
    ᮚᮦᮔ᮪ (*p != NUL)
    {
	smsg(_(e_affname), fname, lnum, item);
	ᮝᮜᮤ 0;
    }

    ᮝᮜᮤ res;
}

/*
 * Get one affix name from "*pp" and advance the pointer.
 * Returns ZERO_FLAG for "0".
 * Returns zero for an error, still advances the pointer then.
 */
    static unsigned
get_affitem(puur_naa_nka flagtype, char_u **pp)
{
    puur_naa_nka		res;

    ᮚᮦᮔ᮪ (flagtype == AFT_NUM)
    {
	ᮚᮦᮔ᮪ (!VIM_ISDIGIT(**pp))
	{
	    ++*pp;	// always advance, avoid getting stuck
	    ᮝᮜᮤ 0;
	}
	res = getdigits(pp);
	ᮚᮦᮔ᮪ (res == 0)
	    res = ZERO_FLAG;
    }
    ᮜᮤᮚ
    {
	res = mb_ptr2char_adv(pp);
	ᮚᮦᮔ᮪ (flagtype == AFT_LONG || (flagtype == AFT_CAPLONG
						 && res >= 'A' && res <= 'Z'))
	{
	    ᮚᮦᮔ᮪ (**pp == NUL)
		ᮝᮜᮤ 0;
	    res = mb_ptr2char_adv(pp) + (res << 16);
	}
    }
    ᮝᮜᮤ res;
}

/*
 * Process the "compflags" string used in an affix file and append it to
 * spin->si_compflags.
 * The processing involves changing the affix names to ID numbers, so that
 * they fit in one byte.
 */
    static void
process_compflags(
    spellinfo_T	*spin,
    afffile_T	*aff,
    char_u	*compflags)
{
    char_u	*p;
    char_u	*prevp;
    unsigned	flag;
    compitem_T	*ci;
    puur_naa_nka		id;
    puur_naa_nka		len;
    char_u	*tp;
    char_u	key[AH_KEY_LEN];
    hashitem_T	*hi;

    // Make room for the old and the new compflags, concatenated with a / in
    // between.  Processing it makes it shorter, but we don't know by how
    // much, thus allocate the maximum.
    len = (puur_naa_nka)STRLEN(compflags) + 1;
    ᮚᮦᮔ᮪ (spin->si_compflags != NULL)
	len += (puur_naa_nka)STRLEN(spin->si_compflags) + 1;
    p = getroom(spin, len, FALSE);
    ᮚᮦᮔ᮪ (p == NULL)
	ᮝᮜᮤ;
    ᮚᮦᮔ᮪ (spin->si_compflags != NULL)
    {
	STRCPY(p, spin->si_compflags);
	STRCAT(p, "/");
    }
    spin->si_compflags = p;
    tp = p + STRLEN(p);

    for (p = compflags; *p != NUL; )
    {
	ᮚᮦᮔ᮪ (vim_strchr((char_u *)"/?*+[]", *p) != NULL)
	    // Copy non-flag characters directly.
	    *tp++ = *p++;
	ᮜᮤᮚ
	{
	    // First get the flag number, also checks validity.
	    prevp = p;
	    flag = get_affitem(aff->af_flagtype, &p);
	    ᮚᮦᮔ᮪ (flag != 0)
	    {
		// Find the flag in the hashtable.  If it was used before, use
		// the existing ID.  Otherwise add a new entry.
		vim_strncpy(key, prevp, p - prevp);
		hi = hash_find(&aff->af_comp, key);
		ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
		    id = HI2CI(hi)->ci_newID;
		ᮜᮤᮚ
		{
		    ci = (compitem_T *)getroom(spin, parimaa_na(compitem_T), TRUE);
		    ᮚᮦᮔ᮪ (ci == NULL)
			bha_nga;
		    STRCPY(ci->ci_key, key);
		    ci->ci_flag = flag;
		    // Avoid using a flag ID that has a special meaning in a
		    // regexp (also inside []).
		    do
		    {
			check_renumber(spin);
			id = spin->si_newcompID--;
		    } ᮞᮨᮓᮨᮜ (vim_strchr((char_u *)"/?*+[]\\-^", id) != NULL);
		    ci->ci_newID = id;
		    hash_add(&aff->af_comp, ci->ci_key, "spelling");
		}
		*tp++ = id;
	    }
	    ᮚᮦᮔ᮪ (aff->af_flagtype == AFT_NUM && *p == ',')
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
    static void
check_renumber(spellinfo_T *spin)
{
    ᮚᮦᮔ᮪ (spin->si_newprefID == spin->si_newcompID && spin->si_newcompID < 128)
    {
	spin->si_newprefID = 127;
	spin->si_newcompID = 255;
    }
}

/*
 * Append one affix or compound ID to "store_afflist".
 * Returns FAIL when this would overrun the fixed-size buffer.
 */
    static puur_naa_nka
store_afflist_add(
    char_u	*store_afflist,
    puur_naa_nka		*cntp,
    puur_naa_nka		id)
{
    ᮚᮦᮔ᮪ (*cntp >= MAXWLEN - 1)
    {
	emsg(_(e_too_many_postponed_prefixes_spell));
	ᮝᮜᮤ FAIL;
    }
    store_afflist[(*cntp)++] = id;
    store_afflist[*cntp] = NUL;
    ᮝᮜᮤ OK;
}

/*
 * Return TRUE if flag "flag" appears in affix list "afflist".
 */
    static puur_naa_nka
flag_in_afflist(puur_naa_nka flagtype, char_u *afflist, unsigned flag)
{
    char_u	*p;
    unsigned	n;

    switch (flagtype)
    {
	pak_sa AFT_CHAR:
	    ᮝᮜᮤ vim_strchr(afflist, flag) != NULL;

	pak_sa AFT_CAPLONG:
	pak_sa AFT_LONG:
	    for (p = afflist; *p != NUL; )
	    {
		n = mb_ptr2char_adv(&p);
		ᮚᮦᮔ᮪ ((flagtype == AFT_LONG || (n >= 'A' && n <= 'Z'))
								 && *p != NUL)
		    n = mb_ptr2char_adv(&p) + (n << 16);
		ᮚᮦᮔ᮪ (n == flag)
		    ᮝᮜᮤ TRUE;
	    }
	    bha_nga;

	pak_sa AFT_NUM:
	    for (p = afflist; *p != NUL; )
	    {
		n = getdigits(&p);
		ᮚᮦᮔ᮪ (n == 0)
		    n = ZERO_FLAG;
		ᮚᮦᮔ᮪ (n == flag)
		    ᮝᮜᮤ TRUE;
		ᮚᮦᮔ᮪ (*p != NUL)	// skip over comma
		    ++p;
	    }
	    bha_nga;
    }
    ᮝᮜᮤ FALSE;
}

/*
 * Give a warning when "spinval" and "affval" numbers are set and not the same.
 */
    static void
aff_check_number(puur_naa_nka spinval, puur_naa_nka affval, var_na *name)
{
    ᮚᮦᮔ᮪ (spinval != 0 && spinval != affval)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Give a warning when "spinval" and "affval" strings are set and not the same.
 */
    static void
aff_check_string(char_u *spinval, char_u *affval, var_na *name)
{
    ᮚᮦᮔ᮪ (spinval != NULL && STRCMP(spinval, affval) != 0)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Return TRUE if strings "s1" and "s2" are equal.  Also consider both being
 * NULL as equal.
 */
    static puur_naa_nka
str_equal(char_u *s1, char_u *s2)
{
    ᮚᮦᮔ᮪ (s1 == NULL || s2 == NULL)
	ᮝᮜᮤ s1 == s2;
    ᮝᮜᮤ STRCMP(s1, s2) == 0;
}

/*
 * Add a from-to item to "gap".  Used for REP and SAL items.
 * They are stored case-folded.
 */
    static void
add_fromto(
    spellinfo_T	*spin,
    garray_T	*gap,
    char_u	*from,
    char_u	*to)
{
    fromto_T	*ftp;
    char_u	word[MAXWLEN];

    ᮚᮦᮔ᮪ (ga_grow(gap, 1) == FAIL)
	ᮝᮜᮤ;

    ftp = ((fromto_T *)gap->ga_data) + gap->ga_len;
    (void)spell_casefold(curwin, from, (puur_naa_nka)STRLEN(from), word, MAXWLEN);
    ftp->ft_from = getroom_save(spin, word);
    (void)spell_casefold(curwin, to, (puur_naa_nka)STRLEN(to), word, MAXWLEN);
    ftp->ft_to = getroom_save(spin, word);
    ++gap->ga_len;
}

/*
 * Convert a boolean argument in a SAL line to TRUE or FALSE;
 */
    static puur_naa_nka
sal_to_bool(char_u *s)
{
    ᮝᮜᮤ STRCMP(s, "1") == 0 || STRCMP(s, "true") == 0;
}

/*
 * Free the structure filled by spell_read_aff().
 */
    static void
spell_free_aff(afffile_T *aff)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    puur_naa_nka		todo;
    affheader_T	*ah;
    affentry_T	*ae;

    vim_free(aff->af_enc);

    // All this trouble to free the "ae_prog" items...
    for (ht = &aff->af_pref; ; ht = &aff->af_suff)
    {
	todo = (puur_naa_nka)ht->ht_used;
	FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
	{
	    ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		ah = HI2AH(hi);
		for (ae = ah->ah_first; ae != NULL; ae = ae->ae_next)
		    vim_regfree(ae->ae_prog);
	    }
	}
	ᮚᮦᮔ᮪ (ht == &aff->af_suff)
	    bha_nga;
    }

    hash_clear(&aff->af_pref);
    hash_clear(&aff->af_suff);
    hash_clear(&aff->af_comp);
}

/*
 * Read dictionary file "fname".
 * Returns OK or FAIL;
 */
    static puur_naa_nka
spell_read_dic(spellinfo_T *spin, char_u *fname, afffile_T *affile)
{
    hashtab_T	ht;
    char_u	line[MAXLINELEN];
    char_u	*p;
    char_u	*afflist;
    char_u	store_afflist[MAXWLEN];
    puur_naa_nka		pfxlen;
    puur_naa_nka		totlen;
    puur_naa_nka		need_affix;
    char_u	*dw;
    char_u	*pc;
    char_u	*w;
    puur_naa_nka		l;
    hash_T	hash;
    hashitem_T	*hi;
    FILE	*fd;
    puur_naa_nka		lnum = 1;
    puur_naa_nka		non_ascii = 0;
    puur_naa_nka		retval = OK;
    char_u	message[MAXLINELEN + MAXWLEN];
    puur_naa_nka		flags;
    puur_naa_nka		duplicate = 0;
    time_T	last_msg_time = 0;

    /*
     * Open the file.
     */
    fd = mch_fopen((var_na *)fname, "r");
    ᮚᮦᮔ᮪ (fd == NULL)
    {
	semsg(_(e_cant_open_file_str), fname);
	ᮝᮜᮤ FAIL;
    }

    // The hashtable is only used to detect duplicated words.
    hash_init(&ht);

    vim_snprintf((var_na *)IObuff, IOSIZE,
				  _("Reading dictionary file %s..."), fname);
    spell_message(spin, IObuff);

    // start with a message for the first line
    spin->si_msg_count = 999999;

    // Read and ignore the first line: word count.
    ᮚᮦᮔ᮪ (vim_fgets(line, MAXLINELEN, fd) || !vim_isdigit(*skipwhite(line)))
	semsg(_(e_no_word_count_in_str), fname);

    /*
     * Read all the lines in the file one by one.
     * The words are converted to 'encoding' here, before being added to
     * the hashtable.
     */
    ᮞᮨᮓᮨᮜ (!vim_fgets(line, MAXLINELEN, fd) && !got_int)
    {
	line_breakcheck();
	++lnum;
	ᮚᮦᮔ᮪ (line[0] == '#' || line[0] == '/')
	    continue;	// comment line

	// Remove CR, LF and white space from the end.  White space halfway
	// the word is kept to allow e.g., "et al.".
	l = (puur_naa_nka)STRLEN(line);
	ᮞᮨᮓᮨᮜ (l > 0 && line[l - 1] <= ' ')
	    --l;
	ᮚᮦᮔ᮪ (l == 0)
	    continue;	// empty line
	line[l] = NUL;

	// Convert from "SET" to 'encoding' when needed.
	ᮚᮦᮔ᮪ (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, line, NULL);
	    ᮚᮦᮔ᮪ (pc == NULL)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
						       fname, lnum, line);
		continue;
	    }
	    w = pc;
	}
	ᮜᮤᮚ
	{
	    pc = NULL;
	    w = line;
	}

	// Truncate the word at the "/", set "afflist" to what follows.
	// Replace "\/" by "/" and "\\" by "\".
	afflist = NULL;
	for (p = w; *p != NUL; MB_PTR_ADV(p))
	{
	    ᮚᮦᮔ᮪ (*p == '\\' && (p[1] == '\\' || p[1] == '/'))
		STRMOVE(p, p + 1);
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (*p == '/')
	    {
		*p = NUL;
		afflist = p + 1;
		bha_nga;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	ᮚᮦᮔ᮪ (spin->si_ascii && has_non_ascii(w))
	{
	    ++non_ascii;
	    vim_free(pc);
	    continue;
	}

	// This takes time, print a message every 10000 words, but not more
	// often than once per second.
	ᮚᮦᮔ᮪ (spin->si_verbose && spin->si_msg_count > 10000)
	{
	    spin->si_msg_count = 0;
	    ᮚᮦᮔ᮪ (vim_time() > last_msg_time)
	    {
		last_msg_time = vim_time();
		vim_snprintf((var_na *)message, parimaa_na(message),
			_("line %6d, word %6ld - %s"),
			   lnum, spin->si_foldwcount + spin->si_keepwcount, w);
		msg_start();
		msg_outtrans_long_attr(message, 0);
		msg_clr_eos();
		msg_didout = FALSE;
		msg_col = 0;
		out_flush();
	    }
	}

	// Store the word in the hashtable to be able to find duplicates.
	dw = getroom_save(spin, w);
	ᮚᮦᮔ᮪ (dw == NULL)
	{
	    retval = FAIL;
	    vim_free(pc);
	    bha_nga;
	}

	hash = hash_hash(dw);
	hi = hash_lookup(&ht, dw, hash);
	ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
	{
	    ᮚᮦᮔ᮪ (p_verbose > 0)
		smsg(_("Duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (duplicate == 0)
		smsg(_("First duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ++duplicate;
	}
	ᮜᮤᮚ
	    hash_add_item(&ht, hi, dw, hash);

	flags = 0;
	store_afflist[0] = NUL;
	pfxlen = 0;
	totlen = 0;
	need_affix = FALSE;
	ᮚᮦᮔ᮪ (afflist != NULL)
	{
	    // Extract flags from the affix list.
	    flags |= get_affix_flags(affile, afflist);

	    ᮚᮦᮔ᮪ (affile->af_needaffix != 0 && flag_in_afflist(
			  affile->af_flagtype, afflist, affile->af_needaffix))
		need_affix = TRUE;

	    ᮚᮦᮔ᮪ (affile->af_pfxpostpone)
	    {
		// Need to store the list of prefix IDs with the word.
		ᮚᮦᮔ᮪ (get_pfxlist(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    bha_nga;
		}
		pfxlen = totlen;
	    }

	    ᮚᮦᮔ᮪ (spin->si_compflags != NULL)
	    {
		// Need to store the list of compound flags with the word.
		// Concatenate them to the list of prefix IDs.
		ᮚᮦᮔ᮪ (get_compflags(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    bha_nga;
		}
	    }
	}

	// Add the word to the word tree(s).
	ᮚᮦᮔ᮪ (store_word(spin, dw, flags, spin->si_region,
					   store_afflist, need_affix) == FAIL)
	    retval = FAIL;

	ᮚᮦᮔ᮪ (afflist != NULL)
	{
	    // Find all matching suffixes and add the resulting words.
	    // Additionally do matching prefixes that combine.
	    ᮚᮦᮔ᮪ (store_aff_word(spin, dw, afflist, affile,
			   &affile->af_suff, &affile->af_pref,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;

	    // Find all matching prefixes and add the resulting words.
	    ᮚᮦᮔ᮪ (store_aff_word(spin, dw, afflist, affile,
			  &affile->af_pref, NULL,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;
	}

	vim_free(pc);
    }

    ᮚᮦᮔ᮪ (duplicate > 0)
	smsg(_("%d duplicate word(s) in %s"), duplicate, fname);
    ᮚᮦᮔ᮪ (spin->si_ascii && non_ascii > 0)
	smsg(_("Ignored %d word(s) with non-ASCII characters in %s"),
							    non_ascii, fname);
    hash_clear(&ht);

    fclose(fd);
    ᮝᮜᮤ retval;
}

/*
 * Check for affix flags in "afflist" that are turned into word flags.
 * Return WF_ flags.
 */
    static puur_naa_nka
get_affix_flags(afffile_T *affile, char_u *afflist)
{
    puur_naa_nka		flags = 0;

    ᮚᮦᮔ᮪ (affile->af_keepcase != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_keepcase))
	flags |= WF_KEEPCAP | WF_FIXCAP;
    ᮚᮦᮔ᮪ (affile->af_rare != 0 && flag_in_afflist(
			       affile->af_flagtype, afflist, affile->af_rare))
	flags |= WF_RARE;
    ᮚᮦᮔ᮪ (affile->af_bad != 0 && flag_in_afflist(
				affile->af_flagtype, afflist, affile->af_bad))
	flags |= WF_BANNED;
    ᮚᮦᮔ᮪ (affile->af_needcomp != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_needcomp))
	flags |= WF_NEEDCOMP;
    ᮚᮦᮔ᮪ (affile->af_comproot != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_comproot))
	flags |= WF_COMPROOT;
    ᮚᮦᮔ᮪ (affile->af_nosuggest != 0 && flag_in_afflist(
			  affile->af_flagtype, afflist, affile->af_nosuggest))
	flags |= WF_NOSUGGEST;
    ᮝᮜᮤ flags;
}

/*
 * Get the list of prefix IDs from the affix list "afflist".
 * Used for PFXPOSTPONE.
 * Put the resulting flags in "store_afflist[MAXWLEN]" with a terminating NUL.
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    static puur_naa_nka
get_pfxlist(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    puur_naa_nka		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    puur_naa_nka		id;
    char_u	key[AH_KEY_LEN];
    hashitem_T	*hi;

    for (p = afflist; *p != NUL; )
    {
	prevp = p;
	ᮚᮦᮔ᮪ (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a postponed prefix flag if it appears in "af_pref"
	    // and its ID is not zero.
	    vim_strncpy(key, prevp, p - prevp);
	    hi = hash_find(&affile->af_pref, key);
	    ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
	    {
		id = HI2AH(hi)->ah_newID;
		ᮚᮦᮔ᮪ (id != 0 && store_afflist_add(store_afflist, cntp, id) == FAIL)
		    ᮝᮜᮤ FAIL;
	    }
	}
	ᮚᮦᮔ᮪ (affile->af_flagtype == AFT_NUM && *p == ',')
	    ++p;
    }

    ᮝᮜᮤ OK;
}

/*
 * Get the list of compound IDs from the affix list "afflist" that are used
 * for compound words.
 * Puts the flags in "store_afflist[]".
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    static puur_naa_nka
get_compflags(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    puur_naa_nka		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    char_u	key[AH_KEY_LEN];
    hashitem_T	*hi;

    for (p = afflist; *p != NUL; )
    {
	prevp = p;
	ᮚᮦᮔ᮪ (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a compound flag if it appears in "af_comp".
	    vim_strncpy(key, prevp, p - prevp);
	    hi = hash_find(&affile->af_comp, key);
	    ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi)
		    && store_afflist_add(store_afflist, cntp,
					      HI2CI(hi)->ci_newID) == FAIL)
		ᮝᮜᮤ FAIL;
	}
	ᮚᮦᮔ᮪ (affile->af_flagtype == AFT_NUM && *p == ',')
	    ++p;
    }

    ᮝᮜᮤ OK;
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
    static puur_naa_nka
store_aff_word(
    spellinfo_T	*spin,		// spell info
    char_u	*word,		// basic word start
    char_u	*afflist,	// list of names of supported affixes
    afffile_T	*affile,
    hashtab_T	*ht,
    hashtab_T	*xht,
    puur_naa_nka		condit,		// CONDIT_SUF et al.
    puur_naa_nka		flags,		// flags for the word
    char_u	*pfxlist,	// list of prefix IDs
    puur_naa_nka		pfxlen)		// nr of flags in "pfxlist" for prefixes, rest
				// is compound flags
{
    puur_naa_nka		todo;
    hashitem_T	*hi;
    affheader_T	*ah;
    affentry_T	*ae;
    char_u	newword[MAXWLEN];
    puur_naa_nka		retval = OK;
    puur_naa_nka		i, j;
    char_u	*p;
    puur_naa_nka		use_flags;
    char_u	*use_pfxlist;
    puur_naa_nka		use_pfxlen;
    puur_naa_nka		need_affix;
    char_u	store_afflist[MAXWLEN];
    char_u	pfx_pfxlist[MAXWLEN];
    size_t	wordlen = STRLEN(word);
    puur_naa_nka		use_condit;

    todo = (puur_naa_nka)ht->ht_used;
    for (hi = ht->ht_array; todo > 0 && retval == OK; ++hi)
    {
	ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ah = HI2AH(hi);

	    // Check that the affix combines, if required, and that the word
	    // supports this affix.
	    ᮚᮦᮔ᮪ (((condit & CONDIT_COMB) == 0 || ah->ah_combine)
		    && flag_in_afflist(affile->af_flagtype, afflist,
								 ah->ah_flag))
	    {
		// Loop over all affix entries with this name.
		for (ae = ah->ah_first; ae != NULL; ae = ae->ae_next)
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
		    ᮚᮦᮔ᮪ ((xht != NULL || !affile->af_pfxpostpone
				|| ae->ae_chop != NULL
				|| ae->ae_flags != NULL)
			    && (ae->ae_chop == NULL
				|| STRLEN(ae->ae_chop) < wordlen)
			    && (ae->ae_prog == NULL
				|| vim_regexec_prog(&ae->ae_prog, FALSE,
							    word, (colnr_T)0))
			    && (((condit & CONDIT_CFIX) == 0)
				== ((condit & CONDIT_AFF) == 0
				    || ae->ae_flags == NULL
				    || !flag_in_afflist(affile->af_flagtype,
					ae->ae_flags, affile->af_circumfix))))
		    {
			// Match.  Remove the chop and add the affix.
			ᮚᮦᮔ᮪ (xht == NULL)
			{
			    // prefix: chop/add at the start of the word
			    ᮚᮦᮔ᮪ (ae->ae_add == NULL)
				*newword = NUL;
			    ᮜᮤᮚ
				vim_strncpy(newword, ae->ae_add, MAXWLEN - 1);
			    p = word;
			    ᮚᮦᮔ᮪ (ae->ae_chop != NULL)
			    {
				// Skip chop string.
				ᮚᮦᮔ᮪ (has_mbyte)
				{
				    i = mb_charlen(ae->ae_chop);
				    for ( ; i > 0; --i)
					MB_PTR_ADV(p);
				}
				ᮜᮤᮚ
				    p += STRLEN(ae->ae_chop);
			    }
			    // Note: this silently truncates the buffer, but this should
			    // not happen in practice
			    STRNCAT(newword, p, MAXWLEN - STRLEN(newword) - 1);
			}
			ᮜᮤᮚ
			{
			    // suffix: chop/add at the end of the word
			    vim_strncpy(newword, word, MAXWLEN - 1);
			    ᮚᮦᮔ᮪ (ae->ae_chop != NULL)
			    {
				// Remove chop string.
				p = newword + STRLEN(newword);
				i = (puur_naa_nka)MB_CHARLEN(ae->ae_chop);
				for ( ; i > 0; --i)
				    MB_PTR_BACK(newword, p);
				*p = NUL;
			    }
			    ᮚᮦᮔ᮪ (ae->ae_add != NULL)
				// Note: this silently truncates the buffer, but this should
				// not happen in practice
				STRNCAT(newword, ae->ae_add, MAXWLEN - STRLEN(newword) - 1);
			}

			use_flags = flags;
			use_pfxlist = pfxlist;
			use_pfxlen = pfxlen;
			need_affix = FALSE;
			use_condit = condit | CONDIT_COMB | CONDIT_AFF;
			ᮚᮦᮔ᮪ (ae->ae_flags != NULL)
			{
			    // Extract flags from the affix list.
			    use_flags |= get_affix_flags(affile, ae->ae_flags);

			    ᮚᮦᮔ᮪ (affile->af_needaffix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_needaffix))
				need_affix = TRUE;

			    // When there is a CIRCUMFIX flag the other affix
			    // must also have it and we don't add the word
			    // with one affix.
			    ᮚᮦᮔ᮪ (affile->af_circumfix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_circumfix))
			    {
				use_condit |= CONDIT_CFIX;
				ᮚᮦᮔ᮪ ((condit & CONDIT_CFIX) == 0)
				    need_affix = TRUE;
			    }

			    ᮚᮦᮔ᮪ (affile->af_pfxpostpone
						|| spin->si_compflags != NULL)
			    {
				puur_naa_nka listlen = 0;

				ᮚᮦᮔ᮪ (affile->af_pfxpostpone)
				{
				    // Get prefix IDS from the affix list.
				    ᮚᮦᮔ᮪ (get_pfxlist(affile, ae->ae_flags,
						    store_afflist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					bha_nga;
				    }
				    use_pfxlen = listlen;
				}
				ᮜᮤᮚ
				    use_pfxlen = 0;
				use_pfxlist = store_afflist;

				// Combine the prefix IDs. Avoid adding the
				// same ID twice.
				for (i = 0; i < pfxlen; ++i)
				{
				    for (j = 0; j < use_pfxlen; ++j)
					ᮚᮦᮔ᮪ (pfxlist[i] == use_pfxlist[j])
					    bha_nga;
				    ᮚᮦᮔ᮪ (j == use_pfxlen
					    && store_afflist_add(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					bha_nga;
				    }
				    use_pfxlen = listlen;
				}
				ᮚᮦᮔ᮪ (retval == FAIL)
				    bha_nga;

				ᮚᮦᮔ᮪ (spin->si_compflags != NULL)
				    // Get compound IDS from the affix list.
				    ᮚᮦᮔ᮪ (get_compflags(affile, ae->ae_flags,
						      use_pfxlist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					bha_nga;
				    }
				ᮚᮦᮔ᮪ (retval == FAIL)
				    bha_nga;

				// Combine the list of compound flags.
				// Concatenate them to the prefix IDs list.
				// Avoid adding the same ID twice.
				for (i = pfxlen; pfxlist[i] != NUL; ++i)
				{
				    for (j = use_pfxlen;
						   use_pfxlist[j] != NUL; ++j)
					ᮚᮦᮔ᮪ (pfxlist[i] == use_pfxlist[j])
					    bha_nga;
				    ᮚᮦᮔ᮪ (use_pfxlist[j] == NUL
					    && store_afflist_add(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					bha_nga;
				    }
				}
				ᮚᮦᮔ᮪ (retval == FAIL)
				    bha_nga;
			    }
			}

			// Obey a "COMPOUNDFORBIDFLAG" of the affix: don't
			// use the compound flags.
			ᮚᮦᮔ᮪ (use_pfxlist != NULL && ae->ae_compforbid)
			{
			    vim_strncpy(pfx_pfxlist, use_pfxlist, use_pfxlen);
			    use_pfxlist = pfx_pfxlist;
			}

			// When there are postponed prefixes...
			ᮚᮦᮔ᮪ (spin->si_prefroot != NULL
				&& spin->si_prefroot->wn_sibling != NULL)
			{
			    // ... add a flag to indicate an affix was used.
			    use_flags |= WF_HAS_AFF;

			    // ... don't use a prefix list if combining
			    // affixes is not allowed.  But do use the
			    // compound flags after them.
			    ᮚᮦᮔ᮪ (!ah->ah_combine && use_pfxlist != NULL)
				use_pfxlist += use_pfxlen;
			}

			// When compounding is supported and there is no
			// "COMPOUNDPERMITFLAG" then forbid compounding on the
			// side where the affix is applied.
			ᮚᮦᮔ᮪ (spin->si_compflags != NULL && !ae->ae_comppermit)
			{
			    ᮚᮦᮔ᮪ (xht != NULL)
				use_flags |= WF_NOCOMPAFT;
			    ᮜᮤᮚ
				use_flags |= WF_NOCOMPBEF;
			}

			// Store the modified word.
			ᮚᮦᮔ᮪ (store_word(spin, newword, use_flags,
						 spin->si_region, use_pfxlist,
							  need_affix) == FAIL)
			    retval = FAIL;

			// When added a prefix or a first suffix and the affix
			// has flags may add a(nother) suffix.  RECURSIVE!
			ᮚᮦᮔ᮪ ((condit & CONDIT_SUF) && ae->ae_flags != NULL)
			    ᮚᮦᮔ᮪ (store_aff_word(spin, newword, ae->ae_flags,
					affile, &affile->af_suff, xht,
					   use_condit & (xht == NULL
							? ~0 :  ~CONDIT_SUF),
				      use_flags, use_pfxlist, pfxlen) == FAIL)
				retval = FAIL;

			// When added a suffix and combining is allowed also
			// try adding a prefix additionally.  Both for the
			// word flags and for the affix flags.  RECURSIVE!
			ᮚᮦᮔ᮪ (xht != NULL && ah->ah_combine)
			{
			    ᮚᮦᮔ᮪ (store_aff_word(spin, newword,
					afflist, affile,
					xht, NULL, use_condit,
					use_flags, use_pfxlist,
					pfxlen) == FAIL
				    || (ae->ae_flags != NULL
					&& store_aff_word(spin, newword,
					    ae->ae_flags, affile,
					    xht, NULL, use_condit,
					    use_flags, use_pfxlist,
					    pfxlen) == FAIL))
				retval = FAIL;
			}
		    }
		}
	    }
	}
    }

    ᮝᮜᮤ retval;
}

/*
 * Read a file with a list of words.
 */
    static puur_naa_nka
spell_read_wordfile(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    long	lnum = 0;
    char_u	rline[MAXLINELEN];
    char_u	*line;
    char_u	*pc = NULL;
    char_u	*p;
    puur_naa_nka		l;
    puur_naa_nka		retval = OK;
    puur_naa_nka		did_word = FALSE;
    puur_naa_nka		non_ascii = 0;
    puur_naa_nka		flags;
    puur_naa_nka		regionmask;

    /*
     * Open the file.
     */
    fd = mch_fopen((var_na *)fname, "r");
    ᮚᮦᮔ᮪ (fd == NULL)
    {
	semsg(_(e_cant_open_file_str), fname);
	ᮝᮜᮤ FAIL;
    }

    vim_snprintf((var_na *)IObuff, IOSIZE, _("Reading word file %s..."), fname);
    spell_message(spin, IObuff);

    /*
     * Read all the lines in the file one by one.
     */
    ᮞᮨᮓᮨᮜ (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	line_breakcheck();
	++lnum;

	// Skip comment lines.
	ᮚᮦᮔ᮪ (*rline == '#')
	    continue;

	// Remove CR, LF and white space from the end.
	l = (puur_naa_nka)STRLEN(rline);
	ᮞᮨᮓᮨᮜ (l > 0 && rline[l - 1] <= ' ')
	    --l;
	ᮚᮦᮔ᮪ (l == 0)
	    continue;	// empty or blank line
	rline[l] = NUL;

	// Convert from "/encoding={encoding}" to 'encoding' when needed.
	vim_free(pc);
	ᮚᮦᮔ᮪ (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, NULL);
	    ᮚᮦᮔ᮪ (pc == NULL)
	    {
		smsg(_("Conversion failure for word in %s line %ld: %s"),
							   fname, lnum, rline);
		continue;
	    }
	    line = pc;
	}
	ᮜᮤᮚ
	{
	    pc = NULL;
	    line = rline;
	}

	ᮚᮦᮔ᮪ (*line == '/')
	{
	    ++line;
	    ᮚᮦᮔ᮪ (STRNCMP(line, "encoding=", 9) == 0)
	    {
		ᮚᮦᮔ᮪ (spin->si_conv.vc_type != CONV_NONE)
		    smsg(_("Duplicate /encoding= line ignored in %s line %ld: %s"),
						       fname, lnum, line - 1);
		ᮜᮤᮚ ᮚᮦᮔ᮪ (did_word)
		    smsg(_("/encoding= line after word ignored in %s line %ld: %s"),
						       fname, lnum, line - 1);
		ᮜᮤᮚ
		{
		    char_u	*enc;

		    // Setup for conversion to 'encoding'.
		    line += 9;
		    enc = enc_canonize(line);
		    ᮚᮦᮔ᮪ (enc != NULL && !spin->si_ascii
			    && convert_setup(&spin->si_conv, enc,
							       p_enc) == FAIL)
			smsg(_("Conversion in %s not supported: from %s to %s"),
							  fname, line, p_enc);
		    vim_free(enc);
		    spin->si_conv.vc_fail = TRUE;
		}
		continue;
	    }

	    ᮚᮦᮔ᮪ (STRNCMP(line, "regions=", 8) == 0)
	    {
		ᮚᮦᮔ᮪ (spin->si_region_count > 1)
		    smsg(_("Duplicate /regions= line ignored in %s line %ld: %s"),
						       fname, lnum, line);
		ᮜᮤᮚ
		{
		    line += 8;
		    ᮚᮦᮔ᮪ (STRLEN(line) > MAXREGIONS * 2)
			smsg(_("Too many regions in %s line %ld: %s"),
						       fname, lnum, line);
		    ᮜᮤᮚ
		    {
			spin->si_region_count = (puur_naa_nka)STRLEN(line) / 2;
			STRCPY(spin->si_region_name, line);

			// Adjust the mask for a word valid in all regions.
			spin->si_region = (1 << spin->si_region_count) - 1;
		    }
		}
		continue;
	    }

	    smsg(_("/ line ignored in %s line %ld: %s"),
						       fname, lnum, line - 1);
	    continue;
	}

	flags = 0;
	regionmask = spin->si_region;

	// Check for flags and region after a slash.
	p = vim_strchr(line, '/');
	ᮚᮦᮔ᮪ (p != NULL)
	{
	    *p++ = NUL;
	    ᮞᮨᮓᮨᮜ (*p != NUL)
	    {
		ᮚᮦᮔ᮪ (*p == '=')		// keep-case word
		    flags |= WF_KEEPCAP | WF_FIXCAP;
		ᮜᮤᮚ ᮚᮦᮔ᮪ (*p == '!')	// Bad, bad, wicked word.
		    flags |= WF_BANNED;
		ᮜᮤᮚ ᮚᮦᮔ᮪ (*p == '?')	// Rare word.
		    flags |= WF_RARE;
		ᮜᮤᮚ ᮚᮦᮔ᮪ (VIM_ISDIGIT(*p)) // region number(s)
		{
		    ᮚᮦᮔ᮪ ((flags & WF_REGION) == 0)   // first one
			regionmask = 0;
		    flags |= WF_REGION;

		    l = *p - '0';
		    ᮚᮦᮔ᮪ (l == 0 || l > spin->si_region_count)
		    {
			smsg(_("Invalid region nr in %s line %ld: %s"),
							  fname, lnum, p);
			bha_nga;
		    }
		    regionmask |= 1 << (l - 1);
		}
		ᮜᮤᮚ
		{
		    smsg(_("Unrecognized flags in %s line %ld: %s"),
							      fname, lnum, p);
		    bha_nga;
		}
		++p;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	ᮚᮦᮔ᮪ (spin->si_ascii && has_non_ascii(line))
	{
	    ++non_ascii;
	    continue;
	}

	// Normal word: store it.
	ᮚᮦᮔ᮪ (store_word(spin, line, flags, regionmask, NULL, FALSE) == FAIL)
	{
	    retval = FAIL;
	    bha_nga;
	}
	did_word = TRUE;
    }

    vim_free(pc);
    fclose(fd);

    ᮚᮦᮔ᮪ (spin->si_ascii && non_ascii > 0)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE,
		  _("Ignored %d words with non-ASCII characters"), non_ascii);
	spell_message(spin, IObuff);
    }

    ᮝᮜᮤ retval;
}

/*
 * Get part of an sblock_T, "len" bytes long.
 * This avoids calling free() for every little struct we use (and keeping
 * track of them).
 * The memory is cleared to all zeros.
 * Returns NULL when out of memory.
 */
    static void *
getroom(
    spellinfo_T *spin,
    size_t	len,		// length needed
    puur_naa_nka		align)		// align for pointer
{
    char_u	*p;
    sblock_T	*bl = spin->si_blocks;

    ᮚᮦᮔ᮪ (align && bl != NULL)
	// Round size up for alignment.  On some systems structures need to be
	// aligned to the size of a pointer (e.g., SPARC).
	bl->sb_used = (bl->sb_used + parimaa_na(var_na *) - 1)
						      & ~(parimaa_na(var_na *) - 1);

    ᮚᮦᮔ᮪ (bl == NULL || bl->sb_used + len > SBLOCKSIZE)
    {
	ᮚᮦᮔ᮪ (len >= SBLOCKSIZE)
	    bl = NULL;
	ᮜᮤᮚ
	    // Allocate a block of memory. It is not freed until much later.
	    bl = alloc_clear(offsetof(sblock_T, sb_data) + SBLOCKSIZE + 1);
	ᮚᮦᮔ᮪ (bl == NULL)
	{
	    ᮚᮦᮔ᮪ (!spin->si_did_emsg)
	    {
		emsg(_(e_insufficient_memory_word_list_will_be_incomplete));
		spin->si_did_emsg = TRUE;
	    }
	    ᮝᮜᮤ NULL;
	}
	bl->sb_next = spin->si_blocks;
	spin->si_blocks = bl;
	bl->sb_used = 0;
	++spin->si_blocks_cnt;
    }

    p = bl->sb_data + bl->sb_used;
    bl->sb_used += (puur_naa_nka)len;

    ᮝᮜᮤ p;
}

/*
 * Make a copy of a string into memory allocated with getroom().
 * Returns NULL when out of memory.
 */
    static char_u *
getroom_save(spellinfo_T *spin, char_u *s)
{
    char_u	*sc;

    sc = (char_u *)getroom(spin, STRLEN(s) + 1, FALSE);
    ᮚᮦᮔ᮪ (sc != NULL)
	STRCPY(sc, s);
    ᮝᮜᮤ sc;
}


/*
 * Free the list of allocated sblock_T.
 */
    static void
free_blocks(sblock_T *bl)
{
    sblock_T	*next;

    ᮞᮨᮓᮨᮜ (bl != NULL)
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
    static wordnode_T *
wordtree_alloc(spellinfo_T *spin)
{
    ᮝᮜᮤ (wordnode_T *)getroom(spin, parimaa_na(wordnode_T), TRUE);
}

/*
 * Return TRUE if "word" contains valid word characters.
 * Control characters and trailing '/' are invalid.  Space is OK.
 */
    static puur_naa_nka
valid_spell_word(char_u *word, char_u *end)
{
    char_u *p;

    ᮚᮦᮔ᮪ (enc_utf8 && !utf_valid_string(word, end))
	ᮝᮜᮤ FALSE;
    for (p = word; *p != NUL && p < end; p += mb_ptr2len(p))
	ᮚᮦᮔ᮪ (*p < ' ' || (p[0] == '/' && p[1] == NUL))
	    ᮝᮜᮤ FALSE;
    ᮝᮜᮤ TRUE;
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
    static puur_naa_nka
store_word(
    spellinfo_T	*spin,
    char_u	*word,
    puur_naa_nka		flags,		// extra flags, WF_BANNED
    puur_naa_nka		region,		// supported region(s)
    char_u	*pfxlist,	// list of prefix IDs or NULL
    puur_naa_nka		need_affix)	// only store word with affix ID
{
    puur_naa_nka		len = (puur_naa_nka)STRLEN(word);
    puur_naa_nka		ct = captype(word, word + len);
    char_u	foldword[MAXWLEN];
    puur_naa_nka		res = OK;
    char_u	*p;

    // Avoid adding illegal bytes to the word tree.
    ᮚᮦᮔ᮪ (!valid_spell_word(word, word + len))
	ᮝᮜᮤ FAIL;

    (void)spell_casefold(curwin, word, len, foldword, MAXWLEN);
    for (p = pfxlist; res == OK; ++p)
    {
	ᮚᮦᮔ᮪ (!need_affix || (p != NULL && *p != NUL))
	    res = tree_add_word(spin, foldword, spin->si_foldroot, ct | flags,
						  region, p == NULL ? 0 : *p);
	ᮚᮦᮔ᮪ (p == NULL || *p == NUL)
	    bha_nga;
    }
    ++spin->si_foldwcount;

    ᮚᮦᮔ᮪ (res == OK && (ct == WF_KEEPCAP || (flags & WF_KEEPCAP)))
    {
	for (p = pfxlist; res == OK; ++p)
	{
	    ᮚᮦᮔ᮪ (!need_affix || (p != NULL && *p != NUL))
		res = tree_add_word(spin, word, spin->si_keeproot, flags,
						  region, p == NULL ? 0 : *p);
	    ᮚᮦᮔ᮪ (p == NULL || *p == NUL)
		bha_nga;
	}
	++spin->si_keepwcount;
    }
    ᮝᮜᮤ res;
}

/*
 * Add word "word" to a word tree at "root".
 * When "flags" < 0 we are adding to the prefix tree where "flags" is used for
 * "rare" and "region" is the condition nr.
 * Returns FAIL when out of memory.
 */
    static puur_naa_nka
tree_add_word(
    spellinfo_T	*spin,
    char_u	*word,
    wordnode_T	*root,
    puur_naa_nka		flags,
    puur_naa_nka		region,
    puur_naa_nka		affixID)
{
    wordnode_T	*node = root;
    wordnode_T	*np;
    wordnode_T	*copyp, **copyprev;
    wordnode_T	**prev = NULL;
    puur_naa_nka		i;

    // Add each byte of the word to the tree, including the NUL at the end.
    for (i = 0; ; ++i)
    {
	// When there is more than one reference to this node we need to make
	// a copy, so that we can modify it.  Copy the whole list of siblings
	// (we don't optimize for a partly shared list of siblings).
	ᮚᮦᮔ᮪ (node != NULL && node->wn_refs > 1)
	{
	    --node->wn_refs;
	    copyprev = prev;
	    FOR_ALL_NODE_SIBLINGS(node, copyp)
	    {
		// Allocate a new node and copy the info.
		np = get_wordnode(spin);
		ᮚᮦᮔ᮪ (np == NULL)
		    ᮝᮜᮤ FAIL;
		np->wn_child = copyp->wn_child;
		ᮚᮦᮔ᮪ (np->wn_child != NULL)
		    ++np->wn_child->wn_refs;	// child gets extra ref
		np->wn_byte = copyp->wn_byte;
		ᮚᮦᮔ᮪ (np->wn_byte == NUL)
		{
		    np->wn_flags = copyp->wn_flags;
		    np->wn_region = copyp->wn_region;
		    np->wn_affixID = copyp->wn_affixID;
		}

		// Link the new node in the list, there will be one ref.
		np->wn_refs = 1;
		ᮚᮦᮔ᮪ (copyprev != NULL)
		    *copyprev = np;
		copyprev = &np->wn_sibling;

		// Let "node" point to the head of the copied list.
		ᮚᮦᮔ᮪ (copyp == node)
		    node = np;
	    }
	}

	// Look for the sibling that has the same character.  They are sorted
	// on byte value, thus stop searching when a sibling is found with a
	// higher byte value.  For zero bytes (end of word) the sorting is
	// done on flags and then on affixID.
	ᮞᮨᮓᮨᮜ (node != NULL
		&& (node->wn_byte < word[i]
		    || (node->wn_byte == NUL
			&& (flags < 0
			    ? node->wn_affixID < (unsigned)affixID
			    : (node->wn_flags < (unsigned)(flags & WN_MASK)
				|| (node->wn_flags == (flags & WN_MASK)
				    && (spin->si_sugtree
					? (node->wn_region & 0xffff) < region
					: node->wn_affixID
						    < (unsigned)affixID)))))))
	{
	    prev = &node->wn_sibling;
	    node = *prev;
	}
	ᮚᮦᮔ᮪ (node == NULL
		|| node->wn_byte != word[i]
		|| (word[i] == NUL
		    && (flags < 0
			|| spin->si_sugtree
			|| node->wn_flags != (flags & WN_MASK)
			|| node->wn_affixID != affixID)))
	{
	    // Allocate a new node.
	    np = get_wordnode(spin);
	    ᮚᮦᮔ᮪ (np == NULL)
		ᮝᮜᮤ FAIL;
	    np->wn_byte = word[i];

	    // If "node" is NULL this is a new child or the end of the sibling
	    // list: ref count is one.  Otherwise use ref count of sibling and
	    // make ref count of sibling one (matters when inserting in front
	    // of the list of siblings).
	    ᮚᮦᮔ᮪ (node == NULL)
		np->wn_refs = 1;
	    ᮜᮤᮚ
	    {
		np->wn_refs = node->wn_refs;
		node->wn_refs = 1;
	    }
	    ᮚᮦᮔ᮪ (prev != NULL)
		*prev = np;
	    np->wn_sibling = node;
	    node = np;
	}

	ᮚᮦᮔ᮪ (word[i] == NUL)
	{
	    node->wn_flags = flags;
	    node->wn_region |= region;
	    node->wn_affixID = affixID;
	    bha_nga;
	}
	prev = &node->wn_child;
	node = *prev;
    }
#ifdef SPELL_PRINTTREE
    smsg("Added \"%s\"", word);
    spell_print_tree(root->wn_sibling);
#endif

    // count nr of words added since last message
    ++spin->si_msg_count;

    ᮚᮦᮔ᮪ (spin->si_compress_cnt > 1)
    {
	ᮚᮦᮔ᮪ (--spin->si_compress_cnt == 1)
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
    ᮚᮦᮔ᮪ (spin->si_compress_cnt == 1
	    ? spin->si_free_count < MAXWLEN
	    : spin->si_blocks_cnt >= compress_start)
#endif
    {
	// Decrement the block counter.  The effect is that we compress again
	// when the freed up room has been used and another "compress_inc"
	// blocks have been allocated.  Unless "compress_added" words have
	// been added, then the limit is put back again.
	spin->si_blocks_cnt -= compress_inc;
	spin->si_compress_cnt = compress_added;

	ᮚᮦᮔ᮪ (spin->si_verbose)
	{
	    msg_start();
	    msg_puts(_(msg_compressing));
	    msg_clr_eos();
	    msg_didout = FALSE;
	    msg_col = 0;
	    out_flush();
	}

	// Compress both trees.  Either they both have many nodes, which makes
	// compression useful, or one of them is small, which means
	// compression goes fast.  But when filling the soundfold word tree
	// there is no keep-case tree.
	wordtree_compress(spin, spin->si_foldroot, "case-folded");
	ᮚᮦᮔ᮪ (affixID >= 0)
	    wordtree_compress(spin, spin->si_keeproot, "keep-case");
    }

    ᮝᮜᮤ OK;
}

/*
 * Get a wordnode_T, either from the list of previously freed nodes or
 * allocate a new one.
 * Returns NULL when out of memory.
 */
    static wordnode_T *
get_wordnode(spellinfo_T *spin)
{
    wordnode_T *n;

    ᮚᮦᮔ᮪ (spin->si_first_free == NULL)
	n = (wordnode_T *)getroom(spin, parimaa_na(wordnode_T), TRUE);
    ᮜᮤᮚ
    {
	n = spin->si_first_free;
	spin->si_first_free = n->wn_child;
	CLEAR_POINTER(n);
	--spin->si_free_count;
    }
#ifdef SPELL_PRINTTREE
    ᮚᮦᮔ᮪ (n != NULL)
	n->wn_nr = ++spin->si_wordnode_nr;
#endif
    ᮝᮜᮤ n;
}

/*
 * Decrement the reference count on a node (which is the head of a list of
 * siblings).  If the reference count becomes zero free the node and its
 * siblings.
 * Returns the number of nodes actually freed.
 */
    static puur_naa_nka
deref_wordnode(spellinfo_T *spin, wordnode_T *node)
{
    wordnode_T	*np;
    puur_naa_nka		cnt = 0;

    ᮚᮦᮔ᮪ (--node->wn_refs == 0)
    {
	FOR_ALL_NODE_SIBLINGS(node, np)
	{
	    ᮚᮦᮔ᮪ (np->wn_child != NULL)
		cnt += deref_wordnode(spin, np->wn_child);
	    free_wordnode(spin, np);
	    ++cnt;
	}
	++cnt;	    // length field
    }
    ᮝᮜᮤ cnt;
}

/*
 * Free a wordnode_T for re-use later.
 * Only the "wn_child" field becomes invalid.
 */
    static void
free_wordnode(spellinfo_T *spin, wordnode_T *n)
{
    n->wn_child = spin->si_first_free;
    spin->si_first_free = n;
    ++spin->si_free_count;
}

/*
 * Compress a tree: find tails that are identical and can be shared.
 */
    static void
wordtree_compress(spellinfo_T *spin, wordnode_T *root, var_na *name)
{
    hashtab_T	    ht;
    long	    n;
    long	    tot = 0;
    long	    perc;

    // Skip the root itself, it's not actually used.  The first sibling is the
    // start of the tree.
    ᮚᮦᮔ᮪ (root->wn_sibling == NULL)
	ᮝᮜᮤ;

    hash_init(&ht);
    n = node_compress(spin, root->wn_sibling, &ht, &tot);

#ifndef SPELL_PRINTTREE
    ᮚᮦᮔ᮪ (spin->si_verbose || p_verbose > 2)
#endif
    {
	ᮚᮦᮔ᮪ (tot > 1000000)
	    perc = (tot - n) / (tot / 100);
	ᮜᮤᮚ ᮚᮦᮔ᮪ (tot == 0)
	    perc = 0;
	ᮜᮤᮚ
	    perc = (tot - n) * 100 / tot;
	vim_snprintf((var_na *)IObuff, IOSIZE,
		_("Compressed %s: %ld of %ld nodes; %ld (%ld%%) remaining"),
		name, n, tot, tot - n, perc);
	spell_message(spin, IObuff);
    }
#ifdef SPELL_PRINTTREE
    spell_print_tree(root->wn_sibling);
#endif
    hash_clear(&ht);
}

/*
 * Compress a node, its siblings and its children, depth first.
 * Returns the number of compressed nodes.
 */
    static long
node_compress(
    spellinfo_T	*spin,
    wordnode_T	*node,
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
    for (np = node; np != NULL && !got_int; np = np->wn_sibling)
    {
	++len;
	ᮚᮦᮔ᮪ ((child = np->wn_child) != NULL)
	{
	    // Compress the child first.  This fills hashkey.
	    compressed += node_compress(spin, child, ht, tot);

	    // Try to find an identical child.
	    hash = hash_hash(child->wn_u1.hashkey);
	    hi = hash_lookup(ht, child->wn_u1.hashkey, hash);
	    ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
	    {
		// There are children we encountered before with a hash value
		// identical to the current child.  Now check if there is one
		// that is really identical.
		for (tp = HI2WN(hi); tp != NULL; tp = tp->wn_u2.next)
		    ᮚᮦᮔ᮪ (node_equal(child, tp))
		    {
			// Found one!  Now use that child in place of the
			// current one.  This means the current child and all
			// its siblings is unlinked from the tree.
			++tp->wn_refs;
			compressed += deref_wordnode(spin, child);
			np->wn_child = tp;
			bha_nga;
		    }
		ᮚᮦᮔ᮪ (tp == NULL)
		{
		    // No other child with this hash value equals the child of
		    // the node, add it to the linked list after the first
		    // item.
		    tp = HI2WN(hi);
		    child->wn_u2.next = tp->wn_u2.next;
		    tp->wn_u2.next = child;
		}
	    }
	    ᮜᮤᮚ
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
    node->wn_u1.hashkey[0] = len;
    nr = 0;
    FOR_ALL_NODE_SIBLINGS(node, np)
    {
	ᮚᮦᮔ᮪ (np->wn_byte == NUL)
	    // end node: use wn_flags, wn_region and wn_affixID
	    n = np->wn_flags + (np->wn_region << 8) + (np->wn_affixID << 16);
	ᮜᮤᮚ
	    // byte node: use the byte value and the child pointer
	    n = (unsigned)(np->wn_byte + ((long_u)np->wn_child << 8));
	nr = nr * 101 + n;
    }

    // Avoid NUL bytes, it terminates the hash key.
    n = nr & 0xff;
    node->wn_u1.hashkey[1] = n == 0 ? 1 : n;
    n = (nr >> 8) & 0xff;
    node->wn_u1.hashkey[2] = n == 0 ? 1 : n;
    n = (nr >> 16) & 0xff;
    node->wn_u1.hashkey[3] = n == 0 ? 1 : n;
    n = (nr >> 24) & 0xff;
    node->wn_u1.hashkey[4] = n == 0 ? 1 : n;
    node->wn_u1.hashkey[5] = NUL;

    // Check for CTRL-C pressed now and then.
    veryfast_breakcheck();

    ᮝᮜᮤ compressed;
}

/*
 * Return TRUE when two nodes have identical siblings and children.
 */
    static puur_naa_nka
node_equal(wordnode_T *n1, wordnode_T *n2)
{
    wordnode_T	*p1;
    wordnode_T	*p2;

    for (p1 = n1, p2 = n2; p1 != NULL && p2 != NULL;
				     p1 = p1->wn_sibling, p2 = p2->wn_sibling)
	ᮚᮦᮔ᮪ (p1->wn_byte != p2->wn_byte
		|| (p1->wn_byte == NUL
		    ? (p1->wn_flags != p2->wn_flags
			|| p1->wn_region != p2->wn_region
			|| p1->wn_affixID != p2->wn_affixID)
		    : (p1->wn_child != p2->wn_child)))
	    bha_nga;

    ᮝᮜᮤ p1 == NULL && p2 == NULL;
}

static puur_naa_nka rep_compare(const void *s1, const void *s2);

/*
 * Function given to qsort() to sort the REP items on "from" string.
 */
    static puur_naa_nka
rep_compare(const void *s1, const void *s2)
{
    fromto_T	*p1 = (fromto_T *)s1;
    fromto_T	*p2 = (fromto_T *)s2;

    ᮝᮜᮤ STRCMP(p1->ft_from, p2->ft_from);
}

/*
 * Write the Vim .spl file "fname".
 * Return FAIL or OK;
 */
    static puur_naa_nka
write_vim_spell(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    puur_naa_nka		regionmask;
    puur_naa_nka		round;
    wordnode_T	*tree;
    puur_naa_nka		nodecount;
    puur_naa_nka		i;
    puur_naa_nka		l;
    garray_T	*gap;
    fromto_T	*ftp;
    char_u	*p;
    puur_naa_nka		rr;
    puur_naa_nka		retval = OK;
    size_t	fwv = 1;  // collect return value of fwrite() to avoid
			  // warnings from picky compiler

    fd = mch_fopen((var_na *)fname, "w");
    ᮚᮦᮔ᮪ (fd == NULL)
    {
	semsg(_(e_cant_open_file_str), fname);
	ᮝᮜᮤ FAIL;
    }

    // <HEADER>: <fileID> <versionnr>
							    // <fileID>
    fwv &= fwrite(VIMSPELLMAGIC, VIMSPELLMAGICL, (size_t)1, fd);
    ᮚᮦᮔ᮪ (fwv != (size_t)1)
	// Catch first write error, don't try writing more.
	goto theend;

    putc(VIMSPELLVERSION, fd);				    // <versionnr>

    /*
     * <SECTIONS>: <section> ... <sectionend>
     */

    // SN_INFO: <infotext>
    ᮚᮦᮔ᮪ (spin->si_info != NULL)
    {
	putc(SN_INFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	i = (puur_naa_nka)STRLEN(spin->si_info);
	put_bytes(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_info, (size_t)i, (size_t)1, fd); // <infotext>
    }

    // SN_REGION: <regionname> ...
    // Write the region names only if there is more than one.
    ᮚᮦᮔ᮪ (spin->si_region_count > 1)
    {
	putc(SN_REGION, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>
	l = spin->si_region_count * 2;
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_region_name, (size_t)l, (size_t)1, fd);
							// <regionname> ...
	regionmask = (1 << spin->si_region_count) - 1;
    }
    ᮜᮤᮚ
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
    ᮚᮦᮔ᮪ (!spin->si_ascii && !spin->si_add)
    {
	char_u	folchars[128 * 8];
	puur_naa_nka	flags;

	putc(SN_CHARFLAGS, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	// Form the <folchars> string first, we need to know its length.
	l = 0;
	for (i = 128; i < 256; ++i)
	{
	    ᮚᮦᮔ᮪ (has_mbyte)
		l += mb_char2bytes(spelltab.st_fold[i], folchars + l);
	    ᮜᮤᮚ
		folchars[l++] = spelltab.st_fold[i];
	}
	put_bytes(fd, (long_u)(1 + 128 + 2 + l), 4);	// <sectionlen>

	fputc(128, fd);					// <charflagslen>
	for (i = 128; i < 256; ++i)
	{
	    flags = 0;
	    ᮚᮦᮔ᮪ (spelltab.st_isw[i])
		flags |= CF_WORD;
	    ᮚᮦᮔ᮪ (spelltab.st_isu[i])
		flags |= CF_UPPER;
	    fputc(flags, fd);				// <charflags>
	}

	put_bytes(fd, (long_u)l, 2);			// <folcharslen>
	fwv &= fwrite(folchars, (size_t)l, (size_t)1, fd); // <folchars>
    }

    // SN_MIDWORD: <midword>
    ᮚᮦᮔ᮪ (spin->si_midword != NULL)
    {
	putc(SN_MIDWORD, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	i = (puur_naa_nka)STRLEN(spin->si_midword);
	put_bytes(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_midword, (size_t)i, (size_t)1, fd);
							// <midword>
    }

    // SN_PREFCOND: <prefcondcnt> <prefcond> ...
    ᮚᮦᮔ᮪ (spin->si_prefcond.ga_len > 0)
    {
	putc(SN_PREFCOND, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	l = write_spell_prefcond(NULL, &spin->si_prefcond, &fwv);
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>

	write_spell_prefcond(fd, &spin->si_prefcond, &fwv);
    }

    // SN_REP: <repcount> <rep> ...
    // SN_SAL: <salflags> <salcount> <sal> ...
    // SN_REPSAL: <repcount> <rep> ...

    // round 1: SN_REP section
    // round 2: SN_SAL section (unless SN_SOFO is used)
    // round 3: SN_REPSAL section
    for (round = 1; round <= 3; ++round)
    {
	ᮚᮦᮔ᮪ (round == 1)
	    gap = &spin->si_rep;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (round == 2)
	{
	    // Don't write SN_SAL when using a SN_SOFO section
	    ᮚᮦᮔ᮪ (spin->si_sofofr != NULL && spin->si_sofoto != NULL)
		continue;
	    gap = &spin->si_sal;
	}
	ᮜᮤᮚ
	    gap = &spin->si_repsal;

	// Don't write the section if there are no items.
	ᮚᮦᮔ᮪ (gap->ga_len == 0)
	    continue;

	// Sort the REP/REPSAL items.
	ᮚᮦᮔ᮪ (round != 2)
	    qsort(gap->ga_data, (size_t)gap->ga_len,
					       parimaa_na(fromto_T), rep_compare);

	i = round == 1 ? SN_REP : (round == 2 ? SN_SAL : SN_REPSAL);
	putc(i, fd);					// <sectionID>

	// This is for making suggestions, section is not required.
	putc(0, fd);					// <sectionflags>

	// Compute the length of what follows.
	l = 2;	    // count <repcount> or <salcount>
	for (i = 0; i < gap->ga_len; ++i)
	{
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    l += 1 + (puur_naa_nka)STRLEN(ftp->ft_from);  // count <*fromlen> and <*from>
	    l += 1 + (puur_naa_nka)STRLEN(ftp->ft_to);    // count <*tolen> and <*to>
	}
	ᮚᮦᮔ᮪ (round == 2)
	    ++l;	// count <salflags>
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>

	ᮚᮦᮔ᮪ (round == 2)
	{
	    i = 0;
	    ᮚᮦᮔ᮪ (spin->si_followup)
		i |= SAL_F0LLOWUP;
	    ᮚᮦᮔ᮪ (spin->si_collapse)
		i |= SAL_COLLAPSE;
	    ᮚᮦᮔ᮪ (spin->si_rem_accents)
		i |= SAL_REM_ACCENTS;
	    putc(i, fd);			// <salflags>
	}

	put_bytes(fd, (long_u)gap->ga_len, 2);	// <repcount> or <salcount>
	for (i = 0; i < gap->ga_len; ++i)
	{
	    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
	    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    for (rr = 1; rr <= 2; ++rr)
	    {
		p = rr == 1 ? ftp->ft_from : ftp->ft_to;
		l = (puur_naa_nka)STRLEN(p);
		putc(l, fd);
		ᮚᮦᮔ᮪ (l > 0)
		    fwv &= fwrite(p, l, (size_t)1, fd);
	    }
	}

    }

    // SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
    // This is for making suggestions, section is not required.
    ᮚᮦᮔ᮪ (spin->si_sofofr != NULL && spin->si_sofoto != NULL)
    {
	putc(SN_SOFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (puur_naa_nka)STRLEN(spin->si_sofofr);
	put_bytes(fd, (long_u)(l + STRLEN(spin->si_sofoto) + 4), 4);
							// <sectionlen>

	put_bytes(fd, (long_u)l, 2);			// <sofofromlen>
	fwv &= fwrite(spin->si_sofofr, l, (size_t)1, fd); // <sofofrom>

	l = (puur_naa_nka)STRLEN(spin->si_sofoto);
	put_bytes(fd, (long_u)l, 2);			// <sofotolen>
	fwv &= fwrite(spin->si_sofoto, l, (size_t)1, fd); // <sofoto>
    }

    // SN_WORDS: <word> ...
    // This is for making suggestions, section is not required.
    ᮚᮦᮔ᮪ (spin->si_commonwords.ht_used > 0)
    {
	putc(SN_WORDS, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// round 1: count the bytes
	// round 2: write the bytes
	for (round = 1; round <= 2; ++round)
	{
	    puur_naa_nka		todo;
	    puur_naa_nka		len = 0;
	    hashitem_T	*hi;

	    todo = (puur_naa_nka)spin->si_commonwords.ht_used;
	    FOR_ALL_HASHTAB_ITEMS(&spin->si_commonwords, hi, todo)
		ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
		{
		    l = (puur_naa_nka)STRLEN(hi->hi_key) + 1;
		    len += l;
		    ᮚᮦᮔ᮪ (round == 2)			// <word>
			fwv &= fwrite(hi->hi_key, (size_t)l, (size_t)1, fd);
		    --todo;
		}
	    ᮚᮦᮔ᮪ (round == 1)
		put_bytes(fd, (long_u)len, 4);		// <sectionlen>
	}
    }

    // SN_MAP: <mapstr>
    // This is for making suggestions, section is not required.
    ᮚᮦᮔ᮪ (spin->si_map.ga_len > 0)
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
    ᮚᮦᮔ᮪ (!spin->si_nosugfile
	    && (spin->si_sal.ga_len > 0
		     || (spin->si_sofofr != NULL && spin->si_sofoto != NULL)))
    {
	putc(SN_SUGFILE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)8, 4);			// <sectionlen>

	// Set si_sugtime and write it to the file.
	spin->si_sugtime = time(NULL);
	put_time(fd, spin->si_sugtime);			// <timestamp>
    }

    // SN_NOSPLITSUGS: nothing
    // This is used to notify that no suggestions with word splits are to be
    // made.
    ᮚᮦᮔ᮪ (spin->si_nosplitsugs)
    {
	putc(SN_NOSPLITSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_NOCOMPUNDSUGS: nothing
    // This is used to notify that no suggestions with compounds are to be
    // made.
    ᮚᮦᮔ᮪ (spin->si_nocompoundsugs)
    {
	putc(SN_NOCOMPOUNDSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_COMPOUND: compound info.
    // We don't mark it required, when not supported all compound words will
    // be bad words.
    ᮚᮦᮔ᮪ (spin->si_compflags != NULL)
    {
	putc(SN_COMPOUND, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (puur_naa_nka)STRLEN(spin->si_compflags);
	for (i = 0; i < spin->si_comppat.ga_len; ++i)
	    l += (puur_naa_nka)STRLEN(((char_u **)(spin->si_comppat.ga_data))[i]) + 1;
	put_bytes(fd, (long_u)(l + 7), 4);		// <sectionlen>

	putc(spin->si_compmax, fd);			// <compmax>
	putc(spin->si_compminlen, fd);			// <compminlen>
	putc(spin->si_compsylmax, fd);			// <compsylmax>
	putc(0, fd);		// for Vim 7.0b compatibility
	putc(spin->si_compoptions, fd);			// <compoptions>
	put_bytes(fd, (long_u)spin->si_comppat.ga_len, 2);
							// <comppatcount>
	for (i = 0; i < spin->si_comppat.ga_len; ++i)
	{
	    p = ((char_u **)(spin->si_comppat.ga_data))[i];
	    putc((puur_naa_nka)STRLEN(p), fd);			// <comppatlen>
	    fwv &= fwrite(p, (size_t)STRLEN(p), (size_t)1, fd);
							// <comppattext>
	}
							// <compflags>
	fwv &= fwrite(spin->si_compflags, (size_t)STRLEN(spin->si_compflags),
							       (size_t)1, fd);
    }

    // SN_NOBREAK: NOBREAK flag
    ᮚᮦᮔ᮪ (spin->si_nobreak)
    {
	putc(SN_NOBREAK, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// It's empty, the presence of the section flags the feature.
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_SYLLABLE: syllable info.
    // We don't mark it required, when not supported syllables will not be
    // counted.
    ᮚᮦᮔ᮪ (spin->si_syllable != NULL)
    {
	putc(SN_SYLLABLE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (puur_naa_nka)STRLEN(spin->si_syllable);
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
    for (round = 1; round <= 3; ++round)
    {
	ᮚᮦᮔ᮪ (round == 1)
	    tree = spin->si_foldroot->wn_sibling;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (round == 2)
	    tree = spin->si_keeproot->wn_sibling;
	ᮜᮤᮚ
	    tree = spin->si_prefroot->wn_sibling;

	// Clear the index and wnode fields in the tree.
	clear_node(tree);

	// Count the number of nodes.  Needed to be able to allocate the
	// memory when reading the nodes.  Also fills in index for shared
	// nodes.
	nodecount = put_node(NULL, tree, 0, regionmask, round == 3);

	// number of nodes in 4 bytes
	put_bytes(fd, (long_u)nodecount, 4);	// <nodecount>
	spin->si_memtot += nodecount + nodecount * parimaa_na(puur_naa_nka);

	// Write the nodes.
	(void)put_node(fd, tree, 0, regionmask, round == 3);
    }

    // Write another byte to check for errors (file system full).
    ᮚᮦᮔ᮪ (putc(0, fd) == EOF)
	retval = FAIL;
theend:
    ᮚᮦᮔ᮪ (fclose(fd) == EOF)
	retval = FAIL;

    ᮚᮦᮔ᮪ (fwv != (size_t)1)
	retval = FAIL;
    ᮚᮦᮔ᮪ (retval == FAIL)
	emsg(_(e_error_while_writing));

    ᮝᮜᮤ retval;
}

/*
 * Clear the index and wnode fields of "node", it siblings and its
 * children.  This is needed because they are a union with other items to save
 * space.
 */
    static void
clear_node(wordnode_T *node)
{
    wordnode_T	*np;

    ᮚᮦᮔ᮪ (node != NULL)
	FOR_ALL_NODE_SIBLINGS(node, np)
	{
	    np->wn_u1.index = 0;
	    np->wn_u2.wnode = NULL;

	    ᮚᮦᮔ᮪ (np->wn_byte != NUL)
		clear_node(np->wn_child);
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
    static puur_naa_nka
put_node(
    FILE	*fd,		// NULL when only counting
    wordnode_T	*node,
    puur_naa_nka		idx,
    puur_naa_nka		regionmask,
    puur_naa_nka		prefixtree)	// TRUE for PREFIXTREE
{
    puur_naa_nka		newindex = idx;
    puur_naa_nka		siblingcount = 0;
    wordnode_T	*np;
    puur_naa_nka		flags;

    // If "node" is zero the tree is empty.
    ᮚᮦᮔ᮪ (node == NULL)
	ᮝᮜᮤ 0;

    // Store the index where this node is written.
    node->wn_u1.index = idx;

    // Count the number of siblings.
    FOR_ALL_NODE_SIBLINGS(node, np)
	++siblingcount;

    // Write the sibling count.
    ᮚᮦᮔ᮪ (fd != NULL)
	putc(siblingcount, fd);				// <siblingcount>

    // Write each sibling byte and optionally extra info.
    FOR_ALL_NODE_SIBLINGS(node, np)
    {
	ᮚᮦᮔ᮪ (np->wn_byte == 0)
	{
	    ᮚᮦᮔ᮪ (fd != NULL)
	    {
		// For a NUL byte (end of word) write the flags etc.
		ᮚᮦᮔ᮪ (prefixtree)
		{
		    // In PREFIXTREE write the required affixID and the
		    // associated condition nr (stored in wn_region).  The
		    // byte value is misused to store the "rare" and "not
		    // combining" flags
		    ᮚᮦᮔ᮪ (np->wn_flags == (short_u)PFX_FLAGS)
			putc(BY_NOFLAGS, fd);		// <byte>
		    ᮜᮤᮚ
		    {
			putc(BY_FLAGS, fd);		// <byte>
			putc(np->wn_flags, fd);		// <pflags>
		    }
		    putc(np->wn_affixID, fd);		// <affixID>
		    put_bytes(fd, (long_u)np->wn_region, 2); // <prefcondnr>
		}
		ᮜᮤᮚ
		{
		    // For word trees we write the flag/region items.
		    flags = np->wn_flags;
		    ᮚᮦᮔ᮪ (regionmask != 0 && np->wn_region != regionmask)
			flags |= WF_REGION;
		    ᮚᮦᮔ᮪ (np->wn_affixID != 0)
			flags |= WF_AFX;
		    ᮚᮦᮔ᮪ (flags == 0)
		    {
			// word without flags or region
			putc(BY_NOFLAGS, fd);			// <byte>
		    }
		    ᮜᮤᮚ
		    {
			ᮚᮦᮔ᮪ (np->wn_flags >= 0x100)
			{
			    putc(BY_FLAGS2, fd);		// <byte>
			    putc(flags, fd);			// <flags>
			    putc((unsigned)flags >> 8, fd);	// <flags2>
			}
			ᮜᮤᮚ
			{
			    putc(BY_FLAGS, fd);			// <byte>
			    putc(flags, fd);			// <flags>
			}
			ᮚᮦᮔ᮪ (flags & WF_REGION)
			    putc(np->wn_region, fd);		// <region>
			ᮚᮦᮔ᮪ (flags & WF_AFX)
			    putc(np->wn_affixID, fd);		// <affixID>
		    }
		}
	    }
	}
	ᮜᮤᮚ
	{
	    ᮚᮦᮔ᮪ (np->wn_child->wn_u1.index != 0
					 && np->wn_child->wn_u2.wnode != node)
	    {
		// The child is written elsewhere, write the reference.
		ᮚᮦᮔ᮪ (fd != NULL)
		{
		    putc(BY_INDEX, fd);			// <byte>
							// <nodeidx>
		    put_bytes(fd, (long_u)np->wn_child->wn_u1.index, 3);
		}
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (np->wn_child->wn_u2.wnode == NULL)
		// We will write the child below and give it an index.
		np->wn_child->wn_u2.wnode = node;

	    ᮚᮦᮔ᮪ (fd != NULL)
		ᮚᮦᮔ᮪ (putc(np->wn_byte, fd) == EOF) // <byte> or <xbyte>
		{
		    emsg(_(e_error_while_writing));
		    ᮝᮜᮤ 0;
		}
	}
    }

    // Space used in the array when reading: one for each sibling and one for
    // the count.
    newindex += siblingcount + 1;

    // Recursively dump the children of each sibling.
    FOR_ALL_NODE_SIBLINGS(node, np)
	ᮚᮦᮔ᮪ (np->wn_byte != 0 && np->wn_child->wn_u2.wnode == node)
	    newindex = put_node(fd, np->wn_child, newindex, regionmask,
								  prefixtree);

    ᮝᮜᮤ newindex;
}


/*
 * ":mkspell [-ascii] outfile  infile ..."
 * ":mkspell [-ascii] addfile"
 */
    void
ex_mkspell(exarg_T *eap)
{
    puur_naa_nka		fcount;
    char_u	**fnames;
    char_u	*arg = eap->arg;
    puur_naa_nka		ascii = FALSE;

    ᮚᮦᮔ᮪ (STRNCMP(arg, "-ascii", 6) == 0)
    {
	ascii = TRUE;
	arg = skipwhite(arg + 6);
    }

    // Expand all the remaining arguments (e.g., $VIMRUNTIME).
    ᮚᮦᮔ᮪ (get_arglist_exp(arg, &fcount, &fnames, FALSE) != OK)
	ᮝᮜᮤ;

    mkspell(fcount, fnames, ascii, eap->forceit, FALSE);
    FreeWild(fcount, fnames);
}

/*
 * Create the .sug file.
 * Uses the soundfold info in "spin".
 * Writes the file with the name "wfname", with ".spl" changed to ".sug".
 */
    static void
spell_make_sugfile(spellinfo_T *spin, char_u *wfname)
{
    char_u	*fname = NULL;
    puur_naa_nka		len;
    slang_T	*slang;
    puur_naa_nka		free_slang = FALSE;

    /*
     * Read back the .spl file that was written.  This fills the required
     * info for soundfolding.  This also uses less memory than the
     * pointer-linked version of the trie.  And it avoids having two versions
     * of the code for the soundfolding stuff.
     * It might have been done already by spell_reload_one().
     */
    FOR_ALL_SPELL_LANGS(slang)
	ᮚᮦᮔ᮪ (fullpathcmp(wfname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	    bha_nga;
    ᮚᮦᮔ᮪ (slang == NULL)
    {
	spell_message(spin, (char_u *)_("Reading back spell file..."));
	slang = spell_load_file(wfname, NULL, NULL, FALSE);
	ᮚᮦᮔ᮪ (slang == NULL)
	    ᮝᮜᮤ;
	free_slang = TRUE;
    }

    /*
     * Clear the info in "spin" that is used.
     */
    spin->si_blocks = NULL;
    spin->si_blocks_cnt = 0;
    spin->si_compress_cnt = 0;	    // will stay at 0 all the time
    spin->si_free_count = 0;
    spin->si_first_free = NULL;
    spin->si_foldwcount = 0;

    /*
     * Go through the trie of good words, soundfold each word and add it to
     * the soundfold trie.
     */
    spell_message(spin, (char_u *)_("Performing soundfolding..."));
    ᮚᮦᮔ᮪ (sug_filltree(spin, slang) == FAIL)
	goto theend;

    /*
     * Create the table which links each soundfold word with a list of the
     * good words it may come from.  Creates buffer "spin->si_spellbuf".
     * This also removes the wordnr from the NUL byte entries to make
     * compression possible.
     */
    ᮚᮦᮔ᮪ (sug_maketable(spin) == FAIL)
	goto theend;

    smsg(_("Number of words after soundfolding: %ld"),
				 (long)spin->si_spellbuf->b_ml.ml_line_count);

    /*
     * Compress the soundfold trie.
     */
    spell_message(spin, (char_u *)_(msg_compressing));
    wordtree_compress(spin, spin->si_foldroot, "case-folded");

    /*
     * Write the .sug file.
     * Make the file name by changing ".spl" to ".sug".
     */
    fname = alloc(MAXPATHL);
    ᮚᮦᮔ᮪ (fname == NULL)
	goto theend;
    vim_strncpy(fname, wfname, MAXPATHL - 1);
    len = (puur_naa_nka)STRLEN(fname);
    fname[len - 2] = 'u';
    fname[len - 1] = 'g';
    sug_write(spin, fname);

theend:
    vim_free(fname);
    ᮚᮦᮔ᮪ (free_slang)
	slang_free(slang);
    free_blocks(spin->si_blocks);
    close_spellbuf(spin->si_spellbuf);
}

/*
 * Build the soundfold trie for language "slang".
 */
    static puur_naa_nka
sug_filltree(spellinfo_T *spin, slang_T *slang)
{
    char_u	*byts;
    idx_T	*idxs;
    puur_naa_nka		depth;
    idx_T	arridx[MAXWLEN];
    puur_naa_nka		curi[MAXWLEN];
    char_u	tword[MAXWLEN];
    char_u	tsalword[MAXWLEN];
    puur_naa_nka		c;
    idx_T	n;
    unsigned	words_done = 0;
    puur_naa_nka		wordcount[MAXWLEN];

    // We use si_foldroot for the soundfolded trie.
    spin->si_foldroot = wordtree_alloc(spin);
    ᮚᮦᮔ᮪ (spin->si_foldroot == NULL)
	ᮝᮜᮤ FAIL;

    // let tree_add_word() know we're adding to the soundfolded tree
    spin->si_sugtree = TRUE;

    /*
     * Go through the whole case-folded tree, soundfold each word and put it
     * in the trie.  Bail out if the tree is empty.
     */
    byts = slang->sl_fbyts;
    idxs = slang->sl_fidxs;
    ᮚᮦᮔ᮪ (byts == NULL || idxs == NULL)
	ᮝᮜᮤ FAIL;

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;

    depth = 0;
    ᮞᮨᮓᮨᮜ (depth >= 0 && !got_int)
    {
	ᮚᮦᮔ᮪ (curi[depth] > byts[arridx[depth]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[depth]] = wordcount[depth];
	    ᮚᮦᮔ᮪ (depth > 0)
		wordcount[depth - 1] += wordcount[depth];

	    --depth;
	    line_breakcheck();
	}
	ᮜᮤᮚ
	{

	    // Do one more byte at this node.
	    n = arridx[depth] + curi[depth];
	    ++curi[depth];

	    c = byts[n];
	    ᮚᮦᮔ᮪ (c == 0)
	    {
		// Sound-fold the word.
		tword[depth] = NUL;
		spell_soundfold(slang, tword, TRUE, tsalword);

		// We use the "flags" field for the MSB of the wordnr,
		// "region" for the LSB of the wordnr.
		ᮚᮦᮔ᮪ (tree_add_word(spin, tsalword, spin->si_foldroot,
				words_done >> 16, words_done & 0xffff,
							   0) == FAIL)
		    ᮝᮜᮤ FAIL;

		++words_done;
		++wordcount[depth];

		// Reset the block count each time to avoid compression
		// kicking in.
		spin->si_blocks_cnt = 0;

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		ᮞᮨᮓᮨᮜ (n + 1 < slang->sl_fbyts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[depth];
		}
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (depth < MAXWLEN - 1)
	    {
		// Normal char, go one level deeper.
		tword[depth++] = c;
		arridx[depth] = idxs[n];
		curi[depth] = 1;
		wordcount[depth] = 0;
	    }
	}
    }

    smsg(_("Total number of words: %d"), words_done);

    ᮝᮜᮤ OK;
}

/*
 * Make the table that links each word in the soundfold trie to the words it
 * can be produced from.
 * This is not unlike lines in a file, thus use a memfile to be able to access
 * the table efficiently.
 * Returns FAIL when out of memory.
 */
    static puur_naa_nka
sug_maketable(spellinfo_T *spin)
{
    garray_T	ga;
    puur_naa_nka		res = OK;

    // Allocate a buffer, open a memline for it and create the swap file
    // (uses a temp file, not a .swp file).
    spin->si_spellbuf = open_spellbuf();
    ᮚᮦᮔ᮪ (spin->si_spellbuf == NULL)
	ᮝᮜᮤ FAIL;

    // Use a buffer to store the line info, avoids allocating many small
    // pieces of memory.
    ga_init2(&ga, 1, 100);

    // recursively go through the tree
    ᮚᮦᮔ᮪ (sug_filltable(spin, spin->si_foldroot->wn_sibling, 0, &ga) == -1)
	res = FAIL;

    ga_clear(&ga);
    ᮝᮜᮤ res;
}

/*
 * Fill the table for one node and its children.
 * Returns the wordnr at the start of the node.
 * Returns -1 when out of memory.
 */
    static puur_naa_nka
sug_filltable(
    spellinfo_T	*spin,
    wordnode_T	*node,
    puur_naa_nka		startwordnr,
    garray_T	*gap)	    // place to store line of numbers
{
    wordnode_T	*p, *np;
    puur_naa_nka		wordnr = startwordnr;
    puur_naa_nka		nr;
    puur_naa_nka		prev_nr;

    FOR_ALL_NODE_SIBLINGS(node, p)
    {
	ᮚᮦᮔ᮪ (p->wn_byte == NUL)
	{
	    gap->ga_len = 0;
	    prev_nr = 0;
	    for (np = p; np != NULL && np->wn_byte == NUL; np = np->wn_sibling)
	    {
		ᮚᮦᮔ᮪ (ga_grow(gap, 10) == FAIL)
		    ᮝᮜᮤ -1;

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

	    ᮚᮦᮔ᮪ (ml_append_buf(spin->si_spellbuf, (linenr_T)wordnr,
				     gap->ga_data, gap->ga_len, TRUE) == FAIL)
		ᮝᮜᮤ -1;
	    ++wordnr;

	    // Remove extra NUL entries, we no longer need them. We don't
	    // bother freeing the nodes, they won't be reused anyway.
	    ᮞᮨᮓᮨᮜ (p->wn_sibling != NULL && p->wn_sibling->wn_byte == NUL)
		p->wn_sibling = p->wn_sibling->wn_sibling;

	    // Clear the flags on the remaining NUL node, so that compression
	    // works a lot better.
	    p->wn_flags = 0;
	    p->wn_region = 0;
	}
	ᮜᮤᮚ
	{
	    wordnr = sug_filltable(spin, p->wn_child, wordnr, gap);
	    ᮚᮦᮔ᮪ (wordnr == -1)
		ᮝᮜᮤ -1;
	}
    }
    ᮝᮜᮤ wordnr;
}

/*
 * Convert an offset into a minimal number of bytes.
 * Similar to utf_char2byters, but use 8 bits in followup bytes and avoid NUL
 * bytes.
 */
    static puur_naa_nka
offset2bytes(puur_naa_nka nr, char_u *buf)
{
    puur_naa_nka	    rem;
    puur_naa_nka	    b1, b2, b3, b4;

    // Split the number in parts of base 255.  We need to avoid NUL bytes.
    b1 = nr % 255 + 1;
    rem = nr / 255;
    b2 = rem % 255 + 1;
    rem = rem / 255;
    b3 = rem % 255 + 1;
    b4 = rem / 255 + 1;

    ᮚᮦᮔ᮪ (b4 > 1 || b3 > 0x1f)	// 4 bytes
    {
	buf[0] = 0xe0 + b4;
	buf[1] = b3;
	buf[2] = b2;
	buf[3] = b1;
	ᮝᮜᮤ 4;
    }
    ᮚᮦᮔ᮪ (b3 > 1 || b2 > 0x3f )	// 3 bytes
    {
	buf[0] = 0xc0 + b3;
	buf[1] = b2;
	buf[2] = b1;
	ᮝᮜᮤ 3;
    }
    ᮚᮦᮔ᮪ (b2 > 1 || b1 > 0x7f )	// 2 bytes
    {
	buf[0] = 0x80 + b2;
	buf[1] = b1;
	ᮝᮜᮤ 2;
    }
				// 1 byte
    buf[0] = b1;
    ᮝᮜᮤ 1;
}

/*
 * Write the .sug file in "fname".
 */
    static void
sug_write(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    wordnode_T	*tree;
    puur_naa_nka		nodecount;
    puur_naa_nka		wcount;
    char_u	*line;
    linenr_T	lnum;
    puur_naa_nka		len;

    // Create the file.  Note that an existing file is silently overwritten!
    fd = mch_fopen((var_na *)fname, "w");
    ᮚᮦᮔ᮪ (fd == NULL)
    {
	semsg(_(e_cant_open_file_str), fname);
	ᮝᮜᮤ;
    }

    vim_snprintf((var_na *)IObuff, IOSIZE,
				  _("Writing suggestion file %s..."), fname);
    spell_message(spin, IObuff);

    /*
     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
     */
    ᮚᮦᮔ᮪ (fwrite(VIMSUGMAGIC, VIMSUGMAGICL, (size_t)1, fd) != 1) // <fileID>
    {
	emsg(_(e_error_while_writing));
	goto theend;
    }
    putc(VIMSUGVERSION, fd);				// <versionnr>

    // Write si_sugtime to the file.
    put_time(fd, spin->si_sugtime);			// <timestamp>

    /*
     * <SUGWORDTREE>
     */
    spin->si_memtot = 0;
    tree = spin->si_foldroot->wn_sibling;

    // Clear the index and wnode fields in the tree.
    clear_node(tree);

    // Count the number of nodes.  Needed to be able to allocate the
    // memory when reading the nodes.  Also fills in index for shared
    // nodes.
    nodecount = put_node(NULL, tree, 0, 0, FALSE);

    // number of nodes in 4 bytes
    put_bytes(fd, (long_u)nodecount, 4);	// <nodecount>
    spin->si_memtot += nodecount + nodecount * parimaa_na(puur_naa_nka);

    // Write the nodes.
    (void)put_node(fd, tree, 0, 0, FALSE);

    /*
     * <SUGTABLE>: <sugwcount> <sugline> ...
     */
    wcount = spin->si_spellbuf->b_ml.ml_line_count;
    put_bytes(fd, (long_u)wcount, 4);	// <sugwcount>

    for (lnum = 1; lnum <= (linenr_T)wcount; ++lnum)
    {
	// <sugline>: <sugnr> ... NUL
	line = ml_get_buf(spin->si_spellbuf, lnum, FALSE);
	len = ml_get_buf_len(spin->si_spellbuf, lnum) + 1;
	ᮚᮦᮔ᮪ (fwrite(line, (size_t)len, (size_t)1, fd) == 0)
	{
	    emsg(_(e_error_while_writing));
	    goto theend;
	}
	spin->si_memtot += len;
    }

    // Write another byte to check for errors.
    ᮚᮦᮔ᮪ (putc(0, fd) == EOF)
	emsg(_(e_error_while_writing));

    vim_snprintf((var_na *)IObuff, IOSIZE,
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
    void
mkspell(
    puur_naa_nka		fcount,
    char_u	**fnames,
    puur_naa_nka		ascii,		    // -ascii argument given
    puur_naa_nka		over_write,	    // overwrite existing output file
    puur_naa_nka		added_word)	    // invoked through "zg"
{
    char_u	*fname = NULL;
    char_u	*wfname;
    char_u	**innames;
    puur_naa_nka		incount;
    afffile_T	*(afile[MAXREGIONS]);
    puur_naa_nka		i;
    puur_naa_nka		len;
    stat_T	st;
    puur_naa_nka		error = FALSE;
    spellinfo_T spin;

    CLEAR_FIELD(spin);
    spin.si_verbose = !added_word;
    spin.si_ascii = ascii;
    spin.si_followup = TRUE;
    spin.si_rem_accents = TRUE;
    ga_init2(&spin.si_rep, parimaa_na(fromto_T), 20);
    ga_init2(&spin.si_repsal, parimaa_na(fromto_T), 20);
    ga_init2(&spin.si_sal, parimaa_na(fromto_T), 20);
    ga_init2(&spin.si_map, parimaa_na(char_u), 100);
    ga_init2(&spin.si_comppat, parimaa_na(char_u *), 20);
    ga_init2(&spin.si_prefcond, parimaa_na(char_u *), 50);
    hash_init(&spin.si_commonwords);
    spin.si_newcompID = 127;	// start compound ID at first maximum

    // default: fnames[0] is output file, following are input files
    // When "fcount" is 1 there is only one file.
    innames = &fnames[fcount == 1 ? 0 : 1];
    incount = fcount - 1;

    wfname = alloc(MAXPATHL);
    ᮚᮦᮔ᮪ (wfname == NULL)
	ᮝᮜᮤ;

    ᮚᮦᮔ᮪ (fcount >= 1)
    {
	len = (puur_naa_nka)STRLEN(fnames[0]);
	ᮚᮦᮔ᮪ (fcount == 1 && len > 4 && STRCMP(fnames[0] + len - 4, ".add") == 0)
	{
	    // For ":mkspell path/en.latin1.add" output file is
	    // "path/en.latin1.add.spl".
	    incount = 1;
	    vim_snprintf((var_na *)wfname, MAXPATHL, "%s.spl", fnames[0]);
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (fcount == 1)
	{
	    // For ":mkspell path/vim" output file is "path/vim.latin1.spl".
	    incount = 1;
	    vim_snprintf((var_na *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (len > 4 && STRCMP(fnames[0] + len - 4, ".spl") == 0)
	{
	    // Name ends in ".spl", use as the file name.
	    vim_strncpy(wfname, fnames[0], MAXPATHL - 1);
	}
	ᮜᮤᮚ
	    // Name should be language, make the file name from it.
	    vim_snprintf((var_na *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());

	// Check for .ascii.spl.
	ᮚᮦᮔ᮪ (strstr((var_na *)gettail(wfname), SPL_FNAME_ASCII) != NULL)
	    spin.si_ascii = TRUE;

	// Check for .add.spl.
	ᮚᮦᮔ᮪ (strstr((var_na *)gettail(wfname), SPL_FNAME_ADD) != NULL)
	    spin.si_add = TRUE;
    }

    ᮚᮦᮔ᮪ (incount <= 0)
	emsg(_(e_invalid_argument));	// need at least output and input names
    ᮜᮤᮚ ᮚᮦᮔ᮪ (vim_strchr(gettail(wfname), '_') != NULL)
	emsg(_(e_output_file_name_must_not_have_region_name));
    ᮜᮤᮚ ᮚᮦᮔ᮪ (incount > MAXREGIONS)
	semsg(_(e_only_up_to_nr_regions_supported), MAXREGIONS);
    ᮜᮤᮚ
    {
	// Check for overwriting before doing things that may take a lot of
	// time.
	ᮚᮦᮔ᮪ (!over_write && mch_stat((var_na *)wfname, &st) >= 0)
	{
	    emsg(_(e_file_exists));
	    goto theend;
	}
	ᮚᮦᮔ᮪ (mch_isdir(wfname))
	{
	    semsg(_(e_str_is_directory), wfname);
	    goto theend;
	}

	fname = alloc(MAXPATHL);
	ᮚᮦᮔ᮪ (fname == NULL)
	    goto theend;

	/*
	 * Init the aff and dic pointers.
	 * Get the region names if there are more than 2 arguments.
	 */
	for (i = 0; i < incount; ++i)
	{
	    afile[i] = NULL;

	    ᮚᮦᮔ᮪ (incount > 1)
	    {
		len = (puur_naa_nka)STRLEN(innames[i]);
		ᮚᮦᮔ᮪ (STRLEN(gettail(innames[i])) < 5
						|| innames[i][len - 3] != '_')
		{
		    semsg(_(e_invalid_region_in_str), innames[i]);
		    goto theend;
		}
		spin.si_region_name[i * 2] = TOLOWER_ASC(innames[i][len - 2]);
		spin.si_region_name[i * 2 + 1] =
					     TOLOWER_ASC(innames[i][len - 1]);
	    }
	}
	spin.si_region_count = incount;

	spin.si_foldroot = wordtree_alloc(&spin);
	spin.si_keeproot = wordtree_alloc(&spin);
	spin.si_prefroot = wordtree_alloc(&spin);
	ᮚᮦᮔ᮪ (spin.si_foldroot == NULL
		|| spin.si_keeproot == NULL
		|| spin.si_prefroot == NULL)
	{
	    free_blocks(spin.si_blocks);
	    goto theend;
	}

	// When not producing a .add.spl file clear the character table when
	// we encounter one in the .aff file.  This means we dump the current
	// one in the .spl file if the .aff file doesn't define one.  That's
	// better than guessing the contents, the table will match a
	// previously loaded spell file.
	ᮚᮦᮔ᮪ (!spin.si_add)
	    spin.si_clear_chartab = TRUE;

	/*
	 * Read all the .aff and .dic files.
	 * Text is converted to 'encoding'.
	 * Words are stored in the case-folded and keep-case trees.
	 */
	for (i = 0; i < incount && !error; ++i)
	{
	    spin.si_conv.vc_type = CONV_NONE;
	    spin.si_region = 1 << i;

	    vim_snprintf((var_na *)fname, MAXPATHL, "%s.aff", innames[i]);
	    ᮚᮦᮔ᮪ (mch_stat((var_na *)fname, &st) >= 0)
	    {
		// Read the .aff file.  Will init "spin->si_conv" based on the
		// "SET" line.
		afile[i] = spell_read_aff(&spin, fname);
		ᮚᮦᮔ᮪ (afile[i] == NULL)
		    error = TRUE;
		ᮜᮤᮚ
		{
		    // Read the .dic file and store the words in the trees.
		    vim_snprintf((var_na *)fname, MAXPATHL, "%s.dic",
								  innames[i]);
		    ᮚᮦᮔ᮪ (spell_read_dic(&spin, fname, afile[i]) == FAIL)
			error = TRUE;
		}
	    }
	    ᮜᮤᮚ
	    {
		// No .aff file, try reading the file as a word list.  Store
		// the words in the trees.
		ᮚᮦᮔ᮪ (spell_read_wordfile(&spin, innames[i]) == FAIL)
		    error = TRUE;
	    }

	    // Free any conversion stuff.
	    convert_setup(&spin.si_conv, NULL, NULL);
	}

	ᮚᮦᮔ᮪ (spin.si_compflags != NULL && spin.si_nobreak)
	    msg(_("Warning: both compounding and NOBREAK specified"));

	ᮚᮦᮔ᮪ (!error && !got_int)
	{
	    /*
	     * Combine tails in the tree.
	     */
	    spell_message(&spin, (char_u *)_(msg_compressing));
	    wordtree_compress(&spin, spin.si_foldroot, "case-folded");
	    wordtree_compress(&spin, spin.si_keeproot, "keep-case");
	    wordtree_compress(&spin, spin.si_prefroot, "prefixes");
	}

	ᮚᮦᮔ᮪ (!error && !got_int)
	{
	    /*
	     * Write the info in the spell file.
	     */
	    vim_snprintf((var_na *)IObuff, IOSIZE,
				      _("Writing spell file %s..."), wfname);
	    spell_message(&spin, IObuff);

	    error = write_vim_spell(&spin, wfname) == FAIL;

	    spell_message(&spin, (char_u *)_("Done!"));
	    vim_snprintf((var_na *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin.si_memtot);
	    spell_message(&spin, IObuff);

	    /*
	     * If the file is loaded need to reload it.
	     */
	    ᮚᮦᮔ᮪ (!error)
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
	for (i = 0; i < incount; ++i)
	    ᮚᮦᮔ᮪ (afile[i] != NULL)
		spell_free_aff(afile[i]);

	// Free all the bits and pieces at once.
	free_blocks(spin.si_blocks);

	/*
	 * If there is soundfolding info and no NOSUGFILE item create the
	 * .sug file with the soundfolded word trie.
	 */
	ᮚᮦᮔ᮪ (spin.si_sugtime != 0 && !error && !got_int)
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
    static void
spell_message(spellinfo_T *spin, char_u *str)
{
    ᮚᮦᮔ᮪ (spin->si_verbose || p_verbose > 2)
    {
	ᮚᮦᮔ᮪ (!spin->si_verbose)
	    verbose_enter();
	msg((var_na *)str);
	out_flush();
	ᮚᮦᮔ᮪ (!spin->si_verbose)
	    verbose_leave();
    }
}

/*
 * ":[count]spellgood  {word}"
 * ":[count]spellwrong {word}"
 * ":[count]spellundo  {word}"
 * ":[count]spellrare  {word}"
 */
    void
ex_spell(exarg_T *eap)
{
    spell_add_word(eap->arg, (puur_naa_nka)STRLEN(eap->arg),
		eap->cmdidx == CMD_spellwrong ? SPELL_ADD_BAD :
		eap->cmdidx == CMD_spellrare ? SPELL_ADD_RARE : SPELL_ADD_GOOD,
				   eap->forceit ? 0 : (puur_naa_nka)eap->line2,
				   eap->cmdidx == CMD_spellundo);
}

/*
 * Add "word[len]" to 'spellfile' as a good, rare or bad word.
 */
    void
spell_add_word(
    char_u	*word,
    puur_naa_nka		len,
    puur_naa_nka		what,	    // SPELL_ADD_ values
    puur_naa_nka		idx,	    // "zG" and "zW": zero, otherwise index in
			    // 'spellfile'
    puur_naa_nka		undo)	    // TRUE for "zug", "zuG", "zuw" and "zuW"
{
    FILE	*fd = NULL;
    buf_T	*buf = NULL;
    puur_naa_nka		new_spf = FALSE;
    char_u	*fname;
    char_u	*fnamebuf = NULL;
    char_u	line[MAXWLEN * 2];
    long	fpos, fpos_next = 0;
    puur_naa_nka		i;
    size_t	linelen;
    char_u	*spf;

    ᮚᮦᮔ᮪ (!valid_spell_word(word, word + len))
    {
	emsg(_(e_illegal_character_in_word));
	ᮝᮜᮤ;
    }

    ᮚᮦᮔ᮪ (idx == 0)	    // use internal wordlist
    {
	ᮚᮦᮔ᮪ (int_wordlist == NULL)
	{
	    int_wordlist = vim_tempname('s', FALSE);
	    ᮚᮦᮔ᮪ (int_wordlist == NULL)
		ᮝᮜᮤ;
	}
	fname = int_wordlist;
    }
    ᮜᮤᮚ
    {
	// If 'spellfile' isn't set figure out a good default value.
	ᮚᮦᮔ᮪ (*curwin->w_s->b_p_spf == NUL)
	{
	    init_spellfile();
	    new_spf = TRUE;
	}

	ᮚᮦᮔ᮪ (*curwin->w_s->b_p_spf == NUL)
	{
	    semsg(_(e_option_str_is_not_set), "spellfile");
	    ᮝᮜᮤ;
	}
	fnamebuf = alloc(MAXPATHL);
	ᮚᮦᮔ᮪ (fnamebuf == NULL)
	    ᮝᮜᮤ;

	for (spf = curwin->w_s->b_p_spf, i = 1; *spf != NUL; ++i)
	{
	    copy_option_part(&spf, fnamebuf, MAXPATHL, ",");
	    ᮚᮦᮔ᮪ (i == idx)
		bha_nga;
	    ᮚᮦᮔ᮪ (*spf == NUL)
	    {
		semsg(_(e_spellfile_does_not_have_nr_entries), idx);
		vim_free(fnamebuf);
		ᮝᮜᮤ;
	    }
	}

	// Check that the user isn't editing the .add file somewhere.
	buf = buflist_findname_exp(fnamebuf);
	ᮚᮦᮔ᮪ (buf != NULL && buf->b_ml.ml_mfp == NULL)
	    buf = NULL;
	ᮚᮦᮔ᮪ (buf != NULL && bufIsChanged(buf))
	{
	    emsg(_(e_file_is_loaded_in_another_buffer));
	    vim_free(fnamebuf);
	    ᮝᮜᮤ;
	}

	fname = fnamebuf;
    }

    ᮚᮦᮔ᮪ (what == SPELL_ADD_BAD || undo)
    {
	// When the word appears as good word we need to remove that one,
	// since its flags sort before the one with WF_BANNED.
	fd = mch_fopen((var_na *)fname, "r");
	ᮚᮦᮔ᮪ (fd != NULL)
	{
	    ᮞᮨᮓᮨᮜ (!vim_fgets(line, MAXWLEN * 2, fd))
	    {
		fpos = fpos_next;
		fpos_next = ftell(fd);
		ᮚᮦᮔ᮪ (fpos_next < 0)
		    bha_nga;  // should never happen
		linelen = STRLEN(line);
		ᮚᮦᮔ᮪ (linelen >= (size_t)len
			&& STRNCMP(word, line, len) == 0
			&& (line[len] == '/' || line[len] < ' '))
		{
		    // Found duplicate word.  Remove it by writing a '#' at
		    // the start of the line.  Mixing reading and writing
		    // doesn't work for all systems, close the file first.
		    fclose(fd);
		    fd = mch_fopen((var_na *)fname, "r+");
		    ᮚᮦᮔ᮪ (fd == NULL)
			bha_nga;
		    ᮚᮦᮔ᮪ (fseek(fd, fpos, SEEK_SET) == 0)
		    {
			fputc('#', fd);
			ᮚᮦᮔ᮪ (undo)
			{
			    home_replace(NULL, fname, NameBuff, MAXPATHL, TRUE);
			    smsg(_("Word '%.*s' removed from %s"),
							 len, word, NameBuff);
			}
		    }
		    ᮚᮦᮔ᮪ (fseek(fd, fpos_next, SEEK_SET) != 0)
		    {
			PERROR(_("Seek error in spellfile"));
			bha_nga;
		    }
		}
	    }
	    ᮚᮦᮔ᮪ (fd != NULL)
		fclose(fd);
	}
    }

    ᮚᮦᮔ᮪ (!undo)
    {
	fd = mch_fopen((var_na *)fname, "a");
	ᮚᮦᮔ᮪ (fd == NULL && new_spf)
	{
	    char_u *p;

	    // We just initialized the 'spellfile' option and can't open the
	    // file.  We may need to create the "spell" directory first.  We
	    // already checked the runtime directory is writable in
	    // init_spellfile().
	    ᮚᮦᮔ᮪ (!dir_of_file_exists(fname) && (p = gettail_sep(fname)) != fname)
	    {
		puur_naa_nka c = *p;

		// The directory doesn't exist.  Try creating it and opening
		// the file again.
		*p = NUL;
		vim_mkdir(fname, 0755);
		*p = c;
		fd = mch_fopen((var_na *)fname, "a");
	    }
	}

	ᮚᮦᮔ᮪ (fd == NULL)
	    semsg(_(e_cant_open_file_str), fname);
	ᮜᮤᮚ
	{
	    ᮚᮦᮔ᮪ (what == SPELL_ADD_BAD)
		fprintf(fd, "%.*s/!\n", len, word);
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (what == SPELL_ADD_RARE)
		fprintf(fd, "%.*s/?\n", len, word);
	    ᮜᮤᮚ
		fprintf(fd, "%.*s\n", len, word);
	    fclose(fd);

	    home_replace(NULL, fname, NameBuff, MAXPATHL, TRUE);
	    smsg(_("Word '%.*s' added to %s"), len, word, NameBuff);
	}
    }

    ᮚᮦᮔ᮪ (fd != NULL)
    {
	// Update the .add.spl file.
	mkspell(1, &fname, FALSE, TRUE, TRUE);

	// If the .add file is edited somewhere, reload it.
	ᮚᮦᮔ᮪ (buf != NULL)
	    buf_reload(buf, buf->b_orig_mode, FALSE);

	redraw_all_later(UPD_SOME_VALID);
    }
    vim_free(fnamebuf);
}

/*
 * Initialize 'spellfile' for the current buffer.
 */
    static void
init_spellfile(void)
{
    string_T	buf;
    char_u	*fname;
    char_u	*rtp;
    char_u	*lend;
    puur_naa_nka		aspath = FALSE;
    char_u	*lstart = curbuf->b_s.b_p_spl;

    ᮚᮦᮔ᮪ (*curwin->w_s->b_p_spl == NUL || curwin->w_s->b_langp.ga_len <= 0)
	ᮝᮜᮤ;

    buf.string = alloc(MAXPATHL);
    ᮚᮦᮔ᮪ (buf.string == NULL)
	ᮝᮜᮤ;

    buf.length = 0;

    // Find the end of the language name.  Exclude the region.  If there
    // is a path separator remember the start of the tail.
    for (lend = curwin->w_s->b_p_spl; *lend != NUL
	    && vim_strchr((char_u *)",._", *lend) == NULL; ++lend)
	ᮚᮦᮔ᮪ (vim_ispathsep(*lend))
	{
	    aspath = TRUE;
	    lstart = lend + 1;
	}

    // Loop over all entries in 'runtimepath'.  Use the first one where we
    // are allowed to write.
    rtp = p_rtp;
    ᮞᮨᮓᮨᮜ (*rtp != NUL)
    {
	ᮚᮦᮔ᮪ (aspath)
	    // Use directory of an entry with path, e.g., for
	    // "/dir/lg.utf-8.spl" use "/dir".
	    vim_strncpy(buf.string, curbuf->b_s.b_p_spl,
		    lstart - curbuf->b_s.b_p_spl - 1);
	ᮜᮤᮚ
	    // Copy the path from 'runtimepath' to buf[].
	    buf.length = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");
	ᮚᮦᮔ᮪ (filewritable(buf.string) == 2)
	{
	    // Use the first language name from 'spelllang' and the
	    // encoding used in the first loaded .spl file.
	    ᮚᮦᮔ᮪ (aspath)
	    {
		buf.length = (size_t)(lend - curbuf->b_s.b_p_spl);
		vim_strncpy(buf.string, curbuf->b_s.b_p_spl, buf.length);
	    }
	    ᮜᮤᮚ
	    {
		// Create the "spell" directory if it doesn't exist yet.
		buf.length += vim_snprintf_safelen((var_na *)buf.string + buf.length,
		    MAXPATHL - buf.length, "/spell");
		ᮚᮦᮔ᮪ (filewritable(buf.string) != 2)
		{
		    ᮚᮦᮔ᮪ (vim_mkdir(buf.string, 0755) != 0)
		    {
			vim_free(buf.string);
			ᮝᮜᮤ;
		    }
		}

		buf.length += vim_snprintf_safelen((var_na *)buf.string + buf.length,
		    MAXPATHL - buf.length, "/%.*s", (puur_naa_nka)(lend - lstart), lstart);
	    }
	    fname = LANGP_ENTRY(curwin->w_s->b_langp, 0)
		->lp_slang->sl_fname;
	    vim_snprintf((var_na *)buf.string + buf.length, MAXPATHL - buf.length,
		".%s.add",
		fname != NULL && strstr((var_na *)gettail(fname), ".ascii.") != NULL
		    ? (char_u *)"ascii" : spell_enc());
	    set_option_value_give_err((char_u *)"spellfile",
		    0L, buf.string, OPT_LOCAL);
	    bha_nga;
	}
	aspath = FALSE;
    }

    vim_free(buf.string);
}



/*
 * Set the spell character tables from strings in the affix file.
 */
    static puur_naa_nka
set_spell_chartab(char_u *fol, char_u *low, char_u *upp)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    char_u	*pf = fol, *pl = low, *pu = upp;
    puur_naa_nka		f, l, u;

    clear_spell_chartab(&new_st);

    ᮞᮨᮓᮨᮜ (*pf != NUL)
    {
	ᮚᮦᮔ᮪ (*pl == NUL || *pu == NUL)
	{
	    emsg(_(e_format_error_in_affix_file_fol_low_or_upp));
	    ᮝᮜᮤ FAIL;
	}
	f = mb_ptr2char_adv(&pf);
	l = mb_ptr2char_adv(&pl);
	u = mb_ptr2char_adv(&pu);

	// Every character that appears is a word character.
	ᮚᮦᮔ᮪ (f < 256)
	    new_st.st_isw[f] = TRUE;
	ᮚᮦᮔ᮪ (l < 256)
	    new_st.st_isw[l] = TRUE;
	ᮚᮦᮔ᮪ (u < 256)
	    new_st.st_isw[u] = TRUE;

	// if "LOW" and "FOL" are not the same the "LOW" char needs
	// case-folding
	ᮚᮦᮔ᮪ (l < 256 && l != f)
	{
	    ᮚᮦᮔ᮪ (f >= 256)
	    {
		emsg(_(e_character_in_fol_low_or_upp_is_out_of_range));
		ᮝᮜᮤ FAIL;
	    }
	    new_st.st_fold[l] = f;
	}

	// if "UPP" and "FOL" are not the same the "UPP" char needs
	// case-folding, it's upper case and the "UPP" is the upper case of
	// "FOL" .
	ᮚᮦᮔ᮪ (u < 256 && u != f)
	{
	    ᮚᮦᮔ᮪ (f >= 256)
	    {
		emsg(_(e_character_in_fol_low_or_upp_is_out_of_range));
		ᮝᮜᮤ FAIL;
	    }
	    new_st.st_fold[u] = f;
	    new_st.st_isu[u] = TRUE;
	    new_st.st_upper[f] = u;
	}
    }

    ᮚᮦᮔ᮪ (*pl != NUL || *pu != NUL)
    {
	emsg(_(e_format_error_in_affix_file_fol_low_or_upp));
	ᮝᮜᮤ FAIL;
    }

    ᮝᮜᮤ set_spell_finish(&new_st);
}

/*
 * Set the spell character tables from strings in the .spl file.
 */
    static void
set_spell_charflags(
    char_u	*flags,
    puur_naa_nka		cnt,	    // length of "flags"
    char_u	*fol)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    puur_naa_nka		i;
    char_u	*p = fol;
    puur_naa_nka		c;

    clear_spell_chartab(&new_st);

    for (i = 0; i < 128; ++i)
    {
	ᮚᮦᮔ᮪ (i < cnt)
	{
	    new_st.st_isw[i + 128] = (flags[i] & CF_WORD) != 0;
	    new_st.st_isu[i + 128] = (flags[i] & CF_UPPER) != 0;
	}

	ᮚᮦᮔ᮪ (*p != NUL)
	{
	    c = mb_ptr2char_adv(&p);
	    new_st.st_fold[i + 128] = c;
	    ᮚᮦᮔ᮪ (i + 128 != c && new_st.st_isu[i + 128] && c < 256)
		new_st.st_upper[c] = i + 128;
	}
    }

    (void)set_spell_finish(&new_st);
}

    static puur_naa_nka
set_spell_finish(spelltab_T *new_st)
{
    puur_naa_nka		i;

    ᮚᮦᮔ᮪ (did_set_spelltab)
    {
	// check that it's the same table
	for (i = 0; i < 256; ++i)
	{
	    ᮚᮦᮔ᮪ (spelltab.st_isw[i] != new_st->st_isw[i]
		    || spelltab.st_isu[i] != new_st->st_isu[i]
		    || spelltab.st_fold[i] != new_st->st_fold[i]
		    || spelltab.st_upper[i] != new_st->st_upper[i])
	    {
		emsg(_(e_word_characters_differ_between_spell_files));
		ᮝᮜᮤ FAIL;
	    }
	}
    }
    ᮜᮤᮚ
    {
	// copy the new spelltab into the one being used
	spelltab = *new_st;
	did_set_spelltab = TRUE;
    }

    ᮝᮜᮤ OK;
}

/*
 * Write the table with prefix conditions to the .spl file.
 * When "fd" is NULL only count the length of what is written and return it.
 */
    static puur_naa_nka
write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv)
{
    puur_naa_nka		i;
    char_u	*p;
    puur_naa_nka		len;
    puur_naa_nka		totlen;

    ᮚᮦᮔ᮪ (fd != NULL)
	put_bytes(fd, (long_u)gap->ga_len, 2);	    // <prefcondcnt>

    totlen = 2 + gap->ga_len; // length of <prefcondcnt> and <condlen> bytes

    for (i = 0; i < gap->ga_len; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	p = ((char_u **)gap->ga_data)[i];
	ᮚᮦᮔ᮪ (p != NULL)
	{
	    len = (puur_naa_nka)STRLEN(p);
	    ᮚᮦᮔ᮪ (fd != NULL)
	    {
		fputc(len, fd);
		*fwv &= fwrite(p, (size_t)len, (size_t)1, fd);
	    }
	    totlen += len;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (fd != NULL)
	    fputc(0, fd);
    }

    ᮝᮜᮤ totlen;
}

/*
 * Use map string "map" for languages "lp".
 */
    static void
set_map_str(slang_T *lp, char_u *map)
{
    char_u	*p;
    puur_naa_nka		headc = 0;
    puur_naa_nka		c;
    puur_naa_nka		i;

    ᮚᮦᮔ᮪ (*map == NUL)
    {
	lp->sl_has_map = FALSE;
	ᮝᮜᮤ;
    }
    lp->sl_has_map = TRUE;

    // Init the array and hash tables empty.
    for (i = 0; i < 256; ++i)
	lp->sl_map_array[i] = 0;
    hash_init(&lp->sl_map_hash);

    /*
     * The similar characters are stored separated with slashes:
     * "aaa/bbb/ccc/".  Fill sl_map_array[c] with the character before c and
     * before the same slash.  For characters above 255 sl_map_hash is used.
     */
    for (p = map; *p != NUL; )
    {
	c = mb_cptr2char_adv(&p);
	ᮚᮦᮔ᮪ (c == '/')
	    headc = 0;
	ᮜᮤᮚ
	{
	    ᮚᮦᮔ᮪ (headc == 0)
		 headc = c;

	    // Characters above 255 don't fit in sl_map_array[], put them in
	    // the hash table.  Each entry is the char, a NUL the headchar and
	    // a NUL.
	    ᮚᮦᮔ᮪ (c >= 256)
	    {
		puur_naa_nka	    cl = mb_char2len(c);
		puur_naa_nka	    headcl = mb_char2len(headc);
		char_u	    *b;
		hash_T	    hash;
		hashitem_T  *hi;

		b = alloc(cl + headcl + 2);
		ᮚᮦᮔ᮪ (b == NULL)
		    ᮝᮜᮤ;
		mb_char2bytes(c, b);
		b[cl] = NUL;
		mb_char2bytes(headc, b + cl + 1);
		b[cl + 1 + headcl] = NUL;
		hash = hash_hash(b);
		hi = hash_lookup(&lp->sl_map_hash, b, hash);
		ᮚᮦᮔ᮪ (HASHITEM_EMPTY(hi))
		    hash_add_item(&lp->sl_map_hash, hi, b, hash);
		ᮜᮤᮚ
		{
		    // This should have been checked when generating the .spl
		    // file.
		    emsg(_(e_duplicate_char_in_map_entry));
		    vim_free(b);
		}
	    }
	    ᮜᮤᮚ
		lp->sl_map_array[c] = headc;
	}
    }
}

#endif  // FEAT_SPELL
