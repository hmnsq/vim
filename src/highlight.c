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

#ಅನ್ತರ್ಭಾವ "vim.h"

#define SG_TERM		1	// term has been set
#define SG_CTERM	2	// cterm has been set
#define SG_GUI		4	// gui has been set
#define SG_LINK		8	// link has been set

#define MAX_SYN_NAME	200

/*
 * The "term", "cterm" and "gui" arguments can be any combination of the
 * following names, separated by commas (but no spaces!).
 */
// must be sorted by the 'value' field because it is used by bsearch()!
// note: inverse and reverse use the same key
ಸ್ಥಿರ keyvalue_T highlight_tab[] = {
    KEYVALUE_ENTRY(HL_BOLD, "bold"),			    // index 0
    KEYVALUE_ENTRY(HL_INVERSE, "inverse"),		    // index 1
    KEYVALUE_ENTRY(HL_ITALIC, "italic"),		    // index 2
    KEYVALUE_ENTRY(HL_NOCOMBINE, "nocombine"),		    // index 3
    KEYVALUE_ENTRY(HL_NORMAL, "NONE"),			    // index 4
    KEYVALUE_ENTRY(HL_INVERSE, "reverse"),		    // index 5
    KEYVALUE_ENTRY(HL_STANDOUT, "standout"),		    // index 6
    KEYVALUE_ENTRY(HL_STRIKETHROUGH, "strikethrough"),	    // index 7
    KEYVALUE_ENTRY(HL_UNDERCURL, "undercurl"),		    // index 8
    KEYVALUE_ENTRY(HL_UNDERDASHED, "underdashed"),	    // index 9
    KEYVALUE_ENTRY(HL_UNDERDOTTED, "underdotted"),	    // index 10
    KEYVALUE_ENTRY(HL_UNDERDOUBLE, "underdouble"),	    // index 11
    KEYVALUE_ENTRY(HL_UNDERLINE, "underline")		    // index 12
};

// this table is used to display highlight names in the "correct" sequence.
// keep this in sync with highlight_tab[].
ಸ್ಥಿರ keyvalue_T *highlight_index_tab[] = {
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
#define MAX_ATTR_LEN 120

#define ATTR_COMBINE(attr_a, attr_b) ((((attr_b) & HL_NOCOMBINE) ? (attr_b) : (attr_a)) | (attr_b))

ಗಣನಾ {
    BLACK = 0,
    DARKBLUE,
    DARKGREEN,
    DARKCYAN,
    DARKRED,
    DARKMAGENTA,
    BROWN,
    DARKYELLOW,
    GRAY,
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
ಸ್ಥಿರ keyvalue_T color_name_tab[] = {
    KEYVALUE_ENTRY(BLACK, "Black"),
    KEYVALUE_ENTRY(BLUE, "Blue"),
    KEYVALUE_ENTRY(BROWN, "Brown"),
    KEYVALUE_ENTRY(CYAN, "Cyan"),
    KEYVALUE_ENTRY(DARKBLUE, "DarkBlue"),
    KEYVALUE_ENTRY(DARKCYAN, "DarkCyan"),
    KEYVALUE_ENTRY(DARKGRAY, "DarkGray"),
    KEYVALUE_ENTRY(DARKGREEN, "DarkGreen"),
    KEYVALUE_ENTRY(DARKGREY, "DarkGrey"),
    KEYVALUE_ENTRY(DARKMAGENTA, "DarkMagenta"),
    KEYVALUE_ENTRY(DARKRED, "DarkRed"),
    KEYVALUE_ENTRY(DARKYELLOW, "DarkYellow"),
    KEYVALUE_ENTRY(GRAY, "Gray"),
    KEYVALUE_ENTRY(GREEN, "Green"),
    KEYVALUE_ENTRY(GREY, "Grey"),
    KEYVALUE_ENTRY(LIGHTBLUE, "LightBlue"),
    KEYVALUE_ENTRY(LIGHTCYAN, "LightCyan"),
    KEYVALUE_ENTRY(LIGHTGRAY, "LightGray"),
    KEYVALUE_ENTRY(LIGHTGREEN, "LightGreen"),
    KEYVALUE_ENTRY(LIGHTGREY, "LightGrey"),
    KEYVALUE_ENTRY(LIGHTMAGENTA, "LightMagenta"),
    KEYVALUE_ENTRY(LIGHTRED, "LightRed"),
    KEYVALUE_ENTRY(LIGHTYELLOW, "LightYellow"),
    KEYVALUE_ENTRY(MAGENTA, "Magenta"),
    KEYVALUE_ENTRY(NONE, "NONE"),
    KEYVALUE_ENTRY(RED, "Red"),
    KEYVALUE_ENTRY(WHITE, "White"),
    KEYVALUE_ENTRY(YELLOW, "Yellow")
};

/*
 * Structure that stores information about a highlight group.
 * The ID of a highlight group is also called group ID.  It is the index in
 * the highlight_ga array PLUS ONE.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    char_u	*sg_name;	// highlight group name
    char_u	*sg_name_u;	// uppercase of sg_name
    ಪೂರ್ಣಾಙ್ಕ		sg_cleared;	// "hi clear" was used
// for normal terminals
    ಪೂರ್ಣಾಙ್ಕ		sg_term;	// "term=" highlighting attributes
    char_u	*sg_start;	// terminal string for start highl
    char_u	*sg_stop;	// terminal string for stop highl
    ಪೂರ್ಣಾಙ್ಕ		sg_term_attr;	// Screen attr for term mode
// for color terminals
    ಪೂರ್ಣಾಙ್ಕ		sg_cterm;	// "cterm=" highlighting attr
    ಪೂರ್ಣಾಙ್ಕ		sg_cterm_bold;	// bold attr was set for light color
    ಪೂರ್ಣಾಙ್ಕ		sg_cterm_fg;	// terminal fg color number + 1
    ಪೂರ್ಣಾಙ್ಕ		sg_cterm_bg;	// terminal bg color number + 1
    ಪೂರ್ಣಾಙ್ಕ		sg_cterm_ul;	// terminal ul color number + 1
    ಪೂರ್ಣಾಙ್ಕ		sg_cterm_attr;	// Screen attr for color term mode
    ಪೂರ್ಣಾಙ್ಕ		sg_cterm_font;	// terminal alternative font (0 for normal)
// for when using the GUI
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    guicolor_T	sg_gui_fg;	// GUI foreground color handle
    guicolor_T	sg_gui_bg;	// GUI background color handle
    guicolor_T	sg_gui_sp;	// GUI special color handle
#endif
#ifdef FEAT_GUI
    GuiFont	sg_font;	// GUI font handle
# ifdef FEAT_XFONTSET
    GuiFontset	sg_fontset;	// GUI fontset handle
# endif
    char_u	*sg_font_name;  // GUI font or fontset name
    ಪೂರ್ಣಾಙ್ಕ		sg_gui_attr;    // Screen attr for GUI mode
#endif
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_EVAL)
// Store the sp color name for the GUI or synIDattr()
    ಪೂರ್ಣಾಙ್ಕ		sg_gui;		// "gui=" highlighting attributes
    char_u	*sg_gui_fg_name;// GUI foreground color name
    char_u	*sg_gui_bg_name;// GUI background color name
    char_u	*sg_gui_sp_name;// GUI special color name
#endif
    ಪೂರ್ಣಾಙ್ಕ		sg_link;	// link to this highlight group ID
    ಪೂರ್ಣಾಙ್ಕ		sg_deflink;	// default link; restored in highlight_clear()
    ಪೂರ್ಣಾಙ್ಕ		sg_set;		// combination of SG_* flags
#ifdef FEAT_EVAL
    sctx_T	sg_deflink_sctx;  // script where the default link was set
    sctx_T	sg_script_ctx;	// script in which the group was last set
#endif
} hl_group_T;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ hl_overrides_S hl_overrides_T;
ಸಂರಚನಾ hl_overrides_S
{
    hl_override_T   *arr;   // May be NULL if "arr" was freed or no highlight
			    // overrides (all values set to default)
    ಪೂರ್ಣಾಙ್ಕ		    len;
    hl_overrides_T  *next;  // Used to handle recursive calls

    ಪೂರ್ಣಾಙ್ಕ attr[HLF_ಎಣಿಕೆ];    // highlight_attr[] before being updated.
};

ಸ್ಥಿರ hl_overrides_T *overrides = ರಿಕ್ತ;

// Synced with highlight_attr, each is the highlight group id for each attr. If
// an element is 0, then there is no attr for it. If an element is -1, then it
// means it was set to itself.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ highlight_ids[HLF_ಎಣಿಕೆ];

// Same as highlight_attr[] but is not modified by highlight group overrides
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ highlight_attr_raw[HLF_ಎಣಿಕೆ];

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ hl_flags[HLF_ಎಣಿಕೆ] = HL_FLAGS;

// highlight groups for 'highlight' option
ಸ್ಥಿರ garray_T highlight_ga;
#define HL_TABLE()	((hl_group_T *)((highlight_ga.ga_data)))

// Wrapper struct for hashtable entries: stores the highlight group ID alongside
// the uppercase name used as a hash key.  Uses the same offsetof pattern as
// signgroup_T / HI2SG.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ		hn_id;		// highlight group ID (1-based)
    char_u	hn_ಕೀಲಿ[1];	// uppercase name (stretchable array)
} hlname_T;

#define HLNAME_ಕೀಲಿ_OFF	offsetof(hlname_T, hn_ಕೀಲಿ)
#define HI2HLNAME(hi)	((hlname_T *)((hi)->hi_ಕೀಲಿ - HLNAME_ಕೀಲಿ_OFF))

// hashtable for quick highlight group name lookup
ಸ್ಥಿರ hashtab_T highlight_ht;
ಸ್ಥಿರ bool	 highlight_ht_inited = false;

/*
 * An attribute number is the index in attr_table plus ATTR_OFF.
 */
#define ATTR_OFF (HL_ALL + 1)

ಸ್ಥಿರ ಶೂನ್ಯ syn_unadd_group(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ set_hl_attr(ಪೂರ್ಣಾಙ್ಕ idx);
ಸ್ಥಿರ ಶೂನ್ಯ highlight_list_one(ಪೂರ್ಣಾಙ್ಕ id);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ highlight_list_arg(ಪೂರ್ಣಾಙ್ಕ id, ಪೂರ್ಣಾಙ್ಕ didh, ಪೂರ್ಣಾಙ್ಕ type, ಪೂರ್ಣಾಙ್ಕ iarg, char_u *sarg, ವರ್ಣ *name);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syn_add_group(char_u *name);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syn_name2id_len(char_u *name, ಪೂರ್ಣಾಙ್ಕ len);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ hl_has_settings(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ check_link);
ಸ್ಥಿರ ಶೂನ್ಯ highlight_clear(ಪೂರ್ಣಾಙ್ಕ idx);

#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
ಸ್ಥಿರ ಶೂನ್ಯ gui_do_one_color(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ do_menu, ಪೂರ್ಣಾಙ್ಕ do_tooltip);
#endif
#ifdef FEAT_GUI
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  set_group_colors(char_u *name, guicolor_T *fgp, guicolor_T *bgp, ಪೂರ್ಣಾಙ್ಕ do_menu, ಪೂರ್ಣಾಙ್ಕ use_norm, ಪೂರ್ಣಾಙ್ಕ do_tooltip);
ಸ್ಥಿರ ಶೂನ್ಯ hl_do_font(ಪೂರ್ಣಾಙ್ಕ idx, char_u *arg, ಪೂರ್ಣಾಙ್ಕ do_normal, ಪೂರ್ಣಾಙ್ಕ do_menu, ಪೂರ್ಣಾಙ್ಕ do_tooltip, ಪೂರ್ಣಾಙ್ಕ free_font);
#endif

/*
 * The default highlight groups.  These are compiled-in for fast startup and
 * they still work when the runtime files can't be found.
 * When making changes here, also change runtime/colors/default.vim!
 * The #ifdefs are needed to reduce the amount of static data.  Helps to make
 * the 16 bit DOS (museum) version compile.
 */
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_EVAL)
# define CENT(a, b) b
#ಅನ್ಯಥಾ
# define CENT(a, b) a
#endif
ಸ್ಥಿರ ವರ್ಣ *(highlight_init_both[]) = {
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
#ifdef FEAT_CLIPBOARD
    CENT("VisualNOS term=underline,bold cterm=underline,bold",
	 "VisualNOS term=underline,bold cterm=underline,bold gui=underline,bold"),
#endif
#ifdef FEAT_DIFF
    CENT("DiffText term=reverse cterm=bold ctermbg=Red ctermfg=Black",
	 "DiffText term=reverse cterm=bold ctermbg=Red ctermfg=Black gui=bold guibg=Red"),
    "default link DiffTextAdd DiffText",
#endif
    CENT("PmenuSbar term=NONE ctermbg=Grey",
	 "PmenuSbar term=NONE ctermbg=Grey guibg=Grey"),
    CENT("TabLineSel term=bold cterm=bold",
	 "TabLineSel term=bold cterm=bold gui=bold"),
    CENT("TabLineFill term=reverse cterm=reverse",
	 "TabLineFill term=reverse cterm=reverse gui=reverse"),
    "default link TabPanel TabLine",
    "default link TabPanelSel TabLineSel",
    "default link TabPanelFill TabLineFill",
#ifdef FEAT_GUI
    "Cursor guibg=fg guifg=bg",
    "lCursor guibg=fg guifg=bg", // should be different, but what?
#endif
#ifdef FEAT_GUI_MSWIN
    "TitleBar guibg=bg guifg=fg",
    "TitleBarNC guibg=NONE guifg=NONE",
#endif
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
    ರಿಕ್ತ
};

// Default colors only used with a light background.
ಸ್ಥಿರ ವರ್ಣ *(highlight_init_light[]) = {
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
#ifdef FEAT_SPELL
    CENT("SpellBad term=reverse ctermbg=LightRed",
	 "SpellBad term=reverse ctermbg=LightRed guisp=Red gui=undercurl"),
    CENT("SpellCap term=reverse ctermbg=LightBlue",
	 "SpellCap term=reverse ctermbg=LightBlue guisp=Blue gui=undercurl"),
    CENT("SpellRare term=reverse ctermbg=LightMagenta",
	 "SpellRare term=reverse ctermbg=LightMagenta guisp=Magenta gui=undercurl"),
    CENT("SpellLocal term=underline ctermbg=Cyan",
	 "SpellLocal term=underline ctermbg=Cyan guisp=DarkCyan gui=undercurl"),
#endif
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
#ifdef FEAT_FOLDING
    CENT("Folded term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "Folded term=standout ctermbg=Grey ctermfg=DarkBlue guibg=LightGrey guifg=DarkBlue"),
    CENT("FoldColumn term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "FoldColumn term=standout ctermbg=Grey ctermfg=DarkBlue guibg=Grey guifg=DarkBlue"),
#endif
#ifdef FEAT_SIGNS
    CENT("SignColumn term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "SignColumn term=standout ctermbg=Grey ctermfg=DarkBlue guibg=Grey guifg=DarkBlue"),
#endif
    CENT("Visual ctermbg=Grey ctermfg=Black",
	 "Visual ctermbg=Grey ctermfg=Black guibg=LightGrey guifg=Black"),
#ifdef FEAT_DIFF
    CENT("DiffAdd term=bold ctermbg=LightBlue ctermfg=Black",
	 "DiffAdd term=bold ctermbg=LightBlue ctermfg=Black guibg=LightBlue"),
    CENT("DiffChange term=bold ctermbg=LightMagenta ctermfg=Black",
	 "DiffChange term=bold ctermbg=LightMagenta ctermfg=Black guibg=LightMagenta"),
    CENT("DiffDelete term=bold ctermfg=Blue ctermbg=LightCyan",
	 "DiffDelete term=bold ctermfg=Blue ctermbg=LightCyan gui=bold guifg=Blue guibg=LightCyan"),
#endif
    CENT("TabLine term=underline cterm=underline ctermfg=black ctermbg=LightGrey",
	 "TabLine term=underline cterm=underline ctermfg=black ctermbg=LightGrey gui=underline guibg=LightGrey"),
#ifdef FEAT_SYN_HL
    CENT("CursorColumn term=reverse ctermbg=LightGrey",
	 "CursorColumn term=reverse ctermbg=LightGrey guibg=Grey90"),
    CENT("CursorLine term=underline cterm=underline",
	 "CursorLine term=underline cterm=underline guibg=Grey90"),
    CENT("ColorColumn term=reverse ctermbg=LightRed",
	 "ColorColumn term=reverse ctermbg=LightRed guibg=LightRed"),
#endif
#ifdef FEAT_CONCEAL
    CENT("Conceal ctermbg=DarkGrey ctermfg=LightGrey",
	 "Conceal ctermbg=DarkGrey ctermfg=LightGrey guibg=DarkGrey guifg=LightGrey"),
#endif
    CENT("MatchParen term=reverse ctermbg=Cyan",
	 "MatchParen term=reverse ctermbg=Cyan guibg=Cyan"),
#ifdef FEAT_TERMINAL
    CENT("StatusLineTerm term=reverse,bold cterm=bold ctermfg=White ctermbg=DarkGreen",
	 "StatusLineTerm term=reverse,bold cterm=bold ctermfg=White ctermbg=DarkGreen gui=bold guifg=bg guibg=DarkGreen"),
    CENT("StatusLineTermNC term=reverse ctermfg=White ctermbg=DarkGreen",
	 "StatusLineTermNC term=reverse ctermfg=White ctermbg=DarkGreen guifg=bg guibg=DarkGreen"),
#endif
#ifdef FEAT_MENU
    CENT("ToolbarLine term=underline ctermbg=LightGrey",
	 "ToolbarLine term=underline ctermbg=LightGrey guibg=LightGrey"),
    CENT("ToolbarButton cterm=bold ctermfg=White ctermbg=DarkGrey",
	 "ToolbarButton cterm=bold ctermfg=White ctermbg=DarkGrey gui=bold guifg=White guibg=Grey40"),
#endif
    ರಿಕ್ತ
};

// Default colors only used with a dark background.
ಸ್ಥಿರ ವರ್ಣ *(highlight_init_dark[]) = {
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
#ifdef FEAT_SPELL
    CENT("SpellBad term=reverse ctermbg=Red",
	 "SpellBad term=reverse ctermbg=Red guisp=Red gui=undercurl"),
    CENT("SpellCap term=reverse ctermbg=Blue",
	 "SpellCap term=reverse ctermbg=Blue guisp=Blue gui=undercurl"),
    CENT("SpellRare term=reverse ctermbg=Magenta",
	 "SpellRare term=reverse ctermbg=Magenta guisp=Magenta gui=undercurl"),
    CENT("SpellLocal term=underline ctermbg=Cyan",
	 "SpellLocal term=underline ctermbg=Cyan guisp=Cyan gui=undercurl"),
#endif
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
#ifdef FEAT_FOLDING
    CENT("Folded term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "Folded term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=DarkGrey guifg=Cyan"),
    CENT("FoldColumn term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "FoldColumn term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=Grey guifg=Cyan"),
#endif
#ifdef FEAT_SIGNS
    CENT("SignColumn term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "SignColumn term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=Grey guifg=Cyan"),
#endif
    CENT("Visual ctermbg=Grey ctermfg=Black",
	 "Visual ctermbg=Grey ctermfg=Black guibg=#575757 guifg=LightGrey"),
#ifdef FEAT_DIFF
    CENT("DiffAdd term=bold ctermbg=DarkBlue",
	 "DiffAdd term=bold ctermbg=DarkBlue guibg=DarkBlue"),
    CENT("DiffChange term=bold ctermbg=DarkMagenta",
	 "DiffChange term=bold ctermbg=DarkMagenta guibg=DarkMagenta"),
    CENT("DiffDelete term=bold ctermfg=Blue ctermbg=DarkCyan",
	 "DiffDelete term=bold ctermfg=Blue ctermbg=DarkCyan gui=bold guifg=Blue guibg=DarkCyan"),
#endif
    CENT("TabLine term=underline cterm=underline ctermfg=white ctermbg=DarkGrey",
	 "TabLine term=underline cterm=underline ctermfg=white ctermbg=DarkGrey gui=underline guibg=DarkGrey"),
#ifdef FEAT_SYN_HL
    CENT("CursorColumn term=reverse ctermbg=DarkGrey",
	 "CursorColumn term=reverse ctermbg=DarkGrey guibg=Grey40"),
    CENT("CursorLine term=underline cterm=underline",
	 "CursorLine term=underline cterm=underline guibg=Grey40"),
    CENT("ColorColumn term=reverse ctermbg=DarkRed",
	 "ColorColumn term=reverse ctermbg=DarkRed guibg=DarkRed"),
#endif
    CENT("MatchParen term=reverse ctermbg=DarkCyan",
	 "MatchParen term=reverse ctermbg=DarkCyan guibg=DarkCyan"),
#ifdef FEAT_CONCEAL
    CENT("Conceal ctermbg=DarkGrey ctermfg=LightGrey",
	 "Conceal ctermbg=DarkGrey ctermfg=LightGrey guibg=DarkGrey guifg=LightGrey"),
#endif
#ifdef FEAT_TERMINAL
    CENT("StatusLineTerm term=reverse,bold cterm=bold ctermfg=Black ctermbg=LightGreen",
	 "StatusLineTerm term=reverse,bold cterm=bold ctermfg=Black ctermbg=LightGreen gui=bold guifg=bg guibg=LightGreen"),
    CENT("StatusLineTermNC term=reverse ctermfg=Black ctermbg=LightGreen",
	 "StatusLineTermNC term=reverse ctermfg=Black ctermbg=LightGreen guifg=bg guibg=LightGreen"),
#endif
#ifdef FEAT_MENU
    CENT("ToolbarLine term=underline ctermbg=DarkGrey",
	 "ToolbarLine term=underline ctermbg=DarkGrey guibg=Grey50"),
    CENT("ToolbarButton cterm=bold ctermfg=Black ctermbg=LightGrey",
	 "ToolbarButton cterm=bold ctermfg=Black ctermbg=LightGrey gui=bold guifg=Black guibg=LightGrey"),
#endif
    ರಿಕ್ತ
};

#ಯದಿ defined(FEAT_SYN_HL)
/*
 * Returns the number of highlight groups.
 */
    ಪೂರ್ಣಾಙ್ಕ
highlight_ಅಂಕೆ_groups(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ highlight_ga.ga_len;
}

/*
 * Returns the name of a highlight group.
 */
    char_u *
highlight_group_name(ಪೂರ್ಣಾಙ್ಕ id)
{
    ಪ್ರತ್ಯಾಗಮ HL_TABLE()[id].sg_name;
}

/*
 * Returns the ID of the link to a highlight group.
 */
    ಪೂರ್ಣಾಙ್ಕ
highlight_link_id(ಪೂರ್ಣಾಙ್ಕ id)
{
    ಪ್ರತ್ಯಾಗಮ HL_TABLE()[id].sg_link;
}
#endif

ಸ್ಥಿರ ಶೂನ್ಯ resolve_fallback_fg_to_rgb(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ resolve_fallback_bg_to_rgb(ಶೂನ್ಯ);

    ಶೂನ್ಯ
init_highlight(
    ಪೂರ್ಣಾಙ್ಕ		both,	    // include groups where 'bg' doesn't matter
    ಪೂರ್ಣಾಙ್ಕ		reset)	    // clear group first
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ವರ್ಣ	**pp;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	had_both = FALSE;
#ifdef FEAT_EVAL
    char_u	*p;

    // Try finding the color scheme file.  Used when a color file was loaded
    // and 'background' or 't_Co' is changed.
    p = get_var_ಮೌಲ್ಯ((char_u *)"g:colors_name");
    ಯದಿ (p != ರಿಕ್ತ)
    {
	// The value of g:colors_name could be freed when sourcing the script,
	// making "p" invalid, so copy it.
	char_u *copy_p = vim_strsave(p);
	ಪೂರ್ಣಾಙ್ಕ    r;

	ಯದಿ (copy_p != ರಿಕ್ತ)
	{
	    r = load_colors(copy_p);
	    vim_free(copy_p);
	    ಯದಿ (r == OK)
		ಪ್ರತ್ಯಾಗಮ;
	}
    }

#endif

    // Didn't use a color file, use the compiled-in colors.
    ಯದಿ (both)
    {
	had_both = TRUE;
	pp = highlight_init_both;
	ಪ್ರತಿ (i = 0; pp[i] != ರಿಕ್ತ; ++i)
	    do_highlight((char_u *)pp[i], reset, TRUE);
    }
    ಅನ್ಯಥಾ ಯದಿ (!had_both)
	// Don't do anything before the call with both == TRUE from main().
	// Not everything has been setup then, and that call will overrule
	// everything anyway.
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (*p_bg == 'l')
	pp = highlight_init_light;
    ಅನ್ಯಥಾ
	pp = highlight_init_dark;
    ಪ್ರತಿ (i = 0; pp[i] != ರಿಕ್ತ; ++i)
	do_highlight((char_u *)pp[i], reset, TRUE);

    // Reverse looks ugly, but grey may not work for less than 8 colors.  Thus
    // let it depend on the number of colors available.
    ಯದಿ (t_colors < 8)
	do_highlight((char_u *)"Visual term=reverse cterm=reverse ctermbg=NONE ctermfg=NONE",
		FALSE, TRUE);
    // With 8 colors brown is equal to yellow, need to use black for Search fg
    // to avoid Statement highlighted text disappears.
    // Clear the attributes, needed when changing the t_Co value.
    ಯದಿ (t_colors <= 8)
    {
	ಯದಿ (*p_bg == 'l')
	    do_highlight((char_u *)"Search ctermfg=black", FALSE, TRUE);
    }

#ifdef FEAT_SYN_HL
    // If syntax highlighting is enabled load the highlighting for it.
    ಯದಿ (get_var_ಮೌಲ್ಯ((char_u *)"g:syntax_on") != ರಿಕ್ತ)
    {
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = 0;

	ಯದಿ (recursive >= 5)
	    emsg(_(e_recursive_loop_loading_syncolor_vim));
	ಅನ್ಯಥಾ
	{
	    ++recursive;
	    (ಶೂನ್ಯ)source_runtime((char_u *)"syntax/syncolor.vim", DIP_ALL);
	    --recursive;
	}
    }
#endif
    resolve_fallback_fg_to_rgb();
    resolve_fallback_bg_to_rgb();
}

#ಯದಿ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
/*
 * Load a default color list. Intended to support legacy color names but allows
 * the user to override the color values. Only loaded once.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
load_default_colors_lists(ಶೂನ್ಯ)
{
    // Lacking a default color list isn't the end of the world but it is likely
    // an inconvenience so users should know when it is missing.
    ಯದಿ (source_runtime((char_u *)"colors/lists/default.vim", DIP_ALL) != OK)
	ಸಂದೇಶ("failed to load colors/lists/default.vim");
}
#endif

/*
 * Load color file "name".
 * Return OK for success, FAIL for failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
load_colors(char_u *name)
{
    char_u	*buf;
    ಪೂರ್ಣಾಙ್ಕ		retval = FAIL;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = FALSE;

    // When being called recursively, this is probably because setting
    // 'background' caused the highlighting to be reloaded.  This means it is
    // working, thus we should return OK.
    ಯದಿ (recursive)
	ಪ್ರತ್ಯಾಗಮ OK;

    recursive = TRUE;
    buf = alloc(STRLEN(name) + 12);
    ಯದಿ (buf != ರಿಕ್ತ)
    {
#ಯದಿ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	load_default_colors_lists();
#endif
	apply_autocmds(EVENT_COLORSCHEMEPRE, name,
					       curbuf->b_fname, FALSE, curbuf);
	sprintf((ವರ್ಣ *)buf, "colors/%s.vim", name);
	retval = source_runtime(buf, DIP_START + DIP_OPT);
	vim_free(buf);
	ಯದಿ (retval == OK)
	    apply_autocmds(EVENT_COLORSCHEME, name, curbuf->b_fname,
								FALSE, curbuf);
    }
    recursive = FALSE;

    ಪ್ರತ್ಯಾಗಮ retval;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ color_numbers_16[28] = {0, 1, 2, 3,
				 4, 5, 6, 6,
				 7, 7, 7, 7,
				 8, 8,
				 9, 9, 10, 10,
				 11, 11, 12, 12, 13,
				 13, 14, 14, 15, -1};
// for xterm with 88 colors...
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ color_numbers_88[28] = {0, 4, 2, 6,
				 1, 5, 32, 72,
				 84, 84, 7, 7,
				 82, 82,
				 12, 43, 10, 61,
				 14, 63, 9, 74, 13,
				 75, 11, 78, 15, -1};
// for xterm with 256 colors...
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ color_numbers_256[28] = {0, 4, 2, 6,
				 1, 5, 130, 3,
				 248, 248, 7, 7,
				 242, 242,
				 12, 81, 10, 121,
				 14, 159, 9, 224, 13,
				 225, 11, 229, 15, -1};
// for terminals with less than 16 colors...
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ color_numbers_8[28] = {0, 4, 2, 6,
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
lookup_color(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ foreground, ಪೂರ್ಣಾಙ್ಕ *boldp)
{
    ಪೂರ್ಣಾಙ್ಕ		color = color_numbers_16[idx];
    char_u	*p;

    // Use the _16 table to check if it's a valid color name.
    ಯದಿ (color < 0)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (t_colors == 8)
    {
	// t_Co is 8: use the 8 colors table
#ಯದಿ defined(__QNXNTO__)
	// On qnx, the 8 & 16 color arrays are the same
	ಯದಿ (STRNCMP(T_NAME, "qansi", 5) == 0)
	    color = color_numbers_16[idx];
	ಅನ್ಯಥಾ
#endif
	    color = color_numbers_8[idx];
	ಯದಿ (foreground)
	{
	    // set/reset bold attribute to get light foreground
	    // colors (on some terminals, e.g. "linux")
	    ಯದಿ (color & 8)
		*boldp = TRUE;
	    ಅನ್ಯಥಾ
		*boldp = FALSE;
	}
	color &= 7;	// truncate to 8 colors
    }
    ಅನ್ಯಥಾ ಯದಿ (t_colors == 16 || t_colors == 88
					   || t_colors >= 256)
    {
	// Guess: if the termcap entry ends in 'm', it is
	// probably an xterm-like terminal.  Use the changed
	// order for colors.
	ಯದಿ (*T_CAF != NUL)
	    p = T_CAF;
	ಅನ್ಯಥಾ
	    p = T_CSF;
	ಯದಿ (*p != NUL && (t_colors > 256
			      || *(p + STRLEN(p) - 1) == 'm'))
	{
	    ಯದಿ (t_colors == 88)
		color = color_numbers_88[idx];
	    ಅನ್ಯಥಾ ಯದಿ (t_colors >= 256)
		color = color_numbers_256[idx];
	    ಅನ್ಯಥಾ
		color = color_numbers_8[idx];
	}
#ifdef FEAT_TERMRESPONSE
	ಯದಿ (t_colors >= 256 && color == 15 && is_mac_terminal)
	    // Terminal.app has a bug: 15 is light grey. Use white
	    // from the color cube instead.
	    color = 231;
#endif
    }
    ಪ್ರತ್ಯಾಗಮ color;
}

/*
 * Link highlight group 'from_hg' to 'to_hg'.
 * 'dodefault' is set to TRUE for ":highlight default link".
 * 'forceit' is set to TRUE for ":highlight! link"
 * 'init' is set to TRUE when initializing all the highlight groups.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_group_link(
	char_u	*from_hg,
	ಪೂರ್ಣಾಙ್ಕ	from_len,
	char_u	*to_hg,
	ಪೂರ್ಣಾಙ್ಕ	to_len,
	ಪೂರ್ಣಾಙ್ಕ	dodefault,
	ಪೂರ್ಣಾಙ್ಕ	forceit,
	ಪೂರ್ಣಾಙ್ಕ	init)
{
    ಪೂರ್ಣಾಙ್ಕ		from_id;
    ಪೂರ್ಣಾಙ್ಕ		to_id;
    hl_group_T	*hlgroup = ರಿಕ್ತ;

    from_id = syn_check_group(from_hg, from_len);
    ಯದಿ (STRNCMP(to_hg, "NONE", 4) == 0)
	to_id = 0;
    ಅನ್ಯಥಾ
	to_id = syn_check_group(to_hg, to_len);

    ಯದಿ (from_id > 0)
    {
	hlgroup = &HL_TABLE()[from_id - 1];
	ಯದಿ (dodefault && (forceit || hlgroup->sg_deflink == 0))
	{
	    hlgroup->sg_deflink = to_id;
#ifdef FEAT_EVAL
	    hlgroup->sg_deflink_sctx = current_sctx;
	    hlgroup->sg_deflink_sctx.sc_lnum += SOURCING_LNUM;
#endif
	}
    }

    ಯದಿ (from_id > 0 && (!init || hlgroup->sg_set == 0))
    {
	// Don't allow a link when there already is some highlighting
	// for the group, unless '!' is used
	ಯದಿ (to_id > 0 && !forceit && !init
		&& hl_has_settings(from_id - 1, dodefault))
	{
	    ಯದಿ (SOURCING_NAME == ರಿಕ್ತ && !dodefault)
		emsg(_(e_group_has_settings_highlight_link_ignored));
	}
	ಅನ್ಯಥಾ ಯದಿ (hlgroup->sg_link != to_id
#ifdef FEAT_EVAL
		|| hlgroup->sg_script_ctx.sc_sid != current_sctx.sc_sid
#endif
		|| hlgroup->sg_cleared)
	{
	    ಯದಿ (!init)
		hlgroup->sg_set |= SG_LINK;
	    hlgroup->sg_link = to_id;
#ifdef FEAT_EVAL
	    hlgroup->sg_script_ctx = current_sctx;
	    hlgroup->sg_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
	    hlgroup->sg_cleared = FALSE;
	    redraw_all_later(UPD_SOME_VALID);

	    // Only call highlight_changed() once after multiple changes.
	    need_highlight_changed = TRUE;
	}
    }

}

/*
 * Reset all highlighting to the defaults. Removes all highlighting for the
 * groups added by the user.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_reset_all(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;

#ifdef FEAT_GUI
    // First, we do not destroy the old values, but allocate the new
    // ones and update the display. THEN we destroy the old values.
    // If we destroy the old values first, then the old values
    // (such as GuiFont's or GuiFontset's) will still be displayed but
    // invalid because they were free'd.
    ಯದಿ (gui.in_use)
    {
# ifdef FEAT_BEVAL_TIP
	gui_init_tooltip_font();
# endif
# ಯದಿ defined(FEAT_MENU) && defined(FEAT_GUI_MOTIF)
	gui_init_menu_font();
# endif
    }
# ಯದಿ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_X11)
    gui_mch_def_colors();
# endif
# ifdef FEAT_GUI_MSWIN
    ಯದಿ (gui.in_use)
	gui_mch_set_titlebar_colors();
# endif
# ifdef FEAT_GUI_X11
#  ifdef FEAT_MENU

    // This only needs to be done when there is no Menu highlight
    // group defined by default, which IS currently the case.
    gui_mch_new_menu_colors();
#  endif
    ಯದಿ (gui.in_use)
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
#endif
#ifdef FEAT_EVAL
    do_unlet((char_u *)"g:colors_name", TRUE);
#endif
    restore_cterm_colors();

    // Clear all default highlight groups and load the defaults.
    ಪ್ರತಿ (idx = 0; idx < highlight_ga.ga_len; ++idx)
	highlight_clear(idx);
    init_highlight(TRUE, TRUE);
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಯದಿ (USE_24BIT)
	highlight_gui_started();
    ಅನ್ಯಥಾ
#endif
	highlight_changed();
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
highlight_set_termgui_attr(ಪೂರ್ಣಾಙ್ಕ idx, char_u *ಕೀಲಿ, char_u *arg, ಪೂರ್ಣಾಙ್ಕ init)
{
    ಪೂರ್ಣಾಙ್ಕ		attr;
    size_t	off;
    keyvalue_T	target;
    keyvalue_T	*entry;

    attr = 0;
    off = 0;
    target.ಕೀಲಿ = 0;
    target.ಮೌಲ್ಯ.length = 0;	// not used, see cmp_keyvalue_value_ni()
    ಯಾವತ್ (arg[off] != NUL)
    {
	target.ಮೌಲ್ಯ.string = arg + off;
	entry = (keyvalue_T *)bsearch(&target, &highlight_tab,
		ARRAY_LENGTH(highlight_tab), ಪರಿಮಾಣ(highlight_tab[0]),
		cmp_keyvalue_ಮೌಲ್ಯ_ni);
	ಯದಿ (entry == ರಿಕ್ತ)
	{
	    semsg(_(e_illegal_ಮೌಲ್ಯ_str), arg);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	attr |= entry->ಕೀಲಿ;
	off += entry->ಮೌಲ್ಯ.length;
	ಯದಿ (arg[off] == ',')		// another one follows
	    ++off;
    }
    ಯದಿ (*ಕೀಲಿ == 'T')
    {
	ಯದಿ (!init || !(HL_TABLE()[idx].sg_set & SG_TERM))
	{
	    ಯದಿ (!init)
		HL_TABLE()[idx].sg_set |= SG_TERM;
	    HL_TABLE()[idx].sg_term = attr;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (*ಕೀಲಿ == 'C')
    {
	ಯದಿ (!init || !(HL_TABLE()[idx].sg_set & SG_CTERM))
	{
	    ಯದಿ (!init)
		HL_TABLE()[idx].sg_set |= SG_CTERM;
	    HL_TABLE()[idx].sg_cterm = attr;
	    HL_TABLE()[idx].sg_cterm_bold = FALSE;
	}
    }
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_EVAL)
    ಅನ್ಯಥಾ
    {
	ಯದಿ (!init || !(HL_TABLE()[idx].sg_set & SG_GUI))
	{
	    ಯದಿ (!init)
		HL_TABLE()[idx].sg_set |= SG_GUI;
	    HL_TABLE()[idx].sg_gui = attr;
	}
    }
#endif

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

#ifdef FEAT_GUI
/*
 * Set the font for the highlight group at 'idx'.
 * 'arg' is the font name.
 * Returns TRUE if the font is changed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
highlight_set_font(
	ಪೂರ್ಣಾಙ್ಕ	idx,
	char_u	*arg,
	ಪೂರ್ಣಾಙ್ಕ	is_normal_group,
	ಪೂರ್ಣಾಙ್ಕ	is_menu_group,
	ಪೂರ್ಣಾಙ್ಕ	is_tooltip_group)
{
    ಪೂರ್ಣಾಙ್ಕ		did_change = FALSE;

    // in non-GUI fonts are simply ignored
    ಯದಿ (HL_TABLE()[idx].sg_font_name != ರಿಕ್ತ
	    && STRCMP(HL_TABLE()[idx].sg_font_name, arg) == 0)
    {
	// Font name didn't change, ignore.
    }
    ಅನ್ಯಥಾ ಯದಿ (!gui.shell_created)
    {
	// GUI not started yet, always accept the name.
	vim_free(HL_TABLE()[idx].sg_font_name);
	HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	did_change = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	GuiFont temp_sg_font = HL_TABLE()[idx].sg_font;
# ifdef FEAT_XFONTSET
	GuiFontset temp_sg_fontset = HL_TABLE()[idx].sg_fontset;
# endif
	// First, save the current font/fontset.
	// Then try to allocate the font/fontset.
	// If the allocation fails, HL_TABLE()[idx].sg_font OR
	// sg_fontset will be set to NOFONT or NOFONTSET respectively.

	HL_TABLE()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
	HL_TABLE()[idx].sg_fontset = NOFONTSET;
# endif
	hl_do_font(idx, arg, is_normal_group, is_menu_group,
		is_tooltip_group, FALSE);

# ifdef FEAT_XFONTSET
	ಯದಿ (HL_TABLE()[idx].sg_fontset != NOFONTSET)
	{
	    // New fontset was accepted. Free the old one, if there
	    // was one.
	    gui_mch_free_fontset(temp_sg_fontset);
	    did_change = TRUE;
	}
	ಅನ್ಯಥಾ
	    HL_TABLE()[idx].sg_fontset = temp_sg_fontset;
# endif
	ಯದಿ (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    // New font was accepted. Free the old one, if there was
	    // one.
	    gui_mch_free_font(temp_sg_font);
	    did_change = TRUE;
	}
	ಅನ್ಯಥಾ
	    HL_TABLE()[idx].sg_font = temp_sg_font;
	// Update sg_font_name once here, after both font and fontset results
	// are known.
	ಯದಿ (did_change)
	{
	    vim_free(HL_TABLE()[idx].sg_font_name);
	    HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	}
    }

    ಪ್ರತ್ಯಾಗಮ did_change;
}
#endif

/*
 * Set the cterm foreground color for the Normal highlight group to "color" and
 * the bold attribute to "bold".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hl_set_ctermfg_normal_group(ಪೂರ್ಣಾಙ್ಕ color, ಪೂರ್ಣಾಙ್ಕ bold)
{
    cterm_normal_fg_color = color + 1;
    cterm_normal_fg_bold = bold;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    ಯದಿ (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	ಯದಿ (termcap_active && color >= 0)
	    term_fg_color(color);
    }
}

/*
 * Set the cterm foreground color for the highlight group at 'idx' to 'color'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_set_ctermfg(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ color, ಪೂರ್ಣಾಙ್ಕ is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_fg = color + 1;
    ಯದಿ (is_normal_group)
	hl_set_ctermfg_normal_group(color,
					(HL_TABLE()[idx].sg_cterm & HL_BOLD));
}

/*
 * Set the cterm background color for the Normal highlight group to "color".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hl_set_ctermbg_normal_group(ಪೂರ್ಣಾಙ್ಕ color)
{
    cterm_normal_bg_color = color + 1;
#ifdef FEAT_GUI
    // Don't mess with 'background' if the GUI is used.
    ಯದಿ (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	ಯದಿ (color >= 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ dark = -1;

	    ಯದಿ (termcap_active)
		term_bg_color(color);
	    ಯದಿ (t_colors < 16)
		dark = (color == 0 || color == 4);
	    // Limit the heuristic to the standard 16 colors
	    ಅನ್ಯಥಾ ಯದಿ (color < 16)
		dark = (color < 7 || color == 8);
	    // Set the 'background' option if the value is
	    // wrong.
	    ಯದಿ (dark != -1
		    && dark != (*p_bg == 'd')
		    && !option_was_set((char_u *)"bg"))
	    {
		set_option_ಮೌಲ್ಯ_give_err((char_u *)"bg",
			0L, (char_u *)(dark ? "dark" : "light"), 0);
		reset_option_was_set((char_u *)"bg");
	    }
	}
    }
}

/*
 * Set the cterm background color for the highlight group at 'idx' to 'color'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_set_ctermbg(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ color, ಪೂರ್ಣಾಙ್ಕ is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_bg = color + 1;
    ಯದಿ (is_normal_group)
	hl_set_ctermbg_normal_group(color);
}

/*
 * Set the cterm underline color for the Normal highlight group to "color".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hl_set_ctermul_normal_group(ಪೂರ್ಣಾಙ್ಕ color)
{
    cterm_normal_ul_color = color + 1;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    ಯದಿ (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	ಯದಿ (termcap_active && color >= 0)
	    term_ul_color(color);
    }
}

/*
 * Set the cterm underline color for the highlight group at 'idx' to 'color'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_set_ctermul(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ color, ಪೂರ್ಣಾಙ್ಕ is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_ul = color + 1;
    ಯದಿ (is_normal_group)
	hl_set_ctermul_normal_group(color);
}

/*
 * Set the cterm font for the highlight group at 'idx'.
 * 'arg' is the color name or the numeric value as a string.
 * 'init' is set to TRUE when initializing highlighting.
 * Called for the ":highlight" command and the "hlset()" function.
 *
 * Returns TRUE if the font is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
highlight_set_cterm_font(
	ಪೂರ್ಣಾಙ್ಕ	idx,
	char_u	*arg,
	ಪೂರ್ಣಾಙ್ಕ	init)
{
    ಪೂರ್ಣಾಙ್ಕ		font;

    ಯದಿ (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    ಯದಿ (VIM_ISDIGIT(*arg))
	font = atoi((ವರ್ಣ *)arg);
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(arg, "NONE") == 0)
	font = -1;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ FALSE;

    HL_TABLE()[idx].sg_cterm_font = font + 1;
    ಪ್ರತ್ಯಾಗಮ TRUE;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
highlight_set_cterm_color(
	ಪೂರ್ಣಾಙ್ಕ	idx,
	char_u	*ಕೀಲಿ,
	char_u	*ಕೀಲಿ_start,
	char_u	*arg,
	ಪೂರ್ಣಾಙ್ಕ	is_normal_group,
	ಪೂರ್ಣಾಙ್ಕ	init)
{
    ಪೂರ್ಣಾಙ್ಕ		color;

    ಯದಿ (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    // When setting the foreground color, and previously the "bold"
    // flag was set for a light color, reset it now
    ಯದಿ (ಕೀಲಿ[5] == 'F' && HL_TABLE()[idx].sg_cterm_bold)
    {
	HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
	HL_TABLE()[idx].sg_cterm_bold = FALSE;
    }

    ಯದಿ (VIM_ISDIGIT(*arg))
	color = atoi((ವರ್ಣ *)arg);
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(arg, "fg") == 0)
    {
	ಯದಿ (cterm_normal_fg_color)
	    color = cterm_normal_fg_color - 1;
	ಅನ್ಯಥಾ
	{
	    emsg(_(e_fg_color_unknown));
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(arg, "bg") == 0)
    {
	ಯದಿ (cterm_normal_bg_color > 0)
	    color = cterm_normal_bg_color - 1;
	ಅನ್ಯಥಾ
	{
	    emsg(_(e_bg_color_unknown));
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(arg, "ul") == 0)
    {
	ಯದಿ (cterm_normal_ul_color > 0)
	    color = cterm_normal_ul_color - 1;
	ಅನ್ಯಥಾ
	{
	    emsg(_(e_ul_color_unknown));
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ bold = MAYBE;
	keyvalue_T target;
	keyvalue_T *entry;

	target.ಕೀಲಿ = 0;
	target.ಮೌಲ್ಯ.string = arg;
	target.ಮೌಲ್ಯ.length = 0;	// not used, see cmp_keyvalue_value_i()
	entry = (keyvalue_T *)bsearch(&target, &color_name_tab,
		ARRAY_LENGTH(color_name_tab), ಪರಿಮಾಣ(color_name_tab[0]),
		cmp_keyvalue_ಮೌಲ್ಯ_i);
	ಯದಿ (entry == ರಿಕ್ತ)
	{
	    semsg(_(e_color_name_or_ಸಂಖ್ಯೆ_not_recognized_str), ಕೀಲಿ_start);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	color = lookup_color(entry->ಕೀಲಿ, ಕೀಲಿ[5] == 'F', &bold);

	// set/reset bold attribute to get light foreground
	// colors (on some terminals, e.g. "linux")
	ಯದಿ (bold == TRUE)
	{
	    HL_TABLE()[idx].sg_cterm |= HL_BOLD;
	    HL_TABLE()[idx].sg_cterm_bold = TRUE;
	}
	ಅನ್ಯಥಾ ಯದಿ (bold == FALSE)
	    HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
    }

    // Add one to the argument, to avoid zero.  Zero is used for
    // "NONE", then "color" is -1.
    ಯದಿ (ಕೀಲಿ[5] == 'F')
	highlight_set_ctermfg(idx, color, is_normal_group);
    ಅನ್ಯಥಾ ಯದಿ (ಕೀಲಿ[5] == 'B')
	highlight_set_ctermbg(idx, color, is_normal_group);
    ಅನ್ಯಥಾ // ctermul
	highlight_set_ctermul(idx, color, is_normal_group);

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

#ಯದಿ defined(FEAT_GUI) || defined(FEAT_EVAL)
/*
 * Set the GUI foreground color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
highlight_set_guifg(
	ಪೂರ್ಣಾಙ್ಕ	idx,
	char_u	*arg,
	ಪೂರ್ಣಾಙ್ಕ	is_menu_group UNUSED,
	ಪೂರ್ಣಾಙ್ಕ	is_scrollbar_group UNUSED,
	ಪೂರ್ಣಾಙ್ಕ	is_tooltip_group UNUSED,
	ಪೂರ್ಣಾಙ್ಕ	is_titlebar_group UNUSED,
	ಪೂರ್ಣಾಙ್ಕ	is_titlebarnc_group UNUSED,
	ಪೂರ್ಣಾಙ್ಕ	*do_colors UNUSED,
	ಪೂರ್ಣಾಙ್ಕ	init)
{
# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    long	i;
# endif
    char_u	**namep;
    ಪೂರ್ಣಾಙ್ಕ		did_change = FALSE;

    ಯದಿ (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    namep = &HL_TABLE()[idx].sg_gui_fg_name;
    ಯದಿ (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guifg colors are only used when recognized
    i = color_name2handle(arg);
    ಯದಿ (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_fg = i;
# endif
	ಯದಿ (*namep == ರಿಕ್ತ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ಯದಿ (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    ಅನ್ಯಥಾ
		*namep = ರಿಕ್ತ;
	    did_change = TRUE;
	}
# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	ಯದಿ (is_titlebar_group && gui.title_fg_pixel != i)
	{
	    gui.title_fg_pixel = i;
	    *do_colors = TRUE;
	}
	ಯದಿ (is_titlebarnc_group && gui.titlenc_fg_pixel != i)
	{
	    gui.titlenc_fg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	ಯದಿ (is_menu_group && gui.menu_fg_pixel != i)
	{
	    gui.menu_fg_pixel = i;
	    *do_colors = TRUE;
	}
	ಯದಿ (is_scrollbar_group && gui.scroll_fg_pixel != i)
	{
	    gui.scroll_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	ಯದಿ (is_tooltip_group && gui.tooltip_fg_pixel != i)
	{
	    gui.tooltip_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    ಪ್ರತ್ಯಾಗಮ did_change;
}

/*
 * Set the GUI background color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
highlight_set_guibg(
	ಪೂರ್ಣಾಙ್ಕ	idx,
	char_u	*arg,
	ಪೂರ್ಣಾಙ್ಕ	is_menu_group UNUSED,
	ಪೂರ್ಣಾಙ್ಕ	is_scrollbar_group UNUSED,
	ಪೂರ್ಣಾಙ್ಕ	is_tooltip_group UNUSED,
	ಪೂರ್ಣಾಙ್ಕ	is_titlebar_group UNUSED,
	ಪೂರ್ಣಾಙ್ಕ	is_titlebarnc_group UNUSED,
	ಪೂರ್ಣಾಙ್ಕ	*do_colors UNUSED,
	ಪೂರ್ಣಾಙ್ಕ	init)
{
# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಪೂರ್ಣಾಙ್ಕ		i;
# endif
    char_u	**namep;
    ಪೂರ್ಣಾಙ್ಕ		did_change = FALSE;

    ಯದಿ (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    namep = &HL_TABLE()[idx].sg_gui_bg_name;
    ಯದಿ (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guibg colors are only used when recognized
    i = color_name2handle(arg);
    ಯದಿ (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_bg = i;
# endif
	ಯದಿ (*namep == ರಿಕ್ತ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ಯದಿ (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    ಅನ್ಯಥಾ
		*namep = ರಿಕ್ತ;
	    did_change = TRUE;
	}
# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	ಯದಿ (is_titlebar_group && gui.title_bg_pixel != i)
	{
	    gui.title_bg_pixel = i;
	    *do_colors = TRUE;
	}
	ಯದಿ (is_titlebarnc_group && gui.titlenc_bg_pixel != i)
	{
	    gui.titlenc_bg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	ಯದಿ (is_menu_group && gui.menu_bg_pixel != i)
	{
	    gui.menu_bg_pixel = i;
	    *do_colors = TRUE;
	}
	ಯದಿ (is_scrollbar_group && gui.scroll_bg_pixel != i)
	{
	    gui.scroll_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	ಯದಿ (is_tooltip_group && gui.tooltip_bg_pixel != i)
	{
	    gui.tooltip_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    ಪ್ರತ್ಯಾಗಮ did_change;
}

/*
 * Set the GUI undercurl/strikethrough color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
highlight_set_guisp(ಪೂರ್ಣಾಙ್ಕ idx, char_u *arg, ಪೂರ್ಣಾಙ್ಕ init)
{
# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಪೂರ್ಣಾಙ್ಕ		i;
# endif
    ಪೂರ್ಣಾಙ್ಕ		did_change = FALSE;
    char_u	**namep;

    ಯದಿ (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    namep = &HL_TABLE()[idx].sg_gui_sp_name;
    ಯದಿ (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guisp colors are only used when recognized
    i = color_name2handle(arg);
    ಯದಿ (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_sp = i;
# endif
	ಯದಿ (*namep == ರಿಕ್ತ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ಯದಿ (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    ಅನ್ಯಥಾ
		*namep = ರಿಕ್ತ;
	    did_change = TRUE;
	}
# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    }
# endif

    ಪ್ರತ್ಯಾಗಮ did_change;
}
#endif

/*
 * Set the start/stop terminal codes for a highlight group.
 * Returns TRUE if the terminal code is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
highlight_set_startstop_termcode(ಪೂರ್ಣಾಙ್ಕ idx, char_u *ಕೀಲಿ, char_u *arg, ಪೂರ್ಣಾಙ್ಕ init)
{
    ಪೂರ್ಣಾಙ್ಕ		off;
    char_u	buf[100];
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*tname;
    char_u	*p;

    ಯದಿ (!init)
	HL_TABLE()[idx].sg_set |= SG_TERM;

    // The "start" and "stop"  arguments can be a literal escape
    // sequence, or a comma separated list of terminal codes.
    ಯದಿ (STRNCMP(arg, "t_", 2) == 0)
    {
	off = 0;
	buf[0] = 0;
	ಯಾವತ್ (arg[off] != NUL)
	{
	    // Isolate one termcap name
	    ಪ್ರತಿ (len = 0; arg[off + len] &&
		    arg[off + len] != ','; ++len)
		;
	    tname = vim_strnsave(arg + off, len);
	    ಯದಿ (tname == ರಿಕ್ತ)		// out of memory
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    // lookup the escape sequence for the item
	    p = get_term_code(tname);
	    vim_free(tname);
	    ಯದಿ (p == ರಿಕ್ತ)	    // ignore non-existing things
		p = (char_u *)"";

	    // Append it to the already found stuff
	    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)(STRLEN(buf) + STRLEN(p)) >= 99)
	    {
		semsg(_(e_terminal_code_too_long_str), arg);
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	    STRCAT(buf, p);

	    // Advance to the next item
	    off += len;
	    ಯದಿ (arg[off] == ',')	    // another one follows
		++off;
	}
    }
    ಅನ್ಯಥಾ
    {
	// Copy characters from arg[] to buf[], translating <> codes.
	ಪ್ರತಿ (p = arg, off = 0; off < 100 - 6 && *p; )
	{
	    len = trans_special(&p, buf + off, FSK_SIMPLIFY, FALSE, ರಿಕ್ತ);
	    ಯದಿ (len > 0)	    // recognized special char
		off += len;
	    ಅನ್ಯಥಾ		    // copy as normal char
		buf[off++] = *p++;
	}
	buf[off] = NUL;
    }

    ಯದಿ (STRCMP(buf, "NONE") == 0)	// resetting the value
	p = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	p = vim_strsave(buf);
    ಯದಿ (ಕೀಲಿ[2] == 'A')
    {
	vim_free(HL_TABLE()[idx].sg_start);
	HL_TABLE()[idx].sg_start = p;
    }
    ಅನ್ಯಥಾ
    {
	vim_free(HL_TABLE()[idx].sg_stop);
	HL_TABLE()[idx].sg_stop = p;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Handle the ":highlight .." command.
 * When using ":hi clear" this is called recursively for each group with
 * "forceit" and "init" both TRUE.
 */
    ಶೂನ್ಯ
do_highlight(
    char_u	*ಸಾಲು,
    ಪೂರ್ಣಾಙ್ಕ		forceit,
    ಪೂರ್ಣಾಙ್ಕ		init)	    // TRUE when called for initializing
{
    char_u	*name_end;
    char_u	*linep;
    char_u	*ಕೀಲಿ_start;
    char_u	*arg_start;
    char_u	*ಕೀಲಿ = ರಿಕ್ತ, *arg = ರಿಕ್ತ;
    long	i;
    ಪೂರ್ಣಾಙ್ಕ		id;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    hl_group_T	item_before;
    ಪೂರ್ಣಾಙ್ಕ		did_change = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		dodefault = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		doclear = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		dolink = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		is_normal_group = FALSE;	// "Normal" group
#ifdef FEAT_GUI_MSWIN
    ಪೂರ್ಣಾಙ್ಕ		is_titlebar_group = FALSE;	// "TitleBar" group
    ಪೂರ್ಣಾಙ್ಕ		is_titlebarnc_group = FALSE;	// "TitleBarNC" group
#ಅನ್ಯಥಾ
# define is_titlebar_group 0
# define is_titlebarnc_group 0
#endif
#ifdef FEAT_GUI_X11
    ಪೂರ್ಣಾಙ್ಕ		is_menu_group = FALSE;		// "Menu" group
    ಪೂರ್ಣಾಙ್ಕ		is_scrollbar_group = FALSE;	// "Scrollbar" group
    ಪೂರ್ಣಾಙ್ಕ		is_tooltip_group = FALSE;	// "Tooltip" group
#ಅನ್ಯಥಾ
# define is_menu_group 0
# define is_tooltip_group 0
# define is_scrollbar_group 0
#endif
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_EVAL)
    ಪೂರ್ಣಾಙ್ಕ		do_colors = FALSE;		// need to update colors?
#endif
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಪೂರ್ಣಾಙ್ಕ		did_highlight_changed = FALSE;
#endif

    // If no argument, list current highlighting.
    ಯದಿ (!init && ends_excmd2(ಸಾಲು - 1, ಸಾಲು))
    {
	ಪ್ರತಿ (i = 1; i <= highlight_ga.ga_len && !got_int; ++i)
	    // TODO: only call when the group has attributes set
	    highlight_list_one((ಪೂರ್ಣಾಙ್ಕ)i);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Isolate the name.
    name_end = skiptowhite(ಸಾಲು);
    linep = skipwhite(name_end);

    // Check for "default" argument.
    ಯದಿ (STRNCMP(ಸಾಲು, "default", name_end - ಸಾಲು) == 0)
    {
	dodefault = TRUE;
	ಸಾಲು = linep;
	name_end = skiptowhite(ಸಾಲು);
	linep = skipwhite(name_end);
    }

    // Check for "clear" or "link" argument.
    ಯದಿ (STRNCMP(ಸಾಲು, "clear", name_end - ಸಾಲು) == 0)
	doclear = TRUE;
    ಯದಿ (STRNCMP(ಸಾಲು, "link", name_end - ಸಾಲು) == 0)
	dolink = TRUE;

    // ":highlight {group-name}": list highlighting for one group.
    ಯದಿ (!doclear && !dolink && ends_excmd2(ಸಾಲು, linep))
    {
	id = syn_namen2id(ಸಾಲು, (ಪೂರ್ಣಾಙ್ಕ)(name_end - ಸಾಲು));
	ಯದಿ (id == 0)
	    semsg(_(e_highlight_group_name_not_found_str), ಸಾಲು);
	ಅನ್ಯಥಾ
	    highlight_list_one(id);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Handle ":highlight link {from} {to}" command.
    ಯದಿ (dolink)
    {
	char_u	    *from_start = linep;
	char_u	    *from_end;
	ಪೂರ್ಣಾಙ್ಕ	    from_len;
	char_u	    *to_start;
	char_u	    *to_end;
	ಪೂರ್ಣಾಙ್ಕ	    to_len;

	from_end = skiptowhite(from_start);
	to_start = skipwhite(from_end);
	to_end	 = skiptowhite(to_start);

	ಯದಿ (ends_excmd2(ಸಾಲು, from_start) || ends_excmd2(ಸಾಲು, to_start))
	{
	    semsg(_(e_not_enough_arguments_highlight_link_str), from_start);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (!ends_excmd2(ಸಾಲು, skipwhite(to_end)))
	{
	    semsg(_(e_too_many_arguments_highlight_link_str), from_start);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	from_len = (ಪೂರ್ಣಾಙ್ಕ)(from_end - from_start);
	to_len = (ಪೂರ್ಣಾಙ್ಕ)(to_end - to_start);
	highlight_group_link(from_start, from_len, to_start, to_len,
						dodefault, forceit, init);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (doclear)
    {
	// ":highlight clear [group]" command.
	ಯದಿ (ends_excmd2(ಸಾಲು, linep))
	{
	    // ":highlight clear" without group name
	    highlight_reset_all();
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಸಾಲು = linep;
	name_end = skiptowhite(ಸಾಲು);
	linep = skipwhite(name_end);
    }

    // Find the group name in the table.  If it does not exist yet, add it.
    id = syn_check_group(ಸಾಲು, (ಪೂರ್ಣಾಙ್ಕ)(name_end - ಸಾಲು));
    ಯದಿ (id == 0)	// failed (out of memory)
	ಪ್ರತ್ಯಾಗಮ;
    idx = id - 1;	// index is ID minus one

    // Return if "default" was used and the group already has settings.
    ಯದಿ (dodefault && hl_has_settings(idx, TRUE))
	ಪ್ರತ್ಯಾಗಮ;

    // Make a copy so we can check if any attribute actually changed.
    item_before = HL_TABLE()[idx];

    ಯದಿ (STRCMP(HL_TABLE()[idx].sg_name_u, "NORMAL") == 0)
	is_normal_group = TRUE;
#ifdef FEAT_GUI_MSWIN
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBAR") == 0)
	is_titlebar_group = TRUE;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBARNC") == 0)
	is_titlebarnc_group = TRUE;
#endif
#ifdef FEAT_GUI_X11
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(HL_TABLE()[idx].sg_name_u, "MENU") == 0)
	is_menu_group = TRUE;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(HL_TABLE()[idx].sg_name_u, "SCROLLBAR") == 0)
	is_scrollbar_group = TRUE;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(HL_TABLE()[idx].sg_name_u, "TOOLTIP") == 0)
	is_tooltip_group = TRUE;
#endif

    // Clear the highlighting for ":hi clear {group}" and ":hi clear".
    ಯದಿ (doclear || (forceit && init))
    {
	highlight_clear(idx);
	ಯದಿ (!doclear)
	    HL_TABLE()[idx].sg_set = 0;
    }

    ಯದಿ (!doclear)
	ಯಾವತ್ (!ends_excmd2(ಸಾಲು, linep))
	{
	    ಕೀಲಿ_start = linep;
	    ಯದಿ (*linep == '=')
	    {
		semsg(_(e_unexpected_equal_sign_str), ಕೀಲಿ_start);
		error = TRUE;
		ಭಙ್ಗ;
	    }

	    // Note: Keep this in sync with get_highlight_group_key.

	    // Isolate the key ("term", "ctermfg", "ctermbg", "font", "guifg"
	    // or "guibg").
	    ಯಾವತ್ (*linep && !VIM_ISWHITE(*linep) && *linep != '=')
		++linep;
	    vim_free(ಕೀಲಿ);
	    ಕೀಲಿ = vim_strnsave_up(ಕೀಲಿ_start, linep - ಕೀಲಿ_start);
	    ಯದಿ (ಕೀಲಿ == ರಿಕ್ತ)
	    {
		error = TRUE;
		ಭಙ್ಗ;
	    }
	    linep = skipwhite(linep);

	    ಯದಿ (STRCMP(ಕೀಲಿ, "NONE") == 0)
	    {
		ಯದಿ (!init || HL_TABLE()[idx].sg_set == 0)
		{
		    ಯದಿ (!init)
			HL_TABLE()[idx].sg_set |= SG_TERM+SG_CTERM+SG_GUI;
		    highlight_clear(idx);
		}
		ಅನುವರ್ತ;
	    }

	    // Check for the equal sign.
	    ಯದಿ (*linep != '=')
	    {
		semsg(_(e_missing_equal_sign_str_2), ಕೀಲಿ_start);
		error = TRUE;
		ಭಙ್ಗ;
	    }
	    ++linep;

	    // Isolate the argument.
	    linep = skipwhite(linep);
	    ಯದಿ (*linep == '\'')		// guifg='color name'
	    {
		arg_start = ++linep;
		linep = vim_strchr(linep, '\'');
		ಯದಿ (linep == ರಿಕ್ತ)
		{
		    semsg(_(e_invalid_argument_str), ಕೀಲಿ_start);
		    error = TRUE;
		    ಭಙ್ಗ;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		arg_start = linep;
		linep = skiptowhite(linep);
	    }
	    ಯದಿ (linep == arg_start)
	    {
		semsg(_(e_missing_argument_str), ಕೀಲಿ_start);
		error = TRUE;
		ಭಙ್ಗ;
	    }
	    vim_free(arg);
	    arg = vim_strnsave(arg_start, linep - arg_start);
	    ಯದಿ (arg == ರಿಕ್ತ)
	    {
		error = TRUE;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (*linep == '\'')
		++linep;

	    // Store the argument.
	    ಯದಿ (STRCMP(ಕೀಲಿ, "TERM") == 0
		    || STRCMP(ಕೀಲಿ, "CTERM") == 0
		    || STRCMP(ಕೀಲಿ, "GUI") == 0)
	    {
		ಯದಿ (!highlight_set_termgui_attr(idx, ಕೀಲಿ, arg, init))
		{
		    error = TRUE;
		    ಭಙ್ಗ;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(ಕೀಲಿ, "FONT") == 0)
	    {
		// in non-GUI fonts are simply ignored
#ifdef FEAT_GUI
		ಯದಿ (highlight_set_font(idx, arg, is_normal_group,
					      is_menu_group, is_tooltip_group))
		    did_change = TRUE;
#endif
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(ಕೀಲಿ, "CTERMFG") == 0
		    || STRCMP(ಕೀಲಿ, "CTERMBG") == 0
		    || STRCMP(ಕೀಲಿ, "CTERMUL") == 0)
	    {
		ಯದಿ (!highlight_set_cterm_color(idx, ಕೀಲಿ, ಕೀಲಿ_start, arg,
							is_normal_group, init))
		{
		    error = TRUE;
		    ಭಙ್ಗ;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(ಕೀಲಿ, "CTERMFONT") == 0)
	    {
		ಯದಿ (!highlight_set_cterm_font(idx, arg, init))
		{
		    error = TRUE;
		    ಭಙ್ಗ;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(ಕೀಲಿ, "GUIFG") == 0)
	    {
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_EVAL)
		ಯದಿ (highlight_set_guifg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(ಕೀಲಿ, "GUIBG") == 0)
	    {
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_EVAL)
		ಯದಿ (highlight_set_guibg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(ಕೀಲಿ, "GUISP") == 0)
	    {
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_EVAL)
		ಯದಿ (highlight_set_guisp(idx, arg, init))
		    did_change = TRUE;
#endif
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(ಕೀಲಿ, "START") == 0 || STRCMP(ಕೀಲಿ, "STOP") == 0)
	    {
		ಯದಿ (!highlight_set_startstop_termcode(idx, ಕೀಲಿ, arg, init))
		{
		    error = TRUE;
		    ಭಙ್ಗ;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		semsg(_(e_illegal_argument_str_3), ಕೀಲಿ_start);
		error = TRUE;
		ಭಙ್ಗ;
	    }
	    HL_TABLE()[idx].sg_cleared = FALSE;

	    // When highlighting has been given for a group, don't link it.
	    ಯದಿ (!init || !(HL_TABLE()[idx].sg_set & SG_LINK))
		HL_TABLE()[idx].sg_link = 0;

	    // Continue with next argument.
	    linep = skipwhite(linep);
	}

    // If there is an error, and it's a new entry, remove it from the table.
    ಯದಿ (error && idx == highlight_ga.ga_len)
	syn_unadd_group();
    ಅನ್ಯಥಾ
    {
	ಯದಿ (is_normal_group)
	{
	    HL_TABLE()[idx].sg_term_attr = 0;
	    HL_TABLE()[idx].sg_cterm_attr = 0;
#ifdef FEAT_GUI
	    HL_TABLE()[idx].sg_gui_attr = 0;
	    // Need to update all groups, because they might be using "bg"
	    // and/or "fg", which have been changed now.
#endif
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    ಯದಿ (USE_24BIT)
	    {
		highlight_gui_started();
		did_highlight_changed = TRUE;
		redraw_all_later(UPD_NOT_VALID);
	    }
#endif
	    resolve_fallback_fg_to_rgb();
	    resolve_fallback_bg_to_rgb();
#ifdef FEAT_VTP
	    control_console_color_rgb();
#endif
	}
#ifdef FEAT_GUI_MSWIN
	ಅನ್ಯಥಾ ಯದಿ (is_titlebar_group)
	{
	    ಯದಿ (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
	ಅನ್ಯಥಾ ಯದಿ (is_titlebarnc_group)
	{
	    ಯದಿ (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
#endif
#ifdef FEAT_GUI_X11
# ifdef FEAT_MENU
	ಅನ್ಯಥಾ ಯದಿ (is_menu_group)
	{
	    ಯದಿ (gui.in_use && do_colors)
		gui_mch_new_menu_colors();
	}
# endif
	ಅನ್ಯಥಾ ಯದಿ (is_scrollbar_group)
	{
	    ಯದಿ (gui.in_use && do_colors)
		gui_new_scrollbar_colors();
	    ಅನ್ಯಥಾ
		set_hl_attr(idx);
	}
# ifdef FEAT_BEVAL_GUI
	ಅನ್ಯಥಾ ಯದಿ (is_tooltip_group)
	{
	    ಯದಿ (gui.in_use && do_colors)
		gui_mch_new_tooltip_colors();
	}
# endif
#endif
	ಅನ್ಯಥಾ
	    set_hl_attr(idx);
#ifdef FEAT_EVAL
	HL_TABLE()[idx].sg_script_ctx = current_sctx;
	HL_TABLE()[idx].sg_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
    }

    vim_free(ಕೀಲಿ);
    vim_free(arg);

    // Only call highlight_changed() once, after a sequence of highlight
    // commands, and only if an attribute actually changed.
    ಯದಿ ((did_change
	   || memcmp(&HL_TABLE()[idx], &item_before, ಪರಿಮಾಣ(item_before)) != 0)
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    && !did_highlight_changed
#endif
       )
    {
	// Do not trigger a redraw when highlighting is changed while
	// redrawing.  This may happen when evaluating 'statusline' changes the
	// StatusLine group.
	ಯದಿ (!updating_screen)
	    redraw_all_later(UPD_NOT_VALID);
	need_highlight_changed = TRUE;
    }
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
free_highlight(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಪ್ರತಿ (i = 0; i < highlight_ga.ga_len; ++i)
    {
	highlight_clear(i);
	vim_free(HL_TABLE()[i].sg_name);
	vim_free(HL_TABLE()[i].sg_name_u - HLNAME_ಕೀಲಿ_OFF);
    }
    ga_clear(&highlight_ga);
    hash_clear(&highlight_ht);
    highlight_ht_inited = false;
}
#endif

/*
 * Reset the cterm colors to what they were before Vim was started, if
 * possible.  Otherwise reset them to zero.
 */
    ಶೂನ್ಯ
restore_cterm_colors(ಶೂನ್ಯ)
{
#ಯದಿ defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
    // Since t_me has been set, this probably means that the user
    // wants to use this as default colors.  Need to reset default
    // background/foreground colors.
    mch_set_normal_colors();
#ಅನ್ಯಥಾ
# ifdef VIMDLL
    ಯದಿ (!gui.in_use)
    {
	mch_set_normal_colors();
	ಪ್ರತ್ಯಾಗಮ;
    }
# endif
    cterm_normal_fg_color = 0;
    cterm_normal_fg_bold = 0;
    cterm_normal_bg_color = 0;
# ifdef FEAT_TERMGUICOLORS
    cterm_normal_fg_gui_color = INVALCOLOR;
    cterm_normal_bg_gui_color = INVALCOLOR;
    cterm_normal_ul_gui_color = INVALCOLOR;
# endif
    fallback_fg_rgb = INVALCOLOR;
    fallback_bg_rgb = INVALCOLOR;
#endif
}

/*
 * Return TRUE if highlight group "idx" has any settings.
 * When "check_link" is TRUE also check for an existing link.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
hl_has_settings(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ check_link)
{
    ಪ್ರತ್ಯಾಗಮ HL_TABLE()[idx].sg_cleared == 0
	 && (  HL_TABLE()[idx].sg_term_attr != 0
	    || HL_TABLE()[idx].sg_cterm_attr != 0
	    || HL_TABLE()[idx].sg_cterm_fg != 0
	    || HL_TABLE()[idx].sg_cterm_bg != 0
	    || HL_TABLE()[idx].sg_cterm_font != 0
#ifdef FEAT_GUI
	    || HL_TABLE()[idx].sg_gui_attr != 0
	    || HL_TABLE()[idx].sg_gui_fg_name != ರಿಕ್ತ
	    || HL_TABLE()[idx].sg_gui_bg_name != ರಿಕ್ತ
	    || HL_TABLE()[idx].sg_gui_sp_name != ರಿಕ್ತ
	    || HL_TABLE()[idx].sg_font_name != ರಿಕ್ತ
#endif
	    || (check_link && (HL_TABLE()[idx].sg_set & SG_LINK)));
}

/*
 * Clear highlighting for one group.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_clear(ಪೂರ್ಣಾಙ್ಕ idx)
{
    HL_TABLE()[idx].sg_cleared = TRUE;

    HL_TABLE()[idx].sg_term = 0;
    VIM_CLEAR(HL_TABLE()[idx].sg_start);
    VIM_CLEAR(HL_TABLE()[idx].sg_stop);
    HL_TABLE()[idx].sg_term_attr = 0;
    HL_TABLE()[idx].sg_cterm = 0;
    HL_TABLE()[idx].sg_cterm_bold = FALSE;
    HL_TABLE()[idx].sg_cterm_fg = 0;
    HL_TABLE()[idx].sg_cterm_bg = 0;
    HL_TABLE()[idx].sg_cterm_attr = 0;
    HL_TABLE()[idx].sg_cterm_font = 0;
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_EVAL)
    HL_TABLE()[idx].sg_gui = 0;
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_fg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_bg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_sp_name);
#endif
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_TABLE()[idx].sg_gui_fg = INVALCOLOR;
    HL_TABLE()[idx].sg_gui_bg = INVALCOLOR;
    HL_TABLE()[idx].sg_gui_sp = INVALCOLOR;
#endif
#ifdef FEAT_GUI
    gui_mch_free_font(HL_TABLE()[idx].sg_font);
    HL_TABLE()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
    gui_mch_free_fontset(HL_TABLE()[idx].sg_fontset);
    HL_TABLE()[idx].sg_fontset = NOFONTSET;
# endif
    VIM_CLEAR(HL_TABLE()[idx].sg_font_name);
    HL_TABLE()[idx].sg_gui_attr = 0;
#endif
    // Restore default link and context if they exist. Otherwise clears.
    HL_TABLE()[idx].sg_link = HL_TABLE()[idx].sg_deflink;
#ifdef FEAT_EVAL
    // Since we set the default link, set the location to where the default
    // link was set.
    HL_TABLE()[idx].sg_script_ctx = HL_TABLE()[idx].sg_deflink_sctx;
#endif
}

#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Set the normal foreground and background colors according to the "Normal"
 * highlighting group.  For X11 also set "Menu", "Scrollbar" and "Tooltip"
 * colors.  For MS-Windows also set "TitleBar" and "TitleBarNC" colors.
 */
    ಶೂನ್ಯ
set_normal_colors(ಶೂನ್ಯ)
{
# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    ಯದಿ (gui.in_use)
#  endif
    {
	ಯದಿ (set_group_colors((char_u *)"Normal",
				 &gui.norm_pixel, &gui.back_pixel,
				 FALSE, TRUE, FALSE))
	{
	    gui_mch_new_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  ifdef FEAT_GUI_MSWIN
	ಯದಿ (set_group_colors((char_u *)"TitleBar",
				 &gui.title_fg_pixel, &gui.title_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
	ಯದಿ (set_group_colors((char_u *)"TitleBarNC",
				 &gui.titlenc_fg_pixel, &gui.titlenc_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  endif
#  ifdef FEAT_GUI_X11
	ಯದಿ (set_group_colors((char_u *)"Menu",
			     &gui.menu_fg_pixel, &gui.menu_bg_pixel,
			     TRUE, FALSE, FALSE))
	{
#   ifdef FEAT_MENU
	    gui_mch_new_menu_colors();
#   endif
	    set_must_redraw(UPD_CLEAR);
	}
#   ifdef FEAT_BEVAL_GUI
	ಯದಿ (set_group_colors((char_u *)"Tooltip",
			     &gui.tooltip_fg_pixel, &gui.tooltip_bg_pixel,
			     FALSE, FALSE, TRUE))
	{
#    ifdef FEAT_TOOLBAR
	    gui_mch_new_tooltip_colors();
#    endif
	    set_must_redraw(UPD_CLEAR);
	}
#   endif
	ಯದಿ (set_group_colors((char_u *)"Scrollbar",
			&gui.scroll_fg_pixel, &gui.scroll_bg_pixel,
			FALSE, FALSE, FALSE))
	{
	    gui_new_scrollbar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  endif
    }
# endif
# ifdef FEAT_TERMGUICOLORS
#  ifdef FEAT_GUI
    ಅನ್ಯಥಾ
#  endif
    {
	ಪೂರ್ಣಾಙ್ಕ		idx;

	idx = syn_name2id((char_u *)"Normal") - 1;
	ಯದಿ (idx >= 0)
	{
	    gui_do_one_color(idx, FALSE, FALSE);

	    // If the normal fg or bg color changed a complete redraw is
	    // required.
	    ಯದಿ (cterm_normal_fg_gui_color != HL_TABLE()[idx].sg_gui_fg
		    || cterm_normal_bg_gui_color != HL_TABLE()[idx].sg_gui_bg)
	    {
		// if the GUI color is INVALCOLOR then we use the default cterm
		// color
		cterm_normal_fg_gui_color = HL_TABLE()[idx].sg_gui_fg;
		cterm_normal_bg_gui_color = HL_TABLE()[idx].sg_gui_bg;
		set_must_redraw(UPD_CLEAR);
	    }
	}
    }
# endif
}
#endif

#ಯದಿ defined(FEAT_GUI)
/*
 * Set the colors for "Normal", "Menu", "TitleBar", "TitleBarNC", "Tooltip" or
 * "Scrollbar".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_group_colors(
    char_u	*name,
    guicolor_T	*fgp,
    guicolor_T	*bgp,
    ಪೂರ್ಣಾಙ್ಕ		do_menu,
    ಪೂರ್ಣಾಙ್ಕ		use_norm,
    ಪೂರ್ಣಾಙ್ಕ		do_tooltip)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;

    idx = syn_name2id(name) - 1;
    ಯದಿ (idx < 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    gui_do_one_color(idx, do_menu, do_tooltip);

    ಯದಿ (HL_TABLE()[idx].sg_gui_fg != INVALCOLOR)
	*fgp = HL_TABLE()[idx].sg_gui_fg;
    ಅನ್ಯಥಾ ಯದಿ (use_norm)
	*fgp = gui.def_norm_pixel;
    ಯದಿ (HL_TABLE()[idx].sg_gui_bg != INVALCOLOR)
	*bgp = HL_TABLE()[idx].sg_gui_bg;
    ಅನ್ಯಥಾ ಯದಿ (use_norm)
	*bgp = gui.def_back_pixel;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Get the font of the "Normal" group.
 * Returns "" when it's not found or not set.
 */
    char_u *
hl_get_font_name(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		id;
    char_u	*s;

    id = syn_name2id((char_u *)"Normal");
    ಯದಿ (id > 0)
    {
	s = HL_TABLE()[id - 1].sg_font_name;
	ಯದಿ (s != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ s;
    }
    ಪ್ರತ್ಯಾಗಮ (char_u *)"";
}

/*
 * Set font for "Normal" group.  Called by gui_mch_init_font() when a font has
 * actually chosen to be used.
 */
    ಶೂನ್ಯ
hl_set_font_name(char_u *font_name)
{
    ಪೂರ್ಣಾಙ್ಕ	    id;

    id = syn_name2id((char_u *)"Normal");
    ಯದಿ (id <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    vim_free(HL_TABLE()[id - 1].sg_font_name);
    HL_TABLE()[id - 1].sg_font_name = vim_strsave(font_name);
}

/*
 * Set background color for "Normal" group.  Called by gui_set_bg_color()
 * when the color is known.
 */
    ಶೂನ್ಯ
hl_set_bg_color_name(
    char_u  *name)	    // must have been allocated
{
    ಪೂರ್ಣಾಙ್ಕ	    id;

    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    id = syn_name2id((char_u *)"Normal");
    ಯದಿ (id <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    vim_free(HL_TABLE()[id - 1].sg_gui_bg_name);
    HL_TABLE()[id - 1].sg_gui_bg_name = name;
}

/*
 * Set foreground color for "Normal" group.  Called by gui_set_fg_color()
 * when the color is known.
 */
    ಶೂನ್ಯ
hl_set_fg_color_name(
    char_u  *name)	    // must have been allocated
{
    ಪೂರ್ಣಾಙ್ಕ	    id;

    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    id = syn_name2id((char_u *)"Normal");
    ಯದಿ (id <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    vim_free(HL_TABLE()[id - 1].sg_gui_fg_name);
    HL_TABLE()[id - 1].sg_gui_fg_name = name;
}

/*
 * Return the handle for a font name.
 * Returns NOFONT when failed.
 */
    ಸ್ಥಿರ GuiFont
font_name2handle(char_u *name)
{
    ಯದಿ (STRCMP(name, "NONE") == 0)
	ಪ್ರತ್ಯಾಗಮ NOFONT;

    ಪ್ರತ್ಯಾಗಮ gui_mch_get_font(name, TRUE);
}

# ifdef FEAT_XFONTSET
/*
 * Return the handle for a fontset name.
 * Returns NOFONTSET when failed.
 */
    ಸ್ಥಿರ GuiFontset
fontset_name2handle(char_u *name, ಪೂರ್ಣಾಙ್ಕ fixed_width)
{
    ಯದಿ (STRCMP(name, "NONE") == 0)
	ಪ್ರತ್ಯಾಗಮ NOFONTSET;

    ಪ್ರತ್ಯಾಗಮ gui_mch_get_fontset(name, TRUE, fixed_width);
}
# endif

/*
 * Get the font or fontset for one highlight group.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hl_do_font(
    ಪೂರ್ಣಾಙ್ಕ		idx,
    char_u	*arg,
    ಪೂರ್ಣಾಙ್ಕ		do_normal,		// set normal font
    ಪೂರ್ಣಾಙ್ಕ		do_menu UNUSED,		// set menu font
    ಪೂರ್ಣಾಙ್ಕ		do_tooltip UNUSED,	// set tooltip font
    ಪೂರ್ಣಾಙ್ಕ		free_font)		// free current font/fontset
{
# ifdef FEAT_XFONTSET
    // If 'guifontset' is not empty, first try using the name as a
    // fontset.  If that doesn't work, use it as a font name.
    ಯದಿ (*p_guifontset != NUL
#  ifdef FONTSET_ALWAYS
	|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
	// In Motif, the Tooltip highlight group is always a fontset
	|| do_tooltip
#  endif
	    )
    {
	ಯದಿ (free_font)
	    gui_mch_free_fontset(HL_TABLE()[idx].sg_fontset);
	HL_TABLE()[idx].sg_fontset = fontset_name2handle(arg, 0
#  ifdef FONTSET_ALWAYS
		|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
		|| do_tooltip
#  endif
		);
    }
    ಯದಿ (HL_TABLE()[idx].sg_fontset != NOFONTSET)
    {
	// If it worked and it's the Normal group, use it as the normal
	// fontset.  Same for the Menu group.
	ಯದಿ (do_normal)
	    gui_init_font(arg, TRUE);
#  ಯದಿ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	ಯದಿ (do_menu)
	{
#   ifdef FONTSET_ALWAYS
	    gui.menu_fontset = HL_TABLE()[idx].sg_fontset;
#   ಅನ್ಯಥಾ
	    // YIKES!  This is a bug waiting to crash the program
	    gui.menu_font = HL_TABLE()[idx].sg_fontset;
#   endif
	    gui_mch_new_menu_font();
	}
#   ifdef FEAT_BEVAL_GUI
	ಯದಿ (do_tooltip)
	{
	    // The Athena widget set could not handle switching between
	    // displaying a single font and a fontset.
	    // If the XtNinternational resource is set to True at widget
	    // creation, then a fontset is always used, otherwise an
	    // XFontStruct is used.
	    gui.tooltip_fontset = (XFontSet)HL_TABLE()[idx].sg_fontset;
	    gui_mch_new_tooltip_font();
	}
#   endif
#  endif
    }
    ಅನ್ಯಥಾ
# endif
    {
	ಯದಿ (free_font)
	    gui_mch_free_font(HL_TABLE()[idx].sg_font);
	HL_TABLE()[idx].sg_font = font_name2handle(arg);
	// If it worked and it's the Normal group, use it as the
	// normal font.  Same for the Menu group.
	ಯದಿ (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    ಯದಿ (do_normal)
		gui_init_font(arg, FALSE);
# ifndef FONTSET_ALWAYS
#  ಯದಿ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	    ಯದಿ (do_menu)
	    {
		gui.menu_font = HL_TABLE()[idx].sg_font;
		gui_mch_new_menu_font();
	    }
#  endif
# endif
	}
    }
}

#endif // FEAT_GUI

#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return the handle for a color name.
 * Returns INVALCOLOR when failed.
 */
    guicolor_T
color_name2handle(char_u *name)
{
    ಯದಿ (STRCMP(name, "NONE") == 0)
	ಪ್ರತ್ಯಾಗಮ INVALCOLOR;

    ಯದಿ (STRICMP(name, "fg") == 0 || STRICMP(name, "foreground") == 0)
    {
# ಯದಿ defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	ಯದಿ (gui.in_use)
# endif
# ifdef FEAT_GUI
	    ಪ್ರತ್ಯಾಗಮ gui.norm_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	ಯದಿ (cterm_normal_fg_gui_color != INVALCOLOR)
	    ಪ್ರತ್ಯಾಗಮ cterm_normal_fg_gui_color;
	// Guess that the foreground is black or white.
	ಪ್ರತ್ಯಾಗಮ GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "black" : "white"));
# endif
    }
    ಯದಿ (STRICMP(name, "bg") == 0 || STRICMP(name, "background") == 0)
    {
# ಯದಿ defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	ಯದಿ (gui.in_use)
# endif
# ifdef FEAT_GUI
	    ಪ್ರತ್ಯಾಗಮ gui.back_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	ಯದಿ (cterm_normal_bg_gui_color != INVALCOLOR)
	    ಪ್ರತ್ಯಾಗಮ cterm_normal_bg_gui_color;
	// Guess that the background is white or black.
	ಪ್ರತ್ಯಾಗಮ GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "white" : "black"));
# endif
    }

    ಪ್ರತ್ಯಾಗಮ GUI_GET_COLOR(name);
}

// On MS-Windows an RGB macro is available and it produces 0x00bbggrr color
// values as used by the MS-Windows GDI api.  It should be used only for
// MS-Windows GDI builds.
# ಯದಿ defined(RGB) && defined(MSWIN) && !defined(FEAT_GUI)
#  undef RGB
# endif
# ifndef RGB
#  define RGB(r, g, b)	(((r)<<16) | ((g)<<8) | (b))
# endif

# ifdef VIMDLL
    ಸ್ಥಿರ guicolor_T
gui_adjust_rgb(guicolor_T c)
{
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ c;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ ((c & 0xff) << 16) | (c & 0x00ff00) | ((c >> 16) & 0xff);
}
# ಅನ್ಯಥಾ
#  define gui_adjust_rgb(c) (c)
# endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
hex_digit(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (SAFE_isdigit(c))
	ಪ್ರತ್ಯಾಗಮ c - '0';
    c = TOLOWER_ASC(c);
    ಯದಿ (c >= 'a' && c <= 'f')
	ಪ್ರತ್ಯಾಗಮ c - 'a' + 10;
    ಪ್ರತ್ಯಾಗಮ 0x1ffffff;
}

    ಸ್ಥಿರ guicolor_T
decode_hex_color(char_u *hex)
{
    guicolor_T color;

    ಯದಿ (hex[0] != '#' || STRLEN(hex) != 7)
	ಪ್ರತ್ಯಾಗಮ INVALCOLOR;

    // Name is in "#rrggbb" format
    color = RGB(((hex_digit(hex[1]) << 4) + hex_digit(hex[2])),
		((hex_digit(hex[3]) << 4) + hex_digit(hex[4])),
		((hex_digit(hex[5]) << 4) + hex_digit(hex[6])));
    ಯದಿ (color > 0xffffff)
	ಪ್ರತ್ಯಾಗಮ INVALCOLOR;
    ಪ್ರತ್ಯಾಗಮ gui_adjust_rgb(color);
}

# ifdef FEAT_EVAL
// Returns the color currently mapped to the given name or INVALCOLOR if no
// such name exists in the color table. The convention is to use lowercase for
// all keys in the v:colornames dictionary. The value can be either a string in
// the form #rrggbb or a number, either of which is converted to a guicolor_T.
    ಸ್ಥಿರ guicolor_T
colorname2rgb(char_u *name)
{
    dict_T      *colornames_table = get_vim_var_dict(VV_COLORNAMES);
    char_u      *lc_name;
    dictitem_T  *colentry;
    char_u      *colstr;
    varnumber_T colnum;

    lc_name = strlow_save(name);
    ಯದಿ (lc_name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ INVALCOLOR;

    colentry = dict_find(colornames_table, lc_name, -1);
    vim_free(lc_name);
    ಯದಿ (colentry == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ INVALCOLOR;

    ಯದಿ (colentry->di_tv.v_type == VAR_STRING)
    {
	colstr = tv_get_string_strict(&colentry->di_tv);
	ಯದಿ ((STRLEN(colstr) == 7) && (*colstr == '#'))
	{
	    ಪ್ರತ್ಯಾಗಮ decode_hex_color(colstr);
	}
	ಅನ್ಯಥಾ
	{
	    semsg(_(e_bad_color_string_str), colstr);
	    ಪ್ರತ್ಯಾಗಮ INVALCOLOR;
	}
    }

    ಯದಿ (colentry->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
    {
	colnum = tv_get_ಸಂಖ್ಯೆ(&colentry->di_tv);
	ಪ್ರತ್ಯಾಗಮ (guicolor_T)colnum;
    }

    ಪ್ರತ್ಯಾಗಮ INVALCOLOR;
}

# endif

    guicolor_T
gui_get_color_cmn(char_u *name)
{
    guicolor_T  color;
    // Only non X11 colors (not present in rgb.txt) and colors in
    // color_name_tab[], useful when $VIMRUNTIME is not found,.
    // must be sorted by the 'value' field because it is used by bsearch()!
    ಸ್ಥಿರ keyvalue_T rgb_tab[] = {
	KEYVALUE_ENTRY(RGB(0x00, 0x00, 0x00), "black"),
	KEYVALUE_ENTRY(RGB(0x00, 0x00, 0xFF), "blue"),
	KEYVALUE_ENTRY(RGB(0xA5, 0x2A, 0x2A), "brown"),
	KEYVALUE_ENTRY(RGB(0x00, 0xFF, 0xFF), "cyan"),
	KEYVALUE_ENTRY(RGB(0x00, 0x00, 0x8B), "darkblue"),
	KEYVALUE_ENTRY(RGB(0x00, 0x8B, 0x8B), "darkcyan"),
	KEYVALUE_ENTRY(RGB(0xA9, 0xA9, 0xA9), "darkgray"),
	KEYVALUE_ENTRY(RGB(0x00, 0x64, 0x00), "darkgreen"),
	KEYVALUE_ENTRY(RGB(0xA9, 0xA9, 0xA9), "darkgrey"),
	KEYVALUE_ENTRY(RGB(0x8B, 0x00, 0x8B), "darkmagenta"),
	KEYVALUE_ENTRY(RGB(0x8B, 0x00, 0x00), "darkred"),
	KEYVALUE_ENTRY(RGB(0x8B, 0x8B, 0x00), "darkyellow"),	 // No X11
	KEYVALUE_ENTRY(RGB(0xBE, 0xBE, 0xBE), "gray"),
	KEYVALUE_ENTRY(RGB(0x00, 0xFF, 0x00), "green"),
	KEYVALUE_ENTRY(RGB(0xBE, 0xBE, 0xBE), "grey"),
	KEYVALUE_ENTRY(RGB(0x66, 0x66, 0x66), "grey40"),
	KEYVALUE_ENTRY(RGB(0x7F, 0x7F, 0x7F), "grey50"),
	KEYVALUE_ENTRY(RGB(0xE5, 0xE5, 0xE5), "grey90"),
	KEYVALUE_ENTRY(RGB(0xAD, 0xD8, 0xE6), "lightblue"),
	KEYVALUE_ENTRY(RGB(0xE0, 0xFF, 0xFF), "lightcyan"),
	KEYVALUE_ENTRY(RGB(0xD3, 0xD3, 0xD3), "lightgray"),
	KEYVALUE_ENTRY(RGB(0x90, 0xEE, 0x90), "lightgreen"),
	KEYVALUE_ENTRY(RGB(0xD3, 0xD3, 0xD3), "lightgrey"),
	KEYVALUE_ENTRY(RGB(0xFF, 0x8B, 0xFF), "lightmagenta"),     // No XX
	KEYVALUE_ENTRY(RGB(0xFF, 0x8B, 0x8B), "lightred"),	 // No XX
	KEYVALUE_ENTRY(RGB(0xFF, 0xFF, 0xE0), "lightyellow"),
	KEYVALUE_ENTRY(RGB(0xFF, 0x00, 0xFF), "magenta"),
	KEYVALUE_ENTRY(RGB(0xFF, 0x00, 0x00), "red"),
	KEYVALUE_ENTRY(RGB(0x2E, 0x8B, 0x57), "seagreen"),
	KEYVALUE_ENTRY(RGB(0xFF, 0xFF, 0xFF), "white"),
	KEYVALUE_ENTRY(RGB(0xFF, 0xFF, 0x00), "yellow")
    };
    keyvalue_T target;
    keyvalue_T *entry;

    color = decode_hex_color(name);
    ಯದಿ (color != INVALCOLOR)
	ಪ್ರತ್ಯಾಗಮ color;

    target.ಕೀಲಿ = 0;
    target.ಮೌಲ್ಯ.string = name;
    target.ಮೌಲ್ಯ.length = 0;	// not used, see cmp_keyvalue_value_i()
    entry = (keyvalue_T *)bsearch(&target, &rgb_tab, ARRAY_LENGTH(rgb_tab),
	    ಪರಿಮಾಣ(rgb_tab[0]), cmp_keyvalue_ಮೌಲ್ಯ_i);
    ಯದಿ (entry != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ gui_adjust_rgb((guicolor_T)entry->ಕೀಲಿ);

# ಯದಿ defined(FEAT_EVAL)
    /*
     * Not a traditional color. Load additional color aliases and then consult the alias table.
     */

    color = colorname2rgb(name);
    ಯದಿ (color == INVALCOLOR)
    {
	load_default_colors_lists();
	color = colorname2rgb(name);
    }

    ಪ್ರತ್ಯಾಗಮ color;
# ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ INVALCOLOR;
# endif
}

    guicolor_T
gui_get_rgb_color_cmn(ಪೂರ್ಣಾಙ್ಕ r, ಪೂರ್ಣಾಙ್ಕ g, ಪೂರ್ಣಾಙ್ಕ b)
{
    guicolor_T  color = RGB(r, g, b);

    ಯದಿ (color > 0xffffff)
	ಪ್ರತ್ಯಾಗಮ INVALCOLOR;
    ಪ್ರತ್ಯಾಗಮ gui_adjust_rgb(color);
}
#endif

/*
 * Table with the specifications for an attribute number.
 * Note that this table is used by ALL buffers.  This is required because the
 * GUI can redraw at any time for any buffer.
 */
ಸ್ಥಿರ garray_T	term_attr_table = {0, 0, 0, 0, ರಿಕ್ತ};

#define TERM_ATTR_ENTRY(idx) ((attrentry_T *)term_attr_table.ga_data)[idx]

ಸ್ಥಿರ garray_T	cterm_attr_table = {0, 0, 0, 0, ರಿಕ್ತ};

#define CTERM_ATTR_ENTRY(idx) ((attrentry_T *)cterm_attr_table.ga_data)[idx]

#ifdef FEAT_GUI
ಸ್ಥಿರ garray_T	gui_attr_table = {0, 0, 0, 0, ರಿಕ್ತ};

# define GUI_ATTR_ENTRY(idx) ((attrentry_T *)gui_attr_table.ga_data)[idx]
#endif

/*
 * Return the attr number for a set of colors and font.
 * Add a new entry to the term_attr_table, cterm_attr_table or gui_attr_table
 * if the combination is new.
 * Return 0 for error (no more room).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_attr_entry(garray_T *table, attrentry_T *aep)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    attrentry_T	*taep;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = FALSE;

    // Init the table, in case it wasn't done yet.
    table->ga_itemsize = ಪರಿಮಾಣ(attrentry_T);
    table->ga_growsize = 7;

    // Try to find an entry with the same specifications.
    ಪ್ರತಿ (i = 0; i < table->ga_len; ++i)
    {
	taep = &(((attrentry_T *)table->ga_data)[i]);
	ಯದಿ (	   aep->ae_attr == taep->ae_attr
		&& (
#ifdef FEAT_GUI
		       (table == &gui_attr_table
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
#endif
		       (table == &term_attr_table
			&& (aep->ae_u.term.start == ರಿಕ್ತ)
					    == (taep->ae_u.term.start == ರಿಕ್ತ)
			&& (aep->ae_u.term.start == ರಿಕ್ತ
			    || STRCMP(aep->ae_u.term.start,
						  taep->ae_u.term.start) == 0)
			&& (aep->ae_u.term.stop == ರಿಕ್ತ)
					     == (taep->ae_u.term.stop == ರಿಕ್ತ)
			&& (aep->ae_u.term.stop == ರಿಕ್ತ
			    || STRCMP(aep->ae_u.term.stop,
						  taep->ae_u.term.stop) == 0))
		    || (table == &cterm_attr_table
			    && aep->ae_u.cterm.fg_color
						  == taep->ae_u.cterm.fg_color
			    && aep->ae_u.cterm.bg_color
						  == taep->ae_u.cterm.bg_color
			    && aep->ae_u.cterm.ul_color
						  == taep->ae_u.cterm.ul_color
			    && aep->ae_u.cterm.font
						  == taep->ae_u.cterm.font
#ifdef FEAT_TERMGUICOLORS
			    && aep->ae_u.cterm.fg_rgb
						    == taep->ae_u.cterm.fg_rgb
			    && aep->ae_u.cterm.bg_rgb
						    == taep->ae_u.cterm.bg_rgb
			    && aep->ae_u.cterm.ul_rgb
						    == taep->ae_u.cterm.ul_rgb
#endif
		       )))

	ಪ್ರತ್ಯಾಗಮ i + ATTR_OFF;
    }

    ಯದಿ (table->ga_len + ATTR_OFF > MAX_TYPENR)
    {
	// Running out of attribute entries!  remove all attributes, and
	// compute new ones for all groups.
	// When called recursively, we are really out of numbers.
	ಯದಿ (recursive)
	{
	    emsg(_(e_too_many_different_highlighting_attributes_in_use));
	    ಪ್ರತ್ಯಾಗಮ 0;
	}
	recursive = TRUE;

	clear_hl_tables();

	set_must_redraw(UPD_CLEAR);

	ಪ್ರತಿ (i = 0; i < highlight_ga.ga_len; ++i)
	    set_hl_attr(i);

	recursive = FALSE;
    }

    // This is a new combination of colors and font, add an entry.
    ಯದಿ (ga_grow(table, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ 0;

    taep = &(((attrentry_T *)table->ga_data)[table->ga_len]);
    CLEAR_POINTER(taep);
    taep->ae_attr = aep->ae_attr;
#ifdef FEAT_GUI
    ಯದಿ (table == &gui_attr_table)
    {
	taep->ae_u.gui.fg_color = aep->ae_u.gui.fg_color;
	taep->ae_u.gui.bg_color = aep->ae_u.gui.bg_color;
	taep->ae_u.gui.sp_color = aep->ae_u.gui.sp_color;
	taep->ae_u.gui.font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
	taep->ae_u.gui.fontset = aep->ae_u.gui.fontset;
# endif
    }
#endif
    ಯದಿ (table == &term_attr_table)
    {
	ಯದಿ (aep->ae_u.term.start == ರಿಕ್ತ)
	    taep->ae_u.term.start = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	    taep->ae_u.term.start = vim_strsave(aep->ae_u.term.start);
	ಯದಿ (aep->ae_u.term.stop == ರಿಕ್ತ)
	    taep->ae_u.term.stop = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	    taep->ae_u.term.stop = vim_strsave(aep->ae_u.term.stop);
    }
    ಅನ್ಯಥಾ ಯದಿ (table == &cterm_attr_table)
    {
	taep->ae_u.cterm.fg_color = aep->ae_u.cterm.fg_color;
	taep->ae_u.cterm.bg_color = aep->ae_u.cterm.bg_color;
	taep->ae_u.cterm.ul_color = aep->ae_u.cterm.ul_color;
	taep->ae_u.cterm.font = aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
	taep->ae_u.cterm.fg_rgb = aep->ae_u.cterm.fg_rgb;
	taep->ae_u.cterm.bg_rgb = aep->ae_u.cterm.bg_rgb;
	taep->ae_u.cterm.ul_rgb = aep->ae_u.cterm.ul_rgb;
#endif
    }
    ++table->ga_len;
    ಪ್ರತ್ಯಾಗಮ (table->ga_len - 1 + ATTR_OFF);
}

#ಯದಿ defined(FEAT_TERMINAL)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_cterm_attr_idx(ಪೂರ್ಣಾಙ್ಕ attr, ಪೂರ್ಣಾಙ್ಕ fg, ಪೂರ್ಣಾಙ್ಕ bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
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
    ಪ್ರತ್ಯಾಗಮ get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#ಯದಿ defined(FEAT_TERMINAL) && defined(FEAT_TERMGUICOLORS)
/*
 * Get an attribute index for a 'termguicolors' entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_tgc_attr_idx(ಪೂರ್ಣಾಙ್ಕ attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    ಯದಿ (fg == INVALCOLOR && bg == INVALCOLOR)
    {
	// If both GUI colors are not set fall back to the cterm colors.  Helps
	// if the GUI only has an attribute, such as undercurl.
	at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
    }
    ಅನ್ಯಥಾ
    {
	at_en.ae_u.cterm.fg_rgb = fg;
	at_en.ae_u.cterm.bg_rgb = bg;
    }
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
    ಪ್ರತ್ಯಾಗಮ get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#ಯದಿ defined(FEAT_TERMINAL) && defined(FEAT_GUI)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_gui_attr_idx(ಪೂರ್ಣಾಙ್ಕ attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    at_en.ae_u.gui.fg_color = fg;
    at_en.ae_u.gui.bg_color = bg;
    ಪ್ರತ್ಯಾಗಮ get_attr_entry(&gui_attr_table, &at_en);
}
#endif

/*
 * Clear all highlight tables.
 */
    ಶೂನ್ಯ
clear_hl_tables(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    attrentry_T	*taep;

#ifdef FEAT_GUI
    ga_clear(&gui_attr_table);
#endif
    ಪ್ರತಿ (i = 0; i < term_attr_table.ga_len; ++i)
    {
	taep = &(((attrentry_T *)term_attr_table.ga_data)[i]);
	vim_free(taep->ae_u.term.start);
	vim_free(taep->ae_u.term.stop);
    }
    ga_clear(&term_attr_table);
    ga_clear(&cterm_attr_table);
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
    ಪೂರ್ಣಾಙ್ಕ
hl_combine_attr(ಪೂರ್ಣಾಙ್ಕ char_attr, ಪೂರ್ಣಾಙ್ಕ prim_attr)
{
    attrentry_T *char_aep = ರಿಕ್ತ;
    attrentry_T *prim_aep;
    attrentry_T new_en;

    ಯದಿ (char_attr == 0)
	ಪ್ರತ್ಯಾಗಮ prim_attr;
    ಯದಿ (char_attr <= HL_ALL && prim_attr <= HL_ALL)
	ಪ್ರತ್ಯಾಗಮ ATTR_COMBINE(char_attr, prim_attr);
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
    {
	ಯದಿ (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	ಯದಿ (char_aep != ರಿಕ್ತ)
	    new_en = *char_aep;
	ಅನ್ಯಥಾ
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    ಯದಿ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	ಯದಿ (prim_attr <= HL_ALL)
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	ಅನ್ಯಥಾ
	{
	    prim_aep = syn_gui_attr2entry(prim_attr);
	    ಯದಿ (prim_aep != ರಿಕ್ತ)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		ಯದಿ (prim_aep->ae_u.gui.fg_color != INVALCOLOR)
		    new_en.ae_u.gui.fg_color = prim_aep->ae_u.gui.fg_color;
		ಯದಿ (prim_aep->ae_u.gui.bg_color != INVALCOLOR)
		    new_en.ae_u.gui.bg_color = prim_aep->ae_u.gui.bg_color;
		ಯದಿ (prim_aep->ae_u.gui.sp_color != INVALCOLOR)
		    new_en.ae_u.gui.sp_color = prim_aep->ae_u.gui.sp_color;
		ಯದಿ (prim_aep->ae_u.gui.font != NOFONT)
		    new_en.ae_u.gui.font = prim_aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
		ಯದಿ (prim_aep->ae_u.gui.fontset != NOFONTSET)
		    new_en.ae_u.gui.fontset = prim_aep->ae_u.gui.fontset;
# endif
	    }
	}
	ಪ್ರತ್ಯಾಗಮ get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    ಯದಿ (IS_CTERM)
    {
	ಯದಿ (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	ಯದಿ (char_aep != ರಿಕ್ತ)
	    new_en = *char_aep;
	ಅನ್ಯಥಾ
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    ಯದಿ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	ಯದಿ (prim_attr <= HL_ALL)
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	ಅನ್ಯಥಾ
	{
	    prim_aep = syn_cterm_attr2entry(prim_attr);
	    ಯದಿ (prim_aep != ರಿಕ್ತ)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		ಯದಿ (prim_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color = prim_aep->ae_u.cterm.fg_color;
		ಯದಿ (prim_aep->ae_u.cterm.bg_color > 0)
		    new_en.ae_u.cterm.bg_color = prim_aep->ae_u.cterm.bg_color;
		ಯದಿ (prim_aep->ae_u.cterm.ul_color > 0)
		    new_en.ae_u.cterm.ul_color = prim_aep->ae_u.cterm.ul_color;
		ಯದಿ (prim_aep->ae_u.cterm.font > 0)
		    new_en.ae_u.cterm.font = prim_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		// If both fg and bg are not set fall back to cterm colors.
		// Helps for SpellBad which uses undercurl in the GUI.
		ಯದಿ (COLOR_INVALID(prim_aep->ae_u.cterm.fg_rgb)
			&& COLOR_INVALID(prim_aep->ae_u.cterm.bg_rgb))
		{
		    ಯದಿ (prim_aep->ae_u.cterm.fg_color > 0)
			new_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
		    ಯದಿ (prim_aep->ae_u.cterm.bg_color > 0)
			new_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (prim_aep->ae_u.cterm.fg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.fg_rgb = prim_aep->ae_u.cterm.fg_rgb;
		    ಯದಿ (prim_aep->ae_u.cterm.bg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.bg_rgb = prim_aep->ae_u.cterm.bg_rgb;
		}
		ಯದಿ (prim_aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		    new_en.ae_u.cterm.ul_rgb = prim_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	}
	ಪ್ರತ್ಯಾಗಮ get_attr_entry(&cterm_attr_table, &new_en);
    }

    ಯದಿ (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    ಯದಿ (char_aep != ರಿಕ್ತ)
	new_en = *char_aep;
    ಅನ್ಯಥಾ
    {
	CLEAR_FIELD(new_en);
	ಯದಿ (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    ಯದಿ (prim_attr <= HL_ALL)
	new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
    ಅನ್ಯಥಾ
    {
	prim_aep = syn_term_attr2entry(prim_attr);
	ಯದಿ (prim_aep != ರಿಕ್ತ)
	{
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_aep->ae_attr);
	    ಯದಿ (prim_aep->ae_u.term.start != ರಿಕ್ತ)
	    {
		new_en.ae_u.term.start = prim_aep->ae_u.term.start;
		new_en.ae_u.term.stop = prim_aep->ae_u.term.stop;
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ get_attr_entry(&term_attr_table, &new_en);
}

// ANSI color order: Black, Red, Green, Yellow, Blue, Magenta,
// Cyan, White, then bright variants.  Approximate RGB values used when
// only a cterm color number is known (no guifg/guibg).  Real terminal
// palettes may differ if the user reconfigured their emulator, but
// these are reasonable xterm-ish defaults.
ಸ್ಥಿರ ಧ್ರುವ long_u cterm_color_16[16] = {
    0x000000, 0xc00000, 0x008000, 0x808000,
    0x0000c0, 0xc000c0, 0x004080, 0xc0c0c0,
    0x808080, 0xff8080, 0x00ff00, 0xffff00,
    0x6060ff, 0xff40ff, 0x00ffff, 0xffffff
};

#ifdef FEAT_TERMGUICOLORS
/*
 * Convert a cterm color number (1-16) to an RGB value.
 * Used as a fallback when 'termguicolors' is set but only cterm colors are
 * specified (no guifg/guibg).
 * Returns INVALCOLOR if the color number is out of range.
 */
    ಸ್ಥಿರ guicolor_T
cterm_color_to_rgb(ಪೂರ್ಣಾಙ್ಕ color_nr)
{
    ಯದಿ (color_nr < 1 || color_nr > 16)
	ಪ್ರತ್ಯಾಗಮ INVALCOLOR;
    ಪ್ರತ್ಯಾಗಮ (guicolor_T)cterm_color_16[color_nr - 1];
}
#endif

/*
 * Convert an xterm 256-color index (0-255) to an approximate RGB triple.
 * Uses the standard xterm palette: 0-15 ANSI (cterm_color_16), 16-231
 * 6x6x6 cube, 232-255 grayscale ramp.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cterm_idx_to_rgb(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ *r, ಪೂರ್ಣಾಙ್ಕ *g, ಪೂರ್ಣಾಙ್ಕ *b)
{
    ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

    ಯದಿ (idx < 0 || idx > 255)
    {
	*r = *g = *b = 0;
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (idx < 16)
    {
	long_u rgb = cterm_color_16[idx];
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
    }
    ಅನ್ಯಥಾ ಯದಿ (idx < 232)
    {
	ಪೂರ್ಣಾಙ್ಕ n = idx - 16;
	*r = cube[n / 36 % 6];
	*g = cube[n / 6  % 6];
	*b = cube[n      % 6];
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ v = 8 + (idx - 232) * 10;
	*r = *g = *b = v;
    }
}

/*
 * Approximate an RGB triple to the nearest xterm 256-color index.
 * Searches the 6x6x6 cube and the grayscale ramp; returns 0-255.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
rgb_to_cterm_idx(ಪೂರ್ಣಾಙ್ಕ r, ಪೂರ್ಣಾಙ್ಕ g, ಪೂರ್ಣಾಙ್ಕ b)
{
    ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };
    ಪೂರ್ಣಾಙ್ಕ best = 0;
    long best_d = -1;
    ಪೂರ್ಣಾಙ್ಕ i;

    // Search the 6x6x6 cube.
    ಪ್ರತಿ (i = 16; i < 232; ++i)
    {
	ಪೂರ್ಣಾಙ್ಕ n = i - 16;
	ಪೂರ್ಣಾಙ್ಕ cr = cube[n / 36 % 6];
	ಪೂರ್ಣಾಙ್ಕ cg = cube[n / 6  % 6];
	ಪೂರ್ಣಾಙ್ಕ cb = cube[n      % 6];
	long d = (cr - r) * (cr - r) + (cg - g) * (cg - g) + (cb - b) * (cb - b);
	ಯದಿ (best_d < 0 || d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    // Search the grayscale ramp.
    ಪ್ರತಿ (i = 232; i < 256; ++i)
    {
	ಪೂರ್ಣಾಙ್ಕ v = 8 + (i - 232) * 10;
	long d = (v - r) * (v - r) + (v - g) * (v - g) + (v - b) * (v - b);
	ಯದಿ (d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    ಪ್ರತ್ಯಾಗಮ best;
}

/*
 * Resolve a color to an RGB triple.  Prefer the gui RGB value (set by
 * guifg/guibg) when valid, otherwise convert the cterm 256-color index.
 * Returns true on success; false when no color is defined.
 * cterm_c is 1-based (0 means "no color").
 */
    ಸ್ಥಿರ bool
resolve_color_to_rgb(ಪೂರ್ಣಾಙ್ಕ cterm_c, guicolor_T rgb UNUSED, ಪೂರ್ಣಾಙ್ಕ *r, ಪೂರ್ಣಾಙ್ಕ *g, ಪೂರ್ಣಾಙ್ಕ *b)
{
#ifdef FEAT_TERMGUICOLORS
    ಯದಿ (!COLOR_INVALID(rgb))
    {
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
	ಪ್ರತ್ಯಾಗಮ true;
    }
#endif
    ಯದಿ (cterm_c > 0)
    {
	cterm_idx_to_rgb(cterm_c - 1, r, g, b);
	ಪ್ರತ್ಯಾಗಮ true;
    }
    ಪ್ರತ್ಯಾಗಮ false;
}

/*
 * get a RGB fallback color from gui, cterm or default color
 */
    ಸ್ಥಿರ guicolor_T
resolve_fallback_color(ಪೂರ್ಣಾಙ್ಕ cterm_c, guicolor_T rgb, guicolor_T default_rgb)
{
    ಪೂರ್ಣಾಙ್ಕ red, green, blue;
    ಯದಿ (!resolve_color_to_rgb(cterm_c, rgb, &red, &green, &blue))
	ಪ್ರತ್ಯಾಗಮ default_rgb;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ (red << 16) | (green << 8) | blue;
}

/*
 * get a RGB fallback foreground color from guifg, ctermfg or deduced from background
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
resolve_fallback_fg_to_rgb(ಶೂನ್ಯ)
{
    guicolor_T fgcolor_or_gui_fgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    fgcolor_or_gui_fgcolor = cterm_normal_fg_gui_color;
#endif
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	fgcolor_or_gui_fgcolor = gui.norm_pixel;
#endif
    fallback_fg_rgb = resolve_fallback_color(cterm_normal_fg_color, fgcolor_or_gui_fgcolor, (*p_bg == 'l') ? 0x000000 : 0xFFFFFF);
}

/*
 * get a RGB fallback background color from guifg, ctermbg or deduced from background
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
resolve_fallback_bg_to_rgb(ಶೂನ್ಯ)
{
    guicolor_T bgcolor_or_gui_bgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    bgcolor_or_gui_bgcolor = cterm_normal_bg_gui_color;
#endif
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	bgcolor_or_gui_bgcolor = gui.back_pixel;
#endif
    fallback_bg_rgb = resolve_fallback_color(cterm_normal_bg_color, bgcolor_or_gui_bgcolor, (*p_bg == 'l') ? 0xFFFFFF : 0x000000);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
blend_cterm_colors(ಪೂರ್ಣಾಙ್ಕ popup_c, guicolor_T popup_rgb,
		   ಪೂರ್ಣಾಙ್ಕ under_c, guicolor_T under_rgb,
		   ಪೂರ್ಣಾಙ್ಕ default_rgb,
		   ಪೂರ್ಣಾಙ್ಕ blend_val)
{
    ಪೂರ್ಣಾಙ್ಕ pr, pg, pb, ur, ug, ub, r, g, b;

    ಯದಿ (t_colors < 256)
	ಪ್ರತ್ಯಾಗಮ popup_c;
    ಯದಿ (!resolve_color_to_rgb(popup_c, popup_rgb, &pr, &pg, &pb))
	ಪ್ರತ್ಯಾಗಮ under_c;
    ಯದಿ (blend_val <= 0)
	ಪ್ರತ್ಯಾಗಮ rgb_to_cterm_idx(pr, pg, pb) + 1;
    ಯದಿ (!resolve_color_to_rgb(under_c, under_rgb, &ur, &ug, &ub))
    {
	ur = (default_rgb >> 16) & 0xFF;
	ug = (default_rgb >> 8) & 0xFF;
	ub = default_rgb & 0xFF;
    }
    ಯದಿ (blend_val >= 100)
	ಪ್ರತ್ಯಾಗಮ rgb_to_cterm_idx(ur, ug, ub) + 1;
    r = pr + (ur - pr) * blend_val / 100;
    g = pg + (ug - pg) * blend_val / 100;
    b = pb + (ub - pb) * blend_val / 100;
    ಪ್ರತ್ಯಾಗಮ rgb_to_cterm_idx(r, g, b) + 1;
}

#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Blend two RGB colors based on blend value (0-100).
 * blend: 0=return first argument color, 100=return second argument color
 * If bg_color is INVALCOLOR, high blend means more visible (return INVALCOLOR).
 */
    ಸ್ಥಿರ guicolor_T
blend_colors(guicolor_T popup_color, guicolor_T bg_color, ಪೂರ್ಣಾಙ್ಕ blend_val)
{
    ಪೂರ್ಣಾಙ್ಕ r1, g1, b1, r2, g2, b2, r, g, b;

    // CTERMCOLOR is a sentinel meaning "use the cterm color"; for blending
    // it has no real RGB so treat it like INVALCOLOR.
    ಯದಿ (COLOR_INVALID(popup_color))
	ಪ್ರತ್ಯಾಗಮ INVALCOLOR;

    // Fully transparent: use underlying color as-is.
    ಯದಿ (blend_val >= 100)
	ಪ್ರತ್ಯಾಗಮ bg_color;

    r1 = (popup_color >> 16) & 0xFF;
    g1 = (popup_color >> 8) & 0xFF;
    b1 = popup_color & 0xFF;

    ಯದಿ (COLOR_INVALID(bg_color))
	bg_color = fallback_bg_rgb;

    r2 = (bg_color >> 16) & 0xFF;
    g2 = (bg_color >> 8) & 0xFF;
    b2 = bg_color & 0xFF;

    r = r1 + (r2 - r1) * blend_val / 100;
    g = g1 + (g2 - g1) * blend_val / 100;
    b = b1 + (b2 - b1) * blend_val / 100;

    ಪ್ರತ್ಯಾಗಮ (r << 16) | (g << 8) | b;
}
#endif

/*
 * Blend attributes for popup windows and menus with opacity.
 * Blends foreground and/or background colors based on blend value (0-100).
 * blend: 0 = opaque (use popup colors), 100 = transparent (use background colors)
 * blend_fg: TRUE to blend foreground color, FALSE to keep popup foreground
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
hl_blend_attr_common(
    ಪೂರ್ಣಾಙ್ಕ		char_attr,
    ಪೂರ್ಣಾಙ್ಕ		popup_attr,
    ಪೂರ್ಣಾಙ್ಕ		blend,
    ಪೂರ್ಣಾಙ್ಕ		blend_fg)
{
    attrentry_T *char_aep = ರಿಕ್ತ;
    attrentry_T *popup_aep;
    attrentry_T new_en;
    attrentry_T tmp_en;

#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
    {
	ಯದಿ (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	ಯದಿ (char_aep != ರಿಕ್ತ)
	    new_en = *char_aep;
	ಅನ್ಯಥಾ
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    ಯದಿ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	ಯದಿ (popup_attr <= HL_ALL)
	{
	    CLEAR_FIELD(tmp_en);
	    tmp_en.ae_u.gui.fg_color = INVALCOLOR;
	    tmp_en.ae_u.gui.bg_color = INVALCOLOR;
	    tmp_en.ae_u.gui.sp_color = INVALCOLOR;
	    // preserve attributes other than color
	    tmp_en.ae_attr = popup_attr;
	    popup_aep = &tmp_en;

	    popup_aep->ae_u.gui.bg_color = fallback_bg_rgb;
	}
	ಅನ್ಯಥಾ
	    popup_aep = syn_gui_attr2entry(popup_attr);

	ಯದಿ (popup_aep != ರಿಕ್ತ)
	{
	    guicolor_T popup_bg_rgb = popup_aep->ae_u.gui.bg_color;
	    ಯದಿ (COLOR_INVALID(popup_bg_rgb))
		popup_bg_rgb = fallback_bg_rgb;

	    ಯದಿ (blend_fg)
	    {
		// blend_fg=TRUE: fade underlying text toward popup bg.
		{
		    ಪೂರ್ಣಾಙ್ಕ base_fg = fallback_fg_rgb;
		    ಯದಿ (char_aep != ರಿಕ್ತ
			    && char_aep->ae_u.gui.fg_color != INVALCOLOR)
			base_fg = char_aep->ae_u.gui.fg_color;
		    new_en.ae_u.gui.fg_color = blend_colors(
			    popup_bg_rgb, base_fg, blend);
		}
	    }
	    ಅನ್ಯಥಾ
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
		ಯದಿ (char_aep != ರಿಕ್ತ
			&& !COLOR_INVALID(char_aep->ae_u.gui.bg_color))
		    underlying_bg = char_aep->ae_u.gui.bg_color;
		new_en.ae_u.gui.bg_color = blend_colors(
			popup_bg_rgb,
			underlying_bg, blend);
	    }
	}
	ಪ್ರತ್ಯಾಗಮ get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    ಯದಿ (IS_CTERM)
    {
	ಯದಿ (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	ಯದಿ (char_aep != ರಿಕ್ತ)
	    new_en = *char_aep;
	ಅನ್ಯಥಾ
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    ಯದಿ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	ಯದಿ (popup_attr <= HL_ALL)
	{
	    CLEAR_FIELD(tmp_en);
#ifdef FEAT_TERMGUICOLORS
	    tmp_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    tmp_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	    // allow blending with termguicolors
	    tmp_en.ae_u.cterm.bg_rgb = fallback_bg_rgb;
#endif
	    // preserve attributes other than color
	    tmp_en.ae_attr = popup_attr;
	    popup_aep = &tmp_en;

	    // allow blending with notermguicolors
	    popup_aep->ae_u.cterm.bg_color = cterm_normal_bg_color;
	}
	ಅನ್ಯಥಾ
	    popup_aep = syn_cterm_attr2entry(popup_attr);

	ಯದಿ (popup_aep != ರಿಕ್ತ)
	{
	    guicolor_T popup_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
	    // Fall back to cterm color converted to RGB when gui color is not set.
	    popup_bg_rgb = popup_aep->ae_u.cterm.bg_rgb;
	    ಯದಿ (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color > 0)
		popup_bg_rgb = cterm_color_to_rgb(
			popup_aep->ae_u.cterm.bg_color);
#endif
	    // assign default color if guibg and ctermbg are not set for popup
	    ಯದಿ (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color == 0)
		popup_bg_rgb = fallback_bg_rgb;

	    ಯದಿ (!blend_fg)
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
		ಯದಿ (popup_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color =
			popup_aep->ae_u.cterm.fg_color;
		ಅನ್ಯಥಾ ಯದಿ (cterm_normal_fg_color > 0)
		    new_en.ae_u.cterm.fg_color = cterm_normal_fg_color;
		ಅನ್ಯಥಾ
		    // black-ish or white-ish
		    new_en.ae_u.cterm.fg_color = (*p_bg == 'l') ? 1 : 16;
		new_en.ae_u.cterm.ul_color = popup_aep->ae_u.cterm.ul_color;
		new_en.ae_u.cterm.font = popup_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		new_en.ae_u.cterm.ul_rgb = popup_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	    ಅನ್ಯಥಾ
	    {
		// blend_fg=TRUE: fade underlying fg toward popup bg in
		// the 256-color palette.  Used when the popup is over a
		// cell rendered with cterm colors (no termguicolors RGB).
		ಪೂರ್ಣಾಙ್ಕ under_fg = (char_aep != ರಿಕ್ತ)
		    ? char_aep->ae_u.cterm.fg_color : 0;
		guicolor_T under_fg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		ಯದಿ (char_aep != ರಿಕ್ತ)
		    under_fg_rgb = char_aep->ae_u.cterm.fg_rgb;
#endif
		new_en.ae_u.cterm.fg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_fg, under_fg_rgb, fallback_fg_rgb, blend);
	    }
	    // Approximate cterm bg by blending with the underlying bg
	    // in the 256-color palette and mapping to the nearest entry.
	    {
		ಪೂರ್ಣಾಙ್ಕ under_bg = (char_aep != ರಿಕ್ತ)
		    ? char_aep->ae_u.cterm.bg_color : 0;
		guicolor_T under_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		ಯದಿ (char_aep != ರಿಕ್ತ)
		    under_bg_rgb = char_aep->ae_u.cterm.bg_rgb;
#endif
		new_en.ae_u.cterm.bg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_bg, under_bg_rgb, fallback_bg_rgb, blend);
	    }
#ifdef FEAT_TERMGUICOLORS
	    // Blend RGB colors for termguicolors mode.
	    {
		ಯದಿ (blend_fg)
		{
		    // blend_fg=TRUE: fade underlying text toward popup bg.
		    ಯದಿ (popup_bg_rgb != INVALCOLOR)
		    {
			ಪೂರ್ಣಾಙ್ಕ base_fg = fallback_fg_rgb;
			// CTERMCOLOR is a sentinel meaning "use the cterm
			// color"; treat it as no underlying color so it is
			// not blended in as a real near-white pixel.
			ಯದಿ (char_aep != ರಿಕ್ತ
				&& !COLOR_INVALID(char_aep->ae_u.cterm.fg_rgb))
			    base_fg = char_aep->ae_u.cterm.fg_rgb;
			new_en.ae_u.cterm.fg_rgb = blend_colors(
				popup_bg_rgb, base_fg, blend);
		    }
		}
		ಅನ್ಯಥಾ
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

		    ಯದಿ (COLOR_INVALID(popup_fg)
			    && popup_aep->ae_u.cterm.fg_color > 0)
			popup_fg = cterm_color_to_rgb(
				popup_aep->ae_u.cterm.fg_color);

		    ಯದಿ (!COLOR_INVALID(popup_fg))
			new_en.ae_u.cterm.fg_rgb = popup_fg;
		    ಅನ್ಯಥಾ ಯದಿ (!COLOR_INVALID(cterm_normal_fg_gui_color))
			new_en.ae_u.cterm.fg_rgb = cterm_normal_fg_gui_color;
		    ಅನ್ಯಥಾ
			new_en.ae_u.cterm.fg_rgb = fallback_fg_rgb;
		}
		ಯದಿ (popup_bg_rgb != INVALCOLOR)
		{
		    // Blend popup bg toward underlying bg
		    guicolor_T underlying_bg = fallback_bg_rgb;
		    ಯದಿ (char_aep != ರಿಕ್ತ
			    && !COLOR_INVALID(char_aep->ae_u.cterm.bg_rgb))
			underlying_bg = char_aep->ae_u.cterm.bg_rgb;
		    new_en.ae_u.cterm.bg_rgb = blend_colors(
			    popup_bg_rgb, underlying_bg, blend);
		}
	    }
#endif
	}
	ಪ್ರತ್ಯಾಗಮ get_attr_entry(&cterm_attr_table, &new_en);
    }

    ಯದಿ (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    ಯದಿ (char_aep != ರಿಕ್ತ)
	new_en = *char_aep;
    ಅನ್ಯಥಾ
    {
	CLEAR_FIELD(new_en);
	ಯದಿ (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    // For term mode, no separate background color handling.
    ಪ್ರತ್ಯಾಗಮ get_attr_entry(&term_attr_table, &new_en);
}

/*
 * Blend attributes for popup windows with opacity.
 */
    ಪೂರ್ಣಾಙ್ಕ
hl_blend_attr(ಪೂರ್ಣಾಙ್ಕ char_attr, ಪೂರ್ಣಾಙ್ಕ popup_attr, ಪೂರ್ಣಾಙ್ಕ blend, ಪೂರ್ಣಾಙ್ಕ blend_fg)
{
    ಯದಿ (blend >= 100 && blend_fg)
	ಪ್ರತ್ಯಾಗಮ char_attr;

    ಪ್ರತ್ಯಾಗಮ hl_blend_attr_common(char_attr, popup_attr, blend, blend_fg);
}

/*
 * Blend underlying foreground and background colors for pum space cells.
 */
    ಪೂರ್ಣಾಙ್ಕ
hl_pum_blend_attr(ಪೂರ್ಣಾಙ್ಕ char_attr, ಪೂರ್ಣಾಙ್ಕ popup_attr, ಪೂರ್ಣಾಙ್ಕ blend)
{
    ಪ್ರತ್ಯಾಗಮ hl_blend_attr_common(char_attr, popup_attr, blend, TRUE);
}

#ifdef FEAT_GUI
    attrentry_T *
syn_gui_attr2entry(ಪೂರ್ಣಾಙ್ಕ attr)
{
    attr -= ATTR_OFF;
    ಯದಿ (attr >= gui_attr_table.ga_len)	    // did ":syntax clear"
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ &(GUI_ATTR_ENTRY(attr));
}
#endif

/*
 * Get the highlight attributes (HL_BOLD etc.) from an attribute nr.
 * Only to be used when "attr" > HL_ALL.
 */
    ಪೂರ್ಣಾಙ್ಕ
syn_attr2attr(ಪೂರ್ಣಾಙ್ಕ attr)
{
    attrentry_T	*aep;

#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	aep = syn_gui_attr2entry(attr);
    ಅನ್ಯಥಾ
#endif
	ಯದಿ (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	ಅನ್ಯಥಾ
	    aep = syn_term_attr2entry(attr);

    ಯದಿ (aep == ರಿಕ್ತ)	    // highlighting not set
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ aep->ae_attr;
}


    attrentry_T *
syn_term_attr2entry(ಪೂರ್ಣಾಙ್ಕ attr)
{
    attr -= ATTR_OFF;
    ಯದಿ (attr >= term_attr_table.ga_len)	    // did ":syntax clear"
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ &(TERM_ATTR_ENTRY(attr));
}

    attrentry_T *
syn_cterm_attr2entry(ಪೂರ್ಣಾಙ್ಕ attr)
{
    attr -= ATTR_OFF;
    ಯದಿ (attr >= cterm_attr_table.ga_len)	// did ":syntax clear"
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ &(CTERM_ATTR_ENTRY(attr));
}

#define LIST_ATTR   1
#define LIST_STRING 2
#define LIST_INT    3

    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_list_one(ಪೂರ್ಣಾಙ್ಕ id)
{
    hl_group_T	    *sgp;
    ಪೂರ್ಣಾಙ್ಕ		    didh = FALSE;

    sgp = &HL_TABLE()[id - 1];	    // index is ID minus one

    ಯದಿ (message_filtered(sgp->sg_name))
	ಪ್ರತ್ಯಾಗಮ;

    // Note: Keep this in sync with expand_highlight_group().
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_term, ರಿಕ್ತ, "term");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_start, "start");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_stop, "stop");

    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_cterm, ರಿಕ್ತ, "cterm");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_fg, ರಿಕ್ತ, "ctermfg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_bg, ರಿಕ್ತ, "ctermbg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_ul, ರಿಕ್ತ, "ctermul");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_font, ರಿಕ್ತ, "ctermfont");

#ಯದಿ defined(FEAT_GUI) || defined(FEAT_EVAL)
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_gui, ರಿಕ್ತ, "gui");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_fg_name, "guifg");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_bg_name, "guibg");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_sp_name, "guisp");
#endif
#ifdef FEAT_GUI
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_font_name, "font");
#endif

    ಯದಿ (sgp->sg_link && !got_int)
    {
	(ಶೂನ್ಯ)syn_list_header(didh, 9999, id);
	didh = TRUE;
	ಸಂದೇಶ_puts_attr("links to", HL_ATTR(HLF_D));
	ಸಂದೇಶ_putchar(' ');
	ಸಂದೇಶ_outtrans(HL_TABLE()[HL_TABLE()[id - 1].sg_link - 1].sg_name);
    }

    ಯದಿ (!didh)
	highlight_list_arg(id, didh, LIST_STRING, 0, (char_u *)"cleared", "");
#ifdef FEAT_EVAL
    ಯದಿ (p_verbose > 0)
	last_set_ಸಂದೇಶ(sgp->sg_script_ctx);
#endif
}

    ಸ್ಥಿರ char_u*
highlight_arg_to_string(
    ಪೂರ್ಣಾಙ್ಕ		type,
    ಪೂರ್ಣಾಙ್ಕ		iarg,
    char_u	*sarg,
    char_u	*buf)
{
    ಯದಿ (type == LIST_INT)
	sprintf((ವರ್ಣ *)buf, "%d", iarg - 1);
    ಅನ್ಯಥಾ ಯದಿ (type == LIST_STRING)
	ಪ್ರತ್ಯಾಗಮ sarg;
    ಅನ್ಯಥಾ // type == LIST_ATTR
    {
	size_t buflen;

	buf[0] = NUL;
	buflen = 0;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(highlight_index_tab); ++i)
	{
	    ಯದಿ (iarg & highlight_index_tab[i]->ಕೀಲಿ)
	    {
		ಯದಿ (buflen > 0)
		{
		    STRCPY(buf + buflen, (char_u *)",");
		    ++buflen;
		}
		STRCPY(buf + buflen, highlight_index_tab[i]->ಮೌಲ್ಯ.string);
		buflen += highlight_index_tab[i]->ಮೌಲ್ಯ.length;
		iarg &= ~highlight_index_tab[i]->ಕೀಲಿ;	    // don't want "inverse"/"reverse"
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ buf;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
highlight_list_arg(
    ಪೂರ್ಣಾಙ್ಕ		id,
    ಪೂರ್ಣಾಙ್ಕ		didh,
    ಪೂರ್ಣಾಙ್ಕ		type,
    ಪೂರ್ಣಾಙ್ಕ		iarg,
    char_u	*sarg,
    ವರ್ಣ	*name)
{
    char_u	buf[MAX_ATTR_LEN];
    char_u	*ts;

    ಯದಿ (got_int)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (type == LIST_STRING ? (sarg == ರಿಕ್ತ) : (iarg == 0))
	ಪ್ರತ್ಯಾಗಮ didh;

    ts = highlight_arg_to_string(type, iarg, sarg, buf);

    (ಶೂನ್ಯ)syn_list_header(didh,
	    (ಪೂರ್ಣಾಙ್ಕ)(vim_strsize(ts) + STRLEN(name) + 1), id);
    didh = TRUE;
    ಯದಿ (!got_int)
    {
	ಯದಿ (*name != NUL)
	{
	    ಸಂದೇಶ_puts_attr(name, HL_ATTR(HLF_D));
	    ಸಂದೇಶ_puts_attr("=", HL_ATTR(HLF_D));
	}
	ಸಂದೇಶ_outtrans(ts);
    }
    ಪ್ರತ್ಯಾಗಮ didh;
}

#ಯದಿ ((defined(FEAT_EVAL) || defined(FEAT_PRINTER))) && defined(FEAT_SYN_HL)
/*
 * Return "1" if highlight group "id" has attribute "flag".
 * Return NULL otherwise.
 */
    char_u *
highlight_has_attr(
    ಪೂರ್ಣಾಙ್ಕ		id,
    ಪೂರ್ಣಾಙ್ಕ		flag,
    ಪೂರ್ಣಾಙ್ಕ		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    ಪೂರ್ಣಾಙ್ಕ		attr;

    ಯದಿ (id <= 0 || id > highlight_ga.ga_len)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

# ಯದಿ defined(FEAT_GUI) || defined(FEAT_EVAL)
    ಯದಿ (modec == 'g')
	attr = HL_TABLE()[id - 1].sg_gui;
    ಅನ್ಯಥಾ
# endif
    {
	ಯದಿ (modec == 'c')
	    attr = HL_TABLE()[id - 1].sg_cterm;
	ಅನ್ಯಥಾ
	    attr = HL_TABLE()[id - 1].sg_term;
    }

    ಯದಿ (attr & flag)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"1";
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(FEAT_SYN_HL) && defined(FEAT_EVAL)
/*
 * Return color name of highlight group "id".
 */
    char_u *
highlight_color(
    ಪೂರ್ಣಾಙ್ಕ		id,
    char_u	*what,	// "font", "fg", "bg", "sp", "ul", "fg#", "bg#" or "sp#"
    ಪೂರ್ಣಾಙ್ಕ		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    ಸ್ಥಿರ char_u	name[20];
    ಪೂರ್ಣಾಙ್ಕ			n;
    ಪೂರ್ಣಾಙ್ಕ			fg = FALSE;
    ಪೂರ್ಣಾಙ್ಕ			sp = FALSE;
    ಪೂರ್ಣಾಙ್ಕ			ul = FALSE;
    ಪೂರ್ಣಾಙ್ಕ			font = FALSE;

    ಯದಿ (id <= 0 || id > highlight_ga.ga_len)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'g')
	fg = TRUE;
    ಅನ್ಯಥಾ ಯದಿ (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'o'
	     && TOLOWER_ASC(what[2]) == 'n' && TOLOWER_ASC(what[3]) == 't')
	font = TRUE;
    ಅನ್ಯಥಾ ಯದಿ (TOLOWER_ASC(what[0]) == 's' && TOLOWER_ASC(what[1]) == 'p')
	sp = TRUE;
    ಅನ್ಯಥಾ ಯದಿ (TOLOWER_ASC(what[0]) == 'u' && TOLOWER_ASC(what[1]) == 'l')
	ul = TRUE;
    ಅನ್ಯಥಾ ಯದಿ (!(TOLOWER_ASC(what[0]) == 'b' && TOLOWER_ASC(what[1]) == 'g'))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (modec == 'g')
    {
# ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI
	// return font name
	ಯದಿ (font)
	    ಪ್ರತ್ಯಾಗಮ HL_TABLE()[id - 1].sg_font_name;
#  endif

	// return #RRGGBB form (only possible when GUI is running)
	ಯದಿ ((USE_24BIT) && what[2] == '#')
	{
	    guicolor_T		color;
	    long_u		rgb;
	    ಸ್ಥಿರ char_u	buf[10];

	    ಯದಿ (fg)
		color = HL_TABLE()[id - 1].sg_gui_fg;
	    ಅನ್ಯಥಾ ಯದಿ (sp)
		color = HL_TABLE()[id - 1].sg_gui_sp;
	    ಅನ್ಯಥಾ
		color = HL_TABLE()[id - 1].sg_gui_bg;
	    ಯದಿ (color == INVALCOLOR)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    rgb = (long_u)GUI_MCH_GET_RGB(color);
	    sprintf((ವರ್ಣ *)buf, "#%02x%02x%02x",
				      (unsigned)(rgb >> 16),
				      (unsigned)(rgb >> 8) & 255,
				      (unsigned)rgb & 255);
	    ಪ್ರತ್ಯಾಗಮ buf;
	}
# endif
	ಯದಿ (fg)
	    ಪ್ರತ್ಯಾಗಮ (HL_TABLE()[id - 1].sg_gui_fg_name);
	ಯದಿ (sp)
	    ಪ್ರತ್ಯಾಗಮ (HL_TABLE()[id - 1].sg_gui_sp_name);
	ಪ್ರತ್ಯಾಗಮ (HL_TABLE()[id - 1].sg_gui_bg_name);
    }
    ಯದಿ (sp)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (modec == 'c')
    {
	ಯದಿ (fg)
	    n = HL_TABLE()[id - 1].sg_cterm_fg - 1;
	ಅನ್ಯಥಾ ಯದಿ (ul)
	    n = HL_TABLE()[id - 1].sg_cterm_ul - 1;
	ಅನ್ಯಥಾ ಯದಿ (font)
	    n = HL_TABLE()[id - 1].sg_cterm_font - 1;
	ಅನ್ಯಥಾ
	    n = HL_TABLE()[id - 1].sg_cterm_bg - 1;
	ಯದಿ (n < 0)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	sprintf((ವರ್ಣ *)name, "%d", n);
	ಪ್ರತ್ಯಾಗಮ name;
    }
    // term doesn't have color
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(FEAT_SYN_HL) \
	    && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)) \
	&& defined(FEAT_PRINTER)
/*
 * Return color name of highlight group "id" as RGB value.
 */
    long_u
highlight_gui_color_rgb(
    ಪೂರ್ಣಾಙ್ಕ		id,
    ಪೂರ್ಣಾಙ್ಕ		fg)	// TRUE = fg, FALSE = bg
{
    guicolor_T	color;

    ಯದಿ (id <= 0 || id > highlight_ga.ga_len)
	ಪ್ರತ್ಯಾಗಮ 0L;

    ಯದಿ (fg)
	color = HL_TABLE()[id - 1].sg_gui_fg;
    ಅನ್ಯಥಾ
	color = HL_TABLE()[id - 1].sg_gui_bg;

    ಯದಿ (color == INVALCOLOR)
	ಪ್ರತ್ಯಾಗಮ 0L;

    ಪ್ರತ್ಯಾಗಮ GUI_MCH_GET_RGB(color);
}
#endif

/*
 * Output the syntax list header.
 * Return TRUE when started a new line.
 */
    ಪೂರ್ಣಾಙ್ಕ
syn_list_header(
    ಪೂರ್ಣಾಙ್ಕ	    did_header,		// did header already
    ಪೂರ್ಣಾಙ್ಕ	    outlen,		// length of string that comes
    ಪೂರ್ಣಾಙ್ಕ	    id)			// highlight group id
{
    ಪೂರ್ಣಾಙ್ಕ	    endcol = 19;
    ಪೂರ್ಣಾಙ್ಕ	    newline = TRUE;
    ಪೂರ್ಣಾಙ್ಕ	    name_col = 0;

    ಯದಿ (!did_header)
    {
	ಸಂದೇಶ_putchar('\n');
	ಯದಿ (got_int)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	ಸಂದೇಶ_outtrans(HL_TABLE()[id - 1].sg_name);
	name_col = ಸಂದೇಶ_col;
	endcol = 15;
    }
    ಅನ್ಯಥಾ ಯದಿ (ಸಂದೇಶ_col + outlen + 1 >= cmdline_width)
    {
	ಸಂದೇಶ_putchar('\n');
	ಯದಿ (got_int)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (ಸಂದೇಶ_col >= endcol)	// wrap around is like starting a new line
	    newline = FALSE;
    }

    ಯದಿ (ಸಂದೇಶ_col >= endcol)	// output at least one space
	endcol = ಸಂದೇಶ_col + 1;
    ಯದಿ (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    ಸಂದೇಶ_advance(endcol);

    // Show "xxx" with the attributes.
    ಯದಿ (!did_header)
    {
	ಯದಿ (endcol == cmdline_width - 1 && endcol <= name_col)
	    ಸಂದೇಶ_putchar(' ');
	ಸಂದೇಶ_puts_attr("xxx", syn_id2attr(id));
	ಸಂದೇಶ_putchar(' ');
    }

    ಪ್ರತ್ಯಾಗಮ newline;
}

/*
 * Set the attribute numbers for a highlight group.
 * Called after one of the attributes has changed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_hl_attr(
    ಪೂರ್ಣಾಙ್ಕ		idx)	    // index in array
{
    attrentry_T	    at_en;
    hl_group_T	    *sgp = HL_TABLE() + idx;

    // The "Normal" group doesn't need an attribute number
    ಯದಿ (sgp->sg_name_u != ರಿಕ್ತ && STRCMP(sgp->sg_name_u, "NORMAL") == 0)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_GUI
    // For the GUI mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    ಯದಿ (sgp->sg_gui_fg == INVALCOLOR
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
    ಅನ್ಯಥಾ
    {
	at_en.ae_attr = sgp->sg_gui;
	at_en.ae_u.gui.fg_color = sgp->sg_gui_fg;
	at_en.ae_u.gui.bg_color = sgp->sg_gui_bg;
	at_en.ae_u.gui.sp_color = sgp->sg_gui_sp;
	at_en.ae_u.gui.font = sgp->sg_font;
# ifdef FEAT_XFONTSET
	at_en.ae_u.gui.fontset = sgp->sg_fontset;
# endif
	sgp->sg_gui_attr = get_attr_entry(&gui_attr_table, &at_en);
    }
#endif
    // For the term mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    ಯದಿ (sgp->sg_start == ರಿಕ್ತ && sgp->sg_stop == ರಿಕ್ತ)
	sgp->sg_term_attr = sgp->sg_term;
    ಅನ್ಯಥಾ
    {
	at_en.ae_attr = sgp->sg_term;
	at_en.ae_u.term.start = sgp->sg_start;
	at_en.ae_u.term.stop = sgp->sg_stop;
	sgp->sg_term_attr = get_attr_entry(&term_attr_table, &at_en);
    }

    // For the color term mode: If there are other than "normal"
    // highlighting attributes, need to allocate an attr number.
    ಯದಿ (sgp->sg_cterm_fg == 0 && sgp->sg_cterm_bg == 0 &&
	sgp->sg_cterm_ul == 0 && sgp->sg_cterm_font == 0
#ifdef FEAT_TERMGUICOLORS
	    && sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
#endif
	    )
	sgp->sg_cterm_attr = sgp->sg_cterm;
    ಅನ್ಯಥಾ
    {
	at_en.ae_attr = sgp->sg_cterm;
	at_en.ae_u.cterm.fg_color = sgp->sg_cterm_fg;
	at_en.ae_u.cterm.bg_color = sgp->sg_cterm_bg;
	at_en.ae_u.cterm.ul_color = sgp->sg_cterm_ul;
	at_en.ae_u.cterm.font = sgp->sg_cterm_font;
#ifdef FEAT_TERMGUICOLORS
	at_en.ae_u.cterm.fg_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_fg);
	at_en.ae_u.cterm.bg_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_bg);
	// Only use the underline/undercurl color when used, it may clear the
	// background color if not supported.
	ಯದಿ (sgp->sg_cterm & (HL_UNDERLINE | HL_UNDERCURL
			   | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED))
	    at_en.ae_u.cterm.ul_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_sp);
	ಅನ್ಯಥಾ
	    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	ಯದಿ (at_en.ae_u.cterm.fg_rgb == INVALCOLOR
		&& at_en.ae_u.cterm.bg_rgb == INVALCOLOR)
	{
	    // If both fg and bg are invalid fall back to the cterm colors.
	    // Helps when the GUI only uses an attribute, e.g. undercurl.
	    at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	    at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
	}
#endif
	sgp->sg_cterm_attr = get_attr_entry(&cterm_attr_table, &at_en);
    }
}

/*
 * Check if highlight id is overridden, and return the overriding highlight id.
 * Otherwise return the original id if no override.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_override(ಪೂರ್ಣಾಙ್ಕ id)
{
    ಯದಿ (overrides != ರಿಕ್ತ && overrides->arr != ರಿಕ್ತ)
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < overrides->len; k++)
	    ಯದಿ (overrides->arr[k].from == id)
		ಪ್ರತ್ಯಾಗಮ overrides->arr[k].to;
    ಪ್ರತ್ಯಾಗಮ id;
}

/*
 * Lookup a highlight group name by pointer and length.
 * If it is not found, 0 is returned.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_name2id_len(char_u *name, ಪೂರ್ಣಾಙ್ಕ len)
{
    char_u	name_u[MAX_SYN_NAME + 1];
    hashitem_T	*hi;

    ಯದಿ (len > MAX_SYN_NAME)
	len = MAX_SYN_NAME;
    mch_memmove(name_u, name, len);
    name_u[len] = NUL;
    vim_strup(name_u);
    ಯದಿ (!highlight_ht_inited)
	ಪ್ರತ್ಯಾಗಮ 0;
    hi = hash_find(&highlight_ht, name_u);
    ಯದಿ (HASHITEM_EMPTY(hi))
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ HI2HLNAME(hi)->hn_id;
}

/*
 * Lookup a highlight group name and return its ID.
 * If it is not found, 0 is returned.
 */
    ಪೂರ್ಣಾಙ್ಕ
syn_name2id(char_u *name)
{
    // Avoid using stricmp() too much, it's slow on some systems.
    ಪ್ರತ್ಯಾಗಮ syn_name2id_len(name, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(name));
}

/*
 * Lookup a highlight group name and return its attributes.
 * Return zero if not found.
 */
    ಪೂರ್ಣಾಙ್ಕ
syn_name2attr(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ id = syn_name2id(name);

    ಯದಿ (id != 0)
	ಪ್ರತ್ಯಾಗಮ syn_id2attr(id);
    ಪ್ರತ್ಯಾಗಮ 0;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return TRUE if highlight group "name" exists.
 */
    ಪೂರ್ಣಾಙ್ಕ
highlight_exists(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ (syn_name2id(name) > 0);
}

# ಯದಿ defined(FEAT_ಹುಡುಕು_EXTRA)
/*
 * Return the name of highlight group "id".
 * When not a valid ID return an empty string.
 */
    char_u *
syn_id2name(ಪೂರ್ಣಾಙ್ಕ id)
{
    ಯದಿ (id <= 0 || id > highlight_ga.ga_len)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"";
    ಪ್ರತ್ಯಾಗಮ HL_TABLE()[id - 1].sg_name;
}
# endif
#endif

/*
 * Like syn_name2id(), but take a pointer + length argument.
 */
    ಪೂರ್ಣಾಙ್ಕ
syn_namen2id(char_u *linep, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪ್ರತ್ಯಾಗಮ syn_name2id_len(linep, len);
}

/*
 * Find highlight group name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
syn_check_group(char_u *pp, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪೂರ್ಣಾಙ್ಕ	    id;
    char_u  *name;

    ಯದಿ (len > MAX_SYN_NAME)
    {
	emsg(_(e_highlight_group_name_too_long));
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    id = syn_name2id_len(pp, len);
    ಯದಿ (id == 0)			// doesn't exist yet
    {
	name = vim_strnsave(pp, len);
	ಯದಿ (name == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ 0;
	id = syn_add_group(name);
    }
    ಪ್ರತ್ಯಾಗಮ id;
}

/*
 * Add new highlight group and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_add_group(char_u *name)
{
    char_u	*p;
    char_u	*name_up;

    // Check that the name is valid (ASCII letters, digits, underscores, dots, or hyphens).
    ಪ್ರತಿ (p = name; *p != NUL; ++p)
    {
	ಯದಿ (!vim_isprintc(*p))
	{
	    emsg(_(e_unprintable_character_in_group_name));
	    vim_free(name);
	    ಪ್ರತ್ಯಾಗಮ 0;
	}
	ಅನ್ಯಥಾ ಯದಿ (!ASCII_ISALNUM(*p) && *p != '_' && *p != '.' && *p != '-')
	{
	    // This is an error, but since there previously was no check only
	    // give a warning.
	    ಸಂದೇಶ_source(HL_ATTR(HLF_W));
	    ಸಂದೇಶ(_("W18: Invalid character in group name"));
	    ಭಙ್ಗ;
	}
    }

    // First call for this growarray: init growing array.
    ಯದಿ (highlight_ga.ga_data == ರಿಕ್ತ)
    {
	highlight_ga.ga_itemsize = ಪರಿಮಾಣ(hl_group_T);
	highlight_ga.ga_growsize = 10;
	hash_init(&highlight_ht);
	highlight_ht_inited = true;
    }

    ಯದಿ (highlight_ga.ga_len >= MAX_HL_ID)
    {
	emsg(_(e_too_many_highlight_and_syntax_groups));
	vim_free(name);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    // Make room for at least one other syntax_highlight entry.
    ಯದಿ (ga_grow(&highlight_ga, 1) == FAIL)
    {
	vim_free(name);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    {
	hlname_T    *hn;
	ಪೂರ್ಣಾಙ್ಕ	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(name);

	hn = alloc(offsetof(hlname_T, hn_ಕೀಲಿ) + len + 1);
	ಯದಿ (hn == ರಿಕ್ತ)
	{
	    vim_free(name);
	    ಪ್ರತ್ಯಾಗಮ 0;
	}
	vim_strncpy(hn->hn_ಕೀಲಿ, name, len);
	vim_strup(hn->hn_ಕೀಲಿ);
	hn->hn_id = highlight_ga.ga_len + 1;	// ID is index plus one
	name_up = hn->hn_ಕೀಲಿ;
    }

    CLEAR_POINTER(&(HL_TABLE()[highlight_ga.ga_len]));
    HL_TABLE()[highlight_ga.ga_len].sg_name = name;
    HL_TABLE()[highlight_ga.ga_len].sg_name_u = name_up;
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_TABLE()[highlight_ga.ga_len].sg_gui_bg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_fg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_sp = INVALCOLOR;
#endif
    hash_add(&highlight_ht, name_up, "highlight");
    ++highlight_ga.ga_len;

    ಪ್ರತ್ಯಾಗಮ highlight_ga.ga_len;		    // ID is index plus one
}

/*
 * When, just after calling syn_add_group(), an error is discovered, this
 * function deletes the new name.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_unadd_group(ಶೂನ್ಯ)
{
    hashitem_T *hi;

    --highlight_ga.ga_len;
    hi = hash_find(&highlight_ht, HL_TABLE()[highlight_ga.ga_len].sg_name_u);
    ಯದಿ (!HASHITEM_EMPTY(hi))
	hash_remove(&highlight_ht, hi, "highlight");
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name);
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name_u - HLNAME_ಕೀಲಿ_OFF);
}

/*
 * Translate a group ID to highlight attributes.
 * "hl_id" must be valid: > 0, caller must check.
 */
    ಪೂರ್ಣಾಙ್ಕ
syn_id2attr(ಪೂರ್ಣಾಙ್ಕ hl_id)
{
    ಪೂರ್ಣಾಙ್ಕ		attr;
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ಪ್ರತಿಜ್ಞಾ(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

#ifdef FEAT_GUI
    // Only use GUI attr when the GUI is being used.
    ಯದಿ (gui.in_use)
	attr = sgp->sg_gui_attr;
    ಅನ್ಯಥಾ
#endif
	ಯದಿ (IS_CTERM)
	    attr = sgp->sg_cterm_attr;
	ಅನ್ಯಥಾ
	    attr = sgp->sg_term_attr;

    ಪ್ರತ್ಯಾಗಮ attr;
}

#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Get the GUI colors and attributes for a group ID.
 * NOTE: the colors will be INVALCOLOR when not set, the color otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
syn_id2colors(ಪೂರ್ಣಾಙ್ಕ hl_id, guicolor_T *fgp, guicolor_T *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ಪ್ರತಿಜ್ಞಾ(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

    *fgp = sgp->sg_gui_fg;
    *bgp = sgp->sg_gui_bg;
    ಪ್ರತ್ಯಾಗಮ sgp->sg_gui;
}
#endif

#ಯದಿ (defined(MSWIN) \
	    && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL)) \
	    && defined(FEAT_TERMGUICOLORS)) \
	|| defined(FEAT_TERMINAL)
    ಶೂನ್ಯ
syn_id2cterm_bg(ಪೂರ್ಣಾಙ್ಕ hl_id, ಪೂರ್ಣಾಙ್ಕ *fgp, ಪೂರ್ಣಾಙ್ಕ *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ಪ್ರತಿಜ್ಞಾ(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
    *fgp = sgp->sg_cterm_fg - 1;
    *bgp = sgp->sg_cterm_bg - 1;
}
#endif

/*
 * Translate a group ID to the final group ID (following links).
 */
    ಪೂರ್ಣಾಙ್ಕ
syn_get_final_id(ಪೂರ್ಣಾಙ್ಕ hl_id)
{
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    hl_group_T	*sgp;

    ಯದಿ (hl_id > highlight_ga.ga_len || hl_id < 1)
	ಪ್ರತ್ಯಾಗಮ 0;			// Can be called from eval!!

    // Follow links until there is no more.
    // Look out for loops!  Break after 100 links.
    ಪ್ರತಿ (ಎಣಿಕೆ = 100; --ಎಣಿಕೆ >= 0; )
    {
	ಪೂರ್ಣಾಙ್ಕ tmp = hl_id;

	sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
	ಯದಿ (sgp->sg_link == 0 || sgp->sg_link > highlight_ga.ga_len)
	    ಭಙ್ಗ;

	// This is to handle highlight groups that are overridden but are in the
	// middle of a link chain.
	hl_id = syn_override(hl_id);
	ಯದಿ (tmp != hl_id)
	    ಅನುವರ್ತ;
	hl_id = sgp->sg_link;
    }
    ಪ್ರತ್ಯಾಗಮ syn_override(hl_id);
}

#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Call this function just after the GUI has started.
 * Also called when 'termguicolors' was set, gui.in_use will be FALSE then.
 * It finds the font and color handles for the highlighting groups.
 */
    ಶೂನ್ಯ
highlight_gui_started(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    idx;

    // First get the colors from the "Normal" and "Menu" group, if set
    ಯದಿ (USE_24BIT)
	set_normal_colors();

    ಪ್ರತಿ (idx = 0; idx < highlight_ga.ga_len; ++idx)
	gui_do_one_color(idx, FALSE, FALSE);

    highlight_changed();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_do_one_color(
    ಪೂರ್ಣಾಙ್ಕ		idx,
    ಪೂರ್ಣಾಙ್ಕ		do_menu UNUSED,	   // TRUE: might set the menu font
    ಪೂರ್ಣಾಙ್ಕ		do_tooltip UNUSED) // TRUE: might set the tooltip font
{
    ಪೂರ್ಣಾಙ್ಕ		didit = FALSE;

# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    ಯದಿ (gui.in_use)
#  endif
	ಯದಿ (HL_TABLE()[idx].sg_font_name != ರಿಕ್ತ)
	{
	    hl_do_font(idx, HL_TABLE()[idx].sg_font_name, FALSE, do_menu,
							    do_tooltip, TRUE);
	    didit = TRUE;
	}
# endif
    ಯದಿ (HL_TABLE()[idx].sg_gui_fg_name != ರಿಕ್ತ)
    {
	HL_TABLE()[idx].sg_gui_fg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_fg_name);
	didit = TRUE;
    }
    ಯದಿ (HL_TABLE()[idx].sg_gui_bg_name != ರಿಕ್ತ)
    {
	HL_TABLE()[idx].sg_gui_bg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_bg_name);
	didit = TRUE;
    }
    ಯದಿ (HL_TABLE()[idx].sg_gui_sp_name != ರಿಕ್ತ)
    {
	HL_TABLE()[idx].sg_gui_sp =
			    color_name2handle(HL_TABLE()[idx].sg_gui_sp_name);
	didit = TRUE;
    }
    ಯದಿ (didit)	// need to get a new attr number
	set_hl_attr(idx);
}
#endif

#ಯದಿ defined(USER_HIGHLIGHT) && defined(FEAT_STL_OPT)
/*
 * Apply difference between User[1-9] and HLF_S to HLF_SNC, HLF_ST or HLF_STNC.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
combine_stl_hlt(
	ಪೂರ್ಣಾಙ್ಕ id,
	ಪೂರ್ಣಾಙ್ಕ id_S,
	ಪೂರ್ಣಾಙ್ಕ id_alt,
	ಪೂರ್ಣಾಙ್ಕ hlcnt,
	ಪೂರ್ಣಾಙ್ಕ i,
	ಪೂರ್ಣಾಙ್ಕ hlf,
	ಪೂರ್ಣಾಙ್ಕ *table)
{
    hl_group_T *hlt = HL_TABLE();

    ಯದಿ (id_alt == 0)
    {
	CLEAR_POINTER(&hlt[hlcnt + i]);
	hlt[hlcnt + i].sg_term = highlight_attr[hlf];
	hlt[hlcnt + i].sg_cterm = highlight_attr[hlf];
# ಯದಿ defined(FEAT_GUI) || defined(FEAT_EVAL)
	hlt[hlcnt + i].sg_gui = highlight_attr[hlf];
# endif
    }
    ಅನ್ಯಥಾ
	mch_memmove(&hlt[hlcnt + i],
		    &hlt[id_alt - 1],
		    ಪರಿಮಾಣ(hl_group_T));
    hlt[hlcnt + i].sg_link = 0;

    hlt[hlcnt + i].sg_term ^=
	hlt[id - 1].sg_term ^ hlt[id_S - 1].sg_term;
    ಯದಿ (hlt[id - 1].sg_start != hlt[id_S - 1].sg_start)
	hlt[hlcnt + i].sg_start = hlt[id - 1].sg_start;
    ಯದಿ (hlt[id - 1].sg_stop != hlt[id_S - 1].sg_stop)
	hlt[hlcnt + i].sg_stop = hlt[id - 1].sg_stop;
    hlt[hlcnt + i].sg_cterm ^=
	hlt[id - 1].sg_cterm ^ hlt[id_S - 1].sg_cterm;
    ಯದಿ (hlt[id - 1].sg_cterm_fg != hlt[id_S - 1].sg_cterm_fg)
	hlt[hlcnt + i].sg_cterm_fg = hlt[id - 1].sg_cterm_fg;
    ಯದಿ (hlt[id - 1].sg_cterm_bg != hlt[id_S - 1].sg_cterm_bg)
	hlt[hlcnt + i].sg_cterm_bg = hlt[id - 1].sg_cterm_bg;
    ಯದಿ (hlt[id - 1].sg_cterm_font != hlt[id_S - 1].sg_cterm_font)
	hlt[hlcnt + i].sg_cterm_font = hlt[id - 1].sg_cterm_font;
# ಯದಿ defined(FEAT_GUI) || defined(FEAT_EVAL)
    hlt[hlcnt + i].sg_gui ^=
	hlt[id - 1].sg_gui ^ hlt[id_S - 1].sg_gui;
# endif
# ifdef FEAT_GUI
    ಯದಿ (hlt[id - 1].sg_gui_fg != hlt[id_S - 1].sg_gui_fg)
	hlt[hlcnt + i].sg_gui_fg = hlt[id - 1].sg_gui_fg;
    ಯದಿ (hlt[id - 1].sg_gui_bg != hlt[id_S - 1].sg_gui_bg)
	hlt[hlcnt + i].sg_gui_bg = hlt[id - 1].sg_gui_bg;
    ಯದಿ (hlt[id - 1].sg_gui_sp != hlt[id_S - 1].sg_gui_sp)
	hlt[hlcnt + i].sg_gui_sp = hlt[id - 1].sg_gui_sp;
    ಯದಿ (hlt[id - 1].sg_font != hlt[id_S - 1].sg_font)
	hlt[hlcnt + i].sg_font = hlt[id - 1].sg_font;
#  ifdef FEAT_XFONTSET
    ಯದಿ (hlt[id - 1].sg_fontset != hlt[id_S - 1].sg_fontset)
	hlt[hlcnt + i].sg_fontset = hlt[id - 1].sg_fontset;
#  endif
# endif
    highlight_ga.ga_len = hlcnt + i + 1;
    set_hl_attr(hlcnt + i);	// At long last we can apply
    table[i] = syn_id2attr(hlcnt + i + 1);
}
#endif

/*
 * Translate the 'highlight' option into attributes in highlight_attr[] and
 * set up the user highlights User1..9.  If FEAT_STL_OPT is in use, a set of
 * corresponding highlights to use on top of HLF_SNC is computed.
 * Called only when the 'highlight' option has been changed and upon first
 * screen redraw after any :highlight command.
 * Return FAIL when an invalid flag is found in 'highlight'.  OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
highlight_changed(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		hlf;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*p;
    char_u	*default_hl;
    ಪೂರ್ಣಾಙ್ಕ		attr;
    char_u	*end;
    ಪೂರ್ಣಾಙ್ಕ		id;
#ifdef USER_HIGHLIGHT
    char_u      userhl[30];  // use 30 to avoid compiler warning
# ifdef FEAT_STL_OPT
    ಪೂರ್ಣಾಙ್ಕ		id_S = -1;
    ಪೂರ್ಣಾಙ್ಕ		id_SNC = 0;
#  ifdef FEAT_TERMINAL
    ಪೂರ್ಣಾಙ್ಕ		id_ST = 0;
    ಪೂರ್ಣಾಙ್ಕ		id_STNC = 0;
#  endif
    ಪೂರ್ಣಾಙ್ಕ		hlcnt;
# endif
#endif
    win_T	*wp;

    need_highlight_changed = FALSE;

#ifdef FEAT_TERMINAL
    term_update_colors_all();
    term_update_hlfwin_all();
#endif

    // Clear all attributes.
    ಪ್ರತಿ (hlf = 0; hlf < (ಪೂರ್ಣಾಙ್ಕ)HLF_ಎಣಿಕೆ; ++hlf)
    {
	highlight_attr[hlf] = 0;
	highlight_attr_raw[hlf] = 0;
	highlight_ids[hlf] = 0;
    }

    default_hl = get_highlight_default();

    // First set all attributes to their default value.
    // Then use the attributes from the 'highlight' option, unless it already
    // matches the default and would repeat the same work.
    ಪ್ರತಿ (i = 0; i < 2; ++i)
    {
	ಯದಿ (i)
	{
	    ಯದಿ (default_hl != ರಿಕ್ತ && p_hl != ರಿಕ್ತ
				      && STRCMP(default_hl, p_hl) == 0)
		ಅನುವರ್ತ;
	    p = p_hl;
	}
	ಅನ್ಯಥಾ
	    p = default_hl;
	ಯದಿ (p == ರಿಕ್ತ)	    // just in case
	    ಅನುವರ್ತ;

	ಯಾವತ್ (*p)
	{
	    ಪ್ರತಿ (hlf = 0; hlf < (ಪೂರ್ಣಾಙ್ಕ)HLF_ಎಣಿಕೆ; ++hlf)
		ಯದಿ (hl_flags[hlf] == *p)
		    ಭಙ್ಗ;
	    ++p;
	    ಯದಿ (hlf == (ಪೂರ್ಣಾಙ್ಕ)HLF_ಎಣಿಕೆ || *p == NUL)
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    // Allow several hl_flags to be combined, like "bu" for
	    // bold-underlined.
	    attr = 0;
	    id = 0;
	    ಪ್ರತಿ ( ; *p && *p != ','; ++p)	    // parse up to comma
	    {
		ಯದಿ (VIM_ISWHITE(*p))		    // ignore white space
		    ಅನುವರ್ತ;

		ಯದಿ (attr > HL_ALL)  // Combination with ':' is not allowed.
		    ಪ್ರತ್ಯಾಗಮ FAIL;

		// Note: Keep this in sync with expand_set_highlight().
		ವಿಕಲ್ಪ (*p)
		{
		    ಪಕ್ಷ 'b':	attr |= HL_BOLD;
				ಭಙ್ಗ;
		    ಪಕ್ಷ 'i':	attr |= HL_ITALIC;
				ಭಙ್ಗ;
		    ಪಕ್ಷ '-':
		    ಪಕ್ಷ 'n':			    // no highlighting
				ಭಙ್ಗ;
		    ಪಕ್ಷ 'r':	attr |= HL_INVERSE;
				ಭಙ್ಗ;
		    ಪಕ್ಷ 's':	attr |= HL_STANDOUT;
				ಭಙ್ಗ;
		    ಪಕ್ಷ 'u':	attr |= HL_UNDERLINE;
				ಭಙ್ಗ;
		    ಪಕ್ಷ 'c':	attr |= HL_UNDERCURL;
				ಭಙ್ಗ;
		    ಪಕ್ಷ '2':	attr |= HL_UNDERDOUBLE;
				ಭಙ್ಗ;
		    ಪಕ್ಷ 'd':	attr |= HL_UNDERDOTTED;
				ಭಙ್ಗ;
		    ಪಕ್ಷ '=':	attr |= HL_UNDERDASHED;
				ಭಙ್ಗ;
		    ಪಕ್ಷ 't':	attr |= HL_STRIKETHROUGH;
				ಭಙ್ಗ;
		    ಪಕ್ಷ ':':	++p;		    // highlight group name
				ಯದಿ (attr || *p == NUL)	 // no combinations
				    ಪ್ರತ್ಯಾಗಮ FAIL;
				end = vim_strchr(p, ',');
				ಯದಿ (end == ರಿಕ್ತ)
				    end = p + STRLEN(p);
				id = syn_check_group(p, (ಪೂರ್ಣಾಙ್ಕ)(end - p));
				ಯದಿ (id == 0)
				    ಪ್ರತ್ಯಾಗಮ FAIL;
				attr = syn_id2attr(id);
				p = end - 1;
#ಯದಿ defined(FEAT_STL_OPT) && defined(USER_HIGHLIGHT)
				ಯದಿ (hlf == (ಪೂರ್ಣಾಙ್ಕ)HLF_SNC)
				    id_SNC = syn_get_final_id(id);
# ifdef FEAT_TERMINAL
				ಅನ್ಯಥಾ ಯದಿ (hlf == (ಪೂರ್ಣಾಙ್ಕ)HLF_ST)
				    id_ST = syn_get_final_id(id);
				ಅನ್ಯಥಾ ಯದಿ (hlf == (ಪೂರ್ಣಾಙ್ಕ)HLF_STNC)
				    id_STNC = syn_get_final_id(id);
# endif
				ಅನ್ಯಥಾ ಯದಿ (hlf == (ಪೂರ್ಣಾಙ್ಕ)HLF_S)
				    id_S = syn_get_final_id(id);
#endif
				ಭಙ್ಗ;
		    ಸ್ವಭಾವ:	ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    highlight_attr[hlf] = attr;
	    highlight_attr_raw[hlf] = attr;
	    highlight_ids[hlf] = id;

	    p = skip_to_option_part(p);	    // skip comma and spaces
	}
    }

    // Highlight ids may have been changed, so keep windows up to date
    FOR_ALL_WINDOWS(wp)
    {
	ವರ್ಣ *errmsg = update_winhighlight(wp, wp->w_p_whl);

	ಯದಿ (errmsg != ರಿಕ್ತ)
	    emsg(_(errmsg));
    }

#ifdef FEAT_TERMINAL
    term_update_hlfwin_all();
#endif

#ifdef USER_HIGHLIGHT
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
    ಯದಿ (ga_grow(&highlight_ga, 28) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    hlcnt = highlight_ga.ga_len;
    ಯದಿ (id_S == -1)
    {
	// Make sure id_S is always valid to simplify code below. Use the last
	// entry.
	CLEAR_POINTER(&HL_TABLE()[hlcnt + 27]);
	HL_TABLE()[hlcnt + 18].sg_term = highlight_attr[HLF_S];
	id_S = hlcnt + 19;
    }
# endif
    ಪ್ರತಿ (i = 0; i < 9; i++)
    {
	sprintf((ವರ್ಣ *)userhl, "User%d", i + 1);
	id = syn_name2id(userhl);
	ಯದಿ (id == 0)
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
	ಅನ್ಯಥಾ
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

#endif // USER_HIGHLIGHT

    ಪ್ರತ್ಯಾಗಮ OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ highlight_list(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ highlight_list_two(ಪೂರ್ಣಾಙ್ಕ cnt, ಪೂರ್ಣಾಙ್ಕ attr);

// context for :highlight <group> <arg> expansion
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ expand_hi_synid = 0;	    // ID for highlight group being completed
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ expand_hi_equal_col = 0; // column where the '=' is
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ expand_hi_include_orig = 0;	    // whether to fill the existing current value or not
ಸ್ಥಿರ char_u *expand_hi_curvalue = ರಿಕ್ತ;   // the existing current value
#ಯದಿ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
ಸ್ಥಿರ dict_iterator_T expand_colornames_iter;	// iterator for looping through v:colornames
#endif

/*
 * Handle command line completion for :highlight command.
 */
    ಶೂನ್ಯ
set_context_in_highlight_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		expand_group = TRUE;

    // Default: expand group names
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 0;
    include_link = 2;
    include_default = 1;

    ಯದಿ (*arg == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    // past "default" or group name
    include_default = 0;
    ಯದಿ (STRNCMP("default", arg, p - arg) == 0)
    {
	arg = skipwhite(p);
	xp->xp_pattern = arg;
	p = skiptowhite(arg);
    }
    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    // past group name
    include_link = 0;
    ಯದಿ (arg[1] == 'i' && arg[0] == 'N')
    {
	highlight_list();
	expand_group = FALSE;
    }
    ಯದಿ (STRNCMP("link", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	ಯದಿ (*p != NUL)		// past first group name
	{
	    xp->xp_pattern = skipwhite(p);
	    p = skiptowhite(xp->xp_pattern);
	    include_none = 1;
	}
	expand_group = FALSE;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP("clear", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	expand_group = FALSE;
    }
    ಯದಿ (*p != NUL)			// past group name(s)
    {
	ಯದಿ (expand_group)
	{
	    // expansion will be done in expand_highlight_group()
	    xp->xp_context = EXPAND_HIGHLIGHT_GROUP;

	    expand_hi_synid = syn_namen2id(arg, (ಪೂರ್ಣಾಙ್ಕ)(p - arg));

	    ಯಾವತ್ (*p != NUL)
	    {
		arg = skipwhite(p);
		p = skiptowhite(arg);
	    }

	    p = vim_strchr(arg, '=');
	    ಯದಿ (p == ರಿಕ್ತ)
	    {
		// Didn't find a key=<value> pattern
		xp->xp_pattern = arg;
		expand_hi_equal_col = -1;
		expand_hi_include_orig = FALSE;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Found key=<value> pattern, record the exact location
		expand_hi_equal_col = (ಪೂರ್ಣಾಙ್ಕ)(p - xp->xp_ಸಾಲು);

		// Only include the original value if the pattern is empty
		ಯದಿ (*(p + 1) == NUL)
		    expand_hi_include_orig = TRUE;
		ಅನ್ಯಥಾ
		    expand_hi_include_orig = FALSE;

		// Account for comma-separated values
		ಯದಿ (STRNCMP(arg, "term=", 5) == 0 ||
			STRNCMP(arg, "cterm=", 6) == 0 ||
			STRNCMP(arg, "gui=", 4) == 0)
		{
		    char_u *comma = vim_strrchr(p + 1, ',');
		    ಯದಿ (comma != ರಿಕ್ತ)
			p = comma;
		}
		xp->xp_pattern = p + 1;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    xp->xp_context = EXPAND_NOTHING;
	}
    }
}

/*
 * List highlighting matches in a nice way.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_list(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 10; --i >= 0; )
	highlight_list_two(i, HL_ATTR(HLF_D));
    ಪ್ರತಿ (i = 40; --i >= 0; )
	highlight_list_two(99, 0);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_list_two(ಪೂರ್ಣಾಙ್ಕ cnt, ಪೂರ್ಣಾಙ್ಕ attr)
{
    ಸಂದೇಶ_puts_attr(&("N \bI \b!  \b"[cnt / 11]), attr);
    ಸಂದೇಶ_clr_eos();
    out_flush();
    ui_delay(cnt == 99 ? 40L : (long)cnt * 50L, FALSE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_highlight_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ get_highlight_name_ext(xp, idx, TRUE);
}

/*
 * Obtain a highlight group name.
 * When "skip_cleared" is TRUE don't return a cleared entry.
 */
    char_u *
get_highlight_name_ext(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ skip_cleared)
{
    ಯದಿ (idx < 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Items are never removed from the table, skip the ones that were
    // cleared.
    ಯದಿ (skip_cleared && idx < highlight_ga.ga_len && HL_TABLE()[idx].sg_cleared)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"";

    ಯದಿ (idx == highlight_ga.ga_len && include_none != 0)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"NONE";
    ಯದಿ (idx == highlight_ga.ga_len + include_none && include_default != 0)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"default";
    ಯದಿ (idx == highlight_ga.ga_len + include_none + include_default
							 && include_link != 0)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"link";
    ಯದಿ (idx == highlight_ga.ga_len + include_none + include_default + 1
							 && include_link != 0)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"clear";
    ಯದಿ (idx >= highlight_ga.ga_len)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ HL_TABLE()[idx].sg_name;
}

    ಸ್ಥಿರ char_u *
get_highlight_attr_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx == 0)
    {
	// Fill with current value first
	ಯದಿ (expand_hi_curvalue != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ expand_hi_curvalue;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ (char_u*)"";
    }
    ಯದಿ (idx < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(highlight_index_tab) + 1)
    {
	char_u *ಮೌಲ್ಯ = highlight_index_tab[idx-1]->ಮೌಲ್ಯ.string;
	ಯದಿ (expand_hi_curvalue != ರಿಕ್ತ && STRCMP(expand_hi_curvalue, ಮೌಲ್ಯ) == 0)
	{
	    // Already returned the current value above, just skip.
	    ಪ್ರತ್ಯಾಗಮ (char_u*)"";
	}
	ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ char_u *
get_highlight_cterm_color(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx == 0)
    {
	// Fill with current value first
	ಯದಿ (expand_hi_curvalue != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ expand_hi_curvalue;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ (char_u*)"";
    }
    // See highlight_set_cterm_color()
    ಅನ್ಯಥಾ ಯದಿ (idx == 1)
	ಪ್ರತ್ಯಾಗಮ (char_u*)"fg";
    ಅನ್ಯಥಾ ಯದಿ (idx == 2)
	ಪ್ರತ್ಯಾಗಮ (char_u*)"bg";
    ಯದಿ (idx < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(color_name_tab) + 3)
    {
	char_u *ಮೌಲ್ಯ = color_name_tab[idx-3].ಮೌಲ್ಯ.string;
	ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
    ಸ್ಥಿರ char_u *
get_highlight_gui_color(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx == 0)
    {
	// Fill with current value first
	ಯದಿ (expand_hi_curvalue != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ expand_hi_curvalue;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ (char_u*)"";
    }
    // See color_name2handle()
    ಅನ್ಯಥಾ ಯದಿ (idx == 1)
	ಪ್ರತ್ಯಾಗಮ (char_u*)"fg";
    ಅನ್ಯಥಾ ಯದಿ (idx == 2)
	ಪ್ರತ್ಯಾಗಮ (char_u*)"bg";
    ಅನ್ಯಥಾ ಯದಿ (idx == 3)
	ಪ್ರತ್ಯಾಗಮ (char_u*)"NONE";

    // Complete from v:colornames. Don't do platform specific names for now.
    typval_T *tv_result;
    char_u *colorname = dict_iterate_next(&expand_colornames_iter, &tv_result);
    ಯದಿ (colorname != ರಿಕ್ತ)
    {
	// :hi command doesn't allow space, so don't suggest any malformed items
	ಯದಿ (vim_strchr(colorname, ' ') != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ (char_u*)"";

	ಯದಿ (expand_hi_curvalue != ರಿಕ್ತ && STRICMP(expand_hi_curvalue, colorname) == 0)
	{
	    // Already returned the current value above, just skip.
	    ಪ್ರತ್ಯಾಗಮ (char_u*)"";
	}
    }
    ಪ್ರತ್ಯಾಗಮ colorname;
}
#endif

    ಸ್ಥಿರ char_u *
get_highlight_group_ಕೀಲಿ(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    // Note: Keep this in sync with do_highlight.
    ಸ್ಥಿರ ವರ್ಣ *(p_hi_group_ಕೀಲಿ_ಮೌಲ್ಯಗಳು[]) =
    {
	"term=",
	"start=",
	"stop=",
	"cterm=",
	"ctermfg=",
	"ctermbg=",
	"ctermul=",
	"ctermfont=",
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	"gui=",
	"guifg=",
	"guibg=",
	"guisp=",
#endif
#ifdef FEAT_GUI
	"font=",
#endif
	"NONE",
    };

    ಯದಿ (idx < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(p_hi_group_ಕೀಲಿ_ಮೌಲ್ಯಗಳು))
	ಪ್ರತ್ಯಾಗಮ (char_u*)p_hi_group_ಕೀಲಿ_ಮೌಲ್ಯಗಳು[idx];
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Command-line expansion for :hi {group-name} <args>...
 */
    ಪೂರ್ಣಾಙ್ಕ
expand_highlight_group(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	ಪೂರ್ಣಾಙ್ಕ	    *ಅಂಕೆ_Matches)
{
    ಯದಿ (expand_hi_equal_col != -1)
    {
	// List the values. First fill in the current value, then if possible colors
	// or attribute names.
	char_u	    *(*expandfunc)(expand_T *, ಪೂರ್ಣಾಙ್ಕ) = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ	    type = 0;
	hl_group_T  *sgp = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ	    iarg = 0;
	char_u	    *sarg = ರಿಕ್ತ;

	ಪೂರ್ಣಾಙ್ಕ	    unsorted_ವಸ್ತುಗಳು = -1; // don't sort by default

	ಯದಿ (expand_hi_synid != 0)
	    sgp = &HL_TABLE()[expand_hi_synid - 1]; // index is ID minus one

	// Note: Keep this in sync with highlight_list_one().
	char_u	    *name_end = xp->xp_ಸಾಲು + expand_hi_equal_col;
	ಯದಿ (name_end - xp->xp_ಸಾಲು >= 5
		&& STRNCMP(name_end - 5, " term", 5) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ಯದಿ (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_term;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (name_end - xp->xp_ಸಾಲು >= 6
		&& STRNCMP(name_end - 6, " cterm", 6) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ಯದಿ (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_cterm;
	    }
	}
#ಯದಿ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	ಅನ್ಯಥಾ ಯದಿ (name_end - xp->xp_ಸಾಲು >= 4
		&& STRNCMP(name_end - 4, " gui", 4) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ಯದಿ (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_gui;
	    }
	}
#endif
	ಅನ್ಯಥಾ ಯದಿ (name_end - xp->xp_ಸಾಲು >= 8
		&& STRNCMP(name_end - 8, " ctermfg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ಯದಿ (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_fg;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (name_end - xp->xp_ಸಾಲು >= 8
		&& STRNCMP(name_end - 8, " ctermbg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ಯದಿ (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_bg;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (name_end - xp->xp_ಸಾಲು >= 8
		&& STRNCMP(name_end - 8, " ctermul", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ಯದಿ (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_ul;
	    }
	}
#ಯದಿ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	ಅನ್ಯಥಾ ಯದಿ (name_end - xp->xp_ಸಾಲು >= 6
		&& STRNCMP(name_end - 6, " guifg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ಯದಿ (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_fg_name;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (name_end - xp->xp_ಸಾಲು >= 6
		&& STRNCMP(name_end - 6, " guibg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ಯದಿ (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_bg_name;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (name_end - xp->xp_ಸಾಲು >= 6
		&& STRNCMP(name_end - 6, " guisp", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ಯದಿ (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_sp_name;
	    }
	}
#endif

#ಯದಿ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	ಯದಿ (expandfunc == get_highlight_gui_color)
	{
	    // Top 4 items are special, after that sort all the color names
	    unsorted_ವಸ್ತುಗಳು = 4;

	    dict_T *colornames_table = get_vim_var_dict(VV_COLORNAMES);
	    typval_T colornames_val;
	    colornames_val.v_type = VAR_DICT;
	    colornames_val.vval.v_dict = colornames_table;
	    dict_iterate_start(&colornames_val, &expand_colornames_iter);
	}
#endif

	char_u	    buf[MAX_ATTR_LEN];

	expand_hi_curvalue = ರಿಕ್ತ;
	ಯದಿ (expand_hi_include_orig)
	{
	    ಯದಿ (((type == LIST_ATTR || type == LIST_INT) && iarg != 0) ||
		(type == LIST_STRING && sarg != ರಿಕ್ತ))
	    {
		// Retrieve the current value to go first in completion
		expand_hi_curvalue = highlight_arg_to_string(
			type, iarg, sarg, buf);
	    }
	}

	ಯದಿ (expandfunc != ರಿಕ್ತ)
	{
	    ಪ್ರತ್ಯಾಗಮ ExpandGenericExt(
		    pat,
		    xp,
		    rmp,
		    matches,
		    ಅಂಕೆ_Matches,
		    expandfunc,
		    FALSE,
		    unsorted_ವಸ್ತುಗಳು);
	}

	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // List all the key names
    ಪ್ರತ್ಯಾಗಮ ExpandGenericExt(
	    pat,
	    xp,
	    rmp,
	    matches,
	    ಅಂಕೆ_Matches,
	    get_highlight_group_ಕೀಲಿ,
	    FALSE,
	    -1);
}

#ಯದಿ defined(FEAT_GUI)
/*
 * Free all the highlight group fonts.
 * Used when quitting for systems which need it.
 */
    ಶೂನ್ಯ
free_highlight_fonts(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    idx;

    ಪ್ರತಿ (idx = 0; idx < highlight_ga.ga_len; ++idx)
    {
	gui_mch_free_font(HL_TABLE()[idx].sg_font);
	HL_TABLE()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
	gui_mch_free_fontset(HL_TABLE()[idx].sg_fontset);
	HL_TABLE()[idx].sg_fontset = NOFONTSET;
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
#endif

#ಯದಿ defined(FEAT_EVAL)
/*
 * Convert each of the highlight attribute bits (bold, standout, underline,
 * etc.) set in 'hlattr' into a separate boolean item in a Dictionary with
 * the attribute name as the key.
 */
    ಸ್ಥಿರ dict_T *
highlight_get_attr_dict(ಪೂರ್ಣಾಙ್ಕ hlattr)
{
    dict_T	*dict;
    ಪೂರ್ಣಾಙ್ಕ		i;

    dict = dict_alloc();
    ಯದಿ (dict == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(highlight_index_tab); ++i)
    {
	ಯದಿ (hlattr & highlight_index_tab[i]->ಕೀಲಿ)
	{
	    dict_add_bool(dict, (ವರ್ಣ *)highlight_index_tab[i]->ಮೌಲ್ಯ.string,
		    VVAL_TRUE);
	    // don't want "inverse"/"reverse"
	    hlattr &= ~highlight_index_tab[i]->ಕೀಲಿ;
	}
    }

    ಪ್ರತ್ಯಾಗಮ dict;
}

/*
 * Return the attributes of the highlight group at index 'hl_idx' as a
 * Dictionary. If 'resolve_link' is TRUE, then resolves the highlight group
 * links recursively.
 */
    ಸ್ಥಿರ dict_T *
highlight_get_info(ಪೂರ್ಣಾಙ್ಕ hl_idx, ಪೂರ್ಣಾಙ್ಕ resolve_link)
{
    dict_T	*dict;
    hl_group_T	*sgp;
    dict_T	*attr_dict = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		hlgid;

    dict = dict_alloc();
    ಯದಿ (dict == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ dict;

    sgp = &HL_TABLE()[hl_idx];
    // highlight group id is 1-based
    hlgid = hl_idx + 1;

    ಯದಿ (dict_add_string(dict, "name", sgp->sg_name) == FAIL)
	ಗಚ್ಛ error;
    ಯದಿ (dict_add_ಸಂಖ್ಯೆ(dict, "id", hlgid) == FAIL)
	ಗಚ್ಛ error;

    ಯದಿ (resolve_link)
    {
	// resolve the highlight group link recursively
	ಯಾವತ್ (sgp->sg_link)
	{
	    hlgid = sgp->sg_link;
	    sgp = &HL_TABLE()[sgp->sg_link - 1];
	}
    }

    ಯದಿ (sgp->sg_term != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_term);
	ಯದಿ (attr_dict != ರಿಕ್ತ)
	{
	    ಯದಿ (dict_add_dict(dict, "term", attr_dict) == FAIL)
		ಗಚ್ಛ error;
	    attr_dict = ರಿಕ್ತ;
	}
    }
    ಯದಿ (sgp->sg_start != ರಿಕ್ತ)
	ಯದಿ (dict_add_string(dict, "start", sgp->sg_start) == FAIL)
	    ಗಚ್ಛ error;
    ಯದಿ (sgp->sg_stop != ರಿಕ್ತ)
	ಯದಿ (dict_add_string(dict, "stop", sgp->sg_stop) == FAIL)
	    ಗಚ್ಛ error;
    ಯದಿ (sgp->sg_cterm != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_cterm);
	ಯದಿ (attr_dict != ರಿಕ್ತ)
	{
	    ಯದಿ (dict_add_dict(dict, "cterm", attr_dict) == FAIL)
		ಗಚ್ಛ error;
	    attr_dict = ರಿಕ್ತ;
	}
    }
    ಯದಿ (sgp->sg_cterm_fg != 0)
	ಯದಿ (dict_add_string(dict, "ctermfg",
		    highlight_color(hlgid, (char_u *)"fg", 'c')) == FAIL)
	    ಗಚ್ಛ error;
    ಯದಿ (sgp->sg_cterm_bg != 0)
	ಯದಿ (dict_add_string(dict, "ctermbg",
			highlight_color(hlgid, (char_u *)"bg", 'c')) == FAIL)
	    ಗಚ್ಛ error;
    ಯದಿ (sgp->sg_cterm_ul != 0)
	ಯದಿ (dict_add_string(dict, "ctermul",
			highlight_color(hlgid, (char_u *)"ul", 'c')) == FAIL)
	    ಗಚ್ಛ error;
    ಯದಿ (sgp->sg_cterm_font != 0)
	ಯದಿ (dict_add_string(dict, "ctermfont",
			highlight_color(hlgid, (char_u *)"font", 'c')) == FAIL)
	    ಗಚ್ಛ error;
    ಯದಿ (sgp->sg_gui != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_gui);
	ಯದಿ (attr_dict != ರಿಕ್ತ)
	{
	    ಯದಿ (dict_add_dict(dict, "gui", attr_dict) == FAIL)
		ಗಚ್ಛ error;
	    attr_dict = ರಿಕ್ತ;
	}
    }
    ಯದಿ (sgp->sg_gui_fg_name != ರಿಕ್ತ)
	ಯದಿ (dict_add_string(dict, "guifg",
			highlight_color(hlgid, (char_u *)"fg", 'g')) == FAIL)
	    ಗಚ್ಛ error;
    ಯದಿ (sgp->sg_gui_bg_name != ರಿಕ್ತ)
	ಯದಿ (dict_add_string(dict, "guibg",
			highlight_color(hlgid, (char_u *)"bg", 'g')) == FAIL)
	    ಗಚ್ಛ error;
    ಯದಿ (sgp->sg_gui_sp_name != ರಿಕ್ತ)
	ಯದಿ (dict_add_string(dict, "guisp",
			highlight_color(hlgid, (char_u *)"sp", 'g')) == FAIL)
	    ಗಚ್ಛ error;
# ifdef FEAT_GUI
    ಯದಿ (sgp->sg_font_name != ರಿಕ್ತ)
	ಯದಿ (dict_add_string(dict, "font", sgp->sg_font_name) == FAIL)
	    ಗಚ್ಛ error;
# endif
    ಯದಿ (sgp->sg_link)
    {
	char_u	*link;

	link = HL_TABLE()[sgp->sg_link - 1].sg_name;
	ಯದಿ (link != ರಿಕ್ತ && dict_add_string(dict, "linksto", link) == FAIL)
	    ಗಚ್ಛ error;

	ಯದಿ (sgp->sg_deflink)
	    dict_add_bool(dict, "default", VVAL_TRUE);
    }
    ಯದಿ (dict_len(dict) == 2)
	// If only 'name' is present, then the highlight group is cleared.
	dict_add_bool(dict, "cleared", VVAL_TRUE);

    ಪ್ರತ್ಯಾಗಮ dict;

error:
    dict_unref(attr_dict);
    dict_unref(dict);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * "hlget([name])" function
 * Return the attributes of a specific highlight group (if specified) or all
 * the highlight groups.
 */
    ಶೂನ್ಯ
f_hlget(typval_T *argvars, typval_T *rettv)
{
    list_T	*list;
    dict_T	*dict;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*hlarg = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		resolve_link = FALSE;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_opt_string_arg(argvars, 0) == FAIL
	    || (argvars[0].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// highlight group name supplied
	hlarg = tv_get_string_chk(&argvars[0]);
	ಯದಿ (hlarg == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    ಪೂರ್ಣಾಙ್ಕ error = FALSE;

	    resolve_link = tv_get_bool_chk(&argvars[1], &error);
	    ಯದಿ (error)
		ಪ್ರತ್ಯಾಗಮ;
	}
    }

    list = rettv->vval.v_list;
    ಪ್ರತಿ (i = 0; i < highlight_ga.ga_len && !got_int; ++i)
    {
	ಯದಿ (hlarg == ರಿಕ್ತ || STRICMP(hlarg, HL_TABLE()[i].sg_name) == 0)
	{
	    dict = highlight_get_info(i, resolve_link);
	    ಯದಿ (dict != ರಿಕ್ತ && list_append_dict(list, dict) == FAIL)
		dict_unref(dict);
	}
    }
}

/*
 * Returns the string value at 'dict[key]'. Returns NULL, if 'key' is not in
 * 'dict' or the value is not a string type. If the value is not a string type
 * or is NULL, then 'error' is set to TRUE.
 */
    ಸ್ಥಿರ char_u *
hldict_get_string(dict_T *dict, char_u *ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ *error)
{
    dictitem_T	*di;

    *error = FALSE;
    di = dict_find(dict, ಕೀಲಿ, -1);
    ಯದಿ (di == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (di->di_tv.v_type != VAR_STRING || di->di_tv.vval.v_string == ರಿಕ್ತ)
    {
	emsg(_(e_string_required));
	*error = TRUE;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ di->di_tv.vval.v_string;
}

/*
 * Convert the highlight attribute Dictionary at 'dict[key]' into a string
 * value in 'attr_str' of length 'len'. Returns FALSE if 'dict[key]' is not a
 * Dictionary or is NULL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
hldict_attr_to_str(
	dict_T	*dict,
	char_u	*ಕೀಲಿ,
	char_u	*attr_str,
	size_t	len)
{
    dictitem_T	*di;
    dict_T	*attrdict;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*p;

    attr_str[0] = NUL;
    di = dict_find(dict, ಕೀಲಿ, -1);
    ಯದಿ (di == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ಯದಿ (di->di_tv.v_type != VAR_DICT || di->di_tv.vval.v_dict == ರಿಕ್ತ)
    {
	emsg(_(e_dictionary_required));
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    attrdict = di->di_tv.vval.v_dict;

    // If the attribute dict is empty, then return NONE to clear the attributes
    ಯದಿ (dict_len(attrdict) == 0)
    {
	vim_strcat(attr_str, (char_u *)"NONE", len);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    p = attr_str;
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(highlight_tab); ++i)
    {
	ಯದಿ (dict_get_bool(attrdict, (ವರ್ಣ *)highlight_tab[i].ಮೌಲ್ಯ.string,
		    VVAL_FALSE) == VVAL_TRUE)
	{
	    ಯದಿ (p != attr_str && (size_t)(p - attr_str + 2) < len)
		STRCPY(p, (char_u *)",");
	    ಯದಿ (p - attr_str + highlight_tab[i].ಮೌಲ್ಯ.length + 1 < len)
	    {
		STRCPY(p, highlight_tab[i].ಮೌಲ್ಯ.string);
		p += highlight_tab[i].ಮೌಲ್ಯ.length;
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

// Temporary buffer used to store the command string produced by hlset().
// IObuff cannot be used for this as the error messages produced by hlset()
// internally use IObuff.
# define	HLSETBUFSZ  512
ಸ್ಥಿರ char_u hlsetBuf[HLSETBUFSZ + 1];

/*
 * Add the highlight attribute "attr" of length "attrlen" and "value" at
 * "dptr", which points into "hlsetBuf".
 * Returns the updated pointer.
 */
    ಸ್ಥಿರ char_u *
add_attr_and_ಮೌಲ್ಯ(char_u *dptr, char_u *attr, ಪೂರ್ಣಾಙ್ಕ attrlen, char_u *ಮೌಲ್ಯ)
{
    size_t	vallen;

    // Do nothing if the value is not specified or is empty
    ಯದಿ (ಮೌಲ್ಯ == ರಿಕ್ತ || *ಮೌಲ್ಯ == NUL)
	ಪ್ರತ್ಯಾಗಮ dptr;

    vallen = STRLEN(ಮೌಲ್ಯ);
    // When the value contains a space and the attribute has an "=" (i.e. it
    // is a key=value pair), surround the value with single quotes so that
    // do_highlight() can parse it correctly.
    ಯದಿ (vim_strchr(ಮೌಲ್ಯ, ' ') != ರಿಕ್ತ && vim_strchr(attr, '=') != ರಿಕ್ತ)
    {
	ಯದಿ (dptr + attrlen + vallen + 3 < hlsetBuf + HLSETBUFSZ)
	{
	    STRCPY(dptr, attr);
	    dptr += attrlen;
	    *dptr++ = '\'';
	    STRCPY(dptr, ಮೌಲ್ಯ);
	    dptr += vallen;
	    *dptr++ = '\'';
	    *dptr = NUL;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (dptr + attrlen + vallen + 1 < hlsetBuf + HLSETBUFSZ)
    {
	STRCPY(dptr, attr);
	dptr += attrlen;
	STRCPY(dptr, ಮೌಲ್ಯ);
	dptr += vallen;
    }

    ಪ್ರತ್ಯಾಗಮ dptr;
}

/*
 * Add or update a highlight group using 'dict' items. Returns TRUE if
 * successfully updated the highlight group.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
hlg_add_or_update(dict_T *dict)
{
    char_u	*name;
    ಪೂರ್ಣಾಙ್ಕ		error;
    char_u	term_attr[MAX_ATTR_LEN];
    char_u	cterm_attr[MAX_ATTR_LEN];
    char_u	gui_attr[MAX_ATTR_LEN];
    char_u	*start;
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
    ಪೂರ್ಣಾಙ್ಕ		forceit = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		dodefault = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		done = FALSE;
    char_u	*p;

    name = hldict_get_string(dict, (char_u *)"name", &error);
    ಯದಿ (name == ರಿಕ್ತ || *name == NUL || error)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (dict_get_bool(dict, "force", VVAL_FALSE) == VVAL_TRUE)
	forceit = TRUE;

    ಯದಿ (dict_get_bool(dict, "default", VVAL_FALSE) == VVAL_TRUE)
	dodefault = TRUE;

    ಯದಿ (dict_has_ಕೀಲಿ(dict, "cleared"))
    {
	varnumber_T	cleared;

	// clear a highlight group
	cleared = dict_get_bool(dict, "cleared", FALSE);
	ಯದಿ (cleared == TRUE)
	{
	    vim_snprintf((ವರ್ಣ *)hlsetBuf, HLSETBUFSZ, "clear %s", name);
	    do_highlight(hlsetBuf, forceit, FALSE);
	    done = TRUE;
	}
    }

    ಯದಿ (dict_has_ಕೀಲಿ(dict, "linksto"))
    {
	char_u	*linksto;

	// link highlight groups
	linksto = hldict_get_string(dict, (char_u *)"linksto", &error);
	ಯದಿ (linksto == ರಿಕ್ತ || *linksto == NUL || error)
	    ಪ್ರತ್ಯಾಗಮ FALSE;

	vim_snprintf((ವರ್ಣ *)hlsetBuf, HLSETBUFSZ, "%slink %s %s",
				dodefault ? "default " : "", name, linksto);
	do_highlight(hlsetBuf, forceit, FALSE);

	done = TRUE;
    }

    // If 'cleared' or 'linksto' are specified, then don't process the other
    // attributes.
    ಯದಿ (done)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    start = hldict_get_string(dict, (char_u *)"start", &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    stop = hldict_get_string(dict, (char_u *)"stop", &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (!hldict_attr_to_str(dict, (char_u *)"term", term_attr,
							    ಪರಿಮಾಣ(term_attr)))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (!hldict_attr_to_str(dict, (char_u *)"cterm", cterm_attr,
							   ಪರಿಮಾಣ(cterm_attr)))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ctermfg = hldict_get_string(dict, (char_u *)"ctermfg", &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ctermbg = hldict_get_string(dict, (char_u *)"ctermbg", &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ctermul = hldict_get_string(dict, (char_u *)"ctermul", &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ctermfont = hldict_get_string(dict, (char_u *)"ctermfont", &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (!hldict_attr_to_str(dict, (char_u *)"gui", gui_attr, ಪರಿಮಾಣ(gui_attr)))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    guifg = hldict_get_string(dict, (char_u *)"guifg", &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    guibg = hldict_get_string(dict, (char_u *)"guibg", &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    guisp = hldict_get_string(dict, (char_u *)"guisp", &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ FALSE;

# ifdef FEAT_GUI
    font = hldict_get_string(dict, (char_u *)"font", &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ FALSE;
# endif

    // If none of the attributes are specified, then do nothing.
    ಯದಿ (term_attr[0] == NUL && start == ರಿಕ್ತ && stop == ರಿಕ್ತ
	    && cterm_attr[0] == NUL && ctermfg == ರಿಕ್ತ && ctermbg == ರಿಕ್ತ
	    && ctermul == ರಿಕ್ತ && ctermfont == ರಿಕ್ತ && gui_attr[0] == NUL
# ifdef FEAT_GUI
	    && font == ರಿಕ್ತ
# endif
	    && guifg == ರಿಕ್ತ && guibg == ರಿಕ್ತ && guisp == ರಿಕ್ತ
	    )
	ಪ್ರತ್ಯಾಗಮ TRUE;

    hlsetBuf[0] = NUL;
    p = hlsetBuf;
    ಯದಿ (dodefault)
	p = add_attr_and_ಮೌಲ್ಯ(p, (char_u *)"default", 7, (char_u *)" ");
    p = add_attr_and_ಮೌಲ್ಯ(p, (char_u *)"", 0, name);
    p = add_attr_and_ಮೌಲ್ಯ(p, (char_u *)" term=", 6, term_attr);
    p = add_attr_and_ಮೌಲ್ಯ(p, (char_u *)" start=", 7, start);
    p = add_attr_and_ಮೌಲ್ಯ(p, (char_u *)" stop=", 6, stop);
    p = add_attr_and_ಮೌಲ್ಯ(p, (char_u *)" cterm=", 7, cterm_attr);
    p = add_attr_and_ಮೌಲ್ಯ(p, (char_u *)" ctermfg=", 9, ctermfg);
    p = add_attr_and_ಮೌಲ್ಯ(p, (char_u *)" ctermbg=", 9, ctermbg);
    p = add_attr_and_ಮೌಲ್ಯ(p, (char_u *)" ctermul=", 9, ctermul);
    p = add_attr_and_ಮೌಲ್ಯ(p, (char_u *)" ctermfont=", 11, ctermfont);
    p = add_attr_and_ಮೌಲ್ಯ(p, (char_u *)" gui=", 5, gui_attr);
# ifdef FEAT_GUI
    p = add_attr_and_ಮೌಲ್ಯ(p, (char_u *)" font=", 6, font);
# endif
    p = add_attr_and_ಮೌಲ್ಯ(p, (char_u *)" guifg=", 7, guifg);
    p = add_attr_and_ಮೌಲ್ಯ(p, (char_u *)" guibg=", 7, guibg);
    (ಶೂನ್ಯ)add_attr_and_ಮೌಲ್ಯ(p, (char_u *)" guisp=", 7, guisp);

    do_highlight(hlsetBuf, forceit, FALSE);

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * "hlset([{highlight_attr}])" function
 * Add or modify highlight groups
 */
    ಶೂನ್ಯ
f_hlset(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    dict_T	*dict;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (check_for_list_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(argvars->vval.v_list, li)
    {
	ಯದಿ (li->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_dictionary_required));
	    ಪ್ರತ್ಯಾಗಮ;
	}

	dict = li->li_tv.vval.v_dict;
	ಯದಿ (!hlg_add_or_update(dict))
	    ಪ್ರತ್ಯಾಗಮ;
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
}
#endif

/*
 * If "old" is in the override stack, then update it to "hl_new". Does not free
 * "old".
 */
    ಶೂನ್ಯ
update_highlight_overrides(hl_override_T *old, hl_override_T *hl_new, ಪೂರ್ಣಾಙ್ಕ newlen)
{
    ಯದಿ (old == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (hl_overrides_T *set = overrides; set != ರಿಕ್ತ; set = set->next)
    {
	ಯದಿ (set->arr == old)
	{
	    set->arr = hl_new;
	    set->len = newlen;
	}
    }
}

/*
 * If "arr" is in the highlight overrides list, then mark it as invalid.
 */
    ಶೂನ್ಯ
remove_highlight_overrides(hl_override_T *arr)
{
    ಯದಿ (arr == ರಿಕ್ತ || overrides == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (hl_overrides_T *set = overrides; set != ರಿಕ್ತ; set = set->next)
    {
	ಯದಿ (set->arr == arr)
	{
	    set->arr = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	}
    }
}

/*
 * Update highlight_attr[] array. If "update_ids" is true, then update
 * highlight_ids[] array instead.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_highlight_attr(hl_override_T *arr, ಪೂರ್ಣಾಙ್ಕ len, bool update_ids)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < len; i++)
    {
	hl_override_T	*override = arr + i;
	ಪೂರ್ಣಾಙ್ಕ		hlf = -1, attr;

	ಯದಿ (override->from <= 0)
	    // Directly map to highlight_attr
	    hlf = -override->from;
	ಅನ್ಯಥಾ
	{
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < HLF_ಎಣಿಕೆ; k++)
		ಯದಿ (override->from == highlight_ids[k])
		{
		    hlf = k;
		    ಭಙ್ಗ;
		}

	    ಯದಿ (hlf == -1)
		ಅನುವರ್ತ;
	}

	ಯದಿ (update_ids)
	{
	    ಯದಿ (override->to <= 0)
	    {
		ಯದಿ (hlf == -override->to)
		    highlight_ids[hlf] = -1;
		ಅನ್ಯಥಾ
		    highlight_ids[hlf] = highlight_ids[-override->to];
	    }
	    ಅನ್ಯಥಾ
		highlight_ids[hlf] = override->to;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (override->to <= 0)
		// Directly use highlight_attr[]
		attr = highlight_attr[-override->to];
	    ಅನ್ಯಥಾ
		attr = syn_id2attr(override->to);

	    highlight_attr[hlf] = attr;
	}
    }
}

/*
 * Set the current highlight override to "arr". Returns true if successful
 */
    bool
ತಳ್ಳು_highlight_overrides(hl_override_T *arr, ಪೂರ್ಣಾಙ್ಕ len)
{
    // Don't want to do anything if "arr" is already the current override. If
    // "arr" is NULL (but overrides->arr is not), then still push an override,
    // but "->arr" will just be NULL so any previous overrides are cleared.
    ಯದಿ (overrides != ರಿಕ್ತ && overrides->arr == arr)
	ಪ್ರತ್ಯಾಗಮ false;

    hl_overrides_T *set;

    set = ALLOC_ONE(hl_overrides_T);
    ಯದಿ (set == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ false;

    set->arr = arr;
    set->len = len;

    // Push to front of list
    set->next = overrides;
    overrides = set;

    // Save current highlight_attr[], so it can be restored later after this
    // override is popped.
    memcpy(set->attr, highlight_attr, ಪರಿಮಾಣ(highlight_attr));

    // Reset highlight_attr[] to default values, as we don't want changes from
    // other overrides.
    memcpy(highlight_attr, highlight_attr_raw, ಪರಿಮಾಣ(highlight_attr));

    // Update highlight_attr[] array
    ಯದಿ (arr != ರಿಕ್ತ)
	set_highlight_attr(arr, len, false);

    ಪ್ರತ್ಯಾಗಮ true;
}

/*
 * Pop the current highlight override (if any) from the stack. This should
 * always be paired with a *successful* push.
 */
    ಶೂನ್ಯ
ತೆಗೆ_highlight_overrides(ಶೂನ್ಯ)
{
    hl_overrides_T *set = overrides;

    ಯದಿ (overrides == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    overrides = set->next;

    // Set highlight_attr[] to state before override was pushed.
    memcpy(highlight_attr, set->attr, ಪರಿಮಾಣ(highlight_attr));
    vim_free(set);
}

/*
 * Parse the 'winhighlight' option and return array. Returns NULL on failure or
 * if empty option. If failure, then errmsg is set.
 */
    ಸ್ಥಿರ hl_override_T *
parse_winhighlight(char_u *opt, ಪೂರ್ಣಾಙ್ಕ *len, ವರ್ಣ **errmsg)
{
    char_u	    *p = opt;
    hl_override_T   *arr;
    ಪೂರ್ಣಾಙ್ಕ		    i = 0;
    ಪೂರ್ಣಾಙ್ಕ		    ಅಂಕೆ = 1;
    ಪೂರ್ಣಾಙ್ಕ		    n_colons = 0;

    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Get number of overrides first so we can allocate array
    ಯಾವತ್ ((p = vim_strchr(p, ',')) != ರಿಕ್ತ)
    {
	p++;
	ಅಂಕೆ++;
    }
    p = opt;
    // Check if number of ':' matches number of ','
    ಯಾವತ್ ((p = vim_strchr(p, ':')) != ರಿಕ್ತ)
    {
	p++;
	n_colons++;
    }

    ಯದಿ (ಅಂಕೆ != n_colons)
    {
	*errmsg = e_invalid_argument;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    arr = ALLOC_MULT(hl_override_T, ಅಂಕೆ);
    ಯದಿ (arr == ರಿಕ್ತ)
    {
	*errmsg = e_out_of_memory;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    p = opt;

    ಯಾವತ್ (true)
    {
	hl_override_T *override = arr + i++;
	char_u	*fromname = p, *toname;
	char_u	*tmp;
	char_u	**names[2] = {&fromname, &toname};
	ಪೂರ್ಣಾಙ್ಕ	fromlen, tolen;
	ಪೂರ್ಣಾಙ್ಕ	*lens[2] = {&fromlen, &tolen};
	ಪೂರ್ಣಾಙ್ಕ	fromid, toid;
	ಪೂರ್ಣಾಙ್ಕ	*ids[2] = {&fromid, &toid};

	p = vim_strchr(p, ':');

	ಯದಿ (p == ರಿಕ್ತ)
	    ಗಚ್ಛ fail;

	fromlen = p - fromname; // Get hl for "from"
	ಯದಿ (fromlen == 0)
	    ಗಚ್ಛ fail;
	p++; // Skip colon ':'
	ಯದಿ (*p == NUL)
	    ಗಚ್ಛ fail;

	toname = p;
	tmp = vim_strchr(p, ',');

	// Get hl for "to", must check for no trailing comma in case last
	// element.
	ಯದಿ (tmp == ರಿಕ್ತ)
	    tolen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);
	ಅನ್ಯಥಾ
	{
	    tolen = tmp - toname;
	    p = ++tmp;
	}
	ಯದಿ (tolen == 0)
	    ಗಚ್ಛ fail;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < 2; k++)
	{
	    char_u  *name = *names[k];
	    ಪೂರ್ಣಾಙ್ಕ	    nlen = *lens[k];

	    ಯದಿ (*name == '!')
	    {
		// If starts with "!", then it maps directly to a 'highlight'
		// occasion.
		ಪೂರ್ಣಾಙ್ಕ hlf;

		ಯದಿ (nlen != 2)
		    ಗಚ್ಛ fail;

		ಪ್ರತಿ (hlf = 0; hlf < (ಪೂರ್ಣಾಙ್ಕ)HLF_ಎಣಿಕೆ; ++hlf)
		    ಯದಿ (hl_flags[hlf] == name[1])
			ಭಙ್ಗ;

		ಯದಿ (hlf >= HLF_ಎಣಿಕೆ)
		    ಗಚ್ಛ fail;

		*ids[k] = -hlf;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Otherwise get the highlight group id.
		ಯದಿ (syn_check_group(name, nlen) == 0)
		    ಗಚ್ಛ fail;

		*ids[k] = syn_namen2id(name, nlen);
		ಯದಿ (*ids[k] == 0)
		    ಗಚ್ಛ fail;

		// Always map "Normal" group to HLF_WIN
		ಯದಿ (ids[k] == &fromid &&
			STRCMP(HL_TABLE()[*ids[k] - 1].sg_name_u, "NORMAL") == 0)
		    *ids[k] = -HLF_WIN;
	    }
	}

	// If fromid == toid, leave it be, this is so that we know HLF_WIN has
	// been set (even to itself e.g. "Normal"), and therefore know to use
	// its attr instead.
	override->from = fromid;
	override->to = toid;

	ಯದಿ (tmp == ರಿಕ್ತ)
	    ಭಙ್ಗ;
    }

    *len = ಅಂಕೆ;
    ಪ್ರತ್ಯಾಗಮ arr;
fail:
    vim_free(arr);
    *errmsg = e_invalid_argument;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Update w_hl for "wp" using given option value. Returns error
 * message on failure, otherwise NULL.
 */
    ವರ್ಣ *
update_winhighlight(win_T *wp, char_u *opt)
{
    ವರ್ಣ	    *err = ರಿಕ್ತ;
    hl_override_T   *arr;
    ಪೂರ್ಣಾಙ್ಕ		    ಅಂಕೆ = 1;

    arr = parse_winhighlight(opt, &ಅಂಕೆ, &err);

    ಯದಿ (arr == ರಿಕ್ತ && err != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ err;

    ಯದಿ (arr == ರಿಕ್ತ && wp->w_hl == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    update_highlight_overrides(wp->w_hl, arr, ಅಂಕೆ);

    vim_free(wp->w_hl);
    wp->w_hl = arr;
    wp->w_hl_len = ಅಂಕೆ;

    wp->w_hlfwin_id = hlf_get_id(wp, HLF_WIN);

#ifdef FEAT_TERMINAL
    term_update_hlfwin(wp);
#endif

#ifdef FEAT_TERMINAL
    // May be NULL (such as in after_copy_winopt())
    ಯದಿ (wp->w_buffer != ರಿಕ್ತ)
    {
	// Make sure terminal highlighting is updated
	bool ret = ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);

	ಯದಿ (wp->w_buffer->b_term != ರಿಕ್ತ)
	    term_init_default_colors(wp->w_buffer->b_term);
	ಯದಿ (ret)
	    ತೆಗೆ_highlight_overrides();
    }
#endif

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Get id that the hlf attr is using in window "wp", otherwise zero. Note that
 * -1 may be returned if the hlf was set to itself.
 */
    ಪೂರ್ಣಾಙ್ಕ
hlf_get_id(win_T *wp, ಪೂರ್ಣಾಙ್ಕ hlf)
{
    ಪೂರ್ಣಾಙ್ಕ id;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prev[HLF_ಎಣಿಕೆ];

    ಯದಿ (wp->w_hl == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ highlight_ids[hlf];

    memcpy(prev, highlight_ids, ಪರಿಮಾಣ(highlight_ids));
    set_highlight_attr(wp->w_hl, wp->w_hl_len, true);
    id = highlight_ids[hlf];
    memcpy(highlight_ids, prev, ಪರಿಮಾಣ(highlight_ids));

    ಪ್ರತ್ಯಾಗಮ id;
}

/*
 * Update 'winhighlight' using a 'wincolor' style option value. Returns error
 * message on failure otherwise NULL. Does not update the actual 'wincolor'
 * value, but does update the 'winhighlight' value.
 */
    ವರ್ಣ *
update_wincolor(win_T *wp, char_u *opt)
{
    char_u  *str = *opt == NUL ? "" : alloc(ಪರಿಮಾಣ("!(:") + STRLEN(opt));
    ವರ್ಣ    *errmsg;

    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ e_out_of_memory;

    ಯದಿ (*opt != NUL)
	sprintf((ವರ್ಣ *)str, "!(:%s", opt);

    errmsg = update_winhighlight(wp, str);

    ಯದಿ (errmsg == ರಿಕ್ತ)
	set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		str, OPT_FREE|OPT_LOCAL, 0);

    ಯದಿ (*opt != NUL)
	ಮುಕ್ತ(str);
    ಪ್ರತ್ಯಾಗಮ errmsg;
}
