/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * Highlighting stuff.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SG_ਪਦ		1	// term has been set
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SG_CTERM	2	// cterm has been set
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SG_GUI		4	// gui has been set
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SG_LINK		8	// link has been set

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_SYN_NAME	200

/*
 * The "term", "cterm" and "gui" arguments can be any combination of the
 * following names, separated by commas (but no spaces!).
 */
// must be sorted by the 'value' field because it is used by bsearch()!
// note: inverse and reverse use the same key
ਸਥਿਰ keyvalue_T highlight_tab[] = {
    KEYVALUE_ਇੰਦਰਾਜ(HL_BOLD, "bold"),			    // index 0
    KEYVALUE_ਇੰਦਰਾਜ(HL_INVERSE, "inverse"),		    // index 1
    KEYVALUE_ਇੰਦਰਾਜ(HL_ITALIC, "italic"),		    // index 2
    KEYVALUE_ਇੰਦਰਾਜ(HL_NOCOMBINE, "nocombine"),		    // index 3
    KEYVALUE_ਇੰਦਰਾਜ(HL_ਸਾਧਾਰਨ, "NONE"),			    // index 4
    KEYVALUE_ਇੰਦਰਾਜ(HL_INVERSE, "reverse"),		    // index 5
    KEYVALUE_ਇੰਦਰਾਜ(HL_STANDOUT, "standout"),		    // index 6
    KEYVALUE_ਇੰਦਰਾਜ(HL_STRIKETHROUGH, "strikethrough"),	    // index 7
    KEYVALUE_ਇੰਦਰਾਜ(HL_UNDERCURL, "undercurl"),		    // index 8
    KEYVALUE_ਇੰਦਰਾਜ(HL_UNDERDASHED, "underdashed"),	    // index 9
    KEYVALUE_ਇੰਦਰਾਜ(HL_UNDERDOTTED, "underdotted"),	    // index 10
    KEYVALUE_ਇੰਦਰਾਜ(HL_UNDERDOUBLE, "underdouble"),	    // index 11
    KEYVALUE_ਇੰਦਰਾਜ(HL_UNDERLINE, "underline")		    // index 12
};

// this table is used to display highlight names in the "correct" sequence.
// keep this in sync with highlight_tab[].
ਸਥਿਰ keyvalue_T *highlight_ਸੂਚਕਾਂਕ_tab[] = {
    &highlight_tab[0],					    // HL_BOLD
    &highlight_tab[6],					    // HL_STANDOUT
    &highlight_tab[12],					    // HL_UNDERLINE
    &highlight_tab[8],					    // HL_UNDERCURL
    &highlight_tab[11],					    // HL_UNDERDOUBLE
    &highlight_tab[10],					    // HL_UNDERDOTTED
    &highlight_tab[9],					    // HL_UNDERDASHED
    &highlight_tab[2],					    // HL_ITALIC
    &highlight_tab[5],					    // HL_REVERSE
    &highlight_tab[1],					    // HL_INVERSE
    &highlight_tab[3],					    // HL_NOCOMBINE
    &highlight_tab[7],					    // HL_STRIKETHROUGH
    &highlight_tab[4]					    // HL_NORMAL
};

// length of all attribute names, plus commas, together (and a bit more)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ATTR_LEN 120

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATTR_COMBINE(attr_a, attr_b) ((((attr_b) & HL_NOCOMBINE) ? (attr_b) : (attr_a)) | (attr_b))

ਗਿਣਤੀ {
    BLACK = 0,
    DARKBLUE,
    DARKGREEN,
    DARKCYAN,
    DARKRED,
    DARKMAGENTA,
    BROWN,
    DARKYELLOW,
    ਸਲੇਟੀ,
    GREY,
    LIGHTGRAY,
    LIGHTGREY,
    DARKGRAY,
    DARKGREY,
    BLUE,
    LIGHTBLUE,
    GREEN,
    LIGHTGREEN,
    CYAN,
    LIGHTCYAN,
    RED,
    LIGHTRED,
    MAGENTA,
    LIGHTMAGENTA,
    YELLOW,
    LIGHTYELLOW,
    WHITE,
    NONE
};

// must be sorted by the 'value' field because it is used by bsearch()!
ਸਥਿਰ keyvalue_T color_name_tab[] = {
    KEYVALUE_ਇੰਦਰਾਜ(BLACK, "Black"),
    KEYVALUE_ਇੰਦਰਾਜ(BLUE, "Blue"),
    KEYVALUE_ਇੰਦਰਾਜ(BROWN, "Brown"),
    KEYVALUE_ਇੰਦਰਾਜ(CYAN, "Cyan"),
    KEYVALUE_ਇੰਦਰਾਜ(DARKBLUE, "DarkBlue"),
    KEYVALUE_ਇੰਦਰਾਜ(DARKCYAN, "DarkCyan"),
    KEYVALUE_ਇੰਦਰਾਜ(DARKGRAY, "DarkGray"),
    KEYVALUE_ਇੰਦਰਾਜ(DARKGREEN, "DarkGreen"),
    KEYVALUE_ਇੰਦਰਾਜ(DARKGREY, "DarkGrey"),
    KEYVALUE_ਇੰਦਰਾਜ(DARKMAGENTA, "DarkMagenta"),
    KEYVALUE_ਇੰਦਰਾਜ(DARKRED, "DarkRed"),
    KEYVALUE_ਇੰਦਰਾਜ(DARKYELLOW, "DarkYellow"),
    KEYVALUE_ਇੰਦਰਾਜ(ਸਲੇਟੀ, "Gray"),
    KEYVALUE_ਇੰਦਰਾਜ(GREEN, "Green"),
    KEYVALUE_ਇੰਦਰਾਜ(GREY, "Grey"),
    KEYVALUE_ਇੰਦਰਾਜ(LIGHTBLUE, "LightBlue"),
    KEYVALUE_ਇੰਦਰਾਜ(LIGHTCYAN, "LightCyan"),
    KEYVALUE_ਇੰਦਰਾਜ(LIGHTGRAY, "LightGray"),
    KEYVALUE_ਇੰਦਰਾਜ(LIGHTGREEN, "LightGreen"),
    KEYVALUE_ਇੰਦਰਾਜ(LIGHTGREY, "LightGrey"),
    KEYVALUE_ਇੰਦਰਾਜ(LIGHTMAGENTA, "LightMagenta"),
    KEYVALUE_ਇੰਦਰਾਜ(LIGHTRED, "LightRed"),
    KEYVALUE_ਇੰਦਰਾਜ(LIGHTYELLOW, "LightYellow"),
    KEYVALUE_ਇੰਦਰਾਜ(MAGENTA, "Magenta"),
    KEYVALUE_ਇੰਦਰਾਜ(NONE, "NONE"),
    KEYVALUE_ਇੰਦਰਾਜ(RED, "Red"),
    KEYVALUE_ਇੰਦਰਾਜ(WHITE, "White"),
    KEYVALUE_ਇੰਦਰਾਜ(YELLOW, "Yellow")
};

/*
 * Structure that stores information about a highlight group.
 * The ID of a highlight group is also called group ID.  It is the index in
 * the highlight_ga array PLUS ONE.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	*sg_name;	// highlight group name
    char_u	*sg_name_u;	// uppercase of sg_name
    ਪੂਰਨ		sg_cleared;	// "hi clear" was used
// for normal terminals
    ਪੂਰਨ		sg_ਪਦ;	// "term=" highlighting attributes
    char_u	*sg_ਸ਼ੁਰੂ;	// terminal string for start highl
    char_u	*sg_stop;	// terminal string for stop highl
    ਪੂਰਨ		sg_ਪਦ_attr;	// Screen attr for term mode
// for color terminals
    ਪੂਰਨ		sg_cterm;	// "cterm=" highlighting attr
    ਪੂਰਨ		sg_cterm_bold;	// bold attr was set for light color
    ਪੂਰਨ		sg_cterm_fg;	// terminal fg color number + 1
    ਪੂਰਨ		sg_cterm_bg;	// terminal bg color number + 1
    ਪੂਰਨ		sg_cterm_ul;	// terminal ul color number + 1
    ਪੂਰਨ		sg_cterm_attr;	// Screen attr for color term mode
    ਪੂਰਨ		sg_cterm_font;	// terminal alternative font (0 for normal)
// for when using the GUI
#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    guicolor_T	sg_gui_fg;	// GUI foreground color handle
    guicolor_T	sg_gui_bg;	// GUI background color handle
    guicolor_T	sg_gui_sp;	// GUI special color handle
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    GuiFont	sg_font;	// GUI font handle
# ifdef FEAT_XFONTSET
    GuiFontset	sg_fontset;	// GUI fontset handle
# endif
    char_u	*sg_font_name;  // GUI font or fontset name
    ਪੂਰਨ		sg_gui_attr;    // Screen attr for GUI mode
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI) || defined(FEAT_EVAL)
// Store the sp color name for the GUI or synIDattr()
    ਪੂਰਨ		sg_gui;		// "gui=" highlighting attributes
    char_u	*sg_gui_fg_name;// GUI foreground color name
    char_u	*sg_gui_bg_name;// GUI background color name
    char_u	*sg_gui_sp_name;// GUI special color name
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		sg_link;	// link to this highlight group ID
    ਪੂਰਨ		sg_deflink;	// default link; restored in highlight_clear()
    ਪੂਰਨ		sg_ਸੈੱਟ;		// combination of SG_* flags
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    sctx_T	sg_deflink_sctx;  // script where the default link was set
    sctx_T	sg_script_ctx;	// script in which the group was last set
#ਜੇ_ਅੰਤ
} hl_ਸਮੂਹ_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ hl_overrides_S hl_overrides_T;
ਬਣਤਰ hl_overrides_S
{
    hl_override_T   *arr;   // May be NULL if "arr" was freed or no highlight
			    // overrides (all values set to default)
    ਪੂਰਨ		    len;
    hl_overrides_T  *ਅਗਲਾ;  // Used to handle recursive calls

    ਪੂਰਨ attr[HLF_ਗਿਣਤੀ];    // highlight_attr[] before being updated.
};

ਸਥਿਰ hl_overrides_T *overrides = ਨਿਰਾ;

// Synced with highlight_attr, each is the highlight group id for each attr. If
// an element is 0, then there is no attr for it. If an element is -1, then it
// means it was set to itself.
ਸਥਿਰ ਪੂਰਨ highlight_ids[HLF_ਗਿਣਤੀ];

// Same as highlight_attr[] but is not modified by highlight group overrides
ਸਥਿਰ ਪੂਰਨ highlight_attr_raw[HLF_ਗਿਣਤੀ];

ਸਥਿਰ ਪੂਰਨ hl_flags[HLF_ਗਿਣਤੀ] = HL_FLAGS;

// highlight groups for 'highlight' option
ਸਥਿਰ garray_T highlight_ga;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HL_ਸਾਰਣੀ()	((hl_ਸਮੂਹ_T *)((highlight_ga.ga_ਅੰਕੜੇ)))

// Wrapper struct for hashtable entries: stores the highlight group ID alongside
// the uppercase name used as a hash key.  Uses the same offsetof pattern as
// signgroup_T / HI2SG.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		hn_id;		// highlight group ID (1-based)
    char_u	hn_ਕੁੰਜੀ[1];	// uppercase name (stretchable array)
} hlname_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HLNAME_ਕੁੰਜੀ_OFF	offsetof(hlname_T, hn_ਕੁੰਜੀ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HI2HLNAME(hi)	((hlname_T *)((hi)->hi_ਕੁੰਜੀ - HLNAME_ਕੁੰਜੀ_OFF))

// hashtable for quick highlight group name lookup
ਸਥਿਰ hashtab_T highlight_ht;
ਸਥਿਰ bool	 highlight_ht_inited = false;

/*
 * An attribute number is the index in attr_table plus ATTR_OFF.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATTR_OFF (HL_ALL + 1)

ਸਥਿਰ ਖਾਲੀ syn_unadd_ਸਮੂਹ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_hl_attr(ਪੂਰਨ idx);
ਸਥਿਰ ਖਾਲੀ highlight_list_one(ਪੂਰਨ id);
ਸਥਿਰ ਪੂਰਨ highlight_list_arg(ਪੂਰਨ id, ਪੂਰਨ didh, ਪੂਰਨ type, ਪੂਰਨ iarg, char_u *sarg, ਅੱਖਰ *name);
ਸਥਿਰ ਪੂਰਨ syn_ਜੋੜੋ_ਸਮੂਹ(char_u *name);
ਸਥਿਰ ਪੂਰਨ syn_name2id_len(char_u *name, ਪੂਰਨ len);
ਸਥਿਰ ਪੂਰਨ hl_has_settings(ਪੂਰਨ idx, ਪੂਰਨ check_link);
ਸਥਿਰ ਖਾਲੀ highlight_clear(ਪੂਰਨ idx);

#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
ਸਥਿਰ ਖਾਲੀ gui_do_one_color(ਪੂਰਨ idx, ਪੂਰਨ do_menu, ਪੂਰਨ do_tooltip);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
ਸਥਿਰ ਪੂਰਨ  ਸੈੱਟ_ਸਮੂਹ_colors(char_u *name, guicolor_T *fgp, guicolor_T *bgp, ਪੂਰਨ do_menu, ਪੂਰਨ use_norm, ਪੂਰਨ do_tooltip);
ਸਥਿਰ ਖਾਲੀ hl_do_font(ਪੂਰਨ idx, char_u *arg, ਪੂਰਨ do_ਸਾਧਾਰਨ, ਪੂਰਨ do_menu, ਪੂਰਨ do_tooltip, ਪੂਰਨ free_font);
#ਜੇ_ਅੰਤ

/*
 * The default highlight groups.  These are compiled-in for fast startup and
 * they still work when the runtime files can't be found.
 * When making changes here, also change runtime/colors/default.vim!
 * The #ifdefs are needed to reduce the amount of static data.  Helps to make
 * the 16 bit DOS (museum) version compile.
 */
#ਜੇ defined(FEAT_GUI) || defined(FEAT_EVAL)
# define CENT(a, b) b
#ਨਹੀਂਤਾਂ
# define CENT(a, b) a
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਅੱਖਰ *(highlight_init_both[]) = {
    CENT("ErrorMsg term=standout ctermbg=DarkRed ctermfg=White",
	 "ErrorMsg term=standout ctermbg=DarkRed ctermfg=White guibg=Red guifg=White"),
    CENT("IncSearch term=reverse,bold,underline cterm=reverse",
	 "IncSearch term=reverse,bold,underline cterm=reverse gui=reverse"),
    CENT("ModeMsg term=bold cterm=bold",
	 "ModeMsg term=bold cterm=bold gui=bold"),
    CENT("NonText term=bold ctermfg=Blue",
	 "NonText term=bold ctermfg=Blue gui=bold guifg=Blue"),
    CENT("StatusLine term=reverse,bold cterm=reverse,bold",
	 "StatusLine term=reverse,bold cterm=reverse,bold gui=reverse,bold"),
    CENT("StatusLineNC term=reverse cterm=reverse",
	 "StatusLineNC term=reverse cterm=reverse gui=reverse"),
    "default link EndOfBuffer NonText",
    CENT("VertSplit term=reverse cterm=reverse",
	 "VertSplit term=reverse cterm=reverse gui=reverse"),
    "default link VertSplitNC VertSplit",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    CENT("VisualNOS term=underline,bold cterm=underline,bold",
	 "VisualNOS term=underline,bold cterm=underline,bold gui=underline,bold"),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    CENT("DiffText term=reverse cterm=bold ctermbg=Red ctermfg=Black",
	 "DiffText term=reverse cterm=bold ctermbg=Red ctermfg=Black gui=bold guibg=Red"),
    "default link DiffTextAdd DiffText",
#ਜੇ_ਅੰਤ
    CENT("PmenuSbar term=NONE ctermbg=Grey",
	 "PmenuSbar term=NONE ctermbg=Grey guibg=Grey"),
    CENT("TabLineSel term=bold cterm=bold",
	 "TabLineSel term=bold cterm=bold gui=bold"),
    CENT("TabLineFill term=reverse cterm=reverse",
	 "TabLineFill term=reverse cterm=reverse gui=reverse"),
    "default link TabPanel TabLine",
    "default link TabPanelSel TabLineSel",
    "default link TabPanelFill TabLineFill",
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    "Cursor guibg=fg guifg=bg",
    "lCursor guibg=fg guifg=bg", // should be different, but what?
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
    "TitleBar guibg=bg guifg=fg",
    "TitleBarNC guibg=NONE guifg=NONE",
#ਜੇ_ਅੰਤ
    "default link QuickFixLine Search",
    "default link CursorLineSign SignColumn",
    "default link CursorLineFold FoldColumn",
    "default link CurSearch Search",
    "default link PmenuKind Pmenu",
    "default link PmenuKindSel PmenuSel",
    "default link PmenuMatch Pmenu",
    "default link PmenuMatchSel PmenuSel",
    "default link PmenuExtra Pmenu",
    "default link PmenuExtraSel PmenuSel",
    "default link PmenuBorder Pmenu",
    "default link PopupSelected PmenuSel",
    "default link Popup Pmenu",
    "default link PopupBorder Pmenu",
    "default link PopupTitle Pmenu",
    "default link MessageWindow WarningMsg",
    "default link PopupNotification WarningMsg",
    "default link PreInsert Added",
    CENT("Normal cterm=NONE", "Normal gui=NONE"),
    ਨਿਰਾ
};

// Default colors only used with a light background.
ਸਥਿਰ ਅੱਖਰ *(highlight_init_light[]) = {
    CENT("Directory term=bold ctermfg=DarkBlue",
	 "Directory term=bold ctermfg=DarkBlue guifg=Blue"),
    CENT("LineNr term=NONE ctermfg=Brown",
	 "LineNr term=NONE ctermfg=Brown guifg=Brown"),
    CENT("CursorLineNr term=bold cterm=underline ctermfg=Brown",
	 "CursorLineNr term=bold cterm=underline ctermfg=Brown gui=bold guifg=Brown"),
    CENT("MoreMsg term=bold ctermfg=DarkGreen",
	 "MoreMsg term=bold ctermfg=DarkGreen gui=bold guifg=SeaGreen"),
    CENT("Question term=standout ctermfg=DarkGreen",
	 "Question term=standout ctermfg=DarkGreen gui=bold guifg=SeaGreen"),
    CENT("Search term=reverse ctermbg=Yellow ctermfg=NONE",
	 "Search term=reverse ctermbg=Yellow ctermfg=NONE guibg=Yellow guifg=NONE"),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    CENT("SpellBad term=reverse ctermbg=LightRed",
	 "SpellBad term=reverse ctermbg=LightRed guisp=Red gui=undercurl"),
    CENT("SpellCap term=reverse ctermbg=LightBlue",
	 "SpellCap term=reverse ctermbg=LightBlue guisp=Blue gui=undercurl"),
    CENT("SpellRare term=reverse ctermbg=LightMagenta",
	 "SpellRare term=reverse ctermbg=LightMagenta guisp=Magenta gui=undercurl"),
    CENT("SpellLocal term=underline ctermbg=Cyan",
	 "SpellLocal term=underline ctermbg=Cyan guisp=DarkCyan gui=undercurl"),
#ਜੇ_ਅੰਤ
    CENT("PmenuThumb term=reverse ctermbg=Black",
	 "PmenuThumb term=reverse ctermbg=Black guibg=Black"),
    CENT("PmenuShadow term=NONE ctermbg=Black ctermfg=DarkGrey",
	 "PmenuShadow term=NONE ctermbg=Black ctermfg=DarkGrey guibg=Black guifg=DarkGrey"),
    CENT("Pmenu term=reverse ctermbg=LightMagenta ctermfg=Black",
	 "Pmenu term=reverse ctermbg=LightMagenta ctermfg=Black guibg=LightMagenta"),
    CENT("PmenuSel term=underline ctermbg=LightGrey ctermfg=Black",
	 "PmenuSel term=underline ctermbg=LightGrey ctermfg=Black guibg=Grey"),
    CENT("SpecialKey term=NONE ctermfg=DarkBlue",
	 "SpecialKey term=NONE ctermfg=DarkBlue guifg=Blue"),
    CENT("Title term=bold ctermfg=DarkMagenta",
	 "Title term=bold ctermfg=DarkMagenta gui=bold guifg=Magenta"),
    CENT("WarningMsg term=standout ctermfg=DarkRed",
	 "WarningMsg term=standout ctermfg=DarkRed guifg=Red"),
    CENT("WildMenu term=underline ctermbg=Yellow ctermfg=Black",
	 "WildMenu term=underline ctermbg=Yellow ctermfg=Black guibg=Yellow guifg=Black"),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    CENT("Folded term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "Folded term=standout ctermbg=Grey ctermfg=DarkBlue guibg=LightGrey guifg=DarkBlue"),
    CENT("FoldColumn term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "FoldColumn term=standout ctermbg=Grey ctermfg=DarkBlue guibg=Grey guifg=DarkBlue"),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
    CENT("SignColumn term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "SignColumn term=standout ctermbg=Grey ctermfg=DarkBlue guibg=Grey guifg=DarkBlue"),
#ਜੇ_ਅੰਤ
    CENT("Visual ctermbg=Grey ctermfg=Black",
	 "Visual ctermbg=Grey ctermfg=Black guibg=LightGrey guifg=Black"),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    CENT("DiffAdd term=bold ctermbg=LightBlue ctermfg=Black",
	 "DiffAdd term=bold ctermbg=LightBlue ctermfg=Black guibg=LightBlue"),
    CENT("DiffChange term=bold ctermbg=LightMagenta ctermfg=Black",
	 "DiffChange term=bold ctermbg=LightMagenta ctermfg=Black guibg=LightMagenta"),
    CENT("DiffDelete term=bold ctermfg=Blue ctermbg=LightCyan",
	 "DiffDelete term=bold ctermfg=Blue ctermbg=LightCyan gui=bold guifg=Blue guibg=LightCyan"),
#ਜੇ_ਅੰਤ
    CENT("TabLine term=underline cterm=underline ctermfg=black ctermbg=LightGrey",
	 "TabLine term=underline cterm=underline ctermfg=black ctermbg=LightGrey gui=underline guibg=LightGrey"),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    CENT("CursorColumn term=reverse ctermbg=LightGrey",
	 "CursorColumn term=reverse ctermbg=LightGrey guibg=Grey90"),
    CENT("CursorLine term=underline cterm=underline",
	 "CursorLine term=underline cterm=underline guibg=Grey90"),
    CENT("ColorColumn term=reverse ctermbg=LightRed",
	 "ColorColumn term=reverse ctermbg=LightRed guibg=LightRed"),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    CENT("Conceal ctermbg=DarkGrey ctermfg=LightGrey",
	 "Conceal ctermbg=DarkGrey ctermfg=LightGrey guibg=DarkGrey guifg=LightGrey"),
#ਜੇ_ਅੰਤ
    CENT("MatchParen term=reverse ctermbg=Cyan",
	 "MatchParen term=reverse ctermbg=Cyan guibg=Cyan"),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    CENT("StatusLineTerm term=reverse,bold cterm=bold ctermfg=White ctermbg=DarkGreen",
	 "StatusLineTerm term=reverse,bold cterm=bold ctermfg=White ctermbg=DarkGreen gui=bold guifg=bg guibg=DarkGreen"),
    CENT("StatusLineTermNC term=reverse ctermfg=White ctermbg=DarkGreen",
	 "StatusLineTermNC term=reverse ctermfg=White ctermbg=DarkGreen guifg=bg guibg=DarkGreen"),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    CENT("ToolbarLine term=underline ctermbg=LightGrey",
	 "ToolbarLine term=underline ctermbg=LightGrey guibg=LightGrey"),
    CENT("ToolbarButton cterm=bold ctermfg=White ctermbg=DarkGrey",
	 "ToolbarButton cterm=bold ctermfg=White ctermbg=DarkGrey gui=bold guifg=White guibg=Grey40"),
#ਜੇ_ਅੰਤ
    ਨਿਰਾ
};

// Default colors only used with a dark background.
ਸਥਿਰ ਅੱਖਰ *(highlight_init_dark[]) = {
    CENT("Directory term=bold ctermfg=LightCyan",
	 "Directory term=bold ctermfg=LightCyan guifg=Cyan"),
    CENT("LineNr term=NONE ctermfg=Yellow",
	 "LineNr term=NONE ctermfg=Yellow guifg=Yellow"),
    CENT("CursorLineNr term=bold cterm=underline ctermfg=Yellow",
	 "CursorLineNr term=bold cterm=underline ctermfg=Yellow gui=bold guifg=Yellow"),
    CENT("MoreMsg term=bold ctermfg=LightGreen",
	 "MoreMsg term=bold ctermfg=LightGreen gui=bold guifg=SeaGreen"),
    CENT("Question term=standout ctermfg=LightGreen",
	 "Question term=standout ctermfg=LightGreen gui=bold guifg=Green"),
    CENT("Search term=reverse ctermbg=Yellow ctermfg=Black",
	 "Search term=reverse ctermbg=Yellow ctermfg=Black guibg=Yellow guifg=Black"),
    CENT("SpecialKey term=NONE ctermfg=LightBlue",
	 "SpecialKey term=NONE ctermfg=LightBlue guifg=Cyan"),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    CENT("SpellBad term=reverse ctermbg=Red",
	 "SpellBad term=reverse ctermbg=Red guisp=Red gui=undercurl"),
    CENT("SpellCap term=reverse ctermbg=Blue",
	 "SpellCap term=reverse ctermbg=Blue guisp=Blue gui=undercurl"),
    CENT("SpellRare term=reverse ctermbg=Magenta",
	 "SpellRare term=reverse ctermbg=Magenta guisp=Magenta gui=undercurl"),
    CENT("SpellLocal term=underline ctermbg=Cyan",
	 "SpellLocal term=underline ctermbg=Cyan guisp=Cyan gui=undercurl"),
#ਜੇ_ਅੰਤ
    CENT("PmenuThumb term=reverse ctermbg=White",
	 "PmenuThumb term=reverse ctermbg=White guibg=White"),
    CENT("PmenuShadow term=NONE ctermbg=Black ctermfg=DarkGrey",
	 "PmenuShadow term=NONE ctermbg=Black ctermfg=DarkGrey guibg=Black guifg=DarkGrey"),
    CENT("Pmenu term=reverse ctermbg=Magenta ctermfg=Black",
	 "Pmenu term=reverse ctermbg=Magenta ctermfg=Black guibg=Magenta"),
    CENT("PmenuSel term=underline ctermbg=Black ctermfg=DarkGrey",
	 "PmenuSel term=underline ctermbg=Black ctermfg=DarkGrey guibg=DarkGrey"),
    CENT("Title term=bold ctermfg=LightMagenta",
	 "Title term=bold ctermfg=LightMagenta gui=bold guifg=Magenta"),
    CENT("WarningMsg term=standout ctermfg=LightRed",
	 "WarningMsg term=standout ctermfg=LightRed guifg=Red"),
    CENT("WildMenu term=underline ctermbg=Yellow ctermfg=Black",
	 "WildMenu term=underline ctermbg=Yellow ctermfg=Black guibg=Yellow guifg=Black"),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    CENT("Folded term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "Folded term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=DarkGrey guifg=Cyan"),
    CENT("FoldColumn term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "FoldColumn term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=Grey guifg=Cyan"),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
    CENT("SignColumn term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "SignColumn term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=Grey guifg=Cyan"),
#ਜੇ_ਅੰਤ
    CENT("Visual ctermbg=Grey ctermfg=Black",
	 "Visual ctermbg=Grey ctermfg=Black guibg=#575757 guifg=LightGrey"),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    CENT("DiffAdd term=bold ctermbg=DarkBlue",
	 "DiffAdd term=bold ctermbg=DarkBlue guibg=DarkBlue"),
    CENT("DiffChange term=bold ctermbg=DarkMagenta",
	 "DiffChange term=bold ctermbg=DarkMagenta guibg=DarkMagenta"),
    CENT("DiffDelete term=bold ctermfg=Blue ctermbg=DarkCyan",
	 "DiffDelete term=bold ctermfg=Blue ctermbg=DarkCyan gui=bold guifg=Blue guibg=DarkCyan"),
#ਜੇ_ਅੰਤ
    CENT("TabLine term=underline cterm=underline ctermfg=white ctermbg=DarkGrey",
	 "TabLine term=underline cterm=underline ctermfg=white ctermbg=DarkGrey gui=underline guibg=DarkGrey"),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    CENT("CursorColumn term=reverse ctermbg=DarkGrey",
	 "CursorColumn term=reverse ctermbg=DarkGrey guibg=Grey40"),
    CENT("CursorLine term=underline cterm=underline",
	 "CursorLine term=underline cterm=underline guibg=Grey40"),
    CENT("ColorColumn term=reverse ctermbg=DarkRed",
	 "ColorColumn term=reverse ctermbg=DarkRed guibg=DarkRed"),
#ਜੇ_ਅੰਤ
    CENT("MatchParen term=reverse ctermbg=DarkCyan",
	 "MatchParen term=reverse ctermbg=DarkCyan guibg=DarkCyan"),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    CENT("Conceal ctermbg=DarkGrey ctermfg=LightGrey",
	 "Conceal ctermbg=DarkGrey ctermfg=LightGrey guibg=DarkGrey guifg=LightGrey"),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    CENT("StatusLineTerm term=reverse,bold cterm=bold ctermfg=Black ctermbg=LightGreen",
	 "StatusLineTerm term=reverse,bold cterm=bold ctermfg=Black ctermbg=LightGreen gui=bold guifg=bg guibg=LightGreen"),
    CENT("StatusLineTermNC term=reverse ctermfg=Black ctermbg=LightGreen",
	 "StatusLineTermNC term=reverse ctermfg=Black ctermbg=LightGreen guifg=bg guibg=LightGreen"),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    CENT("ToolbarLine term=underline ctermbg=DarkGrey",
	 "ToolbarLine term=underline ctermbg=DarkGrey guibg=Grey50"),
    CENT("ToolbarButton cterm=bold ctermfg=Black ctermbg=LightGrey",
	 "ToolbarButton cterm=bold ctermfg=Black ctermbg=LightGrey gui=bold guifg=Black guibg=LightGrey"),
#ਜੇ_ਅੰਤ
    ਨਿਰਾ
};

#ਜੇ defined(FEAT_SYN_HL)
/*
 * Returns the number of highlight groups.
 */
    ਪੂਰਨ
highlight_ਅੰਕ_groups(ਖਾਲੀ)
{
    ਵਾਪਸ highlight_ga.ga_len;
}

/*
 * Returns the name of a highlight group.
 */
    char_u *
highlight_ਸਮੂਹ_name(ਪੂਰਨ id)
{
    ਵਾਪਸ HL_ਸਾਰਣੀ()[id].sg_name;
}

/*
 * Returns the ID of the link to a highlight group.
 */
    ਪੂਰਨ
highlight_link_id(ਪੂਰਨ id)
{
    ਵਾਪਸ HL_ਸਾਰਣੀ()[id].sg_link;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ resolve_fallback_fg_ਮੰਜ਼ਿਲ_rgb(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ resolve_fallback_bg_ਮੰਜ਼ਿਲ_rgb(ਖਾਲੀ);

    ਖਾਲੀ
init_highlight(
    ਪੂਰਨ		both,	    // include groups where 'bg' doesn't matter
    ਪੂਰਨ		reset)	    // clear group first
{
    ਪੂਰਨ		i;
    ਅੱਖਰ	**pp;
    ਸਥਿਰ ਪੂਰਨ	had_both = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    char_u	*p;

    // Try finding the color scheme file.  Used when a color file was loaded
    // and 'background' or 't_Co' is changed.
    p = get_var_ਮੁੱਲ((char_u *)"g:colors_name");
    ਜੇ (p != ਨਿਰਾ)
    {
	// The value of g:colors_name could be freed when sourcing the script,
	// making "p" invalid, so copy it.
	char_u *copy_p = vim_strsave(p);
	ਪੂਰਨ    r;

	ਜੇ (copy_p != ਨਿਰਾ)
	{
	    r = ਲੱਦੋ_colors(copy_p);
	    vim_free(copy_p);
	    ਜੇ (r == OK)
		ਵਾਪਸ;
	}
    }

#ਜੇ_ਅੰਤ

    // Didn't use a color file, use the compiled-in colors.
    ਜੇ (both)
    {
	had_both = TRUE;
	pp = highlight_init_both;
	ਲਈ (i = 0; pp[i] != ਨਿਰਾ; ++i)
	    do_highlight((char_u *)pp[i], reset, TRUE);
    }
    ਨਹੀਂਤਾਂ ਜੇ (!had_both)
	// Don't do anything before the call with both == TRUE from main().
	// Not everything has been setup then, and that call will overrule
	// everything anyway.
	ਵਾਪਸ;

    ਜੇ (*p_bg == 'l')
	pp = highlight_init_light;
    ਨਹੀਂਤਾਂ
	pp = highlight_init_dark;
    ਲਈ (i = 0; pp[i] != ਨਿਰਾ; ++i)
	do_highlight((char_u *)pp[i], reset, TRUE);

    // Reverse looks ugly, but grey may not work for less than 8 colors.  Thus
    // let it depend on the number of colors available.
    ਜੇ (t_colors < 8)
	do_highlight((char_u *)"Visual term=reverse cterm=reverse ctermbg=NONE ctermfg=NONE",
		FALSE, TRUE);
    // With 8 colors brown is equal to yellow, need to use black for Search fg
    // to avoid Statement highlighted text disappears.
    // Clear the attributes, needed when changing the t_Co value.
    ਜੇ (t_colors <= 8)
    {
	ਜੇ (*p_bg == 'l')
	    do_highlight((char_u *)"Search ctermfg=black", FALSE, TRUE);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    // If syntax highlighting is enabled load the highlighting for it.
    ਜੇ (get_var_ਮੁੱਲ((char_u *)"g:syntax_on") != ਨਿਰਾ)
    {
	ਸਥਿਰ ਪੂਰਨ	recursive = 0;

	ਜੇ (recursive >= 5)
	    emsg(_(e_recursive_loop_loading_syncolor_vim));
	ਨਹੀਂਤਾਂ
	{
	    ++recursive;
	    (ਖਾਲੀ)ਸਰੋਤ_runtime((char_u *)"syntax/syncolor.vim", DIP_ALL);
	    --recursive;
	}
    }
#ਜੇ_ਅੰਤ
    resolve_fallback_fg_ਮੰਜ਼ਿਲ_rgb();
    resolve_fallback_bg_ਮੰਜ਼ਿਲ_rgb();
}

#ਜੇ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
/*
 * Load a default color list. Intended to support legacy color names but allows
 * the user to override the color values. Only loaded once.
 */
    ਸਥਿਰ ਖਾਲੀ
ਲੱਦੋ_default_colors_lists(ਖਾਲੀ)
{
    // Lacking a default color list isn't the end of the world but it is likely
    // an inconvenience so users should know when it is missing.
    ਜੇ (ਸਰੋਤ_runtime((char_u *)"colors/lists/default.vim", DIP_ALL) != OK)
	ਸੁਨੇਹਾ("failed to load colors/lists/default.vim");
}
#ਜੇ_ਅੰਤ

/*
 * Load color file "name".
 * Return OK for success, FAIL for failure.
 */
    ਪੂਰਨ
ਲੱਦੋ_colors(char_u *name)
{
    char_u	*buf;
    ਪੂਰਨ		retval = FAIL;
    ਸਥਿਰ ਪੂਰਨ	recursive = FALSE;

    // When being called recursively, this is probably because setting
    // 'background' caused the highlighting to be reloaded.  This means it is
    // working, thus we should return OK.
    ਜੇ (recursive)
	ਵਾਪਸ OK;

    recursive = TRUE;
    buf = alloc(STRLEN(name) + 12);
    ਜੇ (buf != ਨਿਰਾ)
    {
#ਜੇ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	ਲੱਦੋ_default_colors_lists();
#ਜੇ_ਅੰਤ
	apply_autocmds(EVENT_COLORSCHEMEPRE, name,
					       curbuf->b_fname, FALSE, curbuf);
	sprintf((ਅੱਖਰ *)buf, "colors/%s.vim", name);
	retval = ਸਰੋਤ_runtime(buf, DIP_ਸ਼ੁਰੂ + DIP_OPT);
	vim_free(buf);
	ਜੇ (retval == OK)
	    apply_autocmds(EVENT_COLORSCHEME, name, curbuf->b_fname,
								FALSE, curbuf);
    }
    recursive = FALSE;

    ਵਾਪਸ retval;
}

ਸਥਿਰ ਪੂਰਨ color_numbers_16[28] = {0, 1, 2, 3,
				 4, 5, 6, 6,
				 7, 7, 7, 7,
				 8, 8,
				 9, 9, 10, 10,
				 11, 11, 12, 12, 13,
				 13, 14, 14, 15, -1};
// for xterm with 88 colors...
ਸਥਿਰ ਪੂਰਨ color_numbers_88[28] = {0, 4, 2, 6,
				 1, 5, 32, 72,
				 84, 84, 7, 7,
				 82, 82,
				 12, 43, 10, 61,
				 14, 63, 9, 74, 13,
				 75, 11, 78, 15, -1};
// for xterm with 256 colors...
ਸਥਿਰ ਪੂਰਨ color_numbers_256[28] = {0, 4, 2, 6,
				 1, 5, 130, 3,
				 248, 248, 7, 7,
				 242, 242,
				 12, 81, 10, 121,
				 14, 159, 9, 224, 13,
				 225, 11, 229, 15, -1};
// for terminals with less than 16 colors...
ਸਥਿਰ ਪੂਰਨ color_numbers_8[28] = {0, 4, 2, 6,
				 1, 5, 3, 3,
				 7, 7, 7, 7,
				 0+8, 0+8,
				 4+8, 4+8, 2+8, 2+8,
				 6+8, 6+8, 1+8, 1+8, 5+8,
				 5+8, 3+8, 3+8, 7+8, -1};

/*
 * Lookup the "cterm" value to be used for color with index "idx" in
 * color_name_tab[].
 * "boldp" will be set to TRUE or FALSE for a foreground color when using 8
 * colors, otherwise it will be unchanged.
 */
    ਸਥਿਰ ਪੂਰਨ
lookup_color(ਪੂਰਨ idx, ਪੂਰਨ foreground, ਪੂਰਨ *boldp)
{
    ਪੂਰਨ		color = color_numbers_16[idx];
    char_u	*p;

    // Use the _16 table to check if it's a valid color name.
    ਜੇ (color < 0)
	ਵਾਪਸ -1;

    ਜੇ (t_colors == 8)
    {
	// t_Co is 8: use the 8 colors table
#ਜੇ defined(__QNXNTO__)
	// On qnx, the 8 & 16 color arrays are the same
	ਜੇ (STRNCMP(T_NAME, "qansi", 5) == 0)
	    color = color_numbers_16[idx];
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    color = color_numbers_8[idx];
	ਜੇ (foreground)
	{
	    // set/reset bold attribute to get light foreground
	    // colors (on some terminals, e.g. "linux")
	    ਜੇ (color & 8)
		*boldp = TRUE;
	    ਨਹੀਂਤਾਂ
		*boldp = FALSE;
	}
	color &= 7;	// truncate to 8 colors
    }
    ਨਹੀਂਤਾਂ ਜੇ (t_colors == 16 || t_colors == 88
					   || t_colors >= 256)
    {
	// Guess: if the termcap entry ends in 'm', it is
	// probably an xterm-like terminal.  Use the changed
	// order for colors.
	ਜੇ (*T_CAF != NUL)
	    p = T_CAF;
	ਨਹੀਂਤਾਂ
	    p = T_CSF;
	ਜੇ (*p != NUL && (t_colors > 256
			      || *(p + STRLEN(p) - 1) == 'm'))
	{
	    ਜੇ (t_colors == 88)
		color = color_numbers_88[idx];
	    ਨਹੀਂਤਾਂ ਜੇ (t_colors >= 256)
		color = color_numbers_256[idx];
	    ਨਹੀਂਤਾਂ
		color = color_numbers_8[idx];
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
	ਜੇ (t_colors >= 256 && color == 15 && is_mac_terminal)
	    // Terminal.app has a bug: 15 is light grey. Use white
	    // from the color cube instead.
	    color = 231;
#ਜੇ_ਅੰਤ
    }
    ਵਾਪਸ color;
}

/*
 * Link highlight group 'from_hg' to 'to_hg'.
 * 'dodefault' is set to TRUE for ":highlight default link".
 * 'forceit' is set to TRUE for ":highlight! link"
 * 'init' is set to TRUE when initializing all the highlight groups.
 */
    ਸਥਿਰ ਖਾਲੀ
highlight_ਸਮੂਹ_link(
	char_u	*from_hg,
	ਪੂਰਨ	from_len,
	char_u	*ਮੰਜ਼ਿਲ_hg,
	ਪੂਰਨ	ਮੰਜ਼ਿਲ_len,
	ਪੂਰਨ	dodefault,
	ਪੂਰਨ	forceit,
	ਪੂਰਨ	init)
{
    ਪੂਰਨ		from_id;
    ਪੂਰਨ		ਮੰਜ਼ਿਲ_id;
    hl_ਸਮੂਹ_T	*hlgroup = ਨਿਰਾ;

    from_id = syn_check_ਸਮੂਹ(from_hg, from_len);
    ਜੇ (STRNCMP(ਮੰਜ਼ਿਲ_hg, "NONE", 4) == 0)
	ਮੰਜ਼ਿਲ_id = 0;
    ਨਹੀਂਤਾਂ
	ਮੰਜ਼ਿਲ_id = syn_check_ਸਮੂਹ(ਮੰਜ਼ਿਲ_hg, ਮੰਜ਼ਿਲ_len);

    ਜੇ (from_id > 0)
    {
	hlgroup = &HL_ਸਾਰਣੀ()[from_id - 1];
	ਜੇ (dodefault && (forceit || hlgroup->sg_deflink == 0))
	{
	    hlgroup->sg_deflink = ਮੰਜ਼ਿਲ_id;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    hlgroup->sg_deflink_sctx = current_sctx;
	    hlgroup->sg_deflink_sctx.sc_lnum += SOURCING_LNUM;
#ਜੇ_ਅੰਤ
	}
    }

    ਜੇ (from_id > 0 && (!init || hlgroup->sg_ਸੈੱਟ == 0))
    {
	// Don't allow a link when there already is some highlighting
	// for the group, unless '!' is used
	ਜੇ (ਮੰਜ਼ਿਲ_id > 0 && !forceit && !init
		&& hl_has_settings(from_id - 1, dodefault))
	{
	    ਜੇ (SOURCING_NAME == ਨਿਰਾ && !dodefault)
		emsg(_(e_ਸਮੂਹ_has_settings_highlight_link_ignored));
	}
	ਨਹੀਂਤਾਂ ਜੇ (hlgroup->sg_link != ਮੰਜ਼ਿਲ_id
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		|| hlgroup->sg_script_ctx.sc_sid != current_sctx.sc_sid
#ਜੇ_ਅੰਤ
		|| hlgroup->sg_cleared)
	{
	    ਜੇ (!init)
		hlgroup->sg_ਸੈੱਟ |= SG_LINK;
	    hlgroup->sg_link = ਮੰਜ਼ਿਲ_id;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    hlgroup->sg_script_ctx = current_sctx;
	    hlgroup->sg_script_ctx.sc_lnum += SOURCING_LNUM;
#ਜੇ_ਅੰਤ
	    hlgroup->sg_cleared = FALSE;
	    redraw_all_later(UPD_SOME_VALID);

	    // Only call highlight_changed() once after multiple changes.
	    need_highlight_ਬਦਲਿਆ = TRUE;
	}
    }

}

/*
 * Reset all highlighting to the defaults. Removes all highlighting for the
 * groups added by the user.
 */
    ਸਥਿਰ ਖਾਲੀ
highlight_reset_all(ਖਾਲੀ)
{
    ਪੂਰਨ		idx;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // First, we do not destroy the old values, but allocate the new
    // ones and update the display. THEN we destroy the old values.
    // If we destroy the old values first, then the old values
    // (such as GuiFont's or GuiFontset's) will still be displayed but
    // invalid because they were free'd.
    ਜੇ (gui.ਵਿੱਚ_use)
    {
# ifdef FEAT_BEVAL_TIP
	gui_init_tooltip_font();
# endif
# ਜੇ defined(FEAT_MENU) && defined(FEAT_GUI_MOTIF)
	gui_init_menu_font();
# endif
    }
# ਜੇ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_X11)
    gui_mch_def_colors();
# endif
# ifdef FEAT_GUI_MSWIN
    ਜੇ (gui.ਵਿੱਚ_use)
	gui_mch_ਸੈੱਟ_titlebar_colors();
# endif
# ifdef FEAT_GUI_X11
#  ifdef FEAT_MENU

    // This only needs to be done when there is no Menu highlight
    // group defined by default, which IS currently the case.
    gui_mch_new_menu_colors();
#  endif
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	gui_new_scrollbar_colors();
#  ifdef FEAT_BEVAL_GUI
	gui_mch_new_tooltip_colors();
#  endif
#  ifdef FEAT_MENU
	gui_mch_new_menu_font();
#  endif
    }
# endif

    // Ok, we're done allocating the new default graphics items.
    // The screen should already be refreshed at this point.
    // It is now Ok to clear out the old data.
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    do_unlet((char_u *)"g:colors_name", TRUE);
#ਜੇ_ਅੰਤ
    restore_cterm_colors();

    // Clear all default highlight groups and load the defaults.
    ਲਈ (idx = 0; idx < highlight_ga.ga_len; ++idx)
	highlight_clear(idx);
    init_highlight(TRUE, TRUE);
#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ਜੇ (USE_24_ਦੋਆਧਾਰੀ_ਅੰਕ)
	highlight_gui_started();
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	highlight_ਬਦਲਿਆ();
    redraw_later_clear();
}

/*
 * Set the 'term' or 'cterm' or 'gui' attributes for the highlight group at
 * index 'idx'.
 * 'key' is one of 'TERM' or 'CTERM' or 'GUI'
 * 'arg' is the list of attribute names separated by comma.
 * 'init' is set to TRUE when initializing all the highlight groups.
 * Returns TRUE if the attributes are set.
 */
    ਸਥਿਰ ਪੂਰਨ
highlight_ਸੈੱਟ_termgui_attr(ਪੂਰਨ idx, char_u *ਕੁੰਜੀ, char_u *arg, ਪੂਰਨ init)
{
    ਪੂਰਨ		attr;
    size_t	off;
    keyvalue_T	ਨਿਸ਼ਾਨਾ;
    keyvalue_T	*ਇੰਦਰਾਜ;

    attr = 0;
    off = 0;
    ਨਿਸ਼ਾਨਾ.ਕੁੰਜੀ = 0;
    ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.ਲੰਮਾਈ = 0;	// not used, see cmp_keyvalue_value_ni()
    ਜਦੋਂ (arg[off] != NUL)
    {
	ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.string = arg + off;
	ਇੰਦਰਾਜ = (keyvalue_T *)bsearch(&ਨਿਸ਼ਾਨਾ, &highlight_tab,
		ARRAY_ਲੰਮਾਈ(highlight_tab), ਆਕਾਰ(highlight_tab[0]),
		cmp_keyvalue_ਮੁੱਲ_ni);
	ਜੇ (ਇੰਦਰਾਜ == ਨਿਰਾ)
	{
	    semsg(_(e_illegal_ਮੁੱਲ_str), arg);
	    ਵਾਪਸ FALSE;
	}

	attr |= ਇੰਦਰਾਜ->ਕੁੰਜੀ;
	off += ਇੰਦਰਾਜ->ਮੁੱਲ.ਲੰਮਾਈ;
	ਜੇ (arg[off] == ',')		// another one follows
	    ++off;
    }
    ਜੇ (*ਕੁੰਜੀ == 'T')
    {
	ਜੇ (!init || !(HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ & SG_ਪਦ))
	{
	    ਜੇ (!init)
		HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ |= SG_ਪਦ;
	    HL_ਸਾਰਣੀ()[idx].sg_ਪਦ = attr;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (*ਕੁੰਜੀ == 'C')
    {
	ਜੇ (!init || !(HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ & SG_CTERM))
	{
	    ਜੇ (!init)
		HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ |= SG_CTERM;
	    HL_ਸਾਰਣੀ()[idx].sg_cterm = attr;
	    HL_ਸਾਰਣੀ()[idx].sg_cterm_bold = FALSE;
	}
    }
#ਜੇ defined(FEAT_GUI) || defined(FEAT_EVAL)
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!init || !(HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ & SG_GUI))
	{
	    ਜੇ (!init)
		HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ |= SG_GUI;
	    HL_ਸਾਰਣੀ()[idx].sg_gui = attr;
	}
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ TRUE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
/*
 * Set the font for the highlight group at 'idx'.
 * 'arg' is the font name.
 * Returns TRUE if the font is changed.
 */
    ਸਥਿਰ ਪੂਰਨ
highlight_ਸੈੱਟ_font(
	ਪੂਰਨ	idx,
	char_u	*arg,
	ਪੂਰਨ	is_ਸਾਧਾਰਨ_ਸਮੂਹ,
	ਪੂਰਨ	is_menu_ਸਮੂਹ,
	ਪੂਰਨ	is_tooltip_ਸਮੂਹ)
{
    ਪੂਰਨ		did_change = FALSE;

    // in non-GUI fonts are simply ignored
    ਜੇ (HL_ਸਾਰਣੀ()[idx].sg_font_name != ਨਿਰਾ
	    && STRCMP(HL_ਸਾਰਣੀ()[idx].sg_font_name, arg) == 0)
    {
	// Font name didn't change, ignore.
    }
    ਨਹੀਂਤਾਂ ਜੇ (!gui.shell_created)
    {
	// GUI not started yet, always accept the name.
	vim_free(HL_ਸਾਰਣੀ()[idx].sg_font_name);
	HL_ਸਾਰਣੀ()[idx].sg_font_name = vim_strsave(arg);
	did_change = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	GuiFont ਆਰਜ਼ੀ_sg_font = HL_ਸਾਰਣੀ()[idx].sg_font;
# ifdef FEAT_XFONTSET
	GuiFontset ਆਰਜ਼ੀ_sg_fontset = HL_ਸਾਰਣੀ()[idx].sg_fontset;
# endif
	// First, save the current font/fontset.
	// Then try to allocate the font/fontset.
	// If the allocation fails, HL_TABLE()[idx].sg_font OR
	// sg_fontset will be set to NOFONT or NOFONTSET respectively.

	HL_ਸਾਰਣੀ()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
	HL_ਸਾਰਣੀ()[idx].sg_fontset = NOFONTSET;
# endif
	hl_do_font(idx, arg, is_ਸਾਧਾਰਨ_ਸਮੂਹ, is_menu_ਸਮੂਹ,
		is_tooltip_ਸਮੂਹ, FALSE);

# ifdef FEAT_XFONTSET
	ਜੇ (HL_ਸਾਰਣੀ()[idx].sg_fontset != NOFONTSET)
	{
	    // New fontset was accepted. Free the old one, if there
	    // was one.
	    gui_mch_free_fontset(ਆਰਜ਼ੀ_sg_fontset);
	    did_change = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    HL_ਸਾਰਣੀ()[idx].sg_fontset = ਆਰਜ਼ੀ_sg_fontset;
# endif
	ਜੇ (HL_ਸਾਰਣੀ()[idx].sg_font != NOFONT)
	{
	    // New font was accepted. Free the old one, if there was
	    // one.
	    gui_mch_free_font(ਆਰਜ਼ੀ_sg_font);
	    did_change = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    HL_ਸਾਰਣੀ()[idx].sg_font = ਆਰਜ਼ੀ_sg_font;
	// Update sg_font_name once here, after both font and fontset results
	// are known.
	ਜੇ (did_change)
	{
	    vim_free(HL_ਸਾਰਣੀ()[idx].sg_font_name);
	    HL_ਸਾਰਣੀ()[idx].sg_font_name = vim_strsave(arg);
	}
    }

    ਵਾਪਸ did_change;
}
#ਜੇ_ਅੰਤ

/*
 * Set the cterm foreground color for the Normal highlight group to "color" and
 * the bold attribute to "bold".
 */
    ਸਥਿਰ ਖਾਲੀ
hl_ਸੈੱਟ_ctermfg_ਸਾਧਾਰਨ_ਸਮੂਹ(ਪੂਰਨ color, ਪੂਰਨ bold)
{
    cterm_ਸਾਧਾਰਨ_fg_color = color + 1;
    cterm_ਸਾਧਾਰਨ_fg_bold = bold;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Don't do this if the GUI is used.
    ਜੇ (!gui.ਵਿੱਚ_use && !gui.starting)
#ਜੇ_ਅੰਤ
    {
	ਸੈੱਟ_must_redraw(UPD_CLEAR);
	ਜੇ (termcap_ਸਰਗਰਮ && color >= 0)
	    ਪਦ_fg_color(color);
    }
}

/*
 * Set the cterm foreground color for the highlight group at 'idx' to 'color'.
 */
    ਸਥਿਰ ਖਾਲੀ
highlight_ਸੈੱਟ_ctermfg(ਪੂਰਨ idx, ਪੂਰਨ color, ਪੂਰਨ is_ਸਾਧਾਰਨ_ਸਮੂਹ)
{
    HL_ਸਾਰਣੀ()[idx].sg_cterm_fg = color + 1;
    ਜੇ (is_ਸਾਧਾਰਨ_ਸਮੂਹ)
	hl_ਸੈੱਟ_ctermfg_ਸਾਧਾਰਨ_ਸਮੂਹ(color,
					(HL_ਸਾਰਣੀ()[idx].sg_cterm & HL_BOLD));
}

/*
 * Set the cterm background color for the Normal highlight group to "color".
 */
    ਸਥਿਰ ਖਾਲੀ
hl_ਸੈੱਟ_ctermbg_ਸਾਧਾਰਨ_ਸਮੂਹ(ਪੂਰਨ color)
{
    cterm_ਸਾਧਾਰਨ_bg_color = color + 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Don't mess with 'background' if the GUI is used.
    ਜੇ (!gui.ਵਿੱਚ_use && !gui.starting)
#ਜੇ_ਅੰਤ
    {
	ਸੈੱਟ_must_redraw(UPD_CLEAR);
	ਜੇ (color >= 0)
	{
	    ਪੂਰਨ dark = -1;

	    ਜੇ (termcap_ਸਰਗਰਮ)
		ਪਦ_bg_color(color);
	    ਜੇ (t_colors < 16)
		dark = (color == 0 || color == 4);
	    // Limit the heuristic to the standard 16 colors
	    ਨਹੀਂਤਾਂ ਜੇ (color < 16)
		dark = (color < 7 || color == 8);
	    // Set the 'background' option if the value is
	    // wrong.
	    ਜੇ (dark != -1
		    && dark != (*p_bg == 'd')
		    && !option_was_ਸੈੱਟ((char_u *)"bg"))
	    {
		ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"bg",
			0L, (char_u *)(dark ? "dark" : "light"), 0);
		reset_option_was_ਸੈੱਟ((char_u *)"bg");
	    }
	}
    }
}

/*
 * Set the cterm background color for the highlight group at 'idx' to 'color'.
 */
    ਸਥਿਰ ਖਾਲੀ
highlight_ਸੈੱਟ_ctermbg(ਪੂਰਨ idx, ਪੂਰਨ color, ਪੂਰਨ is_ਸਾਧਾਰਨ_ਸਮੂਹ)
{
    HL_ਸਾਰਣੀ()[idx].sg_cterm_bg = color + 1;
    ਜੇ (is_ਸਾਧਾਰਨ_ਸਮੂਹ)
	hl_ਸੈੱਟ_ctermbg_ਸਾਧਾਰਨ_ਸਮੂਹ(color);
}

/*
 * Set the cterm underline color for the Normal highlight group to "color".
 */
    ਸਥਿਰ ਖਾਲੀ
hl_ਸੈੱਟ_ctermul_ਸਾਧਾਰਨ_ਸਮੂਹ(ਪੂਰਨ color)
{
    cterm_ਸਾਧਾਰਨ_ul_color = color + 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Don't do this if the GUI is used.
    ਜੇ (!gui.ਵਿੱਚ_use && !gui.starting)
#ਜੇ_ਅੰਤ
    {
	ਸੈੱਟ_must_redraw(UPD_CLEAR);
	ਜੇ (termcap_ਸਰਗਰਮ && color >= 0)
	    ਪਦ_ul_color(color);
    }
}

/*
 * Set the cterm underline color for the highlight group at 'idx' to 'color'.
 */
    ਸਥਿਰ ਖਾਲੀ
highlight_ਸੈੱਟ_ctermul(ਪੂਰਨ idx, ਪੂਰਨ color, ਪੂਰਨ is_ਸਾਧਾਰਨ_ਸਮੂਹ)
{
    HL_ਸਾਰਣੀ()[idx].sg_cterm_ul = color + 1;
    ਜੇ (is_ਸਾਧਾਰਨ_ਸਮੂਹ)
	hl_ਸੈੱਟ_ctermul_ਸਾਧਾਰਨ_ਸਮੂਹ(color);
}

/*
 * Set the cterm font for the highlight group at 'idx'.
 * 'arg' is the color name or the numeric value as a string.
 * 'init' is set to TRUE when initializing highlighting.
 * Called for the ":highlight" command and the "hlset()" function.
 *
 * Returns TRUE if the font is set.
 */
    ਸਥਿਰ ਪੂਰਨ
highlight_ਸੈੱਟ_cterm_font(
	ਪੂਰਨ	idx,
	char_u	*arg,
	ਪੂਰਨ	init)
{
    ਪੂਰਨ		font;

    ਜੇ (init && (HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ & SG_CTERM))
	ਵਾਪਸ FALSE;

    ਜੇ (!init)
	HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ |= SG_CTERM;

    ਜੇ (VIM_ISDIGIT(*arg))
	font = atoi((ਅੱਖਰ *)arg);
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(arg, "NONE") == 0)
	font = -1;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ FALSE;

    HL_ਸਾਰਣੀ()[idx].sg_cterm_font = font + 1;
    ਵਾਪਸ TRUE;
}

/*
 * Set the cterm fg/bg/ul color for the highlight group at 'idx'.
 * 'key' is one of 'CTERMFG' or 'CTERMBG' or 'CTERMUL'.
 * 'keystart' is the color name/value.
 * 'arg' is the color name or the numeric value as a string.
 * 'is_normal_group' is set if the highlight group is 'NORMAL'
 * 'init' is set to TRUE when initializing highlighting.
 * Called for the ":highlight" command and the "hlset()" function.
 *
 * Returns TRUE if the color is set.
 */
    ਸਥਿਰ ਪੂਰਨ
highlight_ਸੈੱਟ_cterm_color(
	ਪੂਰਨ	idx,
	char_u	*ਕੁੰਜੀ,
	char_u	*ਕੁੰਜੀ_ਸ਼ੁਰੂ,
	char_u	*arg,
	ਪੂਰਨ	is_ਸਾਧਾਰਨ_ਸਮੂਹ,
	ਪੂਰਨ	init)
{
    ਪੂਰਨ		color;

    ਜੇ (init && (HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ & SG_CTERM))
	ਵਾਪਸ FALSE;

    ਜੇ (!init)
	HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ |= SG_CTERM;

    // When setting the foreground color, and previously the "bold"
    // flag was set for a light color, reset it now
    ਜੇ (ਕੁੰਜੀ[5] == 'F' && HL_ਸਾਰਣੀ()[idx].sg_cterm_bold)
    {
	HL_ਸਾਰਣੀ()[idx].sg_cterm &= ~HL_BOLD;
	HL_ਸਾਰਣੀ()[idx].sg_cterm_bold = FALSE;
    }

    ਜੇ (VIM_ISDIGIT(*arg))
	color = atoi((ਅੱਖਰ *)arg);
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(arg, "fg") == 0)
    {
	ਜੇ (cterm_ਸਾਧਾਰਨ_fg_color)
	    color = cterm_ਸਾਧਾਰਨ_fg_color - 1;
	ਨਹੀਂਤਾਂ
	{
	    emsg(_(e_fg_color_unknown));
	    ਵਾਪਸ FALSE;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(arg, "bg") == 0)
    {
	ਜੇ (cterm_ਸਾਧਾਰਨ_bg_color > 0)
	    color = cterm_ਸਾਧਾਰਨ_bg_color - 1;
	ਨਹੀਂਤਾਂ
	{
	    emsg(_(e_bg_color_unknown));
	    ਵਾਪਸ FALSE;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(arg, "ul") == 0)
    {
	ਜੇ (cterm_ਸਾਧਾਰਨ_ul_color > 0)
	    color = cterm_ਸਾਧਾਰਨ_ul_color - 1;
	ਨਹੀਂਤਾਂ
	{
	    emsg(_(e_ul_color_unknown));
	    ਵਾਪਸ FALSE;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ bold = MAYBE;
	keyvalue_T ਨਿਸ਼ਾਨਾ;
	keyvalue_T *ਇੰਦਰਾਜ;

	ਨਿਸ਼ਾਨਾ.ਕੁੰਜੀ = 0;
	ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.string = arg;
	ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.ਲੰਮਾਈ = 0;	// not used, see cmp_keyvalue_value_i()
	ਇੰਦਰਾਜ = (keyvalue_T *)bsearch(&ਨਿਸ਼ਾਨਾ, &color_name_tab,
		ARRAY_ਲੰਮਾਈ(color_name_tab), ਆਕਾਰ(color_name_tab[0]),
		cmp_keyvalue_ਮੁੱਲ_i);
	ਜੇ (ਇੰਦਰਾਜ == ਨਿਰਾ)
	{
	    semsg(_(e_color_name_or_ਸੰਖਿਆ_not_recognized_str), ਕੁੰਜੀ_ਸ਼ੁਰੂ);
	    ਵਾਪਸ FALSE;
	}

	color = lookup_color(ਇੰਦਰਾਜ->ਕੁੰਜੀ, ਕੁੰਜੀ[5] == 'F', &bold);

	// set/reset bold attribute to get light foreground
	// colors (on some terminals, e.g. "linux")
	ਜੇ (bold == TRUE)
	{
	    HL_ਸਾਰਣੀ()[idx].sg_cterm |= HL_BOLD;
	    HL_ਸਾਰਣੀ()[idx].sg_cterm_bold = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (bold == FALSE)
	    HL_ਸਾਰਣੀ()[idx].sg_cterm &= ~HL_BOLD;
    }

    // Add one to the argument, to avoid zero.  Zero is used for
    // "NONE", then "color" is -1.
    ਜੇ (ਕੁੰਜੀ[5] == 'F')
	highlight_ਸੈੱਟ_ctermfg(idx, color, is_ਸਾਧਾਰਨ_ਸਮੂਹ);
    ਨਹੀਂਤਾਂ ਜੇ (ਕੁੰਜੀ[5] == 'B')
	highlight_ਸੈੱਟ_ctermbg(idx, color, is_ਸਾਧਾਰਨ_ਸਮੂਹ);
    ਨਹੀਂਤਾਂ // ctermul
	highlight_ਸੈੱਟ_ctermul(idx, color, is_ਸਾਧਾਰਨ_ਸਮੂਹ);

    ਵਾਪਸ TRUE;
}

#ਜੇ defined(FEAT_GUI) || defined(FEAT_EVAL)
/*
 * Set the GUI foreground color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ਸਥਿਰ ਪੂਰਨ
highlight_ਸੈੱਟ_guifg(
	ਪੂਰਨ	idx,
	char_u	*arg,
	ਪੂਰਨ	is_menu_ਸਮੂਹ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ	is_scrollbar_ਸਮੂਹ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ	is_tooltip_ਸਮੂਹ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ	is_titlebar_ਸਮੂਹ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ	is_titlebarnc_ਸਮੂਹ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ	*do_colors ਅਣਵਰਤਿਆ,
	ਪੂਰਨ	init)
{
# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ਲੰਮਾ	i;
# endif
    char_u	**namep;
    ਪੂਰਨ		did_change = FALSE;

    ਜੇ (init && (HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ & SG_GUI))
	ਵਾਪਸ FALSE;

    namep = &HL_ਸਾਰਣੀ()[idx].sg_gui_fg_name;
    ਜੇ (!init)
	HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ |= SG_GUI;

# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guifg colors are only used when recognized
    i = color_name2handle(arg);
    ਜੇ (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24_ਦੋਆਧਾਰੀ_ਅੰਕ)
    {
	HL_ਸਾਰਣੀ()[idx].sg_gui_fg = i;
# endif
	ਜੇ (*namep == ਨਿਰਾ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ਜੇ (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    ਨਹੀਂਤਾਂ
		*namep = ਨਿਰਾ;
	    did_change = TRUE;
	}
# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	ਜੇ (is_titlebar_ਸਮੂਹ && gui.title_fg_pixel != i)
	{
	    gui.title_fg_pixel = i;
	    *do_colors = TRUE;
	}
	ਜੇ (is_titlebarnc_ਸਮੂਹ && gui.titlenc_fg_pixel != i)
	{
	    gui.titlenc_fg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	ਜੇ (is_menu_ਸਮੂਹ && gui.menu_fg_pixel != i)
	{
	    gui.menu_fg_pixel = i;
	    *do_colors = TRUE;
	}
	ਜੇ (is_scrollbar_ਸਮੂਹ && gui.scroll_fg_pixel != i)
	{
	    gui.scroll_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	ਜੇ (is_tooltip_ਸਮੂਹ && gui.tooltip_fg_pixel != i)
	{
	    gui.tooltip_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    ਵਾਪਸ did_change;
}

/*
 * Set the GUI background color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ਸਥਿਰ ਪੂਰਨ
highlight_ਸੈੱਟ_guibg(
	ਪੂਰਨ	idx,
	char_u	*arg,
	ਪੂਰਨ	is_menu_ਸਮੂਹ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ	is_scrollbar_ਸਮੂਹ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ	is_tooltip_ਸਮੂਹ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ	is_titlebar_ਸਮੂਹ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ	is_titlebarnc_ਸਮੂਹ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ	*do_colors ਅਣਵਰਤਿਆ,
	ਪੂਰਨ	init)
{
# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ਪੂਰਨ		i;
# endif
    char_u	**namep;
    ਪੂਰਨ		did_change = FALSE;

    ਜੇ (init && (HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ & SG_GUI))
	ਵਾਪਸ FALSE;

    namep = &HL_ਸਾਰਣੀ()[idx].sg_gui_bg_name;
    ਜੇ (!init)
	HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ |= SG_GUI;

# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guibg colors are only used when recognized
    i = color_name2handle(arg);
    ਜੇ (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24_ਦੋਆਧਾਰੀ_ਅੰਕ)
    {
	HL_ਸਾਰਣੀ()[idx].sg_gui_bg = i;
# endif
	ਜੇ (*namep == ਨਿਰਾ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ਜੇ (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    ਨਹੀਂਤਾਂ
		*namep = ਨਿਰਾ;
	    did_change = TRUE;
	}
# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	ਜੇ (is_titlebar_ਸਮੂਹ && gui.title_bg_pixel != i)
	{
	    gui.title_bg_pixel = i;
	    *do_colors = TRUE;
	}
	ਜੇ (is_titlebarnc_ਸਮੂਹ && gui.titlenc_bg_pixel != i)
	{
	    gui.titlenc_bg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	ਜੇ (is_menu_ਸਮੂਹ && gui.menu_bg_pixel != i)
	{
	    gui.menu_bg_pixel = i;
	    *do_colors = TRUE;
	}
	ਜੇ (is_scrollbar_ਸਮੂਹ && gui.scroll_bg_pixel != i)
	{
	    gui.scroll_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	ਜੇ (is_tooltip_ਸਮੂਹ && gui.tooltip_bg_pixel != i)
	{
	    gui.tooltip_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    ਵਾਪਸ did_change;
}

/*
 * Set the GUI undercurl/strikethrough color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ਸਥਿਰ ਪੂਰਨ
highlight_ਸੈੱਟ_guisp(ਪੂਰਨ idx, char_u *arg, ਪੂਰਨ init)
{
# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ਪੂਰਨ		i;
# endif
    ਪੂਰਨ		did_change = FALSE;
    char_u	**namep;

    ਜੇ (init && (HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ & SG_GUI))
	ਵਾਪਸ FALSE;

    namep = &HL_ਸਾਰਣੀ()[idx].sg_gui_sp_name;
    ਜੇ (!init)
	HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ |= SG_GUI;

# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guisp colors are only used when recognized
    i = color_name2handle(arg);
    ਜੇ (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24_ਦੋਆਧਾਰੀ_ਅੰਕ)
    {
	HL_ਸਾਰਣੀ()[idx].sg_gui_sp = i;
# endif
	ਜੇ (*namep == ਨਿਰਾ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ਜੇ (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    ਨਹੀਂਤਾਂ
		*namep = ਨਿਰਾ;
	    did_change = TRUE;
	}
# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    }
# endif

    ਵਾਪਸ did_change;
}
#ਜੇ_ਅੰਤ

/*
 * Set the start/stop terminal codes for a highlight group.
 * Returns TRUE if the terminal code is set.
 */
    ਸਥਿਰ ਪੂਰਨ
highlight_ਸੈੱਟ_startstop_termcode(ਪੂਰਨ idx, char_u *ਕੁੰਜੀ, char_u *arg, ਪੂਰਨ init)
{
    ਪੂਰਨ		off;
    char_u	buf[100];
    ਪੂਰਨ		len;
    char_u	*tname;
    char_u	*p;

    ਜੇ (!init)
	HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ |= SG_ਪਦ;

    // The "start" and "stop"  arguments can be a literal escape
    // sequence, or a comma separated list of terminal codes.
    ਜੇ (STRNCMP(arg, "t_", 2) == 0)
    {
	off = 0;
	buf[0] = 0;
	ਜਦੋਂ (arg[off] != NUL)
	{
	    // Isolate one termcap name
	    ਲਈ (len = 0; arg[off + len] &&
		    arg[off + len] != ','; ++len)
		;
	    tname = vim_strnsave(arg + off, len);
	    ਜੇ (tname == ਨਿਰਾ)		// out of memory
		ਵਾਪਸ FALSE;
	    // lookup the escape sequence for the item
	    p = get_ਪਦ_ਕੋਡ(tname);
	    vim_free(tname);
	    ਜੇ (p == ਨਿਰਾ)	    // ignore non-existing things
		p = (char_u *)"";

	    // Append it to the already found stuff
	    ਜੇ ((ਪੂਰਨ)(STRLEN(buf) + STRLEN(p)) >= 99)
	    {
		semsg(_(e_terminal_ਕੋਡ_too_long_str), arg);
		ਵਾਪਸ FALSE;
	    }
	    STRCAT(buf, p);

	    // Advance to the next item
	    off += len;
	    ਜੇ (arg[off] == ',')	    // another one follows
		++off;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// Copy characters from arg[] to buf[], translating <> codes.
	ਲਈ (p = arg, off = 0; off < 100 - 6 && *p; )
	{
	    len = trans_special(&p, buf + off, FSK_SIMPLIFY, FALSE, ਨਿਰਾ);
	    ਜੇ (len > 0)	    // recognized special char
		off += len;
	    ਨਹੀਂਤਾਂ		    // copy as normal char
		buf[off++] = *p++;
	}
	buf[off] = NUL;
    }

    ਜੇ (STRCMP(buf, "NONE") == 0)	// resetting the value
	p = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	p = vim_strsave(buf);
    ਜੇ (ਕੁੰਜੀ[2] == 'A')
    {
	vim_free(HL_ਸਾਰਣੀ()[idx].sg_ਸ਼ੁਰੂ);
	HL_ਸਾਰਣੀ()[idx].sg_ਸ਼ੁਰੂ = p;
    }
    ਨਹੀਂਤਾਂ
    {
	vim_free(HL_ਸਾਰਣੀ()[idx].sg_stop);
	HL_ਸਾਰਣੀ()[idx].sg_stop = p;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Handle the ":highlight .." command.
 * When using ":hi clear" this is called recursively for each group with
 * "forceit" and "init" both TRUE.
 */
    ਖਾਲੀ
do_highlight(
    char_u	*ਸਤਰ,
    ਪੂਰਨ		forceit,
    ਪੂਰਨ		init)	    // TRUE when called for initializing
{
    char_u	*name_ਸਿਰਾ;
    char_u	*linep;
    char_u	*ਕੁੰਜੀ_ਸ਼ੁਰੂ;
    char_u	*arg_ਸ਼ੁਰੂ;
    char_u	*ਕੁੰਜੀ = ਨਿਰਾ, *arg = ਨਿਰਾ;
    ਲੰਮਾ	i;
    ਪੂਰਨ		id;
    ਪੂਰਨ		idx;
    hl_ਸਮੂਹ_T	ਵਸਤੂ_before;
    ਪੂਰਨ		did_change = FALSE;
    ਪੂਰਨ		dodefault = FALSE;
    ਪੂਰਨ		doclear = FALSE;
    ਪੂਰਨ		dolink = FALSE;
    ਪੂਰਨ		error = FALSE;
    ਪੂਰਨ		is_ਸਾਧਾਰਨ_ਸਮੂਹ = FALSE;	// "Normal" group
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
    ਪੂਰਨ		is_titlebar_ਸਮੂਹ = FALSE;	// "TitleBar" group
    ਪੂਰਨ		is_titlebarnc_ਸਮੂਹ = FALSE;	// "TitleBarNC" group
#ਨਹੀਂਤਾਂ
# define is_titlebar_ਸਮੂਹ 0
# define is_titlebarnc_ਸਮੂਹ 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_X11
    ਪੂਰਨ		is_menu_ਸਮੂਹ = FALSE;		// "Menu" group
    ਪੂਰਨ		is_scrollbar_ਸਮੂਹ = FALSE;	// "Scrollbar" group
    ਪੂਰਨ		is_tooltip_ਸਮੂਹ = FALSE;	// "Tooltip" group
#ਨਹੀਂਤਾਂ
# define is_menu_ਸਮੂਹ 0
# define is_tooltip_ਸਮੂਹ 0
# define is_scrollbar_ਸਮੂਹ 0
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI) || defined(FEAT_EVAL)
    ਪੂਰਨ		do_colors = FALSE;		// need to update colors?
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ਪੂਰਨ		did_highlight_ਬਦਲਿਆ = FALSE;
#ਜੇ_ਅੰਤ

    // If no argument, list current highlighting.
    ਜੇ (!init && ਸਿਰੇ_excmd2(ਸਤਰ - 1, ਸਤਰ))
    {
	ਲਈ (i = 1; i <= highlight_ga.ga_len && !got_int; ++i)
	    // TODO: only call when the group has attributes set
	    highlight_list_one((ਪੂਰਨ)i);
	ਵਾਪਸ;
    }

    // Isolate the name.
    name_ਸਿਰਾ = skiptowhite(ਸਤਰ);
    linep = skipwhite(name_ਸਿਰਾ);

    // Check for "default" argument.
    ਜੇ (STRNCMP(ਸਤਰ, "default", name_ਸਿਰਾ - ਸਤਰ) == 0)
    {
	dodefault = TRUE;
	ਸਤਰ = linep;
	name_ਸਿਰਾ = skiptowhite(ਸਤਰ);
	linep = skipwhite(name_ਸਿਰਾ);
    }

    // Check for "clear" or "link" argument.
    ਜੇ (STRNCMP(ਸਤਰ, "clear", name_ਸਿਰਾ - ਸਤਰ) == 0)
	doclear = TRUE;
    ਜੇ (STRNCMP(ਸਤਰ, "link", name_ਸਿਰਾ - ਸਤਰ) == 0)
	dolink = TRUE;

    // ":highlight {group-name}": list highlighting for one group.
    ਜੇ (!doclear && !dolink && ਸਿਰੇ_excmd2(ਸਤਰ, linep))
    {
	id = syn_namen2id(ਸਤਰ, (ਪੂਰਨ)(name_ਸਿਰਾ - ਸਤਰ));
	ਜੇ (id == 0)
	    semsg(_(e_highlight_ਸਮੂਹ_name_not_ਮਿਲਿਆ_str), ਸਤਰ);
	ਨਹੀਂਤਾਂ
	    highlight_list_one(id);
	ਵਾਪਸ;
    }

    // Handle ":highlight link {from} {to}" command.
    ਜੇ (dolink)
    {
	char_u	    *from_ਸ਼ੁਰੂ = linep;
	char_u	    *from_ਸਿਰਾ;
	ਪੂਰਨ	    from_len;
	char_u	    *ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ;
	char_u	    *ਮੰਜ਼ਿਲ_ਸਿਰਾ;
	ਪੂਰਨ	    ਮੰਜ਼ਿਲ_len;

	from_ਸਿਰਾ = skiptowhite(from_ਸ਼ੁਰੂ);
	ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ = skipwhite(from_ਸਿਰਾ);
	ਮੰਜ਼ਿਲ_ਸਿਰਾ	 = skiptowhite(ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ);

	ਜੇ (ਸਿਰੇ_excmd2(ਸਤਰ, from_ਸ਼ੁਰੂ) || ਸਿਰੇ_excmd2(ਸਤਰ, ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ))
	{
	    semsg(_(e_not_enough_arguments_highlight_link_str), from_ਸ਼ੁਰੂ);
	    ਵਾਪਸ;
	}

	ਜੇ (!ਸਿਰੇ_excmd2(ਸਤਰ, skipwhite(ਮੰਜ਼ਿਲ_ਸਿਰਾ)))
	{
	    semsg(_(e_too_many_arguments_highlight_link_str), from_ਸ਼ੁਰੂ);
	    ਵਾਪਸ;
	}

	from_len = (ਪੂਰਨ)(from_ਸਿਰਾ - from_ਸ਼ੁਰੂ);
	ਮੰਜ਼ਿਲ_len = (ਪੂਰਨ)(ਮੰਜ਼ਿਲ_ਸਿਰਾ - ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ);
	highlight_ਸਮੂਹ_link(from_ਸ਼ੁਰੂ, from_len, ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ, ਮੰਜ਼ਿਲ_len,
						dodefault, forceit, init);
	ਵਾਪਸ;
    }

    ਜੇ (doclear)
    {
	// ":highlight clear [group]" command.
	ਜੇ (ਸਿਰੇ_excmd2(ਸਤਰ, linep))
	{
	    // ":highlight clear" without group name
	    highlight_reset_all();
	    ਵਾਪਸ;
	}
	ਸਤਰ = linep;
	name_ਸਿਰਾ = skiptowhite(ਸਤਰ);
	linep = skipwhite(name_ਸਿਰਾ);
    }

    // Find the group name in the table.  If it does not exist yet, add it.
    id = syn_check_ਸਮੂਹ(ਸਤਰ, (ਪੂਰਨ)(name_ਸਿਰਾ - ਸਤਰ));
    ਜੇ (id == 0)	// failed (out of memory)
	ਵਾਪਸ;
    idx = id - 1;	// index is ID minus one

    // Return if "default" was used and the group already has settings.
    ਜੇ (dodefault && hl_has_settings(idx, TRUE))
	ਵਾਪਸ;

    // Make a copy so we can check if any attribute actually changed.
    ਵਸਤੂ_before = HL_ਸਾਰਣੀ()[idx];

    ਜੇ (STRCMP(HL_ਸਾਰਣੀ()[idx].sg_name_u, "NORMAL") == 0)
	is_ਸਾਧਾਰਨ_ਸਮੂਹ = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(HL_ਸਾਰਣੀ()[idx].sg_name_u, "TITLEBAR") == 0)
	is_titlebar_ਸਮੂਹ = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(HL_ਸਾਰਣੀ()[idx].sg_name_u, "TITLEBARNC") == 0)
	is_titlebarnc_ਸਮੂਹ = TRUE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_X11
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(HL_ਸਾਰਣੀ()[idx].sg_name_u, "MENU") == 0)
	is_menu_ਸਮੂਹ = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(HL_ਸਾਰਣੀ()[idx].sg_name_u, "SCROLLBAR") == 0)
	is_scrollbar_ਸਮੂਹ = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(HL_ਸਾਰਣੀ()[idx].sg_name_u, "TOOLTIP") == 0)
	is_tooltip_ਸਮੂਹ = TRUE;
#ਜੇ_ਅੰਤ

    // Clear the highlighting for ":hi clear {group}" and ":hi clear".
    ਜੇ (doclear || (forceit && init))
    {
	highlight_clear(idx);
	ਜੇ (!doclear)
	    HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ = 0;
    }

    ਜੇ (!doclear)
	ਜਦੋਂ (!ਸਿਰੇ_excmd2(ਸਤਰ, linep))
	{
	    ਕੁੰਜੀ_ਸ਼ੁਰੂ = linep;
	    ਜੇ (*linep == '=')
	    {
		semsg(_(e_unexpected_equal_sign_str), ਕੁੰਜੀ_ਸ਼ੁਰੂ);
		error = TRUE;
		ਰੋਕੋ;
	    }

	    // Note: Keep this in sync with get_highlight_group_key.

	    // Isolate the key ("term", "ctermfg", "ctermbg", "font", "guifg"
	    // or "guibg").
	    ਜਦੋਂ (*linep && !VIM_ISWHITE(*linep) && *linep != '=')
		++linep;
	    vim_free(ਕੁੰਜੀ);
	    ਕੁੰਜੀ = vim_strnsave_ਉੱਪਰ(ਕੁੰਜੀ_ਸ਼ੁਰੂ, linep - ਕੁੰਜੀ_ਸ਼ੁਰੂ);
	    ਜੇ (ਕੁੰਜੀ == ਨਿਰਾ)
	    {
		error = TRUE;
		ਰੋਕੋ;
	    }
	    linep = skipwhite(linep);

	    ਜੇ (STRCMP(ਕੁੰਜੀ, "NONE") == 0)
	    {
		ਜੇ (!init || HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ == 0)
		{
		    ਜੇ (!init)
			HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ |= SG_ਪਦ+SG_CTERM+SG_GUI;
		    highlight_clear(idx);
		}
		ਜਾਰੀ;
	    }

	    // Check for the equal sign.
	    ਜੇ (*linep != '=')
	    {
		semsg(_(e_missing_equal_sign_str_2), ਕੁੰਜੀ_ਸ਼ੁਰੂ);
		error = TRUE;
		ਰੋਕੋ;
	    }
	    ++linep;

	    // Isolate the argument.
	    linep = skipwhite(linep);
	    ਜੇ (*linep == '\'')		// guifg='color name'
	    {
		arg_ਸ਼ੁਰੂ = ++linep;
		linep = vim_strchr(linep, '\'');
		ਜੇ (linep == ਨਿਰਾ)
		{
		    semsg(_(e_invalid_ਦਲੀਲ_str), ਕੁੰਜੀ_ਸ਼ੁਰੂ);
		    error = TRUE;
		    ਰੋਕੋ;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		arg_ਸ਼ੁਰੂ = linep;
		linep = skiptowhite(linep);
	    }
	    ਜੇ (linep == arg_ਸ਼ੁਰੂ)
	    {
		semsg(_(e_missing_ਦਲੀਲ_str), ਕੁੰਜੀ_ਸ਼ੁਰੂ);
		error = TRUE;
		ਰੋਕੋ;
	    }
	    vim_free(arg);
	    arg = vim_strnsave(arg_ਸ਼ੁਰੂ, linep - arg_ਸ਼ੁਰੂ);
	    ਜੇ (arg == ਨਿਰਾ)
	    {
		error = TRUE;
		ਰੋਕੋ;
	    }
	    ਜੇ (*linep == '\'')
		++linep;

	    // Store the argument.
	    ਜੇ (STRCMP(ਕੁੰਜੀ, "TERM") == 0
		    || STRCMP(ਕੁੰਜੀ, "CTERM") == 0
		    || STRCMP(ਕੁੰਜੀ, "GUI") == 0)
	    {
		ਜੇ (!highlight_ਸੈੱਟ_termgui_attr(idx, ਕੁੰਜੀ, arg, init))
		{
		    error = TRUE;
		    ਰੋਕੋ;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਕੁੰਜੀ, "FONT") == 0)
	    {
		// in non-GUI fonts are simply ignored
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		ਜੇ (highlight_ਸੈੱਟ_font(idx, arg, is_ਸਾਧਾਰਨ_ਸਮੂਹ,
					      is_menu_ਸਮੂਹ, is_tooltip_ਸਮੂਹ))
		    did_change = TRUE;
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਕੁੰਜੀ, "CTERMFG") == 0
		    || STRCMP(ਕੁੰਜੀ, "CTERMBG") == 0
		    || STRCMP(ਕੁੰਜੀ, "CTERMUL") == 0)
	    {
		ਜੇ (!highlight_ਸੈੱਟ_cterm_color(idx, ਕੁੰਜੀ, ਕੁੰਜੀ_ਸ਼ੁਰੂ, arg,
							is_ਸਾਧਾਰਨ_ਸਮੂਹ, init))
		{
		    error = TRUE;
		    ਰੋਕੋ;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਕੁੰਜੀ, "CTERMFONT") == 0)
	    {
		ਜੇ (!highlight_ਸੈੱਟ_cterm_font(idx, arg, init))
		{
		    error = TRUE;
		    ਰੋਕੋ;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਕੁੰਜੀ, "GUIFG") == 0)
	    {
#ਜੇ defined(FEAT_GUI) || defined(FEAT_EVAL)
		ਜੇ (highlight_ਸੈੱਟ_guifg(idx, arg, is_menu_ਸਮੂਹ,
			    is_scrollbar_ਸਮੂਹ, is_tooltip_ਸਮੂਹ,
			    is_titlebar_ਸਮੂਹ, is_titlebarnc_ਸਮੂਹ,
			    &do_colors, init))
		    did_change = TRUE;
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਕੁੰਜੀ, "GUIBG") == 0)
	    {
#ਜੇ defined(FEAT_GUI) || defined(FEAT_EVAL)
		ਜੇ (highlight_ਸੈੱਟ_guibg(idx, arg, is_menu_ਸਮੂਹ,
			    is_scrollbar_ਸਮੂਹ, is_tooltip_ਸਮੂਹ,
			    is_titlebar_ਸਮੂਹ, is_titlebarnc_ਸਮੂਹ,
			    &do_colors, init))
		    did_change = TRUE;
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਕੁੰਜੀ, "GUISP") == 0)
	    {
#ਜੇ defined(FEAT_GUI) || defined(FEAT_EVAL)
		ਜੇ (highlight_ਸੈੱਟ_guisp(idx, arg, init))
		    did_change = TRUE;
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਕੁੰਜੀ, "START") == 0 || STRCMP(ਕੁੰਜੀ, "STOP") == 0)
	    {
		ਜੇ (!highlight_ਸੈੱਟ_startstop_termcode(idx, ਕੁੰਜੀ, arg, init))
		{
		    error = TRUE;
		    ਰੋਕੋ;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		semsg(_(e_illegal_ਦਲੀਲ_str_3), ਕੁੰਜੀ_ਸ਼ੁਰੂ);
		error = TRUE;
		ਰੋਕੋ;
	    }
	    HL_ਸਾਰਣੀ()[idx].sg_cleared = FALSE;

	    // When highlighting has been given for a group, don't link it.
	    ਜੇ (!init || !(HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ & SG_LINK))
		HL_ਸਾਰਣੀ()[idx].sg_link = 0;

	    // Continue with next argument.
	    linep = skipwhite(linep);
	}

    // If there is an error, and it's a new entry, remove it from the table.
    ਜੇ (error && idx == highlight_ga.ga_len)
	syn_unadd_ਸਮੂਹ();
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (is_ਸਾਧਾਰਨ_ਸਮੂਹ)
	{
	    HL_ਸਾਰਣੀ()[idx].sg_ਪਦ_attr = 0;
	    HL_ਸਾਰਣੀ()[idx].sg_cterm_attr = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    HL_ਸਾਰਣੀ()[idx].sg_gui_attr = 0;
	    // Need to update all groups, because they might be using "bg"
	    // and/or "fg", which have been changed now.
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    ਜੇ (USE_24_ਦੋਆਧਾਰੀ_ਅੰਕ)
	    {
		highlight_gui_started();
		did_highlight_ਬਦਲਿਆ = TRUE;
		redraw_all_later(UPD_NOT_VALID);
	    }
#ਜੇ_ਅੰਤ
	    resolve_fallback_fg_ਮੰਜ਼ਿਲ_rgb();
	    resolve_fallback_bg_ਮੰਜ਼ਿਲ_rgb();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VTP
	    control_console_color_rgb();
#ਜੇ_ਅੰਤ
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
	ਨਹੀਂਤਾਂ ਜੇ (is_titlebar_ਸਮੂਹ)
	{
	    ਜੇ (gui.ਵਿੱਚ_use && do_colors)
		gui_mch_ਸੈੱਟ_titlebar_colors();
	}
	ਨਹੀਂਤਾਂ ਜੇ (is_titlebarnc_ਸਮੂਹ)
	{
	    ਜੇ (gui.ਵਿੱਚ_use && do_colors)
		gui_mch_ਸੈੱਟ_titlebar_colors();
	}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_X11
# ifdef FEAT_MENU
	ਨਹੀਂਤਾਂ ਜੇ (is_menu_ਸਮੂਹ)
	{
	    ਜੇ (gui.ਵਿੱਚ_use && do_colors)
		gui_mch_new_menu_colors();
	}
# endif
	ਨਹੀਂਤਾਂ ਜੇ (is_scrollbar_ਸਮੂਹ)
	{
	    ਜੇ (gui.ਵਿੱਚ_use && do_colors)
		gui_new_scrollbar_colors();
	    ਨਹੀਂਤਾਂ
		ਸੈੱਟ_hl_attr(idx);
	}
# ifdef FEAT_BEVAL_GUI
	ਨਹੀਂਤਾਂ ਜੇ (is_tooltip_ਸਮੂਹ)
	{
	    ਜੇ (gui.ਵਿੱਚ_use && do_colors)
		gui_mch_new_tooltip_colors();
	}
# endif
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ
	    ਸੈੱਟ_hl_attr(idx);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	HL_ਸਾਰਣੀ()[idx].sg_script_ctx = current_sctx;
	HL_ਸਾਰਣੀ()[idx].sg_script_ctx.sc_lnum += SOURCING_LNUM;
#ਜੇ_ਅੰਤ
    }

    vim_free(ਕੁੰਜੀ);
    vim_free(arg);

    // Only call highlight_changed() once, after a sequence of highlight
    // commands, and only if an attribute actually changed.
    ਜੇ ((did_change
	   || memcmp(&HL_ਸਾਰਣੀ()[idx], &ਵਸਤੂ_before, ਆਕਾਰ(ਵਸਤੂ_before)) != 0)
#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    && !did_highlight_ਬਦਲਿਆ
#ਜੇ_ਅੰਤ
       )
    {
	// Do not trigger a redraw when highlighting is changed while
	// redrawing.  This may happen when evaluating 'statusline' changes the
	// StatusLine group.
	ਜੇ (!updating_screen)
	    redraw_all_later(UPD_NOT_VALID);
	need_highlight_ਬਦਲਿਆ = TRUE;
    }
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_highlight(ਖਾਲੀ)
{
    ਪੂਰਨ	    i;

    ਲਈ (i = 0; i < highlight_ga.ga_len; ++i)
    {
	highlight_clear(i);
	vim_free(HL_ਸਾਰਣੀ()[i].sg_name);
	vim_free(HL_ਸਾਰਣੀ()[i].sg_name_u - HLNAME_ਕੁੰਜੀ_OFF);
    }
    ga_clear(&highlight_ga);
    hash_clear(&highlight_ht);
    highlight_ht_inited = false;
}
#ਜੇ_ਅੰਤ

/*
 * Reset the cterm colors to what they were before Vim was started, if
 * possible.  Otherwise reset them to zero.
 */
    ਖਾਲੀ
restore_cterm_colors(ਖਾਲੀ)
{
#ਜੇ defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
    // Since t_me has been set, this probably means that the user
    // wants to use this as default colors.  Need to reset default
    // background/foreground colors.
    mch_ਸੈੱਟ_ਸਾਧਾਰਨ_colors();
#ਨਹੀਂਤਾਂ
# ifdef VIMDLL
    ਜੇ (!gui.ਵਿੱਚ_use)
    {
	mch_ਸੈੱਟ_ਸਾਧਾਰਨ_colors();
	ਵਾਪਸ;
    }
# endif
    cterm_ਸਾਧਾਰਨ_fg_color = 0;
    cterm_ਸਾਧਾਰਨ_fg_bold = 0;
    cterm_ਸਾਧਾਰਨ_bg_color = 0;
# ifdef FEAT_TERMGUICOLORS
    cterm_ਸਾਧਾਰਨ_fg_gui_color = INVALCOLOR;
    cterm_ਸਾਧਾਰਨ_bg_gui_color = INVALCOLOR;
    cterm_ਸਾਧਾਰਨ_ul_gui_color = INVALCOLOR;
# endif
    fallback_fg_rgb = INVALCOLOR;
    fallback_bg_rgb = INVALCOLOR;
#ਜੇ_ਅੰਤ
}

/*
 * Return TRUE if highlight group "idx" has any settings.
 * When "check_link" is TRUE also check for an existing link.
 */
    ਸਥਿਰ ਪੂਰਨ
hl_has_settings(ਪੂਰਨ idx, ਪੂਰਨ check_link)
{
    ਵਾਪਸ HL_ਸਾਰਣੀ()[idx].sg_cleared == 0
	 && (  HL_ਸਾਰਣੀ()[idx].sg_ਪਦ_attr != 0
	    || HL_ਸਾਰਣੀ()[idx].sg_cterm_attr != 0
	    || HL_ਸਾਰਣੀ()[idx].sg_cterm_fg != 0
	    || HL_ਸਾਰਣੀ()[idx].sg_cterm_bg != 0
	    || HL_ਸਾਰਣੀ()[idx].sg_cterm_font != 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    || HL_ਸਾਰਣੀ()[idx].sg_gui_attr != 0
	    || HL_ਸਾਰਣੀ()[idx].sg_gui_fg_name != ਨਿਰਾ
	    || HL_ਸਾਰਣੀ()[idx].sg_gui_bg_name != ਨਿਰਾ
	    || HL_ਸਾਰਣੀ()[idx].sg_gui_sp_name != ਨਿਰਾ
	    || HL_ਸਾਰਣੀ()[idx].sg_font_name != ਨਿਰਾ
#ਜੇ_ਅੰਤ
	    || (check_link && (HL_ਸਾਰਣੀ()[idx].sg_ਸੈੱਟ & SG_LINK)));
}

/*
 * Clear highlighting for one group.
 */
    ਸਥਿਰ ਖਾਲੀ
highlight_clear(ਪੂਰਨ idx)
{
    HL_ਸਾਰਣੀ()[idx].sg_cleared = TRUE;

    HL_ਸਾਰਣੀ()[idx].sg_ਪਦ = 0;
    VIM_CLEAR(HL_ਸਾਰਣੀ()[idx].sg_ਸ਼ੁਰੂ);
    VIM_CLEAR(HL_ਸਾਰਣੀ()[idx].sg_stop);
    HL_ਸਾਰਣੀ()[idx].sg_ਪਦ_attr = 0;
    HL_ਸਾਰਣੀ()[idx].sg_cterm = 0;
    HL_ਸਾਰਣੀ()[idx].sg_cterm_bold = FALSE;
    HL_ਸਾਰਣੀ()[idx].sg_cterm_fg = 0;
    HL_ਸਾਰਣੀ()[idx].sg_cterm_bg = 0;
    HL_ਸਾਰਣੀ()[idx].sg_cterm_attr = 0;
    HL_ਸਾਰਣੀ()[idx].sg_cterm_font = 0;
#ਜੇ defined(FEAT_GUI) || defined(FEAT_EVAL)
    HL_ਸਾਰਣੀ()[idx].sg_gui = 0;
    VIM_CLEAR(HL_ਸਾਰਣੀ()[idx].sg_gui_fg_name);
    VIM_CLEAR(HL_ਸਾਰਣੀ()[idx].sg_gui_bg_name);
    VIM_CLEAR(HL_ਸਾਰਣੀ()[idx].sg_gui_sp_name);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_ਸਾਰਣੀ()[idx].sg_gui_fg = INVALCOLOR;
    HL_ਸਾਰਣੀ()[idx].sg_gui_bg = INVALCOLOR;
    HL_ਸਾਰਣੀ()[idx].sg_gui_sp = INVALCOLOR;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    gui_mch_free_font(HL_ਸਾਰਣੀ()[idx].sg_font);
    HL_ਸਾਰਣੀ()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
    gui_mch_free_fontset(HL_ਸਾਰਣੀ()[idx].sg_fontset);
    HL_ਸਾਰਣੀ()[idx].sg_fontset = NOFONTSET;
# endif
    VIM_CLEAR(HL_ਸਾਰਣੀ()[idx].sg_font_name);
    HL_ਸਾਰਣੀ()[idx].sg_gui_attr = 0;
#ਜੇ_ਅੰਤ
    // Restore default link and context if they exist. Otherwise clears.
    HL_ਸਾਰਣੀ()[idx].sg_link = HL_ਸਾਰਣੀ()[idx].sg_deflink;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // Since we set the default link, set the location to where the default
    // link was set.
    HL_ਸਾਰਣੀ()[idx].sg_script_ctx = HL_ਸਾਰਣੀ()[idx].sg_deflink_sctx;
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Set the normal foreground and background colors according to the "Normal"
 * highlighting group.  For X11 also set "Menu", "Scrollbar" and "Tooltip"
 * colors.  For MS-Windows also set "TitleBar" and "TitleBarNC" colors.
 */
    ਖਾਲੀ
ਸੈੱਟ_ਸਾਧਾਰਨ_colors(ਖਾਲੀ)
{
# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    ਜੇ (gui.ਵਿੱਚ_use)
#  endif
    {
	ਜੇ (ਸੈੱਟ_ਸਮੂਹ_colors((char_u *)"Normal",
				 &gui.norm_pixel, &gui.ਪਿੱਠ_pixel,
				 FALSE, TRUE, FALSE))
	{
	    gui_mch_new_colors();
	    ਸੈੱਟ_must_redraw(UPD_CLEAR);
	}
#  ifdef FEAT_GUI_MSWIN
	ਜੇ (ਸੈੱਟ_ਸਮੂਹ_colors((char_u *)"TitleBar",
				 &gui.title_fg_pixel, &gui.title_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_ਸੈੱਟ_titlebar_colors();
	    ਸੈੱਟ_must_redraw(UPD_CLEAR);
	}
	ਜੇ (ਸੈੱਟ_ਸਮੂਹ_colors((char_u *)"TitleBarNC",
				 &gui.titlenc_fg_pixel, &gui.titlenc_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_ਸੈੱਟ_titlebar_colors();
	    ਸੈੱਟ_must_redraw(UPD_CLEAR);
	}
#  endif
#  ifdef FEAT_GUI_X11
	ਜੇ (ਸੈੱਟ_ਸਮੂਹ_colors((char_u *)"Menu",
			     &gui.menu_fg_pixel, &gui.menu_bg_pixel,
			     TRUE, FALSE, FALSE))
	{
#   ifdef FEAT_MENU
	    gui_mch_new_menu_colors();
#   endif
	    ਸੈੱਟ_must_redraw(UPD_CLEAR);
	}
#   ifdef FEAT_BEVAL_GUI
	ਜੇ (ਸੈੱਟ_ਸਮੂਹ_colors((char_u *)"Tooltip",
			     &gui.tooltip_fg_pixel, &gui.tooltip_bg_pixel,
			     FALSE, FALSE, TRUE))
	{
#    ifdef FEAT_TOOLBAR
	    gui_mch_new_tooltip_colors();
#    endif
	    ਸੈੱਟ_must_redraw(UPD_CLEAR);
	}
#   endif
	ਜੇ (ਸੈੱਟ_ਸਮੂਹ_colors((char_u *)"Scrollbar",
			&gui.scroll_fg_pixel, &gui.scroll_bg_pixel,
			FALSE, FALSE, FALSE))
	{
	    gui_new_scrollbar_colors();
	    ਸੈੱਟ_must_redraw(UPD_CLEAR);
	}
#  endif
    }
# endif
# ifdef FEAT_TERMGUICOLORS
#  ifdef FEAT_GUI
    ਨਹੀਂਤਾਂ
#  endif
    {
	ਪੂਰਨ		idx;

	idx = syn_name2id((char_u *)"Normal") - 1;
	ਜੇ (idx >= 0)
	{
	    gui_do_one_color(idx, FALSE, FALSE);

	    // If the normal fg or bg color changed a complete redraw is
	    // required.
	    ਜੇ (cterm_ਸਾਧਾਰਨ_fg_gui_color != HL_ਸਾਰਣੀ()[idx].sg_gui_fg
		    || cterm_ਸਾਧਾਰਨ_bg_gui_color != HL_ਸਾਰਣੀ()[idx].sg_gui_bg)
	    {
		// if the GUI color is INVALCOLOR then we use the default cterm
		// color
		cterm_ਸਾਧਾਰਨ_fg_gui_color = HL_ਸਾਰਣੀ()[idx].sg_gui_fg;
		cterm_ਸਾਧਾਰਨ_bg_gui_color = HL_ਸਾਰਣੀ()[idx].sg_gui_bg;
		ਸੈੱਟ_must_redraw(UPD_CLEAR);
	    }
	}
    }
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI)
/*
 * Set the colors for "Normal", "Menu", "TitleBar", "TitleBarNC", "Tooltip" or
 * "Scrollbar".
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_ਸਮੂਹ_colors(
    char_u	*name,
    guicolor_T	*fgp,
    guicolor_T	*bgp,
    ਪੂਰਨ		do_menu,
    ਪੂਰਨ		use_norm,
    ਪੂਰਨ		do_tooltip)
{
    ਪੂਰਨ		idx;

    idx = syn_name2id(name) - 1;
    ਜੇ (idx < 0)
	ਵਾਪਸ FALSE;

    gui_do_one_color(idx, do_menu, do_tooltip);

    ਜੇ (HL_ਸਾਰਣੀ()[idx].sg_gui_fg != INVALCOLOR)
	*fgp = HL_ਸਾਰਣੀ()[idx].sg_gui_fg;
    ਨਹੀਂਤਾਂ ਜੇ (use_norm)
	*fgp = gui.def_norm_pixel;
    ਜੇ (HL_ਸਾਰਣੀ()[idx].sg_gui_bg != INVALCOLOR)
	*bgp = HL_ਸਾਰਣੀ()[idx].sg_gui_bg;
    ਨਹੀਂਤਾਂ ਜੇ (use_norm)
	*bgp = gui.def_ਪਿੱਠ_pixel;
    ਵਾਪਸ TRUE;
}

/*
 * Get the font of the "Normal" group.
 * Returns "" when it's not found or not set.
 */
    char_u *
hl_get_font_name(ਖਾਲੀ)
{
    ਪੂਰਨ		id;
    char_u	*s;

    id = syn_name2id((char_u *)"Normal");
    ਜੇ (id > 0)
    {
	s = HL_ਸਾਰਣੀ()[id - 1].sg_font_name;
	ਜੇ (s != ਨਿਰਾ)
	    ਵਾਪਸ s;
    }
    ਵਾਪਸ (char_u *)"";
}

/*
 * Set font for "Normal" group.  Called by gui_mch_init_font() when a font has
 * actually chosen to be used.
 */
    ਖਾਲੀ
hl_ਸੈੱਟ_font_name(char_u *font_name)
{
    ਪੂਰਨ	    id;

    id = syn_name2id((char_u *)"Normal");
    ਜੇ (id <= 0)
	ਵਾਪਸ;

    vim_free(HL_ਸਾਰਣੀ()[id - 1].sg_font_name);
    HL_ਸਾਰਣੀ()[id - 1].sg_font_name = vim_strsave(font_name);
}

/*
 * Set background color for "Normal" group.  Called by gui_set_bg_color()
 * when the color is known.
 */
    ਖਾਲੀ
hl_ਸੈੱਟ_bg_color_name(
    char_u  *name)	    // must have been allocated
{
    ਪੂਰਨ	    id;

    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ;

    id = syn_name2id((char_u *)"Normal");
    ਜੇ (id <= 0)
	ਵਾਪਸ;

    vim_free(HL_ਸਾਰਣੀ()[id - 1].sg_gui_bg_name);
    HL_ਸਾਰਣੀ()[id - 1].sg_gui_bg_name = name;
}

/*
 * Set foreground color for "Normal" group.  Called by gui_set_fg_color()
 * when the color is known.
 */
    ਖਾਲੀ
hl_ਸੈੱਟ_fg_color_name(
    char_u  *name)	    // must have been allocated
{
    ਪੂਰਨ	    id;

    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ;

    id = syn_name2id((char_u *)"Normal");
    ਜੇ (id <= 0)
	ਵਾਪਸ;

    vim_free(HL_ਸਾਰਣੀ()[id - 1].sg_gui_fg_name);
    HL_ਸਾਰਣੀ()[id - 1].sg_gui_fg_name = name;
}

/*
 * Return the handle for a font name.
 * Returns NOFONT when failed.
 */
    ਸਥਿਰ GuiFont
font_name2handle(char_u *name)
{
    ਜੇ (STRCMP(name, "NONE") == 0)
	ਵਾਪਸ NOFONT;

    ਵਾਪਸ gui_mch_get_font(name, TRUE);
}

# ifdef FEAT_XFONTSET
/*
 * Return the handle for a fontset name.
 * Returns NOFONTSET when failed.
 */
    ਸਥਿਰ GuiFontset
fontset_name2handle(char_u *name, ਪੂਰਨ fixed_ਚੌੜਾਈ)
{
    ਜੇ (STRCMP(name, "NONE") == 0)
	ਵਾਪਸ NOFONTSET;

    ਵਾਪਸ gui_mch_get_fontset(name, TRUE, fixed_ਚੌੜਾਈ);
}
# endif

/*
 * Get the font or fontset for one highlight group.
 */
    ਸਥਿਰ ਖਾਲੀ
hl_do_font(
    ਪੂਰਨ		idx,
    char_u	*arg,
    ਪੂਰਨ		do_ਸਾਧਾਰਨ,		// set normal font
    ਪੂਰਨ		do_menu ਅਣਵਰਤਿਆ,		// set menu font
    ਪੂਰਨ		do_tooltip ਅਣਵਰਤਿਆ,	// set tooltip font
    ਪੂਰਨ		free_font)		// free current font/fontset
{
# ifdef FEAT_XFONTSET
    // If 'guifontset' is not empty, first try using the name as a
    // fontset.  If that doesn't work, use it as a font name.
    ਜੇ (*p_guifontset != NUL
#  ifdef FONTSET_ALWAYS
	|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
	// In Motif, the Tooltip highlight group is always a fontset
	|| do_tooltip
#  endif
	    )
    {
	ਜੇ (free_font)
	    gui_mch_free_fontset(HL_ਸਾਰਣੀ()[idx].sg_fontset);
	HL_ਸਾਰਣੀ()[idx].sg_fontset = fontset_name2handle(arg, 0
#  ifdef FONTSET_ALWAYS
		|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
		|| do_tooltip
#  endif
		);
    }
    ਜੇ (HL_ਸਾਰਣੀ()[idx].sg_fontset != NOFONTSET)
    {
	// If it worked and it's the Normal group, use it as the normal
	// fontset.  Same for the Menu group.
	ਜੇ (do_ਸਾਧਾਰਨ)
	    gui_init_font(arg, TRUE);
#  ਜੇ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	ਜੇ (do_menu)
	{
#   ifdef FONTSET_ALWAYS
	    gui.menu_fontset = HL_ਸਾਰਣੀ()[idx].sg_fontset;
#   ਨਹੀਂਤਾਂ
	    // YIKES!  This is a bug waiting to crash the program
	    gui.menu_font = HL_ਸਾਰਣੀ()[idx].sg_fontset;
#   endif
	    gui_mch_new_menu_font();
	}
#   ifdef FEAT_BEVAL_GUI
	ਜੇ (do_tooltip)
	{
	    // The Athena widget set could not handle switching between
	    // displaying a single font and a fontset.
	    // If the XtNinternational resource is set to True at widget
	    // creation, then a fontset is always used, otherwise an
	    // XFontStruct is used.
	    gui.tooltip_fontset = (XFont_ਸੈੱਟ)HL_ਸਾਰਣੀ()[idx].sg_fontset;
	    gui_mch_new_tooltip_font();
	}
#   endif
#  endif
    }
    ਨਹੀਂਤਾਂ
# endif
    {
	ਜੇ (free_font)
	    gui_mch_free_font(HL_ਸਾਰਣੀ()[idx].sg_font);
	HL_ਸਾਰਣੀ()[idx].sg_font = font_name2handle(arg);
	// If it worked and it's the Normal group, use it as the
	// normal font.  Same for the Menu group.
	ਜੇ (HL_ਸਾਰਣੀ()[idx].sg_font != NOFONT)
	{
	    ਜੇ (do_ਸਾਧਾਰਨ)
		gui_init_font(arg, FALSE);
# ifndef FONTSET_ALWAYS
#  ਜੇ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	    ਜੇ (do_menu)
	    {
		gui.menu_font = HL_ਸਾਰਣੀ()[idx].sg_font;
		gui_mch_new_menu_font();
	    }
#  endif
# endif
	}
    }
}

#ਜੇ_ਅੰਤ // FEAT_GUI

#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return the handle for a color name.
 * Returns INVALCOLOR when failed.
 */
    guicolor_T
color_name2handle(char_u *name)
{
    ਜੇ (STRCMP(name, "NONE") == 0)
	ਵਾਪਸ INVALCOLOR;

    ਜੇ (STRICMP(name, "fg") == 0 || STRICMP(name, "foreground") == 0)
    {
# ਜੇ defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	ਜੇ (gui.ਵਿੱਚ_use)
# endif
# ifdef FEAT_GUI
	    ਵਾਪਸ gui.norm_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	ਜੇ (cterm_ਸਾਧਾਰਨ_fg_gui_color != INVALCOLOR)
	    ਵਾਪਸ cterm_ਸਾਧਾਰਨ_fg_gui_color;
	// Guess that the foreground is black or white.
	ਵਾਪਸ GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "black" : "white"));
# endif
    }
    ਜੇ (STRICMP(name, "bg") == 0 || STRICMP(name, "background") == 0)
    {
# ਜੇ defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	ਜੇ (gui.ਵਿੱਚ_use)
# endif
# ifdef FEAT_GUI
	    ਵਾਪਸ gui.ਪਿੱਠ_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	ਜੇ (cterm_ਸਾਧਾਰਨ_bg_gui_color != INVALCOLOR)
	    ਵਾਪਸ cterm_ਸਾਧਾਰਨ_bg_gui_color;
	// Guess that the background is white or black.
	ਵਾਪਸ GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "white" : "black"));
# endif
    }

    ਵਾਪਸ GUI_GET_COLOR(name);
}

// On MS-Windows an RGB macro is available and it produces 0x00bbggrr color
// values as used by the MS-Windows GDI api.  It should be used only for
// MS-Windows GDI builds.
# ਜੇ defined(RGB) && defined(MSWIN) && !defined(FEAT_GUI)
#  undef RGB
# endif
# ifndef RGB
#  define RGB(r, g, b)	(((r)<<16) | ((g)<<8) | (b))
# endif

# ifdef VIMDLL
    ਸਥਿਰ guicolor_T
gui_adjust_rgb(guicolor_T c)
{
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ c;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ((c & 0xff) << 16) | (c & 0x00ff00) | ((c >> 16) & 0xff);
}
# ਨਹੀਂਤਾਂ
#  define gui_adjust_rgb(c) (c)
# endif

    ਸਥਿਰ ਪੂਰਨ
hex_digit(ਪੂਰਨ c)
{
    ਜੇ (ਸੁਰੱਖਿਅਤ_isdigit(c))
	ਵਾਪਸ c - '0';
    c = TOLOWER_ASC(c);
    ਜੇ (c >= 'a' && c <= 'f')
	ਵਾਪਸ c - 'a' + 10;
    ਵਾਪਸ 0x1ffffff;
}

    ਸਥਿਰ guicolor_T
decode_hex_color(char_u *hex)
{
    guicolor_T color;

    ਜੇ (hex[0] != '#' || STRLEN(hex) != 7)
	ਵਾਪਸ INVALCOLOR;

    // Name is in "#rrggbb" format
    color = RGB(((hex_digit(hex[1]) << 4) + hex_digit(hex[2])),
		((hex_digit(hex[3]) << 4) + hex_digit(hex[4])),
		((hex_digit(hex[5]) << 4) + hex_digit(hex[6])));
    ਜੇ (color > 0xffffff)
	ਵਾਪਸ INVALCOLOR;
    ਵਾਪਸ gui_adjust_rgb(color);
}

# ifdef FEAT_EVAL
// Returns the color currently mapped to the given name or INVALCOLOR if no
// such name exists in the color table. The convention is to use lowercase for
// all keys in the v:colornames dictionary. The value can be either a string in
// the form #rrggbb or a number, either of which is converted to a guicolor_T.
    ਸਥਿਰ guicolor_T
colorname2rgb(char_u *name)
{
    dict_T      *colornames_ਸਾਰਣੀ = get_vim_var_dict(VV_COLORNAMES);
    char_u      *lc_name;
    dictitem_T  *colentry;
    char_u      *colstr;
    varnumber_T colnum;

    lc_name = strlow_save(name);
    ਜੇ (lc_name == ਨਿਰਾ)
	ਵਾਪਸ INVALCOLOR;

    colentry = dict_ਲੱਭੋ(colornames_ਸਾਰਣੀ, lc_name, -1);
    vim_free(lc_name);
    ਜੇ (colentry == ਨਿਰਾ)
	ਵਾਪਸ INVALCOLOR;

    ਜੇ (colentry->di_tv.v_type == VAR_STRING)
    {
	colstr = tv_get_string_strict(&colentry->di_tv);
	ਜੇ ((STRLEN(colstr) == 7) && (*colstr == '#'))
	{
	    ਵਾਪਸ decode_hex_color(colstr);
	}
	ਨਹੀਂਤਾਂ
	{
	    semsg(_(e_bad_color_string_str), colstr);
	    ਵਾਪਸ INVALCOLOR;
	}
    }

    ਜੇ (colentry->di_tv.v_type == VAR_ਸੰਖਿਆ)
    {
	colnum = tv_get_ਸੰਖਿਆ(&colentry->di_tv);
	ਵਾਪਸ (guicolor_T)colnum;
    }

    ਵਾਪਸ INVALCOLOR;
}

# endif

    guicolor_T
gui_get_color_cmn(char_u *name)
{
    guicolor_T  color;
    // Only non X11 colors (not present in rgb.txt) and colors in
    // color_name_tab[], useful when $VIMRUNTIME is not found,.
    // must be sorted by the 'value' field because it is used by bsearch()!
    ਸਥਿਰ keyvalue_T rgb_tab[] = {
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0x00, 0x00, 0x00), "black"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0x00, 0x00, 0xFF), "blue"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xA5, 0x2A, 0x2A), "brown"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0x00, 0xFF, 0xFF), "cyan"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0x00, 0x00, 0x8B), "darkblue"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0x00, 0x8B, 0x8B), "darkcyan"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xA9, 0xA9, 0xA9), "darkgray"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0x00, 0x64, 0x00), "darkgreen"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xA9, 0xA9, 0xA9), "darkgrey"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0x8B, 0x00, 0x8B), "darkmagenta"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0x8B, 0x00, 0x00), "darkred"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0x8B, 0x8B, 0x00), "darkyellow"),	 // No X11
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xBE, 0xBE, 0xBE), "gray"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0x00, 0xFF, 0x00), "green"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xBE, 0xBE, 0xBE), "grey"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0x66, 0x66, 0x66), "grey40"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0x7F, 0x7F, 0x7F), "grey50"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xE5, 0xE5, 0xE5), "grey90"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xAD, 0xD8, 0xE6), "lightblue"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xE0, 0xFF, 0xFF), "lightcyan"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xD3, 0xD3, 0xD3), "lightgray"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0x90, 0xEE, 0x90), "lightgreen"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xD3, 0xD3, 0xD3), "lightgrey"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xFF, 0x8B, 0xFF), "lightmagenta"),     // No XX
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xFF, 0x8B, 0x8B), "lightred"),	 // No XX
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xFF, 0xFF, 0xE0), "lightyellow"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xFF, 0x00, 0xFF), "magenta"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xFF, 0x00, 0x00), "red"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0x2E, 0x8B, 0x57), "seagreen"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xFF, 0xFF, 0xFF), "white"),
	KEYVALUE_ਇੰਦਰਾਜ(RGB(0xFF, 0xFF, 0x00), "yellow")
    };
    keyvalue_T ਨਿਸ਼ਾਨਾ;
    keyvalue_T *ਇੰਦਰਾਜ;

    color = decode_hex_color(name);
    ਜੇ (color != INVALCOLOR)
	ਵਾਪਸ color;

    ਨਿਸ਼ਾਨਾ.ਕੁੰਜੀ = 0;
    ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.string = name;
    ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.ਲੰਮਾਈ = 0;	// not used, see cmp_keyvalue_value_i()
    ਇੰਦਰਾਜ = (keyvalue_T *)bsearch(&ਨਿਸ਼ਾਨਾ, &rgb_tab, ARRAY_ਲੰਮਾਈ(rgb_tab),
	    ਆਕਾਰ(rgb_tab[0]), cmp_keyvalue_ਮੁੱਲ_i);
    ਜੇ (ਇੰਦਰਾਜ != ਨਿਰਾ)
	ਵਾਪਸ gui_adjust_rgb((guicolor_T)ਇੰਦਰਾਜ->ਕੁੰਜੀ);

# ਜੇ defined(FEAT_EVAL)
    /*
     * Not a traditional color. Load additional color aliases and then consult the alias table.
     */

    color = colorname2rgb(name);
    ਜੇ (color == INVALCOLOR)
    {
	ਲੱਦੋ_default_colors_lists();
	color = colorname2rgb(name);
    }

    ਵਾਪਸ color;
# ਨਹੀਂਤਾਂ
    ਵਾਪਸ INVALCOLOR;
# endif
}

    guicolor_T
gui_get_rgb_color_cmn(ਪੂਰਨ r, ਪੂਰਨ g, ਪੂਰਨ b)
{
    guicolor_T  color = RGB(r, g, b);

    ਜੇ (color > 0xffffff)
	ਵਾਪਸ INVALCOLOR;
    ਵਾਪਸ gui_adjust_rgb(color);
}
#ਜੇ_ਅੰਤ

/*
 * Table with the specifications for an attribute number.
 * Note that this table is used by ALL buffers.  This is required because the
 * GUI can redraw at any time for any buffer.
 */
ਸਥਿਰ garray_T	ਪਦ_attr_ਸਾਰਣੀ = {0, 0, 0, 0, ਨਿਰਾ};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪਦ_ATTR_ਇੰਦਰਾਜ(idx) ((attrentry_T *)ਪਦ_attr_ਸਾਰਣੀ.ga_ਅੰਕੜੇ)[idx]

ਸਥਿਰ garray_T	cterm_attr_ਸਾਰਣੀ = {0, 0, 0, 0, ਨਿਰਾ};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTERM_ATTR_ਇੰਦਰਾਜ(idx) ((attrentry_T *)cterm_attr_ਸਾਰਣੀ.ga_ਅੰਕੜੇ)[idx]

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
ਸਥਿਰ garray_T	gui_attr_ਸਾਰਣੀ = {0, 0, 0, 0, ਨਿਰਾ};

# define GUI_ATTR_ਇੰਦਰਾਜ(idx) ((attrentry_T *)gui_attr_ਸਾਰਣੀ.ga_ਅੰਕੜੇ)[idx]
#ਜੇ_ਅੰਤ

/*
 * Return the attr number for a set of colors and font.
 * Add a new entry to the term_attr_table, cterm_attr_table or gui_attr_table
 * if the combination is new.
 * Return 0 for error (no more room).
 */
    ਸਥਿਰ ਪੂਰਨ
get_attr_ਇੰਦਰਾਜ(garray_T *ਸਾਰਣੀ, attrentry_T *aep)
{
    ਪੂਰਨ		i;
    attrentry_T	*taep;
    ਸਥਿਰ ਪੂਰਨ	recursive = FALSE;

    // Init the table, in case it wasn't done yet.
    ਸਾਰਣੀ->ga_itemsize = ਆਕਾਰ(attrentry_T);
    ਸਾਰਣੀ->ga_growsize = 7;

    // Try to find an entry with the same specifications.
    ਲਈ (i = 0; i < ਸਾਰਣੀ->ga_len; ++i)
    {
	taep = &(((attrentry_T *)ਸਾਰਣੀ->ga_ਅੰਕੜੇ)[i]);
	ਜੇ (	   aep->ae_attr == taep->ae_attr
		&& (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		       (ਸਾਰਣੀ == &gui_attr_ਸਾਰਣੀ
			&& (aep->ae_u.gui.fg_color == taep->ae_u.gui.fg_color
			    && aep->ae_u.gui.bg_color
						    == taep->ae_u.gui.bg_color
			    && aep->ae_u.gui.sp_color
						    == taep->ae_u.gui.sp_color
			    && aep->ae_u.gui.font == taep->ae_u.gui.font
# ifdef FEAT_XFONTSET
			    && aep->ae_u.gui.fontset == taep->ae_u.gui.fontset
# endif
			    ))
		    ||
#ਜੇ_ਅੰਤ
		       (ਸਾਰਣੀ == &ਪਦ_attr_ਸਾਰਣੀ
			&& (aep->ae_u.ਪਦ.ਸ਼ੁਰੂ == ਨਿਰਾ)
					    == (taep->ae_u.ਪਦ.ਸ਼ੁਰੂ == ਨਿਰਾ)
			&& (aep->ae_u.ਪਦ.ਸ਼ੁਰੂ == ਨਿਰਾ
			    || STRCMP(aep->ae_u.ਪਦ.ਸ਼ੁਰੂ,
						  taep->ae_u.ਪਦ.ਸ਼ੁਰੂ) == 0)
			&& (aep->ae_u.ਪਦ.stop == ਨਿਰਾ)
					     == (taep->ae_u.ਪਦ.stop == ਨਿਰਾ)
			&& (aep->ae_u.ਪਦ.stop == ਨਿਰਾ
			    || STRCMP(aep->ae_u.ਪਦ.stop,
						  taep->ae_u.ਪਦ.stop) == 0))
		    || (ਸਾਰਣੀ == &cterm_attr_ਸਾਰਣੀ
			    && aep->ae_u.cterm.fg_color
						  == taep->ae_u.cterm.fg_color
			    && aep->ae_u.cterm.bg_color
						  == taep->ae_u.cterm.bg_color
			    && aep->ae_u.cterm.ul_color
						  == taep->ae_u.cterm.ul_color
			    && aep->ae_u.cterm.font
						  == taep->ae_u.cterm.font
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
			    && aep->ae_u.cterm.fg_rgb
						    == taep->ae_u.cterm.fg_rgb
			    && aep->ae_u.cterm.bg_rgb
						    == taep->ae_u.cterm.bg_rgb
			    && aep->ae_u.cterm.ul_rgb
						    == taep->ae_u.cterm.ul_rgb
#ਜੇ_ਅੰਤ
		       )))

	ਵਾਪਸ i + ATTR_OFF;
    }

    ਜੇ (ਸਾਰਣੀ->ga_len + ATTR_OFF > MAX_TYPENR)
    {
	// Running out of attribute entries!  remove all attributes, and
	// compute new ones for all groups.
	// When called recursively, we are really out of numbers.
	ਜੇ (recursive)
	{
	    emsg(_(e_too_many_different_highlighting_ਗੁਣ_ਵਿੱਚ_use));
	    ਵਾਪਸ 0;
	}
	recursive = TRUE;

	clear_hl_tables();

	ਸੈੱਟ_must_redraw(UPD_CLEAR);

	ਲਈ (i = 0; i < highlight_ga.ga_len; ++i)
	    ਸੈੱਟ_hl_attr(i);

	recursive = FALSE;
    }

    // This is a new combination of colors and font, add an entry.
    ਜੇ (ga_grow(ਸਾਰਣੀ, 1) == FAIL)
	ਵਾਪਸ 0;

    taep = &(((attrentry_T *)ਸਾਰਣੀ->ga_ਅੰਕੜੇ)[ਸਾਰਣੀ->ga_len]);
    CLEAR_ਸੂਚਕ(taep);
    taep->ae_attr = aep->ae_attr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (ਸਾਰਣੀ == &gui_attr_ਸਾਰਣੀ)
    {
	taep->ae_u.gui.fg_color = aep->ae_u.gui.fg_color;
	taep->ae_u.gui.bg_color = aep->ae_u.gui.bg_color;
	taep->ae_u.gui.sp_color = aep->ae_u.gui.sp_color;
	taep->ae_u.gui.font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
	taep->ae_u.gui.fontset = aep->ae_u.gui.fontset;
# endif
    }
#ਜੇ_ਅੰਤ
    ਜੇ (ਸਾਰਣੀ == &ਪਦ_attr_ਸਾਰਣੀ)
    {
	ਜੇ (aep->ae_u.ਪਦ.ਸ਼ੁਰੂ == ਨਿਰਾ)
	    taep->ae_u.ਪਦ.ਸ਼ੁਰੂ = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    taep->ae_u.ਪਦ.ਸ਼ੁਰੂ = vim_strsave(aep->ae_u.ਪਦ.ਸ਼ੁਰੂ);
	ਜੇ (aep->ae_u.ਪਦ.stop == ਨਿਰਾ)
	    taep->ae_u.ਪਦ.stop = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    taep->ae_u.ਪਦ.stop = vim_strsave(aep->ae_u.ਪਦ.stop);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਸਾਰਣੀ == &cterm_attr_ਸਾਰਣੀ)
    {
	taep->ae_u.cterm.fg_color = aep->ae_u.cterm.fg_color;
	taep->ae_u.cterm.bg_color = aep->ae_u.cterm.bg_color;
	taep->ae_u.cterm.ul_color = aep->ae_u.cterm.ul_color;
	taep->ae_u.cterm.font = aep->ae_u.cterm.font;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
	taep->ae_u.cterm.fg_rgb = aep->ae_u.cterm.fg_rgb;
	taep->ae_u.cterm.bg_rgb = aep->ae_u.cterm.bg_rgb;
	taep->ae_u.cterm.ul_rgb = aep->ae_u.cterm.ul_rgb;
#ਜੇ_ਅੰਤ
    }
    ++ਸਾਰਣੀ->ga_len;
    ਵਾਪਸ (ਸਾਰਣੀ->ga_len - 1 + ATTR_OFF);
}

#ਜੇ defined(FEAT_TERMINAL)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    ਪੂਰਨ
get_cterm_attr_idx(ਪੂਰਨ attr, ਪੂਰਨ fg, ਪੂਰਨ bg)
{
    attrentry_T		at_en;

    CLEAR_ਖੇਤਰ(at_en);
# ifdef FEAT_TERMGUICOLORS
    at_en.ae_u.cterm.fg_rgb = INVALCOLOR;
    at_en.ae_u.cterm.bg_rgb = INVALCOLOR;
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
# endif
    at_en.ae_attr = attr;
    at_en.ae_u.cterm.fg_color = fg;
    at_en.ae_u.cterm.bg_color = bg;
    at_en.ae_u.cterm.ul_color = 0;
    at_en.ae_u.cterm.font = 0;
    ਵਾਪਸ get_attr_ਇੰਦਰਾਜ(&cterm_attr_ਸਾਰਣੀ, &at_en);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_TERMINAL) && defined(FEAT_TERMGUICOLORS)
/*
 * Get an attribute index for a 'termguicolors' entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    ਪੂਰਨ
get_tgc_attr_idx(ਪੂਰਨ attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_ਖੇਤਰ(at_en);
    at_en.ae_attr = attr;
    ਜੇ (fg == INVALCOLOR && bg == INVALCOLOR)
    {
	// If both GUI colors are not set fall back to the cterm colors.  Helps
	// if the GUI only has an attribute, such as undercurl.
	at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
    }
    ਨਹੀਂਤਾਂ
    {
	at_en.ae_u.cterm.fg_rgb = fg;
	at_en.ae_u.cterm.bg_rgb = bg;
    }
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
    ਵਾਪਸ get_attr_ਇੰਦਰਾਜ(&cterm_attr_ਸਾਰਣੀ, &at_en);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_TERMINAL) && defined(FEAT_GUI)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    ਪੂਰਨ
get_gui_attr_idx(ਪੂਰਨ attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_ਖੇਤਰ(at_en);
    at_en.ae_attr = attr;
    at_en.ae_u.gui.fg_color = fg;
    at_en.ae_u.gui.bg_color = bg;
    ਵਾਪਸ get_attr_ਇੰਦਰਾਜ(&gui_attr_ਸਾਰਣੀ, &at_en);
}
#ਜੇ_ਅੰਤ

/*
 * Clear all highlight tables.
 */
    ਖਾਲੀ
clear_hl_tables(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    attrentry_T	*taep;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ga_clear(&gui_attr_ਸਾਰਣੀ);
#ਜੇ_ਅੰਤ
    ਲਈ (i = 0; i < ਪਦ_attr_ਸਾਰਣੀ.ga_len; ++i)
    {
	taep = &(((attrentry_T *)ਪਦ_attr_ਸਾਰਣੀ.ga_ਅੰਕੜੇ)[i]);
	vim_free(taep->ae_u.ਪਦ.ਸ਼ੁਰੂ);
	vim_free(taep->ae_u.ਪਦ.stop);
    }
    ga_clear(&ਪਦ_attr_ਸਾਰਣੀ);
    ga_clear(&cterm_attr_ਸਾਰਣੀ);
}

/*
 * Combine special attributes (e.g., for spelling) with other attributes
 * (e.g., for syntax highlighting).
 * "prim_attr" overrules "char_attr".
 * This creates a new group when required.
 * Since we expect there to be few spelling mistakes we don't cache the
 * result.
 * Return the resulting attributes.
 */
    ਪੂਰਨ
hl_combine_attr(ਪੂਰਨ char_attr, ਪੂਰਨ prim_attr)
{
    attrentry_T *char_aep = ਨਿਰਾ;
    attrentry_T *prim_aep;
    attrentry_T new_en;

    ਜੇ (char_attr == 0)
	ਵਾਪਸ prim_attr;
    ਜੇ (char_attr <= HL_ALL && prim_attr <= HL_ALL)
	ਵਾਪਸ ATTR_COMBINE(char_attr, prim_attr);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	ਜੇ (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	ਜੇ (char_aep != ਨਿਰਾ)
	    new_en = *char_aep;
	ਨਹੀਂਤਾਂ
	{
	    CLEAR_ਖੇਤਰ(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    ਜੇ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	ਜੇ (prim_attr <= HL_ALL)
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	ਨਹੀਂਤਾਂ
	{
	    prim_aep = syn_gui_attr2entry(prim_attr);
	    ਜੇ (prim_aep != ਨਿਰਾ)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		ਜੇ (prim_aep->ae_u.gui.fg_color != INVALCOLOR)
		    new_en.ae_u.gui.fg_color = prim_aep->ae_u.gui.fg_color;
		ਜੇ (prim_aep->ae_u.gui.bg_color != INVALCOLOR)
		    new_en.ae_u.gui.bg_color = prim_aep->ae_u.gui.bg_color;
		ਜੇ (prim_aep->ae_u.gui.sp_color != INVALCOLOR)
		    new_en.ae_u.gui.sp_color = prim_aep->ae_u.gui.sp_color;
		ਜੇ (prim_aep->ae_u.gui.font != NOFONT)
		    new_en.ae_u.gui.font = prim_aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
		ਜੇ (prim_aep->ae_u.gui.fontset != NOFONTSET)
		    new_en.ae_u.gui.fontset = prim_aep->ae_u.gui.fontset;
# endif
	    }
	}
	ਵਾਪਸ get_attr_ਇੰਦਰਾਜ(&gui_attr_ਸਾਰਣੀ, &new_en);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (IS_CTERM)
    {
	ਜੇ (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	ਜੇ (char_aep != ਨਿਰਾ)
	    new_en = *char_aep;
	ਨਹੀਂਤਾਂ
	{
	    CLEAR_ਖੇਤਰ(new_en);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#ਜੇ_ਅੰਤ
	    ਜੇ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	ਜੇ (prim_attr <= HL_ALL)
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	ਨਹੀਂਤਾਂ
	{
	    prim_aep = syn_cterm_attr2entry(prim_attr);
	    ਜੇ (prim_aep != ਨਿਰਾ)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		ਜੇ (prim_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color = prim_aep->ae_u.cterm.fg_color;
		ਜੇ (prim_aep->ae_u.cterm.bg_color > 0)
		    new_en.ae_u.cterm.bg_color = prim_aep->ae_u.cterm.bg_color;
		ਜੇ (prim_aep->ae_u.cterm.ul_color > 0)
		    new_en.ae_u.cterm.ul_color = prim_aep->ae_u.cterm.ul_color;
		ਜੇ (prim_aep->ae_u.cterm.font > 0)
		    new_en.ae_u.cterm.font = prim_aep->ae_u.cterm.font;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
		// If both fg and bg are not set fall back to cterm colors.
		// Helps for SpellBad which uses undercurl in the GUI.
		ਜੇ (COLOR_INVALID(prim_aep->ae_u.cterm.fg_rgb)
			&& COLOR_INVALID(prim_aep->ae_u.cterm.bg_rgb))
		{
		    ਜੇ (prim_aep->ae_u.cterm.fg_color > 0)
			new_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
		    ਜੇ (prim_aep->ae_u.cterm.bg_color > 0)
			new_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (prim_aep->ae_u.cterm.fg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.fg_rgb = prim_aep->ae_u.cterm.fg_rgb;
		    ਜੇ (prim_aep->ae_u.cterm.bg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.bg_rgb = prim_aep->ae_u.cterm.bg_rgb;
		}
		ਜੇ (prim_aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		    new_en.ae_u.cterm.ul_rgb = prim_aep->ae_u.cterm.ul_rgb;
#ਜੇ_ਅੰਤ
	    }
	}
	ਵਾਪਸ get_attr_ਇੰਦਰਾਜ(&cterm_attr_ਸਾਰਣੀ, &new_en);
    }

    ਜੇ (char_attr > HL_ALL)
	char_aep = syn_ਪਦ_attr2entry(char_attr);
    ਜੇ (char_aep != ਨਿਰਾ)
	new_en = *char_aep;
    ਨਹੀਂਤਾਂ
    {
	CLEAR_ਖੇਤਰ(new_en);
	ਜੇ (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    ਜੇ (prim_attr <= HL_ALL)
	new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
    ਨਹੀਂਤਾਂ
    {
	prim_aep = syn_ਪਦ_attr2entry(prim_attr);
	ਜੇ (prim_aep != ਨਿਰਾ)
	{
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_aep->ae_attr);
	    ਜੇ (prim_aep->ae_u.ਪਦ.ਸ਼ੁਰੂ != ਨਿਰਾ)
	    {
		new_en.ae_u.ਪਦ.ਸ਼ੁਰੂ = prim_aep->ae_u.ਪਦ.ਸ਼ੁਰੂ;
		new_en.ae_u.ਪਦ.stop = prim_aep->ae_u.ਪਦ.stop;
	    }
	}
    }
    ਵਾਪਸ get_attr_ਇੰਦਰਾਜ(&ਪਦ_attr_ਸਾਰਣੀ, &new_en);
}

// ANSI color order: Black, Red, Green, Yellow, Blue, Magenta,
// Cyan, White, then bright variants.  Approximate RGB values used when
// only a cterm color number is known (no guifg/guibg).  Real terminal
// palettes may differ if the user reconfigured their emulator, but
// these are reasonable xterm-ish defaults.
ਸਥਿਰ ਅਚਲ long_u cterm_color_16[16] = {
    0x000000, 0xc00000, 0x008000, 0x808000,
    0x0000c0, 0xc000c0, 0x004080, 0xc0c0c0,
    0x808080, 0xff8080, 0x00ff00, 0xffff00,
    0x6060ff, 0xff40ff, 0x00ffff, 0xffffff
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
/*
 * Convert a cterm color number (1-16) to an RGB value.
 * Used as a fallback when 'termguicolors' is set but only cterm colors are
 * specified (no guifg/guibg).
 * Returns INVALCOLOR if the color number is out of range.
 */
    ਸਥਿਰ guicolor_T
cterm_color_ਮੰਜ਼ਿਲ_rgb(ਪੂਰਨ color_nr)
{
    ਜੇ (color_nr < 1 || color_nr > 16)
	ਵਾਪਸ INVALCOLOR;
    ਵਾਪਸ (guicolor_T)cterm_color_16[color_nr - 1];
}
#ਜੇ_ਅੰਤ

/*
 * Convert an xterm 256-color index (0-255) to an approximate RGB triple.
 * Uses the standard xterm palette: 0-15 ANSI (cterm_color_16), 16-231
 * 6x6x6 cube, 232-255 grayscale ramp.
 */
    ਸਥਿਰ ਖਾਲੀ
cterm_idx_ਮੰਜ਼ਿਲ_rgb(ਪੂਰਨ idx, ਪੂਰਨ *r, ਪੂਰਨ *g, ਪੂਰਨ *b)
{
    ਸਥਿਰ ਅਚਲ ਪੂਰਨ cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

    ਜੇ (idx < 0 || idx > 255)
    {
	*r = *g = *b = 0;
	ਵਾਪਸ;
    }
    ਜੇ (idx < 16)
    {
	long_u rgb = cterm_color_16[idx];
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
    }
    ਨਹੀਂਤਾਂ ਜੇ (idx < 232)
    {
	ਪੂਰਨ n = idx - 16;
	*r = cube[n / 36 % 6];
	*g = cube[n / 6  % 6];
	*b = cube[n      % 6];
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ v = 8 + (idx - 232) * 10;
	*r = *g = *b = v;
    }
}

/*
 * Approximate an RGB triple to the nearest xterm 256-color index.
 * Searches the 6x6x6 cube and the grayscale ramp; returns 0-255.
 */
    ਸਥਿਰ ਪੂਰਨ
rgb_ਮੰਜ਼ਿਲ_cterm_idx(ਪੂਰਨ r, ਪੂਰਨ g, ਪੂਰਨ b)
{
    ਸਥਿਰ ਅਚਲ ਪੂਰਨ cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };
    ਪੂਰਨ ਉੱਤਮ = 0;
    ਲੰਮਾ ਉੱਤਮ_d = -1;
    ਪੂਰਨ i;

    // Search the 6x6x6 cube.
    ਲਈ (i = 16; i < 232; ++i)
    {
	ਪੂਰਨ n = i - 16;
	ਪੂਰਨ cr = cube[n / 36 % 6];
	ਪੂਰਨ cg = cube[n / 6  % 6];
	ਪੂਰਨ cb = cube[n      % 6];
	ਲੰਮਾ d = (cr - r) * (cr - r) + (cg - g) * (cg - g) + (cb - b) * (cb - b);
	ਜੇ (ਉੱਤਮ_d < 0 || d < ਉੱਤਮ_d)
	{
	    ਉੱਤਮ_d = d;
	    ਉੱਤਮ = i;
	}
    }
    // Search the grayscale ramp.
    ਲਈ (i = 232; i < 256; ++i)
    {
	ਪੂਰਨ v = 8 + (i - 232) * 10;
	ਲੰਮਾ d = (v - r) * (v - r) + (v - g) * (v - g) + (v - b) * (v - b);
	ਜੇ (d < ਉੱਤਮ_d)
	{
	    ਉੱਤਮ_d = d;
	    ਉੱਤਮ = i;
	}
    }
    ਵਾਪਸ ਉੱਤਮ;
}

/*
 * Resolve a color to an RGB triple.  Prefer the gui RGB value (set by
 * guifg/guibg) when valid, otherwise convert the cterm 256-color index.
 * Returns true on success; false when no color is defined.
 * cterm_c is 1-based (0 means "no color").
 */
    ਸਥਿਰ bool
resolve_color_ਮੰਜ਼ਿਲ_rgb(ਪੂਰਨ cterm_c, guicolor_T rgb ਅਣਵਰਤਿਆ, ਪੂਰਨ *r, ਪੂਰਨ *g, ਪੂਰਨ *b)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
    ਜੇ (!COLOR_INVALID(rgb))
    {
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
	ਵਾਪਸ true;
    }
#ਜੇ_ਅੰਤ
    ਜੇ (cterm_c > 0)
    {
	cterm_idx_ਮੰਜ਼ਿਲ_rgb(cterm_c - 1, r, g, b);
	ਵਾਪਸ true;
    }
    ਵਾਪਸ false;
}

/*
 * get a RGB fallback color from gui, cterm or default color
 */
    ਸਥਿਰ guicolor_T
resolve_fallback_color(ਪੂਰਨ cterm_c, guicolor_T rgb, guicolor_T default_rgb)
{
    ਪੂਰਨ red, green, blue;
    ਜੇ (!resolve_color_ਮੰਜ਼ਿਲ_rgb(cterm_c, rgb, &red, &green, &blue))
	ਵਾਪਸ default_rgb;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ (red << 16) | (green << 8) | blue;
}

/*
 * get a RGB fallback foreground color from guifg, ctermfg or deduced from background
 */
    ਸਥਿਰ ਖਾਲੀ
resolve_fallback_fg_ਮੰਜ਼ਿਲ_rgb(ਖਾਲੀ)
{
    guicolor_T fgcolor_or_gui_fgcolor = INVALCOLOR;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
    fgcolor_or_gui_fgcolor = cterm_ਸਾਧਾਰਨ_fg_gui_color;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	fgcolor_or_gui_fgcolor = gui.norm_pixel;
#ਜੇ_ਅੰਤ
    fallback_fg_rgb = resolve_fallback_color(cterm_ਸਾਧਾਰਨ_fg_color, fgcolor_or_gui_fgcolor, (*p_bg == 'l') ? 0x000000 : 0xFFFFFF);
}

/*
 * get a RGB fallback background color from guifg, ctermbg or deduced from background
 */
    ਸਥਿਰ ਖਾਲੀ
resolve_fallback_bg_ਮੰਜ਼ਿਲ_rgb(ਖਾਲੀ)
{
    guicolor_T bgcolor_or_gui_bgcolor = INVALCOLOR;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
    bgcolor_or_gui_bgcolor = cterm_ਸਾਧਾਰਨ_bg_gui_color;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	bgcolor_or_gui_bgcolor = gui.ਪਿੱਠ_pixel;
#ਜੇ_ਅੰਤ
    fallback_bg_rgb = resolve_fallback_color(cterm_ਸਾਧਾਰਨ_bg_color, bgcolor_or_gui_bgcolor, (*p_bg == 'l') ? 0xFFFFFF : 0x000000);
}

/*
 * Blend two colors expressed as (cterm 256 index, gui RGB) pairs and
 * return the nearest 1-based cterm 256-color index.  Prefers the gui
 * RGB so highlight definitions like "guibg=#2D2A3D" without ctermbg
 * still produce a meaningful blend in 256-color terminals.
 *
 * "default_rgb" is used as the underlying color when neither under_c
 * nor under_rgb is set; pass 0xFFFFFF for fg-style blends (so text
 * fades toward white) or 0x000000 for bg-style blends (so the popup
 * fades toward terminal default dark).
 *
 * Requires t_colors >= 256; otherwise returns popup_c unchanged.
 */
    ਸਥਿਰ ਪੂਰਨ
blend_cterm_colors(ਪੂਰਨ popup_c, guicolor_T popup_rgb,
		   ਪੂਰਨ under_c, guicolor_T under_rgb,
		   ਪੂਰਨ default_rgb,
		   ਪੂਰਨ blend_val)
{
    ਪੂਰਨ pr, pg, pb, ur, ug, ub, r, g, b;

    ਜੇ (t_colors < 256)
	ਵਾਪਸ popup_c;
    ਜੇ (!resolve_color_ਮੰਜ਼ਿਲ_rgb(popup_c, popup_rgb, &pr, &pg, &pb))
	ਵਾਪਸ under_c;
    ਜੇ (blend_val <= 0)
	ਵਾਪਸ rgb_ਮੰਜ਼ਿਲ_cterm_idx(pr, pg, pb) + 1;
    ਜੇ (!resolve_color_ਮੰਜ਼ਿਲ_rgb(under_c, under_rgb, &ur, &ug, &ub))
    {
	ur = (default_rgb >> 16) & 0xFF;
	ug = (default_rgb >> 8) & 0xFF;
	ub = default_rgb & 0xFF;
    }
    ਜੇ (blend_val >= 100)
	ਵਾਪਸ rgb_ਮੰਜ਼ਿਲ_cterm_idx(ur, ug, ub) + 1;
    r = pr + (ur - pr) * blend_val / 100;
    g = pg + (ug - pg) * blend_val / 100;
    b = pb + (ub - pb) * blend_val / 100;
    ਵਾਪਸ rgb_ਮੰਜ਼ਿਲ_cterm_idx(r, g, b) + 1;
}

#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Blend two RGB colors based on blend value (0-100).
 * blend: 0=return first argument color, 100=return second argument color
 * If bg_color is INVALCOLOR, high blend means more visible (return INVALCOLOR).
 */
    ਸਥਿਰ guicolor_T
blend_colors(guicolor_T popup_color, guicolor_T bg_color, ਪੂਰਨ blend_val)
{
    ਪੂਰਨ r1, g1, b1, r2, g2, b2, r, g, b;

    // CTERMCOLOR is a sentinel meaning "use the cterm color"; for blending
    // it has no real RGB so treat it like INVALCOLOR.
    ਜੇ (COLOR_INVALID(popup_color))
	ਵਾਪਸ INVALCOLOR;

    // Fully transparent: use underlying color as-is.
    ਜੇ (blend_val >= 100)
	ਵਾਪਸ bg_color;

    r1 = (popup_color >> 16) & 0xFF;
    g1 = (popup_color >> 8) & 0xFF;
    b1 = popup_color & 0xFF;

    ਜੇ (COLOR_INVALID(bg_color))
	bg_color = fallback_bg_rgb;

    r2 = (bg_color >> 16) & 0xFF;
    g2 = (bg_color >> 8) & 0xFF;
    b2 = bg_color & 0xFF;

    r = r1 + (r2 - r1) * blend_val / 100;
    g = g1 + (g2 - g1) * blend_val / 100;
    b = b1 + (b2 - b1) * blend_val / 100;

    ਵਾਪਸ (r << 16) | (g << 8) | b;
}
#ਜੇ_ਅੰਤ

/*
 * Blend attributes for popup windows and menus with opacity.
 * Blends foreground and/or background colors based on blend value (0-100).
 * blend: 0 = opaque (use popup colors), 100 = transparent (use background colors)
 * blend_fg: TRUE to blend foreground color, FALSE to keep popup foreground
 */
    ਸਥਿਰ ਪੂਰਨ
hl_blend_attr_common(
    ਪੂਰਨ		char_attr,
    ਪੂਰਨ		popup_attr,
    ਪੂਰਨ		blend,
    ਪੂਰਨ		blend_fg)
{
    attrentry_T *char_aep = ਨਿਰਾ;
    attrentry_T *popup_aep;
    attrentry_T new_en;
    attrentry_T tmp_en;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	ਜੇ (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	ਜੇ (char_aep != ਨਿਰਾ)
	    new_en = *char_aep;
	ਨਹੀਂਤਾਂ
	{
	    CLEAR_ਖੇਤਰ(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    ਜੇ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	ਜੇ (popup_attr <= HL_ALL)
	{
	    CLEAR_ਖੇਤਰ(tmp_en);
	    tmp_en.ae_u.gui.fg_color = INVALCOLOR;
	    tmp_en.ae_u.gui.bg_color = INVALCOLOR;
	    tmp_en.ae_u.gui.sp_color = INVALCOLOR;
	    // preserve attributes other than color
	    tmp_en.ae_attr = popup_attr;
	    popup_aep = &tmp_en;

	    popup_aep->ae_u.gui.bg_color = fallback_bg_rgb;
	}
	ਨਹੀਂਤਾਂ
	    popup_aep = syn_gui_attr2entry(popup_attr);

	ਜੇ (popup_aep != ਨਿਰਾ)
	{
	    guicolor_T popup_bg_rgb = popup_aep->ae_u.gui.bg_color;
	    ਜੇ (COLOR_INVALID(popup_bg_rgb))
		popup_bg_rgb = fallback_bg_rgb;

	    ਜੇ (blend_fg)
	    {
		// blend_fg=TRUE: fade underlying text toward popup bg.
		{
		    ਪੂਰਨ ਅਧਾਰ_fg = fallback_fg_rgb;
		    ਜੇ (char_aep != ਨਿਰਾ
			    && char_aep->ae_u.gui.fg_color != INVALCOLOR)
			ਅਧਾਰ_fg = char_aep->ae_u.gui.fg_color;
		    new_en.ae_u.gui.fg_color = blend_colors(
			    popup_bg_rgb, ਅਧਾਰ_fg, blend);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// blend_fg=FALSE: popup text is opaque.  Replace the
		// underlying cell's attribute flags, fg, special color
		// and font with the popup's, so the underlying syntax
		// highlighting and any decoration (textprop undercurl,
		// ...) do not bleed through.
		new_en.ae_attr = popup_aep->ae_attr;
		// fallback correctly to Normal fg color if fg_color == INVALCOLOR
		new_en.ae_u.gui.fg_color = popup_aep->ae_u.gui.fg_color;
		new_en.ae_u.gui.sp_color = popup_aep->ae_u.gui.sp_color;
		new_en.ae_u.gui.font = popup_aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
		new_en.ae_u.gui.fontset = popup_aep->ae_u.gui.fontset;
# endif
	    }
	    // Blend background color: blend popup bg toward underlying bg
	    {
		guicolor_T underlying_bg = fallback_bg_rgb;
		ਜੇ (char_aep != ਨਿਰਾ
			&& !COLOR_INVALID(char_aep->ae_u.gui.bg_color))
		    underlying_bg = char_aep->ae_u.gui.bg_color;
		new_en.ae_u.gui.bg_color = blend_colors(
			popup_bg_rgb,
			underlying_bg, blend);
	    }
	}
	ਵਾਪਸ get_attr_ਇੰਦਰਾਜ(&gui_attr_ਸਾਰਣੀ, &new_en);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (IS_CTERM)
    {
	ਜੇ (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	ਜੇ (char_aep != ਨਿਰਾ)
	    new_en = *char_aep;
	ਨਹੀਂਤਾਂ
	{
	    CLEAR_ਖੇਤਰ(new_en);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#ਜੇ_ਅੰਤ
	    ਜੇ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	ਜੇ (popup_attr <= HL_ALL)
	{
	    CLEAR_ਖੇਤਰ(tmp_en);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
	    tmp_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    tmp_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	    // allow blending with termguicolors
	    tmp_en.ae_u.cterm.bg_rgb = fallback_bg_rgb;
#ਜੇ_ਅੰਤ
	    // preserve attributes other than color
	    tmp_en.ae_attr = popup_attr;
	    popup_aep = &tmp_en;

	    // allow blending with notermguicolors
	    popup_aep->ae_u.cterm.bg_color = cterm_ਸਾਧਾਰਨ_bg_color;
	}
	ਨਹੀਂਤਾਂ
	    popup_aep = syn_cterm_attr2entry(popup_attr);

	ਜੇ (popup_aep != ਨਿਰਾ)
	{
	    guicolor_T popup_bg_rgb = INVALCOLOR;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
	    // Fall back to cterm color converted to RGB when gui color is not set.
	    popup_bg_rgb = popup_aep->ae_u.cterm.bg_rgb;
	    ਜੇ (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color > 0)
		popup_bg_rgb = cterm_color_ਮੰਜ਼ਿਲ_rgb(
			popup_aep->ae_u.cterm.bg_color);
#ਜੇ_ਅੰਤ
	    // assign default color if guibg and ctermbg are not set for popup
	    ਜੇ (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color == 0)
		popup_bg_rgb = fallback_bg_rgb;

	    ਜੇ (!blend_fg)
	    {
		// blend_fg=FALSE: popup text is opaque.  Replace the
		// underlying cell's attribute flags, fg, underline color
		// and font with the popup's, so the underlying syntax
		// highlighting and any decoration (textprop undercurl,
		// ...) do not bleed through.  When the popup has no fg
		// (e.g. "guifg=NONE") fall back to Normal's fg so the
		// text is still readable instead of taking on whatever
		// the underlying cell happened to have.
		new_en.ae_attr = popup_aep->ae_attr;
		ਜੇ (popup_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color =
			popup_aep->ae_u.cterm.fg_color;
		ਨਹੀਂਤਾਂ ਜੇ (cterm_ਸਾਧਾਰਨ_fg_color > 0)
		    new_en.ae_u.cterm.fg_color = cterm_ਸਾਧਾਰਨ_fg_color;
		ਨਹੀਂਤਾਂ
		    // black-ish or white-ish
		    new_en.ae_u.cterm.fg_color = (*p_bg == 'l') ? 1 : 16;
		new_en.ae_u.cterm.ul_color = popup_aep->ae_u.cterm.ul_color;
		new_en.ae_u.cterm.font = popup_aep->ae_u.cterm.font;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
		new_en.ae_u.cterm.ul_rgb = popup_aep->ae_u.cterm.ul_rgb;
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// blend_fg=TRUE: fade underlying fg toward popup bg in
		// the 256-color palette.  Used when the popup is over a
		// cell rendered with cterm colors (no termguicolors RGB).
		ਪੂਰਨ under_fg = (char_aep != ਨਿਰਾ)
		    ? char_aep->ae_u.cterm.fg_color : 0;
		guicolor_T under_fg_rgb = INVALCOLOR;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
		ਜੇ (char_aep != ਨਿਰਾ)
		    under_fg_rgb = char_aep->ae_u.cterm.fg_rgb;
#ਜੇ_ਅੰਤ
		new_en.ae_u.cterm.fg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_fg, under_fg_rgb, fallback_fg_rgb, blend);
	    }
	    // Approximate cterm bg by blending with the underlying bg
	    // in the 256-color palette and mapping to the nearest entry.
	    {
		ਪੂਰਨ under_bg = (char_aep != ਨਿਰਾ)
		    ? char_aep->ae_u.cterm.bg_color : 0;
		guicolor_T under_bg_rgb = INVALCOLOR;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
		ਜੇ (char_aep != ਨਿਰਾ)
		    under_bg_rgb = char_aep->ae_u.cterm.bg_rgb;
#ਜੇ_ਅੰਤ
		new_en.ae_u.cterm.bg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_bg, under_bg_rgb, fallback_bg_rgb, blend);
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
	    // Blend RGB colors for termguicolors mode.
	    {
		ਜੇ (blend_fg)
		{
		    // blend_fg=TRUE: fade underlying text toward popup bg.
		    ਜੇ (popup_bg_rgb != INVALCOLOR)
		    {
			ਪੂਰਨ ਅਧਾਰ_fg = fallback_fg_rgb;
			// CTERMCOLOR is a sentinel meaning "use the cterm
			// color"; treat it as no underlying color so it is
			// not blended in as a real near-white pixel.
			ਜੇ (char_aep != ਨਿਰਾ
				&& !COLOR_INVALID(char_aep->ae_u.cterm.fg_rgb))
			    ਅਧਾਰ_fg = char_aep->ae_u.cterm.fg_rgb;
			new_en.ae_u.cterm.fg_rgb = blend_colors(
				popup_bg_rgb, ਅਧਾਰ_fg, blend);
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    // blend_fg=FALSE: popup text is opaque.  Replace fg with
		    // popup's so the underlying syntax highlighting fg does
		    // not bleed.  ae_attr was already set above for this
		    // branch.  When the popup has no fg fall back to Normal's
		    // fg, then to white, so the text stays readable instead of
		    // rendering as default (which can be black on dark themes).
		    // Fall back to cterm color converted to RGB when the gui
		    // color is not set.
		    guicolor_T popup_fg = popup_aep->ae_u.cterm.fg_rgb;

		    ਜੇ (COLOR_INVALID(popup_fg)
			    && popup_aep->ae_u.cterm.fg_color > 0)
			popup_fg = cterm_color_ਮੰਜ਼ਿਲ_rgb(
				popup_aep->ae_u.cterm.fg_color);

		    ਜੇ (!COLOR_INVALID(popup_fg))
			new_en.ae_u.cterm.fg_rgb = popup_fg;
		    ਨਹੀਂਤਾਂ ਜੇ (!COLOR_INVALID(cterm_ਸਾਧਾਰਨ_fg_gui_color))
			new_en.ae_u.cterm.fg_rgb = cterm_ਸਾਧਾਰਨ_fg_gui_color;
		    ਨਹੀਂਤਾਂ
			new_en.ae_u.cterm.fg_rgb = fallback_fg_rgb;
		}
		ਜੇ (popup_bg_rgb != INVALCOLOR)
		{
		    // Blend popup bg toward underlying bg
		    guicolor_T underlying_bg = fallback_bg_rgb;
		    ਜੇ (char_aep != ਨਿਰਾ
			    && !COLOR_INVALID(char_aep->ae_u.cterm.bg_rgb))
			underlying_bg = char_aep->ae_u.cterm.bg_rgb;
		    new_en.ae_u.cterm.bg_rgb = blend_colors(
			    popup_bg_rgb, underlying_bg, blend);
		}
	    }
#ਜੇ_ਅੰਤ
	}
	ਵਾਪਸ get_attr_ਇੰਦਰਾਜ(&cterm_attr_ਸਾਰਣੀ, &new_en);
    }

    ਜੇ (char_attr > HL_ALL)
	char_aep = syn_ਪਦ_attr2entry(char_attr);
    ਜੇ (char_aep != ਨਿਰਾ)
	new_en = *char_aep;
    ਨਹੀਂਤਾਂ
    {
	CLEAR_ਖੇਤਰ(new_en);
	ਜੇ (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    // For term mode, no separate background color handling.
    ਵਾਪਸ get_attr_ਇੰਦਰਾਜ(&ਪਦ_attr_ਸਾਰਣੀ, &new_en);
}

/*
 * Blend attributes for popup windows with opacity.
 */
    ਪੂਰਨ
hl_blend_attr(ਪੂਰਨ char_attr, ਪੂਰਨ popup_attr, ਪੂਰਨ blend, ਪੂਰਨ blend_fg)
{
    ਜੇ (blend >= 100 && blend_fg)
	ਵਾਪਸ char_attr;

    ਵਾਪਸ hl_blend_attr_common(char_attr, popup_attr, blend, blend_fg);
}

/*
 * Blend underlying foreground and background colors for pum space cells.
 */
    ਪੂਰਨ
hl_pum_blend_attr(ਪੂਰਨ char_attr, ਪੂਰਨ popup_attr, ਪੂਰਨ blend)
{
    ਵਾਪਸ hl_blend_attr_common(char_attr, popup_attr, blend, TRUE);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    attrentry_T *
syn_gui_attr2entry(ਪੂਰਨ attr)
{
    attr -= ATTR_OFF;
    ਜੇ (attr >= gui_attr_ਸਾਰਣੀ.ga_len)	    // did ":syntax clear"
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ &(GUI_ATTR_ਇੰਦਰਾਜ(attr));
}
#ਜੇ_ਅੰਤ

/*
 * Get the highlight attributes (HL_BOLD etc.) from an attribute nr.
 * Only to be used when "attr" > HL_ALL.
 */
    ਪੂਰਨ
syn_attr2attr(ਪੂਰਨ attr)
{
    attrentry_T	*aep;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	aep = syn_gui_attr2entry(attr);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜੇ (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	ਨਹੀਂਤਾਂ
	    aep = syn_ਪਦ_attr2entry(attr);

    ਜੇ (aep == ਨਿਰਾ)	    // highlighting not set
	ਵਾਪਸ 0;
    ਵਾਪਸ aep->ae_attr;
}


    attrentry_T *
syn_ਪਦ_attr2entry(ਪੂਰਨ attr)
{
    attr -= ATTR_OFF;
    ਜੇ (attr >= ਪਦ_attr_ਸਾਰਣੀ.ga_len)	    // did ":syntax clear"
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ &(ਪਦ_ATTR_ਇੰਦਰਾਜ(attr));
}

    attrentry_T *
syn_cterm_attr2entry(ਪੂਰਨ attr)
{
    attr -= ATTR_OFF;
    ਜੇ (attr >= cterm_attr_ਸਾਰਣੀ.ga_len)	// did ":syntax clear"
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ &(CTERM_ATTR_ਇੰਦਰਾਜ(attr));
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_ATTR   1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_STRING 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_INT    3

    ਸਥਿਰ ਖਾਲੀ
highlight_list_one(ਪੂਰਨ id)
{
    hl_ਸਮੂਹ_T	    *sgp;
    ਪੂਰਨ		    didh = FALSE;

    sgp = &HL_ਸਾਰਣੀ()[id - 1];	    // index is ID minus one

    ਜੇ (ਸੁਨੇਹਾ_filtered(sgp->sg_name))
	ਵਾਪਸ;

    // Note: Keep this in sync with expand_highlight_group().
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_ਪਦ, ਨਿਰਾ, "term");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_ਸ਼ੁਰੂ, "start");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_stop, "stop");

    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_cterm, ਨਿਰਾ, "cterm");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_fg, ਨਿਰਾ, "ctermfg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_bg, ਨਿਰਾ, "ctermbg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_ul, ਨਿਰਾ, "ctermul");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_font, ਨਿਰਾ, "ctermfont");

#ਜੇ defined(FEAT_GUI) || defined(FEAT_EVAL)
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_gui, ਨਿਰਾ, "gui");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_fg_name, "guifg");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_bg_name, "guibg");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_sp_name, "guisp");
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_font_name, "font");
#ਜੇ_ਅੰਤ

    ਜੇ (sgp->sg_link && !got_int)
    {
	(ਖਾਲੀ)syn_list_ਸਿਰਲੇਖ(didh, 9999, id);
	didh = TRUE;
	ਸੁਨੇਹਾ_puts_attr("links to", HL_ATTR(HLF_D));
	ਸੁਨੇਹਾ_putchar(' ');
	ਸੁਨੇਹਾ_outtrans(HL_ਸਾਰਣੀ()[HL_ਸਾਰਣੀ()[id - 1].sg_link - 1].sg_name);
    }

    ਜੇ (!didh)
	highlight_list_arg(id, didh, LIST_STRING, 0, (char_u *)"cleared", "");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (p_verbose > 0)
	ਆਖਰੀ_ਸੈੱਟ_ਸੁਨੇਹਾ(sgp->sg_script_ctx);
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ char_u*
highlight_arg_ਮੰਜ਼ਿਲ_string(
    ਪੂਰਨ		type,
    ਪੂਰਨ		iarg,
    char_u	*sarg,
    char_u	*buf)
{
    ਜੇ (type == LIST_INT)
	sprintf((ਅੱਖਰ *)buf, "%d", iarg - 1);
    ਨਹੀਂਤਾਂ ਜੇ (type == LIST_STRING)
	ਵਾਪਸ sarg;
    ਨਹੀਂਤਾਂ // type == LIST_ATTR
    {
	size_t buflen;

	buf[0] = NUL;
	buflen = 0;
	ਲਈ (ਪੂਰਨ i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(highlight_ਸੂਚਕਾਂਕ_tab); ++i)
	{
	    ਜੇ (iarg & highlight_ਸੂਚਕਾਂਕ_tab[i]->ਕੁੰਜੀ)
	    {
		ਜੇ (buflen > 0)
		{
		    STRCPY(buf + buflen, (char_u *)",");
		    ++buflen;
		}
		STRCPY(buf + buflen, highlight_ਸੂਚਕਾਂਕ_tab[i]->ਮੁੱਲ.string);
		buflen += highlight_ਸੂਚਕਾਂਕ_tab[i]->ਮੁੱਲ.ਲੰਮਾਈ;
		iarg &= ~highlight_ਸੂਚਕਾਂਕ_tab[i]->ਕੁੰਜੀ;	    // don't want "inverse"/"reverse"
	    }
	}
    }
    ਵਾਪਸ buf;
}

    ਸਥਿਰ ਪੂਰਨ
highlight_list_arg(
    ਪੂਰਨ		id,
    ਪੂਰਨ		didh,
    ਪੂਰਨ		type,
    ਪੂਰਨ		iarg,
    char_u	*sarg,
    ਅੱਖਰ	*name)
{
    char_u	buf[MAX_ATTR_LEN];
    char_u	*ts;

    ਜੇ (got_int)
	ਵਾਪਸ FALSE;

    ਜੇ (type == LIST_STRING ? (sarg == ਨਿਰਾ) : (iarg == 0))
	ਵਾਪਸ didh;

    ts = highlight_arg_ਮੰਜ਼ਿਲ_string(type, iarg, sarg, buf);

    (ਖਾਲੀ)syn_list_ਸਿਰਲੇਖ(didh,
	    (ਪੂਰਨ)(vim_strsize(ts) + STRLEN(name) + 1), id);
    didh = TRUE;
    ਜੇ (!got_int)
    {
	ਜੇ (*name != NUL)
	{
	    ਸੁਨੇਹਾ_puts_attr(name, HL_ATTR(HLF_D));
	    ਸੁਨੇਹਾ_puts_attr("=", HL_ATTR(HLF_D));
	}
	ਸੁਨੇਹਾ_outtrans(ts);
    }
    ਵਾਪਸ didh;
}

#ਜੇ ((defined(FEAT_EVAL) || defined(FEAT_PRINTER))) && defined(FEAT_SYN_HL)
/*
 * Return "1" if highlight group "id" has attribute "flag".
 * Return NULL otherwise.
 */
    char_u *
highlight_has_attr(
    ਪੂਰਨ		id,
    ਪੂਰਨ		flag,
    ਪੂਰਨ		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    ਪੂਰਨ		attr;

    ਜੇ (id <= 0 || id > highlight_ga.ga_len)
	ਵਾਪਸ ਨਿਰਾ;

# ਜੇ defined(FEAT_GUI) || defined(FEAT_EVAL)
    ਜੇ (modec == 'g')
	attr = HL_ਸਾਰਣੀ()[id - 1].sg_gui;
    ਨਹੀਂਤਾਂ
# endif
    {
	ਜੇ (modec == 'c')
	    attr = HL_ਸਾਰਣੀ()[id - 1].sg_cterm;
	ਨਹੀਂਤਾਂ
	    attr = HL_ਸਾਰਣੀ()[id - 1].sg_ਪਦ;
    }

    ਜੇ (attr & flag)
	ਵਾਪਸ (char_u *)"1";
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_SYN_HL) && defined(FEAT_EVAL)
/*
 * Return color name of highlight group "id".
 */
    char_u *
highlight_color(
    ਪੂਰਨ		id,
    char_u	*what,	// "font", "fg", "bg", "sp", "ul", "fg#", "bg#" or "sp#"
    ਪੂਰਨ		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    ਸਥਿਰ char_u	name[20];
    ਪੂਰਨ			n;
    ਪੂਰਨ			fg = FALSE;
    ਪੂਰਨ			sp = FALSE;
    ਪੂਰਨ			ul = FALSE;
    ਪੂਰਨ			font = FALSE;

    ਜੇ (id <= 0 || id > highlight_ga.ga_len)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'g')
	fg = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'o'
	     && TOLOWER_ASC(what[2]) == 'n' && TOLOWER_ASC(what[3]) == 't')
	font = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ (TOLOWER_ASC(what[0]) == 's' && TOLOWER_ASC(what[1]) == 'p')
	sp = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ (TOLOWER_ASC(what[0]) == 'u' && TOLOWER_ASC(what[1]) == 'l')
	ul = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ (!(TOLOWER_ASC(what[0]) == 'b' && TOLOWER_ASC(what[1]) == 'g'))
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (modec == 'g')
    {
# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI
	// return font name
	ਜੇ (font)
	    ਵਾਪਸ HL_ਸਾਰਣੀ()[id - 1].sg_font_name;
#  endif

	// return #RRGGBB form (only possible when GUI is running)
	ਜੇ ((USE_24_ਦੋਆਧਾਰੀ_ਅੰਕ) && what[2] == '#')
	{
	    guicolor_T		color;
	    long_u		rgb;
	    ਸਥਿਰ char_u	buf[10];

	    ਜੇ (fg)
		color = HL_ਸਾਰਣੀ()[id - 1].sg_gui_fg;
	    ਨਹੀਂਤਾਂ ਜੇ (sp)
		color = HL_ਸਾਰਣੀ()[id - 1].sg_gui_sp;
	    ਨਹੀਂਤਾਂ
		color = HL_ਸਾਰਣੀ()[id - 1].sg_gui_bg;
	    ਜੇ (color == INVALCOLOR)
		ਵਾਪਸ ਨਿਰਾ;
	    rgb = (long_u)GUI_MCH_GET_RGB(color);
	    sprintf((ਅੱਖਰ *)buf, "#%02x%02x%02x",
				      (ਬਿਨਾਨਿਸ਼ਾਨੀ)(rgb >> 16),
				      (ਬਿਨਾਨਿਸ਼ਾਨੀ)(rgb >> 8) & 255,
				      (ਬਿਨਾਨਿਸ਼ਾਨੀ)rgb & 255);
	    ਵਾਪਸ buf;
	}
# endif
	ਜੇ (fg)
	    ਵਾਪਸ (HL_ਸਾਰਣੀ()[id - 1].sg_gui_fg_name);
	ਜੇ (sp)
	    ਵਾਪਸ (HL_ਸਾਰਣੀ()[id - 1].sg_gui_sp_name);
	ਵਾਪਸ (HL_ਸਾਰਣੀ()[id - 1].sg_gui_bg_name);
    }
    ਜੇ (sp)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (modec == 'c')
    {
	ਜੇ (fg)
	    n = HL_ਸਾਰਣੀ()[id - 1].sg_cterm_fg - 1;
	ਨਹੀਂਤਾਂ ਜੇ (ul)
	    n = HL_ਸਾਰਣੀ()[id - 1].sg_cterm_ul - 1;
	ਨਹੀਂਤਾਂ ਜੇ (font)
	    n = HL_ਸਾਰਣੀ()[id - 1].sg_cterm_font - 1;
	ਨਹੀਂਤਾਂ
	    n = HL_ਸਾਰਣੀ()[id - 1].sg_cterm_bg - 1;
	ਜੇ (n < 0)
	    ਵਾਪਸ ਨਿਰਾ;
	sprintf((ਅੱਖਰ *)name, "%d", n);
	ਵਾਪਸ name;
    }
    // term doesn't have color
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_SYN_HL) \
	    && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)) \
	&& defined(FEAT_PRINTER)
/*
 * Return color name of highlight group "id" as RGB value.
 */
    long_u
highlight_gui_color_rgb(
    ਪੂਰਨ		id,
    ਪੂਰਨ		fg)	// TRUE = fg, FALSE = bg
{
    guicolor_T	color;

    ਜੇ (id <= 0 || id > highlight_ga.ga_len)
	ਵਾਪਸ 0L;

    ਜੇ (fg)
	color = HL_ਸਾਰਣੀ()[id - 1].sg_gui_fg;
    ਨਹੀਂਤਾਂ
	color = HL_ਸਾਰਣੀ()[id - 1].sg_gui_bg;

    ਜੇ (color == INVALCOLOR)
	ਵਾਪਸ 0L;

    ਵਾਪਸ GUI_MCH_GET_RGB(color);
}
#ਜੇ_ਅੰਤ

/*
 * Output the syntax list header.
 * Return TRUE when started a new line.
 */
    ਪੂਰਨ
syn_list_ਸਿਰਲੇਖ(
    ਪੂਰਨ	    did_ਸਿਰਲੇਖ,		// did header already
    ਪੂਰਨ	    outlen,		// length of string that comes
    ਪੂਰਨ	    id)			// highlight group id
{
    ਪੂਰਨ	    endcol = 19;
    ਪੂਰਨ	    newline = TRUE;
    ਪੂਰਨ	    name_ਖਾਨਾ = 0;

    ਜੇ (!did_ਸਿਰਲੇਖ)
    {
	ਸੁਨੇਹਾ_putchar('\n');
	ਜੇ (got_int)
	    ਵਾਪਸ TRUE;
	ਸੁਨੇਹਾ_outtrans(HL_ਸਾਰਣੀ()[id - 1].sg_name);
	name_ਖਾਨਾ = ਸੁਨੇਹਾ_ਖਾਨਾ;
	endcol = 15;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ + outlen + 1 >= cmdline_ਚੌੜਾਈ)
    {
	ਸੁਨੇਹਾ_putchar('\n');
	ਜੇ (got_int)
	    ਵਾਪਸ TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ >= endcol)	// wrap around is like starting a new line
	    newline = FALSE;
    }

    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ >= endcol)	// output at least one space
	endcol = ਸੁਨੇਹਾ_ਖਾਨਾ + 1;
    ਜੇ (cmdline_ਚੌੜਾਈ <= endcol)	// avoid hang for tiny window
	endcol = cmdline_ਚੌੜਾਈ - 1;

    ਸੁਨੇਹਾ_advance(endcol);

    // Show "xxx" with the attributes.
    ਜੇ (!did_ਸਿਰਲੇਖ)
    {
	ਜੇ (endcol == cmdline_ਚੌੜਾਈ - 1 && endcol <= name_ਖਾਨਾ)
	    ਸੁਨੇਹਾ_putchar(' ');
	ਸੁਨੇਹਾ_puts_attr("xxx", syn_id2attr(id));
	ਸੁਨੇਹਾ_putchar(' ');
    }

    ਵਾਪਸ newline;
}

/*
 * Set the attribute numbers for a highlight group.
 * Called after one of the attributes has changed.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_hl_attr(
    ਪੂਰਨ		idx)	    // index in array
{
    attrentry_T	    at_en;
    hl_ਸਮੂਹ_T	    *sgp = HL_ਸਾਰਣੀ() + idx;

    // The "Normal" group doesn't need an attribute number
    ਜੇ (sgp->sg_name_u != ਨਿਰਾ && STRCMP(sgp->sg_name_u, "NORMAL") == 0)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // For the GUI mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    ਜੇ (sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
	    && sgp->sg_font == NOFONT
# ifdef FEAT_XFONTSET
	    && sgp->sg_fontset == NOFONTSET
# endif
	    )
    {
	sgp->sg_gui_attr = sgp->sg_gui;
    }
    ਨਹੀਂਤਾਂ
    {
	at_en.ae_attr = sgp->sg_gui;
	at_en.ae_u.gui.fg_color = sgp->sg_gui_fg;
	at_en.ae_u.gui.bg_color = sgp->sg_gui_bg;
	at_en.ae_u.gui.sp_color = sgp->sg_gui_sp;
	at_en.ae_u.gui.font = sgp->sg_font;
# ifdef FEAT_XFONTSET
	at_en.ae_u.gui.fontset = sgp->sg_fontset;
# endif
	sgp->sg_gui_attr = get_attr_ਇੰਦਰਾਜ(&gui_attr_ਸਾਰਣੀ, &at_en);
    }
#ਜੇ_ਅੰਤ
    // For the term mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    ਜੇ (sgp->sg_ਸ਼ੁਰੂ == ਨਿਰਾ && sgp->sg_stop == ਨਿਰਾ)
	sgp->sg_ਪਦ_attr = sgp->sg_ਪਦ;
    ਨਹੀਂਤਾਂ
    {
	at_en.ae_attr = sgp->sg_ਪਦ;
	at_en.ae_u.ਪਦ.ਸ਼ੁਰੂ = sgp->sg_ਸ਼ੁਰੂ;
	at_en.ae_u.ਪਦ.stop = sgp->sg_stop;
	sgp->sg_ਪਦ_attr = get_attr_ਇੰਦਰਾਜ(&ਪਦ_attr_ਸਾਰਣੀ, &at_en);
    }

    // For the color term mode: If there are other than "normal"
    // highlighting attributes, need to allocate an attr number.
    ਜੇ (sgp->sg_cterm_fg == 0 && sgp->sg_cterm_bg == 0 &&
	sgp->sg_cterm_ul == 0 && sgp->sg_cterm_font == 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
	    && sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
#ਜੇ_ਅੰਤ
	    )
	sgp->sg_cterm_attr = sgp->sg_cterm;
    ਨਹੀਂਤਾਂ
    {
	at_en.ae_attr = sgp->sg_cterm;
	at_en.ae_u.cterm.fg_color = sgp->sg_cterm_fg;
	at_en.ae_u.cterm.bg_color = sgp->sg_cterm_bg;
	at_en.ae_u.cterm.ul_color = sgp->sg_cterm_ul;
	at_en.ae_u.cterm.font = sgp->sg_cterm_font;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
	at_en.ae_u.cterm.fg_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_fg);
	at_en.ae_u.cterm.bg_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_bg);
	// Only use the underline/undercurl color when used, it may clear the
	// background color if not supported.
	ਜੇ (sgp->sg_cterm & (HL_UNDERLINE | HL_UNDERCURL
			   | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED))
	    at_en.ae_u.cterm.ul_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_sp);
	ਨਹੀਂਤਾਂ
	    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	ਜੇ (at_en.ae_u.cterm.fg_rgb == INVALCOLOR
		&& at_en.ae_u.cterm.bg_rgb == INVALCOLOR)
	{
	    // If both fg and bg are invalid fall back to the cterm colors.
	    // Helps when the GUI only uses an attribute, e.g. undercurl.
	    at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	    at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
	}
#ਜੇ_ਅੰਤ
	sgp->sg_cterm_attr = get_attr_ਇੰਦਰਾਜ(&cterm_attr_ਸਾਰਣੀ, &at_en);
    }
}

/*
 * Check if highlight id is overridden, and return the overriding highlight id.
 * Otherwise return the original id if no override.
 */
    ਸਥਿਰ ਪੂਰਨ
syn_override(ਪੂਰਨ id)
{
    ਜੇ (overrides != ਨਿਰਾ && overrides->arr != ਨਿਰਾ)
	ਲਈ (ਪੂਰਨ k = 0; k < overrides->len; k++)
	    ਜੇ (overrides->arr[k].from == id)
		ਵਾਪਸ overrides->arr[k].ਮੰਜ਼ਿਲ;
    ਵਾਪਸ id;
}

/*
 * Lookup a highlight group name by pointer and length.
 * If it is not found, 0 is returned.
 */
    ਸਥਿਰ ਪੂਰਨ
syn_name2id_len(char_u *name, ਪੂਰਨ len)
{
    char_u	name_u[MAX_SYN_NAME + 1];
    hashitem_T	*hi;

    ਜੇ (len > MAX_SYN_NAME)
	len = MAX_SYN_NAME;
    mch_memmove(name_u, name, len);
    name_u[len] = NUL;
    vim_strup(name_u);
    ਜੇ (!highlight_ht_inited)
	ਵਾਪਸ 0;
    hi = hash_ਲੱਭੋ(&highlight_ht, name_u);
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	ਵਾਪਸ 0;
    ਵਾਪਸ HI2HLNAME(hi)->hn_id;
}

/*
 * Lookup a highlight group name and return its ID.
 * If it is not found, 0 is returned.
 */
    ਪੂਰਨ
syn_name2id(char_u *name)
{
    // Avoid using stricmp() too much, it's slow on some systems.
    ਵਾਪਸ syn_name2id_len(name, (ਪੂਰਨ)STRLEN(name));
}

/*
 * Lookup a highlight group name and return its attributes.
 * Return zero if not found.
 */
    ਪੂਰਨ
syn_name2attr(char_u *name)
{
    ਪੂਰਨ id = syn_name2id(name);

    ਜੇ (id != 0)
	ਵਾਪਸ syn_id2attr(id);
    ਵਾਪਸ 0;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Return TRUE if highlight group "name" exists.
 */
    ਪੂਰਨ
highlight_exists(char_u *name)
{
    ਵਾਪਸ (syn_name2id(name) > 0);
}

# ਜੇ defined(FEAT_ਖੋਜ_EXTRA)
/*
 * Return the name of highlight group "id".
 * When not a valid ID return an empty string.
 */
    char_u *
syn_id2name(ਪੂਰਨ id)
{
    ਜੇ (id <= 0 || id > highlight_ga.ga_len)
	ਵਾਪਸ (char_u *)"";
    ਵਾਪਸ HL_ਸਾਰਣੀ()[id - 1].sg_name;
}
# endif
#ਜੇ_ਅੰਤ

/*
 * Like syn_name2id(), but take a pointer + length argument.
 */
    ਪੂਰਨ
syn_namen2id(char_u *linep, ਪੂਰਨ len)
{
    ਵਾਪਸ syn_name2id_len(linep, len);
}

/*
 * Find highlight group name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    ਪੂਰਨ
syn_check_ਸਮੂਹ(char_u *pp, ਪੂਰਨ len)
{
    ਪੂਰਨ	    id;
    char_u  *name;

    ਜੇ (len > MAX_SYN_NAME)
    {
	emsg(_(e_highlight_ਸਮੂਹ_name_too_long));
	ਵਾਪਸ 0;
    }
    id = syn_name2id_len(pp, len);
    ਜੇ (id == 0)			// doesn't exist yet
    {
	name = vim_strnsave(pp, len);
	ਜੇ (name == ਨਿਰਾ)
	    ਵਾਪਸ 0;
	id = syn_ਜੋੜੋ_ਸਮੂਹ(name);
    }
    ਵਾਪਸ id;
}

/*
 * Add new highlight group and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    ਸਥਿਰ ਪੂਰਨ
syn_ਜੋੜੋ_ਸਮੂਹ(char_u *name)
{
    char_u	*p;
    char_u	*name_ਉੱਪਰ;

    // Check that the name is valid (ASCII letters, digits, underscores, dots, or hyphens).
    ਲਈ (p = name; *p != NUL; ++p)
    {
	ਜੇ (!vim_isprintc(*p))
	{
	    emsg(_(e_unprintable_character_ਵਿੱਚ_ਸਮੂਹ_name));
	    vim_free(name);
	    ਵਾਪਸ 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (!ASCII_ISALNUM(*p) && *p != '_' && *p != '.' && *p != '-')
	{
	    // This is an error, but since there previously was no check only
	    // give a warning.
	    ਸੁਨੇਹਾ_ਸਰੋਤ(HL_ATTR(HLF_W));
	    ਸੁਨੇਹਾ(_("W18: Invalid character in group name"));
	    ਰੋਕੋ;
	}
    }

    // First call for this growarray: init growing array.
    ਜੇ (highlight_ga.ga_ਅੰਕੜੇ == ਨਿਰਾ)
    {
	highlight_ga.ga_itemsize = ਆਕਾਰ(hl_ਸਮੂਹ_T);
	highlight_ga.ga_growsize = 10;
	hash_init(&highlight_ht);
	highlight_ht_inited = true;
    }

    ਜੇ (highlight_ga.ga_len >= MAX_HL_ID)
    {
	emsg(_(e_too_many_highlight_and_syntax_groups));
	vim_free(name);
	ਵਾਪਸ 0;
    }

    // Make room for at least one other syntax_highlight entry.
    ਜੇ (ga_grow(&highlight_ga, 1) == FAIL)
    {
	vim_free(name);
	ਵਾਪਸ 0;
    }

    {
	hlname_T    *hn;
	ਪੂਰਨ	    len = (ਪੂਰਨ)STRLEN(name);

	hn = alloc(offsetof(hlname_T, hn_ਕੁੰਜੀ) + len + 1);
	ਜੇ (hn == ਨਿਰਾ)
	{
	    vim_free(name);
	    ਵਾਪਸ 0;
	}
	vim_strncpy(hn->hn_ਕੁੰਜੀ, name, len);
	vim_strup(hn->hn_ਕੁੰਜੀ);
	hn->hn_id = highlight_ga.ga_len + 1;	// ID is index plus one
	name_ਉੱਪਰ = hn->hn_ਕੁੰਜੀ;
    }

    CLEAR_ਸੂਚਕ(&(HL_ਸਾਰਣੀ()[highlight_ga.ga_len]));
    HL_ਸਾਰਣੀ()[highlight_ga.ga_len].sg_name = name;
    HL_ਸਾਰਣੀ()[highlight_ga.ga_len].sg_name_u = name_ਉੱਪਰ;
#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_ਸਾਰਣੀ()[highlight_ga.ga_len].sg_gui_bg = INVALCOLOR;
    HL_ਸਾਰਣੀ()[highlight_ga.ga_len].sg_gui_fg = INVALCOLOR;
    HL_ਸਾਰਣੀ()[highlight_ga.ga_len].sg_gui_sp = INVALCOLOR;
#ਜੇ_ਅੰਤ
    hash_ਜੋੜੋ(&highlight_ht, name_ਉੱਪਰ, "highlight");
    ++highlight_ga.ga_len;

    ਵਾਪਸ highlight_ga.ga_len;		    // ID is index plus one
}

/*
 * When, just after calling syn_add_group(), an error is discovered, this
 * function deletes the new name.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_unadd_ਸਮੂਹ(ਖਾਲੀ)
{
    hashitem_T *hi;

    --highlight_ga.ga_len;
    hi = hash_ਲੱਭੋ(&highlight_ht, HL_ਸਾਰਣੀ()[highlight_ga.ga_len].sg_name_u);
    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	hash_remove(&highlight_ht, hi, "highlight");
    vim_free(HL_ਸਾਰਣੀ()[highlight_ga.ga_len].sg_name);
    vim_free(HL_ਸਾਰਣੀ()[highlight_ga.ga_len].sg_name_u - HLNAME_ਕੁੰਜੀ_OFF);
}

/*
 * Translate a group ID to highlight attributes.
 * "hl_id" must be valid: > 0, caller must check.
 */
    ਪੂਰਨ
syn_id2attr(ਪੂਰਨ hl_id)
{
    ਪੂਰਨ		attr;
    hl_ਸਮੂਹ_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ਪ੍ਰਤਿਜ੍ਞਾ(hl_id > 0);
    sgp = &HL_ਸਾਰਣੀ()[hl_id - 1];	    // index is ID minus one

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Only use GUI attr when the GUI is being used.
    ਜੇ (gui.ਵਿੱਚ_use)
	attr = sgp->sg_gui_attr;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜੇ (IS_CTERM)
	    attr = sgp->sg_cterm_attr;
	ਨਹੀਂਤਾਂ
	    attr = sgp->sg_ਪਦ_attr;

    ਵਾਪਸ attr;
}

#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Get the GUI colors and attributes for a group ID.
 * NOTE: the colors will be INVALCOLOR when not set, the color otherwise.
 */
    ਪੂਰਨ
syn_id2colors(ਪੂਰਨ hl_id, guicolor_T *fgp, guicolor_T *bgp)
{
    hl_ਸਮੂਹ_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ਪ੍ਰਤਿਜ੍ਞਾ(hl_id > 0);
    sgp = &HL_ਸਾਰਣੀ()[hl_id - 1];	    // index is ID minus one

    *fgp = sgp->sg_gui_fg;
    *bgp = sgp->sg_gui_bg;
    ਵਾਪਸ sgp->sg_gui;
}
#ਜੇ_ਅੰਤ

#ਜੇ (defined(MSWIN) \
	    && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL)) \
	    && defined(FEAT_TERMGUICOLORS)) \
	|| defined(FEAT_TERMINAL)
    ਖਾਲੀ
syn_id2cterm_bg(ਪੂਰਨ hl_id, ਪੂਰਨ *fgp, ਪੂਰਨ *bgp)
{
    hl_ਸਮੂਹ_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ਪ੍ਰਤਿਜ੍ਞਾ(hl_id > 0);
    sgp = &HL_ਸਾਰਣੀ()[hl_id - 1];	    // index is ID minus one
    *fgp = sgp->sg_cterm_fg - 1;
    *bgp = sgp->sg_cterm_bg - 1;
}
#ਜੇ_ਅੰਤ

/*
 * Translate a group ID to the final group ID (following links).
 */
    ਪੂਰਨ
syn_get_final_id(ਪੂਰਨ hl_id)
{
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    hl_ਸਮੂਹ_T	*sgp;

    ਜੇ (hl_id > highlight_ga.ga_len || hl_id < 1)
	ਵਾਪਸ 0;			// Can be called from eval!!

    // Follow links until there is no more.
    // Look out for loops!  Break after 100 links.
    ਲਈ (ਗਿਣਤੀ_2 = 100; --ਗਿਣਤੀ_2 >= 0; )
    {
	ਪੂਰਨ tmp = hl_id;

	sgp = &HL_ਸਾਰਣੀ()[hl_id - 1];	    // index is ID minus one
	ਜੇ (sgp->sg_link == 0 || sgp->sg_link > highlight_ga.ga_len)
	    ਰੋਕੋ;

	// This is to handle highlight groups that are overridden but are in the
	// middle of a link chain.
	hl_id = syn_override(hl_id);
	ਜੇ (tmp != hl_id)
	    ਜਾਰੀ;
	hl_id = sgp->sg_link;
    }
    ਵਾਪਸ syn_override(hl_id);
}

#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Call this function just after the GUI has started.
 * Also called when 'termguicolors' was set, gui.in_use will be FALSE then.
 * It finds the font and color handles for the highlighting groups.
 */
    ਖਾਲੀ
highlight_gui_started(ਖਾਲੀ)
{
    ਪੂਰਨ	    idx;

    // First get the colors from the "Normal" and "Menu" group, if set
    ਜੇ (USE_24_ਦੋਆਧਾਰੀ_ਅੰਕ)
	ਸੈੱਟ_ਸਾਧਾਰਨ_colors();

    ਲਈ (idx = 0; idx < highlight_ga.ga_len; ++idx)
	gui_do_one_color(idx, FALSE, FALSE);

    highlight_ਬਦਲਿਆ();
}

    ਸਥਿਰ ਖਾਲੀ
gui_do_one_color(
    ਪੂਰਨ		idx,
    ਪੂਰਨ		do_menu ਅਣਵਰਤਿਆ,	   // TRUE: might set the menu font
    ਪੂਰਨ		do_tooltip ਅਣਵਰਤਿਆ) // TRUE: might set the tooltip font
{
    ਪੂਰਨ		didit = FALSE;

# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    ਜੇ (gui.ਵਿੱਚ_use)
#  endif
	ਜੇ (HL_ਸਾਰਣੀ()[idx].sg_font_name != ਨਿਰਾ)
	{
	    hl_do_font(idx, HL_ਸਾਰਣੀ()[idx].sg_font_name, FALSE, do_menu,
							    do_tooltip, TRUE);
	    didit = TRUE;
	}
# endif
    ਜੇ (HL_ਸਾਰਣੀ()[idx].sg_gui_fg_name != ਨਿਰਾ)
    {
	HL_ਸਾਰਣੀ()[idx].sg_gui_fg =
			    color_name2handle(HL_ਸਾਰਣੀ()[idx].sg_gui_fg_name);
	didit = TRUE;
    }
    ਜੇ (HL_ਸਾਰਣੀ()[idx].sg_gui_bg_name != ਨਿਰਾ)
    {
	HL_ਸਾਰਣੀ()[idx].sg_gui_bg =
			    color_name2handle(HL_ਸਾਰਣੀ()[idx].sg_gui_bg_name);
	didit = TRUE;
    }
    ਜੇ (HL_ਸਾਰਣੀ()[idx].sg_gui_sp_name != ਨਿਰਾ)
    {
	HL_ਸਾਰਣੀ()[idx].sg_gui_sp =
			    color_name2handle(HL_ਸਾਰਣੀ()[idx].sg_gui_sp_name);
	didit = TRUE;
    }
    ਜੇ (didit)	// need to get a new attr number
	ਸੈੱਟ_hl_attr(idx);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(USER_HIGHLIGHT) && defined(FEAT_STL_OPT)
/*
 * Apply difference between User[1-9] and HLF_S to HLF_SNC, HLF_ST or HLF_STNC.
 */
    ਸਥਿਰ ਖਾਲੀ
combine_stl_hlt(
	ਪੂਰਨ id,
	ਪੂਰਨ id_S,
	ਪੂਰਨ id_alt,
	ਪੂਰਨ hlcnt,
	ਪੂਰਨ i,
	ਪੂਰਨ hlf,
	ਪੂਰਨ *ਸਾਰਣੀ)
{
    hl_ਸਮੂਹ_T *hlt = HL_ਸਾਰਣੀ();

    ਜੇ (id_alt == 0)
    {
	CLEAR_ਸੂਚਕ(&hlt[hlcnt + i]);
	hlt[hlcnt + i].sg_ਪਦ = highlight_attr[hlf];
	hlt[hlcnt + i].sg_cterm = highlight_attr[hlf];
# ਜੇ defined(FEAT_GUI) || defined(FEAT_EVAL)
	hlt[hlcnt + i].sg_gui = highlight_attr[hlf];
# endif
    }
    ਨਹੀਂਤਾਂ
	mch_memmove(&hlt[hlcnt + i],
		    &hlt[id_alt - 1],
		    ਆਕਾਰ(hl_ਸਮੂਹ_T));
    hlt[hlcnt + i].sg_link = 0;

    hlt[hlcnt + i].sg_ਪਦ ^=
	hlt[id - 1].sg_ਪਦ ^ hlt[id_S - 1].sg_ਪਦ;
    ਜੇ (hlt[id - 1].sg_ਸ਼ੁਰੂ != hlt[id_S - 1].sg_ਸ਼ੁਰੂ)
	hlt[hlcnt + i].sg_ਸ਼ੁਰੂ = hlt[id - 1].sg_ਸ਼ੁਰੂ;
    ਜੇ (hlt[id - 1].sg_stop != hlt[id_S - 1].sg_stop)
	hlt[hlcnt + i].sg_stop = hlt[id - 1].sg_stop;
    hlt[hlcnt + i].sg_cterm ^=
	hlt[id - 1].sg_cterm ^ hlt[id_S - 1].sg_cterm;
    ਜੇ (hlt[id - 1].sg_cterm_fg != hlt[id_S - 1].sg_cterm_fg)
	hlt[hlcnt + i].sg_cterm_fg = hlt[id - 1].sg_cterm_fg;
    ਜੇ (hlt[id - 1].sg_cterm_bg != hlt[id_S - 1].sg_cterm_bg)
	hlt[hlcnt + i].sg_cterm_bg = hlt[id - 1].sg_cterm_bg;
    ਜੇ (hlt[id - 1].sg_cterm_font != hlt[id_S - 1].sg_cterm_font)
	hlt[hlcnt + i].sg_cterm_font = hlt[id - 1].sg_cterm_font;
# ਜੇ defined(FEAT_GUI) || defined(FEAT_EVAL)
    hlt[hlcnt + i].sg_gui ^=
	hlt[id - 1].sg_gui ^ hlt[id_S - 1].sg_gui;
# endif
# ifdef FEAT_GUI
    ਜੇ (hlt[id - 1].sg_gui_fg != hlt[id_S - 1].sg_gui_fg)
	hlt[hlcnt + i].sg_gui_fg = hlt[id - 1].sg_gui_fg;
    ਜੇ (hlt[id - 1].sg_gui_bg != hlt[id_S - 1].sg_gui_bg)
	hlt[hlcnt + i].sg_gui_bg = hlt[id - 1].sg_gui_bg;
    ਜੇ (hlt[id - 1].sg_gui_sp != hlt[id_S - 1].sg_gui_sp)
	hlt[hlcnt + i].sg_gui_sp = hlt[id - 1].sg_gui_sp;
    ਜੇ (hlt[id - 1].sg_font != hlt[id_S - 1].sg_font)
	hlt[hlcnt + i].sg_font = hlt[id - 1].sg_font;
#  ifdef FEAT_XFONTSET
    ਜੇ (hlt[id - 1].sg_fontset != hlt[id_S - 1].sg_fontset)
	hlt[hlcnt + i].sg_fontset = hlt[id - 1].sg_fontset;
#  endif
# endif
    highlight_ga.ga_len = hlcnt + i + 1;
    ਸੈੱਟ_hl_attr(hlcnt + i);	// At long last we can apply
    ਸਾਰਣੀ[i] = syn_id2attr(hlcnt + i + 1);
}
#ਜੇ_ਅੰਤ

/*
 * Translate the 'highlight' option into attributes in highlight_attr[] and
 * set up the user highlights User1..9.  If FEAT_STL_OPT is in use, a set of
 * corresponding highlights to use on top of HLF_SNC is computed.
 * Called only when the 'highlight' option has been changed and upon first
 * screen redraw after any :highlight command.
 * Return FAIL when an invalid flag is found in 'highlight'.  OK otherwise.
 */
    ਪੂਰਨ
highlight_ਬਦਲਿਆ(ਖਾਲੀ)
{
    ਪੂਰਨ		hlf;
    ਪੂਰਨ		i;
    char_u	*p;
    char_u	*default_hl;
    ਪੂਰਨ		attr;
    char_u	*ਸਿਰਾ;
    ਪੂਰਨ		id;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USER_HIGHLIGHT
    char_u      userhl[30];  // use 30 to avoid compiler warning
# ifdef FEAT_STL_OPT
    ਪੂਰਨ		id_S = -1;
    ਪੂਰਨ		id_SNC = 0;
#  ifdef FEAT_TERMINAL
    ਪੂਰਨ		id_ST = 0;
    ਪੂਰਨ		id_STNC = 0;
#  endif
    ਪੂਰਨ		hlcnt;
# endif
#ਜੇ_ਅੰਤ
    win_T	*wp;

    need_highlight_ਬਦਲਿਆ = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਪਦ_update_colors_all();
    ਪਦ_update_hlfwin_all();
#ਜੇ_ਅੰਤ

    // Clear all attributes.
    ਲਈ (hlf = 0; hlf < (ਪੂਰਨ)HLF_ਗਿਣਤੀ; ++hlf)
    {
	highlight_attr[hlf] = 0;
	highlight_attr_raw[hlf] = 0;
	highlight_ids[hlf] = 0;
    }

    default_hl = get_highlight_default();

    // First set all attributes to their default value.
    // Then use the attributes from the 'highlight' option, unless it already
    // matches the default and would repeat the same work.
    ਲਈ (i = 0; i < 2; ++i)
    {
	ਜੇ (i)
	{
	    ਜੇ (default_hl != ਨਿਰਾ && p_hl != ਨਿਰਾ
				      && STRCMP(default_hl, p_hl) == 0)
		ਜਾਰੀ;
	    p = p_hl;
	}
	ਨਹੀਂਤਾਂ
	    p = default_hl;
	ਜੇ (p == ਨਿਰਾ)	    // just in case
	    ਜਾਰੀ;

	ਜਦੋਂ (*p)
	{
	    ਲਈ (hlf = 0; hlf < (ਪੂਰਨ)HLF_ਗਿਣਤੀ; ++hlf)
		ਜੇ (hl_flags[hlf] == *p)
		    ਰੋਕੋ;
	    ++p;
	    ਜੇ (hlf == (ਪੂਰਨ)HLF_ਗਿਣਤੀ || *p == NUL)
		ਵਾਪਸ FAIL;

	    // Allow several hl_flags to be combined, like "bu" for
	    // bold-underlined.
	    attr = 0;
	    id = 0;
	    ਲਈ ( ; *p && *p != ','; ++p)	    // parse up to comma
	    {
		ਜੇ (VIM_ISWHITE(*p))		    // ignore white space
		    ਜਾਰੀ;

		ਜੇ (attr > HL_ALL)  // Combination with ':' is not allowed.
		    ਵਾਪਸ FAIL;

		// Note: Keep this in sync with expand_set_highlight().
		ਚੋਣ (*p)
		{
		    ਹਾਲਤ 'b':	attr |= HL_BOLD;
				ਰੋਕੋ;
		    ਹਾਲਤ 'i':	attr |= HL_ITALIC;
				ਰੋਕੋ;
		    ਹਾਲਤ '-':
		    ਹਾਲਤ 'n':			    // no highlighting
				ਰੋਕੋ;
		    ਹਾਲਤ 'r':	attr |= HL_INVERSE;
				ਰੋਕੋ;
		    ਹਾਲਤ 's':	attr |= HL_STANDOUT;
				ਰੋਕੋ;
		    ਹਾਲਤ 'u':	attr |= HL_UNDERLINE;
				ਰੋਕੋ;
		    ਹਾਲਤ 'c':	attr |= HL_UNDERCURL;
				ਰੋਕੋ;
		    ਹਾਲਤ '2':	attr |= HL_UNDERDOUBLE;
				ਰੋਕੋ;
		    ਹਾਲਤ 'd':	attr |= HL_UNDERDOTTED;
				ਰੋਕੋ;
		    ਹਾਲਤ '=':	attr |= HL_UNDERDASHED;
				ਰੋਕੋ;
		    ਹਾਲਤ 't':	attr |= HL_STRIKETHROUGH;
				ਰੋਕੋ;
		    ਹਾਲਤ ':':	++p;		    // highlight group name
				ਜੇ (attr || *p == NUL)	 // no combinations
				    ਵਾਪਸ FAIL;
				ਸਿਰਾ = vim_strchr(p, ',');
				ਜੇ (ਸਿਰਾ == ਨਿਰਾ)
				    ਸਿਰਾ = p + STRLEN(p);
				id = syn_check_ਸਮੂਹ(p, (ਪੂਰਨ)(ਸਿਰਾ - p));
				ਜੇ (id == 0)
				    ਵਾਪਸ FAIL;
				attr = syn_id2attr(id);
				p = ਸਿਰਾ - 1;
#ਜੇ defined(FEAT_STL_OPT) && defined(USER_HIGHLIGHT)
				ਜੇ (hlf == (ਪੂਰਨ)HLF_SNC)
				    id_SNC = syn_get_final_id(id);
# ifdef FEAT_TERMINAL
				ਨਹੀਂਤਾਂ ਜੇ (hlf == (ਪੂਰਨ)HLF_ST)
				    id_ST = syn_get_final_id(id);
				ਨਹੀਂਤਾਂ ਜੇ (hlf == (ਪੂਰਨ)HLF_STNC)
				    id_STNC = syn_get_final_id(id);
# endif
				ਨਹੀਂਤਾਂ ਜੇ (hlf == (ਪੂਰਨ)HLF_S)
				    id_S = syn_get_final_id(id);
#ਜੇ_ਅੰਤ
				ਰੋਕੋ;
		    ਮੂਲ:	ਵਾਪਸ FAIL;
		}
	    }
	    highlight_attr[hlf] = attr;
	    highlight_attr_raw[hlf] = attr;
	    highlight_ids[hlf] = id;

	    p = skip_ਮੰਜ਼ਿਲ_option_part(p);	    // skip comma and spaces
	}
    }

    // Highlight ids may have been changed, so keep windows up to date
    FOR_ALL_WINDOWS(wp)
    {
	ਅੱਖਰ *errmsg = update_winhighlight(wp, wp->w_p_whl);

	ਜੇ (errmsg != ਨਿਰਾ)
	    emsg(_(errmsg));
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਪਦ_update_hlfwin_all();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USER_HIGHLIGHT
    // Setup the user highlights
    //
    // Temporarily utilize 28 more hl entries:
    // 9 for User1-User9 combined with StatusLineNC
    // 9 for User1-User9 combined with StatusLineTerm
    // 9 for User1-User9 combined with StatusLineTermNC
    // 1 for StatusLine default
    // Have to be in there simultaneously in case of table overflows in
    // get_attr_entry()
# ifdef FEAT_STL_OPT
    ਜੇ (ga_grow(&highlight_ga, 28) == FAIL)
	ਵਾਪਸ FAIL;
    hlcnt = highlight_ga.ga_len;
    ਜੇ (id_S == -1)
    {
	// Make sure id_S is always valid to simplify code below. Use the last
	// entry.
	CLEAR_ਸੂਚਕ(&HL_ਸਾਰਣੀ()[hlcnt + 27]);
	HL_ਸਾਰਣੀ()[hlcnt + 18].sg_ਪਦ = highlight_attr[HLF_S];
	id_S = hlcnt + 19;
    }
# endif
    ਲਈ (i = 0; i < 9; i++)
    {
	sprintf((ਅੱਖਰ *)userhl, "User%d", i + 1);
	id = syn_name2id(userhl);
	ਜੇ (id == 0)
	{
	    highlight_user[i] = 0;
# ifdef FEAT_STL_OPT
	    highlight_stlnc[i] = 0;
#  ifdef FEAT_TERMINAL
	    highlight_stlterm[i] = 0;
	    highlight_stltermnc[i] = 0;
#  endif
# endif
	}
	ਨਹੀਂਤਾਂ
	{
	    highlight_user[i] = syn_id2attr(id);
# ifdef FEAT_STL_OPT
	    combine_stl_hlt(id, id_S, id_SNC, hlcnt, i,
						     HLF_SNC, highlight_stlnc);
#  ifdef FEAT_TERMINAL
	    combine_stl_hlt(id, id_S, id_ST, hlcnt + 9, i,
						    HLF_ST, highlight_stlterm);
	    combine_stl_hlt(id, id_S, id_STNC, hlcnt + 18, i,
						HLF_STNC, highlight_stltermnc);
#  endif
# endif
	}
    }
# ifdef FEAT_STL_OPT
    highlight_ga.ga_len = hlcnt;
# endif

#ਜੇ_ਅੰਤ // USER_HIGHLIGHT

    ਵਾਪਸ OK;
}

ਸਥਿਰ ਖਾਲੀ highlight_list(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ highlight_list_two(ਪੂਰਨ cnt, ਪੂਰਨ attr);

// context for :highlight <group> <arg> expansion
ਸਥਿਰ ਪੂਰਨ expand_hi_synid = 0;	    // ID for highlight group being completed
ਸਥਿਰ ਪੂਰਨ expand_hi_equal_ਖਾਨਾ = 0; // column where the '=' is
ਸਥਿਰ ਪੂਰਨ expand_hi_include_orig = 0;	    // whether to fill the existing current value or not
ਸਥਿਰ char_u *expand_hi_curvalue = ਨਿਰਾ;   // the existing current value
#ਜੇ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
ਸਥਿਰ dict_iterator_T expand_colornames_iter;	// iterator for looping through v:colornames
#ਜੇ_ਅੰਤ

/*
 * Handle command line completion for :highlight command.
 */
    ਖਾਲੀ
ਸੈੱਟ_context_ਵਿੱਚ_highlight_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;
    ਪੂਰਨ		expand_ਸਮੂਹ = TRUE;

    // Default: expand group names
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_ਨਮੂਨਾ = arg;
    include_none = 0;
    include_link = 2;
    include_default = 1;

    ਜੇ (*arg == NUL)
	ਵਾਪਸ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ਜੇ (*p == NUL)
	ਵਾਪਸ;

    // past "default" or group name
    include_default = 0;
    ਜੇ (STRNCMP("default", arg, p - arg) == 0)
    {
	arg = skipwhite(p);
	xp->xp_ਨਮੂਨਾ = arg;
	p = skiptowhite(arg);
    }
    ਜੇ (*p == NUL)
	ਵਾਪਸ;

    // past group name
    include_link = 0;
    ਜੇ (arg[1] == 'i' && arg[0] == 'N')
    {
	highlight_list();
	expand_ਸਮੂਹ = FALSE;
    }
    ਜੇ (STRNCMP("link", arg, p - arg) == 0)
    {
	xp->xp_ਨਮੂਨਾ = skipwhite(p);
	p = skiptowhite(xp->xp_ਨਮੂਨਾ);
	ਜੇ (*p != NUL)		// past first group name
	{
	    xp->xp_ਨਮੂਨਾ = skipwhite(p);
	    p = skiptowhite(xp->xp_ਨਮੂਨਾ);
	    include_none = 1;
	}
	expand_ਸਮੂਹ = FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP("clear", arg, p - arg) == 0)
    {
	xp->xp_ਨਮੂਨਾ = skipwhite(p);
	p = skiptowhite(xp->xp_ਨਮੂਨਾ);
	expand_ਸਮੂਹ = FALSE;
    }
    ਜੇ (*p != NUL)			// past group name(s)
    {
	ਜੇ (expand_ਸਮੂਹ)
	{
	    // expansion will be done in expand_highlight_group()
	    xp->xp_context = EXPAND_HIGHLIGHT_ਸਮੂਹ;

	    expand_hi_synid = syn_namen2id(arg, (ਪੂਰਨ)(p - arg));

	    ਜਦੋਂ (*p != NUL)
	    {
		arg = skipwhite(p);
		p = skiptowhite(arg);
	    }

	    p = vim_strchr(arg, '=');
	    ਜੇ (p == ਨਿਰਾ)
	    {
		// Didn't find a key=<value> pattern
		xp->xp_ਨਮੂਨਾ = arg;
		expand_hi_equal_ਖਾਨਾ = -1;
		expand_hi_include_orig = FALSE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Found key=<value> pattern, record the exact location
		expand_hi_equal_ਖਾਨਾ = (ਪੂਰਨ)(p - xp->xp_ਸਤਰ);

		// Only include the original value if the pattern is empty
		ਜੇ (*(p + 1) == NUL)
		    expand_hi_include_orig = TRUE;
		ਨਹੀਂਤਾਂ
		    expand_hi_include_orig = FALSE;

		// Account for comma-separated values
		ਜੇ (STRNCMP(arg, "term=", 5) == 0 ||
			STRNCMP(arg, "cterm=", 6) == 0 ||
			STRNCMP(arg, "gui=", 4) == 0)
		{
		    char_u *comma = vim_strrchr(p + 1, ',');
		    ਜੇ (comma != ਨਿਰਾ)
			p = comma;
		}
		xp->xp_ਨਮੂਨਾ = p + 1;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    xp->xp_context = EXPAND_NOTHING;
	}
    }
}

/*
 * List highlighting matches in a nice way.
 */
    ਸਥਿਰ ਖਾਲੀ
highlight_list(ਖਾਲੀ)
{
    ਪੂਰਨ		i;

    ਲਈ (i = 10; --i >= 0; )
	highlight_list_two(i, HL_ATTR(HLF_D));
    ਲਈ (i = 40; --i >= 0; )
	highlight_list_two(99, 0);
}

    ਸਥਿਰ ਖਾਲੀ
highlight_list_two(ਪੂਰਨ cnt, ਪੂਰਨ attr)
{
    ਸੁਨੇਹਾ_puts_attr(&("N \bI \b!  \b"[cnt / 11]), attr);
    ਸੁਨੇਹਾ_clr_eos();
    ਬਾਹਰਲਾ_flush();
    ui_delay(cnt == 99 ? 40L : (ਲੰਮਾ)cnt * 50L, FALSE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_highlight_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਵਾਪਸ get_highlight_name_ext(xp, idx, TRUE);
}

/*
 * Obtain a highlight group name.
 * When "skip_cleared" is TRUE don't return a cleared entry.
 */
    char_u *
get_highlight_name_ext(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx, ਪੂਰਨ skip_cleared)
{
    ਜੇ (idx < 0)
	ਵਾਪਸ ਨਿਰਾ;

    // Items are never removed from the table, skip the ones that were
    // cleared.
    ਜੇ (skip_cleared && idx < highlight_ga.ga_len && HL_ਸਾਰਣੀ()[idx].sg_cleared)
	ਵਾਪਸ (char_u *)"";

    ਜੇ (idx == highlight_ga.ga_len && include_none != 0)
	ਵਾਪਸ (char_u *)"NONE";
    ਜੇ (idx == highlight_ga.ga_len + include_none && include_default != 0)
	ਵਾਪਸ (char_u *)"default";
    ਜੇ (idx == highlight_ga.ga_len + include_none + include_default
							 && include_link != 0)
	ਵਾਪਸ (char_u *)"link";
    ਜੇ (idx == highlight_ga.ga_len + include_none + include_default + 1
							 && include_link != 0)
	ਵਾਪਸ (char_u *)"clear";
    ਜੇ (idx >= highlight_ga.ga_len)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ HL_ਸਾਰਣੀ()[idx].sg_name;
}

    ਸਥਿਰ char_u *
get_highlight_attr_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx == 0)
    {
	// Fill with current value first
	ਜੇ (expand_hi_curvalue != ਨਿਰਾ)
	    ਵਾਪਸ expand_hi_curvalue;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ (char_u*)"";
    }
    ਜੇ (idx < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(highlight_ਸੂਚਕਾਂਕ_tab) + 1)
    {
	char_u *ਮੁੱਲ = highlight_ਸੂਚਕਾਂਕ_tab[idx-1]->ਮੁੱਲ.string;
	ਜੇ (expand_hi_curvalue != ਨਿਰਾ && STRCMP(expand_hi_curvalue, ਮੁੱਲ) == 0)
	{
	    // Already returned the current value above, just skip.
	    ਵਾਪਸ (char_u*)"";
	}
	ਵਾਪਸ ਮੁੱਲ;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ char_u *
get_highlight_cterm_color(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx == 0)
    {
	// Fill with current value first
	ਜੇ (expand_hi_curvalue != ਨਿਰਾ)
	    ਵਾਪਸ expand_hi_curvalue;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ (char_u*)"";
    }
    // See highlight_set_cterm_color()
    ਨਹੀਂਤਾਂ ਜੇ (idx == 1)
	ਵਾਪਸ (char_u*)"fg";
    ਨਹੀਂਤਾਂ ਜੇ (idx == 2)
	ਵਾਪਸ (char_u*)"bg";
    ਜੇ (idx < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(color_name_tab) + 3)
    {
	char_u *ਮੁੱਲ = color_name_tab[idx-3].ਮੁੱਲ.string;
	ਵਾਪਸ ਮੁੱਲ;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
    ਸਥਿਰ char_u *
get_highlight_gui_color(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx == 0)
    {
	// Fill with current value first
	ਜੇ (expand_hi_curvalue != ਨਿਰਾ)
	    ਵਾਪਸ expand_hi_curvalue;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ (char_u*)"";
    }
    // See color_name2handle()
    ਨਹੀਂਤਾਂ ਜੇ (idx == 1)
	ਵਾਪਸ (char_u*)"fg";
    ਨਹੀਂਤਾਂ ਜੇ (idx == 2)
	ਵਾਪਸ (char_u*)"bg";
    ਨਹੀਂਤਾਂ ਜੇ (idx == 3)
	ਵਾਪਸ (char_u*)"NONE";

    // Complete from v:colornames. Don't do platform specific names for now.
    typval_T *tv_ਨਤੀਜਾ;
    char_u *colorname = dict_iterate_ਅਗਲਾ(&expand_colornames_iter, &tv_ਨਤੀਜਾ);
    ਜੇ (colorname != ਨਿਰਾ)
    {
	// :hi command doesn't allow space, so don't suggest any malformed items
	ਜੇ (vim_strchr(colorname, ' ') != ਨਿਰਾ)
	    ਵਾਪਸ (char_u*)"";

	ਜੇ (expand_hi_curvalue != ਨਿਰਾ && STRICMP(expand_hi_curvalue, colorname) == 0)
	{
	    // Already returned the current value above, just skip.
	    ਵਾਪਸ (char_u*)"";
	}
    }
    ਵਾਪਸ colorname;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ char_u *
get_highlight_ਸਮੂਹ_ਕੁੰਜੀ(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    // Note: Keep this in sync with do_highlight.
    ਸਥਿਰ ਅੱਖਰ *(p_hi_ਸਮੂਹ_ਕੁੰਜੀ_ਮੁੱਲਾਂ[]) =
    {
	"term=",
	"start=",
	"stop=",
	"cterm=",
	"ctermfg=",
	"ctermbg=",
	"ctermul=",
	"ctermfont=",
#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	"gui=",
	"guifg=",
	"guibg=",
	"guisp=",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	"font=",
#ਜੇ_ਅੰਤ
	"NONE",
    };

    ਜੇ (idx < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(p_hi_ਸਮੂਹ_ਕੁੰਜੀ_ਮੁੱਲਾਂ))
	ਵਾਪਸ (char_u*)p_hi_ਸਮੂਹ_ਕੁੰਜੀ_ਮੁੱਲਾਂ[idx];
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Command-line expansion for :hi {group-name} <args>...
 */
    ਪੂਰਨ
expand_highlight_ਸਮੂਹ(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	ਪੂਰਨ	    *ਅੰਕ_Matches)
{
    ਜੇ (expand_hi_equal_ਖਾਨਾ != -1)
    {
	// List the values. First fill in the current value, then if possible colors
	// or attribute names.
	char_u	    *(*expandfunc)(expand_T *, ਪੂਰਨ) = ਨਿਰਾ;
	ਪੂਰਨ	    type = 0;
	hl_ਸਮੂਹ_T  *sgp = ਨਿਰਾ;
	ਪੂਰਨ	    iarg = 0;
	char_u	    *sarg = ਨਿਰਾ;

	ਪੂਰਨ	    unsorted_ਚੀਜ਼ਾਂ = -1; // don't sort by default

	ਜੇ (expand_hi_synid != 0)
	    sgp = &HL_ਸਾਰਣੀ()[expand_hi_synid - 1]; // index is ID minus one

	// Note: Keep this in sync with highlight_list_one().
	char_u	    *name_ਸਿਰਾ = xp->xp_ਸਤਰ + expand_hi_equal_ਖਾਨਾ;
	ਜੇ (name_ਸਿਰਾ - xp->xp_ਸਤਰ >= 5
		&& STRNCMP(name_ਸਿਰਾ - 5, " term", 5) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ਜੇ (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_ਪਦ;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (name_ਸਿਰਾ - xp->xp_ਸਤਰ >= 6
		&& STRNCMP(name_ਸਿਰਾ - 6, " cterm", 6) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ਜੇ (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_cterm;
	    }
	}
#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	ਨਹੀਂਤਾਂ ਜੇ (name_ਸਿਰਾ - xp->xp_ਸਤਰ >= 4
		&& STRNCMP(name_ਸਿਰਾ - 4, " gui", 4) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ਜੇ (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_gui;
	    }
	}
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ ਜੇ (name_ਸਿਰਾ - xp->xp_ਸਤਰ >= 8
		&& STRNCMP(name_ਸਿਰਾ - 8, " ctermfg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ਜੇ (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_fg;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (name_ਸਿਰਾ - xp->xp_ਸਤਰ >= 8
		&& STRNCMP(name_ਸਿਰਾ - 8, " ctermbg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ਜੇ (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_bg;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (name_ਸਿਰਾ - xp->xp_ਸਤਰ >= 8
		&& STRNCMP(name_ਸਿਰਾ - 8, " ctermul", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ਜੇ (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_ul;
	    }
	}
#ਜੇ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	ਨਹੀਂਤਾਂ ਜੇ (name_ਸਿਰਾ - xp->xp_ਸਤਰ >= 6
		&& STRNCMP(name_ਸਿਰਾ - 6, " guifg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ਜੇ (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_fg_name;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (name_ਸਿਰਾ - xp->xp_ਸਤਰ >= 6
		&& STRNCMP(name_ਸਿਰਾ - 6, " guibg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ਜੇ (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_bg_name;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (name_ਸਿਰਾ - xp->xp_ਸਤਰ >= 6
		&& STRNCMP(name_ਸਿਰਾ - 6, " guisp", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ਜੇ (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_sp_name;
	    }
	}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	ਜੇ (expandfunc == get_highlight_gui_color)
	{
	    // Top 4 items are special, after that sort all the color names
	    unsorted_ਚੀਜ਼ਾਂ = 4;

	    dict_T *colornames_ਸਾਰਣੀ = get_vim_var_dict(VV_COLORNAMES);
	    typval_T colornames_val;
	    colornames_val.v_type = VAR_DICT;
	    colornames_val.vval.v_dict = colornames_ਸਾਰਣੀ;
	    dict_iterate_ਸ਼ੁਰੂ(&colornames_val, &expand_colornames_iter);
	}
#ਜੇ_ਅੰਤ

	char_u	    buf[MAX_ATTR_LEN];

	expand_hi_curvalue = ਨਿਰਾ;
	ਜੇ (expand_hi_include_orig)
	{
	    ਜੇ (((type == LIST_ATTR || type == LIST_INT) && iarg != 0) ||
		(type == LIST_STRING && sarg != ਨਿਰਾ))
	    {
		// Retrieve the current value to go first in completion
		expand_hi_curvalue = highlight_arg_ਮੰਜ਼ਿਲ_string(
			type, iarg, sarg, buf);
	    }
	}

	ਜੇ (expandfunc != ਨਿਰਾ)
	{
	    ਵਾਪਸ ExpandGenericExt(
		    pat,
		    xp,
		    rmp,
		    matches,
		    ਅੰਕ_Matches,
		    expandfunc,
		    FALSE,
		    unsorted_ਚੀਜ਼ਾਂ);
	}

	ਵਾਪਸ FAIL;
    }

    // List all the key names
    ਵਾਪਸ ExpandGenericExt(
	    pat,
	    xp,
	    rmp,
	    matches,
	    ਅੰਕ_Matches,
	    get_highlight_ਸਮੂਹ_ਕੁੰਜੀ,
	    FALSE,
	    -1);
}

#ਜੇ defined(FEAT_GUI)
/*
 * Free all the highlight group fonts.
 * Used when quitting for systems which need it.
 */
    ਖਾਲੀ
free_highlight_fonts(ਖਾਲੀ)
{
    ਪੂਰਨ	    idx;

    ਲਈ (idx = 0; idx < highlight_ga.ga_len; ++idx)
    {
	gui_mch_free_font(HL_ਸਾਰਣੀ()[idx].sg_font);
	HL_ਸਾਰਣੀ()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
	gui_mch_free_fontset(HL_ਸਾਰਣੀ()[idx].sg_fontset);
	HL_ਸਾਰਣੀ()[idx].sg_fontset = NOFONTSET;
# endif
    }

    gui_mch_free_font(gui.norm_font);
# ifdef FEAT_XFONTSET
    gui_mch_free_fontset(gui.fontset);
# endif
# ifndef FEAT_GUI_GTK
    gui_mch_free_font(gui.bold_font);
    gui_mch_free_font(gui.ital_font);
    gui_mch_free_font(gui.boldital_font);
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * Convert each of the highlight attribute bits (bold, standout, underline,
 * etc.) set in 'hlattr' into a separate boolean item in a Dictionary with
 * the attribute name as the key.
 */
    ਸਥਿਰ dict_T *
highlight_get_attr_dict(ਪੂਰਨ hlattr)
{
    dict_T	*dict;
    ਪੂਰਨ		i;

    dict = dict_alloc();
    ਜੇ (dict == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(highlight_ਸੂਚਕਾਂਕ_tab); ++i)
    {
	ਜੇ (hlattr & highlight_ਸੂਚਕਾਂਕ_tab[i]->ਕੁੰਜੀ)
	{
	    dict_ਜੋੜੋ_bool(dict, (ਅੱਖਰ *)highlight_ਸੂਚਕਾਂਕ_tab[i]->ਮੁੱਲ.string,
		    VVAL_TRUE);
	    // don't want "inverse"/"reverse"
	    hlattr &= ~highlight_ਸੂਚਕਾਂਕ_tab[i]->ਕੁੰਜੀ;
	}
    }

    ਵਾਪਸ dict;
}

/*
 * Return the attributes of the highlight group at index 'hl_idx' as a
 * Dictionary. If 'resolve_link' is TRUE, then resolves the highlight group
 * links recursively.
 */
    ਸਥਿਰ dict_T *
highlight_get_info(ਪੂਰਨ hl_idx, ਪੂਰਨ resolve_link)
{
    dict_T	*dict;
    hl_ਸਮੂਹ_T	*sgp;
    dict_T	*attr_dict = ਨਿਰਾ;
    ਪੂਰਨ		hlgid;

    dict = dict_alloc();
    ਜੇ (dict == ਨਿਰਾ)
	ਵਾਪਸ dict;

    sgp = &HL_ਸਾਰਣੀ()[hl_idx];
    // highlight group id is 1-based
    hlgid = hl_idx + 1;

    ਜੇ (dict_ਜੋੜੋ_string(dict, "name", sgp->sg_name) == FAIL)
	ਜਾਓ error;
    ਜੇ (dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "id", hlgid) == FAIL)
	ਜਾਓ error;

    ਜੇ (resolve_link)
    {
	// resolve the highlight group link recursively
	ਜਦੋਂ (sgp->sg_link)
	{
	    hlgid = sgp->sg_link;
	    sgp = &HL_ਸਾਰਣੀ()[sgp->sg_link - 1];
	}
    }

    ਜੇ (sgp->sg_ਪਦ != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_ਪਦ);
	ਜੇ (attr_dict != ਨਿਰਾ)
	{
	    ਜੇ (dict_ਜੋੜੋ_dict(dict, "term", attr_dict) == FAIL)
		ਜਾਓ error;
	    attr_dict = ਨਿਰਾ;
	}
    }
    ਜੇ (sgp->sg_ਸ਼ੁਰੂ != ਨਿਰਾ)
	ਜੇ (dict_ਜੋੜੋ_string(dict, "start", sgp->sg_ਸ਼ੁਰੂ) == FAIL)
	    ਜਾਓ error;
    ਜੇ (sgp->sg_stop != ਨਿਰਾ)
	ਜੇ (dict_ਜੋੜੋ_string(dict, "stop", sgp->sg_stop) == FAIL)
	    ਜਾਓ error;
    ਜੇ (sgp->sg_cterm != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_cterm);
	ਜੇ (attr_dict != ਨਿਰਾ)
	{
	    ਜੇ (dict_ਜੋੜੋ_dict(dict, "cterm", attr_dict) == FAIL)
		ਜਾਓ error;
	    attr_dict = ਨਿਰਾ;
	}
    }
    ਜੇ (sgp->sg_cterm_fg != 0)
	ਜੇ (dict_ਜੋੜੋ_string(dict, "ctermfg",
		    highlight_color(hlgid, (char_u *)"fg", 'c')) == FAIL)
	    ਜਾਓ error;
    ਜੇ (sgp->sg_cterm_bg != 0)
	ਜੇ (dict_ਜੋੜੋ_string(dict, "ctermbg",
			highlight_color(hlgid, (char_u *)"bg", 'c')) == FAIL)
	    ਜਾਓ error;
    ਜੇ (sgp->sg_cterm_ul != 0)
	ਜੇ (dict_ਜੋੜੋ_string(dict, "ctermul",
			highlight_color(hlgid, (char_u *)"ul", 'c')) == FAIL)
	    ਜਾਓ error;
    ਜੇ (sgp->sg_cterm_font != 0)
	ਜੇ (dict_ਜੋੜੋ_string(dict, "ctermfont",
			highlight_color(hlgid, (char_u *)"font", 'c')) == FAIL)
	    ਜਾਓ error;
    ਜੇ (sgp->sg_gui != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_gui);
	ਜੇ (attr_dict != ਨਿਰਾ)
	{
	    ਜੇ (dict_ਜੋੜੋ_dict(dict, "gui", attr_dict) == FAIL)
		ਜਾਓ error;
	    attr_dict = ਨਿਰਾ;
	}
    }
    ਜੇ (sgp->sg_gui_fg_name != ਨਿਰਾ)
	ਜੇ (dict_ਜੋੜੋ_string(dict, "guifg",
			highlight_color(hlgid, (char_u *)"fg", 'g')) == FAIL)
	    ਜਾਓ error;
    ਜੇ (sgp->sg_gui_bg_name != ਨਿਰਾ)
	ਜੇ (dict_ਜੋੜੋ_string(dict, "guibg",
			highlight_color(hlgid, (char_u *)"bg", 'g')) == FAIL)
	    ਜਾਓ error;
    ਜੇ (sgp->sg_gui_sp_name != ਨਿਰਾ)
	ਜੇ (dict_ਜੋੜੋ_string(dict, "guisp",
			highlight_color(hlgid, (char_u *)"sp", 'g')) == FAIL)
	    ਜਾਓ error;
# ifdef FEAT_GUI
    ਜੇ (sgp->sg_font_name != ਨਿਰਾ)
	ਜੇ (dict_ਜੋੜੋ_string(dict, "font", sgp->sg_font_name) == FAIL)
	    ਜਾਓ error;
# endif
    ਜੇ (sgp->sg_link)
    {
	char_u	*link;

	link = HL_ਸਾਰਣੀ()[sgp->sg_link - 1].sg_name;
	ਜੇ (link != ਨਿਰਾ && dict_ਜੋੜੋ_string(dict, "linksto", link) == FAIL)
	    ਜਾਓ error;

	ਜੇ (sgp->sg_deflink)
	    dict_ਜੋੜੋ_bool(dict, "default", VVAL_TRUE);
    }
    ਜੇ (dict_len(dict) == 2)
	// If only 'name' is present, then the highlight group is cleared.
	dict_ਜੋੜੋ_bool(dict, "cleared", VVAL_TRUE);

    ਵਾਪਸ dict;

error:
    dict_unref(attr_dict);
    dict_unref(dict);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * "hlget([name])" function
 * Return the attributes of a specific highlight group (if specified) or all
 * the highlight groups.
 */
    ਖਾਲੀ
f_hlget(typval_T *argvars, typval_T *rettv)
{
    list_T	*list;
    dict_T	*dict;
    ਪੂਰਨ		i;
    char_u	*hlarg = ਨਿਰਾ;
    ਪੂਰਨ		resolve_link = FALSE;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (check_for_opt_string_arg(argvars, 0) == FAIL
	    || (argvars[0].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// highlight group name supplied
	hlarg = tv_get_string_chk(&argvars[0]);
	ਜੇ (hlarg == ਨਿਰਾ)
	    ਵਾਪਸ;

	ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    ਪੂਰਨ error = FALSE;

	    resolve_link = tv_get_bool_chk(&argvars[1], &error);
	    ਜੇ (error)
		ਵਾਪਸ;
	}
    }

    list = rettv->vval.v_list;
    ਲਈ (i = 0; i < highlight_ga.ga_len && !got_int; ++i)
    {
	ਜੇ (hlarg == ਨਿਰਾ || STRICMP(hlarg, HL_ਸਾਰਣੀ()[i].sg_name) == 0)
	{
	    dict = highlight_get_info(i, resolve_link);
	    ਜੇ (dict != ਨਿਰਾ && list_append_dict(list, dict) == FAIL)
		dict_unref(dict);
	}
    }
}

/*
 * Returns the string value at 'dict[key]'. Returns NULL, if 'key' is not in
 * 'dict' or the value is not a string type. If the value is not a string type
 * or is NULL, then 'error' is set to TRUE.
 */
    ਸਥਿਰ char_u *
hldict_get_string(dict_T *dict, char_u *ਕੁੰਜੀ, ਪੂਰਨ *error)
{
    dictitem_T	*di;

    *error = FALSE;
    di = dict_ਲੱਭੋ(dict, ਕੁੰਜੀ, -1);
    ਜੇ (di == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (di->di_tv.v_type != VAR_STRING || di->di_tv.vval.v_string == ਨਿਰਾ)
    {
	emsg(_(e_string_required));
	*error = TRUE;
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ di->di_tv.vval.v_string;
}

/*
 * Convert the highlight attribute Dictionary at 'dict[key]' into a string
 * value in 'attr_str' of length 'len'. Returns FALSE if 'dict[key]' is not a
 * Dictionary or is NULL.
 */
    ਸਥਿਰ ਪੂਰਨ
hldict_attr_ਮੰਜ਼ਿਲ_str(
	dict_T	*dict,
	char_u	*ਕੁੰਜੀ,
	char_u	*attr_str,
	size_t	len)
{
    dictitem_T	*di;
    dict_T	*attrdict;
    ਪੂਰਨ		i;
    char_u	*p;

    attr_str[0] = NUL;
    di = dict_ਲੱਭੋ(dict, ਕੁੰਜੀ, -1);
    ਜੇ (di == ਨਿਰਾ)
	ਵਾਪਸ TRUE;

    ਜੇ (di->di_tv.v_type != VAR_DICT || di->di_tv.vval.v_dict == ਨਿਰਾ)
    {
	emsg(_(e_dictionary_required));
	ਵਾਪਸ FALSE;
    }

    attrdict = di->di_tv.vval.v_dict;

    // If the attribute dict is empty, then return NONE to clear the attributes
    ਜੇ (dict_len(attrdict) == 0)
    {
	vim_strcat(attr_str, (char_u *)"NONE", len);
	ਵਾਪਸ TRUE;
    }

    p = attr_str;
    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(highlight_tab); ++i)
    {
	ਜੇ (dict_get_bool(attrdict, (ਅੱਖਰ *)highlight_tab[i].ਮੁੱਲ.string,
		    VVAL_FALSE) == VVAL_TRUE)
	{
	    ਜੇ (p != attr_str && (size_t)(p - attr_str + 2) < len)
		STRCPY(p, (char_u *)",");
	    ਜੇ (p - attr_str + highlight_tab[i].ਮੁੱਲ.ਲੰਮਾਈ + 1 < len)
	    {
		STRCPY(p, highlight_tab[i].ਮੁੱਲ.string);
		p += highlight_tab[i].ਮੁੱਲ.ਲੰਮਾਈ;
	    }
	}
    }

    ਵਾਪਸ TRUE;
}

// Temporary buffer used to store the command string produced by hlset().
// IObuff cannot be used for this as the error messages produced by hlset()
// internally use IObuff.
# define	HLSETBUFSZ  512
ਸਥਿਰ char_u hlsetBuf[HLSETBUFSZ + 1];

/*
 * Add the highlight attribute "attr" of length "attrlen" and "value" at
 * "dptr", which points into "hlsetBuf".
 * Returns the updated pointer.
 */
    ਸਥਿਰ char_u *
ਜੋੜੋ_attr_and_ਮੁੱਲ(char_u *dptr, char_u *attr, ਪੂਰਨ attrlen, char_u *ਮੁੱਲ)
{
    size_t	vallen;

    // Do nothing if the value is not specified or is empty
    ਜੇ (ਮੁੱਲ == ਨਿਰਾ || *ਮੁੱਲ == NUL)
	ਵਾਪਸ dptr;

    vallen = STRLEN(ਮੁੱਲ);
    // When the value contains a space and the attribute has an "=" (i.e. it
    // is a key=value pair), surround the value with single quotes so that
    // do_highlight() can parse it correctly.
    ਜੇ (vim_strchr(ਮੁੱਲ, ' ') != ਨਿਰਾ && vim_strchr(attr, '=') != ਨਿਰਾ)
    {
	ਜੇ (dptr + attrlen + vallen + 3 < hlsetBuf + HLSETBUFSZ)
	{
	    STRCPY(dptr, attr);
	    dptr += attrlen;
	    *dptr++ = '\'';
	    STRCPY(dptr, ਮੁੱਲ);
	    dptr += vallen;
	    *dptr++ = '\'';
	    *dptr = NUL;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (dptr + attrlen + vallen + 1 < hlsetBuf + HLSETBUFSZ)
    {
	STRCPY(dptr, attr);
	dptr += attrlen;
	STRCPY(dptr, ਮੁੱਲ);
	dptr += vallen;
    }

    ਵਾਪਸ dptr;
}

/*
 * Add or update a highlight group using 'dict' items. Returns TRUE if
 * successfully updated the highlight group.
 */
    ਸਥਿਰ ਪੂਰਨ
hlg_ਜੋੜੋ_or_update(dict_T *dict)
{
    char_u	*name;
    ਪੂਰਨ		error;
    char_u	ਪਦ_attr[MAX_ATTR_LEN];
    char_u	cterm_attr[MAX_ATTR_LEN];
    char_u	gui_attr[MAX_ATTR_LEN];
    char_u	*ਸ਼ੁਰੂ;
    char_u	*stop;
    char_u	*ctermfg;
    char_u	*ctermbg;
    char_u	*ctermul;
    char_u	*ctermfont;
    char_u	*guifg;
    char_u	*guibg;
    char_u	*guisp;
# ifdef FEAT_GUI
    char_u	*font;
# endif
    ਪੂਰਨ		forceit = FALSE;
    ਪੂਰਨ		dodefault = FALSE;
    ਪੂਰਨ		ਪੂਰਾ = FALSE;
    char_u	*p;

    name = hldict_get_string(dict, (char_u *)"name", &error);
    ਜੇ (name == ਨਿਰਾ || *name == NUL || error)
	ਵਾਪਸ FALSE;

    ਜੇ (dict_get_bool(dict, "force", VVAL_FALSE) == VVAL_TRUE)
	forceit = TRUE;

    ਜੇ (dict_get_bool(dict, "default", VVAL_FALSE) == VVAL_TRUE)
	dodefault = TRUE;

    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "cleared"))
    {
	varnumber_T	cleared;

	// clear a highlight group
	cleared = dict_get_bool(dict, "cleared", FALSE);
	ਜੇ (cleared == TRUE)
	{
	    vim_snprintf((ਅੱਖਰ *)hlsetBuf, HLSETBUFSZ, "clear %s", name);
	    do_highlight(hlsetBuf, forceit, FALSE);
	    ਪੂਰਾ = TRUE;
	}
    }

    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "linksto"))
    {
	char_u	*linksto;

	// link highlight groups
	linksto = hldict_get_string(dict, (char_u *)"linksto", &error);
	ਜੇ (linksto == ਨਿਰਾ || *linksto == NUL || error)
	    ਵਾਪਸ FALSE;

	vim_snprintf((ਅੱਖਰ *)hlsetBuf, HLSETBUFSZ, "%slink %s %s",
				dodefault ? "default " : "", name, linksto);
	do_highlight(hlsetBuf, forceit, FALSE);

	ਪੂਰਾ = TRUE;
    }

    // If 'cleared' or 'linksto' are specified, then don't process the other
    // attributes.
    ਜੇ (ਪੂਰਾ)
	ਵਾਪਸ TRUE;

    ਸ਼ੁਰੂ = hldict_get_string(dict, (char_u *)"start", &error);
    ਜੇ (error)
	ਵਾਪਸ FALSE;

    stop = hldict_get_string(dict, (char_u *)"stop", &error);
    ਜੇ (error)
	ਵਾਪਸ FALSE;

    ਜੇ (!hldict_attr_ਮੰਜ਼ਿਲ_str(dict, (char_u *)"term", ਪਦ_attr,
							    ਆਕਾਰ(ਪਦ_attr)))
	ਵਾਪਸ FALSE;

    ਜੇ (!hldict_attr_ਮੰਜ਼ਿਲ_str(dict, (char_u *)"cterm", cterm_attr,
							   ਆਕਾਰ(cterm_attr)))
	ਵਾਪਸ FALSE;

    ctermfg = hldict_get_string(dict, (char_u *)"ctermfg", &error);
    ਜੇ (error)
	ਵਾਪਸ FALSE;

    ctermbg = hldict_get_string(dict, (char_u *)"ctermbg", &error);
    ਜੇ (error)
	ਵਾਪਸ FALSE;

    ctermul = hldict_get_string(dict, (char_u *)"ctermul", &error);
    ਜੇ (error)
	ਵਾਪਸ FALSE;

    ctermfont = hldict_get_string(dict, (char_u *)"ctermfont", &error);
    ਜੇ (error)
	ਵਾਪਸ FALSE;

    ਜੇ (!hldict_attr_ਮੰਜ਼ਿਲ_str(dict, (char_u *)"gui", gui_attr, ਆਕਾਰ(gui_attr)))
	ਵਾਪਸ FALSE;

    guifg = hldict_get_string(dict, (char_u *)"guifg", &error);
    ਜੇ (error)
	ਵਾਪਸ FALSE;

    guibg = hldict_get_string(dict, (char_u *)"guibg", &error);
    ਜੇ (error)
	ਵਾਪਸ FALSE;

    guisp = hldict_get_string(dict, (char_u *)"guisp", &error);
    ਜੇ (error)
	ਵਾਪਸ FALSE;

# ifdef FEAT_GUI
    font = hldict_get_string(dict, (char_u *)"font", &error);
    ਜੇ (error)
	ਵਾਪਸ FALSE;
# endif

    // If none of the attributes are specified, then do nothing.
    ਜੇ (ਪਦ_attr[0] == NUL && ਸ਼ੁਰੂ == ਨਿਰਾ && stop == ਨਿਰਾ
	    && cterm_attr[0] == NUL && ctermfg == ਨਿਰਾ && ctermbg == ਨਿਰਾ
	    && ctermul == ਨਿਰਾ && ctermfont == ਨਿਰਾ && gui_attr[0] == NUL
# ifdef FEAT_GUI
	    && font == ਨਿਰਾ
# endif
	    && guifg == ਨਿਰਾ && guibg == ਨਿਰਾ && guisp == ਨਿਰਾ
	    )
	ਵਾਪਸ TRUE;

    hlsetBuf[0] = NUL;
    p = hlsetBuf;
    ਜੇ (dodefault)
	p = ਜੋੜੋ_attr_and_ਮੁੱਲ(p, (char_u *)"default", 7, (char_u *)" ");
    p = ਜੋੜੋ_attr_and_ਮੁੱਲ(p, (char_u *)"", 0, name);
    p = ਜੋੜੋ_attr_and_ਮੁੱਲ(p, (char_u *)" term=", 6, ਪਦ_attr);
    p = ਜੋੜੋ_attr_and_ਮੁੱਲ(p, (char_u *)" start=", 7, ਸ਼ੁਰੂ);
    p = ਜੋੜੋ_attr_and_ਮੁੱਲ(p, (char_u *)" stop=", 6, stop);
    p = ਜੋੜੋ_attr_and_ਮੁੱਲ(p, (char_u *)" cterm=", 7, cterm_attr);
    p = ਜੋੜੋ_attr_and_ਮੁੱਲ(p, (char_u *)" ctermfg=", 9, ctermfg);
    p = ਜੋੜੋ_attr_and_ਮੁੱਲ(p, (char_u *)" ctermbg=", 9, ctermbg);
    p = ਜੋੜੋ_attr_and_ਮੁੱਲ(p, (char_u *)" ctermul=", 9, ctermul);
    p = ਜੋੜੋ_attr_and_ਮੁੱਲ(p, (char_u *)" ctermfont=", 11, ctermfont);
    p = ਜੋੜੋ_attr_and_ਮੁੱਲ(p, (char_u *)" gui=", 5, gui_attr);
# ifdef FEAT_GUI
    p = ਜੋੜੋ_attr_and_ਮੁੱਲ(p, (char_u *)" font=", 6, font);
# endif
    p = ਜੋੜੋ_attr_and_ਮੁੱਲ(p, (char_u *)" guifg=", 7, guifg);
    p = ਜੋੜੋ_attr_and_ਮੁੱਲ(p, (char_u *)" guibg=", 7, guibg);
    (ਖਾਲੀ)ਜੋੜੋ_attr_and_ਮੁੱਲ(p, (char_u *)" guisp=", 7, guisp);

    do_highlight(hlsetBuf, forceit, FALSE);

    ਵਾਪਸ TRUE;
}

/*
 * "hlset([{highlight_attr}])" function
 * Add or modify highlight groups
 */
    ਖਾਲੀ
f_hlset(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    dict_T	*dict;

    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (check_for_list_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(argvars->vval.v_list, li)
    {
	ਜੇ (li->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_dictionary_required));
	    ਵਾਪਸ;
	}

	dict = li->li_tv.vval.v_dict;
	ਜੇ (!hlg_ਜੋੜੋ_or_update(dict))
	    ਵਾਪਸ;
    }

    rettv->vval.v_ਸੰਖਿਆ = 0;
}
#ਜੇ_ਅੰਤ

/*
 * If "old" is in the override stack, then update it to "hl_new". Does not free
 * "old".
 */
    ਖਾਲੀ
update_highlight_overrides(hl_override_T *old, hl_override_T *hl_new, ਪੂਰਨ newlen)
{
    ਜੇ (old == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (hl_overrides_T *ਸੈੱਟ_2 = overrides; ਸੈੱਟ_2 != ਨਿਰਾ; ਸੈੱਟ_2 = ਸੈੱਟ_2->ਅਗਲਾ)
    {
	ਜੇ (ਸੈੱਟ_2->arr == old)
	{
	    ਸੈੱਟ_2->arr = hl_new;
	    ਸੈੱਟ_2->len = newlen;
	}
    }
}

/*
 * If "arr" is in the highlight overrides list, then mark it as invalid.
 */
    ਖਾਲੀ
remove_highlight_overrides(hl_override_T *arr)
{
    ਜੇ (arr == ਨਿਰਾ || overrides == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (hl_overrides_T *ਸੈੱਟ_2 = overrides; ਸੈੱਟ_2 != ਨਿਰਾ; ਸੈੱਟ_2 = ਸੈੱਟ_2->ਅਗਲਾ)
    {
	ਜੇ (ਸੈੱਟ_2->arr == arr)
	{
	    ਸੈੱਟ_2->arr = ਨਿਰਾ;
	    ਰੋਕੋ;
	}
    }
}

/*
 * Update highlight_attr[] array. If "update_ids" is true, then update
 * highlight_ids[] array instead.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_highlight_attr(hl_override_T *arr, ਪੂਰਨ len, bool update_ids)
{
    ਲਈ (ਪੂਰਨ i = 0; i < len; i++)
    {
	hl_override_T	*override = arr + i;
	ਪੂਰਨ		hlf = -1, attr;

	ਜੇ (override->from <= 0)
	    // Directly map to highlight_attr
	    hlf = -override->from;
	ਨਹੀਂਤਾਂ
	{
	    ਲਈ (ਪੂਰਨ k = 0; k < HLF_ਗਿਣਤੀ; k++)
		ਜੇ (override->from == highlight_ids[k])
		{
		    hlf = k;
		    ਰੋਕੋ;
		}

	    ਜੇ (hlf == -1)
		ਜਾਰੀ;
	}

	ਜੇ (update_ids)
	{
	    ਜੇ (override->ਮੰਜ਼ਿਲ <= 0)
	    {
		ਜੇ (hlf == -override->ਮੰਜ਼ਿਲ)
		    highlight_ids[hlf] = -1;
		ਨਹੀਂਤਾਂ
		    highlight_ids[hlf] = highlight_ids[-override->ਮੰਜ਼ਿਲ];
	    }
	    ਨਹੀਂਤਾਂ
		highlight_ids[hlf] = override->ਮੰਜ਼ਿਲ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (override->ਮੰਜ਼ਿਲ <= 0)
		// Directly use highlight_attr[]
		attr = highlight_attr[-override->ਮੰਜ਼ਿਲ];
	    ਨਹੀਂਤਾਂ
		attr = syn_id2attr(override->ਮੰਜ਼ਿਲ);

	    highlight_attr[hlf] = attr;
	}
    }
}

/*
 * Set the current highlight override to "arr". Returns true if successful
 */
    bool
ਧੱਕੋ_highlight_overrides(hl_override_T *arr, ਪੂਰਨ len)
{
    // Don't want to do anything if "arr" is already the current override. If
    // "arr" is NULL (but overrides->arr is not), then still push an override,
    // but "->arr" will just be NULL so any previous overrides are cleared.
    ਜੇ (overrides != ਨਿਰਾ && overrides->arr == arr)
	ਵਾਪਸ false;

    hl_overrides_T *ਸੈੱਟ_2;

    ਸੈੱਟ_2 = ALLOC_ONE(hl_overrides_T);
    ਜੇ (ਸੈੱਟ_2 == ਨਿਰਾ)
	ਵਾਪਸ false;

    ਸੈੱਟ_2->arr = arr;
    ਸੈੱਟ_2->len = len;

    // Push to front of list
    ਸੈੱਟ_2->ਅਗਲਾ = overrides;
    overrides = ਸੈੱਟ_2;

    // Save current highlight_attr[], so it can be restored later after this
    // override is popped.
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਸੈੱਟ_2->attr, highlight_attr, ਆਕਾਰ(highlight_attr));

    // Reset highlight_attr[] to default values, as we don't want changes from
    // other overrides.
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(highlight_attr, highlight_attr_raw, ਆਕਾਰ(highlight_attr));

    // Update highlight_attr[] array
    ਜੇ (arr != ਨਿਰਾ)
	ਸੈੱਟ_highlight_attr(arr, len, false);

    ਵਾਪਸ true;
}

/*
 * Pop the current highlight override (if any) from the stack. This should
 * always be paired with a *successful* push.
 */
    ਖਾਲੀ
ਕੱਢੋ_highlight_overrides(ਖਾਲੀ)
{
    hl_overrides_T *ਸੈੱਟ_2 = overrides;

    ਜੇ (overrides == ਨਿਰਾ)
	ਵਾਪਸ;

    overrides = ਸੈੱਟ_2->ਅਗਲਾ;

    // Set highlight_attr[] to state before override was pushed.
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(highlight_attr, ਸੈੱਟ_2->attr, ਆਕਾਰ(highlight_attr));
    vim_free(ਸੈੱਟ_2);
}

/*
 * Parse the 'winhighlight' option and return array. Returns NULL on failure or
 * if empty option. If failure, then errmsg is set.
 */
    ਸਥਿਰ hl_override_T *
parse_winhighlight(char_u *opt, ਪੂਰਨ *len, ਅੱਖਰ **errmsg)
{
    char_u	    *p = opt;
    hl_override_T   *arr;
    ਪੂਰਨ		    i = 0;
    ਪੂਰਨ		    ਅੰਕ = 1;
    ਪੂਰਨ		    n_colons = 0;

    ਜੇ (*p == NUL)
	ਵਾਪਸ ਨਿਰਾ;

    // Get number of overrides first so we can allocate array
    ਜਦੋਂ ((p = vim_strchr(p, ',')) != ਨਿਰਾ)
    {
	p++;
	ਅੰਕ++;
    }
    p = opt;
    // Check if number of ':' matches number of ','
    ਜਦੋਂ ((p = vim_strchr(p, ':')) != ਨਿਰਾ)
    {
	p++;
	n_colons++;
    }

    ਜੇ (ਅੰਕ != n_colons)
    {
	*errmsg = e_invalid_ਦਲੀਲ;
	ਵਾਪਸ ਨਿਰਾ;
    }

    arr = ALLOC_MULT(hl_override_T, ਅੰਕ);
    ਜੇ (arr == ਨਿਰਾ)
    {
	*errmsg = e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
	ਵਾਪਸ ਨਿਰਾ;
    }

    p = opt;

    ਜਦੋਂ (true)
    {
	hl_override_T *override = arr + i++;
	char_u	*fromname = p, *toname;
	char_u	*tmp;
	char_u	**ਨਾਂ[2] = {&fromname, &toname};
	ਪੂਰਨ	fromlen, tolen;
	ਪੂਰਨ	*lens[2] = {&fromlen, &tolen};
	ਪੂਰਨ	fromid, toid;
	ਪੂਰਨ	*ids[2] = {&fromid, &toid};

	p = vim_strchr(p, ':');

	ਜੇ (p == ਨਿਰਾ)
	    ਜਾਓ fail;

	fromlen = p - fromname; // Get hl for "from"
	ਜੇ (fromlen == 0)
	    ਜਾਓ fail;
	p++; // Skip colon ':'
	ਜੇ (*p == NUL)
	    ਜਾਓ fail;

	toname = p;
	tmp = vim_strchr(p, ',');

	// Get hl for "to", must check for no trailing comma in case last
	// element.
	ਜੇ (tmp == ਨਿਰਾ)
	    tolen = (ਪੂਰਨ)STRLEN(p);
	ਨਹੀਂਤਾਂ
	{
	    tolen = tmp - toname;
	    p = ++tmp;
	}
	ਜੇ (tolen == 0)
	    ਜਾਓ fail;

	ਲਈ (ਪੂਰਨ k = 0; k < 2; k++)
	{
	    char_u  *name = *ਨਾਂ[k];
	    ਪੂਰਨ	    nlen = *lens[k];

	    ਜੇ (*name == '!')
	    {
		// If starts with "!", then it maps directly to a 'highlight'
		// occasion.
		ਪੂਰਨ hlf;

		ਜੇ (nlen != 2)
		    ਜਾਓ fail;

		ਲਈ (hlf = 0; hlf < (ਪੂਰਨ)HLF_ਗਿਣਤੀ; ++hlf)
		    ਜੇ (hl_flags[hlf] == name[1])
			ਰੋਕੋ;

		ਜੇ (hlf >= HLF_ਗਿਣਤੀ)
		    ਜਾਓ fail;

		*ids[k] = -hlf;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Otherwise get the highlight group id.
		ਜੇ (syn_check_ਸਮੂਹ(name, nlen) == 0)
		    ਜਾਓ fail;

		*ids[k] = syn_namen2id(name, nlen);
		ਜੇ (*ids[k] == 0)
		    ਜਾਓ fail;

		// Always map "Normal" group to HLF_WIN
		ਜੇ (ids[k] == &fromid &&
			STRCMP(HL_ਸਾਰਣੀ()[*ids[k] - 1].sg_name_u, "NORMAL") == 0)
		    *ids[k] = -HLF_WIN;
	    }
	}

	// If fromid == toid, leave it be, this is so that we know HLF_WIN has
	// been set (even to itself e.g. "Normal"), and therefore know to use
	// its attr instead.
	override->from = fromid;
	override->ਮੰਜ਼ਿਲ = toid;

	ਜੇ (tmp == ਨਿਰਾ)
	    ਰੋਕੋ;
    }

    *len = ਅੰਕ;
    ਵਾਪਸ arr;
fail:
    vim_free(arr);
    *errmsg = e_invalid_ਦਲੀਲ;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Update w_hl for "wp" using given option value. Returns error
 * message on failure, otherwise NULL.
 */
    ਅੱਖਰ *
update_winhighlight(win_T *wp, char_u *opt)
{
    ਅੱਖਰ	    *err = ਨਿਰਾ;
    hl_override_T   *arr;
    ਪੂਰਨ		    ਅੰਕ = 1;

    arr = parse_winhighlight(opt, &ਅੰਕ, &err);

    ਜੇ (arr == ਨਿਰਾ && err != ਨਿਰਾ)
	ਵਾਪਸ err;

    ਜੇ (arr == ਨਿਰਾ && wp->w_hl == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    update_highlight_overrides(wp->w_hl, arr, ਅੰਕ);

    vim_free(wp->w_hl);
    wp->w_hl = arr;
    wp->w_hl_len = ਅੰਕ;

    wp->w_hlfwin_id = hlf_get_id(wp, HLF_WIN);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਪਦ_update_hlfwin(wp);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    // May be NULL (such as in after_copy_winopt())
    ਜੇ (wp->w_ਬਫਰ != ਨਿਰਾ)
    {
	// Make sure terminal highlighting is updated
	bool ret = ਧੱਕੋ_highlight_overrides(wp->w_hl, wp->w_hl_len);

	ਜੇ (wp->w_ਬਫਰ->b_ਪਦ != ਨਿਰਾ)
	    ਪਦ_init_default_colors(wp->w_ਬਫਰ->b_ਪਦ);
	ਜੇ (ret)
	    ਕੱਢੋ_highlight_overrides();
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Get id that the hlf attr is using in window "wp", otherwise zero. Note that
 * -1 may be returned if the hlf was set to itself.
 */
    ਪੂਰਨ
hlf_get_id(win_T *wp, ਪੂਰਨ hlf)
{
    ਪੂਰਨ id;
    ਸਥਿਰ ਪੂਰਨ prev[HLF_ਗਿਣਤੀ];

    ਜੇ (wp->w_hl == ਨਿਰਾ)
	ਵਾਪਸ highlight_ids[hlf];

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(prev, highlight_ids, ਆਕਾਰ(highlight_ids));
    ਸੈੱਟ_highlight_attr(wp->w_hl, wp->w_hl_len, true);
    id = highlight_ids[hlf];
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(highlight_ids, prev, ਆਕਾਰ(highlight_ids));

    ਵਾਪਸ id;
}

/*
 * Update 'winhighlight' using a 'wincolor' style option value. Returns error
 * message on failure otherwise NULL. Does not update the actual 'wincolor'
 * value, but does update the 'winhighlight' value.
 */
    ਅੱਖਰ *
update_wincolor(win_T *wp, char_u *opt)
{
    char_u  *str = *opt == NUL ? "" : alloc(ਆਕਾਰ("!(:") + STRLEN(opt));
    ਅੱਖਰ    *errmsg;

    ਜੇ (str == ਨਿਰਾ)
	ਵਾਪਸ e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    ਜੇ (*opt != NUL)
	sprintf((ਅੱਖਰ *)str, "!(:%s", opt);

    errmsg = update_winhighlight(wp, str);

    ਜੇ (errmsg == ਨਿਰਾ)
	ਸੈੱਟ_string_option_ਸਿੱਧਾ_ਵਿੱਚ_win(wp, (char_u *)"winhighlight", -1,
		str, OPT_FREE|OPT_LOCAL, 0);

    ਜੇ (*opt != NUL)
	ਮੁਕਤ(str);
    ਵਾਪਸ errmsg;
}
