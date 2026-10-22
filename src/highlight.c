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

#അന്തര്ഭാവ "vim.h"

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
സ്ഥിതം keyvalue_T highlight_tab[] = {
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
സ്ഥിതം keyvalue_T *highlight_index_tab[] = {
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

എണ്ണൽ {
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
സ്ഥിതം keyvalue_T color_name_tab[] = {
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
തരനിർവചനം ഘടന
{
    char_u	*sg_name;	// highlight group name
    char_u	*sg_name_u;	// uppercase of sg_name
    പൂർണ്ണസംഖ്യ		sg_cleared;	// "hi clear" was used
// for normal terminals
    പൂർണ്ണസംഖ്യ		sg_term;	// "term=" highlighting attributes
    char_u	*sg_start;	// terminal string for start highl
    char_u	*sg_stop;	// terminal string for stop highl
    പൂർണ്ണസംഖ്യ		sg_term_attr;	// Screen attr for term mode
// for color terminals
    പൂർണ്ണസംഖ്യ		sg_cterm;	// "cterm=" highlighting attr
    പൂർണ്ണസംഖ്യ		sg_cterm_bold;	// bold attr was set for light color
    പൂർണ്ണസംഖ്യ		sg_cterm_fg;	// terminal fg color number + 1
    പൂർണ്ണസംഖ്യ		sg_cterm_bg;	// terminal bg color number + 1
    പൂർണ്ണസംഖ്യ		sg_cterm_ul;	// terminal ul color number + 1
    പൂർണ്ണസംഖ്യ		sg_cterm_attr;	// Screen attr for color term mode
    പൂർണ്ണസംഖ്യ		sg_cterm_font;	// terminal alternative font (0 for normal)
// for when using the GUI
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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
    പൂർണ്ണസംഖ്യ		sg_gui_attr;    // Screen attr for GUI mode
#endif
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_EVAL)
// Store the sp color name for the GUI or synIDattr()
    പൂർണ്ണസംഖ്യ		sg_gui;		// "gui=" highlighting attributes
    char_u	*sg_gui_fg_name;// GUI foreground color name
    char_u	*sg_gui_bg_name;// GUI background color name
    char_u	*sg_gui_sp_name;// GUI special color name
#endif
    പൂർണ്ണസംഖ്യ		sg_link;	// link to this highlight group ID
    പൂർണ്ണസംഖ്യ		sg_deflink;	// default link; restored in highlight_clear()
    പൂർണ്ണസംഖ്യ		sg_set;		// combination of SG_* flags
#ifdef FEAT_EVAL
    sctx_T	sg_deflink_sctx;  // script where the default link was set
    sctx_T	sg_script_ctx;	// script in which the group was last set
#endif
} hl_group_T;

തരനിർവചനം ഘടന hl_overrides_S hl_overrides_T;
ഘടന hl_overrides_S
{
    hl_override_T   *arr;   // May be NULL if "arr" was freed or no highlight
			    // overrides (all values set to default)
    പൂർണ്ണസംഖ്യ		    len;
    hl_overrides_T  *next;  // Used to handle recursive calls

    പൂർണ്ണസംഖ്യ attr[HLF_എണ്ണം];    // highlight_attr[] before being updated.
};

സ്ഥിതം hl_overrides_T *overrides = ശൂന്യ;

// Synced with highlight_attr, each is the highlight group id for each attr. If
// an element is 0, then there is no attr for it. If an element is -1, then it
// means it was set to itself.
സ്ഥിതം പൂർണ്ണസംഖ്യ highlight_ids[HLF_എണ്ണം];

// Same as highlight_attr[] but is not modified by highlight group overrides
സ്ഥിതം പൂർണ്ണസംഖ്യ highlight_attr_raw[HLF_എണ്ണം];

സ്ഥിതം പൂർണ്ണസംഖ്യ hl_flags[HLF_എണ്ണം] = HL_FLAGS;

// highlight groups for 'highlight' option
സ്ഥിതം garray_T highlight_ga;
#define HL_TABLE()	((hl_group_T *)((highlight_ga.ga_data)))

// Wrapper struct for hashtable entries: stores the highlight group ID alongside
// the uppercase name used as a hash key.  Uses the same offsetof pattern as
// signgroup_T / HI2SG.
തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ		hn_id;		// highlight group ID (1-based)
    char_u	hn_താക്കോൽ[1];	// uppercase name (stretchable array)
} hlname_T;

#define HLNAME_താക്കോൽ_OFF	offsetof(hlname_T, hn_താക്കോൽ)
#define HI2HLNAME(hi)	((hlname_T *)((hi)->hi_താക്കോൽ - HLNAME_താക്കോൽ_OFF))

// hashtable for quick highlight group name lookup
സ്ഥിതം hashtab_T highlight_ht;
സ്ഥിതം bool	 highlight_ht_inited = false;

/*
 * An attribute number is the index in attr_table plus ATTR_OFF.
 */
#define ATTR_OFF (HL_ALL + 1)

സ്ഥിതം ശൂന്യം syn_unadd_group(ശൂന്യം);
സ്ഥിതം ശൂന്യം set_hl_attr(പൂർണ്ണസംഖ്യ idx);
സ്ഥിതം ശൂന്യം highlight_list_one(പൂർണ്ണസംഖ്യ id);
സ്ഥിതം പൂർണ്ണസംഖ്യ highlight_list_arg(പൂർണ്ണസംഖ്യ id, പൂർണ്ണസംഖ്യ didh, പൂർണ്ണസംഖ്യ type, പൂർണ്ണസംഖ്യ iarg, char_u *sarg, അക്ഷരം *name);
സ്ഥിതം പൂർണ്ണസംഖ്യ syn_add_group(char_u *name);
സ്ഥിതം പൂർണ്ണസംഖ്യ syn_name2id_len(char_u *name, പൂർണ്ണസംഖ്യ len);
സ്ഥിതം പൂർണ്ണസംഖ്യ hl_has_settings(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ check_link);
സ്ഥിതം ശൂന്യം highlight_clear(പൂർണ്ണസംഖ്യ idx);

#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
സ്ഥിതം ശൂന്യം gui_do_one_color(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ do_menu, പൂർണ്ണസംഖ്യ do_tooltip);
#endif
#ifdef FEAT_GUI
സ്ഥിതം പൂർണ്ണസംഖ്യ  set_group_colors(char_u *name, guicolor_T *fgp, guicolor_T *bgp, പൂർണ്ണസംഖ്യ do_menu, പൂർണ്ണസംഖ്യ use_norm, പൂർണ്ണസംഖ്യ do_tooltip);
സ്ഥിതം ശൂന്യം hl_do_font(പൂർണ്ണസംഖ്യ idx, char_u *arg, പൂർണ്ണസംഖ്യ do_normal, പൂർണ്ണസംഖ്യ do_menu, പൂർണ്ണസംഖ്യ do_tooltip, പൂർണ്ണസംഖ്യ free_font);
#endif

/*
 * The default highlight groups.  These are compiled-in for fast startup and
 * they still work when the runtime files can't be found.
 * When making changes here, also change runtime/colors/default.vim!
 * The #ifdefs are needed to reduce the amount of static data.  Helps to make
 * the 16 bit DOS (museum) version compile.
 */
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_EVAL)
# define CENT(a, b) b
#അല്ലെങ്കിൽ
# define CENT(a, b) a
#endif
സ്ഥിതം അക്ഷരം *(highlight_init_both[]) = {
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
    ശൂന്യ
};

// Default colors only used with a light background.
സ്ഥിതം അക്ഷരം *(highlight_init_light[]) = {
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
    ശൂന്യ
};

// Default colors only used with a dark background.
സ്ഥിതം അക്ഷരം *(highlight_init_dark[]) = {
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
    ശൂന്യ
};

#എങ്കിൽ defined(FEAT_SYN_HL)
/*
 * Returns the number of highlight groups.
 */
    പൂർണ്ണസംഖ്യ
highlight_അക്കം_groups(ശൂന്യം)
{
    തിരികെ highlight_ga.ga_len;
}

/*
 * Returns the name of a highlight group.
 */
    char_u *
highlight_group_name(പൂർണ്ണസംഖ്യ id)
{
    തിരികെ HL_TABLE()[id].sg_name;
}

/*
 * Returns the ID of the link to a highlight group.
 */
    പൂർണ്ണസംഖ്യ
highlight_link_id(പൂർണ്ണസംഖ്യ id)
{
    തിരികെ HL_TABLE()[id].sg_link;
}
#endif

സ്ഥിതം ശൂന്യം resolve_fallback_fg_to_rgb(ശൂന്യം);
സ്ഥിതം ശൂന്യം resolve_fallback_bg_to_rgb(ശൂന്യം);

    ശൂന്യം
init_highlight(
    പൂർണ്ണസംഖ്യ		both,	    // include groups where 'bg' doesn't matter
    പൂർണ്ണസംഖ്യ		reset)	    // clear group first
{
    പൂർണ്ണസംഖ്യ		i;
    അക്ഷരം	**pp;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	had_both = FALSE;
#ifdef FEAT_EVAL
    char_u	*p;

    // Try finding the color scheme file.  Used when a color file was loaded
    // and 'background' or 't_Co' is changed.
    p = get_var_മൂല്യം((char_u *)"g:colors_name");
    എങ്കിൽ (p != ശൂന്യ)
    {
	// The value of g:colors_name could be freed when sourcing the script,
	// making "p" invalid, so copy it.
	char_u *copy_p = vim_strsave(p);
	പൂർണ്ണസംഖ്യ    r;

	എങ്കിൽ (copy_p != ശൂന്യ)
	{
	    r = load_colors(copy_p);
	    vim_free(copy_p);
	    എങ്കിൽ (r == OK)
		തിരികെ;
	}
    }

#endif

    // Didn't use a color file, use the compiled-in colors.
    എങ്കിൽ (both)
    {
	had_both = TRUE;
	pp = highlight_init_both;
	വേണ്ടി (i = 0; pp[i] != ശൂന്യ; ++i)
	    do_highlight((char_u *)pp[i], reset, TRUE);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!had_both)
	// Don't do anything before the call with both == TRUE from main().
	// Not everything has been setup then, and that call will overrule
	// everything anyway.
	തിരികെ;

    എങ്കിൽ (*p_bg == 'l')
	pp = highlight_init_light;
    അല്ലെങ്കിൽ
	pp = highlight_init_dark;
    വേണ്ടി (i = 0; pp[i] != ശൂന്യ; ++i)
	do_highlight((char_u *)pp[i], reset, TRUE);

    // Reverse looks ugly, but grey may not work for less than 8 colors.  Thus
    // let it depend on the number of colors available.
    എങ്കിൽ (t_colors < 8)
	do_highlight((char_u *)"Visual term=reverse cterm=reverse ctermbg=NONE ctermfg=NONE",
		FALSE, TRUE);
    // With 8 colors brown is equal to yellow, need to use black for Search fg
    // to avoid Statement highlighted text disappears.
    // Clear the attributes, needed when changing the t_Co value.
    എങ്കിൽ (t_colors <= 8)
    {
	എങ്കിൽ (*p_bg == 'l')
	    do_highlight((char_u *)"Search ctermfg=black", FALSE, TRUE);
    }

#ifdef FEAT_SYN_HL
    // If syntax highlighting is enabled load the highlighting for it.
    എങ്കിൽ (get_var_മൂല്യം((char_u *)"g:syntax_on") != ശൂന്യ)
    {
	സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = 0;

	എങ്കിൽ (recursive >= 5)
	    emsg(_(e_recursive_loop_loading_syncolor_vim));
	അല്ലെങ്കിൽ
	{
	    ++recursive;
	    (ശൂന്യം)source_runtime((char_u *)"syntax/syncolor.vim", DIP_ALL);
	    --recursive;
	}
    }
#endif
    resolve_fallback_fg_to_rgb();
    resolve_fallback_bg_to_rgb();
}

#എങ്കിൽ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
/*
 * Load a default color list. Intended to support legacy color names but allows
 * the user to override the color values. Only loaded once.
 */
    സ്ഥിതം ശൂന്യം
load_default_colors_lists(ശൂന്യം)
{
    // Lacking a default color list isn't the end of the world but it is likely
    // an inconvenience so users should know when it is missing.
    എങ്കിൽ (source_runtime((char_u *)"colors/lists/default.vim", DIP_ALL) != OK)
	സന്ദേശം("failed to load colors/lists/default.vim");
}
#endif

/*
 * Load color file "name".
 * Return OK for success, FAIL for failure.
 */
    പൂർണ്ണസംഖ്യ
load_colors(char_u *name)
{
    char_u	*buf;
    പൂർണ്ണസംഖ്യ		retval = FAIL;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = FALSE;

    // When being called recursively, this is probably because setting
    // 'background' caused the highlighting to be reloaded.  This means it is
    // working, thus we should return OK.
    എങ്കിൽ (recursive)
	തിരികെ OK;

    recursive = TRUE;
    buf = alloc(STRLEN(name) + 12);
    എങ്കിൽ (buf != ശൂന്യ)
    {
#എങ്കിൽ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	load_default_colors_lists();
#endif
	apply_autocmds(EVENT_COLORSCHEMEPRE, name,
					       curbuf->b_fname, FALSE, curbuf);
	sprintf((അക്ഷരം *)buf, "colors/%s.vim", name);
	retval = source_runtime(buf, DIP_START + DIP_OPT);
	vim_free(buf);
	എങ്കിൽ (retval == OK)
	    apply_autocmds(EVENT_COLORSCHEME, name, curbuf->b_fname,
								FALSE, curbuf);
    }
    recursive = FALSE;

    തിരികെ retval;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ color_numbers_16[28] = {0, 1, 2, 3,
				 4, 5, 6, 6,
				 7, 7, 7, 7,
				 8, 8,
				 9, 9, 10, 10,
				 11, 11, 12, 12, 13,
				 13, 14, 14, 15, -1};
// for xterm with 88 colors...
സ്ഥിതം പൂർണ്ണസംഖ്യ color_numbers_88[28] = {0, 4, 2, 6,
				 1, 5, 32, 72,
				 84, 84, 7, 7,
				 82, 82,
				 12, 43, 10, 61,
				 14, 63, 9, 74, 13,
				 75, 11, 78, 15, -1};
// for xterm with 256 colors...
സ്ഥിതം പൂർണ്ണസംഖ്യ color_numbers_256[28] = {0, 4, 2, 6,
				 1, 5, 130, 3,
				 248, 248, 7, 7,
				 242, 242,
				 12, 81, 10, 121,
				 14, 159, 9, 224, 13,
				 225, 11, 229, 15, -1};
// for terminals with less than 16 colors...
സ്ഥിതം പൂർണ്ണസംഖ്യ color_numbers_8[28] = {0, 4, 2, 6,
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
lookup_color(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ foreground, പൂർണ്ണസംഖ്യ *boldp)
{
    പൂർണ്ണസംഖ്യ		color = color_numbers_16[idx];
    char_u	*p;

    // Use the _16 table to check if it's a valid color name.
    എങ്കിൽ (color < 0)
	തിരികെ -1;

    എങ്കിൽ (t_colors == 8)
    {
	// t_Co is 8: use the 8 colors table
#എങ്കിൽ defined(__QNXNTO__)
	// On qnx, the 8 & 16 color arrays are the same
	എങ്കിൽ (STRNCMP(T_NAME, "qansi", 5) == 0)
	    color = color_numbers_16[idx];
	അല്ലെങ്കിൽ
#endif
	    color = color_numbers_8[idx];
	എങ്കിൽ (foreground)
	{
	    // set/reset bold attribute to get light foreground
	    // colors (on some terminals, e.g. "linux")
	    എങ്കിൽ (color & 8)
		*boldp = TRUE;
	    അല്ലെങ്കിൽ
		*boldp = FALSE;
	}
	color &= 7;	// truncate to 8 colors
    }
    അല്ലെങ്കിൽ എങ്കിൽ (t_colors == 16 || t_colors == 88
					   || t_colors >= 256)
    {
	// Guess: if the termcap entry ends in 'm', it is
	// probably an xterm-like terminal.  Use the changed
	// order for colors.
	എങ്കിൽ (*T_CAF != NUL)
	    p = T_CAF;
	അല്ലെങ്കിൽ
	    p = T_CSF;
	എങ്കിൽ (*p != NUL && (t_colors > 256
			      || *(p + STRLEN(p) - 1) == 'm'))
	{
	    എങ്കിൽ (t_colors == 88)
		color = color_numbers_88[idx];
	    അല്ലെങ്കിൽ എങ്കിൽ (t_colors >= 256)
		color = color_numbers_256[idx];
	    അല്ലെങ്കിൽ
		color = color_numbers_8[idx];
	}
#ifdef FEAT_TERMRESPONSE
	എങ്കിൽ (t_colors >= 256 && color == 15 && is_mac_terminal)
	    // Terminal.app has a bug: 15 is light grey. Use white
	    // from the color cube instead.
	    color = 231;
#endif
    }
    തിരികെ color;
}

/*
 * Link highlight group 'from_hg' to 'to_hg'.
 * 'dodefault' is set to TRUE for ":highlight default link".
 * 'forceit' is set to TRUE for ":highlight! link"
 * 'init' is set to TRUE when initializing all the highlight groups.
 */
    സ്ഥിതം ശൂന്യം
highlight_group_link(
	char_u	*from_hg,
	പൂർണ്ണസംഖ്യ	from_len,
	char_u	*to_hg,
	പൂർണ്ണസംഖ്യ	to_len,
	പൂർണ്ണസംഖ്യ	dodefault,
	പൂർണ്ണസംഖ്യ	forceit,
	പൂർണ്ണസംഖ്യ	init)
{
    പൂർണ്ണസംഖ്യ		from_id;
    പൂർണ്ണസംഖ്യ		to_id;
    hl_group_T	*hlgroup = ശൂന്യ;

    from_id = syn_check_group(from_hg, from_len);
    എങ്കിൽ (STRNCMP(to_hg, "NONE", 4) == 0)
	to_id = 0;
    അല്ലെങ്കിൽ
	to_id = syn_check_group(to_hg, to_len);

    എങ്കിൽ (from_id > 0)
    {
	hlgroup = &HL_TABLE()[from_id - 1];
	എങ്കിൽ (dodefault && (forceit || hlgroup->sg_deflink == 0))
	{
	    hlgroup->sg_deflink = to_id;
#ifdef FEAT_EVAL
	    hlgroup->sg_deflink_sctx = current_sctx;
	    hlgroup->sg_deflink_sctx.sc_lnum += SOURCING_LNUM;
#endif
	}
    }

    എങ്കിൽ (from_id > 0 && (!init || hlgroup->sg_set == 0))
    {
	// Don't allow a link when there already is some highlighting
	// for the group, unless '!' is used
	എങ്കിൽ (to_id > 0 && !forceit && !init
		&& hl_has_settings(from_id - 1, dodefault))
	{
	    എങ്കിൽ (SOURCING_NAME == ശൂന്യ && !dodefault)
		emsg(_(e_group_has_settings_highlight_link_ignored));
	}
	അല്ലെങ്കിൽ എങ്കിൽ (hlgroup->sg_link != to_id
#ifdef FEAT_EVAL
		|| hlgroup->sg_script_ctx.sc_sid != current_sctx.sc_sid
#endif
		|| hlgroup->sg_cleared)
	{
	    എങ്കിൽ (!init)
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
    സ്ഥിതം ശൂന്യം
highlight_reset_all(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		idx;

#ifdef FEAT_GUI
    // First, we do not destroy the old values, but allocate the new
    // ones and update the display. THEN we destroy the old values.
    // If we destroy the old values first, then the old values
    // (such as GuiFont's or GuiFontset's) will still be displayed but
    // invalid because they were free'd.
    എങ്കിൽ (gui.in_use)
    {
# ifdef FEAT_BEVAL_TIP
	gui_init_tooltip_font();
# endif
# എങ്കിൽ defined(FEAT_MENU) && defined(FEAT_GUI_MOTIF)
	gui_init_menu_font();
# endif
    }
# എങ്കിൽ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_X11)
    gui_mch_def_colors();
# endif
# ifdef FEAT_GUI_MSWIN
    എങ്കിൽ (gui.in_use)
	gui_mch_set_titlebar_colors();
# endif
# ifdef FEAT_GUI_X11
#  ifdef FEAT_MENU

    // This only needs to be done when there is no Menu highlight
    // group defined by default, which IS currently the case.
    gui_mch_new_menu_colors();
#  endif
    എങ്കിൽ (gui.in_use)
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
    വേണ്ടി (idx = 0; idx < highlight_ga.ga_len; ++idx)
	highlight_clear(idx);
    init_highlight(TRUE, TRUE);
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    എങ്കിൽ (USE_24BIT)
	highlight_gui_started();
    അല്ലെങ്കിൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
highlight_set_termgui_attr(പൂർണ്ണസംഖ്യ idx, char_u *താക്കോൽ, char_u *arg, പൂർണ്ണസംഖ്യ init)
{
    പൂർണ്ണസംഖ്യ		attr;
    size_t	off;
    keyvalue_T	target;
    keyvalue_T	*entry;

    attr = 0;
    off = 0;
    target.താക്കോൽ = 0;
    target.മൂല്യം.length = 0;	// not used, see cmp_keyvalue_value_ni()
    ആകുമ്പോൾ (arg[off] != NUL)
    {
	target.മൂല്യം.string = arg + off;
	entry = (keyvalue_T *)bsearch(&target, &highlight_tab,
		ARRAY_LENGTH(highlight_tab), വലുപ്പം(highlight_tab[0]),
		cmp_keyvalue_മൂല്യം_ni);
	എങ്കിൽ (entry == ശൂന്യ)
	{
	    semsg(_(e_illegal_മൂല്യം_str), arg);
	    തിരികെ FALSE;
	}

	attr |= entry->താക്കോൽ;
	off += entry->മൂല്യം.length;
	എങ്കിൽ (arg[off] == ',')		// another one follows
	    ++off;
    }
    എങ്കിൽ (*താക്കോൽ == 'T')
    {
	എങ്കിൽ (!init || !(HL_TABLE()[idx].sg_set & SG_TERM))
	{
	    എങ്കിൽ (!init)
		HL_TABLE()[idx].sg_set |= SG_TERM;
	    HL_TABLE()[idx].sg_term = attr;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*താക്കോൽ == 'C')
    {
	എങ്കിൽ (!init || !(HL_TABLE()[idx].sg_set & SG_CTERM))
	{
	    എങ്കിൽ (!init)
		HL_TABLE()[idx].sg_set |= SG_CTERM;
	    HL_TABLE()[idx].sg_cterm = attr;
	    HL_TABLE()[idx].sg_cterm_bold = FALSE;
	}
    }
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_EVAL)
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (!init || !(HL_TABLE()[idx].sg_set & SG_GUI))
	{
	    എങ്കിൽ (!init)
		HL_TABLE()[idx].sg_set |= SG_GUI;
	    HL_TABLE()[idx].sg_gui = attr;
	}
    }
#endif

    തിരികെ TRUE;
}

#ifdef FEAT_GUI
/*
 * Set the font for the highlight group at 'idx'.
 * 'arg' is the font name.
 * Returns TRUE if the font is changed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
highlight_set_font(
	പൂർണ്ണസംഖ്യ	idx,
	char_u	*arg,
	പൂർണ്ണസംഖ്യ	is_normal_group,
	പൂർണ്ണസംഖ്യ	is_menu_group,
	പൂർണ്ണസംഖ്യ	is_tooltip_group)
{
    പൂർണ്ണസംഖ്യ		did_change = FALSE;

    // in non-GUI fonts are simply ignored
    എങ്കിൽ (HL_TABLE()[idx].sg_font_name != ശൂന്യ
	    && STRCMP(HL_TABLE()[idx].sg_font_name, arg) == 0)
    {
	// Font name didn't change, ignore.
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!gui.shell_created)
    {
	// GUI not started yet, always accept the name.
	vim_free(HL_TABLE()[idx].sg_font_name);
	HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	did_change = TRUE;
    }
    അല്ലെങ്കിൽ
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
	എങ്കിൽ (HL_TABLE()[idx].sg_fontset != NOFONTSET)
	{
	    // New fontset was accepted. Free the old one, if there
	    // was one.
	    gui_mch_free_fontset(temp_sg_fontset);
	    did_change = TRUE;
	}
	അല്ലെങ്കിൽ
	    HL_TABLE()[idx].sg_fontset = temp_sg_fontset;
# endif
	എങ്കിൽ (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    // New font was accepted. Free the old one, if there was
	    // one.
	    gui_mch_free_font(temp_sg_font);
	    did_change = TRUE;
	}
	അല്ലെങ്കിൽ
	    HL_TABLE()[idx].sg_font = temp_sg_font;
	// Update sg_font_name once here, after both font and fontset results
	// are known.
	എങ്കിൽ (did_change)
	{
	    vim_free(HL_TABLE()[idx].sg_font_name);
	    HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	}
    }

    തിരികെ did_change;
}
#endif

/*
 * Set the cterm foreground color for the Normal highlight group to "color" and
 * the bold attribute to "bold".
 */
    സ്ഥിതം ശൂന്യം
hl_set_ctermfg_normal_group(പൂർണ്ണസംഖ്യ color, പൂർണ്ണസംഖ്യ bold)
{
    cterm_normal_fg_color = color + 1;
    cterm_normal_fg_bold = bold;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    എങ്കിൽ (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	എങ്കിൽ (termcap_active && color >= 0)
	    term_fg_color(color);
    }
}

/*
 * Set the cterm foreground color for the highlight group at 'idx' to 'color'.
 */
    സ്ഥിതം ശൂന്യം
highlight_set_ctermfg(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ color, പൂർണ്ണസംഖ്യ is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_fg = color + 1;
    എങ്കിൽ (is_normal_group)
	hl_set_ctermfg_normal_group(color,
					(HL_TABLE()[idx].sg_cterm & HL_BOLD));
}

/*
 * Set the cterm background color for the Normal highlight group to "color".
 */
    സ്ഥിതം ശൂന്യം
hl_set_ctermbg_normal_group(പൂർണ്ണസംഖ്യ color)
{
    cterm_normal_bg_color = color + 1;
#ifdef FEAT_GUI
    // Don't mess with 'background' if the GUI is used.
    എങ്കിൽ (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	എങ്കിൽ (color >= 0)
	{
	    പൂർണ്ണസംഖ്യ dark = -1;

	    എങ്കിൽ (termcap_active)
		term_bg_color(color);
	    എങ്കിൽ (t_colors < 16)
		dark = (color == 0 || color == 4);
	    // Limit the heuristic to the standard 16 colors
	    അല്ലെങ്കിൽ എങ്കിൽ (color < 16)
		dark = (color < 7 || color == 8);
	    // Set the 'background' option if the value is
	    // wrong.
	    എങ്കിൽ (dark != -1
		    && dark != (*p_bg == 'd')
		    && !option_was_set((char_u *)"bg"))
	    {
		set_option_മൂല്യം_give_err((char_u *)"bg",
			0L, (char_u *)(dark ? "dark" : "light"), 0);
		reset_option_was_set((char_u *)"bg");
	    }
	}
    }
}

/*
 * Set the cterm background color for the highlight group at 'idx' to 'color'.
 */
    സ്ഥിതം ശൂന്യം
highlight_set_ctermbg(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ color, പൂർണ്ണസംഖ്യ is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_bg = color + 1;
    എങ്കിൽ (is_normal_group)
	hl_set_ctermbg_normal_group(color);
}

/*
 * Set the cterm underline color for the Normal highlight group to "color".
 */
    സ്ഥിതം ശൂന്യം
hl_set_ctermul_normal_group(പൂർണ്ണസംഖ്യ color)
{
    cterm_normal_ul_color = color + 1;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    എങ്കിൽ (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	എങ്കിൽ (termcap_active && color >= 0)
	    term_ul_color(color);
    }
}

/*
 * Set the cterm underline color for the highlight group at 'idx' to 'color'.
 */
    സ്ഥിതം ശൂന്യം
highlight_set_ctermul(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ color, പൂർണ്ണസംഖ്യ is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_ul = color + 1;
    എങ്കിൽ (is_normal_group)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
highlight_set_cterm_font(
	പൂർണ്ണസംഖ്യ	idx,
	char_u	*arg,
	പൂർണ്ണസംഖ്യ	init)
{
    പൂർണ്ണസംഖ്യ		font;

    എങ്കിൽ (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	തിരികെ FALSE;

    എങ്കിൽ (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    എങ്കിൽ (VIM_ISDIGIT(*arg))
	font = atoi((അക്ഷരം *)arg);
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(arg, "NONE") == 0)
	font = -1;
    അല്ലെങ്കിൽ
	തിരികെ FALSE;

    HL_TABLE()[idx].sg_cterm_font = font + 1;
    തിരികെ TRUE;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
highlight_set_cterm_color(
	പൂർണ്ണസംഖ്യ	idx,
	char_u	*താക്കോൽ,
	char_u	*താക്കോൽ_start,
	char_u	*arg,
	പൂർണ്ണസംഖ്യ	is_normal_group,
	പൂർണ്ണസംഖ്യ	init)
{
    പൂർണ്ണസംഖ്യ		color;

    എങ്കിൽ (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	തിരികെ FALSE;

    എങ്കിൽ (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    // When setting the foreground color, and previously the "bold"
    // flag was set for a light color, reset it now
    എങ്കിൽ (താക്കോൽ[5] == 'F' && HL_TABLE()[idx].sg_cterm_bold)
    {
	HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
	HL_TABLE()[idx].sg_cterm_bold = FALSE;
    }

    എങ്കിൽ (VIM_ISDIGIT(*arg))
	color = atoi((അക്ഷരം *)arg);
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(arg, "fg") == 0)
    {
	എങ്കിൽ (cterm_normal_fg_color)
	    color = cterm_normal_fg_color - 1;
	അല്ലെങ്കിൽ
	{
	    emsg(_(e_fg_color_unknown));
	    തിരികെ FALSE;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(arg, "bg") == 0)
    {
	എങ്കിൽ (cterm_normal_bg_color > 0)
	    color = cterm_normal_bg_color - 1;
	അല്ലെങ്കിൽ
	{
	    emsg(_(e_bg_color_unknown));
	    തിരികെ FALSE;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(arg, "ul") == 0)
    {
	എങ്കിൽ (cterm_normal_ul_color > 0)
	    color = cterm_normal_ul_color - 1;
	അല്ലെങ്കിൽ
	{
	    emsg(_(e_ul_color_unknown));
	    തിരികെ FALSE;
	}
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ bold = MAYBE;
	keyvalue_T target;
	keyvalue_T *entry;

	target.താക്കോൽ = 0;
	target.മൂല്യം.string = arg;
	target.മൂല്യം.length = 0;	// not used, see cmp_keyvalue_value_i()
	entry = (keyvalue_T *)bsearch(&target, &color_name_tab,
		ARRAY_LENGTH(color_name_tab), വലുപ്പം(color_name_tab[0]),
		cmp_keyvalue_മൂല്യം_i);
	എങ്കിൽ (entry == ശൂന്യ)
	{
	    semsg(_(e_color_name_or_സംഖ്യ_not_recognized_str), താക്കോൽ_start);
	    തിരികെ FALSE;
	}

	color = lookup_color(entry->താക്കോൽ, താക്കോൽ[5] == 'F', &bold);

	// set/reset bold attribute to get light foreground
	// colors (on some terminals, e.g. "linux")
	എങ്കിൽ (bold == TRUE)
	{
	    HL_TABLE()[idx].sg_cterm |= HL_BOLD;
	    HL_TABLE()[idx].sg_cterm_bold = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (bold == FALSE)
	    HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
    }

    // Add one to the argument, to avoid zero.  Zero is used for
    // "NONE", then "color" is -1.
    എങ്കിൽ (താക്കോൽ[5] == 'F')
	highlight_set_ctermfg(idx, color, is_normal_group);
    അല്ലെങ്കിൽ എങ്കിൽ (താക്കോൽ[5] == 'B')
	highlight_set_ctermbg(idx, color, is_normal_group);
    അല്ലെങ്കിൽ // ctermul
	highlight_set_ctermul(idx, color, is_normal_group);

    തിരികെ TRUE;
}

#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_EVAL)
/*
 * Set the GUI foreground color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
highlight_set_guifg(
	പൂർണ്ണസംഖ്യ	idx,
	char_u	*arg,
	പൂർണ്ണസംഖ്യ	is_menu_group UNUSED,
	പൂർണ്ണസംഖ്യ	is_scrollbar_group UNUSED,
	പൂർണ്ണസംഖ്യ	is_tooltip_group UNUSED,
	പൂർണ്ണസംഖ്യ	is_titlebar_group UNUSED,
	പൂർണ്ണസംഖ്യ	is_titlebarnc_group UNUSED,
	പൂർണ്ണസംഖ്യ	*do_colors UNUSED,
	പൂർണ്ണസംഖ്യ	init)
{
# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    long	i;
# endif
    char_u	**namep;
    പൂർണ്ണസംഖ്യ		did_change = FALSE;

    എങ്കിൽ (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	തിരികെ FALSE;

    namep = &HL_TABLE()[idx].sg_gui_fg_name;
    എങ്കിൽ (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guifg colors are only used when recognized
    i = color_name2handle(arg);
    എങ്കിൽ (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_fg = i;
# endif
	എങ്കിൽ (*namep == ശൂന്യ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    എങ്കിൽ (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    അല്ലെങ്കിൽ
		*namep = ശൂന്യ;
	    did_change = TRUE;
	}
# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	എങ്കിൽ (is_titlebar_group && gui.title_fg_pixel != i)
	{
	    gui.title_fg_pixel = i;
	    *do_colors = TRUE;
	}
	എങ്കിൽ (is_titlebarnc_group && gui.titlenc_fg_pixel != i)
	{
	    gui.titlenc_fg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	എങ്കിൽ (is_menu_group && gui.menu_fg_pixel != i)
	{
	    gui.menu_fg_pixel = i;
	    *do_colors = TRUE;
	}
	എങ്കിൽ (is_scrollbar_group && gui.scroll_fg_pixel != i)
	{
	    gui.scroll_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	എങ്കിൽ (is_tooltip_group && gui.tooltip_fg_pixel != i)
	{
	    gui.tooltip_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    തിരികെ did_change;
}

/*
 * Set the GUI background color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
highlight_set_guibg(
	പൂർണ്ണസംഖ്യ	idx,
	char_u	*arg,
	പൂർണ്ണസംഖ്യ	is_menu_group UNUSED,
	പൂർണ്ണസംഖ്യ	is_scrollbar_group UNUSED,
	പൂർണ്ണസംഖ്യ	is_tooltip_group UNUSED,
	പൂർണ്ണസംഖ്യ	is_titlebar_group UNUSED,
	പൂർണ്ണസംഖ്യ	is_titlebarnc_group UNUSED,
	പൂർണ്ണസംഖ്യ	*do_colors UNUSED,
	പൂർണ്ണസംഖ്യ	init)
{
# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    പൂർണ്ണസംഖ്യ		i;
# endif
    char_u	**namep;
    പൂർണ്ണസംഖ്യ		did_change = FALSE;

    എങ്കിൽ (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	തിരികെ FALSE;

    namep = &HL_TABLE()[idx].sg_gui_bg_name;
    എങ്കിൽ (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guibg colors are only used when recognized
    i = color_name2handle(arg);
    എങ്കിൽ (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_bg = i;
# endif
	എങ്കിൽ (*namep == ശൂന്യ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    എങ്കിൽ (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    അല്ലെങ്കിൽ
		*namep = ശൂന്യ;
	    did_change = TRUE;
	}
# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	എങ്കിൽ (is_titlebar_group && gui.title_bg_pixel != i)
	{
	    gui.title_bg_pixel = i;
	    *do_colors = TRUE;
	}
	എങ്കിൽ (is_titlebarnc_group && gui.titlenc_bg_pixel != i)
	{
	    gui.titlenc_bg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	എങ്കിൽ (is_menu_group && gui.menu_bg_pixel != i)
	{
	    gui.menu_bg_pixel = i;
	    *do_colors = TRUE;
	}
	എങ്കിൽ (is_scrollbar_group && gui.scroll_bg_pixel != i)
	{
	    gui.scroll_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	എങ്കിൽ (is_tooltip_group && gui.tooltip_bg_pixel != i)
	{
	    gui.tooltip_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    തിരികെ did_change;
}

/*
 * Set the GUI undercurl/strikethrough color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
highlight_set_guisp(പൂർണ്ണസംഖ്യ idx, char_u *arg, പൂർണ്ണസംഖ്യ init)
{
# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    പൂർണ്ണസംഖ്യ		i;
# endif
    പൂർണ്ണസംഖ്യ		did_change = FALSE;
    char_u	**namep;

    എങ്കിൽ (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	തിരികെ FALSE;

    namep = &HL_TABLE()[idx].sg_gui_sp_name;
    എങ്കിൽ (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guisp colors are only used when recognized
    i = color_name2handle(arg);
    എങ്കിൽ (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_sp = i;
# endif
	എങ്കിൽ (*namep == ശൂന്യ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    എങ്കിൽ (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    അല്ലെങ്കിൽ
		*namep = ശൂന്യ;
	    did_change = TRUE;
	}
# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    }
# endif

    തിരികെ did_change;
}
#endif

/*
 * Set the start/stop terminal codes for a highlight group.
 * Returns TRUE if the terminal code is set.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
highlight_set_startstop_termcode(പൂർണ്ണസംഖ്യ idx, char_u *താക്കോൽ, char_u *arg, പൂർണ്ണസംഖ്യ init)
{
    പൂർണ്ണസംഖ്യ		off;
    char_u	buf[100];
    പൂർണ്ണസംഖ്യ		len;
    char_u	*tname;
    char_u	*p;

    എങ്കിൽ (!init)
	HL_TABLE()[idx].sg_set |= SG_TERM;

    // The "start" and "stop"  arguments can be a literal escape
    // sequence, or a comma separated list of terminal codes.
    എങ്കിൽ (STRNCMP(arg, "t_", 2) == 0)
    {
	off = 0;
	buf[0] = 0;
	ആകുമ്പോൾ (arg[off] != NUL)
	{
	    // Isolate one termcap name
	    വേണ്ടി (len = 0; arg[off + len] &&
		    arg[off + len] != ','; ++len)
		;
	    tname = vim_strnsave(arg + off, len);
	    എങ്കിൽ (tname == ശൂന്യ)		// out of memory
		തിരികെ FALSE;
	    // lookup the escape sequence for the item
	    p = get_term_code(tname);
	    vim_free(tname);
	    എങ്കിൽ (p == ശൂന്യ)	    // ignore non-existing things
		p = (char_u *)"";

	    // Append it to the already found stuff
	    എങ്കിൽ ((പൂർണ്ണസംഖ്യ)(STRLEN(buf) + STRLEN(p)) >= 99)
	    {
		semsg(_(e_terminal_code_too_long_str), arg);
		തിരികെ FALSE;
	    }
	    STRCAT(buf, p);

	    // Advance to the next item
	    off += len;
	    എങ്കിൽ (arg[off] == ',')	    // another one follows
		++off;
	}
    }
    അല്ലെങ്കിൽ
    {
	// Copy characters from arg[] to buf[], translating <> codes.
	വേണ്ടി (p = arg, off = 0; off < 100 - 6 && *p; )
	{
	    len = trans_special(&p, buf + off, FSK_SIMPLIFY, FALSE, ശൂന്യ);
	    എങ്കിൽ (len > 0)	    // recognized special char
		off += len;
	    അല്ലെങ്കിൽ		    // copy as normal char
		buf[off++] = *p++;
	}
	buf[off] = NUL;
    }

    എങ്കിൽ (STRCMP(buf, "NONE") == 0)	// resetting the value
	p = ശൂന്യ;
    അല്ലെങ്കിൽ
	p = vim_strsave(buf);
    എങ്കിൽ (താക്കോൽ[2] == 'A')
    {
	vim_free(HL_TABLE()[idx].sg_start);
	HL_TABLE()[idx].sg_start = p;
    }
    അല്ലെങ്കിൽ
    {
	vim_free(HL_TABLE()[idx].sg_stop);
	HL_TABLE()[idx].sg_stop = p;
    }
    തിരികെ TRUE;
}

/*
 * Handle the ":highlight .." command.
 * When using ":hi clear" this is called recursively for each group with
 * "forceit" and "init" both TRUE.
 */
    ശൂന്യം
do_highlight(
    char_u	*വരി,
    പൂർണ്ണസംഖ്യ		forceit,
    പൂർണ്ണസംഖ്യ		init)	    // TRUE when called for initializing
{
    char_u	*name_end;
    char_u	*linep;
    char_u	*താക്കോൽ_start;
    char_u	*arg_start;
    char_u	*താക്കോൽ = ശൂന്യ, *arg = ശൂന്യ;
    long	i;
    പൂർണ്ണസംഖ്യ		id;
    പൂർണ്ണസംഖ്യ		idx;
    hl_group_T	item_before;
    പൂർണ്ണസംഖ്യ		did_change = FALSE;
    പൂർണ്ണസംഖ്യ		dodefault = FALSE;
    പൂർണ്ണസംഖ്യ		doclear = FALSE;
    പൂർണ്ണസംഖ്യ		dolink = FALSE;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    പൂർണ്ണസംഖ്യ		is_normal_group = FALSE;	// "Normal" group
#ifdef FEAT_GUI_MSWIN
    പൂർണ്ണസംഖ്യ		is_titlebar_group = FALSE;	// "TitleBar" group
    പൂർണ്ണസംഖ്യ		is_titlebarnc_group = FALSE;	// "TitleBarNC" group
#അല്ലെങ്കിൽ
# define is_titlebar_group 0
# define is_titlebarnc_group 0
#endif
#ifdef FEAT_GUI_X11
    പൂർണ്ണസംഖ്യ		is_menu_group = FALSE;		// "Menu" group
    പൂർണ്ണസംഖ്യ		is_scrollbar_group = FALSE;	// "Scrollbar" group
    പൂർണ്ണസംഖ്യ		is_tooltip_group = FALSE;	// "Tooltip" group
#അല്ലെങ്കിൽ
# define is_menu_group 0
# define is_tooltip_group 0
# define is_scrollbar_group 0
#endif
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_EVAL)
    പൂർണ്ണസംഖ്യ		do_colors = FALSE;		// need to update colors?
#endif
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    പൂർണ്ണസംഖ്യ		did_highlight_changed = FALSE;
#endif

    // If no argument, list current highlighting.
    എങ്കിൽ (!init && ends_excmd2(വരി - 1, വരി))
    {
	വേണ്ടി (i = 1; i <= highlight_ga.ga_len && !got_int; ++i)
	    // TODO: only call when the group has attributes set
	    highlight_list_one((പൂർണ്ണസംഖ്യ)i);
	തിരികെ;
    }

    // Isolate the name.
    name_end = skiptowhite(വരി);
    linep = skipwhite(name_end);

    // Check for "default" argument.
    എങ്കിൽ (STRNCMP(വരി, "default", name_end - വരി) == 0)
    {
	dodefault = TRUE;
	വരി = linep;
	name_end = skiptowhite(വരി);
	linep = skipwhite(name_end);
    }

    // Check for "clear" or "link" argument.
    എങ്കിൽ (STRNCMP(വരി, "clear", name_end - വരി) == 0)
	doclear = TRUE;
    എങ്കിൽ (STRNCMP(വരി, "link", name_end - വരി) == 0)
	dolink = TRUE;

    // ":highlight {group-name}": list highlighting for one group.
    എങ്കിൽ (!doclear && !dolink && ends_excmd2(വരി, linep))
    {
	id = syn_namen2id(വരി, (പൂർണ്ണസംഖ്യ)(name_end - വരി));
	എങ്കിൽ (id == 0)
	    semsg(_(e_highlight_group_name_not_found_str), വരി);
	അല്ലെങ്കിൽ
	    highlight_list_one(id);
	തിരികെ;
    }

    // Handle ":highlight link {from} {to}" command.
    എങ്കിൽ (dolink)
    {
	char_u	    *from_start = linep;
	char_u	    *from_end;
	പൂർണ്ണസംഖ്യ	    from_len;
	char_u	    *to_start;
	char_u	    *to_end;
	പൂർണ്ണസംഖ്യ	    to_len;

	from_end = skiptowhite(from_start);
	to_start = skipwhite(from_end);
	to_end	 = skiptowhite(to_start);

	എങ്കിൽ (ends_excmd2(വരി, from_start) || ends_excmd2(വരി, to_start))
	{
	    semsg(_(e_not_enough_arguments_highlight_link_str), from_start);
	    തിരികെ;
	}

	എങ്കിൽ (!ends_excmd2(വരി, skipwhite(to_end)))
	{
	    semsg(_(e_too_many_arguments_highlight_link_str), from_start);
	    തിരികെ;
	}

	from_len = (പൂർണ്ണസംഖ്യ)(from_end - from_start);
	to_len = (പൂർണ്ണസംഖ്യ)(to_end - to_start);
	highlight_group_link(from_start, from_len, to_start, to_len,
						dodefault, forceit, init);
	തിരികെ;
    }

    എങ്കിൽ (doclear)
    {
	// ":highlight clear [group]" command.
	എങ്കിൽ (ends_excmd2(വരി, linep))
	{
	    // ":highlight clear" without group name
	    highlight_reset_all();
	    തിരികെ;
	}
	വരി = linep;
	name_end = skiptowhite(വരി);
	linep = skipwhite(name_end);
    }

    // Find the group name in the table.  If it does not exist yet, add it.
    id = syn_check_group(വരി, (പൂർണ്ണസംഖ്യ)(name_end - വരി));
    എങ്കിൽ (id == 0)	// failed (out of memory)
	തിരികെ;
    idx = id - 1;	// index is ID minus one

    // Return if "default" was used and the group already has settings.
    എങ്കിൽ (dodefault && hl_has_settings(idx, TRUE))
	തിരികെ;

    // Make a copy so we can check if any attribute actually changed.
    item_before = HL_TABLE()[idx];

    എങ്കിൽ (STRCMP(HL_TABLE()[idx].sg_name_u, "NORMAL") == 0)
	is_normal_group = TRUE;
#ifdef FEAT_GUI_MSWIN
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBAR") == 0)
	is_titlebar_group = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBARNC") == 0)
	is_titlebarnc_group = TRUE;
#endif
#ifdef FEAT_GUI_X11
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(HL_TABLE()[idx].sg_name_u, "MENU") == 0)
	is_menu_group = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(HL_TABLE()[idx].sg_name_u, "SCROLLBAR") == 0)
	is_scrollbar_group = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(HL_TABLE()[idx].sg_name_u, "TOOLTIP") == 0)
	is_tooltip_group = TRUE;
#endif

    // Clear the highlighting for ":hi clear {group}" and ":hi clear".
    എങ്കിൽ (doclear || (forceit && init))
    {
	highlight_clear(idx);
	എങ്കിൽ (!doclear)
	    HL_TABLE()[idx].sg_set = 0;
    }

    എങ്കിൽ (!doclear)
	ആകുമ്പോൾ (!ends_excmd2(വരി, linep))
	{
	    താക്കോൽ_start = linep;
	    എങ്കിൽ (*linep == '=')
	    {
		semsg(_(e_unexpected_equal_sign_str), താക്കോൽ_start);
		error = TRUE;
		നിർത്തുക;
	    }

	    // Note: Keep this in sync with get_highlight_group_key.

	    // Isolate the key ("term", "ctermfg", "ctermbg", "font", "guifg"
	    // or "guibg").
	    ആകുമ്പോൾ (*linep && !VIM_ISWHITE(*linep) && *linep != '=')
		++linep;
	    vim_free(താക്കോൽ);
	    താക്കോൽ = vim_strnsave_up(താക്കോൽ_start, linep - താക്കോൽ_start);
	    എങ്കിൽ (താക്കോൽ == ശൂന്യ)
	    {
		error = TRUE;
		നിർത്തുക;
	    }
	    linep = skipwhite(linep);

	    എങ്കിൽ (STRCMP(താക്കോൽ, "NONE") == 0)
	    {
		എങ്കിൽ (!init || HL_TABLE()[idx].sg_set == 0)
		{
		    എങ്കിൽ (!init)
			HL_TABLE()[idx].sg_set |= SG_TERM+SG_CTERM+SG_GUI;
		    highlight_clear(idx);
		}
		തുടരുക;
	    }

	    // Check for the equal sign.
	    എങ്കിൽ (*linep != '=')
	    {
		semsg(_(e_missing_equal_sign_str_2), താക്കോൽ_start);
		error = TRUE;
		നിർത്തുക;
	    }
	    ++linep;

	    // Isolate the argument.
	    linep = skipwhite(linep);
	    എങ്കിൽ (*linep == '\'')		// guifg='color name'
	    {
		arg_start = ++linep;
		linep = vim_strchr(linep, '\'');
		എങ്കിൽ (linep == ശൂന്യ)
		{
		    semsg(_(e_invalid_argument_str), താക്കോൽ_start);
		    error = TRUE;
		    നിർത്തുക;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		arg_start = linep;
		linep = skiptowhite(linep);
	    }
	    എങ്കിൽ (linep == arg_start)
	    {
		semsg(_(e_missing_argument_str), താക്കോൽ_start);
		error = TRUE;
		നിർത്തുക;
	    }
	    vim_free(arg);
	    arg = vim_strnsave(arg_start, linep - arg_start);
	    എങ്കിൽ (arg == ശൂന്യ)
	    {
		error = TRUE;
		നിർത്തുക;
	    }
	    എങ്കിൽ (*linep == '\'')
		++linep;

	    // Store the argument.
	    എങ്കിൽ (STRCMP(താക്കോൽ, "TERM") == 0
		    || STRCMP(താക്കോൽ, "CTERM") == 0
		    || STRCMP(താക്കോൽ, "GUI") == 0)
	    {
		എങ്കിൽ (!highlight_set_termgui_attr(idx, താക്കോൽ, arg, init))
		{
		    error = TRUE;
		    നിർത്തുക;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(താക്കോൽ, "FONT") == 0)
	    {
		// in non-GUI fonts are simply ignored
#ifdef FEAT_GUI
		എങ്കിൽ (highlight_set_font(idx, arg, is_normal_group,
					      is_menu_group, is_tooltip_group))
		    did_change = TRUE;
#endif
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(താക്കോൽ, "CTERMFG") == 0
		    || STRCMP(താക്കോൽ, "CTERMBG") == 0
		    || STRCMP(താക്കോൽ, "CTERMUL") == 0)
	    {
		എങ്കിൽ (!highlight_set_cterm_color(idx, താക്കോൽ, താക്കോൽ_start, arg,
							is_normal_group, init))
		{
		    error = TRUE;
		    നിർത്തുക;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(താക്കോൽ, "CTERMFONT") == 0)
	    {
		എങ്കിൽ (!highlight_set_cterm_font(idx, arg, init))
		{
		    error = TRUE;
		    നിർത്തുക;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(താക്കോൽ, "GUIFG") == 0)
	    {
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_EVAL)
		എങ്കിൽ (highlight_set_guifg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(താക്കോൽ, "GUIBG") == 0)
	    {
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_EVAL)
		എങ്കിൽ (highlight_set_guibg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(താക്കോൽ, "GUISP") == 0)
	    {
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_EVAL)
		എങ്കിൽ (highlight_set_guisp(idx, arg, init))
		    did_change = TRUE;
#endif
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(താക്കോൽ, "START") == 0 || STRCMP(താക്കോൽ, "STOP") == 0)
	    {
		എങ്കിൽ (!highlight_set_startstop_termcode(idx, താക്കോൽ, arg, init))
		{
		    error = TRUE;
		    നിർത്തുക;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		semsg(_(e_illegal_argument_str_3), താക്കോൽ_start);
		error = TRUE;
		നിർത്തുക;
	    }
	    HL_TABLE()[idx].sg_cleared = FALSE;

	    // When highlighting has been given for a group, don't link it.
	    എങ്കിൽ (!init || !(HL_TABLE()[idx].sg_set & SG_LINK))
		HL_TABLE()[idx].sg_link = 0;

	    // Continue with next argument.
	    linep = skipwhite(linep);
	}

    // If there is an error, and it's a new entry, remove it from the table.
    എങ്കിൽ (error && idx == highlight_ga.ga_len)
	syn_unadd_group();
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (is_normal_group)
	{
	    HL_TABLE()[idx].sg_term_attr = 0;
	    HL_TABLE()[idx].sg_cterm_attr = 0;
#ifdef FEAT_GUI
	    HL_TABLE()[idx].sg_gui_attr = 0;
	    // Need to update all groups, because they might be using "bg"
	    // and/or "fg", which have been changed now.
#endif
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    എങ്കിൽ (USE_24BIT)
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
	അല്ലെങ്കിൽ എങ്കിൽ (is_titlebar_group)
	{
	    എങ്കിൽ (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
	അല്ലെങ്കിൽ എങ്കിൽ (is_titlebarnc_group)
	{
	    എങ്കിൽ (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
#endif
#ifdef FEAT_GUI_X11
# ifdef FEAT_MENU
	അല്ലെങ്കിൽ എങ്കിൽ (is_menu_group)
	{
	    എങ്കിൽ (gui.in_use && do_colors)
		gui_mch_new_menu_colors();
	}
# endif
	അല്ലെങ്കിൽ എങ്കിൽ (is_scrollbar_group)
	{
	    എങ്കിൽ (gui.in_use && do_colors)
		gui_new_scrollbar_colors();
	    അല്ലെങ്കിൽ
		set_hl_attr(idx);
	}
# ifdef FEAT_BEVAL_GUI
	അല്ലെങ്കിൽ എങ്കിൽ (is_tooltip_group)
	{
	    എങ്കിൽ (gui.in_use && do_colors)
		gui_mch_new_tooltip_colors();
	}
# endif
#endif
	അല്ലെങ്കിൽ
	    set_hl_attr(idx);
#ifdef FEAT_EVAL
	HL_TABLE()[idx].sg_script_ctx = current_sctx;
	HL_TABLE()[idx].sg_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
    }

    vim_free(താക്കോൽ);
    vim_free(arg);

    // Only call highlight_changed() once, after a sequence of highlight
    // commands, and only if an attribute actually changed.
    എങ്കിൽ ((did_change
	   || memcmp(&HL_TABLE()[idx], &item_before, വലുപ്പം(item_before)) != 0)
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    && !did_highlight_changed
#endif
       )
    {
	// Do not trigger a redraw when highlighting is changed while
	// redrawing.  This may happen when evaluating 'statusline' changes the
	// StatusLine group.
	എങ്കിൽ (!updating_screen)
	    redraw_all_later(UPD_NOT_VALID);
	need_highlight_changed = TRUE;
    }
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_highlight(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    i;

    വേണ്ടി (i = 0; i < highlight_ga.ga_len; ++i)
    {
	highlight_clear(i);
	vim_free(HL_TABLE()[i].sg_name);
	vim_free(HL_TABLE()[i].sg_name_u - HLNAME_താക്കോൽ_OFF);
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
    ശൂന്യം
restore_cterm_colors(ശൂന്യം)
{
#എങ്കിൽ defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
    // Since t_me has been set, this probably means that the user
    // wants to use this as default colors.  Need to reset default
    // background/foreground colors.
    mch_set_normal_colors();
#അല്ലെങ്കിൽ
# ifdef VIMDLL
    എങ്കിൽ (!gui.in_use)
    {
	mch_set_normal_colors();
	തിരികെ;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
hl_has_settings(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ check_link)
{
    തിരികെ HL_TABLE()[idx].sg_cleared == 0
	 && (  HL_TABLE()[idx].sg_term_attr != 0
	    || HL_TABLE()[idx].sg_cterm_attr != 0
	    || HL_TABLE()[idx].sg_cterm_fg != 0
	    || HL_TABLE()[idx].sg_cterm_bg != 0
	    || HL_TABLE()[idx].sg_cterm_font != 0
#ifdef FEAT_GUI
	    || HL_TABLE()[idx].sg_gui_attr != 0
	    || HL_TABLE()[idx].sg_gui_fg_name != ശൂന്യ
	    || HL_TABLE()[idx].sg_gui_bg_name != ശൂന്യ
	    || HL_TABLE()[idx].sg_gui_sp_name != ശൂന്യ
	    || HL_TABLE()[idx].sg_font_name != ശൂന്യ
#endif
	    || (check_link && (HL_TABLE()[idx].sg_set & SG_LINK)));
}

/*
 * Clear highlighting for one group.
 */
    സ്ഥിതം ശൂന്യം
highlight_clear(പൂർണ്ണസംഖ്യ idx)
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
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_EVAL)
    HL_TABLE()[idx].sg_gui = 0;
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_fg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_bg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_sp_name);
#endif
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Set the normal foreground and background colors according to the "Normal"
 * highlighting group.  For X11 also set "Menu", "Scrollbar" and "Tooltip"
 * colors.  For MS-Windows also set "TitleBar" and "TitleBarNC" colors.
 */
    ശൂന്യം
set_normal_colors(ശൂന്യം)
{
# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    എങ്കിൽ (gui.in_use)
#  endif
    {
	എങ്കിൽ (set_group_colors((char_u *)"Normal",
				 &gui.norm_pixel, &gui.back_pixel,
				 FALSE, TRUE, FALSE))
	{
	    gui_mch_new_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  ifdef FEAT_GUI_MSWIN
	എങ്കിൽ (set_group_colors((char_u *)"TitleBar",
				 &gui.title_fg_pixel, &gui.title_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
	എങ്കിൽ (set_group_colors((char_u *)"TitleBarNC",
				 &gui.titlenc_fg_pixel, &gui.titlenc_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  endif
#  ifdef FEAT_GUI_X11
	എങ്കിൽ (set_group_colors((char_u *)"Menu",
			     &gui.menu_fg_pixel, &gui.menu_bg_pixel,
			     TRUE, FALSE, FALSE))
	{
#   ifdef FEAT_MENU
	    gui_mch_new_menu_colors();
#   endif
	    set_must_redraw(UPD_CLEAR);
	}
#   ifdef FEAT_BEVAL_GUI
	എങ്കിൽ (set_group_colors((char_u *)"Tooltip",
			     &gui.tooltip_fg_pixel, &gui.tooltip_bg_pixel,
			     FALSE, FALSE, TRUE))
	{
#    ifdef FEAT_TOOLBAR
	    gui_mch_new_tooltip_colors();
#    endif
	    set_must_redraw(UPD_CLEAR);
	}
#   endif
	എങ്കിൽ (set_group_colors((char_u *)"Scrollbar",
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
    അല്ലെങ്കിൽ
#  endif
    {
	പൂർണ്ണസംഖ്യ		idx;

	idx = syn_name2id((char_u *)"Normal") - 1;
	എങ്കിൽ (idx >= 0)
	{
	    gui_do_one_color(idx, FALSE, FALSE);

	    // If the normal fg or bg color changed a complete redraw is
	    // required.
	    എങ്കിൽ (cterm_normal_fg_gui_color != HL_TABLE()[idx].sg_gui_fg
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

#എങ്കിൽ defined(FEAT_GUI)
/*
 * Set the colors for "Normal", "Menu", "TitleBar", "TitleBarNC", "Tooltip" or
 * "Scrollbar".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_group_colors(
    char_u	*name,
    guicolor_T	*fgp,
    guicolor_T	*bgp,
    പൂർണ്ണസംഖ്യ		do_menu,
    പൂർണ്ണസംഖ്യ		use_norm,
    പൂർണ്ണസംഖ്യ		do_tooltip)
{
    പൂർണ്ണസംഖ്യ		idx;

    idx = syn_name2id(name) - 1;
    എങ്കിൽ (idx < 0)
	തിരികെ FALSE;

    gui_do_one_color(idx, do_menu, do_tooltip);

    എങ്കിൽ (HL_TABLE()[idx].sg_gui_fg != INVALCOLOR)
	*fgp = HL_TABLE()[idx].sg_gui_fg;
    അല്ലെങ്കിൽ എങ്കിൽ (use_norm)
	*fgp = gui.def_norm_pixel;
    എങ്കിൽ (HL_TABLE()[idx].sg_gui_bg != INVALCOLOR)
	*bgp = HL_TABLE()[idx].sg_gui_bg;
    അല്ലെങ്കിൽ എങ്കിൽ (use_norm)
	*bgp = gui.def_back_pixel;
    തിരികെ TRUE;
}

/*
 * Get the font of the "Normal" group.
 * Returns "" when it's not found or not set.
 */
    char_u *
hl_get_font_name(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		id;
    char_u	*s;

    id = syn_name2id((char_u *)"Normal");
    എങ്കിൽ (id > 0)
    {
	s = HL_TABLE()[id - 1].sg_font_name;
	എങ്കിൽ (s != ശൂന്യ)
	    തിരികെ s;
    }
    തിരികെ (char_u *)"";
}

/*
 * Set font for "Normal" group.  Called by gui_mch_init_font() when a font has
 * actually chosen to be used.
 */
    ശൂന്യം
hl_set_font_name(char_u *font_name)
{
    പൂർണ്ണസംഖ്യ	    id;

    id = syn_name2id((char_u *)"Normal");
    എങ്കിൽ (id <= 0)
	തിരികെ;

    vim_free(HL_TABLE()[id - 1].sg_font_name);
    HL_TABLE()[id - 1].sg_font_name = vim_strsave(font_name);
}

/*
 * Set background color for "Normal" group.  Called by gui_set_bg_color()
 * when the color is known.
 */
    ശൂന്യം
hl_set_bg_color_name(
    char_u  *name)	    // must have been allocated
{
    പൂർണ്ണസംഖ്യ	    id;

    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ;

    id = syn_name2id((char_u *)"Normal");
    എങ്കിൽ (id <= 0)
	തിരികെ;

    vim_free(HL_TABLE()[id - 1].sg_gui_bg_name);
    HL_TABLE()[id - 1].sg_gui_bg_name = name;
}

/*
 * Set foreground color for "Normal" group.  Called by gui_set_fg_color()
 * when the color is known.
 */
    ശൂന്യം
hl_set_fg_color_name(
    char_u  *name)	    // must have been allocated
{
    പൂർണ്ണസംഖ്യ	    id;

    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ;

    id = syn_name2id((char_u *)"Normal");
    എങ്കിൽ (id <= 0)
	തിരികെ;

    vim_free(HL_TABLE()[id - 1].sg_gui_fg_name);
    HL_TABLE()[id - 1].sg_gui_fg_name = name;
}

/*
 * Return the handle for a font name.
 * Returns NOFONT when failed.
 */
    സ്ഥിതം GuiFont
font_name2handle(char_u *name)
{
    എങ്കിൽ (STRCMP(name, "NONE") == 0)
	തിരികെ NOFONT;

    തിരികെ gui_mch_get_font(name, TRUE);
}

# ifdef FEAT_XFONTSET
/*
 * Return the handle for a fontset name.
 * Returns NOFONTSET when failed.
 */
    സ്ഥിതം GuiFontset
fontset_name2handle(char_u *name, പൂർണ്ണസംഖ്യ fixed_width)
{
    എങ്കിൽ (STRCMP(name, "NONE") == 0)
	തിരികെ NOFONTSET;

    തിരികെ gui_mch_get_fontset(name, TRUE, fixed_width);
}
# endif

/*
 * Get the font or fontset for one highlight group.
 */
    സ്ഥിതം ശൂന്യം
hl_do_font(
    പൂർണ്ണസംഖ്യ		idx,
    char_u	*arg,
    പൂർണ്ണസംഖ്യ		do_normal,		// set normal font
    പൂർണ്ണസംഖ്യ		do_menu UNUSED,		// set menu font
    പൂർണ്ണസംഖ്യ		do_tooltip UNUSED,	// set tooltip font
    പൂർണ്ണസംഖ്യ		free_font)		// free current font/fontset
{
# ifdef FEAT_XFONTSET
    // If 'guifontset' is not empty, first try using the name as a
    // fontset.  If that doesn't work, use it as a font name.
    എങ്കിൽ (*p_guifontset != NUL
#  ifdef FONTSET_ALWAYS
	|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
	// In Motif, the Tooltip highlight group is always a fontset
	|| do_tooltip
#  endif
	    )
    {
	എങ്കിൽ (free_font)
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
    എങ്കിൽ (HL_TABLE()[idx].sg_fontset != NOFONTSET)
    {
	// If it worked and it's the Normal group, use it as the normal
	// fontset.  Same for the Menu group.
	എങ്കിൽ (do_normal)
	    gui_init_font(arg, TRUE);
#  എങ്കിൽ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	എങ്കിൽ (do_menu)
	{
#   ifdef FONTSET_ALWAYS
	    gui.menu_fontset = HL_TABLE()[idx].sg_fontset;
#   അല്ലെങ്കിൽ
	    // YIKES!  This is a bug waiting to crash the program
	    gui.menu_font = HL_TABLE()[idx].sg_fontset;
#   endif
	    gui_mch_new_menu_font();
	}
#   ifdef FEAT_BEVAL_GUI
	എങ്കിൽ (do_tooltip)
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
    അല്ലെങ്കിൽ
# endif
    {
	എങ്കിൽ (free_font)
	    gui_mch_free_font(HL_TABLE()[idx].sg_font);
	HL_TABLE()[idx].sg_font = font_name2handle(arg);
	// If it worked and it's the Normal group, use it as the
	// normal font.  Same for the Menu group.
	എങ്കിൽ (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    എങ്കിൽ (do_normal)
		gui_init_font(arg, FALSE);
# ifndef FONTSET_ALWAYS
#  എങ്കിൽ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	    എങ്കിൽ (do_menu)
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

#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return the handle for a color name.
 * Returns INVALCOLOR when failed.
 */
    guicolor_T
color_name2handle(char_u *name)
{
    എങ്കിൽ (STRCMP(name, "NONE") == 0)
	തിരികെ INVALCOLOR;

    എങ്കിൽ (STRICMP(name, "fg") == 0 || STRICMP(name, "foreground") == 0)
    {
# എങ്കിൽ defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	എങ്കിൽ (gui.in_use)
# endif
# ifdef FEAT_GUI
	    തിരികെ gui.norm_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	എങ്കിൽ (cterm_normal_fg_gui_color != INVALCOLOR)
	    തിരികെ cterm_normal_fg_gui_color;
	// Guess that the foreground is black or white.
	തിരികെ GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "black" : "white"));
# endif
    }
    എങ്കിൽ (STRICMP(name, "bg") == 0 || STRICMP(name, "background") == 0)
    {
# എങ്കിൽ defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	എങ്കിൽ (gui.in_use)
# endif
# ifdef FEAT_GUI
	    തിരികെ gui.back_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	എങ്കിൽ (cterm_normal_bg_gui_color != INVALCOLOR)
	    തിരികെ cterm_normal_bg_gui_color;
	// Guess that the background is white or black.
	തിരികെ GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "white" : "black"));
# endif
    }

    തിരികെ GUI_GET_COLOR(name);
}

// On MS-Windows an RGB macro is available and it produces 0x00bbggrr color
// values as used by the MS-Windows GDI api.  It should be used only for
// MS-Windows GDI builds.
# എങ്കിൽ defined(RGB) && defined(MSWIN) && !defined(FEAT_GUI)
#  undef RGB
# endif
# ifndef RGB
#  define RGB(r, g, b)	(((r)<<16) | ((g)<<8) | (b))
# endif

# ifdef VIMDLL
    സ്ഥിതം guicolor_T
gui_adjust_rgb(guicolor_T c)
{
    എങ്കിൽ (gui.in_use)
	തിരികെ c;
    അല്ലെങ്കിൽ
	തിരികെ ((c & 0xff) << 16) | (c & 0x00ff00) | ((c >> 16) & 0xff);
}
# അല്ലെങ്കിൽ
#  define gui_adjust_rgb(c) (c)
# endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ
hex_digit(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (SAFE_isdigit(c))
	തിരികെ c - '0';
    c = TOLOWER_ASC(c);
    എങ്കിൽ (c >= 'a' && c <= 'f')
	തിരികെ c - 'a' + 10;
    തിരികെ 0x1ffffff;
}

    സ്ഥിതം guicolor_T
decode_hex_color(char_u *hex)
{
    guicolor_T color;

    എങ്കിൽ (hex[0] != '#' || STRLEN(hex) != 7)
	തിരികെ INVALCOLOR;

    // Name is in "#rrggbb" format
    color = RGB(((hex_digit(hex[1]) << 4) + hex_digit(hex[2])),
		((hex_digit(hex[3]) << 4) + hex_digit(hex[4])),
		((hex_digit(hex[5]) << 4) + hex_digit(hex[6])));
    എങ്കിൽ (color > 0xffffff)
	തിരികെ INVALCOLOR;
    തിരികെ gui_adjust_rgb(color);
}

# ifdef FEAT_EVAL
// Returns the color currently mapped to the given name or INVALCOLOR if no
// such name exists in the color table. The convention is to use lowercase for
// all keys in the v:colornames dictionary. The value can be either a string in
// the form #rrggbb or a number, either of which is converted to a guicolor_T.
    സ്ഥിതം guicolor_T
colorname2rgb(char_u *name)
{
    dict_T      *colornames_table = get_vim_var_dict(VV_COLORNAMES);
    char_u      *lc_name;
    dictitem_T  *colentry;
    char_u      *colstr;
    varnumber_T colnum;

    lc_name = strlow_save(name);
    എങ്കിൽ (lc_name == ശൂന്യ)
	തിരികെ INVALCOLOR;

    colentry = dict_find(colornames_table, lc_name, -1);
    vim_free(lc_name);
    എങ്കിൽ (colentry == ശൂന്യ)
	തിരികെ INVALCOLOR;

    എങ്കിൽ (colentry->di_tv.v_type == VAR_STRING)
    {
	colstr = tv_get_string_strict(&colentry->di_tv);
	എങ്കിൽ ((STRLEN(colstr) == 7) && (*colstr == '#'))
	{
	    തിരികെ decode_hex_color(colstr);
	}
	അല്ലെങ്കിൽ
	{
	    semsg(_(e_bad_color_string_str), colstr);
	    തിരികെ INVALCOLOR;
	}
    }

    എങ്കിൽ (colentry->di_tv.v_type == VAR_സംഖ്യ)
    {
	colnum = tv_get_സംഖ്യ(&colentry->di_tv);
	തിരികെ (guicolor_T)colnum;
    }

    തിരികെ INVALCOLOR;
}

# endif

    guicolor_T
gui_get_color_cmn(char_u *name)
{
    guicolor_T  color;
    // Only non X11 colors (not present in rgb.txt) and colors in
    // color_name_tab[], useful when $VIMRUNTIME is not found,.
    // must be sorted by the 'value' field because it is used by bsearch()!
    സ്ഥിതം keyvalue_T rgb_tab[] = {
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
    എങ്കിൽ (color != INVALCOLOR)
	തിരികെ color;

    target.താക്കോൽ = 0;
    target.മൂല്യം.string = name;
    target.മൂല്യം.length = 0;	// not used, see cmp_keyvalue_value_i()
    entry = (keyvalue_T *)bsearch(&target, &rgb_tab, ARRAY_LENGTH(rgb_tab),
	    വലുപ്പം(rgb_tab[0]), cmp_keyvalue_മൂല്യം_i);
    എങ്കിൽ (entry != ശൂന്യ)
	തിരികെ gui_adjust_rgb((guicolor_T)entry->താക്കോൽ);

# എങ്കിൽ defined(FEAT_EVAL)
    /*
     * Not a traditional color. Load additional color aliases and then consult the alias table.
     */

    color = colorname2rgb(name);
    എങ്കിൽ (color == INVALCOLOR)
    {
	load_default_colors_lists();
	color = colorname2rgb(name);
    }

    തിരികെ color;
# അല്ലെങ്കിൽ
    തിരികെ INVALCOLOR;
# endif
}

    guicolor_T
gui_get_rgb_color_cmn(പൂർണ്ണസംഖ്യ r, പൂർണ്ണസംഖ്യ g, പൂർണ്ണസംഖ്യ b)
{
    guicolor_T  color = RGB(r, g, b);

    എങ്കിൽ (color > 0xffffff)
	തിരികെ INVALCOLOR;
    തിരികെ gui_adjust_rgb(color);
}
#endif

/*
 * Table with the specifications for an attribute number.
 * Note that this table is used by ALL buffers.  This is required because the
 * GUI can redraw at any time for any buffer.
 */
സ്ഥിതം garray_T	term_attr_table = {0, 0, 0, 0, ശൂന്യ};

#define TERM_ATTR_ENTRY(idx) ((attrentry_T *)term_attr_table.ga_data)[idx]

സ്ഥിതം garray_T	cterm_attr_table = {0, 0, 0, 0, ശൂന്യ};

#define CTERM_ATTR_ENTRY(idx) ((attrentry_T *)cterm_attr_table.ga_data)[idx]

#ifdef FEAT_GUI
സ്ഥിതം garray_T	gui_attr_table = {0, 0, 0, 0, ശൂന്യ};

# define GUI_ATTR_ENTRY(idx) ((attrentry_T *)gui_attr_table.ga_data)[idx]
#endif

/*
 * Return the attr number for a set of colors and font.
 * Add a new entry to the term_attr_table, cterm_attr_table or gui_attr_table
 * if the combination is new.
 * Return 0 for error (no more room).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_attr_entry(garray_T *table, attrentry_T *aep)
{
    പൂർണ്ണസംഖ്യ		i;
    attrentry_T	*taep;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = FALSE;

    // Init the table, in case it wasn't done yet.
    table->ga_itemsize = വലുപ്പം(attrentry_T);
    table->ga_growsize = 7;

    // Try to find an entry with the same specifications.
    വേണ്ടി (i = 0; i < table->ga_len; ++i)
    {
	taep = &(((attrentry_T *)table->ga_data)[i]);
	എങ്കിൽ (	   aep->ae_attr == taep->ae_attr
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
			&& (aep->ae_u.term.start == ശൂന്യ)
					    == (taep->ae_u.term.start == ശൂന്യ)
			&& (aep->ae_u.term.start == ശൂന്യ
			    || STRCMP(aep->ae_u.term.start,
						  taep->ae_u.term.start) == 0)
			&& (aep->ae_u.term.stop == ശൂന്യ)
					     == (taep->ae_u.term.stop == ശൂന്യ)
			&& (aep->ae_u.term.stop == ശൂന്യ
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

	തിരികെ i + ATTR_OFF;
    }

    എങ്കിൽ (table->ga_len + ATTR_OFF > MAX_TYPENR)
    {
	// Running out of attribute entries!  remove all attributes, and
	// compute new ones for all groups.
	// When called recursively, we are really out of numbers.
	എങ്കിൽ (recursive)
	{
	    emsg(_(e_too_many_different_highlighting_attributes_in_use));
	    തിരികെ 0;
	}
	recursive = TRUE;

	clear_hl_tables();

	set_must_redraw(UPD_CLEAR);

	വേണ്ടി (i = 0; i < highlight_ga.ga_len; ++i)
	    set_hl_attr(i);

	recursive = FALSE;
    }

    // This is a new combination of colors and font, add an entry.
    എങ്കിൽ (ga_grow(table, 1) == FAIL)
	തിരികെ 0;

    taep = &(((attrentry_T *)table->ga_data)[table->ga_len]);
    CLEAR_POINTER(taep);
    taep->ae_attr = aep->ae_attr;
#ifdef FEAT_GUI
    എങ്കിൽ (table == &gui_attr_table)
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
    എങ്കിൽ (table == &term_attr_table)
    {
	എങ്കിൽ (aep->ae_u.term.start == ശൂന്യ)
	    taep->ae_u.term.start = ശൂന്യ;
	അല്ലെങ്കിൽ
	    taep->ae_u.term.start = vim_strsave(aep->ae_u.term.start);
	എങ്കിൽ (aep->ae_u.term.stop == ശൂന്യ)
	    taep->ae_u.term.stop = ശൂന്യ;
	അല്ലെങ്കിൽ
	    taep->ae_u.term.stop = vim_strsave(aep->ae_u.term.stop);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (table == &cterm_attr_table)
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
    തിരികെ (table->ga_len - 1 + ATTR_OFF);
}

#എങ്കിൽ defined(FEAT_TERMINAL)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    പൂർണ്ണസംഖ്യ
get_cterm_attr_idx(പൂർണ്ണസംഖ്യ attr, പൂർണ്ണസംഖ്യ fg, പൂർണ്ണസംഖ്യ bg)
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
    തിരികെ get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#എങ്കിൽ defined(FEAT_TERMINAL) && defined(FEAT_TERMGUICOLORS)
/*
 * Get an attribute index for a 'termguicolors' entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    പൂർണ്ണസംഖ്യ
get_tgc_attr_idx(പൂർണ്ണസംഖ്യ attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    എങ്കിൽ (fg == INVALCOLOR && bg == INVALCOLOR)
    {
	// If both GUI colors are not set fall back to the cterm colors.  Helps
	// if the GUI only has an attribute, such as undercurl.
	at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
    }
    അല്ലെങ്കിൽ
    {
	at_en.ae_u.cterm.fg_rgb = fg;
	at_en.ae_u.cterm.bg_rgb = bg;
    }
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
    തിരികെ get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#എങ്കിൽ defined(FEAT_TERMINAL) && defined(FEAT_GUI)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    പൂർണ്ണസംഖ്യ
get_gui_attr_idx(പൂർണ്ണസംഖ്യ attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    at_en.ae_u.gui.fg_color = fg;
    at_en.ae_u.gui.bg_color = bg;
    തിരികെ get_attr_entry(&gui_attr_table, &at_en);
}
#endif

/*
 * Clear all highlight tables.
 */
    ശൂന്യം
clear_hl_tables(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;
    attrentry_T	*taep;

#ifdef FEAT_GUI
    ga_clear(&gui_attr_table);
#endif
    വേണ്ടി (i = 0; i < term_attr_table.ga_len; ++i)
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
    പൂർണ്ണസംഖ്യ
hl_combine_attr(പൂർണ്ണസംഖ്യ char_attr, പൂർണ്ണസംഖ്യ prim_attr)
{
    attrentry_T *char_aep = ശൂന്യ;
    attrentry_T *prim_aep;
    attrentry_T new_en;

    എങ്കിൽ (char_attr == 0)
	തിരികെ prim_attr;
    എങ്കിൽ (char_attr <= HL_ALL && prim_attr <= HL_ALL)
	തിരികെ ATTR_COMBINE(char_attr, prim_attr);
#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
    {
	എങ്കിൽ (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	എങ്കിൽ (char_aep != ശൂന്യ)
	    new_en = *char_aep;
	അല്ലെങ്കിൽ
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    എങ്കിൽ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	എങ്കിൽ (prim_attr <= HL_ALL)
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	അല്ലെങ്കിൽ
	{
	    prim_aep = syn_gui_attr2entry(prim_attr);
	    എങ്കിൽ (prim_aep != ശൂന്യ)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		എങ്കിൽ (prim_aep->ae_u.gui.fg_color != INVALCOLOR)
		    new_en.ae_u.gui.fg_color = prim_aep->ae_u.gui.fg_color;
		എങ്കിൽ (prim_aep->ae_u.gui.bg_color != INVALCOLOR)
		    new_en.ae_u.gui.bg_color = prim_aep->ae_u.gui.bg_color;
		എങ്കിൽ (prim_aep->ae_u.gui.sp_color != INVALCOLOR)
		    new_en.ae_u.gui.sp_color = prim_aep->ae_u.gui.sp_color;
		എങ്കിൽ (prim_aep->ae_u.gui.font != NOFONT)
		    new_en.ae_u.gui.font = prim_aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
		എങ്കിൽ (prim_aep->ae_u.gui.fontset != NOFONTSET)
		    new_en.ae_u.gui.fontset = prim_aep->ae_u.gui.fontset;
# endif
	    }
	}
	തിരികെ get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    എങ്കിൽ (IS_CTERM)
    {
	എങ്കിൽ (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	എങ്കിൽ (char_aep != ശൂന്യ)
	    new_en = *char_aep;
	അല്ലെങ്കിൽ
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    എങ്കിൽ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	എങ്കിൽ (prim_attr <= HL_ALL)
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	അല്ലെങ്കിൽ
	{
	    prim_aep = syn_cterm_attr2entry(prim_attr);
	    എങ്കിൽ (prim_aep != ശൂന്യ)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		എങ്കിൽ (prim_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color = prim_aep->ae_u.cterm.fg_color;
		എങ്കിൽ (prim_aep->ae_u.cterm.bg_color > 0)
		    new_en.ae_u.cterm.bg_color = prim_aep->ae_u.cterm.bg_color;
		എങ്കിൽ (prim_aep->ae_u.cterm.ul_color > 0)
		    new_en.ae_u.cterm.ul_color = prim_aep->ae_u.cterm.ul_color;
		എങ്കിൽ (prim_aep->ae_u.cterm.font > 0)
		    new_en.ae_u.cterm.font = prim_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		// If both fg and bg are not set fall back to cterm colors.
		// Helps for SpellBad which uses undercurl in the GUI.
		എങ്കിൽ (COLOR_INVALID(prim_aep->ae_u.cterm.fg_rgb)
			&& COLOR_INVALID(prim_aep->ae_u.cterm.bg_rgb))
		{
		    എങ്കിൽ (prim_aep->ae_u.cterm.fg_color > 0)
			new_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
		    എങ്കിൽ (prim_aep->ae_u.cterm.bg_color > 0)
			new_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (prim_aep->ae_u.cterm.fg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.fg_rgb = prim_aep->ae_u.cterm.fg_rgb;
		    എങ്കിൽ (prim_aep->ae_u.cterm.bg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.bg_rgb = prim_aep->ae_u.cterm.bg_rgb;
		}
		എങ്കിൽ (prim_aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		    new_en.ae_u.cterm.ul_rgb = prim_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	}
	തിരികെ get_attr_entry(&cterm_attr_table, &new_en);
    }

    എങ്കിൽ (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    എങ്കിൽ (char_aep != ശൂന്യ)
	new_en = *char_aep;
    അല്ലെങ്കിൽ
    {
	CLEAR_FIELD(new_en);
	എങ്കിൽ (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    എങ്കിൽ (prim_attr <= HL_ALL)
	new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
    അല്ലെങ്കിൽ
    {
	prim_aep = syn_term_attr2entry(prim_attr);
	എങ്കിൽ (prim_aep != ശൂന്യ)
	{
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_aep->ae_attr);
	    എങ്കിൽ (prim_aep->ae_u.term.start != ശൂന്യ)
	    {
		new_en.ae_u.term.start = prim_aep->ae_u.term.start;
		new_en.ae_u.term.stop = prim_aep->ae_u.term.stop;
	    }
	}
    }
    തിരികെ get_attr_entry(&term_attr_table, &new_en);
}

// ANSI color order: Black, Red, Green, Yellow, Blue, Magenta,
// Cyan, White, then bright variants.  Approximate RGB values used when
// only a cterm color number is known (no guifg/guibg).  Real terminal
// palettes may differ if the user reconfigured their emulator, but
// these are reasonable xterm-ish defaults.
സ്ഥിതം സ്ഥിരം long_u cterm_color_16[16] = {
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
    സ്ഥിതം guicolor_T
cterm_color_to_rgb(പൂർണ്ണസംഖ്യ color_nr)
{
    എങ്കിൽ (color_nr < 1 || color_nr > 16)
	തിരികെ INVALCOLOR;
    തിരികെ (guicolor_T)cterm_color_16[color_nr - 1];
}
#endif

/*
 * Convert an xterm 256-color index (0-255) to an approximate RGB triple.
 * Uses the standard xterm palette: 0-15 ANSI (cterm_color_16), 16-231
 * 6x6x6 cube, 232-255 grayscale ramp.
 */
    സ്ഥിതം ശൂന്യം
cterm_idx_to_rgb(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ *r, പൂർണ്ണസംഖ്യ *g, പൂർണ്ണസംഖ്യ *b)
{
    സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

    എങ്കിൽ (idx < 0 || idx > 255)
    {
	*r = *g = *b = 0;
	തിരികെ;
    }
    എങ്കിൽ (idx < 16)
    {
	long_u rgb = cterm_color_16[idx];
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (idx < 232)
    {
	പൂർണ്ണസംഖ്യ n = idx - 16;
	*r = cube[n / 36 % 6];
	*g = cube[n / 6  % 6];
	*b = cube[n      % 6];
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ v = 8 + (idx - 232) * 10;
	*r = *g = *b = v;
    }
}

/*
 * Approximate an RGB triple to the nearest xterm 256-color index.
 * Searches the 6x6x6 cube and the grayscale ramp; returns 0-255.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
rgb_to_cterm_idx(പൂർണ്ണസംഖ്യ r, പൂർണ്ണസംഖ്യ g, പൂർണ്ണസംഖ്യ b)
{
    സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };
    പൂർണ്ണസംഖ്യ best = 0;
    long best_d = -1;
    പൂർണ്ണസംഖ്യ i;

    // Search the 6x6x6 cube.
    വേണ്ടി (i = 16; i < 232; ++i)
    {
	പൂർണ്ണസംഖ്യ n = i - 16;
	പൂർണ്ണസംഖ്യ cr = cube[n / 36 % 6];
	പൂർണ്ണസംഖ്യ cg = cube[n / 6  % 6];
	പൂർണ്ണസംഖ്യ cb = cube[n      % 6];
	long d = (cr - r) * (cr - r) + (cg - g) * (cg - g) + (cb - b) * (cb - b);
	എങ്കിൽ (best_d < 0 || d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    // Search the grayscale ramp.
    വേണ്ടി (i = 232; i < 256; ++i)
    {
	പൂർണ്ണസംഖ്യ v = 8 + (i - 232) * 10;
	long d = (v - r) * (v - r) + (v - g) * (v - g) + (v - b) * (v - b);
	എങ്കിൽ (d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    തിരികെ best;
}

/*
 * Resolve a color to an RGB triple.  Prefer the gui RGB value (set by
 * guifg/guibg) when valid, otherwise convert the cterm 256-color index.
 * Returns true on success; false when no color is defined.
 * cterm_c is 1-based (0 means "no color").
 */
    സ്ഥിതം bool
resolve_color_to_rgb(പൂർണ്ണസംഖ്യ cterm_c, guicolor_T rgb UNUSED, പൂർണ്ണസംഖ്യ *r, പൂർണ്ണസംഖ്യ *g, പൂർണ്ണസംഖ്യ *b)
{
#ifdef FEAT_TERMGUICOLORS
    എങ്കിൽ (!COLOR_INVALID(rgb))
    {
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
	തിരികെ true;
    }
#endif
    എങ്കിൽ (cterm_c > 0)
    {
	cterm_idx_to_rgb(cterm_c - 1, r, g, b);
	തിരികെ true;
    }
    തിരികെ false;
}

/*
 * get a RGB fallback color from gui, cterm or default color
 */
    സ്ഥിതം guicolor_T
resolve_fallback_color(പൂർണ്ണസംഖ്യ cterm_c, guicolor_T rgb, guicolor_T default_rgb)
{
    പൂർണ്ണസംഖ്യ red, green, blue;
    എങ്കിൽ (!resolve_color_to_rgb(cterm_c, rgb, &red, &green, &blue))
	തിരികെ default_rgb;
    അല്ലെങ്കിൽ
	തിരികെ (red << 16) | (green << 8) | blue;
}

/*
 * get a RGB fallback foreground color from guifg, ctermfg or deduced from background
 */
    സ്ഥിതം ശൂന്യം
resolve_fallback_fg_to_rgb(ശൂന്യം)
{
    guicolor_T fgcolor_or_gui_fgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    fgcolor_or_gui_fgcolor = cterm_normal_fg_gui_color;
#endif
#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	fgcolor_or_gui_fgcolor = gui.norm_pixel;
#endif
    fallback_fg_rgb = resolve_fallback_color(cterm_normal_fg_color, fgcolor_or_gui_fgcolor, (*p_bg == 'l') ? 0x000000 : 0xFFFFFF);
}

/*
 * get a RGB fallback background color from guifg, ctermbg or deduced from background
 */
    സ്ഥിതം ശൂന്യം
resolve_fallback_bg_to_rgb(ശൂന്യം)
{
    guicolor_T bgcolor_or_gui_bgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    bgcolor_or_gui_bgcolor = cterm_normal_bg_gui_color;
#endif
#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
blend_cterm_colors(പൂർണ്ണസംഖ്യ popup_c, guicolor_T popup_rgb,
		   പൂർണ്ണസംഖ്യ under_c, guicolor_T under_rgb,
		   പൂർണ്ണസംഖ്യ default_rgb,
		   പൂർണ്ണസംഖ്യ blend_val)
{
    പൂർണ്ണസംഖ്യ pr, pg, pb, ur, ug, ub, r, g, b;

    എങ്കിൽ (t_colors < 256)
	തിരികെ popup_c;
    എങ്കിൽ (!resolve_color_to_rgb(popup_c, popup_rgb, &pr, &pg, &pb))
	തിരികെ under_c;
    എങ്കിൽ (blend_val <= 0)
	തിരികെ rgb_to_cterm_idx(pr, pg, pb) + 1;
    എങ്കിൽ (!resolve_color_to_rgb(under_c, under_rgb, &ur, &ug, &ub))
    {
	ur = (default_rgb >> 16) & 0xFF;
	ug = (default_rgb >> 8) & 0xFF;
	ub = default_rgb & 0xFF;
    }
    എങ്കിൽ (blend_val >= 100)
	തിരികെ rgb_to_cterm_idx(ur, ug, ub) + 1;
    r = pr + (ur - pr) * blend_val / 100;
    g = pg + (ug - pg) * blend_val / 100;
    b = pb + (ub - pb) * blend_val / 100;
    തിരികെ rgb_to_cterm_idx(r, g, b) + 1;
}

#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Blend two RGB colors based on blend value (0-100).
 * blend: 0=return first argument color, 100=return second argument color
 * If bg_color is INVALCOLOR, high blend means more visible (return INVALCOLOR).
 */
    സ്ഥിതം guicolor_T
blend_colors(guicolor_T popup_color, guicolor_T bg_color, പൂർണ്ണസംഖ്യ blend_val)
{
    പൂർണ്ണസംഖ്യ r1, g1, b1, r2, g2, b2, r, g, b;

    // CTERMCOLOR is a sentinel meaning "use the cterm color"; for blending
    // it has no real RGB so treat it like INVALCOLOR.
    എങ്കിൽ (COLOR_INVALID(popup_color))
	തിരികെ INVALCOLOR;

    // Fully transparent: use underlying color as-is.
    എങ്കിൽ (blend_val >= 100)
	തിരികെ bg_color;

    r1 = (popup_color >> 16) & 0xFF;
    g1 = (popup_color >> 8) & 0xFF;
    b1 = popup_color & 0xFF;

    എങ്കിൽ (COLOR_INVALID(bg_color))
	bg_color = fallback_bg_rgb;

    r2 = (bg_color >> 16) & 0xFF;
    g2 = (bg_color >> 8) & 0xFF;
    b2 = bg_color & 0xFF;

    r = r1 + (r2 - r1) * blend_val / 100;
    g = g1 + (g2 - g1) * blend_val / 100;
    b = b1 + (b2 - b1) * blend_val / 100;

    തിരികെ (r << 16) | (g << 8) | b;
}
#endif

/*
 * Blend attributes for popup windows and menus with opacity.
 * Blends foreground and/or background colors based on blend value (0-100).
 * blend: 0 = opaque (use popup colors), 100 = transparent (use background colors)
 * blend_fg: TRUE to blend foreground color, FALSE to keep popup foreground
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
hl_blend_attr_common(
    പൂർണ്ണസംഖ്യ		char_attr,
    പൂർണ്ണസംഖ്യ		popup_attr,
    പൂർണ്ണസംഖ്യ		blend,
    പൂർണ്ണസംഖ്യ		blend_fg)
{
    attrentry_T *char_aep = ശൂന്യ;
    attrentry_T *popup_aep;
    attrentry_T new_en;
    attrentry_T tmp_en;

#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
    {
	എങ്കിൽ (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	എങ്കിൽ (char_aep != ശൂന്യ)
	    new_en = *char_aep;
	അല്ലെങ്കിൽ
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    എങ്കിൽ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	എങ്കിൽ (popup_attr <= HL_ALL)
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
	അല്ലെങ്കിൽ
	    popup_aep = syn_gui_attr2entry(popup_attr);

	എങ്കിൽ (popup_aep != ശൂന്യ)
	{
	    guicolor_T popup_bg_rgb = popup_aep->ae_u.gui.bg_color;
	    എങ്കിൽ (COLOR_INVALID(popup_bg_rgb))
		popup_bg_rgb = fallback_bg_rgb;

	    എങ്കിൽ (blend_fg)
	    {
		// blend_fg=TRUE: fade underlying text toward popup bg.
		{
		    പൂർണ്ണസംഖ്യ base_fg = fallback_fg_rgb;
		    എങ്കിൽ (char_aep != ശൂന്യ
			    && char_aep->ae_u.gui.fg_color != INVALCOLOR)
			base_fg = char_aep->ae_u.gui.fg_color;
		    new_en.ae_u.gui.fg_color = blend_colors(
			    popup_bg_rgb, base_fg, blend);
		}
	    }
	    അല്ലെങ്കിൽ
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
		എങ്കിൽ (char_aep != ശൂന്യ
			&& !COLOR_INVALID(char_aep->ae_u.gui.bg_color))
		    underlying_bg = char_aep->ae_u.gui.bg_color;
		new_en.ae_u.gui.bg_color = blend_colors(
			popup_bg_rgb,
			underlying_bg, blend);
	    }
	}
	തിരികെ get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    എങ്കിൽ (IS_CTERM)
    {
	എങ്കിൽ (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	എങ്കിൽ (char_aep != ശൂന്യ)
	    new_en = *char_aep;
	അല്ലെങ്കിൽ
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    എങ്കിൽ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	എങ്കിൽ (popup_attr <= HL_ALL)
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
	അല്ലെങ്കിൽ
	    popup_aep = syn_cterm_attr2entry(popup_attr);

	എങ്കിൽ (popup_aep != ശൂന്യ)
	{
	    guicolor_T popup_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
	    // Fall back to cterm color converted to RGB when gui color is not set.
	    popup_bg_rgb = popup_aep->ae_u.cterm.bg_rgb;
	    എങ്കിൽ (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color > 0)
		popup_bg_rgb = cterm_color_to_rgb(
			popup_aep->ae_u.cterm.bg_color);
#endif
	    // assign default color if guibg and ctermbg are not set for popup
	    എങ്കിൽ (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color == 0)
		popup_bg_rgb = fallback_bg_rgb;

	    എങ്കിൽ (!blend_fg)
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
		എങ്കിൽ (popup_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color =
			popup_aep->ae_u.cterm.fg_color;
		അല്ലെങ്കിൽ എങ്കിൽ (cterm_normal_fg_color > 0)
		    new_en.ae_u.cterm.fg_color = cterm_normal_fg_color;
		അല്ലെങ്കിൽ
		    // black-ish or white-ish
		    new_en.ae_u.cterm.fg_color = (*p_bg == 'l') ? 1 : 16;
		new_en.ae_u.cterm.ul_color = popup_aep->ae_u.cterm.ul_color;
		new_en.ae_u.cterm.font = popup_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		new_en.ae_u.cterm.ul_rgb = popup_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	    അല്ലെങ്കിൽ
	    {
		// blend_fg=TRUE: fade underlying fg toward popup bg in
		// the 256-color palette.  Used when the popup is over a
		// cell rendered with cterm colors (no termguicolors RGB).
		പൂർണ്ണസംഖ്യ under_fg = (char_aep != ശൂന്യ)
		    ? char_aep->ae_u.cterm.fg_color : 0;
		guicolor_T under_fg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		എങ്കിൽ (char_aep != ശൂന്യ)
		    under_fg_rgb = char_aep->ae_u.cterm.fg_rgb;
#endif
		new_en.ae_u.cterm.fg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_fg, under_fg_rgb, fallback_fg_rgb, blend);
	    }
	    // Approximate cterm bg by blending with the underlying bg
	    // in the 256-color palette and mapping to the nearest entry.
	    {
		പൂർണ്ണസംഖ്യ under_bg = (char_aep != ശൂന്യ)
		    ? char_aep->ae_u.cterm.bg_color : 0;
		guicolor_T under_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		എങ്കിൽ (char_aep != ശൂന്യ)
		    under_bg_rgb = char_aep->ae_u.cterm.bg_rgb;
#endif
		new_en.ae_u.cterm.bg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_bg, under_bg_rgb, fallback_bg_rgb, blend);
	    }
#ifdef FEAT_TERMGUICOLORS
	    // Blend RGB colors for termguicolors mode.
	    {
		എങ്കിൽ (blend_fg)
		{
		    // blend_fg=TRUE: fade underlying text toward popup bg.
		    എങ്കിൽ (popup_bg_rgb != INVALCOLOR)
		    {
			പൂർണ്ണസംഖ്യ base_fg = fallback_fg_rgb;
			// CTERMCOLOR is a sentinel meaning "use the cterm
			// color"; treat it as no underlying color so it is
			// not blended in as a real near-white pixel.
			എങ്കിൽ (char_aep != ശൂന്യ
				&& !COLOR_INVALID(char_aep->ae_u.cterm.fg_rgb))
			    base_fg = char_aep->ae_u.cterm.fg_rgb;
			new_en.ae_u.cterm.fg_rgb = blend_colors(
				popup_bg_rgb, base_fg, blend);
		    }
		}
		അല്ലെങ്കിൽ
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

		    എങ്കിൽ (COLOR_INVALID(popup_fg)
			    && popup_aep->ae_u.cterm.fg_color > 0)
			popup_fg = cterm_color_to_rgb(
				popup_aep->ae_u.cterm.fg_color);

		    എങ്കിൽ (!COLOR_INVALID(popup_fg))
			new_en.ae_u.cterm.fg_rgb = popup_fg;
		    അല്ലെങ്കിൽ എങ്കിൽ (!COLOR_INVALID(cterm_normal_fg_gui_color))
			new_en.ae_u.cterm.fg_rgb = cterm_normal_fg_gui_color;
		    അല്ലെങ്കിൽ
			new_en.ae_u.cterm.fg_rgb = fallback_fg_rgb;
		}
		എങ്കിൽ (popup_bg_rgb != INVALCOLOR)
		{
		    // Blend popup bg toward underlying bg
		    guicolor_T underlying_bg = fallback_bg_rgb;
		    എങ്കിൽ (char_aep != ശൂന്യ
			    && !COLOR_INVALID(char_aep->ae_u.cterm.bg_rgb))
			underlying_bg = char_aep->ae_u.cterm.bg_rgb;
		    new_en.ae_u.cterm.bg_rgb = blend_colors(
			    popup_bg_rgb, underlying_bg, blend);
		}
	    }
#endif
	}
	തിരികെ get_attr_entry(&cterm_attr_table, &new_en);
    }

    എങ്കിൽ (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    എങ്കിൽ (char_aep != ശൂന്യ)
	new_en = *char_aep;
    അല്ലെങ്കിൽ
    {
	CLEAR_FIELD(new_en);
	എങ്കിൽ (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    // For term mode, no separate background color handling.
    തിരികെ get_attr_entry(&term_attr_table, &new_en);
}

/*
 * Blend attributes for popup windows with opacity.
 */
    പൂർണ്ണസംഖ്യ
hl_blend_attr(പൂർണ്ണസംഖ്യ char_attr, പൂർണ്ണസംഖ്യ popup_attr, പൂർണ്ണസംഖ്യ blend, പൂർണ്ണസംഖ്യ blend_fg)
{
    എങ്കിൽ (blend >= 100 && blend_fg)
	തിരികെ char_attr;

    തിരികെ hl_blend_attr_common(char_attr, popup_attr, blend, blend_fg);
}

/*
 * Blend underlying foreground and background colors for pum space cells.
 */
    പൂർണ്ണസംഖ്യ
hl_pum_blend_attr(പൂർണ്ണസംഖ്യ char_attr, പൂർണ്ണസംഖ്യ popup_attr, പൂർണ്ണസംഖ്യ blend)
{
    തിരികെ hl_blend_attr_common(char_attr, popup_attr, blend, TRUE);
}

#ifdef FEAT_GUI
    attrentry_T *
syn_gui_attr2entry(പൂർണ്ണസംഖ്യ attr)
{
    attr -= ATTR_OFF;
    എങ്കിൽ (attr >= gui_attr_table.ga_len)	    // did ":syntax clear"
	തിരികെ ശൂന്യ;
    തിരികെ &(GUI_ATTR_ENTRY(attr));
}
#endif

/*
 * Get the highlight attributes (HL_BOLD etc.) from an attribute nr.
 * Only to be used when "attr" > HL_ALL.
 */
    പൂർണ്ണസംഖ്യ
syn_attr2attr(പൂർണ്ണസംഖ്യ attr)
{
    attrentry_T	*aep;

#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	aep = syn_gui_attr2entry(attr);
    അല്ലെങ്കിൽ
#endif
	എങ്കിൽ (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	അല്ലെങ്കിൽ
	    aep = syn_term_attr2entry(attr);

    എങ്കിൽ (aep == ശൂന്യ)	    // highlighting not set
	തിരികെ 0;
    തിരികെ aep->ae_attr;
}


    attrentry_T *
syn_term_attr2entry(പൂർണ്ണസംഖ്യ attr)
{
    attr -= ATTR_OFF;
    എങ്കിൽ (attr >= term_attr_table.ga_len)	    // did ":syntax clear"
	തിരികെ ശൂന്യ;
    തിരികെ &(TERM_ATTR_ENTRY(attr));
}

    attrentry_T *
syn_cterm_attr2entry(പൂർണ്ണസംഖ്യ attr)
{
    attr -= ATTR_OFF;
    എങ്കിൽ (attr >= cterm_attr_table.ga_len)	// did ":syntax clear"
	തിരികെ ശൂന്യ;
    തിരികെ &(CTERM_ATTR_ENTRY(attr));
}

#define LIST_ATTR   1
#define LIST_STRING 2
#define LIST_INT    3

    സ്ഥിതം ശൂന്യം
highlight_list_one(പൂർണ്ണസംഖ്യ id)
{
    hl_group_T	    *sgp;
    പൂർണ്ണസംഖ്യ		    didh = FALSE;

    sgp = &HL_TABLE()[id - 1];	    // index is ID minus one

    എങ്കിൽ (message_filtered(sgp->sg_name))
	തിരികെ;

    // Note: Keep this in sync with expand_highlight_group().
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_term, ശൂന്യ, "term");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_start, "start");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_stop, "stop");

    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_cterm, ശൂന്യ, "cterm");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_fg, ശൂന്യ, "ctermfg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_bg, ശൂന്യ, "ctermbg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_ul, ശൂന്യ, "ctermul");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_font, ശൂന്യ, "ctermfont");

#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_EVAL)
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_gui, ശൂന്യ, "gui");
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

    എങ്കിൽ (sgp->sg_link && !got_int)
    {
	(ശൂന്യം)syn_list_header(didh, 9999, id);
	didh = TRUE;
	സന്ദേശം_puts_attr("links to", HL_ATTR(HLF_D));
	സന്ദേശം_putchar(' ');
	സന്ദേശം_outtrans(HL_TABLE()[HL_TABLE()[id - 1].sg_link - 1].sg_name);
    }

    എങ്കിൽ (!didh)
	highlight_list_arg(id, didh, LIST_STRING, 0, (char_u *)"cleared", "");
#ifdef FEAT_EVAL
    എങ്കിൽ (p_verbose > 0)
	last_set_സന്ദേശം(sgp->sg_script_ctx);
#endif
}

    സ്ഥിതം char_u*
highlight_arg_to_string(
    പൂർണ്ണസംഖ്യ		type,
    പൂർണ്ണസംഖ്യ		iarg,
    char_u	*sarg,
    char_u	*buf)
{
    എങ്കിൽ (type == LIST_INT)
	sprintf((അക്ഷരം *)buf, "%d", iarg - 1);
    അല്ലെങ്കിൽ എങ്കിൽ (type == LIST_STRING)
	തിരികെ sarg;
    അല്ലെങ്കിൽ // type == LIST_ATTR
    {
	size_t buflen;

	buf[0] = NUL;
	buflen = 0;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(highlight_index_tab); ++i)
	{
	    എങ്കിൽ (iarg & highlight_index_tab[i]->താക്കോൽ)
	    {
		എങ്കിൽ (buflen > 0)
		{
		    STRCPY(buf + buflen, (char_u *)",");
		    ++buflen;
		}
		STRCPY(buf + buflen, highlight_index_tab[i]->മൂല്യം.string);
		buflen += highlight_index_tab[i]->മൂല്യം.length;
		iarg &= ~highlight_index_tab[i]->താക്കോൽ;	    // don't want "inverse"/"reverse"
	    }
	}
    }
    തിരികെ buf;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
highlight_list_arg(
    പൂർണ്ണസംഖ്യ		id,
    പൂർണ്ണസംഖ്യ		didh,
    പൂർണ്ണസംഖ്യ		type,
    പൂർണ്ണസംഖ്യ		iarg,
    char_u	*sarg,
    അക്ഷരം	*name)
{
    char_u	buf[MAX_ATTR_LEN];
    char_u	*ts;

    എങ്കിൽ (got_int)
	തിരികെ FALSE;

    എങ്കിൽ (type == LIST_STRING ? (sarg == ശൂന്യ) : (iarg == 0))
	തിരികെ didh;

    ts = highlight_arg_to_string(type, iarg, sarg, buf);

    (ശൂന്യം)syn_list_header(didh,
	    (പൂർണ്ണസംഖ്യ)(vim_strsize(ts) + STRLEN(name) + 1), id);
    didh = TRUE;
    എങ്കിൽ (!got_int)
    {
	എങ്കിൽ (*name != NUL)
	{
	    സന്ദേശം_puts_attr(name, HL_ATTR(HLF_D));
	    സന്ദേശം_puts_attr("=", HL_ATTR(HLF_D));
	}
	സന്ദേശം_outtrans(ts);
    }
    തിരികെ didh;
}

#എങ്കിൽ ((defined(FEAT_EVAL) || defined(FEAT_PRINTER))) && defined(FEAT_SYN_HL)
/*
 * Return "1" if highlight group "id" has attribute "flag".
 * Return NULL otherwise.
 */
    char_u *
highlight_has_attr(
    പൂർണ്ണസംഖ്യ		id,
    പൂർണ്ണസംഖ്യ		flag,
    പൂർണ്ണസംഖ്യ		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    പൂർണ്ണസംഖ്യ		attr;

    എങ്കിൽ (id <= 0 || id > highlight_ga.ga_len)
	തിരികെ ശൂന്യ;

# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_EVAL)
    എങ്കിൽ (modec == 'g')
	attr = HL_TABLE()[id - 1].sg_gui;
    അല്ലെങ്കിൽ
# endif
    {
	എങ്കിൽ (modec == 'c')
	    attr = HL_TABLE()[id - 1].sg_cterm;
	അല്ലെങ്കിൽ
	    attr = HL_TABLE()[id - 1].sg_term;
    }

    എങ്കിൽ (attr & flag)
	തിരികെ (char_u *)"1";
    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(FEAT_SYN_HL) && defined(FEAT_EVAL)
/*
 * Return color name of highlight group "id".
 */
    char_u *
highlight_color(
    പൂർണ്ണസംഖ്യ		id,
    char_u	*what,	// "font", "fg", "bg", "sp", "ul", "fg#", "bg#" or "sp#"
    പൂർണ്ണസംഖ്യ		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    സ്ഥിതം char_u	name[20];
    പൂർണ്ണസംഖ്യ			n;
    പൂർണ്ണസംഖ്യ			fg = FALSE;
    പൂർണ്ണസംഖ്യ			sp = FALSE;
    പൂർണ്ണസംഖ്യ			ul = FALSE;
    പൂർണ്ണസംഖ്യ			font = FALSE;

    എങ്കിൽ (id <= 0 || id > highlight_ga.ga_len)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'g')
	fg = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'o'
	     && TOLOWER_ASC(what[2]) == 'n' && TOLOWER_ASC(what[3]) == 't')
	font = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ (TOLOWER_ASC(what[0]) == 's' && TOLOWER_ASC(what[1]) == 'p')
	sp = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ (TOLOWER_ASC(what[0]) == 'u' && TOLOWER_ASC(what[1]) == 'l')
	ul = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ (!(TOLOWER_ASC(what[0]) == 'b' && TOLOWER_ASC(what[1]) == 'g'))
	തിരികെ ശൂന്യ;
    എങ്കിൽ (modec == 'g')
    {
# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI
	// return font name
	എങ്കിൽ (font)
	    തിരികെ HL_TABLE()[id - 1].sg_font_name;
#  endif

	// return #RRGGBB form (only possible when GUI is running)
	എങ്കിൽ ((USE_24BIT) && what[2] == '#')
	{
	    guicolor_T		color;
	    long_u		rgb;
	    സ്ഥിതം char_u	buf[10];

	    എങ്കിൽ (fg)
		color = HL_TABLE()[id - 1].sg_gui_fg;
	    അല്ലെങ്കിൽ എങ്കിൽ (sp)
		color = HL_TABLE()[id - 1].sg_gui_sp;
	    അല്ലെങ്കിൽ
		color = HL_TABLE()[id - 1].sg_gui_bg;
	    എങ്കിൽ (color == INVALCOLOR)
		തിരികെ ശൂന്യ;
	    rgb = (long_u)GUI_MCH_GET_RGB(color);
	    sprintf((അക്ഷരം *)buf, "#%02x%02x%02x",
				      (unsigned)(rgb >> 16),
				      (unsigned)(rgb >> 8) & 255,
				      (unsigned)rgb & 255);
	    തിരികെ buf;
	}
# endif
	എങ്കിൽ (fg)
	    തിരികെ (HL_TABLE()[id - 1].sg_gui_fg_name);
	എങ്കിൽ (sp)
	    തിരികെ (HL_TABLE()[id - 1].sg_gui_sp_name);
	തിരികെ (HL_TABLE()[id - 1].sg_gui_bg_name);
    }
    എങ്കിൽ (sp)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (modec == 'c')
    {
	എങ്കിൽ (fg)
	    n = HL_TABLE()[id - 1].sg_cterm_fg - 1;
	അല്ലെങ്കിൽ എങ്കിൽ (ul)
	    n = HL_TABLE()[id - 1].sg_cterm_ul - 1;
	അല്ലെങ്കിൽ എങ്കിൽ (font)
	    n = HL_TABLE()[id - 1].sg_cterm_font - 1;
	അല്ലെങ്കിൽ
	    n = HL_TABLE()[id - 1].sg_cterm_bg - 1;
	എങ്കിൽ (n < 0)
	    തിരികെ ശൂന്യ;
	sprintf((അക്ഷരം *)name, "%d", n);
	തിരികെ name;
    }
    // term doesn't have color
    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(FEAT_SYN_HL) \
	    && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)) \
	&& defined(FEAT_PRINTER)
/*
 * Return color name of highlight group "id" as RGB value.
 */
    long_u
highlight_gui_color_rgb(
    പൂർണ്ണസംഖ്യ		id,
    പൂർണ്ണസംഖ്യ		fg)	// TRUE = fg, FALSE = bg
{
    guicolor_T	color;

    എങ്കിൽ (id <= 0 || id > highlight_ga.ga_len)
	തിരികെ 0L;

    എങ്കിൽ (fg)
	color = HL_TABLE()[id - 1].sg_gui_fg;
    അല്ലെങ്കിൽ
	color = HL_TABLE()[id - 1].sg_gui_bg;

    എങ്കിൽ (color == INVALCOLOR)
	തിരികെ 0L;

    തിരികെ GUI_MCH_GET_RGB(color);
}
#endif

/*
 * Output the syntax list header.
 * Return TRUE when started a new line.
 */
    പൂർണ്ണസംഖ്യ
syn_list_header(
    പൂർണ്ണസംഖ്യ	    did_header,		// did header already
    പൂർണ്ണസംഖ്യ	    outlen,		// length of string that comes
    പൂർണ്ണസംഖ്യ	    id)			// highlight group id
{
    പൂർണ്ണസംഖ്യ	    endcol = 19;
    പൂർണ്ണസംഖ്യ	    newline = TRUE;
    പൂർണ്ണസംഖ്യ	    name_col = 0;

    എങ്കിൽ (!did_header)
    {
	സന്ദേശം_putchar('\n');
	എങ്കിൽ (got_int)
	    തിരികെ TRUE;
	സന്ദേശം_outtrans(HL_TABLE()[id - 1].sg_name);
	name_col = സന്ദേശം_col;
	endcol = 15;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (സന്ദേശം_col + outlen + 1 >= cmdline_width)
    {
	സന്ദേശം_putchar('\n');
	എങ്കിൽ (got_int)
	    തിരികെ TRUE;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (സന്ദേശം_col >= endcol)	// wrap around is like starting a new line
	    newline = FALSE;
    }

    എങ്കിൽ (സന്ദേശം_col >= endcol)	// output at least one space
	endcol = സന്ദേശം_col + 1;
    എങ്കിൽ (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    സന്ദേശം_advance(endcol);

    // Show "xxx" with the attributes.
    എങ്കിൽ (!did_header)
    {
	എങ്കിൽ (endcol == cmdline_width - 1 && endcol <= name_col)
	    സന്ദേശം_putchar(' ');
	സന്ദേശം_puts_attr("xxx", syn_id2attr(id));
	സന്ദേശം_putchar(' ');
    }

    തിരികെ newline;
}

/*
 * Set the attribute numbers for a highlight group.
 * Called after one of the attributes has changed.
 */
    സ്ഥിതം ശൂന്യം
set_hl_attr(
    പൂർണ്ണസംഖ്യ		idx)	    // index in array
{
    attrentry_T	    at_en;
    hl_group_T	    *sgp = HL_TABLE() + idx;

    // The "Normal" group doesn't need an attribute number
    എങ്കിൽ (sgp->sg_name_u != ശൂന്യ && STRCMP(sgp->sg_name_u, "NORMAL") == 0)
	തിരികെ;

#ifdef FEAT_GUI
    // For the GUI mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    എങ്കിൽ (sgp->sg_gui_fg == INVALCOLOR
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
    അല്ലെങ്കിൽ
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
    എങ്കിൽ (sgp->sg_start == ശൂന്യ && sgp->sg_stop == ശൂന്യ)
	sgp->sg_term_attr = sgp->sg_term;
    അല്ലെങ്കിൽ
    {
	at_en.ae_attr = sgp->sg_term;
	at_en.ae_u.term.start = sgp->sg_start;
	at_en.ae_u.term.stop = sgp->sg_stop;
	sgp->sg_term_attr = get_attr_entry(&term_attr_table, &at_en);
    }

    // For the color term mode: If there are other than "normal"
    // highlighting attributes, need to allocate an attr number.
    എങ്കിൽ (sgp->sg_cterm_fg == 0 && sgp->sg_cterm_bg == 0 &&
	sgp->sg_cterm_ul == 0 && sgp->sg_cterm_font == 0
#ifdef FEAT_TERMGUICOLORS
	    && sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
#endif
	    )
	sgp->sg_cterm_attr = sgp->sg_cterm;
    അല്ലെങ്കിൽ
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
	എങ്കിൽ (sgp->sg_cterm & (HL_UNDERLINE | HL_UNDERCURL
			   | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED))
	    at_en.ae_u.cterm.ul_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_sp);
	അല്ലെങ്കിൽ
	    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	എങ്കിൽ (at_en.ae_u.cterm.fg_rgb == INVALCOLOR
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
syn_override(പൂർണ്ണസംഖ്യ id)
{
    എങ്കിൽ (overrides != ശൂന്യ && overrides->arr != ശൂന്യ)
	വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < overrides->len; k++)
	    എങ്കിൽ (overrides->arr[k].from == id)
		തിരികെ overrides->arr[k].to;
    തിരികെ id;
}

/*
 * Lookup a highlight group name by pointer and length.
 * If it is not found, 0 is returned.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
syn_name2id_len(char_u *name, പൂർണ്ണസംഖ്യ len)
{
    char_u	name_u[MAX_SYN_NAME + 1];
    hashitem_T	*hi;

    എങ്കിൽ (len > MAX_SYN_NAME)
	len = MAX_SYN_NAME;
    mch_memmove(name_u, name, len);
    name_u[len] = NUL;
    vim_strup(name_u);
    എങ്കിൽ (!highlight_ht_inited)
	തിരികെ 0;
    hi = hash_find(&highlight_ht, name_u);
    എങ്കിൽ (HASHITEM_EMPTY(hi))
	തിരികെ 0;
    തിരികെ HI2HLNAME(hi)->hn_id;
}

/*
 * Lookup a highlight group name and return its ID.
 * If it is not found, 0 is returned.
 */
    പൂർണ്ണസംഖ്യ
syn_name2id(char_u *name)
{
    // Avoid using stricmp() too much, it's slow on some systems.
    തിരികെ syn_name2id_len(name, (പൂർണ്ണസംഖ്യ)STRLEN(name));
}

/*
 * Lookup a highlight group name and return its attributes.
 * Return zero if not found.
 */
    പൂർണ്ണസംഖ്യ
syn_name2attr(char_u *name)
{
    പൂർണ്ണസംഖ്യ id = syn_name2id(name);

    എങ്കിൽ (id != 0)
	തിരികെ syn_id2attr(id);
    തിരികെ 0;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Return TRUE if highlight group "name" exists.
 */
    പൂർണ്ണസംഖ്യ
highlight_exists(char_u *name)
{
    തിരികെ (syn_name2id(name) > 0);
}

# എങ്കിൽ defined(FEAT_തിരയൽ_EXTRA)
/*
 * Return the name of highlight group "id".
 * When not a valid ID return an empty string.
 */
    char_u *
syn_id2name(പൂർണ്ണസംഖ്യ id)
{
    എങ്കിൽ (id <= 0 || id > highlight_ga.ga_len)
	തിരികെ (char_u *)"";
    തിരികെ HL_TABLE()[id - 1].sg_name;
}
# endif
#endif

/*
 * Like syn_name2id(), but take a pointer + length argument.
 */
    പൂർണ്ണസംഖ്യ
syn_namen2id(char_u *linep, പൂർണ്ണസംഖ്യ len)
{
    തിരികെ syn_name2id_len(linep, len);
}

/*
 * Find highlight group name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    പൂർണ്ണസംഖ്യ
syn_check_group(char_u *pp, പൂർണ്ണസംഖ്യ len)
{
    പൂർണ്ണസംഖ്യ	    id;
    char_u  *name;

    എങ്കിൽ (len > MAX_SYN_NAME)
    {
	emsg(_(e_highlight_group_name_too_long));
	തിരികെ 0;
    }
    id = syn_name2id_len(pp, len);
    എങ്കിൽ (id == 0)			// doesn't exist yet
    {
	name = vim_strnsave(pp, len);
	എങ്കിൽ (name == ശൂന്യ)
	    തിരികെ 0;
	id = syn_add_group(name);
    }
    തിരികെ id;
}

/*
 * Add new highlight group and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
syn_add_group(char_u *name)
{
    char_u	*p;
    char_u	*name_up;

    // Check that the name is valid (ASCII letters, digits, underscores, dots, or hyphens).
    വേണ്ടി (p = name; *p != NUL; ++p)
    {
	എങ്കിൽ (!vim_isprintc(*p))
	{
	    emsg(_(e_unprintable_character_in_group_name));
	    vim_free(name);
	    തിരികെ 0;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!ASCII_ISALNUM(*p) && *p != '_' && *p != '.' && *p != '-')
	{
	    // This is an error, but since there previously was no check only
	    // give a warning.
	    സന്ദേശം_source(HL_ATTR(HLF_W));
	    സന്ദേശം(_("W18: Invalid character in group name"));
	    നിർത്തുക;
	}
    }

    // First call for this growarray: init growing array.
    എങ്കിൽ (highlight_ga.ga_data == ശൂന്യ)
    {
	highlight_ga.ga_itemsize = വലുപ്പം(hl_group_T);
	highlight_ga.ga_growsize = 10;
	hash_init(&highlight_ht);
	highlight_ht_inited = true;
    }

    എങ്കിൽ (highlight_ga.ga_len >= MAX_HL_ID)
    {
	emsg(_(e_too_many_highlight_and_syntax_groups));
	vim_free(name);
	തിരികെ 0;
    }

    // Make room for at least one other syntax_highlight entry.
    എങ്കിൽ (ga_grow(&highlight_ga, 1) == FAIL)
    {
	vim_free(name);
	തിരികെ 0;
    }

    {
	hlname_T    *hn;
	പൂർണ്ണസംഖ്യ	    len = (പൂർണ്ണസംഖ്യ)STRLEN(name);

	hn = alloc(offsetof(hlname_T, hn_താക്കോൽ) + len + 1);
	എങ്കിൽ (hn == ശൂന്യ)
	{
	    vim_free(name);
	    തിരികെ 0;
	}
	vim_strncpy(hn->hn_താക്കോൽ, name, len);
	vim_strup(hn->hn_താക്കോൽ);
	hn->hn_id = highlight_ga.ga_len + 1;	// ID is index plus one
	name_up = hn->hn_താക്കോൽ;
    }

    CLEAR_POINTER(&(HL_TABLE()[highlight_ga.ga_len]));
    HL_TABLE()[highlight_ga.ga_len].sg_name = name;
    HL_TABLE()[highlight_ga.ga_len].sg_name_u = name_up;
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_TABLE()[highlight_ga.ga_len].sg_gui_bg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_fg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_sp = INVALCOLOR;
#endif
    hash_add(&highlight_ht, name_up, "highlight");
    ++highlight_ga.ga_len;

    തിരികെ highlight_ga.ga_len;		    // ID is index plus one
}

/*
 * When, just after calling syn_add_group(), an error is discovered, this
 * function deletes the new name.
 */
    സ്ഥിതം ശൂന്യം
syn_unadd_group(ശൂന്യം)
{
    hashitem_T *hi;

    --highlight_ga.ga_len;
    hi = hash_find(&highlight_ht, HL_TABLE()[highlight_ga.ga_len].sg_name_u);
    എങ്കിൽ (!HASHITEM_EMPTY(hi))
	hash_remove(&highlight_ht, hi, "highlight");
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name);
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name_u - HLNAME_താക്കോൽ_OFF);
}

/*
 * Translate a group ID to highlight attributes.
 * "hl_id" must be valid: > 0, caller must check.
 */
    പൂർണ്ണസംഖ്യ
syn_id2attr(പൂർണ്ണസംഖ്യ hl_id)
{
    പൂർണ്ണസംഖ്യ		attr;
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    പ്രതിജ്ഞാ(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

#ifdef FEAT_GUI
    // Only use GUI attr when the GUI is being used.
    എങ്കിൽ (gui.in_use)
	attr = sgp->sg_gui_attr;
    അല്ലെങ്കിൽ
#endif
	എങ്കിൽ (IS_CTERM)
	    attr = sgp->sg_cterm_attr;
	അല്ലെങ്കിൽ
	    attr = sgp->sg_term_attr;

    തിരികെ attr;
}

#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Get the GUI colors and attributes for a group ID.
 * NOTE: the colors will be INVALCOLOR when not set, the color otherwise.
 */
    പൂർണ്ണസംഖ്യ
syn_id2colors(പൂർണ്ണസംഖ്യ hl_id, guicolor_T *fgp, guicolor_T *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    പ്രതിജ്ഞാ(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

    *fgp = sgp->sg_gui_fg;
    *bgp = sgp->sg_gui_bg;
    തിരികെ sgp->sg_gui;
}
#endif

#എങ്കിൽ (defined(MSWIN) \
	    && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL)) \
	    && defined(FEAT_TERMGUICOLORS)) \
	|| defined(FEAT_TERMINAL)
    ശൂന്യം
syn_id2cterm_bg(പൂർണ്ണസംഖ്യ hl_id, പൂർണ്ണസംഖ്യ *fgp, പൂർണ്ണസംഖ്യ *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    പ്രതിജ്ഞാ(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
    *fgp = sgp->sg_cterm_fg - 1;
    *bgp = sgp->sg_cterm_bg - 1;
}
#endif

/*
 * Translate a group ID to the final group ID (following links).
 */
    പൂർണ്ണസംഖ്യ
syn_get_final_id(പൂർണ്ണസംഖ്യ hl_id)
{
    പൂർണ്ണസംഖ്യ		എണ്ണം;
    hl_group_T	*sgp;

    എങ്കിൽ (hl_id > highlight_ga.ga_len || hl_id < 1)
	തിരികെ 0;			// Can be called from eval!!

    // Follow links until there is no more.
    // Look out for loops!  Break after 100 links.
    വേണ്ടി (എണ്ണം = 100; --എണ്ണം >= 0; )
    {
	പൂർണ്ണസംഖ്യ tmp = hl_id;

	sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
	എങ്കിൽ (sgp->sg_link == 0 || sgp->sg_link > highlight_ga.ga_len)
	    നിർത്തുക;

	// This is to handle highlight groups that are overridden but are in the
	// middle of a link chain.
	hl_id = syn_override(hl_id);
	എങ്കിൽ (tmp != hl_id)
	    തുടരുക;
	hl_id = sgp->sg_link;
    }
    തിരികെ syn_override(hl_id);
}

#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Call this function just after the GUI has started.
 * Also called when 'termguicolors' was set, gui.in_use will be FALSE then.
 * It finds the font and color handles for the highlighting groups.
 */
    ശൂന്യം
highlight_gui_started(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    idx;

    // First get the colors from the "Normal" and "Menu" group, if set
    എങ്കിൽ (USE_24BIT)
	set_normal_colors();

    വേണ്ടി (idx = 0; idx < highlight_ga.ga_len; ++idx)
	gui_do_one_color(idx, FALSE, FALSE);

    highlight_changed();
}

    സ്ഥിതം ശൂന്യം
gui_do_one_color(
    പൂർണ്ണസംഖ്യ		idx,
    പൂർണ്ണസംഖ്യ		do_menu UNUSED,	   // TRUE: might set the menu font
    പൂർണ്ണസംഖ്യ		do_tooltip UNUSED) // TRUE: might set the tooltip font
{
    പൂർണ്ണസംഖ്യ		didit = FALSE;

# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    എങ്കിൽ (gui.in_use)
#  endif
	എങ്കിൽ (HL_TABLE()[idx].sg_font_name != ശൂന്യ)
	{
	    hl_do_font(idx, HL_TABLE()[idx].sg_font_name, FALSE, do_menu,
							    do_tooltip, TRUE);
	    didit = TRUE;
	}
# endif
    എങ്കിൽ (HL_TABLE()[idx].sg_gui_fg_name != ശൂന്യ)
    {
	HL_TABLE()[idx].sg_gui_fg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_fg_name);
	didit = TRUE;
    }
    എങ്കിൽ (HL_TABLE()[idx].sg_gui_bg_name != ശൂന്യ)
    {
	HL_TABLE()[idx].sg_gui_bg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_bg_name);
	didit = TRUE;
    }
    എങ്കിൽ (HL_TABLE()[idx].sg_gui_sp_name != ശൂന്യ)
    {
	HL_TABLE()[idx].sg_gui_sp =
			    color_name2handle(HL_TABLE()[idx].sg_gui_sp_name);
	didit = TRUE;
    }
    എങ്കിൽ (didit)	// need to get a new attr number
	set_hl_attr(idx);
}
#endif

#എങ്കിൽ defined(USER_HIGHLIGHT) && defined(FEAT_STL_OPT)
/*
 * Apply difference between User[1-9] and HLF_S to HLF_SNC, HLF_ST or HLF_STNC.
 */
    സ്ഥിതം ശൂന്യം
combine_stl_hlt(
	പൂർണ്ണസംഖ്യ id,
	പൂർണ്ണസംഖ്യ id_S,
	പൂർണ്ണസംഖ്യ id_alt,
	പൂർണ്ണസംഖ്യ hlcnt,
	പൂർണ്ണസംഖ്യ i,
	പൂർണ്ണസംഖ്യ hlf,
	പൂർണ്ണസംഖ്യ *table)
{
    hl_group_T *hlt = HL_TABLE();

    എങ്കിൽ (id_alt == 0)
    {
	CLEAR_POINTER(&hlt[hlcnt + i]);
	hlt[hlcnt + i].sg_term = highlight_attr[hlf];
	hlt[hlcnt + i].sg_cterm = highlight_attr[hlf];
# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_EVAL)
	hlt[hlcnt + i].sg_gui = highlight_attr[hlf];
# endif
    }
    അല്ലെങ്കിൽ
	mch_memmove(&hlt[hlcnt + i],
		    &hlt[id_alt - 1],
		    വലുപ്പം(hl_group_T));
    hlt[hlcnt + i].sg_link = 0;

    hlt[hlcnt + i].sg_term ^=
	hlt[id - 1].sg_term ^ hlt[id_S - 1].sg_term;
    എങ്കിൽ (hlt[id - 1].sg_start != hlt[id_S - 1].sg_start)
	hlt[hlcnt + i].sg_start = hlt[id - 1].sg_start;
    എങ്കിൽ (hlt[id - 1].sg_stop != hlt[id_S - 1].sg_stop)
	hlt[hlcnt + i].sg_stop = hlt[id - 1].sg_stop;
    hlt[hlcnt + i].sg_cterm ^=
	hlt[id - 1].sg_cterm ^ hlt[id_S - 1].sg_cterm;
    എങ്കിൽ (hlt[id - 1].sg_cterm_fg != hlt[id_S - 1].sg_cterm_fg)
	hlt[hlcnt + i].sg_cterm_fg = hlt[id - 1].sg_cterm_fg;
    എങ്കിൽ (hlt[id - 1].sg_cterm_bg != hlt[id_S - 1].sg_cterm_bg)
	hlt[hlcnt + i].sg_cterm_bg = hlt[id - 1].sg_cterm_bg;
    എങ്കിൽ (hlt[id - 1].sg_cterm_font != hlt[id_S - 1].sg_cterm_font)
	hlt[hlcnt + i].sg_cterm_font = hlt[id - 1].sg_cterm_font;
# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_EVAL)
    hlt[hlcnt + i].sg_gui ^=
	hlt[id - 1].sg_gui ^ hlt[id_S - 1].sg_gui;
# endif
# ifdef FEAT_GUI
    എങ്കിൽ (hlt[id - 1].sg_gui_fg != hlt[id_S - 1].sg_gui_fg)
	hlt[hlcnt + i].sg_gui_fg = hlt[id - 1].sg_gui_fg;
    എങ്കിൽ (hlt[id - 1].sg_gui_bg != hlt[id_S - 1].sg_gui_bg)
	hlt[hlcnt + i].sg_gui_bg = hlt[id - 1].sg_gui_bg;
    എങ്കിൽ (hlt[id - 1].sg_gui_sp != hlt[id_S - 1].sg_gui_sp)
	hlt[hlcnt + i].sg_gui_sp = hlt[id - 1].sg_gui_sp;
    എങ്കിൽ (hlt[id - 1].sg_font != hlt[id_S - 1].sg_font)
	hlt[hlcnt + i].sg_font = hlt[id - 1].sg_font;
#  ifdef FEAT_XFONTSET
    എങ്കിൽ (hlt[id - 1].sg_fontset != hlt[id_S - 1].sg_fontset)
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
    പൂർണ്ണസംഖ്യ
highlight_changed(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		hlf;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*p;
    char_u	*default_hl;
    പൂർണ്ണസംഖ്യ		attr;
    char_u	*end;
    പൂർണ്ണസംഖ്യ		id;
#ifdef USER_HIGHLIGHT
    char_u      userhl[30];  // use 30 to avoid compiler warning
# ifdef FEAT_STL_OPT
    പൂർണ്ണസംഖ്യ		id_S = -1;
    പൂർണ്ണസംഖ്യ		id_SNC = 0;
#  ifdef FEAT_TERMINAL
    പൂർണ്ണസംഖ്യ		id_ST = 0;
    പൂർണ്ണസംഖ്യ		id_STNC = 0;
#  endif
    പൂർണ്ണസംഖ്യ		hlcnt;
# endif
#endif
    win_T	*wp;

    need_highlight_changed = FALSE;

#ifdef FEAT_TERMINAL
    term_update_colors_all();
    term_update_hlfwin_all();
#endif

    // Clear all attributes.
    വേണ്ടി (hlf = 0; hlf < (പൂർണ്ണസംഖ്യ)HLF_എണ്ണം; ++hlf)
    {
	highlight_attr[hlf] = 0;
	highlight_attr_raw[hlf] = 0;
	highlight_ids[hlf] = 0;
    }

    default_hl = get_highlight_default();

    // First set all attributes to their default value.
    // Then use the attributes from the 'highlight' option, unless it already
    // matches the default and would repeat the same work.
    വേണ്ടി (i = 0; i < 2; ++i)
    {
	എങ്കിൽ (i)
	{
	    എങ്കിൽ (default_hl != ശൂന്യ && p_hl != ശൂന്യ
				      && STRCMP(default_hl, p_hl) == 0)
		തുടരുക;
	    p = p_hl;
	}
	അല്ലെങ്കിൽ
	    p = default_hl;
	എങ്കിൽ (p == ശൂന്യ)	    // just in case
	    തുടരുക;

	ആകുമ്പോൾ (*p)
	{
	    വേണ്ടി (hlf = 0; hlf < (പൂർണ്ണസംഖ്യ)HLF_എണ്ണം; ++hlf)
		എങ്കിൽ (hl_flags[hlf] == *p)
		    നിർത്തുക;
	    ++p;
	    എങ്കിൽ (hlf == (പൂർണ്ണസംഖ്യ)HLF_എണ്ണം || *p == NUL)
		തിരികെ FAIL;

	    // Allow several hl_flags to be combined, like "bu" for
	    // bold-underlined.
	    attr = 0;
	    id = 0;
	    വേണ്ടി ( ; *p && *p != ','; ++p)	    // parse up to comma
	    {
		എങ്കിൽ (VIM_ISWHITE(*p))		    // ignore white space
		    തുടരുക;

		എങ്കിൽ (attr > HL_ALL)  // Combination with ':' is not allowed.
		    തിരികെ FAIL;

		// Note: Keep this in sync with expand_set_highlight().
		തിരഞ്ഞെടുപ്പ് (*p)
		{
		    അവസ്ഥ 'b':	attr |= HL_BOLD;
				നിർത്തുക;
		    അവസ്ഥ 'i':	attr |= HL_ITALIC;
				നിർത്തുക;
		    അവസ്ഥ '-':
		    അവസ്ഥ 'n':			    // no highlighting
				നിർത്തുക;
		    അവസ്ഥ 'r':	attr |= HL_INVERSE;
				നിർത്തുക;
		    അവസ്ഥ 's':	attr |= HL_STANDOUT;
				നിർത്തുക;
		    അവസ്ഥ 'u':	attr |= HL_UNDERLINE;
				നിർത്തുക;
		    അവസ്ഥ 'c':	attr |= HL_UNDERCURL;
				നിർത്തുക;
		    അവസ്ഥ '2':	attr |= HL_UNDERDOUBLE;
				നിർത്തുക;
		    അവസ്ഥ 'd':	attr |= HL_UNDERDOTTED;
				നിർത്തുക;
		    അവസ്ഥ '=':	attr |= HL_UNDERDASHED;
				നിർത്തുക;
		    അവസ്ഥ 't':	attr |= HL_STRIKETHROUGH;
				നിർത്തുക;
		    അവസ്ഥ ':':	++p;		    // highlight group name
				എങ്കിൽ (attr || *p == NUL)	 // no combinations
				    തിരികെ FAIL;
				end = vim_strchr(p, ',');
				എങ്കിൽ (end == ശൂന്യ)
				    end = p + STRLEN(p);
				id = syn_check_group(p, (പൂർണ്ണസംഖ്യ)(end - p));
				എങ്കിൽ (id == 0)
				    തിരികെ FAIL;
				attr = syn_id2attr(id);
				p = end - 1;
#എങ്കിൽ defined(FEAT_STL_OPT) && defined(USER_HIGHLIGHT)
				എങ്കിൽ (hlf == (പൂർണ്ണസംഖ്യ)HLF_SNC)
				    id_SNC = syn_get_final_id(id);
# ifdef FEAT_TERMINAL
				അല്ലെങ്കിൽ എങ്കിൽ (hlf == (പൂർണ്ണസംഖ്യ)HLF_ST)
				    id_ST = syn_get_final_id(id);
				അല്ലെങ്കിൽ എങ്കിൽ (hlf == (പൂർണ്ണസംഖ്യ)HLF_STNC)
				    id_STNC = syn_get_final_id(id);
# endif
				അല്ലെങ്കിൽ എങ്കിൽ (hlf == (പൂർണ്ണസംഖ്യ)HLF_S)
				    id_S = syn_get_final_id(id);
#endif
				നിർത്തുക;
		    സ്ഥിരസ്ഥിതി:	തിരികെ FAIL;
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
	അക്ഷരം *errmsg = update_winhighlight(wp, wp->w_p_whl);

	എങ്കിൽ (errmsg != ശൂന്യ)
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
    എങ്കിൽ (ga_grow(&highlight_ga, 28) == FAIL)
	തിരികെ FAIL;
    hlcnt = highlight_ga.ga_len;
    എങ്കിൽ (id_S == -1)
    {
	// Make sure id_S is always valid to simplify code below. Use the last
	// entry.
	CLEAR_POINTER(&HL_TABLE()[hlcnt + 27]);
	HL_TABLE()[hlcnt + 18].sg_term = highlight_attr[HLF_S];
	id_S = hlcnt + 19;
    }
# endif
    വേണ്ടി (i = 0; i < 9; i++)
    {
	sprintf((അക്ഷരം *)userhl, "User%d", i + 1);
	id = syn_name2id(userhl);
	എങ്കിൽ (id == 0)
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
	അല്ലെങ്കിൽ
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

    തിരികെ OK;
}

സ്ഥിതം ശൂന്യം highlight_list(ശൂന്യം);
സ്ഥിതം ശൂന്യം highlight_list_two(പൂർണ്ണസംഖ്യ cnt, പൂർണ്ണസംഖ്യ attr);

// context for :highlight <group> <arg> expansion
സ്ഥിതം പൂർണ്ണസംഖ്യ expand_hi_synid = 0;	    // ID for highlight group being completed
സ്ഥിതം പൂർണ്ണസംഖ്യ expand_hi_equal_col = 0; // column where the '=' is
സ്ഥിതം പൂർണ്ണസംഖ്യ expand_hi_include_orig = 0;	    // whether to fill the existing current value or not
സ്ഥിതം char_u *expand_hi_curvalue = ശൂന്യ;   // the existing current value
#എങ്കിൽ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
സ്ഥിതം dict_iterator_T expand_colornames_iter;	// iterator for looping through v:colornames
#endif

/*
 * Handle command line completion for :highlight command.
 */
    ശൂന്യം
set_context_in_highlight_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		expand_group = TRUE;

    // Default: expand group names
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 0;
    include_link = 2;
    include_default = 1;

    എങ്കിൽ (*arg == NUL)
	തിരികെ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    എങ്കിൽ (*p == NUL)
	തിരികെ;

    // past "default" or group name
    include_default = 0;
    എങ്കിൽ (STRNCMP("default", arg, p - arg) == 0)
    {
	arg = skipwhite(p);
	xp->xp_pattern = arg;
	p = skiptowhite(arg);
    }
    എങ്കിൽ (*p == NUL)
	തിരികെ;

    // past group name
    include_link = 0;
    എങ്കിൽ (arg[1] == 'i' && arg[0] == 'N')
    {
	highlight_list();
	expand_group = FALSE;
    }
    എങ്കിൽ (STRNCMP("link", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	എങ്കിൽ (*p != NUL)		// past first group name
	{
	    xp->xp_pattern = skipwhite(p);
	    p = skiptowhite(xp->xp_pattern);
	    include_none = 1;
	}
	expand_group = FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP("clear", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	expand_group = FALSE;
    }
    എങ്കിൽ (*p != NUL)			// past group name(s)
    {
	എങ്കിൽ (expand_group)
	{
	    // expansion will be done in expand_highlight_group()
	    xp->xp_context = EXPAND_HIGHLIGHT_GROUP;

	    expand_hi_synid = syn_namen2id(arg, (പൂർണ്ണസംഖ്യ)(p - arg));

	    ആകുമ്പോൾ (*p != NUL)
	    {
		arg = skipwhite(p);
		p = skiptowhite(arg);
	    }

	    p = vim_strchr(arg, '=');
	    എങ്കിൽ (p == ശൂന്യ)
	    {
		// Didn't find a key=<value> pattern
		xp->xp_pattern = arg;
		expand_hi_equal_col = -1;
		expand_hi_include_orig = FALSE;
	    }
	    അല്ലെങ്കിൽ
	    {
		// Found key=<value> pattern, record the exact location
		expand_hi_equal_col = (പൂർണ്ണസംഖ്യ)(p - xp->xp_വരി);

		// Only include the original value if the pattern is empty
		എങ്കിൽ (*(p + 1) == NUL)
		    expand_hi_include_orig = TRUE;
		അല്ലെങ്കിൽ
		    expand_hi_include_orig = FALSE;

		// Account for comma-separated values
		എങ്കിൽ (STRNCMP(arg, "term=", 5) == 0 ||
			STRNCMP(arg, "cterm=", 6) == 0 ||
			STRNCMP(arg, "gui=", 4) == 0)
		{
		    char_u *comma = vim_strrchr(p + 1, ',');
		    എങ്കിൽ (comma != ശൂന്യ)
			p = comma;
		}
		xp->xp_pattern = p + 1;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    xp->xp_context = EXPAND_NOTHING;
	}
    }
}

/*
 * List highlighting matches in a nice way.
 */
    സ്ഥിതം ശൂന്യം
highlight_list(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 10; --i >= 0; )
	highlight_list_two(i, HL_ATTR(HLF_D));
    വേണ്ടി (i = 40; --i >= 0; )
	highlight_list_two(99, 0);
}

    സ്ഥിതം ശൂന്യം
highlight_list_two(പൂർണ്ണസംഖ്യ cnt, പൂർണ്ണസംഖ്യ attr)
{
    സന്ദേശം_puts_attr(&("N \bI \b!  \b"[cnt / 11]), attr);
    സന്ദേശം_clr_eos();
    out_flush();
    ui_delay(cnt == 99 ? 40L : (long)cnt * 50L, FALSE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_highlight_name(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ get_highlight_name_ext(xp, idx, TRUE);
}

/*
 * Obtain a highlight group name.
 * When "skip_cleared" is TRUE don't return a cleared entry.
 */
    char_u *
get_highlight_name_ext(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ skip_cleared)
{
    എങ്കിൽ (idx < 0)
	തിരികെ ശൂന്യ;

    // Items are never removed from the table, skip the ones that were
    // cleared.
    എങ്കിൽ (skip_cleared && idx < highlight_ga.ga_len && HL_TABLE()[idx].sg_cleared)
	തിരികെ (char_u *)"";

    എങ്കിൽ (idx == highlight_ga.ga_len && include_none != 0)
	തിരികെ (char_u *)"NONE";
    എങ്കിൽ (idx == highlight_ga.ga_len + include_none && include_default != 0)
	തിരികെ (char_u *)"default";
    എങ്കിൽ (idx == highlight_ga.ga_len + include_none + include_default
							 && include_link != 0)
	തിരികെ (char_u *)"link";
    എങ്കിൽ (idx == highlight_ga.ga_len + include_none + include_default + 1
							 && include_link != 0)
	തിരികെ (char_u *)"clear";
    എങ്കിൽ (idx >= highlight_ga.ga_len)
	തിരികെ ശൂന്യ;
    തിരികെ HL_TABLE()[idx].sg_name;
}

    സ്ഥിതം char_u *
get_highlight_attr_name(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx == 0)
    {
	// Fill with current value first
	എങ്കിൽ (expand_hi_curvalue != ശൂന്യ)
	    തിരികെ expand_hi_curvalue;
	അല്ലെങ്കിൽ
	    തിരികെ (char_u*)"";
    }
    എങ്കിൽ (idx < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(highlight_index_tab) + 1)
    {
	char_u *മൂല്യം = highlight_index_tab[idx-1]->മൂല്യം.string;
	എങ്കിൽ (expand_hi_curvalue != ശൂന്യ && STRCMP(expand_hi_curvalue, മൂല്യം) == 0)
	{
	    // Already returned the current value above, just skip.
	    തിരികെ (char_u*)"";
	}
	തിരികെ മൂല്യം;
    }
    തിരികെ ശൂന്യ;
}

    സ്ഥിതം char_u *
get_highlight_cterm_color(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx == 0)
    {
	// Fill with current value first
	എങ്കിൽ (expand_hi_curvalue != ശൂന്യ)
	    തിരികെ expand_hi_curvalue;
	അല്ലെങ്കിൽ
	    തിരികെ (char_u*)"";
    }
    // See highlight_set_cterm_color()
    അല്ലെങ്കിൽ എങ്കിൽ (idx == 1)
	തിരികെ (char_u*)"fg";
    അല്ലെങ്കിൽ എങ്കിൽ (idx == 2)
	തിരികെ (char_u*)"bg";
    എങ്കിൽ (idx < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(color_name_tab) + 3)
    {
	char_u *മൂല്യം = color_name_tab[idx-3].മൂല്യം.string;
	തിരികെ മൂല്യം;
    }
    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
    സ്ഥിതം char_u *
get_highlight_gui_color(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx == 0)
    {
	// Fill with current value first
	എങ്കിൽ (expand_hi_curvalue != ശൂന്യ)
	    തിരികെ expand_hi_curvalue;
	അല്ലെങ്കിൽ
	    തിരികെ (char_u*)"";
    }
    // See color_name2handle()
    അല്ലെങ്കിൽ എങ്കിൽ (idx == 1)
	തിരികെ (char_u*)"fg";
    അല്ലെങ്കിൽ എങ്കിൽ (idx == 2)
	തിരികെ (char_u*)"bg";
    അല്ലെങ്കിൽ എങ്കിൽ (idx == 3)
	തിരികെ (char_u*)"NONE";

    // Complete from v:colornames. Don't do platform specific names for now.
    typval_T *tv_result;
    char_u *colorname = dict_iterate_next(&expand_colornames_iter, &tv_result);
    എങ്കിൽ (colorname != ശൂന്യ)
    {
	// :hi command doesn't allow space, so don't suggest any malformed items
	എങ്കിൽ (vim_strchr(colorname, ' ') != ശൂന്യ)
	    തിരികെ (char_u*)"";

	എങ്കിൽ (expand_hi_curvalue != ശൂന്യ && STRICMP(expand_hi_curvalue, colorname) == 0)
	{
	    // Already returned the current value above, just skip.
	    തിരികെ (char_u*)"";
	}
    }
    തിരികെ colorname;
}
#endif

    സ്ഥിതം char_u *
get_highlight_group_താക്കോൽ(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    // Note: Keep this in sync with do_highlight.
    സ്ഥിതം അക്ഷരം *(p_hi_group_താക്കോൽ_മൂല്യങ്ങൾ[]) =
    {
	"term=",
	"start=",
	"stop=",
	"cterm=",
	"ctermfg=",
	"ctermbg=",
	"ctermul=",
	"ctermfont=",
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

    എങ്കിൽ (idx < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(p_hi_group_താക്കോൽ_മൂല്യങ്ങൾ))
	തിരികെ (char_u*)p_hi_group_താക്കോൽ_മൂല്യങ്ങൾ[idx];
    തിരികെ ശൂന്യ;
}

/*
 * Command-line expansion for :hi {group-name} <args>...
 */
    പൂർണ്ണസംഖ്യ
expand_highlight_group(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	പൂർണ്ണസംഖ്യ	    *അക്കം_Matches)
{
    എങ്കിൽ (expand_hi_equal_col != -1)
    {
	// List the values. First fill in the current value, then if possible colors
	// or attribute names.
	char_u	    *(*expandfunc)(expand_T *, പൂർണ്ണസംഖ്യ) = ശൂന്യ;
	പൂർണ്ണസംഖ്യ	    type = 0;
	hl_group_T  *sgp = ശൂന്യ;
	പൂർണ്ണസംഖ്യ	    iarg = 0;
	char_u	    *sarg = ശൂന്യ;

	പൂർണ്ണസംഖ്യ	    unsorted_ഇനങ്ങൾ = -1; // don't sort by default

	എങ്കിൽ (expand_hi_synid != 0)
	    sgp = &HL_TABLE()[expand_hi_synid - 1]; // index is ID minus one

	// Note: Keep this in sync with highlight_list_one().
	char_u	    *name_end = xp->xp_വരി + expand_hi_equal_col;
	എങ്കിൽ (name_end - xp->xp_വരി >= 5
		&& STRNCMP(name_end - 5, " term", 5) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    എങ്കിൽ (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_term;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (name_end - xp->xp_വരി >= 6
		&& STRNCMP(name_end - 6, " cterm", 6) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    എങ്കിൽ (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_cterm;
	    }
	}
#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	അല്ലെങ്കിൽ എങ്കിൽ (name_end - xp->xp_വരി >= 4
		&& STRNCMP(name_end - 4, " gui", 4) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    എങ്കിൽ (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_gui;
	    }
	}
#endif
	അല്ലെങ്കിൽ എങ്കിൽ (name_end - xp->xp_വരി >= 8
		&& STRNCMP(name_end - 8, " ctermfg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    എങ്കിൽ (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_fg;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (name_end - xp->xp_വരി >= 8
		&& STRNCMP(name_end - 8, " ctermbg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    എങ്കിൽ (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_bg;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (name_end - xp->xp_വരി >= 8
		&& STRNCMP(name_end - 8, " ctermul", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    എങ്കിൽ (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_ul;
	    }
	}
#എങ്കിൽ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	അല്ലെങ്കിൽ എങ്കിൽ (name_end - xp->xp_വരി >= 6
		&& STRNCMP(name_end - 6, " guifg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    എങ്കിൽ (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_fg_name;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (name_end - xp->xp_വരി >= 6
		&& STRNCMP(name_end - 6, " guibg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    എങ്കിൽ (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_bg_name;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (name_end - xp->xp_വരി >= 6
		&& STRNCMP(name_end - 6, " guisp", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    എങ്കിൽ (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_sp_name;
	    }
	}
#endif

#എങ്കിൽ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	എങ്കിൽ (expandfunc == get_highlight_gui_color)
	{
	    // Top 4 items are special, after that sort all the color names
	    unsorted_ഇനങ്ങൾ = 4;

	    dict_T *colornames_table = get_vim_var_dict(VV_COLORNAMES);
	    typval_T colornames_val;
	    colornames_val.v_type = VAR_DICT;
	    colornames_val.vval.v_dict = colornames_table;
	    dict_iterate_start(&colornames_val, &expand_colornames_iter);
	}
#endif

	char_u	    buf[MAX_ATTR_LEN];

	expand_hi_curvalue = ശൂന്യ;
	എങ്കിൽ (expand_hi_include_orig)
	{
	    എങ്കിൽ (((type == LIST_ATTR || type == LIST_INT) && iarg != 0) ||
		(type == LIST_STRING && sarg != ശൂന്യ))
	    {
		// Retrieve the current value to go first in completion
		expand_hi_curvalue = highlight_arg_to_string(
			type, iarg, sarg, buf);
	    }
	}

	എങ്കിൽ (expandfunc != ശൂന്യ)
	{
	    തിരികെ ExpandGenericExt(
		    pat,
		    xp,
		    rmp,
		    matches,
		    അക്കം_Matches,
		    expandfunc,
		    FALSE,
		    unsorted_ഇനങ്ങൾ);
	}

	തിരികെ FAIL;
    }

    // List all the key names
    തിരികെ ExpandGenericExt(
	    pat,
	    xp,
	    rmp,
	    matches,
	    അക്കം_Matches,
	    get_highlight_group_താക്കോൽ,
	    FALSE,
	    -1);
}

#എങ്കിൽ defined(FEAT_GUI)
/*
 * Free all the highlight group fonts.
 * Used when quitting for systems which need it.
 */
    ശൂന്യം
free_highlight_fonts(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    idx;

    വേണ്ടി (idx = 0; idx < highlight_ga.ga_len; ++idx)
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

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Convert each of the highlight attribute bits (bold, standout, underline,
 * etc.) set in 'hlattr' into a separate boolean item in a Dictionary with
 * the attribute name as the key.
 */
    സ്ഥിതം dict_T *
highlight_get_attr_dict(പൂർണ്ണസംഖ്യ hlattr)
{
    dict_T	*dict;
    പൂർണ്ണസംഖ്യ		i;

    dict = dict_alloc();
    എങ്കിൽ (dict == ശൂന്യ)
	തിരികെ ശൂന്യ;

    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(highlight_index_tab); ++i)
    {
	എങ്കിൽ (hlattr & highlight_index_tab[i]->താക്കോൽ)
	{
	    dict_add_bool(dict, (അക്ഷരം *)highlight_index_tab[i]->മൂല്യം.string,
		    VVAL_TRUE);
	    // don't want "inverse"/"reverse"
	    hlattr &= ~highlight_index_tab[i]->താക്കോൽ;
	}
    }

    തിരികെ dict;
}

/*
 * Return the attributes of the highlight group at index 'hl_idx' as a
 * Dictionary. If 'resolve_link' is TRUE, then resolves the highlight group
 * links recursively.
 */
    സ്ഥിതം dict_T *
highlight_get_info(പൂർണ്ണസംഖ്യ hl_idx, പൂർണ്ണസംഖ്യ resolve_link)
{
    dict_T	*dict;
    hl_group_T	*sgp;
    dict_T	*attr_dict = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		hlgid;

    dict = dict_alloc();
    എങ്കിൽ (dict == ശൂന്യ)
	തിരികെ dict;

    sgp = &HL_TABLE()[hl_idx];
    // highlight group id is 1-based
    hlgid = hl_idx + 1;

    എങ്കിൽ (dict_add_string(dict, "name", sgp->sg_name) == FAIL)
	പോകുക error;
    എങ്കിൽ (dict_add_സംഖ്യ(dict, "id", hlgid) == FAIL)
	പോകുക error;

    എങ്കിൽ (resolve_link)
    {
	// resolve the highlight group link recursively
	ആകുമ്പോൾ (sgp->sg_link)
	{
	    hlgid = sgp->sg_link;
	    sgp = &HL_TABLE()[sgp->sg_link - 1];
	}
    }

    എങ്കിൽ (sgp->sg_term != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_term);
	എങ്കിൽ (attr_dict != ശൂന്യ)
	{
	    എങ്കിൽ (dict_add_dict(dict, "term", attr_dict) == FAIL)
		പോകുക error;
	    attr_dict = ശൂന്യ;
	}
    }
    എങ്കിൽ (sgp->sg_start != ശൂന്യ)
	എങ്കിൽ (dict_add_string(dict, "start", sgp->sg_start) == FAIL)
	    പോകുക error;
    എങ്കിൽ (sgp->sg_stop != ശൂന്യ)
	എങ്കിൽ (dict_add_string(dict, "stop", sgp->sg_stop) == FAIL)
	    പോകുക error;
    എങ്കിൽ (sgp->sg_cterm != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_cterm);
	എങ്കിൽ (attr_dict != ശൂന്യ)
	{
	    എങ്കിൽ (dict_add_dict(dict, "cterm", attr_dict) == FAIL)
		പോകുക error;
	    attr_dict = ശൂന്യ;
	}
    }
    എങ്കിൽ (sgp->sg_cterm_fg != 0)
	എങ്കിൽ (dict_add_string(dict, "ctermfg",
		    highlight_color(hlgid, (char_u *)"fg", 'c')) == FAIL)
	    പോകുക error;
    എങ്കിൽ (sgp->sg_cterm_bg != 0)
	എങ്കിൽ (dict_add_string(dict, "ctermbg",
			highlight_color(hlgid, (char_u *)"bg", 'c')) == FAIL)
	    പോകുക error;
    എങ്കിൽ (sgp->sg_cterm_ul != 0)
	എങ്കിൽ (dict_add_string(dict, "ctermul",
			highlight_color(hlgid, (char_u *)"ul", 'c')) == FAIL)
	    പോകുക error;
    എങ്കിൽ (sgp->sg_cterm_font != 0)
	എങ്കിൽ (dict_add_string(dict, "ctermfont",
			highlight_color(hlgid, (char_u *)"font", 'c')) == FAIL)
	    പോകുക error;
    എങ്കിൽ (sgp->sg_gui != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_gui);
	എങ്കിൽ (attr_dict != ശൂന്യ)
	{
	    എങ്കിൽ (dict_add_dict(dict, "gui", attr_dict) == FAIL)
		പോകുക error;
	    attr_dict = ശൂന്യ;
	}
    }
    എങ്കിൽ (sgp->sg_gui_fg_name != ശൂന്യ)
	എങ്കിൽ (dict_add_string(dict, "guifg",
			highlight_color(hlgid, (char_u *)"fg", 'g')) == FAIL)
	    പോകുക error;
    എങ്കിൽ (sgp->sg_gui_bg_name != ശൂന്യ)
	എങ്കിൽ (dict_add_string(dict, "guibg",
			highlight_color(hlgid, (char_u *)"bg", 'g')) == FAIL)
	    പോകുക error;
    എങ്കിൽ (sgp->sg_gui_sp_name != ശൂന്യ)
	എങ്കിൽ (dict_add_string(dict, "guisp",
			highlight_color(hlgid, (char_u *)"sp", 'g')) == FAIL)
	    പോകുക error;
# ifdef FEAT_GUI
    എങ്കിൽ (sgp->sg_font_name != ശൂന്യ)
	എങ്കിൽ (dict_add_string(dict, "font", sgp->sg_font_name) == FAIL)
	    പോകുക error;
# endif
    എങ്കിൽ (sgp->sg_link)
    {
	char_u	*link;

	link = HL_TABLE()[sgp->sg_link - 1].sg_name;
	എങ്കിൽ (link != ശൂന്യ && dict_add_string(dict, "linksto", link) == FAIL)
	    പോകുക error;

	എങ്കിൽ (sgp->sg_deflink)
	    dict_add_bool(dict, "default", VVAL_TRUE);
    }
    എങ്കിൽ (dict_len(dict) == 2)
	// If only 'name' is present, then the highlight group is cleared.
	dict_add_bool(dict, "cleared", VVAL_TRUE);

    തിരികെ dict;

error:
    dict_unref(attr_dict);
    dict_unref(dict);
    തിരികെ ശൂന്യ;
}

/*
 * "hlget([name])" function
 * Return the attributes of a specific highlight group (if specified) or all
 * the highlight groups.
 */
    ശൂന്യം
f_hlget(typval_T *argvars, typval_T *rettv)
{
    list_T	*list;
    dict_T	*dict;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*hlarg = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		resolve_link = FALSE;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (check_for_opt_string_arg(argvars, 0) == FAIL
	    || (argvars[0].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// highlight group name supplied
	hlarg = tv_get_string_chk(&argvars[0]);
	എങ്കിൽ (hlarg == ശൂന്യ)
	    തിരികെ;

	എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    പൂർണ്ണസംഖ്യ error = FALSE;

	    resolve_link = tv_get_bool_chk(&argvars[1], &error);
	    എങ്കിൽ (error)
		തിരികെ;
	}
    }

    list = rettv->vval.v_list;
    വേണ്ടി (i = 0; i < highlight_ga.ga_len && !got_int; ++i)
    {
	എങ്കിൽ (hlarg == ശൂന്യ || STRICMP(hlarg, HL_TABLE()[i].sg_name) == 0)
	{
	    dict = highlight_get_info(i, resolve_link);
	    എങ്കിൽ (dict != ശൂന്യ && list_append_dict(list, dict) == FAIL)
		dict_unref(dict);
	}
    }
}

/*
 * Returns the string value at 'dict[key]'. Returns NULL, if 'key' is not in
 * 'dict' or the value is not a string type. If the value is not a string type
 * or is NULL, then 'error' is set to TRUE.
 */
    സ്ഥിതം char_u *
hldict_get_string(dict_T *dict, char_u *താക്കോൽ, പൂർണ്ണസംഖ്യ *error)
{
    dictitem_T	*di;

    *error = FALSE;
    di = dict_find(dict, താക്കോൽ, -1);
    എങ്കിൽ (di == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (di->di_tv.v_type != VAR_STRING || di->di_tv.vval.v_string == ശൂന്യ)
    {
	emsg(_(e_string_required));
	*error = TRUE;
	തിരികെ ശൂന്യ;
    }

    തിരികെ di->di_tv.vval.v_string;
}

/*
 * Convert the highlight attribute Dictionary at 'dict[key]' into a string
 * value in 'attr_str' of length 'len'. Returns FALSE if 'dict[key]' is not a
 * Dictionary or is NULL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
hldict_attr_to_str(
	dict_T	*dict,
	char_u	*താക്കോൽ,
	char_u	*attr_str,
	size_t	len)
{
    dictitem_T	*di;
    dict_T	*attrdict;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*p;

    attr_str[0] = NUL;
    di = dict_find(dict, താക്കോൽ, -1);
    എങ്കിൽ (di == ശൂന്യ)
	തിരികെ TRUE;

    എങ്കിൽ (di->di_tv.v_type != VAR_DICT || di->di_tv.vval.v_dict == ശൂന്യ)
    {
	emsg(_(e_dictionary_required));
	തിരികെ FALSE;
    }

    attrdict = di->di_tv.vval.v_dict;

    // If the attribute dict is empty, then return NONE to clear the attributes
    എങ്കിൽ (dict_len(attrdict) == 0)
    {
	vim_strcat(attr_str, (char_u *)"NONE", len);
	തിരികെ TRUE;
    }

    p = attr_str;
    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(highlight_tab); ++i)
    {
	എങ്കിൽ (dict_get_bool(attrdict, (അക്ഷരം *)highlight_tab[i].മൂല്യം.string,
		    VVAL_FALSE) == VVAL_TRUE)
	{
	    എങ്കിൽ (p != attr_str && (size_t)(p - attr_str + 2) < len)
		STRCPY(p, (char_u *)",");
	    എങ്കിൽ (p - attr_str + highlight_tab[i].മൂല്യം.length + 1 < len)
	    {
		STRCPY(p, highlight_tab[i].മൂല്യം.string);
		p += highlight_tab[i].മൂല്യം.length;
	    }
	}
    }

    തിരികെ TRUE;
}

// Temporary buffer used to store the command string produced by hlset().
// IObuff cannot be used for this as the error messages produced by hlset()
// internally use IObuff.
# define	HLSETBUFSZ  512
സ്ഥിതം char_u hlsetBuf[HLSETBUFSZ + 1];

/*
 * Add the highlight attribute "attr" of length "attrlen" and "value" at
 * "dptr", which points into "hlsetBuf".
 * Returns the updated pointer.
 */
    സ്ഥിതം char_u *
add_attr_and_മൂല്യം(char_u *dptr, char_u *attr, പൂർണ്ണസംഖ്യ attrlen, char_u *മൂല്യം)
{
    size_t	vallen;

    // Do nothing if the value is not specified or is empty
    എങ്കിൽ (മൂല്യം == ശൂന്യ || *മൂല്യം == NUL)
	തിരികെ dptr;

    vallen = STRLEN(മൂല്യം);
    // When the value contains a space and the attribute has an "=" (i.e. it
    // is a key=value pair), surround the value with single quotes so that
    // do_highlight() can parse it correctly.
    എങ്കിൽ (vim_strchr(മൂല്യം, ' ') != ശൂന്യ && vim_strchr(attr, '=') != ശൂന്യ)
    {
	എങ്കിൽ (dptr + attrlen + vallen + 3 < hlsetBuf + HLSETBUFSZ)
	{
	    STRCPY(dptr, attr);
	    dptr += attrlen;
	    *dptr++ = '\'';
	    STRCPY(dptr, മൂല്യം);
	    dptr += vallen;
	    *dptr++ = '\'';
	    *dptr = NUL;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (dptr + attrlen + vallen + 1 < hlsetBuf + HLSETBUFSZ)
    {
	STRCPY(dptr, attr);
	dptr += attrlen;
	STRCPY(dptr, മൂല്യം);
	dptr += vallen;
    }

    തിരികെ dptr;
}

/*
 * Add or update a highlight group using 'dict' items. Returns TRUE if
 * successfully updated the highlight group.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
hlg_add_or_update(dict_T *dict)
{
    char_u	*name;
    പൂർണ്ണസംഖ്യ		error;
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
    പൂർണ്ണസംഖ്യ		forceit = FALSE;
    പൂർണ്ണസംഖ്യ		dodefault = FALSE;
    പൂർണ്ണസംഖ്യ		done = FALSE;
    char_u	*p;

    name = hldict_get_string(dict, (char_u *)"name", &error);
    എങ്കിൽ (name == ശൂന്യ || *name == NUL || error)
	തിരികെ FALSE;

    എങ്കിൽ (dict_get_bool(dict, "force", VVAL_FALSE) == VVAL_TRUE)
	forceit = TRUE;

    എങ്കിൽ (dict_get_bool(dict, "default", VVAL_FALSE) == VVAL_TRUE)
	dodefault = TRUE;

    എങ്കിൽ (dict_has_താക്കോൽ(dict, "cleared"))
    {
	varnumber_T	cleared;

	// clear a highlight group
	cleared = dict_get_bool(dict, "cleared", FALSE);
	എങ്കിൽ (cleared == TRUE)
	{
	    vim_snprintf((അക്ഷരം *)hlsetBuf, HLSETBUFSZ, "clear %s", name);
	    do_highlight(hlsetBuf, forceit, FALSE);
	    done = TRUE;
	}
    }

    എങ്കിൽ (dict_has_താക്കോൽ(dict, "linksto"))
    {
	char_u	*linksto;

	// link highlight groups
	linksto = hldict_get_string(dict, (char_u *)"linksto", &error);
	എങ്കിൽ (linksto == ശൂന്യ || *linksto == NUL || error)
	    തിരികെ FALSE;

	vim_snprintf((അക്ഷരം *)hlsetBuf, HLSETBUFSZ, "%slink %s %s",
				dodefault ? "default " : "", name, linksto);
	do_highlight(hlsetBuf, forceit, FALSE);

	done = TRUE;
    }

    // If 'cleared' or 'linksto' are specified, then don't process the other
    // attributes.
    എങ്കിൽ (done)
	തിരികെ TRUE;

    start = hldict_get_string(dict, (char_u *)"start", &error);
    എങ്കിൽ (error)
	തിരികെ FALSE;

    stop = hldict_get_string(dict, (char_u *)"stop", &error);
    എങ്കിൽ (error)
	തിരികെ FALSE;

    എങ്കിൽ (!hldict_attr_to_str(dict, (char_u *)"term", term_attr,
							    വലുപ്പം(term_attr)))
	തിരികെ FALSE;

    എങ്കിൽ (!hldict_attr_to_str(dict, (char_u *)"cterm", cterm_attr,
							   വലുപ്പം(cterm_attr)))
	തിരികെ FALSE;

    ctermfg = hldict_get_string(dict, (char_u *)"ctermfg", &error);
    എങ്കിൽ (error)
	തിരികെ FALSE;

    ctermbg = hldict_get_string(dict, (char_u *)"ctermbg", &error);
    എങ്കിൽ (error)
	തിരികെ FALSE;

    ctermul = hldict_get_string(dict, (char_u *)"ctermul", &error);
    എങ്കിൽ (error)
	തിരികെ FALSE;

    ctermfont = hldict_get_string(dict, (char_u *)"ctermfont", &error);
    എങ്കിൽ (error)
	തിരികെ FALSE;

    എങ്കിൽ (!hldict_attr_to_str(dict, (char_u *)"gui", gui_attr, വലുപ്പം(gui_attr)))
	തിരികെ FALSE;

    guifg = hldict_get_string(dict, (char_u *)"guifg", &error);
    എങ്കിൽ (error)
	തിരികെ FALSE;

    guibg = hldict_get_string(dict, (char_u *)"guibg", &error);
    എങ്കിൽ (error)
	തിരികെ FALSE;

    guisp = hldict_get_string(dict, (char_u *)"guisp", &error);
    എങ്കിൽ (error)
	തിരികെ FALSE;

# ifdef FEAT_GUI
    font = hldict_get_string(dict, (char_u *)"font", &error);
    എങ്കിൽ (error)
	തിരികെ FALSE;
# endif

    // If none of the attributes are specified, then do nothing.
    എങ്കിൽ (term_attr[0] == NUL && start == ശൂന്യ && stop == ശൂന്യ
	    && cterm_attr[0] == NUL && ctermfg == ശൂന്യ && ctermbg == ശൂന്യ
	    && ctermul == ശൂന്യ && ctermfont == ശൂന്യ && gui_attr[0] == NUL
# ifdef FEAT_GUI
	    && font == ശൂന്യ
# endif
	    && guifg == ശൂന്യ && guibg == ശൂന്യ && guisp == ശൂന്യ
	    )
	തിരികെ TRUE;

    hlsetBuf[0] = NUL;
    p = hlsetBuf;
    എങ്കിൽ (dodefault)
	p = add_attr_and_മൂല്യം(p, (char_u *)"default", 7, (char_u *)" ");
    p = add_attr_and_മൂല്യം(p, (char_u *)"", 0, name);
    p = add_attr_and_മൂല്യം(p, (char_u *)" term=", 6, term_attr);
    p = add_attr_and_മൂല്യം(p, (char_u *)" start=", 7, start);
    p = add_attr_and_മൂല്യം(p, (char_u *)" stop=", 6, stop);
    p = add_attr_and_മൂല്യം(p, (char_u *)" cterm=", 7, cterm_attr);
    p = add_attr_and_മൂല്യം(p, (char_u *)" ctermfg=", 9, ctermfg);
    p = add_attr_and_മൂല്യം(p, (char_u *)" ctermbg=", 9, ctermbg);
    p = add_attr_and_മൂല്യം(p, (char_u *)" ctermul=", 9, ctermul);
    p = add_attr_and_മൂല്യം(p, (char_u *)" ctermfont=", 11, ctermfont);
    p = add_attr_and_മൂല്യം(p, (char_u *)" gui=", 5, gui_attr);
# ifdef FEAT_GUI
    p = add_attr_and_മൂല്യം(p, (char_u *)" font=", 6, font);
# endif
    p = add_attr_and_മൂല്യം(p, (char_u *)" guifg=", 7, guifg);
    p = add_attr_and_മൂല്യം(p, (char_u *)" guibg=", 7, guibg);
    (ശൂന്യം)add_attr_and_മൂല്യം(p, (char_u *)" guisp=", 7, guisp);

    do_highlight(hlsetBuf, forceit, FALSE);

    തിരികെ TRUE;
}

/*
 * "hlset([{highlight_attr}])" function
 * Add or modify highlight groups
 */
    ശൂന്യം
f_hlset(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    dict_T	*dict;

    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (check_for_list_arg(argvars, 0) == FAIL)
	തിരികെ;

    FOR_ALL_LIST_ഇനങ്ങൾ(argvars->vval.v_list, li)
    {
	എങ്കിൽ (li->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_dictionary_required));
	    തിരികെ;
	}

	dict = li->li_tv.vval.v_dict;
	എങ്കിൽ (!hlg_add_or_update(dict))
	    തിരികെ;
    }

    rettv->vval.v_സംഖ്യ = 0;
}
#endif

/*
 * If "old" is in the override stack, then update it to "hl_new". Does not free
 * "old".
 */
    ശൂന്യം
update_highlight_overrides(hl_override_T *old, hl_override_T *hl_new, പൂർണ്ണസംഖ്യ newlen)
{
    എങ്കിൽ (old == ശൂന്യ)
	തിരികെ;

    വേണ്ടി (hl_overrides_T *set = overrides; set != ശൂന്യ; set = set->next)
    {
	എങ്കിൽ (set->arr == old)
	{
	    set->arr = hl_new;
	    set->len = newlen;
	}
    }
}

/*
 * If "arr" is in the highlight overrides list, then mark it as invalid.
 */
    ശൂന്യം
remove_highlight_overrides(hl_override_T *arr)
{
    എങ്കിൽ (arr == ശൂന്യ || overrides == ശൂന്യ)
	തിരികെ;

    വേണ്ടി (hl_overrides_T *set = overrides; set != ശൂന്യ; set = set->next)
    {
	എങ്കിൽ (set->arr == arr)
	{
	    set->arr = ശൂന്യ;
	    നിർത്തുക;
	}
    }
}

/*
 * Update highlight_attr[] array. If "update_ids" is true, then update
 * highlight_ids[] array instead.
 */
    സ്ഥിതം ശൂന്യം
set_highlight_attr(hl_override_T *arr, പൂർണ്ണസംഖ്യ len, bool update_ids)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < len; i++)
    {
	hl_override_T	*override = arr + i;
	പൂർണ്ണസംഖ്യ		hlf = -1, attr;

	എങ്കിൽ (override->from <= 0)
	    // Directly map to highlight_attr
	    hlf = -override->from;
	അല്ലെങ്കിൽ
	{
	    വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < HLF_എണ്ണം; k++)
		എങ്കിൽ (override->from == highlight_ids[k])
		{
		    hlf = k;
		    നിർത്തുക;
		}

	    എങ്കിൽ (hlf == -1)
		തുടരുക;
	}

	എങ്കിൽ (update_ids)
	{
	    എങ്കിൽ (override->to <= 0)
	    {
		എങ്കിൽ (hlf == -override->to)
		    highlight_ids[hlf] = -1;
		അല്ലെങ്കിൽ
		    highlight_ids[hlf] = highlight_ids[-override->to];
	    }
	    അല്ലെങ്കിൽ
		highlight_ids[hlf] = override->to;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (override->to <= 0)
		// Directly use highlight_attr[]
		attr = highlight_attr[-override->to];
	    അല്ലെങ്കിൽ
		attr = syn_id2attr(override->to);

	    highlight_attr[hlf] = attr;
	}
    }
}

/*
 * Set the current highlight override to "arr". Returns true if successful
 */
    bool
തള്ളുക_highlight_overrides(hl_override_T *arr, പൂർണ്ണസംഖ്യ len)
{
    // Don't want to do anything if "arr" is already the current override. If
    // "arr" is NULL (but overrides->arr is not), then still push an override,
    // but "->arr" will just be NULL so any previous overrides are cleared.
    എങ്കിൽ (overrides != ശൂന്യ && overrides->arr == arr)
	തിരികെ false;

    hl_overrides_T *set;

    set = ALLOC_ONE(hl_overrides_T);
    എങ്കിൽ (set == ശൂന്യ)
	തിരികെ false;

    set->arr = arr;
    set->len = len;

    // Push to front of list
    set->next = overrides;
    overrides = set;

    // Save current highlight_attr[], so it can be restored later after this
    // override is popped.
    memcpy(set->attr, highlight_attr, വലുപ്പം(highlight_attr));

    // Reset highlight_attr[] to default values, as we don't want changes from
    // other overrides.
    memcpy(highlight_attr, highlight_attr_raw, വലുപ്പം(highlight_attr));

    // Update highlight_attr[] array
    എങ്കിൽ (arr != ശൂന്യ)
	set_highlight_attr(arr, len, false);

    തിരികെ true;
}

/*
 * Pop the current highlight override (if any) from the stack. This should
 * always be paired with a *successful* push.
 */
    ശൂന്യം
എടുക്കുക_highlight_overrides(ശൂന്യം)
{
    hl_overrides_T *set = overrides;

    എങ്കിൽ (overrides == ശൂന്യ)
	തിരികെ;

    overrides = set->next;

    // Set highlight_attr[] to state before override was pushed.
    memcpy(highlight_attr, set->attr, വലുപ്പം(highlight_attr));
    vim_free(set);
}

/*
 * Parse the 'winhighlight' option and return array. Returns NULL on failure or
 * if empty option. If failure, then errmsg is set.
 */
    സ്ഥിതം hl_override_T *
parse_winhighlight(char_u *opt, പൂർണ്ണസംഖ്യ *len, അക്ഷരം **errmsg)
{
    char_u	    *p = opt;
    hl_override_T   *arr;
    പൂർണ്ണസംഖ്യ		    i = 0;
    പൂർണ്ണസംഖ്യ		    അക്കം = 1;
    പൂർണ്ണസംഖ്യ		    n_colons = 0;

    എങ്കിൽ (*p == NUL)
	തിരികെ ശൂന്യ;

    // Get number of overrides first so we can allocate array
    ആകുമ്പോൾ ((p = vim_strchr(p, ',')) != ശൂന്യ)
    {
	p++;
	അക്കം++;
    }
    p = opt;
    // Check if number of ':' matches number of ','
    ആകുമ്പോൾ ((p = vim_strchr(p, ':')) != ശൂന്യ)
    {
	p++;
	n_colons++;
    }

    എങ്കിൽ (അക്കം != n_colons)
    {
	*errmsg = e_invalid_argument;
	തിരികെ ശൂന്യ;
    }

    arr = ALLOC_MULT(hl_override_T, അക്കം);
    എങ്കിൽ (arr == ശൂന്യ)
    {
	*errmsg = e_out_of_memory;
	തിരികെ ശൂന്യ;
    }

    p = opt;

    ആകുമ്പോൾ (true)
    {
	hl_override_T *override = arr + i++;
	char_u	*fromname = p, *toname;
	char_u	*tmp;
	char_u	**names[2] = {&fromname, &toname};
	പൂർണ്ണസംഖ്യ	fromlen, tolen;
	പൂർണ്ണസംഖ്യ	*lens[2] = {&fromlen, &tolen};
	പൂർണ്ണസംഖ്യ	fromid, toid;
	പൂർണ്ണസംഖ്യ	*ids[2] = {&fromid, &toid};

	p = vim_strchr(p, ':');

	എങ്കിൽ (p == ശൂന്യ)
	    പോകുക fail;

	fromlen = p - fromname; // Get hl for "from"
	എങ്കിൽ (fromlen == 0)
	    പോകുക fail;
	p++; // Skip colon ':'
	എങ്കിൽ (*p == NUL)
	    പോകുക fail;

	toname = p;
	tmp = vim_strchr(p, ',');

	// Get hl for "to", must check for no trailing comma in case last
	// element.
	എങ്കിൽ (tmp == ശൂന്യ)
	    tolen = (പൂർണ്ണസംഖ്യ)STRLEN(p);
	അല്ലെങ്കിൽ
	{
	    tolen = tmp - toname;
	    p = ++tmp;
	}
	എങ്കിൽ (tolen == 0)
	    പോകുക fail;

	വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < 2; k++)
	{
	    char_u  *name = *names[k];
	    പൂർണ്ണസംഖ്യ	    nlen = *lens[k];

	    എങ്കിൽ (*name == '!')
	    {
		// If starts with "!", then it maps directly to a 'highlight'
		// occasion.
		പൂർണ്ണസംഖ്യ hlf;

		എങ്കിൽ (nlen != 2)
		    പോകുക fail;

		വേണ്ടി (hlf = 0; hlf < (പൂർണ്ണസംഖ്യ)HLF_എണ്ണം; ++hlf)
		    എങ്കിൽ (hl_flags[hlf] == name[1])
			നിർത്തുക;

		എങ്കിൽ (hlf >= HLF_എണ്ണം)
		    പോകുക fail;

		*ids[k] = -hlf;
	    }
	    അല്ലെങ്കിൽ
	    {
		// Otherwise get the highlight group id.
		എങ്കിൽ (syn_check_group(name, nlen) == 0)
		    പോകുക fail;

		*ids[k] = syn_namen2id(name, nlen);
		എങ്കിൽ (*ids[k] == 0)
		    പോകുക fail;

		// Always map "Normal" group to HLF_WIN
		എങ്കിൽ (ids[k] == &fromid &&
			STRCMP(HL_TABLE()[*ids[k] - 1].sg_name_u, "NORMAL") == 0)
		    *ids[k] = -HLF_WIN;
	    }
	}

	// If fromid == toid, leave it be, this is so that we know HLF_WIN has
	// been set (even to itself e.g. "Normal"), and therefore know to use
	// its attr instead.
	override->from = fromid;
	override->to = toid;

	എങ്കിൽ (tmp == ശൂന്യ)
	    നിർത്തുക;
    }

    *len = അക്കം;
    തിരികെ arr;
fail:
    vim_free(arr);
    *errmsg = e_invalid_argument;
    തിരികെ ശൂന്യ;
}

/*
 * Update w_hl for "wp" using given option value. Returns error
 * message on failure, otherwise NULL.
 */
    അക്ഷരം *
update_winhighlight(win_T *wp, char_u *opt)
{
    അക്ഷരം	    *err = ശൂന്യ;
    hl_override_T   *arr;
    പൂർണ്ണസംഖ്യ		    അക്കം = 1;

    arr = parse_winhighlight(opt, &അക്കം, &err);

    എങ്കിൽ (arr == ശൂന്യ && err != ശൂന്യ)
	തിരികെ err;

    എങ്കിൽ (arr == ശൂന്യ && wp->w_hl == ശൂന്യ)
	തിരികെ ശൂന്യ;

    update_highlight_overrides(wp->w_hl, arr, അക്കം);

    vim_free(wp->w_hl);
    wp->w_hl = arr;
    wp->w_hl_len = അക്കം;

    wp->w_hlfwin_id = hlf_get_id(wp, HLF_WIN);

#ifdef FEAT_TERMINAL
    term_update_hlfwin(wp);
#endif

#ifdef FEAT_TERMINAL
    // May be NULL (such as in after_copy_winopt())
    എങ്കിൽ (wp->w_buffer != ശൂന്യ)
    {
	// Make sure terminal highlighting is updated
	bool ret = തള്ളുക_highlight_overrides(wp->w_hl, wp->w_hl_len);

	എങ്കിൽ (wp->w_buffer->b_term != ശൂന്യ)
	    term_init_default_colors(wp->w_buffer->b_term);
	എങ്കിൽ (ret)
	    എടുക്കുക_highlight_overrides();
    }
#endif

    തിരികെ ശൂന്യ;
}

/*
 * Get id that the hlf attr is using in window "wp", otherwise zero. Note that
 * -1 may be returned if the hlf was set to itself.
 */
    പൂർണ്ണസംഖ്യ
hlf_get_id(win_T *wp, പൂർണ്ണസംഖ്യ hlf)
{
    പൂർണ്ണസംഖ്യ id;
    സ്ഥിതം പൂർണ്ണസംഖ്യ prev[HLF_എണ്ണം];

    എങ്കിൽ (wp->w_hl == ശൂന്യ)
	തിരികെ highlight_ids[hlf];

    memcpy(prev, highlight_ids, വലുപ്പം(highlight_ids));
    set_highlight_attr(wp->w_hl, wp->w_hl_len, true);
    id = highlight_ids[hlf];
    memcpy(highlight_ids, prev, വലുപ്പം(highlight_ids));

    തിരികെ id;
}

/*
 * Update 'winhighlight' using a 'wincolor' style option value. Returns error
 * message on failure otherwise NULL. Does not update the actual 'wincolor'
 * value, but does update the 'winhighlight' value.
 */
    അക്ഷരം *
update_wincolor(win_T *wp, char_u *opt)
{
    char_u  *str = *opt == NUL ? "" : alloc(വലുപ്പം("!(:") + STRLEN(opt));
    അക്ഷരം    *errmsg;

    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ e_out_of_memory;

    എങ്കിൽ (*opt != NUL)
	sprintf((അക്ഷരം *)str, "!(:%s", opt);

    errmsg = update_winhighlight(wp, str);

    എങ്കിൽ (errmsg == ശൂന്യ)
	set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		str, OPT_FREE|OPT_LOCAL, 0);

    എങ്കിൽ (*opt != NUL)
	സ്വതന്ത്രം(str);
    തിരികെ errmsg;
}
