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

#अन्तर्भाव "vim.h"

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
स्थिर keyvalue_T highlight_tab[] = {
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
स्थिर keyvalue_T *highlight_index_tab[] = {
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

गणना {
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
स्थिर keyvalue_T color_name_tab[] = {
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
प्रकारव्याख्या संरचना
{
    char_u	*sg_name;	// highlight group name
    char_u	*sg_name_u;	// uppercase of sg_name
    पूर्णांक		sg_cleared;	// "hi clear" was used
// for normal terminals
    पूर्णांक		sg_term;	// "term=" highlighting attributes
    char_u	*sg_start;	// terminal string for start highl
    char_u	*sg_stop;	// terminal string for stop highl
    पूर्णांक		sg_term_attr;	// Screen attr for term mode
// for color terminals
    पूर्णांक		sg_cterm;	// "cterm=" highlighting attr
    पूर्णांक		sg_cterm_bold;	// bold attr was set for light color
    पूर्णांक		sg_cterm_fg;	// terminal fg color number + 1
    पूर्णांक		sg_cterm_bg;	// terminal bg color number + 1
    पूर्णांक		sg_cterm_ul;	// terminal ul color number + 1
    पूर्णांक		sg_cterm_attr;	// Screen attr for color term mode
    पूर्णांक		sg_cterm_font;	// terminal alternative font (0 for normal)
// for when using the GUI
#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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
    पूर्णांक		sg_gui_attr;    // Screen attr for GUI mode
#endif
#जर defined(FEAT_GUI) || defined(FEAT_EVAL)
// Store the sp color name for the GUI or synIDattr()
    पूर्णांक		sg_gui;		// "gui=" highlighting attributes
    char_u	*sg_gui_fg_name;// GUI foreground color name
    char_u	*sg_gui_bg_name;// GUI background color name
    char_u	*sg_gui_sp_name;// GUI special color name
#endif
    पूर्णांक		sg_link;	// link to this highlight group ID
    पूर्णांक		sg_deflink;	// default link; restored in highlight_clear()
    पूर्णांक		sg_set;		// combination of SG_* flags
#ifdef FEAT_EVAL
    sctx_T	sg_deflink_sctx;  // script where the default link was set
    sctx_T	sg_script_ctx;	// script in which the group was last set
#endif
} hl_group_T;

प्रकारव्याख्या संरचना hl_overrides_S hl_overrides_T;
संरचना hl_overrides_S
{
    hl_override_T   *arr;   // May be NULL if "arr" was freed or no highlight
			    // overrides (all values set to default)
    पूर्णांक		    len;
    hl_overrides_T  *next;  // Used to handle recursive calls

    पूर्णांक attr[HLF_मोजणी];    // highlight_attr[] before being updated.
};

स्थिर hl_overrides_T *overrides = रिक्त;

// Synced with highlight_attr, each is the highlight group id for each attr. If
// an element is 0, then there is no attr for it. If an element is -1, then it
// means it was set to itself.
स्थिर पूर्णांक highlight_ids[HLF_मोजणी];

// Same as highlight_attr[] but is not modified by highlight group overrides
स्थिर पूर्णांक highlight_attr_raw[HLF_मोजणी];

स्थिर पूर्णांक hl_flags[HLF_मोजणी] = HL_FLAGS;

// highlight groups for 'highlight' option
स्थिर garray_T highlight_ga;
#define HL_TABLE()	((hl_group_T *)((highlight_ga.ga_data)))

// Wrapper struct for hashtable entries: stores the highlight group ID alongside
// the uppercase name used as a hash key.  Uses the same offsetof pattern as
// signgroup_T / HI2SG.
प्रकारव्याख्या संरचना {
    पूर्णांक		hn_id;		// highlight group ID (1-based)
    char_u	hn_किल्ली[1];	// uppercase name (stretchable array)
} hlname_T;

#define HLNAME_किल्ली_OFF	दुरत्व(hlname_T, hn_किल्ली)
#define HI2HLNAME(hi)	((hlname_T *)((hi)->hi_किल्ली - HLNAME_किल्ली_OFF))

// hashtable for quick highlight group name lookup
स्थिर hashtab_T highlight_ht;
स्थिर bool	 highlight_ht_inited = false;

/*
 * An attribute number is the index in attr_table plus ATTR_OFF.
 */
#define ATTR_OFF (HL_ALL + 1)

स्थिर शून्य syn_unadd_group(शून्य);
स्थिर शून्य set_hl_attr(पूर्णांक idx);
स्थिर शून्य highlight_list_one(पूर्णांक id);
स्थिर पूर्णांक highlight_list_arg(पूर्णांक id, पूर्णांक didh, पूर्णांक type, पूर्णांक iarg, char_u *sarg, अक्षर *name);
स्थिर पूर्णांक syn_add_group(char_u *name);
स्थिर पूर्णांक syn_name2id_len(char_u *name, पूर्णांक len);
स्थिर पूर्णांक hl_has_settings(पूर्णांक idx, पूर्णांक check_link);
स्थिर शून्य highlight_clear(पूर्णांक idx);

#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
स्थिर शून्य gui_do_one_color(पूर्णांक idx, पूर्णांक do_menu, पूर्णांक do_tooltip);
#endif
#ifdef FEAT_GUI
स्थिर पूर्णांक  set_group_colors(char_u *name, guicolor_T *fgp, guicolor_T *bgp, पूर्णांक do_menu, पूर्णांक use_norm, पूर्णांक do_tooltip);
स्थिर शून्य hl_do_font(पूर्णांक idx, char_u *arg, पूर्णांक do_normal, पूर्णांक do_menu, पूर्णांक do_tooltip, पूर्णांक free_font);
#endif

/*
 * The default highlight groups.  These are compiled-in for fast startup and
 * they still work when the runtime files can't be found.
 * When making changes here, also change runtime/colors/default.vim!
 * The #ifdefs are needed to reduce the amount of static data.  Helps to make
 * the 16 bit DOS (museum) version compile.
 */
#जर defined(FEAT_GUI) || defined(FEAT_EVAL)
# define CENT(a, b) b
#अन्यथा
# define CENT(a, b) a
#endif
स्थिर अक्षर *(highlight_init_both[]) = {
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
    रिक्त
};

// Default colors only used with a light background.
स्थिर अक्षर *(highlight_init_light[]) = {
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
    रिक्त
};

// Default colors only used with a dark background.
स्थिर अक्षर *(highlight_init_dark[]) = {
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
    रिक्त
};

#जर defined(FEAT_SYN_HL)
/*
 * Returns the number of highlight groups.
 */
    पूर्णांक
highlight_अंक_groups(शून्य)
{
    परत highlight_ga.ga_len;
}

/*
 * Returns the name of a highlight group.
 */
    char_u *
highlight_group_name(पूर्णांक id)
{
    परत HL_TABLE()[id].sg_name;
}

/*
 * Returns the ID of the link to a highlight group.
 */
    पूर्णांक
highlight_link_id(पूर्णांक id)
{
    परत HL_TABLE()[id].sg_link;
}
#endif

स्थिर शून्य resolve_fallback_fg_to_rgb(शून्य);
स्थिर शून्य resolve_fallback_bg_to_rgb(शून्य);

    शून्य
init_highlight(
    पूर्णांक		both,	    // include groups where 'bg' doesn't matter
    पूर्णांक		reset)	    // clear group first
{
    पूर्णांक		i;
    अक्षर	**pp;
    स्थिर पूर्णांक	had_both = FALSE;
#ifdef FEAT_EVAL
    char_u	*p;

    // Try finding the color scheme file.  Used when a color file was loaded
    // and 'background' or 't_Co' is changed.
    p = get_var_मूल्य((char_u *)"g:colors_name");
    जर (p != रिक्त)
    {
	// The value of g:colors_name could be freed when sourcing the script,
	// making "p" invalid, so copy it.
	char_u *copy_p = vim_strsave(p);
	पूर्णांक    r;

	जर (copy_p != रिक्त)
	{
	    r = load_colors(copy_p);
	    vim_free(copy_p);
	    जर (r == OK)
		परत;
	}
    }

#endif

    // Didn't use a color file, use the compiled-in colors.
    जर (both)
    {
	had_both = TRUE;
	pp = highlight_init_both;
	साठी (i = 0; pp[i] != रिक्त; ++i)
	    do_highlight((char_u *)pp[i], reset, TRUE);
    }
    अन्यथा जर (!had_both)
	// Don't do anything before the call with both == TRUE from main().
	// Not everything has been setup then, and that call will overrule
	// everything anyway.
	परत;

    जर (*p_bg == 'l')
	pp = highlight_init_light;
    अन्यथा
	pp = highlight_init_dark;
    साठी (i = 0; pp[i] != रिक्त; ++i)
	do_highlight((char_u *)pp[i], reset, TRUE);

    // Reverse looks ugly, but grey may not work for less than 8 colors.  Thus
    // let it depend on the number of colors available.
    जर (t_colors < 8)
	do_highlight((char_u *)"Visual term=reverse cterm=reverse ctermbg=NONE ctermfg=NONE",
		FALSE, TRUE);
    // With 8 colors brown is equal to yellow, need to use black for Search fg
    // to avoid Statement highlighted text disappears.
    // Clear the attributes, needed when changing the t_Co value.
    जर (t_colors <= 8)
    {
	जर (*p_bg == 'l')
	    do_highlight((char_u *)"Search ctermfg=black", FALSE, TRUE);
    }

#ifdef FEAT_SYN_HL
    // If syntax highlighting is enabled load the highlighting for it.
    जर (get_var_मूल्य((char_u *)"g:syntax_on") != रिक्त)
    {
	स्थिर पूर्णांक	recursive = 0;

	जर (recursive >= 5)
	    emsg(_(e_recursive_loop_loading_syncolor_vim));
	अन्यथा
	{
	    ++recursive;
	    (शून्य)source_runtime((char_u *)"syntax/syncolor.vim", DIP_ALL);
	    --recursive;
	}
    }
#endif
    resolve_fallback_fg_to_rgb();
    resolve_fallback_bg_to_rgb();
}

#जर defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
/*
 * Load a default color list. Intended to support legacy color names but allows
 * the user to override the color values. Only loaded once.
 */
    स्थिर शून्य
load_default_colors_lists(शून्य)
{
    // Lacking a default color list isn't the end of the world but it is likely
    // an inconvenience so users should know when it is missing.
    जर (source_runtime((char_u *)"colors/lists/default.vim", DIP_ALL) != OK)
	संदेश("failed to load colors/lists/default.vim");
}
#endif

/*
 * Load color file "name".
 * Return OK for success, FAIL for failure.
 */
    पूर्णांक
load_colors(char_u *name)
{
    char_u	*buf;
    पूर्णांक		retval = FAIL;
    स्थिर पूर्णांक	recursive = FALSE;

    // When being called recursively, this is probably because setting
    // 'background' caused the highlighting to be reloaded.  This means it is
    // working, thus we should return OK.
    जर (recursive)
	परत OK;

    recursive = TRUE;
    buf = alloc(STRLEN(name) + 12);
    जर (buf != रिक्त)
    {
#जर defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	load_default_colors_lists();
#endif
	apply_autocmds(EVENT_COLORSCHEMEPRE, name,
					       curbuf->b_fname, FALSE, curbuf);
	प्र_लिखो((अक्षर *)buf, "colors/%s.vim", name);
	retval = source_runtime(buf, DIP_START + DIP_OPT);
	vim_free(buf);
	जर (retval == OK)
	    apply_autocmds(EVENT_COLORSCHEME, name, curbuf->b_fname,
								FALSE, curbuf);
    }
    recursive = FALSE;

    परत retval;
}

स्थिर पूर्णांक color_numbers_16[28] = {0, 1, 2, 3,
				 4, 5, 6, 6,
				 7, 7, 7, 7,
				 8, 8,
				 9, 9, 10, 10,
				 11, 11, 12, 12, 13,
				 13, 14, 14, 15, -1};
// for xterm with 88 colors...
स्थिर पूर्णांक color_numbers_88[28] = {0, 4, 2, 6,
				 1, 5, 32, 72,
				 84, 84, 7, 7,
				 82, 82,
				 12, 43, 10, 61,
				 14, 63, 9, 74, 13,
				 75, 11, 78, 15, -1};
// for xterm with 256 colors...
स्थिर पूर्णांक color_numbers_256[28] = {0, 4, 2, 6,
				 1, 5, 130, 3,
				 248, 248, 7, 7,
				 242, 242,
				 12, 81, 10, 121,
				 14, 159, 9, 224, 13,
				 225, 11, 229, 15, -1};
// for terminals with less than 16 colors...
स्थिर पूर्णांक color_numbers_8[28] = {0, 4, 2, 6,
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
    स्थिर पूर्णांक
lookup_color(पूर्णांक idx, पूर्णांक foreground, पूर्णांक *boldp)
{
    पूर्णांक		color = color_numbers_16[idx];
    char_u	*p;

    // Use the _16 table to check if it's a valid color name.
    जर (color < 0)
	परत -1;

    जर (t_colors == 8)
    {
	// t_Co is 8: use the 8 colors table
#जर defined(__QNXNTO__)
	// On qnx, the 8 & 16 color arrays are the same
	जर (STRNCMP(T_NAME, "qansi", 5) == 0)
	    color = color_numbers_16[idx];
	अन्यथा
#endif
	    color = color_numbers_8[idx];
	जर (foreground)
	{
	    // set/reset bold attribute to get light foreground
	    // colors (on some terminals, e.g. "linux")
	    जर (color & 8)
		*boldp = TRUE;
	    अन्यथा
		*boldp = FALSE;
	}
	color &= 7;	// truncate to 8 colors
    }
    अन्यथा जर (t_colors == 16 || t_colors == 88
					   || t_colors >= 256)
    {
	// Guess: if the termcap entry ends in 'm', it is
	// probably an xterm-like terminal.  Use the changed
	// order for colors.
	जर (*T_CAF != NUL)
	    p = T_CAF;
	अन्यथा
	    p = T_CSF;
	जर (*p != NUL && (t_colors > 256
			      || *(p + STRLEN(p) - 1) == 'm'))
	{
	    जर (t_colors == 88)
		color = color_numbers_88[idx];
	    अन्यथा जर (t_colors >= 256)
		color = color_numbers_256[idx];
	    अन्यथा
		color = color_numbers_8[idx];
	}
#ifdef FEAT_TERMRESPONSE
	जर (t_colors >= 256 && color == 15 && is_mac_terminal)
	    // Terminal.app has a bug: 15 is light grey. Use white
	    // from the color cube instead.
	    color = 231;
#endif
    }
    परत color;
}

/*
 * Link highlight group 'from_hg' to 'to_hg'.
 * 'dodefault' is set to TRUE for ":highlight default link".
 * 'forceit' is set to TRUE for ":highlight! link"
 * 'init' is set to TRUE when initializing all the highlight groups.
 */
    स्थिर शून्य
highlight_group_link(
	char_u	*from_hg,
	पूर्णांक	from_len,
	char_u	*to_hg,
	पूर्णांक	to_len,
	पूर्णांक	dodefault,
	पूर्णांक	forceit,
	पूर्णांक	init)
{
    पूर्णांक		from_id;
    पूर्णांक		to_id;
    hl_group_T	*hlgroup = रिक्त;

    from_id = syn_check_group(from_hg, from_len);
    जर (STRNCMP(to_hg, "NONE", 4) == 0)
	to_id = 0;
    अन्यथा
	to_id = syn_check_group(to_hg, to_len);

    जर (from_id > 0)
    {
	hlgroup = &HL_TABLE()[from_id - 1];
	जर (dodefault && (forceit || hlgroup->sg_deflink == 0))
	{
	    hlgroup->sg_deflink = to_id;
#ifdef FEAT_EVAL
	    hlgroup->sg_deflink_sctx = current_sctx;
	    hlgroup->sg_deflink_sctx.sc_lnum += SOURCING_LNUM;
#endif
	}
    }

    जर (from_id > 0 && (!init || hlgroup->sg_set == 0))
    {
	// Don't allow a link when there already is some highlighting
	// for the group, unless '!' is used
	जर (to_id > 0 && !forceit && !init
		&& hl_has_settings(from_id - 1, dodefault))
	{
	    जर (SOURCING_NAME == रिक्त && !dodefault)
		emsg(_(e_group_has_settings_highlight_link_ignored));
	}
	अन्यथा जर (hlgroup->sg_link != to_id
#ifdef FEAT_EVAL
		|| hlgroup->sg_script_ctx.sc_sid != current_sctx.sc_sid
#endif
		|| hlgroup->sg_cleared)
	{
	    जर (!init)
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
    स्थिर शून्य
highlight_reset_all(शून्य)
{
    पूर्णांक		idx;

#ifdef FEAT_GUI
    // First, we do not destroy the old values, but allocate the new
    // ones and update the display. THEN we destroy the old values.
    // If we destroy the old values first, then the old values
    // (such as GuiFont's or GuiFontset's) will still be displayed but
    // invalid because they were free'd.
    जर (gui.in_use)
    {
# ifdef FEAT_BEVAL_TIP
	gui_init_tooltip_font();
# endif
# जर defined(FEAT_MENU) && defined(FEAT_GUI_MOTIF)
	gui_init_menu_font();
# endif
    }
# जर defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_X11)
    gui_mch_def_colors();
# endif
# ifdef FEAT_GUI_MSWIN
    जर (gui.in_use)
	gui_mch_set_titlebar_colors();
# endif
# ifdef FEAT_GUI_X11
#  ifdef FEAT_MENU

    // This only needs to be done when there is no Menu highlight
    // group defined by default, which IS currently the case.
    gui_mch_new_menu_colors();
#  endif
    जर (gui.in_use)
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
    साठी (idx = 0; idx < highlight_ga.ga_len; ++idx)
	highlight_clear(idx);
    init_highlight(TRUE, TRUE);
#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    जर (USE_24BIT)
	highlight_gui_started();
    अन्यथा
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
    स्थिर पूर्णांक
highlight_set_termgui_attr(पूर्णांक idx, char_u *किल्ली, char_u *arg, पूर्णांक init)
{
    पूर्णांक		attr;
    माप_प्रकार	off;
    keyvalue_T	target;
    keyvalue_T	*entry;

    attr = 0;
    off = 0;
    target.किल्ली = 0;
    target.मूल्य.length = 0;	// not used, see cmp_keyvalue_value_ni()
    पर्यंत (arg[off] != NUL)
    {
	target.मूल्य.string = arg + off;
	entry = (keyvalue_T *)द्वा_खोज(&target, &highlight_tab,
		ARRAY_LENGTH(highlight_tab), आकार(highlight_tab[0]),
		cmp_keyvalue_मूल्य_ni);
	जर (entry == रिक्त)
	{
	    semsg(_(e_illegal_मूल्य_str), arg);
	    परत FALSE;
	}

	attr |= entry->किल्ली;
	off += entry->मूल्य.length;
	जर (arg[off] == ',')		// another one follows
	    ++off;
    }
    जर (*किल्ली == 'T')
    {
	जर (!init || !(HL_TABLE()[idx].sg_set & SG_TERM))
	{
	    जर (!init)
		HL_TABLE()[idx].sg_set |= SG_TERM;
	    HL_TABLE()[idx].sg_term = attr;
	}
    }
    अन्यथा जर (*किल्ली == 'C')
    {
	जर (!init || !(HL_TABLE()[idx].sg_set & SG_CTERM))
	{
	    जर (!init)
		HL_TABLE()[idx].sg_set |= SG_CTERM;
	    HL_TABLE()[idx].sg_cterm = attr;
	    HL_TABLE()[idx].sg_cterm_bold = FALSE;
	}
    }
#जर defined(FEAT_GUI) || defined(FEAT_EVAL)
    अन्यथा
    {
	जर (!init || !(HL_TABLE()[idx].sg_set & SG_GUI))
	{
	    जर (!init)
		HL_TABLE()[idx].sg_set |= SG_GUI;
	    HL_TABLE()[idx].sg_gui = attr;
	}
    }
#endif

    परत TRUE;
}

#ifdef FEAT_GUI
/*
 * Set the font for the highlight group at 'idx'.
 * 'arg' is the font name.
 * Returns TRUE if the font is changed.
 */
    स्थिर पूर्णांक
highlight_set_font(
	पूर्णांक	idx,
	char_u	*arg,
	पूर्णांक	is_normal_group,
	पूर्णांक	is_menu_group,
	पूर्णांक	is_tooltip_group)
{
    पूर्णांक		did_change = FALSE;

    // in non-GUI fonts are simply ignored
    जर (HL_TABLE()[idx].sg_font_name != रिक्त
	    && STRCMP(HL_TABLE()[idx].sg_font_name, arg) == 0)
    {
	// Font name didn't change, ignore.
    }
    अन्यथा जर (!gui.shell_created)
    {
	// GUI not started yet, always accept the name.
	vim_free(HL_TABLE()[idx].sg_font_name);
	HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	did_change = TRUE;
    }
    अन्यथा
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
	जर (HL_TABLE()[idx].sg_fontset != NOFONTSET)
	{
	    // New fontset was accepted. Free the old one, if there
	    // was one.
	    gui_mch_free_fontset(temp_sg_fontset);
	    did_change = TRUE;
	}
	अन्यथा
	    HL_TABLE()[idx].sg_fontset = temp_sg_fontset;
# endif
	जर (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    // New font was accepted. Free the old one, if there was
	    // one.
	    gui_mch_free_font(temp_sg_font);
	    did_change = TRUE;
	}
	अन्यथा
	    HL_TABLE()[idx].sg_font = temp_sg_font;
	// Update sg_font_name once here, after both font and fontset results
	// are known.
	जर (did_change)
	{
	    vim_free(HL_TABLE()[idx].sg_font_name);
	    HL_TABLE()[idx].sg_font_name = vim_strsave(arg);
	}
    }

    परत did_change;
}
#endif

/*
 * Set the cterm foreground color for the Normal highlight group to "color" and
 * the bold attribute to "bold".
 */
    स्थिर शून्य
hl_set_ctermfg_normal_group(पूर्णांक color, पूर्णांक bold)
{
    cterm_normal_fg_color = color + 1;
    cterm_normal_fg_bold = bold;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    जर (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	जर (termcap_active && color >= 0)
	    term_fg_color(color);
    }
}

/*
 * Set the cterm foreground color for the highlight group at 'idx' to 'color'.
 */
    स्थिर शून्य
highlight_set_ctermfg(पूर्णांक idx, पूर्णांक color, पूर्णांक is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_fg = color + 1;
    जर (is_normal_group)
	hl_set_ctermfg_normal_group(color,
					(HL_TABLE()[idx].sg_cterm & HL_BOLD));
}

/*
 * Set the cterm background color for the Normal highlight group to "color".
 */
    स्थिर शून्य
hl_set_ctermbg_normal_group(पूर्णांक color)
{
    cterm_normal_bg_color = color + 1;
#ifdef FEAT_GUI
    // Don't mess with 'background' if the GUI is used.
    जर (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	जर (color >= 0)
	{
	    पूर्णांक dark = -1;

	    जर (termcap_active)
		term_bg_color(color);
	    जर (t_colors < 16)
		dark = (color == 0 || color == 4);
	    // Limit the heuristic to the standard 16 colors
	    अन्यथा जर (color < 16)
		dark = (color < 7 || color == 8);
	    // Set the 'background' option if the value is
	    // wrong.
	    जर (dark != -1
		    && dark != (*p_bg == 'd')
		    && !option_was_set((char_u *)"bg"))
	    {
		set_option_मूल्य_give_err((char_u *)"bg",
			0L, (char_u *)(dark ? "dark" : "light"), 0);
		reset_option_was_set((char_u *)"bg");
	    }
	}
    }
}

/*
 * Set the cterm background color for the highlight group at 'idx' to 'color'.
 */
    स्थिर शून्य
highlight_set_ctermbg(पूर्णांक idx, पूर्णांक color, पूर्णांक is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_bg = color + 1;
    जर (is_normal_group)
	hl_set_ctermbg_normal_group(color);
}

/*
 * Set the cterm underline color for the Normal highlight group to "color".
 */
    स्थिर शून्य
hl_set_ctermul_normal_group(पूर्णांक color)
{
    cterm_normal_ul_color = color + 1;
#ifdef FEAT_GUI
    // Don't do this if the GUI is used.
    जर (!gui.in_use && !gui.starting)
#endif
    {
	set_must_redraw(UPD_CLEAR);
	जर (termcap_active && color >= 0)
	    term_ul_color(color);
    }
}

/*
 * Set the cterm underline color for the highlight group at 'idx' to 'color'.
 */
    स्थिर शून्य
highlight_set_ctermul(पूर्णांक idx, पूर्णांक color, पूर्णांक is_normal_group)
{
    HL_TABLE()[idx].sg_cterm_ul = color + 1;
    जर (is_normal_group)
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
    स्थिर पूर्णांक
highlight_set_cterm_font(
	पूर्णांक	idx,
	char_u	*arg,
	पूर्णांक	init)
{
    पूर्णांक		font;

    जर (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	परत FALSE;

    जर (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    जर (VIM_ISDIGIT(*arg))
	font = म_से_प((अक्षर *)arg);
    अन्यथा जर (STRICMP(arg, "NONE") == 0)
	font = -1;
    अन्यथा
	परत FALSE;

    HL_TABLE()[idx].sg_cterm_font = font + 1;
    परत TRUE;
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
    स्थिर पूर्णांक
highlight_set_cterm_color(
	पूर्णांक	idx,
	char_u	*किल्ली,
	char_u	*किल्ली_start,
	char_u	*arg,
	पूर्णांक	is_normal_group,
	पूर्णांक	init)
{
    पूर्णांक		color;

    जर (init && (HL_TABLE()[idx].sg_set & SG_CTERM))
	परत FALSE;

    जर (!init)
	HL_TABLE()[idx].sg_set |= SG_CTERM;

    // When setting the foreground color, and previously the "bold"
    // flag was set for a light color, reset it now
    जर (किल्ली[5] == 'F' && HL_TABLE()[idx].sg_cterm_bold)
    {
	HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
	HL_TABLE()[idx].sg_cterm_bold = FALSE;
    }

    जर (VIM_ISDIGIT(*arg))
	color = म_से_प((अक्षर *)arg);
    अन्यथा जर (STRICMP(arg, "fg") == 0)
    {
	जर (cterm_normal_fg_color)
	    color = cterm_normal_fg_color - 1;
	अन्यथा
	{
	    emsg(_(e_fg_color_unknown));
	    परत FALSE;
	}
    }
    अन्यथा जर (STRICMP(arg, "bg") == 0)
    {
	जर (cterm_normal_bg_color > 0)
	    color = cterm_normal_bg_color - 1;
	अन्यथा
	{
	    emsg(_(e_bg_color_unknown));
	    परत FALSE;
	}
    }
    अन्यथा जर (STRICMP(arg, "ul") == 0)
    {
	जर (cterm_normal_ul_color > 0)
	    color = cterm_normal_ul_color - 1;
	अन्यथा
	{
	    emsg(_(e_ul_color_unknown));
	    परत FALSE;
	}
    }
    अन्यथा
    {
	पूर्णांक bold = MAYBE;
	keyvalue_T target;
	keyvalue_T *entry;

	target.किल्ली = 0;
	target.मूल्य.string = arg;
	target.मूल्य.length = 0;	// not used, see cmp_keyvalue_value_i()
	entry = (keyvalue_T *)द्वा_खोज(&target, &color_name_tab,
		ARRAY_LENGTH(color_name_tab), आकार(color_name_tab[0]),
		cmp_keyvalue_मूल्य_i);
	जर (entry == रिक्त)
	{
	    semsg(_(e_color_name_or_संख्या_not_recognized_str), किल्ली_start);
	    परत FALSE;
	}

	color = lookup_color(entry->किल्ली, किल्ली[5] == 'F', &bold);

	// set/reset bold attribute to get light foreground
	// colors (on some terminals, e.g. "linux")
	जर (bold == TRUE)
	{
	    HL_TABLE()[idx].sg_cterm |= HL_BOLD;
	    HL_TABLE()[idx].sg_cterm_bold = TRUE;
	}
	अन्यथा जर (bold == FALSE)
	    HL_TABLE()[idx].sg_cterm &= ~HL_BOLD;
    }

    // Add one to the argument, to avoid zero.  Zero is used for
    // "NONE", then "color" is -1.
    जर (किल्ली[5] == 'F')
	highlight_set_ctermfg(idx, color, is_normal_group);
    अन्यथा जर (किल्ली[5] == 'B')
	highlight_set_ctermbg(idx, color, is_normal_group);
    अन्यथा // ctermul
	highlight_set_ctermul(idx, color, is_normal_group);

    परत TRUE;
}

#जर defined(FEAT_GUI) || defined(FEAT_EVAL)
/*
 * Set the GUI foreground color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    स्थिर पूर्णांक
highlight_set_guifg(
	पूर्णांक	idx,
	char_u	*arg,
	पूर्णांक	is_menu_group UNUSED,
	पूर्णांक	is_scrollbar_group UNUSED,
	पूर्णांक	is_tooltip_group UNUSED,
	पूर्णांक	is_titlebar_group UNUSED,
	पूर्णांक	is_titlebarnc_group UNUSED,
	पूर्णांक	*do_colors UNUSED,
	पूर्णांक	init)
{
# जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    दीर्घ	i;
# endif
    char_u	**namep;
    पूर्णांक		did_change = FALSE;

    जर (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	परत FALSE;

    namep = &HL_TABLE()[idx].sg_gui_fg_name;
    जर (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guifg colors are only used when recognized
    i = color_name2handle(arg);
    जर (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_fg = i;
# endif
	जर (*namep == रिक्त || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    जर (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    अन्यथा
		*namep = रिक्त;
	    did_change = TRUE;
	}
# जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	जर (is_titlebar_group && gui.title_fg_pixel != i)
	{
	    gui.title_fg_pixel = i;
	    *do_colors = TRUE;
	}
	जर (is_titlebarnc_group && gui.titlenc_fg_pixel != i)
	{
	    gui.titlenc_fg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	जर (is_menu_group && gui.menu_fg_pixel != i)
	{
	    gui.menu_fg_pixel = i;
	    *do_colors = TRUE;
	}
	जर (is_scrollbar_group && gui.scroll_fg_pixel != i)
	{
	    gui.scroll_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	जर (is_tooltip_group && gui.tooltip_fg_pixel != i)
	{
	    gui.tooltip_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    परत did_change;
}

/*
 * Set the GUI background color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    स्थिर पूर्णांक
highlight_set_guibg(
	पूर्णांक	idx,
	char_u	*arg,
	पूर्णांक	is_menu_group UNUSED,
	पूर्णांक	is_scrollbar_group UNUSED,
	पूर्णांक	is_tooltip_group UNUSED,
	पूर्णांक	is_titlebar_group UNUSED,
	पूर्णांक	is_titlebarnc_group UNUSED,
	पूर्णांक	*do_colors UNUSED,
	पूर्णांक	init)
{
# जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    पूर्णांक		i;
# endif
    char_u	**namep;
    पूर्णांक		did_change = FALSE;

    जर (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	परत FALSE;

    namep = &HL_TABLE()[idx].sg_gui_bg_name;
    जर (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guibg colors are only used when recognized
    i = color_name2handle(arg);
    जर (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_bg = i;
# endif
	जर (*namep == रिक्त || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    जर (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    अन्यथा
		*namep = रिक्त;
	    did_change = TRUE;
	}
# जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	जर (is_titlebar_group && gui.title_bg_pixel != i)
	{
	    gui.title_bg_pixel = i;
	    *do_colors = TRUE;
	}
	जर (is_titlebarnc_group && gui.titlenc_bg_pixel != i)
	{
	    gui.titlenc_bg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	जर (is_menu_group && gui.menu_bg_pixel != i)
	{
	    gui.menu_bg_pixel = i;
	    *do_colors = TRUE;
	}
	जर (is_scrollbar_group && gui.scroll_bg_pixel != i)
	{
	    gui.scroll_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	जर (is_tooltip_group && gui.tooltip_bg_pixel != i)
	{
	    gui.tooltip_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    परत did_change;
}

/*
 * Set the GUI undercurl/strikethrough color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    स्थिर पूर्णांक
highlight_set_guisp(पूर्णांक idx, char_u *arg, पूर्णांक init)
{
# जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    पूर्णांक		i;
# endif
    पूर्णांक		did_change = FALSE;
    char_u	**namep;

    जर (init && (HL_TABLE()[idx].sg_set & SG_GUI))
	परत FALSE;

    namep = &HL_TABLE()[idx].sg_gui_sp_name;
    जर (!init)
	HL_TABLE()[idx].sg_set |= SG_GUI;

# जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guisp colors are only used when recognized
    i = color_name2handle(arg);
    जर (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24BIT)
    {
	HL_TABLE()[idx].sg_gui_sp = i;
# endif
	जर (*namep == रिक्त || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    जर (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    अन्यथा
		*namep = रिक्त;
	    did_change = TRUE;
	}
# जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    }
# endif

    परत did_change;
}
#endif

/*
 * Set the start/stop terminal codes for a highlight group.
 * Returns TRUE if the terminal code is set.
 */
    स्थिर पूर्णांक
highlight_set_startstop_termcode(पूर्णांक idx, char_u *किल्ली, char_u *arg, पूर्णांक init)
{
    पूर्णांक		off;
    char_u	buf[100];
    पूर्णांक		len;
    char_u	*tname;
    char_u	*p;

    जर (!init)
	HL_TABLE()[idx].sg_set |= SG_TERM;

    // The "start" and "stop"  arguments can be a literal escape
    // sequence, or a comma separated list of terminal codes.
    जर (STRNCMP(arg, "t_", 2) == 0)
    {
	off = 0;
	buf[0] = 0;
	पर्यंत (arg[off] != NUL)
	{
	    // Isolate one termcap name
	    साठी (len = 0; arg[off + len] &&
		    arg[off + len] != ','; ++len)
		;
	    tname = vim_strnsave(arg + off, len);
	    जर (tname == रिक्त)		// out of memory
		परत FALSE;
	    // lookup the escape sequence for the item
	    p = get_term_code(tname);
	    vim_free(tname);
	    जर (p == रिक्त)	    // ignore non-existing things
		p = (char_u *)"";

	    // Append it to the already found stuff
	    जर ((पूर्णांक)(STRLEN(buf) + STRLEN(p)) >= 99)
	    {
		semsg(_(e_terminal_code_too_long_str), arg);
		परत FALSE;
	    }
	    STRCAT(buf, p);

	    // Advance to the next item
	    off += len;
	    जर (arg[off] == ',')	    // another one follows
		++off;
	}
    }
    अन्यथा
    {
	// Copy characters from arg[] to buf[], translating <> codes.
	साठी (p = arg, off = 0; off < 100 - 6 && *p; )
	{
	    len = trans_special(&p, buf + off, FSK_SIMPLIFY, FALSE, रिक्त);
	    जर (len > 0)	    // recognized special char
		off += len;
	    अन्यथा		    // copy as normal char
		buf[off++] = *p++;
	}
	buf[off] = NUL;
    }

    जर (STRCMP(buf, "NONE") == 0)	// resetting the value
	p = रिक्त;
    अन्यथा
	p = vim_strsave(buf);
    जर (किल्ली[2] == 'A')
    {
	vim_free(HL_TABLE()[idx].sg_start);
	HL_TABLE()[idx].sg_start = p;
    }
    अन्यथा
    {
	vim_free(HL_TABLE()[idx].sg_stop);
	HL_TABLE()[idx].sg_stop = p;
    }
    परत TRUE;
}

/*
 * Handle the ":highlight .." command.
 * When using ":hi clear" this is called recursively for each group with
 * "forceit" and "init" both TRUE.
 */
    शून्य
do_highlight(
    char_u	*ओळ,
    पूर्णांक		forceit,
    पूर्णांक		init)	    // TRUE when called for initializing
{
    char_u	*name_end;
    char_u	*linep;
    char_u	*किल्ली_start;
    char_u	*arg_start;
    char_u	*किल्ली = रिक्त, *arg = रिक्त;
    दीर्घ	i;
    पूर्णांक		id;
    पूर्णांक		idx;
    hl_group_T	item_before;
    पूर्णांक		did_change = FALSE;
    पूर्णांक		dodefault = FALSE;
    पूर्णांक		doclear = FALSE;
    पूर्णांक		dolink = FALSE;
    पूर्णांक		error = FALSE;
    पूर्णांक		is_normal_group = FALSE;	// "Normal" group
#ifdef FEAT_GUI_MSWIN
    पूर्णांक		is_titlebar_group = FALSE;	// "TitleBar" group
    पूर्णांक		is_titlebarnc_group = FALSE;	// "TitleBarNC" group
#अन्यथा
# define is_titlebar_group 0
# define is_titlebarnc_group 0
#endif
#ifdef FEAT_GUI_X11
    पूर्णांक		is_menu_group = FALSE;		// "Menu" group
    पूर्णांक		is_scrollbar_group = FALSE;	// "Scrollbar" group
    पूर्णांक		is_tooltip_group = FALSE;	// "Tooltip" group
#अन्यथा
# define is_menu_group 0
# define is_tooltip_group 0
# define is_scrollbar_group 0
#endif
#जर defined(FEAT_GUI) || defined(FEAT_EVAL)
    पूर्णांक		do_colors = FALSE;		// need to update colors?
#endif
#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    पूर्णांक		did_highlight_changed = FALSE;
#endif

    // If no argument, list current highlighting.
    जर (!init && ends_excmd2(ओळ - 1, ओळ))
    {
	साठी (i = 1; i <= highlight_ga.ga_len && !got_int; ++i)
	    // TODO: only call when the group has attributes set
	    highlight_list_one((पूर्णांक)i);
	परत;
    }

    // Isolate the name.
    name_end = skiptowhite(ओळ);
    linep = skipwhite(name_end);

    // Check for "default" argument.
    जर (STRNCMP(ओळ, "default", name_end - ओळ) == 0)
    {
	dodefault = TRUE;
	ओळ = linep;
	name_end = skiptowhite(ओळ);
	linep = skipwhite(name_end);
    }

    // Check for "clear" or "link" argument.
    जर (STRNCMP(ओळ, "clear", name_end - ओळ) == 0)
	doclear = TRUE;
    जर (STRNCMP(ओळ, "link", name_end - ओळ) == 0)
	dolink = TRUE;

    // ":highlight {group-name}": list highlighting for one group.
    जर (!doclear && !dolink && ends_excmd2(ओळ, linep))
    {
	id = syn_namen2id(ओळ, (पूर्णांक)(name_end - ओळ));
	जर (id == 0)
	    semsg(_(e_highlight_group_name_not_found_str), ओळ);
	अन्यथा
	    highlight_list_one(id);
	परत;
    }

    // Handle ":highlight link {from} {to}" command.
    जर (dolink)
    {
	char_u	    *from_start = linep;
	char_u	    *from_end;
	पूर्णांक	    from_len;
	char_u	    *to_start;
	char_u	    *to_end;
	पूर्णांक	    to_len;

	from_end = skiptowhite(from_start);
	to_start = skipwhite(from_end);
	to_end	 = skiptowhite(to_start);

	जर (ends_excmd2(ओळ, from_start) || ends_excmd2(ओळ, to_start))
	{
	    semsg(_(e_not_enough_arguments_highlight_link_str), from_start);
	    परत;
	}

	जर (!ends_excmd2(ओळ, skipwhite(to_end)))
	{
	    semsg(_(e_too_many_arguments_highlight_link_str), from_start);
	    परत;
	}

	from_len = (पूर्णांक)(from_end - from_start);
	to_len = (पूर्णांक)(to_end - to_start);
	highlight_group_link(from_start, from_len, to_start, to_len,
						dodefault, forceit, init);
	परत;
    }

    जर (doclear)
    {
	// ":highlight clear [group]" command.
	जर (ends_excmd2(ओळ, linep))
	{
	    // ":highlight clear" without group name
	    highlight_reset_all();
	    परत;
	}
	ओळ = linep;
	name_end = skiptowhite(ओळ);
	linep = skipwhite(name_end);
    }

    // Find the group name in the table.  If it does not exist yet, add it.
    id = syn_check_group(ओळ, (पूर्णांक)(name_end - ओळ));
    जर (id == 0)	// failed (out of memory)
	परत;
    idx = id - 1;	// index is ID minus one

    // Return if "default" was used and the group already has settings.
    जर (dodefault && hl_has_settings(idx, TRUE))
	परत;

    // Make a copy so we can check if any attribute actually changed.
    item_before = HL_TABLE()[idx];

    जर (STRCMP(HL_TABLE()[idx].sg_name_u, "NORMAL") == 0)
	is_normal_group = TRUE;
#ifdef FEAT_GUI_MSWIN
    अन्यथा जर (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBAR") == 0)
	is_titlebar_group = TRUE;
    अन्यथा जर (STRCMP(HL_TABLE()[idx].sg_name_u, "TITLEBARNC") == 0)
	is_titlebarnc_group = TRUE;
#endif
#ifdef FEAT_GUI_X11
    अन्यथा जर (STRCMP(HL_TABLE()[idx].sg_name_u, "MENU") == 0)
	is_menu_group = TRUE;
    अन्यथा जर (STRCMP(HL_TABLE()[idx].sg_name_u, "SCROLLBAR") == 0)
	is_scrollbar_group = TRUE;
    अन्यथा जर (STRCMP(HL_TABLE()[idx].sg_name_u, "TOOLTIP") == 0)
	is_tooltip_group = TRUE;
#endif

    // Clear the highlighting for ":hi clear {group}" and ":hi clear".
    जर (doclear || (forceit && init))
    {
	highlight_clear(idx);
	जर (!doclear)
	    HL_TABLE()[idx].sg_set = 0;
    }

    जर (!doclear)
	पर्यंत (!ends_excmd2(ओळ, linep))
	{
	    किल्ली_start = linep;
	    जर (*linep == '=')
	    {
		semsg(_(e_unexpected_equal_sign_str), किल्ली_start);
		error = TRUE;
		थांबा;
	    }

	    // Note: Keep this in sync with get_highlight_group_key.

	    // Isolate the key ("term", "ctermfg", "ctermbg", "font", "guifg"
	    // or "guibg").
	    पर्यंत (*linep && !VIM_ISWHITE(*linep) && *linep != '=')
		++linep;
	    vim_free(किल्ली);
	    किल्ली = vim_strnsave_up(किल्ली_start, linep - किल्ली_start);
	    जर (किल्ली == रिक्त)
	    {
		error = TRUE;
		थांबा;
	    }
	    linep = skipwhite(linep);

	    जर (STRCMP(किल्ली, "NONE") == 0)
	    {
		जर (!init || HL_TABLE()[idx].sg_set == 0)
		{
		    जर (!init)
			HL_TABLE()[idx].sg_set |= SG_TERM+SG_CTERM+SG_GUI;
		    highlight_clear(idx);
		}
		सुरूठेवा;
	    }

	    // Check for the equal sign.
	    जर (*linep != '=')
	    {
		semsg(_(e_missing_equal_sign_str_2), किल्ली_start);
		error = TRUE;
		थांबा;
	    }
	    ++linep;

	    // Isolate the argument.
	    linep = skipwhite(linep);
	    जर (*linep == '\'')		// guifg='color name'
	    {
		arg_start = ++linep;
		linep = vim_strchr(linep, '\'');
		जर (linep == रिक्त)
		{
		    semsg(_(e_invalid_argument_str), किल्ली_start);
		    error = TRUE;
		    थांबा;
		}
	    }
	    अन्यथा
	    {
		arg_start = linep;
		linep = skiptowhite(linep);
	    }
	    जर (linep == arg_start)
	    {
		semsg(_(e_missing_argument_str), किल्ली_start);
		error = TRUE;
		थांबा;
	    }
	    vim_free(arg);
	    arg = vim_strnsave(arg_start, linep - arg_start);
	    जर (arg == रिक्त)
	    {
		error = TRUE;
		थांबा;
	    }
	    जर (*linep == '\'')
		++linep;

	    // Store the argument.
	    जर (STRCMP(किल्ली, "TERM") == 0
		    || STRCMP(किल्ली, "CTERM") == 0
		    || STRCMP(किल्ली, "GUI") == 0)
	    {
		जर (!highlight_set_termgui_attr(idx, किल्ली, arg, init))
		{
		    error = TRUE;
		    थांबा;
		}
	    }
	    अन्यथा जर (STRCMP(किल्ली, "FONT") == 0)
	    {
		// in non-GUI fonts are simply ignored
#ifdef FEAT_GUI
		जर (highlight_set_font(idx, arg, is_normal_group,
					      is_menu_group, is_tooltip_group))
		    did_change = TRUE;
#endif
	    }
	    अन्यथा जर (STRCMP(किल्ली, "CTERMFG") == 0
		    || STRCMP(किल्ली, "CTERMBG") == 0
		    || STRCMP(किल्ली, "CTERMUL") == 0)
	    {
		जर (!highlight_set_cterm_color(idx, किल्ली, किल्ली_start, arg,
							is_normal_group, init))
		{
		    error = TRUE;
		    थांबा;
		}
	    }
	    अन्यथा जर (STRCMP(किल्ली, "CTERMFONT") == 0)
	    {
		जर (!highlight_set_cterm_font(idx, arg, init))
		{
		    error = TRUE;
		    थांबा;
		}
	    }
	    अन्यथा जर (STRCMP(किल्ली, "GUIFG") == 0)
	    {
#जर defined(FEAT_GUI) || defined(FEAT_EVAL)
		जर (highlight_set_guifg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    अन्यथा जर (STRCMP(किल्ली, "GUIBG") == 0)
	    {
#जर defined(FEAT_GUI) || defined(FEAT_EVAL)
		जर (highlight_set_guibg(idx, arg, is_menu_group,
			    is_scrollbar_group, is_tooltip_group,
			    is_titlebar_group, is_titlebarnc_group,
			    &do_colors, init))
		    did_change = TRUE;
#endif
	    }
	    अन्यथा जर (STRCMP(किल्ली, "GUISP") == 0)
	    {
#जर defined(FEAT_GUI) || defined(FEAT_EVAL)
		जर (highlight_set_guisp(idx, arg, init))
		    did_change = TRUE;
#endif
	    }
	    अन्यथा जर (STRCMP(किल्ली, "START") == 0 || STRCMP(किल्ली, "STOP") == 0)
	    {
		जर (!highlight_set_startstop_termcode(idx, किल्ली, arg, init))
		{
		    error = TRUE;
		    थांबा;
		}
	    }
	    अन्यथा
	    {
		semsg(_(e_illegal_argument_str_3), किल्ली_start);
		error = TRUE;
		थांबा;
	    }
	    HL_TABLE()[idx].sg_cleared = FALSE;

	    // When highlighting has been given for a group, don't link it.
	    जर (!init || !(HL_TABLE()[idx].sg_set & SG_LINK))
		HL_TABLE()[idx].sg_link = 0;

	    // Continue with next argument.
	    linep = skipwhite(linep);
	}

    // If there is an error, and it's a new entry, remove it from the table.
    जर (error && idx == highlight_ga.ga_len)
	syn_unadd_group();
    अन्यथा
    {
	जर (is_normal_group)
	{
	    HL_TABLE()[idx].sg_term_attr = 0;
	    HL_TABLE()[idx].sg_cterm_attr = 0;
#ifdef FEAT_GUI
	    HL_TABLE()[idx].sg_gui_attr = 0;
	    // Need to update all groups, because they might be using "bg"
	    // and/or "fg", which have been changed now.
#endif
#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    जर (USE_24BIT)
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
	अन्यथा जर (is_titlebar_group)
	{
	    जर (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
	अन्यथा जर (is_titlebarnc_group)
	{
	    जर (gui.in_use && do_colors)
		gui_mch_set_titlebar_colors();
	}
#endif
#ifdef FEAT_GUI_X11
# ifdef FEAT_MENU
	अन्यथा जर (is_menu_group)
	{
	    जर (gui.in_use && do_colors)
		gui_mch_new_menu_colors();
	}
# endif
	अन्यथा जर (is_scrollbar_group)
	{
	    जर (gui.in_use && do_colors)
		gui_new_scrollbar_colors();
	    अन्यथा
		set_hl_attr(idx);
	}
# ifdef FEAT_BEVAL_GUI
	अन्यथा जर (is_tooltip_group)
	{
	    जर (gui.in_use && do_colors)
		gui_mch_new_tooltip_colors();
	}
# endif
#endif
	अन्यथा
	    set_hl_attr(idx);
#ifdef FEAT_EVAL
	HL_TABLE()[idx].sg_script_ctx = current_sctx;
	HL_TABLE()[idx].sg_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
    }

    vim_free(किल्ली);
    vim_free(arg);

    // Only call highlight_changed() once, after a sequence of highlight
    // commands, and only if an attribute actually changed.
    जर ((did_change
	   || स_भेद(&HL_TABLE()[idx], &item_before, आकार(item_before)) != 0)
#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    && !did_highlight_changed
#endif
       )
    {
	// Do not trigger a redraw when highlighting is changed while
	// redrawing.  This may happen when evaluating 'statusline' changes the
	// StatusLine group.
	जर (!updating_screen)
	    redraw_all_later(UPD_NOT_VALID);
	need_highlight_changed = TRUE;
    }
}

#जर defined(EXITFREE)
    शून्य
free_highlight(शून्य)
{
    पूर्णांक	    i;

    साठी (i = 0; i < highlight_ga.ga_len; ++i)
    {
	highlight_clear(i);
	vim_free(HL_TABLE()[i].sg_name);
	vim_free(HL_TABLE()[i].sg_name_u - HLNAME_किल्ली_OFF);
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
    शून्य
restore_cterm_colors(शून्य)
{
#जर defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
    // Since t_me has been set, this probably means that the user
    // wants to use this as default colors.  Need to reset default
    // background/foreground colors.
    mch_set_normal_colors();
#अन्यथा
# ifdef VIMDLL
    जर (!gui.in_use)
    {
	mch_set_normal_colors();
	परत;
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
    स्थिर पूर्णांक
hl_has_settings(पूर्णांक idx, पूर्णांक check_link)
{
    परत HL_TABLE()[idx].sg_cleared == 0
	 && (  HL_TABLE()[idx].sg_term_attr != 0
	    || HL_TABLE()[idx].sg_cterm_attr != 0
	    || HL_TABLE()[idx].sg_cterm_fg != 0
	    || HL_TABLE()[idx].sg_cterm_bg != 0
	    || HL_TABLE()[idx].sg_cterm_font != 0
#ifdef FEAT_GUI
	    || HL_TABLE()[idx].sg_gui_attr != 0
	    || HL_TABLE()[idx].sg_gui_fg_name != रिक्त
	    || HL_TABLE()[idx].sg_gui_bg_name != रिक्त
	    || HL_TABLE()[idx].sg_gui_sp_name != रिक्त
	    || HL_TABLE()[idx].sg_font_name != रिक्त
#endif
	    || (check_link && (HL_TABLE()[idx].sg_set & SG_LINK)));
}

/*
 * Clear highlighting for one group.
 */
    स्थिर शून्य
highlight_clear(पूर्णांक idx)
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
#जर defined(FEAT_GUI) || defined(FEAT_EVAL)
    HL_TABLE()[idx].sg_gui = 0;
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_fg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_bg_name);
    VIM_CLEAR(HL_TABLE()[idx].sg_gui_sp_name);
#endif
#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Set the normal foreground and background colors according to the "Normal"
 * highlighting group.  For X11 also set "Menu", "Scrollbar" and "Tooltip"
 * colors.  For MS-Windows also set "TitleBar" and "TitleBarNC" colors.
 */
    शून्य
set_normal_colors(शून्य)
{
# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    जर (gui.in_use)
#  endif
    {
	जर (set_group_colors((char_u *)"Normal",
				 &gui.norm_pixel, &gui.back_pixel,
				 FALSE, TRUE, FALSE))
	{
	    gui_mch_new_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  ifdef FEAT_GUI_MSWIN
	जर (set_group_colors((char_u *)"TitleBar",
				 &gui.title_fg_pixel, &gui.title_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
	जर (set_group_colors((char_u *)"TitleBarNC",
				 &gui.titlenc_fg_pixel, &gui.titlenc_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_set_titlebar_colors();
	    set_must_redraw(UPD_CLEAR);
	}
#  endif
#  ifdef FEAT_GUI_X11
	जर (set_group_colors((char_u *)"Menu",
			     &gui.menu_fg_pixel, &gui.menu_bg_pixel,
			     TRUE, FALSE, FALSE))
	{
#   ifdef FEAT_MENU
	    gui_mch_new_menu_colors();
#   endif
	    set_must_redraw(UPD_CLEAR);
	}
#   ifdef FEAT_BEVAL_GUI
	जर (set_group_colors((char_u *)"Tooltip",
			     &gui.tooltip_fg_pixel, &gui.tooltip_bg_pixel,
			     FALSE, FALSE, TRUE))
	{
#    ifdef FEAT_TOOLBAR
	    gui_mch_new_tooltip_colors();
#    endif
	    set_must_redraw(UPD_CLEAR);
	}
#   endif
	जर (set_group_colors((char_u *)"Scrollbar",
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
    अन्यथा
#  endif
    {
	पूर्णांक		idx;

	idx = syn_name2id((char_u *)"Normal") - 1;
	जर (idx >= 0)
	{
	    gui_do_one_color(idx, FALSE, FALSE);

	    // If the normal fg or bg color changed a complete redraw is
	    // required.
	    जर (cterm_normal_fg_gui_color != HL_TABLE()[idx].sg_gui_fg
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

#जर defined(FEAT_GUI)
/*
 * Set the colors for "Normal", "Menu", "TitleBar", "TitleBarNC", "Tooltip" or
 * "Scrollbar".
 */
    स्थिर पूर्णांक
set_group_colors(
    char_u	*name,
    guicolor_T	*fgp,
    guicolor_T	*bgp,
    पूर्णांक		do_menu,
    पूर्णांक		use_norm,
    पूर्णांक		do_tooltip)
{
    पूर्णांक		idx;

    idx = syn_name2id(name) - 1;
    जर (idx < 0)
	परत FALSE;

    gui_do_one_color(idx, do_menu, do_tooltip);

    जर (HL_TABLE()[idx].sg_gui_fg != INVALCOLOR)
	*fgp = HL_TABLE()[idx].sg_gui_fg;
    अन्यथा जर (use_norm)
	*fgp = gui.def_norm_pixel;
    जर (HL_TABLE()[idx].sg_gui_bg != INVALCOLOR)
	*bgp = HL_TABLE()[idx].sg_gui_bg;
    अन्यथा जर (use_norm)
	*bgp = gui.def_back_pixel;
    परत TRUE;
}

/*
 * Get the font of the "Normal" group.
 * Returns "" when it's not found or not set.
 */
    char_u *
hl_get_font_name(शून्य)
{
    पूर्णांक		id;
    char_u	*s;

    id = syn_name2id((char_u *)"Normal");
    जर (id > 0)
    {
	s = HL_TABLE()[id - 1].sg_font_name;
	जर (s != रिक्त)
	    परत s;
    }
    परत (char_u *)"";
}

/*
 * Set font for "Normal" group.  Called by gui_mch_init_font() when a font has
 * actually chosen to be used.
 */
    शून्य
hl_set_font_name(char_u *font_name)
{
    पूर्णांक	    id;

    id = syn_name2id((char_u *)"Normal");
    जर (id <= 0)
	परत;

    vim_free(HL_TABLE()[id - 1].sg_font_name);
    HL_TABLE()[id - 1].sg_font_name = vim_strsave(font_name);
}

/*
 * Set background color for "Normal" group.  Called by gui_set_bg_color()
 * when the color is known.
 */
    शून्य
hl_set_bg_color_name(
    char_u  *name)	    // must have been allocated
{
    पूर्णांक	    id;

    जर (name == रिक्त)
	परत;

    id = syn_name2id((char_u *)"Normal");
    जर (id <= 0)
	परत;

    vim_free(HL_TABLE()[id - 1].sg_gui_bg_name);
    HL_TABLE()[id - 1].sg_gui_bg_name = name;
}

/*
 * Set foreground color for "Normal" group.  Called by gui_set_fg_color()
 * when the color is known.
 */
    शून्य
hl_set_fg_color_name(
    char_u  *name)	    // must have been allocated
{
    पूर्णांक	    id;

    जर (name == रिक्त)
	परत;

    id = syn_name2id((char_u *)"Normal");
    जर (id <= 0)
	परत;

    vim_free(HL_TABLE()[id - 1].sg_gui_fg_name);
    HL_TABLE()[id - 1].sg_gui_fg_name = name;
}

/*
 * Return the handle for a font name.
 * Returns NOFONT when failed.
 */
    स्थिर GuiFont
font_name2handle(char_u *name)
{
    जर (STRCMP(name, "NONE") == 0)
	परत NOFONT;

    परत gui_mch_get_font(name, TRUE);
}

# ifdef FEAT_XFONTSET
/*
 * Return the handle for a fontset name.
 * Returns NOFONTSET when failed.
 */
    स्थिर GuiFontset
fontset_name2handle(char_u *name, पूर्णांक fixed_width)
{
    जर (STRCMP(name, "NONE") == 0)
	परत NOFONTSET;

    परत gui_mch_get_fontset(name, TRUE, fixed_width);
}
# endif

/*
 * Get the font or fontset for one highlight group.
 */
    स्थिर शून्य
hl_do_font(
    पूर्णांक		idx,
    char_u	*arg,
    पूर्णांक		do_normal,		// set normal font
    पूर्णांक		do_menu UNUSED,		// set menu font
    पूर्णांक		do_tooltip UNUSED,	// set tooltip font
    पूर्णांक		free_font)		// free current font/fontset
{
# ifdef FEAT_XFONTSET
    // If 'guifontset' is not empty, first try using the name as a
    // fontset.  If that doesn't work, use it as a font name.
    जर (*p_guifontset != NUL
#  ifdef FONTSET_ALWAYS
	|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
	// In Motif, the Tooltip highlight group is always a fontset
	|| do_tooltip
#  endif
	    )
    {
	जर (free_font)
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
    जर (HL_TABLE()[idx].sg_fontset != NOFONTSET)
    {
	// If it worked and it's the Normal group, use it as the normal
	// fontset.  Same for the Menu group.
	जर (do_normal)
	    gui_init_font(arg, TRUE);
#  जर defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	जर (do_menu)
	{
#   ifdef FONTSET_ALWAYS
	    gui.menu_fontset = HL_TABLE()[idx].sg_fontset;
#   अन्यथा
	    // YIKES!  This is a bug waiting to crash the program
	    gui.menu_font = HL_TABLE()[idx].sg_fontset;
#   endif
	    gui_mch_new_menu_font();
	}
#   ifdef FEAT_BEVAL_GUI
	जर (do_tooltip)
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
    अन्यथा
# endif
    {
	जर (free_font)
	    gui_mch_free_font(HL_TABLE()[idx].sg_font);
	HL_TABLE()[idx].sg_font = font_name2handle(arg);
	// If it worked and it's the Normal group, use it as the
	// normal font.  Same for the Menu group.
	जर (HL_TABLE()[idx].sg_font != NOFONT)
	{
	    जर (do_normal)
		gui_init_font(arg, FALSE);
# ifndef FONTSET_ALWAYS
#  जर defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	    जर (do_menu)
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

#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return the handle for a color name.
 * Returns INVALCOLOR when failed.
 */
    guicolor_T
color_name2handle(char_u *name)
{
    जर (STRCMP(name, "NONE") == 0)
	परत INVALCOLOR;

    जर (STRICMP(name, "fg") == 0 || STRICMP(name, "foreground") == 0)
    {
# जर defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	जर (gui.in_use)
# endif
# ifdef FEAT_GUI
	    परत gui.norm_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	जर (cterm_normal_fg_gui_color != INVALCOLOR)
	    परत cterm_normal_fg_gui_color;
	// Guess that the foreground is black or white.
	परत GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "black" : "white"));
# endif
    }
    जर (STRICMP(name, "bg") == 0 || STRICMP(name, "background") == 0)
    {
# जर defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	जर (gui.in_use)
# endif
# ifdef FEAT_GUI
	    परत gui.back_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	जर (cterm_normal_bg_gui_color != INVALCOLOR)
	    परत cterm_normal_bg_gui_color;
	// Guess that the background is white or black.
	परत GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "white" : "black"));
# endif
    }

    परत GUI_GET_COLOR(name);
}

// On MS-Windows an RGB macro is available and it produces 0x00bbggrr color
// values as used by the MS-Windows GDI api.  It should be used only for
// MS-Windows GDI builds.
# जर defined(RGB) && defined(MSWIN) && !defined(FEAT_GUI)
#  undef RGB
# endif
# ifndef RGB
#  define RGB(r, g, b)	(((r)<<16) | ((g)<<8) | (b))
# endif

# ifdef VIMDLL
    स्थिर guicolor_T
gui_adjust_rgb(guicolor_T c)
{
    जर (gui.in_use)
	परत c;
    अन्यथा
	परत ((c & 0xff) << 16) | (c & 0x00ff00) | ((c >> 16) & 0xff);
}
# अन्यथा
#  define gui_adjust_rgb(c) (c)
# endif

    स्थिर पूर्णांक
hex_digit(पूर्णांक c)
{
    जर (SAFE_isdigit(c))
	परत c - '0';
    c = TOLOWER_ASC(c);
    जर (c >= 'a' && c <= 'f')
	परत c - 'a' + 10;
    परत 0x1ffffff;
}

    स्थिर guicolor_T
decode_hex_color(char_u *hex)
{
    guicolor_T color;

    जर (hex[0] != '#' || STRLEN(hex) != 7)
	परत INVALCOLOR;

    // Name is in "#rrggbb" format
    color = RGB(((hex_digit(hex[1]) << 4) + hex_digit(hex[2])),
		((hex_digit(hex[3]) << 4) + hex_digit(hex[4])),
		((hex_digit(hex[5]) << 4) + hex_digit(hex[6])));
    जर (color > 0xffffff)
	परत INVALCOLOR;
    परत gui_adjust_rgb(color);
}

# ifdef FEAT_EVAL
// Returns the color currently mapped to the given name or INVALCOLOR if no
// such name exists in the color table. The convention is to use lowercase for
// all keys in the v:colornames dictionary. The value can be either a string in
// the form #rrggbb or a number, either of which is converted to a guicolor_T.
    स्थिर guicolor_T
colorname2rgb(char_u *name)
{
    dict_T      *colornames_table = get_vim_var_dict(VV_COLORNAMES);
    char_u      *lc_name;
    dictitem_T  *colentry;
    char_u      *colstr;
    varnumber_T colnum;

    lc_name = strlow_save(name);
    जर (lc_name == रिक्त)
	परत INVALCOLOR;

    colentry = dict_find(colornames_table, lc_name, -1);
    vim_free(lc_name);
    जर (colentry == रिक्त)
	परत INVALCOLOR;

    जर (colentry->di_tv.v_type == VAR_STRING)
    {
	colstr = tv_get_string_strict(&colentry->di_tv);
	जर ((STRLEN(colstr) == 7) && (*colstr == '#'))
	{
	    परत decode_hex_color(colstr);
	}
	अन्यथा
	{
	    semsg(_(e_bad_color_string_str), colstr);
	    परत INVALCOLOR;
	}
    }

    जर (colentry->di_tv.v_type == VAR_संख्या)
    {
	colnum = tv_get_संख्या(&colentry->di_tv);
	परत (guicolor_T)colnum;
    }

    परत INVALCOLOR;
}

# endif

    guicolor_T
gui_get_color_cmn(char_u *name)
{
    guicolor_T  color;
    // Only non X11 colors (not present in rgb.txt) and colors in
    // color_name_tab[], useful when $VIMRUNTIME is not found,.
    // must be sorted by the 'value' field because it is used by bsearch()!
    स्थिर keyvalue_T rgb_tab[] = {
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
    जर (color != INVALCOLOR)
	परत color;

    target.किल्ली = 0;
    target.मूल्य.string = name;
    target.मूल्य.length = 0;	// not used, see cmp_keyvalue_value_i()
    entry = (keyvalue_T *)द्वा_खोज(&target, &rgb_tab, ARRAY_LENGTH(rgb_tab),
	    आकार(rgb_tab[0]), cmp_keyvalue_मूल्य_i);
    जर (entry != रिक्त)
	परत gui_adjust_rgb((guicolor_T)entry->किल्ली);

# जर defined(FEAT_EVAL)
    /*
     * Not a traditional color. Load additional color aliases and then consult the alias table.
     */

    color = colorname2rgb(name);
    जर (color == INVALCOLOR)
    {
	load_default_colors_lists();
	color = colorname2rgb(name);
    }

    परत color;
# अन्यथा
    परत INVALCOLOR;
# endif
}

    guicolor_T
gui_get_rgb_color_cmn(पूर्णांक r, पूर्णांक g, पूर्णांक b)
{
    guicolor_T  color = RGB(r, g, b);

    जर (color > 0xffffff)
	परत INVALCOLOR;
    परत gui_adjust_rgb(color);
}
#endif

/*
 * Table with the specifications for an attribute number.
 * Note that this table is used by ALL buffers.  This is required because the
 * GUI can redraw at any time for any buffer.
 */
स्थिर garray_T	term_attr_table = {0, 0, 0, 0, रिक्त};

#define TERM_ATTR_ENTRY(idx) ((attrentry_T *)term_attr_table.ga_data)[idx]

स्थिर garray_T	cterm_attr_table = {0, 0, 0, 0, रिक्त};

#define CTERM_ATTR_ENTRY(idx) ((attrentry_T *)cterm_attr_table.ga_data)[idx]

#ifdef FEAT_GUI
स्थिर garray_T	gui_attr_table = {0, 0, 0, 0, रिक्त};

# define GUI_ATTR_ENTRY(idx) ((attrentry_T *)gui_attr_table.ga_data)[idx]
#endif

/*
 * Return the attr number for a set of colors and font.
 * Add a new entry to the term_attr_table, cterm_attr_table or gui_attr_table
 * if the combination is new.
 * Return 0 for error (no more room).
 */
    स्थिर पूर्णांक
get_attr_entry(garray_T *table, attrentry_T *aep)
{
    पूर्णांक		i;
    attrentry_T	*taep;
    स्थिर पूर्णांक	recursive = FALSE;

    // Init the table, in case it wasn't done yet.
    table->ga_itemsize = आकार(attrentry_T);
    table->ga_growsize = 7;

    // Try to find an entry with the same specifications.
    साठी (i = 0; i < table->ga_len; ++i)
    {
	taep = &(((attrentry_T *)table->ga_data)[i]);
	जर (	   aep->ae_attr == taep->ae_attr
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
			&& (aep->ae_u.term.start == रिक्त)
					    == (taep->ae_u.term.start == रिक्त)
			&& (aep->ae_u.term.start == रिक्त
			    || STRCMP(aep->ae_u.term.start,
						  taep->ae_u.term.start) == 0)
			&& (aep->ae_u.term.stop == रिक्त)
					     == (taep->ae_u.term.stop == रिक्त)
			&& (aep->ae_u.term.stop == रिक्त
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

	परत i + ATTR_OFF;
    }

    जर (table->ga_len + ATTR_OFF > MAX_TYPENR)
    {
	// Running out of attribute entries!  remove all attributes, and
	// compute new ones for all groups.
	// When called recursively, we are really out of numbers.
	जर (recursive)
	{
	    emsg(_(e_too_many_different_highlighting_attributes_in_use));
	    परत 0;
	}
	recursive = TRUE;

	clear_hl_tables();

	set_must_redraw(UPD_CLEAR);

	साठी (i = 0; i < highlight_ga.ga_len; ++i)
	    set_hl_attr(i);

	recursive = FALSE;
    }

    // This is a new combination of colors and font, add an entry.
    जर (ga_grow(table, 1) == FAIL)
	परत 0;

    taep = &(((attrentry_T *)table->ga_data)[table->ga_len]);
    CLEAR_POINTER(taep);
    taep->ae_attr = aep->ae_attr;
#ifdef FEAT_GUI
    जर (table == &gui_attr_table)
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
    जर (table == &term_attr_table)
    {
	जर (aep->ae_u.term.start == रिक्त)
	    taep->ae_u.term.start = रिक्त;
	अन्यथा
	    taep->ae_u.term.start = vim_strsave(aep->ae_u.term.start);
	जर (aep->ae_u.term.stop == रिक्त)
	    taep->ae_u.term.stop = रिक्त;
	अन्यथा
	    taep->ae_u.term.stop = vim_strsave(aep->ae_u.term.stop);
    }
    अन्यथा जर (table == &cterm_attr_table)
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
    परत (table->ga_len - 1 + ATTR_OFF);
}

#जर defined(FEAT_TERMINAL)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    पूर्णांक
get_cterm_attr_idx(पूर्णांक attr, पूर्णांक fg, पूर्णांक bg)
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
    परत get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#जर defined(FEAT_TERMINAL) && defined(FEAT_TERMGUICOLORS)
/*
 * Get an attribute index for a 'termguicolors' entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    पूर्णांक
get_tgc_attr_idx(पूर्णांक attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    जर (fg == INVALCOLOR && bg == INVALCOLOR)
    {
	// If both GUI colors are not set fall back to the cterm colors.  Helps
	// if the GUI only has an attribute, such as undercurl.
	at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
    }
    अन्यथा
    {
	at_en.ae_u.cterm.fg_rgb = fg;
	at_en.ae_u.cterm.bg_rgb = bg;
    }
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
    परत get_attr_entry(&cterm_attr_table, &at_en);
}
#endif

#जर defined(FEAT_TERMINAL) && defined(FEAT_GUI)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    पूर्णांक
get_gui_attr_idx(पूर्णांक attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_FIELD(at_en);
    at_en.ae_attr = attr;
    at_en.ae_u.gui.fg_color = fg;
    at_en.ae_u.gui.bg_color = bg;
    परत get_attr_entry(&gui_attr_table, &at_en);
}
#endif

/*
 * Clear all highlight tables.
 */
    शून्य
clear_hl_tables(शून्य)
{
    पूर्णांक		i;
    attrentry_T	*taep;

#ifdef FEAT_GUI
    ga_clear(&gui_attr_table);
#endif
    साठी (i = 0; i < term_attr_table.ga_len; ++i)
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
    पूर्णांक
hl_combine_attr(पूर्णांक char_attr, पूर्णांक prim_attr)
{
    attrentry_T *char_aep = रिक्त;
    attrentry_T *prim_aep;
    attrentry_T new_en;

    जर (char_attr == 0)
	परत prim_attr;
    जर (char_attr <= HL_ALL && prim_attr <= HL_ALL)
	परत ATTR_COMBINE(char_attr, prim_attr);
#ifdef FEAT_GUI
    जर (gui.in_use)
    {
	जर (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	जर (char_aep != रिक्त)
	    new_en = *char_aep;
	अन्यथा
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    जर (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	जर (prim_attr <= HL_ALL)
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	अन्यथा
	{
	    prim_aep = syn_gui_attr2entry(prim_attr);
	    जर (prim_aep != रिक्त)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		जर (prim_aep->ae_u.gui.fg_color != INVALCOLOR)
		    new_en.ae_u.gui.fg_color = prim_aep->ae_u.gui.fg_color;
		जर (prim_aep->ae_u.gui.bg_color != INVALCOLOR)
		    new_en.ae_u.gui.bg_color = prim_aep->ae_u.gui.bg_color;
		जर (prim_aep->ae_u.gui.sp_color != INVALCOLOR)
		    new_en.ae_u.gui.sp_color = prim_aep->ae_u.gui.sp_color;
		जर (prim_aep->ae_u.gui.font != NOFONT)
		    new_en.ae_u.gui.font = prim_aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
		जर (prim_aep->ae_u.gui.fontset != NOFONTSET)
		    new_en.ae_u.gui.fontset = prim_aep->ae_u.gui.fontset;
# endif
	    }
	}
	परत get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    जर (IS_CTERM)
    {
	जर (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	जर (char_aep != रिक्त)
	    new_en = *char_aep;
	अन्यथा
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    जर (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	जर (prim_attr <= HL_ALL)
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	अन्यथा
	{
	    prim_aep = syn_cterm_attr2entry(prim_attr);
	    जर (prim_aep != रिक्त)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		जर (prim_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color = prim_aep->ae_u.cterm.fg_color;
		जर (prim_aep->ae_u.cterm.bg_color > 0)
		    new_en.ae_u.cterm.bg_color = prim_aep->ae_u.cterm.bg_color;
		जर (prim_aep->ae_u.cterm.ul_color > 0)
		    new_en.ae_u.cterm.ul_color = prim_aep->ae_u.cterm.ul_color;
		जर (prim_aep->ae_u.cterm.font > 0)
		    new_en.ae_u.cterm.font = prim_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		// If both fg and bg are not set fall back to cterm colors.
		// Helps for SpellBad which uses undercurl in the GUI.
		जर (COLOR_INVALID(prim_aep->ae_u.cterm.fg_rgb)
			&& COLOR_INVALID(prim_aep->ae_u.cterm.bg_rgb))
		{
		    जर (prim_aep->ae_u.cterm.fg_color > 0)
			new_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
		    जर (prim_aep->ae_u.cterm.bg_color > 0)
			new_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
		}
		अन्यथा
		{
		    जर (prim_aep->ae_u.cterm.fg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.fg_rgb = prim_aep->ae_u.cterm.fg_rgb;
		    जर (prim_aep->ae_u.cterm.bg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.bg_rgb = prim_aep->ae_u.cterm.bg_rgb;
		}
		जर (prim_aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		    new_en.ae_u.cterm.ul_rgb = prim_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	}
	परत get_attr_entry(&cterm_attr_table, &new_en);
    }

    जर (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    जर (char_aep != रिक्त)
	new_en = *char_aep;
    अन्यथा
    {
	CLEAR_FIELD(new_en);
	जर (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    जर (prim_attr <= HL_ALL)
	new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
    अन्यथा
    {
	prim_aep = syn_term_attr2entry(prim_attr);
	जर (prim_aep != रिक्त)
	{
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_aep->ae_attr);
	    जर (prim_aep->ae_u.term.start != रिक्त)
	    {
		new_en.ae_u.term.start = prim_aep->ae_u.term.start;
		new_en.ae_u.term.stop = prim_aep->ae_u.term.stop;
	    }
	}
    }
    परत get_attr_entry(&term_attr_table, &new_en);
}

// ANSI color order: Black, Red, Green, Yellow, Blue, Magenta,
// Cyan, White, then bright variants.  Approximate RGB values used when
// only a cterm color number is known (no guifg/guibg).  Real terminal
// palettes may differ if the user reconfigured their emulator, but
// these are reasonable xterm-ish defaults.
स्थिर स्थिरांक long_u cterm_color_16[16] = {
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
    स्थिर guicolor_T
cterm_color_to_rgb(पूर्णांक color_nr)
{
    जर (color_nr < 1 || color_nr > 16)
	परत INVALCOLOR;
    परत (guicolor_T)cterm_color_16[color_nr - 1];
}
#endif

/*
 * Convert an xterm 256-color index (0-255) to an approximate RGB triple.
 * Uses the standard xterm palette: 0-15 ANSI (cterm_color_16), 16-231
 * 6x6x6 cube, 232-255 grayscale ramp.
 */
    स्थिर शून्य
cterm_idx_to_rgb(पूर्णांक idx, पूर्णांक *r, पूर्णांक *g, पूर्णांक *b)
{
    स्थिर स्थिरांक पूर्णांक cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

    जर (idx < 0 || idx > 255)
    {
	*r = *g = *b = 0;
	परत;
    }
    जर (idx < 16)
    {
	long_u rgb = cterm_color_16[idx];
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
    }
    अन्यथा जर (idx < 232)
    {
	पूर्णांक n = idx - 16;
	*r = cube[n / 36 % 6];
	*g = cube[n / 6  % 6];
	*b = cube[n      % 6];
    }
    अन्यथा
    {
	पूर्णांक v = 8 + (idx - 232) * 10;
	*r = *g = *b = v;
    }
}

/*
 * Approximate an RGB triple to the nearest xterm 256-color index.
 * Searches the 6x6x6 cube and the grayscale ramp; returns 0-255.
 */
    स्थिर पूर्णांक
rgb_to_cterm_idx(पूर्णांक r, पूर्णांक g, पूर्णांक b)
{
    स्थिर स्थिरांक पूर्णांक cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };
    पूर्णांक best = 0;
    दीर्घ best_d = -1;
    पूर्णांक i;

    // Search the 6x6x6 cube.
    साठी (i = 16; i < 232; ++i)
    {
	पूर्णांक n = i - 16;
	पूर्णांक cr = cube[n / 36 % 6];
	पूर्णांक cg = cube[n / 6  % 6];
	पूर्णांक cb = cube[n      % 6];
	दीर्घ d = (cr - r) * (cr - r) + (cg - g) * (cg - g) + (cb - b) * (cb - b);
	जर (best_d < 0 || d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    // Search the grayscale ramp.
    साठी (i = 232; i < 256; ++i)
    {
	पूर्णांक v = 8 + (i - 232) * 10;
	दीर्घ d = (v - r) * (v - r) + (v - g) * (v - g) + (v - b) * (v - b);
	जर (d < best_d)
	{
	    best_d = d;
	    best = i;
	}
    }
    परत best;
}

/*
 * Resolve a color to an RGB triple.  Prefer the gui RGB value (set by
 * guifg/guibg) when valid, otherwise convert the cterm 256-color index.
 * Returns true on success; false when no color is defined.
 * cterm_c is 1-based (0 means "no color").
 */
    स्थिर bool
resolve_color_to_rgb(पूर्णांक cterm_c, guicolor_T rgb UNUSED, पूर्णांक *r, पूर्णांक *g, पूर्णांक *b)
{
#ifdef FEAT_TERMGUICOLORS
    जर (!COLOR_INVALID(rgb))
    {
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
	परत true;
    }
#endif
    जर (cterm_c > 0)
    {
	cterm_idx_to_rgb(cterm_c - 1, r, g, b);
	परत true;
    }
    परत false;
}

/*
 * get a RGB fallback color from gui, cterm or default color
 */
    स्थिर guicolor_T
resolve_fallback_color(पूर्णांक cterm_c, guicolor_T rgb, guicolor_T default_rgb)
{
    पूर्णांक red, green, blue;
    जर (!resolve_color_to_rgb(cterm_c, rgb, &red, &green, &blue))
	परत default_rgb;
    अन्यथा
	परत (red << 16) | (green << 8) | blue;
}

/*
 * get a RGB fallback foreground color from guifg, ctermfg or deduced from background
 */
    स्थिर शून्य
resolve_fallback_fg_to_rgb(शून्य)
{
    guicolor_T fgcolor_or_gui_fgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    fgcolor_or_gui_fgcolor = cterm_normal_fg_gui_color;
#endif
#ifdef FEAT_GUI
    जर (gui.in_use)
	fgcolor_or_gui_fgcolor = gui.norm_pixel;
#endif
    fallback_fg_rgb = resolve_fallback_color(cterm_normal_fg_color, fgcolor_or_gui_fgcolor, (*p_bg == 'l') ? 0x000000 : 0xFFFFFF);
}

/*
 * get a RGB fallback background color from guifg, ctermbg or deduced from background
 */
    स्थिर शून्य
resolve_fallback_bg_to_rgb(शून्य)
{
    guicolor_T bgcolor_or_gui_bgcolor = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
    bgcolor_or_gui_bgcolor = cterm_normal_bg_gui_color;
#endif
#ifdef FEAT_GUI
    जर (gui.in_use)
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
    स्थिर पूर्णांक
blend_cterm_colors(पूर्णांक popup_c, guicolor_T popup_rgb,
		   पूर्णांक under_c, guicolor_T under_rgb,
		   पूर्णांक default_rgb,
		   पूर्णांक blend_val)
{
    पूर्णांक pr, pg, pb, ur, ug, ub, r, g, b;

    जर (t_colors < 256)
	परत popup_c;
    जर (!resolve_color_to_rgb(popup_c, popup_rgb, &pr, &pg, &pb))
	परत under_c;
    जर (blend_val <= 0)
	परत rgb_to_cterm_idx(pr, pg, pb) + 1;
    जर (!resolve_color_to_rgb(under_c, under_rgb, &ur, &ug, &ub))
    {
	ur = (default_rgb >> 16) & 0xFF;
	ug = (default_rgb >> 8) & 0xFF;
	ub = default_rgb & 0xFF;
    }
    जर (blend_val >= 100)
	परत rgb_to_cterm_idx(ur, ug, ub) + 1;
    r = pr + (ur - pr) * blend_val / 100;
    g = pg + (ug - pg) * blend_val / 100;
    b = pb + (ub - pb) * blend_val / 100;
    परत rgb_to_cterm_idx(r, g, b) + 1;
}

#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Blend two RGB colors based on blend value (0-100).
 * blend: 0=return first argument color, 100=return second argument color
 * If bg_color is INVALCOLOR, high blend means more visible (return INVALCOLOR).
 */
    स्थिर guicolor_T
blend_colors(guicolor_T popup_color, guicolor_T bg_color, पूर्णांक blend_val)
{
    पूर्णांक r1, g1, b1, r2, g2, b2, r, g, b;

    // CTERMCOLOR is a sentinel meaning "use the cterm color"; for blending
    // it has no real RGB so treat it like INVALCOLOR.
    जर (COLOR_INVALID(popup_color))
	परत INVALCOLOR;

    // Fully transparent: use underlying color as-is.
    जर (blend_val >= 100)
	परत bg_color;

    r1 = (popup_color >> 16) & 0xFF;
    g1 = (popup_color >> 8) & 0xFF;
    b1 = popup_color & 0xFF;

    जर (COLOR_INVALID(bg_color))
	bg_color = fallback_bg_rgb;

    r2 = (bg_color >> 16) & 0xFF;
    g2 = (bg_color >> 8) & 0xFF;
    b2 = bg_color & 0xFF;

    r = r1 + (r2 - r1) * blend_val / 100;
    g = g1 + (g2 - g1) * blend_val / 100;
    b = b1 + (b2 - b1) * blend_val / 100;

    परत (r << 16) | (g << 8) | b;
}
#endif

/*
 * Blend attributes for popup windows and menus with opacity.
 * Blends foreground and/or background colors based on blend value (0-100).
 * blend: 0 = opaque (use popup colors), 100 = transparent (use background colors)
 * blend_fg: TRUE to blend foreground color, FALSE to keep popup foreground
 */
    स्थिर पूर्णांक
hl_blend_attr_common(
    पूर्णांक		char_attr,
    पूर्णांक		popup_attr,
    पूर्णांक		blend,
    पूर्णांक		blend_fg)
{
    attrentry_T *char_aep = रिक्त;
    attrentry_T *popup_aep;
    attrentry_T new_en;
    attrentry_T tmp_en;

#ifdef FEAT_GUI
    जर (gui.in_use)
    {
	जर (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	जर (char_aep != रिक्त)
	    new_en = *char_aep;
	अन्यथा
	{
	    CLEAR_FIELD(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    जर (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	जर (popup_attr <= HL_ALL)
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
	अन्यथा
	    popup_aep = syn_gui_attr2entry(popup_attr);

	जर (popup_aep != रिक्त)
	{
	    guicolor_T popup_bg_rgb = popup_aep->ae_u.gui.bg_color;
	    जर (COLOR_INVALID(popup_bg_rgb))
		popup_bg_rgb = fallback_bg_rgb;

	    जर (blend_fg)
	    {
		// blend_fg=TRUE: fade underlying text toward popup bg.
		{
		    पूर्णांक base_fg = fallback_fg_rgb;
		    जर (char_aep != रिक्त
			    && char_aep->ae_u.gui.fg_color != INVALCOLOR)
			base_fg = char_aep->ae_u.gui.fg_color;
		    new_en.ae_u.gui.fg_color = blend_colors(
			    popup_bg_rgb, base_fg, blend);
		}
	    }
	    अन्यथा
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
		जर (char_aep != रिक्त
			&& !COLOR_INVALID(char_aep->ae_u.gui.bg_color))
		    underlying_bg = char_aep->ae_u.gui.bg_color;
		new_en.ae_u.gui.bg_color = blend_colors(
			popup_bg_rgb,
			underlying_bg, blend);
	    }
	}
	परत get_attr_entry(&gui_attr_table, &new_en);
    }
#endif

    जर (IS_CTERM)
    {
	जर (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	जर (char_aep != रिक्त)
	    new_en = *char_aep;
	अन्यथा
	{
	    CLEAR_FIELD(new_en);
#ifdef FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#endif
	    जर (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	जर (popup_attr <= HL_ALL)
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
	अन्यथा
	    popup_aep = syn_cterm_attr2entry(popup_attr);

	जर (popup_aep != रिक्त)
	{
	    guicolor_T popup_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
	    // Fall back to cterm color converted to RGB when gui color is not set.
	    popup_bg_rgb = popup_aep->ae_u.cterm.bg_rgb;
	    जर (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color > 0)
		popup_bg_rgb = cterm_color_to_rgb(
			popup_aep->ae_u.cterm.bg_color);
#endif
	    // assign default color if guibg and ctermbg are not set for popup
	    जर (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color == 0)
		popup_bg_rgb = fallback_bg_rgb;

	    जर (!blend_fg)
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
		जर (popup_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color =
			popup_aep->ae_u.cterm.fg_color;
		अन्यथा जर (cterm_normal_fg_color > 0)
		    new_en.ae_u.cterm.fg_color = cterm_normal_fg_color;
		अन्यथा
		    // black-ish or white-ish
		    new_en.ae_u.cterm.fg_color = (*p_bg == 'l') ? 1 : 16;
		new_en.ae_u.cterm.ul_color = popup_aep->ae_u.cterm.ul_color;
		new_en.ae_u.cterm.font = popup_aep->ae_u.cterm.font;
#ifdef FEAT_TERMGUICOLORS
		new_en.ae_u.cterm.ul_rgb = popup_aep->ae_u.cterm.ul_rgb;
#endif
	    }
	    अन्यथा
	    {
		// blend_fg=TRUE: fade underlying fg toward popup bg in
		// the 256-color palette.  Used when the popup is over a
		// cell rendered with cterm colors (no termguicolors RGB).
		पूर्णांक under_fg = (char_aep != रिक्त)
		    ? char_aep->ae_u.cterm.fg_color : 0;
		guicolor_T under_fg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		जर (char_aep != रिक्त)
		    under_fg_rgb = char_aep->ae_u.cterm.fg_rgb;
#endif
		new_en.ae_u.cterm.fg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_fg, under_fg_rgb, fallback_fg_rgb, blend);
	    }
	    // Approximate cterm bg by blending with the underlying bg
	    // in the 256-color palette and mapping to the nearest entry.
	    {
		पूर्णांक under_bg = (char_aep != रिक्त)
		    ? char_aep->ae_u.cterm.bg_color : 0;
		guicolor_T under_bg_rgb = INVALCOLOR;
#ifdef FEAT_TERMGUICOLORS
		जर (char_aep != रिक्त)
		    under_bg_rgb = char_aep->ae_u.cterm.bg_rgb;
#endif
		new_en.ae_u.cterm.bg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_bg, under_bg_rgb, fallback_bg_rgb, blend);
	    }
#ifdef FEAT_TERMGUICOLORS
	    // Blend RGB colors for termguicolors mode.
	    {
		जर (blend_fg)
		{
		    // blend_fg=TRUE: fade underlying text toward popup bg.
		    जर (popup_bg_rgb != INVALCOLOR)
		    {
			पूर्णांक base_fg = fallback_fg_rgb;
			// CTERMCOLOR is a sentinel meaning "use the cterm
			// color"; treat it as no underlying color so it is
			// not blended in as a real near-white pixel.
			जर (char_aep != रिक्त
				&& !COLOR_INVALID(char_aep->ae_u.cterm.fg_rgb))
			    base_fg = char_aep->ae_u.cterm.fg_rgb;
			new_en.ae_u.cterm.fg_rgb = blend_colors(
				popup_bg_rgb, base_fg, blend);
		    }
		}
		अन्यथा
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

		    जर (COLOR_INVALID(popup_fg)
			    && popup_aep->ae_u.cterm.fg_color > 0)
			popup_fg = cterm_color_to_rgb(
				popup_aep->ae_u.cterm.fg_color);

		    जर (!COLOR_INVALID(popup_fg))
			new_en.ae_u.cterm.fg_rgb = popup_fg;
		    अन्यथा जर (!COLOR_INVALID(cterm_normal_fg_gui_color))
			new_en.ae_u.cterm.fg_rgb = cterm_normal_fg_gui_color;
		    अन्यथा
			new_en.ae_u.cterm.fg_rgb = fallback_fg_rgb;
		}
		जर (popup_bg_rgb != INVALCOLOR)
		{
		    // Blend popup bg toward underlying bg
		    guicolor_T underlying_bg = fallback_bg_rgb;
		    जर (char_aep != रिक्त
			    && !COLOR_INVALID(char_aep->ae_u.cterm.bg_rgb))
			underlying_bg = char_aep->ae_u.cterm.bg_rgb;
		    new_en.ae_u.cterm.bg_rgb = blend_colors(
			    popup_bg_rgb, underlying_bg, blend);
		}
	    }
#endif
	}
	परत get_attr_entry(&cterm_attr_table, &new_en);
    }

    जर (char_attr > HL_ALL)
	char_aep = syn_term_attr2entry(char_attr);
    जर (char_aep != रिक्त)
	new_en = *char_aep;
    अन्यथा
    {
	CLEAR_FIELD(new_en);
	जर (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    // For term mode, no separate background color handling.
    परत get_attr_entry(&term_attr_table, &new_en);
}

/*
 * Blend attributes for popup windows with opacity.
 */
    पूर्णांक
hl_blend_attr(पूर्णांक char_attr, पूर्णांक popup_attr, पूर्णांक blend, पूर्णांक blend_fg)
{
    जर (blend >= 100 && blend_fg)
	परत char_attr;

    परत hl_blend_attr_common(char_attr, popup_attr, blend, blend_fg);
}

/*
 * Blend underlying foreground and background colors for pum space cells.
 */
    पूर्णांक
hl_pum_blend_attr(पूर्णांक char_attr, पूर्णांक popup_attr, पूर्णांक blend)
{
    परत hl_blend_attr_common(char_attr, popup_attr, blend, TRUE);
}

#ifdef FEAT_GUI
    attrentry_T *
syn_gui_attr2entry(पूर्णांक attr)
{
    attr -= ATTR_OFF;
    जर (attr >= gui_attr_table.ga_len)	    // did ":syntax clear"
	परत रिक्त;
    परत &(GUI_ATTR_ENTRY(attr));
}
#endif

/*
 * Get the highlight attributes (HL_BOLD etc.) from an attribute nr.
 * Only to be used when "attr" > HL_ALL.
 */
    पूर्णांक
syn_attr2attr(पूर्णांक attr)
{
    attrentry_T	*aep;

#ifdef FEAT_GUI
    जर (gui.in_use)
	aep = syn_gui_attr2entry(attr);
    अन्यथा
#endif
	जर (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	अन्यथा
	    aep = syn_term_attr2entry(attr);

    जर (aep == रिक्त)	    // highlighting not set
	परत 0;
    परत aep->ae_attr;
}


    attrentry_T *
syn_term_attr2entry(पूर्णांक attr)
{
    attr -= ATTR_OFF;
    जर (attr >= term_attr_table.ga_len)	    // did ":syntax clear"
	परत रिक्त;
    परत &(TERM_ATTR_ENTRY(attr));
}

    attrentry_T *
syn_cterm_attr2entry(पूर्णांक attr)
{
    attr -= ATTR_OFF;
    जर (attr >= cterm_attr_table.ga_len)	// did ":syntax clear"
	परत रिक्त;
    परत &(CTERM_ATTR_ENTRY(attr));
}

#define LIST_ATTR   1
#define LIST_STRING 2
#define LIST_INT    3

    स्थिर शून्य
highlight_list_one(पूर्णांक id)
{
    hl_group_T	    *sgp;
    पूर्णांक		    didh = FALSE;

    sgp = &HL_TABLE()[id - 1];	    // index is ID minus one

    जर (message_filtered(sgp->sg_name))
	परत;

    // Note: Keep this in sync with expand_highlight_group().
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_term, रिक्त, "term");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_start, "start");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_stop, "stop");

    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_cterm, रिक्त, "cterm");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_fg, रिक्त, "ctermfg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_bg, रिक्त, "ctermbg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_ul, रिक्त, "ctermul");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_font, रिक्त, "ctermfont");

#जर defined(FEAT_GUI) || defined(FEAT_EVAL)
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_gui, रिक्त, "gui");
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

    जर (sgp->sg_link && !got_int)
    {
	(शून्य)syn_list_header(didh, 9999, id);
	didh = TRUE;
	संदेश_puts_attr("links to", HL_ATTR(HLF_D));
	संदेश_putchar(' ');
	संदेश_outtrans(HL_TABLE()[HL_TABLE()[id - 1].sg_link - 1].sg_name);
    }

    जर (!didh)
	highlight_list_arg(id, didh, LIST_STRING, 0, (char_u *)"cleared", "");
#ifdef FEAT_EVAL
    जर (p_verbose > 0)
	last_set_संदेश(sgp->sg_script_ctx);
#endif
}

    स्थिर char_u*
highlight_arg_to_string(
    पूर्णांक		type,
    पूर्णांक		iarg,
    char_u	*sarg,
    char_u	*buf)
{
    जर (type == LIST_INT)
	प्र_लिखो((अक्षर *)buf, "%d", iarg - 1);
    अन्यथा जर (type == LIST_STRING)
	परत sarg;
    अन्यथा // type == LIST_ATTR
    {
	माप_प्रकार buflen;

	buf[0] = NUL;
	buflen = 0;
	साठी (पूर्णांक i = 0; i < (पूर्णांक)ARRAY_LENGTH(highlight_index_tab); ++i)
	{
	    जर (iarg & highlight_index_tab[i]->किल्ली)
	    {
		जर (buflen > 0)
		{
		    STRCPY(buf + buflen, (char_u *)",");
		    ++buflen;
		}
		STRCPY(buf + buflen, highlight_index_tab[i]->मूल्य.string);
		buflen += highlight_index_tab[i]->मूल्य.length;
		iarg &= ~highlight_index_tab[i]->किल्ली;	    // don't want "inverse"/"reverse"
	    }
	}
    }
    परत buf;
}

    स्थिर पूर्णांक
highlight_list_arg(
    पूर्णांक		id,
    पूर्णांक		didh,
    पूर्णांक		type,
    पूर्णांक		iarg,
    char_u	*sarg,
    अक्षर	*name)
{
    char_u	buf[MAX_ATTR_LEN];
    char_u	*ts;

    जर (got_int)
	परत FALSE;

    जर (type == LIST_STRING ? (sarg == रिक्त) : (iarg == 0))
	परत didh;

    ts = highlight_arg_to_string(type, iarg, sarg, buf);

    (शून्य)syn_list_header(didh,
	    (पूर्णांक)(vim_strsize(ts) + STRLEN(name) + 1), id);
    didh = TRUE;
    जर (!got_int)
    {
	जर (*name != NUL)
	{
	    संदेश_puts_attr(name, HL_ATTR(HLF_D));
	    संदेश_puts_attr("=", HL_ATTR(HLF_D));
	}
	संदेश_outtrans(ts);
    }
    परत didh;
}

#जर ((defined(FEAT_EVAL) || defined(FEAT_PRINTER))) && defined(FEAT_SYN_HL)
/*
 * Return "1" if highlight group "id" has attribute "flag".
 * Return NULL otherwise.
 */
    char_u *
highlight_has_attr(
    पूर्णांक		id,
    पूर्णांक		flag,
    पूर्णांक		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    पूर्णांक		attr;

    जर (id <= 0 || id > highlight_ga.ga_len)
	परत रिक्त;

# जर defined(FEAT_GUI) || defined(FEAT_EVAL)
    जर (modec == 'g')
	attr = HL_TABLE()[id - 1].sg_gui;
    अन्यथा
# endif
    {
	जर (modec == 'c')
	    attr = HL_TABLE()[id - 1].sg_cterm;
	अन्यथा
	    attr = HL_TABLE()[id - 1].sg_term;
    }

    जर (attr & flag)
	परत (char_u *)"1";
    परत रिक्त;
}
#endif

#जर defined(FEAT_SYN_HL) && defined(FEAT_EVAL)
/*
 * Return color name of highlight group "id".
 */
    char_u *
highlight_color(
    पूर्णांक		id,
    char_u	*what,	// "font", "fg", "bg", "sp", "ul", "fg#", "bg#" or "sp#"
    पूर्णांक		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    स्थिर char_u	name[20];
    पूर्णांक			n;
    पूर्णांक			fg = FALSE;
    पूर्णांक			sp = FALSE;
    पूर्णांक			ul = FALSE;
    पूर्णांक			font = FALSE;

    जर (id <= 0 || id > highlight_ga.ga_len)
	परत रिक्त;

    जर (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'g')
	fg = TRUE;
    अन्यथा जर (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'o'
	     && TOLOWER_ASC(what[2]) == 'n' && TOLOWER_ASC(what[3]) == 't')
	font = TRUE;
    अन्यथा जर (TOLOWER_ASC(what[0]) == 's' && TOLOWER_ASC(what[1]) == 'p')
	sp = TRUE;
    अन्यथा जर (TOLOWER_ASC(what[0]) == 'u' && TOLOWER_ASC(what[1]) == 'l')
	ul = TRUE;
    अन्यथा जर (!(TOLOWER_ASC(what[0]) == 'b' && TOLOWER_ASC(what[1]) == 'g'))
	परत रिक्त;
    जर (modec == 'g')
    {
# जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI
	// return font name
	जर (font)
	    परत HL_TABLE()[id - 1].sg_font_name;
#  endif

	// return #RRGGBB form (only possible when GUI is running)
	जर ((USE_24BIT) && what[2] == '#')
	{
	    guicolor_T		color;
	    long_u		rgb;
	    स्थिर char_u	buf[10];

	    जर (fg)
		color = HL_TABLE()[id - 1].sg_gui_fg;
	    अन्यथा जर (sp)
		color = HL_TABLE()[id - 1].sg_gui_sp;
	    अन्यथा
		color = HL_TABLE()[id - 1].sg_gui_bg;
	    जर (color == INVALCOLOR)
		परत रिक्त;
	    rgb = (long_u)GUI_MCH_GET_RGB(color);
	    प्र_लिखो((अक्षर *)buf, "#%02x%02x%02x",
				      (अचिन्हित)(rgb >> 16),
				      (अचिन्हित)(rgb >> 8) & 255,
				      (अचिन्हित)rgb & 255);
	    परत buf;
	}
# endif
	जर (fg)
	    परत (HL_TABLE()[id - 1].sg_gui_fg_name);
	जर (sp)
	    परत (HL_TABLE()[id - 1].sg_gui_sp_name);
	परत (HL_TABLE()[id - 1].sg_gui_bg_name);
    }
    जर (sp)
	परत रिक्त;
    जर (modec == 'c')
    {
	जर (fg)
	    n = HL_TABLE()[id - 1].sg_cterm_fg - 1;
	अन्यथा जर (ul)
	    n = HL_TABLE()[id - 1].sg_cterm_ul - 1;
	अन्यथा जर (font)
	    n = HL_TABLE()[id - 1].sg_cterm_font - 1;
	अन्यथा
	    n = HL_TABLE()[id - 1].sg_cterm_bg - 1;
	जर (n < 0)
	    परत रिक्त;
	प्र_लिखो((अक्षर *)name, "%d", n);
	परत name;
    }
    // term doesn't have color
    परत रिक्त;
}
#endif

#जर defined(FEAT_SYN_HL) \
	    && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)) \
	&& defined(FEAT_PRINTER)
/*
 * Return color name of highlight group "id" as RGB value.
 */
    long_u
highlight_gui_color_rgb(
    पूर्णांक		id,
    पूर्णांक		fg)	// TRUE = fg, FALSE = bg
{
    guicolor_T	color;

    जर (id <= 0 || id > highlight_ga.ga_len)
	परत 0L;

    जर (fg)
	color = HL_TABLE()[id - 1].sg_gui_fg;
    अन्यथा
	color = HL_TABLE()[id - 1].sg_gui_bg;

    जर (color == INVALCOLOR)
	परत 0L;

    परत GUI_MCH_GET_RGB(color);
}
#endif

/*
 * Output the syntax list header.
 * Return TRUE when started a new line.
 */
    पूर्णांक
syn_list_header(
    पूर्णांक	    did_header,		// did header already
    पूर्णांक	    outlen,		// length of string that comes
    पूर्णांक	    id)			// highlight group id
{
    पूर्णांक	    endcol = 19;
    पूर्णांक	    newline = TRUE;
    पूर्णांक	    name_col = 0;

    जर (!did_header)
    {
	संदेश_putchar('\n');
	जर (got_int)
	    परत TRUE;
	संदेश_outtrans(HL_TABLE()[id - 1].sg_name);
	name_col = संदेश_col;
	endcol = 15;
    }
    अन्यथा जर (संदेश_col + outlen + 1 >= cmdline_width)
    {
	संदेश_putchar('\n');
	जर (got_int)
	    परत TRUE;
    }
    अन्यथा
    {
	जर (संदेश_col >= endcol)	// wrap around is like starting a new line
	    newline = FALSE;
    }

    जर (संदेश_col >= endcol)	// output at least one space
	endcol = संदेश_col + 1;
    जर (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    संदेश_advance(endcol);

    // Show "xxx" with the attributes.
    जर (!did_header)
    {
	जर (endcol == cmdline_width - 1 && endcol <= name_col)
	    संदेश_putchar(' ');
	संदेश_puts_attr("xxx", syn_id2attr(id));
	संदेश_putchar(' ');
    }

    परत newline;
}

/*
 * Set the attribute numbers for a highlight group.
 * Called after one of the attributes has changed.
 */
    स्थिर शून्य
set_hl_attr(
    पूर्णांक		idx)	    // index in array
{
    attrentry_T	    at_en;
    hl_group_T	    *sgp = HL_TABLE() + idx;

    // The "Normal" group doesn't need an attribute number
    जर (sgp->sg_name_u != रिक्त && STRCMP(sgp->sg_name_u, "NORMAL") == 0)
	परत;

#ifdef FEAT_GUI
    // For the GUI mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    जर (sgp->sg_gui_fg == INVALCOLOR
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
    अन्यथा
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
    जर (sgp->sg_start == रिक्त && sgp->sg_stop == रिक्त)
	sgp->sg_term_attr = sgp->sg_term;
    अन्यथा
    {
	at_en.ae_attr = sgp->sg_term;
	at_en.ae_u.term.start = sgp->sg_start;
	at_en.ae_u.term.stop = sgp->sg_stop;
	sgp->sg_term_attr = get_attr_entry(&term_attr_table, &at_en);
    }

    // For the color term mode: If there are other than "normal"
    // highlighting attributes, need to allocate an attr number.
    जर (sgp->sg_cterm_fg == 0 && sgp->sg_cterm_bg == 0 &&
	sgp->sg_cterm_ul == 0 && sgp->sg_cterm_font == 0
#ifdef FEAT_TERMGUICOLORS
	    && sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
#endif
	    )
	sgp->sg_cterm_attr = sgp->sg_cterm;
    अन्यथा
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
	जर (sgp->sg_cterm & (HL_UNDERLINE | HL_UNDERCURL
			   | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED))
	    at_en.ae_u.cterm.ul_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_sp);
	अन्यथा
	    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	जर (at_en.ae_u.cterm.fg_rgb == INVALCOLOR
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
    स्थिर पूर्णांक
syn_override(पूर्णांक id)
{
    जर (overrides != रिक्त && overrides->arr != रिक्त)
	साठी (पूर्णांक k = 0; k < overrides->len; k++)
	    जर (overrides->arr[k].from == id)
		परत overrides->arr[k].to;
    परत id;
}

/*
 * Lookup a highlight group name by pointer and length.
 * If it is not found, 0 is returned.
 */
    स्थिर पूर्णांक
syn_name2id_len(char_u *name, पूर्णांक len)
{
    char_u	name_u[MAX_SYN_NAME + 1];
    hashitem_T	*hi;

    जर (len > MAX_SYN_NAME)
	len = MAX_SYN_NAME;
    mch_memmove(name_u, name, len);
    name_u[len] = NUL;
    vim_strup(name_u);
    जर (!highlight_ht_inited)
	परत 0;
    hi = hash_find(&highlight_ht, name_u);
    जर (HASHITEM_EMPTY(hi))
	परत 0;
    परत HI2HLNAME(hi)->hn_id;
}

/*
 * Lookup a highlight group name and return its ID.
 * If it is not found, 0 is returned.
 */
    पूर्णांक
syn_name2id(char_u *name)
{
    // Avoid using stricmp() too much, it's slow on some systems.
    परत syn_name2id_len(name, (पूर्णांक)STRLEN(name));
}

/*
 * Lookup a highlight group name and return its attributes.
 * Return zero if not found.
 */
    पूर्णांक
syn_name2attr(char_u *name)
{
    पूर्णांक id = syn_name2id(name);

    जर (id != 0)
	परत syn_id2attr(id);
    परत 0;
}

#जर defined(FEAT_EVAL)
/*
 * Return TRUE if highlight group "name" exists.
 */
    पूर्णांक
highlight_exists(char_u *name)
{
    परत (syn_name2id(name) > 0);
}

# जर defined(FEAT_शोध_EXTRA)
/*
 * Return the name of highlight group "id".
 * When not a valid ID return an empty string.
 */
    char_u *
syn_id2name(पूर्णांक id)
{
    जर (id <= 0 || id > highlight_ga.ga_len)
	परत (char_u *)"";
    परत HL_TABLE()[id - 1].sg_name;
}
# endif
#endif

/*
 * Like syn_name2id(), but take a pointer + length argument.
 */
    पूर्णांक
syn_namen2id(char_u *linep, पूर्णांक len)
{
    परत syn_name2id_len(linep, len);
}

/*
 * Find highlight group name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    पूर्णांक
syn_check_group(char_u *pp, पूर्णांक len)
{
    पूर्णांक	    id;
    char_u  *name;

    जर (len > MAX_SYN_NAME)
    {
	emsg(_(e_highlight_group_name_too_long));
	परत 0;
    }
    id = syn_name2id_len(pp, len);
    जर (id == 0)			// doesn't exist yet
    {
	name = vim_strnsave(pp, len);
	जर (name == रिक्त)
	    परत 0;
	id = syn_add_group(name);
    }
    परत id;
}

/*
 * Add new highlight group and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    स्थिर पूर्णांक
syn_add_group(char_u *name)
{
    char_u	*p;
    char_u	*name_up;

    // Check that the name is valid (ASCII letters, digits, underscores, dots, or hyphens).
    साठी (p = name; *p != NUL; ++p)
    {
	जर (!vim_isprintc(*p))
	{
	    emsg(_(e_unprintable_character_in_group_name));
	    vim_free(name);
	    परत 0;
	}
	अन्यथा जर (!ASCII_ISALNUM(*p) && *p != '_' && *p != '.' && *p != '-')
	{
	    // This is an error, but since there previously was no check only
	    // give a warning.
	    संदेश_source(HL_ATTR(HLF_W));
	    संदेश(_("W18: Invalid character in group name"));
	    थांबा;
	}
    }

    // First call for this growarray: init growing array.
    जर (highlight_ga.ga_data == रिक्त)
    {
	highlight_ga.ga_itemsize = आकार(hl_group_T);
	highlight_ga.ga_growsize = 10;
	hash_init(&highlight_ht);
	highlight_ht_inited = true;
    }

    जर (highlight_ga.ga_len >= MAX_HL_ID)
    {
	emsg(_(e_too_many_highlight_and_syntax_groups));
	vim_free(name);
	परत 0;
    }

    // Make room for at least one other syntax_highlight entry.
    जर (ga_grow(&highlight_ga, 1) == FAIL)
    {
	vim_free(name);
	परत 0;
    }

    {
	hlname_T    *hn;
	पूर्णांक	    len = (पूर्णांक)STRLEN(name);

	hn = alloc(दुरत्व(hlname_T, hn_किल्ली) + len + 1);
	जर (hn == रिक्त)
	{
	    vim_free(name);
	    परत 0;
	}
	vim_strncpy(hn->hn_किल्ली, name, len);
	vim_strup(hn->hn_किल्ली);
	hn->hn_id = highlight_ga.ga_len + 1;	// ID is index plus one
	name_up = hn->hn_किल्ली;
    }

    CLEAR_POINTER(&(HL_TABLE()[highlight_ga.ga_len]));
    HL_TABLE()[highlight_ga.ga_len].sg_name = name;
    HL_TABLE()[highlight_ga.ga_len].sg_name_u = name_up;
#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_TABLE()[highlight_ga.ga_len].sg_gui_bg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_fg = INVALCOLOR;
    HL_TABLE()[highlight_ga.ga_len].sg_gui_sp = INVALCOLOR;
#endif
    hash_add(&highlight_ht, name_up, "highlight");
    ++highlight_ga.ga_len;

    परत highlight_ga.ga_len;		    // ID is index plus one
}

/*
 * When, just after calling syn_add_group(), an error is discovered, this
 * function deletes the new name.
 */
    स्थिर शून्य
syn_unadd_group(शून्य)
{
    hashitem_T *hi;

    --highlight_ga.ga_len;
    hi = hash_find(&highlight_ht, HL_TABLE()[highlight_ga.ga_len].sg_name_u);
    जर (!HASHITEM_EMPTY(hi))
	hash_remove(&highlight_ht, hi, "highlight");
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name);
    vim_free(HL_TABLE()[highlight_ga.ga_len].sg_name_u - HLNAME_किल्ली_OFF);
}

/*
 * Translate a group ID to highlight attributes.
 * "hl_id" must be valid: > 0, caller must check.
 */
    पूर्णांक
syn_id2attr(पूर्णांक hl_id)
{
    पूर्णांक		attr;
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    प्रतिज्ञा(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

#ifdef FEAT_GUI
    // Only use GUI attr when the GUI is being used.
    जर (gui.in_use)
	attr = sgp->sg_gui_attr;
    अन्यथा
#endif
	जर (IS_CTERM)
	    attr = sgp->sg_cterm_attr;
	अन्यथा
	    attr = sgp->sg_term_attr;

    परत attr;
}

#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Get the GUI colors and attributes for a group ID.
 * NOTE: the colors will be INVALCOLOR when not set, the color otherwise.
 */
    पूर्णांक
syn_id2colors(पूर्णांक hl_id, guicolor_T *fgp, guicolor_T *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    प्रतिज्ञा(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one

    *fgp = sgp->sg_gui_fg;
    *bgp = sgp->sg_gui_bg;
    परत sgp->sg_gui;
}
#endif

#जर (defined(MSWIN) \
	    && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL)) \
	    && defined(FEAT_TERMGUICOLORS)) \
	|| defined(FEAT_TERMINAL)
    शून्य
syn_id2cterm_bg(पूर्णांक hl_id, पूर्णांक *fgp, पूर्णांक *bgp)
{
    hl_group_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    प्रतिज्ञा(hl_id > 0);
    sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
    *fgp = sgp->sg_cterm_fg - 1;
    *bgp = sgp->sg_cterm_bg - 1;
}
#endif

/*
 * Translate a group ID to the final group ID (following links).
 */
    पूर्णांक
syn_get_final_id(पूर्णांक hl_id)
{
    पूर्णांक		मोजणी;
    hl_group_T	*sgp;

    जर (hl_id > highlight_ga.ga_len || hl_id < 1)
	परत 0;			// Can be called from eval!!

    // Follow links until there is no more.
    // Look out for loops!  Break after 100 links.
    साठी (मोजणी = 100; --मोजणी >= 0; )
    {
	पूर्णांक tmp = hl_id;

	sgp = &HL_TABLE()[hl_id - 1];	    // index is ID minus one
	जर (sgp->sg_link == 0 || sgp->sg_link > highlight_ga.ga_len)
	    थांबा;

	// This is to handle highlight groups that are overridden but are in the
	// middle of a link chain.
	hl_id = syn_override(hl_id);
	जर (tmp != hl_id)
	    सुरूठेवा;
	hl_id = sgp->sg_link;
    }
    परत syn_override(hl_id);
}

#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Call this function just after the GUI has started.
 * Also called when 'termguicolors' was set, gui.in_use will be FALSE then.
 * It finds the font and color handles for the highlighting groups.
 */
    शून्य
highlight_gui_started(शून्य)
{
    पूर्णांक	    idx;

    // First get the colors from the "Normal" and "Menu" group, if set
    जर (USE_24BIT)
	set_normal_colors();

    साठी (idx = 0; idx < highlight_ga.ga_len; ++idx)
	gui_do_one_color(idx, FALSE, FALSE);

    highlight_changed();
}

    स्थिर शून्य
gui_do_one_color(
    पूर्णांक		idx,
    पूर्णांक		do_menu UNUSED,	   // TRUE: might set the menu font
    पूर्णांक		do_tooltip UNUSED) // TRUE: might set the tooltip font
{
    पूर्णांक		didit = FALSE;

# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    जर (gui.in_use)
#  endif
	जर (HL_TABLE()[idx].sg_font_name != रिक्त)
	{
	    hl_do_font(idx, HL_TABLE()[idx].sg_font_name, FALSE, do_menu,
							    do_tooltip, TRUE);
	    didit = TRUE;
	}
# endif
    जर (HL_TABLE()[idx].sg_gui_fg_name != रिक्त)
    {
	HL_TABLE()[idx].sg_gui_fg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_fg_name);
	didit = TRUE;
    }
    जर (HL_TABLE()[idx].sg_gui_bg_name != रिक्त)
    {
	HL_TABLE()[idx].sg_gui_bg =
			    color_name2handle(HL_TABLE()[idx].sg_gui_bg_name);
	didit = TRUE;
    }
    जर (HL_TABLE()[idx].sg_gui_sp_name != रिक्त)
    {
	HL_TABLE()[idx].sg_gui_sp =
			    color_name2handle(HL_TABLE()[idx].sg_gui_sp_name);
	didit = TRUE;
    }
    जर (didit)	// need to get a new attr number
	set_hl_attr(idx);
}
#endif

#जर defined(USER_HIGHLIGHT) && defined(FEAT_STL_OPT)
/*
 * Apply difference between User[1-9] and HLF_S to HLF_SNC, HLF_ST or HLF_STNC.
 */
    स्थिर शून्य
combine_stl_hlt(
	पूर्णांक id,
	पूर्णांक id_S,
	पूर्णांक id_alt,
	पूर्णांक hlcnt,
	पूर्णांक i,
	पूर्णांक hlf,
	पूर्णांक *table)
{
    hl_group_T *hlt = HL_TABLE();

    जर (id_alt == 0)
    {
	CLEAR_POINTER(&hlt[hlcnt + i]);
	hlt[hlcnt + i].sg_term = highlight_attr[hlf];
	hlt[hlcnt + i].sg_cterm = highlight_attr[hlf];
# जर defined(FEAT_GUI) || defined(FEAT_EVAL)
	hlt[hlcnt + i].sg_gui = highlight_attr[hlf];
# endif
    }
    अन्यथा
	mch_memmove(&hlt[hlcnt + i],
		    &hlt[id_alt - 1],
		    आकार(hl_group_T));
    hlt[hlcnt + i].sg_link = 0;

    hlt[hlcnt + i].sg_term ^=
	hlt[id - 1].sg_term ^ hlt[id_S - 1].sg_term;
    जर (hlt[id - 1].sg_start != hlt[id_S - 1].sg_start)
	hlt[hlcnt + i].sg_start = hlt[id - 1].sg_start;
    जर (hlt[id - 1].sg_stop != hlt[id_S - 1].sg_stop)
	hlt[hlcnt + i].sg_stop = hlt[id - 1].sg_stop;
    hlt[hlcnt + i].sg_cterm ^=
	hlt[id - 1].sg_cterm ^ hlt[id_S - 1].sg_cterm;
    जर (hlt[id - 1].sg_cterm_fg != hlt[id_S - 1].sg_cterm_fg)
	hlt[hlcnt + i].sg_cterm_fg = hlt[id - 1].sg_cterm_fg;
    जर (hlt[id - 1].sg_cterm_bg != hlt[id_S - 1].sg_cterm_bg)
	hlt[hlcnt + i].sg_cterm_bg = hlt[id - 1].sg_cterm_bg;
    जर (hlt[id - 1].sg_cterm_font != hlt[id_S - 1].sg_cterm_font)
	hlt[hlcnt + i].sg_cterm_font = hlt[id - 1].sg_cterm_font;
# जर defined(FEAT_GUI) || defined(FEAT_EVAL)
    hlt[hlcnt + i].sg_gui ^=
	hlt[id - 1].sg_gui ^ hlt[id_S - 1].sg_gui;
# endif
# ifdef FEAT_GUI
    जर (hlt[id - 1].sg_gui_fg != hlt[id_S - 1].sg_gui_fg)
	hlt[hlcnt + i].sg_gui_fg = hlt[id - 1].sg_gui_fg;
    जर (hlt[id - 1].sg_gui_bg != hlt[id_S - 1].sg_gui_bg)
	hlt[hlcnt + i].sg_gui_bg = hlt[id - 1].sg_gui_bg;
    जर (hlt[id - 1].sg_gui_sp != hlt[id_S - 1].sg_gui_sp)
	hlt[hlcnt + i].sg_gui_sp = hlt[id - 1].sg_gui_sp;
    जर (hlt[id - 1].sg_font != hlt[id_S - 1].sg_font)
	hlt[hlcnt + i].sg_font = hlt[id - 1].sg_font;
#  ifdef FEAT_XFONTSET
    जर (hlt[id - 1].sg_fontset != hlt[id_S - 1].sg_fontset)
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
    पूर्णांक
highlight_changed(शून्य)
{
    पूर्णांक		hlf;
    पूर्णांक		i;
    char_u	*p;
    char_u	*default_hl;
    पूर्णांक		attr;
    char_u	*end;
    पूर्णांक		id;
#ifdef USER_HIGHLIGHT
    char_u      userhl[30];  // use 30 to avoid compiler warning
# ifdef FEAT_STL_OPT
    पूर्णांक		id_S = -1;
    पूर्णांक		id_SNC = 0;
#  ifdef FEAT_TERMINAL
    पूर्णांक		id_ST = 0;
    पूर्णांक		id_STNC = 0;
#  endif
    पूर्णांक		hlcnt;
# endif
#endif
    win_T	*wp;

    need_highlight_changed = FALSE;

#ifdef FEAT_TERMINAL
    term_update_colors_all();
    term_update_hlfwin_all();
#endif

    // Clear all attributes.
    साठी (hlf = 0; hlf < (पूर्णांक)HLF_मोजणी; ++hlf)
    {
	highlight_attr[hlf] = 0;
	highlight_attr_raw[hlf] = 0;
	highlight_ids[hlf] = 0;
    }

    default_hl = get_highlight_default();

    // First set all attributes to their default value.
    // Then use the attributes from the 'highlight' option, unless it already
    // matches the default and would repeat the same work.
    साठी (i = 0; i < 2; ++i)
    {
	जर (i)
	{
	    जर (default_hl != रिक्त && p_hl != रिक्त
				      && STRCMP(default_hl, p_hl) == 0)
		सुरूठेवा;
	    p = p_hl;
	}
	अन्यथा
	    p = default_hl;
	जर (p == रिक्त)	    // just in case
	    सुरूठेवा;

	पर्यंत (*p)
	{
	    साठी (hlf = 0; hlf < (पूर्णांक)HLF_मोजणी; ++hlf)
		जर (hl_flags[hlf] == *p)
		    थांबा;
	    ++p;
	    जर (hlf == (पूर्णांक)HLF_मोजणी || *p == NUL)
		परत FAIL;

	    // Allow several hl_flags to be combined, like "bu" for
	    // bold-underlined.
	    attr = 0;
	    id = 0;
	    साठी ( ; *p && *p != ','; ++p)	    // parse up to comma
	    {
		जर (VIM_ISWHITE(*p))		    // ignore white space
		    सुरूठेवा;

		जर (attr > HL_ALL)  // Combination with ':' is not allowed.
		    परत FAIL;

		// Note: Keep this in sync with expand_set_highlight().
		निवड (*p)
		{
		    स्थिती 'b':	attr |= HL_BOLD;
				थांबा;
		    स्थिती 'i':	attr |= HL_ITALIC;
				थांबा;
		    स्थिती '-':
		    स्थिती 'n':			    // no highlighting
				थांबा;
		    स्थिती 'r':	attr |= HL_INVERSE;
				थांबा;
		    स्थिती 's':	attr |= HL_STANDOUT;
				थांबा;
		    स्थिती 'u':	attr |= HL_UNDERLINE;
				थांबा;
		    स्थिती 'c':	attr |= HL_UNDERCURL;
				थांबा;
		    स्थिती '2':	attr |= HL_UNDERDOUBLE;
				थांबा;
		    स्थिती 'd':	attr |= HL_UNDERDOTTED;
				थांबा;
		    स्थिती '=':	attr |= HL_UNDERDASHED;
				थांबा;
		    स्थिती 't':	attr |= HL_STRIKETHROUGH;
				थांबा;
		    स्थिती ':':	++p;		    // highlight group name
				जर (attr || *p == NUL)	 // no combinations
				    परत FAIL;
				end = vim_strchr(p, ',');
				जर (end == रिक्त)
				    end = p + STRLEN(p);
				id = syn_check_group(p, (पूर्णांक)(end - p));
				जर (id == 0)
				    परत FAIL;
				attr = syn_id2attr(id);
				p = end - 1;
#जर defined(FEAT_STL_OPT) && defined(USER_HIGHLIGHT)
				जर (hlf == (पूर्णांक)HLF_SNC)
				    id_SNC = syn_get_final_id(id);
# ifdef FEAT_TERMINAL
				अन्यथा जर (hlf == (पूर्णांक)HLF_ST)
				    id_ST = syn_get_final_id(id);
				अन्यथा जर (hlf == (पूर्णांक)HLF_STNC)
				    id_STNC = syn_get_final_id(id);
# endif
				अन्यथा जर (hlf == (पूर्णांक)HLF_S)
				    id_S = syn_get_final_id(id);
#endif
				थांबा;
		    पूर्वनिर्धारित:	परत FAIL;
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
	अक्षर *errmsg = update_winhighlight(wp, wp->w_p_whl);

	जर (errmsg != रिक्त)
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
    जर (ga_grow(&highlight_ga, 28) == FAIL)
	परत FAIL;
    hlcnt = highlight_ga.ga_len;
    जर (id_S == -1)
    {
	// Make sure id_S is always valid to simplify code below. Use the last
	// entry.
	CLEAR_POINTER(&HL_TABLE()[hlcnt + 27]);
	HL_TABLE()[hlcnt + 18].sg_term = highlight_attr[HLF_S];
	id_S = hlcnt + 19;
    }
# endif
    साठी (i = 0; i < 9; i++)
    {
	प्र_लिखो((अक्षर *)userhl, "User%d", i + 1);
	id = syn_name2id(userhl);
	जर (id == 0)
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
	अन्यथा
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

    परत OK;
}

स्थिर शून्य highlight_list(शून्य);
स्थिर शून्य highlight_list_two(पूर्णांक cnt, पूर्णांक attr);

// context for :highlight <group> <arg> expansion
स्थिर पूर्णांक expand_hi_synid = 0;	    // ID for highlight group being completed
स्थिर पूर्णांक expand_hi_equal_col = 0; // column where the '=' is
स्थिर पूर्णांक expand_hi_include_orig = 0;	    // whether to fill the existing current value or not
स्थिर char_u *expand_hi_curvalue = रिक्त;   // the existing current value
#जर defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
स्थिर dict_iterator_T expand_colornames_iter;	// iterator for looping through v:colornames
#endif

/*
 * Handle command line completion for :highlight command.
 */
    शून्य
set_context_in_highlight_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;
    पूर्णांक		expand_group = TRUE;

    // Default: expand group names
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 0;
    include_link = 2;
    include_default = 1;

    जर (*arg == NUL)
	परत;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    जर (*p == NUL)
	परत;

    // past "default" or group name
    include_default = 0;
    जर (STRNCMP("default", arg, p - arg) == 0)
    {
	arg = skipwhite(p);
	xp->xp_pattern = arg;
	p = skiptowhite(arg);
    }
    जर (*p == NUL)
	परत;

    // past group name
    include_link = 0;
    जर (arg[1] == 'i' && arg[0] == 'N')
    {
	highlight_list();
	expand_group = FALSE;
    }
    जर (STRNCMP("link", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	जर (*p != NUL)		// past first group name
	{
	    xp->xp_pattern = skipwhite(p);
	    p = skiptowhite(xp->xp_pattern);
	    include_none = 1;
	}
	expand_group = FALSE;
    }
    अन्यथा जर (STRNCMP("clear", arg, p - arg) == 0)
    {
	xp->xp_pattern = skipwhite(p);
	p = skiptowhite(xp->xp_pattern);
	expand_group = FALSE;
    }
    जर (*p != NUL)			// past group name(s)
    {
	जर (expand_group)
	{
	    // expansion will be done in expand_highlight_group()
	    xp->xp_context = EXPAND_HIGHLIGHT_GROUP;

	    expand_hi_synid = syn_namen2id(arg, (पूर्णांक)(p - arg));

	    पर्यंत (*p != NUL)
	    {
		arg = skipwhite(p);
		p = skiptowhite(arg);
	    }

	    p = vim_strchr(arg, '=');
	    जर (p == रिक्त)
	    {
		// Didn't find a key=<value> pattern
		xp->xp_pattern = arg;
		expand_hi_equal_col = -1;
		expand_hi_include_orig = FALSE;
	    }
	    अन्यथा
	    {
		// Found key=<value> pattern, record the exact location
		expand_hi_equal_col = (पूर्णांक)(p - xp->xp_ओळ);

		// Only include the original value if the pattern is empty
		जर (*(p + 1) == NUL)
		    expand_hi_include_orig = TRUE;
		अन्यथा
		    expand_hi_include_orig = FALSE;

		// Account for comma-separated values
		जर (STRNCMP(arg, "term=", 5) == 0 ||
			STRNCMP(arg, "cterm=", 6) == 0 ||
			STRNCMP(arg, "gui=", 4) == 0)
		{
		    char_u *comma = vim_strrchr(p + 1, ',');
		    जर (comma != रिक्त)
			p = comma;
		}
		xp->xp_pattern = p + 1;
	    }
	}
	अन्यथा
	{
	    xp->xp_context = EXPAND_NOTHING;
	}
    }
}

/*
 * List highlighting matches in a nice way.
 */
    स्थिर शून्य
highlight_list(शून्य)
{
    पूर्णांक		i;

    साठी (i = 10; --i >= 0; )
	highlight_list_two(i, HL_ATTR(HLF_D));
    साठी (i = 40; --i >= 0; )
	highlight_list_two(99, 0);
}

    स्थिर शून्य
highlight_list_two(पूर्णांक cnt, पूर्णांक attr)
{
    संदेश_puts_attr(&("N \bI \b!  \b"[cnt / 11]), attr);
    संदेश_clr_eos();
    out_flush();
    ui_delay(cnt == 99 ? 40L : (दीर्घ)cnt * 50L, FALSE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_highlight_name(expand_T *xp UNUSED, पूर्णांक idx)
{
    परत get_highlight_name_ext(xp, idx, TRUE);
}

/*
 * Obtain a highlight group name.
 * When "skip_cleared" is TRUE don't return a cleared entry.
 */
    char_u *
get_highlight_name_ext(expand_T *xp UNUSED, पूर्णांक idx, पूर्णांक skip_cleared)
{
    जर (idx < 0)
	परत रिक्त;

    // Items are never removed from the table, skip the ones that were
    // cleared.
    जर (skip_cleared && idx < highlight_ga.ga_len && HL_TABLE()[idx].sg_cleared)
	परत (char_u *)"";

    जर (idx == highlight_ga.ga_len && include_none != 0)
	परत (char_u *)"NONE";
    जर (idx == highlight_ga.ga_len + include_none && include_default != 0)
	परत (char_u *)"default";
    जर (idx == highlight_ga.ga_len + include_none + include_default
							 && include_link != 0)
	परत (char_u *)"link";
    जर (idx == highlight_ga.ga_len + include_none + include_default + 1
							 && include_link != 0)
	परत (char_u *)"clear";
    जर (idx >= highlight_ga.ga_len)
	परत रिक्त;
    परत HL_TABLE()[idx].sg_name;
}

    स्थिर char_u *
get_highlight_attr_name(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx == 0)
    {
	// Fill with current value first
	जर (expand_hi_curvalue != रिक्त)
	    परत expand_hi_curvalue;
	अन्यथा
	    परत (char_u*)"";
    }
    जर (idx < (पूर्णांक)ARRAY_LENGTH(highlight_index_tab) + 1)
    {
	char_u *मूल्य = highlight_index_tab[idx-1]->मूल्य.string;
	जर (expand_hi_curvalue != रिक्त && STRCMP(expand_hi_curvalue, मूल्य) == 0)
	{
	    // Already returned the current value above, just skip.
	    परत (char_u*)"";
	}
	परत मूल्य;
    }
    परत रिक्त;
}

    स्थिर char_u *
get_highlight_cterm_color(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx == 0)
    {
	// Fill with current value first
	जर (expand_hi_curvalue != रिक्त)
	    परत expand_hi_curvalue;
	अन्यथा
	    परत (char_u*)"";
    }
    // See highlight_set_cterm_color()
    अन्यथा जर (idx == 1)
	परत (char_u*)"fg";
    अन्यथा जर (idx == 2)
	परत (char_u*)"bg";
    जर (idx < (पूर्णांक)ARRAY_LENGTH(color_name_tab) + 3)
    {
	char_u *मूल्य = color_name_tab[idx-3].मूल्य.string;
	परत मूल्य;
    }
    परत रिक्त;
}

#जर defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
    स्थिर char_u *
get_highlight_gui_color(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx == 0)
    {
	// Fill with current value first
	जर (expand_hi_curvalue != रिक्त)
	    परत expand_hi_curvalue;
	अन्यथा
	    परत (char_u*)"";
    }
    // See color_name2handle()
    अन्यथा जर (idx == 1)
	परत (char_u*)"fg";
    अन्यथा जर (idx == 2)
	परत (char_u*)"bg";
    अन्यथा जर (idx == 3)
	परत (char_u*)"NONE";

    // Complete from v:colornames. Don't do platform specific names for now.
    typval_T *tv_result;
    char_u *colorname = dict_iterate_next(&expand_colornames_iter, &tv_result);
    जर (colorname != रिक्त)
    {
	// :hi command doesn't allow space, so don't suggest any malformed items
	जर (vim_strchr(colorname, ' ') != रिक्त)
	    परत (char_u*)"";

	जर (expand_hi_curvalue != रिक्त && STRICMP(expand_hi_curvalue, colorname) == 0)
	{
	    // Already returned the current value above, just skip.
	    परत (char_u*)"";
	}
    }
    परत colorname;
}
#endif

    स्थिर char_u *
get_highlight_group_किल्ली(expand_T *xp UNUSED, पूर्णांक idx)
{
    // Note: Keep this in sync with do_highlight.
    स्थिर अक्षर *(p_hi_group_किल्ली_मूल्ये[]) =
    {
	"term=",
	"start=",
	"stop=",
	"cterm=",
	"ctermfg=",
	"ctermbg=",
	"ctermul=",
	"ctermfont=",
#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
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

    जर (idx < (पूर्णांक)ARRAY_LENGTH(p_hi_group_किल्ली_मूल्ये))
	परत (char_u*)p_hi_group_किल्ली_मूल्ये[idx];
    परत रिक्त;
}

/*
 * Command-line expansion for :hi {group-name} <args>...
 */
    पूर्णांक
expand_highlight_group(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	पूर्णांक	    *अंक_Matches)
{
    जर (expand_hi_equal_col != -1)
    {
	// List the values. First fill in the current value, then if possible colors
	// or attribute names.
	char_u	    *(*expandfunc)(expand_T *, पूर्णांक) = रिक्त;
	पूर्णांक	    type = 0;
	hl_group_T  *sgp = रिक्त;
	पूर्णांक	    iarg = 0;
	char_u	    *sarg = रिक्त;

	पूर्णांक	    unsorted_वस्तुसंच = -1; // don't sort by default

	जर (expand_hi_synid != 0)
	    sgp = &HL_TABLE()[expand_hi_synid - 1]; // index is ID minus one

	// Note: Keep this in sync with highlight_list_one().
	char_u	    *name_end = xp->xp_ओळ + expand_hi_equal_col;
	जर (name_end - xp->xp_ओळ >= 5
		&& STRNCMP(name_end - 5, " term", 5) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    जर (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_term;
	    }
	}
	अन्यथा जर (name_end - xp->xp_ओळ >= 6
		&& STRNCMP(name_end - 6, " cterm", 6) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    जर (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_cterm;
	    }
	}
#जर defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	अन्यथा जर (name_end - xp->xp_ओळ >= 4
		&& STRNCMP(name_end - 4, " gui", 4) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    जर (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_gui;
	    }
	}
#endif
	अन्यथा जर (name_end - xp->xp_ओळ >= 8
		&& STRNCMP(name_end - 8, " ctermfg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    जर (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_fg;
	    }
	}
	अन्यथा जर (name_end - xp->xp_ओळ >= 8
		&& STRNCMP(name_end - 8, " ctermbg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    जर (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_bg;
	    }
	}
	अन्यथा जर (name_end - xp->xp_ओळ >= 8
		&& STRNCMP(name_end - 8, " ctermul", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    जर (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_ul;
	    }
	}
#जर defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	अन्यथा जर (name_end - xp->xp_ओळ >= 6
		&& STRNCMP(name_end - 6, " guifg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    जर (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_fg_name;
	    }
	}
	अन्यथा जर (name_end - xp->xp_ओळ >= 6
		&& STRNCMP(name_end - 6, " guibg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    जर (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_bg_name;
	    }
	}
	अन्यथा जर (name_end - xp->xp_ओळ >= 6
		&& STRNCMP(name_end - 6, " guisp", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    जर (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_sp_name;
	    }
	}
#endif

#जर defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	जर (expandfunc == get_highlight_gui_color)
	{
	    // Top 4 items are special, after that sort all the color names
	    unsorted_वस्तुसंच = 4;

	    dict_T *colornames_table = get_vim_var_dict(VV_COLORNAMES);
	    typval_T colornames_val;
	    colornames_val.v_type = VAR_DICT;
	    colornames_val.vval.v_dict = colornames_table;
	    dict_iterate_start(&colornames_val, &expand_colornames_iter);
	}
#endif

	char_u	    buf[MAX_ATTR_LEN];

	expand_hi_curvalue = रिक्त;
	जर (expand_hi_include_orig)
	{
	    जर (((type == LIST_ATTR || type == LIST_INT) && iarg != 0) ||
		(type == LIST_STRING && sarg != रिक्त))
	    {
		// Retrieve the current value to go first in completion
		expand_hi_curvalue = highlight_arg_to_string(
			type, iarg, sarg, buf);
	    }
	}

	जर (expandfunc != रिक्त)
	{
	    परत ExpandGenericExt(
		    pat,
		    xp,
		    rmp,
		    matches,
		    अंक_Matches,
		    expandfunc,
		    FALSE,
		    unsorted_वस्तुसंच);
	}

	परत FAIL;
    }

    // List all the key names
    परत ExpandGenericExt(
	    pat,
	    xp,
	    rmp,
	    matches,
	    अंक_Matches,
	    get_highlight_group_किल्ली,
	    FALSE,
	    -1);
}

#जर defined(FEAT_GUI)
/*
 * Free all the highlight group fonts.
 * Used when quitting for systems which need it.
 */
    शून्य
free_highlight_fonts(शून्य)
{
    पूर्णांक	    idx;

    साठी (idx = 0; idx < highlight_ga.ga_len; ++idx)
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

#जर defined(FEAT_EVAL)
/*
 * Convert each of the highlight attribute bits (bold, standout, underline,
 * etc.) set in 'hlattr' into a separate boolean item in a Dictionary with
 * the attribute name as the key.
 */
    स्थिर dict_T *
highlight_get_attr_dict(पूर्णांक hlattr)
{
    dict_T	*dict;
    पूर्णांक		i;

    dict = dict_alloc();
    जर (dict == रिक्त)
	परत रिक्त;

    साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(highlight_index_tab); ++i)
    {
	जर (hlattr & highlight_index_tab[i]->किल्ली)
	{
	    dict_add_bool(dict, (अक्षर *)highlight_index_tab[i]->मूल्य.string,
		    VVAL_TRUE);
	    // don't want "inverse"/"reverse"
	    hlattr &= ~highlight_index_tab[i]->किल्ली;
	}
    }

    परत dict;
}

/*
 * Return the attributes of the highlight group at index 'hl_idx' as a
 * Dictionary. If 'resolve_link' is TRUE, then resolves the highlight group
 * links recursively.
 */
    स्थिर dict_T *
highlight_get_info(पूर्णांक hl_idx, पूर्णांक resolve_link)
{
    dict_T	*dict;
    hl_group_T	*sgp;
    dict_T	*attr_dict = रिक्त;
    पूर्णांक		hlgid;

    dict = dict_alloc();
    जर (dict == रिक्त)
	परत dict;

    sgp = &HL_TABLE()[hl_idx];
    // highlight group id is 1-based
    hlgid = hl_idx + 1;

    जर (dict_add_string(dict, "name", sgp->sg_name) == FAIL)
	जा error;
    जर (dict_add_संख्या(dict, "id", hlgid) == FAIL)
	जा error;

    जर (resolve_link)
    {
	// resolve the highlight group link recursively
	पर्यंत (sgp->sg_link)
	{
	    hlgid = sgp->sg_link;
	    sgp = &HL_TABLE()[sgp->sg_link - 1];
	}
    }

    जर (sgp->sg_term != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_term);
	जर (attr_dict != रिक्त)
	{
	    जर (dict_add_dict(dict, "term", attr_dict) == FAIL)
		जा error;
	    attr_dict = रिक्त;
	}
    }
    जर (sgp->sg_start != रिक्त)
	जर (dict_add_string(dict, "start", sgp->sg_start) == FAIL)
	    जा error;
    जर (sgp->sg_stop != रिक्त)
	जर (dict_add_string(dict, "stop", sgp->sg_stop) == FAIL)
	    जा error;
    जर (sgp->sg_cterm != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_cterm);
	जर (attr_dict != रिक्त)
	{
	    जर (dict_add_dict(dict, "cterm", attr_dict) == FAIL)
		जा error;
	    attr_dict = रिक्त;
	}
    }
    जर (sgp->sg_cterm_fg != 0)
	जर (dict_add_string(dict, "ctermfg",
		    highlight_color(hlgid, (char_u *)"fg", 'c')) == FAIL)
	    जा error;
    जर (sgp->sg_cterm_bg != 0)
	जर (dict_add_string(dict, "ctermbg",
			highlight_color(hlgid, (char_u *)"bg", 'c')) == FAIL)
	    जा error;
    जर (sgp->sg_cterm_ul != 0)
	जर (dict_add_string(dict, "ctermul",
			highlight_color(hlgid, (char_u *)"ul", 'c')) == FAIL)
	    जा error;
    जर (sgp->sg_cterm_font != 0)
	जर (dict_add_string(dict, "ctermfont",
			highlight_color(hlgid, (char_u *)"font", 'c')) == FAIL)
	    जा error;
    जर (sgp->sg_gui != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_gui);
	जर (attr_dict != रिक्त)
	{
	    जर (dict_add_dict(dict, "gui", attr_dict) == FAIL)
		जा error;
	    attr_dict = रिक्त;
	}
    }
    जर (sgp->sg_gui_fg_name != रिक्त)
	जर (dict_add_string(dict, "guifg",
			highlight_color(hlgid, (char_u *)"fg", 'g')) == FAIL)
	    जा error;
    जर (sgp->sg_gui_bg_name != रिक्त)
	जर (dict_add_string(dict, "guibg",
			highlight_color(hlgid, (char_u *)"bg", 'g')) == FAIL)
	    जा error;
    जर (sgp->sg_gui_sp_name != रिक्त)
	जर (dict_add_string(dict, "guisp",
			highlight_color(hlgid, (char_u *)"sp", 'g')) == FAIL)
	    जा error;
# ifdef FEAT_GUI
    जर (sgp->sg_font_name != रिक्त)
	जर (dict_add_string(dict, "font", sgp->sg_font_name) == FAIL)
	    जा error;
# endif
    जर (sgp->sg_link)
    {
	char_u	*link;

	link = HL_TABLE()[sgp->sg_link - 1].sg_name;
	जर (link != रिक्त && dict_add_string(dict, "linksto", link) == FAIL)
	    जा error;

	जर (sgp->sg_deflink)
	    dict_add_bool(dict, "default", VVAL_TRUE);
    }
    जर (dict_len(dict) == 2)
	// If only 'name' is present, then the highlight group is cleared.
	dict_add_bool(dict, "cleared", VVAL_TRUE);

    परत dict;

error:
    dict_unref(attr_dict);
    dict_unref(dict);
    परत रिक्त;
}

/*
 * "hlget([name])" function
 * Return the attributes of a specific highlight group (if specified) or all
 * the highlight groups.
 */
    शून्य
f_hlget(typval_T *argvars, typval_T *rettv)
{
    list_T	*list;
    dict_T	*dict;
    पूर्णांक		i;
    char_u	*hlarg = रिक्त;
    पूर्णांक		resolve_link = FALSE;

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    जर (check_for_opt_string_arg(argvars, 0) == FAIL
	    || (argvars[0].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_arg(argvars, 1) == FAIL))
	परत;

    जर (argvars[0].v_type != VAR_UNKNOWN)
    {
	// highlight group name supplied
	hlarg = tv_get_string_chk(&argvars[0]);
	जर (hlarg == रिक्त)
	    परत;

	जर (argvars[1].v_type != VAR_UNKNOWN)
	{
	    पूर्णांक error = FALSE;

	    resolve_link = tv_get_bool_chk(&argvars[1], &error);
	    जर (error)
		परत;
	}
    }

    list = rettv->vval.v_list;
    साठी (i = 0; i < highlight_ga.ga_len && !got_int; ++i)
    {
	जर (hlarg == रिक्त || STRICMP(hlarg, HL_TABLE()[i].sg_name) == 0)
	{
	    dict = highlight_get_info(i, resolve_link);
	    जर (dict != रिक्त && list_append_dict(list, dict) == FAIL)
		dict_unref(dict);
	}
    }
}

/*
 * Returns the string value at 'dict[key]'. Returns NULL, if 'key' is not in
 * 'dict' or the value is not a string type. If the value is not a string type
 * or is NULL, then 'error' is set to TRUE.
 */
    स्थिर char_u *
hldict_get_string(dict_T *dict, char_u *किल्ली, पूर्णांक *error)
{
    dictitem_T	*di;

    *error = FALSE;
    di = dict_find(dict, किल्ली, -1);
    जर (di == रिक्त)
	परत रिक्त;

    जर (di->di_tv.v_type != VAR_STRING || di->di_tv.vval.v_string == रिक्त)
    {
	emsg(_(e_string_required));
	*error = TRUE;
	परत रिक्त;
    }

    परत di->di_tv.vval.v_string;
}

/*
 * Convert the highlight attribute Dictionary at 'dict[key]' into a string
 * value in 'attr_str' of length 'len'. Returns FALSE if 'dict[key]' is not a
 * Dictionary or is NULL.
 */
    स्थिर पूर्णांक
hldict_attr_to_str(
	dict_T	*dict,
	char_u	*किल्ली,
	char_u	*attr_str,
	माप_प्रकार	len)
{
    dictitem_T	*di;
    dict_T	*attrdict;
    पूर्णांक		i;
    char_u	*p;

    attr_str[0] = NUL;
    di = dict_find(dict, किल्ली, -1);
    जर (di == रिक्त)
	परत TRUE;

    जर (di->di_tv.v_type != VAR_DICT || di->di_tv.vval.v_dict == रिक्त)
    {
	emsg(_(e_dictionary_required));
	परत FALSE;
    }

    attrdict = di->di_tv.vval.v_dict;

    // If the attribute dict is empty, then return NONE to clear the attributes
    जर (dict_len(attrdict) == 0)
    {
	vim_strcat(attr_str, (char_u *)"NONE", len);
	परत TRUE;
    }

    p = attr_str;
    साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(highlight_tab); ++i)
    {
	जर (dict_get_bool(attrdict, (अक्षर *)highlight_tab[i].मूल्य.string,
		    VVAL_FALSE) == VVAL_TRUE)
	{
	    जर (p != attr_str && (माप_प्रकार)(p - attr_str + 2) < len)
		STRCPY(p, (char_u *)",");
	    जर (p - attr_str + highlight_tab[i].मूल्य.length + 1 < len)
	    {
		STRCPY(p, highlight_tab[i].मूल्य.string);
		p += highlight_tab[i].मूल्य.length;
	    }
	}
    }

    परत TRUE;
}

// Temporary buffer used to store the command string produced by hlset().
// IObuff cannot be used for this as the error messages produced by hlset()
// internally use IObuff.
# define	HLSETBUFSZ  512
स्थिर char_u hlsetBuf[HLSETBUFSZ + 1];

/*
 * Add the highlight attribute "attr" of length "attrlen" and "value" at
 * "dptr", which points into "hlsetBuf".
 * Returns the updated pointer.
 */
    स्थिर char_u *
add_attr_and_मूल्य(char_u *dptr, char_u *attr, पूर्णांक attrlen, char_u *मूल्य)
{
    माप_प्रकार	vallen;

    // Do nothing if the value is not specified or is empty
    जर (मूल्य == रिक्त || *मूल्य == NUL)
	परत dptr;

    vallen = STRLEN(मूल्य);
    // When the value contains a space and the attribute has an "=" (i.e. it
    // is a key=value pair), surround the value with single quotes so that
    // do_highlight() can parse it correctly.
    जर (vim_strchr(मूल्य, ' ') != रिक्त && vim_strchr(attr, '=') != रिक्त)
    {
	जर (dptr + attrlen + vallen + 3 < hlsetBuf + HLSETBUFSZ)
	{
	    STRCPY(dptr, attr);
	    dptr += attrlen;
	    *dptr++ = '\'';
	    STRCPY(dptr, मूल्य);
	    dptr += vallen;
	    *dptr++ = '\'';
	    *dptr = NUL;
	}
    }
    अन्यथा जर (dptr + attrlen + vallen + 1 < hlsetBuf + HLSETBUFSZ)
    {
	STRCPY(dptr, attr);
	dptr += attrlen;
	STRCPY(dptr, मूल्य);
	dptr += vallen;
    }

    परत dptr;
}

/*
 * Add or update a highlight group using 'dict' items. Returns TRUE if
 * successfully updated the highlight group.
 */
    स्थिर पूर्णांक
hlg_add_or_update(dict_T *dict)
{
    char_u	*name;
    पूर्णांक		error;
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
    पूर्णांक		forceit = FALSE;
    पूर्णांक		dodefault = FALSE;
    पूर्णांक		done = FALSE;
    char_u	*p;

    name = hldict_get_string(dict, (char_u *)"name", &error);
    जर (name == रिक्त || *name == NUL || error)
	परत FALSE;

    जर (dict_get_bool(dict, "force", VVAL_FALSE) == VVAL_TRUE)
	forceit = TRUE;

    जर (dict_get_bool(dict, "default", VVAL_FALSE) == VVAL_TRUE)
	dodefault = TRUE;

    जर (dict_has_किल्ली(dict, "cleared"))
    {
	varnumber_T	cleared;

	// clear a highlight group
	cleared = dict_get_bool(dict, "cleared", FALSE);
	जर (cleared == TRUE)
	{
	    vim_snprintf((अक्षर *)hlsetBuf, HLSETBUFSZ, "clear %s", name);
	    do_highlight(hlsetBuf, forceit, FALSE);
	    done = TRUE;
	}
    }

    जर (dict_has_किल्ली(dict, "linksto"))
    {
	char_u	*linksto;

	// link highlight groups
	linksto = hldict_get_string(dict, (char_u *)"linksto", &error);
	जर (linksto == रिक्त || *linksto == NUL || error)
	    परत FALSE;

	vim_snprintf((अक्षर *)hlsetBuf, HLSETBUFSZ, "%slink %s %s",
				dodefault ? "default " : "", name, linksto);
	do_highlight(hlsetBuf, forceit, FALSE);

	done = TRUE;
    }

    // If 'cleared' or 'linksto' are specified, then don't process the other
    // attributes.
    जर (done)
	परत TRUE;

    start = hldict_get_string(dict, (char_u *)"start", &error);
    जर (error)
	परत FALSE;

    stop = hldict_get_string(dict, (char_u *)"stop", &error);
    जर (error)
	परत FALSE;

    जर (!hldict_attr_to_str(dict, (char_u *)"term", term_attr,
							    आकार(term_attr)))
	परत FALSE;

    जर (!hldict_attr_to_str(dict, (char_u *)"cterm", cterm_attr,
							   आकार(cterm_attr)))
	परत FALSE;

    ctermfg = hldict_get_string(dict, (char_u *)"ctermfg", &error);
    जर (error)
	परत FALSE;

    ctermbg = hldict_get_string(dict, (char_u *)"ctermbg", &error);
    जर (error)
	परत FALSE;

    ctermul = hldict_get_string(dict, (char_u *)"ctermul", &error);
    जर (error)
	परत FALSE;

    ctermfont = hldict_get_string(dict, (char_u *)"ctermfont", &error);
    जर (error)
	परत FALSE;

    जर (!hldict_attr_to_str(dict, (char_u *)"gui", gui_attr, आकार(gui_attr)))
	परत FALSE;

    guifg = hldict_get_string(dict, (char_u *)"guifg", &error);
    जर (error)
	परत FALSE;

    guibg = hldict_get_string(dict, (char_u *)"guibg", &error);
    जर (error)
	परत FALSE;

    guisp = hldict_get_string(dict, (char_u *)"guisp", &error);
    जर (error)
	परत FALSE;

# ifdef FEAT_GUI
    font = hldict_get_string(dict, (char_u *)"font", &error);
    जर (error)
	परत FALSE;
# endif

    // If none of the attributes are specified, then do nothing.
    जर (term_attr[0] == NUL && start == रिक्त && stop == रिक्त
	    && cterm_attr[0] == NUL && ctermfg == रिक्त && ctermbg == रिक्त
	    && ctermul == रिक्त && ctermfont == रिक्त && gui_attr[0] == NUL
# ifdef FEAT_GUI
	    && font == रिक्त
# endif
	    && guifg == रिक्त && guibg == रिक्त && guisp == रिक्त
	    )
	परत TRUE;

    hlsetBuf[0] = NUL;
    p = hlsetBuf;
    जर (dodefault)
	p = add_attr_and_मूल्य(p, (char_u *)"default", 7, (char_u *)" ");
    p = add_attr_and_मूल्य(p, (char_u *)"", 0, name);
    p = add_attr_and_मूल्य(p, (char_u *)" term=", 6, term_attr);
    p = add_attr_and_मूल्य(p, (char_u *)" start=", 7, start);
    p = add_attr_and_मूल्य(p, (char_u *)" stop=", 6, stop);
    p = add_attr_and_मूल्य(p, (char_u *)" cterm=", 7, cterm_attr);
    p = add_attr_and_मूल्य(p, (char_u *)" ctermfg=", 9, ctermfg);
    p = add_attr_and_मूल्य(p, (char_u *)" ctermbg=", 9, ctermbg);
    p = add_attr_and_मूल्य(p, (char_u *)" ctermul=", 9, ctermul);
    p = add_attr_and_मूल्य(p, (char_u *)" ctermfont=", 11, ctermfont);
    p = add_attr_and_मूल्य(p, (char_u *)" gui=", 5, gui_attr);
# ifdef FEAT_GUI
    p = add_attr_and_मूल्य(p, (char_u *)" font=", 6, font);
# endif
    p = add_attr_and_मूल्य(p, (char_u *)" guifg=", 7, guifg);
    p = add_attr_and_मूल्य(p, (char_u *)" guibg=", 7, guibg);
    (शून्य)add_attr_and_मूल्य(p, (char_u *)" guisp=", 7, guisp);

    do_highlight(hlsetBuf, forceit, FALSE);

    परत TRUE;
}

/*
 * "hlset([{highlight_attr}])" function
 * Add or modify highlight groups
 */
    शून्य
f_hlset(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    dict_T	*dict;

    rettv->vval.v_संख्या = -1;

    जर (check_for_list_arg(argvars, 0) == FAIL)
	परत;

    FOR_ALL_LIST_वस्तुसंच(argvars->vval.v_list, li)
    {
	जर (li->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_dictionary_required));
	    परत;
	}

	dict = li->li_tv.vval.v_dict;
	जर (!hlg_add_or_update(dict))
	    परत;
    }

    rettv->vval.v_संख्या = 0;
}
#endif

/*
 * If "old" is in the override stack, then update it to "hl_new". Does not free
 * "old".
 */
    शून्य
update_highlight_overrides(hl_override_T *old, hl_override_T *hl_new, पूर्णांक newlen)
{
    जर (old == रिक्त)
	परत;

    साठी (hl_overrides_T *set = overrides; set != रिक्त; set = set->next)
    {
	जर (set->arr == old)
	{
	    set->arr = hl_new;
	    set->len = newlen;
	}
    }
}

/*
 * If "arr" is in the highlight overrides list, then mark it as invalid.
 */
    शून्य
remove_highlight_overrides(hl_override_T *arr)
{
    जर (arr == रिक्त || overrides == रिक्त)
	परत;

    साठी (hl_overrides_T *set = overrides; set != रिक्त; set = set->next)
    {
	जर (set->arr == arr)
	{
	    set->arr = रिक्त;
	    थांबा;
	}
    }
}

/*
 * Update highlight_attr[] array. If "update_ids" is true, then update
 * highlight_ids[] array instead.
 */
    स्थिर शून्य
set_highlight_attr(hl_override_T *arr, पूर्णांक len, bool update_ids)
{
    साठी (पूर्णांक i = 0; i < len; i++)
    {
	hl_override_T	*override = arr + i;
	पूर्णांक		hlf = -1, attr;

	जर (override->from <= 0)
	    // Directly map to highlight_attr
	    hlf = -override->from;
	अन्यथा
	{
	    साठी (पूर्णांक k = 0; k < HLF_मोजणी; k++)
		जर (override->from == highlight_ids[k])
		{
		    hlf = k;
		    थांबा;
		}

	    जर (hlf == -1)
		सुरूठेवा;
	}

	जर (update_ids)
	{
	    जर (override->to <= 0)
	    {
		जर (hlf == -override->to)
		    highlight_ids[hlf] = -1;
		अन्यथा
		    highlight_ids[hlf] = highlight_ids[-override->to];
	    }
	    अन्यथा
		highlight_ids[hlf] = override->to;
	}
	अन्यथा
	{
	    जर (override->to <= 0)
		// Directly use highlight_attr[]
		attr = highlight_attr[-override->to];
	    अन्यथा
		attr = syn_id2attr(override->to);

	    highlight_attr[hlf] = attr;
	}
    }
}

/*
 * Set the current highlight override to "arr". Returns true if successful
 */
    bool
ढकला_highlight_overrides(hl_override_T *arr, पूर्णांक len)
{
    // Don't want to do anything if "arr" is already the current override. If
    // "arr" is NULL (but overrides->arr is not), then still push an override,
    // but "->arr" will just be NULL so any previous overrides are cleared.
    जर (overrides != रिक्त && overrides->arr == arr)
	परत false;

    hl_overrides_T *set;

    set = ALLOC_ONE(hl_overrides_T);
    जर (set == रिक्त)
	परत false;

    set->arr = arr;
    set->len = len;

    // Push to front of list
    set->next = overrides;
    overrides = set;

    // Save current highlight_attr[], so it can be restored later after this
    // override is popped.
    स_नकल(set->attr, highlight_attr, आकार(highlight_attr));

    // Reset highlight_attr[] to default values, as we don't want changes from
    // other overrides.
    स_नकल(highlight_attr, highlight_attr_raw, आकार(highlight_attr));

    // Update highlight_attr[] array
    जर (arr != रिक्त)
	set_highlight_attr(arr, len, false);

    परत true;
}

/*
 * Pop the current highlight override (if any) from the stack. This should
 * always be paired with a *successful* push.
 */
    शून्य
काढा_highlight_overrides(शून्य)
{
    hl_overrides_T *set = overrides;

    जर (overrides == रिक्त)
	परत;

    overrides = set->next;

    // Set highlight_attr[] to state before override was pushed.
    स_नकल(highlight_attr, set->attr, आकार(highlight_attr));
    vim_free(set);
}

/*
 * Parse the 'winhighlight' option and return array. Returns NULL on failure or
 * if empty option. If failure, then errmsg is set.
 */
    स्थिर hl_override_T *
parse_winhighlight(char_u *opt, पूर्णांक *len, अक्षर **errmsg)
{
    char_u	    *p = opt;
    hl_override_T   *arr;
    पूर्णांक		    i = 0;
    पूर्णांक		    अंक = 1;
    पूर्णांक		    n_colons = 0;

    जर (*p == NUL)
	परत रिक्त;

    // Get number of overrides first so we can allocate array
    पर्यंत ((p = vim_strchr(p, ',')) != रिक्त)
    {
	p++;
	अंक++;
    }
    p = opt;
    // Check if number of ':' matches number of ','
    पर्यंत ((p = vim_strchr(p, ':')) != रिक्त)
    {
	p++;
	n_colons++;
    }

    जर (अंक != n_colons)
    {
	*errmsg = e_invalid_argument;
	परत रिक्त;
    }

    arr = ALLOC_MULT(hl_override_T, अंक);
    जर (arr == रिक्त)
    {
	*errmsg = e_out_of_memory;
	परत रिक्त;
    }

    p = opt;

    पर्यंत (true)
    {
	hl_override_T *override = arr + i++;
	char_u	*fromname = p, *toname;
	char_u	*tmp;
	char_u	**names[2] = {&fromname, &toname};
	पूर्णांक	fromlen, tolen;
	पूर्णांक	*lens[2] = {&fromlen, &tolen};
	पूर्णांक	fromid, toid;
	पूर्णांक	*ids[2] = {&fromid, &toid};

	p = vim_strchr(p, ':');

	जर (p == रिक्त)
	    जा fail;

	fromlen = p - fromname; // Get hl for "from"
	जर (fromlen == 0)
	    जा fail;
	p++; // Skip colon ':'
	जर (*p == NUL)
	    जा fail;

	toname = p;
	tmp = vim_strchr(p, ',');

	// Get hl for "to", must check for no trailing comma in case last
	// element.
	जर (tmp == रिक्त)
	    tolen = (पूर्णांक)STRLEN(p);
	अन्यथा
	{
	    tolen = tmp - toname;
	    p = ++tmp;
	}
	जर (tolen == 0)
	    जा fail;

	साठी (पूर्णांक k = 0; k < 2; k++)
	{
	    char_u  *name = *names[k];
	    पूर्णांक	    nlen = *lens[k];

	    जर (*name == '!')
	    {
		// If starts with "!", then it maps directly to a 'highlight'
		// occasion.
		पूर्णांक hlf;

		जर (nlen != 2)
		    जा fail;

		साठी (hlf = 0; hlf < (पूर्णांक)HLF_मोजणी; ++hlf)
		    जर (hl_flags[hlf] == name[1])
			थांबा;

		जर (hlf >= HLF_मोजणी)
		    जा fail;

		*ids[k] = -hlf;
	    }
	    अन्यथा
	    {
		// Otherwise get the highlight group id.
		जर (syn_check_group(name, nlen) == 0)
		    जा fail;

		*ids[k] = syn_namen2id(name, nlen);
		जर (*ids[k] == 0)
		    जा fail;

		// Always map "Normal" group to HLF_WIN
		जर (ids[k] == &fromid &&
			STRCMP(HL_TABLE()[*ids[k] - 1].sg_name_u, "NORMAL") == 0)
		    *ids[k] = -HLF_WIN;
	    }
	}

	// If fromid == toid, leave it be, this is so that we know HLF_WIN has
	// been set (even to itself e.g. "Normal"), and therefore know to use
	// its attr instead.
	override->from = fromid;
	override->to = toid;

	जर (tmp == रिक्त)
	    थांबा;
    }

    *len = अंक;
    परत arr;
fail:
    vim_free(arr);
    *errmsg = e_invalid_argument;
    परत रिक्त;
}

/*
 * Update w_hl for "wp" using given option value. Returns error
 * message on failure, otherwise NULL.
 */
    अक्षर *
update_winhighlight(win_T *wp, char_u *opt)
{
    अक्षर	    *err = रिक्त;
    hl_override_T   *arr;
    पूर्णांक		    अंक = 1;

    arr = parse_winhighlight(opt, &अंक, &err);

    जर (arr == रिक्त && err != रिक्त)
	परत err;

    जर (arr == रिक्त && wp->w_hl == रिक्त)
	परत रिक्त;

    update_highlight_overrides(wp->w_hl, arr, अंक);

    vim_free(wp->w_hl);
    wp->w_hl = arr;
    wp->w_hl_len = अंक;

    wp->w_hlfwin_id = hlf_get_id(wp, HLF_WIN);

#ifdef FEAT_TERMINAL
    term_update_hlfwin(wp);
#endif

#ifdef FEAT_TERMINAL
    // May be NULL (such as in after_copy_winopt())
    जर (wp->w_buffer != रिक्त)
    {
	// Make sure terminal highlighting is updated
	bool ret = ढकला_highlight_overrides(wp->w_hl, wp->w_hl_len);

	जर (wp->w_buffer->b_term != रिक्त)
	    term_init_default_colors(wp->w_buffer->b_term);
	जर (ret)
	    काढा_highlight_overrides();
    }
#endif

    परत रिक्त;
}

/*
 * Get id that the hlf attr is using in window "wp", otherwise zero. Note that
 * -1 may be returned if the hlf was set to itself.
 */
    पूर्णांक
hlf_get_id(win_T *wp, पूर्णांक hlf)
{
    पूर्णांक id;
    स्थिर पूर्णांक prev[HLF_मोजणी];

    जर (wp->w_hl == रिक्त)
	परत highlight_ids[hlf];

    स_नकल(prev, highlight_ids, आकार(highlight_ids));
    set_highlight_attr(wp->w_hl, wp->w_hl_len, true);
    id = highlight_ids[hlf];
    स_नकल(highlight_ids, prev, आकार(highlight_ids));

    परत id;
}

/*
 * Update 'winhighlight' using a 'wincolor' style option value. Returns error
 * message on failure otherwise NULL. Does not update the actual 'wincolor'
 * value, but does update the 'winhighlight' value.
 */
    अक्षर *
update_wincolor(win_T *wp, char_u *opt)
{
    char_u  *str = *opt == NUL ? "" : alloc(आकार("!(:") + STRLEN(opt));
    अक्षर    *errmsg;

    जर (str == रिक्त)
	परत e_out_of_memory;

    जर (*opt != NUL)
	प्र_लिखो((अक्षर *)str, "!(:%s", opt);

    errmsg = update_winhighlight(wp, str);

    जर (errmsg == रिक्त)
	set_string_option_direct_in_win(wp, (char_u *)"winhighlight", -1,
		str, OPT_FREE|OPT_LOCAL, 0);

    जर (*opt != NUL)
	मुक्त(str);
    परत errmsg;
}
