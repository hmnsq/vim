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
#ਜੇ 0
# define SPELL_PRINTTREE
#ਜੇ_ਅੰਤ

// Use SPELL_COMPRESS_ALWAYS for debugging: compress the word tree after
// adding a word.  Only use it for small word lists!
#ਜੇ 0
# define SPELL_COMPRESS_ALWAYS
#ਜੇ_ਅੰਤ

// Use DEBUG_TRIEWALK to print the changes made in suggest_trie_walk() for a
// specific word.
#ਜੇ 0
# define DEBUG_TRIEWALK
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXWLEN 254		// Assume max. word len is this many bytes.
				// Some places assume a word length fits in a
				// byte, thus it can't be above 255.
				// Must be >= PFD_NOTSPECIAL.

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXREGIONS 8		// Number of regions supported.

// Type used for indexes in the word tree need to be at least 4 bytes.  If int
// is 8 bytes we could use something smaller, but what?
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ idx_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ salfirst_T;

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ slang_S slang_T;
ਬਣਤਰ slang_S
{
    slang_T	*sl_ਅਗਲਾ;	// next language
    char_u	*sl_name;	// language name "en", "en.rare", "nl", etc.
    char_u	*sl_fname;	// name of .spl file
    ਪੂਰਨ		sl_ਜੋੜੋ;		// TRUE if it's a .add file.

    char_u	*sl_fbyts;	// case-folded word bytes
    ਲੰਮਾ	sl_fbyts_len;	// length of sl_fbyts
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

    ਪੂਰਨ		sl_compmax;	// COMPOUNDWORDMAX (default: MAXWLEN)
    ਪੂਰਨ		sl_compminlen;	// COMPOUNDMIN (default: 0)
    ਪੂਰਨ		sl_compsylmax;	// COMPOUNDSYLMAX (default: MAXWLEN)
    ਪੂਰਨ		sl_compoptions;	// COMP_* flags
    garray_T	sl_comppat;	// CHECKCOMPOUNDPATTERN items
    regprog_T	*sl_compprog;	// COMPOUNDRULE turned into a regexp progrm
				// (NULL when no compounding)
    char_u	*sl_comprules;	// all COMPOUNDRULE concatenated (or NULL)
    char_u	*sl_compstartflags; // flags for first compound word
    char_u	*sl_compallflags; // all flags for compound words
    char_u	sl_nobreak;	// When TRUE: no spaces between words
    char_u	*sl_syllable;	// SYLLABLE repeatable chars or NULL
    garray_T	sl_syl_ਚੀਜ਼ਾਂ;	// syllable items

    ਪੂਰਨ		sl_prefixcnt;	// number of items in "sl_prefprog"
    regprog_T	**sl_prefprog;	// table with regprogs for prefixes

    garray_T	sl_rep;		// list of fromto_T entries from REP lines
    ਛੋਟਾ	sl_rep_ਪਹਿਲਾ[256];  // indexes where byte first appears, -1 if
				    // there is none
    garray_T	sl_sal;		// list of salitem_T entries from SAL lines
    salfirst_T	sl_sal_ਪਹਿਲਾ[256];  // indexes where byte first appears, -1 if
				    // there is none
    ਪੂਰਨ		sl_followup;	// SAL followup
    ਪੂਰਨ		sl_collapse;	// SAL collapse_result
    ਪੂਰਨ		sl_rem_accents;	// SAL remove_accents
    ਪੂਰਨ		sl_sofo;	// SOFOFROM and SOFOTO instead of SAL items:
				// "sl_sal_first" maps chars, when has_mbyte
				// "sl_sal" is a list of wide char lists.
    garray_T	sl_repsal;	// list of fromto_T entries from REPSAL lines
    ਛੋਟਾ	sl_repsal_ਪਹਿਲਾ[256];  // sl_rep_first for REPSAL lines
    ਪੂਰਨ		sl_nosplitsugs;	// don't suggest splitting a word
    ਪੂਰਨ		sl_nocompoundsugs; // don't suggest compounding

    // Info from the .sug file.  Loaded on demand.
    time_t	sl_sugtime;	// timestamp for .sug file
    char_u	*sl_sbyts;	// soundfolded word bytes
    ਲੰਮਾ	sl_sbyts_len;	// length of sl_sbyts
    idx_T	*sl_sidxs;	// soundfolded word indexes
    buf_T	*sl_sugbuf;	// buffer with word number table
    ਪੂਰਨ		sl_sugloaded;	// TRUE when .sug file was loaded or failed to
				//ਲੱਦੋ

    ਪੂਰਨ		sl_has_map;	// TRUE if there is a MAP line
    hashtab_T	sl_map_hash;	// MAP for multi-byte chars
    ਪੂਰਨ		sl_map_array[256]; // MAP for first 256 chars
    hashtab_T	sl_sounddone;	// table with soundfolded words that have
				// handled, see add_sound_suggest()
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
# define SPL_FNAME_TMPL  "%s_%s.spl"
# define SPL_FNAME_ਜੋੜੋ   "_add."
# define SPL_FNAME_ASCII "_ascii."
#ਨਹੀਂਤਾਂ
# define SPL_FNAME_TMPL  "%s.%s.spl"
# define SPL_FNAME_ਜੋੜੋ   ".add."
# define SPL_FNAME_ASCII ".ascii."
#ਜੇ_ਅੰਤ

// Flags used for a word.  Only the lowest byte can be used, the region byte
// comes above it.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_REGION   0x01	// region byte follows
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_ONECAP   0x02	// word with one capital (or all capitals)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_ALLCAP   0x04	// word must be all capitals
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_RARE	    0x08	// rare word
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_BANNED   0x10	// bad word
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_AFX	    0x20	// affix ID follows
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_FIXCAP   0x40	// keep-case word, allcap not allowed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_KEEPCAP  0x80	// keep-case word

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_CAPMASK (WF_ONECAP | WF_ALLCAP | WF_KEEPCAP | WF_FIXCAP)

// for <flags2>, shifted up one byte to be used in wn_flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_HAS_AFF  0x0100	// word includes affix
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_NEEDCOMP 0x0200	// word only valid in compound
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_NOSUGGEST 0x0400	// word not to be suggested
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_COMPROOT 0x0800	// already compounded word, COMPOUNDROOT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_NOCOMPBEF 0x1000	// no compounding before this word
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_NOCOMPAFT 0x2000	// no compounding after this word

// flags for <pflags>
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WFP_RARE	    0x01	// rare prefix
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WFP_NC		    0x02	// prefix is not combining
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WFP_ਉੱਪਰ		    0x04	// to-upper prefix
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WFP_COMPPERMIT	    0x08	// prefix with COMPOUNDPERMITFLAG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WFP_COMPFORBID	    0x10	// prefix with COMPOUNDFORBIDFLAG

// Flags for postponed prefixes in "sl_pidxs".  Must be above affixID (one
// byte) and prefcondnr (two bytes).
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_RAREPFX  (WFP_RARE << 24)	// rare postponed prefix
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_PFX_NC   (WFP_NC << 24)	// non-combining postponed prefix
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_PFX_ਉੱਪਰ   (WFP_ਉੱਪਰ << 24)	// to-upper postponed prefix
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_PFX_COMPPERMIT (WFP_COMPPERMIT << 24) // postponed prefix with
						 // COMPOUNDPERMITFLAG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WF_PFX_COMPFORBID (WFP_COMPFORBID << 24) // postponed prefix with
						 // COMPOUNDFORBIDFLAG

// flags for <compoptions>
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMP_CHECKDUP		1	// CHECKCOMPOUNDDUP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMP_CHECKREP		2	// CHECKCOMPOUNDREP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMP_CHECKCASE		4	// CHECKCOMPOUNDCASE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMP_CHECKTRIPLE	8	// CHECKCOMPOUNDTRIPLE

// Info from "REP", "REPSAL" and "SAL" entries in ".aff" file used in si_rep,
// si_repsal, sl_rep, and si_sal.  Not for sl_sal!
// One replacement: from "ft_from" to "ft_to".
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ fromto_S
{
    char_u	*ft_from;
    char_u	*ft_ਮੰਜ਼ਿਲ;
} fromto_T;

// Info from "SAL" entries in ".aff" file used in sl_sal.
// The info is split for quick processing by spell_soundfold().
// Note that "sm_oneof" and "sm_rules" point into sm_lead.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ salitem_S
{
    char_u	*sm_lead;	// leading letters
    ਪੂਰਨ		sm_leadlen;	// length of "sm_lead"
    char_u	*sm_oneof;	// letters from () or NULL
    char_u	*sm_ਨਿਯਮਾਂ;	// rules like ^, $, priority
    char_u	*sm_ਮੰਜ਼ਿਲ;		// replacement.
    ਪੂਰਨ		*sm_lead_w;	// wide character copy of "sm_lead"
    ਪੂਰਨ		*sm_oneof_w;	// wide character copy of "sm_oneof"
    ਪੂਰਨ		*sm_ਮੰਜ਼ਿਲ_w;	// wide character copy of "sm_to"
} salitem_T;

// Values for SP_*ERROR are negative, positive values are used by
// read_cnt_string().
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	SP_TRUNCERROR	(-1)	// spell file truncated error
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	SP_FORMERROR	(-2)	// format error in spell file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_OTHERERROR	(-3)	// other error while reading spell file

/*
 * Structure used in "b_langp", filled from 'spelllang'.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ langp_S
{
    slang_T	*lp_slang;	// info for this language
    slang_T	*lp_sallang;	// language used for sound folding or NULL
    slang_T	*lp_replang;	// language used for REP items or NULL
    ਪੂਰਨ		lp_region;	// bitmask for region or REGION_ALL
} langp_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LANGP_ਇੰਦਰਾਜ(ga, i)	(((langp_T *)(ga).ga_ਅੰਕੜੇ) + (i))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIMSUGMAGIC "VIMsug"	// string at start of Vim .sug file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIMSUGMAGICL 6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIMSUGVERSION 1

/*
 * The tables used for recognizing word characters according to spelling.
 * These are only used for the first 256 characters of 'encoding'.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ spelltab_S
{
    char_u  st_isw[256];	// flags: is word char
    char_u  st_isu[256];	// flags: is uppercase char
    char_u  st_fold[256];	// chars: folded case
    char_u  st_ਉੱਤਲਾ[256];	// chars: upper case
} spelltab_T;

/*
 * Use our own character-case definitions, because the current locale may
 * differ from what the .spl file uses.
 * These must not be called with negative number!
 */
#ਜੇ defined(HAVE_WCHAR_H)
# include <wchar.h>	    // for towupper() and towlower()
#ਜੇ_ਅੰਤ
// Multi-byte implementation.  For Unicode we can call utf_*(), but don't do
// that for ASCII, because we don't want to use 'casemap' here.  Otherwise use
// the "w" library function for characters above 255 if available.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TOWLOWER
# define SPELL_TOFOLD(c) (enc_utf8 && (c) >= 128 ? utf_fold(c) \
	    : (c) < 256 ? (ਪੂਰਨ)spelltab.st_fold[c] : (ਪੂਰਨ)towlower(c))
#ਨਹੀਂਤਾਂ
# define SPELL_TOFOLD(c) (enc_utf8 && (c) >= 128 ? utf_fold(c) \
	    : (c) < 256 ? (ਪੂਰਨ)spelltab.st_fold[c] : (c))
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TOWUPPER
# define SPELL_TOUPPER(c) (enc_utf8 && (c) >= 128 ? utf_toupper(c) \
	    : (c) < 256 ? (ਪੂਰਨ)spelltab.st_ਉੱਤਲਾ[c] : (ਪੂਰਨ)towupper(c))
#ਨਹੀਂਤਾਂ
# define SPELL_TOUPPER(c) (enc_utf8 && (c) >= 128 ? utf_toupper(c) \
	    : (c) < 256 ? (ਪੂਰਨ)spelltab.st_ਉੱਤਲਾ[c] : (c))
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ISWUPPER
# define SPELL_ISUPPER(c) (enc_utf8 && (c) >= 128 ? utf_isupper(c) \
	    : (c) < 256 ? spelltab.st_isu[c] : iswupper(c))
#ਨਹੀਂਤਾਂ
# define SPELL_ISUPPER(c) (enc_utf8 && (c) >= 128 ? utf_isupper(c) \
	    : (c) < 256 ? spelltab.st_isu[c] : (FALSE))
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
# ifdef ਵਿੱਚ_SPELL_C
#  define SPELL_EXTERN
#  define SPELL_INIT(x) x
# ਨਹੀਂਤਾਂ
#  define SPELL_EXTERN ਬਾਹਰੀ
#  define SPELL_INIT(x)
# endif

// First language that is loaded, start of the linked list of loaded
// languages.
SPELL_EXTERN slang_T	*ਪਹਿਲਾ_lang SPELL_INIT(= ਨਿਰਾ);

// file used for "zG" and "zW"
SPELL_EXTERN char_u	*int_wordlist SPELL_INIT(= ਨਿਰਾ);

SPELL_EXTERN spelltab_T   spelltab;
SPELL_EXTERN ਪੂਰਨ	  did_ਸੈੱਟ_spelltab;

// Values for "what" argument of spell_add_word()
# define SPELL_ਜੋੜੋ_GOOD	0
# define SPELL_ਜੋੜੋ_BAD	1
# define SPELL_ਜੋੜੋ_RARE	2

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ wordcount_S
{
    short_u	wc_ਗਿਣਤੀ;	    // nr of times word was seen
    char_u	wc_ਸ਼ਬਦ[1];	    // word, actually longer
} wordcount_T;

# define WC_ਕੁੰਜੀ_OFF  offsetof(wordcount_T, wc_ਸ਼ਬਦ)
# define HI2WC(hi)     ((wordcount_T *)((hi)->hi_ਕੁੰਜੀ - WC_ਕੁੰਜੀ_OFF))
# define MAXWORDCOUNT 0xffff

// Remember what "z?" replaced.
SPELL_EXTERN char_u	*repl_from SPELL_INIT(= ਨਿਰਾ);
SPELL_EXTERN char_u	*repl_ਮੰਜ਼ਿਲ SPELL_INIT(= ਨਿਰਾ);
#ਜੇ_ਅੰਤ
