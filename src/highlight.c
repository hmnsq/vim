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

#અન્તર્ભાવ "vim.h"

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
સ્થિર keyvalue_T highlight_tab[] = {
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
સ્થિર keyvalue_T *highlight_index_tab[] = {
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

ગણના {
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
સ્થિર keyvalue_T color_name_tab[] = {
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
પ્રકારવ્યાખ્યા સંરચના
{
    char_u	*sg_name;	// highlight group name
    char_u	*sg_name_u;	// uppercase of sg_name
    પૂર્ણાંક		sg_cleared;	// "hi clear" was used
// for normal terminals
    પૂર્ણાંક		sg_term;	// "term=" highlighting attributes
    char_u	*sg_start;	// terminal string for start highl
    char_u	*sg_stop;	// terminal string for stop highl
    પૂર્ણાંક		sg_term_attr;	// Screen attr for term mode
// for color terminals
    પૂર્ણાંક		sg_cterm;	// "cterm=" highlighting attr
    પૂર્ણાંક		sg_cterm_bold;	// bold attr was set for light color
    પૂર્ણાંક		sg_cterm_fg;	// terminal fg color number + 1
    પૂર્ણાંક		sg_cterm_bg;	// terminal bg color number + 1
    પૂર્ણાંક		sg_cterm_ul;	// terminal ul color number + 1
    પૂર્ણાંક		sg_cterm_attr;	// Screen attr for color term mode
    પૂર્ણાંક		sg_cterm_font;	// terminal alternative font (0 for normal)
// for when using the GUI
#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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
    પૂર્ણાંક		sg_gui_attr;    // Screen attr for GUI mode
#endif
#જો defined(FEAT_GUI) || defined(FEAT_EVAL)
// Store the sp color name for the GUI or synIDattr()
    પૂર્ણાંક		sg_gui;		// "gui=" highlighting attributes
    char_u	*sg_gui_fg_name;// GUI foreground color name
    char_u	*sg_gui_bg_name;// GUI background color name
    char_u	*sg_gui_sp_name;// GUI special color name
#endif
    પૂર્ણાંક		sg_link;	// link to this highlight group ID
    પૂર્ણાંક		sg_deflink;	// default link; restored in highlight_clear()
    પૂર્ણાંક		sg_set;		// combination of SG_* flags
#ifdef FEAT_EVAL
    sctx_T	sg_deflink_sctx;  // script where the default link was set
    sctx_T	sg_script_ctx;	// script in which the group was last set
#endif
} hl_group_T;

પ્રકારવ્યાખ્યા સંરચના hl_overrides_S hl_overrides_T;
સંરચના hl_overrides_S
{
    hl_override_T   *arr;   // May be NULL if "arr" was freed or no highlight
			    // overrides (all values set to default)
    પૂર્ણાંક		    len;
    hl_overrides_T  *next;  // Used to handle recursive calls

    પૂર્ણાંક attr[HLF_ગણતરી];    // highlight_attr[] before being updated.
};

સ્થિર hl_overrides_T *overrides = ખાલી;

// Synced with highlight_attr, each is the highlight group id for each attr. If
// an element is 0, then there is no attr for it. If an element is -1, then it
// means it was set to itself.
સ્થિર પૂર્ણાંક highlight_ids[HLF_ગણતરી];

// Same as highlight_attr[] but is not modified by highlight group overrides
સ્થિર પૂર્ણાંક highlight_attr_raw[HLF_ગણતરી];

સ્થિર પૂર્ણાંક hl_flags[HLF_ગણતરી] = HL_FLAGS;

// highlight groups for 'highlight' option
સ્થિર garray_T highlight_ga;
#define HL_TABLE()	((hl_group_T *)((highlight_ga.ga_data)))

// Wrapper struct for hashtable entries: stores the highlight group ID alongside
// the uppercase name used as a hash key.  Uses the same offsetof pattern as
// signgroup_T / HI2SG.
પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક		hn_id;		// highlight group ID (1-based)
    char_u	hn_ચાવી[1];	// uppercase name (stretchable array)
} hlname_T;

#define HLNAME_ચાવી_OFF	offsetof(hlname_T, hn_ચાવી)
#define HI2HLNAME(hi)	((hlname_T *)((hi)->hi_ચાવી - HLNAME_ચાવી_OFF))

// hashtable for quick highlight group name lookup
સ્થિર hashtab_T highlight_ht;
સ્થિર bool	 highlight_ht_inited = false;

/*
 * An attribute number is the index in attr_table plus ATTR_OFF.
 */
#define ATTR_OFF (HL_ALL + 1)

સ્થિર શૂન્ય syn_unadd_group(શૂન્ય);
સ્થિર શૂન્ય set_hl_attr(પૂર્ણાંક idx);
સ્થિર શૂન્ય highlight_list_one(પૂર્ણાંક id);
સ્થિર પૂર્ણાંક highlight_list_arg(પૂર્ણાંક id, પૂર્ણાંક didh, પૂર્ણાંક type, પૂર્ણાંક iarg, char_u *sarg, અક્ષર *name);
સ્થિર પૂર્ણાંક syn_add_group(char_u *name);
સ્થિર પૂર્ણાંક syn_name2id_len(char_u *name, પૂર્ણાંક len);
સ્થિર પૂર્ણાંક hl_has_settings(પૂર્ણાંક idx, પૂર્ણાંક check_link);
સ્થિર શૂન્ય highlight_clear(પૂર્ણાંક idx);

#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
સ્થિર શૂન્ય gui_do_one_color(પૂર્ણાંક idx, પૂર્ણાંક do_menu, પૂર્ણાંક do_tooltip);
#endif
#ifdef FEAT_GUI
સ્થિર પૂર્ણાંક  set_group_colors(char_u *name, guicolor_T *fgp, guicolor_T *bgp, પૂર્ણાંક do_menu, પૂર્ણાંક use_norm, પૂર્ણાંક do_tooltip);
સ્થિર શૂન્ય hl_do_font(પૂર્ણાંક idx, char_u *arg, પૂર્ણાંક do_normal, પૂર્ણાંક do_menu, પૂર્ણાંક do_tooltip, પૂર્ણાંક free_font);
#endif

/*
 * The default highlight groups.  These are compiled-in for fast startup and
 * they still work when the runtime files can't be found.
 * When making changes here, also change runtime/colors/default.vim!
 * The #ifdefs are needed to reduce the amount of static data.  Helps to make
 * the 16 bit DOS (museum) version compile.
 */
#જો defined(FEAT_GUI) || defined(FEAT_EVAL)
# define CENT(a, b) b
#નહીંતર
# define CENT(a, b) a
#endif
સ્થિર અક્ષર *(highlight_init_both[]) = {
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
    ખાલી
};

// Default colors only used with a light background.
સ્થિર અક્ષર *(highlight_init_light[]) = {
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
    ખાલી
};

// Default colors only used with a dark background.
સ્થિર અક્ષર *(highlight_init_dark[]) = {
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
    ખાલી
};

#જો defined(FEAT_SYN_HL)
/*
 * Returns the number of highlight groups.
 */
    પૂર્ણાંક
highlight_અંક_groups(શૂન્ય)
{
    પાછા highlight_ga.ga_len;
}

/*
 * Returns the name of a highlight group.
 */
    char_u *
highlight_group_name(પૂર્ણાંક id)
{
    પાછા HL_TABLE()[id].sg_name;
}

/*
 * Returns the ID of the link to a highlight group.
 */
    પૂર્ણાંક
highlight_link_id(પૂર્ણાંક id)
{
    પાછા HL_TABLE()[id].sg_link;
}
#endif

સ્થિર શૂન્ય resolve_fallback_fg_to_rgb(શૂન્ય);
સ્થિર શૂન્ય resolve_fallback_bg_to_rgb(શૂન્ય);

    શૂન્ય
init_highlight(
    પૂર્ણાંક		both,	    // include groups where 'bg' doesn't matter
    પૂર્ણાંક		reset)	    // clear group first
{
    પૂર્ણાંક		i;
    અક્ષર	**pp;
    સ્થિર પૂર્ણાંક	had_both = FALSE;
#ifdef FEAT_EVAL
    char_u	*p;

    // Try finding the color scheme file.  Used when a color file was loaded
    // and 'background' or 't_Co' is changed.
    p = get_var_કિંમત((char_u *)"g:colors_name");
    જો (p != ખાલી)
    {
	// The value of g:colors_name could be freed when sourcing the script,
	// making "p" invalid, so copy it.
	char_u *copy_p = vim_strsave(p);
	પૂર્ણાંક    r;

	જો (copy_p != ખાલી)
	{
	    r = load_colors(copy_p);
	    vim_free(copy_p);
	    જો (r == OK)
		પાછા;
	}
    }

#endif

    // Didn't use a color file, use the compiled-in colors.
    જો (both)
    {
	had_both = TRUE;
	pp = highlight_init_both;
	માટે (i = 0; pp[i] != ખાલી; ++i)
	    do_highlight((char_u *)pp[i], reset, TRUE);
    }
    નહીંતર જો (!had_both)
	// Don't do anything before the call with both == TRUE from main().
	// Not everything has been setup then, and that call will overrule
	// everything anyway.
	પાછા;

    જો (*p_bg == 'l')
	pp = highlight_init_light;
    નહીંતર
	pp = highlight_init_dark;
    માટે (i = 0; pp[i] != ખાલી; ++i)
	do_highlight((char_u *)pp[i], reset, TRUE);

    // Reverse looks ugly, but grey may not work for less than 8 colors.  Thus
    // let it depend on the number of colors available.
    જો (t_colors < 8)
	do_highlight((char_u *)"Visual term=reverse cterm=reverse ctermbg=NONE ctermfg=NONE",
		FALSE, TRUE);
    // With 8 colors brown is equal to yellow, need to use black for Search fg
    // to avoid Statement highlighted text disappears.
    // Clear the attributes, needed when changing the t_Co value.
    જો (t_colors <= 8)
    {
	જો (*p_bg == 'l')
	    do_highlight((char_u *)"Search ctermfg=black", FALSE, TRUE);
    }

#ifdef FEAT_SYN_HL
    // If syntax highlighting is enabled load the highlighting for it.
    જો (get_var_કિંમત((char_u *)"g:syntax_on") != ખાલી)
    {
	સ્થિર પૂર્ણાંક	recursive = 0;

	જો (recursive >= 5)
	    emsg(_(e_recursive_loop_loading_syncolor_vim));
	નહીંતર
	{
	    ++recursive;
	    (શૂન્ય)source_runtime((char_u *)"syntax/syncolor.vim", DIP_ALL);
	    --recursive;
	}
    }
#endif
    resolve_fallback_fg_to_rgb();
    resolve_fallback_bg_to_rgb();
}

#જો defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
/*
 * Load a default color list. Intended to support legacy color names but allows
 * the user to override the color values. Only loaded once.
 */
    સ્થિર શૂન્ય
load_default_colors_lists(શૂન્ય)
{
    // Lacking a default color list isn't the end of the world but it is likely
    // an inconvenience so users should know when it is missing.
    જો (source_runtime((char_u *)"colors/lists/default.vim", DIP_ALL) != OK)
	સંદેશ("failed to load colors/lists/default.vim");
}
#endif

/*
 * Load color file "name".
 * Return OK for success, FAIL for failure.
 */
    પૂર્ણાંક
load_colors(char_u *name)
{
    char_u	*buf;
    પૂર્ણાંક		retval = FAIL;
    સ્થિર પૂર્ણાંક	recursive = FALSE;

    // When being called recursively, this is probably because setting
    // 'background' caused the highlighting to be reloaded.  This means it is
    // working, thus we should return OK.
    જો (recursive)
	પાછા OK;

    recursive = TRUE;
    buf = alloc(STRLEN(name) + 12);
    જો (buf != ખાલી)
    {
#જો defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	load_default_colors_lists();
#endif
	apply_autocmds(EVENT_COLORSCHEMEPRE, name,
					       curbuf->b_fname, FALSE, curbuf);
	sprintf((અક્ષર *)buf, "colors/%s.vim", name);
	retval = source_runtime(buf, DIP_START + DIP_OPT);
	vim_free(buf);
	જો (retval == OK)
	    apply_autocmds(EVENT_COLORSCHEME, name, curbuf->b_fname,
								FALSE, curbuf);
    }
    recursive = FALSE;

    પાછા retval;
}

સ્થિર પૂર્ણાંક color_numbers_16[28] = {0, 1, 2, 3,
				 4, 5, 6, 6,
				 7, 7, 7, 7,
				 8, 8,
				 9, 9, 10, 10,
				 11, 11, 12, 12, 13,
				 13, 14, 14, 15, -1};
// for xterm with 88 colors...
સ્થિર પૂર્ણાંક color_numbers_88[28] = {0, 4, 2, 6,
				 1, 5, 32, 72,
				 84, 84, 7, 7,
				 82, 82,
				 12, 43, 10, 61,
				 14, 63, 9, 74, 13,
				 75, 11, 78, 15, -1};
// for xterm with 256 colors...
સ્થિર પૂર્ણાંક color_numbers_256[28] = {0, 4, 2, 6,
				 1, 5, 130, 3,
				 248, 248, 7, 7,
				 242, 242,
				 12, 81, 10, 121,
				 14, 159, 9, 224, 13,
				 225, 11, 229, 15, -1};
// for terminals with less than 16 colors...
સ્થિર પૂર્ણાંક color_numbers_8[28] = {0, 4, 2, 6,
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
    સ્થિર પૂર્ણાંક
lookup_color(પૂર્ણાંક idx, પૂર્ણાંક foreground, પૂર્ણાંક *boldp)
{
    પૂર્ણાંક		color = color_numbers_16[idx];
    char_u	*p;

    // Use the _16 table to check if it's a valid color name.
    જો (color < 0)
	પાછા -1;

    જો (t_colors == 8)
    {
	// t_Co is 8: use the 8 colors table
#જો defined(__QNXNTO__)
	// On qnx, the 8 & 16 color arrays are the same
	જો (STRNCMP(T_NAME, "qansi", 5) == 0)
	    color = color_numbers_16[idx];
	નહીંતર
#endif
	    color = color_numbers_8[idx];
	જો (foreground)
	{
	    // set/reset bold attribute to get light foreground
	    // colors (on some terminals, e.g. "linux")
	    જો (color & 8)
		*boldp = TRUE;
	    નહીંતર
		*boldp = FALSE;
	}
	color &= 7;	// truncate to 8 colors
    }
    નહીંતર જો (t_colors == 16 || t_colors == 88
					   || t_colors >= 256)
    {
	// Guess: if the termcap entry ends in 'm', it is
	// probably an xterm-like terminal.  Use the changed
	// order for colors.
	જો (*T_CAF != NUL)
	    p = T_CAF;
	નહીંતર
	    p = T_CSF;
	જો (*p != NUL && (t_colors > 256
			      || *(p + STRLEN(p) - 1) == 'm'))
	{
	    જો (t_colors == 88)
		color = color_numbers_88[idx];
	    નહીંતર જો (t_colors >= 256)
		color = color_numbers_256[idx];
	    નહીંતર
		color = color_numbers_8[idx];
	}
#ifdef FEAT_TERMRESPONSE
	જો (t_colors >= 256 && color == 15 && is_mac_terminal)
	    // Terminal.app has a bug: 15 is light grey. Use white
	    // from the color cube instead.
	    color = 231;
#endif
    }
    પાછા color;
}

/*
 * Link highlight group 'from_hg' to 'to_hg'.
 * 'dodefault' is set to TRUE for ":highlight default link".
 * 'forceit' is set to TRUE for ":highlight! link"
 * 'init' is set to TRUE when initializing all the highlight groups.
 */
    સ્થિર શૂન્ય
highlight_group_link(
	char_u	*from_hg,
	પૂર્ણાંક	from_len,
	char_u	*to_hg,
	પૂર્ણાંક	to_len,
	પૂર્ણાંક	dodefault,
	પૂર્ણાંક	forceit,
	પૂર્ણાંક	init)
{
    પૂર્ણાંક		from_id;
    પૂર્ણાંક		to_id;
    hl_group_T	*hlgroup = ખાલી;

    from_id = syn_check_group(from_hg, from_len);
    જો (STRNCMP(to_hg, "NONE", 4) == 0)
	to_id = 0;
    નહીંતર
	to_id = syn_check_group(to_hg, to_len);

    જો (from_id > 0)
    {
	hlgroup = &HL_TABLE()[from_id - 1];
	જો (dodefault && (forceit || hlgroup->sg_deflink == 0))
	{
	    hlgroup->sg_deflink = to_id;
#ifdef FEAT_EVAL
	    hlgroup->sg_deflink_sctx = current_sctx;
	    hlgroup->sg_deflink_sctx.sc_lnum += SOURCING_LNUM;
#endif
	}
    }

    જો (from_id > 0 && (!init || hlgroup->sg_set == 0))
    {
	// Don't allow a link when there already is some highlighting
	// for the group, unless '!' is used
	જો (to_id > 0 && !forceit && !init
		&& hl_has_settings(from_id - 1, dodefault))
	{
	    જો (SOURCING_NAME == ખાલી && !dodefault)
		emsg(_(e_group_has_settings_highlight_link_ignored));
	}
	નહીંતર જો (hlgroup->sg_link != to_id
#ifdef FEAT_EVAL
		|| hlgroup->sg_script_ctx.sc_sid != current_sctx.sc_sid
#endif
		|| hlgroup->sg_cleared)
	{
	    જો (!init)
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
    સ્થિર શૂન્ય
highlight_reset_all(શૂન્ય)
{
    પૂર્ણાંક		idx;

#ifdef FEAT_GUI
    // First, we do not destroy the old values, but allocate the new
    // ones and update the display. THEN we destroy the old values.
    // If we destroy the old values first, then the old values
    // (such as GuiFont's or GuiFontset's) will still be displayed but
    // invalid because they were free'd.
    જો (gui.in_use)
    {
# ifdef FEAT_BEVAL_TIP
	gui_init_tooltip_font();
# endif
# જો defined(FEAT_MENU) && defined(FEAT_GUI_MOTIF)
	gui_init_menu_font();
# endif
    }
# જો defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_X11)
    gui_mch_def_colors();
# endif
# ifdef FEAT_GUI_MSWIN
    જો (gui.in_use)
	gui_mch_set_titlebar_colors();
# endif
# ifdef FEAT_GUI_X11
#  ifdef FEAT_MENU

    // This only needs to be done when there is no Menu highlight
    // group defined by default, which IS currently the case.
    gui_mch_new_menu_colors();
#  endif
    જો (gui.in_use)
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
    માટે (idx = 0; idx < highlight_ga.ga_len; ++idx)
	highlight_clear(idx);
    init_highlight(TRUE, TRUE);
#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    જો (USE_24BIT)
	highlight_gui_started();
    નહીંતર
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
    સ્થિર પૂર્ણાંક
highlight_set_termgui_attr(પૂર્ણાંક idx, char_u *ચાવી, char_u *arg, પૂર્ણાંક init)
{
    પૂર્ણાંક		attr;
    size_t	off;
    keyvalue_T	target;
    keyvalue_T	*entry;

    attr = 0;
    off = 0;
    target.ચાવી = 0;
    target.કિંમત.length = 0;	// not used, see cmp_keyvalue_value_ni()
    જ્યારે (arg[off] != NUL)
    {
	target.કિંમત.string = arg + off;
	entry = (keyvalue_T *)bsearch(&target, &highlight_tab,
		ARRAY_LENGTH(highlight_tab), કદ(highlight_tab[0]),
		cmp_keyvalue_કિંમત_ni);
	જો (entry == ખાલી)
	{
	    semsg(_(e_illegal_કિંમત_str), arg);
	    પાછા FALSE;
	}

	attr |= entry->ચાવી;
	off += entry->કિંમત.length;
	જો (arg[off] == ',')		// another one follows
	    ++off;
    }
    જો (*ચાવી == 'T')
    {
	જો (!init || !(HL_TABLE()[idx].sg_set & SG_TERM))
	{
	    જો (!init)
		HL_TABLE()[idx].sg_set |= SG_TERM;
	    HL_TABLE()[idx].sg_term = attr;
	}
    }
    નહીંતર જો (*ચાવી == 'C')
    {
	જો (!init || !(HL_TABLE()[idx].sg_set & SG_CTERM))
	{
	    જો (!init)
		HL_TABLE()[idx].sg_set |= SG_CTERM;
	    HL_TABLE()[idx].sg_cterm = attr;
	    HL_TABLE()[idx].sg_cterm_bold = FALSE;
	}
    }
#જો defined(FEAT_GUI) || defined(FEAT_EVAL)
    નહીંતર
    {
	જો (!init || !(HL_TABLE()[idx].sg_set & SG_GUI))
	{
	    જો (!init)
		HL_TABLE()[idx].sg_set |= SG_GUI;
	    HL_TABLE()[idx].sg_gui = attr;
	}
    }
#endif

    પાછા TRUE;
}

#ifdef FEAT_GUI
/*
 * Set the font for the highlight group at 'idx'.
 * 'arg' is the font name.
 * Returns TRUE if the font is changed.
 */
    સ્થિર પૂર્ણાંક
highlight_set_font(
	પૂર્ણાંક	idx,
	char_u	*arg,
	પૂર્ણાંક	is_normal_group,
	પૂર્ણાંક	is_menu_group,
	પૂર્ણાંક	is_tooltip_group)
{
    પૂર્ણાંક		did_change = FALSE;

    // in non-GUI fonts are simply ignored
    જો (HL_TABLE()[idx].sg_font_name != ખાલી
	    && STRCMP(HL_TABLE()[idx].sg_font_name, arg) == 0)
    {
	// Font name didn't change, ignore.
    }
    નહીંતર જો (!gui.shell_created)
    {
	// GUI not started yet, always accept the name.
	vim_free(HL_TABLE()[idx].sg_font_name);
	HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	did_change = TRUE;
    }
    નહીંતર
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
	જો (HL_TABLE()[idx].sg_fontset != NOFONTSET)
	{
	    // New fontset was accepted. Free the old one, if there
	    // was one.
	    gui_mch_free_fontset(temp_sg_fontset);
	    did_change = TRUE;
	}
	નહીંતર
	    HL_TABLE()[idx].sg_fontset = temp_sg_fontset;
# endif
	જો (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    // New font was accepted. Free the old one, if there was
	    // one.
	    gui_mch_free_font(temp_sg_font);
	    did_change = TRUE;
	}
	નહીંતર
	    HL_TABLE()[idx].sg_font = temp_sg_font;
	// Update sg_font_name once here, after both font and fontset results
	// are known.
	જો (did_change)
	{
	    vim_free(HL_TABLE()[idx].sg_font_name);
	    HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	}
    }

    પાછા did_change;
}
#endif

/*
 * Set the cterm foreground color for the Normal highlight group to "color" and
 * the bold attribute to "bold".
 */
    સ્થિર શૂન્ય
hl_set_ctermfg_normal_group(પૂર્ણાંક color, પૂર્ણાંક bold)
{
    cterm_normal_fg_color = color + 1;
    cterm_normal_fg_bold = bold;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    જો (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	જો (termcap_active && color >= 0)
	    term_fg_color(color);
    }
}

/*
 * Set the cterm foreground color for the highlight group at 'idx' to 'color'.
 */
    સ્થિર શૂન્ય
highlight_set_ctermfg(પૂર્ણાંક idx, પૂર્ણાંક color, પૂર્ણાંક is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_fg = color + 1;
    જો (is_normal_group)
	hl_set_ctermfg_normal_group(color,
					(HL_TABLE()[idx].sg_cterm & HL_BOLD));
}

/*
 * Set the cterm background color for the Normal highlight group to "color".
 */
    સ્થિર શૂન્ય
hl_set_ctermbg_normal_group(પૂર્ણાંક color)
{
    cterm_normal_bg_color = color + 1;
#ifdef FEAT_GUI
    // Don't mess with 'background' if the GUI is used.
    જો (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	જો (color >= 0)
	{
	    પૂર્ણાંક dark = -1;

	    જો (termcap_active)
		term_bg_color(color);
	    જો (t_colors < 16)
		dark = (color == 0 || color == 4);
	    // Limit the heuristic to the standard 16 colors
	    નહીંતર જો (color < 16)
		dark = (color < 7 || color == 8);
	    // Set the 'background' option if the value is
	    // wrong.
	    જો (dark != -1
		    && dark != (*p_bg == 'd')
		    && !option_was_set((char_u *)"bg"))
	    {
		set_option_કિંમત_give_err((char_u *)"bg",
			0L, (char_u *)(dark ? "dark" : "light"), 0);
		reset_option_was_set((char_u *)"bg");
	    }
	}
    }
}

/*
 * Set the cterm background color for the highlight group at 'idx' to 'color'.
 */
    સ્થિર શૂન્ય
highlight_set_ctermbg(પૂર્ણાંક idx, પૂર્ણાંક color, પૂર્ણાંક is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_bg = color + 1;
    જો (is_normal_group)
	hl_set_ctermbg_normal_group(color);
}

/*
 * Set the cterm underline color for the Normal highlight group to "color".
 */
    સ્થિર શૂન્ય
hl_set_ctermul_normal_group(પૂર્ણાંક color)
{
    cterm_normal_ul_color = color + 1;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    જો (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	જો (termcap_active && color >= 0)
	    term_ul_color(color);
    }
}

/*
 * Set the cterm underline color for the highlight group at 'idx' to 'color'.
 */
    સ્થિર શૂન્ય
highlight_set_ctermul(પૂર્ણાંક idx, પૂર્ણાંક color, પૂર્ણાંક is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_ul = color + 1;
    જો (is_normal_group)
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
    સ્થિર પૂર્ણાંક
highlight_set_cterm_font(
	પૂર્ણાંક	idx,
	char_u	*arg,
	પૂર્ણાંક	init)
{
    પૂર્ણાંક		font;

    જો (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	પાછા FALSE;

    જો (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    જો (VIM_ISDIGIT(*arg))
	font = atoi((અક્ષર *)arg);
    નહીંતર જો (STRICMP(arg, "NONE") == 0)
	font = -1;
    નહીંતર
	પાછા FALSE;

    HL_TABLE()[idx].sg_cterm_font = font + 1;
    પાછા TRUE;
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
    સ્થિર પૂર્ણાંક
highlight_set_cterm_color(
	પૂર્ણાંક	idx,
	char_u	*ચાવી,
	char_u	*ચાવી_start,
	char_u	*arg,
	પૂર્ણાંક	is_normal_group,
	પૂર્ણાંક	init)
{
    પૂર્ણાંક		color;

    જો (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	પાછા FALSE;

    જો (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    // When setting the foreground color, and previously the "bold"
    // flag was set for a light color, reset it now
    જો (ચાવી[5] == 'F' && HL_TABLE()[idx].sg_cterm_bold)
    {
	HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
	HL_TABLE()[idx].sg_cterm_bold = FALSE;
    }

    જો (VIM_ISDIGIT(*arg))
	color = atoi((અક્ષર *)arg);
    નહીંતર જો (STRICMP(arg, "fg") == 0)
    {
	જો (cterm_normal_fg_color)
	    color = cterm_normal_fg_color - 1;
	નહીંતર
	{
	    emsg(_(e_fg_color_unknown));
	    પાછા FALSE;
	}
    }
    નહીંતર જો (STRICMP(arg, "bg") == 0)
    {
	જો (cterm_normal_bg_color > 0)
	    color = cterm_normal_bg_color - 1;
	નહીંતર
	{
	    emsg(_(e_bg_color_unknown));
	    પાછા FALSE;
	}
    }
    નહીંતર જો (STRICMP(arg, "ul") == 0)
    {
	જો (cterm_normal_ul_color > 0)
	    color = cterm_normal_ul_color - 1;
	નહીંતર
	{
	    emsg(_(e_ul_color_unknown));
	    પાછા FALSE;
	}
    }
    નહીંતર
    {
	પૂર્ણાંક bold = MAYBE;
	keyvalue_T target;
	keyvalue_T *entry;

	target.ચાવી = 0;
	target.કિંમત.string = arg;
	target.કિંમત.length = 0;	// not used, see cmp_keyvalue_value_i()
	entry = (keyvalue_T *)bsearch(&target, &color_name_tab,
		ARRAY_LENGTH(color_name_tab), કદ(color_name_tab[0]),
		cmp_keyvalue_કિંમત_i);
	જો (entry == ખાલી)
	{
	    semsg(_(e_color_name_or_સંખ્યા_not_recognized_str), ચાવી_start);
	    પાછા FALSE;
	}

	color = lookup_color(entry->ચાવી, ચાવી[5] == 'F', &bold);

	// set/reset bold attribute to get light foreground
	// colors (on some terminals, e.g. "linux")
	જો (bold == TRUE)
	{
	    HL_TABLE()[idx].sg_cterm |= HL_BOLD;
	    HL_TABLE()[idx].sg_cterm_bold = TRUE;
	}
	નહીંતર જો (bold == FALSE)
	    HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
    }

    // Add one to the argument, to avoid zero.  Zero is used for
    // "NONE", then "color" is -1.
    જો (ચાવી[5] == 'F')
	highlight_set_ctermfg(idx, color, is_normal_group);
    નહીંતર જો (ચાવી[5] == 'B')
	highlight_set_ctermbg(idx, color, is_normal_group);
    નહીંતર // ctermul
	highlight_set_ctermul(idx, color, is_normal_group);

    પાછા TRUE;
}

#જો defined(FEAT_GUI) || defined(FEAT_EVAL)
/*
 * Set the GUI foreground color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    સ્થિર પૂર્ણાંક
highlight_set_guifg(
	પૂર્ણાંક	idx,
	char_u	*arg,
	પૂર્ણાંક	is_menu_group UNUSED,
	પૂર્ણાંક	is_scrollbar_group UNUSED,
	પૂર્ણાંક	is_tooltip_group UNUSED,
	પૂર્ણાંક	is_titlebar_group UNUSED,
	પૂર્ણાંક	is_titlebarnc_group UNUSED,
	પૂર્ણાંક	*do_colors UNUSED,
	પૂર્ણાંક	init)
{
# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    long	i;
# endif
    char_u	**namep;
    પૂર્ણાંક		did_change = FALSE;

    જો (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	પાછા FALSE;

    namep = &HL_TABLE()[idx].sg_gui_fg_name;
    જો (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guifg colors are only used when recognized
    i = color_name2handle(arg);
    જો (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_fg = i;
# endif
	જો (*namep == ખાલી || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    જો (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    નહીંતર
		*namep = ખાલી;
	    did_change = TRUE;
	}
# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	જો (is_titlebar_group && gui.title_fg_pixel != i)
	{
	    gui.title_fg_pixel = i;
	    *do_colors = TRUE;
	}
	જો (is_titlebarnc_group && gui.titlenc_fg_pixel != i)
	{
	    gui.titlenc_fg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	જો (is_menu_group && gui.menu_fg_pixel != i)
	{
	    gui.menu_fg_pixel = i;
	    *do_colors = TRUE;
	}
	જો (is_scrollbar_group && gui.scroll_fg_pixel != i)
	{
	    gui.scroll_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	જો (is_tooltip_group && gui.tooltip_fg_pixel != i)
	{
	    gui.tooltip_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    પાછા did_change;
}

/*
 * Set the GUI background color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    સ્થિર પૂર્ણાંક
highlight_set_guibg(
	પૂર્ણાંક	idx,
	char_u	*arg,
	પૂર્ણાંક	is_menu_group UNUSED,
	પૂર્ણાંક	is_scrollbar_group UNUSED,
	પૂર્ણાંક	is_tooltip_group UNUSED,
	પૂર્ણાંક	is_titlebar_group UNUSED,
	પૂર્ણાંક	is_titlebarnc_group UNUSED,
	પૂર્ણાંક	*do_colors UNUSED,
	પૂર્ણાંક	init)
{
# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    પૂર્ણાંક		i;
# endif
    char_u	**namep;
    પૂર્ણાંક		did_change = FALSE;

    જો (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	પાછા FALSE;

    namep = &HL_TABLE()[idx].sg_gui_bg_name;
    જો (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guibg colors are only used when recognized
    i = color_name2handle(arg);
    જો (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_bg = i;
# endif
	જો (*namep == ખાલી || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    જો (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    નહીંતર
		*namep = ખાલી;
	    did_change = TRUE;
	}
# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	જો (is_titlebar_group && gui.title_bg_pixel != i)
	{
	    gui.title_bg_pixel = i;
	    *do_colors = TRUE;
	}
	જો (is_titlebarnc_group && gui.titlenc_bg_pixel != i)
	{
	    gui.titlenc_bg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	જો (is_menu_group && gui.menu_bg_pixel != i)
	{
	    gui.menu_bg_pixel = i;
	    *do_colors = TRUE;
	}
	જો (is_scrollbar_group && gui.scroll_bg_pixel != i)
	{
	    gui.scroll_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	જો (is_tooltip_group && gui.tooltip_bg_pixel != i)
	{
	    gui.tooltip_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    પાછા did_change;
}

/*
 * Set the GUI undercurl/strikethrough color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    સ્થિર પૂર્ણાંક
highlight_set_guisp(પૂર્ણાંક idx, char_u *arg, પૂર્ણાંક init)
{
# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    પૂર્ણાંક		i;
# endif
    પૂર્ણાંક		did_change = FALSE;
    char_u	**namep;

    જો (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	પાછા FALSE;

    namep = &HL_TABLE()[idx].sg_gui_sp_name;
    જો (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guisp colors are only used when recognized
    i = color_name2handle(arg);
    જો (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_sp = i;
# endif
	જો (*namep == ખાલી || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    જો (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    નહીંતર
		*namep = ખાલી;
	    did_change = TRUE;
	}
# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    }
# endif

    પાછા did_change;
}
#endif

/*
 * Set the start/stop terminal codes for a highlight group.
 * Returns TRUE if the terminal code is set.
 */
    સ્થિર પૂર્ણાંક
highlight_set_startstop_termcode(પૂર્ણાંક idx, char_u *ચાવી, char_u *arg, પૂર્ણાંક init)
{
    પૂર્ણાંક		off;
    char_u	buf[100];
    પૂર્ણાંક		len;
    char_u	*tname;
    char_u	*p;

    જો (!init)
	HL_TABLE()[idx].sg_set |= SG_TERM;

    // The "start" and "stop"  arguments can be a literal escape
    // sequence, or a comma separated list of terminal codes.
    જો (STRNCMP(arg, "t_", 2) == 0)
    {
	off = 0;
	buf[0] = 0;
	જ્યારે (arg[off] != NUL)
	{
	    // Isolate one termcap name
	    માટે (len = 0; arg[off + len] &&
		    arg[off + len] != ','; ++len)
		;
	    tname = vim_strnsave(arg + off, len);
	    જો (tname == ખાલી)		// out of memory
		પાછા FALSE;
	    // lookup the escape sequence for the item
	    p = get_term_code(tname);
	    vim_free(tname);
	    જો (p == ખાલી)	    // ignore non-existing things
		p = (char_u *)"";

	    // Append it to the already found stuff
	    જો ((પૂર્ણાંક)(STRLEN(buf) + STRLEN(p)) >= 99)
	    {
		semsg(_(e_terminal_code_too_long_str), arg);
		પાછા FALSE;
	    }
	    STRCAT(buf, p);

	    // Advance to the next item
	    off += len;
	    જો (arg[off] == ',')	    // another one follows
		++off;
	}
    }
    નહીંતર
    {
	// Copy characters from arg[] to buf[], translating <> codes.
	માટે (p = arg, off = 0; off < 100 - 6 && *p; )
	{
	    len = trans_special(&p, buf + off, FSK_SIMPLIFY, FALSE, ખાલી);
	    જો (len > 0)	    // recognized special char
		off += len;
	    નહીંતર		    // copy as normal char
		buf[off++] = *p++;
	}
	buf[off] = NUL;
    }

    જો (STRCMP(buf, "NONE") == 0)	// resetting the value
	p = ખાલી;
    નહીંતર
	p = vim_strsave(buf);
    જો (ચાવી[2] == 'A')
    {
	vim_free(HL_TABLE()[idx].sg_start);
	HL_TABLE()[idx].sg_start = p;
    }
    નહીંતર
    {
	vim_free(HL_TABLE()[idx].sg_stop);
	HL_TABLE()[idx].sg_stop = p;
    }
    પાછા TRUE;
}

/*
 * Handle the ":highlight .." command.
 * When using ":hi clear" this is called recursively for each group with
 * "forceit" and "init" both TRUE.
 */
    શૂન્ય
do_highlight(
    char_u	*લીટી,
    પૂર્ણાંક		forceit,
    પૂર્ણાંક		init)	    // TRUE when called for initializing
{
    char_u	*name_end;
    char_u	*linep;
    char_u	*ચાવી_start;
    char_u	*arg_start;
    char_u	*ચાવી = ખાલી, *arg = ખાલી;
    long	i;
    પૂર્ણાંક		id;
    પૂર્ણાંક		idx;
    hl_group_T	item_before;
    પૂર્ણાંક		did_change = FALSE;
    પૂર્ણાંક		dodefault = FALSE;
    પૂર્ણાંક		doclear = FALSE;
    પૂર્ણાંક		dolink = FALSE;
    પૂર્ણાંક		error = FALSE;
    પૂર્ણાંક		is_normal_group = FALSE;	// "Normal" group
#ifdef FEAT_GUI_MSWIN
    પૂર્ણાંક		is_titlebar_group = FALSE;	// "TitleBar" group
    પૂર્ણાંક		is_titlebarnc_group = FALSE;	// "TitleBarNC" group
#નહીંતર
# define is_titlebar_group 0
# define is_titlebarnc_group 0
#endif
#ifdef FEAT_GUI_X11
    પૂર્ણાંક		is_menu_group = FALSE;		// "Menu" group
    પૂર્ણાંક		is_scrollbar_group = FALSE;	// "Scrollbar" group
    પૂર્ણાંક		is_tooltip_group = FALSE;	// "Tooltip" group
#નહીંતર
# define is_menu_group 0
# define is_tooltip_group 0
# define is_scrollbar_group 0
#endif
#જો defined(FEAT_GUI) || defined(FEAT_EVAL)
    પૂર્ણાંક		do_colors = FALSE;		// need to update colors?
#endif
#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    પૂર્ણાંક		did_highlight_changed = FALSE;
#endif

    // If no argument, list current highlighting.
    જો (!init && ends_excmd2(લીટી - 1, લીટી))
    {
	માટે (i = 1; i <= highlight_ga.ga_len && !got_int; ++i)
	    // TODO: only call when the group has attributes set
	    highlight_list_one((પૂર્ણાંક)i);
	પાછા;
    }

    // Isolate the name.
    name_end = skiptowhite(લીટી);
    linep = skipwhite(name_end);

    // Check for "default" argument.
    જો (STRNCMP(લીટી, "default", name_end - લીટી) == 0)
    {
	dodefault = TRUE;
	લીટી = linep;
	name_end = skiptowhite(લીટી);
	linep = skipwhite(name_end);
    }

    // Check for "clear" or "link" argument.
    જો (STRNCMP(લીટી, "clear", name_end - લીટી) == 0)
	doclear = TRUE;
    જો (STRNCMP(લીટી, "link", name_end - લીટી) == 0)
	dolink = TRUE;

    // ":highlight {group-name}": list highlighting for one group.
    જો (!doclear && !dolink && ends_excmd2(લીટી, linep))
    {
	id = syn_namen2id(લીટી, (પૂર્ણાંક)(name_end - લીટી));
	જો (id == 0)
	    semsg(_(e_highlight_group_name_not_found_str), લીટી);
	નહીંતર
	    highlight_list_one(id);
	પાછા;
    }

    // Handle ":highlight link {from} {to}" command.
    જો (dolink)
    {
	char_u	    *from_start = linep;
	char_u	    *from_end;
	પૂર્ણાંક	    from_len;
	char_u	    *to_start;
	char_u	    *to_end;
	પૂર્ણાંક	    to_len;

	from_end = skiptowhite(from_start);
	to_start = skipwhite(from_end);
	to_end	 = skiptowhite(to_start);

	જો (ends_excmd2(લીટી, from_start) || ends_excmd2(લીટી, to_start))
	{
	    semsg(_(e_not_enough_arguments_highlight_link_str), from_start);
	    પાછા;
	}

	જો (!ends_excmd2(લીટી, skipwhite(to_end)))
	{
	    semsg(_(e_too_many_arguments_highlight_link_str), from_start);
	    પાછા;
	}

	from_len = (પૂર્ણાંક)(from_end - from_start);
	to_len = (પૂર્ણાંક)(to_end - to_start);
	highlight_group_link(from_start, from_len, to_start, to_len,
						dodefault, forceit, init);
	પાછા;
    }

    જો (doclear)
    {
	// ":highlight clear [group]" command.
	જો (ends_excmd2(લીટી, linep))
	{
	    // ":highlight clear" without group name
	    highlight_reset_all();
	    પાછા;
	}
	લીટી = linep;
	name_end = skiptowhite(લીટી);
	linep = skipwhite(name_end);
    }

    // Find the group name in the table.  If it does not exist yet, add it.
    id = syn_check_group(લીટી, (પૂર્ણાંક)(name_end - લીટી));
    જો (id == 0)	// failed (out of memory)
	પાછા;
    idx = id - 1;	// index is ID minus one

    // Return if "default" was used and the group already has settings.
    જો (dodefault && hl_has_settings(idx, TRUE))
	પાછા;

    // Make a copy so we can check if any attribute actually changed.
    item_before = HL_TABLE()[idx];

    જો (STRCMP(HL_TABLE()[idx].sg_name_u, "NORMAL") == 0)
	is_normal_group = TRUE;
#ifdef FEAT_GUI_MSWIN
    નહીંતર જો (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBAR") == 0)
	is_titlebar_group = TRUE;
    નહીંતર જો (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBARNC") == 0)
	is_titlebarnc_group = TRUE;
#endif
#ifdef FEAT_GUI_X11
    નહીંતર જો (STRCMP(HL_TABLE()[idx].sg_name_u, "MENU") == 0)
	is_menu_group = TRUE;
    નહીંતર જો (STRCMP(HL_TABLE()[idx].sg_name_u, "SCROLLBAR") == 0)
	is_scrollbar_group = TRUE;
    નહીંતર જો (STRCMP(HL_TABLE()[idx].sg_name_u, "TOOLTIP") == 0)
	is_tooltip_group = TRUE;
#endif

    // Clear the highlighting for ":hi clear {group}" and ":hi clear".
    જો (doclear || (forceit && init))
    {
	highlight_clear(idx);
	જો (!doclear)
	    HL_TABLE()[idx].sg_set = 0;
    }

    જો (!doclear)
	જ્યારે (!ends_excmd2(લીટી, linep))
	{
	    ચાવી_start = linep;
	    જો (*linep == '=')
	    {
		semsg(_(e_unexpected_equal_sign_str), ચાવી_start);
		error = TRUE;
		તોડો;
	    }

	    // Note: Keep this in sync with get_highlight_group_key.

	    // Isolate the key ("term", "ctermfg", "ctermbg", "font", "guifg"
	    // or "guibg").
	    જ્યારે (*linep && !VIM_ISWHITE(*linep) && *linep != '=')
		++linep;
	    vim_free(ચાવી);
	    ચાવી = vim_strnsave_up(ચાવી_start, linep - ચાવી_start);
	    જો (ચાવી == ખાલી)
	    {
		error = TRUE;
		તોડો;
	    }
	    linep = skipwhite(linep);

	    જો (STRCMP(ચાવી, "NONE") == 0)
	    {
		જો (!init || HL_TABLE()[idx].sg_set == 0)
		{
		    જો (!init)
			HL_TABLE()[idx].sg_set |= SG_TERM+SG_CTERM+SG_GUI;
		    highlight_clear(idx);
		}
		ચાલુ;
	    }

	    // Check for the equal sign.
	    જો (*linep != '=')
	    {
		semsg(_(e_missing_equal_sign_str_2), ચાવી_start);
		error = TRUE;
		તોડો;
	    }
	    ++linep;

	    // Isolate the argument.
	    linep = skipwhite(linep);
	    જો (*linep == '\'')		// guifg='color name'
	    {
		arg_start = ++linep;
		linep = vim_strchr(linep, '\'');
		જો (linep == ખાલી)
		{
		    semsg(_(e_invalid_argument_str), ચાવી_start);
		    error = TRUE;
		    તોડો;
		}
	    }
	    નહીંતર
	    {
		arg_start = linep;
		linep = skiptowhite(linep);
	    }
	    જો (linep == arg_start)
	    {
		semsg(_(e_missing_argument_str), ચાવી_start);
		error = TRUE;
		તોડો;
	    }
	    vim_free(arg);
	    arg = vim_strnsave(arg_start, linep - arg_start);
	    જો (arg == ખાલી)
	    {
		error = TRUE;
		તોડો;
	    }
	    જો (*linep == '\'')
		++linep;

	    // Store the argument.
	    જો (STRCMP(ચાવી, "TERM") == 0
		    || STRCMP(ચાવી, "CTERM") == 0
		    || STRCMP(ચાવી, "GUI") == 0)
	    {
		જો (!highlight_set_termgui_attr(idx, ચાવી, arg, init))
		{
		    error = TRUE;
		    તોડો;
		}
	    }
	    નહીંતર જો (STRCMP(ચાવી, "FONT") == 0)
	    {
		// in non-GUI fonts are simply ignored
#ifdef FEAT_GUI
		જો (highlight_set_font(idx, arg, is_normal_group,
					      is_menu_group, is_tooltip_group))
		    did_change = TRUE;
#endif
	    }
	    નહીંતર જો (STRCMP(ચાવી, "CTERMFG") == 0
		    || STRCMP(ચાવી, "CTERMBG") == 0
		    || STRCMP(ચાવી, "CTERMUL") == 0)
	    {
		જો (!highlight_set_cterm_color(idx, ચાવી, ચાવી_start, arg,
							is_normal_group, init))
		{
		    error = TRUE;
		    તોડો;
		}
	    }
	    નહીંતર જો (STRCMP(ચાવી, "CTERMFONT") == 0)
	    {
		જો (!highlight_set_cterm_font(idx, arg, init))
		{
		    error = TRUE;
		    તોડો;
		}
	    }
	    નહીંતર જો (STRCMP(ચાવી, "GUIFG") == 0)
	    {
#જો defined(FEAT_GUI) || defined(FEAT_EVAL)
		જો (highlight_set_guifg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    નહીંતર જો (STRCMP(ચાવી, "GUIBG") == 0)
	    {
#જો defined(FEAT_GUI) || defined(FEAT_EVAL)
		જો (highlight_set_guibg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    નહીંતર જો (STRCMP(ચાવી, "GUISP") == 0)
	    {
#જો defined(FEAT_GUI) || defined(FEAT_EVAL)
		જો (highlight_set_guisp(idx, arg, init))
		    did_change = TRUE;
#endif
	    }
	    નહીંતર જો (STRCMP(ચાવી, "START") == 0 || STRCMP(ચાવી, "STOP") == 0)
	    {
		જો (!highlight_set_startstop_termcode(idx, ચાવી, arg, init))
		{
		    error = TRUE;
		    તોડો;
		}
	    }
	    નહીંતર
	    {
		semsg(_(e_illegal_argument_str_3), ચાવી_start);
		error = TRUE;
		તોડો;
	    }
	    HL_TABLE()[idx].sg_cleared = FALSE;

	    // When highlighting has been given for a group, don't link it.
	    જો (!init || !(HL_TABLE()[idx].sg_set & SG_LINK))
		HL_TABLE()[idx].sg_link = 0;

	    // Continue with next argument.
	    linep = skipwhite(linep);
	}

    // If there is an error, and it's a new entry, remove it from the table.
    જો (error && idx == highlight_ga.ga_len)
	syn_unadd_group();
    નહીંતર
    {
	જો (is_normal_group)
	{
	    HL_TABLE()[idx].sg_term_attr = 0;
	    HL_TABLE()[idx].sg_cterm_attr = 0;
#ifdef FEAT_GUI
	    HL_TABLE()[idx].sg_gui_attr = 0;
	    // Need to update all groups, because they might be using "bg"
	    // and/or "fg", which have been changed now.
#endif
#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    જો (USE_24BIT)
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
	નહીંતર જો (is_titlebar_group)
	{
	    જો (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
	નહીંતર જો (is_titlebarnc_group)
	{
	    જો (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
#endif
#ifdef FEAT_GUI_X11
# ifdef FEAT_MENU
	નહીંતર જો (is_menu_group)
	{
	    જો (gui.in_use && do_colors)
		gui_mch_new_menu_colors();
	}
# endif
	નહીંતર જો (is_scrollbar_group)
	{
	    જો (gui.in_use && do_colors)
		gui_new_scrollbar_colors();
	    નહીંતર
		set_hl_attr(idx);
	}
# ifdef FEAT_BEVAL_GUI
	નહીંતર જો (is_tooltip_group)
	{
	    જો (gui.in_use && do_colors)
		gui_mch_new_tooltip_colors();
	}
# endif
#endif
	નહીંતર
	    set_hl_attr(idx);
#ifdef FEAT_EVAL
	HL_TABLE()[idx].sg_script_ctx = current_sctx;
	HL_TABLE()[idx].sg_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
    }

    vim_free(ચાવી);
    vim_free(arg);

    // Only call highlight_changed() once, after a sequence of highlight
    // commands, and only if an attribute actually changed.
    જો ((did_change
	   || memcmp(&HL_TABLE()[idx], &item_before, કદ(item_before)) != 0)
#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    && !did_highlight_changed
#endif
       )
    {
	// Do not trigger a redraw when highlighting is changed while
	// redrawing.  This may happen when evaluating 'statusline' changes the
	// StatusLine group.
	જો (!updating_screen)
	    redraw_all_later(UPD_NOT_VALID);
	need_highlight_changed = TRUE;
    }
}

#જો defined(EXITFREE)
    શૂન્ય
free_highlight(શૂન્ય)
{
    પૂર્ણાંક	    i;

    માટે (i = 0; i < highlight_ga.ga_len; ++i)
    {
	highlight_clear(i);
	vim_free(HL_TABLE()[i].sg_name);
	vim_free(HL_TABLE()[i].sg_name_u - HLNAME_ચાવી_OFF);
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
    શૂન્ય
restore_cterm_colors(શૂન્ય)
{
#જો defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
    // Since t_me has been set, this probably means that the user
    // wants to use this as default colors.  Need to reset default
    // background/foreground colors.
    mch_set_normal_colors();
#નહીંતર
# ifdef VIMDLL
    જો (!gui.in_use)
    {
	mch_set_normal_colors();
	પાછા;
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
    સ્થિર પૂર્ણાંક
hl_has_settings(પૂર્ણાંક idx, પૂર્ણાંક check_link)
{
    પાછા HL_TABLE()[idx].sg_cleared == 0
	 && (  HL_TABLE()[idx].sg_term_attr != 0
	    || HL_TABLE()[idx].sg_cterm_attr != 0
	    || HL_TABLE()[idx].sg_cterm_fg != 0
	    || HL_TABLE()[idx].sg_cterm_bg != 0
	    || HL_TABLE()[idx].sg_cterm_font != 0
#ifdef FEAT_GUI
	    || HL_TABLE()[idx].sg_gui_attr != 0
	    || HL_TABLE()[idx].sg_gui_fg_name != ખાલી
	    || HL_TABLE()[idx].sg_gui_bg_name != ખાલી
	    || HL_TABLE()[idx].sg_gui_sp_name != ખાલી
	    || HL_TABLE()[idx].sg_font_name != ખાલી
#endif
	    || (check_link && (HL_TABLE()[idx].sg_set & SG_LINK)));
}

/*
 * Clear highlighting for one group.
 */
    સ્થિર શૂન્ય
highlight_clear(પૂર્ણાંક idx)
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
#જો defined(FEAT_GUI) || defined(FEAT_EVAL)
    HL_TABLE()[idx].sg_gui = 0;
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_fg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_bg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_sp_name);
#endif
#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Set the normal foreground and background colors according to the "Normal"
 * highlighting group.  For X11 also set "Menu", "Scrollbar" and "Tooltip"
 * colors.  For MS-Windows also set "TitleBar" and "TitleBarNC" colors.
 */
    શૂન્ય
set_normal_colors(શૂન્ય)
{
# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    જો (gui.in_use)
#  endif
    {
	જો (set_group_colors((char_u *)"Normal",
				 &gui.norm_pixel, &gui.back_pixel,
				 FALSE, TRUE, FALSE))
	{
	    gui_mch_new_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  ifdef FEAT_GUI_MSWIN
	જો (set_group_colors((char_u *)"TitleBar",
				 &gui.title_fg_pixel, &gui.title_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
	જો (set_group_colors((char_u *)"TitleBarNC",
				 &gui.titlenc_fg_pixel, &gui.titlenc_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  endif
#  ifdef FEAT_GUI_X11
	જો (set_group_colors((char_u *)"Menu",
			     &gui.menu_fg_pixel, &gui.menu_bg_pixel,
			     TRUE, FALSE, FALSE))
	{
#   ifdef FEAT_MENU
	    gui_mch_new_menu_colors();
#   endif
	    set_must_redraw(UPD_CLEAR);
	}
#   ifdef FEAT_BEVAL_GUI
	જો (set_group_colors((char_u *)"Tooltip",
			     &gui.tooltip_fg_pixel, &gui.tooltip_bg_pixel,
			     FALSE, FALSE, TRUE))
	{
#    ifdef FEAT_TOOLBAR
	    gui_mch_new_tooltip_colors();
#    endif
	    set_must_redraw(UPD_CLEAR);
	}
#   endif
	જો (set_group_colors((char_u *)"Scrollbar",
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
    નહીંતર
#  endif
    {
	પૂર્ણાંક		idx;

	idx = syn_name2id((char_u *)"Normal") - 1;
	જો (idx >= 0)
	{
	    gui_do_one_color(idx, FALSE, FALSE);

	    // If the normal fg or bg color changed a complete redraw is
	    // required.
	    જો (cterm_normal_fg_gui_color != HL_TABLE()[idx].sg_gui_fg
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

#જો defined(FEAT_GUI)
/*
 * Set the colors for "Normal", "Menu", "TitleBar", "TitleBarNC", "Tooltip" or
 * "Scrollbar".
 */
    સ્થિર પૂર્ણાંક
set_group_colors(
    char_u	*name,
    guicolor_T	*fgp,
    guicolor_T	*bgp,
    પૂર્ણાંક		do_menu,
    પૂર્ણાંક		use_norm,
    પૂર્ણાંક		do_tooltip)
{
    પૂર્ણાંક		idx;

    idx = syn_name2id(name) - 1;
    જો (idx < 0)
	પાછા FALSE;

    gui_do_one_color(idx, do_menu, do_tooltip);

    જો (HL_TABLE()[idx].sg_gui_fg != INVALCOLOR)
	*fgp = HL_TABLE()[idx].sg_gui_fg;
    નહીંતર જો (use_norm)
	*fgp = gui.def_norm_pixel;
    જો (HL_TABLE()[idx].sg_gui_bg != INVALCOLOR)
	*bgp = HL_TABLE()[idx].sg_gui_bg;
    નહીંતર જો (use_norm)
	*bgp = gui.def_back_pixel;
    પાછા TRUE;
}

/*
 * Get the font of the "Normal" group.
 * Returns "" when it's not found or not set.
 */
    char_u *
hl_get_font_name(શૂન્ય)
{
    પૂર્ણાંક		id;
    char_u	*s;

    id = syn_name2id((char_u *)"Normal");
    જો (id > 0)
    {
	s = HL_TABLE()[id - 1].sg_font_name;
	જો (s != ખાલી)
	    પાછા s;
    }
    પાછા (char_u *)"";
}

/*
 * Set font for "Normal" group.  Called by gui_mch_init_font() when a font has
 * actually chosen to be used.
 */
    શૂન્ય
hl_set_font_name(char_u *font_name)
{
    પૂર્ણાંક	    id;

    id = syn_name2id((char_u *)"Normal");
    જો (id <= 0)
	પાછા;

    vim_free(HL_TABLE()[id - 1].sg_font_name);
    HL_TABLE()[id - 1].sg_font_name = vim_strsave(font_name);
}

/*
 * Set background color for "Normal" group.  Called by gui_set_bg_color()
 * when the color is known.
 */
    શૂન્ય
hl_set_bg_color_name(
    char_u  *name)	    // must have been allocated
{
    પૂર્ણાંક	    id;

    જો (name == ખાલી)
	પાછા;

    id = syn_name2id((char_u *)"Normal");
    જો (id <= 0)
	પાછા;

    vim_free(HL_TABLE()[id - 1].sg_gui_bg_name);
    HL_TABLE()[id - 1].sg_gui_bg_name = name;
}

/*
 * Set foreground color for "Normal" group.  Called by gui_set_fg_color()
 * when the color is known.
 */
    શૂન્ય
hl_set_fg_color_name(
    char_u  *name)	    // must have been allocated
{
    પૂર્ણાંક	    id;

    જો (name == ખાલી)
	પાછા;

    id = syn_name2id((char_u *)"Normal");
    જો (id <= 0)
	પાછા;

    vim_free(HL_TABLE()[id - 1].sg_gui_fg_name);
    HL_TABLE()[id - 1].sg_gui_fg_name = name;
}

/*
 * Return the handle for a font name.
 * Returns NOFONT when failed.
 */
    સ્થિર GuiFont
font_name2handle(char_u *name)
{
    જો (STRCMP(name, "NONE") == 0)
	પાછા NOFONT;

    પાછા gui_mch_get_font(name, TRUE);
}

# ifdef FEAT_XFONTSET
/*
 * Return the handle for a fontset name.
 * Returns NOFONTSET when failed.
 */
    સ્થિર GuiFontset
fontset_name2handle(char_u *name, પૂર્ણાંક fixed_width)
{
    જો (STRCMP(name, "NONE") == 0)
	પાછા NOFONTSET;

    પાછા gui_mch_get_fontset(name, TRUE, fixed_width);
}
# endif

/*
 * Get the font or fontset for one highlight group.
 */
    સ્થિર શૂન્ય
hl_do_font(
    પૂર્ણાંક		idx,
    char_u	*arg,
    પૂર્ણાંક		do_normal,		// set normal font
    પૂર્ણાંક		do_menu UNUSED,		// set menu font
    પૂર્ણાંક		do_tooltip UNUSED,	// set tooltip font
    પૂર્ણાંક		free_font)		// free current font/fontset
{
# ifdef FEAT_XFONTSET
    // If 'guifontset' is not empty, first try using the name as a
    // fontset.  If that doesn't work, use it as a font name.
    જો (*p_guifontset != NUL
#  ifdef FONTSET_ALWAYS
	|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
	// In Motif, the Tooltip highlight group is always a fontset
	|| do_tooltip
#  endif
	    )
    {
	જો (free_font)
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
    જો (HL_TABLE()[idx].sg_fontset != NOFONTSET)
    {
	// If it worked and it's the Normal group, use it as the normal
	// fontset.  Same for the Menu group.
	જો (do_normal)
	    gui_init_font(arg, TRUE);
#  જો defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	જો (do_menu)
	{
#   ifdef FONTSET_ALWAYS
	    gui.menu_fontset = HL_TABLE()[idx].sg_fontset;
#   નહીંતર
	    // YIKES!  This is a bug waiting to crash the program
	    gui.menu_font = HL_TABLE()[idx].sg_fontset;
#   endif
	    gui_mch_new_menu_font();
	}
#   ifdef FEAT_BEVAL_GUI
	જો (do_tooltip)
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
    નહીંતર
# endif
    {
	જો (free_font)
	    gui_mch_free_font(HL_TABLE()[idx].sg_font);
	HL_TABLE()[idx].sg_font = font_name2handle(arg);
	// If it worked and it's the Normal group, use it as the
	// normal font.  Same for the Menu group.
	જો (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    જો (do_normal)
		gui_init_font(arg, FALSE);
# ifndef FONTSET_ALWAYS
#  જો defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	    જો (do_menu)
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

#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return the handle for a color name.
 * Returns INVALCOLOR when failed.
 */
    guicolor_T
color_name2handle(char_u *name)
{
    જો (STRCMP(name, "NONE") == 0)
	પાછા INVALCOLOR;

    જો (STRICMP(name, "fg") == 0 || STRICMP(name, "foreground") == 0)
    {
# જો defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	જો (gui.in_use)
# endif
# ifdef FEAT_GUI
	    પાછા gui.norm_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	જો (cterm_normal_fg_gui_color != INVALCOLOR)
	    પાછા cterm_normal_fg_gui_color;
	// Guess that the foreground is black or white.
	પાછા GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "black" : "white"));
# endif
    }
    જો (STRICMP(name, "bg") == 0 || STRICMP(name, "background") == 0)
    {
# જો defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	જો (gui.in_use)
# endif
# ifdef FEAT_GUI
	    પાછા gui.back_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	જો (cterm_normal_bg_gui_color != INVALCOLOR)
	    પાછા cterm_normal_bg_gui_color;
	// Guess that the background is white or black.
	પાછા GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "white" : "black"));
# endif
    }

    પાછા GUI_GET_COLOR(name);
}

// On MS-Windows an RGB macro is available and it produces 0x00bbggrr color
// values as used by the MS-Windows GDI api.  It should be used only for
// MS-Windows GDI builds.
# જો defined(RGB) && defined(MSWIN) && !defined(FEAT_GUI)
#  undef RGB
# endif
# ifndef RGB
#  define RGB(r, g, b)	(((r)<<16) | ((g)<<8) | (b))
# endif

# ifdef VIMDLL
    સ્થિર guicolor_T
gui_adjust_rgb(guicolor_T c)
{
    જો (gui.in_use)
	પાછા c;
    નહીંતર
	પાછા ((c & 0xff) << 16) | (c & 0x00ff00) | ((c >> 16) & 0xff);
}
# નહીંતર
#  define gui_adjust_rgb(c) (c)
# endif

    સ્થિર પૂર્ણાંક
hex_digit(પૂર્ણાંક c)
{
    જો (SAFE_isdigit(c))
	પાછા c - '0';
    c = TOLOWER_ASC(c);
    જો (c >= 'a' && c <= 'f')
	પાછા c - 'a' + 10;
    પાછા 0x1ffffff;
}

    સ્થિર guicolor_T
decode_hex_color(char_u *hex)
{
    guicolor_T color;

    જો (hex[0] != '#' || STRLEN(hex) != 7)
	પાછા INVALCOLOR;

    // Name is in "#rrggbb" format
    color = RGB(((hex_digit(hex[1]) << 4) + hex_digit(hex[2])),
		((hex_digit(hex[3]) << 4) + hex_digit(hex[4])),
		((hex_digit(hex[5]) << 4) + hex_digit(hex[6])));
    જો (color > 0xffffff)
	પાછા INVALCOLOR;
    પાછા gui_adjust_rgb(color);
}

# ifdef FEAT_EVAL
// Returns the color currently mapped to the given name or INVALCOLOR if no
// such name exists in the color table. The convention is to use lowercase for
// all keys in the v:colornames dictionary. The value can be either a string in
// the form #rrggbb or a number, either of which is converted to a guicolor_T.
    સ્થિર guicolor_T
colorname2rgb(char_u *name)
{
    dict_T      *colornames_table = get_vim_var_dict(VV_COLORNAMES);
    char_u      *lc_name;
    dictitem_T  *colentry;
    char_u      *colstr;
    varnumber_T colnum;

    lc_name = strlow_save(name);
    જો (lc_name == ખાલી)
	પાછા INVALCOLOR;

    colentry = dict_find(colornames_table, lc_name, -1);
    vim_free(lc_name);
    જો (colentry == ખાલી)
	પાછા INVALCOLOR;

    જો (colentry->di_tv.v_type == VAR_STRING)
    {
	colstr = tv_get_string_strict(&colentry->di_tv);
	જો ((STRLEN(colstr) == 7) && (*colstr == '#'))
	{
	    પાછા decode_hex_color(colstr);
	}
	નહીંતર
	{
	    semsg(_(e_bad_color_string_str), colstr);
	    પાછા INVALCOLOR;
	}
    }

    જો (colentry->di_tv.v_type == VAR_સંખ્યા)
    {
	colnum = tv_get_સંખ્યા(&colentry->di_tv);
	પાછા (guicolor_T)colnum;
    }

    પાછા INVALCOLOR;
}

# endif

    guicolor_T
gui_get_color_cmn(char_u *name)
{
    guicolor_T  color;
    // Only non X11 colors (not present in rgb.txt) and colors in
    // color_name_tab[], useful when $VIMRUNTIME is not found,.
    // must be sorted by the 'value' field because it is used by bsearch()!
    સ્થિર keyvalue_T rgb_tab[] = {
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
    જો (color != INVALCOLOR)
	પાછા color;

    target.ચાવી = 0;
    target.કિંમત.string = name;
    target.કિંમત.length = 0;	// not used, see cmp_keyvalue_value_i()
    entry = (keyvalue_T *)bsearch(&target, &rgb_tab, ARRAY_LENGTH(rgb_tab),
	    કદ(rgb_tab[0]), cmp_keyvalue_કિંમત_i);
    જો (entry != ખાલી)
	પાછા gui_adjust_rgb((guicolor_T)entry->ચાવી);

# જો defined(FEAT_EVAL)
    /*
     * Not a traditional color. Load additional color aliases and then consult the alias table.
     */

    color = colorname2rgb(name);
    જો (color == INVALCOLOR)
    {
	load_default_colors_lists();
	color = colorname2rgb(name);
    }

    પાછા color;
# નહીંતર
    પાછા INVALCOLOR;
# endif
}

    guicolor_T
gui_get_rgb_color_cmn(પૂર્ણાંક r, પૂર્ણાંક g, પૂર્ણાંક b)
{
    guicolor_T  color = RGB(r, g, b);

    જો (color > 0xffffff)
	પાછા INVALCOLOR;
    પાછા gui_adjust_rgb(color);
}
#endif

/*
 * Table with the specifications for an attribute number.
 * Note that this table is used by ALL buffers.  This is required because the
 * GUI can redraw at any time for any buffer.
 */
સ્થિર garray_T	term_attr_table = {0, 0, 0, 0, ખાલી};

#define TERM_ATTR_ENTRY(idx) ((attrentry_T *)term_attr_table.ga_data)[idx]

સ્થિર garray_T	cterm_attr_table = {0, 0, 0, 0, ખાલી};

#define CTERM_ATTR_ENTRY(idx) ((attrentry_T *)cterm_attr_table.ga_data)[idx]

#ifdef FEAT_GUI
સ્થિર garray_T	gui_attr_table = {0, 0, 0, 0, ખાલી};

# define GUI_ATTR_ENTRY(idx) ((attrentry_T *)gui_attr_table.ga_data)[idx]
#endif

/*
 * Return the attr number for a set of colors and font.
 * Add a new entry to the term_attr_table, cterm_attr_table or gui_attr_table
 * if the combination is new.
 * Return 0 for error (no more room).
 */
    સ્થિર પૂર્ણાંક
get_attr_entry(garray_T *table, attrentry_T *aep)
{
    પૂર્ણાંક		i;
    attrentry_T	*taep;
    સ્થિર પૂર્ણાંક	recursive = FALSE;

    // Init the table, in case it wasn't done yet.
    table->ga_itemsize = કદ(attrentry_T);
    table->ga_growsize = 7;

    // Try to find an entry with the same specifications.
    માટે (i = 0; i < table->ga_len; ++i)
    {
	taep = &(((attrentry_T *)table->ga_data)[i]);
	જો (	   aep->ae_attr == taep->ae_attr
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
			&& (aep->ae_u.term.start == ખાલી)
					    == (taep->ae_u.term.start == ખાલી)
			&& (aep->ae_u.term.start == ખાલી
			    || STRCMP(aep->ae_u.term.start,
						  taep->ae_u.term.start) == 0)
			&& (aep->ae_u.term.stop == ખાલી)
					     == (taep->ae_u.term.stop == ખાલી)
			&& (aep->ae_u.term.stop == ખાલી
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

	પાછા i + ATTR_OFF;
    }

    જો (table->ga_len + ATTR_OFF > MAX_TYPENR)
    {
	// Running out of attribute entries!  remove all attributes, and
	// compute new ones for all groups.
	// When called recursively, we are really out of numbers.
	જો (recursive)
	{
	    emsg(_(e_too_many_different_highlighting_attributes_in_use));
	    પાછા 0;
	}
	recursive = TRUE;

	clear_hl_tables();

	set_must_redraw(UPD_CLEAR);

	માટે (i = 0; i < highlight_ga.ga_len; ++i)
	    set_hl_attr(i);

	recursive = FALSE;
    }

    // This is a new combination of colors and font, add an entry.
    જો (ga_grow(table, 1) == FAIL)
	પાછા 0;

    taep = &(((attrentry_T *)table->ga_data)[table->ga_len]);
    CLEAR_POINTER(taep);
    taep->ae_attr = aep->ae_attr;
#ifdef FEAT_GUI
    જો (table == &gui_attr_table)
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
    જો (table == &term_attr_table)
    {
	જો (aep->ae_u.term.start == ખાલી)
	    taep->ae_u.term.start = ખાલી;
	નહીંતર
	    taep->ae_u.term.start = vim_strsave(aep->ae_u.term.start);
	જો (aep->ae_u.term.stop == ખાલી)
	    taep->ae_u.term.stop = ખાલી;
	નહીંતર
	    taep->ae_u.term.stop = vim_strsave(aep->ae_u.term.stop);
    }
    નહીંતર જો (table == &cterm_attr_table)
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
    પાછા (table->ga_len - 1 + ATTR_OFF);
}

#જો defined(FEAT_TERMINAL)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    પૂર્ણાંક
get_cterm_attr_idx(પૂર્ણાંક attr, પૂર્ણાંક fg, પૂર્ણાંક bg)
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
    પાછા get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#જો defined(FEAT_TERMINAL) && defined(FEAT_TERMGUICOLORS)
/*
 * Get an attribute index for a 'termguicolors' entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    પૂર્ણાંક
get_tgc_attr_idx(પૂર્ણાંક attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    જો (fg == INVALCOLOR && bg == INVALCOLOR)
    {
	// If both GUI colors are not set fall back to the cterm colors.  Helps
	// if the GUI only has an attribute, such as undercurl.
	at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
    }
    નહીંતર
    {
	at_en.ae_u.cterm.fg_rgb = fg;
	at_en.ae_u.cterm.bg_rgb = bg;
    }
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
    પાછા get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#જો defined(FEAT_TERMINAL) && defined(FEAT_GUI)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    પૂર્ણાંક
get_gui_attr_idx(પૂર્ણાંક attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    at_en.ae_u.gui.fg_color = fg;
    at_en.ae_u.gui.bg_color = bg;
    પાછા get_attr_entry(&gui_attr_table, &at_en);
}
#endif

/*
 * Clear all highlight tables.
 */
    શૂન્ય
clear_hl_tables(શૂન્ય)
{
    પૂર્ણાંક		i;
    attrentry_T	*taep;

#ifdef FEAT_GUI
    ga_clear(&gui_attr_table);
#endif
    માટે (i = 0; i < term_attr_table.ga_len; ++i)
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
    પૂર્ણાંક
hl_combine_attr(પૂર્ણાંક char_attr, પૂર્ણાંક prim_attr)
{
    attrentry_T *char_aep = ખાલી;
    attrentry_T *prim_aep;
    attrentry_T new_en;

    જો (char_attr == 0)
	પાછા prim_attr;
    જો (char_attr <= HL_ALL && prim_attr <= HL_ALL)
	પાછા ATTR_COMBINE(char_attr, prim_attr);
#ifdef FEAT_GUI
    જો (gui.in_use)
    {
	જો (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	જો (char_aep != ખાલી)
	    new_en = *char_aep;
	નહીંતર
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    જો (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	જો (prim_attr <= HL_ALL)
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	નહીંતર
	{
	    prim_aep = syn_gui_attr2entry(prim_attr);
	    જો (prim_aep != ખાલી)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		જો (prim_aep->ae_u.gui.fg_color != INVALCOLOR)
		    new_en.ae_u.gui.fg_color = prim_aep->ae_u.gui.fg_color;
		જો (prim_aep->ae_u.gui.bg_color != INVALCOLOR)
		    new_en.ae_u.gui.bg_color = prim_aep->ae_u.gui.bg_color;
		જો (prim_aep->ae_u.gui.sp_color != INVALCOLOR)
		    new_en.ae_u.gui.sp_color = prim_aep->ae_u.gui.sp_color;
		જો (prim_aep->ae_u.gui.font != NOFONT)
		    new_en.ae_u.gui.font = prim_aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
		જો (prim_aep->ae_u.gui.fontset != NOFONTSET)
		    new_en.ae_u.gui.fontset = prim_aep->ae_u.gui.fontset;
# endif
	    }
	}
	પાછા get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    જો (IS_CTERM)
    {
	જો (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	જો (char_aep != ખાલી)
	    new_en = *char_aep;
	નહીંતર
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    જો (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	જો (prim_attr <= HL_ALL)
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	નહીંતર
	{
	    prim_aep = syn_cterm_attr2entry(prim_attr);
	    જો (prim_aep != ખાલી)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		જો (prim_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color = prim_aep->ae_u.cterm.fg_color;
		જો (prim_aep->ae_u.cterm.bg_color > 0)
		    new_en.ae_u.cterm.bg_color = prim_aep->ae_u.cterm.bg_color;
		જો (prim_aep->ae_u.cterm.ul_color > 0)
		    new_en.ae_u.cterm.ul_color = prim_aep->ae_u.cterm.ul_color;
		જો (prim_aep->ae_u.cterm.font > 0)
		    new_en.ae_u.cterm.font = prim_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		// If both fg and bg are not set fall back to cterm colors.
		// Helps for SpellBad which uses undercurl in the GUI.
		જો (COLOR_INVALID(prim_aep->ae_u.cterm.fg_rgb)
			&& COLOR_INVALID(prim_aep->ae_u.cterm.bg_rgb))
		{
		    જો (prim_aep->ae_u.cterm.fg_color > 0)
			new_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
		    જો (prim_aep->ae_u.cterm.bg_color > 0)
			new_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
		}
		નહીંતર
		{
		    જો (prim_aep->ae_u.cterm.fg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.fg_rgb = prim_aep->ae_u.cterm.fg_rgb;
		    જો (prim_aep->ae_u.cterm.bg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.bg_rgb = prim_aep->ae_u.cterm.bg_rgb;
		}
		જો (prim_aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		    new_en.ae_u.cterm.ul_rgb = prim_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	}
	પાછા get_attr_entry(&cterm_attr_table, &new_en);
    }

    જો (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    જો (char_aep != ખાલી)
	new_en = *char_aep;
    નહીંતર
    {
	CLEAR_FIELD(new_en);
	જો (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    જો (prim_attr <= HL_ALL)
	new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
    નહીંતર
    {
	prim_aep = syn_term_attr2entry(prim_attr);
	જો (prim_aep != ખાલી)
	{
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_aep->ae_attr);
	    જો (prim_aep->ae_u.term.start != ખાલી)
	    {
		new_en.ae_u.term.start = prim_aep->ae_u.term.start;
		new_en.ae_u.term.stop = prim_aep->ae_u.term.stop;
	    }
	}
    }
    પાછા get_attr_entry(&term_attr_table, &new_en);
}

// ANSI color order: Black, Red, Green, Yellow, Blue, Magenta,
// Cyan, White, then bright variants.  Approximate RGB values used when
// only a cterm color number is known (no guifg/guibg).  Real terminal
// palettes may differ if the user reconfigured their emulator, but
// these are reasonable xterm-ish defaults.
સ્થિર અચલ long_u cterm_color_16[16] = {
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
    સ્થિર guicolor_T
cterm_color_to_rgb(પૂર્ણાંક color_nr)
{
    જો (color_nr < 1 || color_nr > 16)
	પાછા INVALCOLOR;
    પાછા (guicolor_T)cterm_color_16[color_nr - 1];
}
#endif

/*
 * Convert an xterm 256-color index (0-255) to an approximate RGB triple.
 * Uses the standard xterm palette: 0-15 ANSI (cterm_color_16), 16-231
 * 6x6x6 cube, 232-255 grayscale ramp.
 */
    સ્થિર શૂન્ય
cterm_idx_to_rgb(પૂર્ણાંક idx, પૂર્ણાંક *r, પૂર્ણાંક *g, પૂર્ણાંક *b)
{
    સ્થિર અચલ પૂર્ણાંક cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

    જો (idx < 0 || idx > 255)
    {
	*r = *g = *b = 0;
	પાછા;
    }
    જો (idx < 16)
    {
	long_u rgb = cterm_color_16[idx];
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
    }
    નહીંતર જો (idx < 232)
    {
	પૂર્ણાંક n = idx - 16;
	*r = cube[n / 36 % 6];
	*g = cube[n / 6  % 6];
	*b = cube[n      % 6];
    }
    નહીંતર
    {
	પૂર્ણાંક v = 8 + (idx - 232) * 10;
	*r = *g = *b = v;
    }
}

/*
 * Approximate an RGB triple to the nearest xterm 256-color index.
 * Searches the 6x6x6 cube and the grayscale ramp; returns 0-255.
 */
    સ્થિર પૂર્ણાંક
rgb_to_cterm_idx(પૂર્ણાંક r, પૂર્ણાંક g, પૂર્ણાંક b)
{
    સ્થિર અચલ પૂર્ણાંક cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };
    પૂર્ણાંક best = 0;
    long best_d = -1;
    પૂર્ણાંક i;

    // Search the 6x6x6 cube.
    માટે (i = 16; i < 232; ++i)
    {
	પૂર્ણાંક n = i - 16;
	પૂર્ણાંક cr = cube[n / 36 % 6];
	પૂર્ણાંક cg = cube[n / 6  % 6];
	પૂર્ણાંક cb = cube[n      % 6];
	long d = (cr - r) * (cr - r) + (cg - g) * (cg - g) + (cb - b) * (cb - b);
	જો (best_d < 0 || d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    // Search the grayscale ramp.
    માટે (i = 232; i < 256; ++i)
    {
	પૂર્ણાંક v = 8 + (i - 232) * 10;
	long d = (v - r) * (v - r) + (v - g) * (v - g) + (v - b) * (v - b);
	જો (d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    પાછા best;
}

/*
 * Resolve a color to an RGB triple.  Prefer the gui RGB value (set by
 * guifg/guibg) when valid, otherwise convert the cterm 256-color index.
 * Returns true on success; false when no color is defined.
 * cterm_c is 1-based (0 means "no color").
 */
    સ્થિર bool
resolve_color_to_rgb(પૂર્ણાંક cterm_c, guicolor_T rgb UNUSED, પૂર્ણાંક *r, પૂર્ણાંક *g, પૂર્ણાંક *b)
{
#ifdef FEAT_TERMGUICOLORS
    જો (!COLOR_INVALID(rgb))
    {
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
	પાછા true;
    }
#endif
    જો (cterm_c > 0)
    {
	cterm_idx_to_rgb(cterm_c - 1, r, g, b);
	પાછા true;
    }
    પાછા false;
}

/*
 * get a RGB fallback color from gui, cterm or default color
 */
    સ્થિર guicolor_T
resolve_fallback_color(પૂર્ણાંક cterm_c, guicolor_T rgb, guicolor_T default_rgb)
{
    પૂર્ણાંક red, green, blue;
    જો (!resolve_color_to_rgb(cterm_c, rgb, &red, &green, &blue))
	પાછા default_rgb;
    નહીંતર
	પાછા (red << 16) | (green << 8) | blue;
}

/*
 * get a RGB fallback foreground color from guifg, ctermfg or deduced from background
 */
    સ્થિર શૂન્ય
resolve_fallback_fg_to_rgb(શૂન્ય)
{
    guicolor_T fgcolor_or_gui_fgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    fgcolor_or_gui_fgcolor = cterm_normal_fg_gui_color;
#endif
#ifdef FEAT_GUI
    જો (gui.in_use)
	fgcolor_or_gui_fgcolor = gui.norm_pixel;
#endif
    fallback_fg_rgb = resolve_fallback_color(cterm_normal_fg_color, fgcolor_or_gui_fgcolor, (*p_bg == 'l') ? 0x000000 : 0xFFFFFF);
}

/*
 * get a RGB fallback background color from guifg, ctermbg or deduced from background
 */
    સ્થિર શૂન્ય
resolve_fallback_bg_to_rgb(શૂન્ય)
{
    guicolor_T bgcolor_or_gui_bgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    bgcolor_or_gui_bgcolor = cterm_normal_bg_gui_color;
#endif
#ifdef FEAT_GUI
    જો (gui.in_use)
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
    સ્થિર પૂર્ણાંક
blend_cterm_colors(પૂર્ણાંક popup_c, guicolor_T popup_rgb,
		   પૂર્ણાંક under_c, guicolor_T under_rgb,
		   પૂર્ણાંક default_rgb,
		   પૂર્ણાંક blend_val)
{
    પૂર્ણાંક pr, pg, pb, ur, ug, ub, r, g, b;

    જો (t_colors < 256)
	પાછા popup_c;
    જો (!resolve_color_to_rgb(popup_c, popup_rgb, &pr, &pg, &pb))
	પાછા under_c;
    જો (blend_val <= 0)
	પાછા rgb_to_cterm_idx(pr, pg, pb) + 1;
    જો (!resolve_color_to_rgb(under_c, under_rgb, &ur, &ug, &ub))
    {
	ur = (default_rgb >> 16) & 0xFF;
	ug = (default_rgb >> 8) & 0xFF;
	ub = default_rgb & 0xFF;
    }
    જો (blend_val >= 100)
	પાછા rgb_to_cterm_idx(ur, ug, ub) + 1;
    r = pr + (ur - pr) * blend_val / 100;
    g = pg + (ug - pg) * blend_val / 100;
    b = pb + (ub - pb) * blend_val / 100;
    પાછા rgb_to_cterm_idx(r, g, b) + 1;
}

#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Blend two RGB colors based on blend value (0-100).
 * blend: 0=return first argument color, 100=return second argument color
 * If bg_color is INVALCOLOR, high blend means more visible (return INVALCOLOR).
 */
    સ્થિર guicolor_T
blend_colors(guicolor_T popup_color, guicolor_T bg_color, પૂર્ણાંક blend_val)
{
    પૂર્ણાંક r1, g1, b1, r2, g2, b2, r, g, b;

    // CTERMCOLOR is a sentinel meaning "use the cterm color"; for blending
    // it has no real RGB so treat it like INVALCOLOR.
    જો (COLOR_INVALID(popup_color))
	પાછા INVALCOLOR;

    // Fully transparent: use underlying color as-is.
    જો (blend_val >= 100)
	પાછા bg_color;

    r1 = (popup_color >> 16) & 0xFF;
    g1 = (popup_color >> 8) & 0xFF;
    b1 = popup_color & 0xFF;

    જો (COLOR_INVALID(bg_color))
	bg_color = fallback_bg_rgb;

    r2 = (bg_color >> 16) & 0xFF;
    g2 = (bg_color >> 8) & 0xFF;
    b2 = bg_color & 0xFF;

    r = r1 + (r2 - r1) * blend_val / 100;
    g = g1 + (g2 - g1) * blend_val / 100;
    b = b1 + (b2 - b1) * blend_val / 100;

    પાછા (r << 16) | (g << 8) | b;
}
#endif

/*
 * Blend attributes for popup windows and menus with opacity.
 * Blends foreground and/or background colors based on blend value (0-100).
 * blend: 0 = opaque (use popup colors), 100 = transparent (use background colors)
 * blend_fg: TRUE to blend foreground color, FALSE to keep popup foreground
 */
    સ્થિર પૂર્ણાંક
hl_blend_attr_common(
    પૂર્ણાંક		char_attr,
    પૂર્ણાંક		popup_attr,
    પૂર્ણાંક		blend,
    પૂર્ણાંક		blend_fg)
{
    attrentry_T *char_aep = ખાલી;
    attrentry_T *popup_aep;
    attrentry_T new_en;
    attrentry_T tmp_en;

#ifdef FEAT_GUI
    જો (gui.in_use)
    {
	જો (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	જો (char_aep != ખાલી)
	    new_en = *char_aep;
	નહીંતર
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    જો (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	જો (popup_attr <= HL_ALL)
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
	નહીંતર
	    popup_aep = syn_gui_attr2entry(popup_attr);

	જો (popup_aep != ખાલી)
	{
	    guicolor_T popup_bg_rgb = popup_aep->ae_u.gui.bg_color;
	    જો (COLOR_INVALID(popup_bg_rgb))
		popup_bg_rgb = fallback_bg_rgb;

	    જો (blend_fg)
	    {
		// blend_fg=TRUE: fade underlying text toward popup bg.
		{
		    પૂર્ણાંક base_fg = fallback_fg_rgb;
		    જો (char_aep != ખાલી
			    && char_aep->ae_u.gui.fg_color != INVALCOLOR)
			base_fg = char_aep->ae_u.gui.fg_color;
		    new_en.ae_u.gui.fg_color = blend_colors(
			    popup_bg_rgb, base_fg, blend);
		}
	    }
	    નહીંતર
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
		જો (char_aep != ખાલી
			&& !COLOR_INVALID(char_aep->ae_u.gui.bg_color))
		    underlying_bg = char_aep->ae_u.gui.bg_color;
		new_en.ae_u.gui.bg_color = blend_colors(
			popup_bg_rgb,
			underlying_bg, blend);
	    }
	}
	પાછા get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    જો (IS_CTERM)
    {
	જો (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	જો (char_aep != ખાલી)
	    new_en = *char_aep;
	નહીંતર
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    જો (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	જો (popup_attr <= HL_ALL)
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
	નહીંતર
	    popup_aep = syn_cterm_attr2entry(popup_attr);

	જો (popup_aep != ખાલી)
	{
	    guicolor_T popup_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
	    // Fall back to cterm color converted to RGB when gui color is not set.
	    popup_bg_rgb = popup_aep->ae_u.cterm.bg_rgb;
	    જો (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color > 0)
		popup_bg_rgb = cterm_color_to_rgb(
			popup_aep->ae_u.cterm.bg_color);
#endif
	    // assign default color if guibg and ctermbg are not set for popup
	    જો (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color == 0)
		popup_bg_rgb = fallback_bg_rgb;

	    જો (!blend_fg)
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
		જો (popup_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color =
			popup_aep->ae_u.cterm.fg_color;
		નહીંતર જો (cterm_normal_fg_color > 0)
		    new_en.ae_u.cterm.fg_color = cterm_normal_fg_color;
		નહીંતર
		    // black-ish or white-ish
		    new_en.ae_u.cterm.fg_color = (*p_bg == 'l') ? 1 : 16;
		new_en.ae_u.cterm.ul_color = popup_aep->ae_u.cterm.ul_color;
		new_en.ae_u.cterm.font = popup_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		new_en.ae_u.cterm.ul_rgb = popup_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	    નહીંતર
	    {
		// blend_fg=TRUE: fade underlying fg toward popup bg in
		// the 256-color palette.  Used when the popup is over a
		// cell rendered with cterm colors (no termguicolors RGB).
		પૂર્ણાંક under_fg = (char_aep != ખાલી)
		    ? char_aep->ae_u.cterm.fg_color : 0;
		guicolor_T under_fg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		જો (char_aep != ખાલી)
		    under_fg_rgb = char_aep->ae_u.cterm.fg_rgb;
#endif
		new_en.ae_u.cterm.fg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_fg, under_fg_rgb, fallback_fg_rgb, blend);
	    }
	    // Approximate cterm bg by blending with the underlying bg
	    // in the 256-color palette and mapping to the nearest entry.
	    {
		પૂર્ણાંક under_bg = (char_aep != ખાલી)
		    ? char_aep->ae_u.cterm.bg_color : 0;
		guicolor_T under_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		જો (char_aep != ખાલી)
		    under_bg_rgb = char_aep->ae_u.cterm.bg_rgb;
#endif
		new_en.ae_u.cterm.bg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_bg, under_bg_rgb, fallback_bg_rgb, blend);
	    }
#ifdef FEAT_TERMGUICOLORS
	    // Blend RGB colors for termguicolors mode.
	    {
		જો (blend_fg)
		{
		    // blend_fg=TRUE: fade underlying text toward popup bg.
		    જો (popup_bg_rgb != INVALCOLOR)
		    {
			પૂર્ણાંક base_fg = fallback_fg_rgb;
			// CTERMCOLOR is a sentinel meaning "use the cterm
			// color"; treat it as no underlying color so it is
			// not blended in as a real near-white pixel.
			જો (char_aep != ખાલી
				&& !COLOR_INVALID(char_aep->ae_u.cterm.fg_rgb))
			    base_fg = char_aep->ae_u.cterm.fg_rgb;
			new_en.ae_u.cterm.fg_rgb = blend_colors(
				popup_bg_rgb, base_fg, blend);
		    }
		}
		નહીંતર
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

		    જો (COLOR_INVALID(popup_fg)
			    && popup_aep->ae_u.cterm.fg_color > 0)
			popup_fg = cterm_color_to_rgb(
				popup_aep->ae_u.cterm.fg_color);

		    જો (!COLOR_INVALID(popup_fg))
			new_en.ae_u.cterm.fg_rgb = popup_fg;
		    નહીંતર જો (!COLOR_INVALID(cterm_normal_fg_gui_color))
			new_en.ae_u.cterm.fg_rgb = cterm_normal_fg_gui_color;
		    નહીંતર
			new_en.ae_u.cterm.fg_rgb = fallback_fg_rgb;
		}
		જો (popup_bg_rgb != INVALCOLOR)
		{
		    // Blend popup bg toward underlying bg
		    guicolor_T underlying_bg = fallback_bg_rgb;
		    જો (char_aep != ખાલી
			    && !COLOR_INVALID(char_aep->ae_u.cterm.bg_rgb))
			underlying_bg = char_aep->ae_u.cterm.bg_rgb;
		    new_en.ae_u.cterm.bg_rgb = blend_colors(
			    popup_bg_rgb, underlying_bg, blend);
		}
	    }
#endif
	}
	પાછા get_attr_entry(&cterm_attr_table, &new_en);
    }

    જો (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    જો (char_aep != ખાલી)
	new_en = *char_aep;
    નહીંતર
    {
	CLEAR_FIELD(new_en);
	જો (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    // For term mode, no separate background color handling.
    પાછા get_attr_entry(&term_attr_table, &new_en);
}

/*
 * Blend attributes for popup windows with opacity.
 */
    પૂર્ણાંક
hl_blend_attr(પૂર્ણાંક char_attr, પૂર્ણાંક popup_attr, પૂર્ણાંક blend, પૂર્ણાંક blend_fg)
{
    જો (blend >= 100 && blend_fg)
	પાછા char_attr;

    પાછા hl_blend_attr_common(char_attr, popup_attr, blend, blend_fg);
}

/*
 * Blend underlying foreground and background colors for pum space cells.
 */
    પૂર્ણાંક
hl_pum_blend_attr(પૂર્ણાંક char_attr, પૂર્ણાંક popup_attr, પૂર્ણાંક blend)
{
    પાછા hl_blend_attr_common(char_attr, popup_attr, blend, TRUE);
}

#ifdef FEAT_GUI
    attrentry_T *
syn_gui_attr2entry(પૂર્ણાંક attr)
{
    attr -= ATTR_OFF;
    જો (attr >= gui_attr_table.ga_len)	    // did ":syntax clear"
	પાછા ખાલી;
    પાછા &(GUI_ATTR_ENTRY(attr));
}
#endif

/*
 * Get the highlight attributes (HL_BOLD etc.) from an attribute nr.
 * Only to be used when "attr" > HL_ALL.
 */
    પૂર્ણાંક
syn_attr2attr(પૂર્ણાંક attr)
{
    attrentry_T	*aep;

#ifdef FEAT_GUI
    જો (gui.in_use)
	aep = syn_gui_attr2entry(attr);
    નહીંતર
#endif
	જો (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	નહીંતર
	    aep = syn_term_attr2entry(attr);

    જો (aep == ખાલી)	    // highlighting not set
	પાછા 0;
    પાછા aep->ae_attr;
}


    attrentry_T *
syn_term_attr2entry(પૂર્ણાંક attr)
{
    attr -= ATTR_OFF;
    જો (attr >= term_attr_table.ga_len)	    // did ":syntax clear"
	પાછા ખાલી;
    પાછા &(TERM_ATTR_ENTRY(attr));
}

    attrentry_T *
syn_cterm_attr2entry(પૂર્ણાંક attr)
{
    attr -= ATTR_OFF;
    જો (attr >= cterm_attr_table.ga_len)	// did ":syntax clear"
	પાછા ખાલી;
    પાછા &(CTERM_ATTR_ENTRY(attr));
}

#define LIST_ATTR   1
#define LIST_STRING 2
#define LIST_INT    3

    સ્થિર શૂન્ય
highlight_list_one(પૂર્ણાંક id)
{
    hl_group_T	    *sgp;
    પૂર્ણાંક		    didh = FALSE;

    sgp = &HL_TABLE()[id - 1];	    // index is ID minus one

    જો (message_filtered(sgp->sg_name))
	પાછા;

    // Note: Keep this in sync with expand_highlight_group().
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_term, ખાલી, "term");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_start, "start");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_stop, "stop");

    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_cterm, ખાલી, "cterm");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_fg, ખાલી, "ctermfg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_bg, ખાલી, "ctermbg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_ul, ખાલી, "ctermul");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_font, ખાલી, "ctermfont");

#જો defined(FEAT_GUI) || defined(FEAT_EVAL)
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_gui, ખાલી, "gui");
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

    જો (sgp->sg_link && !got_int)
    {
	(શૂન્ય)syn_list_header(didh, 9999, id);
	didh = TRUE;
	સંદેશ_puts_attr("links to", HL_ATTR(HLF_D));
	સંદેશ_putchar(' ');
	સંદેશ_outtrans(HL_TABLE()[HL_TABLE()[id - 1].sg_link - 1].sg_name);
    }

    જો (!didh)
	highlight_list_arg(id, didh, LIST_STRING, 0, (char_u *)"cleared", "");
#ifdef FEAT_EVAL
    જો (p_verbose > 0)
	last_set_સંદેશ(sgp->sg_script_ctx);
#endif
}

    સ્થિર char_u*
highlight_arg_to_string(
    પૂર્ણાંક		type,
    પૂર્ણાંક		iarg,
    char_u	*sarg,
    char_u	*buf)
{
    જો (type == LIST_INT)
	sprintf((અક્ષર *)buf, "%d", iarg - 1);
    નહીંતર જો (type == LIST_STRING)
	પાછા sarg;
    નહીંતર // type == LIST_ATTR
    {
	size_t buflen;

	buf[0] = NUL;
	buflen = 0;
	માટે (પૂર્ણાંક i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(highlight_index_tab); ++i)
	{
	    જો (iarg & highlight_index_tab[i]->ચાવી)
	    {
		જો (buflen > 0)
		{
		    STRCPY(buf + buflen, (char_u *)",");
		    ++buflen;
		}
		STRCPY(buf + buflen, highlight_index_tab[i]->કિંમત.string);
		buflen += highlight_index_tab[i]->કિંમત.length;
		iarg &= ~highlight_index_tab[i]->ચાવી;	    // don't want "inverse"/"reverse"
	    }
	}
    }
    પાછા buf;
}

    સ્થિર પૂર્ણાંક
highlight_list_arg(
    પૂર્ણાંક		id,
    પૂર્ણાંક		didh,
    પૂર્ણાંક		type,
    પૂર્ણાંક		iarg,
    char_u	*sarg,
    અક્ષર	*name)
{
    char_u	buf[MAX_ATTR_LEN];
    char_u	*ts;

    જો (got_int)
	પાછા FALSE;

    જો (type == LIST_STRING ? (sarg == ખાલી) : (iarg == 0))
	પાછા didh;

    ts = highlight_arg_to_string(type, iarg, sarg, buf);

    (શૂન્ય)syn_list_header(didh,
	    (પૂર્ણાંક)(vim_strsize(ts) + STRLEN(name) + 1), id);
    didh = TRUE;
    જો (!got_int)
    {
	જો (*name != NUL)
	{
	    સંદેશ_puts_attr(name, HL_ATTR(HLF_D));
	    સંદેશ_puts_attr("=", HL_ATTR(HLF_D));
	}
	સંદેશ_outtrans(ts);
    }
    પાછા didh;
}

#જો ((defined(FEAT_EVAL) || defined(FEAT_PRINTER))) && defined(FEAT_SYN_HL)
/*
 * Return "1" if highlight group "id" has attribute "flag".
 * Return NULL otherwise.
 */
    char_u *
highlight_has_attr(
    પૂર્ણાંક		id,
    પૂર્ણાંક		flag,
    પૂર્ણાંક		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    પૂર્ણાંક		attr;

    જો (id <= 0 || id > highlight_ga.ga_len)
	પાછા ખાલી;

# જો defined(FEAT_GUI) || defined(FEAT_EVAL)
    જો (modec == 'g')
	attr = HL_TABLE()[id - 1].sg_gui;
    નહીંતર
# endif
    {
	જો (modec == 'c')
	    attr = HL_TABLE()[id - 1].sg_cterm;
	નહીંતર
	    attr = HL_TABLE()[id - 1].sg_term;
    }

    જો (attr & flag)
	પાછા (char_u *)"1";
    પાછા ખાલી;
}
#endif

#જો defined(FEAT_SYN_HL) && defined(FEAT_EVAL)
/*
 * Return color name of highlight group "id".
 */
    char_u *
highlight_color(
    પૂર્ણાંક		id,
    char_u	*what,	// "font", "fg", "bg", "sp", "ul", "fg#", "bg#" or "sp#"
    પૂર્ણાંક		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    સ્થિર char_u	name[20];
    પૂર્ણાંક			n;
    પૂર્ણાંક			fg = FALSE;
    પૂર્ણાંક			sp = FALSE;
    પૂર્ણાંક			ul = FALSE;
    પૂર્ણાંક			font = FALSE;

    જો (id <= 0 || id > highlight_ga.ga_len)
	પાછા ખાલી;

    જો (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'g')
	fg = TRUE;
    નહીંતર જો (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'o'
	     && TOLOWER_ASC(what[2]) == 'n' && TOLOWER_ASC(what[3]) == 't')
	font = TRUE;
    નહીંતર જો (TOLOWER_ASC(what[0]) == 's' && TOLOWER_ASC(what[1]) == 'p')
	sp = TRUE;
    નહીંતર જો (TOLOWER_ASC(what[0]) == 'u' && TOLOWER_ASC(what[1]) == 'l')
	ul = TRUE;
    નહીંતર જો (!(TOLOWER_ASC(what[0]) == 'b' && TOLOWER_ASC(what[1]) == 'g'))
	પાછા ખાલી;
    જો (modec == 'g')
    {
# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI
	// return font name
	જો (font)
	    પાછા HL_TABLE()[id - 1].sg_font_name;
#  endif

	// return #RRGGBB form (only possible when GUI is running)
	જો ((USE_24BIT) && what[2] == '#')
	{
	    guicolor_T		color;
	    long_u		rgb;
	    સ્થિર char_u	buf[10];

	    જો (fg)
		color = HL_TABLE()[id - 1].sg_gui_fg;
	    નહીંતર જો (sp)
		color = HL_TABLE()[id - 1].sg_gui_sp;
	    નહીંતર
		color = HL_TABLE()[id - 1].sg_gui_bg;
	    જો (color == INVALCOLOR)
		પાછા ખાલી;
	    rgb = (long_u)GUI_MCH_GET_RGB(color);
	    sprintf((અક્ષર *)buf, "#%02x%02x%02x",
				      (unsigned)(rgb >> 16),
				      (unsigned)(rgb >> 8) & 255,
				      (unsigned)rgb & 255);
	    પાછા buf;
	}
# endif
	જો (fg)
	    પાછા (HL_TABLE()[id - 1].sg_gui_fg_name);
	જો (sp)
	    પાછા (HL_TABLE()[id - 1].sg_gui_sp_name);
	પાછા (HL_TABLE()[id - 1].sg_gui_bg_name);
    }
    જો (sp)
	પાછા ખાલી;
    જો (modec == 'c')
    {
	જો (fg)
	    n = HL_TABLE()[id - 1].sg_cterm_fg - 1;
	નહીંતર જો (ul)
	    n = HL_TABLE()[id - 1].sg_cterm_ul - 1;
	નહીંતર જો (font)
	    n = HL_TABLE()[id - 1].sg_cterm_font - 1;
	નહીંતર
	    n = HL_TABLE()[id - 1].sg_cterm_bg - 1;
	જો (n < 0)
	    પાછા ખાલી;
	sprintf((અક્ષર *)name, "%d", n);
	પાછા name;
    }
    // term doesn't have color
    પાછા ખાલી;
}
#endif

#જો defined(FEAT_SYN_HL) \
	    && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)) \
	&& defined(FEAT_PRINTER)
/*
 * Return color name of highlight group "id" as RGB value.
 */
    long_u
highlight_gui_color_rgb(
    પૂર્ણાંક		id,
    પૂર્ણાંક		fg)	// TRUE = fg, FALSE = bg
{
    guicolor_T	color;

    જો (id <= 0 || id > highlight_ga.ga_len)
	પાછા 0L;

    જો (fg)
	color = HL_TABLE()[id - 1].sg_gui_fg;
    નહીંતર
	color = HL_TABLE()[id - 1].sg_gui_bg;

    જો (color == INVALCOLOR)
	પાછા 0L;

    પાછા GUI_MCH_GET_RGB(color);
}
#endif

/*
 * Output the syntax list header.
 * Return TRUE when started a new line.
 */
    પૂર્ણાંક
syn_list_header(
    પૂર્ણાંક	    did_header,		// did header already
    પૂર્ણાંક	    outlen,		// length of string that comes
    પૂર્ણાંક	    id)			// highlight group id
{
    પૂર્ણાંક	    endcol = 19;
    પૂર્ણાંક	    newline = TRUE;
    પૂર્ણાંક	    name_col = 0;

    જો (!did_header)
    {
	સંદેશ_putchar('\n');
	જો (got_int)
	    પાછા TRUE;
	સંદેશ_outtrans(HL_TABLE()[id - 1].sg_name);
	name_col = સંદેશ_col;
	endcol = 15;
    }
    નહીંતર જો (સંદેશ_col + outlen + 1 >= cmdline_width)
    {
	સંદેશ_putchar('\n');
	જો (got_int)
	    પાછા TRUE;
    }
    નહીંતર
    {
	જો (સંદેશ_col >= endcol)	// wrap around is like starting a new line
	    newline = FALSE;
    }

    જો (સંદેશ_col >= endcol)	// output at least one space
	endcol = સંદેશ_col + 1;
    જો (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    સંદેશ_advance(endcol);

    // Show "xxx" with the attributes.
    જો (!did_header)
    {
	જો (endcol == cmdline_width - 1 && endcol <= name_col)
	    સંદેશ_putchar(' ');
	સંદેશ_puts_attr("xxx", syn_id2attr(id));
	સંદેશ_putchar(' ');
    }

    પાછા newline;
}

/*
 * Set the attribute numbers for a highlight group.
 * Called after one of the attributes has changed.
 */
    સ્થિર શૂન્ય
set_hl_attr(
    પૂર્ણાંક		idx)	    // index in array
{
    attrentry_T	    at_en;
    hl_group_T	    *sgp = HL_TABLE() + idx;

    // The "Normal" group doesn't need an attribute number
    જો (sgp->sg_name_u != ખાલી && STRCMP(sgp->sg_name_u, "NORMAL") == 0)
	પાછા;

#ifdef FEAT_GUI
    // For the GUI mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    જો (sgp->sg_gui_fg == INVALCOLOR
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
    નહીંતર
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
    જો (sgp->sg_start == ખાલી && sgp->sg_stop == ખાલી)
	sgp->sg_term_attr = sgp->sg_term;
    નહીંતર
    {
	at_en.ae_attr = sgp->sg_term;
	at_en.ae_u.term.start = sgp->sg_start;
	at_en.ae_u.term.stop = sgp->sg_stop;
	sgp->sg_term_attr = get_attr_entry(&term_attr_table, &at_en);
    }

    // For the color term mode: If there are other than "normal"
    // highlighting attributes, need to allocate an attr number.
    જો (sgp->sg_cterm_fg == 0 && sgp->sg_cterm_bg == 0 &&
	sgp->sg_cterm_ul == 0 && sgp->sg_cterm_font == 0
#ifdef FEAT_TERMGUICOLORS
	    && sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
#endif
	    )
	sgp->sg_cterm_attr = sgp->sg_cterm;
    નહીંતર
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
	જો (sgp->sg_cterm & (HL_UNDERLINE | HL_UNDERCURL
			   | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED))
	    at_en.ae_u.cterm.ul_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_sp);
	નહીંતર
	    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	જો (at_en.ae_u.cterm.fg_rgb == INVALCOLOR
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
    સ્થિર પૂર્ણાંક
syn_override(પૂર્ણાંક id)
{
    જો (overrides != ખાલી && overrides->arr != ખાલી)
	માટે (પૂર્ણાંક k = 0; k < overrides->len; k++)
	    જો (overrides->arr[k].from == id)
		પાછા overrides->arr[k].to;
    પાછા id;
}

/*
 * Lookup a highlight group name by pointer and length.
 * If it is not found, 0 is returned.
 */
    સ્થિર પૂર્ણાંક
syn_name2id_len(char_u *name, પૂર્ણાંક len)
{
    char_u	name_u[MAX_SYN_NAME + 1];
    hashitem_T	*hi;

    જો (len > MAX_SYN_NAME)
	len = MAX_SYN_NAME;
    mch_memmove(name_u, name, len);
    name_u[len] = NUL;
    vim_strup(name_u);
    જો (!highlight_ht_inited)
	પાછા 0;
    hi = hash_find(&highlight_ht, name_u);
    જો (HASHITEM_EMPTY(hi))
	પાછા 0;
    પાછા HI2HLNAME(hi)->hn_id;
}

/*
 * Lookup a highlight group name and return its ID.
 * If it is not found, 0 is returned.
 */
    પૂર્ણાંક
syn_name2id(char_u *name)
{
    // Avoid using stricmp() too much, it's slow on some systems.
    પાછા syn_name2id_len(name, (પૂર્ણાંક)STRLEN(name));
}

/*
 * Lookup a highlight group name and return its attributes.
 * Return zero if not found.
 */
    પૂર્ણાંક
syn_name2attr(char_u *name)
{
    પૂર્ણાંક id = syn_name2id(name);

    જો (id != 0)
	પાછા syn_id2attr(id);
    પાછા 0;
}

#જો defined(FEAT_EVAL)
/*
 * Return TRUE if highlight group "name" exists.
 */
    પૂર્ણાંક
highlight_exists(char_u *name)
{
    પાછા (syn_name2id(name) > 0);
}

# જો defined(FEAT_શોધ_EXTRA)
/*
 * Return the name of highlight group "id".
 * When not a valid ID return an empty string.
 */
    char_u *
syn_id2name(પૂર્ણાંક id)
{
    જો (id <= 0 || id > highlight_ga.ga_len)
	પાછા (char_u *)"";
    પાછા HL_TABLE()[id - 1].sg_name;
}
# endif
#endif

/*
 * Like syn_name2id(), but take a pointer + length argument.
 */
    પૂર્ણાંક
syn_namen2id(char_u *linep, પૂર્ણાંક len)
{
    પાછા syn_name2id_len(linep, len);
}

/*
 * Find highlight group name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    પૂર્ણાંક
syn_check_group(char_u *pp, પૂર્ણાંક len)
{
    પૂર્ણાંક	    id;
    char_u  *name;

    જો (len > MAX_SYN_NAME)
    {
	emsg(_(e_highlight_group_name_too_long));
	પાછા 0;
    }
    id = syn_name2id_len(pp, len);
    જો (id == 0)			// doesn't exist yet
    {
	name = vim_strnsave(pp, len);
	જો (name == ખાલી)
	    પાછા 0;
	id = syn_add_group(name);
    }
    પાછા id;
}

/*
 * Add new highlight group and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    સ્થિર પૂર્ણાંક
syn_add_group(char_u *name)
{
    char_u	*p;
    char_u	*name_up;

    // Check that the name is valid (ASCII letters, digits, underscores, dots, or hyphens).
    માટે (p = name; *p != NUL; ++p)
    {
	જો (!vim_isprintc(*p))
	{
	    emsg(_(e_unprintable_character_in_group_name));
	    vim_free(name);
	    પાછા 0;
	}
	નહીંતર જો (!ASCII_ISALNUM(*p) && *p != '_' && *p != '.' && *p != '-')
	{
	    // This is an error, but since there previously was no check only
	    // give a warning.
	    સંદેશ_source(HL_ATTR(HLF_W));
	    સંદેશ(_("W18: Invalid character in group name"));
	    તોડો;
	}
    }

    // First call for this growarray: init growing array.
    જો (highlight_ga.ga_data == ખાલી)
    {
	highlight_ga.ga_itemsize = કદ(hl_group_T);
	highlight_ga.ga_growsize = 10;
	hash_init(&highlight_ht);
	highlight_ht_inited = true;
    }

    જો (highlight_ga.ga_len >= MAX_HL_ID)
    {
	emsg(_(e_too_many_highlight_and_syntax_groups));
	vim_free(name);
	પાછા 0;
    }

    // Make room for at least one other syntax_highlight entry.
    જો (ga_grow(&highlight_ga, 1) == FAIL)
    {
	vim_free(name);
	પાછા 0;
    }

    {
	hlname_T    *hn;
	પૂર્ણાંક	    len = (પૂર્ણાંક)STRLEN(name);

	hn = alloc(offsetof(hlname_T, hn_ચાવી) + len + 1);
	જો (hn == ખાલી)
	{
	    vim_free(name);
	    પાછા 0;
	}
	vim_strncpy(hn->hn_ચાવી, name, len);
	vim_strup(hn->hn_ચાવી);
	hn->hn_id = highlight_ga.ga_len + 1;	// ID is index plus one
	name_up = hn->hn_ચાવી;
    }

    CLEAR_POINTER(&(HL_TABLE()[highlight_ga.ga_len]));
    HL_TABLE()[highlight_ga.ga_len].sg_name = name;
    HL_TABLE()[highlight_ga.ga_len].sg_name_u = name_up;
#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_TABLE()[highlight_ga.ga_len].sg_gui_bg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_fg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_sp = INVALCOLOR;
#endif
    hash_add(&highlight_ht, name_up, "highlight");
    ++highlight_ga.ga_len;

    પાછા highlight_ga.ga_len;		    // ID is index plus one
}

/*
 * When, just after calling syn_add_group(), an error is discovered, this
 * function deletes the new name.
 */
    સ્થિર શૂન્ય
syn_unadd_group(શૂન્ય)
{
    hashitem_T *hi;

    --highlight_ga.ga_len;
    hi = hash_find(&highlight_ht, HL_TABLE()[highlight_ga.ga_len].sg_name_u);
    જો (!HASHITEM_EMPTY(hi))
	hash_remove(&highlight_ht, hi, "highlight");
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name);
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name_u - HLNAME_ચાવી_OFF);
}

/*
 * Translate a group ID to highlight attributes.
 * "hl_id" must be valid: > 0, caller must check.
 */
    પૂર્ણાંક
syn_id2attr(પૂર્ણાંક hl_id)
{
    પૂર્ણાંક		attr;
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    પ્રતિજ્ઞા(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

#ifdef FEAT_GUI
    // Only use GUI attr when the GUI is being used.
    જો (gui.in_use)
	attr = sgp->sg_gui_attr;
    નહીંતર
#endif
	જો (IS_CTERM)
	    attr = sgp->sg_cterm_attr;
	નહીંતર
	    attr = sgp->sg_term_attr;

    પાછા attr;
}

#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Get the GUI colors and attributes for a group ID.
 * NOTE: the colors will be INVALCOLOR when not set, the color otherwise.
 */
    પૂર્ણાંક
syn_id2colors(પૂર્ણાંક hl_id, guicolor_T *fgp, guicolor_T *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    પ્રતિજ્ઞા(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

    *fgp = sgp->sg_gui_fg;
    *bgp = sgp->sg_gui_bg;
    પાછા sgp->sg_gui;
}
#endif

#જો (defined(MSWIN) \
	    && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL)) \
	    && defined(FEAT_TERMGUICOLORS)) \
	|| defined(FEAT_TERMINAL)
    શૂન્ય
syn_id2cterm_bg(પૂર્ણાંક hl_id, પૂર્ણાંક *fgp, પૂર્ણાંક *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    પ્રતિજ્ઞા(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
    *fgp = sgp->sg_cterm_fg - 1;
    *bgp = sgp->sg_cterm_bg - 1;
}
#endif

/*
 * Translate a group ID to the final group ID (following links).
 */
    પૂર્ણાંક
syn_get_final_id(પૂર્ણાંક hl_id)
{
    પૂર્ણાંક		ગણતરી;
    hl_group_T	*sgp;

    જો (hl_id > highlight_ga.ga_len || hl_id < 1)
	પાછા 0;			// Can be called from eval!!

    // Follow links until there is no more.
    // Look out for loops!  Break after 100 links.
    માટે (ગણતરી = 100; --ગણતરી >= 0; )
    {
	પૂર્ણાંક tmp = hl_id;

	sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
	જો (sgp->sg_link == 0 || sgp->sg_link > highlight_ga.ga_len)
	    તોડો;

	// This is to handle highlight groups that are overridden but are in the
	// middle of a link chain.
	hl_id = syn_override(hl_id);
	જો (tmp != hl_id)
	    ચાલુ;
	hl_id = sgp->sg_link;
    }
    પાછા syn_override(hl_id);
}

#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Call this function just after the GUI has started.
 * Also called when 'termguicolors' was set, gui.in_use will be FALSE then.
 * It finds the font and color handles for the highlighting groups.
 */
    શૂન્ય
highlight_gui_started(શૂન્ય)
{
    પૂર્ણાંક	    idx;

    // First get the colors from the "Normal" and "Menu" group, if set
    જો (USE_24BIT)
	set_normal_colors();

    માટે (idx = 0; idx < highlight_ga.ga_len; ++idx)
	gui_do_one_color(idx, FALSE, FALSE);

    highlight_changed();
}

    સ્થિર શૂન્ય
gui_do_one_color(
    પૂર્ણાંક		idx,
    પૂર્ણાંક		do_menu UNUSED,	   // TRUE: might set the menu font
    પૂર્ણાંક		do_tooltip UNUSED) // TRUE: might set the tooltip font
{
    પૂર્ણાંક		didit = FALSE;

# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    જો (gui.in_use)
#  endif
	જો (HL_TABLE()[idx].sg_font_name != ખાલી)
	{
	    hl_do_font(idx, HL_TABLE()[idx].sg_font_name, FALSE, do_menu,
							    do_tooltip, TRUE);
	    didit = TRUE;
	}
# endif
    જો (HL_TABLE()[idx].sg_gui_fg_name != ખાલી)
    {
	HL_TABLE()[idx].sg_gui_fg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_fg_name);
	didit = TRUE;
    }
    જો (HL_TABLE()[idx].sg_gui_bg_name != ખાલી)
    {
	HL_TABLE()[idx].sg_gui_bg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_bg_name);
	didit = TRUE;
    }
    જો (HL_TABLE()[idx].sg_gui_sp_name != ખાલી)
    {
	HL_TABLE()[idx].sg_gui_sp =
			    color_name2handle(HL_TABLE()[idx].sg_gui_sp_name);
	didit = TRUE;
    }
    જો (didit)	// need to get a new attr number
	set_hl_attr(idx);
}
#endif

#જો defined(USER_HIGHLIGHT) && defined(FEAT_STL_OPT)
/*
 * Apply difference between User[1-9] and HLF_S to HLF_SNC, HLF_ST or HLF_STNC.
 */
    સ્થિર શૂન્ય
combine_stl_hlt(
	પૂર્ણાંક id,
	પૂર્ણાંક id_S,
	પૂર્ણાંક id_alt,
	પૂર્ણાંક hlcnt,
	પૂર્ણાંક i,
	પૂર્ણાંક hlf,
	પૂર્ણાંક *table)
{
    hl_group_T *hlt = HL_TABLE();

    જો (id_alt == 0)
    {
	CLEAR_POINTER(&hlt[hlcnt + i]);
	hlt[hlcnt + i].sg_term = highlight_attr[hlf];
	hlt[hlcnt + i].sg_cterm = highlight_attr[hlf];
# જો defined(FEAT_GUI) || defined(FEAT_EVAL)
	hlt[hlcnt + i].sg_gui = highlight_attr[hlf];
# endif
    }
    નહીંતર
	mch_memmove(&hlt[hlcnt + i],
		    &hlt[id_alt - 1],
		    કદ(hl_group_T));
    hlt[hlcnt + i].sg_link = 0;

    hlt[hlcnt + i].sg_term ^=
	hlt[id - 1].sg_term ^ hlt[id_S - 1].sg_term;
    જો (hlt[id - 1].sg_start != hlt[id_S - 1].sg_start)
	hlt[hlcnt + i].sg_start = hlt[id - 1].sg_start;
    જો (hlt[id - 1].sg_stop != hlt[id_S - 1].sg_stop)
	hlt[hlcnt + i].sg_stop = hlt[id - 1].sg_stop;
    hlt[hlcnt + i].sg_cterm ^=
	hlt[id - 1].sg_cterm ^ hlt[id_S - 1].sg_cterm;
    જો (hlt[id - 1].sg_cterm_fg != hlt[id_S - 1].sg_cterm_fg)
	hlt[hlcnt + i].sg_cterm_fg = hlt[id - 1].sg_cterm_fg;
    જો (hlt[id - 1].sg_cterm_bg != hlt[id_S - 1].sg_cterm_bg)
	hlt[hlcnt + i].sg_cterm_bg = hlt[id - 1].sg_cterm_bg;
    જો (hlt[id - 1].sg_cterm_font != hlt[id_S - 1].sg_cterm_font)
	hlt[hlcnt + i].sg_cterm_font = hlt[id - 1].sg_cterm_font;
# જો defined(FEAT_GUI) || defined(FEAT_EVAL)
    hlt[hlcnt + i].sg_gui ^=
	hlt[id - 1].sg_gui ^ hlt[id_S - 1].sg_gui;
# endif
# ifdef FEAT_GUI
    જો (hlt[id - 1].sg_gui_fg != hlt[id_S - 1].sg_gui_fg)
	hlt[hlcnt + i].sg_gui_fg = hlt[id - 1].sg_gui_fg;
    જો (hlt[id - 1].sg_gui_bg != hlt[id_S - 1].sg_gui_bg)
	hlt[hlcnt + i].sg_gui_bg = hlt[id - 1].sg_gui_bg;
    જો (hlt[id - 1].sg_gui_sp != hlt[id_S - 1].sg_gui_sp)
	hlt[hlcnt + i].sg_gui_sp = hlt[id - 1].sg_gui_sp;
    જો (hlt[id - 1].sg_font != hlt[id_S - 1].sg_font)
	hlt[hlcnt + i].sg_font = hlt[id - 1].sg_font;
#  ifdef FEAT_XFONTSET
    જો (hlt[id - 1].sg_fontset != hlt[id_S - 1].sg_fontset)
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
    પૂર્ણાંક
highlight_changed(શૂન્ય)
{
    પૂર્ણાંક		hlf;
    પૂર્ણાંક		i;
    char_u	*p;
    char_u	*default_hl;
    પૂર્ણાંક		attr;
    char_u	*end;
    પૂર્ણાંક		id;
#ifdef USER_HIGHLIGHT
    char_u      userhl[30];  // use 30 to avoid compiler warning
# ifdef FEAT_STL_OPT
    પૂર્ણાંક		id_S = -1;
    પૂર્ણાંક		id_SNC = 0;
#  ifdef FEAT_TERMINAL
    પૂર્ણાંક		id_ST = 0;
    પૂર્ણાંક		id_STNC = 0;
#  endif
    પૂર્ણાંક		hlcnt;
# endif
#endif
    win_T	*wp;

    need_highlight_changed = FALSE;

#ifdef FEAT_TERMINAL
    term_update_colors_all();
    term_update_hlfwin_all();
#endif

    // Clear all attributes.
    માટે (hlf = 0; hlf < (પૂર્ણાંક)HLF_ગણતરી; ++hlf)
    {
	highlight_attr[hlf] = 0;
	highlight_attr_raw[hlf] = 0;
	highlight_ids[hlf] = 0;
    }

    default_hl = get_highlight_default();

    // First set all attributes to their default value.
    // Then use the attributes from the 'highlight' option, unless it already
    // matches the default and would repeat the same work.
    માટે (i = 0; i < 2; ++i)
    {
	જો (i)
	{
	    જો (default_hl != ખાલી && p_hl != ખાલી
				      && STRCMP(default_hl, p_hl) == 0)
		ચાલુ;
	    p = p_hl;
	}
	નહીંતર
	    p = default_hl;
	જો (p == ખાલી)	    // just in case
	    ચાલુ;

	જ્યારે (*p)
	{
	    માટે (hlf = 0; hlf < (પૂર્ણાંક)HLF_ગણતરી; ++hlf)
		જો (hl_flags[hlf] == *p)
		    તોડો;
	    ++p;
	    જો (hlf == (પૂર્ણાંક)HLF_ગણતરી || *p == NUL)
		પાછા FAIL;

	    // Allow several hl_flags to be combined, like "bu" for
	    // bold-underlined.
	    attr = 0;
	    id = 0;
	    માટે ( ; *p && *p != ','; ++p)	    // parse up to comma
	    {
		જો (VIM_ISWHITE(*p))		    // ignore white space
		    ચાલુ;

		જો (attr > HL_ALL)  // Combination with ':' is not allowed.
		    પાછા FAIL;

		// Note: Keep this in sync with expand_set_highlight().
		પસંદગી (*p)
		{
		    સ્થિતિ 'b':	attr |= HL_BOLD;
				તોડો;
		    સ્થિતિ 'i':	attr |= HL_ITALIC;
				તોડો;
		    સ્થિતિ '-':
		    સ્થિતિ 'n':			    // no highlighting
				તોડો;
		    સ્થિતિ 'r':	attr |= HL_INVERSE;
				તોડો;
		    સ્થિતિ 's':	attr |= HL_STANDOUT;
				તોડો;
		    સ્થિતિ 'u':	attr |= HL_UNDERLINE;
				તોડો;
		    સ્થિતિ 'c':	attr |= HL_UNDERCURL;
				તોડો;
		    સ્થિતિ '2':	attr |= HL_UNDERDOUBLE;
				તોડો;
		    સ્થિતિ 'd':	attr |= HL_UNDERDOTTED;
				તોડો;
		    સ્થિતિ '=':	attr |= HL_UNDERDASHED;
				તોડો;
		    સ્થિતિ 't':	attr |= HL_STRIKETHROUGH;
				તોડો;
		    સ્થિતિ ':':	++p;		    // highlight group name
				જો (attr || *p == NUL)	 // no combinations
				    પાછા FAIL;
				end = vim_strchr(p, ',');
				જો (end == ખાલી)
				    end = p + STRLEN(p);
				id = syn_check_group(p, (પૂર્ણાંક)(end - p));
				જો (id == 0)
				    પાછા FAIL;
				attr = syn_id2attr(id);
				p = end - 1;
#જો defined(FEAT_STL_OPT) && defined(USER_HIGHLIGHT)
				જો (hlf == (પૂર્ણાંક)HLF_SNC)
				    id_SNC = syn_get_final_id(id);
# ifdef FEAT_TERMINAL
				નહીંતર જો (hlf == (પૂર્ણાંક)HLF_ST)
				    id_ST = syn_get_final_id(id);
				નહીંતર જો (hlf == (પૂર્ણાંક)HLF_STNC)
				    id_STNC = syn_get_final_id(id);
# endif
				નહીંતર જો (hlf == (પૂર્ણાંક)HLF_S)
				    id_S = syn_get_final_id(id);
#endif
				તોડો;
		    મૂળભૂત:	પાછા FAIL;
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
	અક્ષર *errmsg = update_winhighlight(wp, wp->w_p_whl);

	જો (errmsg != ખાલી)
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
    જો (ga_grow(&highlight_ga, 28) == FAIL)
	પાછા FAIL;
    hlcnt = highlight_ga.ga_len;
    જો (id_S == -1)
    {
	// Make sure id_S is always valid to simplify code below. Use the last
	// entry.
	CLEAR_POINTER(&HL_TABLE()[hlcnt + 27]);
	HL_TABLE()[hlcnt + 18].sg_term = highlight_attr[HLF_S];
	id_S = hlcnt + 19;
    }
# endif
    માટે (i = 0; i < 9; i++)
    {
	sprintf((અક્ષર *)userhl, "User%d", i + 1);
	id = syn_name2id(userhl);
	જો (id == 0)
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
	નહીંતર
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

    પાછા OK;
}

સ્થિર શૂન્ય highlight_list(શૂન્ય);
સ્થિર શૂન્ય highlight_list_two(પૂર્ણાંક cnt, પૂર્ણાંક attr);

// context for :highlight <group> <arg> expansion
સ્થિર પૂર્ણાંક expand_hi_synid = 0;	    // ID for highlight group being completed
સ્થિર પૂર્ણાંક expand_hi_equal_col = 0; // column where the '=' is
સ્થિર પૂર્ણાંક expand_hi_include_orig = 0;	    // whether to fill the existing current value or not
સ્થિર char_u *expand_hi_curvalue = ખાલી;   // the existing current value
#જો defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
સ્થિર dict_iterator_T expand_colornames_iter;	// iterator for looping through v:colornames
#endif

/*
 * Handle command line completion for :highlight command.
 */
    શૂન્ય
set_context_in_highlight_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;
    પૂર્ણાંક		expand_group = TRUE;

    // Default: expand group names
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 0;
    include_link = 2;
    include_default = 1;

    જો (*arg == NUL)
	પાછા;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    જો (*p == NUL)
	પાછા;

    // past "default" or group name
    include_default = 0;
    જો (STRNCMP("default", arg, p - arg) == 0)
    {
	arg = skipwhite(p);
	xp->xp_pattern = arg;
	p = skiptowhite(arg);
    }
    જો (*p == NUL)
	પાછા;

    // past group name
    include_link = 0;
    જો (arg[1] == 'i' && arg[0] == 'N')
    {
	highlight_list();
	expand_group = FALSE;
    }
    જો (STRNCMP("link", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	જો (*p != NUL)		// past first group name
	{
	    xp->xp_pattern = skipwhite(p);
	    p = skiptowhite(xp->xp_pattern);
	    include_none = 1;
	}
	expand_group = FALSE;
    }
    નહીંતર જો (STRNCMP("clear", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	expand_group = FALSE;
    }
    જો (*p != NUL)			// past group name(s)
    {
	જો (expand_group)
	{
	    // expansion will be done in expand_highlight_group()
	    xp->xp_context = EXPAND_HIGHLIGHT_GROUP;

	    expand_hi_synid = syn_namen2id(arg, (પૂર્ણાંક)(p - arg));

	    જ્યારે (*p != NUL)
	    {
		arg = skipwhite(p);
		p = skiptowhite(arg);
	    }

	    p = vim_strchr(arg, '=');
	    જો (p == ખાલી)
	    {
		// Didn't find a key=<value> pattern
		xp->xp_pattern = arg;
		expand_hi_equal_col = -1;
		expand_hi_include_orig = FALSE;
	    }
	    નહીંતર
	    {
		// Found key=<value> pattern, record the exact location
		expand_hi_equal_col = (પૂર્ણાંક)(p - xp->xp_લીટી);

		// Only include the original value if the pattern is empty
		જો (*(p + 1) == NUL)
		    expand_hi_include_orig = TRUE;
		નહીંતર
		    expand_hi_include_orig = FALSE;

		// Account for comma-separated values
		જો (STRNCMP(arg, "term=", 5) == 0 ||
			STRNCMP(arg, "cterm=", 6) == 0 ||
			STRNCMP(arg, "gui=", 4) == 0)
		{
		    char_u *comma = vim_strrchr(p + 1, ',');
		    જો (comma != ખાલી)
			p = comma;
		}
		xp->xp_pattern = p + 1;
	    }
	}
	નહીંતર
	{
	    xp->xp_context = EXPAND_NOTHING;
	}
    }
}

/*
 * List highlighting matches in a nice way.
 */
    સ્થિર શૂન્ય
highlight_list(શૂન્ય)
{
    પૂર્ણાંક		i;

    માટે (i = 10; --i >= 0; )
	highlight_list_two(i, HL_ATTR(HLF_D));
    માટે (i = 40; --i >= 0; )
	highlight_list_two(99, 0);
}

    સ્થિર શૂન્ય
highlight_list_two(પૂર્ણાંક cnt, પૂર્ણાંક attr)
{
    સંદેશ_puts_attr(&("N \bI \b!  \b"[cnt / 11]), attr);
    સંદેશ_clr_eos();
    out_flush();
    ui_delay(cnt == 99 ? 40L : (long)cnt * 50L, FALSE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_highlight_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    પાછા get_highlight_name_ext(xp, idx, TRUE);
}

/*
 * Obtain a highlight group name.
 * When "skip_cleared" is TRUE don't return a cleared entry.
 */
    char_u *
get_highlight_name_ext(expand_T *xp UNUSED, પૂર્ણાંક idx, પૂર્ણાંક skip_cleared)
{
    જો (idx < 0)
	પાછા ખાલી;

    // Items are never removed from the table, skip the ones that were
    // cleared.
    જો (skip_cleared && idx < highlight_ga.ga_len && HL_TABLE()[idx].sg_cleared)
	પાછા (char_u *)"";

    જો (idx == highlight_ga.ga_len && include_none != 0)
	પાછા (char_u *)"NONE";
    જો (idx == highlight_ga.ga_len + include_none && include_default != 0)
	પાછા (char_u *)"default";
    જો (idx == highlight_ga.ga_len + include_none + include_default
							 && include_link != 0)
	પાછા (char_u *)"link";
    જો (idx == highlight_ga.ga_len + include_none + include_default + 1
							 && include_link != 0)
	પાછા (char_u *)"clear";
    જો (idx >= highlight_ga.ga_len)
	પાછા ખાલી;
    પાછા HL_TABLE()[idx].sg_name;
}

    સ્થિર char_u *
get_highlight_attr_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx == 0)
    {
	// Fill with current value first
	જો (expand_hi_curvalue != ખાલી)
	    પાછા expand_hi_curvalue;
	નહીંતર
	    પાછા (char_u*)"";
    }
    જો (idx < (પૂર્ણાંક)ARRAY_LENGTH(highlight_index_tab) + 1)
    {
	char_u *કિંમત = highlight_index_tab[idx-1]->કિંમત.string;
	જો (expand_hi_curvalue != ખાલી && STRCMP(expand_hi_curvalue, કિંમત) == 0)
	{
	    // Already returned the current value above, just skip.
	    પાછા (char_u*)"";
	}
	પાછા કિંમત;
    }
    પાછા ખાલી;
}

    સ્થિર char_u *
get_highlight_cterm_color(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx == 0)
    {
	// Fill with current value first
	જો (expand_hi_curvalue != ખાલી)
	    પાછા expand_hi_curvalue;
	નહીંતર
	    પાછા (char_u*)"";
    }
    // See highlight_set_cterm_color()
    નહીંતર જો (idx == 1)
	પાછા (char_u*)"fg";
    નહીંતર જો (idx == 2)
	પાછા (char_u*)"bg";
    જો (idx < (પૂર્ણાંક)ARRAY_LENGTH(color_name_tab) + 3)
    {
	char_u *કિંમત = color_name_tab[idx-3].કિંમત.string;
	પાછા કિંમત;
    }
    પાછા ખાલી;
}

#જો defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
    સ્થિર char_u *
get_highlight_gui_color(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx == 0)
    {
	// Fill with current value first
	જો (expand_hi_curvalue != ખાલી)
	    પાછા expand_hi_curvalue;
	નહીંતર
	    પાછા (char_u*)"";
    }
    // See color_name2handle()
    નહીંતર જો (idx == 1)
	પાછા (char_u*)"fg";
    નહીંતર જો (idx == 2)
	પાછા (char_u*)"bg";
    નહીંતર જો (idx == 3)
	પાછા (char_u*)"NONE";

    // Complete from v:colornames. Don't do platform specific names for now.
    typval_T *tv_result;
    char_u *colorname = dict_iterate_next(&expand_colornames_iter, &tv_result);
    જો (colorname != ખાલી)
    {
	// :hi command doesn't allow space, so don't suggest any malformed items
	જો (vim_strchr(colorname, ' ') != ખાલી)
	    પાછા (char_u*)"";

	જો (expand_hi_curvalue != ખાલી && STRICMP(expand_hi_curvalue, colorname) == 0)
	{
	    // Already returned the current value above, just skip.
	    પાછા (char_u*)"";
	}
    }
    પાછા colorname;
}
#endif

    સ્થિર char_u *
get_highlight_group_ચાવી(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    // Note: Keep this in sync with do_highlight.
    સ્થિર અક્ષર *(p_hi_group_ચાવી_કિંમતો[]) =
    {
	"term=",
	"start=",
	"stop=",
	"cterm=",
	"ctermfg=",
	"ctermbg=",
	"ctermul=",
	"ctermfont=",
#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

    જો (idx < (પૂર્ણાંક)ARRAY_LENGTH(p_hi_group_ચાવી_કિંમતો))
	પાછા (char_u*)p_hi_group_ચાવી_કિંમતો[idx];
    પાછા ખાલી;
}

/*
 * Command-line expansion for :hi {group-name} <args>...
 */
    પૂર્ણાંક
expand_highlight_group(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	પૂર્ણાંક	    *અંક_Matches)
{
    જો (expand_hi_equal_col != -1)
    {
	// List the values. First fill in the current value, then if possible colors
	// or attribute names.
	char_u	    *(*expandfunc)(expand_T *, પૂર્ણાંક) = ખાલી;
	પૂર્ણાંક	    type = 0;
	hl_group_T  *sgp = ખાલી;
	પૂર્ણાંક	    iarg = 0;
	char_u	    *sarg = ખાલી;

	પૂર્ણાંક	    unsorted_વસ્તુઓ = -1; // don't sort by default

	જો (expand_hi_synid != 0)
	    sgp = &HL_TABLE()[expand_hi_synid - 1]; // index is ID minus one

	// Note: Keep this in sync with highlight_list_one().
	char_u	    *name_end = xp->xp_લીટી + expand_hi_equal_col;
	જો (name_end - xp->xp_લીટી >= 5
		&& STRNCMP(name_end - 5, " term", 5) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    જો (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_term;
	    }
	}
	નહીંતર જો (name_end - xp->xp_લીટી >= 6
		&& STRNCMP(name_end - 6, " cterm", 6) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    જો (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_cterm;
	    }
	}
#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	નહીંતર જો (name_end - xp->xp_લીટી >= 4
		&& STRNCMP(name_end - 4, " gui", 4) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    જો (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_gui;
	    }
	}
#endif
	નહીંતર જો (name_end - xp->xp_લીટી >= 8
		&& STRNCMP(name_end - 8, " ctermfg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    જો (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_fg;
	    }
	}
	નહીંતર જો (name_end - xp->xp_લીટી >= 8
		&& STRNCMP(name_end - 8, " ctermbg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    જો (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_bg;
	    }
	}
	નહીંતર જો (name_end - xp->xp_લીટી >= 8
		&& STRNCMP(name_end - 8, " ctermul", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    જો (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_ul;
	    }
	}
#જો defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	નહીંતર જો (name_end - xp->xp_લીટી >= 6
		&& STRNCMP(name_end - 6, " guifg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    જો (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_fg_name;
	    }
	}
	નહીંતર જો (name_end - xp->xp_લીટી >= 6
		&& STRNCMP(name_end - 6, " guibg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    જો (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_bg_name;
	    }
	}
	નહીંતર જો (name_end - xp->xp_લીટી >= 6
		&& STRNCMP(name_end - 6, " guisp", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    જો (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_sp_name;
	    }
	}
#endif

#જો defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	જો (expandfunc == get_highlight_gui_color)
	{
	    // Top 4 items are special, after that sort all the color names
	    unsorted_વસ્તુઓ = 4;

	    dict_T *colornames_table = get_vim_var_dict(VV_COLORNAMES);
	    typval_T colornames_val;
	    colornames_val.v_type = VAR_DICT;
	    colornames_val.vval.v_dict = colornames_table;
	    dict_iterate_start(&colornames_val, &expand_colornames_iter);
	}
#endif

	char_u	    buf[MAX_ATTR_LEN];

	expand_hi_curvalue = ખાલી;
	જો (expand_hi_include_orig)
	{
	    જો (((type == LIST_ATTR || type == LIST_INT) && iarg != 0) ||
		(type == LIST_STRING && sarg != ખાલી))
	    {
		// Retrieve the current value to go first in completion
		expand_hi_curvalue = highlight_arg_to_string(
			type, iarg, sarg, buf);
	    }
	}

	જો (expandfunc != ખાલી)
	{
	    પાછા ExpandGenericExt(
		    pat,
		    xp,
		    rmp,
		    matches,
		    અંક_Matches,
		    expandfunc,
		    FALSE,
		    unsorted_વસ્તુઓ);
	}

	પાછા FAIL;
    }

    // List all the key names
    પાછા ExpandGenericExt(
	    pat,
	    xp,
	    rmp,
	    matches,
	    અંક_Matches,
	    get_highlight_group_ચાવી,
	    FALSE,
	    -1);
}

#જો defined(FEAT_GUI)
/*
 * Free all the highlight group fonts.
 * Used when quitting for systems which need it.
 */
    શૂન્ય
free_highlight_fonts(શૂન્ય)
{
    પૂર્ણાંક	    idx;

    માટે (idx = 0; idx < highlight_ga.ga_len; ++idx)
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

#જો defined(FEAT_EVAL)
/*
 * Convert each of the highlight attribute bits (bold, standout, underline,
 * etc.) set in 'hlattr' into a separate boolean item in a Dictionary with
 * the attribute name as the key.
 */
    સ્થિર dict_T *
highlight_get_attr_dict(પૂર્ણાંક hlattr)
{
    dict_T	*dict;
    પૂર્ણાંક		i;

    dict = dict_alloc();
    જો (dict == ખાલી)
	પાછા ખાલી;

    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(highlight_index_tab); ++i)
    {
	જો (hlattr & highlight_index_tab[i]->ચાવી)
	{
	    dict_add_bool(dict, (અક્ષર *)highlight_index_tab[i]->કિંમત.string,
		    VVAL_TRUE);
	    // don't want "inverse"/"reverse"
	    hlattr &= ~highlight_index_tab[i]->ચાવી;
	}
    }

    પાછા dict;
}

/*
 * Return the attributes of the highlight group at index 'hl_idx' as a
 * Dictionary. If 'resolve_link' is TRUE, then resolves the highlight group
 * links recursively.
 */
    સ્થિર dict_T *
highlight_get_info(પૂર્ણાંક hl_idx, પૂર્ણાંક resolve_link)
{
    dict_T	*dict;
    hl_group_T	*sgp;
    dict_T	*attr_dict = ખાલી;
    પૂર્ણાંક		hlgid;

    dict = dict_alloc();
    જો (dict == ખાલી)
	પાછા dict;

    sgp = &HL_TABLE()[hl_idx];
    // highlight group id is 1-based
    hlgid = hl_idx + 1;

    જો (dict_add_string(dict, "name", sgp->sg_name) == FAIL)
	જાઓ error;
    જો (dict_add_સંખ્યા(dict, "id", hlgid) == FAIL)
	જાઓ error;

    જો (resolve_link)
    {
	// resolve the highlight group link recursively
	જ્યારે (sgp->sg_link)
	{
	    hlgid = sgp->sg_link;
	    sgp = &HL_TABLE()[sgp->sg_link - 1];
	}
    }

    જો (sgp->sg_term != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_term);
	જો (attr_dict != ખાલી)
	{
	    જો (dict_add_dict(dict, "term", attr_dict) == FAIL)
		જાઓ error;
	    attr_dict = ખાલી;
	}
    }
    જો (sgp->sg_start != ખાલી)
	જો (dict_add_string(dict, "start", sgp->sg_start) == FAIL)
	    જાઓ error;
    જો (sgp->sg_stop != ખાલી)
	જો (dict_add_string(dict, "stop", sgp->sg_stop) == FAIL)
	    જાઓ error;
    જો (sgp->sg_cterm != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_cterm);
	જો (attr_dict != ખાલી)
	{
	    જો (dict_add_dict(dict, "cterm", attr_dict) == FAIL)
		જાઓ error;
	    attr_dict = ખાલી;
	}
    }
    જો (sgp->sg_cterm_fg != 0)
	જો (dict_add_string(dict, "ctermfg",
		    highlight_color(hlgid, (char_u *)"fg", 'c')) == FAIL)
	    જાઓ error;
    જો (sgp->sg_cterm_bg != 0)
	જો (dict_add_string(dict, "ctermbg",
			highlight_color(hlgid, (char_u *)"bg", 'c')) == FAIL)
	    જાઓ error;
    જો (sgp->sg_cterm_ul != 0)
	જો (dict_add_string(dict, "ctermul",
			highlight_color(hlgid, (char_u *)"ul", 'c')) == FAIL)
	    જાઓ error;
    જો (sgp->sg_cterm_font != 0)
	જો (dict_add_string(dict, "ctermfont",
			highlight_color(hlgid, (char_u *)"font", 'c')) == FAIL)
	    જાઓ error;
    જો (sgp->sg_gui != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_gui);
	જો (attr_dict != ખાલી)
	{
	    જો (dict_add_dict(dict, "gui", attr_dict) == FAIL)
		જાઓ error;
	    attr_dict = ખાલી;
	}
    }
    જો (sgp->sg_gui_fg_name != ખાલી)
	જો (dict_add_string(dict, "guifg",
			highlight_color(hlgid, (char_u *)"fg", 'g')) == FAIL)
	    જાઓ error;
    જો (sgp->sg_gui_bg_name != ખાલી)
	જો (dict_add_string(dict, "guibg",
			highlight_color(hlgid, (char_u *)"bg", 'g')) == FAIL)
	    જાઓ error;
    જો (sgp->sg_gui_sp_name != ખાલી)
	જો (dict_add_string(dict, "guisp",
			highlight_color(hlgid, (char_u *)"sp", 'g')) == FAIL)
	    જાઓ error;
# ifdef FEAT_GUI
    જો (sgp->sg_font_name != ખાલી)
	જો (dict_add_string(dict, "font", sgp->sg_font_name) == FAIL)
	    જાઓ error;
# endif
    જો (sgp->sg_link)
    {
	char_u	*link;

	link = HL_TABLE()[sgp->sg_link - 1].sg_name;
	જો (link != ખાલી && dict_add_string(dict, "linksto", link) == FAIL)
	    જાઓ error;

	જો (sgp->sg_deflink)
	    dict_add_bool(dict, "default", VVAL_TRUE);
    }
    જો (dict_len(dict) == 2)
	// If only 'name' is present, then the highlight group is cleared.
	dict_add_bool(dict, "cleared", VVAL_TRUE);

    પાછા dict;

error:
    dict_unref(attr_dict);
    dict_unref(dict);
    પાછા ખાલી;
}

/*
 * "hlget([name])" function
 * Return the attributes of a specific highlight group (if specified) or all
 * the highlight groups.
 */
    શૂન્ય
f_hlget(typval_T *argvars, typval_T *rettv)
{
    list_T	*list;
    dict_T	*dict;
    પૂર્ણાંક		i;
    char_u	*hlarg = ખાલી;
    પૂર્ણાંક		resolve_link = FALSE;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (check_for_opt_string_arg(argvars, 0) == FAIL
	    || (argvars[0].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_arg(argvars, 1) == FAIL))
	પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	// highlight group name supplied
	hlarg = tv_get_string_chk(&argvars[0]);
	જો (hlarg == ખાલી)
	    પાછા;

	જો (argvars[1].v_type != VAR_UNKNOWN)
	{
	    પૂર્ણાંક error = FALSE;

	    resolve_link = tv_get_bool_chk(&argvars[1], &error);
	    જો (error)
		પાછા;
	}
    }

    list = rettv->vval.v_list;
    માટે (i = 0; i < highlight_ga.ga_len && !got_int; ++i)
    {
	જો (hlarg == ખાલી || STRICMP(hlarg, HL_TABLE()[i].sg_name) == 0)
	{
	    dict = highlight_get_info(i, resolve_link);
	    જો (dict != ખાલી && list_append_dict(list, dict) == FAIL)
		dict_unref(dict);
	}
    }
}

/*
 * Returns the string value at 'dict[key]'. Returns NULL, if 'key' is not in
 * 'dict' or the value is not a string type. If the value is not a string type
 * or is NULL, then 'error' is set to TRUE.
 */
    સ્થિર char_u *
hldict_get_string(dict_T *dict, char_u *ચાવી, પૂર્ણાંક *error)
{
    dictitem_T	*di;

    *error = FALSE;
    di = dict_find(dict, ચાવી, -1);
    જો (di == ખાલી)
	પાછા ખાલી;

    જો (di->di_tv.v_type != VAR_STRING || di->di_tv.vval.v_string == ખાલી)
    {
	emsg(_(e_string_required));
	*error = TRUE;
	પાછા ખાલી;
    }

    પાછા di->di_tv.vval.v_string;
}

/*
 * Convert the highlight attribute Dictionary at 'dict[key]' into a string
 * value in 'attr_str' of length 'len'. Returns FALSE if 'dict[key]' is not a
 * Dictionary or is NULL.
 */
    સ્થિર પૂર્ણાંક
hldict_attr_to_str(
	dict_T	*dict,
	char_u	*ચાવી,
	char_u	*attr_str,
	size_t	len)
{
    dictitem_T	*di;
    dict_T	*attrdict;
    પૂર્ણાંક		i;
    char_u	*p;

    attr_str[0] = NUL;
    di = dict_find(dict, ચાવી, -1);
    જો (di == ખાલી)
	પાછા TRUE;

    જો (di->di_tv.v_type != VAR_DICT || di->di_tv.vval.v_dict == ખાલી)
    {
	emsg(_(e_dictionary_required));
	પાછા FALSE;
    }

    attrdict = di->di_tv.vval.v_dict;

    // If the attribute dict is empty, then return NONE to clear the attributes
    જો (dict_len(attrdict) == 0)
    {
	vim_strcat(attr_str, (char_u *)"NONE", len);
	પાછા TRUE;
    }

    p = attr_str;
    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(highlight_tab); ++i)
    {
	જો (dict_get_bool(attrdict, (અક્ષર *)highlight_tab[i].કિંમત.string,
		    VVAL_FALSE) == VVAL_TRUE)
	{
	    જો (p != attr_str && (size_t)(p - attr_str + 2) < len)
		STRCPY(p, (char_u *)",");
	    જો (p - attr_str + highlight_tab[i].કિંમત.length + 1 < len)
	    {
		STRCPY(p, highlight_tab[i].કિંમત.string);
		p += highlight_tab[i].કિંમત.length;
	    }
	}
    }

    પાછા TRUE;
}

// Temporary buffer used to store the command string produced by hlset().
// IObuff cannot be used for this as the error messages produced by hlset()
// internally use IObuff.
# define	HLSETBUFSZ  512
સ્થિર char_u hlsetBuf[HLSETBUFSZ + 1];

/*
 * Add the highlight attribute "attr" of length "attrlen" and "value" at
 * "dptr", which points into "hlsetBuf".
 * Returns the updated pointer.
 */
    સ્થિર char_u *
add_attr_and_કિંમત(char_u *dptr, char_u *attr, પૂર્ણાંક attrlen, char_u *કિંમત)
{
    size_t	vallen;

    // Do nothing if the value is not specified or is empty
    જો (કિંમત == ખાલી || *કિંમત == NUL)
	પાછા dptr;

    vallen = STRLEN(કિંમત);
    // When the value contains a space and the attribute has an "=" (i.e. it
    // is a key=value pair), surround the value with single quotes so that
    // do_highlight() can parse it correctly.
    જો (vim_strchr(કિંમત, ' ') != ખાલી && vim_strchr(attr, '=') != ખાલી)
    {
	જો (dptr + attrlen + vallen + 3 < hlsetBuf + HLSETBUFSZ)
	{
	    STRCPY(dptr, attr);
	    dptr += attrlen;
	    *dptr++ = '\'';
	    STRCPY(dptr, કિંમત);
	    dptr += vallen;
	    *dptr++ = '\'';
	    *dptr = NUL;
	}
    }
    નહીંતર જો (dptr + attrlen + vallen + 1 < hlsetBuf + HLSETBUFSZ)
    {
	STRCPY(dptr, attr);
	dptr += attrlen;
	STRCPY(dptr, કિંમત);
	dptr += vallen;
    }

    પાછા dptr;
}

/*
 * Add or update a highlight group using 'dict' items. Returns TRUE if
 * successfully updated the highlight group.
 */
    સ્થિર પૂર્ણાંક
hlg_add_or_update(dict_T *dict)
{
    char_u	*name;
    પૂર્ણાંક		error;
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
    પૂર્ણાંક		forceit = FALSE;
    પૂર્ણાંક		dodefault = FALSE;
    પૂર્ણાંક		done = FALSE;
    char_u	*p;

    name = hldict_get_string(dict, (char_u *)"name", &error);
    જો (name == ખાલી || *name == NUL || error)
	પાછા FALSE;

    જો (dict_get_bool(dict, "force", VVAL_FALSE) == VVAL_TRUE)
	forceit = TRUE;

    જો (dict_get_bool(dict, "default", VVAL_FALSE) == VVAL_TRUE)
	dodefault = TRUE;

    જો (dict_has_ચાવી(dict, "cleared"))
    {
	varnumber_T	cleared;

	// clear a highlight group
	cleared = dict_get_bool(dict, "cleared", FALSE);
	જો (cleared == TRUE)
	{
	    vim_snprintf((અક્ષર *)hlsetBuf, HLSETBUFSZ, "clear %s", name);
	    do_highlight(hlsetBuf, forceit, FALSE);
	    done = TRUE;
	}
    }

    જો (dict_has_ચાવી(dict, "linksto"))
    {
	char_u	*linksto;

	// link highlight groups
	linksto = hldict_get_string(dict, (char_u *)"linksto", &error);
	જો (linksto == ખાલી || *linksto == NUL || error)
	    પાછા FALSE;

	vim_snprintf((અક્ષર *)hlsetBuf, HLSETBUFSZ, "%slink %s %s",
				dodefault ? "default " : "", name, linksto);
	do_highlight(hlsetBuf, forceit, FALSE);

	done = TRUE;
    }

    // If 'cleared' or 'linksto' are specified, then don't process the other
    // attributes.
    જો (done)
	પાછા TRUE;

    start = hldict_get_string(dict, (char_u *)"start", &error);
    જો (error)
	પાછા FALSE;

    stop = hldict_get_string(dict, (char_u *)"stop", &error);
    જો (error)
	પાછા FALSE;

    જો (!hldict_attr_to_str(dict, (char_u *)"term", term_attr,
							    કદ(term_attr)))
	પાછા FALSE;

    જો (!hldict_attr_to_str(dict, (char_u *)"cterm", cterm_attr,
							   કદ(cterm_attr)))
	પાછા FALSE;

    ctermfg = hldict_get_string(dict, (char_u *)"ctermfg", &error);
    જો (error)
	પાછા FALSE;

    ctermbg = hldict_get_string(dict, (char_u *)"ctermbg", &error);
    જો (error)
	પાછા FALSE;

    ctermul = hldict_get_string(dict, (char_u *)"ctermul", &error);
    જો (error)
	પાછા FALSE;

    ctermfont = hldict_get_string(dict, (char_u *)"ctermfont", &error);
    જો (error)
	પાછા FALSE;

    જો (!hldict_attr_to_str(dict, (char_u *)"gui", gui_attr, કદ(gui_attr)))
	પાછા FALSE;

    guifg = hldict_get_string(dict, (char_u *)"guifg", &error);
    જો (error)
	પાછા FALSE;

    guibg = hldict_get_string(dict, (char_u *)"guibg", &error);
    જો (error)
	પાછા FALSE;

    guisp = hldict_get_string(dict, (char_u *)"guisp", &error);
    જો (error)
	પાછા FALSE;

# ifdef FEAT_GUI
    font = hldict_get_string(dict, (char_u *)"font", &error);
    જો (error)
	પાછા FALSE;
# endif

    // If none of the attributes are specified, then do nothing.
    જો (term_attr[0] == NUL && start == ખાલી && stop == ખાલી
	    && cterm_attr[0] == NUL && ctermfg == ખાલી && ctermbg == ખાલી
	    && ctermul == ખાલી && ctermfont == ખાલી && gui_attr[0] == NUL
# ifdef FEAT_GUI
	    && font == ખાલી
# endif
	    && guifg == ખાલી && guibg == ખાલી && guisp == ખાલી
	    )
	પાછા TRUE;

    hlsetBuf[0] = NUL;
    p = hlsetBuf;
    જો (dodefault)
	p = add_attr_and_કિંમત(p, (char_u *)"default", 7, (char_u *)" ");
    p = add_attr_and_કિંમત(p, (char_u *)"", 0, name);
    p = add_attr_and_કિંમત(p, (char_u *)" term=", 6, term_attr);
    p = add_attr_and_કિંમત(p, (char_u *)" start=", 7, start);
    p = add_attr_and_કિંમત(p, (char_u *)" stop=", 6, stop);
    p = add_attr_and_કિંમત(p, (char_u *)" cterm=", 7, cterm_attr);
    p = add_attr_and_કિંમત(p, (char_u *)" ctermfg=", 9, ctermfg);
    p = add_attr_and_કિંમત(p, (char_u *)" ctermbg=", 9, ctermbg);
    p = add_attr_and_કિંમત(p, (char_u *)" ctermul=", 9, ctermul);
    p = add_attr_and_કિંમત(p, (char_u *)" ctermfont=", 11, ctermfont);
    p = add_attr_and_કિંમત(p, (char_u *)" gui=", 5, gui_attr);
# ifdef FEAT_GUI
    p = add_attr_and_કિંમત(p, (char_u *)" font=", 6, font);
# endif
    p = add_attr_and_કિંમત(p, (char_u *)" guifg=", 7, guifg);
    p = add_attr_and_કિંમત(p, (char_u *)" guibg=", 7, guibg);
    (શૂન્ય)add_attr_and_કિંમત(p, (char_u *)" guisp=", 7, guisp);

    do_highlight(hlsetBuf, forceit, FALSE);

    પાછા TRUE;
}

/*
 * "hlset([{highlight_attr}])" function
 * Add or modify highlight groups
 */
    શૂન્ય
f_hlset(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    dict_T	*dict;

    rettv->vval.v_સંખ્યા = -1;

    જો (check_for_list_arg(argvars, 0) == FAIL)
	પાછા;

    FOR_ALL_LIST_વસ્તુઓ(argvars->vval.v_list, li)
    {
	જો (li->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_dictionary_required));
	    પાછા;
	}

	dict = li->li_tv.vval.v_dict;
	જો (!hlg_add_or_update(dict))
	    પાછા;
    }

    rettv->vval.v_સંખ્યા = 0;
}
#endif

/*
 * If "old" is in the override stack, then update it to "hl_new". Does not free
 * "old".
 */
    શૂન્ય
update_highlight_overrides(hl_override_T *old, hl_override_T *hl_new, પૂર્ણાંક newlen)
{
    જો (old == ખાલી)
	પાછા;

    માટે (hl_overrides_T *set = overrides; set != ખાલી; set = set->next)
    {
	જો (set->arr == old)
	{
	    set->arr = hl_new;
	    set->len = newlen;
	}
    }
}

/*
 * If "arr" is in the highlight overrides list, then mark it as invalid.
 */
    શૂન્ય
remove_highlight_overrides(hl_override_T *arr)
{
    જો (arr == ખાલી || overrides == ખાલી)
	પાછા;

    માટે (hl_overrides_T *set = overrides; set != ખાલી; set = set->next)
    {
	જો (set->arr == arr)
	{
	    set->arr = ખાલી;
	    તોડો;
	}
    }
}

/*
 * Update highlight_attr[] array. If "update_ids" is true, then update
 * highlight_ids[] array instead.
 */
    સ્થિર શૂન્ય
set_highlight_attr(hl_override_T *arr, પૂર્ણાંક len, bool update_ids)
{
    માટે (પૂર્ણાંક i = 0; i < len; i++)
    {
	hl_override_T	*override = arr + i;
	પૂર્ણાંક		hlf = -1, attr;

	જો (override->from <= 0)
	    // Directly map to highlight_attr
	    hlf = -override->from;
	નહીંતર
	{
	    માટે (પૂર્ણાંક k = 0; k < HLF_ગણતરી; k++)
		જો (override->from == highlight_ids[k])
		{
		    hlf = k;
		    તોડો;
		}

	    જો (hlf == -1)
		ચાલુ;
	}

	જો (update_ids)
	{
	    જો (override->to <= 0)
	    {
		જો (hlf == -override->to)
		    highlight_ids[hlf] = -1;
		નહીંતર
		    highlight_ids[hlf] = highlight_ids[-override->to];
	    }
	    નહીંતર
		highlight_ids[hlf] = override->to;
	}
	નહીંતર
	{
	    જો (override->to <= 0)
		// Directly use highlight_attr[]
		attr = highlight_attr[-override->to];
	    નહીંતર
		attr = syn_id2attr(override->to);

	    highlight_attr[hlf] = attr;
	}
    }
}

/*
 * Set the current highlight override to "arr". Returns true if successful
 */
    bool
ધકેલો_highlight_overrides(hl_override_T *arr, પૂર્ણાંક len)
{
    // Don't want to do anything if "arr" is already the current override. If
    // "arr" is NULL (but overrides->arr is not), then still push an override,
    // but "->arr" will just be NULL so any previous overrides are cleared.
    જો (overrides != ખાલી && overrides->arr == arr)
	પાછા false;

    hl_overrides_T *set;

    set = ALLOC_ONE(hl_overrides_T);
    જો (set == ખાલી)
	પાછા false;

    set->arr = arr;
    set->len = len;

    // Push to front of list
    set->next = overrides;
    overrides = set;

    // Save current highlight_attr[], so it can be restored later after this
    // override is popped.
    memcpy(set->attr, highlight_attr, કદ(highlight_attr));

    // Reset highlight_attr[] to default values, as we don't want changes from
    // other overrides.
    memcpy(highlight_attr, highlight_attr_raw, કદ(highlight_attr));

    // Update highlight_attr[] array
    જો (arr != ખાલી)
	set_highlight_attr(arr, len, false);

    પાછા true;
}

/*
 * Pop the current highlight override (if any) from the stack. This should
 * always be paired with a *successful* push.
 */
    શૂન્ય
કાઢો_highlight_overrides(શૂન્ય)
{
    hl_overrides_T *set = overrides;

    જો (overrides == ખાલી)
	પાછા;

    overrides = set->next;

    // Set highlight_attr[] to state before override was pushed.
    memcpy(highlight_attr, set->attr, કદ(highlight_attr));
    vim_free(set);
}

/*
 * Parse the 'winhighlight' option and return array. Returns NULL on failure or
 * if empty option. If failure, then errmsg is set.
 */
    સ્થિર hl_override_T *
parse_winhighlight(char_u *opt, પૂર્ણાંક *len, અક્ષર **errmsg)
{
    char_u	    *p = opt;
    hl_override_T   *arr;
    પૂર્ણાંક		    i = 0;
    પૂર્ણાંક		    અંક = 1;
    પૂર્ણાંક		    n_colons = 0;

    જો (*p == NUL)
	પાછા ખાલી;

    // Get number of overrides first so we can allocate array
    જ્યારે ((p = vim_strchr(p, ',')) != ખાલી)
    {
	p++;
	અંક++;
    }
    p = opt;
    // Check if number of ':' matches number of ','
    જ્યારે ((p = vim_strchr(p, ':')) != ખાલી)
    {
	p++;
	n_colons++;
    }

    જો (અંક != n_colons)
    {
	*errmsg = e_invalid_argument;
	પાછા ખાલી;
    }

    arr = ALLOC_MULT(hl_override_T, અંક);
    જો (arr == ખાલી)
    {
	*errmsg = e_out_of_memory;
	પાછા ખાલી;
    }

    p = opt;

    જ્યારે (true)
    {
	hl_override_T *override = arr + i++;
	char_u	*fromname = p, *toname;
	char_u	*tmp;
	char_u	**names[2] = {&fromname, &toname};
	પૂર્ણાંક	fromlen, tolen;
	પૂર્ણાંક	*lens[2] = {&fromlen, &tolen};
	પૂર્ણાંક	fromid, toid;
	પૂર્ણાંક	*ids[2] = {&fromid, &toid};

	p = vim_strchr(p, ':');

	જો (p == ખાલી)
	    જાઓ fail;

	fromlen = p - fromname; // Get hl for "from"
	જો (fromlen == 0)
	    જાઓ fail;
	p++; // Skip colon ':'
	જો (*p == NUL)
	    જાઓ fail;

	toname = p;
	tmp = vim_strchr(p, ',');

	// Get hl for "to", must check for no trailing comma in case last
	// element.
	જો (tmp == ખાલી)
	    tolen = (પૂર્ણાંક)STRLEN(p);
	નહીંતર
	{
	    tolen = tmp - toname;
	    p = ++tmp;
	}
	જો (tolen == 0)
	    જાઓ fail;

	માટે (પૂર્ણાંક k = 0; k < 2; k++)
	{
	    char_u  *name = *names[k];
	    પૂર્ણાંક	    nlen = *lens[k];

	    જો (*name == '!')
	    {
		// If starts with "!", then it maps directly to a 'highlight'
		// occasion.
		પૂર્ણાંક hlf;

		જો (nlen != 2)
		    જાઓ fail;

		માટે (hlf = 0; hlf < (પૂર્ણાંક)HLF_ગણતરી; ++hlf)
		    જો (hl_flags[hlf] == name[1])
			તોડો;

		જો (hlf >= HLF_ગણતરી)
		    જાઓ fail;

		*ids[k] = -hlf;
	    }
	    નહીંતર
	    {
		// Otherwise get the highlight group id.
		જો (syn_check_group(name, nlen) == 0)
		    જાઓ fail;

		*ids[k] = syn_namen2id(name, nlen);
		જો (*ids[k] == 0)
		    જાઓ fail;

		// Always map "Normal" group to HLF_WIN
		જો (ids[k] == &fromid &&
			STRCMP(HL_TABLE()[*ids[k] - 1].sg_name_u, "NORMAL") == 0)
		    *ids[k] = -HLF_WIN;
	    }
	}

	// If fromid == toid, leave it be, this is so that we know HLF_WIN has
	// been set (even to itself e.g. "Normal"), and therefore know to use
	// its attr instead.
	override->from = fromid;
	override->to = toid;

	જો (tmp == ખાલી)
	    તોડો;
    }

    *len = અંક;
    પાછા arr;
fail:
    vim_free(arr);
    *errmsg = e_invalid_argument;
    પાછા ખાલી;
}

/*
 * Update w_hl for "wp" using given option value. Returns error
 * message on failure, otherwise NULL.
 */
    અક્ષર *
update_winhighlight(win_T *wp, char_u *opt)
{
    અક્ષર	    *err = ખાલી;
    hl_override_T   *arr;
    પૂર્ણાંક		    અંક = 1;

    arr = parse_winhighlight(opt, &અંક, &err);

    જો (arr == ખાલી && err != ખાલી)
	પાછા err;

    જો (arr == ખાલી && wp->w_hl == ખાલી)
	પાછા ખાલી;

    update_highlight_overrides(wp->w_hl, arr, અંક);

    vim_free(wp->w_hl);
    wp->w_hl = arr;
    wp->w_hl_len = અંક;

    wp->w_hlfwin_id = hlf_get_id(wp, HLF_WIN);

#ifdef FEAT_TERMINAL
    term_update_hlfwin(wp);
#endif

#ifdef FEAT_TERMINAL
    // May be NULL (such as in after_copy_winopt())
    જો (wp->w_buffer != ખાલી)
    {
	// Make sure terminal highlighting is updated
	bool ret = ધકેલો_highlight_overrides(wp->w_hl, wp->w_hl_len);

	જો (wp->w_buffer->b_term != ખાલી)
	    term_init_default_colors(wp->w_buffer->b_term);
	જો (ret)
	    કાઢો_highlight_overrides();
    }
#endif

    પાછા ખાલી;
}

/*
 * Get id that the hlf attr is using in window "wp", otherwise zero. Note that
 * -1 may be returned if the hlf was set to itself.
 */
    પૂર્ણાંક
hlf_get_id(win_T *wp, પૂર્ણાંક hlf)
{
    પૂર્ણાંક id;
    સ્થિર પૂર્ણાંક prev[HLF_ગણતરી];

    જો (wp->w_hl == ખાલી)
	પાછા highlight_ids[hlf];

    memcpy(prev, highlight_ids, કદ(highlight_ids));
    set_highlight_attr(wp->w_hl, wp->w_hl_len, true);
    id = highlight_ids[hlf];
    memcpy(highlight_ids, prev, કદ(highlight_ids));

    પાછા id;
}

/*
 * Update 'winhighlight' using a 'wincolor' style option value. Returns error
 * message on failure otherwise NULL. Does not update the actual 'wincolor'
 * value, but does update the 'winhighlight' value.
 */
    અક્ષર *
update_wincolor(win_T *wp, char_u *opt)
{
    char_u  *str = *opt == NUL ? "" : alloc(કદ("!(:") + STRLEN(opt));
    અક્ષર    *errmsg;

    જો (str == ખાલી)
	પાછા e_out_of_memory;

    જો (*opt != NUL)
	sprintf((અક્ષર *)str, "!(:%s", opt);

    errmsg = update_winhighlight(wp, str);

    જો (errmsg == ખાલી)
	set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		str, OPT_FREE|OPT_LOCAL, 0);

    જો (*opt != NUL)
	મુક્ત(str);
    પાછા errmsg;
}
