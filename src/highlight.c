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

#అన్తర్భావ "vim.h"

#నిర్వచించు SG_పదం		1	// term has been set
#నిర్వచించు SG_CTERM	2	// cterm has been set
#నిర్వచించు SG_GUI		4	// gui has been set
#నిర్వచించు SG_LINK		8	// link has been set

#నిర్వచించు MAX_SYN_NAME	200

/*
 * The "term", "cterm" and "gui" arguments can be any combination of the
 * following names, separated by commas (but no spaces!).
 */
// must be sorted by the 'value' field because it is used by bsearch()!
// note: inverse and reverse use the same key
స్థిర keyvalue_T highlight_tab[] = {
    KEYVALUE_నమోదు(HL_BOLD, "bold"),			    // index 0
    KEYVALUE_నమోదు(HL_INVERSE, "inverse"),		    // index 1
    KEYVALUE_నమోదు(HL_ITALIC, "italic"),		    // index 2
    KEYVALUE_నమోదు(HL_NOCOMBINE, "nocombine"),		    // index 3
    KEYVALUE_నమోదు(HL_సాధారణ, "NONE"),			    // index 4
    KEYVALUE_నమోదు(HL_INVERSE, "reverse"),		    // index 5
    KEYVALUE_నమోదు(HL_STANDOUT, "standout"),		    // index 6
    KEYVALUE_నమోదు(HL_STRIKETHROUGH, "strikethrough"),	    // index 7
    KEYVALUE_నమోదు(HL_UNDERCURL, "undercurl"),		    // index 8
    KEYVALUE_నమోదు(HL_UNDERDASHED, "underdashed"),	    // index 9
    KEYVALUE_నమోదు(HL_UNDERDOTTED, "underdotted"),	    // index 10
    KEYVALUE_నమోదు(HL_UNDERDOUBLE, "underdouble"),	    // index 11
    KEYVALUE_నమోదు(HL_UNDERLINE, "underline")		    // index 12
};

// this table is used to display highlight names in the "correct" sequence.
// keep this in sync with highlight_tab[].
స్థిర keyvalue_T *highlight_సూచీ_tab[] = {
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
#నిర్వచించు MAX_ATTR_LEN 120

#నిర్వచించు ATTR_COMBINE(attr_a, attr_b) ((((attr_b) & HL_NOCOMBINE) ? (attr_b) : (attr_a)) | (attr_b))

గణన {
    BLACK = 0,
    DARKBLUE,
    DARKGREEN,
    DARKCYAN,
    DARKRED,
    DARKMAGENTA,
    BROWN,
    DARKYELLOW,
    బూడిద,
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
స్థిర keyvalue_T color_name_tab[] = {
    KEYVALUE_నమోదు(BLACK, "Black"),
    KEYVALUE_నమోదు(BLUE, "Blue"),
    KEYVALUE_నమోదు(BROWN, "Brown"),
    KEYVALUE_నమోదు(CYAN, "Cyan"),
    KEYVALUE_నమోదు(DARKBLUE, "DarkBlue"),
    KEYVALUE_నమోదు(DARKCYAN, "DarkCyan"),
    KEYVALUE_నమోదు(DARKGRAY, "DarkGray"),
    KEYVALUE_నమోదు(DARKGREEN, "DarkGreen"),
    KEYVALUE_నమోదు(DARKGREY, "DarkGrey"),
    KEYVALUE_నమోదు(DARKMAGENTA, "DarkMagenta"),
    KEYVALUE_నమోదు(DARKRED, "DarkRed"),
    KEYVALUE_నమోదు(DARKYELLOW, "DarkYellow"),
    KEYVALUE_నమోదు(బూడిద, "Gray"),
    KEYVALUE_నమోదు(GREEN, "Green"),
    KEYVALUE_నమోదు(GREY, "Grey"),
    KEYVALUE_నమోదు(LIGHTBLUE, "LightBlue"),
    KEYVALUE_నమోదు(LIGHTCYAN, "LightCyan"),
    KEYVALUE_నమోదు(LIGHTGRAY, "LightGray"),
    KEYVALUE_నమోదు(LIGHTGREEN, "LightGreen"),
    KEYVALUE_నమోదు(LIGHTGREY, "LightGrey"),
    KEYVALUE_నమోదు(LIGHTMAGENTA, "LightMagenta"),
    KEYVALUE_నమోదు(LIGHTRED, "LightRed"),
    KEYVALUE_నమోదు(LIGHTYELLOW, "LightYellow"),
    KEYVALUE_నమోదు(MAGENTA, "Magenta"),
    KEYVALUE_నమోదు(NONE, "NONE"),
    KEYVALUE_నమోదు(RED, "Red"),
    KEYVALUE_నమోదు(WHITE, "White"),
    KEYVALUE_నమోదు(YELLOW, "Yellow")
};

/*
 * Structure that stores information about a highlight group.
 * The ID of a highlight group is also called group ID.  It is the index in
 * the highlight_ga array PLUS ONE.
 */
రకనిర్వచనం నిర్మాణం
{
    char_u	*sg_name;	// highlight group name
    char_u	*sg_name_u;	// uppercase of sg_name
    పూర్ణాంకం		sg_cleared;	// "hi clear" was used
// for normal terminals
    పూర్ణాంకం		sg_పదం;	// "term=" highlighting attributes
    char_u	*sg_ప్రారంభం;	// terminal string for start highl
    char_u	*sg_stop;	// terminal string for stop highl
    పూర్ణాంకం		sg_పదం_attr;	// Screen attr for term mode
// for color terminals
    పూర్ణాంకం		sg_cterm;	// "cterm=" highlighting attr
    పూర్ణాంకం		sg_cterm_bold;	// bold attr was set for light color
    పూర్ణాంకం		sg_cterm_fg;	// terminal fg color number + 1
    పూర్ణాంకం		sg_cterm_bg;	// terminal bg color number + 1
    పూర్ణాంకం		sg_cterm_ul;	// terminal ul color number + 1
    పూర్ణాంకం		sg_cterm_attr;	// Screen attr for color term mode
    పూర్ణాంకం		sg_cterm_font;	// terminal alternative font (0 for normal)
// for when using the GUI
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    guicolor_T	sg_gui_fg;	// GUI foreground color handle
    guicolor_T	sg_gui_bg;	// GUI background color handle
    guicolor_T	sg_gui_sp;	// GUI special color handle
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
    GuiFont	sg_font;	// GUI font handle
# ifdef FEAT_XFONTSET
    GuiFontset	sg_fontset;	// GUI fontset handle
# endif
    char_u	*sg_font_name;  // GUI font or fontset name
    పూర్ణాంకం		sg_gui_attr;    // Screen attr for GUI mode
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_EVAL)
// Store the sp color name for the GUI or synIDattr()
    పూర్ణాంకం		sg_gui;		// "gui=" highlighting attributes
    char_u	*sg_gui_fg_name;// GUI foreground color name
    char_u	*sg_gui_bg_name;// GUI background color name
    char_u	*sg_gui_sp_name;// GUI special color name
#అయితే_ముగింపు
    పూర్ణాంకం		sg_link;	// link to this highlight group ID
    పూర్ణాంకం		sg_deflink;	// default link; restored in highlight_clear()
    పూర్ణాంకం		sg_సమితి;		// combination of SG_* flags
#నిర్వచితమైతే FEAT_EVAL
    sctx_T	sg_deflink_sctx;  // script where the default link was set
    sctx_T	sg_script_ctx;	// script in which the group was last set
#అయితే_ముగింపు
} hl_సమూహం_T;

రకనిర్వచనం నిర్మాణం hl_overrides_S hl_overrides_T;
నిర్మాణం hl_overrides_S
{
    hl_override_T   *arr;   // May be NULL if "arr" was freed or no highlight
			    // overrides (all values set to default)
    పూర్ణాంకం		    len;
    hl_overrides_T  *తదుపరి;  // Used to handle recursive calls

    పూర్ణాంకం attr[HLF_లెక్క];    // highlight_attr[] before being updated.
};

స్థిర hl_overrides_T *overrides = ఖాళీ;

// Synced with highlight_attr, each is the highlight group id for each attr. If
// an element is 0, then there is no attr for it. If an element is -1, then it
// means it was set to itself.
స్థిర పూర్ణాంకం highlight_ids[HLF_లెక్క];

// Same as highlight_attr[] but is not modified by highlight group overrides
స్థిర పూర్ణాంకం highlight_attr_raw[HLF_లెక్క];

స్థిర పూర్ణాంకం hl_flags[HLF_లెక్క] = HL_FLAGS;

// highlight groups for 'highlight' option
స్థిర garray_T highlight_ga;
#నిర్వచించు HL_పట్టిక()	((hl_సమూహం_T *)((highlight_ga.ga_దత్తాంశం)))

// Wrapper struct for hashtable entries: stores the highlight group ID alongside
// the uppercase name used as a hash key.  Uses the same offsetof pattern as
// signgroup_T / HI2SG.
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		hn_id;		// highlight group ID (1-based)
    char_u	hn_కీలకం[1];	// uppercase name (stretchable array)
} hlname_T;

#నిర్వచించు HLNAME_కీలకం_OFF	offsetof(hlname_T, hn_కీలకం)
#నిర్వచించు HI2HLNAME(hi)	((hlname_T *)((hi)->hi_కీలకం - HLNAME_కీలకం_OFF))

// hashtable for quick highlight group name lookup
స్థిర hashtab_T highlight_ht;
స్థిర bool	 highlight_ht_inited = false;

/*
 * An attribute number is the index in attr_table plus ATTR_OFF.
 */
#నిర్వచించు ATTR_OFF (HL_ALL + 1)

స్థిర శూన్య syn_unadd_సమూహం(శూన్య);
స్థిర శూన్య సమితి_hl_attr(పూర్ణాంకం idx);
స్థిర శూన్య highlight_list_one(పూర్ణాంకం id);
స్థిర పూర్ణాంకం highlight_list_arg(పూర్ణాంకం id, పూర్ణాంకం didh, పూర్ణాంకం type, పూర్ణాంకం iarg, char_u *sarg, అక్షరం *name);
స్థిర పూర్ణాంకం syn_కూడు_సమూహం(char_u *name);
స్థిర పూర్ణాంకం syn_name2id_len(char_u *name, పూర్ణాంకం len);
స్థిర పూర్ణాంకం hl_has_settings(పూర్ణాంకం idx, పూర్ణాంకం check_link);
స్థిర శూన్య highlight_clear(పూర్ణాంకం idx);

#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
స్థిర శూన్య gui_do_one_color(పూర్ణాంకం idx, పూర్ణాంకం do_menu, పూర్ణాంకం do_tooltip);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
స్థిర పూర్ణాంకం  సమితి_సమూహం_colors(char_u *name, guicolor_T *fgp, guicolor_T *bgp, పూర్ణాంకం do_menu, పూర్ణాంకం use_norm, పూర్ణాంకం do_tooltip);
స్థిర శూన్య hl_do_font(పూర్ణాంకం idx, char_u *arg, పూర్ణాంకం do_సాధారణ, పూర్ణాంకం do_menu, పూర్ణాంకం do_tooltip, పూర్ణాంకం free_font);
#అయితే_ముగింపు

/*
 * The default highlight groups.  These are compiled-in for fast startup and
 * they still work when the runtime files can't be found.
 * When making changes here, also change runtime/colors/default.vim!
 * The #ifdefs are needed to reduce the amount of static data.  Helps to make
 * the 16 bit DOS (museum) version compile.
 */
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_EVAL)
# define CENT(a, b) b
#లేకపోతే
# define CENT(a, b) a
#అయితే_ముగింపు
స్థిర అక్షరం *(highlight_init_both[]) = {
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
#నిర్వచితమైతే FEAT_CLIPBOARD
    CENT("VisualNOS term=underline,bold cterm=underline,bold",
	 "VisualNOS term=underline,bold cterm=underline,bold gui=underline,bold"),
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
    CENT("DiffText term=reverse cterm=bold ctermbg=Red ctermfg=Black",
	 "DiffText term=reverse cterm=bold ctermbg=Red ctermfg=Black gui=bold guibg=Red"),
    "default link DiffTextAdd DiffText",
#అయితే_ముగింపు
    CENT("PmenuSbar term=NONE ctermbg=Grey",
	 "PmenuSbar term=NONE ctermbg=Grey guibg=Grey"),
    CENT("TabLineSel term=bold cterm=bold",
	 "TabLineSel term=bold cterm=bold gui=bold"),
    CENT("TabLineFill term=reverse cterm=reverse",
	 "TabLineFill term=reverse cterm=reverse gui=reverse"),
    "default link TabPanel TabLine",
    "default link TabPanelSel TabLineSel",
    "default link TabPanelFill TabLineFill",
#నిర్వచితమైతే FEAT_GUI
    "Cursor guibg=fg guifg=bg",
    "lCursor guibg=fg guifg=bg", // should be different, but what?
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_MSWIN
    "TitleBar guibg=bg guifg=fg",
    "TitleBarNC guibg=NONE guifg=NONE",
#అయితే_ముగింపు
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
    ఖాళీ
};

// Default colors only used with a light background.
స్థిర అక్షరం *(highlight_init_light[]) = {
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
#నిర్వచితమైతే FEAT_SPELL
    CENT("SpellBad term=reverse ctermbg=LightRed",
	 "SpellBad term=reverse ctermbg=LightRed guisp=Red gui=undercurl"),
    CENT("SpellCap term=reverse ctermbg=LightBlue",
	 "SpellCap term=reverse ctermbg=LightBlue guisp=Blue gui=undercurl"),
    CENT("SpellRare term=reverse ctermbg=LightMagenta",
	 "SpellRare term=reverse ctermbg=LightMagenta guisp=Magenta gui=undercurl"),
    CENT("SpellLocal term=underline ctermbg=Cyan",
	 "SpellLocal term=underline ctermbg=Cyan guisp=DarkCyan gui=undercurl"),
#అయితే_ముగింపు
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
#నిర్వచితమైతే FEAT_FOLDING
    CENT("Folded term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "Folded term=standout ctermbg=Grey ctermfg=DarkBlue guibg=LightGrey guifg=DarkBlue"),
    CENT("FoldColumn term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "FoldColumn term=standout ctermbg=Grey ctermfg=DarkBlue guibg=Grey guifg=DarkBlue"),
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
    CENT("SignColumn term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "SignColumn term=standout ctermbg=Grey ctermfg=DarkBlue guibg=Grey guifg=DarkBlue"),
#అయితే_ముగింపు
    CENT("Visual ctermbg=Grey ctermfg=Black",
	 "Visual ctermbg=Grey ctermfg=Black guibg=LightGrey guifg=Black"),
#నిర్వచితమైతే FEAT_DIFF
    CENT("DiffAdd term=bold ctermbg=LightBlue ctermfg=Black",
	 "DiffAdd term=bold ctermbg=LightBlue ctermfg=Black guibg=LightBlue"),
    CENT("DiffChange term=bold ctermbg=LightMagenta ctermfg=Black",
	 "DiffChange term=bold ctermbg=LightMagenta ctermfg=Black guibg=LightMagenta"),
    CENT("DiffDelete term=bold ctermfg=Blue ctermbg=LightCyan",
	 "DiffDelete term=bold ctermfg=Blue ctermbg=LightCyan gui=bold guifg=Blue guibg=LightCyan"),
#అయితే_ముగింపు
    CENT("TabLine term=underline cterm=underline ctermfg=black ctermbg=LightGrey",
	 "TabLine term=underline cterm=underline ctermfg=black ctermbg=LightGrey gui=underline guibg=LightGrey"),
#నిర్వచితమైతే FEAT_SYN_HL
    CENT("CursorColumn term=reverse ctermbg=LightGrey",
	 "CursorColumn term=reverse ctermbg=LightGrey guibg=Grey90"),
    CENT("CursorLine term=underline cterm=underline",
	 "CursorLine term=underline cterm=underline guibg=Grey90"),
    CENT("ColorColumn term=reverse ctermbg=LightRed",
	 "ColorColumn term=reverse ctermbg=LightRed guibg=LightRed"),
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
    CENT("Conceal ctermbg=DarkGrey ctermfg=LightGrey",
	 "Conceal ctermbg=DarkGrey ctermfg=LightGrey guibg=DarkGrey guifg=LightGrey"),
#అయితే_ముగింపు
    CENT("MatchParen term=reverse ctermbg=Cyan",
	 "MatchParen term=reverse ctermbg=Cyan guibg=Cyan"),
#నిర్వచితమైతే FEAT_TERMINAL
    CENT("StatusLineTerm term=reverse,bold cterm=bold ctermfg=White ctermbg=DarkGreen",
	 "StatusLineTerm term=reverse,bold cterm=bold ctermfg=White ctermbg=DarkGreen gui=bold guifg=bg guibg=DarkGreen"),
    CENT("StatusLineTermNC term=reverse ctermfg=White ctermbg=DarkGreen",
	 "StatusLineTermNC term=reverse ctermfg=White ctermbg=DarkGreen guifg=bg guibg=DarkGreen"),
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MENU
    CENT("ToolbarLine term=underline ctermbg=LightGrey",
	 "ToolbarLine term=underline ctermbg=LightGrey guibg=LightGrey"),
    CENT("ToolbarButton cterm=bold ctermfg=White ctermbg=DarkGrey",
	 "ToolbarButton cterm=bold ctermfg=White ctermbg=DarkGrey gui=bold guifg=White guibg=Grey40"),
#అయితే_ముగింపు
    ఖాళీ
};

// Default colors only used with a dark background.
స్థిర అక్షరం *(highlight_init_dark[]) = {
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
#నిర్వచితమైతే FEAT_SPELL
    CENT("SpellBad term=reverse ctermbg=Red",
	 "SpellBad term=reverse ctermbg=Red guisp=Red gui=undercurl"),
    CENT("SpellCap term=reverse ctermbg=Blue",
	 "SpellCap term=reverse ctermbg=Blue guisp=Blue gui=undercurl"),
    CENT("SpellRare term=reverse ctermbg=Magenta",
	 "SpellRare term=reverse ctermbg=Magenta guisp=Magenta gui=undercurl"),
    CENT("SpellLocal term=underline ctermbg=Cyan",
	 "SpellLocal term=underline ctermbg=Cyan guisp=Cyan gui=undercurl"),
#అయితే_ముగింపు
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
#నిర్వచితమైతే FEAT_FOLDING
    CENT("Folded term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "Folded term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=DarkGrey guifg=Cyan"),
    CENT("FoldColumn term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "FoldColumn term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=Grey guifg=Cyan"),
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
    CENT("SignColumn term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "SignColumn term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=Grey guifg=Cyan"),
#అయితే_ముగింపు
    CENT("Visual ctermbg=Grey ctermfg=Black",
	 "Visual ctermbg=Grey ctermfg=Black guibg=#575757 guifg=LightGrey"),
#నిర్వచితమైతే FEAT_DIFF
    CENT("DiffAdd term=bold ctermbg=DarkBlue",
	 "DiffAdd term=bold ctermbg=DarkBlue guibg=DarkBlue"),
    CENT("DiffChange term=bold ctermbg=DarkMagenta",
	 "DiffChange term=bold ctermbg=DarkMagenta guibg=DarkMagenta"),
    CENT("DiffDelete term=bold ctermfg=Blue ctermbg=DarkCyan",
	 "DiffDelete term=bold ctermfg=Blue ctermbg=DarkCyan gui=bold guifg=Blue guibg=DarkCyan"),
#అయితే_ముగింపు
    CENT("TabLine term=underline cterm=underline ctermfg=white ctermbg=DarkGrey",
	 "TabLine term=underline cterm=underline ctermfg=white ctermbg=DarkGrey gui=underline guibg=DarkGrey"),
#నిర్వచితమైతే FEAT_SYN_HL
    CENT("CursorColumn term=reverse ctermbg=DarkGrey",
	 "CursorColumn term=reverse ctermbg=DarkGrey guibg=Grey40"),
    CENT("CursorLine term=underline cterm=underline",
	 "CursorLine term=underline cterm=underline guibg=Grey40"),
    CENT("ColorColumn term=reverse ctermbg=DarkRed",
	 "ColorColumn term=reverse ctermbg=DarkRed guibg=DarkRed"),
#అయితే_ముగింపు
    CENT("MatchParen term=reverse ctermbg=DarkCyan",
	 "MatchParen term=reverse ctermbg=DarkCyan guibg=DarkCyan"),
#నిర్వచితమైతే FEAT_CONCEAL
    CENT("Conceal ctermbg=DarkGrey ctermfg=LightGrey",
	 "Conceal ctermbg=DarkGrey ctermfg=LightGrey guibg=DarkGrey guifg=LightGrey"),
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
    CENT("StatusLineTerm term=reverse,bold cterm=bold ctermfg=Black ctermbg=LightGreen",
	 "StatusLineTerm term=reverse,bold cterm=bold ctermfg=Black ctermbg=LightGreen gui=bold guifg=bg guibg=LightGreen"),
    CENT("StatusLineTermNC term=reverse ctermfg=Black ctermbg=LightGreen",
	 "StatusLineTermNC term=reverse ctermfg=Black ctermbg=LightGreen guifg=bg guibg=LightGreen"),
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MENU
    CENT("ToolbarLine term=underline ctermbg=DarkGrey",
	 "ToolbarLine term=underline ctermbg=DarkGrey guibg=Grey50"),
    CENT("ToolbarButton cterm=bold ctermfg=Black ctermbg=LightGrey",
	 "ToolbarButton cterm=bold ctermfg=Black ctermbg=LightGrey gui=bold guifg=Black guibg=LightGrey"),
#అయితే_ముగింపు
    ఖాళీ
};

#ఒకవేళ defined(FEAT_SYN_HL)
/*
 * Returns the number of highlight groups.
 */
    పూర్ణాంకం
highlight_అంకె_groups(శూన్య)
{
    తిరిగి highlight_ga.ga_len;
}

/*
 * Returns the name of a highlight group.
 */
    char_u *
highlight_సమూహం_name(పూర్ణాంకం id)
{
    తిరిగి HL_పట్టిక()[id].sg_name;
}

/*
 * Returns the ID of the link to a highlight group.
 */
    పూర్ణాంకం
highlight_link_id(పూర్ణాంకం id)
{
    తిరిగి HL_పట్టిక()[id].sg_link;
}
#అయితే_ముగింపు

స్థిర శూన్య resolve_fallback_fg_గమ్యం_rgb(శూన్య);
స్థిర శూన్య resolve_fallback_bg_గమ్యం_rgb(శూన్య);

    శూన్య
init_highlight(
    పూర్ణాంకం		both,	    // include groups where 'bg' doesn't matter
    పూర్ణాంకం		reset)	    // clear group first
{
    పూర్ణాంకం		i;
    అక్షరం	**pp;
    స్థిర పూర్ణాంకం	had_both = FALSE;
#నిర్వచితమైతే FEAT_EVAL
    char_u	*p;

    // Try finding the color scheme file.  Used when a color file was loaded
    // and 'background' or 't_Co' is changed.
    p = get_var_విలువ((char_u *)"g:colors_name");
    ఒకవేళ (p != ఖాళీ)
    {
	// The value of g:colors_name could be freed when sourcing the script,
	// making "p" invalid, so copy it.
	char_u *copy_p = vim_strsave(p);
	పూర్ణాంకం    r;

	ఒకవేళ (copy_p != ఖాళీ)
	{
	    r = ఎక్కించు_colors(copy_p);
	    vim_free(copy_p);
	    ఒకవేళ (r == OK)
		తిరిగి;
	}
    }

#అయితే_ముగింపు

    // Didn't use a color file, use the compiled-in colors.
    ఒకవేళ (both)
    {
	had_both = TRUE;
	pp = highlight_init_both;
	కొరకు (i = 0; pp[i] != ఖాళీ; ++i)
	    do_highlight((char_u *)pp[i], reset, TRUE);
    }
    లేకపోతే ఒకవేళ (!had_both)
	// Don't do anything before the call with both == TRUE from main().
	// Not everything has been setup then, and that call will overrule
	// everything anyway.
	తిరిగి;

    ఒకవేళ (*p_bg == 'l')
	pp = highlight_init_light;
    లేకపోతే
	pp = highlight_init_dark;
    కొరకు (i = 0; pp[i] != ఖాళీ; ++i)
	do_highlight((char_u *)pp[i], reset, TRUE);

    // Reverse looks ugly, but grey may not work for less than 8 colors.  Thus
    // let it depend on the number of colors available.
    ఒకవేళ (t_colors < 8)
	do_highlight((char_u *)"Visual term=reverse cterm=reverse ctermbg=NONE ctermfg=NONE",
		FALSE, TRUE);
    // With 8 colors brown is equal to yellow, need to use black for Search fg
    // to avoid Statement highlighted text disappears.
    // Clear the attributes, needed when changing the t_Co value.
    ఒకవేళ (t_colors <= 8)
    {
	ఒకవేళ (*p_bg == 'l')
	    do_highlight((char_u *)"Search ctermfg=black", FALSE, TRUE);
    }

#నిర్వచితమైతే FEAT_SYN_HL
    // If syntax highlighting is enabled load the highlighting for it.
    ఒకవేళ (get_var_విలువ((char_u *)"g:syntax_on") != ఖాళీ)
    {
	స్థిర పూర్ణాంకం	recursive = 0;

	ఒకవేళ (recursive >= 5)
	    emsg(_(e_recursive_loop_loading_syncolor_vim));
	లేకపోతే
	{
	    ++recursive;
	    (శూన్య)ఆధారం_runtime((char_u *)"syntax/syncolor.vim", DIP_ALL);
	    --recursive;
	}
    }
#అయితే_ముగింపు
    resolve_fallback_fg_గమ్యం_rgb();
    resolve_fallback_bg_గమ్యం_rgb();
}

#ఒకవేళ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
/*
 * Load a default color list. Intended to support legacy color names but allows
 * the user to override the color values. Only loaded once.
 */
    స్థిర శూన్య
ఎక్కించు_default_colors_lists(శూన్య)
{
    // Lacking a default color list isn't the end of the world but it is likely
    // an inconvenience so users should know when it is missing.
    ఒకవేళ (ఆధారం_runtime((char_u *)"colors/lists/default.vim", DIP_ALL) != OK)
	సందేశం("failed to load colors/lists/default.vim");
}
#అయితే_ముగింపు

/*
 * Load color file "name".
 * Return OK for success, FAIL for failure.
 */
    పూర్ణాంకం
ఎక్కించు_colors(char_u *name)
{
    char_u	*buf;
    పూర్ణాంకం		retval = FAIL;
    స్థిర పూర్ణాంకం	recursive = FALSE;

    // When being called recursively, this is probably because setting
    // 'background' caused the highlighting to be reloaded.  This means it is
    // working, thus we should return OK.
    ఒకవేళ (recursive)
	తిరిగి OK;

    recursive = TRUE;
    buf = alloc(STRLEN(name) + 12);
    ఒకవేళ (buf != ఖాళీ)
    {
#ఒకవేళ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	ఎక్కించు_default_colors_lists();
#అయితే_ముగింపు
	apply_autocmds(EVENT_COLORSCHEMEPRE, name,
					       curbuf->b_fname, FALSE, curbuf);
	sprintf((అక్షరం *)buf, "colors/%s.vim", name);
	retval = ఆధారం_runtime(buf, DIP_ప్రారంభం + DIP_OPT);
	vim_free(buf);
	ఒకవేళ (retval == OK)
	    apply_autocmds(EVENT_COLORSCHEME, name, curbuf->b_fname,
								FALSE, curbuf);
    }
    recursive = FALSE;

    తిరిగి retval;
}

స్థిర పూర్ణాంకం color_numbers_16[28] = {0, 1, 2, 3,
				 4, 5, 6, 6,
				 7, 7, 7, 7,
				 8, 8,
				 9, 9, 10, 10,
				 11, 11, 12, 12, 13,
				 13, 14, 14, 15, -1};
// for xterm with 88 colors...
స్థిర పూర్ణాంకం color_numbers_88[28] = {0, 4, 2, 6,
				 1, 5, 32, 72,
				 84, 84, 7, 7,
				 82, 82,
				 12, 43, 10, 61,
				 14, 63, 9, 74, 13,
				 75, 11, 78, 15, -1};
// for xterm with 256 colors...
స్థిర పూర్ణాంకం color_numbers_256[28] = {0, 4, 2, 6,
				 1, 5, 130, 3,
				 248, 248, 7, 7,
				 242, 242,
				 12, 81, 10, 121,
				 14, 159, 9, 224, 13,
				 225, 11, 229, 15, -1};
// for terminals with less than 16 colors...
స్థిర పూర్ణాంకం color_numbers_8[28] = {0, 4, 2, 6,
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
    స్థిర పూర్ణాంకం
lookup_color(పూర్ణాంకం idx, పూర్ణాంకం foreground, పూర్ణాంకం *boldp)
{
    పూర్ణాంకం		color = color_numbers_16[idx];
    char_u	*p;

    // Use the _16 table to check if it's a valid color name.
    ఒకవేళ (color < 0)
	తిరిగి -1;

    ఒకవేళ (t_colors == 8)
    {
	// t_Co is 8: use the 8 colors table
#ఒకవేళ defined(__QNXNTO__)
	// On qnx, the 8 & 16 color arrays are the same
	ఒకవేళ (STRNCMP(T_NAME, "qansi", 5) == 0)
	    color = color_numbers_16[idx];
	లేకపోతే
#అయితే_ముగింపు
	    color = color_numbers_8[idx];
	ఒకవేళ (foreground)
	{
	    // set/reset bold attribute to get light foreground
	    // colors (on some terminals, e.g. "linux")
	    ఒకవేళ (color & 8)
		*boldp = TRUE;
	    లేకపోతే
		*boldp = FALSE;
	}
	color &= 7;	// truncate to 8 colors
    }
    లేకపోతే ఒకవేళ (t_colors == 16 || t_colors == 88
					   || t_colors >= 256)
    {
	// Guess: if the termcap entry ends in 'm', it is
	// probably an xterm-like terminal.  Use the changed
	// order for colors.
	ఒకవేళ (*T_CAF != NUL)
	    p = T_CAF;
	లేకపోతే
	    p = T_CSF;
	ఒకవేళ (*p != NUL && (t_colors > 256
			      || *(p + STRLEN(p) - 1) == 'm'))
	{
	    ఒకవేళ (t_colors == 88)
		color = color_numbers_88[idx];
	    లేకపోతే ఒకవేళ (t_colors >= 256)
		color = color_numbers_256[idx];
	    లేకపోతే
		color = color_numbers_8[idx];
	}
#నిర్వచితమైతే FEAT_TERMRESPONSE
	ఒకవేళ (t_colors >= 256 && color == 15 && is_mac_terminal)
	    // Terminal.app has a bug: 15 is light grey. Use white
	    // from the color cube instead.
	    color = 231;
#అయితే_ముగింపు
    }
    తిరిగి color;
}

/*
 * Link highlight group 'from_hg' to 'to_hg'.
 * 'dodefault' is set to TRUE for ":highlight default link".
 * 'forceit' is set to TRUE for ":highlight! link"
 * 'init' is set to TRUE when initializing all the highlight groups.
 */
    స్థిర శూన్య
highlight_సమూహం_link(
	char_u	*from_hg,
	పూర్ణాంకం	from_len,
	char_u	*గమ్యం_hg,
	పూర్ణాంకం	గమ్యం_len,
	పూర్ణాంకం	dodefault,
	పూర్ణాంకం	forceit,
	పూర్ణాంకం	init)
{
    పూర్ణాంకం		from_id;
    పూర్ణాంకం		గమ్యం_id;
    hl_సమూహం_T	*hlgroup = ఖాళీ;

    from_id = syn_check_సమూహం(from_hg, from_len);
    ఒకవేళ (STRNCMP(గమ్యం_hg, "NONE", 4) == 0)
	గమ్యం_id = 0;
    లేకపోతే
	గమ్యం_id = syn_check_సమూహం(గమ్యం_hg, గమ్యం_len);

    ఒకవేళ (from_id > 0)
    {
	hlgroup = &HL_పట్టిక()[from_id - 1];
	ఒకవేళ (dodefault && (forceit || hlgroup->sg_deflink == 0))
	{
	    hlgroup->sg_deflink = గమ్యం_id;
#నిర్వచితమైతే FEAT_EVAL
	    hlgroup->sg_deflink_sctx = current_sctx;
	    hlgroup->sg_deflink_sctx.sc_lnum += SOURCING_LNUM;
#అయితే_ముగింపు
	}
    }

    ఒకవేళ (from_id > 0 && (!init || hlgroup->sg_సమితి == 0))
    {
	// Don't allow a link when there already is some highlighting
	// for the group, unless '!' is used
	ఒకవేళ (గమ్యం_id > 0 && !forceit && !init
		&& hl_has_settings(from_id - 1, dodefault))
	{
	    ఒకవేళ (SOURCING_NAME == ఖాళీ && !dodefault)
		emsg(_(e_సమూహం_has_settings_highlight_link_ignored));
	}
	లేకపోతే ఒకవేళ (hlgroup->sg_link != గమ్యం_id
#నిర్వచితమైతే FEAT_EVAL
		|| hlgroup->sg_script_ctx.sc_sid != current_sctx.sc_sid
#అయితే_ముగింపు
		|| hlgroup->sg_cleared)
	{
	    ఒకవేళ (!init)
		hlgroup->sg_సమితి |= SG_LINK;
	    hlgroup->sg_link = గమ్యం_id;
#నిర్వచితమైతే FEAT_EVAL
	    hlgroup->sg_script_ctx = current_sctx;
	    hlgroup->sg_script_ctx.sc_lnum += SOURCING_LNUM;
#అయితే_ముగింపు
	    hlgroup->sg_cleared = FALSE;
	    redraw_all_later(UPD_SOME_VALID);

	    // Only call highlight_changed() once after multiple changes.
	    need_highlight_మారింది = TRUE;
	}
    }

}

/*
 * Reset all highlighting to the defaults. Removes all highlighting for the
 * groups added by the user.
 */
    స్థిర శూన్య
highlight_reset_all(శూన్య)
{
    పూర్ణాంకం		idx;

#నిర్వచితమైతే FEAT_GUI
    // First, we do not destroy the old values, but allocate the new
    // ones and update the display. THEN we destroy the old values.
    // If we destroy the old values first, then the old values
    // (such as GuiFont's or GuiFontset's) will still be displayed but
    // invalid because they were free'd.
    ఒకవేళ (gui.లోపల_use)
    {
# ifdef FEAT_BEVAL_TIP
	gui_init_tooltip_font();
# endif
# ఒకవేళ defined(FEAT_MENU) && defined(FEAT_GUI_MOTIF)
	gui_init_menu_font();
# endif
    }
# ఒకవేళ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_X11)
    gui_mch_def_colors();
# endif
# ifdef FEAT_GUI_MSWIN
    ఒకవేళ (gui.లోపల_use)
	gui_mch_సమితి_titlebar_colors();
# endif
# ifdef FEAT_GUI_X11
#  ifdef FEAT_MENU

    // This only needs to be done when there is no Menu highlight
    // group defined by default, which IS currently the case.
    gui_mch_new_menu_colors();
#  endif
    ఒకవేళ (gui.లోపల_use)
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
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
    do_unlet((char_u *)"g:colors_name", TRUE);
#అయితే_ముగింపు
    restore_cterm_colors();

    // Clear all default highlight groups and load the defaults.
    కొరకు (idx = 0; idx < highlight_ga.ga_len; ++idx)
	highlight_clear(idx);
    init_highlight(TRUE, TRUE);
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ఒకవేళ (USE_24_బిట్టు)
	highlight_gui_started();
    లేకపోతే
#అయితే_ముగింపు
	highlight_మారింది();
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
    స్థిర పూర్ణాంకం
highlight_సమితి_termgui_attr(పూర్ణాంకం idx, char_u *కీలకం, char_u *arg, పూర్ణాంకం init)
{
    పూర్ణాంకం		attr;
    size_t	off;
    keyvalue_T	లక్ష్యం;
    keyvalue_T	*నమోదు_2;

    attr = 0;
    off = 0;
    లక్ష్యం.కీలకం = 0;
    లక్ష్యం.విలువ.నిడివి = 0;	// not used, see cmp_keyvalue_value_ni()
    అయితే (arg[off] != NUL)
    {
	లక్ష్యం.విలువ.string = arg + off;
	నమోదు_2 = (keyvalue_T *)bsearch(&లక్ష్యం, &highlight_tab,
		ARRAY_నిడివి(highlight_tab), పరిమాణం(highlight_tab[0]),
		cmp_keyvalue_విలువ_ni);
	ఒకవేళ (నమోదు_2 == ఖాళీ)
	{
	    semsg(_(e_illegal_విలువ_str), arg);
	    తిరిగి FALSE;
	}

	attr |= నమోదు_2->కీలకం;
	off += నమోదు_2->విలువ.నిడివి;
	ఒకవేళ (arg[off] == ',')		// another one follows
	    ++off;
    }
    ఒకవేళ (*కీలకం == 'T')
    {
	ఒకవేళ (!init || !(HL_పట్టిక()[idx].sg_సమితి & SG_పదం))
	{
	    ఒకవేళ (!init)
		HL_పట్టిక()[idx].sg_సమితి |= SG_పదం;
	    HL_పట్టిక()[idx].sg_పదం = attr;
	}
    }
    లేకపోతే ఒకవేళ (*కీలకం == 'C')
    {
	ఒకవేళ (!init || !(HL_పట్టిక()[idx].sg_సమితి & SG_CTERM))
	{
	    ఒకవేళ (!init)
		HL_పట్టిక()[idx].sg_సమితి |= SG_CTERM;
	    HL_పట్టిక()[idx].sg_cterm = attr;
	    HL_పట్టిక()[idx].sg_cterm_bold = FALSE;
	}
    }
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_EVAL)
    లేకపోతే
    {
	ఒకవేళ (!init || !(HL_పట్టిక()[idx].sg_సమితి & SG_GUI))
	{
	    ఒకవేళ (!init)
		HL_పట్టిక()[idx].sg_సమితి |= SG_GUI;
	    HL_పట్టిక()[idx].sg_gui = attr;
	}
    }
#అయితే_ముగింపు

    తిరిగి TRUE;
}

#నిర్వచితమైతే FEAT_GUI
/*
 * Set the font for the highlight group at 'idx'.
 * 'arg' is the font name.
 * Returns TRUE if the font is changed.
 */
    స్థిర పూర్ణాంకం
highlight_సమితి_font(
	పూర్ణాంకం	idx,
	char_u	*arg,
	పూర్ణాంకం	is_సాధారణ_సమూహం,
	పూర్ణాంకం	is_menu_సమూహం,
	పూర్ణాంకం	is_tooltip_సమూహం)
{
    పూర్ణాంకం		did_change = FALSE;

    // in non-GUI fonts are simply ignored
    ఒకవేళ (HL_పట్టిక()[idx].sg_font_name != ఖాళీ
	    && STRCMP(HL_పట్టిక()[idx].sg_font_name, arg) == 0)
    {
	// Font name didn't change, ignore.
    }
    లేకపోతే ఒకవేళ (!gui.shell_created)
    {
	// GUI not started yet, always accept the name.
	vim_free(HL_పట్టిక()[idx].sg_font_name);
	HL_పట్టిక()[idx].sg_font_name = vim_strsave(arg);
	did_change = TRUE;
    }
    లేకపోతే
    {
	GuiFont తాత్కాలిక_sg_font = HL_పట్టిక()[idx].sg_font;
# ifdef FEAT_XFONTSET
	GuiFontset తాత్కాలిక_sg_fontset = HL_పట్టిక()[idx].sg_fontset;
# endif
	// First, save the current font/fontset.
	// Then try to allocate the font/fontset.
	// If the allocation fails, HL_TABLE()[idx].sg_font OR
	// sg_fontset will be set to NOFONT or NOFONTSET respectively.

	HL_పట్టిక()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
	HL_పట్టిక()[idx].sg_fontset = NOFONTSET;
# endif
	hl_do_font(idx, arg, is_సాధారణ_సమూహం, is_menu_సమూహం,
		is_tooltip_సమూహం, FALSE);

# ifdef FEAT_XFONTSET
	ఒకవేళ (HL_పట్టిక()[idx].sg_fontset != NOFONTSET)
	{
	    // New fontset was accepted. Free the old one, if there
	    // was one.
	    gui_mch_free_fontset(తాత్కాలిక_sg_fontset);
	    did_change = TRUE;
	}
	లేకపోతే
	    HL_పట్టిక()[idx].sg_fontset = తాత్కాలిక_sg_fontset;
# endif
	ఒకవేళ (HL_పట్టిక()[idx].sg_font != NOFONT)
	{
	    // New font was accepted. Free the old one, if there was
	    // one.
	    gui_mch_free_font(తాత్కాలిక_sg_font);
	    did_change = TRUE;
	}
	లేకపోతే
	    HL_పట్టిక()[idx].sg_font = తాత్కాలిక_sg_font;
	// Update sg_font_name once here, after both font and fontset results
	// are known.
	ఒకవేళ (did_change)
	{
	    vim_free(HL_పట్టిక()[idx].sg_font_name);
	    HL_పట్టిక()[idx].sg_font_name = vim_strsave(arg);
	}
    }

    తిరిగి did_change;
}
#అయితే_ముగింపు

/*
 * Set the cterm foreground color for the Normal highlight group to "color" and
 * the bold attribute to "bold".
 */
    స్థిర శూన్య
hl_సమితి_ctermfg_సాధారణ_సమూహం(పూర్ణాంకం color, పూర్ణాంకం bold)
{
    cterm_సాధారణ_fg_color = color + 1;
    cterm_సాధారణ_fg_bold = bold;
#నిర్వచితమైతే FEAT_GUI
    // Don't do this if the GUI is used.
    ఒకవేళ (!gui.లోపల_use && !gui.starting)
#అయితే_ముగింపు
    {
	సమితి_must_redraw(UPD_CLEAR);
	ఒకవేళ (termcap_చురుకైన && color >= 0)
	    పదం_fg_color(color);
    }
}

/*
 * Set the cterm foreground color for the highlight group at 'idx' to 'color'.
 */
    స్థిర శూన్య
highlight_సమితి_ctermfg(పూర్ణాంకం idx, పూర్ణాంకం color, పూర్ణాంకం is_సాధారణ_సమూహం)
{
    HL_పట్టిక()[idx].sg_cterm_fg = color + 1;
    ఒకవేళ (is_సాధారణ_సమూహం)
	hl_సమితి_ctermfg_సాధారణ_సమూహం(color,
					(HL_పట్టిక()[idx].sg_cterm & HL_BOLD));
}

/*
 * Set the cterm background color for the Normal highlight group to "color".
 */
    స్థిర శూన్య
hl_సమితి_ctermbg_సాధారణ_సమూహం(పూర్ణాంకం color)
{
    cterm_సాధారణ_bg_color = color + 1;
#నిర్వచితమైతే FEAT_GUI
    // Don't mess with 'background' if the GUI is used.
    ఒకవేళ (!gui.లోపల_use && !gui.starting)
#అయితే_ముగింపు
    {
	సమితి_must_redraw(UPD_CLEAR);
	ఒకవేళ (color >= 0)
	{
	    పూర్ణాంకం dark = -1;

	    ఒకవేళ (termcap_చురుకైన)
		పదం_bg_color(color);
	    ఒకవేళ (t_colors < 16)
		dark = (color == 0 || color == 4);
	    // Limit the heuristic to the standard 16 colors
	    లేకపోతే ఒకవేళ (color < 16)
		dark = (color < 7 || color == 8);
	    // Set the 'background' option if the value is
	    // wrong.
	    ఒకవేళ (dark != -1
		    && dark != (*p_bg == 'd')
		    && !option_was_సమితి((char_u *)"bg"))
	    {
		సమితి_option_విలువ_give_err((char_u *)"bg",
			0L, (char_u *)(dark ? "dark" : "light"), 0);
		reset_option_was_సమితి((char_u *)"bg");
	    }
	}
    }
}

/*
 * Set the cterm background color for the highlight group at 'idx' to 'color'.
 */
    స్థిర శూన్య
highlight_సమితి_ctermbg(పూర్ణాంకం idx, పూర్ణాంకం color, పూర్ణాంకం is_సాధారణ_సమూహం)
{
    HL_పట్టిక()[idx].sg_cterm_bg = color + 1;
    ఒకవేళ (is_సాధారణ_సమూహం)
	hl_సమితి_ctermbg_సాధారణ_సమూహం(color);
}

/*
 * Set the cterm underline color for the Normal highlight group to "color".
 */
    స్థిర శూన్య
hl_సమితి_ctermul_సాధారణ_సమూహం(పూర్ణాంకం color)
{
    cterm_సాధారణ_ul_color = color + 1;
#నిర్వచితమైతే FEAT_GUI
    // Don't do this if the GUI is used.
    ఒకవేళ (!gui.లోపల_use && !gui.starting)
#అయితే_ముగింపు
    {
	సమితి_must_redraw(UPD_CLEAR);
	ఒకవేళ (termcap_చురుకైన && color >= 0)
	    పదం_ul_color(color);
    }
}

/*
 * Set the cterm underline color for the highlight group at 'idx' to 'color'.
 */
    స్థిర శూన్య
highlight_సమితి_ctermul(పూర్ణాంకం idx, పూర్ణాంకం color, పూర్ణాంకం is_సాధారణ_సమూహం)
{
    HL_పట్టిక()[idx].sg_cterm_ul = color + 1;
    ఒకవేళ (is_సాధారణ_సమూహం)
	hl_సమితి_ctermul_సాధారణ_సమూహం(color);
}

/*
 * Set the cterm font for the highlight group at 'idx'.
 * 'arg' is the color name or the numeric value as a string.
 * 'init' is set to TRUE when initializing highlighting.
 * Called for the ":highlight" command and the "hlset()" function.
 *
 * Returns TRUE if the font is set.
 */
    స్థిర పూర్ణాంకం
highlight_సమితి_cterm_font(
	పూర్ణాంకం	idx,
	char_u	*arg,
	పూర్ణాంకం	init)
{
    పూర్ణాంకం		font;

    ఒకవేళ (init && (HL_పట్టిక()[idx].sg_సమితి & SG_CTERM))
	తిరిగి FALSE;

    ఒకవేళ (!init)
	HL_పట్టిక()[idx].sg_సమితి |= SG_CTERM;

    ఒకవేళ (VIM_ISDIGIT(*arg))
	font = atoi((అక్షరం *)arg);
    లేకపోతే ఒకవేళ (STRICMP(arg, "NONE") == 0)
	font = -1;
    లేకపోతే
	తిరిగి FALSE;

    HL_పట్టిక()[idx].sg_cterm_font = font + 1;
    తిరిగి TRUE;
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
    స్థిర పూర్ణాంకం
highlight_సమితి_cterm_color(
	పూర్ణాంకం	idx,
	char_u	*కీలకం,
	char_u	*కీలకం_ప్రారంభం,
	char_u	*arg,
	పూర్ణాంకం	is_సాధారణ_సమూహం,
	పూర్ణాంకం	init)
{
    పూర్ణాంకం		color;

    ఒకవేళ (init && (HL_పట్టిక()[idx].sg_సమితి & SG_CTERM))
	తిరిగి FALSE;

    ఒకవేళ (!init)
	HL_పట్టిక()[idx].sg_సమితి |= SG_CTERM;

    // When setting the foreground color, and previously the "bold"
    // flag was set for a light color, reset it now
    ఒకవేళ (కీలకం[5] == 'F' && HL_పట్టిక()[idx].sg_cterm_bold)
    {
	HL_పట్టిక()[idx].sg_cterm &= ~HL_BOLD;
	HL_పట్టిక()[idx].sg_cterm_bold = FALSE;
    }

    ఒకవేళ (VIM_ISDIGIT(*arg))
	color = atoi((అక్షరం *)arg);
    లేకపోతే ఒకవేళ (STRICMP(arg, "fg") == 0)
    {
	ఒకవేళ (cterm_సాధారణ_fg_color)
	    color = cterm_సాధారణ_fg_color - 1;
	లేకపోతే
	{
	    emsg(_(e_fg_color_unknown));
	    తిరిగి FALSE;
	}
    }
    లేకపోతే ఒకవేళ (STRICMP(arg, "bg") == 0)
    {
	ఒకవేళ (cterm_సాధారణ_bg_color > 0)
	    color = cterm_సాధారణ_bg_color - 1;
	లేకపోతే
	{
	    emsg(_(e_bg_color_unknown));
	    తిరిగి FALSE;
	}
    }
    లేకపోతే ఒకవేళ (STRICMP(arg, "ul") == 0)
    {
	ఒకవేళ (cterm_సాధారణ_ul_color > 0)
	    color = cterm_సాధారణ_ul_color - 1;
	లేకపోతే
	{
	    emsg(_(e_ul_color_unknown));
	    తిరిగి FALSE;
	}
    }
    లేకపోతే
    {
	పూర్ణాంకం bold = MAYBE;
	keyvalue_T లక్ష్యం;
	keyvalue_T *నమోదు_2;

	లక్ష్యం.కీలకం = 0;
	లక్ష్యం.విలువ.string = arg;
	లక్ష్యం.విలువ.నిడివి = 0;	// not used, see cmp_keyvalue_value_i()
	నమోదు_2 = (keyvalue_T *)bsearch(&లక్ష్యం, &color_name_tab,
		ARRAY_నిడివి(color_name_tab), పరిమాణం(color_name_tab[0]),
		cmp_keyvalue_విలువ_i);
	ఒకవేళ (నమోదు_2 == ఖాళీ)
	{
	    semsg(_(e_color_name_or_సంఖ్య_not_recognized_str), కీలకం_ప్రారంభం);
	    తిరిగి FALSE;
	}

	color = lookup_color(నమోదు_2->కీలకం, కీలకం[5] == 'F', &bold);

	// set/reset bold attribute to get light foreground
	// colors (on some terminals, e.g. "linux")
	ఒకవేళ (bold == TRUE)
	{
	    HL_పట్టిక()[idx].sg_cterm |= HL_BOLD;
	    HL_పట్టిక()[idx].sg_cterm_bold = TRUE;
	}
	లేకపోతే ఒకవేళ (bold == FALSE)
	    HL_పట్టిక()[idx].sg_cterm &= ~HL_BOLD;
    }

    // Add one to the argument, to avoid zero.  Zero is used for
    // "NONE", then "color" is -1.
    ఒకవేళ (కీలకం[5] == 'F')
	highlight_సమితి_ctermfg(idx, color, is_సాధారణ_సమూహం);
    లేకపోతే ఒకవేళ (కీలకం[5] == 'B')
	highlight_సమితి_ctermbg(idx, color, is_సాధారణ_సమూహం);
    లేకపోతే // ctermul
	highlight_సమితి_ctermul(idx, color, is_సాధారణ_సమూహం);

    తిరిగి TRUE;
}

#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_EVAL)
/*
 * Set the GUI foreground color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    స్థిర పూర్ణాంకం
highlight_సమితి_guifg(
	పూర్ణాంకం	idx,
	char_u	*arg,
	పూర్ణాంకం	is_menu_సమూహం వాడని,
	పూర్ణాంకం	is_scrollbar_సమూహం వాడని,
	పూర్ణాంకం	is_tooltip_సమూహం వాడని,
	పూర్ణాంకం	is_titlebar_సమూహం వాడని,
	పూర్ణాంకం	is_titlebarnc_సమూహం వాడని,
	పూర్ణాంకం	*do_colors వాడని,
	పూర్ణాంకం	init)
{
# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    దీర్ఘ	i;
# endif
    char_u	**namep;
    పూర్ణాంకం		did_change = FALSE;

    ఒకవేళ (init && (HL_పట్టిక()[idx].sg_సమితి & SG_GUI))
	తిరిగి FALSE;

    namep = &HL_పట్టిక()[idx].sg_gui_fg_name;
    ఒకవేళ (!init)
	HL_పట్టిక()[idx].sg_సమితి |= SG_GUI;

# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guifg colors are only used when recognized
    i = color_name2handle(arg);
    ఒకవేళ (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24_బిట్టు)
    {
	HL_పట్టిక()[idx].sg_gui_fg = i;
# endif
	ఒకవేళ (*namep == ఖాళీ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ఒకవేళ (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    లేకపోతే
		*namep = ఖాళీ;
	    did_change = TRUE;
	}
# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	ఒకవేళ (is_titlebar_సమూహం && gui.title_fg_pixel != i)
	{
	    gui.title_fg_pixel = i;
	    *do_colors = TRUE;
	}
	ఒకవేళ (is_titlebarnc_సమూహం && gui.titlenc_fg_pixel != i)
	{
	    gui.titlenc_fg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	ఒకవేళ (is_menu_సమూహం && gui.menu_fg_pixel != i)
	{
	    gui.menu_fg_pixel = i;
	    *do_colors = TRUE;
	}
	ఒకవేళ (is_scrollbar_సమూహం && gui.scroll_fg_pixel != i)
	{
	    gui.scroll_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	ఒకవేళ (is_tooltip_సమూహం && gui.tooltip_fg_pixel != i)
	{
	    gui.tooltip_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    తిరిగి did_change;
}

/*
 * Set the GUI background color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    స్థిర పూర్ణాంకం
highlight_సమితి_guibg(
	పూర్ణాంకం	idx,
	char_u	*arg,
	పూర్ణాంకం	is_menu_సమూహం వాడని,
	పూర్ణాంకం	is_scrollbar_సమూహం వాడని,
	పూర్ణాంకం	is_tooltip_సమూహం వాడని,
	పూర్ణాంకం	is_titlebar_సమూహం వాడని,
	పూర్ణాంకం	is_titlebarnc_సమూహం వాడని,
	పూర్ణాంకం	*do_colors వాడని,
	పూర్ణాంకం	init)
{
# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    పూర్ణాంకం		i;
# endif
    char_u	**namep;
    పూర్ణాంకం		did_change = FALSE;

    ఒకవేళ (init && (HL_పట్టిక()[idx].sg_సమితి & SG_GUI))
	తిరిగి FALSE;

    namep = &HL_పట్టిక()[idx].sg_gui_bg_name;
    ఒకవేళ (!init)
	HL_పట్టిక()[idx].sg_సమితి |= SG_GUI;

# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guibg colors are only used when recognized
    i = color_name2handle(arg);
    ఒకవేళ (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24_బిట్టు)
    {
	HL_పట్టిక()[idx].sg_gui_bg = i;
# endif
	ఒకవేళ (*namep == ఖాళీ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ఒకవేళ (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    లేకపోతే
		*namep = ఖాళీ;
	    did_change = TRUE;
	}
# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	ఒకవేళ (is_titlebar_సమూహం && gui.title_bg_pixel != i)
	{
	    gui.title_bg_pixel = i;
	    *do_colors = TRUE;
	}
	ఒకవేళ (is_titlebarnc_సమూహం && gui.titlenc_bg_pixel != i)
	{
	    gui.titlenc_bg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	ఒకవేళ (is_menu_సమూహం && gui.menu_bg_pixel != i)
	{
	    gui.menu_bg_pixel = i;
	    *do_colors = TRUE;
	}
	ఒకవేళ (is_scrollbar_సమూహం && gui.scroll_bg_pixel != i)
	{
	    gui.scroll_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	ఒకవేళ (is_tooltip_సమూహం && gui.tooltip_bg_pixel != i)
	{
	    gui.tooltip_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    తిరిగి did_change;
}

/*
 * Set the GUI undercurl/strikethrough color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    స్థిర పూర్ణాంకం
highlight_సమితి_guisp(పూర్ణాంకం idx, char_u *arg, పూర్ణాంకం init)
{
# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    పూర్ణాంకం		i;
# endif
    పూర్ణాంకం		did_change = FALSE;
    char_u	**namep;

    ఒకవేళ (init && (HL_పట్టిక()[idx].sg_సమితి & SG_GUI))
	తిరిగి FALSE;

    namep = &HL_పట్టిక()[idx].sg_gui_sp_name;
    ఒకవేళ (!init)
	HL_పట్టిక()[idx].sg_సమితి |= SG_GUI;

# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guisp colors are only used when recognized
    i = color_name2handle(arg);
    ఒకవేళ (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24_బిట్టు)
    {
	HL_పట్టిక()[idx].sg_gui_sp = i;
# endif
	ఒకవేళ (*namep == ఖాళీ || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    ఒకవేళ (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    లేకపోతే
		*namep = ఖాళీ;
	    did_change = TRUE;
	}
# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    }
# endif

    తిరిగి did_change;
}
#అయితే_ముగింపు

/*
 * Set the start/stop terminal codes for a highlight group.
 * Returns TRUE if the terminal code is set.
 */
    స్థిర పూర్ణాంకం
highlight_సమితి_startstop_termcode(పూర్ణాంకం idx, char_u *కీలకం, char_u *arg, పూర్ణాంకం init)
{
    పూర్ణాంకం		off;
    char_u	buf[100];
    పూర్ణాంకం		len;
    char_u	*tname;
    char_u	*p;

    ఒకవేళ (!init)
	HL_పట్టిక()[idx].sg_సమితి |= SG_పదం;

    // The "start" and "stop"  arguments can be a literal escape
    // sequence, or a comma separated list of terminal codes.
    ఒకవేళ (STRNCMP(arg, "t_", 2) == 0)
    {
	off = 0;
	buf[0] = 0;
	అయితే (arg[off] != NUL)
	{
	    // Isolate one termcap name
	    కొరకు (len = 0; arg[off + len] &&
		    arg[off + len] != ','; ++len)
		;
	    tname = vim_strnsave(arg + off, len);
	    ఒకవేళ (tname == ఖాళీ)		// out of memory
		తిరిగి FALSE;
	    // lookup the escape sequence for the item
	    p = get_పదం_సంకేతం(tname);
	    vim_free(tname);
	    ఒకవేళ (p == ఖాళీ)	    // ignore non-existing things
		p = (char_u *)"";

	    // Append it to the already found stuff
	    ఒకవేళ ((పూర్ణాంకం)(STRLEN(buf) + STRLEN(p)) >= 99)
	    {
		semsg(_(e_terminal_సంకేతం_too_long_str), arg);
		తిరిగి FALSE;
	    }
	    STRCAT(buf, p);

	    // Advance to the next item
	    off += len;
	    ఒకవేళ (arg[off] == ',')	    // another one follows
		++off;
	}
    }
    లేకపోతే
    {
	// Copy characters from arg[] to buf[], translating <> codes.
	కొరకు (p = arg, off = 0; off < 100 - 6 && *p; )
	{
	    len = trans_special(&p, buf + off, FSK_SIMPLIFY, FALSE, ఖాళీ);
	    ఒకవేళ (len > 0)	    // recognized special char
		off += len;
	    లేకపోతే		    // copy as normal char
		buf[off++] = *p++;
	}
	buf[off] = NUL;
    }

    ఒకవేళ (STRCMP(buf, "NONE") == 0)	// resetting the value
	p = ఖాళీ;
    లేకపోతే
	p = vim_strsave(buf);
    ఒకవేళ (కీలకం[2] == 'A')
    {
	vim_free(HL_పట్టిక()[idx].sg_ప్రారంభం);
	HL_పట్టిక()[idx].sg_ప్రారంభం = p;
    }
    లేకపోతే
    {
	vim_free(HL_పట్టిక()[idx].sg_stop);
	HL_పట్టిక()[idx].sg_stop = p;
    }
    తిరిగి TRUE;
}

/*
 * Handle the ":highlight .." command.
 * When using ":hi clear" this is called recursively for each group with
 * "forceit" and "init" both TRUE.
 */
    శూన్య
do_highlight(
    char_u	*పంక్తి,
    పూర్ణాంకం		forceit,
    పూర్ణాంకం		init)	    // TRUE when called for initializing
{
    char_u	*name_చివర;
    char_u	*linep;
    char_u	*కీలకం_ప్రారంభం;
    char_u	*arg_ప్రారంభం;
    char_u	*కీలకం = ఖాళీ, *arg = ఖాళీ;
    దీర్ఘ	i;
    పూర్ణాంకం		id;
    పూర్ణాంకం		idx;
    hl_సమూహం_T	అంశం_before;
    పూర్ణాంకం		did_change = FALSE;
    పూర్ణాంకం		dodefault = FALSE;
    పూర్ణాంకం		doclear = FALSE;
    పూర్ణాంకం		dolink = FALSE;
    పూర్ణాంకం		error = FALSE;
    పూర్ణాంకం		is_సాధారణ_సమూహం = FALSE;	// "Normal" group
#నిర్వచితమైతే FEAT_GUI_MSWIN
    పూర్ణాంకం		is_titlebar_సమూహం = FALSE;	// "TitleBar" group
    పూర్ణాంకం		is_titlebarnc_సమూహం = FALSE;	// "TitleBarNC" group
#లేకపోతే
# define is_titlebar_సమూహం 0
# define is_titlebarnc_సమూహం 0
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_X11
    పూర్ణాంకం		is_menu_సమూహం = FALSE;		// "Menu" group
    పూర్ణాంకం		is_scrollbar_సమూహం = FALSE;	// "Scrollbar" group
    పూర్ణాంకం		is_tooltip_సమూహం = FALSE;	// "Tooltip" group
#లేకపోతే
# define is_menu_సమూహం 0
# define is_tooltip_సమూహం 0
# define is_scrollbar_సమూహం 0
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_EVAL)
    పూర్ణాంకం		do_colors = FALSE;		// need to update colors?
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    పూర్ణాంకం		did_highlight_మారింది = FALSE;
#అయితే_ముగింపు

    // If no argument, list current highlighting.
    ఒకవేళ (!init && చివర్లు_excmd2(పంక్తి - 1, పంక్తి))
    {
	కొరకు (i = 1; i <= highlight_ga.ga_len && !got_int; ++i)
	    // TODO: only call when the group has attributes set
	    highlight_list_one((పూర్ణాంకం)i);
	తిరిగి;
    }

    // Isolate the name.
    name_చివర = skiptowhite(పంక్తి);
    linep = skipwhite(name_చివర);

    // Check for "default" argument.
    ఒకవేళ (STRNCMP(పంక్తి, "default", name_చివర - పంక్తి) == 0)
    {
	dodefault = TRUE;
	పంక్తి = linep;
	name_చివర = skiptowhite(పంక్తి);
	linep = skipwhite(name_చివర);
    }

    // Check for "clear" or "link" argument.
    ఒకవేళ (STRNCMP(పంక్తి, "clear", name_చివర - పంక్తి) == 0)
	doclear = TRUE;
    ఒకవేళ (STRNCMP(పంక్తి, "link", name_చివర - పంక్తి) == 0)
	dolink = TRUE;

    // ":highlight {group-name}": list highlighting for one group.
    ఒకవేళ (!doclear && !dolink && చివర్లు_excmd2(పంక్తి, linep))
    {
	id = syn_namen2id(పంక్తి, (పూర్ణాంకం)(name_చివర - పంక్తి));
	ఒకవేళ (id == 0)
	    semsg(_(e_highlight_సమూహం_name_not_దొరికింది_str), పంక్తి);
	లేకపోతే
	    highlight_list_one(id);
	తిరిగి;
    }

    // Handle ":highlight link {from} {to}" command.
    ఒకవేళ (dolink)
    {
	char_u	    *from_ప్రారంభం = linep;
	char_u	    *from_చివర;
	పూర్ణాంకం	    from_len;
	char_u	    *గమ్యం_ప్రారంభం;
	char_u	    *గమ్యం_చివర;
	పూర్ణాంకం	    గమ్యం_len;

	from_చివర = skiptowhite(from_ప్రారంభం);
	గమ్యం_ప్రారంభం = skipwhite(from_చివర);
	గమ్యం_చివర	 = skiptowhite(గమ్యం_ప్రారంభం);

	ఒకవేళ (చివర్లు_excmd2(పంక్తి, from_ప్రారంభం) || చివర్లు_excmd2(పంక్తి, గమ్యం_ప్రారంభం))
	{
	    semsg(_(e_not_enough_arguments_highlight_link_str), from_ప్రారంభం);
	    తిరిగి;
	}

	ఒకవేళ (!చివర్లు_excmd2(పంక్తి, skipwhite(గమ్యం_చివర)))
	{
	    semsg(_(e_too_many_arguments_highlight_link_str), from_ప్రారంభం);
	    తిరిగి;
	}

	from_len = (పూర్ణాంకం)(from_చివర - from_ప్రారంభం);
	గమ్యం_len = (పూర్ణాంకం)(గమ్యం_చివర - గమ్యం_ప్రారంభం);
	highlight_సమూహం_link(from_ప్రారంభం, from_len, గమ్యం_ప్రారంభం, గమ్యం_len,
						dodefault, forceit, init);
	తిరిగి;
    }

    ఒకవేళ (doclear)
    {
	// ":highlight clear [group]" command.
	ఒకవేళ (చివర్లు_excmd2(పంక్తి, linep))
	{
	    // ":highlight clear" without group name
	    highlight_reset_all();
	    తిరిగి;
	}
	పంక్తి = linep;
	name_చివర = skiptowhite(పంక్తి);
	linep = skipwhite(name_చివర);
    }

    // Find the group name in the table.  If it does not exist yet, add it.
    id = syn_check_సమూహం(పంక్తి, (పూర్ణాంకం)(name_చివర - పంక్తి));
    ఒకవేళ (id == 0)	// failed (out of memory)
	తిరిగి;
    idx = id - 1;	// index is ID minus one

    // Return if "default" was used and the group already has settings.
    ఒకవేళ (dodefault && hl_has_settings(idx, TRUE))
	తిరిగి;

    // Make a copy so we can check if any attribute actually changed.
    అంశం_before = HL_పట్టిక()[idx];

    ఒకవేళ (STRCMP(HL_పట్టిక()[idx].sg_name_u, "NORMAL") == 0)
	is_సాధారణ_సమూహం = TRUE;
#నిర్వచితమైతే FEAT_GUI_MSWIN
    లేకపోతే ఒకవేళ (STRCMP(HL_పట్టిక()[idx].sg_name_u, "TITLEBAR") == 0)
	is_titlebar_సమూహం = TRUE;
    లేకపోతే ఒకవేళ (STRCMP(HL_పట్టిక()[idx].sg_name_u, "TITLEBARNC") == 0)
	is_titlebarnc_సమూహం = TRUE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_X11
    లేకపోతే ఒకవేళ (STRCMP(HL_పట్టిక()[idx].sg_name_u, "MENU") == 0)
	is_menu_సమూహం = TRUE;
    లేకపోతే ఒకవేళ (STRCMP(HL_పట్టిక()[idx].sg_name_u, "SCROLLBAR") == 0)
	is_scrollbar_సమూహం = TRUE;
    లేకపోతే ఒకవేళ (STRCMP(HL_పట్టిక()[idx].sg_name_u, "TOOLTIP") == 0)
	is_tooltip_సమూహం = TRUE;
#అయితే_ముగింపు

    // Clear the highlighting for ":hi clear {group}" and ":hi clear".
    ఒకవేళ (doclear || (forceit && init))
    {
	highlight_clear(idx);
	ఒకవేళ (!doclear)
	    HL_పట్టిక()[idx].sg_సమితి = 0;
    }

    ఒకవేళ (!doclear)
	అయితే (!చివర్లు_excmd2(పంక్తి, linep))
	{
	    కీలకం_ప్రారంభం = linep;
	    ఒకవేళ (*linep == '=')
	    {
		semsg(_(e_unexpected_equal_sign_str), కీలకం_ప్రారంభం);
		error = TRUE;
		ఆపు;
	    }

	    // Note: Keep this in sync with get_highlight_group_key.

	    // Isolate the key ("term", "ctermfg", "ctermbg", "font", "guifg"
	    // or "guibg").
	    అయితే (*linep && !VIM_ISWHITE(*linep) && *linep != '=')
		++linep;
	    vim_free(కీలకం);
	    కీలకం = vim_strnsave_పైకి(కీలకం_ప్రారంభం, linep - కీలకం_ప్రారంభం);
	    ఒకవేళ (కీలకం == ఖాళీ)
	    {
		error = TRUE;
		ఆపు;
	    }
	    linep = skipwhite(linep);

	    ఒకవేళ (STRCMP(కీలకం, "NONE") == 0)
	    {
		ఒకవేళ (!init || HL_పట్టిక()[idx].sg_సమితి == 0)
		{
		    ఒకవేళ (!init)
			HL_పట్టిక()[idx].sg_సమితి |= SG_పదం+SG_CTERM+SG_GUI;
		    highlight_clear(idx);
		}
		కొనసాగు;
	    }

	    // Check for the equal sign.
	    ఒకవేళ (*linep != '=')
	    {
		semsg(_(e_missing_equal_sign_str_2), కీలకం_ప్రారంభం);
		error = TRUE;
		ఆపు;
	    }
	    ++linep;

	    // Isolate the argument.
	    linep = skipwhite(linep);
	    ఒకవేళ (*linep == '\'')		// guifg='color name'
	    {
		arg_ప్రారంభం = ++linep;
		linep = vim_strchr(linep, '\'');
		ఒకవేళ (linep == ఖాళీ)
		{
		    semsg(_(e_invalid_వాదం_str), కీలకం_ప్రారంభం);
		    error = TRUE;
		    ఆపు;
		}
	    }
	    లేకపోతే
	    {
		arg_ప్రారంభం = linep;
		linep = skiptowhite(linep);
	    }
	    ఒకవేళ (linep == arg_ప్రారంభం)
	    {
		semsg(_(e_missing_వాదం_str), కీలకం_ప్రారంభం);
		error = TRUE;
		ఆపు;
	    }
	    vim_free(arg);
	    arg = vim_strnsave(arg_ప్రారంభం, linep - arg_ప్రారంభం);
	    ఒకవేళ (arg == ఖాళీ)
	    {
		error = TRUE;
		ఆపు;
	    }
	    ఒకవేళ (*linep == '\'')
		++linep;

	    // Store the argument.
	    ఒకవేళ (STRCMP(కీలకం, "TERM") == 0
		    || STRCMP(కీలకం, "CTERM") == 0
		    || STRCMP(కీలకం, "GUI") == 0)
	    {
		ఒకవేళ (!highlight_సమితి_termgui_attr(idx, కీలకం, arg, init))
		{
		    error = TRUE;
		    ఆపు;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(కీలకం, "FONT") == 0)
	    {
		// in non-GUI fonts are simply ignored
#నిర్వచితమైతే FEAT_GUI
		ఒకవేళ (highlight_సమితి_font(idx, arg, is_సాధారణ_సమూహం,
					      is_menu_సమూహం, is_tooltip_సమూహం))
		    did_change = TRUE;
#అయితే_ముగింపు
	    }
	    లేకపోతే ఒకవేళ (STRCMP(కీలకం, "CTERMFG") == 0
		    || STRCMP(కీలకం, "CTERMBG") == 0
		    || STRCMP(కీలకం, "CTERMUL") == 0)
	    {
		ఒకవేళ (!highlight_సమితి_cterm_color(idx, కీలకం, కీలకం_ప్రారంభం, arg,
							is_సాధారణ_సమూహం, init))
		{
		    error = TRUE;
		    ఆపు;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(కీలకం, "CTERMFONT") == 0)
	    {
		ఒకవేళ (!highlight_సమితి_cterm_font(idx, arg, init))
		{
		    error = TRUE;
		    ఆపు;
		}
	    }
	    లేకపోతే ఒకవేళ (STRCMP(కీలకం, "GUIFG") == 0)
	    {
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_EVAL)
		ఒకవేళ (highlight_సమితి_guifg(idx, arg, is_menu_సమూహం,
			    is_scrollbar_సమూహం, is_tooltip_సమూహం,
			    is_titlebar_సమూహం, is_titlebarnc_సమూహం,
			    &do_colors, init))
		    did_change = TRUE;
#అయితే_ముగింపు
	    }
	    లేకపోతే ఒకవేళ (STRCMP(కీలకం, "GUIBG") == 0)
	    {
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_EVAL)
		ఒకవేళ (highlight_సమితి_guibg(idx, arg, is_menu_సమూహం,
			    is_scrollbar_సమూహం, is_tooltip_సమూహం,
			    is_titlebar_సమూహం, is_titlebarnc_సమూహం,
			    &do_colors, init))
		    did_change = TRUE;
#అయితే_ముగింపు
	    }
	    లేకపోతే ఒకవేళ (STRCMP(కీలకం, "GUISP") == 0)
	    {
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_EVAL)
		ఒకవేళ (highlight_సమితి_guisp(idx, arg, init))
		    did_change = TRUE;
#అయితే_ముగింపు
	    }
	    లేకపోతే ఒకవేళ (STRCMP(కీలకం, "START") == 0 || STRCMP(కీలకం, "STOP") == 0)
	    {
		ఒకవేళ (!highlight_సమితి_startstop_termcode(idx, కీలకం, arg, init))
		{
		    error = TRUE;
		    ఆపు;
		}
	    }
	    లేకపోతే
	    {
		semsg(_(e_illegal_వాదం_str_3), కీలకం_ప్రారంభం);
		error = TRUE;
		ఆపు;
	    }
	    HL_పట్టిక()[idx].sg_cleared = FALSE;

	    // When highlighting has been given for a group, don't link it.
	    ఒకవేళ (!init || !(HL_పట్టిక()[idx].sg_సమితి & SG_LINK))
		HL_పట్టిక()[idx].sg_link = 0;

	    // Continue with next argument.
	    linep = skipwhite(linep);
	}

    // If there is an error, and it's a new entry, remove it from the table.
    ఒకవేళ (error && idx == highlight_ga.ga_len)
	syn_unadd_సమూహం();
    లేకపోతే
    {
	ఒకవేళ (is_సాధారణ_సమూహం)
	{
	    HL_పట్టిక()[idx].sg_పదం_attr = 0;
	    HL_పట్టిక()[idx].sg_cterm_attr = 0;
#నిర్వచితమైతే FEAT_GUI
	    HL_పట్టిక()[idx].sg_gui_attr = 0;
	    // Need to update all groups, because they might be using "bg"
	    // and/or "fg", which have been changed now.
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    ఒకవేళ (USE_24_బిట్టు)
	    {
		highlight_gui_started();
		did_highlight_మారింది = TRUE;
		redraw_all_later(UPD_NOT_VALID);
	    }
#అయితే_ముగింపు
	    resolve_fallback_fg_గమ్యం_rgb();
	    resolve_fallback_bg_గమ్యం_rgb();
#నిర్వచితమైతే FEAT_VTP
	    control_console_color_rgb();
#అయితే_ముగింపు
	}
#నిర్వచితమైతే FEAT_GUI_MSWIN
	లేకపోతే ఒకవేళ (is_titlebar_సమూహం)
	{
	    ఒకవేళ (gui.లోపల_use && do_colors)
		gui_mch_సమితి_titlebar_colors();
	}
	లేకపోతే ఒకవేళ (is_titlebarnc_సమూహం)
	{
	    ఒకవేళ (gui.లోపల_use && do_colors)
		gui_mch_సమితి_titlebar_colors();
	}
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_X11
# ifdef FEAT_MENU
	లేకపోతే ఒకవేళ (is_menu_సమూహం)
	{
	    ఒకవేళ (gui.లోపల_use && do_colors)
		gui_mch_new_menu_colors();
	}
# endif
	లేకపోతే ఒకవేళ (is_scrollbar_సమూహం)
	{
	    ఒకవేళ (gui.లోపల_use && do_colors)
		gui_new_scrollbar_colors();
	    లేకపోతే
		సమితి_hl_attr(idx);
	}
# ifdef FEAT_BEVAL_GUI
	లేకపోతే ఒకవేళ (is_tooltip_సమూహం)
	{
	    ఒకవేళ (gui.లోపల_use && do_colors)
		gui_mch_new_tooltip_colors();
	}
# endif
#అయితే_ముగింపు
	లేకపోతే
	    సమితి_hl_attr(idx);
#నిర్వచితమైతే FEAT_EVAL
	HL_పట్టిక()[idx].sg_script_ctx = current_sctx;
	HL_పట్టిక()[idx].sg_script_ctx.sc_lnum += SOURCING_LNUM;
#అయితే_ముగింపు
    }

    vim_free(కీలకం);
    vim_free(arg);

    // Only call highlight_changed() once, after a sequence of highlight
    // commands, and only if an attribute actually changed.
    ఒకవేళ ((did_change
	   || memcmp(&HL_పట్టిక()[idx], &అంశం_before, పరిమాణం(అంశం_before)) != 0)
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    && !did_highlight_మారింది
#అయితే_ముగింపు
       )
    {
	// Do not trigger a redraw when highlighting is changed while
	// redrawing.  This may happen when evaluating 'statusline' changes the
	// StatusLine group.
	ఒకవేళ (!updating_screen)
	    redraw_all_later(UPD_NOT_VALID);
	need_highlight_మారింది = TRUE;
    }
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_highlight(శూన్య)
{
    పూర్ణాంకం	    i;

    కొరకు (i = 0; i < highlight_ga.ga_len; ++i)
    {
	highlight_clear(i);
	vim_free(HL_పట్టిక()[i].sg_name);
	vim_free(HL_పట్టిక()[i].sg_name_u - HLNAME_కీలకం_OFF);
    }
    ga_clear(&highlight_ga);
    hash_clear(&highlight_ht);
    highlight_ht_inited = false;
}
#అయితే_ముగింపు

/*
 * Reset the cterm colors to what they were before Vim was started, if
 * possible.  Otherwise reset them to zero.
 */
    శూన్య
restore_cterm_colors(శూన్య)
{
#ఒకవేళ defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
    // Since t_me has been set, this probably means that the user
    // wants to use this as default colors.  Need to reset default
    // background/foreground colors.
    mch_సమితి_సాధారణ_colors();
#లేకపోతే
# ifdef VIMDLL
    ఒకవేళ (!gui.లోపల_use)
    {
	mch_సమితి_సాధారణ_colors();
	తిరిగి;
    }
# endif
    cterm_సాధారణ_fg_color = 0;
    cterm_సాధారణ_fg_bold = 0;
    cterm_సాధారణ_bg_color = 0;
# ifdef FEAT_TERMGUICOLORS
    cterm_సాధారణ_fg_gui_color = INVALCOLOR;
    cterm_సాధారణ_bg_gui_color = INVALCOLOR;
    cterm_సాధారణ_ul_gui_color = INVALCOLOR;
# endif
    fallback_fg_rgb = INVALCOLOR;
    fallback_bg_rgb = INVALCOLOR;
#అయితే_ముగింపు
}

/*
 * Return TRUE if highlight group "idx" has any settings.
 * When "check_link" is TRUE also check for an existing link.
 */
    స్థిర పూర్ణాంకం
hl_has_settings(పూర్ణాంకం idx, పూర్ణాంకం check_link)
{
    తిరిగి HL_పట్టిక()[idx].sg_cleared == 0
	 && (  HL_పట్టిక()[idx].sg_పదం_attr != 0
	    || HL_పట్టిక()[idx].sg_cterm_attr != 0
	    || HL_పట్టిక()[idx].sg_cterm_fg != 0
	    || HL_పట్టిక()[idx].sg_cterm_bg != 0
	    || HL_పట్టిక()[idx].sg_cterm_font != 0
#నిర్వచితమైతే FEAT_GUI
	    || HL_పట్టిక()[idx].sg_gui_attr != 0
	    || HL_పట్టిక()[idx].sg_gui_fg_name != ఖాళీ
	    || HL_పట్టిక()[idx].sg_gui_bg_name != ఖాళీ
	    || HL_పట్టిక()[idx].sg_gui_sp_name != ఖాళీ
	    || HL_పట్టిక()[idx].sg_font_name != ఖాళీ
#అయితే_ముగింపు
	    || (check_link && (HL_పట్టిక()[idx].sg_సమితి & SG_LINK)));
}

/*
 * Clear highlighting for one group.
 */
    స్థిర శూన్య
highlight_clear(పూర్ణాంకం idx)
{
    HL_పట్టిక()[idx].sg_cleared = TRUE;

    HL_పట్టిక()[idx].sg_పదం = 0;
    VIM_CLEAR(HL_పట్టిక()[idx].sg_ప్రారంభం);
    VIM_CLEAR(HL_పట్టిక()[idx].sg_stop);
    HL_పట్టిక()[idx].sg_పదం_attr = 0;
    HL_పట్టిక()[idx].sg_cterm = 0;
    HL_పట్టిక()[idx].sg_cterm_bold = FALSE;
    HL_పట్టిక()[idx].sg_cterm_fg = 0;
    HL_పట్టిక()[idx].sg_cterm_bg = 0;
    HL_పట్టిక()[idx].sg_cterm_attr = 0;
    HL_పట్టిక()[idx].sg_cterm_font = 0;
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_EVAL)
    HL_పట్టిక()[idx].sg_gui = 0;
    VIM_CLEAR(HL_పట్టిక()[idx].sg_gui_fg_name);
    VIM_CLEAR(HL_పట్టిక()[idx].sg_gui_bg_name);
    VIM_CLEAR(HL_పట్టిక()[idx].sg_gui_sp_name);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_పట్టిక()[idx].sg_gui_fg = INVALCOLOR;
    HL_పట్టిక()[idx].sg_gui_bg = INVALCOLOR;
    HL_పట్టిక()[idx].sg_gui_sp = INVALCOLOR;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
    gui_mch_free_font(HL_పట్టిక()[idx].sg_font);
    HL_పట్టిక()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
    gui_mch_free_fontset(HL_పట్టిక()[idx].sg_fontset);
    HL_పట్టిక()[idx].sg_fontset = NOFONTSET;
# endif
    VIM_CLEAR(HL_పట్టిక()[idx].sg_font_name);
    HL_పట్టిక()[idx].sg_gui_attr = 0;
#అయితే_ముగింపు
    // Restore default link and context if they exist. Otherwise clears.
    HL_పట్టిక()[idx].sg_link = HL_పట్టిక()[idx].sg_deflink;
#నిర్వచితమైతే FEAT_EVAL
    // Since we set the default link, set the location to where the default
    // link was set.
    HL_పట్టిక()[idx].sg_script_ctx = HL_పట్టిక()[idx].sg_deflink_sctx;
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Set the normal foreground and background colors according to the "Normal"
 * highlighting group.  For X11 also set "Menu", "Scrollbar" and "Tooltip"
 * colors.  For MS-Windows also set "TitleBar" and "TitleBarNC" colors.
 */
    శూన్య
సమితి_సాధారణ_colors(శూన్య)
{
# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    ఒకవేళ (gui.లోపల_use)
#  endif
    {
	ఒకవేళ (సమితి_సమూహం_colors((char_u *)"Normal",
				 &gui.norm_pixel, &gui.వెనుక_pixel,
				 FALSE, TRUE, FALSE))
	{
	    gui_mch_new_colors();
	    సమితి_must_redraw(UPD_CLEAR);
	}
#  ifdef FEAT_GUI_MSWIN
	ఒకవేళ (సమితి_సమూహం_colors((char_u *)"TitleBar",
				 &gui.title_fg_pixel, &gui.title_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_సమితి_titlebar_colors();
	    సమితి_must_redraw(UPD_CLEAR);
	}
	ఒకవేళ (సమితి_సమూహం_colors((char_u *)"TitleBarNC",
				 &gui.titlenc_fg_pixel, &gui.titlenc_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_సమితి_titlebar_colors();
	    సమితి_must_redraw(UPD_CLEAR);
	}
#  endif
#  ifdef FEAT_GUI_X11
	ఒకవేళ (సమితి_సమూహం_colors((char_u *)"Menu",
			     &gui.menu_fg_pixel, &gui.menu_bg_pixel,
			     TRUE, FALSE, FALSE))
	{
#   ifdef FEAT_MENU
	    gui_mch_new_menu_colors();
#   endif
	    సమితి_must_redraw(UPD_CLEAR);
	}
#   ifdef FEAT_BEVAL_GUI
	ఒకవేళ (సమితి_సమూహం_colors((char_u *)"Tooltip",
			     &gui.tooltip_fg_pixel, &gui.tooltip_bg_pixel,
			     FALSE, FALSE, TRUE))
	{
#    ifdef FEAT_TOOLBAR
	    gui_mch_new_tooltip_colors();
#    endif
	    సమితి_must_redraw(UPD_CLEAR);
	}
#   endif
	ఒకవేళ (సమితి_సమూహం_colors((char_u *)"Scrollbar",
			&gui.scroll_fg_pixel, &gui.scroll_bg_pixel,
			FALSE, FALSE, FALSE))
	{
	    gui_new_scrollbar_colors();
	    సమితి_must_redraw(UPD_CLEAR);
	}
#  endif
    }
# endif
# ifdef FEAT_TERMGUICOLORS
#  ifdef FEAT_GUI
    లేకపోతే
#  endif
    {
	పూర్ణాంకం		idx;

	idx = syn_name2id((char_u *)"Normal") - 1;
	ఒకవేళ (idx >= 0)
	{
	    gui_do_one_color(idx, FALSE, FALSE);

	    // If the normal fg or bg color changed a complete redraw is
	    // required.
	    ఒకవేళ (cterm_సాధారణ_fg_gui_color != HL_పట్టిక()[idx].sg_gui_fg
		    || cterm_సాధారణ_bg_gui_color != HL_పట్టిక()[idx].sg_gui_bg)
	    {
		// if the GUI color is INVALCOLOR then we use the default cterm
		// color
		cterm_సాధారణ_fg_gui_color = HL_పట్టిక()[idx].sg_gui_fg;
		cterm_సాధారణ_bg_gui_color = HL_పట్టిక()[idx].sg_gui_bg;
		సమితి_must_redraw(UPD_CLEAR);
	    }
	}
    }
# endif
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI)
/*
 * Set the colors for "Normal", "Menu", "TitleBar", "TitleBarNC", "Tooltip" or
 * "Scrollbar".
 */
    స్థిర పూర్ణాంకం
సమితి_సమూహం_colors(
    char_u	*name,
    guicolor_T	*fgp,
    guicolor_T	*bgp,
    పూర్ణాంకం		do_menu,
    పూర్ణాంకం		use_norm,
    పూర్ణాంకం		do_tooltip)
{
    పూర్ణాంకం		idx;

    idx = syn_name2id(name) - 1;
    ఒకవేళ (idx < 0)
	తిరిగి FALSE;

    gui_do_one_color(idx, do_menu, do_tooltip);

    ఒకవేళ (HL_పట్టిక()[idx].sg_gui_fg != INVALCOLOR)
	*fgp = HL_పట్టిక()[idx].sg_gui_fg;
    లేకపోతే ఒకవేళ (use_norm)
	*fgp = gui.def_norm_pixel;
    ఒకవేళ (HL_పట్టిక()[idx].sg_gui_bg != INVALCOLOR)
	*bgp = HL_పట్టిక()[idx].sg_gui_bg;
    లేకపోతే ఒకవేళ (use_norm)
	*bgp = gui.def_వెనుక_pixel;
    తిరిగి TRUE;
}

/*
 * Get the font of the "Normal" group.
 * Returns "" when it's not found or not set.
 */
    char_u *
hl_get_font_name(శూన్య)
{
    పూర్ణాంకం		id;
    char_u	*s;

    id = syn_name2id((char_u *)"Normal");
    ఒకవేళ (id > 0)
    {
	s = HL_పట్టిక()[id - 1].sg_font_name;
	ఒకవేళ (s != ఖాళీ)
	    తిరిగి s;
    }
    తిరిగి (char_u *)"";
}

/*
 * Set font for "Normal" group.  Called by gui_mch_init_font() when a font has
 * actually chosen to be used.
 */
    శూన్య
hl_సమితి_font_name(char_u *font_name)
{
    పూర్ణాంకం	    id;

    id = syn_name2id((char_u *)"Normal");
    ఒకవేళ (id <= 0)
	తిరిగి;

    vim_free(HL_పట్టిక()[id - 1].sg_font_name);
    HL_పట్టిక()[id - 1].sg_font_name = vim_strsave(font_name);
}

/*
 * Set background color for "Normal" group.  Called by gui_set_bg_color()
 * when the color is known.
 */
    శూన్య
hl_సమితి_bg_color_name(
    char_u  *name)	    // must have been allocated
{
    పూర్ణాంకం	    id;

    ఒకవేళ (name == ఖాళీ)
	తిరిగి;

    id = syn_name2id((char_u *)"Normal");
    ఒకవేళ (id <= 0)
	తిరిగి;

    vim_free(HL_పట్టిక()[id - 1].sg_gui_bg_name);
    HL_పట్టిక()[id - 1].sg_gui_bg_name = name;
}

/*
 * Set foreground color for "Normal" group.  Called by gui_set_fg_color()
 * when the color is known.
 */
    శూన్య
hl_సమితి_fg_color_name(
    char_u  *name)	    // must have been allocated
{
    పూర్ణాంకం	    id;

    ఒకవేళ (name == ఖాళీ)
	తిరిగి;

    id = syn_name2id((char_u *)"Normal");
    ఒకవేళ (id <= 0)
	తిరిగి;

    vim_free(HL_పట్టిక()[id - 1].sg_gui_fg_name);
    HL_పట్టిక()[id - 1].sg_gui_fg_name = name;
}

/*
 * Return the handle for a font name.
 * Returns NOFONT when failed.
 */
    స్థిర GuiFont
font_name2handle(char_u *name)
{
    ఒకవేళ (STRCMP(name, "NONE") == 0)
	తిరిగి NOFONT;

    తిరిగి gui_mch_get_font(name, TRUE);
}

# ifdef FEAT_XFONTSET
/*
 * Return the handle for a fontset name.
 * Returns NOFONTSET when failed.
 */
    స్థిర GuiFontset
fontset_name2handle(char_u *name, పూర్ణాంకం fixed_వెడల్పు)
{
    ఒకవేళ (STRCMP(name, "NONE") == 0)
	తిరిగి NOFONTSET;

    తిరిగి gui_mch_get_fontset(name, TRUE, fixed_వెడల్పు);
}
# endif

/*
 * Get the font or fontset for one highlight group.
 */
    స్థిర శూన్య
hl_do_font(
    పూర్ణాంకం		idx,
    char_u	*arg,
    పూర్ణాంకం		do_సాధారణ,		// set normal font
    పూర్ణాంకం		do_menu వాడని,		// set menu font
    పూర్ణాంకం		do_tooltip వాడని,	// set tooltip font
    పూర్ణాంకం		free_font)		// free current font/fontset
{
# ifdef FEAT_XFONTSET
    // If 'guifontset' is not empty, first try using the name as a
    // fontset.  If that doesn't work, use it as a font name.
    ఒకవేళ (*p_guifontset != NUL
#  ifdef FONTSET_ALWAYS
	|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
	// In Motif, the Tooltip highlight group is always a fontset
	|| do_tooltip
#  endif
	    )
    {
	ఒకవేళ (free_font)
	    gui_mch_free_fontset(HL_పట్టిక()[idx].sg_fontset);
	HL_పట్టిక()[idx].sg_fontset = fontset_name2handle(arg, 0
#  ifdef FONTSET_ALWAYS
		|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
		|| do_tooltip
#  endif
		);
    }
    ఒకవేళ (HL_పట్టిక()[idx].sg_fontset != NOFONTSET)
    {
	// If it worked and it's the Normal group, use it as the normal
	// fontset.  Same for the Menu group.
	ఒకవేళ (do_సాధారణ)
	    gui_init_font(arg, TRUE);
#  ఒకవేళ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	ఒకవేళ (do_menu)
	{
#   ifdef FONTSET_ALWAYS
	    gui.menu_fontset = HL_పట్టిక()[idx].sg_fontset;
#   లేకపోతే
	    // YIKES!  This is a bug waiting to crash the program
	    gui.menu_font = HL_పట్టిక()[idx].sg_fontset;
#   endif
	    gui_mch_new_menu_font();
	}
#   ifdef FEAT_BEVAL_GUI
	ఒకవేళ (do_tooltip)
	{
	    // The Athena widget set could not handle switching between
	    // displaying a single font and a fontset.
	    // If the XtNinternational resource is set to True at widget
	    // creation, then a fontset is always used, otherwise an
	    // XFontStruct is used.
	    gui.tooltip_fontset = (XFont_సమితి)HL_పట్టిక()[idx].sg_fontset;
	    gui_mch_new_tooltip_font();
	}
#   endif
#  endif
    }
    లేకపోతే
# endif
    {
	ఒకవేళ (free_font)
	    gui_mch_free_font(HL_పట్టిక()[idx].sg_font);
	HL_పట్టిక()[idx].sg_font = font_name2handle(arg);
	// If it worked and it's the Normal group, use it as the
	// normal font.  Same for the Menu group.
	ఒకవేళ (HL_పట్టిక()[idx].sg_font != NOFONT)
	{
	    ఒకవేళ (do_సాధారణ)
		gui_init_font(arg, FALSE);
# ifndef FONTSET_ALWAYS
#  ఒకవేళ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	    ఒకవేళ (do_menu)
	    {
		gui.menu_font = HL_పట్టిక()[idx].sg_font;
		gui_mch_new_menu_font();
	    }
#  endif
# endif
	}
    }
}

#అయితే_ముగింపు // FEAT_GUI

#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return the handle for a color name.
 * Returns INVALCOLOR when failed.
 */
    guicolor_T
color_name2handle(char_u *name)
{
    ఒకవేళ (STRCMP(name, "NONE") == 0)
	తిరిగి INVALCOLOR;

    ఒకవేళ (STRICMP(name, "fg") == 0 || STRICMP(name, "foreground") == 0)
    {
# ఒకవేళ defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	ఒకవేళ (gui.లోపల_use)
# endif
# ifdef FEAT_GUI
	    తిరిగి gui.norm_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	ఒకవేళ (cterm_సాధారణ_fg_gui_color != INVALCOLOR)
	    తిరిగి cterm_సాధారణ_fg_gui_color;
	// Guess that the foreground is black or white.
	తిరిగి GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "black" : "white"));
# endif
    }
    ఒకవేళ (STRICMP(name, "bg") == 0 || STRICMP(name, "background") == 0)
    {
# ఒకవేళ defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	ఒకవేళ (gui.లోపల_use)
# endif
# ifdef FEAT_GUI
	    తిరిగి gui.వెనుక_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	ఒకవేళ (cterm_సాధారణ_bg_gui_color != INVALCOLOR)
	    తిరిగి cterm_సాధారణ_bg_gui_color;
	// Guess that the background is white or black.
	తిరిగి GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "white" : "black"));
# endif
    }

    తిరిగి GUI_GET_COLOR(name);
}

// On MS-Windows an RGB macro is available and it produces 0x00bbggrr color
// values as used by the MS-Windows GDI api.  It should be used only for
// MS-Windows GDI builds.
# ఒకవేళ defined(RGB) && defined(MSWIN) && !defined(FEAT_GUI)
#  undef RGB
# endif
# ifndef RGB
#  define RGB(r, g, b)	(((r)<<16) | ((g)<<8) | (b))
# endif

# ifdef VIMDLL
    స్థిర guicolor_T
gui_adjust_rgb(guicolor_T c)
{
    ఒకవేళ (gui.లోపల_use)
	తిరిగి c;
    లేకపోతే
	తిరిగి ((c & 0xff) << 16) | (c & 0x00ff00) | ((c >> 16) & 0xff);
}
# లేకపోతే
#  define gui_adjust_rgb(c) (c)
# endif

    స్థిర పూర్ణాంకం
hex_digit(పూర్ణాంకం c)
{
    ఒకవేళ (సురక్షిత_isdigit(c))
	తిరిగి c - '0';
    c = TOLOWER_ASC(c);
    ఒకవేళ (c >= 'a' && c <= 'f')
	తిరిగి c - 'a' + 10;
    తిరిగి 0x1ffffff;
}

    స్థిర guicolor_T
decode_hex_color(char_u *hex)
{
    guicolor_T color;

    ఒకవేళ (hex[0] != '#' || STRLEN(hex) != 7)
	తిరిగి INVALCOLOR;

    // Name is in "#rrggbb" format
    color = RGB(((hex_digit(hex[1]) << 4) + hex_digit(hex[2])),
		((hex_digit(hex[3]) << 4) + hex_digit(hex[4])),
		((hex_digit(hex[5]) << 4) + hex_digit(hex[6])));
    ఒకవేళ (color > 0xffffff)
	తిరిగి INVALCOLOR;
    తిరిగి gui_adjust_rgb(color);
}

# ifdef FEAT_EVAL
// Returns the color currently mapped to the given name or INVALCOLOR if no
// such name exists in the color table. The convention is to use lowercase for
// all keys in the v:colornames dictionary. The value can be either a string in
// the form #rrggbb or a number, either of which is converted to a guicolor_T.
    స్థిర guicolor_T
colorname2rgb(char_u *name)
{
    dict_T      *colornames_పట్టిక = get_vim_var_dict(VV_COLORNAMES);
    char_u      *lc_name;
    dictitem_T  *colentry;
    char_u      *colstr;
    varnumber_T colnum;

    lc_name = strlow_save(name);
    ఒకవేళ (lc_name == ఖాళీ)
	తిరిగి INVALCOLOR;

    colentry = dict_కనుగొను(colornames_పట్టిక, lc_name, -1);
    vim_free(lc_name);
    ఒకవేళ (colentry == ఖాళీ)
	తిరిగి INVALCOLOR;

    ఒకవేళ (colentry->di_tv.v_type == VAR_STRING)
    {
	colstr = tv_get_string_strict(&colentry->di_tv);
	ఒకవేళ ((STRLEN(colstr) == 7) && (*colstr == '#'))
	{
	    తిరిగి decode_hex_color(colstr);
	}
	లేకపోతే
	{
	    semsg(_(e_bad_color_string_str), colstr);
	    తిరిగి INVALCOLOR;
	}
    }

    ఒకవేళ (colentry->di_tv.v_type == VAR_సంఖ్య)
    {
	colnum = tv_get_సంఖ్య(&colentry->di_tv);
	తిరిగి (guicolor_T)colnum;
    }

    తిరిగి INVALCOLOR;
}

# endif

    guicolor_T
gui_get_color_cmn(char_u *name)
{
    guicolor_T  color;
    // Only non X11 colors (not present in rgb.txt) and colors in
    // color_name_tab[], useful when $VIMRUNTIME is not found,.
    // must be sorted by the 'value' field because it is used by bsearch()!
    స్థిర keyvalue_T rgb_tab[] = {
	KEYVALUE_నమోదు(RGB(0x00, 0x00, 0x00), "black"),
	KEYVALUE_నమోదు(RGB(0x00, 0x00, 0xFF), "blue"),
	KEYVALUE_నమోదు(RGB(0xA5, 0x2A, 0x2A), "brown"),
	KEYVALUE_నమోదు(RGB(0x00, 0xFF, 0xFF), "cyan"),
	KEYVALUE_నమోదు(RGB(0x00, 0x00, 0x8B), "darkblue"),
	KEYVALUE_నమోదు(RGB(0x00, 0x8B, 0x8B), "darkcyan"),
	KEYVALUE_నమోదు(RGB(0xA9, 0xA9, 0xA9), "darkgray"),
	KEYVALUE_నమోదు(RGB(0x00, 0x64, 0x00), "darkgreen"),
	KEYVALUE_నమోదు(RGB(0xA9, 0xA9, 0xA9), "darkgrey"),
	KEYVALUE_నమోదు(RGB(0x8B, 0x00, 0x8B), "darkmagenta"),
	KEYVALUE_నమోదు(RGB(0x8B, 0x00, 0x00), "darkred"),
	KEYVALUE_నమోదు(RGB(0x8B, 0x8B, 0x00), "darkyellow"),	 // No X11
	KEYVALUE_నమోదు(RGB(0xBE, 0xBE, 0xBE), "gray"),
	KEYVALUE_నమోదు(RGB(0x00, 0xFF, 0x00), "green"),
	KEYVALUE_నమోదు(RGB(0xBE, 0xBE, 0xBE), "grey"),
	KEYVALUE_నమోదు(RGB(0x66, 0x66, 0x66), "grey40"),
	KEYVALUE_నమోదు(RGB(0x7F, 0x7F, 0x7F), "grey50"),
	KEYVALUE_నమోదు(RGB(0xE5, 0xE5, 0xE5), "grey90"),
	KEYVALUE_నమోదు(RGB(0xAD, 0xD8, 0xE6), "lightblue"),
	KEYVALUE_నమోదు(RGB(0xE0, 0xFF, 0xFF), "lightcyan"),
	KEYVALUE_నమోదు(RGB(0xD3, 0xD3, 0xD3), "lightgray"),
	KEYVALUE_నమోదు(RGB(0x90, 0xEE, 0x90), "lightgreen"),
	KEYVALUE_నమోదు(RGB(0xD3, 0xD3, 0xD3), "lightgrey"),
	KEYVALUE_నమోదు(RGB(0xFF, 0x8B, 0xFF), "lightmagenta"),     // No XX
	KEYVALUE_నమోదు(RGB(0xFF, 0x8B, 0x8B), "lightred"),	 // No XX
	KEYVALUE_నమోదు(RGB(0xFF, 0xFF, 0xE0), "lightyellow"),
	KEYVALUE_నమోదు(RGB(0xFF, 0x00, 0xFF), "magenta"),
	KEYVALUE_నమోదు(RGB(0xFF, 0x00, 0x00), "red"),
	KEYVALUE_నమోదు(RGB(0x2E, 0x8B, 0x57), "seagreen"),
	KEYVALUE_నమోదు(RGB(0xFF, 0xFF, 0xFF), "white"),
	KEYVALUE_నమోదు(RGB(0xFF, 0xFF, 0x00), "yellow")
    };
    keyvalue_T లక్ష్యం;
    keyvalue_T *నమోదు_2;

    color = decode_hex_color(name);
    ఒకవేళ (color != INVALCOLOR)
	తిరిగి color;

    లక్ష్యం.కీలకం = 0;
    లక్ష్యం.విలువ.string = name;
    లక్ష్యం.విలువ.నిడివి = 0;	// not used, see cmp_keyvalue_value_i()
    నమోదు_2 = (keyvalue_T *)bsearch(&లక్ష్యం, &rgb_tab, ARRAY_నిడివి(rgb_tab),
	    పరిమాణం(rgb_tab[0]), cmp_keyvalue_విలువ_i);
    ఒకవేళ (నమోదు_2 != ఖాళీ)
	తిరిగి gui_adjust_rgb((guicolor_T)నమోదు_2->కీలకం);

# ఒకవేళ defined(FEAT_EVAL)
    /*
     * Not a traditional color. Load additional color aliases and then consult the alias table.
     */

    color = colorname2rgb(name);
    ఒకవేళ (color == INVALCOLOR)
    {
	ఎక్కించు_default_colors_lists();
	color = colorname2rgb(name);
    }

    తిరిగి color;
# లేకపోతే
    తిరిగి INVALCOLOR;
# endif
}

    guicolor_T
gui_get_rgb_color_cmn(పూర్ణాంకం r, పూర్ణాంకం g, పూర్ణాంకం b)
{
    guicolor_T  color = RGB(r, g, b);

    ఒకవేళ (color > 0xffffff)
	తిరిగి INVALCOLOR;
    తిరిగి gui_adjust_rgb(color);
}
#అయితే_ముగింపు

/*
 * Table with the specifications for an attribute number.
 * Note that this table is used by ALL buffers.  This is required because the
 * GUI can redraw at any time for any buffer.
 */
స్థిర garray_T	పదం_attr_పట్టిక = {0, 0, 0, 0, ఖాళీ};

#నిర్వచించు పదం_ATTR_నమోదు(idx) ((attrentry_T *)పదం_attr_పట్టిక.ga_దత్తాంశం)[idx]

స్థిర garray_T	cterm_attr_పట్టిక = {0, 0, 0, 0, ఖాళీ};

#నిర్వచించు CTERM_ATTR_నమోదు(idx) ((attrentry_T *)cterm_attr_పట్టిక.ga_దత్తాంశం)[idx]

#నిర్వచితమైతే FEAT_GUI
స్థిర garray_T	gui_attr_పట్టిక = {0, 0, 0, 0, ఖాళీ};

# define GUI_ATTR_నమోదు(idx) ((attrentry_T *)gui_attr_పట్టిక.ga_దత్తాంశం)[idx]
#అయితే_ముగింపు

/*
 * Return the attr number for a set of colors and font.
 * Add a new entry to the term_attr_table, cterm_attr_table or gui_attr_table
 * if the combination is new.
 * Return 0 for error (no more room).
 */
    స్థిర పూర్ణాంకం
get_attr_నమోదు(garray_T *పట్టిక, attrentry_T *aep)
{
    పూర్ణాంకం		i;
    attrentry_T	*taep;
    స్థిర పూర్ణాంకం	recursive = FALSE;

    // Init the table, in case it wasn't done yet.
    పట్టిక->ga_itemsize = పరిమాణం(attrentry_T);
    పట్టిక->ga_growsize = 7;

    // Try to find an entry with the same specifications.
    కొరకు (i = 0; i < పట్టిక->ga_len; ++i)
    {
	taep = &(((attrentry_T *)పట్టిక->ga_దత్తాంశం)[i]);
	ఒకవేళ (	   aep->ae_attr == taep->ae_attr
		&& (
#నిర్వచితమైతే FEAT_GUI
		       (పట్టిక == &gui_attr_పట్టిక
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
#అయితే_ముగింపు
		       (పట్టిక == &పదం_attr_పట్టిక
			&& (aep->ae_u.పదం.ప్రారంభం == ఖాళీ)
					    == (taep->ae_u.పదం.ప్రారంభం == ఖాళీ)
			&& (aep->ae_u.పదం.ప్రారంభం == ఖాళీ
			    || STRCMP(aep->ae_u.పదం.ప్రారంభం,
						  taep->ae_u.పదం.ప్రారంభం) == 0)
			&& (aep->ae_u.పదం.stop == ఖాళీ)
					     == (taep->ae_u.పదం.stop == ఖాళీ)
			&& (aep->ae_u.పదం.stop == ఖాళీ
			    || STRCMP(aep->ae_u.పదం.stop,
						  taep->ae_u.పదం.stop) == 0))
		    || (పట్టిక == &cterm_attr_పట్టిక
			    && aep->ae_u.cterm.fg_color
						  == taep->ae_u.cterm.fg_color
			    && aep->ae_u.cterm.bg_color
						  == taep->ae_u.cterm.bg_color
			    && aep->ae_u.cterm.ul_color
						  == taep->ae_u.cterm.ul_color
			    && aep->ae_u.cterm.font
						  == taep->ae_u.cterm.font
#నిర్వచితమైతే FEAT_TERMGUICOLORS
			    && aep->ae_u.cterm.fg_rgb
						    == taep->ae_u.cterm.fg_rgb
			    && aep->ae_u.cterm.bg_rgb
						    == taep->ae_u.cterm.bg_rgb
			    && aep->ae_u.cterm.ul_rgb
						    == taep->ae_u.cterm.ul_rgb
#అయితే_ముగింపు
		       )))

	తిరిగి i + ATTR_OFF;
    }

    ఒకవేళ (పట్టిక->ga_len + ATTR_OFF > MAX_TYPENR)
    {
	// Running out of attribute entries!  remove all attributes, and
	// compute new ones for all groups.
	// When called recursively, we are really out of numbers.
	ఒకవేళ (recursive)
	{
	    emsg(_(e_too_many_different_highlighting_లక్షణాలు_లోపల_use));
	    తిరిగి 0;
	}
	recursive = TRUE;

	clear_hl_tables();

	సమితి_must_redraw(UPD_CLEAR);

	కొరకు (i = 0; i < highlight_ga.ga_len; ++i)
	    సమితి_hl_attr(i);

	recursive = FALSE;
    }

    // This is a new combination of colors and font, add an entry.
    ఒకవేళ (ga_grow(పట్టిక, 1) == FAIL)
	తిరిగి 0;

    taep = &(((attrentry_T *)పట్టిక->ga_దత్తాంశం)[పట్టిక->ga_len]);
    CLEAR_సూచిక(taep);
    taep->ae_attr = aep->ae_attr;
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (పట్టిక == &gui_attr_పట్టిక)
    {
	taep->ae_u.gui.fg_color = aep->ae_u.gui.fg_color;
	taep->ae_u.gui.bg_color = aep->ae_u.gui.bg_color;
	taep->ae_u.gui.sp_color = aep->ae_u.gui.sp_color;
	taep->ae_u.gui.font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
	taep->ae_u.gui.fontset = aep->ae_u.gui.fontset;
# endif
    }
#అయితే_ముగింపు
    ఒకవేళ (పట్టిక == &పదం_attr_పట్టిక)
    {
	ఒకవేళ (aep->ae_u.పదం.ప్రారంభం == ఖాళీ)
	    taep->ae_u.పదం.ప్రారంభం = ఖాళీ;
	లేకపోతే
	    taep->ae_u.పదం.ప్రారంభం = vim_strsave(aep->ae_u.పదం.ప్రారంభం);
	ఒకవేళ (aep->ae_u.పదం.stop == ఖాళీ)
	    taep->ae_u.పదం.stop = ఖాళీ;
	లేకపోతే
	    taep->ae_u.పదం.stop = vim_strsave(aep->ae_u.పదం.stop);
    }
    లేకపోతే ఒకవేళ (పట్టిక == &cterm_attr_పట్టిక)
    {
	taep->ae_u.cterm.fg_color = aep->ae_u.cterm.fg_color;
	taep->ae_u.cterm.bg_color = aep->ae_u.cterm.bg_color;
	taep->ae_u.cterm.ul_color = aep->ae_u.cterm.ul_color;
	taep->ae_u.cterm.font = aep->ae_u.cterm.font;
#నిర్వచితమైతే FEAT_TERMGUICOLORS
	taep->ae_u.cterm.fg_rgb = aep->ae_u.cterm.fg_rgb;
	taep->ae_u.cterm.bg_rgb = aep->ae_u.cterm.bg_rgb;
	taep->ae_u.cterm.ul_rgb = aep->ae_u.cterm.ul_rgb;
#అయితే_ముగింపు
    }
    ++పట్టిక->ga_len;
    తిరిగి (పట్టిక->ga_len - 1 + ATTR_OFF);
}

#ఒకవేళ defined(FEAT_TERMINAL)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    పూర్ణాంకం
get_cterm_attr_idx(పూర్ణాంకం attr, పూర్ణాంకం fg, పూర్ణాంకం bg)
{
    attrentry_T		at_en;

    CLEAR_క్షేత్రం(at_en);
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
    తిరిగి get_attr_నమోదు(&cterm_attr_పట్టిక, &at_en);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_TERMINAL) && defined(FEAT_TERMGUICOLORS)
/*
 * Get an attribute index for a 'termguicolors' entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    పూర్ణాంకం
get_tgc_attr_idx(పూర్ణాంకం attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_క్షేత్రం(at_en);
    at_en.ae_attr = attr;
    ఒకవేళ (fg == INVALCOLOR && bg == INVALCOLOR)
    {
	// If both GUI colors are not set fall back to the cterm colors.  Helps
	// if the GUI only has an attribute, such as undercurl.
	at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
    }
    లేకపోతే
    {
	at_en.ae_u.cterm.fg_rgb = fg;
	at_en.ae_u.cterm.bg_rgb = bg;
    }
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
    తిరిగి get_attr_నమోదు(&cterm_attr_పట్టిక, &at_en);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_TERMINAL) && defined(FEAT_GUI)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    పూర్ణాంకం
get_gui_attr_idx(పూర్ణాంకం attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_క్షేత్రం(at_en);
    at_en.ae_attr = attr;
    at_en.ae_u.gui.fg_color = fg;
    at_en.ae_u.gui.bg_color = bg;
    తిరిగి get_attr_నమోదు(&gui_attr_పట్టిక, &at_en);
}
#అయితే_ముగింపు

/*
 * Clear all highlight tables.
 */
    శూన్య
clear_hl_tables(శూన్య)
{
    పూర్ణాంకం		i;
    attrentry_T	*taep;

#నిర్వచితమైతే FEAT_GUI
    ga_clear(&gui_attr_పట్టిక);
#అయితే_ముగింపు
    కొరకు (i = 0; i < పదం_attr_పట్టిక.ga_len; ++i)
    {
	taep = &(((attrentry_T *)పదం_attr_పట్టిక.ga_దత్తాంశం)[i]);
	vim_free(taep->ae_u.పదం.ప్రారంభం);
	vim_free(taep->ae_u.పదం.stop);
    }
    ga_clear(&పదం_attr_పట్టిక);
    ga_clear(&cterm_attr_పట్టిక);
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
    పూర్ణాంకం
hl_combine_attr(పూర్ణాంకం char_attr, పూర్ణాంకం prim_attr)
{
    attrentry_T *char_aep = ఖాళీ;
    attrentry_T *prim_aep;
    attrentry_T new_en;

    ఒకవేళ (char_attr == 0)
	తిరిగి prim_attr;
    ఒకవేళ (char_attr <= HL_ALL && prim_attr <= HL_ALL)
	తిరిగి ATTR_COMBINE(char_attr, prim_attr);
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	ఒకవేళ (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	ఒకవేళ (char_aep != ఖాళీ)
	    new_en = *char_aep;
	లేకపోతే
	{
	    CLEAR_క్షేత్రం(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    ఒకవేళ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	ఒకవేళ (prim_attr <= HL_ALL)
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	లేకపోతే
	{
	    prim_aep = syn_gui_attr2entry(prim_attr);
	    ఒకవేళ (prim_aep != ఖాళీ)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		ఒకవేళ (prim_aep->ae_u.gui.fg_color != INVALCOLOR)
		    new_en.ae_u.gui.fg_color = prim_aep->ae_u.gui.fg_color;
		ఒకవేళ (prim_aep->ae_u.gui.bg_color != INVALCOLOR)
		    new_en.ae_u.gui.bg_color = prim_aep->ae_u.gui.bg_color;
		ఒకవేళ (prim_aep->ae_u.gui.sp_color != INVALCOLOR)
		    new_en.ae_u.gui.sp_color = prim_aep->ae_u.gui.sp_color;
		ఒకవేళ (prim_aep->ae_u.gui.font != NOFONT)
		    new_en.ae_u.gui.font = prim_aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
		ఒకవేళ (prim_aep->ae_u.gui.fontset != NOFONTSET)
		    new_en.ae_u.gui.fontset = prim_aep->ae_u.gui.fontset;
# endif
	    }
	}
	తిరిగి get_attr_నమోదు(&gui_attr_పట్టిక, &new_en);
    }
#అయితే_ముగింపు

    ఒకవేళ (IS_CTERM)
    {
	ఒకవేళ (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	ఒకవేళ (char_aep != ఖాళీ)
	    new_en = *char_aep;
	లేకపోతే
	{
	    CLEAR_క్షేత్రం(new_en);
#నిర్వచితమైతే FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#అయితే_ముగింపు
	    ఒకవేళ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	ఒకవేళ (prim_attr <= HL_ALL)
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	లేకపోతే
	{
	    prim_aep = syn_cterm_attr2entry(prim_attr);
	    ఒకవేళ (prim_aep != ఖాళీ)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		ఒకవేళ (prim_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color = prim_aep->ae_u.cterm.fg_color;
		ఒకవేళ (prim_aep->ae_u.cterm.bg_color > 0)
		    new_en.ae_u.cterm.bg_color = prim_aep->ae_u.cterm.bg_color;
		ఒకవేళ (prim_aep->ae_u.cterm.ul_color > 0)
		    new_en.ae_u.cterm.ul_color = prim_aep->ae_u.cterm.ul_color;
		ఒకవేళ (prim_aep->ae_u.cterm.font > 0)
		    new_en.ae_u.cterm.font = prim_aep->ae_u.cterm.font;
#నిర్వచితమైతే FEAT_TERMGUICOLORS
		// If both fg and bg are not set fall back to cterm colors.
		// Helps for SpellBad which uses undercurl in the GUI.
		ఒకవేళ (COLOR_INVALID(prim_aep->ae_u.cterm.fg_rgb)
			&& COLOR_INVALID(prim_aep->ae_u.cterm.bg_rgb))
		{
		    ఒకవేళ (prim_aep->ae_u.cterm.fg_color > 0)
			new_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
		    ఒకవేళ (prim_aep->ae_u.cterm.bg_color > 0)
			new_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
		}
		లేకపోతే
		{
		    ఒకవేళ (prim_aep->ae_u.cterm.fg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.fg_rgb = prim_aep->ae_u.cterm.fg_rgb;
		    ఒకవేళ (prim_aep->ae_u.cterm.bg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.bg_rgb = prim_aep->ae_u.cterm.bg_rgb;
		}
		ఒకవేళ (prim_aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		    new_en.ae_u.cterm.ul_rgb = prim_aep->ae_u.cterm.ul_rgb;
#అయితే_ముగింపు
	    }
	}
	తిరిగి get_attr_నమోదు(&cterm_attr_పట్టిక, &new_en);
    }

    ఒకవేళ (char_attr > HL_ALL)
	char_aep = syn_పదం_attr2entry(char_attr);
    ఒకవేళ (char_aep != ఖాళీ)
	new_en = *char_aep;
    లేకపోతే
    {
	CLEAR_క్షేత్రం(new_en);
	ఒకవేళ (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    ఒకవేళ (prim_attr <= HL_ALL)
	new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
    లేకపోతే
    {
	prim_aep = syn_పదం_attr2entry(prim_attr);
	ఒకవేళ (prim_aep != ఖాళీ)
	{
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_aep->ae_attr);
	    ఒకవేళ (prim_aep->ae_u.పదం.ప్రారంభం != ఖాళీ)
	    {
		new_en.ae_u.పదం.ప్రారంభం = prim_aep->ae_u.పదం.ప్రారంభం;
		new_en.ae_u.పదం.stop = prim_aep->ae_u.పదం.stop;
	    }
	}
    }
    తిరిగి get_attr_నమోదు(&పదం_attr_పట్టిక, &new_en);
}

// ANSI color order: Black, Red, Green, Yellow, Blue, Magenta,
// Cyan, White, then bright variants.  Approximate RGB values used when
// only a cterm color number is known (no guifg/guibg).  Real terminal
// palettes may differ if the user reconfigured their emulator, but
// these are reasonable xterm-ish defaults.
స్థిర స్థిరరాశి long_u cterm_color_16[16] = {
    0x000000, 0xc00000, 0x008000, 0x808000,
    0x0000c0, 0xc000c0, 0x004080, 0xc0c0c0,
    0x808080, 0xff8080, 0x00ff00, 0xffff00,
    0x6060ff, 0xff40ff, 0x00ffff, 0xffffff
};

#నిర్వచితమైతే FEAT_TERMGUICOLORS
/*
 * Convert a cterm color number (1-16) to an RGB value.
 * Used as a fallback when 'termguicolors' is set but only cterm colors are
 * specified (no guifg/guibg).
 * Returns INVALCOLOR if the color number is out of range.
 */
    స్థిర guicolor_T
cterm_color_గమ్యం_rgb(పూర్ణాంకం color_nr)
{
    ఒకవేళ (color_nr < 1 || color_nr > 16)
	తిరిగి INVALCOLOR;
    తిరిగి (guicolor_T)cterm_color_16[color_nr - 1];
}
#అయితే_ముగింపు

/*
 * Convert an xterm 256-color index (0-255) to an approximate RGB triple.
 * Uses the standard xterm palette: 0-15 ANSI (cterm_color_16), 16-231
 * 6x6x6 cube, 232-255 grayscale ramp.
 */
    స్థిర శూన్య
cterm_idx_గమ్యం_rgb(పూర్ణాంకం idx, పూర్ణాంకం *r, పూర్ణాంకం *g, పూర్ణాంకం *b)
{
    స్థిర స్థిరరాశి పూర్ణాంకం cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

    ఒకవేళ (idx < 0 || idx > 255)
    {
	*r = *g = *b = 0;
	తిరిగి;
    }
    ఒకవేళ (idx < 16)
    {
	long_u rgb = cterm_color_16[idx];
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
    }
    లేకపోతే ఒకవేళ (idx < 232)
    {
	పూర్ణాంకం n = idx - 16;
	*r = cube[n / 36 % 6];
	*g = cube[n / 6  % 6];
	*b = cube[n      % 6];
    }
    లేకపోతే
    {
	పూర్ణాంకం v = 8 + (idx - 232) * 10;
	*r = *g = *b = v;
    }
}

/*
 * Approximate an RGB triple to the nearest xterm 256-color index.
 * Searches the 6x6x6 cube and the grayscale ramp; returns 0-255.
 */
    స్థిర పూర్ణాంకం
rgb_గమ్యం_cterm_idx(పూర్ణాంకం r, పూర్ణాంకం g, పూర్ణాంకం b)
{
    స్థిర స్థిరరాశి పూర్ణాంకం cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };
    పూర్ణాంకం ఉత్తమ = 0;
    దీర్ఘ ఉత్తమ_d = -1;
    పూర్ణాంకం i;

    // Search the 6x6x6 cube.
    కొరకు (i = 16; i < 232; ++i)
    {
	పూర్ణాంకం n = i - 16;
	పూర్ణాంకం cr = cube[n / 36 % 6];
	పూర్ణాంకం cg = cube[n / 6  % 6];
	పూర్ణాంకం cb = cube[n      % 6];
	దీర్ఘ d = (cr - r) * (cr - r) + (cg - g) * (cg - g) + (cb - b) * (cb - b);
	ఒకవేళ (ఉత్తమ_d < 0 || d < ఉత్తమ_d)
	{
	    ఉత్తమ_d = d;
	    ఉత్తమ = i;
	}
    }
    // Search the grayscale ramp.
    కొరకు (i = 232; i < 256; ++i)
    {
	పూర్ణాంకం v = 8 + (i - 232) * 10;
	దీర్ఘ d = (v - r) * (v - r) + (v - g) * (v - g) + (v - b) * (v - b);
	ఒకవేళ (d < ఉత్తమ_d)
	{
	    ఉత్తమ_d = d;
	    ఉత్తమ = i;
	}
    }
    తిరిగి ఉత్తమ;
}

/*
 * Resolve a color to an RGB triple.  Prefer the gui RGB value (set by
 * guifg/guibg) when valid, otherwise convert the cterm 256-color index.
 * Returns true on success; false when no color is defined.
 * cterm_c is 1-based (0 means "no color").
 */
    స్థిర bool
resolve_color_గమ్యం_rgb(పూర్ణాంకం cterm_c, guicolor_T rgb వాడని, పూర్ణాంకం *r, పూర్ణాంకం *g, పూర్ణాంకం *b)
{
#నిర్వచితమైతే FEAT_TERMGUICOLORS
    ఒకవేళ (!COLOR_INVALID(rgb))
    {
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
	తిరిగి true;
    }
#అయితే_ముగింపు
    ఒకవేళ (cterm_c > 0)
    {
	cterm_idx_గమ్యం_rgb(cterm_c - 1, r, g, b);
	తిరిగి true;
    }
    తిరిగి false;
}

/*
 * get a RGB fallback color from gui, cterm or default color
 */
    స్థిర guicolor_T
resolve_fallback_color(పూర్ణాంకం cterm_c, guicolor_T rgb, guicolor_T default_rgb)
{
    పూర్ణాంకం red, green, blue;
    ఒకవేళ (!resolve_color_గమ్యం_rgb(cterm_c, rgb, &red, &green, &blue))
	తిరిగి default_rgb;
    లేకపోతే
	తిరిగి (red << 16) | (green << 8) | blue;
}

/*
 * get a RGB fallback foreground color from guifg, ctermfg or deduced from background
 */
    స్థిర శూన్య
resolve_fallback_fg_గమ్యం_rgb(శూన్య)
{
    guicolor_T fgcolor_or_gui_fgcolor = INVALCOLOR;
#నిర్వచితమైతే FEAT_TERMGUICOLORS
    fgcolor_or_gui_fgcolor = cterm_సాధారణ_fg_gui_color;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	fgcolor_or_gui_fgcolor = gui.norm_pixel;
#అయితే_ముగింపు
    fallback_fg_rgb = resolve_fallback_color(cterm_సాధారణ_fg_color, fgcolor_or_gui_fgcolor, (*p_bg == 'l') ? 0x000000 : 0xFFFFFF);
}

/*
 * get a RGB fallback background color from guifg, ctermbg or deduced from background
 */
    స్థిర శూన్య
resolve_fallback_bg_గమ్యం_rgb(శూన్య)
{
    guicolor_T bgcolor_or_gui_bgcolor = INVALCOLOR;
#నిర్వచితమైతే FEAT_TERMGUICOLORS
    bgcolor_or_gui_bgcolor = cterm_సాధారణ_bg_gui_color;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	bgcolor_or_gui_bgcolor = gui.వెనుక_pixel;
#అయితే_ముగింపు
    fallback_bg_rgb = resolve_fallback_color(cterm_సాధారణ_bg_color, bgcolor_or_gui_bgcolor, (*p_bg == 'l') ? 0xFFFFFF : 0x000000);
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
    స్థిర పూర్ణాంకం
blend_cterm_colors(పూర్ణాంకం popup_c, guicolor_T popup_rgb,
		   పూర్ణాంకం under_c, guicolor_T under_rgb,
		   పూర్ణాంకం default_rgb,
		   పూర్ణాంకం blend_val)
{
    పూర్ణాంకం pr, pg, pb, ur, ug, ub, r, g, b;

    ఒకవేళ (t_colors < 256)
	తిరిగి popup_c;
    ఒకవేళ (!resolve_color_గమ్యం_rgb(popup_c, popup_rgb, &pr, &pg, &pb))
	తిరిగి under_c;
    ఒకవేళ (blend_val <= 0)
	తిరిగి rgb_గమ్యం_cterm_idx(pr, pg, pb) + 1;
    ఒకవేళ (!resolve_color_గమ్యం_rgb(under_c, under_rgb, &ur, &ug, &ub))
    {
	ur = (default_rgb >> 16) & 0xFF;
	ug = (default_rgb >> 8) & 0xFF;
	ub = default_rgb & 0xFF;
    }
    ఒకవేళ (blend_val >= 100)
	తిరిగి rgb_గమ్యం_cterm_idx(ur, ug, ub) + 1;
    r = pr + (ur - pr) * blend_val / 100;
    g = pg + (ug - pg) * blend_val / 100;
    b = pb + (ub - pb) * blend_val / 100;
    తిరిగి rgb_గమ్యం_cterm_idx(r, g, b) + 1;
}

#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Blend two RGB colors based on blend value (0-100).
 * blend: 0=return first argument color, 100=return second argument color
 * If bg_color is INVALCOLOR, high blend means more visible (return INVALCOLOR).
 */
    స్థిర guicolor_T
blend_colors(guicolor_T popup_color, guicolor_T bg_color, పూర్ణాంకం blend_val)
{
    పూర్ణాంకం r1, g1, b1, r2, g2, b2, r, g, b;

    // CTERMCOLOR is a sentinel meaning "use the cterm color"; for blending
    // it has no real RGB so treat it like INVALCOLOR.
    ఒకవేళ (COLOR_INVALID(popup_color))
	తిరిగి INVALCOLOR;

    // Fully transparent: use underlying color as-is.
    ఒకవేళ (blend_val >= 100)
	తిరిగి bg_color;

    r1 = (popup_color >> 16) & 0xFF;
    g1 = (popup_color >> 8) & 0xFF;
    b1 = popup_color & 0xFF;

    ఒకవేళ (COLOR_INVALID(bg_color))
	bg_color = fallback_bg_rgb;

    r2 = (bg_color >> 16) & 0xFF;
    g2 = (bg_color >> 8) & 0xFF;
    b2 = bg_color & 0xFF;

    r = r1 + (r2 - r1) * blend_val / 100;
    g = g1 + (g2 - g1) * blend_val / 100;
    b = b1 + (b2 - b1) * blend_val / 100;

    తిరిగి (r << 16) | (g << 8) | b;
}
#అయితే_ముగింపు

/*
 * Blend attributes for popup windows and menus with opacity.
 * Blends foreground and/or background colors based on blend value (0-100).
 * blend: 0 = opaque (use popup colors), 100 = transparent (use background colors)
 * blend_fg: TRUE to blend foreground color, FALSE to keep popup foreground
 */
    స్థిర పూర్ణాంకం
hl_blend_attr_common(
    పూర్ణాంకం		char_attr,
    పూర్ణాంకం		popup_attr,
    పూర్ణాంకం		blend,
    పూర్ణాంకం		blend_fg)
{
    attrentry_T *char_aep = ఖాళీ;
    attrentry_T *popup_aep;
    attrentry_T new_en;
    attrentry_T tmp_en;

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	ఒకవేళ (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	ఒకవేళ (char_aep != ఖాళీ)
	    new_en = *char_aep;
	లేకపోతే
	{
	    CLEAR_క్షేత్రం(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    ఒకవేళ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	ఒకవేళ (popup_attr <= HL_ALL)
	{
	    CLEAR_క్షేత్రం(tmp_en);
	    tmp_en.ae_u.gui.fg_color = INVALCOLOR;
	    tmp_en.ae_u.gui.bg_color = INVALCOLOR;
	    tmp_en.ae_u.gui.sp_color = INVALCOLOR;
	    // preserve attributes other than color
	    tmp_en.ae_attr = popup_attr;
	    popup_aep = &tmp_en;

	    popup_aep->ae_u.gui.bg_color = fallback_bg_rgb;
	}
	లేకపోతే
	    popup_aep = syn_gui_attr2entry(popup_attr);

	ఒకవేళ (popup_aep != ఖాళీ)
	{
	    guicolor_T popup_bg_rgb = popup_aep->ae_u.gui.bg_color;
	    ఒకవేళ (COLOR_INVALID(popup_bg_rgb))
		popup_bg_rgb = fallback_bg_rgb;

	    ఒకవేళ (blend_fg)
	    {
		// blend_fg=TRUE: fade underlying text toward popup bg.
		{
		    పూర్ణాంకం పునాది_fg = fallback_fg_rgb;
		    ఒకవేళ (char_aep != ఖాళీ
			    && char_aep->ae_u.gui.fg_color != INVALCOLOR)
			పునాది_fg = char_aep->ae_u.gui.fg_color;
		    new_en.ae_u.gui.fg_color = blend_colors(
			    popup_bg_rgb, పునాది_fg, blend);
		}
	    }
	    లేకపోతే
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
		ఒకవేళ (char_aep != ఖాళీ
			&& !COLOR_INVALID(char_aep->ae_u.gui.bg_color))
		    underlying_bg = char_aep->ae_u.gui.bg_color;
		new_en.ae_u.gui.bg_color = blend_colors(
			popup_bg_rgb,
			underlying_bg, blend);
	    }
	}
	తిరిగి get_attr_నమోదు(&gui_attr_పట్టిక, &new_en);
    }
#అయితే_ముగింపు

    ఒకవేళ (IS_CTERM)
    {
	ఒకవేళ (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	ఒకవేళ (char_aep != ఖాళీ)
	    new_en = *char_aep;
	లేకపోతే
	{
	    CLEAR_క్షేత్రం(new_en);
#నిర్వచితమైతే FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#అయితే_ముగింపు
	    ఒకవేళ (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	ఒకవేళ (popup_attr <= HL_ALL)
	{
	    CLEAR_క్షేత్రం(tmp_en);
#నిర్వచితమైతే FEAT_TERMGUICOLORS
	    tmp_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    tmp_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	    // allow blending with termguicolors
	    tmp_en.ae_u.cterm.bg_rgb = fallback_bg_rgb;
#అయితే_ముగింపు
	    // preserve attributes other than color
	    tmp_en.ae_attr = popup_attr;
	    popup_aep = &tmp_en;

	    // allow blending with notermguicolors
	    popup_aep->ae_u.cterm.bg_color = cterm_సాధారణ_bg_color;
	}
	లేకపోతే
	    popup_aep = syn_cterm_attr2entry(popup_attr);

	ఒకవేళ (popup_aep != ఖాళీ)
	{
	    guicolor_T popup_bg_rgb = INVALCOLOR;
#నిర్వచితమైతే FEAT_TERMGUICOLORS
	    // Fall back to cterm color converted to RGB when gui color is not set.
	    popup_bg_rgb = popup_aep->ae_u.cterm.bg_rgb;
	    ఒకవేళ (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color > 0)
		popup_bg_rgb = cterm_color_గమ్యం_rgb(
			popup_aep->ae_u.cterm.bg_color);
#అయితే_ముగింపు
	    // assign default color if guibg and ctermbg are not set for popup
	    ఒకవేళ (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color == 0)
		popup_bg_rgb = fallback_bg_rgb;

	    ఒకవేళ (!blend_fg)
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
		ఒకవేళ (popup_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color =
			popup_aep->ae_u.cterm.fg_color;
		లేకపోతే ఒకవేళ (cterm_సాధారణ_fg_color > 0)
		    new_en.ae_u.cterm.fg_color = cterm_సాధారణ_fg_color;
		లేకపోతే
		    // black-ish or white-ish
		    new_en.ae_u.cterm.fg_color = (*p_bg == 'l') ? 1 : 16;
		new_en.ae_u.cterm.ul_color = popup_aep->ae_u.cterm.ul_color;
		new_en.ae_u.cterm.font = popup_aep->ae_u.cterm.font;
#నిర్వచితమైతే FEAT_TERMGUICOLORS
		new_en.ae_u.cterm.ul_rgb = popup_aep->ae_u.cterm.ul_rgb;
#అయితే_ముగింపు
	    }
	    లేకపోతే
	    {
		// blend_fg=TRUE: fade underlying fg toward popup bg in
		// the 256-color palette.  Used when the popup is over a
		// cell rendered with cterm colors (no termguicolors RGB).
		పూర్ణాంకం under_fg = (char_aep != ఖాళీ)
		    ? char_aep->ae_u.cterm.fg_color : 0;
		guicolor_T under_fg_rgb = INVALCOLOR;
#నిర్వచితమైతే FEAT_TERMGUICOLORS
		ఒకవేళ (char_aep != ఖాళీ)
		    under_fg_rgb = char_aep->ae_u.cterm.fg_rgb;
#అయితే_ముగింపు
		new_en.ae_u.cterm.fg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_fg, under_fg_rgb, fallback_fg_rgb, blend);
	    }
	    // Approximate cterm bg by blending with the underlying bg
	    // in the 256-color palette and mapping to the nearest entry.
	    {
		పూర్ణాంకం under_bg = (char_aep != ఖాళీ)
		    ? char_aep->ae_u.cterm.bg_color : 0;
		guicolor_T under_bg_rgb = INVALCOLOR;
#నిర్వచితమైతే FEAT_TERMGUICOLORS
		ఒకవేళ (char_aep != ఖాళీ)
		    under_bg_rgb = char_aep->ae_u.cterm.bg_rgb;
#అయితే_ముగింపు
		new_en.ae_u.cterm.bg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_bg, under_bg_rgb, fallback_bg_rgb, blend);
	    }
#నిర్వచితమైతే FEAT_TERMGUICOLORS
	    // Blend RGB colors for termguicolors mode.
	    {
		ఒకవేళ (blend_fg)
		{
		    // blend_fg=TRUE: fade underlying text toward popup bg.
		    ఒకవేళ (popup_bg_rgb != INVALCOLOR)
		    {
			పూర్ణాంకం పునాది_fg = fallback_fg_rgb;
			// CTERMCOLOR is a sentinel meaning "use the cterm
			// color"; treat it as no underlying color so it is
			// not blended in as a real near-white pixel.
			ఒకవేళ (char_aep != ఖాళీ
				&& !COLOR_INVALID(char_aep->ae_u.cterm.fg_rgb))
			    పునాది_fg = char_aep->ae_u.cterm.fg_rgb;
			new_en.ae_u.cterm.fg_rgb = blend_colors(
				popup_bg_rgb, పునాది_fg, blend);
		    }
		}
		లేకపోతే
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

		    ఒకవేళ (COLOR_INVALID(popup_fg)
			    && popup_aep->ae_u.cterm.fg_color > 0)
			popup_fg = cterm_color_గమ్యం_rgb(
				popup_aep->ae_u.cterm.fg_color);

		    ఒకవేళ (!COLOR_INVALID(popup_fg))
			new_en.ae_u.cterm.fg_rgb = popup_fg;
		    లేకపోతే ఒకవేళ (!COLOR_INVALID(cterm_సాధారణ_fg_gui_color))
			new_en.ae_u.cterm.fg_rgb = cterm_సాధారణ_fg_gui_color;
		    లేకపోతే
			new_en.ae_u.cterm.fg_rgb = fallback_fg_rgb;
		}
		ఒకవేళ (popup_bg_rgb != INVALCOLOR)
		{
		    // Blend popup bg toward underlying bg
		    guicolor_T underlying_bg = fallback_bg_rgb;
		    ఒకవేళ (char_aep != ఖాళీ
			    && !COLOR_INVALID(char_aep->ae_u.cterm.bg_rgb))
			underlying_bg = char_aep->ae_u.cterm.bg_rgb;
		    new_en.ae_u.cterm.bg_rgb = blend_colors(
			    popup_bg_rgb, underlying_bg, blend);
		}
	    }
#అయితే_ముగింపు
	}
	తిరిగి get_attr_నమోదు(&cterm_attr_పట్టిక, &new_en);
    }

    ఒకవేళ (char_attr > HL_ALL)
	char_aep = syn_పదం_attr2entry(char_attr);
    ఒకవేళ (char_aep != ఖాళీ)
	new_en = *char_aep;
    లేకపోతే
    {
	CLEAR_క్షేత్రం(new_en);
	ఒకవేళ (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    // For term mode, no separate background color handling.
    తిరిగి get_attr_నమోదు(&పదం_attr_పట్టిక, &new_en);
}

/*
 * Blend attributes for popup windows with opacity.
 */
    పూర్ణాంకం
hl_blend_attr(పూర్ణాంకం char_attr, పూర్ణాంకం popup_attr, పూర్ణాంకం blend, పూర్ణాంకం blend_fg)
{
    ఒకవేళ (blend >= 100 && blend_fg)
	తిరిగి char_attr;

    తిరిగి hl_blend_attr_common(char_attr, popup_attr, blend, blend_fg);
}

/*
 * Blend underlying foreground and background colors for pum space cells.
 */
    పూర్ణాంకం
hl_pum_blend_attr(పూర్ణాంకం char_attr, పూర్ణాంకం popup_attr, పూర్ణాంకం blend)
{
    తిరిగి hl_blend_attr_common(char_attr, popup_attr, blend, TRUE);
}

#నిర్వచితమైతే FEAT_GUI
    attrentry_T *
syn_gui_attr2entry(పూర్ణాంకం attr)
{
    attr -= ATTR_OFF;
    ఒకవేళ (attr >= gui_attr_పట్టిక.ga_len)	    // did ":syntax clear"
	తిరిగి ఖాళీ;
    తిరిగి &(GUI_ATTR_నమోదు(attr));
}
#అయితే_ముగింపు

/*
 * Get the highlight attributes (HL_BOLD etc.) from an attribute nr.
 * Only to be used when "attr" > HL_ALL.
 */
    పూర్ణాంకం
syn_attr2attr(పూర్ణాంకం attr)
{
    attrentry_T	*aep;

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	aep = syn_gui_attr2entry(attr);
    లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	లేకపోతే
	    aep = syn_పదం_attr2entry(attr);

    ఒకవేళ (aep == ఖాళీ)	    // highlighting not set
	తిరిగి 0;
    తిరిగి aep->ae_attr;
}


    attrentry_T *
syn_పదం_attr2entry(పూర్ణాంకం attr)
{
    attr -= ATTR_OFF;
    ఒకవేళ (attr >= పదం_attr_పట్టిక.ga_len)	    // did ":syntax clear"
	తిరిగి ఖాళీ;
    తిరిగి &(పదం_ATTR_నమోదు(attr));
}

    attrentry_T *
syn_cterm_attr2entry(పూర్ణాంకం attr)
{
    attr -= ATTR_OFF;
    ఒకవేళ (attr >= cterm_attr_పట్టిక.ga_len)	// did ":syntax clear"
	తిరిగి ఖాళీ;
    తిరిగి &(CTERM_ATTR_నమోదు(attr));
}

#నిర్వచించు LIST_ATTR   1
#నిర్వచించు LIST_STRING 2
#నిర్వచించు LIST_INT    3

    స్థిర శూన్య
highlight_list_one(పూర్ణాంకం id)
{
    hl_సమూహం_T	    *sgp;
    పూర్ణాంకం		    didh = FALSE;

    sgp = &HL_పట్టిక()[id - 1];	    // index is ID minus one

    ఒకవేళ (సందేశం_filtered(sgp->sg_name))
	తిరిగి;

    // Note: Keep this in sync with expand_highlight_group().
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_పదం, ఖాళీ, "term");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_ప్రారంభం, "start");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_stop, "stop");

    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_cterm, ఖాళీ, "cterm");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_fg, ఖాళీ, "ctermfg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_bg, ఖాళీ, "ctermbg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_ul, ఖాళీ, "ctermul");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_font, ఖాళీ, "ctermfont");

#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_EVAL)
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_gui, ఖాళీ, "gui");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_fg_name, "guifg");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_bg_name, "guibg");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_sp_name, "guisp");
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_font_name, "font");
#అయితే_ముగింపు

    ఒకవేళ (sgp->sg_link && !got_int)
    {
	(శూన్య)syn_list_శీర్షిక(didh, 9999, id);
	didh = TRUE;
	సందేశం_puts_attr("links to", HL_ATTR(HLF_D));
	సందేశం_putchar(' ');
	సందేశం_outtrans(HL_పట్టిక()[HL_పట్టిక()[id - 1].sg_link - 1].sg_name);
    }

    ఒకవేళ (!didh)
	highlight_list_arg(id, didh, LIST_STRING, 0, (char_u *)"cleared", "");
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (p_verbose > 0)
	చివరి_సమితి_సందేశం(sgp->sg_script_ctx);
#అయితే_ముగింపు
}

    స్థిర char_u*
highlight_arg_గమ్యం_string(
    పూర్ణాంకం		type,
    పూర్ణాంకం		iarg,
    char_u	*sarg,
    char_u	*buf)
{
    ఒకవేళ (type == LIST_INT)
	sprintf((అక్షరం *)buf, "%d", iarg - 1);
    లేకపోతే ఒకవేళ (type == LIST_STRING)
	తిరిగి sarg;
    లేకపోతే // type == LIST_ATTR
    {
	size_t buflen;

	buf[0] = NUL;
	buflen = 0;
	కొరకు (పూర్ణాంకం i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(highlight_సూచీ_tab); ++i)
	{
	    ఒకవేళ (iarg & highlight_సూచీ_tab[i]->కీలకం)
	    {
		ఒకవేళ (buflen > 0)
		{
		    STRCPY(buf + buflen, (char_u *)",");
		    ++buflen;
		}
		STRCPY(buf + buflen, highlight_సూచీ_tab[i]->విలువ.string);
		buflen += highlight_సూచీ_tab[i]->విలువ.నిడివి;
		iarg &= ~highlight_సూచీ_tab[i]->కీలకం;	    // don't want "inverse"/"reverse"
	    }
	}
    }
    తిరిగి buf;
}

    స్థిర పూర్ణాంకం
highlight_list_arg(
    పూర్ణాంకం		id,
    పూర్ణాంకం		didh,
    పూర్ణాంకం		type,
    పూర్ణాంకం		iarg,
    char_u	*sarg,
    అక్షరం	*name)
{
    char_u	buf[MAX_ATTR_LEN];
    char_u	*ts;

    ఒకవేళ (got_int)
	తిరిగి FALSE;

    ఒకవేళ (type == LIST_STRING ? (sarg == ఖాళీ) : (iarg == 0))
	తిరిగి didh;

    ts = highlight_arg_గమ్యం_string(type, iarg, sarg, buf);

    (శూన్య)syn_list_శీర్షిక(didh,
	    (పూర్ణాంకం)(vim_strsize(ts) + STRLEN(name) + 1), id);
    didh = TRUE;
    ఒకవేళ (!got_int)
    {
	ఒకవేళ (*name != NUL)
	{
	    సందేశం_puts_attr(name, HL_ATTR(HLF_D));
	    సందేశం_puts_attr("=", HL_ATTR(HLF_D));
	}
	సందేశం_outtrans(ts);
    }
    తిరిగి didh;
}

#ఒకవేళ ((defined(FEAT_EVAL) || defined(FEAT_PRINTER))) && defined(FEAT_SYN_HL)
/*
 * Return "1" if highlight group "id" has attribute "flag".
 * Return NULL otherwise.
 */
    char_u *
highlight_has_attr(
    పూర్ణాంకం		id,
    పూర్ణాంకం		flag,
    పూర్ణాంకం		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    పూర్ణాంకం		attr;

    ఒకవేళ (id <= 0 || id > highlight_ga.ga_len)
	తిరిగి ఖాళీ;

# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_EVAL)
    ఒకవేళ (modec == 'g')
	attr = HL_పట్టిక()[id - 1].sg_gui;
    లేకపోతే
# endif
    {
	ఒకవేళ (modec == 'c')
	    attr = HL_పట్టిక()[id - 1].sg_cterm;
	లేకపోతే
	    attr = HL_పట్టిక()[id - 1].sg_పదం;
    }

    ఒకవేళ (attr & flag)
	తిరిగి (char_u *)"1";
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_SYN_HL) && defined(FEAT_EVAL)
/*
 * Return color name of highlight group "id".
 */
    char_u *
highlight_color(
    పూర్ణాంకం		id,
    char_u	*what,	// "font", "fg", "bg", "sp", "ul", "fg#", "bg#" or "sp#"
    పూర్ణాంకం		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    స్థిర char_u	name[20];
    పూర్ణాంకం			n;
    పూర్ణాంకం			fg = FALSE;
    పూర్ణాంకం			sp = FALSE;
    పూర్ణాంకం			ul = FALSE;
    పూర్ణాంకం			font = FALSE;

    ఒకవేళ (id <= 0 || id > highlight_ga.ga_len)
	తిరిగి ఖాళీ;

    ఒకవేళ (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'g')
	fg = TRUE;
    లేకపోతే ఒకవేళ (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'o'
	     && TOLOWER_ASC(what[2]) == 'n' && TOLOWER_ASC(what[3]) == 't')
	font = TRUE;
    లేకపోతే ఒకవేళ (TOLOWER_ASC(what[0]) == 's' && TOLOWER_ASC(what[1]) == 'p')
	sp = TRUE;
    లేకపోతే ఒకవేళ (TOLOWER_ASC(what[0]) == 'u' && TOLOWER_ASC(what[1]) == 'l')
	ul = TRUE;
    లేకపోతే ఒకవేళ (!(TOLOWER_ASC(what[0]) == 'b' && TOLOWER_ASC(what[1]) == 'g'))
	తిరిగి ఖాళీ;
    ఒకవేళ (modec == 'g')
    {
# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI
	// return font name
	ఒకవేళ (font)
	    తిరిగి HL_పట్టిక()[id - 1].sg_font_name;
#  endif

	// return #RRGGBB form (only possible when GUI is running)
	ఒకవేళ ((USE_24_బిట్టు) && what[2] == '#')
	{
	    guicolor_T		color;
	    long_u		rgb;
	    స్థిర char_u	buf[10];

	    ఒకవేళ (fg)
		color = HL_పట్టిక()[id - 1].sg_gui_fg;
	    లేకపోతే ఒకవేళ (sp)
		color = HL_పట్టిక()[id - 1].sg_gui_sp;
	    లేకపోతే
		color = HL_పట్టిక()[id - 1].sg_gui_bg;
	    ఒకవేళ (color == INVALCOLOR)
		తిరిగి ఖాళీ;
	    rgb = (long_u)GUI_MCH_GET_RGB(color);
	    sprintf((అక్షరం *)buf, "#%02x%02x%02x",
				      (గుర్తులేని)(rgb >> 16),
				      (గుర్తులేని)(rgb >> 8) & 255,
				      (గుర్తులేని)rgb & 255);
	    తిరిగి buf;
	}
# endif
	ఒకవేళ (fg)
	    తిరిగి (HL_పట్టిక()[id - 1].sg_gui_fg_name);
	ఒకవేళ (sp)
	    తిరిగి (HL_పట్టిక()[id - 1].sg_gui_sp_name);
	తిరిగి (HL_పట్టిక()[id - 1].sg_gui_bg_name);
    }
    ఒకవేళ (sp)
	తిరిగి ఖాళీ;
    ఒకవేళ (modec == 'c')
    {
	ఒకవేళ (fg)
	    n = HL_పట్టిక()[id - 1].sg_cterm_fg - 1;
	లేకపోతే ఒకవేళ (ul)
	    n = HL_పట్టిక()[id - 1].sg_cterm_ul - 1;
	లేకపోతే ఒకవేళ (font)
	    n = HL_పట్టిక()[id - 1].sg_cterm_font - 1;
	లేకపోతే
	    n = HL_పట్టిక()[id - 1].sg_cterm_bg - 1;
	ఒకవేళ (n < 0)
	    తిరిగి ఖాళీ;
	sprintf((అక్షరం *)name, "%d", n);
	తిరిగి name;
    }
    // term doesn't have color
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_SYN_HL) \
	    && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)) \
	&& defined(FEAT_PRINTER)
/*
 * Return color name of highlight group "id" as RGB value.
 */
    long_u
highlight_gui_color_rgb(
    పూర్ణాంకం		id,
    పూర్ణాంకం		fg)	// TRUE = fg, FALSE = bg
{
    guicolor_T	color;

    ఒకవేళ (id <= 0 || id > highlight_ga.ga_len)
	తిరిగి 0L;

    ఒకవేళ (fg)
	color = HL_పట్టిక()[id - 1].sg_gui_fg;
    లేకపోతే
	color = HL_పట్టిక()[id - 1].sg_gui_bg;

    ఒకవేళ (color == INVALCOLOR)
	తిరిగి 0L;

    తిరిగి GUI_MCH_GET_RGB(color);
}
#అయితే_ముగింపు

/*
 * Output the syntax list header.
 * Return TRUE when started a new line.
 */
    పూర్ణాంకం
syn_list_శీర్షిక(
    పూర్ణాంకం	    did_శీర్షిక,		// did header already
    పూర్ణాంకం	    outlen,		// length of string that comes
    పూర్ణాంకం	    id)			// highlight group id
{
    పూర్ణాంకం	    endcol = 19;
    పూర్ణాంకం	    newline = TRUE;
    పూర్ణాంకం	    name_నిలువు = 0;

    ఒకవేళ (!did_శీర్షిక)
    {
	సందేశం_putchar('\n');
	ఒకవేళ (got_int)
	    తిరిగి TRUE;
	సందేశం_outtrans(HL_పట్టిక()[id - 1].sg_name);
	name_నిలువు = సందేశం_నిలువు;
	endcol = 15;
    }
    లేకపోతే ఒకవేళ (సందేశం_నిలువు + outlen + 1 >= cmdline_వెడల్పు)
    {
	సందేశం_putchar('\n');
	ఒకవేళ (got_int)
	    తిరిగి TRUE;
    }
    లేకపోతే
    {
	ఒకవేళ (సందేశం_నిలువు >= endcol)	// wrap around is like starting a new line
	    newline = FALSE;
    }

    ఒకవేళ (సందేశం_నిలువు >= endcol)	// output at least one space
	endcol = సందేశం_నిలువు + 1;
    ఒకవేళ (cmdline_వెడల్పు <= endcol)	// avoid hang for tiny window
	endcol = cmdline_వెడల్పు - 1;

    సందేశం_advance(endcol);

    // Show "xxx" with the attributes.
    ఒకవేళ (!did_శీర్షిక)
    {
	ఒకవేళ (endcol == cmdline_వెడల్పు - 1 && endcol <= name_నిలువు)
	    సందేశం_putchar(' ');
	సందేశం_puts_attr("xxx", syn_id2attr(id));
	సందేశం_putchar(' ');
    }

    తిరిగి newline;
}

/*
 * Set the attribute numbers for a highlight group.
 * Called after one of the attributes has changed.
 */
    స్థిర శూన్య
సమితి_hl_attr(
    పూర్ణాంకం		idx)	    // index in array
{
    attrentry_T	    at_en;
    hl_సమూహం_T	    *sgp = HL_పట్టిక() + idx;

    // The "Normal" group doesn't need an attribute number
    ఒకవేళ (sgp->sg_name_u != ఖాళీ && STRCMP(sgp->sg_name_u, "NORMAL") == 0)
	తిరిగి;

#నిర్వచితమైతే FEAT_GUI
    // For the GUI mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    ఒకవేళ (sgp->sg_gui_fg == INVALCOLOR
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
    లేకపోతే
    {
	at_en.ae_attr = sgp->sg_gui;
	at_en.ae_u.gui.fg_color = sgp->sg_gui_fg;
	at_en.ae_u.gui.bg_color = sgp->sg_gui_bg;
	at_en.ae_u.gui.sp_color = sgp->sg_gui_sp;
	at_en.ae_u.gui.font = sgp->sg_font;
# ifdef FEAT_XFONTSET
	at_en.ae_u.gui.fontset = sgp->sg_fontset;
# endif
	sgp->sg_gui_attr = get_attr_నమోదు(&gui_attr_పట్టిక, &at_en);
    }
#అయితే_ముగింపు
    // For the term mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    ఒకవేళ (sgp->sg_ప్రారంభం == ఖాళీ && sgp->sg_stop == ఖాళీ)
	sgp->sg_పదం_attr = sgp->sg_పదం;
    లేకపోతే
    {
	at_en.ae_attr = sgp->sg_పదం;
	at_en.ae_u.పదం.ప్రారంభం = sgp->sg_ప్రారంభం;
	at_en.ae_u.పదం.stop = sgp->sg_stop;
	sgp->sg_పదం_attr = get_attr_నమోదు(&పదం_attr_పట్టిక, &at_en);
    }

    // For the color term mode: If there are other than "normal"
    // highlighting attributes, need to allocate an attr number.
    ఒకవేళ (sgp->sg_cterm_fg == 0 && sgp->sg_cterm_bg == 0 &&
	sgp->sg_cterm_ul == 0 && sgp->sg_cterm_font == 0
#నిర్వచితమైతే FEAT_TERMGUICOLORS
	    && sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
#అయితే_ముగింపు
	    )
	sgp->sg_cterm_attr = sgp->sg_cterm;
    లేకపోతే
    {
	at_en.ae_attr = sgp->sg_cterm;
	at_en.ae_u.cterm.fg_color = sgp->sg_cterm_fg;
	at_en.ae_u.cterm.bg_color = sgp->sg_cterm_bg;
	at_en.ae_u.cterm.ul_color = sgp->sg_cterm_ul;
	at_en.ae_u.cterm.font = sgp->sg_cterm_font;
#నిర్వచితమైతే FEAT_TERMGUICOLORS
	at_en.ae_u.cterm.fg_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_fg);
	at_en.ae_u.cterm.bg_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_bg);
	// Only use the underline/undercurl color when used, it may clear the
	// background color if not supported.
	ఒకవేళ (sgp->sg_cterm & (HL_UNDERLINE | HL_UNDERCURL
			   | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED))
	    at_en.ae_u.cterm.ul_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_sp);
	లేకపోతే
	    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	ఒకవేళ (at_en.ae_u.cterm.fg_rgb == INVALCOLOR
		&& at_en.ae_u.cterm.bg_rgb == INVALCOLOR)
	{
	    // If both fg and bg are invalid fall back to the cterm colors.
	    // Helps when the GUI only uses an attribute, e.g. undercurl.
	    at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	    at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
	}
#అయితే_ముగింపు
	sgp->sg_cterm_attr = get_attr_నమోదు(&cterm_attr_పట్టిక, &at_en);
    }
}

/*
 * Check if highlight id is overridden, and return the overriding highlight id.
 * Otherwise return the original id if no override.
 */
    స్థిర పూర్ణాంకం
syn_override(పూర్ణాంకం id)
{
    ఒకవేళ (overrides != ఖాళీ && overrides->arr != ఖాళీ)
	కొరకు (పూర్ణాంకం k = 0; k < overrides->len; k++)
	    ఒకవేళ (overrides->arr[k].from == id)
		తిరిగి overrides->arr[k].గమ్యం;
    తిరిగి id;
}

/*
 * Lookup a highlight group name by pointer and length.
 * If it is not found, 0 is returned.
 */
    స్థిర పూర్ణాంకం
syn_name2id_len(char_u *name, పూర్ణాంకం len)
{
    char_u	name_u[MAX_SYN_NAME + 1];
    hashitem_T	*hi;

    ఒకవేళ (len > MAX_SYN_NAME)
	len = MAX_SYN_NAME;
    mch_memmove(name_u, name, len);
    name_u[len] = NUL;
    vim_strup(name_u);
    ఒకవేళ (!highlight_ht_inited)
	తిరిగి 0;
    hi = hash_కనుగొను(&highlight_ht, name_u);
    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	తిరిగి 0;
    తిరిగి HI2HLNAME(hi)->hn_id;
}

/*
 * Lookup a highlight group name and return its ID.
 * If it is not found, 0 is returned.
 */
    పూర్ణాంకం
syn_name2id(char_u *name)
{
    // Avoid using stricmp() too much, it's slow on some systems.
    తిరిగి syn_name2id_len(name, (పూర్ణాంకం)STRLEN(name));
}

/*
 * Lookup a highlight group name and return its attributes.
 * Return zero if not found.
 */
    పూర్ణాంకం
syn_name2attr(char_u *name)
{
    పూర్ణాంకం id = syn_name2id(name);

    ఒకవేళ (id != 0)
	తిరిగి syn_id2attr(id);
    తిరిగి 0;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return TRUE if highlight group "name" exists.
 */
    పూర్ణాంకం
highlight_exists(char_u *name)
{
    తిరిగి (syn_name2id(name) > 0);
}

# ఒకవేళ defined(FEAT_శోధన_EXTRA)
/*
 * Return the name of highlight group "id".
 * When not a valid ID return an empty string.
 */
    char_u *
syn_id2name(పూర్ణాంకం id)
{
    ఒకవేళ (id <= 0 || id > highlight_ga.ga_len)
	తిరిగి (char_u *)"";
    తిరిగి HL_పట్టిక()[id - 1].sg_name;
}
# endif
#అయితే_ముగింపు

/*
 * Like syn_name2id(), but take a pointer + length argument.
 */
    పూర్ణాంకం
syn_namen2id(char_u *linep, పూర్ణాంకం len)
{
    తిరిగి syn_name2id_len(linep, len);
}

/*
 * Find highlight group name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    పూర్ణాంకం
syn_check_సమూహం(char_u *pp, పూర్ణాంకం len)
{
    పూర్ణాంకం	    id;
    char_u  *name;

    ఒకవేళ (len > MAX_SYN_NAME)
    {
	emsg(_(e_highlight_సమూహం_name_too_long));
	తిరిగి 0;
    }
    id = syn_name2id_len(pp, len);
    ఒకవేళ (id == 0)			// doesn't exist yet
    {
	name = vim_strnsave(pp, len);
	ఒకవేళ (name == ఖాళీ)
	    తిరిగి 0;
	id = syn_కూడు_సమూహం(name);
    }
    తిరిగి id;
}

/*
 * Add new highlight group and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    స్థిర పూర్ణాంకం
syn_కూడు_సమూహం(char_u *name)
{
    char_u	*p;
    char_u	*name_పైకి;

    // Check that the name is valid (ASCII letters, digits, underscores, dots, or hyphens).
    కొరకు (p = name; *p != NUL; ++p)
    {
	ఒకవేళ (!vim_isprintc(*p))
	{
	    emsg(_(e_unprintable_character_లోపల_సమూహం_name));
	    vim_free(name);
	    తిరిగి 0;
	}
	లేకపోతే ఒకవేళ (!ASCII_ISALNUM(*p) && *p != '_' && *p != '.' && *p != '-')
	{
	    // This is an error, but since there previously was no check only
	    // give a warning.
	    సందేశం_ఆధారం(HL_ATTR(HLF_W));
	    సందేశం(_("W18: Invalid character in group name"));
	    ఆపు;
	}
    }

    // First call for this growarray: init growing array.
    ఒకవేళ (highlight_ga.ga_దత్తాంశం == ఖాళీ)
    {
	highlight_ga.ga_itemsize = పరిమాణం(hl_సమూహం_T);
	highlight_ga.ga_growsize = 10;
	hash_init(&highlight_ht);
	highlight_ht_inited = true;
    }

    ఒకవేళ (highlight_ga.ga_len >= MAX_HL_ID)
    {
	emsg(_(e_too_many_highlight_and_syntax_groups));
	vim_free(name);
	తిరిగి 0;
    }

    // Make room for at least one other syntax_highlight entry.
    ఒకవేళ (ga_grow(&highlight_ga, 1) == FAIL)
    {
	vim_free(name);
	తిరిగి 0;
    }

    {
	hlname_T    *hn;
	పూర్ణాంకం	    len = (పూర్ణాంకం)STRLEN(name);

	hn = alloc(offsetof(hlname_T, hn_కీలకం) + len + 1);
	ఒకవేళ (hn == ఖాళీ)
	{
	    vim_free(name);
	    తిరిగి 0;
	}
	vim_strncpy(hn->hn_కీలకం, name, len);
	vim_strup(hn->hn_కీలకం);
	hn->hn_id = highlight_ga.ga_len + 1;	// ID is index plus one
	name_పైకి = hn->hn_కీలకం;
    }

    CLEAR_సూచిక(&(HL_పట్టిక()[highlight_ga.ga_len]));
    HL_పట్టిక()[highlight_ga.ga_len].sg_name = name;
    HL_పట్టిక()[highlight_ga.ga_len].sg_name_u = name_పైకి;
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_పట్టిక()[highlight_ga.ga_len].sg_gui_bg = INVALCOLOR;
    HL_పట్టిక()[highlight_ga.ga_len].sg_gui_fg = INVALCOLOR;
    HL_పట్టిక()[highlight_ga.ga_len].sg_gui_sp = INVALCOLOR;
#అయితే_ముగింపు
    hash_కూడు(&highlight_ht, name_పైకి, "highlight");
    ++highlight_ga.ga_len;

    తిరిగి highlight_ga.ga_len;		    // ID is index plus one
}

/*
 * When, just after calling syn_add_group(), an error is discovered, this
 * function deletes the new name.
 */
    స్థిర శూన్య
syn_unadd_సమూహం(శూన్య)
{
    hashitem_T *hi;

    --highlight_ga.ga_len;
    hi = hash_కనుగొను(&highlight_ht, HL_పట్టిక()[highlight_ga.ga_len].sg_name_u);
    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	hash_remove(&highlight_ht, hi, "highlight");
    vim_free(HL_పట్టిక()[highlight_ga.ga_len].sg_name);
    vim_free(HL_పట్టిక()[highlight_ga.ga_len].sg_name_u - HLNAME_కీలకం_OFF);
}

/*
 * Translate a group ID to highlight attributes.
 * "hl_id" must be valid: > 0, caller must check.
 */
    పూర్ణాంకం
syn_id2attr(పూర్ణాంకం hl_id)
{
    పూర్ణాంకం		attr;
    hl_సమూహం_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ప్రతిజ్ఞా(hl_id > 0);
    sgp = &HL_పట్టిక()[hl_id - 1];	    // index is ID minus one

#నిర్వచితమైతే FEAT_GUI
    // Only use GUI attr when the GUI is being used.
    ఒకవేళ (gui.లోపల_use)
	attr = sgp->sg_gui_attr;
    లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ (IS_CTERM)
	    attr = sgp->sg_cterm_attr;
	లేకపోతే
	    attr = sgp->sg_పదం_attr;

    తిరిగి attr;
}

#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Get the GUI colors and attributes for a group ID.
 * NOTE: the colors will be INVALCOLOR when not set, the color otherwise.
 */
    పూర్ణాంకం
syn_id2colors(పూర్ణాంకం hl_id, guicolor_T *fgp, guicolor_T *bgp)
{
    hl_సమూహం_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ప్రతిజ్ఞా(hl_id > 0);
    sgp = &HL_పట్టిక()[hl_id - 1];	    // index is ID minus one

    *fgp = sgp->sg_gui_fg;
    *bgp = sgp->sg_gui_bg;
    తిరిగి sgp->sg_gui;
}
#అయితే_ముగింపు

#ఒకవేళ (defined(MSWIN) \
	    && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL)) \
	    && defined(FEAT_TERMGUICOLORS)) \
	|| defined(FEAT_TERMINAL)
    శూన్య
syn_id2cterm_bg(పూర్ణాంకం hl_id, పూర్ణాంకం *fgp, పూర్ణాంకం *bgp)
{
    hl_సమూహం_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    ప్రతిజ్ఞా(hl_id > 0);
    sgp = &HL_పట్టిక()[hl_id - 1];	    // index is ID minus one
    *fgp = sgp->sg_cterm_fg - 1;
    *bgp = sgp->sg_cterm_bg - 1;
}
#అయితే_ముగింపు

/*
 * Translate a group ID to the final group ID (following links).
 */
    పూర్ణాంకం
syn_get_final_id(పూర్ణాంకం hl_id)
{
    పూర్ణాంకం		లెక్క;
    hl_సమూహం_T	*sgp;

    ఒకవేళ (hl_id > highlight_ga.ga_len || hl_id < 1)
	తిరిగి 0;			// Can be called from eval!!

    // Follow links until there is no more.
    // Look out for loops!  Break after 100 links.
    కొరకు (లెక్క = 100; --లెక్క >= 0; )
    {
	పూర్ణాంకం tmp = hl_id;

	sgp = &HL_పట్టిక()[hl_id - 1];	    // index is ID minus one
	ఒకవేళ (sgp->sg_link == 0 || sgp->sg_link > highlight_ga.ga_len)
	    ఆపు;

	// This is to handle highlight groups that are overridden but are in the
	// middle of a link chain.
	hl_id = syn_override(hl_id);
	ఒకవేళ (tmp != hl_id)
	    కొనసాగు;
	hl_id = sgp->sg_link;
    }
    తిరిగి syn_override(hl_id);
}

#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Call this function just after the GUI has started.
 * Also called when 'termguicolors' was set, gui.in_use will be FALSE then.
 * It finds the font and color handles for the highlighting groups.
 */
    శూన్య
highlight_gui_started(శూన్య)
{
    పూర్ణాంకం	    idx;

    // First get the colors from the "Normal" and "Menu" group, if set
    ఒకవేళ (USE_24_బిట్టు)
	సమితి_సాధారణ_colors();

    కొరకు (idx = 0; idx < highlight_ga.ga_len; ++idx)
	gui_do_one_color(idx, FALSE, FALSE);

    highlight_మారింది();
}

    స్థిర శూన్య
gui_do_one_color(
    పూర్ణాంకం		idx,
    పూర్ణాంకం		do_menu వాడని,	   // TRUE: might set the menu font
    పూర్ణాంకం		do_tooltip వాడని) // TRUE: might set the tooltip font
{
    పూర్ణాంకం		didit = FALSE;

# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    ఒకవేళ (gui.లోపల_use)
#  endif
	ఒకవేళ (HL_పట్టిక()[idx].sg_font_name != ఖాళీ)
	{
	    hl_do_font(idx, HL_పట్టిక()[idx].sg_font_name, FALSE, do_menu,
							    do_tooltip, TRUE);
	    didit = TRUE;
	}
# endif
    ఒకవేళ (HL_పట్టిక()[idx].sg_gui_fg_name != ఖాళీ)
    {
	HL_పట్టిక()[idx].sg_gui_fg =
			    color_name2handle(HL_పట్టిక()[idx].sg_gui_fg_name);
	didit = TRUE;
    }
    ఒకవేళ (HL_పట్టిక()[idx].sg_gui_bg_name != ఖాళీ)
    {
	HL_పట్టిక()[idx].sg_gui_bg =
			    color_name2handle(HL_పట్టిక()[idx].sg_gui_bg_name);
	didit = TRUE;
    }
    ఒకవేళ (HL_పట్టిక()[idx].sg_gui_sp_name != ఖాళీ)
    {
	HL_పట్టిక()[idx].sg_gui_sp =
			    color_name2handle(HL_పట్టిక()[idx].sg_gui_sp_name);
	didit = TRUE;
    }
    ఒకవేళ (didit)	// need to get a new attr number
	సమితి_hl_attr(idx);
}
#అయితే_ముగింపు

#ఒకవేళ defined(USER_HIGHLIGHT) && defined(FEAT_STL_OPT)
/*
 * Apply difference between User[1-9] and HLF_S to HLF_SNC, HLF_ST or HLF_STNC.
 */
    స్థిర శూన్య
combine_stl_hlt(
	పూర్ణాంకం id,
	పూర్ణాంకం id_S,
	పూర్ణాంకం id_alt,
	పూర్ణాంకం hlcnt,
	పూర్ణాంకం i,
	పూర్ణాంకం hlf,
	పూర్ణాంకం *పట్టిక)
{
    hl_సమూహం_T *hlt = HL_పట్టిక();

    ఒకవేళ (id_alt == 0)
    {
	CLEAR_సూచిక(&hlt[hlcnt + i]);
	hlt[hlcnt + i].sg_పదం = highlight_attr[hlf];
	hlt[hlcnt + i].sg_cterm = highlight_attr[hlf];
# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_EVAL)
	hlt[hlcnt + i].sg_gui = highlight_attr[hlf];
# endif
    }
    లేకపోతే
	mch_memmove(&hlt[hlcnt + i],
		    &hlt[id_alt - 1],
		    పరిమాణం(hl_సమూహం_T));
    hlt[hlcnt + i].sg_link = 0;

    hlt[hlcnt + i].sg_పదం ^=
	hlt[id - 1].sg_పదం ^ hlt[id_S - 1].sg_పదం;
    ఒకవేళ (hlt[id - 1].sg_ప్రారంభం != hlt[id_S - 1].sg_ప్రారంభం)
	hlt[hlcnt + i].sg_ప్రారంభం = hlt[id - 1].sg_ప్రారంభం;
    ఒకవేళ (hlt[id - 1].sg_stop != hlt[id_S - 1].sg_stop)
	hlt[hlcnt + i].sg_stop = hlt[id - 1].sg_stop;
    hlt[hlcnt + i].sg_cterm ^=
	hlt[id - 1].sg_cterm ^ hlt[id_S - 1].sg_cterm;
    ఒకవేళ (hlt[id - 1].sg_cterm_fg != hlt[id_S - 1].sg_cterm_fg)
	hlt[hlcnt + i].sg_cterm_fg = hlt[id - 1].sg_cterm_fg;
    ఒకవేళ (hlt[id - 1].sg_cterm_bg != hlt[id_S - 1].sg_cterm_bg)
	hlt[hlcnt + i].sg_cterm_bg = hlt[id - 1].sg_cterm_bg;
    ఒకవేళ (hlt[id - 1].sg_cterm_font != hlt[id_S - 1].sg_cterm_font)
	hlt[hlcnt + i].sg_cterm_font = hlt[id - 1].sg_cterm_font;
# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_EVAL)
    hlt[hlcnt + i].sg_gui ^=
	hlt[id - 1].sg_gui ^ hlt[id_S - 1].sg_gui;
# endif
# ifdef FEAT_GUI
    ఒకవేళ (hlt[id - 1].sg_gui_fg != hlt[id_S - 1].sg_gui_fg)
	hlt[hlcnt + i].sg_gui_fg = hlt[id - 1].sg_gui_fg;
    ఒకవేళ (hlt[id - 1].sg_gui_bg != hlt[id_S - 1].sg_gui_bg)
	hlt[hlcnt + i].sg_gui_bg = hlt[id - 1].sg_gui_bg;
    ఒకవేళ (hlt[id - 1].sg_gui_sp != hlt[id_S - 1].sg_gui_sp)
	hlt[hlcnt + i].sg_gui_sp = hlt[id - 1].sg_gui_sp;
    ఒకవేళ (hlt[id - 1].sg_font != hlt[id_S - 1].sg_font)
	hlt[hlcnt + i].sg_font = hlt[id - 1].sg_font;
#  ifdef FEAT_XFONTSET
    ఒకవేళ (hlt[id - 1].sg_fontset != hlt[id_S - 1].sg_fontset)
	hlt[hlcnt + i].sg_fontset = hlt[id - 1].sg_fontset;
#  endif
# endif
    highlight_ga.ga_len = hlcnt + i + 1;
    సమితి_hl_attr(hlcnt + i);	// At long last we can apply
    పట్టిక[i] = syn_id2attr(hlcnt + i + 1);
}
#అయితే_ముగింపు

/*
 * Translate the 'highlight' option into attributes in highlight_attr[] and
 * set up the user highlights User1..9.  If FEAT_STL_OPT is in use, a set of
 * corresponding highlights to use on top of HLF_SNC is computed.
 * Called only when the 'highlight' option has been changed and upon first
 * screen redraw after any :highlight command.
 * Return FAIL when an invalid flag is found in 'highlight'.  OK otherwise.
 */
    పూర్ణాంకం
highlight_మారింది(శూన్య)
{
    పూర్ణాంకం		hlf;
    పూర్ణాంకం		i;
    char_u	*p;
    char_u	*default_hl;
    పూర్ణాంకం		attr;
    char_u	*చివర;
    పూర్ణాంకం		id;
#నిర్వచితమైతే USER_HIGHLIGHT
    char_u      userhl[30];  // use 30 to avoid compiler warning
# ifdef FEAT_STL_OPT
    పూర్ణాంకం		id_S = -1;
    పూర్ణాంకం		id_SNC = 0;
#  ifdef FEAT_TERMINAL
    పూర్ణాంకం		id_ST = 0;
    పూర్ణాంకం		id_STNC = 0;
#  endif
    పూర్ణాంకం		hlcnt;
# endif
#అయితే_ముగింపు
    win_T	*wp;

    need_highlight_మారింది = FALSE;

#నిర్వచితమైతే FEAT_TERMINAL
    పదం_update_colors_all();
    పదం_update_hlfwin_all();
#అయితే_ముగింపు

    // Clear all attributes.
    కొరకు (hlf = 0; hlf < (పూర్ణాంకం)HLF_లెక్క; ++hlf)
    {
	highlight_attr[hlf] = 0;
	highlight_attr_raw[hlf] = 0;
	highlight_ids[hlf] = 0;
    }

    default_hl = get_highlight_default();

    // First set all attributes to their default value.
    // Then use the attributes from the 'highlight' option, unless it already
    // matches the default and would repeat the same work.
    కొరకు (i = 0; i < 2; ++i)
    {
	ఒకవేళ (i)
	{
	    ఒకవేళ (default_hl != ఖాళీ && p_hl != ఖాళీ
				      && STRCMP(default_hl, p_hl) == 0)
		కొనసాగు;
	    p = p_hl;
	}
	లేకపోతే
	    p = default_hl;
	ఒకవేళ (p == ఖాళీ)	    // just in case
	    కొనసాగు;

	అయితే (*p)
	{
	    కొరకు (hlf = 0; hlf < (పూర్ణాంకం)HLF_లెక్క; ++hlf)
		ఒకవేళ (hl_flags[hlf] == *p)
		    ఆపు;
	    ++p;
	    ఒకవేళ (hlf == (పూర్ణాంకం)HLF_లెక్క || *p == NUL)
		తిరిగి FAIL;

	    // Allow several hl_flags to be combined, like "bu" for
	    // bold-underlined.
	    attr = 0;
	    id = 0;
	    కొరకు ( ; *p && *p != ','; ++p)	    // parse up to comma
	    {
		ఒకవేళ (VIM_ISWHITE(*p))		    // ignore white space
		    కొనసాగు;

		ఒకవేళ (attr > HL_ALL)  // Combination with ':' is not allowed.
		    తిరిగి FAIL;

		// Note: Keep this in sync with expand_set_highlight().
		ఎంపిక (*p)
		{
		    స్థితి 'b':	attr |= HL_BOLD;
				ఆపు;
		    స్థితి 'i':	attr |= HL_ITALIC;
				ఆపు;
		    స్థితి '-':
		    స్థితి 'n':			    // no highlighting
				ఆపు;
		    స్థితి 'r':	attr |= HL_INVERSE;
				ఆపు;
		    స్థితి 's':	attr |= HL_STANDOUT;
				ఆపు;
		    స్థితి 'u':	attr |= HL_UNDERLINE;
				ఆపు;
		    స్థితి 'c':	attr |= HL_UNDERCURL;
				ఆపు;
		    స్థితి '2':	attr |= HL_UNDERDOUBLE;
				ఆపు;
		    స్థితి 'd':	attr |= HL_UNDERDOTTED;
				ఆపు;
		    స్థితి '=':	attr |= HL_UNDERDASHED;
				ఆపు;
		    స్థితి 't':	attr |= HL_STRIKETHROUGH;
				ఆపు;
		    స్థితి ':':	++p;		    // highlight group name
				ఒకవేళ (attr || *p == NUL)	 // no combinations
				    తిరిగి FAIL;
				చివర = vim_strchr(p, ',');
				ఒకవేళ (చివర == ఖాళీ)
				    చివర = p + STRLEN(p);
				id = syn_check_సమూహం(p, (పూర్ణాంకం)(చివర - p));
				ఒకవేళ (id == 0)
				    తిరిగి FAIL;
				attr = syn_id2attr(id);
				p = చివర - 1;
#ఒకవేళ defined(FEAT_STL_OPT) && defined(USER_HIGHLIGHT)
				ఒకవేళ (hlf == (పూర్ణాంకం)HLF_SNC)
				    id_SNC = syn_get_final_id(id);
# ifdef FEAT_TERMINAL
				లేకపోతే ఒకవేళ (hlf == (పూర్ణాంకం)HLF_ST)
				    id_ST = syn_get_final_id(id);
				లేకపోతే ఒకవేళ (hlf == (పూర్ణాంకం)HLF_STNC)
				    id_STNC = syn_get_final_id(id);
# endif
				లేకపోతే ఒకవేళ (hlf == (పూర్ణాంకం)HLF_S)
				    id_S = syn_get_final_id(id);
#అయితే_ముగింపు
				ఆపు;
		    అప్రమేయ:	తిరిగి FAIL;
		}
	    }
	    highlight_attr[hlf] = attr;
	    highlight_attr_raw[hlf] = attr;
	    highlight_ids[hlf] = id;

	    p = skip_గమ్యం_option_part(p);	    // skip comma and spaces
	}
    }

    // Highlight ids may have been changed, so keep windows up to date
    FOR_ALL_WINDOWS(wp)
    {
	అక్షరం *errmsg = update_winhighlight(wp, wp->w_p_whl);

	ఒకవేళ (errmsg != ఖాళీ)
	    emsg(_(errmsg));
    }

#నిర్వచితమైతే FEAT_TERMINAL
    పదం_update_hlfwin_all();
#అయితే_ముగింపు

#నిర్వచితమైతే USER_HIGHLIGHT
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
    ఒకవేళ (ga_grow(&highlight_ga, 28) == FAIL)
	తిరిగి FAIL;
    hlcnt = highlight_ga.ga_len;
    ఒకవేళ (id_S == -1)
    {
	// Make sure id_S is always valid to simplify code below. Use the last
	// entry.
	CLEAR_సూచిక(&HL_పట్టిక()[hlcnt + 27]);
	HL_పట్టిక()[hlcnt + 18].sg_పదం = highlight_attr[HLF_S];
	id_S = hlcnt + 19;
    }
# endif
    కొరకు (i = 0; i < 9; i++)
    {
	sprintf((అక్షరం *)userhl, "User%d", i + 1);
	id = syn_name2id(userhl);
	ఒకవేళ (id == 0)
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
	లేకపోతే
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

#అయితే_ముగింపు // USER_HIGHLIGHT

    తిరిగి OK;
}

స్థిర శూన్య highlight_list(శూన్య);
స్థిర శూన్య highlight_list_two(పూర్ణాంకం cnt, పూర్ణాంకం attr);

// context for :highlight <group> <arg> expansion
స్థిర పూర్ణాంకం expand_hi_synid = 0;	    // ID for highlight group being completed
స్థిర పూర్ణాంకం expand_hi_equal_నిలువు = 0; // column where the '=' is
స్థిర పూర్ణాంకం expand_hi_include_orig = 0;	    // whether to fill the existing current value or not
స్థిర char_u *expand_hi_curvalue = ఖాళీ;   // the existing current value
#ఒకవేళ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
స్థిర dict_iterator_T expand_colornames_iter;	// iterator for looping through v:colornames
#అయితే_ముగింపు

/*
 * Handle command line completion for :highlight command.
 */
    శూన్య
సమితి_context_లోపల_highlight_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;
    పూర్ణాంకం		expand_సమూహం = TRUE;

    // Default: expand group names
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_నమూనా = arg;
    include_none = 0;
    include_link = 2;
    include_default = 1;

    ఒకవేళ (*arg == NUL)
	తిరిగి;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ఒకవేళ (*p == NUL)
	తిరిగి;

    // past "default" or group name
    include_default = 0;
    ఒకవేళ (STRNCMP("default", arg, p - arg) == 0)
    {
	arg = skipwhite(p);
	xp->xp_నమూనా = arg;
	p = skiptowhite(arg);
    }
    ఒకవేళ (*p == NUL)
	తిరిగి;

    // past group name
    include_link = 0;
    ఒకవేళ (arg[1] == 'i' && arg[0] == 'N')
    {
	highlight_list();
	expand_సమూహం = FALSE;
    }
    ఒకవేళ (STRNCMP("link", arg, p - arg) == 0)
    {
	xp->xp_నమూనా = skipwhite(p);
	p = skiptowhite(xp->xp_నమూనా);
	ఒకవేళ (*p != NUL)		// past first group name
	{
	    xp->xp_నమూనా = skipwhite(p);
	    p = skiptowhite(xp->xp_నమూనా);
	    include_none = 1;
	}
	expand_సమూహం = FALSE;
    }
    లేకపోతే ఒకవేళ (STRNCMP("clear", arg, p - arg) == 0)
    {
	xp->xp_నమూనా = skipwhite(p);
	p = skiptowhite(xp->xp_నమూనా);
	expand_సమూహం = FALSE;
    }
    ఒకవేళ (*p != NUL)			// past group name(s)
    {
	ఒకవేళ (expand_సమూహం)
	{
	    // expansion will be done in expand_highlight_group()
	    xp->xp_context = EXPAND_HIGHLIGHT_సమూహం;

	    expand_hi_synid = syn_namen2id(arg, (పూర్ణాంకం)(p - arg));

	    అయితే (*p != NUL)
	    {
		arg = skipwhite(p);
		p = skiptowhite(arg);
	    }

	    p = vim_strchr(arg, '=');
	    ఒకవేళ (p == ఖాళీ)
	    {
		// Didn't find a key=<value> pattern
		xp->xp_నమూనా = arg;
		expand_hi_equal_నిలువు = -1;
		expand_hi_include_orig = FALSE;
	    }
	    లేకపోతే
	    {
		// Found key=<value> pattern, record the exact location
		expand_hi_equal_నిలువు = (పూర్ణాంకం)(p - xp->xp_పంక్తి);

		// Only include the original value if the pattern is empty
		ఒకవేళ (*(p + 1) == NUL)
		    expand_hi_include_orig = TRUE;
		లేకపోతే
		    expand_hi_include_orig = FALSE;

		// Account for comma-separated values
		ఒకవేళ (STRNCMP(arg, "term=", 5) == 0 ||
			STRNCMP(arg, "cterm=", 6) == 0 ||
			STRNCMP(arg, "gui=", 4) == 0)
		{
		    char_u *comma = vim_strrchr(p + 1, ',');
		    ఒకవేళ (comma != ఖాళీ)
			p = comma;
		}
		xp->xp_నమూనా = p + 1;
	    }
	}
	లేకపోతే
	{
	    xp->xp_context = EXPAND_NOTHING;
	}
    }
}

/*
 * List highlighting matches in a nice way.
 */
    స్థిర శూన్య
highlight_list(శూన్య)
{
    పూర్ణాంకం		i;

    కొరకు (i = 10; --i >= 0; )
	highlight_list_two(i, HL_ATTR(HLF_D));
    కొరకు (i = 40; --i >= 0; )
	highlight_list_two(99, 0);
}

    స్థిర శూన్య
highlight_list_two(పూర్ణాంకం cnt, పూర్ణాంకం attr)
{
    సందేశం_puts_attr(&("N \bI \b!  \b"[cnt / 11]), attr);
    సందేశం_clr_eos();
    బహిర్గతం_flush();
    ui_delay(cnt == 99 ? 40L : (దీర్ఘ)cnt * 50L, FALSE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_highlight_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    తిరిగి get_highlight_name_ext(xp, idx, TRUE);
}

/*
 * Obtain a highlight group name.
 * When "skip_cleared" is TRUE don't return a cleared entry.
 */
    char_u *
get_highlight_name_ext(expand_T *xp వాడని, పూర్ణాంకం idx, పూర్ణాంకం skip_cleared)
{
    ఒకవేళ (idx < 0)
	తిరిగి ఖాళీ;

    // Items are never removed from the table, skip the ones that were
    // cleared.
    ఒకవేళ (skip_cleared && idx < highlight_ga.ga_len && HL_పట్టిక()[idx].sg_cleared)
	తిరిగి (char_u *)"";

    ఒకవేళ (idx == highlight_ga.ga_len && include_none != 0)
	తిరిగి (char_u *)"NONE";
    ఒకవేళ (idx == highlight_ga.ga_len + include_none && include_default != 0)
	తిరిగి (char_u *)"default";
    ఒకవేళ (idx == highlight_ga.ga_len + include_none + include_default
							 && include_link != 0)
	తిరిగి (char_u *)"link";
    ఒకవేళ (idx == highlight_ga.ga_len + include_none + include_default + 1
							 && include_link != 0)
	తిరిగి (char_u *)"clear";
    ఒకవేళ (idx >= highlight_ga.ga_len)
	తిరిగి ఖాళీ;
    తిరిగి HL_పట్టిక()[idx].sg_name;
}

    స్థిర char_u *
get_highlight_attr_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx == 0)
    {
	// Fill with current value first
	ఒకవేళ (expand_hi_curvalue != ఖాళీ)
	    తిరిగి expand_hi_curvalue;
	లేకపోతే
	    తిరిగి (char_u*)"";
    }
    ఒకవేళ (idx < (పూర్ణాంకం)ARRAY_నిడివి(highlight_సూచీ_tab) + 1)
    {
	char_u *విలువ = highlight_సూచీ_tab[idx-1]->విలువ.string;
	ఒకవేళ (expand_hi_curvalue != ఖాళీ && STRCMP(expand_hi_curvalue, విలువ) == 0)
	{
	    // Already returned the current value above, just skip.
	    తిరిగి (char_u*)"";
	}
	తిరిగి విలువ;
    }
    తిరిగి ఖాళీ;
}

    స్థిర char_u *
get_highlight_cterm_color(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx == 0)
    {
	// Fill with current value first
	ఒకవేళ (expand_hi_curvalue != ఖాళీ)
	    తిరిగి expand_hi_curvalue;
	లేకపోతే
	    తిరిగి (char_u*)"";
    }
    // See highlight_set_cterm_color()
    లేకపోతే ఒకవేళ (idx == 1)
	తిరిగి (char_u*)"fg";
    లేకపోతే ఒకవేళ (idx == 2)
	తిరిగి (char_u*)"bg";
    ఒకవేళ (idx < (పూర్ణాంకం)ARRAY_నిడివి(color_name_tab) + 3)
    {
	char_u *విలువ = color_name_tab[idx-3].విలువ.string;
	తిరిగి విలువ;
    }
    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
    స్థిర char_u *
get_highlight_gui_color(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx == 0)
    {
	// Fill with current value first
	ఒకవేళ (expand_hi_curvalue != ఖాళీ)
	    తిరిగి expand_hi_curvalue;
	లేకపోతే
	    తిరిగి (char_u*)"";
    }
    // See color_name2handle()
    లేకపోతే ఒకవేళ (idx == 1)
	తిరిగి (char_u*)"fg";
    లేకపోతే ఒకవేళ (idx == 2)
	తిరిగి (char_u*)"bg";
    లేకపోతే ఒకవేళ (idx == 3)
	తిరిగి (char_u*)"NONE";

    // Complete from v:colornames. Don't do platform specific names for now.
    typval_T *tv_ఫలితం;
    char_u *colorname = dict_iterate_తదుపరి(&expand_colornames_iter, &tv_ఫలితం);
    ఒకవేళ (colorname != ఖాళీ)
    {
	// :hi command doesn't allow space, so don't suggest any malformed items
	ఒకవేళ (vim_strchr(colorname, ' ') != ఖాళీ)
	    తిరిగి (char_u*)"";

	ఒకవేళ (expand_hi_curvalue != ఖాళీ && STRICMP(expand_hi_curvalue, colorname) == 0)
	{
	    // Already returned the current value above, just skip.
	    తిరిగి (char_u*)"";
	}
    }
    తిరిగి colorname;
}
#అయితే_ముగింపు

    స్థిర char_u *
get_highlight_సమూహం_కీలకం(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    // Note: Keep this in sync with do_highlight.
    స్థిర అక్షరం *(p_hi_సమూహం_కీలకం_విలువలు[]) =
    {
	"term=",
	"start=",
	"stop=",
	"cterm=",
	"ctermfg=",
	"ctermbg=",
	"ctermul=",
	"ctermfont=",
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	"gui=",
	"guifg=",
	"guibg=",
	"guisp=",
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
	"font=",
#అయితే_ముగింపు
	"NONE",
    };

    ఒకవేళ (idx < (పూర్ణాంకం)ARRAY_నిడివి(p_hi_సమూహం_కీలకం_విలువలు))
	తిరిగి (char_u*)p_hi_సమూహం_కీలకం_విలువలు[idx];
    తిరిగి ఖాళీ;
}

/*
 * Command-line expansion for :hi {group-name} <args>...
 */
    పూర్ణాంకం
expand_highlight_సమూహం(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	పూర్ణాంకం	    *అంకె_Matches)
{
    ఒకవేళ (expand_hi_equal_నిలువు != -1)
    {
	// List the values. First fill in the current value, then if possible colors
	// or attribute names.
	char_u	    *(*expandfunc)(expand_T *, పూర్ణాంకం) = ఖాళీ;
	పూర్ణాంకం	    type = 0;
	hl_సమూహం_T  *sgp = ఖాళీ;
	పూర్ణాంకం	    iarg = 0;
	char_u	    *sarg = ఖాళీ;

	పూర్ణాంకం	    unsorted_వస్తువులు = -1; // don't sort by default

	ఒకవేళ (expand_hi_synid != 0)
	    sgp = &HL_పట్టిక()[expand_hi_synid - 1]; // index is ID minus one

	// Note: Keep this in sync with highlight_list_one().
	char_u	    *name_చివర = xp->xp_పంక్తి + expand_hi_equal_నిలువు;
	ఒకవేళ (name_చివర - xp->xp_పంక్తి >= 5
		&& STRNCMP(name_చివర - 5, " term", 5) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ఒకవేళ (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_పదం;
	    }
	}
	లేకపోతే ఒకవేళ (name_చివర - xp->xp_పంక్తి >= 6
		&& STRNCMP(name_చివర - 6, " cterm", 6) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ఒకవేళ (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_cterm;
	    }
	}
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	లేకపోతే ఒకవేళ (name_చివర - xp->xp_పంక్తి >= 4
		&& STRNCMP(name_చివర - 4, " gui", 4) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    ఒకవేళ (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_gui;
	    }
	}
#అయితే_ముగింపు
	లేకపోతే ఒకవేళ (name_చివర - xp->xp_పంక్తి >= 8
		&& STRNCMP(name_చివర - 8, " ctermfg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ఒకవేళ (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_fg;
	    }
	}
	లేకపోతే ఒకవేళ (name_చివర - xp->xp_పంక్తి >= 8
		&& STRNCMP(name_చివర - 8, " ctermbg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ఒకవేళ (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_bg;
	    }
	}
	లేకపోతే ఒకవేళ (name_చివర - xp->xp_పంక్తి >= 8
		&& STRNCMP(name_చివర - 8, " ctermul", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    ఒకవేళ (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_ul;
	    }
	}
#ఒకవేళ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	లేకపోతే ఒకవేళ (name_చివర - xp->xp_పంక్తి >= 6
		&& STRNCMP(name_చివర - 6, " guifg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ఒకవేళ (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_fg_name;
	    }
	}
	లేకపోతే ఒకవేళ (name_చివర - xp->xp_పంక్తి >= 6
		&& STRNCMP(name_చివర - 6, " guibg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ఒకవేళ (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_bg_name;
	    }
	}
	లేకపోతే ఒకవేళ (name_చివర - xp->xp_పంక్తి >= 6
		&& STRNCMP(name_చివర - 6, " guisp", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    ఒకవేళ (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_sp_name;
	    }
	}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	ఒకవేళ (expandfunc == get_highlight_gui_color)
	{
	    // Top 4 items are special, after that sort all the color names
	    unsorted_వస్తువులు = 4;

	    dict_T *colornames_పట్టిక = get_vim_var_dict(VV_COLORNAMES);
	    typval_T colornames_val;
	    colornames_val.v_type = VAR_DICT;
	    colornames_val.vval.v_dict = colornames_పట్టిక;
	    dict_iterate_ప్రారంభం(&colornames_val, &expand_colornames_iter);
	}
#అయితే_ముగింపు

	char_u	    buf[MAX_ATTR_LEN];

	expand_hi_curvalue = ఖాళీ;
	ఒకవేళ (expand_hi_include_orig)
	{
	    ఒకవేళ (((type == LIST_ATTR || type == LIST_INT) && iarg != 0) ||
		(type == LIST_STRING && sarg != ఖాళీ))
	    {
		// Retrieve the current value to go first in completion
		expand_hi_curvalue = highlight_arg_గమ్యం_string(
			type, iarg, sarg, buf);
	    }
	}

	ఒకవేళ (expandfunc != ఖాళీ)
	{
	    తిరిగి ExpandGenericExt(
		    pat,
		    xp,
		    rmp,
		    matches,
		    అంకె_Matches,
		    expandfunc,
		    FALSE,
		    unsorted_వస్తువులు);
	}

	తిరిగి FAIL;
    }

    // List all the key names
    తిరిగి ExpandGenericExt(
	    pat,
	    xp,
	    rmp,
	    matches,
	    అంకె_Matches,
	    get_highlight_సమూహం_కీలకం,
	    FALSE,
	    -1);
}

#ఒకవేళ defined(FEAT_GUI)
/*
 * Free all the highlight group fonts.
 * Used when quitting for systems which need it.
 */
    శూన్య
free_highlight_fonts(శూన్య)
{
    పూర్ణాంకం	    idx;

    కొరకు (idx = 0; idx < highlight_ga.ga_len; ++idx)
    {
	gui_mch_free_font(HL_పట్టిక()[idx].sg_font);
	HL_పట్టిక()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
	gui_mch_free_fontset(HL_పట్టిక()[idx].sg_fontset);
	HL_పట్టిక()[idx].sg_fontset = NOFONTSET;
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
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Convert each of the highlight attribute bits (bold, standout, underline,
 * etc.) set in 'hlattr' into a separate boolean item in a Dictionary with
 * the attribute name as the key.
 */
    స్థిర dict_T *
highlight_get_attr_dict(పూర్ణాంకం hlattr)
{
    dict_T	*dict;
    పూర్ణాంకం		i;

    dict = dict_alloc();
    ఒకవేళ (dict == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(highlight_సూచీ_tab); ++i)
    {
	ఒకవేళ (hlattr & highlight_సూచీ_tab[i]->కీలకం)
	{
	    dict_కూడు_bool(dict, (అక్షరం *)highlight_సూచీ_tab[i]->విలువ.string,
		    VVAL_TRUE);
	    // don't want "inverse"/"reverse"
	    hlattr &= ~highlight_సూచీ_tab[i]->కీలకం;
	}
    }

    తిరిగి dict;
}

/*
 * Return the attributes of the highlight group at index 'hl_idx' as a
 * Dictionary. If 'resolve_link' is TRUE, then resolves the highlight group
 * links recursively.
 */
    స్థిర dict_T *
highlight_get_info(పూర్ణాంకం hl_idx, పూర్ణాంకం resolve_link)
{
    dict_T	*dict;
    hl_సమూహం_T	*sgp;
    dict_T	*attr_dict = ఖాళీ;
    పూర్ణాంకం		hlgid;

    dict = dict_alloc();
    ఒకవేళ (dict == ఖాళీ)
	తిరిగి dict;

    sgp = &HL_పట్టిక()[hl_idx];
    // highlight group id is 1-based
    hlgid = hl_idx + 1;

    ఒకవేళ (dict_కూడు_string(dict, "name", sgp->sg_name) == FAIL)
	వెళ్ళు error;
    ఒకవేళ (dict_కూడు_సంఖ్య(dict, "id", hlgid) == FAIL)
	వెళ్ళు error;

    ఒకవేళ (resolve_link)
    {
	// resolve the highlight group link recursively
	అయితే (sgp->sg_link)
	{
	    hlgid = sgp->sg_link;
	    sgp = &HL_పట్టిక()[sgp->sg_link - 1];
	}
    }

    ఒకవేళ (sgp->sg_పదం != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_పదం);
	ఒకవేళ (attr_dict != ఖాళీ)
	{
	    ఒకవేళ (dict_కూడు_dict(dict, "term", attr_dict) == FAIL)
		వెళ్ళు error;
	    attr_dict = ఖాళీ;
	}
    }
    ఒకవేళ (sgp->sg_ప్రారంభం != ఖాళీ)
	ఒకవేళ (dict_కూడు_string(dict, "start", sgp->sg_ప్రారంభం) == FAIL)
	    వెళ్ళు error;
    ఒకవేళ (sgp->sg_stop != ఖాళీ)
	ఒకవేళ (dict_కూడు_string(dict, "stop", sgp->sg_stop) == FAIL)
	    వెళ్ళు error;
    ఒకవేళ (sgp->sg_cterm != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_cterm);
	ఒకవేళ (attr_dict != ఖాళీ)
	{
	    ఒకవేళ (dict_కూడు_dict(dict, "cterm", attr_dict) == FAIL)
		వెళ్ళు error;
	    attr_dict = ఖాళీ;
	}
    }
    ఒకవేళ (sgp->sg_cterm_fg != 0)
	ఒకవేళ (dict_కూడు_string(dict, "ctermfg",
		    highlight_color(hlgid, (char_u *)"fg", 'c')) == FAIL)
	    వెళ్ళు error;
    ఒకవేళ (sgp->sg_cterm_bg != 0)
	ఒకవేళ (dict_కూడు_string(dict, "ctermbg",
			highlight_color(hlgid, (char_u *)"bg", 'c')) == FAIL)
	    వెళ్ళు error;
    ఒకవేళ (sgp->sg_cterm_ul != 0)
	ఒకవేళ (dict_కూడు_string(dict, "ctermul",
			highlight_color(hlgid, (char_u *)"ul", 'c')) == FAIL)
	    వెళ్ళు error;
    ఒకవేళ (sgp->sg_cterm_font != 0)
	ఒకవేళ (dict_కూడు_string(dict, "ctermfont",
			highlight_color(hlgid, (char_u *)"font", 'c')) == FAIL)
	    వెళ్ళు error;
    ఒకవేళ (sgp->sg_gui != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_gui);
	ఒకవేళ (attr_dict != ఖాళీ)
	{
	    ఒకవేళ (dict_కూడు_dict(dict, "gui", attr_dict) == FAIL)
		వెళ్ళు error;
	    attr_dict = ఖాళీ;
	}
    }
    ఒకవేళ (sgp->sg_gui_fg_name != ఖాళీ)
	ఒకవేళ (dict_కూడు_string(dict, "guifg",
			highlight_color(hlgid, (char_u *)"fg", 'g')) == FAIL)
	    వెళ్ళు error;
    ఒకవేళ (sgp->sg_gui_bg_name != ఖాళీ)
	ఒకవేళ (dict_కూడు_string(dict, "guibg",
			highlight_color(hlgid, (char_u *)"bg", 'g')) == FAIL)
	    వెళ్ళు error;
    ఒకవేళ (sgp->sg_gui_sp_name != ఖాళీ)
	ఒకవేళ (dict_కూడు_string(dict, "guisp",
			highlight_color(hlgid, (char_u *)"sp", 'g')) == FAIL)
	    వెళ్ళు error;
# ifdef FEAT_GUI
    ఒకవేళ (sgp->sg_font_name != ఖాళీ)
	ఒకవేళ (dict_కూడు_string(dict, "font", sgp->sg_font_name) == FAIL)
	    వెళ్ళు error;
# endif
    ఒకవేళ (sgp->sg_link)
    {
	char_u	*link;

	link = HL_పట్టిక()[sgp->sg_link - 1].sg_name;
	ఒకవేళ (link != ఖాళీ && dict_కూడు_string(dict, "linksto", link) == FAIL)
	    వెళ్ళు error;

	ఒకవేళ (sgp->sg_deflink)
	    dict_కూడు_bool(dict, "default", VVAL_TRUE);
    }
    ఒకవేళ (dict_len(dict) == 2)
	// If only 'name' is present, then the highlight group is cleared.
	dict_కూడు_bool(dict, "cleared", VVAL_TRUE);

    తిరిగి dict;

error:
    dict_unref(attr_dict);
    dict_unref(dict);
    తిరిగి ఖాళీ;
}

/*
 * "hlget([name])" function
 * Return the attributes of a specific highlight group (if specified) or all
 * the highlight groups.
 */
    శూన్య
f_hlget(typval_T *argvars, typval_T *rettv)
{
    list_T	*list;
    dict_T	*dict;
    పూర్ణాంకం		i;
    char_u	*hlarg = ఖాళీ;
    పూర్ణాంకం		resolve_link = FALSE;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (check_for_opt_string_arg(argvars, 0) == FAIL
	    || (argvars[0].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// highlight group name supplied
	hlarg = tv_get_string_chk(&argvars[0]);
	ఒకవేళ (hlarg == ఖాళీ)
	    తిరిగి;

	ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	{
	    పూర్ణాంకం error = FALSE;

	    resolve_link = tv_get_bool_chk(&argvars[1], &error);
	    ఒకవేళ (error)
		తిరిగి;
	}
    }

    list = rettv->vval.v_list;
    కొరకు (i = 0; i < highlight_ga.ga_len && !got_int; ++i)
    {
	ఒకవేళ (hlarg == ఖాళీ || STRICMP(hlarg, HL_పట్టిక()[i].sg_name) == 0)
	{
	    dict = highlight_get_info(i, resolve_link);
	    ఒకవేళ (dict != ఖాళీ && list_append_dict(list, dict) == FAIL)
		dict_unref(dict);
	}
    }
}

/*
 * Returns the string value at 'dict[key]'. Returns NULL, if 'key' is not in
 * 'dict' or the value is not a string type. If the value is not a string type
 * or is NULL, then 'error' is set to TRUE.
 */
    స్థిర char_u *
hldict_get_string(dict_T *dict, char_u *కీలకం, పూర్ణాంకం *error)
{
    dictitem_T	*di;

    *error = FALSE;
    di = dict_కనుగొను(dict, కీలకం, -1);
    ఒకవేళ (di == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (di->di_tv.v_type != VAR_STRING || di->di_tv.vval.v_string == ఖాళీ)
    {
	emsg(_(e_string_required));
	*error = TRUE;
	తిరిగి ఖాళీ;
    }

    తిరిగి di->di_tv.vval.v_string;
}

/*
 * Convert the highlight attribute Dictionary at 'dict[key]' into a string
 * value in 'attr_str' of length 'len'. Returns FALSE if 'dict[key]' is not a
 * Dictionary or is NULL.
 */
    స్థిర పూర్ణాంకం
hldict_attr_గమ్యం_str(
	dict_T	*dict,
	char_u	*కీలకం,
	char_u	*attr_str,
	size_t	len)
{
    dictitem_T	*di;
    dict_T	*attrdict;
    పూర్ణాంకం		i;
    char_u	*p;

    attr_str[0] = NUL;
    di = dict_కనుగొను(dict, కీలకం, -1);
    ఒకవేళ (di == ఖాళీ)
	తిరిగి TRUE;

    ఒకవేళ (di->di_tv.v_type != VAR_DICT || di->di_tv.vval.v_dict == ఖాళీ)
    {
	emsg(_(e_dictionary_required));
	తిరిగి FALSE;
    }

    attrdict = di->di_tv.vval.v_dict;

    // If the attribute dict is empty, then return NONE to clear the attributes
    ఒకవేళ (dict_len(attrdict) == 0)
    {
	vim_strcat(attr_str, (char_u *)"NONE", len);
	తిరిగి TRUE;
    }

    p = attr_str;
    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(highlight_tab); ++i)
    {
	ఒకవేళ (dict_get_bool(attrdict, (అక్షరం *)highlight_tab[i].విలువ.string,
		    VVAL_FALSE) == VVAL_TRUE)
	{
	    ఒకవేళ (p != attr_str && (size_t)(p - attr_str + 2) < len)
		STRCPY(p, (char_u *)",");
	    ఒకవేళ (p - attr_str + highlight_tab[i].విలువ.నిడివి + 1 < len)
	    {
		STRCPY(p, highlight_tab[i].విలువ.string);
		p += highlight_tab[i].విలువ.నిడివి;
	    }
	}
    }

    తిరిగి TRUE;
}

// Temporary buffer used to store the command string produced by hlset().
// IObuff cannot be used for this as the error messages produced by hlset()
// internally use IObuff.
# define	HLSETBUFSZ  512
స్థిర char_u hlsetBuf[HLSETBUFSZ + 1];

/*
 * Add the highlight attribute "attr" of length "attrlen" and "value" at
 * "dptr", which points into "hlsetBuf".
 * Returns the updated pointer.
 */
    స్థిర char_u *
కూడు_attr_and_విలువ(char_u *dptr, char_u *attr, పూర్ణాంకం attrlen, char_u *విలువ)
{
    size_t	vallen;

    // Do nothing if the value is not specified or is empty
    ఒకవేళ (విలువ == ఖాళీ || *విలువ == NUL)
	తిరిగి dptr;

    vallen = STRLEN(విలువ);
    // When the value contains a space and the attribute has an "=" (i.e. it
    // is a key=value pair), surround the value with single quotes so that
    // do_highlight() can parse it correctly.
    ఒకవేళ (vim_strchr(విలువ, ' ') != ఖాళీ && vim_strchr(attr, '=') != ఖాళీ)
    {
	ఒకవేళ (dptr + attrlen + vallen + 3 < hlsetBuf + HLSETBUFSZ)
	{
	    STRCPY(dptr, attr);
	    dptr += attrlen;
	    *dptr++ = '\'';
	    STRCPY(dptr, విలువ);
	    dptr += vallen;
	    *dptr++ = '\'';
	    *dptr = NUL;
	}
    }
    లేకపోతే ఒకవేళ (dptr + attrlen + vallen + 1 < hlsetBuf + HLSETBUFSZ)
    {
	STRCPY(dptr, attr);
	dptr += attrlen;
	STRCPY(dptr, విలువ);
	dptr += vallen;
    }

    తిరిగి dptr;
}

/*
 * Add or update a highlight group using 'dict' items. Returns TRUE if
 * successfully updated the highlight group.
 */
    స్థిర పూర్ణాంకం
hlg_కూడు_or_update(dict_T *dict)
{
    char_u	*name;
    పూర్ణాంకం		error;
    char_u	పదం_attr[MAX_ATTR_LEN];
    char_u	cterm_attr[MAX_ATTR_LEN];
    char_u	gui_attr[MAX_ATTR_LEN];
    char_u	*ప్రారంభం;
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
    పూర్ణాంకం		forceit = FALSE;
    పూర్ణాంకం		dodefault = FALSE;
    పూర్ణాంకం		పూర్తి = FALSE;
    char_u	*p;

    name = hldict_get_string(dict, (char_u *)"name", &error);
    ఒకవేళ (name == ఖాళీ || *name == NUL || error)
	తిరిగి FALSE;

    ఒకవేళ (dict_get_bool(dict, "force", VVAL_FALSE) == VVAL_TRUE)
	forceit = TRUE;

    ఒకవేళ (dict_get_bool(dict, "default", VVAL_FALSE) == VVAL_TRUE)
	dodefault = TRUE;

    ఒకవేళ (dict_has_కీలకం(dict, "cleared"))
    {
	varnumber_T	cleared;

	// clear a highlight group
	cleared = dict_get_bool(dict, "cleared", FALSE);
	ఒకవేళ (cleared == TRUE)
	{
	    vim_snprintf((అక్షరం *)hlsetBuf, HLSETBUFSZ, "clear %s", name);
	    do_highlight(hlsetBuf, forceit, FALSE);
	    పూర్తి = TRUE;
	}
    }

    ఒకవేళ (dict_has_కీలకం(dict, "linksto"))
    {
	char_u	*linksto;

	// link highlight groups
	linksto = hldict_get_string(dict, (char_u *)"linksto", &error);
	ఒకవేళ (linksto == ఖాళీ || *linksto == NUL || error)
	    తిరిగి FALSE;

	vim_snprintf((అక్షరం *)hlsetBuf, HLSETBUFSZ, "%slink %s %s",
				dodefault ? "default " : "", name, linksto);
	do_highlight(hlsetBuf, forceit, FALSE);

	పూర్తి = TRUE;
    }

    // If 'cleared' or 'linksto' are specified, then don't process the other
    // attributes.
    ఒకవేళ (పూర్తి)
	తిరిగి TRUE;

    ప్రారంభం = hldict_get_string(dict, (char_u *)"start", &error);
    ఒకవేళ (error)
	తిరిగి FALSE;

    stop = hldict_get_string(dict, (char_u *)"stop", &error);
    ఒకవేళ (error)
	తిరిగి FALSE;

    ఒకవేళ (!hldict_attr_గమ్యం_str(dict, (char_u *)"term", పదం_attr,
							    పరిమాణం(పదం_attr)))
	తిరిగి FALSE;

    ఒకవేళ (!hldict_attr_గమ్యం_str(dict, (char_u *)"cterm", cterm_attr,
							   పరిమాణం(cterm_attr)))
	తిరిగి FALSE;

    ctermfg = hldict_get_string(dict, (char_u *)"ctermfg", &error);
    ఒకవేళ (error)
	తిరిగి FALSE;

    ctermbg = hldict_get_string(dict, (char_u *)"ctermbg", &error);
    ఒకవేళ (error)
	తిరిగి FALSE;

    ctermul = hldict_get_string(dict, (char_u *)"ctermul", &error);
    ఒకవేళ (error)
	తిరిగి FALSE;

    ctermfont = hldict_get_string(dict, (char_u *)"ctermfont", &error);
    ఒకవేళ (error)
	తిరిగి FALSE;

    ఒకవేళ (!hldict_attr_గమ్యం_str(dict, (char_u *)"gui", gui_attr, పరిమాణం(gui_attr)))
	తిరిగి FALSE;

    guifg = hldict_get_string(dict, (char_u *)"guifg", &error);
    ఒకవేళ (error)
	తిరిగి FALSE;

    guibg = hldict_get_string(dict, (char_u *)"guibg", &error);
    ఒకవేళ (error)
	తిరిగి FALSE;

    guisp = hldict_get_string(dict, (char_u *)"guisp", &error);
    ఒకవేళ (error)
	తిరిగి FALSE;

# ifdef FEAT_GUI
    font = hldict_get_string(dict, (char_u *)"font", &error);
    ఒకవేళ (error)
	తిరిగి FALSE;
# endif

    // If none of the attributes are specified, then do nothing.
    ఒకవేళ (పదం_attr[0] == NUL && ప్రారంభం == ఖాళీ && stop == ఖాళీ
	    && cterm_attr[0] == NUL && ctermfg == ఖాళీ && ctermbg == ఖాళీ
	    && ctermul == ఖాళీ && ctermfont == ఖాళీ && gui_attr[0] == NUL
# ifdef FEAT_GUI
	    && font == ఖాళీ
# endif
	    && guifg == ఖాళీ && guibg == ఖాళీ && guisp == ఖాళీ
	    )
	తిరిగి TRUE;

    hlsetBuf[0] = NUL;
    p = hlsetBuf;
    ఒకవేళ (dodefault)
	p = కూడు_attr_and_విలువ(p, (char_u *)"default", 7, (char_u *)" ");
    p = కూడు_attr_and_విలువ(p, (char_u *)"", 0, name);
    p = కూడు_attr_and_విలువ(p, (char_u *)" term=", 6, పదం_attr);
    p = కూడు_attr_and_విలువ(p, (char_u *)" start=", 7, ప్రారంభం);
    p = కూడు_attr_and_విలువ(p, (char_u *)" stop=", 6, stop);
    p = కూడు_attr_and_విలువ(p, (char_u *)" cterm=", 7, cterm_attr);
    p = కూడు_attr_and_విలువ(p, (char_u *)" ctermfg=", 9, ctermfg);
    p = కూడు_attr_and_విలువ(p, (char_u *)" ctermbg=", 9, ctermbg);
    p = కూడు_attr_and_విలువ(p, (char_u *)" ctermul=", 9, ctermul);
    p = కూడు_attr_and_విలువ(p, (char_u *)" ctermfont=", 11, ctermfont);
    p = కూడు_attr_and_విలువ(p, (char_u *)" gui=", 5, gui_attr);
# ifdef FEAT_GUI
    p = కూడు_attr_and_విలువ(p, (char_u *)" font=", 6, font);
# endif
    p = కూడు_attr_and_విలువ(p, (char_u *)" guifg=", 7, guifg);
    p = కూడు_attr_and_విలువ(p, (char_u *)" guibg=", 7, guibg);
    (శూన్య)కూడు_attr_and_విలువ(p, (char_u *)" guisp=", 7, guisp);

    do_highlight(hlsetBuf, forceit, FALSE);

    తిరిగి TRUE;
}

/*
 * "hlset([{highlight_attr}])" function
 * Add or modify highlight groups
 */
    శూన్య
f_hlset(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    dict_T	*dict;

    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (check_for_list_arg(argvars, 0) == FAIL)
	తిరిగి;

    FOR_ALL_LIST_వస్తువులు(argvars->vval.v_list, li)
    {
	ఒకవేళ (li->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_dictionary_required));
	    తిరిగి;
	}

	dict = li->li_tv.vval.v_dict;
	ఒకవేళ (!hlg_కూడు_or_update(dict))
	    తిరిగి;
    }

    rettv->vval.v_సంఖ్య = 0;
}
#అయితే_ముగింపు

/*
 * If "old" is in the override stack, then update it to "hl_new". Does not free
 * "old".
 */
    శూన్య
update_highlight_overrides(hl_override_T *old, hl_override_T *hl_new, పూర్ణాంకం newlen)
{
    ఒకవేళ (old == ఖాళీ)
	తిరిగి;

    కొరకు (hl_overrides_T *సమితి_2 = overrides; సమితి_2 != ఖాళీ; సమితి_2 = సమితి_2->తదుపరి)
    {
	ఒకవేళ (సమితి_2->arr == old)
	{
	    సమితి_2->arr = hl_new;
	    సమితి_2->len = newlen;
	}
    }
}

/*
 * If "arr" is in the highlight overrides list, then mark it as invalid.
 */
    శూన్య
remove_highlight_overrides(hl_override_T *arr)
{
    ఒకవేళ (arr == ఖాళీ || overrides == ఖాళీ)
	తిరిగి;

    కొరకు (hl_overrides_T *సమితి_2 = overrides; సమితి_2 != ఖాళీ; సమితి_2 = సమితి_2->తదుపరి)
    {
	ఒకవేళ (సమితి_2->arr == arr)
	{
	    సమితి_2->arr = ఖాళీ;
	    ఆపు;
	}
    }
}

/*
 * Update highlight_attr[] array. If "update_ids" is true, then update
 * highlight_ids[] array instead.
 */
    స్థిర శూన్య
సమితి_highlight_attr(hl_override_T *arr, పూర్ణాంకం len, bool update_ids)
{
    కొరకు (పూర్ణాంకం i = 0; i < len; i++)
    {
	hl_override_T	*override = arr + i;
	పూర్ణాంకం		hlf = -1, attr;

	ఒకవేళ (override->from <= 0)
	    // Directly map to highlight_attr
	    hlf = -override->from;
	లేకపోతే
	{
	    కొరకు (పూర్ణాంకం k = 0; k < HLF_లెక్క; k++)
		ఒకవేళ (override->from == highlight_ids[k])
		{
		    hlf = k;
		    ఆపు;
		}

	    ఒకవేళ (hlf == -1)
		కొనసాగు;
	}

	ఒకవేళ (update_ids)
	{
	    ఒకవేళ (override->గమ్యం <= 0)
	    {
		ఒకవేళ (hlf == -override->గమ్యం)
		    highlight_ids[hlf] = -1;
		లేకపోతే
		    highlight_ids[hlf] = highlight_ids[-override->గమ్యం];
	    }
	    లేకపోతే
		highlight_ids[hlf] = override->గమ్యం;
	}
	లేకపోతే
	{
	    ఒకవేళ (override->గమ్యం <= 0)
		// Directly use highlight_attr[]
		attr = highlight_attr[-override->గమ్యం];
	    లేకపోతే
		attr = syn_id2attr(override->గమ్యం);

	    highlight_attr[hlf] = attr;
	}
    }
}

/*
 * Set the current highlight override to "arr". Returns true if successful
 */
    bool
నెట్టు_highlight_overrides(hl_override_T *arr, పూర్ణాంకం len)
{
    // Don't want to do anything if "arr" is already the current override. If
    // "arr" is NULL (but overrides->arr is not), then still push an override,
    // but "->arr" will just be NULL so any previous overrides are cleared.
    ఒకవేళ (overrides != ఖాళీ && overrides->arr == arr)
	తిరిగి false;

    hl_overrides_T *సమితి_2;

    సమితి_2 = ALLOC_ONE(hl_overrides_T);
    ఒకవేళ (సమితి_2 == ఖాళీ)
	తిరిగి false;

    సమితి_2->arr = arr;
    సమితి_2->len = len;

    // Push to front of list
    సమితి_2->తదుపరి = overrides;
    overrides = సమితి_2;

    // Save current highlight_attr[], so it can be restored later after this
    // override is popped.
    జ్ఞాపకం_నకలు(సమితి_2->attr, highlight_attr, పరిమాణం(highlight_attr));

    // Reset highlight_attr[] to default values, as we don't want changes from
    // other overrides.
    జ్ఞాపకం_నకలు(highlight_attr, highlight_attr_raw, పరిమాణం(highlight_attr));

    // Update highlight_attr[] array
    ఒకవేళ (arr != ఖాళీ)
	సమితి_highlight_attr(arr, len, false);

    తిరిగి true;
}

/*
 * Pop the current highlight override (if any) from the stack. This should
 * always be paired with a *successful* push.
 */
    శూన్య
తీయి_highlight_overrides(శూన్య)
{
    hl_overrides_T *సమితి_2 = overrides;

    ఒకవేళ (overrides == ఖాళీ)
	తిరిగి;

    overrides = సమితి_2->తదుపరి;

    // Set highlight_attr[] to state before override was pushed.
    జ్ఞాపకం_నకలు(highlight_attr, సమితి_2->attr, పరిమాణం(highlight_attr));
    vim_free(సమితి_2);
}

/*
 * Parse the 'winhighlight' option and return array. Returns NULL on failure or
 * if empty option. If failure, then errmsg is set.
 */
    స్థిర hl_override_T *
parse_winhighlight(char_u *opt, పూర్ణాంకం *len, అక్షరం **errmsg)
{
    char_u	    *p = opt;
    hl_override_T   *arr;
    పూర్ణాంకం		    i = 0;
    పూర్ణాంకం		    అంకె = 1;
    పూర్ణాంకం		    n_colons = 0;

    ఒకవేళ (*p == NUL)
	తిరిగి ఖాళీ;

    // Get number of overrides first so we can allocate array
    అయితే ((p = vim_strchr(p, ',')) != ఖాళీ)
    {
	p++;
	అంకె++;
    }
    p = opt;
    // Check if number of ':' matches number of ','
    అయితే ((p = vim_strchr(p, ':')) != ఖాళీ)
    {
	p++;
	n_colons++;
    }

    ఒకవేళ (అంకె != n_colons)
    {
	*errmsg = e_invalid_వాదం;
	తిరిగి ఖాళీ;
    }

    arr = ALLOC_MULT(hl_override_T, అంకె);
    ఒకవేళ (arr == ఖాళీ)
    {
	*errmsg = e_బహిర్గతం_యొక్క_జ్ఞాపకం;
	తిరిగి ఖాళీ;
    }

    p = opt;

    అయితే (true)
    {
	hl_override_T *override = arr + i++;
	char_u	*fromname = p, *toname;
	char_u	*tmp;
	char_u	**పేర్లు[2] = {&fromname, &toname};
	పూర్ణాంకం	fromlen, tolen;
	పూర్ణాంకం	*lens[2] = {&fromlen, &tolen};
	పూర్ణాంకం	fromid, toid;
	పూర్ణాంకం	*ids[2] = {&fromid, &toid};

	p = vim_strchr(p, ':');

	ఒకవేళ (p == ఖాళీ)
	    వెళ్ళు fail;

	fromlen = p - fromname; // Get hl for "from"
	ఒకవేళ (fromlen == 0)
	    వెళ్ళు fail;
	p++; // Skip colon ':'
	ఒకవేళ (*p == NUL)
	    వెళ్ళు fail;

	toname = p;
	tmp = vim_strchr(p, ',');

	// Get hl for "to", must check for no trailing comma in case last
	// element.
	ఒకవేళ (tmp == ఖాళీ)
	    tolen = (పూర్ణాంకం)STRLEN(p);
	లేకపోతే
	{
	    tolen = tmp - toname;
	    p = ++tmp;
	}
	ఒకవేళ (tolen == 0)
	    వెళ్ళు fail;

	కొరకు (పూర్ణాంకం k = 0; k < 2; k++)
	{
	    char_u  *name = *పేర్లు[k];
	    పూర్ణాంకం	    nlen = *lens[k];

	    ఒకవేళ (*name == '!')
	    {
		// If starts with "!", then it maps directly to a 'highlight'
		// occasion.
		పూర్ణాంకం hlf;

		ఒకవేళ (nlen != 2)
		    వెళ్ళు fail;

		కొరకు (hlf = 0; hlf < (పూర్ణాంకం)HLF_లెక్క; ++hlf)
		    ఒకవేళ (hl_flags[hlf] == name[1])
			ఆపు;

		ఒకవేళ (hlf >= HLF_లెక్క)
		    వెళ్ళు fail;

		*ids[k] = -hlf;
	    }
	    లేకపోతే
	    {
		// Otherwise get the highlight group id.
		ఒకవేళ (syn_check_సమూహం(name, nlen) == 0)
		    వెళ్ళు fail;

		*ids[k] = syn_namen2id(name, nlen);
		ఒకవేళ (*ids[k] == 0)
		    వెళ్ళు fail;

		// Always map "Normal" group to HLF_WIN
		ఒకవేళ (ids[k] == &fromid &&
			STRCMP(HL_పట్టిక()[*ids[k] - 1].sg_name_u, "NORMAL") == 0)
		    *ids[k] = -HLF_WIN;
	    }
	}

	// If fromid == toid, leave it be, this is so that we know HLF_WIN has
	// been set (even to itself e.g. "Normal"), and therefore know to use
	// its attr instead.
	override->from = fromid;
	override->గమ్యం = toid;

	ఒకవేళ (tmp == ఖాళీ)
	    ఆపు;
    }

    *len = అంకె;
    తిరిగి arr;
fail:
    vim_free(arr);
    *errmsg = e_invalid_వాదం;
    తిరిగి ఖాళీ;
}

/*
 * Update w_hl for "wp" using given option value. Returns error
 * message on failure, otherwise NULL.
 */
    అక్షరం *
update_winhighlight(win_T *wp, char_u *opt)
{
    అక్షరం	    *err = ఖాళీ;
    hl_override_T   *arr;
    పూర్ణాంకం		    అంకె = 1;

    arr = parse_winhighlight(opt, &అంకె, &err);

    ఒకవేళ (arr == ఖాళీ && err != ఖాళీ)
	తిరిగి err;

    ఒకవేళ (arr == ఖాళీ && wp->w_hl == ఖాళీ)
	తిరిగి ఖాళీ;

    update_highlight_overrides(wp->w_hl, arr, అంకె);

    vim_free(wp->w_hl);
    wp->w_hl = arr;
    wp->w_hl_len = అంకె;

    wp->w_hlfwin_id = hlf_get_id(wp, HLF_WIN);

#నిర్వచితమైతే FEAT_TERMINAL
    పదం_update_hlfwin(wp);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TERMINAL
    // May be NULL (such as in after_copy_winopt())
    ఒకవేళ (wp->w_బఫరు != ఖాళీ)
    {
	// Make sure terminal highlighting is updated
	bool ret = నెట్టు_highlight_overrides(wp->w_hl, wp->w_hl_len);

	ఒకవేళ (wp->w_బఫరు->b_పదం != ఖాళీ)
	    పదం_init_default_colors(wp->w_బఫరు->b_పదం);
	ఒకవేళ (ret)
	    తీయి_highlight_overrides();
    }
#అయితే_ముగింపు

    తిరిగి ఖాళీ;
}

/*
 * Get id that the hlf attr is using in window "wp", otherwise zero. Note that
 * -1 may be returned if the hlf was set to itself.
 */
    పూర్ణాంకం
hlf_get_id(win_T *wp, పూర్ణాంకం hlf)
{
    పూర్ణాంకం id;
    స్థిర పూర్ణాంకం prev[HLF_లెక్క];

    ఒకవేళ (wp->w_hl == ఖాళీ)
	తిరిగి highlight_ids[hlf];

    జ్ఞాపకం_నకలు(prev, highlight_ids, పరిమాణం(highlight_ids));
    సమితి_highlight_attr(wp->w_hl, wp->w_hl_len, true);
    id = highlight_ids[hlf];
    జ్ఞాపకం_నకలు(highlight_ids, prev, పరిమాణం(highlight_ids));

    తిరిగి id;
}

/*
 * Update 'winhighlight' using a 'wincolor' style option value. Returns error
 * message on failure otherwise NULL. Does not update the actual 'wincolor'
 * value, but does update the 'winhighlight' value.
 */
    అక్షరం *
update_wincolor(win_T *wp, char_u *opt)
{
    char_u  *str = *opt == NUL ? "" : alloc(పరిమాణం("!(:") + STRLEN(opt));
    అక్షరం    *errmsg;

    ఒకవేళ (str == ఖాళీ)
	తిరిగి e_బహిర్గతం_యొక్క_జ్ఞాపకం;

    ఒకవేళ (*opt != NUL)
	sprintf((అక్షరం *)str, "!(:%s", opt);

    errmsg = update_winhighlight(wp, str);

    ఒకవేళ (errmsg == ఖాళీ)
	సమితి_string_option_ప్రత్యక్ష_లోపల_win(wp, (char_u *)"winhighlight", -1,
		str, OPT_FREE|OPT_LOCAL, 0);

    ఒకవేళ (*opt != NUL)
	విడుదల(str);
    తిరిగి errmsg;
}
