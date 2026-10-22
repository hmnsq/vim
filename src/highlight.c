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

enum {
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
typedef struct
{
    char_u	*sg_name;	// highlight group name
    char_u	*sg_name_u;	// uppercase of sg_name
    цяло		sg_cleared;	// "hi clear" was used
// for normal terminals
    цяло		sg_term;	// "term=" highlighting attributes
    char_u	*sg_start;	// terminal string for start highl
    char_u	*sg_stop;	// terminal string for stop highl
    цяло		sg_term_attr;	// Screen attr for term mode
// for color terminals
    цяло		sg_cterm;	// "cterm=" highlighting attr
    цяло		sg_cterm_bold;	// bold attr was set for light color
    цяло		sg_cterm_fg;	// terminal fg color number + 1
    цяло		sg_cterm_bg;	// terminal bg color number + 1
    цяло		sg_cterm_ul;	// terminal ul color number + 1
    цяло		sg_cterm_attr;	// Screen attr for color term mode
    цяло		sg_cterm_font;	// terminal alternative font (0 for normal)
// for when using the GUI
#ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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
    цяло		sg_gui_attr;    // Screen attr for GUI mode
#endif
#ако defined(FEAT_GUI) || defined(FEAT_EVAL)
// Store the sp color name for the GUI or synIDattr()
    цяло		sg_gui;		// "gui=" highlighting attributes
    char_u	*sg_gui_fg_name;// GUI foreground color name
    char_u	*sg_gui_bg_name;// GUI background color name
    char_u	*sg_gui_sp_name;// GUI special color name
#endif
    цяло		sg_link;	// link to this highlight group ID
    цяло		sg_deflink;	// default link; restored in highlight_clear()
    цяло		sg_set;		// combination of SG_* flags
#ifdef FEAT_EVAL
    sctx_T	sg_deflink_sctx;  // script where the default link was set
    sctx_T	sg_script_ctx;	// script in which the group was last set
#endif
} hl_group_T;

typedef struct hl_overrides_S hl_overrides_T;
struct hl_overrides_S
{
    hl_override_T   *arr;   // May be NULL if "arr" was freed or no highlight
			    // overrides (all values set to default)
    цяло		    len;
    hl_overrides_T  *next;  // Used to handle recursive calls

    цяло attr[HLF_COUNT];    // highlight_attr[] before being updated.
};

static hl_overrides_T *overrides = NULL;

// Synced with highlight_attr, each is the highlight group id for each attr. If
// an element is 0, then there is no attr for it. If an element is -1, then it
// means it was set to itself.
static цяло highlight_ids[HLF_COUNT];

// Same as highlight_attr[] but is not modified by highlight group overrides
static цяло highlight_attr_raw[HLF_COUNT];

static цяло hl_flags[HLF_COUNT] = HL_FLAGS;

// highlight groups for 'highlight' option
static garray_T highlight_ga;
#define HL_TABLE()	((hl_group_T *)((highlight_ga.ga_data)))

// Wrapper struct for hashtable entries: stores the highlight group ID alongside
// the uppercase name used as a hash key.  Uses the same offsetof pattern as
// signgroup_T / HI2SG.
typedef struct {
    цяло		hn_id;		// highlight group ID (1-based)
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
static void set_hl_attr(цяло idx);
static void highlight_list_one(цяло id);
static цяло highlight_list_arg(цяло id, цяло didh, цяло type, цяло iarg, char_u *sarg, char *name);
static цяло syn_add_group(char_u *name);
static цяло syn_name2id_len(char_u *name, цяло len);
static цяло hl_has_settings(цяло idx, цяло check_link);
static void highlight_clear(цяло idx);

#ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
static void gui_do_one_color(цяло idx, цяло do_menu, цяло do_tooltip);
#endif
#ifdef FEAT_GUI
static цяло  set_group_colors(char_u *name, guicolor_T *fgp, guicolor_T *bgp, цяло do_menu, цяло use_norm, цяло do_tooltip);
static void hl_do_font(цяло idx, char_u *arg, цяло do_normal, цяло do_menu, цяло do_tooltip, цяло free_font);
#endif

/*
 * The default highlight groups.  These are compiled-in for fast startup and
 * they still work when the runtime files can't be found.
 * When making changes here, also change runtime/colors/default.vim!
 * The #ifdefs are needed to reduce the amount of static data.  Helps to make
 * the 16 bit DOS (museum) version compile.
 */
#ако defined(FEAT_GUI) || defined(FEAT_EVAL)
# define CENT(a, b) b
#иначе
# define CENT(a, b) a
#endif
static char *(highlight_init_both[]) = {
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
static char *(highlight_init_light[]) = {
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
static char *(highlight_init_dark[]) = {
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

#ако defined(FEAT_SYN_HL)
/*
 * Returns the number of highlight groups.
 */
    цяло
highlight_num_groups(void)
{
    върни highlight_ga.ga_len;
}

/*
 * Returns the name of a highlight group.
 */
    char_u *
highlight_group_name(цяло id)
{
    върни HL_TABLE()[id].sg_name;
}

/*
 * Returns the ID of the link to a highlight group.
 */
    цяло
highlight_link_id(цяло id)
{
    върни HL_TABLE()[id].sg_link;
}
#endif

static void resolve_fallback_fg_to_rgb(void);
static void resolve_fallback_bg_to_rgb(void);

    void
init_highlight(
    цяло		both,	    // include groups where 'bg' doesn't matter
    цяло		reset)	    // clear group first
{
    цяло		i;
    char	**pp;
    static цяло	had_both = FALSE;
#ifdef FEAT_EVAL
    char_u	*p;

    // Try finding the color scheme file.  Used when a color file was loaded
    // and 'background' or 't_Co' is changed.
    p = get_var_value((char_u *)"g:colors_name");
    ако (p != NULL)
    {
	// The value of g:colors_name could be freed when sourcing the script,
	// making "p" invalid, so copy it.
	char_u *copy_p = vim_strsave(p);
	цяло    r;

	ако (copy_p != NULL)
	{
	    r = load_colors(copy_p);
	    vim_free(copy_p);
	    ако (r == OK)
		върни;
	}
    }

#endif

    // Didn't use a color file, use the compiled-in colors.
    ако (both)
    {
	had_both = TRUE;
	pp = highlight_init_both;
	за (i = 0; pp[i] != NULL; ++i)
	    do_highlight((char_u *)pp[i], reset, TRUE);
    }
    иначе ако (!had_both)
	// Don't do anything before the call with both == TRUE from main().
	// Not everything has been setup then, and that call will overrule
	// everything anyway.
	върни;

    ако (*p_bg == 'l')
	pp = highlight_init_light;
    иначе
	pp = highlight_init_dark;
    за (i = 0; pp[i] != NULL; ++i)
	do_highlight((char_u *)pp[i], reset, TRUE);

    // Reverse looks ugly, but grey may not work for less than 8 colors.  Thus
    // let it depend on the number of colors available.
    ако (t_colors < 8)
	do_highlight((char_u *)"Visual term=reverse cterm=reverse ctermbg=NONE ctermfg=NONE",
		FALSE, TRUE);
    // With 8 colors brown is equal to yellow, need to use black for Search fg
    // to avoid Statement highlighted text disappears.
    // Clear the attributes, needed when changing the t_Co value.
    ако (t_colors <= 8)
    {
	ако (*p_bg == 'l')
	    do_highlight((char_u *)"Search ctermfg=black", FALSE, TRUE);
    }

#ifdef FEAT_SYN_HL
    // If syntax highlighting is enabled load the highlighting for it.
    ако (get_var_value((char_u *)"g:syntax_on") != NULL)
    {
	static цяло	recursive = 0;

	ако (recursive >= 5)
	    emsg(_(e_recursive_loop_loading_syncolor_vim));
	иначе
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

#ако defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
/*
 * Load a default color list. Intended to support legacy color names but allows
 * the user to override the color values. Only loaded once.
 */
    static void
load_default_colors_lists(void)
{
    // Lacking a default color list isn't the end of the world but it is likely
    // an inconvenience so users should know when it is missing.
    ако (source_runtime((char_u *)"colors/lists/default.vim", DIP_ALL) != OK)
	msg("failed to load colors/lists/default.vim");
}
#endif

/*
 * Load color file "name".
 * Return OK for success, FAIL for failure.
 */
    цяло
load_colors(char_u *name)
{
    char_u	*buf;
    цяло		retval = FAIL;
    static цяло	recursive = FALSE;

    // When being called recursively, this is probably because setting
    // 'background' caused the highlighting to be reloaded.  This means it is
    // working, thus we should return OK.
    ако (recursive)
	върни OK;

    recursive = TRUE;
    buf = alloc(STRLEN(name) + 12);
    ако (buf != NULL)
    {
#ако defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	load_default_colors_lists();
#endif
	apply_autocmds(EVENT_COLORSCHEMEPRE, name,
					       curbuf->b_fname, FALSE, curbuf);
	sprintf((char *)buf, "colors/%s.vim", name);
	retval = source_runtime(buf, DIP_START + DIP_OPT);
	vim_free(buf);
	ако (retval == OK)
	    apply_autocmds(EVENT_COLORSCHEME, name, curbuf->b_fname,
								FALSE, curbuf);
    }
    recursive = FALSE;

    върни retval;
}

static цяло color_numbers_16[28] = {0, 1, 2, 3,
				 4, 5, 6, 6,
				 7, 7, 7, 7,
				 8, 8,
				 9, 9, 10, 10,
				 11, 11, 12, 12, 13,
				 13, 14, 14, 15, -1};
// for xterm with 88 colors...
static цяло color_numbers_88[28] = {0, 4, 2, 6,
				 1, 5, 32, 72,
				 84, 84, 7, 7,
				 82, 82,
				 12, 43, 10, 61,
				 14, 63, 9, 74, 13,
				 75, 11, 78, 15, -1};
// for xterm with 256 colors...
static цяло color_numbers_256[28] = {0, 4, 2, 6,
				 1, 5, 130, 3,
				 248, 248, 7, 7,
				 242, 242,
				 12, 81, 10, 121,
				 14, 159, 9, 224, 13,
				 225, 11, 229, 15, -1};
// for terminals with less than 16 colors...
static цяло color_numbers_8[28] = {0, 4, 2, 6,
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
    static цяло
lookup_color(цяло idx, цяло foreground, цяло *boldp)
{
    цяло		color = color_numbers_16[idx];
    char_u	*p;

    // Use the _16 table to check if it's a valid color name.
    ако (color < 0)
	върни -1;

    ако (t_colors == 8)
    {
	// t_Co is 8: use the 8 colors table
#ако defined(__QNXNTO__)
	// On qnx, the 8 & 16 color arrays are the same
	ако (STRNCMP(T_NAME, "qansi", 5) == 0)
	    color = color_numbers_16[idx];
	иначе
#endif
	    color = color_numbers_8[idx];
	ако (foreground)
	{
	    // set/reset bold attribute to get light foreground
	    // colors (on some terminals, e.g. "linux")
	    ако (color & 8)
		*boldp = TRUE;
	    иначе
		*boldp = FALSE;
	}
	color &= 7;	// truncate to 8 colors
    }
    иначе ако (t_colors == 16 || t_colors == 88
					   || t_colors >= 256)
    {
	// Guess: if the termcap entry ends in 'm', it is
	// probably an xterm-like terminal.  Use the changed
	// order for colors.
	ако (*T_CAF != NUL)
	    p = T_CAF;
	иначе
	    p = T_CSF;
	ако (*p != NUL && (t_colors > 256
			      || *(p + STRLEN(p) - 1) == 'm'))
	{
	    ако (t_colors == 88)
		color = color_numbers_88[idx];
	    иначе ако (t_colors >= 256)
		color = color_numbers_256[idx];
	    иначе
		color = color_numbers_8[idx];
	}
#ifdef FEAT_TERMRESPONSE
	ако (t_colors >= 256 && color == 15 && is_mac_terminal)
	    // Terminal.app has a bug: 15 is light grey. Use white
	    // from the color cube instead.
	    color = 231;
#endif
    }
    върни color;
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
	цяло	from_len,
	char_u	*to_hg,
	цяло	to_len,
	цяло	dodefault,
	цяло	forceit,
	цяло	init)
{
    цяло		from_id;
    цяло		to_id;
    hl_group_T	*hlgroup = NULL;

    from_id = syn_check_group(from_hg, from_len);
    ако (STRNCMP(to_hg, "NONE", 4) == 0)
	to_id = 0;
    иначе
	to_id = syn_check_group(to_hg, to_len);

    ако (from_id > 0)
    {
	hlgroup = &HL_TABLE()[from_id - 1];
	ако (dodefault && (forceit || hlgroup->sg_deflink == 0))
	{
	    hlgroup->sg_deflink = to_id;
#ifdef FEAT_EVAL
	    hlgroup->sg_deflink_sctx = current_sctx;
	    hlgroup->sg_deflink_sctx.sc_lnum += SOURCING_LNUM;
#endif
	}
    }

    ако (from_id > 0 && (!init || hlgroup->sg_set == 0))
    {
	// Don't allow a link when there already is some highlighting
	// for the group, unless '!' is used
	ако (to_id > 0 && !forceit && !init
		&& hl_has_settings(from_id - 1, dodefault))
	{
	    ако (SOURCING_NAME == NULL && !dodefault)
		emsg(_(e_group_has_settings_highlight_link_ignored));
	}
	иначе ако (hlgroup->sg_link != to_id
#ifdef FEAT_EVAL
		|| hlgroup->sg_script_ctx.sc_sid != current_sctx.sc_sid
#endif
		|| hlgroup->sg_cleared)
	{
	    ако (!init)
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
    цяло		idx;

#ifdef FEAT_GUI
    // First, we do not destroy the old values, but allocate the new
    // ones and update the display. THEN we destroy the old values.
    // If we destroy the old values first, then the old values
    // (such as GuiFont's or GuiFontset's) will still be displayed but
    // invalid because they were free'd.
    ако (gui.in_use)
    {
# ifdef FEAT_BEVAL_TIP
	gui_init_tooltip_font();
# endif
# ако defined(FEAT_MENU) && defined(FEAT_GUI_MOTIF)
	gui_init_menu_font();
# endif
    }
# ако defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_X11)
    gui_mch_def_colors();
# endif
# ifdef FEAT_GUI_MSWIN
    ако (gui.in_use)
	gui_mch_set_titlebar_colors();
# endif
# ifdef FEAT_GUI_X11
#  ifdef FEAT_MENU

    // This only needs to be done when there is no Menu highlight
    // group defined by default, which IS currently the case.
    gui_mch_new_menu_colors();
#  endif
    ако (gui.in_use)
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
    за (idx = 0; idx < highlight_ga.ga_len; ++idx)
	highlight_clear(idx);
    init_highlight(TRUE, TRUE);
#ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ако (USE_24BIT)
	highlight_gui_started();
    иначе
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
    static цяло
highlight_set_termgui_attr(цяло idx, char_u *key, char_u *arg, цяло init)
{
    цяло		attr;
    size_t	off;
    keyvalue_T	target;
    keyvalue_T	*entry;

    attr = 0;
    off = 0;
    target.key = 0;
    target.value.length = 0;	// not used, see cmp_keyvalue_value_ni()
    докато (arg[off] != NUL)
    {
	target.value.string = arg + off;
	entry = (keyvalue_T *)bsearch(&target, &highlight_tab,
		ARRAY_LENGTH(highlight_tab), sizeof(highlight_tab[0]),
		cmp_keyvalue_value_ni);
	ако (entry == NULL)
	{
	    semsg(_(e_illegal_value_str), arg);
	    върни FALSE;
	}

	attr |= entry->key;
	off += entry->value.length;
	ако (arg[off] == ',')		// another one follows
	    ++off;
    }
    ако (*key == 'T')
    {
	ако (!init || !(HL_TABLE()[idx].sg_set & SG_TERM))
	{
	    ако (!init)
		HL_TABLE()[idx].sg_set |= SG_TERM;
	    HL_TABLE()[idx].sg_term = attr;
	}
    }
    иначе ако (*key == 'C')
    {
	ако (!init || !(HL_TABLE()[idx].sg_set & SG_CTERM))
	{
	    ако (!init)
		HL_TABLE()[idx].sg_set |= SG_CTERM;
	    HL_TABLE()[idx].sg_cterm = attr;
	    HL_TABLE()[idx].sg_cterm_bold = FALSE;
	}
    }
#ако defined(FEAT_GUI) || defined(FEAT_EVAL)
    иначе
    {
	ако (!init || !(HL_TABLE()[idx].sg_set & SG_GUI))
	{
	    ако (!init)
		HL_TABLE()[idx].sg_set |= SG_GUI;
	    HL_TABLE()[idx].sg_gui = attr;
	}
    }
#endif

    върни TRUE;
}

#ifdef FEAT_GUI
/*
 * Set the font for the highlight group at 'idx'.
 * 'arg' is the font name.
 * Returns TRUE if the font is changed.
 */
    static цяло
highlight_set_font(
	цяло	idx,
	char_u	*arg,
	цяло	is_normal_group,
	цяло	is_menu_group,
	цяло	is_tooltip_group)
{
    цяло		did_change = FALSE;

    // in non-GUI fonts are simply ignored
    ако (HL_TABLE()[idx].sg_font_name != NULL
	    && STRCMP(HL_TABLE()[idx].sg_font_name, arg) == 0)
    {
	// Font name didn't change, ignore.
    }
    иначе ако (!gui.shell_created)
    {
	// GUI not started yet, always accept the name.
	vim_free(HL_TABLE()[idx].sg_font_name);
	HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	did_change = TRUE;
    }
    иначе
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
	ако (HL_TABLE()[idx].sg_fontset != NOFONTSET)
	{
	    // New fontset was accepted. Free the old one, if there
	    // was one.
	    gui_mch_free_fontset(temp_sg_fontset);
	    did_change = TRUE;
	}
	иначе
	    HL_TABLE()[idx].sg_fontset = temp_sg_fontset;
# endif
	ако (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    // New font was accepted. Free the old one, if there was
	    // one.
	    gui_mch_free_font(temp_sg_font);
	    did_change = TRUE;
	}
	иначе
	    HL_TABLE()[idx].sg_font = temp_sg_font;
	// Update sg_font_name once here, after both font and fontset results
	// are known.
	ако (did_change)
	{
	    vim_free(HL_TABLE()[idx].sg_font_name);
	    HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	}
    }

    върни did_change;
}
#endif

/*
 * Set the cterm foreground color for the Normal highlight group to "color" and
 * the bold attribute to "bold".
 */
    static void
hl_set_ctermfg_normal_group(цяло color, цяло bold)
{
    cterm_normal_fg_color = color + 1;
    cterm_normal_fg_bold = bold;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    ако (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	ако (termcap_active && color >= 0)
	    term_fg_color(color);
    }
}

/*
 * Set the cterm foreground color for the highlight group at 'idx' to 'color'.
 */
    static void
highlight_set_ctermfg(цяло idx, цяло color, цяло is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_fg = color + 1;
    ако (is_normal_group)
	hl_set_ctermfg_normal_group(color,
					(HL_TABLE()[idx].sg_cterm & HL_BOLD));
}

/*
 * Set the cterm background color for the Normal highlight group to "color".
 */
    static void
hl_set_ctermbg_normal_group(цяло color)
{
    cterm_normal_bg_color = color + 1;
#ifdef FEAT_GUI
    // Don't mess with 'background' if the GUI is used.
    ако (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	ако (color >= 0)
	{
	    цяло dark = -1;

	    ако (termcap_active)
		term_bg_color(color);
	    ако (t_colors < 16)
		dark = (color == 0 || color == 4);
	    // Limit the heuristic to the standard 16 colors
	    иначе ако (color < 16)
		dark = (color < 7 || color == 8);
	    // Set the 'background' option if the value is
	    // wrong.
	    ако (dark != -1
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
highlight_set_ctermbg(цяло idx, цяло color, цяло is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_bg = color + 1;
    ако (is_normal_group)
	hl_set_ctermbg_normal_group(color);
}

/*
 * Set the cterm underline color for the Normal highlight group to "color".
 */
    static void
hl_set_ctermul_normal_group(цяло color)
{
    cterm_normal_ul_color = color + 1;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    ако (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	ако (termcap_active && color >= 0)
	    term_ul_color(color);
    }
}

/*
 * Set the cterm underline color for the highlight group at 'idx' to 'color'.
 */
    static void
highlight_set_ctermul(цяло idx, цяло color, цяло is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_ul = color + 1;
    ако (is_normal_group)
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
    static цяло
highlight_set_cterm_font(
	цяло	idx,
	char_u	*arg,
	цяло	init)
{
    цяло		font;

    ако (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	върни FALSE;

    ако (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    ако (VIM_ISDIGIT(*arg))
	font = atoi((char *)arg);
    иначе ако (STRICMP(arg, "NONE") == 0)
	font = -1;
    иначе
	върни FALSE;

    HL_TABLE()[idx].sg_cterm_font = font + 1;
    върни TRUE;
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
    static цяло
highlight_set_cterm_color(
	цяло	idx,
	char_u	*key,
	char_u	*key_start,
	char_u	*arg,
	цяло	is_normal_group,
	цяло	init)
{
    цяло		color;

    ако (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	върни FALSE;

    ако (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    // When setting the foreground color, and previously the "bold"
    // flag was set for a light color, reset it now
    ако (key[5] == 'F' && HL_TABLE()[idx].sg_cterm_bold)
    {
	HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
	HL_TABLE()[idx].sg_cterm_bold = FALSE;
    }

    ако (VIM_ISDIGIT(*arg))
	color = atoi((char *)arg);
    иначе ако (STRICMP(arg, "fg") == 0)
    {
	ако (cterm_normal_fg_color)
	    color = cterm_normal_fg_color - 1;
	иначе
	{
	    emsg(_(e_fg_color_unknown));
	    върни FALSE;
	}
    }
    иначе ако (STRICMP(arg, "bg") == 0)
    {
	ако (cterm_normal_bg_color > 0)
	    color = cterm_normal_bg_color - 1;
	иначе
	{
	    emsg(_(e_bg_color_unknown));
	    върни FALSE;
	}
    }
    иначе ако (STRICMP(arg, "ul") == 0)
    {
	ако (cterm_normal_ul_color > 0)
	    color = cterm_normal_ul_color - 1;
	иначе
	{
	    emsg(_(e_ul_color_unknown));
	    върни FALSE;
	}
    }
    иначе
    {
	цяло bold = MAYBE;
	keyvalue_T target;
	keyvalue_T *entry;

	target.key = 0;
	target.value.string = arg;
	target.value.length = 0;	// not used, see cmp_keyvalue_value_i()
	entry = (keyvalue_T *)bsearch(&target, &color_name_tab,
		ARRAY_LENGTH(color_name_tab), sizeof(color_name_tab[0]),
		cmp_keyvalue_value_i);
	ако (entry == NULL)
	{
	    semsg(_(e_color_name_or_number_not_recognized_str), key_start);
	    върни FALSE;
	}

	color = lookup_color(entry->key, key[5] == 'F', &bold);

	// set/reset bold attribute to get light foreground
	// colors (on some terminals, e.g. "linux")
	ако (bold == TRUE)
	{
	    HL_TABLE()[idx].sg_cterm |= HL_BOLD;
	    HL_TABLE()[idx].sg_cterm_bold = TRUE;
	}
	иначе ако (bold == FALSE)
	    HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
    }

    // Add one to the argument, to avoid zero.  Zero is used for
    // "NONE", then "color" is -1.
    ако (key[5] == 'F')
	highlight_set_ctermfg(idx, color, is_normal_group);
    иначе ако (key[5] == 'B')
	highlight_set_ctermbg(idx, color, is_normal_group);
    иначе // ctermul
	highlight_set_ctermul(idx, color, is_normal_group);

    върни TRUE;
}

#ако defined(FEAT_GUI) || defined(FEAT_EVAL)
/*
 * Set the GUI foreground color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    static цяло
highlight_set_guifg(
	цяло	idx,
	char_u	*arg,
	цяло	is_menu_group UNUSED,
	цяло	is_scrollbar_group UNUSED,
	цяло	is_tooltip_group UNUSED,
	цяло	is_titlebar_group UNUSED,
	цяло	is_titlebarnc_group UNUSED,
	цяло	*do_colors UNUSED,
	цяло	init)
{
# ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    long	i;
# endif
    char_u	**namep;
    цяло		did_change = FALSE;

    ако (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	върни FALSE;

    namep = &HL_TABLE()[idx].sg_gui_fg_name;
    ако (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guifg colors are only used when recognized
    i = color_name2handle(arg);
    ако (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_fg = i;
# endif
	ако (*namep == NULL || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ако (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    иначе
		*namep = NULL;
	    did_change = TRUE;
	}
# ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	ако (is_titlebar_group && gui.title_fg_pixel != i)
	{
	    gui.title_fg_pixel = i;
	    *do_colors = TRUE;
	}
	ако (is_titlebarnc_group && gui.titlenc_fg_pixel != i)
	{
	    gui.titlenc_fg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	ако (is_menu_group && gui.menu_fg_pixel != i)
	{
	    gui.menu_fg_pixel = i;
	    *do_colors = TRUE;
	}
	ако (is_scrollbar_group && gui.scroll_fg_pixel != i)
	{
	    gui.scroll_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	ако (is_tooltip_group && gui.tooltip_fg_pixel != i)
	{
	    gui.tooltip_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    върни did_change;
}

/*
 * Set the GUI background color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    static цяло
highlight_set_guibg(
	цяло	idx,
	char_u	*arg,
	цяло	is_menu_group UNUSED,
	цяло	is_scrollbar_group UNUSED,
	цяло	is_tooltip_group UNUSED,
	цяло	is_titlebar_group UNUSED,
	цяло	is_titlebarnc_group UNUSED,
	цяло	*do_colors UNUSED,
	цяло	init)
{
# ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    цяло		i;
# endif
    char_u	**namep;
    цяло		did_change = FALSE;

    ако (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	върни FALSE;

    namep = &HL_TABLE()[idx].sg_gui_bg_name;
    ако (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guibg colors are only used when recognized
    i = color_name2handle(arg);
    ако (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_bg = i;
# endif
	ако (*namep == NULL || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ако (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    иначе
		*namep = NULL;
	    did_change = TRUE;
	}
# ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	ако (is_titlebar_group && gui.title_bg_pixel != i)
	{
	    gui.title_bg_pixel = i;
	    *do_colors = TRUE;
	}
	ако (is_titlebarnc_group && gui.titlenc_bg_pixel != i)
	{
	    gui.titlenc_bg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	ако (is_menu_group && gui.menu_bg_pixel != i)
	{
	    gui.menu_bg_pixel = i;
	    *do_colors = TRUE;
	}
	ако (is_scrollbar_group && gui.scroll_bg_pixel != i)
	{
	    gui.scroll_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	ако (is_tooltip_group && gui.tooltip_bg_pixel != i)
	{
	    gui.tooltip_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    върни did_change;
}

/*
 * Set the GUI undercurl/strikethrough color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    static цяло
highlight_set_guisp(цяло idx, char_u *arg, цяло init)
{
# ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    цяло		i;
# endif
    цяло		did_change = FALSE;
    char_u	**namep;

    ако (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	върни FALSE;

    namep = &HL_TABLE()[idx].sg_gui_sp_name;
    ако (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guisp colors are only used when recognized
    i = color_name2handle(arg);
    ако (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_sp = i;
# endif
	ако (*namep == NULL || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ако (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    иначе
		*namep = NULL;
	    did_change = TRUE;
	}
# ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    }
# endif

    върни did_change;
}
#endif

/*
 * Set the start/stop terminal codes for a highlight group.
 * Returns TRUE if the terminal code is set.
 */
    static цяло
highlight_set_startstop_termcode(цяло idx, char_u *key, char_u *arg, цяло init)
{
    цяло		off;
    char_u	buf[100];
    цяло		len;
    char_u	*tname;
    char_u	*p;

    ако (!init)
	HL_TABLE()[idx].sg_set |= SG_TERM;

    // The "start" and "stop"  arguments can be a literal escape
    // sequence, or a comma separated list of terminal codes.
    ако (STRNCMP(arg, "t_", 2) == 0)
    {
	off = 0;
	buf[0] = 0;
	докато (arg[off] != NUL)
	{
	    // Isolate one termcap name
	    за (len = 0; arg[off + len] &&
		    arg[off + len] != ','; ++len)
		;
	    tname = vim_strnsave(arg + off, len);
	    ако (tname == NULL)		// out of memory
		върни FALSE;
	    // lookup the escape sequence for the item
	    p = get_term_code(tname);
	    vim_free(tname);
	    ако (p == NULL)	    // ignore non-existing things
		p = (char_u *)"";

	    // Append it to the already found stuff
	    ако ((цяло)(STRLEN(buf) + STRLEN(p)) >= 99)
	    {
		semsg(_(e_terminal_code_too_long_str), arg);
		върни FALSE;
	    }
	    STRCAT(buf, p);

	    // Advance to the next item
	    off += len;
	    ако (arg[off] == ',')	    // another one follows
		++off;
	}
    }
    иначе
    {
	// Copy characters from arg[] to buf[], translating <> codes.
	за (p = arg, off = 0; off < 100 - 6 && *p; )
	{
	    len = trans_special(&p, buf + off, FSK_SIMPLIFY, FALSE, NULL);
	    ако (len > 0)	    // recognized special char
		off += len;
	    иначе		    // copy as normal char
		buf[off++] = *p++;
	}
	buf[off] = NUL;
    }

    ако (STRCMP(buf, "NONE") == 0)	// resetting the value
	p = NULL;
    иначе
	p = vim_strsave(buf);
    ако (key[2] == 'A')
    {
	vim_free(HL_TABLE()[idx].sg_start);
	HL_TABLE()[idx].sg_start = p;
    }
    иначе
    {
	vim_free(HL_TABLE()[idx].sg_stop);
	HL_TABLE()[idx].sg_stop = p;
    }
    върни TRUE;
}

/*
 * Handle the ":highlight .." command.
 * When using ":hi clear" this is called recursively for each group with
 * "forceit" and "init" both TRUE.
 */
    void
do_highlight(
    char_u	*line,
    цяло		forceit,
    цяло		init)	    // TRUE when called for initializing
{
    char_u	*name_end;
    char_u	*linep;
    char_u	*key_start;
    char_u	*arg_start;
    char_u	*key = NULL, *arg = NULL;
    long	i;
    цяло		id;
    цяло		idx;
    hl_group_T	item_before;
    цяло		did_change = FALSE;
    цяло		dodefault = FALSE;
    цяло		doclear = FALSE;
    цяло		dolink = FALSE;
    цяло		error = FALSE;
    цяло		is_normal_group = FALSE;	// "Normal" group
#ifdef FEAT_GUI_MSWIN
    цяло		is_titlebar_group = FALSE;	// "TitleBar" group
    цяло		is_titlebarnc_group = FALSE;	// "TitleBarNC" group
#иначе
# define is_titlebar_group 0
# define is_titlebarnc_group 0
#endif
#ifdef FEAT_GUI_X11
    цяло		is_menu_group = FALSE;		// "Menu" group
    цяло		is_scrollbar_group = FALSE;	// "Scrollbar" group
    цяло		is_tooltip_group = FALSE;	// "Tooltip" group
#иначе
# define is_menu_group 0
# define is_tooltip_group 0
# define is_scrollbar_group 0
#endif
#ако defined(FEAT_GUI) || defined(FEAT_EVAL)
    цяло		do_colors = FALSE;		// need to update colors?
#endif
#ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    цяло		did_highlight_changed = FALSE;
#endif

    // If no argument, list current highlighting.
    ако (!init && ends_excmd2(line - 1, line))
    {
	за (i = 1; i <= highlight_ga.ga_len && !got_int; ++i)
	    // TODO: only call when the group has attributes set
	    highlight_list_one((цяло)i);
	върни;
    }

    // Isolate the name.
    name_end = skiptowhite(line);
    linep = skipwhite(name_end);

    // Check for "default" argument.
    ако (STRNCMP(line, "default", name_end - line) == 0)
    {
	dodefault = TRUE;
	line = linep;
	name_end = skiptowhite(line);
	linep = skipwhite(name_end);
    }

    // Check for "clear" or "link" argument.
    ако (STRNCMP(line, "clear", name_end - line) == 0)
	doclear = TRUE;
    ако (STRNCMP(line, "link", name_end - line) == 0)
	dolink = TRUE;

    // ":highlight {group-name}": list highlighting for one group.
    ако (!doclear && !dolink && ends_excmd2(line, linep))
    {
	id = syn_namen2id(line, (цяло)(name_end - line));
	ако (id == 0)
	    semsg(_(e_highlight_group_name_not_found_str), line);
	иначе
	    highlight_list_one(id);
	върни;
    }

    // Handle ":highlight link {from} {to}" command.
    ако (dolink)
    {
	char_u	    *from_start = linep;
	char_u	    *from_end;
	цяло	    from_len;
	char_u	    *to_start;
	char_u	    *to_end;
	цяло	    to_len;

	from_end = skiptowhite(from_start);
	to_start = skipwhite(from_end);
	to_end	 = skiptowhite(to_start);

	ако (ends_excmd2(line, from_start) || ends_excmd2(line, to_start))
	{
	    semsg(_(e_not_enough_arguments_highlight_link_str), from_start);
	    върни;
	}

	ако (!ends_excmd2(line, skipwhite(to_end)))
	{
	    semsg(_(e_too_many_arguments_highlight_link_str), from_start);
	    върни;
	}

	from_len = (цяло)(from_end - from_start);
	to_len = (цяло)(to_end - to_start);
	highlight_group_link(from_start, from_len, to_start, to_len,
						dodefault, forceit, init);
	върни;
    }

    ако (doclear)
    {
	// ":highlight clear [group]" command.
	ако (ends_excmd2(line, linep))
	{
	    // ":highlight clear" without group name
	    highlight_reset_all();
	    върни;
	}
	line = linep;
	name_end = skiptowhite(line);
	linep = skipwhite(name_end);
    }

    // Find the group name in the table.  If it does not exist yet, add it.
    id = syn_check_group(line, (цяло)(name_end - line));
    ако (id == 0)	// failed (out of memory)
	върни;
    idx = id - 1;	// index is ID minus one

    // Return if "default" was used and the group already has settings.
    ако (dodefault && hl_has_settings(idx, TRUE))
	върни;

    // Make a copy so we can check if any attribute actually changed.
    item_before = HL_TABLE()[idx];

    ако (STRCMP(HL_TABLE()[idx].sg_name_u, "NORMAL") == 0)
	is_normal_group = TRUE;
#ifdef FEAT_GUI_MSWIN
    иначе ако (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBAR") == 0)
	is_titlebar_group = TRUE;
    иначе ако (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBARNC") == 0)
	is_titlebarnc_group = TRUE;
#endif
#ifdef FEAT_GUI_X11
    иначе ако (STRCMP(HL_TABLE()[idx].sg_name_u, "MENU") == 0)
	is_menu_group = TRUE;
    иначе ако (STRCMP(HL_TABLE()[idx].sg_name_u, "SCROLLBAR") == 0)
	is_scrollbar_group = TRUE;
    иначе ако (STRCMP(HL_TABLE()[idx].sg_name_u, "TOOLTIP") == 0)
	is_tooltip_group = TRUE;
#endif

    // Clear the highlighting for ":hi clear {group}" and ":hi clear".
    ако (doclear || (forceit && init))
    {
	highlight_clear(idx);
	ако (!doclear)
	    HL_TABLE()[idx].sg_set = 0;
    }

    ако (!doclear)
	докато (!ends_excmd2(line, linep))
	{
	    key_start = linep;
	    ако (*linep == '=')
	    {
		semsg(_(e_unexpected_equal_sign_str), key_start);
		error = TRUE;
		break;
	    }

	    // Note: Keep this in sync with get_highlight_group_key.

	    // Isolate the key ("term", "ctermfg", "ctermbg", "font", "guifg"
	    // or "guibg").
	    докато (*linep && !VIM_ISWHITE(*linep) && *linep != '=')
		++linep;
	    vim_free(key);
	    key = vim_strnsave_up(key_start, linep - key_start);
	    ако (key == NULL)
	    {
		error = TRUE;
		break;
	    }
	    linep = skipwhite(linep);

	    ако (STRCMP(key, "NONE") == 0)
	    {
		ако (!init || HL_TABLE()[idx].sg_set == 0)
		{
		    ако (!init)
			HL_TABLE()[idx].sg_set |= SG_TERM+SG_CTERM+SG_GUI;
		    highlight_clear(idx);
		}
		continue;
	    }

	    // Check for the equal sign.
	    ако (*linep != '=')
	    {
		semsg(_(e_missing_equal_sign_str_2), key_start);
		error = TRUE;
		break;
	    }
	    ++linep;

	    // Isolate the argument.
	    linep = skipwhite(linep);
	    ако (*linep == '\'')		// guifg='color name'
	    {
		arg_start = ++linep;
		linep = vim_strchr(linep, '\'');
		ако (linep == NULL)
		{
		    semsg(_(e_invalid_argument_str), key_start);
		    error = TRUE;
		    break;
		}
	    }
	    иначе
	    {
		arg_start = linep;
		linep = skiptowhite(linep);
	    }
	    ако (linep == arg_start)
	    {
		semsg(_(e_missing_argument_str), key_start);
		error = TRUE;
		break;
	    }
	    vim_free(arg);
	    arg = vim_strnsave(arg_start, linep - arg_start);
	    ако (arg == NULL)
	    {
		error = TRUE;
		break;
	    }
	    ако (*linep == '\'')
		++linep;

	    // Store the argument.
	    ако (STRCMP(key, "TERM") == 0
		    || STRCMP(key, "CTERM") == 0
		    || STRCMP(key, "GUI") == 0)
	    {
		ако (!highlight_set_termgui_attr(idx, key, arg, init))
		{
		    error = TRUE;
		    break;
		}
	    }
	    иначе ако (STRCMP(key, "FONT") == 0)
	    {
		// in non-GUI fonts are simply ignored
#ifdef FEAT_GUI
		ако (highlight_set_font(idx, arg, is_normal_group,
					      is_menu_group, is_tooltip_group))
		    did_change = TRUE;
#endif
	    }
	    иначе ако (STRCMP(key, "CTERMFG") == 0
		    || STRCMP(key, "CTERMBG") == 0
		    || STRCMP(key, "CTERMUL") == 0)
	    {
		ако (!highlight_set_cterm_color(idx, key, key_start, arg,
							is_normal_group, init))
		{
		    error = TRUE;
		    break;
		}
	    }
	    иначе ако (STRCMP(key, "CTERMFONT") == 0)
	    {
		ако (!highlight_set_cterm_font(idx, arg, init))
		{
		    error = TRUE;
		    break;
		}
	    }
	    иначе ако (STRCMP(key, "GUIFG") == 0)
	    {
#ако defined(FEAT_GUI) || defined(FEAT_EVAL)
		ако (highlight_set_guifg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    иначе ако (STRCMP(key, "GUIBG") == 0)
	    {
#ако defined(FEAT_GUI) || defined(FEAT_EVAL)
		ако (highlight_set_guibg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    иначе ако (STRCMP(key, "GUISP") == 0)
	    {
#ако defined(FEAT_GUI) || defined(FEAT_EVAL)
		ако (highlight_set_guisp(idx, arg, init))
		    did_change = TRUE;
#endif
	    }
	    иначе ако (STRCMP(key, "START") == 0 || STRCMP(key, "STOP") == 0)
	    {
		ако (!highlight_set_startstop_termcode(idx, key, arg, init))
		{
		    error = TRUE;
		    break;
		}
	    }
	    иначе
	    {
		semsg(_(e_illegal_argument_str_3), key_start);
		error = TRUE;
		break;
	    }
	    HL_TABLE()[idx].sg_cleared = FALSE;

	    // When highlighting has been given for a group, don't link it.
	    ако (!init || !(HL_TABLE()[idx].sg_set & SG_LINK))
		HL_TABLE()[idx].sg_link = 0;

	    // Continue with next argument.
	    linep = skipwhite(linep);
	}

    // If there is an error, and it's a new entry, remove it from the table.
    ако (error && idx == highlight_ga.ga_len)
	syn_unadd_group();
    иначе
    {
	ако (is_normal_group)
	{
	    HL_TABLE()[idx].sg_term_attr = 0;
	    HL_TABLE()[idx].sg_cterm_attr = 0;
#ifdef FEAT_GUI
	    HL_TABLE()[idx].sg_gui_attr = 0;
	    // Need to update all groups, because they might be using "bg"
	    // and/or "fg", which have been changed now.
#endif
#ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    ако (USE_24BIT)
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
	иначе ако (is_titlebar_group)
	{
	    ако (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
	иначе ако (is_titlebarnc_group)
	{
	    ако (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
#endif
#ifdef FEAT_GUI_X11
# ifdef FEAT_MENU
	иначе ако (is_menu_group)
	{
	    ако (gui.in_use && do_colors)
		gui_mch_new_menu_colors();
	}
# endif
	иначе ако (is_scrollbar_group)
	{
	    ако (gui.in_use && do_colors)
		gui_new_scrollbar_colors();
	    иначе
		set_hl_attr(idx);
	}
# ifdef FEAT_BEVAL_GUI
	иначе ако (is_tooltip_group)
	{
	    ако (gui.in_use && do_colors)
		gui_mch_new_tooltip_colors();
	}
# endif
#endif
	иначе
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
    ако ((did_change
	   || memcmp(&HL_TABLE()[idx], &item_before, sizeof(item_before)) != 0)
#ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    && !did_highlight_changed
#endif
       )
    {
	// Do not trigger a redraw when highlighting is changed while
	// redrawing.  This may happen when evaluating 'statusline' changes the
	// StatusLine group.
	ако (!updating_screen)
	    redraw_all_later(UPD_NOT_VALID);
	need_highlight_changed = TRUE;
    }
}

#ако defined(EXITFREE)
    void
free_highlight(void)
{
    цяло	    i;

    за (i = 0; i < highlight_ga.ga_len; ++i)
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
#ако defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
    // Since t_me has been set, this probably means that the user
    // wants to use this as default colors.  Need to reset default
    // background/foreground colors.
    mch_set_normal_colors();
#иначе
# ifdef VIMDLL
    ако (!gui.in_use)
    {
	mch_set_normal_colors();
	върни;
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
    static цяло
hl_has_settings(цяло idx, цяло check_link)
{
    върни HL_TABLE()[idx].sg_cleared == 0
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
highlight_clear(цяло idx)
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
#ако defined(FEAT_GUI) || defined(FEAT_EVAL)
    HL_TABLE()[idx].sg_gui = 0;
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_fg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_bg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_sp_name);
#endif
#ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

#ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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
    ако (gui.in_use)
#  endif
    {
	ако (set_group_colors((char_u *)"Normal",
				 &gui.norm_pixel, &gui.back_pixel,
				 FALSE, TRUE, FALSE))
	{
	    gui_mch_new_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  ifdef FEAT_GUI_MSWIN
	ако (set_group_colors((char_u *)"TitleBar",
				 &gui.title_fg_pixel, &gui.title_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
	ако (set_group_colors((char_u *)"TitleBarNC",
				 &gui.titlenc_fg_pixel, &gui.titlenc_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  endif
#  ifdef FEAT_GUI_X11
	ако (set_group_colors((char_u *)"Menu",
			     &gui.menu_fg_pixel, &gui.menu_bg_pixel,
			     TRUE, FALSE, FALSE))
	{
#   ifdef FEAT_MENU
	    gui_mch_new_menu_colors();
#   endif
	    set_must_redraw(UPD_CLEAR);
	}
#   ifdef FEAT_BEVAL_GUI
	ако (set_group_colors((char_u *)"Tooltip",
			     &gui.tooltip_fg_pixel, &gui.tooltip_bg_pixel,
			     FALSE, FALSE, TRUE))
	{
#    ifdef FEAT_TOOLBAR
	    gui_mch_new_tooltip_colors();
#    endif
	    set_must_redraw(UPD_CLEAR);
	}
#   endif
	ако (set_group_colors((char_u *)"Scrollbar",
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
    иначе
#  endif
    {
	цяло		idx;

	idx = syn_name2id((char_u *)"Normal") - 1;
	ако (idx >= 0)
	{
	    gui_do_one_color(idx, FALSE, FALSE);

	    // If the normal fg or bg color changed a complete redraw is
	    // required.
	    ако (cterm_normal_fg_gui_color != HL_TABLE()[idx].sg_gui_fg
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

#ако defined(FEAT_GUI)
/*
 * Set the colors for "Normal", "Menu", "TitleBar", "TitleBarNC", "Tooltip" or
 * "Scrollbar".
 */
    static цяло
set_group_colors(
    char_u	*name,
    guicolor_T	*fgp,
    guicolor_T	*bgp,
    цяло		do_menu,
    цяло		use_norm,
    цяло		do_tooltip)
{
    цяло		idx;

    idx = syn_name2id(name) - 1;
    ако (idx < 0)
	върни FALSE;

    gui_do_one_color(idx, do_menu, do_tooltip);

    ако (HL_TABLE()[idx].sg_gui_fg != INVALCOLOR)
	*fgp = HL_TABLE()[idx].sg_gui_fg;
    иначе ако (use_norm)
	*fgp = gui.def_norm_pixel;
    ако (HL_TABLE()[idx].sg_gui_bg != INVALCOLOR)
	*bgp = HL_TABLE()[idx].sg_gui_bg;
    иначе ако (use_norm)
	*bgp = gui.def_back_pixel;
    върни TRUE;
}

/*
 * Get the font of the "Normal" group.
 * Returns "" when it's not found or not set.
 */
    char_u *
hl_get_font_name(void)
{
    цяло		id;
    char_u	*s;

    id = syn_name2id((char_u *)"Normal");
    ако (id > 0)
    {
	s = HL_TABLE()[id - 1].sg_font_name;
	ако (s != NULL)
	    върни s;
    }
    върни (char_u *)"";
}

/*
 * Set font for "Normal" group.  Called by gui_mch_init_font() when a font has
 * actually chosen to be used.
 */
    void
hl_set_font_name(char_u *font_name)
{
    цяло	    id;

    id = syn_name2id((char_u *)"Normal");
    ако (id <= 0)
	върни;

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
    цяло	    id;

    ако (name == NULL)
	върни;

    id = syn_name2id((char_u *)"Normal");
    ако (id <= 0)
	върни;

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
    цяло	    id;

    ако (name == NULL)
	върни;

    id = syn_name2id((char_u *)"Normal");
    ако (id <= 0)
	върни;

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
    ако (STRCMP(name, "NONE") == 0)
	върни NOFONT;

    върни gui_mch_get_font(name, TRUE);
}

# ifdef FEAT_XFONTSET
/*
 * Return the handle for a fontset name.
 * Returns NOFONTSET when failed.
 */
    static GuiFontset
fontset_name2handle(char_u *name, цяло fixed_width)
{
    ако (STRCMP(name, "NONE") == 0)
	върни NOFONTSET;

    върни gui_mch_get_fontset(name, TRUE, fixed_width);
}
# endif

/*
 * Get the font or fontset for one highlight group.
 */
    static void
hl_do_font(
    цяло		idx,
    char_u	*arg,
    цяло		do_normal,		// set normal font
    цяло		do_menu UNUSED,		// set menu font
    цяло		do_tooltip UNUSED,	// set tooltip font
    цяло		free_font)		// free current font/fontset
{
# ifdef FEAT_XFONTSET
    // If 'guifontset' is not empty, first try using the name as a
    // fontset.  If that doesn't work, use it as a font name.
    ако (*p_guifontset != NUL
#  ifdef FONTSET_ALWAYS
	|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
	// In Motif, the Tooltip highlight group is always a fontset
	|| do_tooltip
#  endif
	    )
    {
	ако (free_font)
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
    ако (HL_TABLE()[idx].sg_fontset != NOFONTSET)
    {
	// If it worked and it's the Normal group, use it as the normal
	// fontset.  Same for the Menu group.
	ако (do_normal)
	    gui_init_font(arg, TRUE);
#  ако defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	ако (do_menu)
	{
#   ifdef FONTSET_ALWAYS
	    gui.menu_fontset = HL_TABLE()[idx].sg_fontset;
#   иначе
	    // YIKES!  This is a bug waiting to crash the program
	    gui.menu_font = HL_TABLE()[idx].sg_fontset;
#   endif
	    gui_mch_new_menu_font();
	}
#   ifdef FEAT_BEVAL_GUI
	ако (do_tooltip)
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
    иначе
# endif
    {
	ако (free_font)
	    gui_mch_free_font(HL_TABLE()[idx].sg_font);
	HL_TABLE()[idx].sg_font = font_name2handle(arg);
	// If it worked and it's the Normal group, use it as the
	// normal font.  Same for the Menu group.
	ако (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    ако (do_normal)
		gui_init_font(arg, FALSE);
# ifndef FONTSET_ALWAYS
#  ако defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	    ако (do_menu)
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

#ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return the handle for a color name.
 * Returns INVALCOLOR when failed.
 */
    guicolor_T
color_name2handle(char_u *name)
{
    ако (STRCMP(name, "NONE") == 0)
	върни INVALCOLOR;

    ако (STRICMP(name, "fg") == 0 || STRICMP(name, "foreground") == 0)
    {
# ако defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	ако (gui.in_use)
# endif
# ifdef FEAT_GUI
	    върни gui.norm_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	ако (cterm_normal_fg_gui_color != INVALCOLOR)
	    върни cterm_normal_fg_gui_color;
	// Guess that the foreground is black or white.
	върни GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "black" : "white"));
# endif
    }
    ако (STRICMP(name, "bg") == 0 || STRICMP(name, "background") == 0)
    {
# ако defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	ако (gui.in_use)
# endif
# ifdef FEAT_GUI
	    върни gui.back_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	ако (cterm_normal_bg_gui_color != INVALCOLOR)
	    върни cterm_normal_bg_gui_color;
	// Guess that the background is white or black.
	върни GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "white" : "black"));
# endif
    }

    върни GUI_GET_COLOR(name);
}

// On MS-Windows an RGB macro is available and it produces 0x00bbggrr color
// values as used by the MS-Windows GDI api.  It should be used only for
// MS-Windows GDI builds.
# ако defined(RGB) && defined(MSWIN) && !defined(FEAT_GUI)
#  undef RGB
# endif
# ifndef RGB
#  define RGB(r, g, b)	(((r)<<16) | ((g)<<8) | (b))
# endif

# ifdef VIMDLL
    static guicolor_T
gui_adjust_rgb(guicolor_T c)
{
    ако (gui.in_use)
	върни c;
    иначе
	върни ((c & 0xff) << 16) | (c & 0x00ff00) | ((c >> 16) & 0xff);
}
# иначе
#  define gui_adjust_rgb(c) (c)
# endif

    static цяло
hex_digit(цяло c)
{
    ако (SAFE_isdigit(c))
	върни c - '0';
    c = TOLOWER_ASC(c);
    ако (c >= 'a' && c <= 'f')
	върни c - 'a' + 10;
    върни 0x1ffffff;
}

    static guicolor_T
decode_hex_color(char_u *hex)
{
    guicolor_T color;

    ако (hex[0] != '#' || STRLEN(hex) != 7)
	върни INVALCOLOR;

    // Name is in "#rrggbb" format
    color = RGB(((hex_digit(hex[1]) << 4) + hex_digit(hex[2])),
		((hex_digit(hex[3]) << 4) + hex_digit(hex[4])),
		((hex_digit(hex[5]) << 4) + hex_digit(hex[6])));
    ако (color > 0xffffff)
	върни INVALCOLOR;
    върни gui_adjust_rgb(color);
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
    ако (lc_name == NULL)
	върни INVALCOLOR;

    colentry = dict_find(colornames_table, lc_name, -1);
    vim_free(lc_name);
    ако (colentry == NULL)
	върни INVALCOLOR;

    ако (colentry->di_tv.v_type == VAR_STRING)
    {
	colstr = tv_get_string_strict(&colentry->di_tv);
	ако ((STRLEN(colstr) == 7) && (*colstr == '#'))
	{
	    върни decode_hex_color(colstr);
	}
	иначе
	{
	    semsg(_(e_bad_color_string_str), colstr);
	    върни INVALCOLOR;
	}
    }

    ако (colentry->di_tv.v_type == VAR_NUMBER)
    {
	colnum = tv_get_number(&colentry->di_tv);
	върни (guicolor_T)colnum;
    }

    върни INVALCOLOR;
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
    ако (color != INVALCOLOR)
	върни color;

    target.key = 0;
    target.value.string = name;
    target.value.length = 0;	// not used, see cmp_keyvalue_value_i()
    entry = (keyvalue_T *)bsearch(&target, &rgb_tab, ARRAY_LENGTH(rgb_tab),
	    sizeof(rgb_tab[0]), cmp_keyvalue_value_i);
    ако (entry != NULL)
	върни gui_adjust_rgb((guicolor_T)entry->key);

# ако defined(FEAT_EVAL)
    /*
     * Not a traditional color. Load additional color aliases and then consult the alias table.
     */

    color = colorname2rgb(name);
    ако (color == INVALCOLOR)
    {
	load_default_colors_lists();
	color = colorname2rgb(name);
    }

    върни color;
# иначе
    върни INVALCOLOR;
# endif
}

    guicolor_T
gui_get_rgb_color_cmn(цяло r, цяло g, цяло b)
{
    guicolor_T  color = RGB(r, g, b);

    ако (color > 0xffffff)
	върни INVALCOLOR;
    върни gui_adjust_rgb(color);
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
    static цяло
get_attr_entry(garray_T *table, attrentry_T *aep)
{
    цяло		i;
    attrentry_T	*taep;
    static цяло	recursive = FALSE;

    // Init the table, in case it wasn't done yet.
    table->ga_itemsize = sizeof(attrentry_T);
    table->ga_growsize = 7;

    // Try to find an entry with the same specifications.
    за (i = 0; i < table->ga_len; ++i)
    {
	taep = &(((attrentry_T *)table->ga_data)[i]);
	ако (	   aep->ae_attr == taep->ae_attr
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

	върни i + ATTR_OFF;
    }

    ако (table->ga_len + ATTR_OFF > MAX_TYPENR)
    {
	// Running out of attribute entries!  remove all attributes, and
	// compute new ones for all groups.
	// When called recursively, we are really out of numbers.
	ако (recursive)
	{
	    emsg(_(e_too_many_different_highlighting_attributes_in_use));
	    върни 0;
	}
	recursive = TRUE;

	clear_hl_tables();

	set_must_redraw(UPD_CLEAR);

	за (i = 0; i < highlight_ga.ga_len; ++i)
	    set_hl_attr(i);

	recursive = FALSE;
    }

    // This is a new combination of colors and font, add an entry.
    ако (ga_grow(table, 1) == FAIL)
	върни 0;

    taep = &(((attrentry_T *)table->ga_data)[table->ga_len]);
    CLEAR_POINTER(taep);
    taep->ae_attr = aep->ae_attr;
#ifdef FEAT_GUI
    ако (table == &gui_attr_table)
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
    ако (table == &term_attr_table)
    {
	ако (aep->ae_u.term.start == NULL)
	    taep->ae_u.term.start = NULL;
	иначе
	    taep->ae_u.term.start = vim_strsave(aep->ae_u.term.start);
	ако (aep->ae_u.term.stop == NULL)
	    taep->ae_u.term.stop = NULL;
	иначе
	    taep->ae_u.term.stop = vim_strsave(aep->ae_u.term.stop);
    }
    иначе ако (table == &cterm_attr_table)
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
    върни (table->ga_len - 1 + ATTR_OFF);
}

#ако defined(FEAT_TERMINAL)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    цяло
get_cterm_attr_idx(цяло attr, цяло fg, цяло bg)
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
    върни get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#ако defined(FEAT_TERMINAL) && defined(FEAT_TERMGUICOLORS)
/*
 * Get an attribute index for a 'termguicolors' entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    цяло
get_tgc_attr_idx(цяло attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    ако (fg == INVALCOLOR && bg == INVALCOLOR)
    {
	// If both GUI colors are not set fall back to the cterm colors.  Helps
	// if the GUI only has an attribute, such as undercurl.
	at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
    }
    иначе
    {
	at_en.ae_u.cterm.fg_rgb = fg;
	at_en.ae_u.cterm.bg_rgb = bg;
    }
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
    върни get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#ако defined(FEAT_TERMINAL) && defined(FEAT_GUI)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    цяло
get_gui_attr_idx(цяло attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    at_en.ae_u.gui.fg_color = fg;
    at_en.ae_u.gui.bg_color = bg;
    върни get_attr_entry(&gui_attr_table, &at_en);
}
#endif

/*
 * Clear all highlight tables.
 */
    void
clear_hl_tables(void)
{
    цяло		i;
    attrentry_T	*taep;

#ifdef FEAT_GUI
    ga_clear(&gui_attr_table);
#endif
    за (i = 0; i < term_attr_table.ga_len; ++i)
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
    цяло
hl_combine_attr(цяло char_attr, цяло prim_attr)
{
    attrentry_T *char_aep = NULL;
    attrentry_T *prim_aep;
    attrentry_T new_en;

    ако (char_attr == 0)
	върни prim_attr;
    ако (char_attr <= HL_ALL && prim_attr <= HL_ALL)
	върни ATTR_COMBINE(char_attr, prim_attr);
#ifdef FEAT_GUI
    ако (gui.in_use)
    {
	ако (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	ако (char_aep != NULL)
	    new_en = *char_aep;
	иначе
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    ако (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	ако (prim_attr <= HL_ALL)
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	иначе
	{
	    prim_aep = syn_gui_attr2entry(prim_attr);
	    ако (prim_aep != NULL)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		ако (prim_aep->ae_u.gui.fg_color != INVALCOLOR)
		    new_en.ae_u.gui.fg_color = prim_aep->ae_u.gui.fg_color;
		ако (prim_aep->ae_u.gui.bg_color != INVALCOLOR)
		    new_en.ae_u.gui.bg_color = prim_aep->ae_u.gui.bg_color;
		ако (prim_aep->ae_u.gui.sp_color != INVALCOLOR)
		    new_en.ae_u.gui.sp_color = prim_aep->ae_u.gui.sp_color;
		ако (prim_aep->ae_u.gui.font != NOFONT)
		    new_en.ae_u.gui.font = prim_aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
		ако (prim_aep->ae_u.gui.fontset != NOFONTSET)
		    new_en.ae_u.gui.fontset = prim_aep->ae_u.gui.fontset;
# endif
	    }
	}
	върни get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    ако (IS_CTERM)
    {
	ако (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	ако (char_aep != NULL)
	    new_en = *char_aep;
	иначе
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    ако (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	ако (prim_attr <= HL_ALL)
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	иначе
	{
	    prim_aep = syn_cterm_attr2entry(prim_attr);
	    ако (prim_aep != NULL)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		ако (prim_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color = prim_aep->ae_u.cterm.fg_color;
		ако (prim_aep->ae_u.cterm.bg_color > 0)
		    new_en.ae_u.cterm.bg_color = prim_aep->ae_u.cterm.bg_color;
		ако (prim_aep->ae_u.cterm.ul_color > 0)
		    new_en.ae_u.cterm.ul_color = prim_aep->ae_u.cterm.ul_color;
		ако (prim_aep->ae_u.cterm.font > 0)
		    new_en.ae_u.cterm.font = prim_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		// If both fg and bg are not set fall back to cterm colors.
		// Helps for SpellBad which uses undercurl in the GUI.
		ако (COLOR_INVALID(prim_aep->ae_u.cterm.fg_rgb)
			&& COLOR_INVALID(prim_aep->ae_u.cterm.bg_rgb))
		{
		    ако (prim_aep->ae_u.cterm.fg_color > 0)
			new_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
		    ако (prim_aep->ae_u.cterm.bg_color > 0)
			new_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
		}
		иначе
		{
		    ако (prim_aep->ae_u.cterm.fg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.fg_rgb = prim_aep->ae_u.cterm.fg_rgb;
		    ако (prim_aep->ae_u.cterm.bg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.bg_rgb = prim_aep->ae_u.cterm.bg_rgb;
		}
		ако (prim_aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		    new_en.ae_u.cterm.ul_rgb = prim_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	}
	върни get_attr_entry(&cterm_attr_table, &new_en);
    }

    ако (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    ако (char_aep != NULL)
	new_en = *char_aep;
    иначе
    {
	CLEAR_FIELD(new_en);
	ако (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    ако (prim_attr <= HL_ALL)
	new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
    иначе
    {
	prim_aep = syn_term_attr2entry(prim_attr);
	ако (prim_aep != NULL)
	{
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_aep->ae_attr);
	    ако (prim_aep->ae_u.term.start != NULL)
	    {
		new_en.ae_u.term.start = prim_aep->ae_u.term.start;
		new_en.ae_u.term.stop = prim_aep->ae_u.term.stop;
	    }
	}
    }
    върни get_attr_entry(&term_attr_table, &new_en);
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
cterm_color_to_rgb(цяло color_nr)
{
    ако (color_nr < 1 || color_nr > 16)
	върни INVALCOLOR;
    върни (guicolor_T)cterm_color_16[color_nr - 1];
}
#endif

/*
 * Convert an xterm 256-color index (0-255) to an approximate RGB triple.
 * Uses the standard xterm palette: 0-15 ANSI (cterm_color_16), 16-231
 * 6x6x6 cube, 232-255 grayscale ramp.
 */
    static void
cterm_idx_to_rgb(цяло idx, цяло *r, цяло *g, цяло *b)
{
    static const цяло cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

    ако (idx < 0 || idx > 255)
    {
	*r = *g = *b = 0;
	върни;
    }
    ако (idx < 16)
    {
	long_u rgb = cterm_color_16[idx];
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
    }
    иначе ако (idx < 232)
    {
	цяло n = idx - 16;
	*r = cube[n / 36 % 6];
	*g = cube[n / 6  % 6];
	*b = cube[n      % 6];
    }
    иначе
    {
	цяло v = 8 + (idx - 232) * 10;
	*r = *g = *b = v;
    }
}

/*
 * Approximate an RGB triple to the nearest xterm 256-color index.
 * Searches the 6x6x6 cube and the grayscale ramp; returns 0-255.
 */
    static цяло
rgb_to_cterm_idx(цяло r, цяло g, цяло b)
{
    static const цяло cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };
    цяло best = 0;
    long best_d = -1;
    цяло i;

    // Search the 6x6x6 cube.
    за (i = 16; i < 232; ++i)
    {
	цяло n = i - 16;
	цяло cr = cube[n / 36 % 6];
	цяло cg = cube[n / 6  % 6];
	цяло cb = cube[n      % 6];
	long d = (cr - r) * (cr - r) + (cg - g) * (cg - g) + (cb - b) * (cb - b);
	ако (best_d < 0 || d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    // Search the grayscale ramp.
    за (i = 232; i < 256; ++i)
    {
	цяло v = 8 + (i - 232) * 10;
	long d = (v - r) * (v - r) + (v - g) * (v - g) + (v - b) * (v - b);
	ако (d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    върни best;
}

/*
 * Resolve a color to an RGB triple.  Prefer the gui RGB value (set by
 * guifg/guibg) when valid, otherwise convert the cterm 256-color index.
 * Returns true on success; false when no color is defined.
 * cterm_c is 1-based (0 means "no color").
 */
    static bool
resolve_color_to_rgb(цяло cterm_c, guicolor_T rgb UNUSED, цяло *r, цяло *g, цяло *b)
{
#ifdef FEAT_TERMGUICOLORS
    ако (!COLOR_INVALID(rgb))
    {
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
	върни true;
    }
#endif
    ако (cterm_c > 0)
    {
	cterm_idx_to_rgb(cterm_c - 1, r, g, b);
	върни true;
    }
    върни false;
}

/*
 * get a RGB fallback color from gui, cterm or default color
 */
    static guicolor_T
resolve_fallback_color(цяло cterm_c, guicolor_T rgb, guicolor_T default_rgb)
{
    цяло red, green, blue;
    ако (!resolve_color_to_rgb(cterm_c, rgb, &red, &green, &blue))
	върни default_rgb;
    иначе
	върни (red << 16) | (green << 8) | blue;
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
    ако (gui.in_use)
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
    ако (gui.in_use)
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
    static цяло
blend_cterm_colors(цяло popup_c, guicolor_T popup_rgb,
		   цяло under_c, guicolor_T under_rgb,
		   цяло default_rgb,
		   цяло blend_val)
{
    цяло pr, pg, pb, ur, ug, ub, r, g, b;

    ако (t_colors < 256)
	върни popup_c;
    ако (!resolve_color_to_rgb(popup_c, popup_rgb, &pr, &pg, &pb))
	върни under_c;
    ако (blend_val <= 0)
	върни rgb_to_cterm_idx(pr, pg, pb) + 1;
    ако (!resolve_color_to_rgb(under_c, under_rgb, &ur, &ug, &ub))
    {
	ur = (default_rgb >> 16) & 0xFF;
	ug = (default_rgb >> 8) & 0xFF;
	ub = default_rgb & 0xFF;
    }
    ако (blend_val >= 100)
	върни rgb_to_cterm_idx(ur, ug, ub) + 1;
    r = pr + (ur - pr) * blend_val / 100;
    g = pg + (ug - pg) * blend_val / 100;
    b = pb + (ub - pb) * blend_val / 100;
    върни rgb_to_cterm_idx(r, g, b) + 1;
}

#ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Blend two RGB colors based on blend value (0-100).
 * blend: 0=return first argument color, 100=return second argument color
 * If bg_color is INVALCOLOR, high blend means more visible (return INVALCOLOR).
 */
    static guicolor_T
blend_colors(guicolor_T popup_color, guicolor_T bg_color, цяло blend_val)
{
    цяло r1, g1, b1, r2, g2, b2, r, g, b;

    // CTERMCOLOR is a sentinel meaning "use the cterm color"; for blending
    // it has no real RGB so treat it like INVALCOLOR.
    ако (COLOR_INVALID(popup_color))
	върни INVALCOLOR;

    // Fully transparent: use underlying color as-is.
    ако (blend_val >= 100)
	върни bg_color;

    r1 = (popup_color >> 16) & 0xFF;
    g1 = (popup_color >> 8) & 0xFF;
    b1 = popup_color & 0xFF;

    ако (COLOR_INVALID(bg_color))
	bg_color = fallback_bg_rgb;

    r2 = (bg_color >> 16) & 0xFF;
    g2 = (bg_color >> 8) & 0xFF;
    b2 = bg_color & 0xFF;

    r = r1 + (r2 - r1) * blend_val / 100;
    g = g1 + (g2 - g1) * blend_val / 100;
    b = b1 + (b2 - b1) * blend_val / 100;

    върни (r << 16) | (g << 8) | b;
}
#endif

/*
 * Blend attributes for popup windows and menus with opacity.
 * Blends foreground and/or background colors based on blend value (0-100).
 * blend: 0 = opaque (use popup colors), 100 = transparent (use background colors)
 * blend_fg: TRUE to blend foreground color, FALSE to keep popup foreground
 */
    static цяло
hl_blend_attr_common(
    цяло		char_attr,
    цяло		popup_attr,
    цяло		blend,
    цяло		blend_fg)
{
    attrentry_T *char_aep = NULL;
    attrentry_T *popup_aep;
    attrentry_T new_en;
    attrentry_T tmp_en;

#ifdef FEAT_GUI
    ако (gui.in_use)
    {
	ако (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	ако (char_aep != NULL)
	    new_en = *char_aep;
	иначе
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    ако (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	ако (popup_attr <= HL_ALL)
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
	иначе
	    popup_aep = syn_gui_attr2entry(popup_attr);

	ако (popup_aep != NULL)
	{
	    guicolor_T popup_bg_rgb = popup_aep->ae_u.gui.bg_color;
	    ако (COLOR_INVALID(popup_bg_rgb))
		popup_bg_rgb = fallback_bg_rgb;

	    ако (blend_fg)
	    {
		// blend_fg=TRUE: fade underlying text toward popup bg.
		{
		    цяло base_fg = fallback_fg_rgb;
		    ако (char_aep != NULL
			    && char_aep->ae_u.gui.fg_color != INVALCOLOR)
			base_fg = char_aep->ae_u.gui.fg_color;
		    new_en.ae_u.gui.fg_color = blend_colors(
			    popup_bg_rgb, base_fg, blend);
		}
	    }
	    иначе
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
		ако (char_aep != NULL
			&& !COLOR_INVALID(char_aep->ae_u.gui.bg_color))
		    underlying_bg = char_aep->ae_u.gui.bg_color;
		new_en.ae_u.gui.bg_color = blend_colors(
			popup_bg_rgb,
			underlying_bg, blend);
	    }
	}
	върни get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    ако (IS_CTERM)
    {
	ако (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	ако (char_aep != NULL)
	    new_en = *char_aep;
	иначе
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    ако (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	ако (popup_attr <= HL_ALL)
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
	иначе
	    popup_aep = syn_cterm_attr2entry(popup_attr);

	ако (popup_aep != NULL)
	{
	    guicolor_T popup_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
	    // Fall back to cterm color converted to RGB when gui color is not set.
	    popup_bg_rgb = popup_aep->ae_u.cterm.bg_rgb;
	    ако (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color > 0)
		popup_bg_rgb = cterm_color_to_rgb(
			popup_aep->ae_u.cterm.bg_color);
#endif
	    // assign default color if guibg and ctermbg are not set for popup
	    ако (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color == 0)
		popup_bg_rgb = fallback_bg_rgb;

	    ако (!blend_fg)
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
		ако (popup_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color =
			popup_aep->ae_u.cterm.fg_color;
		иначе ако (cterm_normal_fg_color > 0)
		    new_en.ae_u.cterm.fg_color = cterm_normal_fg_color;
		иначе
		    // black-ish or white-ish
		    new_en.ae_u.cterm.fg_color = (*p_bg == 'l') ? 1 : 16;
		new_en.ae_u.cterm.ul_color = popup_aep->ae_u.cterm.ul_color;
		new_en.ae_u.cterm.font = popup_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		new_en.ae_u.cterm.ul_rgb = popup_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	    иначе
	    {
		// blend_fg=TRUE: fade underlying fg toward popup bg in
		// the 256-color palette.  Used when the popup is over a
		// cell rendered with cterm colors (no termguicolors RGB).
		цяло under_fg = (char_aep != NULL)
		    ? char_aep->ae_u.cterm.fg_color : 0;
		guicolor_T under_fg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		ако (char_aep != NULL)
		    under_fg_rgb = char_aep->ae_u.cterm.fg_rgb;
#endif
		new_en.ae_u.cterm.fg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_fg, under_fg_rgb, fallback_fg_rgb, blend);
	    }
	    // Approximate cterm bg by blending with the underlying bg
	    // in the 256-color palette and mapping to the nearest entry.
	    {
		цяло under_bg = (char_aep != NULL)
		    ? char_aep->ae_u.cterm.bg_color : 0;
		guicolor_T under_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		ако (char_aep != NULL)
		    under_bg_rgb = char_aep->ae_u.cterm.bg_rgb;
#endif
		new_en.ae_u.cterm.bg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_bg, under_bg_rgb, fallback_bg_rgb, blend);
	    }
#ifdef FEAT_TERMGUICOLORS
	    // Blend RGB colors for termguicolors mode.
	    {
		ако (blend_fg)
		{
		    // blend_fg=TRUE: fade underlying text toward popup bg.
		    ако (popup_bg_rgb != INVALCOLOR)
		    {
			цяло base_fg = fallback_fg_rgb;
			// CTERMCOLOR is a sentinel meaning "use the cterm
			// color"; treat it as no underlying color so it is
			// not blended in as a real near-white pixel.
			ако (char_aep != NULL
				&& !COLOR_INVALID(char_aep->ae_u.cterm.fg_rgb))
			    base_fg = char_aep->ae_u.cterm.fg_rgb;
			new_en.ae_u.cterm.fg_rgb = blend_colors(
				popup_bg_rgb, base_fg, blend);
		    }
		}
		иначе
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

		    ако (COLOR_INVALID(popup_fg)
			    && popup_aep->ae_u.cterm.fg_color > 0)
			popup_fg = cterm_color_to_rgb(
				popup_aep->ae_u.cterm.fg_color);

		    ако (!COLOR_INVALID(popup_fg))
			new_en.ae_u.cterm.fg_rgb = popup_fg;
		    иначе ако (!COLOR_INVALID(cterm_normal_fg_gui_color))
			new_en.ae_u.cterm.fg_rgb = cterm_normal_fg_gui_color;
		    иначе
			new_en.ae_u.cterm.fg_rgb = fallback_fg_rgb;
		}
		ако (popup_bg_rgb != INVALCOLOR)
		{
		    // Blend popup bg toward underlying bg
		    guicolor_T underlying_bg = fallback_bg_rgb;
		    ако (char_aep != NULL
			    && !COLOR_INVALID(char_aep->ae_u.cterm.bg_rgb))
			underlying_bg = char_aep->ae_u.cterm.bg_rgb;
		    new_en.ae_u.cterm.bg_rgb = blend_colors(
			    popup_bg_rgb, underlying_bg, blend);
		}
	    }
#endif
	}
	върни get_attr_entry(&cterm_attr_table, &new_en);
    }

    ако (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    ако (char_aep != NULL)
	new_en = *char_aep;
    иначе
    {
	CLEAR_FIELD(new_en);
	ако (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    // For term mode, no separate background color handling.
    върни get_attr_entry(&term_attr_table, &new_en);
}

/*
 * Blend attributes for popup windows with opacity.
 */
    цяло
hl_blend_attr(цяло char_attr, цяло popup_attr, цяло blend, цяло blend_fg)
{
    ако (blend >= 100 && blend_fg)
	върни char_attr;

    върни hl_blend_attr_common(char_attr, popup_attr, blend, blend_fg);
}

/*
 * Blend underlying foreground and background colors for pum space cells.
 */
    цяло
hl_pum_blend_attr(цяло char_attr, цяло popup_attr, цяло blend)
{
    върни hl_blend_attr_common(char_attr, popup_attr, blend, TRUE);
}

#ifdef FEAT_GUI
    attrentry_T *
syn_gui_attr2entry(цяло attr)
{
    attr -= ATTR_OFF;
    ако (attr >= gui_attr_table.ga_len)	    // did ":syntax clear"
	върни NULL;
    върни &(GUI_ATTR_ENTRY(attr));
}
#endif

/*
 * Get the highlight attributes (HL_BOLD etc.) from an attribute nr.
 * Only to be used when "attr" > HL_ALL.
 */
    цяло
syn_attr2attr(цяло attr)
{
    attrentry_T	*aep;

#ifdef FEAT_GUI
    ако (gui.in_use)
	aep = syn_gui_attr2entry(attr);
    иначе
#endif
	ако (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	иначе
	    aep = syn_term_attr2entry(attr);

    ако (aep == NULL)	    // highlighting not set
	върни 0;
    върни aep->ae_attr;
}


    attrentry_T *
syn_term_attr2entry(цяло attr)
{
    attr -= ATTR_OFF;
    ако (attr >= term_attr_table.ga_len)	    // did ":syntax clear"
	върни NULL;
    върни &(TERM_ATTR_ENTRY(attr));
}

    attrentry_T *
syn_cterm_attr2entry(цяло attr)
{
    attr -= ATTR_OFF;
    ако (attr >= cterm_attr_table.ga_len)	// did ":syntax clear"
	върни NULL;
    върни &(CTERM_ATTR_ENTRY(attr));
}

#define LIST_ATTR   1
#define LIST_STRING 2
#define LIST_INT    3

    static void
highlight_list_one(цяло id)
{
    hl_group_T	    *sgp;
    цяло		    didh = FALSE;

    sgp = &HL_TABLE()[id - 1];	    // index is ID minus one

    ако (message_filtered(sgp->sg_name))
	върни;

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

#ако defined(FEAT_GUI) || defined(FEAT_EVAL)
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

    ако (sgp->sg_link && !got_int)
    {
	(void)syn_list_header(didh, 9999, id);
	didh = TRUE;
	msg_puts_attr("links to", HL_ATTR(HLF_D));
	msg_putchar(' ');
	msg_outtrans(HL_TABLE()[HL_TABLE()[id - 1].sg_link - 1].sg_name);
    }

    ако (!didh)
	highlight_list_arg(id, didh, LIST_STRING, 0, (char_u *)"cleared", "");
#ifdef FEAT_EVAL
    ако (p_verbose > 0)
	last_set_msg(sgp->sg_script_ctx);
#endif
}

    static char_u*
highlight_arg_to_string(
    цяло		type,
    цяло		iarg,
    char_u	*sarg,
    char_u	*buf)
{
    ако (type == LIST_INT)
	sprintf((char *)buf, "%d", iarg - 1);
    иначе ако (type == LIST_STRING)
	върни sarg;
    иначе // type == LIST_ATTR
    {
	size_t buflen;

	buf[0] = NUL;
	buflen = 0;
	за (цяло i = 0; i < (цяло)ARRAY_LENGTH(highlight_index_tab); ++i)
	{
	    ако (iarg & highlight_index_tab[i]->key)
	    {
		ако (buflen > 0)
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
    върни buf;
}

    static цяло
highlight_list_arg(
    цяло		id,
    цяло		didh,
    цяло		type,
    цяло		iarg,
    char_u	*sarg,
    char	*name)
{
    char_u	buf[MAX_ATTR_LEN];
    char_u	*ts;

    ако (got_int)
	върни FALSE;

    ако (type == LIST_STRING ? (sarg == NULL) : (iarg == 0))
	върни didh;

    ts = highlight_arg_to_string(type, iarg, sarg, buf);

    (void)syn_list_header(didh,
	    (цяло)(vim_strsize(ts) + STRLEN(name) + 1), id);
    didh = TRUE;
    ако (!got_int)
    {
	ако (*name != NUL)
	{
	    msg_puts_attr(name, HL_ATTR(HLF_D));
	    msg_puts_attr("=", HL_ATTR(HLF_D));
	}
	msg_outtrans(ts);
    }
    върни didh;
}

#ако ((defined(FEAT_EVAL) || defined(FEAT_PRINTER))) && defined(FEAT_SYN_HL)
/*
 * Return "1" if highlight group "id" has attribute "flag".
 * Return NULL otherwise.
 */
    char_u *
highlight_has_attr(
    цяло		id,
    цяло		flag,
    цяло		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    цяло		attr;

    ако (id <= 0 || id > highlight_ga.ga_len)
	върни NULL;

# ако defined(FEAT_GUI) || defined(FEAT_EVAL)
    ако (modec == 'g')
	attr = HL_TABLE()[id - 1].sg_gui;
    иначе
# endif
    {
	ако (modec == 'c')
	    attr = HL_TABLE()[id - 1].sg_cterm;
	иначе
	    attr = HL_TABLE()[id - 1].sg_term;
    }

    ако (attr & flag)
	върни (char_u *)"1";
    върни NULL;
}
#endif

#ако defined(FEAT_SYN_HL) && defined(FEAT_EVAL)
/*
 * Return color name of highlight group "id".
 */
    char_u *
highlight_color(
    цяло		id,
    char_u	*what,	// "font", "fg", "bg", "sp", "ul", "fg#", "bg#" or "sp#"
    цяло		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    static char_u	name[20];
    цяло			n;
    цяло			fg = FALSE;
    цяло			sp = FALSE;
    цяло			ul = FALSE;
    цяло			font = FALSE;

    ако (id <= 0 || id > highlight_ga.ga_len)
	върни NULL;

    ако (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'g')
	fg = TRUE;
    иначе ако (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'o'
	     && TOLOWER_ASC(what[2]) == 'n' && TOLOWER_ASC(what[3]) == 't')
	font = TRUE;
    иначе ако (TOLOWER_ASC(what[0]) == 's' && TOLOWER_ASC(what[1]) == 'p')
	sp = TRUE;
    иначе ако (TOLOWER_ASC(what[0]) == 'u' && TOLOWER_ASC(what[1]) == 'l')
	ul = TRUE;
    иначе ако (!(TOLOWER_ASC(what[0]) == 'b' && TOLOWER_ASC(what[1]) == 'g'))
	върни NULL;
    ако (modec == 'g')
    {
# ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI
	// return font name
	ако (font)
	    върни HL_TABLE()[id - 1].sg_font_name;
#  endif

	// return #RRGGBB form (only possible when GUI is running)
	ако ((USE_24BIT) && what[2] == '#')
	{
	    guicolor_T		color;
	    long_u		rgb;
	    static char_u	buf[10];

	    ако (fg)
		color = HL_TABLE()[id - 1].sg_gui_fg;
	    иначе ако (sp)
		color = HL_TABLE()[id - 1].sg_gui_sp;
	    иначе
		color = HL_TABLE()[id - 1].sg_gui_bg;
	    ако (color == INVALCOLOR)
		върни NULL;
	    rgb = (long_u)GUI_MCH_GET_RGB(color);
	    sprintf((char *)buf, "#%02x%02x%02x",
				      (unsigned)(rgb >> 16),
				      (unsigned)(rgb >> 8) & 255,
				      (unsigned)rgb & 255);
	    върни buf;
	}
# endif
	ако (fg)
	    върни (HL_TABLE()[id - 1].sg_gui_fg_name);
	ако (sp)
	    върни (HL_TABLE()[id - 1].sg_gui_sp_name);
	върни (HL_TABLE()[id - 1].sg_gui_bg_name);
    }
    ако (sp)
	върни NULL;
    ако (modec == 'c')
    {
	ако (fg)
	    n = HL_TABLE()[id - 1].sg_cterm_fg - 1;
	иначе ако (ul)
	    n = HL_TABLE()[id - 1].sg_cterm_ul - 1;
	иначе ако (font)
	    n = HL_TABLE()[id - 1].sg_cterm_font - 1;
	иначе
	    n = HL_TABLE()[id - 1].sg_cterm_bg - 1;
	ако (n < 0)
	    върни NULL;
	sprintf((char *)name, "%d", n);
	върни name;
    }
    // term doesn't have color
    върни NULL;
}
#endif

#ако defined(FEAT_SYN_HL) \
	    && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)) \
	&& defined(FEAT_PRINTER)
/*
 * Return color name of highlight group "id" as RGB value.
 */
    long_u
highlight_gui_color_rgb(
    цяло		id,
    цяло		fg)	// TRUE = fg, FALSE = bg
{
    guicolor_T	color;

    ако (id <= 0 || id > highlight_ga.ga_len)
	върни 0L;

    ако (fg)
	color = HL_TABLE()[id - 1].sg_gui_fg;
    иначе
	color = HL_TABLE()[id - 1].sg_gui_bg;

    ако (color == INVALCOLOR)
	върни 0L;

    върни GUI_MCH_GET_RGB(color);
}
#endif

/*
 * Output the syntax list header.
 * Return TRUE when started a new line.
 */
    цяло
syn_list_header(
    цяло	    did_header,		// did header already
    цяло	    outlen,		// length of string that comes
    цяло	    id)			// highlight group id
{
    цяло	    endcol = 19;
    цяло	    newline = TRUE;
    цяло	    name_col = 0;

    ако (!did_header)
    {
	msg_putchar('\n');
	ако (got_int)
	    върни TRUE;
	msg_outtrans(HL_TABLE()[id - 1].sg_name);
	name_col = msg_col;
	endcol = 15;
    }
    иначе ако (msg_col + outlen + 1 >= cmdline_width)
    {
	msg_putchar('\n');
	ако (got_int)
	    върни TRUE;
    }
    иначе
    {
	ако (msg_col >= endcol)	// wrap around is like starting a new line
	    newline = FALSE;
    }

    ако (msg_col >= endcol)	// output at least one space
	endcol = msg_col + 1;
    ако (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    msg_advance(endcol);

    // Show "xxx" with the attributes.
    ако (!did_header)
    {
	ако (endcol == cmdline_width - 1 && endcol <= name_col)
	    msg_putchar(' ');
	msg_puts_attr("xxx", syn_id2attr(id));
	msg_putchar(' ');
    }

    върни newline;
}

/*
 * Set the attribute numbers for a highlight group.
 * Called after one of the attributes has changed.
 */
    static void
set_hl_attr(
    цяло		idx)	    // index in array
{
    attrentry_T	    at_en;
    hl_group_T	    *sgp = HL_TABLE() + idx;

    // The "Normal" group doesn't need an attribute number
    ако (sgp->sg_name_u != NULL && STRCMP(sgp->sg_name_u, "NORMAL") == 0)
	върни;

#ifdef FEAT_GUI
    // For the GUI mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    ако (sgp->sg_gui_fg == INVALCOLOR
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
    иначе
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
    ако (sgp->sg_start == NULL && sgp->sg_stop == NULL)
	sgp->sg_term_attr = sgp->sg_term;
    иначе
    {
	at_en.ae_attr = sgp->sg_term;
	at_en.ae_u.term.start = sgp->sg_start;
	at_en.ae_u.term.stop = sgp->sg_stop;
	sgp->sg_term_attr = get_attr_entry(&term_attr_table, &at_en);
    }

    // For the color term mode: If there are other than "normal"
    // highlighting attributes, need to allocate an attr number.
    ако (sgp->sg_cterm_fg == 0 && sgp->sg_cterm_bg == 0 &&
	sgp->sg_cterm_ul == 0 && sgp->sg_cterm_font == 0
#ifdef FEAT_TERMGUICOLORS
	    && sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
#endif
	    )
	sgp->sg_cterm_attr = sgp->sg_cterm;
    иначе
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
	ако (sgp->sg_cterm & (HL_UNDERLINE | HL_UNDERCURL
			   | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED))
	    at_en.ae_u.cterm.ul_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_sp);
	иначе
	    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	ако (at_en.ae_u.cterm.fg_rgb == INVALCOLOR
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
    static цяло
syn_override(цяло id)
{
    ако (overrides != NULL && overrides->arr != NULL)
	за (цяло k = 0; k < overrides->len; k++)
	    ако (overrides->arr[k].from == id)
		върни overrides->arr[k].to;
    върни id;
}

/*
 * Lookup a highlight group name by pointer and length.
 * If it is not found, 0 is returned.
 */
    static цяло
syn_name2id_len(char_u *name, цяло len)
{
    char_u	name_u[MAX_SYN_NAME + 1];
    hashitem_T	*hi;

    ако (len > MAX_SYN_NAME)
	len = MAX_SYN_NAME;
    mch_memmove(name_u, name, len);
    name_u[len] = NUL;
    vim_strup(name_u);
    ако (!highlight_ht_inited)
	върни 0;
    hi = hash_find(&highlight_ht, name_u);
    ако (HASHITEM_EMPTY(hi))
	върни 0;
    върни HI2HLNAME(hi)->hn_id;
}

/*
 * Lookup a highlight group name and return its ID.
 * If it is not found, 0 is returned.
 */
    цяло
syn_name2id(char_u *name)
{
    // Avoid using stricmp() too much, it's slow on some systems.
    върни syn_name2id_len(name, (цяло)STRLEN(name));
}

/*
 * Lookup a highlight group name and return its attributes.
 * Return zero if not found.
 */
    цяло
syn_name2attr(char_u *name)
{
    цяло id = syn_name2id(name);

    ако (id != 0)
	върни syn_id2attr(id);
    върни 0;
}

#ако defined(FEAT_EVAL)
/*
 * Return TRUE if highlight group "name" exists.
 */
    цяло
highlight_exists(char_u *name)
{
    върни (syn_name2id(name) > 0);
}

# ако defined(FEAT_SEARCH_EXTRA)
/*
 * Return the name of highlight group "id".
 * When not a valid ID return an empty string.
 */
    char_u *
syn_id2name(цяло id)
{
    ако (id <= 0 || id > highlight_ga.ga_len)
	върни (char_u *)"";
    върни HL_TABLE()[id - 1].sg_name;
}
# endif
#endif

/*
 * Like syn_name2id(), but take a pointer + length argument.
 */
    цяло
syn_namen2id(char_u *linep, цяло len)
{
    върни syn_name2id_len(linep, len);
}

/*
 * Find highlight group name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    цяло
syn_check_group(char_u *pp, цяло len)
{
    цяло	    id;
    char_u  *name;

    ако (len > MAX_SYN_NAME)
    {
	emsg(_(e_highlight_group_name_too_long));
	върни 0;
    }
    id = syn_name2id_len(pp, len);
    ако (id == 0)			// doesn't exist yet
    {
	name = vim_strnsave(pp, len);
	ако (name == NULL)
	    върни 0;
	id = syn_add_group(name);
    }
    върни id;
}

/*
 * Add new highlight group and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    static цяло
syn_add_group(char_u *name)
{
    char_u	*p;
    char_u	*name_up;

    // Check that the name is valid (ASCII letters, digits, underscores, dots, or hyphens).
    за (p = name; *p != NUL; ++p)
    {
	ако (!vim_isprintc(*p))
	{
	    emsg(_(e_unprintable_character_in_group_name));
	    vim_free(name);
	    върни 0;
	}
	иначе ако (!ASCII_ISALNUM(*p) && *p != '_' && *p != '.' && *p != '-')
	{
	    // This is an error, but since there previously was no check only
	    // give a warning.
	    msg_source(HL_ATTR(HLF_W));
	    msg(_("W18: Invalid character in group name"));
	    break;
	}
    }

    // First call for this growarray: init growing array.
    ако (highlight_ga.ga_data == NULL)
    {
	highlight_ga.ga_itemsize = sizeof(hl_group_T);
	highlight_ga.ga_growsize = 10;
	hash_init(&highlight_ht);
	highlight_ht_inited = true;
    }

    ако (highlight_ga.ga_len >= MAX_HL_ID)
    {
	emsg(_(e_too_many_highlight_and_syntax_groups));
	vim_free(name);
	върни 0;
    }

    // Make room for at least one other syntax_highlight entry.
    ако (ga_grow(&highlight_ga, 1) == FAIL)
    {
	vim_free(name);
	върни 0;
    }

    {
	hlname_T    *hn;
	цяло	    len = (цяло)STRLEN(name);

	hn = alloc(offsetof(hlname_T, hn_key) + len + 1);
	ако (hn == NULL)
	{
	    vim_free(name);
	    върни 0;
	}
	vim_strncpy(hn->hn_key, name, len);
	vim_strup(hn->hn_key);
	hn->hn_id = highlight_ga.ga_len + 1;	// ID is index plus one
	name_up = hn->hn_key;
    }

    CLEAR_POINTER(&(HL_TABLE()[highlight_ga.ga_len]));
    HL_TABLE()[highlight_ga.ga_len].sg_name = name;
    HL_TABLE()[highlight_ga.ga_len].sg_name_u = name_up;
#ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_TABLE()[highlight_ga.ga_len].sg_gui_bg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_fg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_sp = INVALCOLOR;
#endif
    hash_add(&highlight_ht, name_up, "highlight");
    ++highlight_ga.ga_len;

    върни highlight_ga.ga_len;		    // ID is index plus one
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
    ако (!HASHITEM_EMPTY(hi))
	hash_remove(&highlight_ht, hi, "highlight");
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name);
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name_u - HLNAME_KEY_OFF);
}

/*
 * Translate a group ID to highlight attributes.
 * "hl_id" must be valid: > 0, caller must check.
 */
    цяло
syn_id2attr(цяло hl_id)
{
    цяло		attr;
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    assert(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

#ifdef FEAT_GUI
    // Only use GUI attr when the GUI is being used.
    ако (gui.in_use)
	attr = sgp->sg_gui_attr;
    иначе
#endif
	ако (IS_CTERM)
	    attr = sgp->sg_cterm_attr;
	иначе
	    attr = sgp->sg_term_attr;

    върни attr;
}

#ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Get the GUI colors and attributes for a group ID.
 * NOTE: the colors will be INVALCOLOR when not set, the color otherwise.
 */
    цяло
syn_id2colors(цяло hl_id, guicolor_T *fgp, guicolor_T *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    assert(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

    *fgp = sgp->sg_gui_fg;
    *bgp = sgp->sg_gui_bg;
    върни sgp->sg_gui;
}
#endif

#ако (defined(MSWIN) \
	    && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL)) \
	    && defined(FEAT_TERMGUICOLORS)) \
	|| defined(FEAT_TERMINAL)
    void
syn_id2cterm_bg(цяло hl_id, цяло *fgp, цяло *bgp)
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
    цяло
syn_get_final_id(цяло hl_id)
{
    цяло		count;
    hl_group_T	*sgp;

    ако (hl_id > highlight_ga.ga_len || hl_id < 1)
	върни 0;			// Can be called from eval!!

    // Follow links until there is no more.
    // Look out for loops!  Break after 100 links.
    за (count = 100; --count >= 0; )
    {
	цяло tmp = hl_id;

	sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
	ако (sgp->sg_link == 0 || sgp->sg_link > highlight_ga.ga_len)
	    break;

	// This is to handle highlight groups that are overridden but are in the
	// middle of a link chain.
	hl_id = syn_override(hl_id);
	ако (tmp != hl_id)
	    continue;
	hl_id = sgp->sg_link;
    }
    върни syn_override(hl_id);
}

#ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Call this function just after the GUI has started.
 * Also called when 'termguicolors' was set, gui.in_use will be FALSE then.
 * It finds the font and color handles for the highlighting groups.
 */
    void
highlight_gui_started(void)
{
    цяло	    idx;

    // First get the colors from the "Normal" and "Menu" group, if set
    ако (USE_24BIT)
	set_normal_colors();

    за (idx = 0; idx < highlight_ga.ga_len; ++idx)
	gui_do_one_color(idx, FALSE, FALSE);

    highlight_changed();
}

    static void
gui_do_one_color(
    цяло		idx,
    цяло		do_menu UNUSED,	   // TRUE: might set the menu font
    цяло		do_tooltip UNUSED) // TRUE: might set the tooltip font
{
    цяло		didit = FALSE;

# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    ако (gui.in_use)
#  endif
	ако (HL_TABLE()[idx].sg_font_name != NULL)
	{
	    hl_do_font(idx, HL_TABLE()[idx].sg_font_name, FALSE, do_menu,
							    do_tooltip, TRUE);
	    didit = TRUE;
	}
# endif
    ако (HL_TABLE()[idx].sg_gui_fg_name != NULL)
    {
	HL_TABLE()[idx].sg_gui_fg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_fg_name);
	didit = TRUE;
    }
    ако (HL_TABLE()[idx].sg_gui_bg_name != NULL)
    {
	HL_TABLE()[idx].sg_gui_bg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_bg_name);
	didit = TRUE;
    }
    ако (HL_TABLE()[idx].sg_gui_sp_name != NULL)
    {
	HL_TABLE()[idx].sg_gui_sp =
			    color_name2handle(HL_TABLE()[idx].sg_gui_sp_name);
	didit = TRUE;
    }
    ако (didit)	// need to get a new attr number
	set_hl_attr(idx);
}
#endif

#ако defined(USER_HIGHLIGHT) && defined(FEAT_STL_OPT)
/*
 * Apply difference between User[1-9] and HLF_S to HLF_SNC, HLF_ST or HLF_STNC.
 */
    static void
combine_stl_hlt(
	цяло id,
	цяло id_S,
	цяло id_alt,
	цяло hlcnt,
	цяло i,
	цяло hlf,
	цяло *table)
{
    hl_group_T *hlt = HL_TABLE();

    ако (id_alt == 0)
    {
	CLEAR_POINTER(&hlt[hlcnt + i]);
	hlt[hlcnt + i].sg_term = highlight_attr[hlf];
	hlt[hlcnt + i].sg_cterm = highlight_attr[hlf];
# ако defined(FEAT_GUI) || defined(FEAT_EVAL)
	hlt[hlcnt + i].sg_gui = highlight_attr[hlf];
# endif
    }
    иначе
	mch_memmove(&hlt[hlcnt + i],
		    &hlt[id_alt - 1],
		    sizeof(hl_group_T));
    hlt[hlcnt + i].sg_link = 0;

    hlt[hlcnt + i].sg_term ^=
	hlt[id - 1].sg_term ^ hlt[id_S - 1].sg_term;
    ако (hlt[id - 1].sg_start != hlt[id_S - 1].sg_start)
	hlt[hlcnt + i].sg_start = hlt[id - 1].sg_start;
    ако (hlt[id - 1].sg_stop != hlt[id_S - 1].sg_stop)
	hlt[hlcnt + i].sg_stop = hlt[id - 1].sg_stop;
    hlt[hlcnt + i].sg_cterm ^=
	hlt[id - 1].sg_cterm ^ hlt[id_S - 1].sg_cterm;
    ако (hlt[id - 1].sg_cterm_fg != hlt[id_S - 1].sg_cterm_fg)
	hlt[hlcnt + i].sg_cterm_fg = hlt[id - 1].sg_cterm_fg;
    ако (hlt[id - 1].sg_cterm_bg != hlt[id_S - 1].sg_cterm_bg)
	hlt[hlcnt + i].sg_cterm_bg = hlt[id - 1].sg_cterm_bg;
    ако (hlt[id - 1].sg_cterm_font != hlt[id_S - 1].sg_cterm_font)
	hlt[hlcnt + i].sg_cterm_font = hlt[id - 1].sg_cterm_font;
# ако defined(FEAT_GUI) || defined(FEAT_EVAL)
    hlt[hlcnt + i].sg_gui ^=
	hlt[id - 1].sg_gui ^ hlt[id_S - 1].sg_gui;
# endif
# ifdef FEAT_GUI
    ако (hlt[id - 1].sg_gui_fg != hlt[id_S - 1].sg_gui_fg)
	hlt[hlcnt + i].sg_gui_fg = hlt[id - 1].sg_gui_fg;
    ако (hlt[id - 1].sg_gui_bg != hlt[id_S - 1].sg_gui_bg)
	hlt[hlcnt + i].sg_gui_bg = hlt[id - 1].sg_gui_bg;
    ако (hlt[id - 1].sg_gui_sp != hlt[id_S - 1].sg_gui_sp)
	hlt[hlcnt + i].sg_gui_sp = hlt[id - 1].sg_gui_sp;
    ако (hlt[id - 1].sg_font != hlt[id_S - 1].sg_font)
	hlt[hlcnt + i].sg_font = hlt[id - 1].sg_font;
#  ifdef FEAT_XFONTSET
    ако (hlt[id - 1].sg_fontset != hlt[id_S - 1].sg_fontset)
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
    цяло
highlight_changed(void)
{
    цяло		hlf;
    цяло		i;
    char_u	*p;
    char_u	*default_hl;
    цяло		attr;
    char_u	*end;
    цяло		id;
#ifdef USER_HIGHLIGHT
    char_u      userhl[30];  // use 30 to avoid compiler warning
# ifdef FEAT_STL_OPT
    цяло		id_S = -1;
    цяло		id_SNC = 0;
#  ifdef FEAT_TERMINAL
    цяло		id_ST = 0;
    цяло		id_STNC = 0;
#  endif
    цяло		hlcnt;
# endif
#endif
    win_T	*wp;

    need_highlight_changed = FALSE;

#ifdef FEAT_TERMINAL
    term_update_colors_all();
    term_update_hlfwin_all();
#endif

    // Clear all attributes.
    за (hlf = 0; hlf < (цяло)HLF_COUNT; ++hlf)
    {
	highlight_attr[hlf] = 0;
	highlight_attr_raw[hlf] = 0;
	highlight_ids[hlf] = 0;
    }

    default_hl = get_highlight_default();

    // First set all attributes to their default value.
    // Then use the attributes from the 'highlight' option, unless it already
    // matches the default and would repeat the same work.
    за (i = 0; i < 2; ++i)
    {
	ако (i)
	{
	    ако (default_hl != NULL && p_hl != NULL
				      && STRCMP(default_hl, p_hl) == 0)
		continue;
	    p = p_hl;
	}
	иначе
	    p = default_hl;
	ако (p == NULL)	    // just in case
	    continue;

	докато (*p)
	{
	    за (hlf = 0; hlf < (цяло)HLF_COUNT; ++hlf)
		ако (hl_flags[hlf] == *p)
		    break;
	    ++p;
	    ако (hlf == (цяло)HLF_COUNT || *p == NUL)
		върни FAIL;

	    // Allow several hl_flags to be combined, like "bu" for
	    // bold-underlined.
	    attr = 0;
	    id = 0;
	    за ( ; *p && *p != ','; ++p)	    // parse up to comma
	    {
		ако (VIM_ISWHITE(*p))		    // ignore white space
		    continue;

		ако (attr > HL_ALL)  // Combination with ':' is not allowed.
		    върни FAIL;

		// Note: Keep this in sync with expand_set_highlight().
		switch (*p)
		{
		    case 'b':	attr |= HL_BOLD;
				break;
		    case 'i':	attr |= HL_ITALIC;
				break;
		    case '-':
		    case 'n':			    // no highlighting
				break;
		    case 'r':	attr |= HL_INVERSE;
				break;
		    case 's':	attr |= HL_STANDOUT;
				break;
		    case 'u':	attr |= HL_UNDERLINE;
				break;
		    case 'c':	attr |= HL_UNDERCURL;
				break;
		    case '2':	attr |= HL_UNDERDOUBLE;
				break;
		    case 'd':	attr |= HL_UNDERDOTTED;
				break;
		    case '=':	attr |= HL_UNDERDASHED;
				break;
		    case 't':	attr |= HL_STRIKETHROUGH;
				break;
		    case ':':	++p;		    // highlight group name
				ако (attr || *p == NUL)	 // no combinations
				    върни FAIL;
				end = vim_strchr(p, ',');
				ако (end == NULL)
				    end = p + STRLEN(p);
				id = syn_check_group(p, (цяло)(end - p));
				ако (id == 0)
				    върни FAIL;
				attr = syn_id2attr(id);
				p = end - 1;
#ако defined(FEAT_STL_OPT) && defined(USER_HIGHLIGHT)
				ако (hlf == (цяло)HLF_SNC)
				    id_SNC = syn_get_final_id(id);
# ifdef FEAT_TERMINAL
				иначе ако (hlf == (цяло)HLF_ST)
				    id_ST = syn_get_final_id(id);
				иначе ако (hlf == (цяло)HLF_STNC)
				    id_STNC = syn_get_final_id(id);
# endif
				иначе ако (hlf == (цяло)HLF_S)
				    id_S = syn_get_final_id(id);
#endif
				break;
		    default:	върни FAIL;
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
	char *errmsg = update_winhighlight(wp, wp->w_p_whl);

	ако (errmsg != NULL)
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
    ако (ga_grow(&highlight_ga, 28) == FAIL)
	върни FAIL;
    hlcnt = highlight_ga.ga_len;
    ако (id_S == -1)
    {
	// Make sure id_S is always valid to simplify code below. Use the last
	// entry.
	CLEAR_POINTER(&HL_TABLE()[hlcnt + 27]);
	HL_TABLE()[hlcnt + 18].sg_term = highlight_attr[HLF_S];
	id_S = hlcnt + 19;
    }
# endif
    за (i = 0; i < 9; i++)
    {
	sprintf((char *)userhl, "User%d", i + 1);
	id = syn_name2id(userhl);
	ако (id == 0)
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
	иначе
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

    върни OK;
}

static void highlight_list(void);
static void highlight_list_two(цяло cnt, цяло attr);

// context for :highlight <group> <arg> expansion
static цяло expand_hi_synid = 0;	    // ID for highlight group being completed
static цяло expand_hi_equal_col = 0; // column where the '=' is
static цяло expand_hi_include_orig = 0;	    // whether to fill the existing current value or not
static char_u *expand_hi_curvalue = NULL;   // the existing current value
#ако defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
static dict_iterator_T expand_colornames_iter;	// iterator for looping through v:colornames
#endif

/*
 * Handle command line completion for :highlight command.
 */
    void
set_context_in_highlight_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;
    цяло		expand_group = TRUE;

    // Default: expand group names
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 0;
    include_link = 2;
    include_default = 1;

    ако (*arg == NUL)
	върни;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ако (*p == NUL)
	върни;

    // past "default" or group name
    include_default = 0;
    ако (STRNCMP("default", arg, p - arg) == 0)
    {
	arg = skipwhite(p);
	xp->xp_pattern = arg;
	p = skiptowhite(arg);
    }
    ако (*p == NUL)
	върни;

    // past group name
    include_link = 0;
    ако (arg[1] == 'i' && arg[0] == 'N')
    {
	highlight_list();
	expand_group = FALSE;
    }
    ако (STRNCMP("link", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	ако (*p != NUL)		// past first group name
	{
	    xp->xp_pattern = skipwhite(p);
	    p = skiptowhite(xp->xp_pattern);
	    include_none = 1;
	}
	expand_group = FALSE;
    }
    иначе ако (STRNCMP("clear", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	expand_group = FALSE;
    }
    ако (*p != NUL)			// past group name(s)
    {
	ако (expand_group)
	{
	    // expansion will be done in expand_highlight_group()
	    xp->xp_context = EXPAND_HIGHLIGHT_GROUP;

	    expand_hi_synid = syn_namen2id(arg, (цяло)(p - arg));

	    докато (*p != NUL)
	    {
		arg = skipwhite(p);
		p = skiptowhite(arg);
	    }

	    p = vim_strchr(arg, '=');
	    ако (p == NULL)
	    {
		// Didn't find a key=<value> pattern
		xp->xp_pattern = arg;
		expand_hi_equal_col = -1;
		expand_hi_include_orig = FALSE;
	    }
	    иначе
	    {
		// Found key=<value> pattern, record the exact location
		expand_hi_equal_col = (цяло)(p - xp->xp_line);

		// Only include the original value if the pattern is empty
		ако (*(p + 1) == NUL)
		    expand_hi_include_orig = TRUE;
		иначе
		    expand_hi_include_orig = FALSE;

		// Account for comma-separated values
		ако (STRNCMP(arg, "term=", 5) == 0 ||
			STRNCMP(arg, "cterm=", 6) == 0 ||
			STRNCMP(arg, "gui=", 4) == 0)
		{
		    char_u *comma = vim_strrchr(p + 1, ',');
		    ако (comma != NULL)
			p = comma;
		}
		xp->xp_pattern = p + 1;
	    }
	}
	иначе
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
    цяло		i;

    за (i = 10; --i >= 0; )
	highlight_list_two(i, HL_ATTR(HLF_D));
    за (i = 40; --i >= 0; )
	highlight_list_two(99, 0);
}

    static void
highlight_list_two(цяло cnt, цяло attr)
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
get_highlight_name(expand_T *xp UNUSED, цяло idx)
{
    върни get_highlight_name_ext(xp, idx, TRUE);
}

/*
 * Obtain a highlight group name.
 * When "skip_cleared" is TRUE don't return a cleared entry.
 */
    char_u *
get_highlight_name_ext(expand_T *xp UNUSED, цяло idx, цяло skip_cleared)
{
    ако (idx < 0)
	върни NULL;

    // Items are never removed from the table, skip the ones that were
    // cleared.
    ако (skip_cleared && idx < highlight_ga.ga_len && HL_TABLE()[idx].sg_cleared)
	върни (char_u *)"";

    ако (idx == highlight_ga.ga_len && include_none != 0)
	върни (char_u *)"NONE";
    ако (idx == highlight_ga.ga_len + include_none && include_default != 0)
	върни (char_u *)"default";
    ако (idx == highlight_ga.ga_len + include_none + include_default
							 && include_link != 0)
	върни (char_u *)"link";
    ако (idx == highlight_ga.ga_len + include_none + include_default + 1
							 && include_link != 0)
	върни (char_u *)"clear";
    ако (idx >= highlight_ga.ga_len)
	върни NULL;
    върни HL_TABLE()[idx].sg_name;
}

    static char_u *
get_highlight_attr_name(expand_T *xp UNUSED, цяло idx)
{
    ако (idx == 0)
    {
	// Fill with current value first
	ако (expand_hi_curvalue != NULL)
	    върни expand_hi_curvalue;
	иначе
	    върни (char_u*)"";
    }
    ако (idx < (цяло)ARRAY_LENGTH(highlight_index_tab) + 1)
    {
	char_u *value = highlight_index_tab[idx-1]->value.string;
	ако (expand_hi_curvalue != NULL && STRCMP(expand_hi_curvalue, value) == 0)
	{
	    // Already returned the current value above, just skip.
	    върни (char_u*)"";
	}
	върни value;
    }
    върни NULL;
}

    static char_u *
get_highlight_cterm_color(expand_T *xp UNUSED, цяло idx)
{
    ако (idx == 0)
    {
	// Fill with current value first
	ако (expand_hi_curvalue != NULL)
	    върни expand_hi_curvalue;
	иначе
	    върни (char_u*)"";
    }
    // See highlight_set_cterm_color()
    иначе ако (idx == 1)
	върни (char_u*)"fg";
    иначе ако (idx == 2)
	върни (char_u*)"bg";
    ако (idx < (цяло)ARRAY_LENGTH(color_name_tab) + 3)
    {
	char_u *value = color_name_tab[idx-3].value.string;
	върни value;
    }
    върни NULL;
}

#ако defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
    static char_u *
get_highlight_gui_color(expand_T *xp UNUSED, цяло idx)
{
    ако (idx == 0)
    {
	// Fill with current value first
	ако (expand_hi_curvalue != NULL)
	    върни expand_hi_curvalue;
	иначе
	    върни (char_u*)"";
    }
    // See color_name2handle()
    иначе ако (idx == 1)
	върни (char_u*)"fg";
    иначе ако (idx == 2)
	върни (char_u*)"bg";
    иначе ако (idx == 3)
	върни (char_u*)"NONE";

    // Complete from v:colornames. Don't do platform specific names for now.
    typval_T *tv_result;
    char_u *colorname = dict_iterate_next(&expand_colornames_iter, &tv_result);
    ако (colorname != NULL)
    {
	// :hi command doesn't allow space, so don't suggest any malformed items
	ако (vim_strchr(colorname, ' ') != NULL)
	    върни (char_u*)"";

	ако (expand_hi_curvalue != NULL && STRICMP(expand_hi_curvalue, colorname) == 0)
	{
	    // Already returned the current value above, just skip.
	    върни (char_u*)"";
	}
    }
    върни colorname;
}
#endif

    static char_u *
get_highlight_group_key(expand_T *xp UNUSED, цяло idx)
{
    // Note: Keep this in sync with do_highlight.
    static char *(p_hi_group_key_values[]) =
    {
	"term=",
	"start=",
	"stop=",
	"cterm=",
	"ctermfg=",
	"ctermbg=",
	"ctermul=",
	"ctermfont=",
#ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

    ако (idx < (цяло)ARRAY_LENGTH(p_hi_group_key_values))
	върни (char_u*)p_hi_group_key_values[idx];
    върни NULL;
}

/*
 * Command-line expansion for :hi {group-name} <args>...
 */
    цяло
expand_highlight_group(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	цяло	    *numMatches)
{
    ако (expand_hi_equal_col != -1)
    {
	// List the values. First fill in the current value, then if possible colors
	// or attribute names.
	char_u	    *(*expandfunc)(expand_T *, цяло) = NULL;
	цяло	    type = 0;
	hl_group_T  *sgp = NULL;
	цяло	    iarg = 0;
	char_u	    *sarg = NULL;

	цяло	    unsortedItems = -1; // don't sort by default

	ако (expand_hi_synid != 0)
	    sgp = &HL_TABLE()[expand_hi_synid - 1]; // index is ID minus one

	// Note: Keep this in sync with highlight_list_one().
	char_u	    *name_end = xp->xp_line + expand_hi_equal_col;
	ако (name_end - xp->xp_line >= 5
		&& STRNCMP(name_end - 5, " term", 5) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ако (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_term;
	    }
	}
	иначе ако (name_end - xp->xp_line >= 6
		&& STRNCMP(name_end - 6, " cterm", 6) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ако (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_cterm;
	    }
	}
#ако defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	иначе ако (name_end - xp->xp_line >= 4
		&& STRNCMP(name_end - 4, " gui", 4) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ако (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_gui;
	    }
	}
#endif
	иначе ако (name_end - xp->xp_line >= 8
		&& STRNCMP(name_end - 8, " ctermfg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ако (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_fg;
	    }
	}
	иначе ако (name_end - xp->xp_line >= 8
		&& STRNCMP(name_end - 8, " ctermbg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ако (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_bg;
	    }
	}
	иначе ако (name_end - xp->xp_line >= 8
		&& STRNCMP(name_end - 8, " ctermul", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ако (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_ul;
	    }
	}
#ако defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	иначе ако (name_end - xp->xp_line >= 6
		&& STRNCMP(name_end - 6, " guifg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ако (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_fg_name;
	    }
	}
	иначе ако (name_end - xp->xp_line >= 6
		&& STRNCMP(name_end - 6, " guibg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ако (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_bg_name;
	    }
	}
	иначе ако (name_end - xp->xp_line >= 6
		&& STRNCMP(name_end - 6, " guisp", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ако (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_sp_name;
	    }
	}
#endif

#ако defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	ако (expandfunc == get_highlight_gui_color)
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
	ако (expand_hi_include_orig)
	{
	    ако (((type == LIST_ATTR || type == LIST_INT) && iarg != 0) ||
		(type == LIST_STRING && sarg != NULL))
	    {
		// Retrieve the current value to go first in completion
		expand_hi_curvalue = highlight_arg_to_string(
			type, iarg, sarg, buf);
	    }
	}

	ако (expandfunc != NULL)
	{
	    върни ExpandGenericExt(
		    pat,
		    xp,
		    rmp,
		    matches,
		    numMatches,
		    expandfunc,
		    FALSE,
		    unsortedItems);
	}

	върни FAIL;
    }

    // List all the key names
    върни ExpandGenericExt(
	    pat,
	    xp,
	    rmp,
	    matches,
	    numMatches,
	    get_highlight_group_key,
	    FALSE,
	    -1);
}

#ако defined(FEAT_GUI)
/*
 * Free all the highlight group fonts.
 * Used when quitting for systems which need it.
 */
    void
free_highlight_fonts(void)
{
    цяло	    idx;

    за (idx = 0; idx < highlight_ga.ga_len; ++idx)
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

#ако defined(FEAT_EVAL)
/*
 * Convert each of the highlight attribute bits (bold, standout, underline,
 * etc.) set in 'hlattr' into a separate boolean item in a Dictionary with
 * the attribute name as the key.
 */
    static dict_T *
highlight_get_attr_dict(цяло hlattr)
{
    dict_T	*dict;
    цяло		i;

    dict = dict_alloc();
    ако (dict == NULL)
	върни NULL;

    за (i = 0; i < (цяло)ARRAY_LENGTH(highlight_index_tab); ++i)
    {
	ако (hlattr & highlight_index_tab[i]->key)
	{
	    dict_add_bool(dict, (char *)highlight_index_tab[i]->value.string,
		    VVAL_TRUE);
	    // don't want "inverse"/"reverse"
	    hlattr &= ~highlight_index_tab[i]->key;
	}
    }

    върни dict;
}

/*
 * Return the attributes of the highlight group at index 'hl_idx' as a
 * Dictionary. If 'resolve_link' is TRUE, then resolves the highlight group
 * links recursively.
 */
    static dict_T *
highlight_get_info(цяло hl_idx, цяло resolve_link)
{
    dict_T	*dict;
    hl_group_T	*sgp;
    dict_T	*attr_dict = NULL;
    цяло		hlgid;

    dict = dict_alloc();
    ако (dict == NULL)
	върни dict;

    sgp = &HL_TABLE()[hl_idx];
    // highlight group id is 1-based
    hlgid = hl_idx + 1;

    ако (dict_add_string(dict, "name", sgp->sg_name) == FAIL)
	goto error;
    ако (dict_add_number(dict, "id", hlgid) == FAIL)
	goto error;

    ако (resolve_link)
    {
	// resolve the highlight group link recursively
	докато (sgp->sg_link)
	{
	    hlgid = sgp->sg_link;
	    sgp = &HL_TABLE()[sgp->sg_link - 1];
	}
    }

    ако (sgp->sg_term != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_term);
	ако (attr_dict != NULL)
	{
	    ако (dict_add_dict(dict, "term", attr_dict) == FAIL)
		goto error;
	    attr_dict = NULL;
	}
    }
    ако (sgp->sg_start != NULL)
	ако (dict_add_string(dict, "start", sgp->sg_start) == FAIL)
	    goto error;
    ако (sgp->sg_stop != NULL)
	ако (dict_add_string(dict, "stop", sgp->sg_stop) == FAIL)
	    goto error;
    ако (sgp->sg_cterm != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_cterm);
	ако (attr_dict != NULL)
	{
	    ако (dict_add_dict(dict, "cterm", attr_dict) == FAIL)
		goto error;
	    attr_dict = NULL;
	}
    }
    ако (sgp->sg_cterm_fg != 0)
	ако (dict_add_string(dict, "ctermfg",
		    highlight_color(hlgid, (char_u *)"fg", 'c')) == FAIL)
	    goto error;
    ако (sgp->sg_cterm_bg != 0)
	ако (dict_add_string(dict, "ctermbg",
			highlight_color(hlgid, (char_u *)"bg", 'c')) == FAIL)
	    goto error;
    ако (sgp->sg_cterm_ul != 0)
	ако (dict_add_string(dict, "ctermul",
			highlight_color(hlgid, (char_u *)"ul", 'c')) == FAIL)
	    goto error;
    ако (sgp->sg_cterm_font != 0)
	ако (dict_add_string(dict, "ctermfont",
			highlight_color(hlgid, (char_u *)"font", 'c')) == FAIL)
	    goto error;
    ако (sgp->sg_gui != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_gui);
	ако (attr_dict != NULL)
	{
	    ако (dict_add_dict(dict, "gui", attr_dict) == FAIL)
		goto error;
	    attr_dict = NULL;
	}
    }
    ако (sgp->sg_gui_fg_name != NULL)
	ако (dict_add_string(dict, "guifg",
			highlight_color(hlgid, (char_u *)"fg", 'g')) == FAIL)
	    goto error;
    ако (sgp->sg_gui_bg_name != NULL)
	ако (dict_add_string(dict, "guibg",
			highlight_color(hlgid, (char_u *)"bg", 'g')) == FAIL)
	    goto error;
    ако (sgp->sg_gui_sp_name != NULL)
	ако (dict_add_string(dict, "guisp",
			highlight_color(hlgid, (char_u *)"sp", 'g')) == FAIL)
	    goto error;
# ifdef FEAT_GUI
    ако (sgp->sg_font_name != NULL)
	ако (dict_add_string(dict, "font", sgp->sg_font_name) == FAIL)
	    goto error;
# endif
    ако (sgp->sg_link)
    {
	char_u	*link;

	link = HL_TABLE()[sgp->sg_link - 1].sg_name;
	ако (link != NULL && dict_add_string(dict, "linksto", link) == FAIL)
	    goto error;

	ако (sgp->sg_deflink)
	    dict_add_bool(dict, "default", VVAL_TRUE);
    }
    ако (dict_len(dict) == 2)
	// If only 'name' is present, then the highlight group is cleared.
	dict_add_bool(dict, "cleared", VVAL_TRUE);

    върни dict;

error:
    dict_unref(attr_dict);
    dict_unref(dict);
    върни NULL;
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
    цяло		i;
    char_u	*hlarg = NULL;
    цяло		resolve_link = FALSE;

    ако (rettv_list_alloc(rettv) == FAIL)
	върни;

    ако (check_for_opt_string_arg(argvars, 0) == FAIL
	    || (argvars[0].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_arg(argvars, 1) == FAIL))
	върни;

    ако (argvars[0].v_type != VAR_UNKNOWN)
    {
	// highlight group name supplied
	hlarg = tv_get_string_chk(&argvars[0]);
	ако (hlarg == NULL)
	    върни;

	ако (argvars[1].v_type != VAR_UNKNOWN)
	{
	    цяло error = FALSE;

	    resolve_link = tv_get_bool_chk(&argvars[1], &error);
	    ако (error)
		върни;
	}
    }

    list = rettv->vval.v_list;
    за (i = 0; i < highlight_ga.ga_len && !got_int; ++i)
    {
	ако (hlarg == NULL || STRICMP(hlarg, HL_TABLE()[i].sg_name) == 0)
	{
	    dict = highlight_get_info(i, resolve_link);
	    ако (dict != NULL && list_append_dict(list, dict) == FAIL)
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
hldict_get_string(dict_T *dict, char_u *key, цяло *error)
{
    dictitem_T	*di;

    *error = FALSE;
    di = dict_find(dict, key, -1);
    ако (di == NULL)
	върни NULL;

    ако (di->di_tv.v_type != VAR_STRING || di->di_tv.vval.v_string == NULL)
    {
	emsg(_(e_string_required));
	*error = TRUE;
	върни NULL;
    }

    върни di->di_tv.vval.v_string;
}

/*
 * Convert the highlight attribute Dictionary at 'dict[key]' into a string
 * value in 'attr_str' of length 'len'. Returns FALSE if 'dict[key]' is not a
 * Dictionary or is NULL.
 */
    static цяло
hldict_attr_to_str(
	dict_T	*dict,
	char_u	*key,
	char_u	*attr_str,
	size_t	len)
{
    dictitem_T	*di;
    dict_T	*attrdict;
    цяло		i;
    char_u	*p;

    attr_str[0] = NUL;
    di = dict_find(dict, key, -1);
    ако (di == NULL)
	върни TRUE;

    ако (di->di_tv.v_type != VAR_DICT || di->di_tv.vval.v_dict == NULL)
    {
	emsg(_(e_dictionary_required));
	върни FALSE;
    }

    attrdict = di->di_tv.vval.v_dict;

    // If the attribute dict is empty, then return NONE to clear the attributes
    ако (dict_len(attrdict) == 0)
    {
	vim_strcat(attr_str, (char_u *)"NONE", len);
	върни TRUE;
    }

    p = attr_str;
    за (i = 0; i < (цяло)ARRAY_LENGTH(highlight_tab); ++i)
    {
	ако (dict_get_bool(attrdict, (char *)highlight_tab[i].value.string,
		    VVAL_FALSE) == VVAL_TRUE)
	{
	    ако (p != attr_str && (size_t)(p - attr_str + 2) < len)
		STRCPY(p, (char_u *)",");
	    ако (p - attr_str + highlight_tab[i].value.length + 1 < len)
	    {
		STRCPY(p, highlight_tab[i].value.string);
		p += highlight_tab[i].value.length;
	    }
	}
    }

    върни TRUE;
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
add_attr_and_value(char_u *dptr, char_u *attr, цяло attrlen, char_u *value)
{
    size_t	vallen;

    // Do nothing if the value is not specified or is empty
    ако (value == NULL || *value == NUL)
	върни dptr;

    vallen = STRLEN(value);
    // When the value contains a space and the attribute has an "=" (i.e. it
    // is a key=value pair), surround the value with single quotes so that
    // do_highlight() can parse it correctly.
    ако (vim_strchr(value, ' ') != NULL && vim_strchr(attr, '=') != NULL)
    {
	ако (dptr + attrlen + vallen + 3 < hlsetBuf + HLSETBUFSZ)
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
    иначе ако (dptr + attrlen + vallen + 1 < hlsetBuf + HLSETBUFSZ)
    {
	STRCPY(dptr, attr);
	dptr += attrlen;
	STRCPY(dptr, value);
	dptr += vallen;
    }

    върни dptr;
}

/*
 * Add or update a highlight group using 'dict' items. Returns TRUE if
 * successfully updated the highlight group.
 */
    static цяло
hlg_add_or_update(dict_T *dict)
{
    char_u	*name;
    цяло		error;
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
    цяло		forceit = FALSE;
    цяло		dodefault = FALSE;
    цяло		done = FALSE;
    char_u	*p;

    name = hldict_get_string(dict, (char_u *)"name", &error);
    ако (name == NULL || *name == NUL || error)
	върни FALSE;

    ако (dict_get_bool(dict, "force", VVAL_FALSE) == VVAL_TRUE)
	forceit = TRUE;

    ако (dict_get_bool(dict, "default", VVAL_FALSE) == VVAL_TRUE)
	dodefault = TRUE;

    ако (dict_has_key(dict, "cleared"))
    {
	varnumber_T	cleared;

	// clear a highlight group
	cleared = dict_get_bool(dict, "cleared", FALSE);
	ако (cleared == TRUE)
	{
	    vim_snprintf((char *)hlsetBuf, HLSETBUFSZ, "clear %s", name);
	    do_highlight(hlsetBuf, forceit, FALSE);
	    done = TRUE;
	}
    }

    ако (dict_has_key(dict, "linksto"))
    {
	char_u	*linksto;

	// link highlight groups
	linksto = hldict_get_string(dict, (char_u *)"linksto", &error);
	ако (linksto == NULL || *linksto == NUL || error)
	    върни FALSE;

	vim_snprintf((char *)hlsetBuf, HLSETBUFSZ, "%slink %s %s",
				dodefault ? "default " : "", name, linksto);
	do_highlight(hlsetBuf, forceit, FALSE);

	done = TRUE;
    }

    // If 'cleared' or 'linksto' are specified, then don't process the other
    // attributes.
    ако (done)
	върни TRUE;

    start = hldict_get_string(dict, (char_u *)"start", &error);
    ако (error)
	върни FALSE;

    stop = hldict_get_string(dict, (char_u *)"stop", &error);
    ако (error)
	върни FALSE;

    ако (!hldict_attr_to_str(dict, (char_u *)"term", term_attr,
							    sizeof(term_attr)))
	върни FALSE;

    ако (!hldict_attr_to_str(dict, (char_u *)"cterm", cterm_attr,
							   sizeof(cterm_attr)))
	върни FALSE;

    ctermfg = hldict_get_string(dict, (char_u *)"ctermfg", &error);
    ако (error)
	върни FALSE;

    ctermbg = hldict_get_string(dict, (char_u *)"ctermbg", &error);
    ако (error)
	върни FALSE;

    ctermul = hldict_get_string(dict, (char_u *)"ctermul", &error);
    ако (error)
	върни FALSE;

    ctermfont = hldict_get_string(dict, (char_u *)"ctermfont", &error);
    ако (error)
	върни FALSE;

    ако (!hldict_attr_to_str(dict, (char_u *)"gui", gui_attr, sizeof(gui_attr)))
	върни FALSE;

    guifg = hldict_get_string(dict, (char_u *)"guifg", &error);
    ако (error)
	върни FALSE;

    guibg = hldict_get_string(dict, (char_u *)"guibg", &error);
    ако (error)
	върни FALSE;

    guisp = hldict_get_string(dict, (char_u *)"guisp", &error);
    ако (error)
	върни FALSE;

# ifdef FEAT_GUI
    font = hldict_get_string(dict, (char_u *)"font", &error);
    ако (error)
	върни FALSE;
# endif

    // If none of the attributes are specified, then do nothing.
    ако (term_attr[0] == NUL && start == NULL && stop == NULL
	    && cterm_attr[0] == NUL && ctermfg == NULL && ctermbg == NULL
	    && ctermul == NULL && ctermfont == NULL && gui_attr[0] == NUL
# ifdef FEAT_GUI
	    && font == NULL
# endif
	    && guifg == NULL && guibg == NULL && guisp == NULL
	    )
	върни TRUE;

    hlsetBuf[0] = NUL;
    p = hlsetBuf;
    ако (dodefault)
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

    върни TRUE;
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

    ако (check_for_list_arg(argvars, 0) == FAIL)
	върни;

    FOR_ALL_LIST_ITEMS(argvars->vval.v_list, li)
    {
	ако (li->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_dictionary_required));
	    върни;
	}

	dict = li->li_tv.vval.v_dict;
	ако (!hlg_add_or_update(dict))
	    върни;
    }

    rettv->vval.v_number = 0;
}
#endif

/*
 * If "old" is in the override stack, then update it to "hl_new". Does not free
 * "old".
 */
    void
update_highlight_overrides(hl_override_T *old, hl_override_T *hl_new, цяло newlen)
{
    ако (old == NULL)
	върни;

    за (hl_overrides_T *set = overrides; set != NULL; set = set->next)
    {
	ако (set->arr == old)
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
    ако (arr == NULL || overrides == NULL)
	върни;

    за (hl_overrides_T *set = overrides; set != NULL; set = set->next)
    {
	ако (set->arr == arr)
	{
	    set->arr = NULL;
	    break;
	}
    }
}

/*
 * Update highlight_attr[] array. If "update_ids" is true, then update
 * highlight_ids[] array instead.
 */
    static void
set_highlight_attr(hl_override_T *arr, цяло len, bool update_ids)
{
    за (цяло i = 0; i < len; i++)
    {
	hl_override_T	*override = arr + i;
	цяло		hlf = -1, attr;

	ако (override->from <= 0)
	    // Directly map to highlight_attr
	    hlf = -override->from;
	иначе
	{
	    за (цяло k = 0; k < HLF_COUNT; k++)
		ако (override->from == highlight_ids[k])
		{
		    hlf = k;
		    break;
		}

	    ако (hlf == -1)
		continue;
	}

	ако (update_ids)
	{
	    ако (override->to <= 0)
	    {
		ако (hlf == -override->to)
		    highlight_ids[hlf] = -1;
		иначе
		    highlight_ids[hlf] = highlight_ids[-override->to];
	    }
	    иначе
		highlight_ids[hlf] = override->to;
	}
	иначе
	{
	    ако (override->to <= 0)
		// Directly use highlight_attr[]
		attr = highlight_attr[-override->to];
	    иначе
		attr = syn_id2attr(override->to);

	    highlight_attr[hlf] = attr;
	}
    }
}

/*
 * Set the current highlight override to "arr". Returns true if successful
 */
    bool
push_highlight_overrides(hl_override_T *arr, цяло len)
{
    // Don't want to do anything if "arr" is already the current override. If
    // "arr" is NULL (but overrides->arr is not), then still push an override,
    // but "->arr" will just be NULL so any previous overrides are cleared.
    ако (overrides != NULL && overrides->arr == arr)
	върни false;

    hl_overrides_T *set;

    set = ALLOC_ONE(hl_overrides_T);
    ако (set == NULL)
	върни false;

    set->arr = arr;
    set->len = len;

    // Push to front of list
    set->next = overrides;
    overrides = set;

    // Save current highlight_attr[], so it can be restored later after this
    // override is popped.
    memcpy(set->attr, highlight_attr, sizeof(highlight_attr));

    // Reset highlight_attr[] to default values, as we don't want changes from
    // other overrides.
    memcpy(highlight_attr, highlight_attr_raw, sizeof(highlight_attr));

    // Update highlight_attr[] array
    ако (arr != NULL)
	set_highlight_attr(arr, len, false);

    върни true;
}

/*
 * Pop the current highlight override (if any) from the stack. This should
 * always be paired with a *successful* push.
 */
    void
pop_highlight_overrides(void)
{
    hl_overrides_T *set = overrides;

    ако (overrides == NULL)
	върни;

    overrides = set->next;

    // Set highlight_attr[] to state before override was pushed.
    memcpy(highlight_attr, set->attr, sizeof(highlight_attr));
    vim_free(set);
}

/*
 * Parse the 'winhighlight' option and return array. Returns NULL on failure or
 * if empty option. If failure, then errmsg is set.
 */
    static hl_override_T *
parse_winhighlight(char_u *opt, цяло *len, char **errmsg)
{
    char_u	    *p = opt;
    hl_override_T   *arr;
    цяло		    i = 0;
    цяло		    num = 1;
    цяло		    n_colons = 0;

    ако (*p == NUL)
	върни NULL;

    // Get number of overrides first so we can allocate array
    докато ((p = vim_strchr(p, ',')) != NULL)
    {
	p++;
	num++;
    }
    p = opt;
    // Check if number of ':' matches number of ','
    докато ((p = vim_strchr(p, ':')) != NULL)
    {
	p++;
	n_colons++;
    }

    ако (num != n_colons)
    {
	*errmsg = e_invalid_argument;
	върни NULL;
    }

    arr = ALLOC_MULT(hl_override_T, num);
    ако (arr == NULL)
    {
	*errmsg = e_out_of_memory;
	върни NULL;
    }

    p = opt;

    докато (true)
    {
	hl_override_T *override = arr + i++;
	char_u	*fromname = p, *toname;
	char_u	*tmp;
	char_u	**names[2] = {&fromname, &toname};
	цяло	fromlen, tolen;
	цяло	*lens[2] = {&fromlen, &tolen};
	цяло	fromid, toid;
	цяло	*ids[2] = {&fromid, &toid};

	p = vim_strchr(p, ':');

	ако (p == NULL)
	    goto fail;

	fromlen = p - fromname; // Get hl for "from"
	ако (fromlen == 0)
	    goto fail;
	p++; // Skip colon ':'
	ако (*p == NUL)
	    goto fail;

	toname = p;
	tmp = vim_strchr(p, ',');

	// Get hl for "to", must check for no trailing comma in case last
	// element.
	ако (tmp == NULL)
	    tolen = (цяло)STRLEN(p);
	иначе
	{
	    tolen = tmp - toname;
	    p = ++tmp;
	}
	ако (tolen == 0)
	    goto fail;

	за (цяло k = 0; k < 2; k++)
	{
	    char_u  *name = *names[k];
	    цяло	    nlen = *lens[k];

	    ако (*name == '!')
	    {
		// If starts with "!", then it maps directly to a 'highlight'
		// occasion.
		цяло hlf;

		ако (nlen != 2)
		    goto fail;

		за (hlf = 0; hlf < (цяло)HLF_COUNT; ++hlf)
		    ако (hl_flags[hlf] == name[1])
			break;

		ако (hlf >= HLF_COUNT)
		    goto fail;

		*ids[k] = -hlf;
	    }
	    иначе
	    {
		// Otherwise get the highlight group id.
		ако (syn_check_group(name, nlen) == 0)
		    goto fail;

		*ids[k] = syn_namen2id(name, nlen);
		ако (*ids[k] == 0)
		    goto fail;

		// Always map "Normal" group to HLF_WIN
		ако (ids[k] == &fromid &&
			STRCMP(HL_TABLE()[*ids[k] - 1].sg_name_u, "NORMAL") == 0)
		    *ids[k] = -HLF_WIN;
	    }
	}

	// If fromid == toid, leave it be, this is so that we know HLF_WIN has
	// been set (even to itself e.g. "Normal"), and therefore know to use
	// its attr instead.
	override->from = fromid;
	override->to = toid;

	ако (tmp == NULL)
	    break;
    }

    *len = num;
    върни arr;
fail:
    vim_free(arr);
    *errmsg = e_invalid_argument;
    върни NULL;
}

/*
 * Update w_hl for "wp" using given option value. Returns error
 * message on failure, otherwise NULL.
 */
    char *
update_winhighlight(win_T *wp, char_u *opt)
{
    char	    *err = NULL;
    hl_override_T   *arr;
    цяло		    num = 1;

    arr = parse_winhighlight(opt, &num, &err);

    ако (arr == NULL && err != NULL)
	върни err;

    ако (arr == NULL && wp->w_hl == NULL)
	върни NULL;

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
    ако (wp->w_buffer != NULL)
    {
	// Make sure terminal highlighting is updated
	bool ret = push_highlight_overrides(wp->w_hl, wp->w_hl_len);

	ако (wp->w_buffer->b_term != NULL)
	    term_init_default_colors(wp->w_buffer->b_term);
	ако (ret)
	    pop_highlight_overrides();
    }
#endif

    върни NULL;
}

/*
 * Get id that the hlf attr is using in window "wp", otherwise zero. Note that
 * -1 may be returned if the hlf was set to itself.
 */
    цяло
hlf_get_id(win_T *wp, цяло hlf)
{
    цяло id;
    static цяло prev[HLF_COUNT];

    ако (wp->w_hl == NULL)
	върни highlight_ids[hlf];

    memcpy(prev, highlight_ids, sizeof(highlight_ids));
    set_highlight_attr(wp->w_hl, wp->w_hl_len, true);
    id = highlight_ids[hlf];
    memcpy(highlight_ids, prev, sizeof(highlight_ids));

    върни id;
}

/*
 * Update 'winhighlight' using a 'wincolor' style option value. Returns error
 * message on failure otherwise NULL. Does not update the actual 'wincolor'
 * value, but does update the 'winhighlight' value.
 */
    char *
update_wincolor(win_T *wp, char_u *opt)
{
    char_u  *str = *opt == NUL ? "" : alloc(sizeof("!(:") + STRLEN(opt));
    char    *errmsg;

    ако (str == NULL)
	върни e_out_of_memory;

    ако (*opt != NUL)
	sprintf((char *)str, "!(:%s", opt);

    errmsg = update_winhighlight(wp, str);

    ако (errmsg == NULL)
	set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		str, OPT_FREE|OPT_LOCAL, 0);

    ако (*opt != NUL)
	free(str);
    върни errmsg;
}
