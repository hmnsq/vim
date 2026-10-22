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

#include "vim.h"

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
static keyvalue_T highlight_tab[] = {
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
static keyvalue_T *highlight_index_tab[] = {
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

ga_nanaa {
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
static keyvalue_T color_name_tab[] = {
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
typedef sa_nracanaa
{
    char_u	*sg_name;	// highlight group name
    char_u	*sg_name_u;	// uppercase of sg_name
    පූර්ණ_සංඛ්‍යා		sg_cleared;	// "hi clear" was used
// for normal terminals
    පූර්ණ_සංඛ්‍යා		sg_term;	// "term=" highlighting attributes
    char_u	*sg_start;	// terminal string for start highl
    char_u	*sg_stop;	// terminal string for stop highl
    පූර්ණ_සංඛ්‍යා		sg_term_attr;	// Screen attr for term mode
// for color terminals
    පූර්ණ_සංඛ්‍යා		sg_cterm;	// "cterm=" highlighting attr
    පූර්ණ_සංඛ්‍යා		sg_cterm_bold;	// bold attr was set for light color
    පූර්ණ_සංඛ්‍යා		sg_cterm_fg;	// terminal fg color number + 1
    පූර්ණ_සංඛ්‍යා		sg_cterm_bg;	// terminal bg color number + 1
    පූර්ණ_සංඛ්‍යා		sg_cterm_ul;	// terminal ul color number + 1
    පූර්ණ_සංඛ්‍යා		sg_cterm_attr;	// Screen attr for color term mode
    පූර්ණ_සංඛ්‍යා		sg_cterm_font;	// terminal alternative font (0 for normal)
// for when using the GUI
#නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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
    පූර්ණ_සංඛ්‍යා		sg_gui_attr;    // Screen attr for GUI mode
#endif
#නම් defined(FEAT_GUI) || defined(FEAT_EVAL)
// Store the sp color name for the GUI or synIDattr()
    පූර්ණ_සංඛ්‍යා		sg_gui;		// "gui=" highlighting attributes
    char_u	*sg_gui_fg_name;// GUI foreground color name
    char_u	*sg_gui_bg_name;// GUI background color name
    char_u	*sg_gui_sp_name;// GUI special color name
#endif
    පූර්ණ_සංඛ්‍යා		sg_link;	// link to this highlight group ID
    පූර්ණ_සංඛ්‍යා		sg_deflink;	// default link; restored in highlight_clear()
    පූර්ණ_සංඛ්‍යා		sg_set;		// combination of SG_* flags
#ifdef FEAT_EVAL
    sctx_T	sg_deflink_sctx;  // script where the default link was set
    sctx_T	sg_script_ctx;	// script in which the group was last set
#endif
} hl_group_T;

typedef sa_nracanaa hl_overrides_S hl_overrides_T;
sa_nracanaa hl_overrides_S
{
    hl_override_T   *arr;   // May be NULL if "arr" was freed or no highlight
			    // overrides (all values set to default)
    පූර්ණ_සංඛ්‍යා		    len;
    hl_overrides_T  *next;  // Used to handle recursive calls

    පූර්ණ_සංඛ්‍යා attr[HLF_COUNT];    // highlight_attr[] before being updated.
};

static hl_overrides_T *overrides = NULL;

// Synced with highlight_attr, each is the highlight group id for each attr. If
// an element is 0, then there is no attr for it. If an element is -1, then it
// means it was set to itself.
static පූර්ණ_සංඛ්‍යා highlight_ids[HLF_COUNT];

// Same as highlight_attr[] but is not modified by highlight group overrides
static පූර්ණ_සංඛ්‍යා highlight_attr_raw[HLF_COUNT];

static පූර්ණ_සංඛ්‍යා hl_flags[HLF_COUNT] = HL_FLAGS;

// highlight groups for 'highlight' option
static garray_T highlight_ga;
#define HL_TABLE()	((hl_group_T *)((highlight_ga.ga_data)))

// Wrapper struct for hashtable entries: stores the highlight group ID alongside
// the uppercase name used as a hash key.  Uses the same offsetof pattern as
// signgroup_T / HI2SG.
typedef sa_nracanaa {
    පූර්ණ_සංඛ්‍යා		hn_id;		// highlight group ID (1-based)
    char_u	hn_key[1];	// uppercase name (stretchable array)
} hlname_T;

#define HLNAME_KEY_OFF	offsetof(hlname_T, hn_key)
#define HI2HLNAME(hi)	((hlname_T *)((hi)->hi_key - HLNAME_KEY_OFF))

// hashtable for quick highlight group name lookup
static hashtab_T highlight_ht;
static bool	 highlight_ht_inited = false;

/*
 * An attribute number is the index in attr_table plus ATTR_OFF.
 */
#define ATTR_OFF (HL_ALL + 1)

static void syn_unadd_group(void);
static void set_hl_attr(පූර්ණ_සංඛ්‍යා idx);
static void highlight_list_one(පූර්ණ_සංඛ්‍යා id);
static පූර්ණ_සංඛ්‍යා highlight_list_arg(පූර්ණ_සංඛ්‍යා id, පූර්ණ_සංඛ්‍යා didh, පූර්ණ_සංඛ්‍යා type, පූර්ණ_සංඛ්‍යා iarg, char_u *sarg, var_na *name);
static පූර්ණ_සංඛ්‍යා syn_add_group(char_u *name);
static පූර්ණ_සංඛ්‍යා syn_name2id_len(char_u *name, පූර්ණ_සංඛ්‍යා len);
static පූර්ණ_සංඛ්‍යා hl_has_settings(පූර්ණ_සංඛ්‍යා idx, පූර්ණ_සංඛ්‍යා check_link);
static void highlight_clear(පූර්ණ_සංඛ්‍යා idx);

#නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
static void gui_do_one_color(පූර්ණ_සංඛ්‍යා idx, පූර්ණ_සංඛ්‍යා do_menu, පූර්ණ_සංඛ්‍යා do_tooltip);
#endif
#ifdef FEAT_GUI
static පූර්ණ_සංඛ්‍යා  set_group_colors(char_u *name, guicolor_T *fgp, guicolor_T *bgp, පූර්ණ_සංඛ්‍යා do_menu, පූර්ණ_සංඛ්‍යා use_norm, පූර්ණ_සංඛ්‍යා do_tooltip);
static void hl_do_font(පූර්ණ_සංඛ්‍යා idx, char_u *arg, පූර්ණ_සංඛ්‍යා do_normal, පූර්ණ_සංඛ්‍යා do_menu, පූර්ණ_සංඛ්‍යා do_tooltip, පූර්ණ_සංඛ්‍යා free_font);
#endif

/*
 * The default highlight groups.  These are compiled-in for fast startup and
 * they still work when the runtime files can't be found.
 * When making changes here, also change runtime/colors/default.vim!
 * The #ifdefs are needed to reduce the amount of static data.  Helps to make
 * the 16 bit DOS (museum) version compile.
 */
#නම් defined(FEAT_GUI) || defined(FEAT_EVAL)
# define CENT(a, b) b
#නැතහොත්
# define CENT(a, b) a
#endif
static var_na *(highlight_init_both[]) = {
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
    NULL
};

// Default colors only used with a light background.
static var_na *(highlight_init_light[]) = {
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
    NULL
};

// Default colors only used with a dark background.
static var_na *(highlight_init_dark[]) = {
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
    NULL
};

#නම් defined(FEAT_SYN_HL)
/*
 * Returns the number of highlight groups.
 */
    පූර්ණ_සංඛ්‍යා
highlight_num_groups(void)
{
    ආපසු highlight_ga.ga_len;
}

/*
 * Returns the name of a highlight group.
 */
    char_u *
highlight_group_name(පූර්ණ_සංඛ්‍යා id)
{
    ආපසු HL_TABLE()[id].sg_name;
}

/*
 * Returns the ID of the link to a highlight group.
 */
    පූර්ණ_සංඛ්‍යා
highlight_link_id(පූර්ණ_සංඛ්‍යා id)
{
    ආපසු HL_TABLE()[id].sg_link;
}
#endif

static void resolve_fallback_fg_to_rgb(void);
static void resolve_fallback_bg_to_rgb(void);

    void
init_highlight(
    පූර්ණ_සංඛ්‍යා		both,	    // include groups where 'bg' doesn't matter
    පූර්ණ_සංඛ්‍යා		reset)	    // clear group first
{
    පූර්ණ_සංඛ්‍යා		i;
    var_na	**pp;
    static පූර්ණ_සංඛ්‍යා	had_both = FALSE;
#ifdef FEAT_EVAL
    char_u	*p;

    // Try finding the color scheme file.  Used when a color file was loaded
    // and 'background' or 't_Co' is changed.
    p = get_var_value((char_u *)"g:colors_name");
    නම් (p != NULL)
    {
	// The value of g:colors_name could be freed when sourcing the script,
	// making "p" invalid, so copy it.
	char_u *copy_p = vim_strsave(p);
	පූර්ණ_සංඛ්‍යා    r;

	නම් (copy_p != NULL)
	{
	    r = load_colors(copy_p);
	    vim_free(copy_p);
	    නම් (r == OK)
		ආපසු;
	}
    }

#endif

    // Didn't use a color file, use the compiled-in colors.
    නම් (both)
    {
	had_both = TRUE;
	pp = highlight_init_both;
	සඳහා (i = 0; pp[i] != NULL; ++i)
	    do_highlight((char_u *)pp[i], reset, TRUE);
    }
    නැතහොත් නම් (!had_both)
	// Don't do anything before the call with both == TRUE from main().
	// Not everything has been setup then, and that call will overrule
	// everything anyway.
	ආපසු;

    නම් (*p_bg == 'l')
	pp = highlight_init_light;
    නැතහොත්
	pp = highlight_init_dark;
    සඳහා (i = 0; pp[i] != NULL; ++i)
	do_highlight((char_u *)pp[i], reset, TRUE);

    // Reverse looks ugly, but grey may not work for less than 8 colors.  Thus
    // let it depend on the number of colors available.
    නම් (t_colors < 8)
	do_highlight((char_u *)"Visual term=reverse cterm=reverse ctermbg=NONE ctermfg=NONE",
		FALSE, TRUE);
    // With 8 colors brown is equal to yellow, need to use black for Search fg
    // to avoid Statement highlighted text disappears.
    // Clear the attributes, needed when changing the t_Co value.
    නම් (t_colors <= 8)
    {
	නම් (*p_bg == 'l')
	    do_highlight((char_u *)"Search ctermfg=black", FALSE, TRUE);
    }

#ifdef FEAT_SYN_HL
    // If syntax highlighting is enabled load the highlighting for it.
    නම් (get_var_value((char_u *)"g:syntax_on") != NULL)
    {
	static පූර්ණ_සංඛ්‍යා	recursive = 0;

	නම් (recursive >= 5)
	    emsg(_(e_recursive_loop_loading_syncolor_vim));
	නැතහොත්
	{
	    ++recursive;
	    (void)source_runtime((char_u *)"syntax/syncolor.vim", DIP_ALL);
	    --recursive;
	}
    }
#endif
    resolve_fallback_fg_to_rgb();
    resolve_fallback_bg_to_rgb();
}

#නම් defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
/*
 * Load a default color list. Intended to support legacy color names but allows
 * the user to override the color values. Only loaded once.
 */
    static void
load_default_colors_lists(void)
{
    // Lacking a default color list isn't the end of the world but it is likely
    // an inconvenience so users should know when it is missing.
    නම් (source_runtime((char_u *)"colors/lists/default.vim", DIP_ALL) != OK)
	msg("failed to load colors/lists/default.vim");
}
#endif

/*
 * Load color file "name".
 * Return OK for success, FAIL for failure.
 */
    පූර්ණ_සංඛ්‍යා
load_colors(char_u *name)
{
    char_u	*buf;
    පූර්ණ_සංඛ්‍යා		retval = FAIL;
    static පූර්ණ_සංඛ්‍යා	recursive = FALSE;

    // When being called recursively, this is probably because setting
    // 'background' caused the highlighting to be reloaded.  This means it is
    // working, thus we should return OK.
    නම් (recursive)
	ආපසු OK;

    recursive = TRUE;
    buf = alloc(STRLEN(name) + 12);
    නම් (buf != NULL)
    {
#නම් defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	load_default_colors_lists();
#endif
	apply_autocmds(EVENT_COLORSCHEMEPRE, name,
					       curbuf->b_fname, FALSE, curbuf);
	sprintf((var_na *)buf, "colors/%s.vim", name);
	retval = source_runtime(buf, DIP_START + DIP_OPT);
	vim_free(buf);
	නම් (retval == OK)
	    apply_autocmds(EVENT_COLORSCHEME, name, curbuf->b_fname,
								FALSE, curbuf);
    }
    recursive = FALSE;

    ආපසු retval;
}

static පූර්ණ_සංඛ්‍යා color_numbers_16[28] = {0, 1, 2, 3,
				 4, 5, 6, 6,
				 7, 7, 7, 7,
				 8, 8,
				 9, 9, 10, 10,
				 11, 11, 12, 12, 13,
				 13, 14, 14, 15, -1};
// for xterm with 88 colors...
static පූර්ණ_සංඛ්‍යා color_numbers_88[28] = {0, 4, 2, 6,
				 1, 5, 32, 72,
				 84, 84, 7, 7,
				 82, 82,
				 12, 43, 10, 61,
				 14, 63, 9, 74, 13,
				 75, 11, 78, 15, -1};
// for xterm with 256 colors...
static පූර්ණ_සංඛ්‍යා color_numbers_256[28] = {0, 4, 2, 6,
				 1, 5, 130, 3,
				 248, 248, 7, 7,
				 242, 242,
				 12, 81, 10, 121,
				 14, 159, 9, 224, 13,
				 225, 11, 229, 15, -1};
// for terminals with less than 16 colors...
static පූර්ණ_සංඛ්‍යා color_numbers_8[28] = {0, 4, 2, 6,
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
    static පූර්ණ_සංඛ්‍යා
lookup_color(පූර්ණ_සංඛ්‍යා idx, පූර්ණ_සංඛ්‍යා foreground, පූර්ණ_සංඛ්‍යා *boldp)
{
    පූර්ණ_සංඛ්‍යා		color = color_numbers_16[idx];
    char_u	*p;

    // Use the _16 table to check if it's a valid color name.
    නම් (color < 0)
	ආපසු -1;

    නම් (t_colors == 8)
    {
	// t_Co is 8: use the 8 colors table
#නම් defined(__QNXNTO__)
	// On qnx, the 8 & 16 color arrays are the same
	නම් (STRNCMP(T_NAME, "qansi", 5) == 0)
	    color = color_numbers_16[idx];
	නැතහොත්
#endif
	    color = color_numbers_8[idx];
	නම් (foreground)
	{
	    // set/reset bold attribute to get light foreground
	    // colors (on some terminals, e.g. "linux")
	    නම් (color & 8)
		*boldp = TRUE;
	    නැතහොත්
		*boldp = FALSE;
	}
	color &= 7;	// truncate to 8 colors
    }
    නැතහොත් නම් (t_colors == 16 || t_colors == 88
					   || t_colors >= 256)
    {
	// Guess: if the termcap entry ends in 'm', it is
	// probably an xterm-like terminal.  Use the changed
	// order for colors.
	නම් (*T_CAF != NUL)
	    p = T_CAF;
	නැතහොත්
	    p = T_CSF;
	නම් (*p != NUL && (t_colors > 256
			      || *(p + STRLEN(p) - 1) == 'm'))
	{
	    නම් (t_colors == 88)
		color = color_numbers_88[idx];
	    නැතහොත් නම් (t_colors >= 256)
		color = color_numbers_256[idx];
	    නැතහොත්
		color = color_numbers_8[idx];
	}
#ifdef FEAT_TERMRESPONSE
	නම් (t_colors >= 256 && color == 15 && is_mac_terminal)
	    // Terminal.app has a bug: 15 is light grey. Use white
	    // from the color cube instead.
	    color = 231;
#endif
    }
    ආපසු color;
}

/*
 * Link highlight group 'from_hg' to 'to_hg'.
 * 'dodefault' is set to TRUE for ":highlight default link".
 * 'forceit' is set to TRUE for ":highlight! link"
 * 'init' is set to TRUE when initializing all the highlight groups.
 */
    static void
highlight_group_link(
	char_u	*from_hg,
	පූර්ණ_සංඛ්‍යා	from_len,
	char_u	*to_hg,
	පූර්ණ_සංඛ්‍යා	to_len,
	පූර්ණ_සංඛ්‍යා	dodefault,
	පූර්ණ_සංඛ්‍යා	forceit,
	පූර්ණ_සංඛ්‍යා	init)
{
    පූර්ණ_සංඛ්‍යා		from_id;
    පූර්ණ_සංඛ්‍යා		to_id;
    hl_group_T	*hlgroup = NULL;

    from_id = syn_check_group(from_hg, from_len);
    නම් (STRNCMP(to_hg, "NONE", 4) == 0)
	to_id = 0;
    නැතහොත්
	to_id = syn_check_group(to_hg, to_len);

    නම් (from_id > 0)
    {
	hlgroup = &HL_TABLE()[from_id - 1];
	නම් (dodefault && (forceit || hlgroup->sg_deflink == 0))
	{
	    hlgroup->sg_deflink = to_id;
#ifdef FEAT_EVAL
	    hlgroup->sg_deflink_sctx = current_sctx;
	    hlgroup->sg_deflink_sctx.sc_lnum += SOURCING_LNUM;
#endif
	}
    }

    නම් (from_id > 0 && (!init || hlgroup->sg_set == 0))
    {
	// Don't allow a link when there already is some highlighting
	// for the group, unless '!' is used
	නම් (to_id > 0 && !forceit && !init
		&& hl_has_settings(from_id - 1, dodefault))
	{
	    නම් (SOURCING_NAME == NULL && !dodefault)
		emsg(_(e_group_has_settings_highlight_link_ignored));
	}
	නැතහොත් නම් (hlgroup->sg_link != to_id
#ifdef FEAT_EVAL
		|| hlgroup->sg_script_ctx.sc_sid != current_sctx.sc_sid
#endif
		|| hlgroup->sg_cleared)
	{
	    නම් (!init)
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
    static void
highlight_reset_all(void)
{
    පූර්ණ_සංඛ්‍යා		idx;

#ifdef FEAT_GUI
    // First, we do not destroy the old values, but allocate the new
    // ones and update the display. THEN we destroy the old values.
    // If we destroy the old values first, then the old values
    // (such as GuiFont's or GuiFontset's) will still be displayed but
    // invalid because they were free'd.
    නම් (gui.in_use)
    {
# ifdef FEAT_BEVAL_TIP
	gui_init_tooltip_font();
# endif
# නම් defined(FEAT_MENU) && defined(FEAT_GUI_MOTIF)
	gui_init_menu_font();
# endif
    }
# නම් defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_X11)
    gui_mch_def_colors();
# endif
# ifdef FEAT_GUI_MSWIN
    නම් (gui.in_use)
	gui_mch_set_titlebar_colors();
# endif
# ifdef FEAT_GUI_X11
#  ifdef FEAT_MENU

    // This only needs to be done when there is no Menu highlight
    // group defined by default, which IS currently the case.
    gui_mch_new_menu_colors();
#  endif
    නම් (gui.in_use)
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
    සඳහා (idx = 0; idx < highlight_ga.ga_len; ++idx)
	highlight_clear(idx);
    init_highlight(TRUE, TRUE);
#නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    නම් (USE_24BIT)
	highlight_gui_started();
    නැතහොත්
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
    static පූර්ණ_සංඛ්‍යා
highlight_set_termgui_attr(පූර්ණ_සංඛ්‍යා idx, char_u *key, char_u *arg, පූර්ණ_සංඛ්‍යා init)
{
    පූර්ණ_සංඛ්‍යා		attr;
    size_t	off;
    keyvalue_T	target;
    keyvalue_T	*entry;

    attr = 0;
    off = 0;
    target.key = 0;
    target.value.length = 0;	// not used, see cmp_keyvalue_value_ni()
    අතරතුර (arg[off] != NUL)
    {
	target.value.string = arg + off;
	entry = (keyvalue_T *)bsearch(&target, &highlight_tab,
		ARRAY_LENGTH(highlight_tab), parimaa_na(highlight_tab[0]),
		cmp_keyvalue_value_ni);
	නම් (entry == NULL)
	{
	    semsg(_(e_illegal_value_str), arg);
	    ආපසු FALSE;
	}

	attr |= entry->key;
	off += entry->value.length;
	නම් (arg[off] == ',')		// another one follows
	    ++off;
    }
    නම් (*key == 'T')
    {
	නම් (!init || !(HL_TABLE()[idx].sg_set & SG_TERM))
	{
	    නම් (!init)
		HL_TABLE()[idx].sg_set |= SG_TERM;
	    HL_TABLE()[idx].sg_term = attr;
	}
    }
    නැතහොත් නම් (*key == 'C')
    {
	නම් (!init || !(HL_TABLE()[idx].sg_set & SG_CTERM))
	{
	    නම් (!init)
		HL_TABLE()[idx].sg_set |= SG_CTERM;
	    HL_TABLE()[idx].sg_cterm = attr;
	    HL_TABLE()[idx].sg_cterm_bold = FALSE;
	}
    }
#නම් defined(FEAT_GUI) || defined(FEAT_EVAL)
    නැතහොත්
    {
	නම් (!init || !(HL_TABLE()[idx].sg_set & SG_GUI))
	{
	    නම් (!init)
		HL_TABLE()[idx].sg_set |= SG_GUI;
	    HL_TABLE()[idx].sg_gui = attr;
	}
    }
#endif

    ආපසු TRUE;
}

#ifdef FEAT_GUI
/*
 * Set the font for the highlight group at 'idx'.
 * 'arg' is the font name.
 * Returns TRUE if the font is changed.
 */
    static පූර්ණ_සංඛ්‍යා
highlight_set_font(
	පූර්ණ_සංඛ්‍යා	idx,
	char_u	*arg,
	පූර්ණ_සංඛ්‍යා	is_normal_group,
	පූර්ණ_සංඛ්‍යා	is_menu_group,
	පූර්ණ_සංඛ්‍යා	is_tooltip_group)
{
    පූර්ණ_සංඛ්‍යා		did_change = FALSE;

    // in non-GUI fonts are simply ignored
    නම් (HL_TABLE()[idx].sg_font_name != NULL
	    && STRCMP(HL_TABLE()[idx].sg_font_name, arg) == 0)
    {
	// Font name didn't change, ignore.
    }
    නැතහොත් නම් (!gui.shell_created)
    {
	// GUI not started yet, always accept the name.
	vim_free(HL_TABLE()[idx].sg_font_name);
	HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	did_change = TRUE;
    }
    නැතහොත්
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
	නම් (HL_TABLE()[idx].sg_fontset != NOFONTSET)
	{
	    // New fontset was accepted. Free the old one, if there
	    // was one.
	    gui_mch_free_fontset(temp_sg_fontset);
	    did_change = TRUE;
	}
	නැතහොත්
	    HL_TABLE()[idx].sg_fontset = temp_sg_fontset;
# endif
	නම් (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    // New font was accepted. Free the old one, if there was
	    // one.
	    gui_mch_free_font(temp_sg_font);
	    did_change = TRUE;
	}
	නැතහොත්
	    HL_TABLE()[idx].sg_font = temp_sg_font;
	// Update sg_font_name once here, after both font and fontset results
	// are known.
	නම් (did_change)
	{
	    vim_free(HL_TABLE()[idx].sg_font_name);
	    HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	}
    }

    ආපසු did_change;
}
#endif

/*
 * Set the cterm foreground color for the Normal highlight group to "color" and
 * the bold attribute to "bold".
 */
    static void
hl_set_ctermfg_normal_group(පූර්ණ_සංඛ්‍යා color, පූර්ණ_සංඛ්‍යා bold)
{
    cterm_normal_fg_color = color + 1;
    cterm_normal_fg_bold = bold;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    නම් (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	නම් (termcap_active && color >= 0)
	    term_fg_color(color);
    }
}

/*
 * Set the cterm foreground color for the highlight group at 'idx' to 'color'.
 */
    static void
highlight_set_ctermfg(පූර්ණ_සංඛ්‍යා idx, පූර්ණ_සංඛ්‍යා color, පූර්ණ_සංඛ්‍යා is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_fg = color + 1;
    නම් (is_normal_group)
	hl_set_ctermfg_normal_group(color,
					(HL_TABLE()[idx].sg_cterm & HL_BOLD));
}

/*
 * Set the cterm background color for the Normal highlight group to "color".
 */
    static void
hl_set_ctermbg_normal_group(පූර්ණ_සංඛ්‍යා color)
{
    cterm_normal_bg_color = color + 1;
#ifdef FEAT_GUI
    // Don't mess with 'background' if the GUI is used.
    නම් (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	නම් (color >= 0)
	{
	    පූර්ණ_සංඛ්‍යා dark = -1;

	    නම් (termcap_active)
		term_bg_color(color);
	    නම් (t_colors < 16)
		dark = (color == 0 || color == 4);
	    // Limit the heuristic to the standard 16 colors
	    නැතහොත් නම් (color < 16)
		dark = (color < 7 || color == 8);
	    // Set the 'background' option if the value is
	    // wrong.
	    නම් (dark != -1
		    && dark != (*p_bg == 'd')
		    && !option_was_set((char_u *)"bg"))
	    {
		set_option_value_give_err((char_u *)"bg",
			0L, (char_u *)(dark ? "dark" : "light"), 0);
		reset_option_was_set((char_u *)"bg");
	    }
	}
    }
}

/*
 * Set the cterm background color for the highlight group at 'idx' to 'color'.
 */
    static void
highlight_set_ctermbg(පූර්ණ_සංඛ්‍යා idx, පූර්ණ_සංඛ්‍යා color, පූර්ණ_සංඛ්‍යා is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_bg = color + 1;
    නම් (is_normal_group)
	hl_set_ctermbg_normal_group(color);
}

/*
 * Set the cterm underline color for the Normal highlight group to "color".
 */
    static void
hl_set_ctermul_normal_group(පූර්ණ_සංඛ්‍යා color)
{
    cterm_normal_ul_color = color + 1;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    නම් (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	නම් (termcap_active && color >= 0)
	    term_ul_color(color);
    }
}

/*
 * Set the cterm underline color for the highlight group at 'idx' to 'color'.
 */
    static void
highlight_set_ctermul(පූර්ණ_සංඛ්‍යා idx, පූර්ණ_සංඛ්‍යා color, පූර්ණ_සංඛ්‍යා is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_ul = color + 1;
    නම් (is_normal_group)
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
    static පූර්ණ_සංඛ්‍යා
highlight_set_cterm_font(
	පූර්ණ_සංඛ්‍යා	idx,
	char_u	*arg,
	පූර්ණ_සංඛ්‍යා	init)
{
    පූර්ණ_සංඛ්‍යා		font;

    නම් (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	ආපසු FALSE;

    නම් (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    නම් (VIM_ISDIGIT(*arg))
	font = atoi((var_na *)arg);
    නැතහොත් නම් (STRICMP(arg, "NONE") == 0)
	font = -1;
    නැතහොත්
	ආපසු FALSE;

    HL_TABLE()[idx].sg_cterm_font = font + 1;
    ආපසු TRUE;
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
    static පූර්ණ_සංඛ්‍යා
highlight_set_cterm_color(
	පූර්ණ_සංඛ්‍යා	idx,
	char_u	*key,
	char_u	*key_start,
	char_u	*arg,
	පූර්ණ_සංඛ්‍යා	is_normal_group,
	පූර්ණ_සංඛ්‍යා	init)
{
    පූර්ණ_සංඛ්‍යා		color;

    නම් (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	ආපසු FALSE;

    නම් (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    // When setting the foreground color, and previously the "bold"
    // flag was set for a light color, reset it now
    නම් (key[5] == 'F' && HL_TABLE()[idx].sg_cterm_bold)
    {
	HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
	HL_TABLE()[idx].sg_cterm_bold = FALSE;
    }

    නම් (VIM_ISDIGIT(*arg))
	color = atoi((var_na *)arg);
    නැතහොත් නම් (STRICMP(arg, "fg") == 0)
    {
	නම් (cterm_normal_fg_color)
	    color = cterm_normal_fg_color - 1;
	නැතහොත්
	{
	    emsg(_(e_fg_color_unknown));
	    ආපසු FALSE;
	}
    }
    නැතහොත් නම් (STRICMP(arg, "bg") == 0)
    {
	නම් (cterm_normal_bg_color > 0)
	    color = cterm_normal_bg_color - 1;
	නැතහොත්
	{
	    emsg(_(e_bg_color_unknown));
	    ආපසු FALSE;
	}
    }
    නැතහොත් නම් (STRICMP(arg, "ul") == 0)
    {
	නම් (cterm_normal_ul_color > 0)
	    color = cterm_normal_ul_color - 1;
	නැතහොත්
	{
	    emsg(_(e_ul_color_unknown));
	    ආපසු FALSE;
	}
    }
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා bold = MAYBE;
	keyvalue_T target;
	keyvalue_T *entry;

	target.key = 0;
	target.value.string = arg;
	target.value.length = 0;	// not used, see cmp_keyvalue_value_i()
	entry = (keyvalue_T *)bsearch(&target, &color_name_tab,
		ARRAY_LENGTH(color_name_tab), parimaa_na(color_name_tab[0]),
		cmp_keyvalue_value_i);
	නම් (entry == NULL)
	{
	    semsg(_(e_color_name_or_number_not_recognized_str), key_start);
	    ආපසු FALSE;
	}

	color = lookup_color(entry->key, key[5] == 'F', &bold);

	// set/reset bold attribute to get light foreground
	// colors (on some terminals, e.g. "linux")
	නම් (bold == TRUE)
	{
	    HL_TABLE()[idx].sg_cterm |= HL_BOLD;
	    HL_TABLE()[idx].sg_cterm_bold = TRUE;
	}
	නැතහොත් නම් (bold == FALSE)
	    HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
    }

    // Add one to the argument, to avoid zero.  Zero is used for
    // "NONE", then "color" is -1.
    නම් (key[5] == 'F')
	highlight_set_ctermfg(idx, color, is_normal_group);
    නැතහොත් නම් (key[5] == 'B')
	highlight_set_ctermbg(idx, color, is_normal_group);
    නැතහොත් // ctermul
	highlight_set_ctermul(idx, color, is_normal_group);

    ආපසු TRUE;
}

#නම් defined(FEAT_GUI) || defined(FEAT_EVAL)
/*
 * Set the GUI foreground color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    static පූර්ණ_සංඛ්‍යා
highlight_set_guifg(
	පූර්ණ_සංඛ්‍යා	idx,
	char_u	*arg,
	පූර්ණ_සංඛ්‍යා	is_menu_group UNUSED,
	පූර්ණ_සංඛ්‍යා	is_scrollbar_group UNUSED,
	පූර්ණ_සංඛ්‍යා	is_tooltip_group UNUSED,
	පූර්ණ_සංඛ්‍යා	is_titlebar_group UNUSED,
	පූර්ණ_සංඛ්‍යා	is_titlebarnc_group UNUSED,
	පූර්ණ_සංඛ්‍යා	*do_colors UNUSED,
	පූර්ණ_සංඛ්‍යා	init)
{
# නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    long	i;
# endif
    char_u	**namep;
    පූර්ණ_සංඛ්‍යා		did_change = FALSE;

    නම් (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	ආපසු FALSE;

    namep = &HL_TABLE()[idx].sg_gui_fg_name;
    නම් (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guifg colors are only used when recognized
    i = color_name2handle(arg);
    නම් (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_fg = i;
# endif
	නම් (*namep == NULL || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    නම් (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    නැතහොත්
		*namep = NULL;
	    did_change = TRUE;
	}
# නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	නම් (is_titlebar_group && gui.title_fg_pixel != i)
	{
	    gui.title_fg_pixel = i;
	    *do_colors = TRUE;
	}
	නම් (is_titlebarnc_group && gui.titlenc_fg_pixel != i)
	{
	    gui.titlenc_fg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	නම් (is_menu_group && gui.menu_fg_pixel != i)
	{
	    gui.menu_fg_pixel = i;
	    *do_colors = TRUE;
	}
	නම් (is_scrollbar_group && gui.scroll_fg_pixel != i)
	{
	    gui.scroll_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	නම් (is_tooltip_group && gui.tooltip_fg_pixel != i)
	{
	    gui.tooltip_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    ආපසු did_change;
}

/*
 * Set the GUI background color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    static පූර්ණ_සංඛ්‍යා
highlight_set_guibg(
	පූර්ණ_සංඛ්‍යා	idx,
	char_u	*arg,
	පූර්ණ_සංඛ්‍යා	is_menu_group UNUSED,
	පූර්ණ_සංඛ්‍යා	is_scrollbar_group UNUSED,
	පූර්ණ_සංඛ්‍යා	is_tooltip_group UNUSED,
	පූර්ණ_සංඛ්‍යා	is_titlebar_group UNUSED,
	පූර්ණ_සංඛ්‍යා	is_titlebarnc_group UNUSED,
	පූර්ණ_සංඛ්‍යා	*do_colors UNUSED,
	පූර්ණ_සංඛ්‍යා	init)
{
# නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    පූර්ණ_සංඛ්‍යා		i;
# endif
    char_u	**namep;
    පූර්ණ_සංඛ්‍යා		did_change = FALSE;

    නම් (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	ආපසු FALSE;

    namep = &HL_TABLE()[idx].sg_gui_bg_name;
    නම් (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guibg colors are only used when recognized
    i = color_name2handle(arg);
    නම් (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_bg = i;
# endif
	නම් (*namep == NULL || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    නම් (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    නැතහොත්
		*namep = NULL;
	    did_change = TRUE;
	}
# නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	නම් (is_titlebar_group && gui.title_bg_pixel != i)
	{
	    gui.title_bg_pixel = i;
	    *do_colors = TRUE;
	}
	නම් (is_titlebarnc_group && gui.titlenc_bg_pixel != i)
	{
	    gui.titlenc_bg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	නම් (is_menu_group && gui.menu_bg_pixel != i)
	{
	    gui.menu_bg_pixel = i;
	    *do_colors = TRUE;
	}
	නම් (is_scrollbar_group && gui.scroll_bg_pixel != i)
	{
	    gui.scroll_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	නම් (is_tooltip_group && gui.tooltip_bg_pixel != i)
	{
	    gui.tooltip_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    ආපසු did_change;
}

/*
 * Set the GUI undercurl/strikethrough color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    static පූර්ණ_සංඛ්‍යා
highlight_set_guisp(පූර්ණ_සංඛ්‍යා idx, char_u *arg, පූර්ණ_සංඛ්‍යා init)
{
# නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    පූර්ණ_සංඛ්‍යා		i;
# endif
    පූර්ණ_සංඛ්‍යා		did_change = FALSE;
    char_u	**namep;

    නම් (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	ආපසු FALSE;

    namep = &HL_TABLE()[idx].sg_gui_sp_name;
    නම් (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guisp colors are only used when recognized
    i = color_name2handle(arg);
    නම් (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_sp = i;
# endif
	නම් (*namep == NULL || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    නම් (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    නැතහොත්
		*namep = NULL;
	    did_change = TRUE;
	}
# නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    }
# endif

    ආපසු did_change;
}
#endif

/*
 * Set the start/stop terminal codes for a highlight group.
 * Returns TRUE if the terminal code is set.
 */
    static පූර්ණ_සංඛ්‍යා
highlight_set_startstop_termcode(පූර්ණ_සංඛ්‍යා idx, char_u *key, char_u *arg, පූර්ණ_සංඛ්‍යා init)
{
    පූර්ණ_සංඛ්‍යා		off;
    char_u	buf[100];
    පූර්ණ_සංඛ්‍යා		len;
    char_u	*tname;
    char_u	*p;

    නම් (!init)
	HL_TABLE()[idx].sg_set |= SG_TERM;

    // The "start" and "stop"  arguments can be a literal escape
    // sequence, or a comma separated list of terminal codes.
    නම් (STRNCMP(arg, "t_", 2) == 0)
    {
	off = 0;
	buf[0] = 0;
	අතරතුර (arg[off] != NUL)
	{
	    // Isolate one termcap name
	    සඳහා (len = 0; arg[off + len] &&
		    arg[off + len] != ','; ++len)
		;
	    tname = vim_strnsave(arg + off, len);
	    නම් (tname == NULL)		// out of memory
		ආපසු FALSE;
	    // lookup the escape sequence for the item
	    p = get_term_code(tname);
	    vim_free(tname);
	    නම් (p == NULL)	    // ignore non-existing things
		p = (char_u *)"";

	    // Append it to the already found stuff
	    නම් ((පූර්ණ_සංඛ්‍යා)(STRLEN(buf) + STRLEN(p)) >= 99)
	    {
		semsg(_(e_terminal_code_too_long_str), arg);
		ආපසු FALSE;
	    }
	    STRCAT(buf, p);

	    // Advance to the next item
	    off += len;
	    නම් (arg[off] == ',')	    // another one follows
		++off;
	}
    }
    නැතහොත්
    {
	// Copy characters from arg[] to buf[], translating <> codes.
	සඳහා (p = arg, off = 0; off < 100 - 6 && *p; )
	{
	    len = trans_special(&p, buf + off, FSK_SIMPLIFY, FALSE, NULL);
	    නම් (len > 0)	    // recognized special char
		off += len;
	    නැතහොත්		    // copy as normal char
		buf[off++] = *p++;
	}
	buf[off] = NUL;
    }

    නම් (STRCMP(buf, "NONE") == 0)	// resetting the value
	p = NULL;
    නැතහොත්
	p = vim_strsave(buf);
    නම් (key[2] == 'A')
    {
	vim_free(HL_TABLE()[idx].sg_start);
	HL_TABLE()[idx].sg_start = p;
    }
    නැතහොත්
    {
	vim_free(HL_TABLE()[idx].sg_stop);
	HL_TABLE()[idx].sg_stop = p;
    }
    ආපසු TRUE;
}

/*
 * Handle the ":highlight .." command.
 * When using ":hi clear" this is called recursively for each group with
 * "forceit" and "init" both TRUE.
 */
    void
do_highlight(
    char_u	*line,
    පූර්ණ_සංඛ්‍යා		forceit,
    පූර්ණ_සංඛ්‍යා		init)	    // TRUE when called for initializing
{
    char_u	*name_end;
    char_u	*linep;
    char_u	*key_start;
    char_u	*arg_start;
    char_u	*key = NULL, *arg = NULL;
    long	i;
    පූර්ණ_සංඛ්‍යා		id;
    පූර්ණ_සංඛ්‍යා		idx;
    hl_group_T	item_before;
    පූර්ණ_සංඛ්‍යා		did_change = FALSE;
    පූර්ණ_සංඛ්‍යා		dodefault = FALSE;
    පූර්ණ_සංඛ්‍යා		doclear = FALSE;
    පූර්ණ_සංඛ්‍යා		dolink = FALSE;
    පූර්ණ_සංඛ්‍යා		error = FALSE;
    පූර්ණ_සංඛ්‍යා		is_normal_group = FALSE;	// "Normal" group
#ifdef FEAT_GUI_MSWIN
    පූර්ණ_සංඛ්‍යා		is_titlebar_group = FALSE;	// "TitleBar" group
    පූර්ණ_සංඛ්‍යා		is_titlebarnc_group = FALSE;	// "TitleBarNC" group
#නැතහොත්
# define is_titlebar_group 0
# define is_titlebarnc_group 0
#endif
#ifdef FEAT_GUI_X11
    පූර්ණ_සංඛ්‍යා		is_menu_group = FALSE;		// "Menu" group
    පූර්ණ_සංඛ්‍යා		is_scrollbar_group = FALSE;	// "Scrollbar" group
    පූර්ණ_සංඛ්‍යා		is_tooltip_group = FALSE;	// "Tooltip" group
#නැතහොත්
# define is_menu_group 0
# define is_tooltip_group 0
# define is_scrollbar_group 0
#endif
#නම් defined(FEAT_GUI) || defined(FEAT_EVAL)
    පූර්ණ_සංඛ්‍යා		do_colors = FALSE;		// need to update colors?
#endif
#නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    පූර්ණ_සංඛ්‍යා		did_highlight_changed = FALSE;
#endif

    // If no argument, list current highlighting.
    නම් (!init && ends_excmd2(line - 1, line))
    {
	සඳහා (i = 1; i <= highlight_ga.ga_len && !got_int; ++i)
	    // TODO: only call when the group has attributes set
	    highlight_list_one((පූර්ණ_සංඛ්‍යා)i);
	ආපසු;
    }

    // Isolate the name.
    name_end = skiptowhite(line);
    linep = skipwhite(name_end);

    // Check for "default" argument.
    නම් (STRNCMP(line, "default", name_end - line) == 0)
    {
	dodefault = TRUE;
	line = linep;
	name_end = skiptowhite(line);
	linep = skipwhite(name_end);
    }

    // Check for "clear" or "link" argument.
    නම් (STRNCMP(line, "clear", name_end - line) == 0)
	doclear = TRUE;
    නම් (STRNCMP(line, "link", name_end - line) == 0)
	dolink = TRUE;

    // ":highlight {group-name}": list highlighting for one group.
    නම් (!doclear && !dolink && ends_excmd2(line, linep))
    {
	id = syn_namen2id(line, (පූර්ණ_සංඛ්‍යා)(name_end - line));
	නම් (id == 0)
	    semsg(_(e_highlight_group_name_not_found_str), line);
	නැතහොත්
	    highlight_list_one(id);
	ආපසු;
    }

    // Handle ":highlight link {from} {to}" command.
    නම් (dolink)
    {
	char_u	    *from_start = linep;
	char_u	    *from_end;
	පූර්ණ_සංඛ්‍යා	    from_len;
	char_u	    *to_start;
	char_u	    *to_end;
	පූර්ණ_සංඛ්‍යා	    to_len;

	from_end = skiptowhite(from_start);
	to_start = skipwhite(from_end);
	to_end	 = skiptowhite(to_start);

	නම් (ends_excmd2(line, from_start) || ends_excmd2(line, to_start))
	{
	    semsg(_(e_not_enough_arguments_highlight_link_str), from_start);
	    ආපසු;
	}

	නම් (!ends_excmd2(line, skipwhite(to_end)))
	{
	    semsg(_(e_too_many_arguments_highlight_link_str), from_start);
	    ආපසු;
	}

	from_len = (පූර්ණ_සංඛ්‍යා)(from_end - from_start);
	to_len = (පූර්ණ_සංඛ්‍යා)(to_end - to_start);
	highlight_group_link(from_start, from_len, to_start, to_len,
						dodefault, forceit, init);
	ආපසු;
    }

    නම් (doclear)
    {
	// ":highlight clear [group]" command.
	නම් (ends_excmd2(line, linep))
	{
	    // ":highlight clear" without group name
	    highlight_reset_all();
	    ආපසු;
	}
	line = linep;
	name_end = skiptowhite(line);
	linep = skipwhite(name_end);
    }

    // Find the group name in the table.  If it does not exist yet, add it.
    id = syn_check_group(line, (පූර්ණ_සංඛ්‍යා)(name_end - line));
    නම් (id == 0)	// failed (out of memory)
	ආපසු;
    idx = id - 1;	// index is ID minus one

    // Return if "default" was used and the group already has settings.
    නම් (dodefault && hl_has_settings(idx, TRUE))
	ආපසු;

    // Make a copy so we can check if any attribute actually changed.
    item_before = HL_TABLE()[idx];

    නම් (STRCMP(HL_TABLE()[idx].sg_name_u, "NORMAL") == 0)
	is_normal_group = TRUE;
#ifdef FEAT_GUI_MSWIN
    නැතහොත් නම් (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBAR") == 0)
	is_titlebar_group = TRUE;
    නැතහොත් නම් (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBARNC") == 0)
	is_titlebarnc_group = TRUE;
#endif
#ifdef FEAT_GUI_X11
    නැතහොත් නම් (STRCMP(HL_TABLE()[idx].sg_name_u, "MENU") == 0)
	is_menu_group = TRUE;
    නැතහොත් නම් (STRCMP(HL_TABLE()[idx].sg_name_u, "SCROLLBAR") == 0)
	is_scrollbar_group = TRUE;
    නැතහොත් නම් (STRCMP(HL_TABLE()[idx].sg_name_u, "TOOLTIP") == 0)
	is_tooltip_group = TRUE;
#endif

    // Clear the highlighting for ":hi clear {group}" and ":hi clear".
    නම් (doclear || (forceit && init))
    {
	highlight_clear(idx);
	නම් (!doclear)
	    HL_TABLE()[idx].sg_set = 0;
    }

    නම් (!doclear)
	අතරතුර (!ends_excmd2(line, linep))
	{
	    key_start = linep;
	    නම් (*linep == '=')
	    {
		semsg(_(e_unexpected_equal_sign_str), key_start);
		error = TRUE;
		bha_nga;
	    }

	    // Note: Keep this in sync with get_highlight_group_key.

	    // Isolate the key ("term", "ctermfg", "ctermbg", "font", "guifg"
	    // or "guibg").
	    අතරතුර (*linep && !VIM_ISWHITE(*linep) && *linep != '=')
		++linep;
	    vim_free(key);
	    key = vim_strnsave_up(key_start, linep - key_start);
	    නම් (key == NULL)
	    {
		error = TRUE;
		bha_nga;
	    }
	    linep = skipwhite(linep);

	    නම් (STRCMP(key, "NONE") == 0)
	    {
		නම් (!init || HL_TABLE()[idx].sg_set == 0)
		{
		    නම් (!init)
			HL_TABLE()[idx].sg_set |= SG_TERM+SG_CTERM+SG_GUI;
		    highlight_clear(idx);
		}
		continue;
	    }

	    // Check for the equal sign.
	    නම් (*linep != '=')
	    {
		semsg(_(e_missing_equal_sign_str_2), key_start);
		error = TRUE;
		bha_nga;
	    }
	    ++linep;

	    // Isolate the argument.
	    linep = skipwhite(linep);
	    නම් (*linep == '\'')		// guifg='color name'
	    {
		arg_start = ++linep;
		linep = vim_strchr(linep, '\'');
		නම් (linep == NULL)
		{
		    semsg(_(e_invalid_argument_str), key_start);
		    error = TRUE;
		    bha_nga;
		}
	    }
	    නැතහොත්
	    {
		arg_start = linep;
		linep = skiptowhite(linep);
	    }
	    නම් (linep == arg_start)
	    {
		semsg(_(e_missing_argument_str), key_start);
		error = TRUE;
		bha_nga;
	    }
	    vim_free(arg);
	    arg = vim_strnsave(arg_start, linep - arg_start);
	    නම් (arg == NULL)
	    {
		error = TRUE;
		bha_nga;
	    }
	    නම් (*linep == '\'')
		++linep;

	    // Store the argument.
	    නම් (STRCMP(key, "TERM") == 0
		    || STRCMP(key, "CTERM") == 0
		    || STRCMP(key, "GUI") == 0)
	    {
		නම් (!highlight_set_termgui_attr(idx, key, arg, init))
		{
		    error = TRUE;
		    bha_nga;
		}
	    }
	    නැතහොත් නම් (STRCMP(key, "FONT") == 0)
	    {
		// in non-GUI fonts are simply ignored
#ifdef FEAT_GUI
		නම් (highlight_set_font(idx, arg, is_normal_group,
					      is_menu_group, is_tooltip_group))
		    did_change = TRUE;
#endif
	    }
	    නැතහොත් නම් (STRCMP(key, "CTERMFG") == 0
		    || STRCMP(key, "CTERMBG") == 0
		    || STRCMP(key, "CTERMUL") == 0)
	    {
		නම් (!highlight_set_cterm_color(idx, key, key_start, arg,
							is_normal_group, init))
		{
		    error = TRUE;
		    bha_nga;
		}
	    }
	    නැතහොත් නම් (STRCMP(key, "CTERMFONT") == 0)
	    {
		නම් (!highlight_set_cterm_font(idx, arg, init))
		{
		    error = TRUE;
		    bha_nga;
		}
	    }
	    නැතහොත් නම් (STRCMP(key, "GUIFG") == 0)
	    {
#නම් defined(FEAT_GUI) || defined(FEAT_EVAL)
		නම් (highlight_set_guifg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    නැතහොත් නම් (STRCMP(key, "GUIBG") == 0)
	    {
#නම් defined(FEAT_GUI) || defined(FEAT_EVAL)
		නම් (highlight_set_guibg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    නැතහොත් නම් (STRCMP(key, "GUISP") == 0)
	    {
#නම් defined(FEAT_GUI) || defined(FEAT_EVAL)
		නම් (highlight_set_guisp(idx, arg, init))
		    did_change = TRUE;
#endif
	    }
	    නැතහොත් නම් (STRCMP(key, "START") == 0 || STRCMP(key, "STOP") == 0)
	    {
		නම් (!highlight_set_startstop_termcode(idx, key, arg, init))
		{
		    error = TRUE;
		    bha_nga;
		}
	    }
	    නැතහොත්
	    {
		semsg(_(e_illegal_argument_str_3), key_start);
		error = TRUE;
		bha_nga;
	    }
	    HL_TABLE()[idx].sg_cleared = FALSE;

	    // When highlighting has been given for a group, don't link it.
	    නම් (!init || !(HL_TABLE()[idx].sg_set & SG_LINK))
		HL_TABLE()[idx].sg_link = 0;

	    // Continue with next argument.
	    linep = skipwhite(linep);
	}

    // If there is an error, and it's a new entry, remove it from the table.
    නම් (error && idx == highlight_ga.ga_len)
	syn_unadd_group();
    නැතහොත්
    {
	නම් (is_normal_group)
	{
	    HL_TABLE()[idx].sg_term_attr = 0;
	    HL_TABLE()[idx].sg_cterm_attr = 0;
#ifdef FEAT_GUI
	    HL_TABLE()[idx].sg_gui_attr = 0;
	    // Need to update all groups, because they might be using "bg"
	    // and/or "fg", which have been changed now.
#endif
#නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    නම් (USE_24BIT)
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
	නැතහොත් නම් (is_titlebar_group)
	{
	    නම් (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
	නැතහොත් නම් (is_titlebarnc_group)
	{
	    නම් (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
#endif
#ifdef FEAT_GUI_X11
# ifdef FEAT_MENU
	නැතහොත් නම් (is_menu_group)
	{
	    නම් (gui.in_use && do_colors)
		gui_mch_new_menu_colors();
	}
# endif
	නැතහොත් නම් (is_scrollbar_group)
	{
	    නම් (gui.in_use && do_colors)
		gui_new_scrollbar_colors();
	    නැතහොත්
		set_hl_attr(idx);
	}
# ifdef FEAT_BEVAL_GUI
	නැතහොත් නම් (is_tooltip_group)
	{
	    නම් (gui.in_use && do_colors)
		gui_mch_new_tooltip_colors();
	}
# endif
#endif
	නැතහොත්
	    set_hl_attr(idx);
#ifdef FEAT_EVAL
	HL_TABLE()[idx].sg_script_ctx = current_sctx;
	HL_TABLE()[idx].sg_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
    }

    vim_free(key);
    vim_free(arg);

    // Only call highlight_changed() once, after a sequence of highlight
    // commands, and only if an attribute actually changed.
    නම් ((did_change
	   || memcmp(&HL_TABLE()[idx], &item_before, parimaa_na(item_before)) != 0)
#නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    && !did_highlight_changed
#endif
       )
    {
	// Do not trigger a redraw when highlighting is changed while
	// redrawing.  This may happen when evaluating 'statusline' changes the
	// StatusLine group.
	නම් (!updating_screen)
	    redraw_all_later(UPD_NOT_VALID);
	need_highlight_changed = TRUE;
    }
}

#නම් defined(EXITFREE)
    void
free_highlight(void)
{
    පූර්ණ_සංඛ්‍යා	    i;

    සඳහා (i = 0; i < highlight_ga.ga_len; ++i)
    {
	highlight_clear(i);
	vim_free(HL_TABLE()[i].sg_name);
	vim_free(HL_TABLE()[i].sg_name_u - HLNAME_KEY_OFF);
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
    void
restore_cterm_colors(void)
{
#නම් defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
    // Since t_me has been set, this probably means that the user
    // wants to use this as default colors.  Need to reset default
    // background/foreground colors.
    mch_set_normal_colors();
#නැතහොත්
# ifdef VIMDLL
    නම් (!gui.in_use)
    {
	mch_set_normal_colors();
	ආපසු;
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
    static පූර්ණ_සංඛ්‍යා
hl_has_settings(පූර්ණ_සංඛ්‍යා idx, පූර්ණ_සංඛ්‍යා check_link)
{
    ආපසු HL_TABLE()[idx].sg_cleared == 0
	 && (  HL_TABLE()[idx].sg_term_attr != 0
	    || HL_TABLE()[idx].sg_cterm_attr != 0
	    || HL_TABLE()[idx].sg_cterm_fg != 0
	    || HL_TABLE()[idx].sg_cterm_bg != 0
	    || HL_TABLE()[idx].sg_cterm_font != 0
#ifdef FEAT_GUI
	    || HL_TABLE()[idx].sg_gui_attr != 0
	    || HL_TABLE()[idx].sg_gui_fg_name != NULL
	    || HL_TABLE()[idx].sg_gui_bg_name != NULL
	    || HL_TABLE()[idx].sg_gui_sp_name != NULL
	    || HL_TABLE()[idx].sg_font_name != NULL
#endif
	    || (check_link && (HL_TABLE()[idx].sg_set & SG_LINK)));
}

/*
 * Clear highlighting for one group.
 */
    static void
highlight_clear(පූර්ණ_සංඛ්‍යා idx)
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
#නම් defined(FEAT_GUI) || defined(FEAT_EVAL)
    HL_TABLE()[idx].sg_gui = 0;
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_fg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_bg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_sp_name);
#endif
#නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

#නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Set the normal foreground and background colors according to the "Normal"
 * highlighting group.  For X11 also set "Menu", "Scrollbar" and "Tooltip"
 * colors.  For MS-Windows also set "TitleBar" and "TitleBarNC" colors.
 */
    void
set_normal_colors(void)
{
# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    නම් (gui.in_use)
#  endif
    {
	නම් (set_group_colors((char_u *)"Normal",
				 &gui.norm_pixel, &gui.back_pixel,
				 FALSE, TRUE, FALSE))
	{
	    gui_mch_new_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  ifdef FEAT_GUI_MSWIN
	නම් (set_group_colors((char_u *)"TitleBar",
				 &gui.title_fg_pixel, &gui.title_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
	නම් (set_group_colors((char_u *)"TitleBarNC",
				 &gui.titlenc_fg_pixel, &gui.titlenc_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  endif
#  ifdef FEAT_GUI_X11
	නම් (set_group_colors((char_u *)"Menu",
			     &gui.menu_fg_pixel, &gui.menu_bg_pixel,
			     TRUE, FALSE, FALSE))
	{
#   ifdef FEAT_MENU
	    gui_mch_new_menu_colors();
#   endif
	    set_must_redraw(UPD_CLEAR);
	}
#   ifdef FEAT_BEVAL_GUI
	නම් (set_group_colors((char_u *)"Tooltip",
			     &gui.tooltip_fg_pixel, &gui.tooltip_bg_pixel,
			     FALSE, FALSE, TRUE))
	{
#    ifdef FEAT_TOOLBAR
	    gui_mch_new_tooltip_colors();
#    endif
	    set_must_redraw(UPD_CLEAR);
	}
#   endif
	නම් (set_group_colors((char_u *)"Scrollbar",
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
    නැතහොත්
#  endif
    {
	පූර්ණ_සංඛ්‍යා		idx;

	idx = syn_name2id((char_u *)"Normal") - 1;
	නම් (idx >= 0)
	{
	    gui_do_one_color(idx, FALSE, FALSE);

	    // If the normal fg or bg color changed a complete redraw is
	    // required.
	    නම් (cterm_normal_fg_gui_color != HL_TABLE()[idx].sg_gui_fg
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

#නම් defined(FEAT_GUI)
/*
 * Set the colors for "Normal", "Menu", "TitleBar", "TitleBarNC", "Tooltip" or
 * "Scrollbar".
 */
    static පූර්ණ_සංඛ්‍යා
set_group_colors(
    char_u	*name,
    guicolor_T	*fgp,
    guicolor_T	*bgp,
    පූර්ණ_සංඛ්‍යා		do_menu,
    පූර්ණ_සංඛ්‍යා		use_norm,
    පූර්ණ_සංඛ්‍යා		do_tooltip)
{
    පූර්ණ_සංඛ්‍යා		idx;

    idx = syn_name2id(name) - 1;
    නම් (idx < 0)
	ආපසු FALSE;

    gui_do_one_color(idx, do_menu, do_tooltip);

    නම් (HL_TABLE()[idx].sg_gui_fg != INVALCOLOR)
	*fgp = HL_TABLE()[idx].sg_gui_fg;
    නැතහොත් නම් (use_norm)
	*fgp = gui.def_norm_pixel;
    නම් (HL_TABLE()[idx].sg_gui_bg != INVALCOLOR)
	*bgp = HL_TABLE()[idx].sg_gui_bg;
    නැතහොත් නම් (use_norm)
	*bgp = gui.def_back_pixel;
    ආපසු TRUE;
}

/*
 * Get the font of the "Normal" group.
 * Returns "" when it's not found or not set.
 */
    char_u *
hl_get_font_name(void)
{
    පූර්ණ_සංඛ්‍යා		id;
    char_u	*s;

    id = syn_name2id((char_u *)"Normal");
    නම් (id > 0)
    {
	s = HL_TABLE()[id - 1].sg_font_name;
	නම් (s != NULL)
	    ආපසු s;
    }
    ආපසු (char_u *)"";
}

/*
 * Set font for "Normal" group.  Called by gui_mch_init_font() when a font has
 * actually chosen to be used.
 */
    void
hl_set_font_name(char_u *font_name)
{
    පූර්ණ_සංඛ්‍යා	    id;

    id = syn_name2id((char_u *)"Normal");
    නම් (id <= 0)
	ආපසු;

    vim_free(HL_TABLE()[id - 1].sg_font_name);
    HL_TABLE()[id - 1].sg_font_name = vim_strsave(font_name);
}

/*
 * Set background color for "Normal" group.  Called by gui_set_bg_color()
 * when the color is known.
 */
    void
hl_set_bg_color_name(
    char_u  *name)	    // must have been allocated
{
    පූර්ණ_සංඛ්‍යා	    id;

    නම් (name == NULL)
	ආපසු;

    id = syn_name2id((char_u *)"Normal");
    නම් (id <= 0)
	ආපසු;

    vim_free(HL_TABLE()[id - 1].sg_gui_bg_name);
    HL_TABLE()[id - 1].sg_gui_bg_name = name;
}

/*
 * Set foreground color for "Normal" group.  Called by gui_set_fg_color()
 * when the color is known.
 */
    void
hl_set_fg_color_name(
    char_u  *name)	    // must have been allocated
{
    පූර්ණ_සංඛ්‍යා	    id;

    නම් (name == NULL)
	ආපසු;

    id = syn_name2id((char_u *)"Normal");
    නම් (id <= 0)
	ආපසු;

    vim_free(HL_TABLE()[id - 1].sg_gui_fg_name);
    HL_TABLE()[id - 1].sg_gui_fg_name = name;
}

/*
 * Return the handle for a font name.
 * Returns NOFONT when failed.
 */
    static GuiFont
font_name2handle(char_u *name)
{
    නම් (STRCMP(name, "NONE") == 0)
	ආපසු NOFONT;

    ආපසු gui_mch_get_font(name, TRUE);
}

# ifdef FEAT_XFONTSET
/*
 * Return the handle for a fontset name.
 * Returns NOFONTSET when failed.
 */
    static GuiFontset
fontset_name2handle(char_u *name, පූර්ණ_සංඛ්‍යා fixed_width)
{
    නම් (STRCMP(name, "NONE") == 0)
	ආපසු NOFONTSET;

    ආපසු gui_mch_get_fontset(name, TRUE, fixed_width);
}
# endif

/*
 * Get the font or fontset for one highlight group.
 */
    static void
hl_do_font(
    පූර්ණ_සංඛ්‍යා		idx,
    char_u	*arg,
    පූර්ණ_සංඛ්‍යා		do_normal,		// set normal font
    පූර්ණ_සංඛ්‍යා		do_menu UNUSED,		// set menu font
    පූර්ණ_සංඛ්‍යා		do_tooltip UNUSED,	// set tooltip font
    පූර්ණ_සංඛ්‍යා		free_font)		// free current font/fontset
{
# ifdef FEAT_XFONTSET
    // If 'guifontset' is not empty, first try using the name as a
    // fontset.  If that doesn't work, use it as a font name.
    නම් (*p_guifontset != NUL
#  ifdef FONTSET_ALWAYS
	|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
	// In Motif, the Tooltip highlight group is always a fontset
	|| do_tooltip
#  endif
	    )
    {
	නම් (free_font)
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
    නම් (HL_TABLE()[idx].sg_fontset != NOFONTSET)
    {
	// If it worked and it's the Normal group, use it as the normal
	// fontset.  Same for the Menu group.
	නම් (do_normal)
	    gui_init_font(arg, TRUE);
#  නම් defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	නම් (do_menu)
	{
#   ifdef FONTSET_ALWAYS
	    gui.menu_fontset = HL_TABLE()[idx].sg_fontset;
#   නැතහොත්
	    // YIKES!  This is a bug waiting to crash the program
	    gui.menu_font = HL_TABLE()[idx].sg_fontset;
#   endif
	    gui_mch_new_menu_font();
	}
#   ifdef FEAT_BEVAL_GUI
	නම් (do_tooltip)
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
    නැතහොත්
# endif
    {
	නම් (free_font)
	    gui_mch_free_font(HL_TABLE()[idx].sg_font);
	HL_TABLE()[idx].sg_font = font_name2handle(arg);
	// If it worked and it's the Normal group, use it as the
	// normal font.  Same for the Menu group.
	නම් (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    නම් (do_normal)
		gui_init_font(arg, FALSE);
# ifndef FONTSET_ALWAYS
#  නම් defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	    නම් (do_menu)
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

#නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return the handle for a color name.
 * Returns INVALCOLOR when failed.
 */
    guicolor_T
color_name2handle(char_u *name)
{
    නම් (STRCMP(name, "NONE") == 0)
	ආපසු INVALCOLOR;

    නම් (STRICMP(name, "fg") == 0 || STRICMP(name, "foreground") == 0)
    {
# නම් defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	නම් (gui.in_use)
# endif
# ifdef FEAT_GUI
	    ආපසු gui.norm_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	නම් (cterm_normal_fg_gui_color != INVALCOLOR)
	    ආපසු cterm_normal_fg_gui_color;
	// Guess that the foreground is black or white.
	ආපසු GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "black" : "white"));
# endif
    }
    නම් (STRICMP(name, "bg") == 0 || STRICMP(name, "background") == 0)
    {
# නම් defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	නම් (gui.in_use)
# endif
# ifdef FEAT_GUI
	    ආපසු gui.back_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	නම් (cterm_normal_bg_gui_color != INVALCOLOR)
	    ආපසු cterm_normal_bg_gui_color;
	// Guess that the background is white or black.
	ආපසු GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "white" : "black"));
# endif
    }

    ආපසු GUI_GET_COLOR(name);
}

// On MS-Windows an RGB macro is available and it produces 0x00bbggrr color
// values as used by the MS-Windows GDI api.  It should be used only for
// MS-Windows GDI builds.
# නම් defined(RGB) && defined(MSWIN) && !defined(FEAT_GUI)
#  undef RGB
# endif
# ifndef RGB
#  define RGB(r, g, b)	(((r)<<16) | ((g)<<8) | (b))
# endif

# ifdef VIMDLL
    static guicolor_T
gui_adjust_rgb(guicolor_T c)
{
    නම් (gui.in_use)
	ආපසු c;
    නැතහොත්
	ආපසු ((c & 0xff) << 16) | (c & 0x00ff00) | ((c >> 16) & 0xff);
}
# නැතහොත්
#  define gui_adjust_rgb(c) (c)
# endif

    static පූර්ණ_සංඛ්‍යා
hex_digit(පූර්ණ_සංඛ්‍යා c)
{
    නම් (SAFE_isdigit(c))
	ආපසු c - '0';
    c = TOLOWER_ASC(c);
    නම් (c >= 'a' && c <= 'f')
	ආපසු c - 'a' + 10;
    ආපසු 0x1ffffff;
}

    static guicolor_T
decode_hex_color(char_u *hex)
{
    guicolor_T color;

    නම් (hex[0] != '#' || STRLEN(hex) != 7)
	ආපසු INVALCOLOR;

    // Name is in "#rrggbb" format
    color = RGB(((hex_digit(hex[1]) << 4) + hex_digit(hex[2])),
		((hex_digit(hex[3]) << 4) + hex_digit(hex[4])),
		((hex_digit(hex[5]) << 4) + hex_digit(hex[6])));
    නම් (color > 0xffffff)
	ආපසු INVALCOLOR;
    ආපසු gui_adjust_rgb(color);
}

# ifdef FEAT_EVAL
// Returns the color currently mapped to the given name or INVALCOLOR if no
// such name exists in the color table. The convention is to use lowercase for
// all keys in the v:colornames dictionary. The value can be either a string in
// the form #rrggbb or a number, either of which is converted to a guicolor_T.
    static guicolor_T
colorname2rgb(char_u *name)
{
    dict_T      *colornames_table = get_vim_var_dict(VV_COLORNAMES);
    char_u      *lc_name;
    dictitem_T  *colentry;
    char_u      *colstr;
    varnumber_T colnum;

    lc_name = strlow_save(name);
    නම් (lc_name == NULL)
	ආපසු INVALCOLOR;

    colentry = dict_find(colornames_table, lc_name, -1);
    vim_free(lc_name);
    නම් (colentry == NULL)
	ආපසු INVALCOLOR;

    නම් (colentry->di_tv.v_type == VAR_STRING)
    {
	colstr = tv_get_string_strict(&colentry->di_tv);
	නම් ((STRLEN(colstr) == 7) && (*colstr == '#'))
	{
	    ආපසු decode_hex_color(colstr);
	}
	නැතහොත්
	{
	    semsg(_(e_bad_color_string_str), colstr);
	    ආපසු INVALCOLOR;
	}
    }

    නම් (colentry->di_tv.v_type == VAR_NUMBER)
    {
	colnum = tv_get_number(&colentry->di_tv);
	ආපසු (guicolor_T)colnum;
    }

    ආපසු INVALCOLOR;
}

# endif

    guicolor_T
gui_get_color_cmn(char_u *name)
{
    guicolor_T  color;
    // Only non X11 colors (not present in rgb.txt) and colors in
    // color_name_tab[], useful when $VIMRUNTIME is not found,.
    // must be sorted by the 'value' field because it is used by bsearch()!
    static keyvalue_T rgb_tab[] = {
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
    නම් (color != INVALCOLOR)
	ආපසු color;

    target.key = 0;
    target.value.string = name;
    target.value.length = 0;	// not used, see cmp_keyvalue_value_i()
    entry = (keyvalue_T *)bsearch(&target, &rgb_tab, ARRAY_LENGTH(rgb_tab),
	    parimaa_na(rgb_tab[0]), cmp_keyvalue_value_i);
    නම් (entry != NULL)
	ආපසු gui_adjust_rgb((guicolor_T)entry->key);

# නම් defined(FEAT_EVAL)
    /*
     * Not a traditional color. Load additional color aliases and then consult the alias table.
     */

    color = colorname2rgb(name);
    නම් (color == INVALCOLOR)
    {
	load_default_colors_lists();
	color = colorname2rgb(name);
    }

    ආපසු color;
# නැතහොත්
    ආපසු INVALCOLOR;
# endif
}

    guicolor_T
gui_get_rgb_color_cmn(පූර්ණ_සංඛ්‍යා r, පූර්ණ_සංඛ්‍යා g, පූර්ණ_සංඛ්‍යා b)
{
    guicolor_T  color = RGB(r, g, b);

    නම් (color > 0xffffff)
	ආපසු INVALCOLOR;
    ආපසු gui_adjust_rgb(color);
}
#endif

/*
 * Table with the specifications for an attribute number.
 * Note that this table is used by ALL buffers.  This is required because the
 * GUI can redraw at any time for any buffer.
 */
static garray_T	term_attr_table = {0, 0, 0, 0, NULL};

#define TERM_ATTR_ENTRY(idx) ((attrentry_T *)term_attr_table.ga_data)[idx]

static garray_T	cterm_attr_table = {0, 0, 0, 0, NULL};

#define CTERM_ATTR_ENTRY(idx) ((attrentry_T *)cterm_attr_table.ga_data)[idx]

#ifdef FEAT_GUI
static garray_T	gui_attr_table = {0, 0, 0, 0, NULL};

# define GUI_ATTR_ENTRY(idx) ((attrentry_T *)gui_attr_table.ga_data)[idx]
#endif

/*
 * Return the attr number for a set of colors and font.
 * Add a new entry to the term_attr_table, cterm_attr_table or gui_attr_table
 * if the combination is new.
 * Return 0 for error (no more room).
 */
    static පූර්ණ_සංඛ්‍යා
get_attr_entry(garray_T *table, attrentry_T *aep)
{
    පූර්ණ_සංඛ්‍යා		i;
    attrentry_T	*taep;
    static පූර්ණ_සංඛ්‍යා	recursive = FALSE;

    // Init the table, in case it wasn't done yet.
    table->ga_itemsize = parimaa_na(attrentry_T);
    table->ga_growsize = 7;

    // Try to find an entry with the same specifications.
    සඳහා (i = 0; i < table->ga_len; ++i)
    {
	taep = &(((attrentry_T *)table->ga_data)[i]);
	නම් (	   aep->ae_attr == taep->ae_attr
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
			&& (aep->ae_u.term.start == NULL)
					    == (taep->ae_u.term.start == NULL)
			&& (aep->ae_u.term.start == NULL
			    || STRCMP(aep->ae_u.term.start,
						  taep->ae_u.term.start) == 0)
			&& (aep->ae_u.term.stop == NULL)
					     == (taep->ae_u.term.stop == NULL)
			&& (aep->ae_u.term.stop == NULL
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

	ආපසු i + ATTR_OFF;
    }

    නම් (table->ga_len + ATTR_OFF > MAX_TYPENR)
    {
	// Running out of attribute entries!  remove all attributes, and
	// compute new ones for all groups.
	// When called recursively, we are really out of numbers.
	නම් (recursive)
	{
	    emsg(_(e_too_many_different_highlighting_attributes_in_use));
	    ආපසු 0;
	}
	recursive = TRUE;

	clear_hl_tables();

	set_must_redraw(UPD_CLEAR);

	සඳහා (i = 0; i < highlight_ga.ga_len; ++i)
	    set_hl_attr(i);

	recursive = FALSE;
    }

    // This is a new combination of colors and font, add an entry.
    නම් (ga_grow(table, 1) == FAIL)
	ආපසු 0;

    taep = &(((attrentry_T *)table->ga_data)[table->ga_len]);
    CLEAR_POINTER(taep);
    taep->ae_attr = aep->ae_attr;
#ifdef FEAT_GUI
    නම් (table == &gui_attr_table)
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
    නම් (table == &term_attr_table)
    {
	නම් (aep->ae_u.term.start == NULL)
	    taep->ae_u.term.start = NULL;
	නැතහොත්
	    taep->ae_u.term.start = vim_strsave(aep->ae_u.term.start);
	නම් (aep->ae_u.term.stop == NULL)
	    taep->ae_u.term.stop = NULL;
	නැතහොත්
	    taep->ae_u.term.stop = vim_strsave(aep->ae_u.term.stop);
    }
    නැතහොත් නම් (table == &cterm_attr_table)
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
    ආපසු (table->ga_len - 1 + ATTR_OFF);
}

#නම් defined(FEAT_TERMINAL)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    පූර්ණ_සංඛ්‍යා
get_cterm_attr_idx(පූර්ණ_සංඛ්‍යා attr, පූර්ණ_සංඛ්‍යා fg, පූර්ණ_සංඛ්‍යා bg)
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
    ආපසු get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#නම් defined(FEAT_TERMINAL) && defined(FEAT_TERMGUICOLORS)
/*
 * Get an attribute index for a 'termguicolors' entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    පූර්ණ_සංඛ්‍යා
get_tgc_attr_idx(පූර්ණ_සංඛ්‍යා attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    නම් (fg == INVALCOLOR && bg == INVALCOLOR)
    {
	// If both GUI colors are not set fall back to the cterm colors.  Helps
	// if the GUI only has an attribute, such as undercurl.
	at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
    }
    නැතහොත්
    {
	at_en.ae_u.cterm.fg_rgb = fg;
	at_en.ae_u.cterm.bg_rgb = bg;
    }
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
    ආපසු get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#නම් defined(FEAT_TERMINAL) && defined(FEAT_GUI)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    පූර්ණ_සංඛ්‍යා
get_gui_attr_idx(පූර්ණ_සංඛ්‍යා attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    at_en.ae_u.gui.fg_color = fg;
    at_en.ae_u.gui.bg_color = bg;
    ආපසු get_attr_entry(&gui_attr_table, &at_en);
}
#endif

/*
 * Clear all highlight tables.
 */
    void
clear_hl_tables(void)
{
    පූර්ණ_සංඛ්‍යා		i;
    attrentry_T	*taep;

#ifdef FEAT_GUI
    ga_clear(&gui_attr_table);
#endif
    සඳහා (i = 0; i < term_attr_table.ga_len; ++i)
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
    පූර්ණ_සංඛ්‍යා
hl_combine_attr(පූර්ණ_සංඛ්‍යා char_attr, පූර්ණ_සංඛ්‍යා prim_attr)
{
    attrentry_T *char_aep = NULL;
    attrentry_T *prim_aep;
    attrentry_T new_en;

    නම් (char_attr == 0)
	ආපසු prim_attr;
    නම් (char_attr <= HL_ALL && prim_attr <= HL_ALL)
	ආපසු ATTR_COMBINE(char_attr, prim_attr);
#ifdef FEAT_GUI
    නම් (gui.in_use)
    {
	නම් (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	නම් (char_aep != NULL)
	    new_en = *char_aep;
	නැතහොත්
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    නම් (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	නම් (prim_attr <= HL_ALL)
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	නැතහොත්
	{
	    prim_aep = syn_gui_attr2entry(prim_attr);
	    නම් (prim_aep != NULL)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		නම් (prim_aep->ae_u.gui.fg_color != INVALCOLOR)
		    new_en.ae_u.gui.fg_color = prim_aep->ae_u.gui.fg_color;
		නම් (prim_aep->ae_u.gui.bg_color != INVALCOLOR)
		    new_en.ae_u.gui.bg_color = prim_aep->ae_u.gui.bg_color;
		නම් (prim_aep->ae_u.gui.sp_color != INVALCOLOR)
		    new_en.ae_u.gui.sp_color = prim_aep->ae_u.gui.sp_color;
		නම් (prim_aep->ae_u.gui.font != NOFONT)
		    new_en.ae_u.gui.font = prim_aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
		නම් (prim_aep->ae_u.gui.fontset != NOFONTSET)
		    new_en.ae_u.gui.fontset = prim_aep->ae_u.gui.fontset;
# endif
	    }
	}
	ආපසු get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    නම් (IS_CTERM)
    {
	නම් (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	නම් (char_aep != NULL)
	    new_en = *char_aep;
	නැතහොත්
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    නම් (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	නම් (prim_attr <= HL_ALL)
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	නැතහොත්
	{
	    prim_aep = syn_cterm_attr2entry(prim_attr);
	    නම් (prim_aep != NULL)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		නම් (prim_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color = prim_aep->ae_u.cterm.fg_color;
		නම් (prim_aep->ae_u.cterm.bg_color > 0)
		    new_en.ae_u.cterm.bg_color = prim_aep->ae_u.cterm.bg_color;
		නම් (prim_aep->ae_u.cterm.ul_color > 0)
		    new_en.ae_u.cterm.ul_color = prim_aep->ae_u.cterm.ul_color;
		නම් (prim_aep->ae_u.cterm.font > 0)
		    new_en.ae_u.cterm.font = prim_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		// If both fg and bg are not set fall back to cterm colors.
		// Helps for SpellBad which uses undercurl in the GUI.
		නම් (COLOR_INVALID(prim_aep->ae_u.cterm.fg_rgb)
			&& COLOR_INVALID(prim_aep->ae_u.cterm.bg_rgb))
		{
		    නම් (prim_aep->ae_u.cterm.fg_color > 0)
			new_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
		    නම් (prim_aep->ae_u.cterm.bg_color > 0)
			new_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
		}
		නැතහොත්
		{
		    නම් (prim_aep->ae_u.cterm.fg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.fg_rgb = prim_aep->ae_u.cterm.fg_rgb;
		    නම් (prim_aep->ae_u.cterm.bg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.bg_rgb = prim_aep->ae_u.cterm.bg_rgb;
		}
		නම් (prim_aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		    new_en.ae_u.cterm.ul_rgb = prim_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	}
	ආපසු get_attr_entry(&cterm_attr_table, &new_en);
    }

    නම් (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    නම් (char_aep != NULL)
	new_en = *char_aep;
    නැතහොත්
    {
	CLEAR_FIELD(new_en);
	නම් (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    නම් (prim_attr <= HL_ALL)
	new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
    නැතහොත්
    {
	prim_aep = syn_term_attr2entry(prim_attr);
	නම් (prim_aep != NULL)
	{
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_aep->ae_attr);
	    නම් (prim_aep->ae_u.term.start != NULL)
	    {
		new_en.ae_u.term.start = prim_aep->ae_u.term.start;
		new_en.ae_u.term.stop = prim_aep->ae_u.term.stop;
	    }
	}
    }
    ආපසු get_attr_entry(&term_attr_table, &new_en);
}

// ANSI color order: Black, Red, Green, Yellow, Blue, Magenta,
// Cyan, White, then bright variants.  Approximate RGB values used when
// only a cterm color number is known (no guifg/guibg).  Real terminal
// palettes may differ if the user reconfigured their emulator, but
// these are reasonable xterm-ish defaults.
static const long_u cterm_color_16[16] = {
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
    static guicolor_T
cterm_color_to_rgb(පූර්ණ_සංඛ්‍යා color_nr)
{
    නම් (color_nr < 1 || color_nr > 16)
	ආපසු INVALCOLOR;
    ආපසු (guicolor_T)cterm_color_16[color_nr - 1];
}
#endif

/*
 * Convert an xterm 256-color index (0-255) to an approximate RGB triple.
 * Uses the standard xterm palette: 0-15 ANSI (cterm_color_16), 16-231
 * 6x6x6 cube, 232-255 grayscale ramp.
 */
    static void
cterm_idx_to_rgb(පූර්ණ_සංඛ්‍යා idx, පූර්ණ_සංඛ්‍යා *r, පූර්ණ_සංඛ්‍යා *g, පූර්ණ_සංඛ්‍යා *b)
{
    static const පූර්ණ_සංඛ්‍යා cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

    නම් (idx < 0 || idx > 255)
    {
	*r = *g = *b = 0;
	ආපසු;
    }
    නම් (idx < 16)
    {
	long_u rgb = cterm_color_16[idx];
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
    }
    නැතහොත් නම් (idx < 232)
    {
	පූර්ණ_සංඛ්‍යා n = idx - 16;
	*r = cube[n / 36 % 6];
	*g = cube[n / 6  % 6];
	*b = cube[n      % 6];
    }
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා v = 8 + (idx - 232) * 10;
	*r = *g = *b = v;
    }
}

/*
 * Approximate an RGB triple to the nearest xterm 256-color index.
 * Searches the 6x6x6 cube and the grayscale ramp; returns 0-255.
 */
    static පූර්ණ_සංඛ්‍යා
rgb_to_cterm_idx(පූර්ණ_සංඛ්‍යා r, පූර්ණ_සංඛ්‍යා g, පූර්ණ_සංඛ්‍යා b)
{
    static const පූර්ණ_සංඛ්‍යා cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };
    පූර්ණ_සංඛ්‍යා best = 0;
    long best_d = -1;
    පූර්ණ_සංඛ්‍යා i;

    // Search the 6x6x6 cube.
    සඳහා (i = 16; i < 232; ++i)
    {
	පූර්ණ_සංඛ්‍යා n = i - 16;
	පූර්ණ_සංඛ්‍යා cr = cube[n / 36 % 6];
	පූර්ණ_සංඛ්‍යා cg = cube[n / 6  % 6];
	පූර්ණ_සංඛ්‍යා cb = cube[n      % 6];
	long d = (cr - r) * (cr - r) + (cg - g) * (cg - g) + (cb - b) * (cb - b);
	නම් (best_d < 0 || d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    // Search the grayscale ramp.
    සඳහා (i = 232; i < 256; ++i)
    {
	පූර්ණ_සංඛ්‍යා v = 8 + (i - 232) * 10;
	long d = (v - r) * (v - r) + (v - g) * (v - g) + (v - b) * (v - b);
	නම් (d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    ආපසු best;
}

/*
 * Resolve a color to an RGB triple.  Prefer the gui RGB value (set by
 * guifg/guibg) when valid, otherwise convert the cterm 256-color index.
 * Returns true on success; false when no color is defined.
 * cterm_c is 1-based (0 means "no color").
 */
    static bool
resolve_color_to_rgb(පූර්ණ_සංඛ්‍යා cterm_c, guicolor_T rgb UNUSED, පූර්ණ_සංඛ්‍යා *r, පූර්ණ_සංඛ්‍යා *g, පූර්ණ_සංඛ්‍යා *b)
{
#ifdef FEAT_TERMGUICOLORS
    නම් (!COLOR_INVALID(rgb))
    {
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
	ආපසු true;
    }
#endif
    නම් (cterm_c > 0)
    {
	cterm_idx_to_rgb(cterm_c - 1, r, g, b);
	ආපසු true;
    }
    ආපසු false;
}

/*
 * get a RGB fallback color from gui, cterm or default color
 */
    static guicolor_T
resolve_fallback_color(පූර්ණ_සංඛ්‍යා cterm_c, guicolor_T rgb, guicolor_T default_rgb)
{
    පූර්ණ_සංඛ්‍යා red, green, blue;
    නම් (!resolve_color_to_rgb(cterm_c, rgb, &red, &green, &blue))
	ආපසු default_rgb;
    නැතහොත්
	ආපසු (red << 16) | (green << 8) | blue;
}

/*
 * get a RGB fallback foreground color from guifg, ctermfg or deduced from background
 */
    static void
resolve_fallback_fg_to_rgb(void)
{
    guicolor_T fgcolor_or_gui_fgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    fgcolor_or_gui_fgcolor = cterm_normal_fg_gui_color;
#endif
#ifdef FEAT_GUI
    නම් (gui.in_use)
	fgcolor_or_gui_fgcolor = gui.norm_pixel;
#endif
    fallback_fg_rgb = resolve_fallback_color(cterm_normal_fg_color, fgcolor_or_gui_fgcolor, (*p_bg == 'l') ? 0x000000 : 0xFFFFFF);
}

/*
 * get a RGB fallback background color from guifg, ctermbg or deduced from background
 */
    static void
resolve_fallback_bg_to_rgb(void)
{
    guicolor_T bgcolor_or_gui_bgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    bgcolor_or_gui_bgcolor = cterm_normal_bg_gui_color;
#endif
#ifdef FEAT_GUI
    නම් (gui.in_use)
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
    static පූර්ණ_සංඛ්‍යා
blend_cterm_colors(පූර්ණ_සංඛ්‍යා popup_c, guicolor_T popup_rgb,
		   පූර්ණ_සංඛ්‍යා under_c, guicolor_T under_rgb,
		   පූර්ණ_සංඛ්‍යා default_rgb,
		   පූර්ණ_සංඛ්‍යා blend_val)
{
    පූර්ණ_සංඛ්‍යා pr, pg, pb, ur, ug, ub, r, g, b;

    නම් (t_colors < 256)
	ආපසු popup_c;
    නම් (!resolve_color_to_rgb(popup_c, popup_rgb, &pr, &pg, &pb))
	ආපසු under_c;
    නම් (blend_val <= 0)
	ආපසු rgb_to_cterm_idx(pr, pg, pb) + 1;
    නම් (!resolve_color_to_rgb(under_c, under_rgb, &ur, &ug, &ub))
    {
	ur = (default_rgb >> 16) & 0xFF;
	ug = (default_rgb >> 8) & 0xFF;
	ub = default_rgb & 0xFF;
    }
    නම් (blend_val >= 100)
	ආපසු rgb_to_cterm_idx(ur, ug, ub) + 1;
    r = pr + (ur - pr) * blend_val / 100;
    g = pg + (ug - pg) * blend_val / 100;
    b = pb + (ub - pb) * blend_val / 100;
    ආපසු rgb_to_cterm_idx(r, g, b) + 1;
}

#නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Blend two RGB colors based on blend value (0-100).
 * blend: 0=return first argument color, 100=return second argument color
 * If bg_color is INVALCOLOR, high blend means more visible (return INVALCOLOR).
 */
    static guicolor_T
blend_colors(guicolor_T popup_color, guicolor_T bg_color, පූර්ණ_සංඛ්‍යා blend_val)
{
    පූර්ණ_සංඛ්‍යා r1, g1, b1, r2, g2, b2, r, g, b;

    // CTERMCOLOR is a sentinel meaning "use the cterm color"; for blending
    // it has no real RGB so treat it like INVALCOLOR.
    නම් (COLOR_INVALID(popup_color))
	ආපසු INVALCOLOR;

    // Fully transparent: use underlying color as-is.
    නම් (blend_val >= 100)
	ආපසු bg_color;

    r1 = (popup_color >> 16) & 0xFF;
    g1 = (popup_color >> 8) & 0xFF;
    b1 = popup_color & 0xFF;

    නම් (COLOR_INVALID(bg_color))
	bg_color = fallback_bg_rgb;

    r2 = (bg_color >> 16) & 0xFF;
    g2 = (bg_color >> 8) & 0xFF;
    b2 = bg_color & 0xFF;

    r = r1 + (r2 - r1) * blend_val / 100;
    g = g1 + (g2 - g1) * blend_val / 100;
    b = b1 + (b2 - b1) * blend_val / 100;

    ආපසු (r << 16) | (g << 8) | b;
}
#endif

/*
 * Blend attributes for popup windows and menus with opacity.
 * Blends foreground and/or background colors based on blend value (0-100).
 * blend: 0 = opaque (use popup colors), 100 = transparent (use background colors)
 * blend_fg: TRUE to blend foreground color, FALSE to keep popup foreground
 */
    static පූර්ණ_සංඛ්‍යා
hl_blend_attr_common(
    පූර්ණ_සංඛ්‍යා		char_attr,
    පූර්ණ_සංඛ්‍යා		popup_attr,
    පූර්ණ_සංඛ්‍යා		blend,
    පූර්ණ_සංඛ්‍යා		blend_fg)
{
    attrentry_T *char_aep = NULL;
    attrentry_T *popup_aep;
    attrentry_T new_en;
    attrentry_T tmp_en;

#ifdef FEAT_GUI
    නම් (gui.in_use)
    {
	නම් (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	නම් (char_aep != NULL)
	    new_en = *char_aep;
	නැතහොත්
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    නම් (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	නම් (popup_attr <= HL_ALL)
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
	නැතහොත්
	    popup_aep = syn_gui_attr2entry(popup_attr);

	නම් (popup_aep != NULL)
	{
	    guicolor_T popup_bg_rgb = popup_aep->ae_u.gui.bg_color;
	    නම් (COLOR_INVALID(popup_bg_rgb))
		popup_bg_rgb = fallback_bg_rgb;

	    නම් (blend_fg)
	    {
		// blend_fg=TRUE: fade underlying text toward popup bg.
		{
		    පූර්ණ_සංඛ්‍යා base_fg = fallback_fg_rgb;
		    නම් (char_aep != NULL
			    && char_aep->ae_u.gui.fg_color != INVALCOLOR)
			base_fg = char_aep->ae_u.gui.fg_color;
		    new_en.ae_u.gui.fg_color = blend_colors(
			    popup_bg_rgb, base_fg, blend);
		}
	    }
	    නැතහොත්
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
		නම් (char_aep != NULL
			&& !COLOR_INVALID(char_aep->ae_u.gui.bg_color))
		    underlying_bg = char_aep->ae_u.gui.bg_color;
		new_en.ae_u.gui.bg_color = blend_colors(
			popup_bg_rgb,
			underlying_bg, blend);
	    }
	}
	ආපසු get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    නම් (IS_CTERM)
    {
	නම් (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	නම් (char_aep != NULL)
	    new_en = *char_aep;
	නැතහොත්
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    නම් (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	නම් (popup_attr <= HL_ALL)
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
	නැතහොත්
	    popup_aep = syn_cterm_attr2entry(popup_attr);

	නම් (popup_aep != NULL)
	{
	    guicolor_T popup_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
	    // Fall back to cterm color converted to RGB when gui color is not set.
	    popup_bg_rgb = popup_aep->ae_u.cterm.bg_rgb;
	    නම් (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color > 0)
		popup_bg_rgb = cterm_color_to_rgb(
			popup_aep->ae_u.cterm.bg_color);
#endif
	    // assign default color if guibg and ctermbg are not set for popup
	    නම් (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color == 0)
		popup_bg_rgb = fallback_bg_rgb;

	    නම් (!blend_fg)
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
		නම් (popup_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color =
			popup_aep->ae_u.cterm.fg_color;
		නැතහොත් නම් (cterm_normal_fg_color > 0)
		    new_en.ae_u.cterm.fg_color = cterm_normal_fg_color;
		නැතහොත්
		    // black-ish or white-ish
		    new_en.ae_u.cterm.fg_color = (*p_bg == 'l') ? 1 : 16;
		new_en.ae_u.cterm.ul_color = popup_aep->ae_u.cterm.ul_color;
		new_en.ae_u.cterm.font = popup_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		new_en.ae_u.cterm.ul_rgb = popup_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	    නැතහොත්
	    {
		// blend_fg=TRUE: fade underlying fg toward popup bg in
		// the 256-color palette.  Used when the popup is over a
		// cell rendered with cterm colors (no termguicolors RGB).
		පූර්ණ_සංඛ්‍යා under_fg = (char_aep != NULL)
		    ? char_aep->ae_u.cterm.fg_color : 0;
		guicolor_T under_fg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		නම් (char_aep != NULL)
		    under_fg_rgb = char_aep->ae_u.cterm.fg_rgb;
#endif
		new_en.ae_u.cterm.fg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_fg, under_fg_rgb, fallback_fg_rgb, blend);
	    }
	    // Approximate cterm bg by blending with the underlying bg
	    // in the 256-color palette and mapping to the nearest entry.
	    {
		පූර්ණ_සංඛ්‍යා under_bg = (char_aep != NULL)
		    ? char_aep->ae_u.cterm.bg_color : 0;
		guicolor_T under_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		නම් (char_aep != NULL)
		    under_bg_rgb = char_aep->ae_u.cterm.bg_rgb;
#endif
		new_en.ae_u.cterm.bg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_bg, under_bg_rgb, fallback_bg_rgb, blend);
	    }
#ifdef FEAT_TERMGUICOLORS
	    // Blend RGB colors for termguicolors mode.
	    {
		නම් (blend_fg)
		{
		    // blend_fg=TRUE: fade underlying text toward popup bg.
		    නම් (popup_bg_rgb != INVALCOLOR)
		    {
			පූර්ණ_සංඛ්‍යා base_fg = fallback_fg_rgb;
			// CTERMCOLOR is a sentinel meaning "use the cterm
			// color"; treat it as no underlying color so it is
			// not blended in as a real near-white pixel.
			නම් (char_aep != NULL
				&& !COLOR_INVALID(char_aep->ae_u.cterm.fg_rgb))
			    base_fg = char_aep->ae_u.cterm.fg_rgb;
			new_en.ae_u.cterm.fg_rgb = blend_colors(
				popup_bg_rgb, base_fg, blend);
		    }
		}
		නැතහොත්
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

		    නම් (COLOR_INVALID(popup_fg)
			    && popup_aep->ae_u.cterm.fg_color > 0)
			popup_fg = cterm_color_to_rgb(
				popup_aep->ae_u.cterm.fg_color);

		    නම් (!COLOR_INVALID(popup_fg))
			new_en.ae_u.cterm.fg_rgb = popup_fg;
		    නැතහොත් නම් (!COLOR_INVALID(cterm_normal_fg_gui_color))
			new_en.ae_u.cterm.fg_rgb = cterm_normal_fg_gui_color;
		    නැතහොත්
			new_en.ae_u.cterm.fg_rgb = fallback_fg_rgb;
		}
		නම් (popup_bg_rgb != INVALCOLOR)
		{
		    // Blend popup bg toward underlying bg
		    guicolor_T underlying_bg = fallback_bg_rgb;
		    නම් (char_aep != NULL
			    && !COLOR_INVALID(char_aep->ae_u.cterm.bg_rgb))
			underlying_bg = char_aep->ae_u.cterm.bg_rgb;
		    new_en.ae_u.cterm.bg_rgb = blend_colors(
			    popup_bg_rgb, underlying_bg, blend);
		}
	    }
#endif
	}
	ආපසු get_attr_entry(&cterm_attr_table, &new_en);
    }

    නම් (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    නම් (char_aep != NULL)
	new_en = *char_aep;
    නැතහොත්
    {
	CLEAR_FIELD(new_en);
	නම් (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    // For term mode, no separate background color handling.
    ආපසු get_attr_entry(&term_attr_table, &new_en);
}

/*
 * Blend attributes for popup windows with opacity.
 */
    පූර්ණ_සංඛ්‍යා
hl_blend_attr(පූර්ණ_සංඛ්‍යා char_attr, පූර්ණ_සංඛ්‍යා popup_attr, පූර්ණ_සංඛ්‍යා blend, පූර්ණ_සංඛ්‍යා blend_fg)
{
    නම් (blend >= 100 && blend_fg)
	ආපසු char_attr;

    ආපසු hl_blend_attr_common(char_attr, popup_attr, blend, blend_fg);
}

/*
 * Blend underlying foreground and background colors for pum space cells.
 */
    පූර්ණ_සංඛ්‍යා
hl_pum_blend_attr(පූර්ණ_සංඛ්‍යා char_attr, පූර්ණ_සංඛ්‍යා popup_attr, පූර්ණ_සංඛ්‍යා blend)
{
    ආපසු hl_blend_attr_common(char_attr, popup_attr, blend, TRUE);
}

#ifdef FEAT_GUI
    attrentry_T *
syn_gui_attr2entry(පූර්ණ_සංඛ්‍යා attr)
{
    attr -= ATTR_OFF;
    නම් (attr >= gui_attr_table.ga_len)	    // did ":syntax clear"
	ආපසු NULL;
    ආපසු &(GUI_ATTR_ENTRY(attr));
}
#endif

/*
 * Get the highlight attributes (HL_BOLD etc.) from an attribute nr.
 * Only to be used when "attr" > HL_ALL.
 */
    පූර්ණ_සංඛ්‍යා
syn_attr2attr(පූර්ණ_සංඛ්‍යා attr)
{
    attrentry_T	*aep;

#ifdef FEAT_GUI
    නම් (gui.in_use)
	aep = syn_gui_attr2entry(attr);
    නැතහොත්
#endif
	නම් (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	නැතහොත්
	    aep = syn_term_attr2entry(attr);

    නම් (aep == NULL)	    // highlighting not set
	ආපසු 0;
    ආපසු aep->ae_attr;
}


    attrentry_T *
syn_term_attr2entry(පූර්ණ_සංඛ්‍යා attr)
{
    attr -= ATTR_OFF;
    නම් (attr >= term_attr_table.ga_len)	    // did ":syntax clear"
	ආපසු NULL;
    ආපසු &(TERM_ATTR_ENTRY(attr));
}

    attrentry_T *
syn_cterm_attr2entry(පූර්ණ_සංඛ්‍යා attr)
{
    attr -= ATTR_OFF;
    නම් (attr >= cterm_attr_table.ga_len)	// did ":syntax clear"
	ආපසු NULL;
    ආපසු &(CTERM_ATTR_ENTRY(attr));
}

#define LIST_ATTR   1
#define LIST_STRING 2
#define LIST_INT    3

    static void
highlight_list_one(පූර්ණ_සංඛ්‍යා id)
{
    hl_group_T	    *sgp;
    පූර්ණ_සංඛ්‍යා		    didh = FALSE;

    sgp = &HL_TABLE()[id - 1];	    // index is ID minus one

    නම් (message_filtered(sgp->sg_name))
	ආපසු;

    // Note: Keep this in sync with expand_highlight_group().
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_term, NULL, "term");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_start, "start");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_stop, "stop");

    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_cterm, NULL, "cterm");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_fg, NULL, "ctermfg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_bg, NULL, "ctermbg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_ul, NULL, "ctermul");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_font, NULL, "ctermfont");

#නම් defined(FEAT_GUI) || defined(FEAT_EVAL)
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_gui, NULL, "gui");
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

    නම් (sgp->sg_link && !got_int)
    {
	(void)syn_list_header(didh, 9999, id);
	didh = TRUE;
	msg_puts_attr("links to", HL_ATTR(HLF_D));
	msg_putchar(' ');
	msg_outtrans(HL_TABLE()[HL_TABLE()[id - 1].sg_link - 1].sg_name);
    }

    නම් (!didh)
	highlight_list_arg(id, didh, LIST_STRING, 0, (char_u *)"cleared", "");
#ifdef FEAT_EVAL
    නම් (p_verbose > 0)
	last_set_msg(sgp->sg_script_ctx);
#endif
}

    static char_u*
highlight_arg_to_string(
    පූර්ණ_සංඛ්‍යා		type,
    පූර්ණ_සංඛ්‍යා		iarg,
    char_u	*sarg,
    char_u	*buf)
{
    නම් (type == LIST_INT)
	sprintf((var_na *)buf, "%d", iarg - 1);
    නැතහොත් නම් (type == LIST_STRING)
	ආපසු sarg;
    නැතහොත් // type == LIST_ATTR
    {
	size_t buflen;

	buf[0] = NUL;
	buflen = 0;
	සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(highlight_index_tab); ++i)
	{
	    නම් (iarg & highlight_index_tab[i]->key)
	    {
		නම් (buflen > 0)
		{
		    STRCPY(buf + buflen, (char_u *)",");
		    ++buflen;
		}
		STRCPY(buf + buflen, highlight_index_tab[i]->value.string);
		buflen += highlight_index_tab[i]->value.length;
		iarg &= ~highlight_index_tab[i]->key;	    // don't want "inverse"/"reverse"
	    }
	}
    }
    ආපසු buf;
}

    static පූර්ණ_සංඛ්‍යා
highlight_list_arg(
    පූර්ණ_සංඛ්‍යා		id,
    පූර්ණ_සංඛ්‍යා		didh,
    පූර්ණ_සංඛ්‍යා		type,
    පූර්ණ_සංඛ්‍යා		iarg,
    char_u	*sarg,
    var_na	*name)
{
    char_u	buf[MAX_ATTR_LEN];
    char_u	*ts;

    නම් (got_int)
	ආපසු FALSE;

    නම් (type == LIST_STRING ? (sarg == NULL) : (iarg == 0))
	ආපසු didh;

    ts = highlight_arg_to_string(type, iarg, sarg, buf);

    (void)syn_list_header(didh,
	    (පූර්ණ_සංඛ්‍යා)(vim_strsize(ts) + STRLEN(name) + 1), id);
    didh = TRUE;
    නම් (!got_int)
    {
	නම් (*name != NUL)
	{
	    msg_puts_attr(name, HL_ATTR(HLF_D));
	    msg_puts_attr("=", HL_ATTR(HLF_D));
	}
	msg_outtrans(ts);
    }
    ආපසු didh;
}

#නම් ((defined(FEAT_EVAL) || defined(FEAT_PRINTER))) && defined(FEAT_SYN_HL)
/*
 * Return "1" if highlight group "id" has attribute "flag".
 * Return NULL otherwise.
 */
    char_u *
highlight_has_attr(
    පූර්ණ_සංඛ්‍යා		id,
    පූර්ණ_සංඛ්‍යා		flag,
    පූර්ණ_සංඛ්‍යා		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    පූර්ණ_සංඛ්‍යා		attr;

    නම් (id <= 0 || id > highlight_ga.ga_len)
	ආපසු NULL;

# නම් defined(FEAT_GUI) || defined(FEAT_EVAL)
    නම් (modec == 'g')
	attr = HL_TABLE()[id - 1].sg_gui;
    නැතහොත්
# endif
    {
	නම් (modec == 'c')
	    attr = HL_TABLE()[id - 1].sg_cterm;
	නැතහොත්
	    attr = HL_TABLE()[id - 1].sg_term;
    }

    නම් (attr & flag)
	ආපසු (char_u *)"1";
    ආපසු NULL;
}
#endif

#නම් defined(FEAT_SYN_HL) && defined(FEAT_EVAL)
/*
 * Return color name of highlight group "id".
 */
    char_u *
highlight_color(
    පූර්ණ_සංඛ්‍යා		id,
    char_u	*what,	// "font", "fg", "bg", "sp", "ul", "fg#", "bg#" or "sp#"
    පූර්ණ_සංඛ්‍යා		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    static char_u	name[20];
    පූර්ණ_සංඛ්‍යා			n;
    පූර්ණ_සංඛ්‍යා			fg = FALSE;
    පූර්ණ_සංඛ්‍යා			sp = FALSE;
    පූර්ණ_සංඛ්‍යා			ul = FALSE;
    පූර්ණ_සංඛ්‍යා			font = FALSE;

    නම් (id <= 0 || id > highlight_ga.ga_len)
	ආපසු NULL;

    නම් (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'g')
	fg = TRUE;
    නැතහොත් නම් (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'o'
	     && TOLOWER_ASC(what[2]) == 'n' && TOLOWER_ASC(what[3]) == 't')
	font = TRUE;
    නැතහොත් නම් (TOLOWER_ASC(what[0]) == 's' && TOLOWER_ASC(what[1]) == 'p')
	sp = TRUE;
    නැතහොත් නම් (TOLOWER_ASC(what[0]) == 'u' && TOLOWER_ASC(what[1]) == 'l')
	ul = TRUE;
    නැතහොත් නම් (!(TOLOWER_ASC(what[0]) == 'b' && TOLOWER_ASC(what[1]) == 'g'))
	ආපසු NULL;
    නම් (modec == 'g')
    {
# නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI
	// return font name
	නම් (font)
	    ආපසු HL_TABLE()[id - 1].sg_font_name;
#  endif

	// return #RRGGBB form (only possible when GUI is running)
	නම් ((USE_24BIT) && what[2] == '#')
	{
	    guicolor_T		color;
	    long_u		rgb;
	    static char_u	buf[10];

	    නම් (fg)
		color = HL_TABLE()[id - 1].sg_gui_fg;
	    නැතහොත් නම් (sp)
		color = HL_TABLE()[id - 1].sg_gui_sp;
	    නැතහොත්
		color = HL_TABLE()[id - 1].sg_gui_bg;
	    නම් (color == INVALCOLOR)
		ආපසු NULL;
	    rgb = (long_u)GUI_MCH_GET_RGB(color);
	    sprintf((var_na *)buf, "#%02x%02x%02x",
				      (unsigned)(rgb >> 16),
				      (unsigned)(rgb >> 8) & 255,
				      (unsigned)rgb & 255);
	    ආපසු buf;
	}
# endif
	නම් (fg)
	    ආපසු (HL_TABLE()[id - 1].sg_gui_fg_name);
	නම් (sp)
	    ආපසු (HL_TABLE()[id - 1].sg_gui_sp_name);
	ආපසු (HL_TABLE()[id - 1].sg_gui_bg_name);
    }
    නම් (sp)
	ආපසු NULL;
    නම් (modec == 'c')
    {
	නම් (fg)
	    n = HL_TABLE()[id - 1].sg_cterm_fg - 1;
	නැතහොත් නම් (ul)
	    n = HL_TABLE()[id - 1].sg_cterm_ul - 1;
	නැතහොත් නම් (font)
	    n = HL_TABLE()[id - 1].sg_cterm_font - 1;
	නැතහොත්
	    n = HL_TABLE()[id - 1].sg_cterm_bg - 1;
	නම් (n < 0)
	    ආපසු NULL;
	sprintf((var_na *)name, "%d", n);
	ආපසු name;
    }
    // term doesn't have color
    ආපසු NULL;
}
#endif

#නම් defined(FEAT_SYN_HL) \
	    && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)) \
	&& defined(FEAT_PRINTER)
/*
 * Return color name of highlight group "id" as RGB value.
 */
    long_u
highlight_gui_color_rgb(
    පූර්ණ_සංඛ්‍යා		id,
    පූර්ණ_සංඛ්‍යා		fg)	// TRUE = fg, FALSE = bg
{
    guicolor_T	color;

    නම් (id <= 0 || id > highlight_ga.ga_len)
	ආපසු 0L;

    නම් (fg)
	color = HL_TABLE()[id - 1].sg_gui_fg;
    නැතහොත්
	color = HL_TABLE()[id - 1].sg_gui_bg;

    නම් (color == INVALCOLOR)
	ආපසු 0L;

    ආපසු GUI_MCH_GET_RGB(color);
}
#endif

/*
 * Output the syntax list header.
 * Return TRUE when started a new line.
 */
    පූර්ණ_සංඛ්‍යා
syn_list_header(
    පූර්ණ_සංඛ්‍යා	    did_header,		// did header already
    පූර්ණ_සංඛ්‍යා	    outlen,		// length of string that comes
    පූර්ණ_සංඛ්‍යා	    id)			// highlight group id
{
    පූර්ණ_සංඛ්‍යා	    endcol = 19;
    පූර්ණ_සංඛ්‍යා	    newline = TRUE;
    පූර්ණ_සංඛ්‍යා	    name_col = 0;

    නම් (!did_header)
    {
	msg_putchar('\n');
	නම් (got_int)
	    ආපසු TRUE;
	msg_outtrans(HL_TABLE()[id - 1].sg_name);
	name_col = msg_col;
	endcol = 15;
    }
    නැතහොත් නම් (msg_col + outlen + 1 >= cmdline_width)
    {
	msg_putchar('\n');
	නම් (got_int)
	    ආපසු TRUE;
    }
    නැතහොත්
    {
	නම් (msg_col >= endcol)	// wrap around is like starting a new line
	    newline = FALSE;
    }

    නම් (msg_col >= endcol)	// output at least one space
	endcol = msg_col + 1;
    නම් (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    msg_advance(endcol);

    // Show "xxx" with the attributes.
    නම් (!did_header)
    {
	නම් (endcol == cmdline_width - 1 && endcol <= name_col)
	    msg_putchar(' ');
	msg_puts_attr("xxx", syn_id2attr(id));
	msg_putchar(' ');
    }

    ආපසු newline;
}

/*
 * Set the attribute numbers for a highlight group.
 * Called after one of the attributes has changed.
 */
    static void
set_hl_attr(
    පූර්ණ_සංඛ්‍යා		idx)	    // index in array
{
    attrentry_T	    at_en;
    hl_group_T	    *sgp = HL_TABLE() + idx;

    // The "Normal" group doesn't need an attribute number
    නම් (sgp->sg_name_u != NULL && STRCMP(sgp->sg_name_u, "NORMAL") == 0)
	ආපසු;

#ifdef FEAT_GUI
    // For the GUI mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    නම් (sgp->sg_gui_fg == INVALCOLOR
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
    නැතහොත්
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
    නම් (sgp->sg_start == NULL && sgp->sg_stop == NULL)
	sgp->sg_term_attr = sgp->sg_term;
    නැතහොත්
    {
	at_en.ae_attr = sgp->sg_term;
	at_en.ae_u.term.start = sgp->sg_start;
	at_en.ae_u.term.stop = sgp->sg_stop;
	sgp->sg_term_attr = get_attr_entry(&term_attr_table, &at_en);
    }

    // For the color term mode: If there are other than "normal"
    // highlighting attributes, need to allocate an attr number.
    නම් (sgp->sg_cterm_fg == 0 && sgp->sg_cterm_bg == 0 &&
	sgp->sg_cterm_ul == 0 && sgp->sg_cterm_font == 0
#ifdef FEAT_TERMGUICOLORS
	    && sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
#endif
	    )
	sgp->sg_cterm_attr = sgp->sg_cterm;
    නැතහොත්
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
	නම් (sgp->sg_cterm & (HL_UNDERLINE | HL_UNDERCURL
			   | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED))
	    at_en.ae_u.cterm.ul_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_sp);
	නැතහොත්
	    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	නම් (at_en.ae_u.cterm.fg_rgb == INVALCOLOR
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
    static පූර්ණ_සංඛ්‍යා
syn_override(පූර්ණ_සංඛ්‍යා id)
{
    නම් (overrides != NULL && overrides->arr != NULL)
	සඳහා (පූර්ණ_සංඛ්‍යා k = 0; k < overrides->len; k++)
	    නම් (overrides->arr[k].from == id)
		ආපසු overrides->arr[k].to;
    ආපසු id;
}

/*
 * Lookup a highlight group name by pointer and length.
 * If it is not found, 0 is returned.
 */
    static පූර්ණ_සංඛ්‍යා
syn_name2id_len(char_u *name, පූර්ණ_සංඛ්‍යා len)
{
    char_u	name_u[MAX_SYN_NAME + 1];
    hashitem_T	*hi;

    නම් (len > MAX_SYN_NAME)
	len = MAX_SYN_NAME;
    mch_memmove(name_u, name, len);
    name_u[len] = NUL;
    vim_strup(name_u);
    නම් (!highlight_ht_inited)
	ආපසු 0;
    hi = hash_find(&highlight_ht, name_u);
    නම් (HASHITEM_EMPTY(hi))
	ආපසු 0;
    ආපසු HI2HLNAME(hi)->hn_id;
}

/*
 * Lookup a highlight group name and return its ID.
 * If it is not found, 0 is returned.
 */
    පූර්ණ_සංඛ්‍යා
syn_name2id(char_u *name)
{
    // Avoid using stricmp() too much, it's slow on some systems.
    ආපසු syn_name2id_len(name, (පූර්ණ_සංඛ්‍යා)STRLEN(name));
}

/*
 * Lookup a highlight group name and return its attributes.
 * Return zero if not found.
 */
    පූර්ණ_සංඛ්‍යා
syn_name2attr(char_u *name)
{
    පූර්ණ_සංඛ්‍යා id = syn_name2id(name);

    නම් (id != 0)
	ආපසු syn_id2attr(id);
    ආපසු 0;
}

#නම් defined(FEAT_EVAL)
/*
 * Return TRUE if highlight group "name" exists.
 */
    පූර්ණ_සංඛ්‍යා
highlight_exists(char_u *name)
{
    ආපසු (syn_name2id(name) > 0);
}

# නම් defined(FEAT_SEARCH_EXTRA)
/*
 * Return the name of highlight group "id".
 * When not a valid ID return an empty string.
 */
    char_u *
syn_id2name(පූර්ණ_සංඛ්‍යා id)
{
    නම් (id <= 0 || id > highlight_ga.ga_len)
	ආපසු (char_u *)"";
    ආපසු HL_TABLE()[id - 1].sg_name;
}
# endif
#endif

/*
 * Like syn_name2id(), but take a pointer + length argument.
 */
    පූර්ණ_සංඛ්‍යා
syn_namen2id(char_u *linep, පූර්ණ_සංඛ්‍යා len)
{
    ආපසු syn_name2id_len(linep, len);
}

/*
 * Find highlight group name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    පූර්ණ_සංඛ්‍යා
syn_check_group(char_u *pp, පූර්ණ_සංඛ්‍යා len)
{
    පූර්ණ_සංඛ්‍යා	    id;
    char_u  *name;

    නම් (len > MAX_SYN_NAME)
    {
	emsg(_(e_highlight_group_name_too_long));
	ආපසු 0;
    }
    id = syn_name2id_len(pp, len);
    නම් (id == 0)			// doesn't exist yet
    {
	name = vim_strnsave(pp, len);
	නම් (name == NULL)
	    ආපසු 0;
	id = syn_add_group(name);
    }
    ආපසු id;
}

/*
 * Add new highlight group and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    static පූර්ණ_සංඛ්‍යා
syn_add_group(char_u *name)
{
    char_u	*p;
    char_u	*name_up;

    // Check that the name is valid (ASCII letters, digits, underscores, dots, or hyphens).
    සඳහා (p = name; *p != NUL; ++p)
    {
	නම් (!vim_isprintc(*p))
	{
	    emsg(_(e_unprintable_character_in_group_name));
	    vim_free(name);
	    ආපසු 0;
	}
	නැතහොත් නම් (!ASCII_ISALNUM(*p) && *p != '_' && *p != '.' && *p != '-')
	{
	    // This is an error, but since there previously was no check only
	    // give a warning.
	    msg_source(HL_ATTR(HLF_W));
	    msg(_("W18: Invalid character in group name"));
	    bha_nga;
	}
    }

    // First call for this growarray: init growing array.
    නම් (highlight_ga.ga_data == NULL)
    {
	highlight_ga.ga_itemsize = parimaa_na(hl_group_T);
	highlight_ga.ga_growsize = 10;
	hash_init(&highlight_ht);
	highlight_ht_inited = true;
    }

    නම් (highlight_ga.ga_len >= MAX_HL_ID)
    {
	emsg(_(e_too_many_highlight_and_syntax_groups));
	vim_free(name);
	ආපසු 0;
    }

    // Make room for at least one other syntax_highlight entry.
    නම් (ga_grow(&highlight_ga, 1) == FAIL)
    {
	vim_free(name);
	ආපසු 0;
    }

    {
	hlname_T    *hn;
	පූර්ණ_සංඛ්‍යා	    len = (පූර්ණ_සංඛ්‍යා)STRLEN(name);

	hn = alloc(offsetof(hlname_T, hn_key) + len + 1);
	නම් (hn == NULL)
	{
	    vim_free(name);
	    ආපසු 0;
	}
	vim_strncpy(hn->hn_key, name, len);
	vim_strup(hn->hn_key);
	hn->hn_id = highlight_ga.ga_len + 1;	// ID is index plus one
	name_up = hn->hn_key;
    }

    CLEAR_POINTER(&(HL_TABLE()[highlight_ga.ga_len]));
    HL_TABLE()[highlight_ga.ga_len].sg_name = name;
    HL_TABLE()[highlight_ga.ga_len].sg_name_u = name_up;
#නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_TABLE()[highlight_ga.ga_len].sg_gui_bg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_fg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_sp = INVALCOLOR;
#endif
    hash_add(&highlight_ht, name_up, "highlight");
    ++highlight_ga.ga_len;

    ආපසු highlight_ga.ga_len;		    // ID is index plus one
}

/*
 * When, just after calling syn_add_group(), an error is discovered, this
 * function deletes the new name.
 */
    static void
syn_unadd_group(void)
{
    hashitem_T *hi;

    --highlight_ga.ga_len;
    hi = hash_find(&highlight_ht, HL_TABLE()[highlight_ga.ga_len].sg_name_u);
    නම් (!HASHITEM_EMPTY(hi))
	hash_remove(&highlight_ht, hi, "highlight");
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name);
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name_u - HLNAME_KEY_OFF);
}

/*
 * Translate a group ID to highlight attributes.
 * "hl_id" must be valid: > 0, caller must check.
 */
    පූර්ණ_සංඛ්‍යා
syn_id2attr(පූර්ණ_සංඛ්‍යා hl_id)
{
    පූර්ණ_සංඛ්‍යා		attr;
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    assert(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

#ifdef FEAT_GUI
    // Only use GUI attr when the GUI is being used.
    නම් (gui.in_use)
	attr = sgp->sg_gui_attr;
    නැතහොත්
#endif
	නම් (IS_CTERM)
	    attr = sgp->sg_cterm_attr;
	නැතහොත්
	    attr = sgp->sg_term_attr;

    ආපසු attr;
}

#නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Get the GUI colors and attributes for a group ID.
 * NOTE: the colors will be INVALCOLOR when not set, the color otherwise.
 */
    පූර්ණ_සංඛ්‍යා
syn_id2colors(පූර්ණ_සංඛ්‍යා hl_id, guicolor_T *fgp, guicolor_T *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    assert(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

    *fgp = sgp->sg_gui_fg;
    *bgp = sgp->sg_gui_bg;
    ආපසු sgp->sg_gui;
}
#endif

#නම් (defined(MSWIN) \
	    && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL)) \
	    && defined(FEAT_TERMGUICOLORS)) \
	|| defined(FEAT_TERMINAL)
    void
syn_id2cterm_bg(පූර්ණ_සංඛ්‍යා hl_id, පූර්ණ_සංඛ්‍යා *fgp, පූර්ණ_සංඛ්‍යා *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    assert(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
    *fgp = sgp->sg_cterm_fg - 1;
    *bgp = sgp->sg_cterm_bg - 1;
}
#endif

/*
 * Translate a group ID to the final group ID (following links).
 */
    පූර්ණ_සංඛ්‍යා
syn_get_final_id(පූර්ණ_සංඛ්‍යා hl_id)
{
    පූර්ණ_සංඛ්‍යා		count;
    hl_group_T	*sgp;

    නම් (hl_id > highlight_ga.ga_len || hl_id < 1)
	ආපසු 0;			// Can be called from eval!!

    // Follow links until there is no more.
    // Look out for loops!  Break after 100 links.
    සඳහා (count = 100; --count >= 0; )
    {
	පූර්ණ_සංඛ්‍යා tmp = hl_id;

	sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
	නම් (sgp->sg_link == 0 || sgp->sg_link > highlight_ga.ga_len)
	    bha_nga;

	// This is to handle highlight groups that are overridden but are in the
	// middle of a link chain.
	hl_id = syn_override(hl_id);
	නම් (tmp != hl_id)
	    continue;
	hl_id = sgp->sg_link;
    }
    ආපසු syn_override(hl_id);
}

#නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Call this function just after the GUI has started.
 * Also called when 'termguicolors' was set, gui.in_use will be FALSE then.
 * It finds the font and color handles for the highlighting groups.
 */
    void
highlight_gui_started(void)
{
    පූර්ණ_සංඛ්‍යා	    idx;

    // First get the colors from the "Normal" and "Menu" group, if set
    නම් (USE_24BIT)
	set_normal_colors();

    සඳහා (idx = 0; idx < highlight_ga.ga_len; ++idx)
	gui_do_one_color(idx, FALSE, FALSE);

    highlight_changed();
}

    static void
gui_do_one_color(
    පූර්ණ_සංඛ්‍යා		idx,
    පූර්ණ_සංඛ්‍යා		do_menu UNUSED,	   // TRUE: might set the menu font
    පූර්ණ_සංඛ්‍යා		do_tooltip UNUSED) // TRUE: might set the tooltip font
{
    පූර්ණ_සංඛ්‍යා		didit = FALSE;

# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    නම් (gui.in_use)
#  endif
	නම් (HL_TABLE()[idx].sg_font_name != NULL)
	{
	    hl_do_font(idx, HL_TABLE()[idx].sg_font_name, FALSE, do_menu,
							    do_tooltip, TRUE);
	    didit = TRUE;
	}
# endif
    නම් (HL_TABLE()[idx].sg_gui_fg_name != NULL)
    {
	HL_TABLE()[idx].sg_gui_fg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_fg_name);
	didit = TRUE;
    }
    නම් (HL_TABLE()[idx].sg_gui_bg_name != NULL)
    {
	HL_TABLE()[idx].sg_gui_bg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_bg_name);
	didit = TRUE;
    }
    නම් (HL_TABLE()[idx].sg_gui_sp_name != NULL)
    {
	HL_TABLE()[idx].sg_gui_sp =
			    color_name2handle(HL_TABLE()[idx].sg_gui_sp_name);
	didit = TRUE;
    }
    නම් (didit)	// need to get a new attr number
	set_hl_attr(idx);
}
#endif

#නම් defined(USER_HIGHLIGHT) && defined(FEAT_STL_OPT)
/*
 * Apply difference between User[1-9] and HLF_S to HLF_SNC, HLF_ST or HLF_STNC.
 */
    static void
combine_stl_hlt(
	පූර්ණ_සංඛ්‍යා id,
	පූර්ණ_සංඛ්‍යා id_S,
	පූර්ණ_සංඛ්‍යා id_alt,
	පූර්ණ_සංඛ්‍යා hlcnt,
	පූර්ණ_සංඛ්‍යා i,
	පූර්ණ_සංඛ්‍යා hlf,
	පූර්ණ_සංඛ්‍යා *table)
{
    hl_group_T *hlt = HL_TABLE();

    නම් (id_alt == 0)
    {
	CLEAR_POINTER(&hlt[hlcnt + i]);
	hlt[hlcnt + i].sg_term = highlight_attr[hlf];
	hlt[hlcnt + i].sg_cterm = highlight_attr[hlf];
# නම් defined(FEAT_GUI) || defined(FEAT_EVAL)
	hlt[hlcnt + i].sg_gui = highlight_attr[hlf];
# endif
    }
    නැතහොත්
	mch_memmove(&hlt[hlcnt + i],
		    &hlt[id_alt - 1],
		    parimaa_na(hl_group_T));
    hlt[hlcnt + i].sg_link = 0;

    hlt[hlcnt + i].sg_term ^=
	hlt[id - 1].sg_term ^ hlt[id_S - 1].sg_term;
    නම් (hlt[id - 1].sg_start != hlt[id_S - 1].sg_start)
	hlt[hlcnt + i].sg_start = hlt[id - 1].sg_start;
    නම් (hlt[id - 1].sg_stop != hlt[id_S - 1].sg_stop)
	hlt[hlcnt + i].sg_stop = hlt[id - 1].sg_stop;
    hlt[hlcnt + i].sg_cterm ^=
	hlt[id - 1].sg_cterm ^ hlt[id_S - 1].sg_cterm;
    නම් (hlt[id - 1].sg_cterm_fg != hlt[id_S - 1].sg_cterm_fg)
	hlt[hlcnt + i].sg_cterm_fg = hlt[id - 1].sg_cterm_fg;
    නම් (hlt[id - 1].sg_cterm_bg != hlt[id_S - 1].sg_cterm_bg)
	hlt[hlcnt + i].sg_cterm_bg = hlt[id - 1].sg_cterm_bg;
    නම් (hlt[id - 1].sg_cterm_font != hlt[id_S - 1].sg_cterm_font)
	hlt[hlcnt + i].sg_cterm_font = hlt[id - 1].sg_cterm_font;
# නම් defined(FEAT_GUI) || defined(FEAT_EVAL)
    hlt[hlcnt + i].sg_gui ^=
	hlt[id - 1].sg_gui ^ hlt[id_S - 1].sg_gui;
# endif
# ifdef FEAT_GUI
    නම් (hlt[id - 1].sg_gui_fg != hlt[id_S - 1].sg_gui_fg)
	hlt[hlcnt + i].sg_gui_fg = hlt[id - 1].sg_gui_fg;
    නම් (hlt[id - 1].sg_gui_bg != hlt[id_S - 1].sg_gui_bg)
	hlt[hlcnt + i].sg_gui_bg = hlt[id - 1].sg_gui_bg;
    නම් (hlt[id - 1].sg_gui_sp != hlt[id_S - 1].sg_gui_sp)
	hlt[hlcnt + i].sg_gui_sp = hlt[id - 1].sg_gui_sp;
    නම් (hlt[id - 1].sg_font != hlt[id_S - 1].sg_font)
	hlt[hlcnt + i].sg_font = hlt[id - 1].sg_font;
#  ifdef FEAT_XFONTSET
    නම් (hlt[id - 1].sg_fontset != hlt[id_S - 1].sg_fontset)
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
    පූර්ණ_සංඛ්‍යා
highlight_changed(void)
{
    පූර්ණ_සංඛ්‍යා		hlf;
    පූර්ණ_සංඛ්‍යා		i;
    char_u	*p;
    char_u	*default_hl;
    පූර්ණ_සංඛ්‍යා		attr;
    char_u	*end;
    පූර්ණ_සංඛ්‍යා		id;
#ifdef USER_HIGHLIGHT
    char_u      userhl[30];  // use 30 to avoid compiler warning
# ifdef FEAT_STL_OPT
    පූර්ණ_සංඛ්‍යා		id_S = -1;
    පූර්ණ_සංඛ්‍යා		id_SNC = 0;
#  ifdef FEAT_TERMINAL
    පූර්ණ_සංඛ්‍යා		id_ST = 0;
    පූර්ණ_සංඛ්‍යා		id_STNC = 0;
#  endif
    පූර්ණ_සංඛ්‍යා		hlcnt;
# endif
#endif
    win_T	*wp;

    need_highlight_changed = FALSE;

#ifdef FEAT_TERMINAL
    term_update_colors_all();
    term_update_hlfwin_all();
#endif

    // Clear all attributes.
    සඳහා (hlf = 0; hlf < (පූර්ණ_සංඛ්‍යා)HLF_COUNT; ++hlf)
    {
	highlight_attr[hlf] = 0;
	highlight_attr_raw[hlf] = 0;
	highlight_ids[hlf] = 0;
    }

    default_hl = get_highlight_default();

    // First set all attributes to their default value.
    // Then use the attributes from the 'highlight' option, unless it already
    // matches the default and would repeat the same work.
    සඳහා (i = 0; i < 2; ++i)
    {
	නම් (i)
	{
	    නම් (default_hl != NULL && p_hl != NULL
				      && STRCMP(default_hl, p_hl) == 0)
		continue;
	    p = p_hl;
	}
	නැතහොත්
	    p = default_hl;
	නම් (p == NULL)	    // just in case
	    continue;

	අතරතුර (*p)
	{
	    සඳහා (hlf = 0; hlf < (පූර්ණ_සංඛ්‍යා)HLF_COUNT; ++hlf)
		නම් (hl_flags[hlf] == *p)
		    bha_nga;
	    ++p;
	    නම් (hlf == (පූර්ණ_සංඛ්‍යා)HLF_COUNT || *p == NUL)
		ආපසු FAIL;

	    // Allow several hl_flags to be combined, like "bu" for
	    // bold-underlined.
	    attr = 0;
	    id = 0;
	    සඳහා ( ; *p && *p != ','; ++p)	    // parse up to comma
	    {
		නම් (VIM_ISWHITE(*p))		    // ignore white space
		    continue;

		නම් (attr > HL_ALL)  // Combination with ':' is not allowed.
		    ආපසු FAIL;

		// Note: Keep this in sync with expand_set_highlight().
		switch (*p)
		{
		    pak_sa 'b':	attr |= HL_BOLD;
				bha_nga;
		    pak_sa 'i':	attr |= HL_ITALIC;
				bha_nga;
		    pak_sa '-':
		    pak_sa 'n':			    // no highlighting
				bha_nga;
		    pak_sa 'r':	attr |= HL_INVERSE;
				bha_nga;
		    pak_sa 's':	attr |= HL_STANDOUT;
				bha_nga;
		    pak_sa 'u':	attr |= HL_UNDERLINE;
				bha_nga;
		    pak_sa 'c':	attr |= HL_UNDERCURL;
				bha_nga;
		    pak_sa '2':	attr |= HL_UNDERDOUBLE;
				bha_nga;
		    pak_sa 'd':	attr |= HL_UNDERDOTTED;
				bha_nga;
		    pak_sa '=':	attr |= HL_UNDERDASHED;
				bha_nga;
		    pak_sa 't':	attr |= HL_STRIKETHROUGH;
				bha_nga;
		    pak_sa ':':	++p;		    // highlight group name
				නම් (attr || *p == NUL)	 // no combinations
				    ආපසු FAIL;
				end = vim_strchr(p, ',');
				නම් (end == NULL)
				    end = p + STRLEN(p);
				id = syn_check_group(p, (පූර්ණ_සංඛ්‍යා)(end - p));
				නම් (id == 0)
				    ආපසු FAIL;
				attr = syn_id2attr(id);
				p = end - 1;
#නම් defined(FEAT_STL_OPT) && defined(USER_HIGHLIGHT)
				නම් (hlf == (පූර්ණ_සංඛ්‍යා)HLF_SNC)
				    id_SNC = syn_get_final_id(id);
# ifdef FEAT_TERMINAL
				නැතහොත් නම් (hlf == (පූර්ණ_සංඛ්‍යා)HLF_ST)
				    id_ST = syn_get_final_id(id);
				නැතහොත් නම් (hlf == (පූර්ණ_සංඛ්‍යා)HLF_STNC)
				    id_STNC = syn_get_final_id(id);
# endif
				නැතහොත් නම් (hlf == (පූර්ණ_සංඛ්‍යා)HLF_S)
				    id_S = syn_get_final_id(id);
#endif
				bha_nga;
		    default:	ආපසු FAIL;
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
	var_na *errmsg = update_winhighlight(wp, wp->w_p_whl);

	නම් (errmsg != NULL)
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
    නම් (ga_grow(&highlight_ga, 28) == FAIL)
	ආපසු FAIL;
    hlcnt = highlight_ga.ga_len;
    නම් (id_S == -1)
    {
	// Make sure id_S is always valid to simplify code below. Use the last
	// entry.
	CLEAR_POINTER(&HL_TABLE()[hlcnt + 27]);
	HL_TABLE()[hlcnt + 18].sg_term = highlight_attr[HLF_S];
	id_S = hlcnt + 19;
    }
# endif
    සඳහා (i = 0; i < 9; i++)
    {
	sprintf((var_na *)userhl, "User%d", i + 1);
	id = syn_name2id(userhl);
	නම් (id == 0)
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
	නැතහොත්
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

    ආපසු OK;
}

static void highlight_list(void);
static void highlight_list_two(පූර්ණ_සංඛ්‍යා cnt, පූර්ණ_සංඛ්‍යා attr);

// context for :highlight <group> <arg> expansion
static පූර්ණ_සංඛ්‍යා expand_hi_synid = 0;	    // ID for highlight group being completed
static පූර්ණ_සංඛ්‍යා expand_hi_equal_col = 0; // column where the '=' is
static පූර්ණ_සංඛ්‍යා expand_hi_include_orig = 0;	    // whether to fill the existing current value or not
static char_u *expand_hi_curvalue = NULL;   // the existing current value
#නම් defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
static dict_iterator_T expand_colornames_iter;	// iterator for looping through v:colornames
#endif

/*
 * Handle command line completion for :highlight command.
 */
    void
set_context_in_highlight_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		expand_group = TRUE;

    // Default: expand group names
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 0;
    include_link = 2;
    include_default = 1;

    නම් (*arg == NUL)
	ආපසු;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    නම් (*p == NUL)
	ආපසු;

    // past "default" or group name
    include_default = 0;
    නම් (STRNCMP("default", arg, p - arg) == 0)
    {
	arg = skipwhite(p);
	xp->xp_pattern = arg;
	p = skiptowhite(arg);
    }
    නම් (*p == NUL)
	ආපසු;

    // past group name
    include_link = 0;
    නම් (arg[1] == 'i' && arg[0] == 'N')
    {
	highlight_list();
	expand_group = FALSE;
    }
    නම් (STRNCMP("link", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	නම් (*p != NUL)		// past first group name
	{
	    xp->xp_pattern = skipwhite(p);
	    p = skiptowhite(xp->xp_pattern);
	    include_none = 1;
	}
	expand_group = FALSE;
    }
    නැතහොත් නම් (STRNCMP("clear", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	expand_group = FALSE;
    }
    නම් (*p != NUL)			// past group name(s)
    {
	නම් (expand_group)
	{
	    // expansion will be done in expand_highlight_group()
	    xp->xp_context = EXPAND_HIGHLIGHT_GROUP;

	    expand_hi_synid = syn_namen2id(arg, (පූර්ණ_සංඛ්‍යා)(p - arg));

	    අතරතුර (*p != NUL)
	    {
		arg = skipwhite(p);
		p = skiptowhite(arg);
	    }

	    p = vim_strchr(arg, '=');
	    නම් (p == NULL)
	    {
		// Didn't find a key=<value> pattern
		xp->xp_pattern = arg;
		expand_hi_equal_col = -1;
		expand_hi_include_orig = FALSE;
	    }
	    නැතහොත්
	    {
		// Found key=<value> pattern, record the exact location
		expand_hi_equal_col = (පූර්ණ_සංඛ්‍යා)(p - xp->xp_line);

		// Only include the original value if the pattern is empty
		නම් (*(p + 1) == NUL)
		    expand_hi_include_orig = TRUE;
		නැතහොත්
		    expand_hi_include_orig = FALSE;

		// Account for comma-separated values
		නම් (STRNCMP(arg, "term=", 5) == 0 ||
			STRNCMP(arg, "cterm=", 6) == 0 ||
			STRNCMP(arg, "gui=", 4) == 0)
		{
		    char_u *comma = vim_strrchr(p + 1, ',');
		    නම් (comma != NULL)
			p = comma;
		}
		xp->xp_pattern = p + 1;
	    }
	}
	නැතහොත්
	{
	    xp->xp_context = EXPAND_NOTHING;
	}
    }
}

/*
 * List highlighting matches in a nice way.
 */
    static void
highlight_list(void)
{
    පූර්ණ_සංඛ්‍යා		i;

    සඳහා (i = 10; --i >= 0; )
	highlight_list_two(i, HL_ATTR(HLF_D));
    සඳහා (i = 40; --i >= 0; )
	highlight_list_two(99, 0);
}

    static void
highlight_list_two(පූර්ණ_සංඛ්‍යා cnt, පූර්ණ_සංඛ්‍යා attr)
{
    msg_puts_attr(&("N \bI \b!  \b"[cnt / 11]), attr);
    msg_clr_eos();
    out_flush();
    ui_delay(cnt == 99 ? 40L : (long)cnt * 50L, FALSE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_highlight_name(expand_T *xp UNUSED, පූර්ණ_සංඛ්‍යා idx)
{
    ආපසු get_highlight_name_ext(xp, idx, TRUE);
}

/*
 * Obtain a highlight group name.
 * When "skip_cleared" is TRUE don't return a cleared entry.
 */
    char_u *
get_highlight_name_ext(expand_T *xp UNUSED, පූර්ණ_සංඛ්‍යා idx, පූර්ණ_සංඛ්‍යා skip_cleared)
{
    නම් (idx < 0)
	ආපසු NULL;

    // Items are never removed from the table, skip the ones that were
    // cleared.
    නම් (skip_cleared && idx < highlight_ga.ga_len && HL_TABLE()[idx].sg_cleared)
	ආපසු (char_u *)"";

    නම් (idx == highlight_ga.ga_len && include_none != 0)
	ආපසු (char_u *)"NONE";
    නම් (idx == highlight_ga.ga_len + include_none && include_default != 0)
	ආපසු (char_u *)"default";
    නම් (idx == highlight_ga.ga_len + include_none + include_default
							 && include_link != 0)
	ආපසු (char_u *)"link";
    නම් (idx == highlight_ga.ga_len + include_none + include_default + 1
							 && include_link != 0)
	ආපසු (char_u *)"clear";
    නම් (idx >= highlight_ga.ga_len)
	ආපසු NULL;
    ආපසු HL_TABLE()[idx].sg_name;
}

    static char_u *
get_highlight_attr_name(expand_T *xp UNUSED, පූර්ණ_සංඛ්‍යා idx)
{
    නම් (idx == 0)
    {
	// Fill with current value first
	නම් (expand_hi_curvalue != NULL)
	    ආපසු expand_hi_curvalue;
	නැතහොත්
	    ආපසු (char_u*)"";
    }
    නම් (idx < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(highlight_index_tab) + 1)
    {
	char_u *value = highlight_index_tab[idx-1]->value.string;
	නම් (expand_hi_curvalue != NULL && STRCMP(expand_hi_curvalue, value) == 0)
	{
	    // Already returned the current value above, just skip.
	    ආපසු (char_u*)"";
	}
	ආපසු value;
    }
    ආපසු NULL;
}

    static char_u *
get_highlight_cterm_color(expand_T *xp UNUSED, පූර්ණ_සංඛ්‍යා idx)
{
    නම් (idx == 0)
    {
	// Fill with current value first
	නම් (expand_hi_curvalue != NULL)
	    ආපසු expand_hi_curvalue;
	නැතහොත්
	    ආපසු (char_u*)"";
    }
    // See highlight_set_cterm_color()
    නැතහොත් නම් (idx == 1)
	ආපසු (char_u*)"fg";
    නැතහොත් නම් (idx == 2)
	ආපසු (char_u*)"bg";
    නම් (idx < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(color_name_tab) + 3)
    {
	char_u *value = color_name_tab[idx-3].value.string;
	ආපසු value;
    }
    ආපසු NULL;
}

#නම් defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
    static char_u *
get_highlight_gui_color(expand_T *xp UNUSED, පූර්ණ_සංඛ්‍යා idx)
{
    නම් (idx == 0)
    {
	// Fill with current value first
	නම් (expand_hi_curvalue != NULL)
	    ආපසු expand_hi_curvalue;
	නැතහොත්
	    ආපසු (char_u*)"";
    }
    // See color_name2handle()
    නැතහොත් නම් (idx == 1)
	ආපසු (char_u*)"fg";
    නැතහොත් නම් (idx == 2)
	ආපසු (char_u*)"bg";
    නැතහොත් නම් (idx == 3)
	ආපසු (char_u*)"NONE";

    // Complete from v:colornames. Don't do platform specific names for now.
    typval_T *tv_result;
    char_u *colorname = dict_iterate_next(&expand_colornames_iter, &tv_result);
    නම් (colorname != NULL)
    {
	// :hi command doesn't allow space, so don't suggest any malformed items
	නම් (vim_strchr(colorname, ' ') != NULL)
	    ආපසු (char_u*)"";

	නම් (expand_hi_curvalue != NULL && STRICMP(expand_hi_curvalue, colorname) == 0)
	{
	    // Already returned the current value above, just skip.
	    ආපසු (char_u*)"";
	}
    }
    ආපසු colorname;
}
#endif

    static char_u *
get_highlight_group_key(expand_T *xp UNUSED, පූර්ණ_සංඛ්‍යා idx)
{
    // Note: Keep this in sync with do_highlight.
    static var_na *(p_hi_group_key_values[]) =
    {
	"term=",
	"start=",
	"stop=",
	"cterm=",
	"ctermfg=",
	"ctermbg=",
	"ctermul=",
	"ctermfont=",
#නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

    නම් (idx < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(p_hi_group_key_values))
	ආපසු (char_u*)p_hi_group_key_values[idx];
    ආපසු NULL;
}

/*
 * Command-line expansion for :hi {group-name} <args>...
 */
    පූර්ණ_සංඛ්‍යා
expand_highlight_group(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	පූර්ණ_සංඛ්‍යා	    *numMatches)
{
    නම් (expand_hi_equal_col != -1)
    {
	// List the values. First fill in the current value, then if possible colors
	// or attribute names.
	char_u	    *(*expandfunc)(expand_T *, පූර්ණ_සංඛ්‍යා) = NULL;
	පූර්ණ_සංඛ්‍යා	    type = 0;
	hl_group_T  *sgp = NULL;
	පූර්ණ_සංඛ්‍යා	    iarg = 0;
	char_u	    *sarg = NULL;

	පූර්ණ_සංඛ්‍යා	    unsortedItems = -1; // don't sort by default

	නම් (expand_hi_synid != 0)
	    sgp = &HL_TABLE()[expand_hi_synid - 1]; // index is ID minus one

	// Note: Keep this in sync with highlight_list_one().
	char_u	    *name_end = xp->xp_line + expand_hi_equal_col;
	නම් (name_end - xp->xp_line >= 5
		&& STRNCMP(name_end - 5, " term", 5) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    නම් (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_term;
	    }
	}
	නැතහොත් නම් (name_end - xp->xp_line >= 6
		&& STRNCMP(name_end - 6, " cterm", 6) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    නම් (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_cterm;
	    }
	}
#නම් defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	නැතහොත් නම් (name_end - xp->xp_line >= 4
		&& STRNCMP(name_end - 4, " gui", 4) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    නම් (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_gui;
	    }
	}
#endif
	නැතහොත් නම් (name_end - xp->xp_line >= 8
		&& STRNCMP(name_end - 8, " ctermfg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    නම් (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_fg;
	    }
	}
	නැතහොත් නම් (name_end - xp->xp_line >= 8
		&& STRNCMP(name_end - 8, " ctermbg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    නම් (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_bg;
	    }
	}
	නැතහොත් නම් (name_end - xp->xp_line >= 8
		&& STRNCMP(name_end - 8, " ctermul", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    නම් (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_ul;
	    }
	}
#නම් defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	නැතහොත් නම් (name_end - xp->xp_line >= 6
		&& STRNCMP(name_end - 6, " guifg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    නම් (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_fg_name;
	    }
	}
	නැතහොත් නම් (name_end - xp->xp_line >= 6
		&& STRNCMP(name_end - 6, " guibg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    නම් (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_bg_name;
	    }
	}
	නැතහොත් නම් (name_end - xp->xp_line >= 6
		&& STRNCMP(name_end - 6, " guisp", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    නම් (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_sp_name;
	    }
	}
#endif

#නම් defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	නම් (expandfunc == get_highlight_gui_color)
	{
	    // Top 4 items are special, after that sort all the color names
	    unsortedItems = 4;

	    dict_T *colornames_table = get_vim_var_dict(VV_COLORNAMES);
	    typval_T colornames_val;
	    colornames_val.v_type = VAR_DICT;
	    colornames_val.vval.v_dict = colornames_table;
	    dict_iterate_start(&colornames_val, &expand_colornames_iter);
	}
#endif

	char_u	    buf[MAX_ATTR_LEN];

	expand_hi_curvalue = NULL;
	නම් (expand_hi_include_orig)
	{
	    නම් (((type == LIST_ATTR || type == LIST_INT) && iarg != 0) ||
		(type == LIST_STRING && sarg != NULL))
	    {
		// Retrieve the current value to go first in completion
		expand_hi_curvalue = highlight_arg_to_string(
			type, iarg, sarg, buf);
	    }
	}

	නම් (expandfunc != NULL)
	{
	    ආපසු ExpandGenericExt(
		    pat,
		    xp,
		    rmp,
		    matches,
		    numMatches,
		    expandfunc,
		    FALSE,
		    unsortedItems);
	}

	ආපසු FAIL;
    }

    // List all the key names
    ආපසු ExpandGenericExt(
	    pat,
	    xp,
	    rmp,
	    matches,
	    numMatches,
	    get_highlight_group_key,
	    FALSE,
	    -1);
}

#නම් defined(FEAT_GUI)
/*
 * Free all the highlight group fonts.
 * Used when quitting for systems which need it.
 */
    void
free_highlight_fonts(void)
{
    පූර්ණ_සංඛ්‍යා	    idx;

    සඳහා (idx = 0; idx < highlight_ga.ga_len; ++idx)
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

#නම් defined(FEAT_EVAL)
/*
 * Convert each of the highlight attribute bits (bold, standout, underline,
 * etc.) set in 'hlattr' into a separate boolean item in a Dictionary with
 * the attribute name as the key.
 */
    static dict_T *
highlight_get_attr_dict(පූර්ණ_සංඛ්‍යා hlattr)
{
    dict_T	*dict;
    පූර්ණ_සංඛ්‍යා		i;

    dict = dict_alloc();
    නම් (dict == NULL)
	ආපසු NULL;

    සඳහා (i = 0; i < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(highlight_index_tab); ++i)
    {
	නම් (hlattr & highlight_index_tab[i]->key)
	{
	    dict_add_bool(dict, (var_na *)highlight_index_tab[i]->value.string,
		    VVAL_TRUE);
	    // don't want "inverse"/"reverse"
	    hlattr &= ~highlight_index_tab[i]->key;
	}
    }

    ආපසු dict;
}

/*
 * Return the attributes of the highlight group at index 'hl_idx' as a
 * Dictionary. If 'resolve_link' is TRUE, then resolves the highlight group
 * links recursively.
 */
    static dict_T *
highlight_get_info(පූර්ණ_සංඛ්‍යා hl_idx, පූර්ණ_සංඛ්‍යා resolve_link)
{
    dict_T	*dict;
    hl_group_T	*sgp;
    dict_T	*attr_dict = NULL;
    පූර්ණ_සංඛ්‍යා		hlgid;

    dict = dict_alloc();
    නම් (dict == NULL)
	ආපසු dict;

    sgp = &HL_TABLE()[hl_idx];
    // highlight group id is 1-based
    hlgid = hl_idx + 1;

    නම් (dict_add_string(dict, "name", sgp->sg_name) == FAIL)
	goto error;
    නම් (dict_add_number(dict, "id", hlgid) == FAIL)
	goto error;

    නම් (resolve_link)
    {
	// resolve the highlight group link recursively
	අතරතුර (sgp->sg_link)
	{
	    hlgid = sgp->sg_link;
	    sgp = &HL_TABLE()[sgp->sg_link - 1];
	}
    }

    නම් (sgp->sg_term != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_term);
	නම් (attr_dict != NULL)
	{
	    නම් (dict_add_dict(dict, "term", attr_dict) == FAIL)
		goto error;
	    attr_dict = NULL;
	}
    }
    නම් (sgp->sg_start != NULL)
	නම් (dict_add_string(dict, "start", sgp->sg_start) == FAIL)
	    goto error;
    නම් (sgp->sg_stop != NULL)
	නම් (dict_add_string(dict, "stop", sgp->sg_stop) == FAIL)
	    goto error;
    නම් (sgp->sg_cterm != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_cterm);
	නම් (attr_dict != NULL)
	{
	    නම් (dict_add_dict(dict, "cterm", attr_dict) == FAIL)
		goto error;
	    attr_dict = NULL;
	}
    }
    නම් (sgp->sg_cterm_fg != 0)
	නම් (dict_add_string(dict, "ctermfg",
		    highlight_color(hlgid, (char_u *)"fg", 'c')) == FAIL)
	    goto error;
    නම් (sgp->sg_cterm_bg != 0)
	නම් (dict_add_string(dict, "ctermbg",
			highlight_color(hlgid, (char_u *)"bg", 'c')) == FAIL)
	    goto error;
    නම් (sgp->sg_cterm_ul != 0)
	නම් (dict_add_string(dict, "ctermul",
			highlight_color(hlgid, (char_u *)"ul", 'c')) == FAIL)
	    goto error;
    නම් (sgp->sg_cterm_font != 0)
	නම් (dict_add_string(dict, "ctermfont",
			highlight_color(hlgid, (char_u *)"font", 'c')) == FAIL)
	    goto error;
    නම් (sgp->sg_gui != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_gui);
	නම් (attr_dict != NULL)
	{
	    නම් (dict_add_dict(dict, "gui", attr_dict) == FAIL)
		goto error;
	    attr_dict = NULL;
	}
    }
    නම් (sgp->sg_gui_fg_name != NULL)
	නම් (dict_add_string(dict, "guifg",
			highlight_color(hlgid, (char_u *)"fg", 'g')) == FAIL)
	    goto error;
    නම් (sgp->sg_gui_bg_name != NULL)
	නම් (dict_add_string(dict, "guibg",
			highlight_color(hlgid, (char_u *)"bg", 'g')) == FAIL)
	    goto error;
    නම් (sgp->sg_gui_sp_name != NULL)
	නම් (dict_add_string(dict, "guisp",
			highlight_color(hlgid, (char_u *)"sp", 'g')) == FAIL)
	    goto error;
# ifdef FEAT_GUI
    නම් (sgp->sg_font_name != NULL)
	නම් (dict_add_string(dict, "font", sgp->sg_font_name) == FAIL)
	    goto error;
# endif
    නම් (sgp->sg_link)
    {
	char_u	*link;

	link = HL_TABLE()[sgp->sg_link - 1].sg_name;
	නම් (link != NULL && dict_add_string(dict, "linksto", link) == FAIL)
	    goto error;

	නම් (sgp->sg_deflink)
	    dict_add_bool(dict, "default", VVAL_TRUE);
    }
    නම් (dict_len(dict) == 2)
	// If only 'name' is present, then the highlight group is cleared.
	dict_add_bool(dict, "cleared", VVAL_TRUE);

    ආපසු dict;

error:
    dict_unref(attr_dict);
    dict_unref(dict);
    ආපසු NULL;
}

/*
 * "hlget([name])" function
 * Return the attributes of a specific highlight group (if specified) or all
 * the highlight groups.
 */
    void
f_hlget(typval_T *argvars, typval_T *rettv)
{
    list_T	*list;
    dict_T	*dict;
    පූර්ණ_සංඛ්‍යා		i;
    char_u	*hlarg = NULL;
    පූර්ණ_සංඛ්‍යා		resolve_link = FALSE;

    නම් (rettv_list_alloc(rettv) == FAIL)
	ආපසු;

    නම් (check_for_opt_string_arg(argvars, 0) == FAIL
	    || (argvars[0].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_arg(argvars, 1) == FAIL))
	ආපසු;

    නම් (argvars[0].v_type != VAR_UNKNOWN)
    {
	// highlight group name supplied
	hlarg = tv_get_string_chk(&argvars[0]);
	නම් (hlarg == NULL)
	    ආපසු;

	නම් (argvars[1].v_type != VAR_UNKNOWN)
	{
	    පූර්ණ_සංඛ්‍යා error = FALSE;

	    resolve_link = tv_get_bool_chk(&argvars[1], &error);
	    නම් (error)
		ආපසු;
	}
    }

    list = rettv->vval.v_list;
    සඳහා (i = 0; i < highlight_ga.ga_len && !got_int; ++i)
    {
	නම් (hlarg == NULL || STRICMP(hlarg, HL_TABLE()[i].sg_name) == 0)
	{
	    dict = highlight_get_info(i, resolve_link);
	    නම් (dict != NULL && list_append_dict(list, dict) == FAIL)
		dict_unref(dict);
	}
    }
}

/*
 * Returns the string value at 'dict[key]'. Returns NULL, if 'key' is not in
 * 'dict' or the value is not a string type. If the value is not a string type
 * or is NULL, then 'error' is set to TRUE.
 */
    static char_u *
hldict_get_string(dict_T *dict, char_u *key, පූර්ණ_සංඛ්‍යා *error)
{
    dictitem_T	*di;

    *error = FALSE;
    di = dict_find(dict, key, -1);
    නම් (di == NULL)
	ආපසු NULL;

    නම් (di->di_tv.v_type != VAR_STRING || di->di_tv.vval.v_string == NULL)
    {
	emsg(_(e_string_required));
	*error = TRUE;
	ආපසු NULL;
    }

    ආපසු di->di_tv.vval.v_string;
}

/*
 * Convert the highlight attribute Dictionary at 'dict[key]' into a string
 * value in 'attr_str' of length 'len'. Returns FALSE if 'dict[key]' is not a
 * Dictionary or is NULL.
 */
    static පූර්ණ_සංඛ්‍යා
hldict_attr_to_str(
	dict_T	*dict,
	char_u	*key,
	char_u	*attr_str,
	size_t	len)
{
    dictitem_T	*di;
    dict_T	*attrdict;
    පූර්ණ_සංඛ්‍යා		i;
    char_u	*p;

    attr_str[0] = NUL;
    di = dict_find(dict, key, -1);
    නම් (di == NULL)
	ආපසු TRUE;

    නම් (di->di_tv.v_type != VAR_DICT || di->di_tv.vval.v_dict == NULL)
    {
	emsg(_(e_dictionary_required));
	ආපසු FALSE;
    }

    attrdict = di->di_tv.vval.v_dict;

    // If the attribute dict is empty, then return NONE to clear the attributes
    නම් (dict_len(attrdict) == 0)
    {
	vim_strcat(attr_str, (char_u *)"NONE", len);
	ආපසු TRUE;
    }

    p = attr_str;
    සඳහා (i = 0; i < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(highlight_tab); ++i)
    {
	නම් (dict_get_bool(attrdict, (var_na *)highlight_tab[i].value.string,
		    VVAL_FALSE) == VVAL_TRUE)
	{
	    නම් (p != attr_str && (size_t)(p - attr_str + 2) < len)
		STRCPY(p, (char_u *)",");
	    නම් (p - attr_str + highlight_tab[i].value.length + 1 < len)
	    {
		STRCPY(p, highlight_tab[i].value.string);
		p += highlight_tab[i].value.length;
	    }
	}
    }

    ආපසු TRUE;
}

// Temporary buffer used to store the command string produced by hlset().
// IObuff cannot be used for this as the error messages produced by hlset()
// internally use IObuff.
# define	HLSETBUFSZ  512
static char_u hlsetBuf[HLSETBUFSZ + 1];

/*
 * Add the highlight attribute "attr" of length "attrlen" and "value" at
 * "dptr", which points into "hlsetBuf".
 * Returns the updated pointer.
 */
    static char_u *
add_attr_and_value(char_u *dptr, char_u *attr, පූර්ණ_සංඛ්‍යා attrlen, char_u *value)
{
    size_t	vallen;

    // Do nothing if the value is not specified or is empty
    නම් (value == NULL || *value == NUL)
	ආපසු dptr;

    vallen = STRLEN(value);
    // When the value contains a space and the attribute has an "=" (i.e. it
    // is a key=value pair), surround the value with single quotes so that
    // do_highlight() can parse it correctly.
    නම් (vim_strchr(value, ' ') != NULL && vim_strchr(attr, '=') != NULL)
    {
	නම් (dptr + attrlen + vallen + 3 < hlsetBuf + HLSETBUFSZ)
	{
	    STRCPY(dptr, attr);
	    dptr += attrlen;
	    *dptr++ = '\'';
	    STRCPY(dptr, value);
	    dptr += vallen;
	    *dptr++ = '\'';
	    *dptr = NUL;
	}
    }
    නැතහොත් නම් (dptr + attrlen + vallen + 1 < hlsetBuf + HLSETBUFSZ)
    {
	STRCPY(dptr, attr);
	dptr += attrlen;
	STRCPY(dptr, value);
	dptr += vallen;
    }

    ආපසු dptr;
}

/*
 * Add or update a highlight group using 'dict' items. Returns TRUE if
 * successfully updated the highlight group.
 */
    static පූර්ණ_සංඛ්‍යා
hlg_add_or_update(dict_T *dict)
{
    char_u	*name;
    පූර්ණ_සංඛ්‍යා		error;
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
    පූර්ණ_සංඛ්‍යා		forceit = FALSE;
    පූර්ණ_සංඛ්‍යා		dodefault = FALSE;
    පූර්ණ_සංඛ්‍යා		done = FALSE;
    char_u	*p;

    name = hldict_get_string(dict, (char_u *)"name", &error);
    නම් (name == NULL || *name == NUL || error)
	ආපසු FALSE;

    නම් (dict_get_bool(dict, "force", VVAL_FALSE) == VVAL_TRUE)
	forceit = TRUE;

    නම් (dict_get_bool(dict, "default", VVAL_FALSE) == VVAL_TRUE)
	dodefault = TRUE;

    නම් (dict_has_key(dict, "cleared"))
    {
	varnumber_T	cleared;

	// clear a highlight group
	cleared = dict_get_bool(dict, "cleared", FALSE);
	නම් (cleared == TRUE)
	{
	    vim_snprintf((var_na *)hlsetBuf, HLSETBUFSZ, "clear %s", name);
	    do_highlight(hlsetBuf, forceit, FALSE);
	    done = TRUE;
	}
    }

    නම් (dict_has_key(dict, "linksto"))
    {
	char_u	*linksto;

	// link highlight groups
	linksto = hldict_get_string(dict, (char_u *)"linksto", &error);
	නම් (linksto == NULL || *linksto == NUL || error)
	    ආපසු FALSE;

	vim_snprintf((var_na *)hlsetBuf, HLSETBUFSZ, "%slink %s %s",
				dodefault ? "default " : "", name, linksto);
	do_highlight(hlsetBuf, forceit, FALSE);

	done = TRUE;
    }

    // If 'cleared' or 'linksto' are specified, then don't process the other
    // attributes.
    නම් (done)
	ආපසු TRUE;

    start = hldict_get_string(dict, (char_u *)"start", &error);
    නම් (error)
	ආපසු FALSE;

    stop = hldict_get_string(dict, (char_u *)"stop", &error);
    නම් (error)
	ආපසු FALSE;

    නම් (!hldict_attr_to_str(dict, (char_u *)"term", term_attr,
							    parimaa_na(term_attr)))
	ආපසු FALSE;

    නම් (!hldict_attr_to_str(dict, (char_u *)"cterm", cterm_attr,
							   parimaa_na(cterm_attr)))
	ආපසු FALSE;

    ctermfg = hldict_get_string(dict, (char_u *)"ctermfg", &error);
    නම් (error)
	ආපසු FALSE;

    ctermbg = hldict_get_string(dict, (char_u *)"ctermbg", &error);
    නම් (error)
	ආපසු FALSE;

    ctermul = hldict_get_string(dict, (char_u *)"ctermul", &error);
    නම් (error)
	ආපසු FALSE;

    ctermfont = hldict_get_string(dict, (char_u *)"ctermfont", &error);
    නම් (error)
	ආපසු FALSE;

    නම් (!hldict_attr_to_str(dict, (char_u *)"gui", gui_attr, parimaa_na(gui_attr)))
	ආපසු FALSE;

    guifg = hldict_get_string(dict, (char_u *)"guifg", &error);
    නම් (error)
	ආපසු FALSE;

    guibg = hldict_get_string(dict, (char_u *)"guibg", &error);
    නම් (error)
	ආපසු FALSE;

    guisp = hldict_get_string(dict, (char_u *)"guisp", &error);
    නම් (error)
	ආපසු FALSE;

# ifdef FEAT_GUI
    font = hldict_get_string(dict, (char_u *)"font", &error);
    නම් (error)
	ආපසු FALSE;
# endif

    // If none of the attributes are specified, then do nothing.
    නම් (term_attr[0] == NUL && start == NULL && stop == NULL
	    && cterm_attr[0] == NUL && ctermfg == NULL && ctermbg == NULL
	    && ctermul == NULL && ctermfont == NULL && gui_attr[0] == NUL
# ifdef FEAT_GUI
	    && font == NULL
# endif
	    && guifg == NULL && guibg == NULL && guisp == NULL
	    )
	ආපසු TRUE;

    hlsetBuf[0] = NUL;
    p = hlsetBuf;
    නම් (dodefault)
	p = add_attr_and_value(p, (char_u *)"default", 7, (char_u *)" ");
    p = add_attr_and_value(p, (char_u *)"", 0, name);
    p = add_attr_and_value(p, (char_u *)" term=", 6, term_attr);
    p = add_attr_and_value(p, (char_u *)" start=", 7, start);
    p = add_attr_and_value(p, (char_u *)" stop=", 6, stop);
    p = add_attr_and_value(p, (char_u *)" cterm=", 7, cterm_attr);
    p = add_attr_and_value(p, (char_u *)" ctermfg=", 9, ctermfg);
    p = add_attr_and_value(p, (char_u *)" ctermbg=", 9, ctermbg);
    p = add_attr_and_value(p, (char_u *)" ctermul=", 9, ctermul);
    p = add_attr_and_value(p, (char_u *)" ctermfont=", 11, ctermfont);
    p = add_attr_and_value(p, (char_u *)" gui=", 5, gui_attr);
# ifdef FEAT_GUI
    p = add_attr_and_value(p, (char_u *)" font=", 6, font);
# endif
    p = add_attr_and_value(p, (char_u *)" guifg=", 7, guifg);
    p = add_attr_and_value(p, (char_u *)" guibg=", 7, guibg);
    (void)add_attr_and_value(p, (char_u *)" guisp=", 7, guisp);

    do_highlight(hlsetBuf, forceit, FALSE);

    ආපසු TRUE;
}

/*
 * "hlset([{highlight_attr}])" function
 * Add or modify highlight groups
 */
    void
f_hlset(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    dict_T	*dict;

    rettv->vval.v_number = -1;

    නම් (check_for_list_arg(argvars, 0) == FAIL)
	ආපසු;

    FOR_ALL_LIST_ITEMS(argvars->vval.v_list, li)
    {
	නම් (li->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_dictionary_required));
	    ආපසු;
	}

	dict = li->li_tv.vval.v_dict;
	නම් (!hlg_add_or_update(dict))
	    ආපසු;
    }

    rettv->vval.v_number = 0;
}
#endif

/*
 * If "old" is in the override stack, then update it to "hl_new". Does not free
 * "old".
 */
    void
update_highlight_overrides(hl_override_T *old, hl_override_T *hl_new, පූර්ණ_සංඛ්‍යා newlen)
{
    නම් (old == NULL)
	ආපසු;

    සඳහා (hl_overrides_T *set = overrides; set != NULL; set = set->next)
    {
	නම් (set->arr == old)
	{
	    set->arr = hl_new;
	    set->len = newlen;
	}
    }
}

/*
 * If "arr" is in the highlight overrides list, then mark it as invalid.
 */
    void
remove_highlight_overrides(hl_override_T *arr)
{
    නම් (arr == NULL || overrides == NULL)
	ආපසු;

    සඳහා (hl_overrides_T *set = overrides; set != NULL; set = set->next)
    {
	නම් (set->arr == arr)
	{
	    set->arr = NULL;
	    bha_nga;
	}
    }
}

/*
 * Update highlight_attr[] array. If "update_ids" is true, then update
 * highlight_ids[] array instead.
 */
    static void
set_highlight_attr(hl_override_T *arr, පූර්ණ_සංඛ්‍යා len, bool update_ids)
{
    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < len; i++)
    {
	hl_override_T	*override = arr + i;
	පූර්ණ_සංඛ්‍යා		hlf = -1, attr;

	නම් (override->from <= 0)
	    // Directly map to highlight_attr
	    hlf = -override->from;
	නැතහොත්
	{
	    සඳහා (පූර්ණ_සංඛ්‍යා k = 0; k < HLF_COUNT; k++)
		නම් (override->from == highlight_ids[k])
		{
		    hlf = k;
		    bha_nga;
		}

	    නම් (hlf == -1)
		continue;
	}

	නම් (update_ids)
	{
	    නම් (override->to <= 0)
	    {
		නම් (hlf == -override->to)
		    highlight_ids[hlf] = -1;
		නැතහොත්
		    highlight_ids[hlf] = highlight_ids[-override->to];
	    }
	    නැතහොත්
		highlight_ids[hlf] = override->to;
	}
	නැතහොත්
	{
	    නම් (override->to <= 0)
		// Directly use highlight_attr[]
		attr = highlight_attr[-override->to];
	    නැතහොත්
		attr = syn_id2attr(override->to);

	    highlight_attr[hlf] = attr;
	}
    }
}

/*
 * Set the current highlight override to "arr". Returns true if successful
 */
    bool
push_highlight_overrides(hl_override_T *arr, පූර්ණ_සංඛ්‍යා len)
{
    // Don't want to do anything if "arr" is already the current override. If
    // "arr" is NULL (but overrides->arr is not), then still push an override,
    // but "->arr" will just be NULL so any previous overrides are cleared.
    නම් (overrides != NULL && overrides->arr == arr)
	ආපසු false;

    hl_overrides_T *set;

    set = ALLOC_ONE(hl_overrides_T);
    නම් (set == NULL)
	ආපසු false;

    set->arr = arr;
    set->len = len;

    // Push to front of list
    set->next = overrides;
    overrides = set;

    // Save current highlight_attr[], so it can be restored later after this
    // override is popped.
    memcpy(set->attr, highlight_attr, parimaa_na(highlight_attr));

    // Reset highlight_attr[] to default values, as we don't want changes from
    // other overrides.
    memcpy(highlight_attr, highlight_attr_raw, parimaa_na(highlight_attr));

    // Update highlight_attr[] array
    නම් (arr != NULL)
	set_highlight_attr(arr, len, false);

    ආපසු true;
}

/*
 * Pop the current highlight override (if any) from the stack. This should
 * always be paired with a *successful* push.
 */
    void
pop_highlight_overrides(void)
{
    hl_overrides_T *set = overrides;

    නම් (overrides == NULL)
	ආපසු;

    overrides = set->next;

    // Set highlight_attr[] to state before override was pushed.
    memcpy(highlight_attr, set->attr, parimaa_na(highlight_attr));
    vim_free(set);
}

/*
 * Parse the 'winhighlight' option and return array. Returns NULL on failure or
 * if empty option. If failure, then errmsg is set.
 */
    static hl_override_T *
parse_winhighlight(char_u *opt, පූර්ණ_සංඛ්‍යා *len, var_na **errmsg)
{
    char_u	    *p = opt;
    hl_override_T   *arr;
    පූර්ණ_සංඛ්‍යා		    i = 0;
    පූර්ණ_සංඛ්‍යා		    num = 1;
    පූර්ණ_සංඛ්‍යා		    n_colons = 0;

    නම් (*p == NUL)
	ආපසු NULL;

    // Get number of overrides first so we can allocate array
    අතරතුර ((p = vim_strchr(p, ',')) != NULL)
    {
	p++;
	num++;
    }
    p = opt;
    // Check if number of ':' matches number of ','
    අතරතුර ((p = vim_strchr(p, ':')) != NULL)
    {
	p++;
	n_colons++;
    }

    නම් (num != n_colons)
    {
	*errmsg = e_invalid_argument;
	ආපසු NULL;
    }

    arr = ALLOC_MULT(hl_override_T, num);
    නම් (arr == NULL)
    {
	*errmsg = e_out_of_memory;
	ආපසු NULL;
    }

    p = opt;

    අතරතුර (true)
    {
	hl_override_T *override = arr + i++;
	char_u	*fromname = p, *toname;
	char_u	*tmp;
	char_u	**names[2] = {&fromname, &toname};
	පූර්ණ_සංඛ්‍යා	fromlen, tolen;
	පූර්ණ_සංඛ්‍යා	*lens[2] = {&fromlen, &tolen};
	පූර්ණ_සංඛ්‍යා	fromid, toid;
	පූර්ණ_සංඛ්‍යා	*ids[2] = {&fromid, &toid};

	p = vim_strchr(p, ':');

	නම් (p == NULL)
	    goto fail;

	fromlen = p - fromname; // Get hl for "from"
	නම් (fromlen == 0)
	    goto fail;
	p++; // Skip colon ':'
	නම් (*p == NUL)
	    goto fail;

	toname = p;
	tmp = vim_strchr(p, ',');

	// Get hl for "to", must check for no trailing comma in case last
	// element.
	නම් (tmp == NULL)
	    tolen = (පූර්ණ_සංඛ්‍යා)STRLEN(p);
	නැතහොත්
	{
	    tolen = tmp - toname;
	    p = ++tmp;
	}
	නම් (tolen == 0)
	    goto fail;

	සඳහා (පූර්ණ_සංඛ්‍යා k = 0; k < 2; k++)
	{
	    char_u  *name = *names[k];
	    පූර්ණ_සංඛ්‍යා	    nlen = *lens[k];

	    නම් (*name == '!')
	    {
		// If starts with "!", then it maps directly to a 'highlight'
		// occasion.
		පූර්ණ_සංඛ්‍යා hlf;

		නම් (nlen != 2)
		    goto fail;

		සඳහා (hlf = 0; hlf < (පූර්ණ_සංඛ්‍යා)HLF_COUNT; ++hlf)
		    නම් (hl_flags[hlf] == name[1])
			bha_nga;

		නම් (hlf >= HLF_COUNT)
		    goto fail;

		*ids[k] = -hlf;
	    }
	    නැතහොත්
	    {
		// Otherwise get the highlight group id.
		නම් (syn_check_group(name, nlen) == 0)
		    goto fail;

		*ids[k] = syn_namen2id(name, nlen);
		නම් (*ids[k] == 0)
		    goto fail;

		// Always map "Normal" group to HLF_WIN
		නම් (ids[k] == &fromid &&
			STRCMP(HL_TABLE()[*ids[k] - 1].sg_name_u, "NORMAL") == 0)
		    *ids[k] = -HLF_WIN;
	    }
	}

	// If fromid == toid, leave it be, this is so that we know HLF_WIN has
	// been set (even to itself e.g. "Normal"), and therefore know to use
	// its attr instead.
	override->from = fromid;
	override->to = toid;

	නම් (tmp == NULL)
	    bha_nga;
    }

    *len = num;
    ආපසු arr;
fail:
    vim_free(arr);
    *errmsg = e_invalid_argument;
    ආපසු NULL;
}

/*
 * Update w_hl for "wp" using given option value. Returns error
 * message on failure, otherwise NULL.
 */
    var_na *
update_winhighlight(win_T *wp, char_u *opt)
{
    var_na	    *err = NULL;
    hl_override_T   *arr;
    පූර්ණ_සංඛ්‍යා		    num = 1;

    arr = parse_winhighlight(opt, &num, &err);

    නම් (arr == NULL && err != NULL)
	ආපසු err;

    නම් (arr == NULL && wp->w_hl == NULL)
	ආපසු NULL;

    update_highlight_overrides(wp->w_hl, arr, num);

    vim_free(wp->w_hl);
    wp->w_hl = arr;
    wp->w_hl_len = num;

    wp->w_hlfwin_id = hlf_get_id(wp, HLF_WIN);

#ifdef FEAT_TERMINAL
    term_update_hlfwin(wp);
#endif

#ifdef FEAT_TERMINAL
    // May be NULL (such as in after_copy_winopt())
    නම් (wp->w_buffer != NULL)
    {
	// Make sure terminal highlighting is updated
	bool ret = push_highlight_overrides(wp->w_hl, wp->w_hl_len);

	නම් (wp->w_buffer->b_term != NULL)
	    term_init_default_colors(wp->w_buffer->b_term);
	නම් (ret)
	    pop_highlight_overrides();
    }
#endif

    ආපසු NULL;
}

/*
 * Get id that the hlf attr is using in window "wp", otherwise zero. Note that
 * -1 may be returned if the hlf was set to itself.
 */
    පූර්ණ_සංඛ්‍යා
hlf_get_id(win_T *wp, පූර්ණ_සංඛ්‍යා hlf)
{
    පූර්ණ_සංඛ්‍යා id;
    static පූර්ණ_සංඛ්‍යා prev[HLF_COUNT];

    නම් (wp->w_hl == NULL)
	ආපසු highlight_ids[hlf];

    memcpy(prev, highlight_ids, parimaa_na(highlight_ids));
    set_highlight_attr(wp->w_hl, wp->w_hl_len, true);
    id = highlight_ids[hlf];
    memcpy(highlight_ids, prev, parimaa_na(highlight_ids));

    ආපසු id;
}

/*
 * Update 'winhighlight' using a 'wincolor' style option value. Returns error
 * message on failure otherwise NULL. Does not update the actual 'wincolor'
 * value, but does update the 'winhighlight' value.
 */
    var_na *
update_wincolor(win_T *wp, char_u *opt)
{
    char_u  *str = *opt == NUL ? "" : alloc(parimaa_na("!(:") + STRLEN(opt));
    var_na    *errmsg;

    නම් (str == NULL)
	ආපසු e_out_of_memory;

    නම් (*opt != NUL)
	sprintf((var_na *)str, "!(:%s", opt);

    errmsg = update_winhighlight(wp, str);

    නම් (errmsg == NULL)
	set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		str, OPT_FREE|OPT_LOCAL, 0);

    නම් (*opt != NUL)
	free(str);
    ආපසු errmsg;
}
