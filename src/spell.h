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
#ಆದರೆ 0
# define SPELL_PRINTTREE
#ಆದರೆ_ಅಂತ್ಯ

// Use SPELL_COMPRESS_ALWAYS for debugging: compress the word tree after
// adding a word.  Only use it for small word lists!
#ಆದರೆ 0
# define SPELL_COMPRESS_ALWAYS
#ಆದರೆ_ಅಂತ್ಯ

// Use DEBUG_TRIEWALK to print the changes made in suggest_trie_walk() for a
// specific word.
#ಆದರೆ 0
# define DEBUG_TRIEWALK
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು MAXWLEN 254		// Assume max. word len is this many bytes.
				// Some places assume a word length fits in a
				// byte, thus it can't be above 255.
				// Must be >= PFD_NOTSPECIAL.

#ವ್ಯಾಖ್ಯಾನಿಸು MAXREGIONS 8		// Number of regions supported.

// Type used for indexes in the word tree need to be at least 4 bytes.  If int
// is 8 bytes we could use something smaller, but what?
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ idx_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ salfirst_T;

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ slang_S slang_T;
ರಚನೆ slang_S
{
    slang_T	*sl_ಮುಂದಿನ;	// next language
    char_u	*sl_name;	// language name "en", "en.rare", "nl", etc.
    char_u	*sl_fname;	// name of .spl file
    ಪೂರ್ಣಾಂಕ		sl_ಕೂಡಿಸು;		// TRUE if it's a .add file.

    char_u	*sl_fbyts;	// case-folded word bytes
    ದೀರ್ಘ	sl_fbyts_len;	// length of sl_fbyts
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

    ಪೂರ್ಣಾಂಕ		sl_compmax;	// COMPOUNDWORDMAX (default: MAXWLEN)
    ಪೂರ್ಣಾಂಕ		sl_compminlen;	// COMPOUNDMIN (default: 0)
    ಪೂರ್ಣಾಂಕ		sl_compsylmax;	// COMPOUNDSYLMAX (default: MAXWLEN)
    ಪೂರ್ಣಾಂಕ		sl_compoptions;	// COMP_* flags
    garray_T	sl_comppat;	// CHECKCOMPOUNDPATTERN items
    regprog_T	*sl_compprog;	// COMPOUNDRULE turned into a regexp progrm
				// (NULL when no compounding)
    char_u	*sl_comprules;	// all COMPOUNDRULE concatenated (or NULL)
    char_u	*sl_compstartflags; // flags for first compound word
    char_u	*sl_compallflags; // all flags for compound words
    char_u	sl_nobreak;	// When TRUE: no spaces between words
    char_u	*sl_syllable;	// SYLLABLE repeatable chars or NULL
    garray_T	sl_syl_ವಸ್ತುಗಳು;	// syllable items

    ಪೂರ್ಣಾಂಕ		sl_prefixcnt;	// number of items in "sl_prefprog"
    regprog_T	**sl_prefprog;	// table with regprogs for prefixes

    garray_T	sl_rep;		// list of fromto_T entries from REP lines
    ಗಿಡ್ಡ	sl_rep_ಮೊದಲ[256];  // indexes where byte first appears, -1 if
				    // there is none
    garray_T	sl_sal;		// list of salitem_T entries from SAL lines
    salfirst_T	sl_sal_ಮೊದಲ[256];  // indexes where byte first appears, -1 if
				    // there is none
    ಪೂರ್ಣಾಂಕ		sl_followup;	// SAL followup
    ಪೂರ್ಣಾಂಕ		sl_collapse;	// SAL collapse_result
    ಪೂರ್ಣಾಂಕ		sl_rem_accents;	// SAL remove_accents
    ಪೂರ್ಣಾಂಕ		sl_sofo;	// SOFOFROM and SOFOTO instead of SAL items:
				// "sl_sal_first" maps chars, when has_mbyte
				// "sl_sal" is a list of wide char lists.
    garray_T	sl_repsal;	// list of fromto_T entries from REPSAL lines
    ಗಿಡ್ಡ	sl_repsal_ಮೊದಲ[256];  // sl_rep_first for REPSAL lines
    ಪೂರ್ಣಾಂಕ		sl_nosplitsugs;	// don't suggest splitting a word
    ಪೂರ್ಣಾಂಕ		sl_nocompoundsugs; // don't suggest compounding

    // Info from the .sug file.  Loaded on demand.
    time_t	sl_sugtime;	// timestamp for .sug file
    char_u	*sl_sbyts;	// soundfolded word bytes
    ದೀರ್ಘ	sl_sbyts_len;	// length of sl_sbyts
    idx_T	*sl_sidxs;	// soundfolded word indexes
    buf_T	*sl_sugbuf;	// buffer with word number table
    ಪೂರ್ಣಾಂಕ		sl_sugloaded;	// TRUE when .sug file was loaded or failed to
				//ತುಂಬು

    ಪೂರ್ಣಾಂಕ		sl_has_map;	// TRUE if there is a MAP line
    hashtab_T	sl_map_hash;	// MAP for multi-byte chars
    ಪೂರ್ಣಾಂಕ		sl_map_array[256]; // MAP for first 256 chars
    hashtab_T	sl_sounddone;	// table with soundfolded words that have
				// handled, see add_sound_suggest()
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
# define SPL_FNAME_TMPL  "%s_%s.spl"
# define SPL_FNAME_ಕೂಡಿಸು   "_add."
# define SPL_FNAME_ASCII "_ascii."
#ಇಲ್ಲದಿದ್ದರೆ
# define SPL_FNAME_TMPL  "%s.%s.spl"
# define SPL_FNAME_ಕೂಡಿಸು   ".add."
# define SPL_FNAME_ASCII ".ascii."
#ಆದರೆ_ಅಂತ್ಯ

// Flags used for a word.  Only the lowest byte can be used, the region byte
// comes above it.
#ವ್ಯಾಖ್ಯಾನಿಸು WF_REGION   0x01	// region byte follows
#ವ್ಯಾಖ್ಯಾನಿಸು WF_ONECAP   0x02	// word with one capital (or all capitals)
#ವ್ಯಾಖ್ಯಾನಿಸು WF_ALLCAP   0x04	// word must be all capitals
#ವ್ಯಾಖ್ಯಾನಿಸು WF_RARE	    0x08	// rare word
#ವ್ಯಾಖ್ಯಾನಿಸು WF_BANNED   0x10	// bad word
#ವ್ಯಾಖ್ಯಾನಿಸು WF_AFX	    0x20	// affix ID follows
#ವ್ಯಾಖ್ಯಾನಿಸು WF_FIXCAP   0x40	// keep-case word, allcap not allowed
#ವ್ಯಾಖ್ಯಾನಿಸು WF_KEEPCAP  0x80	// keep-case word

#ವ್ಯಾಖ್ಯಾನಿಸು WF_CAPMASK (WF_ONECAP | WF_ALLCAP | WF_KEEPCAP | WF_FIXCAP)

// for <flags2>, shifted up one byte to be used in wn_flags
#ವ್ಯಾಖ್ಯಾನಿಸು WF_HAS_AFF  0x0100	// word includes affix
#ವ್ಯಾಖ್ಯಾನಿಸು WF_NEEDCOMP 0x0200	// word only valid in compound
#ವ್ಯಾಖ್ಯಾನಿಸು WF_NOSUGGEST 0x0400	// word not to be suggested
#ವ್ಯಾಖ್ಯಾನಿಸು WF_COMPROOT 0x0800	// already compounded word, COMPOUNDROOT
#ವ್ಯಾಖ್ಯಾನಿಸು WF_NOCOMPBEF 0x1000	// no compounding before this word
#ವ್ಯಾಖ್ಯಾನಿಸು WF_NOCOMPAFT 0x2000	// no compounding after this word

// flags for <pflags>
#ವ್ಯಾಖ್ಯಾನಿಸು WFP_RARE	    0x01	// rare prefix
#ವ್ಯಾಖ್ಯಾನಿಸು WFP_NC		    0x02	// prefix is not combining
#ವ್ಯಾಖ್ಯಾನಿಸು WFP_ಮೇಲೆ		    0x04	// to-upper prefix
#ವ್ಯಾಖ್ಯಾನಿಸು WFP_COMPPERMIT	    0x08	// prefix with COMPOUNDPERMITFLAG
#ವ್ಯಾಖ್ಯಾನಿಸು WFP_COMPFORBID	    0x10	// prefix with COMPOUNDFORBIDFLAG

// Flags for postponed prefixes in "sl_pidxs".  Must be above affixID (one
// byte) and prefcondnr (two bytes).
#ವ್ಯಾಖ್ಯಾನಿಸು WF_RAREPFX  (WFP_RARE << 24)	// rare postponed prefix
#ವ್ಯಾಖ್ಯಾನಿಸು WF_PFX_NC   (WFP_NC << 24)	// non-combining postponed prefix
#ವ್ಯಾಖ್ಯಾನಿಸು WF_PFX_ಮೇಲೆ   (WFP_ಮೇಲೆ << 24)	// to-upper postponed prefix
#ವ್ಯಾಖ್ಯಾನಿಸು WF_PFX_COMPPERMIT (WFP_COMPPERMIT << 24) // postponed prefix with
						 // COMPOUNDPERMITFLAG
#ವ್ಯಾಖ್ಯಾನಿಸು WF_PFX_COMPFORBID (WFP_COMPFORBID << 24) // postponed prefix with
						 // COMPOUNDFORBIDFLAG

// flags for <compoptions>
#ವ್ಯಾಖ್ಯಾನಿಸು COMP_CHECKDUP		1	// CHECKCOMPOUNDDUP
#ವ್ಯಾಖ್ಯಾನಿಸು COMP_CHECKREP		2	// CHECKCOMPOUNDREP
#ವ್ಯಾಖ್ಯಾನಿಸು COMP_CHECKCASE		4	// CHECKCOMPOUNDCASE
#ವ್ಯಾಖ್ಯಾನಿಸು COMP_CHECKTRIPLE	8	// CHECKCOMPOUNDTRIPLE

// Info from "REP", "REPSAL" and "SAL" entries in ".aff" file used in si_rep,
// si_repsal, sl_rep, and si_sal.  Not for sl_sal!
// One replacement: from "ft_from" to "ft_to".
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ fromto_S
{
    char_u	*ft_from;
    char_u	*ft_ಗಮ್ಯ;
} fromto_T;

// Info from "SAL" entries in ".aff" file used in sl_sal.
// The info is split for quick processing by spell_soundfold().
// Note that "sm_oneof" and "sm_rules" point into sm_lead.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ salitem_S
{
    char_u	*sm_lead;	// leading letters
    ಪೂರ್ಣಾಂಕ		sm_leadlen;	// length of "sm_lead"
    char_u	*sm_oneof;	// letters from () or NULL
    char_u	*sm_ನಿಯಮಗಳು;	// rules like ^, $, priority
    char_u	*sm_ಗಮ್ಯ;		// replacement.
    ಪೂರ್ಣಾಂಕ		*sm_lead_w;	// wide character copy of "sm_lead"
    ಪೂರ್ಣಾಂಕ		*sm_oneof_w;	// wide character copy of "sm_oneof"
    ಪೂರ್ಣಾಂಕ		*sm_ಗಮ್ಯ_w;	// wide character copy of "sm_to"
} salitem_T;

// Values for SP_*ERROR are negative, positive values are used by
// read_cnt_string().
#ವ್ಯಾಖ್ಯಾನಿಸು	SP_TRUNCERROR	(-1)	// spell file truncated error
#ವ್ಯಾಖ್ಯಾನಿಸು	SP_FORMERROR	(-2)	// format error in spell file
#ವ್ಯಾಖ್ಯಾನಿಸು SP_OTHERERROR	(-3)	// other error while reading spell file

/*
 * Structure used in "b_langp", filled from 'spelllang'.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ langp_S
{
    slang_T	*lp_slang;	// info for this language
    slang_T	*lp_sallang;	// language used for sound folding or NULL
    slang_T	*lp_replang;	// language used for REP items or NULL
    ಪೂರ್ಣಾಂಕ		lp_region;	// bitmask for region or REGION_ALL
} langp_T;

#ವ್ಯಾಖ್ಯಾನಿಸು LANGP_ನಮೂದು(ga, i)	(((langp_T *)(ga).ga_ದತ್ತಾಂಶ) + (i))

#ವ್ಯಾಖ್ಯಾನಿಸು VIMSUGMAGIC "VIMsug"	// string at start of Vim .sug file
#ವ್ಯಾಖ್ಯಾನಿಸು VIMSUGMAGICL 6
#ವ್ಯಾಖ್ಯಾನಿಸು VIMSUGVERSION 1

/*
 * The tables used for recognizing word characters according to spelling.
 * These are only used for the first 256 characters of 'encoding'.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ spelltab_S
{
    char_u  st_isw[256];	// flags: is word char
    char_u  st_isu[256];	// flags: is uppercase char
    char_u  st_fold[256];	// chars: folded case
    char_u  st_ಊರ್ಧ್ವ[256];	// chars: upper case
} spelltab_T;

/*
 * Use our own character-case definitions, because the current locale may
 * differ from what the .spl file uses.
 * These must not be called with negative number!
 */
#ಆದರೆ defined(HAVE_WCHAR_H)
# include <wchar.h>	    // for towupper() and towlower()
#ಆದರೆ_ಅಂತ್ಯ
// Multi-byte implementation.  For Unicode we can call utf_*(), but don't do
// that for ASCII, because we don't want to use 'casemap' here.  Otherwise use
// the "w" library function for characters above 255 if available.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TOWLOWER
# define SPELL_TOFOLD(c) (enc_utf8 && (c) >= 128 ? utf_fold(c) \
	    : (c) < 256 ? (ಪೂರ್ಣಾಂಕ)spelltab.st_fold[c] : (ಪೂರ್ಣಾಂಕ)towlower(c))
#ಇಲ್ಲದಿದ್ದರೆ
# define SPELL_TOFOLD(c) (enc_utf8 && (c) >= 128 ? utf_fold(c) \
	    : (c) < 256 ? (ಪೂರ್ಣಾಂಕ)spelltab.st_fold[c] : (c))
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TOWUPPER
# define SPELL_TOUPPER(c) (enc_utf8 && (c) >= 128 ? utf_toupper(c) \
	    : (c) < 256 ? (ಪೂರ್ಣಾಂಕ)spelltab.st_ಊರ್ಧ್ವ[c] : (ಪೂರ್ಣಾಂಕ)towupper(c))
#ಇಲ್ಲದಿದ್ದರೆ
# define SPELL_TOUPPER(c) (enc_utf8 && (c) >= 128 ? utf_toupper(c) \
	    : (c) < 256 ? (ಪೂರ್ಣಾಂಕ)spelltab.st_ಊರ್ಧ್ವ[c] : (c))
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ISWUPPER
# define SPELL_ISUPPER(c) (enc_utf8 && (c) >= 128 ? utf_isupper(c) \
	    : (c) < 256 ? spelltab.st_isu[c] : iswupper(c))
#ಇಲ್ಲದಿದ್ದರೆ
# define SPELL_ISUPPER(c) (enc_utf8 && (c) >= 128 ? utf_isupper(c) \
	    : (c) < 256 ? spelltab.st_isu[c] : (FALSE))
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
# ifdef ಒಳ_SPELL_C
#  define SPELL_EXTERN
#  define SPELL_INIT(x) x
# ಇಲ್ಲದಿದ್ದರೆ
#  define SPELL_EXTERN ಬಾಹ್ಯ
#  define SPELL_INIT(x)
# endif

// First language that is loaded, start of the linked list of loaded
// languages.
SPELL_EXTERN slang_T	*ಮೊದಲ_lang SPELL_INIT(= ಖಾಲಿ);

// file used for "zG" and "zW"
SPELL_EXTERN char_u	*int_wordlist SPELL_INIT(= ಖಾಲಿ);

SPELL_EXTERN spelltab_T   spelltab;
SPELL_EXTERN ಪೂರ್ಣಾಂಕ	  did_ಗಣ_spelltab;

// Values for "what" argument of spell_add_word()
# define SPELL_ಕೂಡಿಸು_GOOD	0
# define SPELL_ಕೂಡಿಸು_BAD	1
# define SPELL_ಕೂಡಿಸು_RARE	2

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ wordcount_S
{
    short_u	wc_ಎಣಿಕೆ;	    // nr of times word was seen
    char_u	wc_ಶಬ್ದ[1];	    // word, actually longer
} wordcount_T;

# define WC_ಕೀಲಿ_OFF  offsetof(wordcount_T, wc_ಶಬ್ದ)
# define HI2WC(hi)     ((wordcount_T *)((hi)->hi_ಕೀಲಿ - WC_ಕೀಲಿ_OFF))
# define MAXWORDCOUNT 0xffff

// Remember what "z?" replaced.
SPELL_EXTERN char_u	*repl_from SPELL_INIT(= ಖಾಲಿ);
SPELL_EXTERN char_u	*repl_ಗಮ್ಯ SPELL_INIT(= ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
