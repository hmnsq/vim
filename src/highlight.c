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

#تعریف_کرو SG_رکن		1	// term has been set
#تعریف_کرو SG_CTERM	2	// cterm has been set
#تعریف_کرو SG_GUI		4	// gui has been set
#تعریف_کرو SG_LINK		8	// link has been set

#تعریف_کرو MAX_SYN_NAME	200

/*
 * The "term", "cterm" and "gui" arguments can be any combination of the
 * following names, separated by commas (but no spaces!).
 */
// must be sorted by the 'value' field because it is used by bsearch()!
// note: inverse and reverse use the same key
ساکن keyvalue_T highlight_tab[] = {
    KEYVALUE_اندراج(HL_BOLD, "bold"),			    // index 0
    KEYVALUE_اندراج(HL_INVERSE, "inverse"),		    // index 1
    KEYVALUE_اندراج(HL_ITALIC, "italic"),		    // index 2
    KEYVALUE_اندراج(HL_NOCOMBINE, "nocombine"),		    // index 3
    KEYVALUE_اندراج(HL_معمول, "NONE"),			    // index 4
    KEYVALUE_اندراج(HL_INVERSE, "reverse"),		    // index 5
    KEYVALUE_اندراج(HL_STANDOUT, "standout"),		    // index 6
    KEYVALUE_اندراج(HL_STRIKETHROUGH, "strikethrough"),	    // index 7
    KEYVALUE_اندراج(HL_UNDERCURL, "undercurl"),		    // index 8
    KEYVALUE_اندراج(HL_UNDERDASHED, "underdashed"),	    // index 9
    KEYVALUE_اندراج(HL_UNDERDOTTED, "underdotted"),	    // index 10
    KEYVALUE_اندراج(HL_UNDERDOUBLE, "underdouble"),	    // index 11
    KEYVALUE_اندراج(HL_UNDERLINE, "underline")		    // index 12
};

// this table is used to display highlight names in the "correct" sequence.
// keep this in sync with highlight_tab[].
ساکن keyvalue_T *highlight_اشاریہ_tab[] = {
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
#تعریف_کرو MAX_ATTR_LEN 120

#تعریف_کرو ATTR_COMBINE(attr_a, attr_b) ((((attr_b) & HL_NOCOMBINE) ? (attr_b) : (attr_a)) | (attr_b))

شمار {
    BLACK = 0,
    DARKBLUE,
    DARKGREEN,
    DARKCYAN,
    DARKRED,
    DARKMAGENTA,
    BROWN,
    DARKYELLOW,
    سرمئی,
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
ساکن keyvalue_T color_name_tab[] = {
    KEYVALUE_اندراج(BLACK, "Black"),
    KEYVALUE_اندراج(BLUE, "Blue"),
    KEYVALUE_اندراج(BROWN, "Brown"),
    KEYVALUE_اندراج(CYAN, "Cyan"),
    KEYVALUE_اندراج(DARKBLUE, "DarkBlue"),
    KEYVALUE_اندراج(DARKCYAN, "DarkCyan"),
    KEYVALUE_اندراج(DARKGRAY, "DarkGray"),
    KEYVALUE_اندراج(DARKGREEN, "DarkGreen"),
    KEYVALUE_اندراج(DARKGREY, "DarkGrey"),
    KEYVALUE_اندراج(DARKMAGENTA, "DarkMagenta"),
    KEYVALUE_اندراج(DARKRED, "DarkRed"),
    KEYVALUE_اندراج(DARKYELLOW, "DarkYellow"),
    KEYVALUE_اندراج(سرمئی, "Gray"),
    KEYVALUE_اندراج(GREEN, "Green"),
    KEYVALUE_اندراج(GREY, "Grey"),
    KEYVALUE_اندراج(LIGHTBLUE, "LightBlue"),
    KEYVALUE_اندراج(LIGHTCYAN, "LightCyan"),
    KEYVALUE_اندراج(LIGHTGRAY, "LightGray"),
    KEYVALUE_اندراج(LIGHTGREEN, "LightGreen"),
    KEYVALUE_اندراج(LIGHTGREY, "LightGrey"),
    KEYVALUE_اندراج(LIGHTMAGENTA, "LightMagenta"),
    KEYVALUE_اندراج(LIGHTRED, "LightRed"),
    KEYVALUE_اندراج(LIGHTYELLOW, "LightYellow"),
    KEYVALUE_اندراج(MAGENTA, "Magenta"),
    KEYVALUE_اندراج(NONE, "NONE"),
    KEYVALUE_اندراج(RED, "Red"),
    KEYVALUE_اندراج(WHITE, "White"),
    KEYVALUE_اندراج(YELLOW, "Yellow")
};

/*
 * Structure that stores information about a highlight group.
 * The ID of a highlight group is also called group ID.  It is the index in
 * the highlight_ga array PLUS ONE.
 */
قسمتعریف ساخت
{
    char_u	*sg_name;	// highlight group name
    char_u	*sg_name_u;	// uppercase of sg_name
    صحیح		sg_cleared;	// "hi clear" was used
// for normal terminals
    صحیح		sg_رکن;	// "term=" highlighting attributes
    char_u	*sg_آغاز;	// terminal string for start highl
    char_u	*sg_stop;	// terminal string for stop highl
    صحیح		sg_رکن_attr;	// Screen attr for term mode
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
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
    GuiFont	sg_font;	// GUI font handle
# ifdef FEAT_XFONTSET
    GuiFontset	sg_fontset;	// GUI fontset handle
# endif
    char_u	*sg_font_name;  // GUI font or fontset name
    صحیح		sg_gui_attr;    // Screen attr for GUI mode
#اگر_ختم
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
// Store the sp color name for the GUI or synIDattr()
    صحیح		sg_gui;		// "gui=" highlighting attributes
    char_u	*sg_gui_fg_name;// GUI foreground color name
    char_u	*sg_gui_bg_name;// GUI background color name
    char_u	*sg_gui_sp_name;// GUI special color name
#اگر_ختم
    صحیح		sg_link;	// link to this highlight group ID
    صحیح		sg_deflink;	// default link; restored in highlight_clear()
    صحیح		sg_سیٹ;		// combination of SG_* flags
#اگر_تعریف_شدہ FEAT_EVAL
    sctx_T	sg_deflink_sctx;  // script where the default link was set
    sctx_T	sg_script_ctx;	// script in which the group was last set
#اگر_ختم
} hl_گروہ_T;

قسمتعریف ساخت hl_overrides_S hl_overrides_T;
ساخت hl_overrides_S
{
    hl_override_T   *arr;   // May be NULL if "arr" was freed or no highlight
			    // overrides (all values set to default)
    صحیح		    len;
    hl_overrides_T  *اگلا;  // Used to handle recursive calls

    صحیح attr[HLF_گنتی];    // highlight_attr[] before being updated.
};

ساکن hl_overrides_T *overrides = تہی;

// Synced with highlight_attr, each is the highlight group id for each attr. If
// an element is 0, then there is no attr for it. If an element is -1, then it
// means it was set to itself.
ساکن صحیح highlight_ids[HLF_گنتی];

// Same as highlight_attr[] but is not modified by highlight group overrides
ساکن صحیح highlight_attr_raw[HLF_گنتی];

ساکن صحیح hl_flags[HLF_گنتی] = HL_FLAGS;

// highlight groups for 'highlight' option
ساکن garray_T highlight_ga;
#تعریف_کرو HL_جدول()	((hl_گروہ_T *)((highlight_ga.ga_ڈیٹا)))

// Wrapper struct for hashtable entries: stores the highlight group ID alongside
// the uppercase name used as a hash key.  Uses the same offsetof pattern as
// signgroup_T / HI2SG.
قسمتعریف ساخت {
    صحیح		hn_id;		// highlight group ID (1-based)
    char_u	hn_کلید[1];	// uppercase name (stretchable array)
} hlname_T;

#تعریف_کرو HLNAME_کلید_OFF	offsetof(hlname_T, hn_کلید)
#تعریف_کرو HI2HLNAME(hi)	((hlname_T *)((hi)->hi_کلید - HLNAME_کلید_OFF))

// hashtable for quick highlight group name lookup
ساکن hashtab_T highlight_ht;
ساکن bool	 highlight_ht_inited = false;

/*
 * An attribute number is the index in attr_table plus ATTR_OFF.
 */
#تعریف_کرو ATTR_OFF (HL_ALL + 1)

ساکن خالی syn_unadd_گروہ(خالی);
ساکن خالی سیٹ_hl_attr(صحیح idx);
ساکن خالی highlight_list_one(صحیح id);
ساکن صحیح highlight_list_arg(صحیح id, صحیح didh, صحیح type, صحیح iarg, char_u *sarg, حرف *name);
ساکن صحیح syn_جوڑو_گروہ(char_u *name);
ساکن صحیح syn_name2id_len(char_u *name, صحیح len);
ساکن صحیح hl_has_settings(صحیح idx, صحیح check_link);
ساکن خالی highlight_clear(صحیح idx);

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
ساکن خالی gui_do_one_color(صحیح idx, صحیح do_menu, صحیح do_tooltip);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
ساکن صحیح  سیٹ_گروہ_colors(char_u *name, guicolor_T *fgp, guicolor_T *bgp, صحیح do_menu, صحیح use_norm, صحیح do_tooltip);
ساکن خالی hl_do_font(صحیح idx, char_u *arg, صحیح do_معمول, صحیح do_menu, صحیح do_tooltip, صحیح free_font);
#اگر_ختم

/*
 * The default highlight groups.  These are compiled-in for fast startup and
 * they still work when the runtime files can't be found.
 * When making changes here, also change runtime/colors/default.vim!
 * The #ifdefs are needed to reduce the amount of static data.  Helps to make
 * the 16 bit DOS (museum) version compile.
 */
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
# define CENT(a, b) b
#ورنہ
# define CENT(a, b) a
#اگر_ختم
ساکن حرف *(highlight_init_both[]) = {
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
#اگر_تعریف_شدہ FEAT_CLIPBOARD
    CENT("VisualNOS term=underline,bold cterm=underline,bold",
	 "VisualNOS term=underline,bold cterm=underline,bold gui=underline,bold"),
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIFF
    CENT("DiffText term=reverse cterm=bold ctermbg=Red ctermfg=Black",
	 "DiffText term=reverse cterm=bold ctermbg=Red ctermfg=Black gui=bold guibg=Red"),
    "default link DiffTextAdd DiffText",
#اگر_ختم
    CENT("PmenuSbar term=NONE ctermbg=Grey",
	 "PmenuSbar term=NONE ctermbg=Grey guibg=Grey"),
    CENT("TabLineSel term=bold cterm=bold",
	 "TabLineSel term=bold cterm=bold gui=bold"),
    CENT("TabLineFill term=reverse cterm=reverse",
	 "TabLineFill term=reverse cterm=reverse gui=reverse"),
    "default link TabPanel TabLine",
    "default link TabPanelSel TabLineSel",
    "default link TabPanelFill TabLineFill",
#اگر_تعریف_شدہ FEAT_GUI
    "Cursor guibg=fg guifg=bg",
    "lCursor guibg=fg guifg=bg", // should be different, but what?
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
    "TitleBar guibg=bg guifg=fg",
    "TitleBarNC guibg=NONE guifg=NONE",
#اگر_ختم
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
    تہی
};

// Default colors only used with a light background.
ساکن حرف *(highlight_init_light[]) = {
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
#اگر_تعریف_شدہ FEAT_SPELL
    CENT("SpellBad term=reverse ctermbg=LightRed",
	 "SpellBad term=reverse ctermbg=LightRed guisp=Red gui=undercurl"),
    CENT("SpellCap term=reverse ctermbg=LightBlue",
	 "SpellCap term=reverse ctermbg=LightBlue guisp=Blue gui=undercurl"),
    CENT("SpellRare term=reverse ctermbg=LightMagenta",
	 "SpellRare term=reverse ctermbg=LightMagenta guisp=Magenta gui=undercurl"),
    CENT("SpellLocal term=underline ctermbg=Cyan",
	 "SpellLocal term=underline ctermbg=Cyan guisp=DarkCyan gui=undercurl"),
#اگر_ختم
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
#اگر_تعریف_شدہ FEAT_FOLDING
    CENT("Folded term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "Folded term=standout ctermbg=Grey ctermfg=DarkBlue guibg=LightGrey guifg=DarkBlue"),
    CENT("FoldColumn term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "FoldColumn term=standout ctermbg=Grey ctermfg=DarkBlue guibg=Grey guifg=DarkBlue"),
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGNS
    CENT("SignColumn term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "SignColumn term=standout ctermbg=Grey ctermfg=DarkBlue guibg=Grey guifg=DarkBlue"),
#اگر_ختم
    CENT("Visual ctermbg=Grey ctermfg=Black",
	 "Visual ctermbg=Grey ctermfg=Black guibg=LightGrey guifg=Black"),
#اگر_تعریف_شدہ FEAT_DIFF
    CENT("DiffAdd term=bold ctermbg=LightBlue ctermfg=Black",
	 "DiffAdd term=bold ctermbg=LightBlue ctermfg=Black guibg=LightBlue"),
    CENT("DiffChange term=bold ctermbg=LightMagenta ctermfg=Black",
	 "DiffChange term=bold ctermbg=LightMagenta ctermfg=Black guibg=LightMagenta"),
    CENT("DiffDelete term=bold ctermfg=Blue ctermbg=LightCyan",
	 "DiffDelete term=bold ctermfg=Blue ctermbg=LightCyan gui=bold guifg=Blue guibg=LightCyan"),
#اگر_ختم
    CENT("TabLine term=underline cterm=underline ctermfg=black ctermbg=LightGrey",
	 "TabLine term=underline cterm=underline ctermfg=black ctermbg=LightGrey gui=underline guibg=LightGrey"),
#اگر_تعریف_شدہ FEAT_SYN_HL
    CENT("CursorColumn term=reverse ctermbg=LightGrey",
	 "CursorColumn term=reverse ctermbg=LightGrey guibg=Grey90"),
    CENT("CursorLine term=underline cterm=underline",
	 "CursorLine term=underline cterm=underline guibg=Grey90"),
    CENT("ColorColumn term=reverse ctermbg=LightRed",
	 "ColorColumn term=reverse ctermbg=LightRed guibg=LightRed"),
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CONCEAL
    CENT("Conceal ctermbg=DarkGrey ctermfg=LightGrey",
	 "Conceal ctermbg=DarkGrey ctermfg=LightGrey guibg=DarkGrey guifg=LightGrey"),
#اگر_ختم
    CENT("MatchParen term=reverse ctermbg=Cyan",
	 "MatchParen term=reverse ctermbg=Cyan guibg=Cyan"),
#اگر_تعریف_شدہ FEAT_TERMINAL
    CENT("StatusLineTerm term=reverse,bold cterm=bold ctermfg=White ctermbg=DarkGreen",
	 "StatusLineTerm term=reverse,bold cterm=bold ctermfg=White ctermbg=DarkGreen gui=bold guifg=bg guibg=DarkGreen"),
    CENT("StatusLineTermNC term=reverse ctermfg=White ctermbg=DarkGreen",
	 "StatusLineTermNC term=reverse ctermfg=White ctermbg=DarkGreen guifg=bg guibg=DarkGreen"),
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MENU
    CENT("ToolbarLine term=underline ctermbg=LightGrey",
	 "ToolbarLine term=underline ctermbg=LightGrey guibg=LightGrey"),
    CENT("ToolbarButton cterm=bold ctermfg=White ctermbg=DarkGrey",
	 "ToolbarButton cterm=bold ctermfg=White ctermbg=DarkGrey gui=bold guifg=White guibg=Grey40"),
#اگر_ختم
    تہی
};

// Default colors only used with a dark background.
ساکن حرف *(highlight_init_dark[]) = {
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
#اگر_تعریف_شدہ FEAT_SPELL
    CENT("SpellBad term=reverse ctermbg=Red",
	 "SpellBad term=reverse ctermbg=Red guisp=Red gui=undercurl"),
    CENT("SpellCap term=reverse ctermbg=Blue",
	 "SpellCap term=reverse ctermbg=Blue guisp=Blue gui=undercurl"),
    CENT("SpellRare term=reverse ctermbg=Magenta",
	 "SpellRare term=reverse ctermbg=Magenta guisp=Magenta gui=undercurl"),
    CENT("SpellLocal term=underline ctermbg=Cyan",
	 "SpellLocal term=underline ctermbg=Cyan guisp=Cyan gui=undercurl"),
#اگر_ختم
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
#اگر_تعریف_شدہ FEAT_FOLDING
    CENT("Folded term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "Folded term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=DarkGrey guifg=Cyan"),
    CENT("FoldColumn term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "FoldColumn term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=Grey guifg=Cyan"),
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGNS
    CENT("SignColumn term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "SignColumn term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=Grey guifg=Cyan"),
#اگر_ختم
    CENT("Visual ctermbg=Grey ctermfg=Black",
	 "Visual ctermbg=Grey ctermfg=Black guibg=#575757 guifg=LightGrey"),
#اگر_تعریف_شدہ FEAT_DIFF
    CENT("DiffAdd term=bold ctermbg=DarkBlue",
	 "DiffAdd term=bold ctermbg=DarkBlue guibg=DarkBlue"),
    CENT("DiffChange term=bold ctermbg=DarkMagenta",
	 "DiffChange term=bold ctermbg=DarkMagenta guibg=DarkMagenta"),
    CENT("DiffDelete term=bold ctermfg=Blue ctermbg=DarkCyan",
	 "DiffDelete term=bold ctermfg=Blue ctermbg=DarkCyan gui=bold guifg=Blue guibg=DarkCyan"),
#اگر_ختم
    CENT("TabLine term=underline cterm=underline ctermfg=white ctermbg=DarkGrey",
	 "TabLine term=underline cterm=underline ctermfg=white ctermbg=DarkGrey gui=underline guibg=DarkGrey"),
#اگر_تعریف_شدہ FEAT_SYN_HL
    CENT("CursorColumn term=reverse ctermbg=DarkGrey",
	 "CursorColumn term=reverse ctermbg=DarkGrey guibg=Grey40"),
    CENT("CursorLine term=underline cterm=underline",
	 "CursorLine term=underline cterm=underline guibg=Grey40"),
    CENT("ColorColumn term=reverse ctermbg=DarkRed",
	 "ColorColumn term=reverse ctermbg=DarkRed guibg=DarkRed"),
#اگر_ختم
    CENT("MatchParen term=reverse ctermbg=DarkCyan",
	 "MatchParen term=reverse ctermbg=DarkCyan guibg=DarkCyan"),
#اگر_تعریف_شدہ FEAT_CONCEAL
    CENT("Conceal ctermbg=DarkGrey ctermfg=LightGrey",
	 "Conceal ctermbg=DarkGrey ctermfg=LightGrey guibg=DarkGrey guifg=LightGrey"),
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
    CENT("StatusLineTerm term=reverse,bold cterm=bold ctermfg=Black ctermbg=LightGreen",
	 "StatusLineTerm term=reverse,bold cterm=bold ctermfg=Black ctermbg=LightGreen gui=bold guifg=bg guibg=LightGreen"),
    CENT("StatusLineTermNC term=reverse ctermfg=Black ctermbg=LightGreen",
	 "StatusLineTermNC term=reverse ctermfg=Black ctermbg=LightGreen guifg=bg guibg=LightGreen"),
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MENU
    CENT("ToolbarLine term=underline ctermbg=DarkGrey",
	 "ToolbarLine term=underline ctermbg=DarkGrey guibg=Grey50"),
    CENT("ToolbarButton cterm=bold ctermfg=Black ctermbg=LightGrey",
	 "ToolbarButton cterm=bold ctermfg=Black ctermbg=LightGrey gui=bold guifg=Black guibg=LightGrey"),
#اگر_ختم
    تہی
};

#اگر defined(FEAT_SYN_HL)
/*
 * Returns the number of highlight groups.
 */
    صحیح
highlight_عدد_groups(خالی)
{
    واپس highlight_ga.ga_len;
}

/*
 * Returns the name of a highlight group.
 */
    char_u *
highlight_گروہ_name(صحیح id)
{
    واپس HL_جدول()[id].sg_name;
}

/*
 * Returns the ID of the link to a highlight group.
 */
    صحیح
highlight_link_id(صحیح id)
{
    واپس HL_جدول()[id].sg_link;
}
#اگر_ختم

ساکن خالی resolve_fallback_fg_مقصد_rgb(خالی);
ساکن خالی resolve_fallback_bg_مقصد_rgb(خالی);

    خالی
init_highlight(
    صحیح		both,	    // include groups where 'bg' doesn't matter
    صحیح		reset)	    // clear group first
{
    صحیح		i;
    حرف	**pp;
    ساکن صحیح	had_both = FALSE;
#اگر_تعریف_شدہ FEAT_EVAL
    char_u	*p;

    // Try finding the color scheme file.  Used when a color file was loaded
    // and 'background' or 't_Co' is changed.
    p = get_var_قدر((char_u *)"g:colors_name");
    اگر (p != تہی)
    {
	// The value of g:colors_name could be freed when sourcing the script,
	// making "p" invalid, so copy it.
	char_u *copy_p = vim_strsave(p);
	صحیح    r;

	اگر (copy_p != تہی)
	{
	    r = بھرو_colors(copy_p);
	    vim_free(copy_p);
	    اگر (r == OK)
		واپس;
	}
    }

#اگر_ختم

    // Didn't use a color file, use the compiled-in colors.
    اگر (both)
    {
	had_both = TRUE;
	pp = highlight_init_both;
	برائے (i = 0; pp[i] != تہی; ++i)
	    do_highlight((char_u *)pp[i], reset, TRUE);
    }
    ورنہ اگر (!had_both)
	// Don't do anything before the call with both == TRUE from main().
	// Not everything has been setup then, and that call will overrule
	// everything anyway.
	واپس;

    اگر (*p_bg == 'l')
	pp = highlight_init_light;
    ورنہ
	pp = highlight_init_dark;
    برائے (i = 0; pp[i] != تہی; ++i)
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

#اگر_تعریف_شدہ FEAT_SYN_HL
    // If syntax highlighting is enabled load the highlighting for it.
    اگر (get_var_قدر((char_u *)"g:syntax_on") != تہی)
    {
	ساکن صحیح	recursive = 0;

	اگر (recursive >= 5)
	    emsg(_(e_recursive_loop_loading_syncolor_vim));
	ورنہ
	{
	    ++recursive;
	    (خالی)ماخذ_runtime((char_u *)"syntax/syncolor.vim", DIP_ALL);
	    --recursive;
	}
    }
#اگر_ختم
    resolve_fallback_fg_مقصد_rgb();
    resolve_fallback_bg_مقصد_rgb();
}

#اگر defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
/*
 * Load a default color list. Intended to support legacy color names but allows
 * the user to override the color values. Only loaded once.
 */
    ساکن خالی
بھرو_default_colors_lists(خالی)
{
    // Lacking a default color list isn't the end of the world but it is likely
    // an inconvenience so users should know when it is missing.
    اگر (ماخذ_runtime((char_u *)"colors/lists/default.vim", DIP_ALL) != OK)
	پیغام("failed to load colors/lists/default.vim");
}
#اگر_ختم

/*
 * Load color file "name".
 * Return OK for success, FAIL for failure.
 */
    صحیح
بھرو_colors(char_u *name)
{
    char_u	*buf;
    صحیح		retval = FAIL;
    ساکن صحیح	recursive = FALSE;

    // When being called recursively, this is probably because setting
    // 'background' caused the highlighting to be reloaded.  This means it is
    // working, thus we should return OK.
    اگر (recursive)
	واپس OK;

    recursive = TRUE;
    buf = alloc(STRLEN(name) + 12);
    اگر (buf != تہی)
    {
#اگر defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	بھرو_default_colors_lists();
#اگر_ختم
	apply_autocmds(EVENT_COLORSCHEMEPRE, name,
					       curbuf->b_fname, FALSE, curbuf);
	sprintf((حرف *)buf, "colors/%s.vim", name);
	retval = ماخذ_runtime(buf, DIP_آغاز + DIP_OPT);
	vim_free(buf);
	اگر (retval == OK)
	    apply_autocmds(EVENT_COLORSCHEME, name, curbuf->b_fname,
								FALSE, curbuf);
    }
    recursive = FALSE;

    واپس retval;
}

ساکن صحیح color_numbers_16[28] = {0, 1, 2, 3,
				 4, 5, 6, 6,
				 7, 7, 7, 7,
				 8, 8,
				 9, 9, 10, 10,
				 11, 11, 12, 12, 13,
				 13, 14, 14, 15, -1};
// for xterm with 88 colors...
ساکن صحیح color_numbers_88[28] = {0, 4, 2, 6,
				 1, 5, 32, 72,
				 84, 84, 7, 7,
				 82, 82,
				 12, 43, 10, 61,
				 14, 63, 9, 74, 13,
				 75, 11, 78, 15, -1};
// for xterm with 256 colors...
ساکن صحیح color_numbers_256[28] = {0, 4, 2, 6,
				 1, 5, 130, 3,
				 248, 248, 7, 7,
				 242, 242,
				 12, 81, 10, 121,
				 14, 159, 9, 224, 13,
				 225, 11, 229, 15, -1};
// for terminals with less than 16 colors...
ساکن صحیح color_numbers_8[28] = {0, 4, 2, 6,
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
    ساکن صحیح
lookup_color(صحیح idx, صحیح foreground, صحیح *boldp)
{
    صحیح		color = color_numbers_16[idx];
    char_u	*p;

    // Use the _16 table to check if it's a valid color name.
    اگر (color < 0)
	واپس -1;

    اگر (t_colors == 8)
    {
	// t_Co is 8: use the 8 colors table
#اگر defined(__QNXNTO__)
	// On qnx, the 8 & 16 color arrays are the same
	اگر (STRNCMP(T_NAME, "qansi", 5) == 0)
	    color = color_numbers_16[idx];
	ورنہ
#اگر_ختم
	    color = color_numbers_8[idx];
	اگر (foreground)
	{
	    // set/reset bold attribute to get light foreground
	    // colors (on some terminals, e.g. "linux")
	    اگر (color & 8)
		*boldp = TRUE;
	    ورنہ
		*boldp = FALSE;
	}
	color &= 7;	// truncate to 8 colors
    }
    ورنہ اگر (t_colors == 16 || t_colors == 88
					   || t_colors >= 256)
    {
	// Guess: if the termcap entry ends in 'm', it is
	// probably an xterm-like terminal.  Use the changed
	// order for colors.
	اگر (*T_CAF != NUL)
	    p = T_CAF;
	ورنہ
	    p = T_CSF;
	اگر (*p != NUL && (t_colors > 256
			      || *(p + STRLEN(p) - 1) == 'm'))
	{
	    اگر (t_colors == 88)
		color = color_numbers_88[idx];
	    ورنہ اگر (t_colors >= 256)
		color = color_numbers_256[idx];
	    ورنہ
		color = color_numbers_8[idx];
	}
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
	اگر (t_colors >= 256 && color == 15 && is_mac_terminal)
	    // Terminal.app has a bug: 15 is light grey. Use white
	    // from the color cube instead.
	    color = 231;
#اگر_ختم
    }
    واپس color;
}

/*
 * Link highlight group 'from_hg' to 'to_hg'.
 * 'dodefault' is set to TRUE for ":highlight default link".
 * 'forceit' is set to TRUE for ":highlight! link"
 * 'init' is set to TRUE when initializing all the highlight groups.
 */
    ساکن خالی
highlight_گروہ_link(
	char_u	*from_hg,
	صحیح	from_len,
	char_u	*مقصد_hg,
	صحیح	مقصد_len,
	صحیح	dodefault,
	صحیح	forceit,
	صحیح	init)
{
    صحیح		from_id;
    صحیح		مقصد_id;
    hl_گروہ_T	*hlgroup = تہی;

    from_id = syn_check_گروہ(from_hg, from_len);
    اگر (STRNCMP(مقصد_hg, "NONE", 4) == 0)
	مقصد_id = 0;
    ورنہ
	مقصد_id = syn_check_گروہ(مقصد_hg, مقصد_len);

    اگر (from_id > 0)
    {
	hlgroup = &HL_جدول()[from_id - 1];
	اگر (dodefault && (forceit || hlgroup->sg_deflink == 0))
	{
	    hlgroup->sg_deflink = مقصد_id;
#اگر_تعریف_شدہ FEAT_EVAL
	    hlgroup->sg_deflink_sctx = current_sctx;
	    hlgroup->sg_deflink_sctx.sc_lnum += SOURCING_LNUM;
#اگر_ختم
	}
    }

    اگر (from_id > 0 && (!init || hlgroup->sg_سیٹ == 0))
    {
	// Don't allow a link when there already is some highlighting
	// for the group, unless '!' is used
	اگر (مقصد_id > 0 && !forceit && !init
		&& hl_has_settings(from_id - 1, dodefault))
	{
	    اگر (SOURCING_NAME == تہی && !dodefault)
		emsg(_(e_گروہ_has_settings_highlight_link_ignored));
	}
	ورنہ اگر (hlgroup->sg_link != مقصد_id
#اگر_تعریف_شدہ FEAT_EVAL
		|| hlgroup->sg_script_ctx.sc_sid != current_sctx.sc_sid
#اگر_ختم
		|| hlgroup->sg_cleared)
	{
	    اگر (!init)
		hlgroup->sg_سیٹ |= SG_LINK;
	    hlgroup->sg_link = مقصد_id;
#اگر_تعریف_شدہ FEAT_EVAL
	    hlgroup->sg_script_ctx = current_sctx;
	    hlgroup->sg_script_ctx.sc_lnum += SOURCING_LNUM;
#اگر_ختم
	    hlgroup->sg_cleared = FALSE;
	    redraw_all_later(UPD_SOME_VALID);

	    // Only call highlight_changed() once after multiple changes.
	    need_highlight_بدلا = TRUE;
	}
    }

}

/*
 * Reset all highlighting to the defaults. Removes all highlighting for the
 * groups added by the user.
 */
    ساکن خالی
highlight_reset_all(خالی)
{
    صحیح		idx;

#اگر_تعریف_شدہ FEAT_GUI
    // First, we do not destroy the old values, but allocate the new
    // ones and update the display. THEN we destroy the old values.
    // If we destroy the old values first, then the old values
    // (such as GuiFont's or GuiFontset's) will still be displayed but
    // invalid because they were free'd.
    اگر (gui.اندر_use)
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
    اگر (gui.اندر_use)
	gui_mch_سیٹ_titlebar_colors();
# endif
# ifdef FEAT_GUI_X11
#  ifdef FEAT_MENU

    // This only needs to be done when there is no Menu highlight
    // group defined by default, which IS currently the case.
    gui_mch_new_menu_colors();
#  endif
    اگر (gui.اندر_use)
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
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
    do_unlet((char_u *)"g:colors_name", TRUE);
#اگر_ختم
    restore_cterm_colors();

    // Clear all default highlight groups and load the defaults.
    برائے (idx = 0; idx < highlight_ga.ga_len; ++idx)
	highlight_clear(idx);
    init_highlight(TRUE, TRUE);
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    اگر (USE_24_ثنائی_ہندسہ)
	highlight_gui_started();
    ورنہ
#اگر_ختم
	highlight_بدلا();
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
    ساکن صحیح
highlight_سیٹ_termgui_attr(صحیح idx, char_u *کلید, char_u *arg, صحیح init)
{
    صحیح		attr;
    size_t	off;
    keyvalue_T	ہدف;
    keyvalue_T	*اندراج_2;

    attr = 0;
    off = 0;
    ہدف.کلید = 0;
    ہدف.قدر.لمبائی = 0;	// not used, see cmp_keyvalue_value_ni()
    جب (arg[off] != NUL)
    {
	ہدف.قدر.string = arg + off;
	اندراج_2 = (keyvalue_T *)bsearch(&ہدف, &highlight_tab,
		ARRAY_لمبائی(highlight_tab), حجم(highlight_tab[0]),
		cmp_keyvalue_قدر_ni);
	اگر (اندراج_2 == تہی)
	{
	    semsg(_(e_illegal_قدر_str), arg);
	    واپس FALSE;
	}

	attr |= اندراج_2->کلید;
	off += اندراج_2->قدر.لمبائی;
	اگر (arg[off] == ',')		// another one follows
	    ++off;
    }
    اگر (*کلید == 'T')
    {
	اگر (!init || !(HL_جدول()[idx].sg_سیٹ & SG_رکن))
	{
	    اگر (!init)
		HL_جدول()[idx].sg_سیٹ |= SG_رکن;
	    HL_جدول()[idx].sg_رکن = attr;
	}
    }
    ورنہ اگر (*کلید == 'C')
    {
	اگر (!init || !(HL_جدول()[idx].sg_سیٹ & SG_CTERM))
	{
	    اگر (!init)
		HL_جدول()[idx].sg_سیٹ |= SG_CTERM;
	    HL_جدول()[idx].sg_cterm = attr;
	    HL_جدول()[idx].sg_cterm_bold = FALSE;
	}
    }
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
    ورنہ
    {
	اگر (!init || !(HL_جدول()[idx].sg_سیٹ & SG_GUI))
	{
	    اگر (!init)
		HL_جدول()[idx].sg_سیٹ |= SG_GUI;
	    HL_جدول()[idx].sg_gui = attr;
	}
    }
#اگر_ختم

    واپس TRUE;
}

#اگر_تعریف_شدہ FEAT_GUI
/*
 * Set the font for the highlight group at 'idx'.
 * 'arg' is the font name.
 * Returns TRUE if the font is changed.
 */
    ساکن صحیح
highlight_سیٹ_font(
	صحیح	idx,
	char_u	*arg,
	صحیح	is_معمول_گروہ,
	صحیح	is_menu_گروہ,
	صحیح	is_tooltip_گروہ)
{
    صحیح		did_change = FALSE;

    // in non-GUI fonts are simply ignored
    اگر (HL_جدول()[idx].sg_font_name != تہی
	    && STRCMP(HL_جدول()[idx].sg_font_name, arg) == 0)
    {
	// Font name didn't change, ignore.
    }
    ورنہ اگر (!gui.shell_created)
    {
	// GUI not started yet, always accept the name.
	vim_free(HL_جدول()[idx].sg_font_name);
	HL_جدول()[idx].sg_font_name = vim_strsave(arg);
	did_change = TRUE;
    }
    ورنہ
    {
	GuiFont عارضی_sg_font = HL_جدول()[idx].sg_font;
# ifdef FEAT_XFONTSET
	GuiFontset عارضی_sg_fontset = HL_جدول()[idx].sg_fontset;
# endif
	// First, save the current font/fontset.
	// Then try to allocate the font/fontset.
	// If the allocation fails, HL_TABLE()[idx].sg_font OR
	// sg_fontset will be set to NOFONT or NOFONTSET respectively.

	HL_جدول()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
	HL_جدول()[idx].sg_fontset = NOFONTSET;
# endif
	hl_do_font(idx, arg, is_معمول_گروہ, is_menu_گروہ,
		is_tooltip_گروہ, FALSE);

# ifdef FEAT_XFONTSET
	اگر (HL_جدول()[idx].sg_fontset != NOFONTSET)
	{
	    // New fontset was accepted. Free the old one, if there
	    // was one.
	    gui_mch_free_fontset(عارضی_sg_fontset);
	    did_change = TRUE;
	}
	ورنہ
	    HL_جدول()[idx].sg_fontset = عارضی_sg_fontset;
# endif
	اگر (HL_جدول()[idx].sg_font != NOFONT)
	{
	    // New font was accepted. Free the old one, if there was
	    // one.
	    gui_mch_free_font(عارضی_sg_font);
	    did_change = TRUE;
	}
	ورنہ
	    HL_جدول()[idx].sg_font = عارضی_sg_font;
	// Update sg_font_name once here, after both font and fontset results
	// are known.
	اگر (did_change)
	{
	    vim_free(HL_جدول()[idx].sg_font_name);
	    HL_جدول()[idx].sg_font_name = vim_strsave(arg);
	}
    }

    واپس did_change;
}
#اگر_ختم

/*
 * Set the cterm foreground color for the Normal highlight group to "color" and
 * the bold attribute to "bold".
 */
    ساکن خالی
hl_سیٹ_ctermfg_معمول_گروہ(صحیح color, صحیح bold)
{
    cterm_معمول_fg_color = color + 1;
    cterm_معمول_fg_bold = bold;
#اگر_تعریف_شدہ FEAT_GUI
    // Don't do this if the GUI is used.
    اگر (!gui.اندر_use && !gui.starting)
#اگر_ختم
    {
	سیٹ_must_redraw(UPD_CLEAR);
	اگر (termcap_فعال && color >= 0)
	    رکن_fg_color(color);
    }
}

/*
 * Set the cterm foreground color for the highlight group at 'idx' to 'color'.
 */
    ساکن خالی
highlight_سیٹ_ctermfg(صحیح idx, صحیح color, صحیح is_معمول_گروہ)
{
    HL_جدول()[idx].sg_cterm_fg = color + 1;
    اگر (is_معمول_گروہ)
	hl_سیٹ_ctermfg_معمول_گروہ(color,
					(HL_جدول()[idx].sg_cterm & HL_BOLD));
}

/*
 * Set the cterm background color for the Normal highlight group to "color".
 */
    ساکن خالی
hl_سیٹ_ctermbg_معمول_گروہ(صحیح color)
{
    cterm_معمول_bg_color = color + 1;
#اگر_تعریف_شدہ FEAT_GUI
    // Don't mess with 'background' if the GUI is used.
    اگر (!gui.اندر_use && !gui.starting)
#اگر_ختم
    {
	سیٹ_must_redraw(UPD_CLEAR);
	اگر (color >= 0)
	{
	    صحیح dark = -1;

	    اگر (termcap_فعال)
		رکن_bg_color(color);
	    اگر (t_colors < 16)
		dark = (color == 0 || color == 4);
	    // Limit the heuristic to the standard 16 colors
	    ورنہ اگر (color < 16)
		dark = (color < 7 || color == 8);
	    // Set the 'background' option if the value is
	    // wrong.
	    اگر (dark != -1
		    && dark != (*p_bg == 'd')
		    && !option_was_سیٹ((char_u *)"bg"))
	    {
		سیٹ_option_قدر_give_err((char_u *)"bg",
			0L, (char_u *)(dark ? "dark" : "light"), 0);
		reset_option_was_سیٹ((char_u *)"bg");
	    }
	}
    }
}

/*
 * Set the cterm background color for the highlight group at 'idx' to 'color'.
 */
    ساکن خالی
highlight_سیٹ_ctermbg(صحیح idx, صحیح color, صحیح is_معمول_گروہ)
{
    HL_جدول()[idx].sg_cterm_bg = color + 1;
    اگر (is_معمول_گروہ)
	hl_سیٹ_ctermbg_معمول_گروہ(color);
}

/*
 * Set the cterm underline color for the Normal highlight group to "color".
 */
    ساکن خالی
hl_سیٹ_ctermul_معمول_گروہ(صحیح color)
{
    cterm_معمول_ul_color = color + 1;
#اگر_تعریف_شدہ FEAT_GUI
    // Don't do this if the GUI is used.
    اگر (!gui.اندر_use && !gui.starting)
#اگر_ختم
    {
	سیٹ_must_redraw(UPD_CLEAR);
	اگر (termcap_فعال && color >= 0)
	    رکن_ul_color(color);
    }
}

/*
 * Set the cterm underline color for the highlight group at 'idx' to 'color'.
 */
    ساکن خالی
highlight_سیٹ_ctermul(صحیح idx, صحیح color, صحیح is_معمول_گروہ)
{
    HL_جدول()[idx].sg_cterm_ul = color + 1;
    اگر (is_معمول_گروہ)
	hl_سیٹ_ctermul_معمول_گروہ(color);
}

/*
 * Set the cterm font for the highlight group at 'idx'.
 * 'arg' is the color name or the numeric value as a string.
 * 'init' is set to TRUE when initializing highlighting.
 * Called for the ":highlight" command and the "hlset()" function.
 *
 * Returns TRUE if the font is set.
 */
    ساکن صحیح
highlight_سیٹ_cterm_font(
	صحیح	idx,
	char_u	*arg,
	صحیح	init)
{
    صحیح		font;

    اگر (init && (HL_جدول()[idx].sg_سیٹ & SG_CTERM))
	واپس FALSE;

    اگر (!init)
	HL_جدول()[idx].sg_سیٹ |= SG_CTERM;

    اگر (VIM_ISDIGIT(*arg))
	font = atoi((حرف *)arg);
    ورنہ اگر (STRICMP(arg, "NONE") == 0)
	font = -1;
    ورنہ
	واپس FALSE;

    HL_جدول()[idx].sg_cterm_font = font + 1;
    واپس TRUE;
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
    ساکن صحیح
highlight_سیٹ_cterm_color(
	صحیح	idx,
	char_u	*کلید,
	char_u	*کلید_آغاز,
	char_u	*arg,
	صحیح	is_معمول_گروہ,
	صحیح	init)
{
    صحیح		color;

    اگر (init && (HL_جدول()[idx].sg_سیٹ & SG_CTERM))
	واپس FALSE;

    اگر (!init)
	HL_جدول()[idx].sg_سیٹ |= SG_CTERM;

    // When setting the foreground color, and previously the "bold"
    // flag was set for a light color, reset it now
    اگر (کلید[5] == 'F' && HL_جدول()[idx].sg_cterm_bold)
    {
	HL_جدول()[idx].sg_cterm &= ~HL_BOLD;
	HL_جدول()[idx].sg_cterm_bold = FALSE;
    }

    اگر (VIM_ISDIGIT(*arg))
	color = atoi((حرف *)arg);
    ورنہ اگر (STRICMP(arg, "fg") == 0)
    {
	اگر (cterm_معمول_fg_color)
	    color = cterm_معمول_fg_color - 1;
	ورنہ
	{
	    emsg(_(e_fg_color_unknown));
	    واپس FALSE;
	}
    }
    ورنہ اگر (STRICMP(arg, "bg") == 0)
    {
	اگر (cterm_معمول_bg_color > 0)
	    color = cterm_معمول_bg_color - 1;
	ورنہ
	{
	    emsg(_(e_bg_color_unknown));
	    واپس FALSE;
	}
    }
    ورنہ اگر (STRICMP(arg, "ul") == 0)
    {
	اگر (cterm_معمول_ul_color > 0)
	    color = cterm_معمول_ul_color - 1;
	ورنہ
	{
	    emsg(_(e_ul_color_unknown));
	    واپس FALSE;
	}
    }
    ورنہ
    {
	صحیح bold = MAYBE;
	keyvalue_T ہدف;
	keyvalue_T *اندراج_2;

	ہدف.کلید = 0;
	ہدف.قدر.string = arg;
	ہدف.قدر.لمبائی = 0;	// not used, see cmp_keyvalue_value_i()
	اندراج_2 = (keyvalue_T *)bsearch(&ہدف, &color_name_tab,
		ARRAY_لمبائی(color_name_tab), حجم(color_name_tab[0]),
		cmp_keyvalue_قدر_i);
	اگر (اندراج_2 == تہی)
	{
	    semsg(_(e_color_name_or_نمبر_not_recognized_str), کلید_آغاز);
	    واپس FALSE;
	}

	color = lookup_color(اندراج_2->کلید, کلید[5] == 'F', &bold);

	// set/reset bold attribute to get light foreground
	// colors (on some terminals, e.g. "linux")
	اگر (bold == TRUE)
	{
	    HL_جدول()[idx].sg_cterm |= HL_BOLD;
	    HL_جدول()[idx].sg_cterm_bold = TRUE;
	}
	ورنہ اگر (bold == FALSE)
	    HL_جدول()[idx].sg_cterm &= ~HL_BOLD;
    }

    // Add one to the argument, to avoid zero.  Zero is used for
    // "NONE", then "color" is -1.
    اگر (کلید[5] == 'F')
	highlight_سیٹ_ctermfg(idx, color, is_معمول_گروہ);
    ورنہ اگر (کلید[5] == 'B')
	highlight_سیٹ_ctermbg(idx, color, is_معمول_گروہ);
    ورنہ // ctermul
	highlight_سیٹ_ctermul(idx, color, is_معمول_گروہ);

    واپس TRUE;
}

#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
/*
 * Set the GUI foreground color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ساکن صحیح
highlight_سیٹ_guifg(
	صحیح	idx,
	char_u	*arg,
	صحیح	is_menu_گروہ غیر_مستعمل,
	صحیح	is_scrollbar_گروہ غیر_مستعمل,
	صحیح	is_tooltip_گروہ غیر_مستعمل,
	صحیح	is_titlebar_گروہ غیر_مستعمل,
	صحیح	is_titlebarnc_گروہ غیر_مستعمل,
	صحیح	*do_colors غیر_مستعمل,
	صحیح	init)
{
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    طویل	i;
# endif
    char_u	**namep;
    صحیح		did_change = FALSE;

    اگر (init && (HL_جدول()[idx].sg_سیٹ & SG_GUI))
	واپس FALSE;

    namep = &HL_جدول()[idx].sg_gui_fg_name;
    اگر (!init)
	HL_جدول()[idx].sg_سیٹ |= SG_GUI;

# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guifg colors are only used when recognized
    i = color_name2handle(arg);
    اگر (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24_ثنائی_ہندسہ)
    {
	HL_جدول()[idx].sg_gui_fg = i;
# endif
	اگر (*namep == تہی || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    اگر (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    ورنہ
		*namep = تہی;
	    did_change = TRUE;
	}
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	اگر (is_titlebar_گروہ && gui.title_fg_pixel != i)
	{
	    gui.title_fg_pixel = i;
	    *do_colors = TRUE;
	}
	اگر (is_titlebarnc_گروہ && gui.titlenc_fg_pixel != i)
	{
	    gui.titlenc_fg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	اگر (is_menu_گروہ && gui.menu_fg_pixel != i)
	{
	    gui.menu_fg_pixel = i;
	    *do_colors = TRUE;
	}
	اگر (is_scrollbar_گروہ && gui.scroll_fg_pixel != i)
	{
	    gui.scroll_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	اگر (is_tooltip_گروہ && gui.tooltip_fg_pixel != i)
	{
	    gui.tooltip_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    واپس did_change;
}

/*
 * Set the GUI background color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ساکن صحیح
highlight_سیٹ_guibg(
	صحیح	idx,
	char_u	*arg,
	صحیح	is_menu_گروہ غیر_مستعمل,
	صحیح	is_scrollbar_گروہ غیر_مستعمل,
	صحیح	is_tooltip_گروہ غیر_مستعمل,
	صحیح	is_titlebar_گروہ غیر_مستعمل,
	صحیح	is_titlebarnc_گروہ غیر_مستعمل,
	صحیح	*do_colors غیر_مستعمل,
	صحیح	init)
{
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    صحیح		i;
# endif
    char_u	**namep;
    صحیح		did_change = FALSE;

    اگر (init && (HL_جدول()[idx].sg_سیٹ & SG_GUI))
	واپس FALSE;

    namep = &HL_جدول()[idx].sg_gui_bg_name;
    اگر (!init)
	HL_جدول()[idx].sg_سیٹ |= SG_GUI;

# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guibg colors are only used when recognized
    i = color_name2handle(arg);
    اگر (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24_ثنائی_ہندسہ)
    {
	HL_جدول()[idx].sg_gui_bg = i;
# endif
	اگر (*namep == تہی || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    اگر (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    ورنہ
		*namep = تہی;
	    did_change = TRUE;
	}
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	اگر (is_titlebar_گروہ && gui.title_bg_pixel != i)
	{
	    gui.title_bg_pixel = i;
	    *do_colors = TRUE;
	}
	اگر (is_titlebarnc_گروہ && gui.titlenc_bg_pixel != i)
	{
	    gui.titlenc_bg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	اگر (is_menu_گروہ && gui.menu_bg_pixel != i)
	{
	    gui.menu_bg_pixel = i;
	    *do_colors = TRUE;
	}
	اگر (is_scrollbar_گروہ && gui.scroll_bg_pixel != i)
	{
	    gui.scroll_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	اگر (is_tooltip_گروہ && gui.tooltip_bg_pixel != i)
	{
	    gui.tooltip_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    واپس did_change;
}

/*
 * Set the GUI undercurl/strikethrough color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ساکن صحیح
highlight_سیٹ_guisp(صحیح idx, char_u *arg, صحیح init)
{
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    صحیح		i;
# endif
    صحیح		did_change = FALSE;
    char_u	**namep;

    اگر (init && (HL_جدول()[idx].sg_سیٹ & SG_GUI))
	واپس FALSE;

    namep = &HL_جدول()[idx].sg_gui_sp_name;
    اگر (!init)
	HL_جدول()[idx].sg_سیٹ |= SG_GUI;

# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guisp colors are only used when recognized
    i = color_name2handle(arg);
    اگر (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24_ثنائی_ہندسہ)
    {
	HL_جدول()[idx].sg_gui_sp = i;
# endif
	اگر (*namep == تہی || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    اگر (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    ورنہ
		*namep = تہی;
	    did_change = TRUE;
	}
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    }
# endif

    واپس did_change;
}
#اگر_ختم

/*
 * Set the start/stop terminal codes for a highlight group.
 * Returns TRUE if the terminal code is set.
 */
    ساکن صحیح
highlight_سیٹ_startstop_termcode(صحیح idx, char_u *کلید, char_u *arg, صحیح init)
{
    صحیح		off;
    char_u	buf[100];
    صحیح		len;
    char_u	*tname;
    char_u	*p;

    اگر (!init)
	HL_جدول()[idx].sg_سیٹ |= SG_رکن;

    // The "start" and "stop"  arguments can be a literal escape
    // sequence, or a comma separated list of terminal codes.
    اگر (STRNCMP(arg, "t_", 2) == 0)
    {
	off = 0;
	buf[0] = 0;
	جب (arg[off] != NUL)
	{
	    // Isolate one termcap name
	    برائے (len = 0; arg[off + len] &&
		    arg[off + len] != ','; ++len)
		;
	    tname = vim_strnsave(arg + off, len);
	    اگر (tname == تہی)		// out of memory
		واپس FALSE;
	    // lookup the escape sequence for the item
	    p = get_رکن_رمز(tname);
	    vim_free(tname);
	    اگر (p == تہی)	    // ignore non-existing things
		p = (char_u *)"";

	    // Append it to the already found stuff
	    اگر ((صحیح)(STRLEN(buf) + STRLEN(p)) >= 99)
	    {
		semsg(_(e_terminal_رمز_too_long_str), arg);
		واپس FALSE;
	    }
	    STRCAT(buf, p);

	    // Advance to the next item
	    off += len;
	    اگر (arg[off] == ',')	    // another one follows
		++off;
	}
    }
    ورنہ
    {
	// Copy characters from arg[] to buf[], translating <> codes.
	برائے (p = arg, off = 0; off < 100 - 6 && *p; )
	{
	    len = trans_special(&p, buf + off, FSK_SIMPLIFY, FALSE, تہی);
	    اگر (len > 0)	    // recognized special char
		off += len;
	    ورنہ		    // copy as normal char
		buf[off++] = *p++;
	}
	buf[off] = NUL;
    }

    اگر (STRCMP(buf, "NONE") == 0)	// resetting the value
	p = تہی;
    ورنہ
	p = vim_strsave(buf);
    اگر (کلید[2] == 'A')
    {
	vim_free(HL_جدول()[idx].sg_آغاز);
	HL_جدول()[idx].sg_آغاز = p;
    }
    ورنہ
    {
	vim_free(HL_جدول()[idx].sg_stop);
	HL_جدول()[idx].sg_stop = p;
    }
    واپس TRUE;
}

/*
 * Handle the ":highlight .." command.
 * When using ":hi clear" this is called recursively for each group with
 * "forceit" and "init" both TRUE.
 */
    خالی
do_highlight(
    char_u	*سطر,
    صحیح		forceit,
    صحیح		init)	    // TRUE when called for initializing
{
    char_u	*name_انتہا;
    char_u	*linep;
    char_u	*کلید_آغاز;
    char_u	*arg_آغاز;
    char_u	*کلید = تہی, *arg = تہی;
    طویل	i;
    صحیح		id;
    صحیح		idx;
    hl_گروہ_T	شے_before;
    صحیح		did_change = FALSE;
    صحیح		dodefault = FALSE;
    صحیح		doclear = FALSE;
    صحیح		dolink = FALSE;
    صحیح		error = FALSE;
    صحیح		is_معمول_گروہ = FALSE;	// "Normal" group
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
    صحیح		is_titlebar_گروہ = FALSE;	// "TitleBar" group
    صحیح		is_titlebarnc_گروہ = FALSE;	// "TitleBarNC" group
#ورنہ
# define is_titlebar_گروہ 0
# define is_titlebarnc_گروہ 0
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_X11
    صحیح		is_menu_گروہ = FALSE;		// "Menu" group
    صحیح		is_scrollbar_گروہ = FALSE;	// "Scrollbar" group
    صحیح		is_tooltip_گروہ = FALSE;	// "Tooltip" group
#ورنہ
# define is_menu_گروہ 0
# define is_tooltip_گروہ 0
# define is_scrollbar_گروہ 0
#اگر_ختم
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
    صحیح		do_colors = FALSE;		// need to update colors?
#اگر_ختم
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    صحیح		did_highlight_بدلا = FALSE;
#اگر_ختم

    // If no argument, list current highlighting.
    اگر (!init && سرے_excmd2(سطر - 1, سطر))
    {
	برائے (i = 1; i <= highlight_ga.ga_len && !got_int; ++i)
	    // TODO: only call when the group has attributes set
	    highlight_list_one((صحیح)i);
	واپس;
    }

    // Isolate the name.
    name_انتہا = skiptowhite(سطر);
    linep = skipwhite(name_انتہا);

    // Check for "default" argument.
    اگر (STRNCMP(سطر, "default", name_انتہا - سطر) == 0)
    {
	dodefault = TRUE;
	سطر = linep;
	name_انتہا = skiptowhite(سطر);
	linep = skipwhite(name_انتہا);
    }

    // Check for "clear" or "link" argument.
    اگر (STRNCMP(سطر, "clear", name_انتہا - سطر) == 0)
	doclear = TRUE;
    اگر (STRNCMP(سطر, "link", name_انتہا - سطر) == 0)
	dolink = TRUE;

    // ":highlight {group-name}": list highlighting for one group.
    اگر (!doclear && !dolink && سرے_excmd2(سطر, linep))
    {
	id = syn_namen2id(سطر, (صحیح)(name_انتہا - سطر));
	اگر (id == 0)
	    semsg(_(e_highlight_گروہ_name_not_ملا_str), سطر);
	ورنہ
	    highlight_list_one(id);
	واپس;
    }

    // Handle ":highlight link {from} {to}" command.
    اگر (dolink)
    {
	char_u	    *from_آغاز = linep;
	char_u	    *from_انتہا;
	صحیح	    from_len;
	char_u	    *مقصد_آغاز;
	char_u	    *مقصد_انتہا;
	صحیح	    مقصد_len;

	from_انتہا = skiptowhite(from_آغاز);
	مقصد_آغاز = skipwhite(from_انتہا);
	مقصد_انتہا	 = skiptowhite(مقصد_آغاز);

	اگر (سرے_excmd2(سطر, from_آغاز) || سرے_excmd2(سطر, مقصد_آغاز))
	{
	    semsg(_(e_not_enough_arguments_highlight_link_str), from_آغاز);
	    واپس;
	}

	اگر (!سرے_excmd2(سطر, skipwhite(مقصد_انتہا)))
	{
	    semsg(_(e_too_many_arguments_highlight_link_str), from_آغاز);
	    واپس;
	}

	from_len = (صحیح)(from_انتہا - from_آغاز);
	مقصد_len = (صحیح)(مقصد_انتہا - مقصد_آغاز);
	highlight_گروہ_link(from_آغاز, from_len, مقصد_آغاز, مقصد_len,
						dodefault, forceit, init);
	واپس;
    }

    اگر (doclear)
    {
	// ":highlight clear [group]" command.
	اگر (سرے_excmd2(سطر, linep))
	{
	    // ":highlight clear" without group name
	    highlight_reset_all();
	    واپس;
	}
	سطر = linep;
	name_انتہا = skiptowhite(سطر);
	linep = skipwhite(name_انتہا);
    }

    // Find the group name in the table.  If it does not exist yet, add it.
    id = syn_check_گروہ(سطر, (صحیح)(name_انتہا - سطر));
    اگر (id == 0)	// failed (out of memory)
	واپس;
    idx = id - 1;	// index is ID minus one

    // Return if "default" was used and the group already has settings.
    اگر (dodefault && hl_has_settings(idx, TRUE))
	واپس;

    // Make a copy so we can check if any attribute actually changed.
    شے_before = HL_جدول()[idx];

    اگر (STRCMP(HL_جدول()[idx].sg_name_u, "NORMAL") == 0)
	is_معمول_گروہ = TRUE;
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
    ورنہ اگر (STRCMP(HL_جدول()[idx].sg_name_u, "TITLEBAR") == 0)
	is_titlebar_گروہ = TRUE;
    ورنہ اگر (STRCMP(HL_جدول()[idx].sg_name_u, "TITLEBARNC") == 0)
	is_titlebarnc_گروہ = TRUE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_X11
    ورنہ اگر (STRCMP(HL_جدول()[idx].sg_name_u, "MENU") == 0)
	is_menu_گروہ = TRUE;
    ورنہ اگر (STRCMP(HL_جدول()[idx].sg_name_u, "SCROLLBAR") == 0)
	is_scrollbar_گروہ = TRUE;
    ورنہ اگر (STRCMP(HL_جدول()[idx].sg_name_u, "TOOLTIP") == 0)
	is_tooltip_گروہ = TRUE;
#اگر_ختم

    // Clear the highlighting for ":hi clear {group}" and ":hi clear".
    اگر (doclear || (forceit && init))
    {
	highlight_clear(idx);
	اگر (!doclear)
	    HL_جدول()[idx].sg_سیٹ = 0;
    }

    اگر (!doclear)
	جب (!سرے_excmd2(سطر, linep))
	{
	    کلید_آغاز = linep;
	    اگر (*linep == '=')
	    {
		semsg(_(e_unexpected_equal_sign_str), کلید_آغاز);
		error = TRUE;
		توڑو;
	    }

	    // Note: Keep this in sync with get_highlight_group_key.

	    // Isolate the key ("term", "ctermfg", "ctermbg", "font", "guifg"
	    // or "guibg").
	    جب (*linep && !VIM_ISWHITE(*linep) && *linep != '=')
		++linep;
	    vim_free(کلید);
	    کلید = vim_strnsave_اوپر(کلید_آغاز, linep - کلید_آغاز);
	    اگر (کلید == تہی)
	    {
		error = TRUE;
		توڑو;
	    }
	    linep = skipwhite(linep);

	    اگر (STRCMP(کلید, "NONE") == 0)
	    {
		اگر (!init || HL_جدول()[idx].sg_سیٹ == 0)
		{
		    اگر (!init)
			HL_جدول()[idx].sg_سیٹ |= SG_رکن+SG_CTERM+SG_GUI;
		    highlight_clear(idx);
		}
		جاری;
	    }

	    // Check for the equal sign.
	    اگر (*linep != '=')
	    {
		semsg(_(e_missing_equal_sign_str_2), کلید_آغاز);
		error = TRUE;
		توڑو;
	    }
	    ++linep;

	    // Isolate the argument.
	    linep = skipwhite(linep);
	    اگر (*linep == '\'')		// guifg='color name'
	    {
		arg_آغاز = ++linep;
		linep = vim_strchr(linep, '\'');
		اگر (linep == تہی)
		{
		    semsg(_(e_invalid_دلیل_str), کلید_آغاز);
		    error = TRUE;
		    توڑو;
		}
	    }
	    ورنہ
	    {
		arg_آغاز = linep;
		linep = skiptowhite(linep);
	    }
	    اگر (linep == arg_آغاز)
	    {
		semsg(_(e_missing_دلیل_str), کلید_آغاز);
		error = TRUE;
		توڑو;
	    }
	    vim_free(arg);
	    arg = vim_strnsave(arg_آغاز, linep - arg_آغاز);
	    اگر (arg == تہی)
	    {
		error = TRUE;
		توڑو;
	    }
	    اگر (*linep == '\'')
		++linep;

	    // Store the argument.
	    اگر (STRCMP(کلید, "TERM") == 0
		    || STRCMP(کلید, "CTERM") == 0
		    || STRCMP(کلید, "GUI") == 0)
	    {
		اگر (!highlight_سیٹ_termgui_attr(idx, کلید, arg, init))
		{
		    error = TRUE;
		    توڑو;
		}
	    }
	    ورنہ اگر (STRCMP(کلید, "FONT") == 0)
	    {
		// in non-GUI fonts are simply ignored
#اگر_تعریف_شدہ FEAT_GUI
		اگر (highlight_سیٹ_font(idx, arg, is_معمول_گروہ,
					      is_menu_گروہ, is_tooltip_گروہ))
		    did_change = TRUE;
#اگر_ختم
	    }
	    ورنہ اگر (STRCMP(کلید, "CTERMFG") == 0
		    || STRCMP(کلید, "CTERMBG") == 0
		    || STRCMP(کلید, "CTERMUL") == 0)
	    {
		اگر (!highlight_سیٹ_cterm_color(idx, کلید, کلید_آغاز, arg,
							is_معمول_گروہ, init))
		{
		    error = TRUE;
		    توڑو;
		}
	    }
	    ورنہ اگر (STRCMP(کلید, "CTERMFONT") == 0)
	    {
		اگر (!highlight_سیٹ_cterm_font(idx, arg, init))
		{
		    error = TRUE;
		    توڑو;
		}
	    }
	    ورنہ اگر (STRCMP(کلید, "GUIFG") == 0)
	    {
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
		اگر (highlight_سیٹ_guifg(idx, arg, is_menu_گروہ,
			    is_scrollbar_گروہ, is_tooltip_گروہ,
			    is_titlebar_گروہ, is_titlebarnc_گروہ,
			    &do_colors, init))
		    did_change = TRUE;
#اگر_ختم
	    }
	    ورنہ اگر (STRCMP(کلید, "GUIBG") == 0)
	    {
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
		اگر (highlight_سیٹ_guibg(idx, arg, is_menu_گروہ,
			    is_scrollbar_گروہ, is_tooltip_گروہ,
			    is_titlebar_گروہ, is_titlebarnc_گروہ,
			    &do_colors, init))
		    did_change = TRUE;
#اگر_ختم
	    }
	    ورنہ اگر (STRCMP(کلید, "GUISP") == 0)
	    {
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
		اگر (highlight_سیٹ_guisp(idx, arg, init))
		    did_change = TRUE;
#اگر_ختم
	    }
	    ورنہ اگر (STRCMP(کلید, "START") == 0 || STRCMP(کلید, "STOP") == 0)
	    {
		اگر (!highlight_سیٹ_startstop_termcode(idx, کلید, arg, init))
		{
		    error = TRUE;
		    توڑو;
		}
	    }
	    ورنہ
	    {
		semsg(_(e_illegal_دلیل_str_3), کلید_آغاز);
		error = TRUE;
		توڑو;
	    }
	    HL_جدول()[idx].sg_cleared = FALSE;

	    // When highlighting has been given for a group, don't link it.
	    اگر (!init || !(HL_جدول()[idx].sg_سیٹ & SG_LINK))
		HL_جدول()[idx].sg_link = 0;

	    // Continue with next argument.
	    linep = skipwhite(linep);
	}

    // If there is an error, and it's a new entry, remove it from the table.
    اگر (error && idx == highlight_ga.ga_len)
	syn_unadd_گروہ();
    ورنہ
    {
	اگر (is_معمول_گروہ)
	{
	    HL_جدول()[idx].sg_رکن_attr = 0;
	    HL_جدول()[idx].sg_cterm_attr = 0;
#اگر_تعریف_شدہ FEAT_GUI
	    HL_جدول()[idx].sg_gui_attr = 0;
	    // Need to update all groups, because they might be using "bg"
	    // and/or "fg", which have been changed now.
#اگر_ختم
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    اگر (USE_24_ثنائی_ہندسہ)
	    {
		highlight_gui_started();
		did_highlight_بدلا = TRUE;
		redraw_all_later(UPD_NOT_VALID);
	    }
#اگر_ختم
	    resolve_fallback_fg_مقصد_rgb();
	    resolve_fallback_bg_مقصد_rgb();
#اگر_تعریف_شدہ FEAT_VTP
	    control_console_color_rgb();
#اگر_ختم
	}
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
	ورنہ اگر (is_titlebar_گروہ)
	{
	    اگر (gui.اندر_use && do_colors)
		gui_mch_سیٹ_titlebar_colors();
	}
	ورنہ اگر (is_titlebarnc_گروہ)
	{
	    اگر (gui.اندر_use && do_colors)
		gui_mch_سیٹ_titlebar_colors();
	}
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_X11
# ifdef FEAT_MENU
	ورنہ اگر (is_menu_گروہ)
	{
	    اگر (gui.اندر_use && do_colors)
		gui_mch_new_menu_colors();
	}
# endif
	ورنہ اگر (is_scrollbar_گروہ)
	{
	    اگر (gui.اندر_use && do_colors)
		gui_new_scrollbar_colors();
	    ورنہ
		سیٹ_hl_attr(idx);
	}
# ifdef FEAT_BEVAL_GUI
	ورنہ اگر (is_tooltip_گروہ)
	{
	    اگر (gui.اندر_use && do_colors)
		gui_mch_new_tooltip_colors();
	}
# endif
#اگر_ختم
	ورنہ
	    سیٹ_hl_attr(idx);
#اگر_تعریف_شدہ FEAT_EVAL
	HL_جدول()[idx].sg_script_ctx = current_sctx;
	HL_جدول()[idx].sg_script_ctx.sc_lnum += SOURCING_LNUM;
#اگر_ختم
    }

    vim_free(کلید);
    vim_free(arg);

    // Only call highlight_changed() once, after a sequence of highlight
    // commands, and only if an attribute actually changed.
    اگر ((did_change
	   || memcmp(&HL_جدول()[idx], &شے_before, حجم(شے_before)) != 0)
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    && !did_highlight_بدلا
#اگر_ختم
       )
    {
	// Do not trigger a redraw when highlighting is changed while
	// redrawing.  This may happen when evaluating 'statusline' changes the
	// StatusLine group.
	اگر (!updating_screen)
	    redraw_all_later(UPD_NOT_VALID);
	need_highlight_بدلا = TRUE;
    }
}

#اگر defined(EXITFREE)
    خالی
free_highlight(خالی)
{
    صحیح	    i;

    برائے (i = 0; i < highlight_ga.ga_len; ++i)
    {
	highlight_clear(i);
	vim_free(HL_جدول()[i].sg_name);
	vim_free(HL_جدول()[i].sg_name_u - HLNAME_کلید_OFF);
    }
    ga_clear(&highlight_ga);
    hash_clear(&highlight_ht);
    highlight_ht_inited = false;
}
#اگر_ختم

/*
 * Reset the cterm colors to what they were before Vim was started, if
 * possible.  Otherwise reset them to zero.
 */
    خالی
restore_cterm_colors(خالی)
{
#اگر defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
    // Since t_me has been set, this probably means that the user
    // wants to use this as default colors.  Need to reset default
    // background/foreground colors.
    mch_سیٹ_معمول_colors();
#ورنہ
# ifdef VIMDLL
    اگر (!gui.اندر_use)
    {
	mch_سیٹ_معمول_colors();
	واپس;
    }
# endif
    cterm_معمول_fg_color = 0;
    cterm_معمول_fg_bold = 0;
    cterm_معمول_bg_color = 0;
# ifdef FEAT_TERMGUICOLORS
    cterm_معمول_fg_gui_color = INVALCOLOR;
    cterm_معمول_bg_gui_color = INVALCOLOR;
    cterm_معمول_ul_gui_color = INVALCOLOR;
# endif
    fallback_fg_rgb = INVALCOLOR;
    fallback_bg_rgb = INVALCOLOR;
#اگر_ختم
}

/*
 * Return TRUE if highlight group "idx" has any settings.
 * When "check_link" is TRUE also check for an existing link.
 */
    ساکن صحیح
hl_has_settings(صحیح idx, صحیح check_link)
{
    واپس HL_جدول()[idx].sg_cleared == 0
	 && (  HL_جدول()[idx].sg_رکن_attr != 0
	    || HL_جدول()[idx].sg_cterm_attr != 0
	    || HL_جدول()[idx].sg_cterm_fg != 0
	    || HL_جدول()[idx].sg_cterm_bg != 0
	    || HL_جدول()[idx].sg_cterm_font != 0
#اگر_تعریف_شدہ FEAT_GUI
	    || HL_جدول()[idx].sg_gui_attr != 0
	    || HL_جدول()[idx].sg_gui_fg_name != تہی
	    || HL_جدول()[idx].sg_gui_bg_name != تہی
	    || HL_جدول()[idx].sg_gui_sp_name != تہی
	    || HL_جدول()[idx].sg_font_name != تہی
#اگر_ختم
	    || (check_link && (HL_جدول()[idx].sg_سیٹ & SG_LINK)));
}

/*
 * Clear highlighting for one group.
 */
    ساکن خالی
highlight_clear(صحیح idx)
{
    HL_جدول()[idx].sg_cleared = TRUE;

    HL_جدول()[idx].sg_رکن = 0;
    VIM_CLEAR(HL_جدول()[idx].sg_آغاز);
    VIM_CLEAR(HL_جدول()[idx].sg_stop);
    HL_جدول()[idx].sg_رکن_attr = 0;
    HL_جدول()[idx].sg_cterm = 0;
    HL_جدول()[idx].sg_cterm_bold = FALSE;
    HL_جدول()[idx].sg_cterm_fg = 0;
    HL_جدول()[idx].sg_cterm_bg = 0;
    HL_جدول()[idx].sg_cterm_attr = 0;
    HL_جدول()[idx].sg_cterm_font = 0;
#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
    HL_جدول()[idx].sg_gui = 0;
    VIM_CLEAR(HL_جدول()[idx].sg_gui_fg_name);
    VIM_CLEAR(HL_جدول()[idx].sg_gui_bg_name);
    VIM_CLEAR(HL_جدول()[idx].sg_gui_sp_name);
#اگر_ختم
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_جدول()[idx].sg_gui_fg = INVALCOLOR;
    HL_جدول()[idx].sg_gui_bg = INVALCOLOR;
    HL_جدول()[idx].sg_gui_sp = INVALCOLOR;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
    gui_mch_free_font(HL_جدول()[idx].sg_font);
    HL_جدول()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
    gui_mch_free_fontset(HL_جدول()[idx].sg_fontset);
    HL_جدول()[idx].sg_fontset = NOFONTSET;
# endif
    VIM_CLEAR(HL_جدول()[idx].sg_font_name);
    HL_جدول()[idx].sg_gui_attr = 0;
#اگر_ختم
    // Restore default link and context if they exist. Otherwise clears.
    HL_جدول()[idx].sg_link = HL_جدول()[idx].sg_deflink;
#اگر_تعریف_شدہ FEAT_EVAL
    // Since we set the default link, set the location to where the default
    // link was set.
    HL_جدول()[idx].sg_script_ctx = HL_جدول()[idx].sg_deflink_sctx;
#اگر_ختم
}

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Set the normal foreground and background colors according to the "Normal"
 * highlighting group.  For X11 also set "Menu", "Scrollbar" and "Tooltip"
 * colors.  For MS-Windows also set "TitleBar" and "TitleBarNC" colors.
 */
    خالی
سیٹ_معمول_colors(خالی)
{
# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    اگر (gui.اندر_use)
#  endif
    {
	اگر (سیٹ_گروہ_colors((char_u *)"Normal",
				 &gui.norm_pixel, &gui.عقب_pixel,
				 FALSE, TRUE, FALSE))
	{
	    gui_mch_new_colors();
	    سیٹ_must_redraw(UPD_CLEAR);
	}
#  ifdef FEAT_GUI_MSWIN
	اگر (سیٹ_گروہ_colors((char_u *)"TitleBar",
				 &gui.title_fg_pixel, &gui.title_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_سیٹ_titlebar_colors();
	    سیٹ_must_redraw(UPD_CLEAR);
	}
	اگر (سیٹ_گروہ_colors((char_u *)"TitleBarNC",
				 &gui.titlenc_fg_pixel, &gui.titlenc_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_سیٹ_titlebar_colors();
	    سیٹ_must_redraw(UPD_CLEAR);
	}
#  endif
#  ifdef FEAT_GUI_X11
	اگر (سیٹ_گروہ_colors((char_u *)"Menu",
			     &gui.menu_fg_pixel, &gui.menu_bg_pixel,
			     TRUE, FALSE, FALSE))
	{
#   ifdef FEAT_MENU
	    gui_mch_new_menu_colors();
#   endif
	    سیٹ_must_redraw(UPD_CLEAR);
	}
#   ifdef FEAT_BEVAL_GUI
	اگر (سیٹ_گروہ_colors((char_u *)"Tooltip",
			     &gui.tooltip_fg_pixel, &gui.tooltip_bg_pixel,
			     FALSE, FALSE, TRUE))
	{
#    ifdef FEAT_TOOLBAR
	    gui_mch_new_tooltip_colors();
#    endif
	    سیٹ_must_redraw(UPD_CLEAR);
	}
#   endif
	اگر (سیٹ_گروہ_colors((char_u *)"Scrollbar",
			&gui.scroll_fg_pixel, &gui.scroll_bg_pixel,
			FALSE, FALSE, FALSE))
	{
	    gui_new_scrollbar_colors();
	    سیٹ_must_redraw(UPD_CLEAR);
	}
#  endif
    }
# endif
# ifdef FEAT_TERMGUICOLORS
#  ifdef FEAT_GUI
    ورنہ
#  endif
    {
	صحیح		idx;

	idx = syn_name2id((char_u *)"Normal") - 1;
	اگر (idx >= 0)
	{
	    gui_do_one_color(idx, FALSE, FALSE);

	    // If the normal fg or bg color changed a complete redraw is
	    // required.
	    اگر (cterm_معمول_fg_gui_color != HL_جدول()[idx].sg_gui_fg
		    || cterm_معمول_bg_gui_color != HL_جدول()[idx].sg_gui_bg)
	    {
		// if the GUI color is INVALCOLOR then we use the default cterm
		// color
		cterm_معمول_fg_gui_color = HL_جدول()[idx].sg_gui_fg;
		cterm_معمول_bg_gui_color = HL_جدول()[idx].sg_gui_bg;
		سیٹ_must_redraw(UPD_CLEAR);
	    }
	}
    }
# endif
}
#اگر_ختم

#اگر defined(FEAT_GUI)
/*
 * Set the colors for "Normal", "Menu", "TitleBar", "TitleBarNC", "Tooltip" or
 * "Scrollbar".
 */
    ساکن صحیح
سیٹ_گروہ_colors(
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
	واپس FALSE;

    gui_do_one_color(idx, do_menu, do_tooltip);

    اگر (HL_جدول()[idx].sg_gui_fg != INVALCOLOR)
	*fgp = HL_جدول()[idx].sg_gui_fg;
    ورنہ اگر (use_norm)
	*fgp = gui.def_norm_pixel;
    اگر (HL_جدول()[idx].sg_gui_bg != INVALCOLOR)
	*bgp = HL_جدول()[idx].sg_gui_bg;
    ورنہ اگر (use_norm)
	*bgp = gui.def_عقب_pixel;
    واپس TRUE;
}

/*
 * Get the font of the "Normal" group.
 * Returns "" when it's not found or not set.
 */
    char_u *
hl_get_font_name(خالی)
{
    صحیح		id;
    char_u	*s;

    id = syn_name2id((char_u *)"Normal");
    اگر (id > 0)
    {
	s = HL_جدول()[id - 1].sg_font_name;
	اگر (s != تہی)
	    واپس s;
    }
    واپس (char_u *)"";
}

/*
 * Set font for "Normal" group.  Called by gui_mch_init_font() when a font has
 * actually chosen to be used.
 */
    خالی
hl_سیٹ_font_name(char_u *font_name)
{
    صحیح	    id;

    id = syn_name2id((char_u *)"Normal");
    اگر (id <= 0)
	واپس;

    vim_free(HL_جدول()[id - 1].sg_font_name);
    HL_جدول()[id - 1].sg_font_name = vim_strsave(font_name);
}

/*
 * Set background color for "Normal" group.  Called by gui_set_bg_color()
 * when the color is known.
 */
    خالی
hl_سیٹ_bg_color_name(
    char_u  *name)	    // must have been allocated
{
    صحیح	    id;

    اگر (name == تہی)
	واپس;

    id = syn_name2id((char_u *)"Normal");
    اگر (id <= 0)
	واپس;

    vim_free(HL_جدول()[id - 1].sg_gui_bg_name);
    HL_جدول()[id - 1].sg_gui_bg_name = name;
}

/*
 * Set foreground color for "Normal" group.  Called by gui_set_fg_color()
 * when the color is known.
 */
    خالی
hl_سیٹ_fg_color_name(
    char_u  *name)	    // must have been allocated
{
    صحیح	    id;

    اگر (name == تہی)
	واپس;

    id = syn_name2id((char_u *)"Normal");
    اگر (id <= 0)
	واپس;

    vim_free(HL_جدول()[id - 1].sg_gui_fg_name);
    HL_جدول()[id - 1].sg_gui_fg_name = name;
}

/*
 * Return the handle for a font name.
 * Returns NOFONT when failed.
 */
    ساکن GuiFont
font_name2handle(char_u *name)
{
    اگر (STRCMP(name, "NONE") == 0)
	واپس NOFONT;

    واپس gui_mch_get_font(name, TRUE);
}

# ifdef FEAT_XFONTSET
/*
 * Return the handle for a fontset name.
 * Returns NOFONTSET when failed.
 */
    ساکن GuiFontset
fontset_name2handle(char_u *name, صحیح fixed_چوڑائی)
{
    اگر (STRCMP(name, "NONE") == 0)
	واپس NOFONTSET;

    واپس gui_mch_get_fontset(name, TRUE, fixed_چوڑائی);
}
# endif

/*
 * Get the font or fontset for one highlight group.
 */
    ساکن خالی
hl_do_font(
    صحیح		idx,
    char_u	*arg,
    صحیح		do_معمول,		// set normal font
    صحیح		do_menu غیر_مستعمل,		// set menu font
    صحیح		do_tooltip غیر_مستعمل,	// set tooltip font
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
	    gui_mch_free_fontset(HL_جدول()[idx].sg_fontset);
	HL_جدول()[idx].sg_fontset = fontset_name2handle(arg, 0
#  ifdef FONTSET_ALWAYS
		|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
		|| do_tooltip
#  endif
		);
    }
    اگر (HL_جدول()[idx].sg_fontset != NOFONTSET)
    {
	// If it worked and it's the Normal group, use it as the normal
	// fontset.  Same for the Menu group.
	اگر (do_معمول)
	    gui_init_font(arg, TRUE);
#  اگر defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	اگر (do_menu)
	{
#   ifdef FONTSET_ALWAYS
	    gui.menu_fontset = HL_جدول()[idx].sg_fontset;
#   ورنہ
	    // YIKES!  This is a bug waiting to crash the program
	    gui.menu_font = HL_جدول()[idx].sg_fontset;
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
	    gui.tooltip_fontset = (XFont_سیٹ)HL_جدول()[idx].sg_fontset;
	    gui_mch_new_tooltip_font();
	}
#   endif
#  endif
    }
    ورنہ
# endif
    {
	اگر (free_font)
	    gui_mch_free_font(HL_جدول()[idx].sg_font);
	HL_جدول()[idx].sg_font = font_name2handle(arg);
	// If it worked and it's the Normal group, use it as the
	// normal font.  Same for the Menu group.
	اگر (HL_جدول()[idx].sg_font != NOFONT)
	{
	    اگر (do_معمول)
		gui_init_font(arg, FALSE);
# ifndef FONTSET_ALWAYS
#  اگر defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	    اگر (do_menu)
	    {
		gui.menu_font = HL_جدول()[idx].sg_font;
		gui_mch_new_menu_font();
	    }
#  endif
# endif
	}
    }
}

#اگر_ختم // FEAT_GUI

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return the handle for a color name.
 * Returns INVALCOLOR when failed.
 */
    guicolor_T
color_name2handle(char_u *name)
{
    اگر (STRCMP(name, "NONE") == 0)
	واپس INVALCOLOR;

    اگر (STRICMP(name, "fg") == 0 || STRICMP(name, "foreground") == 0)
    {
# اگر defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	اگر (gui.اندر_use)
# endif
# ifdef FEAT_GUI
	    واپس gui.norm_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	اگر (cterm_معمول_fg_gui_color != INVALCOLOR)
	    واپس cterm_معمول_fg_gui_color;
	// Guess that the foreground is black or white.
	واپس GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "black" : "white"));
# endif
    }
    اگر (STRICMP(name, "bg") == 0 || STRICMP(name, "background") == 0)
    {
# اگر defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	اگر (gui.اندر_use)
# endif
# ifdef FEAT_GUI
	    واپس gui.عقب_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	اگر (cterm_معمول_bg_gui_color != INVALCOLOR)
	    واپس cterm_معمول_bg_gui_color;
	// Guess that the background is white or black.
	واپس GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "white" : "black"));
# endif
    }

    واپس GUI_GET_COLOR(name);
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
    ساکن guicolor_T
gui_adjust_rgb(guicolor_T c)
{
    اگر (gui.اندر_use)
	واپس c;
    ورنہ
	واپس ((c & 0xff) << 16) | (c & 0x00ff00) | ((c >> 16) & 0xff);
}
# ورنہ
#  define gui_adjust_rgb(c) (c)
# endif

    ساکن صحیح
hex_digit(صحیح c)
{
    اگر (محفوظ_isdigit(c))
	واپس c - '0';
    c = TOLOWER_ASC(c);
    اگر (c >= 'a' && c <= 'f')
	واپس c - 'a' + 10;
    واپس 0x1ffffff;
}

    ساکن guicolor_T
decode_hex_color(char_u *hex)
{
    guicolor_T color;

    اگر (hex[0] != '#' || STRLEN(hex) != 7)
	واپس INVALCOLOR;

    // Name is in "#rrggbb" format
    color = RGB(((hex_digit(hex[1]) << 4) + hex_digit(hex[2])),
		((hex_digit(hex[3]) << 4) + hex_digit(hex[4])),
		((hex_digit(hex[5]) << 4) + hex_digit(hex[6])));
    اگر (color > 0xffffff)
	واپس INVALCOLOR;
    واپس gui_adjust_rgb(color);
}

# ifdef FEAT_EVAL
// Returns the color currently mapped to the given name or INVALCOLOR if no
// such name exists in the color table. The convention is to use lowercase for
// all keys in the v:colornames dictionary. The value can be either a string in
// the form #rrggbb or a number, either of which is converted to a guicolor_T.
    ساکن guicolor_T
colorname2rgb(char_u *name)
{
    dict_T      *colornames_جدول = get_vim_var_dict(VV_COLORNAMES);
    char_u      *lc_name;
    dictitem_T  *colentry;
    char_u      *colstr;
    varnumber_T colnum;

    lc_name = strlow_save(name);
    اگر (lc_name == تہی)
	واپس INVALCOLOR;

    colentry = dict_ڈھونڈو(colornames_جدول, lc_name, -1);
    vim_free(lc_name);
    اگر (colentry == تہی)
	واپس INVALCOLOR;

    اگر (colentry->di_tv.v_type == VAR_STRING)
    {
	colstr = tv_get_string_strict(&colentry->di_tv);
	اگر ((STRLEN(colstr) == 7) && (*colstr == '#'))
	{
	    واپس decode_hex_color(colstr);
	}
	ورنہ
	{
	    semsg(_(e_bad_color_string_str), colstr);
	    واپس INVALCOLOR;
	}
    }

    اگر (colentry->di_tv.v_type == VAR_نمبر)
    {
	colnum = tv_get_نمبر(&colentry->di_tv);
	واپس (guicolor_T)colnum;
    }

    واپس INVALCOLOR;
}

# endif

    guicolor_T
gui_get_color_cmn(char_u *name)
{
    guicolor_T  color;
    // Only non X11 colors (not present in rgb.txt) and colors in
    // color_name_tab[], useful when $VIMRUNTIME is not found,.
    // must be sorted by the 'value' field because it is used by bsearch()!
    ساکن keyvalue_T rgb_tab[] = {
	KEYVALUE_اندراج(RGB(0x00, 0x00, 0x00), "black"),
	KEYVALUE_اندراج(RGB(0x00, 0x00, 0xFF), "blue"),
	KEYVALUE_اندراج(RGB(0xA5, 0x2A, 0x2A), "brown"),
	KEYVALUE_اندراج(RGB(0x00, 0xFF, 0xFF), "cyan"),
	KEYVALUE_اندراج(RGB(0x00, 0x00, 0x8B), "darkblue"),
	KEYVALUE_اندراج(RGB(0x00, 0x8B, 0x8B), "darkcyan"),
	KEYVALUE_اندراج(RGB(0xA9, 0xA9, 0xA9), "darkgray"),
	KEYVALUE_اندراج(RGB(0x00, 0x64, 0x00), "darkgreen"),
	KEYVALUE_اندراج(RGB(0xA9, 0xA9, 0xA9), "darkgrey"),
	KEYVALUE_اندراج(RGB(0x8B, 0x00, 0x8B), "darkmagenta"),
	KEYVALUE_اندراج(RGB(0x8B, 0x00, 0x00), "darkred"),
	KEYVALUE_اندراج(RGB(0x8B, 0x8B, 0x00), "darkyellow"),	 // No X11
	KEYVALUE_اندراج(RGB(0xBE, 0xBE, 0xBE), "gray"),
	KEYVALUE_اندراج(RGB(0x00, 0xFF, 0x00), "green"),
	KEYVALUE_اندراج(RGB(0xBE, 0xBE, 0xBE), "grey"),
	KEYVALUE_اندراج(RGB(0x66, 0x66, 0x66), "grey40"),
	KEYVALUE_اندراج(RGB(0x7F, 0x7F, 0x7F), "grey50"),
	KEYVALUE_اندراج(RGB(0xE5, 0xE5, 0xE5), "grey90"),
	KEYVALUE_اندراج(RGB(0xAD, 0xD8, 0xE6), "lightblue"),
	KEYVALUE_اندراج(RGB(0xE0, 0xFF, 0xFF), "lightcyan"),
	KEYVALUE_اندراج(RGB(0xD3, 0xD3, 0xD3), "lightgray"),
	KEYVALUE_اندراج(RGB(0x90, 0xEE, 0x90), "lightgreen"),
	KEYVALUE_اندراج(RGB(0xD3, 0xD3, 0xD3), "lightgrey"),
	KEYVALUE_اندراج(RGB(0xFF, 0x8B, 0xFF), "lightmagenta"),     // No XX
	KEYVALUE_اندراج(RGB(0xFF, 0x8B, 0x8B), "lightred"),	 // No XX
	KEYVALUE_اندراج(RGB(0xFF, 0xFF, 0xE0), "lightyellow"),
	KEYVALUE_اندراج(RGB(0xFF, 0x00, 0xFF), "magenta"),
	KEYVALUE_اندراج(RGB(0xFF, 0x00, 0x00), "red"),
	KEYVALUE_اندراج(RGB(0x2E, 0x8B, 0x57), "seagreen"),
	KEYVALUE_اندراج(RGB(0xFF, 0xFF, 0xFF), "white"),
	KEYVALUE_اندراج(RGB(0xFF, 0xFF, 0x00), "yellow")
    };
    keyvalue_T ہدف;
    keyvalue_T *اندراج_2;

    color = decode_hex_color(name);
    اگر (color != INVALCOLOR)
	واپس color;

    ہدف.کلید = 0;
    ہدف.قدر.string = name;
    ہدف.قدر.لمبائی = 0;	// not used, see cmp_keyvalue_value_i()
    اندراج_2 = (keyvalue_T *)bsearch(&ہدف, &rgb_tab, ARRAY_لمبائی(rgb_tab),
	    حجم(rgb_tab[0]), cmp_keyvalue_قدر_i);
    اگر (اندراج_2 != تہی)
	واپس gui_adjust_rgb((guicolor_T)اندراج_2->کلید);

# اگر defined(FEAT_EVAL)
    /*
     * Not a traditional color. Load additional color aliases and then consult the alias table.
     */

    color = colorname2rgb(name);
    اگر (color == INVALCOLOR)
    {
	بھرو_default_colors_lists();
	color = colorname2rgb(name);
    }

    واپس color;
# ورنہ
    واپس INVALCOLOR;
# endif
}

    guicolor_T
gui_get_rgb_color_cmn(صحیح r, صحیح g, صحیح b)
{
    guicolor_T  color = RGB(r, g, b);

    اگر (color > 0xffffff)
	واپس INVALCOLOR;
    واپس gui_adjust_rgb(color);
}
#اگر_ختم

/*
 * Table with the specifications for an attribute number.
 * Note that this table is used by ALL buffers.  This is required because the
 * GUI can redraw at any time for any buffer.
 */
ساکن garray_T	رکن_attr_جدول = {0, 0, 0, 0, تہی};

#تعریف_کرو رکن_ATTR_اندراج(idx) ((attrentry_T *)رکن_attr_جدول.ga_ڈیٹا)[idx]

ساکن garray_T	cterm_attr_جدول = {0, 0, 0, 0, تہی};

#تعریف_کرو CTERM_ATTR_اندراج(idx) ((attrentry_T *)cterm_attr_جدول.ga_ڈیٹا)[idx]

#اگر_تعریف_شدہ FEAT_GUI
ساکن garray_T	gui_attr_جدول = {0, 0, 0, 0, تہی};

# define GUI_ATTR_اندراج(idx) ((attrentry_T *)gui_attr_جدول.ga_ڈیٹا)[idx]
#اگر_ختم

/*
 * Return the attr number for a set of colors and font.
 * Add a new entry to the term_attr_table, cterm_attr_table or gui_attr_table
 * if the combination is new.
 * Return 0 for error (no more room).
 */
    ساکن صحیح
get_attr_اندراج(garray_T *جدول, attrentry_T *aep)
{
    صحیح		i;
    attrentry_T	*taep;
    ساکن صحیح	recursive = FALSE;

    // Init the table, in case it wasn't done yet.
    جدول->ga_itemsize = حجم(attrentry_T);
    جدول->ga_growsize = 7;

    // Try to find an entry with the same specifications.
    برائے (i = 0; i < جدول->ga_len; ++i)
    {
	taep = &(((attrentry_T *)جدول->ga_ڈیٹا)[i]);
	اگر (	   aep->ae_attr == taep->ae_attr
		&& (
#اگر_تعریف_شدہ FEAT_GUI
		       (جدول == &gui_attr_جدول
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
#اگر_ختم
		       (جدول == &رکن_attr_جدول
			&& (aep->ae_u.رکن.آغاز == تہی)
					    == (taep->ae_u.رکن.آغاز == تہی)
			&& (aep->ae_u.رکن.آغاز == تہی
			    || STRCMP(aep->ae_u.رکن.آغاز,
						  taep->ae_u.رکن.آغاز) == 0)
			&& (aep->ae_u.رکن.stop == تہی)
					     == (taep->ae_u.رکن.stop == تہی)
			&& (aep->ae_u.رکن.stop == تہی
			    || STRCMP(aep->ae_u.رکن.stop,
						  taep->ae_u.رکن.stop) == 0))
		    || (جدول == &cterm_attr_جدول
			    && aep->ae_u.cterm.fg_color
						  == taep->ae_u.cterm.fg_color
			    && aep->ae_u.cterm.bg_color
						  == taep->ae_u.cterm.bg_color
			    && aep->ae_u.cterm.ul_color
						  == taep->ae_u.cterm.ul_color
			    && aep->ae_u.cterm.font
						  == taep->ae_u.cterm.font
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
			    && aep->ae_u.cterm.fg_rgb
						    == taep->ae_u.cterm.fg_rgb
			    && aep->ae_u.cterm.bg_rgb
						    == taep->ae_u.cterm.bg_rgb
			    && aep->ae_u.cterm.ul_rgb
						    == taep->ae_u.cterm.ul_rgb
#اگر_ختم
		       )))

	واپس i + ATTR_OFF;
    }

    اگر (جدول->ga_len + ATTR_OFF > MAX_TYPENR)
    {
	// Running out of attribute entries!  remove all attributes, and
	// compute new ones for all groups.
	// When called recursively, we are really out of numbers.
	اگر (recursive)
	{
	    emsg(_(e_too_many_different_highlighting_اوصاف_اندر_use));
	    واپس 0;
	}
	recursive = TRUE;

	clear_hl_tables();

	سیٹ_must_redraw(UPD_CLEAR);

	برائے (i = 0; i < highlight_ga.ga_len; ++i)
	    سیٹ_hl_attr(i);

	recursive = FALSE;
    }

    // This is a new combination of colors and font, add an entry.
    اگر (ga_grow(جدول, 1) == FAIL)
	واپس 0;

    taep = &(((attrentry_T *)جدول->ga_ڈیٹا)[جدول->ga_len]);
    CLEAR_اشارہ_گر(taep);
    taep->ae_attr = aep->ae_attr;
#اگر_تعریف_شدہ FEAT_GUI
    اگر (جدول == &gui_attr_جدول)
    {
	taep->ae_u.gui.fg_color = aep->ae_u.gui.fg_color;
	taep->ae_u.gui.bg_color = aep->ae_u.gui.bg_color;
	taep->ae_u.gui.sp_color = aep->ae_u.gui.sp_color;
	taep->ae_u.gui.font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
	taep->ae_u.gui.fontset = aep->ae_u.gui.fontset;
# endif
    }
#اگر_ختم
    اگر (جدول == &رکن_attr_جدول)
    {
	اگر (aep->ae_u.رکن.آغاز == تہی)
	    taep->ae_u.رکن.آغاز = تہی;
	ورنہ
	    taep->ae_u.رکن.آغاز = vim_strsave(aep->ae_u.رکن.آغاز);
	اگر (aep->ae_u.رکن.stop == تہی)
	    taep->ae_u.رکن.stop = تہی;
	ورنہ
	    taep->ae_u.رکن.stop = vim_strsave(aep->ae_u.رکن.stop);
    }
    ورنہ اگر (جدول == &cterm_attr_جدول)
    {
	taep->ae_u.cterm.fg_color = aep->ae_u.cterm.fg_color;
	taep->ae_u.cterm.bg_color = aep->ae_u.cterm.bg_color;
	taep->ae_u.cterm.ul_color = aep->ae_u.cterm.ul_color;
	taep->ae_u.cterm.font = aep->ae_u.cterm.font;
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
	taep->ae_u.cterm.fg_rgb = aep->ae_u.cterm.fg_rgb;
	taep->ae_u.cterm.bg_rgb = aep->ae_u.cterm.bg_rgb;
	taep->ae_u.cterm.ul_rgb = aep->ae_u.cterm.ul_rgb;
#اگر_ختم
    }
    ++جدول->ga_len;
    واپس (جدول->ga_len - 1 + ATTR_OFF);
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

    CLEAR_میدان(at_en);
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
    واپس get_attr_اندراج(&cterm_attr_جدول, &at_en);
}
#اگر_ختم

#اگر defined(FEAT_TERMINAL) && defined(FEAT_TERMGUICOLORS)
/*
 * Get an attribute index for a 'termguicolors' entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    صحیح
get_tgc_attr_idx(صحیح attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_میدان(at_en);
    at_en.ae_attr = attr;
    اگر (fg == INVALCOLOR && bg == INVALCOLOR)
    {
	// If both GUI colors are not set fall back to the cterm colors.  Helps
	// if the GUI only has an attribute, such as undercurl.
	at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
    }
    ورنہ
    {
	at_en.ae_u.cterm.fg_rgb = fg;
	at_en.ae_u.cterm.bg_rgb = bg;
    }
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
    واپس get_attr_اندراج(&cterm_attr_جدول, &at_en);
}
#اگر_ختم

#اگر defined(FEAT_TERMINAL) && defined(FEAT_GUI)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    صحیح
get_gui_attr_idx(صحیح attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_میدان(at_en);
    at_en.ae_attr = attr;
    at_en.ae_u.gui.fg_color = fg;
    at_en.ae_u.gui.bg_color = bg;
    واپس get_attr_اندراج(&gui_attr_جدول, &at_en);
}
#اگر_ختم

/*
 * Clear all highlight tables.
 */
    خالی
clear_hl_tables(خالی)
{
    صحیح		i;
    attrentry_T	*taep;

#اگر_تعریف_شدہ FEAT_GUI
    ga_clear(&gui_attr_جدول);
#اگر_ختم
    برائے (i = 0; i < رکن_attr_جدول.ga_len; ++i)
    {
	taep = &(((attrentry_T *)رکن_attr_جدول.ga_ڈیٹا)[i]);
	vim_free(taep->ae_u.رکن.آغاز);
	vim_free(taep->ae_u.رکن.stop);
    }
    ga_clear(&رکن_attr_جدول);
    ga_clear(&cterm_attr_جدول);
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
    attrentry_T *char_aep = تہی;
    attrentry_T *prim_aep;
    attrentry_T new_en;

    اگر (char_attr == 0)
	واپس prim_attr;
    اگر (char_attr <= HL_ALL && prim_attr <= HL_ALL)
	واپس ATTR_COMBINE(char_attr, prim_attr);
#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
    {
	اگر (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	اگر (char_aep != تہی)
	    new_en = *char_aep;
	ورنہ
	{
	    CLEAR_میدان(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    اگر (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	اگر (prim_attr <= HL_ALL)
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	ورنہ
	{
	    prim_aep = syn_gui_attr2entry(prim_attr);
	    اگر (prim_aep != تہی)
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
	واپس get_attr_اندراج(&gui_attr_جدول, &new_en);
    }
#اگر_ختم

    اگر (IS_CTERM)
    {
	اگر (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	اگر (char_aep != تہی)
	    new_en = *char_aep;
	ورنہ
	{
	    CLEAR_میدان(new_en);
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#اگر_ختم
	    اگر (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	اگر (prim_attr <= HL_ALL)
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	ورنہ
	{
	    prim_aep = syn_cterm_attr2entry(prim_attr);
	    اگر (prim_aep != تہی)
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
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
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
		ورنہ
		{
		    اگر (prim_aep->ae_u.cterm.fg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.fg_rgb = prim_aep->ae_u.cterm.fg_rgb;
		    اگر (prim_aep->ae_u.cterm.bg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.bg_rgb = prim_aep->ae_u.cterm.bg_rgb;
		}
		اگر (prim_aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		    new_en.ae_u.cterm.ul_rgb = prim_aep->ae_u.cterm.ul_rgb;
#اگر_ختم
	    }
	}
	واپس get_attr_اندراج(&cterm_attr_جدول, &new_en);
    }

    اگر (char_attr > HL_ALL)
	char_aep = syn_رکن_attr2entry(char_attr);
    اگر (char_aep != تہی)
	new_en = *char_aep;
    ورنہ
    {
	CLEAR_میدان(new_en);
	اگر (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    اگر (prim_attr <= HL_ALL)
	new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
    ورنہ
    {
	prim_aep = syn_رکن_attr2entry(prim_attr);
	اگر (prim_aep != تہی)
	{
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_aep->ae_attr);
	    اگر (prim_aep->ae_u.رکن.آغاز != تہی)
	    {
		new_en.ae_u.رکن.آغاز = prim_aep->ae_u.رکن.آغاز;
		new_en.ae_u.رکن.stop = prim_aep->ae_u.رکن.stop;
	    }
	}
    }
    واپس get_attr_اندراج(&رکن_attr_جدول, &new_en);
}

// ANSI color order: Black, Red, Green, Yellow, Blue, Magenta,
// Cyan, White, then bright variants.  Approximate RGB values used when
// only a cterm color number is known (no guifg/guibg).  Real terminal
// palettes may differ if the user reconfigured their emulator, but
// these are reasonable xterm-ish defaults.
ساکن ثابت long_u cterm_color_16[16] = {
    0x000000, 0xc00000, 0x008000, 0x808000,
    0x0000c0, 0xc000c0, 0x004080, 0xc0c0c0,
    0x808080, 0xff8080, 0x00ff00, 0xffff00,
    0x6060ff, 0xff40ff, 0x00ffff, 0xffffff
};

#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
/*
 * Convert a cterm color number (1-16) to an RGB value.
 * Used as a fallback when 'termguicolors' is set but only cterm colors are
 * specified (no guifg/guibg).
 * Returns INVALCOLOR if the color number is out of range.
 */
    ساکن guicolor_T
cterm_color_مقصد_rgb(صحیح color_nr)
{
    اگر (color_nr < 1 || color_nr > 16)
	واپس INVALCOLOR;
    واپس (guicolor_T)cterm_color_16[color_nr - 1];
}
#اگر_ختم

/*
 * Convert an xterm 256-color index (0-255) to an approximate RGB triple.
 * Uses the standard xterm palette: 0-15 ANSI (cterm_color_16), 16-231
 * 6x6x6 cube, 232-255 grayscale ramp.
 */
    ساکن خالی
cterm_idx_مقصد_rgb(صحیح idx, صحیح *r, صحیح *g, صحیح *b)
{
    ساکن ثابت صحیح cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

    اگر (idx < 0 || idx > 255)
    {
	*r = *g = *b = 0;
	واپس;
    }
    اگر (idx < 16)
    {
	long_u rgb = cterm_color_16[idx];
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
    }
    ورنہ اگر (idx < 232)
    {
	صحیح n = idx - 16;
	*r = cube[n / 36 % 6];
	*g = cube[n / 6  % 6];
	*b = cube[n      % 6];
    }
    ورنہ
    {
	صحیح v = 8 + (idx - 232) * 10;
	*r = *g = *b = v;
    }
}

/*
 * Approximate an RGB triple to the nearest xterm 256-color index.
 * Searches the 6x6x6 cube and the grayscale ramp; returns 0-255.
 */
    ساکن صحیح
rgb_مقصد_cterm_idx(صحیح r, صحیح g, صحیح b)
{
    ساکن ثابت صحیح cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };
    صحیح بہترین = 0;
    طویل بہترین_d = -1;
    صحیح i;

    // Search the 6x6x6 cube.
    برائے (i = 16; i < 232; ++i)
    {
	صحیح n = i - 16;
	صحیح cr = cube[n / 36 % 6];
	صحیح cg = cube[n / 6  % 6];
	صحیح cb = cube[n      % 6];
	طویل d = (cr - r) * (cr - r) + (cg - g) * (cg - g) + (cb - b) * (cb - b);
	اگر (بہترین_d < 0 || d < بہترین_d)
	{
	    بہترین_d = d;
	    بہترین = i;
	}
    }
    // Search the grayscale ramp.
    برائے (i = 232; i < 256; ++i)
    {
	صحیح v = 8 + (i - 232) * 10;
	طویل d = (v - r) * (v - r) + (v - g) * (v - g) + (v - b) * (v - b);
	اگر (d < بہترین_d)
	{
	    بہترین_d = d;
	    بہترین = i;
	}
    }
    واپس بہترین;
}

/*
 * Resolve a color to an RGB triple.  Prefer the gui RGB value (set by
 * guifg/guibg) when valid, otherwise convert the cterm 256-color index.
 * Returns true on success; false when no color is defined.
 * cterm_c is 1-based (0 means "no color").
 */
    ساکن bool
resolve_color_مقصد_rgb(صحیح cterm_c, guicolor_T rgb غیر_مستعمل, صحیح *r, صحیح *g, صحیح *b)
{
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
    اگر (!COLOR_INVALID(rgb))
    {
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
	واپس true;
    }
#اگر_ختم
    اگر (cterm_c > 0)
    {
	cterm_idx_مقصد_rgb(cterm_c - 1, r, g, b);
	واپس true;
    }
    واپس false;
}

/*
 * get a RGB fallback color from gui, cterm or default color
 */
    ساکن guicolor_T
resolve_fallback_color(صحیح cterm_c, guicolor_T rgb, guicolor_T default_rgb)
{
    صحیح red, green, blue;
    اگر (!resolve_color_مقصد_rgb(cterm_c, rgb, &red, &green, &blue))
	واپس default_rgb;
    ورنہ
	واپس (red << 16) | (green << 8) | blue;
}

/*
 * get a RGB fallback foreground color from guifg, ctermfg or deduced from background
 */
    ساکن خالی
resolve_fallback_fg_مقصد_rgb(خالی)
{
    guicolor_T fgcolor_or_gui_fgcolor = INVALCOLOR;
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
    fgcolor_or_gui_fgcolor = cterm_معمول_fg_gui_color;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
	fgcolor_or_gui_fgcolor = gui.norm_pixel;
#اگر_ختم
    fallback_fg_rgb = resolve_fallback_color(cterm_معمول_fg_color, fgcolor_or_gui_fgcolor, (*p_bg == 'l') ? 0x000000 : 0xFFFFFF);
}

/*
 * get a RGB fallback background color from guifg, ctermbg or deduced from background
 */
    ساکن خالی
resolve_fallback_bg_مقصد_rgb(خالی)
{
    guicolor_T bgcolor_or_gui_bgcolor = INVALCOLOR;
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
    bgcolor_or_gui_bgcolor = cterm_معمول_bg_gui_color;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
	bgcolor_or_gui_bgcolor = gui.عقب_pixel;
#اگر_ختم
    fallback_bg_rgb = resolve_fallback_color(cterm_معمول_bg_color, bgcolor_or_gui_bgcolor, (*p_bg == 'l') ? 0xFFFFFF : 0x000000);
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
    ساکن صحیح
blend_cterm_colors(صحیح popup_c, guicolor_T popup_rgb,
		   صحیح under_c, guicolor_T under_rgb,
		   صحیح default_rgb,
		   صحیح blend_val)
{
    صحیح pr, pg, pb, ur, ug, ub, r, g, b;

    اگر (t_colors < 256)
	واپس popup_c;
    اگر (!resolve_color_مقصد_rgb(popup_c, popup_rgb, &pr, &pg, &pb))
	واپس under_c;
    اگر (blend_val <= 0)
	واپس rgb_مقصد_cterm_idx(pr, pg, pb) + 1;
    اگر (!resolve_color_مقصد_rgb(under_c, under_rgb, &ur, &ug, &ub))
    {
	ur = (default_rgb >> 16) & 0xFF;
	ug = (default_rgb >> 8) & 0xFF;
	ub = default_rgb & 0xFF;
    }
    اگر (blend_val >= 100)
	واپس rgb_مقصد_cterm_idx(ur, ug, ub) + 1;
    r = pr + (ur - pr) * blend_val / 100;
    g = pg + (ug - pg) * blend_val / 100;
    b = pb + (ub - pb) * blend_val / 100;
    واپس rgb_مقصد_cterm_idx(r, g, b) + 1;
}

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Blend two RGB colors based on blend value (0-100).
 * blend: 0=return first argument color, 100=return second argument color
 * If bg_color is INVALCOLOR, high blend means more visible (return INVALCOLOR).
 */
    ساکن guicolor_T
blend_colors(guicolor_T popup_color, guicolor_T bg_color, صحیح blend_val)
{
    صحیح r1, g1, b1, r2, g2, b2, r, g, b;

    // CTERMCOLOR is a sentinel meaning "use the cterm color"; for blending
    // it has no real RGB so treat it like INVALCOLOR.
    اگر (COLOR_INVALID(popup_color))
	واپس INVALCOLOR;

    // Fully transparent: use underlying color as-is.
    اگر (blend_val >= 100)
	واپس bg_color;

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

    واپس (r << 16) | (g << 8) | b;
}
#اگر_ختم

/*
 * Blend attributes for popup windows and menus with opacity.
 * Blends foreground and/or background colors based on blend value (0-100).
 * blend: 0 = opaque (use popup colors), 100 = transparent (use background colors)
 * blend_fg: TRUE to blend foreground color, FALSE to keep popup foreground
 */
    ساکن صحیح
hl_blend_attr_common(
    صحیح		char_attr,
    صحیح		popup_attr,
    صحیح		blend,
    صحیح		blend_fg)
{
    attrentry_T *char_aep = تہی;
    attrentry_T *popup_aep;
    attrentry_T new_en;
    attrentry_T tmp_en;

#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
    {
	اگر (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	اگر (char_aep != تہی)
	    new_en = *char_aep;
	ورنہ
	{
	    CLEAR_میدان(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    اگر (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	اگر (popup_attr <= HL_ALL)
	{
	    CLEAR_میدان(tmp_en);
	    tmp_en.ae_u.gui.fg_color = INVALCOLOR;
	    tmp_en.ae_u.gui.bg_color = INVALCOLOR;
	    tmp_en.ae_u.gui.sp_color = INVALCOLOR;
	    // preserve attributes other than color
	    tmp_en.ae_attr = popup_attr;
	    popup_aep = &tmp_en;

	    popup_aep->ae_u.gui.bg_color = fallback_bg_rgb;
	}
	ورنہ
	    popup_aep = syn_gui_attr2entry(popup_attr);

	اگر (popup_aep != تہی)
	{
	    guicolor_T popup_bg_rgb = popup_aep->ae_u.gui.bg_color;
	    اگر (COLOR_INVALID(popup_bg_rgb))
		popup_bg_rgb = fallback_bg_rgb;

	    اگر (blend_fg)
	    {
		// blend_fg=TRUE: fade underlying text toward popup bg.
		{
		    صحیح بنیاد_fg = fallback_fg_rgb;
		    اگر (char_aep != تہی
			    && char_aep->ae_u.gui.fg_color != INVALCOLOR)
			بنیاد_fg = char_aep->ae_u.gui.fg_color;
		    new_en.ae_u.gui.fg_color = blend_colors(
			    popup_bg_rgb, بنیاد_fg, blend);
		}
	    }
	    ورنہ
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
		اگر (char_aep != تہی
			&& !COLOR_INVALID(char_aep->ae_u.gui.bg_color))
		    underlying_bg = char_aep->ae_u.gui.bg_color;
		new_en.ae_u.gui.bg_color = blend_colors(
			popup_bg_rgb,
			underlying_bg, blend);
	    }
	}
	واپس get_attr_اندراج(&gui_attr_جدول, &new_en);
    }
#اگر_ختم

    اگر (IS_CTERM)
    {
	اگر (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	اگر (char_aep != تہی)
	    new_en = *char_aep;
	ورنہ
	{
	    CLEAR_میدان(new_en);
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#اگر_ختم
	    اگر (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	اگر (popup_attr <= HL_ALL)
	{
	    CLEAR_میدان(tmp_en);
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
	    tmp_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    tmp_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	    // allow blending with termguicolors
	    tmp_en.ae_u.cterm.bg_rgb = fallback_bg_rgb;
#اگر_ختم
	    // preserve attributes other than color
	    tmp_en.ae_attr = popup_attr;
	    popup_aep = &tmp_en;

	    // allow blending with notermguicolors
	    popup_aep->ae_u.cterm.bg_color = cterm_معمول_bg_color;
	}
	ورنہ
	    popup_aep = syn_cterm_attr2entry(popup_attr);

	اگر (popup_aep != تہی)
	{
	    guicolor_T popup_bg_rgb = INVALCOLOR;
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
	    // Fall back to cterm color converted to RGB when gui color is not set.
	    popup_bg_rgb = popup_aep->ae_u.cterm.bg_rgb;
	    اگر (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color > 0)
		popup_bg_rgb = cterm_color_مقصد_rgb(
			popup_aep->ae_u.cterm.bg_color);
#اگر_ختم
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
		ورنہ اگر (cterm_معمول_fg_color > 0)
		    new_en.ae_u.cterm.fg_color = cterm_معمول_fg_color;
		ورنہ
		    // black-ish or white-ish
		    new_en.ae_u.cterm.fg_color = (*p_bg == 'l') ? 1 : 16;
		new_en.ae_u.cterm.ul_color = popup_aep->ae_u.cterm.ul_color;
		new_en.ae_u.cterm.font = popup_aep->ae_u.cterm.font;
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
		new_en.ae_u.cterm.ul_rgb = popup_aep->ae_u.cterm.ul_rgb;
#اگر_ختم
	    }
	    ورنہ
	    {
		// blend_fg=TRUE: fade underlying fg toward popup bg in
		// the 256-color palette.  Used when the popup is over a
		// cell rendered with cterm colors (no termguicolors RGB).
		صحیح under_fg = (char_aep != تہی)
		    ? char_aep->ae_u.cterm.fg_color : 0;
		guicolor_T under_fg_rgb = INVALCOLOR;
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
		اگر (char_aep != تہی)
		    under_fg_rgb = char_aep->ae_u.cterm.fg_rgb;
#اگر_ختم
		new_en.ae_u.cterm.fg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_fg, under_fg_rgb, fallback_fg_rgb, blend);
	    }
	    // Approximate cterm bg by blending with the underlying bg
	    // in the 256-color palette and mapping to the nearest entry.
	    {
		صحیح under_bg = (char_aep != تہی)
		    ? char_aep->ae_u.cterm.bg_color : 0;
		guicolor_T under_bg_rgb = INVALCOLOR;
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
		اگر (char_aep != تہی)
		    under_bg_rgb = char_aep->ae_u.cterm.bg_rgb;
#اگر_ختم
		new_en.ae_u.cterm.bg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_bg, under_bg_rgb, fallback_bg_rgb, blend);
	    }
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
	    // Blend RGB colors for termguicolors mode.
	    {
		اگر (blend_fg)
		{
		    // blend_fg=TRUE: fade underlying text toward popup bg.
		    اگر (popup_bg_rgb != INVALCOLOR)
		    {
			صحیح بنیاد_fg = fallback_fg_rgb;
			// CTERMCOLOR is a sentinel meaning "use the cterm
			// color"; treat it as no underlying color so it is
			// not blended in as a real near-white pixel.
			اگر (char_aep != تہی
				&& !COLOR_INVALID(char_aep->ae_u.cterm.fg_rgb))
			    بنیاد_fg = char_aep->ae_u.cterm.fg_rgb;
			new_en.ae_u.cterm.fg_rgb = blend_colors(
				popup_bg_rgb, بنیاد_fg, blend);
		    }
		}
		ورنہ
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
			popup_fg = cterm_color_مقصد_rgb(
				popup_aep->ae_u.cterm.fg_color);

		    اگر (!COLOR_INVALID(popup_fg))
			new_en.ae_u.cterm.fg_rgb = popup_fg;
		    ورنہ اگر (!COLOR_INVALID(cterm_معمول_fg_gui_color))
			new_en.ae_u.cterm.fg_rgb = cterm_معمول_fg_gui_color;
		    ورنہ
			new_en.ae_u.cterm.fg_rgb = fallback_fg_rgb;
		}
		اگر (popup_bg_rgb != INVALCOLOR)
		{
		    // Blend popup bg toward underlying bg
		    guicolor_T underlying_bg = fallback_bg_rgb;
		    اگر (char_aep != تہی
			    && !COLOR_INVALID(char_aep->ae_u.cterm.bg_rgb))
			underlying_bg = char_aep->ae_u.cterm.bg_rgb;
		    new_en.ae_u.cterm.bg_rgb = blend_colors(
			    popup_bg_rgb, underlying_bg, blend);
		}
	    }
#اگر_ختم
	}
	واپس get_attr_اندراج(&cterm_attr_جدول, &new_en);
    }

    اگر (char_attr > HL_ALL)
	char_aep = syn_رکن_attr2entry(char_attr);
    اگر (char_aep != تہی)
	new_en = *char_aep;
    ورنہ
    {
	CLEAR_میدان(new_en);
	اگر (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    // For term mode, no separate background color handling.
    واپس get_attr_اندراج(&رکن_attr_جدول, &new_en);
}

/*
 * Blend attributes for popup windows with opacity.
 */
    صحیح
hl_blend_attr(صحیح char_attr, صحیح popup_attr, صحیح blend, صحیح blend_fg)
{
    اگر (blend >= 100 && blend_fg)
	واپس char_attr;

    واپس hl_blend_attr_common(char_attr, popup_attr, blend, blend_fg);
}

/*
 * Blend underlying foreground and background colors for pum space cells.
 */
    صحیح
hl_pum_blend_attr(صحیح char_attr, صحیح popup_attr, صحیح blend)
{
    واپس hl_blend_attr_common(char_attr, popup_attr, blend, TRUE);
}

#اگر_تعریف_شدہ FEAT_GUI
    attrentry_T *
syn_gui_attr2entry(صحیح attr)
{
    attr -= ATTR_OFF;
    اگر (attr >= gui_attr_جدول.ga_len)	    // did ":syntax clear"
	واپس تہی;
    واپس &(GUI_ATTR_اندراج(attr));
}
#اگر_ختم

/*
 * Get the highlight attributes (HL_BOLD etc.) from an attribute nr.
 * Only to be used when "attr" > HL_ALL.
 */
    صحیح
syn_attr2attr(صحیح attr)
{
    attrentry_T	*aep;

#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
	aep = syn_gui_attr2entry(attr);
    ورنہ
#اگر_ختم
	اگر (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	ورنہ
	    aep = syn_رکن_attr2entry(attr);

    اگر (aep == تہی)	    // highlighting not set
	واپس 0;
    واپس aep->ae_attr;
}


    attrentry_T *
syn_رکن_attr2entry(صحیح attr)
{
    attr -= ATTR_OFF;
    اگر (attr >= رکن_attr_جدول.ga_len)	    // did ":syntax clear"
	واپس تہی;
    واپس &(رکن_ATTR_اندراج(attr));
}

    attrentry_T *
syn_cterm_attr2entry(صحیح attr)
{
    attr -= ATTR_OFF;
    اگر (attr >= cterm_attr_جدول.ga_len)	// did ":syntax clear"
	واپس تہی;
    واپس &(CTERM_ATTR_اندراج(attr));
}

#تعریف_کرو LIST_ATTR   1
#تعریف_کرو LIST_STRING 2
#تعریف_کرو LIST_INT    3

    ساکن خالی
highlight_list_one(صحیح id)
{
    hl_گروہ_T	    *sgp;
    صحیح		    didh = FALSE;

    sgp = &HL_جدول()[id - 1];	    // index is ID minus one

    اگر (پیغام_filtered(sgp->sg_name))
	واپس;

    // Note: Keep this in sync with expand_highlight_group().
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_رکن, تہی, "term");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_آغاز, "start");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_stop, "stop");

    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_cterm, تہی, "cterm");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_fg, تہی, "ctermfg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_bg, تہی, "ctermbg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_ul, تہی, "ctermul");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_font, تہی, "ctermfont");

#اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_gui, تہی, "gui");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_fg_name, "guifg");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_bg_name, "guibg");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_sp_name, "guisp");
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_font_name, "font");
#اگر_ختم

    اگر (sgp->sg_link && !got_int)
    {
	(خالی)syn_list_سرخی(didh, 9999, id);
	didh = TRUE;
	پیغام_puts_attr("links to", HL_ATTR(HLF_D));
	پیغام_putchar(' ');
	پیغام_outtrans(HL_جدول()[HL_جدول()[id - 1].sg_link - 1].sg_name);
    }

    اگر (!didh)
	highlight_list_arg(id, didh, LIST_STRING, 0, (char_u *)"cleared", "");
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (p_verbose > 0)
	آخری_سیٹ_پیغام(sgp->sg_script_ctx);
#اگر_ختم
}

    ساکن char_u*
highlight_arg_مقصد_string(
    صحیح		type,
    صحیح		iarg,
    char_u	*sarg,
    char_u	*buf)
{
    اگر (type == LIST_INT)
	sprintf((حرف *)buf, "%d", iarg - 1);
    ورنہ اگر (type == LIST_STRING)
	واپس sarg;
    ورنہ // type == LIST_ATTR
    {
	size_t buflen;

	buf[0] = NUL;
	buflen = 0;
	برائے (صحیح i = 0; i < (صحیح)ARRAY_لمبائی(highlight_اشاریہ_tab); ++i)
	{
	    اگر (iarg & highlight_اشاریہ_tab[i]->کلید)
	    {
		اگر (buflen > 0)
		{
		    STRCPY(buf + buflen, (char_u *)",");
		    ++buflen;
		}
		STRCPY(buf + buflen, highlight_اشاریہ_tab[i]->قدر.string);
		buflen += highlight_اشاریہ_tab[i]->قدر.لمبائی;
		iarg &= ~highlight_اشاریہ_tab[i]->کلید;	    // don't want "inverse"/"reverse"
	    }
	}
    }
    واپس buf;
}

    ساکن صحیح
highlight_list_arg(
    صحیح		id,
    صحیح		didh,
    صحیح		type,
    صحیح		iarg,
    char_u	*sarg,
    حرف	*name)
{
    char_u	buf[MAX_ATTR_LEN];
    char_u	*ts;

    اگر (got_int)
	واپس FALSE;

    اگر (type == LIST_STRING ? (sarg == تہی) : (iarg == 0))
	واپس didh;

    ts = highlight_arg_مقصد_string(type, iarg, sarg, buf);

    (خالی)syn_list_سرخی(didh,
	    (صحیح)(vim_strsize(ts) + STRLEN(name) + 1), id);
    didh = TRUE;
    اگر (!got_int)
    {
	اگر (*name != NUL)
	{
	    پیغام_puts_attr(name, HL_ATTR(HLF_D));
	    پیغام_puts_attr("=", HL_ATTR(HLF_D));
	}
	پیغام_outtrans(ts);
    }
    واپس didh;
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
	واپس تہی;

# اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
    اگر (modec == 'g')
	attr = HL_جدول()[id - 1].sg_gui;
    ورنہ
# endif
    {
	اگر (modec == 'c')
	    attr = HL_جدول()[id - 1].sg_cterm;
	ورنہ
	    attr = HL_جدول()[id - 1].sg_رکن;
    }

    اگر (attr & flag)
	واپس (char_u *)"1";
    واپس تہی;
}
#اگر_ختم

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
    ساکن char_u	name[20];
    صحیح			n;
    صحیح			fg = FALSE;
    صحیح			sp = FALSE;
    صحیح			ul = FALSE;
    صحیح			font = FALSE;

    اگر (id <= 0 || id > highlight_ga.ga_len)
	واپس تہی;

    اگر (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'g')
	fg = TRUE;
    ورنہ اگر (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'o'
	     && TOLOWER_ASC(what[2]) == 'n' && TOLOWER_ASC(what[3]) == 't')
	font = TRUE;
    ورنہ اگر (TOLOWER_ASC(what[0]) == 's' && TOLOWER_ASC(what[1]) == 'p')
	sp = TRUE;
    ورنہ اگر (TOLOWER_ASC(what[0]) == 'u' && TOLOWER_ASC(what[1]) == 'l')
	ul = TRUE;
    ورنہ اگر (!(TOLOWER_ASC(what[0]) == 'b' && TOLOWER_ASC(what[1]) == 'g'))
	واپس تہی;
    اگر (modec == 'g')
    {
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI
	// return font name
	اگر (font)
	    واپس HL_جدول()[id - 1].sg_font_name;
#  endif

	// return #RRGGBB form (only possible when GUI is running)
	اگر ((USE_24_ثنائی_ہندسہ) && what[2] == '#')
	{
	    guicolor_T		color;
	    long_u		rgb;
	    ساکن char_u	buf[10];

	    اگر (fg)
		color = HL_جدول()[id - 1].sg_gui_fg;
	    ورنہ اگر (sp)
		color = HL_جدول()[id - 1].sg_gui_sp;
	    ورنہ
		color = HL_جدول()[id - 1].sg_gui_bg;
	    اگر (color == INVALCOLOR)
		واپس تہی;
	    rgb = (long_u)GUI_MCH_GET_RGB(color);
	    sprintf((حرف *)buf, "#%02x%02x%02x",
				      (غیرعلامتی)(rgb >> 16),
				      (غیرعلامتی)(rgb >> 8) & 255,
				      (غیرعلامتی)rgb & 255);
	    واپس buf;
	}
# endif
	اگر (fg)
	    واپس (HL_جدول()[id - 1].sg_gui_fg_name);
	اگر (sp)
	    واپس (HL_جدول()[id - 1].sg_gui_sp_name);
	واپس (HL_جدول()[id - 1].sg_gui_bg_name);
    }
    اگر (sp)
	واپس تہی;
    اگر (modec == 'c')
    {
	اگر (fg)
	    n = HL_جدول()[id - 1].sg_cterm_fg - 1;
	ورنہ اگر (ul)
	    n = HL_جدول()[id - 1].sg_cterm_ul - 1;
	ورنہ اگر (font)
	    n = HL_جدول()[id - 1].sg_cterm_font - 1;
	ورنہ
	    n = HL_جدول()[id - 1].sg_cterm_bg - 1;
	اگر (n < 0)
	    واپس تہی;
	sprintf((حرف *)name, "%d", n);
	واپس name;
    }
    // term doesn't have color
    واپس تہی;
}
#اگر_ختم

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
	واپس 0L;

    اگر (fg)
	color = HL_جدول()[id - 1].sg_gui_fg;
    ورنہ
	color = HL_جدول()[id - 1].sg_gui_bg;

    اگر (color == INVALCOLOR)
	واپس 0L;

    واپس GUI_MCH_GET_RGB(color);
}
#اگر_ختم

/*
 * Output the syntax list header.
 * Return TRUE when started a new line.
 */
    صحیح
syn_list_سرخی(
    صحیح	    did_سرخی,		// did header already
    صحیح	    outlen,		// length of string that comes
    صحیح	    id)			// highlight group id
{
    صحیح	    endcol = 19;
    صحیح	    newline = TRUE;
    صحیح	    name_ستون = 0;

    اگر (!did_سرخی)
    {
	پیغام_putchar('\n');
	اگر (got_int)
	    واپس TRUE;
	پیغام_outtrans(HL_جدول()[id - 1].sg_name);
	name_ستون = پیغام_ستون;
	endcol = 15;
    }
    ورنہ اگر (پیغام_ستون + outlen + 1 >= cmdline_چوڑائی)
    {
	پیغام_putchar('\n');
	اگر (got_int)
	    واپس TRUE;
    }
    ورنہ
    {
	اگر (پیغام_ستون >= endcol)	// wrap around is like starting a new line
	    newline = FALSE;
    }

    اگر (پیغام_ستون >= endcol)	// output at least one space
	endcol = پیغام_ستون + 1;
    اگر (cmdline_چوڑائی <= endcol)	// avoid hang for tiny window
	endcol = cmdline_چوڑائی - 1;

    پیغام_advance(endcol);

    // Show "xxx" with the attributes.
    اگر (!did_سرخی)
    {
	اگر (endcol == cmdline_چوڑائی - 1 && endcol <= name_ستون)
	    پیغام_putchar(' ');
	پیغام_puts_attr("xxx", syn_id2attr(id));
	پیغام_putchar(' ');
    }

    واپس newline;
}

/*
 * Set the attribute numbers for a highlight group.
 * Called after one of the attributes has changed.
 */
    ساکن خالی
سیٹ_hl_attr(
    صحیح		idx)	    // index in array
{
    attrentry_T	    at_en;
    hl_گروہ_T	    *sgp = HL_جدول() + idx;

    // The "Normal" group doesn't need an attribute number
    اگر (sgp->sg_name_u != تہی && STRCMP(sgp->sg_name_u, "NORMAL") == 0)
	واپس;

#اگر_تعریف_شدہ FEAT_GUI
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
    ورنہ
    {
	at_en.ae_attr = sgp->sg_gui;
	at_en.ae_u.gui.fg_color = sgp->sg_gui_fg;
	at_en.ae_u.gui.bg_color = sgp->sg_gui_bg;
	at_en.ae_u.gui.sp_color = sgp->sg_gui_sp;
	at_en.ae_u.gui.font = sgp->sg_font;
# ifdef FEAT_XFONTSET
	at_en.ae_u.gui.fontset = sgp->sg_fontset;
# endif
	sgp->sg_gui_attr = get_attr_اندراج(&gui_attr_جدول, &at_en);
    }
#اگر_ختم
    // For the term mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    اگر (sgp->sg_آغاز == تہی && sgp->sg_stop == تہی)
	sgp->sg_رکن_attr = sgp->sg_رکن;
    ورنہ
    {
	at_en.ae_attr = sgp->sg_رکن;
	at_en.ae_u.رکن.آغاز = sgp->sg_آغاز;
	at_en.ae_u.رکن.stop = sgp->sg_stop;
	sgp->sg_رکن_attr = get_attr_اندراج(&رکن_attr_جدول, &at_en);
    }

    // For the color term mode: If there are other than "normal"
    // highlighting attributes, need to allocate an attr number.
    اگر (sgp->sg_cterm_fg == 0 && sgp->sg_cterm_bg == 0 &&
	sgp->sg_cterm_ul == 0 && sgp->sg_cterm_font == 0
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
	    && sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
#اگر_ختم
	    )
	sgp->sg_cterm_attr = sgp->sg_cterm;
    ورنہ
    {
	at_en.ae_attr = sgp->sg_cterm;
	at_en.ae_u.cterm.fg_color = sgp->sg_cterm_fg;
	at_en.ae_u.cterm.bg_color = sgp->sg_cterm_bg;
	at_en.ae_u.cterm.ul_color = sgp->sg_cterm_ul;
	at_en.ae_u.cterm.font = sgp->sg_cterm_font;
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
	at_en.ae_u.cterm.fg_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_fg);
	at_en.ae_u.cterm.bg_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_bg);
	// Only use the underline/undercurl color when used, it may clear the
	// background color if not supported.
	اگر (sgp->sg_cterm & (HL_UNDERLINE | HL_UNDERCURL
			   | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED))
	    at_en.ae_u.cterm.ul_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_sp);
	ورنہ
	    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	اگر (at_en.ae_u.cterm.fg_rgb == INVALCOLOR
		&& at_en.ae_u.cterm.bg_rgb == INVALCOLOR)
	{
	    // If both fg and bg are invalid fall back to the cterm colors.
	    // Helps when the GUI only uses an attribute, e.g. undercurl.
	    at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	    at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
	}
#اگر_ختم
	sgp->sg_cterm_attr = get_attr_اندراج(&cterm_attr_جدول, &at_en);
    }
}

/*
 * Check if highlight id is overridden, and return the overriding highlight id.
 * Otherwise return the original id if no override.
 */
    ساکن صحیح
syn_override(صحیح id)
{
    اگر (overrides != تہی && overrides->arr != تہی)
	برائے (صحیح k = 0; k < overrides->len; k++)
	    اگر (overrides->arr[k].from == id)
		واپس overrides->arr[k].مقصد;
    واپس id;
}

/*
 * Lookup a highlight group name by pointer and length.
 * If it is not found, 0 is returned.
 */
    ساکن صحیح
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
	واپس 0;
    hi = hash_ڈھونڈو(&highlight_ht, name_u);
    اگر (HASHITEM_خالی_خانے(hi))
	واپس 0;
    واپس HI2HLNAME(hi)->hn_id;
}

/*
 * Lookup a highlight group name and return its ID.
 * If it is not found, 0 is returned.
 */
    صحیح
syn_name2id(char_u *name)
{
    // Avoid using stricmp() too much, it's slow on some systems.
    واپس syn_name2id_len(name, (صحیح)STRLEN(name));
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
	واپس syn_id2attr(id);
    واپس 0;
}

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE if highlight group "name" exists.
 */
    صحیح
highlight_exists(char_u *name)
{
    واپس (syn_name2id(name) > 0);
}

# اگر defined(FEAT_تلاش_EXTRA)
/*
 * Return the name of highlight group "id".
 * When not a valid ID return an empty string.
 */
    char_u *
syn_id2name(صحیح id)
{
    اگر (id <= 0 || id > highlight_ga.ga_len)
	واپس (char_u *)"";
    واپس HL_جدول()[id - 1].sg_name;
}
# endif
#اگر_ختم

/*
 * Like syn_name2id(), but take a pointer + length argument.
 */
    صحیح
syn_namen2id(char_u *linep, صحیح len)
{
    واپس syn_name2id_len(linep, len);
}

/*
 * Find highlight group name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    صحیح
syn_check_گروہ(char_u *pp, صحیح len)
{
    صحیح	    id;
    char_u  *name;

    اگر (len > MAX_SYN_NAME)
    {
	emsg(_(e_highlight_گروہ_name_too_long));
	واپس 0;
    }
    id = syn_name2id_len(pp, len);
    اگر (id == 0)			// doesn't exist yet
    {
	name = vim_strnsave(pp, len);
	اگر (name == تہی)
	    واپس 0;
	id = syn_جوڑو_گروہ(name);
    }
    واپس id;
}

/*
 * Add new highlight group and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    ساکن صحیح
syn_جوڑو_گروہ(char_u *name)
{
    char_u	*p;
    char_u	*name_اوپر;

    // Check that the name is valid (ASCII letters, digits, underscores, dots, or hyphens).
    برائے (p = name; *p != NUL; ++p)
    {
	اگر (!vim_isprintc(*p))
	{
	    emsg(_(e_unprintable_character_اندر_گروہ_name));
	    vim_free(name);
	    واپس 0;
	}
	ورنہ اگر (!ASCII_ISALNUM(*p) && *p != '_' && *p != '.' && *p != '-')
	{
	    // This is an error, but since there previously was no check only
	    // give a warning.
	    پیغام_ماخذ(HL_ATTR(HLF_W));
	    پیغام(_("W18: Invalid character in group name"));
	    توڑو;
	}
    }

    // First call for this growarray: init growing array.
    اگر (highlight_ga.ga_ڈیٹا == تہی)
    {
	highlight_ga.ga_itemsize = حجم(hl_گروہ_T);
	highlight_ga.ga_growsize = 10;
	hash_init(&highlight_ht);
	highlight_ht_inited = true;
    }

    اگر (highlight_ga.ga_len >= MAX_HL_ID)
    {
	emsg(_(e_too_many_highlight_and_syntax_groups));
	vim_free(name);
	واپس 0;
    }

    // Make room for at least one other syntax_highlight entry.
    اگر (ga_grow(&highlight_ga, 1) == FAIL)
    {
	vim_free(name);
	واپس 0;
    }

    {
	hlname_T    *hn;
	صحیح	    len = (صحیح)STRLEN(name);

	hn = alloc(offsetof(hlname_T, hn_کلید) + len + 1);
	اگر (hn == تہی)
	{
	    vim_free(name);
	    واپس 0;
	}
	vim_strncpy(hn->hn_کلید, name, len);
	vim_strup(hn->hn_کلید);
	hn->hn_id = highlight_ga.ga_len + 1;	// ID is index plus one
	name_اوپر = hn->hn_کلید;
    }

    CLEAR_اشارہ_گر(&(HL_جدول()[highlight_ga.ga_len]));
    HL_جدول()[highlight_ga.ga_len].sg_name = name;
    HL_جدول()[highlight_ga.ga_len].sg_name_u = name_اوپر;
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_جدول()[highlight_ga.ga_len].sg_gui_bg = INVALCOLOR;
    HL_جدول()[highlight_ga.ga_len].sg_gui_fg = INVALCOLOR;
    HL_جدول()[highlight_ga.ga_len].sg_gui_sp = INVALCOLOR;
#اگر_ختم
    hash_جوڑو(&highlight_ht, name_اوپر, "highlight");
    ++highlight_ga.ga_len;

    واپس highlight_ga.ga_len;		    // ID is index plus one
}

/*
 * When, just after calling syn_add_group(), an error is discovered, this
 * function deletes the new name.
 */
    ساکن خالی
syn_unadd_گروہ(خالی)
{
    hashitem_T *hi;

    --highlight_ga.ga_len;
    hi = hash_ڈھونڈو(&highlight_ht, HL_جدول()[highlight_ga.ga_len].sg_name_u);
    اگر (!HASHITEM_خالی_خانے(hi))
	hash_remove(&highlight_ht, hi, "highlight");
    vim_free(HL_جدول()[highlight_ga.ga_len].sg_name);
    vim_free(HL_جدول()[highlight_ga.ga_len].sg_name_u - HLNAME_کلید_OFF);
}

/*
 * Translate a group ID to highlight attributes.
 * "hl_id" must be valid: > 0, caller must check.
 */
    صحیح
syn_id2attr(صحیح hl_id)
{
    صحیح		attr;
    hl_گروہ_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    أكد(hl_id > 0);
    sgp = &HL_جدول()[hl_id - 1];	    // index is ID minus one

#اگر_تعریف_شدہ FEAT_GUI
    // Only use GUI attr when the GUI is being used.
    اگر (gui.اندر_use)
	attr = sgp->sg_gui_attr;
    ورنہ
#اگر_ختم
	اگر (IS_CTERM)
	    attr = sgp->sg_cterm_attr;
	ورنہ
	    attr = sgp->sg_رکن_attr;

    واپس attr;
}

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Get the GUI colors and attributes for a group ID.
 * NOTE: the colors will be INVALCOLOR when not set, the color otherwise.
 */
    صحیح
syn_id2colors(صحیح hl_id, guicolor_T *fgp, guicolor_T *bgp)
{
    hl_گروہ_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    أكد(hl_id > 0);
    sgp = &HL_جدول()[hl_id - 1];	    // index is ID minus one

    *fgp = sgp->sg_gui_fg;
    *bgp = sgp->sg_gui_bg;
    واپس sgp->sg_gui;
}
#اگر_ختم

#اگر (defined(MSWIN) \
	    && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL)) \
	    && defined(FEAT_TERMGUICOLORS)) \
	|| defined(FEAT_TERMINAL)
    خالی
syn_id2cterm_bg(صحیح hl_id, صحیح *fgp, صحیح *bgp)
{
    hl_گروہ_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    أكد(hl_id > 0);
    sgp = &HL_جدول()[hl_id - 1];	    // index is ID minus one
    *fgp = sgp->sg_cterm_fg - 1;
    *bgp = sgp->sg_cterm_bg - 1;
}
#اگر_ختم

/*
 * Translate a group ID to the final group ID (following links).
 */
    صحیح
syn_get_final_id(صحیح hl_id)
{
    صحیح		گنتی;
    hl_گروہ_T	*sgp;

    اگر (hl_id > highlight_ga.ga_len || hl_id < 1)
	واپس 0;			// Can be called from eval!!

    // Follow links until there is no more.
    // Look out for loops!  Break after 100 links.
    برائے (گنتی = 100; --گنتی >= 0; )
    {
	صحیح tmp = hl_id;

	sgp = &HL_جدول()[hl_id - 1];	    // index is ID minus one
	اگر (sgp->sg_link == 0 || sgp->sg_link > highlight_ga.ga_len)
	    توڑو;

	// This is to handle highlight groups that are overridden but are in the
	// middle of a link chain.
	hl_id = syn_override(hl_id);
	اگر (tmp != hl_id)
	    جاری;
	hl_id = sgp->sg_link;
    }
    واپس syn_override(hl_id);
}

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Call this function just after the GUI has started.
 * Also called when 'termguicolors' was set, gui.in_use will be FALSE then.
 * It finds the font and color handles for the highlighting groups.
 */
    خالی
highlight_gui_started(خالی)
{
    صحیح	    idx;

    // First get the colors from the "Normal" and "Menu" group, if set
    اگر (USE_24_ثنائی_ہندسہ)
	سیٹ_معمول_colors();

    برائے (idx = 0; idx < highlight_ga.ga_len; ++idx)
	gui_do_one_color(idx, FALSE, FALSE);

    highlight_بدلا();
}

    ساکن خالی
gui_do_one_color(
    صحیح		idx,
    صحیح		do_menu غیر_مستعمل,	   // TRUE: might set the menu font
    صحیح		do_tooltip غیر_مستعمل) // TRUE: might set the tooltip font
{
    صحیح		didit = FALSE;

# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    اگر (gui.اندر_use)
#  endif
	اگر (HL_جدول()[idx].sg_font_name != تہی)
	{
	    hl_do_font(idx, HL_جدول()[idx].sg_font_name, FALSE, do_menu,
							    do_tooltip, TRUE);
	    didit = TRUE;
	}
# endif
    اگر (HL_جدول()[idx].sg_gui_fg_name != تہی)
    {
	HL_جدول()[idx].sg_gui_fg =
			    color_name2handle(HL_جدول()[idx].sg_gui_fg_name);
	didit = TRUE;
    }
    اگر (HL_جدول()[idx].sg_gui_bg_name != تہی)
    {
	HL_جدول()[idx].sg_gui_bg =
			    color_name2handle(HL_جدول()[idx].sg_gui_bg_name);
	didit = TRUE;
    }
    اگر (HL_جدول()[idx].sg_gui_sp_name != تہی)
    {
	HL_جدول()[idx].sg_gui_sp =
			    color_name2handle(HL_جدول()[idx].sg_gui_sp_name);
	didit = TRUE;
    }
    اگر (didit)	// need to get a new attr number
	سیٹ_hl_attr(idx);
}
#اگر_ختم

#اگر defined(USER_HIGHLIGHT) && defined(FEAT_STL_OPT)
/*
 * Apply difference between User[1-9] and HLF_S to HLF_SNC, HLF_ST or HLF_STNC.
 */
    ساکن خالی
combine_stl_hlt(
	صحیح id,
	صحیح id_S,
	صحیح id_alt,
	صحیح hlcnt,
	صحیح i,
	صحیح hlf,
	صحیح *جدول)
{
    hl_گروہ_T *hlt = HL_جدول();

    اگر (id_alt == 0)
    {
	CLEAR_اشارہ_گر(&hlt[hlcnt + i]);
	hlt[hlcnt + i].sg_رکن = highlight_attr[hlf];
	hlt[hlcnt + i].sg_cterm = highlight_attr[hlf];
# اگر defined(FEAT_GUI) || defined(FEAT_EVAL)
	hlt[hlcnt + i].sg_gui = highlight_attr[hlf];
# endif
    }
    ورنہ
	mch_memmove(&hlt[hlcnt + i],
		    &hlt[id_alt - 1],
		    حجم(hl_گروہ_T));
    hlt[hlcnt + i].sg_link = 0;

    hlt[hlcnt + i].sg_رکن ^=
	hlt[id - 1].sg_رکن ^ hlt[id_S - 1].sg_رکن;
    اگر (hlt[id - 1].sg_آغاز != hlt[id_S - 1].sg_آغاز)
	hlt[hlcnt + i].sg_آغاز = hlt[id - 1].sg_آغاز;
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
    سیٹ_hl_attr(hlcnt + i);	// At long last we can apply
    جدول[i] = syn_id2attr(hlcnt + i + 1);
}
#اگر_ختم

/*
 * Translate the 'highlight' option into attributes in highlight_attr[] and
 * set up the user highlights User1..9.  If FEAT_STL_OPT is in use, a set of
 * corresponding highlights to use on top of HLF_SNC is computed.
 * Called only when the 'highlight' option has been changed and upon first
 * screen redraw after any :highlight command.
 * Return FAIL when an invalid flag is found in 'highlight'.  OK otherwise.
 */
    صحیح
highlight_بدلا(خالی)
{
    صحیح		hlf;
    صحیح		i;
    char_u	*p;
    char_u	*default_hl;
    صحیح		attr;
    char_u	*انتہا;
    صحیح		id;
#اگر_تعریف_شدہ USER_HIGHLIGHT
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
#اگر_ختم
    win_T	*wp;

    need_highlight_بدلا = FALSE;

#اگر_تعریف_شدہ FEAT_TERMINAL
    رکن_update_colors_all();
    رکن_update_hlfwin_all();
#اگر_ختم

    // Clear all attributes.
    برائے (hlf = 0; hlf < (صحیح)HLF_گنتی; ++hlf)
    {
	highlight_attr[hlf] = 0;
	highlight_attr_raw[hlf] = 0;
	highlight_ids[hlf] = 0;
    }

    default_hl = get_highlight_default();

    // First set all attributes to their default value.
    // Then use the attributes from the 'highlight' option, unless it already
    // matches the default and would repeat the same work.
    برائے (i = 0; i < 2; ++i)
    {
	اگر (i)
	{
	    اگر (default_hl != تہی && p_hl != تہی
				      && STRCMP(default_hl, p_hl) == 0)
		جاری;
	    p = p_hl;
	}
	ورنہ
	    p = default_hl;
	اگر (p == تہی)	    // just in case
	    جاری;

	جب (*p)
	{
	    برائے (hlf = 0; hlf < (صحیح)HLF_گنتی; ++hlf)
		اگر (hl_flags[hlf] == *p)
		    توڑو;
	    ++p;
	    اگر (hlf == (صحیح)HLF_گنتی || *p == NUL)
		واپس FAIL;

	    // Allow several hl_flags to be combined, like "bu" for
	    // bold-underlined.
	    attr = 0;
	    id = 0;
	    برائے ( ; *p && *p != ','; ++p)	    // parse up to comma
	    {
		اگر (VIM_ISWHITE(*p))		    // ignore white space
		    جاری;

		اگر (attr > HL_ALL)  // Combination with ':' is not allowed.
		    واپس FAIL;

		// Note: Keep this in sync with expand_set_highlight().
		انتخاب (*p)
		{
		    حالت 'b':	attr |= HL_BOLD;
				توڑو;
		    حالت 'i':	attr |= HL_ITALIC;
				توڑو;
		    حالت '-':
		    حالت 'n':			    // no highlighting
				توڑو;
		    حالت 'r':	attr |= HL_INVERSE;
				توڑو;
		    حالت 's':	attr |= HL_STANDOUT;
				توڑو;
		    حالت 'u':	attr |= HL_UNDERLINE;
				توڑو;
		    حالت 'c':	attr |= HL_UNDERCURL;
				توڑو;
		    حالت '2':	attr |= HL_UNDERDOUBLE;
				توڑو;
		    حالت 'd':	attr |= HL_UNDERDOTTED;
				توڑو;
		    حالت '=':	attr |= HL_UNDERDASHED;
				توڑو;
		    حالت 't':	attr |= HL_STRIKETHROUGH;
				توڑو;
		    حالت ':':	++p;		    // highlight group name
				اگر (attr || *p == NUL)	 // no combinations
				    واپس FAIL;
				انتہا = vim_strchr(p, ',');
				اگر (انتہا == تہی)
				    انتہا = p + STRLEN(p);
				id = syn_check_گروہ(p, (صحیح)(انتہا - p));
				اگر (id == 0)
				    واپس FAIL;
				attr = syn_id2attr(id);
				p = انتہا - 1;
#اگر defined(FEAT_STL_OPT) && defined(USER_HIGHLIGHT)
				اگر (hlf == (صحیح)HLF_SNC)
				    id_SNC = syn_get_final_id(id);
# ifdef FEAT_TERMINAL
				ورنہ اگر (hlf == (صحیح)HLF_ST)
				    id_ST = syn_get_final_id(id);
				ورنہ اگر (hlf == (صحیح)HLF_STNC)
				    id_STNC = syn_get_final_id(id);
# endif
				ورنہ اگر (hlf == (صحیح)HLF_S)
				    id_S = syn_get_final_id(id);
#اگر_ختم
				توڑو;
		    طےشدہ:	واپس FAIL;
		}
	    }
	    highlight_attr[hlf] = attr;
	    highlight_attr_raw[hlf] = attr;
	    highlight_ids[hlf] = id;

	    p = skip_مقصد_option_part(p);	    // skip comma and spaces
	}
    }

    // Highlight ids may have been changed, so keep windows up to date
    FOR_ALL_WINDOWS(wp)
    {
	حرف *errmsg = update_winhighlight(wp, wp->w_p_whl);

	اگر (errmsg != تہی)
	    emsg(_(errmsg));
    }

#اگر_تعریف_شدہ FEAT_TERMINAL
    رکن_update_hlfwin_all();
#اگر_ختم

#اگر_تعریف_شدہ USER_HIGHLIGHT
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
	واپس FAIL;
    hlcnt = highlight_ga.ga_len;
    اگر (id_S == -1)
    {
	// Make sure id_S is always valid to simplify code below. Use the last
	// entry.
	CLEAR_اشارہ_گر(&HL_جدول()[hlcnt + 27]);
	HL_جدول()[hlcnt + 18].sg_رکن = highlight_attr[HLF_S];
	id_S = hlcnt + 19;
    }
# endif
    برائے (i = 0; i < 9; i++)
    {
	sprintf((حرف *)userhl, "User%d", i + 1);
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
	ورنہ
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

#اگر_ختم // USER_HIGHLIGHT

    واپس OK;
}

ساکن خالی highlight_list(خالی);
ساکن خالی highlight_list_two(صحیح cnt, صحیح attr);

// context for :highlight <group> <arg> expansion
ساکن صحیح expand_hi_synid = 0;	    // ID for highlight group being completed
ساکن صحیح expand_hi_equal_ستون = 0; // column where the '=' is
ساکن صحیح expand_hi_include_orig = 0;	    // whether to fill the existing current value or not
ساکن char_u *expand_hi_curvalue = تہی;   // the existing current value
#اگر defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
ساکن dict_iterator_T expand_colornames_iter;	// iterator for looping through v:colornames
#اگر_ختم

/*
 * Handle command line completion for :highlight command.
 */
    خالی
سیٹ_context_اندر_highlight_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;
    صحیح		expand_گروہ = TRUE;

    // Default: expand group names
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_نمونہ = arg;
    include_none = 0;
    include_link = 2;
    include_default = 1;

    اگر (*arg == NUL)
	واپس;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    اگر (*p == NUL)
	واپس;

    // past "default" or group name
    include_default = 0;
    اگر (STRNCMP("default", arg, p - arg) == 0)
    {
	arg = skipwhite(p);
	xp->xp_نمونہ = arg;
	p = skiptowhite(arg);
    }
    اگر (*p == NUL)
	واپس;

    // past group name
    include_link = 0;
    اگر (arg[1] == 'i' && arg[0] == 'N')
    {
	highlight_list();
	expand_گروہ = FALSE;
    }
    اگر (STRNCMP("link", arg, p - arg) == 0)
    {
	xp->xp_نمونہ = skipwhite(p);
	p = skiptowhite(xp->xp_نمونہ);
	اگر (*p != NUL)		// past first group name
	{
	    xp->xp_نمونہ = skipwhite(p);
	    p = skiptowhite(xp->xp_نمونہ);
	    include_none = 1;
	}
	expand_گروہ = FALSE;
    }
    ورنہ اگر (STRNCMP("clear", arg, p - arg) == 0)
    {
	xp->xp_نمونہ = skipwhite(p);
	p = skiptowhite(xp->xp_نمونہ);
	expand_گروہ = FALSE;
    }
    اگر (*p != NUL)			// past group name(s)
    {
	اگر (expand_گروہ)
	{
	    // expansion will be done in expand_highlight_group()
	    xp->xp_context = EXPAND_HIGHLIGHT_گروہ;

	    expand_hi_synid = syn_namen2id(arg, (صحیح)(p - arg));

	    جب (*p != NUL)
	    {
		arg = skipwhite(p);
		p = skiptowhite(arg);
	    }

	    p = vim_strchr(arg, '=');
	    اگر (p == تہی)
	    {
		// Didn't find a key=<value> pattern
		xp->xp_نمونہ = arg;
		expand_hi_equal_ستون = -1;
		expand_hi_include_orig = FALSE;
	    }
	    ورنہ
	    {
		// Found key=<value> pattern, record the exact location
		expand_hi_equal_ستون = (صحیح)(p - xp->xp_سطر);

		// Only include the original value if the pattern is empty
		اگر (*(p + 1) == NUL)
		    expand_hi_include_orig = TRUE;
		ورنہ
		    expand_hi_include_orig = FALSE;

		// Account for comma-separated values
		اگر (STRNCMP(arg, "term=", 5) == 0 ||
			STRNCMP(arg, "cterm=", 6) == 0 ||
			STRNCMP(arg, "gui=", 4) == 0)
		{
		    char_u *comma = vim_strrchr(p + 1, ',');
		    اگر (comma != تہی)
			p = comma;
		}
		xp->xp_نمونہ = p + 1;
	    }
	}
	ورنہ
	{
	    xp->xp_context = EXPAND_NOTHING;
	}
    }
}

/*
 * List highlighting matches in a nice way.
 */
    ساکن خالی
highlight_list(خالی)
{
    صحیح		i;

    برائے (i = 10; --i >= 0; )
	highlight_list_two(i, HL_ATTR(HLF_D));
    برائے (i = 40; --i >= 0; )
	highlight_list_two(99, 0);
}

    ساکن خالی
highlight_list_two(صحیح cnt, صحیح attr)
{
    پیغام_puts_attr(&("N \bI \b!  \b"[cnt / 11]), attr);
    پیغام_clr_eos();
    برآمد_flush();
    ui_delay(cnt == 99 ? 40L : (طویل)cnt * 50L, FALSE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_highlight_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    واپس get_highlight_name_ext(xp, idx, TRUE);
}

/*
 * Obtain a highlight group name.
 * When "skip_cleared" is TRUE don't return a cleared entry.
 */
    char_u *
get_highlight_name_ext(expand_T *xp غیر_مستعمل, صحیح idx, صحیح skip_cleared)
{
    اگر (idx < 0)
	واپس تہی;

    // Items are never removed from the table, skip the ones that were
    // cleared.
    اگر (skip_cleared && idx < highlight_ga.ga_len && HL_جدول()[idx].sg_cleared)
	واپس (char_u *)"";

    اگر (idx == highlight_ga.ga_len && include_none != 0)
	واپس (char_u *)"NONE";
    اگر (idx == highlight_ga.ga_len + include_none && include_default != 0)
	واپس (char_u *)"default";
    اگر (idx == highlight_ga.ga_len + include_none + include_default
							 && include_link != 0)
	واپس (char_u *)"link";
    اگر (idx == highlight_ga.ga_len + include_none + include_default + 1
							 && include_link != 0)
	واپس (char_u *)"clear";
    اگر (idx >= highlight_ga.ga_len)
	واپس تہی;
    واپس HL_جدول()[idx].sg_name;
}

    ساکن char_u *
get_highlight_attr_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx == 0)
    {
	// Fill with current value first
	اگر (expand_hi_curvalue != تہی)
	    واپس expand_hi_curvalue;
	ورنہ
	    واپس (char_u*)"";
    }
    اگر (idx < (صحیح)ARRAY_لمبائی(highlight_اشاریہ_tab) + 1)
    {
	char_u *قدر = highlight_اشاریہ_tab[idx-1]->قدر.string;
	اگر (expand_hi_curvalue != تہی && STRCMP(expand_hi_curvalue, قدر) == 0)
	{
	    // Already returned the current value above, just skip.
	    واپس (char_u*)"";
	}
	واپس قدر;
    }
    واپس تہی;
}

    ساکن char_u *
get_highlight_cterm_color(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx == 0)
    {
	// Fill with current value first
	اگر (expand_hi_curvalue != تہی)
	    واپس expand_hi_curvalue;
	ورنہ
	    واپس (char_u*)"";
    }
    // See highlight_set_cterm_color()
    ورنہ اگر (idx == 1)
	واپس (char_u*)"fg";
    ورنہ اگر (idx == 2)
	واپس (char_u*)"bg";
    اگر (idx < (صحیح)ARRAY_لمبائی(color_name_tab) + 3)
    {
	char_u *قدر = color_name_tab[idx-3].قدر.string;
	واپس قدر;
    }
    واپس تہی;
}

#اگر defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
    ساکن char_u *
get_highlight_gui_color(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx == 0)
    {
	// Fill with current value first
	اگر (expand_hi_curvalue != تہی)
	    واپس expand_hi_curvalue;
	ورنہ
	    واپس (char_u*)"";
    }
    // See color_name2handle()
    ورنہ اگر (idx == 1)
	واپس (char_u*)"fg";
    ورنہ اگر (idx == 2)
	واپس (char_u*)"bg";
    ورنہ اگر (idx == 3)
	واپس (char_u*)"NONE";

    // Complete from v:colornames. Don't do platform specific names for now.
    typval_T *tv_نتیجہ;
    char_u *colorname = dict_iterate_اگلا(&expand_colornames_iter, &tv_نتیجہ);
    اگر (colorname != تہی)
    {
	// :hi command doesn't allow space, so don't suggest any malformed items
	اگر (vim_strchr(colorname, ' ') != تہی)
	    واپس (char_u*)"";

	اگر (expand_hi_curvalue != تہی && STRICMP(expand_hi_curvalue, colorname) == 0)
	{
	    // Already returned the current value above, just skip.
	    واپس (char_u*)"";
	}
    }
    واپس colorname;
}
#اگر_ختم

    ساکن char_u *
get_highlight_گروہ_کلید(expand_T *xp غیر_مستعمل, صحیح idx)
{
    // Note: Keep this in sync with do_highlight.
    ساکن حرف *(p_hi_گروہ_کلید_اقدار[]) =
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
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
	"font=",
#اگر_ختم
	"NONE",
    };

    اگر (idx < (صحیح)ARRAY_لمبائی(p_hi_گروہ_کلید_اقدار))
	واپس (char_u*)p_hi_گروہ_کلید_اقدار[idx];
    واپس تہی;
}

/*
 * Command-line expansion for :hi {group-name} <args>...
 */
    صحیح
expand_highlight_گروہ(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	صحیح	    *عدد_Matches)
{
    اگر (expand_hi_equal_ستون != -1)
    {
	// List the values. First fill in the current value, then if possible colors
	// or attribute names.
	char_u	    *(*expandfunc)(expand_T *, صحیح) = تہی;
	صحیح	    type = 0;
	hl_گروہ_T  *sgp = تہی;
	صحیح	    iarg = 0;
	char_u	    *sarg = تہی;

	صحیح	    unsorted_اشیاء = -1; // don't sort by default

	اگر (expand_hi_synid != 0)
	    sgp = &HL_جدول()[expand_hi_synid - 1]; // index is ID minus one

	// Note: Keep this in sync with highlight_list_one().
	char_u	    *name_انتہا = xp->xp_سطر + expand_hi_equal_ستون;
	اگر (name_انتہا - xp->xp_سطر >= 5
		&& STRNCMP(name_انتہا - 5, " term", 5) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    اگر (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_رکن;
	    }
	}
	ورنہ اگر (name_انتہا - xp->xp_سطر >= 6
		&& STRNCMP(name_انتہا - 6, " cterm", 6) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    اگر (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_cterm;
	    }
	}
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	ورنہ اگر (name_انتہا - xp->xp_سطر >= 4
		&& STRNCMP(name_انتہا - 4, " gui", 4) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    اگر (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_gui;
	    }
	}
#اگر_ختم
	ورنہ اگر (name_انتہا - xp->xp_سطر >= 8
		&& STRNCMP(name_انتہا - 8, " ctermfg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    اگر (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_fg;
	    }
	}
	ورنہ اگر (name_انتہا - xp->xp_سطر >= 8
		&& STRNCMP(name_انتہا - 8, " ctermbg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    اگر (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_bg;
	    }
	}
	ورنہ اگر (name_انتہا - xp->xp_سطر >= 8
		&& STRNCMP(name_انتہا - 8, " ctermul", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    اگر (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_ul;
	    }
	}
#اگر defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	ورنہ اگر (name_انتہا - xp->xp_سطر >= 6
		&& STRNCMP(name_انتہا - 6, " guifg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    اگر (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_fg_name;
	    }
	}
	ورنہ اگر (name_انتہا - xp->xp_سطر >= 6
		&& STRNCMP(name_انتہا - 6, " guibg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    اگر (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_bg_name;
	    }
	}
	ورنہ اگر (name_انتہا - xp->xp_سطر >= 6
		&& STRNCMP(name_انتہا - 6, " guisp", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    اگر (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_sp_name;
	    }
	}
#اگر_ختم

#اگر defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	اگر (expandfunc == get_highlight_gui_color)
	{
	    // Top 4 items are special, after that sort all the color names
	    unsorted_اشیاء = 4;

	    dict_T *colornames_جدول = get_vim_var_dict(VV_COLORNAMES);
	    typval_T colornames_val;
	    colornames_val.v_type = VAR_DICT;
	    colornames_val.vval.v_dict = colornames_جدول;
	    dict_iterate_آغاز(&colornames_val, &expand_colornames_iter);
	}
#اگر_ختم

	char_u	    buf[MAX_ATTR_LEN];

	expand_hi_curvalue = تہی;
	اگر (expand_hi_include_orig)
	{
	    اگر (((type == LIST_ATTR || type == LIST_INT) && iarg != 0) ||
		(type == LIST_STRING && sarg != تہی))
	    {
		// Retrieve the current value to go first in completion
		expand_hi_curvalue = highlight_arg_مقصد_string(
			type, iarg, sarg, buf);
	    }
	}

	اگر (expandfunc != تہی)
	{
	    واپس ExpandGenericExt(
		    pat,
		    xp,
		    rmp,
		    matches,
		    عدد_Matches,
		    expandfunc,
		    FALSE,
		    unsorted_اشیاء);
	}

	واپس FAIL;
    }

    // List all the key names
    واپس ExpandGenericExt(
	    pat,
	    xp,
	    rmp,
	    matches,
	    عدد_Matches,
	    get_highlight_گروہ_کلید,
	    FALSE,
	    -1);
}

#اگر defined(FEAT_GUI)
/*
 * Free all the highlight group fonts.
 * Used when quitting for systems which need it.
 */
    خالی
free_highlight_fonts(خالی)
{
    صحیح	    idx;

    برائے (idx = 0; idx < highlight_ga.ga_len; ++idx)
    {
	gui_mch_free_font(HL_جدول()[idx].sg_font);
	HL_جدول()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
	gui_mch_free_fontset(HL_جدول()[idx].sg_fontset);
	HL_جدول()[idx].sg_fontset = NOFONTSET;
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
#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * Convert each of the highlight attribute bits (bold, standout, underline,
 * etc.) set in 'hlattr' into a separate boolean item in a Dictionary with
 * the attribute name as the key.
 */
    ساکن dict_T *
highlight_get_attr_dict(صحیح hlattr)
{
    dict_T	*dict;
    صحیح		i;

    dict = dict_alloc();
    اگر (dict == تہی)
	واپس تہی;

    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(highlight_اشاریہ_tab); ++i)
    {
	اگر (hlattr & highlight_اشاریہ_tab[i]->کلید)
	{
	    dict_جوڑو_bool(dict, (حرف *)highlight_اشاریہ_tab[i]->قدر.string,
		    VVAL_TRUE);
	    // don't want "inverse"/"reverse"
	    hlattr &= ~highlight_اشاریہ_tab[i]->کلید;
	}
    }

    واپس dict;
}

/*
 * Return the attributes of the highlight group at index 'hl_idx' as a
 * Dictionary. If 'resolve_link' is TRUE, then resolves the highlight group
 * links recursively.
 */
    ساکن dict_T *
highlight_get_info(صحیح hl_idx, صحیح resolve_link)
{
    dict_T	*dict;
    hl_گروہ_T	*sgp;
    dict_T	*attr_dict = تہی;
    صحیح		hlgid;

    dict = dict_alloc();
    اگر (dict == تہی)
	واپس dict;

    sgp = &HL_جدول()[hl_idx];
    // highlight group id is 1-based
    hlgid = hl_idx + 1;

    اگر (dict_جوڑو_string(dict, "name", sgp->sg_name) == FAIL)
	جاؤ error;
    اگر (dict_جوڑو_نمبر(dict, "id", hlgid) == FAIL)
	جاؤ error;

    اگر (resolve_link)
    {
	// resolve the highlight group link recursively
	جب (sgp->sg_link)
	{
	    hlgid = sgp->sg_link;
	    sgp = &HL_جدول()[sgp->sg_link - 1];
	}
    }

    اگر (sgp->sg_رکن != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_رکن);
	اگر (attr_dict != تہی)
	{
	    اگر (dict_جوڑو_dict(dict, "term", attr_dict) == FAIL)
		جاؤ error;
	    attr_dict = تہی;
	}
    }
    اگر (sgp->sg_آغاز != تہی)
	اگر (dict_جوڑو_string(dict, "start", sgp->sg_آغاز) == FAIL)
	    جاؤ error;
    اگر (sgp->sg_stop != تہی)
	اگر (dict_جوڑو_string(dict, "stop", sgp->sg_stop) == FAIL)
	    جاؤ error;
    اگر (sgp->sg_cterm != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_cterm);
	اگر (attr_dict != تہی)
	{
	    اگر (dict_جوڑو_dict(dict, "cterm", attr_dict) == FAIL)
		جاؤ error;
	    attr_dict = تہی;
	}
    }
    اگر (sgp->sg_cterm_fg != 0)
	اگر (dict_جوڑو_string(dict, "ctermfg",
		    highlight_color(hlgid, (char_u *)"fg", 'c')) == FAIL)
	    جاؤ error;
    اگر (sgp->sg_cterm_bg != 0)
	اگر (dict_جوڑو_string(dict, "ctermbg",
			highlight_color(hlgid, (char_u *)"bg", 'c')) == FAIL)
	    جاؤ error;
    اگر (sgp->sg_cterm_ul != 0)
	اگر (dict_جوڑو_string(dict, "ctermul",
			highlight_color(hlgid, (char_u *)"ul", 'c')) == FAIL)
	    جاؤ error;
    اگر (sgp->sg_cterm_font != 0)
	اگر (dict_جوڑو_string(dict, "ctermfont",
			highlight_color(hlgid, (char_u *)"font", 'c')) == FAIL)
	    جاؤ error;
    اگر (sgp->sg_gui != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_gui);
	اگر (attr_dict != تہی)
	{
	    اگر (dict_جوڑو_dict(dict, "gui", attr_dict) == FAIL)
		جاؤ error;
	    attr_dict = تہی;
	}
    }
    اگر (sgp->sg_gui_fg_name != تہی)
	اگر (dict_جوڑو_string(dict, "guifg",
			highlight_color(hlgid, (char_u *)"fg", 'g')) == FAIL)
	    جاؤ error;
    اگر (sgp->sg_gui_bg_name != تہی)
	اگر (dict_جوڑو_string(dict, "guibg",
			highlight_color(hlgid, (char_u *)"bg", 'g')) == FAIL)
	    جاؤ error;
    اگر (sgp->sg_gui_sp_name != تہی)
	اگر (dict_جوڑو_string(dict, "guisp",
			highlight_color(hlgid, (char_u *)"sp", 'g')) == FAIL)
	    جاؤ error;
# ifdef FEAT_GUI
    اگر (sgp->sg_font_name != تہی)
	اگر (dict_جوڑو_string(dict, "font", sgp->sg_font_name) == FAIL)
	    جاؤ error;
# endif
    اگر (sgp->sg_link)
    {
	char_u	*link;

	link = HL_جدول()[sgp->sg_link - 1].sg_name;
	اگر (link != تہی && dict_جوڑو_string(dict, "linksto", link) == FAIL)
	    جاؤ error;

	اگر (sgp->sg_deflink)
	    dict_جوڑو_bool(dict, "default", VVAL_TRUE);
    }
    اگر (dict_len(dict) == 2)
	// If only 'name' is present, then the highlight group is cleared.
	dict_جوڑو_bool(dict, "cleared", VVAL_TRUE);

    واپس dict;

error:
    dict_unref(attr_dict);
    dict_unref(dict);
    واپس تہی;
}

/*
 * "hlget([name])" function
 * Return the attributes of a specific highlight group (if specified) or all
 * the highlight groups.
 */
    خالی
f_hlget(typval_T *argvars, typval_T *rettv)
{
    list_T	*list;
    dict_T	*dict;
    صحیح		i;
    char_u	*hlarg = تہی;
    صحیح		resolve_link = FALSE;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (check_for_opt_string_arg(argvars, 0) == FAIL
	    || (argvars[0].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_arg(argvars, 1) == FAIL))
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	// highlight group name supplied
	hlarg = tv_get_string_chk(&argvars[0]);
	اگر (hlarg == تہی)
	    واپس;

	اگر (argvars[1].v_type != VAR_UNKNOWN)
	{
	    صحیح error = FALSE;

	    resolve_link = tv_get_bool_chk(&argvars[1], &error);
	    اگر (error)
		واپس;
	}
    }

    list = rettv->vval.v_list;
    برائے (i = 0; i < highlight_ga.ga_len && !got_int; ++i)
    {
	اگر (hlarg == تہی || STRICMP(hlarg, HL_جدول()[i].sg_name) == 0)
	{
	    dict = highlight_get_info(i, resolve_link);
	    اگر (dict != تہی && list_append_dict(list, dict) == FAIL)
		dict_unref(dict);
	}
    }
}

/*
 * Returns the string value at 'dict[key]'. Returns NULL, if 'key' is not in
 * 'dict' or the value is not a string type. If the value is not a string type
 * or is NULL, then 'error' is set to TRUE.
 */
    ساکن char_u *
hldict_get_string(dict_T *dict, char_u *کلید, صحیح *error)
{
    dictitem_T	*di;

    *error = FALSE;
    di = dict_ڈھونڈو(dict, کلید, -1);
    اگر (di == تہی)
	واپس تہی;

    اگر (di->di_tv.v_type != VAR_STRING || di->di_tv.vval.v_string == تہی)
    {
	emsg(_(e_string_required));
	*error = TRUE;
	واپس تہی;
    }

    واپس di->di_tv.vval.v_string;
}

/*
 * Convert the highlight attribute Dictionary at 'dict[key]' into a string
 * value in 'attr_str' of length 'len'. Returns FALSE if 'dict[key]' is not a
 * Dictionary or is NULL.
 */
    ساکن صحیح
hldict_attr_مقصد_str(
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
    di = dict_ڈھونڈو(dict, کلید, -1);
    اگر (di == تہی)
	واپس TRUE;

    اگر (di->di_tv.v_type != VAR_DICT || di->di_tv.vval.v_dict == تہی)
    {
	emsg(_(e_dictionary_required));
	واپس FALSE;
    }

    attrdict = di->di_tv.vval.v_dict;

    // If the attribute dict is empty, then return NONE to clear the attributes
    اگر (dict_len(attrdict) == 0)
    {
	vim_strcat(attr_str, (char_u *)"NONE", len);
	واپس TRUE;
    }

    p = attr_str;
    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(highlight_tab); ++i)
    {
	اگر (dict_get_bool(attrdict, (حرف *)highlight_tab[i].قدر.string,
		    VVAL_FALSE) == VVAL_TRUE)
	{
	    اگر (p != attr_str && (size_t)(p - attr_str + 2) < len)
		STRCPY(p, (char_u *)",");
	    اگر (p - attr_str + highlight_tab[i].قدر.لمبائی + 1 < len)
	    {
		STRCPY(p, highlight_tab[i].قدر.string);
		p += highlight_tab[i].قدر.لمبائی;
	    }
	}
    }

    واپس TRUE;
}

// Temporary buffer used to store the command string produced by hlset().
// IObuff cannot be used for this as the error messages produced by hlset()
// internally use IObuff.
# define	HLSETBUFSZ  512
ساکن char_u hlsetBuf[HLSETBUFSZ + 1];

/*
 * Add the highlight attribute "attr" of length "attrlen" and "value" at
 * "dptr", which points into "hlsetBuf".
 * Returns the updated pointer.
 */
    ساکن char_u *
جوڑو_attr_and_قدر(char_u *dptr, char_u *attr, صحیح attrlen, char_u *قدر)
{
    size_t	vallen;

    // Do nothing if the value is not specified or is empty
    اگر (قدر == تہی || *قدر == NUL)
	واپس dptr;

    vallen = STRLEN(قدر);
    // When the value contains a space and the attribute has an "=" (i.e. it
    // is a key=value pair), surround the value with single quotes so that
    // do_highlight() can parse it correctly.
    اگر (vim_strchr(قدر, ' ') != تہی && vim_strchr(attr, '=') != تہی)
    {
	اگر (dptr + attrlen + vallen + 3 < hlsetBuf + HLSETBUFSZ)
	{
	    STRCPY(dptr, attr);
	    dptr += attrlen;
	    *dptr++ = '\'';
	    STRCPY(dptr, قدر);
	    dptr += vallen;
	    *dptr++ = '\'';
	    *dptr = NUL;
	}
    }
    ورنہ اگر (dptr + attrlen + vallen + 1 < hlsetBuf + HLSETBUFSZ)
    {
	STRCPY(dptr, attr);
	dptr += attrlen;
	STRCPY(dptr, قدر);
	dptr += vallen;
    }

    واپس dptr;
}

/*
 * Add or update a highlight group using 'dict' items. Returns TRUE if
 * successfully updated the highlight group.
 */
    ساکن صحیح
hlg_جوڑو_or_update(dict_T *dict)
{
    char_u	*name;
    صحیح		error;
    char_u	رکن_attr[MAX_ATTR_LEN];
    char_u	cterm_attr[MAX_ATTR_LEN];
    char_u	gui_attr[MAX_ATTR_LEN];
    char_u	*آغاز;
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
    صحیح		مکمل = FALSE;
    char_u	*p;

    name = hldict_get_string(dict, (char_u *)"name", &error);
    اگر (name == تہی || *name == NUL || error)
	واپس FALSE;

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
	    vim_snprintf((حرف *)hlsetBuf, HLSETBUFSZ, "clear %s", name);
	    do_highlight(hlsetBuf, forceit, FALSE);
	    مکمل = TRUE;
	}
    }

    اگر (dict_has_کلید(dict, "linksto"))
    {
	char_u	*linksto;

	// link highlight groups
	linksto = hldict_get_string(dict, (char_u *)"linksto", &error);
	اگر (linksto == تہی || *linksto == NUL || error)
	    واپس FALSE;

	vim_snprintf((حرف *)hlsetBuf, HLSETBUFSZ, "%slink %s %s",
				dodefault ? "default " : "", name, linksto);
	do_highlight(hlsetBuf, forceit, FALSE);

	مکمل = TRUE;
    }

    // If 'cleared' or 'linksto' are specified, then don't process the other
    // attributes.
    اگر (مکمل)
	واپس TRUE;

    آغاز = hldict_get_string(dict, (char_u *)"start", &error);
    اگر (error)
	واپس FALSE;

    stop = hldict_get_string(dict, (char_u *)"stop", &error);
    اگر (error)
	واپس FALSE;

    اگر (!hldict_attr_مقصد_str(dict, (char_u *)"term", رکن_attr,
							    حجم(رکن_attr)))
	واپس FALSE;

    اگر (!hldict_attr_مقصد_str(dict, (char_u *)"cterm", cterm_attr,
							   حجم(cterm_attr)))
	واپس FALSE;

    ctermfg = hldict_get_string(dict, (char_u *)"ctermfg", &error);
    اگر (error)
	واپس FALSE;

    ctermbg = hldict_get_string(dict, (char_u *)"ctermbg", &error);
    اگر (error)
	واپس FALSE;

    ctermul = hldict_get_string(dict, (char_u *)"ctermul", &error);
    اگر (error)
	واپس FALSE;

    ctermfont = hldict_get_string(dict, (char_u *)"ctermfont", &error);
    اگر (error)
	واپس FALSE;

    اگر (!hldict_attr_مقصد_str(dict, (char_u *)"gui", gui_attr, حجم(gui_attr)))
	واپس FALSE;

    guifg = hldict_get_string(dict, (char_u *)"guifg", &error);
    اگر (error)
	واپس FALSE;

    guibg = hldict_get_string(dict, (char_u *)"guibg", &error);
    اگر (error)
	واپس FALSE;

    guisp = hldict_get_string(dict, (char_u *)"guisp", &error);
    اگر (error)
	واپس FALSE;

# ifdef FEAT_GUI
    font = hldict_get_string(dict, (char_u *)"font", &error);
    اگر (error)
	واپس FALSE;
# endif

    // If none of the attributes are specified, then do nothing.
    اگر (رکن_attr[0] == NUL && آغاز == تہی && stop == تہی
	    && cterm_attr[0] == NUL && ctermfg == تہی && ctermbg == تہی
	    && ctermul == تہی && ctermfont == تہی && gui_attr[0] == NUL
# ifdef FEAT_GUI
	    && font == تہی
# endif
	    && guifg == تہی && guibg == تہی && guisp == تہی
	    )
	واپس TRUE;

    hlsetBuf[0] = NUL;
    p = hlsetBuf;
    اگر (dodefault)
	p = جوڑو_attr_and_قدر(p, (char_u *)"default", 7, (char_u *)" ");
    p = جوڑو_attr_and_قدر(p, (char_u *)"", 0, name);
    p = جوڑو_attr_and_قدر(p, (char_u *)" term=", 6, رکن_attr);
    p = جوڑو_attr_and_قدر(p, (char_u *)" start=", 7, آغاز);
    p = جوڑو_attr_and_قدر(p, (char_u *)" stop=", 6, stop);
    p = جوڑو_attr_and_قدر(p, (char_u *)" cterm=", 7, cterm_attr);
    p = جوڑو_attr_and_قدر(p, (char_u *)" ctermfg=", 9, ctermfg);
    p = جوڑو_attr_and_قدر(p, (char_u *)" ctermbg=", 9, ctermbg);
    p = جوڑو_attr_and_قدر(p, (char_u *)" ctermul=", 9, ctermul);
    p = جوڑو_attr_and_قدر(p, (char_u *)" ctermfont=", 11, ctermfont);
    p = جوڑو_attr_and_قدر(p, (char_u *)" gui=", 5, gui_attr);
# ifdef FEAT_GUI
    p = جوڑو_attr_and_قدر(p, (char_u *)" font=", 6, font);
# endif
    p = جوڑو_attr_and_قدر(p, (char_u *)" guifg=", 7, guifg);
    p = جوڑو_attr_and_قدر(p, (char_u *)" guibg=", 7, guibg);
    (خالی)جوڑو_attr_and_قدر(p, (char_u *)" guisp=", 7, guisp);

    do_highlight(hlsetBuf, forceit, FALSE);

    واپس TRUE;
}

/*
 * "hlset([{highlight_attr}])" function
 * Add or modify highlight groups
 */
    خالی
f_hlset(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    dict_T	*dict;

    rettv->vval.v_نمبر = -1;

    اگر (check_for_list_arg(argvars, 0) == FAIL)
	واپس;

    FOR_ALL_LIST_اشیاء(argvars->vval.v_list, li)
    {
	اگر (li->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_dictionary_required));
	    واپس;
	}

	dict = li->li_tv.vval.v_dict;
	اگر (!hlg_جوڑو_or_update(dict))
	    واپس;
    }

    rettv->vval.v_نمبر = 0;
}
#اگر_ختم

/*
 * If "old" is in the override stack, then update it to "hl_new". Does not free
 * "old".
 */
    خالی
update_highlight_overrides(hl_override_T *old, hl_override_T *hl_new, صحیح newlen)
{
    اگر (old == تہی)
	واپس;

    برائے (hl_overrides_T *سیٹ_2 = overrides; سیٹ_2 != تہی; سیٹ_2 = سیٹ_2->اگلا)
    {
	اگر (سیٹ_2->arr == old)
	{
	    سیٹ_2->arr = hl_new;
	    سیٹ_2->len = newlen;
	}
    }
}

/*
 * If "arr" is in the highlight overrides list, then mark it as invalid.
 */
    خالی
remove_highlight_overrides(hl_override_T *arr)
{
    اگر (arr == تہی || overrides == تہی)
	واپس;

    برائے (hl_overrides_T *سیٹ_2 = overrides; سیٹ_2 != تہی; سیٹ_2 = سیٹ_2->اگلا)
    {
	اگر (سیٹ_2->arr == arr)
	{
	    سیٹ_2->arr = تہی;
	    توڑو;
	}
    }
}

/*
 * Update highlight_attr[] array. If "update_ids" is true, then update
 * highlight_ids[] array instead.
 */
    ساکن خالی
سیٹ_highlight_attr(hl_override_T *arr, صحیح len, bool update_ids)
{
    برائے (صحیح i = 0; i < len; i++)
    {
	hl_override_T	*override = arr + i;
	صحیح		hlf = -1, attr;

	اگر (override->from <= 0)
	    // Directly map to highlight_attr
	    hlf = -override->from;
	ورنہ
	{
	    برائے (صحیح k = 0; k < HLF_گنتی; k++)
		اگر (override->from == highlight_ids[k])
		{
		    hlf = k;
		    توڑو;
		}

	    اگر (hlf == -1)
		جاری;
	}

	اگر (update_ids)
	{
	    اگر (override->مقصد <= 0)
	    {
		اگر (hlf == -override->مقصد)
		    highlight_ids[hlf] = -1;
		ورنہ
		    highlight_ids[hlf] = highlight_ids[-override->مقصد];
	    }
	    ورنہ
		highlight_ids[hlf] = override->مقصد;
	}
	ورنہ
	{
	    اگر (override->مقصد <= 0)
		// Directly use highlight_attr[]
		attr = highlight_attr[-override->مقصد];
	    ورنہ
		attr = syn_id2attr(override->مقصد);

	    highlight_attr[hlf] = attr;
	}
    }
}

/*
 * Set the current highlight override to "arr". Returns true if successful
 */
    bool
ڈالو_highlight_overrides(hl_override_T *arr, صحیح len)
{
    // Don't want to do anything if "arr" is already the current override. If
    // "arr" is NULL (but overrides->arr is not), then still push an override,
    // but "->arr" will just be NULL so any previous overrides are cleared.
    اگر (overrides != تہی && overrides->arr == arr)
	واپس false;

    hl_overrides_T *سیٹ_2;

    سیٹ_2 = ALLOC_ONE(hl_overrides_T);
    اگر (سیٹ_2 == تہی)
	واپس false;

    سیٹ_2->arr = arr;
    سیٹ_2->len = len;

    // Push to front of list
    سیٹ_2->اگلا = overrides;
    overrides = سیٹ_2;

    // Save current highlight_attr[], so it can be restored later after this
    // override is popped.
    حافظہ_نقل(سیٹ_2->attr, highlight_attr, حجم(highlight_attr));

    // Reset highlight_attr[] to default values, as we don't want changes from
    // other overrides.
    حافظہ_نقل(highlight_attr, highlight_attr_raw, حجم(highlight_attr));

    // Update highlight_attr[] array
    اگر (arr != تہی)
	سیٹ_highlight_attr(arr, len, false);

    واپس true;
}

/*
 * Pop the current highlight override (if any) from the stack. This should
 * always be paired with a *successful* push.
 */
    خالی
نکالو_highlight_overrides(خالی)
{
    hl_overrides_T *سیٹ_2 = overrides;

    اگر (overrides == تہی)
	واپس;

    overrides = سیٹ_2->اگلا;

    // Set highlight_attr[] to state before override was pushed.
    حافظہ_نقل(highlight_attr, سیٹ_2->attr, حجم(highlight_attr));
    vim_free(سیٹ_2);
}

/*
 * Parse the 'winhighlight' option and return array. Returns NULL on failure or
 * if empty option. If failure, then errmsg is set.
 */
    ساکن hl_override_T *
parse_winhighlight(char_u *opt, صحیح *len, حرف **errmsg)
{
    char_u	    *p = opt;
    hl_override_T   *arr;
    صحیح		    i = 0;
    صحیح		    عدد = 1;
    صحیح		    n_colons = 0;

    اگر (*p == NUL)
	واپس تہی;

    // Get number of overrides first so we can allocate array
    جب ((p = vim_strchr(p, ',')) != تہی)
    {
	p++;
	عدد++;
    }
    p = opt;
    // Check if number of ':' matches number of ','
    جب ((p = vim_strchr(p, ':')) != تہی)
    {
	p++;
	n_colons++;
    }

    اگر (عدد != n_colons)
    {
	*errmsg = e_invalid_دلیل;
	واپس تہی;
    }

    arr = ALLOC_MULT(hl_override_T, عدد);
    اگر (arr == تہی)
    {
	*errmsg = e_برآمد_کا_حافظہ;
	واپس تہی;
    }

    p = opt;

    جب (true)
    {
	hl_override_T *override = arr + i++;
	char_u	*fromname = p, *toname;
	char_u	*tmp;
	char_u	**نام[2] = {&fromname, &toname};
	صحیح	fromlen, tolen;
	صحیح	*lens[2] = {&fromlen, &tolen};
	صحیح	fromid, toid;
	صحیح	*ids[2] = {&fromid, &toid};

	p = vim_strchr(p, ':');

	اگر (p == تہی)
	    جاؤ fail;

	fromlen = p - fromname; // Get hl for "from"
	اگر (fromlen == 0)
	    جاؤ fail;
	p++; // Skip colon ':'
	اگر (*p == NUL)
	    جاؤ fail;

	toname = p;
	tmp = vim_strchr(p, ',');

	// Get hl for "to", must check for no trailing comma in case last
	// element.
	اگر (tmp == تہی)
	    tolen = (صحیح)STRLEN(p);
	ورنہ
	{
	    tolen = tmp - toname;
	    p = ++tmp;
	}
	اگر (tolen == 0)
	    جاؤ fail;

	برائے (صحیح k = 0; k < 2; k++)
	{
	    char_u  *name = *نام[k];
	    صحیح	    nlen = *lens[k];

	    اگر (*name == '!')
	    {
		// If starts with "!", then it maps directly to a 'highlight'
		// occasion.
		صحیح hlf;

		اگر (nlen != 2)
		    جاؤ fail;

		برائے (hlf = 0; hlf < (صحیح)HLF_گنتی; ++hlf)
		    اگر (hl_flags[hlf] == name[1])
			توڑو;

		اگر (hlf >= HLF_گنتی)
		    جاؤ fail;

		*ids[k] = -hlf;
	    }
	    ورنہ
	    {
		// Otherwise get the highlight group id.
		اگر (syn_check_گروہ(name, nlen) == 0)
		    جاؤ fail;

		*ids[k] = syn_namen2id(name, nlen);
		اگر (*ids[k] == 0)
		    جاؤ fail;

		// Always map "Normal" group to HLF_WIN
		اگر (ids[k] == &fromid &&
			STRCMP(HL_جدول()[*ids[k] - 1].sg_name_u, "NORMAL") == 0)
		    *ids[k] = -HLF_WIN;
	    }
	}

	// If fromid == toid, leave it be, this is so that we know HLF_WIN has
	// been set (even to itself e.g. "Normal"), and therefore know to use
	// its attr instead.
	override->from = fromid;
	override->مقصد = toid;

	اگر (tmp == تہی)
	    توڑو;
    }

    *len = عدد;
    واپس arr;
fail:
    vim_free(arr);
    *errmsg = e_invalid_دلیل;
    واپس تہی;
}

/*
 * Update w_hl for "wp" using given option value. Returns error
 * message on failure, otherwise NULL.
 */
    حرف *
update_winhighlight(win_T *wp, char_u *opt)
{
    حرف	    *err = تہی;
    hl_override_T   *arr;
    صحیح		    عدد = 1;

    arr = parse_winhighlight(opt, &عدد, &err);

    اگر (arr == تہی && err != تہی)
	واپس err;

    اگر (arr == تہی && wp->w_hl == تہی)
	واپس تہی;

    update_highlight_overrides(wp->w_hl, arr, عدد);

    vim_free(wp->w_hl);
    wp->w_hl = arr;
    wp->w_hl_len = عدد;

    wp->w_hlfwin_id = hlf_get_id(wp, HLF_WIN);

#اگر_تعریف_شدہ FEAT_TERMINAL
    رکن_update_hlfwin(wp);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TERMINAL
    // May be NULL (such as in after_copy_winopt())
    اگر (wp->w_بفر != تہی)
    {
	// Make sure terminal highlighting is updated
	bool ret = ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);

	اگر (wp->w_بفر->b_رکن != تہی)
	    رکن_init_default_colors(wp->w_بفر->b_رکن);
	اگر (ret)
	    نکالو_highlight_overrides();
    }
#اگر_ختم

    واپس تہی;
}

/*
 * Get id that the hlf attr is using in window "wp", otherwise zero. Note that
 * -1 may be returned if the hlf was set to itself.
 */
    صحیح
hlf_get_id(win_T *wp, صحیح hlf)
{
    صحیح id;
    ساکن صحیح پچھلا[HLF_گنتی];

    اگر (wp->w_hl == تہی)
	واپس highlight_ids[hlf];

    حافظہ_نقل(پچھلا, highlight_ids, حجم(highlight_ids));
    سیٹ_highlight_attr(wp->w_hl, wp->w_hl_len, true);
    id = highlight_ids[hlf];
    حافظہ_نقل(highlight_ids, پچھلا, حجم(highlight_ids));

    واپس id;
}

/*
 * Update 'winhighlight' using a 'wincolor' style option value. Returns error
 * message on failure otherwise NULL. Does not update the actual 'wincolor'
 * value, but does update the 'winhighlight' value.
 */
    حرف *
update_wincolor(win_T *wp, char_u *opt)
{
    char_u  *str = *opt == NUL ? "" : alloc(حجم("!(:") + STRLEN(opt));
    حرف    *errmsg;

    اگر (str == تہی)
	واپس e_برآمد_کا_حافظہ;

    اگر (*opt != NUL)
	sprintf((حرف *)str, "!(:%s", opt);

    errmsg = update_winhighlight(wp, str);

    اگر (errmsg == تہی)
	سیٹ_string_option_براہ_راست_اندر_win(wp, (char_u *)"winhighlight", -1,
		str, OPT_FREE|OPT_LOCAL, 0);

    اگر (*opt != NUL)
	آزاد(str);
    واپس errmsg;
}
