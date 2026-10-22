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
#إذا 0
# define SPELL_PRINTTREE
#د_که_پای

// Use SPELL_COMPRESS_ALWAYS for debugging: compress the word tree after
// adding a word.  Only use it for small word lists!
#إذا 0
# define SPELL_COMPRESS_ALWAYS
#د_که_پای

// Use DEBUG_TRIEWALK to print the changes made in suggest_trie_walk() for a
// specific word.
#إذا 0
# define DEBUG_TRIEWALK
#د_که_پای

#تعریف_کړه MAXWLEN 254		// Assume max. word len is this many bytes.
				// Some places assume a word length fits in a
				// byte, thus it can't be above 255.
				// Must be >= PFD_NOTSPECIAL.

#تعریف_کړه MAXREGIONS 8		// Number of regions supported.

// Type used for indexes in the word tree need to be at least 4 bytes.  If int
// is 8 bytes we could use something smaller, but what?
تعريفنوع صحيح idx_T;

تعريفنوع صحيح salfirst_T;

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
تعريفنوع بنية slang_S slang_T;
بنية slang_S
{
    slang_T	*sl_بل;	// next language
    char_u	*sl_name;	// language name "en", "en.rare", "nl", etc.
    char_u	*sl_fname;	// name of .spl file
    صحيح		sl_جمع_کړه;		// TRUE if it's a .add file.

    char_u	*sl_fbyts;	// case-folded word bytes
    اوږد	sl_fbyts_len;	// length of sl_fbyts
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

    صحيح		sl_compmax;	// COMPOUNDWORDMAX (default: MAXWLEN)
    صحيح		sl_compminlen;	// COMPOUNDMIN (default: 0)
    صحيح		sl_compsylmax;	// COMPOUNDSYLMAX (default: MAXWLEN)
    صحيح		sl_compoptions;	// COMP_* flags
    garray_T	sl_comppat;	// CHECKCOMPOUNDPATTERN items
    regprog_T	*sl_compprog;	// COMPOUNDRULE turned into a regexp progrm
				// (NULL when no compounding)
    char_u	*sl_comprules;	// all COMPOUNDRULE concatenated (or NULL)
    char_u	*sl_compstartflags; // flags for first compound word
    char_u	*sl_compallflags; // all flags for compound words
    char_u	sl_nobreak;	// When TRUE: no spaces between words
    char_u	*sl_syllable;	// SYLLABLE repeatable chars or NULL
    garray_T	sl_syl_توکي;	// syllable items

    صحيح		sl_prefixcnt;	// number of items in "sl_prefprog"
    regprog_T	**sl_prefprog;	// table with regprogs for prefixes

    garray_T	sl_rep;		// list of fromto_T entries from REP lines
    لنډ	sl_rep_لومړی[256];  // indexes where byte first appears, -1 if
				    // there is none
    garray_T	sl_sal;		// list of salitem_T entries from SAL lines
    salfirst_T	sl_sal_لومړی[256];  // indexes where byte first appears, -1 if
				    // there is none
    صحيح		sl_followup;	// SAL followup
    صحيح		sl_collapse;	// SAL collapse_result
    صحيح		sl_rem_accents;	// SAL remove_accents
    صحيح		sl_sofo;	// SOFOFROM and SOFOTO instead of SAL items:
				// "sl_sal_first" maps chars, when has_mbyte
				// "sl_sal" is a list of wide char lists.
    garray_T	sl_repsal;	// list of fromto_T entries from REPSAL lines
    لنډ	sl_repsal_لومړی[256];  // sl_rep_first for REPSAL lines
    صحيح		sl_nosplitsugs;	// don't suggest splitting a word
    صحيح		sl_nocompoundsugs; // don't suggest compounding

    // Info from the .sug file.  Loaded on demand.
    time_t	sl_sugtime;	// timestamp for .sug file
    char_u	*sl_sbyts;	// soundfolded word bytes
    اوږد	sl_sbyts_len;	// length of sl_sbyts
    idx_T	*sl_sidxs;	// soundfolded word indexes
    buf_T	*sl_sugbuf;	// buffer with word number table
    صحيح		sl_sugloaded;	// TRUE when .sug file was loaded or failed to
				//بارول

    صحيح		sl_has_map;	// TRUE if there is a MAP line
    hashtab_T	sl_map_hash;	// MAP for multi-byte chars
    صحيح		sl_map_array[256]; // MAP for first 256 chars
    hashtab_T	sl_sounddone;	// table with soundfolded words that have
				// handled, see add_sound_suggest()
};

#که_تعریف_شوی VMS
# define SPL_FNAME_TMPL  "%s_%s.spl"
# define SPL_FNAME_جمع_کړه   "_add."
# define SPL_FNAME_ASCII "_ascii."
#وإلا
# define SPL_FNAME_TMPL  "%s.%s.spl"
# define SPL_FNAME_جمع_کړه   ".add."
# define SPL_FNAME_ASCII ".ascii."
#د_که_پای

// Flags used for a word.  Only the lowest byte can be used, the region byte
// comes above it.
#تعریف_کړه WF_REGION   0x01	// region byte follows
#تعریف_کړه WF_ONECAP   0x02	// word with one capital (or all capitals)
#تعریف_کړه WF_ALLCAP   0x04	// word must be all capitals
#تعریف_کړه WF_RARE	    0x08	// rare word
#تعریف_کړه WF_BANNED   0x10	// bad word
#تعریف_کړه WF_AFX	    0x20	// affix ID follows
#تعریف_کړه WF_FIXCAP   0x40	// keep-case word, allcap not allowed
#تعریف_کړه WF_KEEPCAP  0x80	// keep-case word

#تعریف_کړه WF_CAPMASK (WF_ONECAP | WF_ALLCAP | WF_KEEPCAP | WF_FIXCAP)

// for <flags2>, shifted up one byte to be used in wn_flags
#تعریف_کړه WF_HAS_AFF  0x0100	// word includes affix
#تعریف_کړه WF_NEEDCOMP 0x0200	// word only valid in compound
#تعریف_کړه WF_NOSUGGEST 0x0400	// word not to be suggested
#تعریف_کړه WF_COMPROOT 0x0800	// already compounded word, COMPOUNDROOT
#تعریف_کړه WF_NOCOMPBEF 0x1000	// no compounding before this word
#تعریف_کړه WF_NOCOMPAFT 0x2000	// no compounding after this word

// flags for <pflags>
#تعریف_کړه WFP_RARE	    0x01	// rare prefix
#تعریف_کړه WFP_NC		    0x02	// prefix is not combining
#تعریف_کړه WFP_پورته		    0x04	// to-upper prefix
#تعریف_کړه WFP_COMPPERMIT	    0x08	// prefix with COMPOUNDPERMITFLAG
#تعریف_کړه WFP_COMPFORBID	    0x10	// prefix with COMPOUNDFORBIDFLAG

// Flags for postponed prefixes in "sl_pidxs".  Must be above affixID (one
// byte) and prefcondnr (two bytes).
#تعریف_کړه WF_RAREPFX  (WFP_RARE << 24)	// rare postponed prefix
#تعریف_کړه WF_PFX_NC   (WFP_NC << 24)	// non-combining postponed prefix
#تعریف_کړه WF_PFX_پورته   (WFP_پورته << 24)	// to-upper postponed prefix
#تعریف_کړه WF_PFX_COMPPERMIT (WFP_COMPPERMIT << 24) // postponed prefix with
						 // COMPOUNDPERMITFLAG
#تعریف_کړه WF_PFX_COMPFORBID (WFP_COMPFORBID << 24) // postponed prefix with
						 // COMPOUNDFORBIDFLAG

// flags for <compoptions>
#تعریف_کړه COMP_CHECKDUP		1	// CHECKCOMPOUNDDUP
#تعریف_کړه COMP_CHECKREP		2	// CHECKCOMPOUNDREP
#تعریف_کړه COMP_CHECKCASE		4	// CHECKCOMPOUNDCASE
#تعریف_کړه COMP_CHECKTRIPLE	8	// CHECKCOMPOUNDTRIPLE

// Info from "REP", "REPSAL" and "SAL" entries in ".aff" file used in si_rep,
// si_repsal, sl_rep, and si_sal.  Not for sl_sal!
// One replacement: from "ft_from" to "ft_to".
تعريفنوع بنية fromto_S
{
    char_u	*ft_from;
    char_u	*ft_منزل;
} fromto_T;

// Info from "SAL" entries in ".aff" file used in sl_sal.
// The info is split for quick processing by spell_soundfold().
// Note that "sm_oneof" and "sm_rules" point into sm_lead.
تعريفنوع بنية salitem_S
{
    char_u	*sm_lead;	// leading letters
    صحيح		sm_leadlen;	// length of "sm_lead"
    char_u	*sm_oneof;	// letters from () or NULL
    char_u	*sm_قواعد;	// rules like ^, $, priority
    char_u	*sm_منزل;		// replacement.
    صحيح		*sm_lead_w;	// wide character copy of "sm_lead"
    صحيح		*sm_oneof_w;	// wide character copy of "sm_oneof"
    صحيح		*sm_منزل_w;	// wide character copy of "sm_to"
} salitem_T;

// Values for SP_*ERROR are negative, positive values are used by
// read_cnt_string().
#تعریف_کړه	SP_TRUNCERROR	(-1)	// spell file truncated error
#تعریف_کړه	SP_FORMERROR	(-2)	// format error in spell file
#تعریف_کړه SP_OTHERERROR	(-3)	// other error while reading spell file

/*
 * Structure used in "b_langp", filled from 'spelllang'.
 */
تعريفنوع بنية langp_S
{
    slang_T	*lp_slang;	// info for this language
    slang_T	*lp_sallang;	// language used for sound folding or NULL
    slang_T	*lp_replang;	// language used for REP items or NULL
    صحيح		lp_region;	// bitmask for region or REGION_ALL
} langp_T;

#تعریف_کړه LANGP_ثبت(ga, i)	(((langp_T *)(ga).ga_معلومات) + (i))

#تعریف_کړه VIMSUGMAGIC "VIMsug"	// string at start of Vim .sug file
#تعریف_کړه VIMSUGMAGICL 6
#تعریف_کړه VIMSUGVERSION 1

/*
 * The tables used for recognizing word characters according to spelling.
 * These are only used for the first 256 characters of 'encoding'.
 */
تعريفنوع بنية spelltab_S
{
    char_u  st_isw[256];	// flags: is word char
    char_u  st_isu[256];	// flags: is uppercase char
    char_u  st_fold[256];	// chars: folded case
    char_u  st_پاسنی[256];	// chars: upper case
} spelltab_T;

/*
 * Use our own character-case definitions, because the current locale may
 * differ from what the .spl file uses.
 * These must not be called with negative number!
 */
#إذا defined(HAVE_WCHAR_H)
# include <wchar.h>	    // for towupper() and towlower()
#د_که_پای
// Multi-byte implementation.  For Unicode we can call utf_*(), but don't do
// that for ASCII, because we don't want to use 'casemap' here.  Otherwise use
// the "w" library function for characters above 255 if available.
#که_تعریف_شوی HAVE_TOWLOWER
# define SPELL_TOFOLD(c) (enc_utf8 && (c) >= 128 ? utf_fold(c) \
	    : (c) < 256 ? (صحيح)spelltab.st_fold[c] : (صحيح)towlower(c))
#وإلا
# define SPELL_TOFOLD(c) (enc_utf8 && (c) >= 128 ? utf_fold(c) \
	    : (c) < 256 ? (صحيح)spelltab.st_fold[c] : (c))
#د_که_پای

#که_تعریف_شوی HAVE_TOWUPPER
# define SPELL_TOUPPER(c) (enc_utf8 && (c) >= 128 ? utf_toupper(c) \
	    : (c) < 256 ? (صحيح)spelltab.st_پاسنی[c] : (صحيح)towupper(c))
#وإلا
# define SPELL_TOUPPER(c) (enc_utf8 && (c) >= 128 ? utf_toupper(c) \
	    : (c) < 256 ? (صحيح)spelltab.st_پاسنی[c] : (c))
#د_که_پای

#که_تعریف_شوی HAVE_ISWUPPER
# define SPELL_ISUPPER(c) (enc_utf8 && (c) >= 128 ? utf_isupper(c) \
	    : (c) < 256 ? spelltab.st_isu[c] : iswupper(c))
#وإلا
# define SPELL_ISUPPER(c) (enc_utf8 && (c) >= 128 ? utf_isupper(c) \
	    : (c) < 256 ? spelltab.st_isu[c] : (FALSE))
#د_که_پای

#که_تعریف_شوی FEAT_SPELL
# ifdef دننه_SPELL_C
#  define SPELL_EXTERN
#  define SPELL_INIT(x) x
# وإلا
#  define SPELL_EXTERN خارجي
#  define SPELL_INIT(x)
# endif

// First language that is loaded, start of the linked list of loaded
// languages.
SPELL_EXTERN slang_T	*لومړی_lang SPELL_INIT(= عدم);

// file used for "zG" and "zW"
SPELL_EXTERN char_u	*int_wordlist SPELL_INIT(= عدم);

SPELL_EXTERN spelltab_T   spelltab;
SPELL_EXTERN صحيح	  did_سېټ_spelltab;

// Values for "what" argument of spell_add_word()
# define SPELL_جمع_کړه_GOOD	0
# define SPELL_جمع_کړه_BAD	1
# define SPELL_جمع_کړه_RARE	2

تعريفنوع بنية wordcount_S
{
    short_u	wc_شمېر;	    // nr of times word was seen
    char_u	wc_کلمه[1];	    // word, actually longer
} wordcount_T;

# define WC_کلی_OFF  offsetof(wordcount_T, wc_کلمه)
# define HI2WC(hi)     ((wordcount_T *)((hi)->hi_کلی - WC_کلی_OFF))
# define MAXWORDCOUNT 0xffff

// Remember what "z?" replaced.
SPELL_EXTERN char_u	*repl_from SPELL_INIT(= عدم);
SPELL_EXTERN char_u	*repl_منزل SPELL_INIT(= عدم);
#د_که_پای
