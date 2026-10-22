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

#ವ್ಯಾಖ್ಯಾನಿಸು SG_ಪದ		1	// term has been set
#ವ್ಯಾಖ್ಯಾನಿಸು SG_CTERM	2	// cterm has been set
#ವ್ಯಾಖ್ಯಾನಿಸು SG_GUI		4	// gui has been set
#ವ್ಯಾಖ್ಯಾನಿಸು SG_LINK		8	// link has been set

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_SYN_NAME	200

/*
 * The "term", "cterm" and "gui" arguments can be any combination of the
 * following names, separated by commas (but no spaces!).
 */
// must be sorted by the 'value' field because it is used by bsearch()!
// note: inverse and reverse use the same key
ಸ್ಥಿರ keyvalue_T highlight_tab[] = {
    KEYVALUE_ನಮೂದು(HL_BOLD, "bold"),			    // index 0
    KEYVALUE_ನಮೂದು(HL_INVERSE, "inverse"),		    // index 1
    KEYVALUE_ನಮೂದು(HL_ITALIC, "italic"),		    // index 2
    KEYVALUE_ನಮೂದು(HL_NOCOMBINE, "nocombine"),		    // index 3
    KEYVALUE_ನಮೂದು(HL_ಸಾಮಾನ್ಯ, "NONE"),			    // index 4
    KEYVALUE_ನಮೂದು(HL_INVERSE, "reverse"),		    // index 5
    KEYVALUE_ನಮೂದು(HL_STANDOUT, "standout"),		    // index 6
    KEYVALUE_ನಮೂದು(HL_STRIKETHROUGH, "strikethrough"),	    // index 7
    KEYVALUE_ನಮೂದು(HL_UNDERCURL, "undercurl"),		    // index 8
    KEYVALUE_ನಮೂದು(HL_UNDERDASHED, "underdashed"),	    // index 9
    KEYVALUE_ನಮೂದು(HL_UNDERDOTTED, "underdotted"),	    // index 10
    KEYVALUE_ನಮೂದು(HL_UNDERDOUBLE, "underdouble"),	    // index 11
    KEYVALUE_ನಮೂದು(HL_UNDERLINE, "underline")		    // index 12
};

// this table is used to display highlight names in the "correct" sequence.
// keep this in sync with highlight_tab[].
ಸ್ಥಿರ keyvalue_T *highlight_ಸೂಚ್ಯಂಕ_tab[] = {
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
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ATTR_LEN 120

#ವ್ಯಾಖ್ಯಾನಿಸು ATTR_COMBINE(attr_a, attr_b) ((((attr_b) & HL_NOCOMBINE) ? (attr_b) : (attr_a)) | (attr_b))

ಎಣಿಕೆ {
    BLACK = 0,
    DARKBLUE,
    DARKGREEN,
    DARKCYAN,
    DARKRED,
    DARKMAGENTA,
    BROWN,
    DARKYELLOW,
    ಬೂದು,
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
    KEYVALUE_ನಮೂದು(BLACK, "Black"),
    KEYVALUE_ನಮೂದು(BLUE, "Blue"),
    KEYVALUE_ನಮೂದು(BROWN, "Brown"),
    KEYVALUE_ನಮೂದು(CYAN, "Cyan"),
    KEYVALUE_ನಮೂದು(DARKBLUE, "DarkBlue"),
    KEYVALUE_ನಮೂದು(DARKCYAN, "DarkCyan"),
    KEYVALUE_ನಮೂದು(DARKGRAY, "DarkGray"),
    KEYVALUE_ನಮೂದು(DARKGREEN, "DarkGreen"),
    KEYVALUE_ನಮೂದು(DARKGREY, "DarkGrey"),
    KEYVALUE_ನಮೂದು(DARKMAGENTA, "DarkMagenta"),
    KEYVALUE_ನಮೂದು(DARKRED, "DarkRed"),
    KEYVALUE_ನಮೂದು(DARKYELLOW, "DarkYellow"),
    KEYVALUE_ನಮೂದು(ಬೂದು, "Gray"),
    KEYVALUE_ನಮೂದು(GREEN, "Green"),
    KEYVALUE_ನಮೂದು(GREY, "Grey"),
    KEYVALUE_ನಮೂದು(LIGHTBLUE, "LightBlue"),
    KEYVALUE_ನಮೂದು(LIGHTCYAN, "LightCyan"),
    KEYVALUE_ನಮೂದು(LIGHTGRAY, "LightGray"),
    KEYVALUE_ನಮೂದು(LIGHTGREEN, "LightGreen"),
    KEYVALUE_ನಮೂದು(LIGHTGREY, "LightGrey"),
    KEYVALUE_ನಮೂದು(LIGHTMAGENTA, "LightMagenta"),
    KEYVALUE_ನಮೂದು(LIGHTRED, "LightRed"),
    KEYVALUE_ನಮೂದು(LIGHTYELLOW, "LightYellow"),
    KEYVALUE_ನಮೂದು(MAGENTA, "Magenta"),
    KEYVALUE_ನಮೂದು(NONE, "NONE"),
    KEYVALUE_ನಮೂದು(RED, "Red"),
    KEYVALUE_ನಮೂದು(WHITE, "White"),
    KEYVALUE_ನಮೂದು(YELLOW, "Yellow")
};

/*
 * Structure that stores information about a highlight group.
 * The ID of a highlight group is also called group ID.  It is the index in
 * the highlight_ga array PLUS ONE.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	*sg_name;	// highlight group name
    char_u	*sg_name_u;	// uppercase of sg_name
    ಪೂರ್ಣಾಂಕ		sg_cleared;	// "hi clear" was used
// for normal terminals
    ಪೂರ್ಣಾಂಕ		sg_ಪದ;	// "term=" highlighting attributes
    char_u	*sg_ಆರಂಭ;	// terminal string for start highl
    char_u	*sg_stop;	// terminal string for stop highl
    ಪೂರ್ಣಾಂಕ		sg_ಪದ_attr;	// Screen attr for term mode
// for color terminals
    ಪೂರ್ಣಾಂಕ		sg_cterm;	// "cterm=" highlighting attr
    ಪೂರ್ಣಾಂಕ		sg_cterm_bold;	// bold attr was set for light color
    ಪೂರ್ಣಾಂಕ		sg_cterm_fg;	// terminal fg color number + 1
    ಪೂರ್ಣಾಂಕ		sg_cterm_bg;	// terminal bg color number + 1
    ಪೂರ್ಣಾಂಕ		sg_cterm_ul;	// terminal ul color number + 1
    ಪೂರ್ಣಾಂಕ		sg_cterm_attr;	// Screen attr for color term mode
    ಪೂರ್ಣಾಂಕ		sg_cterm_font;	// terminal alternative font (0 for normal)
// for when using the GUI
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    guicolor_T	sg_gui_fg;	// GUI foreground color handle
    guicolor_T	sg_gui_bg;	// GUI background color handle
    guicolor_T	sg_gui_sp;	// GUI special color handle
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    GuiFont	sg_font;	// GUI font handle
# ifdef FEAT_XFONTSET
    GuiFontset	sg_fontset;	// GUI fontset handle
# endif
    char_u	*sg_font_name;  // GUI font or fontset name
    ಪೂರ್ಣಾಂಕ		sg_gui_attr;    // Screen attr for GUI mode
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_EVAL)
// Store the sp color name for the GUI or synIDattr()
    ಪೂರ್ಣಾಂಕ		sg_gui;		// "gui=" highlighting attributes
    char_u	*sg_gui_fg_name;// GUI foreground color name
    char_u	*sg_gui_bg_name;// GUI background color name
    char_u	*sg_gui_sp_name;// GUI special color name
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		sg_link;	// link to this highlight group ID
    ಪೂರ್ಣಾಂಕ		sg_deflink;	// default link; restored in highlight_clear()
    ಪೂರ್ಣಾಂಕ		sg_ಗಣ;		// combination of SG_* flags
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    sctx_T	sg_deflink_sctx;  // script where the default link was set
    sctx_T	sg_script_ctx;	// script in which the group was last set
#ಆದರೆ_ಅಂತ್ಯ
} hl_ಗುಂಪು_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ hl_overrides_S hl_overrides_T;
ರಚನೆ hl_overrides_S
{
    hl_override_T   *arr;   // May be NULL if "arr" was freed or no highlight
			    // overrides (all values set to default)
    ಪೂರ್ಣಾಂಕ		    len;
    hl_overrides_T  *ಮುಂದಿನ;  // Used to handle recursive calls

    ಪೂರ್ಣಾಂಕ attr[HLF_ಎಣಿಕೆ];    // highlight_attr[] before being updated.
};

ಸ್ಥಿರ hl_overrides_T *overrides = ಖಾಲಿ;

// Synced with highlight_attr, each is the highlight group id for each attr. If
// an element is 0, then there is no attr for it. If an element is -1, then it
// means it was set to itself.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ highlight_ids[HLF_ಎಣಿಕೆ];

// Same as highlight_attr[] but is not modified by highlight group overrides
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ highlight_attr_raw[HLF_ಎಣಿಕೆ];

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hl_flags[HLF_ಎಣಿಕೆ] = HL_FLAGS;

// highlight groups for 'highlight' option
ಸ್ಥಿರ garray_T highlight_ga;
#ವ್ಯಾಖ್ಯಾನಿಸು HL_ಕೋಷ್ಟಕ()	((hl_ಗುಂಪು_T *)((highlight_ga.ga_ದತ್ತಾಂಶ)))

// Wrapper struct for hashtable entries: stores the highlight group ID alongside
// the uppercase name used as a hash key.  Uses the same offsetof pattern as
// signgroup_T / HI2SG.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		hn_id;		// highlight group ID (1-based)
    char_u	hn_ಕೀಲಿ[1];	// uppercase name (stretchable array)
} hlname_T;

#ವ್ಯಾಖ್ಯಾನಿಸು HLNAME_ಕೀಲಿ_OFF	offsetof(hlname_T, hn_ಕೀಲಿ)
#ವ್ಯಾಖ್ಯಾನಿಸು HI2HLNAME(hi)	((hlname_T *)((hi)->hi_ಕೀಲಿ - HLNAME_ಕೀಲಿ_OFF))

// hashtable for quick highlight group name lookup
ಸ್ಥಿರ hashtab_T highlight_ht;
ಸ್ಥಿರ bool	 highlight_ht_inited = false;

/*
 * An attribute number is the index in attr_table plus ATTR_OFF.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ATTR_OFF (HL_ALL + 1)

ಸ್ಥಿರ ಶೂನ್ಯ syn_unadd_ಗುಂಪು(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_hl_attr(ಪೂರ್ಣಾಂಕ idx);
ಸ್ಥಿರ ಶೂನ್ಯ highlight_list_one(ಪೂರ್ಣಾಂಕ id);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ highlight_list_arg(ಪೂರ್ಣಾಂಕ id, ಪೂರ್ಣಾಂಕ didh, ಪೂರ್ಣಾಂಕ type, ಪೂರ್ಣಾಂಕ iarg, char_u *sarg, ಅಕ್ಷರ *name);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syn_ಕೂಡಿಸು_ಗುಂಪು(char_u *name);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syn_name2id_len(char_u *name, ಪೂರ್ಣಾಂಕ len);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hl_has_settings(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ check_link);
ಸ್ಥಿರ ಶೂನ್ಯ highlight_clear(ಪೂರ್ಣಾಂಕ idx);

#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
ಸ್ಥಿರ ಶೂನ್ಯ gui_do_one_color(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ do_menu, ಪೂರ್ಣಾಂಕ do_tooltip);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ಗಣ_ಗುಂಪು_colors(char_u *name, guicolor_T *fgp, guicolor_T *bgp, ಪೂರ್ಣಾಂಕ do_menu, ಪೂರ್ಣಾಂಕ use_norm, ಪೂರ್ಣಾಂಕ do_tooltip);
ಸ್ಥಿರ ಶೂನ್ಯ hl_do_font(ಪೂರ್ಣಾಂಕ idx, char_u *arg, ಪೂರ್ಣಾಂಕ do_ಸಾಮಾನ್ಯ, ಪೂರ್ಣಾಂಕ do_menu, ಪೂರ್ಣಾಂಕ do_tooltip, ಪೂರ್ಣಾಂಕ free_font);
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The default highlight groups.  These are compiled-in for fast startup and
 * they still work when the runtime files can't be found.
 * When making changes here, also change runtime/colors/default.vim!
 * The #ifdefs are needed to reduce the amount of static data.  Helps to make
 * the 16 bit DOS (museum) version compile.
 */
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_EVAL)
# define CENT(a, b) b
#ಇಲ್ಲದಿದ್ದರೆ
# define CENT(a, b) a
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಅಕ್ಷರ *(highlight_init_both[]) = {
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    CENT("VisualNOS term=underline,bold cterm=underline,bold",
	 "VisualNOS term=underline,bold cterm=underline,bold gui=underline,bold"),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    CENT("DiffText term=reverse cterm=bold ctermbg=Red ctermfg=Black",
	 "DiffText term=reverse cterm=bold ctermbg=Red ctermfg=Black gui=bold guibg=Red"),
    "default link DiffTextAdd DiffText",
#ಆದರೆ_ಅಂತ್ಯ
    CENT("PmenuSbar term=NONE ctermbg=Grey",
	 "PmenuSbar term=NONE ctermbg=Grey guibg=Grey"),
    CENT("TabLineSel term=bold cterm=bold",
	 "TabLineSel term=bold cterm=bold gui=bold"),
    CENT("TabLineFill term=reverse cterm=reverse",
	 "TabLineFill term=reverse cterm=reverse gui=reverse"),
    "default link TabPanel TabLine",
    "default link TabPanelSel TabLineSel",
    "default link TabPanelFill TabLineFill",
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    "Cursor guibg=fg guifg=bg",
    "lCursor guibg=fg guifg=bg", // should be different, but what?
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
    "TitleBar guibg=bg guifg=fg",
    "TitleBarNC guibg=NONE guifg=NONE",
#ಆದರೆ_ಅಂತ್ಯ
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
    ಖಾಲಿ
};

// Default colors only used with a light background.
ಸ್ಥಿರ ಅಕ್ಷರ *(highlight_init_light[]) = {
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    CENT("SpellBad term=reverse ctermbg=LightRed",
	 "SpellBad term=reverse ctermbg=LightRed guisp=Red gui=undercurl"),
    CENT("SpellCap term=reverse ctermbg=LightBlue",
	 "SpellCap term=reverse ctermbg=LightBlue guisp=Blue gui=undercurl"),
    CENT("SpellRare term=reverse ctermbg=LightMagenta",
	 "SpellRare term=reverse ctermbg=LightMagenta guisp=Magenta gui=undercurl"),
    CENT("SpellLocal term=underline ctermbg=Cyan",
	 "SpellLocal term=underline ctermbg=Cyan guisp=DarkCyan gui=undercurl"),
#ಆದರೆ_ಅಂತ್ಯ
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    CENT("Folded term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "Folded term=standout ctermbg=Grey ctermfg=DarkBlue guibg=LightGrey guifg=DarkBlue"),
    CENT("FoldColumn term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "FoldColumn term=standout ctermbg=Grey ctermfg=DarkBlue guibg=Grey guifg=DarkBlue"),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
    CENT("SignColumn term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "SignColumn term=standout ctermbg=Grey ctermfg=DarkBlue guibg=Grey guifg=DarkBlue"),
#ಆದರೆ_ಅಂತ್ಯ
    CENT("Visual ctermbg=Grey ctermfg=Black",
	 "Visual ctermbg=Grey ctermfg=Black guibg=LightGrey guifg=Black"),
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    CENT("DiffAdd term=bold ctermbg=LightBlue ctermfg=Black",
	 "DiffAdd term=bold ctermbg=LightBlue ctermfg=Black guibg=LightBlue"),
    CENT("DiffChange term=bold ctermbg=LightMagenta ctermfg=Black",
	 "DiffChange term=bold ctermbg=LightMagenta ctermfg=Black guibg=LightMagenta"),
    CENT("DiffDelete term=bold ctermfg=Blue ctermbg=LightCyan",
	 "DiffDelete term=bold ctermfg=Blue ctermbg=LightCyan gui=bold guifg=Blue guibg=LightCyan"),
#ಆದರೆ_ಅಂತ್ಯ
    CENT("TabLine term=underline cterm=underline ctermfg=black ctermbg=LightGrey",
	 "TabLine term=underline cterm=underline ctermfg=black ctermbg=LightGrey gui=underline guibg=LightGrey"),
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    CENT("CursorColumn term=reverse ctermbg=LightGrey",
	 "CursorColumn term=reverse ctermbg=LightGrey guibg=Grey90"),
    CENT("CursorLine term=underline cterm=underline",
	 "CursorLine term=underline cterm=underline guibg=Grey90"),
    CENT("ColorColumn term=reverse ctermbg=LightRed",
	 "ColorColumn term=reverse ctermbg=LightRed guibg=LightRed"),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    CENT("Conceal ctermbg=DarkGrey ctermfg=LightGrey",
	 "Conceal ctermbg=DarkGrey ctermfg=LightGrey guibg=DarkGrey guifg=LightGrey"),
#ಆದರೆ_ಅಂತ್ಯ
    CENT("MatchParen term=reverse ctermbg=Cyan",
	 "MatchParen term=reverse ctermbg=Cyan guibg=Cyan"),
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    CENT("StatusLineTerm term=reverse,bold cterm=bold ctermfg=White ctermbg=DarkGreen",
	 "StatusLineTerm term=reverse,bold cterm=bold ctermfg=White ctermbg=DarkGreen gui=bold guifg=bg guibg=DarkGreen"),
    CENT("StatusLineTermNC term=reverse ctermfg=White ctermbg=DarkGreen",
	 "StatusLineTermNC term=reverse ctermfg=White ctermbg=DarkGreen guifg=bg guibg=DarkGreen"),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    CENT("ToolbarLine term=underline ctermbg=LightGrey",
	 "ToolbarLine term=underline ctermbg=LightGrey guibg=LightGrey"),
    CENT("ToolbarButton cterm=bold ctermfg=White ctermbg=DarkGrey",
	 "ToolbarButton cterm=bold ctermfg=White ctermbg=DarkGrey gui=bold guifg=White guibg=Grey40"),
#ಆದರೆ_ಅಂತ್ಯ
    ಖಾಲಿ
};

// Default colors only used with a dark background.
ಸ್ಥಿರ ಅಕ್ಷರ *(highlight_init_dark[]) = {
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    CENT("SpellBad term=reverse ctermbg=Red",
	 "SpellBad term=reverse ctermbg=Red guisp=Red gui=undercurl"),
    CENT("SpellCap term=reverse ctermbg=Blue",
	 "SpellCap term=reverse ctermbg=Blue guisp=Blue gui=undercurl"),
    CENT("SpellRare term=reverse ctermbg=Magenta",
	 "SpellRare term=reverse ctermbg=Magenta guisp=Magenta gui=undercurl"),
    CENT("SpellLocal term=underline ctermbg=Cyan",
	 "SpellLocal term=underline ctermbg=Cyan guisp=Cyan gui=undercurl"),
#ಆದರೆ_ಅಂತ್ಯ
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    CENT("Folded term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "Folded term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=DarkGrey guifg=Cyan"),
    CENT("FoldColumn term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "FoldColumn term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=Grey guifg=Cyan"),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
    CENT("SignColumn term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "SignColumn term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=Grey guifg=Cyan"),
#ಆದರೆ_ಅಂತ್ಯ
    CENT("Visual ctermbg=Grey ctermfg=Black",
	 "Visual ctermbg=Grey ctermfg=Black guibg=#575757 guifg=LightGrey"),
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    CENT("DiffAdd term=bold ctermbg=DarkBlue",
	 "DiffAdd term=bold ctermbg=DarkBlue guibg=DarkBlue"),
    CENT("DiffChange term=bold ctermbg=DarkMagenta",
	 "DiffChange term=bold ctermbg=DarkMagenta guibg=DarkMagenta"),
    CENT("DiffDelete term=bold ctermfg=Blue ctermbg=DarkCyan",
	 "DiffDelete term=bold ctermfg=Blue ctermbg=DarkCyan gui=bold guifg=Blue guibg=DarkCyan"),
#ಆದರೆ_ಅಂತ್ಯ
    CENT("TabLine term=underline cterm=underline ctermfg=white ctermbg=DarkGrey",
	 "TabLine term=underline cterm=underline ctermfg=white ctermbg=DarkGrey gui=underline guibg=DarkGrey"),
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    CENT("CursorColumn term=reverse ctermbg=DarkGrey",
	 "CursorColumn term=reverse ctermbg=DarkGrey guibg=Grey40"),
    CENT("CursorLine term=underline cterm=underline",
	 "CursorLine term=underline cterm=underline guibg=Grey40"),
    CENT("ColorColumn term=reverse ctermbg=DarkRed",
	 "ColorColumn term=reverse ctermbg=DarkRed guibg=DarkRed"),
#ಆದರೆ_ಅಂತ್ಯ
    CENT("MatchParen term=reverse ctermbg=DarkCyan",
	 "MatchParen term=reverse ctermbg=DarkCyan guibg=DarkCyan"),
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    CENT("Conceal ctermbg=DarkGrey ctermfg=LightGrey",
	 "Conceal ctermbg=DarkGrey ctermfg=LightGrey guibg=DarkGrey guifg=LightGrey"),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    CENT("StatusLineTerm term=reverse,bold cterm=bold ctermfg=Black ctermbg=LightGreen",
	 "StatusLineTerm term=reverse,bold cterm=bold ctermfg=Black ctermbg=LightGreen gui=bold guifg=bg guibg=LightGreen"),
    CENT("StatusLineTermNC term=reverse ctermfg=Black ctermbg=LightGreen",
	 "StatusLineTermNC term=reverse ctermfg=Black ctermbg=LightGreen guifg=bg guibg=LightGreen"),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    CENT("ToolbarLine term=underline ctermbg=DarkGrey",
	 "ToolbarLine term=underline ctermbg=DarkGrey guibg=Grey50"),
    CENT("ToolbarButton cterm=bold ctermfg=Black ctermbg=LightGrey",
	 "ToolbarButton cterm=bold ctermfg=Black ctermbg=LightGrey gui=bold guifg=Black guibg=LightGrey"),
#ಆದರೆ_ಅಂತ್ಯ
    ಖಾಲಿ
};

#ಆದರೆ defined(FEAT_SYN_HL)
/*
 * Returns the number of highlight groups.
 */
    ಪೂರ್ಣಾಂಕ
highlight_ಅಂಕೆ_groups(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ highlight_ga.ga_len;
}

/*
 * Returns the name of a highlight group.
 */
    char_u *
highlight_ಗುಂಪು_name(ಪೂರ್ಣಾಂಕ id)
{
    ಹಿಂತಿರುಗಿ HL_ಕೋಷ್ಟಕ()[id].sg_name;
}

/*
 * Returns the ID of the link to a highlight group.
 */
    ಪೂರ್ಣಾಂಕ
highlight_link_id(ಪೂರ್ಣಾಂಕ id)
{
    ಹಿಂತಿರುಗಿ HL_ಕೋಷ್ಟಕ()[id].sg_link;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ resolve_fallback_fg_ಗಮ್ಯ_rgb(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ resolve_fallback_bg_ಗಮ್ಯ_rgb(ಶೂನ್ಯ);

    ಶೂನ್ಯ
init_highlight(
    ಪೂರ್ಣಾಂಕ		both,	    // include groups where 'bg' doesn't matter
    ಪೂರ್ಣಾಂಕ		reset)	    // clear group first
{
    ಪೂರ್ಣಾಂಕ		i;
    ಅಕ್ಷರ	**pp;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	had_both = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    char_u	*p;

    // Try finding the color scheme file.  Used when a color file was loaded
    // and 'background' or 't_Co' is changed.
    p = get_var_ಮೌಲ್ಯ((char_u *)"g:colors_name");
    ಆದರೆ (p != ಖಾಲಿ)
    {
	// The value of g:colors_name could be freed when sourcing the script,
	// making "p" invalid, so copy it.
	char_u *copy_p = vim_strsave(p);
	ಪೂರ್ಣಾಂಕ    r;

	ಆದರೆ (copy_p != ಖಾಲಿ)
	{
	    r = ತುಂಬು_colors(copy_p);
	    vim_free(copy_p);
	    ಆದರೆ (r == OK)
		ಹಿಂತಿರುಗಿ;
	}
    }

#ಆದರೆ_ಅಂತ್ಯ

    // Didn't use a color file, use the compiled-in colors.
    ಆದರೆ (both)
    {
	had_both = TRUE;
	pp = highlight_init_both;
	ಗಾಗಿ (i = 0; pp[i] != ಖಾಲಿ; ++i)
	    do_highlight((char_u *)pp[i], reset, TRUE);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!had_both)
	// Don't do anything before the call with both == TRUE from main().
	// Not everything has been setup then, and that call will overrule
	// everything anyway.
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (*p_bg == 'l')
	pp = highlight_init_light;
    ಇಲ್ಲದಿದ್ದರೆ
	pp = highlight_init_dark;
    ಗಾಗಿ (i = 0; pp[i] != ಖಾಲಿ; ++i)
	do_highlight((char_u *)pp[i], reset, TRUE);

    // Reverse looks ugly, but grey may not work for less than 8 colors.  Thus
    // let it depend on the number of colors available.
    ಆದರೆ (t_colors < 8)
	do_highlight((char_u *)"Visual term=reverse cterm=reverse ctermbg=NONE ctermfg=NONE",
		FALSE, TRUE);
    // With 8 colors brown is equal to yellow, need to use black for Search fg
    // to avoid Statement highlighted text disappears.
    // Clear the attributes, needed when changing the t_Co value.
    ಆದರೆ (t_colors <= 8)
    {
	ಆದರೆ (*p_bg == 'l')
	    do_highlight((char_u *)"Search ctermfg=black", FALSE, TRUE);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    // If syntax highlighting is enabled load the highlighting for it.
    ಆದರೆ (get_var_ಮೌಲ್ಯ((char_u *)"g:syntax_on") != ಖಾಲಿ)
    {
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = 0;

	ಆದರೆ (recursive >= 5)
	    emsg(_(e_recursive_loop_loading_syncolor_vim));
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ++recursive;
	    (ಶೂನ್ಯ)ಮೂಲ_runtime((char_u *)"syntax/syncolor.vim", DIP_ALL);
	    --recursive;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
    resolve_fallback_fg_ಗಮ್ಯ_rgb();
    resolve_fallback_bg_ಗಮ್ಯ_rgb();
}

#ಆದರೆ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
/*
 * Load a default color list. Intended to support legacy color names but allows
 * the user to override the color values. Only loaded once.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ತುಂಬು_default_colors_lists(ಶೂನ್ಯ)
{
    // Lacking a default color list isn't the end of the world but it is likely
    // an inconvenience so users should know when it is missing.
    ಆದರೆ (ಮೂಲ_runtime((char_u *)"colors/lists/default.vim", DIP_ALL) != OK)
	ಸಂದೇಶ("failed to load colors/lists/default.vim");
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Load color file "name".
 * Return OK for success, FAIL for failure.
 */
    ಪೂರ್ಣಾಂಕ
ತುಂಬು_colors(char_u *name)
{
    char_u	*buf;
    ಪೂರ್ಣಾಂಕ		retval = FAIL;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = FALSE;

    // When being called recursively, this is probably because setting
    // 'background' caused the highlighting to be reloaded.  This means it is
    // working, thus we should return OK.
    ಆದರೆ (recursive)
	ಹಿಂತಿರುಗಿ OK;

    recursive = TRUE;
    buf = alloc(STRLEN(name) + 12);
    ಆದರೆ (buf != ಖಾಲಿ)
    {
#ಆದರೆ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	ತುಂಬು_default_colors_lists();
#ಆದರೆ_ಅಂತ್ಯ
	apply_autocmds(EVENT_COLORSCHEMEPRE, name,
					       curbuf->b_fname, FALSE, curbuf);
	sprintf((ಅಕ್ಷರ *)buf, "colors/%s.vim", name);
	retval = ಮೂಲ_runtime(buf, DIP_ಆರಂಭ + DIP_OPT);
	vim_free(buf);
	ಆದರೆ (retval == OK)
	    apply_autocmds(EVENT_COLORSCHEME, name, curbuf->b_fname,
								FALSE, curbuf);
    }
    recursive = FALSE;

    ಹಿಂತಿರುಗಿ retval;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ color_numbers_16[28] = {0, 1, 2, 3,
				 4, 5, 6, 6,
				 7, 7, 7, 7,
				 8, 8,
				 9, 9, 10, 10,
				 11, 11, 12, 12, 13,
				 13, 14, 14, 15, -1};
// for xterm with 88 colors...
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ color_numbers_88[28] = {0, 4, 2, 6,
				 1, 5, 32, 72,
				 84, 84, 7, 7,
				 82, 82,
				 12, 43, 10, 61,
				 14, 63, 9, 74, 13,
				 75, 11, 78, 15, -1};
// for xterm with 256 colors...
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ color_numbers_256[28] = {0, 4, 2, 6,
				 1, 5, 130, 3,
				 248, 248, 7, 7,
				 242, 242,
				 12, 81, 10, 121,
				 14, 159, 9, 224, 13,
				 225, 11, 229, 15, -1};
// for terminals with less than 16 colors...
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ color_numbers_8[28] = {0, 4, 2, 6,
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lookup_color(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ foreground, ಪೂರ್ಣಾಂಕ *boldp)
{
    ಪೂರ್ಣಾಂಕ		color = color_numbers_16[idx];
    char_u	*p;

    // Use the _16 table to check if it's a valid color name.
    ಆದರೆ (color < 0)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (t_colors == 8)
    {
	// t_Co is 8: use the 8 colors table
#ಆದರೆ defined(__QNXNTO__)
	// On qnx, the 8 & 16 color arrays are the same
	ಆದರೆ (STRNCMP(T_NAME, "qansi", 5) == 0)
	    color = color_numbers_16[idx];
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    color = color_numbers_8[idx];
	ಆದರೆ (foreground)
	{
	    // set/reset bold attribute to get light foreground
	    // colors (on some terminals, e.g. "linux")
	    ಆದರೆ (color & 8)
		*boldp = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
		*boldp = FALSE;
	}
	color &= 7;	// truncate to 8 colors
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (t_colors == 16 || t_colors == 88
					   || t_colors >= 256)
    {
	// Guess: if the termcap entry ends in 'm', it is
	// probably an xterm-like terminal.  Use the changed
	// order for colors.
	ಆದರೆ (*T_CAF != NUL)
	    p = T_CAF;
	ಇಲ್ಲದಿದ್ದರೆ
	    p = T_CSF;
	ಆದರೆ (*p != NUL && (t_colors > 256
			      || *(p + STRLEN(p) - 1) == 'm'))
	{
	    ಆದರೆ (t_colors == 88)
		color = color_numbers_88[idx];
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (t_colors >= 256)
		color = color_numbers_256[idx];
	    ಇಲ್ಲದಿದ್ದರೆ
		color = color_numbers_8[idx];
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
	ಆದರೆ (t_colors >= 256 && color == 15 && is_mac_terminal)
	    // Terminal.app has a bug: 15 is light grey. Use white
	    // from the color cube instead.
	    color = 231;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಹಿಂತಿರುಗಿ color;
}

/*
 * Link highlight group 'from_hg' to 'to_hg'.
 * 'dodefault' is set to TRUE for ":highlight default link".
 * 'forceit' is set to TRUE for ":highlight! link"
 * 'init' is set to TRUE when initializing all the highlight groups.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_ಗುಂಪು_link(
	char_u	*from_hg,
	ಪೂರ್ಣಾಂಕ	from_len,
	char_u	*ಗಮ್ಯ_hg,
	ಪೂರ್ಣಾಂಕ	ಗಮ್ಯ_len,
	ಪೂರ್ಣಾಂಕ	dodefault,
	ಪೂರ್ಣಾಂಕ	forceit,
	ಪೂರ್ಣಾಂಕ	init)
{
    ಪೂರ್ಣಾಂಕ		from_id;
    ಪೂರ್ಣಾಂಕ		ಗಮ್ಯ_id;
    hl_ಗುಂಪು_T	*hlgroup = ಖಾಲಿ;

    from_id = syn_check_ಗುಂಪು(from_hg, from_len);
    ಆದರೆ (STRNCMP(ಗಮ್ಯ_hg, "NONE", 4) == 0)
	ಗಮ್ಯ_id = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಮ್ಯ_id = syn_check_ಗುಂಪು(ಗಮ್ಯ_hg, ಗಮ್ಯ_len);

    ಆದರೆ (from_id > 0)
    {
	hlgroup = &HL_ಕೋಷ್ಟಕ()[from_id - 1];
	ಆದರೆ (dodefault && (forceit || hlgroup->sg_deflink == 0))
	{
	    hlgroup->sg_deflink = ಗಮ್ಯ_id;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    hlgroup->sg_deflink_sctx = current_sctx;
	    hlgroup->sg_deflink_sctx.sc_lnum += SOURCING_LNUM;
#ಆದರೆ_ಅಂತ್ಯ
	}
    }

    ಆದರೆ (from_id > 0 && (!init || hlgroup->sg_ಗಣ == 0))
    {
	// Don't allow a link when there already is some highlighting
	// for the group, unless '!' is used
	ಆದರೆ (ಗಮ್ಯ_id > 0 && !forceit && !init
		&& hl_has_settings(from_id - 1, dodefault))
	{
	    ಆದರೆ (SOURCING_NAME == ಖಾಲಿ && !dodefault)
		emsg(_(e_ಗುಂಪು_has_settings_highlight_link_ignored));
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hlgroup->sg_link != ಗಮ್ಯ_id
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		|| hlgroup->sg_script_ctx.sc_sid != current_sctx.sc_sid
#ಆದರೆ_ಅಂತ್ಯ
		|| hlgroup->sg_cleared)
	{
	    ಆದರೆ (!init)
		hlgroup->sg_ಗಣ |= SG_LINK;
	    hlgroup->sg_link = ಗಮ್ಯ_id;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    hlgroup->sg_script_ctx = current_sctx;
	    hlgroup->sg_script_ctx.sc_lnum += SOURCING_LNUM;
#ಆದರೆ_ಅಂತ್ಯ
	    hlgroup->sg_cleared = FALSE;
	    redraw_all_later(UPD_SOME_VALID);

	    // Only call highlight_changed() once after multiple changes.
	    need_highlight_ಬದಲಾಯಿತು = TRUE;
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
    ಪೂರ್ಣಾಂಕ		idx;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // First, we do not destroy the old values, but allocate the new
    // ones and update the display. THEN we destroy the old values.
    // If we destroy the old values first, then the old values
    // (such as GuiFont's or GuiFontset's) will still be displayed but
    // invalid because they were free'd.
    ಆದರೆ (gui.ಒಳ_use)
    {
# ifdef FEAT_BEVAL_TIP
	gui_init_tooltip_font();
# endif
# ಆದರೆ defined(FEAT_MENU) && defined(FEAT_GUI_MOTIF)
	gui_init_menu_font();
# endif
    }
# ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_X11)
    gui_mch_def_colors();
# endif
# ifdef FEAT_GUI_MSWIN
    ಆದರೆ (gui.ಒಳ_use)
	gui_mch_ಗಣ_titlebar_colors();
# endif
# ifdef FEAT_GUI_X11
#  ifdef FEAT_MENU

    // This only needs to be done when there is no Menu highlight
    // group defined by default, which IS currently the case.
    gui_mch_new_menu_colors();
#  endif
    ಆದರೆ (gui.ಒಳ_use)
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
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    do_unlet((char_u *)"g:colors_name", TRUE);
#ಆದರೆ_ಅಂತ್ಯ
    restore_cterm_colors();

    // Clear all default highlight groups and load the defaults.
    ಗಾಗಿ (idx = 0; idx < highlight_ga.ga_len; ++idx)
	highlight_clear(idx);
    init_highlight(TRUE, TRUE);
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಆದರೆ (USE_24_ಬಿಟ್)
	highlight_gui_started();
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	highlight_ಬದಲಾಯಿತು();
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
highlight_ಗಣ_termgui_attr(ಪೂರ್ಣಾಂಕ idx, char_u *ಕೀಲಿ, char_u *arg, ಪೂರ್ಣಾಂಕ init)
{
    ಪೂರ್ಣಾಂಕ		attr;
    size_t	off;
    keyvalue_T	ಗುರಿ;
    keyvalue_T	*ನಮೂದು;

    attr = 0;
    off = 0;
    ಗುರಿ.ಕೀಲಿ = 0;
    ಗುರಿ.ಮೌಲ್ಯ.ಉದ್ದಳತೆ = 0;	// not used, see cmp_keyvalue_value_ni()
    ಆಗ (arg[off] != NUL)
    {
	ಗುರಿ.ಮೌಲ್ಯ.string = arg + off;
	ನಮೂದು = (keyvalue_T *)bsearch(&ಗುರಿ, &highlight_tab,
		ARRAY_ಉದ್ದಳತೆ(highlight_tab), ಗಾತ್ರ(highlight_tab[0]),
		cmp_keyvalue_ಮೌಲ್ಯ_ni);
	ಆದರೆ (ನಮೂದು == ಖಾಲಿ)
	{
	    semsg(_(e_illegal_ಮೌಲ್ಯ_str), arg);
	    ಹಿಂತಿರುಗಿ FALSE;
	}

	attr |= ನಮೂದು->ಕೀಲಿ;
	off += ನಮೂದು->ಮೌಲ್ಯ.ಉದ್ದಳತೆ;
	ಆದರೆ (arg[off] == ',')		// another one follows
	    ++off;
    }
    ಆದರೆ (*ಕೀಲಿ == 'T')
    {
	ಆದರೆ (!init || !(HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ & SG_ಪದ))
	{
	    ಆದರೆ (!init)
		HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ |= SG_ಪದ;
	    HL_ಕೋಷ್ಟಕ()[idx].sg_ಪದ = attr;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಕೀಲಿ == 'C')
    {
	ಆದರೆ (!init || !(HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ & SG_CTERM))
	{
	    ಆದರೆ (!init)
		HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ |= SG_CTERM;
	    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm = attr;
	    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_bold = FALSE;
	}
    }
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_EVAL)
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (!init || !(HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ & SG_GUI))
	{
	    ಆದರೆ (!init)
		HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ |= SG_GUI;
	    HL_ಕೋಷ್ಟಕ()[idx].sg_gui = attr;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
/*
 * Set the font for the highlight group at 'idx'.
 * 'arg' is the font name.
 * Returns TRUE if the font is changed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
highlight_ಗಣ_font(
	ಪೂರ್ಣಾಂಕ	idx,
	char_u	*arg,
	ಪೂರ್ಣಾಂಕ	is_ಸಾಮಾನ್ಯ_ಗುಂಪು,
	ಪೂರ್ಣಾಂಕ	is_menu_ಗುಂಪು,
	ಪೂರ್ಣಾಂಕ	is_tooltip_ಗುಂಪು)
{
    ಪೂರ್ಣಾಂಕ		did_change = FALSE;

    // in non-GUI fonts are simply ignored
    ಆದರೆ (HL_ಕೋಷ್ಟಕ()[idx].sg_font_name != ಖಾಲಿ
	    && STRCMP(HL_ಕೋಷ್ಟಕ()[idx].sg_font_name, arg) == 0)
    {
	// Font name didn't change, ignore.
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!gui.shell_created)
    {
	// GUI not started yet, always accept the name.
	vim_free(HL_ಕೋಷ್ಟಕ()[idx].sg_font_name);
	HL_ಕೋಷ್ಟಕ()[idx].sg_font_name = vim_strsave(arg);
	did_change = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	GuiFont ತಾತ್ಕಾಲಿಕ_sg_font = HL_ಕೋಷ್ಟಕ()[idx].sg_font;
# ifdef FEAT_XFONTSET
	GuiFontset ತಾತ್ಕಾಲಿಕ_sg_fontset = HL_ಕೋಷ್ಟಕ()[idx].sg_fontset;
# endif
	// First, save the current font/fontset.
	// Then try to allocate the font/fontset.
	// If the allocation fails, HL_TABLE()[idx].sg_font OR
	// sg_fontset will be set to NOFONT or NOFONTSET respectively.

	HL_ಕೋಷ್ಟಕ()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
	HL_ಕೋಷ್ಟಕ()[idx].sg_fontset = NOFONTSET;
# endif
	hl_do_font(idx, arg, is_ಸಾಮಾನ್ಯ_ಗುಂಪು, is_menu_ಗುಂಪು,
		is_tooltip_ಗುಂಪು, FALSE);

# ifdef FEAT_XFONTSET
	ಆದರೆ (HL_ಕೋಷ್ಟಕ()[idx].sg_fontset != NOFONTSET)
	{
	    // New fontset was accepted. Free the old one, if there
	    // was one.
	    gui_mch_free_fontset(ತಾತ್ಕಾಲಿಕ_sg_fontset);
	    did_change = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    HL_ಕೋಷ್ಟಕ()[idx].sg_fontset = ತಾತ್ಕಾಲಿಕ_sg_fontset;
# endif
	ಆದರೆ (HL_ಕೋಷ್ಟಕ()[idx].sg_font != NOFONT)
	{
	    // New font was accepted. Free the old one, if there was
	    // one.
	    gui_mch_free_font(ತಾತ್ಕಾಲಿಕ_sg_font);
	    did_change = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    HL_ಕೋಷ್ಟಕ()[idx].sg_font = ತಾತ್ಕಾಲಿಕ_sg_font;
	// Update sg_font_name once here, after both font and fontset results
	// are known.
	ಆದರೆ (did_change)
	{
	    vim_free(HL_ಕೋಷ್ಟಕ()[idx].sg_font_name);
	    HL_ಕೋಷ್ಟಕ()[idx].sg_font_name = vim_strsave(arg);
	}
    }

    ಹಿಂತಿರುಗಿ did_change;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set the cterm foreground color for the Normal highlight group to "color" and
 * the bold attribute to "bold".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hl_ಗಣ_ctermfg_ಸಾಮಾನ್ಯ_ಗುಂಪು(ಪೂರ್ಣಾಂಕ color, ಪೂರ್ಣಾಂಕ bold)
{
    cterm_ಸಾಮಾನ್ಯ_fg_color = color + 1;
    cterm_ಸಾಮಾನ್ಯ_fg_bold = bold;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Don't do this if the GUI is used.
    ಆದರೆ (!gui.ಒಳ_use && !gui.starting)
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಗಣ_must_redraw(UPD_CLEAR);
	ಆದರೆ (termcap_ಸಕ್ರಿಯ && color >= 0)
	    ಪದ_fg_color(color);
    }
}

/*
 * Set the cterm foreground color for the highlight group at 'idx' to 'color'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_ಗಣ_ctermfg(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ color, ಪೂರ್ಣಾಂಕ is_ಸಾಮಾನ್ಯ_ಗುಂಪು)
{
    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_fg = color + 1;
    ಆದರೆ (is_ಸಾಮಾನ್ಯ_ಗುಂಪು)
	hl_ಗಣ_ctermfg_ಸಾಮಾನ್ಯ_ಗುಂಪು(color,
					(HL_ಕೋಷ್ಟಕ()[idx].sg_cterm & HL_BOLD));
}

/*
 * Set the cterm background color for the Normal highlight group to "color".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hl_ಗಣ_ctermbg_ಸಾಮಾನ್ಯ_ಗುಂಪು(ಪೂರ್ಣಾಂಕ color)
{
    cterm_ಸಾಮಾನ್ಯ_bg_color = color + 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Don't mess with 'background' if the GUI is used.
    ಆದರೆ (!gui.ಒಳ_use && !gui.starting)
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಗಣ_must_redraw(UPD_CLEAR);
	ಆದರೆ (color >= 0)
	{
	    ಪೂರ್ಣಾಂಕ dark = -1;

	    ಆದರೆ (termcap_ಸಕ್ರಿಯ)
		ಪದ_bg_color(color);
	    ಆದರೆ (t_colors < 16)
		dark = (color == 0 || color == 4);
	    // Limit the heuristic to the standard 16 colors
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (color < 16)
		dark = (color < 7 || color == 8);
	    // Set the 'background' option if the value is
	    // wrong.
	    ಆದರೆ (dark != -1
		    && dark != (*p_bg == 'd')
		    && !option_was_ಗಣ((char_u *)"bg"))
	    {
		ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"bg",
			0L, (char_u *)(dark ? "dark" : "light"), 0);
		reset_option_was_ಗಣ((char_u *)"bg");
	    }
	}
    }
}

/*
 * Set the cterm background color for the highlight group at 'idx' to 'color'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_ಗಣ_ctermbg(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ color, ಪೂರ್ಣಾಂಕ is_ಸಾಮಾನ್ಯ_ಗುಂಪು)
{
    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_bg = color + 1;
    ಆದರೆ (is_ಸಾಮಾನ್ಯ_ಗುಂಪು)
	hl_ಗಣ_ctermbg_ಸಾಮಾನ್ಯ_ಗುಂಪು(color);
}

/*
 * Set the cterm underline color for the Normal highlight group to "color".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hl_ಗಣ_ctermul_ಸಾಮಾನ್ಯ_ಗುಂಪು(ಪೂರ್ಣಾಂಕ color)
{
    cterm_ಸಾಮಾನ್ಯ_ul_color = color + 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Don't do this if the GUI is used.
    ಆದರೆ (!gui.ಒಳ_use && !gui.starting)
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಗಣ_must_redraw(UPD_CLEAR);
	ಆದರೆ (termcap_ಸಕ್ರಿಯ && color >= 0)
	    ಪದ_ul_color(color);
    }
}

/*
 * Set the cterm underline color for the highlight group at 'idx' to 'color'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_ಗಣ_ctermul(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ color, ಪೂರ್ಣಾಂಕ is_ಸಾಮಾನ್ಯ_ಗುಂಪು)
{
    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_ul = color + 1;
    ಆದರೆ (is_ಸಾಮಾನ್ಯ_ಗುಂಪು)
	hl_ಗಣ_ctermul_ಸಾಮಾನ್ಯ_ಗುಂಪು(color);
}

/*
 * Set the cterm font for the highlight group at 'idx'.
 * 'arg' is the color name or the numeric value as a string.
 * 'init' is set to TRUE when initializing highlighting.
 * Called for the ":highlight" command and the "hlset()" function.
 *
 * Returns TRUE if the font is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
highlight_ಗಣ_cterm_font(
	ಪೂರ್ಣಾಂಕ	idx,
	char_u	*arg,
	ಪೂರ್ಣಾಂಕ	init)
{
    ಪೂರ್ಣಾಂಕ		font;

    ಆದರೆ (init && (HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ & SG_CTERM))
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (!init)
	HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ |= SG_CTERM;

    ಆದರೆ (VIM_ISDIGIT(*arg))
	font = atoi((ಅಕ್ಷರ *)arg);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(arg, "NONE") == 0)
	font = -1;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ FALSE;

    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_font = font + 1;
    ಹಿಂತಿರುಗಿ TRUE;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
highlight_ಗಣ_cterm_color(
	ಪೂರ್ಣಾಂಕ	idx,
	char_u	*ಕೀಲಿ,
	char_u	*ಕೀಲಿ_ಆರಂಭ,
	char_u	*arg,
	ಪೂರ್ಣಾಂಕ	is_ಸಾಮಾನ್ಯ_ಗುಂಪು,
	ಪೂರ್ಣಾಂಕ	init)
{
    ಪೂರ್ಣಾಂಕ		color;

    ಆದರೆ (init && (HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ & SG_CTERM))
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (!init)
	HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ |= SG_CTERM;

    // When setting the foreground color, and previously the "bold"
    // flag was set for a light color, reset it now
    ಆದರೆ (ಕೀಲಿ[5] == 'F' && HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_bold)
    {
	HL_ಕೋಷ್ಟಕ()[idx].sg_cterm &= ~HL_BOLD;
	HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_bold = FALSE;
    }

    ಆದರೆ (VIM_ISDIGIT(*arg))
	color = atoi((ಅಕ್ಷರ *)arg);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(arg, "fg") == 0)
    {
	ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_fg_color)
	    color = cterm_ಸಾಮಾನ್ಯ_fg_color - 1;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    emsg(_(e_fg_color_unknown));
	    ಹಿಂತಿರುಗಿ FALSE;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(arg, "bg") == 0)
    {
	ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_bg_color > 0)
	    color = cterm_ಸಾಮಾನ್ಯ_bg_color - 1;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    emsg(_(e_bg_color_unknown));
	    ಹಿಂತಿರುಗಿ FALSE;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(arg, "ul") == 0)
    {
	ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_ul_color > 0)
	    color = cterm_ಸಾಮಾನ್ಯ_ul_color - 1;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    emsg(_(e_ul_color_unknown));
	    ಹಿಂತಿರುಗಿ FALSE;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ bold = MAYBE;
	keyvalue_T ಗುರಿ;
	keyvalue_T *ನಮೂದು;

	ಗುರಿ.ಕೀಲಿ = 0;
	ಗುರಿ.ಮೌಲ್ಯ.string = arg;
	ಗುರಿ.ಮೌಲ್ಯ.ಉದ್ದಳತೆ = 0;	// not used, see cmp_keyvalue_value_i()
	ನಮೂದು = (keyvalue_T *)bsearch(&ಗುರಿ, &color_name_tab,
		ARRAY_ಉದ್ದಳತೆ(color_name_tab), ಗಾತ್ರ(color_name_tab[0]),
		cmp_keyvalue_ಮೌಲ್ಯ_i);
	ಆದರೆ (ನಮೂದು == ಖಾಲಿ)
	{
	    semsg(_(e_color_name_or_ಸಂಖ್ಯೆ_not_recognized_str), ಕೀಲಿ_ಆರಂಭ);
	    ಹಿಂತಿರುಗಿ FALSE;
	}

	color = lookup_color(ನಮೂದು->ಕೀಲಿ, ಕೀಲಿ[5] == 'F', &bold);

	// set/reset bold attribute to get light foreground
	// colors (on some terminals, e.g. "linux")
	ಆದರೆ (bold == TRUE)
	{
	    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm |= HL_BOLD;
	    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_bold = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bold == FALSE)
	    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm &= ~HL_BOLD;
    }

    // Add one to the argument, to avoid zero.  Zero is used for
    // "NONE", then "color" is -1.
    ಆದರೆ (ಕೀಲಿ[5] == 'F')
	highlight_ಗಣ_ctermfg(idx, color, is_ಸಾಮಾನ್ಯ_ಗುಂಪು);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೀಲಿ[5] == 'B')
	highlight_ಗಣ_ctermbg(idx, color, is_ಸಾಮಾನ್ಯ_ಗುಂಪು);
    ಇಲ್ಲದಿದ್ದರೆ // ctermul
	highlight_ಗಣ_ctermul(idx, color, is_ಸಾಮಾನ್ಯ_ಗುಂಪು);

    ಹಿಂತಿರುಗಿ TRUE;
}

#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_EVAL)
/*
 * Set the GUI foreground color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
highlight_ಗಣ_guifg(
	ಪೂರ್ಣಾಂಕ	idx,
	char_u	*arg,
	ಪೂರ್ಣಾಂಕ	is_menu_ಗುಂಪು ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ	is_scrollbar_ಗುಂಪು ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ	is_tooltip_ಗುಂಪು ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ	is_titlebar_ಗುಂಪು ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ	is_titlebarnc_ಗುಂಪು ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ	*do_colors ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ	init)
{
# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ದೀರ್ಘ	i;
# endif
    char_u	**namep;
    ಪೂರ್ಣಾಂಕ		did_change = FALSE;

    ಆದರೆ (init && (HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ & SG_GUI))
	ಹಿಂತಿರುಗಿ FALSE;

    namep = &HL_ಕೋಷ್ಟಕ()[idx].sg_gui_fg_name;
    ಆದರೆ (!init)
	HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ |= SG_GUI;

# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guifg colors are only used when recognized
    i = color_name2handle(arg);
    ಆದರೆ (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24_ಬಿಟ್)
    {
	HL_ಕೋಷ್ಟಕ()[idx].sg_gui_fg = i;
# endif
	ಆದರೆ (*namep == ಖಾಲಿ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ಆದರೆ (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    ಇಲ್ಲದಿದ್ದರೆ
		*namep = ಖಾಲಿ;
	    did_change = TRUE;
	}
# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	ಆದರೆ (is_titlebar_ಗುಂಪು && gui.title_fg_pixel != i)
	{
	    gui.title_fg_pixel = i;
	    *do_colors = TRUE;
	}
	ಆದರೆ (is_titlebarnc_ಗುಂಪು && gui.titlenc_fg_pixel != i)
	{
	    gui.titlenc_fg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	ಆದರೆ (is_menu_ಗುಂಪು && gui.menu_fg_pixel != i)
	{
	    gui.menu_fg_pixel = i;
	    *do_colors = TRUE;
	}
	ಆದರೆ (is_scrollbar_ಗುಂಪು && gui.scroll_fg_pixel != i)
	{
	    gui.scroll_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	ಆದರೆ (is_tooltip_ಗುಂಪು && gui.tooltip_fg_pixel != i)
	{
	    gui.tooltip_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    ಹಿಂತಿರುಗಿ did_change;
}

/*
 * Set the GUI background color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
highlight_ಗಣ_guibg(
	ಪೂರ್ಣಾಂಕ	idx,
	char_u	*arg,
	ಪೂರ್ಣಾಂಕ	is_menu_ಗುಂಪು ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ	is_scrollbar_ಗುಂಪು ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ	is_tooltip_ಗುಂಪು ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ	is_titlebar_ಗುಂಪು ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ	is_titlebarnc_ಗುಂಪು ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ	*do_colors ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ	init)
{
# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಪೂರ್ಣಾಂಕ		i;
# endif
    char_u	**namep;
    ಪೂರ್ಣಾಂಕ		did_change = FALSE;

    ಆದರೆ (init && (HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ & SG_GUI))
	ಹಿಂತಿರುಗಿ FALSE;

    namep = &HL_ಕೋಷ್ಟಕ()[idx].sg_gui_bg_name;
    ಆದರೆ (!init)
	HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ |= SG_GUI;

# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guibg colors are only used when recognized
    i = color_name2handle(arg);
    ಆದರೆ (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24_ಬಿಟ್)
    {
	HL_ಕೋಷ್ಟಕ()[idx].sg_gui_bg = i;
# endif
	ಆದರೆ (*namep == ಖಾಲಿ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ಆದರೆ (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    ಇಲ್ಲದಿದ್ದರೆ
		*namep = ಖಾಲಿ;
	    did_change = TRUE;
	}
# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	ಆದರೆ (is_titlebar_ಗುಂಪು && gui.title_bg_pixel != i)
	{
	    gui.title_bg_pixel = i;
	    *do_colors = TRUE;
	}
	ಆದರೆ (is_titlebarnc_ಗುಂಪು && gui.titlenc_bg_pixel != i)
	{
	    gui.titlenc_bg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	ಆದರೆ (is_menu_ಗುಂಪು && gui.menu_bg_pixel != i)
	{
	    gui.menu_bg_pixel = i;
	    *do_colors = TRUE;
	}
	ಆದರೆ (is_scrollbar_ಗುಂಪು && gui.scroll_bg_pixel != i)
	{
	    gui.scroll_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	ಆದರೆ (is_tooltip_ಗುಂಪು && gui.tooltip_bg_pixel != i)
	{
	    gui.tooltip_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    ಹಿಂತಿರುಗಿ did_change;
}

/*
 * Set the GUI undercurl/strikethrough color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
highlight_ಗಣ_guisp(ಪೂರ್ಣಾಂಕ idx, char_u *arg, ಪೂರ್ಣಾಂಕ init)
{
# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಪೂರ್ಣಾಂಕ		i;
# endif
    ಪೂರ್ಣಾಂಕ		did_change = FALSE;
    char_u	**namep;

    ಆದರೆ (init && (HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ & SG_GUI))
	ಹಿಂತಿರುಗಿ FALSE;

    namep = &HL_ಕೋಷ್ಟಕ()[idx].sg_gui_sp_name;
    ಆದರೆ (!init)
	HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ |= SG_GUI;

# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guisp colors are only used when recognized
    i = color_name2handle(arg);
    ಆದರೆ (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24_ಬಿಟ್)
    {
	HL_ಕೋಷ್ಟಕ()[idx].sg_gui_sp = i;
# endif
	ಆದರೆ (*namep == ಖಾಲಿ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ಆದರೆ (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    ಇಲ್ಲದಿದ್ದರೆ
		*namep = ಖಾಲಿ;
	    did_change = TRUE;
	}
# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    }
# endif

    ಹಿಂತಿರುಗಿ did_change;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set the start/stop terminal codes for a highlight group.
 * Returns TRUE if the terminal code is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
highlight_ಗಣ_startstop_termcode(ಪೂರ್ಣಾಂಕ idx, char_u *ಕೀಲಿ, char_u *arg, ಪೂರ್ಣಾಂಕ init)
{
    ಪೂರ್ಣಾಂಕ		off;
    char_u	buf[100];
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*tname;
    char_u	*p;

    ಆದರೆ (!init)
	HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ |= SG_ಪದ;

    // The "start" and "stop"  arguments can be a literal escape
    // sequence, or a comma separated list of terminal codes.
    ಆದರೆ (STRNCMP(arg, "t_", 2) == 0)
    {
	off = 0;
	buf[0] = 0;
	ಆಗ (arg[off] != NUL)
	{
	    // Isolate one termcap name
	    ಗಾಗಿ (len = 0; arg[off + len] &&
		    arg[off + len] != ','; ++len)
		;
	    tname = vim_strnsave(arg + off, len);
	    ಆದರೆ (tname == ಖಾಲಿ)		// out of memory
		ಹಿಂತಿರುಗಿ FALSE;
	    // lookup the escape sequence for the item
	    p = get_ಪದ_ಸಂಕೇತ(tname);
	    vim_free(tname);
	    ಆದರೆ (p == ಖಾಲಿ)	    // ignore non-existing things
		p = (char_u *)"";

	    // Append it to the already found stuff
	    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)(STRLEN(buf) + STRLEN(p)) >= 99)
	    {
		semsg(_(e_terminal_ಸಂಕೇತ_too_long_str), arg);
		ಹಿಂತಿರುಗಿ FALSE;
	    }
	    STRCAT(buf, p);

	    // Advance to the next item
	    off += len;
	    ಆದರೆ (arg[off] == ',')	    // another one follows
		++off;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Copy characters from arg[] to buf[], translating <> codes.
	ಗಾಗಿ (p = arg, off = 0; off < 100 - 6 && *p; )
	{
	    len = trans_special(&p, buf + off, FSK_SIMPLIFY, FALSE, ಖಾಲಿ);
	    ಆದರೆ (len > 0)	    // recognized special char
		off += len;
	    ಇಲ್ಲದಿದ್ದರೆ		    // copy as normal char
		buf[off++] = *p++;
	}
	buf[off] = NUL;
    }

    ಆದರೆ (STRCMP(buf, "NONE") == 0)	// resetting the value
	p = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	p = vim_strsave(buf);
    ಆದರೆ (ಕೀಲಿ[2] == 'A')
    {
	vim_free(HL_ಕೋಷ್ಟಕ()[idx].sg_ಆರಂಭ);
	HL_ಕೋಷ್ಟಕ()[idx].sg_ಆರಂಭ = p;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vim_free(HL_ಕೋಷ್ಟಕ()[idx].sg_stop);
	HL_ಕೋಷ್ಟಕ()[idx].sg_stop = p;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Handle the ":highlight .." command.
 * When using ":hi clear" this is called recursively for each group with
 * "forceit" and "init" both TRUE.
 */
    ಶೂನ್ಯ
do_highlight(
    char_u	*ಸಾಲು,
    ಪೂರ್ಣಾಂಕ		forceit,
    ಪೂರ್ಣಾಂಕ		init)	    // TRUE when called for initializing
{
    char_u	*name_ಕೊನೆ;
    char_u	*linep;
    char_u	*ಕೀಲಿ_ಆರಂಭ;
    char_u	*arg_ಆರಂಭ;
    char_u	*ಕೀಲಿ = ಖಾಲಿ, *arg = ಖಾಲಿ;
    ದೀರ್ಘ	i;
    ಪೂರ್ಣಾಂಕ		id;
    ಪೂರ್ಣಾಂಕ		idx;
    hl_ಗುಂಪು_T	ಅಂಶ_before;
    ಪೂರ್ಣಾಂಕ		did_change = FALSE;
    ಪೂರ್ಣಾಂಕ		dodefault = FALSE;
    ಪೂರ್ಣಾಂಕ		doclear = FALSE;
    ಪೂರ್ಣಾಂಕ		dolink = FALSE;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    ಪೂರ್ಣಾಂಕ		is_ಸಾಮಾನ್ಯ_ಗುಂಪು = FALSE;	// "Normal" group
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
    ಪೂರ್ಣಾಂಕ		is_titlebar_ಗುಂಪು = FALSE;	// "TitleBar" group
    ಪೂರ್ಣಾಂಕ		is_titlebarnc_ಗುಂಪು = FALSE;	// "TitleBarNC" group
#ಇಲ್ಲದಿದ್ದರೆ
# define is_titlebar_ಗುಂಪು 0
# define is_titlebarnc_ಗುಂಪು 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_X11
    ಪೂರ್ಣಾಂಕ		is_menu_ಗುಂಪು = FALSE;		// "Menu" group
    ಪೂರ್ಣಾಂಕ		is_scrollbar_ಗುಂಪು = FALSE;	// "Scrollbar" group
    ಪೂರ್ಣಾಂಕ		is_tooltip_ಗುಂಪು = FALSE;	// "Tooltip" group
#ಇಲ್ಲದಿದ್ದರೆ
# define is_menu_ಗುಂಪು 0
# define is_tooltip_ಗುಂಪು 0
# define is_scrollbar_ಗುಂಪು 0
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_EVAL)
    ಪೂರ್ಣಾಂಕ		do_colors = FALSE;		// need to update colors?
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಪೂರ್ಣಾಂಕ		did_highlight_ಬದಲಾಯಿತು = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // If no argument, list current highlighting.
    ಆದರೆ (!init && ತುದಿಗಳು_excmd2(ಸಾಲು - 1, ಸಾಲು))
    {
	ಗಾಗಿ (i = 1; i <= highlight_ga.ga_len && !got_int; ++i)
	    // TODO: only call when the group has attributes set
	    highlight_list_one((ಪೂರ್ಣಾಂಕ)i);
	ಹಿಂತಿರುಗಿ;
    }

    // Isolate the name.
    name_ಕೊನೆ = skiptowhite(ಸಾಲು);
    linep = skipwhite(name_ಕೊನೆ);

    // Check for "default" argument.
    ಆದರೆ (STRNCMP(ಸಾಲು, "default", name_ಕೊನೆ - ಸಾಲು) == 0)
    {
	dodefault = TRUE;
	ಸಾಲು = linep;
	name_ಕೊನೆ = skiptowhite(ಸಾಲು);
	linep = skipwhite(name_ಕೊನೆ);
    }

    // Check for "clear" or "link" argument.
    ಆದರೆ (STRNCMP(ಸಾಲು, "clear", name_ಕೊನೆ - ಸಾಲು) == 0)
	doclear = TRUE;
    ಆದರೆ (STRNCMP(ಸಾಲು, "link", name_ಕೊನೆ - ಸಾಲು) == 0)
	dolink = TRUE;

    // ":highlight {group-name}": list highlighting for one group.
    ಆದರೆ (!doclear && !dolink && ತುದಿಗಳು_excmd2(ಸಾಲು, linep))
    {
	id = syn_namen2id(ಸಾಲು, (ಪೂರ್ಣಾಂಕ)(name_ಕೊನೆ - ಸಾಲು));
	ಆದರೆ (id == 0)
	    semsg(_(e_highlight_ಗುಂಪು_name_not_ಸಿಕ್ಕಿತು_str), ಸಾಲು);
	ಇಲ್ಲದಿದ್ದರೆ
	    highlight_list_one(id);
	ಹಿಂತಿರುಗಿ;
    }

    // Handle ":highlight link {from} {to}" command.
    ಆದರೆ (dolink)
    {
	char_u	    *from_ಆರಂಭ = linep;
	char_u	    *from_ಕೊನೆ;
	ಪೂರ್ಣಾಂಕ	    from_len;
	char_u	    *ಗಮ್ಯ_ಆರಂಭ;
	char_u	    *ಗಮ್ಯ_ಕೊನೆ;
	ಪೂರ್ಣಾಂಕ	    ಗಮ್ಯ_len;

	from_ಕೊನೆ = skiptowhite(from_ಆರಂಭ);
	ಗಮ್ಯ_ಆರಂಭ = skipwhite(from_ಕೊನೆ);
	ಗಮ್ಯ_ಕೊನೆ	 = skiptowhite(ಗಮ್ಯ_ಆರಂಭ);

	ಆದರೆ (ತುದಿಗಳು_excmd2(ಸಾಲು, from_ಆರಂಭ) || ತುದಿಗಳು_excmd2(ಸಾಲು, ಗಮ್ಯ_ಆರಂಭ))
	{
	    semsg(_(e_not_enough_arguments_highlight_link_str), from_ಆರಂಭ);
	    ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (!ತುದಿಗಳು_excmd2(ಸಾಲು, skipwhite(ಗಮ್ಯ_ಕೊನೆ)))
	{
	    semsg(_(e_too_many_arguments_highlight_link_str), from_ಆರಂಭ);
	    ಹಿಂತಿರುಗಿ;
	}

	from_len = (ಪೂರ್ಣಾಂಕ)(from_ಕೊನೆ - from_ಆರಂಭ);
	ಗಮ್ಯ_len = (ಪೂರ್ಣಾಂಕ)(ಗಮ್ಯ_ಕೊನೆ - ಗಮ್ಯ_ಆರಂಭ);
	highlight_ಗುಂಪು_link(from_ಆರಂಭ, from_len, ಗಮ್ಯ_ಆರಂಭ, ಗಮ್ಯ_len,
						dodefault, forceit, init);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (doclear)
    {
	// ":highlight clear [group]" command.
	ಆದರೆ (ತುದಿಗಳು_excmd2(ಸಾಲು, linep))
	{
	    // ":highlight clear" without group name
	    highlight_reset_all();
	    ಹಿಂತಿರುಗಿ;
	}
	ಸಾಲು = linep;
	name_ಕೊನೆ = skiptowhite(ಸಾಲು);
	linep = skipwhite(name_ಕೊನೆ);
    }

    // Find the group name in the table.  If it does not exist yet, add it.
    id = syn_check_ಗುಂಪು(ಸಾಲು, (ಪೂರ್ಣಾಂಕ)(name_ಕೊನೆ - ಸಾಲು));
    ಆದರೆ (id == 0)	// failed (out of memory)
	ಹಿಂತಿರುಗಿ;
    idx = id - 1;	// index is ID minus one

    // Return if "default" was used and the group already has settings.
    ಆದರೆ (dodefault && hl_has_settings(idx, TRUE))
	ಹಿಂತಿರುಗಿ;

    // Make a copy so we can check if any attribute actually changed.
    ಅಂಶ_before = HL_ಕೋಷ್ಟಕ()[idx];

    ಆದರೆ (STRCMP(HL_ಕೋಷ್ಟಕ()[idx].sg_name_u, "NORMAL") == 0)
	is_ಸಾಮಾನ್ಯ_ಗುಂಪು = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(HL_ಕೋಷ್ಟಕ()[idx].sg_name_u, "TITLEBAR") == 0)
	is_titlebar_ಗುಂಪು = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(HL_ಕೋಷ್ಟಕ()[idx].sg_name_u, "TITLEBARNC") == 0)
	is_titlebarnc_ಗುಂಪು = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_X11
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(HL_ಕೋಷ್ಟಕ()[idx].sg_name_u, "MENU") == 0)
	is_menu_ಗುಂಪು = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(HL_ಕೋಷ್ಟಕ()[idx].sg_name_u, "SCROLLBAR") == 0)
	is_scrollbar_ಗುಂಪು = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(HL_ಕೋಷ್ಟಕ()[idx].sg_name_u, "TOOLTIP") == 0)
	is_tooltip_ಗುಂಪು = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    // Clear the highlighting for ":hi clear {group}" and ":hi clear".
    ಆದರೆ (doclear || (forceit && init))
    {
	highlight_clear(idx);
	ಆದರೆ (!doclear)
	    HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ = 0;
    }

    ಆದರೆ (!doclear)
	ಆಗ (!ತುದಿಗಳು_excmd2(ಸಾಲು, linep))
	{
	    ಕೀಲಿ_ಆರಂಭ = linep;
	    ಆದರೆ (*linep == '=')
	    {
		semsg(_(e_unexpected_equal_sign_str), ಕೀಲಿ_ಆರಂಭ);
		error = TRUE;
		ನಿಲ್ಲಿಸು;
	    }

	    // Note: Keep this in sync with get_highlight_group_key.

	    // Isolate the key ("term", "ctermfg", "ctermbg", "font", "guifg"
	    // or "guibg").
	    ಆಗ (*linep && !VIM_ISWHITE(*linep) && *linep != '=')
		++linep;
	    vim_free(ಕೀಲಿ);
	    ಕೀಲಿ = vim_strnsave_ಮೇಲೆ(ಕೀಲಿ_ಆರಂಭ, linep - ಕೀಲಿ_ಆರಂಭ);
	    ಆದರೆ (ಕೀಲಿ == ಖಾಲಿ)
	    {
		error = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	    linep = skipwhite(linep);

	    ಆದರೆ (STRCMP(ಕೀಲಿ, "NONE") == 0)
	    {
		ಆದರೆ (!init || HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ == 0)
		{
		    ಆದರೆ (!init)
			HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ |= SG_ಪದ+SG_CTERM+SG_GUI;
		    highlight_clear(idx);
		}
		ಮುಂದುವರಿ;
	    }

	    // Check for the equal sign.
	    ಆದರೆ (*linep != '=')
	    {
		semsg(_(e_missing_equal_sign_str_2), ಕೀಲಿ_ಆರಂಭ);
		error = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	    ++linep;

	    // Isolate the argument.
	    linep = skipwhite(linep);
	    ಆದರೆ (*linep == '\'')		// guifg='color name'
	    {
		arg_ಆರಂಭ = ++linep;
		linep = vim_strchr(linep, '\'');
		ಆದರೆ (linep == ಖಾಲಿ)
		{
		    semsg(_(e_invalid_ವಾದ_str), ಕೀಲಿ_ಆರಂಭ);
		    error = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		arg_ಆರಂಭ = linep;
		linep = skiptowhite(linep);
	    }
	    ಆದರೆ (linep == arg_ಆರಂಭ)
	    {
		semsg(_(e_missing_ವಾದ_str), ಕೀಲಿ_ಆರಂಭ);
		error = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	    vim_free(arg);
	    arg = vim_strnsave(arg_ಆರಂಭ, linep - arg_ಆರಂಭ);
	    ಆದರೆ (arg == ಖಾಲಿ)
	    {
		error = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (*linep == '\'')
		++linep;

	    // Store the argument.
	    ಆದರೆ (STRCMP(ಕೀಲಿ, "TERM") == 0
		    || STRCMP(ಕೀಲಿ, "CTERM") == 0
		    || STRCMP(ಕೀಲಿ, "GUI") == 0)
	    {
		ಆದರೆ (!highlight_ಗಣ_termgui_attr(idx, ಕೀಲಿ, arg, init))
		{
		    error = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(ಕೀಲಿ, "FONT") == 0)
	    {
		// in non-GUI fonts are simply ignored
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		ಆದರೆ (highlight_ಗಣ_font(idx, arg, is_ಸಾಮಾನ್ಯ_ಗುಂಪು,
					      is_menu_ಗುಂಪು, is_tooltip_ಗುಂಪು))
		    did_change = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(ಕೀಲಿ, "CTERMFG") == 0
		    || STRCMP(ಕೀಲಿ, "CTERMBG") == 0
		    || STRCMP(ಕೀಲಿ, "CTERMUL") == 0)
	    {
		ಆದರೆ (!highlight_ಗಣ_cterm_color(idx, ಕೀಲಿ, ಕೀಲಿ_ಆರಂಭ, arg,
							is_ಸಾಮಾನ್ಯ_ಗುಂಪು, init))
		{
		    error = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(ಕೀಲಿ, "CTERMFONT") == 0)
	    {
		ಆದರೆ (!highlight_ಗಣ_cterm_font(idx, arg, init))
		{
		    error = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(ಕೀಲಿ, "GUIFG") == 0)
	    {
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_EVAL)
		ಆದರೆ (highlight_ಗಣ_guifg(idx, arg, is_menu_ಗುಂಪು,
			    is_scrollbar_ಗುಂಪು, is_tooltip_ಗುಂಪು,
			    is_titlebar_ಗುಂಪು, is_titlebarnc_ಗುಂಪು,
			    &do_colors, init))
		    did_change = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(ಕೀಲಿ, "GUIBG") == 0)
	    {
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_EVAL)
		ಆದರೆ (highlight_ಗಣ_guibg(idx, arg, is_menu_ಗುಂಪು,
			    is_scrollbar_ಗುಂಪು, is_tooltip_ಗುಂಪು,
			    is_titlebar_ಗುಂಪು, is_titlebarnc_ಗುಂಪು,
			    &do_colors, init))
		    did_change = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(ಕೀಲಿ, "GUISP") == 0)
	    {
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_EVAL)
		ಆದರೆ (highlight_ಗಣ_guisp(idx, arg, init))
		    did_change = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(ಕೀಲಿ, "START") == 0 || STRCMP(ಕೀಲಿ, "STOP") == 0)
	    {
		ಆದರೆ (!highlight_ಗಣ_startstop_termcode(idx, ಕೀಲಿ, arg, init))
		{
		    error = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		semsg(_(e_illegal_ವಾದ_str_3), ಕೀಲಿ_ಆರಂಭ);
		error = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	    HL_ಕೋಷ್ಟಕ()[idx].sg_cleared = FALSE;

	    // When highlighting has been given for a group, don't link it.
	    ಆದರೆ (!init || !(HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ & SG_LINK))
		HL_ಕೋಷ್ಟಕ()[idx].sg_link = 0;

	    // Continue with next argument.
	    linep = skipwhite(linep);
	}

    // If there is an error, and it's a new entry, remove it from the table.
    ಆದರೆ (error && idx == highlight_ga.ga_len)
	syn_unadd_ಗುಂಪು();
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (is_ಸಾಮಾನ್ಯ_ಗುಂಪು)
	{
	    HL_ಕೋಷ್ಟಕ()[idx].sg_ಪದ_attr = 0;
	    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_attr = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    HL_ಕೋಷ್ಟಕ()[idx].sg_gui_attr = 0;
	    // Need to update all groups, because they might be using "bg"
	    // and/or "fg", which have been changed now.
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    ಆದರೆ (USE_24_ಬಿಟ್)
	    {
		highlight_gui_started();
		did_highlight_ಬದಲಾಯಿತು = TRUE;
		redraw_all_later(UPD_NOT_VALID);
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    resolve_fallback_fg_ಗಮ್ಯ_rgb();
	    resolve_fallback_bg_ಗಮ್ಯ_rgb();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VTP
	    control_console_color_rgb();
#ಆದರೆ_ಅಂತ್ಯ
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_titlebar_ಗುಂಪು)
	{
	    ಆದರೆ (gui.ಒಳ_use && do_colors)
		gui_mch_ಗಣ_titlebar_colors();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_titlebarnc_ಗುಂಪು)
	{
	    ಆದರೆ (gui.ಒಳ_use && do_colors)
		gui_mch_ಗಣ_titlebar_colors();
	}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_X11
# ifdef FEAT_MENU
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_menu_ಗುಂಪು)
	{
	    ಆದರೆ (gui.ಒಳ_use && do_colors)
		gui_mch_new_menu_colors();
	}
# endif
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_scrollbar_ಗುಂಪು)
	{
	    ಆದರೆ (gui.ಒಳ_use && do_colors)
		gui_new_scrollbar_colors();
	    ಇಲ್ಲದಿದ್ದರೆ
		ಗಣ_hl_attr(idx);
	}
# ifdef FEAT_BEVAL_GUI
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_tooltip_ಗುಂಪು)
	{
	    ಆದರೆ (gui.ಒಳ_use && do_colors)
		gui_mch_new_tooltip_colors();
	}
# endif
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ
	    ಗಣ_hl_attr(idx);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	HL_ಕೋಷ್ಟಕ()[idx].sg_script_ctx = current_sctx;
	HL_ಕೋಷ್ಟಕ()[idx].sg_script_ctx.sc_lnum += SOURCING_LNUM;
#ಆದರೆ_ಅಂತ್ಯ
    }

    vim_free(ಕೀಲಿ);
    vim_free(arg);

    // Only call highlight_changed() once, after a sequence of highlight
    // commands, and only if an attribute actually changed.
    ಆದರೆ ((did_change
	   || memcmp(&HL_ಕೋಷ್ಟಕ()[idx], &ಅಂಶ_before, ಗಾತ್ರ(ಅಂಶ_before)) != 0)
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    && !did_highlight_ಬದಲಾಯಿತು
#ಆದರೆ_ಅಂತ್ಯ
       )
    {
	// Do not trigger a redraw when highlighting is changed while
	// redrawing.  This may happen when evaluating 'statusline' changes the
	// StatusLine group.
	ಆದರೆ (!updating_screen)
	    redraw_all_later(UPD_NOT_VALID);
	need_highlight_ಬದಲಾಯಿತು = TRUE;
    }
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_highlight(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಾಗಿ (i = 0; i < highlight_ga.ga_len; ++i)
    {
	highlight_clear(i);
	vim_free(HL_ಕೋಷ್ಟಕ()[i].sg_name);
	vim_free(HL_ಕೋಷ್ಟಕ()[i].sg_name_u - HLNAME_ಕೀಲಿ_OFF);
    }
    ga_clear(&highlight_ga);
    hash_clear(&highlight_ht);
    highlight_ht_inited = false;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Reset the cterm colors to what they were before Vim was started, if
 * possible.  Otherwise reset them to zero.
 */
    ಶೂನ್ಯ
restore_cterm_colors(ಶೂನ್ಯ)
{
#ಆದರೆ defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
    // Since t_me has been set, this probably means that the user
    // wants to use this as default colors.  Need to reset default
    // background/foreground colors.
    mch_ಗಣ_ಸಾಮಾನ್ಯ_colors();
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef VIMDLL
    ಆದರೆ (!gui.ಒಳ_use)
    {
	mch_ಗಣ_ಸಾಮಾನ್ಯ_colors();
	ಹಿಂತಿರುಗಿ;
    }
# endif
    cterm_ಸಾಮಾನ್ಯ_fg_color = 0;
    cterm_ಸಾಮಾನ್ಯ_fg_bold = 0;
    cterm_ಸಾಮಾನ್ಯ_bg_color = 0;
# ifdef FEAT_TERMGUICOLORS
    cterm_ಸಾಮಾನ್ಯ_fg_gui_color = INVALCOLOR;
    cterm_ಸಾಮಾನ್ಯ_bg_gui_color = INVALCOLOR;
    cterm_ಸಾಮಾನ್ಯ_ul_gui_color = INVALCOLOR;
# endif
    fallback_fg_rgb = INVALCOLOR;
    fallback_bg_rgb = INVALCOLOR;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Return TRUE if highlight group "idx" has any settings.
 * When "check_link" is TRUE also check for an existing link.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
hl_has_settings(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ check_link)
{
    ಹಿಂತಿರುಗಿ HL_ಕೋಷ್ಟಕ()[idx].sg_cleared == 0
	 && (  HL_ಕೋಷ್ಟಕ()[idx].sg_ಪದ_attr != 0
	    || HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_attr != 0
	    || HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_fg != 0
	    || HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_bg != 0
	    || HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_font != 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    || HL_ಕೋಷ್ಟಕ()[idx].sg_gui_attr != 0
	    || HL_ಕೋಷ್ಟಕ()[idx].sg_gui_fg_name != ಖಾಲಿ
	    || HL_ಕೋಷ್ಟಕ()[idx].sg_gui_bg_name != ಖಾಲಿ
	    || HL_ಕೋಷ್ಟಕ()[idx].sg_gui_sp_name != ಖಾಲಿ
	    || HL_ಕೋಷ್ಟಕ()[idx].sg_font_name != ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
	    || (check_link && (HL_ಕೋಷ್ಟಕ()[idx].sg_ಗಣ & SG_LINK)));
}

/*
 * Clear highlighting for one group.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_clear(ಪೂರ್ಣಾಂಕ idx)
{
    HL_ಕೋಷ್ಟಕ()[idx].sg_cleared = TRUE;

    HL_ಕೋಷ್ಟಕ()[idx].sg_ಪದ = 0;
    VIM_CLEAR(HL_ಕೋಷ್ಟಕ()[idx].sg_ಆರಂಭ);
    VIM_CLEAR(HL_ಕೋಷ್ಟಕ()[idx].sg_stop);
    HL_ಕೋಷ್ಟಕ()[idx].sg_ಪದ_attr = 0;
    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm = 0;
    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_bold = FALSE;
    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_fg = 0;
    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_bg = 0;
    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_attr = 0;
    HL_ಕೋಷ್ಟಕ()[idx].sg_cterm_font = 0;
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_EVAL)
    HL_ಕೋಷ್ಟಕ()[idx].sg_gui = 0;
    VIM_CLEAR(HL_ಕೋಷ್ಟಕ()[idx].sg_gui_fg_name);
    VIM_CLEAR(HL_ಕೋಷ್ಟಕ()[idx].sg_gui_bg_name);
    VIM_CLEAR(HL_ಕೋಷ್ಟಕ()[idx].sg_gui_sp_name);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_ಕೋಷ್ಟಕ()[idx].sg_gui_fg = INVALCOLOR;
    HL_ಕೋಷ್ಟಕ()[idx].sg_gui_bg = INVALCOLOR;
    HL_ಕೋಷ್ಟಕ()[idx].sg_gui_sp = INVALCOLOR;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    gui_mch_free_font(HL_ಕೋಷ್ಟಕ()[idx].sg_font);
    HL_ಕೋಷ್ಟಕ()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
    gui_mch_free_fontset(HL_ಕೋಷ್ಟಕ()[idx].sg_fontset);
    HL_ಕೋಷ್ಟಕ()[idx].sg_fontset = NOFONTSET;
# endif
    VIM_CLEAR(HL_ಕೋಷ್ಟಕ()[idx].sg_font_name);
    HL_ಕೋಷ್ಟಕ()[idx].sg_gui_attr = 0;
#ಆದರೆ_ಅಂತ್ಯ
    // Restore default link and context if they exist. Otherwise clears.
    HL_ಕೋಷ್ಟಕ()[idx].sg_link = HL_ಕೋಷ್ಟಕ()[idx].sg_deflink;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // Since we set the default link, set the location to where the default
    // link was set.
    HL_ಕೋಷ್ಟಕ()[idx].sg_script_ctx = HL_ಕೋಷ್ಟಕ()[idx].sg_deflink_sctx;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Set the normal foreground and background colors according to the "Normal"
 * highlighting group.  For X11 also set "Menu", "Scrollbar" and "Tooltip"
 * colors.  For MS-Windows also set "TitleBar" and "TitleBarNC" colors.
 */
    ಶೂನ್ಯ
ಗಣ_ಸಾಮಾನ್ಯ_colors(ಶೂನ್ಯ)
{
# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    ಆದರೆ (gui.ಒಳ_use)
#  endif
    {
	ಆದರೆ (ಗಣ_ಗುಂಪು_colors((char_u *)"Normal",
				 &gui.norm_pixel, &gui.ಹಿಂಭಾಗ_pixel,
				 FALSE, TRUE, FALSE))
	{
	    gui_mch_new_colors();
	    ಗಣ_must_redraw(UPD_CLEAR);
	}
#  ifdef FEAT_GUI_MSWIN
	ಆದರೆ (ಗಣ_ಗುಂಪು_colors((char_u *)"TitleBar",
				 &gui.title_fg_pixel, &gui.title_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_ಗಣ_titlebar_colors();
	    ಗಣ_must_redraw(UPD_CLEAR);
	}
	ಆದರೆ (ಗಣ_ಗುಂಪು_colors((char_u *)"TitleBarNC",
				 &gui.titlenc_fg_pixel, &gui.titlenc_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_ಗಣ_titlebar_colors();
	    ಗಣ_must_redraw(UPD_CLEAR);
	}
#  endif
#  ifdef FEAT_GUI_X11
	ಆದರೆ (ಗಣ_ಗುಂಪು_colors((char_u *)"Menu",
			     &gui.menu_fg_pixel, &gui.menu_bg_pixel,
			     TRUE, FALSE, FALSE))
	{
#   ifdef FEAT_MENU
	    gui_mch_new_menu_colors();
#   endif
	    ಗಣ_must_redraw(UPD_CLEAR);
	}
#   ifdef FEAT_BEVAL_GUI
	ಆದರೆ (ಗಣ_ಗುಂಪು_colors((char_u *)"Tooltip",
			     &gui.tooltip_fg_pixel, &gui.tooltip_bg_pixel,
			     FALSE, FALSE, TRUE))
	{
#    ifdef FEAT_TOOLBAR
	    gui_mch_new_tooltip_colors();
#    endif
	    ಗಣ_must_redraw(UPD_CLEAR);
	}
#   endif
	ಆದರೆ (ಗಣ_ಗುಂಪು_colors((char_u *)"Scrollbar",
			&gui.scroll_fg_pixel, &gui.scroll_bg_pixel,
			FALSE, FALSE, FALSE))
	{
	    gui_new_scrollbar_colors();
	    ಗಣ_must_redraw(UPD_CLEAR);
	}
#  endif
    }
# endif
# ifdef FEAT_TERMGUICOLORS
#  ifdef FEAT_GUI
    ಇಲ್ಲದಿದ್ದರೆ
#  endif
    {
	ಪೂರ್ಣಾಂಕ		idx;

	idx = syn_name2id((char_u *)"Normal") - 1;
	ಆದರೆ (idx >= 0)
	{
	    gui_do_one_color(idx, FALSE, FALSE);

	    // If the normal fg or bg color changed a complete redraw is
	    // required.
	    ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_fg_gui_color != HL_ಕೋಷ್ಟಕ()[idx].sg_gui_fg
		    || cterm_ಸಾಮಾನ್ಯ_bg_gui_color != HL_ಕೋಷ್ಟಕ()[idx].sg_gui_bg)
	    {
		// if the GUI color is INVALCOLOR then we use the default cterm
		// color
		cterm_ಸಾಮಾನ್ಯ_fg_gui_color = HL_ಕೋಷ್ಟಕ()[idx].sg_gui_fg;
		cterm_ಸಾಮಾನ್ಯ_bg_gui_color = HL_ಕೋಷ್ಟಕ()[idx].sg_gui_bg;
		ಗಣ_must_redraw(UPD_CLEAR);
	    }
	}
    }
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI)
/*
 * Set the colors for "Normal", "Menu", "TitleBar", "TitleBarNC", "Tooltip" or
 * "Scrollbar".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_ಗುಂಪು_colors(
    char_u	*name,
    guicolor_T	*fgp,
    guicolor_T	*bgp,
    ಪೂರ್ಣಾಂಕ		do_menu,
    ಪೂರ್ಣಾಂಕ		use_norm,
    ಪೂರ್ಣಾಂಕ		do_tooltip)
{
    ಪೂರ್ಣಾಂಕ		idx;

    idx = syn_name2id(name) - 1;
    ಆದರೆ (idx < 0)
	ಹಿಂತಿರುಗಿ FALSE;

    gui_do_one_color(idx, do_menu, do_tooltip);

    ಆದರೆ (HL_ಕೋಷ್ಟಕ()[idx].sg_gui_fg != INVALCOLOR)
	*fgp = HL_ಕೋಷ್ಟಕ()[idx].sg_gui_fg;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (use_norm)
	*fgp = gui.def_norm_pixel;
    ಆದರೆ (HL_ಕೋಷ್ಟಕ()[idx].sg_gui_bg != INVALCOLOR)
	*bgp = HL_ಕೋಷ್ಟಕ()[idx].sg_gui_bg;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (use_norm)
	*bgp = gui.def_ಹಿಂಭಾಗ_pixel;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Get the font of the "Normal" group.
 * Returns "" when it's not found or not set.
 */
    char_u *
hl_get_font_name(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		id;
    char_u	*s;

    id = syn_name2id((char_u *)"Normal");
    ಆದರೆ (id > 0)
    {
	s = HL_ಕೋಷ್ಟಕ()[id - 1].sg_font_name;
	ಆದರೆ (s != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ s;
    }
    ಹಿಂತಿರುಗಿ (char_u *)"";
}

/*
 * Set font for "Normal" group.  Called by gui_mch_init_font() when a font has
 * actually chosen to be used.
 */
    ಶೂನ್ಯ
hl_ಗಣ_font_name(char_u *font_name)
{
    ಪೂರ್ಣಾಂಕ	    id;

    id = syn_name2id((char_u *)"Normal");
    ಆದರೆ (id <= 0)
	ಹಿಂತಿರುಗಿ;

    vim_free(HL_ಕೋಷ್ಟಕ()[id - 1].sg_font_name);
    HL_ಕೋಷ್ಟಕ()[id - 1].sg_font_name = vim_strsave(font_name);
}

/*
 * Set background color for "Normal" group.  Called by gui_set_bg_color()
 * when the color is known.
 */
    ಶೂನ್ಯ
hl_ಗಣ_bg_color_name(
    char_u  *name)	    // must have been allocated
{
    ಪೂರ್ಣಾಂಕ	    id;

    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    id = syn_name2id((char_u *)"Normal");
    ಆದರೆ (id <= 0)
	ಹಿಂತಿರುಗಿ;

    vim_free(HL_ಕೋಷ್ಟಕ()[id - 1].sg_gui_bg_name);
    HL_ಕೋಷ್ಟಕ()[id - 1].sg_gui_bg_name = name;
}

/*
 * Set foreground color for "Normal" group.  Called by gui_set_fg_color()
 * when the color is known.
 */
    ಶೂನ್ಯ
hl_ಗಣ_fg_color_name(
    char_u  *name)	    // must have been allocated
{
    ಪೂರ್ಣಾಂಕ	    id;

    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    id = syn_name2id((char_u *)"Normal");
    ಆದರೆ (id <= 0)
	ಹಿಂತಿರುಗಿ;

    vim_free(HL_ಕೋಷ್ಟಕ()[id - 1].sg_gui_fg_name);
    HL_ಕೋಷ್ಟಕ()[id - 1].sg_gui_fg_name = name;
}

/*
 * Return the handle for a font name.
 * Returns NOFONT when failed.
 */
    ಸ್ಥಿರ GuiFont
font_name2handle(char_u *name)
{
    ಆದರೆ (STRCMP(name, "NONE") == 0)
	ಹಿಂತಿರುಗಿ NOFONT;

    ಹಿಂತಿರುಗಿ gui_mch_get_font(name, TRUE);
}

# ifdef FEAT_XFONTSET
/*
 * Return the handle for a fontset name.
 * Returns NOFONTSET when failed.
 */
    ಸ್ಥಿರ GuiFontset
fontset_name2handle(char_u *name, ಪೂರ್ಣಾಂಕ fixed_ಅಗಲ)
{
    ಆದರೆ (STRCMP(name, "NONE") == 0)
	ಹಿಂತಿರುಗಿ NOFONTSET;

    ಹಿಂತಿರುಗಿ gui_mch_get_fontset(name, TRUE, fixed_ಅಗಲ);
}
# endif

/*
 * Get the font or fontset for one highlight group.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hl_do_font(
    ಪೂರ್ಣಾಂಕ		idx,
    char_u	*arg,
    ಪೂರ್ಣಾಂಕ		do_ಸಾಮಾನ್ಯ,		// set normal font
    ಪೂರ್ಣಾಂಕ		do_menu ಬಳಸದ,		// set menu font
    ಪೂರ್ಣಾಂಕ		do_tooltip ಬಳಸದ,	// set tooltip font
    ಪೂರ್ಣಾಂಕ		free_font)		// free current font/fontset
{
# ifdef FEAT_XFONTSET
    // If 'guifontset' is not empty, first try using the name as a
    // fontset.  If that doesn't work, use it as a font name.
    ಆದರೆ (*p_guifontset != NUL
#  ifdef FONTSET_ALWAYS
	|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
	// In Motif, the Tooltip highlight group is always a fontset
	|| do_tooltip
#  endif
	    )
    {
	ಆದರೆ (free_font)
	    gui_mch_free_fontset(HL_ಕೋಷ್ಟಕ()[idx].sg_fontset);
	HL_ಕೋಷ್ಟಕ()[idx].sg_fontset = fontset_name2handle(arg, 0
#  ifdef FONTSET_ALWAYS
		|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
		|| do_tooltip
#  endif
		);
    }
    ಆದರೆ (HL_ಕೋಷ್ಟಕ()[idx].sg_fontset != NOFONTSET)
    {
	// If it worked and it's the Normal group, use it as the normal
	// fontset.  Same for the Menu group.
	ಆದರೆ (do_ಸಾಮಾನ್ಯ)
	    gui_init_font(arg, TRUE);
#  ಆದರೆ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	ಆದರೆ (do_menu)
	{
#   ifdef FONTSET_ALWAYS
	    gui.menu_fontset = HL_ಕೋಷ್ಟಕ()[idx].sg_fontset;
#   ಇಲ್ಲದಿದ್ದರೆ
	    // YIKES!  This is a bug waiting to crash the program
	    gui.menu_font = HL_ಕೋಷ್ಟಕ()[idx].sg_fontset;
#   endif
	    gui_mch_new_menu_font();
	}
#   ifdef FEAT_BEVAL_GUI
	ಆದರೆ (do_tooltip)
	{
	    // The Athena widget set could not handle switching between
	    // displaying a single font and a fontset.
	    // If the XtNinternational resource is set to True at widget
	    // creation, then a fontset is always used, otherwise an
	    // XFontStruct is used.
	    gui.tooltip_fontset = (XFont_ಗಣ)HL_ಕೋಷ್ಟಕ()[idx].sg_fontset;
	    gui_mch_new_tooltip_font();
	}
#   endif
#  endif
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
	ಆದರೆ (free_font)
	    gui_mch_free_font(HL_ಕೋಷ್ಟಕ()[idx].sg_font);
	HL_ಕೋಷ್ಟಕ()[idx].sg_font = font_name2handle(arg);
	// If it worked and it's the Normal group, use it as the
	// normal font.  Same for the Menu group.
	ಆದರೆ (HL_ಕೋಷ್ಟಕ()[idx].sg_font != NOFONT)
	{
	    ಆದರೆ (do_ಸಾಮಾನ್ಯ)
		gui_init_font(arg, FALSE);
# ifndef FONTSET_ALWAYS
#  ಆದರೆ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	    ಆದರೆ (do_menu)
	    {
		gui.menu_font = HL_ಕೋಷ್ಟಕ()[idx].sg_font;
		gui_mch_new_menu_font();
	    }
#  endif
# endif
	}
    }
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI

#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return the handle for a color name.
 * Returns INVALCOLOR when failed.
 */
    guicolor_T
color_name2handle(char_u *name)
{
    ಆದರೆ (STRCMP(name, "NONE") == 0)
	ಹಿಂತಿರುಗಿ INVALCOLOR;

    ಆದರೆ (STRICMP(name, "fg") == 0 || STRICMP(name, "foreground") == 0)
    {
# ಆದರೆ defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	ಆದರೆ (gui.ಒಳ_use)
# endif
# ifdef FEAT_GUI
	    ಹಿಂತಿರುಗಿ gui.norm_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_fg_gui_color != INVALCOLOR)
	    ಹಿಂತಿರುಗಿ cterm_ಸಾಮಾನ್ಯ_fg_gui_color;
	// Guess that the foreground is black or white.
	ಹಿಂತಿರುಗಿ GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "black" : "white"));
# endif
    }
    ಆದರೆ (STRICMP(name, "bg") == 0 || STRICMP(name, "background") == 0)
    {
# ಆದರೆ defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	ಆದರೆ (gui.ಒಳ_use)
# endif
# ifdef FEAT_GUI
	    ಹಿಂತಿರುಗಿ gui.ಹಿಂಭಾಗ_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_bg_gui_color != INVALCOLOR)
	    ಹಿಂತಿರುಗಿ cterm_ಸಾಮಾನ್ಯ_bg_gui_color;
	// Guess that the background is white or black.
	ಹಿಂತಿರುಗಿ GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "white" : "black"));
# endif
    }

    ಹಿಂತಿರುಗಿ GUI_GET_COLOR(name);
}

// On MS-Windows an RGB macro is available and it produces 0x00bbggrr color
// values as used by the MS-Windows GDI api.  It should be used only for
// MS-Windows GDI builds.
# ಆದರೆ defined(RGB) && defined(MSWIN) && !defined(FEAT_GUI)
#  undef RGB
# endif
# ifndef RGB
#  define RGB(r, g, b)	(((r)<<16) | ((g)<<8) | (b))
# endif

# ifdef VIMDLL
    ಸ್ಥಿರ guicolor_T
gui_adjust_rgb(guicolor_T c)
{
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ c;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ((c & 0xff) << 16) | (c & 0x00ff00) | ((c >> 16) & 0xff);
}
# ಇಲ್ಲದಿದ್ದರೆ
#  define gui_adjust_rgb(c) (c)
# endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
hex_digit(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (ಸುರಕ್ಷಿತ_isdigit(c))
	ಹಿಂತಿರುಗಿ c - '0';
    c = TOLOWER_ASC(c);
    ಆದರೆ (c >= 'a' && c <= 'f')
	ಹಿಂತಿರುಗಿ c - 'a' + 10;
    ಹಿಂತಿರುಗಿ 0x1ffffff;
}

    ಸ್ಥಿರ guicolor_T
decode_hex_color(char_u *hex)
{
    guicolor_T color;

    ಆದರೆ (hex[0] != '#' || STRLEN(hex) != 7)
	ಹಿಂತಿರುಗಿ INVALCOLOR;

    // Name is in "#rrggbb" format
    color = RGB(((hex_digit(hex[1]) << 4) + hex_digit(hex[2])),
		((hex_digit(hex[3]) << 4) + hex_digit(hex[4])),
		((hex_digit(hex[5]) << 4) + hex_digit(hex[6])));
    ಆದರೆ (color > 0xffffff)
	ಹಿಂತಿರುಗಿ INVALCOLOR;
    ಹಿಂತಿರುಗಿ gui_adjust_rgb(color);
}

# ifdef FEAT_EVAL
// Returns the color currently mapped to the given name or INVALCOLOR if no
// such name exists in the color table. The convention is to use lowercase for
// all keys in the v:colornames dictionary. The value can be either a string in
// the form #rrggbb or a number, either of which is converted to a guicolor_T.
    ಸ್ಥಿರ guicolor_T
colorname2rgb(char_u *name)
{
    dict_T      *colornames_ಕೋಷ್ಟಕ = get_vim_var_dict(VV_COLORNAMES);
    char_u      *lc_name;
    dictitem_T  *colentry;
    char_u      *colstr;
    varnumber_T colnum;

    lc_name = strlow_save(name);
    ಆದರೆ (lc_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ INVALCOLOR;

    colentry = dict_ಕಂಡುಹಿಡಿ(colornames_ಕೋಷ್ಟಕ, lc_name, -1);
    vim_free(lc_name);
    ಆದರೆ (colentry == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ INVALCOLOR;

    ಆದರೆ (colentry->di_tv.v_type == VAR_STRING)
    {
	colstr = tv_get_string_strict(&colentry->di_tv);
	ಆದರೆ ((STRLEN(colstr) == 7) && (*colstr == '#'))
	{
	    ಹಿಂತಿರುಗಿ decode_hex_color(colstr);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    semsg(_(e_bad_color_string_str), colstr);
	    ಹಿಂತಿರುಗಿ INVALCOLOR;
	}
    }

    ಆದರೆ (colentry->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
    {
	colnum = tv_get_ಸಂಖ್ಯೆ(&colentry->di_tv);
	ಹಿಂತಿರುಗಿ (guicolor_T)colnum;
    }

    ಹಿಂತಿರುಗಿ INVALCOLOR;
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
	KEYVALUE_ನಮೂದು(RGB(0x00, 0x00, 0x00), "black"),
	KEYVALUE_ನಮೂದು(RGB(0x00, 0x00, 0xFF), "blue"),
	KEYVALUE_ನಮೂದು(RGB(0xA5, 0x2A, 0x2A), "brown"),
	KEYVALUE_ನಮೂದು(RGB(0x00, 0xFF, 0xFF), "cyan"),
	KEYVALUE_ನಮೂದು(RGB(0x00, 0x00, 0x8B), "darkblue"),
	KEYVALUE_ನಮೂದು(RGB(0x00, 0x8B, 0x8B), "darkcyan"),
	KEYVALUE_ನಮೂದು(RGB(0xA9, 0xA9, 0xA9), "darkgray"),
	KEYVALUE_ನಮೂದು(RGB(0x00, 0x64, 0x00), "darkgreen"),
	KEYVALUE_ನಮೂದು(RGB(0xA9, 0xA9, 0xA9), "darkgrey"),
	KEYVALUE_ನಮೂದು(RGB(0x8B, 0x00, 0x8B), "darkmagenta"),
	KEYVALUE_ನಮೂದು(RGB(0x8B, 0x00, 0x00), "darkred"),
	KEYVALUE_ನಮೂದು(RGB(0x8B, 0x8B, 0x00), "darkyellow"),	 // No X11
	KEYVALUE_ನಮೂದು(RGB(0xBE, 0xBE, 0xBE), "gray"),
	KEYVALUE_ನಮೂದು(RGB(0x00, 0xFF, 0x00), "green"),
	KEYVALUE_ನಮೂದು(RGB(0xBE, 0xBE, 0xBE), "grey"),
	KEYVALUE_ನಮೂದು(RGB(0x66, 0x66, 0x66), "grey40"),
	KEYVALUE_ನಮೂದು(RGB(0x7F, 0x7F, 0x7F), "grey50"),
	KEYVALUE_ನಮೂದು(RGB(0xE5, 0xE5, 0xE5), "grey90"),
	KEYVALUE_ನಮೂದು(RGB(0xAD, 0xD8, 0xE6), "lightblue"),
	KEYVALUE_ನಮೂದು(RGB(0xE0, 0xFF, 0xFF), "lightcyan"),
	KEYVALUE_ನಮೂದು(RGB(0xD3, 0xD3, 0xD3), "lightgray"),
	KEYVALUE_ನಮೂದು(RGB(0x90, 0xEE, 0x90), "lightgreen"),
	KEYVALUE_ನಮೂದು(RGB(0xD3, 0xD3, 0xD3), "lightgrey"),
	KEYVALUE_ನಮೂದು(RGB(0xFF, 0x8B, 0xFF), "lightmagenta"),     // No XX
	KEYVALUE_ನಮೂದು(RGB(0xFF, 0x8B, 0x8B), "lightred"),	 // No XX
	KEYVALUE_ನಮೂದು(RGB(0xFF, 0xFF, 0xE0), "lightyellow"),
	KEYVALUE_ನಮೂದು(RGB(0xFF, 0x00, 0xFF), "magenta"),
	KEYVALUE_ನಮೂದು(RGB(0xFF, 0x00, 0x00), "red"),
	KEYVALUE_ನಮೂದು(RGB(0x2E, 0x8B, 0x57), "seagreen"),
	KEYVALUE_ನಮೂದು(RGB(0xFF, 0xFF, 0xFF), "white"),
	KEYVALUE_ನಮೂದು(RGB(0xFF, 0xFF, 0x00), "yellow")
    };
    keyvalue_T ಗುರಿ;
    keyvalue_T *ನಮೂದು;

    color = decode_hex_color(name);
    ಆದರೆ (color != INVALCOLOR)
	ಹಿಂತಿರುಗಿ color;

    ಗುರಿ.ಕೀಲಿ = 0;
    ಗುರಿ.ಮೌಲ್ಯ.string = name;
    ಗುರಿ.ಮೌಲ್ಯ.ಉದ್ದಳತೆ = 0;	// not used, see cmp_keyvalue_value_i()
    ನಮೂದು = (keyvalue_T *)bsearch(&ಗುರಿ, &rgb_tab, ARRAY_ಉದ್ದಳತೆ(rgb_tab),
	    ಗಾತ್ರ(rgb_tab[0]), cmp_keyvalue_ಮೌಲ್ಯ_i);
    ಆದರೆ (ನಮೂದು != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ gui_adjust_rgb((guicolor_T)ನಮೂದು->ಕೀಲಿ);

# ಆದರೆ defined(FEAT_EVAL)
    /*
     * Not a traditional color. Load additional color aliases and then consult the alias table.
     */

    color = colorname2rgb(name);
    ಆದರೆ (color == INVALCOLOR)
    {
	ತುಂಬು_default_colors_lists();
	color = colorname2rgb(name);
    }

    ಹಿಂತಿರುಗಿ color;
# ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ INVALCOLOR;
# endif
}

    guicolor_T
gui_get_rgb_color_cmn(ಪೂರ್ಣಾಂಕ r, ಪೂರ್ಣಾಂಕ g, ಪೂರ್ಣಾಂಕ b)
{
    guicolor_T  color = RGB(r, g, b);

    ಆದರೆ (color > 0xffffff)
	ಹಿಂತಿರುಗಿ INVALCOLOR;
    ಹಿಂತಿರುಗಿ gui_adjust_rgb(color);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Table with the specifications for an attribute number.
 * Note that this table is used by ALL buffers.  This is required because the
 * GUI can redraw at any time for any buffer.
 */
ಸ್ಥಿರ garray_T	ಪದ_attr_ಕೋಷ್ಟಕ = {0, 0, 0, 0, ಖಾಲಿ};

#ವ್ಯಾಖ್ಯಾನಿಸು ಪದ_ATTR_ನಮೂದು(idx) ((attrentry_T *)ಪದ_attr_ಕೋಷ್ಟಕ.ga_ದತ್ತಾಂಶ)[idx]

ಸ್ಥಿರ garray_T	cterm_attr_ಕೋಷ್ಟಕ = {0, 0, 0, 0, ಖಾಲಿ};

#ವ್ಯಾಖ್ಯಾನಿಸು CTERM_ATTR_ನಮೂದು(idx) ((attrentry_T *)cterm_attr_ಕೋಷ್ಟಕ.ga_ದತ್ತಾಂಶ)[idx]

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
ಸ್ಥಿರ garray_T	gui_attr_ಕೋಷ್ಟಕ = {0, 0, 0, 0, ಖಾಲಿ};

# define GUI_ATTR_ನಮೂದು(idx) ((attrentry_T *)gui_attr_ಕೋಷ್ಟಕ.ga_ದತ್ತಾಂಶ)[idx]
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return the attr number for a set of colors and font.
 * Add a new entry to the term_attr_table, cterm_attr_table or gui_attr_table
 * if the combination is new.
 * Return 0 for error (no more room).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_attr_ನಮೂದು(garray_T *ಕೋಷ್ಟಕ, attrentry_T *aep)
{
    ಪೂರ್ಣಾಂಕ		i;
    attrentry_T	*taep;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = FALSE;

    // Init the table, in case it wasn't done yet.
    ಕೋಷ್ಟಕ->ga_itemsize = ಗಾತ್ರ(attrentry_T);
    ಕೋಷ್ಟಕ->ga_growsize = 7;

    // Try to find an entry with the same specifications.
    ಗಾಗಿ (i = 0; i < ಕೋಷ್ಟಕ->ga_len; ++i)
    {
	taep = &(((attrentry_T *)ಕೋಷ್ಟಕ->ga_ದತ್ತಾಂಶ)[i]);
	ಆದರೆ (	   aep->ae_attr == taep->ae_attr
		&& (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		       (ಕೋಷ್ಟಕ == &gui_attr_ಕೋಷ್ಟಕ
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
#ಆದರೆ_ಅಂತ್ಯ
		       (ಕೋಷ್ಟಕ == &ಪದ_attr_ಕೋಷ್ಟಕ
			&& (aep->ae_u.ಪದ.ಆರಂಭ == ಖಾಲಿ)
					    == (taep->ae_u.ಪದ.ಆರಂಭ == ಖಾಲಿ)
			&& (aep->ae_u.ಪದ.ಆರಂಭ == ಖಾಲಿ
			    || STRCMP(aep->ae_u.ಪದ.ಆರಂಭ,
						  taep->ae_u.ಪದ.ಆರಂಭ) == 0)
			&& (aep->ae_u.ಪದ.stop == ಖಾಲಿ)
					     == (taep->ae_u.ಪದ.stop == ಖಾಲಿ)
			&& (aep->ae_u.ಪದ.stop == ಖಾಲಿ
			    || STRCMP(aep->ae_u.ಪದ.stop,
						  taep->ae_u.ಪದ.stop) == 0))
		    || (ಕೋಷ್ಟಕ == &cterm_attr_ಕೋಷ್ಟಕ
			    && aep->ae_u.cterm.fg_color
						  == taep->ae_u.cterm.fg_color
			    && aep->ae_u.cterm.bg_color
						  == taep->ae_u.cterm.bg_color
			    && aep->ae_u.cterm.ul_color
						  == taep->ae_u.cterm.ul_color
			    && aep->ae_u.cterm.font
						  == taep->ae_u.cterm.font
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
			    && aep->ae_u.cterm.fg_rgb
						    == taep->ae_u.cterm.fg_rgb
			    && aep->ae_u.cterm.bg_rgb
						    == taep->ae_u.cterm.bg_rgb
			    && aep->ae_u.cterm.ul_rgb
						    == taep->ae_u.cterm.ul_rgb
#ಆದರೆ_ಅಂತ್ಯ
		       )))

	ಹಿಂತಿರುಗಿ i + ATTR_OFF;
    }

    ಆದರೆ (ಕೋಷ್ಟಕ->ga_len + ATTR_OFF > MAX_TYPENR)
    {
	// Running out of attribute entries!  remove all attributes, and
	// compute new ones for all groups.
	// When called recursively, we are really out of numbers.
	ಆದರೆ (recursive)
	{
	    emsg(_(e_too_many_different_highlighting_ಗುಣಲಕ್ಷಣಗಳು_ಒಳ_use));
	    ಹಿಂತಿರುಗಿ 0;
	}
	recursive = TRUE;

	clear_hl_tables();

	ಗಣ_must_redraw(UPD_CLEAR);

	ಗಾಗಿ (i = 0; i < highlight_ga.ga_len; ++i)
	    ಗಣ_hl_attr(i);

	recursive = FALSE;
    }

    // This is a new combination of colors and font, add an entry.
    ಆದರೆ (ga_grow(ಕೋಷ್ಟಕ, 1) == FAIL)
	ಹಿಂತಿರುಗಿ 0;

    taep = &(((attrentry_T *)ಕೋಷ್ಟಕ->ga_ದತ್ತಾಂಶ)[ಕೋಷ್ಟಕ->ga_len]);
    CLEAR_ಸೂಚಕ(taep);
    taep->ae_attr = aep->ae_attr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (ಕೋಷ್ಟಕ == &gui_attr_ಕೋಷ್ಟಕ)
    {
	taep->ae_u.gui.fg_color = aep->ae_u.gui.fg_color;
	taep->ae_u.gui.bg_color = aep->ae_u.gui.bg_color;
	taep->ae_u.gui.sp_color = aep->ae_u.gui.sp_color;
	taep->ae_u.gui.font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
	taep->ae_u.gui.fontset = aep->ae_u.gui.fontset;
# endif
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಕೋಷ್ಟಕ == &ಪದ_attr_ಕೋಷ್ಟಕ)
    {
	ಆದರೆ (aep->ae_u.ಪದ.ಆರಂಭ == ಖಾಲಿ)
	    taep->ae_u.ಪದ.ಆರಂಭ = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    taep->ae_u.ಪದ.ಆರಂಭ = vim_strsave(aep->ae_u.ಪದ.ಆರಂಭ);
	ಆದರೆ (aep->ae_u.ಪದ.stop == ಖಾಲಿ)
	    taep->ae_u.ಪದ.stop = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    taep->ae_u.ಪದ.stop = vim_strsave(aep->ae_u.ಪದ.stop);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೋಷ್ಟಕ == &cterm_attr_ಕೋಷ್ಟಕ)
    {
	taep->ae_u.cterm.fg_color = aep->ae_u.cterm.fg_color;
	taep->ae_u.cterm.bg_color = aep->ae_u.cterm.bg_color;
	taep->ae_u.cterm.ul_color = aep->ae_u.cterm.ul_color;
	taep->ae_u.cterm.font = aep->ae_u.cterm.font;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
	taep->ae_u.cterm.fg_rgb = aep->ae_u.cterm.fg_rgb;
	taep->ae_u.cterm.bg_rgb = aep->ae_u.cterm.bg_rgb;
	taep->ae_u.cterm.ul_rgb = aep->ae_u.cterm.ul_rgb;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ++ಕೋಷ್ಟಕ->ga_len;
    ಹಿಂತಿರುಗಿ (ಕೋಷ್ಟಕ->ga_len - 1 + ATTR_OFF);
}

#ಆದರೆ defined(FEAT_TERMINAL)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    ಪೂರ್ಣಾಂಕ
get_cterm_attr_idx(ಪೂರ್ಣಾಂಕ attr, ಪೂರ್ಣಾಂಕ fg, ಪೂರ್ಣಾಂಕ bg)
{
    attrentry_T		at_en;

    CLEAR_ಕ್ಷೇತ್ರ(at_en);
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
    ಹಿಂತಿರುಗಿ get_attr_ನಮೂದು(&cterm_attr_ಕೋಷ್ಟಕ, &at_en);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_TERMINAL) && defined(FEAT_TERMGUICOLORS)
/*
 * Get an attribute index for a 'termguicolors' entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    ಪೂರ್ಣಾಂಕ
get_tgc_attr_idx(ಪೂರ್ಣಾಂಕ attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_ಕ್ಷೇತ್ರ(at_en);
    at_en.ae_attr = attr;
    ಆದರೆ (fg == INVALCOLOR && bg == INVALCOLOR)
    {
	// If both GUI colors are not set fall back to the cterm colors.  Helps
	// if the GUI only has an attribute, such as undercurl.
	at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	at_en.ae_u.cterm.fg_rgb = fg;
	at_en.ae_u.cterm.bg_rgb = bg;
    }
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
    ಹಿಂತಿರುಗಿ get_attr_ನಮೂದು(&cterm_attr_ಕೋಷ್ಟಕ, &at_en);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_TERMINAL) && defined(FEAT_GUI)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    ಪೂರ್ಣಾಂಕ
get_gui_attr_idx(ಪೂರ್ಣಾಂಕ attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_ಕ್ಷೇತ್ರ(at_en);
    at_en.ae_attr = attr;
    at_en.ae_u.gui.fg_color = fg;
    at_en.ae_u.gui.bg_color = bg;
    ಹಿಂತಿರುಗಿ get_attr_ನಮೂದು(&gui_attr_ಕೋಷ್ಟಕ, &at_en);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Clear all highlight tables.
 */
    ಶೂನ್ಯ
clear_hl_tables(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    attrentry_T	*taep;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ga_clear(&gui_attr_ಕೋಷ್ಟಕ);
#ಆದರೆ_ಅಂತ್ಯ
    ಗಾಗಿ (i = 0; i < ಪದ_attr_ಕೋಷ್ಟಕ.ga_len; ++i)
    {
	taep = &(((attrentry_T *)ಪದ_attr_ಕೋಷ್ಟಕ.ga_ದತ್ತಾಂಶ)[i]);
	vim_free(taep->ae_u.ಪದ.ಆರಂಭ);
	vim_free(taep->ae_u.ಪದ.stop);
    }
    ga_clear(&ಪದ_attr_ಕೋಷ್ಟಕ);
    ga_clear(&cterm_attr_ಕೋಷ್ಟಕ);
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
    ಪೂರ್ಣಾಂಕ
hl_combine_attr(ಪೂರ್ಣಾಂಕ char_attr, ಪೂರ್ಣಾಂಕ prim_attr)
{
    attrentry_T *char_aep = ಖಾಲಿ;
    attrentry_T *prim_aep;
    attrentry_T new_en;

    ಆದರೆ (char_attr == 0)
	ಹಿಂತಿರುಗಿ prim_attr;
    ಆದರೆ (char_attr <= HL_ALL && prim_attr <= HL_ALL)
	ಹಿಂತಿರುಗಿ ATTR_COMBINE(char_attr, prim_attr);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	ಆದರೆ (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	ಆದರೆ (char_aep != ಖಾಲಿ)
	    new_en = *char_aep;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    CLEAR_ಕ್ಷೇತ್ರ(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    ಆದರೆ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	ಆದರೆ (prim_attr <= HL_ALL)
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    prim_aep = syn_gui_attr2entry(prim_attr);
	    ಆದರೆ (prim_aep != ಖಾಲಿ)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		ಆದರೆ (prim_aep->ae_u.gui.fg_color != INVALCOLOR)
		    new_en.ae_u.gui.fg_color = prim_aep->ae_u.gui.fg_color;
		ಆದರೆ (prim_aep->ae_u.gui.bg_color != INVALCOLOR)
		    new_en.ae_u.gui.bg_color = prim_aep->ae_u.gui.bg_color;
		ಆದರೆ (prim_aep->ae_u.gui.sp_color != INVALCOLOR)
		    new_en.ae_u.gui.sp_color = prim_aep->ae_u.gui.sp_color;
		ಆದರೆ (prim_aep->ae_u.gui.font != NOFONT)
		    new_en.ae_u.gui.font = prim_aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
		ಆದರೆ (prim_aep->ae_u.gui.fontset != NOFONTSET)
		    new_en.ae_u.gui.fontset = prim_aep->ae_u.gui.fontset;
# endif
	    }
	}
	ಹಿಂತಿರುಗಿ get_attr_ನಮೂದು(&gui_attr_ಕೋಷ್ಟಕ, &new_en);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (IS_CTERM)
    {
	ಆದರೆ (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	ಆದರೆ (char_aep != ಖಾಲಿ)
	    new_en = *char_aep;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    CLEAR_ಕ್ಷೇತ್ರ(new_en);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	ಆದರೆ (prim_attr <= HL_ALL)
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    prim_aep = syn_cterm_attr2entry(prim_attr);
	    ಆದರೆ (prim_aep != ಖಾಲಿ)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		ಆದರೆ (prim_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color = prim_aep->ae_u.cterm.fg_color;
		ಆದರೆ (prim_aep->ae_u.cterm.bg_color > 0)
		    new_en.ae_u.cterm.bg_color = prim_aep->ae_u.cterm.bg_color;
		ಆದರೆ (prim_aep->ae_u.cterm.ul_color > 0)
		    new_en.ae_u.cterm.ul_color = prim_aep->ae_u.cterm.ul_color;
		ಆದರೆ (prim_aep->ae_u.cterm.font > 0)
		    new_en.ae_u.cterm.font = prim_aep->ae_u.cterm.font;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
		// If both fg and bg are not set fall back to cterm colors.
		// Helps for SpellBad which uses undercurl in the GUI.
		ಆದರೆ (COLOR_INVALID(prim_aep->ae_u.cterm.fg_rgb)
			&& COLOR_INVALID(prim_aep->ae_u.cterm.bg_rgb))
		{
		    ಆದರೆ (prim_aep->ae_u.cterm.fg_color > 0)
			new_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
		    ಆದರೆ (prim_aep->ae_u.cterm.bg_color > 0)
			new_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (prim_aep->ae_u.cterm.fg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.fg_rgb = prim_aep->ae_u.cterm.fg_rgb;
		    ಆದರೆ (prim_aep->ae_u.cterm.bg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.bg_rgb = prim_aep->ae_u.cterm.bg_rgb;
		}
		ಆದರೆ (prim_aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		    new_en.ae_u.cterm.ul_rgb = prim_aep->ae_u.cterm.ul_rgb;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	}
	ಹಿಂತಿರುಗಿ get_attr_ನಮೂದು(&cterm_attr_ಕೋಷ್ಟಕ, &new_en);
    }

    ಆದರೆ (char_attr > HL_ALL)
	char_aep = syn_ಪದ_attr2entry(char_attr);
    ಆದರೆ (char_aep != ಖಾಲಿ)
	new_en = *char_aep;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	CLEAR_ಕ್ಷೇತ್ರ(new_en);
	ಆದರೆ (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    ಆದರೆ (prim_attr <= HL_ALL)
	new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	prim_aep = syn_ಪದ_attr2entry(prim_attr);
	ಆದರೆ (prim_aep != ಖಾಲಿ)
	{
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_aep->ae_attr);
	    ಆದರೆ (prim_aep->ae_u.ಪದ.ಆರಂಭ != ಖಾಲಿ)
	    {
		new_en.ae_u.ಪದ.ಆರಂಭ = prim_aep->ae_u.ಪದ.ಆರಂಭ;
		new_en.ae_u.ಪದ.stop = prim_aep->ae_u.ಪದ.stop;
	    }
	}
    }
    ಹಿಂತಿರುಗಿ get_attr_ನಮೂದು(&ಪದ_attr_ಕೋಷ್ಟಕ, &new_en);
}

// ANSI color order: Black, Red, Green, Yellow, Blue, Magenta,
// Cyan, White, then bright variants.  Approximate RGB values used when
// only a cterm color number is known (no guifg/guibg).  Real terminal
// palettes may differ if the user reconfigured their emulator, but
// these are reasonable xterm-ish defaults.
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ long_u cterm_color_16[16] = {
    0x000000, 0xc00000, 0x008000, 0x808000,
    0x0000c0, 0xc000c0, 0x004080, 0xc0c0c0,
    0x808080, 0xff8080, 0x00ff00, 0xffff00,
    0x6060ff, 0xff40ff, 0x00ffff, 0xffffff
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
/*
 * Convert a cterm color number (1-16) to an RGB value.
 * Used as a fallback when 'termguicolors' is set but only cterm colors are
 * specified (no guifg/guibg).
 * Returns INVALCOLOR if the color number is out of range.
 */
    ಸ್ಥಿರ guicolor_T
cterm_color_ಗಮ್ಯ_rgb(ಪೂರ್ಣಾಂಕ color_nr)
{
    ಆದರೆ (color_nr < 1 || color_nr > 16)
	ಹಿಂತಿರುಗಿ INVALCOLOR;
    ಹಿಂತಿರುಗಿ (guicolor_T)cterm_color_16[color_nr - 1];
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Convert an xterm 256-color index (0-255) to an approximate RGB triple.
 * Uses the standard xterm palette: 0-15 ANSI (cterm_color_16), 16-231
 * 6x6x6 cube, 232-255 grayscale ramp.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cterm_idx_ಗಮ್ಯ_rgb(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ *r, ಪೂರ್ಣಾಂಕ *g, ಪೂರ್ಣಾಂಕ *b)
{
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

    ಆದರೆ (idx < 0 || idx > 255)
    {
	*r = *g = *b = 0;
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (idx < 16)
    {
	long_u rgb = cterm_color_16[idx];
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx < 232)
    {
	ಪೂರ್ಣಾಂಕ n = idx - 16;
	*r = cube[n / 36 % 6];
	*g = cube[n / 6  % 6];
	*b = cube[n      % 6];
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ v = 8 + (idx - 232) * 10;
	*r = *g = *b = v;
    }
}

/*
 * Approximate an RGB triple to the nearest xterm 256-color index.
 * Searches the 6x6x6 cube and the grayscale ramp; returns 0-255.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
rgb_ಗಮ್ಯ_cterm_idx(ಪೂರ್ಣಾಂಕ r, ಪೂರ್ಣಾಂಕ g, ಪೂರ್ಣಾಂಕ b)
{
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };
    ಪೂರ್ಣಾಂಕ ಉತ್ತಮ = 0;
    ದೀರ್ಘ ಉತ್ತಮ_d = -1;
    ಪೂರ್ಣಾಂಕ i;

    // Search the 6x6x6 cube.
    ಗಾಗಿ (i = 16; i < 232; ++i)
    {
	ಪೂರ್ಣಾಂಕ n = i - 16;
	ಪೂರ್ಣಾಂಕ cr = cube[n / 36 % 6];
	ಪೂರ್ಣಾಂಕ cg = cube[n / 6  % 6];
	ಪೂರ್ಣಾಂಕ cb = cube[n      % 6];
	ದೀರ್ಘ d = (cr - r) * (cr - r) + (cg - g) * (cg - g) + (cb - b) * (cb - b);
	ಆದರೆ (ಉತ್ತಮ_d < 0 || d < ಉತ್ತಮ_d)
	{
	    ಉತ್ತಮ_d = d;
	    ಉತ್ತಮ = i;
	}
    }
    // Search the grayscale ramp.
    ಗಾಗಿ (i = 232; i < 256; ++i)
    {
	ಪೂರ್ಣಾಂಕ v = 8 + (i - 232) * 10;
	ದೀರ್ಘ d = (v - r) * (v - r) + (v - g) * (v - g) + (v - b) * (v - b);
	ಆದರೆ (d < ಉತ್ತಮ_d)
	{
	    ಉತ್ತಮ_d = d;
	    ಉತ್ತಮ = i;
	}
    }
    ಹಿಂತಿರುಗಿ ಉತ್ತಮ;
}

/*
 * Resolve a color to an RGB triple.  Prefer the gui RGB value (set by
 * guifg/guibg) when valid, otherwise convert the cterm 256-color index.
 * Returns true on success; false when no color is defined.
 * cterm_c is 1-based (0 means "no color").
 */
    ಸ್ಥಿರ bool
resolve_color_ಗಮ್ಯ_rgb(ಪೂರ್ಣಾಂಕ cterm_c, guicolor_T rgb ಬಳಸದ, ಪೂರ್ಣಾಂಕ *r, ಪೂರ್ಣಾಂಕ *g, ಪೂರ್ಣಾಂಕ *b)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
    ಆದರೆ (!COLOR_INVALID(rgb))
    {
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
	ಹಿಂತಿರುಗಿ true;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (cterm_c > 0)
    {
	cterm_idx_ಗಮ್ಯ_rgb(cterm_c - 1, r, g, b);
	ಹಿಂತಿರುಗಿ true;
    }
    ಹಿಂತಿರುಗಿ false;
}

/*
 * get a RGB fallback color from gui, cterm or default color
 */
    ಸ್ಥಿರ guicolor_T
resolve_fallback_color(ಪೂರ್ಣಾಂಕ cterm_c, guicolor_T rgb, guicolor_T default_rgb)
{
    ಪೂರ್ಣಾಂಕ red, green, blue;
    ಆದರೆ (!resolve_color_ಗಮ್ಯ_rgb(cterm_c, rgb, &red, &green, &blue))
	ಹಿಂತಿರುಗಿ default_rgb;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ (red << 16) | (green << 8) | blue;
}

/*
 * get a RGB fallback foreground color from guifg, ctermfg or deduced from background
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
resolve_fallback_fg_ಗಮ್ಯ_rgb(ಶೂನ್ಯ)
{
    guicolor_T fgcolor_or_gui_fgcolor = INVALCOLOR;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
    fgcolor_or_gui_fgcolor = cterm_ಸಾಮಾನ್ಯ_fg_gui_color;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	fgcolor_or_gui_fgcolor = gui.norm_pixel;
#ಆದರೆ_ಅಂತ್ಯ
    fallback_fg_rgb = resolve_fallback_color(cterm_ಸಾಮಾನ್ಯ_fg_color, fgcolor_or_gui_fgcolor, (*p_bg == 'l') ? 0x000000 : 0xFFFFFF);
}

/*
 * get a RGB fallback background color from guifg, ctermbg or deduced from background
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
resolve_fallback_bg_ಗಮ್ಯ_rgb(ಶೂನ್ಯ)
{
    guicolor_T bgcolor_or_gui_bgcolor = INVALCOLOR;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
    bgcolor_or_gui_bgcolor = cterm_ಸಾಮಾನ್ಯ_bg_gui_color;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	bgcolor_or_gui_bgcolor = gui.ಹಿಂಭಾಗ_pixel;
#ಆದರೆ_ಅಂತ್ಯ
    fallback_bg_rgb = resolve_fallback_color(cterm_ಸಾಮಾನ್ಯ_bg_color, bgcolor_or_gui_bgcolor, (*p_bg == 'l') ? 0xFFFFFF : 0x000000);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
blend_cterm_colors(ಪೂರ್ಣಾಂಕ popup_c, guicolor_T popup_rgb,
		   ಪೂರ್ಣಾಂಕ under_c, guicolor_T under_rgb,
		   ಪೂರ್ಣಾಂಕ default_rgb,
		   ಪೂರ್ಣಾಂಕ blend_val)
{
    ಪೂರ್ಣಾಂಕ pr, pg, pb, ur, ug, ub, r, g, b;

    ಆದರೆ (t_colors < 256)
	ಹಿಂತಿರುಗಿ popup_c;
    ಆದರೆ (!resolve_color_ಗಮ್ಯ_rgb(popup_c, popup_rgb, &pr, &pg, &pb))
	ಹಿಂತಿರುಗಿ under_c;
    ಆದರೆ (blend_val <= 0)
	ಹಿಂತಿರುಗಿ rgb_ಗಮ್ಯ_cterm_idx(pr, pg, pb) + 1;
    ಆದರೆ (!resolve_color_ಗಮ್ಯ_rgb(under_c, under_rgb, &ur, &ug, &ub))
    {
	ur = (default_rgb >> 16) & 0xFF;
	ug = (default_rgb >> 8) & 0xFF;
	ub = default_rgb & 0xFF;
    }
    ಆದರೆ (blend_val >= 100)
	ಹಿಂತಿರುಗಿ rgb_ಗಮ್ಯ_cterm_idx(ur, ug, ub) + 1;
    r = pr + (ur - pr) * blend_val / 100;
    g = pg + (ug - pg) * blend_val / 100;
    b = pb + (ub - pb) * blend_val / 100;
    ಹಿಂತಿರುಗಿ rgb_ಗಮ್ಯ_cterm_idx(r, g, b) + 1;
}

#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Blend two RGB colors based on blend value (0-100).
 * blend: 0=return first argument color, 100=return second argument color
 * If bg_color is INVALCOLOR, high blend means more visible (return INVALCOLOR).
 */
    ಸ್ಥಿರ guicolor_T
blend_colors(guicolor_T popup_color, guicolor_T bg_color, ಪೂರ್ಣಾಂಕ blend_val)
{
    ಪೂರ್ಣಾಂಕ r1, g1, b1, r2, g2, b2, r, g, b;

    // CTERMCOLOR is a sentinel meaning "use the cterm color"; for blending
    // it has no real RGB so treat it like INVALCOLOR.
    ಆದರೆ (COLOR_INVALID(popup_color))
	ಹಿಂತಿರುಗಿ INVALCOLOR;

    // Fully transparent: use underlying color as-is.
    ಆದರೆ (blend_val >= 100)
	ಹಿಂತಿರುಗಿ bg_color;

    r1 = (popup_color >> 16) & 0xFF;
    g1 = (popup_color >> 8) & 0xFF;
    b1 = popup_color & 0xFF;

    ಆದರೆ (COLOR_INVALID(bg_color))
	bg_color = fallback_bg_rgb;

    r2 = (bg_color >> 16) & 0xFF;
    g2 = (bg_color >> 8) & 0xFF;
    b2 = bg_color & 0xFF;

    r = r1 + (r2 - r1) * blend_val / 100;
    g = g1 + (g2 - g1) * blend_val / 100;
    b = b1 + (b2 - b1) * blend_val / 100;

    ಹಿಂತಿರುಗಿ (r << 16) | (g << 8) | b;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Blend attributes for popup windows and menus with opacity.
 * Blends foreground and/or background colors based on blend value (0-100).
 * blend: 0 = opaque (use popup colors), 100 = transparent (use background colors)
 * blend_fg: TRUE to blend foreground color, FALSE to keep popup foreground
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
hl_blend_attr_common(
    ಪೂರ್ಣಾಂಕ		char_attr,
    ಪೂರ್ಣಾಂಕ		popup_attr,
    ಪೂರ್ಣಾಂಕ		blend,
    ಪೂರ್ಣಾಂಕ		blend_fg)
{
    attrentry_T *char_aep = ಖಾಲಿ;
    attrentry_T *popup_aep;
    attrentry_T new_en;
    attrentry_T tmp_en;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	ಆದರೆ (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	ಆದರೆ (char_aep != ಖಾಲಿ)
	    new_en = *char_aep;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    CLEAR_ಕ್ಷೇತ್ರ(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    ಆದರೆ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	ಆದರೆ (popup_attr <= HL_ALL)
	{
	    CLEAR_ಕ್ಷೇತ್ರ(tmp_en);
	    tmp_en.ae_u.gui.fg_color = INVALCOLOR;
	    tmp_en.ae_u.gui.bg_color = INVALCOLOR;
	    tmp_en.ae_u.gui.sp_color = INVALCOLOR;
	    // preserve attributes other than color
	    tmp_en.ae_attr = popup_attr;
	    popup_aep = &tmp_en;

	    popup_aep->ae_u.gui.bg_color = fallback_bg_rgb;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    popup_aep = syn_gui_attr2entry(popup_attr);

	ಆದರೆ (popup_aep != ಖಾಲಿ)
	{
	    guicolor_T popup_bg_rgb = popup_aep->ae_u.gui.bg_color;
	    ಆದರೆ (COLOR_INVALID(popup_bg_rgb))
		popup_bg_rgb = fallback_bg_rgb;

	    ಆದರೆ (blend_fg)
	    {
		// blend_fg=TRUE: fade underlying text toward popup bg.
		{
		    ಪೂರ್ಣಾಂಕ ತಳ_fg = fallback_fg_rgb;
		    ಆದರೆ (char_aep != ಖಾಲಿ
			    && char_aep->ae_u.gui.fg_color != INVALCOLOR)
			ತಳ_fg = char_aep->ae_u.gui.fg_color;
		    new_en.ae_u.gui.fg_color = blend_colors(
			    popup_bg_rgb, ತಳ_fg, blend);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
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
		ಆದರೆ (char_aep != ಖಾಲಿ
			&& !COLOR_INVALID(char_aep->ae_u.gui.bg_color))
		    underlying_bg = char_aep->ae_u.gui.bg_color;
		new_en.ae_u.gui.bg_color = blend_colors(
			popup_bg_rgb,
			underlying_bg, blend);
	    }
	}
	ಹಿಂತಿರುಗಿ get_attr_ನಮೂದು(&gui_attr_ಕೋಷ್ಟಕ, &new_en);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (IS_CTERM)
    {
	ಆದರೆ (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	ಆದರೆ (char_aep != ಖಾಲಿ)
	    new_en = *char_aep;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    CLEAR_ಕ್ಷೇತ್ರ(new_en);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	ಆದರೆ (popup_attr <= HL_ALL)
	{
	    CLEAR_ಕ್ಷೇತ್ರ(tmp_en);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
	    tmp_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    tmp_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	    // allow blending with termguicolors
	    tmp_en.ae_u.cterm.bg_rgb = fallback_bg_rgb;
#ಆದರೆ_ಅಂತ್ಯ
	    // preserve attributes other than color
	    tmp_en.ae_attr = popup_attr;
	    popup_aep = &tmp_en;

	    // allow blending with notermguicolors
	    popup_aep->ae_u.cterm.bg_color = cterm_ಸಾಮಾನ್ಯ_bg_color;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    popup_aep = syn_cterm_attr2entry(popup_attr);

	ಆದರೆ (popup_aep != ಖಾಲಿ)
	{
	    guicolor_T popup_bg_rgb = INVALCOLOR;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
	    // Fall back to cterm color converted to RGB when gui color is not set.
	    popup_bg_rgb = popup_aep->ae_u.cterm.bg_rgb;
	    ಆದರೆ (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color > 0)
		popup_bg_rgb = cterm_color_ಗಮ್ಯ_rgb(
			popup_aep->ae_u.cterm.bg_color);
#ಆದರೆ_ಅಂತ್ಯ
	    // assign default color if guibg and ctermbg are not set for popup
	    ಆದರೆ (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color == 0)
		popup_bg_rgb = fallback_bg_rgb;

	    ಆದರೆ (!blend_fg)
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
		ಆದರೆ (popup_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color =
			popup_aep->ae_u.cterm.fg_color;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_fg_color > 0)
		    new_en.ae_u.cterm.fg_color = cterm_ಸಾಮಾನ್ಯ_fg_color;
		ಇಲ್ಲದಿದ್ದರೆ
		    // black-ish or white-ish
		    new_en.ae_u.cterm.fg_color = (*p_bg == 'l') ? 1 : 16;
		new_en.ae_u.cterm.ul_color = popup_aep->ae_u.cterm.ul_color;
		new_en.ae_u.cterm.font = popup_aep->ae_u.cterm.font;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
		new_en.ae_u.cterm.ul_rgb = popup_aep->ae_u.cterm.ul_rgb;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// blend_fg=TRUE: fade underlying fg toward popup bg in
		// the 256-color palette.  Used when the popup is over a
		// cell rendered with cterm colors (no termguicolors RGB).
		ಪೂರ್ಣಾಂಕ under_fg = (char_aep != ಖಾಲಿ)
		    ? char_aep->ae_u.cterm.fg_color : 0;
		guicolor_T under_fg_rgb = INVALCOLOR;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
		ಆದರೆ (char_aep != ಖಾಲಿ)
		    under_fg_rgb = char_aep->ae_u.cterm.fg_rgb;
#ಆದರೆ_ಅಂತ್ಯ
		new_en.ae_u.cterm.fg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_fg, under_fg_rgb, fallback_fg_rgb, blend);
	    }
	    // Approximate cterm bg by blending with the underlying bg
	    // in the 256-color palette and mapping to the nearest entry.
	    {
		ಪೂರ್ಣಾಂಕ under_bg = (char_aep != ಖಾಲಿ)
		    ? char_aep->ae_u.cterm.bg_color : 0;
		guicolor_T under_bg_rgb = INVALCOLOR;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
		ಆದರೆ (char_aep != ಖಾಲಿ)
		    under_bg_rgb = char_aep->ae_u.cterm.bg_rgb;
#ಆದರೆ_ಅಂತ್ಯ
		new_en.ae_u.cterm.bg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_bg, under_bg_rgb, fallback_bg_rgb, blend);
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
	    // Blend RGB colors for termguicolors mode.
	    {
		ಆದರೆ (blend_fg)
		{
		    // blend_fg=TRUE: fade underlying text toward popup bg.
		    ಆದರೆ (popup_bg_rgb != INVALCOLOR)
		    {
			ಪೂರ್ಣಾಂಕ ತಳ_fg = fallback_fg_rgb;
			// CTERMCOLOR is a sentinel meaning "use the cterm
			// color"; treat it as no underlying color so it is
			// not blended in as a real near-white pixel.
			ಆದರೆ (char_aep != ಖಾಲಿ
				&& !COLOR_INVALID(char_aep->ae_u.cterm.fg_rgb))
			    ತಳ_fg = char_aep->ae_u.cterm.fg_rgb;
			new_en.ae_u.cterm.fg_rgb = blend_colors(
				popup_bg_rgb, ತಳ_fg, blend);
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
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

		    ಆದರೆ (COLOR_INVALID(popup_fg)
			    && popup_aep->ae_u.cterm.fg_color > 0)
			popup_fg = cterm_color_ಗಮ್ಯ_rgb(
				popup_aep->ae_u.cterm.fg_color);

		    ಆದರೆ (!COLOR_INVALID(popup_fg))
			new_en.ae_u.cterm.fg_rgb = popup_fg;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!COLOR_INVALID(cterm_ಸಾಮಾನ್ಯ_fg_gui_color))
			new_en.ae_u.cterm.fg_rgb = cterm_ಸಾಮಾನ್ಯ_fg_gui_color;
		    ಇಲ್ಲದಿದ್ದರೆ
			new_en.ae_u.cterm.fg_rgb = fallback_fg_rgb;
		}
		ಆದರೆ (popup_bg_rgb != INVALCOLOR)
		{
		    // Blend popup bg toward underlying bg
		    guicolor_T underlying_bg = fallback_bg_rgb;
		    ಆದರೆ (char_aep != ಖಾಲಿ
			    && !COLOR_INVALID(char_aep->ae_u.cterm.bg_rgb))
			underlying_bg = char_aep->ae_u.cterm.bg_rgb;
		    new_en.ae_u.cterm.bg_rgb = blend_colors(
			    popup_bg_rgb, underlying_bg, blend);
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಹಿಂತಿರುಗಿ get_attr_ನಮೂದು(&cterm_attr_ಕೋಷ್ಟಕ, &new_en);
    }

    ಆದರೆ (char_attr > HL_ALL)
	char_aep = syn_ಪದ_attr2entry(char_attr);
    ಆದರೆ (char_aep != ಖಾಲಿ)
	new_en = *char_aep;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	CLEAR_ಕ್ಷೇತ್ರ(new_en);
	ಆದರೆ (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    // For term mode, no separate background color handling.
    ಹಿಂತಿರುಗಿ get_attr_ನಮೂದು(&ಪದ_attr_ಕೋಷ್ಟಕ, &new_en);
}

/*
 * Blend attributes for popup windows with opacity.
 */
    ಪೂರ್ಣಾಂಕ
hl_blend_attr(ಪೂರ್ಣಾಂಕ char_attr, ಪೂರ್ಣಾಂಕ popup_attr, ಪೂರ್ಣಾಂಕ blend, ಪೂರ್ಣಾಂಕ blend_fg)
{
    ಆದರೆ (blend >= 100 && blend_fg)
	ಹಿಂತಿರುಗಿ char_attr;

    ಹಿಂತಿರುಗಿ hl_blend_attr_common(char_attr, popup_attr, blend, blend_fg);
}

/*
 * Blend underlying foreground and background colors for pum space cells.
 */
    ಪೂರ್ಣಾಂಕ
hl_pum_blend_attr(ಪೂರ್ಣಾಂಕ char_attr, ಪೂರ್ಣಾಂಕ popup_attr, ಪೂರ್ಣಾಂಕ blend)
{
    ಹಿಂತಿರುಗಿ hl_blend_attr_common(char_attr, popup_attr, blend, TRUE);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    attrentry_T *
syn_gui_attr2entry(ಪೂರ್ಣಾಂಕ attr)
{
    attr -= ATTR_OFF;
    ಆದರೆ (attr >= gui_attr_ಕೋಷ್ಟಕ.ga_len)	    // did ":syntax clear"
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ &(GUI_ATTR_ನಮೂದು(attr));
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get the highlight attributes (HL_BOLD etc.) from an attribute nr.
 * Only to be used when "attr" > HL_ALL.
 */
    ಪೂರ್ಣಾಂಕ
syn_attr2attr(ಪೂರ್ಣಾಂಕ attr)
{
    attrentry_T	*aep;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	aep = syn_gui_attr2entry(attr);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	ಇಲ್ಲದಿದ್ದರೆ
	    aep = syn_ಪದ_attr2entry(attr);

    ಆದರೆ (aep == ಖಾಲಿ)	    // highlighting not set
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ aep->ae_attr;
}


    attrentry_T *
syn_ಪದ_attr2entry(ಪೂರ್ಣಾಂಕ attr)
{
    attr -= ATTR_OFF;
    ಆದರೆ (attr >= ಪದ_attr_ಕೋಷ್ಟಕ.ga_len)	    // did ":syntax clear"
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ &(ಪದ_ATTR_ನಮೂದು(attr));
}

    attrentry_T *
syn_cterm_attr2entry(ಪೂರ್ಣಾಂಕ attr)
{
    attr -= ATTR_OFF;
    ಆದರೆ (attr >= cterm_attr_ಕೋಷ್ಟಕ.ga_len)	// did ":syntax clear"
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ &(CTERM_ATTR_ನಮೂದು(attr));
}

#ವ್ಯಾಖ್ಯಾನಿಸು LIST_ATTR   1
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_STRING 2
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_INT    3

    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_list_one(ಪೂರ್ಣಾಂಕ id)
{
    hl_ಗುಂಪು_T	    *sgp;
    ಪೂರ್ಣಾಂಕ		    didh = FALSE;

    sgp = &HL_ಕೋಷ್ಟಕ()[id - 1];	    // index is ID minus one

    ಆದರೆ (ಸಂದೇಶ_filtered(sgp->sg_name))
	ಹಿಂತಿರುಗಿ;

    // Note: Keep this in sync with expand_highlight_group().
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_ಪದ, ಖಾಲಿ, "term");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_ಆರಂಭ, "start");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_stop, "stop");

    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_cterm, ಖಾಲಿ, "cterm");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_fg, ಖಾಲಿ, "ctermfg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_bg, ಖಾಲಿ, "ctermbg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_ul, ಖಾಲಿ, "ctermul");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_font, ಖಾಲಿ, "ctermfont");

#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_EVAL)
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_gui, ಖಾಲಿ, "gui");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_fg_name, "guifg");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_bg_name, "guibg");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_sp_name, "guisp");
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_font_name, "font");
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (sgp->sg_link && !got_int)
    {
	(ಶೂನ್ಯ)syn_list_ಶೀರ್ಷಿಕೆ(didh, 9999, id);
	didh = TRUE;
	ಸಂದೇಶ_puts_attr("links to", HL_ATTR(HLF_D));
	ಸಂದೇಶ_putchar(' ');
	ಸಂದೇಶ_outtrans(HL_ಕೋಷ್ಟಕ()[HL_ಕೋಷ್ಟಕ()[id - 1].sg_link - 1].sg_name);
    }

    ಆದರೆ (!didh)
	highlight_list_arg(id, didh, LIST_STRING, 0, (char_u *)"cleared", "");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (p_verbose > 0)
	ಕೊನೆಯ_ಗಣ_ಸಂದೇಶ(sgp->sg_script_ctx);
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ char_u*
highlight_arg_ಗಮ್ಯ_string(
    ಪೂರ್ಣಾಂಕ		type,
    ಪೂರ್ಣಾಂಕ		iarg,
    char_u	*sarg,
    char_u	*buf)
{
    ಆದರೆ (type == LIST_INT)
	sprintf((ಅಕ್ಷರ *)buf, "%d", iarg - 1);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == LIST_STRING)
	ಹಿಂತಿರುಗಿ sarg;
    ಇಲ್ಲದಿದ್ದರೆ // type == LIST_ATTR
    {
	size_t buflen;

	buf[0] = NUL;
	buflen = 0;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(highlight_ಸೂಚ್ಯಂಕ_tab); ++i)
	{
	    ಆದರೆ (iarg & highlight_ಸೂಚ್ಯಂಕ_tab[i]->ಕೀಲಿ)
	    {
		ಆದರೆ (buflen > 0)
		{
		    STRCPY(buf + buflen, (char_u *)",");
		    ++buflen;
		}
		STRCPY(buf + buflen, highlight_ಸೂಚ್ಯಂಕ_tab[i]->ಮೌಲ್ಯ.string);
		buflen += highlight_ಸೂಚ್ಯಂಕ_tab[i]->ಮೌಲ್ಯ.ಉದ್ದಳತೆ;
		iarg &= ~highlight_ಸೂಚ್ಯಂಕ_tab[i]->ಕೀಲಿ;	    // don't want "inverse"/"reverse"
	    }
	}
    }
    ಹಿಂತಿರುಗಿ buf;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
highlight_list_arg(
    ಪೂರ್ಣಾಂಕ		id,
    ಪೂರ್ಣಾಂಕ		didh,
    ಪೂರ್ಣಾಂಕ		type,
    ಪೂರ್ಣಾಂಕ		iarg,
    char_u	*sarg,
    ಅಕ್ಷರ	*name)
{
    char_u	buf[MAX_ATTR_LEN];
    char_u	*ts;

    ಆದರೆ (got_int)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (type == LIST_STRING ? (sarg == ಖಾಲಿ) : (iarg == 0))
	ಹಿಂತಿರುಗಿ didh;

    ts = highlight_arg_ಗಮ್ಯ_string(type, iarg, sarg, buf);

    (ಶೂನ್ಯ)syn_list_ಶೀರ್ಷಿಕೆ(didh,
	    (ಪೂರ್ಣಾಂಕ)(vim_strsize(ts) + STRLEN(name) + 1), id);
    didh = TRUE;
    ಆದರೆ (!got_int)
    {
	ಆದರೆ (*name != NUL)
	{
	    ಸಂದೇಶ_puts_attr(name, HL_ATTR(HLF_D));
	    ಸಂದೇಶ_puts_attr("=", HL_ATTR(HLF_D));
	}
	ಸಂದೇಶ_outtrans(ts);
    }
    ಹಿಂತಿರುಗಿ didh;
}

#ಆದರೆ ((defined(FEAT_EVAL) || defined(FEAT_PRINTER))) && defined(FEAT_SYN_HL)
/*
 * Return "1" if highlight group "id" has attribute "flag".
 * Return NULL otherwise.
 */
    char_u *
highlight_has_attr(
    ಪೂರ್ಣಾಂಕ		id,
    ಪೂರ್ಣಾಂಕ		flag,
    ಪೂರ್ಣಾಂಕ		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    ಪೂರ್ಣಾಂಕ		attr;

    ಆದರೆ (id <= 0 || id > highlight_ga.ga_len)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_EVAL)
    ಆದರೆ (modec == 'g')
	attr = HL_ಕೋಷ್ಟಕ()[id - 1].sg_gui;
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
	ಆದರೆ (modec == 'c')
	    attr = HL_ಕೋಷ್ಟಕ()[id - 1].sg_cterm;
	ಇಲ್ಲದಿದ್ದರೆ
	    attr = HL_ಕೋಷ್ಟಕ()[id - 1].sg_ಪದ;
    }

    ಆದರೆ (attr & flag)
	ಹಿಂತಿರುಗಿ (char_u *)"1";
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_SYN_HL) && defined(FEAT_EVAL)
/*
 * Return color name of highlight group "id".
 */
    char_u *
highlight_color(
    ಪೂರ್ಣಾಂಕ		id,
    char_u	*what,	// "font", "fg", "bg", "sp", "ul", "fg#", "bg#" or "sp#"
    ಪೂರ್ಣಾಂಕ		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    ಸ್ಥಿರ char_u	name[20];
    ಪೂರ್ಣಾಂಕ			n;
    ಪೂರ್ಣಾಂಕ			fg = FALSE;
    ಪೂರ್ಣಾಂಕ			sp = FALSE;
    ಪೂರ್ಣಾಂಕ			ul = FALSE;
    ಪೂರ್ಣಾಂಕ			font = FALSE;

    ಆದರೆ (id <= 0 || id > highlight_ga.ga_len)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'g')
	fg = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'o'
	     && TOLOWER_ASC(what[2]) == 'n' && TOLOWER_ASC(what[3]) == 't')
	font = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (TOLOWER_ASC(what[0]) == 's' && TOLOWER_ASC(what[1]) == 'p')
	sp = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (TOLOWER_ASC(what[0]) == 'u' && TOLOWER_ASC(what[1]) == 'l')
	ul = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(TOLOWER_ASC(what[0]) == 'b' && TOLOWER_ASC(what[1]) == 'g'))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (modec == 'g')
    {
# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI
	// return font name
	ಆದರೆ (font)
	    ಹಿಂತಿರುಗಿ HL_ಕೋಷ್ಟಕ()[id - 1].sg_font_name;
#  endif

	// return #RRGGBB form (only possible when GUI is running)
	ಆದರೆ ((USE_24_ಬಿಟ್) && what[2] == '#')
	{
	    guicolor_T		color;
	    long_u		rgb;
	    ಸ್ಥಿರ char_u	buf[10];

	    ಆದರೆ (fg)
		color = HL_ಕೋಷ್ಟಕ()[id - 1].sg_gui_fg;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sp)
		color = HL_ಕೋಷ್ಟಕ()[id - 1].sg_gui_sp;
	    ಇಲ್ಲದಿದ್ದರೆ
		color = HL_ಕೋಷ್ಟಕ()[id - 1].sg_gui_bg;
	    ಆದರೆ (color == INVALCOLOR)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    rgb = (long_u)GUI_MCH_GET_RGB(color);
	    sprintf((ಅಕ್ಷರ *)buf, "#%02x%02x%02x",
				      (ಚಿಹ್ನೆರಹಿತ)(rgb >> 16),
				      (ಚಿಹ್ನೆರಹಿತ)(rgb >> 8) & 255,
				      (ಚಿಹ್ನೆರಹಿತ)rgb & 255);
	    ಹಿಂತಿರುಗಿ buf;
	}
# endif
	ಆದರೆ (fg)
	    ಹಿಂತಿರುಗಿ (HL_ಕೋಷ್ಟಕ()[id - 1].sg_gui_fg_name);
	ಆದರೆ (sp)
	    ಹಿಂತಿರುಗಿ (HL_ಕೋಷ್ಟಕ()[id - 1].sg_gui_sp_name);
	ಹಿಂತಿರುಗಿ (HL_ಕೋಷ್ಟಕ()[id - 1].sg_gui_bg_name);
    }
    ಆದರೆ (sp)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (modec == 'c')
    {
	ಆದರೆ (fg)
	    n = HL_ಕೋಷ್ಟಕ()[id - 1].sg_cterm_fg - 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ul)
	    n = HL_ಕೋಷ್ಟಕ()[id - 1].sg_cterm_ul - 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (font)
	    n = HL_ಕೋಷ್ಟಕ()[id - 1].sg_cterm_font - 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    n = HL_ಕೋಷ್ಟಕ()[id - 1].sg_cterm_bg - 1;
	ಆದರೆ (n < 0)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	sprintf((ಅಕ್ಷರ *)name, "%d", n);
	ಹಿಂತಿರುಗಿ name;
    }
    // term doesn't have color
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_SYN_HL) \
	    && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)) \
	&& defined(FEAT_PRINTER)
/*
 * Return color name of highlight group "id" as RGB value.
 */
    long_u
highlight_gui_color_rgb(
    ಪೂರ್ಣಾಂಕ		id,
    ಪೂರ್ಣಾಂಕ		fg)	// TRUE = fg, FALSE = bg
{
    guicolor_T	color;

    ಆದರೆ (id <= 0 || id > highlight_ga.ga_len)
	ಹಿಂತಿರುಗಿ 0L;

    ಆದರೆ (fg)
	color = HL_ಕೋಷ್ಟಕ()[id - 1].sg_gui_fg;
    ಇಲ್ಲದಿದ್ದರೆ
	color = HL_ಕೋಷ್ಟಕ()[id - 1].sg_gui_bg;

    ಆದರೆ (color == INVALCOLOR)
	ಹಿಂತಿರುಗಿ 0L;

    ಹಿಂತಿರುಗಿ GUI_MCH_GET_RGB(color);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Output the syntax list header.
 * Return TRUE when started a new line.
 */
    ಪೂರ್ಣಾಂಕ
syn_list_ಶೀರ್ಷಿಕೆ(
    ಪೂರ್ಣಾಂಕ	    did_ಶೀರ್ಷಿಕೆ,		// did header already
    ಪೂರ್ಣಾಂಕ	    outlen,		// length of string that comes
    ಪೂರ್ಣಾಂಕ	    id)			// highlight group id
{
    ಪೂರ್ಣಾಂಕ	    endcol = 19;
    ಪೂರ್ಣಾಂಕ	    newline = TRUE;
    ಪೂರ್ಣಾಂಕ	    name_ಲಂಬಸಾಲು = 0;

    ಆದರೆ (!did_ಶೀರ್ಷಿಕೆ)
    {
	ಸಂದೇಶ_putchar('\n');
	ಆದರೆ (got_int)
	    ಹಿಂತಿರುಗಿ TRUE;
	ಸಂದೇಶ_outtrans(HL_ಕೋಷ್ಟಕ()[id - 1].sg_name);
	name_ಲಂಬಸಾಲು = ಸಂದೇಶ_ಲಂಬಸಾಲು;
	endcol = 15;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು + outlen + 1 >= cmdline_ಅಗಲ)
    {
	ಸಂದೇಶ_putchar('\n');
	ಆದರೆ (got_int)
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು >= endcol)	// wrap around is like starting a new line
	    newline = FALSE;
    }

    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು >= endcol)	// output at least one space
	endcol = ಸಂದೇಶ_ಲಂಬಸಾಲು + 1;
    ಆದರೆ (cmdline_ಅಗಲ <= endcol)	// avoid hang for tiny window
	endcol = cmdline_ಅಗಲ - 1;

    ಸಂದೇಶ_advance(endcol);

    // Show "xxx" with the attributes.
    ಆದರೆ (!did_ಶೀರ್ಷಿಕೆ)
    {
	ಆದರೆ (endcol == cmdline_ಅಗಲ - 1 && endcol <= name_ಲಂಬಸಾಲು)
	    ಸಂದೇಶ_putchar(' ');
	ಸಂದೇಶ_puts_attr("xxx", syn_id2attr(id));
	ಸಂದೇಶ_putchar(' ');
    }

    ಹಿಂತಿರುಗಿ newline;
}

/*
 * Set the attribute numbers for a highlight group.
 * Called after one of the attributes has changed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_hl_attr(
    ಪೂರ್ಣಾಂಕ		idx)	    // index in array
{
    attrentry_T	    at_en;
    hl_ಗುಂಪು_T	    *sgp = HL_ಕೋಷ್ಟಕ() + idx;

    // The "Normal" group doesn't need an attribute number
    ಆದರೆ (sgp->sg_name_u != ಖಾಲಿ && STRCMP(sgp->sg_name_u, "NORMAL") == 0)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // For the GUI mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    ಆದರೆ (sgp->sg_gui_fg == INVALCOLOR
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
    ಇಲ್ಲದಿದ್ದರೆ
    {
	at_en.ae_attr = sgp->sg_gui;
	at_en.ae_u.gui.fg_color = sgp->sg_gui_fg;
	at_en.ae_u.gui.bg_color = sgp->sg_gui_bg;
	at_en.ae_u.gui.sp_color = sgp->sg_gui_sp;
	at_en.ae_u.gui.font = sgp->sg_font;
# ifdef FEAT_XFONTSET
	at_en.ae_u.gui.fontset = sgp->sg_fontset;
# endif
	sgp->sg_gui_attr = get_attr_ನಮೂದು(&gui_attr_ಕೋಷ್ಟಕ, &at_en);
    }
#ಆದರೆ_ಅಂತ್ಯ
    // For the term mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    ಆದರೆ (sgp->sg_ಆರಂಭ == ಖಾಲಿ && sgp->sg_stop == ಖಾಲಿ)
	sgp->sg_ಪದ_attr = sgp->sg_ಪದ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	at_en.ae_attr = sgp->sg_ಪದ;
	at_en.ae_u.ಪದ.ಆರಂಭ = sgp->sg_ಆರಂಭ;
	at_en.ae_u.ಪದ.stop = sgp->sg_stop;
	sgp->sg_ಪದ_attr = get_attr_ನಮೂದು(&ಪದ_attr_ಕೋಷ್ಟಕ, &at_en);
    }

    // For the color term mode: If there are other than "normal"
    // highlighting attributes, need to allocate an attr number.
    ಆದರೆ (sgp->sg_cterm_fg == 0 && sgp->sg_cterm_bg == 0 &&
	sgp->sg_cterm_ul == 0 && sgp->sg_cterm_font == 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
	    && sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
#ಆದರೆ_ಅಂತ್ಯ
	    )
	sgp->sg_cterm_attr = sgp->sg_cterm;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	at_en.ae_attr = sgp->sg_cterm;
	at_en.ae_u.cterm.fg_color = sgp->sg_cterm_fg;
	at_en.ae_u.cterm.bg_color = sgp->sg_cterm_bg;
	at_en.ae_u.cterm.ul_color = sgp->sg_cterm_ul;
	at_en.ae_u.cterm.font = sgp->sg_cterm_font;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
	at_en.ae_u.cterm.fg_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_fg);
	at_en.ae_u.cterm.bg_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_bg);
	// Only use the underline/undercurl color when used, it may clear the
	// background color if not supported.
	ಆದರೆ (sgp->sg_cterm & (HL_UNDERLINE | HL_UNDERCURL
			   | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED))
	    at_en.ae_u.cterm.ul_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_sp);
	ಇಲ್ಲದಿದ್ದರೆ
	    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	ಆದರೆ (at_en.ae_u.cterm.fg_rgb == INVALCOLOR
		&& at_en.ae_u.cterm.bg_rgb == INVALCOLOR)
	{
	    // If both fg and bg are invalid fall back to the cterm colors.
	    // Helps when the GUI only uses an attribute, e.g. undercurl.
	    at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	    at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
	}
#ಆದರೆ_ಅಂತ್ಯ
	sgp->sg_cterm_attr = get_attr_ನಮೂದು(&cterm_attr_ಕೋಷ್ಟಕ, &at_en);
    }
}

/*
 * Check if highlight id is overridden, and return the overriding highlight id.
 * Otherwise return the original id if no override.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_override(ಪೂರ್ಣಾಂಕ id)
{
    ಆದರೆ (overrides != ಖಾಲಿ && overrides->arr != ಖಾಲಿ)
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < overrides->len; k++)
	    ಆದರೆ (overrides->arr[k].from == id)
		ಹಿಂತಿರುಗಿ overrides->arr[k].ಗಮ್ಯ;
    ಹಿಂತಿರುಗಿ id;
}

/*
 * Lookup a highlight group name by pointer and length.
 * If it is not found, 0 is returned.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_name2id_len(char_u *name, ಪೂರ್ಣಾಂಕ len)
{
    char_u	name_u[MAX_SYN_NAME + 1];
    hashitem_T	*hi;

    ಆದರೆ (len > MAX_SYN_NAME)
	len = MAX_SYN_NAME;
    mch_memmove(name_u, name, len);
    name_u[len] = NUL;
    vim_strup(name_u);
    ಆದರೆ (!highlight_ht_inited)
	ಹಿಂತಿರುಗಿ 0;
    hi = hash_ಕಂಡುಹಿಡಿ(&highlight_ht, name_u);
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ HI2HLNAME(hi)->hn_id;
}

/*
 * Lookup a highlight group name and return its ID.
 * If it is not found, 0 is returned.
 */
    ಪೂರ್ಣಾಂಕ
syn_name2id(char_u *name)
{
    // Avoid using stricmp() too much, it's slow on some systems.
    ಹಿಂತಿರುಗಿ syn_name2id_len(name, (ಪೂರ್ಣಾಂಕ)STRLEN(name));
}

/*
 * Lookup a highlight group name and return its attributes.
 * Return zero if not found.
 */
    ಪೂರ್ಣಾಂಕ
syn_name2attr(char_u *name)
{
    ಪೂರ್ಣಾಂಕ id = syn_name2id(name);

    ಆದರೆ (id != 0)
	ಹಿಂತಿರುಗಿ syn_id2attr(id);
    ಹಿಂತಿರುಗಿ 0;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return TRUE if highlight group "name" exists.
 */
    ಪೂರ್ಣಾಂಕ
highlight_exists(char_u *name)
{
    ಹಿಂತಿರುಗಿ (syn_name2id(name) > 0);
}

# ಆದರೆ defined(FEAT_ಹುಡುಕು_EXTRA)
/*
 * Return the name of highlight group "id".
 * When not a valid ID return an empty string.
 */
    char_u *
syn_id2name(ಪೂರ್ಣಾಂಕ id)
{
    ಆದರೆ (id <= 0 || id > highlight_ga.ga_len)
	ಹಿಂತಿರುಗಿ (char_u *)"";
    ಹಿಂತಿರುಗಿ HL_ಕೋಷ್ಟಕ()[id - 1].sg_name;
}
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Like syn_name2id(), but take a pointer + length argument.
 */
    ಪೂರ್ಣಾಂಕ
syn_namen2id(char_u *linep, ಪೂರ್ಣಾಂಕ len)
{
    ಹಿಂತಿರುಗಿ syn_name2id_len(linep, len);
}

/*
 * Find highlight group name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    ಪೂರ್ಣಾಂಕ
syn_check_ಗುಂಪು(char_u *pp, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ	    id;
    char_u  *name;

    ಆದರೆ (len > MAX_SYN_NAME)
    {
	emsg(_(e_highlight_ಗುಂಪು_name_too_long));
	ಹಿಂತಿರುಗಿ 0;
    }
    id = syn_name2id_len(pp, len);
    ಆದರೆ (id == 0)			// doesn't exist yet
    {
	name = vim_strnsave(pp, len);
	ಆದರೆ (name == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ 0;
	id = syn_ಕೂಡಿಸು_ಗುಂಪು(name);
    }
    ಹಿಂತಿರುಗಿ id;
}

/*
 * Add new highlight group and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_ಕೂಡಿಸು_ಗುಂಪು(char_u *name)
{
    char_u	*p;
    char_u	*name_ಮೇಲೆ;

    // Check that the name is valid (ASCII letters, digits, underscores, dots, or hyphens).
    ಗಾಗಿ (p = name; *p != NUL; ++p)
    {
	ಆದರೆ (!vim_isprintc(*p))
	{
	    emsg(_(e_unprintable_character_ಒಳ_ಗುಂಪು_name));
	    vim_free(name);
	    ಹಿಂತಿರುಗಿ 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ASCII_ISALNUM(*p) && *p != '_' && *p != '.' && *p != '-')
	{
	    // This is an error, but since there previously was no check only
	    // give a warning.
	    ಸಂದೇಶ_ಮೂಲ(HL_ATTR(HLF_W));
	    ಸಂದೇಶ(_("W18: Invalid character in group name"));
	    ನಿಲ್ಲಿಸು;
	}
    }

    // First call for this growarray: init growing array.
    ಆದರೆ (highlight_ga.ga_ದತ್ತಾಂಶ == ಖಾಲಿ)
    {
	highlight_ga.ga_itemsize = ಗಾತ್ರ(hl_ಗುಂಪು_T);
	highlight_ga.ga_growsize = 10;
	hash_init(&highlight_ht);
	highlight_ht_inited = true;
    }

    ಆದರೆ (highlight_ga.ga_len >= MAX_HL_ID)
    {
	emsg(_(e_too_many_highlight_and_syntax_groups));
	vim_free(name);
	ಹಿಂತಿರುಗಿ 0;
    }

    // Make room for at least one other syntax_highlight entry.
    ಆದರೆ (ga_grow(&highlight_ga, 1) == FAIL)
    {
	vim_free(name);
	ಹಿಂತಿರುಗಿ 0;
    }

    {
	hlname_T    *hn;
	ಪೂರ್ಣಾಂಕ	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(name);

	hn = alloc(offsetof(hlname_T, hn_ಕೀಲಿ) + len + 1);
	ಆದರೆ (hn == ಖಾಲಿ)
	{
	    vim_free(name);
	    ಹಿಂತಿರುಗಿ 0;
	}
	vim_strncpy(hn->hn_ಕೀಲಿ, name, len);
	vim_strup(hn->hn_ಕೀಲಿ);
	hn->hn_id = highlight_ga.ga_len + 1;	// ID is index plus one
	name_ಮೇಲೆ = hn->hn_ಕೀಲಿ;
    }

    CLEAR_ಸೂಚಕ(&(HL_ಕೋಷ್ಟಕ()[highlight_ga.ga_len]));
    HL_ಕೋಷ್ಟಕ()[highlight_ga.ga_len].sg_name = name;
    HL_ಕೋಷ್ಟಕ()[highlight_ga.ga_len].sg_name_u = name_ಮೇಲೆ;
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_ಕೋಷ್ಟಕ()[highlight_ga.ga_len].sg_gui_bg = INVALCOLOR;
    HL_ಕೋಷ್ಟಕ()[highlight_ga.ga_len].sg_gui_fg = INVALCOLOR;
    HL_ಕೋಷ್ಟಕ()[highlight_ga.ga_len].sg_gui_sp = INVALCOLOR;
#ಆದರೆ_ಅಂತ್ಯ
    hash_ಕೂಡಿಸು(&highlight_ht, name_ಮೇಲೆ, "highlight");
    ++highlight_ga.ga_len;

    ಹಿಂತಿರುಗಿ highlight_ga.ga_len;		    // ID is index plus one
}

/*
 * When, just after calling syn_add_group(), an error is discovered, this
 * function deletes the new name.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_unadd_ಗುಂಪು(ಶೂನ್ಯ)
{
    hashitem_T *hi;

    --highlight_ga.ga_len;
    hi = hash_ಕಂಡುಹಿಡಿ(&highlight_ht, HL_ಕೋಷ್ಟಕ()[highlight_ga.ga_len].sg_name_u);
    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	hash_remove(&highlight_ht, hi, "highlight");
    vim_free(HL_ಕೋಷ್ಟಕ()[highlight_ga.ga_len].sg_name);
    vim_free(HL_ಕೋಷ್ಟಕ()[highlight_ga.ga_len].sg_name_u - HLNAME_ಕೀಲಿ_OFF);
}

/*
 * Translate a group ID to highlight attributes.
 * "hl_id" must be valid: > 0, caller must check.
 */
    ಪೂರ್ಣಾಂಕ
syn_id2attr(ಪೂರ್ಣಾಂಕ hl_id)
{
    ಪೂರ್ಣಾಂಕ		attr;
    hl_ಗುಂಪು_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ಪ್ರತಿಜ್ಞಾ(hl_id > 0);
    sgp = &HL_ಕೋಷ್ಟಕ()[hl_id - 1];	    // index is ID minus one

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Only use GUI attr when the GUI is being used.
    ಆದರೆ (gui.ಒಳ_use)
	attr = sgp->sg_gui_attr;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (IS_CTERM)
	    attr = sgp->sg_cterm_attr;
	ಇಲ್ಲದಿದ್ದರೆ
	    attr = sgp->sg_ಪದ_attr;

    ಹಿಂತಿರುಗಿ attr;
}

#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Get the GUI colors and attributes for a group ID.
 * NOTE: the colors will be INVALCOLOR when not set, the color otherwise.
 */
    ಪೂರ್ಣಾಂಕ
syn_id2colors(ಪೂರ್ಣಾಂಕ hl_id, guicolor_T *fgp, guicolor_T *bgp)
{
    hl_ಗುಂಪು_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ಪ್ರತಿಜ್ಞಾ(hl_id > 0);
    sgp = &HL_ಕೋಷ್ಟಕ()[hl_id - 1];	    // index is ID minus one

    *fgp = sgp->sg_gui_fg;
    *bgp = sgp->sg_gui_bg;
    ಹಿಂತಿರುಗಿ sgp->sg_gui;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined(MSWIN) \
	    && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL)) \
	    && defined(FEAT_TERMGUICOLORS)) \
	|| defined(FEAT_TERMINAL)
    ಶೂನ್ಯ
syn_id2cterm_bg(ಪೂರ್ಣಾಂಕ hl_id, ಪೂರ್ಣಾಂಕ *fgp, ಪೂರ್ಣಾಂಕ *bgp)
{
    hl_ಗುಂಪು_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ಪ್ರತಿಜ್ಞಾ(hl_id > 0);
    sgp = &HL_ಕೋಷ್ಟಕ()[hl_id - 1];	    // index is ID minus one
    *fgp = sgp->sg_cterm_fg - 1;
    *bgp = sgp->sg_cterm_bg - 1;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Translate a group ID to the final group ID (following links).
 */
    ಪೂರ್ಣಾಂಕ
syn_get_final_id(ಪೂರ್ಣಾಂಕ hl_id)
{
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    hl_ಗುಂಪು_T	*sgp;

    ಆದರೆ (hl_id > highlight_ga.ga_len || hl_id < 1)
	ಹಿಂತಿರುಗಿ 0;			// Can be called from eval!!

    // Follow links until there is no more.
    // Look out for loops!  Break after 100 links.
    ಗಾಗಿ (ಎಣಿಕೆ_2 = 100; --ಎಣಿಕೆ_2 >= 0; )
    {
	ಪೂರ್ಣಾಂಕ tmp = hl_id;

	sgp = &HL_ಕೋಷ್ಟಕ()[hl_id - 1];	    // index is ID minus one
	ಆದರೆ (sgp->sg_link == 0 || sgp->sg_link > highlight_ga.ga_len)
	    ನಿಲ್ಲಿಸು;

	// This is to handle highlight groups that are overridden but are in the
	// middle of a link chain.
	hl_id = syn_override(hl_id);
	ಆದರೆ (tmp != hl_id)
	    ಮುಂದುವರಿ;
	hl_id = sgp->sg_link;
    }
    ಹಿಂತಿರುಗಿ syn_override(hl_id);
}

#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Call this function just after the GUI has started.
 * Also called when 'termguicolors' was set, gui.in_use will be FALSE then.
 * It finds the font and color handles for the highlighting groups.
 */
    ಶೂನ್ಯ
highlight_gui_started(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    idx;

    // First get the colors from the "Normal" and "Menu" group, if set
    ಆದರೆ (USE_24_ಬಿಟ್)
	ಗಣ_ಸಾಮಾನ್ಯ_colors();

    ಗಾಗಿ (idx = 0; idx < highlight_ga.ga_len; ++idx)
	gui_do_one_color(idx, FALSE, FALSE);

    highlight_ಬದಲಾಯಿತು();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_do_one_color(
    ಪೂರ್ಣಾಂಕ		idx,
    ಪೂರ್ಣಾಂಕ		do_menu ಬಳಸದ,	   // TRUE: might set the menu font
    ಪೂರ್ಣಾಂಕ		do_tooltip ಬಳಸದ) // TRUE: might set the tooltip font
{
    ಪೂರ್ಣಾಂಕ		didit = FALSE;

# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    ಆದರೆ (gui.ಒಳ_use)
#  endif
	ಆದರೆ (HL_ಕೋಷ್ಟಕ()[idx].sg_font_name != ಖಾಲಿ)
	{
	    hl_do_font(idx, HL_ಕೋಷ್ಟಕ()[idx].sg_font_name, FALSE, do_menu,
							    do_tooltip, TRUE);
	    didit = TRUE;
	}
# endif
    ಆದರೆ (HL_ಕೋಷ್ಟಕ()[idx].sg_gui_fg_name != ಖಾಲಿ)
    {
	HL_ಕೋಷ್ಟಕ()[idx].sg_gui_fg =
			    color_name2handle(HL_ಕೋಷ್ಟಕ()[idx].sg_gui_fg_name);
	didit = TRUE;
    }
    ಆದರೆ (HL_ಕೋಷ್ಟಕ()[idx].sg_gui_bg_name != ಖಾಲಿ)
    {
	HL_ಕೋಷ್ಟಕ()[idx].sg_gui_bg =
			    color_name2handle(HL_ಕೋಷ್ಟಕ()[idx].sg_gui_bg_name);
	didit = TRUE;
    }
    ಆದರೆ (HL_ಕೋಷ್ಟಕ()[idx].sg_gui_sp_name != ಖಾಲಿ)
    {
	HL_ಕೋಷ್ಟಕ()[idx].sg_gui_sp =
			    color_name2handle(HL_ಕೋಷ್ಟಕ()[idx].sg_gui_sp_name);
	didit = TRUE;
    }
    ಆದರೆ (didit)	// need to get a new attr number
	ಗಣ_hl_attr(idx);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USER_HIGHLIGHT) && defined(FEAT_STL_OPT)
/*
 * Apply difference between User[1-9] and HLF_S to HLF_SNC, HLF_ST or HLF_STNC.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
combine_stl_hlt(
	ಪೂರ್ಣಾಂಕ id,
	ಪೂರ್ಣಾಂಕ id_S,
	ಪೂರ್ಣಾಂಕ id_alt,
	ಪೂರ್ಣಾಂಕ hlcnt,
	ಪೂರ್ಣಾಂಕ i,
	ಪೂರ್ಣಾಂಕ hlf,
	ಪೂರ್ಣಾಂಕ *ಕೋಷ್ಟಕ)
{
    hl_ಗುಂಪು_T *hlt = HL_ಕೋಷ್ಟಕ();

    ಆದರೆ (id_alt == 0)
    {
	CLEAR_ಸೂಚಕ(&hlt[hlcnt + i]);
	hlt[hlcnt + i].sg_ಪದ = highlight_attr[hlf];
	hlt[hlcnt + i].sg_cterm = highlight_attr[hlf];
# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_EVAL)
	hlt[hlcnt + i].sg_gui = highlight_attr[hlf];
# endif
    }
    ಇಲ್ಲದಿದ್ದರೆ
	mch_memmove(&hlt[hlcnt + i],
		    &hlt[id_alt - 1],
		    ಗಾತ್ರ(hl_ಗುಂಪು_T));
    hlt[hlcnt + i].sg_link = 0;

    hlt[hlcnt + i].sg_ಪದ ^=
	hlt[id - 1].sg_ಪದ ^ hlt[id_S - 1].sg_ಪದ;
    ಆದರೆ (hlt[id - 1].sg_ಆರಂಭ != hlt[id_S - 1].sg_ಆರಂಭ)
	hlt[hlcnt + i].sg_ಆರಂಭ = hlt[id - 1].sg_ಆರಂಭ;
    ಆದರೆ (hlt[id - 1].sg_stop != hlt[id_S - 1].sg_stop)
	hlt[hlcnt + i].sg_stop = hlt[id - 1].sg_stop;
    hlt[hlcnt + i].sg_cterm ^=
	hlt[id - 1].sg_cterm ^ hlt[id_S - 1].sg_cterm;
    ಆದರೆ (hlt[id - 1].sg_cterm_fg != hlt[id_S - 1].sg_cterm_fg)
	hlt[hlcnt + i].sg_cterm_fg = hlt[id - 1].sg_cterm_fg;
    ಆದರೆ (hlt[id - 1].sg_cterm_bg != hlt[id_S - 1].sg_cterm_bg)
	hlt[hlcnt + i].sg_cterm_bg = hlt[id - 1].sg_cterm_bg;
    ಆದರೆ (hlt[id - 1].sg_cterm_font != hlt[id_S - 1].sg_cterm_font)
	hlt[hlcnt + i].sg_cterm_font = hlt[id - 1].sg_cterm_font;
# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_EVAL)
    hlt[hlcnt + i].sg_gui ^=
	hlt[id - 1].sg_gui ^ hlt[id_S - 1].sg_gui;
# endif
# ifdef FEAT_GUI
    ಆದರೆ (hlt[id - 1].sg_gui_fg != hlt[id_S - 1].sg_gui_fg)
	hlt[hlcnt + i].sg_gui_fg = hlt[id - 1].sg_gui_fg;
    ಆದರೆ (hlt[id - 1].sg_gui_bg != hlt[id_S - 1].sg_gui_bg)
	hlt[hlcnt + i].sg_gui_bg = hlt[id - 1].sg_gui_bg;
    ಆದರೆ (hlt[id - 1].sg_gui_sp != hlt[id_S - 1].sg_gui_sp)
	hlt[hlcnt + i].sg_gui_sp = hlt[id - 1].sg_gui_sp;
    ಆದರೆ (hlt[id - 1].sg_font != hlt[id_S - 1].sg_font)
	hlt[hlcnt + i].sg_font = hlt[id - 1].sg_font;
#  ifdef FEAT_XFONTSET
    ಆದರೆ (hlt[id - 1].sg_fontset != hlt[id_S - 1].sg_fontset)
	hlt[hlcnt + i].sg_fontset = hlt[id - 1].sg_fontset;
#  endif
# endif
    highlight_ga.ga_len = hlcnt + i + 1;
    ಗಣ_hl_attr(hlcnt + i);	// At long last we can apply
    ಕೋಷ್ಟಕ[i] = syn_id2attr(hlcnt + i + 1);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Translate the 'highlight' option into attributes in highlight_attr[] and
 * set up the user highlights User1..9.  If FEAT_STL_OPT is in use, a set of
 * corresponding highlights to use on top of HLF_SNC is computed.
 * Called only when the 'highlight' option has been changed and upon first
 * screen redraw after any :highlight command.
 * Return FAIL when an invalid flag is found in 'highlight'.  OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
highlight_ಬದಲಾಯಿತು(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		hlf;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*p;
    char_u	*default_hl;
    ಪೂರ್ಣಾಂಕ		attr;
    char_u	*ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		id;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USER_HIGHLIGHT
    char_u      userhl[30];  // use 30 to avoid compiler warning
# ifdef FEAT_STL_OPT
    ಪೂರ್ಣಾಂಕ		id_S = -1;
    ಪೂರ್ಣಾಂಕ		id_SNC = 0;
#  ifdef FEAT_TERMINAL
    ಪೂರ್ಣಾಂಕ		id_ST = 0;
    ಪೂರ್ಣಾಂಕ		id_STNC = 0;
#  endif
    ಪೂರ್ಣಾಂಕ		hlcnt;
# endif
#ಆದರೆ_ಅಂತ್ಯ
    win_T	*wp;

    need_highlight_ಬದಲಾಯಿತು = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಪದ_update_colors_all();
    ಪದ_update_hlfwin_all();
#ಆದರೆ_ಅಂತ್ಯ

    // Clear all attributes.
    ಗಾಗಿ (hlf = 0; hlf < (ಪೂರ್ಣಾಂಕ)HLF_ಎಣಿಕೆ; ++hlf)
    {
	highlight_attr[hlf] = 0;
	highlight_attr_raw[hlf] = 0;
	highlight_ids[hlf] = 0;
    }

    default_hl = get_highlight_default();

    // First set all attributes to their default value.
    // Then use the attributes from the 'highlight' option, unless it already
    // matches the default and would repeat the same work.
    ಗಾಗಿ (i = 0; i < 2; ++i)
    {
	ಆದರೆ (i)
	{
	    ಆದರೆ (default_hl != ಖಾಲಿ && p_hl != ಖಾಲಿ
				      && STRCMP(default_hl, p_hl) == 0)
		ಮುಂದುವರಿ;
	    p = p_hl;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    p = default_hl;
	ಆದರೆ (p == ಖಾಲಿ)	    // just in case
	    ಮುಂದುವರಿ;

	ಆಗ (*p)
	{
	    ಗಾಗಿ (hlf = 0; hlf < (ಪೂರ್ಣಾಂಕ)HLF_ಎಣಿಕೆ; ++hlf)
		ಆದರೆ (hl_flags[hlf] == *p)
		    ನಿಲ್ಲಿಸು;
	    ++p;
	    ಆದರೆ (hlf == (ಪೂರ್ಣಾಂಕ)HLF_ಎಣಿಕೆ || *p == NUL)
		ಹಿಂತಿರುಗಿ FAIL;

	    // Allow several hl_flags to be combined, like "bu" for
	    // bold-underlined.
	    attr = 0;
	    id = 0;
	    ಗಾಗಿ ( ; *p && *p != ','; ++p)	    // parse up to comma
	    {
		ಆದರೆ (VIM_ISWHITE(*p))		    // ignore white space
		    ಮುಂದುವರಿ;

		ಆದರೆ (attr > HL_ALL)  // Combination with ':' is not allowed.
		    ಹಿಂತಿರುಗಿ FAIL;

		// Note: Keep this in sync with expand_set_highlight().
		ಆಯ್ಕೆ (*p)
		{
		    ಸ್ಥಿತಿ 'b':	attr |= HL_BOLD;
				ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'i':	attr |= HL_ITALIC;
				ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '-':
		    ಸ್ಥಿತಿ 'n':			    // no highlighting
				ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'r':	attr |= HL_INVERSE;
				ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 's':	attr |= HL_STANDOUT;
				ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'u':	attr |= HL_UNDERLINE;
				ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'c':	attr |= HL_UNDERCURL;
				ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '2':	attr |= HL_UNDERDOUBLE;
				ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'd':	attr |= HL_UNDERDOTTED;
				ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '=':	attr |= HL_UNDERDASHED;
				ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 't':	attr |= HL_STRIKETHROUGH;
				ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ':':	++p;		    // highlight group name
				ಆದರೆ (attr || *p == NUL)	 // no combinations
				    ಹಿಂತಿರುಗಿ FAIL;
				ಕೊನೆ = vim_strchr(p, ',');
				ಆದರೆ (ಕೊನೆ == ಖಾಲಿ)
				    ಕೊನೆ = p + STRLEN(p);
				id = syn_check_ಗುಂಪು(p, (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - p));
				ಆದರೆ (id == 0)
				    ಹಿಂತಿರುಗಿ FAIL;
				attr = syn_id2attr(id);
				p = ಕೊನೆ - 1;
#ಆದರೆ defined(FEAT_STL_OPT) && defined(USER_HIGHLIGHT)
				ಆದರೆ (hlf == (ಪೂರ್ಣಾಂಕ)HLF_SNC)
				    id_SNC = syn_get_final_id(id);
# ifdef FEAT_TERMINAL
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hlf == (ಪೂರ್ಣಾಂಕ)HLF_ST)
				    id_ST = syn_get_final_id(id);
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hlf == (ಪೂರ್ಣಾಂಕ)HLF_STNC)
				    id_STNC = syn_get_final_id(id);
# endif
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hlf == (ಪೂರ್ಣಾಂಕ)HLF_S)
				    id_S = syn_get_final_id(id);
#ಆದರೆ_ಅಂತ್ಯ
				ನಿಲ್ಲಿಸು;
		    ಪೂರ್ವನಿಯೋಜಿತ:	ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    highlight_attr[hlf] = attr;
	    highlight_attr_raw[hlf] = attr;
	    highlight_ids[hlf] = id;

	    p = skip_ಗಮ್ಯ_option_part(p);	    // skip comma and spaces
	}
    }

    // Highlight ids may have been changed, so keep windows up to date
    FOR_ALL_WINDOWS(wp)
    {
	ಅಕ್ಷರ *errmsg = update_winhighlight(wp, wp->w_p_whl);

	ಆದರೆ (errmsg != ಖಾಲಿ)
	    emsg(_(errmsg));
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಪದ_update_hlfwin_all();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USER_HIGHLIGHT
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
    ಆದರೆ (ga_grow(&highlight_ga, 28) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    hlcnt = highlight_ga.ga_len;
    ಆದರೆ (id_S == -1)
    {
	// Make sure id_S is always valid to simplify code below. Use the last
	// entry.
	CLEAR_ಸೂಚಕ(&HL_ಕೋಷ್ಟಕ()[hlcnt + 27]);
	HL_ಕೋಷ್ಟಕ()[hlcnt + 18].sg_ಪದ = highlight_attr[HLF_S];
	id_S = hlcnt + 19;
    }
# endif
    ಗಾಗಿ (i = 0; i < 9; i++)
    {
	sprintf((ಅಕ್ಷರ *)userhl, "User%d", i + 1);
	id = syn_name2id(userhl);
	ಆದರೆ (id == 0)
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
	ಇಲ್ಲದಿದ್ದರೆ
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

#ಆದರೆ_ಅಂತ್ಯ // USER_HIGHLIGHT

    ಹಿಂತಿರುಗಿ OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ highlight_list(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ highlight_list_two(ಪೂರ್ಣಾಂಕ cnt, ಪೂರ್ಣಾಂಕ attr);

// context for :highlight <group> <arg> expansion
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ expand_hi_synid = 0;	    // ID for highlight group being completed
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ expand_hi_equal_ಲಂಬಸಾಲು = 0; // column where the '=' is
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ expand_hi_include_orig = 0;	    // whether to fill the existing current value or not
ಸ್ಥಿರ char_u *expand_hi_curvalue = ಖಾಲಿ;   // the existing current value
#ಆದರೆ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
ಸ್ಥಿರ dict_iterator_T expand_colornames_iter;	// iterator for looping through v:colornames
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Handle command line completion for :highlight command.
 */
    ಶೂನ್ಯ
ಗಣ_context_ಒಳ_highlight_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		expand_ಗುಂಪು = TRUE;

    // Default: expand group names
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_ನಮೂನೆ = arg;
    include_none = 0;
    include_link = 2;
    include_default = 1;

    ಆದರೆ (*arg == NUL)
	ಹಿಂತಿರುಗಿ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ಆದರೆ (*p == NUL)
	ಹಿಂತಿರುಗಿ;

    // past "default" or group name
    include_default = 0;
    ಆದರೆ (STRNCMP("default", arg, p - arg) == 0)
    {
	arg = skipwhite(p);
	xp->xp_ನಮೂನೆ = arg;
	p = skiptowhite(arg);
    }
    ಆದರೆ (*p == NUL)
	ಹಿಂತಿರುಗಿ;

    // past group name
    include_link = 0;
    ಆದರೆ (arg[1] == 'i' && arg[0] == 'N')
    {
	highlight_list();
	expand_ಗುಂಪು = FALSE;
    }
    ಆದರೆ (STRNCMP("link", arg, p - arg) == 0)
    {
	xp->xp_ನಮೂನೆ = skipwhite(p);
	p = skiptowhite(xp->xp_ನಮೂನೆ);
	ಆದರೆ (*p != NUL)		// past first group name
	{
	    xp->xp_ನಮೂನೆ = skipwhite(p);
	    p = skiptowhite(xp->xp_ನಮೂನೆ);
	    include_none = 1;
	}
	expand_ಗುಂಪು = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP("clear", arg, p - arg) == 0)
    {
	xp->xp_ನಮೂನೆ = skipwhite(p);
	p = skiptowhite(xp->xp_ನಮೂನೆ);
	expand_ಗುಂಪು = FALSE;
    }
    ಆದರೆ (*p != NUL)			// past group name(s)
    {
	ಆದರೆ (expand_ಗುಂಪು)
	{
	    // expansion will be done in expand_highlight_group()
	    xp->xp_context = EXPAND_HIGHLIGHT_ಗುಂಪು;

	    expand_hi_synid = syn_namen2id(arg, (ಪೂರ್ಣಾಂಕ)(p - arg));

	    ಆಗ (*p != NUL)
	    {
		arg = skipwhite(p);
		p = skiptowhite(arg);
	    }

	    p = vim_strchr(arg, '=');
	    ಆದರೆ (p == ಖಾಲಿ)
	    {
		// Didn't find a key=<value> pattern
		xp->xp_ನಮೂನೆ = arg;
		expand_hi_equal_ಲಂಬಸಾಲು = -1;
		expand_hi_include_orig = FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Found key=<value> pattern, record the exact location
		expand_hi_equal_ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(p - xp->xp_ಸಾಲು);

		// Only include the original value if the pattern is empty
		ಆದರೆ (*(p + 1) == NUL)
		    expand_hi_include_orig = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		    expand_hi_include_orig = FALSE;

		// Account for comma-separated values
		ಆದರೆ (STRNCMP(arg, "term=", 5) == 0 ||
			STRNCMP(arg, "cterm=", 6) == 0 ||
			STRNCMP(arg, "gui=", 4) == 0)
		{
		    char_u *comma = vim_strrchr(p + 1, ',');
		    ಆದರೆ (comma != ಖಾಲಿ)
			p = comma;
		}
		xp->xp_ನಮೂನೆ = p + 1;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
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
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 10; --i >= 0; )
	highlight_list_two(i, HL_ATTR(HLF_D));
    ಗಾಗಿ (i = 40; --i >= 0; )
	highlight_list_two(99, 0);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
highlight_list_two(ಪೂರ್ಣಾಂಕ cnt, ಪೂರ್ಣಾಂಕ attr)
{
    ಸಂದೇಶ_puts_attr(&("N \bI \b!  \b"[cnt / 11]), attr);
    ಸಂದೇಶ_clr_eos();
    ಹೊರಹೋಗು_flush();
    ui_delay(cnt == 99 ? 40L : (ದೀರ್ಘ)cnt * 50L, FALSE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_highlight_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ get_highlight_name_ext(xp, idx, TRUE);
}

/*
 * Obtain a highlight group name.
 * When "skip_cleared" is TRUE don't return a cleared entry.
 */
    char_u *
get_highlight_name_ext(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ skip_cleared)
{
    ಆದರೆ (idx < 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Items are never removed from the table, skip the ones that were
    // cleared.
    ಆದರೆ (skip_cleared && idx < highlight_ga.ga_len && HL_ಕೋಷ್ಟಕ()[idx].sg_cleared)
	ಹಿಂತಿರುಗಿ (char_u *)"";

    ಆದರೆ (idx == highlight_ga.ga_len && include_none != 0)
	ಹಿಂತಿರುಗಿ (char_u *)"NONE";
    ಆದರೆ (idx == highlight_ga.ga_len + include_none && include_default != 0)
	ಹಿಂತಿರುಗಿ (char_u *)"default";
    ಆದರೆ (idx == highlight_ga.ga_len + include_none + include_default
							 && include_link != 0)
	ಹಿಂತಿರುಗಿ (char_u *)"link";
    ಆದರೆ (idx == highlight_ga.ga_len + include_none + include_default + 1
							 && include_link != 0)
	ಹಿಂತಿರುಗಿ (char_u *)"clear";
    ಆದರೆ (idx >= highlight_ga.ga_len)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ HL_ಕೋಷ್ಟಕ()[idx].sg_name;
}

    ಸ್ಥಿರ char_u *
get_highlight_attr_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx == 0)
    {
	// Fill with current value first
	ಆದರೆ (expand_hi_curvalue != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ expand_hi_curvalue;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ (char_u*)"";
    }
    ಆದರೆ (idx < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(highlight_ಸೂಚ್ಯಂಕ_tab) + 1)
    {
	char_u *ಮೌಲ್ಯ = highlight_ಸೂಚ್ಯಂಕ_tab[idx-1]->ಮೌಲ್ಯ.string;
	ಆದರೆ (expand_hi_curvalue != ಖಾಲಿ && STRCMP(expand_hi_curvalue, ಮೌಲ್ಯ) == 0)
	{
	    // Already returned the current value above, just skip.
	    ಹಿಂತಿರುಗಿ (char_u*)"";
	}
	ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ char_u *
get_highlight_cterm_color(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx == 0)
    {
	// Fill with current value first
	ಆದರೆ (expand_hi_curvalue != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ expand_hi_curvalue;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ (char_u*)"";
    }
    // See highlight_set_cterm_color()
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == 1)
	ಹಿಂತಿರುಗಿ (char_u*)"fg";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == 2)
	ಹಿಂತಿರುಗಿ (char_u*)"bg";
    ಆದರೆ (idx < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(color_name_tab) + 3)
    {
	char_u *ಮೌಲ್ಯ = color_name_tab[idx-3].ಮೌಲ್ಯ.string;
	ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
    ಸ್ಥಿರ char_u *
get_highlight_gui_color(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx == 0)
    {
	// Fill with current value first
	ಆದರೆ (expand_hi_curvalue != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ expand_hi_curvalue;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ (char_u*)"";
    }
    // See color_name2handle()
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == 1)
	ಹಿಂತಿರುಗಿ (char_u*)"fg";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == 2)
	ಹಿಂತಿರುಗಿ (char_u*)"bg";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == 3)
	ಹಿಂತಿರುಗಿ (char_u*)"NONE";

    // Complete from v:colornames. Don't do platform specific names for now.
    typval_T *tv_ಫಲಿತಾಂಶ;
    char_u *colorname = dict_iterate_ಮುಂದಿನ(&expand_colornames_iter, &tv_ಫಲಿತಾಂಶ);
    ಆದರೆ (colorname != ಖಾಲಿ)
    {
	// :hi command doesn't allow space, so don't suggest any malformed items
	ಆದರೆ (vim_strchr(colorname, ' ') != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ (char_u*)"";

	ಆದರೆ (expand_hi_curvalue != ಖಾಲಿ && STRICMP(expand_hi_curvalue, colorname) == 0)
	{
	    // Already returned the current value above, just skip.
	    ಹಿಂತಿರುಗಿ (char_u*)"";
	}
    }
    ಹಿಂತಿರುಗಿ colorname;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ char_u *
get_highlight_ಗುಂಪು_ಕೀಲಿ(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    // Note: Keep this in sync with do_highlight.
    ಸ್ಥಿರ ಅಕ್ಷರ *(p_hi_ಗುಂಪು_ಕೀಲಿ_ಮೌಲ್ಯಗಳು[]) =
    {
	"term=",
	"start=",
	"stop=",
	"cterm=",
	"ctermfg=",
	"ctermbg=",
	"ctermul=",
	"ctermfont=",
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	"gui=",
	"guifg=",
	"guibg=",
	"guisp=",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	"font=",
#ಆದರೆ_ಅಂತ್ಯ
	"NONE",
    };

    ಆದರೆ (idx < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(p_hi_ಗುಂಪು_ಕೀಲಿ_ಮೌಲ್ಯಗಳು))
	ಹಿಂತಿರುಗಿ (char_u*)p_hi_ಗುಂಪು_ಕೀಲಿ_ಮೌಲ್ಯಗಳು[idx];
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Command-line expansion for :hi {group-name} <args>...
 */
    ಪೂರ್ಣಾಂಕ
expand_highlight_ಗುಂಪು(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	ಪೂರ್ಣಾಂಕ	    *ಅಂಕೆ_Matches)
{
    ಆದರೆ (expand_hi_equal_ಲಂಬಸಾಲು != -1)
    {
	// List the values. First fill in the current value, then if possible colors
	// or attribute names.
	char_u	    *(*expandfunc)(expand_T *, ಪೂರ್ಣಾಂಕ) = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ	    type = 0;
	hl_ಗುಂಪು_T  *sgp = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ	    iarg = 0;
	char_u	    *sarg = ಖಾಲಿ;

	ಪೂರ್ಣಾಂಕ	    unsorted_ವಸ್ತುಗಳು = -1; // don't sort by default

	ಆದರೆ (expand_hi_synid != 0)
	    sgp = &HL_ಕೋಷ್ಟಕ()[expand_hi_synid - 1]; // index is ID minus one

	// Note: Keep this in sync with highlight_list_one().
	char_u	    *name_ಕೊನೆ = xp->xp_ಸಾಲು + expand_hi_equal_ಲಂಬಸಾಲು;
	ಆದರೆ (name_ಕೊನೆ - xp->xp_ಸಾಲು >= 5
		&& STRNCMP(name_ಕೊನೆ - 5, " term", 5) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ಆದರೆ (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_ಪದ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name_ಕೊನೆ - xp->xp_ಸಾಲು >= 6
		&& STRNCMP(name_ಕೊನೆ - 6, " cterm", 6) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ಆದರೆ (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_cterm;
	    }
	}
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name_ಕೊನೆ - xp->xp_ಸಾಲು >= 4
		&& STRNCMP(name_ಕೊನೆ - 4, " gui", 4) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ಆದರೆ (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_gui;
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name_ಕೊನೆ - xp->xp_ಸಾಲು >= 8
		&& STRNCMP(name_ಕೊನೆ - 8, " ctermfg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ಆದರೆ (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_fg;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name_ಕೊನೆ - xp->xp_ಸಾಲು >= 8
		&& STRNCMP(name_ಕೊನೆ - 8, " ctermbg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ಆದರೆ (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_bg;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name_ಕೊನೆ - xp->xp_ಸಾಲು >= 8
		&& STRNCMP(name_ಕೊನೆ - 8, " ctermul", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ಆದರೆ (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_ul;
	    }
	}
#ಆದರೆ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name_ಕೊನೆ - xp->xp_ಸಾಲು >= 6
		&& STRNCMP(name_ಕೊನೆ - 6, " guifg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ಆದರೆ (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_fg_name;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name_ಕೊನೆ - xp->xp_ಸಾಲು >= 6
		&& STRNCMP(name_ಕೊನೆ - 6, " guibg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ಆದರೆ (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_bg_name;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name_ಕೊನೆ - xp->xp_ಸಾಲು >= 6
		&& STRNCMP(name_ಕೊನೆ - 6, " guisp", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ಆದರೆ (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_sp_name;
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	ಆದರೆ (expandfunc == get_highlight_gui_color)
	{
	    // Top 4 items are special, after that sort all the color names
	    unsorted_ವಸ್ತುಗಳು = 4;

	    dict_T *colornames_ಕೋಷ್ಟಕ = get_vim_var_dict(VV_COLORNAMES);
	    typval_T colornames_val;
	    colornames_val.v_type = VAR_DICT;
	    colornames_val.vval.v_dict = colornames_ಕೋಷ್ಟಕ;
	    dict_iterate_ಆರಂಭ(&colornames_val, &expand_colornames_iter);
	}
#ಆದರೆ_ಅಂತ್ಯ

	char_u	    buf[MAX_ATTR_LEN];

	expand_hi_curvalue = ಖಾಲಿ;
	ಆದರೆ (expand_hi_include_orig)
	{
	    ಆದರೆ (((type == LIST_ATTR || type == LIST_INT) && iarg != 0) ||
		(type == LIST_STRING && sarg != ಖಾಲಿ))
	    {
		// Retrieve the current value to go first in completion
		expand_hi_curvalue = highlight_arg_ಗಮ್ಯ_string(
			type, iarg, sarg, buf);
	    }
	}

	ಆದರೆ (expandfunc != ಖಾಲಿ)
	{
	    ಹಿಂತಿರುಗಿ ExpandGenericExt(
		    pat,
		    xp,
		    rmp,
		    matches,
		    ಅಂಕೆ_Matches,
		    expandfunc,
		    FALSE,
		    unsorted_ವಸ್ತುಗಳು);
	}

	ಹಿಂತಿರುಗಿ FAIL;
    }

    // List all the key names
    ಹಿಂತಿರುಗಿ ExpandGenericExt(
	    pat,
	    xp,
	    rmp,
	    matches,
	    ಅಂಕೆ_Matches,
	    get_highlight_ಗುಂಪು_ಕೀಲಿ,
	    FALSE,
	    -1);
}

#ಆದರೆ defined(FEAT_GUI)
/*
 * Free all the highlight group fonts.
 * Used when quitting for systems which need it.
 */
    ಶೂನ್ಯ
free_highlight_fonts(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    idx;

    ಗಾಗಿ (idx = 0; idx < highlight_ga.ga_len; ++idx)
    {
	gui_mch_free_font(HL_ಕೋಷ್ಟಕ()[idx].sg_font);
	HL_ಕೋಷ್ಟಕ()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
	gui_mch_free_fontset(HL_ಕೋಷ್ಟಕ()[idx].sg_fontset);
	HL_ಕೋಷ್ಟಕ()[idx].sg_fontset = NOFONTSET;
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
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Convert each of the highlight attribute bits (bold, standout, underline,
 * etc.) set in 'hlattr' into a separate boolean item in a Dictionary with
 * the attribute name as the key.
 */
    ಸ್ಥಿರ dict_T *
highlight_get_attr_dict(ಪೂರ್ಣಾಂಕ hlattr)
{
    dict_T	*dict;
    ಪೂರ್ಣಾಂಕ		i;

    dict = dict_alloc();
    ಆದರೆ (dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(highlight_ಸೂಚ್ಯಂಕ_tab); ++i)
    {
	ಆದರೆ (hlattr & highlight_ಸೂಚ್ಯಂಕ_tab[i]->ಕೀಲಿ)
	{
	    dict_ಕೂಡಿಸು_bool(dict, (ಅಕ್ಷರ *)highlight_ಸೂಚ್ಯಂಕ_tab[i]->ಮೌಲ್ಯ.string,
		    VVAL_TRUE);
	    // don't want "inverse"/"reverse"
	    hlattr &= ~highlight_ಸೂಚ್ಯಂಕ_tab[i]->ಕೀಲಿ;
	}
    }

    ಹಿಂತಿರುಗಿ dict;
}

/*
 * Return the attributes of the highlight group at index 'hl_idx' as a
 * Dictionary. If 'resolve_link' is TRUE, then resolves the highlight group
 * links recursively.
 */
    ಸ್ಥಿರ dict_T *
highlight_get_info(ಪೂರ್ಣಾಂಕ hl_idx, ಪೂರ್ಣಾಂಕ resolve_link)
{
    dict_T	*dict;
    hl_ಗುಂಪು_T	*sgp;
    dict_T	*attr_dict = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		hlgid;

    dict = dict_alloc();
    ಆದರೆ (dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ dict;

    sgp = &HL_ಕೋಷ್ಟಕ()[hl_idx];
    // highlight group id is 1-based
    hlgid = hl_idx + 1;

    ಆದರೆ (dict_ಕೂಡಿಸು_string(dict, "name", sgp->sg_name) == FAIL)
	ಹೋಗು error;
    ಆದರೆ (dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "id", hlgid) == FAIL)
	ಹೋಗು error;

    ಆದರೆ (resolve_link)
    {
	// resolve the highlight group link recursively
	ಆಗ (sgp->sg_link)
	{
	    hlgid = sgp->sg_link;
	    sgp = &HL_ಕೋಷ್ಟಕ()[sgp->sg_link - 1];
	}
    }

    ಆದರೆ (sgp->sg_ಪದ != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_ಪದ);
	ಆದರೆ (attr_dict != ಖಾಲಿ)
	{
	    ಆದರೆ (dict_ಕೂಡಿಸು_dict(dict, "term", attr_dict) == FAIL)
		ಹೋಗು error;
	    attr_dict = ಖಾಲಿ;
	}
    }
    ಆದರೆ (sgp->sg_ಆರಂಭ != ಖಾಲಿ)
	ಆದರೆ (dict_ಕೂಡಿಸು_string(dict, "start", sgp->sg_ಆರಂಭ) == FAIL)
	    ಹೋಗು error;
    ಆದರೆ (sgp->sg_stop != ಖಾಲಿ)
	ಆದರೆ (dict_ಕೂಡಿಸು_string(dict, "stop", sgp->sg_stop) == FAIL)
	    ಹೋಗು error;
    ಆದರೆ (sgp->sg_cterm != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_cterm);
	ಆದರೆ (attr_dict != ಖಾಲಿ)
	{
	    ಆದರೆ (dict_ಕೂಡಿಸು_dict(dict, "cterm", attr_dict) == FAIL)
		ಹೋಗು error;
	    attr_dict = ಖಾಲಿ;
	}
    }
    ಆದರೆ (sgp->sg_cterm_fg != 0)
	ಆದರೆ (dict_ಕೂಡಿಸು_string(dict, "ctermfg",
		    highlight_color(hlgid, (char_u *)"fg", 'c')) == FAIL)
	    ಹೋಗು error;
    ಆದರೆ (sgp->sg_cterm_bg != 0)
	ಆದರೆ (dict_ಕೂಡಿಸು_string(dict, "ctermbg",
			highlight_color(hlgid, (char_u *)"bg", 'c')) == FAIL)
	    ಹೋಗು error;
    ಆದರೆ (sgp->sg_cterm_ul != 0)
	ಆದರೆ (dict_ಕೂಡಿಸು_string(dict, "ctermul",
			highlight_color(hlgid, (char_u *)"ul", 'c')) == FAIL)
	    ಹೋಗು error;
    ಆದರೆ (sgp->sg_cterm_font != 0)
	ಆದರೆ (dict_ಕೂಡಿಸು_string(dict, "ctermfont",
			highlight_color(hlgid, (char_u *)"font", 'c')) == FAIL)
	    ಹೋಗು error;
    ಆದರೆ (sgp->sg_gui != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_gui);
	ಆದರೆ (attr_dict != ಖಾಲಿ)
	{
	    ಆದರೆ (dict_ಕೂಡಿಸು_dict(dict, "gui", attr_dict) == FAIL)
		ಹೋಗು error;
	    attr_dict = ಖಾಲಿ;
	}
    }
    ಆದರೆ (sgp->sg_gui_fg_name != ಖಾಲಿ)
	ಆದರೆ (dict_ಕೂಡಿಸು_string(dict, "guifg",
			highlight_color(hlgid, (char_u *)"fg", 'g')) == FAIL)
	    ಹೋಗು error;
    ಆದರೆ (sgp->sg_gui_bg_name != ಖಾಲಿ)
	ಆದರೆ (dict_ಕೂಡಿಸು_string(dict, "guibg",
			highlight_color(hlgid, (char_u *)"bg", 'g')) == FAIL)
	    ಹೋಗು error;
    ಆದರೆ (sgp->sg_gui_sp_name != ಖಾಲಿ)
	ಆದರೆ (dict_ಕೂಡಿಸು_string(dict, "guisp",
			highlight_color(hlgid, (char_u *)"sp", 'g')) == FAIL)
	    ಹೋಗು error;
# ifdef FEAT_GUI
    ಆದರೆ (sgp->sg_font_name != ಖಾಲಿ)
	ಆದರೆ (dict_ಕೂಡಿಸು_string(dict, "font", sgp->sg_font_name) == FAIL)
	    ಹೋಗು error;
# endif
    ಆದರೆ (sgp->sg_link)
    {
	char_u	*link;

	link = HL_ಕೋಷ್ಟಕ()[sgp->sg_link - 1].sg_name;
	ಆದರೆ (link != ಖಾಲಿ && dict_ಕೂಡಿಸು_string(dict, "linksto", link) == FAIL)
	    ಹೋಗು error;

	ಆದರೆ (sgp->sg_deflink)
	    dict_ಕೂಡಿಸು_bool(dict, "default", VVAL_TRUE);
    }
    ಆದರೆ (dict_len(dict) == 2)
	// If only 'name' is present, then the highlight group is cleared.
	dict_ಕೂಡಿಸು_bool(dict, "cleared", VVAL_TRUE);

    ಹಿಂತಿರುಗಿ dict;

error:
    dict_unref(attr_dict);
    dict_unref(dict);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*hlarg = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		resolve_link = FALSE;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_opt_string_arg(argvars, 0) == FAIL
	    || (argvars[0].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// highlight group name supplied
	hlarg = tv_get_string_chk(&argvars[0]);
	ಆದರೆ (hlarg == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

	ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    ಪೂರ್ಣಾಂಕ error = FALSE;

	    resolve_link = tv_get_bool_chk(&argvars[1], &error);
	    ಆದರೆ (error)
		ಹಿಂತಿರುಗಿ;
	}
    }

    list = rettv->vval.v_list;
    ಗಾಗಿ (i = 0; i < highlight_ga.ga_len && !got_int; ++i)
    {
	ಆದರೆ (hlarg == ಖಾಲಿ || STRICMP(hlarg, HL_ಕೋಷ್ಟಕ()[i].sg_name) == 0)
	{
	    dict = highlight_get_info(i, resolve_link);
	    ಆದರೆ (dict != ಖಾಲಿ && list_append_dict(list, dict) == FAIL)
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
hldict_get_string(dict_T *dict, char_u *ಕೀಲಿ, ಪೂರ್ಣಾಂಕ *error)
{
    dictitem_T	*di;

    *error = FALSE;
    di = dict_ಕಂಡುಹಿಡಿ(dict, ಕೀಲಿ, -1);
    ಆದರೆ (di == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (di->di_tv.v_type != VAR_STRING || di->di_tv.vval.v_string == ಖಾಲಿ)
    {
	emsg(_(e_string_required));
	*error = TRUE;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ di->di_tv.vval.v_string;
}

/*
 * Convert the highlight attribute Dictionary at 'dict[key]' into a string
 * value in 'attr_str' of length 'len'. Returns FALSE if 'dict[key]' is not a
 * Dictionary or is NULL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
hldict_attr_ಗಮ್ಯ_str(
	dict_T	*dict,
	char_u	*ಕೀಲಿ,
	char_u	*attr_str,
	size_t	len)
{
    dictitem_T	*di;
    dict_T	*attrdict;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*p;

    attr_str[0] = NUL;
    di = dict_ಕಂಡುಹಿಡಿ(dict, ಕೀಲಿ, -1);
    ಆದರೆ (di == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;

    ಆದರೆ (di->di_tv.v_type != VAR_DICT || di->di_tv.vval.v_dict == ಖಾಲಿ)
    {
	emsg(_(e_dictionary_required));
	ಹಿಂತಿರುಗಿ FALSE;
    }

    attrdict = di->di_tv.vval.v_dict;

    // If the attribute dict is empty, then return NONE to clear the attributes
    ಆದರೆ (dict_len(attrdict) == 0)
    {
	vim_strcat(attr_str, (char_u *)"NONE", len);
	ಹಿಂತಿರುಗಿ TRUE;
    }

    p = attr_str;
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(highlight_tab); ++i)
    {
	ಆದರೆ (dict_get_bool(attrdict, (ಅಕ್ಷರ *)highlight_tab[i].ಮೌಲ್ಯ.string,
		    VVAL_FALSE) == VVAL_TRUE)
	{
	    ಆದರೆ (p != attr_str && (size_t)(p - attr_str + 2) < len)
		STRCPY(p, (char_u *)",");
	    ಆದರೆ (p - attr_str + highlight_tab[i].ಮೌಲ್ಯ.ಉದ್ದಳತೆ + 1 < len)
	    {
		STRCPY(p, highlight_tab[i].ಮೌಲ್ಯ.string);
		p += highlight_tab[i].ಮೌಲ್ಯ.ಉದ್ದಳತೆ;
	    }
	}
    }

    ಹಿಂತಿರುಗಿ TRUE;
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
ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(char_u *dptr, char_u *attr, ಪೂರ್ಣಾಂಕ attrlen, char_u *ಮೌಲ್ಯ)
{
    size_t	vallen;

    // Do nothing if the value is not specified or is empty
    ಆದರೆ (ಮೌಲ್ಯ == ಖಾಲಿ || *ಮೌಲ್ಯ == NUL)
	ಹಿಂತಿರುಗಿ dptr;

    vallen = STRLEN(ಮೌಲ್ಯ);
    // When the value contains a space and the attribute has an "=" (i.e. it
    // is a key=value pair), surround the value with single quotes so that
    // do_highlight() can parse it correctly.
    ಆದರೆ (vim_strchr(ಮೌಲ್ಯ, ' ') != ಖಾಲಿ && vim_strchr(attr, '=') != ಖಾಲಿ)
    {
	ಆದರೆ (dptr + attrlen + vallen + 3 < hlsetBuf + HLSETBUFSZ)
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
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dptr + attrlen + vallen + 1 < hlsetBuf + HLSETBUFSZ)
    {
	STRCPY(dptr, attr);
	dptr += attrlen;
	STRCPY(dptr, ಮೌಲ್ಯ);
	dptr += vallen;
    }

    ಹಿಂತಿರುಗಿ dptr;
}

/*
 * Add or update a highlight group using 'dict' items. Returns TRUE if
 * successfully updated the highlight group.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
hlg_ಕೂಡಿಸು_or_update(dict_T *dict)
{
    char_u	*name;
    ಪೂರ್ಣಾಂಕ		error;
    char_u	ಪದ_attr[MAX_ATTR_LEN];
    char_u	cterm_attr[MAX_ATTR_LEN];
    char_u	gui_attr[MAX_ATTR_LEN];
    char_u	*ಆರಂಭ;
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
    ಪೂರ್ಣಾಂಕ		forceit = FALSE;
    ಪೂರ್ಣಾಂಕ		dodefault = FALSE;
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ = FALSE;
    char_u	*p;

    name = hldict_get_string(dict, (char_u *)"name", &error);
    ಆದರೆ (name == ಖಾಲಿ || *name == NUL || error)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (dict_get_bool(dict, "force", VVAL_FALSE) == VVAL_TRUE)
	forceit = TRUE;

    ಆದರೆ (dict_get_bool(dict, "default", VVAL_FALSE) == VVAL_TRUE)
	dodefault = TRUE;

    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "cleared"))
    {
	varnumber_T	cleared;

	// clear a highlight group
	cleared = dict_get_bool(dict, "cleared", FALSE);
	ಆದರೆ (cleared == TRUE)
	{
	    vim_snprintf((ಅಕ್ಷರ *)hlsetBuf, HLSETBUFSZ, "clear %s", name);
	    do_highlight(hlsetBuf, forceit, FALSE);
	    ಮುಗಿದ = TRUE;
	}
    }

    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "linksto"))
    {
	char_u	*linksto;

	// link highlight groups
	linksto = hldict_get_string(dict, (char_u *)"linksto", &error);
	ಆದರೆ (linksto == ಖಾಲಿ || *linksto == NUL || error)
	    ಹಿಂತಿರುಗಿ FALSE;

	vim_snprintf((ಅಕ್ಷರ *)hlsetBuf, HLSETBUFSZ, "%slink %s %s",
				dodefault ? "default " : "", name, linksto);
	do_highlight(hlsetBuf, forceit, FALSE);

	ಮುಗಿದ = TRUE;
    }

    // If 'cleared' or 'linksto' are specified, then don't process the other
    // attributes.
    ಆದರೆ (ಮುಗಿದ)
	ಹಿಂತಿರುಗಿ TRUE;

    ಆರಂಭ = hldict_get_string(dict, (char_u *)"start", &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ FALSE;

    stop = hldict_get_string(dict, (char_u *)"stop", &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (!hldict_attr_ಗಮ್ಯ_str(dict, (char_u *)"term", ಪದ_attr,
							    ಗಾತ್ರ(ಪದ_attr)))
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (!hldict_attr_ಗಮ್ಯ_str(dict, (char_u *)"cterm", cterm_attr,
							   ಗಾತ್ರ(cterm_attr)))
	ಹಿಂತಿರುಗಿ FALSE;

    ctermfg = hldict_get_string(dict, (char_u *)"ctermfg", &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ FALSE;

    ctermbg = hldict_get_string(dict, (char_u *)"ctermbg", &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ FALSE;

    ctermul = hldict_get_string(dict, (char_u *)"ctermul", &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ FALSE;

    ctermfont = hldict_get_string(dict, (char_u *)"ctermfont", &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (!hldict_attr_ಗಮ್ಯ_str(dict, (char_u *)"gui", gui_attr, ಗಾತ್ರ(gui_attr)))
	ಹಿಂತಿರುಗಿ FALSE;

    guifg = hldict_get_string(dict, (char_u *)"guifg", &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ FALSE;

    guibg = hldict_get_string(dict, (char_u *)"guibg", &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ FALSE;

    guisp = hldict_get_string(dict, (char_u *)"guisp", &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ FALSE;

# ifdef FEAT_GUI
    font = hldict_get_string(dict, (char_u *)"font", &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ FALSE;
# endif

    // If none of the attributes are specified, then do nothing.
    ಆದರೆ (ಪದ_attr[0] == NUL && ಆರಂಭ == ಖಾಲಿ && stop == ಖಾಲಿ
	    && cterm_attr[0] == NUL && ctermfg == ಖಾಲಿ && ctermbg == ಖಾಲಿ
	    && ctermul == ಖಾಲಿ && ctermfont == ಖಾಲಿ && gui_attr[0] == NUL
# ifdef FEAT_GUI
	    && font == ಖಾಲಿ
# endif
	    && guifg == ಖಾಲಿ && guibg == ಖಾಲಿ && guisp == ಖಾಲಿ
	    )
	ಹಿಂತಿರುಗಿ TRUE;

    hlsetBuf[0] = NUL;
    p = hlsetBuf;
    ಆದರೆ (dodefault)
	p = ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(p, (char_u *)"default", 7, (char_u *)" ");
    p = ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(p, (char_u *)"", 0, name);
    p = ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(p, (char_u *)" term=", 6, ಪದ_attr);
    p = ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(p, (char_u *)" start=", 7, ಆರಂಭ);
    p = ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(p, (char_u *)" stop=", 6, stop);
    p = ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(p, (char_u *)" cterm=", 7, cterm_attr);
    p = ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(p, (char_u *)" ctermfg=", 9, ctermfg);
    p = ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(p, (char_u *)" ctermbg=", 9, ctermbg);
    p = ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(p, (char_u *)" ctermul=", 9, ctermul);
    p = ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(p, (char_u *)" ctermfont=", 11, ctermfont);
    p = ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(p, (char_u *)" gui=", 5, gui_attr);
# ifdef FEAT_GUI
    p = ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(p, (char_u *)" font=", 6, font);
# endif
    p = ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(p, (char_u *)" guifg=", 7, guifg);
    p = ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(p, (char_u *)" guibg=", 7, guibg);
    (ಶೂನ್ಯ)ಕೂಡಿಸು_attr_and_ಮೌಲ್ಯ(p, (char_u *)" guisp=", 7, guisp);

    do_highlight(hlsetBuf, forceit, FALSE);

    ಹಿಂತಿರುಗಿ TRUE;
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

    ಆದರೆ (check_for_list_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(argvars->vval.v_list, li)
    {
	ಆದರೆ (li->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_dictionary_required));
	    ಹಿಂತಿರುಗಿ;
	}

	dict = li->li_tv.vval.v_dict;
	ಆದರೆ (!hlg_ಕೂಡಿಸು_or_update(dict))
	    ಹಿಂತಿರುಗಿ;
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * If "old" is in the override stack, then update it to "hl_new". Does not free
 * "old".
 */
    ಶೂನ್ಯ
update_highlight_overrides(hl_override_T *old, hl_override_T *hl_new, ಪೂರ್ಣಾಂಕ newlen)
{
    ಆದರೆ (old == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (hl_overrides_T *ಗಣ_2 = overrides; ಗಣ_2 != ಖಾಲಿ; ಗಣ_2 = ಗಣ_2->ಮುಂದಿನ)
    {
	ಆದರೆ (ಗಣ_2->arr == old)
	{
	    ಗಣ_2->arr = hl_new;
	    ಗಣ_2->len = newlen;
	}
    }
}

/*
 * If "arr" is in the highlight overrides list, then mark it as invalid.
 */
    ಶೂನ್ಯ
remove_highlight_overrides(hl_override_T *arr)
{
    ಆದರೆ (arr == ಖಾಲಿ || overrides == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (hl_overrides_T *ಗಣ_2 = overrides; ಗಣ_2 != ಖಾಲಿ; ಗಣ_2 = ಗಣ_2->ಮುಂದಿನ)
    {
	ಆದರೆ (ಗಣ_2->arr == arr)
	{
	    ಗಣ_2->arr = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	}
    }
}

/*
 * Update highlight_attr[] array. If "update_ids" is true, then update
 * highlight_ids[] array instead.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_highlight_attr(hl_override_T *arr, ಪೂರ್ಣಾಂಕ len, bool update_ids)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < len; i++)
    {
	hl_override_T	*override = arr + i;
	ಪೂರ್ಣಾಂಕ		hlf = -1, attr;

	ಆದರೆ (override->from <= 0)
	    // Directly map to highlight_attr
	    hlf = -override->from;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < HLF_ಎಣಿಕೆ; k++)
		ಆದರೆ (override->from == highlight_ids[k])
		{
		    hlf = k;
		    ನಿಲ್ಲಿಸು;
		}

	    ಆದರೆ (hlf == -1)
		ಮುಂದುವರಿ;
	}

	ಆದರೆ (update_ids)
	{
	    ಆದರೆ (override->ಗಮ್ಯ <= 0)
	    {
		ಆದರೆ (hlf == -override->ಗಮ್ಯ)
		    highlight_ids[hlf] = -1;
		ಇಲ್ಲದಿದ್ದರೆ
		    highlight_ids[hlf] = highlight_ids[-override->ಗಮ್ಯ];
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		highlight_ids[hlf] = override->ಗಮ್ಯ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (override->ಗಮ್ಯ <= 0)
		// Directly use highlight_attr[]
		attr = highlight_attr[-override->ಗಮ್ಯ];
	    ಇಲ್ಲದಿದ್ದರೆ
		attr = syn_id2attr(override->ಗಮ್ಯ);

	    highlight_attr[hlf] = attr;
	}
    }
}

/*
 * Set the current highlight override to "arr". Returns true if successful
 */
    bool
ತಳ್ಳು_highlight_overrides(hl_override_T *arr, ಪೂರ್ಣಾಂಕ len)
{
    // Don't want to do anything if "arr" is already the current override. If
    // "arr" is NULL (but overrides->arr is not), then still push an override,
    // but "->arr" will just be NULL so any previous overrides are cleared.
    ಆದರೆ (overrides != ಖಾಲಿ && overrides->arr == arr)
	ಹಿಂತಿರುಗಿ false;

    hl_overrides_T *ಗಣ_2;

    ಗಣ_2 = ALLOC_ONE(hl_overrides_T);
    ಆದರೆ (ಗಣ_2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ false;

    ಗಣ_2->arr = arr;
    ಗಣ_2->len = len;

    // Push to front of list
    ಗಣ_2->ಮುಂದಿನ = overrides;
    overrides = ಗಣ_2;

    // Save current highlight_attr[], so it can be restored later after this
    // override is popped.
    ಸ್ಮರಣೆ_ನಕಲು(ಗಣ_2->attr, highlight_attr, ಗಾತ್ರ(highlight_attr));

    // Reset highlight_attr[] to default values, as we don't want changes from
    // other overrides.
    ಸ್ಮರಣೆ_ನಕಲು(highlight_attr, highlight_attr_raw, ಗಾತ್ರ(highlight_attr));

    // Update highlight_attr[] array
    ಆದರೆ (arr != ಖಾಲಿ)
	ಗಣ_highlight_attr(arr, len, false);

    ಹಿಂತಿರುಗಿ true;
}

/*
 * Pop the current highlight override (if any) from the stack. This should
 * always be paired with a *successful* push.
 */
    ಶೂನ್ಯ
ತೆಗೆ_highlight_overrides(ಶೂನ್ಯ)
{
    hl_overrides_T *ಗಣ_2 = overrides;

    ಆದರೆ (overrides == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    overrides = ಗಣ_2->ಮುಂದಿನ;

    // Set highlight_attr[] to state before override was pushed.
    ಸ್ಮರಣೆ_ನಕಲು(highlight_attr, ಗಣ_2->attr, ಗಾತ್ರ(highlight_attr));
    vim_free(ಗಣ_2);
}

/*
 * Parse the 'winhighlight' option and return array. Returns NULL on failure or
 * if empty option. If failure, then errmsg is set.
 */
    ಸ್ಥಿರ hl_override_T *
parse_winhighlight(char_u *opt, ಪೂರ್ಣಾಂಕ *len, ಅಕ್ಷರ **errmsg)
{
    char_u	    *p = opt;
    hl_override_T   *arr;
    ಪೂರ್ಣಾಂಕ		    i = 0;
    ಪೂರ್ಣಾಂಕ		    ಅಂಕೆ = 1;
    ಪೂರ್ಣಾಂಕ		    n_colons = 0;

    ಆದರೆ (*p == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Get number of overrides first so we can allocate array
    ಆಗ ((p = vim_strchr(p, ',')) != ಖಾಲಿ)
    {
	p++;
	ಅಂಕೆ++;
    }
    p = opt;
    // Check if number of ':' matches number of ','
    ಆಗ ((p = vim_strchr(p, ':')) != ಖಾಲಿ)
    {
	p++;
	n_colons++;
    }

    ಆದರೆ (ಅಂಕೆ != n_colons)
    {
	*errmsg = e_invalid_ವಾದ;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    arr = ALLOC_MULT(hl_override_T, ಅಂಕೆ);
    ಆದರೆ (arr == ಖಾಲಿ)
    {
	*errmsg = e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    p = opt;

    ಆಗ (true)
    {
	hl_override_T *override = arr + i++;
	char_u	*fromname = p, *toname;
	char_u	*tmp;
	char_u	**ಹೆಸರುಗಳು[2] = {&fromname, &toname};
	ಪೂರ್ಣಾಂಕ	fromlen, tolen;
	ಪೂರ್ಣಾಂಕ	*lens[2] = {&fromlen, &tolen};
	ಪೂರ್ಣಾಂಕ	fromid, toid;
	ಪೂರ್ಣಾಂಕ	*ids[2] = {&fromid, &toid};

	p = vim_strchr(p, ':');

	ಆದರೆ (p == ಖಾಲಿ)
	    ಹೋಗು fail;

	fromlen = p - fromname; // Get hl for "from"
	ಆದರೆ (fromlen == 0)
	    ಹೋಗು fail;
	p++; // Skip colon ':'
	ಆದರೆ (*p == NUL)
	    ಹೋಗು fail;

	toname = p;
	tmp = vim_strchr(p, ',');

	// Get hl for "to", must check for no trailing comma in case last
	// element.
	ಆದರೆ (tmp == ಖಾಲಿ)
	    tolen = (ಪೂರ್ಣಾಂಕ)STRLEN(p);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    tolen = tmp - toname;
	    p = ++tmp;
	}
	ಆದರೆ (tolen == 0)
	    ಹೋಗು fail;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < 2; k++)
	{
	    char_u  *name = *ಹೆಸರುಗಳು[k];
	    ಪೂರ್ಣಾಂಕ	    nlen = *lens[k];

	    ಆದರೆ (*name == '!')
	    {
		// If starts with "!", then it maps directly to a 'highlight'
		// occasion.
		ಪೂರ್ಣಾಂಕ hlf;

		ಆದರೆ (nlen != 2)
		    ಹೋಗು fail;

		ಗಾಗಿ (hlf = 0; hlf < (ಪೂರ್ಣಾಂಕ)HLF_ಎಣಿಕೆ; ++hlf)
		    ಆದರೆ (hl_flags[hlf] == name[1])
			ನಿಲ್ಲಿಸು;

		ಆದರೆ (hlf >= HLF_ಎಣಿಕೆ)
		    ಹೋಗು fail;

		*ids[k] = -hlf;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Otherwise get the highlight group id.
		ಆದರೆ (syn_check_ಗುಂಪು(name, nlen) == 0)
		    ಹೋಗು fail;

		*ids[k] = syn_namen2id(name, nlen);
		ಆದರೆ (*ids[k] == 0)
		    ಹೋಗು fail;

		// Always map "Normal" group to HLF_WIN
		ಆದರೆ (ids[k] == &fromid &&
			STRCMP(HL_ಕೋಷ್ಟಕ()[*ids[k] - 1].sg_name_u, "NORMAL") == 0)
		    *ids[k] = -HLF_WIN;
	    }
	}

	// If fromid == toid, leave it be, this is so that we know HLF_WIN has
	// been set (even to itself e.g. "Normal"), and therefore know to use
	// its attr instead.
	override->from = fromid;
	override->ಗಮ್ಯ = toid;

	ಆದರೆ (tmp == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
    }

    *len = ಅಂಕೆ;
    ಹಿಂತಿರುಗಿ arr;
fail:
    vim_free(arr);
    *errmsg = e_invalid_ವಾದ;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Update w_hl for "wp" using given option value. Returns error
 * message on failure, otherwise NULL.
 */
    ಅಕ್ಷರ *
update_winhighlight(win_T *wp, char_u *opt)
{
    ಅಕ್ಷರ	    *err = ಖಾಲಿ;
    hl_override_T   *arr;
    ಪೂರ್ಣಾಂಕ		    ಅಂಕೆ = 1;

    arr = parse_winhighlight(opt, &ಅಂಕೆ, &err);

    ಆದರೆ (arr == ಖಾಲಿ && err != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ err;

    ಆದರೆ (arr == ಖಾಲಿ && wp->w_hl == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    update_highlight_overrides(wp->w_hl, arr, ಅಂಕೆ);

    vim_free(wp->w_hl);
    wp->w_hl = arr;
    wp->w_hl_len = ಅಂಕೆ;

    wp->w_hlfwin_id = hlf_get_id(wp, HLF_WIN);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಪದ_update_hlfwin(wp);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    // May be NULL (such as in after_copy_winopt())
    ಆದರೆ (wp->w_ಬಫರ್ != ಖಾಲಿ)
    {
	// Make sure terminal highlighting is updated
	bool ret = ತಳ್ಳು_highlight_overrides(wp->w_hl, wp->w_hl_len);

	ಆದರೆ (wp->w_ಬಫರ್->b_ಪದ != ಖಾಲಿ)
	    ಪದ_init_default_colors(wp->w_ಬಫರ್->b_ಪದ);
	ಆದರೆ (ret)
	    ತೆಗೆ_highlight_overrides();
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Get id that the hlf attr is using in window "wp", otherwise zero. Note that
 * -1 may be returned if the hlf was set to itself.
 */
    ಪೂರ್ಣಾಂಕ
hlf_get_id(win_T *wp, ಪೂರ್ಣಾಂಕ hlf)
{
    ಪೂರ್ಣಾಂಕ id;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prev[HLF_ಎಣಿಕೆ];

    ಆದರೆ (wp->w_hl == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ highlight_ids[hlf];

    ಸ್ಮರಣೆ_ನಕಲು(prev, highlight_ids, ಗಾತ್ರ(highlight_ids));
    ಗಣ_highlight_attr(wp->w_hl, wp->w_hl_len, true);
    id = highlight_ids[hlf];
    ಸ್ಮರಣೆ_ನಕಲು(highlight_ids, prev, ಗಾತ್ರ(highlight_ids));

    ಹಿಂತಿರುಗಿ id;
}

/*
 * Update 'winhighlight' using a 'wincolor' style option value. Returns error
 * message on failure otherwise NULL. Does not update the actual 'wincolor'
 * value, but does update the 'winhighlight' value.
 */
    ಅಕ್ಷರ *
update_wincolor(win_T *wp, char_u *opt)
{
    char_u  *str = *opt == NUL ? "" : alloc(ಗಾತ್ರ("!(:") + STRLEN(opt));
    ಅಕ್ಷರ    *errmsg;

    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    ಆದರೆ (*opt != NUL)
	sprintf((ಅಕ್ಷರ *)str, "!(:%s", opt);

    errmsg = update_winhighlight(wp, str);

    ಆದರೆ (errmsg == ಖಾಲಿ)
	ಗಣ_string_option_ನೇರ_ಒಳ_win(wp, (char_u *)"winhighlight", -1,
		str, OPT_FREE|OPT_LOCAL, 0);

    ಆದರೆ (*opt != NUL)
	ಬಿಡುಗಡೆ(str);
    ಹಿಂತಿರುಗಿ errmsg;
}
