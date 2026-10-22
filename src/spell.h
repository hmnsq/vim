/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * spell.h: common code for spell checking, used by spell.c and spellfile.c.
 */

// Use SPELL_PRINTTREE for debugging: dump the word tree after adding a word.
// Only use it for small word lists!
#ఒకవేళ 0
# define SPELL_PRINTTREE
#అయితే_ముగింపు

// Use SPELL_COMPRESS_ALWAYS for debugging: compress the word tree after
// adding a word.  Only use it for small word lists!
#ఒకవేళ 0
# define SPELL_COMPRESS_ALWAYS
#అయితే_ముగింపు

// Use DEBUG_TRIEWALK to print the changes made in suggest_trie_walk() for a
// specific word.
#ఒకవేళ 0
# define DEBUG_TRIEWALK
#అయితే_ముగింపు

#నిర్వచించు MAXWLEN 254		// Assume max. word len is this many bytes.
				// Some places assume a word length fits in a
				// byte, thus it can't be above 255.
				// Must be >= PFD_NOTSPECIAL.

#నిర్వచించు MAXREGIONS 8		// Number of regions supported.

// Type used for indexes in the word tree need to be at least 4 bytes.  If int
// is 8 bytes we could use something smaller, but what?
రకనిర్వచనం పూర్ణాంకం idx_T;

రకనిర్వచనం పూర్ణాంకం salfirst_T;

/*
 * Structure used to store words and other info for one language, loaded from
 * a .spl file.
 * The main access is through the tree in "sl_fbyts/sl_fidxs", storing the
 * case-folded words.  "sl_kbyts/sl_kidxs" is for keep-case words.
 *
 * The "byts" array stores the possible bytes in each tree node, preceded by
 * the number of possible bytes, sorted on byte value:
 *	<len> <byte1> <byte2> ...
 * The "idxs" array stores the index of the child node corresponding to the
 * byte in "byts".
 * Exception: when the byte is zero, the word may end here and "idxs" holds
 * the flags, region mask and affixID for the word.  There may be several
 * zeros in sequence for alternative flag/region/affixID combinations.
 */
రకనిర్వచనం నిర్మాణం slang_S slang_T;
నిర్మాణం slang_S
{
    slang_T	*sl_తదుపరి;	// next language
    char_u	*sl_name;	// language name "en", "en.rare", "nl", etc.
    char_u	*sl_fname;	// name of .spl file
    పూర్ణాంకం		sl_కూడు;		// TRUE if it's a .add file.

    char_u	*sl_fbyts;	// case-folded word bytes
    దీర్ఘ	sl_fbyts_len;	// length of sl_fbyts
    idx_T	*sl_fidxs;	// case-folded word indexes
    char_u	*sl_kbyts;	// keep-case word bytes
    idx_T	*sl_kidxs;	// keep-case word indexes
    char_u	*sl_pbyts;	// prefix tree word bytes
    idx_T	*sl_pidxs;	// prefix tree word indexes

    char_u	*sl_info;	// infotext string or NULL

    char_u	sl_regions[MAXREGIONS * 2 + 1];
				// table with up to 8 region names plus NUL

    char_u	*sl_midword;	// MIDWORD string or NULL

    hashtab_T	sl_wordcount;	// hashtable with word count, wordcount_T

    పూర్ణాంకం		sl_compmax;	// COMPOUNDWORDMAX (default: MAXWLEN)
    పూర్ణాంకం		sl_compminlen;	// COMPOUNDMIN (default: 0)
    పూర్ణాంకం		sl_compsylmax;	// COMPOUNDSYLMAX (default: MAXWLEN)
    పూర్ణాంకం		sl_compoptions;	// COMP_* flags
    garray_T	sl_comppat;	// CHECKCOMPOUNDPATTERN items
    regprog_T	*sl_compprog;	// COMPOUNDRULE turned into a regexp progrm
				// (NULL when no compounding)
    char_u	*sl_comprules;	// all COMPOUNDRULE concatenated (or NULL)
    char_u	*sl_compstartflags; // flags for first compound word
    char_u	*sl_compallflags; // all flags for compound words
    char_u	sl_nobreak;	// When TRUE: no spaces between words
    char_u	*sl_syllable;	// SYLLABLE repeatable chars or NULL
    garray_T	sl_syl_వస్తువులు;	// syllable items

    పూర్ణాంకం		sl_prefixcnt;	// number of items in "sl_prefprog"
    regprog_T	**sl_prefprog;	// table with regprogs for prefixes

    garray_T	sl_rep;		// list of fromto_T entries from REP lines
    పొట్టి	sl_rep_మొదటి[256];  // indexes where byte first appears, -1 if
				    // there is none
    garray_T	sl_sal;		// list of salitem_T entries from SAL lines
    salfirst_T	sl_sal_మొదటి[256];  // indexes where byte first appears, -1 if
				    // there is none
    పూర్ణాంకం		sl_followup;	// SAL followup
    పూర్ణాంకం		sl_collapse;	// SAL collapse_result
    పూర్ణాంకం		sl_rem_accents;	// SAL remove_accents
    పూర్ణాంకం		sl_sofo;	// SOFOFROM and SOFOTO instead of SAL items:
				// "sl_sal_first" maps chars, when has_mbyte
				// "sl_sal" is a list of wide char lists.
    garray_T	sl_repsal;	// list of fromto_T entries from REPSAL lines
    పొట్టి	sl_repsal_మొదటి[256];  // sl_rep_first for REPSAL lines
    పూర్ణాంకం		sl_nosplitsugs;	// don't suggest splitting a word
    పూర్ణాంకం		sl_nocompoundsugs; // don't suggest compounding

    // Info from the .sug file.  Loaded on demand.
    time_t	sl_sugtime;	// timestamp for .sug file
    char_u	*sl_sbyts;	// soundfolded word bytes
    దీర్ఘ	sl_sbyts_len;	// length of sl_sbyts
    idx_T	*sl_sidxs;	// soundfolded word indexes
    buf_T	*sl_sugbuf;	// buffer with word number table
    పూర్ణాంకం		sl_sugloaded;	// TRUE when .sug file was loaded or failed to
				//ఎక్కించు

    పూర్ణాంకం		sl_has_map;	// TRUE if there is a MAP line
    hashtab_T	sl_map_hash;	// MAP for multi-byte chars
    పూర్ణాంకం		sl_map_array[256]; // MAP for first 256 chars
    hashtab_T	sl_sounddone;	// table with soundfolded words that have
				// handled, see add_sound_suggest()
};

#నిర్వచితమైతే VMS
# define SPL_FNAME_TMPL  "%s_%s.spl"
# define SPL_FNAME_కూడు   "_add."
# define SPL_FNAME_ASCII "_ascii."
#లేకపోతే
# define SPL_FNAME_TMPL  "%s.%s.spl"
# define SPL_FNAME_కూడు   ".add."
# define SPL_FNAME_ASCII ".ascii."
#అయితే_ముగింపు

// Flags used for a word.  Only the lowest byte can be used, the region byte
// comes above it.
#నిర్వచించు WF_REGION   0x01	// region byte follows
#నిర్వచించు WF_ONECAP   0x02	// word with one capital (or all capitals)
#నిర్వచించు WF_ALLCAP   0x04	// word must be all capitals
#నిర్వచించు WF_RARE	    0x08	// rare word
#నిర్వచించు WF_BANNED   0x10	// bad word
#నిర్వచించు WF_AFX	    0x20	// affix ID follows
#నిర్వచించు WF_FIXCAP   0x40	// keep-case word, allcap not allowed
#నిర్వచించు WF_KEEPCAP  0x80	// keep-case word

#నిర్వచించు WF_CAPMASK (WF_ONECAP | WF_ALLCAP | WF_KEEPCAP | WF_FIXCAP)

// for <flags2>, shifted up one byte to be used in wn_flags
#నిర్వచించు WF_HAS_AFF  0x0100	// word includes affix
#నిర్వచించు WF_NEEDCOMP 0x0200	// word only valid in compound
#నిర్వచించు WF_NOSUGGEST 0x0400	// word not to be suggested
#నిర్వచించు WF_COMPROOT 0x0800	// already compounded word, COMPOUNDROOT
#నిర్వచించు WF_NOCOMPBEF 0x1000	// no compounding before this word
#నిర్వచించు WF_NOCOMPAFT 0x2000	// no compounding after this word

// flags for <pflags>
#నిర్వచించు WFP_RARE	    0x01	// rare prefix
#నిర్వచించు WFP_NC		    0x02	// prefix is not combining
#నిర్వచించు WFP_పైకి		    0x04	// to-upper prefix
#నిర్వచించు WFP_COMPPERMIT	    0x08	// prefix with COMPOUNDPERMITFLAG
#నిర్వచించు WFP_COMPFORBID	    0x10	// prefix with COMPOUNDFORBIDFLAG

// Flags for postponed prefixes in "sl_pidxs".  Must be above affixID (one
// byte) and prefcondnr (two bytes).
#నిర్వచించు WF_RAREPFX  (WFP_RARE << 24)	// rare postponed prefix
#నిర్వచించు WF_PFX_NC   (WFP_NC << 24)	// non-combining postponed prefix
#నిర్వచించు WF_PFX_పైకి   (WFP_పైకి << 24)	// to-upper postponed prefix
#నిర్వచించు WF_PFX_COMPPERMIT (WFP_COMPPERMIT << 24) // postponed prefix with
						 // COMPOUNDPERMITFLAG
#నిర్వచించు WF_PFX_COMPFORBID (WFP_COMPFORBID << 24) // postponed prefix with
						 // COMPOUNDFORBIDFLAG

// flags for <compoptions>
#నిర్వచించు COMP_CHECKDUP		1	// CHECKCOMPOUNDDUP
#నిర్వచించు COMP_CHECKREP		2	// CHECKCOMPOUNDREP
#నిర్వచించు COMP_CHECKCASE		4	// CHECKCOMPOUNDCASE
#నిర్వచించు COMP_CHECKTRIPLE	8	// CHECKCOMPOUNDTRIPLE

// Info from "REP", "REPSAL" and "SAL" entries in ".aff" file used in si_rep,
// si_repsal, sl_rep, and si_sal.  Not for sl_sal!
// One replacement: from "ft_from" to "ft_to".
రకనిర్వచనం నిర్మాణం fromto_S
{
    char_u	*ft_from;
    char_u	*ft_గమ్యం;
} fromto_T;

// Info from "SAL" entries in ".aff" file used in sl_sal.
// The info is split for quick processing by spell_soundfold().
// Note that "sm_oneof" and "sm_rules" point into sm_lead.
రకనిర్వచనం నిర్మాణం salitem_S
{
    char_u	*sm_lead;	// leading letters
    పూర్ణాంకం		sm_leadlen;	// length of "sm_lead"
    char_u	*sm_oneof;	// letters from () or NULL
    char_u	*sm_నియమాలు;	// rules like ^, $, priority
    char_u	*sm_గమ్యం;		// replacement.
    పూర్ణాంకం		*sm_lead_w;	// wide character copy of "sm_lead"
    పూర్ణాంకం		*sm_oneof_w;	// wide character copy of "sm_oneof"
    పూర్ణాంకం		*sm_గమ్యం_w;	// wide character copy of "sm_to"
} salitem_T;

// Values for SP_*ERROR are negative, positive values are used by
// read_cnt_string().
#నిర్వచించు	SP_TRUNCERROR	(-1)	// spell file truncated error
#నిర్వచించు	SP_FORMERROR	(-2)	// format error in spell file
#నిర్వచించు SP_OTHERERROR	(-3)	// other error while reading spell file

/*
 * Structure used in "b_langp", filled from 'spelllang'.
 */
రకనిర్వచనం నిర్మాణం langp_S
{
    slang_T	*lp_slang;	// info for this language
    slang_T	*lp_sallang;	// language used for sound folding or NULL
    slang_T	*lp_replang;	// language used for REP items or NULL
    పూర్ణాంకం		lp_region;	// bitmask for region or REGION_ALL
} langp_T;

#నిర్వచించు LANGP_నమోదు(ga, i)	(((langp_T *)(ga).ga_దత్తాంశం) + (i))

#నిర్వచించు VIMSUGMAGIC "VIMsug"	// string at start of Vim .sug file
#నిర్వచించు VIMSUGMAGICL 6
#నిర్వచించు VIMSUGVERSION 1

/*
 * The tables used for recognizing word characters according to spelling.
 * These are only used for the first 256 characters of 'encoding'.
 */
రకనిర్వచనం నిర్మాణం spelltab_S
{
    char_u  st_isw[256];	// flags: is word char
    char_u  st_isu[256];	// flags: is uppercase char
    char_u  st_fold[256];	// chars: folded case
    char_u  st_పై[256];	// chars: upper case
} spelltab_T;

/*
 * Use our own character-case definitions, because the current locale may
 * differ from what the .spl file uses.
 * These must not be called with negative number!
 */
#ఒకవేళ defined(HAVE_WCHAR_H)
# include <wchar.h>	    // for towupper() and towlower()
#అయితే_ముగింపు
// Multi-byte implementation.  For Unicode we can call utf_*(), but don't do
// that for ASCII, because we don't want to use 'casemap' here.  Otherwise use
// the "w" library function for characters above 255 if available.
#నిర్వచితమైతే HAVE_TOWLOWER
# define SPELL_TOFOLD(c) (enc_utf8 && (c) >= 128 ? utf_fold(c) \
	    : (c) < 256 ? (పూర్ణాంకం)spelltab.st_fold[c] : (పూర్ణాంకం)towlower(c))
#లేకపోతే
# define SPELL_TOFOLD(c) (enc_utf8 && (c) >= 128 ? utf_fold(c) \
	    : (c) < 256 ? (పూర్ణాంకం)spelltab.st_fold[c] : (c))
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_TOWUPPER
# define SPELL_TOUPPER(c) (enc_utf8 && (c) >= 128 ? utf_toupper(c) \
	    : (c) < 256 ? (పూర్ణాంకం)spelltab.st_పై[c] : (పూర్ణాంకం)towupper(c))
#లేకపోతే
# define SPELL_TOUPPER(c) (enc_utf8 && (c) >= 128 ? utf_toupper(c) \
	    : (c) < 256 ? (పూర్ణాంకం)spelltab.st_పై[c] : (c))
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_ISWUPPER
# define SPELL_ISUPPER(c) (enc_utf8 && (c) >= 128 ? utf_isupper(c) \
	    : (c) < 256 ? spelltab.st_isu[c] : iswupper(c))
#లేకపోతే
# define SPELL_ISUPPER(c) (enc_utf8 && (c) >= 128 ? utf_isupper(c) \
	    : (c) < 256 ? spelltab.st_isu[c] : (FALSE))
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SPELL
# ifdef లోపల_SPELL_C
#  define SPELL_EXTERN
#  define SPELL_INIT(x) x
# లేకపోతే
#  define SPELL_EXTERN బాహ్య
#  define SPELL_INIT(x)
# endif

// First language that is loaded, start of the linked list of loaded
// languages.
SPELL_EXTERN slang_T	*మొదటి_lang SPELL_INIT(= ఖాళీ);

// file used for "zG" and "zW"
SPELL_EXTERN char_u	*int_wordlist SPELL_INIT(= ఖాళీ);

SPELL_EXTERN spelltab_T   spelltab;
SPELL_EXTERN పూర్ణాంకం	  did_సమితి_spelltab;

// Values for "what" argument of spell_add_word()
# define SPELL_కూడు_GOOD	0
# define SPELL_కూడు_BAD	1
# define SPELL_కూడు_RARE	2

రకనిర్వచనం నిర్మాణం wordcount_S
{
    short_u	wc_లెక్క;	    // nr of times word was seen
    char_u	wc_మాట[1];	    // word, actually longer
} wordcount_T;

# define WC_కీలకం_OFF  offsetof(wordcount_T, wc_మాట)
# define HI2WC(hi)     ((wordcount_T *)((hi)->hi_కీలకం - WC_కీలకం_OFF))
# define MAXWORDCOUNT 0xffff

// Remember what "z?" replaced.
SPELL_EXTERN char_u	*repl_from SPELL_INIT(= ఖాళీ);
SPELL_EXTERN char_u	*repl_గమ్యం SPELL_INIT(= ఖాళీ);
#అయితే_ముగింపు
