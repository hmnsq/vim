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

#شامل "vim.h"

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
ایستا keyvalue_T highlight_tab[] = {
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
ایستا keyvalue_T *highlight_index_tab[] = {
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

شمارش {
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
ایستا keyvalue_T color_name_tab[] = {
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
تعریفنوع ساختار
{
    char_u	*sg_name;	// highlight group name
    char_u	*sg_name_u;	// uppercase of sg_name
    صحیح		sg_cleared;	// "hi clear" was used
// for normal terminals
    صحیح		sg_term;	// "term=" highlighting attributes
    char_u	*sg_start;	// terminal string for start highl
    char_u	*sg_stop;	// terminal string for stop highl
    صحیح		sg_term_attr;	// Screen attr for term mode
// for color terminals
    صحیح		sg_cterm;	// "cterm=" highlighting attr
    صحیح		sg_cterm_bold;	// bold attr was set for light color
    صحیح		sg_cterm_fg;	// terminal fg color number + 1
    صحیح		sg_cterm_bg;	// terminal bg color number + 1
    صحیح		sg_cterm_ul;	// terminal ul color number + 1
    صحیح		sg_cterm_attr;	// Screen attr for color term mode
    صحیح		sg_cterm_font;	// terminal alternative font (0 for normal)
// for when using the GUI
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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
    صحیح		sg_gui_attr;    // Screen attr for GUI mode
#endif
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
// Store the sp color name for the GUI or synIDattr()
    صحیح		sg_gui;		// "gui=" highlighting attributes
    char_u	*sg_gui_fg_name;// GUI foreground color name
    char_u	*sg_gui_bg_name;// GUI background color name
    char_u	*sg_gui_sp_name;// GUI special color name
#endif
    صحیح		sg_link;	// link to this highlight group ID
    صحیح		sg_deflink;	// default link; restored in highlight_clear()
    صحیح		sg_set;		// combination of SG_* flags
#ifdef FEAT_EVAL
    sctx_T	sg_deflink_sctx;  // script where the default link was set
    sctx_T	sg_script_ctx;	// script in which the group was last set
#endif
} hl_group_T;

تعریفنوع ساختار hl_overrides_S hl_overrides_T;
ساختار hl_overrides_S
{
    hl_override_T   *arr;   // May be NULL if "arr" was freed or no highlight
			    // overrides (all values set to default)
    صحیح		    len;
    hl_overrides_T  *next;  // Used to handle recursive calls

    صحیح attr[HLF_شمار];    // highlight_attr[] before being updated.
};

ایستا hl_overrides_T *overrides = پوچ;

// Synced with highlight_attr, each is the highlight group id for each attr. If
// an element is 0, then there is no attr for it. If an element is -1, then it
// means it was set to itself.
ایستا صحیح highlight_ids[HLF_شمار];

// Same as highlight_attr[] but is not modified by highlight group overrides
ایستا صحیح highlight_attr_raw[HLF_شمار];

ایستا صحیح hl_flags[HLF_شمار] = HL_FLAGS;

// highlight groups for 'highlight' option
ایستا garray_T highlight_ga;
#define HL_TABLE()	((hl_group_T *)((highlight_ga.ga_data)))

// Wrapper struct for hashtable entries: stores the highlight group ID alongside
// the uppercase name used as a hash key.  Uses the same offsetof pattern as
// signgroup_T / HI2SG.
تعریفنوع ساختار {
    صحیح		hn_id;		// highlight group ID (1-based)
    char_u	hn_کلید[1];	// uppercase name (stretchable array)
} hlname_T;

#define HLNAME_کلید_OFF	offsetof(hlname_T, hn_کلید)
#define HI2HLNAME(hi)	((hlname_T *)((hi)->hi_کلید - HLNAME_کلید_OFF))

// hashtable for quick highlight group name lookup
ایستا hashtab_T highlight_ht;
ایستا bool	 highlight_ht_inited = false;

/*
 * An attribute number is the index in attr_table plus ATTR_OFF.
 */
#define ATTR_OFF (HL_ALL + 1)

ایستا تهی syn_unadd_group(تهی);
ایستا تهی set_hl_attr(صحیح idx);
ایستا تهی highlight_list_one(صحیح id);
ایستا صحیح highlight_list_arg(صحیح id, صحیح didh, صحیح type, صحیح iarg, char_u *sarg, نویسه *name);
ایستا صحیح syn_add_group(char_u *name);
ایستا صحیح syn_name2id_len(char_u *name, صحیح len);
ایستا صحیح hl_has_settings(صحیح idx, صحیح check_link);
ایستا تهی highlight_clear(صحیح idx);

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
ایستا تهی gui_do_one_color(صحیح idx, صحیح do_menu, صحیح do_tooltip);
#endif
#ifdef FEAT_GUI
ایستا صحیح  set_group_colors(char_u *name, guicolor_T *fgp, guicolor_T *bgp, صحیح do_menu, صحیح use_norm, صحیح do_tooltip);
ایستا تهی hl_do_font(صحیح idx, char_u *arg, صحیح do_normal, صحیح do_menu, صحیح do_tooltip, صحیح free_font);
#endif

/*
 * The default highlight groups.  These are compiled-in for fast startup and
 * they still work when the runtime files can't be found.
 * When making changes here, also change runtime/colors/default.vim!
 * The #ifdefs are needed to reduce the amount of static data.  Helps to make
 * the 16 bit DOS (museum) version compile.
 */
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
# define CENT(a, b) b
#وگرنه
# define CENT(a, b) a
#endif
ایستا نویسه *(highlight_init_both[]) = {
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
    پوچ
};

// Default colors only used with a light background.
ایستا نویسه *(highlight_init_light[]) = {
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
    پوچ
};

// Default colors only used with a dark background.
ایستا نویسه *(highlight_init_dark[]) = {
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
    پوچ
};

#اگر defined(FEAT_SYN_HL)
/*
 * Returns the number of highlight groups.
 */
    صحیح
highlight_رقم_groups(تهی)
{
    بازگشت highlight_ga.ga_len;
}

/*
 * Returns the name of a highlight group.
 */
    char_u *
highlight_group_name(صحیح id)
{
    بازگشت HL_TABLE()[id].sg_name;
}

/*
 * Returns the ID of the link to a highlight group.
 */
    صحیح
highlight_link_id(صحیح id)
{
    بازگشت HL_TABLE()[id].sg_link;
}
#endif

ایستا تهی resolve_fallback_fg_to_rgb(تهی);
ایستا تهی resolve_fallback_bg_to_rgb(تهی);

    تهی
init_highlight(
    صحیح		both,	    // include groups where 'bg' doesn't matter
    صحیح		reset)	    // clear group first
{
    صحیح		i;
    نویسه	**pp;
    ایستا صحیح	had_both = FALSE;
#ifdef FEAT_EVAL
    char_u	*p;

    // Try finding the color scheme file.  Used when a color file was loaded
    // and 'background' or 't_Co' is changed.
    p = get_var_مقدار((char_u *)"g:colors_name");
    اگر (p != پوچ)
    {
	// The value of g:colors_name could be freed when sourcing the script,
	// making "p" invalid, so copy it.
	char_u *copy_p = vim_strsave(p);
	صحیح    r;

	اگر (copy_p != پوچ)
	{
	    r = load_colors(copy_p);
	    vim_free(copy_p);
	    اگر (r == OK)
		بازگشت;
	}
    }

#endif

    // Didn't use a color file, use the compiled-in colors.
    اگر (both)
    {
	had_both = TRUE;
	pp = highlight_init_both;
	برای (i = 0; pp[i] != پوچ; ++i)
	    do_highlight((char_u *)pp[i], reset, TRUE);
    }
    وگرنه اگر (!had_both)
	// Don't do anything before the call with both == TRUE from main().
	// Not everything has been setup then, and that call will overrule
	// everything anyway.
	بازگشت;

    اگر (*p_bg == 'l')
	pp = highlight_init_light;
    وگرنه
	pp = highlight_init_dark;
    برای (i = 0; pp[i] != پوچ; ++i)
	do_highlight((char_u *)pp[i], reset, TRUE);

    // Reverse looks ugly, but grey may not work for less than 8 colors.  Thus
    // let it depend on the number of colors available.
    اگر (t_colors < 8)
	do_highlight((char_u *)"Visual term=reverse cterm=reverse ctermbg=NONE ctermfg=NONE",
		FALSE, TRUE);
    // With 8 colors brown is equal to yellow, need to use black for Search fg
    // to avoid Statement highlighted text disappears.
    // Clear the attributes, needed when changing the t_Co value.
    اگر (t_colors <= 8)
    {
	اگر (*p_bg == 'l')
	    do_highlight((char_u *)"Search ctermfg=black", FALSE, TRUE);
    }

#ifdef FEAT_SYN_HL
    // If syntax highlighting is enabled load the highlighting for it.
    اگر (get_var_مقدار((char_u *)"g:syntax_on") != پوچ)
    {
	ایستا صحیح	recursive = 0;

	اگر (recursive >= 5)
	    emsg(_(e_recursive_loop_loading_syncolor_vim));
	وگرنه
	{
	    ++recursive;
	    (تهی)source_runtime((char_u *)"syntax/syncolor.vim", DIP_ALL);
	    --recursive;
	}
    }
#endif
    resolve_fallback_fg_to_rgb();
    resolve_fallback_bg_to_rgb();
}

#اگر defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
/*
 * Load a default color list. Intended to support legacy color names but allows
 * the user to override the color values. Only loaded once.
 */
    ایستا تهی
load_default_colors_lists(تهی)
{
    // Lacking a default color list isn't the end of the world but it is likely
    // an inconvenience so users should know when it is missing.
    اگر (source_runtime((char_u *)"colors/lists/default.vim", DIP_ALL) != OK)
	پیام("failed to load colors/lists/default.vim");
}
#endif

/*
 * Load color file "name".
 * Return OK for success, FAIL for failure.
 */
    صحیح
load_colors(char_u *name)
{
    char_u	*buf;
    صحیح		retval = FAIL;
    ایستا صحیح	recursive = FALSE;

    // When being called recursively, this is probably because setting
    // 'background' caused the highlighting to be reloaded.  This means it is
    // working, thus we should return OK.
    اگر (recursive)
	بازگشت OK;

    recursive = TRUE;
    buf = alloc(STRLEN(name) + 12);
    اگر (buf != پوچ)
    {
#اگر defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	load_default_colors_lists();
#endif
	apply_autocmds(EVENT_COLORSCHEMEPRE, name,
					       curbuf->b_fname, FALSE, curbuf);
	sprintf((نویسه *)buf, "colors/%s.vim", name);
	retval = source_runtime(buf, DIP_START + DIP_OPT);
	vim_free(buf);
	اگر (retval == OK)
	    apply_autocmds(EVENT_COLORSCHEME, name, curbuf->b_fname,
								FALSE, curbuf);
    }
    recursive = FALSE;

    بازگشت retval;
}

ایستا صحیح color_numbers_16[28] = {0, 1, 2, 3,
				 4, 5, 6, 6,
				 7, 7, 7, 7,
				 8, 8,
				 9, 9, 10, 10,
				 11, 11, 12, 12, 13,
				 13, 14, 14, 15, -1};
// for xterm with 88 colors...
ایستا صحیح color_numbers_88[28] = {0, 4, 2, 6,
				 1, 5, 32, 72,
				 84, 84, 7, 7,
				 82, 82,
				 12, 43, 10, 61,
				 14, 63, 9, 74, 13,
				 75, 11, 78, 15, -1};
// for xterm with 256 colors...
ایستا صحیح color_numbers_256[28] = {0, 4, 2, 6,
				 1, 5, 130, 3,
				 248, 248, 7, 7,
				 242, 242,
				 12, 81, 10, 121,
				 14, 159, 9, 224, 13,
				 225, 11, 229, 15, -1};
// for terminals with less than 16 colors...
ایستا صحیح color_numbers_8[28] = {0, 4, 2, 6,
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
    ایستا صحیح
lookup_color(صحیح idx, صحیح foreground, صحیح *boldp)
{
    صحیح		color = color_numbers_16[idx];
    char_u	*p;

    // Use the _16 table to check if it's a valid color name.
    اگر (color < 0)
	بازگشت -1;

    اگر (t_colors == 8)
    {
	// t_Co is 8: use the 8 colors table
#اگر defined(__QNXNTO__)
	// On qnx, the 8 & 16 color arrays are the same
	اگر (STRNCMP(T_NAME, "qansi", 5) == 0)
	    color = color_numbers_16[idx];
	وگرنه
#endif
	    color = color_numbers_8[idx];
	اگر (foreground)
	{
	    // set/reset bold attribute to get light foreground
	    // colors (on some terminals, e.g. "linux")
	    اگر (color & 8)
		*boldp = TRUE;
	    وگرنه
		*boldp = FALSE;
	}
	color &= 7;	// truncate to 8 colors
    }
    وگرنه اگر (t_colors == 16 || t_colors == 88
					   || t_colors >= 256)
    {
	// Guess: if the termcap entry ends in 'm', it is
	// probably an xterm-like terminal.  Use the changed
	// order for colors.
	اگر (*T_CAF != NUL)
	    p = T_CAF;
	وگرنه
	    p = T_CSF;
	اگر (*p != NUL && (t_colors > 256
			      || *(p + STRLEN(p) - 1) == 'm'))
	{
	    اگر (t_colors == 88)
		color = color_numbers_88[idx];
	    وگرنه اگر (t_colors >= 256)
		color = color_numbers_256[idx];
	    وگرنه
		color = color_numbers_8[idx];
	}
#ifdef FEAT_TERMRESPONSE
	اگر (t_colors >= 256 && color == 15 && is_mac_terminal)
	    // Terminal.app has a bug: 15 is light grey. Use white
	    // from the color cube instead.
	    color = 231;
#endif
    }
    بازگشت color;
}

/*
 * Link highlight group 'from_hg' to 'to_hg'.
 * 'dodefault' is set to TRUE for ":highlight default link".
 * 'forceit' is set to TRUE for ":highlight! link"
 * 'init' is set to TRUE when initializing all the highlight groups.
 */
    ایستا تهی
highlight_group_link(
	char_u	*from_hg,
	صحیح	from_len,
	char_u	*to_hg,
	صحیح	to_len,
	صحیح	dodefault,
	صحیح	forceit,
	صحیح	init)
{
    صحیح		from_id;
    صحیح		to_id;
    hl_group_T	*hlgroup = پوچ;

    from_id = syn_check_group(from_hg, from_len);
    اگر (STRNCMP(to_hg, "NONE", 4) == 0)
	to_id = 0;
    وگرنه
	to_id = syn_check_group(to_hg, to_len);

    اگر (from_id > 0)
    {
	hlgroup = &HL_TABLE()[from_id - 1];
	اگر (dodefault && (forceit || hlgroup->sg_deflink == 0))
	{
	    hlgroup->sg_deflink = to_id;
#ifdef FEAT_EVAL
	    hlgroup->sg_deflink_sctx = current_sctx;
	    hlgroup->sg_deflink_sctx.sc_lnum += SOURCING_LNUM;
#endif
	}
    }

    اگر (from_id > 0 && (!init || hlgroup->sg_set == 0))
    {
	// Don't allow a link when there already is some highlighting
	// for the group, unless '!' is used
	اگر (to_id > 0 && !forceit && !init
		&& hl_has_settings(from_id - 1, dodefault))
	{
	    اگر (SOURCING_NAME == پوچ && !dodefault)
		emsg(_(e_group_has_settings_highlight_link_ignored));
	}
	وگرنه اگر (hlgroup->sg_link != to_id
#ifdef FEAT_EVAL
		|| hlgroup->sg_script_ctx.sc_sid != current_sctx.sc_sid
#endif
		|| hlgroup->sg_cleared)
	{
	    اگر (!init)
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
    ایستا تهی
highlight_reset_all(تهی)
{
    صحیح		idx;

#ifdef FEAT_GUI
    // First, we do not destroy the old values, but allocate the new
    // ones and update the display. THEN we destroy the old values.
    // If we destroy the old values first, then the old values
    // (such as GuiFont's or GuiFontset's) will still be displayed but
    // invalid because they were free'd.
    اگر (gui.in_use)
    {
# ifdef FEAT_BEVAL_TIP
	gui_init_tooltip_font();
# endif
# اگر defined(FEAT_MENU) && defined(FEAT_GUI_MOTIF)
	gui_init_menu_font();
# endif
    }
# اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_X11)
    gui_mch_def_colors();
# endif
# ifdef FEAT_GUI_MSWIN
    اگر (gui.in_use)
	gui_mch_set_titlebar_colors();
# endif
# ifdef FEAT_GUI_X11
#  ifdef FEAT_MENU

    // This only needs to be done when there is no Menu highlight
    // group defined by default, which IS currently the case.
    gui_mch_new_menu_colors();
#  endif
    اگر (gui.in_use)
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
    برای (idx = 0; idx < highlight_ga.ga_len; ++idx)
	highlight_clear(idx);
    init_highlight(TRUE, TRUE);
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    اگر (USE_24BIT)
	highlight_gui_started();
    وگرنه
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
    ایستا صحیح
highlight_set_termgui_attr(صحیح idx, char_u *کلید, char_u *arg, صحیح init)
{
    صحیح		attr;
    size_t	off;
    keyvalue_T	target;
    keyvalue_T	*entry;

    attr = 0;
    off = 0;
    target.کلید = 0;
    target.مقدار.length = 0;	// not used, see cmp_keyvalue_value_ni()
    وقتی (arg[off] != NUL)
    {
	target.مقدار.string = arg + off;
	entry = (keyvalue_T *)bsearch(&target, &highlight_tab,
		ARRAY_LENGTH(highlight_tab), اندازه(highlight_tab[0]),
		cmp_keyvalue_مقدار_ni);
	اگر (entry == پوچ)
	{
	    semsg(_(e_illegal_مقدار_str), arg);
	    بازگشت FALSE;
	}

	attr |= entry->کلید;
	off += entry->مقدار.length;
	اگر (arg[off] == ',')		// another one follows
	    ++off;
    }
    اگر (*کلید == 'T')
    {
	اگر (!init || !(HL_TABLE()[idx].sg_set & SG_TERM))
	{
	    اگر (!init)
		HL_TABLE()[idx].sg_set |= SG_TERM;
	    HL_TABLE()[idx].sg_term = attr;
	}
    }
    وگرنه اگر (*کلید == 'C')
    {
	اگر (!init || !(HL_TABLE()[idx].sg_set & SG_CTERM))
	{
	    اگر (!init)
		HL_TABLE()[idx].sg_set |= SG_CTERM;
	    HL_TABLE()[idx].sg_cterm = attr;
	    HL_TABLE()[idx].sg_cterm_bold = FALSE;
	}
    }
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
    وگرنه
    {
	اگر (!init || !(HL_TABLE()[idx].sg_set & SG_GUI))
	{
	    اگر (!init)
		HL_TABLE()[idx].sg_set |= SG_GUI;
	    HL_TABLE()[idx].sg_gui = attr;
	}
    }
#endif

    بازگشت TRUE;
}

#ifdef FEAT_GUI
/*
 * Set the font for the highlight group at 'idx'.
 * 'arg' is the font name.
 * Returns TRUE if the font is changed.
 */
    ایستا صحیح
highlight_set_font(
	صحیح	idx,
	char_u	*arg,
	صحیح	is_normal_group,
	صحیح	is_menu_group,
	صحیح	is_tooltip_group)
{
    صحیح		did_change = FALSE;

    // in non-GUI fonts are simply ignored
    اگر (HL_TABLE()[idx].sg_font_name != پوچ
	    && STRCMP(HL_TABLE()[idx].sg_font_name, arg) == 0)
    {
	// Font name didn't change, ignore.
    }
    وگرنه اگر (!gui.shell_created)
    {
	// GUI not started yet, always accept the name.
	vim_free(HL_TABLE()[idx].sg_font_name);
	HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	did_change = TRUE;
    }
    وگرنه
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
	اگر (HL_TABLE()[idx].sg_fontset != NOFONTSET)
	{
	    // New fontset was accepted. Free the old one, if there
	    // was one.
	    gui_mch_free_fontset(temp_sg_fontset);
	    did_change = TRUE;
	}
	وگرنه
	    HL_TABLE()[idx].sg_fontset = temp_sg_fontset;
# endif
	اگر (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    // New font was accepted. Free the old one, if there was
	    // one.
	    gui_mch_free_font(temp_sg_font);
	    did_change = TRUE;
	}
	وگرنه
	    HL_TABLE()[idx].sg_font = temp_sg_font;
	// Update sg_font_name once here, after both font and fontset results
	// are known.
	اگر (did_change)
	{
	    vim_free(HL_TABLE()[idx].sg_font_name);
	    HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	}
    }

    بازگشت did_change;
}
#endif

/*
 * Set the cterm foreground color for the Normal highlight group to "color" and
 * the bold attribute to "bold".
 */
    ایستا تهی
hl_set_ctermfg_normal_group(صحیح color, صحیح bold)
{
    cterm_normal_fg_color = color + 1;
    cterm_normal_fg_bold = bold;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    اگر (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	اگر (termcap_active && color >= 0)
	    term_fg_color(color);
    }
}

/*
 * Set the cterm foreground color for the highlight group at 'idx' to 'color'.
 */
    ایستا تهی
highlight_set_ctermfg(صحیح idx, صحیح color, صحیح is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_fg = color + 1;
    اگر (is_normal_group)
	hl_set_ctermfg_normal_group(color,
					(HL_TABLE()[idx].sg_cterm & HL_BOLD));
}

/*
 * Set the cterm background color for the Normal highlight group to "color".
 */
    ایستا تهی
hl_set_ctermbg_normal_group(صحیح color)
{
    cterm_normal_bg_color = color + 1;
#ifdef FEAT_GUI
    // Don't mess with 'background' if the GUI is used.
    اگر (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	اگر (color >= 0)
	{
	    صحیح dark = -1;

	    اگر (termcap_active)
		term_bg_color(color);
	    اگر (t_colors < 16)
		dark = (color == 0 || color == 4);
	    // Limit the heuristic to the standard 16 colors
	    وگرنه اگر (color < 16)
		dark = (color < 7 || color == 8);
	    // Set the 'background' option if the value is
	    // wrong.
	    اگر (dark != -1
		    && dark != (*p_bg == 'd')
		    && !option_was_set((char_u *)"bg"))
	    {
		set_option_مقدار_give_err((char_u *)"bg",
			0L, (char_u *)(dark ? "dark" : "light"), 0);
		reset_option_was_set((char_u *)"bg");
	    }
	}
    }
}

/*
 * Set the cterm background color for the highlight group at 'idx' to 'color'.
 */
    ایستا تهی
highlight_set_ctermbg(صحیح idx, صحیح color, صحیح is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_bg = color + 1;
    اگر (is_normal_group)
	hl_set_ctermbg_normal_group(color);
}

/*
 * Set the cterm underline color for the Normal highlight group to "color".
 */
    ایستا تهی
hl_set_ctermul_normal_group(صحیح color)
{
    cterm_normal_ul_color = color + 1;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    اگر (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	اگر (termcap_active && color >= 0)
	    term_ul_color(color);
    }
}

/*
 * Set the cterm underline color for the highlight group at 'idx' to 'color'.
 */
    ایستا تهی
highlight_set_ctermul(صحیح idx, صحیح color, صحیح is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_ul = color + 1;
    اگر (is_normal_group)
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
    ایستا صحیح
highlight_set_cterm_font(
	صحیح	idx,
	char_u	*arg,
	صحیح	init)
{
    صحیح		font;

    اگر (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	بازگشت FALSE;

    اگر (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    اگر (VIM_ISDIGIT(*arg))
	font = atoi((نویسه *)arg);
    وگرنه اگر (STRICMP(arg, "NONE") == 0)
	font = -1;
    وگرنه
	بازگشت FALSE;

    HL_TABLE()[idx].sg_cterm_font = font + 1;
    بازگشت TRUE;
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
    ایستا صحیح
highlight_set_cterm_color(
	صحیح	idx,
	char_u	*کلید,
	char_u	*کلید_start,
	char_u	*arg,
	صحیح	is_normal_group,
	صحیح	init)
{
    صحیح		color;

    اگر (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	بازگشت FALSE;

    اگر (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    // When setting the foreground color, and previously the "bold"
    // flag was set for a light color, reset it now
    اگر (کلید[5] == 'F' && HL_TABLE()[idx].sg_cterm_bold)
    {
	HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
	HL_TABLE()[idx].sg_cterm_bold = FALSE;
    }

    اگر (VIM_ISDIGIT(*arg))
	color = atoi((نویسه *)arg);
    وگرنه اگر (STRICMP(arg, "fg") == 0)
    {
	اگر (cterm_normal_fg_color)
	    color = cterm_normal_fg_color - 1;
	وگرنه
	{
	    emsg(_(e_fg_color_unknown));
	    بازگشت FALSE;
	}
    }
    وگرنه اگر (STRICMP(arg, "bg") == 0)
    {
	اگر (cterm_normal_bg_color > 0)
	    color = cterm_normal_bg_color - 1;
	وگرنه
	{
	    emsg(_(e_bg_color_unknown));
	    بازگشت FALSE;
	}
    }
    وگرنه اگر (STRICMP(arg, "ul") == 0)
    {
	اگر (cterm_normal_ul_color > 0)
	    color = cterm_normal_ul_color - 1;
	وگرنه
	{
	    emsg(_(e_ul_color_unknown));
	    بازگشت FALSE;
	}
    }
    وگرنه
    {
	صحیح bold = MAYBE;
	keyvalue_T target;
	keyvalue_T *entry;

	target.کلید = 0;
	target.مقدار.string = arg;
	target.مقدار.length = 0;	// not used, see cmp_keyvalue_value_i()
	entry = (keyvalue_T *)bsearch(&target, &color_name_tab,
		ARRAY_LENGTH(color_name_tab), اندازه(color_name_tab[0]),
		cmp_keyvalue_مقدار_i);
	اگر (entry == پوچ)
	{
	    semsg(_(e_color_name_or_عدد_not_recognized_str), کلید_start);
	    بازگشت FALSE;
	}

	color = lookup_color(entry->کلید, کلید[5] == 'F', &bold);

	// set/reset bold attribute to get light foreground
	// colors (on some terminals, e.g. "linux")
	اگر (bold == TRUE)
	{
	    HL_TABLE()[idx].sg_cterm |= HL_BOLD;
	    HL_TABLE()[idx].sg_cterm_bold = TRUE;
	}
	وگرنه اگر (bold == FALSE)
	    HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
    }

    // Add one to the argument, to avoid zero.  Zero is used for
    // "NONE", then "color" is -1.
    اگر (کلید[5] == 'F')
	highlight_set_ctermfg(idx, color, is_normal_group);
    وگرنه اگر (کلید[5] == 'B')
	highlight_set_ctermbg(idx, color, is_normal_group);
    وگرنه // ctermul
	highlight_set_ctermul(idx, color, is_normal_group);

    بازگشت TRUE;
}

#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
/*
 * Set the GUI foreground color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ایستا صحیح
highlight_set_guifg(
	صحیح	idx,
	char_u	*arg,
	صحیح	is_menu_group UNUSED,
	صحیح	is_scrollbar_group UNUSED,
	صحیح	is_tooltip_group UNUSED,
	صحیح	is_titlebar_group UNUSED,
	صحیح	is_titlebarnc_group UNUSED,
	صحیح	*do_colors UNUSED,
	صحیح	init)
{
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    long	i;
# endif
    char_u	**namep;
    صحیح		did_change = FALSE;

    اگر (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	بازگشت FALSE;

    namep = &HL_TABLE()[idx].sg_gui_fg_name;
    اگر (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guifg colors are only used when recognized
    i = color_name2handle(arg);
    اگر (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_fg = i;
# endif
	اگر (*namep == پوچ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    اگر (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    وگرنه
		*namep = پوچ;
	    did_change = TRUE;
	}
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	اگر (is_titlebar_group && gui.title_fg_pixel != i)
	{
	    gui.title_fg_pixel = i;
	    *do_colors = TRUE;
	}
	اگر (is_titlebarnc_group && gui.titlenc_fg_pixel != i)
	{
	    gui.titlenc_fg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	اگر (is_menu_group && gui.menu_fg_pixel != i)
	{
	    gui.menu_fg_pixel = i;
	    *do_colors = TRUE;
	}
	اگر (is_scrollbar_group && gui.scroll_fg_pixel != i)
	{
	    gui.scroll_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	اگر (is_tooltip_group && gui.tooltip_fg_pixel != i)
	{
	    gui.tooltip_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    بازگشت did_change;
}

/*
 * Set the GUI background color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ایستا صحیح
highlight_set_guibg(
	صحیح	idx,
	char_u	*arg,
	صحیح	is_menu_group UNUSED,
	صحیح	is_scrollbar_group UNUSED,
	صحیح	is_tooltip_group UNUSED,
	صحیح	is_titlebar_group UNUSED,
	صحیح	is_titlebarnc_group UNUSED,
	صحیح	*do_colors UNUSED,
	صحیح	init)
{
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    صحیح		i;
# endif
    char_u	**namep;
    صحیح		did_change = FALSE;

    اگر (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	بازگشت FALSE;

    namep = &HL_TABLE()[idx].sg_gui_bg_name;
    اگر (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guibg colors are only used when recognized
    i = color_name2handle(arg);
    اگر (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_bg = i;
# endif
	اگر (*namep == پوچ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    اگر (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    وگرنه
		*namep = پوچ;
	    did_change = TRUE;
	}
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	اگر (is_titlebar_group && gui.title_bg_pixel != i)
	{
	    gui.title_bg_pixel = i;
	    *do_colors = TRUE;
	}
	اگر (is_titlebarnc_group && gui.titlenc_bg_pixel != i)
	{
	    gui.titlenc_bg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	اگر (is_menu_group && gui.menu_bg_pixel != i)
	{
	    gui.menu_bg_pixel = i;
	    *do_colors = TRUE;
	}
	اگر (is_scrollbar_group && gui.scroll_bg_pixel != i)
	{
	    gui.scroll_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	اگر (is_tooltip_group && gui.tooltip_bg_pixel != i)
	{
	    gui.tooltip_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    بازگشت did_change;
}

/*
 * Set the GUI undercurl/strikethrough color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ایستا صحیح
highlight_set_guisp(صحیح idx, char_u *arg, صحیح init)
{
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    صحیح		i;
# endif
    صحیح		did_change = FALSE;
    char_u	**namep;

    اگر (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	بازگشت FALSE;

    namep = &HL_TABLE()[idx].sg_gui_sp_name;
    اگر (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guisp colors are only used when recognized
    i = color_name2handle(arg);
    اگر (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_sp = i;
# endif
	اگر (*namep == پوچ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    اگر (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    وگرنه
		*namep = پوچ;
	    did_change = TRUE;
	}
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    }
# endif

    بازگشت did_change;
}
#endif

/*
 * Set the start/stop terminal codes for a highlight group.
 * Returns TRUE if the terminal code is set.
 */
    ایستا صحیح
highlight_set_startstop_termcode(صحیح idx, char_u *کلید, char_u *arg, صحیح init)
{
    صحیح		off;
    char_u	buf[100];
    صحیح		len;
    char_u	*tname;
    char_u	*p;

    اگر (!init)
	HL_TABLE()[idx].sg_set |= SG_TERM;

    // The "start" and "stop"  arguments can be a literal escape
    // sequence, or a comma separated list of terminal codes.
    اگر (STRNCMP(arg, "t_", 2) == 0)
    {
	off = 0;
	buf[0] = 0;
	وقتی (arg[off] != NUL)
	{
	    // Isolate one termcap name
	    برای (len = 0; arg[off + len] &&
		    arg[off + len] != ','; ++len)
		;
	    tname = vim_strnsave(arg + off, len);
	    اگر (tname == پوچ)		// out of memory
		بازگشت FALSE;
	    // lookup the escape sequence for the item
	    p = get_term_code(tname);
	    vim_free(tname);
	    اگر (p == پوچ)	    // ignore non-existing things
		p = (char_u *)"";

	    // Append it to the already found stuff
	    اگر ((صحیح)(STRLEN(buf) + STRLEN(p)) >= 99)
	    {
		semsg(_(e_terminal_code_too_long_str), arg);
		بازگشت FALSE;
	    }
	    STRCAT(buf, p);

	    // Advance to the next item
	    off += len;
	    اگر (arg[off] == ',')	    // another one follows
		++off;
	}
    }
    وگرنه
    {
	// Copy characters from arg[] to buf[], translating <> codes.
	برای (p = arg, off = 0; off < 100 - 6 && *p; )
	{
	    len = trans_special(&p, buf + off, FSK_SIMPLIFY, FALSE, پوچ);
	    اگر (len > 0)	    // recognized special char
		off += len;
	    وگرنه		    // copy as normal char
		buf[off++] = *p++;
	}
	buf[off] = NUL;
    }

    اگر (STRCMP(buf, "NONE") == 0)	// resetting the value
	p = پوچ;
    وگرنه
	p = vim_strsave(buf);
    اگر (کلید[2] == 'A')
    {
	vim_free(HL_TABLE()[idx].sg_start);
	HL_TABLE()[idx].sg_start = p;
    }
    وگرنه
    {
	vim_free(HL_TABLE()[idx].sg_stop);
	HL_TABLE()[idx].sg_stop = p;
    }
    بازگشت TRUE;
}

/*
 * Handle the ":highlight .." command.
 * When using ":hi clear" this is called recursively for each group with
 * "forceit" and "init" both TRUE.
 */
    تهی
do_highlight(
    char_u	*سطر,
    صحیح		forceit,
    صحیح		init)	    // TRUE when called for initializing
{
    char_u	*name_end;
    char_u	*linep;
    char_u	*کلید_start;
    char_u	*arg_start;
    char_u	*کلید = پوچ, *arg = پوچ;
    long	i;
    صحیح		id;
    صحیح		idx;
    hl_group_T	item_before;
    صحیح		did_change = FALSE;
    صحیح		dodefault = FALSE;
    صحیح		doclear = FALSE;
    صحیح		dolink = FALSE;
    صحیح		error = FALSE;
    صحیح		is_normal_group = FALSE;	// "Normal" group
#ifdef FEAT_GUI_MSWIN
    صحیح		is_titlebar_group = FALSE;	// "TitleBar" group
    صحیح		is_titlebarnc_group = FALSE;	// "TitleBarNC" group
#وگرنه
# define is_titlebar_group 0
# define is_titlebarnc_group 0
#endif
#ifdef FEAT_GUI_X11
    صحیح		is_menu_group = FALSE;		// "Menu" group
    صحیح		is_scrollbar_group = FALSE;	// "Scrollbar" group
    صحیح		is_tooltip_group = FALSE;	// "Tooltip" group
#وگرنه
# define is_menu_group 0
# define is_tooltip_group 0
# define is_scrollbar_group 0
#endif
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
    صحیح		do_colors = FALSE;		// need to update colors?
#endif
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    صحیح		did_highlight_changed = FALSE;
#endif

    // If no argument, list current highlighting.
    اگر (!init && ends_excmd2(سطر - 1, سطر))
    {
	برای (i = 1; i <= highlight_ga.ga_len && !got_int; ++i)
	    // TODO: only call when the group has attributes set
	    highlight_list_one((صحیح)i);
	بازگشت;
    }

    // Isolate the name.
    name_end = skiptowhite(سطر);
    linep = skipwhite(name_end);

    // Check for "default" argument.
    اگر (STRNCMP(سطر, "default", name_end - سطر) == 0)
    {
	dodefault = TRUE;
	سطر = linep;
	name_end = skiptowhite(سطر);
	linep = skipwhite(name_end);
    }

    // Check for "clear" or "link" argument.
    اگر (STRNCMP(سطر, "clear", name_end - سطر) == 0)
	doclear = TRUE;
    اگر (STRNCMP(سطر, "link", name_end - سطر) == 0)
	dolink = TRUE;

    // ":highlight {group-name}": list highlighting for one group.
    اگر (!doclear && !dolink && ends_excmd2(سطر, linep))
    {
	id = syn_namen2id(سطر, (صحیح)(name_end - سطر));
	اگر (id == 0)
	    semsg(_(e_highlight_group_name_not_found_str), سطر);
	وگرنه
	    highlight_list_one(id);
	بازگشت;
    }

    // Handle ":highlight link {from} {to}" command.
    اگر (dolink)
    {
	char_u	    *from_start = linep;
	char_u	    *from_end;
	صحیح	    from_len;
	char_u	    *to_start;
	char_u	    *to_end;
	صحیح	    to_len;

	from_end = skiptowhite(from_start);
	to_start = skipwhite(from_end);
	to_end	 = skiptowhite(to_start);

	اگر (ends_excmd2(سطر, from_start) || ends_excmd2(سطر, to_start))
	{
	    semsg(_(e_not_enough_arguments_highlight_link_str), from_start);
	    بازگشت;
	}

	اگر (!ends_excmd2(سطر, skipwhite(to_end)))
	{
	    semsg(_(e_too_many_arguments_highlight_link_str), from_start);
	    بازگشت;
	}

	from_len = (صحیح)(from_end - from_start);
	to_len = (صحیح)(to_end - to_start);
	highlight_group_link(from_start, from_len, to_start, to_len,
						dodefault, forceit, init);
	بازگشت;
    }

    اگر (doclear)
    {
	// ":highlight clear [group]" command.
	اگر (ends_excmd2(سطر, linep))
	{
	    // ":highlight clear" without group name
	    highlight_reset_all();
	    بازگشت;
	}
	سطر = linep;
	name_end = skiptowhite(سطر);
	linep = skipwhite(name_end);
    }

    // Find the group name in the table.  If it does not exist yet, add it.
    id = syn_check_group(سطر, (صحیح)(name_end - سطر));
    اگر (id == 0)	// failed (out of memory)
	بازگشت;
    idx = id - 1;	// index is ID minus one

    // Return if "default" was used and the group already has settings.
    اگر (dodefault && hl_has_settings(idx, TRUE))
	بازگشت;

    // Make a copy so we can check if any attribute actually changed.
    item_before = HL_TABLE()[idx];

    اگر (STRCMP(HL_TABLE()[idx].sg_name_u, "NORMAL") == 0)
	is_normal_group = TRUE;
#ifdef FEAT_GUI_MSWIN
    وگرنه اگر (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBAR") == 0)
	is_titlebar_group = TRUE;
    وگرنه اگر (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBARNC") == 0)
	is_titlebarnc_group = TRUE;
#endif
#ifdef FEAT_GUI_X11
    وگرنه اگر (STRCMP(HL_TABLE()[idx].sg_name_u, "MENU") == 0)
	is_menu_group = TRUE;
    وگرنه اگر (STRCMP(HL_TABLE()[idx].sg_name_u, "SCROLLBAR") == 0)
	is_scrollbar_group = TRUE;
    وگرنه اگر (STRCMP(HL_TABLE()[idx].sg_name_u, "TOOLTIP") == 0)
	is_tooltip_group = TRUE;
#endif

    // Clear the highlighting for ":hi clear {group}" and ":hi clear".
    اگر (doclear || (forceit && init))
    {
	highlight_clear(idx);
	اگر (!doclear)
	    HL_TABLE()[idx].sg_set = 0;
    }

    اگر (!doclear)
	وقتی (!ends_excmd2(سطر, linep))
	{
	    کلید_start = linep;
	    اگر (*linep == '=')
	    {
		semsg(_(e_unexpected_equal_sign_str), کلید_start);
		error = TRUE;
		بشکن;
	    }

	    // Note: Keep this in sync with get_highlight_group_key.

	    // Isolate the key ("term", "ctermfg", "ctermbg", "font", "guifg"
	    // or "guibg").
	    وقتی (*linep && !VIM_ISWHITE(*linep) && *linep != '=')
		++linep;
	    vim_free(کلید);
	    کلید = vim_strnsave_up(کلید_start, linep - کلید_start);
	    اگر (کلید == پوچ)
	    {
		error = TRUE;
		بشکن;
	    }
	    linep = skipwhite(linep);

	    اگر (STRCMP(کلید, "NONE") == 0)
	    {
		اگر (!init || HL_TABLE()[idx].sg_set == 0)
		{
		    اگر (!init)
			HL_TABLE()[idx].sg_set |= SG_TERM+SG_CTERM+SG_GUI;
		    highlight_clear(idx);
		}
		ادامه;
	    }

	    // Check for the equal sign.
	    اگر (*linep != '=')
	    {
		semsg(_(e_missing_equal_sign_str_2), کلید_start);
		error = TRUE;
		بشکن;
	    }
	    ++linep;

	    // Isolate the argument.
	    linep = skipwhite(linep);
	    اگر (*linep == '\'')		// guifg='color name'
	    {
		arg_start = ++linep;
		linep = vim_strchr(linep, '\'');
		اگر (linep == پوچ)
		{
		    semsg(_(e_invalid_argument_str), کلید_start);
		    error = TRUE;
		    بشکن;
		}
	    }
	    وگرنه
	    {
		arg_start = linep;
		linep = skiptowhite(linep);
	    }
	    اگر (linep == arg_start)
	    {
		semsg(_(e_missing_argument_str), کلید_start);
		error = TRUE;
		بشکن;
	    }
	    vim_free(arg);
	    arg = vim_strnsave(arg_start, linep - arg_start);
	    اگر (arg == پوچ)
	    {
		error = TRUE;
		بشکن;
	    }
	    اگر (*linep == '\'')
		++linep;

	    // Store the argument.
	    اگر (STRCMP(کلید, "TERM") == 0
		    || STRCMP(کلید, "CTERM") == 0
		    || STRCMP(کلید, "GUI") == 0)
	    {
		اگر (!highlight_set_termgui_attr(idx, کلید, arg, init))
		{
		    error = TRUE;
		    بشکن;
		}
	    }
	    وگرنه اگر (STRCMP(کلید, "FONT") == 0)
	    {
		// in non-GUI fonts are simply ignored
#ifdef FEAT_GUI
		اگر (highlight_set_font(idx, arg, is_normal_group,
					      is_menu_group, is_tooltip_group))
		    did_change = TRUE;
#endif
	    }
	    وگرنه اگر (STRCMP(کلید, "CTERMFG") == 0
		    || STRCMP(کلید, "CTERMBG") == 0
		    || STRCMP(کلید, "CTERMUL") == 0)
	    {
		اگر (!highlight_set_cterm_color(idx, کلید, کلید_start, arg,
							is_normal_group, init))
		{
		    error = TRUE;
		    بشکن;
		}
	    }
	    وگرنه اگر (STRCMP(کلید, "CTERMFONT") == 0)
	    {
		اگر (!highlight_set_cterm_font(idx, arg, init))
		{
		    error = TRUE;
		    بشکن;
		}
	    }
	    وگرنه اگر (STRCMP(کلید, "GUIFG") == 0)
	    {
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
		اگر (highlight_set_guifg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    وگرنه اگر (STRCMP(کلید, "GUIBG") == 0)
	    {
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
		اگر (highlight_set_guibg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    وگرنه اگر (STRCMP(کلید, "GUISP") == 0)
	    {
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
		اگر (highlight_set_guisp(idx, arg, init))
		    did_change = TRUE;
#endif
	    }
	    وگرنه اگر (STRCMP(کلید, "START") == 0 || STRCMP(کلید, "STOP") == 0)
	    {
		اگر (!highlight_set_startstop_termcode(idx, کلید, arg, init))
		{
		    error = TRUE;
		    بشکن;
		}
	    }
	    وگرنه
	    {
		semsg(_(e_illegal_argument_str_3), کلید_start);
		error = TRUE;
		بشکن;
	    }
	    HL_TABLE()[idx].sg_cleared = FALSE;

	    // When highlighting has been given for a group, don't link it.
	    اگر (!init || !(HL_TABLE()[idx].sg_set & SG_LINK))
		HL_TABLE()[idx].sg_link = 0;

	    // Continue with next argument.
	    linep = skipwhite(linep);
	}

    // If there is an error, and it's a new entry, remove it from the table.
    اگر (error && idx == highlight_ga.ga_len)
	syn_unadd_group();
    وگرنه
    {
	اگر (is_normal_group)
	{
	    HL_TABLE()[idx].sg_term_attr = 0;
	    HL_TABLE()[idx].sg_cterm_attr = 0;
#ifdef FEAT_GUI
	    HL_TABLE()[idx].sg_gui_attr = 0;
	    // Need to update all groups, because they might be using "bg"
	    // and/or "fg", which have been changed now.
#endif
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    اگر (USE_24BIT)
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
	وگرنه اگر (is_titlebar_group)
	{
	    اگر (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
	وگرنه اگر (is_titlebarnc_group)
	{
	    اگر (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
#endif
#ifdef FEAT_GUI_X11
# ifdef FEAT_MENU
	وگرنه اگر (is_menu_group)
	{
	    اگر (gui.in_use && do_colors)
		gui_mch_new_menu_colors();
	}
# endif
	وگرنه اگر (is_scrollbar_group)
	{
	    اگر (gui.in_use && do_colors)
		gui_new_scrollbar_colors();
	    وگرنه
		set_hl_attr(idx);
	}
# ifdef FEAT_BEVAL_GUI
	وگرنه اگر (is_tooltip_group)
	{
	    اگر (gui.in_use && do_colors)
		gui_mch_new_tooltip_colors();
	}
# endif
#endif
	وگرنه
	    set_hl_attr(idx);
#ifdef FEAT_EVAL
	HL_TABLE()[idx].sg_script_ctx = current_sctx;
	HL_TABLE()[idx].sg_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
    }

    vim_free(کلید);
    vim_free(arg);

    // Only call highlight_changed() once, after a sequence of highlight
    // commands, and only if an attribute actually changed.
    اگر ((did_change
	   || memcmp(&HL_TABLE()[idx], &item_before, اندازه(item_before)) != 0)
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    && !did_highlight_changed
#endif
       )
    {
	// Do not trigger a redraw when highlighting is changed while
	// redrawing.  This may happen when evaluating 'statusline' changes the
	// StatusLine group.
	اگر (!updating_screen)
	    redraw_all_later(UPD_NOT_VALID);
	need_highlight_changed = TRUE;
    }
}

#اگر defined(EXITFREE)
    تهی
free_highlight(تهی)
{
    صحیح	    i;

    برای (i = 0; i < highlight_ga.ga_len; ++i)
    {
	highlight_clear(i);
	vim_free(HL_TABLE()[i].sg_name);
	vim_free(HL_TABLE()[i].sg_name_u - HLNAME_کلید_OFF);
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
    تهی
restore_cterm_colors(تهی)
{
#اگر defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
    // Since t_me has been set, this probably means that the user
    // wants to use this as default colors.  Need to reset default
    // background/foreground colors.
    mch_set_normal_colors();
#وگرنه
# ifdef VIMDLL
    اگر (!gui.in_use)
    {
	mch_set_normal_colors();
	بازگشت;
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
    ایستا صحیح
hl_has_settings(صحیح idx, صحیح check_link)
{
    بازگشت HL_TABLE()[idx].sg_cleared == 0
	 && (  HL_TABLE()[idx].sg_term_attr != 0
	    || HL_TABLE()[idx].sg_cterm_attr != 0
	    || HL_TABLE()[idx].sg_cterm_fg != 0
	    || HL_TABLE()[idx].sg_cterm_bg != 0
	    || HL_TABLE()[idx].sg_cterm_font != 0
#ifdef FEAT_GUI
	    || HL_TABLE()[idx].sg_gui_attr != 0
	    || HL_TABLE()[idx].sg_gui_fg_name != پوچ
	    || HL_TABLE()[idx].sg_gui_bg_name != پوچ
	    || HL_TABLE()[idx].sg_gui_sp_name != پوچ
	    || HL_TABLE()[idx].sg_font_name != پوچ
#endif
	    || (check_link && (HL_TABLE()[idx].sg_set & SG_LINK)));
}

/*
 * Clear highlighting for one group.
 */
    ایستا تهی
highlight_clear(صحیح idx)
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
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
    HL_TABLE()[idx].sg_gui = 0;
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_fg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_bg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_sp_name);
#endif
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Set the normal foreground and background colors according to the "Normal"
 * highlighting group.  For X11 also set "Menu", "Scrollbar" and "Tooltip"
 * colors.  For MS-Windows also set "TitleBar" and "TitleBarNC" colors.
 */
    تهی
set_normal_colors(تهی)
{
# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    اگر (gui.in_use)
#  endif
    {
	اگر (set_group_colors((char_u *)"Normal",
				 &gui.norm_pixel, &gui.back_pixel,
				 FALSE, TRUE, FALSE))
	{
	    gui_mch_new_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  ifdef FEAT_GUI_MSWIN
	اگر (set_group_colors((char_u *)"TitleBar",
				 &gui.title_fg_pixel, &gui.title_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
	اگر (set_group_colors((char_u *)"TitleBarNC",
				 &gui.titlenc_fg_pixel, &gui.titlenc_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  endif
#  ifdef FEAT_GUI_X11
	اگر (set_group_colors((char_u *)"Menu",
			     &gui.menu_fg_pixel, &gui.menu_bg_pixel,
			     TRUE, FALSE, FALSE))
	{
#   ifdef FEAT_MENU
	    gui_mch_new_menu_colors();
#   endif
	    set_must_redraw(UPD_CLEAR);
	}
#   ifdef FEAT_BEVAL_GUI
	اگر (set_group_colors((char_u *)"Tooltip",
			     &gui.tooltip_fg_pixel, &gui.tooltip_bg_pixel,
			     FALSE, FALSE, TRUE))
	{
#    ifdef FEAT_TOOLBAR
	    gui_mch_new_tooltip_colors();
#    endif
	    set_must_redraw(UPD_CLEAR);
	}
#   endif
	اگر (set_group_colors((char_u *)"Scrollbar",
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
    وگرنه
#  endif
    {
	صحیح		idx;

	idx = syn_name2id((char_u *)"Normal") - 1;
	اگر (idx >= 0)
	{
	    gui_do_one_color(idx, FALSE, FALSE);

	    // If the normal fg or bg color changed a complete redraw is
	    // required.
	    اگر (cterm_normal_fg_gui_color != HL_TABLE()[idx].sg_gui_fg
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

#اگر defined(FEAT_GUI)
/*
 * Set the colors for "Normal", "Menu", "TitleBar", "TitleBarNC", "Tooltip" or
 * "Scrollbar".
 */
    ایستا صحیح
set_group_colors(
    char_u	*name,
    guicolor_T	*fgp,
    guicolor_T	*bgp,
    صحیح		do_menu,
    صحیح		use_norm,
    صحیح		do_tooltip)
{
    صحیح		idx;

    idx = syn_name2id(name) - 1;
    اگر (idx < 0)
	بازگشت FALSE;

    gui_do_one_color(idx, do_menu, do_tooltip);

    اگر (HL_TABLE()[idx].sg_gui_fg != INVALCOLOR)
	*fgp = HL_TABLE()[idx].sg_gui_fg;
    وگرنه اگر (use_norm)
	*fgp = gui.def_norm_pixel;
    اگر (HL_TABLE()[idx].sg_gui_bg != INVALCOLOR)
	*bgp = HL_TABLE()[idx].sg_gui_bg;
    وگرنه اگر (use_norm)
	*bgp = gui.def_back_pixel;
    بازگشت TRUE;
}

/*
 * Get the font of the "Normal" group.
 * Returns "" when it's not found or not set.
 */
    char_u *
hl_get_font_name(تهی)
{
    صحیح		id;
    char_u	*s;

    id = syn_name2id((char_u *)"Normal");
    اگر (id > 0)
    {
	s = HL_TABLE()[id - 1].sg_font_name;
	اگر (s != پوچ)
	    بازگشت s;
    }
    بازگشت (char_u *)"";
}

/*
 * Set font for "Normal" group.  Called by gui_mch_init_font() when a font has
 * actually chosen to be used.
 */
    تهی
hl_set_font_name(char_u *font_name)
{
    صحیح	    id;

    id = syn_name2id((char_u *)"Normal");
    اگر (id <= 0)
	بازگشت;

    vim_free(HL_TABLE()[id - 1].sg_font_name);
    HL_TABLE()[id - 1].sg_font_name = vim_strsave(font_name);
}

/*
 * Set background color for "Normal" group.  Called by gui_set_bg_color()
 * when the color is known.
 */
    تهی
hl_set_bg_color_name(
    char_u  *name)	    // must have been allocated
{
    صحیح	    id;

    اگر (name == پوچ)
	بازگشت;

    id = syn_name2id((char_u *)"Normal");
    اگر (id <= 0)
	بازگشت;

    vim_free(HL_TABLE()[id - 1].sg_gui_bg_name);
    HL_TABLE()[id - 1].sg_gui_bg_name = name;
}

/*
 * Set foreground color for "Normal" group.  Called by gui_set_fg_color()
 * when the color is known.
 */
    تهی
hl_set_fg_color_name(
    char_u  *name)	    // must have been allocated
{
    صحیح	    id;

    اگر (name == پوچ)
	بازگشت;

    id = syn_name2id((char_u *)"Normal");
    اگر (id <= 0)
	بازگشت;

    vim_free(HL_TABLE()[id - 1].sg_gui_fg_name);
    HL_TABLE()[id - 1].sg_gui_fg_name = name;
}

/*
 * Return the handle for a font name.
 * Returns NOFONT when failed.
 */
    ایستا GuiFont
font_name2handle(char_u *name)
{
    اگر (STRCMP(name, "NONE") == 0)
	بازگشت NOFONT;

    بازگشت gui_mch_get_font(name, TRUE);
}

# ifdef FEAT_XFONTSET
/*
 * Return the handle for a fontset name.
 * Returns NOFONTSET when failed.
 */
    ایستا GuiFontset
fontset_name2handle(char_u *name, صحیح fixed_width)
{
    اگر (STRCMP(name, "NONE") == 0)
	بازگشت NOFONTSET;

    بازگشت gui_mch_get_fontset(name, TRUE, fixed_width);
}
# endif

/*
 * Get the font or fontset for one highlight group.
 */
    ایستا تهی
hl_do_font(
    صحیح		idx,
    char_u	*arg,
    صحیح		do_normal,		// set normal font
    صحیح		do_menu UNUSED,		// set menu font
    صحیح		do_tooltip UNUSED,	// set tooltip font
    صحیح		free_font)		// free current font/fontset
{
# ifdef FEAT_XFONTSET
    // If 'guifontset' is not empty, first try using the name as a
    // fontset.  If that doesn't work, use it as a font name.
    اگر (*p_guifontset != NUL
#  ifdef FONTSET_ALWAYS
	|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
	// In Motif, the Tooltip highlight group is always a fontset
	|| do_tooltip
#  endif
	    )
    {
	اگر (free_font)
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
    اگر (HL_TABLE()[idx].sg_fontset != NOFONTSET)
    {
	// If it worked and it's the Normal group, use it as the normal
	// fontset.  Same for the Menu group.
	اگر (do_normal)
	    gui_init_font(arg, TRUE);
#  اگر defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	اگر (do_menu)
	{
#   ifdef FONTSET_ALWAYS
	    gui.menu_fontset = HL_TABLE()[idx].sg_fontset;
#   وگرنه
	    // YIKES!  This is a bug waiting to crash the program
	    gui.menu_font = HL_TABLE()[idx].sg_fontset;
#   endif
	    gui_mch_new_menu_font();
	}
#   ifdef FEAT_BEVAL_GUI
	اگر (do_tooltip)
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
    وگرنه
# endif
    {
	اگر (free_font)
	    gui_mch_free_font(HL_TABLE()[idx].sg_font);
	HL_TABLE()[idx].sg_font = font_name2handle(arg);
	// If it worked and it's the Normal group, use it as the
	// normal font.  Same for the Menu group.
	اگر (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    اگر (do_normal)
		gui_init_font(arg, FALSE);
# ifndef FONTSET_ALWAYS
#  اگر defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	    اگر (do_menu)
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

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return the handle for a color name.
 * Returns INVALCOLOR when failed.
 */
    guicolor_T
color_name2handle(char_u *name)
{
    اگر (STRCMP(name, "NONE") == 0)
	بازگشت INVALCOLOR;

    اگر (STRICMP(name, "fg") == 0 || STRICMP(name, "foreground") == 0)
    {
# اگر defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	اگر (gui.in_use)
# endif
# ifdef FEAT_GUI
	    بازگشت gui.norm_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	اگر (cterm_normal_fg_gui_color != INVALCOLOR)
	    بازگشت cterm_normal_fg_gui_color;
	// Guess that the foreground is black or white.
	بازگشت GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "black" : "white"));
# endif
    }
    اگر (STRICMP(name, "bg") == 0 || STRICMP(name, "background") == 0)
    {
# اگر defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	اگر (gui.in_use)
# endif
# ifdef FEAT_GUI
	    بازگشت gui.back_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	اگر (cterm_normal_bg_gui_color != INVALCOLOR)
	    بازگشت cterm_normal_bg_gui_color;
	// Guess that the background is white or black.
	بازگشت GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "white" : "black"));
# endif
    }

    بازگشت GUI_GET_COLOR(name);
}

// On MS-Windows an RGB macro is available and it produces 0x00bbggrr color
// values as used by the MS-Windows GDI api.  It should be used only for
// MS-Windows GDI builds.
# اگر defined(RGB) && defined(MSWIN) && !defined(FEAT_GUI)
#  undef RGB
# endif
# ifndef RGB
#  define RGB(r, g, b)	(((r)<<16) | ((g)<<8) | (b))
# endif

# ifdef VIMDLL
    ایستا guicolor_T
gui_adjust_rgb(guicolor_T c)
{
    اگر (gui.in_use)
	بازگشت c;
    وگرنه
	بازگشت ((c & 0xff) << 16) | (c & 0x00ff00) | ((c >> 16) & 0xff);
}
# وگرنه
#  define gui_adjust_rgb(c) (c)
# endif

    ایستا صحیح
hex_digit(صحیح c)
{
    اگر (SAFE_isdigit(c))
	بازگشت c - '0';
    c = TOLOWER_ASC(c);
    اگر (c >= 'a' && c <= 'f')
	بازگشت c - 'a' + 10;
    بازگشت 0x1ffffff;
}

    ایستا guicolor_T
decode_hex_color(char_u *hex)
{
    guicolor_T color;

    اگر (hex[0] != '#' || STRLEN(hex) != 7)
	بازگشت INVALCOLOR;

    // Name is in "#rrggbb" format
    color = RGB(((hex_digit(hex[1]) << 4) + hex_digit(hex[2])),
		((hex_digit(hex[3]) << 4) + hex_digit(hex[4])),
		((hex_digit(hex[5]) << 4) + hex_digit(hex[6])));
    اگر (color > 0xffffff)
	بازگشت INVALCOLOR;
    بازگشت gui_adjust_rgb(color);
}

# ifdef FEAT_EVAL
// Returns the color currently mapped to the given name or INVALCOLOR if no
// such name exists in the color table. The convention is to use lowercase for
// all keys in the v:colornames dictionary. The value can be either a string in
// the form #rrggbb or a number, either of which is converted to a guicolor_T.
    ایستا guicolor_T
colorname2rgb(char_u *name)
{
    dict_T      *colornames_table = get_vim_var_dict(VV_COLORNAMES);
    char_u      *lc_name;
    dictitem_T  *colentry;
    char_u      *colstr;
    varnumber_T colnum;

    lc_name = strlow_save(name);
    اگر (lc_name == پوچ)
	بازگشت INVALCOLOR;

    colentry = dict_find(colornames_table, lc_name, -1);
    vim_free(lc_name);
    اگر (colentry == پوچ)
	بازگشت INVALCOLOR;

    اگر (colentry->di_tv.v_type == VAR_STRING)
    {
	colstr = tv_get_string_strict(&colentry->di_tv);
	اگر ((STRLEN(colstr) == 7) && (*colstr == '#'))
	{
	    بازگشت decode_hex_color(colstr);
	}
	وگرنه
	{
	    semsg(_(e_bad_color_string_str), colstr);
	    بازگشت INVALCOLOR;
	}
    }

    اگر (colentry->di_tv.v_type == VAR_عدد)
    {
	colnum = tv_get_عدد(&colentry->di_tv);
	بازگشت (guicolor_T)colnum;
    }

    بازگشت INVALCOLOR;
}

# endif

    guicolor_T
gui_get_color_cmn(char_u *name)
{
    guicolor_T  color;
    // Only non X11 colors (not present in rgb.txt) and colors in
    // color_name_tab[], useful when $VIMRUNTIME is not found,.
    // must be sorted by the 'value' field because it is used by bsearch()!
    ایستا keyvalue_T rgb_tab[] = {
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
    اگر (color != INVALCOLOR)
	بازگشت color;

    target.کلید = 0;
    target.مقدار.string = name;
    target.مقدار.length = 0;	// not used, see cmp_keyvalue_value_i()
    entry = (keyvalue_T *)bsearch(&target, &rgb_tab, ARRAY_LENGTH(rgb_tab),
	    اندازه(rgb_tab[0]), cmp_keyvalue_مقدار_i);
    اگر (entry != پوچ)
	بازگشت gui_adjust_rgb((guicolor_T)entry->کلید);

# اگر defined(FEAT_EVAL)
    /*
     * Not a traditional color. Load additional color aliases and then consult the alias table.
     */

    color = colorname2rgb(name);
    اگر (color == INVALCOLOR)
    {
	load_default_colors_lists();
	color = colorname2rgb(name);
    }

    بازگشت color;
# وگرنه
    بازگشت INVALCOLOR;
# endif
}

    guicolor_T
gui_get_rgb_color_cmn(صحیح r, صحیح g, صحیح b)
{
    guicolor_T  color = RGB(r, g, b);

    اگر (color > 0xffffff)
	بازگشت INVALCOLOR;
    بازگشت gui_adjust_rgb(color);
}
#endif

/*
 * Table with the specifications for an attribute number.
 * Note that this table is used by ALL buffers.  This is required because the
 * GUI can redraw at any time for any buffer.
 */
ایستا garray_T	term_attr_table = {0, 0, 0, 0, پوچ};

#define TERM_ATTR_ENTRY(idx) ((attrentry_T *)term_attr_table.ga_data)[idx]

ایستا garray_T	cterm_attr_table = {0, 0, 0, 0, پوچ};

#define CTERM_ATTR_ENTRY(idx) ((attrentry_T *)cterm_attr_table.ga_data)[idx]

#ifdef FEAT_GUI
ایستا garray_T	gui_attr_table = {0, 0, 0, 0, پوچ};

# define GUI_ATTR_ENTRY(idx) ((attrentry_T *)gui_attr_table.ga_data)[idx]
#endif

/*
 * Return the attr number for a set of colors and font.
 * Add a new entry to the term_attr_table, cterm_attr_table or gui_attr_table
 * if the combination is new.
 * Return 0 for error (no more room).
 */
    ایستا صحیح
get_attr_entry(garray_T *table, attrentry_T *aep)
{
    صحیح		i;
    attrentry_T	*taep;
    ایستا صحیح	recursive = FALSE;

    // Init the table, in case it wasn't done yet.
    table->ga_itemsize = اندازه(attrentry_T);
    table->ga_growsize = 7;

    // Try to find an entry with the same specifications.
    برای (i = 0; i < table->ga_len; ++i)
    {
	taep = &(((attrentry_T *)table->ga_data)[i]);
	اگر (	   aep->ae_attr == taep->ae_attr
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
			&& (aep->ae_u.term.start == پوچ)
					    == (taep->ae_u.term.start == پوچ)
			&& (aep->ae_u.term.start == پوچ
			    || STRCMP(aep->ae_u.term.start,
						  taep->ae_u.term.start) == 0)
			&& (aep->ae_u.term.stop == پوچ)
					     == (taep->ae_u.term.stop == پوچ)
			&& (aep->ae_u.term.stop == پوچ
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

	بازگشت i + ATTR_OFF;
    }

    اگر (table->ga_len + ATTR_OFF > MAX_TYPENR)
    {
	// Running out of attribute entries!  remove all attributes, and
	// compute new ones for all groups.
	// When called recursively, we are really out of numbers.
	اگر (recursive)
	{
	    emsg(_(e_too_many_different_highlighting_attributes_in_use));
	    بازگشت 0;
	}
	recursive = TRUE;

	clear_hl_tables();

	set_must_redraw(UPD_CLEAR);

	برای (i = 0; i < highlight_ga.ga_len; ++i)
	    set_hl_attr(i);

	recursive = FALSE;
    }

    // This is a new combination of colors and font, add an entry.
    اگر (ga_grow(table, 1) == FAIL)
	بازگشت 0;

    taep = &(((attrentry_T *)table->ga_data)[table->ga_len]);
    CLEAR_POINTER(taep);
    taep->ae_attr = aep->ae_attr;
#ifdef FEAT_GUI
    اگر (table == &gui_attr_table)
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
    اگر (table == &term_attr_table)
    {
	اگر (aep->ae_u.term.start == پوچ)
	    taep->ae_u.term.start = پوچ;
	وگرنه
	    taep->ae_u.term.start = vim_strsave(aep->ae_u.term.start);
	اگر (aep->ae_u.term.stop == پوچ)
	    taep->ae_u.term.stop = پوچ;
	وگرنه
	    taep->ae_u.term.stop = vim_strsave(aep->ae_u.term.stop);
    }
    وگرنه اگر (table == &cterm_attr_table)
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
    بازگشت (table->ga_len - 1 + ATTR_OFF);
}

#اگر defined(FEAT_TERMINAL)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    صحیح
get_cterm_attr_idx(صحیح attr, صحیح fg, صحیح bg)
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
    بازگشت get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#اگر defined(FEAT_TERMINAL) && defined(FEAT_TERMGUICOLORS)
/*
 * Get an attribute index for a 'termguicolors' entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    صحیح
get_tgc_attr_idx(صحیح attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    اگر (fg == INVALCOLOR && bg == INVALCOLOR)
    {
	// If both GUI colors are not set fall back to the cterm colors.  Helps
	// if the GUI only has an attribute, such as undercurl.
	at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
    }
    وگرنه
    {
	at_en.ae_u.cterm.fg_rgb = fg;
	at_en.ae_u.cterm.bg_rgb = bg;
    }
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
    بازگشت get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#اگر defined(FEAT_TERMINAL) && defined(FEAT_GUI)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    صحیح
get_gui_attr_idx(صحیح attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    at_en.ae_u.gui.fg_color = fg;
    at_en.ae_u.gui.bg_color = bg;
    بازگشت get_attr_entry(&gui_attr_table, &at_en);
}
#endif

/*
 * Clear all highlight tables.
 */
    تهی
clear_hl_tables(تهی)
{
    صحیح		i;
    attrentry_T	*taep;

#ifdef FEAT_GUI
    ga_clear(&gui_attr_table);
#endif
    برای (i = 0; i < term_attr_table.ga_len; ++i)
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
    صحیح
hl_combine_attr(صحیح char_attr, صحیح prim_attr)
{
    attrentry_T *char_aep = پوچ;
    attrentry_T *prim_aep;
    attrentry_T new_en;

    اگر (char_attr == 0)
	بازگشت prim_attr;
    اگر (char_attr <= HL_ALL && prim_attr <= HL_ALL)
	بازگشت ATTR_COMBINE(char_attr, prim_attr);
#ifdef FEAT_GUI
    اگر (gui.in_use)
    {
	اگر (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	اگر (char_aep != پوچ)
	    new_en = *char_aep;
	وگرنه
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    اگر (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	اگر (prim_attr <= HL_ALL)
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	وگرنه
	{
	    prim_aep = syn_gui_attr2entry(prim_attr);
	    اگر (prim_aep != پوچ)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		اگر (prim_aep->ae_u.gui.fg_color != INVALCOLOR)
		    new_en.ae_u.gui.fg_color = prim_aep->ae_u.gui.fg_color;
		اگر (prim_aep->ae_u.gui.bg_color != INVALCOLOR)
		    new_en.ae_u.gui.bg_color = prim_aep->ae_u.gui.bg_color;
		اگر (prim_aep->ae_u.gui.sp_color != INVALCOLOR)
		    new_en.ae_u.gui.sp_color = prim_aep->ae_u.gui.sp_color;
		اگر (prim_aep->ae_u.gui.font != NOFONT)
		    new_en.ae_u.gui.font = prim_aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
		اگر (prim_aep->ae_u.gui.fontset != NOFONTSET)
		    new_en.ae_u.gui.fontset = prim_aep->ae_u.gui.fontset;
# endif
	    }
	}
	بازگشت get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    اگر (IS_CTERM)
    {
	اگر (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	اگر (char_aep != پوچ)
	    new_en = *char_aep;
	وگرنه
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    اگر (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	اگر (prim_attr <= HL_ALL)
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	وگرنه
	{
	    prim_aep = syn_cterm_attr2entry(prim_attr);
	    اگر (prim_aep != پوچ)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		اگر (prim_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color = prim_aep->ae_u.cterm.fg_color;
		اگر (prim_aep->ae_u.cterm.bg_color > 0)
		    new_en.ae_u.cterm.bg_color = prim_aep->ae_u.cterm.bg_color;
		اگر (prim_aep->ae_u.cterm.ul_color > 0)
		    new_en.ae_u.cterm.ul_color = prim_aep->ae_u.cterm.ul_color;
		اگر (prim_aep->ae_u.cterm.font > 0)
		    new_en.ae_u.cterm.font = prim_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		// If both fg and bg are not set fall back to cterm colors.
		// Helps for SpellBad which uses undercurl in the GUI.
		اگر (COLOR_INVALID(prim_aep->ae_u.cterm.fg_rgb)
			&& COLOR_INVALID(prim_aep->ae_u.cterm.bg_rgb))
		{
		    اگر (prim_aep->ae_u.cterm.fg_color > 0)
			new_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
		    اگر (prim_aep->ae_u.cterm.bg_color > 0)
			new_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
		}
		وگرنه
		{
		    اگر (prim_aep->ae_u.cterm.fg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.fg_rgb = prim_aep->ae_u.cterm.fg_rgb;
		    اگر (prim_aep->ae_u.cterm.bg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.bg_rgb = prim_aep->ae_u.cterm.bg_rgb;
		}
		اگر (prim_aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		    new_en.ae_u.cterm.ul_rgb = prim_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	}
	بازگشت get_attr_entry(&cterm_attr_table, &new_en);
    }

    اگر (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    اگر (char_aep != پوچ)
	new_en = *char_aep;
    وگرنه
    {
	CLEAR_FIELD(new_en);
	اگر (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    اگر (prim_attr <= HL_ALL)
	new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
    وگرنه
    {
	prim_aep = syn_term_attr2entry(prim_attr);
	اگر (prim_aep != پوچ)
	{
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_aep->ae_attr);
	    اگر (prim_aep->ae_u.term.start != پوچ)
	    {
		new_en.ae_u.term.start = prim_aep->ae_u.term.start;
		new_en.ae_u.term.stop = prim_aep->ae_u.term.stop;
	    }
	}
    }
    بازگشت get_attr_entry(&term_attr_table, &new_en);
}

// ANSI color order: Black, Red, Green, Yellow, Blue, Magenta,
// Cyan, White, then bright variants.  Approximate RGB values used when
// only a cterm color number is known (no guifg/guibg).  Real terminal
// palettes may differ if the user reconfigured their emulator, but
// these are reasonable xterm-ish defaults.
ایستا ثابت long_u cterm_color_16[16] = {
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
    ایستا guicolor_T
cterm_color_to_rgb(صحیح color_nr)
{
    اگر (color_nr < 1 || color_nr > 16)
	بازگشت INVALCOLOR;
    بازگشت (guicolor_T)cterm_color_16[color_nr - 1];
}
#endif

/*
 * Convert an xterm 256-color index (0-255) to an approximate RGB triple.
 * Uses the standard xterm palette: 0-15 ANSI (cterm_color_16), 16-231
 * 6x6x6 cube, 232-255 grayscale ramp.
 */
    ایستا تهی
cterm_idx_to_rgb(صحیح idx, صحیح *r, صحیح *g, صحیح *b)
{
    ایستا ثابت صحیح cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

    اگر (idx < 0 || idx > 255)
    {
	*r = *g = *b = 0;
	بازگشت;
    }
    اگر (idx < 16)
    {
	long_u rgb = cterm_color_16[idx];
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
    }
    وگرنه اگر (idx < 232)
    {
	صحیح n = idx - 16;
	*r = cube[n / 36 % 6];
	*g = cube[n / 6  % 6];
	*b = cube[n      % 6];
    }
    وگرنه
    {
	صحیح v = 8 + (idx - 232) * 10;
	*r = *g = *b = v;
    }
}

/*
 * Approximate an RGB triple to the nearest xterm 256-color index.
 * Searches the 6x6x6 cube and the grayscale ramp; returns 0-255.
 */
    ایستا صحیح
rgb_to_cterm_idx(صحیح r, صحیح g, صحیح b)
{
    ایستا ثابت صحیح cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };
    صحیح best = 0;
    long best_d = -1;
    صحیح i;

    // Search the 6x6x6 cube.
    برای (i = 16; i < 232; ++i)
    {
	صحیح n = i - 16;
	صحیح cr = cube[n / 36 % 6];
	صحیح cg = cube[n / 6  % 6];
	صحیح cb = cube[n      % 6];
	long d = (cr - r) * (cr - r) + (cg - g) * (cg - g) + (cb - b) * (cb - b);
	اگر (best_d < 0 || d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    // Search the grayscale ramp.
    برای (i = 232; i < 256; ++i)
    {
	صحیح v = 8 + (i - 232) * 10;
	long d = (v - r) * (v - r) + (v - g) * (v - g) + (v - b) * (v - b);
	اگر (d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    بازگشت best;
}

/*
 * Resolve a color to an RGB triple.  Prefer the gui RGB value (set by
 * guifg/guibg) when valid, otherwise convert the cterm 256-color index.
 * Returns true on success; false when no color is defined.
 * cterm_c is 1-based (0 means "no color").
 */
    ایستا bool
resolve_color_to_rgb(صحیح cterm_c, guicolor_T rgb UNUSED, صحیح *r, صحیح *g, صحیح *b)
{
#ifdef FEAT_TERMGUICOLORS
    اگر (!COLOR_INVALID(rgb))
    {
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
	بازگشت true;
    }
#endif
    اگر (cterm_c > 0)
    {
	cterm_idx_to_rgb(cterm_c - 1, r, g, b);
	بازگشت true;
    }
    بازگشت false;
}

/*
 * get a RGB fallback color from gui, cterm or default color
 */
    ایستا guicolor_T
resolve_fallback_color(صحیح cterm_c, guicolor_T rgb, guicolor_T default_rgb)
{
    صحیح red, green, blue;
    اگر (!resolve_color_to_rgb(cterm_c, rgb, &red, &green, &blue))
	بازگشت default_rgb;
    وگرنه
	بازگشت (red << 16) | (green << 8) | blue;
}

/*
 * get a RGB fallback foreground color from guifg, ctermfg or deduced from background
 */
    ایستا تهی
resolve_fallback_fg_to_rgb(تهی)
{
    guicolor_T fgcolor_or_gui_fgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    fgcolor_or_gui_fgcolor = cterm_normal_fg_gui_color;
#endif
#ifdef FEAT_GUI
    اگر (gui.in_use)
	fgcolor_or_gui_fgcolor = gui.norm_pixel;
#endif
    fallback_fg_rgb = resolve_fallback_color(cterm_normal_fg_color, fgcolor_or_gui_fgcolor, (*p_bg == 'l') ? 0x000000 : 0xFFFFFF);
}

/*
 * get a RGB fallback background color from guifg, ctermbg or deduced from background
 */
    ایستا تهی
resolve_fallback_bg_to_rgb(تهی)
{
    guicolor_T bgcolor_or_gui_bgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    bgcolor_or_gui_bgcolor = cterm_normal_bg_gui_color;
#endif
#ifdef FEAT_GUI
    اگر (gui.in_use)
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
    ایستا صحیح
blend_cterm_colors(صحیح popup_c, guicolor_T popup_rgb,
		   صحیح under_c, guicolor_T under_rgb,
		   صحیح default_rgb,
		   صحیح blend_val)
{
    صحیح pr, pg, pb, ur, ug, ub, r, g, b;

    اگر (t_colors < 256)
	بازگشت popup_c;
    اگر (!resolve_color_to_rgb(popup_c, popup_rgb, &pr, &pg, &pb))
	بازگشت under_c;
    اگر (blend_val <= 0)
	بازگشت rgb_to_cterm_idx(pr, pg, pb) + 1;
    اگر (!resolve_color_to_rgb(under_c, under_rgb, &ur, &ug, &ub))
    {
	ur = (default_rgb >> 16) & 0xFF;
	ug = (default_rgb >> 8) & 0xFF;
	ub = default_rgb & 0xFF;
    }
    اگر (blend_val >= 100)
	بازگشت rgb_to_cterm_idx(ur, ug, ub) + 1;
    r = pr + (ur - pr) * blend_val / 100;
    g = pg + (ug - pg) * blend_val / 100;
    b = pb + (ub - pb) * blend_val / 100;
    بازگشت rgb_to_cterm_idx(r, g, b) + 1;
}

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Blend two RGB colors based on blend value (0-100).
 * blend: 0=return first argument color, 100=return second argument color
 * If bg_color is INVALCOLOR, high blend means more visible (return INVALCOLOR).
 */
    ایستا guicolor_T
blend_colors(guicolor_T popup_color, guicolor_T bg_color, صحیح blend_val)
{
    صحیح r1, g1, b1, r2, g2, b2, r, g, b;

    // CTERMCOLOR is a sentinel meaning "use the cterm color"; for blending
    // it has no real RGB so treat it like INVALCOLOR.
    اگر (COLOR_INVALID(popup_color))
	بازگشت INVALCOLOR;

    // Fully transparent: use underlying color as-is.
    اگر (blend_val >= 100)
	بازگشت bg_color;

    r1 = (popup_color >> 16) & 0xFF;
    g1 = (popup_color >> 8) & 0xFF;
    b1 = popup_color & 0xFF;

    اگر (COLOR_INVALID(bg_color))
	bg_color = fallback_bg_rgb;

    r2 = (bg_color >> 16) & 0xFF;
    g2 = (bg_color >> 8) & 0xFF;
    b2 = bg_color & 0xFF;

    r = r1 + (r2 - r1) * blend_val / 100;
    g = g1 + (g2 - g1) * blend_val / 100;
    b = b1 + (b2 - b1) * blend_val / 100;

    بازگشت (r << 16) | (g << 8) | b;
}
#endif

/*
 * Blend attributes for popup windows and menus with opacity.
 * Blends foreground and/or background colors based on blend value (0-100).
 * blend: 0 = opaque (use popup colors), 100 = transparent (use background colors)
 * blend_fg: TRUE to blend foreground color, FALSE to keep popup foreground
 */
    ایستا صحیح
hl_blend_attr_common(
    صحیح		char_attr,
    صحیح		popup_attr,
    صحیح		blend,
    صحیح		blend_fg)
{
    attrentry_T *char_aep = پوچ;
    attrentry_T *popup_aep;
    attrentry_T new_en;
    attrentry_T tmp_en;

#ifdef FEAT_GUI
    اگر (gui.in_use)
    {
	اگر (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	اگر (char_aep != پوچ)
	    new_en = *char_aep;
	وگرنه
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    اگر (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	اگر (popup_attr <= HL_ALL)
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
	وگرنه
	    popup_aep = syn_gui_attr2entry(popup_attr);

	اگر (popup_aep != پوچ)
	{
	    guicolor_T popup_bg_rgb = popup_aep->ae_u.gui.bg_color;
	    اگر (COLOR_INVALID(popup_bg_rgb))
		popup_bg_rgb = fallback_bg_rgb;

	    اگر (blend_fg)
	    {
		// blend_fg=TRUE: fade underlying text toward popup bg.
		{
		    صحیح base_fg = fallback_fg_rgb;
		    اگر (char_aep != پوچ
			    && char_aep->ae_u.gui.fg_color != INVALCOLOR)
			base_fg = char_aep->ae_u.gui.fg_color;
		    new_en.ae_u.gui.fg_color = blend_colors(
			    popup_bg_rgb, base_fg, blend);
		}
	    }
	    وگرنه
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
		اگر (char_aep != پوچ
			&& !COLOR_INVALID(char_aep->ae_u.gui.bg_color))
		    underlying_bg = char_aep->ae_u.gui.bg_color;
		new_en.ae_u.gui.bg_color = blend_colors(
			popup_bg_rgb,
			underlying_bg, blend);
	    }
	}
	بازگشت get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    اگر (IS_CTERM)
    {
	اگر (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	اگر (char_aep != پوچ)
	    new_en = *char_aep;
	وگرنه
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    اگر (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	اگر (popup_attr <= HL_ALL)
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
	وگرنه
	    popup_aep = syn_cterm_attr2entry(popup_attr);

	اگر (popup_aep != پوچ)
	{
	    guicolor_T popup_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
	    // Fall back to cterm color converted to RGB when gui color is not set.
	    popup_bg_rgb = popup_aep->ae_u.cterm.bg_rgb;
	    اگر (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color > 0)
		popup_bg_rgb = cterm_color_to_rgb(
			popup_aep->ae_u.cterm.bg_color);
#endif
	    // assign default color if guibg and ctermbg are not set for popup
	    اگر (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color == 0)
		popup_bg_rgb = fallback_bg_rgb;

	    اگر (!blend_fg)
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
		اگر (popup_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color =
			popup_aep->ae_u.cterm.fg_color;
		وگرنه اگر (cterm_normal_fg_color > 0)
		    new_en.ae_u.cterm.fg_color = cterm_normal_fg_color;
		وگرنه
		    // black-ish or white-ish
		    new_en.ae_u.cterm.fg_color = (*p_bg == 'l') ? 1 : 16;
		new_en.ae_u.cterm.ul_color = popup_aep->ae_u.cterm.ul_color;
		new_en.ae_u.cterm.font = popup_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		new_en.ae_u.cterm.ul_rgb = popup_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	    وگرنه
	    {
		// blend_fg=TRUE: fade underlying fg toward popup bg in
		// the 256-color palette.  Used when the popup is over a
		// cell rendered with cterm colors (no termguicolors RGB).
		صحیح under_fg = (char_aep != پوچ)
		    ? char_aep->ae_u.cterm.fg_color : 0;
		guicolor_T under_fg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		اگر (char_aep != پوچ)
		    under_fg_rgb = char_aep->ae_u.cterm.fg_rgb;
#endif
		new_en.ae_u.cterm.fg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_fg, under_fg_rgb, fallback_fg_rgb, blend);
	    }
	    // Approximate cterm bg by blending with the underlying bg
	    // in the 256-color palette and mapping to the nearest entry.
	    {
		صحیح under_bg = (char_aep != پوچ)
		    ? char_aep->ae_u.cterm.bg_color : 0;
		guicolor_T under_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		اگر (char_aep != پوچ)
		    under_bg_rgb = char_aep->ae_u.cterm.bg_rgb;
#endif
		new_en.ae_u.cterm.bg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_bg, under_bg_rgb, fallback_bg_rgb, blend);
	    }
#ifdef FEAT_TERMGUICOLORS
	    // Blend RGB colors for termguicolors mode.
	    {
		اگر (blend_fg)
		{
		    // blend_fg=TRUE: fade underlying text toward popup bg.
		    اگر (popup_bg_rgb != INVALCOLOR)
		    {
			صحیح base_fg = fallback_fg_rgb;
			// CTERMCOLOR is a sentinel meaning "use the cterm
			// color"; treat it as no underlying color so it is
			// not blended in as a real near-white pixel.
			اگر (char_aep != پوچ
				&& !COLOR_INVALID(char_aep->ae_u.cterm.fg_rgb))
			    base_fg = char_aep->ae_u.cterm.fg_rgb;
			new_en.ae_u.cterm.fg_rgb = blend_colors(
				popup_bg_rgb, base_fg, blend);
		    }
		}
		وگرنه
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

		    اگر (COLOR_INVALID(popup_fg)
			    && popup_aep->ae_u.cterm.fg_color > 0)
			popup_fg = cterm_color_to_rgb(
				popup_aep->ae_u.cterm.fg_color);

		    اگر (!COLOR_INVALID(popup_fg))
			new_en.ae_u.cterm.fg_rgb = popup_fg;
		    وگرنه اگر (!COLOR_INVALID(cterm_normal_fg_gui_color))
			new_en.ae_u.cterm.fg_rgb = cterm_normal_fg_gui_color;
		    وگرنه
			new_en.ae_u.cterm.fg_rgb = fallback_fg_rgb;
		}
		اگر (popup_bg_rgb != INVALCOLOR)
		{
		    // Blend popup bg toward underlying bg
		    guicolor_T underlying_bg = fallback_bg_rgb;
		    اگر (char_aep != پوچ
			    && !COLOR_INVALID(char_aep->ae_u.cterm.bg_rgb))
			underlying_bg = char_aep->ae_u.cterm.bg_rgb;
		    new_en.ae_u.cterm.bg_rgb = blend_colors(
			    popup_bg_rgb, underlying_bg, blend);
		}
	    }
#endif
	}
	بازگشت get_attr_entry(&cterm_attr_table, &new_en);
    }

    اگر (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    اگر (char_aep != پوچ)
	new_en = *char_aep;
    وگرنه
    {
	CLEAR_FIELD(new_en);
	اگر (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    // For term mode, no separate background color handling.
    بازگشت get_attr_entry(&term_attr_table, &new_en);
}

/*
 * Blend attributes for popup windows with opacity.
 */
    صحیح
hl_blend_attr(صحیح char_attr, صحیح popup_attr, صحیح blend, صحیح blend_fg)
{
    اگر (blend >= 100 && blend_fg)
	بازگشت char_attr;

    بازگشت hl_blend_attr_common(char_attr, popup_attr, blend, blend_fg);
}

/*
 * Blend underlying foreground and background colors for pum space cells.
 */
    صحیح
hl_pum_blend_attr(صحیح char_attr, صحیح popup_attr, صحیح blend)
{
    بازگشت hl_blend_attr_common(char_attr, popup_attr, blend, TRUE);
}

#ifdef FEAT_GUI
    attrentry_T *
syn_gui_attr2entry(صحیح attr)
{
    attr -= ATTR_OFF;
    اگر (attr >= gui_attr_table.ga_len)	    // did ":syntax clear"
	بازگشت پوچ;
    بازگشت &(GUI_ATTR_ENTRY(attr));
}
#endif

/*
 * Get the highlight attributes (HL_BOLD etc.) from an attribute nr.
 * Only to be used when "attr" > HL_ALL.
 */
    صحیح
syn_attr2attr(صحیح attr)
{
    attrentry_T	*aep;

#ifdef FEAT_GUI
    اگر (gui.in_use)
	aep = syn_gui_attr2entry(attr);
    وگرنه
#endif
	اگر (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	وگرنه
	    aep = syn_term_attr2entry(attr);

    اگر (aep == پوچ)	    // highlighting not set
	بازگشت 0;
    بازگشت aep->ae_attr;
}


    attrentry_T *
syn_term_attr2entry(صحیح attr)
{
    attr -= ATTR_OFF;
    اگر (attr >= term_attr_table.ga_len)	    // did ":syntax clear"
	بازگشت پوچ;
    بازگشت &(TERM_ATTR_ENTRY(attr));
}

    attrentry_T *
syn_cterm_attr2entry(صحیح attr)
{
    attr -= ATTR_OFF;
    اگر (attr >= cterm_attr_table.ga_len)	// did ":syntax clear"
	بازگشت پوچ;
    بازگشت &(CTERM_ATTR_ENTRY(attr));
}

#define LIST_ATTR   1
#define LIST_STRING 2
#define LIST_INT    3

    ایستا تهی
highlight_list_one(صحیح id)
{
    hl_group_T	    *sgp;
    صحیح		    didh = FALSE;

    sgp = &HL_TABLE()[id - 1];	    // index is ID minus one

    اگر (message_filtered(sgp->sg_name))
	بازگشت;

    // Note: Keep this in sync with expand_highlight_group().
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_term, پوچ, "term");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_start, "start");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_stop, "stop");

    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_cterm, پوچ, "cterm");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_fg, پوچ, "ctermfg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_bg, پوچ, "ctermbg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_ul, پوچ, "ctermul");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_font, پوچ, "ctermfont");

#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_gui, پوچ, "gui");
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

    اگر (sgp->sg_link && !got_int)
    {
	(تهی)syn_list_header(didh, 9999, id);
	didh = TRUE;
	پیام_puts_attr("links to", HL_ATTR(HLF_D));
	پیام_putchar(' ');
	پیام_outtrans(HL_TABLE()[HL_TABLE()[id - 1].sg_link - 1].sg_name);
    }

    اگر (!didh)
	highlight_list_arg(id, didh, LIST_STRING, 0, (char_u *)"cleared", "");
#ifdef FEAT_EVAL
    اگر (p_verbose > 0)
	last_set_پیام(sgp->sg_script_ctx);
#endif
}

    ایستا char_u*
highlight_arg_to_string(
    صحیح		type,
    صحیح		iarg,
    char_u	*sarg,
    char_u	*buf)
{
    اگر (type == LIST_INT)
	sprintf((نویسه *)buf, "%d", iarg - 1);
    وگرنه اگر (type == LIST_STRING)
	بازگشت sarg;
    وگرنه // type == LIST_ATTR
    {
	size_t buflen;

	buf[0] = NUL;
	buflen = 0;
	برای (صحیح i = 0; i < (صحیح)ARRAY_LENGTH(highlight_index_tab); ++i)
	{
	    اگر (iarg & highlight_index_tab[i]->کلید)
	    {
		اگر (buflen > 0)
		{
		    STRCPY(buf + buflen, (char_u *)",");
		    ++buflen;
		}
		STRCPY(buf + buflen, highlight_index_tab[i]->مقدار.string);
		buflen += highlight_index_tab[i]->مقدار.length;
		iarg &= ~highlight_index_tab[i]->کلید;	    // don't want "inverse"/"reverse"
	    }
	}
    }
    بازگشت buf;
}

    ایستا صحیح
highlight_list_arg(
    صحیح		id,
    صحیح		didh,
    صحیح		type,
    صحیح		iarg,
    char_u	*sarg,
    نویسه	*name)
{
    char_u	buf[MAX_ATTR_LEN];
    char_u	*ts;

    اگر (got_int)
	بازگشت FALSE;

    اگر (type == LIST_STRING ? (sarg == پوچ) : (iarg == 0))
	بازگشت didh;

    ts = highlight_arg_to_string(type, iarg, sarg, buf);

    (تهی)syn_list_header(didh,
	    (صحیح)(vim_strsize(ts) + STRLEN(name) + 1), id);
    didh = TRUE;
    اگر (!got_int)
    {
	اگر (*name != NUL)
	{
	    پیام_puts_attr(name, HL_ATTR(HLF_D));
	    پیام_puts_attr("=", HL_ATTR(HLF_D));
	}
	پیام_outtrans(ts);
    }
    بازگشت didh;
}

#اگر ((defined(FEAT_EVAL) || defined(FEAT_PRINTER))) && defined(FEAT_SYN_HL)
/*
 * Return "1" if highlight group "id" has attribute "flag".
 * Return NULL otherwise.
 */
    char_u *
highlight_has_attr(
    صحیح		id,
    صحیح		flag,
    صحیح		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    صحیح		attr;

    اگر (id <= 0 || id > highlight_ga.ga_len)
	بازگشت پوچ;

# اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
    اگر (modec == 'g')
	attr = HL_TABLE()[id - 1].sg_gui;
    وگرنه
# endif
    {
	اگر (modec == 'c')
	    attr = HL_TABLE()[id - 1].sg_cterm;
	وگرنه
	    attr = HL_TABLE()[id - 1].sg_term;
    }

    اگر (attr & flag)
	بازگشت (char_u *)"1";
    بازگشت پوچ;
}
#endif

#اگر defined(FEAT_SYN_HL) && defined(FEAT_EVAL)
/*
 * Return color name of highlight group "id".
 */
    char_u *
highlight_color(
    صحیح		id,
    char_u	*what,	// "font", "fg", "bg", "sp", "ul", "fg#", "bg#" or "sp#"
    صحیح		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    ایستا char_u	name[20];
    صحیح			n;
    صحیح			fg = FALSE;
    صحیح			sp = FALSE;
    صحیح			ul = FALSE;
    صحیح			font = FALSE;

    اگر (id <= 0 || id > highlight_ga.ga_len)
	بازگشت پوچ;

    اگر (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'g')
	fg = TRUE;
    وگرنه اگر (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'o'
	     && TOLOWER_ASC(what[2]) == 'n' && TOLOWER_ASC(what[3]) == 't')
	font = TRUE;
    وگرنه اگر (TOLOWER_ASC(what[0]) == 's' && TOLOWER_ASC(what[1]) == 'p')
	sp = TRUE;
    وگرنه اگر (TOLOWER_ASC(what[0]) == 'u' && TOLOWER_ASC(what[1]) == 'l')
	ul = TRUE;
    وگرنه اگر (!(TOLOWER_ASC(what[0]) == 'b' && TOLOWER_ASC(what[1]) == 'g'))
	بازگشت پوچ;
    اگر (modec == 'g')
    {
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI
	// return font name
	اگر (font)
	    بازگشت HL_TABLE()[id - 1].sg_font_name;
#  endif

	// return #RRGGBB form (only possible when GUI is running)
	اگر ((USE_24BIT) && what[2] == '#')
	{
	    guicolor_T		color;
	    long_u		rgb;
	    ایستا char_u	buf[10];

	    اگر (fg)
		color = HL_TABLE()[id - 1].sg_gui_fg;
	    وگرنه اگر (sp)
		color = HL_TABLE()[id - 1].sg_gui_sp;
	    وگرنه
		color = HL_TABLE()[id - 1].sg_gui_bg;
	    اگر (color == INVALCOLOR)
		بازگشت پوچ;
	    rgb = (long_u)GUI_MCH_GET_RGB(color);
	    sprintf((نویسه *)buf, "#%02x%02x%02x",
				      (unsigned)(rgb >> 16),
				      (unsigned)(rgb >> 8) & 255,
				      (unsigned)rgb & 255);
	    بازگشت buf;
	}
# endif
	اگر (fg)
	    بازگشت (HL_TABLE()[id - 1].sg_gui_fg_name);
	اگر (sp)
	    بازگشت (HL_TABLE()[id - 1].sg_gui_sp_name);
	بازگشت (HL_TABLE()[id - 1].sg_gui_bg_name);
    }
    اگر (sp)
	بازگشت پوچ;
    اگر (modec == 'c')
    {
	اگر (fg)
	    n = HL_TABLE()[id - 1].sg_cterm_fg - 1;
	وگرنه اگر (ul)
	    n = HL_TABLE()[id - 1].sg_cterm_ul - 1;
	وگرنه اگر (font)
	    n = HL_TABLE()[id - 1].sg_cterm_font - 1;
	وگرنه
	    n = HL_TABLE()[id - 1].sg_cterm_bg - 1;
	اگر (n < 0)
	    بازگشت پوچ;
	sprintf((نویسه *)name, "%d", n);
	بازگشت name;
    }
    // term doesn't have color
    بازگشت پوچ;
}
#endif

#اگر defined(FEAT_SYN_HL) \
	    && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)) \
	&& defined(FEAT_PRINTER)
/*
 * Return color name of highlight group "id" as RGB value.
 */
    long_u
highlight_gui_color_rgb(
    صحیح		id,
    صحیح		fg)	// TRUE = fg, FALSE = bg
{
    guicolor_T	color;

    اگر (id <= 0 || id > highlight_ga.ga_len)
	بازگشت 0L;

    اگر (fg)
	color = HL_TABLE()[id - 1].sg_gui_fg;
    وگرنه
	color = HL_TABLE()[id - 1].sg_gui_bg;

    اگر (color == INVALCOLOR)
	بازگشت 0L;

    بازگشت GUI_MCH_GET_RGB(color);
}
#endif

/*
 * Output the syntax list header.
 * Return TRUE when started a new line.
 */
    صحیح
syn_list_header(
    صحیح	    did_header,		// did header already
    صحیح	    outlen,		// length of string that comes
    صحیح	    id)			// highlight group id
{
    صحیح	    endcol = 19;
    صحیح	    newline = TRUE;
    صحیح	    name_col = 0;

    اگر (!did_header)
    {
	پیام_putchar('\n');
	اگر (got_int)
	    بازگشت TRUE;
	پیام_outtrans(HL_TABLE()[id - 1].sg_name);
	name_col = پیام_col;
	endcol = 15;
    }
    وگرنه اگر (پیام_col + outlen + 1 >= cmdline_width)
    {
	پیام_putchar('\n');
	اگر (got_int)
	    بازگشت TRUE;
    }
    وگرنه
    {
	اگر (پیام_col >= endcol)	// wrap around is like starting a new line
	    newline = FALSE;
    }

    اگر (پیام_col >= endcol)	// output at least one space
	endcol = پیام_col + 1;
    اگر (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    پیام_advance(endcol);

    // Show "xxx" with the attributes.
    اگر (!did_header)
    {
	اگر (endcol == cmdline_width - 1 && endcol <= name_col)
	    پیام_putchar(' ');
	پیام_puts_attr("xxx", syn_id2attr(id));
	پیام_putchar(' ');
    }

    بازگشت newline;
}

/*
 * Set the attribute numbers for a highlight group.
 * Called after one of the attributes has changed.
 */
    ایستا تهی
set_hl_attr(
    صحیح		idx)	    // index in array
{
    attrentry_T	    at_en;
    hl_group_T	    *sgp = HL_TABLE() + idx;

    // The "Normal" group doesn't need an attribute number
    اگر (sgp->sg_name_u != پوچ && STRCMP(sgp->sg_name_u, "NORMAL") == 0)
	بازگشت;

#ifdef FEAT_GUI
    // For the GUI mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    اگر (sgp->sg_gui_fg == INVALCOLOR
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
    وگرنه
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
    اگر (sgp->sg_start == پوچ && sgp->sg_stop == پوچ)
	sgp->sg_term_attr = sgp->sg_term;
    وگرنه
    {
	at_en.ae_attr = sgp->sg_term;
	at_en.ae_u.term.start = sgp->sg_start;
	at_en.ae_u.term.stop = sgp->sg_stop;
	sgp->sg_term_attr = get_attr_entry(&term_attr_table, &at_en);
    }

    // For the color term mode: If there are other than "normal"
    // highlighting attributes, need to allocate an attr number.
    اگر (sgp->sg_cterm_fg == 0 && sgp->sg_cterm_bg == 0 &&
	sgp->sg_cterm_ul == 0 && sgp->sg_cterm_font == 0
#ifdef FEAT_TERMGUICOLORS
	    && sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
#endif
	    )
	sgp->sg_cterm_attr = sgp->sg_cterm;
    وگرنه
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
	اگر (sgp->sg_cterm & (HL_UNDERLINE | HL_UNDERCURL
			   | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED))
	    at_en.ae_u.cterm.ul_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_sp);
	وگرنه
	    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	اگر (at_en.ae_u.cterm.fg_rgb == INVALCOLOR
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
    ایستا صحیح
syn_override(صحیح id)
{
    اگر (overrides != پوچ && overrides->arr != پوچ)
	برای (صحیح k = 0; k < overrides->len; k++)
	    اگر (overrides->arr[k].from == id)
		بازگشت overrides->arr[k].to;
    بازگشت id;
}

/*
 * Lookup a highlight group name by pointer and length.
 * If it is not found, 0 is returned.
 */
    ایستا صحیح
syn_name2id_len(char_u *name, صحیح len)
{
    char_u	name_u[MAX_SYN_NAME + 1];
    hashitem_T	*hi;

    اگر (len > MAX_SYN_NAME)
	len = MAX_SYN_NAME;
    mch_memmove(name_u, name, len);
    name_u[len] = NUL;
    vim_strup(name_u);
    اگر (!highlight_ht_inited)
	بازگشت 0;
    hi = hash_find(&highlight_ht, name_u);
    اگر (HASHITEM_EMPTY(hi))
	بازگشت 0;
    بازگشت HI2HLNAME(hi)->hn_id;
}

/*
 * Lookup a highlight group name and return its ID.
 * If it is not found, 0 is returned.
 */
    صحیح
syn_name2id(char_u *name)
{
    // Avoid using stricmp() too much, it's slow on some systems.
    بازگشت syn_name2id_len(name, (صحیح)STRLEN(name));
}

/*
 * Lookup a highlight group name and return its attributes.
 * Return zero if not found.
 */
    صحیح
syn_name2attr(char_u *name)
{
    صحیح id = syn_name2id(name);

    اگر (id != 0)
	بازگشت syn_id2attr(id);
    بازگشت 0;
}

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE if highlight group "name" exists.
 */
    صحیح
highlight_exists(char_u *name)
{
    بازگشت (syn_name2id(name) > 0);
}

# اگر defined(FEAT_جستجو_EXTRA)
/*
 * Return the name of highlight group "id".
 * When not a valid ID return an empty string.
 */
    char_u *
syn_id2name(صحیح id)
{
    اگر (id <= 0 || id > highlight_ga.ga_len)
	بازگشت (char_u *)"";
    بازگشت HL_TABLE()[id - 1].sg_name;
}
# endif
#endif

/*
 * Like syn_name2id(), but take a pointer + length argument.
 */
    صحیح
syn_namen2id(char_u *linep, صحیح len)
{
    بازگشت syn_name2id_len(linep, len);
}

/*
 * Find highlight group name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    صحیح
syn_check_group(char_u *pp, صحیح len)
{
    صحیح	    id;
    char_u  *name;

    اگر (len > MAX_SYN_NAME)
    {
	emsg(_(e_highlight_group_name_too_long));
	بازگشت 0;
    }
    id = syn_name2id_len(pp, len);
    اگر (id == 0)			// doesn't exist yet
    {
	name = vim_strnsave(pp, len);
	اگر (name == پوچ)
	    بازگشت 0;
	id = syn_add_group(name);
    }
    بازگشت id;
}

/*
 * Add new highlight group and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    ایستا صحیح
syn_add_group(char_u *name)
{
    char_u	*p;
    char_u	*name_up;

    // Check that the name is valid (ASCII letters, digits, underscores, dots, or hyphens).
    برای (p = name; *p != NUL; ++p)
    {
	اگر (!vim_isprintc(*p))
	{
	    emsg(_(e_unprintable_character_in_group_name));
	    vim_free(name);
	    بازگشت 0;
	}
	وگرنه اگر (!ASCII_ISALNUM(*p) && *p != '_' && *p != '.' && *p != '-')
	{
	    // This is an error, but since there previously was no check only
	    // give a warning.
	    پیام_source(HL_ATTR(HLF_W));
	    پیام(_("W18: Invalid character in group name"));
	    بشکن;
	}
    }

    // First call for this growarray: init growing array.
    اگر (highlight_ga.ga_data == پوچ)
    {
	highlight_ga.ga_itemsize = اندازه(hl_group_T);
	highlight_ga.ga_growsize = 10;
	hash_init(&highlight_ht);
	highlight_ht_inited = true;
    }

    اگر (highlight_ga.ga_len >= MAX_HL_ID)
    {
	emsg(_(e_too_many_highlight_and_syntax_groups));
	vim_free(name);
	بازگشت 0;
    }

    // Make room for at least one other syntax_highlight entry.
    اگر (ga_grow(&highlight_ga, 1) == FAIL)
    {
	vim_free(name);
	بازگشت 0;
    }

    {
	hlname_T    *hn;
	صحیح	    len = (صحیح)STRLEN(name);

	hn = alloc(offsetof(hlname_T, hn_کلید) + len + 1);
	اگر (hn == پوچ)
	{
	    vim_free(name);
	    بازگشت 0;
	}
	vim_strncpy(hn->hn_کلید, name, len);
	vim_strup(hn->hn_کلید);
	hn->hn_id = highlight_ga.ga_len + 1;	// ID is index plus one
	name_up = hn->hn_کلید;
    }

    CLEAR_POINTER(&(HL_TABLE()[highlight_ga.ga_len]));
    HL_TABLE()[highlight_ga.ga_len].sg_name = name;
    HL_TABLE()[highlight_ga.ga_len].sg_name_u = name_up;
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_TABLE()[highlight_ga.ga_len].sg_gui_bg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_fg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_sp = INVALCOLOR;
#endif
    hash_add(&highlight_ht, name_up, "highlight");
    ++highlight_ga.ga_len;

    بازگشت highlight_ga.ga_len;		    // ID is index plus one
}

/*
 * When, just after calling syn_add_group(), an error is discovered, this
 * function deletes the new name.
 */
    ایستا تهی
syn_unadd_group(تهی)
{
    hashitem_T *hi;

    --highlight_ga.ga_len;
    hi = hash_find(&highlight_ht, HL_TABLE()[highlight_ga.ga_len].sg_name_u);
    اگر (!HASHITEM_EMPTY(hi))
	hash_remove(&highlight_ht, hi, "highlight");
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name);
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name_u - HLNAME_کلید_OFF);
}

/*
 * Translate a group ID to highlight attributes.
 * "hl_id" must be valid: > 0, caller must check.
 */
    صحیح
syn_id2attr(صحیح hl_id)
{
    صحیح		attr;
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    أكد(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

#ifdef FEAT_GUI
    // Only use GUI attr when the GUI is being used.
    اگر (gui.in_use)
	attr = sgp->sg_gui_attr;
    وگرنه
#endif
	اگر (IS_CTERM)
	    attr = sgp->sg_cterm_attr;
	وگرنه
	    attr = sgp->sg_term_attr;

    بازگشت attr;
}

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Get the GUI colors and attributes for a group ID.
 * NOTE: the colors will be INVALCOLOR when not set, the color otherwise.
 */
    صحیح
syn_id2colors(صحیح hl_id, guicolor_T *fgp, guicolor_T *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    أكد(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

    *fgp = sgp->sg_gui_fg;
    *bgp = sgp->sg_gui_bg;
    بازگشت sgp->sg_gui;
}
#endif

#اگر (defined(MSWIN) \
	    && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL)) \
	    && defined(FEAT_TERMGUICOLORS)) \
	|| defined(FEAT_TERMINAL)
    تهی
syn_id2cterm_bg(صحیح hl_id, صحیح *fgp, صحیح *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    أكد(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
    *fgp = sgp->sg_cterm_fg - 1;
    *bgp = sgp->sg_cterm_bg - 1;
}
#endif

/*
 * Translate a group ID to the final group ID (following links).
 */
    صحیح
syn_get_final_id(صحیح hl_id)
{
    صحیح		شمار;
    hl_group_T	*sgp;

    اگر (hl_id > highlight_ga.ga_len || hl_id < 1)
	بازگشت 0;			// Can be called from eval!!

    // Follow links until there is no more.
    // Look out for loops!  Break after 100 links.
    برای (شمار = 100; --شمار >= 0; )
    {
	صحیح tmp = hl_id;

	sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
	اگر (sgp->sg_link == 0 || sgp->sg_link > highlight_ga.ga_len)
	    بشکن;

	// This is to handle highlight groups that are overridden but are in the
	// middle of a link chain.
	hl_id = syn_override(hl_id);
	اگر (tmp != hl_id)
	    ادامه;
	hl_id = sgp->sg_link;
    }
    بازگشت syn_override(hl_id);
}

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Call this function just after the GUI has started.
 * Also called when 'termguicolors' was set, gui.in_use will be FALSE then.
 * It finds the font and color handles for the highlighting groups.
 */
    تهی
highlight_gui_started(تهی)
{
    صحیح	    idx;

    // First get the colors from the "Normal" and "Menu" group, if set
    اگر (USE_24BIT)
	set_normal_colors();

    برای (idx = 0; idx < highlight_ga.ga_len; ++idx)
	gui_do_one_color(idx, FALSE, FALSE);

    highlight_changed();
}

    ایستا تهی
gui_do_one_color(
    صحیح		idx,
    صحیح		do_menu UNUSED,	   // TRUE: might set the menu font
    صحیح		do_tooltip UNUSED) // TRUE: might set the tooltip font
{
    صحیح		didit = FALSE;

# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    اگر (gui.in_use)
#  endif
	اگر (HL_TABLE()[idx].sg_font_name != پوچ)
	{
	    hl_do_font(idx, HL_TABLE()[idx].sg_font_name, FALSE, do_menu,
							    do_tooltip, TRUE);
	    didit = TRUE;
	}
# endif
    اگر (HL_TABLE()[idx].sg_gui_fg_name != پوچ)
    {
	HL_TABLE()[idx].sg_gui_fg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_fg_name);
	didit = TRUE;
    }
    اگر (HL_TABLE()[idx].sg_gui_bg_name != پوچ)
    {
	HL_TABLE()[idx].sg_gui_bg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_bg_name);
	didit = TRUE;
    }
    اگر (HL_TABLE()[idx].sg_gui_sp_name != پوچ)
    {
	HL_TABLE()[idx].sg_gui_sp =
			    color_name2handle(HL_TABLE()[idx].sg_gui_sp_name);
	didit = TRUE;
    }
    اگر (didit)	// need to get a new attr number
	set_hl_attr(idx);
}
#endif

#اگر defined(USER_HIGHLIGHT) && defined(FEAT_STL_OPT)
/*
 * Apply difference between User[1-9] and HLF_S to HLF_SNC, HLF_ST or HLF_STNC.
 */
    ایستا تهی
combine_stl_hlt(
	صحیح id,
	صحیح id_S,
	صحیح id_alt,
	صحیح hlcnt,
	صحیح i,
	صحیح hlf,
	صحیح *table)
{
    hl_group_T *hlt = HL_TABLE();

    اگر (id_alt == 0)
    {
	CLEAR_POINTER(&hlt[hlcnt + i]);
	hlt[hlcnt + i].sg_term = highlight_attr[hlf];
	hlt[hlcnt + i].sg_cterm = highlight_attr[hlf];
# اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
	hlt[hlcnt + i].sg_gui = highlight_attr[hlf];
# endif
    }
    وگرنه
	mch_memmove(&hlt[hlcnt + i],
		    &hlt[id_alt - 1],
		    اندازه(hl_group_T));
    hlt[hlcnt + i].sg_link = 0;

    hlt[hlcnt + i].sg_term ^=
	hlt[id - 1].sg_term ^ hlt[id_S - 1].sg_term;
    اگر (hlt[id - 1].sg_start != hlt[id_S - 1].sg_start)
	hlt[hlcnt + i].sg_start = hlt[id - 1].sg_start;
    اگر (hlt[id - 1].sg_stop != hlt[id_S - 1].sg_stop)
	hlt[hlcnt + i].sg_stop = hlt[id - 1].sg_stop;
    hlt[hlcnt + i].sg_cterm ^=
	hlt[id - 1].sg_cterm ^ hlt[id_S - 1].sg_cterm;
    اگر (hlt[id - 1].sg_cterm_fg != hlt[id_S - 1].sg_cterm_fg)
	hlt[hlcnt + i].sg_cterm_fg = hlt[id - 1].sg_cterm_fg;
    اگر (hlt[id - 1].sg_cterm_bg != hlt[id_S - 1].sg_cterm_bg)
	hlt[hlcnt + i].sg_cterm_bg = hlt[id - 1].sg_cterm_bg;
    اگر (hlt[id - 1].sg_cterm_font != hlt[id_S - 1].sg_cterm_font)
	hlt[hlcnt + i].sg_cterm_font = hlt[id - 1].sg_cterm_font;
# اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
    hlt[hlcnt + i].sg_gui ^=
	hlt[id - 1].sg_gui ^ hlt[id_S - 1].sg_gui;
# endif
# ifdef FEAT_GUI
    اگر (hlt[id - 1].sg_gui_fg != hlt[id_S - 1].sg_gui_fg)
	hlt[hlcnt + i].sg_gui_fg = hlt[id - 1].sg_gui_fg;
    اگر (hlt[id - 1].sg_gui_bg != hlt[id_S - 1].sg_gui_bg)
	hlt[hlcnt + i].sg_gui_bg = hlt[id - 1].sg_gui_bg;
    اگر (hlt[id - 1].sg_gui_sp != hlt[id_S - 1].sg_gui_sp)
	hlt[hlcnt + i].sg_gui_sp = hlt[id - 1].sg_gui_sp;
    اگر (hlt[id - 1].sg_font != hlt[id_S - 1].sg_font)
	hlt[hlcnt + i].sg_font = hlt[id - 1].sg_font;
#  ifdef FEAT_XFONTSET
    اگر (hlt[id - 1].sg_fontset != hlt[id_S - 1].sg_fontset)
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
    صحیح
highlight_changed(تهی)
{
    صحیح		hlf;
    صحیح		i;
    char_u	*p;
    char_u	*default_hl;
    صحیح		attr;
    char_u	*end;
    صحیح		id;
#ifdef USER_HIGHLIGHT
    char_u      userhl[30];  // use 30 to avoid compiler warning
# ifdef FEAT_STL_OPT
    صحیح		id_S = -1;
    صحیح		id_SNC = 0;
#  ifdef FEAT_TERMINAL
    صحیح		id_ST = 0;
    صحیح		id_STNC = 0;
#  endif
    صحیح		hlcnt;
# endif
#endif
    win_T	*wp;

    need_highlight_changed = FALSE;

#ifdef FEAT_TERMINAL
    term_update_colors_all();
    term_update_hlfwin_all();
#endif

    // Clear all attributes.
    برای (hlf = 0; hlf < (صحیح)HLF_شمار; ++hlf)
    {
	highlight_attr[hlf] = 0;
	highlight_attr_raw[hlf] = 0;
	highlight_ids[hlf] = 0;
    }

    default_hl = get_highlight_default();

    // First set all attributes to their default value.
    // Then use the attributes from the 'highlight' option, unless it already
    // matches the default and would repeat the same work.
    برای (i = 0; i < 2; ++i)
    {
	اگر (i)
	{
	    اگر (default_hl != پوچ && p_hl != پوچ
				      && STRCMP(default_hl, p_hl) == 0)
		ادامه;
	    p = p_hl;
	}
	وگرنه
	    p = default_hl;
	اگر (p == پوچ)	    // just in case
	    ادامه;

	وقتی (*p)
	{
	    برای (hlf = 0; hlf < (صحیح)HLF_شمار; ++hlf)
		اگر (hl_flags[hlf] == *p)
		    بشکن;
	    ++p;
	    اگر (hlf == (صحیح)HLF_شمار || *p == NUL)
		بازگشت FAIL;

	    // Allow several hl_flags to be combined, like "bu" for
	    // bold-underlined.
	    attr = 0;
	    id = 0;
	    برای ( ; *p && *p != ','; ++p)	    // parse up to comma
	    {
		اگر (VIM_ISWHITE(*p))		    // ignore white space
		    ادامه;

		اگر (attr > HL_ALL)  // Combination with ':' is not allowed.
		    بازگشت FAIL;

		// Note: Keep this in sync with expand_set_highlight().
		انتخاب (*p)
		{
		    حالت 'b':	attr |= HL_BOLD;
				بشکن;
		    حالت 'i':	attr |= HL_ITALIC;
				بشکن;
		    حالت '-':
		    حالت 'n':			    // no highlighting
				بشکن;
		    حالت 'r':	attr |= HL_INVERSE;
				بشکن;
		    حالت 's':	attr |= HL_STANDOUT;
				بشکن;
		    حالت 'u':	attr |= HL_UNDERLINE;
				بشکن;
		    حالت 'c':	attr |= HL_UNDERCURL;
				بشکن;
		    حالت '2':	attr |= HL_UNDERDOUBLE;
				بشکن;
		    حالت 'd':	attr |= HL_UNDERDOTTED;
				بشکن;
		    حالت '=':	attr |= HL_UNDERDASHED;
				بشکن;
		    حالت 't':	attr |= HL_STRIKETHROUGH;
				بشکن;
		    حالت ':':	++p;		    // highlight group name
				اگر (attr || *p == NUL)	 // no combinations
				    بازگشت FAIL;
				end = vim_strchr(p, ',');
				اگر (end == پوچ)
				    end = p + STRLEN(p);
				id = syn_check_group(p, (صحیح)(end - p));
				اگر (id == 0)
				    بازگشت FAIL;
				attr = syn_id2attr(id);
				p = end - 1;
#اگر defined(FEAT_STL_OPT) && defined(USER_HIGHLIGHT)
				اگر (hlf == (صحیح)HLF_SNC)
				    id_SNC = syn_get_final_id(id);
# ifdef FEAT_TERMINAL
				وگرنه اگر (hlf == (صحیح)HLF_ST)
				    id_ST = syn_get_final_id(id);
				وگرنه اگر (hlf == (صحیح)HLF_STNC)
				    id_STNC = syn_get_final_id(id);
# endif
				وگرنه اگر (hlf == (صحیح)HLF_S)
				    id_S = syn_get_final_id(id);
#endif
				بشکن;
		    پیشفرض:	بازگشت FAIL;
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
	نویسه *errmsg = update_winhighlight(wp, wp->w_p_whl);

	اگر (errmsg != پوچ)
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
    اگر (ga_grow(&highlight_ga, 28) == FAIL)
	بازگشت FAIL;
    hlcnt = highlight_ga.ga_len;
    اگر (id_S == -1)
    {
	// Make sure id_S is always valid to simplify code below. Use the last
	// entry.
	CLEAR_POINTER(&HL_TABLE()[hlcnt + 27]);
	HL_TABLE()[hlcnt + 18].sg_term = highlight_attr[HLF_S];
	id_S = hlcnt + 19;
    }
# endif
    برای (i = 0; i < 9; i++)
    {
	sprintf((نویسه *)userhl, "User%d", i + 1);
	id = syn_name2id(userhl);
	اگر (id == 0)
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
	وگرنه
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

    بازگشت OK;
}

ایستا تهی highlight_list(تهی);
ایستا تهی highlight_list_two(صحیح cnt, صحیح attr);

// context for :highlight <group> <arg> expansion
ایستا صحیح expand_hi_synid = 0;	    // ID for highlight group being completed
ایستا صحیح expand_hi_equal_col = 0; // column where the '=' is
ایستا صحیح expand_hi_include_orig = 0;	    // whether to fill the existing current value or not
ایستا char_u *expand_hi_curvalue = پوچ;   // the existing current value
#اگر defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
ایستا dict_iterator_T expand_colornames_iter;	// iterator for looping through v:colornames
#endif

/*
 * Handle command line completion for :highlight command.
 */
    تهی
set_context_in_highlight_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;
    صحیح		expand_group = TRUE;

    // Default: expand group names
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 0;
    include_link = 2;
    include_default = 1;

    اگر (*arg == NUL)
	بازگشت;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    اگر (*p == NUL)
	بازگشت;

    // past "default" or group name
    include_default = 0;
    اگر (STRNCMP("default", arg, p - arg) == 0)
    {
	arg = skipwhite(p);
	xp->xp_pattern = arg;
	p = skiptowhite(arg);
    }
    اگر (*p == NUL)
	بازگشت;

    // past group name
    include_link = 0;
    اگر (arg[1] == 'i' && arg[0] == 'N')
    {
	highlight_list();
	expand_group = FALSE;
    }
    اگر (STRNCMP("link", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	اگر (*p != NUL)		// past first group name
	{
	    xp->xp_pattern = skipwhite(p);
	    p = skiptowhite(xp->xp_pattern);
	    include_none = 1;
	}
	expand_group = FALSE;
    }
    وگرنه اگر (STRNCMP("clear", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	expand_group = FALSE;
    }
    اگر (*p != NUL)			// past group name(s)
    {
	اگر (expand_group)
	{
	    // expansion will be done in expand_highlight_group()
	    xp->xp_context = EXPAND_HIGHLIGHT_GROUP;

	    expand_hi_synid = syn_namen2id(arg, (صحیح)(p - arg));

	    وقتی (*p != NUL)
	    {
		arg = skipwhite(p);
		p = skiptowhite(arg);
	    }

	    p = vim_strchr(arg, '=');
	    اگر (p == پوچ)
	    {
		// Didn't find a key=<value> pattern
		xp->xp_pattern = arg;
		expand_hi_equal_col = -1;
		expand_hi_include_orig = FALSE;
	    }
	    وگرنه
	    {
		// Found key=<value> pattern, record the exact location
		expand_hi_equal_col = (صحیح)(p - xp->xp_سطر);

		// Only include the original value if the pattern is empty
		اگر (*(p + 1) == NUL)
		    expand_hi_include_orig = TRUE;
		وگرنه
		    expand_hi_include_orig = FALSE;

		// Account for comma-separated values
		اگر (STRNCMP(arg, "term=", 5) == 0 ||
			STRNCMP(arg, "cterm=", 6) == 0 ||
			STRNCMP(arg, "gui=", 4) == 0)
		{
		    char_u *comma = vim_strrchr(p + 1, ',');
		    اگر (comma != پوچ)
			p = comma;
		}
		xp->xp_pattern = p + 1;
	    }
	}
	وگرنه
	{
	    xp->xp_context = EXPAND_NOTHING;
	}
    }
}

/*
 * List highlighting matches in a nice way.
 */
    ایستا تهی
highlight_list(تهی)
{
    صحیح		i;

    برای (i = 10; --i >= 0; )
	highlight_list_two(i, HL_ATTR(HLF_D));
    برای (i = 40; --i >= 0; )
	highlight_list_two(99, 0);
}

    ایستا تهی
highlight_list_two(صحیح cnt, صحیح attr)
{
    پیام_puts_attr(&("N \bI \b!  \b"[cnt / 11]), attr);
    پیام_clr_eos();
    out_flush();
    ui_delay(cnt == 99 ? 40L : (long)cnt * 50L, FALSE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_highlight_name(expand_T *xp UNUSED, صحیح idx)
{
    بازگشت get_highlight_name_ext(xp, idx, TRUE);
}

/*
 * Obtain a highlight group name.
 * When "skip_cleared" is TRUE don't return a cleared entry.
 */
    char_u *
get_highlight_name_ext(expand_T *xp UNUSED, صحیح idx, صحیح skip_cleared)
{
    اگر (idx < 0)
	بازگشت پوچ;

    // Items are never removed from the table, skip the ones that were
    // cleared.
    اگر (skip_cleared && idx < highlight_ga.ga_len && HL_TABLE()[idx].sg_cleared)
	بازگشت (char_u *)"";

    اگر (idx == highlight_ga.ga_len && include_none != 0)
	بازگشت (char_u *)"NONE";
    اگر (idx == highlight_ga.ga_len + include_none && include_default != 0)
	بازگشت (char_u *)"default";
    اگر (idx == highlight_ga.ga_len + include_none + include_default
							 && include_link != 0)
	بازگشت (char_u *)"link";
    اگر (idx == highlight_ga.ga_len + include_none + include_default + 1
							 && include_link != 0)
	بازگشت (char_u *)"clear";
    اگر (idx >= highlight_ga.ga_len)
	بازگشت پوچ;
    بازگشت HL_TABLE()[idx].sg_name;
}

    ایستا char_u *
get_highlight_attr_name(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx == 0)
    {
	// Fill with current value first
	اگر (expand_hi_curvalue != پوچ)
	    بازگشت expand_hi_curvalue;
	وگرنه
	    بازگشت (char_u*)"";
    }
    اگر (idx < (صحیح)ARRAY_LENGTH(highlight_index_tab) + 1)
    {
	char_u *مقدار = highlight_index_tab[idx-1]->مقدار.string;
	اگر (expand_hi_curvalue != پوچ && STRCMP(expand_hi_curvalue, مقدار) == 0)
	{
	    // Already returned the current value above, just skip.
	    بازگشت (char_u*)"";
	}
	بازگشت مقدار;
    }
    بازگشت پوچ;
}

    ایستا char_u *
get_highlight_cterm_color(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx == 0)
    {
	// Fill with current value first
	اگر (expand_hi_curvalue != پوچ)
	    بازگشت expand_hi_curvalue;
	وگرنه
	    بازگشت (char_u*)"";
    }
    // See highlight_set_cterm_color()
    وگرنه اگر (idx == 1)
	بازگشت (char_u*)"fg";
    وگرنه اگر (idx == 2)
	بازگشت (char_u*)"bg";
    اگر (idx < (صحیح)ARRAY_LENGTH(color_name_tab) + 3)
    {
	char_u *مقدار = color_name_tab[idx-3].مقدار.string;
	بازگشت مقدار;
    }
    بازگشت پوچ;
}

#اگر defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
    ایستا char_u *
get_highlight_gui_color(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx == 0)
    {
	// Fill with current value first
	اگر (expand_hi_curvalue != پوچ)
	    بازگشت expand_hi_curvalue;
	وگرنه
	    بازگشت (char_u*)"";
    }
    // See color_name2handle()
    وگرنه اگر (idx == 1)
	بازگشت (char_u*)"fg";
    وگرنه اگر (idx == 2)
	بازگشت (char_u*)"bg";
    وگرنه اگر (idx == 3)
	بازگشت (char_u*)"NONE";

    // Complete from v:colornames. Don't do platform specific names for now.
    typval_T *tv_result;
    char_u *colorname = dict_iterate_next(&expand_colornames_iter, &tv_result);
    اگر (colorname != پوچ)
    {
	// :hi command doesn't allow space, so don't suggest any malformed items
	اگر (vim_strchr(colorname, ' ') != پوچ)
	    بازگشت (char_u*)"";

	اگر (expand_hi_curvalue != پوچ && STRICMP(expand_hi_curvalue, colorname) == 0)
	{
	    // Already returned the current value above, just skip.
	    بازگشت (char_u*)"";
	}
    }
    بازگشت colorname;
}
#endif

    ایستا char_u *
get_highlight_group_کلید(expand_T *xp UNUSED, صحیح idx)
{
    // Note: Keep this in sync with do_highlight.
    ایستا نویسه *(p_hi_group_کلید_مقادیر[]) =
    {
	"term=",
	"start=",
	"stop=",
	"cterm=",
	"ctermfg=",
	"ctermbg=",
	"ctermul=",
	"ctermfont=",
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

    اگر (idx < (صحیح)ARRAY_LENGTH(p_hi_group_کلید_مقادیر))
	بازگشت (char_u*)p_hi_group_کلید_مقادیر[idx];
    بازگشت پوچ;
}

/*
 * Command-line expansion for :hi {group-name} <args>...
 */
    صحیح
expand_highlight_group(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	صحیح	    *رقم_Matches)
{
    اگر (expand_hi_equal_col != -1)
    {
	// List the values. First fill in the current value, then if possible colors
	// or attribute names.
	char_u	    *(*expandfunc)(expand_T *, صحیح) = پوچ;
	صحیح	    type = 0;
	hl_group_T  *sgp = پوچ;
	صحیح	    iarg = 0;
	char_u	    *sarg = پوچ;

	صحیح	    unsorted_اقلام = -1; // don't sort by default

	اگر (expand_hi_synid != 0)
	    sgp = &HL_TABLE()[expand_hi_synid - 1]; // index is ID minus one

	// Note: Keep this in sync with highlight_list_one().
	char_u	    *name_end = xp->xp_سطر + expand_hi_equal_col;
	اگر (name_end - xp->xp_سطر >= 5
		&& STRNCMP(name_end - 5, " term", 5) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    اگر (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_term;
	    }
	}
	وگرنه اگر (name_end - xp->xp_سطر >= 6
		&& STRNCMP(name_end - 6, " cterm", 6) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    اگر (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_cterm;
	    }
	}
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	وگرنه اگر (name_end - xp->xp_سطر >= 4
		&& STRNCMP(name_end - 4, " gui", 4) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    اگر (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_gui;
	    }
	}
#endif
	وگرنه اگر (name_end - xp->xp_سطر >= 8
		&& STRNCMP(name_end - 8, " ctermfg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    اگر (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_fg;
	    }
	}
	وگرنه اگر (name_end - xp->xp_سطر >= 8
		&& STRNCMP(name_end - 8, " ctermbg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    اگر (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_bg;
	    }
	}
	وگرنه اگر (name_end - xp->xp_سطر >= 8
		&& STRNCMP(name_end - 8, " ctermul", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    اگر (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_ul;
	    }
	}
#اگر defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	وگرنه اگر (name_end - xp->xp_سطر >= 6
		&& STRNCMP(name_end - 6, " guifg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    اگر (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_fg_name;
	    }
	}
	وگرنه اگر (name_end - xp->xp_سطر >= 6
		&& STRNCMP(name_end - 6, " guibg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    اگر (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_bg_name;
	    }
	}
	وگرنه اگر (name_end - xp->xp_سطر >= 6
		&& STRNCMP(name_end - 6, " guisp", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    اگر (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_sp_name;
	    }
	}
#endif

#اگر defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	اگر (expandfunc == get_highlight_gui_color)
	{
	    // Top 4 items are special, after that sort all the color names
	    unsorted_اقلام = 4;

	    dict_T *colornames_table = get_vim_var_dict(VV_COLORNAMES);
	    typval_T colornames_val;
	    colornames_val.v_type = VAR_DICT;
	    colornames_val.vval.v_dict = colornames_table;
	    dict_iterate_start(&colornames_val, &expand_colornames_iter);
	}
#endif

	char_u	    buf[MAX_ATTR_LEN];

	expand_hi_curvalue = پوچ;
	اگر (expand_hi_include_orig)
	{
	    اگر (((type == LIST_ATTR || type == LIST_INT) && iarg != 0) ||
		(type == LIST_STRING && sarg != پوچ))
	    {
		// Retrieve the current value to go first in completion
		expand_hi_curvalue = highlight_arg_to_string(
			type, iarg, sarg, buf);
	    }
	}

	اگر (expandfunc != پوچ)
	{
	    بازگشت ExpandGenericExt(
		    pat,
		    xp,
		    rmp,
		    matches,
		    رقم_Matches,
		    expandfunc,
		    FALSE,
		    unsorted_اقلام);
	}

	بازگشت FAIL;
    }

    // List all the key names
    بازگشت ExpandGenericExt(
	    pat,
	    xp,
	    rmp,
	    matches,
	    رقم_Matches,
	    get_highlight_group_کلید,
	    FALSE,
	    -1);
}

#اگر defined(FEAT_GUI)
/*
 * Free all the highlight group fonts.
 * Used when quitting for systems which need it.
 */
    تهی
free_highlight_fonts(تهی)
{
    صحیح	    idx;

    برای (idx = 0; idx < highlight_ga.ga_len; ++idx)
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

#اگر defined(FEAT_EVAL)
/*
 * Convert each of the highlight attribute bits (bold, standout, underline,
 * etc.) set in 'hlattr' into a separate boolean item in a Dictionary with
 * the attribute name as the key.
 */
    ایستا dict_T *
highlight_get_attr_dict(صحیح hlattr)
{
    dict_T	*dict;
    صحیح		i;

    dict = dict_alloc();
    اگر (dict == پوچ)
	بازگشت پوچ;

    برای (i = 0; i < (صحیح)ARRAY_LENGTH(highlight_index_tab); ++i)
    {
	اگر (hlattr & highlight_index_tab[i]->کلید)
	{
	    dict_add_bool(dict, (نویسه *)highlight_index_tab[i]->مقدار.string,
		    VVAL_TRUE);
	    // don't want "inverse"/"reverse"
	    hlattr &= ~highlight_index_tab[i]->کلید;
	}
    }

    بازگشت dict;
}

/*
 * Return the attributes of the highlight group at index 'hl_idx' as a
 * Dictionary. If 'resolve_link' is TRUE, then resolves the highlight group
 * links recursively.
 */
    ایستا dict_T *
highlight_get_info(صحیح hl_idx, صحیح resolve_link)
{
    dict_T	*dict;
    hl_group_T	*sgp;
    dict_T	*attr_dict = پوچ;
    صحیح		hlgid;

    dict = dict_alloc();
    اگر (dict == پوچ)
	بازگشت dict;

    sgp = &HL_TABLE()[hl_idx];
    // highlight group id is 1-based
    hlgid = hl_idx + 1;

    اگر (dict_add_string(dict, "name", sgp->sg_name) == FAIL)
	برو error;
    اگر (dict_add_عدد(dict, "id", hlgid) == FAIL)
	برو error;

    اگر (resolve_link)
    {
	// resolve the highlight group link recursively
	وقتی (sgp->sg_link)
	{
	    hlgid = sgp->sg_link;
	    sgp = &HL_TABLE()[sgp->sg_link - 1];
	}
    }

    اگر (sgp->sg_term != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_term);
	اگر (attr_dict != پوچ)
	{
	    اگر (dict_add_dict(dict, "term", attr_dict) == FAIL)
		برو error;
	    attr_dict = پوچ;
	}
    }
    اگر (sgp->sg_start != پوچ)
	اگر (dict_add_string(dict, "start", sgp->sg_start) == FAIL)
	    برو error;
    اگر (sgp->sg_stop != پوچ)
	اگر (dict_add_string(dict, "stop", sgp->sg_stop) == FAIL)
	    برو error;
    اگر (sgp->sg_cterm != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_cterm);
	اگر (attr_dict != پوچ)
	{
	    اگر (dict_add_dict(dict, "cterm", attr_dict) == FAIL)
		برو error;
	    attr_dict = پوچ;
	}
    }
    اگر (sgp->sg_cterm_fg != 0)
	اگر (dict_add_string(dict, "ctermfg",
		    highlight_color(hlgid, (char_u *)"fg", 'c')) == FAIL)
	    برو error;
    اگر (sgp->sg_cterm_bg != 0)
	اگر (dict_add_string(dict, "ctermbg",
			highlight_color(hlgid, (char_u *)"bg", 'c')) == FAIL)
	    برو error;
    اگر (sgp->sg_cterm_ul != 0)
	اگر (dict_add_string(dict, "ctermul",
			highlight_color(hlgid, (char_u *)"ul", 'c')) == FAIL)
	    برو error;
    اگر (sgp->sg_cterm_font != 0)
	اگر (dict_add_string(dict, "ctermfont",
			highlight_color(hlgid, (char_u *)"font", 'c')) == FAIL)
	    برو error;
    اگر (sgp->sg_gui != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_gui);
	اگر (attr_dict != پوچ)
	{
	    اگر (dict_add_dict(dict, "gui", attr_dict) == FAIL)
		برو error;
	    attr_dict = پوچ;
	}
    }
    اگر (sgp->sg_gui_fg_name != پوچ)
	اگر (dict_add_string(dict, "guifg",
			highlight_color(hlgid, (char_u *)"fg", 'g')) == FAIL)
	    برو error;
    اگر (sgp->sg_gui_bg_name != پوچ)
	اگر (dict_add_string(dict, "guibg",
			highlight_color(hlgid, (char_u *)"bg", 'g')) == FAIL)
	    برو error;
    اگر (sgp->sg_gui_sp_name != پوچ)
	اگر (dict_add_string(dict, "guisp",
			highlight_color(hlgid, (char_u *)"sp", 'g')) == FAIL)
	    برو error;
# ifdef FEAT_GUI
    اگر (sgp->sg_font_name != پوچ)
	اگر (dict_add_string(dict, "font", sgp->sg_font_name) == FAIL)
	    برو error;
# endif
    اگر (sgp->sg_link)
    {
	char_u	*link;

	link = HL_TABLE()[sgp->sg_link - 1].sg_name;
	اگر (link != پوچ && dict_add_string(dict, "linksto", link) == FAIL)
	    برو error;

	اگر (sgp->sg_deflink)
	    dict_add_bool(dict, "default", VVAL_TRUE);
    }
    اگر (dict_len(dict) == 2)
	// If only 'name' is present, then the highlight group is cleared.
	dict_add_bool(dict, "cleared", VVAL_TRUE);

    بازگشت dict;

error:
    dict_unref(attr_dict);
    dict_unref(dict);
    بازگشت پوچ;
}

/*
 * "hlget([name])" function
 * Return the attributes of a specific highlight group (if specified) or all
 * the highlight groups.
 */
    تهی
f_hlget(typval_T *argvars, typval_T *rettv)
{
    list_T	*list;
    dict_T	*dict;
    صحیح		i;
    char_u	*hlarg = پوچ;
    صحیح		resolve_link = FALSE;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (check_for_opt_string_arg(argvars, 0) == FAIL
	    || (argvars[0].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	// highlight group name supplied
	hlarg = tv_get_string_chk(&argvars[0]);
	اگر (hlarg == پوچ)
	    بازگشت;

	اگر (argvars[1].v_type != VAR_UNKNOWN)
	{
	    صحیح error = FALSE;

	    resolve_link = tv_get_bool_chk(&argvars[1], &error);
	    اگر (error)
		بازگشت;
	}
    }

    list = rettv->vval.v_list;
    برای (i = 0; i < highlight_ga.ga_len && !got_int; ++i)
    {
	اگر (hlarg == پوچ || STRICMP(hlarg, HL_TABLE()[i].sg_name) == 0)
	{
	    dict = highlight_get_info(i, resolve_link);
	    اگر (dict != پوچ && list_append_dict(list, dict) == FAIL)
		dict_unref(dict);
	}
    }
}

/*
 * Returns the string value at 'dict[key]'. Returns NULL, if 'key' is not in
 * 'dict' or the value is not a string type. If the value is not a string type
 * or is NULL, then 'error' is set to TRUE.
 */
    ایستا char_u *
hldict_get_string(dict_T *dict, char_u *کلید, صحیح *error)
{
    dictitem_T	*di;

    *error = FALSE;
    di = dict_find(dict, کلید, -1);
    اگر (di == پوچ)
	بازگشت پوچ;

    اگر (di->di_tv.v_type != VAR_STRING || di->di_tv.vval.v_string == پوچ)
    {
	emsg(_(e_string_required));
	*error = TRUE;
	بازگشت پوچ;
    }

    بازگشت di->di_tv.vval.v_string;
}

/*
 * Convert the highlight attribute Dictionary at 'dict[key]' into a string
 * value in 'attr_str' of length 'len'. Returns FALSE if 'dict[key]' is not a
 * Dictionary or is NULL.
 */
    ایستا صحیح
hldict_attr_to_str(
	dict_T	*dict,
	char_u	*کلید,
	char_u	*attr_str,
	size_t	len)
{
    dictitem_T	*di;
    dict_T	*attrdict;
    صحیح		i;
    char_u	*p;

    attr_str[0] = NUL;
    di = dict_find(dict, کلید, -1);
    اگر (di == پوچ)
	بازگشت TRUE;

    اگر (di->di_tv.v_type != VAR_DICT || di->di_tv.vval.v_dict == پوچ)
    {
	emsg(_(e_dictionary_required));
	بازگشت FALSE;
    }

    attrdict = di->di_tv.vval.v_dict;

    // If the attribute dict is empty, then return NONE to clear the attributes
    اگر (dict_len(attrdict) == 0)
    {
	vim_strcat(attr_str, (char_u *)"NONE", len);
	بازگشت TRUE;
    }

    p = attr_str;
    برای (i = 0; i < (صحیح)ARRAY_LENGTH(highlight_tab); ++i)
    {
	اگر (dict_get_bool(attrdict, (نویسه *)highlight_tab[i].مقدار.string,
		    VVAL_FALSE) == VVAL_TRUE)
	{
	    اگر (p != attr_str && (size_t)(p - attr_str + 2) < len)
		STRCPY(p, (char_u *)",");
	    اگر (p - attr_str + highlight_tab[i].مقدار.length + 1 < len)
	    {
		STRCPY(p, highlight_tab[i].مقدار.string);
		p += highlight_tab[i].مقدار.length;
	    }
	}
    }

    بازگشت TRUE;
}

// Temporary buffer used to store the command string produced by hlset().
// IObuff cannot be used for this as the error messages produced by hlset()
// internally use IObuff.
# define	HLSETBUFSZ  512
ایستا char_u hlsetBuf[HLSETBUFSZ + 1];

/*
 * Add the highlight attribute "attr" of length "attrlen" and "value" at
 * "dptr", which points into "hlsetBuf".
 * Returns the updated pointer.
 */
    ایستا char_u *
add_attr_and_مقدار(char_u *dptr, char_u *attr, صحیح attrlen, char_u *مقدار)
{
    size_t	vallen;

    // Do nothing if the value is not specified or is empty
    اگر (مقدار == پوچ || *مقدار == NUL)
	بازگشت dptr;

    vallen = STRLEN(مقدار);
    // When the value contains a space and the attribute has an "=" (i.e. it
    // is a key=value pair), surround the value with single quotes so that
    // do_highlight() can parse it correctly.
    اگر (vim_strchr(مقدار, ' ') != پوچ && vim_strchr(attr, '=') != پوچ)
    {
	اگر (dptr + attrlen + vallen + 3 < hlsetBuf + HLSETBUFSZ)
	{
	    STRCPY(dptr, attr);
	    dptr += attrlen;
	    *dptr++ = '\'';
	    STRCPY(dptr, مقدار);
	    dptr += vallen;
	    *dptr++ = '\'';
	    *dptr = NUL;
	}
    }
    وگرنه اگر (dptr + attrlen + vallen + 1 < hlsetBuf + HLSETBUFSZ)
    {
	STRCPY(dptr, attr);
	dptr += attrlen;
	STRCPY(dptr, مقدار);
	dptr += vallen;
    }

    بازگشت dptr;
}

/*
 * Add or update a highlight group using 'dict' items. Returns TRUE if
 * successfully updated the highlight group.
 */
    ایستا صحیح
hlg_add_or_update(dict_T *dict)
{
    char_u	*name;
    صحیح		error;
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
    صحیح		forceit = FALSE;
    صحیح		dodefault = FALSE;
    صحیح		done = FALSE;
    char_u	*p;

    name = hldict_get_string(dict, (char_u *)"name", &error);
    اگر (name == پوچ || *name == NUL || error)
	بازگشت FALSE;

    اگر (dict_get_bool(dict, "force", VVAL_FALSE) == VVAL_TRUE)
	forceit = TRUE;

    اگر (dict_get_bool(dict, "default", VVAL_FALSE) == VVAL_TRUE)
	dodefault = TRUE;

    اگر (dict_has_کلید(dict, "cleared"))
    {
	varnumber_T	cleared;

	// clear a highlight group
	cleared = dict_get_bool(dict, "cleared", FALSE);
	اگر (cleared == TRUE)
	{
	    vim_snprintf((نویسه *)hlsetBuf, HLSETBUFSZ, "clear %s", name);
	    do_highlight(hlsetBuf, forceit, FALSE);
	    done = TRUE;
	}
    }

    اگر (dict_has_کلید(dict, "linksto"))
    {
	char_u	*linksto;

	// link highlight groups
	linksto = hldict_get_string(dict, (char_u *)"linksto", &error);
	اگر (linksto == پوچ || *linksto == NUL || error)
	    بازگشت FALSE;

	vim_snprintf((نویسه *)hlsetBuf, HLSETBUFSZ, "%slink %s %s",
				dodefault ? "default " : "", name, linksto);
	do_highlight(hlsetBuf, forceit, FALSE);

	done = TRUE;
    }

    // If 'cleared' or 'linksto' are specified, then don't process the other
    // attributes.
    اگر (done)
	بازگشت TRUE;

    start = hldict_get_string(dict, (char_u *)"start", &error);
    اگر (error)
	بازگشت FALSE;

    stop = hldict_get_string(dict, (char_u *)"stop", &error);
    اگر (error)
	بازگشت FALSE;

    اگر (!hldict_attr_to_str(dict, (char_u *)"term", term_attr,
							    اندازه(term_attr)))
	بازگشت FALSE;

    اگر (!hldict_attr_to_str(dict, (char_u *)"cterm", cterm_attr,
							   اندازه(cterm_attr)))
	بازگشت FALSE;

    ctermfg = hldict_get_string(dict, (char_u *)"ctermfg", &error);
    اگر (error)
	بازگشت FALSE;

    ctermbg = hldict_get_string(dict, (char_u *)"ctermbg", &error);
    اگر (error)
	بازگشت FALSE;

    ctermul = hldict_get_string(dict, (char_u *)"ctermul", &error);
    اگر (error)
	بازگشت FALSE;

    ctermfont = hldict_get_string(dict, (char_u *)"ctermfont", &error);
    اگر (error)
	بازگشت FALSE;

    اگر (!hldict_attr_to_str(dict, (char_u *)"gui", gui_attr, اندازه(gui_attr)))
	بازگشت FALSE;

    guifg = hldict_get_string(dict, (char_u *)"guifg", &error);
    اگر (error)
	بازگشت FALSE;

    guibg = hldict_get_string(dict, (char_u *)"guibg", &error);
    اگر (error)
	بازگشت FALSE;

    guisp = hldict_get_string(dict, (char_u *)"guisp", &error);
    اگر (error)
	بازگشت FALSE;

# ifdef FEAT_GUI
    font = hldict_get_string(dict, (char_u *)"font", &error);
    اگر (error)
	بازگشت FALSE;
# endif

    // If none of the attributes are specified, then do nothing.
    اگر (term_attr[0] == NUL && start == پوچ && stop == پوچ
	    && cterm_attr[0] == NUL && ctermfg == پوچ && ctermbg == پوچ
	    && ctermul == پوچ && ctermfont == پوچ && gui_attr[0] == NUL
# ifdef FEAT_GUI
	    && font == پوچ
# endif
	    && guifg == پوچ && guibg == پوچ && guisp == پوچ
	    )
	بازگشت TRUE;

    hlsetBuf[0] = NUL;
    p = hlsetBuf;
    اگر (dodefault)
	p = add_attr_and_مقدار(p, (char_u *)"default", 7, (char_u *)" ");
    p = add_attr_and_مقدار(p, (char_u *)"", 0, name);
    p = add_attr_and_مقدار(p, (char_u *)" term=", 6, term_attr);
    p = add_attr_and_مقدار(p, (char_u *)" start=", 7, start);
    p = add_attr_and_مقدار(p, (char_u *)" stop=", 6, stop);
    p = add_attr_and_مقدار(p, (char_u *)" cterm=", 7, cterm_attr);
    p = add_attr_and_مقدار(p, (char_u *)" ctermfg=", 9, ctermfg);
    p = add_attr_and_مقدار(p, (char_u *)" ctermbg=", 9, ctermbg);
    p = add_attr_and_مقدار(p, (char_u *)" ctermul=", 9, ctermul);
    p = add_attr_and_مقدار(p, (char_u *)" ctermfont=", 11, ctermfont);
    p = add_attr_and_مقدار(p, (char_u *)" gui=", 5, gui_attr);
# ifdef FEAT_GUI
    p = add_attr_and_مقدار(p, (char_u *)" font=", 6, font);
# endif
    p = add_attr_and_مقدار(p, (char_u *)" guifg=", 7, guifg);
    p = add_attr_and_مقدار(p, (char_u *)" guibg=", 7, guibg);
    (تهی)add_attr_and_مقدار(p, (char_u *)" guisp=", 7, guisp);

    do_highlight(hlsetBuf, forceit, FALSE);

    بازگشت TRUE;
}

/*
 * "hlset([{highlight_attr}])" function
 * Add or modify highlight groups
 */
    تهی
f_hlset(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    dict_T	*dict;

    rettv->vval.v_عدد = -1;

    اگر (check_for_list_arg(argvars, 0) == FAIL)
	بازگشت;

    FOR_ALL_LIST_اقلام(argvars->vval.v_list, li)
    {
	اگر (li->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_dictionary_required));
	    بازگشت;
	}

	dict = li->li_tv.vval.v_dict;
	اگر (!hlg_add_or_update(dict))
	    بازگشت;
    }

    rettv->vval.v_عدد = 0;
}
#endif

/*
 * If "old" is in the override stack, then update it to "hl_new". Does not free
 * "old".
 */
    تهی
update_highlight_overrides(hl_override_T *old, hl_override_T *hl_new, صحیح newlen)
{
    اگر (old == پوچ)
	بازگشت;

    برای (hl_overrides_T *set = overrides; set != پوچ; set = set->next)
    {
	اگر (set->arr == old)
	{
	    set->arr = hl_new;
	    set->len = newlen;
	}
    }
}

/*
 * If "arr" is in the highlight overrides list, then mark it as invalid.
 */
    تهی
remove_highlight_overrides(hl_override_T *arr)
{
    اگر (arr == پوچ || overrides == پوچ)
	بازگشت;

    برای (hl_overrides_T *set = overrides; set != پوچ; set = set->next)
    {
	اگر (set->arr == arr)
	{
	    set->arr = پوچ;
	    بشکن;
	}
    }
}

/*
 * Update highlight_attr[] array. If "update_ids" is true, then update
 * highlight_ids[] array instead.
 */
    ایستا تهی
set_highlight_attr(hl_override_T *arr, صحیح len, bool update_ids)
{
    برای (صحیح i = 0; i < len; i++)
    {
	hl_override_T	*override = arr + i;
	صحیح		hlf = -1, attr;

	اگر (override->from <= 0)
	    // Directly map to highlight_attr
	    hlf = -override->from;
	وگرنه
	{
	    برای (صحیح k = 0; k < HLF_شمار; k++)
		اگر (override->from == highlight_ids[k])
		{
		    hlf = k;
		    بشکن;
		}

	    اگر (hlf == -1)
		ادامه;
	}

	اگر (update_ids)
	{
	    اگر (override->to <= 0)
	    {
		اگر (hlf == -override->to)
		    highlight_ids[hlf] = -1;
		وگرنه
		    highlight_ids[hlf] = highlight_ids[-override->to];
	    }
	    وگرنه
		highlight_ids[hlf] = override->to;
	}
	وگرنه
	{
	    اگر (override->to <= 0)
		// Directly use highlight_attr[]
		attr = highlight_attr[-override->to];
	    وگرنه
		attr = syn_id2attr(override->to);

	    highlight_attr[hlf] = attr;
	}
    }
}

/*
 * Set the current highlight override to "arr". Returns true if successful
 */
    bool
افزودن_highlight_overrides(hl_override_T *arr, صحیح len)
{
    // Don't want to do anything if "arr" is already the current override. If
    // "arr" is NULL (but overrides->arr is not), then still push an override,
    // but "->arr" will just be NULL so any previous overrides are cleared.
    اگر (overrides != پوچ && overrides->arr == arr)
	بازگشت false;

    hl_overrides_T *set;

    set = ALLOC_ONE(hl_overrides_T);
    اگر (set == پوچ)
	بازگشت false;

    set->arr = arr;
    set->len = len;

    // Push to front of list
    set->next = overrides;
    overrides = set;

    // Save current highlight_attr[], so it can be restored later after this
    // override is popped.
    memcpy(set->attr, highlight_attr, اندازه(highlight_attr));

    // Reset highlight_attr[] to default values, as we don't want changes from
    // other overrides.
    memcpy(highlight_attr, highlight_attr_raw, اندازه(highlight_attr));

    // Update highlight_attr[] array
    اگر (arr != پوچ)
	set_highlight_attr(arr, len, false);

    بازگشت true;
}

/*
 * Pop the current highlight override (if any) from the stack. This should
 * always be paired with a *successful* push.
 */
    تهی
برداشت_highlight_overrides(تهی)
{
    hl_overrides_T *set = overrides;

    اگر (overrides == پوچ)
	بازگشت;

    overrides = set->next;

    // Set highlight_attr[] to state before override was pushed.
    memcpy(highlight_attr, set->attr, اندازه(highlight_attr));
    vim_free(set);
}

/*
 * Parse the 'winhighlight' option and return array. Returns NULL on failure or
 * if empty option. If failure, then errmsg is set.
 */
    ایستا hl_override_T *
parse_winhighlight(char_u *opt, صحیح *len, نویسه **errmsg)
{
    char_u	    *p = opt;
    hl_override_T   *arr;
    صحیح		    i = 0;
    صحیح		    رقم = 1;
    صحیح		    n_colons = 0;

    اگر (*p == NUL)
	بازگشت پوچ;

    // Get number of overrides first so we can allocate array
    وقتی ((p = vim_strchr(p, ',')) != پوچ)
    {
	p++;
	رقم++;
    }
    p = opt;
    // Check if number of ':' matches number of ','
    وقتی ((p = vim_strchr(p, ':')) != پوچ)
    {
	p++;
	n_colons++;
    }

    اگر (رقم != n_colons)
    {
	*errmsg = e_invalid_argument;
	بازگشت پوچ;
    }

    arr = ALLOC_MULT(hl_override_T, رقم);
    اگر (arr == پوچ)
    {
	*errmsg = e_out_of_memory;
	بازگشت پوچ;
    }

    p = opt;

    وقتی (true)
    {
	hl_override_T *override = arr + i++;
	char_u	*fromname = p, *toname;
	char_u	*tmp;
	char_u	**names[2] = {&fromname, &toname};
	صحیح	fromlen, tolen;
	صحیح	*lens[2] = {&fromlen, &tolen};
	صحیح	fromid, toid;
	صحیح	*ids[2] = {&fromid, &toid};

	p = vim_strchr(p, ':');

	اگر (p == پوچ)
	    برو fail;

	fromlen = p - fromname; // Get hl for "from"
	اگر (fromlen == 0)
	    برو fail;
	p++; // Skip colon ':'
	اگر (*p == NUL)
	    برو fail;

	toname = p;
	tmp = vim_strchr(p, ',');

	// Get hl for "to", must check for no trailing comma in case last
	// element.
	اگر (tmp == پوچ)
	    tolen = (صحیح)STRLEN(p);
	وگرنه
	{
	    tolen = tmp - toname;
	    p = ++tmp;
	}
	اگر (tolen == 0)
	    برو fail;

	برای (صحیح k = 0; k < 2; k++)
	{
	    char_u  *name = *names[k];
	    صحیح	    nlen = *lens[k];

	    اگر (*name == '!')
	    {
		// If starts with "!", then it maps directly to a 'highlight'
		// occasion.
		صحیح hlf;

		اگر (nlen != 2)
		    برو fail;

		برای (hlf = 0; hlf < (صحیح)HLF_شمار; ++hlf)
		    اگر (hl_flags[hlf] == name[1])
			بشکن;

		اگر (hlf >= HLF_شمار)
		    برو fail;

		*ids[k] = -hlf;
	    }
	    وگرنه
	    {
		// Otherwise get the highlight group id.
		اگر (syn_check_group(name, nlen) == 0)
		    برو fail;

		*ids[k] = syn_namen2id(name, nlen);
		اگر (*ids[k] == 0)
		    برو fail;

		// Always map "Normal" group to HLF_WIN
		اگر (ids[k] == &fromid &&
			STRCMP(HL_TABLE()[*ids[k] - 1].sg_name_u, "NORMAL") == 0)
		    *ids[k] = -HLF_WIN;
	    }
	}

	// If fromid == toid, leave it be, this is so that we know HLF_WIN has
	// been set (even to itself e.g. "Normal"), and therefore know to use
	// its attr instead.
	override->from = fromid;
	override->to = toid;

	اگر (tmp == پوچ)
	    بشکن;
    }

    *len = رقم;
    بازگشت arr;
fail:
    vim_free(arr);
    *errmsg = e_invalid_argument;
    بازگشت پوچ;
}

/*
 * Update w_hl for "wp" using given option value. Returns error
 * message on failure, otherwise NULL.
 */
    نویسه *
update_winhighlight(win_T *wp, char_u *opt)
{
    نویسه	    *err = پوچ;
    hl_override_T   *arr;
    صحیح		    رقم = 1;

    arr = parse_winhighlight(opt, &رقم, &err);

    اگر (arr == پوچ && err != پوچ)
	بازگشت err;

    اگر (arr == پوچ && wp->w_hl == پوچ)
	بازگشت پوچ;

    update_highlight_overrides(wp->w_hl, arr, رقم);

    vim_free(wp->w_hl);
    wp->w_hl = arr;
    wp->w_hl_len = رقم;

    wp->w_hlfwin_id = hlf_get_id(wp, HLF_WIN);

#ifdef FEAT_TERMINAL
    term_update_hlfwin(wp);
#endif

#ifdef FEAT_TERMINAL
    // May be NULL (such as in after_copy_winopt())
    اگر (wp->w_buffer != پوچ)
    {
	// Make sure terminal highlighting is updated
	bool ret = افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);

	اگر (wp->w_buffer->b_term != پوچ)
	    term_init_default_colors(wp->w_buffer->b_term);
	اگر (ret)
	    برداشت_highlight_overrides();
    }
#endif

    بازگشت پوچ;
}

/*
 * Get id that the hlf attr is using in window "wp", otherwise zero. Note that
 * -1 may be returned if the hlf was set to itself.
 */
    صحیح
hlf_get_id(win_T *wp, صحیح hlf)
{
    صحیح id;
    ایستا صحیح prev[HLF_شمار];

    اگر (wp->w_hl == پوچ)
	بازگشت highlight_ids[hlf];

    memcpy(prev, highlight_ids, اندازه(highlight_ids));
    set_highlight_attr(wp->w_hl, wp->w_hl_len, true);
    id = highlight_ids[hlf];
    memcpy(highlight_ids, prev, اندازه(highlight_ids));

    بازگشت id;
}

/*
 * Update 'winhighlight' using a 'wincolor' style option value. Returns error
 * message on failure otherwise NULL. Does not update the actual 'wincolor'
 * value, but does update the 'winhighlight' value.
 */
    نویسه *
update_wincolor(win_T *wp, char_u *opt)
{
    char_u  *str = *opt == NUL ? "" : alloc(اندازه("!(:") + STRLEN(opt));
    نویسه    *errmsg;

    اگر (str == پوچ)
	بازگشت e_out_of_memory;

    اگر (*opt != NUL)
	sprintf((نویسه *)str, "!(:%s", opt);

    errmsg = update_winhighlight(wp, str);

    اگر (errmsg == پوچ)
	set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		str, OPT_FREE|OPT_LOCAL, 0);

    اگر (*opt != NUL)
	آزاد(str);
    بازگشت errmsg;
}
