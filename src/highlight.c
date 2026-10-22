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

#ضمن "vim.h"

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
ساكن keyvalue_T highlight_tab[] = {
    KEYVALUE_اندراج(HL_BOLD, "bold"),			    // index 0
    KEYVALUE_اندراج(HL_INVERSE, "inverse"),		    // index 1
    KEYVALUE_اندراج(HL_ITALIC, "italic"),		    // index 2
    KEYVALUE_اندراج(HL_NOCOMBINE, "nocombine"),		    // index 3
    KEYVALUE_اندراج(HL_عام, "NONE"),			    // index 4
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
ساكن keyvalue_T *highlight_اشاریہ_tab[] = {
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

تعداد {
    BLACK = 0,
    DARKBLUE,
    DARKGREEN,
    DARKCYAN,
    DARKRED,
    DARKMAGENTA,
    BROWN,
    DARKYELLOW,
    سلیٹی,
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
ساكن keyvalue_T color_name_tab[] = {
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
    KEYVALUE_اندراج(سلیٹی, "Gray"),
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
تعريفنوع بنية
{
    char_u	*sg_name;	// highlight group name
    char_u	*sg_name_u;	// uppercase of sg_name
    صحيح		sg_cleared;	// "hi clear" was used
// for normal terminals
    صحيح		sg_رکن;	// "term=" highlighting attributes
    char_u	*sg_شروع;	// terminal string for start highl
    char_u	*sg_stop;	// terminal string for stop highl
    صحيح		sg_رکن_attr;	// Screen attr for term mode
// for color terminals
    صحيح		sg_cterm;	// "cterm=" highlighting attr
    صحيح		sg_cterm_bold;	// bold attr was set for light color
    صحيح		sg_cterm_fg;	// terminal fg color number + 1
    صحيح		sg_cterm_bg;	// terminal bg color number + 1
    صحيح		sg_cterm_ul;	// terminal ul color number + 1
    صحيح		sg_cterm_attr;	// Screen attr for color term mode
    صحيح		sg_cterm_font;	// terminal alternative font (0 for normal)
// for when using the GUI
#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    guicolor_T	sg_gui_fg;	// GUI foreground color handle
    guicolor_T	sg_gui_bg;	// GUI background color handle
    guicolor_T	sg_gui_sp;	// GUI special color handle
#جے_انت
#جے_تعریف_شدہ FEAT_GUI
    GuiFont	sg_font;	// GUI font handle
# ifdef FEAT_XFONTSET
    GuiFontset	sg_fontset;	// GUI fontset handle
# endif
    char_u	*sg_font_name;  // GUI font or fontset name
    صحيح		sg_gui_attr;    // Screen attr for GUI mode
#جے_انت
#إذا defined(FEAT_GUI) || defined(FEAT_EVAL)
// Store the sp color name for the GUI or synIDattr()
    صحيح		sg_gui;		// "gui=" highlighting attributes
    char_u	*sg_gui_fg_name;// GUI foreground color name
    char_u	*sg_gui_bg_name;// GUI background color name
    char_u	*sg_gui_sp_name;// GUI special color name
#جے_انت
    صحيح		sg_link;	// link to this highlight group ID
    صحيح		sg_deflink;	// default link; restored in highlight_clear()
    صحيح		sg_سیٹ;		// combination of SG_* flags
#جے_تعریف_شدہ FEAT_EVAL
    sctx_T	sg_deflink_sctx;  // script where the default link was set
    sctx_T	sg_script_ctx;	// script in which the group was last set
#جے_انت
} hl_گروہ_T;

تعريفنوع بنية hl_overrides_S hl_overrides_T;
بنية hl_overrides_S
{
    hl_override_T   *arr;   // May be NULL if "arr" was freed or no highlight
			    // overrides (all values set to default)
    صحيح		    len;
    hl_overrides_T  *اگلا;  // Used to handle recursive calls

    صحيح attr[HLF_گنتی];    // highlight_attr[] before being updated.
};

ساكن hl_overrides_T *overrides = عدم;

// Synced with highlight_attr, each is the highlight group id for each attr. If
// an element is 0, then there is no attr for it. If an element is -1, then it
// means it was set to itself.
ساكن صحيح highlight_ids[HLF_گنتی];

// Same as highlight_attr[] but is not modified by highlight group overrides
ساكن صحيح highlight_attr_raw[HLF_گنتی];

ساكن صحيح hl_flags[HLF_گنتی] = HL_FLAGS;

// highlight groups for 'highlight' option
ساكن garray_T highlight_ga;
#تعریف_کرو HL_جدول()	((hl_گروہ_T *)((highlight_ga.ga_انکڑے)))

// Wrapper struct for hashtable entries: stores the highlight group ID alongside
// the uppercase name used as a hash key.  Uses the same offsetof pattern as
// signgroup_T / HI2SG.
تعريفنوع بنية {
    صحيح		hn_id;		// highlight group ID (1-based)
    char_u	hn_کلید[1];	// uppercase name (stretchable array)
} hlname_T;

#تعریف_کرو HLNAME_کلید_OFF	offsetof(hlname_T, hn_کلید)
#تعریف_کرو HI2HLNAME(hi)	((hlname_T *)((hi)->hi_کلید - HLNAME_کلید_OFF))

// hashtable for quick highlight group name lookup
ساكن hashtab_T highlight_ht;
ساكن bool	 highlight_ht_inited = false;

/*
 * An attribute number is the index in attr_table plus ATTR_OFF.
 */
#تعریف_کرو ATTR_OFF (HL_ALL + 1)

ساكن فراغ syn_unadd_گروہ(فراغ);
ساكن فراغ سیٹ_hl_attr(صحيح idx);
ساكن فراغ highlight_list_one(صحيح id);
ساكن صحيح highlight_list_arg(صحيح id, صحيح didh, صحيح type, صحيح iarg, char_u *sarg, حرف *name);
ساكن صحيح syn_جوڑو_گروہ(char_u *name);
ساكن صحيح syn_name2id_len(char_u *name, صحيح len);
ساكن صحيح hl_has_settings(صحيح idx, صحيح check_link);
ساكن فراغ highlight_clear(صحيح idx);

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
ساكن فراغ gui_do_one_color(صحيح idx, صحيح do_menu, صحيح do_tooltip);
#جے_انت
#جے_تعریف_شدہ FEAT_GUI
ساكن صحيح  سیٹ_گروہ_colors(char_u *name, guicolor_T *fgp, guicolor_T *bgp, صحيح do_menu, صحيح use_norm, صحيح do_tooltip);
ساكن فراغ hl_do_font(صحيح idx, char_u *arg, صحيح do_عام, صحيح do_menu, صحيح do_tooltip, صحيح free_font);
#جے_انت

/*
 * The default highlight groups.  These are compiled-in for fast startup and
 * they still work when the runtime files can't be found.
 * When making changes here, also change runtime/colors/default.vim!
 * The #ifdefs are needed to reduce the amount of static data.  Helps to make
 * the 16 bit DOS (museum) version compile.
 */
#إذا defined(FEAT_GUI) || defined(FEAT_EVAL)
# define CENT(a, b) b
#وإلا
# define CENT(a, b) a
#جے_انت
ساكن حرف *(highlight_init_both[]) = {
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
#جے_تعریف_شدہ FEAT_CLIPBOARD
    CENT("VisualNOS term=underline,bold cterm=underline,bold",
	 "VisualNOS term=underline,bold cterm=underline,bold gui=underline,bold"),
#جے_انت
#جے_تعریف_شدہ FEAT_DIFF
    CENT("DiffText term=reverse cterm=bold ctermbg=Red ctermfg=Black",
	 "DiffText term=reverse cterm=bold ctermbg=Red ctermfg=Black gui=bold guibg=Red"),
    "default link DiffTextAdd DiffText",
#جے_انت
    CENT("PmenuSbar term=NONE ctermbg=Grey",
	 "PmenuSbar term=NONE ctermbg=Grey guibg=Grey"),
    CENT("TabLineSel term=bold cterm=bold",
	 "TabLineSel term=bold cterm=bold gui=bold"),
    CENT("TabLineFill term=reverse cterm=reverse",
	 "TabLineFill term=reverse cterm=reverse gui=reverse"),
    "default link TabPanel TabLine",
    "default link TabPanelSel TabLineSel",
    "default link TabPanelFill TabLineFill",
#جے_تعریف_شدہ FEAT_GUI
    "Cursor guibg=fg guifg=bg",
    "lCursor guibg=fg guifg=bg", // should be different, but what?
#جے_انت
#جے_تعریف_شدہ FEAT_GUI_MSWIN
    "TitleBar guibg=bg guifg=fg",
    "TitleBarNC guibg=NONE guifg=NONE",
#جے_انت
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
    عدم
};

// Default colors only used with a light background.
ساكن حرف *(highlight_init_light[]) = {
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
#جے_تعریف_شدہ FEAT_SPELL
    CENT("SpellBad term=reverse ctermbg=LightRed",
	 "SpellBad term=reverse ctermbg=LightRed guisp=Red gui=undercurl"),
    CENT("SpellCap term=reverse ctermbg=LightBlue",
	 "SpellCap term=reverse ctermbg=LightBlue guisp=Blue gui=undercurl"),
    CENT("SpellRare term=reverse ctermbg=LightMagenta",
	 "SpellRare term=reverse ctermbg=LightMagenta guisp=Magenta gui=undercurl"),
    CENT("SpellLocal term=underline ctermbg=Cyan",
	 "SpellLocal term=underline ctermbg=Cyan guisp=DarkCyan gui=undercurl"),
#جے_انت
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
#جے_تعریف_شدہ FEAT_FOLDING
    CENT("Folded term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "Folded term=standout ctermbg=Grey ctermfg=DarkBlue guibg=LightGrey guifg=DarkBlue"),
    CENT("FoldColumn term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "FoldColumn term=standout ctermbg=Grey ctermfg=DarkBlue guibg=Grey guifg=DarkBlue"),
#جے_انت
#جے_تعریف_شدہ FEAT_SIGNS
    CENT("SignColumn term=standout ctermbg=Grey ctermfg=DarkBlue",
	 "SignColumn term=standout ctermbg=Grey ctermfg=DarkBlue guibg=Grey guifg=DarkBlue"),
#جے_انت
    CENT("Visual ctermbg=Grey ctermfg=Black",
	 "Visual ctermbg=Grey ctermfg=Black guibg=LightGrey guifg=Black"),
#جے_تعریف_شدہ FEAT_DIFF
    CENT("DiffAdd term=bold ctermbg=LightBlue ctermfg=Black",
	 "DiffAdd term=bold ctermbg=LightBlue ctermfg=Black guibg=LightBlue"),
    CENT("DiffChange term=bold ctermbg=LightMagenta ctermfg=Black",
	 "DiffChange term=bold ctermbg=LightMagenta ctermfg=Black guibg=LightMagenta"),
    CENT("DiffDelete term=bold ctermfg=Blue ctermbg=LightCyan",
	 "DiffDelete term=bold ctermfg=Blue ctermbg=LightCyan gui=bold guifg=Blue guibg=LightCyan"),
#جے_انت
    CENT("TabLine term=underline cterm=underline ctermfg=black ctermbg=LightGrey",
	 "TabLine term=underline cterm=underline ctermfg=black ctermbg=LightGrey gui=underline guibg=LightGrey"),
#جے_تعریف_شدہ FEAT_SYN_HL
    CENT("CursorColumn term=reverse ctermbg=LightGrey",
	 "CursorColumn term=reverse ctermbg=LightGrey guibg=Grey90"),
    CENT("CursorLine term=underline cterm=underline",
	 "CursorLine term=underline cterm=underline guibg=Grey90"),
    CENT("ColorColumn term=reverse ctermbg=LightRed",
	 "ColorColumn term=reverse ctermbg=LightRed guibg=LightRed"),
#جے_انت
#جے_تعریف_شدہ FEAT_CONCEAL
    CENT("Conceal ctermbg=DarkGrey ctermfg=LightGrey",
	 "Conceal ctermbg=DarkGrey ctermfg=LightGrey guibg=DarkGrey guifg=LightGrey"),
#جے_انت
    CENT("MatchParen term=reverse ctermbg=Cyan",
	 "MatchParen term=reverse ctermbg=Cyan guibg=Cyan"),
#جے_تعریف_شدہ FEAT_TERMINAL
    CENT("StatusLineTerm term=reverse,bold cterm=bold ctermfg=White ctermbg=DarkGreen",
	 "StatusLineTerm term=reverse,bold cterm=bold ctermfg=White ctermbg=DarkGreen gui=bold guifg=bg guibg=DarkGreen"),
    CENT("StatusLineTermNC term=reverse ctermfg=White ctermbg=DarkGreen",
	 "StatusLineTermNC term=reverse ctermfg=White ctermbg=DarkGreen guifg=bg guibg=DarkGreen"),
#جے_انت
#جے_تعریف_شدہ FEAT_MENU
    CENT("ToolbarLine term=underline ctermbg=LightGrey",
	 "ToolbarLine term=underline ctermbg=LightGrey guibg=LightGrey"),
    CENT("ToolbarButton cterm=bold ctermfg=White ctermbg=DarkGrey",
	 "ToolbarButton cterm=bold ctermfg=White ctermbg=DarkGrey gui=bold guifg=White guibg=Grey40"),
#جے_انت
    عدم
};

// Default colors only used with a dark background.
ساكن حرف *(highlight_init_dark[]) = {
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
#جے_تعریف_شدہ FEAT_SPELL
    CENT("SpellBad term=reverse ctermbg=Red",
	 "SpellBad term=reverse ctermbg=Red guisp=Red gui=undercurl"),
    CENT("SpellCap term=reverse ctermbg=Blue",
	 "SpellCap term=reverse ctermbg=Blue guisp=Blue gui=undercurl"),
    CENT("SpellRare term=reverse ctermbg=Magenta",
	 "SpellRare term=reverse ctermbg=Magenta guisp=Magenta gui=undercurl"),
    CENT("SpellLocal term=underline ctermbg=Cyan",
	 "SpellLocal term=underline ctermbg=Cyan guisp=Cyan gui=undercurl"),
#جے_انت
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
#جے_تعریف_شدہ FEAT_FOLDING
    CENT("Folded term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "Folded term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=DarkGrey guifg=Cyan"),
    CENT("FoldColumn term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "FoldColumn term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=Grey guifg=Cyan"),
#جے_انت
#جے_تعریف_شدہ FEAT_SIGNS
    CENT("SignColumn term=standout ctermbg=DarkGrey ctermfg=Cyan",
	 "SignColumn term=standout ctermbg=DarkGrey ctermfg=Cyan guibg=Grey guifg=Cyan"),
#جے_انت
    CENT("Visual ctermbg=Grey ctermfg=Black",
	 "Visual ctermbg=Grey ctermfg=Black guibg=#575757 guifg=LightGrey"),
#جے_تعریف_شدہ FEAT_DIFF
    CENT("DiffAdd term=bold ctermbg=DarkBlue",
	 "DiffAdd term=bold ctermbg=DarkBlue guibg=DarkBlue"),
    CENT("DiffChange term=bold ctermbg=DarkMagenta",
	 "DiffChange term=bold ctermbg=DarkMagenta guibg=DarkMagenta"),
    CENT("DiffDelete term=bold ctermfg=Blue ctermbg=DarkCyan",
	 "DiffDelete term=bold ctermfg=Blue ctermbg=DarkCyan gui=bold guifg=Blue guibg=DarkCyan"),
#جے_انت
    CENT("TabLine term=underline cterm=underline ctermfg=white ctermbg=DarkGrey",
	 "TabLine term=underline cterm=underline ctermfg=white ctermbg=DarkGrey gui=underline guibg=DarkGrey"),
#جے_تعریف_شدہ FEAT_SYN_HL
    CENT("CursorColumn term=reverse ctermbg=DarkGrey",
	 "CursorColumn term=reverse ctermbg=DarkGrey guibg=Grey40"),
    CENT("CursorLine term=underline cterm=underline",
	 "CursorLine term=underline cterm=underline guibg=Grey40"),
    CENT("ColorColumn term=reverse ctermbg=DarkRed",
	 "ColorColumn term=reverse ctermbg=DarkRed guibg=DarkRed"),
#جے_انت
    CENT("MatchParen term=reverse ctermbg=DarkCyan",
	 "MatchParen term=reverse ctermbg=DarkCyan guibg=DarkCyan"),
#جے_تعریف_شدہ FEAT_CONCEAL
    CENT("Conceal ctermbg=DarkGrey ctermfg=LightGrey",
	 "Conceal ctermbg=DarkGrey ctermfg=LightGrey guibg=DarkGrey guifg=LightGrey"),
#جے_انت
#جے_تعریف_شدہ FEAT_TERMINAL
    CENT("StatusLineTerm term=reverse,bold cterm=bold ctermfg=Black ctermbg=LightGreen",
	 "StatusLineTerm term=reverse,bold cterm=bold ctermfg=Black ctermbg=LightGreen gui=bold guifg=bg guibg=LightGreen"),
    CENT("StatusLineTermNC term=reverse ctermfg=Black ctermbg=LightGreen",
	 "StatusLineTermNC term=reverse ctermfg=Black ctermbg=LightGreen guifg=bg guibg=LightGreen"),
#جے_انت
#جے_تعریف_شدہ FEAT_MENU
    CENT("ToolbarLine term=underline ctermbg=DarkGrey",
	 "ToolbarLine term=underline ctermbg=DarkGrey guibg=Grey50"),
    CENT("ToolbarButton cterm=bold ctermfg=Black ctermbg=LightGrey",
	 "ToolbarButton cterm=bold ctermfg=Black ctermbg=LightGrey gui=bold guifg=Black guibg=LightGrey"),
#جے_انت
    عدم
};

#إذا defined(FEAT_SYN_HL)
/*
 * Returns the number of highlight groups.
 */
    صحيح
highlight_عدد_groups(فراغ)
{
    أرجع highlight_ga.ga_len;
}

/*
 * Returns the name of a highlight group.
 */
    char_u *
highlight_گروہ_name(صحيح id)
{
    أرجع HL_جدول()[id].sg_name;
}

/*
 * Returns the ID of the link to a highlight group.
 */
    صحيح
highlight_link_id(صحيح id)
{
    أرجع HL_جدول()[id].sg_link;
}
#جے_انت

ساكن فراغ resolve_fallback_fg_منزل_rgb(فراغ);
ساكن فراغ resolve_fallback_bg_منزل_rgb(فراغ);

    فراغ
init_highlight(
    صحيح		both,	    // include groups where 'bg' doesn't matter
    صحيح		reset)	    // clear group first
{
    صحيح		i;
    حرف	**pp;
    ساكن صحيح	had_both = FALSE;
#جے_تعریف_شدہ FEAT_EVAL
    char_u	*p;

    // Try finding the color scheme file.  Used when a color file was loaded
    // and 'background' or 't_Co' is changed.
    p = get_var_قدر((char_u *)"g:colors_name");
    إذا (p != عدم)
    {
	// The value of g:colors_name could be freed when sourcing the script,
	// making "p" invalid, so copy it.
	char_u *copy_p = vim_strsave(p);
	صحيح    r;

	إذا (copy_p != عدم)
	{
	    r = لدو_colors(copy_p);
	    vim_free(copy_p);
	    إذا (r == OK)
		أرجع;
	}
    }

#جے_انت

    // Didn't use a color file, use the compiled-in colors.
    إذا (both)
    {
	had_both = TRUE;
	pp = highlight_init_both;
	لكل (i = 0; pp[i] != عدم; ++i)
	    do_highlight((char_u *)pp[i], reset, TRUE);
    }
    وإلا إذا (!had_both)
	// Don't do anything before the call with both == TRUE from main().
	// Not everything has been setup then, and that call will overrule
	// everything anyway.
	أرجع;

    إذا (*p_bg == 'l')
	pp = highlight_init_light;
    وإلا
	pp = highlight_init_dark;
    لكل (i = 0; pp[i] != عدم; ++i)
	do_highlight((char_u *)pp[i], reset, TRUE);

    // Reverse looks ugly, but grey may not work for less than 8 colors.  Thus
    // let it depend on the number of colors available.
    إذا (t_colors < 8)
	do_highlight((char_u *)"Visual term=reverse cterm=reverse ctermbg=NONE ctermfg=NONE",
		FALSE, TRUE);
    // With 8 colors brown is equal to yellow, need to use black for Search fg
    // to avoid Statement highlighted text disappears.
    // Clear the attributes, needed when changing the t_Co value.
    إذا (t_colors <= 8)
    {
	إذا (*p_bg == 'l')
	    do_highlight((char_u *)"Search ctermfg=black", FALSE, TRUE);
    }

#جے_تعریف_شدہ FEAT_SYN_HL
    // If syntax highlighting is enabled load the highlighting for it.
    إذا (get_var_قدر((char_u *)"g:syntax_on") != عدم)
    {
	ساكن صحيح	recursive = 0;

	إذا (recursive >= 5)
	    emsg(_(e_recursive_loop_loading_syncolor_vim));
	وإلا
	{
	    ++recursive;
	    (فراغ)سروت_runtime((char_u *)"syntax/syncolor.vim", DIP_ALL);
	    --recursive;
	}
    }
#جے_انت
    resolve_fallback_fg_منزل_rgb();
    resolve_fallback_bg_منزل_rgb();
}

#إذا defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
/*
 * Load a default color list. Intended to support legacy color names but allows
 * the user to override the color values. Only loaded once.
 */
    ساكن فراغ
لدو_default_colors_lists(فراغ)
{
    // Lacking a default color list isn't the end of the world but it is likely
    // an inconvenience so users should know when it is missing.
    إذا (سروت_runtime((char_u *)"colors/lists/default.vim", DIP_ALL) != OK)
	پیغام("failed to load colors/lists/default.vim");
}
#جے_انت

/*
 * Load color file "name".
 * Return OK for success, FAIL for failure.
 */
    صحيح
لدو_colors(char_u *name)
{
    char_u	*buf;
    صحيح		retval = FAIL;
    ساكن صحيح	recursive = FALSE;

    // When being called recursively, this is probably because setting
    // 'background' caused the highlighting to be reloaded.  This means it is
    // working, thus we should return OK.
    إذا (recursive)
	أرجع OK;

    recursive = TRUE;
    buf = alloc(STRLEN(name) + 12);
    إذا (buf != عدم)
    {
#إذا defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	لدو_default_colors_lists();
#جے_انت
	apply_autocmds(EVENT_COLORSCHEMEPRE, name,
					       curbuf->b_fname, FALSE, curbuf);
	sprintf((حرف *)buf, "colors/%s.vim", name);
	retval = سروت_runtime(buf, DIP_شروع + DIP_OPT);
	vim_free(buf);
	إذا (retval == OK)
	    apply_autocmds(EVENT_COLORSCHEME, name, curbuf->b_fname,
								FALSE, curbuf);
    }
    recursive = FALSE;

    أرجع retval;
}

ساكن صحيح color_numbers_16[28] = {0, 1, 2, 3,
				 4, 5, 6, 6,
				 7, 7, 7, 7,
				 8, 8,
				 9, 9, 10, 10,
				 11, 11, 12, 12, 13,
				 13, 14, 14, 15, -1};
// for xterm with 88 colors...
ساكن صحيح color_numbers_88[28] = {0, 4, 2, 6,
				 1, 5, 32, 72,
				 84, 84, 7, 7,
				 82, 82,
				 12, 43, 10, 61,
				 14, 63, 9, 74, 13,
				 75, 11, 78, 15, -1};
// for xterm with 256 colors...
ساكن صحيح color_numbers_256[28] = {0, 4, 2, 6,
				 1, 5, 130, 3,
				 248, 248, 7, 7,
				 242, 242,
				 12, 81, 10, 121,
				 14, 159, 9, 224, 13,
				 225, 11, 229, 15, -1};
// for terminals with less than 16 colors...
ساكن صحيح color_numbers_8[28] = {0, 4, 2, 6,
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
    ساكن صحيح
lookup_color(صحيح idx, صحيح foreground, صحيح *boldp)
{
    صحيح		color = color_numbers_16[idx];
    char_u	*p;

    // Use the _16 table to check if it's a valid color name.
    إذا (color < 0)
	أرجع -1;

    إذا (t_colors == 8)
    {
	// t_Co is 8: use the 8 colors table
#إذا defined(__QNXNTO__)
	// On qnx, the 8 & 16 color arrays are the same
	إذا (STRNCMP(T_NAME, "qansi", 5) == 0)
	    color = color_numbers_16[idx];
	وإلا
#جے_انت
	    color = color_numbers_8[idx];
	إذا (foreground)
	{
	    // set/reset bold attribute to get light foreground
	    // colors (on some terminals, e.g. "linux")
	    إذا (color & 8)
		*boldp = TRUE;
	    وإلا
		*boldp = FALSE;
	}
	color &= 7;	// truncate to 8 colors
    }
    وإلا إذا (t_colors == 16 || t_colors == 88
					   || t_colors >= 256)
    {
	// Guess: if the termcap entry ends in 'm', it is
	// probably an xterm-like terminal.  Use the changed
	// order for colors.
	إذا (*T_CAF != NUL)
	    p = T_CAF;
	وإلا
	    p = T_CSF;
	إذا (*p != NUL && (t_colors > 256
			      || *(p + STRLEN(p) - 1) == 'm'))
	{
	    إذا (t_colors == 88)
		color = color_numbers_88[idx];
	    وإلا إذا (t_colors >= 256)
		color = color_numbers_256[idx];
	    وإلا
		color = color_numbers_8[idx];
	}
#جے_تعریف_شدہ FEAT_TERMRESPONSE
	إذا (t_colors >= 256 && color == 15 && is_mac_terminal)
	    // Terminal.app has a bug: 15 is light grey. Use white
	    // from the color cube instead.
	    color = 231;
#جے_انت
    }
    أرجع color;
}

/*
 * Link highlight group 'from_hg' to 'to_hg'.
 * 'dodefault' is set to TRUE for ":highlight default link".
 * 'forceit' is set to TRUE for ":highlight! link"
 * 'init' is set to TRUE when initializing all the highlight groups.
 */
    ساكن فراغ
highlight_گروہ_link(
	char_u	*from_hg,
	صحيح	from_len,
	char_u	*منزل_hg,
	صحيح	منزل_len,
	صحيح	dodefault,
	صحيح	forceit,
	صحيح	init)
{
    صحيح		from_id;
    صحيح		منزل_id;
    hl_گروہ_T	*hlgroup = عدم;

    from_id = syn_check_گروہ(from_hg, from_len);
    إذا (STRNCMP(منزل_hg, "NONE", 4) == 0)
	منزل_id = 0;
    وإلا
	منزل_id = syn_check_گروہ(منزل_hg, منزل_len);

    إذا (from_id > 0)
    {
	hlgroup = &HL_جدول()[from_id - 1];
	إذا (dodefault && (forceit || hlgroup->sg_deflink == 0))
	{
	    hlgroup->sg_deflink = منزل_id;
#جے_تعریف_شدہ FEAT_EVAL
	    hlgroup->sg_deflink_sctx = current_sctx;
	    hlgroup->sg_deflink_sctx.sc_lnum += SOURCING_LNUM;
#جے_انت
	}
    }

    إذا (from_id > 0 && (!init || hlgroup->sg_سیٹ == 0))
    {
	// Don't allow a link when there already is some highlighting
	// for the group, unless '!' is used
	إذا (منزل_id > 0 && !forceit && !init
		&& hl_has_settings(from_id - 1, dodefault))
	{
	    إذا (SOURCING_NAME == عدم && !dodefault)
		emsg(_(e_گروہ_has_settings_highlight_link_ignored));
	}
	وإلا إذا (hlgroup->sg_link != منزل_id
#جے_تعریف_شدہ FEAT_EVAL
		|| hlgroup->sg_script_ctx.sc_sid != current_sctx.sc_sid
#جے_انت
		|| hlgroup->sg_cleared)
	{
	    إذا (!init)
		hlgroup->sg_سیٹ |= SG_LINK;
	    hlgroup->sg_link = منزل_id;
#جے_تعریف_شدہ FEAT_EVAL
	    hlgroup->sg_script_ctx = current_sctx;
	    hlgroup->sg_script_ctx.sc_lnum += SOURCING_LNUM;
#جے_انت
	    hlgroup->sg_cleared = FALSE;
	    redraw_all_later(UPD_SOME_VALID);

	    // Only call highlight_changed() once after multiple changes.
	    need_highlight_بدلیا = TRUE;
	}
    }

}

/*
 * Reset all highlighting to the defaults. Removes all highlighting for the
 * groups added by the user.
 */
    ساكن فراغ
highlight_reset_all(فراغ)
{
    صحيح		idx;

#جے_تعریف_شدہ FEAT_GUI
    // First, we do not destroy the old values, but allocate the new
    // ones and update the display. THEN we destroy the old values.
    // If we destroy the old values first, then the old values
    // (such as GuiFont's or GuiFontset's) will still be displayed but
    // invalid because they were free'd.
    إذا (gui.وچ_use)
    {
# ifdef FEAT_BEVAL_TIP
	gui_init_tooltip_font();
# endif
# إذا defined(FEAT_MENU) && defined(FEAT_GUI_MOTIF)
	gui_init_menu_font();
# endif
    }
# إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_X11)
    gui_mch_def_colors();
# endif
# ifdef FEAT_GUI_MSWIN
    إذا (gui.وچ_use)
	gui_mch_سیٹ_titlebar_colors();
# endif
# ifdef FEAT_GUI_X11
#  ifdef FEAT_MENU

    // This only needs to be done when there is no Menu highlight
    // group defined by default, which IS currently the case.
    gui_mch_new_menu_colors();
#  endif
    إذا (gui.وچ_use)
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
#جے_انت
#جے_تعریف_شدہ FEAT_EVAL
    do_unlet((char_u *)"g:colors_name", TRUE);
#جے_انت
    restore_cterm_colors();

    // Clear all default highlight groups and load the defaults.
    لكل (idx = 0; idx < highlight_ga.ga_len; ++idx)
	highlight_clear(idx);
    init_highlight(TRUE, TRUE);
#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (USE_24_ثنائی_ہندسہ)
	highlight_gui_started();
    وإلا
#جے_انت
	highlight_بدلیا();
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
    ساكن صحيح
highlight_سیٹ_termgui_attr(صحيح idx, char_u *کلید, char_u *arg, صحيح init)
{
    صحيح		attr;
    size_t	off;
    keyvalue_T	نشانہ;
    keyvalue_T	*اندراج;

    attr = 0;
    off = 0;
    نشانہ.کلید = 0;
    نشانہ.قدر.لمائی = 0;	// not used, see cmp_keyvalue_value_ni()
    طالما (arg[off] != NUL)
    {
	نشانہ.قدر.string = arg + off;
	اندراج = (keyvalue_T *)bsearch(&نشانہ, &highlight_tab,
		ARRAY_لمائی(highlight_tab), حجم(highlight_tab[0]),
		cmp_keyvalue_قدر_ni);
	إذا (اندراج == عدم)
	{
	    semsg(_(e_illegal_قدر_str), arg);
	    أرجع FALSE;
	}

	attr |= اندراج->کلید;
	off += اندراج->قدر.لمائی;
	إذا (arg[off] == ',')		// another one follows
	    ++off;
    }
    إذا (*کلید == 'T')
    {
	إذا (!init || !(HL_جدول()[idx].sg_سیٹ & SG_رکن))
	{
	    إذا (!init)
		HL_جدول()[idx].sg_سیٹ |= SG_رکن;
	    HL_جدول()[idx].sg_رکن = attr;
	}
    }
    وإلا إذا (*کلید == 'C')
    {
	إذا (!init || !(HL_جدول()[idx].sg_سیٹ & SG_CTERM))
	{
	    إذا (!init)
		HL_جدول()[idx].sg_سیٹ |= SG_CTERM;
	    HL_جدول()[idx].sg_cterm = attr;
	    HL_جدول()[idx].sg_cterm_bold = FALSE;
	}
    }
#إذا defined(FEAT_GUI) || defined(FEAT_EVAL)
    وإلا
    {
	إذا (!init || !(HL_جدول()[idx].sg_سیٹ & SG_GUI))
	{
	    إذا (!init)
		HL_جدول()[idx].sg_سیٹ |= SG_GUI;
	    HL_جدول()[idx].sg_gui = attr;
	}
    }
#جے_انت

    أرجع TRUE;
}

#جے_تعریف_شدہ FEAT_GUI
/*
 * Set the font for the highlight group at 'idx'.
 * 'arg' is the font name.
 * Returns TRUE if the font is changed.
 */
    ساكن صحيح
highlight_سیٹ_font(
	صحيح	idx,
	char_u	*arg,
	صحيح	is_عام_گروہ,
	صحيح	is_menu_گروہ,
	صحيح	is_tooltip_گروہ)
{
    صحيح		did_change = FALSE;

    // in non-GUI fonts are simply ignored
    إذا (HL_جدول()[idx].sg_font_name != عدم
	    && STRCMP(HL_جدول()[idx].sg_font_name, arg) == 0)
    {
	// Font name didn't change, ignore.
    }
    وإلا إذا (!gui.shell_created)
    {
	// GUI not started yet, always accept the name.
	vim_free(HL_جدول()[idx].sg_font_name);
	HL_جدول()[idx].sg_font_name = vim_strsave(arg);
	did_change = TRUE;
    }
    وإلا
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
	hl_do_font(idx, arg, is_عام_گروہ, is_menu_گروہ,
		is_tooltip_گروہ, FALSE);

# ifdef FEAT_XFONTSET
	إذا (HL_جدول()[idx].sg_fontset != NOFONTSET)
	{
	    // New fontset was accepted. Free the old one, if there
	    // was one.
	    gui_mch_free_fontset(عارضی_sg_fontset);
	    did_change = TRUE;
	}
	وإلا
	    HL_جدول()[idx].sg_fontset = عارضی_sg_fontset;
# endif
	إذا (HL_جدول()[idx].sg_font != NOFONT)
	{
	    // New font was accepted. Free the old one, if there was
	    // one.
	    gui_mch_free_font(عارضی_sg_font);
	    did_change = TRUE;
	}
	وإلا
	    HL_جدول()[idx].sg_font = عارضی_sg_font;
	// Update sg_font_name once here, after both font and fontset results
	// are known.
	إذا (did_change)
	{
	    vim_free(HL_جدول()[idx].sg_font_name);
	    HL_جدول()[idx].sg_font_name = vim_strsave(arg);
	}
    }

    أرجع did_change;
}
#جے_انت

/*
 * Set the cterm foreground color for the Normal highlight group to "color" and
 * the bold attribute to "bold".
 */
    ساكن فراغ
hl_سیٹ_ctermfg_عام_گروہ(صحيح color, صحيح bold)
{
    cterm_عام_fg_color = color + 1;
    cterm_عام_fg_bold = bold;
#جے_تعریف_شدہ FEAT_GUI
    // Don't do this if the GUI is used.
    إذا (!gui.وچ_use && !gui.starting)
#جے_انت
    {
	سیٹ_must_redraw(UPD_CLEAR);
	إذا (termcap_سرگرم && color >= 0)
	    رکن_fg_color(color);
    }
}

/*
 * Set the cterm foreground color for the highlight group at 'idx' to 'color'.
 */
    ساكن فراغ
highlight_سیٹ_ctermfg(صحيح idx, صحيح color, صحيح is_عام_گروہ)
{
    HL_جدول()[idx].sg_cterm_fg = color + 1;
    إذا (is_عام_گروہ)
	hl_سیٹ_ctermfg_عام_گروہ(color,
					(HL_جدول()[idx].sg_cterm & HL_BOLD));
}

/*
 * Set the cterm background color for the Normal highlight group to "color".
 */
    ساكن فراغ
hl_سیٹ_ctermbg_عام_گروہ(صحيح color)
{
    cterm_عام_bg_color = color + 1;
#جے_تعریف_شدہ FEAT_GUI
    // Don't mess with 'background' if the GUI is used.
    إذا (!gui.وچ_use && !gui.starting)
#جے_انت
    {
	سیٹ_must_redraw(UPD_CLEAR);
	إذا (color >= 0)
	{
	    صحيح dark = -1;

	    إذا (termcap_سرگرم)
		رکن_bg_color(color);
	    إذا (t_colors < 16)
		dark = (color == 0 || color == 4);
	    // Limit the heuristic to the standard 16 colors
	    وإلا إذا (color < 16)
		dark = (color < 7 || color == 8);
	    // Set the 'background' option if the value is
	    // wrong.
	    إذا (dark != -1
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
    ساكن فراغ
highlight_سیٹ_ctermbg(صحيح idx, صحيح color, صحيح is_عام_گروہ)
{
    HL_جدول()[idx].sg_cterm_bg = color + 1;
    إذا (is_عام_گروہ)
	hl_سیٹ_ctermbg_عام_گروہ(color);
}

/*
 * Set the cterm underline color for the Normal highlight group to "color".
 */
    ساكن فراغ
hl_سیٹ_ctermul_عام_گروہ(صحيح color)
{
    cterm_عام_ul_color = color + 1;
#جے_تعریف_شدہ FEAT_GUI
    // Don't do this if the GUI is used.
    إذا (!gui.وچ_use && !gui.starting)
#جے_انت
    {
	سیٹ_must_redraw(UPD_CLEAR);
	إذا (termcap_سرگرم && color >= 0)
	    رکن_ul_color(color);
    }
}

/*
 * Set the cterm underline color for the highlight group at 'idx' to 'color'.
 */
    ساكن فراغ
highlight_سیٹ_ctermul(صحيح idx, صحيح color, صحيح is_عام_گروہ)
{
    HL_جدول()[idx].sg_cterm_ul = color + 1;
    إذا (is_عام_گروہ)
	hl_سیٹ_ctermul_عام_گروہ(color);
}

/*
 * Set the cterm font for the highlight group at 'idx'.
 * 'arg' is the color name or the numeric value as a string.
 * 'init' is set to TRUE when initializing highlighting.
 * Called for the ":highlight" command and the "hlset()" function.
 *
 * Returns TRUE if the font is set.
 */
    ساكن صحيح
highlight_سیٹ_cterm_font(
	صحيح	idx,
	char_u	*arg,
	صحيح	init)
{
    صحيح		font;

    إذا (init && (HL_جدول()[idx].sg_سیٹ & SG_CTERM))
	أرجع FALSE;

    إذا (!init)
	HL_جدول()[idx].sg_سیٹ |= SG_CTERM;

    إذا (VIM_ISDIGIT(*arg))
	font = atoi((حرف *)arg);
    وإلا إذا (STRICMP(arg, "NONE") == 0)
	font = -1;
    وإلا
	أرجع FALSE;

    HL_جدول()[idx].sg_cterm_font = font + 1;
    أرجع TRUE;
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
    ساكن صحيح
highlight_سیٹ_cterm_color(
	صحيح	idx,
	char_u	*کلید,
	char_u	*کلید_شروع,
	char_u	*arg,
	صحيح	is_عام_گروہ,
	صحيح	init)
{
    صحيح		color;

    إذا (init && (HL_جدول()[idx].sg_سیٹ & SG_CTERM))
	أرجع FALSE;

    إذا (!init)
	HL_جدول()[idx].sg_سیٹ |= SG_CTERM;

    // When setting the foreground color, and previously the "bold"
    // flag was set for a light color, reset it now
    إذا (کلید[5] == 'F' && HL_جدول()[idx].sg_cterm_bold)
    {
	HL_جدول()[idx].sg_cterm &= ~HL_BOLD;
	HL_جدول()[idx].sg_cterm_bold = FALSE;
    }

    إذا (VIM_ISDIGIT(*arg))
	color = atoi((حرف *)arg);
    وإلا إذا (STRICMP(arg, "fg") == 0)
    {
	إذا (cterm_عام_fg_color)
	    color = cterm_عام_fg_color - 1;
	وإلا
	{
	    emsg(_(e_fg_color_unknown));
	    أرجع FALSE;
	}
    }
    وإلا إذا (STRICMP(arg, "bg") == 0)
    {
	إذا (cterm_عام_bg_color > 0)
	    color = cterm_عام_bg_color - 1;
	وإلا
	{
	    emsg(_(e_bg_color_unknown));
	    أرجع FALSE;
	}
    }
    وإلا إذا (STRICMP(arg, "ul") == 0)
    {
	إذا (cterm_عام_ul_color > 0)
	    color = cterm_عام_ul_color - 1;
	وإلا
	{
	    emsg(_(e_ul_color_unknown));
	    أرجع FALSE;
	}
    }
    وإلا
    {
	صحيح bold = MAYBE;
	keyvalue_T نشانہ;
	keyvalue_T *اندراج;

	نشانہ.کلید = 0;
	نشانہ.قدر.string = arg;
	نشانہ.قدر.لمائی = 0;	// not used, see cmp_keyvalue_value_i()
	اندراج = (keyvalue_T *)bsearch(&نشانہ, &color_name_tab,
		ARRAY_لمائی(color_name_tab), حجم(color_name_tab[0]),
		cmp_keyvalue_قدر_i);
	إذا (اندراج == عدم)
	{
	    semsg(_(e_color_name_or_نمبر_not_recognized_str), کلید_شروع);
	    أرجع FALSE;
	}

	color = lookup_color(اندراج->کلید, کلید[5] == 'F', &bold);

	// set/reset bold attribute to get light foreground
	// colors (on some terminals, e.g. "linux")
	إذا (bold == TRUE)
	{
	    HL_جدول()[idx].sg_cterm |= HL_BOLD;
	    HL_جدول()[idx].sg_cterm_bold = TRUE;
	}
	وإلا إذا (bold == FALSE)
	    HL_جدول()[idx].sg_cterm &= ~HL_BOLD;
    }

    // Add one to the argument, to avoid zero.  Zero is used for
    // "NONE", then "color" is -1.
    إذا (کلید[5] == 'F')
	highlight_سیٹ_ctermfg(idx, color, is_عام_گروہ);
    وإلا إذا (کلید[5] == 'B')
	highlight_سیٹ_ctermbg(idx, color, is_عام_گروہ);
    وإلا // ctermul
	highlight_سیٹ_ctermul(idx, color, is_عام_گروہ);

    أرجع TRUE;
}

#إذا defined(FEAT_GUI) || defined(FEAT_EVAL)
/*
 * Set the GUI foreground color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ساكن صحيح
highlight_سیٹ_guifg(
	صحيح	idx,
	char_u	*arg,
	صحيح	is_menu_گروہ بن_ورتیا,
	صحيح	is_scrollbar_گروہ بن_ورتیا,
	صحيح	is_tooltip_گروہ بن_ورتیا,
	صحيح	is_titlebar_گروہ بن_ورتیا,
	صحيح	is_titlebarnc_گروہ بن_ورتیا,
	صحيح	*do_colors بن_ورتیا,
	صحيح	init)
{
# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    لما	i;
# endif
    char_u	**namep;
    صحيح		did_change = FALSE;

    إذا (init && (HL_جدول()[idx].sg_سیٹ & SG_GUI))
	أرجع FALSE;

    namep = &HL_جدول()[idx].sg_gui_fg_name;
    إذا (!init)
	HL_جدول()[idx].sg_سیٹ |= SG_GUI;

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guifg colors are only used when recognized
    i = color_name2handle(arg);
    إذا (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24_ثنائی_ہندسہ)
    {
	HL_جدول()[idx].sg_gui_fg = i;
# endif
	إذا (*namep == عدم || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    إذا (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    وإلا
		*namep = عدم;
	    did_change = TRUE;
	}
# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	إذا (is_titlebar_گروہ && gui.title_fg_pixel != i)
	{
	    gui.title_fg_pixel = i;
	    *do_colors = TRUE;
	}
	إذا (is_titlebarnc_گروہ && gui.titlenc_fg_pixel != i)
	{
	    gui.titlenc_fg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	إذا (is_menu_گروہ && gui.menu_fg_pixel != i)
	{
	    gui.menu_fg_pixel = i;
	    *do_colors = TRUE;
	}
	إذا (is_scrollbar_گروہ && gui.scroll_fg_pixel != i)
	{
	    gui.scroll_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	إذا (is_tooltip_گروہ && gui.tooltip_fg_pixel != i)
	{
	    gui.tooltip_fg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    أرجع did_change;
}

/*
 * Set the GUI background color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ساكن صحيح
highlight_سیٹ_guibg(
	صحيح	idx,
	char_u	*arg,
	صحيح	is_menu_گروہ بن_ورتیا,
	صحيح	is_scrollbar_گروہ بن_ورتیا,
	صحيح	is_tooltip_گروہ بن_ورتیا,
	صحيح	is_titlebar_گروہ بن_ورتیا,
	صحيح	is_titlebarnc_گروہ بن_ورتیا,
	صحيح	*do_colors بن_ورتیا,
	صحيح	init)
{
# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    صحيح		i;
# endif
    char_u	**namep;
    صحيح		did_change = FALSE;

    إذا (init && (HL_جدول()[idx].sg_سیٹ & SG_GUI))
	أرجع FALSE;

    namep = &HL_جدول()[idx].sg_gui_bg_name;
    إذا (!init)
	HL_جدول()[idx].sg_سیٹ |= SG_GUI;

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guibg colors are only used when recognized
    i = color_name2handle(arg);
    إذا (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24_ثنائی_ہندسہ)
    {
	HL_جدول()[idx].sg_gui_bg = i;
# endif
	إذا (*namep == عدم || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    إذا (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    وإلا
		*namep = عدم;
	    did_change = TRUE;
	}
# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI_MSWIN
	إذا (is_titlebar_گروہ && gui.title_bg_pixel != i)
	{
	    gui.title_bg_pixel = i;
	    *do_colors = TRUE;
	}
	إذا (is_titlebarnc_گروہ && gui.titlenc_bg_pixel != i)
	{
	    gui.titlenc_bg_pixel = i;
	    *do_colors = TRUE;
	}
#  endif
#  ifdef FEAT_GUI_X11
	إذا (is_menu_گروہ && gui.menu_bg_pixel != i)
	{
	    gui.menu_bg_pixel = i;
	    *do_colors = TRUE;
	}
	إذا (is_scrollbar_گروہ && gui.scroll_bg_pixel != i)
	{
	    gui.scroll_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   ifdef FEAT_BEVAL_GUI
	إذا (is_tooltip_گروہ && gui.tooltip_bg_pixel != i)
	{
	    gui.tooltip_bg_pixel = i;
	    *do_colors = TRUE;
	}
#   endif
#  endif
    }
# endif

    أرجع did_change;
}

/*
 * Set the GUI undercurl/strikethrough color for the highlight group at 'idx'.
 * Returns TRUE if the color is set.
 */
    ساكن صحيح
highlight_سیٹ_guisp(صحيح idx, char_u *arg, صحيح init)
{
# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    صحيح		i;
# endif
    صحيح		did_change = FALSE;
    char_u	**namep;

    إذا (init && (HL_جدول()[idx].sg_سیٹ & SG_GUI))
	أرجع FALSE;

    namep = &HL_جدول()[idx].sg_gui_sp_name;
    إذا (!init)
	HL_جدول()[idx].sg_سیٹ |= SG_GUI;

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // In GUI guisp colors are only used when recognized
    i = color_name2handle(arg);
    إذا (i != INVALCOLOR || STRCMP(arg, "NONE") == 0 || !USE_24_ثنائی_ہندسہ)
    {
	HL_جدول()[idx].sg_gui_sp = i;
# endif
	إذا (*namep == عدم || STRCMP(*namep, arg) != 0)
	{
	    vim_free(*namep);
	    إذا (STRCMP(arg, "NONE") != 0)
		*namep = vim_strsave(arg);
	    وإلا
		*namep = عدم;
	    did_change = TRUE;
	}
# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    }
# endif

    أرجع did_change;
}
#جے_انت

/*
 * Set the start/stop terminal codes for a highlight group.
 * Returns TRUE if the terminal code is set.
 */
    ساكن صحيح
highlight_سیٹ_startstop_termcode(صحيح idx, char_u *کلید, char_u *arg, صحيح init)
{
    صحيح		off;
    char_u	buf[100];
    صحيح		len;
    char_u	*tname;
    char_u	*p;

    إذا (!init)
	HL_جدول()[idx].sg_سیٹ |= SG_رکن;

    // The "start" and "stop"  arguments can be a literal escape
    // sequence, or a comma separated list of terminal codes.
    إذا (STRNCMP(arg, "t_", 2) == 0)
    {
	off = 0;
	buf[0] = 0;
	طالما (arg[off] != NUL)
	{
	    // Isolate one termcap name
	    لكل (len = 0; arg[off + len] &&
		    arg[off + len] != ','; ++len)
		;
	    tname = vim_strnsave(arg + off, len);
	    إذا (tname == عدم)		// out of memory
		أرجع FALSE;
	    // lookup the escape sequence for the item
	    p = get_رکن_کوڈ(tname);
	    vim_free(tname);
	    إذا (p == عدم)	    // ignore non-existing things
		p = (char_u *)"";

	    // Append it to the already found stuff
	    إذا ((صحيح)(STRLEN(buf) + STRLEN(p)) >= 99)
	    {
		semsg(_(e_terminal_کوڈ_too_long_str), arg);
		أرجع FALSE;
	    }
	    STRCAT(buf, p);

	    // Advance to the next item
	    off += len;
	    إذا (arg[off] == ',')	    // another one follows
		++off;
	}
    }
    وإلا
    {
	// Copy characters from arg[] to buf[], translating <> codes.
	لكل (p = arg, off = 0; off < 100 - 6 && *p; )
	{
	    len = trans_special(&p, buf + off, FSK_SIMPLIFY, FALSE, عدم);
	    إذا (len > 0)	    // recognized special char
		off += len;
	    وإلا		    // copy as normal char
		buf[off++] = *p++;
	}
	buf[off] = NUL;
    }

    إذا (STRCMP(buf, "NONE") == 0)	// resetting the value
	p = عدم;
    وإلا
	p = vim_strsave(buf);
    إذا (کلید[2] == 'A')
    {
	vim_free(HL_جدول()[idx].sg_شروع);
	HL_جدول()[idx].sg_شروع = p;
    }
    وإلا
    {
	vim_free(HL_جدول()[idx].sg_stop);
	HL_جدول()[idx].sg_stop = p;
    }
    أرجع TRUE;
}

/*
 * Handle the ":highlight .." command.
 * When using ":hi clear" this is called recursively for each group with
 * "forceit" and "init" both TRUE.
 */
    فراغ
do_highlight(
    char_u	*سطر,
    صحيح		forceit,
    صحيح		init)	    // TRUE when called for initializing
{
    char_u	*name_سرا;
    char_u	*linep;
    char_u	*کلید_شروع;
    char_u	*arg_شروع;
    char_u	*کلید = عدم, *arg = عدم;
    لما	i;
    صحيح		id;
    صحيح		idx;
    hl_گروہ_T	شے_before;
    صحيح		did_change = FALSE;
    صحيح		dodefault = FALSE;
    صحيح		doclear = FALSE;
    صحيح		dolink = FALSE;
    صحيح		error = FALSE;
    صحيح		is_عام_گروہ = FALSE;	// "Normal" group
#جے_تعریف_شدہ FEAT_GUI_MSWIN
    صحيح		is_titlebar_گروہ = FALSE;	// "TitleBar" group
    صحيح		is_titlebarnc_گروہ = FALSE;	// "TitleBarNC" group
#وإلا
# define is_titlebar_گروہ 0
# define is_titlebarnc_گروہ 0
#جے_انت
#جے_تعریف_شدہ FEAT_GUI_X11
    صحيح		is_menu_گروہ = FALSE;		// "Menu" group
    صحيح		is_scrollbar_گروہ = FALSE;	// "Scrollbar" group
    صحيح		is_tooltip_گروہ = FALSE;	// "Tooltip" group
#وإلا
# define is_menu_گروہ 0
# define is_tooltip_گروہ 0
# define is_scrollbar_گروہ 0
#جے_انت
#إذا defined(FEAT_GUI) || defined(FEAT_EVAL)
    صحيح		do_colors = FALSE;		// need to update colors?
#جے_انت
#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    صحيح		did_highlight_بدلیا = FALSE;
#جے_انت

    // If no argument, list current highlighting.
    إذا (!init && سرے_excmd2(سطر - 1, سطر))
    {
	لكل (i = 1; i <= highlight_ga.ga_len && !got_int; ++i)
	    // TODO: only call when the group has attributes set
	    highlight_list_one((صحيح)i);
	أرجع;
    }

    // Isolate the name.
    name_سرا = skiptowhite(سطر);
    linep = skipwhite(name_سرا);

    // Check for "default" argument.
    إذا (STRNCMP(سطر, "default", name_سرا - سطر) == 0)
    {
	dodefault = TRUE;
	سطر = linep;
	name_سرا = skiptowhite(سطر);
	linep = skipwhite(name_سرا);
    }

    // Check for "clear" or "link" argument.
    إذا (STRNCMP(سطر, "clear", name_سرا - سطر) == 0)
	doclear = TRUE;
    إذا (STRNCMP(سطر, "link", name_سرا - سطر) == 0)
	dolink = TRUE;

    // ":highlight {group-name}": list highlighting for one group.
    إذا (!doclear && !dolink && سرے_excmd2(سطر, linep))
    {
	id = syn_namen2id(سطر, (صحيح)(name_سرا - سطر));
	إذا (id == 0)
	    semsg(_(e_highlight_گروہ_name_not_لبھیا_str), سطر);
	وإلا
	    highlight_list_one(id);
	أرجع;
    }

    // Handle ":highlight link {from} {to}" command.
    إذا (dolink)
    {
	char_u	    *from_شروع = linep;
	char_u	    *from_سرا;
	صحيح	    from_len;
	char_u	    *منزل_شروع;
	char_u	    *منزل_سرا;
	صحيح	    منزل_len;

	from_سرا = skiptowhite(from_شروع);
	منزل_شروع = skipwhite(from_سرا);
	منزل_سرا	 = skiptowhite(منزل_شروع);

	إذا (سرے_excmd2(سطر, from_شروع) || سرے_excmd2(سطر, منزل_شروع))
	{
	    semsg(_(e_not_enough_arguments_highlight_link_str), from_شروع);
	    أرجع;
	}

	إذا (!سرے_excmd2(سطر, skipwhite(منزل_سرا)))
	{
	    semsg(_(e_too_many_arguments_highlight_link_str), from_شروع);
	    أرجع;
	}

	from_len = (صحيح)(from_سرا - from_شروع);
	منزل_len = (صحيح)(منزل_سرا - منزل_شروع);
	highlight_گروہ_link(from_شروع, from_len, منزل_شروع, منزل_len,
						dodefault, forceit, init);
	أرجع;
    }

    إذا (doclear)
    {
	// ":highlight clear [group]" command.
	إذا (سرے_excmd2(سطر, linep))
	{
	    // ":highlight clear" without group name
	    highlight_reset_all();
	    أرجع;
	}
	سطر = linep;
	name_سرا = skiptowhite(سطر);
	linep = skipwhite(name_سرا);
    }

    // Find the group name in the table.  If it does not exist yet, add it.
    id = syn_check_گروہ(سطر, (صحيح)(name_سرا - سطر));
    إذا (id == 0)	// failed (out of memory)
	أرجع;
    idx = id - 1;	// index is ID minus one

    // Return if "default" was used and the group already has settings.
    إذا (dodefault && hl_has_settings(idx, TRUE))
	أرجع;

    // Make a copy so we can check if any attribute actually changed.
    شے_before = HL_جدول()[idx];

    إذا (STRCMP(HL_جدول()[idx].sg_name_u, "NORMAL") == 0)
	is_عام_گروہ = TRUE;
#جے_تعریف_شدہ FEAT_GUI_MSWIN
    وإلا إذا (STRCMP(HL_جدول()[idx].sg_name_u, "TITLEBAR") == 0)
	is_titlebar_گروہ = TRUE;
    وإلا إذا (STRCMP(HL_جدول()[idx].sg_name_u, "TITLEBARNC") == 0)
	is_titlebarnc_گروہ = TRUE;
#جے_انت
#جے_تعریف_شدہ FEAT_GUI_X11
    وإلا إذا (STRCMP(HL_جدول()[idx].sg_name_u, "MENU") == 0)
	is_menu_گروہ = TRUE;
    وإلا إذا (STRCMP(HL_جدول()[idx].sg_name_u, "SCROLLBAR") == 0)
	is_scrollbar_گروہ = TRUE;
    وإلا إذا (STRCMP(HL_جدول()[idx].sg_name_u, "TOOLTIP") == 0)
	is_tooltip_گروہ = TRUE;
#جے_انت

    // Clear the highlighting for ":hi clear {group}" and ":hi clear".
    إذا (doclear || (forceit && init))
    {
	highlight_clear(idx);
	إذا (!doclear)
	    HL_جدول()[idx].sg_سیٹ = 0;
    }

    إذا (!doclear)
	طالما (!سرے_excmd2(سطر, linep))
	{
	    کلید_شروع = linep;
	    إذا (*linep == '=')
	    {
		semsg(_(e_unexpected_equal_sign_str), کلید_شروع);
		error = TRUE;
		اكسر;
	    }

	    // Note: Keep this in sync with get_highlight_group_key.

	    // Isolate the key ("term", "ctermfg", "ctermbg", "font", "guifg"
	    // or "guibg").
	    طالما (*linep && !VIM_ISWHITE(*linep) && *linep != '=')
		++linep;
	    vim_free(کلید);
	    کلید = vim_strnsave_اتے(کلید_شروع, linep - کلید_شروع);
	    إذا (کلید == عدم)
	    {
		error = TRUE;
		اكسر;
	    }
	    linep = skipwhite(linep);

	    إذا (STRCMP(کلید, "NONE") == 0)
	    {
		إذا (!init || HL_جدول()[idx].sg_سیٹ == 0)
		{
		    إذا (!init)
			HL_جدول()[idx].sg_سیٹ |= SG_رکن+SG_CTERM+SG_GUI;
		    highlight_clear(idx);
		}
		استمر;
	    }

	    // Check for the equal sign.
	    إذا (*linep != '=')
	    {
		semsg(_(e_missing_equal_sign_str_2), کلید_شروع);
		error = TRUE;
		اكسر;
	    }
	    ++linep;

	    // Isolate the argument.
	    linep = skipwhite(linep);
	    إذا (*linep == '\'')		// guifg='color name'
	    {
		arg_شروع = ++linep;
		linep = vim_strchr(linep, '\'');
		إذا (linep == عدم)
		{
		    semsg(_(e_invalid_دلیل_str), کلید_شروع);
		    error = TRUE;
		    اكسر;
		}
	    }
	    وإلا
	    {
		arg_شروع = linep;
		linep = skiptowhite(linep);
	    }
	    إذا (linep == arg_شروع)
	    {
		semsg(_(e_missing_دلیل_str), کلید_شروع);
		error = TRUE;
		اكسر;
	    }
	    vim_free(arg);
	    arg = vim_strnsave(arg_شروع, linep - arg_شروع);
	    إذا (arg == عدم)
	    {
		error = TRUE;
		اكسر;
	    }
	    إذا (*linep == '\'')
		++linep;

	    // Store the argument.
	    إذا (STRCMP(کلید, "TERM") == 0
		    || STRCMP(کلید, "CTERM") == 0
		    || STRCMP(کلید, "GUI") == 0)
	    {
		إذا (!highlight_سیٹ_termgui_attr(idx, کلید, arg, init))
		{
		    error = TRUE;
		    اكسر;
		}
	    }
	    وإلا إذا (STRCMP(کلید, "FONT") == 0)
	    {
		// in non-GUI fonts are simply ignored
#جے_تعریف_شدہ FEAT_GUI
		إذا (highlight_سیٹ_font(idx, arg, is_عام_گروہ,
					      is_menu_گروہ, is_tooltip_گروہ))
		    did_change = TRUE;
#جے_انت
	    }
	    وإلا إذا (STRCMP(کلید, "CTERMFG") == 0
		    || STRCMP(کلید, "CTERMBG") == 0
		    || STRCMP(کلید, "CTERMUL") == 0)
	    {
		إذا (!highlight_سیٹ_cterm_color(idx, کلید, کلید_شروع, arg,
							is_عام_گروہ, init))
		{
		    error = TRUE;
		    اكسر;
		}
	    }
	    وإلا إذا (STRCMP(کلید, "CTERMFONT") == 0)
	    {
		إذا (!highlight_سیٹ_cterm_font(idx, arg, init))
		{
		    error = TRUE;
		    اكسر;
		}
	    }
	    وإلا إذا (STRCMP(کلید, "GUIFG") == 0)
	    {
#إذا defined(FEAT_GUI) || defined(FEAT_EVAL)
		إذا (highlight_سیٹ_guifg(idx, arg, is_menu_گروہ,
			    is_scrollbar_گروہ, is_tooltip_گروہ,
			    is_titlebar_گروہ, is_titlebarnc_گروہ,
			    &do_colors, init))
		    did_change = TRUE;
#جے_انت
	    }
	    وإلا إذا (STRCMP(کلید, "GUIBG") == 0)
	    {
#إذا defined(FEAT_GUI) || defined(FEAT_EVAL)
		إذا (highlight_سیٹ_guibg(idx, arg, is_menu_گروہ,
			    is_scrollbar_گروہ, is_tooltip_گروہ,
			    is_titlebar_گروہ, is_titlebarnc_گروہ,
			    &do_colors, init))
		    did_change = TRUE;
#جے_انت
	    }
	    وإلا إذا (STRCMP(کلید, "GUISP") == 0)
	    {
#إذا defined(FEAT_GUI) || defined(FEAT_EVAL)
		إذا (highlight_سیٹ_guisp(idx, arg, init))
		    did_change = TRUE;
#جے_انت
	    }
	    وإلا إذا (STRCMP(کلید, "START") == 0 || STRCMP(کلید, "STOP") == 0)
	    {
		إذا (!highlight_سیٹ_startstop_termcode(idx, کلید, arg, init))
		{
		    error = TRUE;
		    اكسر;
		}
	    }
	    وإلا
	    {
		semsg(_(e_illegal_دلیل_str_3), کلید_شروع);
		error = TRUE;
		اكسر;
	    }
	    HL_جدول()[idx].sg_cleared = FALSE;

	    // When highlighting has been given for a group, don't link it.
	    إذا (!init || !(HL_جدول()[idx].sg_سیٹ & SG_LINK))
		HL_جدول()[idx].sg_link = 0;

	    // Continue with next argument.
	    linep = skipwhite(linep);
	}

    // If there is an error, and it's a new entry, remove it from the table.
    إذا (error && idx == highlight_ga.ga_len)
	syn_unadd_گروہ();
    وإلا
    {
	إذا (is_عام_گروہ)
	{
	    HL_جدول()[idx].sg_رکن_attr = 0;
	    HL_جدول()[idx].sg_cterm_attr = 0;
#جے_تعریف_شدہ FEAT_GUI
	    HL_جدول()[idx].sg_gui_attr = 0;
	    // Need to update all groups, because they might be using "bg"
	    // and/or "fg", which have been changed now.
#جے_انت
#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    إذا (USE_24_ثنائی_ہندسہ)
	    {
		highlight_gui_started();
		did_highlight_بدلیا = TRUE;
		redraw_all_later(UPD_NOT_VALID);
	    }
#جے_انت
	    resolve_fallback_fg_منزل_rgb();
	    resolve_fallback_bg_منزل_rgb();
#جے_تعریف_شدہ FEAT_VTP
	    control_console_color_rgb();
#جے_انت
	}
#جے_تعریف_شدہ FEAT_GUI_MSWIN
	وإلا إذا (is_titlebar_گروہ)
	{
	    إذا (gui.وچ_use && do_colors)
		gui_mch_سیٹ_titlebar_colors();
	}
	وإلا إذا (is_titlebarnc_گروہ)
	{
	    إذا (gui.وچ_use && do_colors)
		gui_mch_سیٹ_titlebar_colors();
	}
#جے_انت
#جے_تعریف_شدہ FEAT_GUI_X11
# ifdef FEAT_MENU
	وإلا إذا (is_menu_گروہ)
	{
	    إذا (gui.وچ_use && do_colors)
		gui_mch_new_menu_colors();
	}
# endif
	وإلا إذا (is_scrollbar_گروہ)
	{
	    إذا (gui.وچ_use && do_colors)
		gui_new_scrollbar_colors();
	    وإلا
		سیٹ_hl_attr(idx);
	}
# ifdef FEAT_BEVAL_GUI
	وإلا إذا (is_tooltip_گروہ)
	{
	    إذا (gui.وچ_use && do_colors)
		gui_mch_new_tooltip_colors();
	}
# endif
#جے_انت
	وإلا
	    سیٹ_hl_attr(idx);
#جے_تعریف_شدہ FEAT_EVAL
	HL_جدول()[idx].sg_script_ctx = current_sctx;
	HL_جدول()[idx].sg_script_ctx.sc_lnum += SOURCING_LNUM;
#جے_انت
    }

    vim_free(کلید);
    vim_free(arg);

    // Only call highlight_changed() once, after a sequence of highlight
    // commands, and only if an attribute actually changed.
    إذا ((did_change
	   || memcmp(&HL_جدول()[idx], &شے_before, حجم(شے_before)) != 0)
#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	    && !did_highlight_بدلیا
#جے_انت
       )
    {
	// Do not trigger a redraw when highlighting is changed while
	// redrawing.  This may happen when evaluating 'statusline' changes the
	// StatusLine group.
	إذا (!updating_screen)
	    redraw_all_later(UPD_NOT_VALID);
	need_highlight_بدلیا = TRUE;
    }
}

#إذا defined(EXITFREE)
    فراغ
free_highlight(فراغ)
{
    صحيح	    i;

    لكل (i = 0; i < highlight_ga.ga_len; ++i)
    {
	highlight_clear(i);
	vim_free(HL_جدول()[i].sg_name);
	vim_free(HL_جدول()[i].sg_name_u - HLNAME_کلید_OFF);
    }
    ga_clear(&highlight_ga);
    hash_clear(&highlight_ht);
    highlight_ht_inited = false;
}
#جے_انت

/*
 * Reset the cterm colors to what they were before Vim was started, if
 * possible.  Otherwise reset them to zero.
 */
    فراغ
restore_cterm_colors(فراغ)
{
#إذا defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
    // Since t_me has been set, this probably means that the user
    // wants to use this as default colors.  Need to reset default
    // background/foreground colors.
    mch_سیٹ_عام_colors();
#وإلا
# ifdef VIMDLL
    إذا (!gui.وچ_use)
    {
	mch_سیٹ_عام_colors();
	أرجع;
    }
# endif
    cterm_عام_fg_color = 0;
    cterm_عام_fg_bold = 0;
    cterm_عام_bg_color = 0;
# ifdef FEAT_TERMGUICOLORS
    cterm_عام_fg_gui_color = INVALCOLOR;
    cterm_عام_bg_gui_color = INVALCOLOR;
    cterm_عام_ul_gui_color = INVALCOLOR;
# endif
    fallback_fg_rgb = INVALCOLOR;
    fallback_bg_rgb = INVALCOLOR;
#جے_انت
}

/*
 * Return TRUE if highlight group "idx" has any settings.
 * When "check_link" is TRUE also check for an existing link.
 */
    ساكن صحيح
hl_has_settings(صحيح idx, صحيح check_link)
{
    أرجع HL_جدول()[idx].sg_cleared == 0
	 && (  HL_جدول()[idx].sg_رکن_attr != 0
	    || HL_جدول()[idx].sg_cterm_attr != 0
	    || HL_جدول()[idx].sg_cterm_fg != 0
	    || HL_جدول()[idx].sg_cterm_bg != 0
	    || HL_جدول()[idx].sg_cterm_font != 0
#جے_تعریف_شدہ FEAT_GUI
	    || HL_جدول()[idx].sg_gui_attr != 0
	    || HL_جدول()[idx].sg_gui_fg_name != عدم
	    || HL_جدول()[idx].sg_gui_bg_name != عدم
	    || HL_جدول()[idx].sg_gui_sp_name != عدم
	    || HL_جدول()[idx].sg_font_name != عدم
#جے_انت
	    || (check_link && (HL_جدول()[idx].sg_سیٹ & SG_LINK)));
}

/*
 * Clear highlighting for one group.
 */
    ساكن فراغ
highlight_clear(صحيح idx)
{
    HL_جدول()[idx].sg_cleared = TRUE;

    HL_جدول()[idx].sg_رکن = 0;
    VIM_CLEAR(HL_جدول()[idx].sg_شروع);
    VIM_CLEAR(HL_جدول()[idx].sg_stop);
    HL_جدول()[idx].sg_رکن_attr = 0;
    HL_جدول()[idx].sg_cterm = 0;
    HL_جدول()[idx].sg_cterm_bold = FALSE;
    HL_جدول()[idx].sg_cterm_fg = 0;
    HL_جدول()[idx].sg_cterm_bg = 0;
    HL_جدول()[idx].sg_cterm_attr = 0;
    HL_جدول()[idx].sg_cterm_font = 0;
#إذا defined(FEAT_GUI) || defined(FEAT_EVAL)
    HL_جدول()[idx].sg_gui = 0;
    VIM_CLEAR(HL_جدول()[idx].sg_gui_fg_name);
    VIM_CLEAR(HL_جدول()[idx].sg_gui_bg_name);
    VIM_CLEAR(HL_جدول()[idx].sg_gui_sp_name);
#جے_انت
#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_جدول()[idx].sg_gui_fg = INVALCOLOR;
    HL_جدول()[idx].sg_gui_bg = INVALCOLOR;
    HL_جدول()[idx].sg_gui_sp = INVALCOLOR;
#جے_انت
#جے_تعریف_شدہ FEAT_GUI
    gui_mch_free_font(HL_جدول()[idx].sg_font);
    HL_جدول()[idx].sg_font = NOFONT;
# ifdef FEAT_XFONTSET
    gui_mch_free_fontset(HL_جدول()[idx].sg_fontset);
    HL_جدول()[idx].sg_fontset = NOFONTSET;
# endif
    VIM_CLEAR(HL_جدول()[idx].sg_font_name);
    HL_جدول()[idx].sg_gui_attr = 0;
#جے_انت
    // Restore default link and context if they exist. Otherwise clears.
    HL_جدول()[idx].sg_link = HL_جدول()[idx].sg_deflink;
#جے_تعریف_شدہ FEAT_EVAL
    // Since we set the default link, set the location to where the default
    // link was set.
    HL_جدول()[idx].sg_script_ctx = HL_جدول()[idx].sg_deflink_sctx;
#جے_انت
}

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Set the normal foreground and background colors according to the "Normal"
 * highlighting group.  For X11 also set "Menu", "Scrollbar" and "Tooltip"
 * colors.  For MS-Windows also set "TitleBar" and "TitleBarNC" colors.
 */
    فراغ
سیٹ_عام_colors(فراغ)
{
# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    إذا (gui.وچ_use)
#  endif
    {
	إذا (سیٹ_گروہ_colors((char_u *)"Normal",
				 &gui.norm_pixel, &gui.پٹھ_pixel,
				 FALSE, TRUE, FALSE))
	{
	    gui_mch_new_colors();
	    سیٹ_must_redraw(UPD_CLEAR);
	}
#  ifdef FEAT_GUI_MSWIN
	إذا (سیٹ_گروہ_colors((char_u *)"TitleBar",
				 &gui.title_fg_pixel, &gui.title_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_سیٹ_titlebar_colors();
	    سیٹ_must_redraw(UPD_CLEAR);
	}
	إذا (سیٹ_گروہ_colors((char_u *)"TitleBarNC",
				 &gui.titlenc_fg_pixel, &gui.titlenc_bg_pixel,
				 FALSE, FALSE, FALSE))
	{
	    gui_mch_سیٹ_titlebar_colors();
	    سیٹ_must_redraw(UPD_CLEAR);
	}
#  endif
#  ifdef FEAT_GUI_X11
	إذا (سیٹ_گروہ_colors((char_u *)"Menu",
			     &gui.menu_fg_pixel, &gui.menu_bg_pixel,
			     TRUE, FALSE, FALSE))
	{
#   ifdef FEAT_MENU
	    gui_mch_new_menu_colors();
#   endif
	    سیٹ_must_redraw(UPD_CLEAR);
	}
#   ifdef FEAT_BEVAL_GUI
	إذا (سیٹ_گروہ_colors((char_u *)"Tooltip",
			     &gui.tooltip_fg_pixel, &gui.tooltip_bg_pixel,
			     FALSE, FALSE, TRUE))
	{
#    ifdef FEAT_TOOLBAR
	    gui_mch_new_tooltip_colors();
#    endif
	    سیٹ_must_redraw(UPD_CLEAR);
	}
#   endif
	إذا (سیٹ_گروہ_colors((char_u *)"Scrollbar",
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
    وإلا
#  endif
    {
	صحيح		idx;

	idx = syn_name2id((char_u *)"Normal") - 1;
	إذا (idx >= 0)
	{
	    gui_do_one_color(idx, FALSE, FALSE);

	    // If the normal fg or bg color changed a complete redraw is
	    // required.
	    إذا (cterm_عام_fg_gui_color != HL_جدول()[idx].sg_gui_fg
		    || cterm_عام_bg_gui_color != HL_جدول()[idx].sg_gui_bg)
	    {
		// if the GUI color is INVALCOLOR then we use the default cterm
		// color
		cterm_عام_fg_gui_color = HL_جدول()[idx].sg_gui_fg;
		cterm_عام_bg_gui_color = HL_جدول()[idx].sg_gui_bg;
		سیٹ_must_redraw(UPD_CLEAR);
	    }
	}
    }
# endif
}
#جے_انت

#إذا defined(FEAT_GUI)
/*
 * Set the colors for "Normal", "Menu", "TitleBar", "TitleBarNC", "Tooltip" or
 * "Scrollbar".
 */
    ساكن صحيح
سیٹ_گروہ_colors(
    char_u	*name,
    guicolor_T	*fgp,
    guicolor_T	*bgp,
    صحيح		do_menu,
    صحيح		use_norm,
    صحيح		do_tooltip)
{
    صحيح		idx;

    idx = syn_name2id(name) - 1;
    إذا (idx < 0)
	أرجع FALSE;

    gui_do_one_color(idx, do_menu, do_tooltip);

    إذا (HL_جدول()[idx].sg_gui_fg != INVALCOLOR)
	*fgp = HL_جدول()[idx].sg_gui_fg;
    وإلا إذا (use_norm)
	*fgp = gui.def_norm_pixel;
    إذا (HL_جدول()[idx].sg_gui_bg != INVALCOLOR)
	*bgp = HL_جدول()[idx].sg_gui_bg;
    وإلا إذا (use_norm)
	*bgp = gui.def_پٹھ_pixel;
    أرجع TRUE;
}

/*
 * Get the font of the "Normal" group.
 * Returns "" when it's not found or not set.
 */
    char_u *
hl_get_font_name(فراغ)
{
    صحيح		id;
    char_u	*s;

    id = syn_name2id((char_u *)"Normal");
    إذا (id > 0)
    {
	s = HL_جدول()[id - 1].sg_font_name;
	إذا (s != عدم)
	    أرجع s;
    }
    أرجع (char_u *)"";
}

/*
 * Set font for "Normal" group.  Called by gui_mch_init_font() when a font has
 * actually chosen to be used.
 */
    فراغ
hl_سیٹ_font_name(char_u *font_name)
{
    صحيح	    id;

    id = syn_name2id((char_u *)"Normal");
    إذا (id <= 0)
	أرجع;

    vim_free(HL_جدول()[id - 1].sg_font_name);
    HL_جدول()[id - 1].sg_font_name = vim_strsave(font_name);
}

/*
 * Set background color for "Normal" group.  Called by gui_set_bg_color()
 * when the color is known.
 */
    فراغ
hl_سیٹ_bg_color_name(
    char_u  *name)	    // must have been allocated
{
    صحيح	    id;

    إذا (name == عدم)
	أرجع;

    id = syn_name2id((char_u *)"Normal");
    إذا (id <= 0)
	أرجع;

    vim_free(HL_جدول()[id - 1].sg_gui_bg_name);
    HL_جدول()[id - 1].sg_gui_bg_name = name;
}

/*
 * Set foreground color for "Normal" group.  Called by gui_set_fg_color()
 * when the color is known.
 */
    فراغ
hl_سیٹ_fg_color_name(
    char_u  *name)	    // must have been allocated
{
    صحيح	    id;

    إذا (name == عدم)
	أرجع;

    id = syn_name2id((char_u *)"Normal");
    إذا (id <= 0)
	أرجع;

    vim_free(HL_جدول()[id - 1].sg_gui_fg_name);
    HL_جدول()[id - 1].sg_gui_fg_name = name;
}

/*
 * Return the handle for a font name.
 * Returns NOFONT when failed.
 */
    ساكن GuiFont
font_name2handle(char_u *name)
{
    إذا (STRCMP(name, "NONE") == 0)
	أرجع NOFONT;

    أرجع gui_mch_get_font(name, TRUE);
}

# ifdef FEAT_XFONTSET
/*
 * Return the handle for a fontset name.
 * Returns NOFONTSET when failed.
 */
    ساكن GuiFontset
fontset_name2handle(char_u *name, صحيح fixed_چوڑائی)
{
    إذا (STRCMP(name, "NONE") == 0)
	أرجع NOFONTSET;

    أرجع gui_mch_get_fontset(name, TRUE, fixed_چوڑائی);
}
# endif

/*
 * Get the font or fontset for one highlight group.
 */
    ساكن فراغ
hl_do_font(
    صحيح		idx,
    char_u	*arg,
    صحيح		do_عام,		// set normal font
    صحيح		do_menu بن_ورتیا,		// set menu font
    صحيح		do_tooltip بن_ورتیا,	// set tooltip font
    صحيح		free_font)		// free current font/fontset
{
# ifdef FEAT_XFONTSET
    // If 'guifontset' is not empty, first try using the name as a
    // fontset.  If that doesn't work, use it as a font name.
    إذا (*p_guifontset != NUL
#  ifdef FONTSET_ALWAYS
	|| do_menu
#  endif
#  ifdef FEAT_BEVAL_TIP
	// In Motif, the Tooltip highlight group is always a fontset
	|| do_tooltip
#  endif
	    )
    {
	إذا (free_font)
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
    إذا (HL_جدول()[idx].sg_fontset != NOFONTSET)
    {
	// If it worked and it's the Normal group, use it as the normal
	// fontset.  Same for the Menu group.
	إذا (do_عام)
	    gui_init_font(arg, TRUE);
#  إذا defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	إذا (do_menu)
	{
#   ifdef FONTSET_ALWAYS
	    gui.menu_fontset = HL_جدول()[idx].sg_fontset;
#   وإلا
	    // YIKES!  This is a bug waiting to crash the program
	    gui.menu_font = HL_جدول()[idx].sg_fontset;
#   endif
	    gui_mch_new_menu_font();
	}
#   ifdef FEAT_BEVAL_GUI
	إذا (do_tooltip)
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
    وإلا
# endif
    {
	إذا (free_font)
	    gui_mch_free_font(HL_جدول()[idx].sg_font);
	HL_جدول()[idx].sg_font = font_name2handle(arg);
	// If it worked and it's the Normal group, use it as the
	// normal font.  Same for the Menu group.
	إذا (HL_جدول()[idx].sg_font != NOFONT)
	{
	    إذا (do_عام)
		gui_init_font(arg, FALSE);
# ifndef FONTSET_ALWAYS
#  إذا defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	    إذا (do_menu)
	    {
		gui.menu_font = HL_جدول()[idx].sg_font;
		gui_mch_new_menu_font();
	    }
#  endif
# endif
	}
    }
}

#جے_انت // FEAT_GUI

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return the handle for a color name.
 * Returns INVALCOLOR when failed.
 */
    guicolor_T
color_name2handle(char_u *name)
{
    إذا (STRCMP(name, "NONE") == 0)
	أرجع INVALCOLOR;

    إذا (STRICMP(name, "fg") == 0 || STRICMP(name, "foreground") == 0)
    {
# إذا defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	إذا (gui.وچ_use)
# endif
# ifdef FEAT_GUI
	    أرجع gui.norm_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	إذا (cterm_عام_fg_gui_color != INVALCOLOR)
	    أرجع cterm_عام_fg_gui_color;
	// Guess that the foreground is black or white.
	أرجع GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "black" : "white"));
# endif
    }
    إذا (STRICMP(name, "bg") == 0 || STRICMP(name, "background") == 0)
    {
# إذا defined(FEAT_TERMGUICOLORS) && defined(FEAT_GUI)
	إذا (gui.وچ_use)
# endif
# ifdef FEAT_GUI
	    أرجع gui.پٹھ_pixel;
# endif
# ifdef FEAT_TERMGUICOLORS
	إذا (cterm_عام_bg_gui_color != INVALCOLOR)
	    أرجع cterm_عام_bg_gui_color;
	// Guess that the background is white or black.
	أرجع GUI_GET_COLOR((char_u *)(*p_bg == 'l' ? "white" : "black"));
# endif
    }

    أرجع GUI_GET_COLOR(name);
}

// On MS-Windows an RGB macro is available and it produces 0x00bbggrr color
// values as used by the MS-Windows GDI api.  It should be used only for
// MS-Windows GDI builds.
# إذا defined(RGB) && defined(MSWIN) && !defined(FEAT_GUI)
#  undef RGB
# endif
# ifndef RGB
#  define RGB(r, g, b)	(((r)<<16) | ((g)<<8) | (b))
# endif

# ifdef VIMDLL
    ساكن guicolor_T
gui_adjust_rgb(guicolor_T c)
{
    إذا (gui.وچ_use)
	أرجع c;
    وإلا
	أرجع ((c & 0xff) << 16) | (c & 0x00ff00) | ((c >> 16) & 0xff);
}
# وإلا
#  define gui_adjust_rgb(c) (c)
# endif

    ساكن صحيح
hex_digit(صحيح c)
{
    إذا (محفوظ_isdigit(c))
	أرجع c - '0';
    c = TOLOWER_ASC(c);
    إذا (c >= 'a' && c <= 'f')
	أرجع c - 'a' + 10;
    أرجع 0x1ffffff;
}

    ساكن guicolor_T
decode_hex_color(char_u *hex)
{
    guicolor_T color;

    إذا (hex[0] != '#' || STRLEN(hex) != 7)
	أرجع INVALCOLOR;

    // Name is in "#rrggbb" format
    color = RGB(((hex_digit(hex[1]) << 4) + hex_digit(hex[2])),
		((hex_digit(hex[3]) << 4) + hex_digit(hex[4])),
		((hex_digit(hex[5]) << 4) + hex_digit(hex[6])));
    إذا (color > 0xffffff)
	أرجع INVALCOLOR;
    أرجع gui_adjust_rgb(color);
}

# ifdef FEAT_EVAL
// Returns the color currently mapped to the given name or INVALCOLOR if no
// such name exists in the color table. The convention is to use lowercase for
// all keys in the v:colornames dictionary. The value can be either a string in
// the form #rrggbb or a number, either of which is converted to a guicolor_T.
    ساكن guicolor_T
colorname2rgb(char_u *name)
{
    dict_T      *colornames_جدول = get_vim_var_dict(VV_COLORNAMES);
    char_u      *lc_name;
    dictitem_T  *colentry;
    char_u      *colstr;
    varnumber_T colnum;

    lc_name = strlow_save(name);
    إذا (lc_name == عدم)
	أرجع INVALCOLOR;

    colentry = dict_لبھو(colornames_جدول, lc_name, -1);
    vim_free(lc_name);
    إذا (colentry == عدم)
	أرجع INVALCOLOR;

    إذا (colentry->di_tv.v_type == VAR_STRING)
    {
	colstr = tv_get_string_strict(&colentry->di_tv);
	إذا ((STRLEN(colstr) == 7) && (*colstr == '#'))
	{
	    أرجع decode_hex_color(colstr);
	}
	وإلا
	{
	    semsg(_(e_bad_color_string_str), colstr);
	    أرجع INVALCOLOR;
	}
    }

    إذا (colentry->di_tv.v_type == VAR_نمبر)
    {
	colnum = tv_get_نمبر(&colentry->di_tv);
	أرجع (guicolor_T)colnum;
    }

    أرجع INVALCOLOR;
}

# endif

    guicolor_T
gui_get_color_cmn(char_u *name)
{
    guicolor_T  color;
    // Only non X11 colors (not present in rgb.txt) and colors in
    // color_name_tab[], useful when $VIMRUNTIME is not found,.
    // must be sorted by the 'value' field because it is used by bsearch()!
    ساكن keyvalue_T rgb_tab[] = {
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
    keyvalue_T نشانہ;
    keyvalue_T *اندراج;

    color = decode_hex_color(name);
    إذا (color != INVALCOLOR)
	أرجع color;

    نشانہ.کلید = 0;
    نشانہ.قدر.string = name;
    نشانہ.قدر.لمائی = 0;	// not used, see cmp_keyvalue_value_i()
    اندراج = (keyvalue_T *)bsearch(&نشانہ, &rgb_tab, ARRAY_لمائی(rgb_tab),
	    حجم(rgb_tab[0]), cmp_keyvalue_قدر_i);
    إذا (اندراج != عدم)
	أرجع gui_adjust_rgb((guicolor_T)اندراج->کلید);

# إذا defined(FEAT_EVAL)
    /*
     * Not a traditional color. Load additional color aliases and then consult the alias table.
     */

    color = colorname2rgb(name);
    إذا (color == INVALCOLOR)
    {
	لدو_default_colors_lists();
	color = colorname2rgb(name);
    }

    أرجع color;
# وإلا
    أرجع INVALCOLOR;
# endif
}

    guicolor_T
gui_get_rgb_color_cmn(صحيح r, صحيح g, صحيح b)
{
    guicolor_T  color = RGB(r, g, b);

    إذا (color > 0xffffff)
	أرجع INVALCOLOR;
    أرجع gui_adjust_rgb(color);
}
#جے_انت

/*
 * Table with the specifications for an attribute number.
 * Note that this table is used by ALL buffers.  This is required because the
 * GUI can redraw at any time for any buffer.
 */
ساكن garray_T	رکن_attr_جدول = {0, 0, 0, 0, عدم};

#تعریف_کرو رکن_ATTR_اندراج(idx) ((attrentry_T *)رکن_attr_جدول.ga_انکڑے)[idx]

ساكن garray_T	cterm_attr_جدول = {0, 0, 0, 0, عدم};

#تعریف_کرو CTERM_ATTR_اندراج(idx) ((attrentry_T *)cterm_attr_جدول.ga_انکڑے)[idx]

#جے_تعریف_شدہ FEAT_GUI
ساكن garray_T	gui_attr_جدول = {0, 0, 0, 0, عدم};

# define GUI_ATTR_اندراج(idx) ((attrentry_T *)gui_attr_جدول.ga_انکڑے)[idx]
#جے_انت

/*
 * Return the attr number for a set of colors and font.
 * Add a new entry to the term_attr_table, cterm_attr_table or gui_attr_table
 * if the combination is new.
 * Return 0 for error (no more room).
 */
    ساكن صحيح
get_attr_اندراج(garray_T *جدول, attrentry_T *aep)
{
    صحيح		i;
    attrentry_T	*taep;
    ساكن صحيح	recursive = FALSE;

    // Init the table, in case it wasn't done yet.
    جدول->ga_itemsize = حجم(attrentry_T);
    جدول->ga_growsize = 7;

    // Try to find an entry with the same specifications.
    لكل (i = 0; i < جدول->ga_len; ++i)
    {
	taep = &(((attrentry_T *)جدول->ga_انکڑے)[i]);
	إذا (	   aep->ae_attr == taep->ae_attr
		&& (
#جے_تعریف_شدہ FEAT_GUI
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
#جے_انت
		       (جدول == &رکن_attr_جدول
			&& (aep->ae_u.رکن.شروع == عدم)
					    == (taep->ae_u.رکن.شروع == عدم)
			&& (aep->ae_u.رکن.شروع == عدم
			    || STRCMP(aep->ae_u.رکن.شروع,
						  taep->ae_u.رکن.شروع) == 0)
			&& (aep->ae_u.رکن.stop == عدم)
					     == (taep->ae_u.رکن.stop == عدم)
			&& (aep->ae_u.رکن.stop == عدم
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
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
			    && aep->ae_u.cterm.fg_rgb
						    == taep->ae_u.cterm.fg_rgb
			    && aep->ae_u.cterm.bg_rgb
						    == taep->ae_u.cterm.bg_rgb
			    && aep->ae_u.cterm.ul_rgb
						    == taep->ae_u.cterm.ul_rgb
#جے_انت
		       )))

	أرجع i + ATTR_OFF;
    }

    إذا (جدول->ga_len + ATTR_OFF > MAX_TYPENR)
    {
	// Running out of attribute entries!  remove all attributes, and
	// compute new ones for all groups.
	// When called recursively, we are really out of numbers.
	إذا (recursive)
	{
	    emsg(_(e_too_many_different_highlighting_گن_وچ_use));
	    أرجع 0;
	}
	recursive = TRUE;

	clear_hl_tables();

	سیٹ_must_redraw(UPD_CLEAR);

	لكل (i = 0; i < highlight_ga.ga_len; ++i)
	    سیٹ_hl_attr(i);

	recursive = FALSE;
    }

    // This is a new combination of colors and font, add an entry.
    إذا (ga_grow(جدول, 1) == FAIL)
	أرجع 0;

    taep = &(((attrentry_T *)جدول->ga_انکڑے)[جدول->ga_len]);
    CLEAR_اشارہ_گر(taep);
    taep->ae_attr = aep->ae_attr;
#جے_تعریف_شدہ FEAT_GUI
    إذا (جدول == &gui_attr_جدول)
    {
	taep->ae_u.gui.fg_color = aep->ae_u.gui.fg_color;
	taep->ae_u.gui.bg_color = aep->ae_u.gui.bg_color;
	taep->ae_u.gui.sp_color = aep->ae_u.gui.sp_color;
	taep->ae_u.gui.font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
	taep->ae_u.gui.fontset = aep->ae_u.gui.fontset;
# endif
    }
#جے_انت
    إذا (جدول == &رکن_attr_جدول)
    {
	إذا (aep->ae_u.رکن.شروع == عدم)
	    taep->ae_u.رکن.شروع = عدم;
	وإلا
	    taep->ae_u.رکن.شروع = vim_strsave(aep->ae_u.رکن.شروع);
	إذا (aep->ae_u.رکن.stop == عدم)
	    taep->ae_u.رکن.stop = عدم;
	وإلا
	    taep->ae_u.رکن.stop = vim_strsave(aep->ae_u.رکن.stop);
    }
    وإلا إذا (جدول == &cterm_attr_جدول)
    {
	taep->ae_u.cterm.fg_color = aep->ae_u.cterm.fg_color;
	taep->ae_u.cterm.bg_color = aep->ae_u.cterm.bg_color;
	taep->ae_u.cterm.ul_color = aep->ae_u.cterm.ul_color;
	taep->ae_u.cterm.font = aep->ae_u.cterm.font;
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
	taep->ae_u.cterm.fg_rgb = aep->ae_u.cterm.fg_rgb;
	taep->ae_u.cterm.bg_rgb = aep->ae_u.cterm.bg_rgb;
	taep->ae_u.cterm.ul_rgb = aep->ae_u.cterm.ul_rgb;
#جے_انت
    }
    ++جدول->ga_len;
    أرجع (جدول->ga_len - 1 + ATTR_OFF);
}

#إذا defined(FEAT_TERMINAL)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    صحيح
get_cterm_attr_idx(صحيح attr, صحيح fg, صحيح bg)
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
    أرجع get_attr_اندراج(&cterm_attr_جدول, &at_en);
}
#جے_انت

#إذا defined(FEAT_TERMINAL) && defined(FEAT_TERMGUICOLORS)
/*
 * Get an attribute index for a 'termguicolors' entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    صحيح
get_tgc_attr_idx(صحيح attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_میدان(at_en);
    at_en.ae_attr = attr;
    إذا (fg == INVALCOLOR && bg == INVALCOLOR)
    {
	// If both GUI colors are not set fall back to the cterm colors.  Helps
	// if the GUI only has an attribute, such as undercurl.
	at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
    }
    وإلا
    {
	at_en.ae_u.cterm.fg_rgb = fg;
	at_en.ae_u.cterm.bg_rgb = bg;
    }
    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
    أرجع get_attr_اندراج(&cterm_attr_جدول, &at_en);
}
#جے_انت

#إذا defined(FEAT_TERMINAL) && defined(FEAT_GUI)
/*
 * Get an attribute index for a cterm entry.
 * Uses an existing entry when possible or adds one when needed.
 */
    صحيح
get_gui_attr_idx(صحيح attr, guicolor_T fg, guicolor_T bg)
{
    attrentry_T		at_en;

    CLEAR_میدان(at_en);
    at_en.ae_attr = attr;
    at_en.ae_u.gui.fg_color = fg;
    at_en.ae_u.gui.bg_color = bg;
    أرجع get_attr_اندراج(&gui_attr_جدول, &at_en);
}
#جے_انت

/*
 * Clear all highlight tables.
 */
    فراغ
clear_hl_tables(فراغ)
{
    صحيح		i;
    attrentry_T	*taep;

#جے_تعریف_شدہ FEAT_GUI
    ga_clear(&gui_attr_جدول);
#جے_انت
    لكل (i = 0; i < رکن_attr_جدول.ga_len; ++i)
    {
	taep = &(((attrentry_T *)رکن_attr_جدول.ga_انکڑے)[i]);
	vim_free(taep->ae_u.رکن.شروع);
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
    صحيح
hl_combine_attr(صحيح char_attr, صحيح prim_attr)
{
    attrentry_T *char_aep = عدم;
    attrentry_T *prim_aep;
    attrentry_T new_en;

    إذا (char_attr == 0)
	أرجع prim_attr;
    إذا (char_attr <= HL_ALL && prim_attr <= HL_ALL)
	أرجع ATTR_COMBINE(char_attr, prim_attr);
#جے_تعریف_شدہ FEAT_GUI
    إذا (gui.وچ_use)
    {
	إذا (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	إذا (char_aep != عدم)
	    new_en = *char_aep;
	وإلا
	{
	    CLEAR_میدان(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    إذا (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	إذا (prim_attr <= HL_ALL)
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	وإلا
	{
	    prim_aep = syn_gui_attr2entry(prim_attr);
	    إذا (prim_aep != عدم)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		إذا (prim_aep->ae_u.gui.fg_color != INVALCOLOR)
		    new_en.ae_u.gui.fg_color = prim_aep->ae_u.gui.fg_color;
		إذا (prim_aep->ae_u.gui.bg_color != INVALCOLOR)
		    new_en.ae_u.gui.bg_color = prim_aep->ae_u.gui.bg_color;
		إذا (prim_aep->ae_u.gui.sp_color != INVALCOLOR)
		    new_en.ae_u.gui.sp_color = prim_aep->ae_u.gui.sp_color;
		إذا (prim_aep->ae_u.gui.font != NOFONT)
		    new_en.ae_u.gui.font = prim_aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
		إذا (prim_aep->ae_u.gui.fontset != NOFONTSET)
		    new_en.ae_u.gui.fontset = prim_aep->ae_u.gui.fontset;
# endif
	    }
	}
	أرجع get_attr_اندراج(&gui_attr_جدول, &new_en);
    }
#جے_انت

    إذا (IS_CTERM)
    {
	إذا (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	إذا (char_aep != عدم)
	    new_en = *char_aep;
	وإلا
	{
	    CLEAR_میدان(new_en);
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#جے_انت
	    إذا (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	إذا (prim_attr <= HL_ALL)
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
	وإلا
	{
	    prim_aep = syn_cterm_attr2entry(prim_attr);
	    إذا (prim_aep != عدم)
	    {
		new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr,
							   prim_aep->ae_attr);
		إذا (prim_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color = prim_aep->ae_u.cterm.fg_color;
		إذا (prim_aep->ae_u.cterm.bg_color > 0)
		    new_en.ae_u.cterm.bg_color = prim_aep->ae_u.cterm.bg_color;
		إذا (prim_aep->ae_u.cterm.ul_color > 0)
		    new_en.ae_u.cterm.ul_color = prim_aep->ae_u.cterm.ul_color;
		إذا (prim_aep->ae_u.cterm.font > 0)
		    new_en.ae_u.cterm.font = prim_aep->ae_u.cterm.font;
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
		// If both fg and bg are not set fall back to cterm colors.
		// Helps for SpellBad which uses undercurl in the GUI.
		إذا (COLOR_INVALID(prim_aep->ae_u.cterm.fg_rgb)
			&& COLOR_INVALID(prim_aep->ae_u.cterm.bg_rgb))
		{
		    إذا (prim_aep->ae_u.cterm.fg_color > 0)
			new_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
		    إذا (prim_aep->ae_u.cterm.bg_color > 0)
			new_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
		}
		وإلا
		{
		    إذا (prim_aep->ae_u.cterm.fg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.fg_rgb = prim_aep->ae_u.cterm.fg_rgb;
		    إذا (prim_aep->ae_u.cterm.bg_rgb != INVALCOLOR)
			new_en.ae_u.cterm.bg_rgb = prim_aep->ae_u.cterm.bg_rgb;
		}
		إذا (prim_aep->ae_u.cterm.ul_rgb != INVALCOLOR)
		    new_en.ae_u.cterm.ul_rgb = prim_aep->ae_u.cterm.ul_rgb;
#جے_انت
	    }
	}
	أرجع get_attr_اندراج(&cterm_attr_جدول, &new_en);
    }

    إذا (char_attr > HL_ALL)
	char_aep = syn_رکن_attr2entry(char_attr);
    إذا (char_aep != عدم)
	new_en = *char_aep;
    وإلا
    {
	CLEAR_میدان(new_en);
	إذا (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    إذا (prim_attr <= HL_ALL)
	new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_attr);
    وإلا
    {
	prim_aep = syn_رکن_attr2entry(prim_attr);
	إذا (prim_aep != عدم)
	{
	    new_en.ae_attr = ATTR_COMBINE(new_en.ae_attr, prim_aep->ae_attr);
	    إذا (prim_aep->ae_u.رکن.شروع != عدم)
	    {
		new_en.ae_u.رکن.شروع = prim_aep->ae_u.رکن.شروع;
		new_en.ae_u.رکن.stop = prim_aep->ae_u.رکن.stop;
	    }
	}
    }
    أرجع get_attr_اندراج(&رکن_attr_جدول, &new_en);
}

// ANSI color order: Black, Red, Green, Yellow, Blue, Magenta,
// Cyan, White, then bright variants.  Approximate RGB values used when
// only a cterm color number is known (no guifg/guibg).  Real terminal
// palettes may differ if the user reconfigured their emulator, but
// these are reasonable xterm-ish defaults.
ساكن ثابت long_u cterm_color_16[16] = {
    0x000000, 0xc00000, 0x008000, 0x808000,
    0x0000c0, 0xc000c0, 0x004080, 0xc0c0c0,
    0x808080, 0xff8080, 0x00ff00, 0xffff00,
    0x6060ff, 0xff40ff, 0x00ffff, 0xffffff
};

#جے_تعریف_شدہ FEAT_TERMGUICOLORS
/*
 * Convert a cterm color number (1-16) to an RGB value.
 * Used as a fallback when 'termguicolors' is set but only cterm colors are
 * specified (no guifg/guibg).
 * Returns INVALCOLOR if the color number is out of range.
 */
    ساكن guicolor_T
cterm_color_منزل_rgb(صحيح color_nr)
{
    إذا (color_nr < 1 || color_nr > 16)
	أرجع INVALCOLOR;
    أرجع (guicolor_T)cterm_color_16[color_nr - 1];
}
#جے_انت

/*
 * Convert an xterm 256-color index (0-255) to an approximate RGB triple.
 * Uses the standard xterm palette: 0-15 ANSI (cterm_color_16), 16-231
 * 6x6x6 cube, 232-255 grayscale ramp.
 */
    ساكن فراغ
cterm_idx_منزل_rgb(صحيح idx, صحيح *r, صحيح *g, صحيح *b)
{
    ساكن ثابت صحيح cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };

    إذا (idx < 0 || idx > 255)
    {
	*r = *g = *b = 0;
	أرجع;
    }
    إذا (idx < 16)
    {
	long_u rgb = cterm_color_16[idx];
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
    }
    وإلا إذا (idx < 232)
    {
	صحيح n = idx - 16;
	*r = cube[n / 36 % 6];
	*g = cube[n / 6  % 6];
	*b = cube[n      % 6];
    }
    وإلا
    {
	صحيح v = 8 + (idx - 232) * 10;
	*r = *g = *b = v;
    }
}

/*
 * Approximate an RGB triple to the nearest xterm 256-color index.
 * Searches the 6x6x6 cube and the grayscale ramp; returns 0-255.
 */
    ساكن صحيح
rgb_منزل_cterm_idx(صحيح r, صحيح g, صحيح b)
{
    ساكن ثابت صحيح cube[] = { 0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF };
    صحيح بہترین = 0;
    لما بہترین_d = -1;
    صحيح i;

    // Search the 6x6x6 cube.
    لكل (i = 16; i < 232; ++i)
    {
	صحيح n = i - 16;
	صحيح cr = cube[n / 36 % 6];
	صحيح cg = cube[n / 6  % 6];
	صحيح cb = cube[n      % 6];
	لما d = (cr - r) * (cr - r) + (cg - g) * (cg - g) + (cb - b) * (cb - b);
	إذا (بہترین_d < 0 || d < بہترین_d)
	{
	    بہترین_d = d;
	    بہترین = i;
	}
    }
    // Search the grayscale ramp.
    لكل (i = 232; i < 256; ++i)
    {
	صحيح v = 8 + (i - 232) * 10;
	لما d = (v - r) * (v - r) + (v - g) * (v - g) + (v - b) * (v - b);
	إذا (d < بہترین_d)
	{
	    بہترین_d = d;
	    بہترین = i;
	}
    }
    أرجع بہترین;
}

/*
 * Resolve a color to an RGB triple.  Prefer the gui RGB value (set by
 * guifg/guibg) when valid, otherwise convert the cterm 256-color index.
 * Returns true on success; false when no color is defined.
 * cterm_c is 1-based (0 means "no color").
 */
    ساكن bool
resolve_color_منزل_rgb(صحيح cterm_c, guicolor_T rgb بن_ورتیا, صحيح *r, صحيح *g, صحيح *b)
{
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
    إذا (!COLOR_INVALID(rgb))
    {
	*r = (rgb >> 16) & 0xFF;
	*g = (rgb >> 8) & 0xFF;
	*b = rgb & 0xFF;
	أرجع true;
    }
#جے_انت
    إذا (cterm_c > 0)
    {
	cterm_idx_منزل_rgb(cterm_c - 1, r, g, b);
	أرجع true;
    }
    أرجع false;
}

/*
 * get a RGB fallback color from gui, cterm or default color
 */
    ساكن guicolor_T
resolve_fallback_color(صحيح cterm_c, guicolor_T rgb, guicolor_T default_rgb)
{
    صحيح red, green, blue;
    إذا (!resolve_color_منزل_rgb(cterm_c, rgb, &red, &green, &blue))
	أرجع default_rgb;
    وإلا
	أرجع (red << 16) | (green << 8) | blue;
}

/*
 * get a RGB fallback foreground color from guifg, ctermfg or deduced from background
 */
    ساكن فراغ
resolve_fallback_fg_منزل_rgb(فراغ)
{
    guicolor_T fgcolor_or_gui_fgcolor = INVALCOLOR;
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
    fgcolor_or_gui_fgcolor = cterm_عام_fg_gui_color;
#جے_انت
#جے_تعریف_شدہ FEAT_GUI
    إذا (gui.وچ_use)
	fgcolor_or_gui_fgcolor = gui.norm_pixel;
#جے_انت
    fallback_fg_rgb = resolve_fallback_color(cterm_عام_fg_color, fgcolor_or_gui_fgcolor, (*p_bg == 'l') ? 0x000000 : 0xFFFFFF);
}

/*
 * get a RGB fallback background color from guifg, ctermbg or deduced from background
 */
    ساكن فراغ
resolve_fallback_bg_منزل_rgb(فراغ)
{
    guicolor_T bgcolor_or_gui_bgcolor = INVALCOLOR;
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
    bgcolor_or_gui_bgcolor = cterm_عام_bg_gui_color;
#جے_انت
#جے_تعریف_شدہ FEAT_GUI
    إذا (gui.وچ_use)
	bgcolor_or_gui_bgcolor = gui.پٹھ_pixel;
#جے_انت
    fallback_bg_rgb = resolve_fallback_color(cterm_عام_bg_color, bgcolor_or_gui_bgcolor, (*p_bg == 'l') ? 0xFFFFFF : 0x000000);
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
    ساكن صحيح
blend_cterm_colors(صحيح popup_c, guicolor_T popup_rgb,
		   صحيح under_c, guicolor_T under_rgb,
		   صحيح default_rgb,
		   صحيح blend_val)
{
    صحيح pr, pg, pb, ur, ug, ub, r, g, b;

    إذا (t_colors < 256)
	أرجع popup_c;
    إذا (!resolve_color_منزل_rgb(popup_c, popup_rgb, &pr, &pg, &pb))
	أرجع under_c;
    إذا (blend_val <= 0)
	أرجع rgb_منزل_cterm_idx(pr, pg, pb) + 1;
    إذا (!resolve_color_منزل_rgb(under_c, under_rgb, &ur, &ug, &ub))
    {
	ur = (default_rgb >> 16) & 0xFF;
	ug = (default_rgb >> 8) & 0xFF;
	ub = default_rgb & 0xFF;
    }
    إذا (blend_val >= 100)
	أرجع rgb_منزل_cterm_idx(ur, ug, ub) + 1;
    r = pr + (ur - pr) * blend_val / 100;
    g = pg + (ug - pg) * blend_val / 100;
    b = pb + (ub - pb) * blend_val / 100;
    أرجع rgb_منزل_cterm_idx(r, g, b) + 1;
}

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Blend two RGB colors based on blend value (0-100).
 * blend: 0=return first argument color, 100=return second argument color
 * If bg_color is INVALCOLOR, high blend means more visible (return INVALCOLOR).
 */
    ساكن guicolor_T
blend_colors(guicolor_T popup_color, guicolor_T bg_color, صحيح blend_val)
{
    صحيح r1, g1, b1, r2, g2, b2, r, g, b;

    // CTERMCOLOR is a sentinel meaning "use the cterm color"; for blending
    // it has no real RGB so treat it like INVALCOLOR.
    إذا (COLOR_INVALID(popup_color))
	أرجع INVALCOLOR;

    // Fully transparent: use underlying color as-is.
    إذا (blend_val >= 100)
	أرجع bg_color;

    r1 = (popup_color >> 16) & 0xFF;
    g1 = (popup_color >> 8) & 0xFF;
    b1 = popup_color & 0xFF;

    إذا (COLOR_INVALID(bg_color))
	bg_color = fallback_bg_rgb;

    r2 = (bg_color >> 16) & 0xFF;
    g2 = (bg_color >> 8) & 0xFF;
    b2 = bg_color & 0xFF;

    r = r1 + (r2 - r1) * blend_val / 100;
    g = g1 + (g2 - g1) * blend_val / 100;
    b = b1 + (b2 - b1) * blend_val / 100;

    أرجع (r << 16) | (g << 8) | b;
}
#جے_انت

/*
 * Blend attributes for popup windows and menus with opacity.
 * Blends foreground and/or background colors based on blend value (0-100).
 * blend: 0 = opaque (use popup colors), 100 = transparent (use background colors)
 * blend_fg: TRUE to blend foreground color, FALSE to keep popup foreground
 */
    ساكن صحيح
hl_blend_attr_common(
    صحيح		char_attr,
    صحيح		popup_attr,
    صحيح		blend,
    صحيح		blend_fg)
{
    attrentry_T *char_aep = عدم;
    attrentry_T *popup_aep;
    attrentry_T new_en;
    attrentry_T tmp_en;

#جے_تعریف_شدہ FEAT_GUI
    إذا (gui.وچ_use)
    {
	إذا (char_attr > HL_ALL)
	    char_aep = syn_gui_attr2entry(char_attr);
	إذا (char_aep != عدم)
	    new_en = *char_aep;
	وإلا
	{
	    CLEAR_میدان(new_en);
	    new_en.ae_u.gui.fg_color = INVALCOLOR;
	    new_en.ae_u.gui.bg_color = INVALCOLOR;
	    new_en.ae_u.gui.sp_color = INVALCOLOR;
	    إذا (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	إذا (popup_attr <= HL_ALL)
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
	وإلا
	    popup_aep = syn_gui_attr2entry(popup_attr);

	إذا (popup_aep != عدم)
	{
	    guicolor_T popup_bg_rgb = popup_aep->ae_u.gui.bg_color;
	    إذا (COLOR_INVALID(popup_bg_rgb))
		popup_bg_rgb = fallback_bg_rgb;

	    إذا (blend_fg)
	    {
		// blend_fg=TRUE: fade underlying text toward popup bg.
		{
		    صحيح ادھار_fg = fallback_fg_rgb;
		    إذا (char_aep != عدم
			    && char_aep->ae_u.gui.fg_color != INVALCOLOR)
			ادھار_fg = char_aep->ae_u.gui.fg_color;
		    new_en.ae_u.gui.fg_color = blend_colors(
			    popup_bg_rgb, ادھار_fg, blend);
		}
	    }
	    وإلا
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
		إذا (char_aep != عدم
			&& !COLOR_INVALID(char_aep->ae_u.gui.bg_color))
		    underlying_bg = char_aep->ae_u.gui.bg_color;
		new_en.ae_u.gui.bg_color = blend_colors(
			popup_bg_rgb,
			underlying_bg, blend);
	    }
	}
	أرجع get_attr_اندراج(&gui_attr_جدول, &new_en);
    }
#جے_انت

    إذا (IS_CTERM)
    {
	إذا (char_attr > HL_ALL)
	    char_aep = syn_cterm_attr2entry(char_attr);
	إذا (char_aep != عدم)
	    new_en = *char_aep;
	وإلا
	{
	    CLEAR_میدان(new_en);
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
	    new_en.ae_u.cterm.bg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    new_en.ae_u.cterm.ul_rgb = INVALCOLOR;
#جے_انت
	    إذا (char_attr <= HL_ALL)
		new_en.ae_attr = char_attr;
	}

	// initialize an empty entry if no highlight set for popup
	إذا (popup_attr <= HL_ALL)
	{
	    CLEAR_میدان(tmp_en);
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
	    tmp_en.ae_u.cterm.fg_rgb = INVALCOLOR;
	    tmp_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	    // allow blending with termguicolors
	    tmp_en.ae_u.cterm.bg_rgb = fallback_bg_rgb;
#جے_انت
	    // preserve attributes other than color
	    tmp_en.ae_attr = popup_attr;
	    popup_aep = &tmp_en;

	    // allow blending with notermguicolors
	    popup_aep->ae_u.cterm.bg_color = cterm_عام_bg_color;
	}
	وإلا
	    popup_aep = syn_cterm_attr2entry(popup_attr);

	إذا (popup_aep != عدم)
	{
	    guicolor_T popup_bg_rgb = INVALCOLOR;
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
	    // Fall back to cterm color converted to RGB when gui color is not set.
	    popup_bg_rgb = popup_aep->ae_u.cterm.bg_rgb;
	    إذا (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color > 0)
		popup_bg_rgb = cterm_color_منزل_rgb(
			popup_aep->ae_u.cterm.bg_color);
#جے_انت
	    // assign default color if guibg and ctermbg are not set for popup
	    إذا (COLOR_INVALID(popup_bg_rgb)
		    && popup_aep->ae_u.cterm.bg_color == 0)
		popup_bg_rgb = fallback_bg_rgb;

	    إذا (!blend_fg)
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
		إذا (popup_aep->ae_u.cterm.fg_color > 0)
		    new_en.ae_u.cterm.fg_color =
			popup_aep->ae_u.cterm.fg_color;
		وإلا إذا (cterm_عام_fg_color > 0)
		    new_en.ae_u.cterm.fg_color = cterm_عام_fg_color;
		وإلا
		    // black-ish or white-ish
		    new_en.ae_u.cterm.fg_color = (*p_bg == 'l') ? 1 : 16;
		new_en.ae_u.cterm.ul_color = popup_aep->ae_u.cterm.ul_color;
		new_en.ae_u.cterm.font = popup_aep->ae_u.cterm.font;
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
		new_en.ae_u.cterm.ul_rgb = popup_aep->ae_u.cterm.ul_rgb;
#جے_انت
	    }
	    وإلا
	    {
		// blend_fg=TRUE: fade underlying fg toward popup bg in
		// the 256-color palette.  Used when the popup is over a
		// cell rendered with cterm colors (no termguicolors RGB).
		صحيح under_fg = (char_aep != عدم)
		    ? char_aep->ae_u.cterm.fg_color : 0;
		guicolor_T under_fg_rgb = INVALCOLOR;
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
		إذا (char_aep != عدم)
		    under_fg_rgb = char_aep->ae_u.cterm.fg_rgb;
#جے_انت
		new_en.ae_u.cterm.fg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_fg, under_fg_rgb, fallback_fg_rgb, blend);
	    }
	    // Approximate cterm bg by blending with the underlying bg
	    // in the 256-color palette and mapping to the nearest entry.
	    {
		صحيح under_bg = (char_aep != عدم)
		    ? char_aep->ae_u.cterm.bg_color : 0;
		guicolor_T under_bg_rgb = INVALCOLOR;
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
		إذا (char_aep != عدم)
		    under_bg_rgb = char_aep->ae_u.cterm.bg_rgb;
#جے_انت
		new_en.ae_u.cterm.bg_color = blend_cterm_colors(
			popup_aep->ae_u.cterm.bg_color, popup_bg_rgb,
			under_bg, under_bg_rgb, fallback_bg_rgb, blend);
	    }
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
	    // Blend RGB colors for termguicolors mode.
	    {
		إذا (blend_fg)
		{
		    // blend_fg=TRUE: fade underlying text toward popup bg.
		    إذا (popup_bg_rgb != INVALCOLOR)
		    {
			صحيح ادھار_fg = fallback_fg_rgb;
			// CTERMCOLOR is a sentinel meaning "use the cterm
			// color"; treat it as no underlying color so it is
			// not blended in as a real near-white pixel.
			إذا (char_aep != عدم
				&& !COLOR_INVALID(char_aep->ae_u.cterm.fg_rgb))
			    ادھار_fg = char_aep->ae_u.cterm.fg_rgb;
			new_en.ae_u.cterm.fg_rgb = blend_colors(
				popup_bg_rgb, ادھار_fg, blend);
		    }
		}
		وإلا
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

		    إذا (COLOR_INVALID(popup_fg)
			    && popup_aep->ae_u.cterm.fg_color > 0)
			popup_fg = cterm_color_منزل_rgb(
				popup_aep->ae_u.cterm.fg_color);

		    إذا (!COLOR_INVALID(popup_fg))
			new_en.ae_u.cterm.fg_rgb = popup_fg;
		    وإلا إذا (!COLOR_INVALID(cterm_عام_fg_gui_color))
			new_en.ae_u.cterm.fg_rgb = cterm_عام_fg_gui_color;
		    وإلا
			new_en.ae_u.cterm.fg_rgb = fallback_fg_rgb;
		}
		إذا (popup_bg_rgb != INVALCOLOR)
		{
		    // Blend popup bg toward underlying bg
		    guicolor_T underlying_bg = fallback_bg_rgb;
		    إذا (char_aep != عدم
			    && !COLOR_INVALID(char_aep->ae_u.cterm.bg_rgb))
			underlying_bg = char_aep->ae_u.cterm.bg_rgb;
		    new_en.ae_u.cterm.bg_rgb = blend_colors(
			    popup_bg_rgb, underlying_bg, blend);
		}
	    }
#جے_انت
	}
	أرجع get_attr_اندراج(&cterm_attr_جدول, &new_en);
    }

    إذا (char_attr > HL_ALL)
	char_aep = syn_رکن_attr2entry(char_attr);
    إذا (char_aep != عدم)
	new_en = *char_aep;
    وإلا
    {
	CLEAR_میدان(new_en);
	إذا (char_attr <= HL_ALL)
	    new_en.ae_attr = char_attr;
    }

    // For term mode, no separate background color handling.
    أرجع get_attr_اندراج(&رکن_attr_جدول, &new_en);
}

/*
 * Blend attributes for popup windows with opacity.
 */
    صحيح
hl_blend_attr(صحيح char_attr, صحيح popup_attr, صحيح blend, صحيح blend_fg)
{
    إذا (blend >= 100 && blend_fg)
	أرجع char_attr;

    أرجع hl_blend_attr_common(char_attr, popup_attr, blend, blend_fg);
}

/*
 * Blend underlying foreground and background colors for pum space cells.
 */
    صحيح
hl_pum_blend_attr(صحيح char_attr, صحيح popup_attr, صحيح blend)
{
    أرجع hl_blend_attr_common(char_attr, popup_attr, blend, TRUE);
}

#جے_تعریف_شدہ FEAT_GUI
    attrentry_T *
syn_gui_attr2entry(صحيح attr)
{
    attr -= ATTR_OFF;
    إذا (attr >= gui_attr_جدول.ga_len)	    // did ":syntax clear"
	أرجع عدم;
    أرجع &(GUI_ATTR_اندراج(attr));
}
#جے_انت

/*
 * Get the highlight attributes (HL_BOLD etc.) from an attribute nr.
 * Only to be used when "attr" > HL_ALL.
 */
    صحيح
syn_attr2attr(صحيح attr)
{
    attrentry_T	*aep;

#جے_تعریف_شدہ FEAT_GUI
    إذا (gui.وچ_use)
	aep = syn_gui_attr2entry(attr);
    وإلا
#جے_انت
	إذا (IS_CTERM)
	    aep = syn_cterm_attr2entry(attr);
	وإلا
	    aep = syn_رکن_attr2entry(attr);

    إذا (aep == عدم)	    // highlighting not set
	أرجع 0;
    أرجع aep->ae_attr;
}


    attrentry_T *
syn_رکن_attr2entry(صحيح attr)
{
    attr -= ATTR_OFF;
    إذا (attr >= رکن_attr_جدول.ga_len)	    // did ":syntax clear"
	أرجع عدم;
    أرجع &(رکن_ATTR_اندراج(attr));
}

    attrentry_T *
syn_cterm_attr2entry(صحيح attr)
{
    attr -= ATTR_OFF;
    إذا (attr >= cterm_attr_جدول.ga_len)	// did ":syntax clear"
	أرجع عدم;
    أرجع &(CTERM_ATTR_اندراج(attr));
}

#تعریف_کرو LIST_ATTR   1
#تعریف_کرو LIST_STRING 2
#تعریف_کرو LIST_INT    3

    ساكن فراغ
highlight_list_one(صحيح id)
{
    hl_گروہ_T	    *sgp;
    صحيح		    didh = FALSE;

    sgp = &HL_جدول()[id - 1];	    // index is ID minus one

    إذا (سنیہا_filtered(sgp->sg_name))
	أرجع;

    // Note: Keep this in sync with expand_highlight_group().
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_رکن, عدم, "term");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_شروع, "start");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_stop, "stop");

    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_cterm, عدم, "cterm");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_fg, عدم, "ctermfg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_bg, عدم, "ctermbg");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_ul, عدم, "ctermul");
    didh = highlight_list_arg(id, didh, LIST_INT,
				    sgp->sg_cterm_font, عدم, "ctermfont");

#إذا defined(FEAT_GUI) || defined(FEAT_EVAL)
    didh = highlight_list_arg(id, didh, LIST_ATTR,
				    sgp->sg_gui, عدم, "gui");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_fg_name, "guifg");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_bg_name, "guibg");
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_gui_sp_name, "guisp");
#جے_انت
#جے_تعریف_شدہ FEAT_GUI
    didh = highlight_list_arg(id, didh, LIST_STRING,
				    0, sgp->sg_font_name, "font");
#جے_انت

    إذا (sgp->sg_link && !got_int)
    {
	(فراغ)syn_list_سرخی(didh, 9999, id);
	didh = TRUE;
	پیغام_puts_attr("links to", HL_ATTR(HLF_D));
	پیغام_putchar(' ');
	پیغام_outtrans(HL_جدول()[HL_جدول()[id - 1].sg_link - 1].sg_name);
    }

    إذا (!didh)
	highlight_list_arg(id, didh, LIST_STRING, 0, (char_u *)"cleared", "");
#جے_تعریف_شدہ FEAT_EVAL
    إذا (p_verbose > 0)
	آخری_سیٹ_پیغام(sgp->sg_script_ctx);
#جے_انت
}

    ساكن char_u*
highlight_arg_منزل_string(
    صحيح		type,
    صحيح		iarg,
    char_u	*sarg,
    char_u	*buf)
{
    إذا (type == LIST_INT)
	sprintf((حرف *)buf, "%d", iarg - 1);
    وإلا إذا (type == LIST_STRING)
	أرجع sarg;
    وإلا // type == LIST_ATTR
    {
	size_t buflen;

	buf[0] = NUL;
	buflen = 0;
	لكل (صحيح i = 0; i < (صحيح)ARRAY_لمائی(highlight_اشاریہ_tab); ++i)
	{
	    إذا (iarg & highlight_اشاریہ_tab[i]->کلید)
	    {
		إذا (buflen > 0)
		{
		    STRCPY(buf + buflen, (char_u *)",");
		    ++buflen;
		}
		STRCPY(buf + buflen, highlight_اشاریہ_tab[i]->قدر.string);
		buflen += highlight_اشاریہ_tab[i]->قدر.لمائی;
		iarg &= ~highlight_اشاریہ_tab[i]->کلید;	    // don't want "inverse"/"reverse"
	    }
	}
    }
    أرجع buf;
}

    ساكن صحيح
highlight_list_arg(
    صحيح		id,
    صحيح		didh,
    صحيح		type,
    صحيح		iarg,
    char_u	*sarg,
    حرف	*name)
{
    char_u	buf[MAX_ATTR_LEN];
    char_u	*ts;

    إذا (got_int)
	أرجع FALSE;

    إذا (type == LIST_STRING ? (sarg == عدم) : (iarg == 0))
	أرجع didh;

    ts = highlight_arg_منزل_string(type, iarg, sarg, buf);

    (فراغ)syn_list_سرخی(didh,
	    (صحيح)(vim_strsize(ts) + STRLEN(name) + 1), id);
    didh = TRUE;
    إذا (!got_int)
    {
	إذا (*name != NUL)
	{
	    پیغام_puts_attr(name, HL_ATTR(HLF_D));
	    پیغام_puts_attr("=", HL_ATTR(HLF_D));
	}
	پیغام_outtrans(ts);
    }
    أرجع didh;
}

#إذا ((defined(FEAT_EVAL) || defined(FEAT_PRINTER))) && defined(FEAT_SYN_HL)
/*
 * Return "1" if highlight group "id" has attribute "flag".
 * Return NULL otherwise.
 */
    char_u *
highlight_has_attr(
    صحيح		id,
    صحيح		flag,
    صحيح		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    صحيح		attr;

    إذا (id <= 0 || id > highlight_ga.ga_len)
	أرجع عدم;

# إذا defined(FEAT_GUI) || defined(FEAT_EVAL)
    إذا (modec == 'g')
	attr = HL_جدول()[id - 1].sg_gui;
    وإلا
# endif
    {
	إذا (modec == 'c')
	    attr = HL_جدول()[id - 1].sg_cterm;
	وإلا
	    attr = HL_جدول()[id - 1].sg_رکن;
    }

    إذا (attr & flag)
	أرجع (char_u *)"1";
    أرجع عدم;
}
#جے_انت

#إذا defined(FEAT_SYN_HL) && defined(FEAT_EVAL)
/*
 * Return color name of highlight group "id".
 */
    char_u *
highlight_color(
    صحيح		id,
    char_u	*what,	// "font", "fg", "bg", "sp", "ul", "fg#", "bg#" or "sp#"
    صحيح		modec)	// 'g' for GUI, 'c' for cterm, 't' for term
{
    ساكن char_u	name[20];
    صحيح			n;
    صحيح			fg = FALSE;
    صحيح			sp = FALSE;
    صحيح			ul = FALSE;
    صحيح			font = FALSE;

    إذا (id <= 0 || id > highlight_ga.ga_len)
	أرجع عدم;

    إذا (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'g')
	fg = TRUE;
    وإلا إذا (TOLOWER_ASC(what[0]) == 'f' && TOLOWER_ASC(what[1]) == 'o'
	     && TOLOWER_ASC(what[2]) == 'n' && TOLOWER_ASC(what[3]) == 't')
	font = TRUE;
    وإلا إذا (TOLOWER_ASC(what[0]) == 's' && TOLOWER_ASC(what[1]) == 'p')
	sp = TRUE;
    وإلا إذا (TOLOWER_ASC(what[0]) == 'u' && TOLOWER_ASC(what[1]) == 'l')
	ul = TRUE;
    وإلا إذا (!(TOLOWER_ASC(what[0]) == 'b' && TOLOWER_ASC(what[1]) == 'g'))
	أرجع عدم;
    إذا (modec == 'g')
    {
# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
#  ifdef FEAT_GUI
	// return font name
	إذا (font)
	    أرجع HL_جدول()[id - 1].sg_font_name;
#  endif

	// return #RRGGBB form (only possible when GUI is running)
	إذا ((USE_24_ثنائی_ہندسہ) && what[2] == '#')
	{
	    guicolor_T		color;
	    long_u		rgb;
	    ساكن char_u	buf[10];

	    إذا (fg)
		color = HL_جدول()[id - 1].sg_gui_fg;
	    وإلا إذا (sp)
		color = HL_جدول()[id - 1].sg_gui_sp;
	    وإلا
		color = HL_جدول()[id - 1].sg_gui_bg;
	    إذا (color == INVALCOLOR)
		أرجع عدم;
	    rgb = (long_u)GUI_MCH_GET_RGB(color);
	    sprintf((حرف *)buf, "#%02x%02x%02x",
				      (بنانشانی)(rgb >> 16),
				      (بنانشانی)(rgb >> 8) & 255,
				      (بنانشانی)rgb & 255);
	    أرجع buf;
	}
# endif
	إذا (fg)
	    أرجع (HL_جدول()[id - 1].sg_gui_fg_name);
	إذا (sp)
	    أرجع (HL_جدول()[id - 1].sg_gui_sp_name);
	أرجع (HL_جدول()[id - 1].sg_gui_bg_name);
    }
    إذا (sp)
	أرجع عدم;
    إذا (modec == 'c')
    {
	إذا (fg)
	    n = HL_جدول()[id - 1].sg_cterm_fg - 1;
	وإلا إذا (ul)
	    n = HL_جدول()[id - 1].sg_cterm_ul - 1;
	وإلا إذا (font)
	    n = HL_جدول()[id - 1].sg_cterm_font - 1;
	وإلا
	    n = HL_جدول()[id - 1].sg_cterm_bg - 1;
	إذا (n < 0)
	    أرجع عدم;
	sprintf((حرف *)name, "%d", n);
	أرجع name;
    }
    // term doesn't have color
    أرجع عدم;
}
#جے_انت

#إذا defined(FEAT_SYN_HL) \
	    && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)) \
	&& defined(FEAT_PRINTER)
/*
 * Return color name of highlight group "id" as RGB value.
 */
    long_u
highlight_gui_color_rgb(
    صحيح		id,
    صحيح		fg)	// TRUE = fg, FALSE = bg
{
    guicolor_T	color;

    إذا (id <= 0 || id > highlight_ga.ga_len)
	أرجع 0L;

    إذا (fg)
	color = HL_جدول()[id - 1].sg_gui_fg;
    وإلا
	color = HL_جدول()[id - 1].sg_gui_bg;

    إذا (color == INVALCOLOR)
	أرجع 0L;

    أرجع GUI_MCH_GET_RGB(color);
}
#جے_انت

/*
 * Output the syntax list header.
 * Return TRUE when started a new line.
 */
    صحيح
syn_list_سرخی(
    صحيح	    did_سرخی,		// did header already
    صحيح	    outlen,		// length of string that comes
    صحيح	    id)			// highlight group id
{
    صحيح	    endcol = 19;
    صحيح	    newline = TRUE;
    صحيح	    name_خانہ = 0;

    إذا (!did_سرخی)
    {
	پیغام_putchar('\n');
	إذا (got_int)
	    أرجع TRUE;
	پیغام_outtrans(HL_جدول()[id - 1].sg_name);
	name_خانہ = پیغام_خانہ;
	endcol = 15;
    }
    وإلا إذا (پیغام_خانہ + outlen + 1 >= cmdline_چوڑائی)
    {
	پیغام_putchar('\n');
	إذا (got_int)
	    أرجع TRUE;
    }
    وإلا
    {
	إذا (پیغام_خانہ >= endcol)	// wrap around is like starting a new line
	    newline = FALSE;
    }

    إذا (پیغام_خانہ >= endcol)	// output at least one space
	endcol = پیغام_خانہ + 1;
    إذا (cmdline_چوڑائی <= endcol)	// avoid hang for tiny window
	endcol = cmdline_چوڑائی - 1;

    پیغام_advance(endcol);

    // Show "xxx" with the attributes.
    إذا (!did_سرخی)
    {
	إذا (endcol == cmdline_چوڑائی - 1 && endcol <= name_خانہ)
	    پیغام_putchar(' ');
	پیغام_puts_attr("xxx", syn_id2attr(id));
	پیغام_putchar(' ');
    }

    أرجع newline;
}

/*
 * Set the attribute numbers for a highlight group.
 * Called after one of the attributes has changed.
 */
    ساكن فراغ
سیٹ_hl_attr(
    صحيح		idx)	    // index in array
{
    attrentry_T	    at_en;
    hl_گروہ_T	    *sgp = HL_جدول() + idx;

    // The "Normal" group doesn't need an attribute number
    إذا (sgp->sg_name_u != عدم && STRCMP(sgp->sg_name_u, "NORMAL") == 0)
	أرجع;

#جے_تعریف_شدہ FEAT_GUI
    // For the GUI mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    إذا (sgp->sg_gui_fg == INVALCOLOR
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
    وإلا
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
#جے_انت
    // For the term mode: If there are other than "normal" highlighting
    // attributes, need to allocate an attr number.
    إذا (sgp->sg_شروع == عدم && sgp->sg_stop == عدم)
	sgp->sg_رکن_attr = sgp->sg_رکن;
    وإلا
    {
	at_en.ae_attr = sgp->sg_رکن;
	at_en.ae_u.رکن.شروع = sgp->sg_شروع;
	at_en.ae_u.رکن.stop = sgp->sg_stop;
	sgp->sg_رکن_attr = get_attr_اندراج(&رکن_attr_جدول, &at_en);
    }

    // For the color term mode: If there are other than "normal"
    // highlighting attributes, need to allocate an attr number.
    إذا (sgp->sg_cterm_fg == 0 && sgp->sg_cterm_bg == 0 &&
	sgp->sg_cterm_ul == 0 && sgp->sg_cterm_font == 0
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
	    && sgp->sg_gui_fg == INVALCOLOR
	    && sgp->sg_gui_bg == INVALCOLOR
	    && sgp->sg_gui_sp == INVALCOLOR
#جے_انت
	    )
	sgp->sg_cterm_attr = sgp->sg_cterm;
    وإلا
    {
	at_en.ae_attr = sgp->sg_cterm;
	at_en.ae_u.cterm.fg_color = sgp->sg_cterm_fg;
	at_en.ae_u.cterm.bg_color = sgp->sg_cterm_bg;
	at_en.ae_u.cterm.ul_color = sgp->sg_cterm_ul;
	at_en.ae_u.cterm.font = sgp->sg_cterm_font;
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
	at_en.ae_u.cterm.fg_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_fg);
	at_en.ae_u.cterm.bg_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_bg);
	// Only use the underline/undercurl color when used, it may clear the
	// background color if not supported.
	إذا (sgp->sg_cterm & (HL_UNDERLINE | HL_UNDERCURL
			   | HL_UNDERDOUBLE | HL_UNDERDOTTED | HL_UNDERDASHED))
	    at_en.ae_u.cterm.ul_rgb = GUI_MCH_GET_RGB2(sgp->sg_gui_sp);
	وإلا
	    at_en.ae_u.cterm.ul_rgb = INVALCOLOR;
	إذا (at_en.ae_u.cterm.fg_rgb == INVALCOLOR
		&& at_en.ae_u.cterm.bg_rgb == INVALCOLOR)
	{
	    // If both fg and bg are invalid fall back to the cterm colors.
	    // Helps when the GUI only uses an attribute, e.g. undercurl.
	    at_en.ae_u.cterm.fg_rgb = CTERMCOLOR;
	    at_en.ae_u.cterm.bg_rgb = CTERMCOLOR;
	}
#جے_انت
	sgp->sg_cterm_attr = get_attr_اندراج(&cterm_attr_جدول, &at_en);
    }
}

/*
 * Check if highlight id is overridden, and return the overriding highlight id.
 * Otherwise return the original id if no override.
 */
    ساكن صحيح
syn_override(صحيح id)
{
    إذا (overrides != عدم && overrides->arr != عدم)
	لكل (صحيح k = 0; k < overrides->len; k++)
	    إذا (overrides->arr[k].from == id)
		أرجع overrides->arr[k].منزل;
    أرجع id;
}

/*
 * Lookup a highlight group name by pointer and length.
 * If it is not found, 0 is returned.
 */
    ساكن صحيح
syn_name2id_len(char_u *name, صحيح len)
{
    char_u	name_u[MAX_SYN_NAME + 1];
    hashitem_T	*hi;

    إذا (len > MAX_SYN_NAME)
	len = MAX_SYN_NAME;
    mch_memmove(name_u, name, len);
    name_u[len] = NUL;
    vim_strup(name_u);
    إذا (!highlight_ht_inited)
	أرجع 0;
    hi = hash_لبھو(&highlight_ht, name_u);
    إذا (HASHITEM_خالی_تھاواں(hi))
	أرجع 0;
    أرجع HI2HLNAME(hi)->hn_id;
}

/*
 * Lookup a highlight group name and return its ID.
 * If it is not found, 0 is returned.
 */
    صحيح
syn_name2id(char_u *name)
{
    // Avoid using stricmp() too much, it's slow on some systems.
    أرجع syn_name2id_len(name, (صحيح)STRLEN(name));
}

/*
 * Lookup a highlight group name and return its attributes.
 * Return zero if not found.
 */
    صحيح
syn_name2attr(char_u *name)
{
    صحيح id = syn_name2id(name);

    إذا (id != 0)
	أرجع syn_id2attr(id);
    أرجع 0;
}

#إذا defined(FEAT_EVAL)
/*
 * Return TRUE if highlight group "name" exists.
 */
    صحيح
highlight_exists(char_u *name)
{
    أرجع (syn_name2id(name) > 0);
}

# إذا defined(FEAT_تلاش_EXTRA)
/*
 * Return the name of highlight group "id".
 * When not a valid ID return an empty string.
 */
    char_u *
syn_id2name(صحيح id)
{
    إذا (id <= 0 || id > highlight_ga.ga_len)
	أرجع (char_u *)"";
    أرجع HL_جدول()[id - 1].sg_name;
}
# endif
#جے_انت

/*
 * Like syn_name2id(), but take a pointer + length argument.
 */
    صحيح
syn_namen2id(char_u *linep, صحيح len)
{
    أرجع syn_name2id_len(linep, len);
}

/*
 * Find highlight group name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    صحيح
syn_check_گروہ(char_u *pp, صحيح len)
{
    صحيح	    id;
    char_u  *name;

    إذا (len > MAX_SYN_NAME)
    {
	emsg(_(e_highlight_گروہ_name_too_long));
	أرجع 0;
    }
    id = syn_name2id_len(pp, len);
    إذا (id == 0)			// doesn't exist yet
    {
	name = vim_strnsave(pp, len);
	إذا (name == عدم)
	    أرجع 0;
	id = syn_جوڑو_گروہ(name);
    }
    أرجع id;
}

/*
 * Add new highlight group and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    ساكن صحيح
syn_جوڑو_گروہ(char_u *name)
{
    char_u	*p;
    char_u	*name_اتے;

    // Check that the name is valid (ASCII letters, digits, underscores, dots, or hyphens).
    لكل (p = name; *p != NUL; ++p)
    {
	إذا (!vim_isprintc(*p))
	{
	    emsg(_(e_unprintable_character_وچ_گروہ_name));
	    vim_free(name);
	    أرجع 0;
	}
	وإلا إذا (!ASCII_ISALNUM(*p) && *p != '_' && *p != '.' && *p != '-')
	{
	    // This is an error, but since there previously was no check only
	    // give a warning.
	    پیغام_سروت(HL_ATTR(HLF_W));
	    پیغام(_("W18: Invalid character in group name"));
	    اكسر;
	}
    }

    // First call for this growarray: init growing array.
    إذا (highlight_ga.ga_انکڑے == عدم)
    {
	highlight_ga.ga_itemsize = حجم(hl_گروہ_T);
	highlight_ga.ga_growsize = 10;
	hash_init(&highlight_ht);
	highlight_ht_inited = true;
    }

    إذا (highlight_ga.ga_len >= MAX_HL_ID)
    {
	emsg(_(e_too_many_highlight_and_syntax_groups));
	vim_free(name);
	أرجع 0;
    }

    // Make room for at least one other syntax_highlight entry.
    إذا (ga_grow(&highlight_ga, 1) == FAIL)
    {
	vim_free(name);
	أرجع 0;
    }

    {
	hlname_T    *hn;
	صحيح	    len = (صحيح)STRLEN(name);

	hn = alloc(offsetof(hlname_T, hn_کلید) + len + 1);
	إذا (hn == عدم)
	{
	    vim_free(name);
	    أرجع 0;
	}
	vim_strncpy(hn->hn_کلید, name, len);
	vim_strup(hn->hn_کلید);
	hn->hn_id = highlight_ga.ga_len + 1;	// ID is index plus one
	name_اتے = hn->hn_کلید;
    }

    CLEAR_اشارہ_گر(&(HL_جدول()[highlight_ga.ga_len]));
    HL_جدول()[highlight_ga.ga_len].sg_name = name;
    HL_جدول()[highlight_ga.ga_len].sg_name_u = name_اتے;
#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    HL_جدول()[highlight_ga.ga_len].sg_gui_bg = INVALCOLOR;
    HL_جدول()[highlight_ga.ga_len].sg_gui_fg = INVALCOLOR;
    HL_جدول()[highlight_ga.ga_len].sg_gui_sp = INVALCOLOR;
#جے_انت
    hash_جوڑو(&highlight_ht, name_اتے, "highlight");
    ++highlight_ga.ga_len;

    أرجع highlight_ga.ga_len;		    // ID is index plus one
}

/*
 * When, just after calling syn_add_group(), an error is discovered, this
 * function deletes the new name.
 */
    ساكن فراغ
syn_unadd_گروہ(فراغ)
{
    hashitem_T *hi;

    --highlight_ga.ga_len;
    hi = hash_لبھو(&highlight_ht, HL_جدول()[highlight_ga.ga_len].sg_name_u);
    إذا (!HASHITEM_خالی_تھاواں(hi))
	hash_remove(&highlight_ht, hi, "highlight");
    vim_free(HL_جدول()[highlight_ga.ga_len].sg_name);
    vim_free(HL_جدول()[highlight_ga.ga_len].sg_name_u - HLNAME_کلید_OFF);
}

/*
 * Translate a group ID to highlight attributes.
 * "hl_id" must be valid: > 0, caller must check.
 */
    صحيح
syn_id2attr(صحيح hl_id)
{
    صحيح		attr;
    hl_گروہ_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    أكد(hl_id > 0);
    sgp = &HL_جدول()[hl_id - 1];	    // index is ID minus one

#جے_تعریف_شدہ FEAT_GUI
    // Only use GUI attr when the GUI is being used.
    إذا (gui.وچ_use)
	attr = sgp->sg_gui_attr;
    وإلا
#جے_انت
	إذا (IS_CTERM)
	    attr = sgp->sg_cterm_attr;
	وإلا
	    attr = sgp->sg_رکن_attr;

    أرجع attr;
}

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Get the GUI colors and attributes for a group ID.
 * NOTE: the colors will be INVALCOLOR when not set, the color otherwise.
 */
    صحيح
syn_id2colors(صحيح hl_id, guicolor_T *fgp, guicolor_T *bgp)
{
    hl_گروہ_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    أكد(hl_id > 0);
    sgp = &HL_جدول()[hl_id - 1];	    // index is ID minus one

    *fgp = sgp->sg_gui_fg;
    *bgp = sgp->sg_gui_bg;
    أرجع sgp->sg_gui;
}
#جے_انت

#إذا (defined(MSWIN) \
	    && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL)) \
	    && defined(FEAT_TERMGUICOLORS)) \
	|| defined(FEAT_TERMINAL)
    فراغ
syn_id2cterm_bg(صحيح hl_id, صحيح *fgp, صحيح *bgp)
{
    hl_گروہ_T	*sgp;

    hl_id = syn_get_final_id(hl_id);
    // shouldn't happen
    أكد(hl_id > 0);
    sgp = &HL_جدول()[hl_id - 1];	    // index is ID minus one
    *fgp = sgp->sg_cterm_fg - 1;
    *bgp = sgp->sg_cterm_bg - 1;
}
#جے_انت

/*
 * Translate a group ID to the final group ID (following links).
 */
    صحيح
syn_get_final_id(صحيح hl_id)
{
    صحيح		گنتی;
    hl_گروہ_T	*sgp;

    إذا (hl_id > highlight_ga.ga_len || hl_id < 1)
	أرجع 0;			// Can be called from eval!!

    // Follow links until there is no more.
    // Look out for loops!  Break after 100 links.
    لكل (گنتی = 100; --گنتی >= 0; )
    {
	صحيح tmp = hl_id;

	sgp = &HL_جدول()[hl_id - 1];	    // index is ID minus one
	إذا (sgp->sg_link == 0 || sgp->sg_link > highlight_ga.ga_len)
	    اكسر;

	// This is to handle highlight groups that are overridden but are in the
	// middle of a link chain.
	hl_id = syn_override(hl_id);
	إذا (tmp != hl_id)
	    استمر;
	hl_id = sgp->sg_link;
    }
    أرجع syn_override(hl_id);
}

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Call this function just after the GUI has started.
 * Also called when 'termguicolors' was set, gui.in_use will be FALSE then.
 * It finds the font and color handles for the highlighting groups.
 */
    فراغ
highlight_gui_started(فراغ)
{
    صحيح	    idx;

    // First get the colors from the "Normal" and "Menu" group, if set
    إذا (USE_24_ثنائی_ہندسہ)
	سیٹ_عام_colors();

    لكل (idx = 0; idx < highlight_ga.ga_len; ++idx)
	gui_do_one_color(idx, FALSE, FALSE);

    highlight_بدلیا();
}

    ساكن فراغ
gui_do_one_color(
    صحيح		idx,
    صحيح		do_menu بن_ورتیا,	   // TRUE: might set the menu font
    صحيح		do_tooltip بن_ورتیا) // TRUE: might set the tooltip font
{
    صحيح		didit = FALSE;

# ifdef FEAT_GUI
#  ifdef FEAT_TERMGUICOLORS
    إذا (gui.وچ_use)
#  endif
	إذا (HL_جدول()[idx].sg_font_name != عدم)
	{
	    hl_do_font(idx, HL_جدول()[idx].sg_font_name, FALSE, do_menu,
							    do_tooltip, TRUE);
	    didit = TRUE;
	}
# endif
    إذا (HL_جدول()[idx].sg_gui_fg_name != عدم)
    {
	HL_جدول()[idx].sg_gui_fg =
			    color_name2handle(HL_جدول()[idx].sg_gui_fg_name);
	didit = TRUE;
    }
    إذا (HL_جدول()[idx].sg_gui_bg_name != عدم)
    {
	HL_جدول()[idx].sg_gui_bg =
			    color_name2handle(HL_جدول()[idx].sg_gui_bg_name);
	didit = TRUE;
    }
    إذا (HL_جدول()[idx].sg_gui_sp_name != عدم)
    {
	HL_جدول()[idx].sg_gui_sp =
			    color_name2handle(HL_جدول()[idx].sg_gui_sp_name);
	didit = TRUE;
    }
    إذا (didit)	// need to get a new attr number
	سیٹ_hl_attr(idx);
}
#جے_انت

#إذا defined(USER_HIGHLIGHT) && defined(FEAT_STL_OPT)
/*
 * Apply difference between User[1-9] and HLF_S to HLF_SNC, HLF_ST or HLF_STNC.
 */
    ساكن فراغ
combine_stl_hlt(
	صحيح id,
	صحيح id_S,
	صحيح id_alt,
	صحيح hlcnt,
	صحيح i,
	صحيح hlf,
	صحيح *جدول)
{
    hl_گروہ_T *hlt = HL_جدول();

    إذا (id_alt == 0)
    {
	CLEAR_اشارہ_گر(&hlt[hlcnt + i]);
	hlt[hlcnt + i].sg_رکن = highlight_attr[hlf];
	hlt[hlcnt + i].sg_cterm = highlight_attr[hlf];
# إذا defined(FEAT_GUI) || defined(FEAT_EVAL)
	hlt[hlcnt + i].sg_gui = highlight_attr[hlf];
# endif
    }
    وإلا
	mch_memmove(&hlt[hlcnt + i],
		    &hlt[id_alt - 1],
		    حجم(hl_گروہ_T));
    hlt[hlcnt + i].sg_link = 0;

    hlt[hlcnt + i].sg_رکن ^=
	hlt[id - 1].sg_رکن ^ hlt[id_S - 1].sg_رکن;
    إذا (hlt[id - 1].sg_شروع != hlt[id_S - 1].sg_شروع)
	hlt[hlcnt + i].sg_شروع = hlt[id - 1].sg_شروع;
    إذا (hlt[id - 1].sg_stop != hlt[id_S - 1].sg_stop)
	hlt[hlcnt + i].sg_stop = hlt[id - 1].sg_stop;
    hlt[hlcnt + i].sg_cterm ^=
	hlt[id - 1].sg_cterm ^ hlt[id_S - 1].sg_cterm;
    إذا (hlt[id - 1].sg_cterm_fg != hlt[id_S - 1].sg_cterm_fg)
	hlt[hlcnt + i].sg_cterm_fg = hlt[id - 1].sg_cterm_fg;
    إذا (hlt[id - 1].sg_cterm_bg != hlt[id_S - 1].sg_cterm_bg)
	hlt[hlcnt + i].sg_cterm_bg = hlt[id - 1].sg_cterm_bg;
    إذا (hlt[id - 1].sg_cterm_font != hlt[id_S - 1].sg_cterm_font)
	hlt[hlcnt + i].sg_cterm_font = hlt[id - 1].sg_cterm_font;
# إذا defined(FEAT_GUI) || defined(FEAT_EVAL)
    hlt[hlcnt + i].sg_gui ^=
	hlt[id - 1].sg_gui ^ hlt[id_S - 1].sg_gui;
# endif
# ifdef FEAT_GUI
    إذا (hlt[id - 1].sg_gui_fg != hlt[id_S - 1].sg_gui_fg)
	hlt[hlcnt + i].sg_gui_fg = hlt[id - 1].sg_gui_fg;
    إذا (hlt[id - 1].sg_gui_bg != hlt[id_S - 1].sg_gui_bg)
	hlt[hlcnt + i].sg_gui_bg = hlt[id - 1].sg_gui_bg;
    إذا (hlt[id - 1].sg_gui_sp != hlt[id_S - 1].sg_gui_sp)
	hlt[hlcnt + i].sg_gui_sp = hlt[id - 1].sg_gui_sp;
    إذا (hlt[id - 1].sg_font != hlt[id_S - 1].sg_font)
	hlt[hlcnt + i].sg_font = hlt[id - 1].sg_font;
#  ifdef FEAT_XFONTSET
    إذا (hlt[id - 1].sg_fontset != hlt[id_S - 1].sg_fontset)
	hlt[hlcnt + i].sg_fontset = hlt[id - 1].sg_fontset;
#  endif
# endif
    highlight_ga.ga_len = hlcnt + i + 1;
    سیٹ_hl_attr(hlcnt + i);	// At long last we can apply
    جدول[i] = syn_id2attr(hlcnt + i + 1);
}
#جے_انت

/*
 * Translate the 'highlight' option into attributes in highlight_attr[] and
 * set up the user highlights User1..9.  If FEAT_STL_OPT is in use, a set of
 * corresponding highlights to use on top of HLF_SNC is computed.
 * Called only when the 'highlight' option has been changed and upon first
 * screen redraw after any :highlight command.
 * Return FAIL when an invalid flag is found in 'highlight'.  OK otherwise.
 */
    صحيح
highlight_بدلیا(فراغ)
{
    صحيح		hlf;
    صحيح		i;
    char_u	*p;
    char_u	*default_hl;
    صحيح		attr;
    char_u	*سرا;
    صحيح		id;
#جے_تعریف_شدہ USER_HIGHLIGHT
    char_u      userhl[30];  // use 30 to avoid compiler warning
# ifdef FEAT_STL_OPT
    صحيح		id_S = -1;
    صحيح		id_SNC = 0;
#  ifdef FEAT_TERMINAL
    صحيح		id_ST = 0;
    صحيح		id_STNC = 0;
#  endif
    صحيح		hlcnt;
# endif
#جے_انت
    win_T	*wp;

    need_highlight_بدلیا = FALSE;

#جے_تعریف_شدہ FEAT_TERMINAL
    رکن_update_colors_all();
    رکن_update_hlfwin_all();
#جے_انت

    // Clear all attributes.
    لكل (hlf = 0; hlf < (صحيح)HLF_گنتی; ++hlf)
    {
	highlight_attr[hlf] = 0;
	highlight_attr_raw[hlf] = 0;
	highlight_ids[hlf] = 0;
    }

    default_hl = get_highlight_default();

    // First set all attributes to their default value.
    // Then use the attributes from the 'highlight' option, unless it already
    // matches the default and would repeat the same work.
    لكل (i = 0; i < 2; ++i)
    {
	إذا (i)
	{
	    إذا (default_hl != عدم && p_hl != عدم
				      && STRCMP(default_hl, p_hl) == 0)
		استمر;
	    p = p_hl;
	}
	وإلا
	    p = default_hl;
	إذا (p == عدم)	    // just in case
	    استمر;

	طالما (*p)
	{
	    لكل (hlf = 0; hlf < (صحيح)HLF_گنتی; ++hlf)
		إذا (hl_flags[hlf] == *p)
		    اكسر;
	    ++p;
	    إذا (hlf == (صحيح)HLF_گنتی || *p == NUL)
		أرجع FAIL;

	    // Allow several hl_flags to be combined, like "bu" for
	    // bold-underlined.
	    attr = 0;
	    id = 0;
	    لكل ( ; *p && *p != ','; ++p)	    // parse up to comma
	    {
		إذا (VIM_ISWHITE(*p))		    // ignore white space
		    استمر;

		إذا (attr > HL_ALL)  // Combination with ':' is not allowed.
		    أرجع FAIL;

		// Note: Keep this in sync with expand_set_highlight().
		بدل (*p)
		{
		    حالة 'b':	attr |= HL_BOLD;
				اكسر;
		    حالة 'i':	attr |= HL_ITALIC;
				اكسر;
		    حالة '-':
		    حالة 'n':			    // no highlighting
				اكسر;
		    حالة 'r':	attr |= HL_INVERSE;
				اكسر;
		    حالة 's':	attr |= HL_STANDOUT;
				اكسر;
		    حالة 'u':	attr |= HL_UNDERLINE;
				اكسر;
		    حالة 'c':	attr |= HL_UNDERCURL;
				اكسر;
		    حالة '2':	attr |= HL_UNDERDOUBLE;
				اكسر;
		    حالة 'd':	attr |= HL_UNDERDOTTED;
				اكسر;
		    حالة '=':	attr |= HL_UNDERDASHED;
				اكسر;
		    حالة 't':	attr |= HL_STRIKETHROUGH;
				اكسر;
		    حالة ':':	++p;		    // highlight group name
				إذا (attr || *p == NUL)	 // no combinations
				    أرجع FAIL;
				سرا = vim_strchr(p, ',');
				إذا (سرا == عدم)
				    سرا = p + STRLEN(p);
				id = syn_check_گروہ(p, (صحيح)(سرا - p));
				إذا (id == 0)
				    أرجع FAIL;
				attr = syn_id2attr(id);
				p = سرا - 1;
#إذا defined(FEAT_STL_OPT) && defined(USER_HIGHLIGHT)
				إذا (hlf == (صحيح)HLF_SNC)
				    id_SNC = syn_get_final_id(id);
# ifdef FEAT_TERMINAL
				وإلا إذا (hlf == (صحيح)HLF_ST)
				    id_ST = syn_get_final_id(id);
				وإلا إذا (hlf == (صحيح)HLF_STNC)
				    id_STNC = syn_get_final_id(id);
# endif
				وإلا إذا (hlf == (صحيح)HLF_S)
				    id_S = syn_get_final_id(id);
#جے_انت
				اكسر;
		    افتراضي:	أرجع FAIL;
		}
	    }
	    highlight_attr[hlf] = attr;
	    highlight_attr_raw[hlf] = attr;
	    highlight_ids[hlf] = id;

	    p = skip_منزل_option_part(p);	    // skip comma and spaces
	}
    }

    // Highlight ids may have been changed, so keep windows up to date
    FOR_ALL_WINDOWS(wp)
    {
	حرف *errmsg = update_winhighlight(wp, wp->w_p_whl);

	إذا (errmsg != عدم)
	    emsg(_(errmsg));
    }

#جے_تعریف_شدہ FEAT_TERMINAL
    رکن_update_hlfwin_all();
#جے_انت

#جے_تعریف_شدہ USER_HIGHLIGHT
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
    إذا (ga_grow(&highlight_ga, 28) == FAIL)
	أرجع FAIL;
    hlcnt = highlight_ga.ga_len;
    إذا (id_S == -1)
    {
	// Make sure id_S is always valid to simplify code below. Use the last
	// entry.
	CLEAR_اشارہ_گر(&HL_جدول()[hlcnt + 27]);
	HL_جدول()[hlcnt + 18].sg_رکن = highlight_attr[HLF_S];
	id_S = hlcnt + 19;
    }
# endif
    لكل (i = 0; i < 9; i++)
    {
	sprintf((حرف *)userhl, "User%d", i + 1);
	id = syn_name2id(userhl);
	إذا (id == 0)
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
	وإلا
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

#جے_انت // USER_HIGHLIGHT

    أرجع OK;
}

ساكن فراغ highlight_list(فراغ);
ساكن فراغ highlight_list_two(صحيح cnt, صحيح attr);

// context for :highlight <group> <arg> expansion
ساكن صحيح expand_hi_synid = 0;	    // ID for highlight group being completed
ساكن صحيح expand_hi_equal_خانہ = 0; // column where the '=' is
ساكن صحيح expand_hi_include_orig = 0;	    // whether to fill the existing current value or not
ساكن char_u *expand_hi_curvalue = عدم;   // the existing current value
#إذا defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
ساكن dict_iterator_T expand_colornames_iter;	// iterator for looping through v:colornames
#جے_انت

/*
 * Handle command line completion for :highlight command.
 */
    فراغ
سیٹ_context_وچ_highlight_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;
    صحيح		expand_گروہ = TRUE;

    // Default: expand group names
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_نمونہ = arg;
    include_none = 0;
    include_link = 2;
    include_default = 1;

    إذا (*arg == NUL)
	أرجع;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    إذا (*p == NUL)
	أرجع;

    // past "default" or group name
    include_default = 0;
    إذا (STRNCMP("default", arg, p - arg) == 0)
    {
	arg = skipwhite(p);
	xp->xp_نمونہ = arg;
	p = skiptowhite(arg);
    }
    إذا (*p == NUL)
	أرجع;

    // past group name
    include_link = 0;
    إذا (arg[1] == 'i' && arg[0] == 'N')
    {
	highlight_list();
	expand_گروہ = FALSE;
    }
    إذا (STRNCMP("link", arg, p - arg) == 0)
    {
	xp->xp_نمونہ = skipwhite(p);
	p = skiptowhite(xp->xp_نمونہ);
	إذا (*p != NUL)		// past first group name
	{
	    xp->xp_نمونہ = skipwhite(p);
	    p = skiptowhite(xp->xp_نمونہ);
	    include_none = 1;
	}
	expand_گروہ = FALSE;
    }
    وإلا إذا (STRNCMP("clear", arg, p - arg) == 0)
    {
	xp->xp_نمونہ = skipwhite(p);
	p = skiptowhite(xp->xp_نمونہ);
	expand_گروہ = FALSE;
    }
    إذا (*p != NUL)			// past group name(s)
    {
	إذا (expand_گروہ)
	{
	    // expansion will be done in expand_highlight_group()
	    xp->xp_context = EXPAND_HIGHLIGHT_گروہ;

	    expand_hi_synid = syn_namen2id(arg, (صحيح)(p - arg));

	    طالما (*p != NUL)
	    {
		arg = skipwhite(p);
		p = skiptowhite(arg);
	    }

	    p = vim_strchr(arg, '=');
	    إذا (p == عدم)
	    {
		// Didn't find a key=<value> pattern
		xp->xp_نمونہ = arg;
		expand_hi_equal_خانہ = -1;
		expand_hi_include_orig = FALSE;
	    }
	    وإلا
	    {
		// Found key=<value> pattern, record the exact location
		expand_hi_equal_خانہ = (صحيح)(p - xp->xp_سطر);

		// Only include the original value if the pattern is empty
		إذا (*(p + 1) == NUL)
		    expand_hi_include_orig = TRUE;
		وإلا
		    expand_hi_include_orig = FALSE;

		// Account for comma-separated values
		إذا (STRNCMP(arg, "term=", 5) == 0 ||
			STRNCMP(arg, "cterm=", 6) == 0 ||
			STRNCMP(arg, "gui=", 4) == 0)
		{
		    char_u *comma = vim_strrchr(p + 1, ',');
		    إذا (comma != عدم)
			p = comma;
		}
		xp->xp_نمونہ = p + 1;
	    }
	}
	وإلا
	{
	    xp->xp_context = EXPAND_NOTHING;
	}
    }
}

/*
 * List highlighting matches in a nice way.
 */
    ساكن فراغ
highlight_list(فراغ)
{
    صحيح		i;

    لكل (i = 10; --i >= 0; )
	highlight_list_two(i, HL_ATTR(HLF_D));
    لكل (i = 40; --i >= 0; )
	highlight_list_two(99, 0);
}

    ساكن فراغ
highlight_list_two(صحيح cnt, صحيح attr)
{
    پیغام_puts_attr(&("N \bI \b!  \b"[cnt / 11]), attr);
    پیغام_clr_eos();
    باہرلا_flush();
    ui_delay(cnt == 99 ? 40L : (لما)cnt * 50L, FALSE);
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_highlight_name(expand_T *xp بن_ورتیا, صحيح idx)
{
    أرجع get_highlight_name_ext(xp, idx, TRUE);
}

/*
 * Obtain a highlight group name.
 * When "skip_cleared" is TRUE don't return a cleared entry.
 */
    char_u *
get_highlight_name_ext(expand_T *xp بن_ورتیا, صحيح idx, صحيح skip_cleared)
{
    إذا (idx < 0)
	أرجع عدم;

    // Items are never removed from the table, skip the ones that were
    // cleared.
    إذا (skip_cleared && idx < highlight_ga.ga_len && HL_جدول()[idx].sg_cleared)
	أرجع (char_u *)"";

    إذا (idx == highlight_ga.ga_len && include_none != 0)
	أرجع (char_u *)"NONE";
    إذا (idx == highlight_ga.ga_len + include_none && include_default != 0)
	أرجع (char_u *)"default";
    إذا (idx == highlight_ga.ga_len + include_none + include_default
							 && include_link != 0)
	أرجع (char_u *)"link";
    إذا (idx == highlight_ga.ga_len + include_none + include_default + 1
							 && include_link != 0)
	أرجع (char_u *)"clear";
    إذا (idx >= highlight_ga.ga_len)
	أرجع عدم;
    أرجع HL_جدول()[idx].sg_name;
}

    ساكن char_u *
get_highlight_attr_name(expand_T *xp بن_ورتیا, صحيح idx)
{
    إذا (idx == 0)
    {
	// Fill with current value first
	إذا (expand_hi_curvalue != عدم)
	    أرجع expand_hi_curvalue;
	وإلا
	    أرجع (char_u*)"";
    }
    إذا (idx < (صحيح)ARRAY_لمائی(highlight_اشاریہ_tab) + 1)
    {
	char_u *قدر = highlight_اشاریہ_tab[idx-1]->قدر.string;
	إذا (expand_hi_curvalue != عدم && STRCMP(expand_hi_curvalue, قدر) == 0)
	{
	    // Already returned the current value above, just skip.
	    أرجع (char_u*)"";
	}
	أرجع قدر;
    }
    أرجع عدم;
}

    ساكن char_u *
get_highlight_cterm_color(expand_T *xp بن_ورتیا, صحيح idx)
{
    إذا (idx == 0)
    {
	// Fill with current value first
	إذا (expand_hi_curvalue != عدم)
	    أرجع expand_hi_curvalue;
	وإلا
	    أرجع (char_u*)"";
    }
    // See highlight_set_cterm_color()
    وإلا إذا (idx == 1)
	أرجع (char_u*)"fg";
    وإلا إذا (idx == 2)
	أرجع (char_u*)"bg";
    إذا (idx < (صحيح)ARRAY_لمائی(color_name_tab) + 3)
    {
	char_u *قدر = color_name_tab[idx-3].قدر.string;
	أرجع قدر;
    }
    أرجع عدم;
}

#إذا defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
    ساكن char_u *
get_highlight_gui_color(expand_T *xp بن_ورتیا, صحيح idx)
{
    إذا (idx == 0)
    {
	// Fill with current value first
	إذا (expand_hi_curvalue != عدم)
	    أرجع expand_hi_curvalue;
	وإلا
	    أرجع (char_u*)"";
    }
    // See color_name2handle()
    وإلا إذا (idx == 1)
	أرجع (char_u*)"fg";
    وإلا إذا (idx == 2)
	أرجع (char_u*)"bg";
    وإلا إذا (idx == 3)
	أرجع (char_u*)"NONE";

    // Complete from v:colornames. Don't do platform specific names for now.
    typval_T *tv_نتیجہ;
    char_u *colorname = dict_iterate_اگلا(&expand_colornames_iter, &tv_نتیجہ);
    إذا (colorname != عدم)
    {
	// :hi command doesn't allow space, so don't suggest any malformed items
	إذا (vim_strchr(colorname, ' ') != عدم)
	    أرجع (char_u*)"";

	إذا (expand_hi_curvalue != عدم && STRICMP(expand_hi_curvalue, colorname) == 0)
	{
	    // Already returned the current value above, just skip.
	    أرجع (char_u*)"";
	}
    }
    أرجع colorname;
}
#جے_انت

    ساكن char_u *
get_highlight_گروہ_کلید(expand_T *xp بن_ورتیا, صحيح idx)
{
    // Note: Keep this in sync with do_highlight.
    ساكن حرف *(p_hi_گروہ_کلید_اقدار[]) =
    {
	"term=",
	"start=",
	"stop=",
	"cterm=",
	"ctermfg=",
	"ctermbg=",
	"ctermul=",
	"ctermfont=",
#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	"gui=",
	"guifg=",
	"guibg=",
	"guisp=",
#جے_انت
#جے_تعریف_شدہ FEAT_GUI
	"font=",
#جے_انت
	"NONE",
    };

    إذا (idx < (صحيح)ARRAY_لمائی(p_hi_گروہ_کلید_اقدار))
	أرجع (char_u*)p_hi_گروہ_کلید_اقدار[idx];
    أرجع عدم;
}

/*
 * Command-line expansion for :hi {group-name} <args>...
 */
    صحيح
expand_highlight_گروہ(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	صحيح	    *عدد_Matches)
{
    إذا (expand_hi_equal_خانہ != -1)
    {
	// List the values. First fill in the current value, then if possible colors
	// or attribute names.
	char_u	    *(*expandfunc)(expand_T *, صحيح) = عدم;
	صحيح	    type = 0;
	hl_گروہ_T  *sgp = عدم;
	صحيح	    iarg = 0;
	char_u	    *sarg = عدم;

	صحيح	    unsorted_اشیاء = -1; // don't sort by default

	إذا (expand_hi_synid != 0)
	    sgp = &HL_جدول()[expand_hi_synid - 1]; // index is ID minus one

	// Note: Keep this in sync with highlight_list_one().
	char_u	    *name_سرا = xp->xp_سطر + expand_hi_equal_خانہ;
	إذا (name_سرا - xp->xp_سطر >= 5
		&& STRNCMP(name_سرا - 5, " term", 5) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    إذا (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_رکن;
	    }
	}
	وإلا إذا (name_سرا - xp->xp_سطر >= 6
		&& STRNCMP(name_سرا - 6, " cterm", 6) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    إذا (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_cterm;
	    }
	}
#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
	وإلا إذا (name_سرا - xp->xp_سطر >= 4
		&& STRNCMP(name_سرا - 4, " gui", 4) == 0)
	{
	    expandfunc = get_highlight_attr_name;
	    إذا (sgp)
	    {
		type = LIST_ATTR;
		iarg = sgp->sg_gui;
	    }
	}
#جے_انت
	وإلا إذا (name_سرا - xp->xp_سطر >= 8
		&& STRNCMP(name_سرا - 8, " ctermfg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    إذا (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_fg;
	    }
	}
	وإلا إذا (name_سرا - xp->xp_سطر >= 8
		&& STRNCMP(name_سرا - 8, " ctermbg", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    إذا (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_bg;
	    }
	}
	وإلا إذا (name_سرا - xp->xp_سطر >= 8
		&& STRNCMP(name_سرا - 8, " ctermul", 8) == 0)
	{
	    expandfunc = get_highlight_cterm_color;
	    إذا (sgp)
	    {
		type = LIST_INT;
		iarg = sgp->sg_cterm_ul;
	    }
	}
#إذا defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	وإلا إذا (name_سرا - xp->xp_سطر >= 6
		&& STRNCMP(name_سرا - 6, " guifg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    إذا (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_fg_name;
	    }
	}
	وإلا إذا (name_سرا - xp->xp_سطر >= 6
		&& STRNCMP(name_سرا - 6, " guibg", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    إذا (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_bg_name;
	    }
	}
	وإلا إذا (name_سرا - xp->xp_سطر >= 6
		&& STRNCMP(name_سرا - 6, " guisp", 6) == 0)
	{
	    expandfunc = get_highlight_gui_color;
	    إذا (sgp)
	    {
		type = LIST_STRING;
		sarg = sgp->sg_gui_sp_name;
	    }
	}
#جے_انت

#إذا defined(FEAT_EVAL) && (defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS))
	إذا (expandfunc == get_highlight_gui_color)
	{
	    // Top 4 items are special, after that sort all the color names
	    unsorted_اشیاء = 4;

	    dict_T *colornames_جدول = get_vim_var_dict(VV_COLORNAMES);
	    typval_T colornames_val;
	    colornames_val.v_type = VAR_DICT;
	    colornames_val.vval.v_dict = colornames_جدول;
	    dict_iterate_شروع(&colornames_val, &expand_colornames_iter);
	}
#جے_انت

	char_u	    buf[MAX_ATTR_LEN];

	expand_hi_curvalue = عدم;
	إذا (expand_hi_include_orig)
	{
	    إذا (((type == LIST_ATTR || type == LIST_INT) && iarg != 0) ||
		(type == LIST_STRING && sarg != عدم))
	    {
		// Retrieve the current value to go first in completion
		expand_hi_curvalue = highlight_arg_منزل_string(
			type, iarg, sarg, buf);
	    }
	}

	إذا (expandfunc != عدم)
	{
	    أرجع ExpandGenericExt(
		    pat,
		    xp,
		    rmp,
		    matches,
		    عدد_Matches,
		    expandfunc,
		    FALSE,
		    unsorted_اشیاء);
	}

	أرجع FAIL;
    }

    // List all the key names
    أرجع ExpandGenericExt(
	    pat,
	    xp,
	    rmp,
	    matches,
	    عدد_Matches,
	    get_highlight_گروہ_کلید,
	    FALSE,
	    -1);
}

#إذا defined(FEAT_GUI)
/*
 * Free all the highlight group fonts.
 * Used when quitting for systems which need it.
 */
    فراغ
free_highlight_fonts(فراغ)
{
    صحيح	    idx;

    لكل (idx = 0; idx < highlight_ga.ga_len; ++idx)
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
#جے_انت

#إذا defined(FEAT_EVAL)
/*
 * Convert each of the highlight attribute bits (bold, standout, underline,
 * etc.) set in 'hlattr' into a separate boolean item in a Dictionary with
 * the attribute name as the key.
 */
    ساكن dict_T *
highlight_get_attr_dict(صحيح hlattr)
{
    dict_T	*dict;
    صحيح		i;

    dict = dict_alloc();
    إذا (dict == عدم)
	أرجع عدم;

    لكل (i = 0; i < (صحيح)ARRAY_لمائی(highlight_اشاریہ_tab); ++i)
    {
	إذا (hlattr & highlight_اشاریہ_tab[i]->کلید)
	{
	    dict_جوڑو_bool(dict, (حرف *)highlight_اشاریہ_tab[i]->قدر.string,
		    VVAL_TRUE);
	    // don't want "inverse"/"reverse"
	    hlattr &= ~highlight_اشاریہ_tab[i]->کلید;
	}
    }

    أرجع dict;
}

/*
 * Return the attributes of the highlight group at index 'hl_idx' as a
 * Dictionary. If 'resolve_link' is TRUE, then resolves the highlight group
 * links recursively.
 */
    ساكن dict_T *
highlight_get_info(صحيح hl_idx, صحيح resolve_link)
{
    dict_T	*dict;
    hl_گروہ_T	*sgp;
    dict_T	*attr_dict = عدم;
    صحيح		hlgid;

    dict = dict_alloc();
    إذا (dict == عدم)
	أرجع dict;

    sgp = &HL_جدول()[hl_idx];
    // highlight group id is 1-based
    hlgid = hl_idx + 1;

    إذا (dict_جوڑو_string(dict, "name", sgp->sg_name) == FAIL)
	اذهب error;
    إذا (dict_جوڑو_نمبر(dict, "id", hlgid) == FAIL)
	اذهب error;

    إذا (resolve_link)
    {
	// resolve the highlight group link recursively
	طالما (sgp->sg_link)
	{
	    hlgid = sgp->sg_link;
	    sgp = &HL_جدول()[sgp->sg_link - 1];
	}
    }

    إذا (sgp->sg_رکن != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_رکن);
	إذا (attr_dict != عدم)
	{
	    إذا (dict_جوڑو_dict(dict, "term", attr_dict) == FAIL)
		اذهب error;
	    attr_dict = عدم;
	}
    }
    إذا (sgp->sg_شروع != عدم)
	إذا (dict_جوڑو_string(dict, "start", sgp->sg_شروع) == FAIL)
	    اذهب error;
    إذا (sgp->sg_stop != عدم)
	إذا (dict_جوڑو_string(dict, "stop", sgp->sg_stop) == FAIL)
	    اذهب error;
    إذا (sgp->sg_cterm != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_cterm);
	إذا (attr_dict != عدم)
	{
	    إذا (dict_جوڑو_dict(dict, "cterm", attr_dict) == FAIL)
		اذهب error;
	    attr_dict = عدم;
	}
    }
    إذا (sgp->sg_cterm_fg != 0)
	إذا (dict_جوڑو_string(dict, "ctermfg",
		    highlight_color(hlgid, (char_u *)"fg", 'c')) == FAIL)
	    اذهب error;
    إذا (sgp->sg_cterm_bg != 0)
	إذا (dict_جوڑو_string(dict, "ctermbg",
			highlight_color(hlgid, (char_u *)"bg", 'c')) == FAIL)
	    اذهب error;
    إذا (sgp->sg_cterm_ul != 0)
	إذا (dict_جوڑو_string(dict, "ctermul",
			highlight_color(hlgid, (char_u *)"ul", 'c')) == FAIL)
	    اذهب error;
    إذا (sgp->sg_cterm_font != 0)
	إذا (dict_جوڑو_string(dict, "ctermfont",
			highlight_color(hlgid, (char_u *)"font", 'c')) == FAIL)
	    اذهب error;
    إذا (sgp->sg_gui != 0)
    {
	attr_dict = highlight_get_attr_dict(sgp->sg_gui);
	إذا (attr_dict != عدم)
	{
	    إذا (dict_جوڑو_dict(dict, "gui", attr_dict) == FAIL)
		اذهب error;
	    attr_dict = عدم;
	}
    }
    إذا (sgp->sg_gui_fg_name != عدم)
	إذا (dict_جوڑو_string(dict, "guifg",
			highlight_color(hlgid, (char_u *)"fg", 'g')) == FAIL)
	    اذهب error;
    إذا (sgp->sg_gui_bg_name != عدم)
	إذا (dict_جوڑو_string(dict, "guibg",
			highlight_color(hlgid, (char_u *)"bg", 'g')) == FAIL)
	    اذهب error;
    إذا (sgp->sg_gui_sp_name != عدم)
	إذا (dict_جوڑو_string(dict, "guisp",
			highlight_color(hlgid, (char_u *)"sp", 'g')) == FAIL)
	    اذهب error;
# ifdef FEAT_GUI
    إذا (sgp->sg_font_name != عدم)
	إذا (dict_جوڑو_string(dict, "font", sgp->sg_font_name) == FAIL)
	    اذهب error;
# endif
    إذا (sgp->sg_link)
    {
	char_u	*link;

	link = HL_جدول()[sgp->sg_link - 1].sg_name;
	إذا (link != عدم && dict_جوڑو_string(dict, "linksto", link) == FAIL)
	    اذهب error;

	إذا (sgp->sg_deflink)
	    dict_جوڑو_bool(dict, "default", VVAL_TRUE);
    }
    إذا (dict_len(dict) == 2)
	// If only 'name' is present, then the highlight group is cleared.
	dict_جوڑو_bool(dict, "cleared", VVAL_TRUE);

    أرجع dict;

error:
    dict_unref(attr_dict);
    dict_unref(dict);
    أرجع عدم;
}

/*
 * "hlget([name])" function
 * Return the attributes of a specific highlight group (if specified) or all
 * the highlight groups.
 */
    فراغ
f_hlget(typval_T *argvars, typval_T *rettv)
{
    list_T	*list;
    dict_T	*dict;
    صحيح		i;
    char_u	*hlarg = عدم;
    صحيح		resolve_link = FALSE;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (check_for_opt_string_arg(argvars, 0) == FAIL
	    || (argvars[0].v_type != VAR_UNKNOWN
		&& check_for_opt_bool_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	// highlight group name supplied
	hlarg = tv_get_string_chk(&argvars[0]);
	إذا (hlarg == عدم)
	    أرجع;

	إذا (argvars[1].v_type != VAR_UNKNOWN)
	{
	    صحيح error = FALSE;

	    resolve_link = tv_get_bool_chk(&argvars[1], &error);
	    إذا (error)
		أرجع;
	}
    }

    list = rettv->vval.v_list;
    لكل (i = 0; i < highlight_ga.ga_len && !got_int; ++i)
    {
	إذا (hlarg == عدم || STRICMP(hlarg, HL_جدول()[i].sg_name) == 0)
	{
	    dict = highlight_get_info(i, resolve_link);
	    إذا (dict != عدم && list_append_dict(list, dict) == FAIL)
		dict_unref(dict);
	}
    }
}

/*
 * Returns the string value at 'dict[key]'. Returns NULL, if 'key' is not in
 * 'dict' or the value is not a string type. If the value is not a string type
 * or is NULL, then 'error' is set to TRUE.
 */
    ساكن char_u *
hldict_get_string(dict_T *dict, char_u *کلید, صحيح *error)
{
    dictitem_T	*di;

    *error = FALSE;
    di = dict_لبھو(dict, کلید, -1);
    إذا (di == عدم)
	أرجع عدم;

    إذا (di->di_tv.v_type != VAR_STRING || di->di_tv.vval.v_string == عدم)
    {
	emsg(_(e_string_required));
	*error = TRUE;
	أرجع عدم;
    }

    أرجع di->di_tv.vval.v_string;
}

/*
 * Convert the highlight attribute Dictionary at 'dict[key]' into a string
 * value in 'attr_str' of length 'len'. Returns FALSE if 'dict[key]' is not a
 * Dictionary or is NULL.
 */
    ساكن صحيح
hldict_attr_منزل_str(
	dict_T	*dict,
	char_u	*کلید,
	char_u	*attr_str,
	size_t	len)
{
    dictitem_T	*di;
    dict_T	*attrdict;
    صحيح		i;
    char_u	*p;

    attr_str[0] = NUL;
    di = dict_لبھو(dict, کلید, -1);
    إذا (di == عدم)
	أرجع TRUE;

    إذا (di->di_tv.v_type != VAR_DICT || di->di_tv.vval.v_dict == عدم)
    {
	emsg(_(e_dictionary_required));
	أرجع FALSE;
    }

    attrdict = di->di_tv.vval.v_dict;

    // If the attribute dict is empty, then return NONE to clear the attributes
    إذا (dict_len(attrdict) == 0)
    {
	vim_strcat(attr_str, (char_u *)"NONE", len);
	أرجع TRUE;
    }

    p = attr_str;
    لكل (i = 0; i < (صحيح)ARRAY_لمائی(highlight_tab); ++i)
    {
	إذا (dict_get_bool(attrdict, (حرف *)highlight_tab[i].قدر.string,
		    VVAL_FALSE) == VVAL_TRUE)
	{
	    إذا (p != attr_str && (size_t)(p - attr_str + 2) < len)
		STRCPY(p, (char_u *)",");
	    إذا (p - attr_str + highlight_tab[i].قدر.لمائی + 1 < len)
	    {
		STRCPY(p, highlight_tab[i].قدر.string);
		p += highlight_tab[i].قدر.لمائی;
	    }
	}
    }

    أرجع TRUE;
}

// Temporary buffer used to store the command string produced by hlset().
// IObuff cannot be used for this as the error messages produced by hlset()
// internally use IObuff.
# define	HLSETBUFSZ  512
ساكن char_u hlsetBuf[HLSETBUFSZ + 1];

/*
 * Add the highlight attribute "attr" of length "attrlen" and "value" at
 * "dptr", which points into "hlsetBuf".
 * Returns the updated pointer.
 */
    ساكن char_u *
جوڑو_attr_and_قدر(char_u *dptr, char_u *attr, صحيح attrlen, char_u *قدر)
{
    size_t	vallen;

    // Do nothing if the value is not specified or is empty
    إذا (قدر == عدم || *قدر == NUL)
	أرجع dptr;

    vallen = STRLEN(قدر);
    // When the value contains a space and the attribute has an "=" (i.e. it
    // is a key=value pair), surround the value with single quotes so that
    // do_highlight() can parse it correctly.
    إذا (vim_strchr(قدر, ' ') != عدم && vim_strchr(attr, '=') != عدم)
    {
	إذا (dptr + attrlen + vallen + 3 < hlsetBuf + HLSETBUFSZ)
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
    وإلا إذا (dptr + attrlen + vallen + 1 < hlsetBuf + HLSETBUFSZ)
    {
	STRCPY(dptr, attr);
	dptr += attrlen;
	STRCPY(dptr, قدر);
	dptr += vallen;
    }

    أرجع dptr;
}

/*
 * Add or update a highlight group using 'dict' items. Returns TRUE if
 * successfully updated the highlight group.
 */
    ساكن صحيح
hlg_جوڑو_or_update(dict_T *dict)
{
    char_u	*name;
    صحيح		error;
    char_u	رکن_attr[MAX_ATTR_LEN];
    char_u	cterm_attr[MAX_ATTR_LEN];
    char_u	gui_attr[MAX_ATTR_LEN];
    char_u	*شروع;
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
    صحيح		forceit = FALSE;
    صحيح		dodefault = FALSE;
    صحيح		پورا = FALSE;
    char_u	*p;

    name = hldict_get_string(dict, (char_u *)"name", &error);
    إذا (name == عدم || *name == NUL || error)
	أرجع FALSE;

    إذا (dict_get_bool(dict, "force", VVAL_FALSE) == VVAL_TRUE)
	forceit = TRUE;

    إذا (dict_get_bool(dict, "default", VVAL_FALSE) == VVAL_TRUE)
	dodefault = TRUE;

    إذا (dict_has_کلید(dict, "cleared"))
    {
	varnumber_T	cleared;

	// clear a highlight group
	cleared = dict_get_bool(dict, "cleared", FALSE);
	إذا (cleared == TRUE)
	{
	    vim_snprintf((حرف *)hlsetBuf, HLSETBUFSZ, "clear %s", name);
	    do_highlight(hlsetBuf, forceit, FALSE);
	    پورا = TRUE;
	}
    }

    إذا (dict_has_کلید(dict, "linksto"))
    {
	char_u	*linksto;

	// link highlight groups
	linksto = hldict_get_string(dict, (char_u *)"linksto", &error);
	إذا (linksto == عدم || *linksto == NUL || error)
	    أرجع FALSE;

	vim_snprintf((حرف *)hlsetBuf, HLSETBUFSZ, "%slink %s %s",
				dodefault ? "default " : "", name, linksto);
	do_highlight(hlsetBuf, forceit, FALSE);

	پورا = TRUE;
    }

    // If 'cleared' or 'linksto' are specified, then don't process the other
    // attributes.
    إذا (پورا)
	أرجع TRUE;

    شروع = hldict_get_string(dict, (char_u *)"start", &error);
    إذا (error)
	أرجع FALSE;

    stop = hldict_get_string(dict, (char_u *)"stop", &error);
    إذا (error)
	أرجع FALSE;

    إذا (!hldict_attr_منزل_str(dict, (char_u *)"term", رکن_attr,
							    حجم(رکن_attr)))
	أرجع FALSE;

    إذا (!hldict_attr_منزل_str(dict, (char_u *)"cterm", cterm_attr,
							   حجم(cterm_attr)))
	أرجع FALSE;

    ctermfg = hldict_get_string(dict, (char_u *)"ctermfg", &error);
    إذا (error)
	أرجع FALSE;

    ctermbg = hldict_get_string(dict, (char_u *)"ctermbg", &error);
    إذا (error)
	أرجع FALSE;

    ctermul = hldict_get_string(dict, (char_u *)"ctermul", &error);
    إذا (error)
	أرجع FALSE;

    ctermfont = hldict_get_string(dict, (char_u *)"ctermfont", &error);
    إذا (error)
	أرجع FALSE;

    إذا (!hldict_attr_منزل_str(dict, (char_u *)"gui", gui_attr, حجم(gui_attr)))
	أرجع FALSE;

    guifg = hldict_get_string(dict, (char_u *)"guifg", &error);
    إذا (error)
	أرجع FALSE;

    guibg = hldict_get_string(dict, (char_u *)"guibg", &error);
    إذا (error)
	أرجع FALSE;

    guisp = hldict_get_string(dict, (char_u *)"guisp", &error);
    إذا (error)
	أرجع FALSE;

# ifdef FEAT_GUI
    font = hldict_get_string(dict, (char_u *)"font", &error);
    إذا (error)
	أرجع FALSE;
# endif

    // If none of the attributes are specified, then do nothing.
    إذا (رکن_attr[0] == NUL && شروع == عدم && stop == عدم
	    && cterm_attr[0] == NUL && ctermfg == عدم && ctermbg == عدم
	    && ctermul == عدم && ctermfont == عدم && gui_attr[0] == NUL
# ifdef FEAT_GUI
	    && font == عدم
# endif
	    && guifg == عدم && guibg == عدم && guisp == عدم
	    )
	أرجع TRUE;

    hlsetBuf[0] = NUL;
    p = hlsetBuf;
    إذا (dodefault)
	p = جوڑو_attr_and_قدر(p, (char_u *)"default", 7, (char_u *)" ");
    p = جوڑو_attr_and_قدر(p, (char_u *)"", 0, name);
    p = جوڑو_attr_and_قدر(p, (char_u *)" term=", 6, رکن_attr);
    p = جوڑو_attr_and_قدر(p, (char_u *)" start=", 7, شروع);
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
    (فراغ)جوڑو_attr_and_قدر(p, (char_u *)" guisp=", 7, guisp);

    do_highlight(hlsetBuf, forceit, FALSE);

    أرجع TRUE;
}

/*
 * "hlset([{highlight_attr}])" function
 * Add or modify highlight groups
 */
    فراغ
f_hlset(typval_T *argvars, typval_T *rettv)
{
    listitem_T	*li;
    dict_T	*dict;

    rettv->vval.v_نمبر = -1;

    إذا (check_for_list_arg(argvars, 0) == FAIL)
	أرجع;

    FOR_ALL_LIST_اشیاء(argvars->vval.v_list, li)
    {
	إذا (li->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_dictionary_required));
	    أرجع;
	}

	dict = li->li_tv.vval.v_dict;
	إذا (!hlg_جوڑو_or_update(dict))
	    أرجع;
    }

    rettv->vval.v_نمبر = 0;
}
#جے_انت

/*
 * If "old" is in the override stack, then update it to "hl_new". Does not free
 * "old".
 */
    فراغ
update_highlight_overrides(hl_override_T *old, hl_override_T *hl_new, صحيح newlen)
{
    إذا (old == عدم)
	أرجع;

    لكل (hl_overrides_T *سیٹ_2 = overrides; سیٹ_2 != عدم; سیٹ_2 = سیٹ_2->اگلا)
    {
	إذا (سیٹ_2->arr == old)
	{
	    سیٹ_2->arr = hl_new;
	    سیٹ_2->len = newlen;
	}
    }
}

/*
 * If "arr" is in the highlight overrides list, then mark it as invalid.
 */
    فراغ
remove_highlight_overrides(hl_override_T *arr)
{
    إذا (arr == عدم || overrides == عدم)
	أرجع;

    لكل (hl_overrides_T *سیٹ_2 = overrides; سیٹ_2 != عدم; سیٹ_2 = سیٹ_2->اگلا)
    {
	إذا (سیٹ_2->arr == arr)
	{
	    سیٹ_2->arr = عدم;
	    اكسر;
	}
    }
}

/*
 * Update highlight_attr[] array. If "update_ids" is true, then update
 * highlight_ids[] array instead.
 */
    ساكن فراغ
سیٹ_highlight_attr(hl_override_T *arr, صحيح len, bool update_ids)
{
    لكل (صحيح i = 0; i < len; i++)
    {
	hl_override_T	*override = arr + i;
	صحيح		hlf = -1, attr;

	إذا (override->from <= 0)
	    // Directly map to highlight_attr
	    hlf = -override->from;
	وإلا
	{
	    لكل (صحيح k = 0; k < HLF_گنتی; k++)
		إذا (override->from == highlight_ids[k])
		{
		    hlf = k;
		    اكسر;
		}

	    إذا (hlf == -1)
		استمر;
	}

	إذا (update_ids)
	{
	    إذا (override->منزل <= 0)
	    {
		إذا (hlf == -override->منزل)
		    highlight_ids[hlf] = -1;
		وإلا
		    highlight_ids[hlf] = highlight_ids[-override->منزل];
	    }
	    وإلا
		highlight_ids[hlf] = override->منزل;
	}
	وإلا
	{
	    إذا (override->منزل <= 0)
		// Directly use highlight_attr[]
		attr = highlight_attr[-override->منزل];
	    وإلا
		attr = syn_id2attr(override->منزل);

	    highlight_attr[hlf] = attr;
	}
    }
}

/*
 * Set the current highlight override to "arr". Returns true if successful
 */
    bool
ڈالو_highlight_overrides(hl_override_T *arr, صحيح len)
{
    // Don't want to do anything if "arr" is already the current override. If
    // "arr" is NULL (but overrides->arr is not), then still push an override,
    // but "->arr" will just be NULL so any previous overrides are cleared.
    إذا (overrides != عدم && overrides->arr == arr)
	أرجع false;

    hl_overrides_T *سیٹ_2;

    سیٹ_2 = ALLOC_ONE(hl_overrides_T);
    إذا (سیٹ_2 == عدم)
	أرجع false;

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
    إذا (arr != عدم)
	سیٹ_highlight_attr(arr, len, false);

    أرجع true;
}

/*
 * Pop the current highlight override (if any) from the stack. This should
 * always be paired with a *successful* push.
 */
    فراغ
نکالو_highlight_overrides(فراغ)
{
    hl_overrides_T *سیٹ_2 = overrides;

    إذا (overrides == عدم)
	أرجع;

    overrides = سیٹ_2->اگلا;

    // Set highlight_attr[] to state before override was pushed.
    حافظہ_نقل(highlight_attr, سیٹ_2->attr, حجم(highlight_attr));
    vim_free(سیٹ_2);
}

/*
 * Parse the 'winhighlight' option and return array. Returns NULL on failure or
 * if empty option. If failure, then errmsg is set.
 */
    ساكن hl_override_T *
parse_winhighlight(char_u *opt, صحيح *len, حرف **errmsg)
{
    char_u	    *p = opt;
    hl_override_T   *arr;
    صحيح		    i = 0;
    صحيح		    عدد = 1;
    صحيح		    n_colons = 0;

    إذا (*p == NUL)
	أرجع عدم;

    // Get number of overrides first so we can allocate array
    طالما ((p = vim_strchr(p, ',')) != عدم)
    {
	p++;
	عدد++;
    }
    p = opt;
    // Check if number of ':' matches number of ','
    طالما ((p = vim_strchr(p, ':')) != عدم)
    {
	p++;
	n_colons++;
    }

    إذا (عدد != n_colons)
    {
	*errmsg = e_invalid_دلیل;
	أرجع عدم;
    }

    arr = ALLOC_MULT(hl_override_T, عدد);
    إذا (arr == عدم)
    {
	*errmsg = e_باہرلا_دا_حافظہ;
	أرجع عدم;
    }

    p = opt;

    طالما (true)
    {
	hl_override_T *override = arr + i++;
	char_u	*fromname = p, *toname;
	char_u	*tmp;
	char_u	**ناں[2] = {&fromname, &toname};
	صحيح	fromlen, tolen;
	صحيح	*lens[2] = {&fromlen, &tolen};
	صحيح	fromid, toid;
	صحيح	*ids[2] = {&fromid, &toid};

	p = vim_strchr(p, ':');

	إذا (p == عدم)
	    اذهب fail;

	fromlen = p - fromname; // Get hl for "from"
	إذا (fromlen == 0)
	    اذهب fail;
	p++; // Skip colon ':'
	إذا (*p == NUL)
	    اذهب fail;

	toname = p;
	tmp = vim_strchr(p, ',');

	// Get hl for "to", must check for no trailing comma in case last
	// element.
	إذا (tmp == عدم)
	    tolen = (صحيح)STRLEN(p);
	وإلا
	{
	    tolen = tmp - toname;
	    p = ++tmp;
	}
	إذا (tolen == 0)
	    اذهب fail;

	لكل (صحيح k = 0; k < 2; k++)
	{
	    char_u  *name = *ناں[k];
	    صحيح	    nlen = *lens[k];

	    إذا (*name == '!')
	    {
		// If starts with "!", then it maps directly to a 'highlight'
		// occasion.
		صحيح hlf;

		إذا (nlen != 2)
		    اذهب fail;

		لكل (hlf = 0; hlf < (صحيح)HLF_گنتی; ++hlf)
		    إذا (hl_flags[hlf] == name[1])
			اكسر;

		إذا (hlf >= HLF_گنتی)
		    اذهب fail;

		*ids[k] = -hlf;
	    }
	    وإلا
	    {
		// Otherwise get the highlight group id.
		إذا (syn_check_گروہ(name, nlen) == 0)
		    اذهب fail;

		*ids[k] = syn_namen2id(name, nlen);
		إذا (*ids[k] == 0)
		    اذهب fail;

		// Always map "Normal" group to HLF_WIN
		إذا (ids[k] == &fromid &&
			STRCMP(HL_جدول()[*ids[k] - 1].sg_name_u, "NORMAL") == 0)
		    *ids[k] = -HLF_WIN;
	    }
	}

	// If fromid == toid, leave it be, this is so that we know HLF_WIN has
	// been set (even to itself e.g. "Normal"), and therefore know to use
	// its attr instead.
	override->from = fromid;
	override->منزل = toid;

	إذا (tmp == عدم)
	    اكسر;
    }

    *len = عدد;
    أرجع arr;
fail:
    vim_free(arr);
    *errmsg = e_invalid_دلیل;
    أرجع عدم;
}

/*
 * Update w_hl for "wp" using given option value. Returns error
 * message on failure, otherwise NULL.
 */
    حرف *
update_winhighlight(win_T *wp, char_u *opt)
{
    حرف	    *err = عدم;
    hl_override_T   *arr;
    صحيح		    عدد = 1;

    arr = parse_winhighlight(opt, &عدد, &err);

    إذا (arr == عدم && err != عدم)
	أرجع err;

    إذا (arr == عدم && wp->w_hl == عدم)
	أرجع عدم;

    update_highlight_overrides(wp->w_hl, arr, عدد);

    vim_free(wp->w_hl);
    wp->w_hl = arr;
    wp->w_hl_len = عدد;

    wp->w_hlfwin_id = hlf_get_id(wp, HLF_WIN);

#جے_تعریف_شدہ FEAT_TERMINAL
    رکن_update_hlfwin(wp);
#جے_انت

#جے_تعریف_شدہ FEAT_TERMINAL
    // May be NULL (such as in after_copy_winopt())
    إذا (wp->w_بفر != عدم)
    {
	// Make sure terminal highlighting is updated
	bool ret = ڈالو_highlight_overrides(wp->w_hl, wp->w_hl_len);

	إذا (wp->w_بفر->b_رکن != عدم)
	    رکن_init_default_colors(wp->w_بفر->b_رکن);
	إذا (ret)
	    نکالو_highlight_overrides();
    }
#جے_انت

    أرجع عدم;
}

/*
 * Get id that the hlf attr is using in window "wp", otherwise zero. Note that
 * -1 may be returned if the hlf was set to itself.
 */
    صحيح
hlf_get_id(win_T *wp, صحيح hlf)
{
    صحيح id;
    ساكن صحيح prev[HLF_گنتی];

    إذا (wp->w_hl == عدم)
	أرجع highlight_ids[hlf];

    حافظہ_نقل(prev, highlight_ids, حجم(highlight_ids));
    سیٹ_highlight_attr(wp->w_hl, wp->w_hl_len, true);
    id = highlight_ids[hlf];
    حافظہ_نقل(highlight_ids, prev, حجم(highlight_ids));

    أرجع id;
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

    إذا (str == عدم)
	أرجع e_باہرلا_دا_حافظہ;

    إذا (*opt != NUL)
	sprintf((حرف *)str, "!(:%s", opt);

    errmsg = update_winhighlight(wp, str);

    إذا (errmsg == عدم)
	سیٹ_string_option_سدھا_وچ_win(wp, (char_u *)"winhighlight", -1,
		str, OPT_FREE|OPT_LOCAL, 0);

    إذا (*opt != NUL)
	حرر(str);
    أرجع errmsg;
}
