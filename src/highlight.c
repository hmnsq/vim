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

#ଅନ୍ତର୍ଭାଵ "vim.h"

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
ସ୍ଥିର keyvalue_T highlight_tab[] = {
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
ସ୍ଥିର keyvalue_T *highlight_index_tab[] = {
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

ଗଣନା {
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
ସ୍ଥିର keyvalue_T color_name_tab[] = {
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    char_u	*sg_name;	// highlight group name
    char_u	*sg_name_u;	// uppercase of sg_name
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sg_cleared;	// "hi clear" was used
// for normal terminals
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sg_term;	// "term=" highlighting attributes
    char_u	*sg_start;	// terminal string for start highl
    char_u	*sg_stop;	// terminal string for stop highl
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sg_term_attr;	// Screen attr for term mode
// for color terminals
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sg_cterm;	// "cterm=" highlighting attr
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sg_cterm_bold;	// bold attr was set for light color
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sg_cterm_fg;	// terminal fg color number + 1
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sg_cterm_bg;	// terminal bg color number + 1
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sg_cterm_ul;	// terminal ul color number + 1
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sg_cterm_attr;	// Screen attr for color term mode
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sg_cterm_font;	// terminal alternative font (0 for normal)
// for when using the GUI
#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sg_gui_attr;    // Screen attr for GUI mode
#endif
#ଯଦି defined(FEAT_GUI) || defined(FEAT_EVAL)
// Store the sp color name for the GUI or synIDattr()
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sg_gui;		// "gui=" highlighting attributes
    char_u	*sg_gui_fg_name;// GUI foreground color name
    char_u	*sg_gui_bg_name;// GUI background color name
    char_u	*sg_gui_sp_name;// GUI special color name
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sg_link;	// link to this highlight group ID
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sg_deflink;	// default link; restored in highlight_clear()
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sg_set;		// combination of SG_* flags
#ifdef FEAT_EVAL
    sctx_T	sg_deflink_sctx;  // script where the default link was set
    sctx_T	sg_script_ctx;	// script in which the group was last set
#endif
} hl_group_T;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା hl_overrides_S hl_overrides_T;
ସଂରଚନା hl_overrides_S
{
    hl_override_T   *arr;   // May be NULL if "arr" was freed or no highlight
			    // overrides (all values set to default)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    len;
    hl_overrides_T  *next;  // Used to handle recursive calls

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr[HLF_ଗଣନା];    // highlight_attr[] before being updated.
};

ସ୍ଥିର hl_overrides_T *overrides = ଖାଲି;

// Synced with highlight_attr, each is the highlight group id for each attr. If
// an element is 0, then there is no attr for it. If an element is -1, then it
// means it was set to itself.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା highlight_ids[HLF_ଗଣନା];

// Same as highlight_attr[] but is not modified by highlight group overrides
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା highlight_attr_raw[HLF_ଗଣନା];

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା hl_flags[HLF_ଗଣନା] = HL_FLAGS;

// highlight groups for 'highlight' option
ସ୍ଥିର garray_T highlight_ga;
#define HL_TABLE()	((hl_group_T *)((highlight_ga.ga_data)))

// Wrapper struct for hashtable entries: stores the highlight group ID alongside
// the uppercase name used as a hash key.  Uses the same offsetof pattern as
// signgroup_T / HI2SG.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hn_id;		// highlight group ID (1-based)
    char_u	hn_ଚାବି[1];	// uppercase name (stretchable array)
} hlname_T;

#define HLNAME_ଚାବି_OFF	offsetof(hlname_T, hn_ଚାବି)
#define HI2HLNAME(hi)	((hlname_T *)((hi)->hi_ଚାବି - HLNAME_ଚାବି_OFF))

// hashtable for quick highlight group name lookup
ସ୍ଥିର hashtab_T highlight_ht;
ସ୍ଥିର bool	 highlight_ht_inited = false;

/*
 * An attribute number is the index in attr_table plus ATTR_OFF.
 */
#define ATTR_OFF (HL_ALL + 1)

ସ୍ଥିର ଶୂନ୍ୟ syn_unadd_group(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ set_hl_attr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ସ୍ଥିର ଶୂନ୍ୟ highlight_list_one(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା highlight_list_arg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, ପୂର୍ଣ୍ଣସଂଖ୍ୟା didh, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା iarg, char_u *sarg, ଅକ୍ଷର *name);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syn_add_group(char_u *name);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syn_name2id_len(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା hl_has_settings(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_link);
ସ୍ଥିର ଶୂନ୍ୟ highlight_clear(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);

#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
ସ୍ଥିର ଶୂନ୍ୟ gui_do_one_color(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_tooltip);
#endif
#ifdef FEAT_GUI
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  set_group_colors(char_u *name, guicolor_T *fgp, guicolor_T *bgp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_norm, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_tooltip);
ସ୍ଥିର ଶୂନ୍ୟ hl_do_font(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_normal, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_tooltip, ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_font);
#endif

/*
 * The default highlight groups.  These are compiled-in for fast startup and
 * they still work when the runtime files can't be found.
 * When making changes here, also change runtime/colors/default.vim!
 * The #ifdefs are needed to reduce the amount of static data.  Helps to make
 * the 16 bit DOS (museum) version compile.
 */
#ଯଦି defined(FEAT_GUI) || defined(FEAT_EVAL)
# define CENT(a, b) b
#ଅନ୍ୟଥା
# define CENT(a, b) a
#endif
ସ୍ଥିର ଅକ୍ଷର *(highlight_init_both[]) = {
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
    ଖାଲି
};

// Default colors only used with a light background.
ସ୍ଥିର ଅକ୍ଷର *(highlight_init_light[]) = {
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
    ଖାଲି
};

// Default colors only used with a dark background.
ସ୍ଥିର ଅକ୍ଷର *(highlight_init_dark[]) = {
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
    ଖାଲି
};

#ଯଦି defined(FEAT_SYN_HL)
/*
 * Returns the number of highlight groups.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
highlight_ଅଙ୍କ_groups(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ highlight_ga.ga_len;
}

/*
 * Returns the name of a highlight group.
 */
    char_u *
highlight_group_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    ଫେରନ୍ତୁ HL_TABLE()[id].sg_name;
}

/*
 * Returns the ID of the link to a highlight group.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
highlight_link_id(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    ଫେରନ୍ତୁ HL_TABLE()[id].sg_link;
}
#endif

ସ୍ଥିର ଶୂନ୍ୟ resolve_fallback_fg_to_rgb(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ resolve_fallback_bg_to_rgb(ଶୂନ୍ୟ);

    ଶୂନ୍ୟ
init_highlight(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		both,	    // include groups where 'bg' doesn't matter
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		reset)	    // clear group first
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ଅକ୍ଷର	**pp;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	had_both = FALSE;
#ifdef FEAT_EVAL
    char_u	*p;

    // Try finding the color scheme file.  Used when a color file was loaded
    // and 'background' or 't_Co' is changed.
    p = get_var_ମୂଲ୍ୟ((char_u *)"g:colors_name");
    ଯଦି (p != ଖାଲି)
    {
	// The value of g:colors_name could be freed when sourcing the script,
	// making "p" invalid, so copy it.
	char_u *copy_p = vim_strsave(p);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା    r;

	ଯଦି (copy_p != ଖାଲି)
	{
	    r = load_colors(copy_p);
	    vim_free(copy_p);
	    ଯଦି (r == OK)
		ଫେରନ୍ତୁ;
	}
    }

#endif

    // Didn't use a color file, use the compiled-in colors.
    ଯଦି (both)
    {
	had_both = TRUE;
	pp = highlight_init_both;
	ପାଇଁ (i = 0; pp[i] != ଖାଲି; ++i)
	    do_highlight((char_u *)pp[i], reset, TRUE);
    }
    ଅନ୍ୟଥା ଯଦି (!had_both)
	// Don't do anything before the call with both == TRUE from main().
	// Not everything has been setup then, and that call will overrule
	// everything anyway.
	ଫେରନ୍ତୁ;

    ଯଦି (*p_bg == 'l')
	pp = highlight_init_light;
    ଅନ୍ୟଥା
	pp = highlight_init_dark;
    ପାଇଁ (i = 0; pp[i] != ଖାଲି; ++i)
	do_highlight((char_u *)pp[i], reset, TRUE);

    // Reverse looks ugly, but grey may not work for less than 8 colors.  Thus
    // let it depend on the number of colors available.
    ଯଦି (t_colors < 8)
	do_highlight((char_u *)"Visual term=reverse cterm=reverse ctermbg=NONE ctermfg=NONE",
		FALSE, TRUE);
    // With 8 colors brown is equal to yellow, need to use black for Search fg
    // to avoid Statement highlighted text disappears.
    // Clear the attributes, needed when changing the t_Co value.
    ଯଦି (t_colors <= 8)
    {
	ଯଦି (*p_bg == 'l')
	    do_highlight((char_u *)"Search ctermfg=black", FALSE, TRUE);
    }

#ifdef FEAT_SYN_HL
    // If syntax highlighting is enabled load the highlighting for it.
    ଯଦି (get_var_ମୂଲ୍ୟ((char_u *)"g:syntax_on") != ଖାଲି)
    {
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recursive = 0;

	ଯଦି (recursive >= 5)
	    emsg(_(e_recursive_loop_loading_syncolor_vim));
	ଅନ୍ୟଥା
	{
	    ++recursive;
	    (ଶୂନ୍ୟ)source_runtime((char_u *)"syntax/syncolor.vim", DIP_ALL);
	    --recursive;
	}
    }
#endif
    resolve_fallback_fg_to_rgb();
    resolve_fallback_bg_to_rgb();
}

#ଯଦି defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
/*
 * Load a default color list. Intended to support legacy color names but allows
 * the user to override the color values. Only loaded once.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
load_default_colors_lists(ଶୂନ୍ୟ)
{
    // Lacking a default color list isn't the end of the world but it is likely
    // an inconvenience so users should know when it is missing.
    ଯଦି (source_runtime((char_u *)"colors/lists/default.vim", DIP_ALL) != OK)
	ସନ୍ଦେଶ("failed to load colors/lists/default.vim");
}
#endif

/*
 * Load color file "name".
 * Return OK for success, FAIL for failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
load_colors(char_u *name)
{
    char_u	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FAIL;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recursive = FALSE;

    // When being called recursively, this is probably because setting
    // 'background' caused the highlighting to be reloaded.  This means it is
    // working, thus we should return OK.
    ଯଦି (recursive)
	ଫେରନ୍ତୁ OK;

    recursive = TRUE;
    buf = alloc(STRLEN(name) + 12);
    ଯଦି (buf != ଖାଲି)
    {
#ଯଦି defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	load_default_colors_lists();
#endif
	apply_autocmds(EVENT_COLORSCHEMEPRE, name,
					       curbuf->b_fname, FALSE, curbuf);
	sprintf((ଅକ୍ଷର *)buf, "colors/%s.vim", name);
	retval = source_runtime(buf, DIP_START + DIP_OPT);
	vim_free(buf);
	ଯଦି (retval == OK)
	    apply_autocmds(EVENT_COLORSCHEME, name, curbuf->b_fname,
								FALSE, curbuf);
    }
    recursive = FALSE;

    ଫେରନ୍ତୁ retval;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା color_numbers_16[28] = {0, 1, 2, 3,
				 4, 5, 6, 6,
				 7, 7, 7, 7,
				 8, 8,
				 9, 9, 10, 10,
				 11, 11, 12, 12, 13,
				 13, 14, 14, 15, -1};
// for xterm with 88 colors...
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା color_numbers_88[28] = {0, 4, 2, 6,
				 1, 5, 32, 72,
				 84, 84, 7, 7,
				 82, 82,
				 12, 43, 10, 61,
				 14, 63, 9, 74, 13,
				 75, 11, 78, 15, -1};
// for xterm with 256 colors...
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା color_numbers_256[28] = {0, 4, 2, 6,
				 1, 5, 130, 3,
				 248, 248, 7, 7,
				 242, 242,
				 12, 81, 10, 121,
				 14, 159, 9, 224, 13,
				 225, 11, 229, 15, -1};
// for terminals with less than 16 colors...
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା color_numbers_8[28] = {0, 4, 2, 6,
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lookup_color(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା foreground, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *boldp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		color = color_numbers_16[idx];
    char_u	*p;

    // Use the _16 table to check if it's a valid color name.
    ଯଦି (color < 0)
	ଫେରନ୍ତୁ -1;

    ଯଦି (t_colors == 8)
    {
	// t_Co is 8: use the 8 colors table
#ଯଦି defined(__QNXNTO__)
	// On qnx, the 8 & 16 color arrays are the same
	ଯଦି (STRNCMP(T_NAME, "qansi", 5) == 0)
	    color = color_numbers_16[idx];
	ଅନ୍ୟଥା
#endif
	    color = color_numbers_8[idx];
	ଯଦି (foreground)
	{
	    // set/reset bold attribute to get light foreground
	    // colors (on some terminals, e.g. "linux")
	    ଯଦି (color & 8)
		*boldp = TRUE;
	    ଅନ୍ୟଥା
		*boldp = FALSE;
	}
	color &= 7;	// truncate to 8 colors
    }
    ଅନ୍ୟଥା ଯଦି (t_colors == 16 || t_colors == 88
					   || t_colors >= 256)
    {
	// Guess: if the termcap entry ends in 'm', it is
	// probably an xterm-like terminal.  Use the changed
	// order for colors.
	ଯଦି (*T_CAF != NUL)
	    p = T_CAF;
	ଅନ୍ୟଥା
	    p = T_CSF;
	ଯଦି (*p != NUL && (t_colors > 256
			      || *(p + STRLEN(p) - 1) == 'm'))
	{
	    ଯଦି (t_colors == 88)
		color = color_numbers_88[idx];
	    ଅନ୍ୟଥା ଯଦି (t_colors >= 256)
		color = color_numbers_256[idx];
	    ଅନ୍ୟଥା
		color = color_numbers_8[idx];
	}
#ifdef FEAT_TERMRESPONSE
	ଯଦି (t_colors >= 256 && color == 15 && is_mac_terminal)
	    // Terminal.app has a bug: 15 is light grey. Use white
	    // from the color cube instead.
	    color = 231;
#endif
    }
    ଫେରନ୍ତୁ color;
}

/*
 * Link highlight group 'from_hg' to 'to_hg'.
 * 'dodefault' is set to TRUE for ":highlight default link".
 * 'forceit' is set to TRUE for ":highlight! link"
 * 'init' is set to TRUE when initializing all the highlight groups.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
highlight_group_link(
	char_u	*from_hg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	from_len,
	char_u	*to_hg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	to_len,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	dodefault,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	forceit,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	init)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		from_id;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		to_id;
    hl_group_T	*hlgroup = ଖାଲି;

    from_id = syn_check_group(from_hg, from_len);
    ଯଦି (STRNCMP(to_hg, "NONE", 4) == 0)
	to_id = 0;
    ଅନ୍ୟଥା
	to_id = syn_check_group(to_hg, to_len);

    ଯଦି (from_id > 0)
    {
	hlgroup = &HL_TABLE()[from_id - 1];
	ଯଦି (dodefault && (forceit || hlgroup->sg_deflink == 0))
	{
	    hlgroup->sg_deflink = to_id;
#ifdef FEAT_EVAL
	    hlgroup->sg_deflink_sctx = current_sctx;
	    hlgroup->sg_deflink_sctx.sc_lnum += SOURCING_LNUM;
#endif
	}
    }

    ଯଦି (from_id > 0 && (!init || hlgroup->sg_set == 0))
    {
	// Don't allow a link when there already is some highlighting
	// for the group, unless '!' is used
	ଯଦି (to_id > 0 && !forceit && !init
		&& hl_has_settings(from_id - 1, dodefault))
	{
	    ଯଦି (SOURCING_NAME == ଖାଲି && !dodefault)
		emsg(_(e_group_has_settings_highlight_link_ignored));
	}
	ଅନ୍ୟଥା ଯଦି (hlgroup->sg_link != to_id
#ifdef FEAT_EVAL
		|| hlgroup->sg_script_ctx.sc_sid != current_sctx.sc_sid
#endif
		|| hlgroup->sg_cleared)
	{
	    ଯଦି (!init)
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
    ସ୍ଥିର ଶୂନ୍ୟ
highlight_reset_all(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

#ifdef FEAT_GUI
    // First, we do not destroy the old values, but allocate the new
    // ones and update the display. THEN we destroy the old values.
    // If we destroy the old values first, then the old values
    // (such as GuiFont's or GuiFontset's) will still be displayed but
    // invalid because they were free'd.
    ଯଦି (gui.in_use)
    {
# ifdef FEAT_BEVAL_TIP
	gui_init_tooltip_font();
# endif
# ଯଦି defined(FEAT_MENU) && defined(FEAT_GUI_MOTIF)
	gui_init_menu_font();
# endif
    }
# ଯଦି defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_X11)
    gui_mch_def_colors();
# endif
# ifdef FEAT_GUI_MSWIN
    ଯଦି (gui.in_use)
	gui_mch_set_titlebar_colors();
# endif
# ifdef FEAT_GUI_X11
#  ifdef FEAT_MENU

    // This only needs to be done when there is no Menu highlight
    // group defined by default, which IS currently the case.
    gui_mch_new_menu_colors();
#  endif
    ଯଦି (gui.in_use)
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
    ପାଇଁ (idx = 0; idx < highlight_ga.ga_len; ++idx)
	highlight_clear(idx);
    init_highlight(TRUE, TRUE);
#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ଯଦି (USE_24BIT)
	highlight_gui_started();
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
highlight_set_termgui_attr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, char_u *ଚାବି, char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା init)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr;
    size_t	off;
    keyvalue_T	target;
    keyvalue_T	*entry;

    attr = 0;
    off = 0;
    target.ଚାବି = 0;
    target.ମୂଲ୍ୟ.length = 0;	// not used, see cmp_keyvalue_value_ni()
    ଯେତେବେଳେ (arg[off] != NUL)
    {
	target.ମୂଲ୍ୟ.string = arg + off;
	entry = (keyvalue_T *)bsearch(&target, &highlight_tab,
		ARRAY_LENGTH(highlight_tab), ଆକାର(highlight_tab[0]),
		cmp_keyvalue_ମୂଲ୍ୟ_ni);
	ଯଦି (entry == ଖାଲି)
	{
	    semsg(_(e_illegal_ମୂଲ୍ୟ_str), arg);
	    ଫେରନ୍ତୁ FALSE;
	}

	attr |= entry->ଚାବି;
	off += entry->ମୂଲ୍ୟ.length;
	ଯଦି (arg[off] == ',')		// another one follows
	    ++off;
    }
    ଯଦି (*ଚାବି == 'T')
    {
	ଯଦି (!init || !(HL_TABLE()[idx].sg_set & SG_TERM))
	{
	    ଯଦି (!init)
		HL_TABLE()[idx].sg_set |= SG_TERM;
	    HL_TABLE()[idx].sg_term = attr;
	}
    }
    ଅନ୍ୟଥା ଯଦି (*ଚାବି == 'C')
    {
	ଯଦି (!init || !(HL_TABLE()[idx].sg_set & SG_CTERM))
	{
	    ଯଦି (!init)
		HL_TABLE()[idx].sg_set |= SG_CTERM;
	    HL_TABLE()[idx].sg_cterm = attr;
	    HL_TABLE()[idx].sg_cterm_bold = FALSE;
	}
    }
#ଯଦି defined(FEAT_GUI) || defined(FEAT_EVAL)
    ଅନ୍ୟଥା
    {
	ଯଦି (!init || !(HL_TABLE()[idx].sg_set & SG_GUI))
	{
	    ଯଦି (!init)
		HL_TABLE()[idx].sg_set |= SG_GUI;
	    HL_TABLE()[idx].sg_gui = attr;
	}
    }
#endif

    ଫେରନ୍ତୁ TRUE;
}

#ifdef FEAT_GUI
/*
 * Set the font for the highlight group at 'idx'.
 * 'arg' is the font name.
 * Returns TRUE if the font is changed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
highlight_set_font(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx,
	char_u	*arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_normal_group,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_menu_group,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_tooltip_group)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_change = FALSE;

    // in non-GUI fonts are simply ignored
    ଯଦି (HL_TABLE()[idx].sg_font_name != ଖାଲି
	    && STRCMP(HL_TABLE()[idx].sg_font_name, arg) == 0)
    {
	// Font name didn't change, ignore.
    }
    ଅନ୍ୟଥା ଯଦି (!gui.shell_created)
    {
	// GUI not started yet, always accept the name.
	vim_free(HL_TABLE()[idx].sg_font_name);
	HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	did_change = TRUE;
    }
    ଅନ୍ୟଥା
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
	ଯଦି (HL_TABLE()[idx].sg_fontset != NOFONTSET)
	{
	    // New fontset was accepted. Free the old one, if there
	    // was one.
	    gui_mch_free_fontset(temp_sg_fontset);
	    did_change = TRUE;
	}
	ଅନ୍ୟଥା
	    HL_TABLE()[idx].sg_fontset = temp_sg_fontset;
# endif
	ଯଦି (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    // New font was accepted. Free the old one, if there was
	    // one.
	    gui_mch_free_font(temp_sg_font);
	    did_change = TRUE;
	}
	ଅନ୍ୟଥା
	    HL_TABLE()[idx].sg_font = temp_sg_font;
	// Update sg_font_name once here, after both font and fontset results
	// are known.
	ଯଦି (did_change)
	{
	    vim_free(HL_TABLE()[idx].sg_font_name);
	    HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	}
    }

    ଫେରନ୍ତୁ did_change;
}
#endif

/*
 * Set the cterm foreground color for the Normal highlight group to "color" and
 * the bold attribute to "bold".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
hl_set_ctermfg_normal_group(ପୂର୍ଣ୍ଣସଂଖ୍ୟା color, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bold)
{
    cterm_normal_fg_color = color + 1;
    cterm_normal_fg_bold = bold;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    ଯଦି (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	ଯଦି (termcap_active && color >= 0)
	    term_fg_color(color);
    }
}

/*
 * Set the cterm foreground color for the highlight group at 'idx' to 'color'.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
highlight_set_ctermfg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା color, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_fg = color + 1;
    ଯଦି (is_normal_group)
	hl_set_ctermfg_normal_group(color,
					(HL_TABLE()[idx].sg_cterm & HL_BOLD));
}

/*
 * Set the cterm background color for the Normal highlight group to "color".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
hl_set_ctermbg_normal_group(ପୂର୍ଣ୍ଣସଂଖ୍ୟା color)
{
    cterm_normal_bg_color = color + 1;
#ifdef FEAT_GUI
    // Don't mess with 'background' if the GUI is used.
    ଯଦି (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	ଯଦି (color >= 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା dark = -1;

	    ଯଦି (termcap_active)
		term_bg_color(color);
	    ଯଦି (t_colors < 16)
		dark = (color == 0 || color == 4);
	    // Limit the heuristic to the standard 16 colors
	    ଅନ୍ୟଥା ଯଦି (color < 16)
		dark = (color < 7 || color == 8);
	    // Set the 'background' option if the value is
	    // wrong.
	    ଯଦି (dark != -1
		    && dark != (*p_bg == 'd')
		    && !option_was_set((char_u *)"bg"))
	    {
		set_option_ମୂଲ୍ୟ_give_err((char_u *)"bg",
			0L, (char_u *)(dark ? "dark" : "light"), 0);
		reset_option_was_set((char_u *)"bg");
	    }
	}
    }
}

/*
 * Set the cterm background color for the highlight group at 'idx' to 'color'.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
highlight_set_ctermbg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା color, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_bg = color + 1;
    ଯଦି (is_normal_group)
	hl_set_ctermbg_normal_group(color);
}

/*
 * Set the cterm underline color for the Normal highlight group to "color".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
hl_set_ctermul_normal_group(ପୂର୍ଣ୍ଣସଂଖ୍ୟା color)
{
    cterm_normal_ul_color = color + 1;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    ଯଦି (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	ଯଦି (termcap_active && color >= 0)
	    term_ul_color(color);
    }
}

/*
 * Set the cterm underline color for the highlight group at 'idx' to 'color'.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
highlight_set_ctermul(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା color, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_ul = color + 1;
    ଯଦି (is_normal_group)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
highlight_set_cterm_font(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx,
	char_u	*arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	init)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		font;

    ଯଦି (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    ଯଦି (VIM_ISDIGIT(*arg))
	font = atoi((ଅକ୍ଷର *)arg);
    ଅନ୍ୟଥା ଯଦି (STRICMP(arg, "NONE") == 0)
	font = -1;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ FALSE;

    HL_TABLE()[idx].sg_cterm_font = font + 1;
    ଫେରନ୍ତୁ TRUE;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
highlight_set_cterm_color(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx,
	char_u	*ଚାବି,
	char_u	*ଚାବି_start,
	char_u	*arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_normal_group,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	init)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		color;

    ଯଦି (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    // When setting the foreground color, and previously the "bold"
    // flag was set for a light color, reset it now
    ଯଦି (ଚାବି[5] == 'F' && HL_TABLE()[idx].sg_cterm_bold)
    {
	HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
	HL_TABLE()[idx].sg_cterm_bold = FALSE;
    }

    ଯଦି (VIM_ISDIGIT(*arg))
	color = atoi((ଅକ୍ଷର *)arg);
    ଅନ୍ୟଥା ଯଦି (STRICMP(arg, "fg") == 0)
    {
	ଯଦି (cterm_normal_fg_color)
	    color = cterm_normal_fg_color - 1;
	ଅନ୍ୟଥା
	{
	    emsg(_(e_fg_color_unknown));
	    ଫେରନ୍ତୁ FALSE;
	}
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(arg, "bg") == 0)
    {
	ଯଦି (cterm_normal_bg_color > 0)
	    color = cterm_normal_bg_color - 1;
	ଅନ୍ୟଥା
	{
	    emsg(_(e_bg_color_unknown));
	    ଫେରନ୍ତୁ FALSE;
	}
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(arg, "ul") == 0)
    {
	ଯଦି (cterm_normal_ul_color > 0)
	    color = cterm_normal_ul_color - 1;
	ଅନ୍ୟଥା
	{
	    emsg(_(e_ul_color_unknown));
	    ଫେରନ୍ତୁ FALSE;
	}
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bold = MAYBE;
	keyvalue_T target;
	keyvalue_T *entry;

	target.ଚାବି = 0;
	target.ମୂଲ୍ୟ.string = arg;
	target.ମୂଲ୍ୟ.length = 0;	// not used, see cmp_keyvalue_value_i()
	entry = (keyvalue_T *)bsearch(&target, &color_name_tab,
		ARRAY_LENGTH(color_name_tab), ଆକାର(color_name_tab[0]),
		cmp_keyvalue_ମୂଲ୍ୟ_i);
	ଯଦି (entry == ଖାଲି)
	{
	    semsg(_(e_color_name_or_ସଂଖ୍ୟା_not_recognized_str), ଚାବି_start);
	    ଫେରନ୍ତୁ FALSE;
	}

	color = lookup_color(entry->ଚାବି, ଚାବି[5] == 'F', &bold);

	// set/reset bold attribute to get light foreground
	// colors (on some terminals, e.g. "linux")
	ଯଦି (bold == TRUE)
	{
	    HL_TABLE()[idx].sg_cterm |= HL_BOLD;
	    HL_TABLE()[idx].sg_cterm_bold = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (bold == FALSE)
	    HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
    }

    // Add one to the argument, to avoid zero.  Zero is used for
    // "NONE", then "color" is -1.
    ଯଦି (ଚାବି[5] == 'F')
	highlight_set_ctermfg(idx, color, is_normal_group);
    ଅନ୍ୟଥା ଯଦି (ଚାବି[5] == 'B')
	highlight_set_ctermbg(idx, color, is_normal_group);
    ଅନ୍ୟଥା // ctermul
	highlight_set_ctermul(idx, color, is_normal_group);

    ଫେରନ୍ତୁ TRUE;
}

#ଯଦି defined(FEAT_GUI) || defined(FEAT_EVAL)
/*
 * Set the GUI foreground color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
highlight_set_guifg(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx,
	char_u	*arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_menu_group UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_scrollbar_group UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_tooltip_group UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_titlebar_group UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_titlebarnc_group UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*do_colors UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	init)
{
# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    long	i;
# endif
    char_u	**namep;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_change = FALSE;

    ଯଦି (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	ଫେରନ୍ତୁ FALSE;

    namep = &HL_TABLE()[idx].sg_gui_fg_name;
    ଯଦି (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guifg colors are only used when recognized
    i = color_name2handle(arg);
    ଯଦି (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_fg = i;
# endif
	ଯଦି (*namep == ଖାଲି || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ଯଦି (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    ଅନ୍ୟଥା
		*namep = ଖାଲି;
	    did_change = TRUE;
	}
# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	ଯଦି (is_titlebar_group && gui.title_fg_pixel != i)
	{
	    gui.title_fg_pixel = i;
	    *do_colors = TRUE;
	}
	ଯଦି (is_titlebarnc_group && gui.titlenc_fg_pixel != i)
	{
	    gui.titlenc_fg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	ଯଦି (is_menu_group && gui.menu_fg_pixel != i)
	{
	    gui.menu_fg_pixel = i;
	    *do_colors = TRUE;
	}
	ଯଦି (is_scrollbar_group && gui.scroll_fg_pixel != i)
	{
	    gui.scroll_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	ଯଦି (is_tooltip_group && gui.tooltip_fg_pixel != i)
	{
	    gui.tooltip_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    ଫେରନ୍ତୁ did_change;
}

/*
 * Set the GUI background color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
highlight_set_guibg(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx,
	char_u	*arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_menu_group UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_scrollbar_group UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_tooltip_group UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_titlebar_group UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_titlebarnc_group UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*do_colors UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	init)
{
# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
# endif
    char_u	**namep;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_change = FALSE;

    ଯଦି (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	ଫେରନ୍ତୁ FALSE;

    namep = &HL_TABLE()[idx].sg_gui_bg_name;
    ଯଦି (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guibg colors are only used when recognized
    i = color_name2handle(arg);
    ଯଦି (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_bg = i;
# endif
	ଯଦି (*namep == ଖାଲି || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ଯଦି (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    ଅନ୍ୟଥା
		*namep = ଖାଲି;
	    did_change = TRUE;
	}
# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	ଯଦି (is_titlebar_group && gui.title_bg_pixel != i)
	{
	    gui.title_bg_pixel = i;
	    *do_colors = TRUE;
	}
	ଯଦି (is_titlebarnc_group && gui.titlenc_bg_pixel != i)
	{
	    gui.titlenc_bg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	ଯଦି (is_menu_group && gui.menu_bg_pixel != i)
	{
	    gui.menu_bg_pixel = i;
	    *do_colors = TRUE;
	}
	ଯଦି (is_scrollbar_group && gui.scroll_bg_pixel != i)
	{
	    gui.scroll_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	ଯଦି (is_tooltip_group && gui.tooltip_bg_pixel != i)
	{
	    gui.tooltip_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    ଫେରନ୍ତୁ did_change;
}

/*
 * Set the GUI undercurl/strikethrough color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
highlight_set_guisp(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା init)
{
# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
# endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_change = FALSE;
    char_u	**namep;

    ଯଦି (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	ଫେରନ୍ତୁ FALSE;

    namep = &HL_TABLE()[idx].sg_gui_sp_name;
    ଯଦି (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guisp colors are only used when recognized
    i = color_name2handle(arg);
    ଯଦି (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_sp = i;
# endif
	ଯଦି (*namep == ଖାଲି || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ଯଦି (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    ଅନ୍ୟଥା
		*namep = ଖାଲି;
	    did_change = TRUE;
	}
# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    }
# endif

    ଫେରନ୍ତୁ did_change;
}
#endif

/*
 * Set the start/stop terminal codes for a highlight group.
 * Returns TRUE if the terminal code is set.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
highlight_set_startstop_termcode(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, char_u *ଚାବି, char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା init)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off;
    char_u	buf[100];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*tname;
    char_u	*p;

    ଯଦି (!init)
	HL_TABLE()[idx].sg_set |= SG_TERM;

    // The "start" and "stop"  arguments can be a literal escape
    // sequence, or a comma separated list of terminal codes.
    ଯଦି (STRNCMP(arg, "t_", 2) == 0)
    {
	off = 0;
	buf[0] = 0;
	ଯେତେବେଳେ (arg[off] != NUL)
	{
	    // Isolate one termcap name
	    ପାଇଁ (len = 0; arg[off + len] &&
		    arg[off + len] != ','; ++len)
		;
	    tname = vim_strnsave(arg + off, len);
	    ଯଦି (tname == ଖାଲି)		// out of memory
		ଫେରନ୍ତୁ FALSE;
	    // lookup the escape sequence for the item
	    p = get_term_code(tname);
	    vim_free(tname);
	    ଯଦି (p == ଖାଲି)	    // ignore non-existing things
		p = (char_u *)"";

	    // Append it to the already found stuff
	    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(STRLEN(buf) + STRLEN(p)) >= 99)
	    {
		semsg(_(e_terminal_code_too_long_str), arg);
		ଫେରନ୍ତୁ FALSE;
	    }
	    STRCAT(buf, p);

	    // Advance to the next item
	    off += len;
	    ଯଦି (arg[off] == ',')	    // another one follows
		++off;
	}
    }
    ଅନ୍ୟଥା
    {
	// Copy characters from arg[] to buf[], translating <> codes.
	ପାଇଁ (p = arg, off = 0; off < 100 - 6 && *p; )
	{
	    len = trans_special(&p, buf + off, FSK_SIMPLIFY, FALSE, ଖାଲି);
	    ଯଦି (len > 0)	    // recognized special char
		off += len;
	    ଅନ୍ୟଥା		    // copy as normal char
		buf[off++] = *p++;
	}
	buf[off] = NUL;
    }

    ଯଦି (STRCMP(buf, "NONE") == 0)	// resetting the value
	p = ଖାଲି;
    ଅନ୍ୟଥା
	p = vim_strsave(buf);
    ଯଦି (ଚାବି[2] == 'A')
    {
	vim_free(HL_TABLE()[idx].sg_start);
	HL_TABLE()[idx].sg_start = p;
    }
    ଅନ୍ୟଥା
    {
	vim_free(HL_TABLE()[idx].sg_stop);
	HL_TABLE()[idx].sg_stop = p;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Handle the ":highlight .." command.
 * When using ":hi clear" this is called recursively for each group with
 * "forceit" and "init" both TRUE.
 */
    ଶୂନ୍ୟ
do_highlight(
    char_u	*ଧାଡ଼ି,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		init)	    // TRUE when called for initializing
{
    char_u	*name_end;
    char_u	*linep;
    char_u	*ଚାବି_start;
    char_u	*arg_start;
    char_u	*ଚାବି = ଖାଲି, *arg = ଖାଲି;
    long	i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    hl_group_T	item_before;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_change = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dodefault = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		doclear = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dolink = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_normal_group = FALSE;	// "Normal" group
#ifdef FEAT_GUI_MSWIN
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_titlebar_group = FALSE;	// "TitleBar" group
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_titlebarnc_group = FALSE;	// "TitleBarNC" group
#ଅନ୍ୟଥା
# define is_titlebar_group 0
# define is_titlebarnc_group 0
#endif
#ifdef FEAT_GUI_X11
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_menu_group = FALSE;		// "Menu" group
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_scrollbar_group = FALSE;	// "Scrollbar" group
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_tooltip_group = FALSE;	// "Tooltip" group
#ଅନ୍ୟଥା
# define is_menu_group 0
# define is_tooltip_group 0
# define is_scrollbar_group 0
#endif
#ଯଦି defined(FEAT_GUI) || defined(FEAT_EVAL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_colors = FALSE;		// need to update colors?
#endif
#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_highlight_changed = FALSE;
#endif

    // If no argument, list current highlighting.
    ଯଦି (!init && ends_excmd2(ଧାଡ଼ି - 1, ଧାଡ଼ି))
    {
	ପାଇଁ (i = 1; i <= highlight_ga.ga_len && !got_int; ++i)
	    // TODO: only call when the group has attributes set
	    highlight_list_one((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)i);
	ଫେରନ୍ତୁ;
    }

    // Isolate the name.
    name_end = skiptowhite(ଧାଡ଼ି);
    linep = skipwhite(name_end);

    // Check for "default" argument.
    ଯଦି (STRNCMP(ଧାଡ଼ି, "default", name_end - ଧାଡ଼ି) == 0)
    {
	dodefault = TRUE;
	ଧାଡ଼ି = linep;
	name_end = skiptowhite(ଧାଡ଼ି);
	linep = skipwhite(name_end);
    }

    // Check for "clear" or "link" argument.
    ଯଦି (STRNCMP(ଧାଡ଼ି, "clear", name_end - ଧାଡ଼ି) == 0)
	doclear = TRUE;
    ଯଦି (STRNCMP(ଧାଡ଼ି, "link", name_end - ଧାଡ଼ି) == 0)
	dolink = TRUE;

    // ":highlight {group-name}": list highlighting for one group.
    ଯଦି (!doclear && !dolink && ends_excmd2(ଧାଡ଼ି, linep))
    {
	id = syn_namen2id(ଧାଡ଼ି, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(name_end - ଧାଡ଼ି));
	ଯଦି (id == 0)
	    semsg(_(e_highlight_group_name_not_found_str), ଧାଡ଼ି);
	ଅନ୍ୟଥା
	    highlight_list_one(id);
	ଫେରନ୍ତୁ;
    }

    // Handle ":highlight link {from} {to}" command.
    ଯଦି (dolink)
    {
	char_u	    *from_start = linep;
	char_u	    *from_end;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    from_len;
	char_u	    *to_start;
	char_u	    *to_end;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    to_len;

	from_end = skiptowhite(from_start);
	to_start = skipwhite(from_end);
	to_end	 = skiptowhite(to_start);

	ଯଦି (ends_excmd2(ଧାଡ଼ି, from_start) || ends_excmd2(ଧାଡ଼ି, to_start))
	{
	    semsg(_(e_not_enough_arguments_highlight_link_str), from_start);
	    ଫେରନ୍ତୁ;
	}

	ଯଦି (!ends_excmd2(ଧାଡ଼ି, skipwhite(to_end)))
	{
	    semsg(_(e_too_many_arguments_highlight_link_str), from_start);
	    ଫେରନ୍ତୁ;
	}

	from_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(from_end - from_start);
	to_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(to_end - to_start);
	highlight_group_link(from_start, from_len, to_start, to_len,
						dodefault, forceit, init);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (doclear)
    {
	// ":highlight clear [group]" command.
	ଯଦି (ends_excmd2(ଧାଡ଼ି, linep))
	{
	    // ":highlight clear" without group name
	    highlight_reset_all();
	    ଫେରନ୍ତୁ;
	}
	ଧାଡ଼ି = linep;
	name_end = skiptowhite(ଧାଡ଼ି);
	linep = skipwhite(name_end);
    }

    // Find the group name in the table.  If it does not exist yet, add it.
    id = syn_check_group(ଧାଡ଼ି, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(name_end - ଧାଡ଼ି));
    ଯଦି (id == 0)	// failed (out of memory)
	ଫେରନ୍ତୁ;
    idx = id - 1;	// index is ID minus one

    // Return if "default" was used and the group already has settings.
    ଯଦି (dodefault && hl_has_settings(idx, TRUE))
	ଫେରନ୍ତୁ;

    // Make a copy so we can check if any attribute actually changed.
    item_before = HL_TABLE()[idx];

    ଯଦି (STRCMP(HL_TABLE()[idx].sg_name_u, "NORMAL") == 0)
	is_normal_group = TRUE;
#ifdef FEAT_GUI_MSWIN
    ଅନ୍ୟଥା ଯଦି (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBAR") == 0)
	is_titlebar_group = TRUE;
    ଅନ୍ୟଥା ଯଦି (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBARNC") == 0)
	is_titlebarnc_group = TRUE;
#endif
#ifdef FEAT_GUI_X11
    ଅନ୍ୟଥା ଯଦି (STRCMP(HL_TABLE()[idx].sg_name_u, "MENU") == 0)
	is_menu_group = TRUE;
    ଅନ୍ୟଥା ଯଦି (STRCMP(HL_TABLE()[idx].sg_name_u, "SCROLLBAR") == 0)
	is_scrollbar_group = TRUE;
    ଅନ୍ୟଥା ଯଦି (STRCMP(HL_TABLE()[idx].sg_name_u, "TOOLTIP") == 0)
	is_tooltip_group = TRUE;
#endif

    // Clear the highlighting for ":hi clear {group}" and ":hi clear".
    ଯଦି (doclear || (forceit && init))
    {
	highlight_clear(idx);
	ଯଦି (!doclear)
	    HL_TABLE()[idx].sg_set = 0;
    }

    ଯଦି (!doclear)
	ଯେତେବେଳେ (!ends_excmd2(ଧାଡ଼ି, linep))
	{
	    ଚାବି_start = linep;
	    ଯଦି (*linep == '=')
	    {
		semsg(_(e_unexpected_equal_sign_str), ଚାବି_start);
		error = TRUE;
		ଭାଙ୍ଗ;
	    }

	    // Note: Keep this in sync with get_highlight_group_key.

	    // Isolate the key ("term", "ctermfg", "ctermbg", "font", "guifg"
	    // or "guibg").
	    ଯେତେବେଳେ (*linep && !VIM_ISWHITE(*linep) && *linep != '=')
		++linep;
	    vim_free(ଚାବି);
	    ଚାବି = vim_strnsave_up(ଚାବି_start, linep - ଚାବି_start);
	    ଯଦି (ଚାବି == ଖାଲି)
	    {
		error = TRUE;
		ଭାଙ୍ଗ;
	    }
	    linep = skipwhite(linep);

	    ଯଦି (STRCMP(ଚାବି, "NONE") == 0)
	    {
		ଯଦି (!init || HL_TABLE()[idx].sg_set == 0)
		{
		    ଯଦି (!init)
			HL_TABLE()[idx].sg_set |= SG_TERM+SG_CTERM+SG_GUI;
		    highlight_clear(idx);
		}
		ଜାରି;
	    }

	    // Check for the equal sign.
	    ଯଦି (*linep != '=')
	    {
		semsg(_(e_missing_equal_sign_str_2), ଚାବି_start);
		error = TRUE;
		ଭାଙ୍ଗ;
	    }
	    ++linep;

	    // Isolate the argument.
	    linep = skipwhite(linep);
	    ଯଦି (*linep == '\'')		// guifg='color name'
	    {
		arg_start = ++linep;
		linep = vim_strchr(linep, '\'');
		ଯଦି (linep == ଖାଲି)
		{
		    semsg(_(e_invalid_argument_str), ଚାବି_start);
		    error = TRUE;
		    ଭାଙ୍ଗ;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		arg_start = linep;
		linep = skiptowhite(linep);
	    }
	    ଯଦି (linep == arg_start)
	    {
		semsg(_(e_missing_argument_str), ଚାବି_start);
		error = TRUE;
		ଭାଙ୍ଗ;
	    }
	    vim_free(arg);
	    arg = vim_strnsave(arg_start, linep - arg_start);
	    ଯଦି (arg == ଖାଲି)
	    {
		error = TRUE;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (*linep == '\'')
		++linep;

	    // Store the argument.
	    ଯଦି (STRCMP(ଚାବି, "TERM") == 0
		    || STRCMP(ଚାବି, "CTERM") == 0
		    || STRCMP(ଚାବି, "GUI") == 0)
	    {
		ଯଦି (!highlight_set_termgui_attr(idx, ଚାବି, arg, init))
		{
		    error = TRUE;
		    ଭାଙ୍ଗ;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(ଚାବି, "FONT") == 0)
	    {
		// in non-GUI fonts are simply ignored
#ifdef FEAT_GUI
		ଯଦି (highlight_set_font(idx, arg, is_normal_group,
					      is_menu_group, is_tooltip_group))
		    did_change = TRUE;
#endif
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(ଚାବି, "CTERMFG") == 0
		    || STRCMP(ଚାବି, "CTERMBG") == 0
		    || STRCMP(ଚାବି, "CTERMUL") == 0)
	    {
		ଯଦି (!highlight_set_cterm_color(idx, ଚାବି, ଚାବି_start, arg,
							is_normal_group, init))
		{
		    error = TRUE;
		    ଭାଙ୍ଗ;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(ଚାବି, "CTERMFONT") == 0)
	    {
		ଯଦି (!highlight_set_cterm_font(idx, arg, init))
		{
		    error = TRUE;
		    ଭାଙ୍ଗ;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(ଚାବି, "GUIFG") == 0)
	    {
#ଯଦି defined(FEAT_GUI) || defined(FEAT_EVAL)
		ଯଦି (highlight_set_guifg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(ଚାବି, "GUIBG") == 0)
	    {
#ଯଦି defined(FEAT_GUI) || defined(FEAT_EVAL)
		ଯଦି (highlight_set_guibg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(ଚାବି, "GUISP") == 0)
	    {
#ଯଦି defined(FEAT_GUI) || defined(FEAT_EVAL)
		ଯଦି (highlight_set_guisp(idx, arg, init))
		    did_change = TRUE;
#endif
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(ଚାବି, "START") == 0 || STRCMP(ଚାବି, "STOP") == 0)
	    {
		ଯଦି (!highlight_set_startstop_termcode(idx, ଚାବି, arg, init))
		{
		    error = TRUE;
		    ଭାଙ୍ଗ;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		semsg(_(e_illegal_argument_str_3), ଚାବି_start);
		error = TRUE;
		ଭାଙ୍ଗ;
	    }
	    HL_TABLE()[idx].sg_cleared = FALSE;

	    // When highlighting has been given for a group, don't link it.
	    ଯଦି (!init || !(HL_TABLE()[idx].sg_set & SG_LINK))
		HL_TABLE()[idx].sg_link = 0;

	    // Continue with next argument.
	    linep = skipwhite(linep);
	}

    // If there is an error, and it's a new entry, remove it from the table.
    ଯଦି (error && idx == highlight_ga.ga_len)
	syn_unadd_group();
    ଅନ୍ୟଥା
    {
	ଯଦି (is_normal_group)
	{
	    HL_TABLE()[idx].sg_term_attr = 0;
	    HL_TABLE()[idx].sg_cterm_attr = 0;
#ifdef FEAT_GUI
	    HL_TABLE()[idx].sg_gui_attr = 0;
	    // Need to update all groups, because they might be using "bg"
	    // and/or "fg", which have been changed now.
#endif
#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    ଯଦି (USE_24BIT)
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
	ଅନ୍ୟଥା ଯଦି (is_titlebar_group)
	{
	    ଯଦି (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
	ଅନ୍ୟଥା ଯଦି (is_titlebarnc_group)
	{
	    ଯଦି (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
#endif
#ifdef FEAT_GUI_X11
# ifdef FEAT_MENU
	ଅନ୍ୟଥା ଯଦି (is_menu_group)
	{
	    ଯଦି (gui.in_use && do_colors)
		gui_mch_new_menu_colors();
	}
# endif
	ଅନ୍ୟଥା ଯଦି (is_scrollbar_group)
	{
	    ଯଦି (gui.in_use && do_colors)
		gui_new_scrollbar_colors();
	    ଅନ୍ୟଥା
		set_hl_attr(idx);
	}
# ifdef FEAT_BEVAL_GUI
	ଅନ୍ୟଥା ଯଦି (is_tooltip_group)
	{
	    ଯଦି (gui.in_use && do_colors)
		gui_mch_new_tooltip_colors();
	}
# endif
#endif
	ଅନ୍ୟଥା
	    set_hl_attr(idx);
#ifdef FEAT_EVAL
	HL_TABLE()[idx].sg_script_ctx = current_sctx;
	HL_TABLE()[idx].sg_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
    }

    vim_free(ଚାବି);
    vim_free(arg);

    // Only call highlight_changed() once, after a sequence of highlight
    // commands, and only if an attribute actually changed.
    ଯଦି ((did_change
	   || memcmp(&HL_TABLE()[idx], &item_before, ଆକାର(item_before)) != 0)
#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    && !did_highlight_changed
#endif
       )
    {
	// Do not trigger a redraw when highlighting is changed while
	// redrawing.  This may happen when evaluating 'statusline' changes the
	// StatusLine group.
	ଯଦି (!updating_screen)
	    redraw_all_later(UPD_NOT_VALID);
	need_highlight_changed = TRUE;
    }
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_highlight(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ପାଇଁ (i = 0; i < highlight_ga.ga_len; ++i)
    {
	highlight_clear(i);
	vim_free(HL_TABLE()[i].sg_name);
	vim_free(HL_TABLE()[i].sg_name_u - HLNAME_ଚାବି_OFF);
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
    ଶୂନ୍ୟ
restore_cterm_colors(ଶୂନ୍ୟ)
{
#ଯଦି defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
    // Since t_me has been set, this probably means that the user
    // wants to use this as default colors.  Need to reset default
    // background/foreground colors.
    mch_set_normal_colors();
#ଅନ୍ୟଥା
# ifdef VIMDLL
    ଯଦି (!gui.in_use)
    {
	mch_set_normal_colors();
	ଫେରନ୍ତୁ;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
hl_has_settings(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_link)
{
    ଫେରନ୍ତୁ HL_TABLE()[idx].sg_cleared == 0
	 && (  HL_TABLE()[idx].sg_term_attr != 0
	    || HL_TABLE()[idx].sg_cterm_attr != 0
	    || HL_TABLE()[idx].sg_cterm_fg != 0
	    || HL_TABLE()[idx].sg_cterm_bg != 0
	    || HL_TABLE()[idx].sg_cterm_font != 0
#ifdef FEAT_GUI
	    || HL_TABLE()[idx].sg_gui_attr != 0
	    || HL_TABLE()[idx].sg_gui_fg_name != ଖାଲି
	    || HL_TABLE()[idx].sg_gui_bg_name != ଖାଲି
	    || HL_TABLE()[idx].sg_gui_sp_name != ଖାଲି
	    || HL_TABLE()[idx].sg_font_name != ଖାଲି
#endif
	    || (check_link && (HL_TABLE()[idx].sg_set & SG_LINK)));
}

/*
 * Clear highlighting for one group.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
highlight_clear(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
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
#ଯଦି defined(FEAT_GUI) || defined(FEAT_EVAL)
    HL_TABLE()[idx].sg_gui = 0;
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_fg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_bg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_sp_name);
#endif
#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Set the normal foreground and background colors according to the "Normal"
 * highlighting group.  For X11 also set "Menu", "Scrollbar" and "Tooltip"
 * colors.  For MS-Windows also set "TitleBar" and "TitleBarNC" colors.
 */
    ଶୂନ୍ୟ
set_normal_colors(ଶୂନ୍ୟ)
{
# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    ଯଦି (gui.in_use)
#  endif
    {
	ଯଦି (set_group_colors((char_u *)"Normal",
				 &gui.norm_pixel, &gui.back_pixel,
				 FALSE, TRUE, FALSE))
	{
	    gui_mch_new_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  ifdef FEAT_GUI_MSWIN
	ଯଦି (set_group_colors((char_u *)"TitleBar",
				 &gui.title_fg_pixel, &gui.title_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
	ଯଦି (set_group_colors((char_u *)"TitleBarNC",
				 &gui.titlenc_fg_pixel, &gui.titlenc_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  endif
#  ifdef FEAT_GUI_X11
	ଯଦି (set_group_colors((char_u *)"Menu",
			     &gui.menu_fg_pixel, &gui.menu_bg_pixel,
			     TRUE, FALSE, FALSE))
	{
#   ifdef FEAT_MENU
	    gui_mch_new_menu_colors();
#   endif
	    set_must_redraw(UPD_CLEAR);
	}
#   ifdef FEAT_BEVAL_GUI
	ଯଦି (set_group_colors((char_u *)"Tooltip",
			     &gui.tooltip_fg_pixel, &gui.tooltip_bg_pixel,
			     FALSE, FALSE, TRUE))
	{
#    ifdef FEAT_TOOLBAR
	    gui_mch_new_tooltip_colors();
#    endif
	    set_must_redraw(UPD_CLEAR);
	}
#   endif
	ଯଦି (set_group_colors((char_u *)"Scrollbar",
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
    ଅନ୍ୟଥା
#  endif
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

	idx = syn_name2id((char_u *)"Normal") - 1;
	ଯଦି (idx >= 0)
	{
	    gui_do_one_color(idx, FALSE, FALSE);

	    // If the normal fg or bg color changed a complete redraw is
	    // required.
	    ଯଦି (cterm_normal_fg_gui_color != HL_TABLE()[idx].sg_gui_fg
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

#ଯଦି defined(FEAT_GUI)
/*
 * Set the colors for "Normal", "Menu", "TitleBar", "TitleBarNC", "Tooltip" or
 * "Scrollbar".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_group_colors(
    char_u	*name,
    guicolor_T	*fgp,
    guicolor_T	*bgp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_menu,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_norm,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_tooltip)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    idx = syn_name2id(name) - 1;
    ଯଦି (idx < 0)
	ଫେରନ୍ତୁ FALSE;

    gui_do_one_color(idx, do_menu, do_tooltip);

    ଯଦି (HL_TABLE()[idx].sg_gui_fg != INVALCOLOR)
	*fgp = HL_TABLE()[idx].sg_gui_fg;
    ଅନ୍ୟଥା ଯଦି (use_norm)
	*fgp = gui.def_norm_pixel;
    ଯଦି (HL_TABLE()[idx].sg_gui_bg != INVALCOLOR)
	*bgp = HL_TABLE()[idx].sg_gui_bg;
    ଅନ୍ୟଥା ଯଦି (use_norm)
	*bgp = gui.def_back_pixel;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Get the font of the "Normal" group.
 * Returns "" when it's not found or not set.
 */
    char_u *
hl_get_font_name(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    char_u	*s;

    id = syn_name2id((char_u *)"Normal");
    ଯଦି (id > 0)
    {
	s = HL_TABLE()[id - 1].sg_font_name;
	ଯଦି (s != ଖାଲି)
	    ଫେରନ୍ତୁ s;
    }
    ଫେରନ୍ତୁ (char_u *)"";
}

/*
 * Set font for "Normal" group.  Called by gui_mch_init_font() when a font has
 * actually chosen to be used.
 */
    ଶୂନ୍ୟ
hl_set_font_name(char_u *font_name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id;

    id = syn_name2id((char_u *)"Normal");
    ଯଦି (id <= 0)
	ଫେରନ୍ତୁ;

    vim_free(HL_TABLE()[id - 1].sg_font_name);
    HL_TABLE()[id - 1].sg_font_name = vim_strsave(font_name);
}

/*
 * Set background color for "Normal" group.  Called by gui_set_bg_color()
 * when the color is known.
 */
    ଶୂନ୍ୟ
hl_set_bg_color_name(
    char_u  *name)	    // must have been allocated
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id;

    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ;

    id = syn_name2id((char_u *)"Normal");
    ଯଦି (id <= 0)
	ଫେରନ୍ତୁ;

    vim_free(HL_TABLE()[id - 1].sg_gui_bg_name);
    HL_TABLE()[id - 1].sg_gui_bg_name = name;
}

/*
 * Set foreground color for "Normal" group.  Called by gui_set_fg_color()
 * when the color is known.
 */
    ଶୂନ୍ୟ
hl_set_fg_color_name(
    char_u  *name)	    // must have been allocated
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id;

    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ;

    id = syn_name2id((char_u *)"Normal");
    ଯଦି (id <= 0)
	ଫେରନ୍ତୁ;

    vim_free(HL_TABLE()[id - 1].sg_gui_fg_name);
    HL_TABLE()[id - 1].sg_gui_fg_name = name;
}

/*
 * Return the handle for a font name.
 * Returns NOFONT when failed.
 */
    ସ୍ଥିର GuiFont
font_name2handle(char_u *name)
{
    ଯଦି (STRCMP(name, "NONE") == 0)
	ଫେରନ୍ତୁ NOFONT;

    ଫେରନ୍ତୁ gui_mch_get_font(name, TRUE);
}

# ifdef FEAT_XFONTSET
/*
 * Return the handle for a fontset name.
 * Returns NOFONTSET when failed.
 */
    ସ୍ଥିର GuiFontset
fontset_name2handle(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fixed_width)
{
    ଯଦି (STRCMP(name, "NONE") == 0)
	ଫେରନ୍ତୁ NOFONTSET;

    ଫେରନ୍ତୁ gui_mch_get_fontset(name, TRUE, fixed_width);
}
# endif

/*
 * Get the font or fontset for one highlight group.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
hl_do_font(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,
    char_u	*arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_normal,		// set normal font
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_menu UNUSED,		// set menu font
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_tooltip UNUSED,	// set tooltip font
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		free_font)		// free current font/fontset
{
# ifdef FEAT_XFONTSET
    // If 'guifontset' is not empty, first try using the name as a
    // fontset.  If that doesn't work, use it as a font name.
    ଯଦି (*p_guifontset != NUL
#  ifdef FONTSET_ALWAYS
	|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
	// In Motif, the Tooltip highlight group is always a fontset
	|| do_tooltip
#  endif
	    )
    {
	ଯଦି (free_font)
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
    ଯଦି (HL_TABLE()[idx].sg_fontset != NOFONTSET)
    {
	// If it worked and it's the Normal group, use it as the normal
	// fontset.  Same for the Menu group.
	ଯଦି (do_normal)
	    gui_init_font(arg, TRUE);
#  ଯଦି defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	ଯଦି (do_menu)
	{
#   ifdef FONTSET_ALWAYS
	    gui.menu_fontset = HL_TABLE()[idx].sg_fontset;
#   ଅନ୍ୟଥା
	    // YIKES!  This is a bug waiting to crash the program
	    gui.menu_font = HL_TABLE()[idx].sg_fontset;
#   endif
	    gui_mch_new_menu_font();
	}
#   ifdef FEAT_BEVAL_GUI
	ଯଦି (do_tooltip)
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
    ଅନ୍ୟଥା
# endif
    {
	ଯଦି (free_font)
	    gui_mch_free_font(HL_TABLE()[idx].sg_font);
	HL_TABLE()[idx].sg_font = font_name2handle(arg);
	// If it worked and it's the Normal group, use it as the
	// normal font.  Same for the Menu group.
	ଯଦି (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    ଯଦି (do_normal)
		gui_init_font(arg, FALSE);
# ifndef FONTSET_ALWAYS
#  ଯଦି defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	    ଯଦି (do_menu)
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

#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return the handle for a color name.
 * Returns INVALCOLOR when failed.
 */
    guicolor_T
color_name2handle(char_u *name)
{
    ଯଦି (STRCMP(name, "NONE") == 0)
	ଫେରନ୍ତୁ INVALCOLOR;

    ଯଦି (STRICMP(name, "fg") == 0 || STRICMP(name, "foreground") == 0)
    {
# ଯଦି defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	ଯଦି (gui.in_use)
# endif
# ifdef FEAT_GUI
	    ଫେରନ୍ତୁ gui.norm_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	ଯଦି (cterm_normal_fg_gui_color != INVALCOLOR)
	    ଫେରନ୍ତୁ cterm_normal_fg_gui_color;
	// Guess that the foreground is black or white.
	ଫେରନ୍ତୁ GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "black" : "white"));
# endif
    }
    ଯଦି (STRICMP(name, "bg") == 0 || STRICMP(name, "background") == 0)
    {
# ଯଦି defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	ଯଦି (gui.in_use)
# endif
# ifdef FEAT_GUI
	    ଫେରନ୍ତୁ gui.back_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	ଯଦି (cterm_normal_bg_gui_color != INVALCOLOR)
	    ଫେରନ୍ତୁ cterm_normal_bg_gui_color;
	// Guess that the background is white or black.
	ଫେରନ୍ତୁ GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "white" : "black"));
# endif
    }

    ଫେରନ୍ତୁ GUI_GET_COLOR(name);
}

// On MS-Windows an RGB macro is available and it produces 0x00bbggrr color
// values as used by the MS-Windows GDI api.  It should be used only for
// MS-Windows GDI builds.
# ଯଦି defined(RGB) && defined(MSWIN) && !defined(FEAT_GUI)
#  undef RGB
# endif
# ifndef RGB
#  define RGB(r, g, b)	(((r)<<16) | ((g)<<8) | (b))
# endif

# ifdef VIMDLL
    ସ୍ଥିର guicolor_T
gui_adjust_rgb(guicolor_T c)
{
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ c;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ ((c & 0xff) << 16) | (c & 0x00ff00) | ((c >> 16) & 0xff);
}
# ଅନ୍ୟଥା
#  define gui_adjust_rgb(c) (c)
# endif

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
hex_digit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (SAFE_isdigit(c))
	ଫେରନ୍ତୁ c - '0';
    c = TOLOWER_ASC(c);
    ଯଦି (c >= 'a' && c <= 'f')
	ଫେରନ୍ତୁ c - 'a' + 10;
    ଫେରନ୍ତୁ 0x1ffffff;
}

    ସ୍ଥିର guicolor_T
decode_hex_color(char_u *hex)
{
    guicolor_T color;

    ଯଦି (hex[0] != '#' || STRLEN(hex) != 7)
	ଫେରନ୍ତୁ INVALCOLOR;

    // Name is in "#rrggbb" format
    color = RGB(((hex_digit(hex[1]) << 4) + hex_digit(hex[2])),
		((hex_digit(hex[3]) << 4) + hex_digit(hex[4])),
		((hex_digit(hex[5]) << 4) + hex_digit(hex[6])));
    ଯଦି (color > 0xffffff)
	ଫେରନ୍ତୁ INVALCOLOR;
    ଫେରନ୍ତୁ gui_adjust_rgb(color);
}

# ifdef FEAT_EVAL
// Returns the color currently mapped to the given name or INVALCOLOR if no
// such name exists in the color table. The convention is to use lowercase for
// all keys in the v:colornames dictionary. The value can be either a string in
// the form #rrggbb or a number, either of which is converted to a guicolor_T.
    ସ୍ଥିର guicolor_T
colorname2rgb(char_u *name)
{
    dict_T      *colornames_table = get_vim_var_dict(VV_COLORNAMES);
    char_u      *lc_name;
    dictitem_T  *colentry;
    char_u      *colstr;
    varnumber_T colnum;

    lc_name = strlow_save(name);
    ଯଦି (lc_name == ଖାଲି)
	ଫେରନ୍ତୁ INVALCOLOR;

    colentry = dict_find(colornames_table, lc_name, -1);
    vim_free(lc_name);
    ଯଦି (colentry == ଖାଲି)
	ଫେରନ୍ତୁ INVALCOLOR;

    ଯଦି (colentry->di_tv.v_type == VAR_STRING)
    {
	colstr = tv_get_string_strict(&colentry->di_tv);
	ଯଦି ((STRLEN(colstr) == 7) && (*colstr == '#'))
	{
	    ଫେରନ୍ତୁ decode_hex_color(colstr);
	}
	ଅନ୍ୟଥା
	{
	    semsg(_(e_bad_color_string_str), colstr);
	    ଫେରନ୍ତୁ INVALCOLOR;
	}
    }

    ଯଦି (colentry->di_tv.v_type == VAR_ସଂଖ୍ୟା)
    {
	colnum = tv_get_ସଂଖ୍ୟା(&colentry->di_tv);
	ଫେରନ୍ତୁ (guicolor_T)colnum;
    }

    ଫେରନ୍ତୁ INVALCOLOR;
}

# endif

    guicolor_T
gui_get_color_cmn(char_u *name)
{
    guicolor_T  color;
    // Only non X11 colors (not present in rgb.txt) and colors in
    // color_name_tab[], useful when $VIMRUNTIME is not found,.
    // must be sorted by the 'value' field because it is used by bsearch()!
    ସ୍ଥିର keyvalue_T rgb_tab[] = {
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
    ଯଦି (color != INVALCOLOR)
	ଫେରନ୍ତୁ color;

    target.ଚାବି = 0;
    target.ମୂଲ୍ୟ.string = name;
    target.ମୂଲ୍ୟ.length = 0;	// not used, see cmp_keyvalue_value_i()
    entry = (keyvalue_T *)bsearch(&target, &rgb_tab, ARRAY_LENGTH(rgb_tab),
	    ଆକାର(rgb_tab[0]), cmp_keyvalue_ମୂଲ୍ୟ_i);
    ଯଦି (entry != ଖାଲି)
	ଫେରନ୍ତୁ gui_adjust_rgb((guicolor_T)entry->ଚାବି);

# ଯଦି defined(FEAT_EVAL)
    /*
     * Not a traditional color. Load additional color aliases and then consult the alias table.
     */

    color = colorname2rgb(name);
    ଯଦି (color == INVALCOLOR)
    {
	load_default_colors_lists();
	color = colorname2rgb(name);
    }

    ଫେରନ୍ତୁ color;
# ଅନ୍ୟଥା
    ଫେରନ୍ତୁ INVALCOLOR;
# endif
}

    guicolor_T
gui_get_rgb_color_cmn(ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ପୂର୍ଣ୍ଣସଂଖ୍ୟା g, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b)
{
    guicolor_T  color = RGB(r, g, b);

    ଯଦି (color > 0xffffff)
	ଫେରନ୍ତୁ INVALCOLOR;
    ଫେରନ୍ତୁ gui_adjust_rgb(color);
}
#endif

/*
 * Table with the specifications for an attribute number.
 * Note that this table is used by ALL buffers.  This is required because the
 * GUI can redraw at any time for any buffer.
 */
ସ୍ଥିର garray_T	term_attr_table = {0, 0, 0, 0, ଖାଲି};

#define TERM_ATTR_ENTRY(idx) ((attrentry_T *)term_attr_table.ga_data)[idx]

ସ୍ଥିର garray_T	cterm_attr_table = {0, 0, 0, 0, ଖାଲି};

#define CTERM_ATTR_ENTRY(idx) ((attrentry_T *)cterm_attr_table.ga_data)[idx]

#ifdef FEAT_GUI
ସ୍ଥିର garray_T	gui_attr_table = {0, 0, 0, 0, ଖାଲି};

# define GUI_ATTR_ENTRY(idx) ((attrentry_T *)gui_attr_table.ga_data)[idx]
#endif

/*
 * Return the attr number for a set of colors and font.
 * Add a new entry to the term_attr_table, cterm_attr_table or gui_attr_table
 * if the combination is new.
 * Return 0 for error (no more room).
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_attr_entry(garray_T *table, attrentry_T *aep)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    attrentry_T	*taep;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recursive = FALSE;

    // Init the table, in case it wasn't done yet.
    table->ga_itemsize = ଆକାର(attrentry_T);
    table->ga_growsize = 7;

    // Try to find an entry with the same specifications.
    ପାଇଁ (i = 0; i < table->ga_len; ++i)
    {
	taep = &(((attrentry_T *)table->ga_data)[i]);
	ଯଦି (	   aep->ae_attr == taep->ae_attr
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
			&& (aep->ae_u.term.start == ଖାଲି)
					    == (taep->ae_u.term.start == ଖାଲି)
			&& (aep->ae_u.term.start == ଖାଲି
			    || STRCMP(aep->ae_u.term.start,
						  taep->ae_u.term.start) == 0)
			&& (aep->ae_u.term.stop == ଖାଲି)
					     == (taep->ae_u.term.stop == ଖାଲି)
			&& (aep->ae_u.term.stop == ଖାଲି
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

	ଫେରନ୍ତୁ i + ATTR_OFF;
    }

    ଯଦି (table->ga_len + ATTR_OFF > MAX_TYPENR)
    {
	// Running out of attribute entries!  remove all attributes, and
	// compute new ones for all groups.
	// When called recursively, we are really out of numbers.
	ଯଦି (recursive)
	{
	    emsg(_(e_too_many_different_highlighting_attributes_in_use));
	    ଫେରନ୍ତୁ 0;
	}
	recursive = TRUE;

	clear_hl_tables();

	set_must_redraw(UPD_CLEAR);

	ପାଇଁ (i = 0; i < highlight_ga.ga_len; ++i)
	    set_hl_attr(i);

	recursive = FALSE;
    }

    // This is a new combination of colors and font, add an entry.
    ଯଦି (ga_grow(table, 1) == FAIL)
	ଫେରନ୍ତୁ 0;

    taep = &(((attrentry_T *)table->ga_data)[table->ga_len]);
    CLEAR_POINTER(taep);
    taep->ae_attr = aep->ae_attr;
#ifdef FEAT_GUI
    ଯଦି (table == &gui_attr_table)
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
    ଯଦି (table == &term_attr_table)
    {
	ଯଦି (aep->ae_u.term.start == ଖାଲି)
	    taep->ae_u.term.start = ଖାଲି;
	ଅନ୍ୟଥା
	    taep->ae_u.term.start = vim_strsave(aep->ae_u.term.start);
	ଯଦି (aep->ae_u.term.stop == ଖାଲି)
	    taep->ae_u.term.stop = ଖାଲି;
	ଅନ୍ୟଥା
	    taep->ae_u.term.stop = vim_strsave(aep->ae_u.term.stop);
    }
    ଅନ୍ୟଥା ଯଦି (table == &cterm_attr_table)
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
    ଫେରନ୍ତୁ (table->ga_len - 1 + ATTR_OFF);
}

#ଯଦି defined(FEAT_TERMINAL)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_cterm_attr_idx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bg)
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
    ଫେରନ୍ତୁ get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#ଯଦି defined(FEAT_TERMINAL) && defined(FEAT_TERMGUICOLORS)
/*
 * Get an attribute index for a 'termguicolors' entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_tgc_attr_idx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    ଯଦି (fg == INVALCOLOR && bg == INVALCOLOR)
    {
	// If both GUI colors are not set fall back to the cterm colors.  Helps
	// if the GUI only has an attribute, such as undercurl.
	at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
    }
    ଅନ୍ୟଥା
    {
	at_en.ae_u.cterm.fg_rgb = fg;
	at_en.ae_u.cterm.bg_rgb = bg;
    }
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
    ଫେରନ୍ତୁ get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#ଯଦି defined(FEAT_TERMINAL) && defined(FEAT_GUI)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_gui_attr_idx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    at_en.ae_u.gui.fg_color = fg;
    at_en.ae_u.gui.bg_color = bg;
    ଫେରନ୍ତୁ get_attr_entry(&gui_attr_table, &at_en);
}
#endif

/*
 * Clear all highlight tables.
 */
    ଶୂନ୍ୟ
clear_hl_tables(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    attrentry_T	*taep;

#ifdef FEAT_GUI
    ga_clear(&gui_attr_table);
#endif
    ପାଇଁ (i = 0; i < term_attr_table.ga_len; ++i)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
hl_combine_attr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା char_attr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prim_attr)
{
    attrentry_T *char_aep = ଖାଲି;
    attrentry_T *prim_aep;
    attrentry_T new_en;

    ଯଦି (char_attr == 0)
	ଫେରନ୍ତୁ prim_attr;
    ଯଦି (char_attr <= HL_ALL && prim_attr <= HL_ALL)
	ଫେରନ୍ତୁ ATTR_COMBINE(char_attr, prim_attr);
#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
    {
	ଯଦି (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	ଯଦି (char_aep != ଖାଲି)
	    new_en = *char_aep;
	ଅନ୍ୟଥା
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    ଯଦି (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	ଯଦି (prim_attr <= HL_ALL)
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	ଅନ୍ୟଥା
	{
	    prim_aep = syn_gui_attr2entry(prim_attr);
	    ଯଦି (prim_aep != ଖାଲି)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		ଯଦି (prim_aep->ae_u.gui.fg_color != INVALCOLOR)
		    new_en.ae_u.gui.fg_color = prim_aep->ae_u.gui.fg_color;
		ଯଦି (prim_aep->ae_u.gui.bg_color != INVALCOLOR)
		    new_en.ae_u.gui.bg_color = prim_aep->ae_u.gui.bg_color;
		ଯଦି (prim_aep->ae_u.gui.sp_color != INVALCOLOR)
		    new_en.ae_u.gui.sp_color = prim_aep->ae_u.gui.sp_color;
		ଯଦି (prim_aep->ae_u.gui.font != NOFONT)
		    new_en.ae_u.gui.font = prim_aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
		ଯଦି (prim_aep->ae_u.gui.fontset != NOFONTSET)
		    new_en.ae_u.gui.fontset = prim_aep->ae_u.gui.fontset;
# endif
	    }
	}
	ଫେରନ୍ତୁ get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    ଯଦି (IS_CTERM)
    {
	ଯଦି (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	ଯଦି (char_aep != ଖାଲି)
	    new_en = *char_aep;
	ଅନ୍ୟଥା
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    ଯଦି (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	ଯଦି (prim_attr <= HL_ALL)
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	ଅନ୍ୟଥା
	{
	    prim_aep = syn_cterm_attr2entry(prim_attr);
	    ଯଦି (prim_aep != ଖାଲି)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		ଯଦି (prim_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color = prim_aep->ae_u.cterm.fg_color;
		ଯଦି (prim_aep->ae_u.cterm.bg_color > 0)
		    new_en.ae_u.cterm.bg_color = prim_aep->ae_u.cterm.bg_color;
		ଯଦି (prim_aep->ae_u.cterm.ul_color > 0)
		    new_en.ae_u.cterm.ul_color = prim_aep->ae_u.cterm.ul_color;
		ଯଦି (prim_aep->ae_u.cterm.font > 0)
		    new_en.ae_u.cterm.font = prim_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		// If both fg and bg are not set fall back to cterm colors.
		// Helps for SpellBad which uses undercurl in the GUI.
		ଯଦି (COLOR_INVALID(prim_aep->ae_u.cterm.fg_rgb)
			&& COLOR_INVALID(prim_aep->ae_u.cterm.bg_rgb))
		{
		    ଯଦି (prim_aep->ae_u.cterm.fg_color > 0)
			new_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
		    ଯଦି (prim_aep->ae_u.cterm.bg_color > 0)
			new_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (prim_aep->ae_u.cterm.fg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.fg_rgb = prim_aep->ae_u.cterm.fg_rgb;
		    ଯଦି (prim_aep->ae_u.cterm.bg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.bg_rgb = prim_aep->ae_u.cterm.bg_rgb;
		}
		ଯଦି (prim_aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		    new_en.ae_u.cterm.ul_rgb = prim_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	}
	ଫେରନ୍ତୁ get_attr_entry(&cterm_attr_table, &new_en);
    }

    ଯଦି (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    ଯଦି (char_aep != ଖାଲି)
	new_en = *char_aep;
    ଅନ୍ୟଥା
    {
	CLEAR_FIELD(new_en);
	ଯଦି (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    ଯଦି (prim_attr <= HL_ALL)
	new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
    ଅନ୍ୟଥା
    {
	prim_aep = syn_term_attr2entry(prim_attr);
	ଯଦି (prim_aep != ଖାଲି)
	{
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_aep->ae_attr);
	    ଯଦି (prim_aep->ae_u.term.start != ଖାଲି)
	    {
		new_en.ae_u.term.start = prim_aep->ae_u.term.start;
		new_en.ae_u.term.stop = prim_aep->ae_u.term.stop;
	    }
	}
    }
    ଫେରନ୍ତୁ get_attr_entry(&term_attr_table, &new_en);
}

// ANSI color order: Black, Red, Green, Yellow, Blue, Magenta,
// Cyan, White, then bright variants.  Approximate RGB values used when
// only a cterm color number is known (no guifg/guibg).  Real terminal
// palettes may differ if the user reconfigured their emulator, but
// these are reasonable xterm-ish defaults.
ସ୍ଥିର ଅଚଳ long_u cterm_color_16[16] = {
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
    ସ୍ଥିର guicolor_T
cterm_color_to_rgb(ପୂର୍ଣ୍ଣସଂଖ୍ୟା color_nr)
{
    ଯଦି (color_nr < 1 || color_nr > 16)
	ଫେରନ୍ତୁ INVALCOLOR;
    ଫେରନ୍ତୁ (guicolor_T)cterm_color_16[color_nr - 1];
}
#endif

/*
 * Convert an xterm 256-color index (0-255) to an approximate RGB triple.
 * Uses the standard xterm palette: 0-15 ANSI (cterm_color_16), 16-231
 * 6x6x6 cube, 232-255 grayscale ramp.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cterm_idx_to_rgb(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *r, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *g, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *b)
{
    ସ୍ଥିର ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

    ଯଦି (idx < 0 || idx > 255)
    {
	*r = *g = *b = 0;
	ଫେରନ୍ତୁ;
    }
    ଯଦି (idx < 16)
    {
	long_u rgb = cterm_color_16[idx];
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
    }
    ଅନ୍ୟଥା ଯଦି (idx < 232)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = idx - 16;
	*r = cube[n / 36 % 6];
	*g = cube[n / 6  % 6];
	*b = cube[n      % 6];
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା v = 8 + (idx - 232) * 10;
	*r = *g = *b = v;
    }
}

/*
 * Approximate an RGB triple to the nearest xterm 256-color index.
 * Searches the 6x6x6 cube and the grayscale ramp; returns 0-255.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
rgb_to_cterm_idx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ପୂର୍ଣ୍ଣସଂଖ୍ୟା g, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b)
{
    ସ୍ଥିର ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା best = 0;
    long best_d = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    // Search the 6x6x6 cube.
    ପାଇଁ (i = 16; i < 232; ++i)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = i - 16;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cr = cube[n / 36 % 6];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg = cube[n / 6  % 6];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb = cube[n      % 6];
	long d = (cr - r) * (cr - r) + (cg - g) * (cg - g) + (cb - b) * (cb - b);
	ଯଦି (best_d < 0 || d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    // Search the grayscale ramp.
    ପାଇଁ (i = 232; i < 256; ++i)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା v = 8 + (i - 232) * 10;
	long d = (v - r) * (v - r) + (v - g) * (v - g) + (v - b) * (v - b);
	ଯଦି (d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    ଫେରନ୍ତୁ best;
}

/*
 * Resolve a color to an RGB triple.  Prefer the gui RGB value (set by
 * guifg/guibg) when valid, otherwise convert the cterm 256-color index.
 * Returns true on success; false when no color is defined.
 * cterm_c is 1-based (0 means "no color").
 */
    ସ୍ଥିର bool
resolve_color_to_rgb(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cterm_c, guicolor_T rgb UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *r, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *g, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *b)
{
#ifdef FEAT_TERMGUICOLORS
    ଯଦି (!COLOR_INVALID(rgb))
    {
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
	ଫେରନ୍ତୁ true;
    }
#endif
    ଯଦି (cterm_c > 0)
    {
	cterm_idx_to_rgb(cterm_c - 1, r, g, b);
	ଫେରନ୍ତୁ true;
    }
    ଫେରନ୍ତୁ false;
}

/*
 * get a RGB fallback color from gui, cterm or default color
 */
    ସ୍ଥିର guicolor_T
resolve_fallback_color(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cterm_c, guicolor_T rgb, guicolor_T default_rgb)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା red, green, blue;
    ଯଦି (!resolve_color_to_rgb(cterm_c, rgb, &red, &green, &blue))
	ଫେରନ୍ତୁ default_rgb;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ (red << 16) | (green << 8) | blue;
}

/*
 * get a RGB fallback foreground color from guifg, ctermfg or deduced from background
 */
    ସ୍ଥିର ଶୂନ୍ୟ
resolve_fallback_fg_to_rgb(ଶୂନ୍ୟ)
{
    guicolor_T fgcolor_or_gui_fgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    fgcolor_or_gui_fgcolor = cterm_normal_fg_gui_color;
#endif
#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	fgcolor_or_gui_fgcolor = gui.norm_pixel;
#endif
    fallback_fg_rgb = resolve_fallback_color(cterm_normal_fg_color, fgcolor_or_gui_fgcolor, (*p_bg == 'l') ? 0x000000 : 0xFFFFFF);
}

/*
 * get a RGB fallback background color from guifg, ctermbg or deduced from background
 */
    ସ୍ଥିର ଶୂନ୍ୟ
resolve_fallback_bg_to_rgb(ଶୂନ୍ୟ)
{
    guicolor_T bgcolor_or_gui_bgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    bgcolor_or_gui_bgcolor = cterm_normal_bg_gui_color;
#endif
#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
blend_cterm_colors(ପୂର୍ଣ୍ଣସଂଖ୍ୟା popup_c, guicolor_T popup_rgb,
		   ପୂର୍ଣ୍ଣସଂଖ୍ୟା under_c, guicolor_T under_rgb,
		   ପୂର୍ଣ୍ଣସଂଖ୍ୟା default_rgb,
		   ପୂର୍ଣ୍ଣସଂଖ୍ୟା blend_val)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pr, pg, pb, ur, ug, ub, r, g, b;

    ଯଦି (t_colors < 256)
	ଫେରନ୍ତୁ popup_c;
    ଯଦି (!resolve_color_to_rgb(popup_c, popup_rgb, &pr, &pg, &pb))
	ଫେରନ୍ତୁ under_c;
    ଯଦି (blend_val <= 0)
	ଫେରନ୍ତୁ rgb_to_cterm_idx(pr, pg, pb) + 1;
    ଯଦି (!resolve_color_to_rgb(under_c, under_rgb, &ur, &ug, &ub))
    {
	ur = (default_rgb >> 16) & 0xFF;
	ug = (default_rgb >> 8) & 0xFF;
	ub = default_rgb & 0xFF;
    }
    ଯଦି (blend_val >= 100)
	ଫେରନ୍ତୁ rgb_to_cterm_idx(ur, ug, ub) + 1;
    r = pr + (ur - pr) * blend_val / 100;
    g = pg + (ug - pg) * blend_val / 100;
    b = pb + (ub - pb) * blend_val / 100;
    ଫେରନ୍ତୁ rgb_to_cterm_idx(r, g, b) + 1;
}

#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Blend two RGB colors based on blend value (0-100).
 * blend: 0=return first argument color, 100=return second argument color
 * If bg_color is INVALCOLOR, high blend means more visible (return INVALCOLOR).
 */
    ସ୍ଥିର guicolor_T
blend_colors(guicolor_T popup_color, guicolor_T bg_color, ପୂର୍ଣ୍ଣସଂଖ୍ୟା blend_val)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r1, g1, b1, r2, g2, b2, r, g, b;

    // CTERMCOLOR is a sentinel meaning "use the cterm color"; for blending
    // it has no real RGB so treat it like INVALCOLOR.
    ଯଦି (COLOR_INVALID(popup_color))
	ଫେରନ୍ତୁ INVALCOLOR;

    // Fully transparent: use underlying color as-is.
    ଯଦି (blend_val >= 100)
	ଫେରନ୍ତୁ bg_color;

    r1 = (popup_color >> 16) & 0xFF;
    g1 = (popup_color >> 8) & 0xFF;
    b1 = popup_color & 0xFF;

    ଯଦି (COLOR_INVALID(bg_color))
	bg_color = fallback_bg_rgb;

    r2 = (bg_color >> 16) & 0xFF;
    g2 = (bg_color >> 8) & 0xFF;
    b2 = bg_color & 0xFF;

    r = r1 + (r2 - r1) * blend_val / 100;
    g = g1 + (g2 - g1) * blend_val / 100;
    b = b1 + (b2 - b1) * blend_val / 100;

    ଫେରନ୍ତୁ (r << 16) | (g << 8) | b;
}
#endif

/*
 * Blend attributes for popup windows and menus with opacity.
 * Blends foreground and/or background colors based on blend value (0-100).
 * blend: 0 = opaque (use popup colors), 100 = transparent (use background colors)
 * blend_fg: TRUE to blend foreground color, FALSE to keep popup foreground
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
hl_blend_attr_common(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		char_attr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		popup_attr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		blend,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		blend_fg)
{
    attrentry_T *char_aep = ଖାଲି;
    attrentry_T *popup_aep;
    attrentry_T new_en;
    attrentry_T tmp_en;

#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
    {
	ଯଦି (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	ଯଦି (char_aep != ଖାଲି)
	    new_en = *char_aep;
	ଅନ୍ୟଥା
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    ଯଦି (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	ଯଦି (popup_attr <= HL_ALL)
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
	ଅନ୍ୟଥା
	    popup_aep = syn_gui_attr2entry(popup_attr);

	ଯଦି (popup_aep != ଖାଲି)
	{
	    guicolor_T popup_bg_rgb = popup_aep->ae_u.gui.bg_color;
	    ଯଦି (COLOR_INVALID(popup_bg_rgb))
		popup_bg_rgb = fallback_bg_rgb;

	    ଯଦି (blend_fg)
	    {
		// blend_fg=TRUE: fade underlying text toward popup bg.
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_fg = fallback_fg_rgb;
		    ଯଦି (char_aep != ଖାଲି
			    && char_aep->ae_u.gui.fg_color != INVALCOLOR)
			base_fg = char_aep->ae_u.gui.fg_color;
		    new_en.ae_u.gui.fg_color = blend_colors(
			    popup_bg_rgb, base_fg, blend);
		}
	    }
	    ଅନ୍ୟଥା
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
		ଯଦି (char_aep != ଖାଲି
			&& !COLOR_INVALID(char_aep->ae_u.gui.bg_color))
		    underlying_bg = char_aep->ae_u.gui.bg_color;
		new_en.ae_u.gui.bg_color = blend_colors(
			popup_bg_rgb,
			underlying_bg, blend);
	    }
	}
	ଫେରନ୍ତୁ get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    ଯଦି (IS_CTERM)
    {
	ଯଦି (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	ଯଦି (char_aep != ଖାଲି)
	    new_en = *char_aep;
	ଅନ୍ୟଥା
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    ଯଦି (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	ଯଦି (popup_attr <= HL_ALL)
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
	ଅନ୍ୟଥା
	    popup_aep = syn_cterm_attr2entry(popup_attr);

	ଯଦି (popup_aep != ଖାଲି)
	{
	    guicolor_T popup_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
	    // Fall back to cterm color converted to RGB when gui color is not set.
	    popup_bg_rgb = popup_aep->ae_u.cterm.bg_rgb;
	    ଯଦି (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color > 0)
		popup_bg_rgb = cterm_color_to_rgb(
			popup_aep->ae_u.cterm.bg_color);
#endif
	    // assign default color if guibg and ctermbg are not set for popup
	    ଯଦି (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color == 0)
		popup_bg_rgb = fallback_bg_rgb;

	    ଯଦି (!blend_fg)
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
		ଯଦି (popup_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color =
			popup_aep->ae_u.cterm.fg_color;
		ଅନ୍ୟଥା ଯଦି (cterm_normal_fg_color > 0)
		    new_en.ae_u.cterm.fg_color = cterm_normal_fg_color;
		ଅନ୍ୟଥା
		    // black-ish or white-ish
		    new_en.ae_u.cterm.fg_color = (*p_bg == 'l') ? 1 : 16;
		new_en.ae_u.cterm.ul_color = popup_aep->ae_u.cterm.ul_color;
		new_en.ae_u.cterm.font = popup_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		new_en.ae_u.cterm.ul_rgb = popup_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	    ଅନ୍ୟଥା
	    {
		// blend_fg=TRUE: fade underlying fg toward popup bg in
		// the 256-color palette.  Used when the popup is over a
		// cell rendered with cterm colors (no termguicolors RGB).
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା under_fg = (char_aep != ଖାଲି)
		    ? char_aep->ae_u.cterm.fg_color : 0;
		guicolor_T under_fg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		ଯଦି (char_aep != ଖାଲି)
		    under_fg_rgb = char_aep->ae_u.cterm.fg_rgb;
#endif
		new_en.ae_u.cterm.fg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_fg, under_fg_rgb, fallback_fg_rgb, blend);
	    }
	    // Approximate cterm bg by blending with the underlying bg
	    // in the 256-color palette and mapping to the nearest entry.
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା under_bg = (char_aep != ଖାଲି)
		    ? char_aep->ae_u.cterm.bg_color : 0;
		guicolor_T under_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		ଯଦି (char_aep != ଖାଲି)
		    under_bg_rgb = char_aep->ae_u.cterm.bg_rgb;
#endif
		new_en.ae_u.cterm.bg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_bg, under_bg_rgb, fallback_bg_rgb, blend);
	    }
#ifdef FEAT_TERMGUICOLORS
	    // Blend RGB colors for termguicolors mode.
	    {
		ଯଦି (blend_fg)
		{
		    // blend_fg=TRUE: fade underlying text toward popup bg.
		    ଯଦି (popup_bg_rgb != INVALCOLOR)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_fg = fallback_fg_rgb;
			// CTERMCOLOR is a sentinel meaning "use the cterm
			// color"; treat it as no underlying color so it is
			// not blended in as a real near-white pixel.
			ଯଦି (char_aep != ଖାଲି
				&& !COLOR_INVALID(char_aep->ae_u.cterm.fg_rgb))
			    base_fg = char_aep->ae_u.cterm.fg_rgb;
			new_en.ae_u.cterm.fg_rgb = blend_colors(
				popup_bg_rgb, base_fg, blend);
		    }
		}
		ଅନ୍ୟଥା
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

		    ଯଦି (COLOR_INVALID(popup_fg)
			    && popup_aep->ae_u.cterm.fg_color > 0)
			popup_fg = cterm_color_to_rgb(
				popup_aep->ae_u.cterm.fg_color);

		    ଯଦି (!COLOR_INVALID(popup_fg))
			new_en.ae_u.cterm.fg_rgb = popup_fg;
		    ଅନ୍ୟଥା ଯଦି (!COLOR_INVALID(cterm_normal_fg_gui_color))
			new_en.ae_u.cterm.fg_rgb = cterm_normal_fg_gui_color;
		    ଅନ୍ୟଥା
			new_en.ae_u.cterm.fg_rgb = fallback_fg_rgb;
		}
		ଯଦି (popup_bg_rgb != INVALCOLOR)
		{
		    // Blend popup bg toward underlying bg
		    guicolor_T underlying_bg = fallback_bg_rgb;
		    ଯଦି (char_aep != ଖାଲି
			    && !COLOR_INVALID(char_aep->ae_u.cterm.bg_rgb))
			underlying_bg = char_aep->ae_u.cterm.bg_rgb;
		    new_en.ae_u.cterm.bg_rgb = blend_colors(
			    popup_bg_rgb, underlying_bg, blend);
		}
	    }
#endif
	}
	ଫେରନ୍ତୁ get_attr_entry(&cterm_attr_table, &new_en);
    }

    ଯଦି (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    ଯଦି (char_aep != ଖାଲି)
	new_en = *char_aep;
    ଅନ୍ୟଥା
    {
	CLEAR_FIELD(new_en);
	ଯଦି (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    // For term mode, no separate background color handling.
    ଫେରନ୍ତୁ get_attr_entry(&term_attr_table, &new_en);
}

/*
 * Blend attributes for popup windows with opacity.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
hl_blend_attr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା char_attr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା popup_attr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା blend, ପୂର୍ଣ୍ଣସଂଖ୍ୟା blend_fg)
{
    ଯଦି (blend >= 100 && blend_fg)
	ଫେରନ୍ତୁ char_attr;

    ଫେରନ୍ତୁ hl_blend_attr_common(char_attr, popup_attr, blend, blend_fg);
}

/*
 * Blend underlying foreground and background colors for pum space cells.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
hl_pum_blend_attr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା char_attr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା popup_attr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା blend)
{
    ଫେରନ୍ତୁ hl_blend_attr_common(char_attr, popup_attr, blend, TRUE);
}

#ifdef FEAT_GUI
    attrentry_T *
syn_gui_attr2entry(ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    attr -= ATTR_OFF;
    ଯଦି (attr >= gui_attr_table.ga_len)	    // did ":syntax clear"
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ &(GUI_ATTR_ENTRY(attr));
}
#endif

/*
 * Get the highlight attributes (HL_BOLD etc.) from an attribute nr.
 * Only to be used when "attr" > HL_ALL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_attr2attr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    attrentry_T	*aep;

#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	aep = syn_gui_attr2entry(attr);
    ଅନ୍ୟଥା
#endif
	ଯଦି (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	ଅନ୍ୟଥା
	    aep = syn_term_attr2entry(attr);

    ଯଦି (aep == ଖାଲି)	    // highlighting not set
	ଫେରନ୍ତୁ 0;
    ଫେରନ୍ତୁ aep->ae_attr;
}


    attrentry_T *
syn_term_attr2entry(ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    attr -= ATTR_OFF;
    ଯଦି (attr >= term_attr_table.ga_len)	    // did ":syntax clear"
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ &(TERM_ATTR_ENTRY(attr));
}

    attrentry_T *
syn_cterm_attr2entry(ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    attr -= ATTR_OFF;
    ଯଦି (attr >= cterm_attr_table.ga_len)	// did ":syntax clear"
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ &(CTERM_ATTR_ENTRY(attr));
}

#define LIST_ATTR   1
#define LIST_STRING 2
#define LIST_INT    3

    ସ୍ଥିର ଶୂନ୍ୟ
highlight_list_one(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    hl_group_T	    *sgp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    didh = FALSE;

    sgp = &HL_TABLE()[id - 1];	    // index is ID minus one

    ଯଦି (message_filtered(sgp->sg_name))
	ଫେରନ୍ତୁ;

    // Note: Keep this in sync with expand_highlight_group().
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_term, ଖାଲି, "term");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_start, "start");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_stop, "stop");

    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_cterm, ଖାଲି, "cterm");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_fg, ଖାଲି, "ctermfg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_bg, ଖାଲି, "ctermbg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_ul, ଖାଲି, "ctermul");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_font, ଖାଲି, "ctermfont");

#ଯଦି defined(FEAT_GUI) || defined(FEAT_EVAL)
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_gui, ଖାଲି, "gui");
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

    ଯଦି (sgp->sg_link && !got_int)
    {
	(ଶୂନ୍ୟ)syn_list_header(didh, 9999, id);
	didh = TRUE;
	ସନ୍ଦେଶ_puts_attr("links to", HL_ATTR(HLF_D));
	ସନ୍ଦେଶ_putchar(' ');
	ସନ୍ଦେଶ_outtrans(HL_TABLE()[HL_TABLE()[id - 1].sg_link - 1].sg_name);
    }

    ଯଦି (!didh)
	highlight_list_arg(id, didh, LIST_STRING, 0, (char_u *)"cleared", "");
#ifdef FEAT_EVAL
    ଯଦି (p_verbose > 0)
	last_set_ସନ୍ଦେଶ(sgp->sg_script_ctx);
#endif
}

    ସ୍ଥିର char_u*
highlight_arg_to_string(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		iarg,
    char_u	*sarg,
    char_u	*buf)
{
    ଯଦି (type == LIST_INT)
	sprintf((ଅକ୍ଷର *)buf, "%d", iarg - 1);
    ଅନ୍ୟଥା ଯଦି (type == LIST_STRING)
	ଫେରନ୍ତୁ sarg;
    ଅନ୍ୟଥା // type == LIST_ATTR
    {
	size_t buflen;

	buf[0] = NUL;
	buflen = 0;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(highlight_index_tab); ++i)
	{
	    ଯଦି (iarg & highlight_index_tab[i]->ଚାବି)
	    {
		ଯଦି (buflen > 0)
		{
		    STRCPY(buf + buflen, (char_u *)",");
		    ++buflen;
		}
		STRCPY(buf + buflen, highlight_index_tab[i]->ମୂଲ୍ୟ.string);
		buflen += highlight_index_tab[i]->ମୂଲ୍ୟ.length;
		iarg &= ~highlight_index_tab[i]->ଚାବି;	    // don't want "inverse"/"reverse"
	    }
	}
    }
    ଫେରନ୍ତୁ buf;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
highlight_list_arg(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		didh,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		iarg,
    char_u	*sarg,
    ଅକ୍ଷର	*name)
{
    char_u	buf[MAX_ATTR_LEN];
    char_u	*ts;

    ଯଦି (got_int)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (type == LIST_STRING ? (sarg == ଖାଲି) : (iarg == 0))
	ଫେରନ୍ତୁ didh;

    ts = highlight_arg_to_string(type, iarg, sarg, buf);

    (ଶୂନ୍ୟ)syn_list_header(didh,
	    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(vim_strsize(ts) + STRLEN(name) + 1), id);
    didh = TRUE;
    ଯଦି (!got_int)
    {
	ଯଦି (*name != NUL)
	{
	    ସନ୍ଦେଶ_puts_attr(name, HL_ATTR(HLF_D));
	    ସନ୍ଦେଶ_puts_attr("=", HL_ATTR(HLF_D));
	}
	ସନ୍ଦେଶ_outtrans(ts);
    }
    ଫେରନ୍ତୁ didh;
}

#ଯଦି ((defined(FEAT_EVAL) || defined(FEAT_PRINTER))) && defined(FEAT_SYN_HL)
/*
 * Return "1" if highlight group "id" has attribute "flag".
 * Return NULL otherwise.
 */
    char_u *
highlight_has_attr(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flag,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr;

    ଯଦି (id <= 0 || id > highlight_ga.ga_len)
	ଫେରନ୍ତୁ ଖାଲି;

# ଯଦି defined(FEAT_GUI) || defined(FEAT_EVAL)
    ଯଦି (modec == 'g')
	attr = HL_TABLE()[id - 1].sg_gui;
    ଅନ୍ୟଥା
# endif
    {
	ଯଦି (modec == 'c')
	    attr = HL_TABLE()[id - 1].sg_cterm;
	ଅନ୍ୟଥା
	    attr = HL_TABLE()[id - 1].sg_term;
    }

    ଯଦି (attr & flag)
	ଫେରନ୍ତୁ (char_u *)"1";
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(FEAT_SYN_HL) && defined(FEAT_EVAL)
/*
 * Return color name of highlight group "id".
 */
    char_u *
highlight_color(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id,
    char_u	*what,	// "font", "fg", "bg", "sp", "ul", "fg#", "bg#" or "sp#"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    ସ୍ଥିର char_u	name[20];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			fg = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			sp = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ul = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			font = FALSE;

    ଯଦି (id <= 0 || id > highlight_ga.ga_len)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'g')
	fg = TRUE;
    ଅନ୍ୟଥା ଯଦି (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'o'
	     && TOLOWER_ASC(what[2]) == 'n' && TOLOWER_ASC(what[3]) == 't')
	font = TRUE;
    ଅନ୍ୟଥା ଯଦି (TOLOWER_ASC(what[0]) == 's' && TOLOWER_ASC(what[1]) == 'p')
	sp = TRUE;
    ଅନ୍ୟଥା ଯଦି (TOLOWER_ASC(what[0]) == 'u' && TOLOWER_ASC(what[1]) == 'l')
	ul = TRUE;
    ଅନ୍ୟଥା ଯଦି (!(TOLOWER_ASC(what[0]) == 'b' && TOLOWER_ASC(what[1]) == 'g'))
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (modec == 'g')
    {
# ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI
	// return font name
	ଯଦି (font)
	    ଫେରନ୍ତୁ HL_TABLE()[id - 1].sg_font_name;
#  endif

	// return #RRGGBB form (only possible when GUI is running)
	ଯଦି ((USE_24BIT) && what[2] == '#')
	{
	    guicolor_T		color;
	    long_u		rgb;
	    ସ୍ଥିର char_u	buf[10];

	    ଯଦି (fg)
		color = HL_TABLE()[id - 1].sg_gui_fg;
	    ଅନ୍ୟଥା ଯଦି (sp)
		color = HL_TABLE()[id - 1].sg_gui_sp;
	    ଅନ୍ୟଥା
		color = HL_TABLE()[id - 1].sg_gui_bg;
	    ଯଦି (color == INVALCOLOR)
		ଫେରନ୍ତୁ ଖାଲି;
	    rgb = (long_u)GUI_MCH_GET_RGB(color);
	    sprintf((ଅକ୍ଷର *)buf, "#%02x%02x%02x",
				      (unsigned)(rgb >> 16),
				      (unsigned)(rgb >> 8) & 255,
				      (unsigned)rgb & 255);
	    ଫେରନ୍ତୁ buf;
	}
# endif
	ଯଦି (fg)
	    ଫେରନ୍ତୁ (HL_TABLE()[id - 1].sg_gui_fg_name);
	ଯଦି (sp)
	    ଫେରନ୍ତୁ (HL_TABLE()[id - 1].sg_gui_sp_name);
	ଫେରନ୍ତୁ (HL_TABLE()[id - 1].sg_gui_bg_name);
    }
    ଯଦି (sp)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (modec == 'c')
    {
	ଯଦି (fg)
	    n = HL_TABLE()[id - 1].sg_cterm_fg - 1;
	ଅନ୍ୟଥା ଯଦି (ul)
	    n = HL_TABLE()[id - 1].sg_cterm_ul - 1;
	ଅନ୍ୟଥା ଯଦି (font)
	    n = HL_TABLE()[id - 1].sg_cterm_font - 1;
	ଅନ୍ୟଥା
	    n = HL_TABLE()[id - 1].sg_cterm_bg - 1;
	ଯଦି (n < 0)
	    ଫେରନ୍ତୁ ଖାଲି;
	sprintf((ଅକ୍ଷର *)name, "%d", n);
	ଫେରନ୍ତୁ name;
    }
    // term doesn't have color
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(FEAT_SYN_HL) \
	    && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)) \
	&& defined(FEAT_PRINTER)
/*
 * Return color name of highlight group "id" as RGB value.
 */
    long_u
highlight_gui_color_rgb(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fg)	// TRUE = fg, FALSE = bg
{
    guicolor_T	color;

    ଯଦି (id <= 0 || id > highlight_ga.ga_len)
	ଫେରନ୍ତୁ 0L;

    ଯଦି (fg)
	color = HL_TABLE()[id - 1].sg_gui_fg;
    ଅନ୍ୟଥା
	color = HL_TABLE()[id - 1].sg_gui_bg;

    ଯଦି (color == INVALCOLOR)
	ଫେରନ୍ତୁ 0L;

    ଫେରନ୍ତୁ GUI_MCH_GET_RGB(color);
}
#endif

/*
 * Output the syntax list header.
 * Return TRUE when started a new line.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_list_header(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    did_header,		// did header already
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    outlen,		// length of string that comes
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id)			// highlight group id
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    endcol = 19;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    newline = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    name_col = 0;

    ଯଦି (!did_header)
    {
	ସନ୍ଦେଶ_putchar('\n');
	ଯଦି (got_int)
	    ଫେରନ୍ତୁ TRUE;
	ସନ୍ଦେଶ_outtrans(HL_TABLE()[id - 1].sg_name);
	name_col = ସନ୍ଦେଶ_col;
	endcol = 15;
    }
    ଅନ୍ୟଥା ଯଦି (ସନ୍ଦେଶ_col + outlen + 1 >= cmdline_width)
    {
	ସନ୍ଦେଶ_putchar('\n');
	ଯଦି (got_int)
	    ଫେରନ୍ତୁ TRUE;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (ସନ୍ଦେଶ_col >= endcol)	// wrap around is like starting a new line
	    newline = FALSE;
    }

    ଯଦି (ସନ୍ଦେଶ_col >= endcol)	// output at least one space
	endcol = ସନ୍ଦେଶ_col + 1;
    ଯଦି (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    ସନ୍ଦେଶ_advance(endcol);

    // Show "xxx" with the attributes.
    ଯଦି (!did_header)
    {
	ଯଦି (endcol == cmdline_width - 1 && endcol <= name_col)
	    ସନ୍ଦେଶ_putchar(' ');
	ସନ୍ଦେଶ_puts_attr("xxx", syn_id2attr(id));
	ସନ୍ଦେଶ_putchar(' ');
    }

    ଫେରନ୍ତୁ newline;
}

/*
 * Set the attribute numbers for a highlight group.
 * Called after one of the attributes has changed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_hl_attr(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx)	    // index in array
{
    attrentry_T	    at_en;
    hl_group_T	    *sgp = HL_TABLE() + idx;

    // The "Normal" group doesn't need an attribute number
    ଯଦି (sgp->sg_name_u != ଖାଲି && STRCMP(sgp->sg_name_u, "NORMAL") == 0)
	ଫେରନ୍ତୁ;

#ifdef FEAT_GUI
    // For the GUI mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    ଯଦି (sgp->sg_gui_fg == INVALCOLOR
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
    ଅନ୍ୟଥା
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
    ଯଦି (sgp->sg_start == ଖାଲି && sgp->sg_stop == ଖାଲି)
	sgp->sg_term_attr = sgp->sg_term;
    ଅନ୍ୟଥା
    {
	at_en.ae_attr = sgp->sg_term;
	at_en.ae_u.term.start = sgp->sg_start;
	at_en.ae_u.term.stop = sgp->sg_stop;
	sgp->sg_term_attr = get_attr_entry(&term_attr_table, &at_en);
    }

    // For the color term mode: If there are other than "normal"
    // highlighting attributes, need to allocate an attr number.
    ଯଦି (sgp->sg_cterm_fg == 0 && sgp->sg_cterm_bg == 0 &&
	sgp->sg_cterm_ul == 0 && sgp->sg_cterm_font == 0
#ifdef FEAT_TERMGUICOLORS
	    && sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
#endif
	    )
	sgp->sg_cterm_attr = sgp->sg_cterm;
    ଅନ୍ୟଥା
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
	ଯଦି (sgp->sg_cterm & (HL_UNDERLINE | HL_UNDERCURL
			   | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED))
	    at_en.ae_u.cterm.ul_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_sp);
	ଅନ୍ୟଥା
	    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	ଯଦି (at_en.ae_u.cterm.fg_rgb == INVALCOLOR
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_override(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    ଯଦି (overrides != ଖାଲି && overrides->arr != ଖାଲି)
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < overrides->len; k++)
	    ଯଦି (overrides->arr[k].from == id)
		ଫେରନ୍ତୁ overrides->arr[k].to;
    ଫେରନ୍ତୁ id;
}

/*
 * Lookup a highlight group name by pointer and length.
 * If it is not found, 0 is returned.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_name2id_len(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    char_u	name_u[MAX_SYN_NAME + 1];
    hashitem_T	*hi;

    ଯଦି (len > MAX_SYN_NAME)
	len = MAX_SYN_NAME;
    mch_memmove(name_u, name, len);
    name_u[len] = NUL;
    vim_strup(name_u);
    ଯଦି (!highlight_ht_inited)
	ଫେରନ୍ତୁ 0;
    hi = hash_find(&highlight_ht, name_u);
    ଯଦି (HASHITEM_EMPTY(hi))
	ଫେରନ୍ତୁ 0;
    ଫେରନ୍ତୁ HI2HLNAME(hi)->hn_id;
}

/*
 * Lookup a highlight group name and return its ID.
 * If it is not found, 0 is returned.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_name2id(char_u *name)
{
    // Avoid using stricmp() too much, it's slow on some systems.
    ଫେରନ୍ତୁ syn_name2id_len(name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(name));
}

/*
 * Lookup a highlight group name and return its attributes.
 * Return zero if not found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_name2attr(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା id = syn_name2id(name);

    ଯଦି (id != 0)
	ଫେରନ୍ତୁ syn_id2attr(id);
    ଫେରନ୍ତୁ 0;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Return TRUE if highlight group "name" exists.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
highlight_exists(char_u *name)
{
    ଫେରନ୍ତୁ (syn_name2id(name) > 0);
}

# ଯଦି defined(FEAT_ସନ୍ଧାନ_EXTRA)
/*
 * Return the name of highlight group "id".
 * When not a valid ID return an empty string.
 */
    char_u *
syn_id2name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    ଯଦି (id <= 0 || id > highlight_ga.ga_len)
	ଫେରନ୍ତୁ (char_u *)"";
    ଫେରନ୍ତୁ HL_TABLE()[id - 1].sg_name;
}
# endif
#endif

/*
 * Like syn_name2id(), but take a pointer + length argument.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_namen2id(char_u *linep, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ଫେରନ୍ତୁ syn_name2id_len(linep, len);
}

/*
 * Find highlight group name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_check_group(char_u *pp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id;
    char_u  *name;

    ଯଦି (len > MAX_SYN_NAME)
    {
	emsg(_(e_highlight_group_name_too_long));
	ଫେରନ୍ତୁ 0;
    }
    id = syn_name2id_len(pp, len);
    ଯଦି (id == 0)			// doesn't exist yet
    {
	name = vim_strnsave(pp, len);
	ଯଦି (name == ଖାଲି)
	    ଫେରନ୍ତୁ 0;
	id = syn_add_group(name);
    }
    ଫେରନ୍ତୁ id;
}

/*
 * Add new highlight group and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_add_group(char_u *name)
{
    char_u	*p;
    char_u	*name_up;

    // Check that the name is valid (ASCII letters, digits, underscores, dots, or hyphens).
    ପାଇଁ (p = name; *p != NUL; ++p)
    {
	ଯଦି (!vim_isprintc(*p))
	{
	    emsg(_(e_unprintable_character_in_group_name));
	    vim_free(name);
	    ଫେରନ୍ତୁ 0;
	}
	ଅନ୍ୟଥା ଯଦି (!ASCII_ISALNUM(*p) && *p != '_' && *p != '.' && *p != '-')
	{
	    // This is an error, but since there previously was no check only
	    // give a warning.
	    ସନ୍ଦେଶ_source(HL_ATTR(HLF_W));
	    ସନ୍ଦେଶ(_("W18: Invalid character in group name"));
	    ଭାଙ୍ଗ;
	}
    }

    // First call for this growarray: init growing array.
    ଯଦି (highlight_ga.ga_data == ଖାଲି)
    {
	highlight_ga.ga_itemsize = ଆକାର(hl_group_T);
	highlight_ga.ga_growsize = 10;
	hash_init(&highlight_ht);
	highlight_ht_inited = true;
    }

    ଯଦି (highlight_ga.ga_len >= MAX_HL_ID)
    {
	emsg(_(e_too_many_highlight_and_syntax_groups));
	vim_free(name);
	ଫେରନ୍ତୁ 0;
    }

    // Make room for at least one other syntax_highlight entry.
    ଯଦି (ga_grow(&highlight_ga, 1) == FAIL)
    {
	vim_free(name);
	ଫେରନ୍ତୁ 0;
    }

    {
	hlname_T    *hn;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(name);

	hn = alloc(offsetof(hlname_T, hn_ଚାବି) + len + 1);
	ଯଦି (hn == ଖାଲି)
	{
	    vim_free(name);
	    ଫେରନ୍ତୁ 0;
	}
	vim_strncpy(hn->hn_ଚାବି, name, len);
	vim_strup(hn->hn_ଚାବି);
	hn->hn_id = highlight_ga.ga_len + 1;	// ID is index plus one
	name_up = hn->hn_ଚାବି;
    }

    CLEAR_POINTER(&(HL_TABLE()[highlight_ga.ga_len]));
    HL_TABLE()[highlight_ga.ga_len].sg_name = name;
    HL_TABLE()[highlight_ga.ga_len].sg_name_u = name_up;
#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_TABLE()[highlight_ga.ga_len].sg_gui_bg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_fg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_sp = INVALCOLOR;
#endif
    hash_add(&highlight_ht, name_up, "highlight");
    ++highlight_ga.ga_len;

    ଫେରନ୍ତୁ highlight_ga.ga_len;		    // ID is index plus one
}

/*
 * When, just after calling syn_add_group(), an error is discovered, this
 * function deletes the new name.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_unadd_group(ଶୂନ୍ୟ)
{
    hashitem_T *hi;

    --highlight_ga.ga_len;
    hi = hash_find(&highlight_ht, HL_TABLE()[highlight_ga.ga_len].sg_name_u);
    ଯଦି (!HASHITEM_EMPTY(hi))
	hash_remove(&highlight_ht, hi, "highlight");
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name);
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name_u - HLNAME_ଚାବି_OFF);
}

/*
 * Translate a group ID to highlight attributes.
 * "hl_id" must be valid: > 0, caller must check.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_id2attr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hl_id)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr;
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ପ୍ରତିଜ୍ଞା(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

#ifdef FEAT_GUI
    // Only use GUI attr when the GUI is being used.
    ଯଦି (gui.in_use)
	attr = sgp->sg_gui_attr;
    ଅନ୍ୟଥା
#endif
	ଯଦି (IS_CTERM)
	    attr = sgp->sg_cterm_attr;
	ଅନ୍ୟଥା
	    attr = sgp->sg_term_attr;

    ଫେରନ୍ତୁ attr;
}

#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Get the GUI colors and attributes for a group ID.
 * NOTE: the colors will be INVALCOLOR when not set, the color otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_id2colors(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hl_id, guicolor_T *fgp, guicolor_T *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ପ୍ରତିଜ୍ଞା(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

    *fgp = sgp->sg_gui_fg;
    *bgp = sgp->sg_gui_bg;
    ଫେରନ୍ତୁ sgp->sg_gui;
}
#endif

#ଯଦି (defined(MSWIN) \
	    && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL)) \
	    && defined(FEAT_TERMGUICOLORS)) \
	|| defined(FEAT_TERMINAL)
    ଶୂନ୍ୟ
syn_id2cterm_bg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hl_id, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *fgp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ପ୍ରତିଜ୍ଞା(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
    *fgp = sgp->sg_cterm_fg - 1;
    *bgp = sgp->sg_cterm_bg - 1;
}
#endif

/*
 * Translate a group ID to the final group ID (following links).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_get_final_id(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hl_id)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;
    hl_group_T	*sgp;

    ଯଦି (hl_id > highlight_ga.ga_len || hl_id < 1)
	ଫେରନ୍ତୁ 0;			// Can be called from eval!!

    // Follow links until there is no more.
    // Look out for loops!  Break after 100 links.
    ପାଇଁ (ଗଣନା_2 = 100; --ଗଣନା_2 >= 0; )
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmp = hl_id;

	sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
	ଯଦି (sgp->sg_link == 0 || sgp->sg_link > highlight_ga.ga_len)
	    ଭାଙ୍ଗ;

	// This is to handle highlight groups that are overridden but are in the
	// middle of a link chain.
	hl_id = syn_override(hl_id);
	ଯଦି (tmp != hl_id)
	    ଜାରି;
	hl_id = sgp->sg_link;
    }
    ଫେରନ୍ତୁ syn_override(hl_id);
}

#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Call this function just after the GUI has started.
 * Also called when 'termguicolors' was set, gui.in_use will be FALSE then.
 * It finds the font and color handles for the highlighting groups.
 */
    ଶୂନ୍ୟ
highlight_gui_started(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx;

    // First get the colors from the "Normal" and "Menu" group, if set
    ଯଦି (USE_24BIT)
	set_normal_colors();

    ପାଇଁ (idx = 0; idx < highlight_ga.ga_len; ++idx)
	gui_do_one_color(idx, FALSE, FALSE);

    highlight_changed();
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_do_one_color(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_menu UNUSED,	   // TRUE: might set the menu font
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_tooltip UNUSED) // TRUE: might set the tooltip font
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		didit = FALSE;

# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    ଯଦି (gui.in_use)
#  endif
	ଯଦି (HL_TABLE()[idx].sg_font_name != ଖାଲି)
	{
	    hl_do_font(idx, HL_TABLE()[idx].sg_font_name, FALSE, do_menu,
							    do_tooltip, TRUE);
	    didit = TRUE;
	}
# endif
    ଯଦି (HL_TABLE()[idx].sg_gui_fg_name != ଖାଲି)
    {
	HL_TABLE()[idx].sg_gui_fg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_fg_name);
	didit = TRUE;
    }
    ଯଦି (HL_TABLE()[idx].sg_gui_bg_name != ଖାଲି)
    {
	HL_TABLE()[idx].sg_gui_bg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_bg_name);
	didit = TRUE;
    }
    ଯଦି (HL_TABLE()[idx].sg_gui_sp_name != ଖାଲି)
    {
	HL_TABLE()[idx].sg_gui_sp =
			    color_name2handle(HL_TABLE()[idx].sg_gui_sp_name);
	didit = TRUE;
    }
    ଯଦି (didit)	// need to get a new attr number
	set_hl_attr(idx);
}
#endif

#ଯଦି defined(USER_HIGHLIGHT) && defined(FEAT_STL_OPT)
/*
 * Apply difference between User[1-9] and HLF_S to HLF_SNC, HLF_ST or HLF_STNC.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
combine_stl_hlt(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା id,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା id_S,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା id_alt,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା hlcnt,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା hlf,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *table)
{
    hl_group_T *hlt = HL_TABLE();

    ଯଦି (id_alt == 0)
    {
	CLEAR_POINTER(&hlt[hlcnt + i]);
	hlt[hlcnt + i].sg_term = highlight_attr[hlf];
	hlt[hlcnt + i].sg_cterm = highlight_attr[hlf];
# ଯଦି defined(FEAT_GUI) || defined(FEAT_EVAL)
	hlt[hlcnt + i].sg_gui = highlight_attr[hlf];
# endif
    }
    ଅନ୍ୟଥା
	mch_memmove(&hlt[hlcnt + i],
		    &hlt[id_alt - 1],
		    ଆକାର(hl_group_T));
    hlt[hlcnt + i].sg_link = 0;

    hlt[hlcnt + i].sg_term ^=
	hlt[id - 1].sg_term ^ hlt[id_S - 1].sg_term;
    ଯଦି (hlt[id - 1].sg_start != hlt[id_S - 1].sg_start)
	hlt[hlcnt + i].sg_start = hlt[id - 1].sg_start;
    ଯଦି (hlt[id - 1].sg_stop != hlt[id_S - 1].sg_stop)
	hlt[hlcnt + i].sg_stop = hlt[id - 1].sg_stop;
    hlt[hlcnt + i].sg_cterm ^=
	hlt[id - 1].sg_cterm ^ hlt[id_S - 1].sg_cterm;
    ଯଦି (hlt[id - 1].sg_cterm_fg != hlt[id_S - 1].sg_cterm_fg)
	hlt[hlcnt + i].sg_cterm_fg = hlt[id - 1].sg_cterm_fg;
    ଯଦି (hlt[id - 1].sg_cterm_bg != hlt[id_S - 1].sg_cterm_bg)
	hlt[hlcnt + i].sg_cterm_bg = hlt[id - 1].sg_cterm_bg;
    ଯଦି (hlt[id - 1].sg_cterm_font != hlt[id_S - 1].sg_cterm_font)
	hlt[hlcnt + i].sg_cterm_font = hlt[id - 1].sg_cterm_font;
# ଯଦି defined(FEAT_GUI) || defined(FEAT_EVAL)
    hlt[hlcnt + i].sg_gui ^=
	hlt[id - 1].sg_gui ^ hlt[id_S - 1].sg_gui;
# endif
# ifdef FEAT_GUI
    ଯଦି (hlt[id - 1].sg_gui_fg != hlt[id_S - 1].sg_gui_fg)
	hlt[hlcnt + i].sg_gui_fg = hlt[id - 1].sg_gui_fg;
    ଯଦି (hlt[id - 1].sg_gui_bg != hlt[id_S - 1].sg_gui_bg)
	hlt[hlcnt + i].sg_gui_bg = hlt[id - 1].sg_gui_bg;
    ଯଦି (hlt[id - 1].sg_gui_sp != hlt[id_S - 1].sg_gui_sp)
	hlt[hlcnt + i].sg_gui_sp = hlt[id - 1].sg_gui_sp;
    ଯଦି (hlt[id - 1].sg_font != hlt[id_S - 1].sg_font)
	hlt[hlcnt + i].sg_font = hlt[id - 1].sg_font;
#  ifdef FEAT_XFONTSET
    ଯଦି (hlt[id - 1].sg_fontset != hlt[id_S - 1].sg_fontset)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
highlight_changed(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hlf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*p;
    char_u	*default_hl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr;
    char_u	*end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
#ifdef USER_HIGHLIGHT
    char_u      userhl[30];  // use 30 to avoid compiler warning
# ifdef FEAT_STL_OPT
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id_S = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id_SNC = 0;
#  ifdef FEAT_TERMINAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id_ST = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id_STNC = 0;
#  endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hlcnt;
# endif
#endif
    win_T	*wp;

    need_highlight_changed = FALSE;

#ifdef FEAT_TERMINAL
    term_update_colors_all();
    term_update_hlfwin_all();
#endif

    // Clear all attributes.
    ପାଇଁ (hlf = 0; hlf < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)HLF_ଗଣନା; ++hlf)
    {
	highlight_attr[hlf] = 0;
	highlight_attr_raw[hlf] = 0;
	highlight_ids[hlf] = 0;
    }

    default_hl = get_highlight_default();

    // First set all attributes to their default value.
    // Then use the attributes from the 'highlight' option, unless it already
    // matches the default and would repeat the same work.
    ପାଇଁ (i = 0; i < 2; ++i)
    {
	ଯଦି (i)
	{
	    ଯଦି (default_hl != ଖାଲି && p_hl != ଖାଲି
				      && STRCMP(default_hl, p_hl) == 0)
		ଜାରି;
	    p = p_hl;
	}
	ଅନ୍ୟଥା
	    p = default_hl;
	ଯଦି (p == ଖାଲି)	    // just in case
	    ଜାରି;

	ଯେତେବେଳେ (*p)
	{
	    ପାଇଁ (hlf = 0; hlf < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)HLF_ଗଣନା; ++hlf)
		ଯଦି (hl_flags[hlf] == *p)
		    ଭାଙ୍ଗ;
	    ++p;
	    ଯଦି (hlf == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)HLF_ଗଣନା || *p == NUL)
		ଫେରନ୍ତୁ FAIL;

	    // Allow several hl_flags to be combined, like "bu" for
	    // bold-underlined.
	    attr = 0;
	    id = 0;
	    ପାଇଁ ( ; *p && *p != ','; ++p)	    // parse up to comma
	    {
		ଯଦି (VIM_ISWHITE(*p))		    // ignore white space
		    ଜାରି;

		ଯଦି (attr > HL_ALL)  // Combination with ':' is not allowed.
		    ଫେରନ୍ତୁ FAIL;

		// Note: Keep this in sync with expand_set_highlight().
		ବାଛ (*p)
		{
		    ସ୍ଥିତି 'b':	attr |= HL_BOLD;
				ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'i':	attr |= HL_ITALIC;
				ଭାଙ୍ଗ;
		    ସ୍ଥିତି '-':
		    ସ୍ଥିତି 'n':			    // no highlighting
				ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'r':	attr |= HL_INVERSE;
				ଭାଙ୍ଗ;
		    ସ୍ଥିତି 's':	attr |= HL_STANDOUT;
				ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'u':	attr |= HL_UNDERLINE;
				ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'c':	attr |= HL_UNDERCURL;
				ଭାଙ୍ଗ;
		    ସ୍ଥିତି '2':	attr |= HL_UNDERDOUBLE;
				ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'd':	attr |= HL_UNDERDOTTED;
				ଭାଙ୍ଗ;
		    ସ୍ଥିତି '=':	attr |= HL_UNDERDASHED;
				ଭାଙ୍ଗ;
		    ସ୍ଥିତି 't':	attr |= HL_STRIKETHROUGH;
				ଭାଙ୍ଗ;
		    ସ୍ଥିତି ':':	++p;		    // highlight group name
				ଯଦି (attr || *p == NUL)	 // no combinations
				    ଫେରନ୍ତୁ FAIL;
				end = vim_strchr(p, ',');
				ଯଦି (end == ଖାଲି)
				    end = p + STRLEN(p);
				id = syn_check_group(p, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - p));
				ଯଦି (id == 0)
				    ଫେରନ୍ତୁ FAIL;
				attr = syn_id2attr(id);
				p = end - 1;
#ଯଦି defined(FEAT_STL_OPT) && defined(USER_HIGHLIGHT)
				ଯଦି (hlf == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)HLF_SNC)
				    id_SNC = syn_get_final_id(id);
# ifdef FEAT_TERMINAL
				ଅନ୍ୟଥା ଯଦି (hlf == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)HLF_ST)
				    id_ST = syn_get_final_id(id);
				ଅନ୍ୟଥା ଯଦି (hlf == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)HLF_STNC)
				    id_STNC = syn_get_final_id(id);
# endif
				ଅନ୍ୟଥା ଯଦି (hlf == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)HLF_S)
				    id_S = syn_get_final_id(id);
#endif
				ଭାଙ୍ଗ;
		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	ଫେରନ୍ତୁ FAIL;
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
	ଅକ୍ଷର *errmsg = update_winhighlight(wp, wp->w_p_whl);

	ଯଦି (errmsg != ଖାଲି)
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
    ଯଦି (ga_grow(&highlight_ga, 28) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    hlcnt = highlight_ga.ga_len;
    ଯଦି (id_S == -1)
    {
	// Make sure id_S is always valid to simplify code below. Use the last
	// entry.
	CLEAR_POINTER(&HL_TABLE()[hlcnt + 27]);
	HL_TABLE()[hlcnt + 18].sg_term = highlight_attr[HLF_S];
	id_S = hlcnt + 19;
    }
# endif
    ପାଇଁ (i = 0; i < 9; i++)
    {
	sprintf((ଅକ୍ଷର *)userhl, "User%d", i + 1);
	id = syn_name2id(userhl);
	ଯଦି (id == 0)
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
	ଅନ୍ୟଥା
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

    ଫେରନ୍ତୁ OK;
}

ସ୍ଥିର ଶୂନ୍ୟ highlight_list(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ highlight_list_two(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr);

// context for :highlight <group> <arg> expansion
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା expand_hi_synid = 0;	    // ID for highlight group being completed
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା expand_hi_equal_col = 0; // column where the '=' is
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା expand_hi_include_orig = 0;	    // whether to fill the existing current value or not
ସ୍ଥିର char_u *expand_hi_curvalue = ଖାଲି;   // the existing current value
#ଯଦି defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
ସ୍ଥିର dict_iterator_T expand_colornames_iter;	// iterator for looping through v:colornames
#endif

/*
 * Handle command line completion for :highlight command.
 */
    ଶୂନ୍ୟ
set_context_in_highlight_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		expand_group = TRUE;

    // Default: expand group names
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 0;
    include_link = 2;
    include_default = 1;

    ଯଦି (*arg == NUL)
	ଫେରନ୍ତୁ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ଯଦି (*p == NUL)
	ଫେରନ୍ତୁ;

    // past "default" or group name
    include_default = 0;
    ଯଦି (STRNCMP("default", arg, p - arg) == 0)
    {
	arg = skipwhite(p);
	xp->xp_pattern = arg;
	p = skiptowhite(arg);
    }
    ଯଦି (*p == NUL)
	ଫେରନ୍ତୁ;

    // past group name
    include_link = 0;
    ଯଦି (arg[1] == 'i' && arg[0] == 'N')
    {
	highlight_list();
	expand_group = FALSE;
    }
    ଯଦି (STRNCMP("link", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	ଯଦି (*p != NUL)		// past first group name
	{
	    xp->xp_pattern = skipwhite(p);
	    p = skiptowhite(xp->xp_pattern);
	    include_none = 1;
	}
	expand_group = FALSE;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP("clear", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	expand_group = FALSE;
    }
    ଯଦି (*p != NUL)			// past group name(s)
    {
	ଯଦି (expand_group)
	{
	    // expansion will be done in expand_highlight_group()
	    xp->xp_context = EXPAND_HIGHLIGHT_GROUP;

	    expand_hi_synid = syn_namen2id(arg, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - arg));

	    ଯେତେବେଳେ (*p != NUL)
	    {
		arg = skipwhite(p);
		p = skiptowhite(arg);
	    }

	    p = vim_strchr(arg, '=');
	    ଯଦି (p == ଖାଲି)
	    {
		// Didn't find a key=<value> pattern
		xp->xp_pattern = arg;
		expand_hi_equal_col = -1;
		expand_hi_include_orig = FALSE;
	    }
	    ଅନ୍ୟଥା
	    {
		// Found key=<value> pattern, record the exact location
		expand_hi_equal_col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - xp->xp_ଧାଡ଼ି);

		// Only include the original value if the pattern is empty
		ଯଦି (*(p + 1) == NUL)
		    expand_hi_include_orig = TRUE;
		ଅନ୍ୟଥା
		    expand_hi_include_orig = FALSE;

		// Account for comma-separated values
		ଯଦି (STRNCMP(arg, "term=", 5) == 0 ||
			STRNCMP(arg, "cterm=", 6) == 0 ||
			STRNCMP(arg, "gui=", 4) == 0)
		{
		    char_u *comma = vim_strrchr(p + 1, ',');
		    ଯଦି (comma != ଖାଲି)
			p = comma;
		}
		xp->xp_pattern = p + 1;
	    }
	}
	ଅନ୍ୟଥା
	{
	    xp->xp_context = EXPAND_NOTHING;
	}
    }
}

/*
 * List highlighting matches in a nice way.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
highlight_list(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 10; --i >= 0; )
	highlight_list_two(i, HL_ATTR(HLF_D));
    ପାଇଁ (i = 40; --i >= 0; )
	highlight_list_two(99, 0);
}

    ସ୍ଥିର ଶୂନ୍ୟ
highlight_list_two(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    ସନ୍ଦେଶ_puts_attr(&("N \bI \b!  \b"[cnt / 11]), attr);
    ସନ୍ଦେଶ_clr_eos();
    out_flush();
    ui_delay(cnt == 99 ? 40L : (long)cnt * 50L, FALSE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_highlight_name(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ get_highlight_name_ext(xp, idx, TRUE);
}

/*
 * Obtain a highlight group name.
 * When "skip_cleared" is TRUE don't return a cleared entry.
 */
    char_u *
get_highlight_name_ext(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip_cleared)
{
    ଯଦି (idx < 0)
	ଫେରନ୍ତୁ ଖାଲି;

    // Items are never removed from the table, skip the ones that were
    // cleared.
    ଯଦି (skip_cleared && idx < highlight_ga.ga_len && HL_TABLE()[idx].sg_cleared)
	ଫେରନ୍ତୁ (char_u *)"";

    ଯଦି (idx == highlight_ga.ga_len && include_none != 0)
	ଫେରନ୍ତୁ (char_u *)"NONE";
    ଯଦି (idx == highlight_ga.ga_len + include_none && include_default != 0)
	ଫେରନ୍ତୁ (char_u *)"default";
    ଯଦି (idx == highlight_ga.ga_len + include_none + include_default
							 && include_link != 0)
	ଫେରନ୍ତୁ (char_u *)"link";
    ଯଦି (idx == highlight_ga.ga_len + include_none + include_default + 1
							 && include_link != 0)
	ଫେରନ୍ତୁ (char_u *)"clear";
    ଯଦି (idx >= highlight_ga.ga_len)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ HL_TABLE()[idx].sg_name;
}

    ସ୍ଥିର char_u *
get_highlight_attr_name(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx == 0)
    {
	// Fill with current value first
	ଯଦି (expand_hi_curvalue != ଖାଲି)
	    ଫେରନ୍ତୁ expand_hi_curvalue;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ (char_u*)"";
    }
    ଯଦି (idx < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(highlight_index_tab) + 1)
    {
	char_u *ମୂଲ୍ୟ = highlight_index_tab[idx-1]->ମୂଲ୍ୟ.string;
	ଯଦି (expand_hi_curvalue != ଖାଲି && STRCMP(expand_hi_curvalue, ମୂଲ୍ୟ) == 0)
	{
	    // Already returned the current value above, just skip.
	    ଫେରନ୍ତୁ (char_u*)"";
	}
	ଫେରନ୍ତୁ ମୂଲ୍ୟ;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର char_u *
get_highlight_cterm_color(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx == 0)
    {
	// Fill with current value first
	ଯଦି (expand_hi_curvalue != ଖାଲି)
	    ଫେରନ୍ତୁ expand_hi_curvalue;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ (char_u*)"";
    }
    // See highlight_set_cterm_color()
    ଅନ୍ୟଥା ଯଦି (idx == 1)
	ଫେରନ୍ତୁ (char_u*)"fg";
    ଅନ୍ୟଥା ଯଦି (idx == 2)
	ଫେରନ୍ତୁ (char_u*)"bg";
    ଯଦି (idx < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(color_name_tab) + 3)
    {
	char_u *ମୂଲ୍ୟ = color_name_tab[idx-3].ମୂଲ୍ୟ.string;
	ଫେରନ୍ତୁ ମୂଲ୍ୟ;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
    ସ୍ଥିର char_u *
get_highlight_gui_color(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx == 0)
    {
	// Fill with current value first
	ଯଦି (expand_hi_curvalue != ଖାଲି)
	    ଫେରନ୍ତୁ expand_hi_curvalue;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ (char_u*)"";
    }
    // See color_name2handle()
    ଅନ୍ୟଥା ଯଦି (idx == 1)
	ଫେରନ୍ତୁ (char_u*)"fg";
    ଅନ୍ୟଥା ଯଦି (idx == 2)
	ଫେରନ୍ତୁ (char_u*)"bg";
    ଅନ୍ୟଥା ଯଦି (idx == 3)
	ଫେରନ୍ତୁ (char_u*)"NONE";

    // Complete from v:colornames. Don't do platform specific names for now.
    typval_T *tv_result;
    char_u *colorname = dict_iterate_next(&expand_colornames_iter, &tv_result);
    ଯଦି (colorname != ଖାଲି)
    {
	// :hi command doesn't allow space, so don't suggest any malformed items
	ଯଦି (vim_strchr(colorname, ' ') != ଖାଲି)
	    ଫେରନ୍ତୁ (char_u*)"";

	ଯଦି (expand_hi_curvalue != ଖାଲି && STRICMP(expand_hi_curvalue, colorname) == 0)
	{
	    // Already returned the current value above, just skip.
	    ଫେରନ୍ତୁ (char_u*)"";
	}
    }
    ଫେରନ୍ତୁ colorname;
}
#endif

    ସ୍ଥିର char_u *
get_highlight_group_ଚାବି(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    // Note: Keep this in sync with do_highlight.
    ସ୍ଥିର ଅକ୍ଷର *(p_hi_group_ଚାବି_ମୂଲ୍ୟଗୁଡ଼ିକ[]) =
    {
	"term=",
	"start=",
	"stop=",
	"cterm=",
	"ctermfg=",
	"ctermbg=",
	"ctermul=",
	"ctermfont=",
#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

    ଯଦି (idx < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(p_hi_group_ଚାବି_ମୂଲ୍ୟଗୁଡ଼ିକ))
	ଫେରନ୍ତୁ (char_u*)p_hi_group_ଚାବି_ମୂଲ୍ୟଗୁଡ଼ିକ[idx];
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Command-line expansion for :hi {group-name} <args>...
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_highlight_group(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *ଅଙ୍କ_Matches)
{
    ଯଦି (expand_hi_equal_col != -1)
    {
	// List the values. First fill in the current value, then if possible colors
	// or attribute names.
	char_u	    *(*expandfunc)(expand_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା) = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    type = 0;
	hl_group_T  *sgp = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    iarg = 0;
	char_u	    *sarg = ଖାଲି;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    unsorted_ବସ୍ତୁଗୁଡ଼ିକ = -1; // don't sort by default

	ଯଦି (expand_hi_synid != 0)
	    sgp = &HL_TABLE()[expand_hi_synid - 1]; // index is ID minus one

	// Note: Keep this in sync with highlight_list_one().
	char_u	    *name_end = xp->xp_ଧାଡ଼ି + expand_hi_equal_col;
	ଯଦି (name_end - xp->xp_ଧାଡ଼ି >= 5
		&& STRNCMP(name_end - 5, " term", 5) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ଯଦି (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_term;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (name_end - xp->xp_ଧାଡ଼ି >= 6
		&& STRNCMP(name_end - 6, " cterm", 6) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ଯଦି (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_cterm;
	    }
	}
#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	ଅନ୍ୟଥା ଯଦି (name_end - xp->xp_ଧାଡ଼ି >= 4
		&& STRNCMP(name_end - 4, " gui", 4) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ଯଦି (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_gui;
	    }
	}
#endif
	ଅନ୍ୟଥା ଯଦି (name_end - xp->xp_ଧାଡ଼ି >= 8
		&& STRNCMP(name_end - 8, " ctermfg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ଯଦି (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_fg;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (name_end - xp->xp_ଧାଡ଼ି >= 8
		&& STRNCMP(name_end - 8, " ctermbg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ଯଦି (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_bg;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (name_end - xp->xp_ଧାଡ଼ି >= 8
		&& STRNCMP(name_end - 8, " ctermul", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ଯଦି (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_ul;
	    }
	}
#ଯଦି defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	ଅନ୍ୟଥା ଯଦି (name_end - xp->xp_ଧାଡ଼ି >= 6
		&& STRNCMP(name_end - 6, " guifg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ଯଦି (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_fg_name;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (name_end - xp->xp_ଧାଡ଼ି >= 6
		&& STRNCMP(name_end - 6, " guibg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ଯଦି (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_bg_name;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (name_end - xp->xp_ଧାଡ଼ି >= 6
		&& STRNCMP(name_end - 6, " guisp", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ଯଦି (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_sp_name;
	    }
	}
#endif

#ଯଦି defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	ଯଦି (expandfunc == get_highlight_gui_color)
	{
	    // Top 4 items are special, after that sort all the color names
	    unsorted_ବସ୍ତୁଗୁଡ଼ିକ = 4;

	    dict_T *colornames_table = get_vim_var_dict(VV_COLORNAMES);
	    typval_T colornames_val;
	    colornames_val.v_type = VAR_DICT;
	    colornames_val.vval.v_dict = colornames_table;
	    dict_iterate_start(&colornames_val, &expand_colornames_iter);
	}
#endif

	char_u	    buf[MAX_ATTR_LEN];

	expand_hi_curvalue = ଖାଲି;
	ଯଦି (expand_hi_include_orig)
	{
	    ଯଦି (((type == LIST_ATTR || type == LIST_INT) && iarg != 0) ||
		(type == LIST_STRING && sarg != ଖାଲି))
	    {
		// Retrieve the current value to go first in completion
		expand_hi_curvalue = highlight_arg_to_string(
			type, iarg, sarg, buf);
	    }
	}

	ଯଦି (expandfunc != ଖାଲି)
	{
	    ଫେରନ୍ତୁ ExpandGenericExt(
		    pat,
		    xp,
		    rmp,
		    matches,
		    ଅଙ୍କ_Matches,
		    expandfunc,
		    FALSE,
		    unsorted_ବସ୍ତୁଗୁଡ଼ିକ);
	}

	ଫେରନ୍ତୁ FAIL;
    }

    // List all the key names
    ଫେରନ୍ତୁ ExpandGenericExt(
	    pat,
	    xp,
	    rmp,
	    matches,
	    ଅଙ୍କ_Matches,
	    get_highlight_group_ଚାବି,
	    FALSE,
	    -1);
}

#ଯଦି defined(FEAT_GUI)
/*
 * Free all the highlight group fonts.
 * Used when quitting for systems which need it.
 */
    ଶୂନ୍ୟ
free_highlight_fonts(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx;

    ପାଇଁ (idx = 0; idx < highlight_ga.ga_len; ++idx)
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

#ଯଦି defined(FEAT_EVAL)
/*
 * Convert each of the highlight attribute bits (bold, standout, underline,
 * etc.) set in 'hlattr' into a separate boolean item in a Dictionary with
 * the attribute name as the key.
 */
    ସ୍ଥିର dict_T *
highlight_get_attr_dict(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hlattr)
{
    dict_T	*dict;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    dict = dict_alloc();
    ଯଦି (dict == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(highlight_index_tab); ++i)
    {
	ଯଦି (hlattr & highlight_index_tab[i]->ଚାବି)
	{
	    dict_add_bool(dict, (ଅକ୍ଷର *)highlight_index_tab[i]->ମୂଲ୍ୟ.string,
		    VVAL_TRUE);
	    // don't want "inverse"/"reverse"
	    hlattr &= ~highlight_index_tab[i]->ଚାବି;
	}
    }

    ଫେରନ୍ତୁ dict;
}

/*
 * Return the attributes of the highlight group at index 'hl_idx' as a
 * Dictionary. If 'resolve_link' is TRUE, then resolves the highlight group
 * links recursively.
 */
    ସ୍ଥିର dict_T *
highlight_get_info(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hl_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା resolve_link)
{
    dict_T	*dict;
    hl_group_T	*sgp;
    dict_T	*attr_dict = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hlgid;

    dict = dict_alloc();
    ଯଦି (dict == ଖାଲି)
	ଫେରନ୍ତୁ dict;

    sgp = &HL_TABLE()[hl_idx];
    // highlight group id is 1-based
    hlgid = hl_idx + 1;

    ଯଦି (dict_add_string(dict, "name", sgp->sg_name) == FAIL)
	ଯାଅ error;
    ଯଦି (dict_add_ସଂଖ୍ୟା(dict, "id", hlgid) == FAIL)
	ଯାଅ error;

    ଯଦି (resolve_link)
    {
	// resolve the highlight group link recursively
	ଯେତେବେଳେ (sgp->sg_link)
	{
	    hlgid = sgp->sg_link;
	    sgp = &HL_TABLE()[sgp->sg_link - 1];
	}
    }

    ଯଦି (sgp->sg_term != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_term);
	ଯଦି (attr_dict != ଖାଲି)
	{
	    ଯଦି (dict_add_dict(dict, "term", attr_dict) == FAIL)
		ଯାଅ error;
	    attr_dict = ଖାଲି;
	}
    }
    ଯଦି (sgp->sg_start != ଖାଲି)
	ଯଦି (dict_add_string(dict, "start", sgp->sg_start) == FAIL)
	    ଯାଅ error;
    ଯଦି (sgp->sg_stop != ଖାଲି)
	ଯଦି (dict_add_string(dict, "stop", sgp->sg_stop) == FAIL)
	    ଯାଅ error;
    ଯଦି (sgp->sg_cterm != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_cterm);
	ଯଦି (attr_dict != ଖାଲି)
	{
	    ଯଦି (dict_add_dict(dict, "cterm", attr_dict) == FAIL)
		ଯାଅ error;
	    attr_dict = ଖାଲି;
	}
    }
    ଯଦି (sgp->sg_cterm_fg != 0)
	ଯଦି (dict_add_string(dict, "ctermfg",
		    highlight_color(hlgid, (char_u *)"fg", 'c')) == FAIL)
	    ଯାଅ error;
    ଯଦି (sgp->sg_cterm_bg != 0)
	ଯଦି (dict_add_string(dict, "ctermbg",
			highlight_color(hlgid, (char_u *)"bg", 'c')) == FAIL)
	    ଯାଅ error;
    ଯଦି (sgp->sg_cterm_ul != 0)
	ଯଦି (dict_add_string(dict, "ctermul",
			highlight_color(hlgid, (char_u *)"ul", 'c')) == FAIL)
	    ଯାଅ error;
    ଯଦି (sgp->sg_cterm_font != 0)
	ଯଦି (dict_add_string(dict, "ctermfont",
			highlight_color(hlgid, (char_u *)"font", 'c')) == FAIL)
	    ଯାଅ error;
    ଯଦି (sgp->sg_gui != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_gui);
	ଯଦି (attr_dict != ଖାଲି)
	{
	    ଯଦି (dict_add_dict(dict, "gui", attr_dict) == FAIL)
		ଯାଅ error;
	    attr_dict = ଖାଲି;
	}
    }
    ଯଦି (sgp->sg_gui_fg_name != ଖାଲି)
	ଯଦି (dict_add_string(dict, "guifg",
			highlight_color(hlgid, (char_u *)"fg", 'g')) == FAIL)
	    ଯାଅ error;
    ଯଦି (sgp->sg_gui_bg_name != ଖାଲି)
	ଯଦି (dict_add_string(dict, "guibg",
			highlight_color(hlgid, (char_u *)"bg", 'g')) == FAIL)
	    ଯାଅ error;
    ଯଦି (sgp->sg_gui_sp_name != ଖାଲି)
	ଯଦି (dict_add_string(dict, "guisp",
			highlight_color(hlgid, (char_u *)"sp", 'g')) == FAIL)
	    ଯାଅ error;
# ifdef FEAT_GUI
    ଯଦି (sgp->sg_font_name != ଖାଲି)
	ଯଦି (dict_add_string(dict, "font", sgp->sg_font_name) == FAIL)
	    ଯାଅ error;
# endif
    ଯଦି (sgp->sg_link)
    {
	char_u	*link;

	link = HL_TABLE()[sgp->sg_link - 1].sg_name;
	ଯଦି (link != ଖାଲି && dict_add_string(dict, "linksto", link) == FAIL)
	    ଯାଅ error;

	ଯଦି (sgp->sg_deflink)
	    dict_add_bool(dict, "default", VVAL_TRUE);
    }
    ଯଦି (dict_len(dict) == 2)
	// If only 'name' is present, then the highlight group is cleared.
	dict_add_bool(dict, "cleared", VVAL_TRUE);

    ଫେରନ୍ତୁ dict;

error:
    dict_unref(attr_dict);
    dict_unref(dict);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * "hlget([name])" function
 * Return the attributes of a specific highlight group (if specified) or all
 * the highlight groups.
 */
    ଶୂନ୍ୟ
f_hlget(typval_T *argvars, typval_T *rettv)
{
    list_T	*list;
    dict_T	*dict;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*hlarg = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		resolve_link = FALSE;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (check_for_opt_string_arg(argvars, 0) == FAIL
	    || (argvars[0].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	// highlight group name supplied
	hlarg = tv_get_string_chk(&argvars[0]);
	ଯଦି (hlarg == ଖାଲି)
	    ଫେରନ୍ତୁ;

	ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = FALSE;

	    resolve_link = tv_get_bool_chk(&argvars[1], &error);
	    ଯଦି (error)
		ଫେରନ୍ତୁ;
	}
    }

    list = rettv->vval.v_list;
    ପାଇଁ (i = 0; i < highlight_ga.ga_len && !got_int; ++i)
    {
	ଯଦି (hlarg == ଖାଲି || STRICMP(hlarg, HL_TABLE()[i].sg_name) == 0)
	{
	    dict = highlight_get_info(i, resolve_link);
	    ଯଦି (dict != ଖାଲି && list_append_dict(list, dict) == FAIL)
		dict_unref(dict);
	}
    }
}

/*
 * Returns the string value at 'dict[key]'. Returns NULL, if 'key' is not in
 * 'dict' or the value is not a string type. If the value is not a string type
 * or is NULL, then 'error' is set to TRUE.
 */
    ସ୍ଥିର char_u *
hldict_get_string(dict_T *dict, char_u *ଚାବି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *error)
{
    dictitem_T	*di;

    *error = FALSE;
    di = dict_find(dict, ଚାବି, -1);
    ଯଦି (di == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (di->di_tv.v_type != VAR_STRING || di->di_tv.vval.v_string == ଖାଲି)
    {
	emsg(_(e_string_required));
	*error = TRUE;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ di->di_tv.vval.v_string;
}

/*
 * Convert the highlight attribute Dictionary at 'dict[key]' into a string
 * value in 'attr_str' of length 'len'. Returns FALSE if 'dict[key]' is not a
 * Dictionary or is NULL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
hldict_attr_to_str(
	dict_T	*dict,
	char_u	*ଚାବି,
	char_u	*attr_str,
	size_t	len)
{
    dictitem_T	*di;
    dict_T	*attrdict;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*p;

    attr_str[0] = NUL;
    di = dict_find(dict, ଚାବି, -1);
    ଯଦି (di == ଖାଲି)
	ଫେରନ୍ତୁ TRUE;

    ଯଦି (di->di_tv.v_type != VAR_DICT || di->di_tv.vval.v_dict == ଖାଲି)
    {
	emsg(_(e_dictionary_required));
	ଫେରନ୍ତୁ FALSE;
    }

    attrdict = di->di_tv.vval.v_dict;

    // If the attribute dict is empty, then return NONE to clear the attributes
    ଯଦି (dict_len(attrdict) == 0)
    {
	vim_strcat(attr_str, (char_u *)"NONE", len);
	ଫେରନ୍ତୁ TRUE;
    }

    p = attr_str;
    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(highlight_tab); ++i)
    {
	ଯଦି (dict_get_bool(attrdict, (ଅକ୍ଷର *)highlight_tab[i].ମୂଲ୍ୟ.string,
		    VVAL_FALSE) == VVAL_TRUE)
	{
	    ଯଦି (p != attr_str && (size_t)(p - attr_str + 2) < len)
		STRCPY(p, (char_u *)",");
	    ଯଦି (p - attr_str + highlight_tab[i].ମୂଲ୍ୟ.length + 1 < len)
	    {
		STRCPY(p, highlight_tab[i].ମୂଲ୍ୟ.string);
		p += highlight_tab[i].ମୂଲ୍ୟ.length;
	    }
	}
    }

    ଫେରନ୍ତୁ TRUE;
}

// Temporary buffer used to store the command string produced by hlset().
// IObuff cannot be used for this as the error messages produced by hlset()
// internally use IObuff.
# define	HLSETBUFSZ  512
ସ୍ଥିର char_u hlsetBuf[HLSETBUFSZ + 1];

/*
 * Add the highlight attribute "attr" of length "attrlen" and "value" at
 * "dptr", which points into "hlsetBuf".
 * Returns the updated pointer.
 */
    ସ୍ଥିର char_u *
add_attr_and_ମୂଲ୍ୟ(char_u *dptr, char_u *attr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attrlen, char_u *ମୂଲ୍ୟ)
{
    size_t	vallen;

    // Do nothing if the value is not specified or is empty
    ଯଦି (ମୂଲ୍ୟ == ଖାଲି || *ମୂଲ୍ୟ == NUL)
	ଫେରନ୍ତୁ dptr;

    vallen = STRLEN(ମୂଲ୍ୟ);
    // When the value contains a space and the attribute has an "=" (i.e. it
    // is a key=value pair), surround the value with single quotes so that
    // do_highlight() can parse it correctly.
    ଯଦି (vim_strchr(ମୂଲ୍ୟ, ' ') != ଖାଲି && vim_strchr(attr, '=') != ଖାଲି)
    {
	ଯଦି (dptr + attrlen + vallen + 3 < hlsetBuf + HLSETBUFSZ)
	{
	    STRCPY(dptr, attr);
	    dptr += attrlen;
	    *dptr++ = '\'';
	    STRCPY(dptr, ମୂଲ୍ୟ);
	    dptr += vallen;
	    *dptr++ = '\'';
	    *dptr = NUL;
	}
    }
    ଅନ୍ୟଥା ଯଦି (dptr + attrlen + vallen + 1 < hlsetBuf + HLSETBUFSZ)
    {
	STRCPY(dptr, attr);
	dptr += attrlen;
	STRCPY(dptr, ମୂଲ୍ୟ);
	dptr += vallen;
    }

    ଫେରନ୍ତୁ dptr;
}

/*
 * Add or update a highlight group using 'dict' items. Returns TRUE if
 * successfully updated the highlight group.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
hlg_add_or_update(dict_T *dict)
{
    char_u	*name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dodefault = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		done = FALSE;
    char_u	*p;

    name = hldict_get_string(dict, (char_u *)"name", &error);
    ଯଦି (name == ଖାଲି || *name == NUL || error)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (dict_get_bool(dict, "force", VVAL_FALSE) == VVAL_TRUE)
	forceit = TRUE;

    ଯଦି (dict_get_bool(dict, "default", VVAL_FALSE) == VVAL_TRUE)
	dodefault = TRUE;

    ଯଦି (dict_has_ଚାବି(dict, "cleared"))
    {
	varnumber_T	cleared;

	// clear a highlight group
	cleared = dict_get_bool(dict, "cleared", FALSE);
	ଯଦି (cleared == TRUE)
	{
	    vim_snprintf((ଅକ୍ଷର *)hlsetBuf, HLSETBUFSZ, "clear %s", name);
	    do_highlight(hlsetBuf, forceit, FALSE);
	    done = TRUE;
	}
    }

    ଯଦି (dict_has_ଚାବି(dict, "linksto"))
    {
	char_u	*linksto;

	// link highlight groups
	linksto = hldict_get_string(dict, (char_u *)"linksto", &error);
	ଯଦି (linksto == ଖାଲି || *linksto == NUL || error)
	    ଫେରନ୍ତୁ FALSE;

	vim_snprintf((ଅକ୍ଷର *)hlsetBuf, HLSETBUFSZ, "%slink %s %s",
				dodefault ? "default " : "", name, linksto);
	do_highlight(hlsetBuf, forceit, FALSE);

	done = TRUE;
    }

    // If 'cleared' or 'linksto' are specified, then don't process the other
    // attributes.
    ଯଦି (done)
	ଫେରନ୍ତୁ TRUE;

    start = hldict_get_string(dict, (char_u *)"start", &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ FALSE;

    stop = hldict_get_string(dict, (char_u *)"stop", &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (!hldict_attr_to_str(dict, (char_u *)"term", term_attr,
							    ଆକାର(term_attr)))
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (!hldict_attr_to_str(dict, (char_u *)"cterm", cterm_attr,
							   ଆକାର(cterm_attr)))
	ଫେରନ୍ତୁ FALSE;

    ctermfg = hldict_get_string(dict, (char_u *)"ctermfg", &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ FALSE;

    ctermbg = hldict_get_string(dict, (char_u *)"ctermbg", &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ FALSE;

    ctermul = hldict_get_string(dict, (char_u *)"ctermul", &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ FALSE;

    ctermfont = hldict_get_string(dict, (char_u *)"ctermfont", &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (!hldict_attr_to_str(dict, (char_u *)"gui", gui_attr, ଆକାର(gui_attr)))
	ଫେରନ୍ତୁ FALSE;

    guifg = hldict_get_string(dict, (char_u *)"guifg", &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ FALSE;

    guibg = hldict_get_string(dict, (char_u *)"guibg", &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ FALSE;

    guisp = hldict_get_string(dict, (char_u *)"guisp", &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ FALSE;

# ifdef FEAT_GUI
    font = hldict_get_string(dict, (char_u *)"font", &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ FALSE;
# endif

    // If none of the attributes are specified, then do nothing.
    ଯଦି (term_attr[0] == NUL && start == ଖାଲି && stop == ଖାଲି
	    && cterm_attr[0] == NUL && ctermfg == ଖାଲି && ctermbg == ଖାଲି
	    && ctermul == ଖାଲି && ctermfont == ଖାଲି && gui_attr[0] == NUL
# ifdef FEAT_GUI
	    && font == ଖାଲି
# endif
	    && guifg == ଖାଲି && guibg == ଖାଲି && guisp == ଖାଲି
	    )
	ଫେରନ୍ତୁ TRUE;

    hlsetBuf[0] = NUL;
    p = hlsetBuf;
    ଯଦି (dodefault)
	p = add_attr_and_ମୂଲ୍ୟ(p, (char_u *)"default", 7, (char_u *)" ");
    p = add_attr_and_ମୂଲ୍ୟ(p, (char_u *)"", 0, name);
    p = add_attr_and_ମୂଲ୍ୟ(p, (char_u *)" term=", 6, term_attr);
    p = add_attr_and_ମୂଲ୍ୟ(p, (char_u *)" start=", 7, start);
    p = add_attr_and_ମୂଲ୍ୟ(p, (char_u *)" stop=", 6, stop);
    p = add_attr_and_ମୂଲ୍ୟ(p, (char_u *)" cterm=", 7, cterm_attr);
    p = add_attr_and_ମୂଲ୍ୟ(p, (char_u *)" ctermfg=", 9, ctermfg);
    p = add_attr_and_ମୂଲ୍ୟ(p, (char_u *)" ctermbg=", 9, ctermbg);
    p = add_attr_and_ମୂଲ୍ୟ(p, (char_u *)" ctermul=", 9, ctermul);
    p = add_attr_and_ମୂଲ୍ୟ(p, (char_u *)" ctermfont=", 11, ctermfont);
    p = add_attr_and_ମୂଲ୍ୟ(p, (char_u *)" gui=", 5, gui_attr);
# ifdef FEAT_GUI
    p = add_attr_and_ମୂଲ୍ୟ(p, (char_u *)" font=", 6, font);
# endif
    p = add_attr_and_ମୂଲ୍ୟ(p, (char_u *)" guifg=", 7, guifg);
    p = add_attr_and_ମୂଲ୍ୟ(p, (char_u *)" guibg=", 7, guibg);
    (ଶୂନ୍ୟ)add_attr_and_ମୂଲ୍ୟ(p, (char_u *)" guisp=", 7, guisp);

    do_highlight(hlsetBuf, forceit, FALSE);

    ଫେରନ୍ତୁ TRUE;
}

/*
 * "hlset([{highlight_attr}])" function
 * Add or modify highlight groups
 */
    ଶୂନ୍ୟ
f_hlset(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    dict_T	*dict;

    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (check_for_list_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(argvars->vval.v_list, li)
    {
	ଯଦି (li->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_dictionary_required));
	    ଫେରନ୍ତୁ;
	}

	dict = li->li_tv.vval.v_dict;
	ଯଦି (!hlg_add_or_update(dict))
	    ଫେରନ୍ତୁ;
    }

    rettv->vval.v_ସଂଖ୍ୟା = 0;
}
#endif

/*
 * If "old" is in the override stack, then update it to "hl_new". Does not free
 * "old".
 */
    ଶୂନ୍ୟ
update_highlight_overrides(hl_override_T *old, hl_override_T *hl_new, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newlen)
{
    ଯଦି (old == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପାଇଁ (hl_overrides_T *set = overrides; set != ଖାଲି; set = set->next)
    {
	ଯଦି (set->arr == old)
	{
	    set->arr = hl_new;
	    set->len = newlen;
	}
    }
}

/*
 * If "arr" is in the highlight overrides list, then mark it as invalid.
 */
    ଶୂନ୍ୟ
remove_highlight_overrides(hl_override_T *arr)
{
    ଯଦି (arr == ଖାଲି || overrides == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପାଇଁ (hl_overrides_T *set = overrides; set != ଖାଲି; set = set->next)
    {
	ଯଦି (set->arr == arr)
	{
	    set->arr = ଖାଲି;
	    ଭାଙ୍ଗ;
	}
    }
}

/*
 * Update highlight_attr[] array. If "update_ids" is true, then update
 * highlight_ids[] array instead.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_highlight_attr(hl_override_T *arr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, bool update_ids)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < len; i++)
    {
	hl_override_T	*override = arr + i;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hlf = -1, attr;

	ଯଦି (override->from <= 0)
	    // Directly map to highlight_attr
	    hlf = -override->from;
	ଅନ୍ୟଥା
	{
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < HLF_ଗଣନା; k++)
		ଯଦି (override->from == highlight_ids[k])
		{
		    hlf = k;
		    ଭାଙ୍ଗ;
		}

	    ଯଦି (hlf == -1)
		ଜାରି;
	}

	ଯଦି (update_ids)
	{
	    ଯଦି (override->to <= 0)
	    {
		ଯଦି (hlf == -override->to)
		    highlight_ids[hlf] = -1;
		ଅନ୍ୟଥା
		    highlight_ids[hlf] = highlight_ids[-override->to];
	    }
	    ଅନ୍ୟଥା
		highlight_ids[hlf] = override->to;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (override->to <= 0)
		// Directly use highlight_attr[]
		attr = highlight_attr[-override->to];
	    ଅନ୍ୟଥା
		attr = syn_id2attr(override->to);

	    highlight_attr[hlf] = attr;
	}
    }
}

/*
 * Set the current highlight override to "arr". Returns true if successful
 */
    bool
ଠେଲ_highlight_overrides(hl_override_T *arr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    // Don't want to do anything if "arr" is already the current override. If
    // "arr" is NULL (but overrides->arr is not), then still push an override,
    // but "->arr" will just be NULL so any previous overrides are cleared.
    ଯଦି (overrides != ଖାଲି && overrides->arr == arr)
	ଫେରନ୍ତୁ false;

    hl_overrides_T *set;

    set = ALLOC_ONE(hl_overrides_T);
    ଯଦି (set == ଖାଲି)
	ଫେରନ୍ତୁ false;

    set->arr = arr;
    set->len = len;

    // Push to front of list
    set->next = overrides;
    overrides = set;

    // Save current highlight_attr[], so it can be restored later after this
    // override is popped.
    memcpy(set->attr, highlight_attr, ଆକାର(highlight_attr));

    // Reset highlight_attr[] to default values, as we don't want changes from
    // other overrides.
    memcpy(highlight_attr, highlight_attr_raw, ଆକାର(highlight_attr));

    // Update highlight_attr[] array
    ଯଦି (arr != ଖାଲି)
	set_highlight_attr(arr, len, false);

    ଫେରନ୍ତୁ true;
}

/*
 * Pop the current highlight override (if any) from the stack. This should
 * always be paired with a *successful* push.
 */
    ଶୂନ୍ୟ
କାଢ଼_highlight_overrides(ଶୂନ୍ୟ)
{
    hl_overrides_T *set = overrides;

    ଯଦି (overrides == ଖାଲି)
	ଫେରନ୍ତୁ;

    overrides = set->next;

    // Set highlight_attr[] to state before override was pushed.
    memcpy(highlight_attr, set->attr, ଆକାର(highlight_attr));
    vim_free(set);
}

/*
 * Parse the 'winhighlight' option and return array. Returns NULL on failure or
 * if empty option. If failure, then errmsg is set.
 */
    ସ୍ଥିର hl_override_T *
parse_winhighlight(char_u *opt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *len, ଅକ୍ଷର **errmsg)
{
    char_u	    *p = opt;
    hl_override_T   *arr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ଅଙ୍କ = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    n_colons = 0;

    ଯଦି (*p == NUL)
	ଫେରନ୍ତୁ ଖାଲି;

    // Get number of overrides first so we can allocate array
    ଯେତେବେଳେ ((p = vim_strchr(p, ',')) != ଖାଲି)
    {
	p++;
	ଅଙ୍କ++;
    }
    p = opt;
    // Check if number of ':' matches number of ','
    ଯେତେବେଳେ ((p = vim_strchr(p, ':')) != ଖାଲି)
    {
	p++;
	n_colons++;
    }

    ଯଦି (ଅଙ୍କ != n_colons)
    {
	*errmsg = e_invalid_argument;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    arr = ALLOC_MULT(hl_override_T, ଅଙ୍କ);
    ଯଦି (arr == ଖାଲି)
    {
	*errmsg = e_out_of_memory;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    p = opt;

    ଯେତେବେଳେ (true)
    {
	hl_override_T *override = arr + i++;
	char_u	*fromname = p, *toname;
	char_u	*tmp;
	char_u	**names[2] = {&fromname, &toname};
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	fromlen, tolen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*lens[2] = {&fromlen, &tolen};
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	fromid, toid;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*ids[2] = {&fromid, &toid};

	p = vim_strchr(p, ':');

	ଯଦି (p == ଖାଲି)
	    ଯାଅ fail;

	fromlen = p - fromname; // Get hl for "from"
	ଯଦି (fromlen == 0)
	    ଯାଅ fail;
	p++; // Skip colon ':'
	ଯଦି (*p == NUL)
	    ଯାଅ fail;

	toname = p;
	tmp = vim_strchr(p, ',');

	// Get hl for "to", must check for no trailing comma in case last
	// element.
	ଯଦି (tmp == ଖାଲି)
	    tolen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);
	ଅନ୍ୟଥା
	{
	    tolen = tmp - toname;
	    p = ++tmp;
	}
	ଯଦି (tolen == 0)
	    ଯାଅ fail;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < 2; k++)
	{
	    char_u  *name = *names[k];
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    nlen = *lens[k];

	    ଯଦି (*name == '!')
	    {
		// If starts with "!", then it maps directly to a 'highlight'
		// occasion.
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା hlf;

		ଯଦି (nlen != 2)
		    ଯାଅ fail;

		ପାଇଁ (hlf = 0; hlf < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)HLF_ଗଣନା; ++hlf)
		    ଯଦି (hl_flags[hlf] == name[1])
			ଭାଙ୍ଗ;

		ଯଦି (hlf >= HLF_ଗଣନା)
		    ଯାଅ fail;

		*ids[k] = -hlf;
	    }
	    ଅନ୍ୟଥା
	    {
		// Otherwise get the highlight group id.
		ଯଦି (syn_check_group(name, nlen) == 0)
		    ଯାଅ fail;

		*ids[k] = syn_namen2id(name, nlen);
		ଯଦି (*ids[k] == 0)
		    ଯାଅ fail;

		// Always map "Normal" group to HLF_WIN
		ଯଦି (ids[k] == &fromid &&
			STRCMP(HL_TABLE()[*ids[k] - 1].sg_name_u, "NORMAL") == 0)
		    *ids[k] = -HLF_WIN;
	    }
	}

	// If fromid == toid, leave it be, this is so that we know HLF_WIN has
	// been set (even to itself e.g. "Normal"), and therefore know to use
	// its attr instead.
	override->from = fromid;
	override->to = toid;

	ଯଦି (tmp == ଖାଲି)
	    ଭାଙ୍ଗ;
    }

    *len = ଅଙ୍କ;
    ଫେରନ୍ତୁ arr;
fail:
    vim_free(arr);
    *errmsg = e_invalid_argument;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Update w_hl for "wp" using given option value. Returns error
 * message on failure, otherwise NULL.
 */
    ଅକ୍ଷର *
update_winhighlight(win_T *wp, char_u *opt)
{
    ଅକ୍ଷର	    *err = ଖାଲି;
    hl_override_T   *arr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ଅଙ୍କ = 1;

    arr = parse_winhighlight(opt, &ଅଙ୍କ, &err);

    ଯଦି (arr == ଖାଲି && err != ଖାଲି)
	ଫେରନ୍ତୁ err;

    ଯଦି (arr == ଖାଲି && wp->w_hl == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    update_highlight_overrides(wp->w_hl, arr, ଅଙ୍କ);

    vim_free(wp->w_hl);
    wp->w_hl = arr;
    wp->w_hl_len = ଅଙ୍କ;

    wp->w_hlfwin_id = hlf_get_id(wp, HLF_WIN);

#ifdef FEAT_TERMINAL
    term_update_hlfwin(wp);
#endif

#ifdef FEAT_TERMINAL
    // May be NULL (such as in after_copy_winopt())
    ଯଦି (wp->w_buffer != ଖାଲି)
    {
	// Make sure terminal highlighting is updated
	bool ret = ଠେଲ_highlight_overrides(wp->w_hl, wp->w_hl_len);

	ଯଦି (wp->w_buffer->b_term != ଖାଲି)
	    term_init_default_colors(wp->w_buffer->b_term);
	ଯଦି (ret)
	    କାଢ଼_highlight_overrides();
    }
#endif

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Get id that the hlf attr is using in window "wp", otherwise zero. Note that
 * -1 may be returned if the hlf was set to itself.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
hlf_get_id(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hlf)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା id;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev[HLF_ଗଣନା];

    ଯଦି (wp->w_hl == ଖାଲି)
	ଫେରନ୍ତୁ highlight_ids[hlf];

    memcpy(prev, highlight_ids, ଆକାର(highlight_ids));
    set_highlight_attr(wp->w_hl, wp->w_hl_len, true);
    id = highlight_ids[hlf];
    memcpy(highlight_ids, prev, ଆକାର(highlight_ids));

    ଫେରନ୍ତୁ id;
}

/*
 * Update 'winhighlight' using a 'wincolor' style option value. Returns error
 * message on failure otherwise NULL. Does not update the actual 'wincolor'
 * value, but does update the 'winhighlight' value.
 */
    ଅକ୍ଷର *
update_wincolor(win_T *wp, char_u *opt)
{
    char_u  *str = *opt == NUL ? "" : alloc(ଆକାର("!(:") + STRLEN(opt));
    ଅକ୍ଷର    *errmsg;

    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ e_out_of_memory;

    ଯଦି (*opt != NUL)
	sprintf((ଅକ୍ଷର *)str, "!(:%s", opt);

    errmsg = update_winhighlight(wp, str);

    ଯଦି (errmsg == ଖାଲି)
	set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		str, OPT_FREE|OPT_LOCAL, 0);

    ଯଦି (*opt != NUL)
	ମୁକ୍ତ(str);
    ଫେରନ୍ତୁ errmsg;
}
