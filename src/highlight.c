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

#அந்தர்பாவ "vim.h"

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
நிலையான keyvalue_T highlight_tab[] = {
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
நிலையான keyvalue_T *highlight_index_tab[] = {
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

எண்ணிக்கை {
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
நிலையான keyvalue_T color_name_tab[] = {
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
வகைவரை கட்டமைப்பு
{
    char_u	*sg_name;	// highlight group name
    char_u	*sg_name_u;	// uppercase of sg_name
    முழு		sg_cleared;	// "hi clear" was used
// for normal terminals
    முழு		sg_term;	// "term=" highlighting attributes
    char_u	*sg_start;	// terminal string for start highl
    char_u	*sg_stop;	// terminal string for stop highl
    முழு		sg_term_attr;	// Screen attr for term mode
// for color terminals
    முழு		sg_cterm;	// "cterm=" highlighting attr
    முழு		sg_cterm_bold;	// bold attr was set for light color
    முழு		sg_cterm_fg;	// terminal fg color number + 1
    முழு		sg_cterm_bg;	// terminal bg color number + 1
    முழு		sg_cterm_ul;	// terminal ul color number + 1
    முழு		sg_cterm_attr;	// Screen attr for color term mode
    முழு		sg_cterm_font;	// terminal alternative font (0 for normal)
// for when using the GUI
#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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
    முழு		sg_gui_attr;    // Screen attr for GUI mode
#endif
#எனில் defined(FEAT_GUI) || defined(FEAT_EVAL)
// Store the sp color name for the GUI or synIDattr()
    முழு		sg_gui;		// "gui=" highlighting attributes
    char_u	*sg_gui_fg_name;// GUI foreground color name
    char_u	*sg_gui_bg_name;// GUI background color name
    char_u	*sg_gui_sp_name;// GUI special color name
#endif
    முழு		sg_link;	// link to this highlight group ID
    முழு		sg_deflink;	// default link; restored in highlight_clear()
    முழு		sg_set;		// combination of SG_* flags
#ifdef FEAT_EVAL
    sctx_T	sg_deflink_sctx;  // script where the default link was set
    sctx_T	sg_script_ctx;	// script in which the group was last set
#endif
} hl_group_T;

வகைவரை கட்டமைப்பு hl_overrides_S hl_overrides_T;
கட்டமைப்பு hl_overrides_S
{
    hl_override_T   *arr;   // May be NULL if "arr" was freed or no highlight
			    // overrides (all values set to default)
    முழு		    len;
    hl_overrides_T  *next;  // Used to handle recursive calls

    முழு attr[HLF_மொத்தம்];    // highlight_attr[] before being updated.
};

நிலையான hl_overrides_T *overrides = வெற்று;

// Synced with highlight_attr, each is the highlight group id for each attr. If
// an element is 0, then there is no attr for it. If an element is -1, then it
// means it was set to itself.
நிலையான முழு highlight_ids[HLF_மொத்தம்];

// Same as highlight_attr[] but is not modified by highlight group overrides
நிலையான முழு highlight_attr_raw[HLF_மொத்தம்];

நிலையான முழு hl_flags[HLF_மொத்தம்] = HL_FLAGS;

// highlight groups for 'highlight' option
நிலையான garray_T highlight_ga;
#define HL_TABLE()	((hl_group_T *)((highlight_ga.ga_data)))

// Wrapper struct for hashtable entries: stores the highlight group ID alongside
// the uppercase name used as a hash key.  Uses the same offsetof pattern as
// signgroup_T / HI2SG.
வகைவரை கட்டமைப்பு {
    முழு		hn_id;		// highlight group ID (1-based)
    char_u	hn_சாவி[1];	// uppercase name (stretchable array)
} hlname_T;

#define HLNAME_சாவி_OFF	offsetof(hlname_T, hn_சாவி)
#define HI2HLNAME(hi)	((hlname_T *)((hi)->hi_சாவி - HLNAME_சாவி_OFF))

// hashtable for quick highlight group name lookup
நிலையான hashtab_T highlight_ht;
நிலையான bool	 highlight_ht_inited = false;

/*
 * An attribute number is the index in attr_table plus ATTR_OFF.
 */
#define ATTR_OFF (HL_ALL + 1)

நிலையான வெறுமை syn_unadd_group(வெறுமை);
நிலையான வெறுமை set_hl_attr(முழு idx);
நிலையான வெறுமை highlight_list_one(முழு id);
நிலையான முழு highlight_list_arg(முழு id, முழு didh, முழு type, முழு iarg, char_u *sarg, எழுத்து *name);
நிலையான முழு syn_add_group(char_u *name);
நிலையான முழு syn_name2id_len(char_u *name, முழு len);
நிலையான முழு hl_has_settings(முழு idx, முழு check_link);
நிலையான வெறுமை highlight_clear(முழு idx);

#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
நிலையான வெறுமை gui_do_one_color(முழு idx, முழு do_menu, முழு do_tooltip);
#endif
#ifdef FEAT_GUI
நிலையான முழு  set_group_colors(char_u *name, guicolor_T *fgp, guicolor_T *bgp, முழு do_menu, முழு use_norm, முழு do_tooltip);
நிலையான வெறுமை hl_do_font(முழு idx, char_u *arg, முழு do_normal, முழு do_menu, முழு do_tooltip, முழு free_font);
#endif

/*
 * The default highlight groups.  These are compiled-in for fast startup and
 * they still work when the runtime files can't be found.
 * When making changes here, also change runtime/colors/default.vim!
 * The #ifdefs are needed to reduce the amount of static data.  Helps to make
 * the 16 bit DOS (museum) version compile.
 */
#எனில் defined(FEAT_GUI) || defined(FEAT_EVAL)
# define CENT(a, b) b
#இல்லையெனில்
# define CENT(a, b) a
#endif
நிலையான எழுத்து *(highlight_init_both[]) = {
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
    வெற்று
};

// Default colors only used with a light background.
நிலையான எழுத்து *(highlight_init_light[]) = {
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
    வெற்று
};

// Default colors only used with a dark background.
நிலையான எழுத்து *(highlight_init_dark[]) = {
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
    வெற்று
};

#எனில் defined(FEAT_SYN_HL)
/*
 * Returns the number of highlight groups.
 */
    முழு
highlight_இலக்கம்_groups(வெறுமை)
{
    திரும்பு highlight_ga.ga_len;
}

/*
 * Returns the name of a highlight group.
 */
    char_u *
highlight_group_name(முழு id)
{
    திரும்பு HL_TABLE()[id].sg_name;
}

/*
 * Returns the ID of the link to a highlight group.
 */
    முழு
highlight_link_id(முழு id)
{
    திரும்பு HL_TABLE()[id].sg_link;
}
#endif

நிலையான வெறுமை resolve_fallback_fg_to_rgb(வெறுமை);
நிலையான வெறுமை resolve_fallback_bg_to_rgb(வெறுமை);

    வெறுமை
init_highlight(
    முழு		both,	    // include groups where 'bg' doesn't matter
    முழு		reset)	    // clear group first
{
    முழு		i;
    எழுத்து	**pp;
    நிலையான முழு	had_both = FALSE;
#ifdef FEAT_EVAL
    char_u	*p;

    // Try finding the color scheme file.  Used when a color file was loaded
    // and 'background' or 't_Co' is changed.
    p = get_var_மதிப்பு((char_u *)"g:colors_name");
    எனில் (p != வெற்று)
    {
	// The value of g:colors_name could be freed when sourcing the script,
	// making "p" invalid, so copy it.
	char_u *copy_p = vim_strsave(p);
	முழு    r;

	எனில் (copy_p != வெற்று)
	{
	    r = load_colors(copy_p);
	    vim_free(copy_p);
	    எனில் (r == OK)
		திரும்பு;
	}
    }

#endif

    // Didn't use a color file, use the compiled-in colors.
    எனில் (both)
    {
	had_both = TRUE;
	pp = highlight_init_both;
	க்கு (i = 0; pp[i] != வெற்று; ++i)
	    do_highlight((char_u *)pp[i], reset, TRUE);
    }
    இல்லையெனில் எனில் (!had_both)
	// Don't do anything before the call with both == TRUE from main().
	// Not everything has been setup then, and that call will overrule
	// everything anyway.
	திரும்பு;

    எனில் (*p_bg == 'l')
	pp = highlight_init_light;
    இல்லையெனில்
	pp = highlight_init_dark;
    க்கு (i = 0; pp[i] != வெற்று; ++i)
	do_highlight((char_u *)pp[i], reset, TRUE);

    // Reverse looks ugly, but grey may not work for less than 8 colors.  Thus
    // let it depend on the number of colors available.
    எனில் (t_colors < 8)
	do_highlight((char_u *)"Visual term=reverse cterm=reverse ctermbg=NONE ctermfg=NONE",
		FALSE, TRUE);
    // With 8 colors brown is equal to yellow, need to use black for Search fg
    // to avoid Statement highlighted text disappears.
    // Clear the attributes, needed when changing the t_Co value.
    எனில் (t_colors <= 8)
    {
	எனில் (*p_bg == 'l')
	    do_highlight((char_u *)"Search ctermfg=black", FALSE, TRUE);
    }

#ifdef FEAT_SYN_HL
    // If syntax highlighting is enabled load the highlighting for it.
    எனில் (get_var_மதிப்பு((char_u *)"g:syntax_on") != வெற்று)
    {
	நிலையான முழு	recursive = 0;

	எனில் (recursive >= 5)
	    emsg(_(e_recursive_loop_loading_syncolor_vim));
	இல்லையெனில்
	{
	    ++recursive;
	    (வெறுமை)source_runtime((char_u *)"syntax/syncolor.vim", DIP_ALL);
	    --recursive;
	}
    }
#endif
    resolve_fallback_fg_to_rgb();
    resolve_fallback_bg_to_rgb();
}

#எனில் defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
/*
 * Load a default color list. Intended to support legacy color names but allows
 * the user to override the color values. Only loaded once.
 */
    நிலையான வெறுமை
load_default_colors_lists(வெறுமை)
{
    // Lacking a default color list isn't the end of the world but it is likely
    // an inconvenience so users should know when it is missing.
    எனில் (source_runtime((char_u *)"colors/lists/default.vim", DIP_ALL) != OK)
	செய்தி("failed to load colors/lists/default.vim");
}
#endif

/*
 * Load color file "name".
 * Return OK for success, FAIL for failure.
 */
    முழு
load_colors(char_u *name)
{
    char_u	*buf;
    முழு		retval = FAIL;
    நிலையான முழு	recursive = FALSE;

    // When being called recursively, this is probably because setting
    // 'background' caused the highlighting to be reloaded.  This means it is
    // working, thus we should return OK.
    எனில் (recursive)
	திரும்பு OK;

    recursive = TRUE;
    buf = alloc(STRLEN(name) + 12);
    எனில் (buf != வெற்று)
    {
#எனில் defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	load_default_colors_lists();
#endif
	apply_autocmds(EVENT_COLORSCHEMEPRE, name,
					       curbuf->b_fname, FALSE, curbuf);
	sprintf((எழுத்து *)buf, "colors/%s.vim", name);
	retval = source_runtime(buf, DIP_START + DIP_OPT);
	vim_free(buf);
	எனில் (retval == OK)
	    apply_autocmds(EVENT_COLORSCHEME, name, curbuf->b_fname,
								FALSE, curbuf);
    }
    recursive = FALSE;

    திரும்பு retval;
}

நிலையான முழு color_numbers_16[28] = {0, 1, 2, 3,
				 4, 5, 6, 6,
				 7, 7, 7, 7,
				 8, 8,
				 9, 9, 10, 10,
				 11, 11, 12, 12, 13,
				 13, 14, 14, 15, -1};
// for xterm with 88 colors...
நிலையான முழு color_numbers_88[28] = {0, 4, 2, 6,
				 1, 5, 32, 72,
				 84, 84, 7, 7,
				 82, 82,
				 12, 43, 10, 61,
				 14, 63, 9, 74, 13,
				 75, 11, 78, 15, -1};
// for xterm with 256 colors...
நிலையான முழு color_numbers_256[28] = {0, 4, 2, 6,
				 1, 5, 130, 3,
				 248, 248, 7, 7,
				 242, 242,
				 12, 81, 10, 121,
				 14, 159, 9, 224, 13,
				 225, 11, 229, 15, -1};
// for terminals with less than 16 colors...
நிலையான முழு color_numbers_8[28] = {0, 4, 2, 6,
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
    நிலையான முழு
lookup_color(முழு idx, முழு foreground, முழு *boldp)
{
    முழு		color = color_numbers_16[idx];
    char_u	*p;

    // Use the _16 table to check if it's a valid color name.
    எனில் (color < 0)
	திரும்பு -1;

    எனில் (t_colors == 8)
    {
	// t_Co is 8: use the 8 colors table
#எனில் defined(__QNXNTO__)
	// On qnx, the 8 & 16 color arrays are the same
	எனில் (STRNCMP(T_NAME, "qansi", 5) == 0)
	    color = color_numbers_16[idx];
	இல்லையெனில்
#endif
	    color = color_numbers_8[idx];
	எனில் (foreground)
	{
	    // set/reset bold attribute to get light foreground
	    // colors (on some terminals, e.g. "linux")
	    எனில் (color & 8)
		*boldp = TRUE;
	    இல்லையெனில்
		*boldp = FALSE;
	}
	color &= 7;	// truncate to 8 colors
    }
    இல்லையெனில் எனில் (t_colors == 16 || t_colors == 88
					   || t_colors >= 256)
    {
	// Guess: if the termcap entry ends in 'm', it is
	// probably an xterm-like terminal.  Use the changed
	// order for colors.
	எனில் (*T_CAF != NUL)
	    p = T_CAF;
	இல்லையெனில்
	    p = T_CSF;
	எனில் (*p != NUL && (t_colors > 256
			      || *(p + STRLEN(p) - 1) == 'm'))
	{
	    எனில் (t_colors == 88)
		color = color_numbers_88[idx];
	    இல்லையெனில் எனில் (t_colors >= 256)
		color = color_numbers_256[idx];
	    இல்லையெனில்
		color = color_numbers_8[idx];
	}
#ifdef FEAT_TERMRESPONSE
	எனில் (t_colors >= 256 && color == 15 && is_mac_terminal)
	    // Terminal.app has a bug: 15 is light grey. Use white
	    // from the color cube instead.
	    color = 231;
#endif
    }
    திரும்பு color;
}

/*
 * Link highlight group 'from_hg' to 'to_hg'.
 * 'dodefault' is set to TRUE for ":highlight default link".
 * 'forceit' is set to TRUE for ":highlight! link"
 * 'init' is set to TRUE when initializing all the highlight groups.
 */
    நிலையான வெறுமை
highlight_group_link(
	char_u	*from_hg,
	முழு	from_len,
	char_u	*to_hg,
	முழு	to_len,
	முழு	dodefault,
	முழு	forceit,
	முழு	init)
{
    முழு		from_id;
    முழு		to_id;
    hl_group_T	*hlgroup = வெற்று;

    from_id = syn_check_group(from_hg, from_len);
    எனில் (STRNCMP(to_hg, "NONE", 4) == 0)
	to_id = 0;
    இல்லையெனில்
	to_id = syn_check_group(to_hg, to_len);

    எனில் (from_id > 0)
    {
	hlgroup = &HL_TABLE()[from_id - 1];
	எனில் (dodefault && (forceit || hlgroup->sg_deflink == 0))
	{
	    hlgroup->sg_deflink = to_id;
#ifdef FEAT_EVAL
	    hlgroup->sg_deflink_sctx = current_sctx;
	    hlgroup->sg_deflink_sctx.sc_lnum += SOURCING_LNUM;
#endif
	}
    }

    எனில் (from_id > 0 && (!init || hlgroup->sg_set == 0))
    {
	// Don't allow a link when there already is some highlighting
	// for the group, unless '!' is used
	எனில் (to_id > 0 && !forceit && !init
		&& hl_has_settings(from_id - 1, dodefault))
	{
	    எனில் (SOURCING_NAME == வெற்று && !dodefault)
		emsg(_(e_group_has_settings_highlight_link_ignored));
	}
	இல்லையெனில் எனில் (hlgroup->sg_link != to_id
#ifdef FEAT_EVAL
		|| hlgroup->sg_script_ctx.sc_sid != current_sctx.sc_sid
#endif
		|| hlgroup->sg_cleared)
	{
	    எனில் (!init)
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
    நிலையான வெறுமை
highlight_reset_all(வெறுமை)
{
    முழு		idx;

#ifdef FEAT_GUI
    // First, we do not destroy the old values, but allocate the new
    // ones and update the display. THEN we destroy the old values.
    // If we destroy the old values first, then the old values
    // (such as GuiFont's or GuiFontset's) will still be displayed but
    // invalid because they were free'd.
    எனில் (gui.in_use)
    {
# ifdef FEAT_BEVAL_TIP
	gui_init_tooltip_font();
# endif
# எனில் defined(FEAT_MENU) && defined(FEAT_GUI_MOTIF)
	gui_init_menu_font();
# endif
    }
# எனில் defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_X11)
    gui_mch_def_colors();
# endif
# ifdef FEAT_GUI_MSWIN
    எனில் (gui.in_use)
	gui_mch_set_titlebar_colors();
# endif
# ifdef FEAT_GUI_X11
#  ifdef FEAT_MENU

    // This only needs to be done when there is no Menu highlight
    // group defined by default, which IS currently the case.
    gui_mch_new_menu_colors();
#  endif
    எனில் (gui.in_use)
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
    க்கு (idx = 0; idx < highlight_ga.ga_len; ++idx)
	highlight_clear(idx);
    init_highlight(TRUE, TRUE);
#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    எனில் (USE_24BIT)
	highlight_gui_started();
    இல்லையெனில்
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
    நிலையான முழு
highlight_set_termgui_attr(முழு idx, char_u *சாவி, char_u *arg, முழு init)
{
    முழு		attr;
    size_t	off;
    keyvalue_T	target;
    keyvalue_T	*entry;

    attr = 0;
    off = 0;
    target.சாவி = 0;
    target.மதிப்பு.length = 0;	// not used, see cmp_keyvalue_value_ni()
    போது (arg[off] != NUL)
    {
	target.மதிப்பு.string = arg + off;
	entry = (keyvalue_T *)bsearch(&target, &highlight_tab,
		ARRAY_LENGTH(highlight_tab), அளவு(highlight_tab[0]),
		cmp_keyvalue_மதிப்பு_ni);
	எனில் (entry == வெற்று)
	{
	    semsg(_(e_illegal_மதிப்பு_str), arg);
	    திரும்பு FALSE;
	}

	attr |= entry->சாவி;
	off += entry->மதிப்பு.length;
	எனில் (arg[off] == ',')		// another one follows
	    ++off;
    }
    எனில் (*சாவி == 'T')
    {
	எனில் (!init || !(HL_TABLE()[idx].sg_set & SG_TERM))
	{
	    எனில் (!init)
		HL_TABLE()[idx].sg_set |= SG_TERM;
	    HL_TABLE()[idx].sg_term = attr;
	}
    }
    இல்லையெனில் எனில் (*சாவி == 'C')
    {
	எனில் (!init || !(HL_TABLE()[idx].sg_set & SG_CTERM))
	{
	    எனில் (!init)
		HL_TABLE()[idx].sg_set |= SG_CTERM;
	    HL_TABLE()[idx].sg_cterm = attr;
	    HL_TABLE()[idx].sg_cterm_bold = FALSE;
	}
    }
#எனில் defined(FEAT_GUI) || defined(FEAT_EVAL)
    இல்லையெனில்
    {
	எனில் (!init || !(HL_TABLE()[idx].sg_set & SG_GUI))
	{
	    எனில் (!init)
		HL_TABLE()[idx].sg_set |= SG_GUI;
	    HL_TABLE()[idx].sg_gui = attr;
	}
    }
#endif

    திரும்பு TRUE;
}

#ifdef FEAT_GUI
/*
 * Set the font for the highlight group at 'idx'.
 * 'arg' is the font name.
 * Returns TRUE if the font is changed.
 */
    நிலையான முழு
highlight_set_font(
	முழு	idx,
	char_u	*arg,
	முழு	is_normal_group,
	முழு	is_menu_group,
	முழு	is_tooltip_group)
{
    முழு		did_change = FALSE;

    // in non-GUI fonts are simply ignored
    எனில் (HL_TABLE()[idx].sg_font_name != வெற்று
	    && STRCMP(HL_TABLE()[idx].sg_font_name, arg) == 0)
    {
	// Font name didn't change, ignore.
    }
    இல்லையெனில் எனில் (!gui.shell_created)
    {
	// GUI not started yet, always accept the name.
	vim_free(HL_TABLE()[idx].sg_font_name);
	HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	did_change = TRUE;
    }
    இல்லையெனில்
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
	எனில் (HL_TABLE()[idx].sg_fontset != NOFONTSET)
	{
	    // New fontset was accepted. Free the old one, if there
	    // was one.
	    gui_mch_free_fontset(temp_sg_fontset);
	    did_change = TRUE;
	}
	இல்லையெனில்
	    HL_TABLE()[idx].sg_fontset = temp_sg_fontset;
# endif
	எனில் (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    // New font was accepted. Free the old one, if there was
	    // one.
	    gui_mch_free_font(temp_sg_font);
	    did_change = TRUE;
	}
	இல்லையெனில்
	    HL_TABLE()[idx].sg_font = temp_sg_font;
	// Update sg_font_name once here, after both font and fontset results
	// are known.
	எனில் (did_change)
	{
	    vim_free(HL_TABLE()[idx].sg_font_name);
	    HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	}
    }

    திரும்பு did_change;
}
#endif

/*
 * Set the cterm foreground color for the Normal highlight group to "color" and
 * the bold attribute to "bold".
 */
    நிலையான வெறுமை
hl_set_ctermfg_normal_group(முழு color, முழு bold)
{
    cterm_normal_fg_color = color + 1;
    cterm_normal_fg_bold = bold;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    எனில் (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	எனில் (termcap_active && color >= 0)
	    term_fg_color(color);
    }
}

/*
 * Set the cterm foreground color for the highlight group at 'idx' to 'color'.
 */
    நிலையான வெறுமை
highlight_set_ctermfg(முழு idx, முழு color, முழு is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_fg = color + 1;
    எனில் (is_normal_group)
	hl_set_ctermfg_normal_group(color,
					(HL_TABLE()[idx].sg_cterm & HL_BOLD));
}

/*
 * Set the cterm background color for the Normal highlight group to "color".
 */
    நிலையான வெறுமை
hl_set_ctermbg_normal_group(முழு color)
{
    cterm_normal_bg_color = color + 1;
#ifdef FEAT_GUI
    // Don't mess with 'background' if the GUI is used.
    எனில் (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	எனில் (color >= 0)
	{
	    முழு dark = -1;

	    எனில் (termcap_active)
		term_bg_color(color);
	    எனில் (t_colors < 16)
		dark = (color == 0 || color == 4);
	    // Limit the heuristic to the standard 16 colors
	    இல்லையெனில் எனில் (color < 16)
		dark = (color < 7 || color == 8);
	    // Set the 'background' option if the value is
	    // wrong.
	    எனில் (dark != -1
		    && dark != (*p_bg == 'd')
		    && !option_was_set((char_u *)"bg"))
	    {
		set_option_மதிப்பு_give_err((char_u *)"bg",
			0L, (char_u *)(dark ? "dark" : "light"), 0);
		reset_option_was_set((char_u *)"bg");
	    }
	}
    }
}

/*
 * Set the cterm background color for the highlight group at 'idx' to 'color'.
 */
    நிலையான வெறுமை
highlight_set_ctermbg(முழு idx, முழு color, முழு is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_bg = color + 1;
    எனில் (is_normal_group)
	hl_set_ctermbg_normal_group(color);
}

/*
 * Set the cterm underline color for the Normal highlight group to "color".
 */
    நிலையான வெறுமை
hl_set_ctermul_normal_group(முழு color)
{
    cterm_normal_ul_color = color + 1;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    எனில் (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	எனில் (termcap_active && color >= 0)
	    term_ul_color(color);
    }
}

/*
 * Set the cterm underline color for the highlight group at 'idx' to 'color'.
 */
    நிலையான வெறுமை
highlight_set_ctermul(முழு idx, முழு color, முழு is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_ul = color + 1;
    எனில் (is_normal_group)
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
    நிலையான முழு
highlight_set_cterm_font(
	முழு	idx,
	char_u	*arg,
	முழு	init)
{
    முழு		font;

    எனில் (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	திரும்பு FALSE;

    எனில் (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    எனில் (VIM_ISDIGIT(*arg))
	font = atoi((எழுத்து *)arg);
    இல்லையெனில் எனில் (STRICMP(arg, "NONE") == 0)
	font = -1;
    இல்லையெனில்
	திரும்பு FALSE;

    HL_TABLE()[idx].sg_cterm_font = font + 1;
    திரும்பு TRUE;
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
    நிலையான முழு
highlight_set_cterm_color(
	முழு	idx,
	char_u	*சாவி,
	char_u	*சாவி_start,
	char_u	*arg,
	முழு	is_normal_group,
	முழு	init)
{
    முழு		color;

    எனில் (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	திரும்பு FALSE;

    எனில் (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    // When setting the foreground color, and previously the "bold"
    // flag was set for a light color, reset it now
    எனில் (சாவி[5] == 'F' && HL_TABLE()[idx].sg_cterm_bold)
    {
	HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
	HL_TABLE()[idx].sg_cterm_bold = FALSE;
    }

    எனில் (VIM_ISDIGIT(*arg))
	color = atoi((எழுத்து *)arg);
    இல்லையெனில் எனில் (STRICMP(arg, "fg") == 0)
    {
	எனில் (cterm_normal_fg_color)
	    color = cterm_normal_fg_color - 1;
	இல்லையெனில்
	{
	    emsg(_(e_fg_color_unknown));
	    திரும்பு FALSE;
	}
    }
    இல்லையெனில் எனில் (STRICMP(arg, "bg") == 0)
    {
	எனில் (cterm_normal_bg_color > 0)
	    color = cterm_normal_bg_color - 1;
	இல்லையெனில்
	{
	    emsg(_(e_bg_color_unknown));
	    திரும்பு FALSE;
	}
    }
    இல்லையெனில் எனில் (STRICMP(arg, "ul") == 0)
    {
	எனில் (cterm_normal_ul_color > 0)
	    color = cterm_normal_ul_color - 1;
	இல்லையெனில்
	{
	    emsg(_(e_ul_color_unknown));
	    திரும்பு FALSE;
	}
    }
    இல்லையெனில்
    {
	முழு bold = MAYBE;
	keyvalue_T target;
	keyvalue_T *entry;

	target.சாவி = 0;
	target.மதிப்பு.string = arg;
	target.மதிப்பு.length = 0;	// not used, see cmp_keyvalue_value_i()
	entry = (keyvalue_T *)bsearch(&target, &color_name_tab,
		ARRAY_LENGTH(color_name_tab), அளவு(color_name_tab[0]),
		cmp_keyvalue_மதிப்பு_i);
	எனில் (entry == வெற்று)
	{
	    semsg(_(e_color_name_or_எண்_not_recognized_str), சாவி_start);
	    திரும்பு FALSE;
	}

	color = lookup_color(entry->சாவி, சாவி[5] == 'F', &bold);

	// set/reset bold attribute to get light foreground
	// colors (on some terminals, e.g. "linux")
	எனில் (bold == TRUE)
	{
	    HL_TABLE()[idx].sg_cterm |= HL_BOLD;
	    HL_TABLE()[idx].sg_cterm_bold = TRUE;
	}
	இல்லையெனில் எனில் (bold == FALSE)
	    HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
    }

    // Add one to the argument, to avoid zero.  Zero is used for
    // "NONE", then "color" is -1.
    எனில் (சாவி[5] == 'F')
	highlight_set_ctermfg(idx, color, is_normal_group);
    இல்லையெனில் எனில் (சாவி[5] == 'B')
	highlight_set_ctermbg(idx, color, is_normal_group);
    இல்லையெனில் // ctermul
	highlight_set_ctermul(idx, color, is_normal_group);

    திரும்பு TRUE;
}

#எனில் defined(FEAT_GUI) || defined(FEAT_EVAL)
/*
 * Set the GUI foreground color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    நிலையான முழு
highlight_set_guifg(
	முழு	idx,
	char_u	*arg,
	முழு	is_menu_group UNUSED,
	முழு	is_scrollbar_group UNUSED,
	முழு	is_tooltip_group UNUSED,
	முழு	is_titlebar_group UNUSED,
	முழு	is_titlebarnc_group UNUSED,
	முழு	*do_colors UNUSED,
	முழு	init)
{
# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    long	i;
# endif
    char_u	**namep;
    முழு		did_change = FALSE;

    எனில் (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	திரும்பு FALSE;

    namep = &HL_TABLE()[idx].sg_gui_fg_name;
    எனில் (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guifg colors are only used when recognized
    i = color_name2handle(arg);
    எனில் (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_fg = i;
# endif
	எனில் (*namep == வெற்று || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    எனில் (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    இல்லையெனில்
		*namep = வெற்று;
	    did_change = TRUE;
	}
# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	எனில் (is_titlebar_group && gui.title_fg_pixel != i)
	{
	    gui.title_fg_pixel = i;
	    *do_colors = TRUE;
	}
	எனில் (is_titlebarnc_group && gui.titlenc_fg_pixel != i)
	{
	    gui.titlenc_fg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	எனில் (is_menu_group && gui.menu_fg_pixel != i)
	{
	    gui.menu_fg_pixel = i;
	    *do_colors = TRUE;
	}
	எனில் (is_scrollbar_group && gui.scroll_fg_pixel != i)
	{
	    gui.scroll_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	எனில் (is_tooltip_group && gui.tooltip_fg_pixel != i)
	{
	    gui.tooltip_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    திரும்பு did_change;
}

/*
 * Set the GUI background color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    நிலையான முழு
highlight_set_guibg(
	முழு	idx,
	char_u	*arg,
	முழு	is_menu_group UNUSED,
	முழு	is_scrollbar_group UNUSED,
	முழு	is_tooltip_group UNUSED,
	முழு	is_titlebar_group UNUSED,
	முழு	is_titlebarnc_group UNUSED,
	முழு	*do_colors UNUSED,
	முழு	init)
{
# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    முழு		i;
# endif
    char_u	**namep;
    முழு		did_change = FALSE;

    எனில் (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	திரும்பு FALSE;

    namep = &HL_TABLE()[idx].sg_gui_bg_name;
    எனில் (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guibg colors are only used when recognized
    i = color_name2handle(arg);
    எனில் (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_bg = i;
# endif
	எனில் (*namep == வெற்று || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    எனில் (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    இல்லையெனில்
		*namep = வெற்று;
	    did_change = TRUE;
	}
# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	எனில் (is_titlebar_group && gui.title_bg_pixel != i)
	{
	    gui.title_bg_pixel = i;
	    *do_colors = TRUE;
	}
	எனில் (is_titlebarnc_group && gui.titlenc_bg_pixel != i)
	{
	    gui.titlenc_bg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	எனில் (is_menu_group && gui.menu_bg_pixel != i)
	{
	    gui.menu_bg_pixel = i;
	    *do_colors = TRUE;
	}
	எனில் (is_scrollbar_group && gui.scroll_bg_pixel != i)
	{
	    gui.scroll_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	எனில் (is_tooltip_group && gui.tooltip_bg_pixel != i)
	{
	    gui.tooltip_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    திரும்பு did_change;
}

/*
 * Set the GUI undercurl/strikethrough color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    நிலையான முழு
highlight_set_guisp(முழு idx, char_u *arg, முழு init)
{
# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    முழு		i;
# endif
    முழு		did_change = FALSE;
    char_u	**namep;

    எனில் (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	திரும்பு FALSE;

    namep = &HL_TABLE()[idx].sg_gui_sp_name;
    எனில் (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guisp colors are only used when recognized
    i = color_name2handle(arg);
    எனில் (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_sp = i;
# endif
	எனில் (*namep == வெற்று || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    எனில் (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    இல்லையெனில்
		*namep = வெற்று;
	    did_change = TRUE;
	}
# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    }
# endif

    திரும்பு did_change;
}
#endif

/*
 * Set the start/stop terminal codes for a highlight group.
 * Returns TRUE if the terminal code is set.
 */
    நிலையான முழு
highlight_set_startstop_termcode(முழு idx, char_u *சாவி, char_u *arg, முழு init)
{
    முழு		off;
    char_u	buf[100];
    முழு		len;
    char_u	*tname;
    char_u	*p;

    எனில் (!init)
	HL_TABLE()[idx].sg_set |= SG_TERM;

    // The "start" and "stop"  arguments can be a literal escape
    // sequence, or a comma separated list of terminal codes.
    எனில் (STRNCMP(arg, "t_", 2) == 0)
    {
	off = 0;
	buf[0] = 0;
	போது (arg[off] != NUL)
	{
	    // Isolate one termcap name
	    க்கு (len = 0; arg[off + len] &&
		    arg[off + len] != ','; ++len)
		;
	    tname = vim_strnsave(arg + off, len);
	    எனில் (tname == வெற்று)		// out of memory
		திரும்பு FALSE;
	    // lookup the escape sequence for the item
	    p = get_term_code(tname);
	    vim_free(tname);
	    எனில் (p == வெற்று)	    // ignore non-existing things
		p = (char_u *)"";

	    // Append it to the already found stuff
	    எனில் ((முழு)(STRLEN(buf) + STRLEN(p)) >= 99)
	    {
		semsg(_(e_terminal_code_too_long_str), arg);
		திரும்பு FALSE;
	    }
	    STRCAT(buf, p);

	    // Advance to the next item
	    off += len;
	    எனில் (arg[off] == ',')	    // another one follows
		++off;
	}
    }
    இல்லையெனில்
    {
	// Copy characters from arg[] to buf[], translating <> codes.
	க்கு (p = arg, off = 0; off < 100 - 6 && *p; )
	{
	    len = trans_special(&p, buf + off, FSK_SIMPLIFY, FALSE, வெற்று);
	    எனில் (len > 0)	    // recognized special char
		off += len;
	    இல்லையெனில்		    // copy as normal char
		buf[off++] = *p++;
	}
	buf[off] = NUL;
    }

    எனில் (STRCMP(buf, "NONE") == 0)	// resetting the value
	p = வெற்று;
    இல்லையெனில்
	p = vim_strsave(buf);
    எனில் (சாவி[2] == 'A')
    {
	vim_free(HL_TABLE()[idx].sg_start);
	HL_TABLE()[idx].sg_start = p;
    }
    இல்லையெனில்
    {
	vim_free(HL_TABLE()[idx].sg_stop);
	HL_TABLE()[idx].sg_stop = p;
    }
    திரும்பு TRUE;
}

/*
 * Handle the ":highlight .." command.
 * When using ":hi clear" this is called recursively for each group with
 * "forceit" and "init" both TRUE.
 */
    வெறுமை
do_highlight(
    char_u	*வரி,
    முழு		forceit,
    முழு		init)	    // TRUE when called for initializing
{
    char_u	*name_end;
    char_u	*linep;
    char_u	*சாவி_start;
    char_u	*arg_start;
    char_u	*சாவி = வெற்று, *arg = வெற்று;
    long	i;
    முழு		id;
    முழு		idx;
    hl_group_T	item_before;
    முழு		did_change = FALSE;
    முழு		dodefault = FALSE;
    முழு		doclear = FALSE;
    முழு		dolink = FALSE;
    முழு		error = FALSE;
    முழு		is_normal_group = FALSE;	// "Normal" group
#ifdef FEAT_GUI_MSWIN
    முழு		is_titlebar_group = FALSE;	// "TitleBar" group
    முழு		is_titlebarnc_group = FALSE;	// "TitleBarNC" group
#இல்லையெனில்
# define is_titlebar_group 0
# define is_titlebarnc_group 0
#endif
#ifdef FEAT_GUI_X11
    முழு		is_menu_group = FALSE;		// "Menu" group
    முழு		is_scrollbar_group = FALSE;	// "Scrollbar" group
    முழு		is_tooltip_group = FALSE;	// "Tooltip" group
#இல்லையெனில்
# define is_menu_group 0
# define is_tooltip_group 0
# define is_scrollbar_group 0
#endif
#எனில் defined(FEAT_GUI) || defined(FEAT_EVAL)
    முழு		do_colors = FALSE;		// need to update colors?
#endif
#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    முழு		did_highlight_changed = FALSE;
#endif

    // If no argument, list current highlighting.
    எனில் (!init && ends_excmd2(வரி - 1, வரி))
    {
	க்கு (i = 1; i <= highlight_ga.ga_len && !got_int; ++i)
	    // TODO: only call when the group has attributes set
	    highlight_list_one((முழு)i);
	திரும்பு;
    }

    // Isolate the name.
    name_end = skiptowhite(வரி);
    linep = skipwhite(name_end);

    // Check for "default" argument.
    எனில் (STRNCMP(வரி, "default", name_end - வரி) == 0)
    {
	dodefault = TRUE;
	வரி = linep;
	name_end = skiptowhite(வரி);
	linep = skipwhite(name_end);
    }

    // Check for "clear" or "link" argument.
    எனில் (STRNCMP(வரி, "clear", name_end - வரி) == 0)
	doclear = TRUE;
    எனில் (STRNCMP(வரி, "link", name_end - வரி) == 0)
	dolink = TRUE;

    // ":highlight {group-name}": list highlighting for one group.
    எனில் (!doclear && !dolink && ends_excmd2(வரி, linep))
    {
	id = syn_namen2id(வரி, (முழு)(name_end - வரி));
	எனில் (id == 0)
	    semsg(_(e_highlight_group_name_not_found_str), வரி);
	இல்லையெனில்
	    highlight_list_one(id);
	திரும்பு;
    }

    // Handle ":highlight link {from} {to}" command.
    எனில் (dolink)
    {
	char_u	    *from_start = linep;
	char_u	    *from_end;
	முழு	    from_len;
	char_u	    *to_start;
	char_u	    *to_end;
	முழு	    to_len;

	from_end = skiptowhite(from_start);
	to_start = skipwhite(from_end);
	to_end	 = skiptowhite(to_start);

	எனில் (ends_excmd2(வரி, from_start) || ends_excmd2(வரி, to_start))
	{
	    semsg(_(e_not_enough_arguments_highlight_link_str), from_start);
	    திரும்பு;
	}

	எனில் (!ends_excmd2(வரி, skipwhite(to_end)))
	{
	    semsg(_(e_too_many_arguments_highlight_link_str), from_start);
	    திரும்பு;
	}

	from_len = (முழு)(from_end - from_start);
	to_len = (முழு)(to_end - to_start);
	highlight_group_link(from_start, from_len, to_start, to_len,
						dodefault, forceit, init);
	திரும்பு;
    }

    எனில் (doclear)
    {
	// ":highlight clear [group]" command.
	எனில் (ends_excmd2(வரி, linep))
	{
	    // ":highlight clear" without group name
	    highlight_reset_all();
	    திரும்பு;
	}
	வரி = linep;
	name_end = skiptowhite(வரி);
	linep = skipwhite(name_end);
    }

    // Find the group name in the table.  If it does not exist yet, add it.
    id = syn_check_group(வரி, (முழு)(name_end - வரி));
    எனில் (id == 0)	// failed (out of memory)
	திரும்பு;
    idx = id - 1;	// index is ID minus one

    // Return if "default" was used and the group already has settings.
    எனில் (dodefault && hl_has_settings(idx, TRUE))
	திரும்பு;

    // Make a copy so we can check if any attribute actually changed.
    item_before = HL_TABLE()[idx];

    எனில் (STRCMP(HL_TABLE()[idx].sg_name_u, "NORMAL") == 0)
	is_normal_group = TRUE;
#ifdef FEAT_GUI_MSWIN
    இல்லையெனில் எனில் (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBAR") == 0)
	is_titlebar_group = TRUE;
    இல்லையெனில் எனில் (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBARNC") == 0)
	is_titlebarnc_group = TRUE;
#endif
#ifdef FEAT_GUI_X11
    இல்லையெனில் எனில் (STRCMP(HL_TABLE()[idx].sg_name_u, "MENU") == 0)
	is_menu_group = TRUE;
    இல்லையெனில் எனில் (STRCMP(HL_TABLE()[idx].sg_name_u, "SCROLLBAR") == 0)
	is_scrollbar_group = TRUE;
    இல்லையெனில் எனில் (STRCMP(HL_TABLE()[idx].sg_name_u, "TOOLTIP") == 0)
	is_tooltip_group = TRUE;
#endif

    // Clear the highlighting for ":hi clear {group}" and ":hi clear".
    எனில் (doclear || (forceit && init))
    {
	highlight_clear(idx);
	எனில் (!doclear)
	    HL_TABLE()[idx].sg_set = 0;
    }

    எனில் (!doclear)
	போது (!ends_excmd2(வரி, linep))
	{
	    சாவி_start = linep;
	    எனில் (*linep == '=')
	    {
		semsg(_(e_unexpected_equal_sign_str), சாவி_start);
		error = TRUE;
		நிறுத்து;
	    }

	    // Note: Keep this in sync with get_highlight_group_key.

	    // Isolate the key ("term", "ctermfg", "ctermbg", "font", "guifg"
	    // or "guibg").
	    போது (*linep && !VIM_ISWHITE(*linep) && *linep != '=')
		++linep;
	    vim_free(சாவி);
	    சாவி = vim_strnsave_up(சாவி_start, linep - சாவி_start);
	    எனில் (சாவி == வெற்று)
	    {
		error = TRUE;
		நிறுத்து;
	    }
	    linep = skipwhite(linep);

	    எனில் (STRCMP(சாவி, "NONE") == 0)
	    {
		எனில் (!init || HL_TABLE()[idx].sg_set == 0)
		{
		    எனில் (!init)
			HL_TABLE()[idx].sg_set |= SG_TERM+SG_CTERM+SG_GUI;
		    highlight_clear(idx);
		}
		தொடர்;
	    }

	    // Check for the equal sign.
	    எனில் (*linep != '=')
	    {
		semsg(_(e_missing_equal_sign_str_2), சாவி_start);
		error = TRUE;
		நிறுத்து;
	    }
	    ++linep;

	    // Isolate the argument.
	    linep = skipwhite(linep);
	    எனில் (*linep == '\'')		// guifg='color name'
	    {
		arg_start = ++linep;
		linep = vim_strchr(linep, '\'');
		எனில் (linep == வெற்று)
		{
		    semsg(_(e_invalid_argument_str), சாவி_start);
		    error = TRUE;
		    நிறுத்து;
		}
	    }
	    இல்லையெனில்
	    {
		arg_start = linep;
		linep = skiptowhite(linep);
	    }
	    எனில் (linep == arg_start)
	    {
		semsg(_(e_missing_argument_str), சாவி_start);
		error = TRUE;
		நிறுத்து;
	    }
	    vim_free(arg);
	    arg = vim_strnsave(arg_start, linep - arg_start);
	    எனில் (arg == வெற்று)
	    {
		error = TRUE;
		நிறுத்து;
	    }
	    எனில் (*linep == '\'')
		++linep;

	    // Store the argument.
	    எனில் (STRCMP(சாவி, "TERM") == 0
		    || STRCMP(சாவி, "CTERM") == 0
		    || STRCMP(சாவி, "GUI") == 0)
	    {
		எனில் (!highlight_set_termgui_attr(idx, சாவி, arg, init))
		{
		    error = TRUE;
		    நிறுத்து;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(சாவி, "FONT") == 0)
	    {
		// in non-GUI fonts are simply ignored
#ifdef FEAT_GUI
		எனில் (highlight_set_font(idx, arg, is_normal_group,
					      is_menu_group, is_tooltip_group))
		    did_change = TRUE;
#endif
	    }
	    இல்லையெனில் எனில் (STRCMP(சாவி, "CTERMFG") == 0
		    || STRCMP(சாவி, "CTERMBG") == 0
		    || STRCMP(சாவி, "CTERMUL") == 0)
	    {
		எனில் (!highlight_set_cterm_color(idx, சாவி, சாவி_start, arg,
							is_normal_group, init))
		{
		    error = TRUE;
		    நிறுத்து;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(சாவி, "CTERMFONT") == 0)
	    {
		எனில் (!highlight_set_cterm_font(idx, arg, init))
		{
		    error = TRUE;
		    நிறுத்து;
		}
	    }
	    இல்லையெனில் எனில் (STRCMP(சாவி, "GUIFG") == 0)
	    {
#எனில் defined(FEAT_GUI) || defined(FEAT_EVAL)
		எனில் (highlight_set_guifg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    இல்லையெனில் எனில் (STRCMP(சாவி, "GUIBG") == 0)
	    {
#எனில் defined(FEAT_GUI) || defined(FEAT_EVAL)
		எனில் (highlight_set_guibg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    இல்லையெனில் எனில் (STRCMP(சாவி, "GUISP") == 0)
	    {
#எனில் defined(FEAT_GUI) || defined(FEAT_EVAL)
		எனில் (highlight_set_guisp(idx, arg, init))
		    did_change = TRUE;
#endif
	    }
	    இல்லையெனில் எனில் (STRCMP(சாவி, "START") == 0 || STRCMP(சாவி, "STOP") == 0)
	    {
		எனில் (!highlight_set_startstop_termcode(idx, சாவி, arg, init))
		{
		    error = TRUE;
		    நிறுத்து;
		}
	    }
	    இல்லையெனில்
	    {
		semsg(_(e_illegal_argument_str_3), சாவி_start);
		error = TRUE;
		நிறுத்து;
	    }
	    HL_TABLE()[idx].sg_cleared = FALSE;

	    // When highlighting has been given for a group, don't link it.
	    எனில் (!init || !(HL_TABLE()[idx].sg_set & SG_LINK))
		HL_TABLE()[idx].sg_link = 0;

	    // Continue with next argument.
	    linep = skipwhite(linep);
	}

    // If there is an error, and it's a new entry, remove it from the table.
    எனில் (error && idx == highlight_ga.ga_len)
	syn_unadd_group();
    இல்லையெனில்
    {
	எனில் (is_normal_group)
	{
	    HL_TABLE()[idx].sg_term_attr = 0;
	    HL_TABLE()[idx].sg_cterm_attr = 0;
#ifdef FEAT_GUI
	    HL_TABLE()[idx].sg_gui_attr = 0;
	    // Need to update all groups, because they might be using "bg"
	    // and/or "fg", which have been changed now.
#endif
#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    எனில் (USE_24BIT)
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
	இல்லையெனில் எனில் (is_titlebar_group)
	{
	    எனில் (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
	இல்லையெனில் எனில் (is_titlebarnc_group)
	{
	    எனில் (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
#endif
#ifdef FEAT_GUI_X11
# ifdef FEAT_MENU
	இல்லையெனில் எனில் (is_menu_group)
	{
	    எனில் (gui.in_use && do_colors)
		gui_mch_new_menu_colors();
	}
# endif
	இல்லையெனில் எனில் (is_scrollbar_group)
	{
	    எனில் (gui.in_use && do_colors)
		gui_new_scrollbar_colors();
	    இல்லையெனில்
		set_hl_attr(idx);
	}
# ifdef FEAT_BEVAL_GUI
	இல்லையெனில் எனில் (is_tooltip_group)
	{
	    எனில் (gui.in_use && do_colors)
		gui_mch_new_tooltip_colors();
	}
# endif
#endif
	இல்லையெனில்
	    set_hl_attr(idx);
#ifdef FEAT_EVAL
	HL_TABLE()[idx].sg_script_ctx = current_sctx;
	HL_TABLE()[idx].sg_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
    }

    vim_free(சாவி);
    vim_free(arg);

    // Only call highlight_changed() once, after a sequence of highlight
    // commands, and only if an attribute actually changed.
    எனில் ((did_change
	   || memcmp(&HL_TABLE()[idx], &item_before, அளவு(item_before)) != 0)
#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    && !did_highlight_changed
#endif
       )
    {
	// Do not trigger a redraw when highlighting is changed while
	// redrawing.  This may happen when evaluating 'statusline' changes the
	// StatusLine group.
	எனில் (!updating_screen)
	    redraw_all_later(UPD_NOT_VALID);
	need_highlight_changed = TRUE;
    }
}

#எனில் defined(EXITFREE)
    வெறுமை
free_highlight(வெறுமை)
{
    முழு	    i;

    க்கு (i = 0; i < highlight_ga.ga_len; ++i)
    {
	highlight_clear(i);
	vim_free(HL_TABLE()[i].sg_name);
	vim_free(HL_TABLE()[i].sg_name_u - HLNAME_சாவி_OFF);
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
    வெறுமை
restore_cterm_colors(வெறுமை)
{
#எனில் defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
    // Since t_me has been set, this probably means that the user
    // wants to use this as default colors.  Need to reset default
    // background/foreground colors.
    mch_set_normal_colors();
#இல்லையெனில்
# ifdef VIMDLL
    எனில் (!gui.in_use)
    {
	mch_set_normal_colors();
	திரும்பு;
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
    நிலையான முழு
hl_has_settings(முழு idx, முழு check_link)
{
    திரும்பு HL_TABLE()[idx].sg_cleared == 0
	 && (  HL_TABLE()[idx].sg_term_attr != 0
	    || HL_TABLE()[idx].sg_cterm_attr != 0
	    || HL_TABLE()[idx].sg_cterm_fg != 0
	    || HL_TABLE()[idx].sg_cterm_bg != 0
	    || HL_TABLE()[idx].sg_cterm_font != 0
#ifdef FEAT_GUI
	    || HL_TABLE()[idx].sg_gui_attr != 0
	    || HL_TABLE()[idx].sg_gui_fg_name != வெற்று
	    || HL_TABLE()[idx].sg_gui_bg_name != வெற்று
	    || HL_TABLE()[idx].sg_gui_sp_name != வெற்று
	    || HL_TABLE()[idx].sg_font_name != வெற்று
#endif
	    || (check_link && (HL_TABLE()[idx].sg_set & SG_LINK)));
}

/*
 * Clear highlighting for one group.
 */
    நிலையான வெறுமை
highlight_clear(முழு idx)
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
#எனில் defined(FEAT_GUI) || defined(FEAT_EVAL)
    HL_TABLE()[idx].sg_gui = 0;
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_fg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_bg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_sp_name);
#endif
#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Set the normal foreground and background colors according to the "Normal"
 * highlighting group.  For X11 also set "Menu", "Scrollbar" and "Tooltip"
 * colors.  For MS-Windows also set "TitleBar" and "TitleBarNC" colors.
 */
    வெறுமை
set_normal_colors(வெறுமை)
{
# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    எனில் (gui.in_use)
#  endif
    {
	எனில் (set_group_colors((char_u *)"Normal",
				 &gui.norm_pixel, &gui.back_pixel,
				 FALSE, TRUE, FALSE))
	{
	    gui_mch_new_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  ifdef FEAT_GUI_MSWIN
	எனில் (set_group_colors((char_u *)"TitleBar",
				 &gui.title_fg_pixel, &gui.title_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
	எனில் (set_group_colors((char_u *)"TitleBarNC",
				 &gui.titlenc_fg_pixel, &gui.titlenc_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  endif
#  ifdef FEAT_GUI_X11
	எனில் (set_group_colors((char_u *)"Menu",
			     &gui.menu_fg_pixel, &gui.menu_bg_pixel,
			     TRUE, FALSE, FALSE))
	{
#   ifdef FEAT_MENU
	    gui_mch_new_menu_colors();
#   endif
	    set_must_redraw(UPD_CLEAR);
	}
#   ifdef FEAT_BEVAL_GUI
	எனில் (set_group_colors((char_u *)"Tooltip",
			     &gui.tooltip_fg_pixel, &gui.tooltip_bg_pixel,
			     FALSE, FALSE, TRUE))
	{
#    ifdef FEAT_TOOLBAR
	    gui_mch_new_tooltip_colors();
#    endif
	    set_must_redraw(UPD_CLEAR);
	}
#   endif
	எனில் (set_group_colors((char_u *)"Scrollbar",
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
    இல்லையெனில்
#  endif
    {
	முழு		idx;

	idx = syn_name2id((char_u *)"Normal") - 1;
	எனில் (idx >= 0)
	{
	    gui_do_one_color(idx, FALSE, FALSE);

	    // If the normal fg or bg color changed a complete redraw is
	    // required.
	    எனில் (cterm_normal_fg_gui_color != HL_TABLE()[idx].sg_gui_fg
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

#எனில் defined(FEAT_GUI)
/*
 * Set the colors for "Normal", "Menu", "TitleBar", "TitleBarNC", "Tooltip" or
 * "Scrollbar".
 */
    நிலையான முழு
set_group_colors(
    char_u	*name,
    guicolor_T	*fgp,
    guicolor_T	*bgp,
    முழு		do_menu,
    முழு		use_norm,
    முழு		do_tooltip)
{
    முழு		idx;

    idx = syn_name2id(name) - 1;
    எனில் (idx < 0)
	திரும்பு FALSE;

    gui_do_one_color(idx, do_menu, do_tooltip);

    எனில் (HL_TABLE()[idx].sg_gui_fg != INVALCOLOR)
	*fgp = HL_TABLE()[idx].sg_gui_fg;
    இல்லையெனில் எனில் (use_norm)
	*fgp = gui.def_norm_pixel;
    எனில் (HL_TABLE()[idx].sg_gui_bg != INVALCOLOR)
	*bgp = HL_TABLE()[idx].sg_gui_bg;
    இல்லையெனில் எனில் (use_norm)
	*bgp = gui.def_back_pixel;
    திரும்பு TRUE;
}

/*
 * Get the font of the "Normal" group.
 * Returns "" when it's not found or not set.
 */
    char_u *
hl_get_font_name(வெறுமை)
{
    முழு		id;
    char_u	*s;

    id = syn_name2id((char_u *)"Normal");
    எனில் (id > 0)
    {
	s = HL_TABLE()[id - 1].sg_font_name;
	எனில் (s != வெற்று)
	    திரும்பு s;
    }
    திரும்பு (char_u *)"";
}

/*
 * Set font for "Normal" group.  Called by gui_mch_init_font() when a font has
 * actually chosen to be used.
 */
    வெறுமை
hl_set_font_name(char_u *font_name)
{
    முழு	    id;

    id = syn_name2id((char_u *)"Normal");
    எனில் (id <= 0)
	திரும்பு;

    vim_free(HL_TABLE()[id - 1].sg_font_name);
    HL_TABLE()[id - 1].sg_font_name = vim_strsave(font_name);
}

/*
 * Set background color for "Normal" group.  Called by gui_set_bg_color()
 * when the color is known.
 */
    வெறுமை
hl_set_bg_color_name(
    char_u  *name)	    // must have been allocated
{
    முழு	    id;

    எனில் (name == வெற்று)
	திரும்பு;

    id = syn_name2id((char_u *)"Normal");
    எனில் (id <= 0)
	திரும்பு;

    vim_free(HL_TABLE()[id - 1].sg_gui_bg_name);
    HL_TABLE()[id - 1].sg_gui_bg_name = name;
}

/*
 * Set foreground color for "Normal" group.  Called by gui_set_fg_color()
 * when the color is known.
 */
    வெறுமை
hl_set_fg_color_name(
    char_u  *name)	    // must have been allocated
{
    முழு	    id;

    எனில் (name == வெற்று)
	திரும்பு;

    id = syn_name2id((char_u *)"Normal");
    எனில் (id <= 0)
	திரும்பு;

    vim_free(HL_TABLE()[id - 1].sg_gui_fg_name);
    HL_TABLE()[id - 1].sg_gui_fg_name = name;
}

/*
 * Return the handle for a font name.
 * Returns NOFONT when failed.
 */
    நிலையான GuiFont
font_name2handle(char_u *name)
{
    எனில் (STRCMP(name, "NONE") == 0)
	திரும்பு NOFONT;

    திரும்பு gui_mch_get_font(name, TRUE);
}

# ifdef FEAT_XFONTSET
/*
 * Return the handle for a fontset name.
 * Returns NOFONTSET when failed.
 */
    நிலையான GuiFontset
fontset_name2handle(char_u *name, முழு fixed_width)
{
    எனில் (STRCMP(name, "NONE") == 0)
	திரும்பு NOFONTSET;

    திரும்பு gui_mch_get_fontset(name, TRUE, fixed_width);
}
# endif

/*
 * Get the font or fontset for one highlight group.
 */
    நிலையான வெறுமை
hl_do_font(
    முழு		idx,
    char_u	*arg,
    முழு		do_normal,		// set normal font
    முழு		do_menu UNUSED,		// set menu font
    முழு		do_tooltip UNUSED,	// set tooltip font
    முழு		free_font)		// free current font/fontset
{
# ifdef FEAT_XFONTSET
    // If 'guifontset' is not empty, first try using the name as a
    // fontset.  If that doesn't work, use it as a font name.
    எனில் (*p_guifontset != NUL
#  ifdef FONTSET_ALWAYS
	|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
	// In Motif, the Tooltip highlight group is always a fontset
	|| do_tooltip
#  endif
	    )
    {
	எனில் (free_font)
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
    எனில் (HL_TABLE()[idx].sg_fontset != NOFONTSET)
    {
	// If it worked and it's the Normal group, use it as the normal
	// fontset.  Same for the Menu group.
	எனில் (do_normal)
	    gui_init_font(arg, TRUE);
#  எனில் defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	எனில் (do_menu)
	{
#   ifdef FONTSET_ALWAYS
	    gui.menu_fontset = HL_TABLE()[idx].sg_fontset;
#   இல்லையெனில்
	    // YIKES!  This is a bug waiting to crash the program
	    gui.menu_font = HL_TABLE()[idx].sg_fontset;
#   endif
	    gui_mch_new_menu_font();
	}
#   ifdef FEAT_BEVAL_GUI
	எனில் (do_tooltip)
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
    இல்லையெனில்
# endif
    {
	எனில் (free_font)
	    gui_mch_free_font(HL_TABLE()[idx].sg_font);
	HL_TABLE()[idx].sg_font = font_name2handle(arg);
	// If it worked and it's the Normal group, use it as the
	// normal font.  Same for the Menu group.
	எனில் (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    எனில் (do_normal)
		gui_init_font(arg, FALSE);
# ifndef FONTSET_ALWAYS
#  எனில் defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	    எனில் (do_menu)
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

#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return the handle for a color name.
 * Returns INVALCOLOR when failed.
 */
    guicolor_T
color_name2handle(char_u *name)
{
    எனில் (STRCMP(name, "NONE") == 0)
	திரும்பு INVALCOLOR;

    எனில் (STRICMP(name, "fg") == 0 || STRICMP(name, "foreground") == 0)
    {
# எனில் defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	எனில் (gui.in_use)
# endif
# ifdef FEAT_GUI
	    திரும்பு gui.norm_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	எனில் (cterm_normal_fg_gui_color != INVALCOLOR)
	    திரும்பு cterm_normal_fg_gui_color;
	// Guess that the foreground is black or white.
	திரும்பு GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "black" : "white"));
# endif
    }
    எனில் (STRICMP(name, "bg") == 0 || STRICMP(name, "background") == 0)
    {
# எனில் defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	எனில் (gui.in_use)
# endif
# ifdef FEAT_GUI
	    திரும்பு gui.back_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	எனில் (cterm_normal_bg_gui_color != INVALCOLOR)
	    திரும்பு cterm_normal_bg_gui_color;
	// Guess that the background is white or black.
	திரும்பு GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "white" : "black"));
# endif
    }

    திரும்பு GUI_GET_COLOR(name);
}

// On MS-Windows an RGB macro is available and it produces 0x00bbggrr color
// values as used by the MS-Windows GDI api.  It should be used only for
// MS-Windows GDI builds.
# எனில் defined(RGB) && defined(MSWIN) && !defined(FEAT_GUI)
#  undef RGB
# endif
# ifndef RGB
#  define RGB(r, g, b)	(((r)<<16) | ((g)<<8) | (b))
# endif

# ifdef VIMDLL
    நிலையான guicolor_T
gui_adjust_rgb(guicolor_T c)
{
    எனில் (gui.in_use)
	திரும்பு c;
    இல்லையெனில்
	திரும்பு ((c & 0xff) << 16) | (c & 0x00ff00) | ((c >> 16) & 0xff);
}
# இல்லையெனில்
#  define gui_adjust_rgb(c) (c)
# endif

    நிலையான முழு
hex_digit(முழு c)
{
    எனில் (SAFE_isdigit(c))
	திரும்பு c - '0';
    c = TOLOWER_ASC(c);
    எனில் (c >= 'a' && c <= 'f')
	திரும்பு c - 'a' + 10;
    திரும்பு 0x1ffffff;
}

    நிலையான guicolor_T
decode_hex_color(char_u *hex)
{
    guicolor_T color;

    எனில் (hex[0] != '#' || STRLEN(hex) != 7)
	திரும்பு INVALCOLOR;

    // Name is in "#rrggbb" format
    color = RGB(((hex_digit(hex[1]) << 4) + hex_digit(hex[2])),
		((hex_digit(hex[3]) << 4) + hex_digit(hex[4])),
		((hex_digit(hex[5]) << 4) + hex_digit(hex[6])));
    எனில் (color > 0xffffff)
	திரும்பு INVALCOLOR;
    திரும்பு gui_adjust_rgb(color);
}

# ifdef FEAT_EVAL
// Returns the color currently mapped to the given name or INVALCOLOR if no
// such name exists in the color table. The convention is to use lowercase for
// all keys in the v:colornames dictionary. The value can be either a string in
// the form #rrggbb or a number, either of which is converted to a guicolor_T.
    நிலையான guicolor_T
colorname2rgb(char_u *name)
{
    dict_T      *colornames_table = get_vim_var_dict(VV_COLORNAMES);
    char_u      *lc_name;
    dictitem_T  *colentry;
    char_u      *colstr;
    varnumber_T colnum;

    lc_name = strlow_save(name);
    எனில் (lc_name == வெற்று)
	திரும்பு INVALCOLOR;

    colentry = dict_find(colornames_table, lc_name, -1);
    vim_free(lc_name);
    எனில் (colentry == வெற்று)
	திரும்பு INVALCOLOR;

    எனில் (colentry->di_tv.v_type == VAR_STRING)
    {
	colstr = tv_get_string_strict(&colentry->di_tv);
	எனில் ((STRLEN(colstr) == 7) && (*colstr == '#'))
	{
	    திரும்பு decode_hex_color(colstr);
	}
	இல்லையெனில்
	{
	    semsg(_(e_bad_color_string_str), colstr);
	    திரும்பு INVALCOLOR;
	}
    }

    எனில் (colentry->di_tv.v_type == VAR_எண்)
    {
	colnum = tv_get_எண்(&colentry->di_tv);
	திரும்பு (guicolor_T)colnum;
    }

    திரும்பு INVALCOLOR;
}

# endif

    guicolor_T
gui_get_color_cmn(char_u *name)
{
    guicolor_T  color;
    // Only non X11 colors (not present in rgb.txt) and colors in
    // color_name_tab[], useful when $VIMRUNTIME is not found,.
    // must be sorted by the 'value' field because it is used by bsearch()!
    நிலையான keyvalue_T rgb_tab[] = {
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
    எனில் (color != INVALCOLOR)
	திரும்பு color;

    target.சாவி = 0;
    target.மதிப்பு.string = name;
    target.மதிப்பு.length = 0;	// not used, see cmp_keyvalue_value_i()
    entry = (keyvalue_T *)bsearch(&target, &rgb_tab, ARRAY_LENGTH(rgb_tab),
	    அளவு(rgb_tab[0]), cmp_keyvalue_மதிப்பு_i);
    எனில் (entry != வெற்று)
	திரும்பு gui_adjust_rgb((guicolor_T)entry->சாவி);

# எனில் defined(FEAT_EVAL)
    /*
     * Not a traditional color. Load additional color aliases and then consult the alias table.
     */

    color = colorname2rgb(name);
    எனில் (color == INVALCOLOR)
    {
	load_default_colors_lists();
	color = colorname2rgb(name);
    }

    திரும்பு color;
# இல்லையெனில்
    திரும்பு INVALCOLOR;
# endif
}

    guicolor_T
gui_get_rgb_color_cmn(முழு r, முழு g, முழு b)
{
    guicolor_T  color = RGB(r, g, b);

    எனில் (color > 0xffffff)
	திரும்பு INVALCOLOR;
    திரும்பு gui_adjust_rgb(color);
}
#endif

/*
 * Table with the specifications for an attribute number.
 * Note that this table is used by ALL buffers.  This is required because the
 * GUI can redraw at any time for any buffer.
 */
நிலையான garray_T	term_attr_table = {0, 0, 0, 0, வெற்று};

#define TERM_ATTR_ENTRY(idx) ((attrentry_T *)term_attr_table.ga_data)[idx]

நிலையான garray_T	cterm_attr_table = {0, 0, 0, 0, வெற்று};

#define CTERM_ATTR_ENTRY(idx) ((attrentry_T *)cterm_attr_table.ga_data)[idx]

#ifdef FEAT_GUI
நிலையான garray_T	gui_attr_table = {0, 0, 0, 0, வெற்று};

# define GUI_ATTR_ENTRY(idx) ((attrentry_T *)gui_attr_table.ga_data)[idx]
#endif

/*
 * Return the attr number for a set of colors and font.
 * Add a new entry to the term_attr_table, cterm_attr_table or gui_attr_table
 * if the combination is new.
 * Return 0 for error (no more room).
 */
    நிலையான முழு
get_attr_entry(garray_T *table, attrentry_T *aep)
{
    முழு		i;
    attrentry_T	*taep;
    நிலையான முழு	recursive = FALSE;

    // Init the table, in case it wasn't done yet.
    table->ga_itemsize = அளவு(attrentry_T);
    table->ga_growsize = 7;

    // Try to find an entry with the same specifications.
    க்கு (i = 0; i < table->ga_len; ++i)
    {
	taep = &(((attrentry_T *)table->ga_data)[i]);
	எனில் (	   aep->ae_attr == taep->ae_attr
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
			&& (aep->ae_u.term.start == வெற்று)
					    == (taep->ae_u.term.start == வெற்று)
			&& (aep->ae_u.term.start == வெற்று
			    || STRCMP(aep->ae_u.term.start,
						  taep->ae_u.term.start) == 0)
			&& (aep->ae_u.term.stop == வெற்று)
					     == (taep->ae_u.term.stop == வெற்று)
			&& (aep->ae_u.term.stop == வெற்று
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

	திரும்பு i + ATTR_OFF;
    }

    எனில் (table->ga_len + ATTR_OFF > MAX_TYPENR)
    {
	// Running out of attribute entries!  remove all attributes, and
	// compute new ones for all groups.
	// When called recursively, we are really out of numbers.
	எனில் (recursive)
	{
	    emsg(_(e_too_many_different_highlighting_attributes_in_use));
	    திரும்பு 0;
	}
	recursive = TRUE;

	clear_hl_tables();

	set_must_redraw(UPD_CLEAR);

	க்கு (i = 0; i < highlight_ga.ga_len; ++i)
	    set_hl_attr(i);

	recursive = FALSE;
    }

    // This is a new combination of colors and font, add an entry.
    எனில் (ga_grow(table, 1) == FAIL)
	திரும்பு 0;

    taep = &(((attrentry_T *)table->ga_data)[table->ga_len]);
    CLEAR_POINTER(taep);
    taep->ae_attr = aep->ae_attr;
#ifdef FEAT_GUI
    எனில் (table == &gui_attr_table)
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
    எனில் (table == &term_attr_table)
    {
	எனில் (aep->ae_u.term.start == வெற்று)
	    taep->ae_u.term.start = வெற்று;
	இல்லையெனில்
	    taep->ae_u.term.start = vim_strsave(aep->ae_u.term.start);
	எனில் (aep->ae_u.term.stop == வெற்று)
	    taep->ae_u.term.stop = வெற்று;
	இல்லையெனில்
	    taep->ae_u.term.stop = vim_strsave(aep->ae_u.term.stop);
    }
    இல்லையெனில் எனில் (table == &cterm_attr_table)
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
    திரும்பு (table->ga_len - 1 + ATTR_OFF);
}

#எனில் defined(FEAT_TERMINAL)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    முழு
get_cterm_attr_idx(முழு attr, முழு fg, முழு bg)
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
    திரும்பு get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#எனில் defined(FEAT_TERMINAL) && defined(FEAT_TERMGUICOLORS)
/*
 * Get an attribute index for a 'termguicolors' entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    முழு
get_tgc_attr_idx(முழு attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    எனில் (fg == INVALCOLOR && bg == INVALCOLOR)
    {
	// If both GUI colors are not set fall back to the cterm colors.  Helps
	// if the GUI only has an attribute, such as undercurl.
	at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
    }
    இல்லையெனில்
    {
	at_en.ae_u.cterm.fg_rgb = fg;
	at_en.ae_u.cterm.bg_rgb = bg;
    }
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
    திரும்பு get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#எனில் defined(FEAT_TERMINAL) && defined(FEAT_GUI)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    முழு
get_gui_attr_idx(முழு attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    at_en.ae_u.gui.fg_color = fg;
    at_en.ae_u.gui.bg_color = bg;
    திரும்பு get_attr_entry(&gui_attr_table, &at_en);
}
#endif

/*
 * Clear all highlight tables.
 */
    வெறுமை
clear_hl_tables(வெறுமை)
{
    முழு		i;
    attrentry_T	*taep;

#ifdef FEAT_GUI
    ga_clear(&gui_attr_table);
#endif
    க்கு (i = 0; i < term_attr_table.ga_len; ++i)
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
    முழு
hl_combine_attr(முழு char_attr, முழு prim_attr)
{
    attrentry_T *char_aep = வெற்று;
    attrentry_T *prim_aep;
    attrentry_T new_en;

    எனில் (char_attr == 0)
	திரும்பு prim_attr;
    எனில் (char_attr <= HL_ALL && prim_attr <= HL_ALL)
	திரும்பு ATTR_COMBINE(char_attr, prim_attr);
#ifdef FEAT_GUI
    எனில் (gui.in_use)
    {
	எனில் (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	எனில் (char_aep != வெற்று)
	    new_en = *char_aep;
	இல்லையெனில்
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    எனில் (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	எனில் (prim_attr <= HL_ALL)
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	இல்லையெனில்
	{
	    prim_aep = syn_gui_attr2entry(prim_attr);
	    எனில் (prim_aep != வெற்று)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		எனில் (prim_aep->ae_u.gui.fg_color != INVALCOLOR)
		    new_en.ae_u.gui.fg_color = prim_aep->ae_u.gui.fg_color;
		எனில் (prim_aep->ae_u.gui.bg_color != INVALCOLOR)
		    new_en.ae_u.gui.bg_color = prim_aep->ae_u.gui.bg_color;
		எனில் (prim_aep->ae_u.gui.sp_color != INVALCOLOR)
		    new_en.ae_u.gui.sp_color = prim_aep->ae_u.gui.sp_color;
		எனில் (prim_aep->ae_u.gui.font != NOFONT)
		    new_en.ae_u.gui.font = prim_aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
		எனில் (prim_aep->ae_u.gui.fontset != NOFONTSET)
		    new_en.ae_u.gui.fontset = prim_aep->ae_u.gui.fontset;
# endif
	    }
	}
	திரும்பு get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    எனில் (IS_CTERM)
    {
	எனில் (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	எனில் (char_aep != வெற்று)
	    new_en = *char_aep;
	இல்லையெனில்
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    எனில் (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	எனில் (prim_attr <= HL_ALL)
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	இல்லையெனில்
	{
	    prim_aep = syn_cterm_attr2entry(prim_attr);
	    எனில் (prim_aep != வெற்று)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		எனில் (prim_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color = prim_aep->ae_u.cterm.fg_color;
		எனில் (prim_aep->ae_u.cterm.bg_color > 0)
		    new_en.ae_u.cterm.bg_color = prim_aep->ae_u.cterm.bg_color;
		எனில் (prim_aep->ae_u.cterm.ul_color > 0)
		    new_en.ae_u.cterm.ul_color = prim_aep->ae_u.cterm.ul_color;
		எனில் (prim_aep->ae_u.cterm.font > 0)
		    new_en.ae_u.cterm.font = prim_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		// If both fg and bg are not set fall back to cterm colors.
		// Helps for SpellBad which uses undercurl in the GUI.
		எனில் (COLOR_INVALID(prim_aep->ae_u.cterm.fg_rgb)
			&& COLOR_INVALID(prim_aep->ae_u.cterm.bg_rgb))
		{
		    எனில் (prim_aep->ae_u.cterm.fg_color > 0)
			new_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
		    எனில் (prim_aep->ae_u.cterm.bg_color > 0)
			new_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
		}
		இல்லையெனில்
		{
		    எனில் (prim_aep->ae_u.cterm.fg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.fg_rgb = prim_aep->ae_u.cterm.fg_rgb;
		    எனில் (prim_aep->ae_u.cterm.bg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.bg_rgb = prim_aep->ae_u.cterm.bg_rgb;
		}
		எனில் (prim_aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		    new_en.ae_u.cterm.ul_rgb = prim_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	}
	திரும்பு get_attr_entry(&cterm_attr_table, &new_en);
    }

    எனில் (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    எனில் (char_aep != வெற்று)
	new_en = *char_aep;
    இல்லையெனில்
    {
	CLEAR_FIELD(new_en);
	எனில் (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    எனில் (prim_attr <= HL_ALL)
	new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
    இல்லையெனில்
    {
	prim_aep = syn_term_attr2entry(prim_attr);
	எனில் (prim_aep != வெற்று)
	{
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_aep->ae_attr);
	    எனில் (prim_aep->ae_u.term.start != வெற்று)
	    {
		new_en.ae_u.term.start = prim_aep->ae_u.term.start;
		new_en.ae_u.term.stop = prim_aep->ae_u.term.stop;
	    }
	}
    }
    திரும்பு get_attr_entry(&term_attr_table, &new_en);
}

// ANSI color order: Black, Red, Green, Yellow, Blue, Magenta,
// Cyan, White, then bright variants.  Approximate RGB values used when
// only a cterm color number is known (no guifg/guibg).  Real terminal
// palettes may differ if the user reconfigured their emulator, but
// these are reasonable xterm-ish defaults.
நிலையான மாறிலி long_u cterm_color_16[16] = {
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
    நிலையான guicolor_T
cterm_color_to_rgb(முழு color_nr)
{
    எனில் (color_nr < 1 || color_nr > 16)
	திரும்பு INVALCOLOR;
    திரும்பு (guicolor_T)cterm_color_16[color_nr - 1];
}
#endif

/*
 * Convert an xterm 256-color index (0-255) to an approximate RGB triple.
 * Uses the standard xterm palette: 0-15 ANSI (cterm_color_16), 16-231
 * 6x6x6 cube, 232-255 grayscale ramp.
 */
    நிலையான வெறுமை
cterm_idx_to_rgb(முழு idx, முழு *r, முழு *g, முழு *b)
{
    நிலையான மாறிலி முழு cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

    எனில் (idx < 0 || idx > 255)
    {
	*r = *g = *b = 0;
	திரும்பு;
    }
    எனில் (idx < 16)
    {
	long_u rgb = cterm_color_16[idx];
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
    }
    இல்லையெனில் எனில் (idx < 232)
    {
	முழு n = idx - 16;
	*r = cube[n / 36 % 6];
	*g = cube[n / 6  % 6];
	*b = cube[n      % 6];
    }
    இல்லையெனில்
    {
	முழு v = 8 + (idx - 232) * 10;
	*r = *g = *b = v;
    }
}

/*
 * Approximate an RGB triple to the nearest xterm 256-color index.
 * Searches the 6x6x6 cube and the grayscale ramp; returns 0-255.
 */
    நிலையான முழு
rgb_to_cterm_idx(முழு r, முழு g, முழு b)
{
    நிலையான மாறிலி முழு cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };
    முழு best = 0;
    long best_d = -1;
    முழு i;

    // Search the 6x6x6 cube.
    க்கு (i = 16; i < 232; ++i)
    {
	முழு n = i - 16;
	முழு cr = cube[n / 36 % 6];
	முழு cg = cube[n / 6  % 6];
	முழு cb = cube[n      % 6];
	long d = (cr - r) * (cr - r) + (cg - g) * (cg - g) + (cb - b) * (cb - b);
	எனில் (best_d < 0 || d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    // Search the grayscale ramp.
    க்கு (i = 232; i < 256; ++i)
    {
	முழு v = 8 + (i - 232) * 10;
	long d = (v - r) * (v - r) + (v - g) * (v - g) + (v - b) * (v - b);
	எனில் (d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    திரும்பு best;
}

/*
 * Resolve a color to an RGB triple.  Prefer the gui RGB value (set by
 * guifg/guibg) when valid, otherwise convert the cterm 256-color index.
 * Returns true on success; false when no color is defined.
 * cterm_c is 1-based (0 means "no color").
 */
    நிலையான bool
resolve_color_to_rgb(முழு cterm_c, guicolor_T rgb UNUSED, முழு *r, முழு *g, முழு *b)
{
#ifdef FEAT_TERMGUICOLORS
    எனில் (!COLOR_INVALID(rgb))
    {
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
	திரும்பு true;
    }
#endif
    எனில் (cterm_c > 0)
    {
	cterm_idx_to_rgb(cterm_c - 1, r, g, b);
	திரும்பு true;
    }
    திரும்பு false;
}

/*
 * get a RGB fallback color from gui, cterm or default color
 */
    நிலையான guicolor_T
resolve_fallback_color(முழு cterm_c, guicolor_T rgb, guicolor_T default_rgb)
{
    முழு red, green, blue;
    எனில் (!resolve_color_to_rgb(cterm_c, rgb, &red, &green, &blue))
	திரும்பு default_rgb;
    இல்லையெனில்
	திரும்பு (red << 16) | (green << 8) | blue;
}

/*
 * get a RGB fallback foreground color from guifg, ctermfg or deduced from background
 */
    நிலையான வெறுமை
resolve_fallback_fg_to_rgb(வெறுமை)
{
    guicolor_T fgcolor_or_gui_fgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    fgcolor_or_gui_fgcolor = cterm_normal_fg_gui_color;
#endif
#ifdef FEAT_GUI
    எனில் (gui.in_use)
	fgcolor_or_gui_fgcolor = gui.norm_pixel;
#endif
    fallback_fg_rgb = resolve_fallback_color(cterm_normal_fg_color, fgcolor_or_gui_fgcolor, (*p_bg == 'l') ? 0x000000 : 0xFFFFFF);
}

/*
 * get a RGB fallback background color from guifg, ctermbg or deduced from background
 */
    நிலையான வெறுமை
resolve_fallback_bg_to_rgb(வெறுமை)
{
    guicolor_T bgcolor_or_gui_bgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    bgcolor_or_gui_bgcolor = cterm_normal_bg_gui_color;
#endif
#ifdef FEAT_GUI
    எனில் (gui.in_use)
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
    நிலையான முழு
blend_cterm_colors(முழு popup_c, guicolor_T popup_rgb,
		   முழு under_c, guicolor_T under_rgb,
		   முழு default_rgb,
		   முழு blend_val)
{
    முழு pr, pg, pb, ur, ug, ub, r, g, b;

    எனில் (t_colors < 256)
	திரும்பு popup_c;
    எனில் (!resolve_color_to_rgb(popup_c, popup_rgb, &pr, &pg, &pb))
	திரும்பு under_c;
    எனில் (blend_val <= 0)
	திரும்பு rgb_to_cterm_idx(pr, pg, pb) + 1;
    எனில் (!resolve_color_to_rgb(under_c, under_rgb, &ur, &ug, &ub))
    {
	ur = (default_rgb >> 16) & 0xFF;
	ug = (default_rgb >> 8) & 0xFF;
	ub = default_rgb & 0xFF;
    }
    எனில் (blend_val >= 100)
	திரும்பு rgb_to_cterm_idx(ur, ug, ub) + 1;
    r = pr + (ur - pr) * blend_val / 100;
    g = pg + (ug - pg) * blend_val / 100;
    b = pb + (ub - pb) * blend_val / 100;
    திரும்பு rgb_to_cterm_idx(r, g, b) + 1;
}

#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Blend two RGB colors based on blend value (0-100).
 * blend: 0=return first argument color, 100=return second argument color
 * If bg_color is INVALCOLOR, high blend means more visible (return INVALCOLOR).
 */
    நிலையான guicolor_T
blend_colors(guicolor_T popup_color, guicolor_T bg_color, முழு blend_val)
{
    முழு r1, g1, b1, r2, g2, b2, r, g, b;

    // CTERMCOLOR is a sentinel meaning "use the cterm color"; for blending
    // it has no real RGB so treat it like INVALCOLOR.
    எனில் (COLOR_INVALID(popup_color))
	திரும்பு INVALCOLOR;

    // Fully transparent: use underlying color as-is.
    எனில் (blend_val >= 100)
	திரும்பு bg_color;

    r1 = (popup_color >> 16) & 0xFF;
    g1 = (popup_color >> 8) & 0xFF;
    b1 = popup_color & 0xFF;

    எனில் (COLOR_INVALID(bg_color))
	bg_color = fallback_bg_rgb;

    r2 = (bg_color >> 16) & 0xFF;
    g2 = (bg_color >> 8) & 0xFF;
    b2 = bg_color & 0xFF;

    r = r1 + (r2 - r1) * blend_val / 100;
    g = g1 + (g2 - g1) * blend_val / 100;
    b = b1 + (b2 - b1) * blend_val / 100;

    திரும்பு (r << 16) | (g << 8) | b;
}
#endif

/*
 * Blend attributes for popup windows and menus with opacity.
 * Blends foreground and/or background colors based on blend value (0-100).
 * blend: 0 = opaque (use popup colors), 100 = transparent (use background colors)
 * blend_fg: TRUE to blend foreground color, FALSE to keep popup foreground
 */
    நிலையான முழு
hl_blend_attr_common(
    முழு		char_attr,
    முழு		popup_attr,
    முழு		blend,
    முழு		blend_fg)
{
    attrentry_T *char_aep = வெற்று;
    attrentry_T *popup_aep;
    attrentry_T new_en;
    attrentry_T tmp_en;

#ifdef FEAT_GUI
    எனில் (gui.in_use)
    {
	எனில் (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	எனில் (char_aep != வெற்று)
	    new_en = *char_aep;
	இல்லையெனில்
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    எனில் (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	எனில் (popup_attr <= HL_ALL)
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
	இல்லையெனில்
	    popup_aep = syn_gui_attr2entry(popup_attr);

	எனில் (popup_aep != வெற்று)
	{
	    guicolor_T popup_bg_rgb = popup_aep->ae_u.gui.bg_color;
	    எனில் (COLOR_INVALID(popup_bg_rgb))
		popup_bg_rgb = fallback_bg_rgb;

	    எனில் (blend_fg)
	    {
		// blend_fg=TRUE: fade underlying text toward popup bg.
		{
		    முழு base_fg = fallback_fg_rgb;
		    எனில் (char_aep != வெற்று
			    && char_aep->ae_u.gui.fg_color != INVALCOLOR)
			base_fg = char_aep->ae_u.gui.fg_color;
		    new_en.ae_u.gui.fg_color = blend_colors(
			    popup_bg_rgb, base_fg, blend);
		}
	    }
	    இல்லையெனில்
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
		எனில் (char_aep != வெற்று
			&& !COLOR_INVALID(char_aep->ae_u.gui.bg_color))
		    underlying_bg = char_aep->ae_u.gui.bg_color;
		new_en.ae_u.gui.bg_color = blend_colors(
			popup_bg_rgb,
			underlying_bg, blend);
	    }
	}
	திரும்பு get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    எனில் (IS_CTERM)
    {
	எனில் (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	எனில் (char_aep != வெற்று)
	    new_en = *char_aep;
	இல்லையெனில்
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    எனில் (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	எனில் (popup_attr <= HL_ALL)
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
	இல்லையெனில்
	    popup_aep = syn_cterm_attr2entry(popup_attr);

	எனில் (popup_aep != வெற்று)
	{
	    guicolor_T popup_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
	    // Fall back to cterm color converted to RGB when gui color is not set.
	    popup_bg_rgb = popup_aep->ae_u.cterm.bg_rgb;
	    எனில் (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color > 0)
		popup_bg_rgb = cterm_color_to_rgb(
			popup_aep->ae_u.cterm.bg_color);
#endif
	    // assign default color if guibg and ctermbg are not set for popup
	    எனில் (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color == 0)
		popup_bg_rgb = fallback_bg_rgb;

	    எனில் (!blend_fg)
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
		எனில் (popup_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color =
			popup_aep->ae_u.cterm.fg_color;
		இல்லையெனில் எனில் (cterm_normal_fg_color > 0)
		    new_en.ae_u.cterm.fg_color = cterm_normal_fg_color;
		இல்லையெனில்
		    // black-ish or white-ish
		    new_en.ae_u.cterm.fg_color = (*p_bg == 'l') ? 1 : 16;
		new_en.ae_u.cterm.ul_color = popup_aep->ae_u.cterm.ul_color;
		new_en.ae_u.cterm.font = popup_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		new_en.ae_u.cterm.ul_rgb = popup_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	    இல்லையெனில்
	    {
		// blend_fg=TRUE: fade underlying fg toward popup bg in
		// the 256-color palette.  Used when the popup is over a
		// cell rendered with cterm colors (no termguicolors RGB).
		முழு under_fg = (char_aep != வெற்று)
		    ? char_aep->ae_u.cterm.fg_color : 0;
		guicolor_T under_fg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		எனில் (char_aep != வெற்று)
		    under_fg_rgb = char_aep->ae_u.cterm.fg_rgb;
#endif
		new_en.ae_u.cterm.fg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_fg, under_fg_rgb, fallback_fg_rgb, blend);
	    }
	    // Approximate cterm bg by blending with the underlying bg
	    // in the 256-color palette and mapping to the nearest entry.
	    {
		முழு under_bg = (char_aep != வெற்று)
		    ? char_aep->ae_u.cterm.bg_color : 0;
		guicolor_T under_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		எனில் (char_aep != வெற்று)
		    under_bg_rgb = char_aep->ae_u.cterm.bg_rgb;
#endif
		new_en.ae_u.cterm.bg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_bg, under_bg_rgb, fallback_bg_rgb, blend);
	    }
#ifdef FEAT_TERMGUICOLORS
	    // Blend RGB colors for termguicolors mode.
	    {
		எனில் (blend_fg)
		{
		    // blend_fg=TRUE: fade underlying text toward popup bg.
		    எனில் (popup_bg_rgb != INVALCOLOR)
		    {
			முழு base_fg = fallback_fg_rgb;
			// CTERMCOLOR is a sentinel meaning "use the cterm
			// color"; treat it as no underlying color so it is
			// not blended in as a real near-white pixel.
			எனில் (char_aep != வெற்று
				&& !COLOR_INVALID(char_aep->ae_u.cterm.fg_rgb))
			    base_fg = char_aep->ae_u.cterm.fg_rgb;
			new_en.ae_u.cterm.fg_rgb = blend_colors(
				popup_bg_rgb, base_fg, blend);
		    }
		}
		இல்லையெனில்
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

		    எனில் (COLOR_INVALID(popup_fg)
			    && popup_aep->ae_u.cterm.fg_color > 0)
			popup_fg = cterm_color_to_rgb(
				popup_aep->ae_u.cterm.fg_color);

		    எனில் (!COLOR_INVALID(popup_fg))
			new_en.ae_u.cterm.fg_rgb = popup_fg;
		    இல்லையெனில் எனில் (!COLOR_INVALID(cterm_normal_fg_gui_color))
			new_en.ae_u.cterm.fg_rgb = cterm_normal_fg_gui_color;
		    இல்லையெனில்
			new_en.ae_u.cterm.fg_rgb = fallback_fg_rgb;
		}
		எனில் (popup_bg_rgb != INVALCOLOR)
		{
		    // Blend popup bg toward underlying bg
		    guicolor_T underlying_bg = fallback_bg_rgb;
		    எனில் (char_aep != வெற்று
			    && !COLOR_INVALID(char_aep->ae_u.cterm.bg_rgb))
			underlying_bg = char_aep->ae_u.cterm.bg_rgb;
		    new_en.ae_u.cterm.bg_rgb = blend_colors(
			    popup_bg_rgb, underlying_bg, blend);
		}
	    }
#endif
	}
	திரும்பு get_attr_entry(&cterm_attr_table, &new_en);
    }

    எனில் (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    எனில் (char_aep != வெற்று)
	new_en = *char_aep;
    இல்லையெனில்
    {
	CLEAR_FIELD(new_en);
	எனில் (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    // For term mode, no separate background color handling.
    திரும்பு get_attr_entry(&term_attr_table, &new_en);
}

/*
 * Blend attributes for popup windows with opacity.
 */
    முழு
hl_blend_attr(முழு char_attr, முழு popup_attr, முழு blend, முழு blend_fg)
{
    எனில் (blend >= 100 && blend_fg)
	திரும்பு char_attr;

    திரும்பு hl_blend_attr_common(char_attr, popup_attr, blend, blend_fg);
}

/*
 * Blend underlying foreground and background colors for pum space cells.
 */
    முழு
hl_pum_blend_attr(முழு char_attr, முழு popup_attr, முழு blend)
{
    திரும்பு hl_blend_attr_common(char_attr, popup_attr, blend, TRUE);
}

#ifdef FEAT_GUI
    attrentry_T *
syn_gui_attr2entry(முழு attr)
{
    attr -= ATTR_OFF;
    எனில் (attr >= gui_attr_table.ga_len)	    // did ":syntax clear"
	திரும்பு வெற்று;
    திரும்பு &(GUI_ATTR_ENTRY(attr));
}
#endif

/*
 * Get the highlight attributes (HL_BOLD etc.) from an attribute nr.
 * Only to be used when "attr" > HL_ALL.
 */
    முழு
syn_attr2attr(முழு attr)
{
    attrentry_T	*aep;

#ifdef FEAT_GUI
    எனில் (gui.in_use)
	aep = syn_gui_attr2entry(attr);
    இல்லையெனில்
#endif
	எனில் (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	இல்லையெனில்
	    aep = syn_term_attr2entry(attr);

    எனில் (aep == வெற்று)	    // highlighting not set
	திரும்பு 0;
    திரும்பு aep->ae_attr;
}


    attrentry_T *
syn_term_attr2entry(முழு attr)
{
    attr -= ATTR_OFF;
    எனில் (attr >= term_attr_table.ga_len)	    // did ":syntax clear"
	திரும்பு வெற்று;
    திரும்பு &(TERM_ATTR_ENTRY(attr));
}

    attrentry_T *
syn_cterm_attr2entry(முழு attr)
{
    attr -= ATTR_OFF;
    எனில் (attr >= cterm_attr_table.ga_len)	// did ":syntax clear"
	திரும்பு வெற்று;
    திரும்பு &(CTERM_ATTR_ENTRY(attr));
}

#define LIST_ATTR   1
#define LIST_STRING 2
#define LIST_INT    3

    நிலையான வெறுமை
highlight_list_one(முழு id)
{
    hl_group_T	    *sgp;
    முழு		    didh = FALSE;

    sgp = &HL_TABLE()[id - 1];	    // index is ID minus one

    எனில் (message_filtered(sgp->sg_name))
	திரும்பு;

    // Note: Keep this in sync with expand_highlight_group().
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_term, வெற்று, "term");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_start, "start");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_stop, "stop");

    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_cterm, வெற்று, "cterm");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_fg, வெற்று, "ctermfg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_bg, வெற்று, "ctermbg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_ul, வெற்று, "ctermul");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_font, வெற்று, "ctermfont");

#எனில் defined(FEAT_GUI) || defined(FEAT_EVAL)
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_gui, வெற்று, "gui");
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

    எனில் (sgp->sg_link && !got_int)
    {
	(வெறுமை)syn_list_header(didh, 9999, id);
	didh = TRUE;
	செய்தி_puts_attr("links to", HL_ATTR(HLF_D));
	செய்தி_putchar(' ');
	செய்தி_outtrans(HL_TABLE()[HL_TABLE()[id - 1].sg_link - 1].sg_name);
    }

    எனில் (!didh)
	highlight_list_arg(id, didh, LIST_STRING, 0, (char_u *)"cleared", "");
#ifdef FEAT_EVAL
    எனில் (p_verbose > 0)
	last_set_செய்தி(sgp->sg_script_ctx);
#endif
}

    நிலையான char_u*
highlight_arg_to_string(
    முழு		type,
    முழு		iarg,
    char_u	*sarg,
    char_u	*buf)
{
    எனில் (type == LIST_INT)
	sprintf((எழுத்து *)buf, "%d", iarg - 1);
    இல்லையெனில் எனில் (type == LIST_STRING)
	திரும்பு sarg;
    இல்லையெனில் // type == LIST_ATTR
    {
	size_t buflen;

	buf[0] = NUL;
	buflen = 0;
	க்கு (முழு i = 0; i < (முழு)ARRAY_LENGTH(highlight_index_tab); ++i)
	{
	    எனில் (iarg & highlight_index_tab[i]->சாவி)
	    {
		எனில் (buflen > 0)
		{
		    STRCPY(buf + buflen, (char_u *)",");
		    ++buflen;
		}
		STRCPY(buf + buflen, highlight_index_tab[i]->மதிப்பு.string);
		buflen += highlight_index_tab[i]->மதிப்பு.length;
		iarg &= ~highlight_index_tab[i]->சாவி;	    // don't want "inverse"/"reverse"
	    }
	}
    }
    திரும்பு buf;
}

    நிலையான முழு
highlight_list_arg(
    முழு		id,
    முழு		didh,
    முழு		type,
    முழு		iarg,
    char_u	*sarg,
    எழுத்து	*name)
{
    char_u	buf[MAX_ATTR_LEN];
    char_u	*ts;

    எனில் (got_int)
	திரும்பு FALSE;

    எனில் (type == LIST_STRING ? (sarg == வெற்று) : (iarg == 0))
	திரும்பு didh;

    ts = highlight_arg_to_string(type, iarg, sarg, buf);

    (வெறுமை)syn_list_header(didh,
	    (முழு)(vim_strsize(ts) + STRLEN(name) + 1), id);
    didh = TRUE;
    எனில் (!got_int)
    {
	எனில் (*name != NUL)
	{
	    செய்தி_puts_attr(name, HL_ATTR(HLF_D));
	    செய்தி_puts_attr("=", HL_ATTR(HLF_D));
	}
	செய்தி_outtrans(ts);
    }
    திரும்பு didh;
}

#எனில் ((defined(FEAT_EVAL) || defined(FEAT_PRINTER))) && defined(FEAT_SYN_HL)
/*
 * Return "1" if highlight group "id" has attribute "flag".
 * Return NULL otherwise.
 */
    char_u *
highlight_has_attr(
    முழு		id,
    முழு		flag,
    முழு		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    முழு		attr;

    எனில் (id <= 0 || id > highlight_ga.ga_len)
	திரும்பு வெற்று;

# எனில் defined(FEAT_GUI) || defined(FEAT_EVAL)
    எனில் (modec == 'g')
	attr = HL_TABLE()[id - 1].sg_gui;
    இல்லையெனில்
# endif
    {
	எனில் (modec == 'c')
	    attr = HL_TABLE()[id - 1].sg_cterm;
	இல்லையெனில்
	    attr = HL_TABLE()[id - 1].sg_term;
    }

    எனில் (attr & flag)
	திரும்பு (char_u *)"1";
    திரும்பு வெற்று;
}
#endif

#எனில் defined(FEAT_SYN_HL) && defined(FEAT_EVAL)
/*
 * Return color name of highlight group "id".
 */
    char_u *
highlight_color(
    முழு		id,
    char_u	*what,	// "font", "fg", "bg", "sp", "ul", "fg#", "bg#" or "sp#"
    முழு		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    நிலையான char_u	name[20];
    முழு			n;
    முழு			fg = FALSE;
    முழு			sp = FALSE;
    முழு			ul = FALSE;
    முழு			font = FALSE;

    எனில் (id <= 0 || id > highlight_ga.ga_len)
	திரும்பு வெற்று;

    எனில் (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'g')
	fg = TRUE;
    இல்லையெனில் எனில் (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'o'
	     && TOLOWER_ASC(what[2]) == 'n' && TOLOWER_ASC(what[3]) == 't')
	font = TRUE;
    இல்லையெனில் எனில் (TOLOWER_ASC(what[0]) == 's' && TOLOWER_ASC(what[1]) == 'p')
	sp = TRUE;
    இல்லையெனில் எனில் (TOLOWER_ASC(what[0]) == 'u' && TOLOWER_ASC(what[1]) == 'l')
	ul = TRUE;
    இல்லையெனில் எனில் (!(TOLOWER_ASC(what[0]) == 'b' && TOLOWER_ASC(what[1]) == 'g'))
	திரும்பு வெற்று;
    எனில் (modec == 'g')
    {
# எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI
	// return font name
	எனில் (font)
	    திரும்பு HL_TABLE()[id - 1].sg_font_name;
#  endif

	// return #RRGGBB form (only possible when GUI is running)
	எனில் ((USE_24BIT) && what[2] == '#')
	{
	    guicolor_T		color;
	    long_u		rgb;
	    நிலையான char_u	buf[10];

	    எனில் (fg)
		color = HL_TABLE()[id - 1].sg_gui_fg;
	    இல்லையெனில் எனில் (sp)
		color = HL_TABLE()[id - 1].sg_gui_sp;
	    இல்லையெனில்
		color = HL_TABLE()[id - 1].sg_gui_bg;
	    எனில் (color == INVALCOLOR)
		திரும்பு வெற்று;
	    rgb = (long_u)GUI_MCH_GET_RGB(color);
	    sprintf((எழுத்து *)buf, "#%02x%02x%02x",
				      (unsigned)(rgb >> 16),
				      (unsigned)(rgb >> 8) & 255,
				      (unsigned)rgb & 255);
	    திரும்பு buf;
	}
# endif
	எனில் (fg)
	    திரும்பு (HL_TABLE()[id - 1].sg_gui_fg_name);
	எனில் (sp)
	    திரும்பு (HL_TABLE()[id - 1].sg_gui_sp_name);
	திரும்பு (HL_TABLE()[id - 1].sg_gui_bg_name);
    }
    எனில் (sp)
	திரும்பு வெற்று;
    எனில் (modec == 'c')
    {
	எனில் (fg)
	    n = HL_TABLE()[id - 1].sg_cterm_fg - 1;
	இல்லையெனில் எனில் (ul)
	    n = HL_TABLE()[id - 1].sg_cterm_ul - 1;
	இல்லையெனில் எனில் (font)
	    n = HL_TABLE()[id - 1].sg_cterm_font - 1;
	இல்லையெனில்
	    n = HL_TABLE()[id - 1].sg_cterm_bg - 1;
	எனில் (n < 0)
	    திரும்பு வெற்று;
	sprintf((எழுத்து *)name, "%d", n);
	திரும்பு name;
    }
    // term doesn't have color
    திரும்பு வெற்று;
}
#endif

#எனில் defined(FEAT_SYN_HL) \
	    && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)) \
	&& defined(FEAT_PRINTER)
/*
 * Return color name of highlight group "id" as RGB value.
 */
    long_u
highlight_gui_color_rgb(
    முழு		id,
    முழு		fg)	// TRUE = fg, FALSE = bg
{
    guicolor_T	color;

    எனில் (id <= 0 || id > highlight_ga.ga_len)
	திரும்பு 0L;

    எனில் (fg)
	color = HL_TABLE()[id - 1].sg_gui_fg;
    இல்லையெனில்
	color = HL_TABLE()[id - 1].sg_gui_bg;

    எனில் (color == INVALCOLOR)
	திரும்பு 0L;

    திரும்பு GUI_MCH_GET_RGB(color);
}
#endif

/*
 * Output the syntax list header.
 * Return TRUE when started a new line.
 */
    முழு
syn_list_header(
    முழு	    did_header,		// did header already
    முழு	    outlen,		// length of string that comes
    முழு	    id)			// highlight group id
{
    முழு	    endcol = 19;
    முழு	    newline = TRUE;
    முழு	    name_col = 0;

    எனில் (!did_header)
    {
	செய்தி_putchar('\n');
	எனில் (got_int)
	    திரும்பு TRUE;
	செய்தி_outtrans(HL_TABLE()[id - 1].sg_name);
	name_col = செய்தி_col;
	endcol = 15;
    }
    இல்லையெனில் எனில் (செய்தி_col + outlen + 1 >= cmdline_width)
    {
	செய்தி_putchar('\n');
	எனில் (got_int)
	    திரும்பு TRUE;
    }
    இல்லையெனில்
    {
	எனில் (செய்தி_col >= endcol)	// wrap around is like starting a new line
	    newline = FALSE;
    }

    எனில் (செய்தி_col >= endcol)	// output at least one space
	endcol = செய்தி_col + 1;
    எனில் (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    செய்தி_advance(endcol);

    // Show "xxx" with the attributes.
    எனில் (!did_header)
    {
	எனில் (endcol == cmdline_width - 1 && endcol <= name_col)
	    செய்தி_putchar(' ');
	செய்தி_puts_attr("xxx", syn_id2attr(id));
	செய்தி_putchar(' ');
    }

    திரும்பு newline;
}

/*
 * Set the attribute numbers for a highlight group.
 * Called after one of the attributes has changed.
 */
    நிலையான வெறுமை
set_hl_attr(
    முழு		idx)	    // index in array
{
    attrentry_T	    at_en;
    hl_group_T	    *sgp = HL_TABLE() + idx;

    // The "Normal" group doesn't need an attribute number
    எனில் (sgp->sg_name_u != வெற்று && STRCMP(sgp->sg_name_u, "NORMAL") == 0)
	திரும்பு;

#ifdef FEAT_GUI
    // For the GUI mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    எனில் (sgp->sg_gui_fg == INVALCOLOR
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
    இல்லையெனில்
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
    எனில் (sgp->sg_start == வெற்று && sgp->sg_stop == வெற்று)
	sgp->sg_term_attr = sgp->sg_term;
    இல்லையெனில்
    {
	at_en.ae_attr = sgp->sg_term;
	at_en.ae_u.term.start = sgp->sg_start;
	at_en.ae_u.term.stop = sgp->sg_stop;
	sgp->sg_term_attr = get_attr_entry(&term_attr_table, &at_en);
    }

    // For the color term mode: If there are other than "normal"
    // highlighting attributes, need to allocate an attr number.
    எனில் (sgp->sg_cterm_fg == 0 && sgp->sg_cterm_bg == 0 &&
	sgp->sg_cterm_ul == 0 && sgp->sg_cterm_font == 0
#ifdef FEAT_TERMGUICOLORS
	    && sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
#endif
	    )
	sgp->sg_cterm_attr = sgp->sg_cterm;
    இல்லையெனில்
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
	எனில் (sgp->sg_cterm & (HL_UNDERLINE | HL_UNDERCURL
			   | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED))
	    at_en.ae_u.cterm.ul_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_sp);
	இல்லையெனில்
	    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	எனில் (at_en.ae_u.cterm.fg_rgb == INVALCOLOR
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
    நிலையான முழு
syn_override(முழு id)
{
    எனில் (overrides != வெற்று && overrides->arr != வெற்று)
	க்கு (முழு k = 0; k < overrides->len; k++)
	    எனில் (overrides->arr[k].from == id)
		திரும்பு overrides->arr[k].to;
    திரும்பு id;
}

/*
 * Lookup a highlight group name by pointer and length.
 * If it is not found, 0 is returned.
 */
    நிலையான முழு
syn_name2id_len(char_u *name, முழு len)
{
    char_u	name_u[MAX_SYN_NAME + 1];
    hashitem_T	*hi;

    எனில் (len > MAX_SYN_NAME)
	len = MAX_SYN_NAME;
    mch_memmove(name_u, name, len);
    name_u[len] = NUL;
    vim_strup(name_u);
    எனில் (!highlight_ht_inited)
	திரும்பு 0;
    hi = hash_find(&highlight_ht, name_u);
    எனில் (HASHITEM_EMPTY(hi))
	திரும்பு 0;
    திரும்பு HI2HLNAME(hi)->hn_id;
}

/*
 * Lookup a highlight group name and return its ID.
 * If it is not found, 0 is returned.
 */
    முழு
syn_name2id(char_u *name)
{
    // Avoid using stricmp() too much, it's slow on some systems.
    திரும்பு syn_name2id_len(name, (முழு)STRLEN(name));
}

/*
 * Lookup a highlight group name and return its attributes.
 * Return zero if not found.
 */
    முழு
syn_name2attr(char_u *name)
{
    முழு id = syn_name2id(name);

    எனில் (id != 0)
	திரும்பு syn_id2attr(id);
    திரும்பு 0;
}

#எனில் defined(FEAT_EVAL)
/*
 * Return TRUE if highlight group "name" exists.
 */
    முழு
highlight_exists(char_u *name)
{
    திரும்பு (syn_name2id(name) > 0);
}

# எனில் defined(FEAT_தேடல்_EXTRA)
/*
 * Return the name of highlight group "id".
 * When not a valid ID return an empty string.
 */
    char_u *
syn_id2name(முழு id)
{
    எனில் (id <= 0 || id > highlight_ga.ga_len)
	திரும்பு (char_u *)"";
    திரும்பு HL_TABLE()[id - 1].sg_name;
}
# endif
#endif

/*
 * Like syn_name2id(), but take a pointer + length argument.
 */
    முழு
syn_namen2id(char_u *linep, முழு len)
{
    திரும்பு syn_name2id_len(linep, len);
}

/*
 * Find highlight group name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    முழு
syn_check_group(char_u *pp, முழு len)
{
    முழு	    id;
    char_u  *name;

    எனில் (len > MAX_SYN_NAME)
    {
	emsg(_(e_highlight_group_name_too_long));
	திரும்பு 0;
    }
    id = syn_name2id_len(pp, len);
    எனில் (id == 0)			// doesn't exist yet
    {
	name = vim_strnsave(pp, len);
	எனில் (name == வெற்று)
	    திரும்பு 0;
	id = syn_add_group(name);
    }
    திரும்பு id;
}

/*
 * Add new highlight group and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    நிலையான முழு
syn_add_group(char_u *name)
{
    char_u	*p;
    char_u	*name_up;

    // Check that the name is valid (ASCII letters, digits, underscores, dots, or hyphens).
    க்கு (p = name; *p != NUL; ++p)
    {
	எனில் (!vim_isprintc(*p))
	{
	    emsg(_(e_unprintable_character_in_group_name));
	    vim_free(name);
	    திரும்பு 0;
	}
	இல்லையெனில் எனில் (!ASCII_ISALNUM(*p) && *p != '_' && *p != '.' && *p != '-')
	{
	    // This is an error, but since there previously was no check only
	    // give a warning.
	    செய்தி_source(HL_ATTR(HLF_W));
	    செய்தி(_("W18: Invalid character in group name"));
	    நிறுத்து;
	}
    }

    // First call for this growarray: init growing array.
    எனில் (highlight_ga.ga_data == வெற்று)
    {
	highlight_ga.ga_itemsize = அளவு(hl_group_T);
	highlight_ga.ga_growsize = 10;
	hash_init(&highlight_ht);
	highlight_ht_inited = true;
    }

    எனில் (highlight_ga.ga_len >= MAX_HL_ID)
    {
	emsg(_(e_too_many_highlight_and_syntax_groups));
	vim_free(name);
	திரும்பு 0;
    }

    // Make room for at least one other syntax_highlight entry.
    எனில் (ga_grow(&highlight_ga, 1) == FAIL)
    {
	vim_free(name);
	திரும்பு 0;
    }

    {
	hlname_T    *hn;
	முழு	    len = (முழு)STRLEN(name);

	hn = alloc(offsetof(hlname_T, hn_சாவி) + len + 1);
	எனில் (hn == வெற்று)
	{
	    vim_free(name);
	    திரும்பு 0;
	}
	vim_strncpy(hn->hn_சாவி, name, len);
	vim_strup(hn->hn_சாவி);
	hn->hn_id = highlight_ga.ga_len + 1;	// ID is index plus one
	name_up = hn->hn_சாவி;
    }

    CLEAR_POINTER(&(HL_TABLE()[highlight_ga.ga_len]));
    HL_TABLE()[highlight_ga.ga_len].sg_name = name;
    HL_TABLE()[highlight_ga.ga_len].sg_name_u = name_up;
#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_TABLE()[highlight_ga.ga_len].sg_gui_bg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_fg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_sp = INVALCOLOR;
#endif
    hash_add(&highlight_ht, name_up, "highlight");
    ++highlight_ga.ga_len;

    திரும்பு highlight_ga.ga_len;		    // ID is index plus one
}

/*
 * When, just after calling syn_add_group(), an error is discovered, this
 * function deletes the new name.
 */
    நிலையான வெறுமை
syn_unadd_group(வெறுமை)
{
    hashitem_T *hi;

    --highlight_ga.ga_len;
    hi = hash_find(&highlight_ht, HL_TABLE()[highlight_ga.ga_len].sg_name_u);
    எனில் (!HASHITEM_EMPTY(hi))
	hash_remove(&highlight_ht, hi, "highlight");
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name);
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name_u - HLNAME_சாவி_OFF);
}

/*
 * Translate a group ID to highlight attributes.
 * "hl_id" must be valid: > 0, caller must check.
 */
    முழு
syn_id2attr(முழு hl_id)
{
    முழு		attr;
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ப்ரதிஜ்ஞா(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

#ifdef FEAT_GUI
    // Only use GUI attr when the GUI is being used.
    எனில் (gui.in_use)
	attr = sgp->sg_gui_attr;
    இல்லையெனில்
#endif
	எனில் (IS_CTERM)
	    attr = sgp->sg_cterm_attr;
	இல்லையெனில்
	    attr = sgp->sg_term_attr;

    திரும்பு attr;
}

#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Get the GUI colors and attributes for a group ID.
 * NOTE: the colors will be INVALCOLOR when not set, the color otherwise.
 */
    முழு
syn_id2colors(முழு hl_id, guicolor_T *fgp, guicolor_T *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ப்ரதிஜ்ஞா(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

    *fgp = sgp->sg_gui_fg;
    *bgp = sgp->sg_gui_bg;
    திரும்பு sgp->sg_gui;
}
#endif

#எனில் (defined(MSWIN) \
	    && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL)) \
	    && defined(FEAT_TERMGUICOLORS)) \
	|| defined(FEAT_TERMINAL)
    வெறுமை
syn_id2cterm_bg(முழு hl_id, முழு *fgp, முழு *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ப்ரதிஜ்ஞா(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
    *fgp = sgp->sg_cterm_fg - 1;
    *bgp = sgp->sg_cterm_bg - 1;
}
#endif

/*
 * Translate a group ID to the final group ID (following links).
 */
    முழு
syn_get_final_id(முழு hl_id)
{
    முழு		மொத்தம்;
    hl_group_T	*sgp;

    எனில் (hl_id > highlight_ga.ga_len || hl_id < 1)
	திரும்பு 0;			// Can be called from eval!!

    // Follow links until there is no more.
    // Look out for loops!  Break after 100 links.
    க்கு (மொத்தம் = 100; --மொத்தம் >= 0; )
    {
	முழு tmp = hl_id;

	sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
	எனில் (sgp->sg_link == 0 || sgp->sg_link > highlight_ga.ga_len)
	    நிறுத்து;

	// This is to handle highlight groups that are overridden but are in the
	// middle of a link chain.
	hl_id = syn_override(hl_id);
	எனில் (tmp != hl_id)
	    தொடர்;
	hl_id = sgp->sg_link;
    }
    திரும்பு syn_override(hl_id);
}

#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Call this function just after the GUI has started.
 * Also called when 'termguicolors' was set, gui.in_use will be FALSE then.
 * It finds the font and color handles for the highlighting groups.
 */
    வெறுமை
highlight_gui_started(வெறுமை)
{
    முழு	    idx;

    // First get the colors from the "Normal" and "Menu" group, if set
    எனில் (USE_24BIT)
	set_normal_colors();

    க்கு (idx = 0; idx < highlight_ga.ga_len; ++idx)
	gui_do_one_color(idx, FALSE, FALSE);

    highlight_changed();
}

    நிலையான வெறுமை
gui_do_one_color(
    முழு		idx,
    முழு		do_menu UNUSED,	   // TRUE: might set the menu font
    முழு		do_tooltip UNUSED) // TRUE: might set the tooltip font
{
    முழு		didit = FALSE;

# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    எனில் (gui.in_use)
#  endif
	எனில் (HL_TABLE()[idx].sg_font_name != வெற்று)
	{
	    hl_do_font(idx, HL_TABLE()[idx].sg_font_name, FALSE, do_menu,
							    do_tooltip, TRUE);
	    didit = TRUE;
	}
# endif
    எனில் (HL_TABLE()[idx].sg_gui_fg_name != வெற்று)
    {
	HL_TABLE()[idx].sg_gui_fg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_fg_name);
	didit = TRUE;
    }
    எனில் (HL_TABLE()[idx].sg_gui_bg_name != வெற்று)
    {
	HL_TABLE()[idx].sg_gui_bg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_bg_name);
	didit = TRUE;
    }
    எனில் (HL_TABLE()[idx].sg_gui_sp_name != வெற்று)
    {
	HL_TABLE()[idx].sg_gui_sp =
			    color_name2handle(HL_TABLE()[idx].sg_gui_sp_name);
	didit = TRUE;
    }
    எனில் (didit)	// need to get a new attr number
	set_hl_attr(idx);
}
#endif

#எனில் defined(USER_HIGHLIGHT) && defined(FEAT_STL_OPT)
/*
 * Apply difference between User[1-9] and HLF_S to HLF_SNC, HLF_ST or HLF_STNC.
 */
    நிலையான வெறுமை
combine_stl_hlt(
	முழு id,
	முழு id_S,
	முழு id_alt,
	முழு hlcnt,
	முழு i,
	முழு hlf,
	முழு *table)
{
    hl_group_T *hlt = HL_TABLE();

    எனில் (id_alt == 0)
    {
	CLEAR_POINTER(&hlt[hlcnt + i]);
	hlt[hlcnt + i].sg_term = highlight_attr[hlf];
	hlt[hlcnt + i].sg_cterm = highlight_attr[hlf];
# எனில் defined(FEAT_GUI) || defined(FEAT_EVAL)
	hlt[hlcnt + i].sg_gui = highlight_attr[hlf];
# endif
    }
    இல்லையெனில்
	mch_memmove(&hlt[hlcnt + i],
		    &hlt[id_alt - 1],
		    அளவு(hl_group_T));
    hlt[hlcnt + i].sg_link = 0;

    hlt[hlcnt + i].sg_term ^=
	hlt[id - 1].sg_term ^ hlt[id_S - 1].sg_term;
    எனில் (hlt[id - 1].sg_start != hlt[id_S - 1].sg_start)
	hlt[hlcnt + i].sg_start = hlt[id - 1].sg_start;
    எனில் (hlt[id - 1].sg_stop != hlt[id_S - 1].sg_stop)
	hlt[hlcnt + i].sg_stop = hlt[id - 1].sg_stop;
    hlt[hlcnt + i].sg_cterm ^=
	hlt[id - 1].sg_cterm ^ hlt[id_S - 1].sg_cterm;
    எனில் (hlt[id - 1].sg_cterm_fg != hlt[id_S - 1].sg_cterm_fg)
	hlt[hlcnt + i].sg_cterm_fg = hlt[id - 1].sg_cterm_fg;
    எனில் (hlt[id - 1].sg_cterm_bg != hlt[id_S - 1].sg_cterm_bg)
	hlt[hlcnt + i].sg_cterm_bg = hlt[id - 1].sg_cterm_bg;
    எனில் (hlt[id - 1].sg_cterm_font != hlt[id_S - 1].sg_cterm_font)
	hlt[hlcnt + i].sg_cterm_font = hlt[id - 1].sg_cterm_font;
# எனில் defined(FEAT_GUI) || defined(FEAT_EVAL)
    hlt[hlcnt + i].sg_gui ^=
	hlt[id - 1].sg_gui ^ hlt[id_S - 1].sg_gui;
# endif
# ifdef FEAT_GUI
    எனில் (hlt[id - 1].sg_gui_fg != hlt[id_S - 1].sg_gui_fg)
	hlt[hlcnt + i].sg_gui_fg = hlt[id - 1].sg_gui_fg;
    எனில் (hlt[id - 1].sg_gui_bg != hlt[id_S - 1].sg_gui_bg)
	hlt[hlcnt + i].sg_gui_bg = hlt[id - 1].sg_gui_bg;
    எனில் (hlt[id - 1].sg_gui_sp != hlt[id_S - 1].sg_gui_sp)
	hlt[hlcnt + i].sg_gui_sp = hlt[id - 1].sg_gui_sp;
    எனில் (hlt[id - 1].sg_font != hlt[id_S - 1].sg_font)
	hlt[hlcnt + i].sg_font = hlt[id - 1].sg_font;
#  ifdef FEAT_XFONTSET
    எனில் (hlt[id - 1].sg_fontset != hlt[id_S - 1].sg_fontset)
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
    முழு
highlight_changed(வெறுமை)
{
    முழு		hlf;
    முழு		i;
    char_u	*p;
    char_u	*default_hl;
    முழு		attr;
    char_u	*end;
    முழு		id;
#ifdef USER_HIGHLIGHT
    char_u      userhl[30];  // use 30 to avoid compiler warning
# ifdef FEAT_STL_OPT
    முழு		id_S = -1;
    முழு		id_SNC = 0;
#  ifdef FEAT_TERMINAL
    முழு		id_ST = 0;
    முழு		id_STNC = 0;
#  endif
    முழு		hlcnt;
# endif
#endif
    win_T	*wp;

    need_highlight_changed = FALSE;

#ifdef FEAT_TERMINAL
    term_update_colors_all();
    term_update_hlfwin_all();
#endif

    // Clear all attributes.
    க்கு (hlf = 0; hlf < (முழு)HLF_மொத்தம்; ++hlf)
    {
	highlight_attr[hlf] = 0;
	highlight_attr_raw[hlf] = 0;
	highlight_ids[hlf] = 0;
    }

    default_hl = get_highlight_default();

    // First set all attributes to their default value.
    // Then use the attributes from the 'highlight' option, unless it already
    // matches the default and would repeat the same work.
    க்கு (i = 0; i < 2; ++i)
    {
	எனில் (i)
	{
	    எனில் (default_hl != வெற்று && p_hl != வெற்று
				      && STRCMP(default_hl, p_hl) == 0)
		தொடர்;
	    p = p_hl;
	}
	இல்லையெனில்
	    p = default_hl;
	எனில் (p == வெற்று)	    // just in case
	    தொடர்;

	போது (*p)
	{
	    க்கு (hlf = 0; hlf < (முழு)HLF_மொத்தம்; ++hlf)
		எனில் (hl_flags[hlf] == *p)
		    நிறுத்து;
	    ++p;
	    எனில் (hlf == (முழு)HLF_மொத்தம் || *p == NUL)
		திரும்பு FAIL;

	    // Allow several hl_flags to be combined, like "bu" for
	    // bold-underlined.
	    attr = 0;
	    id = 0;
	    க்கு ( ; *p && *p != ','; ++p)	    // parse up to comma
	    {
		எனில் (VIM_ISWHITE(*p))		    // ignore white space
		    தொடர்;

		எனில் (attr > HL_ALL)  // Combination with ':' is not allowed.
		    திரும்பு FAIL;

		// Note: Keep this in sync with expand_set_highlight().
		தேர்வு (*p)
		{
		    நிலை 'b':	attr |= HL_BOLD;
				நிறுத்து;
		    நிலை 'i':	attr |= HL_ITALIC;
				நிறுத்து;
		    நிலை '-':
		    நிலை 'n':			    // no highlighting
				நிறுத்து;
		    நிலை 'r':	attr |= HL_INVERSE;
				நிறுத்து;
		    நிலை 's':	attr |= HL_STANDOUT;
				நிறுத்து;
		    நிலை 'u':	attr |= HL_UNDERLINE;
				நிறுத்து;
		    நிலை 'c':	attr |= HL_UNDERCURL;
				நிறுத்து;
		    நிலை '2':	attr |= HL_UNDERDOUBLE;
				நிறுத்து;
		    நிலை 'd':	attr |= HL_UNDERDOTTED;
				நிறுத்து;
		    நிலை '=':	attr |= HL_UNDERDASHED;
				நிறுத்து;
		    நிலை 't':	attr |= HL_STRIKETHROUGH;
				நிறுத்து;
		    நிலை ':':	++p;		    // highlight group name
				எனில் (attr || *p == NUL)	 // no combinations
				    திரும்பு FAIL;
				end = vim_strchr(p, ',');
				எனில் (end == வெற்று)
				    end = p + STRLEN(p);
				id = syn_check_group(p, (முழு)(end - p));
				எனில் (id == 0)
				    திரும்பு FAIL;
				attr = syn_id2attr(id);
				p = end - 1;
#எனில் defined(FEAT_STL_OPT) && defined(USER_HIGHLIGHT)
				எனில் (hlf == (முழு)HLF_SNC)
				    id_SNC = syn_get_final_id(id);
# ifdef FEAT_TERMINAL
				இல்லையெனில் எனில் (hlf == (முழு)HLF_ST)
				    id_ST = syn_get_final_id(id);
				இல்லையெனில் எனில் (hlf == (முழு)HLF_STNC)
				    id_STNC = syn_get_final_id(id);
# endif
				இல்லையெனில் எனில் (hlf == (முழு)HLF_S)
				    id_S = syn_get_final_id(id);
#endif
				நிறுத்து;
		    இயல்பு:	திரும்பு FAIL;
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
	எழுத்து *errmsg = update_winhighlight(wp, wp->w_p_whl);

	எனில் (errmsg != வெற்று)
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
    எனில் (ga_grow(&highlight_ga, 28) == FAIL)
	திரும்பு FAIL;
    hlcnt = highlight_ga.ga_len;
    எனில் (id_S == -1)
    {
	// Make sure id_S is always valid to simplify code below. Use the last
	// entry.
	CLEAR_POINTER(&HL_TABLE()[hlcnt + 27]);
	HL_TABLE()[hlcnt + 18].sg_term = highlight_attr[HLF_S];
	id_S = hlcnt + 19;
    }
# endif
    க்கு (i = 0; i < 9; i++)
    {
	sprintf((எழுத்து *)userhl, "User%d", i + 1);
	id = syn_name2id(userhl);
	எனில் (id == 0)
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
	இல்லையெனில்
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

    திரும்பு OK;
}

நிலையான வெறுமை highlight_list(வெறுமை);
நிலையான வெறுமை highlight_list_two(முழு cnt, முழு attr);

// context for :highlight <group> <arg> expansion
நிலையான முழு expand_hi_synid = 0;	    // ID for highlight group being completed
நிலையான முழு expand_hi_equal_col = 0; // column where the '=' is
நிலையான முழு expand_hi_include_orig = 0;	    // whether to fill the existing current value or not
நிலையான char_u *expand_hi_curvalue = வெற்று;   // the existing current value
#எனில் defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
நிலையான dict_iterator_T expand_colornames_iter;	// iterator for looping through v:colornames
#endif

/*
 * Handle command line completion for :highlight command.
 */
    வெறுமை
set_context_in_highlight_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;
    முழு		expand_group = TRUE;

    // Default: expand group names
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 0;
    include_link = 2;
    include_default = 1;

    எனில் (*arg == NUL)
	திரும்பு;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    எனில் (*p == NUL)
	திரும்பு;

    // past "default" or group name
    include_default = 0;
    எனில் (STRNCMP("default", arg, p - arg) == 0)
    {
	arg = skipwhite(p);
	xp->xp_pattern = arg;
	p = skiptowhite(arg);
    }
    எனில் (*p == NUL)
	திரும்பு;

    // past group name
    include_link = 0;
    எனில் (arg[1] == 'i' && arg[0] == 'N')
    {
	highlight_list();
	expand_group = FALSE;
    }
    எனில் (STRNCMP("link", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	எனில் (*p != NUL)		// past first group name
	{
	    xp->xp_pattern = skipwhite(p);
	    p = skiptowhite(xp->xp_pattern);
	    include_none = 1;
	}
	expand_group = FALSE;
    }
    இல்லையெனில் எனில் (STRNCMP("clear", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	expand_group = FALSE;
    }
    எனில் (*p != NUL)			// past group name(s)
    {
	எனில் (expand_group)
	{
	    // expansion will be done in expand_highlight_group()
	    xp->xp_context = EXPAND_HIGHLIGHT_GROUP;

	    expand_hi_synid = syn_namen2id(arg, (முழு)(p - arg));

	    போது (*p != NUL)
	    {
		arg = skipwhite(p);
		p = skiptowhite(arg);
	    }

	    p = vim_strchr(arg, '=');
	    எனில் (p == வெற்று)
	    {
		// Didn't find a key=<value> pattern
		xp->xp_pattern = arg;
		expand_hi_equal_col = -1;
		expand_hi_include_orig = FALSE;
	    }
	    இல்லையெனில்
	    {
		// Found key=<value> pattern, record the exact location
		expand_hi_equal_col = (முழு)(p - xp->xp_வரி);

		// Only include the original value if the pattern is empty
		எனில் (*(p + 1) == NUL)
		    expand_hi_include_orig = TRUE;
		இல்லையெனில்
		    expand_hi_include_orig = FALSE;

		// Account for comma-separated values
		எனில் (STRNCMP(arg, "term=", 5) == 0 ||
			STRNCMP(arg, "cterm=", 6) == 0 ||
			STRNCMP(arg, "gui=", 4) == 0)
		{
		    char_u *comma = vim_strrchr(p + 1, ',');
		    எனில் (comma != வெற்று)
			p = comma;
		}
		xp->xp_pattern = p + 1;
	    }
	}
	இல்லையெனில்
	{
	    xp->xp_context = EXPAND_NOTHING;
	}
    }
}

/*
 * List highlighting matches in a nice way.
 */
    நிலையான வெறுமை
highlight_list(வெறுமை)
{
    முழு		i;

    க்கு (i = 10; --i >= 0; )
	highlight_list_two(i, HL_ATTR(HLF_D));
    க்கு (i = 40; --i >= 0; )
	highlight_list_two(99, 0);
}

    நிலையான வெறுமை
highlight_list_two(முழு cnt, முழு attr)
{
    செய்தி_puts_attr(&("N \bI \b!  \b"[cnt / 11]), attr);
    செய்தி_clr_eos();
    out_flush();
    ui_delay(cnt == 99 ? 40L : (long)cnt * 50L, FALSE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_highlight_name(expand_T *xp UNUSED, முழு idx)
{
    திரும்பு get_highlight_name_ext(xp, idx, TRUE);
}

/*
 * Obtain a highlight group name.
 * When "skip_cleared" is TRUE don't return a cleared entry.
 */
    char_u *
get_highlight_name_ext(expand_T *xp UNUSED, முழு idx, முழு skip_cleared)
{
    எனில் (idx < 0)
	திரும்பு வெற்று;

    // Items are never removed from the table, skip the ones that were
    // cleared.
    எனில் (skip_cleared && idx < highlight_ga.ga_len && HL_TABLE()[idx].sg_cleared)
	திரும்பு (char_u *)"";

    எனில் (idx == highlight_ga.ga_len && include_none != 0)
	திரும்பு (char_u *)"NONE";
    எனில் (idx == highlight_ga.ga_len + include_none && include_default != 0)
	திரும்பு (char_u *)"default";
    எனில் (idx == highlight_ga.ga_len + include_none + include_default
							 && include_link != 0)
	திரும்பு (char_u *)"link";
    எனில் (idx == highlight_ga.ga_len + include_none + include_default + 1
							 && include_link != 0)
	திரும்பு (char_u *)"clear";
    எனில் (idx >= highlight_ga.ga_len)
	திரும்பு வெற்று;
    திரும்பு HL_TABLE()[idx].sg_name;
}

    நிலையான char_u *
get_highlight_attr_name(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx == 0)
    {
	// Fill with current value first
	எனில் (expand_hi_curvalue != வெற்று)
	    திரும்பு expand_hi_curvalue;
	இல்லையெனில்
	    திரும்பு (char_u*)"";
    }
    எனில் (idx < (முழு)ARRAY_LENGTH(highlight_index_tab) + 1)
    {
	char_u *மதிப்பு = highlight_index_tab[idx-1]->மதிப்பு.string;
	எனில் (expand_hi_curvalue != வெற்று && STRCMP(expand_hi_curvalue, மதிப்பு) == 0)
	{
	    // Already returned the current value above, just skip.
	    திரும்பு (char_u*)"";
	}
	திரும்பு மதிப்பு;
    }
    திரும்பு வெற்று;
}

    நிலையான char_u *
get_highlight_cterm_color(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx == 0)
    {
	// Fill with current value first
	எனில் (expand_hi_curvalue != வெற்று)
	    திரும்பு expand_hi_curvalue;
	இல்லையெனில்
	    திரும்பு (char_u*)"";
    }
    // See highlight_set_cterm_color()
    இல்லையெனில் எனில் (idx == 1)
	திரும்பு (char_u*)"fg";
    இல்லையெனில் எனில் (idx == 2)
	திரும்பு (char_u*)"bg";
    எனில் (idx < (முழு)ARRAY_LENGTH(color_name_tab) + 3)
    {
	char_u *மதிப்பு = color_name_tab[idx-3].மதிப்பு.string;
	திரும்பு மதிப்பு;
    }
    திரும்பு வெற்று;
}

#எனில் defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
    நிலையான char_u *
get_highlight_gui_color(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx == 0)
    {
	// Fill with current value first
	எனில் (expand_hi_curvalue != வெற்று)
	    திரும்பு expand_hi_curvalue;
	இல்லையெனில்
	    திரும்பு (char_u*)"";
    }
    // See color_name2handle()
    இல்லையெனில் எனில் (idx == 1)
	திரும்பு (char_u*)"fg";
    இல்லையெனில் எனில் (idx == 2)
	திரும்பு (char_u*)"bg";
    இல்லையெனில் எனில் (idx == 3)
	திரும்பு (char_u*)"NONE";

    // Complete from v:colornames. Don't do platform specific names for now.
    typval_T *tv_result;
    char_u *colorname = dict_iterate_next(&expand_colornames_iter, &tv_result);
    எனில் (colorname != வெற்று)
    {
	// :hi command doesn't allow space, so don't suggest any malformed items
	எனில் (vim_strchr(colorname, ' ') != வெற்று)
	    திரும்பு (char_u*)"";

	எனில் (expand_hi_curvalue != வெற்று && STRICMP(expand_hi_curvalue, colorname) == 0)
	{
	    // Already returned the current value above, just skip.
	    திரும்பு (char_u*)"";
	}
    }
    திரும்பு colorname;
}
#endif

    நிலையான char_u *
get_highlight_group_சாவி(expand_T *xp UNUSED, முழு idx)
{
    // Note: Keep this in sync with do_highlight.
    நிலையான எழுத்து *(p_hi_group_சாவி_மதிப்புகள்[]) =
    {
	"term=",
	"start=",
	"stop=",
	"cterm=",
	"ctermfg=",
	"ctermbg=",
	"ctermul=",
	"ctermfont=",
#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

    எனில் (idx < (முழு)ARRAY_LENGTH(p_hi_group_சாவி_மதிப்புகள்))
	திரும்பு (char_u*)p_hi_group_சாவி_மதிப்புகள்[idx];
    திரும்பு வெற்று;
}

/*
 * Command-line expansion for :hi {group-name} <args>...
 */
    முழு
expand_highlight_group(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	முழு	    *இலக்கம்_Matches)
{
    எனில் (expand_hi_equal_col != -1)
    {
	// List the values. First fill in the current value, then if possible colors
	// or attribute names.
	char_u	    *(*expandfunc)(expand_T *, முழு) = வெற்று;
	முழு	    type = 0;
	hl_group_T  *sgp = வெற்று;
	முழு	    iarg = 0;
	char_u	    *sarg = வெற்று;

	முழு	    unsorted_பொருட்கள் = -1; // don't sort by default

	எனில் (expand_hi_synid != 0)
	    sgp = &HL_TABLE()[expand_hi_synid - 1]; // index is ID minus one

	// Note: Keep this in sync with highlight_list_one().
	char_u	    *name_end = xp->xp_வரி + expand_hi_equal_col;
	எனில் (name_end - xp->xp_வரி >= 5
		&& STRNCMP(name_end - 5, " term", 5) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    எனில் (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_term;
	    }
	}
	இல்லையெனில் எனில் (name_end - xp->xp_வரி >= 6
		&& STRNCMP(name_end - 6, " cterm", 6) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    எனில் (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_cterm;
	    }
	}
#எனில் defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	இல்லையெனில் எனில் (name_end - xp->xp_வரி >= 4
		&& STRNCMP(name_end - 4, " gui", 4) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    எனில் (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_gui;
	    }
	}
#endif
	இல்லையெனில் எனில் (name_end - xp->xp_வரி >= 8
		&& STRNCMP(name_end - 8, " ctermfg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    எனில் (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_fg;
	    }
	}
	இல்லையெனில் எனில் (name_end - xp->xp_வரி >= 8
		&& STRNCMP(name_end - 8, " ctermbg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    எனில் (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_bg;
	    }
	}
	இல்லையெனில் எனில் (name_end - xp->xp_வரி >= 8
		&& STRNCMP(name_end - 8, " ctermul", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    எனில் (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_ul;
	    }
	}
#எனில் defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	இல்லையெனில் எனில் (name_end - xp->xp_வரி >= 6
		&& STRNCMP(name_end - 6, " guifg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    எனில் (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_fg_name;
	    }
	}
	இல்லையெனில் எனில் (name_end - xp->xp_வரி >= 6
		&& STRNCMP(name_end - 6, " guibg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    எனில் (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_bg_name;
	    }
	}
	இல்லையெனில் எனில் (name_end - xp->xp_வரி >= 6
		&& STRNCMP(name_end - 6, " guisp", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    எனில் (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_sp_name;
	    }
	}
#endif

#எனில் defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	எனில் (expandfunc == get_highlight_gui_color)
	{
	    // Top 4 items are special, after that sort all the color names
	    unsorted_பொருட்கள் = 4;

	    dict_T *colornames_table = get_vim_var_dict(VV_COLORNAMES);
	    typval_T colornames_val;
	    colornames_val.v_type = VAR_DICT;
	    colornames_val.vval.v_dict = colornames_table;
	    dict_iterate_start(&colornames_val, &expand_colornames_iter);
	}
#endif

	char_u	    buf[MAX_ATTR_LEN];

	expand_hi_curvalue = வெற்று;
	எனில் (expand_hi_include_orig)
	{
	    எனில் (((type == LIST_ATTR || type == LIST_INT) && iarg != 0) ||
		(type == LIST_STRING && sarg != வெற்று))
	    {
		// Retrieve the current value to go first in completion
		expand_hi_curvalue = highlight_arg_to_string(
			type, iarg, sarg, buf);
	    }
	}

	எனில் (expandfunc != வெற்று)
	{
	    திரும்பு ExpandGenericExt(
		    pat,
		    xp,
		    rmp,
		    matches,
		    இலக்கம்_Matches,
		    expandfunc,
		    FALSE,
		    unsorted_பொருட்கள்);
	}

	திரும்பு FAIL;
    }

    // List all the key names
    திரும்பு ExpandGenericExt(
	    pat,
	    xp,
	    rmp,
	    matches,
	    இலக்கம்_Matches,
	    get_highlight_group_சாவி,
	    FALSE,
	    -1);
}

#எனில் defined(FEAT_GUI)
/*
 * Free all the highlight group fonts.
 * Used when quitting for systems which need it.
 */
    வெறுமை
free_highlight_fonts(வெறுமை)
{
    முழு	    idx;

    க்கு (idx = 0; idx < highlight_ga.ga_len; ++idx)
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

#எனில் defined(FEAT_EVAL)
/*
 * Convert each of the highlight attribute bits (bold, standout, underline,
 * etc.) set in 'hlattr' into a separate boolean item in a Dictionary with
 * the attribute name as the key.
 */
    நிலையான dict_T *
highlight_get_attr_dict(முழு hlattr)
{
    dict_T	*dict;
    முழு		i;

    dict = dict_alloc();
    எனில் (dict == வெற்று)
	திரும்பு வெற்று;

    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(highlight_index_tab); ++i)
    {
	எனில் (hlattr & highlight_index_tab[i]->சாவி)
	{
	    dict_add_bool(dict, (எழுத்து *)highlight_index_tab[i]->மதிப்பு.string,
		    VVAL_TRUE);
	    // don't want "inverse"/"reverse"
	    hlattr &= ~highlight_index_tab[i]->சாவி;
	}
    }

    திரும்பு dict;
}

/*
 * Return the attributes of the highlight group at index 'hl_idx' as a
 * Dictionary. If 'resolve_link' is TRUE, then resolves the highlight group
 * links recursively.
 */
    நிலையான dict_T *
highlight_get_info(முழு hl_idx, முழு resolve_link)
{
    dict_T	*dict;
    hl_group_T	*sgp;
    dict_T	*attr_dict = வெற்று;
    முழு		hlgid;

    dict = dict_alloc();
    எனில் (dict == வெற்று)
	திரும்பு dict;

    sgp = &HL_TABLE()[hl_idx];
    // highlight group id is 1-based
    hlgid = hl_idx + 1;

    எனில் (dict_add_string(dict, "name", sgp->sg_name) == FAIL)
	செல் error;
    எனில் (dict_add_எண்(dict, "id", hlgid) == FAIL)
	செல் error;

    எனில் (resolve_link)
    {
	// resolve the highlight group link recursively
	போது (sgp->sg_link)
	{
	    hlgid = sgp->sg_link;
	    sgp = &HL_TABLE()[sgp->sg_link - 1];
	}
    }

    எனில் (sgp->sg_term != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_term);
	எனில் (attr_dict != வெற்று)
	{
	    எனில் (dict_add_dict(dict, "term", attr_dict) == FAIL)
		செல் error;
	    attr_dict = வெற்று;
	}
    }
    எனில் (sgp->sg_start != வெற்று)
	எனில் (dict_add_string(dict, "start", sgp->sg_start) == FAIL)
	    செல் error;
    எனில் (sgp->sg_stop != வெற்று)
	எனில் (dict_add_string(dict, "stop", sgp->sg_stop) == FAIL)
	    செல் error;
    எனில் (sgp->sg_cterm != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_cterm);
	எனில் (attr_dict != வெற்று)
	{
	    எனில் (dict_add_dict(dict, "cterm", attr_dict) == FAIL)
		செல் error;
	    attr_dict = வெற்று;
	}
    }
    எனில் (sgp->sg_cterm_fg != 0)
	எனில் (dict_add_string(dict, "ctermfg",
		    highlight_color(hlgid, (char_u *)"fg", 'c')) == FAIL)
	    செல் error;
    எனில் (sgp->sg_cterm_bg != 0)
	எனில் (dict_add_string(dict, "ctermbg",
			highlight_color(hlgid, (char_u *)"bg", 'c')) == FAIL)
	    செல் error;
    எனில் (sgp->sg_cterm_ul != 0)
	எனில் (dict_add_string(dict, "ctermul",
			highlight_color(hlgid, (char_u *)"ul", 'c')) == FAIL)
	    செல் error;
    எனில் (sgp->sg_cterm_font != 0)
	எனில் (dict_add_string(dict, "ctermfont",
			highlight_color(hlgid, (char_u *)"font", 'c')) == FAIL)
	    செல் error;
    எனில் (sgp->sg_gui != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_gui);
	எனில் (attr_dict != வெற்று)
	{
	    எனில் (dict_add_dict(dict, "gui", attr_dict) == FAIL)
		செல் error;
	    attr_dict = வெற்று;
	}
    }
    எனில் (sgp->sg_gui_fg_name != வெற்று)
	எனில் (dict_add_string(dict, "guifg",
			highlight_color(hlgid, (char_u *)"fg", 'g')) == FAIL)
	    செல் error;
    எனில் (sgp->sg_gui_bg_name != வெற்று)
	எனில் (dict_add_string(dict, "guibg",
			highlight_color(hlgid, (char_u *)"bg", 'g')) == FAIL)
	    செல் error;
    எனில் (sgp->sg_gui_sp_name != வெற்று)
	எனில் (dict_add_string(dict, "guisp",
			highlight_color(hlgid, (char_u *)"sp", 'g')) == FAIL)
	    செல் error;
# ifdef FEAT_GUI
    எனில் (sgp->sg_font_name != வெற்று)
	எனில் (dict_add_string(dict, "font", sgp->sg_font_name) == FAIL)
	    செல் error;
# endif
    எனில் (sgp->sg_link)
    {
	char_u	*link;

	link = HL_TABLE()[sgp->sg_link - 1].sg_name;
	எனில் (link != வெற்று && dict_add_string(dict, "linksto", link) == FAIL)
	    செல் error;

	எனில் (sgp->sg_deflink)
	    dict_add_bool(dict, "default", VVAL_TRUE);
    }
    எனில் (dict_len(dict) == 2)
	// If only 'name' is present, then the highlight group is cleared.
	dict_add_bool(dict, "cleared", VVAL_TRUE);

    திரும்பு dict;

error:
    dict_unref(attr_dict);
    dict_unref(dict);
    திரும்பு வெற்று;
}

/*
 * "hlget([name])" function
 * Return the attributes of a specific highlight group (if specified) or all
 * the highlight groups.
 */
    வெறுமை
f_hlget(typval_T *argvars, typval_T *rettv)
{
    list_T	*list;
    dict_T	*dict;
    முழு		i;
    char_u	*hlarg = வெற்று;
    முழு		resolve_link = FALSE;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (check_for_opt_string_arg(argvars, 0) == FAIL
	    || (argvars[0].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
	// highlight group name supplied
	hlarg = tv_get_string_chk(&argvars[0]);
	எனில் (hlarg == வெற்று)
	    திரும்பு;

	எனில் (argvars[1].v_type != VAR_UNKNOWN)
	{
	    முழு error = FALSE;

	    resolve_link = tv_get_bool_chk(&argvars[1], &error);
	    எனில் (error)
		திரும்பு;
	}
    }

    list = rettv->vval.v_list;
    க்கு (i = 0; i < highlight_ga.ga_len && !got_int; ++i)
    {
	எனில் (hlarg == வெற்று || STRICMP(hlarg, HL_TABLE()[i].sg_name) == 0)
	{
	    dict = highlight_get_info(i, resolve_link);
	    எனில் (dict != வெற்று && list_append_dict(list, dict) == FAIL)
		dict_unref(dict);
	}
    }
}

/*
 * Returns the string value at 'dict[key]'. Returns NULL, if 'key' is not in
 * 'dict' or the value is not a string type. If the value is not a string type
 * or is NULL, then 'error' is set to TRUE.
 */
    நிலையான char_u *
hldict_get_string(dict_T *dict, char_u *சாவி, முழு *error)
{
    dictitem_T	*di;

    *error = FALSE;
    di = dict_find(dict, சாவி, -1);
    எனில் (di == வெற்று)
	திரும்பு வெற்று;

    எனில் (di->di_tv.v_type != VAR_STRING || di->di_tv.vval.v_string == வெற்று)
    {
	emsg(_(e_string_required));
	*error = TRUE;
	திரும்பு வெற்று;
    }

    திரும்பு di->di_tv.vval.v_string;
}

/*
 * Convert the highlight attribute Dictionary at 'dict[key]' into a string
 * value in 'attr_str' of length 'len'. Returns FALSE if 'dict[key]' is not a
 * Dictionary or is NULL.
 */
    நிலையான முழு
hldict_attr_to_str(
	dict_T	*dict,
	char_u	*சாவி,
	char_u	*attr_str,
	size_t	len)
{
    dictitem_T	*di;
    dict_T	*attrdict;
    முழு		i;
    char_u	*p;

    attr_str[0] = NUL;
    di = dict_find(dict, சாவி, -1);
    எனில் (di == வெற்று)
	திரும்பு TRUE;

    எனில் (di->di_tv.v_type != VAR_DICT || di->di_tv.vval.v_dict == வெற்று)
    {
	emsg(_(e_dictionary_required));
	திரும்பு FALSE;
    }

    attrdict = di->di_tv.vval.v_dict;

    // If the attribute dict is empty, then return NONE to clear the attributes
    எனில் (dict_len(attrdict) == 0)
    {
	vim_strcat(attr_str, (char_u *)"NONE", len);
	திரும்பு TRUE;
    }

    p = attr_str;
    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(highlight_tab); ++i)
    {
	எனில் (dict_get_bool(attrdict, (எழுத்து *)highlight_tab[i].மதிப்பு.string,
		    VVAL_FALSE) == VVAL_TRUE)
	{
	    எனில் (p != attr_str && (size_t)(p - attr_str + 2) < len)
		STRCPY(p, (char_u *)",");
	    எனில் (p - attr_str + highlight_tab[i].மதிப்பு.length + 1 < len)
	    {
		STRCPY(p, highlight_tab[i].மதிப்பு.string);
		p += highlight_tab[i].மதிப்பு.length;
	    }
	}
    }

    திரும்பு TRUE;
}

// Temporary buffer used to store the command string produced by hlset().
// IObuff cannot be used for this as the error messages produced by hlset()
// internally use IObuff.
# define	HLSETBUFSZ  512
நிலையான char_u hlsetBuf[HLSETBUFSZ + 1];

/*
 * Add the highlight attribute "attr" of length "attrlen" and "value" at
 * "dptr", which points into "hlsetBuf".
 * Returns the updated pointer.
 */
    நிலையான char_u *
add_attr_and_மதிப்பு(char_u *dptr, char_u *attr, முழு attrlen, char_u *மதிப்பு)
{
    size_t	vallen;

    // Do nothing if the value is not specified or is empty
    எனில் (மதிப்பு == வெற்று || *மதிப்பு == NUL)
	திரும்பு dptr;

    vallen = STRLEN(மதிப்பு);
    // When the value contains a space and the attribute has an "=" (i.e. it
    // is a key=value pair), surround the value with single quotes so that
    // do_highlight() can parse it correctly.
    எனில் (vim_strchr(மதிப்பு, ' ') != வெற்று && vim_strchr(attr, '=') != வெற்று)
    {
	எனில் (dptr + attrlen + vallen + 3 < hlsetBuf + HLSETBUFSZ)
	{
	    STRCPY(dptr, attr);
	    dptr += attrlen;
	    *dptr++ = '\'';
	    STRCPY(dptr, மதிப்பு);
	    dptr += vallen;
	    *dptr++ = '\'';
	    *dptr = NUL;
	}
    }
    இல்லையெனில் எனில் (dptr + attrlen + vallen + 1 < hlsetBuf + HLSETBUFSZ)
    {
	STRCPY(dptr, attr);
	dptr += attrlen;
	STRCPY(dptr, மதிப்பு);
	dptr += vallen;
    }

    திரும்பு dptr;
}

/*
 * Add or update a highlight group using 'dict' items. Returns TRUE if
 * successfully updated the highlight group.
 */
    நிலையான முழு
hlg_add_or_update(dict_T *dict)
{
    char_u	*name;
    முழு		error;
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
    முழு		forceit = FALSE;
    முழு		dodefault = FALSE;
    முழு		done = FALSE;
    char_u	*p;

    name = hldict_get_string(dict, (char_u *)"name", &error);
    எனில் (name == வெற்று || *name == NUL || error)
	திரும்பு FALSE;

    எனில் (dict_get_bool(dict, "force", VVAL_FALSE) == VVAL_TRUE)
	forceit = TRUE;

    எனில் (dict_get_bool(dict, "default", VVAL_FALSE) == VVAL_TRUE)
	dodefault = TRUE;

    எனில் (dict_has_சாவி(dict, "cleared"))
    {
	varnumber_T	cleared;

	// clear a highlight group
	cleared = dict_get_bool(dict, "cleared", FALSE);
	எனில் (cleared == TRUE)
	{
	    vim_snprintf((எழுத்து *)hlsetBuf, HLSETBUFSZ, "clear %s", name);
	    do_highlight(hlsetBuf, forceit, FALSE);
	    done = TRUE;
	}
    }

    எனில் (dict_has_சாவி(dict, "linksto"))
    {
	char_u	*linksto;

	// link highlight groups
	linksto = hldict_get_string(dict, (char_u *)"linksto", &error);
	எனில் (linksto == வெற்று || *linksto == NUL || error)
	    திரும்பு FALSE;

	vim_snprintf((எழுத்து *)hlsetBuf, HLSETBUFSZ, "%slink %s %s",
				dodefault ? "default " : "", name, linksto);
	do_highlight(hlsetBuf, forceit, FALSE);

	done = TRUE;
    }

    // If 'cleared' or 'linksto' are specified, then don't process the other
    // attributes.
    எனில் (done)
	திரும்பு TRUE;

    start = hldict_get_string(dict, (char_u *)"start", &error);
    எனில் (error)
	திரும்பு FALSE;

    stop = hldict_get_string(dict, (char_u *)"stop", &error);
    எனில் (error)
	திரும்பு FALSE;

    எனில் (!hldict_attr_to_str(dict, (char_u *)"term", term_attr,
							    அளவு(term_attr)))
	திரும்பு FALSE;

    எனில் (!hldict_attr_to_str(dict, (char_u *)"cterm", cterm_attr,
							   அளவு(cterm_attr)))
	திரும்பு FALSE;

    ctermfg = hldict_get_string(dict, (char_u *)"ctermfg", &error);
    எனில் (error)
	திரும்பு FALSE;

    ctermbg = hldict_get_string(dict, (char_u *)"ctermbg", &error);
    எனில் (error)
	திரும்பு FALSE;

    ctermul = hldict_get_string(dict, (char_u *)"ctermul", &error);
    எனில் (error)
	திரும்பு FALSE;

    ctermfont = hldict_get_string(dict, (char_u *)"ctermfont", &error);
    எனில் (error)
	திரும்பு FALSE;

    எனில் (!hldict_attr_to_str(dict, (char_u *)"gui", gui_attr, அளவு(gui_attr)))
	திரும்பு FALSE;

    guifg = hldict_get_string(dict, (char_u *)"guifg", &error);
    எனில் (error)
	திரும்பு FALSE;

    guibg = hldict_get_string(dict, (char_u *)"guibg", &error);
    எனில் (error)
	திரும்பு FALSE;

    guisp = hldict_get_string(dict, (char_u *)"guisp", &error);
    எனில் (error)
	திரும்பு FALSE;

# ifdef FEAT_GUI
    font = hldict_get_string(dict, (char_u *)"font", &error);
    எனில் (error)
	திரும்பு FALSE;
# endif

    // If none of the attributes are specified, then do nothing.
    எனில் (term_attr[0] == NUL && start == வெற்று && stop == வெற்று
	    && cterm_attr[0] == NUL && ctermfg == வெற்று && ctermbg == வெற்று
	    && ctermul == வெற்று && ctermfont == வெற்று && gui_attr[0] == NUL
# ifdef FEAT_GUI
	    && font == வெற்று
# endif
	    && guifg == வெற்று && guibg == வெற்று && guisp == வெற்று
	    )
	திரும்பு TRUE;

    hlsetBuf[0] = NUL;
    p = hlsetBuf;
    எனில் (dodefault)
	p = add_attr_and_மதிப்பு(p, (char_u *)"default", 7, (char_u *)" ");
    p = add_attr_and_மதிப்பு(p, (char_u *)"", 0, name);
    p = add_attr_and_மதிப்பு(p, (char_u *)" term=", 6, term_attr);
    p = add_attr_and_மதிப்பு(p, (char_u *)" start=", 7, start);
    p = add_attr_and_மதிப்பு(p, (char_u *)" stop=", 6, stop);
    p = add_attr_and_மதிப்பு(p, (char_u *)" cterm=", 7, cterm_attr);
    p = add_attr_and_மதிப்பு(p, (char_u *)" ctermfg=", 9, ctermfg);
    p = add_attr_and_மதிப்பு(p, (char_u *)" ctermbg=", 9, ctermbg);
    p = add_attr_and_மதிப்பு(p, (char_u *)" ctermul=", 9, ctermul);
    p = add_attr_and_மதிப்பு(p, (char_u *)" ctermfont=", 11, ctermfont);
    p = add_attr_and_மதிப்பு(p, (char_u *)" gui=", 5, gui_attr);
# ifdef FEAT_GUI
    p = add_attr_and_மதிப்பு(p, (char_u *)" font=", 6, font);
# endif
    p = add_attr_and_மதிப்பு(p, (char_u *)" guifg=", 7, guifg);
    p = add_attr_and_மதிப்பு(p, (char_u *)" guibg=", 7, guibg);
    (வெறுமை)add_attr_and_மதிப்பு(p, (char_u *)" guisp=", 7, guisp);

    do_highlight(hlsetBuf, forceit, FALSE);

    திரும்பு TRUE;
}

/*
 * "hlset([{highlight_attr}])" function
 * Add or modify highlight groups
 */
    வெறுமை
f_hlset(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    dict_T	*dict;

    rettv->vval.v_எண் = -1;

    எனில் (check_for_list_arg(argvars, 0) == FAIL)
	திரும்பு;

    FOR_ALL_LIST_பொருட்கள்(argvars->vval.v_list, li)
    {
	எனில் (li->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_dictionary_required));
	    திரும்பு;
	}

	dict = li->li_tv.vval.v_dict;
	எனில் (!hlg_add_or_update(dict))
	    திரும்பு;
    }

    rettv->vval.v_எண் = 0;
}
#endif

/*
 * If "old" is in the override stack, then update it to "hl_new". Does not free
 * "old".
 */
    வெறுமை
update_highlight_overrides(hl_override_T *old, hl_override_T *hl_new, முழு newlen)
{
    எனில் (old == வெற்று)
	திரும்பு;

    க்கு (hl_overrides_T *set = overrides; set != வெற்று; set = set->next)
    {
	எனில் (set->arr == old)
	{
	    set->arr = hl_new;
	    set->len = newlen;
	}
    }
}

/*
 * If "arr" is in the highlight overrides list, then mark it as invalid.
 */
    வெறுமை
remove_highlight_overrides(hl_override_T *arr)
{
    எனில் (arr == வெற்று || overrides == வெற்று)
	திரும்பு;

    க்கு (hl_overrides_T *set = overrides; set != வெற்று; set = set->next)
    {
	எனில் (set->arr == arr)
	{
	    set->arr = வெற்று;
	    நிறுத்து;
	}
    }
}

/*
 * Update highlight_attr[] array. If "update_ids" is true, then update
 * highlight_ids[] array instead.
 */
    நிலையான வெறுமை
set_highlight_attr(hl_override_T *arr, முழு len, bool update_ids)
{
    க்கு (முழு i = 0; i < len; i++)
    {
	hl_override_T	*override = arr + i;
	முழு		hlf = -1, attr;

	எனில் (override->from <= 0)
	    // Directly map to highlight_attr
	    hlf = -override->from;
	இல்லையெனில்
	{
	    க்கு (முழு k = 0; k < HLF_மொத்தம்; k++)
		எனில் (override->from == highlight_ids[k])
		{
		    hlf = k;
		    நிறுத்து;
		}

	    எனில் (hlf == -1)
		தொடர்;
	}

	எனில் (update_ids)
	{
	    எனில் (override->to <= 0)
	    {
		எனில் (hlf == -override->to)
		    highlight_ids[hlf] = -1;
		இல்லையெனில்
		    highlight_ids[hlf] = highlight_ids[-override->to];
	    }
	    இல்லையெனில்
		highlight_ids[hlf] = override->to;
	}
	இல்லையெனில்
	{
	    எனில் (override->to <= 0)
		// Directly use highlight_attr[]
		attr = highlight_attr[-override->to];
	    இல்லையெனில்
		attr = syn_id2attr(override->to);

	    highlight_attr[hlf] = attr;
	}
    }
}

/*
 * Set the current highlight override to "arr". Returns true if successful
 */
    bool
தள்ளு_highlight_overrides(hl_override_T *arr, முழு len)
{
    // Don't want to do anything if "arr" is already the current override. If
    // "arr" is NULL (but overrides->arr is not), then still push an override,
    // but "->arr" will just be NULL so any previous overrides are cleared.
    எனில் (overrides != வெற்று && overrides->arr == arr)
	திரும்பு false;

    hl_overrides_T *set;

    set = ALLOC_ONE(hl_overrides_T);
    எனில் (set == வெற்று)
	திரும்பு false;

    set->arr = arr;
    set->len = len;

    // Push to front of list
    set->next = overrides;
    overrides = set;

    // Save current highlight_attr[], so it can be restored later after this
    // override is popped.
    memcpy(set->attr, highlight_attr, அளவு(highlight_attr));

    // Reset highlight_attr[] to default values, as we don't want changes from
    // other overrides.
    memcpy(highlight_attr, highlight_attr_raw, அளவு(highlight_attr));

    // Update highlight_attr[] array
    எனில் (arr != வெற்று)
	set_highlight_attr(arr, len, false);

    திரும்பு true;
}

/*
 * Pop the current highlight override (if any) from the stack. This should
 * always be paired with a *successful* push.
 */
    வெறுமை
எடு_highlight_overrides(வெறுமை)
{
    hl_overrides_T *set = overrides;

    எனில் (overrides == வெற்று)
	திரும்பு;

    overrides = set->next;

    // Set highlight_attr[] to state before override was pushed.
    memcpy(highlight_attr, set->attr, அளவு(highlight_attr));
    vim_free(set);
}

/*
 * Parse the 'winhighlight' option and return array. Returns NULL on failure or
 * if empty option. If failure, then errmsg is set.
 */
    நிலையான hl_override_T *
parse_winhighlight(char_u *opt, முழு *len, எழுத்து **errmsg)
{
    char_u	    *p = opt;
    hl_override_T   *arr;
    முழு		    i = 0;
    முழு		    இலக்கம் = 1;
    முழு		    n_colons = 0;

    எனில் (*p == NUL)
	திரும்பு வெற்று;

    // Get number of overrides first so we can allocate array
    போது ((p = vim_strchr(p, ',')) != வெற்று)
    {
	p++;
	இலக்கம்++;
    }
    p = opt;
    // Check if number of ':' matches number of ','
    போது ((p = vim_strchr(p, ':')) != வெற்று)
    {
	p++;
	n_colons++;
    }

    எனில் (இலக்கம் != n_colons)
    {
	*errmsg = e_invalid_argument;
	திரும்பு வெற்று;
    }

    arr = ALLOC_MULT(hl_override_T, இலக்கம்);
    எனில் (arr == வெற்று)
    {
	*errmsg = e_out_of_memory;
	திரும்பு வெற்று;
    }

    p = opt;

    போது (true)
    {
	hl_override_T *override = arr + i++;
	char_u	*fromname = p, *toname;
	char_u	*tmp;
	char_u	**names[2] = {&fromname, &toname};
	முழு	fromlen, tolen;
	முழு	*lens[2] = {&fromlen, &tolen};
	முழு	fromid, toid;
	முழு	*ids[2] = {&fromid, &toid};

	p = vim_strchr(p, ':');

	எனில் (p == வெற்று)
	    செல் fail;

	fromlen = p - fromname; // Get hl for "from"
	எனில் (fromlen == 0)
	    செல் fail;
	p++; // Skip colon ':'
	எனில் (*p == NUL)
	    செல் fail;

	toname = p;
	tmp = vim_strchr(p, ',');

	// Get hl for "to", must check for no trailing comma in case last
	// element.
	எனில் (tmp == வெற்று)
	    tolen = (முழு)STRLEN(p);
	இல்லையெனில்
	{
	    tolen = tmp - toname;
	    p = ++tmp;
	}
	எனில் (tolen == 0)
	    செல் fail;

	க்கு (முழு k = 0; k < 2; k++)
	{
	    char_u  *name = *names[k];
	    முழு	    nlen = *lens[k];

	    எனில் (*name == '!')
	    {
		// If starts with "!", then it maps directly to a 'highlight'
		// occasion.
		முழு hlf;

		எனில் (nlen != 2)
		    செல் fail;

		க்கு (hlf = 0; hlf < (முழு)HLF_மொத்தம்; ++hlf)
		    எனில் (hl_flags[hlf] == name[1])
			நிறுத்து;

		எனில் (hlf >= HLF_மொத்தம்)
		    செல் fail;

		*ids[k] = -hlf;
	    }
	    இல்லையெனில்
	    {
		// Otherwise get the highlight group id.
		எனில் (syn_check_group(name, nlen) == 0)
		    செல் fail;

		*ids[k] = syn_namen2id(name, nlen);
		எனில் (*ids[k] == 0)
		    செல் fail;

		// Always map "Normal" group to HLF_WIN
		எனில் (ids[k] == &fromid &&
			STRCMP(HL_TABLE()[*ids[k] - 1].sg_name_u, "NORMAL") == 0)
		    *ids[k] = -HLF_WIN;
	    }
	}

	// If fromid == toid, leave it be, this is so that we know HLF_WIN has
	// been set (even to itself e.g. "Normal"), and therefore know to use
	// its attr instead.
	override->from = fromid;
	override->to = toid;

	எனில் (tmp == வெற்று)
	    நிறுத்து;
    }

    *len = இலக்கம்;
    திரும்பு arr;
fail:
    vim_free(arr);
    *errmsg = e_invalid_argument;
    திரும்பு வெற்று;
}

/*
 * Update w_hl for "wp" using given option value. Returns error
 * message on failure, otherwise NULL.
 */
    எழுத்து *
update_winhighlight(win_T *wp, char_u *opt)
{
    எழுத்து	    *err = வெற்று;
    hl_override_T   *arr;
    முழு		    இலக்கம் = 1;

    arr = parse_winhighlight(opt, &இலக்கம், &err);

    எனில் (arr == வெற்று && err != வெற்று)
	திரும்பு err;

    எனில் (arr == வெற்று && wp->w_hl == வெற்று)
	திரும்பு வெற்று;

    update_highlight_overrides(wp->w_hl, arr, இலக்கம்);

    vim_free(wp->w_hl);
    wp->w_hl = arr;
    wp->w_hl_len = இலக்கம்;

    wp->w_hlfwin_id = hlf_get_id(wp, HLF_WIN);

#ifdef FEAT_TERMINAL
    term_update_hlfwin(wp);
#endif

#ifdef FEAT_TERMINAL
    // May be NULL (such as in after_copy_winopt())
    எனில் (wp->w_buffer != வெற்று)
    {
	// Make sure terminal highlighting is updated
	bool ret = தள்ளு_highlight_overrides(wp->w_hl, wp->w_hl_len);

	எனில் (wp->w_buffer->b_term != வெற்று)
	    term_init_default_colors(wp->w_buffer->b_term);
	எனில் (ret)
	    எடு_highlight_overrides();
    }
#endif

    திரும்பு வெற்று;
}

/*
 * Get id that the hlf attr is using in window "wp", otherwise zero. Note that
 * -1 may be returned if the hlf was set to itself.
 */
    முழு
hlf_get_id(win_T *wp, முழு hlf)
{
    முழு id;
    நிலையான முழு prev[HLF_மொத்தம்];

    எனில் (wp->w_hl == வெற்று)
	திரும்பு highlight_ids[hlf];

    memcpy(prev, highlight_ids, அளவு(highlight_ids));
    set_highlight_attr(wp->w_hl, wp->w_hl_len, true);
    id = highlight_ids[hlf];
    memcpy(highlight_ids, prev, அளவு(highlight_ids));

    திரும்பு id;
}

/*
 * Update 'winhighlight' using a 'wincolor' style option value. Returns error
 * message on failure otherwise NULL. Does not update the actual 'wincolor'
 * value, but does update the 'winhighlight' value.
 */
    எழுத்து *
update_wincolor(win_T *wp, char_u *opt)
{
    char_u  *str = *opt == NUL ? "" : alloc(அளவு("!(:") + STRLEN(opt));
    எழுத்து    *errmsg;

    எனில் (str == வெற்று)
	திரும்பு e_out_of_memory;

    எனில் (*opt != NUL)
	sprintf((எழுத்து *)str, "!(:%s", opt);

    errmsg = update_winhighlight(wp, str);

    எனில் (errmsg == வெற்று)
	set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		str, OPT_FREE|OPT_LOCAL, 0);

    எனில் (*opt != NUL)
	விடுவி(str);
    திரும்பு errmsg;
}
