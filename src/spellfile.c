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

#якщо defined(FEAT_SPELL)

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
    для ((np) = (node); (np) != NULL; (np) = (np)->wn_sibling)

static ціле set_spell_finish(spelltab_T	*new_st);
static ціле write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv);
static ціле read_region_section(FILE *fd, slang_T *slang, ціле len);
static ціле read_charflags_section(FILE *fd);
static ціле read_prefcond_section(FILE *fd, slang_T *lp);
static ціле read_rep_section(FILE *fd, garray_T *gap, short *first);
static ціле read_sal_section(FILE *fd, slang_T *slang);
static ціле read_words_section(FILE *fd, slang_T *lp, ціле len);
static ціле read_sofo_section(FILE *fd, slang_T *slang);
static ціле read_compound(FILE *fd, slang_T *slang, ціле len);
static ціле set_sofo(slang_T *lp, char_u *from, char_u *to);
static void set_sal_first(slang_T *lp);
static ціле *mb_str2wide(char_u *s);
static ціле spell_read_tree(FILE *fd, char_u **bytsp, long *bytsp_len, idx_T **idxsp, ціле prefixtree, ціле prefixcnt);
static idx_T read_tree_node(FILE *fd, char_u *byts, idx_T *idxs, ціле maxidx, idx_T startidx, ціле prefixtree, ціле maxprefcondnr, ціле depth);
static void set_spell_charflags(char_u *flags, ціле cnt, char_u *upp);
static ціле set_spell_chartab(char_u *fol, char_u *low, char_u *upp);
static void set_map_str(slang_T *lp, char_u *map);


static char *e_afftrailing = N_("Trailing text in %s line %d: %s");
static char *e_affname = N_("Affix name too long in %s line %d: %s");
static char *msg_compressing = N_("Compressing word tree...");

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
    ціле		silent)		// no error if file doesn't exist
{
    FILE	*fd;
    char_u	buf[VIMSPELLMAGICL];
    char_u	*p;
    ціле		i;
    ціле		n;
    ціле		len;
    slang_T	*lp = NULL;
    ціле		c = 0;
    ціле		res;
    ціле		did_estack_push = FALSE;
    ESTACK_CHECK_DECLARATION;

    fd = mch_fopen((char *)fname, "r");
    якщо (fd == NULL)
    {
	якщо (!silent)
	    semsg(_(e_cant_open_file_str), fname);
	інакше якщо (p_verbose > 2)
	{
	    verbose_enter();
	    smsg((const char *)e_cant_open_file_str, fname);
	    verbose_leave();
	}
	goto endFAIL;
    }
    якщо (p_verbose > 2)
    {
	verbose_enter();
	smsg(_("Reading spell file \"%s\""), fname);
	verbose_leave();
    }

    якщо (old_lp == NULL)
    {
	lp = slang_alloc(lang);
	якщо (lp == NULL)
	    goto endFAIL;

	// Remember the file name, used to reload the file when it's updated.
	lp->sl_fname = vim_strsave(fname);
	якщо (lp->sl_fname == NULL)
	    goto endFAIL;

	// Check for .add.spl (_add.spl for VMS).
	lp->sl_add = strstr((char *)gettail(fname), SPL_FNAME_ADD) != NULL;
    }
    інакше
	lp = old_lp;

    // Set sourcing_name, so that error messages mention the file name.
    estack_push(ETYPE_SPELL, fname, 0);
    ESTACK_CHECK_SETUP;
    did_estack_push = TRUE;

    /*
     * <HEADER>: <fileID>
     */
    для (i = 0; i < VIMSPELLMAGICL; ++i)
	buf[i] = (c = getc(fd)) == EOF ? 0 : c;		// <fileID>
    якщо (STRNCMP(buf, VIMSPELLMAGIC, VIMSPELLMAGICL) != 0)
    {
	emsg(_(e_this_does_not_look_like_spell_file));
	goto endFAIL;
    }
    c = getc(fd);					// <versionnr>
    якщо (c < VIMSPELLVERSION)
    {
	emsg(_(e_old_spell_file_needs_to_be_updated));
	goto endFAIL;
    }
    інакше якщо (c > VIMSPELLVERSION)
    {
	emsg(_(e_spell_file_is_for_newer_version_of_vim));
	goto endFAIL;
    }


    /*
     * <SECTIONS>: <section> ... <sectionend>
     * <section>: <sectionID> <sectionflags> <sectionlen> (section contents)
     */
    для (;;)
    {
	n = getc(fd);			    // <sectionID> or <sectionend>
	якщо (n == SN_END)
	    break;
	c = getc(fd);					// <sectionflags>
	len = get4c(fd);				// <sectionlen>
	якщо (len < 0)
	    goto truncerr;

	res = 0;
	switch (n)
	{
	    case SN_INFO:
		lp->sl_info = read_string(fd, len);	// <infotext>
		якщо (lp->sl_info == NULL)
		    goto endFAIL;
		break;

	    case SN_REGION:
		res = read_region_section(fd, lp, len);
		break;

	    case SN_CHARFLAGS:
		res = read_charflags_section(fd);
		break;

	    case SN_MIDWORD:
		lp->sl_midword = read_string(fd, len);	// <midword>
		якщо (lp->sl_midword == NULL)
		    goto endFAIL;
		break;

	    case SN_PREFCOND:
		res = read_prefcond_section(fd, lp);
		break;

	    case SN_REP:
		res = read_rep_section(fd, &lp->sl_rep, lp->sl_rep_first);
		break;

	    case SN_REPSAL:
		res = read_rep_section(fd, &lp->sl_repsal, lp->sl_repsal_first);
		break;

	    case SN_SAL:
		res = read_sal_section(fd, lp);
		break;

	    case SN_SOFO:
		res = read_sofo_section(fd, lp);
		break;

	    case SN_MAP:
		p = read_string(fd, len);		// <mapstr>
		якщо (p == NULL)
		    goto endFAIL;
		set_map_str(lp, p);
		vim_free(p);
		break;

	    case SN_WORDS:
		res = read_words_section(fd, lp, len);
		break;

	    case SN_SUGFILE:
		lp->sl_sugtime = get8ctime(fd);		// <timestamp>
		break;

	    case SN_NOSPLITSUGS:
		lp->sl_nosplitsugs = TRUE;
		break;

	    case SN_NOCOMPOUNDSUGS:
		lp->sl_nocompoundsugs = TRUE;
		break;

	    case SN_COMPOUND:
		res = read_compound(fd, lp, len);
		break;

	    case SN_NOBREAK:
		lp->sl_nobreak = TRUE;
		break;

	    case SN_SYLLABLE:
		lp->sl_syllable = read_string(fd, len);	// <syllable>
		якщо (lp->sl_syllable == NULL)
		    goto endFAIL;
		якщо (init_syl_tab(lp) != OK)
		    goto endFAIL;
		break;

	    default:
		// Unsupported section.  When it's required give an error
		// message.  When it's not required skip the contents.
		якщо (c & SNF_REQUIRED)
		{
		    emsg(_(e_unsupported_section_in_spell_file));
		    goto endFAIL;
		}
		поки (--len >= 0)
		    якщо (getc(fd) < 0)
			goto truncerr;
		break;
	}
someerror:
	якщо (res == SP_FORMERROR)
	{
	    emsg(_(e_format_error_in_spell_file));
	    goto endFAIL;
	}
	якщо (res == SP_TRUNCERROR)
	{
truncerr:
	    emsg(_(e_truncated_spell_file));
	    goto endFAIL;
	}
	якщо (res == SP_OTHERERROR)
	    goto endFAIL;
    }

    // <LWORDTREE>
    res = spell_read_tree(fd, &lp->sl_fbyts, &lp->sl_fbyts_len,
						      &lp->sl_fidxs, FALSE, 0);
    якщо (res != 0)
	goto someerror;

    // <KWORDTREE>
    res = spell_read_tree(fd, &lp->sl_kbyts, NULL, &lp->sl_kidxs, FALSE, 0);
    якщо (res != 0)
	goto someerror;

    // <PREFIXTREE>
    res = spell_read_tree(fd, &lp->sl_pbyts, NULL, &lp->sl_pidxs, TRUE,
							    lp->sl_prefixcnt);
    якщо (res != 0)
	goto someerror;

    // For a new file link it in the list of spell files.
    якщо (old_lp == NULL && lang != NULL)
    {
	lp->sl_next = first_lang;
	first_lang = lp;
    }

    goto endOK;

endFAIL:
    якщо (lang != NULL)
	// truncating the name signals the error to spell_load_lang()
	*lang = NUL;
    якщо (lp != NULL && old_lp == NULL)
	slang_free(lp);
    lp = NULL;

endOK:
    якщо (fd != NULL)
	fclose(fd);
    якщо (did_estack_push)
    {
	ESTACK_CHECK_NOW;
	estack_pop();
    }

    повернути lp;
}

/*
 * Fill in the wordcount fields for a trie.
 * Returns the total number of words.
 */
    static void
tree_count_words(char_u *byts, long byts_len, idx_T *idxs)
{
    ціле		depth;
    idx_T	arridx[MAXWLEN];
    ціле		curi[MAXWLEN];
    ціле		c;
    idx_T	n;
    ціле		wordcount[MAXWLEN];

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;
    depth = 0;
    поки (depth >= 0 && !got_int)
    {
	якщо (curi[depth] > byts[arridx[depth]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[depth]] = wordcount[depth];
	    якщо (depth > 0)
		wordcount[depth - 1] += wordcount[depth];

	    --depth;
	    fast_breakcheck();
	}
	інакше
	{
	    // Do one more byte at this node.
	    n = arridx[depth] + curi[depth];
	    ++curi[depth];

	    c = byts[n];
	    якщо (c == 0)
	    {
		// End of word, count it.
		++wordcount[depth];

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		поки (n + 1 < byts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[depth];
		}
	    }
	    інакше якщо (depth < MAXWLEN - 1)
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
    ціле		lpi;
    slang_T	*slang;
    char_u	*dotp;
    FILE	*fd;
    char_u	buf[MAXWLEN];
    ціле		i;
    time_t	timestamp;
    ціле		wcount;
    ціле		wordnr;
    garray_T	ga;
    ціле		c;

    // Do this for all languages that support sound folding.
    для (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_ENTRY(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	якщо (slang->sl_sugtime != 0 && !slang->sl_sugloaded)
	{
	    // Change ".spl" to ".sug" and open the file.  When the file isn't
	    // found silently skip it.  Do set "sl_sugloaded" so that we
	    // don't try again and again.
	    slang->sl_sugloaded = TRUE;

	    dotp = vim_strrchr(slang->sl_fname, '.');
	    якщо (dotp == NULL || fnamecmp(dotp, ".spl") != 0)
		continue;
	    STRCPY(dotp, ".sug");
	    fd = mch_fopen((char *)slang->sl_fname, "r");
	    якщо (fd == NULL)
		goto nextone;

	    /*
	     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
	     */
	    для (i = 0; i < VIMSUGMAGICL; ++i)
		buf[i] = (c = getc(fd)) == EOF ? 0 : c;	// <fileID>
	    якщо (STRNCMP(buf, VIMSUGMAGIC, VIMSUGMAGICL) != 0)
	    {
		semsg(_(e_this_does_not_look_like_sug_file_str),
							     slang->sl_fname);
		goto nextone;
	    }
	    c = getc(fd);				// <versionnr>
	    якщо (c < VIMSUGVERSION)
	    {
		semsg(_(e_old_sug_file_needs_to_be_updated_str),
							     slang->sl_fname);
		goto nextone;
	    }
	    інакше якщо (c > VIMSUGVERSION)
	    {
		semsg(_(e_sug_file_is_for_newer_version_of_vim_str),
							     slang->sl_fname);
		goto nextone;
	    }

	    // Check the timestamp, it must be exactly the same as the one in
	    // the .spl file.  Otherwise the word numbers won't match.
	    timestamp = get8ctime(fd);			// <timestamp>
	    якщо (timestamp != slang->sl_sugtime)
	    {
		semsg(_(e_sug_file_doesnt_match_spl_file_str),
							     slang->sl_fname);
		goto nextone;
	    }

	    /*
	     * <SUGWORDTREE>: <wordtree>
	     * Read the trie with the soundfolded words.
	     */
	    якщо (spell_read_tree(fd, &slang->sl_sbyts, &slang->sl_sbyts_len,
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
	    якщо (slang->sl_sugbuf == NULL)
		goto someerror;
							    // <sugwcount>
	    wcount = get4c(fd);
	    якщо (wcount < 0)
		goto someerror;

	    // Read all the wordnr lists into the buffer, one NUL terminated
	    // list per line.
	    ga_init2(&ga, 1, 100);
	    для (wordnr = 0; wordnr < wcount; ++wordnr)
	    {
		ga.ga_len = 0;
		для (;;)
		{
		    c = getc(fd);			    // <sugline>
		    якщо (c < 0 || ga_grow(&ga, 1) == FAIL)
			goto someerror;
		    ((char_u *)ga.ga_data)[ga.ga_len++] = c;
		    якщо (c == NUL)
			break;
		}
		якщо (ml_append_buf(slang->sl_sugbuf, (linenr_T)wordnr,
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
	    якщо (fd != NULL)
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
read_cnt_string(FILE *fd, ціле cnt_bytes, ціле *cntp)
{
    ціле		cnt = 0;
    ціле		i;
    char_u	*str;

    // read the length bytes, MSB first
    для (i = 0; i < cnt_bytes; ++i)
    {
	ціле c = getc(fd);

	якщо (c == EOF)
	{
	    *cntp = SP_TRUNCERROR;
	    повернути NULL;
	}
	cnt = (cnt << 8) + (unsigned)c;
    }
    *cntp = cnt;
    якщо (cnt == 0)
	повернути NULL;	    // nothing to read, return NULL

    str = read_string(fd, cnt);
    якщо (str == NULL)
	*cntp = SP_OTHERERROR;
    повернути str;
}

/*
 * Read SN_REGION: <regionname> ...
 * Return SP_*ERROR flags.
 */
    static ціле
read_region_section(FILE *fd, slang_T *lp, ціле len)
{
    ціле		i;
    ціле		c = 0;

    якщо (len > MAXREGIONS * 2)
	повернути SP_FORMERROR;
    для (i = 0; i < len; ++i)
	lp->sl_regions[i] = (c = getc(fd)) == EOF ? 0 : c; // <regionname>
    lp->sl_regions[len] = NUL;
    повернути c == EOF ? SP_TRUNCERROR : 0;
}

/*
 * Read SN_CHARFLAGS section: <charflagslen> <charflags>
 *				<folcharslen> <folchars>
 * Return SP_*ERROR flags.
 */
    static ціле
read_charflags_section(FILE *fd)
{
    char_u	*flags;
    char_u	*fol;
    ціле		flagslen, follen;

    // <charflagslen> <charflags>
    flags = read_cnt_string(fd, 1, &flagslen);
    якщо (flagslen < 0)
	повернути flagslen;

    // <folcharslen> <folchars>
    fol = read_cnt_string(fd, 2, &follen);
    якщо (follen < 0)
    {
	vim_free(flags);
	повернути follen;
    }

    // Set the word-char flags and fill SPELL_ISUPPER() table.
    якщо (flags != NULL && fol != NULL)
	set_spell_charflags(flags, flagslen, fol);

    vim_free(flags);
    vim_free(fol);

    // When <charflagslen> is zero then <fcharlen> must also be zero.
    якщо ((flags == NULL) != (fol == NULL))
	повернути SP_FORMERROR;
    повернути 0;
}

/*
 * Read SN_PREFCOND section.
 * Return SP_*ERROR flags.
 */
    static ціле
read_prefcond_section(FILE *fd, slang_T *lp)
{
    ціле		cnt;
    ціле		i;
    ціле		n;
    ціле		c;
    char_u	*p;
    char_u	buf[MAXWLEN + 1];

    // <prefcondcnt> <prefcond> ...
    cnt = get2c(fd);					// <prefcondcnt>
    якщо (cnt <= 0)
	повернути SP_FORMERROR;

    lp->sl_prefprog = ALLOC_CLEAR_MULT(regprog_T *, cnt);
    якщо (lp->sl_prefprog == NULL)
	повернути SP_OTHERERROR;
    lp->sl_prefixcnt = cnt;

    для (i = 0; i < cnt; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	n = getc(fd);					// <condlen>
	якщо (n < 0 || n >= MAXWLEN)
	    повернути SP_FORMERROR;

	// When <condlen> is zero we have an empty condition.  Otherwise
	// compile the regexp program used to check for the condition.
	якщо (n > 0)
	{
	    buf[0] = '^';	    // always match at one position only
	    p = buf + 1;
	    поки (n-- > 0)
		*p++ = (c = getc(fd)) == EOF ? 0 : c;	    // <condstr>
	    якщо (c == EOF)
		break;
	    *p = NUL;
	    lp->sl_prefprog[i] = vim_regcomp(buf, RE_MAGIC + RE_STRING);
	}
    }
    повернути 0;
}

/*
 * Read REP or REPSAL items section from "fd": <repcount> <rep> ...
 * Return SP_*ERROR flags.
 */
    static ціле
read_rep_section(FILE *fd, garray_T *gap, short *first)
{
    ціле		cnt;
    fromto_T	*ftp;
    ціле		i;

    cnt = get2c(fd);					// <repcount>
    якщо (cnt < 0)
	повернути SP_TRUNCERROR;

    якщо (ga_grow(gap, cnt) == FAIL)
	повернути SP_OTHERERROR;

    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
    для (; gap->ga_len < cnt; ++gap->ga_len)
    {
	ftp = &((fromto_T *)gap->ga_data)[gap->ga_len];
	ftp->ft_from = read_cnt_string(fd, 1, &i);
	якщо (i < 0)
	    повернути i;
	якщо (i == 0)
	    повернути SP_FORMERROR;
	ftp->ft_to = read_cnt_string(fd, 1, &i);
	якщо (i <= 0)
	{
	    vim_free(ftp->ft_from);
	    якщо (i < 0)
		повернути i;
	    повернути SP_FORMERROR;
	}
    }

    // Fill the first-index table.
    для (i = 0; i < 256; ++i)
	first[i] = -1;
    для (i = 0; i < gap->ga_len; ++i)
    {
	ftp = &((fromto_T *)gap->ga_data)[i];
	якщо (first[*ftp->ft_from] == -1)
	    first[*ftp->ft_from] = i;
    }
    повернути 0;
}

/*
 * Read SN_SAL section: <salflags> <salcount> <sal> ...
 * Return SP_*ERROR flags.
 */
    static ціле
read_sal_section(FILE *fd, slang_T *slang)
{
    ціле		i;
    ціле		cnt;
    garray_T	*gap;
    salitem_T	*smp;
    ціле		ccnt;
    char_u	*p;

    slang->sl_sofo = FALSE;

    i = getc(fd);				// <salflags>
    якщо (i & SAL_F0LLOWUP)
	slang->sl_followup = TRUE;
    якщо (i & SAL_COLLAPSE)
	slang->sl_collapse = TRUE;
    якщо (i & SAL_REM_ACCENTS)
	slang->sl_rem_accents = TRUE;

    cnt = get2c(fd);				// <salcount>
    якщо (cnt < 0)
	повернути SP_TRUNCERROR;

    gap = &slang->sl_sal;
    ga_init2(gap, sizeof(salitem_T), 10);
    якщо (ga_grow(gap, cnt + 1) == FAIL)
	повернути SP_OTHERERROR;

    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
    для (; gap->ga_len < cnt; ++gap->ga_len)
    {
	ціле	c = NUL;

	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	ccnt = getc(fd);			// <salfromlen>
	якщо (ccnt < 0)
	    повернути SP_TRUNCERROR;
	якщо ((p = alloc(ccnt + 2)) == NULL)
	    повернути SP_OTHERERROR;
	smp->sm_lead = p;

	// Read up to the first special char into sm_lead.
	для (i = 0; i < ccnt; ++i)
	{
	    c = getc(fd);			// <salfrom>
	    якщо (vim_strchr((char_u *)"0123456789(-<^$", c) != NULL)
		break;
	    *p++ = c;
	}
	smp->sm_leadlen = (ціле)(p - smp->sm_lead);
	*p++ = NUL;

	// Put (abc) chars in sm_oneof, if any.
	якщо (c == '(')
	{
	    smp->sm_oneof = p;
	    для (++i; i < ccnt; ++i)
	    {
		c = getc(fd);			// <salfrom>
		якщо (c == ')')
		    break;
		*p++ = c;
	    }
	    *p++ = NUL;
	    якщо (++i < ccnt)
		c = getc(fd);
	}
	інакше
	    smp->sm_oneof = NULL;

	// Any following chars go in sm_rules.
	smp->sm_rules = p;
	якщо (i < ccnt)
	    // store the char we got while checking for end of sm_lead
	    *p++ = c;
	для (++i; i < ccnt; ++i)
	    *p++ = (c = getc(fd)) == EOF ? 0 : c;	// <salfrom>
	*p++ = NUL;

	// <saltolen> <salto>
	smp->sm_to = read_cnt_string(fd, 1, &ccnt);
	якщо (ccnt < 0)
	{
	    vim_free(smp->sm_lead);
	    повернути ccnt;
	}

	якщо (has_mbyte)
	{
	    // convert the multi-byte strings to wide char strings
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = mb_charlen(smp->sm_lead);
	    якщо (smp->sm_oneof == NULL)
		smp->sm_oneof_w = NULL;
	    інакше
		smp->sm_oneof_w = mb_str2wide(smp->sm_oneof);
	    якщо (smp->sm_to == NULL)
		smp->sm_to_w = NULL;
	    інакше
		smp->sm_to_w = mb_str2wide(smp->sm_to);
	    якщо (smp->sm_lead_w == NULL
		    || (smp->sm_oneof_w == NULL && smp->sm_oneof != NULL)
		    || (smp->sm_to_w == NULL && smp->sm_to != NULL))
	    {
		vim_free(smp->sm_lead);
		vim_free(smp->sm_to);
		vim_free(smp->sm_lead_w);
		vim_free(smp->sm_oneof_w);
		vim_free(smp->sm_to_w);
		повернути SP_OTHERERROR;
	    }
	}
    }

    якщо (gap->ga_len > 0)
    {
	// Add one extra entry to mark the end with an empty sm_lead.  Avoids
	// that we need to check the index every time.
	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	якщо ((p = alloc(1)) == NULL)
	    повернути SP_OTHERERROR;
	p[0] = NUL;
	smp->sm_lead = p;
	smp->sm_leadlen = 0;
	smp->sm_oneof = NULL;
	smp->sm_rules = p;
	smp->sm_to = NULL;
	якщо (has_mbyte)
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

    повернути 0;
}

/*
 * Read SN_WORDS: <word> ...
 * Return SP_*ERROR flags.
 */
    static ціле
read_words_section(FILE *fd, slang_T *lp, ціле len)
{
    ціле		done = 0;
    ціле		i;
    ціле		c;
    char_u	word[MAXWLEN];

    поки (done < len)
    {
	// Read one word at a time.
	для (i = 0; ; ++i)
	{
	    c = getc(fd);
	    якщо (c == EOF)
		повернути SP_TRUNCERROR;
	    word[i] = c;
	    якщо (word[i] == NUL)
		break;
	    якщо (i == MAXWLEN - 1)
		повернути SP_FORMERROR;
	}

	// Init the count to 10.
	count_common_word(lp, word, -1, 10);
	done += i + 1;
    }
    повернути 0;
}

/*
 * SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
 * Return SP_*ERROR flags.
 */
    static ціле
read_sofo_section(FILE *fd, slang_T *slang)
{
    ціле		cnt;
    char_u	*from, *to;
    ціле		res;

    // <sofofromlen> <sofofrom>
    from = read_cnt_string(fd, 2, &cnt);
    якщо (cnt < 0)
	повернути cnt;

    // <sofotolen> <sofoto>
    to = read_cnt_string(fd, 2, &cnt);
    якщо (cnt < 0)
    {
	vim_free(from);
	повернути cnt;
    }

    // Store the info in slang->sl_sal and/or slang->sl_sal_first.
    якщо (from != NULL && to != NULL)
	res = set_sofo(slang, from, to);
    інакше якщо (from != NULL || to != NULL)
	res = SP_FORMERROR;    // only one of two strings is an error
    інакше
	res = 0;

    vim_free(from);
    vim_free(to);
    повернути res;
}

/*
 * Read the compound section from the .spl file:
 *	<compmax> <compminlen> <compsylmax> <compoptions> <compflags>
 * Returns SP_*ERROR flags.
 */
    static ціле
read_compound(FILE *fd, slang_T *slang, ціле len)
{
    ціле		todo = len;
    ціле		c;
    ціле		atstart;
    char_u	*pat;
    char_u	*pp;
    char_u	*cp;
    char_u	*ap;
    char_u	*crp;
    ціле		cnt;
    garray_T	*gap;
    size_t	patsize;
    size_t	flagsize;

    якщо (todo < 2)
	повернути SP_FORMERROR;	// need at least two bytes

    --todo;
    c = getc(fd);					// <compmax>
    якщо (c < 2)
	c = MAXWLEN;
    slang->sl_compmax = c;

    --todo;
    c = getc(fd);					// <compminlen>
    якщо (c < 1)
	c = 0;
    slang->sl_compminlen = c;

    --todo;
    c = getc(fd);					// <compsylmax>
    якщо (c < 1)
	c = MAXWLEN;
    slang->sl_compsylmax = c;

    c = getc(fd);					// <compoptions>
    якщо (c != 0)
	ungetc(c, fd);	    // be backwards compatible with Vim 7.0b
    інакше
    {
	--todo;
	c = getc(fd);	    // only use the lower byte for now
	--todo;
	slang->sl_compoptions = c;

	gap = &slang->sl_comppat;
	c = get2c(fd);					// <comppatcount>
	якщо (c < 0)
	    повернути SP_TRUNCERROR;
	todo -= 2;
	ga_init2(gap, sizeof(char_u *), c);
	якщо (ga_grow(gap, c) == OK)
	    поки (--c >= 0)
	    {
		((char_u **)(gap->ga_data))[gap->ga_len++] =
						  read_cnt_string(fd, 1, &cnt);
					    // <comppatlen> <comppattext>
		якщо (cnt < 0)
		    повернути cnt;
		todo -= cnt + 1;
	    }
    }
    якщо (todo < 0)
	повернути SP_FORMERROR;

    // Turn the COMPOUNDRULE items into a regexp pattern:
    // "a[bc]/a*b+" -> "^\(a[bc]\|a*b\+\)$".
    // Inserting backslashes may double the length, "^\(\)$<Nul>" is 7 bytes.
    // Conversion to utf-8 may double the size.
    якщо ((size_t)todo > COMPOUND_MAX_LEN)
	повернути SP_FORMERROR;
    patsize = (size_t)todo * 2 + 7;
    якщо (enc_utf8)
	patsize += (size_t)todo * 2;
    flagsize = (size_t)todo + 1;
    pat = alloc(patsize);
    якщо (pat == NULL)
	повернути SP_OTHERERROR;

    // We also need a list of all flags that can appear at the start and one
    // for all flags.
    cp = alloc(flagsize);
    якщо (cp == NULL)
    {
	vim_free(pat);
	повернути SP_OTHERERROR;
    }
    slang->sl_compstartflags = cp;
    *cp = NUL;

    ap = alloc(flagsize);
    якщо (ap == NULL)
    {
	vim_free(pat);
	повернути SP_OTHERERROR;
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
    поки (todo-- > 0)
    {
	c = getc(fd);					// <compflags>
	якщо (c == EOF)
	{
	    vim_free(pat);
	    повернути SP_TRUNCERROR;
	}

	// Add all flags to "sl_compallflags".
	якщо (vim_strchr((char_u *)"?*+[]/", c) == NULL
		&& !byte_in_str(slang->sl_compallflags, c))
	{
	    *ap++ = c;
	    *ap = NUL;
	}

	якщо (atstart != 0)
	{
	    // At start of item: copy flags to "sl_compstartflags".  For a
	    // [abc] item set "atstart" to 2 and copy up to the ']'.
	    якщо (c == '[')
		atstart = 2;
	    інакше якщо (c == ']')
		atstart = 0;
	    інакше
	    {
		якщо (!byte_in_str(slang->sl_compstartflags, c))
		{
		    *cp++ = c;
		    *cp = NUL;
		}
		якщо (atstart == 1)
		    atstart = 0;
	    }
	}

	// Copy flag to "sl_comprules", unless we run into a wildcard.
	якщо (crp != NULL)
	{
	    якщо (c == '?' || c == '+' || c == '*')
	    {
		VIM_CLEAR(slang->sl_comprules);
		crp = NULL;
	    }
	    інакше
		*crp++ = c;
	}

	якщо (c == '/')	    // slash separates two items
	{
	    *pp++ = '\\';
	    *pp++ = '|';
	    atstart = 1;
	}
	інакше		    // normal char, "[abc]" and '*' are copied as-is
	{
	    якщо (c == '?' || c == '+' || c == '~')
		*pp++ = '\\';	    // "a?" becomes "a\?", "a+" becomes "a\+"
	    якщо (enc_utf8)
		pp += mb_char2bytes(c, pp);
	    інакше
		*pp++ = c;
	}
    }

    *pp++ = '\\';
    *pp++ = ')';
    *pp++ = '$';
    *pp = NUL;

    якщо (crp != NULL)
	*crp = NUL;

    slang->sl_compprog = vim_regcomp(pat, RE_MAGIC + RE_STRING + RE_STRICT);
    vim_free(pat);
    якщо (slang->sl_compprog == NULL)
	повернути SP_FORMERROR;

    повернути 0;
}

/*
 * Set the SOFOFROM and SOFOTO items in language "lp".
 * Returns SP_*ERROR flags when there is something wrong.
 */
    static ціле
set_sofo(slang_T *lp, char_u *from, char_u *to)
{
    ціле		i;

    garray_T	*gap;
    char_u	*s;
    char_u	*p;
    ціле		c;
    ціле		*inp;

    якщо (has_mbyte)
    {
	free_sal_items(&lp->sl_sal);

	// Use "sl_sal" as an array with 256 pointers to a list of wide
	// characters.  The index is the low byte of the character.
	// The list contains from-to pairs with a terminating NUL.
	// sl_sal_first[] is used for latin1 "from" characters.
	gap = &lp->sl_sal;
	ga_init2(gap, sizeof(ціле *), 1);
	якщо (ga_grow(gap, 256) == FAIL)
	    повернути SP_OTHERERROR;
	vim_memset(gap->ga_data, 0, sizeof(ціле *) * 256);
	gap->ga_len = 256;
	lp->sl_sofo = TRUE;

	// First count the number of items for each list.  Temporarily use
	// sl_sal_first[] for this.  Reset it first: a preceding SN_SAL section
	// may have set the entries to -1 via set_sal_first().
	vim_memset(lp->sl_sal_first, 0, sizeof(salfirst_T) * 256);
	для (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    MB_CPTR_ADV(s);
	    якщо (c >= 256)
		++lp->sl_sal_first[c & 0xff];
	}
	якщо (*p != NUL || *s != NUL)	    // lengths differ
	    повернути SP_FORMERROR;

	// Allocate the lists.
	для (i = 0; i < 256; ++i)
	    якщо (lp->sl_sal_first[i] > 0)
	    {
		p = alloc(sizeof(ціле) * (lp->sl_sal_first[i] * 2 + 1));
		якщо (p == NULL)
		    повернути SP_OTHERERROR;
		((ціле **)gap->ga_data)[i] = (ціле *)p;
		*(ціле *)p = 0;
	    }

	// Put the characters up to 255 in sl_sal_first[] the rest in a sl_sal
	// list.
	vim_memset(lp->sl_sal_first, 0, sizeof(salfirst_T) * 256);
	для (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    i = mb_cptr2char_adv(&s);
	    якщо (c >= 256)
	    {
		// Append the from-to chars at the end of the list with
		// the low byte.
		inp = ((ціле **)gap->ga_data)[c & 0xff];
		поки (*inp != 0)
		    ++inp;
		*inp++ = c;		// from char
		*inp++ = i;		// to char
		*inp++ = NUL;		// NUL at the end
	    }
	    інакше
		// mapping byte to char is done in sl_sal_first[]
		lp->sl_sal_first[c] = i;
	}
    }
    інакше
    {
	// mapping bytes to bytes is done in sl_sal_first[]
	якщо (STRLEN(from) != STRLEN(to))
	    повернути SP_FORMERROR;

	для (i = 0; to[i] != NUL; ++i)
	    lp->sl_sal_first[from[i]] = to[i];
	lp->sl_sal.ga_len = 1;		// indicates we have soundfolding
	lp->sl_sofo = TRUE;
    }

    повернути 0;
}

/*
 * Fill the first-index table for "lp".
 */
    static void
set_sal_first(slang_T *lp)
{
    salfirst_T	*sfirst;
    ціле		i;
    salitem_T	*smp;
    ціле		c;
    garray_T	*gap = &lp->sl_sal;

    sfirst = lp->sl_sal_first;
    для (i = 0; i < 256; ++i)
	sfirst[i] = -1;
    smp = (salitem_T *)gap->ga_data;
    для (i = 0; i < gap->ga_len; ++i)
    {
	якщо (has_mbyte)
	    // Use the lowest byte of the first character.  For latin1 it's
	    // the character, for other encodings it should differ for most
	    // characters.
	    c = *smp[i].sm_lead_w & 0xff;
	інакше
	    c = *smp[i].sm_lead;
	якщо (sfirst[c] == -1)
	{
	    sfirst[c] = i;
	    якщо (has_mbyte)
	    {
		ціле		n;

		// Make sure all entries with this byte are following each
		// other.  Move the ones that are in the wrong position.  Do
		// keep the same ordering!
		поки (i + 1 < gap->ga_len
				       && (*smp[i + 1].sm_lead_w & 0xff) == c)
		    // Skip over entry with same index byte.
		    ++i;

		для (n = 1; i + n < gap->ga_len; ++n)
		    якщо ((*smp[i + n].sm_lead_w & 0xff) == c)
		    {
			salitem_T  tsal;

			// Move entry with same index byte after the entries
			// we already found.
			++i;
			--n;
			tsal = smp[i + n];
			mch_memmove(smp + i + 1, smp + i,
						       sizeof(salitem_T) * n);
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
    static ціле *
mb_str2wide(char_u *s)
{
    ціле		*res;
    char_u	*p;
    ціле		i = 0;

    res = ALLOC_MULT(ціле, mb_charlen(s) + 1);
    якщо (res == NULL)
	повернути NULL;

    для (p = s; *p != NUL; )
	res[i++] = mb_ptr2char_adv(&p);
    res[i] = NUL;
    повернути res;
}

/*
 * Read a tree from the .spl or .sug file.
 * Allocates the memory and stores pointers in "bytsp" and "idxsp".
 * This is skipped when the tree has zero length.
 * Returns zero when OK, SP_ value for an error.
 */
    static ціле
spell_read_tree(
    FILE	*fd,
    char_u	**bytsp,
    long	*bytsp_len,
    idx_T	**idxsp,
    ціле		prefixtree,	// TRUE for the prefix tree
    ціле		prefixcnt)	// when "prefixtree" is TRUE: prefix count
{
    long	len;
    ціле		idx;
    char_u	*bp;
    idx_T	*ip;

    // The tree size was computed when writing the file, so that we can
    // allocate it as one long block. <nodecount>
    len = get4c(fd);
    якщо (len < 0)
	повернути SP_TRUNCERROR;
    якщо ((size_t)len > SIZE_MAX / sizeof(ціле))
	// Invalid length, multiply with sizeof(int) would overflow.
	повернути SP_FORMERROR;
    якщо (len <= 0)
	повернути 0;

    // Allocate the byte array.  Zero-initialize so that any position the
    // tree does not visit reads as 0; a stray BY_INDEX shared reference
    // into such a slot then behaves as end-of-word in spellsuggest()
    // instead of consuming an arbitrary heap byte as a siblingcount.
    bp = alloc_clear(len);
    якщо (bp == NULL)
	повернути SP_OTHERERROR;
    *bytsp = bp;
    якщо (bytsp_len != NULL)
	*bytsp_len = len;

    // Allocate the index array.
    ip = lalloc_clear(len * sizeof(ціле), TRUE);
    якщо (ip == NULL)
	повернути SP_OTHERERROR;
    *idxsp = ip;

    // Recursively read the tree and store it in the array.
    idx = read_tree_node(fd, bp, ip, len, 0, prefixtree, prefixcnt, 0);
    якщо (idx < 0)
	повернути idx;
    якщо (idx != len)
	повернути SP_FORMERROR;
    повернути 0;
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
    ціле		maxidx,		    // size of arrays
    idx_T	startidx,	    // current index in "byts" and "idxs"
    ціле		prefixtree,	    // TRUE for reading PREFIXTREE
    ціле		maxprefcondnr,	    // maximum for <prefcondnr>
    ціле		depth)		    // recursion level
{
    ціле		len;
    ціле		i;
    ціле		n;
    idx_T	idx = startidx;
    ціле		c;
    ціле		c2;
#define SHARED_MASK	0x8000000

    // Bail out on a crafted .spl whose tree recurses beyond the maximum
    // word length: each tree level corresponds to one byte of a word, so
    // any well-formed file has depth <= MAXWLEN.
    якщо (depth > MAXWLEN)
	повернути SP_FORMERROR;

    len = getc(fd);					// <siblingcount>
    якщо (len <= 0)
	повернути SP_TRUNCERROR;

    якщо (len >= maxidx - startidx)
	повернути SP_FORMERROR;
    byts[idx++] = len;

    // Read the byte values, flag/region bytes and shared indexes.
    для (i = 1; i <= len; ++i)
    {
	c = getc(fd);					// <byte>
	якщо (c < 0)
	    повернути SP_TRUNCERROR;
	якщо (c <= BY_SPECIAL)
	{
	    якщо (c == BY_NOFLAGS && !prefixtree)
	    {
		// No flags, all regions.
		idxs[idx] = 0;
		c = 0;
	    }
	    інакше якщо (c != BY_INDEX)
	    {
		якщо (prefixtree)
		{
		    // Read the optional pflags byte, the prefix ID and the
		    // condition nr.  In idxs[] store the prefix ID in the low
		    // byte, the condition index shifted up 8 bits, the flags
		    // shifted up 24 bits.
		    якщо (c == BY_FLAGS)
		    {
			n = getc(fd);			// <pflags>
			якщо (n < 0)
			    повернути SP_TRUNCERROR;
			c = n << 24;
		    }
		    інакше
			c = 0;

		    n = getc(fd);			// <affixID>
		    якщо (n < 0)
			повернути SP_TRUNCERROR;
		    c |= n;

		    n = get2c(fd);			// <prefcondnr>
		    якщо (n < 0 || n >= maxprefcondnr)
			повернути SP_FORMERROR;
		    c |= (n << 8);
		}
		інакше // c must be BY_FLAGS or BY_FLAGS2
		{
		    // Read flags and optional region and prefix ID.  In
		    // idxs[] the flags go in the low two bytes, region above
		    // that and prefix ID above the region.
		    c2 = c;
		    c = getc(fd);			// <flags>
		    якщо (c < 0)
			повернути SP_TRUNCERROR;
		    якщо (c2 == BY_FLAGS2)
		    {
			n = getc(fd);			// <flags2>
			якщо (n < 0)
			    повернути SP_TRUNCERROR;
			c = (n << 8) + c;
		    }
		    якщо (c & WF_REGION)
		    {
			n = getc(fd);			// <region>
			якщо (n < 0)
			    повернути SP_TRUNCERROR;
			c = (n << 16) + c;
		    }
		    якщо (c & WF_AFX)
		    {
			n = getc(fd);			// <affixID>
			якщо (n < 0)
			    повернути SP_TRUNCERROR;
			c = (n << 24) + c;
		    }
		}

		idxs[idx] = c;
		c = 0;
	    }
	    інакше // c == BY_INDEX
	    {
							// <nodeidx>
		n = get3c(fd);
		якщо (n < 0 || n >= maxidx)
		    повернути SP_FORMERROR;
		idxs[idx] = n + SHARED_MASK;
		c = getc(fd);				// <xbyte>
	    }
	}
	byts[idx++] = c;
    }

    // Recursively read the children for non-shared siblings.
    // Skip the end-of-word ones (zero byte value) and the shared ones (and
    // remove SHARED_MASK)
    для (i = 1; i <= len; ++i)
	якщо (byts[startidx + i] != 0)
	{
	    якщо (idxs[startidx + i] & SHARED_MASK)
		idxs[startidx + i] &= ~SHARED_MASK;
	    інакше
	    {
		idxs[startidx + i] = idx;
		idx = read_tree_node(fd, byts, idxs, maxidx, idx,
					prefixtree, maxprefcondnr, depth + 1);
		якщо (idx < 0)
		    break;
	    }
	}

    повернути idx;
}

/*
 * Reload the spell file "fname" if it's loaded.
 */
    static void
spell_reload_one(
    char_u	*fname,
    ціле		added_word)	// invoked through "zg"
{
    slang_T	*slang;
    ціле		didit = FALSE;

    FOR_ALL_SPELL_LANGS(slang)
    {
	якщо (fullpathcmp(fname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	{
	    slang_clear(slang);
	    якщо (spell_load_file(fname, NULL, slang, FALSE) == NULL)
		// reloading failed, clear the language
		slang_clear(slang);
	    redraw_all_later(UPD_SOME_VALID);
	    didit = TRUE;
	}
    }

    // When "zg" was used and the file wasn't loaded yet, should redo
    // 'spelllang' to load it now.
    якщо (added_word && !didit)
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
typedef struct afffile_S
{
    char_u	*af_enc;	// "SET", normalized, alloc'ed string or NULL
    ціле		af_flagtype;	// AFT_CHAR, AFT_LONG, AFT_NUM or AFT_CAPLONG
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
    ціле		af_pfxpostpone;	// postpone prefixes without chop string and
				// without flags
    ціле		af_ignoreextra;	// IGNOREEXTRA present
    hashtab_T	af_pref;	// hashtable for prefixes, affheader_T
    hashtab_T	af_suff;	// hashtable for suffixes, affheader_T
    hashtab_T	af_comp;	// hashtable for compound flags, compitem_T
} afffile_T;

#define AFT_CHAR	0	// flags are one character
#define AFT_LONG	1	// flags are two characters
#define AFT_CAPLONG	2	// flags are one or two characters
#define AFT_NUM		3	// flags are numbers, comma separated

typedef struct affentry_S affentry_T;
// Affix entry from ".aff" file.  Used for prefixes and suffixes.
struct affentry_S
{
    affentry_T	*ae_next;	// next affix with same name/number
    char_u	*ae_chop;	// text to chop off basic word (can be NULL)
    char_u	*ae_add;	// text to add to basic word (can be NULL)
    char_u	*ae_flags;	// flags on the affix (can be NULL)
    char_u	*ae_cond;	// condition (NULL for ".")
    regprog_T	*ae_prog;	// regexp program for ae_cond or NULL
    char	ae_compforbid;	// COMPOUNDFORBIDFLAG found
    char	ae_comppermit;	// COMPOUNDPERMITFLAG found
};

#define AH_KEY_LEN 17		// 2 x 8 bytes + NUL

// Affix header from ".aff" file.  Used for af_pref and af_suff.
typedef struct affheader_S
{
    char_u	ah_key[AH_KEY_LEN]; // key for hashtab == name of affix
    unsigned	ah_flag;	// affix name as number, uses "af_flagtype"
    ціле		ah_newID;	// prefix ID after renumbering; 0 if not used
    ціле		ah_combine;	// suffix may combine with prefix
    ціле		ah_follows;	// another affix block should be following
    affentry_T	*ah_first;	// first affix entry
} affheader_T;

#define HI2AH(hi)   ((affheader_T *)(hi)->hi_key)

// Flag used in compound items.
typedef struct compitem_S
{
    char_u	ci_key[AH_KEY_LEN]; // key for hashtab == name of compound
    unsigned	ci_flag;	// affix name as number, uses "af_flagtype"
    ціле		ci_newID;	// affix ID after renumbering.
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
typedef struct sblock_S sblock_T;
struct sblock_S
{
    ціле		sb_used;	// nr of bytes already in use
    sblock_T	*sb_next;	// next block in list
    char_u	sb_data[1];	// data, actually longer
};

/*
 * A node in the tree.
 */
typedef struct wordnode_S wordnode_T;
struct wordnode_S
{
    union   // shared to save space
    {
	char_u	hashkey[6];	// the hash key, only used while compressing
	ціле	index;		// index in written nodes (valid after first
				// round)
    } wn_u1;
    union   // shared to save space
    {
	wordnode_T *next;	// next node with same hash key
	wordnode_T *wnode;	// parent node that will write this node
    } wn_u2;
    wordnode_T	*wn_child;	// child (next byte in word)
    wordnode_T  *wn_sibling;	// next sibling (alternate byte in word,
				// always sorted)
    ціле		wn_refs;	// Nr. of references to this node.  Only
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
    ціле		wn_nr;		// sequence nr for printing
#endif
};

#define WN_MASK	 0xffff		// mask relevant bits of "wn_flags"

#define HI2WN(hi)    (wordnode_T *)((hi)->hi_key)

/*
 * Info used while reading the spell files.
 */
typedef struct spellinfo_S
{
    wordnode_T	*si_foldroot;	// tree with case-folded words
    long	si_foldwcount;	// nr of words in si_foldroot

    wordnode_T	*si_keeproot;	// tree with keep-case words
    long	si_keepwcount;	// nr of words in si_keeproot

    wordnode_T	*si_prefroot;	// tree with postponed prefixes

    long	si_sugtree;	// creating the soundfolding trie

    sblock_T	*si_blocks;	// memory blocks used
    long	si_blocks_cnt;	// memory blocks allocated
    ціле		si_did_emsg;	// TRUE when ran out of memory

    long	si_compress_cnt;    // words to add before lowering
				    // compression limit
    wordnode_T	*si_first_free; // List of nodes that have been freed during
				// compression, linked by "wn_child" field.
    long	si_free_count;	// number of nodes in si_first_free
#ifdef SPELL_PRINTTREE
    ціле		si_wordnode_nr;	// sequence nr for nodes
#endif
    buf_T	*si_spellbuf;	// buffer used to store soundfold word table

    ціле		si_ascii;	// handling only ASCII words
    ціле		si_add;		// addition file
    ціле		si_clear_chartab;   // when TRUE clear char tables
    ціле		si_region;	// region mask
    vimconv_T	si_conv;	// for conversion to 'encoding'
    ціле		si_memtot;	// runtime memory used
    ціле		si_verbose;	// verbose messages
    ціле		si_msg_count;	// number of words added since last message
    char_u	*si_info;	// info text chars or NULL
    ціле		si_region_count; // number of regions supported (1 when there
				 // are no regions)
    char_u	si_region_name[MAXREGIONS * 2 + 1];
				// region names; used only if
				// si_region_count > 1)

    garray_T	si_rep;		// list of fromto_T entries from REP lines
    garray_T	si_repsal;	// list of fromto_T entries from REPSAL lines
    garray_T	si_sal;		// list of fromto_T entries from SAL lines
    char_u	*si_sofofr;	// SOFOFROM text
    char_u	*si_sofoto;	// SOFOTO text
    ціле		si_nosugfile;	// NOSUGFILE item found
    ціле		si_nosplitsugs;	// NOSPLITSUGS item found
    ціле		si_nocompoundsugs; // NOCOMPOUNDSUGS item found
    ціле		si_followup;	// soundsalike: ?
    ціле		si_collapse;	// soundsalike: ?
    hashtab_T	si_commonwords;	// hashtable for common words
    time_t	si_sugtime;	// timestamp for .sug file
    ціле		si_rem_accents;	// soundsalike: remove accents
    garray_T	si_map;		// MAP info concatenated
    char_u	*si_midword;	// MIDWORD chars or NULL
    ціле		si_compmax;	// max nr of words for compounding
    ціле		si_compminlen;	// minimal length for compounding
    ціле		si_compsylmax;	// max nr of syllables for compounding
    ціле		si_compoptions;	// COMP_ flags
    garray_T	si_comppat;	// CHECKCOMPOUNDPATTERN items, each stored as
				// a string
    char_u	*si_compflags;	// flags used for compounding
    char_u	si_nobreak;	// NOBREAK
    char_u	*si_syllable;	// syllable string
    garray_T	si_prefcond;	// table with conditions for postponed
				// prefixes, each stored as a string
    ціле		si_newprefID;	// current value for ah_newID
    ціле		si_newcompID;	// current value for compound ID
} spellinfo_T;

static ціле is_aff_rule(char_u **items, ціле itemcnt, char *rulename, ціле	 mincount);
static void aff_process_flags(afffile_T *affile, affentry_T *entry);
static ціле spell_info_item(char_u *s);
static unsigned affitem2flag(ціле flagtype, char_u *item, char_u	*fname, ціле lnum);
static unsigned get_affitem(ціле flagtype, char_u **pp);
static void process_compflags(spellinfo_T *spin, afffile_T *aff, char_u *compflags);
static void check_renumber(spellinfo_T *spin);
static void aff_check_number(ціле spinval, ціле affval, char *name);
static void aff_check_string(char_u *spinval, char_u *affval, char *name);
static ціле str_equal(char_u *s1, char_u	*s2);
static void add_fromto(spellinfo_T *spin, garray_T *gap, char_u	*from, char_u *to);
static ціле sal_to_bool(char_u *s);
static ціле get_affix_flags(afffile_T *affile, char_u *afflist);
static ціле get_pfxlist(afffile_T *affile, char_u *afflist, char_u *store_afflist, ціле *cntp);
static ціле get_compflags(afffile_T *affile, char_u *afflist, char_u *store_afflist, ціле *cntp);
static ціле store_aff_word(spellinfo_T *spin, char_u *word, char_u *afflist, afffile_T *affile, hashtab_T *ht, hashtab_T *xht, ціле condit, ціле flags, char_u *pfxlist, ціле pfxlen);
static void *getroom(spellinfo_T *spin, size_t len, ціле align);
static char_u *getroom_save(spellinfo_T *spin, char_u *s);
static ціле store_word(spellinfo_T *spin, char_u *word, ціле flags, ціле region, char_u *pfxlist, ціле need_affix);
static ціле tree_add_word(spellinfo_T *spin, char_u *word, wordnode_T *tree, ціле flags, ціле region, ціле affixID);
static wordnode_T *get_wordnode(spellinfo_T *spin);
static void free_wordnode(spellinfo_T *spin, wordnode_T *n);
static void wordtree_compress(spellinfo_T *spin, wordnode_T *root, char *name);
static long node_compress(spellinfo_T *spin, wordnode_T *node, hashtab_T *ht, long *tot);
static ціле node_equal(wordnode_T *n1, wordnode_T *n2);
static void clear_node(wordnode_T *node);
static ціле put_node(FILE *fd, wordnode_T *node, ціле idx, ціле regionmask, ціле prefixtree);
static ціле sug_filltree(spellinfo_T *spin, slang_T *slang);
static ціле sug_maketable(spellinfo_T *spin);
static ціле sug_filltable(spellinfo_T *spin, wordnode_T *node, ціле startwordnr, garray_T *gap);
static ціле offset2bytes(ціле nr, char_u *buf);
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
    ціле
spell_check_msm(void)
{
    char_u	*p = p_msm;
    long	start = 0;
    long	incr = 0;
    long	added = 0;

    якщо (!VIM_ISDIGIT(*p))
	повернути FAIL;
    // block count = (value * 1024) / SBLOCKSIZE (but avoid overflow)
    start = (getdigits(&p) * 10) / (SBLOCKSIZE / 102);
    якщо (*p != ',')
	повернути FAIL;
    ++p;
    якщо (!VIM_ISDIGIT(*p))
	повернути FAIL;
    incr = (getdigits(&p) * 102) / (SBLOCKSIZE / 10);
    якщо (*p != ',')
	повернути FAIL;
    ++p;
    якщо (!VIM_ISDIGIT(*p))
	повернути FAIL;
    added = getdigits(&p) * 1024;
    якщо (*p != NUL)
	повернути FAIL;

    якщо (start == 0 || incr == 0 || added == 0 || incr > start)
	повернути FAIL;

    compress_start = start;
    compress_inc = incr;
    compress_added = added;
    повернути OK;
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

static char line1[PRINTLINESIZE];
static char line2[PRINTLINESIZE];
static char line3[PRINTLINESIZE];

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
spell_print_node(wordnode_T *node, ціле depth)
{
    якщо (node->wn_u1.index)
    {
	// Done this node before, print the reference.
	PRINTSOME(line1, depth, "(%d)", node->wn_nr, 0);
	PRINTSOME(line2, depth, "    ", 0, 0);
	PRINTSOME(line3, depth, "    ", 0, 0);
	msg(line1);
	msg(line2);
	msg(line3);
    }
    інакше
    {
	node->wn_u1.index = TRUE;

	якщо (node->wn_byte != NUL)
	{
	    якщо (node->wn_child != NULL)
		PRINTSOME(line1, depth, " %c -> ", node->wn_byte, 0);
	    інакше
		// Cannot happen?
		PRINTSOME(line1, depth, " %c ???", node->wn_byte, 0);
	}
	інакше
	    PRINTSOME(line1, depth, " $    ", 0, 0);

	PRINTSOME(line2, depth, "%d/%d    ", node->wn_nr, node->wn_refs);

	якщо (node->wn_sibling != NULL)
	    PRINTSOME(line3, depth, " |    ", 0, 0);
	інакше
	    PRINTSOME(line3, depth, "      ", 0, 0);

	якщо (node->wn_byte == NUL)
	{
	    msg(line1);
	    msg(line2);
	    msg(line3);
	}

	// do the children
	якщо (node->wn_byte != NUL && node->wn_child != NULL)
	    spell_print_node(node->wn_child, depth + 1);

	// do the siblings
	якщо (node->wn_sibling != NULL)
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
    якщо (root == NULL)
	повернути;

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
    ціле		itemcnt;
    char_u	*p;
    ціле		lnum = 0;
    affheader_T	*cur_aff = NULL;
    ціле		did_postpone_prefix = FALSE;
    ціле		aff_todo = 0;
    hashtab_T	*tp;
    char_u	*low = NULL;
    char_u	*fol = NULL;
    char_u	*upp = NULL;
    ціле		do_rep;
    ціле		do_repsal;
    ціле		do_sal;
    ціле		do_mapline;
    ціле		found_map = FALSE;
    hashitem_T	*hi;
    ціле		l;
    ціле		compminlen = 0;		// COMPOUNDMIN value
    ціле		compsylmax = 0;		// COMPOUNDSYLMAX value
    ціле		compoptions = 0;	// COMP_ flags
    ціле		compmax = 0;		// COMPOUNDWORDMAX value
    char_u	*compflags = NULL;	// COMPOUNDFLAG and COMPOUNDRULE
					// concatenated
    char_u	*midword = NULL;	// MIDWORD value
    char_u	*syllable = NULL;	// SYLLABLE value
    char_u	*sofofrom = NULL;	// SOFOFROM value
    char_u	*sofoto = NULL;		// SOFOTO value

    /*
     * Open the file.
     */
    fd = mch_fopen((char *)fname, "r");
    якщо (fd == NULL)
    {
	semsg(_(e_cant_open_file_str), fname);
	повернути NULL;
    }

    vim_snprintf((char *)IObuff, IOSIZE, _("Reading affix file %s..."), fname);
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
    aff = (afffile_T *)getroom(spin, sizeof(afffile_T), TRUE);
    якщо (aff == NULL)
    {
	fclose(fd);
	повернути NULL;
    }
    hash_init(&aff->af_pref);
    hash_init(&aff->af_suff);
    hash_init(&aff->af_comp);

    /*
     * Read all the lines in the file one by one.
     */
    поки (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	line_breakcheck();
	++lnum;

	// Skip comment lines.
	якщо (*rline == '#')
	    continue;

	// Convert from "SET" to 'encoding' when needed.
	vim_free(pc);
	якщо (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, NULL);
	    якщо (pc == NULL)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
							   fname, lnum, rline);
		continue;
	    }
	    line = pc;
	}
	інакше
	{
	    pc = NULL;
	    line = rline;
	}

	// Split the line up in white separated items.  Put a NUL after each
	// item.
	itemcnt = 0;
	для (p = line; ; )
	{
	    поки (*p != NUL && *p <= ' ')  // skip white space and CR/NL
		++p;
	    якщо (*p == NUL)
		break;
	    якщо (itemcnt == MAXITEMCNT)	    // too many items
		break;
	    items[itemcnt++] = p;
	    // A few items have arbitrary text argument, don't split them.
	    якщо (itemcnt == 2 && spell_info_item(items[0]))
		поки (*p >= ' ' || *p == TAB)    // skip until CR/NL
		    ++p;
	    інакше
		поки (*p > ' ')    // skip until white space or CR/NL
		    ++p;
	    якщо (*p == NUL)
		break;
	    *p++ = NUL;
	}

	// Handle non-empty lines.
	якщо (itemcnt > 0)
	{
	    якщо (is_aff_rule(items, itemcnt, "SET", 2) && aff->af_enc == NULL)
	    {
		// Setup for conversion from "ENC" to 'encoding'.
		aff->af_enc = enc_canonize(items[1]);
		якщо (aff->af_enc != NULL && !spin->si_ascii
			&& convert_setup(&spin->si_conv, aff->af_enc,
							       p_enc) == FAIL)
		    smsg(_("Conversion in %s not supported: from %s to %s"),
					       fname, aff->af_enc, p_enc);
		spin->si_conv.vc_fail = TRUE;
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "FLAG", 2)
					      && aff->af_flagtype == AFT_CHAR)
	    {
		якщо (STRCMP(items[1], "long") == 0)
		    aff->af_flagtype = AFT_LONG;
		інакше якщо (STRCMP(items[1], "num") == 0)
		    aff->af_flagtype = AFT_NUM;
		інакше якщо (STRCMP(items[1], "caplong") == 0)
		    aff->af_flagtype = AFT_CAPLONG;
		інакше
		    smsg(_("Invalid value for FLAG in %s line %d: %s"),
			    fname, lnum, items[1]);
		якщо (aff->af_rare != 0
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
	    інакше якщо (spell_info_item(items[0]))
	    {
		    p = (char_u *)getroom(spin,
			    (spin->si_info == NULL ? 0 : STRLEN(spin->si_info))
			    + STRLEN(items[0])
			    + STRLEN(items[1]) + 3, FALSE);
		    якщо (p != NULL)
		    {
			якщо (spin->si_info != NULL)
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
	    інакше якщо (is_aff_rule(items, itemcnt, "MIDWORD", 2)
							   && midword == NULL)
	    {
		midword = getroom_save(spin, items[1]);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "TRY", 2))
	    {
		// ignored, we look in the tree for what chars may appear
	    }
	    // TODO: remove "RAR" later
	    інакше якщо ((is_aff_rule(items, itemcnt, "RAR", 2)
			|| is_aff_rule(items, itemcnt, "RARE", 2))
							 && aff->af_rare == 0)
	    {
		aff->af_rare = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    // TODO: remove "KEP" later
	    інакше якщо ((is_aff_rule(items, itemcnt, "KEP", 2)
			|| is_aff_rule(items, itemcnt, "KEEPCASE", 2))
						     && aff->af_keepcase == 0)
	    {
		aff->af_keepcase = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    інакше якщо ((is_aff_rule(items, itemcnt, "BAD", 2)
			|| is_aff_rule(items, itemcnt, "FORBIDDENWORD", 2))
							  && aff->af_bad == 0)
	    {
		aff->af_bad = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "NEEDAFFIX", 2)
						    && aff->af_needaffix == 0)
	    {
		aff->af_needaffix = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "CIRCUMFIX", 2)
						    && aff->af_circumfix == 0)
	    {
		aff->af_circumfix = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "NOSUGGEST", 2)
						    && aff->af_nosuggest == 0)
	    {
		aff->af_nosuggest = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    інакше якщо ((is_aff_rule(items, itemcnt, "NEEDCOMPOUND", 2)
			|| is_aff_rule(items, itemcnt, "ONLYINCOMPOUND", 2))
						     && aff->af_needcomp == 0)
	    {
		aff->af_needcomp = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "COMPOUNDROOT", 2)
						     && aff->af_comproot == 0)
	    {
		aff->af_comproot = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "COMPOUNDFORBIDFLAG", 2)
						   && aff->af_compforbid == 0)
	    {
		aff->af_compforbid = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
		якщо (aff->af_pref.ht_used > 0)
		    smsg(_("Defining COMPOUNDFORBIDFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "COMPOUNDPERMITFLAG", 2)
						   && aff->af_comppermit == 0)
	    {
		aff->af_comppermit = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
		якщо (aff->af_pref.ht_used > 0)
		    smsg(_("Defining COMPOUNDPERMITFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "COMPOUNDFLAG", 2)
							 && compflags == NULL)
	    {
		// Turn flag "c" into COMPOUNDRULE compatible string "c+",
		// "Na" into "Na+", "1234" into "1234+".
		p = getroom(spin, STRLEN(items[1]) + 2, FALSE);
		якщо (p != NULL)
		{
		    STRCPY(p, items[1]);
		    STRCAT(p, "+");
		    compflags = p;
		}
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "COMPOUNDRULES", 2))
	    {
		// We don't use the count, but do check that it's a number and
		// not COMPOUNDRULE mistyped.
		якщо (atoi((char *)items[1]) == 0)
		    smsg(_("Wrong COMPOUNDRULES value in %s line %d: %s"),
						       fname, lnum, items[1]);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "COMPOUNDRULE", 2))
	    {
		// Don't use the first rule if it is a number.
		якщо (compflags != NULL || *skipdigits(items[1]) != NUL)
		{
		    // Concatenate this string to previously defined ones,
		    // using a slash to separate them.
		    l = (ціле)STRLEN(items[1]) + 1;
		    якщо (compflags != NULL)
			l += (ціле)STRLEN(compflags) + 1;
		    p = getroom(spin, l, FALSE);
		    якщо (p != NULL)
		    {
			якщо (compflags != NULL)
			{
			    STRCPY(p, compflags);
			    STRCAT(p, "/");
			}
			STRCAT(p, items[1]);
			compflags = p;
		    }
		}
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "COMPOUNDWORDMAX", 2)
							      && compmax == 0)
	    {
		compmax = atoi((char *)items[1]);
		якщо (compmax == 0)
		    smsg(_("Wrong COMPOUNDWORDMAX value in %s line %d: %s"),
						       fname, lnum, items[1]);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "COMPOUNDMIN", 2)
							   && compminlen == 0)
	    {
		compminlen = atoi((char *)items[1]);
		якщо (compminlen == 0)
		    smsg(_("Wrong COMPOUNDMIN value in %s line %d: %s"),
						       fname, lnum, items[1]);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "COMPOUNDSYLMAX", 2)
							   && compsylmax == 0)
	    {
		compsylmax = atoi((char *)items[1]);
		якщо (compsylmax == 0)
		    smsg(_("Wrong COMPOUNDSYLMAX value in %s line %d: %s"),
						       fname, lnum, items[1]);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDDUP", 1))
	    {
		compoptions |= COMP_CHECKDUP;
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDREP", 1))
	    {
		compoptions |= COMP_CHECKREP;
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDCASE", 1))
	    {
		compoptions |= COMP_CHECKCASE;
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDTRIPLE", 1))
	    {
		compoptions |= COMP_CHECKTRIPLE;
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDPATTERN", 2))
	    {
		якщо (atoi((char *)items[1]) == 0)
		    smsg(_("Wrong CHECKCOMPOUNDPATTERN value in %s line %d: %s"),
						       fname, lnum, items[1]);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDPATTERN", 3))
	    {
		garray_T    *gap = &spin->si_comppat;
		ціле	    i;

		// Only add the couple if it isn't already there.
		для (i = 0; i < gap->ga_len - 1; i += 2)
		    якщо (STRCMP(((char_u **)(gap->ga_data))[i], items[1]) == 0
			    && STRCMP(((char_u **)(gap->ga_data))[i + 1],
							       items[2]) == 0)
			break;
		якщо (i >= gap->ga_len && ga_grow(gap, 2) == OK)
		{
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, items[1]);
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, items[2]);
		}
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "SYLLABLE", 2)
							  && syllable == NULL)
	    {
		syllable = getroom_save(spin, items[1]);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "NOBREAK", 1))
	    {
		spin->si_nobreak = TRUE;
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "NOSPLITSUGS", 1))
	    {
		spin->si_nosplitsugs = TRUE;
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "NOCOMPOUNDSUGS", 1))
	    {
		spin->si_nocompoundsugs = TRUE;
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "NOSUGFILE", 1))
	    {
		spin->si_nosugfile = TRUE;
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "PFXPOSTPONE", 1))
	    {
		aff->af_pfxpostpone = TRUE;
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "IGNOREEXTRA", 1))
	    {
		aff->af_ignoreextra = TRUE;
	    }
	    інакше якщо ((STRCMP(items[0], "PFX") == 0
					      || STRCMP(items[0], "SFX") == 0)
		    && aff_todo == 0
		    && itemcnt >= 4)
	    {
		ціле	lasti = 4;
		char_u	key[AH_KEY_LEN];

		якщо (*items[0] == 'P')
		    tp = &aff->af_pref;
		інакше
		    tp = &aff->af_suff;

		// Myspell allows the same affix name to be used multiple
		// times.  The affix files that do this have an undocumented
		// "S" flag on all but the last block, thus we check for that
		// and store it in ah_follows.
		vim_strncpy(key, items[1], AH_KEY_LEN - 1);
		hi = hash_find(tp, key);
		якщо (!HASHITEM_EMPTY(hi))
		{
		    cur_aff = HI2AH(hi);
		    якщо (cur_aff->ah_combine != (*items[2] == 'Y'))
			smsg(_("Different combining flag in continued affix block in %s line %d: %s"),
						   fname, lnum, items[1]);
		    якщо (!cur_aff->ah_follows)
			smsg(_("Duplicate affix in %s line %d: %s"),
						       fname, lnum, items[1]);
		}
		інакше
		{
		    // New affix letter.
		    cur_aff = (affheader_T *)getroom(spin,
						   sizeof(affheader_T), TRUE);
		    якщо (cur_aff == NULL)
			break;
		    cur_aff->ah_flag = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
		    якщо (cur_aff->ah_flag == 0 || STRLEN(items[1]) >= AH_KEY_LEN)
			break;
		    якщо (cur_aff->ah_flag == aff->af_bad
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
		якщо (itemcnt > lasti && STRCMP(items[lasti], "S") == 0)
		{
		    ++lasti;
		    cur_aff->ah_follows = TRUE;
		}
		інакше
		    cur_aff->ah_follows = FALSE;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter.
		якщо (itemcnt > lasti && *items[lasti] != '#')
		    smsg(_(e_afftrailing), fname, lnum, items[lasti]);

		якщо (STRCMP(items[2], "Y") != 0 && STRCMP(items[2], "N") != 0)
		    smsg(_("Expected Y or N in %s line %d: %s"),
						       fname, lnum, items[2]);

		якщо (*items[0] == 'P' && aff->af_pfxpostpone)
		{
		    якщо (cur_aff->ah_newID == 0)
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
		    інакше
			// Did use the ID in a previous block.
			did_postpone_prefix = TRUE;
		}

		aff_todo = atoi((char *)items[3]);
	    }
	    інакше якщо ((STRCMP(items[0], "PFX") == 0
					      || STRCMP(items[0], "SFX") == 0)
		    && aff_todo > 0
		    && STRCMP(cur_aff->ah_key, items[1]) == 0
		    && itemcnt >= 5)
	    {
		affentry_T	*aff_entry;
		ціле		upper = FALSE;
		ціле		lasti = 5;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter,
		// unless IGNOREEXTRA is used.  Hunspell uses a "-" item.
		якщо (itemcnt > lasti
			&& !aff->af_ignoreextra
			&& *items[lasti] != '#'
			&& (STRCMP(items[lasti], "-") != 0
						     || itemcnt != lasti + 1))
		    smsg(_(e_afftrailing), fname, lnum, items[lasti]);

		// New item for an affix letter.
		--aff_todo;
		aff_entry = (affentry_T *)getroom(spin,
						    sizeof(affentry_T), TRUE);
		якщо (aff_entry == NULL)
		    break;

		якщо (STRCMP(items[2], "0") != 0)
		    aff_entry->ae_chop = getroom_save(spin, items[2]);
		якщо (STRCMP(items[3], "0") != 0)
		{
		    aff_entry->ae_add = getroom_save(spin, items[3]);

		    // Recognize flags on the affix: abcd/XYZ
		    aff_entry->ae_flags = vim_strchr(aff_entry->ae_add, '/');
		    якщо (aff_entry->ae_flags != NULL)
		    {
			*aff_entry->ae_flags++ = NUL;
			aff_process_flags(aff, aff_entry);
		    }
		}

		// Don't use an affix entry with non-ASCII characters when
		// "spin->si_ascii" is TRUE.
		якщо (!spin->si_ascii || !(has_non_ascii(aff_entry->ae_chop)
					  || has_non_ascii(aff_entry->ae_add)))
		{
		    aff_entry->ae_next = cur_aff->ah_first;
		    cur_aff->ah_first = aff_entry;

		    якщо (STRCMP(items[4], ".") != 0)
		    {
			char_u	buf[MAXLINELEN];

			aff_entry->ae_cond = getroom_save(spin, items[4]);
			// Note: this silently truncates the buffer, but this should
			// not happen in practice
			якщо (*items[0] == 'P')
			    vim_snprintf((char *)buf, sizeof(buf), "^%s", items[4]);
			інакше
			    vim_snprintf((char *)buf, sizeof(buf), "%s$", items[4]);
			aff_entry->ae_prog = vim_regcomp(buf,
					    RE_MAGIC + RE_STRING + RE_STRICT);
			якщо (aff_entry->ae_prog == NULL)
			    smsg(_("Broken condition in %s line %d: %s"),
						       fname, lnum, items[4]);
		    }

		    // For postponed prefixes we need an entry in si_prefcond
		    // for the condition.  Use an existing one if possible.
		    // Can't be done for an affix with flags, ignoring
		    // COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG.
		    якщо (*items[0] == 'P' && aff->af_pfxpostpone
					       && aff_entry->ae_flags == NULL)
		    {
			// When the chop string is one lower-case letter and
			// the add string ends in the upper-case letter we set
			// the "upper" flag, clear "ae_chop" and remove the
			// letters from "ae_add".  The condition must either
			// be empty or start with the same letter.
			якщо (aff_entry->ae_chop != NULL
				&& aff_entry->ae_add != NULL
				&& aff_entry->ae_chop[(*mb_ptr2len)(
						   aff_entry->ae_chop)] == NUL)
			{
			    ціле		c, c_up;

			    c = PTR2CHAR(aff_entry->ae_chop);
			    c_up = SPELL_TOUPPER(c);
			    якщо (c_up != c
				    && (aff_entry->ae_cond == NULL
					|| PTR2CHAR(aff_entry->ae_cond) == c))
			    {
				p = aff_entry->ae_add
						  + STRLEN(aff_entry->ae_add);
				MB_PTR_BACK(aff_entry->ae_add, p);
				якщо (PTR2CHAR(p) == c_up)
				{
				    upper = TRUE;
				    aff_entry->ae_chop = NULL;
				    *p = NUL;

				    // The condition is matched with the
				    // actual word, thus must check for the
				    // upper-case letter.
				    якщо (aff_entry->ae_cond != NULL)
				    {
					char_u	buf[MAXLINELEN];

					якщо (has_mbyte)
					{
					    onecap_copy(items[4], buf, TRUE);
					    aff_entry->ae_cond = getroom_save(
								   spin, buf);
					}
					інакше
					    *aff_entry->ae_cond = c_up;
					якщо (aff_entry->ae_cond != NULL)
					{
					    sprintf((char *)buf, "^%s",
							  aff_entry->ae_cond);
					    vim_regfree(aff_entry->ae_prog);
					    aff_entry->ae_prog = vim_regcomp(
						    buf, RE_MAGIC + RE_STRING);
					}
				    }
				}
			    }
			}

			якщо (aff_entry->ae_chop == NULL
					       && aff_entry->ae_flags == NULL)
			{
			    ціле		idx;
			    char_u	**pp;
			    ціле		n;

			    // Find a previously used condition.
			    для (idx = spin->si_prefcond.ga_len - 1; idx >= 0;
									--idx)
			    {
				p = ((char_u **)spin->si_prefcond.ga_data)[idx];
				якщо (str_equal(p, aff_entry->ae_cond))
				    break;
			    }
			    якщо (idx < 0 && ga_grow(&spin->si_prefcond, 1) == OK)
			    {
				// Not found, add a new condition.
				idx = spin->si_prefcond.ga_len++;
				pp = ((char_u **)spin->si_prefcond.ga_data)
									+ idx;
				якщо (aff_entry->ae_cond == NULL)
				    *pp = NULL;
				інакше
				    *pp = getroom_save(spin,
							  aff_entry->ae_cond);
			    }

			    // Add the prefix to the prefix tree.
			    якщо (aff_entry->ae_add == NULL)
				p = (char_u *)"";
			    інакше
				p = aff_entry->ae_add;

			    // PFX_FLAGS is a negative number, so that
			    // tree_add_word() knows this is the prefix tree.
			    n = PFX_FLAGS;
			    якщо (!cur_aff->ah_combine)
				n |= WFP_NC;
			    якщо (upper)
				n |= WFP_UP;
			    якщо (aff_entry->ae_comppermit)
				n |= WFP_COMPPERMIT;
			    якщо (aff_entry->ae_compforbid)
				n |= WFP_COMPFORBID;
			    tree_add_word(spin, p, spin->si_prefroot, n,
						      idx, cur_aff->ah_newID);
			    did_postpone_prefix = TRUE;
			}

			// Didn't actually use ah_newID, backup si_newprefID.
			якщо (aff_todo == 0 && !did_postpone_prefix)
			{
			    --spin->si_newprefID;
			    cur_aff->ah_newID = 0;
			}
		    }
		}
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "FOL", 2) && fol == NULL)
	    {
		fol = vim_strsave(items[1]);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "LOW", 2) && low == NULL)
	    {
		low = vim_strsave(items[1]);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "UPP", 2) && upp == NULL)
	    {
		upp = vim_strsave(items[1]);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "REP", 2)
		     || is_aff_rule(items, itemcnt, "REPSAL", 2))
	    {
		// Ignore REP/REPSAL count
		якщо (!SAFE_isdigit(*items[1]))
		    smsg(_("Expected REP(SAL) count in %s line %d"),
								 fname, lnum);
	    }
	    інакше якщо ((STRCMP(items[0], "REP") == 0
			|| STRCMP(items[0], "REPSAL") == 0)
		    && itemcnt >= 3)
	    {
		// REP/REPSAL item
		// Myspell ignores extra arguments, we require it starts with
		// # to detect mistakes.
		якщо (itemcnt > 3 && items[3][0] != '#')
		    smsg(_(e_afftrailing), fname, lnum, items[3]);
		якщо (items[0][3] == 'S' ? do_repsal : do_rep)
		{
		    // Replace underscore with space (can't include a space
		    // directly).
		    для (p = items[1]; *p != NUL; MB_PTR_ADV(p))
			якщо (*p == '_')
			    *p = ' ';
		    для (p = items[2]; *p != NUL; MB_PTR_ADV(p))
			якщо (*p == '_')
			    *p = ' ';
		    add_fromto(spin, items[0][3] == 'S'
					 ? &spin->si_repsal
					 : &spin->si_rep, items[1], items[2]);
		}
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "MAP", 2))
	    {
		// MAP item or count
		якщо (!found_map)
		{
		    // First line contains the count.
		    found_map = TRUE;
		    якщо (!SAFE_isdigit(*items[1]))
			smsg(_("Expected MAP count in %s line %d"),
								 fname, lnum);
		}
		інакше якщо (do_mapline)
		{
		    ціле		c;

		    // Check that every character appears only once.
		    для (p = items[1]; *p != NUL; )
		    {
			c = mb_ptr2char_adv(&p);
			якщо ((spin->si_map.ga_len > 0
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
	    інакше якщо (is_aff_rule(items, itemcnt, "SAL", 3))
	    {
		якщо (do_sal)
		{
		    // SAL item (sounds-a-like)
		    // Either one of the known keys or a from-to pair.
		    якщо (STRCMP(items[1], "followup") == 0)
			spin->si_followup = sal_to_bool(items[2]);
		    інакше якщо (STRCMP(items[1], "collapse_result") == 0)
			spin->si_collapse = sal_to_bool(items[2]);
		    інакше якщо (STRCMP(items[1], "remove_accents") == 0)
			spin->si_rem_accents = sal_to_bool(items[2]);
		    інакше
			// when "to" is "_" it means empty
			add_fromto(spin, &spin->si_sal, items[1],
				     STRCMP(items[2], "_") == 0 ? (char_u *)""
								: items[2]);
		}
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "SOFOFROM", 2)
							  && sofofrom == NULL)
	    {
		sofofrom = getroom_save(spin, items[1]);
	    }
	    інакше якщо (is_aff_rule(items, itemcnt, "SOFOTO", 2)
							    && sofoto == NULL)
	    {
		sofoto = getroom_save(spin, items[1]);
	    }
	    інакше якщо (STRCMP(items[0], "COMMON") == 0)
	    {
		ціле	i;

		для (i = 1; i < itemcnt; ++i)
		{
		    якщо (HASHITEM_EMPTY(hash_find(&spin->si_commonwords,
								   items[i])))
		    {
			p = vim_strsave(items[i]);
			якщо (p == NULL)
			    break;
			hash_add(&spin->si_commonwords, p, "spelling");
		    }
		}
	    }
	    інакше
		smsg(_("Unrecognized or duplicate item in %s line %d: %s"),
						       fname, lnum, items[0]);
	}
    }

    якщо (fol != NULL || low != NULL || upp != NULL)
    {
	якщо (spin->si_clear_chartab)
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
	якщо (!spin->si_ascii && !enc_utf8)
	{
	    якщо (fol == NULL || low == NULL || upp == NULL)
		smsg(_("Missing FOL/LOW/UPP line in %s"), fname);
	    інакше
		(void)set_spell_chartab(fol, low, upp);
	}

	vim_free(fol);
	vim_free(low);
	vim_free(upp);
    }

    // Use compound specifications of the .aff file for the spell info.
    якщо (compmax != 0)
    {
	aff_check_number(spin->si_compmax, compmax, "COMPOUNDWORDMAX");
	spin->si_compmax = compmax;
    }

    якщо (compminlen != 0)
    {
	aff_check_number(spin->si_compminlen, compminlen, "COMPOUNDMIN");
	spin->si_compminlen = compminlen;
    }

    якщо (compsylmax != 0)
    {
	якщо (syllable == NULL)
	    smsg(_("COMPOUNDSYLMAX used without SYLLABLE"));
	aff_check_number(spin->si_compsylmax, compsylmax, "COMPOUNDSYLMAX");
	spin->si_compsylmax = compsylmax;
    }

    якщо (compoptions != 0)
    {
	aff_check_number(spin->si_compoptions, compoptions, "COMPOUND options");
	spin->si_compoptions |= compoptions;
    }

    якщо (compflags != NULL)
	process_compflags(spin, aff, compflags);

    // Check that we didn't use too many renumbered flags.
    якщо (spin->si_newcompID < spin->si_newprefID)
    {
	якщо (spin->si_newcompID == 127 || spin->si_newcompID == 255)
	    msg(_("Too many postponed prefixes"));
	інакше якщо (spin->si_newprefID == 0 || spin->si_newprefID == 127)
	    msg(_("Too many compound flags"));
	інакше
	    msg(_("Too many postponed prefixes and/or compound flags"));
    }

    якщо (syllable != NULL)
    {
	aff_check_string(spin->si_syllable, syllable, "SYLLABLE");
	spin->si_syllable = syllable;
    }

    якщо (sofofrom != NULL || sofoto != NULL)
    {
	якщо (sofofrom == NULL || sofoto == NULL)
	    smsg(_("Missing SOFO%s line in %s"),
				     sofofrom == NULL ? "FROM" : "TO", fname);
	інакше якщо (spin->si_sal.ga_len > 0)
	    smsg(_("Both SAL and SOFO lines in %s"), fname);
	інакше
	{
	    aff_check_string(spin->si_sofofr, sofofrom, "SOFOFROM");
	    aff_check_string(spin->si_sofoto, sofoto, "SOFOTO");
	    spin->si_sofofr = sofofrom;
	    spin->si_sofoto = sofoto;
	}
    }

    якщо (midword != NULL)
    {
	aff_check_string(spin->si_midword, midword, "MIDWORD");
	spin->si_midword = midword;
    }

    vim_free(pc);
    fclose(fd);
    повернути aff;
}

/*
 * Return TRUE when items[0] equals "rulename", there are "mincount" items or
 * a comment is following after item "mincount".
 */
    static ціле
is_aff_rule(
    char_u	**items,
    ціле		itemcnt,
    char	*rulename,
    ціле		mincount)
{
    повернути (STRCMP(items[0], rulename) == 0
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

    якщо (entry->ae_flags != NULL
		&& (affile->af_compforbid != 0 || affile->af_comppermit != 0))
    {
	для (p = entry->ae_flags; *p != NUL; )
	{
	    prevp = p;
	    flag = get_affitem(affile->af_flagtype, &p);
	    якщо (flag == affile->af_comppermit || flag == affile->af_compforbid)
	    {
		STRMOVE(prevp, p);
		p = prevp;
		якщо (flag == affile->af_comppermit)
		    entry->ae_comppermit = TRUE;
		інакше
		    entry->ae_compforbid = TRUE;
	    }
	    якщо (affile->af_flagtype == AFT_NUM && *p == ',')
		++p;
	}
	якщо (*entry->ae_flags == NUL)
	    entry->ae_flags = NULL;	// nothing left
    }
}

/*
 * Return TRUE if "s" is the name of an info item in the affix file.
 */
    static ціле
spell_info_item(char_u *s)
{
    повернути STRCMP(s, "NAME") == 0
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
    ціле		flagtype,
    char_u	*item,
    char_u	*fname,
    ціле		lnum)
{
    unsigned	res;
    char_u	*p = item;

    res = get_affitem(flagtype, &p);
    якщо (res == 0)
    {
	якщо (flagtype == AFT_NUM)
	    smsg(_("Flag is not a number in %s line %d: %s"),
							   fname, lnum, item);
	інакше
	    smsg(_("Illegal flag in %s line %d: %s"),
							   fname, lnum, item);
    }
    якщо (*p != NUL)
    {
	smsg(_(e_affname), fname, lnum, item);
	повернути 0;
    }

    повернути res;
}

/*
 * Get one affix name from "*pp" and advance the pointer.
 * Returns ZERO_FLAG for "0".
 * Returns zero for an error, still advances the pointer then.
 */
    static unsigned
get_affitem(ціле flagtype, char_u **pp)
{
    ціле		res;

    якщо (flagtype == AFT_NUM)
    {
	якщо (!VIM_ISDIGIT(**pp))
	{
	    ++*pp;	// always advance, avoid getting stuck
	    повернути 0;
	}
	res = getdigits(pp);
	якщо (res == 0)
	    res = ZERO_FLAG;
    }
    інакше
    {
	res = mb_ptr2char_adv(pp);
	якщо (flagtype == AFT_LONG || (flagtype == AFT_CAPLONG
						 && res >= 'A' && res <= 'Z'))
	{
	    якщо (**pp == NUL)
		повернути 0;
	    res = mb_ptr2char_adv(pp) + (res << 16);
	}
    }
    повернути res;
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
    ціле		id;
    ціле		len;
    char_u	*tp;
    char_u	key[AH_KEY_LEN];
    hashitem_T	*hi;

    // Make room for the old and the new compflags, concatenated with a / in
    // between.  Processing it makes it shorter, but we don't know by how
    // much, thus allocate the maximum.
    len = (ціле)STRLEN(compflags) + 1;
    якщо (spin->si_compflags != NULL)
	len += (ціле)STRLEN(spin->si_compflags) + 1;
    p = getroom(spin, len, FALSE);
    якщо (p == NULL)
	повернути;
    якщо (spin->si_compflags != NULL)
    {
	STRCPY(p, spin->si_compflags);
	STRCAT(p, "/");
    }
    spin->si_compflags = p;
    tp = p + STRLEN(p);

    для (p = compflags; *p != NUL; )
    {
	якщо (vim_strchr((char_u *)"/?*+[]", *p) != NULL)
	    // Copy non-flag characters directly.
	    *tp++ = *p++;
	інакше
	{
	    // First get the flag number, also checks validity.
	    prevp = p;
	    flag = get_affitem(aff->af_flagtype, &p);
	    якщо (flag != 0)
	    {
		// Find the flag in the hashtable.  If it was used before, use
		// the existing ID.  Otherwise add a new entry.
		vim_strncpy(key, prevp, p - prevp);
		hi = hash_find(&aff->af_comp, key);
		якщо (!HASHITEM_EMPTY(hi))
		    id = HI2CI(hi)->ci_newID;
		інакше
		{
		    ci = (compitem_T *)getroom(spin, sizeof(compitem_T), TRUE);
		    якщо (ci == NULL)
			break;
		    STRCPY(ci->ci_key, key);
		    ci->ci_flag = flag;
		    // Avoid using a flag ID that has a special meaning in a
		    // regexp (also inside []).
		    do
		    {
			check_renumber(spin);
			id = spin->si_newcompID--;
		    } поки (vim_strchr((char_u *)"/?*+[]\\-^", id) != NULL);
		    ci->ci_newID = id;
		    hash_add(&aff->af_comp, ci->ci_key, "spelling");
		}
		*tp++ = id;
	    }
	    якщо (aff->af_flagtype == AFT_NUM && *p == ',')
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
    якщо (spin->si_newprefID == spin->si_newcompID && spin->si_newcompID < 128)
    {
	spin->si_newprefID = 127;
	spin->si_newcompID = 255;
    }
}

/*
 * Append one affix or compound ID to "store_afflist".
 * Returns FAIL when this would overrun the fixed-size buffer.
 */
    static ціле
store_afflist_add(
    char_u	*store_afflist,
    ціле		*cntp,
    ціле		id)
{
    якщо (*cntp >= MAXWLEN - 1)
    {
	emsg(_(e_too_many_postponed_prefixes_spell));
	повернути FAIL;
    }
    store_afflist[(*cntp)++] = id;
    store_afflist[*cntp] = NUL;
    повернути OK;
}

/*
 * Return TRUE if flag "flag" appears in affix list "afflist".
 */
    static ціле
flag_in_afflist(ціле flagtype, char_u *afflist, unsigned flag)
{
    char_u	*p;
    unsigned	n;

    switch (flagtype)
    {
	case AFT_CHAR:
	    повернути vim_strchr(afflist, flag) != NULL;

	case AFT_CAPLONG:
	case AFT_LONG:
	    для (p = afflist; *p != NUL; )
	    {
		n = mb_ptr2char_adv(&p);
		якщо ((flagtype == AFT_LONG || (n >= 'A' && n <= 'Z'))
								 && *p != NUL)
		    n = mb_ptr2char_adv(&p) + (n << 16);
		якщо (n == flag)
		    повернути TRUE;
	    }
	    break;

	case AFT_NUM:
	    для (p = afflist; *p != NUL; )
	    {
		n = getdigits(&p);
		якщо (n == 0)
		    n = ZERO_FLAG;
		якщо (n == flag)
		    повернути TRUE;
		якщо (*p != NUL)	// skip over comma
		    ++p;
	    }
	    break;
    }
    повернути FALSE;
}

/*
 * Give a warning when "spinval" and "affval" numbers are set and not the same.
 */
    static void
aff_check_number(ціле spinval, ціле affval, char *name)
{
    якщо (spinval != 0 && spinval != affval)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Give a warning when "spinval" and "affval" strings are set and not the same.
 */
    static void
aff_check_string(char_u *spinval, char_u *affval, char *name)
{
    якщо (spinval != NULL && STRCMP(spinval, affval) != 0)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Return TRUE if strings "s1" and "s2" are equal.  Also consider both being
 * NULL as equal.
 */
    static ціле
str_equal(char_u *s1, char_u *s2)
{
    якщо (s1 == NULL || s2 == NULL)
	повернути s1 == s2;
    повернути STRCMP(s1, s2) == 0;
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

    якщо (ga_grow(gap, 1) == FAIL)
	повернути;

    ftp = ((fromto_T *)gap->ga_data) + gap->ga_len;
    (void)spell_casefold(curwin, from, (ціле)STRLEN(from), word, MAXWLEN);
    ftp->ft_from = getroom_save(spin, word);
    (void)spell_casefold(curwin, to, (ціле)STRLEN(to), word, MAXWLEN);
    ftp->ft_to = getroom_save(spin, word);
    ++gap->ga_len;
}

/*
 * Convert a boolean argument in a SAL line to TRUE or FALSE;
 */
    static ціле
sal_to_bool(char_u *s)
{
    повернути STRCMP(s, "1") == 0 || STRCMP(s, "true") == 0;
}

/*
 * Free the structure filled by spell_read_aff().
 */
    static void
spell_free_aff(afffile_T *aff)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    ціле		todo;
    affheader_T	*ah;
    affentry_T	*ae;

    vim_free(aff->af_enc);

    // All this trouble to free the "ae_prog" items...
    для (ht = &aff->af_pref; ; ht = &aff->af_suff)
    {
	todo = (ціле)ht->ht_used;
	FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
	{
	    якщо (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		ah = HI2AH(hi);
		для (ae = ah->ah_first; ae != NULL; ae = ae->ae_next)
		    vim_regfree(ae->ae_prog);
	    }
	}
	якщо (ht == &aff->af_suff)
	    break;
    }

    hash_clear(&aff->af_pref);
    hash_clear(&aff->af_suff);
    hash_clear(&aff->af_comp);
}

/*
 * Read dictionary file "fname".
 * Returns OK or FAIL;
 */
    static ціле
spell_read_dic(spellinfo_T *spin, char_u *fname, afffile_T *affile)
{
    hashtab_T	ht;
    char_u	line[MAXLINELEN];
    char_u	*p;
    char_u	*afflist;
    char_u	store_afflist[MAXWLEN];
    ціле		pfxlen;
    ціле		totlen;
    ціле		need_affix;
    char_u	*dw;
    char_u	*pc;
    char_u	*w;
    ціле		l;
    hash_T	hash;
    hashitem_T	*hi;
    FILE	*fd;
    ціле		lnum = 1;
    ціле		non_ascii = 0;
    ціле		retval = OK;
    char_u	message[MAXLINELEN + MAXWLEN];
    ціле		flags;
    ціле		duplicate = 0;
    time_T	last_msg_time = 0;

    /*
     * Open the file.
     */
    fd = mch_fopen((char *)fname, "r");
    якщо (fd == NULL)
    {
	semsg(_(e_cant_open_file_str), fname);
	повернути FAIL;
    }

    // The hashtable is only used to detect duplicated words.
    hash_init(&ht);

    vim_snprintf((char *)IObuff, IOSIZE,
				  _("Reading dictionary file %s..."), fname);
    spell_message(spin, IObuff);

    // start with a message for the first line
    spin->si_msg_count = 999999;

    // Read and ignore the first line: word count.
    якщо (vim_fgets(line, MAXLINELEN, fd) || !vim_isdigit(*skipwhite(line)))
	semsg(_(e_no_word_count_in_str), fname);

    /*
     * Read all the lines in the file one by one.
     * The words are converted to 'encoding' here, before being added to
     * the hashtable.
     */
    поки (!vim_fgets(line, MAXLINELEN, fd) && !got_int)
    {
	line_breakcheck();
	++lnum;
	якщо (line[0] == '#' || line[0] == '/')
	    continue;	// comment line

	// Remove CR, LF and white space from the end.  White space halfway
	// the word is kept to allow e.g., "et al.".
	l = (ціле)STRLEN(line);
	поки (l > 0 && line[l - 1] <= ' ')
	    --l;
	якщо (l == 0)
	    continue;	// empty line
	line[l] = NUL;

	// Convert from "SET" to 'encoding' when needed.
	якщо (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, line, NULL);
	    якщо (pc == NULL)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
						       fname, lnum, line);
		continue;
	    }
	    w = pc;
	}
	інакше
	{
	    pc = NULL;
	    w = line;
	}

	// Truncate the word at the "/", set "afflist" to what follows.
	// Replace "\/" by "/" and "\\" by "\".
	afflist = NULL;
	для (p = w; *p != NUL; MB_PTR_ADV(p))
	{
	    якщо (*p == '\\' && (p[1] == '\\' || p[1] == '/'))
		STRMOVE(p, p + 1);
	    інакше якщо (*p == '/')
	    {
		*p = NUL;
		afflist = p + 1;
		break;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	якщо (spin->si_ascii && has_non_ascii(w))
	{
	    ++non_ascii;
	    vim_free(pc);
	    continue;
	}

	// This takes time, print a message every 10000 words, but not more
	// often than once per second.
	якщо (spin->si_verbose && spin->si_msg_count > 10000)
	{
	    spin->si_msg_count = 0;
	    якщо (vim_time() > last_msg_time)
	    {
		last_msg_time = vim_time();
		vim_snprintf((char *)message, sizeof(message),
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
	якщо (dw == NULL)
	{
	    retval = FAIL;
	    vim_free(pc);
	    break;
	}

	hash = hash_hash(dw);
	hi = hash_lookup(&ht, dw, hash);
	якщо (!HASHITEM_EMPTY(hi))
	{
	    якщо (p_verbose > 0)
		smsg(_("Duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    інакше якщо (duplicate == 0)
		smsg(_("First duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ++duplicate;
	}
	інакше
	    hash_add_item(&ht, hi, dw, hash);

	flags = 0;
	store_afflist[0] = NUL;
	pfxlen = 0;
	totlen = 0;
	need_affix = FALSE;
	якщо (afflist != NULL)
	{
	    // Extract flags from the affix list.
	    flags |= get_affix_flags(affile, afflist);

	    якщо (affile->af_needaffix != 0 && flag_in_afflist(
			  affile->af_flagtype, afflist, affile->af_needaffix))
		need_affix = TRUE;

	    якщо (affile->af_pfxpostpone)
	    {
		// Need to store the list of prefix IDs with the word.
		якщо (get_pfxlist(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    break;
		}
		pfxlen = totlen;
	    }

	    якщо (spin->si_compflags != NULL)
	    {
		// Need to store the list of compound flags with the word.
		// Concatenate them to the list of prefix IDs.
		якщо (get_compflags(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    break;
		}
	    }
	}

	// Add the word to the word tree(s).
	якщо (store_word(spin, dw, flags, spin->si_region,
					   store_afflist, need_affix) == FAIL)
	    retval = FAIL;

	якщо (afflist != NULL)
	{
	    // Find all matching suffixes and add the resulting words.
	    // Additionally do matching prefixes that combine.
	    якщо (store_aff_word(spin, dw, afflist, affile,
			   &affile->af_suff, &affile->af_pref,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;

	    // Find all matching prefixes and add the resulting words.
	    якщо (store_aff_word(spin, dw, afflist, affile,
			  &affile->af_pref, NULL,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;
	}

	vim_free(pc);
    }

    якщо (duplicate > 0)
	smsg(_("%d duplicate word(s) in %s"), duplicate, fname);
    якщо (spin->si_ascii && non_ascii > 0)
	smsg(_("Ignored %d word(s) with non-ASCII characters in %s"),
							    non_ascii, fname);
    hash_clear(&ht);

    fclose(fd);
    повернути retval;
}

/*
 * Check for affix flags in "afflist" that are turned into word flags.
 * Return WF_ flags.
 */
    static ціле
get_affix_flags(afffile_T *affile, char_u *afflist)
{
    ціле		flags = 0;

    якщо (affile->af_keepcase != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_keepcase))
	flags |= WF_KEEPCAP | WF_FIXCAP;
    якщо (affile->af_rare != 0 && flag_in_afflist(
			       affile->af_flagtype, afflist, affile->af_rare))
	flags |= WF_RARE;
    якщо (affile->af_bad != 0 && flag_in_afflist(
				affile->af_flagtype, afflist, affile->af_bad))
	flags |= WF_BANNED;
    якщо (affile->af_needcomp != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_needcomp))
	flags |= WF_NEEDCOMP;
    якщо (affile->af_comproot != 0 && flag_in_afflist(
			   affile->af_flagtype, afflist, affile->af_comproot))
	flags |= WF_COMPROOT;
    якщо (affile->af_nosuggest != 0 && flag_in_afflist(
			  affile->af_flagtype, afflist, affile->af_nosuggest))
	flags |= WF_NOSUGGEST;
    повернути flags;
}

/*
 * Get the list of prefix IDs from the affix list "afflist".
 * Used for PFXPOSTPONE.
 * Put the resulting flags in "store_afflist[MAXWLEN]" with a terminating NUL.
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    static ціле
get_pfxlist(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    ціле		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    ціле		id;
    char_u	key[AH_KEY_LEN];
    hashitem_T	*hi;

    для (p = afflist; *p != NUL; )
    {
	prevp = p;
	якщо (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a postponed prefix flag if it appears in "af_pref"
	    // and its ID is not zero.
	    vim_strncpy(key, prevp, p - prevp);
	    hi = hash_find(&affile->af_pref, key);
	    якщо (!HASHITEM_EMPTY(hi))
	    {
		id = HI2AH(hi)->ah_newID;
		якщо (id != 0 && store_afflist_add(store_afflist, cntp, id) == FAIL)
		    повернути FAIL;
	    }
	}
	якщо (affile->af_flagtype == AFT_NUM && *p == ',')
	    ++p;
    }

    повернути OK;
}

/*
 * Get the list of compound IDs from the affix list "afflist" that are used
 * for compound words.
 * Puts the flags in "store_afflist[]".
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    static ціле
get_compflags(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    ціле		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    char_u	key[AH_KEY_LEN];
    hashitem_T	*hi;

    для (p = afflist; *p != NUL; )
    {
	prevp = p;
	якщо (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a compound flag if it appears in "af_comp".
	    vim_strncpy(key, prevp, p - prevp);
	    hi = hash_find(&affile->af_comp, key);
	    якщо (!HASHITEM_EMPTY(hi)
		    && store_afflist_add(store_afflist, cntp,
					      HI2CI(hi)->ci_newID) == FAIL)
		повернути FAIL;
	}
	якщо (affile->af_flagtype == AFT_NUM && *p == ',')
	    ++p;
    }

    повернути OK;
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
    static ціле
store_aff_word(
    spellinfo_T	*spin,		// spell info
    char_u	*word,		// basic word start
    char_u	*afflist,	// list of names of supported affixes
    afffile_T	*affile,
    hashtab_T	*ht,
    hashtab_T	*xht,
    ціле		condit,		// CONDIT_SUF et al.
    ціле		flags,		// flags for the word
    char_u	*pfxlist,	// list of prefix IDs
    ціле		pfxlen)		// nr of flags in "pfxlist" for prefixes, rest
				// is compound flags
{
    ціле		todo;
    hashitem_T	*hi;
    affheader_T	*ah;
    affentry_T	*ae;
    char_u	newword[MAXWLEN];
    ціле		retval = OK;
    ціле		i, j;
    char_u	*p;
    ціле		use_flags;
    char_u	*use_pfxlist;
    ціле		use_pfxlen;
    ціле		need_affix;
    char_u	store_afflist[MAXWLEN];
    char_u	pfx_pfxlist[MAXWLEN];
    size_t	wordlen = STRLEN(word);
    ціле		use_condit;

    todo = (ціле)ht->ht_used;
    для (hi = ht->ht_array; todo > 0 && retval == OK; ++hi)
    {
	якщо (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ah = HI2AH(hi);

	    // Check that the affix combines, if required, and that the word
	    // supports this affix.
	    якщо (((condit & CONDIT_COMB) == 0 || ah->ah_combine)
		    && flag_in_afflist(affile->af_flagtype, afflist,
								 ah->ah_flag))
	    {
		// Loop over all affix entries with this name.
		для (ae = ah->ah_first; ae != NULL; ae = ae->ae_next)
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
		    якщо ((xht != NULL || !affile->af_pfxpostpone
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
			якщо (xht == NULL)
			{
			    // prefix: chop/add at the start of the word
			    якщо (ae->ae_add == NULL)
				*newword = NUL;
			    інакше
				vim_strncpy(newword, ae->ae_add, MAXWLEN - 1);
			    p = word;
			    якщо (ae->ae_chop != NULL)
			    {
				// Skip chop string.
				якщо (has_mbyte)
				{
				    i = mb_charlen(ae->ae_chop);
				    для ( ; i > 0; --i)
					MB_PTR_ADV(p);
				}
				інакше
				    p += STRLEN(ae->ae_chop);
			    }
			    // Note: this silently truncates the buffer, but this should
			    // not happen in practice
			    STRNCAT(newword, p, MAXWLEN - STRLEN(newword) - 1);
			}
			інакше
			{
			    // suffix: chop/add at the end of the word
			    vim_strncpy(newword, word, MAXWLEN - 1);
			    якщо (ae->ae_chop != NULL)
			    {
				// Remove chop string.
				p = newword + STRLEN(newword);
				i = (ціле)MB_CHARLEN(ae->ae_chop);
				для ( ; i > 0; --i)
				    MB_PTR_BACK(newword, p);
				*p = NUL;
			    }
			    якщо (ae->ae_add != NULL)
				// Note: this silently truncates the buffer, but this should
				// not happen in practice
				STRNCAT(newword, ae->ae_add, MAXWLEN - STRLEN(newword) - 1);
			}

			use_flags = flags;
			use_pfxlist = pfxlist;
			use_pfxlen = pfxlen;
			need_affix = FALSE;
			use_condit = condit | CONDIT_COMB | CONDIT_AFF;
			якщо (ae->ae_flags != NULL)
			{
			    // Extract flags from the affix list.
			    use_flags |= get_affix_flags(affile, ae->ae_flags);

			    якщо (affile->af_needaffix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_needaffix))
				need_affix = TRUE;

			    // When there is a CIRCUMFIX flag the other affix
			    // must also have it and we don't add the word
			    // with one affix.
			    якщо (affile->af_circumfix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_circumfix))
			    {
				use_condit |= CONDIT_CFIX;
				якщо ((condit & CONDIT_CFIX) == 0)
				    need_affix = TRUE;
			    }

			    якщо (affile->af_pfxpostpone
						|| spin->si_compflags != NULL)
			    {
				ціле listlen = 0;

				якщо (affile->af_pfxpostpone)
				{
				    // Get prefix IDS from the affix list.
				    якщо (get_pfxlist(affile, ae->ae_flags,
						    store_afflist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					break;
				    }
				    use_pfxlen = listlen;
				}
				інакше
				    use_pfxlen = 0;
				use_pfxlist = store_afflist;

				// Combine the prefix IDs. Avoid adding the
				// same ID twice.
				для (i = 0; i < pfxlen; ++i)
				{
				    для (j = 0; j < use_pfxlen; ++j)
					якщо (pfxlist[i] == use_pfxlist[j])
					    break;
				    якщо (j == use_pfxlen
					    && store_afflist_add(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					break;
				    }
				    use_pfxlen = listlen;
				}
				якщо (retval == FAIL)
				    break;

				якщо (spin->si_compflags != NULL)
				    // Get compound IDS from the affix list.
				    якщо (get_compflags(affile, ae->ae_flags,
						      use_pfxlist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					break;
				    }
				якщо (retval == FAIL)
				    break;

				// Combine the list of compound flags.
				// Concatenate them to the prefix IDs list.
				// Avoid adding the same ID twice.
				для (i = pfxlen; pfxlist[i] != NUL; ++i)
				{
				    для (j = use_pfxlen;
						   use_pfxlist[j] != NUL; ++j)
					якщо (pfxlist[i] == use_pfxlist[j])
					    break;
				    якщо (use_pfxlist[j] == NUL
					    && store_afflist_add(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					break;
				    }
				}
				якщо (retval == FAIL)
				    break;
			    }
			}

			// Obey a "COMPOUNDFORBIDFLAG" of the affix: don't
			// use the compound flags.
			якщо (use_pfxlist != NULL && ae->ae_compforbid)
			{
			    vim_strncpy(pfx_pfxlist, use_pfxlist, use_pfxlen);
			    use_pfxlist = pfx_pfxlist;
			}

			// When there are postponed prefixes...
			якщо (spin->si_prefroot != NULL
				&& spin->si_prefroot->wn_sibling != NULL)
			{
			    // ... add a flag to indicate an affix was used.
			    use_flags |= WF_HAS_AFF;

			    // ... don't use a prefix list if combining
			    // affixes is not allowed.  But do use the
			    // compound flags after them.
			    якщо (!ah->ah_combine && use_pfxlist != NULL)
				use_pfxlist += use_pfxlen;
			}

			// When compounding is supported and there is no
			// "COMPOUNDPERMITFLAG" then forbid compounding on the
			// side where the affix is applied.
			якщо (spin->si_compflags != NULL && !ae->ae_comppermit)
			{
			    якщо (xht != NULL)
				use_flags |= WF_NOCOMPAFT;
			    інакше
				use_flags |= WF_NOCOMPBEF;
			}

			// Store the modified word.
			якщо (store_word(spin, newword, use_flags,
						 spin->si_region, use_pfxlist,
							  need_affix) == FAIL)
			    retval = FAIL;

			// When added a prefix or a first suffix and the affix
			// has flags may add a(nother) suffix.  RECURSIVE!
			якщо ((condit & CONDIT_SUF) && ae->ae_flags != NULL)
			    якщо (store_aff_word(spin, newword, ae->ae_flags,
					affile, &affile->af_suff, xht,
					   use_condit & (xht == NULL
							? ~0 :  ~CONDIT_SUF),
				      use_flags, use_pfxlist, pfxlen) == FAIL)
				retval = FAIL;

			// When added a suffix and combining is allowed also
			// try adding a prefix additionally.  Both for the
			// word flags and for the affix flags.  RECURSIVE!
			якщо (xht != NULL && ah->ah_combine)
			{
			    якщо (store_aff_word(spin, newword,
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

    повернути retval;
}

/*
 * Read a file with a list of words.
 */
    static ціле
spell_read_wordfile(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    long	lnum = 0;
    char_u	rline[MAXLINELEN];
    char_u	*line;
    char_u	*pc = NULL;
    char_u	*p;
    ціле		l;
    ціле		retval = OK;
    ціле		did_word = FALSE;
    ціле		non_ascii = 0;
    ціле		flags;
    ціле		regionmask;

    /*
     * Open the file.
     */
    fd = mch_fopen((char *)fname, "r");
    якщо (fd == NULL)
    {
	semsg(_(e_cant_open_file_str), fname);
	повернути FAIL;
    }

    vim_snprintf((char *)IObuff, IOSIZE, _("Reading word file %s..."), fname);
    spell_message(spin, IObuff);

    /*
     * Read all the lines in the file one by one.
     */
    поки (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	line_breakcheck();
	++lnum;

	// Skip comment lines.
	якщо (*rline == '#')
	    continue;

	// Remove CR, LF and white space from the end.
	l = (ціле)STRLEN(rline);
	поки (l > 0 && rline[l - 1] <= ' ')
	    --l;
	якщо (l == 0)
	    continue;	// empty or blank line
	rline[l] = NUL;

	// Convert from "/encoding={encoding}" to 'encoding' when needed.
	vim_free(pc);
	якщо (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, NULL);
	    якщо (pc == NULL)
	    {
		smsg(_("Conversion failure for word in %s line %ld: %s"),
							   fname, lnum, rline);
		continue;
	    }
	    line = pc;
	}
	інакше
	{
	    pc = NULL;
	    line = rline;
	}

	якщо (*line == '/')
	{
	    ++line;
	    якщо (STRNCMP(line, "encoding=", 9) == 0)
	    {
		якщо (spin->si_conv.vc_type != CONV_NONE)
		    smsg(_("Duplicate /encoding= line ignored in %s line %ld: %s"),
						       fname, lnum, line - 1);
		інакше якщо (did_word)
		    smsg(_("/encoding= line after word ignored in %s line %ld: %s"),
						       fname, lnum, line - 1);
		інакше
		{
		    char_u	*enc;

		    // Setup for conversion to 'encoding'.
		    line += 9;
		    enc = enc_canonize(line);
		    якщо (enc != NULL && !spin->si_ascii
			    && convert_setup(&spin->si_conv, enc,
							       p_enc) == FAIL)
			smsg(_("Conversion in %s not supported: from %s to %s"),
							  fname, line, p_enc);
		    vim_free(enc);
		    spin->si_conv.vc_fail = TRUE;
		}
		continue;
	    }

	    якщо (STRNCMP(line, "regions=", 8) == 0)
	    {
		якщо (spin->si_region_count > 1)
		    smsg(_("Duplicate /regions= line ignored in %s line %ld: %s"),
						       fname, lnum, line);
		інакше
		{
		    line += 8;
		    якщо (STRLEN(line) > MAXREGIONS * 2)
			smsg(_("Too many regions in %s line %ld: %s"),
						       fname, lnum, line);
		    інакше
		    {
			spin->si_region_count = (ціле)STRLEN(line) / 2;
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
	якщо (p != NULL)
	{
	    *p++ = NUL;
	    поки (*p != NUL)
	    {
		якщо (*p == '=')		// keep-case word
		    flags |= WF_KEEPCAP | WF_FIXCAP;
		інакше якщо (*p == '!')	// Bad, bad, wicked word.
		    flags |= WF_BANNED;
		інакше якщо (*p == '?')	// Rare word.
		    flags |= WF_RARE;
		інакше якщо (VIM_ISDIGIT(*p)) // region number(s)
		{
		    якщо ((flags & WF_REGION) == 0)   // first one
			regionmask = 0;
		    flags |= WF_REGION;

		    l = *p - '0';
		    якщо (l == 0 || l > spin->si_region_count)
		    {
			smsg(_("Invalid region nr in %s line %ld: %s"),
							  fname, lnum, p);
			break;
		    }
		    regionmask |= 1 << (l - 1);
		}
		інакше
		{
		    smsg(_("Unrecognized flags in %s line %ld: %s"),
							      fname, lnum, p);
		    break;
		}
		++p;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	якщо (spin->si_ascii && has_non_ascii(line))
	{
	    ++non_ascii;
	    continue;
	}

	// Normal word: store it.
	якщо (store_word(spin, line, flags, regionmask, NULL, FALSE) == FAIL)
	{
	    retval = FAIL;
	    break;
	}
	did_word = TRUE;
    }

    vim_free(pc);
    fclose(fd);

    якщо (spin->si_ascii && non_ascii > 0)
    {
	vim_snprintf((char *)IObuff, IOSIZE,
		  _("Ignored %d words with non-ASCII characters"), non_ascii);
	spell_message(spin, IObuff);
    }

    повернути retval;
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
    ціле		align)		// align for pointer
{
    char_u	*p;
    sblock_T	*bl = spin->si_blocks;

    якщо (align && bl != NULL)
	// Round size up for alignment.  On some systems structures need to be
	// aligned to the size of a pointer (e.g., SPARC).
	bl->sb_used = (bl->sb_used + sizeof(char *) - 1)
						      & ~(sizeof(char *) - 1);

    якщо (bl == NULL || bl->sb_used + len > SBLOCKSIZE)
    {
	якщо (len >= SBLOCKSIZE)
	    bl = NULL;
	інакше
	    // Allocate a block of memory. It is not freed until much later.
	    bl = alloc_clear(offsetof(sblock_T, sb_data) + SBLOCKSIZE + 1);
	якщо (bl == NULL)
	{
	    якщо (!spin->si_did_emsg)
	    {
		emsg(_(e_insufficient_memory_word_list_will_be_incomplete));
		spin->si_did_emsg = TRUE;
	    }
	    повернути NULL;
	}
	bl->sb_next = spin->si_blocks;
	spin->si_blocks = bl;
	bl->sb_used = 0;
	++spin->si_blocks_cnt;
    }

    p = bl->sb_data + bl->sb_used;
    bl->sb_used += (ціле)len;

    повернути p;
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
    якщо (sc != NULL)
	STRCPY(sc, s);
    повернути sc;
}


/*
 * Free the list of allocated sblock_T.
 */
    static void
free_blocks(sblock_T *bl)
{
    sblock_T	*next;

    поки (bl != NULL)
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
    повернути (wordnode_T *)getroom(spin, sizeof(wordnode_T), TRUE);
}

/*
 * Return TRUE if "word" contains valid word characters.
 * Control characters and trailing '/' are invalid.  Space is OK.
 */
    static ціле
valid_spell_word(char_u *word, char_u *end)
{
    char_u *p;

    якщо (enc_utf8 && !utf_valid_string(word, end))
	повернути FALSE;
    для (p = word; *p != NUL && p < end; p += mb_ptr2len(p))
	якщо (*p < ' ' || (p[0] == '/' && p[1] == NUL))
	    повернути FALSE;
    повернути TRUE;
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
    static ціле
store_word(
    spellinfo_T	*spin,
    char_u	*word,
    ціле		flags,		// extra flags, WF_BANNED
    ціле		region,		// supported region(s)
    char_u	*pfxlist,	// list of prefix IDs or NULL
    ціле		need_affix)	// only store word with affix ID
{
    ціле		len = (ціле)STRLEN(word);
    ціле		ct = captype(word, word + len);
    char_u	foldword[MAXWLEN];
    ціле		res = OK;
    char_u	*p;

    // Avoid adding illegal bytes to the word tree.
    якщо (!valid_spell_word(word, word + len))
	повернути FAIL;

    (void)spell_casefold(curwin, word, len, foldword, MAXWLEN);
    для (p = pfxlist; res == OK; ++p)
    {
	якщо (!need_affix || (p != NULL && *p != NUL))
	    res = tree_add_word(spin, foldword, spin->si_foldroot, ct | flags,
						  region, p == NULL ? 0 : *p);
	якщо (p == NULL || *p == NUL)
	    break;
    }
    ++spin->si_foldwcount;

    якщо (res == OK && (ct == WF_KEEPCAP || (flags & WF_KEEPCAP)))
    {
	для (p = pfxlist; res == OK; ++p)
	{
	    якщо (!need_affix || (p != NULL && *p != NUL))
		res = tree_add_word(spin, word, spin->si_keeproot, flags,
						  region, p == NULL ? 0 : *p);
	    якщо (p == NULL || *p == NUL)
		break;
	}
	++spin->si_keepwcount;
    }
    повернути res;
}

/*
 * Add word "word" to a word tree at "root".
 * When "flags" < 0 we are adding to the prefix tree where "flags" is used for
 * "rare" and "region" is the condition nr.
 * Returns FAIL when out of memory.
 */
    static ціле
tree_add_word(
    spellinfo_T	*spin,
    char_u	*word,
    wordnode_T	*root,
    ціле		flags,
    ціле		region,
    ціле		affixID)
{
    wordnode_T	*node = root;
    wordnode_T	*np;
    wordnode_T	*copyp, **copyprev;
    wordnode_T	**prev = NULL;
    ціле		i;

    // Add each byte of the word to the tree, including the NUL at the end.
    для (i = 0; ; ++i)
    {
	// When there is more than one reference to this node we need to make
	// a copy, so that we can modify it.  Copy the whole list of siblings
	// (we don't optimize for a partly shared list of siblings).
	якщо (node != NULL && node->wn_refs > 1)
	{
	    --node->wn_refs;
	    copyprev = prev;
	    FOR_ALL_NODE_SIBLINGS(node, copyp)
	    {
		// Allocate a new node and copy the info.
		np = get_wordnode(spin);
		якщо (np == NULL)
		    повернути FAIL;
		np->wn_child = copyp->wn_child;
		якщо (np->wn_child != NULL)
		    ++np->wn_child->wn_refs;	// child gets extra ref
		np->wn_byte = copyp->wn_byte;
		якщо (np->wn_byte == NUL)
		{
		    np->wn_flags = copyp->wn_flags;
		    np->wn_region = copyp->wn_region;
		    np->wn_affixID = copyp->wn_affixID;
		}

		// Link the new node in the list, there will be one ref.
		np->wn_refs = 1;
		якщо (copyprev != NULL)
		    *copyprev = np;
		copyprev = &np->wn_sibling;

		// Let "node" point to the head of the copied list.
		якщо (copyp == node)
		    node = np;
	    }
	}

	// Look for the sibling that has the same character.  They are sorted
	// on byte value, thus stop searching when a sibling is found with a
	// higher byte value.  For zero bytes (end of word) the sorting is
	// done on flags and then on affixID.
	поки (node != NULL
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
	якщо (node == NULL
		|| node->wn_byte != word[i]
		|| (word[i] == NUL
		    && (flags < 0
			|| spin->si_sugtree
			|| node->wn_flags != (flags & WN_MASK)
			|| node->wn_affixID != affixID)))
	{
	    // Allocate a new node.
	    np = get_wordnode(spin);
	    якщо (np == NULL)
		повернути FAIL;
	    np->wn_byte = word[i];

	    // If "node" is NULL this is a new child or the end of the sibling
	    // list: ref count is one.  Otherwise use ref count of sibling and
	    // make ref count of sibling one (matters when inserting in front
	    // of the list of siblings).
	    якщо (node == NULL)
		np->wn_refs = 1;
	    інакше
	    {
		np->wn_refs = node->wn_refs;
		node->wn_refs = 1;
	    }
	    якщо (prev != NULL)
		*prev = np;
	    np->wn_sibling = node;
	    node = np;
	}

	якщо (word[i] == NUL)
	{
	    node->wn_flags = flags;
	    node->wn_region |= region;
	    node->wn_affixID = affixID;
	    break;
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

    якщо (spin->si_compress_cnt > 1)
    {
	якщо (--spin->si_compress_cnt == 1)
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
    якщо (spin->si_compress_cnt == 1
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

	якщо (spin->si_verbose)
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
	якщо (affixID >= 0)
	    wordtree_compress(spin, spin->si_keeproot, "keep-case");
    }

    повернути OK;
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

    якщо (spin->si_first_free == NULL)
	n = (wordnode_T *)getroom(spin, sizeof(wordnode_T), TRUE);
    інакше
    {
	n = spin->si_first_free;
	spin->si_first_free = n->wn_child;
	CLEAR_POINTER(n);
	--spin->si_free_count;
    }
#ifdef SPELL_PRINTTREE
    якщо (n != NULL)
	n->wn_nr = ++spin->si_wordnode_nr;
#endif
    повернути n;
}

/*
 * Decrement the reference count on a node (which is the head of a list of
 * siblings).  If the reference count becomes zero free the node and its
 * siblings.
 * Returns the number of nodes actually freed.
 */
    static ціле
deref_wordnode(spellinfo_T *spin, wordnode_T *node)
{
    wordnode_T	*np;
    ціле		cnt = 0;

    якщо (--node->wn_refs == 0)
    {
	FOR_ALL_NODE_SIBLINGS(node, np)
	{
	    якщо (np->wn_child != NULL)
		cnt += deref_wordnode(spin, np->wn_child);
	    free_wordnode(spin, np);
	    ++cnt;
	}
	++cnt;	    // length field
    }
    повернути cnt;
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
wordtree_compress(spellinfo_T *spin, wordnode_T *root, char *name)
{
    hashtab_T	    ht;
    long	    n;
    long	    tot = 0;
    long	    perc;

    // Skip the root itself, it's not actually used.  The first sibling is the
    // start of the tree.
    якщо (root->wn_sibling == NULL)
	повернути;

    hash_init(&ht);
    n = node_compress(spin, root->wn_sibling, &ht, &tot);

#ifndef SPELL_PRINTTREE
    якщо (spin->si_verbose || p_verbose > 2)
#endif
    {
	якщо (tot > 1000000)
	    perc = (tot - n) / (tot / 100);
	інакше якщо (tot == 0)
	    perc = 0;
	інакше
	    perc = (tot - n) * 100 / tot;
	vim_snprintf((char *)IObuff, IOSIZE,
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
    для (np = node; np != NULL && !got_int; np = np->wn_sibling)
    {
	++len;
	якщо ((child = np->wn_child) != NULL)
	{
	    // Compress the child first.  This fills hashkey.
	    compressed += node_compress(spin, child, ht, tot);

	    // Try to find an identical child.
	    hash = hash_hash(child->wn_u1.hashkey);
	    hi = hash_lookup(ht, child->wn_u1.hashkey, hash);
	    якщо (!HASHITEM_EMPTY(hi))
	    {
		// There are children we encountered before with a hash value
		// identical to the current child.  Now check if there is one
		// that is really identical.
		для (tp = HI2WN(hi); tp != NULL; tp = tp->wn_u2.next)
		    якщо (node_equal(child, tp))
		    {
			// Found one!  Now use that child in place of the
			// current one.  This means the current child and all
			// its siblings is unlinked from the tree.
			++tp->wn_refs;
			compressed += deref_wordnode(spin, child);
			np->wn_child = tp;
			break;
		    }
		якщо (tp == NULL)
		{
		    // No other child with this hash value equals the child of
		    // the node, add it to the linked list after the first
		    // item.
		    tp = HI2WN(hi);
		    child->wn_u2.next = tp->wn_u2.next;
		    tp->wn_u2.next = child;
		}
	    }
	    інакше
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
	якщо (np->wn_byte == NUL)
	    // end node: use wn_flags, wn_region and wn_affixID
	    n = np->wn_flags + (np->wn_region << 8) + (np->wn_affixID << 16);
	інакше
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

    повернути compressed;
}

/*
 * Return TRUE when two nodes have identical siblings and children.
 */
    static ціле
node_equal(wordnode_T *n1, wordnode_T *n2)
{
    wordnode_T	*p1;
    wordnode_T	*p2;

    для (p1 = n1, p2 = n2; p1 != NULL && p2 != NULL;
				     p1 = p1->wn_sibling, p2 = p2->wn_sibling)
	якщо (p1->wn_byte != p2->wn_byte
		|| (p1->wn_byte == NUL
		    ? (p1->wn_flags != p2->wn_flags
			|| p1->wn_region != p2->wn_region
			|| p1->wn_affixID != p2->wn_affixID)
		    : (p1->wn_child != p2->wn_child)))
	    break;

    повернути p1 == NULL && p2 == NULL;
}

static ціле rep_compare(const void *s1, const void *s2);

/*
 * Function given to qsort() to sort the REP items on "from" string.
 */
    static ціле
rep_compare(const void *s1, const void *s2)
{
    fromto_T	*p1 = (fromto_T *)s1;
    fromto_T	*p2 = (fromto_T *)s2;

    повернути STRCMP(p1->ft_from, p2->ft_from);
}

/*
 * Write the Vim .spl file "fname".
 * Return FAIL or OK;
 */
    static ціле
write_vim_spell(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    ціле		regionmask;
    ціле		round;
    wordnode_T	*tree;
    ціле		nodecount;
    ціле		i;
    ціле		l;
    garray_T	*gap;
    fromto_T	*ftp;
    char_u	*p;
    ціле		rr;
    ціле		retval = OK;
    size_t	fwv = 1;  // collect return value of fwrite() to avoid
			  // warnings from picky compiler

    fd = mch_fopen((char *)fname, "w");
    якщо (fd == NULL)
    {
	semsg(_(e_cant_open_file_str), fname);
	повернути FAIL;
    }

    // <HEADER>: <fileID> <versionnr>
							    // <fileID>
    fwv &= fwrite(VIMSPELLMAGIC, VIMSPELLMAGICL, (size_t)1, fd);
    якщо (fwv != (size_t)1)
	// Catch first write error, don't try writing more.
	goto theend;

    putc(VIMSPELLVERSION, fd);				    // <versionnr>

    /*
     * <SECTIONS>: <section> ... <sectionend>
     */

    // SN_INFO: <infotext>
    якщо (spin->si_info != NULL)
    {
	putc(SN_INFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	i = (ціле)STRLEN(spin->si_info);
	put_bytes(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_info, (size_t)i, (size_t)1, fd); // <infotext>
    }

    // SN_REGION: <regionname> ...
    // Write the region names only if there is more than one.
    якщо (spin->si_region_count > 1)
    {
	putc(SN_REGION, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>
	l = spin->si_region_count * 2;
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_region_name, (size_t)l, (size_t)1, fd);
							// <regionname> ...
	regionmask = (1 << spin->si_region_count) - 1;
    }
    інакше
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
    якщо (!spin->si_ascii && !spin->si_add)
    {
	char_u	folchars[128 * 8];
	ціле	flags;

	putc(SN_CHARFLAGS, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	// Form the <folchars> string first, we need to know its length.
	l = 0;
	для (i = 128; i < 256; ++i)
	{
	    якщо (has_mbyte)
		l += mb_char2bytes(spelltab.st_fold[i], folchars + l);
	    інакше
		folchars[l++] = spelltab.st_fold[i];
	}
	put_bytes(fd, (long_u)(1 + 128 + 2 + l), 4);	// <sectionlen>

	fputc(128, fd);					// <charflagslen>
	для (i = 128; i < 256; ++i)
	{
	    flags = 0;
	    якщо (spelltab.st_isw[i])
		flags |= CF_WORD;
	    якщо (spelltab.st_isu[i])
		flags |= CF_UPPER;
	    fputc(flags, fd);				// <charflags>
	}

	put_bytes(fd, (long_u)l, 2);			// <folcharslen>
	fwv &= fwrite(folchars, (size_t)l, (size_t)1, fd); // <folchars>
    }

    // SN_MIDWORD: <midword>
    якщо (spin->si_midword != NULL)
    {
	putc(SN_MIDWORD, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	i = (ціле)STRLEN(spin->si_midword);
	put_bytes(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_midword, (size_t)i, (size_t)1, fd);
							// <midword>
    }

    // SN_PREFCOND: <prefcondcnt> <prefcond> ...
    якщо (spin->si_prefcond.ga_len > 0)
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
    для (round = 1; round <= 3; ++round)
    {
	якщо (round == 1)
	    gap = &spin->si_rep;
	інакше якщо (round == 2)
	{
	    // Don't write SN_SAL when using a SN_SOFO section
	    якщо (spin->si_sofofr != NULL && spin->si_sofoto != NULL)
		continue;
	    gap = &spin->si_sal;
	}
	інакше
	    gap = &spin->si_repsal;

	// Don't write the section if there are no items.
	якщо (gap->ga_len == 0)
	    continue;

	// Sort the REP/REPSAL items.
	якщо (round != 2)
	    qsort(gap->ga_data, (size_t)gap->ga_len,
					       sizeof(fromto_T), rep_compare);

	i = round == 1 ? SN_REP : (round == 2 ? SN_SAL : SN_REPSAL);
	putc(i, fd);					// <sectionID>

	// This is for making suggestions, section is not required.
	putc(0, fd);					// <sectionflags>

	// Compute the length of what follows.
	l = 2;	    // count <repcount> or <salcount>
	для (i = 0; i < gap->ga_len; ++i)
	{
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    l += 1 + (ціле)STRLEN(ftp->ft_from);  // count <*fromlen> and <*from>
	    l += 1 + (ціле)STRLEN(ftp->ft_to);    // count <*tolen> and <*to>
	}
	якщо (round == 2)
	    ++l;	// count <salflags>
	put_bytes(fd, (long_u)l, 4);			// <sectionlen>

	якщо (round == 2)
	{
	    i = 0;
	    якщо (spin->si_followup)
		i |= SAL_F0LLOWUP;
	    якщо (spin->si_collapse)
		i |= SAL_COLLAPSE;
	    якщо (spin->si_rem_accents)
		i |= SAL_REM_ACCENTS;
	    putc(i, fd);			// <salflags>
	}

	put_bytes(fd, (long_u)gap->ga_len, 2);	// <repcount> or <salcount>
	для (i = 0; i < gap->ga_len; ++i)
	{
	    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
	    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    для (rr = 1; rr <= 2; ++rr)
	    {
		p = rr == 1 ? ftp->ft_from : ftp->ft_to;
		l = (ціле)STRLEN(p);
		putc(l, fd);
		якщо (l > 0)
		    fwv &= fwrite(p, l, (size_t)1, fd);
	    }
	}

    }

    // SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
    // This is for making suggestions, section is not required.
    якщо (spin->si_sofofr != NULL && spin->si_sofoto != NULL)
    {
	putc(SN_SOFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (ціле)STRLEN(spin->si_sofofr);
	put_bytes(fd, (long_u)(l + STRLEN(spin->si_sofoto) + 4), 4);
							// <sectionlen>

	put_bytes(fd, (long_u)l, 2);			// <sofofromlen>
	fwv &= fwrite(spin->si_sofofr, l, (size_t)1, fd); // <sofofrom>

	l = (ціле)STRLEN(spin->si_sofoto);
	put_bytes(fd, (long_u)l, 2);			// <sofotolen>
	fwv &= fwrite(spin->si_sofoto, l, (size_t)1, fd); // <sofoto>
    }

    // SN_WORDS: <word> ...
    // This is for making suggestions, section is not required.
    якщо (spin->si_commonwords.ht_used > 0)
    {
	putc(SN_WORDS, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// round 1: count the bytes
	// round 2: write the bytes
	для (round = 1; round <= 2; ++round)
	{
	    ціле		todo;
	    ціле		len = 0;
	    hashitem_T	*hi;

	    todo = (ціле)spin->si_commonwords.ht_used;
	    FOR_ALL_HASHTAB_ITEMS(&spin->si_commonwords, hi, todo)
		якщо (!HASHITEM_EMPTY(hi))
		{
		    l = (ціле)STRLEN(hi->hi_key) + 1;
		    len += l;
		    якщо (round == 2)			// <word>
			fwv &= fwrite(hi->hi_key, (size_t)l, (size_t)1, fd);
		    --todo;
		}
	    якщо (round == 1)
		put_bytes(fd, (long_u)len, 4);		// <sectionlen>
	}
    }

    // SN_MAP: <mapstr>
    // This is for making suggestions, section is not required.
    якщо (spin->si_map.ga_len > 0)
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
    якщо (!spin->si_nosugfile
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
    якщо (spin->si_nosplitsugs)
    {
	putc(SN_NOSPLITSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_NOCOMPUNDSUGS: nothing
    // This is used to notify that no suggestions with compounds are to be
    // made.
    якщо (spin->si_nocompoundsugs)
    {
	putc(SN_NOCOMPOUNDSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_COMPOUND: compound info.
    // We don't mark it required, when not supported all compound words will
    // be bad words.
    якщо (spin->si_compflags != NULL)
    {
	putc(SN_COMPOUND, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (ціле)STRLEN(spin->si_compflags);
	для (i = 0; i < spin->si_comppat.ga_len; ++i)
	    l += (ціле)STRLEN(((char_u **)(spin->si_comppat.ga_data))[i]) + 1;
	put_bytes(fd, (long_u)(l + 7), 4);		// <sectionlen>

	putc(spin->si_compmax, fd);			// <compmax>
	putc(spin->si_compminlen, fd);			// <compminlen>
	putc(spin->si_compsylmax, fd);			// <compsylmax>
	putc(0, fd);		// for Vim 7.0b compatibility
	putc(spin->si_compoptions, fd);			// <compoptions>
	put_bytes(fd, (long_u)spin->si_comppat.ga_len, 2);
							// <comppatcount>
	для (i = 0; i < spin->si_comppat.ga_len; ++i)
	{
	    p = ((char_u **)(spin->si_comppat.ga_data))[i];
	    putc((ціле)STRLEN(p), fd);			// <comppatlen>
	    fwv &= fwrite(p, (size_t)STRLEN(p), (size_t)1, fd);
							// <comppattext>
	}
							// <compflags>
	fwv &= fwrite(spin->si_compflags, (size_t)STRLEN(spin->si_compflags),
							       (size_t)1, fd);
    }

    // SN_NOBREAK: NOBREAK flag
    якщо (spin->si_nobreak)
    {
	putc(SN_NOBREAK, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// It's empty, the presence of the section flags the feature.
	put_bytes(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_SYLLABLE: syllable info.
    // We don't mark it required, when not supported syllables will not be
    // counted.
    якщо (spin->si_syllable != NULL)
    {
	putc(SN_SYLLABLE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (ціле)STRLEN(spin->si_syllable);
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
    для (round = 1; round <= 3; ++round)
    {
	якщо (round == 1)
	    tree = spin->si_foldroot->wn_sibling;
	інакше якщо (round == 2)
	    tree = spin->si_keeproot->wn_sibling;
	інакше
	    tree = spin->si_prefroot->wn_sibling;

	// Clear the index and wnode fields in the tree.
	clear_node(tree);

	// Count the number of nodes.  Needed to be able to allocate the
	// memory when reading the nodes.  Also fills in index for shared
	// nodes.
	nodecount = put_node(NULL, tree, 0, regionmask, round == 3);

	// number of nodes in 4 bytes
	put_bytes(fd, (long_u)nodecount, 4);	// <nodecount>
	spin->si_memtot += nodecount + nodecount * sizeof(ціле);

	// Write the nodes.
	(void)put_node(fd, tree, 0, regionmask, round == 3);
    }

    // Write another byte to check for errors (file system full).
    якщо (putc(0, fd) == EOF)
	retval = FAIL;
theend:
    якщо (fclose(fd) == EOF)
	retval = FAIL;

    якщо (fwv != (size_t)1)
	retval = FAIL;
    якщо (retval == FAIL)
	emsg(_(e_error_while_writing));

    повернути retval;
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

    якщо (node != NULL)
	FOR_ALL_NODE_SIBLINGS(node, np)
	{
	    np->wn_u1.index = 0;
	    np->wn_u2.wnode = NULL;

	    якщо (np->wn_byte != NUL)
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
    static ціле
put_node(
    FILE	*fd,		// NULL when only counting
    wordnode_T	*node,
    ціле		idx,
    ціле		regionmask,
    ціле		prefixtree)	// TRUE for PREFIXTREE
{
    ціле		newindex = idx;
    ціле		siblingcount = 0;
    wordnode_T	*np;
    ціле		flags;

    // If "node" is zero the tree is empty.
    якщо (node == NULL)
	повернути 0;

    // Store the index where this node is written.
    node->wn_u1.index = idx;

    // Count the number of siblings.
    FOR_ALL_NODE_SIBLINGS(node, np)
	++siblingcount;

    // Write the sibling count.
    якщо (fd != NULL)
	putc(siblingcount, fd);				// <siblingcount>

    // Write each sibling byte and optionally extra info.
    FOR_ALL_NODE_SIBLINGS(node, np)
    {
	якщо (np->wn_byte == 0)
	{
	    якщо (fd != NULL)
	    {
		// For a NUL byte (end of word) write the flags etc.
		якщо (prefixtree)
		{
		    // In PREFIXTREE write the required affixID and the
		    // associated condition nr (stored in wn_region).  The
		    // byte value is misused to store the "rare" and "not
		    // combining" flags
		    якщо (np->wn_flags == (short_u)PFX_FLAGS)
			putc(BY_NOFLAGS, fd);		// <byte>
		    інакше
		    {
			putc(BY_FLAGS, fd);		// <byte>
			putc(np->wn_flags, fd);		// <pflags>
		    }
		    putc(np->wn_affixID, fd);		// <affixID>
		    put_bytes(fd, (long_u)np->wn_region, 2); // <prefcondnr>
		}
		інакше
		{
		    // For word trees we write the flag/region items.
		    flags = np->wn_flags;
		    якщо (regionmask != 0 && np->wn_region != regionmask)
			flags |= WF_REGION;
		    якщо (np->wn_affixID != 0)
			flags |= WF_AFX;
		    якщо (flags == 0)
		    {
			// word without flags or region
			putc(BY_NOFLAGS, fd);			// <byte>
		    }
		    інакше
		    {
			якщо (np->wn_flags >= 0x100)
			{
			    putc(BY_FLAGS2, fd);		// <byte>
			    putc(flags, fd);			// <flags>
			    putc((unsigned)flags >> 8, fd);	// <flags2>
			}
			інакше
			{
			    putc(BY_FLAGS, fd);			// <byte>
			    putc(flags, fd);			// <flags>
			}
			якщо (flags & WF_REGION)
			    putc(np->wn_region, fd);		// <region>
			якщо (flags & WF_AFX)
			    putc(np->wn_affixID, fd);		// <affixID>
		    }
		}
	    }
	}
	інакше
	{
	    якщо (np->wn_child->wn_u1.index != 0
					 && np->wn_child->wn_u2.wnode != node)
	    {
		// The child is written elsewhere, write the reference.
		якщо (fd != NULL)
		{
		    putc(BY_INDEX, fd);			// <byte>
							// <nodeidx>
		    put_bytes(fd, (long_u)np->wn_child->wn_u1.index, 3);
		}
	    }
	    інакше якщо (np->wn_child->wn_u2.wnode == NULL)
		// We will write the child below and give it an index.
		np->wn_child->wn_u2.wnode = node;

	    якщо (fd != NULL)
		якщо (putc(np->wn_byte, fd) == EOF) // <byte> or <xbyte>
		{
		    emsg(_(e_error_while_writing));
		    повернути 0;
		}
	}
    }

    // Space used in the array when reading: one for each sibling and one for
    // the count.
    newindex += siblingcount + 1;

    // Recursively dump the children of each sibling.
    FOR_ALL_NODE_SIBLINGS(node, np)
	якщо (np->wn_byte != 0 && np->wn_child->wn_u2.wnode == node)
	    newindex = put_node(fd, np->wn_child, newindex, regionmask,
								  prefixtree);

    повернути newindex;
}


/*
 * ":mkspell [-ascii] outfile  infile ..."
 * ":mkspell [-ascii] addfile"
 */
    void
ex_mkspell(exarg_T *eap)
{
    ціле		fcount;
    char_u	**fnames;
    char_u	*arg = eap->arg;
    ціле		ascii = FALSE;

    якщо (STRNCMP(arg, "-ascii", 6) == 0)
    {
	ascii = TRUE;
	arg = skipwhite(arg + 6);
    }

    // Expand all the remaining arguments (e.g., $VIMRUNTIME).
    якщо (get_arglist_exp(arg, &fcount, &fnames, FALSE) != OK)
	повернути;

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
    ціле		len;
    slang_T	*slang;
    ціле		free_slang = FALSE;

    /*
     * Read back the .spl file that was written.  This fills the required
     * info for soundfolding.  This also uses less memory than the
     * pointer-linked version of the trie.  And it avoids having two versions
     * of the code for the soundfolding stuff.
     * It might have been done already by spell_reload_one().
     */
    FOR_ALL_SPELL_LANGS(slang)
	якщо (fullpathcmp(wfname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	    break;
    якщо (slang == NULL)
    {
	spell_message(spin, (char_u *)_("Reading back spell file..."));
	slang = spell_load_file(wfname, NULL, NULL, FALSE);
	якщо (slang == NULL)
	    повернути;
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
    якщо (sug_filltree(spin, slang) == FAIL)
	goto theend;

    /*
     * Create the table which links each soundfold word with a list of the
     * good words it may come from.  Creates buffer "spin->si_spellbuf".
     * This also removes the wordnr from the NUL byte entries to make
     * compression possible.
     */
    якщо (sug_maketable(spin) == FAIL)
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
    якщо (fname == NULL)
	goto theend;
    vim_strncpy(fname, wfname, MAXPATHL - 1);
    len = (ціле)STRLEN(fname);
    fname[len - 2] = 'u';
    fname[len - 1] = 'g';
    sug_write(spin, fname);

theend:
    vim_free(fname);
    якщо (free_slang)
	slang_free(slang);
    free_blocks(spin->si_blocks);
    close_spellbuf(spin->si_spellbuf);
}

/*
 * Build the soundfold trie for language "slang".
 */
    static ціле
sug_filltree(spellinfo_T *spin, slang_T *slang)
{
    char_u	*byts;
    idx_T	*idxs;
    ціле		depth;
    idx_T	arridx[MAXWLEN];
    ціле		curi[MAXWLEN];
    char_u	tword[MAXWLEN];
    char_u	tsalword[MAXWLEN];
    ціле		c;
    idx_T	n;
    unsigned	words_done = 0;
    ціле		wordcount[MAXWLEN];

    // We use si_foldroot for the soundfolded trie.
    spin->si_foldroot = wordtree_alloc(spin);
    якщо (spin->si_foldroot == NULL)
	повернути FAIL;

    // let tree_add_word() know we're adding to the soundfolded tree
    spin->si_sugtree = TRUE;

    /*
     * Go through the whole case-folded tree, soundfold each word and put it
     * in the trie.  Bail out if the tree is empty.
     */
    byts = slang->sl_fbyts;
    idxs = slang->sl_fidxs;
    якщо (byts == NULL || idxs == NULL)
	повернути FAIL;

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;

    depth = 0;
    поки (depth >= 0 && !got_int)
    {
	якщо (curi[depth] > byts[arridx[depth]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[depth]] = wordcount[depth];
	    якщо (depth > 0)
		wordcount[depth - 1] += wordcount[depth];

	    --depth;
	    line_breakcheck();
	}
	інакше
	{

	    // Do one more byte at this node.
	    n = arridx[depth] + curi[depth];
	    ++curi[depth];

	    c = byts[n];
	    якщо (c == 0)
	    {
		// Sound-fold the word.
		tword[depth] = NUL;
		spell_soundfold(slang, tword, TRUE, tsalword);

		// We use the "flags" field for the MSB of the wordnr,
		// "region" for the LSB of the wordnr.
		якщо (tree_add_word(spin, tsalword, spin->si_foldroot,
				words_done >> 16, words_done & 0xffff,
							   0) == FAIL)
		    повернути FAIL;

		++words_done;
		++wordcount[depth];

		// Reset the block count each time to avoid compression
		// kicking in.
		spin->si_blocks_cnt = 0;

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		поки (n + 1 < slang->sl_fbyts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[depth];
		}
	    }
	    інакше якщо (depth < MAXWLEN - 1)
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

    повернути OK;
}

/*
 * Make the table that links each word in the soundfold trie to the words it
 * can be produced from.
 * This is not unlike lines in a file, thus use a memfile to be able to access
 * the table efficiently.
 * Returns FAIL when out of memory.
 */
    static ціле
sug_maketable(spellinfo_T *spin)
{
    garray_T	ga;
    ціле		res = OK;

    // Allocate a buffer, open a memline for it and create the swap file
    // (uses a temp file, not a .swp file).
    spin->si_spellbuf = open_spellbuf();
    якщо (spin->si_spellbuf == NULL)
	повернути FAIL;

    // Use a buffer to store the line info, avoids allocating many small
    // pieces of memory.
    ga_init2(&ga, 1, 100);

    // recursively go through the tree
    якщо (sug_filltable(spin, spin->si_foldroot->wn_sibling, 0, &ga) == -1)
	res = FAIL;

    ga_clear(&ga);
    повернути res;
}

/*
 * Fill the table for one node and its children.
 * Returns the wordnr at the start of the node.
 * Returns -1 when out of memory.
 */
    static ціле
sug_filltable(
    spellinfo_T	*spin,
    wordnode_T	*node,
    ціле		startwordnr,
    garray_T	*gap)	    // place to store line of numbers
{
    wordnode_T	*p, *np;
    ціле		wordnr = startwordnr;
    ціле		nr;
    ціле		prev_nr;

    FOR_ALL_NODE_SIBLINGS(node, p)
    {
	якщо (p->wn_byte == NUL)
	{
	    gap->ga_len = 0;
	    prev_nr = 0;
	    для (np = p; np != NULL && np->wn_byte == NUL; np = np->wn_sibling)
	    {
		якщо (ga_grow(gap, 10) == FAIL)
		    повернути -1;

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

	    якщо (ml_append_buf(spin->si_spellbuf, (linenr_T)wordnr,
				     gap->ga_data, gap->ga_len, TRUE) == FAIL)
		повернути -1;
	    ++wordnr;

	    // Remove extra NUL entries, we no longer need them. We don't
	    // bother freeing the nodes, they won't be reused anyway.
	    поки (p->wn_sibling != NULL && p->wn_sibling->wn_byte == NUL)
		p->wn_sibling = p->wn_sibling->wn_sibling;

	    // Clear the flags on the remaining NUL node, so that compression
	    // works a lot better.
	    p->wn_flags = 0;
	    p->wn_region = 0;
	}
	інакше
	{
	    wordnr = sug_filltable(spin, p->wn_child, wordnr, gap);
	    якщо (wordnr == -1)
		повернути -1;
	}
    }
    повернути wordnr;
}

/*
 * Convert an offset into a minimal number of bytes.
 * Similar to utf_char2byters, but use 8 bits in followup bytes and avoid NUL
 * bytes.
 */
    static ціле
offset2bytes(ціле nr, char_u *buf)
{
    ціле	    rem;
    ціле	    b1, b2, b3, b4;

    // Split the number in parts of base 255.  We need to avoid NUL bytes.
    b1 = nr % 255 + 1;
    rem = nr / 255;
    b2 = rem % 255 + 1;
    rem = rem / 255;
    b3 = rem % 255 + 1;
    b4 = rem / 255 + 1;

    якщо (b4 > 1 || b3 > 0x1f)	// 4 bytes
    {
	buf[0] = 0xe0 + b4;
	buf[1] = b3;
	buf[2] = b2;
	buf[3] = b1;
	повернути 4;
    }
    якщо (b3 > 1 || b2 > 0x3f )	// 3 bytes
    {
	buf[0] = 0xc0 + b3;
	buf[1] = b2;
	buf[2] = b1;
	повернути 3;
    }
    якщо (b2 > 1 || b1 > 0x7f )	// 2 bytes
    {
	buf[0] = 0x80 + b2;
	buf[1] = b1;
	повернути 2;
    }
				// 1 byte
    buf[0] = b1;
    повернути 1;
}

/*
 * Write the .sug file in "fname".
 */
    static void
sug_write(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    wordnode_T	*tree;
    ціле		nodecount;
    ціле		wcount;
    char_u	*line;
    linenr_T	lnum;
    ціле		len;

    // Create the file.  Note that an existing file is silently overwritten!
    fd = mch_fopen((char *)fname, "w");
    якщо (fd == NULL)
    {
	semsg(_(e_cant_open_file_str), fname);
	повернути;
    }

    vim_snprintf((char *)IObuff, IOSIZE,
				  _("Writing suggestion file %s..."), fname);
    spell_message(spin, IObuff);

    /*
     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
     */
    якщо (fwrite(VIMSUGMAGIC, VIMSUGMAGICL, (size_t)1, fd) != 1) // <fileID>
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
    spin->si_memtot += nodecount + nodecount * sizeof(ціле);

    // Write the nodes.
    (void)put_node(fd, tree, 0, 0, FALSE);

    /*
     * <SUGTABLE>: <sugwcount> <sugline> ...
     */
    wcount = spin->si_spellbuf->b_ml.ml_line_count;
    put_bytes(fd, (long_u)wcount, 4);	// <sugwcount>

    для (lnum = 1; lnum <= (linenr_T)wcount; ++lnum)
    {
	// <sugline>: <sugnr> ... NUL
	line = ml_get_buf(spin->si_spellbuf, lnum, FALSE);
	len = ml_get_buf_len(spin->si_spellbuf, lnum) + 1;
	якщо (fwrite(line, (size_t)len, (size_t)1, fd) == 0)
	{
	    emsg(_(e_error_while_writing));
	    goto theend;
	}
	spin->si_memtot += len;
    }

    // Write another byte to check for errors.
    якщо (putc(0, fd) == EOF)
	emsg(_(e_error_while_writing));

    vim_snprintf((char *)IObuff, IOSIZE,
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
    ціле		fcount,
    char_u	**fnames,
    ціле		ascii,		    // -ascii argument given
    ціле		over_write,	    // overwrite existing output file
    ціле		added_word)	    // invoked through "zg"
{
    char_u	*fname = NULL;
    char_u	*wfname;
    char_u	**innames;
    ціле		incount;
    afffile_T	*(afile[MAXREGIONS]);
    ціле		i;
    ціле		len;
    stat_T	st;
    ціле		error = FALSE;
    spellinfo_T spin;

    CLEAR_FIELD(spin);
    spin.si_verbose = !added_word;
    spin.si_ascii = ascii;
    spin.si_followup = TRUE;
    spin.si_rem_accents = TRUE;
    ga_init2(&spin.si_rep, sizeof(fromto_T), 20);
    ga_init2(&spin.si_repsal, sizeof(fromto_T), 20);
    ga_init2(&spin.si_sal, sizeof(fromto_T), 20);
    ga_init2(&spin.si_map, sizeof(char_u), 100);
    ga_init2(&spin.si_comppat, sizeof(char_u *), 20);
    ga_init2(&spin.si_prefcond, sizeof(char_u *), 50);
    hash_init(&spin.si_commonwords);
    spin.si_newcompID = 127;	// start compound ID at first maximum

    // default: fnames[0] is output file, following are input files
    // When "fcount" is 1 there is only one file.
    innames = &fnames[fcount == 1 ? 0 : 1];
    incount = fcount - 1;

    wfname = alloc(MAXPATHL);
    якщо (wfname == NULL)
	повернути;

    якщо (fcount >= 1)
    {
	len = (ціле)STRLEN(fnames[0]);
	якщо (fcount == 1 && len > 4 && STRCMP(fnames[0] + len - 4, ".add") == 0)
	{
	    // For ":mkspell path/en.latin1.add" output file is
	    // "path/en.latin1.add.spl".
	    incount = 1;
	    vim_snprintf((char *)wfname, MAXPATHL, "%s.spl", fnames[0]);
	}
	інакше якщо (fcount == 1)
	{
	    // For ":mkspell path/vim" output file is "path/vim.latin1.spl".
	    incount = 1;
	    vim_snprintf((char *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());
	}
	інакше якщо (len > 4 && STRCMP(fnames[0] + len - 4, ".spl") == 0)
	{
	    // Name ends in ".spl", use as the file name.
	    vim_strncpy(wfname, fnames[0], MAXPATHL - 1);
	}
	інакше
	    // Name should be language, make the file name from it.
	    vim_snprintf((char *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());

	// Check for .ascii.spl.
	якщо (strstr((char *)gettail(wfname), SPL_FNAME_ASCII) != NULL)
	    spin.si_ascii = TRUE;

	// Check for .add.spl.
	якщо (strstr((char *)gettail(wfname), SPL_FNAME_ADD) != NULL)
	    spin.si_add = TRUE;
    }

    якщо (incount <= 0)
	emsg(_(e_invalid_argument));	// need at least output and input names
    інакше якщо (vim_strchr(gettail(wfname), '_') != NULL)
	emsg(_(e_output_file_name_must_not_have_region_name));
    інакше якщо (incount > MAXREGIONS)
	semsg(_(e_only_up_to_nr_regions_supported), MAXREGIONS);
    інакше
    {
	// Check for overwriting before doing things that may take a lot of
	// time.
	якщо (!over_write && mch_stat((char *)wfname, &st) >= 0)
	{
	    emsg(_(e_file_exists));
	    goto theend;
	}
	якщо (mch_isdir(wfname))
	{
	    semsg(_(e_str_is_directory), wfname);
	    goto theend;
	}

	fname = alloc(MAXPATHL);
	якщо (fname == NULL)
	    goto theend;

	/*
	 * Init the aff and dic pointers.
	 * Get the region names if there are more than 2 arguments.
	 */
	для (i = 0; i < incount; ++i)
	{
	    afile[i] = NULL;

	    якщо (incount > 1)
	    {
		len = (ціле)STRLEN(innames[i]);
		якщо (STRLEN(gettail(innames[i])) < 5
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
	якщо (spin.si_foldroot == NULL
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
	якщо (!spin.si_add)
	    spin.si_clear_chartab = TRUE;

	/*
	 * Read all the .aff and .dic files.
	 * Text is converted to 'encoding'.
	 * Words are stored in the case-folded and keep-case trees.
	 */
	для (i = 0; i < incount && !error; ++i)
	{
	    spin.si_conv.vc_type = CONV_NONE;
	    spin.si_region = 1 << i;

	    vim_snprintf((char *)fname, MAXPATHL, "%s.aff", innames[i]);
	    якщо (mch_stat((char *)fname, &st) >= 0)
	    {
		// Read the .aff file.  Will init "spin->si_conv" based on the
		// "SET" line.
		afile[i] = spell_read_aff(&spin, fname);
		якщо (afile[i] == NULL)
		    error = TRUE;
		інакше
		{
		    // Read the .dic file and store the words in the trees.
		    vim_snprintf((char *)fname, MAXPATHL, "%s.dic",
								  innames[i]);
		    якщо (spell_read_dic(&spin, fname, afile[i]) == FAIL)
			error = TRUE;
		}
	    }
	    інакше
	    {
		// No .aff file, try reading the file as a word list.  Store
		// the words in the trees.
		якщо (spell_read_wordfile(&spin, innames[i]) == FAIL)
		    error = TRUE;
	    }

	    // Free any conversion stuff.
	    convert_setup(&spin.si_conv, NULL, NULL);
	}

	якщо (spin.si_compflags != NULL && spin.si_nobreak)
	    msg(_("Warning: both compounding and NOBREAK specified"));

	якщо (!error && !got_int)
	{
	    /*
	     * Combine tails in the tree.
	     */
	    spell_message(&spin, (char_u *)_(msg_compressing));
	    wordtree_compress(&spin, spin.si_foldroot, "case-folded");
	    wordtree_compress(&spin, spin.si_keeproot, "keep-case");
	    wordtree_compress(&spin, spin.si_prefroot, "prefixes");
	}

	якщо (!error && !got_int)
	{
	    /*
	     * Write the info in the spell file.
	     */
	    vim_snprintf((char *)IObuff, IOSIZE,
				      _("Writing spell file %s..."), wfname);
	    spell_message(&spin, IObuff);

	    error = write_vim_spell(&spin, wfname) == FAIL;

	    spell_message(&spin, (char_u *)_("Done!"));
	    vim_snprintf((char *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin.si_memtot);
	    spell_message(&spin, IObuff);

	    /*
	     * If the file is loaded need to reload it.
	     */
	    якщо (!error)
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
	для (i = 0; i < incount; ++i)
	    якщо (afile[i] != NULL)
		spell_free_aff(afile[i]);

	// Free all the bits and pieces at once.
	free_blocks(spin.si_blocks);

	/*
	 * If there is soundfolding info and no NOSUGFILE item create the
	 * .sug file with the soundfolded word trie.
	 */
	якщо (spin.si_sugtime != 0 && !error && !got_int)
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
    якщо (spin->si_verbose || p_verbose > 2)
    {
	якщо (!spin->si_verbose)
	    verbose_enter();
	msg((char *)str);
	out_flush();
	якщо (!spin->si_verbose)
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
    spell_add_word(eap->arg, (ціле)STRLEN(eap->arg),
		eap->cmdidx == CMD_spellwrong ? SPELL_ADD_BAD :
		eap->cmdidx == CMD_spellrare ? SPELL_ADD_RARE : SPELL_ADD_GOOD,
				   eap->forceit ? 0 : (ціле)eap->line2,
				   eap->cmdidx == CMD_spellundo);
}

/*
 * Add "word[len]" to 'spellfile' as a good, rare or bad word.
 */
    void
spell_add_word(
    char_u	*word,
    ціле		len,
    ціле		what,	    // SPELL_ADD_ values
    ціле		idx,	    // "zG" and "zW": zero, otherwise index in
			    // 'spellfile'
    ціле		undo)	    // TRUE for "zug", "zuG", "zuw" and "zuW"
{
    FILE	*fd = NULL;
    buf_T	*buf = NULL;
    ціле		new_spf = FALSE;
    char_u	*fname;
    char_u	*fnamebuf = NULL;
    char_u	line[MAXWLEN * 2];
    long	fpos, fpos_next = 0;
    ціле		i;
    size_t	linelen;
    char_u	*spf;

    якщо (!valid_spell_word(word, word + len))
    {
	emsg(_(e_illegal_character_in_word));
	повернути;
    }

    якщо (idx == 0)	    // use internal wordlist
    {
	якщо (int_wordlist == NULL)
	{
	    int_wordlist = vim_tempname('s', FALSE);
	    якщо (int_wordlist == NULL)
		повернути;
	}
	fname = int_wordlist;
    }
    інакше
    {
	// If 'spellfile' isn't set figure out a good default value.
	якщо (*curwin->w_s->b_p_spf == NUL)
	{
	    init_spellfile();
	    new_spf = TRUE;
	}

	якщо (*curwin->w_s->b_p_spf == NUL)
	{
	    semsg(_(e_option_str_is_not_set), "spellfile");
	    повернути;
	}
	fnamebuf = alloc(MAXPATHL);
	якщо (fnamebuf == NULL)
	    повернути;

	для (spf = curwin->w_s->b_p_spf, i = 1; *spf != NUL; ++i)
	{
	    copy_option_part(&spf, fnamebuf, MAXPATHL, ",");
	    якщо (i == idx)
		break;
	    якщо (*spf == NUL)
	    {
		semsg(_(e_spellfile_does_not_have_nr_entries), idx);
		vim_free(fnamebuf);
		повернути;
	    }
	}

	// Check that the user isn't editing the .add file somewhere.
	buf = buflist_findname_exp(fnamebuf);
	якщо (buf != NULL && buf->b_ml.ml_mfp == NULL)
	    buf = NULL;
	якщо (buf != NULL && bufIsChanged(buf))
	{
	    emsg(_(e_file_is_loaded_in_another_buffer));
	    vim_free(fnamebuf);
	    повернути;
	}

	fname = fnamebuf;
    }

    якщо (what == SPELL_ADD_BAD || undo)
    {
	// When the word appears as good word we need to remove that one,
	// since its flags sort before the one with WF_BANNED.
	fd = mch_fopen((char *)fname, "r");
	якщо (fd != NULL)
	{
	    поки (!vim_fgets(line, MAXWLEN * 2, fd))
	    {
		fpos = fpos_next;
		fpos_next = ftell(fd);
		якщо (fpos_next < 0)
		    break;  // should never happen
		linelen = STRLEN(line);
		якщо (linelen >= (size_t)len
			&& STRNCMP(word, line, len) == 0
			&& (line[len] == '/' || line[len] < ' '))
		{
		    // Found duplicate word.  Remove it by writing a '#' at
		    // the start of the line.  Mixing reading and writing
		    // doesn't work for all systems, close the file first.
		    fclose(fd);
		    fd = mch_fopen((char *)fname, "r+");
		    якщо (fd == NULL)
			break;
		    якщо (fseek(fd, fpos, SEEK_SET) == 0)
		    {
			fputc('#', fd);
			якщо (undo)
			{
			    home_replace(NULL, fname, NameBuff, MAXPATHL, TRUE);
			    smsg(_("Word '%.*s' removed from %s"),
							 len, word, NameBuff);
			}
		    }
		    якщо (fseek(fd, fpos_next, SEEK_SET) != 0)
		    {
			PERROR(_("Seek error in spellfile"));
			break;
		    }
		}
	    }
	    якщо (fd != NULL)
		fclose(fd);
	}
    }

    якщо (!undo)
    {
	fd = mch_fopen((char *)fname, "a");
	якщо (fd == NULL && new_spf)
	{
	    char_u *p;

	    // We just initialized the 'spellfile' option and can't open the
	    // file.  We may need to create the "spell" directory first.  We
	    // already checked the runtime directory is writable in
	    // init_spellfile().
	    якщо (!dir_of_file_exists(fname) && (p = gettail_sep(fname)) != fname)
	    {
		ціле c = *p;

		// The directory doesn't exist.  Try creating it and opening
		// the file again.
		*p = NUL;
		vim_mkdir(fname, 0755);
		*p = c;
		fd = mch_fopen((char *)fname, "a");
	    }
	}

	якщо (fd == NULL)
	    semsg(_(e_cant_open_file_str), fname);
	інакше
	{
	    якщо (what == SPELL_ADD_BAD)
		fprintf(fd, "%.*s/!\n", len, word);
	    інакше якщо (what == SPELL_ADD_RARE)
		fprintf(fd, "%.*s/?\n", len, word);
	    інакше
		fprintf(fd, "%.*s\n", len, word);
	    fclose(fd);

	    home_replace(NULL, fname, NameBuff, MAXPATHL, TRUE);
	    smsg(_("Word '%.*s' added to %s"), len, word, NameBuff);
	}
    }

    якщо (fd != NULL)
    {
	// Update the .add.spl file.
	mkspell(1, &fname, FALSE, TRUE, TRUE);

	// If the .add file is edited somewhere, reload it.
	якщо (buf != NULL)
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
    ціле		aspath = FALSE;
    char_u	*lstart = curbuf->b_s.b_p_spl;

    якщо (*curwin->w_s->b_p_spl == NUL || curwin->w_s->b_langp.ga_len <= 0)
	повернути;

    buf.string = alloc(MAXPATHL);
    якщо (buf.string == NULL)
	повернути;

    buf.length = 0;

    // Find the end of the language name.  Exclude the region.  If there
    // is a path separator remember the start of the tail.
    для (lend = curwin->w_s->b_p_spl; *lend != NUL
	    && vim_strchr((char_u *)",._", *lend) == NULL; ++lend)
	якщо (vim_ispathsep(*lend))
	{
	    aspath = TRUE;
	    lstart = lend + 1;
	}

    // Loop over all entries in 'runtimepath'.  Use the first one where we
    // are allowed to write.
    rtp = p_rtp;
    поки (*rtp != NUL)
    {
	якщо (aspath)
	    // Use directory of an entry with path, e.g., for
	    // "/dir/lg.utf-8.spl" use "/dir".
	    vim_strncpy(buf.string, curbuf->b_s.b_p_spl,
		    lstart - curbuf->b_s.b_p_spl - 1);
	інакше
	    // Copy the path from 'runtimepath' to buf[].
	    buf.length = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");
	якщо (filewritable(buf.string) == 2)
	{
	    // Use the first language name from 'spelllang' and the
	    // encoding used in the first loaded .spl file.
	    якщо (aspath)
	    {
		buf.length = (size_t)(lend - curbuf->b_s.b_p_spl);
		vim_strncpy(buf.string, curbuf->b_s.b_p_spl, buf.length);
	    }
	    інакше
	    {
		// Create the "spell" directory if it doesn't exist yet.
		buf.length += vim_snprintf_safelen((char *)buf.string + buf.length,
		    MAXPATHL - buf.length, "/spell");
		якщо (filewritable(buf.string) != 2)
		{
		    якщо (vim_mkdir(buf.string, 0755) != 0)
		    {
			vim_free(buf.string);
			повернути;
		    }
		}

		buf.length += vim_snprintf_safelen((char *)buf.string + buf.length,
		    MAXPATHL - buf.length, "/%.*s", (ціле)(lend - lstart), lstart);
	    }
	    fname = LANGP_ENTRY(curwin->w_s->b_langp, 0)
		->lp_slang->sl_fname;
	    vim_snprintf((char *)buf.string + buf.length, MAXPATHL - buf.length,
		".%s.add",
		fname != NULL && strstr((char *)gettail(fname), ".ascii.") != NULL
		    ? (char_u *)"ascii" : spell_enc());
	    set_option_value_give_err((char_u *)"spellfile",
		    0L, buf.string, OPT_LOCAL);
	    break;
	}
	aspath = FALSE;
    }

    vim_free(buf.string);
}



/*
 * Set the spell character tables from strings in the affix file.
 */
    static ціле
set_spell_chartab(char_u *fol, char_u *low, char_u *upp)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    char_u	*pf = fol, *pl = low, *pu = upp;
    ціле		f, l, u;

    clear_spell_chartab(&new_st);

    поки (*pf != NUL)
    {
	якщо (*pl == NUL || *pu == NUL)
	{
	    emsg(_(e_format_error_in_affix_file_fol_low_or_upp));
	    повернути FAIL;
	}
	f = mb_ptr2char_adv(&pf);
	l = mb_ptr2char_adv(&pl);
	u = mb_ptr2char_adv(&pu);

	// Every character that appears is a word character.
	якщо (f < 256)
	    new_st.st_isw[f] = TRUE;
	якщо (l < 256)
	    new_st.st_isw[l] = TRUE;
	якщо (u < 256)
	    new_st.st_isw[u] = TRUE;

	// if "LOW" and "FOL" are not the same the "LOW" char needs
	// case-folding
	якщо (l < 256 && l != f)
	{
	    якщо (f >= 256)
	    {
		emsg(_(e_character_in_fol_low_or_upp_is_out_of_range));
		повернути FAIL;
	    }
	    new_st.st_fold[l] = f;
	}

	// if "UPP" and "FOL" are not the same the "UPP" char needs
	// case-folding, it's upper case and the "UPP" is the upper case of
	// "FOL" .
	якщо (u < 256 && u != f)
	{
	    якщо (f >= 256)
	    {
		emsg(_(e_character_in_fol_low_or_upp_is_out_of_range));
		повернути FAIL;
	    }
	    new_st.st_fold[u] = f;
	    new_st.st_isu[u] = TRUE;
	    new_st.st_upper[f] = u;
	}
    }

    якщо (*pl != NUL || *pu != NUL)
    {
	emsg(_(e_format_error_in_affix_file_fol_low_or_upp));
	повернути FAIL;
    }

    повернути set_spell_finish(&new_st);
}

/*
 * Set the spell character tables from strings in the .spl file.
 */
    static void
set_spell_charflags(
    char_u	*flags,
    ціле		cnt,	    // length of "flags"
    char_u	*fol)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    ціле		i;
    char_u	*p = fol;
    ціле		c;

    clear_spell_chartab(&new_st);

    для (i = 0; i < 128; ++i)
    {
	якщо (i < cnt)
	{
	    new_st.st_isw[i + 128] = (flags[i] & CF_WORD) != 0;
	    new_st.st_isu[i + 128] = (flags[i] & CF_UPPER) != 0;
	}

	якщо (*p != NUL)
	{
	    c = mb_ptr2char_adv(&p);
	    new_st.st_fold[i + 128] = c;
	    якщо (i + 128 != c && new_st.st_isu[i + 128] && c < 256)
		new_st.st_upper[c] = i + 128;
	}
    }

    (void)set_spell_finish(&new_st);
}

    static ціле
set_spell_finish(spelltab_T *new_st)
{
    ціле		i;

    якщо (did_set_spelltab)
    {
	// check that it's the same table
	для (i = 0; i < 256; ++i)
	{
	    якщо (spelltab.st_isw[i] != new_st->st_isw[i]
		    || spelltab.st_isu[i] != new_st->st_isu[i]
		    || spelltab.st_fold[i] != new_st->st_fold[i]
		    || spelltab.st_upper[i] != new_st->st_upper[i])
	    {
		emsg(_(e_word_characters_differ_between_spell_files));
		повернути FAIL;
	    }
	}
    }
    інакше
    {
	// copy the new spelltab into the one being used
	spelltab = *new_st;
	did_set_spelltab = TRUE;
    }

    повернути OK;
}

/*
 * Write the table with prefix conditions to the .spl file.
 * When "fd" is NULL only count the length of what is written and return it.
 */
    static ціле
write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv)
{
    ціле		i;
    char_u	*p;
    ціле		len;
    ціле		totlen;

    якщо (fd != NULL)
	put_bytes(fd, (long_u)gap->ga_len, 2);	    // <prefcondcnt>

    totlen = 2 + gap->ga_len; // length of <prefcondcnt> and <condlen> bytes

    для (i = 0; i < gap->ga_len; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	p = ((char_u **)gap->ga_data)[i];
	якщо (p != NULL)
	{
	    len = (ціле)STRLEN(p);
	    якщо (fd != NULL)
	    {
		fputc(len, fd);
		*fwv &= fwrite(p, (size_t)len, (size_t)1, fd);
	    }
	    totlen += len;
	}
	інакше якщо (fd != NULL)
	    fputc(0, fd);
    }

    повернути totlen;
}

/*
 * Use map string "map" for languages "lp".
 */
    static void
set_map_str(slang_T *lp, char_u *map)
{
    char_u	*p;
    ціле		headc = 0;
    ціле		c;
    ціле		i;

    якщо (*map == NUL)
    {
	lp->sl_has_map = FALSE;
	повернути;
    }
    lp->sl_has_map = TRUE;

    // Init the array and hash tables empty.
    для (i = 0; i < 256; ++i)
	lp->sl_map_array[i] = 0;
    hash_init(&lp->sl_map_hash);

    /*
     * The similar characters are stored separated with slashes:
     * "aaa/bbb/ccc/".  Fill sl_map_array[c] with the character before c and
     * before the same slash.  For characters above 255 sl_map_hash is used.
     */
    для (p = map; *p != NUL; )
    {
	c = mb_cptr2char_adv(&p);
	якщо (c == '/')
	    headc = 0;
	інакше
	{
	    якщо (headc == 0)
		 headc = c;

	    // Characters above 255 don't fit in sl_map_array[], put them in
	    // the hash table.  Each entry is the char, a NUL the headchar and
	    // a NUL.
	    якщо (c >= 256)
	    {
		ціле	    cl = mb_char2len(c);
		ціле	    headcl = mb_char2len(headc);
		char_u	    *b;
		hash_T	    hash;
		hashitem_T  *hi;

		b = alloc(cl + headcl + 2);
		якщо (b == NULL)
		    повернути;
		mb_char2bytes(c, b);
		b[cl] = NUL;
		mb_char2bytes(headc, b + cl + 1);
		b[cl + 1 + headcl] = NUL;
		hash = hash_hash(b);
		hi = hash_lookup(&lp->sl_map_hash, b, hash);
		якщо (HASHITEM_EMPTY(hi))
		    hash_add_item(&lp->sl_map_hash, hi, b, hash);
		інакше
		{
		    // This should have been checked when generating the .spl
		    // file.
		    emsg(_(e_duplicate_char_in_map_entry));
		    vim_free(b);
		}
	    }
	    інакше
		lp->sl_map_array[c] = headc;
	}
    }
}

#endif  // FEAT_SPELL
