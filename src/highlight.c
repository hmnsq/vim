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

#অন্তর্ভাৱ "vim.h"

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
স্থির keyvalue_T highlight_tab[] = {
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
স্থির keyvalue_T *highlight_index_tab[] = {
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

গণনা {
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
স্থির keyvalue_T color_name_tab[] = {
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
প্রকারনির্দেশ সংরচনা
{
    char_u	*sg_name;	// highlight group name
    char_u	*sg_name_u;	// uppercase of sg_name
    পূর্ণাঙ্ক		sg_cleared;	// "hi clear" was used
// for normal terminals
    পূর্ণাঙ্ক		sg_term;	// "term=" highlighting attributes
    char_u	*sg_start;	// terminal string for start highl
    char_u	*sg_stop;	// terminal string for stop highl
    পূর্ণাঙ্ক		sg_term_attr;	// Screen attr for term mode
// for color terminals
    পূর্ণাঙ্ক		sg_cterm;	// "cterm=" highlighting attr
    পূর্ণাঙ্ক		sg_cterm_bold;	// bold attr was set for light color
    পূর্ণাঙ্ক		sg_cterm_fg;	// terminal fg color number + 1
    পূর্ণাঙ্ক		sg_cterm_bg;	// terminal bg color number + 1
    পূর্ণাঙ্ক		sg_cterm_ul;	// terminal ul color number + 1
    পূর্ণাঙ্ক		sg_cterm_attr;	// Screen attr for color term mode
    পূর্ণাঙ্ক		sg_cterm_font;	// terminal alternative font (0 for normal)
// for when using the GUI
#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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
    পূর্ণাঙ্ক		sg_gui_attr;    // Screen attr for GUI mode
#endif
#যদি defined(FEAT_GUI) || defined(FEAT_EVAL)
// Store the sp color name for the GUI or synIDattr()
    পূর্ণাঙ্ক		sg_gui;		// "gui=" highlighting attributes
    char_u	*sg_gui_fg_name;// GUI foreground color name
    char_u	*sg_gui_bg_name;// GUI background color name
    char_u	*sg_gui_sp_name;// GUI special color name
#endif
    পূর্ণাঙ্ক		sg_link;	// link to this highlight group ID
    পূর্ণাঙ্ক		sg_deflink;	// default link; restored in highlight_clear()
    পূর্ণাঙ্ক		sg_set;		// combination of SG_* flags
#ifdef FEAT_EVAL
    sctx_T	sg_deflink_sctx;  // script where the default link was set
    sctx_T	sg_script_ctx;	// script in which the group was last set
#endif
} hl_group_T;

প্রকারনির্দেশ সংরচনা hl_overrides_S hl_overrides_T;
সংরচনা hl_overrides_S
{
    hl_override_T   *arr;   // May be NULL if "arr" was freed or no highlight
			    // overrides (all values set to default)
    পূর্ণাঙ্ক		    len;
    hl_overrides_T  *next;  // Used to handle recursive calls

    পূর্ণাঙ্ক attr[HLF_গুনতি];    // highlight_attr[] before being updated.
};

স্থির hl_overrides_T *overrides = রিক্ত;

// Synced with highlight_attr, each is the highlight group id for each attr. If
// an element is 0, then there is no attr for it. If an element is -1, then it
// means it was set to itself.
স্থির পূর্ণাঙ্ক highlight_ids[HLF_গুনতি];

// Same as highlight_attr[] but is not modified by highlight group overrides
স্থির পূর্ণাঙ্ক highlight_attr_raw[HLF_গুনতি];

স্থির পূর্ণাঙ্ক hl_flags[HLF_গুনতি] = HL_FLAGS;

// highlight groups for 'highlight' option
স্থির garray_T highlight_ga;
#define HL_TABLE()	((hl_group_T *)((highlight_ga.ga_data)))

// Wrapper struct for hashtable entries: stores the highlight group ID alongside
// the uppercase name used as a hash key.  Uses the same offsetof pattern as
// signgroup_T / HI2SG.
প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক		hn_id;		// highlight group ID (1-based)
    char_u	hn_চাবি[1];	// uppercase name (stretchable array)
} hlname_T;

#define HLNAME_চাবি_OFF	offsetof(hlname_T, hn_চাবি)
#define HI2HLNAME(hi)	((hlname_T *)((hi)->hi_চাবি - HLNAME_চাবি_OFF))

// hashtable for quick highlight group name lookup
স্থির hashtab_T highlight_ht;
স্থির bool	 highlight_ht_inited = false;

/*
 * An attribute number is the index in attr_table plus ATTR_OFF.
 */
#define ATTR_OFF (HL_ALL + 1)

স্থির শূন্য syn_unadd_group(শূন্য);
স্থির শূন্য set_hl_attr(পূর্ণাঙ্ক idx);
স্থির শূন্য highlight_list_one(পূর্ণাঙ্ক id);
স্থির পূর্ণাঙ্ক highlight_list_arg(পূর্ণাঙ্ক id, পূর্ণাঙ্ক didh, পূর্ণাঙ্ক type, পূর্ণাঙ্ক iarg, char_u *sarg, ৱর্ণ *name);
স্থির পূর্ণাঙ্ক syn_add_group(char_u *name);
স্থির পূর্ণাঙ্ক syn_name2id_len(char_u *name, পূর্ণাঙ্ক len);
স্থির পূর্ণাঙ্ক hl_has_settings(পূর্ণাঙ্ক idx, পূর্ণাঙ্ক check_link);
স্থির শূন্য highlight_clear(পূর্ণাঙ্ক idx);

#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
স্থির শূন্য gui_do_one_color(পূর্ণাঙ্ক idx, পূর্ণাঙ্ক do_menu, পূর্ণাঙ্ক do_tooltip);
#endif
#ifdef FEAT_GUI
স্থির পূর্ণাঙ্ক  set_group_colors(char_u *name, guicolor_T *fgp, guicolor_T *bgp, পূর্ণাঙ্ক do_menu, পূর্ণাঙ্ক use_norm, পূর্ণাঙ্ক do_tooltip);
স্থির শূন্য hl_do_font(পূর্ণাঙ্ক idx, char_u *arg, পূর্ণাঙ্ক do_normal, পূর্ণাঙ্ক do_menu, পূর্ণাঙ্ক do_tooltip, পূর্ণাঙ্ক free_font);
#endif

/*
 * The default highlight groups.  These are compiled-in for fast startup and
 * they still work when the runtime files can't be found.
 * When making changes here, also change runtime/colors/default.vim!
 * The #ifdefs are needed to reduce the amount of static data.  Helps to make
 * the 16 bit DOS (museum) version compile.
 */
#যদি defined(FEAT_GUI) || defined(FEAT_EVAL)
# define CENT(a, b) b
#অন্যথা
# define CENT(a, b) a
#endif
স্থির ৱর্ণ *(highlight_init_both[]) = {
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
    রিক্ত
};

// Default colors only used with a light background.
স্থির ৱর্ণ *(highlight_init_light[]) = {
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
    রিক্ত
};

// Default colors only used with a dark background.
স্থির ৱর্ণ *(highlight_init_dark[]) = {
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
    রিক্ত
};

#যদি defined(FEAT_SYN_HL)
/*
 * Returns the number of highlight groups.
 */
    পূর্ণাঙ্ক
highlight_অঙ্ক_groups(শূন্য)
{
    ঘূৰাই highlight_ga.ga_len;
}

/*
 * Returns the name of a highlight group.
 */
    char_u *
highlight_group_name(পূর্ণাঙ্ক id)
{
    ঘূৰাই HL_TABLE()[id].sg_name;
}

/*
 * Returns the ID of the link to a highlight group.
 */
    পূর্ণাঙ্ক
highlight_link_id(পূর্ণাঙ্ক id)
{
    ঘূৰাই HL_TABLE()[id].sg_link;
}
#endif

স্থির শূন্য resolve_fallback_fg_to_rgb(শূন্য);
স্থির শূন্য resolve_fallback_bg_to_rgb(শূন্য);

    শূন্য
init_highlight(
    পূর্ণাঙ্ক		both,	    // include groups where 'bg' doesn't matter
    পূর্ণাঙ্ক		reset)	    // clear group first
{
    পূর্ণাঙ্ক		i;
    ৱর্ণ	**pp;
    স্থির পূর্ণাঙ্ক	had_both = FALSE;
#ifdef FEAT_EVAL
    char_u	*p;

    // Try finding the color scheme file.  Used when a color file was loaded
    // and 'background' or 't_Co' is changed.
    p = get_var_মান((char_u *)"g:colors_name");
    যদি (p != রিক্ত)
    {
	// The value of g:colors_name could be freed when sourcing the script,
	// making "p" invalid, so copy it.
	char_u *copy_p = vim_strsave(p);
	পূর্ণাঙ্ক    r;

	যদি (copy_p != রিক্ত)
	{
	    r = load_colors(copy_p);
	    vim_free(copy_p);
	    যদি (r == OK)
		ঘূৰাই;
	}
    }

#endif

    // Didn't use a color file, use the compiled-in colors.
    যদি (both)
    {
	had_both = TRUE;
	pp = highlight_init_both;
	বাবে (i = 0; pp[i] != রিক্ত; ++i)
	    do_highlight((char_u *)pp[i], reset, TRUE);
    }
    অন্যথা যদি (!had_both)
	// Don't do anything before the call with both == TRUE from main().
	// Not everything has been setup then, and that call will overrule
	// everything anyway.
	ঘূৰাই;

    যদি (*p_bg == 'l')
	pp = highlight_init_light;
    অন্যথা
	pp = highlight_init_dark;
    বাবে (i = 0; pp[i] != রিক্ত; ++i)
	do_highlight((char_u *)pp[i], reset, TRUE);

    // Reverse looks ugly, but grey may not work for less than 8 colors.  Thus
    // let it depend on the number of colors available.
    যদি (t_colors < 8)
	do_highlight((char_u *)"Visual term=reverse cterm=reverse ctermbg=NONE ctermfg=NONE",
		FALSE, TRUE);
    // With 8 colors brown is equal to yellow, need to use black for Search fg
    // to avoid Statement highlighted text disappears.
    // Clear the attributes, needed when changing the t_Co value.
    যদি (t_colors <= 8)
    {
	যদি (*p_bg == 'l')
	    do_highlight((char_u *)"Search ctermfg=black", FALSE, TRUE);
    }

#ifdef FEAT_SYN_HL
    // If syntax highlighting is enabled load the highlighting for it.
    যদি (get_var_মান((char_u *)"g:syntax_on") != রিক্ত)
    {
	স্থির পূর্ণাঙ্ক	recursive = 0;

	যদি (recursive >= 5)
	    emsg(_(e_recursive_loop_loading_syncolor_vim));
	অন্যথা
	{
	    ++recursive;
	    (শূন্য)source_runtime((char_u *)"syntax/syncolor.vim", DIP_ALL);
	    --recursive;
	}
    }
#endif
    resolve_fallback_fg_to_rgb();
    resolve_fallback_bg_to_rgb();
}

#যদি defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
/*
 * Load a default color list. Intended to support legacy color names but allows
 * the user to override the color values. Only loaded once.
 */
    স্থির শূন্য
load_default_colors_lists(শূন্য)
{
    // Lacking a default color list isn't the end of the world but it is likely
    // an inconvenience so users should know when it is missing.
    যদি (source_runtime((char_u *)"colors/lists/default.vim", DIP_ALL) != OK)
	বাৰ্তা("failed to load colors/lists/default.vim");
}
#endif

/*
 * Load color file "name".
 * Return OK for success, FAIL for failure.
 */
    পূর্ণাঙ্ক
load_colors(char_u *name)
{
    char_u	*buf;
    পূর্ণাঙ্ক		retval = FAIL;
    স্থির পূর্ণাঙ্ক	recursive = FALSE;

    // When being called recursively, this is probably because setting
    // 'background' caused the highlighting to be reloaded.  This means it is
    // working, thus we should return OK.
    যদি (recursive)
	ঘূৰাই OK;

    recursive = TRUE;
    buf = alloc(STRLEN(name) + 12);
    যদি (buf != রিক্ত)
    {
#যদি defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	load_default_colors_lists();
#endif
	apply_autocmds(EVENT_COLORSCHEMEPRE, name,
					       curbuf->b_fname, FALSE, curbuf);
	sprintf((ৱর্ণ *)buf, "colors/%s.vim", name);
	retval = source_runtime(buf, DIP_START + DIP_OPT);
	vim_free(buf);
	যদি (retval == OK)
	    apply_autocmds(EVENT_COLORSCHEME, name, curbuf->b_fname,
								FALSE, curbuf);
    }
    recursive = FALSE;

    ঘূৰাই retval;
}

স্থির পূর্ণাঙ্ক color_numbers_16[28] = {0, 1, 2, 3,
				 4, 5, 6, 6,
				 7, 7, 7, 7,
				 8, 8,
				 9, 9, 10, 10,
				 11, 11, 12, 12, 13,
				 13, 14, 14, 15, -1};
// for xterm with 88 colors...
স্থির পূর্ণাঙ্ক color_numbers_88[28] = {0, 4, 2, 6,
				 1, 5, 32, 72,
				 84, 84, 7, 7,
				 82, 82,
				 12, 43, 10, 61,
				 14, 63, 9, 74, 13,
				 75, 11, 78, 15, -1};
// for xterm with 256 colors...
স্থির পূর্ণাঙ্ক color_numbers_256[28] = {0, 4, 2, 6,
				 1, 5, 130, 3,
				 248, 248, 7, 7,
				 242, 242,
				 12, 81, 10, 121,
				 14, 159, 9, 224, 13,
				 225, 11, 229, 15, -1};
// for terminals with less than 16 colors...
স্থির পূর্ণাঙ্ক color_numbers_8[28] = {0, 4, 2, 6,
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
    স্থির পূর্ণাঙ্ক
lookup_color(পূর্ণাঙ্ক idx, পূর্ণাঙ্ক foreground, পূর্ণাঙ্ক *boldp)
{
    পূর্ণাঙ্ক		color = color_numbers_16[idx];
    char_u	*p;

    // Use the _16 table to check if it's a valid color name.
    যদি (color < 0)
	ঘূৰাই -1;

    যদি (t_colors == 8)
    {
	// t_Co is 8: use the 8 colors table
#যদি defined(__QNXNTO__)
	// On qnx, the 8 & 16 color arrays are the same
	যদি (STRNCMP(T_NAME, "qansi", 5) == 0)
	    color = color_numbers_16[idx];
	অন্যথা
#endif
	    color = color_numbers_8[idx];
	যদি (foreground)
	{
	    // set/reset bold attribute to get light foreground
	    // colors (on some terminals, e.g. "linux")
	    যদি (color & 8)
		*boldp = TRUE;
	    অন্যথা
		*boldp = FALSE;
	}
	color &= 7;	// truncate to 8 colors
    }
    অন্যথা যদি (t_colors == 16 || t_colors == 88
					   || t_colors >= 256)
    {
	// Guess: if the termcap entry ends in 'm', it is
	// probably an xterm-like terminal.  Use the changed
	// order for colors.
	যদি (*T_CAF != NUL)
	    p = T_CAF;
	অন্যথা
	    p = T_CSF;
	যদি (*p != NUL && (t_colors > 256
			      || *(p + STRLEN(p) - 1) == 'm'))
	{
	    যদি (t_colors == 88)
		color = color_numbers_88[idx];
	    অন্যথা যদি (t_colors >= 256)
		color = color_numbers_256[idx];
	    অন্যথা
		color = color_numbers_8[idx];
	}
#ifdef FEAT_TERMRESPONSE
	যদি (t_colors >= 256 && color == 15 && is_mac_terminal)
	    // Terminal.app has a bug: 15 is light grey. Use white
	    // from the color cube instead.
	    color = 231;
#endif
    }
    ঘূৰাই color;
}

/*
 * Link highlight group 'from_hg' to 'to_hg'.
 * 'dodefault' is set to TRUE for ":highlight default link".
 * 'forceit' is set to TRUE for ":highlight! link"
 * 'init' is set to TRUE when initializing all the highlight groups.
 */
    স্থির শূন্য
highlight_group_link(
	char_u	*from_hg,
	পূর্ণাঙ্ক	from_len,
	char_u	*to_hg,
	পূর্ণাঙ্ক	to_len,
	পূর্ণাঙ্ক	dodefault,
	পূর্ণাঙ্ক	forceit,
	পূর্ণাঙ্ক	init)
{
    পূর্ণাঙ্ক		from_id;
    পূর্ণাঙ্ক		to_id;
    hl_group_T	*hlgroup = রিক্ত;

    from_id = syn_check_group(from_hg, from_len);
    যদি (STRNCMP(to_hg, "NONE", 4) == 0)
	to_id = 0;
    অন্যথা
	to_id = syn_check_group(to_hg, to_len);

    যদি (from_id > 0)
    {
	hlgroup = &HL_TABLE()[from_id - 1];
	যদি (dodefault && (forceit || hlgroup->sg_deflink == 0))
	{
	    hlgroup->sg_deflink = to_id;
#ifdef FEAT_EVAL
	    hlgroup->sg_deflink_sctx = current_sctx;
	    hlgroup->sg_deflink_sctx.sc_lnum += SOURCING_LNUM;
#endif
	}
    }

    যদি (from_id > 0 && (!init || hlgroup->sg_set == 0))
    {
	// Don't allow a link when there already is some highlighting
	// for the group, unless '!' is used
	যদি (to_id > 0 && !forceit && !init
		&& hl_has_settings(from_id - 1, dodefault))
	{
	    যদি (SOURCING_NAME == রিক্ত && !dodefault)
		emsg(_(e_group_has_settings_highlight_link_ignored));
	}
	অন্যথা যদি (hlgroup->sg_link != to_id
#ifdef FEAT_EVAL
		|| hlgroup->sg_script_ctx.sc_sid != current_sctx.sc_sid
#endif
		|| hlgroup->sg_cleared)
	{
	    যদি (!init)
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
    স্থির শূন্য
highlight_reset_all(শূন্য)
{
    পূর্ণাঙ্ক		idx;

#ifdef FEAT_GUI
    // First, we do not destroy the old values, but allocate the new
    // ones and update the display. THEN we destroy the old values.
    // If we destroy the old values first, then the old values
    // (such as GuiFont's or GuiFontset's) will still be displayed but
    // invalid because they were free'd.
    যদি (gui.in_use)
    {
# ifdef FEAT_BEVAL_TIP
	gui_init_tooltip_font();
# endif
# যদি defined(FEAT_MENU) && defined(FEAT_GUI_MOTIF)
	gui_init_menu_font();
# endif
    }
# যদি defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_X11)
    gui_mch_def_colors();
# endif
# ifdef FEAT_GUI_MSWIN
    যদি (gui.in_use)
	gui_mch_set_titlebar_colors();
# endif
# ifdef FEAT_GUI_X11
#  ifdef FEAT_MENU

    // This only needs to be done when there is no Menu highlight
    // group defined by default, which IS currently the case.
    gui_mch_new_menu_colors();
#  endif
    যদি (gui.in_use)
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
    বাবে (idx = 0; idx < highlight_ga.ga_len; ++idx)
	highlight_clear(idx);
    init_highlight(TRUE, TRUE);
#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    যদি (USE_24BIT)
	highlight_gui_started();
    অন্যথা
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
    স্থির পূর্ণাঙ্ক
highlight_set_termgui_attr(পূর্ণাঙ্ক idx, char_u *চাবি, char_u *arg, পূর্ণাঙ্ক init)
{
    পূর্ণাঙ্ক		attr;
    size_t	off;
    keyvalue_T	target;
    keyvalue_T	*entry;

    attr = 0;
    off = 0;
    target.চাবি = 0;
    target.মান.length = 0;	// not used, see cmp_keyvalue_value_ni()
    যেতিয়ালৈ (arg[off] != NUL)
    {
	target.মান.string = arg + off;
	entry = (keyvalue_T *)bsearch(&target, &highlight_tab,
		ARRAY_LENGTH(highlight_tab), পরিমাণ(highlight_tab[0]),
		cmp_keyvalue_মান_ni);
	যদি (entry == রিক্ত)
	{
	    semsg(_(e_illegal_মান_str), arg);
	    ঘূৰাই FALSE;
	}

	attr |= entry->চাবি;
	off += entry->মান.length;
	যদি (arg[off] == ',')		// another one follows
	    ++off;
    }
    যদি (*চাবি == 'T')
    {
	যদি (!init || !(HL_TABLE()[idx].sg_set & SG_TERM))
	{
	    যদি (!init)
		HL_TABLE()[idx].sg_set |= SG_TERM;
	    HL_TABLE()[idx].sg_term = attr;
	}
    }
    অন্যথা যদি (*চাবি == 'C')
    {
	যদি (!init || !(HL_TABLE()[idx].sg_set & SG_CTERM))
	{
	    যদি (!init)
		HL_TABLE()[idx].sg_set |= SG_CTERM;
	    HL_TABLE()[idx].sg_cterm = attr;
	    HL_TABLE()[idx].sg_cterm_bold = FALSE;
	}
    }
#যদি defined(FEAT_GUI) || defined(FEAT_EVAL)
    অন্যথা
    {
	যদি (!init || !(HL_TABLE()[idx].sg_set & SG_GUI))
	{
	    যদি (!init)
		HL_TABLE()[idx].sg_set |= SG_GUI;
	    HL_TABLE()[idx].sg_gui = attr;
	}
    }
#endif

    ঘূৰাই TRUE;
}

#ifdef FEAT_GUI
/*
 * Set the font for the highlight group at 'idx'.
 * 'arg' is the font name.
 * Returns TRUE if the font is changed.
 */
    স্থির পূর্ণাঙ্ক
highlight_set_font(
	পূর্ণাঙ্ক	idx,
	char_u	*arg,
	পূর্ণাঙ্ক	is_normal_group,
	পূর্ণাঙ্ক	is_menu_group,
	পূর্ণাঙ্ক	is_tooltip_group)
{
    পূর্ণাঙ্ক		did_change = FALSE;

    // in non-GUI fonts are simply ignored
    যদি (HL_TABLE()[idx].sg_font_name != রিক্ত
	    && STRCMP(HL_TABLE()[idx].sg_font_name, arg) == 0)
    {
	// Font name didn't change, ignore.
    }
    অন্যথা যদি (!gui.shell_created)
    {
	// GUI not started yet, always accept the name.
	vim_free(HL_TABLE()[idx].sg_font_name);
	HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	did_change = TRUE;
    }
    অন্যথা
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
	যদি (HL_TABLE()[idx].sg_fontset != NOFONTSET)
	{
	    // New fontset was accepted. Free the old one, if there
	    // was one.
	    gui_mch_free_fontset(temp_sg_fontset);
	    did_change = TRUE;
	}
	অন্যথা
	    HL_TABLE()[idx].sg_fontset = temp_sg_fontset;
# endif
	যদি (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    // New font was accepted. Free the old one, if there was
	    // one.
	    gui_mch_free_font(temp_sg_font);
	    did_change = TRUE;
	}
	অন্যথা
	    HL_TABLE()[idx].sg_font = temp_sg_font;
	// Update sg_font_name once here, after both font and fontset results
	// are known.
	যদি (did_change)
	{
	    vim_free(HL_TABLE()[idx].sg_font_name);
	    HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	}
    }

    ঘূৰাই did_change;
}
#endif

/*
 * Set the cterm foreground color for the Normal highlight group to "color" and
 * the bold attribute to "bold".
 */
    স্থির শূন্য
hl_set_ctermfg_normal_group(পূর্ণাঙ্ক color, পূর্ণাঙ্ক bold)
{
    cterm_normal_fg_color = color + 1;
    cterm_normal_fg_bold = bold;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    যদি (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	যদি (termcap_active && color >= 0)
	    term_fg_color(color);
    }
}

/*
 * Set the cterm foreground color for the highlight group at 'idx' to 'color'.
 */
    স্থির শূন্য
highlight_set_ctermfg(পূর্ণাঙ্ক idx, পূর্ণাঙ্ক color, পূর্ণাঙ্ক is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_fg = color + 1;
    যদি (is_normal_group)
	hl_set_ctermfg_normal_group(color,
					(HL_TABLE()[idx].sg_cterm & HL_BOLD));
}

/*
 * Set the cterm background color for the Normal highlight group to "color".
 */
    স্থির শূন্য
hl_set_ctermbg_normal_group(পূর্ণাঙ্ক color)
{
    cterm_normal_bg_color = color + 1;
#ifdef FEAT_GUI
    // Don't mess with 'background' if the GUI is used.
    যদি (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	যদি (color >= 0)
	{
	    পূর্ণাঙ্ক dark = -1;

	    যদি (termcap_active)
		term_bg_color(color);
	    যদি (t_colors < 16)
		dark = (color == 0 || color == 4);
	    // Limit the heuristic to the standard 16 colors
	    অন্যথা যদি (color < 16)
		dark = (color < 7 || color == 8);
	    // Set the 'background' option if the value is
	    // wrong.
	    যদি (dark != -1
		    && dark != (*p_bg == 'd')
		    && !option_was_set((char_u *)"bg"))
	    {
		set_option_মান_give_err((char_u *)"bg",
			0L, (char_u *)(dark ? "dark" : "light"), 0);
		reset_option_was_set((char_u *)"bg");
	    }
	}
    }
}

/*
 * Set the cterm background color for the highlight group at 'idx' to 'color'.
 */
    স্থির শূন্য
highlight_set_ctermbg(পূর্ণাঙ্ক idx, পূর্ণাঙ্ক color, পূর্ণাঙ্ক is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_bg = color + 1;
    যদি (is_normal_group)
	hl_set_ctermbg_normal_group(color);
}

/*
 * Set the cterm underline color for the Normal highlight group to "color".
 */
    স্থির শূন্য
hl_set_ctermul_normal_group(পূর্ণাঙ্ক color)
{
    cterm_normal_ul_color = color + 1;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    যদি (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	যদি (termcap_active && color >= 0)
	    term_ul_color(color);
    }
}

/*
 * Set the cterm underline color for the highlight group at 'idx' to 'color'.
 */
    স্থির শূন্য
highlight_set_ctermul(পূর্ণাঙ্ক idx, পূর্ণাঙ্ক color, পূর্ণাঙ্ক is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_ul = color + 1;
    যদি (is_normal_group)
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
    স্থির পূর্ণাঙ্ক
highlight_set_cterm_font(
	পূর্ণাঙ্ক	idx,
	char_u	*arg,
	পূর্ণাঙ্ক	init)
{
    পূর্ণাঙ্ক		font;

    যদি (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	ঘূৰাই FALSE;

    যদি (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    যদি (VIM_ISDIGIT(*arg))
	font = atoi((ৱর্ণ *)arg);
    অন্যথা যদি (STRICMP(arg, "NONE") == 0)
	font = -1;
    অন্যথা
	ঘূৰাই FALSE;

    HL_TABLE()[idx].sg_cterm_font = font + 1;
    ঘূৰাই TRUE;
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
    স্থির পূর্ণাঙ্ক
highlight_set_cterm_color(
	পূর্ণাঙ্ক	idx,
	char_u	*চাবি,
	char_u	*চাবি_start,
	char_u	*arg,
	পূর্ণাঙ্ক	is_normal_group,
	পূর্ণাঙ্ক	init)
{
    পূর্ণাঙ্ক		color;

    যদি (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	ঘূৰাই FALSE;

    যদি (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    // When setting the foreground color, and previously the "bold"
    // flag was set for a light color, reset it now
    যদি (চাবি[5] == 'F' && HL_TABLE()[idx].sg_cterm_bold)
    {
	HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
	HL_TABLE()[idx].sg_cterm_bold = FALSE;
    }

    যদি (VIM_ISDIGIT(*arg))
	color = atoi((ৱর্ণ *)arg);
    অন্যথা যদি (STRICMP(arg, "fg") == 0)
    {
	যদি (cterm_normal_fg_color)
	    color = cterm_normal_fg_color - 1;
	অন্যথা
	{
	    emsg(_(e_fg_color_unknown));
	    ঘূৰাই FALSE;
	}
    }
    অন্যথা যদি (STRICMP(arg, "bg") == 0)
    {
	যদি (cterm_normal_bg_color > 0)
	    color = cterm_normal_bg_color - 1;
	অন্যথা
	{
	    emsg(_(e_bg_color_unknown));
	    ঘূৰাই FALSE;
	}
    }
    অন্যথা যদি (STRICMP(arg, "ul") == 0)
    {
	যদি (cterm_normal_ul_color > 0)
	    color = cterm_normal_ul_color - 1;
	অন্যথা
	{
	    emsg(_(e_ul_color_unknown));
	    ঘূৰাই FALSE;
	}
    }
    অন্যথা
    {
	পূর্ণাঙ্ক bold = MAYBE;
	keyvalue_T target;
	keyvalue_T *entry;

	target.চাবি = 0;
	target.মান.string = arg;
	target.মান.length = 0;	// not used, see cmp_keyvalue_value_i()
	entry = (keyvalue_T *)bsearch(&target, &color_name_tab,
		ARRAY_LENGTH(color_name_tab), পরিমাণ(color_name_tab[0]),
		cmp_keyvalue_মান_i);
	যদি (entry == রিক্ত)
	{
	    semsg(_(e_color_name_or_সংখ্যা_not_recognized_str), চাবি_start);
	    ঘূৰাই FALSE;
	}

	color = lookup_color(entry->চাবি, চাবি[5] == 'F', &bold);

	// set/reset bold attribute to get light foreground
	// colors (on some terminals, e.g. "linux")
	যদি (bold == TRUE)
	{
	    HL_TABLE()[idx].sg_cterm |= HL_BOLD;
	    HL_TABLE()[idx].sg_cterm_bold = TRUE;
	}
	অন্যথা যদি (bold == FALSE)
	    HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
    }

    // Add one to the argument, to avoid zero.  Zero is used for
    // "NONE", then "color" is -1.
    যদি (চাবি[5] == 'F')
	highlight_set_ctermfg(idx, color, is_normal_group);
    অন্যথা যদি (চাবি[5] == 'B')
	highlight_set_ctermbg(idx, color, is_normal_group);
    অন্যথা // ctermul
	highlight_set_ctermul(idx, color, is_normal_group);

    ঘূৰাই TRUE;
}

#যদি defined(FEAT_GUI) || defined(FEAT_EVAL)
/*
 * Set the GUI foreground color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    স্থির পূর্ণাঙ্ক
highlight_set_guifg(
	পূর্ণাঙ্ক	idx,
	char_u	*arg,
	পূর্ণাঙ্ক	is_menu_group UNUSED,
	পূর্ণাঙ্ক	is_scrollbar_group UNUSED,
	পূর্ণাঙ্ক	is_tooltip_group UNUSED,
	পূর্ণাঙ্ক	is_titlebar_group UNUSED,
	পূর্ণাঙ্ক	is_titlebarnc_group UNUSED,
	পূর্ণাঙ্ক	*do_colors UNUSED,
	পূর্ণাঙ্ক	init)
{
# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    long	i;
# endif
    char_u	**namep;
    পূর্ণাঙ্ক		did_change = FALSE;

    যদি (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	ঘূৰাই FALSE;

    namep = &HL_TABLE()[idx].sg_gui_fg_name;
    যদি (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guifg colors are only used when recognized
    i = color_name2handle(arg);
    যদি (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_fg = i;
# endif
	যদি (*namep == রিক্ত || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    যদি (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    অন্যথা
		*namep = রিক্ত;
	    did_change = TRUE;
	}
# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	যদি (is_titlebar_group && gui.title_fg_pixel != i)
	{
	    gui.title_fg_pixel = i;
	    *do_colors = TRUE;
	}
	যদি (is_titlebarnc_group && gui.titlenc_fg_pixel != i)
	{
	    gui.titlenc_fg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	যদি (is_menu_group && gui.menu_fg_pixel != i)
	{
	    gui.menu_fg_pixel = i;
	    *do_colors = TRUE;
	}
	যদি (is_scrollbar_group && gui.scroll_fg_pixel != i)
	{
	    gui.scroll_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	যদি (is_tooltip_group && gui.tooltip_fg_pixel != i)
	{
	    gui.tooltip_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    ঘূৰাই did_change;
}

/*
 * Set the GUI background color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    স্থির পূর্ণাঙ্ক
highlight_set_guibg(
	পূর্ণাঙ্ক	idx,
	char_u	*arg,
	পূর্ণাঙ্ক	is_menu_group UNUSED,
	পূর্ণাঙ্ক	is_scrollbar_group UNUSED,
	পূর্ণাঙ্ক	is_tooltip_group UNUSED,
	পূর্ণাঙ্ক	is_titlebar_group UNUSED,
	পূর্ণাঙ্ক	is_titlebarnc_group UNUSED,
	পূর্ণাঙ্ক	*do_colors UNUSED,
	পূর্ণাঙ্ক	init)
{
# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    পূর্ণাঙ্ক		i;
# endif
    char_u	**namep;
    পূর্ণাঙ্ক		did_change = FALSE;

    যদি (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	ঘূৰাই FALSE;

    namep = &HL_TABLE()[idx].sg_gui_bg_name;
    যদি (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guibg colors are only used when recognized
    i = color_name2handle(arg);
    যদি (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_bg = i;
# endif
	যদি (*namep == রিক্ত || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    যদি (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    অন্যথা
		*namep = রিক্ত;
	    did_change = TRUE;
	}
# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	যদি (is_titlebar_group && gui.title_bg_pixel != i)
	{
	    gui.title_bg_pixel = i;
	    *do_colors = TRUE;
	}
	যদি (is_titlebarnc_group && gui.titlenc_bg_pixel != i)
	{
	    gui.titlenc_bg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	যদি (is_menu_group && gui.menu_bg_pixel != i)
	{
	    gui.menu_bg_pixel = i;
	    *do_colors = TRUE;
	}
	যদি (is_scrollbar_group && gui.scroll_bg_pixel != i)
	{
	    gui.scroll_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	যদি (is_tooltip_group && gui.tooltip_bg_pixel != i)
	{
	    gui.tooltip_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    ঘূৰাই did_change;
}

/*
 * Set the GUI undercurl/strikethrough color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    স্থির পূর্ণাঙ্ক
highlight_set_guisp(পূর্ণাঙ্ক idx, char_u *arg, পূর্ণাঙ্ক init)
{
# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    পূর্ণাঙ্ক		i;
# endif
    পূর্ণাঙ্ক		did_change = FALSE;
    char_u	**namep;

    যদি (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	ঘূৰাই FALSE;

    namep = &HL_TABLE()[idx].sg_gui_sp_name;
    যদি (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guisp colors are only used when recognized
    i = color_name2handle(arg);
    যদি (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_sp = i;
# endif
	যদি (*namep == রিক্ত || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    যদি (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    অন্যথা
		*namep = রিক্ত;
	    did_change = TRUE;
	}
# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    }
# endif

    ঘূৰাই did_change;
}
#endif

/*
 * Set the start/stop terminal codes for a highlight group.
 * Returns TRUE if the terminal code is set.
 */
    স্থির পূর্ণাঙ্ক
highlight_set_startstop_termcode(পূর্ণাঙ্ক idx, char_u *চাবি, char_u *arg, পূর্ণাঙ্ক init)
{
    পূর্ণাঙ্ক		off;
    char_u	buf[100];
    পূর্ণাঙ্ক		len;
    char_u	*tname;
    char_u	*p;

    যদি (!init)
	HL_TABLE()[idx].sg_set |= SG_TERM;

    // The "start" and "stop"  arguments can be a literal escape
    // sequence, or a comma separated list of terminal codes.
    যদি (STRNCMP(arg, "t_", 2) == 0)
    {
	off = 0;
	buf[0] = 0;
	যেতিয়ালৈ (arg[off] != NUL)
	{
	    // Isolate one termcap name
	    বাবে (len = 0; arg[off + len] &&
		    arg[off + len] != ','; ++len)
		;
	    tname = vim_strnsave(arg + off, len);
	    যদি (tname == রিক্ত)		// out of memory
		ঘূৰাই FALSE;
	    // lookup the escape sequence for the item
	    p = get_term_code(tname);
	    vim_free(tname);
	    যদি (p == রিক্ত)	    // ignore non-existing things
		p = (char_u *)"";

	    // Append it to the already found stuff
	    যদি ((পূর্ণাঙ্ক)(STRLEN(buf) + STRLEN(p)) >= 99)
	    {
		semsg(_(e_terminal_code_too_long_str), arg);
		ঘূৰাই FALSE;
	    }
	    STRCAT(buf, p);

	    // Advance to the next item
	    off += len;
	    যদি (arg[off] == ',')	    // another one follows
		++off;
	}
    }
    অন্যথা
    {
	// Copy characters from arg[] to buf[], translating <> codes.
	বাবে (p = arg, off = 0; off < 100 - 6 && *p; )
	{
	    len = trans_special(&p, buf + off, FSK_SIMPLIFY, FALSE, রিক্ত);
	    যদি (len > 0)	    // recognized special char
		off += len;
	    অন্যথা		    // copy as normal char
		buf[off++] = *p++;
	}
	buf[off] = NUL;
    }

    যদি (STRCMP(buf, "NONE") == 0)	// resetting the value
	p = রিক্ত;
    অন্যথা
	p = vim_strsave(buf);
    যদি (চাবি[2] == 'A')
    {
	vim_free(HL_TABLE()[idx].sg_start);
	HL_TABLE()[idx].sg_start = p;
    }
    অন্যথা
    {
	vim_free(HL_TABLE()[idx].sg_stop);
	HL_TABLE()[idx].sg_stop = p;
    }
    ঘূৰাই TRUE;
}

/*
 * Handle the ":highlight .." command.
 * When using ":hi clear" this is called recursively for each group with
 * "forceit" and "init" both TRUE.
 */
    শূন্য
do_highlight(
    char_u	*সাৰি,
    পূর্ণাঙ্ক		forceit,
    পূর্ণাঙ্ক		init)	    // TRUE when called for initializing
{
    char_u	*name_end;
    char_u	*linep;
    char_u	*চাবি_start;
    char_u	*arg_start;
    char_u	*চাবি = রিক্ত, *arg = রিক্ত;
    long	i;
    পূর্ণাঙ্ক		id;
    পূর্ণাঙ্ক		idx;
    hl_group_T	item_before;
    পূর্ণাঙ্ক		did_change = FALSE;
    পূর্ণাঙ্ক		dodefault = FALSE;
    পূর্ণাঙ্ক		doclear = FALSE;
    পূর্ণাঙ্ক		dolink = FALSE;
    পূর্ণাঙ্ক		error = FALSE;
    পূর্ণাঙ্ক		is_normal_group = FALSE;	// "Normal" group
#ifdef FEAT_GUI_MSWIN
    পূর্ণাঙ্ক		is_titlebar_group = FALSE;	// "TitleBar" group
    পূর্ণাঙ্ক		is_titlebarnc_group = FALSE;	// "TitleBarNC" group
#অন্যথা
# define is_titlebar_group 0
# define is_titlebarnc_group 0
#endif
#ifdef FEAT_GUI_X11
    পূর্ণাঙ্ক		is_menu_group = FALSE;		// "Menu" group
    পূর্ণাঙ্ক		is_scrollbar_group = FALSE;	// "Scrollbar" group
    পূর্ণাঙ্ক		is_tooltip_group = FALSE;	// "Tooltip" group
#অন্যথা
# define is_menu_group 0
# define is_tooltip_group 0
# define is_scrollbar_group 0
#endif
#যদি defined(FEAT_GUI) || defined(FEAT_EVAL)
    পূর্ণাঙ্ক		do_colors = FALSE;		// need to update colors?
#endif
#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    পূর্ণাঙ্ক		did_highlight_changed = FALSE;
#endif

    // If no argument, list current highlighting.
    যদি (!init && ends_excmd2(সাৰি - 1, সাৰি))
    {
	বাবে (i = 1; i <= highlight_ga.ga_len && !got_int; ++i)
	    // TODO: only call when the group has attributes set
	    highlight_list_one((পূর্ণাঙ্ক)i);
	ঘূৰাই;
    }

    // Isolate the name.
    name_end = skiptowhite(সাৰি);
    linep = skipwhite(name_end);

    // Check for "default" argument.
    যদি (STRNCMP(সাৰি, "default", name_end - সাৰি) == 0)
    {
	dodefault = TRUE;
	সাৰি = linep;
	name_end = skiptowhite(সাৰি);
	linep = skipwhite(name_end);
    }

    // Check for "clear" or "link" argument.
    যদি (STRNCMP(সাৰি, "clear", name_end - সাৰি) == 0)
	doclear = TRUE;
    যদি (STRNCMP(সাৰি, "link", name_end - সাৰি) == 0)
	dolink = TRUE;

    // ":highlight {group-name}": list highlighting for one group.
    যদি (!doclear && !dolink && ends_excmd2(সাৰি, linep))
    {
	id = syn_namen2id(সাৰি, (পূর্ণাঙ্ক)(name_end - সাৰি));
	যদি (id == 0)
	    semsg(_(e_highlight_group_name_not_found_str), সাৰি);
	অন্যথা
	    highlight_list_one(id);
	ঘূৰাই;
    }

    // Handle ":highlight link {from} {to}" command.
    যদি (dolink)
    {
	char_u	    *from_start = linep;
	char_u	    *from_end;
	পূর্ণাঙ্ক	    from_len;
	char_u	    *to_start;
	char_u	    *to_end;
	পূর্ণাঙ্ক	    to_len;

	from_end = skiptowhite(from_start);
	to_start = skipwhite(from_end);
	to_end	 = skiptowhite(to_start);

	যদি (ends_excmd2(সাৰি, from_start) || ends_excmd2(সাৰি, to_start))
	{
	    semsg(_(e_not_enough_arguments_highlight_link_str), from_start);
	    ঘূৰাই;
	}

	যদি (!ends_excmd2(সাৰি, skipwhite(to_end)))
	{
	    semsg(_(e_too_many_arguments_highlight_link_str), from_start);
	    ঘূৰাই;
	}

	from_len = (পূর্ণাঙ্ক)(from_end - from_start);
	to_len = (পূর্ণাঙ্ক)(to_end - to_start);
	highlight_group_link(from_start, from_len, to_start, to_len,
						dodefault, forceit, init);
	ঘূৰাই;
    }

    যদি (doclear)
    {
	// ":highlight clear [group]" command.
	যদি (ends_excmd2(সাৰি, linep))
	{
	    // ":highlight clear" without group name
	    highlight_reset_all();
	    ঘূৰাই;
	}
	সাৰি = linep;
	name_end = skiptowhite(সাৰি);
	linep = skipwhite(name_end);
    }

    // Find the group name in the table.  If it does not exist yet, add it.
    id = syn_check_group(সাৰি, (পূর্ণাঙ্ক)(name_end - সাৰি));
    যদি (id == 0)	// failed (out of memory)
	ঘূৰাই;
    idx = id - 1;	// index is ID minus one

    // Return if "default" was used and the group already has settings.
    যদি (dodefault && hl_has_settings(idx, TRUE))
	ঘূৰাই;

    // Make a copy so we can check if any attribute actually changed.
    item_before = HL_TABLE()[idx];

    যদি (STRCMP(HL_TABLE()[idx].sg_name_u, "NORMAL") == 0)
	is_normal_group = TRUE;
#ifdef FEAT_GUI_MSWIN
    অন্যথা যদি (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBAR") == 0)
	is_titlebar_group = TRUE;
    অন্যথা যদি (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBARNC") == 0)
	is_titlebarnc_group = TRUE;
#endif
#ifdef FEAT_GUI_X11
    অন্যথা যদি (STRCMP(HL_TABLE()[idx].sg_name_u, "MENU") == 0)
	is_menu_group = TRUE;
    অন্যথা যদি (STRCMP(HL_TABLE()[idx].sg_name_u, "SCROLLBAR") == 0)
	is_scrollbar_group = TRUE;
    অন্যথা যদি (STRCMP(HL_TABLE()[idx].sg_name_u, "TOOLTIP") == 0)
	is_tooltip_group = TRUE;
#endif

    // Clear the highlighting for ":hi clear {group}" and ":hi clear".
    যদি (doclear || (forceit && init))
    {
	highlight_clear(idx);
	যদি (!doclear)
	    HL_TABLE()[idx].sg_set = 0;
    }

    যদি (!doclear)
	যেতিয়ালৈ (!ends_excmd2(সাৰি, linep))
	{
	    চাবি_start = linep;
	    যদি (*linep == '=')
	    {
		semsg(_(e_unexpected_equal_sign_str), চাবি_start);
		error = TRUE;
		ভঙ্গ;
	    }

	    // Note: Keep this in sync with get_highlight_group_key.

	    // Isolate the key ("term", "ctermfg", "ctermbg", "font", "guifg"
	    // or "guibg").
	    যেতিয়ালৈ (*linep && !VIM_ISWHITE(*linep) && *linep != '=')
		++linep;
	    vim_free(চাবি);
	    চাবি = vim_strnsave_up(চাবি_start, linep - চাবি_start);
	    যদি (চাবি == রিক্ত)
	    {
		error = TRUE;
		ভঙ্গ;
	    }
	    linep = skipwhite(linep);

	    যদি (STRCMP(চাবি, "NONE") == 0)
	    {
		যদি (!init || HL_TABLE()[idx].sg_set == 0)
		{
		    যদি (!init)
			HL_TABLE()[idx].sg_set |= SG_TERM+SG_CTERM+SG_GUI;
		    highlight_clear(idx);
		}
		অনুৱর্ত;
	    }

	    // Check for the equal sign.
	    যদি (*linep != '=')
	    {
		semsg(_(e_missing_equal_sign_str_2), চাবি_start);
		error = TRUE;
		ভঙ্গ;
	    }
	    ++linep;

	    // Isolate the argument.
	    linep = skipwhite(linep);
	    যদি (*linep == '\'')		// guifg='color name'
	    {
		arg_start = ++linep;
		linep = vim_strchr(linep, '\'');
		যদি (linep == রিক্ত)
		{
		    semsg(_(e_invalid_argument_str), চাবি_start);
		    error = TRUE;
		    ভঙ্গ;
		}
	    }
	    অন্যথা
	    {
		arg_start = linep;
		linep = skiptowhite(linep);
	    }
	    যদি (linep == arg_start)
	    {
		semsg(_(e_missing_argument_str), চাবি_start);
		error = TRUE;
		ভঙ্গ;
	    }
	    vim_free(arg);
	    arg = vim_strnsave(arg_start, linep - arg_start);
	    যদি (arg == রিক্ত)
	    {
		error = TRUE;
		ভঙ্গ;
	    }
	    যদি (*linep == '\'')
		++linep;

	    // Store the argument.
	    যদি (STRCMP(চাবি, "TERM") == 0
		    || STRCMP(চাবি, "CTERM") == 0
		    || STRCMP(চাবি, "GUI") == 0)
	    {
		যদি (!highlight_set_termgui_attr(idx, চাবি, arg, init))
		{
		    error = TRUE;
		    ভঙ্গ;
		}
	    }
	    অন্যথা যদি (STRCMP(চাবি, "FONT") == 0)
	    {
		// in non-GUI fonts are simply ignored
#ifdef FEAT_GUI
		যদি (highlight_set_font(idx, arg, is_normal_group,
					      is_menu_group, is_tooltip_group))
		    did_change = TRUE;
#endif
	    }
	    অন্যথা যদি (STRCMP(চাবি, "CTERMFG") == 0
		    || STRCMP(চাবি, "CTERMBG") == 0
		    || STRCMP(চাবি, "CTERMUL") == 0)
	    {
		যদি (!highlight_set_cterm_color(idx, চাবি, চাবি_start, arg,
							is_normal_group, init))
		{
		    error = TRUE;
		    ভঙ্গ;
		}
	    }
	    অন্যথা যদি (STRCMP(চাবি, "CTERMFONT") == 0)
	    {
		যদি (!highlight_set_cterm_font(idx, arg, init))
		{
		    error = TRUE;
		    ভঙ্গ;
		}
	    }
	    অন্যথা যদি (STRCMP(চাবি, "GUIFG") == 0)
	    {
#যদি defined(FEAT_GUI) || defined(FEAT_EVAL)
		যদি (highlight_set_guifg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    অন্যথা যদি (STRCMP(চাবি, "GUIBG") == 0)
	    {
#যদি defined(FEAT_GUI) || defined(FEAT_EVAL)
		যদি (highlight_set_guibg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    অন্যথা যদি (STRCMP(চাবি, "GUISP") == 0)
	    {
#যদি defined(FEAT_GUI) || defined(FEAT_EVAL)
		যদি (highlight_set_guisp(idx, arg, init))
		    did_change = TRUE;
#endif
	    }
	    অন্যথা যদি (STRCMP(চাবি, "START") == 0 || STRCMP(চাবি, "STOP") == 0)
	    {
		যদি (!highlight_set_startstop_termcode(idx, চাবি, arg, init))
		{
		    error = TRUE;
		    ভঙ্গ;
		}
	    }
	    অন্যথা
	    {
		semsg(_(e_illegal_argument_str_3), চাবি_start);
		error = TRUE;
		ভঙ্গ;
	    }
	    HL_TABLE()[idx].sg_cleared = FALSE;

	    // When highlighting has been given for a group, don't link it.
	    যদি (!init || !(HL_TABLE()[idx].sg_set & SG_LINK))
		HL_TABLE()[idx].sg_link = 0;

	    // Continue with next argument.
	    linep = skipwhite(linep);
	}

    // If there is an error, and it's a new entry, remove it from the table.
    যদি (error && idx == highlight_ga.ga_len)
	syn_unadd_group();
    অন্যথা
    {
	যদি (is_normal_group)
	{
	    HL_TABLE()[idx].sg_term_attr = 0;
	    HL_TABLE()[idx].sg_cterm_attr = 0;
#ifdef FEAT_GUI
	    HL_TABLE()[idx].sg_gui_attr = 0;
	    // Need to update all groups, because they might be using "bg"
	    // and/or "fg", which have been changed now.
#endif
#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    যদি (USE_24BIT)
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
	অন্যথা যদি (is_titlebar_group)
	{
	    যদি (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
	অন্যথা যদি (is_titlebarnc_group)
	{
	    যদি (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
#endif
#ifdef FEAT_GUI_X11
# ifdef FEAT_MENU
	অন্যথা যদি (is_menu_group)
	{
	    যদি (gui.in_use && do_colors)
		gui_mch_new_menu_colors();
	}
# endif
	অন্যথা যদি (is_scrollbar_group)
	{
	    যদি (gui.in_use && do_colors)
		gui_new_scrollbar_colors();
	    অন্যথা
		set_hl_attr(idx);
	}
# ifdef FEAT_BEVAL_GUI
	অন্যথা যদি (is_tooltip_group)
	{
	    যদি (gui.in_use && do_colors)
		gui_mch_new_tooltip_colors();
	}
# endif
#endif
	অন্যথা
	    set_hl_attr(idx);
#ifdef FEAT_EVAL
	HL_TABLE()[idx].sg_script_ctx = current_sctx;
	HL_TABLE()[idx].sg_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
    }

    vim_free(চাবি);
    vim_free(arg);

    // Only call highlight_changed() once, after a sequence of highlight
    // commands, and only if an attribute actually changed.
    যদি ((did_change
	   || memcmp(&HL_TABLE()[idx], &item_before, পরিমাণ(item_before)) != 0)
#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    && !did_highlight_changed
#endif
       )
    {
	// Do not trigger a redraw when highlighting is changed while
	// redrawing.  This may happen when evaluating 'statusline' changes the
	// StatusLine group.
	যদি (!updating_screen)
	    redraw_all_later(UPD_NOT_VALID);
	need_highlight_changed = TRUE;
    }
}

#যদি defined(EXITFREE)
    শূন্য
free_highlight(শূন্য)
{
    পূর্ণাঙ্ক	    i;

    বাবে (i = 0; i < highlight_ga.ga_len; ++i)
    {
	highlight_clear(i);
	vim_free(HL_TABLE()[i].sg_name);
	vim_free(HL_TABLE()[i].sg_name_u - HLNAME_চাবি_OFF);
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
    শূন্য
restore_cterm_colors(শূন্য)
{
#যদি defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
    // Since t_me has been set, this probably means that the user
    // wants to use this as default colors.  Need to reset default
    // background/foreground colors.
    mch_set_normal_colors();
#অন্যথা
# ifdef VIMDLL
    যদি (!gui.in_use)
    {
	mch_set_normal_colors();
	ঘূৰাই;
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
    স্থির পূর্ণাঙ্ক
hl_has_settings(পূর্ণাঙ্ক idx, পূর্ণাঙ্ক check_link)
{
    ঘূৰাই HL_TABLE()[idx].sg_cleared == 0
	 && (  HL_TABLE()[idx].sg_term_attr != 0
	    || HL_TABLE()[idx].sg_cterm_attr != 0
	    || HL_TABLE()[idx].sg_cterm_fg != 0
	    || HL_TABLE()[idx].sg_cterm_bg != 0
	    || HL_TABLE()[idx].sg_cterm_font != 0
#ifdef FEAT_GUI
	    || HL_TABLE()[idx].sg_gui_attr != 0
	    || HL_TABLE()[idx].sg_gui_fg_name != রিক্ত
	    || HL_TABLE()[idx].sg_gui_bg_name != রিক্ত
	    || HL_TABLE()[idx].sg_gui_sp_name != রিক্ত
	    || HL_TABLE()[idx].sg_font_name != রিক্ত
#endif
	    || (check_link && (HL_TABLE()[idx].sg_set & SG_LINK)));
}

/*
 * Clear highlighting for one group.
 */
    স্থির শূন্য
highlight_clear(পূর্ণাঙ্ক idx)
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
#যদি defined(FEAT_GUI) || defined(FEAT_EVAL)
    HL_TABLE()[idx].sg_gui = 0;
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_fg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_bg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_sp_name);
#endif
#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Set the normal foreground and background colors according to the "Normal"
 * highlighting group.  For X11 also set "Menu", "Scrollbar" and "Tooltip"
 * colors.  For MS-Windows also set "TitleBar" and "TitleBarNC" colors.
 */
    শূন্য
set_normal_colors(শূন্য)
{
# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    যদি (gui.in_use)
#  endif
    {
	যদি (set_group_colors((char_u *)"Normal",
				 &gui.norm_pixel, &gui.back_pixel,
				 FALSE, TRUE, FALSE))
	{
	    gui_mch_new_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  ifdef FEAT_GUI_MSWIN
	যদি (set_group_colors((char_u *)"TitleBar",
				 &gui.title_fg_pixel, &gui.title_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
	যদি (set_group_colors((char_u *)"TitleBarNC",
				 &gui.titlenc_fg_pixel, &gui.titlenc_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  endif
#  ifdef FEAT_GUI_X11
	যদি (set_group_colors((char_u *)"Menu",
			     &gui.menu_fg_pixel, &gui.menu_bg_pixel,
			     TRUE, FALSE, FALSE))
	{
#   ifdef FEAT_MENU
	    gui_mch_new_menu_colors();
#   endif
	    set_must_redraw(UPD_CLEAR);
	}
#   ifdef FEAT_BEVAL_GUI
	যদি (set_group_colors((char_u *)"Tooltip",
			     &gui.tooltip_fg_pixel, &gui.tooltip_bg_pixel,
			     FALSE, FALSE, TRUE))
	{
#    ifdef FEAT_TOOLBAR
	    gui_mch_new_tooltip_colors();
#    endif
	    set_must_redraw(UPD_CLEAR);
	}
#   endif
	যদি (set_group_colors((char_u *)"Scrollbar",
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
    অন্যথা
#  endif
    {
	পূর্ণাঙ্ক		idx;

	idx = syn_name2id((char_u *)"Normal") - 1;
	যদি (idx >= 0)
	{
	    gui_do_one_color(idx, FALSE, FALSE);

	    // If the normal fg or bg color changed a complete redraw is
	    // required.
	    যদি (cterm_normal_fg_gui_color != HL_TABLE()[idx].sg_gui_fg
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

#যদি defined(FEAT_GUI)
/*
 * Set the colors for "Normal", "Menu", "TitleBar", "TitleBarNC", "Tooltip" or
 * "Scrollbar".
 */
    স্থির পূর্ণাঙ্ক
set_group_colors(
    char_u	*name,
    guicolor_T	*fgp,
    guicolor_T	*bgp,
    পূর্ণাঙ্ক		do_menu,
    পূর্ণাঙ্ক		use_norm,
    পূর্ণাঙ্ক		do_tooltip)
{
    পূর্ণাঙ্ক		idx;

    idx = syn_name2id(name) - 1;
    যদি (idx < 0)
	ঘূৰাই FALSE;

    gui_do_one_color(idx, do_menu, do_tooltip);

    যদি (HL_TABLE()[idx].sg_gui_fg != INVALCOLOR)
	*fgp = HL_TABLE()[idx].sg_gui_fg;
    অন্যথা যদি (use_norm)
	*fgp = gui.def_norm_pixel;
    যদি (HL_TABLE()[idx].sg_gui_bg != INVALCOLOR)
	*bgp = HL_TABLE()[idx].sg_gui_bg;
    অন্যথা যদি (use_norm)
	*bgp = gui.def_back_pixel;
    ঘূৰাই TRUE;
}

/*
 * Get the font of the "Normal" group.
 * Returns "" when it's not found or not set.
 */
    char_u *
hl_get_font_name(শূন্য)
{
    পূর্ণাঙ্ক		id;
    char_u	*s;

    id = syn_name2id((char_u *)"Normal");
    যদি (id > 0)
    {
	s = HL_TABLE()[id - 1].sg_font_name;
	যদি (s != রিক্ত)
	    ঘূৰাই s;
    }
    ঘূৰাই (char_u *)"";
}

/*
 * Set font for "Normal" group.  Called by gui_mch_init_font() when a font has
 * actually chosen to be used.
 */
    শূন্য
hl_set_font_name(char_u *font_name)
{
    পূর্ণাঙ্ক	    id;

    id = syn_name2id((char_u *)"Normal");
    যদি (id <= 0)
	ঘূৰাই;

    vim_free(HL_TABLE()[id - 1].sg_font_name);
    HL_TABLE()[id - 1].sg_font_name = vim_strsave(font_name);
}

/*
 * Set background color for "Normal" group.  Called by gui_set_bg_color()
 * when the color is known.
 */
    শূন্য
hl_set_bg_color_name(
    char_u  *name)	    // must have been allocated
{
    পূর্ণাঙ্ক	    id;

    যদি (name == রিক্ত)
	ঘূৰাই;

    id = syn_name2id((char_u *)"Normal");
    যদি (id <= 0)
	ঘূৰাই;

    vim_free(HL_TABLE()[id - 1].sg_gui_bg_name);
    HL_TABLE()[id - 1].sg_gui_bg_name = name;
}

/*
 * Set foreground color for "Normal" group.  Called by gui_set_fg_color()
 * when the color is known.
 */
    শূন্য
hl_set_fg_color_name(
    char_u  *name)	    // must have been allocated
{
    পূর্ণাঙ্ক	    id;

    যদি (name == রিক্ত)
	ঘূৰাই;

    id = syn_name2id((char_u *)"Normal");
    যদি (id <= 0)
	ঘূৰাই;

    vim_free(HL_TABLE()[id - 1].sg_gui_fg_name);
    HL_TABLE()[id - 1].sg_gui_fg_name = name;
}

/*
 * Return the handle for a font name.
 * Returns NOFONT when failed.
 */
    স্থির GuiFont
font_name2handle(char_u *name)
{
    যদি (STRCMP(name, "NONE") == 0)
	ঘূৰাই NOFONT;

    ঘূৰাই gui_mch_get_font(name, TRUE);
}

# ifdef FEAT_XFONTSET
/*
 * Return the handle for a fontset name.
 * Returns NOFONTSET when failed.
 */
    স্থির GuiFontset
fontset_name2handle(char_u *name, পূর্ণাঙ্ক fixed_width)
{
    যদি (STRCMP(name, "NONE") == 0)
	ঘূৰাই NOFONTSET;

    ঘূৰাই gui_mch_get_fontset(name, TRUE, fixed_width);
}
# endif

/*
 * Get the font or fontset for one highlight group.
 */
    স্থির শূন্য
hl_do_font(
    পূর্ণাঙ্ক		idx,
    char_u	*arg,
    পূর্ণাঙ্ক		do_normal,		// set normal font
    পূর্ণাঙ্ক		do_menu UNUSED,		// set menu font
    পূর্ণাঙ্ক		do_tooltip UNUSED,	// set tooltip font
    পূর্ণাঙ্ক		free_font)		// free current font/fontset
{
# ifdef FEAT_XFONTSET
    // If 'guifontset' is not empty, first try using the name as a
    // fontset.  If that doesn't work, use it as a font name.
    যদি (*p_guifontset != NUL
#  ifdef FONTSET_ALWAYS
	|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
	// In Motif, the Tooltip highlight group is always a fontset
	|| do_tooltip
#  endif
	    )
    {
	যদি (free_font)
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
    যদি (HL_TABLE()[idx].sg_fontset != NOFONTSET)
    {
	// If it worked and it's the Normal group, use it as the normal
	// fontset.  Same for the Menu group.
	যদি (do_normal)
	    gui_init_font(arg, TRUE);
#  যদি defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	যদি (do_menu)
	{
#   ifdef FONTSET_ALWAYS
	    gui.menu_fontset = HL_TABLE()[idx].sg_fontset;
#   অন্যথা
	    // YIKES!  This is a bug waiting to crash the program
	    gui.menu_font = HL_TABLE()[idx].sg_fontset;
#   endif
	    gui_mch_new_menu_font();
	}
#   ifdef FEAT_BEVAL_GUI
	যদি (do_tooltip)
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
    অন্যথা
# endif
    {
	যদি (free_font)
	    gui_mch_free_font(HL_TABLE()[idx].sg_font);
	HL_TABLE()[idx].sg_font = font_name2handle(arg);
	// If it worked and it's the Normal group, use it as the
	// normal font.  Same for the Menu group.
	যদি (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    যদি (do_normal)
		gui_init_font(arg, FALSE);
# ifndef FONTSET_ALWAYS
#  যদি defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	    যদি (do_menu)
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

#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return the handle for a color name.
 * Returns INVALCOLOR when failed.
 */
    guicolor_T
color_name2handle(char_u *name)
{
    যদি (STRCMP(name, "NONE") == 0)
	ঘূৰাই INVALCOLOR;

    যদি (STRICMP(name, "fg") == 0 || STRICMP(name, "foreground") == 0)
    {
# যদি defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	যদি (gui.in_use)
# endif
# ifdef FEAT_GUI
	    ঘূৰাই gui.norm_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	যদি (cterm_normal_fg_gui_color != INVALCOLOR)
	    ঘূৰাই cterm_normal_fg_gui_color;
	// Guess that the foreground is black or white.
	ঘূৰাই GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "black" : "white"));
# endif
    }
    যদি (STRICMP(name, "bg") == 0 || STRICMP(name, "background") == 0)
    {
# যদি defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	যদি (gui.in_use)
# endif
# ifdef FEAT_GUI
	    ঘূৰাই gui.back_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	যদি (cterm_normal_bg_gui_color != INVALCOLOR)
	    ঘূৰাই cterm_normal_bg_gui_color;
	// Guess that the background is white or black.
	ঘূৰাই GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "white" : "black"));
# endif
    }

    ঘূৰাই GUI_GET_COLOR(name);
}

// On MS-Windows an RGB macro is available and it produces 0x00bbggrr color
// values as used by the MS-Windows GDI api.  It should be used only for
// MS-Windows GDI builds.
# যদি defined(RGB) && defined(MSWIN) && !defined(FEAT_GUI)
#  undef RGB
# endif
# ifndef RGB
#  define RGB(r, g, b)	(((r)<<16) | ((g)<<8) | (b))
# endif

# ifdef VIMDLL
    স্থির guicolor_T
gui_adjust_rgb(guicolor_T c)
{
    যদি (gui.in_use)
	ঘূৰাই c;
    অন্যথা
	ঘূৰাই ((c & 0xff) << 16) | (c & 0x00ff00) | ((c >> 16) & 0xff);
}
# অন্যথা
#  define gui_adjust_rgb(c) (c)
# endif

    স্থির পূর্ণাঙ্ক
hex_digit(পূর্ণাঙ্ক c)
{
    যদি (SAFE_isdigit(c))
	ঘূৰাই c - '0';
    c = TOLOWER_ASC(c);
    যদি (c >= 'a' && c <= 'f')
	ঘূৰাই c - 'a' + 10;
    ঘূৰাই 0x1ffffff;
}

    স্থির guicolor_T
decode_hex_color(char_u *hex)
{
    guicolor_T color;

    যদি (hex[0] != '#' || STRLEN(hex) != 7)
	ঘূৰাই INVALCOLOR;

    // Name is in "#rrggbb" format
    color = RGB(((hex_digit(hex[1]) << 4) + hex_digit(hex[2])),
		((hex_digit(hex[3]) << 4) + hex_digit(hex[4])),
		((hex_digit(hex[5]) << 4) + hex_digit(hex[6])));
    যদি (color > 0xffffff)
	ঘূৰাই INVALCOLOR;
    ঘূৰাই gui_adjust_rgb(color);
}

# ifdef FEAT_EVAL
// Returns the color currently mapped to the given name or INVALCOLOR if no
// such name exists in the color table. The convention is to use lowercase for
// all keys in the v:colornames dictionary. The value can be either a string in
// the form #rrggbb or a number, either of which is converted to a guicolor_T.
    স্থির guicolor_T
colorname2rgb(char_u *name)
{
    dict_T      *colornames_table = get_vim_var_dict(VV_COLORNAMES);
    char_u      *lc_name;
    dictitem_T  *colentry;
    char_u      *colstr;
    varnumber_T colnum;

    lc_name = strlow_save(name);
    যদি (lc_name == রিক্ত)
	ঘূৰাই INVALCOLOR;

    colentry = dict_find(colornames_table, lc_name, -1);
    vim_free(lc_name);
    যদি (colentry == রিক্ত)
	ঘূৰাই INVALCOLOR;

    যদি (colentry->di_tv.v_type == VAR_STRING)
    {
	colstr = tv_get_string_strict(&colentry->di_tv);
	যদি ((STRLEN(colstr) == 7) && (*colstr == '#'))
	{
	    ঘূৰাই decode_hex_color(colstr);
	}
	অন্যথা
	{
	    semsg(_(e_bad_color_string_str), colstr);
	    ঘূৰাই INVALCOLOR;
	}
    }

    যদি (colentry->di_tv.v_type == VAR_সংখ্যা)
    {
	colnum = tv_get_সংখ্যা(&colentry->di_tv);
	ঘূৰাই (guicolor_T)colnum;
    }

    ঘূৰাই INVALCOLOR;
}

# endif

    guicolor_T
gui_get_color_cmn(char_u *name)
{
    guicolor_T  color;
    // Only non X11 colors (not present in rgb.txt) and colors in
    // color_name_tab[], useful when $VIMRUNTIME is not found,.
    // must be sorted by the 'value' field because it is used by bsearch()!
    স্থির keyvalue_T rgb_tab[] = {
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
    যদি (color != INVALCOLOR)
	ঘূৰাই color;

    target.চাবি = 0;
    target.মান.string = name;
    target.মান.length = 0;	// not used, see cmp_keyvalue_value_i()
    entry = (keyvalue_T *)bsearch(&target, &rgb_tab, ARRAY_LENGTH(rgb_tab),
	    পরিমাণ(rgb_tab[0]), cmp_keyvalue_মান_i);
    যদি (entry != রিক্ত)
	ঘূৰাই gui_adjust_rgb((guicolor_T)entry->চাবি);

# যদি defined(FEAT_EVAL)
    /*
     * Not a traditional color. Load additional color aliases and then consult the alias table.
     */

    color = colorname2rgb(name);
    যদি (color == INVALCOLOR)
    {
	load_default_colors_lists();
	color = colorname2rgb(name);
    }

    ঘূৰাই color;
# অন্যথা
    ঘূৰাই INVALCOLOR;
# endif
}

    guicolor_T
gui_get_rgb_color_cmn(পূর্ণাঙ্ক r, পূর্ণাঙ্ক g, পূর্ণাঙ্ক b)
{
    guicolor_T  color = RGB(r, g, b);

    যদি (color > 0xffffff)
	ঘূৰাই INVALCOLOR;
    ঘূৰাই gui_adjust_rgb(color);
}
#endif

/*
 * Table with the specifications for an attribute number.
 * Note that this table is used by ALL buffers.  This is required because the
 * GUI can redraw at any time for any buffer.
 */
স্থির garray_T	term_attr_table = {0, 0, 0, 0, রিক্ত};

#define TERM_ATTR_ENTRY(idx) ((attrentry_T *)term_attr_table.ga_data)[idx]

স্থির garray_T	cterm_attr_table = {0, 0, 0, 0, রিক্ত};

#define CTERM_ATTR_ENTRY(idx) ((attrentry_T *)cterm_attr_table.ga_data)[idx]

#ifdef FEAT_GUI
স্থির garray_T	gui_attr_table = {0, 0, 0, 0, রিক্ত};

# define GUI_ATTR_ENTRY(idx) ((attrentry_T *)gui_attr_table.ga_data)[idx]
#endif

/*
 * Return the attr number for a set of colors and font.
 * Add a new entry to the term_attr_table, cterm_attr_table or gui_attr_table
 * if the combination is new.
 * Return 0 for error (no more room).
 */
    স্থির পূর্ণাঙ্ক
get_attr_entry(garray_T *table, attrentry_T *aep)
{
    পূর্ণাঙ্ক		i;
    attrentry_T	*taep;
    স্থির পূর্ণাঙ্ক	recursive = FALSE;

    // Init the table, in case it wasn't done yet.
    table->ga_itemsize = পরিমাণ(attrentry_T);
    table->ga_growsize = 7;

    // Try to find an entry with the same specifications.
    বাবে (i = 0; i < table->ga_len; ++i)
    {
	taep = &(((attrentry_T *)table->ga_data)[i]);
	যদি (	   aep->ae_attr == taep->ae_attr
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
			&& (aep->ae_u.term.start == রিক্ত)
					    == (taep->ae_u.term.start == রিক্ত)
			&& (aep->ae_u.term.start == রিক্ত
			    || STRCMP(aep->ae_u.term.start,
						  taep->ae_u.term.start) == 0)
			&& (aep->ae_u.term.stop == রিক্ত)
					     == (taep->ae_u.term.stop == রিক্ত)
			&& (aep->ae_u.term.stop == রিক্ত
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

	ঘূৰাই i + ATTR_OFF;
    }

    যদি (table->ga_len + ATTR_OFF > MAX_TYPENR)
    {
	// Running out of attribute entries!  remove all attributes, and
	// compute new ones for all groups.
	// When called recursively, we are really out of numbers.
	যদি (recursive)
	{
	    emsg(_(e_too_many_different_highlighting_attributes_in_use));
	    ঘূৰাই 0;
	}
	recursive = TRUE;

	clear_hl_tables();

	set_must_redraw(UPD_CLEAR);

	বাবে (i = 0; i < highlight_ga.ga_len; ++i)
	    set_hl_attr(i);

	recursive = FALSE;
    }

    // This is a new combination of colors and font, add an entry.
    যদি (ga_grow(table, 1) == FAIL)
	ঘূৰাই 0;

    taep = &(((attrentry_T *)table->ga_data)[table->ga_len]);
    CLEAR_POINTER(taep);
    taep->ae_attr = aep->ae_attr;
#ifdef FEAT_GUI
    যদি (table == &gui_attr_table)
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
    যদি (table == &term_attr_table)
    {
	যদি (aep->ae_u.term.start == রিক্ত)
	    taep->ae_u.term.start = রিক্ত;
	অন্যথা
	    taep->ae_u.term.start = vim_strsave(aep->ae_u.term.start);
	যদি (aep->ae_u.term.stop == রিক্ত)
	    taep->ae_u.term.stop = রিক্ত;
	অন্যথা
	    taep->ae_u.term.stop = vim_strsave(aep->ae_u.term.stop);
    }
    অন্যথা যদি (table == &cterm_attr_table)
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
    ঘূৰাই (table->ga_len - 1 + ATTR_OFF);
}

#যদি defined(FEAT_TERMINAL)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    পূর্ণাঙ্ক
get_cterm_attr_idx(পূর্ণাঙ্ক attr, পূর্ণাঙ্ক fg, পূর্ণাঙ্ক bg)
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
    ঘূৰাই get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#যদি defined(FEAT_TERMINAL) && defined(FEAT_TERMGUICOLORS)
/*
 * Get an attribute index for a 'termguicolors' entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    পূর্ণাঙ্ক
get_tgc_attr_idx(পূর্ণাঙ্ক attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    যদি (fg == INVALCOLOR && bg == INVALCOLOR)
    {
	// If both GUI colors are not set fall back to the cterm colors.  Helps
	// if the GUI only has an attribute, such as undercurl.
	at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
    }
    অন্যথা
    {
	at_en.ae_u.cterm.fg_rgb = fg;
	at_en.ae_u.cterm.bg_rgb = bg;
    }
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
    ঘূৰাই get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#যদি defined(FEAT_TERMINAL) && defined(FEAT_GUI)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    পূর্ণাঙ্ক
get_gui_attr_idx(পূর্ণাঙ্ক attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    at_en.ae_u.gui.fg_color = fg;
    at_en.ae_u.gui.bg_color = bg;
    ঘূৰাই get_attr_entry(&gui_attr_table, &at_en);
}
#endif

/*
 * Clear all highlight tables.
 */
    শূন্য
clear_hl_tables(শূন্য)
{
    পূর্ণাঙ্ক		i;
    attrentry_T	*taep;

#ifdef FEAT_GUI
    ga_clear(&gui_attr_table);
#endif
    বাবে (i = 0; i < term_attr_table.ga_len; ++i)
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
    পূর্ণাঙ্ক
hl_combine_attr(পূর্ণাঙ্ক char_attr, পূর্ণাঙ্ক prim_attr)
{
    attrentry_T *char_aep = রিক্ত;
    attrentry_T *prim_aep;
    attrentry_T new_en;

    যদি (char_attr == 0)
	ঘূৰাই prim_attr;
    যদি (char_attr <= HL_ALL && prim_attr <= HL_ALL)
	ঘূৰাই ATTR_COMBINE(char_attr, prim_attr);
#ifdef FEAT_GUI
    যদি (gui.in_use)
    {
	যদি (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	যদি (char_aep != রিক্ত)
	    new_en = *char_aep;
	অন্যথা
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    যদি (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	যদি (prim_attr <= HL_ALL)
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	অন্যথা
	{
	    prim_aep = syn_gui_attr2entry(prim_attr);
	    যদি (prim_aep != রিক্ত)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		যদি (prim_aep->ae_u.gui.fg_color != INVALCOLOR)
		    new_en.ae_u.gui.fg_color = prim_aep->ae_u.gui.fg_color;
		যদি (prim_aep->ae_u.gui.bg_color != INVALCOLOR)
		    new_en.ae_u.gui.bg_color = prim_aep->ae_u.gui.bg_color;
		যদি (prim_aep->ae_u.gui.sp_color != INVALCOLOR)
		    new_en.ae_u.gui.sp_color = prim_aep->ae_u.gui.sp_color;
		যদি (prim_aep->ae_u.gui.font != NOFONT)
		    new_en.ae_u.gui.font = prim_aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
		যদি (prim_aep->ae_u.gui.fontset != NOFONTSET)
		    new_en.ae_u.gui.fontset = prim_aep->ae_u.gui.fontset;
# endif
	    }
	}
	ঘূৰাই get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    যদি (IS_CTERM)
    {
	যদি (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	যদি (char_aep != রিক্ত)
	    new_en = *char_aep;
	অন্যথা
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    যদি (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	যদি (prim_attr <= HL_ALL)
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	অন্যথা
	{
	    prim_aep = syn_cterm_attr2entry(prim_attr);
	    যদি (prim_aep != রিক্ত)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		যদি (prim_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color = prim_aep->ae_u.cterm.fg_color;
		যদি (prim_aep->ae_u.cterm.bg_color > 0)
		    new_en.ae_u.cterm.bg_color = prim_aep->ae_u.cterm.bg_color;
		যদি (prim_aep->ae_u.cterm.ul_color > 0)
		    new_en.ae_u.cterm.ul_color = prim_aep->ae_u.cterm.ul_color;
		যদি (prim_aep->ae_u.cterm.font > 0)
		    new_en.ae_u.cterm.font = prim_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		// If both fg and bg are not set fall back to cterm colors.
		// Helps for SpellBad which uses undercurl in the GUI.
		যদি (COLOR_INVALID(prim_aep->ae_u.cterm.fg_rgb)
			&& COLOR_INVALID(prim_aep->ae_u.cterm.bg_rgb))
		{
		    যদি (prim_aep->ae_u.cterm.fg_color > 0)
			new_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
		    যদি (prim_aep->ae_u.cterm.bg_color > 0)
			new_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
		}
		অন্যথা
		{
		    যদি (prim_aep->ae_u.cterm.fg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.fg_rgb = prim_aep->ae_u.cterm.fg_rgb;
		    যদি (prim_aep->ae_u.cterm.bg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.bg_rgb = prim_aep->ae_u.cterm.bg_rgb;
		}
		যদি (prim_aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		    new_en.ae_u.cterm.ul_rgb = prim_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	}
	ঘূৰাই get_attr_entry(&cterm_attr_table, &new_en);
    }

    যদি (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    যদি (char_aep != রিক্ত)
	new_en = *char_aep;
    অন্যথা
    {
	CLEAR_FIELD(new_en);
	যদি (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    যদি (prim_attr <= HL_ALL)
	new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
    অন্যথা
    {
	prim_aep = syn_term_attr2entry(prim_attr);
	যদি (prim_aep != রিক্ত)
	{
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_aep->ae_attr);
	    যদি (prim_aep->ae_u.term.start != রিক্ত)
	    {
		new_en.ae_u.term.start = prim_aep->ae_u.term.start;
		new_en.ae_u.term.stop = prim_aep->ae_u.term.stop;
	    }
	}
    }
    ঘূৰাই get_attr_entry(&term_attr_table, &new_en);
}

// ANSI color order: Black, Red, Green, Yellow, Blue, Magenta,
// Cyan, White, then bright variants.  Approximate RGB values used when
// only a cterm color number is known (no guifg/guibg).  Real terminal
// palettes may differ if the user reconfigured their emulator, but
// these are reasonable xterm-ish defaults.
স্থির ধ্রুৱ long_u cterm_color_16[16] = {
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
    স্থির guicolor_T
cterm_color_to_rgb(পূর্ণাঙ্ক color_nr)
{
    যদি (color_nr < 1 || color_nr > 16)
	ঘূৰাই INVALCOLOR;
    ঘূৰাই (guicolor_T)cterm_color_16[color_nr - 1];
}
#endif

/*
 * Convert an xterm 256-color index (0-255) to an approximate RGB triple.
 * Uses the standard xterm palette: 0-15 ANSI (cterm_color_16), 16-231
 * 6x6x6 cube, 232-255 grayscale ramp.
 */
    স্থির শূন্য
cterm_idx_to_rgb(পূর্ণাঙ্ক idx, পূর্ণাঙ্ক *r, পূর্ণাঙ্ক *g, পূর্ণাঙ্ক *b)
{
    স্থির ধ্রুৱ পূর্ণাঙ্ক cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

    যদি (idx < 0 || idx > 255)
    {
	*r = *g = *b = 0;
	ঘূৰাই;
    }
    যদি (idx < 16)
    {
	long_u rgb = cterm_color_16[idx];
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
    }
    অন্যথা যদি (idx < 232)
    {
	পূর্ণাঙ্ক n = idx - 16;
	*r = cube[n / 36 % 6];
	*g = cube[n / 6  % 6];
	*b = cube[n      % 6];
    }
    অন্যথা
    {
	পূর্ণাঙ্ক v = 8 + (idx - 232) * 10;
	*r = *g = *b = v;
    }
}

/*
 * Approximate an RGB triple to the nearest xterm 256-color index.
 * Searches the 6x6x6 cube and the grayscale ramp; returns 0-255.
 */
    স্থির পূর্ণাঙ্ক
rgb_to_cterm_idx(পূর্ণাঙ্ক r, পূর্ণাঙ্ক g, পূর্ণাঙ্ক b)
{
    স্থির ধ্রুৱ পূর্ণাঙ্ক cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };
    পূর্ণাঙ্ক best = 0;
    long best_d = -1;
    পূর্ণাঙ্ক i;

    // Search the 6x6x6 cube.
    বাবে (i = 16; i < 232; ++i)
    {
	পূর্ণাঙ্ক n = i - 16;
	পূর্ণাঙ্ক cr = cube[n / 36 % 6];
	পূর্ণাঙ্ক cg = cube[n / 6  % 6];
	পূর্ণাঙ্ক cb = cube[n      % 6];
	long d = (cr - r) * (cr - r) + (cg - g) * (cg - g) + (cb - b) * (cb - b);
	যদি (best_d < 0 || d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    // Search the grayscale ramp.
    বাবে (i = 232; i < 256; ++i)
    {
	পূর্ণাঙ্ক v = 8 + (i - 232) * 10;
	long d = (v - r) * (v - r) + (v - g) * (v - g) + (v - b) * (v - b);
	যদি (d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    ঘূৰাই best;
}

/*
 * Resolve a color to an RGB triple.  Prefer the gui RGB value (set by
 * guifg/guibg) when valid, otherwise convert the cterm 256-color index.
 * Returns true on success; false when no color is defined.
 * cterm_c is 1-based (0 means "no color").
 */
    স্থির bool
resolve_color_to_rgb(পূর্ণাঙ্ক cterm_c, guicolor_T rgb UNUSED, পূর্ণাঙ্ক *r, পূর্ণাঙ্ক *g, পূর্ণাঙ্ক *b)
{
#ifdef FEAT_TERMGUICOLORS
    যদি (!COLOR_INVALID(rgb))
    {
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
	ঘূৰাই true;
    }
#endif
    যদি (cterm_c > 0)
    {
	cterm_idx_to_rgb(cterm_c - 1, r, g, b);
	ঘূৰাই true;
    }
    ঘূৰাই false;
}

/*
 * get a RGB fallback color from gui, cterm or default color
 */
    স্থির guicolor_T
resolve_fallback_color(পূর্ণাঙ্ক cterm_c, guicolor_T rgb, guicolor_T default_rgb)
{
    পূর্ণাঙ্ক red, green, blue;
    যদি (!resolve_color_to_rgb(cterm_c, rgb, &red, &green, &blue))
	ঘূৰাই default_rgb;
    অন্যথা
	ঘূৰাই (red << 16) | (green << 8) | blue;
}

/*
 * get a RGB fallback foreground color from guifg, ctermfg or deduced from background
 */
    স্থির শূন্য
resolve_fallback_fg_to_rgb(শূন্য)
{
    guicolor_T fgcolor_or_gui_fgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    fgcolor_or_gui_fgcolor = cterm_normal_fg_gui_color;
#endif
#ifdef FEAT_GUI
    যদি (gui.in_use)
	fgcolor_or_gui_fgcolor = gui.norm_pixel;
#endif
    fallback_fg_rgb = resolve_fallback_color(cterm_normal_fg_color, fgcolor_or_gui_fgcolor, (*p_bg == 'l') ? 0x000000 : 0xFFFFFF);
}

/*
 * get a RGB fallback background color from guifg, ctermbg or deduced from background
 */
    স্থির শূন্য
resolve_fallback_bg_to_rgb(শূন্য)
{
    guicolor_T bgcolor_or_gui_bgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    bgcolor_or_gui_bgcolor = cterm_normal_bg_gui_color;
#endif
#ifdef FEAT_GUI
    যদি (gui.in_use)
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
    স্থির পূর্ণাঙ্ক
blend_cterm_colors(পূর্ণাঙ্ক popup_c, guicolor_T popup_rgb,
		   পূর্ণাঙ্ক under_c, guicolor_T under_rgb,
		   পূর্ণাঙ্ক default_rgb,
		   পূর্ণাঙ্ক blend_val)
{
    পূর্ণাঙ্ক pr, pg, pb, ur, ug, ub, r, g, b;

    যদি (t_colors < 256)
	ঘূৰাই popup_c;
    যদি (!resolve_color_to_rgb(popup_c, popup_rgb, &pr, &pg, &pb))
	ঘূৰাই under_c;
    যদি (blend_val <= 0)
	ঘূৰাই rgb_to_cterm_idx(pr, pg, pb) + 1;
    যদি (!resolve_color_to_rgb(under_c, under_rgb, &ur, &ug, &ub))
    {
	ur = (default_rgb >> 16) & 0xFF;
	ug = (default_rgb >> 8) & 0xFF;
	ub = default_rgb & 0xFF;
    }
    যদি (blend_val >= 100)
	ঘূৰাই rgb_to_cterm_idx(ur, ug, ub) + 1;
    r = pr + (ur - pr) * blend_val / 100;
    g = pg + (ug - pg) * blend_val / 100;
    b = pb + (ub - pb) * blend_val / 100;
    ঘূৰাই rgb_to_cterm_idx(r, g, b) + 1;
}

#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Blend two RGB colors based on blend value (0-100).
 * blend: 0=return first argument color, 100=return second argument color
 * If bg_color is INVALCOLOR, high blend means more visible (return INVALCOLOR).
 */
    স্থির guicolor_T
blend_colors(guicolor_T popup_color, guicolor_T bg_color, পূর্ণাঙ্ক blend_val)
{
    পূর্ণাঙ্ক r1, g1, b1, r2, g2, b2, r, g, b;

    // CTERMCOLOR is a sentinel meaning "use the cterm color"; for blending
    // it has no real RGB so treat it like INVALCOLOR.
    যদি (COLOR_INVALID(popup_color))
	ঘূৰাই INVALCOLOR;

    // Fully transparent: use underlying color as-is.
    যদি (blend_val >= 100)
	ঘূৰাই bg_color;

    r1 = (popup_color >> 16) & 0xFF;
    g1 = (popup_color >> 8) & 0xFF;
    b1 = popup_color & 0xFF;

    যদি (COLOR_INVALID(bg_color))
	bg_color = fallback_bg_rgb;

    r2 = (bg_color >> 16) & 0xFF;
    g2 = (bg_color >> 8) & 0xFF;
    b2 = bg_color & 0xFF;

    r = r1 + (r2 - r1) * blend_val / 100;
    g = g1 + (g2 - g1) * blend_val / 100;
    b = b1 + (b2 - b1) * blend_val / 100;

    ঘূৰাই (r << 16) | (g << 8) | b;
}
#endif

/*
 * Blend attributes for popup windows and menus with opacity.
 * Blends foreground and/or background colors based on blend value (0-100).
 * blend: 0 = opaque (use popup colors), 100 = transparent (use background colors)
 * blend_fg: TRUE to blend foreground color, FALSE to keep popup foreground
 */
    স্থির পূর্ণাঙ্ক
hl_blend_attr_common(
    পূর্ণাঙ্ক		char_attr,
    পূর্ণাঙ্ক		popup_attr,
    পূর্ণাঙ্ক		blend,
    পূর্ণাঙ্ক		blend_fg)
{
    attrentry_T *char_aep = রিক্ত;
    attrentry_T *popup_aep;
    attrentry_T new_en;
    attrentry_T tmp_en;

#ifdef FEAT_GUI
    যদি (gui.in_use)
    {
	যদি (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	যদি (char_aep != রিক্ত)
	    new_en = *char_aep;
	অন্যথা
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    যদি (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	যদি (popup_attr <= HL_ALL)
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
	অন্যথা
	    popup_aep = syn_gui_attr2entry(popup_attr);

	যদি (popup_aep != রিক্ত)
	{
	    guicolor_T popup_bg_rgb = popup_aep->ae_u.gui.bg_color;
	    যদি (COLOR_INVALID(popup_bg_rgb))
		popup_bg_rgb = fallback_bg_rgb;

	    যদি (blend_fg)
	    {
		// blend_fg=TRUE: fade underlying text toward popup bg.
		{
		    পূর্ণাঙ্ক base_fg = fallback_fg_rgb;
		    যদি (char_aep != রিক্ত
			    && char_aep->ae_u.gui.fg_color != INVALCOLOR)
			base_fg = char_aep->ae_u.gui.fg_color;
		    new_en.ae_u.gui.fg_color = blend_colors(
			    popup_bg_rgb, base_fg, blend);
		}
	    }
	    অন্যথা
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
		যদি (char_aep != রিক্ত
			&& !COLOR_INVALID(char_aep->ae_u.gui.bg_color))
		    underlying_bg = char_aep->ae_u.gui.bg_color;
		new_en.ae_u.gui.bg_color = blend_colors(
			popup_bg_rgb,
			underlying_bg, blend);
	    }
	}
	ঘূৰাই get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    যদি (IS_CTERM)
    {
	যদি (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	যদি (char_aep != রিক্ত)
	    new_en = *char_aep;
	অন্যথা
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    যদি (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	যদি (popup_attr <= HL_ALL)
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
	অন্যথা
	    popup_aep = syn_cterm_attr2entry(popup_attr);

	যদি (popup_aep != রিক্ত)
	{
	    guicolor_T popup_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
	    // Fall back to cterm color converted to RGB when gui color is not set.
	    popup_bg_rgb = popup_aep->ae_u.cterm.bg_rgb;
	    যদি (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color > 0)
		popup_bg_rgb = cterm_color_to_rgb(
			popup_aep->ae_u.cterm.bg_color);
#endif
	    // assign default color if guibg and ctermbg are not set for popup
	    যদি (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color == 0)
		popup_bg_rgb = fallback_bg_rgb;

	    যদি (!blend_fg)
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
		যদি (popup_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color =
			popup_aep->ae_u.cterm.fg_color;
		অন্যথা যদি (cterm_normal_fg_color > 0)
		    new_en.ae_u.cterm.fg_color = cterm_normal_fg_color;
		অন্যথা
		    // black-ish or white-ish
		    new_en.ae_u.cterm.fg_color = (*p_bg == 'l') ? 1 : 16;
		new_en.ae_u.cterm.ul_color = popup_aep->ae_u.cterm.ul_color;
		new_en.ae_u.cterm.font = popup_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		new_en.ae_u.cterm.ul_rgb = popup_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	    অন্যথা
	    {
		// blend_fg=TRUE: fade underlying fg toward popup bg in
		// the 256-color palette.  Used when the popup is over a
		// cell rendered with cterm colors (no termguicolors RGB).
		পূর্ণাঙ্ক under_fg = (char_aep != রিক্ত)
		    ? char_aep->ae_u.cterm.fg_color : 0;
		guicolor_T under_fg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		যদি (char_aep != রিক্ত)
		    under_fg_rgb = char_aep->ae_u.cterm.fg_rgb;
#endif
		new_en.ae_u.cterm.fg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_fg, under_fg_rgb, fallback_fg_rgb, blend);
	    }
	    // Approximate cterm bg by blending with the underlying bg
	    // in the 256-color palette and mapping to the nearest entry.
	    {
		পূর্ণাঙ্ক under_bg = (char_aep != রিক্ত)
		    ? char_aep->ae_u.cterm.bg_color : 0;
		guicolor_T under_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		যদি (char_aep != রিক্ত)
		    under_bg_rgb = char_aep->ae_u.cterm.bg_rgb;
#endif
		new_en.ae_u.cterm.bg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_bg, under_bg_rgb, fallback_bg_rgb, blend);
	    }
#ifdef FEAT_TERMGUICOLORS
	    // Blend RGB colors for termguicolors mode.
	    {
		যদি (blend_fg)
		{
		    // blend_fg=TRUE: fade underlying text toward popup bg.
		    যদি (popup_bg_rgb != INVALCOLOR)
		    {
			পূর্ণাঙ্ক base_fg = fallback_fg_rgb;
			// CTERMCOLOR is a sentinel meaning "use the cterm
			// color"; treat it as no underlying color so it is
			// not blended in as a real near-white pixel.
			যদি (char_aep != রিক্ত
				&& !COLOR_INVALID(char_aep->ae_u.cterm.fg_rgb))
			    base_fg = char_aep->ae_u.cterm.fg_rgb;
			new_en.ae_u.cterm.fg_rgb = blend_colors(
				popup_bg_rgb, base_fg, blend);
		    }
		}
		অন্যথা
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

		    যদি (COLOR_INVALID(popup_fg)
			    && popup_aep->ae_u.cterm.fg_color > 0)
			popup_fg = cterm_color_to_rgb(
				popup_aep->ae_u.cterm.fg_color);

		    যদি (!COLOR_INVALID(popup_fg))
			new_en.ae_u.cterm.fg_rgb = popup_fg;
		    অন্যথা যদি (!COLOR_INVALID(cterm_normal_fg_gui_color))
			new_en.ae_u.cterm.fg_rgb = cterm_normal_fg_gui_color;
		    অন্যথা
			new_en.ae_u.cterm.fg_rgb = fallback_fg_rgb;
		}
		যদি (popup_bg_rgb != INVALCOLOR)
		{
		    // Blend popup bg toward underlying bg
		    guicolor_T underlying_bg = fallback_bg_rgb;
		    যদি (char_aep != রিক্ত
			    && !COLOR_INVALID(char_aep->ae_u.cterm.bg_rgb))
			underlying_bg = char_aep->ae_u.cterm.bg_rgb;
		    new_en.ae_u.cterm.bg_rgb = blend_colors(
			    popup_bg_rgb, underlying_bg, blend);
		}
	    }
#endif
	}
	ঘূৰাই get_attr_entry(&cterm_attr_table, &new_en);
    }

    যদি (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    যদি (char_aep != রিক্ত)
	new_en = *char_aep;
    অন্যথা
    {
	CLEAR_FIELD(new_en);
	যদি (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    // For term mode, no separate background color handling.
    ঘূৰাই get_attr_entry(&term_attr_table, &new_en);
}

/*
 * Blend attributes for popup windows with opacity.
 */
    পূর্ণাঙ্ক
hl_blend_attr(পূর্ণাঙ্ক char_attr, পূর্ণাঙ্ক popup_attr, পূর্ণাঙ্ক blend, পূর্ণাঙ্ক blend_fg)
{
    যদি (blend >= 100 && blend_fg)
	ঘূৰাই char_attr;

    ঘূৰাই hl_blend_attr_common(char_attr, popup_attr, blend, blend_fg);
}

/*
 * Blend underlying foreground and background colors for pum space cells.
 */
    পূর্ণাঙ্ক
hl_pum_blend_attr(পূর্ণাঙ্ক char_attr, পূর্ণাঙ্ক popup_attr, পূর্ণাঙ্ক blend)
{
    ঘূৰাই hl_blend_attr_common(char_attr, popup_attr, blend, TRUE);
}

#ifdef FEAT_GUI
    attrentry_T *
syn_gui_attr2entry(পূর্ণাঙ্ক attr)
{
    attr -= ATTR_OFF;
    যদি (attr >= gui_attr_table.ga_len)	    // did ":syntax clear"
	ঘূৰাই রিক্ত;
    ঘূৰাই &(GUI_ATTR_ENTRY(attr));
}
#endif

/*
 * Get the highlight attributes (HL_BOLD etc.) from an attribute nr.
 * Only to be used when "attr" > HL_ALL.
 */
    পূর্ণাঙ্ক
syn_attr2attr(পূর্ণাঙ্ক attr)
{
    attrentry_T	*aep;

#ifdef FEAT_GUI
    যদি (gui.in_use)
	aep = syn_gui_attr2entry(attr);
    অন্যথা
#endif
	যদি (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	অন্যথা
	    aep = syn_term_attr2entry(attr);

    যদি (aep == রিক্ত)	    // highlighting not set
	ঘূৰাই 0;
    ঘূৰাই aep->ae_attr;
}


    attrentry_T *
syn_term_attr2entry(পূর্ণাঙ্ক attr)
{
    attr -= ATTR_OFF;
    যদি (attr >= term_attr_table.ga_len)	    // did ":syntax clear"
	ঘূৰাই রিক্ত;
    ঘূৰাই &(TERM_ATTR_ENTRY(attr));
}

    attrentry_T *
syn_cterm_attr2entry(পূর্ণাঙ্ক attr)
{
    attr -= ATTR_OFF;
    যদি (attr >= cterm_attr_table.ga_len)	// did ":syntax clear"
	ঘূৰাই রিক্ত;
    ঘূৰাই &(CTERM_ATTR_ENTRY(attr));
}

#define LIST_ATTR   1
#define LIST_STRING 2
#define LIST_INT    3

    স্থির শূন্য
highlight_list_one(পূর্ণাঙ্ক id)
{
    hl_group_T	    *sgp;
    পূর্ণাঙ্ক		    didh = FALSE;

    sgp = &HL_TABLE()[id - 1];	    // index is ID minus one

    যদি (message_filtered(sgp->sg_name))
	ঘূৰাই;

    // Note: Keep this in sync with expand_highlight_group().
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_term, রিক্ত, "term");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_start, "start");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_stop, "stop");

    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_cterm, রিক্ত, "cterm");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_fg, রিক্ত, "ctermfg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_bg, রিক্ত, "ctermbg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_ul, রিক্ত, "ctermul");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_font, রিক্ত, "ctermfont");

#যদি defined(FEAT_GUI) || defined(FEAT_EVAL)
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_gui, রিক্ত, "gui");
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

    যদি (sgp->sg_link && !got_int)
    {
	(শূন্য)syn_list_header(didh, 9999, id);
	didh = TRUE;
	বাৰ্তা_puts_attr("links to", HL_ATTR(HLF_D));
	বাৰ্তা_putchar(' ');
	বাৰ্তা_outtrans(HL_TABLE()[HL_TABLE()[id - 1].sg_link - 1].sg_name);
    }

    যদি (!didh)
	highlight_list_arg(id, didh, LIST_STRING, 0, (char_u *)"cleared", "");
#ifdef FEAT_EVAL
    যদি (p_verbose > 0)
	last_set_বাৰ্তা(sgp->sg_script_ctx);
#endif
}

    স্থির char_u*
highlight_arg_to_string(
    পূর্ণাঙ্ক		type,
    পূর্ণাঙ্ক		iarg,
    char_u	*sarg,
    char_u	*buf)
{
    যদি (type == LIST_INT)
	sprintf((ৱর্ণ *)buf, "%d", iarg - 1);
    অন্যথা যদি (type == LIST_STRING)
	ঘূৰাই sarg;
    অন্যথা // type == LIST_ATTR
    {
	size_t buflen;

	buf[0] = NUL;
	buflen = 0;
	বাবে (পূর্ণাঙ্ক i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(highlight_index_tab); ++i)
	{
	    যদি (iarg & highlight_index_tab[i]->চাবি)
	    {
		যদি (buflen > 0)
		{
		    STRCPY(buf + buflen, (char_u *)",");
		    ++buflen;
		}
		STRCPY(buf + buflen, highlight_index_tab[i]->মান.string);
		buflen += highlight_index_tab[i]->মান.length;
		iarg &= ~highlight_index_tab[i]->চাবি;	    // don't want "inverse"/"reverse"
	    }
	}
    }
    ঘূৰাই buf;
}

    স্থির পূর্ণাঙ্ক
highlight_list_arg(
    পূর্ণাঙ্ক		id,
    পূর্ণাঙ্ক		didh,
    পূর্ণাঙ্ক		type,
    পূর্ণাঙ্ক		iarg,
    char_u	*sarg,
    ৱর্ণ	*name)
{
    char_u	buf[MAX_ATTR_LEN];
    char_u	*ts;

    যদি (got_int)
	ঘূৰাই FALSE;

    যদি (type == LIST_STRING ? (sarg == রিক্ত) : (iarg == 0))
	ঘূৰাই didh;

    ts = highlight_arg_to_string(type, iarg, sarg, buf);

    (শূন্য)syn_list_header(didh,
	    (পূর্ণাঙ্ক)(vim_strsize(ts) + STRLEN(name) + 1), id);
    didh = TRUE;
    যদি (!got_int)
    {
	যদি (*name != NUL)
	{
	    বাৰ্তা_puts_attr(name, HL_ATTR(HLF_D));
	    বাৰ্তা_puts_attr("=", HL_ATTR(HLF_D));
	}
	বাৰ্তা_outtrans(ts);
    }
    ঘূৰাই didh;
}

#যদি ((defined(FEAT_EVAL) || defined(FEAT_PRINTER))) && defined(FEAT_SYN_HL)
/*
 * Return "1" if highlight group "id" has attribute "flag".
 * Return NULL otherwise.
 */
    char_u *
highlight_has_attr(
    পূর্ণাঙ্ক		id,
    পূর্ণাঙ্ক		flag,
    পূর্ণাঙ্ক		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    পূর্ণাঙ্ক		attr;

    যদি (id <= 0 || id > highlight_ga.ga_len)
	ঘূৰাই রিক্ত;

# যদি defined(FEAT_GUI) || defined(FEAT_EVAL)
    যদি (modec == 'g')
	attr = HL_TABLE()[id - 1].sg_gui;
    অন্যথা
# endif
    {
	যদি (modec == 'c')
	    attr = HL_TABLE()[id - 1].sg_cterm;
	অন্যথা
	    attr = HL_TABLE()[id - 1].sg_term;
    }

    যদি (attr & flag)
	ঘূৰাই (char_u *)"1";
    ঘূৰাই রিক্ত;
}
#endif

#যদি defined(FEAT_SYN_HL) && defined(FEAT_EVAL)
/*
 * Return color name of highlight group "id".
 */
    char_u *
highlight_color(
    পূর্ণাঙ্ক		id,
    char_u	*what,	// "font", "fg", "bg", "sp", "ul", "fg#", "bg#" or "sp#"
    পূর্ণাঙ্ক		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    স্থির char_u	name[20];
    পূর্ণাঙ্ক			n;
    পূর্ণাঙ্ক			fg = FALSE;
    পূর্ণাঙ্ক			sp = FALSE;
    পূর্ণাঙ্ক			ul = FALSE;
    পূর্ণাঙ্ক			font = FALSE;

    যদি (id <= 0 || id > highlight_ga.ga_len)
	ঘূৰাই রিক্ত;

    যদি (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'g')
	fg = TRUE;
    অন্যথা যদি (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'o'
	     && TOLOWER_ASC(what[2]) == 'n' && TOLOWER_ASC(what[3]) == 't')
	font = TRUE;
    অন্যথা যদি (TOLOWER_ASC(what[0]) == 's' && TOLOWER_ASC(what[1]) == 'p')
	sp = TRUE;
    অন্যথা যদি (TOLOWER_ASC(what[0]) == 'u' && TOLOWER_ASC(what[1]) == 'l')
	ul = TRUE;
    অন্যথা যদি (!(TOLOWER_ASC(what[0]) == 'b' && TOLOWER_ASC(what[1]) == 'g'))
	ঘূৰাই রিক্ত;
    যদি (modec == 'g')
    {
# যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI
	// return font name
	যদি (font)
	    ঘূৰাই HL_TABLE()[id - 1].sg_font_name;
#  endif

	// return #RRGGBB form (only possible when GUI is running)
	যদি ((USE_24BIT) && what[2] == '#')
	{
	    guicolor_T		color;
	    long_u		rgb;
	    স্থির char_u	buf[10];

	    যদি (fg)
		color = HL_TABLE()[id - 1].sg_gui_fg;
	    অন্যথা যদি (sp)
		color = HL_TABLE()[id - 1].sg_gui_sp;
	    অন্যথা
		color = HL_TABLE()[id - 1].sg_gui_bg;
	    যদি (color == INVALCOLOR)
		ঘূৰাই রিক্ত;
	    rgb = (long_u)GUI_MCH_GET_RGB(color);
	    sprintf((ৱর্ণ *)buf, "#%02x%02x%02x",
				      (unsigned)(rgb >> 16),
				      (unsigned)(rgb >> 8) & 255,
				      (unsigned)rgb & 255);
	    ঘূৰাই buf;
	}
# endif
	যদি (fg)
	    ঘূৰাই (HL_TABLE()[id - 1].sg_gui_fg_name);
	যদি (sp)
	    ঘূৰাই (HL_TABLE()[id - 1].sg_gui_sp_name);
	ঘূৰাই (HL_TABLE()[id - 1].sg_gui_bg_name);
    }
    যদি (sp)
	ঘূৰাই রিক্ত;
    যদি (modec == 'c')
    {
	যদি (fg)
	    n = HL_TABLE()[id - 1].sg_cterm_fg - 1;
	অন্যথা যদি (ul)
	    n = HL_TABLE()[id - 1].sg_cterm_ul - 1;
	অন্যথা যদি (font)
	    n = HL_TABLE()[id - 1].sg_cterm_font - 1;
	অন্যথা
	    n = HL_TABLE()[id - 1].sg_cterm_bg - 1;
	যদি (n < 0)
	    ঘূৰাই রিক্ত;
	sprintf((ৱর্ণ *)name, "%d", n);
	ঘূৰাই name;
    }
    // term doesn't have color
    ঘূৰাই রিক্ত;
}
#endif

#যদি defined(FEAT_SYN_HL) \
	    && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)) \
	&& defined(FEAT_PRINTER)
/*
 * Return color name of highlight group "id" as RGB value.
 */
    long_u
highlight_gui_color_rgb(
    পূর্ণাঙ্ক		id,
    পূর্ণাঙ্ক		fg)	// TRUE = fg, FALSE = bg
{
    guicolor_T	color;

    যদি (id <= 0 || id > highlight_ga.ga_len)
	ঘূৰাই 0L;

    যদি (fg)
	color = HL_TABLE()[id - 1].sg_gui_fg;
    অন্যথা
	color = HL_TABLE()[id - 1].sg_gui_bg;

    যদি (color == INVALCOLOR)
	ঘূৰাই 0L;

    ঘূৰাই GUI_MCH_GET_RGB(color);
}
#endif

/*
 * Output the syntax list header.
 * Return TRUE when started a new line.
 */
    পূর্ণাঙ্ক
syn_list_header(
    পূর্ণাঙ্ক	    did_header,		// did header already
    পূর্ণাঙ্ক	    outlen,		// length of string that comes
    পূর্ণাঙ্ক	    id)			// highlight group id
{
    পূর্ণাঙ্ক	    endcol = 19;
    পূর্ণাঙ্ক	    newline = TRUE;
    পূর্ণাঙ্ক	    name_col = 0;

    যদি (!did_header)
    {
	বাৰ্তা_putchar('\n');
	যদি (got_int)
	    ঘূৰাই TRUE;
	বাৰ্তা_outtrans(HL_TABLE()[id - 1].sg_name);
	name_col = বাৰ্তা_col;
	endcol = 15;
    }
    অন্যথা যদি (বাৰ্তা_col + outlen + 1 >= cmdline_width)
    {
	বাৰ্তা_putchar('\n');
	যদি (got_int)
	    ঘূৰাই TRUE;
    }
    অন্যথা
    {
	যদি (বাৰ্তা_col >= endcol)	// wrap around is like starting a new line
	    newline = FALSE;
    }

    যদি (বাৰ্তা_col >= endcol)	// output at least one space
	endcol = বাৰ্তা_col + 1;
    যদি (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    বাৰ্তা_advance(endcol);

    // Show "xxx" with the attributes.
    যদি (!did_header)
    {
	যদি (endcol == cmdline_width - 1 && endcol <= name_col)
	    বাৰ্তা_putchar(' ');
	বাৰ্তা_puts_attr("xxx", syn_id2attr(id));
	বাৰ্তা_putchar(' ');
    }

    ঘূৰাই newline;
}

/*
 * Set the attribute numbers for a highlight group.
 * Called after one of the attributes has changed.
 */
    স্থির শূন্য
set_hl_attr(
    পূর্ণাঙ্ক		idx)	    // index in array
{
    attrentry_T	    at_en;
    hl_group_T	    *sgp = HL_TABLE() + idx;

    // The "Normal" group doesn't need an attribute number
    যদি (sgp->sg_name_u != রিক্ত && STRCMP(sgp->sg_name_u, "NORMAL") == 0)
	ঘূৰাই;

#ifdef FEAT_GUI
    // For the GUI mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    যদি (sgp->sg_gui_fg == INVALCOLOR
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
    অন্যথা
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
    যদি (sgp->sg_start == রিক্ত && sgp->sg_stop == রিক্ত)
	sgp->sg_term_attr = sgp->sg_term;
    অন্যথা
    {
	at_en.ae_attr = sgp->sg_term;
	at_en.ae_u.term.start = sgp->sg_start;
	at_en.ae_u.term.stop = sgp->sg_stop;
	sgp->sg_term_attr = get_attr_entry(&term_attr_table, &at_en);
    }

    // For the color term mode: If there are other than "normal"
    // highlighting attributes, need to allocate an attr number.
    যদি (sgp->sg_cterm_fg == 0 && sgp->sg_cterm_bg == 0 &&
	sgp->sg_cterm_ul == 0 && sgp->sg_cterm_font == 0
#ifdef FEAT_TERMGUICOLORS
	    && sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
#endif
	    )
	sgp->sg_cterm_attr = sgp->sg_cterm;
    অন্যথা
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
	যদি (sgp->sg_cterm & (HL_UNDERLINE | HL_UNDERCURL
			   | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED))
	    at_en.ae_u.cterm.ul_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_sp);
	অন্যথা
	    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	যদি (at_en.ae_u.cterm.fg_rgb == INVALCOLOR
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
    স্থির পূর্ণাঙ্ক
syn_override(পূর্ণাঙ্ক id)
{
    যদি (overrides != রিক্ত && overrides->arr != রিক্ত)
	বাবে (পূর্ণাঙ্ক k = 0; k < overrides->len; k++)
	    যদি (overrides->arr[k].from == id)
		ঘূৰাই overrides->arr[k].to;
    ঘূৰাই id;
}

/*
 * Lookup a highlight group name by pointer and length.
 * If it is not found, 0 is returned.
 */
    স্থির পূর্ণাঙ্ক
syn_name2id_len(char_u *name, পূর্ণাঙ্ক len)
{
    char_u	name_u[MAX_SYN_NAME + 1];
    hashitem_T	*hi;

    যদি (len > MAX_SYN_NAME)
	len = MAX_SYN_NAME;
    mch_memmove(name_u, name, len);
    name_u[len] = NUL;
    vim_strup(name_u);
    যদি (!highlight_ht_inited)
	ঘূৰাই 0;
    hi = hash_find(&highlight_ht, name_u);
    যদি (HASHITEM_EMPTY(hi))
	ঘূৰাই 0;
    ঘূৰাই HI2HLNAME(hi)->hn_id;
}

/*
 * Lookup a highlight group name and return its ID.
 * If it is not found, 0 is returned.
 */
    পূর্ণাঙ্ক
syn_name2id(char_u *name)
{
    // Avoid using stricmp() too much, it's slow on some systems.
    ঘূৰাই syn_name2id_len(name, (পূর্ণাঙ্ক)STRLEN(name));
}

/*
 * Lookup a highlight group name and return its attributes.
 * Return zero if not found.
 */
    পূর্ণাঙ্ক
syn_name2attr(char_u *name)
{
    পূর্ণাঙ্ক id = syn_name2id(name);

    যদি (id != 0)
	ঘূৰাই syn_id2attr(id);
    ঘূৰাই 0;
}

#যদি defined(FEAT_EVAL)
/*
 * Return TRUE if highlight group "name" exists.
 */
    পূর্ণাঙ্ক
highlight_exists(char_u *name)
{
    ঘূৰাই (syn_name2id(name) > 0);
}

# যদি defined(FEAT_অনুসন্ধান_EXTRA)
/*
 * Return the name of highlight group "id".
 * When not a valid ID return an empty string.
 */
    char_u *
syn_id2name(পূর্ণাঙ্ক id)
{
    যদি (id <= 0 || id > highlight_ga.ga_len)
	ঘূৰাই (char_u *)"";
    ঘূৰাই HL_TABLE()[id - 1].sg_name;
}
# endif
#endif

/*
 * Like syn_name2id(), but take a pointer + length argument.
 */
    পূর্ণাঙ্ক
syn_namen2id(char_u *linep, পূর্ণাঙ্ক len)
{
    ঘূৰাই syn_name2id_len(linep, len);
}

/*
 * Find highlight group name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    পূর্ণাঙ্ক
syn_check_group(char_u *pp, পূর্ণাঙ্ক len)
{
    পূর্ণাঙ্ক	    id;
    char_u  *name;

    যদি (len > MAX_SYN_NAME)
    {
	emsg(_(e_highlight_group_name_too_long));
	ঘূৰাই 0;
    }
    id = syn_name2id_len(pp, len);
    যদি (id == 0)			// doesn't exist yet
    {
	name = vim_strnsave(pp, len);
	যদি (name == রিক্ত)
	    ঘূৰাই 0;
	id = syn_add_group(name);
    }
    ঘূৰাই id;
}

/*
 * Add new highlight group and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    স্থির পূর্ণাঙ্ক
syn_add_group(char_u *name)
{
    char_u	*p;
    char_u	*name_up;

    // Check that the name is valid (ASCII letters, digits, underscores, dots, or hyphens).
    বাবে (p = name; *p != NUL; ++p)
    {
	যদি (!vim_isprintc(*p))
	{
	    emsg(_(e_unprintable_character_in_group_name));
	    vim_free(name);
	    ঘূৰাই 0;
	}
	অন্যথা যদি (!ASCII_ISALNUM(*p) && *p != '_' && *p != '.' && *p != '-')
	{
	    // This is an error, but since there previously was no check only
	    // give a warning.
	    বাৰ্তা_source(HL_ATTR(HLF_W));
	    বাৰ্তা(_("W18: Invalid character in group name"));
	    ভঙ্গ;
	}
    }

    // First call for this growarray: init growing array.
    যদি (highlight_ga.ga_data == রিক্ত)
    {
	highlight_ga.ga_itemsize = পরিমাণ(hl_group_T);
	highlight_ga.ga_growsize = 10;
	hash_init(&highlight_ht);
	highlight_ht_inited = true;
    }

    যদি (highlight_ga.ga_len >= MAX_HL_ID)
    {
	emsg(_(e_too_many_highlight_and_syntax_groups));
	vim_free(name);
	ঘূৰাই 0;
    }

    // Make room for at least one other syntax_highlight entry.
    যদি (ga_grow(&highlight_ga, 1) == FAIL)
    {
	vim_free(name);
	ঘূৰাই 0;
    }

    {
	hlname_T    *hn;
	পূর্ণাঙ্ক	    len = (পূর্ণাঙ্ক)STRLEN(name);

	hn = alloc(offsetof(hlname_T, hn_চাবি) + len + 1);
	যদি (hn == রিক্ত)
	{
	    vim_free(name);
	    ঘূৰাই 0;
	}
	vim_strncpy(hn->hn_চাবি, name, len);
	vim_strup(hn->hn_চাবি);
	hn->hn_id = highlight_ga.ga_len + 1;	// ID is index plus one
	name_up = hn->hn_চাবি;
    }

    CLEAR_POINTER(&(HL_TABLE()[highlight_ga.ga_len]));
    HL_TABLE()[highlight_ga.ga_len].sg_name = name;
    HL_TABLE()[highlight_ga.ga_len].sg_name_u = name_up;
#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_TABLE()[highlight_ga.ga_len].sg_gui_bg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_fg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_sp = INVALCOLOR;
#endif
    hash_add(&highlight_ht, name_up, "highlight");
    ++highlight_ga.ga_len;

    ঘূৰাই highlight_ga.ga_len;		    // ID is index plus one
}

/*
 * When, just after calling syn_add_group(), an error is discovered, this
 * function deletes the new name.
 */
    স্থির শূন্য
syn_unadd_group(শূন্য)
{
    hashitem_T *hi;

    --highlight_ga.ga_len;
    hi = hash_find(&highlight_ht, HL_TABLE()[highlight_ga.ga_len].sg_name_u);
    যদি (!HASHITEM_EMPTY(hi))
	hash_remove(&highlight_ht, hi, "highlight");
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name);
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name_u - HLNAME_চাবি_OFF);
}

/*
 * Translate a group ID to highlight attributes.
 * "hl_id" must be valid: > 0, caller must check.
 */
    পূর্ণাঙ্ক
syn_id2attr(পূর্ণাঙ্ক hl_id)
{
    পূর্ণাঙ্ক		attr;
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    প্রতিজ্ঞা(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

#ifdef FEAT_GUI
    // Only use GUI attr when the GUI is being used.
    যদি (gui.in_use)
	attr = sgp->sg_gui_attr;
    অন্যথা
#endif
	যদি (IS_CTERM)
	    attr = sgp->sg_cterm_attr;
	অন্যথা
	    attr = sgp->sg_term_attr;

    ঘূৰাই attr;
}

#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Get the GUI colors and attributes for a group ID.
 * NOTE: the colors will be INVALCOLOR when not set, the color otherwise.
 */
    পূর্ণাঙ্ক
syn_id2colors(পূর্ণাঙ্ক hl_id, guicolor_T *fgp, guicolor_T *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    প্রতিজ্ঞা(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

    *fgp = sgp->sg_gui_fg;
    *bgp = sgp->sg_gui_bg;
    ঘূৰাই sgp->sg_gui;
}
#endif

#যদি (defined(MSWIN) \
	    && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL)) \
	    && defined(FEAT_TERMGUICOLORS)) \
	|| defined(FEAT_TERMINAL)
    শূন্য
syn_id2cterm_bg(পূর্ণাঙ্ক hl_id, পূর্ণাঙ্ক *fgp, পূর্ণাঙ্ক *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    প্রতিজ্ঞা(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
    *fgp = sgp->sg_cterm_fg - 1;
    *bgp = sgp->sg_cterm_bg - 1;
}
#endif

/*
 * Translate a group ID to the final group ID (following links).
 */
    পূর্ণাঙ্ক
syn_get_final_id(পূর্ণাঙ্ক hl_id)
{
    পূর্ণাঙ্ক		গুনতি;
    hl_group_T	*sgp;

    যদি (hl_id > highlight_ga.ga_len || hl_id < 1)
	ঘূৰাই 0;			// Can be called from eval!!

    // Follow links until there is no more.
    // Look out for loops!  Break after 100 links.
    বাবে (গুনতি = 100; --গুনতি >= 0; )
    {
	পূর্ণাঙ্ক tmp = hl_id;

	sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
	যদি (sgp->sg_link == 0 || sgp->sg_link > highlight_ga.ga_len)
	    ভঙ্গ;

	// This is to handle highlight groups that are overridden but are in the
	// middle of a link chain.
	hl_id = syn_override(hl_id);
	যদি (tmp != hl_id)
	    অনুৱর্ত;
	hl_id = sgp->sg_link;
    }
    ঘূৰাই syn_override(hl_id);
}

#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Call this function just after the GUI has started.
 * Also called when 'termguicolors' was set, gui.in_use will be FALSE then.
 * It finds the font and color handles for the highlighting groups.
 */
    শূন্য
highlight_gui_started(শূন্য)
{
    পূর্ণাঙ্ক	    idx;

    // First get the colors from the "Normal" and "Menu" group, if set
    যদি (USE_24BIT)
	set_normal_colors();

    বাবে (idx = 0; idx < highlight_ga.ga_len; ++idx)
	gui_do_one_color(idx, FALSE, FALSE);

    highlight_changed();
}

    স্থির শূন্য
gui_do_one_color(
    পূর্ণাঙ্ক		idx,
    পূর্ণাঙ্ক		do_menu UNUSED,	   // TRUE: might set the menu font
    পূর্ণাঙ্ক		do_tooltip UNUSED) // TRUE: might set the tooltip font
{
    পূর্ণাঙ্ক		didit = FALSE;

# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    যদি (gui.in_use)
#  endif
	যদি (HL_TABLE()[idx].sg_font_name != রিক্ত)
	{
	    hl_do_font(idx, HL_TABLE()[idx].sg_font_name, FALSE, do_menu,
							    do_tooltip, TRUE);
	    didit = TRUE;
	}
# endif
    যদি (HL_TABLE()[idx].sg_gui_fg_name != রিক্ত)
    {
	HL_TABLE()[idx].sg_gui_fg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_fg_name);
	didit = TRUE;
    }
    যদি (HL_TABLE()[idx].sg_gui_bg_name != রিক্ত)
    {
	HL_TABLE()[idx].sg_gui_bg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_bg_name);
	didit = TRUE;
    }
    যদি (HL_TABLE()[idx].sg_gui_sp_name != রিক্ত)
    {
	HL_TABLE()[idx].sg_gui_sp =
			    color_name2handle(HL_TABLE()[idx].sg_gui_sp_name);
	didit = TRUE;
    }
    যদি (didit)	// need to get a new attr number
	set_hl_attr(idx);
}
#endif

#যদি defined(USER_HIGHLIGHT) && defined(FEAT_STL_OPT)
/*
 * Apply difference between User[1-9] and HLF_S to HLF_SNC, HLF_ST or HLF_STNC.
 */
    স্থির শূন্য
combine_stl_hlt(
	পূর্ণাঙ্ক id,
	পূর্ণাঙ্ক id_S,
	পূর্ণাঙ্ক id_alt,
	পূর্ণাঙ্ক hlcnt,
	পূর্ণাঙ্ক i,
	পূর্ণাঙ্ক hlf,
	পূর্ণাঙ্ক *table)
{
    hl_group_T *hlt = HL_TABLE();

    যদি (id_alt == 0)
    {
	CLEAR_POINTER(&hlt[hlcnt + i]);
	hlt[hlcnt + i].sg_term = highlight_attr[hlf];
	hlt[hlcnt + i].sg_cterm = highlight_attr[hlf];
# যদি defined(FEAT_GUI) || defined(FEAT_EVAL)
	hlt[hlcnt + i].sg_gui = highlight_attr[hlf];
# endif
    }
    অন্যথা
	mch_memmove(&hlt[hlcnt + i],
		    &hlt[id_alt - 1],
		    পরিমাণ(hl_group_T));
    hlt[hlcnt + i].sg_link = 0;

    hlt[hlcnt + i].sg_term ^=
	hlt[id - 1].sg_term ^ hlt[id_S - 1].sg_term;
    যদি (hlt[id - 1].sg_start != hlt[id_S - 1].sg_start)
	hlt[hlcnt + i].sg_start = hlt[id - 1].sg_start;
    যদি (hlt[id - 1].sg_stop != hlt[id_S - 1].sg_stop)
	hlt[hlcnt + i].sg_stop = hlt[id - 1].sg_stop;
    hlt[hlcnt + i].sg_cterm ^=
	hlt[id - 1].sg_cterm ^ hlt[id_S - 1].sg_cterm;
    যদি (hlt[id - 1].sg_cterm_fg != hlt[id_S - 1].sg_cterm_fg)
	hlt[hlcnt + i].sg_cterm_fg = hlt[id - 1].sg_cterm_fg;
    যদি (hlt[id - 1].sg_cterm_bg != hlt[id_S - 1].sg_cterm_bg)
	hlt[hlcnt + i].sg_cterm_bg = hlt[id - 1].sg_cterm_bg;
    যদি (hlt[id - 1].sg_cterm_font != hlt[id_S - 1].sg_cterm_font)
	hlt[hlcnt + i].sg_cterm_font = hlt[id - 1].sg_cterm_font;
# যদি defined(FEAT_GUI) || defined(FEAT_EVAL)
    hlt[hlcnt + i].sg_gui ^=
	hlt[id - 1].sg_gui ^ hlt[id_S - 1].sg_gui;
# endif
# ifdef FEAT_GUI
    যদি (hlt[id - 1].sg_gui_fg != hlt[id_S - 1].sg_gui_fg)
	hlt[hlcnt + i].sg_gui_fg = hlt[id - 1].sg_gui_fg;
    যদি (hlt[id - 1].sg_gui_bg != hlt[id_S - 1].sg_gui_bg)
	hlt[hlcnt + i].sg_gui_bg = hlt[id - 1].sg_gui_bg;
    যদি (hlt[id - 1].sg_gui_sp != hlt[id_S - 1].sg_gui_sp)
	hlt[hlcnt + i].sg_gui_sp = hlt[id - 1].sg_gui_sp;
    যদি (hlt[id - 1].sg_font != hlt[id_S - 1].sg_font)
	hlt[hlcnt + i].sg_font = hlt[id - 1].sg_font;
#  ifdef FEAT_XFONTSET
    যদি (hlt[id - 1].sg_fontset != hlt[id_S - 1].sg_fontset)
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
    পূর্ণাঙ্ক
highlight_changed(শূন্য)
{
    পূর্ণাঙ্ক		hlf;
    পূর্ণাঙ্ক		i;
    char_u	*p;
    char_u	*default_hl;
    পূর্ণাঙ্ক		attr;
    char_u	*end;
    পূর্ণাঙ্ক		id;
#ifdef USER_HIGHLIGHT
    char_u      userhl[30];  // use 30 to avoid compiler warning
# ifdef FEAT_STL_OPT
    পূর্ণাঙ্ক		id_S = -1;
    পূর্ণাঙ্ক		id_SNC = 0;
#  ifdef FEAT_TERMINAL
    পূর্ণাঙ্ক		id_ST = 0;
    পূর্ণাঙ্ক		id_STNC = 0;
#  endif
    পূর্ণাঙ্ক		hlcnt;
# endif
#endif
    win_T	*wp;

    need_highlight_changed = FALSE;

#ifdef FEAT_TERMINAL
    term_update_colors_all();
    term_update_hlfwin_all();
#endif

    // Clear all attributes.
    বাবে (hlf = 0; hlf < (পূর্ণাঙ্ক)HLF_গুনতি; ++hlf)
    {
	highlight_attr[hlf] = 0;
	highlight_attr_raw[hlf] = 0;
	highlight_ids[hlf] = 0;
    }

    default_hl = get_highlight_default();

    // First set all attributes to their default value.
    // Then use the attributes from the 'highlight' option, unless it already
    // matches the default and would repeat the same work.
    বাবে (i = 0; i < 2; ++i)
    {
	যদি (i)
	{
	    যদি (default_hl != রিক্ত && p_hl != রিক্ত
				      && STRCMP(default_hl, p_hl) == 0)
		অনুৱর্ত;
	    p = p_hl;
	}
	অন্যথা
	    p = default_hl;
	যদি (p == রিক্ত)	    // just in case
	    অনুৱর্ত;

	যেতিয়ালৈ (*p)
	{
	    বাবে (hlf = 0; hlf < (পূর্ণাঙ্ক)HLF_গুনতি; ++hlf)
		যদি (hl_flags[hlf] == *p)
		    ভঙ্গ;
	    ++p;
	    যদি (hlf == (পূর্ণাঙ্ক)HLF_গুনতি || *p == NUL)
		ঘূৰাই FAIL;

	    // Allow several hl_flags to be combined, like "bu" for
	    // bold-underlined.
	    attr = 0;
	    id = 0;
	    বাবে ( ; *p && *p != ','; ++p)	    // parse up to comma
	    {
		যদি (VIM_ISWHITE(*p))		    // ignore white space
		    অনুৱর্ত;

		যদি (attr > HL_ALL)  // Combination with ':' is not allowed.
		    ঘূৰাই FAIL;

		// Note: Keep this in sync with expand_set_highlight().
		ৱিকল্প (*p)
		{
		    পক্ষ 'b':	attr |= HL_BOLD;
				ভঙ্গ;
		    পক্ষ 'i':	attr |= HL_ITALIC;
				ভঙ্গ;
		    পক্ষ '-':
		    পক্ষ 'n':			    // no highlighting
				ভঙ্গ;
		    পক্ষ 'r':	attr |= HL_INVERSE;
				ভঙ্গ;
		    পক্ষ 's':	attr |= HL_STANDOUT;
				ভঙ্গ;
		    পক্ষ 'u':	attr |= HL_UNDERLINE;
				ভঙ্গ;
		    পক্ষ 'c':	attr |= HL_UNDERCURL;
				ভঙ্গ;
		    পক্ষ '2':	attr |= HL_UNDERDOUBLE;
				ভঙ্গ;
		    পক্ষ 'd':	attr |= HL_UNDERDOTTED;
				ভঙ্গ;
		    পক্ষ '=':	attr |= HL_UNDERDASHED;
				ভঙ্গ;
		    পক্ষ 't':	attr |= HL_STRIKETHROUGH;
				ভঙ্গ;
		    পক্ষ ':':	++p;		    // highlight group name
				যদি (attr || *p == NUL)	 // no combinations
				    ঘূৰাই FAIL;
				end = vim_strchr(p, ',');
				যদি (end == রিক্ত)
				    end = p + STRLEN(p);
				id = syn_check_group(p, (পূর্ণাঙ্ক)(end - p));
				যদি (id == 0)
				    ঘূৰাই FAIL;
				attr = syn_id2attr(id);
				p = end - 1;
#যদি defined(FEAT_STL_OPT) && defined(USER_HIGHLIGHT)
				যদি (hlf == (পূর্ণাঙ্ক)HLF_SNC)
				    id_SNC = syn_get_final_id(id);
# ifdef FEAT_TERMINAL
				অন্যথা যদি (hlf == (পূর্ণাঙ্ক)HLF_ST)
				    id_ST = syn_get_final_id(id);
				অন্যথা যদি (hlf == (পূর্ণাঙ্ক)HLF_STNC)
				    id_STNC = syn_get_final_id(id);
# endif
				অন্যথা যদি (hlf == (পূর্ণাঙ্ক)HLF_S)
				    id_S = syn_get_final_id(id);
#endif
				ভঙ্গ;
		    স্ৱভাৱ:	ঘূৰাই FAIL;
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
	ৱর্ণ *errmsg = update_winhighlight(wp, wp->w_p_whl);

	যদি (errmsg != রিক্ত)
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
    যদি (ga_grow(&highlight_ga, 28) == FAIL)
	ঘূৰাই FAIL;
    hlcnt = highlight_ga.ga_len;
    যদি (id_S == -1)
    {
	// Make sure id_S is always valid to simplify code below. Use the last
	// entry.
	CLEAR_POINTER(&HL_TABLE()[hlcnt + 27]);
	HL_TABLE()[hlcnt + 18].sg_term = highlight_attr[HLF_S];
	id_S = hlcnt + 19;
    }
# endif
    বাবে (i = 0; i < 9; i++)
    {
	sprintf((ৱর্ণ *)userhl, "User%d", i + 1);
	id = syn_name2id(userhl);
	যদি (id == 0)
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
	অন্যথা
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

    ঘূৰাই OK;
}

স্থির শূন্য highlight_list(শূন্য);
স্থির শূন্য highlight_list_two(পূর্ণাঙ্ক cnt, পূর্ণাঙ্ক attr);

// context for :highlight <group> <arg> expansion
স্থির পূর্ণাঙ্ক expand_hi_synid = 0;	    // ID for highlight group being completed
স্থির পূর্ণাঙ্ক expand_hi_equal_col = 0; // column where the '=' is
স্থির পূর্ণাঙ্ক expand_hi_include_orig = 0;	    // whether to fill the existing current value or not
স্থির char_u *expand_hi_curvalue = রিক্ত;   // the existing current value
#যদি defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
স্থির dict_iterator_T expand_colornames_iter;	// iterator for looping through v:colornames
#endif

/*
 * Handle command line completion for :highlight command.
 */
    শূন্য
set_context_in_highlight_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;
    পূর্ণাঙ্ক		expand_group = TRUE;

    // Default: expand group names
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 0;
    include_link = 2;
    include_default = 1;

    যদি (*arg == NUL)
	ঘূৰাই;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    যদি (*p == NUL)
	ঘূৰাই;

    // past "default" or group name
    include_default = 0;
    যদি (STRNCMP("default", arg, p - arg) == 0)
    {
	arg = skipwhite(p);
	xp->xp_pattern = arg;
	p = skiptowhite(arg);
    }
    যদি (*p == NUL)
	ঘূৰাই;

    // past group name
    include_link = 0;
    যদি (arg[1] == 'i' && arg[0] == 'N')
    {
	highlight_list();
	expand_group = FALSE;
    }
    যদি (STRNCMP("link", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	যদি (*p != NUL)		// past first group name
	{
	    xp->xp_pattern = skipwhite(p);
	    p = skiptowhite(xp->xp_pattern);
	    include_none = 1;
	}
	expand_group = FALSE;
    }
    অন্যথা যদি (STRNCMP("clear", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	expand_group = FALSE;
    }
    যদি (*p != NUL)			// past group name(s)
    {
	যদি (expand_group)
	{
	    // expansion will be done in expand_highlight_group()
	    xp->xp_context = EXPAND_HIGHLIGHT_GROUP;

	    expand_hi_synid = syn_namen2id(arg, (পূর্ণাঙ্ক)(p - arg));

	    যেতিয়ালৈ (*p != NUL)
	    {
		arg = skipwhite(p);
		p = skiptowhite(arg);
	    }

	    p = vim_strchr(arg, '=');
	    যদি (p == রিক্ত)
	    {
		// Didn't find a key=<value> pattern
		xp->xp_pattern = arg;
		expand_hi_equal_col = -1;
		expand_hi_include_orig = FALSE;
	    }
	    অন্যথা
	    {
		// Found key=<value> pattern, record the exact location
		expand_hi_equal_col = (পূর্ণাঙ্ক)(p - xp->xp_সাৰি);

		// Only include the original value if the pattern is empty
		যদি (*(p + 1) == NUL)
		    expand_hi_include_orig = TRUE;
		অন্যথা
		    expand_hi_include_orig = FALSE;

		// Account for comma-separated values
		যদি (STRNCMP(arg, "term=", 5) == 0 ||
			STRNCMP(arg, "cterm=", 6) == 0 ||
			STRNCMP(arg, "gui=", 4) == 0)
		{
		    char_u *comma = vim_strrchr(p + 1, ',');
		    যদি (comma != রিক্ত)
			p = comma;
		}
		xp->xp_pattern = p + 1;
	    }
	}
	অন্যথা
	{
	    xp->xp_context = EXPAND_NOTHING;
	}
    }
}

/*
 * List highlighting matches in a nice way.
 */
    স্থির শূন্য
highlight_list(শূন্য)
{
    পূর্ণাঙ্ক		i;

    বাবে (i = 10; --i >= 0; )
	highlight_list_two(i, HL_ATTR(HLF_D));
    বাবে (i = 40; --i >= 0; )
	highlight_list_two(99, 0);
}

    স্থির শূন্য
highlight_list_two(পূর্ণাঙ্ক cnt, পূর্ণাঙ্ক attr)
{
    বাৰ্তা_puts_attr(&("N \bI \b!  \b"[cnt / 11]), attr);
    বাৰ্তা_clr_eos();
    out_flush();
    ui_delay(cnt == 99 ? 40L : (long)cnt * 50L, FALSE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_highlight_name(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    ঘূৰাই get_highlight_name_ext(xp, idx, TRUE);
}

/*
 * Obtain a highlight group name.
 * When "skip_cleared" is TRUE don't return a cleared entry.
 */
    char_u *
get_highlight_name_ext(expand_T *xp UNUSED, পূর্ণাঙ্ক idx, পূর্ণাঙ্ক skip_cleared)
{
    যদি (idx < 0)
	ঘূৰাই রিক্ত;

    // Items are never removed from the table, skip the ones that were
    // cleared.
    যদি (skip_cleared && idx < highlight_ga.ga_len && HL_TABLE()[idx].sg_cleared)
	ঘূৰাই (char_u *)"";

    যদি (idx == highlight_ga.ga_len && include_none != 0)
	ঘূৰাই (char_u *)"NONE";
    যদি (idx == highlight_ga.ga_len + include_none && include_default != 0)
	ঘূৰাই (char_u *)"default";
    যদি (idx == highlight_ga.ga_len + include_none + include_default
							 && include_link != 0)
	ঘূৰাই (char_u *)"link";
    যদি (idx == highlight_ga.ga_len + include_none + include_default + 1
							 && include_link != 0)
	ঘূৰাই (char_u *)"clear";
    যদি (idx >= highlight_ga.ga_len)
	ঘূৰাই রিক্ত;
    ঘূৰাই HL_TABLE()[idx].sg_name;
}

    স্থির char_u *
get_highlight_attr_name(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx == 0)
    {
	// Fill with current value first
	যদি (expand_hi_curvalue != রিক্ত)
	    ঘূৰাই expand_hi_curvalue;
	অন্যথা
	    ঘূৰাই (char_u*)"";
    }
    যদি (idx < (পূর্ণাঙ্ক)ARRAY_LENGTH(highlight_index_tab) + 1)
    {
	char_u *মান = highlight_index_tab[idx-1]->মান.string;
	যদি (expand_hi_curvalue != রিক্ত && STRCMP(expand_hi_curvalue, মান) == 0)
	{
	    // Already returned the current value above, just skip.
	    ঘূৰাই (char_u*)"";
	}
	ঘূৰাই মান;
    }
    ঘূৰাই রিক্ত;
}

    স্থির char_u *
get_highlight_cterm_color(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx == 0)
    {
	// Fill with current value first
	যদি (expand_hi_curvalue != রিক্ত)
	    ঘূৰাই expand_hi_curvalue;
	অন্যথা
	    ঘূৰাই (char_u*)"";
    }
    // See highlight_set_cterm_color()
    অন্যথা যদি (idx == 1)
	ঘূৰাই (char_u*)"fg";
    অন্যথা যদি (idx == 2)
	ঘূৰাই (char_u*)"bg";
    যদি (idx < (পূর্ণাঙ্ক)ARRAY_LENGTH(color_name_tab) + 3)
    {
	char_u *মান = color_name_tab[idx-3].মান.string;
	ঘূৰাই মান;
    }
    ঘূৰাই রিক্ত;
}

#যদি defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
    স্থির char_u *
get_highlight_gui_color(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx == 0)
    {
	// Fill with current value first
	যদি (expand_hi_curvalue != রিক্ত)
	    ঘূৰাই expand_hi_curvalue;
	অন্যথা
	    ঘূৰাই (char_u*)"";
    }
    // See color_name2handle()
    অন্যথা যদি (idx == 1)
	ঘূৰাই (char_u*)"fg";
    অন্যথা যদি (idx == 2)
	ঘূৰাই (char_u*)"bg";
    অন্যথা যদি (idx == 3)
	ঘূৰাই (char_u*)"NONE";

    // Complete from v:colornames. Don't do platform specific names for now.
    typval_T *tv_result;
    char_u *colorname = dict_iterate_next(&expand_colornames_iter, &tv_result);
    যদি (colorname != রিক্ত)
    {
	// :hi command doesn't allow space, so don't suggest any malformed items
	যদি (vim_strchr(colorname, ' ') != রিক্ত)
	    ঘূৰাই (char_u*)"";

	যদি (expand_hi_curvalue != রিক্ত && STRICMP(expand_hi_curvalue, colorname) == 0)
	{
	    // Already returned the current value above, just skip.
	    ঘূৰাই (char_u*)"";
	}
    }
    ঘূৰাই colorname;
}
#endif

    স্থির char_u *
get_highlight_group_চাবি(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    // Note: Keep this in sync with do_highlight.
    স্থির ৱর্ণ *(p_hi_group_চাবি_মানসমূহ[]) =
    {
	"term=",
	"start=",
	"stop=",
	"cterm=",
	"ctermfg=",
	"ctermbg=",
	"ctermul=",
	"ctermfont=",
#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

    যদি (idx < (পূর্ণাঙ্ক)ARRAY_LENGTH(p_hi_group_চাবি_মানসমূহ))
	ঘূৰাই (char_u*)p_hi_group_চাবি_মানসমূহ[idx];
    ঘূৰাই রিক্ত;
}

/*
 * Command-line expansion for :hi {group-name} <args>...
 */
    পূর্ণাঙ্ক
expand_highlight_group(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	পূর্ণাঙ্ক	    *অঙ্ক_Matches)
{
    যদি (expand_hi_equal_col != -1)
    {
	// List the values. First fill in the current value, then if possible colors
	// or attribute names.
	char_u	    *(*expandfunc)(expand_T *, পূর্ণাঙ্ক) = রিক্ত;
	পূর্ণাঙ্ক	    type = 0;
	hl_group_T  *sgp = রিক্ত;
	পূর্ণাঙ্ক	    iarg = 0;
	char_u	    *sarg = রিক্ত;

	পূর্ণাঙ্ক	    unsorted_বস্তুসমূহ = -1; // don't sort by default

	যদি (expand_hi_synid != 0)
	    sgp = &HL_TABLE()[expand_hi_synid - 1]; // index is ID minus one

	// Note: Keep this in sync with highlight_list_one().
	char_u	    *name_end = xp->xp_সাৰি + expand_hi_equal_col;
	যদি (name_end - xp->xp_সাৰি >= 5
		&& STRNCMP(name_end - 5, " term", 5) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    যদি (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_term;
	    }
	}
	অন্যথা যদি (name_end - xp->xp_সাৰি >= 6
		&& STRNCMP(name_end - 6, " cterm", 6) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    যদি (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_cterm;
	    }
	}
#যদি defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	অন্যথা যদি (name_end - xp->xp_সাৰি >= 4
		&& STRNCMP(name_end - 4, " gui", 4) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    যদি (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_gui;
	    }
	}
#endif
	অন্যথা যদি (name_end - xp->xp_সাৰি >= 8
		&& STRNCMP(name_end - 8, " ctermfg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    যদি (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_fg;
	    }
	}
	অন্যথা যদি (name_end - xp->xp_সাৰি >= 8
		&& STRNCMP(name_end - 8, " ctermbg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    যদি (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_bg;
	    }
	}
	অন্যথা যদি (name_end - xp->xp_সাৰি >= 8
		&& STRNCMP(name_end - 8, " ctermul", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    যদি (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_ul;
	    }
	}
#যদি defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	অন্যথা যদি (name_end - xp->xp_সাৰি >= 6
		&& STRNCMP(name_end - 6, " guifg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    যদি (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_fg_name;
	    }
	}
	অন্যথা যদি (name_end - xp->xp_সাৰি >= 6
		&& STRNCMP(name_end - 6, " guibg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    যদি (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_bg_name;
	    }
	}
	অন্যথা যদি (name_end - xp->xp_সাৰি >= 6
		&& STRNCMP(name_end - 6, " guisp", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    যদি (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_sp_name;
	    }
	}
#endif

#যদি defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	যদি (expandfunc == get_highlight_gui_color)
	{
	    // Top 4 items are special, after that sort all the color names
	    unsorted_বস্তুসমূহ = 4;

	    dict_T *colornames_table = get_vim_var_dict(VV_COLORNAMES);
	    typval_T colornames_val;
	    colornames_val.v_type = VAR_DICT;
	    colornames_val.vval.v_dict = colornames_table;
	    dict_iterate_start(&colornames_val, &expand_colornames_iter);
	}
#endif

	char_u	    buf[MAX_ATTR_LEN];

	expand_hi_curvalue = রিক্ত;
	যদি (expand_hi_include_orig)
	{
	    যদি (((type == LIST_ATTR || type == LIST_INT) && iarg != 0) ||
		(type == LIST_STRING && sarg != রিক্ত))
	    {
		// Retrieve the current value to go first in completion
		expand_hi_curvalue = highlight_arg_to_string(
			type, iarg, sarg, buf);
	    }
	}

	যদি (expandfunc != রিক্ত)
	{
	    ঘূৰাই ExpandGenericExt(
		    pat,
		    xp,
		    rmp,
		    matches,
		    অঙ্ক_Matches,
		    expandfunc,
		    FALSE,
		    unsorted_বস্তুসমূহ);
	}

	ঘূৰাই FAIL;
    }

    // List all the key names
    ঘূৰাই ExpandGenericExt(
	    pat,
	    xp,
	    rmp,
	    matches,
	    অঙ্ক_Matches,
	    get_highlight_group_চাবি,
	    FALSE,
	    -1);
}

#যদি defined(FEAT_GUI)
/*
 * Free all the highlight group fonts.
 * Used when quitting for systems which need it.
 */
    শূন্য
free_highlight_fonts(শূন্য)
{
    পূর্ণাঙ্ক	    idx;

    বাবে (idx = 0; idx < highlight_ga.ga_len; ++idx)
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

#যদি defined(FEAT_EVAL)
/*
 * Convert each of the highlight attribute bits (bold, standout, underline,
 * etc.) set in 'hlattr' into a separate boolean item in a Dictionary with
 * the attribute name as the key.
 */
    স্থির dict_T *
highlight_get_attr_dict(পূর্ণাঙ্ক hlattr)
{
    dict_T	*dict;
    পূর্ণাঙ্ক		i;

    dict = dict_alloc();
    যদি (dict == রিক্ত)
	ঘূৰাই রিক্ত;

    বাবে (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(highlight_index_tab); ++i)
    {
	যদি (hlattr & highlight_index_tab[i]->চাবি)
	{
	    dict_add_bool(dict, (ৱর্ণ *)highlight_index_tab[i]->মান.string,
		    VVAL_TRUE);
	    // don't want "inverse"/"reverse"
	    hlattr &= ~highlight_index_tab[i]->চাবি;
	}
    }

    ঘূৰাই dict;
}

/*
 * Return the attributes of the highlight group at index 'hl_idx' as a
 * Dictionary. If 'resolve_link' is TRUE, then resolves the highlight group
 * links recursively.
 */
    স্থির dict_T *
highlight_get_info(পূর্ণাঙ্ক hl_idx, পূর্ণাঙ্ক resolve_link)
{
    dict_T	*dict;
    hl_group_T	*sgp;
    dict_T	*attr_dict = রিক্ত;
    পূর্ণাঙ্ক		hlgid;

    dict = dict_alloc();
    যদি (dict == রিক্ত)
	ঘূৰাই dict;

    sgp = &HL_TABLE()[hl_idx];
    // highlight group id is 1-based
    hlgid = hl_idx + 1;

    যদি (dict_add_string(dict, "name", sgp->sg_name) == FAIL)
	গচ্ছ error;
    যদি (dict_add_সংখ্যা(dict, "id", hlgid) == FAIL)
	গচ্ছ error;

    যদি (resolve_link)
    {
	// resolve the highlight group link recursively
	যেতিয়ালৈ (sgp->sg_link)
	{
	    hlgid = sgp->sg_link;
	    sgp = &HL_TABLE()[sgp->sg_link - 1];
	}
    }

    যদি (sgp->sg_term != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_term);
	যদি (attr_dict != রিক্ত)
	{
	    যদি (dict_add_dict(dict, "term", attr_dict) == FAIL)
		গচ্ছ error;
	    attr_dict = রিক্ত;
	}
    }
    যদি (sgp->sg_start != রিক্ত)
	যদি (dict_add_string(dict, "start", sgp->sg_start) == FAIL)
	    গচ্ছ error;
    যদি (sgp->sg_stop != রিক্ত)
	যদি (dict_add_string(dict, "stop", sgp->sg_stop) == FAIL)
	    গচ্ছ error;
    যদি (sgp->sg_cterm != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_cterm);
	যদি (attr_dict != রিক্ত)
	{
	    যদি (dict_add_dict(dict, "cterm", attr_dict) == FAIL)
		গচ্ছ error;
	    attr_dict = রিক্ত;
	}
    }
    যদি (sgp->sg_cterm_fg != 0)
	যদি (dict_add_string(dict, "ctermfg",
		    highlight_color(hlgid, (char_u *)"fg", 'c')) == FAIL)
	    গচ্ছ error;
    যদি (sgp->sg_cterm_bg != 0)
	যদি (dict_add_string(dict, "ctermbg",
			highlight_color(hlgid, (char_u *)"bg", 'c')) == FAIL)
	    গচ্ছ error;
    যদি (sgp->sg_cterm_ul != 0)
	যদি (dict_add_string(dict, "ctermul",
			highlight_color(hlgid, (char_u *)"ul", 'c')) == FAIL)
	    গচ্ছ error;
    যদি (sgp->sg_cterm_font != 0)
	যদি (dict_add_string(dict, "ctermfont",
			highlight_color(hlgid, (char_u *)"font", 'c')) == FAIL)
	    গচ্ছ error;
    যদি (sgp->sg_gui != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_gui);
	যদি (attr_dict != রিক্ত)
	{
	    যদি (dict_add_dict(dict, "gui", attr_dict) == FAIL)
		গচ্ছ error;
	    attr_dict = রিক্ত;
	}
    }
    যদি (sgp->sg_gui_fg_name != রিক্ত)
	যদি (dict_add_string(dict, "guifg",
			highlight_color(hlgid, (char_u *)"fg", 'g')) == FAIL)
	    গচ্ছ error;
    যদি (sgp->sg_gui_bg_name != রিক্ত)
	যদি (dict_add_string(dict, "guibg",
			highlight_color(hlgid, (char_u *)"bg", 'g')) == FAIL)
	    গচ্ছ error;
    যদি (sgp->sg_gui_sp_name != রিক্ত)
	যদি (dict_add_string(dict, "guisp",
			highlight_color(hlgid, (char_u *)"sp", 'g')) == FAIL)
	    গচ্ছ error;
# ifdef FEAT_GUI
    যদি (sgp->sg_font_name != রিক্ত)
	যদি (dict_add_string(dict, "font", sgp->sg_font_name) == FAIL)
	    গচ্ছ error;
# endif
    যদি (sgp->sg_link)
    {
	char_u	*link;

	link = HL_TABLE()[sgp->sg_link - 1].sg_name;
	যদি (link != রিক্ত && dict_add_string(dict, "linksto", link) == FAIL)
	    গচ্ছ error;

	যদি (sgp->sg_deflink)
	    dict_add_bool(dict, "default", VVAL_TRUE);
    }
    যদি (dict_len(dict) == 2)
	// If only 'name' is present, then the highlight group is cleared.
	dict_add_bool(dict, "cleared", VVAL_TRUE);

    ঘূৰাই dict;

error:
    dict_unref(attr_dict);
    dict_unref(dict);
    ঘূৰাই রিক্ত;
}

/*
 * "hlget([name])" function
 * Return the attributes of a specific highlight group (if specified) or all
 * the highlight groups.
 */
    শূন্য
f_hlget(typval_T *argvars, typval_T *rettv)
{
    list_T	*list;
    dict_T	*dict;
    পূর্ণাঙ্ক		i;
    char_u	*hlarg = রিক্ত;
    পূর্ণাঙ্ক		resolve_link = FALSE;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ঘূৰাই;

    যদি (check_for_opt_string_arg(argvars, 0) == FAIL
	    || (argvars[0].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	// highlight group name supplied
	hlarg = tv_get_string_chk(&argvars[0]);
	যদি (hlarg == রিক্ত)
	    ঘূৰাই;

	যদি (argvars[1].v_type != VAR_UNKNOWN)
	{
	    পূর্ণাঙ্ক error = FALSE;

	    resolve_link = tv_get_bool_chk(&argvars[1], &error);
	    যদি (error)
		ঘূৰাই;
	}
    }

    list = rettv->vval.v_list;
    বাবে (i = 0; i < highlight_ga.ga_len && !got_int; ++i)
    {
	যদি (hlarg == রিক্ত || STRICMP(hlarg, HL_TABLE()[i].sg_name) == 0)
	{
	    dict = highlight_get_info(i, resolve_link);
	    যদি (dict != রিক্ত && list_append_dict(list, dict) == FAIL)
		dict_unref(dict);
	}
    }
}

/*
 * Returns the string value at 'dict[key]'. Returns NULL, if 'key' is not in
 * 'dict' or the value is not a string type. If the value is not a string type
 * or is NULL, then 'error' is set to TRUE.
 */
    স্থির char_u *
hldict_get_string(dict_T *dict, char_u *চাবি, পূর্ণাঙ্ক *error)
{
    dictitem_T	*di;

    *error = FALSE;
    di = dict_find(dict, চাবি, -1);
    যদি (di == রিক্ত)
	ঘূৰাই রিক্ত;

    যদি (di->di_tv.v_type != VAR_STRING || di->di_tv.vval.v_string == রিক্ত)
    {
	emsg(_(e_string_required));
	*error = TRUE;
	ঘূৰাই রিক্ত;
    }

    ঘূৰাই di->di_tv.vval.v_string;
}

/*
 * Convert the highlight attribute Dictionary at 'dict[key]' into a string
 * value in 'attr_str' of length 'len'. Returns FALSE if 'dict[key]' is not a
 * Dictionary or is NULL.
 */
    স্থির পূর্ণাঙ্ক
hldict_attr_to_str(
	dict_T	*dict,
	char_u	*চাবি,
	char_u	*attr_str,
	size_t	len)
{
    dictitem_T	*di;
    dict_T	*attrdict;
    পূর্ণাঙ্ক		i;
    char_u	*p;

    attr_str[0] = NUL;
    di = dict_find(dict, চাবি, -1);
    যদি (di == রিক্ত)
	ঘূৰাই TRUE;

    যদি (di->di_tv.v_type != VAR_DICT || di->di_tv.vval.v_dict == রিক্ত)
    {
	emsg(_(e_dictionary_required));
	ঘূৰাই FALSE;
    }

    attrdict = di->di_tv.vval.v_dict;

    // If the attribute dict is empty, then return NONE to clear the attributes
    যদি (dict_len(attrdict) == 0)
    {
	vim_strcat(attr_str, (char_u *)"NONE", len);
	ঘূৰাই TRUE;
    }

    p = attr_str;
    বাবে (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(highlight_tab); ++i)
    {
	যদি (dict_get_bool(attrdict, (ৱর্ণ *)highlight_tab[i].মান.string,
		    VVAL_FALSE) == VVAL_TRUE)
	{
	    যদি (p != attr_str && (size_t)(p - attr_str + 2) < len)
		STRCPY(p, (char_u *)",");
	    যদি (p - attr_str + highlight_tab[i].মান.length + 1 < len)
	    {
		STRCPY(p, highlight_tab[i].মান.string);
		p += highlight_tab[i].মান.length;
	    }
	}
    }

    ঘূৰাই TRUE;
}

// Temporary buffer used to store the command string produced by hlset().
// IObuff cannot be used for this as the error messages produced by hlset()
// internally use IObuff.
# define	HLSETBUFSZ  512
স্থির char_u hlsetBuf[HLSETBUFSZ + 1];

/*
 * Add the highlight attribute "attr" of length "attrlen" and "value" at
 * "dptr", which points into "hlsetBuf".
 * Returns the updated pointer.
 */
    স্থির char_u *
add_attr_and_মান(char_u *dptr, char_u *attr, পূর্ণাঙ্ক attrlen, char_u *মান)
{
    size_t	vallen;

    // Do nothing if the value is not specified or is empty
    যদি (মান == রিক্ত || *মান == NUL)
	ঘূৰাই dptr;

    vallen = STRLEN(মান);
    // When the value contains a space and the attribute has an "=" (i.e. it
    // is a key=value pair), surround the value with single quotes so that
    // do_highlight() can parse it correctly.
    যদি (vim_strchr(মান, ' ') != রিক্ত && vim_strchr(attr, '=') != রিক্ত)
    {
	যদি (dptr + attrlen + vallen + 3 < hlsetBuf + HLSETBUFSZ)
	{
	    STRCPY(dptr, attr);
	    dptr += attrlen;
	    *dptr++ = '\'';
	    STRCPY(dptr, মান);
	    dptr += vallen;
	    *dptr++ = '\'';
	    *dptr = NUL;
	}
    }
    অন্যথা যদি (dptr + attrlen + vallen + 1 < hlsetBuf + HLSETBUFSZ)
    {
	STRCPY(dptr, attr);
	dptr += attrlen;
	STRCPY(dptr, মান);
	dptr += vallen;
    }

    ঘূৰাই dptr;
}

/*
 * Add or update a highlight group using 'dict' items. Returns TRUE if
 * successfully updated the highlight group.
 */
    স্থির পূর্ণাঙ্ক
hlg_add_or_update(dict_T *dict)
{
    char_u	*name;
    পূর্ণাঙ্ক		error;
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
    পূর্ণাঙ্ক		forceit = FALSE;
    পূর্ণাঙ্ক		dodefault = FALSE;
    পূর্ণাঙ্ক		done = FALSE;
    char_u	*p;

    name = hldict_get_string(dict, (char_u *)"name", &error);
    যদি (name == রিক্ত || *name == NUL || error)
	ঘূৰাই FALSE;

    যদি (dict_get_bool(dict, "force", VVAL_FALSE) == VVAL_TRUE)
	forceit = TRUE;

    যদি (dict_get_bool(dict, "default", VVAL_FALSE) == VVAL_TRUE)
	dodefault = TRUE;

    যদি (dict_has_চাবি(dict, "cleared"))
    {
	varnumber_T	cleared;

	// clear a highlight group
	cleared = dict_get_bool(dict, "cleared", FALSE);
	যদি (cleared == TRUE)
	{
	    vim_snprintf((ৱর্ণ *)hlsetBuf, HLSETBUFSZ, "clear %s", name);
	    do_highlight(hlsetBuf, forceit, FALSE);
	    done = TRUE;
	}
    }

    যদি (dict_has_চাবি(dict, "linksto"))
    {
	char_u	*linksto;

	// link highlight groups
	linksto = hldict_get_string(dict, (char_u *)"linksto", &error);
	যদি (linksto == রিক্ত || *linksto == NUL || error)
	    ঘূৰাই FALSE;

	vim_snprintf((ৱর্ণ *)hlsetBuf, HLSETBUFSZ, "%slink %s %s",
				dodefault ? "default " : "", name, linksto);
	do_highlight(hlsetBuf, forceit, FALSE);

	done = TRUE;
    }

    // If 'cleared' or 'linksto' are specified, then don't process the other
    // attributes.
    যদি (done)
	ঘূৰাই TRUE;

    start = hldict_get_string(dict, (char_u *)"start", &error);
    যদি (error)
	ঘূৰাই FALSE;

    stop = hldict_get_string(dict, (char_u *)"stop", &error);
    যদি (error)
	ঘূৰাই FALSE;

    যদি (!hldict_attr_to_str(dict, (char_u *)"term", term_attr,
							    পরিমাণ(term_attr)))
	ঘূৰাই FALSE;

    যদি (!hldict_attr_to_str(dict, (char_u *)"cterm", cterm_attr,
							   পরিমাণ(cterm_attr)))
	ঘূৰাই FALSE;

    ctermfg = hldict_get_string(dict, (char_u *)"ctermfg", &error);
    যদি (error)
	ঘূৰাই FALSE;

    ctermbg = hldict_get_string(dict, (char_u *)"ctermbg", &error);
    যদি (error)
	ঘূৰাই FALSE;

    ctermul = hldict_get_string(dict, (char_u *)"ctermul", &error);
    যদি (error)
	ঘূৰাই FALSE;

    ctermfont = hldict_get_string(dict, (char_u *)"ctermfont", &error);
    যদি (error)
	ঘূৰাই FALSE;

    যদি (!hldict_attr_to_str(dict, (char_u *)"gui", gui_attr, পরিমাণ(gui_attr)))
	ঘূৰাই FALSE;

    guifg = hldict_get_string(dict, (char_u *)"guifg", &error);
    যদি (error)
	ঘূৰাই FALSE;

    guibg = hldict_get_string(dict, (char_u *)"guibg", &error);
    যদি (error)
	ঘূৰাই FALSE;

    guisp = hldict_get_string(dict, (char_u *)"guisp", &error);
    যদি (error)
	ঘূৰাই FALSE;

# ifdef FEAT_GUI
    font = hldict_get_string(dict, (char_u *)"font", &error);
    যদি (error)
	ঘূৰাই FALSE;
# endif

    // If none of the attributes are specified, then do nothing.
    যদি (term_attr[0] == NUL && start == রিক্ত && stop == রিক্ত
	    && cterm_attr[0] == NUL && ctermfg == রিক্ত && ctermbg == রিক্ত
	    && ctermul == রিক্ত && ctermfont == রিক্ত && gui_attr[0] == NUL
# ifdef FEAT_GUI
	    && font == রিক্ত
# endif
	    && guifg == রিক্ত && guibg == রিক্ত && guisp == রিক্ত
	    )
	ঘূৰাই TRUE;

    hlsetBuf[0] = NUL;
    p = hlsetBuf;
    যদি (dodefault)
	p = add_attr_and_মান(p, (char_u *)"default", 7, (char_u *)" ");
    p = add_attr_and_মান(p, (char_u *)"", 0, name);
    p = add_attr_and_মান(p, (char_u *)" term=", 6, term_attr);
    p = add_attr_and_মান(p, (char_u *)" start=", 7, start);
    p = add_attr_and_মান(p, (char_u *)" stop=", 6, stop);
    p = add_attr_and_মান(p, (char_u *)" cterm=", 7, cterm_attr);
    p = add_attr_and_মান(p, (char_u *)" ctermfg=", 9, ctermfg);
    p = add_attr_and_মান(p, (char_u *)" ctermbg=", 9, ctermbg);
    p = add_attr_and_মান(p, (char_u *)" ctermul=", 9, ctermul);
    p = add_attr_and_মান(p, (char_u *)" ctermfont=", 11, ctermfont);
    p = add_attr_and_মান(p, (char_u *)" gui=", 5, gui_attr);
# ifdef FEAT_GUI
    p = add_attr_and_মান(p, (char_u *)" font=", 6, font);
# endif
    p = add_attr_and_মান(p, (char_u *)" guifg=", 7, guifg);
    p = add_attr_and_মান(p, (char_u *)" guibg=", 7, guibg);
    (শূন্য)add_attr_and_মান(p, (char_u *)" guisp=", 7, guisp);

    do_highlight(hlsetBuf, forceit, FALSE);

    ঘূৰাই TRUE;
}

/*
 * "hlset([{highlight_attr}])" function
 * Add or modify highlight groups
 */
    শূন্য
f_hlset(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    dict_T	*dict;

    rettv->vval.v_সংখ্যা = -1;

    যদি (check_for_list_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    FOR_ALL_LIST_বস্তুসমূহ(argvars->vval.v_list, li)
    {
	যদি (li->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_dictionary_required));
	    ঘূৰাই;
	}

	dict = li->li_tv.vval.v_dict;
	যদি (!hlg_add_or_update(dict))
	    ঘূৰাই;
    }

    rettv->vval.v_সংখ্যা = 0;
}
#endif

/*
 * If "old" is in the override stack, then update it to "hl_new". Does not free
 * "old".
 */
    শূন্য
update_highlight_overrides(hl_override_T *old, hl_override_T *hl_new, পূর্ণাঙ্ক newlen)
{
    যদি (old == রিক্ত)
	ঘূৰাই;

    বাবে (hl_overrides_T *set = overrides; set != রিক্ত; set = set->next)
    {
	যদি (set->arr == old)
	{
	    set->arr = hl_new;
	    set->len = newlen;
	}
    }
}

/*
 * If "arr" is in the highlight overrides list, then mark it as invalid.
 */
    শূন্য
remove_highlight_overrides(hl_override_T *arr)
{
    যদি (arr == রিক্ত || overrides == রিক্ত)
	ঘূৰাই;

    বাবে (hl_overrides_T *set = overrides; set != রিক্ত; set = set->next)
    {
	যদি (set->arr == arr)
	{
	    set->arr = রিক্ত;
	    ভঙ্গ;
	}
    }
}

/*
 * Update highlight_attr[] array. If "update_ids" is true, then update
 * highlight_ids[] array instead.
 */
    স্থির শূন্য
set_highlight_attr(hl_override_T *arr, পূর্ণাঙ্ক len, bool update_ids)
{
    বাবে (পূর্ণাঙ্ক i = 0; i < len; i++)
    {
	hl_override_T	*override = arr + i;
	পূর্ণাঙ্ক		hlf = -1, attr;

	যদি (override->from <= 0)
	    // Directly map to highlight_attr
	    hlf = -override->from;
	অন্যথা
	{
	    বাবে (পূর্ণাঙ্ক k = 0; k < HLF_গুনতি; k++)
		যদি (override->from == highlight_ids[k])
		{
		    hlf = k;
		    ভঙ্গ;
		}

	    যদি (hlf == -1)
		অনুৱর্ত;
	}

	যদি (update_ids)
	{
	    যদি (override->to <= 0)
	    {
		যদি (hlf == -override->to)
		    highlight_ids[hlf] = -1;
		অন্যথা
		    highlight_ids[hlf] = highlight_ids[-override->to];
	    }
	    অন্যথা
		highlight_ids[hlf] = override->to;
	}
	অন্যথা
	{
	    যদি (override->to <= 0)
		// Directly use highlight_attr[]
		attr = highlight_attr[-override->to];
	    অন্যথা
		attr = syn_id2attr(override->to);

	    highlight_attr[hlf] = attr;
	}
    }
}

/*
 * Set the current highlight override to "arr". Returns true if successful
 */
    bool
ঠেলো_highlight_overrides(hl_override_T *arr, পূর্ণাঙ্ক len)
{
    // Don't want to do anything if "arr" is already the current override. If
    // "arr" is NULL (but overrides->arr is not), then still push an override,
    // but "->arr" will just be NULL so any previous overrides are cleared.
    যদি (overrides != রিক্ত && overrides->arr == arr)
	ঘূৰাই false;

    hl_overrides_T *set;

    set = ALLOC_ONE(hl_overrides_T);
    যদি (set == রিক্ত)
	ঘূৰাই false;

    set->arr = arr;
    set->len = len;

    // Push to front of list
    set->next = overrides;
    overrides = set;

    // Save current highlight_attr[], so it can be restored later after this
    // override is popped.
    memcpy(set->attr, highlight_attr, পরিমাণ(highlight_attr));

    // Reset highlight_attr[] to default values, as we don't want changes from
    // other overrides.
    memcpy(highlight_attr, highlight_attr_raw, পরিমাণ(highlight_attr));

    // Update highlight_attr[] array
    যদি (arr != রিক্ত)
	set_highlight_attr(arr, len, false);

    ঘূৰাই true;
}

/*
 * Pop the current highlight override (if any) from the stack. This should
 * always be paired with a *successful* push.
 */
    শূন্য
বেৰ_কৰো_highlight_overrides(শূন্য)
{
    hl_overrides_T *set = overrides;

    যদি (overrides == রিক্ত)
	ঘূৰাই;

    overrides = set->next;

    // Set highlight_attr[] to state before override was pushed.
    memcpy(highlight_attr, set->attr, পরিমাণ(highlight_attr));
    vim_free(set);
}

/*
 * Parse the 'winhighlight' option and return array. Returns NULL on failure or
 * if empty option. If failure, then errmsg is set.
 */
    স্থির hl_override_T *
parse_winhighlight(char_u *opt, পূর্ণাঙ্ক *len, ৱর্ণ **errmsg)
{
    char_u	    *p = opt;
    hl_override_T   *arr;
    পূর্ণাঙ্ক		    i = 0;
    পূর্ণাঙ্ক		    অঙ্ক = 1;
    পূর্ণাঙ্ক		    n_colons = 0;

    যদি (*p == NUL)
	ঘূৰাই রিক্ত;

    // Get number of overrides first so we can allocate array
    যেতিয়ালৈ ((p = vim_strchr(p, ',')) != রিক্ত)
    {
	p++;
	অঙ্ক++;
    }
    p = opt;
    // Check if number of ':' matches number of ','
    যেতিয়ালৈ ((p = vim_strchr(p, ':')) != রিক্ত)
    {
	p++;
	n_colons++;
    }

    যদি (অঙ্ক != n_colons)
    {
	*errmsg = e_invalid_argument;
	ঘূৰাই রিক্ত;
    }

    arr = ALLOC_MULT(hl_override_T, অঙ্ক);
    যদি (arr == রিক্ত)
    {
	*errmsg = e_out_of_memory;
	ঘূৰাই রিক্ত;
    }

    p = opt;

    যেতিয়ালৈ (true)
    {
	hl_override_T *override = arr + i++;
	char_u	*fromname = p, *toname;
	char_u	*tmp;
	char_u	**names[2] = {&fromname, &toname};
	পূর্ণাঙ্ক	fromlen, tolen;
	পূর্ণাঙ্ক	*lens[2] = {&fromlen, &tolen};
	পূর্ণাঙ্ক	fromid, toid;
	পূর্ণাঙ্ক	*ids[2] = {&fromid, &toid};

	p = vim_strchr(p, ':');

	যদি (p == রিক্ত)
	    গচ্ছ fail;

	fromlen = p - fromname; // Get hl for "from"
	যদি (fromlen == 0)
	    গচ্ছ fail;
	p++; // Skip colon ':'
	যদি (*p == NUL)
	    গচ্ছ fail;

	toname = p;
	tmp = vim_strchr(p, ',');

	// Get hl for "to", must check for no trailing comma in case last
	// element.
	যদি (tmp == রিক্ত)
	    tolen = (পূর্ণাঙ্ক)STRLEN(p);
	অন্যথা
	{
	    tolen = tmp - toname;
	    p = ++tmp;
	}
	যদি (tolen == 0)
	    গচ্ছ fail;

	বাবে (পূর্ণাঙ্ক k = 0; k < 2; k++)
	{
	    char_u  *name = *names[k];
	    পূর্ণাঙ্ক	    nlen = *lens[k];

	    যদি (*name == '!')
	    {
		// If starts with "!", then it maps directly to a 'highlight'
		// occasion.
		পূর্ণাঙ্ক hlf;

		যদি (nlen != 2)
		    গচ্ছ fail;

		বাবে (hlf = 0; hlf < (পূর্ণাঙ্ক)HLF_গুনতি; ++hlf)
		    যদি (hl_flags[hlf] == name[1])
			ভঙ্গ;

		যদি (hlf >= HLF_গুনতি)
		    গচ্ছ fail;

		*ids[k] = -hlf;
	    }
	    অন্যথা
	    {
		// Otherwise get the highlight group id.
		যদি (syn_check_group(name, nlen) == 0)
		    গচ্ছ fail;

		*ids[k] = syn_namen2id(name, nlen);
		যদি (*ids[k] == 0)
		    গচ্ছ fail;

		// Always map "Normal" group to HLF_WIN
		যদি (ids[k] == &fromid &&
			STRCMP(HL_TABLE()[*ids[k] - 1].sg_name_u, "NORMAL") == 0)
		    *ids[k] = -HLF_WIN;
	    }
	}

	// If fromid == toid, leave it be, this is so that we know HLF_WIN has
	// been set (even to itself e.g. "Normal"), and therefore know to use
	// its attr instead.
	override->from = fromid;
	override->to = toid;

	যদি (tmp == রিক্ত)
	    ভঙ্গ;
    }

    *len = অঙ্ক;
    ঘূৰাই arr;
fail:
    vim_free(arr);
    *errmsg = e_invalid_argument;
    ঘূৰাই রিক্ত;
}

/*
 * Update w_hl for "wp" using given option value. Returns error
 * message on failure, otherwise NULL.
 */
    ৱর্ণ *
update_winhighlight(win_T *wp, char_u *opt)
{
    ৱর্ণ	    *err = রিক্ত;
    hl_override_T   *arr;
    পূর্ণাঙ্ক		    অঙ্ক = 1;

    arr = parse_winhighlight(opt, &অঙ্ক, &err);

    যদি (arr == রিক্ত && err != রিক্ত)
	ঘূৰাই err;

    যদি (arr == রিক্ত && wp->w_hl == রিক্ত)
	ঘূৰাই রিক্ত;

    update_highlight_overrides(wp->w_hl, arr, অঙ্ক);

    vim_free(wp->w_hl);
    wp->w_hl = arr;
    wp->w_hl_len = অঙ্ক;

    wp->w_hlfwin_id = hlf_get_id(wp, HLF_WIN);

#ifdef FEAT_TERMINAL
    term_update_hlfwin(wp);
#endif

#ifdef FEAT_TERMINAL
    // May be NULL (such as in after_copy_winopt())
    যদি (wp->w_buffer != রিক্ত)
    {
	// Make sure terminal highlighting is updated
	bool ret = ঠেলো_highlight_overrides(wp->w_hl, wp->w_hl_len);

	যদি (wp->w_buffer->b_term != রিক্ত)
	    term_init_default_colors(wp->w_buffer->b_term);
	যদি (ret)
	    বেৰ_কৰো_highlight_overrides();
    }
#endif

    ঘূৰাই রিক্ত;
}

/*
 * Get id that the hlf attr is using in window "wp", otherwise zero. Note that
 * -1 may be returned if the hlf was set to itself.
 */
    পূর্ণাঙ্ক
hlf_get_id(win_T *wp, পূর্ণাঙ্ক hlf)
{
    পূর্ণাঙ্ক id;
    স্থির পূর্ণাঙ্ক prev[HLF_গুনতি];

    যদি (wp->w_hl == রিক্ত)
	ঘূৰাই highlight_ids[hlf];

    memcpy(prev, highlight_ids, পরিমাণ(highlight_ids));
    set_highlight_attr(wp->w_hl, wp->w_hl_len, true);
    id = highlight_ids[hlf];
    memcpy(highlight_ids, prev, পরিমাণ(highlight_ids));

    ঘূৰাই id;
}

/*
 * Update 'winhighlight' using a 'wincolor' style option value. Returns error
 * message on failure otherwise NULL. Does not update the actual 'wincolor'
 * value, but does update the 'winhighlight' value.
 */
    ৱর্ণ *
update_wincolor(win_T *wp, char_u *opt)
{
    char_u  *str = *opt == NUL ? "" : alloc(পরিমাণ("!(:") + STRLEN(opt));
    ৱর্ণ    *errmsg;

    যদি (str == রিক্ত)
	ঘূৰাই e_out_of_memory;

    যদি (*opt != NUL)
	sprintf((ৱর্ণ *)str, "!(:%s", opt);

    errmsg = update_winhighlight(wp, str);

    যদি (errmsg == রিক্ত)
	set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		str, OPT_FREE|OPT_LOCAL, 0);

    যদি (*opt != NUL)
	মুক্ত(str);
    ঘূৰাই errmsg;
}
