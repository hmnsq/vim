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

#شامل "vim.h"

#اگر defined(FEAT_SPELL)

#شامل <time.h>	// for time_t

// Special byte values for <byte>.  Some are only used in the tree for
// postponed prefixes, some only in the other trees.  This is a bit messy...
#تعریف_کرو BY_NOFLAGS	0	// end of word without flags or region; for
				// postponed prefix: no <pflags>
#تعریف_کرو BY_اشاریہ	1	// child is shared, index follows
#تعریف_کرو BY_FLAGS	2	// end of word, <flags> byte follows; for
				// postponed prefix: <pflags> follows
#تعریف_کرو BY_FLAGS2	3	// end of word, <flags> and <flags2> bytes
				// follow; never used in prefix tree
#تعریف_کرو BY_SPECIAL  BY_FLAGS2	// highest special byte value

#تعریف_کرو ZERO_FLAG	65009	// used when flag is zero: "0"

// Flags used in .spl file for soundsalike flags.
#تعریف_کرو SAL_F0LLOWUP		1
#تعریف_کرو SAL_COLLAPSE		2
#تعریف_کرو SAL_REM_ACCENTS		4

#تعریف_کرو VIMSPELLMAGIC "VIMspell"  // string at start of Vim spell file
#تعریف_کرو VIMSPELLMAGICL 8
#تعریف_کرو VIMSPELLVERSION 50

// Section IDs.  Only renumber them when VIMSPELLVERSION changes!
#تعریف_کرو SN_REGION	0	// <regionname> section
#تعریف_کرو SN_CHARFLAGS	1	// charflags section
#تعریف_کرو SN_MIDWORD	2	// <midword> section
#تعریف_کرو SN_PREFCOND	3	// <prefcond> section
#تعریف_کرو SN_REP		4	// REP items section
#تعریف_کرو SN_SAL		5	// SAL items section
#تعریف_کرو SN_SOFO		6	// soundfolding section
#تعریف_کرو SN_MAP		7	// MAP items section
#تعریف_کرو SN_COMPOUND	8	// compound words section
#تعریف_کرو SN_SYLLABLE	9	// syllable section
#تعریف_کرو SN_NOBREAK	10	// NOBREAK section
#تعریف_کرو SN_SUGFILE	11	// timestamp for .sug file
#تعریف_کرو SN_REPSAL	12	// REPSAL items section
#تعریف_کرو SN_الفاظ	13	// common words
#تعریف_کرو SN_NOSPLITSUGS	14	// don't split word for suggestions
#تعریف_کرو SN_INFO		15	// info section
#تعریف_کرو SN_NOCOMPOUNDSUGS 16	// don't compound for suggestions
#تعریف_کرو SN_انتہا		255	// end of sections

#تعریف_کرو SNF_REQUIRED	1	// <sectionflags>: required section

#تعریف_کرو CF_لفظ		0x01
#تعریف_کرو CF_بالائی	0x02

// Max allowed length for COMPOUND section
#تعریف_کرو COMPOUND_MAX_LEN	100000

/*
 * Loop through all the siblings of a node (including the node)
 */
#تعریف_کرو FOR_ALL_گرہ_SIBLINGS(گرہ, np) \
    برائے ((np) = (گرہ); (np) != تہی; (np) = (np)->wn_sibling)

ساکن صحیح سیٹ_spell_اختتام(spelltab_T	*new_st);
ساکن صحیح write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv);
ساکن صحیح read_region_section(FILE *fd, slang_T *slang, صحیح len);
ساکن صحیح read_charflags_section(FILE *fd);
ساکن صحیح read_prefcond_section(FILE *fd, slang_T *lp);
ساکن صحیح read_rep_section(FILE *fd, garray_T *gap, مختصر *پہلا);
ساکن صحیح read_sal_section(FILE *fd, slang_T *slang);
ساکن صحیح read_الفاظ_section(FILE *fd, slang_T *lp, صحیح len);
ساکن صحیح read_sofo_section(FILE *fd, slang_T *slang);
ساکن صحیح read_compound(FILE *fd, slang_T *slang, صحیح len);
ساکن صحیح سیٹ_sofo(slang_T *lp, char_u *from, char_u *مقصد);
ساکن خالی سیٹ_sal_پہلا(slang_T *lp);
ساکن صحیح *mb_str2wide(char_u *s);
ساکن صحیح spell_read_درخت(FILE *fd, char_u **bytsp, طویل *bytsp_len, idx_T **idxsp, صحیح prefixtree, صحیح prefixcnt);
ساکن idx_T read_درخت_گرہ(FILE *fd, char_u *byts, idx_T *idxs, صحیح maxidx, idx_T startidx, صحیح prefixtree, صحیح maxprefcondnr, صحیح گہرائی);
ساکن خالی سیٹ_spell_charflags(char_u *flags, صحیح cnt, char_u *upp);
ساکن صحیح سیٹ_spell_chartab(char_u *fol, char_u *کم, char_u *upp);
ساکن خالی سیٹ_map_str(slang_T *lp, char_u *map);


ساکن حرف *e_afftrailing = N_("Trailing text in %s line %d: %s");
ساکن حرف *e_affname = N_("Affix name too long in %s line %d: %s");
ساکن حرف *پیغام_compressing = N_("Compressing word tree...");

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
spell_بھرو_file(
    char_u	*fname,
    char_u	*lang,
    slang_T	*old_lp,
    صحیح		silent)		// no error if file doesn't exist
{
    FILE	*fd;
    char_u	buf[VIMSPELLMAGICL];
    char_u	*p;
    صحیح		i;
    صحیح		n;
    صحیح		len;
    slang_T	*lp = تہی;
    صحیح		c = 0;
    صحیح		res;
    صحیح		did_estack_ڈالو = FALSE;
    ESTACK_CHECK_DECLARATION;

    fd = mch_fopen((حرف *)fname, "r");
    اگر (fd == تہی)
    {
	اگر (!silent)
	    semsg(_(e_cant_open_file_str), fname);
	ورنہ اگر (p_verbose > 2)
	{
	    verbose_enter();
	    smsg((ثابت حرف *)e_cant_open_file_str, fname);
	    verbose_leave();
	}
	جاؤ انتہا_FAIL;
    }
    اگر (p_verbose > 2)
    {
	verbose_enter();
	smsg(_("Reading spell file \"%s\""), fname);
	verbose_leave();
    }

    اگر (old_lp == تہی)
    {
	lp = slang_alloc(lang);
	اگر (lp == تہی)
	    جاؤ انتہا_FAIL;

	// Remember the file name, used to reload the file when it's updated.
	lp->sl_fname = vim_strsave(fname);
	اگر (lp->sl_fname == تہی)
	    جاؤ انتہا_FAIL;

	// Check for .add.spl (_add.spl for VMS).
	lp->sl_جوڑو = لڑی_تلاش((حرف *)gettail(fname), SPL_FNAME_جوڑو) != تہی;
    }
    ورنہ
	lp = old_lp;

    // Set sourcing_name, so that error messages mention the file name.
    estack_ڈالو(ETYPE_SPELL, fname, 0);
    ESTACK_CHECK_تیاری;
    did_estack_ڈالو = TRUE;

    /*
     * <HEADER>: <fileID>
     */
    برائے (i = 0; i < VIMSPELLMAGICL; ++i)
	buf[i] = (c = getc(fd)) == EOF ? 0 : c;		// <fileID>
    اگر (STRNCMP(buf, VIMSPELLMAGIC, VIMSPELLMAGICL) != 0)
    {
	emsg(_(e_this_does_not_look_like_spell_file));
	جاؤ انتہا_FAIL;
    }
    c = getc(fd);					// <versionnr>
    اگر (c < VIMSPELLVERSION)
    {
	emsg(_(e_old_spell_file_needs_مقصد_be_updated));
	جاؤ انتہا_FAIL;
    }
    ورنہ اگر (c > VIMSPELLVERSION)
    {
	emsg(_(e_spell_file_is_for_newer_نسخہ_کا_vim));
	جاؤ انتہا_FAIL;
    }


    /*
     * <SECTIONS>: <section> ... <sectionend>
     * <section>: <sectionID> <sectionflags> <sectionlen> (section contents)
     */
    برائے (;;)
    {
	n = getc(fd);			    // <sectionID> or <sectionend>
	اگر (n == SN_انتہا)
	    توڑو;
	c = getc(fd);					// <sectionflags>
	len = get4c(fd);				// <sectionlen>
	اگر (len < 0)
	    جاؤ truncerr;

	res = 0;
	انتخاب (n)
	{
	    حالت SN_INFO:
		lp->sl_info = read_string(fd, len);	// <infotext>
		اگر (lp->sl_info == تہی)
		    جاؤ انتہا_FAIL;
		توڑو;

	    حالت SN_REGION:
		res = read_region_section(fd, lp, len);
		توڑو;

	    حالت SN_CHARFLAGS:
		res = read_charflags_section(fd);
		توڑو;

	    حالت SN_MIDWORD:
		lp->sl_midword = read_string(fd, len);	// <midword>
		اگر (lp->sl_midword == تہی)
		    جاؤ انتہا_FAIL;
		توڑو;

	    حالت SN_PREFCOND:
		res = read_prefcond_section(fd, lp);
		توڑو;

	    حالت SN_REP:
		res = read_rep_section(fd, &lp->sl_rep, lp->sl_rep_پہلا);
		توڑو;

	    حالت SN_REPSAL:
		res = read_rep_section(fd, &lp->sl_repsal, lp->sl_repsal_پہلا);
		توڑو;

	    حالت SN_SAL:
		res = read_sal_section(fd, lp);
		توڑو;

	    حالت SN_SOFO:
		res = read_sofo_section(fd, lp);
		توڑو;

	    حالت SN_MAP:
		p = read_string(fd, len);		// <mapstr>
		اگر (p == تہی)
		    جاؤ انتہا_FAIL;
		سیٹ_map_str(lp, p);
		vim_free(p);
		توڑو;

	    حالت SN_الفاظ:
		res = read_الفاظ_section(fd, lp, len);
		توڑو;

	    حالت SN_SUGFILE:
		lp->sl_sugtime = get8ctime(fd);		// <timestamp>
		توڑو;

	    حالت SN_NOSPLITSUGS:
		lp->sl_nosplitsugs = TRUE;
		توڑو;

	    حالت SN_NOCOMPOUNDSUGS:
		lp->sl_nocompoundsugs = TRUE;
		توڑو;

	    حالت SN_COMPOUND:
		res = read_compound(fd, lp, len);
		توڑو;

	    حالت SN_NOBREAK:
		lp->sl_nobreak = TRUE;
		توڑو;

	    حالت SN_SYLLABLE:
		lp->sl_syllable = read_string(fd, len);	// <syllable>
		اگر (lp->sl_syllable == تہی)
		    جاؤ انتہا_FAIL;
		اگر (init_syl_tab(lp) != OK)
		    جاؤ انتہا_FAIL;
		توڑو;

	    طےشدہ:
		// Unsupported section.  When it's required give an error
		// message.  When it's not required skip the contents.
		اگر (c & SNF_REQUIRED)
		{
		    emsg(_(e_unsupported_section_اندر_spell_file));
		    جاؤ انتہا_FAIL;
		}
		جب (--len >= 0)
		    اگر (getc(fd) < 0)
			جاؤ truncerr;
		توڑو;
	}
someerror:
	اگر (res == SP_FORMERROR)
	{
	    emsg(_(e_format_error_اندر_spell_file));
	    جاؤ انتہا_FAIL;
	}
	اگر (res == SP_TRUNCERROR)
	{
truncerr:
	    emsg(_(e_truncated_spell_file));
	    جاؤ انتہا_FAIL;
	}
	اگر (res == SP_OTHERERROR)
	    جاؤ انتہا_FAIL;
    }

    // <LWORDTREE>
    res = spell_read_درخت(fd, &lp->sl_fbyts, &lp->sl_fbyts_len,
						      &lp->sl_fidxs, FALSE, 0);
    اگر (res != 0)
	جاؤ someerror;

    // <KWORDTREE>
    res = spell_read_درخت(fd, &lp->sl_kbyts, تہی, &lp->sl_kidxs, FALSE, 0);
    اگر (res != 0)
	جاؤ someerror;

    // <PREFIXTREE>
    res = spell_read_درخت(fd, &lp->sl_pbyts, تہی, &lp->sl_pidxs, TRUE,
							    lp->sl_prefixcnt);
    اگر (res != 0)
	جاؤ someerror;

    // For a new file link it in the list of spell files.
    اگر (old_lp == تہی && lang != تہی)
    {
	lp->sl_اگلا = پہلا_lang;
	پہلا_lang = lp;
    }

    جاؤ انتہا_OK;

انتہا_FAIL:
    اگر (lang != تہی)
	// truncating the name signals the error to spell_load_lang()
	*lang = NUL;
    اگر (lp != تہی && old_lp == تہی)
	slang_free(lp);
    lp = تہی;

انتہا_OK:
    اگر (fd != تہی)
	fclose(fd);
    اگر (did_estack_ڈالو)
    {
	ESTACK_CHECK_NOW;
	estack_نکالو();
    }

    واپس lp;
}

/*
 * Fill in the wordcount fields for a trie.
 * Returns the total number of words.
 */
    ساکن خالی
درخت_گنتی_الفاظ(char_u *byts, طویل byts_len, idx_T *idxs)
{
    صحیح		گہرائی;
    idx_T	arridx[MAXWLEN];
    صحیح		curi[MAXWLEN];
    صحیح		c;
    idx_T	n;
    صحیح		wordcount[MAXWLEN];

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;
    گہرائی = 0;
    جب (گہرائی >= 0 && !got_int)
    {
	اگر (curi[گہرائی] > byts[arridx[گہرائی]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[گہرائی]] = wordcount[گہرائی];
	    اگر (گہرائی > 0)
		wordcount[گہرائی - 1] += wordcount[گہرائی];

	    --گہرائی;
	    fast_breakcheck();
	}
	ورنہ
	{
	    // Do one more byte at this node.
	    n = arridx[گہرائی] + curi[گہرائی];
	    ++curi[گہرائی];

	    c = byts[n];
	    اگر (c == 0)
	    {
		// End of word, count it.
		++wordcount[گہرائی];

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		جب (n + 1 < byts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[گہرائی];
		}
	    }
	    ورنہ اگر (گہرائی < MAXWLEN - 1)
	    {
		// Normal char, go one level deeper to count the words.
		++گہرائی;
		arridx[گہرائی] = idxs[n];
		curi[گہرائی] = 1;
		wordcount[گہرائی] = 0;
	    }
	}
    }
}

/*
 * Load the .sug files for languages that have one and weren't loaded yet.
 */
    خالی
suggest_بھرو_files(خالی)
{
    langp_T	*lp;
    صحیح		lpi;
    slang_T	*slang;
    char_u	*dotp;
    FILE	*fd;
    char_u	buf[MAXWLEN];
    صحیح		i;
    time_t	timestamp;
    صحیح		wcount;
    صحیح		wordnr;
    garray_T	ga;
    صحیح		c;

    // Do this for all languages that support sound folding.
    برائے (lpi = 0; lpi < curwin->w_s->b_langp.ga_len; ++lpi)
    {
	lp = LANGP_اندراج(curwin->w_s->b_langp, lpi);
	slang = lp->lp_slang;
	اگر (slang->sl_sugtime != 0 && !slang->sl_sugloaded)
	{
	    // Change ".spl" to ".sug" and open the file.  When the file isn't
	    // found silently skip it.  Do set "sl_sugloaded" so that we
	    // don't try again and again.
	    slang->sl_sugloaded = TRUE;

	    dotp = vim_strrchr(slang->sl_fname, '.');
	    اگر (dotp == تہی || fnamecmp(dotp, ".spl") != 0)
		جاری;
	    STRCPY(dotp, ".sug");
	    fd = mch_fopen((حرف *)slang->sl_fname, "r");
	    اگر (fd == تہی)
		جاؤ nextone;

	    /*
	     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
	     */
	    برائے (i = 0; i < VIMSUGMAGICL; ++i)
		buf[i] = (c = getc(fd)) == EOF ? 0 : c;	// <fileID>
	    اگر (STRNCMP(buf, VIMSUGMAGIC, VIMSUGMAGICL) != 0)
	    {
		semsg(_(e_this_does_not_look_like_sug_file_str),
							     slang->sl_fname);
		جاؤ nextone;
	    }
	    c = getc(fd);				// <versionnr>
	    اگر (c < VIMSUGVERSION)
	    {
		semsg(_(e_old_sug_file_needs_مقصد_be_updated_str),
							     slang->sl_fname);
		جاؤ nextone;
	    }
	    ورنہ اگر (c > VIMSUGVERSION)
	    {
		semsg(_(e_sug_file_is_for_newer_نسخہ_کا_vim_str),
							     slang->sl_fname);
		جاؤ nextone;
	    }

	    // Check the timestamp, it must be exactly the same as the one in
	    // the .spl file.  Otherwise the word numbers won't match.
	    timestamp = get8ctime(fd);			// <timestamp>
	    اگر (timestamp != slang->sl_sugtime)
	    {
		semsg(_(e_sug_file_doesnt_مطابقت_spl_file_str),
							     slang->sl_fname);
		جاؤ nextone;
	    }

	    /*
	     * <SUGWORDTREE>: <wordtree>
	     * Read the trie with the soundfolded words.
	     */
	    اگر (spell_read_درخت(fd, &slang->sl_sbyts, &slang->sl_sbyts_len,
			&slang->sl_sidxs, FALSE, 0) != 0)
	    {
someerror:
		semsg(_(e_error_while_reading_sug_file_str),
							     slang->sl_fname);
		slang_clear_sug(slang);
		جاؤ nextone;
	    }

	    /*
	     * <SUGTABLE>: <sugwcount> <sugline> ...
	     *
	     * Read the table with word numbers.  We use a file buffer for
	     * this, because it's so much like a file with lines.  Makes it
	     * possible to swap the info and save on memory use.
	     */
	    slang->sl_sugbuf = open_spellbuf();
	    اگر (slang->sl_sugbuf == تہی)
		جاؤ someerror;
							    // <sugwcount>
	    wcount = get4c(fd);
	    اگر (wcount < 0)
		جاؤ someerror;

	    // Read all the wordnr lists into the buffer, one NUL terminated
	    // list per line.
	    ga_init2(&ga, 1, 100);
	    برائے (wordnr = 0; wordnr < wcount; ++wordnr)
	    {
		ga.ga_len = 0;
		برائے (;;)
		{
		    c = getc(fd);			    // <sugline>
		    اگر (c < 0 || ga_grow(&ga, 1) == FAIL)
			جاؤ someerror;
		    ((char_u *)ga.ga_ڈیٹا)[ga.ga_len++] = c;
		    اگر (c == NUL)
			توڑو;
		}
		اگر (ml_append_buf(slang->sl_sugbuf, (linenr_T)wordnr,
					 ga.ga_ڈیٹا, ga.ga_len, TRUE) == FAIL)
		    جاؤ someerror;
	    }
	    ga_clear(&ga);

	    /*
	     * Need to put word counts in the word tries, so that we can find
	     * a word by its number.
	     */
	    درخت_گنتی_الفاظ(slang->sl_fbyts, slang->sl_fbyts_len,
		    slang->sl_fidxs);
	    درخت_گنتی_الفاظ(slang->sl_sbyts, slang->sl_sbyts_len,
		    slang->sl_sidxs);

nextone:
	    اگر (fd != تہی)
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
    ساکن char_u *
read_cnt_string(FILE *fd, صحیح cnt_بائٹیں, صحیح *cntp)
{
    صحیح		cnt = 0;
    صحیح		i;
    char_u	*str;

    // read the length bytes, MSB first
    برائے (i = 0; i < cnt_بائٹیں; ++i)
    {
	صحیح c = getc(fd);

	اگر (c == EOF)
	{
	    *cntp = SP_TRUNCERROR;
	    واپس تہی;
	}
	cnt = (cnt << 8) + (غیرعلامتی)c;
    }
    *cntp = cnt;
    اگر (cnt == 0)
	واپس تہی;	    // nothing to read, return NULL

    str = read_string(fd, cnt);
    اگر (str == تہی)
	*cntp = SP_OTHERERROR;
    واپس str;
}

/*
 * Read SN_REGION: <regionname> ...
 * Return SP_*ERROR flags.
 */
    ساکن صحیح
read_region_section(FILE *fd, slang_T *lp, صحیح len)
{
    صحیح		i;
    صحیح		c = 0;

    اگر (len > MAXREGIONS * 2)
	واپس SP_FORMERROR;
    برائے (i = 0; i < len; ++i)
	lp->sl_regions[i] = (c = getc(fd)) == EOF ? 0 : c; // <regionname>
    lp->sl_regions[len] = NUL;
    واپس c == EOF ? SP_TRUNCERROR : 0;
}

/*
 * Read SN_CHARFLAGS section: <charflagslen> <charflags>
 *				<folcharslen> <folchars>
 * Return SP_*ERROR flags.
 */
    ساکن صحیح
read_charflags_section(FILE *fd)
{
    char_u	*flags;
    char_u	*fol;
    صحیح		flagslen, follen;

    // <charflagslen> <charflags>
    flags = read_cnt_string(fd, 1, &flagslen);
    اگر (flagslen < 0)
	واپس flagslen;

    // <folcharslen> <folchars>
    fol = read_cnt_string(fd, 2, &follen);
    اگر (follen < 0)
    {
	vim_free(flags);
	واپس follen;
    }

    // Set the word-char flags and fill SPELL_ISUPPER() table.
    اگر (flags != تہی && fol != تہی)
	سیٹ_spell_charflags(flags, flagslen, fol);

    vim_free(flags);
    vim_free(fol);

    // When <charflagslen> is zero then <fcharlen> must also be zero.
    اگر ((flags == تہی) != (fol == تہی))
	واپس SP_FORMERROR;
    واپس 0;
}

/*
 * Read SN_PREFCOND section.
 * Return SP_*ERROR flags.
 */
    ساکن صحیح
read_prefcond_section(FILE *fd, slang_T *lp)
{
    صحیح		cnt;
    صحیح		i;
    صحیح		n;
    صحیح		c;
    char_u	*p;
    char_u	buf[MAXWLEN + 1];

    // <prefcondcnt> <prefcond> ...
    cnt = get2c(fd);					// <prefcondcnt>
    اگر (cnt <= 0)
	واپس SP_FORMERROR;

    lp->sl_prefprog = ALLOC_CLEAR_MULT(regprog_T *, cnt);
    اگر (lp->sl_prefprog == تہی)
	واپس SP_OTHERERROR;
    lp->sl_prefixcnt = cnt;

    برائے (i = 0; i < cnt; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	n = getc(fd);					// <condlen>
	اگر (n < 0 || n >= MAXWLEN)
	    واپس SP_FORMERROR;

	// When <condlen> is zero we have an empty condition.  Otherwise
	// compile the regexp program used to check for the condition.
	اگر (n > 0)
	{
	    buf[0] = '^';	    // always match at one position only
	    p = buf + 1;
	    جب (n-- > 0)
		*p++ = (c = getc(fd)) == EOF ? 0 : c;	    // <condstr>
	    اگر (c == EOF)
		توڑو;
	    *p = NUL;
	    lp->sl_prefprog[i] = vim_regcomp(buf, RE_MAGIC + RE_STRING);
	}
    }
    واپس 0;
}

/*
 * Read REP or REPSAL items section from "fd": <repcount> <rep> ...
 * Return SP_*ERROR flags.
 */
    ساکن صحیح
read_rep_section(FILE *fd, garray_T *gap, مختصر *پہلا)
{
    صحیح		cnt;
    fromto_T	*ftp;
    صحیح		i;

    cnt = get2c(fd);					// <repcount>
    اگر (cnt < 0)
	واپس SP_TRUNCERROR;

    اگر (ga_grow(gap, cnt) == FAIL)
	واپس SP_OTHERERROR;

    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
    برائے (; gap->ga_len < cnt; ++gap->ga_len)
    {
	ftp = &((fromto_T *)gap->ga_ڈیٹا)[gap->ga_len];
	ftp->ft_from = read_cnt_string(fd, 1, &i);
	اگر (i < 0)
	    واپس i;
	اگر (i == 0)
	    واپس SP_FORMERROR;
	ftp->ft_مقصد = read_cnt_string(fd, 1, &i);
	اگر (i <= 0)
	{
	    vim_free(ftp->ft_from);
	    اگر (i < 0)
		واپس i;
	    واپس SP_FORMERROR;
	}
    }

    // Fill the first-index table.
    برائے (i = 0; i < 256; ++i)
	پہلا[i] = -1;
    برائے (i = 0; i < gap->ga_len; ++i)
    {
	ftp = &((fromto_T *)gap->ga_ڈیٹا)[i];
	اگر (پہلا[*ftp->ft_from] == -1)
	    پہلا[*ftp->ft_from] = i;
    }
    واپس 0;
}

/*
 * Read SN_SAL section: <salflags> <salcount> <sal> ...
 * Return SP_*ERROR flags.
 */
    ساکن صحیح
read_sal_section(FILE *fd, slang_T *slang)
{
    صحیح		i;
    صحیح		cnt;
    garray_T	*gap;
    salitem_T	*smp;
    صحیح		ccnt;
    char_u	*p;

    slang->sl_sofo = FALSE;

    i = getc(fd);				// <salflags>
    اگر (i & SAL_F0LLOWUP)
	slang->sl_followup = TRUE;
    اگر (i & SAL_COLLAPSE)
	slang->sl_collapse = TRUE;
    اگر (i & SAL_REM_ACCENTS)
	slang->sl_rem_accents = TRUE;

    cnt = get2c(fd);				// <salcount>
    اگر (cnt < 0)
	واپس SP_TRUNCERROR;

    gap = &slang->sl_sal;
    ga_init2(gap, حجم(salitem_T), 10);
    اگر (ga_grow(gap, cnt + 1) == FAIL)
	واپس SP_OTHERERROR;

    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
    برائے (; gap->ga_len < cnt; ++gap->ga_len)
    {
	صحیح	c = NUL;

	smp = &((salitem_T *)gap->ga_ڈیٹا)[gap->ga_len];
	ccnt = getc(fd);			// <salfromlen>
	اگر (ccnt < 0)
	    واپس SP_TRUNCERROR;
	اگر ((p = alloc(ccnt + 2)) == تہی)
	    واپس SP_OTHERERROR;
	smp->sm_lead = p;

	// Read up to the first special char into sm_lead.
	برائے (i = 0; i < ccnt; ++i)
	{
	    c = getc(fd);			// <salfrom>
	    اگر (vim_strchr((char_u *)"0123456789(-<^$", c) != تہی)
		توڑو;
	    *p++ = c;
	}
	smp->sm_leadlen = (صحیح)(p - smp->sm_lead);
	*p++ = NUL;

	// Put (abc) chars in sm_oneof, if any.
	اگر (c == '(')
	{
	    smp->sm_oneof = p;
	    برائے (++i; i < ccnt; ++i)
	    {
		c = getc(fd);			// <salfrom>
		اگر (c == ')')
		    توڑو;
		*p++ = c;
	    }
	    *p++ = NUL;
	    اگر (++i < ccnt)
		c = getc(fd);
	}
	ورنہ
	    smp->sm_oneof = تہی;

	// Any following chars go in sm_rules.
	smp->sm_قواعد = p;
	اگر (i < ccnt)
	    // store the char we got while checking for end of sm_lead
	    *p++ = c;
	برائے (++i; i < ccnt; ++i)
	    *p++ = (c = getc(fd)) == EOF ? 0 : c;	// <salfrom>
	*p++ = NUL;

	// <saltolen> <salto>
	smp->sm_مقصد = read_cnt_string(fd, 1, &ccnt);
	اگر (ccnt < 0)
	{
	    vim_free(smp->sm_lead);
	    واپس ccnt;
	}

	اگر (has_mbyte)
	{
	    // convert the multi-byte strings to wide char strings
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = mb_charlen(smp->sm_lead);
	    اگر (smp->sm_oneof == تہی)
		smp->sm_oneof_w = تہی;
	    ورنہ
		smp->sm_oneof_w = mb_str2wide(smp->sm_oneof);
	    اگر (smp->sm_مقصد == تہی)
		smp->sm_مقصد_w = تہی;
	    ورنہ
		smp->sm_مقصد_w = mb_str2wide(smp->sm_مقصد);
	    اگر (smp->sm_lead_w == تہی
		    || (smp->sm_oneof_w == تہی && smp->sm_oneof != تہی)
		    || (smp->sm_مقصد_w == تہی && smp->sm_مقصد != تہی))
	    {
		vim_free(smp->sm_lead);
		vim_free(smp->sm_مقصد);
		vim_free(smp->sm_lead_w);
		vim_free(smp->sm_oneof_w);
		vim_free(smp->sm_مقصد_w);
		واپس SP_OTHERERROR;
	    }
	}
    }

    اگر (gap->ga_len > 0)
    {
	// Add one extra entry to mark the end with an empty sm_lead.  Avoids
	// that we need to check the index every time.
	smp = &((salitem_T *)gap->ga_ڈیٹا)[gap->ga_len];
	اگر ((p = alloc(1)) == تہی)
	    واپس SP_OTHERERROR;
	p[0] = NUL;
	smp->sm_lead = p;
	smp->sm_leadlen = 0;
	smp->sm_oneof = تہی;
	smp->sm_قواعد = p;
	smp->sm_مقصد = تہی;
	اگر (has_mbyte)
	{
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = 0;
	    smp->sm_oneof_w = تہی;
	    smp->sm_مقصد_w = تہی;
	}
	++gap->ga_len;
    }

    // Fill the first-index table.
    سیٹ_sal_پہلا(slang);

    واپس 0;
}

/*
 * Read SN_WORDS: <word> ...
 * Return SP_*ERROR flags.
 */
    ساکن صحیح
read_الفاظ_section(FILE *fd, slang_T *lp, صحیح len)
{
    صحیح		مکمل = 0;
    صحیح		i;
    صحیح		c;
    char_u	لفظ[MAXWLEN];

    جب (مکمل < len)
    {
	// Read one word at a time.
	برائے (i = 0; ; ++i)
	{
	    c = getc(fd);
	    اگر (c == EOF)
		واپس SP_TRUNCERROR;
	    لفظ[i] = c;
	    اگر (لفظ[i] == NUL)
		توڑو;
	    اگر (i == MAXWLEN - 1)
		واپس SP_FORMERROR;
	}

	// Init the count to 10.
	گنتی_common_لفظ(lp, لفظ, -1, 10);
	مکمل += i + 1;
    }
    واپس 0;
}

/*
 * SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
 * Return SP_*ERROR flags.
 */
    ساکن صحیح
read_sofo_section(FILE *fd, slang_T *slang)
{
    صحیح		cnt;
    char_u	*from, *مقصد;
    صحیح		res;

    // <sofofromlen> <sofofrom>
    from = read_cnt_string(fd, 2, &cnt);
    اگر (cnt < 0)
	واپس cnt;

    // <sofotolen> <sofoto>
    مقصد = read_cnt_string(fd, 2, &cnt);
    اگر (cnt < 0)
    {
	vim_free(from);
	واپس cnt;
    }

    // Store the info in slang->sl_sal and/or slang->sl_sal_first.
    اگر (from != تہی && مقصد != تہی)
	res = سیٹ_sofo(slang, from, مقصد);
    ورنہ اگر (from != تہی || مقصد != تہی)
	res = SP_FORMERROR;    // only one of two strings is an error
    ورنہ
	res = 0;

    vim_free(from);
    vim_free(مقصد);
    واپس res;
}

/*
 * Read the compound section from the .spl file:
 *	<compmax> <compminlen> <compsylmax> <compoptions> <compflags>
 * Returns SP_*ERROR flags.
 */
    ساکن صحیح
read_compound(FILE *fd, slang_T *slang, صحیح len)
{
    صحیح		todo = len;
    صحیح		c;
    صحیح		atstart;
    char_u	*pat;
    char_u	*pp;
    char_u	*cp;
    char_u	*ap;
    char_u	*crp;
    صحیح		cnt;
    garray_T	*gap;
    size_t	patsize;
    size_t	flagsize;

    اگر (todo < 2)
	واپس SP_FORMERROR;	// need at least two bytes

    --todo;
    c = getc(fd);					// <compmax>
    اگر (c < 2)
	c = MAXWLEN;
    slang->sl_compmax = c;

    --todo;
    c = getc(fd);					// <compminlen>
    اگر (c < 1)
	c = 0;
    slang->sl_compminlen = c;

    --todo;
    c = getc(fd);					// <compsylmax>
    اگر (c < 1)
	c = MAXWLEN;
    slang->sl_compsylmax = c;

    c = getc(fd);					// <compoptions>
    اگر (c != 0)
	ungetc(c, fd);	    // be backwards compatible with Vim 7.0b
    ورنہ
    {
	--todo;
	c = getc(fd);	    // only use the lower byte for now
	--todo;
	slang->sl_compoptions = c;

	gap = &slang->sl_comppat;
	c = get2c(fd);					// <comppatcount>
	اگر (c < 0)
	    واپس SP_TRUNCERROR;
	todo -= 2;
	ga_init2(gap, حجم(char_u *), c);
	اگر (ga_grow(gap, c) == OK)
	    جب (--c >= 0)
	    {
		((char_u **)(gap->ga_ڈیٹا))[gap->ga_len++] =
						  read_cnt_string(fd, 1, &cnt);
					    // <comppatlen> <comppattext>
		اگر (cnt < 0)
		    واپس cnt;
		todo -= cnt + 1;
	    }
    }
    اگر (todo < 0)
	واپس SP_FORMERROR;

    // Turn the COMPOUNDRULE items into a regexp pattern:
    // "a[bc]/a*b+" -> "^\(a[bc]\|a*b\+\)$".
    // Inserting backslashes may double the length, "^\(\)$<Nul>" is 7 bytes.
    // Conversion to utf-8 may double the size.
    اگر ((size_t)todo > COMPOUND_MAX_LEN)
	واپس SP_FORMERROR;
    patsize = (size_t)todo * 2 + 7;
    اگر (enc_utf8)
	patsize += (size_t)todo * 2;
    flagsize = (size_t)todo + 1;
    pat = alloc(patsize);
    اگر (pat == تہی)
	واپس SP_OTHERERROR;

    // We also need a list of all flags that can appear at the start and one
    // for all flags.
    cp = alloc(flagsize);
    اگر (cp == تہی)
    {
	vim_free(pat);
	واپس SP_OTHERERROR;
    }
    slang->sl_compstartflags = cp;
    *cp = NUL;

    ap = alloc(flagsize);
    اگر (ap == تہی)
    {
	vim_free(pat);
	واپس SP_OTHERERROR;
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
    جب (todo-- > 0)
    {
	c = getc(fd);					// <compflags>
	اگر (c == EOF)
	{
	    vim_free(pat);
	    واپس SP_TRUNCERROR;
	}

	// Add all flags to "sl_compallflags".
	اگر (vim_strchr((char_u *)"?*+[]/", c) == تہی
		&& !byte_اندر_str(slang->sl_compallflags, c))
	{
	    *ap++ = c;
	    *ap = NUL;
	}

	اگر (atstart != 0)
	{
	    // At start of item: copy flags to "sl_compstartflags".  For a
	    // [abc] item set "atstart" to 2 and copy up to the ']'.
	    اگر (c == '[')
		atstart = 2;
	    ورنہ اگر (c == ']')
		atstart = 0;
	    ورنہ
	    {
		اگر (!byte_اندر_str(slang->sl_compstartflags, c))
		{
		    *cp++ = c;
		    *cp = NUL;
		}
		اگر (atstart == 1)
		    atstart = 0;
	    }
	}

	// Copy flag to "sl_comprules", unless we run into a wildcard.
	اگر (crp != تہی)
	{
	    اگر (c == '?' || c == '+' || c == '*')
	    {
		VIM_CLEAR(slang->sl_comprules);
		crp = تہی;
	    }
	    ورنہ
		*crp++ = c;
	}

	اگر (c == '/')	    // slash separates two items
	{
	    *pp++ = '\\';
	    *pp++ = '|';
	    atstart = 1;
	}
	ورنہ		    // normal char, "[abc]" and '*' are copied as-is
	{
	    اگر (c == '?' || c == '+' || c == '~')
		*pp++ = '\\';	    // "a?" becomes "a\?", "a+" becomes "a\+"
	    اگر (enc_utf8)
		pp += mb_char2bytes(c, pp);
	    ورنہ
		*pp++ = c;
	}
    }

    *pp++ = '\\';
    *pp++ = ')';
    *pp++ = '$';
    *pp = NUL;

    اگر (crp != تہی)
	*crp = NUL;

    slang->sl_compprog = vim_regcomp(pat, RE_MAGIC + RE_STRING + RE_STRICT);
    vim_free(pat);
    اگر (slang->sl_compprog == تہی)
	واپس SP_FORMERROR;

    واپس 0;
}

/*
 * Set the SOFOFROM and SOFOTO items in language "lp".
 * Returns SP_*ERROR flags when there is something wrong.
 */
    ساکن صحیح
سیٹ_sofo(slang_T *lp, char_u *from, char_u *مقصد)
{
    صحیح		i;

    garray_T	*gap;
    char_u	*s;
    char_u	*p;
    صحیح		c;
    صحیح		*inp;

    اگر (has_mbyte)
    {
	free_sal_اشیاء(&lp->sl_sal);

	// Use "sl_sal" as an array with 256 pointers to a list of wide
	// characters.  The index is the low byte of the character.
	// The list contains from-to pairs with a terminating NUL.
	// sl_sal_first[] is used for latin1 "from" characters.
	gap = &lp->sl_sal;
	ga_init2(gap, حجم(صحیح *), 1);
	اگر (ga_grow(gap, 256) == FAIL)
	    واپس SP_OTHERERROR;
	vim_memset(gap->ga_ڈیٹا, 0, حجم(صحیح *) * 256);
	gap->ga_len = 256;
	lp->sl_sofo = TRUE;

	// First count the number of items for each list.  Temporarily use
	// sl_sal_first[] for this.  Reset it first: a preceding SN_SAL section
	// may have set the entries to -1 via set_sal_first().
	vim_memset(lp->sl_sal_پہلا, 0, حجم(salfirst_T) * 256);
	برائے (p = from, s = مقصد; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    MB_CPTR_ADV(s);
	    اگر (c >= 256)
		++lp->sl_sal_پہلا[c & 0xff];
	}
	اگر (*p != NUL || *s != NUL)	    // lengths differ
	    واپس SP_FORMERROR;

	// Allocate the lists.
	برائے (i = 0; i < 256; ++i)
	    اگر (lp->sl_sal_پہلا[i] > 0)
	    {
		p = alloc(حجم(صحیح) * (lp->sl_sal_پہلا[i] * 2 + 1));
		اگر (p == تہی)
		    واپس SP_OTHERERROR;
		((صحیح **)gap->ga_ڈیٹا)[i] = (صحیح *)p;
		*(صحیح *)p = 0;
	    }

	// Put the characters up to 255 in sl_sal_first[] the rest in a sl_sal
	// list.
	vim_memset(lp->sl_sal_پہلا, 0, حجم(salfirst_T) * 256);
	برائے (p = from, s = مقصد; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    i = mb_cptr2char_adv(&s);
	    اگر (c >= 256)
	    {
		// Append the from-to chars at the end of the list with
		// the low byte.
		inp = ((صحیح **)gap->ga_ڈیٹا)[c & 0xff];
		جب (*inp != 0)
		    ++inp;
		*inp++ = c;		// from char
		*inp++ = i;		// to char
		*inp++ = NUL;		// NUL at the end
	    }
	    ورنہ
		// mapping byte to char is done in sl_sal_first[]
		lp->sl_sal_پہلا[c] = i;
	}
    }
    ورنہ
    {
	// mapping bytes to bytes is done in sl_sal_first[]
	اگر (STRLEN(from) != STRLEN(مقصد))
	    واپس SP_FORMERROR;

	برائے (i = 0; مقصد[i] != NUL; ++i)
	    lp->sl_sal_پہلا[from[i]] = مقصد[i];
	lp->sl_sal.ga_len = 1;		// indicates we have soundfolding
	lp->sl_sofo = TRUE;
    }

    واپس 0;
}

/*
 * Fill the first-index table for "lp".
 */
    ساکن خالی
سیٹ_sal_پہلا(slang_T *lp)
{
    salfirst_T	*sfirst;
    صحیح		i;
    salitem_T	*smp;
    صحیح		c;
    garray_T	*gap = &lp->sl_sal;

    sfirst = lp->sl_sal_پہلا;
    برائے (i = 0; i < 256; ++i)
	sfirst[i] = -1;
    smp = (salitem_T *)gap->ga_ڈیٹا;
    برائے (i = 0; i < gap->ga_len; ++i)
    {
	اگر (has_mbyte)
	    // Use the lowest byte of the first character.  For latin1 it's
	    // the character, for other encodings it should differ for most
	    // characters.
	    c = *smp[i].sm_lead_w & 0xff;
	ورنہ
	    c = *smp[i].sm_lead;
	اگر (sfirst[c] == -1)
	{
	    sfirst[c] = i;
	    اگر (has_mbyte)
	    {
		صحیح		n;

		// Make sure all entries with this byte are following each
		// other.  Move the ones that are in the wrong position.  Do
		// keep the same ordering!
		جب (i + 1 < gap->ga_len
				       && (*smp[i + 1].sm_lead_w & 0xff) == c)
		    // Skip over entry with same index byte.
		    ++i;

		برائے (n = 1; i + n < gap->ga_len; ++n)
		    اگر ((*smp[i + n].sm_lead_w & 0xff) == c)
		    {
			salitem_T  tsal;

			// Move entry with same index byte after the entries
			// we already found.
			++i;
			--n;
			tsal = smp[i + n];
			mch_memmove(smp + i + 1, smp + i,
						       حجم(salitem_T) * n);
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
    ساکن صحیح *
mb_str2wide(char_u *s)
{
    صحیح		*res;
    char_u	*p;
    صحیح		i = 0;

    res = ALLOC_MULT(صحیح, mb_charlen(s) + 1);
    اگر (res == تہی)
	واپس تہی;

    برائے (p = s; *p != NUL; )
	res[i++] = mb_ptr2char_adv(&p);
    res[i] = NUL;
    واپس res;
}

/*
 * Read a tree from the .spl or .sug file.
 * Allocates the memory and stores pointers in "bytsp" and "idxsp".
 * This is skipped when the tree has zero length.
 * Returns zero when OK, SP_ value for an error.
 */
    ساکن صحیح
spell_read_درخت(
    FILE	*fd,
    char_u	**bytsp,
    طویل	*bytsp_len,
    idx_T	**idxsp,
    صحیح		prefixtree,	// TRUE for the prefix tree
    صحیح		prefixcnt)	// when "prefixtree" is TRUE: prefix count
{
    طویل	len;
    صحیح		idx;
    char_u	*bp;
    idx_T	*ip;

    // The tree size was computed when writing the file, so that we can
    // allocate it as one long block. <nodecount>
    len = get4c(fd);
    اگر (len < 0)
	واپس SP_TRUNCERROR;
    اگر ((size_t)len > سائز_MAX / حجم(صحیح))
	// Invalid length, multiply with sizeof(int) would overflow.
	واپس SP_FORMERROR;
    اگر (len <= 0)
	واپس 0;

    // Allocate the byte array.  Zero-initialize so that any position the
    // tree does not visit reads as 0; a stray BY_INDEX shared reference
    // into such a slot then behaves as end-of-word in spellsuggest()
    // instead of consuming an arbitrary heap byte as a siblingcount.
    bp = alloc_clear(len);
    اگر (bp == تہی)
	واپس SP_OTHERERROR;
    *bytsp = bp;
    اگر (bytsp_len != تہی)
	*bytsp_len = len;

    // Allocate the index array.
    ip = lalloc_clear(len * حجم(صحیح), TRUE);
    اگر (ip == تہی)
	واپس SP_OTHERERROR;
    *idxsp = ip;

    // Recursively read the tree and store it in the array.
    idx = read_درخت_گرہ(fd, bp, ip, len, 0, prefixtree, prefixcnt, 0);
    اگر (idx < 0)
	واپس idx;
    اگر (idx != len)
	واپس SP_FORMERROR;
    واپس 0;
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
    ساکن idx_T
read_درخت_گرہ(
    FILE	*fd,
    char_u	*byts,
    idx_T	*idxs,
    صحیح		maxidx,		    // size of arrays
    idx_T	startidx,	    // current index in "byts" and "idxs"
    صحیح		prefixtree,	    // TRUE for reading PREFIXTREE
    صحیح		maxprefcondnr,	    // maximum for <prefcondnr>
    صحیح		گہرائی)		    // recursion level
{
    صحیح		len;
    صحیح		i;
    صحیح		n;
    idx_T	idx = startidx;
    صحیح		c;
    صحیح		c2;
#تعریف_کرو SHARED_نقاب	0x8000000

    // Bail out on a crafted .spl whose tree recurses beyond the maximum
    // word length: each tree level corresponds to one byte of a word, so
    // any well-formed file has depth <= MAXWLEN.
    اگر (گہرائی > MAXWLEN)
	واپس SP_FORMERROR;

    len = getc(fd);					// <siblingcount>
    اگر (len <= 0)
	واپس SP_TRUNCERROR;

    اگر (len >= maxidx - startidx)
	واپس SP_FORMERROR;
    byts[idx++] = len;

    // Read the byte values, flag/region bytes and shared indexes.
    برائے (i = 1; i <= len; ++i)
    {
	c = getc(fd);					// <byte>
	اگر (c < 0)
	    واپس SP_TRUNCERROR;
	اگر (c <= BY_SPECIAL)
	{
	    اگر (c == BY_NOFLAGS && !prefixtree)
	    {
		// No flags, all regions.
		idxs[idx] = 0;
		c = 0;
	    }
	    ورنہ اگر (c != BY_اشاریہ)
	    {
		اگر (prefixtree)
		{
		    // Read the optional pflags byte, the prefix ID and the
		    // condition nr.  In idxs[] store the prefix ID in the low
		    // byte, the condition index shifted up 8 bits, the flags
		    // shifted up 24 bits.
		    اگر (c == BY_FLAGS)
		    {
			n = getc(fd);			// <pflags>
			اگر (n < 0)
			    واپس SP_TRUNCERROR;
			c = n << 24;
		    }
		    ورنہ
			c = 0;

		    n = getc(fd);			// <affixID>
		    اگر (n < 0)
			واپس SP_TRUNCERROR;
		    c |= n;

		    n = get2c(fd);			// <prefcondnr>
		    اگر (n < 0 || n >= maxprefcondnr)
			واپس SP_FORMERROR;
		    c |= (n << 8);
		}
		ورنہ // c must be BY_FLAGS or BY_FLAGS2
		{
		    // Read flags and optional region and prefix ID.  In
		    // idxs[] the flags go in the low two bytes, region above
		    // that and prefix ID above the region.
		    c2 = c;
		    c = getc(fd);			// <flags>
		    اگر (c < 0)
			واپس SP_TRUNCERROR;
		    اگر (c2 == BY_FLAGS2)
		    {
			n = getc(fd);			// <flags2>
			اگر (n < 0)
			    واپس SP_TRUNCERROR;
			c = (n << 8) + c;
		    }
		    اگر (c & WF_REGION)
		    {
			n = getc(fd);			// <region>
			اگر (n < 0)
			    واپس SP_TRUNCERROR;
			c = (n << 16) + c;
		    }
		    اگر (c & WF_AFX)
		    {
			n = getc(fd);			// <affixID>
			اگر (n < 0)
			    واپس SP_TRUNCERROR;
			c = (n << 24) + c;
		    }
		}

		idxs[idx] = c;
		c = 0;
	    }
	    ورنہ // c == BY_INDEX
	    {
							// <nodeidx>
		n = get3c(fd);
		اگر (n < 0 || n >= maxidx)
		    واپس SP_FORMERROR;
		idxs[idx] = n + SHARED_نقاب;
		c = getc(fd);				// <xbyte>
	    }
	}
	byts[idx++] = c;
    }

    // Recursively read the children for non-shared siblings.
    // Skip the end-of-word ones (zero byte value) and the shared ones (and
    // remove SHARED_MASK)
    برائے (i = 1; i <= len; ++i)
	اگر (byts[startidx + i] != 0)
	{
	    اگر (idxs[startidx + i] & SHARED_نقاب)
		idxs[startidx + i] &= ~SHARED_نقاب;
	    ورنہ
	    {
		idxs[startidx + i] = idx;
		idx = read_درخت_گرہ(fd, byts, idxs, maxidx, idx,
					prefixtree, maxprefcondnr, گہرائی + 1);
		اگر (idx < 0)
		    توڑو;
	    }
	}

    واپس idx;
}

/*
 * Reload the spell file "fname" if it's loaded.
 */
    ساکن خالی
spell_reload_one(
    char_u	*fname,
    صحیح		added_لفظ)	// invoked through "zg"
{
    slang_T	*slang;
    صحیح		didit = FALSE;

    FOR_ALL_SPELL_LANGS(slang)
    {
	اگر (fullpathcmp(fname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	{
	    slang_clear(slang);
	    اگر (spell_بھرو_file(fname, تہی, slang, FALSE) == تہی)
		// reloading failed, clear the language
		slang_clear(slang);
	    redraw_all_later(UPD_SOME_VALID);
	    didit = TRUE;
	}
    }

    // When "zg" was used and the file wasn't loaded yet, should redo
    // 'spelllang' to load it now.
    اگر (added_لفظ && !didit)
	parse_spelllang(curwin);
}


/*
 * Functions for ":mkspell".
 */

#تعریف_کرو MAXLINELEN  500		// Maximum length in bytes of a line in a .aff
				// and .dic file.
/*
 * Main structure to store the contents of a ".aff" file.
 */
قسمتعریف ساخت afffile_S
{
    char_u	*af_enc;	// "SET", normalized, alloc'ed string or NULL
    صحیح		af_flagtype;	// AFT_CHAR, AFT_LONG, AFT_NUM or AFT_CAPLONG
    غیرعلامتی	af_rare;	// RARE ID for rare word
    غیرعلامتی	af_keepcase;	// KEEPCASE ID for keep-case word
    غیرعلامتی	af_bad;		// BAD ID for banned word
    غیرعلامتی	af_needaffix;	// NEEDAFFIX ID
    غیرعلامتی	af_circumfix;	// CIRCUMFIX ID
    غیرعلامتی	af_needcomp;	// NEEDCOMPOUND ID
    غیرعلامتی	af_comproot;	// COMPOUNDROOT ID
    غیرعلامتی	af_compforbid;	// COMPOUNDFORBIDFLAG ID
    غیرعلامتی	af_comppermit;	// COMPOUNDPERMITFLAG ID
    غیرعلامتی	af_nosuggest;	// NOSUGGEST ID
    صحیح		af_pfxpostpone;	// postpone prefixes without chop string and
				// without flags
    صحیح		af_ignoreextra;	// IGNOREEXTRA present
    hashtab_T	af_pref;	// hashtable for prefixes, affheader_T
    hashtab_T	af_suff;	// hashtable for suffixes, affheader_T
    hashtab_T	af_comp;	// hashtable for compound flags, compitem_T
} afffile_T;

#تعریف_کرو AFT_CHAR	0	// flags are one character
#تعریف_کرو AFT_LONG	1	// flags are two characters
#تعریف_کرو AFT_CAPLONG	2	// flags are one or two characters
#تعریف_کرو AFT_عدد		3	// flags are numbers, comma separated

قسمتعریف ساخت affentry_S affentry_T;
// Affix entry from ".aff" file.  Used for prefixes and suffixes.
ساخت affentry_S
{
    affentry_T	*ae_اگلا;	// next affix with same name/number
    char_u	*ae_chop;	// text to chop off basic word (can be NULL)
    char_u	*ae_جوڑو;	// text to add to basic word (can be NULL)
    char_u	*ae_flags;	// flags on the affix (can be NULL)
    char_u	*ae_cond;	// condition (NULL for ".")
    regprog_T	*ae_prog;	// regexp program for ae_cond or NULL
    حرف	ae_compforbid;	// COMPOUNDFORBIDFLAG found
    حرف	ae_comppermit;	// COMPOUNDPERMITFLAG found
};

#تعریف_کرو AH_کلید_LEN 17		// 2 x 8 bytes + NUL

// Affix header from ".aff" file.  Used for af_pref and af_suff.
قسمتعریف ساخت affheader_S
{
    char_u	ah_کلید[AH_کلید_LEN]; // key for hashtab == name of affix
    غیرعلامتی	ah_flag;	// affix name as number, uses "af_flagtype"
    صحیح		ah_newID;	// prefix ID after renumbering; 0 if not used
    صحیح		ah_combine;	// suffix may combine with prefix
    صحیح		ah_follows;	// another affix block should be following
    affentry_T	*ah_پہلا;	// first affix entry
} affheader_T;

#تعریف_کرو HI2AH(hi)   ((affheader_T *)(hi)->hi_کلید)

// Flag used in compound items.
قسمتعریف ساخت compitem_S
{
    char_u	ci_کلید[AH_کلید_LEN]; // key for hashtab == name of compound
    غیرعلامتی	ci_flag;	// affix name as number, uses "af_flagtype"
    صحیح		ci_newID;	// affix ID after renumbering.
} compitem_T;

#تعریف_کرو HI2CI(hi)   ((compitem_T *)(hi)->hi_کلید)

/*
 * Structure that is used to store the items in the word tree.  This avoids
 * the need to keep track of each allocated thing, everything is freed all at
 * once after ":mkspell" is done.
 * Note: "sb_next" must be just before "sb_data" to make sure the alignment of
 * "sb_data" is correct for systems where pointers must be aligned on
 * pointer-size boundaries and sizeof(pointer) > sizeof(int) (e.g., Sparc).
 */
#تعریف_کرو  SBLOCKSIZE 16000	// size of sb_data
قسمتعریف ساخت sblock_S sblock_T;
ساخت sblock_S
{
    صحیح		sb_مستعمل;	// nr of bytes already in use
    sblock_T	*sb_اگلا;	// next block in list
    char_u	sb_ڈیٹا[1];	// data, actually longer
};

/*
 * A node in the tree.
 */
قسمتعریف ساخت wordnode_S wordnode_T;
ساخت wordnode_S
{
    اتحاد   // shared to save space
    {
	char_u	hashkey[6];	// the hash key, only used while compressing
	صحیح	اشاریہ;		// index in written nodes (valid after first
				// round)
    } wn_u1;
    اتحاد   // shared to save space
    {
	wordnode_T *اگلا;	// next node with same hash key
	wordnode_T *wnode;	// parent node that will write this node
    } wn_u2;
    wordnode_T	*wn_بچہ;	// child (next byte in word)
    wordnode_T  *wn_sibling;	// next sibling (alternate byte in word,
				// always sorted)
    صحیح		wn_refs;	// Nr. of references to this node.  Only
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
    مختصر	wn_region;	// region mask

#اگر_تعریف_شدہ SPELL_PRINTTREE
    صحیح		wn_nr;		// sequence nr for printing
#اگر_ختم
};

#تعریف_کرو WN_نقاب	 0xffff		// mask relevant bits of "wn_flags"

#تعریف_کرو HI2WN(hi)    (wordnode_T *)((hi)->hi_کلید)

/*
 * Info used while reading the spell files.
 */
قسمتعریف ساخت spellinfo_S
{
    wordnode_T	*si_foldroot;	// tree with case-folded words
    طویل	si_foldwcount;	// nr of words in si_foldroot

    wordnode_T	*si_keeproot;	// tree with keep-case words
    طویل	si_keepwcount;	// nr of words in si_keeproot

    wordnode_T	*si_prefroot;	// tree with postponed prefixes

    طویل	si_sugtree;	// creating the soundfolding trie

    sblock_T	*si_بلاکیں;	// memory blocks used
    طویل	si_بلاکیں_cnt;	// memory blocks allocated
    صحیح		si_did_emsg;	// TRUE when ran out of memory

    طویل	si_compress_cnt;    // words to add before lowering
				    // compression limit
    wordnode_T	*si_پہلا_free; // List of nodes that have been freed during
				// compression, linked by "wn_child" field.
    طویل	si_free_گنتی;	// number of nodes in si_first_free
#اگر_تعریف_شدہ SPELL_PRINTTREE
    صحیح		si_wordnode_nr;	// sequence nr for nodes
#اگر_ختم
    buf_T	*si_spellbuf;	// buffer used to store soundfold word table

    صحیح		si_ascii;	// handling only ASCII words
    صحیح		si_جوڑو;		// addition file
    صحیح		si_clear_chartab;   // when TRUE clear char tables
    صحیح		si_region;	// region mask
    vimconv_T	si_conv;	// for conversion to 'encoding'
    صحیح		si_memtot;	// runtime memory used
    صحیح		si_verbose;	// verbose messages
    صحیح		si_پیغام_گنتی;	// number of words added since last message
    char_u	*si_info;	// info text chars or NULL
    صحیح		si_region_گنتی; // number of regions supported (1 when there
				 // are no regions)
    char_u	si_region_name[MAXREGIONS * 2 + 1];
				// region names; used only if
				// si_region_count > 1)

    garray_T	si_rep;		// list of fromto_T entries from REP lines
    garray_T	si_repsal;	// list of fromto_T entries from REPSAL lines
    garray_T	si_sal;		// list of fromto_T entries from SAL lines
    char_u	*si_sofofr;	// SOFOFROM text
    char_u	*si_sofoto;	// SOFOTO text
    صحیح		si_nosugfile;	// NOSUGFILE item found
    صحیح		si_nosplitsugs;	// NOSPLITSUGS item found
    صحیح		si_nocompoundsugs; // NOCOMPOUNDSUGS item found
    صحیح		si_followup;	// soundsalike: ?
    صحیح		si_collapse;	// soundsalike: ?
    hashtab_T	si_commonwords;	// hashtable for common words
    time_t	si_sugtime;	// timestamp for .sug file
    صحیح		si_rem_accents;	// soundsalike: remove accents
    garray_T	si_map;		// MAP info concatenated
    char_u	*si_midword;	// MIDWORD chars or NULL
    صحیح		si_compmax;	// max nr of words for compounding
    صحیح		si_compminlen;	// minimal length for compounding
    صحیح		si_compsylmax;	// max nr of syllables for compounding
    صحیح		si_compoptions;	// COMP_ flags
    garray_T	si_comppat;	// CHECKCOMPOUNDPATTERN items, each stored as
				// a string
    char_u	*si_compflags;	// flags used for compounding
    char_u	si_nobreak;	// NOBREAK
    char_u	*si_syllable;	// syllable string
    garray_T	si_prefcond;	// table with conditions for postponed
				// prefixes, each stored as a string
    صحیح		si_newprefID;	// current value for ah_newID
    صحیح		si_newcompID;	// current value for compound ID
} spellinfo_T;

ساکن صحیح is_aff_قاعدہ(char_u **اشیاء, صحیح itemcnt, حرف *rulename, صحیح	 mincount);
ساکن خالی aff_process_flags(afffile_T *affile, affentry_T *اندراج_2);
ساکن صحیح spell_info_شے(char_u *s);
ساکن غیرعلامتی affitem2flag(صحیح flagtype, char_u *شے, char_u	*fname, صحیح lnum);
ساکن غیرعلامتی get_affitem(صحیح flagtype, char_u **pp);
ساکن خالی process_compflags(spellinfo_T *spin, afffile_T *aff, char_u *compflags);
ساکن خالی check_renumber(spellinfo_T *spin);
ساکن خالی aff_check_نمبر(صحیح spinval, صحیح affval, حرف *name);
ساکن خالی aff_check_string(char_u *spinval, char_u *affval, حرف *name);
ساکن صحیح str_equal(char_u *s1, char_u	*s2);
ساکن خالی جوڑو_fromto(spellinfo_T *spin, garray_T *gap, char_u	*from, char_u *مقصد);
ساکن صحیح sal_مقصد_bool(char_u *s);
ساکن صحیح get_affix_flags(afffile_T *affile, char_u *afflist);
ساکن صحیح get_pfxlist(afffile_T *affile, char_u *afflist, char_u *store_afflist, صحیح *cntp);
ساکن صحیح get_compflags(afffile_T *affile, char_u *afflist, char_u *store_afflist, صحیح *cntp);
ساکن صحیح store_aff_لفظ(spellinfo_T *spin, char_u *لفظ, char_u *afflist, afffile_T *affile, hashtab_T *ht, hashtab_T *xht, صحیح condit, صحیح flags, char_u *pfxlist, صحیح pfxlen);
ساکن خالی *getroom(spellinfo_T *spin, size_t len, صحیح align);
ساکن char_u *getroom_save(spellinfo_T *spin, char_u *s);
ساکن صحیح store_لفظ(spellinfo_T *spin, char_u *لفظ, صحیح flags, صحیح region, char_u *pfxlist, صحیح need_affix);
ساکن صحیح درخت_جوڑو_لفظ(spellinfo_T *spin, char_u *لفظ, wordnode_T *درخت, صحیح flags, صحیح region, صحیح affixID);
ساکن wordnode_T *get_wordnode(spellinfo_T *spin);
ساکن خالی free_wordnode(spellinfo_T *spin, wordnode_T *n);
ساکن خالی wordtree_compress(spellinfo_T *spin, wordnode_T *جڑ, حرف *name);
ساکن طویل گرہ_compress(spellinfo_T *spin, wordnode_T *گرہ, hashtab_T *ht, طویل *tot);
ساکن صحیح گرہ_equal(wordnode_T *n1, wordnode_T *n2);
ساکن خالی clear_گرہ(wordnode_T *گرہ);
ساکن صحیح put_گرہ(FILE *fd, wordnode_T *گرہ, صحیح idx, صحیح regionmask, صحیح prefixtree);
ساکن صحیح sug_filltree(spellinfo_T *spin, slang_T *slang);
ساکن صحیح sug_maketable(spellinfo_T *spin);
ساکن صحیح sug_filltable(spellinfo_T *spin, wordnode_T *گرہ, صحیح startwordnr, garray_T *gap);
ساکن صحیح offset2bytes(صحیح nr, char_u *buf);
ساکن خالی sug_write(spellinfo_T *spin, char_u *fname);
ساکن خالی spell_پیغام(spellinfo_T *spin, char_u *str);
ساکن خالی init_spellfile(خالی);

// In the postponed prefixes tree wn_flags is used to store the WFP_ flags,
// but it must be negative to indicate the prefix tree to tree_add_word().
// Use a negative number with the lower 8 bits zero.
#تعریف_کرو PFX_FLAGS	(-256)

// flags for "condit" argument of store_aff_word()
#تعریف_کرو CONDIT_COMB	1	// affix must combine
#تعریف_کرو CONDIT_CFIX	2	// affix must have CIRCUMFIX flag
#تعریف_کرو CONDIT_SUF	4	// add a suffix for matching flags
#تعریف_کرو CONDIT_AFF	8	// word already has an affix

/*
 * Tunable parameters for when the tree is compressed.  Filled from the
 * 'mkspellmem' option.
 */
ساکن طویل compress_آغاز = 30000;	// memory / SBLOCKSIZE
ساکن طویل compress_inc = 100;		// memory / SBLOCKSIZE
ساکن طویل compress_added = 500000;	// word count

/*
 * Check the 'mkspellmem' option.  Return FAIL if it's wrong.
 * Sets "sps_flags".
 */
    صحیح
spell_check_msm(خالی)
{
    char_u	*p = p_msm;
    طویل	آغاز = 0;
    طویل	incr = 0;
    طویل	added = 0;

    اگر (!VIM_ISDIGIT(*p))
	واپس FAIL;
    // block count = (value * 1024) / SBLOCKSIZE (but avoid overflow)
    آغاز = (getdigits(&p) * 10) / (SBLOCKSIZE / 102);
    اگر (*p != ',')
	واپس FAIL;
    ++p;
    اگر (!VIM_ISDIGIT(*p))
	واپس FAIL;
    incr = (getdigits(&p) * 102) / (SBLOCKSIZE / 10);
    اگر (*p != ',')
	واپس FAIL;
    ++p;
    اگر (!VIM_ISDIGIT(*p))
	واپس FAIL;
    added = getdigits(&p) * 1024;
    اگر (*p != NUL)
	واپس FAIL;

    اگر (آغاز == 0 || incr == 0 || added == 0 || incr > آغاز)
	واپس FAIL;

    compress_آغاز = آغاز;
    compress_inc = incr;
    compress_added = added;
    واپس OK;
}

#اگر_تعریف_شدہ SPELL_PRINTTREE
/*
 * For debugging the tree code: print the current tree in a (more or less)
 * readable format, so that we can see what happens when adding a word and/or
 * compressing the tree.
 * Based on code from Olaf Seibert.
 */
# define PRINTLINESIZE	1000
# define PRINTWIDTH	6

# define PRINTSOME(l, گہرائی, fmt, a1, a2) vim_snprintf(l + گہرائی * PRINTWIDTH, \
	    PRINTLINESIZE - PRINTWIDTH * گہرائی, fmt, a1, a2)

ساکن حرف line1[PRINTLINESIZE];
ساکن حرف line2[PRINTLINESIZE];
ساکن حرف line3[PRINTLINESIZE];

    ساکن خالی
spell_clear_flags(wordnode_T *گرہ)
{
    wordnode_T	*np;

    FOR_ALL_گرہ_SIBLINGS(گرہ, np)
    {
	np->wn_u1.اشاریہ = FALSE;
	spell_clear_flags(np->wn_بچہ);
    }
}

    ساکن خالی
spell_print_گرہ(wordnode_T *گرہ, صحیح گہرائی)
{
    اگر (گرہ->wn_u1.اشاریہ)
    {
	// Done this node before, print the reference.
	PRINTSOME(line1, گہرائی, "(%d)", گرہ->wn_nr, 0);
	PRINTSOME(line2, گہرائی, "    ", 0, 0);
	PRINTSOME(line3, گہرائی, "    ", 0, 0);
	پیغام(line1);
	پیغام(line2);
	پیغام(line3);
    }
    ورنہ
    {
	گرہ->wn_u1.اشاریہ = TRUE;

	اگر (گرہ->wn_byte != NUL)
	{
	    اگر (گرہ->wn_بچہ != تہی)
		PRINTSOME(line1, گہرائی, " %c -> ", گرہ->wn_byte, 0);
	    ورنہ
		// Cannot happen?
		PRINTSOME(line1, گہرائی, " %c ???", گرہ->wn_byte, 0);
	}
	ورنہ
	    PRINTSOME(line1, گہرائی, " $    ", 0, 0);

	PRINTSOME(line2, گہرائی, "%d/%d    ", گرہ->wn_nr, گرہ->wn_refs);

	اگر (گرہ->wn_sibling != تہی)
	    PRINTSOME(line3, گہرائی, " |    ", 0, 0);
	ورنہ
	    PRINTSOME(line3, گہرائی, "      ", 0, 0);

	اگر (گرہ->wn_byte == NUL)
	{
	    پیغام(line1);
	    پیغام(line2);
	    پیغام(line3);
	}

	// do the children
	اگر (گرہ->wn_byte != NUL && گرہ->wn_بچہ != تہی)
	    spell_print_گرہ(گرہ->wn_بچہ, گہرائی + 1);

	// do the siblings
	اگر (گرہ->wn_sibling != تہی)
	{
	    // get rid of all parent details except |
	    STRCPY(line1, line3);
	    STRCPY(line2, line3);
	    spell_print_گرہ(گرہ->wn_sibling, گہرائی);
	}
    }
}

    ساکن خالی
spell_print_درخت(wordnode_T *جڑ)
{
    اگر (جڑ == تہی)
	واپس;

    // Clear the "wn_u1.index" fields, used to remember what has been done.
    spell_clear_flags(جڑ);

    // Recursively print the tree.
    spell_print_گرہ(جڑ, 0);
}
#اگر_ختم // SPELL_PRINTTREE

/*
 * Read the affix file "fname".
 * Returns an afffile_T, NULL for complete failure.
 */
    ساکن afffile_T *
spell_read_aff(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    afffile_T	*aff;
    char_u	rline[MAXLINELEN];
    char_u	*سطر;
    char_u	*pc = تہی;
#تعریف_کرو MAXITEMCNT  30
    char_u	*(اشیاء[MAXITEMCNT]);
    صحیح		itemcnt;
    char_u	*p;
    صحیح		lnum = 0;
    affheader_T	*cur_aff = تہی;
    صحیح		did_postpone_سابقہ = FALSE;
    صحیح		aff_todo = 0;
    hashtab_T	*tp;
    char_u	*کم = تہی;
    char_u	*fol = تہی;
    char_u	*upp = تہی;
    صحیح		do_rep;
    صحیح		do_repsal;
    صحیح		do_sal;
    صحیح		do_mapline;
    صحیح		ملا_map = FALSE;
    hashitem_T	*hi;
    صحیح		l;
    صحیح		compminlen = 0;		// COMPOUNDMIN value
    صحیح		compsylmax = 0;		// COMPOUNDSYLMAX value
    صحیح		compoptions = 0;	// COMP_ flags
    صحیح		compmax = 0;		// COMPOUNDWORDMAX value
    char_u	*compflags = تہی;	// COMPOUNDFLAG and COMPOUNDRULE
					// concatenated
    char_u	*midword = تہی;	// MIDWORD value
    char_u	*syllable = تہی;	// SYLLABLE value
    char_u	*sofofrom = تہی;	// SOFOFROM value
    char_u	*sofoto = تہی;		// SOFOTO value

    /*
     * Open the file.
     */
    fd = mch_fopen((حرف *)fname, "r");
    اگر (fd == تہی)
    {
	semsg(_(e_cant_open_file_str), fname);
	واپس تہی;
    }

    vim_snprintf((حرف *)IObuff, IOSIZE, _("Reading affix file %s..."), fname);
    spell_پیغام(spin, IObuff);

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
    aff = (afffile_T *)getroom(spin, حجم(afffile_T), TRUE);
    اگر (aff == تہی)
    {
	fclose(fd);
	واپس تہی;
    }
    hash_init(&aff->af_pref);
    hash_init(&aff->af_suff);
    hash_init(&aff->af_comp);

    /*
     * Read all the lines in the file one by one.
     */
    جب (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	سطر_breakcheck();
	++lnum;

	// Skip comment lines.
	اگر (*rline == '#')
	    جاری;

	// Convert from "SET" to 'encoding' when needed.
	vim_free(pc);
	اگر (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, تہی);
	    اگر (pc == تہی)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
							   fname, lnum, rline);
		جاری;
	    }
	    سطر = pc;
	}
	ورنہ
	{
	    pc = تہی;
	    سطر = rline;
	}

	// Split the line up in white separated items.  Put a NUL after each
	// item.
	itemcnt = 0;
	برائے (p = سطر; ; )
	{
	    جب (*p != NUL && *p <= ' ')  // skip white space and CR/NL
		++p;
	    اگر (*p == NUL)
		توڑو;
	    اگر (itemcnt == MAXITEMCNT)	    // too many items
		توڑو;
	    اشیاء[itemcnt++] = p;
	    // A few items have arbitrary text argument, don't split them.
	    اگر (itemcnt == 2 && spell_info_شے(اشیاء[0]))
		جب (*p >= ' ' || *p == TAB)    // skip until CR/NL
		    ++p;
	    ورنہ
		جب (*p > ' ')    // skip until white space or CR/NL
		    ++p;
	    اگر (*p == NUL)
		توڑو;
	    *p++ = NUL;
	}

	// Handle non-empty lines.
	اگر (itemcnt > 0)
	{
	    اگر (is_aff_قاعدہ(اشیاء, itemcnt, "SET", 2) && aff->af_enc == تہی)
	    {
		// Setup for conversion from "ENC" to 'encoding'.
		aff->af_enc = enc_canonize(اشیاء[1]);
		اگر (aff->af_enc != تہی && !spin->si_ascii
			&& convert_تیاری(&spin->si_conv, aff->af_enc,
							       p_enc) == FAIL)
		    smsg(_("Conversion in %s not supported: from %s to %s"),
					       fname, aff->af_enc, p_enc);
		spin->si_conv.vc_fail = TRUE;
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "FLAG", 2)
					      && aff->af_flagtype == AFT_CHAR)
	    {
		اگر (STRCMP(اشیاء[1], "long") == 0)
		    aff->af_flagtype = AFT_LONG;
		ورنہ اگر (STRCMP(اشیاء[1], "num") == 0)
		    aff->af_flagtype = AFT_عدد;
		ورنہ اگر (STRCMP(اشیاء[1], "caplong") == 0)
		    aff->af_flagtype = AFT_CAPLONG;
		ورنہ
		    smsg(_("Invalid value for FLAG in %s line %d: %s"),
			    fname, lnum, اشیاء[1]);
		اگر (aff->af_rare != 0
			|| aff->af_keepcase != 0
			|| aff->af_bad != 0
			|| aff->af_needaffix != 0
			|| aff->af_circumfix != 0
			|| aff->af_needcomp != 0
			|| aff->af_comproot != 0
			|| aff->af_nosuggest != 0
			|| compflags != تہی
			|| aff->af_suff.ht_مستعمل > 0
			|| aff->af_pref.ht_مستعمل > 0)
		    smsg(_("FLAG after using flags in %s line %d: %s"),
			    fname, lnum, اشیاء[1]);
	    }
	    ورنہ اگر (spell_info_شے(اشیاء[0]))
	    {
		    p = (char_u *)getroom(spin,
			    (spin->si_info == تہی ? 0 : STRLEN(spin->si_info))
			    + STRLEN(اشیاء[0])
			    + STRLEN(اشیاء[1]) + 3, FALSE);
		    اگر (p != تہی)
		    {
			اگر (spin->si_info != تہی)
			{
			    STRCPY(p, spin->si_info);
			    STRCAT(p, "\n");
			}
			STRCAT(p, اشیاء[0]);
			STRCAT(p, " ");
			STRCAT(p, اشیاء[1]);
			spin->si_info = p;
		    }
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "MIDWORD", 2)
							   && midword == تہی)
	    {
		midword = getroom_save(spin, اشیاء[1]);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "TRY", 2))
	    {
		// ignored, we look in the tree for what chars may appear
	    }
	    // TODO: remove "RAR" later
	    ورنہ اگر ((is_aff_قاعدہ(اشیاء, itemcnt, "RAR", 2)
			|| is_aff_قاعدہ(اشیاء, itemcnt, "RARE", 2))
							 && aff->af_rare == 0)
	    {
		aff->af_rare = affitem2flag(aff->af_flagtype, اشیاء[1],
								 fname, lnum);
	    }
	    // TODO: remove "KEP" later
	    ورنہ اگر ((is_aff_قاعدہ(اشیاء, itemcnt, "KEP", 2)
			|| is_aff_قاعدہ(اشیاء, itemcnt, "KEEPCASE", 2))
						     && aff->af_keepcase == 0)
	    {
		aff->af_keepcase = affitem2flag(aff->af_flagtype, اشیاء[1],
								 fname, lnum);
	    }
	    ورنہ اگر ((is_aff_قاعدہ(اشیاء, itemcnt, "BAD", 2)
			|| is_aff_قاعدہ(اشیاء, itemcnt, "FORBIDDENWORD", 2))
							  && aff->af_bad == 0)
	    {
		aff->af_bad = affitem2flag(aff->af_flagtype, اشیاء[1],
								 fname, lnum);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "NEEDAFFIX", 2)
						    && aff->af_needaffix == 0)
	    {
		aff->af_needaffix = affitem2flag(aff->af_flagtype, اشیاء[1],
								 fname, lnum);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "CIRCUMFIX", 2)
						    && aff->af_circumfix == 0)
	    {
		aff->af_circumfix = affitem2flag(aff->af_flagtype, اشیاء[1],
								 fname, lnum);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "NOSUGGEST", 2)
						    && aff->af_nosuggest == 0)
	    {
		aff->af_nosuggest = affitem2flag(aff->af_flagtype, اشیاء[1],
								 fname, lnum);
	    }
	    ورنہ اگر ((is_aff_قاعدہ(اشیاء, itemcnt, "NEEDCOMPOUND", 2)
			|| is_aff_قاعدہ(اشیاء, itemcnt, "ONLYINCOMPOUND", 2))
						     && aff->af_needcomp == 0)
	    {
		aff->af_needcomp = affitem2flag(aff->af_flagtype, اشیاء[1],
								 fname, lnum);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "COMPOUNDROOT", 2)
						     && aff->af_comproot == 0)
	    {
		aff->af_comproot = affitem2flag(aff->af_flagtype, اشیاء[1],
								 fname, lnum);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "COMPOUNDFORBIDFLAG", 2)
						   && aff->af_compforbid == 0)
	    {
		aff->af_compforbid = affitem2flag(aff->af_flagtype, اشیاء[1],
								 fname, lnum);
		اگر (aff->af_pref.ht_مستعمل > 0)
		    smsg(_("Defining COMPOUNDFORBIDFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "COMPOUNDPERMITFLAG", 2)
						   && aff->af_comppermit == 0)
	    {
		aff->af_comppermit = affitem2flag(aff->af_flagtype, اشیاء[1],
								 fname, lnum);
		اگر (aff->af_pref.ht_مستعمل > 0)
		    smsg(_("Defining COMPOUNDPERMITFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "COMPOUNDFLAG", 2)
							 && compflags == تہی)
	    {
		// Turn flag "c" into COMPOUNDRULE compatible string "c+",
		// "Na" into "Na+", "1234" into "1234+".
		p = getroom(spin, STRLEN(اشیاء[1]) + 2, FALSE);
		اگر (p != تہی)
		{
		    STRCPY(p, اشیاء[1]);
		    STRCAT(p, "+");
		    compflags = p;
		}
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "COMPOUNDRULES", 2))
	    {
		// We don't use the count, but do check that it's a number and
		// not COMPOUNDRULE mistyped.
		اگر (atoi((حرف *)اشیاء[1]) == 0)
		    smsg(_("Wrong COMPOUNDRULES value in %s line %d: %s"),
						       fname, lnum, اشیاء[1]);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "COMPOUNDRULE", 2))
	    {
		// Don't use the first rule if it is a number.
		اگر (compflags != تہی || *skipdigits(اشیاء[1]) != NUL)
		{
		    // Concatenate this string to previously defined ones,
		    // using a slash to separate them.
		    l = (صحیح)STRLEN(اشیاء[1]) + 1;
		    اگر (compflags != تہی)
			l += (صحیح)STRLEN(compflags) + 1;
		    p = getroom(spin, l, FALSE);
		    اگر (p != تہی)
		    {
			اگر (compflags != تہی)
			{
			    STRCPY(p, compflags);
			    STRCAT(p, "/");
			}
			STRCAT(p, اشیاء[1]);
			compflags = p;
		    }
		}
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "COMPOUNDWORDMAX", 2)
							      && compmax == 0)
	    {
		compmax = atoi((حرف *)اشیاء[1]);
		اگر (compmax == 0)
		    smsg(_("Wrong COMPOUNDWORDMAX value in %s line %d: %s"),
						       fname, lnum, اشیاء[1]);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "COMPOUNDMIN", 2)
							   && compminlen == 0)
	    {
		compminlen = atoi((حرف *)اشیاء[1]);
		اگر (compminlen == 0)
		    smsg(_("Wrong COMPOUNDMIN value in %s line %d: %s"),
						       fname, lnum, اشیاء[1]);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "COMPOUNDSYLMAX", 2)
							   && compsylmax == 0)
	    {
		compsylmax = atoi((حرف *)اشیاء[1]);
		اگر (compsylmax == 0)
		    smsg(_("Wrong COMPOUNDSYLMAX value in %s line %d: %s"),
						       fname, lnum, اشیاء[1]);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "CHECKCOMPOUNDDUP", 1))
	    {
		compoptions |= COMP_CHECKDUP;
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "CHECKCOMPOUNDREP", 1))
	    {
		compoptions |= COMP_CHECKREP;
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "CHECKCOMPOUNDCASE", 1))
	    {
		compoptions |= COMP_CHECKCASE;
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "CHECKCOMPOUNDTRIPLE", 1))
	    {
		compoptions |= COMP_CHECKTRIPLE;
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "CHECKCOMPOUNDPATTERN", 2))
	    {
		اگر (atoi((حرف *)اشیاء[1]) == 0)
		    smsg(_("Wrong CHECKCOMPOUNDPATTERN value in %s line %d: %s"),
						       fname, lnum, اشیاء[1]);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "CHECKCOMPOUNDPATTERN", 3))
	    {
		garray_T    *gap = &spin->si_comppat;
		صحیح	    i;

		// Only add the couple if it isn't already there.
		برائے (i = 0; i < gap->ga_len - 1; i += 2)
		    اگر (STRCMP(((char_u **)(gap->ga_ڈیٹا))[i], اشیاء[1]) == 0
			    && STRCMP(((char_u **)(gap->ga_ڈیٹا))[i + 1],
							       اشیاء[2]) == 0)
			توڑو;
		اگر (i >= gap->ga_len && ga_grow(gap, 2) == OK)
		{
		    ((char_u **)(gap->ga_ڈیٹا))[gap->ga_len++]
					       = getroom_save(spin, اشیاء[1]);
		    ((char_u **)(gap->ga_ڈیٹا))[gap->ga_len++]
					       = getroom_save(spin, اشیاء[2]);
		}
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "SYLLABLE", 2)
							  && syllable == تہی)
	    {
		syllable = getroom_save(spin, اشیاء[1]);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "NOBREAK", 1))
	    {
		spin->si_nobreak = TRUE;
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "NOSPLITSUGS", 1))
	    {
		spin->si_nosplitsugs = TRUE;
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "NOCOMPOUNDSUGS", 1))
	    {
		spin->si_nocompoundsugs = TRUE;
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "NOSUGFILE", 1))
	    {
		spin->si_nosugfile = TRUE;
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "PFXPOSTPONE", 1))
	    {
		aff->af_pfxpostpone = TRUE;
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "IGNOREEXTRA", 1))
	    {
		aff->af_ignoreextra = TRUE;
	    }
	    ورنہ اگر ((STRCMP(اشیاء[0], "PFX") == 0
					      || STRCMP(اشیاء[0], "SFX") == 0)
		    && aff_todo == 0
		    && itemcnt >= 4)
	    {
		صحیح	lasti = 4;
		char_u	کلید[AH_کلید_LEN];

		اگر (*اشیاء[0] == 'P')
		    tp = &aff->af_pref;
		ورنہ
		    tp = &aff->af_suff;

		// Myspell allows the same affix name to be used multiple
		// times.  The affix files that do this have an undocumented
		// "S" flag on all but the last block, thus we check for that
		// and store it in ah_follows.
		vim_strncpy(کلید, اشیاء[1], AH_کلید_LEN - 1);
		hi = hash_ڈھونڈو(tp, کلید);
		اگر (!HASHITEM_خالی_خانے(hi))
		{
		    cur_aff = HI2AH(hi);
		    اگر (cur_aff->ah_combine != (*اشیاء[2] == 'Y'))
			smsg(_("Different combining flag in continued affix block in %s line %d: %s"),
						   fname, lnum, اشیاء[1]);
		    اگر (!cur_aff->ah_follows)
			smsg(_("Duplicate affix in %s line %d: %s"),
						       fname, lnum, اشیاء[1]);
		}
		ورنہ
		{
		    // New affix letter.
		    cur_aff = (affheader_T *)getroom(spin,
						   حجم(affheader_T), TRUE);
		    اگر (cur_aff == تہی)
			توڑو;
		    cur_aff->ah_flag = affitem2flag(aff->af_flagtype, اشیاء[1],
								 fname, lnum);
		    اگر (cur_aff->ah_flag == 0 || STRLEN(اشیاء[1]) >= AH_کلید_LEN)
			توڑو;
		    اگر (cur_aff->ah_flag == aff->af_bad
			    || cur_aff->ah_flag == aff->af_rare
			    || cur_aff->ah_flag == aff->af_keepcase
			    || cur_aff->ah_flag == aff->af_needaffix
			    || cur_aff->ah_flag == aff->af_circumfix
			    || cur_aff->ah_flag == aff->af_nosuggest
			    || cur_aff->ah_flag == aff->af_needcomp
			    || cur_aff->ah_flag == aff->af_comproot)
			smsg(_("Affix also used for BAD/RARE/KEEPCASE/NEEDAFFIX/NEEDCOMPOUND/NOSUGGEST in %s line %d: %s"),
						       fname, lnum, اشیاء[1]);
		    STRCPY(cur_aff->ah_کلید, اشیاء[1]);
		    hash_جوڑو(tp, cur_aff->ah_کلید, "spelling");

		    cur_aff->ah_combine = (*اشیاء[2] == 'Y');
		}

		// Check for the "S" flag, which apparently means that another
		// block with the same affix name is following.
		اگر (itemcnt > lasti && STRCMP(اشیاء[lasti], "S") == 0)
		{
		    ++lasti;
		    cur_aff->ah_follows = TRUE;
		}
		ورنہ
		    cur_aff->ah_follows = FALSE;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter.
		اگر (itemcnt > lasti && *اشیاء[lasti] != '#')
		    smsg(_(e_afftrailing), fname, lnum, اشیاء[lasti]);

		اگر (STRCMP(اشیاء[2], "Y") != 0 && STRCMP(اشیاء[2], "N") != 0)
		    smsg(_("Expected Y or N in %s line %d: %s"),
						       fname, lnum, اشیاء[2]);

		اگر (*اشیاء[0] == 'P' && aff->af_pfxpostpone)
		{
		    اگر (cur_aff->ah_newID == 0)
		    {
			// Use a new number in the .spl file later, to be able
			// to handle multiple .aff files.
			check_renumber(spin);
			cur_aff->ah_newID = ++spin->si_newprefID;

			// We only really use ah_newID if the prefix is
			// postponed.  We know that only after handling all
			// the items.
			did_postpone_سابقہ = FALSE;
		    }
		    ورنہ
			// Did use the ID in a previous block.
			did_postpone_سابقہ = TRUE;
		}

		aff_todo = atoi((حرف *)اشیاء[3]);
	    }
	    ورنہ اگر ((STRCMP(اشیاء[0], "PFX") == 0
					      || STRCMP(اشیاء[0], "SFX") == 0)
		    && aff_todo > 0
		    && STRCMP(cur_aff->ah_کلید, اشیاء[1]) == 0
		    && itemcnt >= 5)
	    {
		affentry_T	*aff_اندراج;
		صحیح		بالائی_2 = FALSE;
		صحیح		lasti = 5;

		// Myspell allows extra text after the item, but that might
		// mean mistakes go unnoticed.  Require a comment-starter,
		// unless IGNOREEXTRA is used.  Hunspell uses a "-" item.
		اگر (itemcnt > lasti
			&& !aff->af_ignoreextra
			&& *اشیاء[lasti] != '#'
			&& (STRCMP(اشیاء[lasti], "-") != 0
						     || itemcnt != lasti + 1))
		    smsg(_(e_afftrailing), fname, lnum, اشیاء[lasti]);

		// New item for an affix letter.
		--aff_todo;
		aff_اندراج = (affentry_T *)getroom(spin,
						    حجم(affentry_T), TRUE);
		اگر (aff_اندراج == تہی)
		    توڑو;

		اگر (STRCMP(اشیاء[2], "0") != 0)
		    aff_اندراج->ae_chop = getroom_save(spin, اشیاء[2]);
		اگر (STRCMP(اشیاء[3], "0") != 0)
		{
		    aff_اندراج->ae_جوڑو = getroom_save(spin, اشیاء[3]);

		    // Recognize flags on the affix: abcd/XYZ
		    aff_اندراج->ae_flags = vim_strchr(aff_اندراج->ae_جوڑو, '/');
		    اگر (aff_اندراج->ae_flags != تہی)
		    {
			*aff_اندراج->ae_flags++ = NUL;
			aff_process_flags(aff, aff_اندراج);
		    }
		}

		// Don't use an affix entry with non-ASCII characters when
		// "spin->si_ascii" is TRUE.
		اگر (!spin->si_ascii || !(has_non_ascii(aff_اندراج->ae_chop)
					  || has_non_ascii(aff_اندراج->ae_جوڑو)))
		{
		    aff_اندراج->ae_اگلا = cur_aff->ah_پہلا;
		    cur_aff->ah_پہلا = aff_اندراج;

		    اگر (STRCMP(اشیاء[4], ".") != 0)
		    {
			char_u	buf[MAXLINELEN];

			aff_اندراج->ae_cond = getroom_save(spin, اشیاء[4]);
			// Note: this silently truncates the buffer, but this should
			// not happen in practice
			اگر (*اشیاء[0] == 'P')
			    vim_snprintf((حرف *)buf, حجم(buf), "^%s", اشیاء[4]);
			ورنہ
			    vim_snprintf((حرف *)buf, حجم(buf), "%s$", اشیاء[4]);
			aff_اندراج->ae_prog = vim_regcomp(buf,
					    RE_MAGIC + RE_STRING + RE_STRICT);
			اگر (aff_اندراج->ae_prog == تہی)
			    smsg(_("Broken condition in %s line %d: %s"),
						       fname, lnum, اشیاء[4]);
		    }

		    // For postponed prefixes we need an entry in si_prefcond
		    // for the condition.  Use an existing one if possible.
		    // Can't be done for an affix with flags, ignoring
		    // COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG.
		    اگر (*اشیاء[0] == 'P' && aff->af_pfxpostpone
					       && aff_اندراج->ae_flags == تہی)
		    {
			// When the chop string is one lower-case letter and
			// the add string ends in the upper-case letter we set
			// the "upper" flag, clear "ae_chop" and remove the
			// letters from "ae_add".  The condition must either
			// be empty or start with the same letter.
			اگر (aff_اندراج->ae_chop != تہی
				&& aff_اندراج->ae_جوڑو != تہی
				&& aff_اندراج->ae_chop[(*mb_ptr2len)(
						   aff_اندراج->ae_chop)] == NUL)
			{
			    صحیح		c, c_اوپر;

			    c = PTR2CHAR(aff_اندراج->ae_chop);
			    c_اوپر = SPELL_TOUPPER(c);
			    اگر (c_اوپر != c
				    && (aff_اندراج->ae_cond == تہی
					|| PTR2CHAR(aff_اندراج->ae_cond) == c))
			    {
				p = aff_اندراج->ae_جوڑو
						  + STRLEN(aff_اندراج->ae_جوڑو);
				MB_PTR_عقب(aff_اندراج->ae_جوڑو, p);
				اگر (PTR2CHAR(p) == c_اوپر)
				{
				    بالائی_2 = TRUE;
				    aff_اندراج->ae_chop = تہی;
				    *p = NUL;

				    // The condition is matched with the
				    // actual word, thus must check for the
				    // upper-case letter.
				    اگر (aff_اندراج->ae_cond != تہی)
				    {
					char_u	buf[MAXLINELEN];

					اگر (has_mbyte)
					{
					    onecap_copy(اشیاء[4], buf, TRUE);
					    aff_اندراج->ae_cond = getroom_save(
								   spin, buf);
					}
					ورنہ
					    *aff_اندراج->ae_cond = c_اوپر;
					اگر (aff_اندراج->ae_cond != تہی)
					{
					    sprintf((حرف *)buf, "^%s",
							  aff_اندراج->ae_cond);
					    vim_regfree(aff_اندراج->ae_prog);
					    aff_اندراج->ae_prog = vim_regcomp(
						    buf, RE_MAGIC + RE_STRING);
					}
				    }
				}
			    }
			}

			اگر (aff_اندراج->ae_chop == تہی
					       && aff_اندراج->ae_flags == تہی)
			{
			    صحیح		idx;
			    char_u	**pp;
			    صحیح		n;

			    // Find a previously used condition.
			    برائے (idx = spin->si_prefcond.ga_len - 1; idx >= 0;
									--idx)
			    {
				p = ((char_u **)spin->si_prefcond.ga_ڈیٹا)[idx];
				اگر (str_equal(p, aff_اندراج->ae_cond))
				    توڑو;
			    }
			    اگر (idx < 0 && ga_grow(&spin->si_prefcond, 1) == OK)
			    {
				// Not found, add a new condition.
				idx = spin->si_prefcond.ga_len++;
				pp = ((char_u **)spin->si_prefcond.ga_ڈیٹا)
									+ idx;
				اگر (aff_اندراج->ae_cond == تہی)
				    *pp = تہی;
				ورنہ
				    *pp = getroom_save(spin,
							  aff_اندراج->ae_cond);
			    }

			    // Add the prefix to the prefix tree.
			    اگر (aff_اندراج->ae_جوڑو == تہی)
				p = (char_u *)"";
			    ورنہ
				p = aff_اندراج->ae_جوڑو;

			    // PFX_FLAGS is a negative number, so that
			    // tree_add_word() knows this is the prefix tree.
			    n = PFX_FLAGS;
			    اگر (!cur_aff->ah_combine)
				n |= WFP_NC;
			    اگر (بالائی_2)
				n |= WFP_اوپر;
			    اگر (aff_اندراج->ae_comppermit)
				n |= WFP_COMPPERMIT;
			    اگر (aff_اندراج->ae_compforbid)
				n |= WFP_COMPFORBID;
			    درخت_جوڑو_لفظ(spin, p, spin->si_prefroot, n,
						      idx, cur_aff->ah_newID);
			    did_postpone_سابقہ = TRUE;
			}

			// Didn't actually use ah_newID, backup si_newprefID.
			اگر (aff_todo == 0 && !did_postpone_سابقہ)
			{
			    --spin->si_newprefID;
			    cur_aff->ah_newID = 0;
			}
		    }
		}
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "FOL", 2) && fol == تہی)
	    {
		fol = vim_strsave(اشیاء[1]);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "LOW", 2) && کم == تہی)
	    {
		کم = vim_strsave(اشیاء[1]);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "UPP", 2) && upp == تہی)
	    {
		upp = vim_strsave(اشیاء[1]);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "REP", 2)
		     || is_aff_قاعدہ(اشیاء, itemcnt, "REPSAL", 2))
	    {
		// Ignore REP/REPSAL count
		اگر (!محفوظ_isdigit(*اشیاء[1]))
		    smsg(_("Expected REP(SAL) count in %s line %d"),
								 fname, lnum);
	    }
	    ورنہ اگر ((STRCMP(اشیاء[0], "REP") == 0
			|| STRCMP(اشیاء[0], "REPSAL") == 0)
		    && itemcnt >= 3)
	    {
		// REP/REPSAL item
		// Myspell ignores extra arguments, we require it starts with
		// # to detect mistakes.
		اگر (itemcnt > 3 && اشیاء[3][0] != '#')
		    smsg(_(e_afftrailing), fname, lnum, اشیاء[3]);
		اگر (اشیاء[0][3] == 'S' ? do_repsal : do_rep)
		{
		    // Replace underscore with space (can't include a space
		    // directly).
		    برائے (p = اشیاء[1]; *p != NUL; MB_PTR_ADV(p))
			اگر (*p == '_')
			    *p = ' ';
		    برائے (p = اشیاء[2]; *p != NUL; MB_PTR_ADV(p))
			اگر (*p == '_')
			    *p = ' ';
		    جوڑو_fromto(spin, اشیاء[0][3] == 'S'
					 ? &spin->si_repsal
					 : &spin->si_rep, اشیاء[1], اشیاء[2]);
		}
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "MAP", 2))
	    {
		// MAP item or count
		اگر (!ملا_map)
		{
		    // First line contains the count.
		    ملا_map = TRUE;
		    اگر (!محفوظ_isdigit(*اشیاء[1]))
			smsg(_("Expected MAP count in %s line %d"),
								 fname, lnum);
		}
		ورنہ اگر (do_mapline)
		{
		    صحیح		c;

		    // Check that every character appears only once.
		    برائے (p = اشیاء[1]; *p != NUL; )
		    {
			c = mb_ptr2char_adv(&p);
			اگر ((spin->si_map.ga_len > 0
				    && vim_strchr(spin->si_map.ga_ڈیٹا, c)
								      != تہی)
				|| vim_strchr(p, c) != تہی)
			    smsg(_("Duplicate character in MAP in %s line %d"),
								 fname, lnum);
		    }

		    // We simply concatenate all the MAP strings, separated by
		    // slashes.
		    ga_concat(&spin->si_map, اشیاء[1]);
		    ga_append(&spin->si_map, '/');
		}
	    }
	    // Accept "SAL from to" and "SAL from to  #comment".
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "SAL", 3))
	    {
		اگر (do_sal)
		{
		    // SAL item (sounds-a-like)
		    // Either one of the known keys or a from-to pair.
		    اگر (STRCMP(اشیاء[1], "followup") == 0)
			spin->si_followup = sal_مقصد_bool(اشیاء[2]);
		    ورنہ اگر (STRCMP(اشیاء[1], "collapse_result") == 0)
			spin->si_collapse = sal_مقصد_bool(اشیاء[2]);
		    ورنہ اگر (STRCMP(اشیاء[1], "remove_accents") == 0)
			spin->si_rem_accents = sal_مقصد_bool(اشیاء[2]);
		    ورنہ
			// when "to" is "_" it means empty
			جوڑو_fromto(spin, &spin->si_sal, اشیاء[1],
				     STRCMP(اشیاء[2], "_") == 0 ? (char_u *)""
								: اشیاء[2]);
		}
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "SOFOFROM", 2)
							  && sofofrom == تہی)
	    {
		sofofrom = getroom_save(spin, اشیاء[1]);
	    }
	    ورنہ اگر (is_aff_قاعدہ(اشیاء, itemcnt, "SOFOTO", 2)
							    && sofoto == تہی)
	    {
		sofoto = getroom_save(spin, اشیاء[1]);
	    }
	    ورنہ اگر (STRCMP(اشیاء[0], "COMMON") == 0)
	    {
		صحیح	i;

		برائے (i = 1; i < itemcnt; ++i)
		{
		    اگر (HASHITEM_خالی_خانے(hash_ڈھونڈو(&spin->si_commonwords,
								   اشیاء[i])))
		    {
			p = vim_strsave(اشیاء[i]);
			اگر (p == تہی)
			    توڑو;
			hash_جوڑو(&spin->si_commonwords, p, "spelling");
		    }
		}
	    }
	    ورنہ
		smsg(_("Unrecognized or duplicate item in %s line %d: %s"),
						       fname, lnum, اشیاء[0]);
	}
    }

    اگر (fol != تہی || کم != تہی || upp != تہی)
    {
	اگر (spin->si_clear_chartab)
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
	اگر (!spin->si_ascii && !enc_utf8)
	{
	    اگر (fol == تہی || کم == تہی || upp == تہی)
		smsg(_("Missing FOL/LOW/UPP line in %s"), fname);
	    ورنہ
		(خالی)سیٹ_spell_chartab(fol, کم, upp);
	}

	vim_free(fol);
	vim_free(کم);
	vim_free(upp);
    }

    // Use compound specifications of the .aff file for the spell info.
    اگر (compmax != 0)
    {
	aff_check_نمبر(spin->si_compmax, compmax, "COMPOUNDWORDMAX");
	spin->si_compmax = compmax;
    }

    اگر (compminlen != 0)
    {
	aff_check_نمبر(spin->si_compminlen, compminlen, "COMPOUNDMIN");
	spin->si_compminlen = compminlen;
    }

    اگر (compsylmax != 0)
    {
	اگر (syllable == تہی)
	    smsg(_("COMPOUNDSYLMAX used without SYLLABLE"));
	aff_check_نمبر(spin->si_compsylmax, compsylmax, "COMPOUNDSYLMAX");
	spin->si_compsylmax = compsylmax;
    }

    اگر (compoptions != 0)
    {
	aff_check_نمبر(spin->si_compoptions, compoptions, "COMPOUND options");
	spin->si_compoptions |= compoptions;
    }

    اگر (compflags != تہی)
	process_compflags(spin, aff, compflags);

    // Check that we didn't use too many renumbered flags.
    اگر (spin->si_newcompID < spin->si_newprefID)
    {
	اگر (spin->si_newcompID == 127 || spin->si_newcompID == 255)
	    پیغام(_("Too many postponed prefixes"));
	ورنہ اگر (spin->si_newprefID == 0 || spin->si_newprefID == 127)
	    پیغام(_("Too many compound flags"));
	ورنہ
	    پیغام(_("Too many postponed prefixes and/or compound flags"));
    }

    اگر (syllable != تہی)
    {
	aff_check_string(spin->si_syllable, syllable, "SYLLABLE");
	spin->si_syllable = syllable;
    }

    اگر (sofofrom != تہی || sofoto != تہی)
    {
	اگر (sofofrom == تہی || sofoto == تہی)
	    smsg(_("Missing SOFO%s line in %s"),
				     sofofrom == تہی ? "FROM" : "TO", fname);
	ورنہ اگر (spin->si_sal.ga_len > 0)
	    smsg(_("Both SAL and SOFO lines in %s"), fname);
	ورنہ
	{
	    aff_check_string(spin->si_sofofr, sofofrom, "SOFOFROM");
	    aff_check_string(spin->si_sofoto, sofoto, "SOFOTO");
	    spin->si_sofofr = sofofrom;
	    spin->si_sofoto = sofoto;
	}
    }

    اگر (midword != تہی)
    {
	aff_check_string(spin->si_midword, midword, "MIDWORD");
	spin->si_midword = midword;
    }

    vim_free(pc);
    fclose(fd);
    واپس aff;
}

/*
 * Return TRUE when items[0] equals "rulename", there are "mincount" items or
 * a comment is following after item "mincount".
 */
    ساکن صحیح
is_aff_قاعدہ(
    char_u	**اشیاء,
    صحیح		itemcnt,
    حرف	*rulename,
    صحیح		mincount)
{
    واپس (STRCMP(اشیاء[0], rulename) == 0
	    && (itemcnt == mincount
		|| (itemcnt > mincount && اشیاء[mincount][0] == '#')));
}

/*
 * For affix "entry" move COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG from
 * ae_flags to ae_comppermit and ae_compforbid.
 */
    ساکن خالی
aff_process_flags(afffile_T *affile, affentry_T *اندراج_2)
{
    char_u	*p;
    char_u	*prevp;
    غیرعلامتی	flag;

    اگر (اندراج_2->ae_flags != تہی
		&& (affile->af_compforbid != 0 || affile->af_comppermit != 0))
    {
	برائے (p = اندراج_2->ae_flags; *p != NUL; )
	{
	    prevp = p;
	    flag = get_affitem(affile->af_flagtype, &p);
	    اگر (flag == affile->af_comppermit || flag == affile->af_compforbid)
	    {
		STRMOVE(prevp, p);
		p = prevp;
		اگر (flag == affile->af_comppermit)
		    اندراج_2->ae_comppermit = TRUE;
		ورنہ
		    اندراج_2->ae_compforbid = TRUE;
	    }
	    اگر (affile->af_flagtype == AFT_عدد && *p == ',')
		++p;
	}
	اگر (*اندراج_2->ae_flags == NUL)
	    اندراج_2->ae_flags = تہی;	// nothing left
    }
}

/*
 * Return TRUE if "s" is the name of an info item in the affix file.
 */
    ساکن صحیح
spell_info_شے(char_u *s)
{
    واپس STRCMP(s, "NAME") == 0
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
    ساکن غیرعلامتی
affitem2flag(
    صحیح		flagtype,
    char_u	*شے,
    char_u	*fname,
    صحیح		lnum)
{
    غیرعلامتی	res;
    char_u	*p = شے;

    res = get_affitem(flagtype, &p);
    اگر (res == 0)
    {
	اگر (flagtype == AFT_عدد)
	    smsg(_("Flag is not a number in %s line %d: %s"),
							   fname, lnum, شے);
	ورنہ
	    smsg(_("Illegal flag in %s line %d: %s"),
							   fname, lnum, شے);
    }
    اگر (*p != NUL)
    {
	smsg(_(e_affname), fname, lnum, شے);
	واپس 0;
    }

    واپس res;
}

/*
 * Get one affix name from "*pp" and advance the pointer.
 * Returns ZERO_FLAG for "0".
 * Returns zero for an error, still advances the pointer then.
 */
    ساکن غیرعلامتی
get_affitem(صحیح flagtype, char_u **pp)
{
    صحیح		res;

    اگر (flagtype == AFT_عدد)
    {
	اگر (!VIM_ISDIGIT(**pp))
	{
	    ++*pp;	// always advance, avoid getting stuck
	    واپس 0;
	}
	res = getdigits(pp);
	اگر (res == 0)
	    res = ZERO_FLAG;
    }
    ورنہ
    {
	res = mb_ptr2char_adv(pp);
	اگر (flagtype == AFT_LONG || (flagtype == AFT_CAPLONG
						 && res >= 'A' && res <= 'Z'))
	{
	    اگر (**pp == NUL)
		واپس 0;
	    res = mb_ptr2char_adv(pp) + (res << 16);
	}
    }
    واپس res;
}

/*
 * Process the "compflags" string used in an affix file and append it to
 * spin->si_compflags.
 * The processing involves changing the affix names to ID numbers, so that
 * they fit in one byte.
 */
    ساکن خالی
process_compflags(
    spellinfo_T	*spin,
    afffile_T	*aff,
    char_u	*compflags)
{
    char_u	*p;
    char_u	*prevp;
    غیرعلامتی	flag;
    compitem_T	*ci;
    صحیح		id;
    صحیح		len;
    char_u	*tp;
    char_u	کلید[AH_کلید_LEN];
    hashitem_T	*hi;

    // Make room for the old and the new compflags, concatenated with a / in
    // between.  Processing it makes it shorter, but we don't know by how
    // much, thus allocate the maximum.
    len = (صحیح)STRLEN(compflags) + 1;
    اگر (spin->si_compflags != تہی)
	len += (صحیح)STRLEN(spin->si_compflags) + 1;
    p = getroom(spin, len, FALSE);
    اگر (p == تہی)
	واپس;
    اگر (spin->si_compflags != تہی)
    {
	STRCPY(p, spin->si_compflags);
	STRCAT(p, "/");
    }
    spin->si_compflags = p;
    tp = p + STRLEN(p);

    برائے (p = compflags; *p != NUL; )
    {
	اگر (vim_strchr((char_u *)"/?*+[]", *p) != تہی)
	    // Copy non-flag characters directly.
	    *tp++ = *p++;
	ورنہ
	{
	    // First get the flag number, also checks validity.
	    prevp = p;
	    flag = get_affitem(aff->af_flagtype, &p);
	    اگر (flag != 0)
	    {
		// Find the flag in the hashtable.  If it was used before, use
		// the existing ID.  Otherwise add a new entry.
		vim_strncpy(کلید, prevp, p - prevp);
		hi = hash_ڈھونڈو(&aff->af_comp, کلید);
		اگر (!HASHITEM_خالی_خانے(hi))
		    id = HI2CI(hi)->ci_newID;
		ورنہ
		{
		    ci = (compitem_T *)getroom(spin, حجم(compitem_T), TRUE);
		    اگر (ci == تہی)
			توڑو;
		    STRCPY(ci->ci_کلید, کلید);
		    ci->ci_flag = flag;
		    // Avoid using a flag ID that has a special meaning in a
		    // regexp (also inside []).
		    کرو
		    {
			check_renumber(spin);
			id = spin->si_newcompID--;
		    } جب (vim_strchr((char_u *)"/?*+[]\\-^", id) != تہی);
		    ci->ci_newID = id;
		    hash_جوڑو(&aff->af_comp, ci->ci_کلید, "spelling");
		}
		*tp++ = id;
	    }
	    اگر (aff->af_flagtype == AFT_عدد && *p == ',')
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
    ساکن خالی
check_renumber(spellinfo_T *spin)
{
    اگر (spin->si_newprefID == spin->si_newcompID && spin->si_newcompID < 128)
    {
	spin->si_newprefID = 127;
	spin->si_newcompID = 255;
    }
}

/*
 * Append one affix or compound ID to "store_afflist".
 * Returns FAIL when this would overrun the fixed-size buffer.
 */
    ساکن صحیح
store_afflist_جوڑو(
    char_u	*store_afflist,
    صحیح		*cntp,
    صحیح		id)
{
    اگر (*cntp >= MAXWLEN - 1)
    {
	emsg(_(e_too_many_postponed_prefixes_spell));
	واپس FAIL;
    }
    store_afflist[(*cntp)++] = id;
    store_afflist[*cntp] = NUL;
    واپس OK;
}

/*
 * Return TRUE if flag "flag" appears in affix list "afflist".
 */
    ساکن صحیح
flag_اندر_afflist(صحیح flagtype, char_u *afflist, غیرعلامتی flag)
{
    char_u	*p;
    غیرعلامتی	n;

    انتخاب (flagtype)
    {
	حالت AFT_CHAR:
	    واپس vim_strchr(afflist, flag) != تہی;

	حالت AFT_CAPLONG:
	حالت AFT_LONG:
	    برائے (p = afflist; *p != NUL; )
	    {
		n = mb_ptr2char_adv(&p);
		اگر ((flagtype == AFT_LONG || (n >= 'A' && n <= 'Z'))
								 && *p != NUL)
		    n = mb_ptr2char_adv(&p) + (n << 16);
		اگر (n == flag)
		    واپس TRUE;
	    }
	    توڑو;

	حالت AFT_عدد:
	    برائے (p = afflist; *p != NUL; )
	    {
		n = getdigits(&p);
		اگر (n == 0)
		    n = ZERO_FLAG;
		اگر (n == flag)
		    واپس TRUE;
		اگر (*p != NUL)	// skip over comma
		    ++p;
	    }
	    توڑو;
    }
    واپس FALSE;
}

/*
 * Give a warning when "spinval" and "affval" numbers are set and not the same.
 */
    ساکن خالی
aff_check_نمبر(صحیح spinval, صحیح affval, حرف *name)
{
    اگر (spinval != 0 && spinval != affval)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Give a warning when "spinval" and "affval" strings are set and not the same.
 */
    ساکن خالی
aff_check_string(char_u *spinval, char_u *affval, حرف *name)
{
    اگر (spinval != تہی && STRCMP(spinval, affval) != 0)
	smsg(_("%s value differs from what is used in another .aff file"), name);
}

/*
 * Return TRUE if strings "s1" and "s2" are equal.  Also consider both being
 * NULL as equal.
 */
    ساکن صحیح
str_equal(char_u *s1, char_u *s2)
{
    اگر (s1 == تہی || s2 == تہی)
	واپس s1 == s2;
    واپس STRCMP(s1, s2) == 0;
}

/*
 * Add a from-to item to "gap".  Used for REP and SAL items.
 * They are stored case-folded.
 */
    ساکن خالی
جوڑو_fromto(
    spellinfo_T	*spin,
    garray_T	*gap,
    char_u	*from,
    char_u	*مقصد)
{
    fromto_T	*ftp;
    char_u	لفظ[MAXWLEN];

    اگر (ga_grow(gap, 1) == FAIL)
	واپس;

    ftp = ((fromto_T *)gap->ga_ڈیٹا) + gap->ga_len;
    (خالی)spell_casefold(curwin, from, (صحیح)STRLEN(from), لفظ, MAXWLEN);
    ftp->ft_from = getroom_save(spin, لفظ);
    (خالی)spell_casefold(curwin, مقصد, (صحیح)STRLEN(مقصد), لفظ, MAXWLEN);
    ftp->ft_مقصد = getroom_save(spin, لفظ);
    ++gap->ga_len;
}

/*
 * Convert a boolean argument in a SAL line to TRUE or FALSE;
 */
    ساکن صحیح
sal_مقصد_bool(char_u *s)
{
    واپس STRCMP(s, "1") == 0 || STRCMP(s, "true") == 0;
}

/*
 * Free the structure filled by spell_read_aff().
 */
    ساکن خالی
spell_free_aff(afffile_T *aff)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    صحیح		todo;
    affheader_T	*ah;
    affentry_T	*ae;

    vim_free(aff->af_enc);

    // All this trouble to free the "ae_prog" items...
    برائے (ht = &aff->af_pref; ; ht = &aff->af_suff)
    {
	todo = (صحیح)ht->ht_مستعمل;
	FOR_ALL_HASHTAB_اشیاء(ht, hi, todo)
	{
	    اگر (!HASHITEM_خالی_خانے(hi))
	    {
		--todo;
		ah = HI2AH(hi);
		برائے (ae = ah->ah_پہلا; ae != تہی; ae = ae->ae_اگلا)
		    vim_regfree(ae->ae_prog);
	    }
	}
	اگر (ht == &aff->af_suff)
	    توڑو;
    }

    hash_clear(&aff->af_pref);
    hash_clear(&aff->af_suff);
    hash_clear(&aff->af_comp);
}

/*
 * Read dictionary file "fname".
 * Returns OK or FAIL;
 */
    ساکن صحیح
spell_read_dic(spellinfo_T *spin, char_u *fname, afffile_T *affile)
{
    hashtab_T	ht;
    char_u	سطر[MAXLINELEN];
    char_u	*p;
    char_u	*afflist;
    char_u	store_afflist[MAXWLEN];
    صحیح		pfxlen;
    صحیح		totlen;
    صحیح		need_affix;
    char_u	*dw;
    char_u	*pc;
    char_u	*w;
    صحیح		l;
    hash_T	hash;
    hashitem_T	*hi;
    FILE	*fd;
    صحیح		lnum = 1;
    صحیح		non_ascii = 0;
    صحیح		retval = OK;
    char_u	پیغام_2[MAXLINELEN + MAXWLEN];
    صحیح		flags;
    صحیح		duplicate = 0;
    وقت_T	آخری_پیغام_وقت = 0;

    /*
     * Open the file.
     */
    fd = mch_fopen((حرف *)fname, "r");
    اگر (fd == تہی)
    {
	semsg(_(e_cant_open_file_str), fname);
	واپس FAIL;
    }

    // The hashtable is only used to detect duplicated words.
    hash_init(&ht);

    vim_snprintf((حرف *)IObuff, IOSIZE,
				  _("Reading dictionary file %s..."), fname);
    spell_پیغام(spin, IObuff);

    // start with a message for the first line
    spin->si_پیغام_گنتی = 999999;

    // Read and ignore the first line: word count.
    اگر (vim_fgets(سطر, MAXLINELEN, fd) || !vim_isdigit(*skipwhite(سطر)))
	semsg(_(e_no_لفظ_گنتی_اندر_str), fname);

    /*
     * Read all the lines in the file one by one.
     * The words are converted to 'encoding' here, before being added to
     * the hashtable.
     */
    جب (!vim_fgets(سطر, MAXLINELEN, fd) && !got_int)
    {
	سطر_breakcheck();
	++lnum;
	اگر (سطر[0] == '#' || سطر[0] == '/')
	    جاری;	// comment line

	// Remove CR, LF and white space from the end.  White space halfway
	// the word is kept to allow e.g., "et al.".
	l = (صحیح)STRLEN(سطر);
	جب (l > 0 && سطر[l - 1] <= ' ')
	    --l;
	اگر (l == 0)
	    جاری;	// empty line
	سطر[l] = NUL;

	// Convert from "SET" to 'encoding' when needed.
	اگر (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, سطر, تہی);
	    اگر (pc == تہی)
	    {
		smsg(_("Conversion failure for word in %s line %d: %s"),
						       fname, lnum, سطر);
		جاری;
	    }
	    w = pc;
	}
	ورنہ
	{
	    pc = تہی;
	    w = سطر;
	}

	// Truncate the word at the "/", set "afflist" to what follows.
	// Replace "\/" by "/" and "\\" by "\".
	afflist = تہی;
	برائے (p = w; *p != NUL; MB_PTR_ADV(p))
	{
	    اگر (*p == '\\' && (p[1] == '\\' || p[1] == '/'))
		STRMOVE(p, p + 1);
	    ورنہ اگر (*p == '/')
	    {
		*p = NUL;
		afflist = p + 1;
		توڑو;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	اگر (spin->si_ascii && has_non_ascii(w))
	{
	    ++non_ascii;
	    vim_free(pc);
	    جاری;
	}

	// This takes time, print a message every 10000 words, but not more
	// often than once per second.
	اگر (spin->si_verbose && spin->si_پیغام_گنتی > 10000)
	{
	    spin->si_پیغام_گنتی = 0;
	    اگر (vim_وقت() > آخری_پیغام_وقت)
	    {
		آخری_پیغام_وقت = vim_وقت();
		vim_snprintf((حرف *)پیغام_2, حجم(پیغام_2),
			_("line %6d, word %6ld - %s"),
			   lnum, spin->si_foldwcount + spin->si_keepwcount, w);
		پیغام_آغاز();
		پیغام_outtrans_long_attr(پیغام_2, 0);
		پیغام_clr_eos();
		پیغام_didout = FALSE;
		پیغام_ستون = 0;
		برآمد_flush();
	    }
	}

	// Store the word in the hashtable to be able to find duplicates.
	dw = getroom_save(spin, w);
	اگر (dw == تہی)
	{
	    retval = FAIL;
	    vim_free(pc);
	    توڑو;
	}

	hash = hash_hash(dw);
	hi = hash_lookup(&ht, dw, hash);
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    اگر (p_verbose > 0)
		smsg(_("Duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ورنہ اگر (duplicate == 0)
		smsg(_("First duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ++duplicate;
	}
	ورنہ
	    hash_جوڑو_شے(&ht, hi, dw, hash);

	flags = 0;
	store_afflist[0] = NUL;
	pfxlen = 0;
	totlen = 0;
	need_affix = FALSE;
	اگر (afflist != تہی)
	{
	    // Extract flags from the affix list.
	    flags |= get_affix_flags(affile, afflist);

	    اگر (affile->af_needaffix != 0 && flag_اندر_afflist(
			  affile->af_flagtype, afflist, affile->af_needaffix))
		need_affix = TRUE;

	    اگر (affile->af_pfxpostpone)
	    {
		// Need to store the list of prefix IDs with the word.
		اگر (get_pfxlist(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    توڑو;
		}
		pfxlen = totlen;
	    }

	    اگر (spin->si_compflags != تہی)
	    {
		// Need to store the list of compound flags with the word.
		// Concatenate them to the list of prefix IDs.
		اگر (get_compflags(affile, afflist, store_afflist, &totlen)
								      == FAIL)
		{
		    retval = FAIL;
		    vim_free(pc);
		    توڑو;
		}
	    }
	}

	// Add the word to the word tree(s).
	اگر (store_لفظ(spin, dw, flags, spin->si_region,
					   store_afflist, need_affix) == FAIL)
	    retval = FAIL;

	اگر (afflist != تہی)
	{
	    // Find all matching suffixes and add the resulting words.
	    // Additionally do matching prefixes that combine.
	    اگر (store_aff_لفظ(spin, dw, afflist, affile,
			   &affile->af_suff, &affile->af_pref,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;

	    // Find all matching prefixes and add the resulting words.
	    اگر (store_aff_لفظ(spin, dw, afflist, affile,
			  &affile->af_pref, تہی,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;
	}

	vim_free(pc);
    }

    اگر (duplicate > 0)
	smsg(_("%d duplicate word(s) in %s"), duplicate, fname);
    اگر (spin->si_ascii && non_ascii > 0)
	smsg(_("Ignored %d word(s) with non-ASCII characters in %s"),
							    non_ascii, fname);
    hash_clear(&ht);

    fclose(fd);
    واپس retval;
}

/*
 * Check for affix flags in "afflist" that are turned into word flags.
 * Return WF_ flags.
 */
    ساکن صحیح
get_affix_flags(afffile_T *affile, char_u *afflist)
{
    صحیح		flags = 0;

    اگر (affile->af_keepcase != 0 && flag_اندر_afflist(
			   affile->af_flagtype, afflist, affile->af_keepcase))
	flags |= WF_KEEPCAP | WF_FIXCAP;
    اگر (affile->af_rare != 0 && flag_اندر_afflist(
			       affile->af_flagtype, afflist, affile->af_rare))
	flags |= WF_RARE;
    اگر (affile->af_bad != 0 && flag_اندر_afflist(
				affile->af_flagtype, afflist, affile->af_bad))
	flags |= WF_BANNED;
    اگر (affile->af_needcomp != 0 && flag_اندر_afflist(
			   affile->af_flagtype, afflist, affile->af_needcomp))
	flags |= WF_NEEDCOMP;
    اگر (affile->af_comproot != 0 && flag_اندر_afflist(
			   affile->af_flagtype, afflist, affile->af_comproot))
	flags |= WF_COMPROOT;
    اگر (affile->af_nosuggest != 0 && flag_اندر_afflist(
			  affile->af_flagtype, afflist, affile->af_nosuggest))
	flags |= WF_NOSUGGEST;
    واپس flags;
}

/*
 * Get the list of prefix IDs from the affix list "afflist".
 * Used for PFXPOSTPONE.
 * Put the resulting flags in "store_afflist[MAXWLEN]" with a terminating NUL.
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    ساکن صحیح
get_pfxlist(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    صحیح		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    صحیح		id;
    char_u	کلید[AH_کلید_LEN];
    hashitem_T	*hi;

    برائے (p = afflist; *p != NUL; )
    {
	prevp = p;
	اگر (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a postponed prefix flag if it appears in "af_pref"
	    // and its ID is not zero.
	    vim_strncpy(کلید, prevp, p - prevp);
	    hi = hash_ڈھونڈو(&affile->af_pref, کلید);
	    اگر (!HASHITEM_خالی_خانے(hi))
	    {
		id = HI2AH(hi)->ah_newID;
		اگر (id != 0 && store_afflist_جوڑو(store_afflist, cntp, id) == FAIL)
		    واپس FAIL;
	    }
	}
	اگر (affile->af_flagtype == AFT_عدد && *p == ',')
	    ++p;
    }

    واپس OK;
}

/*
 * Get the list of compound IDs from the affix list "afflist" that are used
 * for compound words.
 * Puts the flags in "store_afflist[]".
 * Returns FAIL when the fixed-size buffer would overflow.
 */
    ساکن صحیح
get_compflags(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist,
    صحیح		*cntp)
{
    char_u	*p;
    char_u	*prevp;
    char_u	کلید[AH_کلید_LEN];
    hashitem_T	*hi;

    برائے (p = afflist; *p != NUL; )
    {
	prevp = p;
	اگر (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    // A flag is a compound flag if it appears in "af_comp".
	    vim_strncpy(کلید, prevp, p - prevp);
	    hi = hash_ڈھونڈو(&affile->af_comp, کلید);
	    اگر (!HASHITEM_خالی_خانے(hi)
		    && store_afflist_جوڑو(store_afflist, cntp,
					      HI2CI(hi)->ci_newID) == FAIL)
		واپس FAIL;
	}
	اگر (affile->af_flagtype == AFT_عدد && *p == ',')
	    ++p;
    }

    واپس OK;
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
    ساکن صحیح
store_aff_لفظ(
    spellinfo_T	*spin,		// spell info
    char_u	*لفظ,		// basic word start
    char_u	*afflist,	// list of names of supported affixes
    afffile_T	*affile,
    hashtab_T	*ht,
    hashtab_T	*xht,
    صحیح		condit,		// CONDIT_SUF et al.
    صحیح		flags,		// flags for the word
    char_u	*pfxlist,	// list of prefix IDs
    صحیح		pfxlen)		// nr of flags in "pfxlist" for prefixes, rest
				// is compound flags
{
    صحیح		todo;
    hashitem_T	*hi;
    affheader_T	*ah;
    affentry_T	*ae;
    char_u	newword[MAXWLEN];
    صحیح		retval = OK;
    صحیح		i, j;
    char_u	*p;
    صحیح		use_flags;
    char_u	*use_pfxlist;
    صحیح		use_pfxlen;
    صحیح		need_affix;
    char_u	store_afflist[MAXWLEN];
    char_u	pfx_pfxlist[MAXWLEN];
    size_t	wordlen = STRLEN(لفظ);
    صحیح		use_condit;

    todo = (صحیح)ht->ht_مستعمل;
    برائے (hi = ht->ht_array; todo > 0 && retval == OK; ++hi)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;
	    ah = HI2AH(hi);

	    // Check that the affix combines, if required, and that the word
	    // supports this affix.
	    اگر (((condit & CONDIT_COMB) == 0 || ah->ah_combine)
		    && flag_اندر_afflist(affile->af_flagtype, afflist,
								 ah->ah_flag))
	    {
		// Loop over all affix entries with this name.
		برائے (ae = ah->ah_پہلا; ae != تہی; ae = ae->ae_اگلا)
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
		    اگر ((xht != تہی || !affile->af_pfxpostpone
				|| ae->ae_chop != تہی
				|| ae->ae_flags != تہی)
			    && (ae->ae_chop == تہی
				|| STRLEN(ae->ae_chop) < wordlen)
			    && (ae->ae_prog == تہی
				|| vim_regexec_prog(&ae->ae_prog, FALSE,
							    لفظ, (colnr_T)0))
			    && (((condit & CONDIT_CFIX) == 0)
				== ((condit & CONDIT_AFF) == 0
				    || ae->ae_flags == تہی
				    || !flag_اندر_afflist(affile->af_flagtype,
					ae->ae_flags, affile->af_circumfix))))
		    {
			// Match.  Remove the chop and add the affix.
			اگر (xht == تہی)
			{
			    // prefix: chop/add at the start of the word
			    اگر (ae->ae_جوڑو == تہی)
				*newword = NUL;
			    ورنہ
				vim_strncpy(newword, ae->ae_جوڑو, MAXWLEN - 1);
			    p = لفظ;
			    اگر (ae->ae_chop != تہی)
			    {
				// Skip chop string.
				اگر (has_mbyte)
				{
				    i = mb_charlen(ae->ae_chop);
				    برائے ( ; i > 0; --i)
					MB_PTR_ADV(p);
				}
				ورنہ
				    p += STRLEN(ae->ae_chop);
			    }
			    // Note: this silently truncates the buffer, but this should
			    // not happen in practice
			    STRNCAT(newword, p, MAXWLEN - STRLEN(newword) - 1);
			}
			ورنہ
			{
			    // suffix: chop/add at the end of the word
			    vim_strncpy(newword, لفظ, MAXWLEN - 1);
			    اگر (ae->ae_chop != تہی)
			    {
				// Remove chop string.
				p = newword + STRLEN(newword);
				i = (صحیح)MB_CHARLEN(ae->ae_chop);
				برائے ( ; i > 0; --i)
				    MB_PTR_عقب(newword, p);
				*p = NUL;
			    }
			    اگر (ae->ae_جوڑو != تہی)
				// Note: this silently truncates the buffer, but this should
				// not happen in practice
				STRNCAT(newword, ae->ae_جوڑو, MAXWLEN - STRLEN(newword) - 1);
			}

			use_flags = flags;
			use_pfxlist = pfxlist;
			use_pfxlen = pfxlen;
			need_affix = FALSE;
			use_condit = condit | CONDIT_COMB | CONDIT_AFF;
			اگر (ae->ae_flags != تہی)
			{
			    // Extract flags from the affix list.
			    use_flags |= get_affix_flags(affile, ae->ae_flags);

			    اگر (affile->af_needaffix != 0 && flag_اندر_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_needaffix))
				need_affix = TRUE;

			    // When there is a CIRCUMFIX flag the other affix
			    // must also have it and we don't add the word
			    // with one affix.
			    اگر (affile->af_circumfix != 0 && flag_اندر_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_circumfix))
			    {
				use_condit |= CONDIT_CFIX;
				اگر ((condit & CONDIT_CFIX) == 0)
				    need_affix = TRUE;
			    }

			    اگر (affile->af_pfxpostpone
						|| spin->si_compflags != تہی)
			    {
				صحیح listlen = 0;

				اگر (affile->af_pfxpostpone)
				{
				    // Get prefix IDS from the affix list.
				    اگر (get_pfxlist(affile, ae->ae_flags,
						    store_afflist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					توڑو;
				    }
				    use_pfxlen = listlen;
				}
				ورنہ
				    use_pfxlen = 0;
				use_pfxlist = store_afflist;

				// Combine the prefix IDs. Avoid adding the
				// same ID twice.
				برائے (i = 0; i < pfxlen; ++i)
				{
				    برائے (j = 0; j < use_pfxlen; ++j)
					اگر (pfxlist[i] == use_pfxlist[j])
					    توڑو;
				    اگر (j == use_pfxlen
					    && store_afflist_جوڑو(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					توڑو;
				    }
				    use_pfxlen = listlen;
				}
				اگر (retval == FAIL)
				    توڑو;

				اگر (spin->si_compflags != تہی)
				    // Get compound IDS from the affix list.
				    اگر (get_compflags(affile, ae->ae_flags,
						      use_pfxlist, &listlen)
								      == FAIL)
				    {
					retval = FAIL;
					توڑو;
				    }
				اگر (retval == FAIL)
				    توڑو;

				// Combine the list of compound flags.
				// Concatenate them to the prefix IDs list.
				// Avoid adding the same ID twice.
				برائے (i = pfxlen; pfxlist[i] != NUL; ++i)
				{
				    برائے (j = use_pfxlen;
						   use_pfxlist[j] != NUL; ++j)
					اگر (pfxlist[i] == use_pfxlist[j])
					    توڑو;
				    اگر (use_pfxlist[j] == NUL
					    && store_afflist_جوڑو(use_pfxlist,
							&listlen, pfxlist[i])
								      == FAIL)
				    {
					retval = FAIL;
					توڑو;
				    }
				}
				اگر (retval == FAIL)
				    توڑو;
			    }
			}

			// Obey a "COMPOUNDFORBIDFLAG" of the affix: don't
			// use the compound flags.
			اگر (use_pfxlist != تہی && ae->ae_compforbid)
			{
			    vim_strncpy(pfx_pfxlist, use_pfxlist, use_pfxlen);
			    use_pfxlist = pfx_pfxlist;
			}

			// When there are postponed prefixes...
			اگر (spin->si_prefroot != تہی
				&& spin->si_prefroot->wn_sibling != تہی)
			{
			    // ... add a flag to indicate an affix was used.
			    use_flags |= WF_HAS_AFF;

			    // ... don't use a prefix list if combining
			    // affixes is not allowed.  But do use the
			    // compound flags after them.
			    اگر (!ah->ah_combine && use_pfxlist != تہی)
				use_pfxlist += use_pfxlen;
			}

			// When compounding is supported and there is no
			// "COMPOUNDPERMITFLAG" then forbid compounding on the
			// side where the affix is applied.
			اگر (spin->si_compflags != تہی && !ae->ae_comppermit)
			{
			    اگر (xht != تہی)
				use_flags |= WF_NOCOMPAFT;
			    ورنہ
				use_flags |= WF_NOCOMPBEF;
			}

			// Store the modified word.
			اگر (store_لفظ(spin, newword, use_flags,
						 spin->si_region, use_pfxlist,
							  need_affix) == FAIL)
			    retval = FAIL;

			// When added a prefix or a first suffix and the affix
			// has flags may add a(nother) suffix.  RECURSIVE!
			اگر ((condit & CONDIT_SUF) && ae->ae_flags != تہی)
			    اگر (store_aff_لفظ(spin, newword, ae->ae_flags,
					affile, &affile->af_suff, xht,
					   use_condit & (xht == تہی
							? ~0 :  ~CONDIT_SUF),
				      use_flags, use_pfxlist, pfxlen) == FAIL)
				retval = FAIL;

			// When added a suffix and combining is allowed also
			// try adding a prefix additionally.  Both for the
			// word flags and for the affix flags.  RECURSIVE!
			اگر (xht != تہی && ah->ah_combine)
			{
			    اگر (store_aff_لفظ(spin, newword,
					afflist, affile,
					xht, تہی, use_condit,
					use_flags, use_pfxlist,
					pfxlen) == FAIL
				    || (ae->ae_flags != تہی
					&& store_aff_لفظ(spin, newword,
					    ae->ae_flags, affile,
					    xht, تہی, use_condit,
					    use_flags, use_pfxlist,
					    pfxlen) == FAIL))
				retval = FAIL;
			}
		    }
		}
	    }
	}
    }

    واپس retval;
}

/*
 * Read a file with a list of words.
 */
    ساکن صحیح
spell_read_wordfile(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    طویل	lnum = 0;
    char_u	rline[MAXLINELEN];
    char_u	*سطر;
    char_u	*pc = تہی;
    char_u	*p;
    صحیح		l;
    صحیح		retval = OK;
    صحیح		did_لفظ = FALSE;
    صحیح		non_ascii = 0;
    صحیح		flags;
    صحیح		regionmask;

    /*
     * Open the file.
     */
    fd = mch_fopen((حرف *)fname, "r");
    اگر (fd == تہی)
    {
	semsg(_(e_cant_open_file_str), fname);
	واپس FAIL;
    }

    vim_snprintf((حرف *)IObuff, IOSIZE, _("Reading word file %s..."), fname);
    spell_پیغام(spin, IObuff);

    /*
     * Read all the lines in the file one by one.
     */
    جب (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	سطر_breakcheck();
	++lnum;

	// Skip comment lines.
	اگر (*rline == '#')
	    جاری;

	// Remove CR, LF and white space from the end.
	l = (صحیح)STRLEN(rline);
	جب (l > 0 && rline[l - 1] <= ' ')
	    --l;
	اگر (l == 0)
	    جاری;	// empty or blank line
	rline[l] = NUL;

	// Convert from "/encoding={encoding}" to 'encoding' when needed.
	vim_free(pc);
	اگر (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, تہی);
	    اگر (pc == تہی)
	    {
		smsg(_("Conversion failure for word in %s line %ld: %s"),
							   fname, lnum, rline);
		جاری;
	    }
	    سطر = pc;
	}
	ورنہ
	{
	    pc = تہی;
	    سطر = rline;
	}

	اگر (*سطر == '/')
	{
	    ++سطر;
	    اگر (STRNCMP(سطر, "encoding=", 9) == 0)
	    {
		اگر (spin->si_conv.vc_type != CONV_NONE)
		    smsg(_("Duplicate /encoding= line ignored in %s line %ld: %s"),
						       fname, lnum, سطر - 1);
		ورنہ اگر (did_لفظ)
		    smsg(_("/encoding= line after word ignored in %s line %ld: %s"),
						       fname, lnum, سطر - 1);
		ورنہ
		{
		    char_u	*enc;

		    // Setup for conversion to 'encoding'.
		    سطر += 9;
		    enc = enc_canonize(سطر);
		    اگر (enc != تہی && !spin->si_ascii
			    && convert_تیاری(&spin->si_conv, enc,
							       p_enc) == FAIL)
			smsg(_("Conversion in %s not supported: from %s to %s"),
							  fname, سطر, p_enc);
		    vim_free(enc);
		    spin->si_conv.vc_fail = TRUE;
		}
		جاری;
	    }

	    اگر (STRNCMP(سطر, "regions=", 8) == 0)
	    {
		اگر (spin->si_region_گنتی > 1)
		    smsg(_("Duplicate /regions= line ignored in %s line %ld: %s"),
						       fname, lnum, سطر);
		ورنہ
		{
		    سطر += 8;
		    اگر (STRLEN(سطر) > MAXREGIONS * 2)
			smsg(_("Too many regions in %s line %ld: %s"),
						       fname, lnum, سطر);
		    ورنہ
		    {
			spin->si_region_گنتی = (صحیح)STRLEN(سطر) / 2;
			STRCPY(spin->si_region_name, سطر);

			// Adjust the mask for a word valid in all regions.
			spin->si_region = (1 << spin->si_region_گنتی) - 1;
		    }
		}
		جاری;
	    }

	    smsg(_("/ line ignored in %s line %ld: %s"),
						       fname, lnum, سطر - 1);
	    جاری;
	}

	flags = 0;
	regionmask = spin->si_region;

	// Check for flags and region after a slash.
	p = vim_strchr(سطر, '/');
	اگر (p != تہی)
	{
	    *p++ = NUL;
	    جب (*p != NUL)
	    {
		اگر (*p == '=')		// keep-case word
		    flags |= WF_KEEPCAP | WF_FIXCAP;
		ورنہ اگر (*p == '!')	// Bad, bad, wicked word.
		    flags |= WF_BANNED;
		ورنہ اگر (*p == '?')	// Rare word.
		    flags |= WF_RARE;
		ورنہ اگر (VIM_ISDIGIT(*p)) // region number(s)
		{
		    اگر ((flags & WF_REGION) == 0)   // first one
			regionmask = 0;
		    flags |= WF_REGION;

		    l = *p - '0';
		    اگر (l == 0 || l > spin->si_region_گنتی)
		    {
			smsg(_("Invalid region nr in %s line %ld: %s"),
							  fname, lnum, p);
			توڑو;
		    }
		    regionmask |= 1 << (l - 1);
		}
		ورنہ
		{
		    smsg(_("Unrecognized flags in %s line %ld: %s"),
							      fname, lnum, p);
		    توڑو;
		}
		++p;
	    }
	}

	// Skip non-ASCII words when "spin->si_ascii" is TRUE.
	اگر (spin->si_ascii && has_non_ascii(سطر))
	{
	    ++non_ascii;
	    جاری;
	}

	// Normal word: store it.
	اگر (store_لفظ(spin, سطر, flags, regionmask, تہی, FALSE) == FAIL)
	{
	    retval = FAIL;
	    توڑو;
	}
	did_لفظ = TRUE;
    }

    vim_free(pc);
    fclose(fd);

    اگر (spin->si_ascii && non_ascii > 0)
    {
	vim_snprintf((حرف *)IObuff, IOSIZE,
		  _("Ignored %d words with non-ASCII characters"), non_ascii);
	spell_پیغام(spin, IObuff);
    }

    واپس retval;
}

/*
 * Get part of an sblock_T, "len" bytes long.
 * This avoids calling free() for every little struct we use (and keeping
 * track of them).
 * The memory is cleared to all zeros.
 * Returns NULL when out of memory.
 */
    ساکن خالی *
getroom(
    spellinfo_T *spin,
    size_t	len,		// length needed
    صحیح		align)		// align for pointer
{
    char_u	*p;
    sblock_T	*bl = spin->si_بلاکیں;

    اگر (align && bl != تہی)
	// Round size up for alignment.  On some systems structures need to be
	// aligned to the size of a pointer (e.g., SPARC).
	bl->sb_مستعمل = (bl->sb_مستعمل + حجم(حرف *) - 1)
						      & ~(حجم(حرف *) - 1);

    اگر (bl == تہی || bl->sb_مستعمل + len > SBLOCKSIZE)
    {
	اگر (len >= SBLOCKSIZE)
	    bl = تہی;
	ورنہ
	    // Allocate a block of memory. It is not freed until much later.
	    bl = alloc_clear(offsetof(sblock_T, sb_ڈیٹا) + SBLOCKSIZE + 1);
	اگر (bl == تہی)
	{
	    اگر (!spin->si_did_emsg)
	    {
		emsg(_(e_insufficient_حافظہ_لفظ_list_will_be_incomplete));
		spin->si_did_emsg = TRUE;
	    }
	    واپس تہی;
	}
	bl->sb_اگلا = spin->si_بلاکیں;
	spin->si_بلاکیں = bl;
	bl->sb_مستعمل = 0;
	++spin->si_بلاکیں_cnt;
    }

    p = bl->sb_ڈیٹا + bl->sb_مستعمل;
    bl->sb_مستعمل += (صحیح)len;

    واپس p;
}

/*
 * Make a copy of a string into memory allocated with getroom().
 * Returns NULL when out of memory.
 */
    ساکن char_u *
getroom_save(spellinfo_T *spin, char_u *s)
{
    char_u	*sc;

    sc = (char_u *)getroom(spin, STRLEN(s) + 1, FALSE);
    اگر (sc != تہی)
	STRCPY(sc, s);
    واپس sc;
}


/*
 * Free the list of allocated sblock_T.
 */
    ساکن خالی
free_بلاکیں(sblock_T *bl)
{
    sblock_T	*اگلا;

    جب (bl != تہی)
    {
	اگلا = bl->sb_اگلا;
	vim_free(bl);
	bl = اگلا;
    }
}

/*
 * Allocate the root of a word tree.
 * Returns NULL when out of memory.
 */
    ساکن wordnode_T *
wordtree_alloc(spellinfo_T *spin)
{
    واپس (wordnode_T *)getroom(spin, حجم(wordnode_T), TRUE);
}

/*
 * Return TRUE if "word" contains valid word characters.
 * Control characters and trailing '/' are invalid.  Space is OK.
 */
    ساکن صحیح
valid_spell_لفظ(char_u *لفظ, char_u *انتہا)
{
    char_u *p;

    اگر (enc_utf8 && !utf_valid_string(لفظ, انتہا))
	واپس FALSE;
    برائے (p = لفظ; *p != NUL && p < انتہا; p += mb_ptr2len(p))
	اگر (*p < ' ' || (p[0] == '/' && p[1] == NUL))
	    واپس FALSE;
    واپس TRUE;
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
    ساکن صحیح
store_لفظ(
    spellinfo_T	*spin,
    char_u	*لفظ,
    صحیح		flags,		// extra flags, WF_BANNED
    صحیح		region,		// supported region(s)
    char_u	*pfxlist,	// list of prefix IDs or NULL
    صحیح		need_affix)	// only store word with affix ID
{
    صحیح		len = (صحیح)STRLEN(لفظ);
    صحیح		ct = captype(لفظ, لفظ + len);
    char_u	foldword[MAXWLEN];
    صحیح		res = OK;
    char_u	*p;

    // Avoid adding illegal bytes to the word tree.
    اگر (!valid_spell_لفظ(لفظ, لفظ + len))
	واپس FAIL;

    (خالی)spell_casefold(curwin, لفظ, len, foldword, MAXWLEN);
    برائے (p = pfxlist; res == OK; ++p)
    {
	اگر (!need_affix || (p != تہی && *p != NUL))
	    res = درخت_جوڑو_لفظ(spin, foldword, spin->si_foldroot, ct | flags,
						  region, p == تہی ? 0 : *p);
	اگر (p == تہی || *p == NUL)
	    توڑو;
    }
    ++spin->si_foldwcount;

    اگر (res == OK && (ct == WF_KEEPCAP || (flags & WF_KEEPCAP)))
    {
	برائے (p = pfxlist; res == OK; ++p)
	{
	    اگر (!need_affix || (p != تہی && *p != NUL))
		res = درخت_جوڑو_لفظ(spin, لفظ, spin->si_keeproot, flags,
						  region, p == تہی ? 0 : *p);
	    اگر (p == تہی || *p == NUL)
		توڑو;
	}
	++spin->si_keepwcount;
    }
    واپس res;
}

/*
 * Add word "word" to a word tree at "root".
 * When "flags" < 0 we are adding to the prefix tree where "flags" is used for
 * "rare" and "region" is the condition nr.
 * Returns FAIL when out of memory.
 */
    ساکن صحیح
درخت_جوڑو_لفظ(
    spellinfo_T	*spin,
    char_u	*لفظ,
    wordnode_T	*جڑ,
    صحیح		flags,
    صحیح		region,
    صحیح		affixID)
{
    wordnode_T	*گرہ = جڑ;
    wordnode_T	*np;
    wordnode_T	*copyp, **copyprev;
    wordnode_T	**پچھلا = تہی;
    صحیح		i;

    // Add each byte of the word to the tree, including the NUL at the end.
    برائے (i = 0; ; ++i)
    {
	// When there is more than one reference to this node we need to make
	// a copy, so that we can modify it.  Copy the whole list of siblings
	// (we don't optimize for a partly shared list of siblings).
	اگر (گرہ != تہی && گرہ->wn_refs > 1)
	{
	    --گرہ->wn_refs;
	    copyprev = پچھلا;
	    FOR_ALL_گرہ_SIBLINGS(گرہ, copyp)
	    {
		// Allocate a new node and copy the info.
		np = get_wordnode(spin);
		اگر (np == تہی)
		    واپس FAIL;
		np->wn_بچہ = copyp->wn_بچہ;
		اگر (np->wn_بچہ != تہی)
		    ++np->wn_بچہ->wn_refs;	// child gets extra ref
		np->wn_byte = copyp->wn_byte;
		اگر (np->wn_byte == NUL)
		{
		    np->wn_flags = copyp->wn_flags;
		    np->wn_region = copyp->wn_region;
		    np->wn_affixID = copyp->wn_affixID;
		}

		// Link the new node in the list, there will be one ref.
		np->wn_refs = 1;
		اگر (copyprev != تہی)
		    *copyprev = np;
		copyprev = &np->wn_sibling;

		// Let "node" point to the head of the copied list.
		اگر (copyp == گرہ)
		    گرہ = np;
	    }
	}

	// Look for the sibling that has the same character.  They are sorted
	// on byte value, thus stop searching when a sibling is found with a
	// higher byte value.  For zero bytes (end of word) the sorting is
	// done on flags and then on affixID.
	جب (گرہ != تہی
		&& (گرہ->wn_byte < لفظ[i]
		    || (گرہ->wn_byte == NUL
			&& (flags < 0
			    ? گرہ->wn_affixID < (غیرعلامتی)affixID
			    : (گرہ->wn_flags < (غیرعلامتی)(flags & WN_نقاب)
				|| (گرہ->wn_flags == (flags & WN_نقاب)
				    && (spin->si_sugtree
					? (گرہ->wn_region & 0xffff) < region
					: گرہ->wn_affixID
						    < (غیرعلامتی)affixID)))))))
	{
	    پچھلا = &گرہ->wn_sibling;
	    گرہ = *پچھلا;
	}
	اگر (گرہ == تہی
		|| گرہ->wn_byte != لفظ[i]
		|| (لفظ[i] == NUL
		    && (flags < 0
			|| spin->si_sugtree
			|| گرہ->wn_flags != (flags & WN_نقاب)
			|| گرہ->wn_affixID != affixID)))
	{
	    // Allocate a new node.
	    np = get_wordnode(spin);
	    اگر (np == تہی)
		واپس FAIL;
	    np->wn_byte = لفظ[i];

	    // If "node" is NULL this is a new child or the end of the sibling
	    // list: ref count is one.  Otherwise use ref count of sibling and
	    // make ref count of sibling one (matters when inserting in front
	    // of the list of siblings).
	    اگر (گرہ == تہی)
		np->wn_refs = 1;
	    ورنہ
	    {
		np->wn_refs = گرہ->wn_refs;
		گرہ->wn_refs = 1;
	    }
	    اگر (پچھلا != تہی)
		*پچھلا = np;
	    np->wn_sibling = گرہ;
	    گرہ = np;
	}

	اگر (لفظ[i] == NUL)
	{
	    گرہ->wn_flags = flags;
	    گرہ->wn_region |= region;
	    گرہ->wn_affixID = affixID;
	    توڑو;
	}
	پچھلا = &گرہ->wn_بچہ;
	گرہ = *پچھلا;
    }
#اگر_تعریف_شدہ SPELL_PRINTTREE
    smsg("Added \"%s\"", لفظ);
    spell_print_درخت(جڑ->wn_sibling);
#اگر_ختم

    // count nr of words added since last message
    ++spin->si_پیغام_گنتی;

    اگر (spin->si_compress_cnt > 1)
    {
	اگر (--spin->si_compress_cnt == 1)
	    // Did enough words to lower the block count limit.
	    spin->si_بلاکیں_cnt += compress_inc;
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
#اگر_غیر_تعریف_شدہ SPELL_COMPRESS_ALWAYS
    اگر (spin->si_compress_cnt == 1
	    ? spin->si_free_گنتی < MAXWLEN
	    : spin->si_بلاکیں_cnt >= compress_آغاز)
#اگر_ختم
    {
	// Decrement the block counter.  The effect is that we compress again
	// when the freed up room has been used and another "compress_inc"
	// blocks have been allocated.  Unless "compress_added" words have
	// been added, then the limit is put back again.
	spin->si_بلاکیں_cnt -= compress_inc;
	spin->si_compress_cnt = compress_added;

	اگر (spin->si_verbose)
	{
	    پیغام_آغاز();
	    پیغام_puts(_(پیغام_compressing));
	    پیغام_clr_eos();
	    پیغام_didout = FALSE;
	    پیغام_ستون = 0;
	    برآمد_flush();
	}

	// Compress both trees.  Either they both have many nodes, which makes
	// compression useful, or one of them is small, which means
	// compression goes fast.  But when filling the soundfold word tree
	// there is no keep-case tree.
	wordtree_compress(spin, spin->si_foldroot, "case-folded");
	اگر (affixID >= 0)
	    wordtree_compress(spin, spin->si_keeproot, "keep-case");
    }

    واپس OK;
}

/*
 * Get a wordnode_T, either from the list of previously freed nodes or
 * allocate a new one.
 * Returns NULL when out of memory.
 */
    ساکن wordnode_T *
get_wordnode(spellinfo_T *spin)
{
    wordnode_T *n;

    اگر (spin->si_پہلا_free == تہی)
	n = (wordnode_T *)getroom(spin, حجم(wordnode_T), TRUE);
    ورنہ
    {
	n = spin->si_پہلا_free;
	spin->si_پہلا_free = n->wn_بچہ;
	CLEAR_اشارہ_گر(n);
	--spin->si_free_گنتی;
    }
#اگر_تعریف_شدہ SPELL_PRINTTREE
    اگر (n != تہی)
	n->wn_nr = ++spin->si_wordnode_nr;
#اگر_ختم
    واپس n;
}

/*
 * Decrement the reference count on a node (which is the head of a list of
 * siblings).  If the reference count becomes zero free the node and its
 * siblings.
 * Returns the number of nodes actually freed.
 */
    ساکن صحیح
deref_wordnode(spellinfo_T *spin, wordnode_T *گرہ)
{
    wordnode_T	*np;
    صحیح		cnt = 0;

    اگر (--گرہ->wn_refs == 0)
    {
	FOR_ALL_گرہ_SIBLINGS(گرہ, np)
	{
	    اگر (np->wn_بچہ != تہی)
		cnt += deref_wordnode(spin, np->wn_بچہ);
	    free_wordnode(spin, np);
	    ++cnt;
	}
	++cnt;	    // length field
    }
    واپس cnt;
}

/*
 * Free a wordnode_T for re-use later.
 * Only the "wn_child" field becomes invalid.
 */
    ساکن خالی
free_wordnode(spellinfo_T *spin, wordnode_T *n)
{
    n->wn_بچہ = spin->si_پہلا_free;
    spin->si_پہلا_free = n;
    ++spin->si_free_گنتی;
}

/*
 * Compress a tree: find tails that are identical and can be shared.
 */
    ساکن خالی
wordtree_compress(spellinfo_T *spin, wordnode_T *جڑ, حرف *name)
{
    hashtab_T	    ht;
    طویل	    n;
    طویل	    tot = 0;
    طویل	    perc;

    // Skip the root itself, it's not actually used.  The first sibling is the
    // start of the tree.
    اگر (جڑ->wn_sibling == تہی)
	واپس;

    hash_init(&ht);
    n = گرہ_compress(spin, جڑ->wn_sibling, &ht, &tot);

#اگر_غیر_تعریف_شدہ SPELL_PRINTTREE
    اگر (spin->si_verbose || p_verbose > 2)
#اگر_ختم
    {
	اگر (tot > 1000000)
	    perc = (tot - n) / (tot / 100);
	ورنہ اگر (tot == 0)
	    perc = 0;
	ورنہ
	    perc = (tot - n) * 100 / tot;
	vim_snprintf((حرف *)IObuff, IOSIZE,
		_("Compressed %s: %ld of %ld nodes; %ld (%ld%%) remaining"),
		name, n, tot, tot - n, perc);
	spell_پیغام(spin, IObuff);
    }
#اگر_تعریف_شدہ SPELL_PRINTTREE
    spell_print_درخت(جڑ->wn_sibling);
#اگر_ختم
    hash_clear(&ht);
}

/*
 * Compress a node, its siblings and its children, depth first.
 * Returns the number of compressed nodes.
 */
    ساکن طویل
گرہ_compress(
    spellinfo_T	*spin,
    wordnode_T	*گرہ,
    hashtab_T	*ht,
    طویل	*tot)	    // total count of nodes before compressing,
			    // incremented while going through the tree
{
    wordnode_T	*np;
    wordnode_T	*tp;
    wordnode_T	*بچہ;
    hash_T	hash;
    hashitem_T	*hi;
    طویل	len = 0;
    غیرعلامتی	nr, n;
    طویل	compressed = 0;

    /*
     * Go through the list of siblings.  Compress each child and then try
     * finding an identical child to replace it.
     * Note that with "child" we mean not just the node that is pointed to,
     * but the whole list of siblings of which the child node is the first.
     */
    برائے (np = گرہ; np != تہی && !got_int; np = np->wn_sibling)
    {
	++len;
	اگر ((بچہ = np->wn_بچہ) != تہی)
	{
	    // Compress the child first.  This fills hashkey.
	    compressed += گرہ_compress(spin, بچہ, ht, tot);

	    // Try to find an identical child.
	    hash = hash_hash(بچہ->wn_u1.hashkey);
	    hi = hash_lookup(ht, بچہ->wn_u1.hashkey, hash);
	    اگر (!HASHITEM_خالی_خانے(hi))
	    {
		// There are children we encountered before with a hash value
		// identical to the current child.  Now check if there is one
		// that is really identical.
		برائے (tp = HI2WN(hi); tp != تہی; tp = tp->wn_u2.اگلا)
		    اگر (گرہ_equal(بچہ, tp))
		    {
			// Found one!  Now use that child in place of the
			// current one.  This means the current child and all
			// its siblings is unlinked from the tree.
			++tp->wn_refs;
			compressed += deref_wordnode(spin, بچہ);
			np->wn_بچہ = tp;
			توڑو;
		    }
		اگر (tp == تہی)
		{
		    // No other child with this hash value equals the child of
		    // the node, add it to the linked list after the first
		    // item.
		    tp = HI2WN(hi);
		    بچہ->wn_u2.اگلا = tp->wn_u2.اگلا;
		    tp->wn_u2.اگلا = بچہ;
		}
	    }
	    ورنہ
		// No other child has this hash value, add it to the
		// hashtable.
		hash_جوڑو_شے(ht, hi, بچہ->wn_u1.hashkey, hash);
	}
    }
    *tot += len + 1;	// add one for the node that stores the length

    /*
     * Make a hash key for the node and its siblings, so that we can quickly
     * find a lookalike node.  This must be done after compressing the sibling
     * list, otherwise the hash key would become invalid by the compression.
     */
    گرہ->wn_u1.hashkey[0] = len;
    nr = 0;
    FOR_ALL_گرہ_SIBLINGS(گرہ, np)
    {
	اگر (np->wn_byte == NUL)
	    // end node: use wn_flags, wn_region and wn_affixID
	    n = np->wn_flags + (np->wn_region << 8) + (np->wn_affixID << 16);
	ورنہ
	    // byte node: use the byte value and the child pointer
	    n = (غیرعلامتی)(np->wn_byte + ((long_u)np->wn_بچہ << 8));
	nr = nr * 101 + n;
    }

    // Avoid NUL bytes, it terminates the hash key.
    n = nr & 0xff;
    گرہ->wn_u1.hashkey[1] = n == 0 ? 1 : n;
    n = (nr >> 8) & 0xff;
    گرہ->wn_u1.hashkey[2] = n == 0 ? 1 : n;
    n = (nr >> 16) & 0xff;
    گرہ->wn_u1.hashkey[3] = n == 0 ? 1 : n;
    n = (nr >> 24) & 0xff;
    گرہ->wn_u1.hashkey[4] = n == 0 ? 1 : n;
    گرہ->wn_u1.hashkey[5] = NUL;

    // Check for CTRL-C pressed now and then.
    veryfast_breakcheck();

    واپس compressed;
}

/*
 * Return TRUE when two nodes have identical siblings and children.
 */
    ساکن صحیح
گرہ_equal(wordnode_T *n1, wordnode_T *n2)
{
    wordnode_T	*p1;
    wordnode_T	*p2;

    برائے (p1 = n1, p2 = n2; p1 != تہی && p2 != تہی;
				     p1 = p1->wn_sibling, p2 = p2->wn_sibling)
	اگر (p1->wn_byte != p2->wn_byte
		|| (p1->wn_byte == NUL
		    ? (p1->wn_flags != p2->wn_flags
			|| p1->wn_region != p2->wn_region
			|| p1->wn_affixID != p2->wn_affixID)
		    : (p1->wn_بچہ != p2->wn_بچہ)))
	    توڑو;

    واپس p1 == تہی && p2 == تہی;
}

ساکن صحیح rep_compare(ثابت خالی *s1, ثابت خالی *s2);

/*
 * Function given to qsort() to sort the REP items on "from" string.
 */
    ساکن صحیح
rep_compare(ثابت خالی *s1, ثابت خالی *s2)
{
    fromto_T	*p1 = (fromto_T *)s1;
    fromto_T	*p2 = (fromto_T *)s2;

    واپس STRCMP(p1->ft_from, p2->ft_from);
}

/*
 * Write the Vim .spl file "fname".
 * Return FAIL or OK;
 */
    ساکن صحیح
write_vim_spell(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    صحیح		regionmask;
    صحیح		باری;
    wordnode_T	*درخت;
    صحیح		nodecount;
    صحیح		i;
    صحیح		l;
    garray_T	*gap;
    fromto_T	*ftp;
    char_u	*p;
    صحیح		rr;
    صحیح		retval = OK;
    size_t	fwv = 1;  // collect return value of fwrite() to avoid
			  // warnings from picky compiler

    fd = mch_fopen((حرف *)fname, "w");
    اگر (fd == تہی)
    {
	semsg(_(e_cant_open_file_str), fname);
	واپس FAIL;
    }

    // <HEADER>: <fileID> <versionnr>
							    // <fileID>
    fwv &= fwrite(VIMSPELLMAGIC, VIMSPELLMAGICL, (size_t)1, fd);
    اگر (fwv != (size_t)1)
	// Catch first write error, don't try writing more.
	جاؤ theend;

    putc(VIMSPELLVERSION, fd);				    // <versionnr>

    /*
     * <SECTIONS>: <section> ... <sectionend>
     */

    // SN_INFO: <infotext>
    اگر (spin->si_info != تہی)
    {
	putc(SN_INFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	i = (صحیح)STRLEN(spin->si_info);
	put_بائٹیں(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_info, (size_t)i, (size_t)1, fd); // <infotext>
    }

    // SN_REGION: <regionname> ...
    // Write the region names only if there is more than one.
    اگر (spin->si_region_گنتی > 1)
    {
	putc(SN_REGION, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>
	l = spin->si_region_گنتی * 2;
	put_بائٹیں(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_region_name, (size_t)l, (size_t)1, fd);
							// <regionname> ...
	regionmask = (1 << spin->si_region_گنتی) - 1;
    }
    ورنہ
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
    اگر (!spin->si_ascii && !spin->si_جوڑو)
    {
	char_u	folchars[128 * 8];
	صحیح	flags;

	putc(SN_CHARFLAGS, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	// Form the <folchars> string first, we need to know its length.
	l = 0;
	برائے (i = 128; i < 256; ++i)
	{
	    اگر (has_mbyte)
		l += mb_char2bytes(spelltab.st_fold[i], folchars + l);
	    ورنہ
		folchars[l++] = spelltab.st_fold[i];
	}
	put_بائٹیں(fd, (long_u)(1 + 128 + 2 + l), 4);	// <sectionlen>

	fputc(128, fd);					// <charflagslen>
	برائے (i = 128; i < 256; ++i)
	{
	    flags = 0;
	    اگر (spelltab.st_isw[i])
		flags |= CF_لفظ;
	    اگر (spelltab.st_isu[i])
		flags |= CF_بالائی;
	    fputc(flags, fd);				// <charflags>
	}

	put_بائٹیں(fd, (long_u)l, 2);			// <folcharslen>
	fwv &= fwrite(folchars, (size_t)l, (size_t)1, fd); // <folchars>
    }

    // SN_MIDWORD: <midword>
    اگر (spin->si_midword != تہی)
    {
	putc(SN_MIDWORD, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	i = (صحیح)STRLEN(spin->si_midword);
	put_بائٹیں(fd, (long_u)i, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_midword, (size_t)i, (size_t)1, fd);
							// <midword>
    }

    // SN_PREFCOND: <prefcondcnt> <prefcond> ...
    اگر (spin->si_prefcond.ga_len > 0)
    {
	putc(SN_PREFCOND, fd);				// <sectionID>
	putc(SNF_REQUIRED, fd);				// <sectionflags>

	l = write_spell_prefcond(تہی, &spin->si_prefcond, &fwv);
	put_بائٹیں(fd, (long_u)l, 4);			// <sectionlen>

	write_spell_prefcond(fd, &spin->si_prefcond, &fwv);
    }

    // SN_REP: <repcount> <rep> ...
    // SN_SAL: <salflags> <salcount> <sal> ...
    // SN_REPSAL: <repcount> <rep> ...

    // round 1: SN_REP section
    // round 2: SN_SAL section (unless SN_SOFO is used)
    // round 3: SN_REPSAL section
    برائے (باری = 1; باری <= 3; ++باری)
    {
	اگر (باری == 1)
	    gap = &spin->si_rep;
	ورنہ اگر (باری == 2)
	{
	    // Don't write SN_SAL when using a SN_SOFO section
	    اگر (spin->si_sofofr != تہی && spin->si_sofoto != تہی)
		جاری;
	    gap = &spin->si_sal;
	}
	ورنہ
	    gap = &spin->si_repsal;

	// Don't write the section if there are no items.
	اگر (gap->ga_len == 0)
	    جاری;

	// Sort the REP/REPSAL items.
	اگر (باری != 2)
	    qsort(gap->ga_ڈیٹا, (size_t)gap->ga_len,
					       حجم(fromto_T), rep_compare);

	i = باری == 1 ? SN_REP : (باری == 2 ? SN_SAL : SN_REPSAL);
	putc(i, fd);					// <sectionID>

	// This is for making suggestions, section is not required.
	putc(0, fd);					// <sectionflags>

	// Compute the length of what follows.
	l = 2;	    // count <repcount> or <salcount>
	برائے (i = 0; i < gap->ga_len; ++i)
	{
	    ftp = &((fromto_T *)gap->ga_ڈیٹا)[i];
	    l += 1 + (صحیح)STRLEN(ftp->ft_from);  // count <*fromlen> and <*from>
	    l += 1 + (صحیح)STRLEN(ftp->ft_مقصد);    // count <*tolen> and <*to>
	}
	اگر (باری == 2)
	    ++l;	// count <salflags>
	put_بائٹیں(fd, (long_u)l, 4);			// <sectionlen>

	اگر (باری == 2)
	{
	    i = 0;
	    اگر (spin->si_followup)
		i |= SAL_F0LLOWUP;
	    اگر (spin->si_collapse)
		i |= SAL_COLLAPSE;
	    اگر (spin->si_rem_accents)
		i |= SAL_REM_ACCENTS;
	    putc(i, fd);			// <salflags>
	}

	put_بائٹیں(fd, (long_u)gap->ga_len, 2);	// <repcount> or <salcount>
	برائے (i = 0; i < gap->ga_len; ++i)
	{
	    // <rep> : <repfromlen> <repfrom> <reptolen> <repto>
	    // <sal> : <salfromlen> <salfrom> <saltolen> <salto>
	    ftp = &((fromto_T *)gap->ga_ڈیٹا)[i];
	    برائے (rr = 1; rr <= 2; ++rr)
	    {
		p = rr == 1 ? ftp->ft_from : ftp->ft_مقصد;
		l = (صحیح)STRLEN(p);
		putc(l, fd);
		اگر (l > 0)
		    fwv &= fwrite(p, l, (size_t)1, fd);
	    }
	}

    }

    // SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
    // This is for making suggestions, section is not required.
    اگر (spin->si_sofofr != تہی && spin->si_sofoto != تہی)
    {
	putc(SN_SOFO, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (صحیح)STRLEN(spin->si_sofofr);
	put_بائٹیں(fd, (long_u)(l + STRLEN(spin->si_sofoto) + 4), 4);
							// <sectionlen>

	put_بائٹیں(fd, (long_u)l, 2);			// <sofofromlen>
	fwv &= fwrite(spin->si_sofofr, l, (size_t)1, fd); // <sofofrom>

	l = (صحیح)STRLEN(spin->si_sofoto);
	put_بائٹیں(fd, (long_u)l, 2);			// <sofotolen>
	fwv &= fwrite(spin->si_sofoto, l, (size_t)1, fd); // <sofoto>
    }

    // SN_WORDS: <word> ...
    // This is for making suggestions, section is not required.
    اگر (spin->si_commonwords.ht_مستعمل > 0)
    {
	putc(SN_الفاظ, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// round 1: count the bytes
	// round 2: write the bytes
	برائے (باری = 1; باری <= 2; ++باری)
	{
	    صحیح		todo;
	    صحیح		len = 0;
	    hashitem_T	*hi;

	    todo = (صحیح)spin->si_commonwords.ht_مستعمل;
	    FOR_ALL_HASHTAB_اشیاء(&spin->si_commonwords, hi, todo)
		اگر (!HASHITEM_خالی_خانے(hi))
		{
		    l = (صحیح)STRLEN(hi->hi_کلید) + 1;
		    len += l;
		    اگر (باری == 2)			// <word>
			fwv &= fwrite(hi->hi_کلید, (size_t)l, (size_t)1, fd);
		    --todo;
		}
	    اگر (باری == 1)
		put_بائٹیں(fd, (long_u)len, 4);		// <sectionlen>
	}
    }

    // SN_MAP: <mapstr>
    // This is for making suggestions, section is not required.
    اگر (spin->si_map.ga_len > 0)
    {
	putc(SN_MAP, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>
	l = spin->si_map.ga_len;
	put_بائٹیں(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_map.ga_ڈیٹا, (size_t)l, (size_t)1, fd);
							// <mapstr>
    }

    // SN_SUGFILE: <timestamp>
    // This is used to notify that a .sug file may be available and at the
    // same time allows for checking that a .sug file that is found matches
    // with this .spl file.  That's because the word numbers must be exactly
    // right.
    اگر (!spin->si_nosugfile
	    && (spin->si_sal.ga_len > 0
		     || (spin->si_sofofr != تہی && spin->si_sofoto != تہی)))
    {
	putc(SN_SUGFILE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_بائٹیں(fd, (long_u)8, 4);			// <sectionlen>

	// Set si_sugtime and write it to the file.
	spin->si_sugtime = وقت(تہی);
	put_وقت(fd, spin->si_sugtime);			// <timestamp>
    }

    // SN_NOSPLITSUGS: nothing
    // This is used to notify that no suggestions with word splits are to be
    // made.
    اگر (spin->si_nosplitsugs)
    {
	putc(SN_NOSPLITSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_بائٹیں(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_NOCOMPUNDSUGS: nothing
    // This is used to notify that no suggestions with compounds are to be
    // made.
    اگر (spin->si_nocompoundsugs)
    {
	putc(SN_NOCOMPOUNDSUGS, fd);			// <sectionID>
	putc(0, fd);					// <sectionflags>
	put_بائٹیں(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_COMPOUND: compound info.
    // We don't mark it required, when not supported all compound words will
    // be bad words.
    اگر (spin->si_compflags != تہی)
    {
	putc(SN_COMPOUND, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (صحیح)STRLEN(spin->si_compflags);
	برائے (i = 0; i < spin->si_comppat.ga_len; ++i)
	    l += (صحیح)STRLEN(((char_u **)(spin->si_comppat.ga_ڈیٹا))[i]) + 1;
	put_بائٹیں(fd, (long_u)(l + 7), 4);		// <sectionlen>

	putc(spin->si_compmax, fd);			// <compmax>
	putc(spin->si_compminlen, fd);			// <compminlen>
	putc(spin->si_compsylmax, fd);			// <compsylmax>
	putc(0, fd);		// for Vim 7.0b compatibility
	putc(spin->si_compoptions, fd);			// <compoptions>
	put_بائٹیں(fd, (long_u)spin->si_comppat.ga_len, 2);
							// <comppatcount>
	برائے (i = 0; i < spin->si_comppat.ga_len; ++i)
	{
	    p = ((char_u **)(spin->si_comppat.ga_ڈیٹا))[i];
	    putc((صحیح)STRLEN(p), fd);			// <comppatlen>
	    fwv &= fwrite(p, (size_t)STRLEN(p), (size_t)1, fd);
							// <comppattext>
	}
							// <compflags>
	fwv &= fwrite(spin->si_compflags, (size_t)STRLEN(spin->si_compflags),
							       (size_t)1, fd);
    }

    // SN_NOBREAK: NOBREAK flag
    اگر (spin->si_nobreak)
    {
	putc(SN_NOBREAK, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	// It's empty, the presence of the section flags the feature.
	put_بائٹیں(fd, (long_u)0, 4);			// <sectionlen>
    }

    // SN_SYLLABLE: syllable info.
    // We don't mark it required, when not supported syllables will not be
    // counted.
    اگر (spin->si_syllable != تہی)
    {
	putc(SN_SYLLABLE, fd);				// <sectionID>
	putc(0, fd);					// <sectionflags>

	l = (صحیح)STRLEN(spin->si_syllable);
	put_بائٹیں(fd, (long_u)l, 4);			// <sectionlen>
	fwv &= fwrite(spin->si_syllable, (size_t)l, (size_t)1, fd);
							// <syllable>
    }

    // end of <SECTIONS>
    putc(SN_انتہا, fd);					// <sectionend>


    /*
     * <LWORDTREE>  <KWORDTREE>  <PREFIXTREE>
     */
    spin->si_memtot = 0;
    برائے (باری = 1; باری <= 3; ++باری)
    {
	اگر (باری == 1)
	    درخت = spin->si_foldroot->wn_sibling;
	ورنہ اگر (باری == 2)
	    درخت = spin->si_keeproot->wn_sibling;
	ورنہ
	    درخت = spin->si_prefroot->wn_sibling;

	// Clear the index and wnode fields in the tree.
	clear_گرہ(درخت);

	// Count the number of nodes.  Needed to be able to allocate the
	// memory when reading the nodes.  Also fills in index for shared
	// nodes.
	nodecount = put_گرہ(تہی, درخت, 0, regionmask, باری == 3);

	// number of nodes in 4 bytes
	put_بائٹیں(fd, (long_u)nodecount, 4);	// <nodecount>
	spin->si_memtot += nodecount + nodecount * حجم(صحیح);

	// Write the nodes.
	(خالی)put_گرہ(fd, درخت, 0, regionmask, باری == 3);
    }

    // Write another byte to check for errors (file system full).
    اگر (putc(0, fd) == EOF)
	retval = FAIL;
theend:
    اگر (fclose(fd) == EOF)
	retval = FAIL;

    اگر (fwv != (size_t)1)
	retval = FAIL;
    اگر (retval == FAIL)
	emsg(_(e_error_while_writing));

    واپس retval;
}

/*
 * Clear the index and wnode fields of "node", it siblings and its
 * children.  This is needed because they are a union with other items to save
 * space.
 */
    ساکن خالی
clear_گرہ(wordnode_T *گرہ)
{
    wordnode_T	*np;

    اگر (گرہ != تہی)
	FOR_ALL_گرہ_SIBLINGS(گرہ, np)
	{
	    np->wn_u1.اشاریہ = 0;
	    np->wn_u2.wnode = تہی;

	    اگر (np->wn_byte != NUL)
		clear_گرہ(np->wn_بچہ);
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
    ساکن صحیح
put_گرہ(
    FILE	*fd,		// NULL when only counting
    wordnode_T	*گرہ,
    صحیح		idx,
    صحیح		regionmask,
    صحیح		prefixtree)	// TRUE for PREFIXTREE
{
    صحیح		newindex = idx;
    صحیح		siblingcount = 0;
    wordnode_T	*np;
    صحیح		flags;

    // If "node" is zero the tree is empty.
    اگر (گرہ == تہی)
	واپس 0;

    // Store the index where this node is written.
    گرہ->wn_u1.اشاریہ = idx;

    // Count the number of siblings.
    FOR_ALL_گرہ_SIBLINGS(گرہ, np)
	++siblingcount;

    // Write the sibling count.
    اگر (fd != تہی)
	putc(siblingcount, fd);				// <siblingcount>

    // Write each sibling byte and optionally extra info.
    FOR_ALL_گرہ_SIBLINGS(گرہ, np)
    {
	اگر (np->wn_byte == 0)
	{
	    اگر (fd != تہی)
	    {
		// For a NUL byte (end of word) write the flags etc.
		اگر (prefixtree)
		{
		    // In PREFIXTREE write the required affixID and the
		    // associated condition nr (stored in wn_region).  The
		    // byte value is misused to store the "rare" and "not
		    // combining" flags
		    اگر (np->wn_flags == (short_u)PFX_FLAGS)
			putc(BY_NOFLAGS, fd);		// <byte>
		    ورنہ
		    {
			putc(BY_FLAGS, fd);		// <byte>
			putc(np->wn_flags, fd);		// <pflags>
		    }
		    putc(np->wn_affixID, fd);		// <affixID>
		    put_بائٹیں(fd, (long_u)np->wn_region, 2); // <prefcondnr>
		}
		ورنہ
		{
		    // For word trees we write the flag/region items.
		    flags = np->wn_flags;
		    اگر (regionmask != 0 && np->wn_region != regionmask)
			flags |= WF_REGION;
		    اگر (np->wn_affixID != 0)
			flags |= WF_AFX;
		    اگر (flags == 0)
		    {
			// word without flags or region
			putc(BY_NOFLAGS, fd);			// <byte>
		    }
		    ورنہ
		    {
			اگر (np->wn_flags >= 0x100)
			{
			    putc(BY_FLAGS2, fd);		// <byte>
			    putc(flags, fd);			// <flags>
			    putc((غیرعلامتی)flags >> 8, fd);	// <flags2>
			}
			ورنہ
			{
			    putc(BY_FLAGS, fd);			// <byte>
			    putc(flags, fd);			// <flags>
			}
			اگر (flags & WF_REGION)
			    putc(np->wn_region, fd);		// <region>
			اگر (flags & WF_AFX)
			    putc(np->wn_affixID, fd);		// <affixID>
		    }
		}
	    }
	}
	ورنہ
	{
	    اگر (np->wn_بچہ->wn_u1.اشاریہ != 0
					 && np->wn_بچہ->wn_u2.wnode != گرہ)
	    {
		// The child is written elsewhere, write the reference.
		اگر (fd != تہی)
		{
		    putc(BY_اشاریہ, fd);			// <byte>
							// <nodeidx>
		    put_بائٹیں(fd, (long_u)np->wn_بچہ->wn_u1.اشاریہ, 3);
		}
	    }
	    ورنہ اگر (np->wn_بچہ->wn_u2.wnode == تہی)
		// We will write the child below and give it an index.
		np->wn_بچہ->wn_u2.wnode = گرہ;

	    اگر (fd != تہی)
		اگر (putc(np->wn_byte, fd) == EOF) // <byte> or <xbyte>
		{
		    emsg(_(e_error_while_writing));
		    واپس 0;
		}
	}
    }

    // Space used in the array when reading: one for each sibling and one for
    // the count.
    newindex += siblingcount + 1;

    // Recursively dump the children of each sibling.
    FOR_ALL_گرہ_SIBLINGS(گرہ, np)
	اگر (np->wn_byte != 0 && np->wn_بچہ->wn_u2.wnode == گرہ)
	    newindex = put_گرہ(fd, np->wn_بچہ, newindex, regionmask,
								  prefixtree);

    واپس newindex;
}


/*
 * ":mkspell [-ascii] outfile  infile ..."
 * ":mkspell [-ascii] addfile"
 */
    خالی
ex_mkspell(exarg_T *eap)
{
    صحیح		fcount;
    char_u	**fnames;
    char_u	*arg = eap->arg;
    صحیح		ascii = FALSE;

    اگر (STRNCMP(arg, "-ascii", 6) == 0)
    {
	ascii = TRUE;
	arg = skipwhite(arg + 6);
    }

    // Expand all the remaining arguments (e.g., $VIMRUNTIME).
    اگر (get_arglist_exp(arg, &fcount, &fnames, FALSE) != OK)
	واپس;

    mkspell(fcount, fnames, ascii, eap->forceit, FALSE);
    FreeWild(fcount, fnames);
}

/*
 * Create the .sug file.
 * Uses the soundfold info in "spin".
 * Writes the file with the name "wfname", with ".spl" changed to ".sug".
 */
    ساکن خالی
spell_make_sugfile(spellinfo_T *spin, char_u *wfname)
{
    char_u	*fname = تہی;
    صحیح		len;
    slang_T	*slang;
    صحیح		free_slang = FALSE;

    /*
     * Read back the .spl file that was written.  This fills the required
     * info for soundfolding.  This also uses less memory than the
     * pointer-linked version of the trie.  And it avoids having two versions
     * of the code for the soundfolding stuff.
     * It might have been done already by spell_reload_one().
     */
    FOR_ALL_SPELL_LANGS(slang)
	اگر (fullpathcmp(wfname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	    توڑو;
    اگر (slang == تہی)
    {
	spell_پیغام(spin, (char_u *)_("Reading back spell file..."));
	slang = spell_بھرو_file(wfname, تہی, تہی, FALSE);
	اگر (slang == تہی)
	    واپس;
	free_slang = TRUE;
    }

    /*
     * Clear the info in "spin" that is used.
     */
    spin->si_بلاکیں = تہی;
    spin->si_بلاکیں_cnt = 0;
    spin->si_compress_cnt = 0;	    // will stay at 0 all the time
    spin->si_free_گنتی = 0;
    spin->si_پہلا_free = تہی;
    spin->si_foldwcount = 0;

    /*
     * Go through the trie of good words, soundfold each word and add it to
     * the soundfold trie.
     */
    spell_پیغام(spin, (char_u *)_("Performing soundfolding..."));
    اگر (sug_filltree(spin, slang) == FAIL)
	جاؤ theend;

    /*
     * Create the table which links each soundfold word with a list of the
     * good words it may come from.  Creates buffer "spin->si_spellbuf".
     * This also removes the wordnr from the NUL byte entries to make
     * compression possible.
     */
    اگر (sug_maketable(spin) == FAIL)
	جاؤ theend;

    smsg(_("Number of words after soundfolding: %ld"),
				 (طویل)spin->si_spellbuf->b_ml.ml_سطر_گنتی);

    /*
     * Compress the soundfold trie.
     */
    spell_پیغام(spin, (char_u *)_(پیغام_compressing));
    wordtree_compress(spin, spin->si_foldroot, "case-folded");

    /*
     * Write the .sug file.
     * Make the file name by changing ".spl" to ".sug".
     */
    fname = alloc(MAXPATHL);
    اگر (fname == تہی)
	جاؤ theend;
    vim_strncpy(fname, wfname, MAXPATHL - 1);
    len = (صحیح)STRLEN(fname);
    fname[len - 2] = 'u';
    fname[len - 1] = 'g';
    sug_write(spin, fname);

theend:
    vim_free(fname);
    اگر (free_slang)
	slang_free(slang);
    free_بلاکیں(spin->si_بلاکیں);
    close_spellbuf(spin->si_spellbuf);
}

/*
 * Build the soundfold trie for language "slang".
 */
    ساکن صحیح
sug_filltree(spellinfo_T *spin, slang_T *slang)
{
    char_u	*byts;
    idx_T	*idxs;
    صحیح		گہرائی;
    idx_T	arridx[MAXWLEN];
    صحیح		curi[MAXWLEN];
    char_u	tword[MAXWLEN];
    char_u	tsalword[MAXWLEN];
    صحیح		c;
    idx_T	n;
    غیرعلامتی	الفاظ_مکمل = 0;
    صحیح		wordcount[MAXWLEN];

    // We use si_foldroot for the soundfolded trie.
    spin->si_foldroot = wordtree_alloc(spin);
    اگر (spin->si_foldroot == تہی)
	واپس FAIL;

    // let tree_add_word() know we're adding to the soundfolded tree
    spin->si_sugtree = TRUE;

    /*
     * Go through the whole case-folded tree, soundfold each word and put it
     * in the trie.  Bail out if the tree is empty.
     */
    byts = slang->sl_fbyts;
    idxs = slang->sl_fidxs;
    اگر (byts == تہی || idxs == تہی)
	واپس FAIL;

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;

    گہرائی = 0;
    جب (گہرائی >= 0 && !got_int)
    {
	اگر (curi[گہرائی] > byts[arridx[گہرائی]])
	{
	    // Done all bytes at this node, go up one level.
	    idxs[arridx[گہرائی]] = wordcount[گہرائی];
	    اگر (گہرائی > 0)
		wordcount[گہرائی - 1] += wordcount[گہرائی];

	    --گہرائی;
	    سطر_breakcheck();
	}
	ورنہ
	{

	    // Do one more byte at this node.
	    n = arridx[گہرائی] + curi[گہرائی];
	    ++curi[گہرائی];

	    c = byts[n];
	    اگر (c == 0)
	    {
		// Sound-fold the word.
		tword[گہرائی] = NUL;
		spell_soundfold(slang, tword, TRUE, tsalword);

		// We use the "flags" field for the MSB of the wordnr,
		// "region" for the LSB of the wordnr.
		اگر (درخت_جوڑو_لفظ(spin, tsalword, spin->si_foldroot,
				الفاظ_مکمل >> 16, الفاظ_مکمل & 0xffff,
							   0) == FAIL)
		    واپس FAIL;

		++الفاظ_مکمل;
		++wordcount[گہرائی];

		// Reset the block count each time to avoid compression
		// kicking in.
		spin->si_بلاکیں_cnt = 0;

		// Skip over any other NUL bytes (same word with different
		// flags).  But don't go over the end.
		جب (n + 1 < slang->sl_fbyts_len && byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[گہرائی];
		}
	    }
	    ورنہ اگر (گہرائی < MAXWLEN - 1)
	    {
		// Normal char, go one level deeper.
		tword[گہرائی++] = c;
		arridx[گہرائی] = idxs[n];
		curi[گہرائی] = 1;
		wordcount[گہرائی] = 0;
	    }
	}
    }

    smsg(_("Total number of words: %d"), الفاظ_مکمل);

    واپس OK;
}

/*
 * Make the table that links each word in the soundfold trie to the words it
 * can be produced from.
 * This is not unlike lines in a file, thus use a memfile to be able to access
 * the table efficiently.
 * Returns FAIL when out of memory.
 */
    ساکن صحیح
sug_maketable(spellinfo_T *spin)
{
    garray_T	ga;
    صحیح		res = OK;

    // Allocate a buffer, open a memline for it and create the swap file
    // (uses a temp file, not a .swp file).
    spin->si_spellbuf = open_spellbuf();
    اگر (spin->si_spellbuf == تہی)
	واپس FAIL;

    // Use a buffer to store the line info, avoids allocating many small
    // pieces of memory.
    ga_init2(&ga, 1, 100);

    // recursively go through the tree
    اگر (sug_filltable(spin, spin->si_foldroot->wn_sibling, 0, &ga) == -1)
	res = FAIL;

    ga_clear(&ga);
    واپس res;
}

/*
 * Fill the table for one node and its children.
 * Returns the wordnr at the start of the node.
 * Returns -1 when out of memory.
 */
    ساکن صحیح
sug_filltable(
    spellinfo_T	*spin,
    wordnode_T	*گرہ,
    صحیح		startwordnr,
    garray_T	*gap)	    // place to store line of numbers
{
    wordnode_T	*p, *np;
    صحیح		wordnr = startwordnr;
    صحیح		nr;
    صحیح		پچھلا_nr;

    FOR_ALL_گرہ_SIBLINGS(گرہ, p)
    {
	اگر (p->wn_byte == NUL)
	{
	    gap->ga_len = 0;
	    پچھلا_nr = 0;
	    برائے (np = p; np != تہی && np->wn_byte == NUL; np = np->wn_sibling)
	    {
		اگر (ga_grow(gap, 10) == FAIL)
		    واپس -1;

		nr = (np->wn_flags << 16) + (np->wn_region & 0xffff);
		// Compute the offset from the previous nr and store the
		// offset in a way that it takes a minimum number of bytes.
		// It's a bit like utf-8, but without the need to mark
		// following bytes.
		nr -= پچھلا_nr;
		پچھلا_nr += nr;
		gap->ga_len += offset2bytes(nr,
					 (char_u *)gap->ga_ڈیٹا + gap->ga_len);
	    }

	    // add the NUL byte
	    ((char_u *)gap->ga_ڈیٹا)[gap->ga_len++] = NUL;

	    اگر (ml_append_buf(spin->si_spellbuf, (linenr_T)wordnr,
				     gap->ga_ڈیٹا, gap->ga_len, TRUE) == FAIL)
		واپس -1;
	    ++wordnr;

	    // Remove extra NUL entries, we no longer need them. We don't
	    // bother freeing the nodes, they won't be reused anyway.
	    جب (p->wn_sibling != تہی && p->wn_sibling->wn_byte == NUL)
		p->wn_sibling = p->wn_sibling->wn_sibling;

	    // Clear the flags on the remaining NUL node, so that compression
	    // works a lot better.
	    p->wn_flags = 0;
	    p->wn_region = 0;
	}
	ورنہ
	{
	    wordnr = sug_filltable(spin, p->wn_بچہ, wordnr, gap);
	    اگر (wordnr == -1)
		واپس -1;
	}
    }
    واپس wordnr;
}

/*
 * Convert an offset into a minimal number of bytes.
 * Similar to utf_char2byters, but use 8 bits in followup bytes and avoid NUL
 * bytes.
 */
    ساکن صحیح
offset2bytes(صحیح nr, char_u *buf)
{
    صحیح	    rem;
    صحیح	    b1, b2, b3, b4;

    // Split the number in parts of base 255.  We need to avoid NUL bytes.
    b1 = nr % 255 + 1;
    rem = nr / 255;
    b2 = rem % 255 + 1;
    rem = rem / 255;
    b3 = rem % 255 + 1;
    b4 = rem / 255 + 1;

    اگر (b4 > 1 || b3 > 0x1f)	// 4 bytes
    {
	buf[0] = 0xe0 + b4;
	buf[1] = b3;
	buf[2] = b2;
	buf[3] = b1;
	واپس 4;
    }
    اگر (b3 > 1 || b2 > 0x3f )	// 3 bytes
    {
	buf[0] = 0xc0 + b3;
	buf[1] = b2;
	buf[2] = b1;
	واپس 3;
    }
    اگر (b2 > 1 || b1 > 0x7f )	// 2 bytes
    {
	buf[0] = 0x80 + b2;
	buf[1] = b1;
	واپس 2;
    }
				// 1 byte
    buf[0] = b1;
    واپس 1;
}

/*
 * Write the .sug file in "fname".
 */
    ساکن خالی
sug_write(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    wordnode_T	*درخت;
    صحیح		nodecount;
    صحیح		wcount;
    char_u	*سطر;
    linenr_T	lnum;
    صحیح		len;

    // Create the file.  Note that an existing file is silently overwritten!
    fd = mch_fopen((حرف *)fname, "w");
    اگر (fd == تہی)
    {
	semsg(_(e_cant_open_file_str), fname);
	واپس;
    }

    vim_snprintf((حرف *)IObuff, IOSIZE,
				  _("Writing suggestion file %s..."), fname);
    spell_پیغام(spin, IObuff);

    /*
     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
     */
    اگر (fwrite(VIMSUGMAGIC, VIMSUGMAGICL, (size_t)1, fd) != 1) // <fileID>
    {
	emsg(_(e_error_while_writing));
	جاؤ theend;
    }
    putc(VIMSUGVERSION, fd);				// <versionnr>

    // Write si_sugtime to the file.
    put_وقت(fd, spin->si_sugtime);			// <timestamp>

    /*
     * <SUGWORDTREE>
     */
    spin->si_memtot = 0;
    درخت = spin->si_foldroot->wn_sibling;

    // Clear the index and wnode fields in the tree.
    clear_گرہ(درخت);

    // Count the number of nodes.  Needed to be able to allocate the
    // memory when reading the nodes.  Also fills in index for shared
    // nodes.
    nodecount = put_گرہ(تہی, درخت, 0, 0, FALSE);

    // number of nodes in 4 bytes
    put_بائٹیں(fd, (long_u)nodecount, 4);	// <nodecount>
    spin->si_memtot += nodecount + nodecount * حجم(صحیح);

    // Write the nodes.
    (خالی)put_گرہ(fd, درخت, 0, 0, FALSE);

    /*
     * <SUGTABLE>: <sugwcount> <sugline> ...
     */
    wcount = spin->si_spellbuf->b_ml.ml_سطر_گنتی;
    put_بائٹیں(fd, (long_u)wcount, 4);	// <sugwcount>

    برائے (lnum = 1; lnum <= (linenr_T)wcount; ++lnum)
    {
	// <sugline>: <sugnr> ... NUL
	سطر = ml_get_buf(spin->si_spellbuf, lnum, FALSE);
	len = ml_get_buf_len(spin->si_spellbuf, lnum) + 1;
	اگر (fwrite(سطر, (size_t)len, (size_t)1, fd) == 0)
	{
	    emsg(_(e_error_while_writing));
	    جاؤ theend;
	}
	spin->si_memtot += len;
    }

    // Write another byte to check for errors.
    اگر (putc(0, fd) == EOF)
	emsg(_(e_error_while_writing));

    vim_snprintf((حرف *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin->si_memtot);
    spell_پیغام(spin, IObuff);

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
    خالی
mkspell(
    صحیح		fcount,
    char_u	**fnames,
    صحیح		ascii,		    // -ascii argument given
    صحیح		over_write,	    // overwrite existing output file
    صحیح		added_لفظ)	    // invoked through "zg"
{
    char_u	*fname = تہی;
    char_u	*wfname;
    char_u	**innames;
    صحیح		incount;
    afffile_T	*(afile[MAXREGIONS]);
    صحیح		i;
    صحیح		len;
    stat_T	st;
    صحیح		error = FALSE;
    spellinfo_T spin;

    CLEAR_میدان(spin);
    spin.si_verbose = !added_لفظ;
    spin.si_ascii = ascii;
    spin.si_followup = TRUE;
    spin.si_rem_accents = TRUE;
    ga_init2(&spin.si_rep, حجم(fromto_T), 20);
    ga_init2(&spin.si_repsal, حجم(fromto_T), 20);
    ga_init2(&spin.si_sal, حجم(fromto_T), 20);
    ga_init2(&spin.si_map, حجم(char_u), 100);
    ga_init2(&spin.si_comppat, حجم(char_u *), 20);
    ga_init2(&spin.si_prefcond, حجم(char_u *), 50);
    hash_init(&spin.si_commonwords);
    spin.si_newcompID = 127;	// start compound ID at first maximum

    // default: fnames[0] is output file, following are input files
    // When "fcount" is 1 there is only one file.
    innames = &fnames[fcount == 1 ? 0 : 1];
    incount = fcount - 1;

    wfname = alloc(MAXPATHL);
    اگر (wfname == تہی)
	واپس;

    اگر (fcount >= 1)
    {
	len = (صحیح)STRLEN(fnames[0]);
	اگر (fcount == 1 && len > 4 && STRCMP(fnames[0] + len - 4, ".add") == 0)
	{
	    // For ":mkspell path/en.latin1.add" output file is
	    // "path/en.latin1.add.spl".
	    incount = 1;
	    vim_snprintf((حرف *)wfname, MAXPATHL, "%s.spl", fnames[0]);
	}
	ورنہ اگر (fcount == 1)
	{
	    // For ":mkspell path/vim" output file is "path/vim.latin1.spl".
	    incount = 1;
	    vim_snprintf((حرف *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());
	}
	ورنہ اگر (len > 4 && STRCMP(fnames[0] + len - 4, ".spl") == 0)
	{
	    // Name ends in ".spl", use as the file name.
	    vim_strncpy(wfname, fnames[0], MAXPATHL - 1);
	}
	ورنہ
	    // Name should be language, make the file name from it.
	    vim_snprintf((حرف *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());

	// Check for .ascii.spl.
	اگر (لڑی_تلاش((حرف *)gettail(wfname), SPL_FNAME_ASCII) != تہی)
	    spin.si_ascii = TRUE;

	// Check for .add.spl.
	اگر (لڑی_تلاش((حرف *)gettail(wfname), SPL_FNAME_جوڑو) != تہی)
	    spin.si_جوڑو = TRUE;
    }

    اگر (incount <= 0)
	emsg(_(e_invalid_دلیل));	// need at least output and input names
    ورنہ اگر (vim_strchr(gettail(wfname), '_') != تہی)
	emsg(_(e_output_file_name_must_not_have_region_name));
    ورنہ اگر (incount > MAXREGIONS)
	semsg(_(e_only_اوپر_مقصد_nr_regions_supported), MAXREGIONS);
    ورنہ
    {
	// Check for overwriting before doing things that may take a lot of
	// time.
	اگر (!over_write && mch_stat((حرف *)wfname, &st) >= 0)
	{
	    emsg(_(e_file_exists));
	    جاؤ theend;
	}
	اگر (mch_isdir(wfname))
	{
	    semsg(_(e_str_is_directory), wfname);
	    جاؤ theend;
	}

	fname = alloc(MAXPATHL);
	اگر (fname == تہی)
	    جاؤ theend;

	/*
	 * Init the aff and dic pointers.
	 * Get the region names if there are more than 2 arguments.
	 */
	برائے (i = 0; i < incount; ++i)
	{
	    afile[i] = تہی;

	    اگر (incount > 1)
	    {
		len = (صحیح)STRLEN(innames[i]);
		اگر (STRLEN(gettail(innames[i])) < 5
						|| innames[i][len - 3] != '_')
		{
		    semsg(_(e_invalid_region_اندر_str), innames[i]);
		    جاؤ theend;
		}
		spin.si_region_name[i * 2] = TOLOWER_ASC(innames[i][len - 2]);
		spin.si_region_name[i * 2 + 1] =
					     TOLOWER_ASC(innames[i][len - 1]);
	    }
	}
	spin.si_region_گنتی = incount;

	spin.si_foldroot = wordtree_alloc(&spin);
	spin.si_keeproot = wordtree_alloc(&spin);
	spin.si_prefroot = wordtree_alloc(&spin);
	اگر (spin.si_foldroot == تہی
		|| spin.si_keeproot == تہی
		|| spin.si_prefroot == تہی)
	{
	    free_بلاکیں(spin.si_بلاکیں);
	    جاؤ theend;
	}

	// When not producing a .add.spl file clear the character table when
	// we encounter one in the .aff file.  This means we dump the current
	// one in the .spl file if the .aff file doesn't define one.  That's
	// better than guessing the contents, the table will match a
	// previously loaded spell file.
	اگر (!spin.si_جوڑو)
	    spin.si_clear_chartab = TRUE;

	/*
	 * Read all the .aff and .dic files.
	 * Text is converted to 'encoding'.
	 * Words are stored in the case-folded and keep-case trees.
	 */
	برائے (i = 0; i < incount && !error; ++i)
	{
	    spin.si_conv.vc_type = CONV_NONE;
	    spin.si_region = 1 << i;

	    vim_snprintf((حرف *)fname, MAXPATHL, "%s.aff", innames[i]);
	    اگر (mch_stat((حرف *)fname, &st) >= 0)
	    {
		// Read the .aff file.  Will init "spin->si_conv" based on the
		// "SET" line.
		afile[i] = spell_read_aff(&spin, fname);
		اگر (afile[i] == تہی)
		    error = TRUE;
		ورنہ
		{
		    // Read the .dic file and store the words in the trees.
		    vim_snprintf((حرف *)fname, MAXPATHL, "%s.dic",
								  innames[i]);
		    اگر (spell_read_dic(&spin, fname, afile[i]) == FAIL)
			error = TRUE;
		}
	    }
	    ورنہ
	    {
		// No .aff file, try reading the file as a word list.  Store
		// the words in the trees.
		اگر (spell_read_wordfile(&spin, innames[i]) == FAIL)
		    error = TRUE;
	    }

	    // Free any conversion stuff.
	    convert_تیاری(&spin.si_conv, تہی, تہی);
	}

	اگر (spin.si_compflags != تہی && spin.si_nobreak)
	    پیغام(_("Warning: both compounding and NOBREAK specified"));

	اگر (!error && !got_int)
	{
	    /*
	     * Combine tails in the tree.
	     */
	    spell_پیغام(&spin, (char_u *)_(پیغام_compressing));
	    wordtree_compress(&spin, spin.si_foldroot, "case-folded");
	    wordtree_compress(&spin, spin.si_keeproot, "keep-case");
	    wordtree_compress(&spin, spin.si_prefroot, "prefixes");
	}

	اگر (!error && !got_int)
	{
	    /*
	     * Write the info in the spell file.
	     */
	    vim_snprintf((حرف *)IObuff, IOSIZE,
				      _("Writing spell file %s..."), wfname);
	    spell_پیغام(&spin, IObuff);

	    error = write_vim_spell(&spin, wfname) == FAIL;

	    spell_پیغام(&spin, (char_u *)_("Done!"));
	    vim_snprintf((حرف *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin.si_memtot);
	    spell_پیغام(&spin, IObuff);

	    /*
	     * If the file is loaded need to reload it.
	     */
	    اگر (!error)
		spell_reload_one(wfname, added_لفظ);
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
	برائے (i = 0; i < incount; ++i)
	    اگر (afile[i] != تہی)
		spell_free_aff(afile[i]);

	// Free all the bits and pieces at once.
	free_بلاکیں(spin.si_بلاکیں);

	/*
	 * If there is soundfolding info and no NOSUGFILE item create the
	 * .sug file with the soundfolded word trie.
	 */
	اگر (spin.si_sugtime != 0 && !error && !got_int)
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
    ساکن خالی
spell_پیغام(spellinfo_T *spin, char_u *str)
{
    اگر (spin->si_verbose || p_verbose > 2)
    {
	اگر (!spin->si_verbose)
	    verbose_enter();
	پیغام((حرف *)str);
	برآمد_flush();
	اگر (!spin->si_verbose)
	    verbose_leave();
    }
}

/*
 * ":[count]spellgood  {word}"
 * ":[count]spellwrong {word}"
 * ":[count]spellundo  {word}"
 * ":[count]spellrare  {word}"
 */
    خالی
ex_spell(exarg_T *eap)
{
    spell_جوڑو_لفظ(eap->arg, (صحیح)STRLEN(eap->arg),
		eap->cmdidx == CMD_spellwrong ? SPELL_جوڑو_BAD :
		eap->cmdidx == CMD_spellrare ? SPELL_جوڑو_RARE : SPELL_جوڑو_GOOD,
				   eap->forceit ? 0 : (صحیح)eap->line2,
				   eap->cmdidx == CMD_spellundo);
}

/*
 * Add "word[len]" to 'spellfile' as a good, rare or bad word.
 */
    خالی
spell_جوڑو_لفظ(
    char_u	*لفظ,
    صحیح		len,
    صحیح		what,	    // SPELL_ADD_ values
    صحیح		idx,	    // "zG" and "zW": zero, otherwise index in
			    // 'spellfile'
    صحیح		undo)	    // TRUE for "zug", "zuG", "zuw" and "zuW"
{
    FILE	*fd = تہی;
    buf_T	*buf = تہی;
    صحیح		new_spf = FALSE;
    char_u	*fname;
    char_u	*fnamebuf = تہی;
    char_u	سطر[MAXWLEN * 2];
    طویل	fpos, fpos_اگلا = 0;
    صحیح		i;
    size_t	linelen;
    char_u	*spf;

    اگر (!valid_spell_لفظ(لفظ, لفظ + len))
    {
	emsg(_(e_illegal_character_اندر_لفظ));
	واپس;
    }

    اگر (idx == 0)	    // use internal wordlist
    {
	اگر (int_wordlist == تہی)
	{
	    int_wordlist = vim_tempname('s', FALSE);
	    اگر (int_wordlist == تہی)
		واپس;
	}
	fname = int_wordlist;
    }
    ورنہ
    {
	// If 'spellfile' isn't set figure out a good default value.
	اگر (*curwin->w_s->b_p_spf == NUL)
	{
	    init_spellfile();
	    new_spf = TRUE;
	}

	اگر (*curwin->w_s->b_p_spf == NUL)
	{
	    semsg(_(e_option_str_is_not_سیٹ), "spellfile");
	    واپس;
	}
	fnamebuf = alloc(MAXPATHL);
	اگر (fnamebuf == تہی)
	    واپس;

	برائے (spf = curwin->w_s->b_p_spf, i = 1; *spf != NUL; ++i)
	{
	    copy_option_part(&spf, fnamebuf, MAXPATHL, ",");
	    اگر (i == idx)
		توڑو;
	    اگر (*spf == NUL)
	    {
		semsg(_(e_spellfile_does_not_have_nr_entries), idx);
		vim_free(fnamebuf);
		واپس;
	    }
	}

	// Check that the user isn't editing the .add file somewhere.
	buf = buflist_findname_exp(fnamebuf);
	اگر (buf != تہی && buf->b_ml.ml_mfp == تہی)
	    buf = تہی;
	اگر (buf != تہی && buf_Is_بدلا(buf))
	{
	    emsg(_(e_file_is_loaded_اندر_another_بفر));
	    vim_free(fnamebuf);
	    واپس;
	}

	fname = fnamebuf;
    }

    اگر (what == SPELL_جوڑو_BAD || undo)
    {
	// When the word appears as good word we need to remove that one,
	// since its flags sort before the one with WF_BANNED.
	fd = mch_fopen((حرف *)fname, "r");
	اگر (fd != تہی)
	{
	    جب (!vim_fgets(سطر, MAXWLEN * 2, fd))
	    {
		fpos = fpos_اگلا;
		fpos_اگلا = ftell(fd);
		اگر (fpos_اگلا < 0)
		    توڑو;  // should never happen
		linelen = STRLEN(سطر);
		اگر (linelen >= (size_t)len
			&& STRNCMP(لفظ, سطر, len) == 0
			&& (سطر[len] == '/' || سطر[len] < ' '))
		{
		    // Found duplicate word.  Remove it by writing a '#' at
		    // the start of the line.  Mixing reading and writing
		    // doesn't work for all systems, close the file first.
		    fclose(fd);
		    fd = mch_fopen((حرف *)fname, "r+");
		    اگر (fd == تہی)
			توڑو;
		    اگر (fseek(fd, fpos, SEEK_SET) == 0)
		    {
			fputc('#', fd);
			اگر (undo)
			{
			    home_replace(تہی, fname, NameBuff, MAXPATHL, TRUE);
			    smsg(_("Word '%.*s' removed from %s"),
							 len, لفظ, NameBuff);
			}
		    }
		    اگر (fseek(fd, fpos_اگلا, SEEK_SET) != 0)
		    {
			PERROR(_("Seek error in spellfile"));
			توڑو;
		    }
		}
	    }
	    اگر (fd != تہی)
		fclose(fd);
	}
    }

    اگر (!undo)
    {
	fd = mch_fopen((حرف *)fname, "a");
	اگر (fd == تہی && new_spf)
	{
	    char_u *p;

	    // We just initialized the 'spellfile' option and can't open the
	    // file.  We may need to create the "spell" directory first.  We
	    // already checked the runtime directory is writable in
	    // init_spellfile().
	    اگر (!dir_کا_file_exists(fname) && (p = gettail_sep(fname)) != fname)
	    {
		صحیح c = *p;

		// The directory doesn't exist.  Try creating it and opening
		// the file again.
		*p = NUL;
		vim_mkdir(fname, 0755);
		*p = c;
		fd = mch_fopen((حرف *)fname, "a");
	    }
	}

	اگر (fd == تہی)
	    semsg(_(e_cant_open_file_str), fname);
	ورنہ
	{
	    اگر (what == SPELL_جوڑو_BAD)
		fprintf(fd, "%.*s/!\n", len, لفظ);
	    ورنہ اگر (what == SPELL_جوڑو_RARE)
		fprintf(fd, "%.*s/?\n", len, لفظ);
	    ورنہ
		fprintf(fd, "%.*s\n", len, لفظ);
	    fclose(fd);

	    home_replace(تہی, fname, NameBuff, MAXPATHL, TRUE);
	    smsg(_("Word '%.*s' added to %s"), len, لفظ, NameBuff);
	}
    }

    اگر (fd != تہی)
    {
	// Update the .add.spl file.
	mkspell(1, &fname, FALSE, TRUE, TRUE);

	// If the .add file is edited somewhere, reload it.
	اگر (buf != تہی)
	    buf_reload(buf, buf->b_orig_منوال, FALSE);

	redraw_all_later(UPD_SOME_VALID);
    }
    vim_free(fnamebuf);
}

/*
 * Initialize 'spellfile' for the current buffer.
 */
    ساکن خالی
init_spellfile(خالی)
{
    string_T	buf;
    char_u	*fname;
    char_u	*rtp;
    char_u	*lend;
    صحیح		aspath = FALSE;
    char_u	*lstart = curbuf->b_s.b_p_spl;

    اگر (*curwin->w_s->b_p_spl == NUL || curwin->w_s->b_langp.ga_len <= 0)
	واپس;

    buf.string = alloc(MAXPATHL);
    اگر (buf.string == تہی)
	واپس;

    buf.لمبائی = 0;

    // Find the end of the language name.  Exclude the region.  If there
    // is a path separator remember the start of the tail.
    برائے (lend = curwin->w_s->b_p_spl; *lend != NUL
	    && vim_strchr((char_u *)",._", *lend) == تہی; ++lend)
	اگر (vim_ispathsep(*lend))
	{
	    aspath = TRUE;
	    lstart = lend + 1;
	}

    // Loop over all entries in 'runtimepath'.  Use the first one where we
    // are allowed to write.
    rtp = p_rtp;
    جب (*rtp != NUL)
    {
	اگر (aspath)
	    // Use directory of an entry with path, e.g., for
	    // "/dir/lg.utf-8.spl" use "/dir".
	    vim_strncpy(buf.string, curbuf->b_s.b_p_spl,
		    lstart - curbuf->b_s.b_p_spl - 1);
	ورنہ
	    // Copy the path from 'runtimepath' to buf[].
	    buf.لمبائی = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");
	اگر (filewritable(buf.string) == 2)
	{
	    // Use the first language name from 'spelllang' and the
	    // encoding used in the first loaded .spl file.
	    اگر (aspath)
	    {
		buf.لمبائی = (size_t)(lend - curbuf->b_s.b_p_spl);
		vim_strncpy(buf.string, curbuf->b_s.b_p_spl, buf.لمبائی);
	    }
	    ورنہ
	    {
		// Create the "spell" directory if it doesn't exist yet.
		buf.لمبائی += vim_snprintf_safelen((حرف *)buf.string + buf.لمبائی,
		    MAXPATHL - buf.لمبائی, "/spell");
		اگر (filewritable(buf.string) != 2)
		{
		    اگر (vim_mkdir(buf.string, 0755) != 0)
		    {
			vim_free(buf.string);
			واپس;
		    }
		}

		buf.لمبائی += vim_snprintf_safelen((حرف *)buf.string + buf.لمبائی,
		    MAXPATHL - buf.لمبائی, "/%.*s", (صحیح)(lend - lstart), lstart);
	    }
	    fname = LANGP_اندراج(curwin->w_s->b_langp, 0)
		->lp_slang->sl_fname;
	    vim_snprintf((حرف *)buf.string + buf.لمبائی, MAXPATHL - buf.لمبائی,
		".%s.add",
		fname != تہی && لڑی_تلاش((حرف *)gettail(fname), ".ascii.") != تہی
		    ? (char_u *)"ascii" : spell_enc());
	    سیٹ_option_قدر_give_err((char_u *)"spellfile",
		    0L, buf.string, OPT_LOCAL);
	    توڑو;
	}
	aspath = FALSE;
    }

    vim_free(buf.string);
}



/*
 * Set the spell character tables from strings in the affix file.
 */
    ساکن صحیح
سیٹ_spell_chartab(char_u *fol, char_u *کم, char_u *upp)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    char_u	*pf = fol, *pl = کم, *pu = upp;
    صحیح		f, l, u;

    clear_spell_chartab(&new_st);

    جب (*pf != NUL)
    {
	اگر (*pl == NUL || *pu == NUL)
	{
	    emsg(_(e_format_error_اندر_affix_file_fol_کم_or_upp));
	    واپس FAIL;
	}
	f = mb_ptr2char_adv(&pf);
	l = mb_ptr2char_adv(&pl);
	u = mb_ptr2char_adv(&pu);

	// Every character that appears is a word character.
	اگر (f < 256)
	    new_st.st_isw[f] = TRUE;
	اگر (l < 256)
	    new_st.st_isw[l] = TRUE;
	اگر (u < 256)
	    new_st.st_isw[u] = TRUE;

	// if "LOW" and "FOL" are not the same the "LOW" char needs
	// case-folding
	اگر (l < 256 && l != f)
	{
	    اگر (f >= 256)
	    {
		emsg(_(e_character_اندر_fol_کم_or_upp_is_برآمد_کا_range));
		واپس FAIL;
	    }
	    new_st.st_fold[l] = f;
	}

	// if "UPP" and "FOL" are not the same the "UPP" char needs
	// case-folding, it's upper case and the "UPP" is the upper case of
	// "FOL" .
	اگر (u < 256 && u != f)
	{
	    اگر (f >= 256)
	    {
		emsg(_(e_character_اندر_fol_کم_or_upp_is_برآمد_کا_range));
		واپس FAIL;
	    }
	    new_st.st_fold[u] = f;
	    new_st.st_isu[u] = TRUE;
	    new_st.st_بالائی[f] = u;
	}
    }

    اگر (*pl != NUL || *pu != NUL)
    {
	emsg(_(e_format_error_اندر_affix_file_fol_کم_or_upp));
	واپس FAIL;
    }

    واپس سیٹ_spell_اختتام(&new_st);
}

/*
 * Set the spell character tables from strings in the .spl file.
 */
    ساکن خالی
سیٹ_spell_charflags(
    char_u	*flags,
    صحیح		cnt,	    // length of "flags"
    char_u	*fol)
{
    // We build the new tables here first, so that we can compare with the
    // previous one.
    spelltab_T	new_st;
    صحیح		i;
    char_u	*p = fol;
    صحیح		c;

    clear_spell_chartab(&new_st);

    برائے (i = 0; i < 128; ++i)
    {
	اگر (i < cnt)
	{
	    new_st.st_isw[i + 128] = (flags[i] & CF_لفظ) != 0;
	    new_st.st_isu[i + 128] = (flags[i] & CF_بالائی) != 0;
	}

	اگر (*p != NUL)
	{
	    c = mb_ptr2char_adv(&p);
	    new_st.st_fold[i + 128] = c;
	    اگر (i + 128 != c && new_st.st_isu[i + 128] && c < 256)
		new_st.st_بالائی[c] = i + 128;
	}
    }

    (خالی)سیٹ_spell_اختتام(&new_st);
}

    ساکن صحیح
سیٹ_spell_اختتام(spelltab_T *new_st)
{
    صحیح		i;

    اگر (did_سیٹ_spelltab)
    {
	// check that it's the same table
	برائے (i = 0; i < 256; ++i)
	{
	    اگر (spelltab.st_isw[i] != new_st->st_isw[i]
		    || spelltab.st_isu[i] != new_st->st_isu[i]
		    || spelltab.st_fold[i] != new_st->st_fold[i]
		    || spelltab.st_بالائی[i] != new_st->st_بالائی[i])
	    {
		emsg(_(e_لفظ_characters_فرق_between_spell_files));
		واپس FAIL;
	    }
	}
    }
    ورنہ
    {
	// copy the new spelltab into the one being used
	spelltab = *new_st;
	did_سیٹ_spelltab = TRUE;
    }

    واپس OK;
}

/*
 * Write the table with prefix conditions to the .spl file.
 * When "fd" is NULL only count the length of what is written and return it.
 */
    ساکن صحیح
write_spell_prefcond(FILE *fd, garray_T *gap, size_t *fwv)
{
    صحیح		i;
    char_u	*p;
    صحیح		len;
    صحیح		totlen;

    اگر (fd != تہی)
	put_بائٹیں(fd, (long_u)gap->ga_len, 2);	    // <prefcondcnt>

    totlen = 2 + gap->ga_len; // length of <prefcondcnt> and <condlen> bytes

    برائے (i = 0; i < gap->ga_len; ++i)
    {
	// <prefcond> : <condlen> <condstr>
	p = ((char_u **)gap->ga_ڈیٹا)[i];
	اگر (p != تہی)
	{
	    len = (صحیح)STRLEN(p);
	    اگر (fd != تہی)
	    {
		fputc(len, fd);
		*fwv &= fwrite(p, (size_t)len, (size_t)1, fd);
	    }
	    totlen += len;
	}
	ورنہ اگر (fd != تہی)
	    fputc(0, fd);
    }

    واپس totlen;
}

/*
 * Use map string "map" for languages "lp".
 */
    ساکن خالی
سیٹ_map_str(slang_T *lp, char_u *map)
{
    char_u	*p;
    صحیح		headc = 0;
    صحیح		c;
    صحیح		i;

    اگر (*map == NUL)
    {
	lp->sl_has_map = FALSE;
	واپس;
    }
    lp->sl_has_map = TRUE;

    // Init the array and hash tables empty.
    برائے (i = 0; i < 256; ++i)
	lp->sl_map_array[i] = 0;
    hash_init(&lp->sl_map_hash);

    /*
     * The similar characters are stored separated with slashes:
     * "aaa/bbb/ccc/".  Fill sl_map_array[c] with the character before c and
     * before the same slash.  For characters above 255 sl_map_hash is used.
     */
    برائے (p = map; *p != NUL; )
    {
	c = mb_cptr2char_adv(&p);
	اگر (c == '/')
	    headc = 0;
	ورنہ
	{
	    اگر (headc == 0)
		 headc = c;

	    // Characters above 255 don't fit in sl_map_array[], put them in
	    // the hash table.  Each entry is the char, a NUL the headchar and
	    // a NUL.
	    اگر (c >= 256)
	    {
		صحیح	    cl = mb_char2len(c);
		صحیح	    headcl = mb_char2len(headc);
		char_u	    *b;
		hash_T	    hash;
		hashitem_T  *hi;

		b = alloc(cl + headcl + 2);
		اگر (b == تہی)
		    واپس;
		mb_char2bytes(c, b);
		b[cl] = NUL;
		mb_char2bytes(headc, b + cl + 1);
		b[cl + 1 + headcl] = NUL;
		hash = hash_hash(b);
		hi = hash_lookup(&lp->sl_map_hash, b, hash);
		اگر (HASHITEM_خالی_خانے(hi))
		    hash_جوڑو_شے(&lp->sl_map_hash, hi, b, hash);
		ورنہ
		{
		    // This should have been checked when generating the .spl
		    // file.
		    emsg(_(e_duplicate_char_اندر_map_اندراج));
		    vim_free(b);
		}
	    }
	    ورنہ
		lp->sl_map_array[c] = headc;
	}
    }
}

#اگر_ختم  // FEAT_SPELL
