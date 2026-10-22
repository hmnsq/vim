/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * optiondefs.h: option definitions
 */

// The options that are local to a window or buffer have "indir" set to one of
// these values.  Special values:
// PV_NONE: global option.
// PV_WIN is added: window-local option
// PV_BUF is added: buffer-local option
// PV_BOTH is added: global option which also has a local value.
#تعریف_کرو PV_BOTH 0x1000
#تعریف_کرو PV_WIN  0x2000
#تعریف_کرو PV_BUF  0x4000
#تعریف_کرو PV_نقاب 0x0fff
#تعریف_کرو OPT_WIN(x)  (idopt_T)(PV_WIN + (صحیح)(x))
#تعریف_کرو OPT_BUF(x)  (idopt_T)(PV_BUF + (صحیح)(x))
#تعریف_کرو OPT_BOTH(x) (idopt_T)(PV_BOTH + (صحیح)(x))

// Definition of the PV_ values for buffer-local options.
// The BV_ values are defined in option.h.
#تعریف_کرو PV_AC		OPT_BOTH(OPT_BUF(BV_AC))
#تعریف_کرو PV_AI		OPT_BUF(BV_AI)
#تعریف_کرو PV_AR		OPT_BOTH(OPT_BUF(BV_AR))
#تعریف_کرو PV_BKC		OPT_BOTH(OPT_BUF(BV_BKC))
#تعریف_کرو PV_BH		OPT_BUF(BV_BH)
#تعریف_کرو PV_BT		OPT_BUF(BV_BT)
#اگر_تعریف_شدہ FEAT_QUICKFIX
# define PV_EFM		OPT_BOTH(OPT_BUF(BV_EFM))
# define PV_GEFM	OPT_BOTH(OPT_BUF(BV_GEFM))
# define PV_GP		OPT_BOTH(OPT_BUF(BV_GP))
# define PV_MP		OPT_BOTH(OPT_BUF(BV_MP))
#اگر_ختم
#تعریف_کرو PV_BIN		OPT_BUF(BV_BIN)
#تعریف_کرو PV_BL		OPT_BUF(BV_BL)
#تعریف_کرو PV_BOMB		OPT_BUF(BV_BOMB)
#تعریف_کرو PV_CI		OPT_BUF(BV_CI)
#تعریف_کرو PV_CIN		OPT_BUF(BV_CIN)
#تعریف_کرو PV_CINK		OPT_BUF(BV_CINK)
#تعریف_کرو PV_CINO		OPT_BUF(BV_CINO)
#تعریف_کرو PV_CINSD	OPT_BUF(BV_CINSD)
#تعریف_کرو PV_CINW		OPT_BUF(BV_CINW)
#تعریف_کرو PV_CM		OPT_BOTH(OPT_BUF(BV_CM))
#تعریف_کرو PV_CMS		OPT_BUF(BV_CMS)
#تعریف_کرو PV_COM		OPT_BUF(BV_COM)
#تعریف_کرو PV_COT		OPT_BOTH(OPT_BUF(BV_COT))
#تعریف_کرو PV_CPT		OPT_BUF(BV_CPT)
#تعریف_کرو PV_DICT		OPT_BOTH(OPT_BUF(BV_DICT))
#اگر_تعریف_شدہ FEAT_DIFF
# define PV_DIA		OPT_BOTH(OPT_BUF(BV_DIA))
#اگر_ختم
#تعریف_کرو PV_TSR		OPT_BOTH(OPT_BUF(BV_TSR))
#تعریف_کرو PV_FFU		OPT_BOTH(OPT_BUF(BV_FFU))
#تعریف_کرو PV_CSL		OPT_BUF(BV_CSL)
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
# define PV_CFU		OPT_BUF(BV_CFU)
#اگر_ختم
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
# define PV_DEF		OPT_BOTH(OPT_BUF(BV_DEF))
# define PV_INC		OPT_BOTH(OPT_BUF(BV_INC))
#اگر_ختم
#تعریف_کرو PV_EOF		OPT_BUF(BV_EOF)
#تعریف_کرو PV_EOL		OPT_BUF(BV_EOL)
#تعریف_کرو PV_FIXEOL	OPT_BUF(BV_FIXEOL)
#تعریف_کرو PV_EP		OPT_BOTH(OPT_BUF(BV_EP))
#تعریف_کرو PV_ET		OPT_BUF(BV_ET)
#تعریف_کرو PV_FENC		OPT_BUF(BV_FENC)
#اگر defined(FEAT_BEVAL) && defined(FEAT_EVAL)
# define PV_BEXPR	OPT_BOTH(OPT_BUF(BV_BEXPR))
#اگر_ختم
#تعریف_کرو PV_FP		OPT_BOTH(OPT_BUF(BV_FP))
#اگر_تعریف_شدہ FEAT_EVAL
# define PV_FEX		OPT_BUF(BV_FEX)
#اگر_ختم
#تعریف_کرو PV_FF		OPT_BUF(BV_FF)
#تعریف_کرو PV_FLP		OPT_BUF(BV_FLP)
#تعریف_کرو PV_FO		OPT_BUF(BV_FO)
#اگر_تعریف_شدہ HAVE_FSYNC
# define PV_FS		OPT_BOTH(OPT_BUF(BV_FS))
#اگر_ختم
#تعریف_کرو PV_FT		OPT_BUF(BV_FT)
#تعریف_کرو PV_IMI		OPT_BUF(BV_IMI)
#تعریف_کرو PV_IMS		OPT_BUF(BV_IMS)
#اگر defined(FEAT_EVAL)
# define PV_INDE	OPT_BUF(BV_INDE)
# define PV_INDK	OPT_BUF(BV_INDK)
#اگر_ختم
#اگر defined(FEAT_ڈھونڈو_ID) && defined(FEAT_EVAL)
# define PV_INEX	OPT_BUF(BV_INEX)
#اگر_ختم
#تعریف_کرو PV_INF		OPT_BUF(BV_INF)
#تعریف_کرو PV_ISK		OPT_BUF(BV_ISK)
#اگر_تعریف_شدہ FEAT_CRYPT
# define PV_کلید		OPT_BUF(BV_کلید)
#اگر_ختم
#اگر_تعریف_شدہ FEAT_KEYMAP
# define PV_KMAP	OPT_BUF(BV_KMAP)
#اگر_ختم
#تعریف_کرو PV_KP		OPT_BOTH(OPT_BUF(BV_KP))
#تعریف_کرو PV_LISP		OPT_BUF(BV_LISP)
#تعریف_کرو PV_LOP		OPT_BUF(BV_LOP)
#تعریف_کرو PV_LW		OPT_BOTH(OPT_BUF(BV_LW))
#تعریف_کرو PV_MENC		OPT_BOTH(OPT_BUF(BV_MENC))
#تعریف_کرو PV_MA		OPT_BUF(BV_MA)
#تعریف_کرو PV_ML		OPT_BUF(BV_ML)
#تعریف_کرو PV_MOD		OPT_BUF(BV_MOD)
#تعریف_کرو PV_MPS		OPT_BUF(BV_MPS)
#تعریف_کرو PV_NF		OPT_BUF(BV_NF)
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
# define PV_OFU		OPT_BUF(BV_OFU)
#اگر_ختم
#تعریف_کرو PV_راستہ		OPT_BOTH(OPT_BUF(BV_راستہ))
#تعریف_کرو PV_PI		OPT_BUF(BV_PI)
#تعریف_کرو PV_QE		OPT_BUF(BV_QE)
#تعریف_کرو PV_RO		OPT_BUF(BV_RO)
#تعریف_کرو PV_SI		OPT_BUF(BV_SI)
#تعریف_کرو PV_SN		OPT_BUF(BV_SN)
#اگر_تعریف_شدہ FEAT_SYN_HL
# define PV_SMC		OPT_BUF(BV_SMC)
# define PV_SYN		OPT_BUF(BV_SYN)
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SPELL
# define PV_SPC		OPT_BUF(BV_SPC)
# define PV_SPF		OPT_BUF(BV_SPF)
# define PV_SPL		OPT_BUF(BV_SPL)
# define PV_SPO		OPT_BUF(BV_SPO)
#اگر_ختم
#تعریف_کرو PV_STS		OPT_BUF(BV_STS)
#تعریف_کرو PV_SUA		OPT_BUF(BV_SUA)
#تعریف_کرو PV_SW		OPT_BUF(BV_SW)
#تعریف_کرو PV_SWF		OPT_BUF(BV_SWF)
#اگر_تعریف_شدہ FEAT_EVAL
# define PV_TFU		OPT_BUF(BV_TFU)
#اگر_ختم
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
# define PV_TSRFU	OPT_BOTH(OPT_BUF(BV_TSRFU))
#اگر_ختم
#تعریف_کرو PV_لیبل_فہرست		OPT_BOTH(OPT_BUF(BV_لیبل_فہرست))
#تعریف_کرو PV_TC		OPT_BOTH(OPT_BUF(BV_TC))
#تعریف_کرو PV_TS		OPT_BUF(BV_TS)
#تعریف_کرو PV_TW		OPT_BUF(BV_TW)
#تعریف_کرو PV_TX		OPT_BUF(BV_TX)
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
# define PV_UDF		OPT_BUF(BV_UDF)
#اگر_ختم
#تعریف_کرو PV_WM		OPT_BUF(BV_WM)
#اگر_تعریف_شدہ FEAT_VARTABS
# define PV_VSTS	OPT_BUF(BV_VSTS)
# define PV_VTS		OPT_BUF(BV_VTS)
#اگر_ختم

// Definition of the PV_ values for window-local options.
// The WV_ values are defined in option.h.
#تعریف_کرو PV_LIST		OPT_WIN(WV_LIST)
#اگر_تعریف_شدہ FEAT_ARABIC
# define PV_ARAB	OPT_WIN(WV_ARAB)
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
# define PV_BRI		OPT_WIN(WV_BRI)
# define PV_BRIOPT	OPT_WIN(WV_BRIOPT)
#اگر_ختم
#تعریف_کرو PV_WCR		OPT_WIN(WV_WCR)
#اگر_تعریف_شدہ FEAT_DIFF
# define PV_DIFF	OPT_WIN(WV_DIFF)
#اگر_ختم
#تعریف_کرو PV_EIW		OPT_WIN(WV_EIW)
#اگر_تعریف_شدہ FEAT_FOLDING
# define PV_FDC		OPT_WIN(WV_FDC)
# define PV_FEN		OPT_WIN(WV_FEN)
# define PV_FDI		OPT_WIN(WV_FDI)
# define PV_FDL		OPT_WIN(WV_FDL)
# define PV_FDM		OPT_WIN(WV_FDM)
# define PV_FML		OPT_WIN(WV_FML)
# define PV_FDN		OPT_WIN(WV_FDN)
# ifdef FEAT_EVAL
#  define PV_FDE	OPT_WIN(WV_FDE)
#  define PV_FDT	OPT_WIN(WV_FDT)
# endif
# define PV_FMR		OPT_WIN(WV_FMR)
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
# define PV_LBR		OPT_WIN(WV_LBR)
#اگر_ختم
#تعریف_کرو PV_LCS		OPT_BOTH(OPT_WIN(WV_LCS))
#تعریف_کرو PV_FCS		OPT_BOTH(OPT_WIN(WV_FCS))
#تعریف_کرو PV_NU		OPT_WIN(WV_NU)
#تعریف_کرو PV_RNU		OPT_WIN(WV_RNU)
#تعریف_کرو PV_VE		OPT_BOTH(OPT_WIN(WV_VE))
#اگر_تعریف_شدہ FEAT_LINEBREAK
# define PV_NUW		OPT_WIN(WV_NUW)
#اگر_ختم
#اگر defined(FEAT_QUICKFIX)
# define PV_PVW		OPT_WIN(WV_PVW)
# define PV_LHI         OPT_WIN(WV_LHI)
#اگر_ختم
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
# define PV_RL		OPT_WIN(WV_RL)
# define PV_RLC		OPT_WIN(WV_RLC)
#اگر_ختم
#تعریف_کرو PV_SCBIND	OPT_WIN(WV_SCBIND)
#تعریف_کرو PV_SCROLL	OPT_WIN(WV_SCROLL)
#تعریف_کرو PV_SMS		OPT_WIN(WV_SMS)
#تعریف_کرو PV_SISO		OPT_BOTH(OPT_WIN(WV_SISO))
#تعریف_کرو PV_SO		OPT_BOTH(OPT_WIN(WV_SO))
#تعریف_کرو PV_SOP		OPT_BOTH(OPT_WIN(WV_SOP))
#اگر_تعریف_شدہ FEAT_SPELL
# define PV_SPELL	OPT_WIN(WV_SPELL)
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
# define PV_CUC		OPT_WIN(WV_CUC)
# define PV_CUL		OPT_WIN(WV_CUL)
# define PV_CULOPT	OPT_WIN(WV_CULOPT)
# define PV_CC		OPT_WIN(WV_CC)
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
# define PV_SBR		OPT_BOTH(OPT_WIN(WV_SBR))
#اگر_ختم
#اگر_تعریف_شدہ FEAT_STL_OPT
# define PV_STL		OPT_BOTH(OPT_WIN(WV_STL))
# define PV_STLO	OPT_BOTH(OPT_WIN(WV_STLO))
#اگر_ختم
#تعریف_کرو PV_UL		OPT_BOTH(OPT_BUF(BV_UL))
#تعریف_کرو PV_WFB		OPT_WIN(WV_WFB)
#تعریف_کرو PV_WFH		OPT_WIN(WV_WFH)
#تعریف_کرو PV_WFW		OPT_WIN(WV_WFW)
#تعریف_کرو PV_WHL		OPT_WIN(WV_WHL)
#تعریف_کرو PV_WRAP		OPT_WIN(WV_WRAP)
#تعریف_کرو PV_CRBIND	OPT_WIN(WV_CRBIND)
#اگر_تعریف_شدہ FEAT_CONCEAL
# define PV_COCU	OPT_WIN(WV_COCU)
# define PV_COLE	OPT_WIN(WV_COLE)
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
# define PV_TWK		OPT_WIN(WV_TWK)
# define PV_TWS		OPT_WIN(WV_TWS)
# define PV_TWSL	OPT_BUF(BV_TWSL)
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGNS
# define PV_SCL		OPT_WIN(WV_SCL)
#اگر_ختم

// WV_ and BV_ values get typecasted to this for the "indir" field
قسمتعریف شمار
{
    PV_NONE = 0,
    PV_MAXVAL = 0xffff    // to avoid warnings for value out of range
} idopt_T;

// Options local to a window have a value local to a buffer and global to all
// buffers.  Indicate this by setting "var" to VAR_WIN.
#تعریف_کرو VAR_WIN ((char_u *)-1)

// Saved values for when 'bin' is set.
ساکن صحیح	p_et_nobin;
ساکن صحیح	p_ml_nobin;
ساکن طویل	p_tw_nobin;
ساکن طویل	p_wm_nobin;

// Saved values for when 'paste' is set
ساکن صحیح	p_ai_nopaste;
ساکن صحیح	p_et_nopaste;
ساکن طویل	p_sts_nopaste;
ساکن طویل	p_tw_nopaste;
ساکن طویل	p_wm_nopaste;
#اگر_تعریف_شدہ FEAT_VARTABS
ساکن char_u	*p_vsts_nopaste;
#اگر_ختم

ساخت vimoption
{
    حرف	*fullname;	// full option name
    حرف	*shortname;	// permissible abbreviation
    long_u	flags;		// see in option.h
    char_u	*var;		// global option: pointer to variable;
				// window-local option: VAR_WIN;
				// buffer-local option: global value
    idopt_T	indir;		// global option: PV_NONE;
				// local option: indirect option index
    // callback function to invoke after an option is modified to validate and
    // apply the new value.
    opt_did_سیٹ_cb_T	opt_did_سیٹ_cb;

    // callback function to invoke when expanding possible values on the
    // cmdline. Only useful for string options.
    opt_expand_cb_T	opt_expand_cb;

    char_u	*def_val[2];	// default values for variable (vi and vim)
#اگر_تعریف_شدہ FEAT_EVAL
    sctx_T	script_ctx;	// script context where the option was last set
# define SCTX_INIT , {0, 0, 0, 1}
#ورنہ
# define SCTX_INIT
#اگر_ختم
};

#تعریف_کرو VI_DEFAULT  0	    // def_val[VI_DEFAULT] is Vi default value
#تعریف_کرو VIM_DEFAULT 1	    // def_val[VIM_DEFAULT] is Vim default value

#تعریف_کرو ISK_LATIN1  (char_u *)"@,48-57,_,192-255"

// 'isprint' for latin1 is also used for MS-Windows cp1252, where 0x80 is used
// for the currency sign.
#اگر defined(MSWIN)
# define ISP_LATIN1 (char_u *)"@,~-255"
#ورنہ
# define ISP_LATIN1 (char_u *)"@,161-255"
#اگر_ختم

#تعریف_کرو HIGHLIGHT_INIT "8:SpecialKey,~:EndOfBuffer,@:NonText,d:Directory,e:ErrorMsg,i:IncSearch,l:Search,y:CurSearch,m:MoreMsg,M:ModeMsg,n:LineNr,a:LineNrAbove,b:LineNrBelow,N:CursorLineNr,G:CursorLineSign,O:CursorLineFold,r:Question,s:StatusLine,S:StatusLineNC,c:VertSplit,|:VertSplitNC,t:Title,v:Visual,V:VisualNOS,w:WarningMsg,W:WildMenu,f:Folded,F:FoldColumn,A:DiffAdd,C:DiffChange,D:DiffDelete,T:DiffText,E:DiffTextAdd,>:SignColumn,-:Conceal,B:SpellBad,P:SpellCap,R:SpellRare,L:SpellLocal,+:Pmenu,=:PmenuSel,k:PmenuMatch,<:PmenuMatchSel,[:PmenuKind,]:PmenuKindSel,{:PmenuExtra,}:PmenuExtraSel,x:PmenuSbar,X:PmenuThumb,j:PmenuBorder,H:PmenuShadow,p:Popup,J:PopupBorder,Q:PopupTitle,*:TabLine,#:TabLineSel,_:TabLineFill,!:CursorColumn,.:CursorLine,o:ColorColumn,q:QuickFixLine,z:StatusLineTerm,Z:StatusLineTermNC,g:MsgArea,h:ComplMatchIns,%:TabPanel,^:TabPanelSel,&:TabPanelFill,I:PreInsert"

// Default python version for pyx* commands
#اگر defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
# define DEFAULT_PYTHON_VER	0
#وگرنہ_اگر defined(FEAT_PYTHON3)
# define DEFAULT_PYTHON_VER	3
#وگرنہ_اگر defined(FEAT_PYTHON)
# define DEFAULT_PYTHON_VER	2
#ورنہ
# define DEFAULT_PYTHON_VER	0
#اگر_ختم

// used for 'cinkeys' and 'indentkeys'
#تعریف_کرو INDENTKEYS_DEFAULT (char_u *)"0{,0},0),0],:,0#,!^F,o,O,e"

// options[] is initialized here.
// The order of the options MUST be alphabetic for ":set all" and findoption().
// All option names MUST start with a lowercase letter (for findoption()).
// Exception: "t_" options are at the end.
// The options with a NULL variable are 'hidden': a set command for them is
// ignored and they are not printed.
ساکن ساخت vimoption options[] =
{
    {"aleph",	    "al",   P_عدد|P_VI_DEF|P_CURSWANT,
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
			    (char_u *)&p_aleph, PV_NONE,
#ورنہ
			    (char_u *)تہی, PV_NONE,
#اگر_ختم
			    تہی, تہی,
			    {
#اگر defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
			    (char_u *)128L,
#ورنہ
			    (char_u *)224L,
#اگر_ختم
					    (char_u *)0L} SCTX_INIT},
    {"allowrevins", "ari",  P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
			    (char_u *)&p_ari, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"altkeymap",   "akm",  P_BOOL|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"ambiwidth",  "ambw",  P_STRING|P_VI_DEF|P_RCLR,
			    (char_u *)&p_ambw, PV_NONE, did_سیٹ_ambiwidth, expand_سیٹ_ambiwidth,
			    {(char_u *)"single", (char_u *)0L}
			    SCTX_INIT},
    {"antialias",   "anti", P_BOOL|P_VI_DEF|P_VIM|P_RCLR,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)FALSE}
			    SCTX_INIT},
    {"arabic",	    "arab", P_BOOL|P_VI_DEF|P_VIM|P_CURSWANT,
#اگر_تعریف_شدہ FEAT_ARABIC
			    (char_u *)VAR_WIN, PV_ARAB, did_سیٹ_arabic, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"arabicshape", "arshape", P_BOOL|P_VI_DEF|P_VIM|P_RCLR,
#اگر_تعریف_شدہ FEAT_ARABIC
			    (char_u *)&p_arshape, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"autochdir",  "acd",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_AUTOCHDIR
			    (char_u *)&p_acd, PV_NONE, did_سیٹ_autochdir, تہی,
			    {(char_u *)FALSE, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
#اگر_تعریف_شدہ ELAPSED_FUNC
    {"autocomplete",  "ac", P_BOOL|P_VI_DEF,
			    (char_u *)&p_ac, PV_AC, تہی,
			    تہی, {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autocompletedelay", "acl", P_عدد|P_VI_DEF,
			    (char_u *)&p_acl, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"autocompletetimeout", "act", P_عدد|P_VI_DEF,
			    (char_u *)&p_act, PV_NONE, تہی, تہی,
			    {(char_u *)80L, (char_u *)0L} SCTX_INIT},
#اگر_ختم
    {"autoindent",  "ai",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ai, PV_AI, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autoprint",   "ap",   P_BOOL|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autoread",    "ar",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ar, PV_AR, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autoshelldir",  "asd",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_AUTOSHELLDIR
			    (char_u *)&p_asd, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"autowrite",   "aw",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_aw, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autowriteall","awa",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_awa, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"background",  "bg",   P_STRING|P_VI_DEF|P_RCLR|P_HLONLY,
			    (char_u *)&p_bg, PV_NONE, did_سیٹ_background, expand_سیٹ_background,
			    {
#اگر (defined(MSWIN)) && !defined(FEAT_GUI)
			    (char_u *)"dark",
#ورنہ
			    (char_u *)"light",
#اگر_ختم
					    (char_u *)0L} SCTX_INIT},
    {"backspace",   "bs",   P_STRING|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_bs, PV_NONE, did_سیٹ_backspace, expand_سیٹ_backspace,
			    {(char_u *)"", (char_u *)"indent,eol,start"} SCTX_INIT},
    {"backup",	    "bk",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_bk, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"backupcopy",  "bkc",  P_STRING|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_bkc, PV_BKC, did_سیٹ_backupcopy, expand_سیٹ_backupcopy,
#اگر_تعریف_شدہ UNIX
			    {(char_u *)"yes", (char_u *)"auto"}
#ورنہ
			    {(char_u *)"auto", (char_u *)"auto"}
#اگر_ختم
			    SCTX_INIT},
    {"backupdir",   "bdir", P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA
							    |P_NODUP|P_SECURE,
			    (char_u *)&p_bdir, PV_NONE, تہی, تہی,
			    {(char_u *)DFLT_BDIR, (char_u *)0L} SCTX_INIT},
    {"backupext",   "bex",  P_STRING|P_VI_DEF|P_NFNAME,
			    (char_u *)&p_bex, PV_NONE,
			    did_سیٹ_backupext_or_patchmode, تہی,
			    {
#اگر_تعریف_شدہ VMS
			    (char_u *)"_",
#ورنہ
			    (char_u *)"~",
#اگر_ختم
					    (char_u *)0L} SCTX_INIT},
    {"backupskip",  "bsk",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_bsk, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"balloondelay","bdlay",P_عدد|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_BEVAL
			    (char_u *)&p_bdlay, PV_NONE, تہی, تہی,
			    {(char_u *)600L, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"ballooneval", "beval",P_BOOL|P_VI_DEF|P_NO_MKRC,
#اگر_تعریف_شدہ FEAT_BEVAL_GUI
			    (char_u *)&p_beval, PV_NONE, did_سیٹ_ballooneval, تہی,
			    {(char_u *)FALSE, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"balloonevalterm", "bevalterm",P_BOOL|P_VI_DEF|P_NO_MKRC,
#اگر_تعریف_شدہ FEAT_BEVAL_رکن
			    (char_u *)&p_bevalterm, PV_NONE,
			    did_سیٹ_balloonevalterm, تہی,
			    {(char_u *)FALSE, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"balloonexpr", "bexpr", P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_MLE,
#اگر defined(FEAT_BEVAL) && defined(FEAT_EVAL)
			    (char_u *)&p_bexpr, PV_BEXPR, did_سیٹ_optexpr, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"beautify",    "bf",   P_BOOL|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"belloff",      "bo",  P_STRING|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_bo, PV_NONE, did_سیٹ_belloff, expand_سیٹ_belloff,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"binary",	    "bin",  P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_bin, PV_BIN, did_سیٹ_ثنائی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"bioskey",	    "biosk",P_BOOL|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"bomb",	    تہی,   P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_bomb, PV_BOMB,
			    did_سیٹ_eof_eol_fixeol_bomb, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"breakat",	    "brk",  P_STRING|P_VI_DEF|P_RALL|P_FLAGLIST,
#اگر_تعریف_شدہ FEAT_LINEBREAK
			    (char_u *)&p_breakat, PV_NONE, did_سیٹ_breakat, تہی,
			    {(char_u *)" \t!@*-+;:,./?", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"breakindent",   "bri",  P_BOOL|P_VI_DEF|P_VIM|P_RWIN,
#اگر_تعریف_شدہ FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_BRI, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"breakindentopt", "briopt", P_STRING|P_ALLOCED|P_VI_DEF|P_RBUF
						  |P_ONECOMMA|P_NODUP,
#اگر_تعریف_شدہ FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_BRIOPT,
			    did_سیٹ_breakindentopt, expand_سیٹ_breakindentopt,
			    {(char_u *)"", (char_u *)تہی}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)تہی}
#اگر_ختم
			    SCTX_INIT},
    {"browsedir",   "bsdir",P_STRING|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_BROWSE
			    (char_u *)&p_bsdir, PV_NONE, did_سیٹ_browsedir, expand_سیٹ_browsedir,
			    {(char_u *)"last", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"bufhidden",   "bh",   P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_bh, PV_BH, did_سیٹ_bufhidden, expand_سیٹ_bufhidden,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"buflisted",   "bl",   P_BOOL|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_bl, PV_BL, did_سیٹ_buflisted, تہی,
			    {(char_u *)1L, (char_u *)0L}
			    SCTX_INIT},
    {"buftype",	    "bt",   P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_bt, PV_BT, did_سیٹ_buftype, expand_سیٹ_buftype,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"casemap",	    "cmp",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cmp, PV_NONE, did_سیٹ_casemap, expand_سیٹ_casemap,
			    {(char_u *)"internal,keepascii", (char_u *)0L}
			    SCTX_INIT},
    {"cdhome",	    "cdh",  P_BOOL|P_VI_DEF|P_VIM|P_SECURE,
			    (char_u *)&p_cdh, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L}
			    SCTX_INIT},
    {"cdpath",	    "cd",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE|P_COMMA|P_NODUP,
			    (char_u *)&p_cdpath, PV_NONE, تہی, تہی,
			    {(char_u *)",,", (char_u *)0L}
			    SCTX_INIT},
    {"cedit",	    تہی,   P_STRING,
			    (char_u *)&p_cedit, PV_NONE, did_سیٹ_cedit, تہی,
			    {(char_u *)"", (char_u *)CTRL_F_STR}
			    SCTX_INIT},
    {"charconvert",  "ccv", P_STRING|P_VI_DEF|P_SECURE,
#اگر defined(FEAT_EVAL)
			    (char_u *)&p_ccv, PV_NONE, did_سیٹ_optexpr, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"chistory",    "chi",  P_عدد|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_QUICKFIX
			    (char_u *)&p_chi, PV_NONE, did_سیٹ_xhistory, تہی,
			    {(char_u *)10L, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"cindent",	    "cin",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_cin, PV_CIN, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cinkeys",	    "cink", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cink, PV_CINK, تہی, تہی,
			    {INDENTKEYS_DEFAULT, (char_u *)0L} SCTX_INIT},
    {"cinoptions",  "cino", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cino, PV_CINO, did_سیٹ_cinoptions, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"cinscopedecls", "cinsd", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cinsd, PV_CINSD, تہی, تہی,
			    {(char_u *)"public,protected,private", (char_u *)0L}
			    SCTX_INIT},
    {"cinwords",    "cinw", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cinw, PV_CINW, تہی, تہی,
			    {(char_u *)"if,else,while,do,for,switch",
				(char_u *)0L}
			    SCTX_INIT},
    {"clipboard",   "cb",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شدہ HAVE_CLIPMETHOD
			    (char_u *)&p_cb, PV_NONE, did_سیٹ_clipboard, expand_سیٹ_clipboard,
# اگر defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
			    {(char_u *)"autoselect,exclude:cons\\|linux",
							       (char_u *)0L}
# ورنہ
			    {(char_u *)"", (char_u *)0L}
# endif
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"clipmethod", "cpm",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شدہ HAVE_CLIPMETHOD
			    (char_u *)&p_cpm, PV_NONE, did_سیٹ_clipmethod, expand_سیٹ_clipmethod,
# ifdef UNIX
			    {(char_u *)"wayland,x11", (char_u *)0L}
# elif defined(VMS)
			    {(char_u *)"x11", (char_u *)0L}
# ورنہ
			    {(char_u *)"", (char_u *)0L}
# endif
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"cmdheight",   "ch",   P_عدد|P_VI_DEF|P_RALL,
			    (char_u *)&p_ch, PV_NONE, did_سیٹ_cmdheight, تہی,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"cmdwinheight", "cwh", P_عدد|P_VI_DEF,
			    (char_u *)&p_cwh, PV_NONE, تہی, تہی,
			    {(char_u *)7L, (char_u *)0L} SCTX_INIT},
    {"colorcolumn", "cc",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_RWIN|P_HLONLY,
#اگر_تعریف_شدہ FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CC, did_سیٹ_colorcolumn, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"columns",	    "co",   P_عدد|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RCLR,
			    (char_u *)&Columns, PV_NONE, تہی, تہی,
			    {(char_u *)80L, (char_u *)0L} SCTX_INIT},
    {"comments",    "com",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_com, PV_COM, did_سیٹ_comments, تہی,
			    {(char_u *)"s1:/*,mb:*,ex:*/,://,b:#,:%,:XCOMM,n:>,fb:-",
				(char_u *)0L}
			    SCTX_INIT},
    {"commentstring", "cms", P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)&p_cms, PV_CMS, did_سیٹ_commentstring, تہی,
			    {(char_u *)"/* %s */", (char_u *)0L}
			    SCTX_INIT},
			    // P_PRI_MKRC isn't needed here, optval_default()
			    // always returns TRUE for 'compatible'
    {"compatible",  "cp",   P_BOOL|P_RALL,
			    (char_u *)&p_cp, PV_NONE, did_سیٹ_compatible, تہی,
			    {(char_u *)TRUE, (char_u *)FALSE} SCTX_INIT},
    {"complete",    "cpt",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP|P_MLE,
			    (char_u *)&p_cpt, PV_CPT, did_سیٹ_complete, expand_سیٹ_complete,
			    {(char_u *)".,w,b,u,t,i", (char_u *)0L}
			    SCTX_INIT},
    {"completefunc", "cfu", P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
			    (char_u *)&p_cfu, PV_CFU, did_سیٹ_completefunc, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"completefuzzycollect", "cfc", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cfc, PV_NONE, did_سیٹ_completefuzzycollect, expand_سیٹ_completefuzzycollect,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"completeitemalign", "cia", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cia, PV_NONE, did_سیٹ_completeitemalign, تہی,
			    {(char_u *)"abbr,kind,menu", (char_u *)0L}
			    SCTX_INIT},
    {"completeopt",   "cot",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cot, PV_COT, did_سیٹ_completeopt, expand_سیٹ_completeopt,
			    {(char_u *)"menu,preview", (char_u *)0L}
			    SCTX_INIT},
    {"completepopup", "cpp", P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON,
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
			    (char_u *)&p_cpp, PV_NONE, did_سیٹ_completepopup, expand_سیٹ_completepopup,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"completeslash",   "csl",  P_STRING|P_VI_DEF|P_VIM,
#اگر defined(BACKSLASH_اندر_FILENAME)
			    (char_u *)&p_csl, PV_CSL, did_سیٹ_completeslash, expand_سیٹ_completeslash,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"completetimeout", "cto", P_عدد|P_VI_DEF,
			    (char_u *)&p_cto, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"concealcursor","cocu", P_STRING|P_ALLOCED|P_RWIN|P_VI_DEF|P_FLAGLIST,
#اگر_تعریف_شدہ FEAT_CONCEAL
			    (char_u *)VAR_WIN, PV_COCU, did_سیٹ_concealcursor, expand_سیٹ_concealcursor,
			    {(char_u *)"", (char_u *)تہی}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"conceallevel","cole", P_عدد|P_RWIN|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_CONCEAL
			    (char_u *)VAR_WIN, PV_COLE, did_سیٹ_conceallevel, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)0L, (char_u *)0L}
			    SCTX_INIT},
    {"confirm",     "cf",   P_BOOL|P_VI_DEF,
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			    (char_u *)&p_confirm, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"conskey",	    "consk",P_BOOL|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"copyindent",  "ci",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_ci, PV_CI, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cpoptions",   "cpo",  P_STRING|P_VIM|P_RALL|P_FLAGLIST,
			    (char_u *)&p_cpo, PV_NONE, did_سیٹ_cpoptions, expand_سیٹ_cpoptions,
			    {(char_u *)CPO_VI, (char_u *)CPO_VIM}
			    SCTX_INIT},
    {"cryptmethod", "cm",   P_STRING|P_ALLOCED|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_CRYPT
			    (char_u *)&p_cm, PV_CM, did_سیٹ_cryptmethod, expand_سیٹ_cryptmethod,
			    {(char_u *)"blowfish2", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"cscopepathcomp", "cspc", P_عدد|P_VI_DEF|P_VIM,
#اگر_تعریف_شدہ FEAT_CSCOPE
			    (char_u *)&p_cspc, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopeprg",   "csprg", P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر_تعریف_شدہ FEAT_CSCOPE
			    (char_u *)&p_csprg, PV_NONE, تہی, تہی,
			    {(char_u *)"cscope", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"cscopequickfix", "csqf", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
			    (char_u *)&p_csqf, PV_NONE,
			    did_سیٹ_cscopequickfix, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"cscoperelative", "csre", P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شدہ FEAT_CSCOPE
			    (char_u *)&p_csre, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopetag",   "cst",  P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شدہ FEAT_CSCOPE
			    (char_u *)&p_cst, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopetagorder", "csto", P_عدد|P_VI_DEF|P_VIM,
#اگر_تعریف_شدہ FEAT_CSCOPE
			    (char_u *)&p_csto, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopeverbose", "csverb", P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شدہ FEAT_CSCOPE
			    (char_u *)&p_csverbose, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cursorbind",  "crb",  P_BOOL|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_CRBIND, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cursorcolumn", "cuc", P_BOOL|P_VI_DEF|P_RWIN|P_HLONLY,
#اگر_تعریف_شدہ FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CUC, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cursorline",   "cul", P_BOOL|P_VI_DEF|P_RWIN|P_HLONLY,
#اگر_تعریف_شدہ FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CUL, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cursorlineopt", "culopt", P_STRING|P_VI_DEF|P_RWIN|P_HLONLY|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شدہ FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CULOPT,
			    did_سیٹ_cursorlineopt, expand_سیٹ_cursorlineopt,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)"both", (char_u *)0L} SCTX_INIT},
    {"debug",	    تہی,   P_STRING|P_VI_DEF,
			    (char_u *)&p_debug, PV_NONE, did_سیٹ_debug, expand_سیٹ_debug,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"define",	    "def",  P_STRING|P_ALLOCED|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
			    (char_u *)&p_def, PV_DEF, تہی, تہی,
			    {(char_u *)"^\\s*#\\s*define", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"delcombine", "deco",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_deco, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"dictionary",  "dict", P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA|P_NODUP|P_NDNAME,
			    (char_u *)&p_dict, PV_DICT, تہی, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"diff",	    تہی,   P_BOOL|P_VI_DEF|P_RWIN|P_NOGLOB,
#اگر_تعریف_شدہ FEAT_DIFF
			    (char_u *)VAR_WIN, PV_DIFF, did_سیٹ_diff, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"diffanchors", "dia",  P_STRING|P_VI_DEF|P_ONECOMMA,
#اگر_تعریف_شدہ FEAT_DIFF
			    (char_u *)&p_dia, PV_DIA, did_سیٹ_diffanchors, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)"", (char_u *)تہی} SCTX_INIT},
    {"diffexpr",    "dex",  P_STRING|P_VI_DEF|P_SECURE|P_CURSWANT,
#اگر defined(FEAT_DIFF) && defined(FEAT_EVAL)
			    (char_u *)&p_dex, PV_NONE, did_سیٹ_optexpr, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"diffopt",	    "dip",  P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN|P_ONECOMMA|P_COLON
								     |P_NODUP,
#اگر_تعریف_شدہ FEAT_DIFF
			    (char_u *)&p_dip, PV_NONE, did_سیٹ_diffopt, expand_سیٹ_diffopt,
			    {(char_u *)"internal,filler,closeoff,indent-heuristic,inline:char",
								(char_u *)تہی}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)تہی}
#اگر_ختم
			    SCTX_INIT},
    {"digraph",	    "dg",   P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شدہ FEAT_DIGRAPHS
			    (char_u *)&p_dg, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"directory",   "dir",  P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA
							    |P_NODUP|P_SECURE,
			    (char_u *)&p_dir, PV_NONE, تہی, تہی,
			    {(char_u *)DFLT_DIR, (char_u *)0L} SCTX_INIT},
    {"display",	    "dy",   P_STRING|P_VI_DEF|P_ONECOMMA|P_RALL|P_NODUP,
			    (char_u *)&p_dy, PV_NONE, did_سیٹ_display, expand_سیٹ_display,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"eadirection", "ead",  P_STRING|P_VI_DEF,
			    (char_u *)&p_ead, PV_NONE, did_سیٹ_eadirection, expand_سیٹ_eadirection,
			    {(char_u *)"both", (char_u *)0L}
			    SCTX_INIT},
    {"edcompatible","ed",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ed, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"emoji",      "emo",   P_BOOL|P_VI_DEF|P_RCLR,
			    (char_u *)&p_emoji, PV_NONE, did_سیٹ_ambiwidth, تہی,
			    {(char_u *)TRUE, (char_u *)0L}
			    SCTX_INIT},
    {"encoding",    "enc",  P_STRING|P_VI_DEF|P_RCLR|P_NO_ML,
			    (char_u *)&p_enc, PV_NONE, did_سیٹ_encoding, expand_سیٹ_encoding,
			    {(char_u *)ENC_DFLT, (char_u *)0L}
			    SCTX_INIT},
    {"endoffile",   "eof",  P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_eof, PV_EOF,
			    did_سیٹ_eof_eol_fixeol_bomb, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"endofline",   "eol",  P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_eol, PV_EOL,
			    did_سیٹ_eof_eol_fixeol_bomb, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"equalalways", "ea",   P_BOOL|P_VI_DEF|P_RALL,
			    (char_u *)&p_ea, PV_NONE, did_سیٹ_equalalways, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"equalprg",    "ep",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_ep, PV_EP, تہی, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"errorbells",  "eb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_eb, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"errorfile",   "ef",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر_تعریف_شدہ FEAT_QUICKFIX
			    (char_u *)&p_ef, PV_NONE, تہی, تہی,
			    {(char_u *)DFLT_ERRORFILE, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"errorformat", "efm",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شدہ FEAT_QUICKFIX
			    (char_u *)&p_efm, PV_EFM, تہی, تہی,
			    {(char_u *)DFLT_EFM, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"esckeys",	    "ek",   P_BOOL|P_VIM,
			    (char_u *)&p_ek, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"eventignore", "ei",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_ei, PV_NONE, did_سیٹ_eventignore, expand_سیٹ_eventignore,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"eventignorewin", "eiw",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)VAR_WIN, PV_EIW, did_سیٹ_eventignore, expand_سیٹ_eventignore,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"expandtab",   "et",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_et, PV_ET, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"exrc",	    "ex",   P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_exrc, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"fileencoding","fenc", P_STRING|P_ALLOCED|P_VI_DEF|P_RSTAT|P_RBUF
								   |P_NO_MKRC,
			    (char_u *)&p_fenc, PV_FENC, did_سیٹ_encoding, expand_سیٹ_encoding,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"fileencodings","fencs", P_STRING|P_VI_DEF|P_ONECOMMA,
			    (char_u *)&p_fencs, PV_NONE, تہی, expand_سیٹ_encoding,
			    {(char_u *)"ucs-bom", (char_u *)0L}
			    SCTX_INIT},
    {"fileformat",  "ff",   P_STRING|P_ALLOCED|P_VI_DEF|P_RSTAT|P_NO_MKRC
								  |P_CURSWANT,
			    (char_u *)&p_ff, PV_FF, did_سیٹ_fileformat, expand_سیٹ_fileformat,
			    {(char_u *)DFLT_FF, (char_u *)0L} SCTX_INIT},
    {"fileformats", "ffs",  P_STRING|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_ffs, PV_NONE, did_سیٹ_fileformats, expand_سیٹ_fileformat,
			    {(char_u *)DFLT_FFS_VI, (char_u *)DFLT_FFS_VIM}
			    SCTX_INIT},
    {"fileignorecase", "fic", P_BOOL|P_VI_DEF,
			    (char_u *)&p_fic, PV_NONE, تہی, تہی,
			    {
#اگر_تعریف_شدہ CASE_INSENSITIVE_FILENAME
				    (char_u *)TRUE,
#ورنہ
				    (char_u *)FALSE,
#اگر_ختم
					(char_u *)0L} SCTX_INIT},
    {"filetype",    "ft",   P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB|P_NFNAME,
			    (char_u *)&p_ft, PV_FT,
			    did_سیٹ_filetype_or_syntax, تہی,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"fillchars",   "fcs",  P_STRING|P_VI_DEF|P_RALL|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_fcs, PV_FCS, did_سیٹ_chars_option, expand_سیٹ_chars_option,
			    {(char_u *)"vert:|,fold:-,eob:~,lastline:@",
								  (char_u *)0L}
			    SCTX_INIT},
    {"findfunc", "ffu",     P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#اگر_تعریف_شدہ FEAT_EVAL
			    (char_u *)&p_ffu, PV_FFU,
			    did_سیٹ_findfunc, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"fixendofline",  "fixeol", P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_fixeol, PV_FIXEOL,
			    did_سیٹ_eof_eol_fixeol_bomb, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"fkmap",	    "fk",   P_BOOL|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"flash",	    "fl",   P_BOOL|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"foldclose",   "fcl",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_RWIN,
#اگر_تعریف_شدہ FEAT_FOLDING
			    (char_u *)&p_fcl, PV_NONE, did_سیٹ_foldclose, expand_سیٹ_foldclose,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"foldcolumn",  "fdc",  P_عدد|P_VI_DEF|P_RWIN,
#اگر_تعریف_شدہ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDC, did_سیٹ_foldcolumn, تہی,
			    {(char_u *)FALSE, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"foldenable",  "fen",  P_BOOL|P_VI_DEF|P_RWIN,
#اگر_تعریف_شدہ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FEN, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"foldexpr",    "fde",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN|P_MLE,
#اگر defined(FEAT_FOLDING) && defined(FEAT_EVAL)
			    (char_u *)VAR_WIN, PV_FDE, did_سیٹ_foldexpr, تہی,
			    {(char_u *)"0", (char_u *)تہی}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"foldignore",  "fdi",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN,
#اگر_تعریف_شدہ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDI, did_سیٹ_foldignore, تہی,
			    {(char_u *)"#", (char_u *)تہی}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"foldlevel",   "fdl",  P_عدد|P_VI_DEF|P_RWIN,
#اگر_تعریف_شدہ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDL, did_سیٹ_foldlevel, تہی,
			    {(char_u *)0L, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"foldlevelstart","fdls", P_عدد|P_VI_DEF|P_CURSWANT,
#اگر_تعریف_شدہ FEAT_FOLDING
			    (char_u *)&p_fdls, PV_NONE, تہی, تہی,
			    {(char_u *)-1L, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"foldmarker",  "fmr",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|
						    P_RWIN|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شدہ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FMR, did_سیٹ_foldmarker, تہی,
			    {(char_u *)"{{{,}}}", (char_u *)تہی}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"foldmethod",  "fdm",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN,
#اگر_تعریف_شدہ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDM, did_سیٹ_foldmethod, expand_سیٹ_foldmethod,
			    {(char_u *)"manual", (char_u *)تہی}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"foldminlines","fml",  P_عدد|P_VI_DEF|P_RWIN,
#اگر_تعریف_شدہ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FML, did_سیٹ_foldminlines, تہی,
			    {(char_u *)1L, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"foldnestmax", "fdn",  P_عدد|P_VI_DEF|P_RWIN,
#اگر_تعریف_شدہ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDN, did_سیٹ_foldnestmax, تہی,
			    {(char_u *)20L, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"foldopen",    "fdo",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_CURSWANT,
#اگر_تعریف_شدہ FEAT_FOLDING
			    (char_u *)&p_fdo, PV_NONE, did_سیٹ_foldopen, expand_سیٹ_foldopen,
		 {(char_u *)"block,hor,mark,percent,quickfix,search,tag,undo",
						 (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"foldtext",    "fdt",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN|P_MLE,
#اگر defined(FEAT_FOLDING) && defined(FEAT_EVAL)
			    (char_u *)VAR_WIN, PV_FDT, did_سیٹ_optexpr, تہی,
			    {(char_u *)"foldtext()", (char_u *)تہی}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"formatexpr", "fex",   P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_MLE,
#اگر_تعریف_شدہ FEAT_EVAL
			    (char_u *)&p_fex, PV_FEX, did_سیٹ_optexpr, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"formatlistpat","flp", P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)&p_flp, PV_FLP, تہی, تہی,
			    {(char_u *)"^\\s*\\d\\+[\\]:.)}\\t ]\\s*",
						 (char_u *)0L} SCTX_INIT},
    {"formatoptions","fo",  P_STRING|P_ALLOCED|P_VIM|P_FLAGLIST,
			    (char_u *)&p_fo, PV_FO, did_سیٹ_formatoptions, expand_سیٹ_formatoptions,
			    {(char_u *)DFLT_FO_VI, (char_u *)DFLT_FO_VIM}
			    SCTX_INIT},
    {"formatprg",   "fp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_fp, PV_FP, تہی, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"fsync",       "fs",   P_BOOL|P_SECURE|P_VI_DEF,
#اگر_تعریف_شدہ HAVE_FSYNC
			    (char_u *)&p_fs, PV_FS, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"gdefault",    "gd",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_gd, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"graphic",	    "gr",   P_BOOL|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"grepformat",  "gfm",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شدہ FEAT_QUICKFIX
			    (char_u *)&p_gefm, PV_GEFM, تہی, تہی,
			    {(char_u *)DFLT_GREPFORMAT, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"grepprg",	    "gp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر_تعریف_شدہ FEAT_QUICKFIX
			    (char_u *)&p_gp, PV_GP, تہی, تہی,
			    {
# اگر defined(MSWIN)
			    // may be changed to "grep -n" in os_win32.c
			    (char_u *)"findstr /n",
# elif defined(UNIX)
			    // Add an extra file name so that grep will always
			    // insert a file name in the match line.
			    (char_u *)"grep -n $* /dev/null",
# elif defined(VMS)
			    (char_u *)"SEARCH/NUMBERS ",
# ورنہ
			    (char_u *)"grep -n ",
# endif
			    (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"guicursor",    "gcr", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شدہ CURSOR_SHAPE
			    (char_u *)&p_guicursor, PV_NONE,
			    did_سیٹ_guicursor, تہی,
			    {
# ifdef FEAT_GUI
				(char_u *)"n-v-c:block-Cursor/lCursor,ve:ver35-Cursor,o:hor50-Cursor,i-ci:ver25-Cursor/lCursor,r-cr:hor20-Cursor/lCursor,sm:block-Cursor-blinkwait175-blinkoff150-blinkon175",
# ورنہ	// Win32 console
				(char_u *)"n-v-c:block,o:hor50,i-ci:hor15,r-cr:hor30,sm:block",
# endif
				    (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"guifont",	    "gfn",  P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP
#اگر !defined(FEAT_GUI_GTK)
				|P_COLON
#اگر_ختم
				,
#اگر_تعریف_شدہ FEAT_GUI
			    (char_u *)&p_guifont, PV_NONE, did_سیٹ_guifont, expand_سیٹ_guifont,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"guifontset",  "gfs",  P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA,
#اگر defined(FEAT_GUI) && defined(FEAT_XFONTSET)
			    (char_u *)&p_guifontset, PV_NONE,
			    did_سیٹ_guifontset, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"guifontwide", "gfw",  P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP
#اگر !defined(FEAT_GUI_GTK)
				|P_COLON
#اگر_ختم
				,
#اگر defined(FEAT_GUI)
			    (char_u *)&p_guifontwide, PV_NONE,
			    did_سیٹ_guifontwide, expand_سیٹ_guifont,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},


    {"guiheadroom", "ghr",  P_عدد|P_VI_DEF,
#اگر (defined(FEAT_GUI_GTK) && !defined(USE_GTK4)) || defined(FEAT_GUI_X11)
			    (char_u *)&p_ghr, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)50L, (char_u *)0L} SCTX_INIT},

    {"guiligatures", "gli", P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP,
#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
			    (char_u *)&p_guiligatures, PV_NONE,
			    did_سیٹ_guiligatures, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},

    {"guioptions",  "go",   P_STRING|P_VI_DEF|P_RALL|P_FLAGLIST,
#اگر defined(FEAT_GUI)
			    (char_u *)&p_go, PV_NONE, did_سیٹ_guioptions, expand_سیٹ_guioptions,
# اگر defined(UNIX)
			    {(char_u *)"aegimrLtT", (char_u *)0L}
# ورنہ
			    {(char_u *)"egmrLtT", (char_u *)0L}
# endif
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"guipty",	    تہی,   P_BOOL|P_VI_DEF,
#اگر defined(FEAT_GUI)
			    (char_u *)&p_guipty, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"guitablabel",  "gtl", P_STRING|P_VI_DEF|P_RWIN|P_MLE,
#اگر defined(FEAT_GUI_TABLINE)
			    (char_u *)&p_gtl, PV_NONE, did_سیٹ_guitablabel, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"guitabtooltip",  "gtt", P_STRING|P_VI_DEF|P_RWIN|P_MLE,
#اگر defined(FEAT_GUI_TABLINE)
			    (char_u *)&p_gtt, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"hardtabs",    "ht",   P_عدد|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"helpfile",    "hf",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_hf, PV_NONE, did_سیٹ_helpfile, تہی,
			    {(char_u *)DFLT_HELPFILE, (char_u *)0L}
			    SCTX_INIT},
    {"helpheight",  "hh",   P_عدد|P_VI_DEF,
			    (char_u *)&p_hh, PV_NONE,
			    did_سیٹ_winheight_helpheight, تہی,
			    {(char_u *)20L, (char_u *)0L} SCTX_INIT},
    {"helplang",    "hlg",  P_STRING|P_VI_DEF|P_ONECOMMA,
#اگر_تعریف_شدہ FEAT_MULTI_LANG
			    (char_u *)&p_hlg, PV_NONE, did_سیٹ_helplang, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"hidden",	    "hid",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_hid, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"highlight",   "hl",   P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_hl, PV_NONE, did_سیٹ_highlight, expand_سیٹ_highlight,
			    {(char_u *)HIGHLIGHT_INIT, (char_u *)0L}
			    SCTX_INIT},
    {"history",	    "hi",   P_عدد|P_VIM,
			    (char_u *)&p_hi, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)200L} SCTX_INIT},
    {"hkmap",	    "hk",   P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
			    (char_u *)&p_hkmap, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"hkmapp",	    "hkp",  P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
			    (char_u *)&p_hkmapp, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"hlsearch",    "hls",  P_BOOL|P_VI_DEF|P_VIM|P_RALL|P_HLONLY,
#اگر defined(FEAT_تلاش_EXTRA)
			    (char_u *)&p_hls, PV_NONE, did_سیٹ_hlsearch, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"icon",	    تہی,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_icon, PV_NONE, did_سیٹ_title_icon, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"iconstring",  تہی,   P_STRING|P_VI_DEF|P_MLE,
			    (char_u *)&p_iconstring, PV_NONE,
			    did_سیٹ_iconstring, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"ignorecase",  "ic",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ic, PV_NONE, did_سیٹ_ignorecase, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"imactivatefunc","imaf",P_STRING|P_VI_DEF|P_SECURE|P_FUNC,
#اگر defined(FEAT_EVAL) && \
	(defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
			    (char_u *)&p_imaf, PV_NONE,
			    did_سیٹ_imactivatefunc, تہی,
			    {(char_u *)"", (char_u *)تہی}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"imactivatekey","imak",P_STRING|P_VI_DEF,
#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
			    (char_u *)&p_imak, PV_NONE,
			    did_سیٹ_imactivatekey, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"imageprotocol", "ipc",P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_COLON,
#اگر_تعریف_شدہ FEAT_IMAGE
			    (char_u *)&p_ipc, PV_NONE, did_سیٹ_imageprotocol, expand_سیٹ_imageprotocol,
			    {(char_u *)"kitty:kitty,foot:sixel,ghostty:kitty,wezterm:kitty,xterm:sixel", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"imcmdline",   "imc",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_imcmdline, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"imdisable",   "imd",  P_BOOL|P_VI_DEF,
#اگر defined(HAVE_داخلہ_طریقہ)
			    (char_u *)&p_imdisable, PV_NONE,
			    did_سیٹ_imdisable, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L}
			    SCTX_INIT},
    {"iminsert",    "imi",  P_عدد|P_VI_DEF,
			    (char_u *)&p_iminsert, PV_IMI, did_سیٹ_iminsert, تہی,
			    {(char_u *)B_IMODE_NONE, (char_u *)0L}
			    SCTX_INIT},
    {"imsearch",    "ims",  P_عدد|P_VI_DEF,
			    (char_u *)&p_imsearch, PV_IMS, did_سیٹ_imsearch, تہی,
			    {(char_u *)B_IMODE_USE_داخل_کرو, (char_u *)0L}
			    SCTX_INIT},
    {"imstatusfunc","imsf",P_STRING|P_VI_DEF|P_SECURE|P_FUNC,
#اگر defined(FEAT_EVAL) && \
	(defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
			    (char_u *)&p_imsf, PV_NONE, did_سیٹ_imstatusfunc, تہی,
			    {(char_u *)"", (char_u *)تہی}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"imstyle",	    "imst", P_عدد|P_VI_DEF|P_SECURE,
#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
			    (char_u *)&p_imst, PV_NONE, did_سیٹ_imstyle, تہی,
			    {(char_u *)IM_OVER_THE_SPOT, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"include",	    "inc",  P_STRING|P_ALLOCED|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
			    (char_u *)&p_inc, PV_INC, تہی, تہی,
			    {(char_u *)"^\\s*#\\s*include", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"includeexpr", "inex", P_STRING|P_ALLOCED|P_VI_DEF|P_MLE,
#اگر defined(FEAT_ڈھونڈو_ID) && defined(FEAT_EVAL)
			    (char_u *)&p_inex, PV_INEX, did_سیٹ_optexpr, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"incsearch",   "is",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_is, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"indentexpr", "inde",  P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_MLE,
#اگر defined(FEAT_EVAL)
			    (char_u *)&p_inde, PV_INDE, did_سیٹ_optexpr, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"indentkeys", "indk",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر defined(FEAT_EVAL)
			    (char_u *)&p_indk, PV_INDK, تہی, تہی,
			    {INDENTKEYS_DEFAULT, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"infercase",   "inf",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_inf, PV_INF, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"insertmode",  "im",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_im, PV_NONE, did_سیٹ_insertmode, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"isfname",	    "isf",  P_STRING|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_isf, PV_NONE, did_سیٹ_isopt, تہی,
			    {
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
				// Excluded are: & and ^ are special in cmd.exe
				// ( and ) are used in text separating fnames
			    (char_u *)"@,48-57,/,\\,.,-,_,+,,,#,$,%,{,},[,],:,@-@,!,~,=",
#ورنہ
# اگر defined(AMIGA)
			    (char_u *)"@,48-57,/,.,-,_,+,,,$,:",
# elif defined(VMS)
			    (char_u *)"@,48-57,/,.,-,_,+,,,#,$,%,<,>,[,],:,;,~",
# ورنہ // UNIX et al.
			    (char_u *)"@,48-57,/,.,-,_,+,,,#,$,%,~,=",
# endif
#اگر_ختم
				(char_u *)0L} SCTX_INIT},
    {"isident",	    "isi",  P_STRING|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_isi, PV_NONE, did_سیٹ_isopt, تہی,
			    {
#اگر defined(MSWIN)
			    (char_u *)"@,48-57,_,128-167,224-235",
#ورنہ
			    (char_u *)"@,48-57,_,192-255",
#اگر_ختم
				(char_u *)0L} SCTX_INIT},
    {"iskeyword",   "isk",  P_STRING|P_ALLOCED|P_VIM|P_COMMA|P_NODUP,
			    (char_u *)&p_isk, PV_ISK, did_سیٹ_iskeyword, تہی,
			    {
				(char_u *)"@,48-57,_",
#اگر defined(MSWIN)
				(char_u *)"@,48-57,_,128-167,224-235"
#ورنہ
				ISK_LATIN1
#اگر_ختم
			    } SCTX_INIT},
    {"isprint",	    "isp",  P_STRING|P_VI_DEF|P_RALL|P_COMMA|P_NODUP,
			    (char_u *)&p_isp, PV_NONE, did_سیٹ_isopt, تہی,
			    {
#اگر defined(MSWIN) || defined(VMS)
			    (char_u *)"@,~-255",
#ورنہ
			    ISP_LATIN1,
#اگر_ختم
				(char_u *)0L} SCTX_INIT},
    {"joinspaces",  "js",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_js, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"jumpoptions", "jop",  P_STRING|P_VI_DEF|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_jop, PV_NONE, did_سیٹ_jumpoptions, expand_سیٹ_jumpoptions,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"key",	    تہی,   P_STRING|P_ALLOCED|P_VI_DEF|P_NO_MKRC|P_NO_CMD_EXPAND,
#اگر_تعریف_شدہ FEAT_CRYPT
			    (char_u *)&p_کلید, PV_کلید, did_سیٹ_cryptkey, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"keymap",	    "kmp",  P_STRING|P_ALLOCED|P_VI_DEF|P_RBUF|P_RSTAT|P_NFNAME|P_PRI_MKRC,
#اگر_تعریف_شدہ FEAT_KEYMAP
			    (char_u *)&p_keymap, PV_KMAP, did_سیٹ_keymap, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"keymodel",    "km",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_km, PV_NONE, did_سیٹ_keymodel, expand_سیٹ_keymodel,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"keyprotocol", "kpc",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_kpc, PV_NONE, did_سیٹ_keyprotocol, expand_سیٹ_keyprotocol,
			    {(char_u *)"kitty:kitty,foot:kitty,ghostty:kitty,wezterm:kitty,xterm:mok2", (char_u *)0L}
			    SCTX_INIT},
    {"keywordprg",  "kp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_kp, PV_KP, تہی, تہی,
			    {
#اگر defined(MSWIN)
			    (char_u *)":help",
#وگرنہ_اگر defined(VMS)
			    (char_u *)"help",
#وگرنہ_اگر defined(USEMAN_S)
			    (char_u *)"man -s",
#ورنہ
			    (char_u *)"man",
#اگر_ختم
				(char_u *)0L} SCTX_INIT},
    {"langmap",     "lmap", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_SECURE,
#اگر_تعریف_شدہ FEAT_LANGMAP
			    (char_u *)&p_langmap, PV_NONE, did_سیٹ_langmap, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"langmenu",    "lm",   P_STRING|P_VI_DEF|P_NFNAME,
#اگر defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
			    (char_u *)&p_lm, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"langnoremap",  "lnr",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_LANGMAP
			    (char_u *)&p_lnr, PV_NONE, did_سیٹ_langnoremap, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"langremap",  "lrm",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_LANGMAP
			    (char_u *)&p_lrm, PV_NONE, did_سیٹ_langremap, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"laststatus",  "ls",   P_عدد|P_VI_DEF|P_RALL,
			    (char_u *)&p_ls, PV_NONE, did_سیٹ_laststatus, تہی,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"lazyredraw",  "lz",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_lz, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"lhistory", "lhi",	    P_عدد|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_QUICKFIX
			    (char_u *)VAR_WIN, PV_LHI, did_سیٹ_xhistory, تہی,
			    {(char_u *)10L, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"linebreak",   "lbr",  P_BOOL|P_VI_DEF|P_RWIN,
#اگر_تعریف_شدہ FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_LBR, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"lines",	    تہی,   P_عدد|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RCLR,
			    (char_u *)&صفیں, PV_NONE, تہی, تہی,
			    {
#اگر defined(MSWIN)
			    (char_u *)25L,
#ورنہ
			    (char_u *)24L,
#اگر_ختم
					    (char_u *)0L} SCTX_INIT},
    {"linespace",   "lsp",  P_عدد|P_VI_DEF|P_RCLR,
#اگر_تعریف_شدہ FEAT_GUI
			    (char_u *)&p_linespace, PV_NONE,
			    did_سیٹ_linespace, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
			    {(char_u *)1L, (char_u *)0L}
#ورنہ
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"lisp",	    تہی,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_lisp, PV_LISP, did_سیٹ_lisp, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"lispoptions", "lop",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_lop, PV_LOP, did_سیٹ_lispoptions, expand_سیٹ_lispoptions,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"lispwords",   "lw",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_lispwords, PV_LW, تہی, تہی,
			    {(char_u *)LISPWORD_قدر, (char_u *)0L} SCTX_INIT},
    {"list",	    تہی,   P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_LIST, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"listchars",   "lcs",  P_STRING|P_VI_DEF|P_RALL|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_lcs, PV_LCS, did_سیٹ_chars_option, expand_سیٹ_chars_option,
			    {(char_u *)"eol:$", (char_u *)0L} SCTX_INIT},
    {"loadplugins", "lpl",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_lpl, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"luadll",      تہی,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر defined(DYNAMIC_LUA)
			    (char_u *)&p_luadll, PV_NONE, تہی, تہی,
			    {(char_u *)DYNAMIC_LUA_DLL, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"macatsui",    تہی,   P_BOOL|P_VI_DEF|P_RCLR,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"magic",	    تہی,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_magic, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"makeef",	    "mef",  P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر_تعریف_شدہ FEAT_QUICKFIX
			    (char_u *)&p_mef, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"makeencoding","menc", P_STRING|P_VI_DEF,
			    (char_u *)&p_menc, PV_MENC, did_سیٹ_encoding, expand_سیٹ_encoding,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"makeprg",	    "mp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر_تعریف_شدہ FEAT_QUICKFIX
			    (char_u *)&p_mp, PV_MP, تہی, تہی,
# ifdef VMS
			    {(char_u *)"MMS", (char_u *)0L}
# ورنہ
			    {(char_u *)"make", (char_u *)0L}
# endif
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"matchpairs",  "mps",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_mps, PV_MPS, did_سیٹ_matchpairs, تہی,
			    {(char_u *)"(:),{:},[:]", (char_u *)0L}
			    SCTX_INIT},
    {"matchtime",   "mat",  P_عدد|P_VI_DEF,
			    (char_u *)&p_mat, PV_NONE, تہی, تہی,
			    {(char_u *)5L, (char_u *)0L} SCTX_INIT},
    {"maxcombine",  "mco",  P_عدد|P_VI_DEF|P_CURSWANT,
			    (char_u *)&p_mco, PV_NONE, did_سیٹ_maxcombine, تہی,
			    {(char_u *)2, (char_u *)0L} SCTX_INIT},
    {"maxfuncdepth", "mfd", P_عدد|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_EVAL
			    (char_u *)&p_mfd, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)100L, (char_u *)0L} SCTX_INIT},
    {"maxmapdepth", "mmd",  P_عدد|P_VI_DEF,
			    (char_u *)&p_mmd, PV_NONE, تہی, تہی,
			    {(char_u *)1000L, (char_u *)0L} SCTX_INIT},
    {"maxmem",	    "mm",   P_عدد|P_VI_DEF,
			    (char_u *)&p_mm, PV_NONE, تہی, تہی,
			    {(char_u *)DFLT_MAXMEM, (char_u *)0L}
			    SCTX_INIT},
    {"maxmempattern","mmp", P_عدد|P_VI_DEF,
			    (char_u *)&p_mmp, PV_NONE, تہی, تہی,
			    {(char_u *)1000L, (char_u *)0L} SCTX_INIT},
    {"maxmemtot",   "mmt",  P_عدد|P_VI_DEF,
			    (char_u *)&p_mmt, PV_NONE, تہی, تہی,
			    {(char_u *)DFLT_MAXMEMTOT, (char_u *)0L}
			    SCTX_INIT},
    {"maxsearchcount", "msc", P_عدد|P_VI_DEF,
			    (char_u *)&p_msc, PV_NONE, did_سیٹ_maxsearchcount, تہی,
			    {(char_u *)99L, (char_u *)0L} SCTX_INIT},
    {"menuitems",   "mis",  P_عدد|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_MENU
			    (char_u *)&p_mis, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)25L, (char_u *)0L} SCTX_INIT},
    {"mesg",	    تہی,   P_BOOL|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"messagesopt","mopt",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_COLON|P_NODUP,
			    (char_u *)&p_mopt, PV_NONE, did_سیٹ_messagesopt, expand_سیٹ_messagesopt,
			    {(char_u *)"hit-enter,history:500", (char_u *)تہی} SCTX_INIT},
    {"mkspellmem",  "msm",  P_STRING|P_VI_DEF|P_EXPAND|P_SECURE,
#اگر_تعریف_شدہ FEAT_SPELL
			    (char_u *)&p_msm, PV_NONE, did_سیٹ_mkspellmem, تہی,
			    {(char_u *)"460000,2000,500", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"modeline",    "ml",   P_BOOL|P_VIM,
			    (char_u *)&p_ml, PV_ML, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"modelineexpr", "mle",  P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_mle, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"modelines",   "mls",  P_عدد|P_VI_DEF,
			    (char_u *)&p_mls, PV_NONE, تہی, تہی,
			    {(char_u *)5L, (char_u *)0L} SCTX_INIT},
    {"modelinestrict", "mlst", P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_mlstr, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"modifiable",  "ma",   P_BOOL|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_ma, PV_MA, did_سیٹ_modifiable, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"modified",    "mod",  P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_mod, PV_MOD, did_سیٹ_modified, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"more",	    تہی,   P_BOOL|P_VIM,
			    (char_u *)&p_مزید, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"mouse",	    تہی,   P_STRING|P_VI_DEF|P_FLAGLIST,
			    (char_u *)&p_mouse, PV_NONE, did_سیٹ_mouse, expand_سیٹ_mouse,
			    {
#اگر defined(MSWIN)
				(char_u *)"a",
#ورنہ
				(char_u *)"",
#اگر_ختم
				(char_u *)0L} SCTX_INIT},
    {"mousefocus",   "mousef", P_BOOL|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_GUI
			    (char_u *)&p_mousef, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"mousehide",   "mh",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_GUI
			    (char_u *)&p_mh, PV_NONE, did_سیٹ_mousehide, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"mousemodel",  "mousem", P_STRING|P_VI_DEF,
			    (char_u *)&p_mousem, PV_NONE, did_سیٹ_mousemodel, expand_سیٹ_mousemodel,
			    {
#اگر defined(MSWIN)
				(char_u *)"popup",
#ورنہ
# اگر defined(MACOS_X)
				(char_u *)"popup_setpos",
# ورنہ
				(char_u *)"extend",
# endif
#اگر_ختم
				(char_u *)0L} SCTX_INIT},
    {"mousemoveevent",   "mousemev",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_GUI
			    (char_u *)&p_mousemev, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"mouseshape",  "mouses",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شدہ FEAT_MOUSESHAPE
			    (char_u *)&p_mouseshape, PV_NONE,
			    did_سیٹ_mouseshape, تہی,
			    {(char_u *)"i-r:beam,s:updown,sd:udsizing,vs:leftright,vd:lrsizing,m:no,ml:up-arrow,v:rightup-arrow", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"mousetime",   "mouset",	P_عدد|P_VI_DEF,
			    (char_u *)&p_mouset, PV_NONE, تہی, تہی,
			    {(char_u *)500L, (char_u *)0L} SCTX_INIT},
    {"mzquantum",  "mzq",   P_عدد,
#اگر_تعریف_شدہ FEAT_MZSCHEME
			    (char_u *)&p_mzq, PV_NONE, did_سیٹ_mzquantum, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)100L, (char_u *)100L} SCTX_INIT},
    {"mzschemedll", تہی,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر defined(DYNAMIC_MZSCHEME)
			    (char_u *)&p_mzschemedll, PV_NONE, تہی, تہی,
			    {(char_u *)DYNAMIC_MZSCH_DLL, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"mzschemegcdll", تہی, P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر defined(DYNAMIC_MZSCHEME)
			    (char_u *)&p_mzschemegcdll, PV_NONE, تہی, تہی,
			    {(char_u *)DYNAMIC_MZGC_DLL, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"novice",	    تہی,   P_BOOL|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"nrformats",   "nf",   P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_nf, PV_NF, did_سیٹ_nrformats, expand_سیٹ_nrformats,
			    {(char_u *)"bin,octal,hex", (char_u *)0L}
			    SCTX_INIT},
    {"number",	    "nu",   P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_NU,
			    did_سیٹ_نمبر_relativenumber, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"numberwidth", "nuw",  P_عدد|P_RWIN|P_VIM,
#اگر_تعریف_شدہ FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_NUW, did_سیٹ_numberwidth, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)8L, (char_u *)4L} SCTX_INIT},
    {"omnifunc",    "ofu",  P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
			    (char_u *)&p_ofu, PV_OFU, did_سیٹ_omnifunc, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"open",	    تہی,   P_BOOL|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"opendevice",  "odev", P_BOOL|P_VI_DEF,
#اگر defined(MSWIN)
			    (char_u *)&p_odev, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)FALSE}
			    SCTX_INIT},
    {"operatorfunc", "opfunc", P_STRING|P_VI_DEF|P_SECURE|P_FUNC,
			    (char_u *)&p_opfunc, PV_NONE,
			    did_سیٹ_operatorfunc, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"optimize",    "opt",  P_BOOL|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"osctimeoutlen", "ost", P_عدد|P_VI_DEF,
			    (char_u *)&p_ost, PV_NONE, did_سیٹ_osctimeoutlen, تہی,
			    {(char_u *)1000, (char_u *)0L} SCTX_INIT},
    {"osfiletype",  "oft",  P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"packpath",    "pp",   P_STRING|P_VI_DEF|P_EXPAND|P_ONECOMMA|P_NODUP
								    |P_SECURE,
			    (char_u *)&p_pp, PV_NONE, تہی, تہی,
			    {(char_u *)DFLT_RUNTIMEPATH, (char_u *)0L}
			    SCTX_INIT},
    {"paragraphs",  "para", P_STRING|P_VI_DEF,
			    (char_u *)&p_para, PV_NONE, تہی, تہی,
			    {(char_u *)"IPLPPPQPP TPHPLIPpLpItpplpipbp",
				(char_u *)0L} SCTX_INIT},
    {"paste",	    تہی,   P_BOOL|P_VI_DEF|P_PRI_MKRC,
			    (char_u *)&p_paste, PV_NONE, did_سیٹ_paste, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"pastetoggle", "pt",   P_STRING|P_VI_DEF,
			    (char_u *)&p_pt, PV_NONE, did_سیٹ_pastetoggle, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"patchexpr",   "pex",  P_STRING|P_VI_DEF|P_SECURE,
#اگر defined(FEAT_DIFF) && defined(FEAT_EVAL)
			    (char_u *)&p_pex, PV_NONE, did_سیٹ_optexpr, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"patchmode",   "pm",   P_STRING|P_VI_DEF|P_NFNAME,
			    (char_u *)&p_pm, PV_NONE,
			    did_سیٹ_backupext_or_patchmode, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"path",	    "pa",   P_STRING|P_EXPAND|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_راستہ, PV_راستہ, تہی, تہی,
			    {
#اگر defined(AMIGA) || defined(MSWIN)
			    (char_u *)".,,",
#ورنہ
			    (char_u *)".,/usr/include,,",
#اگر_ختم
				(char_u *)0L} SCTX_INIT},
    {"perldll",     تہی,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر defined(DYNAMIC_PERL)
			    (char_u *)&p_perldll, PV_NONE, تہی, تہی,
			    {(char_u *)DYNAMIC_PERL_DLL, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"preserveindent", "pi", P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_pi, PV_PI, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"previewheight", "pvh", P_عدد|P_VI_DEF,
#اگر defined(FEAT_QUICKFIX)
			    (char_u *)&p_pvh, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)12L, (char_u *)0L} SCTX_INIT},
    {"previewpopup", "pvp", P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON,
#اگر_تعریف_شدہ FEAT_PROP_POPUP
			    (char_u *)&p_pvp, PV_NONE, did_سیٹ_previewpopup, expand_سیٹ_previewpopup,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"previewwindow", "pvw", P_BOOL|P_VI_DEF|P_RSTAT|P_NOGLOB,
#اگر defined(FEAT_QUICKFIX)
			    (char_u *)VAR_WIN, PV_PVW, did_سیٹ_previewwindow, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"printdevice", "pdev", P_STRING|P_VI_DEF|P_SECURE,
#اگر_تعریف_شدہ FEAT_PRINTER
			    (char_u *)&p_pdev, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"printencoding", "penc", P_STRING|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_POSTSCRIPT
			    (char_u *)&p_penc, PV_NONE,
			    did_سیٹ_printencoding, expand_سیٹ_encoding,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"printexpr", "pexpr",  P_STRING|P_VI_DEF|P_SECURE,
#اگر defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
			    (char_u *)&p_pexpr, PV_NONE, did_سیٹ_optexpr, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"printfont", "pfn",    P_STRING|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_PRINTER
			    (char_u *)&p_pfn, PV_NONE, تہی, تہی,
			    {
# ifdef MSWIN
				(char_u *)"Courier_New:h10",
# elif defined(FEAT_PRINT_PANGO)
				(char_u *)"Courier 10",
# ورنہ
				(char_u *)"courier",
# endif
				(char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"printheader", "pheader",  P_STRING|P_VI_DEF|P_GETTEXT|P_MLE,
#اگر_تعریف_شدہ FEAT_PRINTER
			    (char_u *)&p_سرخی, PV_NONE, تہی, تہی,
			    // untranslated to avoid problems when 'encoding'
			    // is changed
			    {(char_u *)"%<%f%h%m%=Page %N", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"printmbcharset", "pmbcs",  P_STRING|P_VI_DEF,
#اگر defined(FEAT_POSTSCRIPT)
			    (char_u *)&p_pmcs, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"printmbfont", "pmbfn",  P_STRING|P_VI_DEF,
#اگر defined(FEAT_POSTSCRIPT)
			    (char_u *)&p_pmfn, PV_NONE, parse_printmbfont, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"printoptions", "popt", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شدہ FEAT_PRINTER
			    (char_u *)&p_popt, PV_NONE, parse_printoptions, expand_سیٹ_printoptions,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"prompt",	    تہی,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_prompt, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"pumborder",   "pb",   P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON|P_SECURE,
			    (char_u *)&p_pb, PV_NONE, did_سیٹ_pumborder, expand_سیٹ_pumborder,
			    {(char_u *)"", (char_u *)تہی} SCTX_INIT},
    {"pumheight",   "ph",   P_عدد|P_VI_DEF,
			    (char_u *)&p_ph, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"pummaxwidth", "pmw",   P_عدد|P_VI_DEF,
			    (char_u *)&p_pmw, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"pumopt",	    تہی,   P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON,
			    (char_u *)&p_pumopt, PV_NONE,
			    did_سیٹ_pumopt, expand_سیٹ_pumopt,
			    {(char_u *)"", (char_u *)تہی} SCTX_INIT},
    {"pumwidth",    "pw",   P_عدد|P_VI_DEF,
			    (char_u *)&p_pw, PV_NONE, تہی, تہی,
			    {(char_u *)15L, (char_u *)15L} SCTX_INIT},
    {"pythondll",   تہی,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر defined(DYNAMIC_PYTHON)
			    (char_u *)&p_pydll, PV_NONE, تہی, تہی,
			    {(char_u *)DYNAMIC_PYTHON_DLL, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"pythonhome",  تہی,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر defined(FEAT_PYTHON)
			    (char_u *)&p_pyhome, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"pythonthreedll",  تہی,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر defined(DYNAMIC_PYTHON3)
			    (char_u *)&p_py3dll, PV_NONE, تہی, تہی,
			    {(char_u *)DYNAMIC_PYTHON3_DLL, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"pythonthreehome", تہی,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر defined(FEAT_PYTHON3)
			    (char_u *)&p_py3home, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"pyxversion", "pyx",   P_عدد|P_VI_DEF|P_SECURE,
#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
			    (char_u *)&p_pyx, PV_NONE, did_سیٹ_pyxversion, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)DEFAULT_PYTHON_VER, (char_u *)0L}
			    SCTX_INIT},
    {"quickfixtextfunc", "qftf", P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_SECURE|P_FUNC,
#اگر defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
			    (char_u *)&p_qftf, PV_NONE,
			    did_سیٹ_quickfixtextfunc, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)تہی}
#اگر_ختم
			    SCTX_INIT},
    {"quoteescape", "qe",   P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)&p_qe, PV_QE, تہی, تہی,
			    {(char_u *)"\\", (char_u *)0L} SCTX_INIT},
    {"readonly",    "ro",   P_BOOL|P_VI_DEF|P_RSTAT|P_NOGLOB,
			    (char_u *)&p_ro, PV_RO, did_سیٹ_readonly, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"redraw",	    تہی,   P_BOOL|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"redrawtime",  "rdt",  P_عدد|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_RELTIME
			    (char_u *)&p_rdt, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)2000L, (char_u *)0L} SCTX_INIT},
    {"regexpengine", "re",  P_عدد|P_VI_DEF,
			    (char_u *)&p_re, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"relativenumber", "rnu", P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_RNU,
			    did_سیٹ_نمبر_relativenumber, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"remap",	    تہی,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_remap, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"renderoptions", "rop", P_STRING|P_ONECOMMA|P_RCLR|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_RENDER_OPTIONS
			    (char_u *)&p_rop, PV_NONE, did_سیٹ_renderoptions, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"report",	    تہی,   P_عدد|P_VI_DEF,
			    (char_u *)&p_report, PV_NONE, تہی, تہی,
			    {(char_u *)2L, (char_u *)0L} SCTX_INIT},
    {"restorescreen", "rs", P_BOOL|P_VI_DEF,
#اگر_تعریف_شدہ MSWIN
			    (char_u *)&p_rs, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"revins",	    "ri",   P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
			    (char_u *)&p_ri, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"rightleft",   "rl",   P_BOOL|P_VI_DEF|P_RWIN,
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
			    (char_u *)VAR_WIN, PV_RL, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"rightleftcmd", "rlc", P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN,
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
			    (char_u *)VAR_WIN, PV_RLC, did_سیٹ_rightleftcmd, expand_سیٹ_rightleftcmd,
			    {(char_u *)"search", (char_u *)تہی}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"rubydll",     تہی,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر defined(DYNAMIC_RUBY)
			    (char_u *)&p_rubydll, PV_NONE, تہی, تہی,
			    {(char_u *)DYNAMIC_RUBY_DLL, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"ruler",	    "ru",   P_BOOL|P_VIM|P_RSTAT,
			    (char_u *)&p_ru, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"rulerformat", "ruf",  P_STRING|P_VI_DEF|P_ALLOCED|P_RSTAT|P_MLE,
#اگر_تعریف_شدہ FEAT_STL_OPT
			    (char_u *)&p_ruf, PV_NONE, did_سیٹ_rulerformat, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"runtimepath", "rtp",  P_STRING|P_VI_DEF|P_EXPAND|P_ONECOMMA|P_NODUP
								    |P_SECURE,
			    (char_u *)&p_rtp, PV_NONE, تہی, تہی,
			    {(char_u *)DFLT_RUNTIMEPATH, (char_u *)0L}
			    SCTX_INIT},
    {"scroll",	    "scr",  P_عدد|P_NO_MKRC|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_SCROLL, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"scrollbind",  "scb",  P_BOOL|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_SCBIND, did_سیٹ_scrollbind, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"scrollfocus", "scf",  P_BOOL|P_VI_DEF,
#اگر defined(MSWIN) && defined(FEAT_GUI)
			    (char_u *)&p_scf, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"scrolljump",  "sj",   P_عدد|P_VI_DEF|P_VIM,
			    (char_u *)&p_sj, PV_NONE, تہی, تہی,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"scrolloff",   "so",   P_عدد|P_VI_DEF|P_VIM|P_RALL,
			    (char_u *)&p_so, PV_SO, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"scrolloffpad", "sop", P_عدد|P_VI_DEF|P_VIM|P_RALL,
				(char_u *)&p_sop, PV_SOP, تہی, تہی,
				{(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"scrollopt",   "sbo",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_sbo, PV_NONE, did_سیٹ_scrollopt, expand_سیٹ_scrollopt,
			    {(char_u *)"ver,jump", (char_u *)0L}
			    SCTX_INIT},
    {"sections",    "sect", P_STRING|P_VI_DEF,
			    (char_u *)&p_sections, PV_NONE, تہی, تہی,
			    {(char_u *)"SHNHH HUnhsh", (char_u *)0L}
			    SCTX_INIT},
    {"secure",	    تہی,   P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_secure, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"selection",   "sel",  P_STRING|P_VI_DEF,
			    (char_u *)&p_sel, PV_NONE, did_سیٹ_selection, expand_سیٹ_selection,
			    {(char_u *)"inclusive", (char_u *)0L}
			    SCTX_INIT},
    {"selectmode",  "slm",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_slm, PV_NONE, did_سیٹ_selectmode, expand_سیٹ_selectmode,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"sessionoptions", "ssop", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شدہ FEAT_SESSION
			    (char_u *)&p_ssop, PV_NONE,
			    did_سیٹ_sessionoptions, expand_سیٹ_sessionoptions,
	 {(char_u *)"blank,buffers,curdir,folds,help,options,tabpages,winsize,terminal",
							       (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"shell",	    "sh",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_sh, PV_NONE, تہی, تہی,
			    {
#اگر_تعریف_شدہ VMS
			    (char_u *)"-",
#ورنہ
# اگر defined(MSWIN)
			    (char_u *)"",	// set in set_init_1()
# ورنہ
			    (char_u *)"sh",
# endif
#اگر_ختم // VMS
				(char_u *)0L} SCTX_INIT},
    {"shellcmdflag","shcf", P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_shcf, PV_NONE, تہی, تہی,
			    {
#اگر defined(MSWIN)
			    (char_u *)"/c",
#ورنہ
			    (char_u *)"-c",
#اگر_ختم
				(char_u *)0L} SCTX_INIT},
    {"shellpipe",   "sp",   P_STRING|P_VI_DEF|P_SECURE,
#اگر_تعریف_شدہ FEAT_QUICKFIX
			    (char_u *)&p_sp, PV_NONE, did_سیٹ_shellpipe_redir, تہی,
			    {
# اگر defined(UNIX)
			    (char_u *)"| tee",
# ورنہ
			    (char_u *)">",
# endif
				(char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"shellquote",  "shq",  P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_shq, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"shellredir",  "srr",  P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_srr, PV_NONE, did_سیٹ_shellpipe_redir, تہی,
			    {(char_u *)">", (char_u *)0L} SCTX_INIT},
    {"shellslash",  "ssl",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
			    (char_u *)&p_ssl, PV_NONE, did_سیٹ_shellslash, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"shelltemp",   "stmp", P_BOOL,
			    (char_u *)&p_stmp, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"shelltype",   "st",   P_عدد|P_VI_DEF,
#اگر_تعریف_شدہ AMIGA
			    (char_u *)&p_st, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"shellxescape", "sxe", P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_sxe, PV_NONE, تہی, تہی,
			    {
#اگر defined(MSWIN)
			    (char_u *)"\"&|<>()@^",
#ورنہ
			    (char_u *)"",
#اگر_ختم
				(char_u *)0L} SCTX_INIT},
    {"shellxquote", "sxq",  P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_sxq, PV_NONE, تہی, تہی,
			    {
#اگر defined(UNIX) && defined(USE_SYSTEM)
			    (char_u *)"\"",
#ورنہ
			    (char_u *)"",
#اگر_ختم
				(char_u *)0L} SCTX_INIT},
    {"shiftround",  "sr",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sr, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"shiftwidth",  "sw",   P_عدد|P_VI_DEF,
			    (char_u *)&p_sw, PV_SW,
			    did_سیٹ_shiftwidth_tabstop, تہی,
			    {(char_u *)8L, (char_u *)0L} SCTX_INIT},
    {"shortmess",   "shm",  P_STRING|P_VIM|P_FLAGLIST,
			    (char_u *)&p_shm, PV_NONE, did_سیٹ_shortmess, expand_سیٹ_shortmess,
			    {(char_u *)"S", (char_u *)"filnxtToOS"}
			    SCTX_INIT},
    {"shortname",   "sn",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sn, PV_SN, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"showbreak",   "sbr",  P_STRING|P_VI_DEF|P_RALL,
#اگر_تعریف_شدہ FEAT_LINEBREAK
			    (char_u *)&p_sbr, PV_SBR, did_سیٹ_showbreak, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"showcmd",	    "sc",   P_BOOL|P_VIM,
			    (char_u *)&p_sc, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"showcmdloc",  "sloc", P_STRING|P_RSTAT,
			    (char_u *)&p_sloc, PV_NONE, did_سیٹ_showcmdloc, expand_سیٹ_showcmdloc,
			    {(char_u *)"last", (char_u *)"last"} SCTX_INIT},
    {"showfulltag", "sft",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_sft, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"showmatch",   "sm",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sm, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"showmode",    "smd",  P_BOOL|P_VIM,
			    (char_u *)&p_smd, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"showtabline", "stal", P_عدد|P_VI_DEF|P_RALL,
			    (char_u *)&p_stal, PV_NONE, did_سیٹ_showtabline, تہی,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
#اگر defined(FEAT_TABPANEL)
    {"showtabpanel", "stpl", P_عدد|P_RALL,
			    (char_u *)&p_stpl, PV_NONE, did_سیٹ_showtabpanel, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
#اگر_ختم
    {"sidescroll",  "ss",   P_عدد|P_VI_DEF,
			    (char_u *)&p_ss, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"sidescrolloff", "siso", P_عدد|P_VI_DEF|P_VIM|P_RBUF,
			    (char_u *)&p_siso, PV_SISO, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"signcolumn",   "scl",  P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN,
#اگر_تعریف_شدہ FEAT_SIGNS
			    (char_u *)VAR_WIN, PV_SCL, did_سیٹ_signcolumn, expand_سیٹ_signcolumn,
			    {(char_u *)"auto", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"slowopen",    "slow", P_BOOL|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smartcase",   "scs",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_scs, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smartindent", "si",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_si, PV_SI, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smarttab",    "sta",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sta, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smoothscroll", "sms", P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_SMS, did_سیٹ_smoothscroll, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"softtabstop", "sts",  P_عدد|P_VI_DEF|P_VIM,
			    (char_u *)&p_sts, PV_STS, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"sourceany",   تہی,   P_BOOL|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"spell",	    تہی,   P_BOOL|P_VI_DEF|P_RWIN|P_HLONLY,
#اگر_تعریف_شدہ FEAT_SPELL
			    (char_u *)VAR_WIN, PV_SPELL, did_سیٹ_spell, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"spellcapcheck", "spc", P_STRING|P_ALLOCED|P_VI_DEF|P_RBUF|P_HLONLY,
#اگر_تعریف_شدہ FEAT_SPELL
			    (char_u *)&p_spc, PV_SPC, did_سیٹ_spellcapcheck, تہی,
			    {(char_u *)"[.?!]\\_[\\])'\"	 ]\\+", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"spellfile",   "spf",  P_STRING|P_EXPAND|P_ALLOCED|P_VI_DEF|P_SECURE
								  |P_ONECOMMA,
#اگر_تعریف_شدہ FEAT_SPELL
			    (char_u *)&p_spf, PV_SPF, did_سیٹ_spellfile, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"spelllang",   "spl",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA
						     |P_RBUF|P_HLONLY|P_EXPAND,
#اگر_تعریف_شدہ FEAT_SPELL
			    (char_u *)&p_spl, PV_SPL, did_سیٹ_spelllang, تہی,
			    {(char_u *)"en", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"spelloptions", "spo",  P_STRING|P_ALLOCED|P_VI_DEF
					   |P_ONECOMMA|P_NODUP|P_RBUF|P_HLONLY,
#اگر_تعریف_شدہ FEAT_SPELL
			    (char_u *)&p_spo, PV_SPO, did_سیٹ_spelloptions, expand_سیٹ_spelloptions,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"spellsuggest", "sps", P_STRING|P_VI_DEF|P_EXPAND|P_SECURE|P_ONECOMMA,
#اگر_تعریف_شدہ FEAT_SPELL
			    (char_u *)&p_sps, PV_NONE, did_سیٹ_spellsuggest, expand_سیٹ_spellsuggest,
			    {(char_u *)"best", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"splitbelow",  "sb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sb, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"splitkeep",   "spk",  P_STRING,
			    (char_u *)&p_spk, PV_NONE, did_سیٹ_splitkeep, expand_سیٹ_splitkeep,
			    {(char_u *)"cursor", (char_u *)"cursor"} SCTX_INIT},
    {"splitright",  "spr",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_spr, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"startofline", "sol",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sol, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"statusline"  ,"stl",  P_STRING|P_VI_DEF|P_ALLOCED|P_RSTAT|P_MLE,
#اگر_تعریف_شدہ FEAT_STL_OPT
			    (char_u *)&p_stl, PV_STL, did_سیٹ_statusline, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"statuslineopt"  ,"stlo",  P_STRING|P_VI_DEF|P_ALLOCED|P_RSTAT|P_MLE
			    |P_ONECOMMA|P_COLON|P_NODUP,
#اگر_تعریف_شدہ FEAT_STL_OPT
			    (char_u *)&p_stlo, PV_STLO, did_سیٹ_statuslineopt, expand_سیٹ_statuslineopt,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"suffixes",    "su",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_su, PV_NONE, تہی, تہی,
			    {(char_u *)".bak,~,.o,.h,.info,.swp,.obj",
				(char_u *)0L} SCTX_INIT},
    {"suffixesadd", "sua",  P_STRING|P_VI_DEF|P_ALLOCED|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_sua, PV_SUA, تہی, تہی,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"swapfile",    "swf",  P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_swf, PV_SWF, did_سیٹ_swapfile, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"swapsync",    "sws",  P_STRING|P_VI_DEF,
			    (char_u *)&p_sws, PV_NONE, did_سیٹ_swapsync, expand_سیٹ_swapsync,
			    {(char_u *)"fsync", (char_u *)0L} SCTX_INIT},
    {"switchbuf",   "swb",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_swb, PV_NONE, did_سیٹ_switchbuf, expand_سیٹ_switchbuf,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"synmaxcol",   "smc",  P_عدد|P_VI_DEF|P_RBUF,
#اگر_تعریف_شدہ FEAT_SYN_HL
			    (char_u *)&p_smc, PV_SMC, تہی, تہی,
			    {(char_u *)3000L, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"syntax",	    "syn",  P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB|P_NFNAME,
#اگر_تعریف_شدہ FEAT_SYN_HL
			    (char_u *)&p_syn, PV_SYN,
			    did_سیٹ_filetype_or_syntax, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"tabclose",    "tcl",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_tcl, PV_NONE, did_سیٹ_tabclose, expand_سیٹ_tabclose,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"tabline",	    "tal",  P_STRING|P_VI_DEF|P_RALL|P_MLE,
#اگر_تعریف_شدہ FEAT_STL_OPT
			    (char_u *)&p_tal, PV_NONE, did_سیٹ_tabline, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"tabpagemax",  "tpm",  P_عدد|P_VI_DEF,
			    (char_u *)&p_tpm, PV_NONE, تہی, تہی,
			    {(char_u *)10L, (char_u *)0L} SCTX_INIT},
#اگر defined(FEAT_TABPANEL)
    {"tabpanel",  "tpl",    P_STRING|P_VI_DEF|P_RALL|P_MLE,
			    (char_u *)&p_tpl, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"tabpanelopt","tplo",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_COLON
								    |P_NODUP,
			    (char_u *)&p_tplo, PV_NONE, did_سیٹ_tabpanelopt,
			    expand_سیٹ_tabpanelopt,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
#اگر_ختم
    {"tabstop",	    "ts",   P_عدد|P_VI_DEF|P_RBUF,
			    (char_u *)&p_ts, PV_TS,
			    did_سیٹ_shiftwidth_tabstop, تہی,
			    {(char_u *)8L, (char_u *)0L} SCTX_INIT},
    {"tagbsearch",  "tbs",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_tbs, PV_NONE, تہی, تہی,
#اگر_تعریف_شدہ VMS	// binary searching doesn't appear to work on VMS
			    {(char_u *)0L, (char_u *)0L}
#ورنہ
			    {(char_u *)TRUE, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"tagcase",	    "tc",   P_STRING|P_VIM,
			    (char_u *)&p_tc, PV_TC, did_سیٹ_tagcase, expand_سیٹ_tagcase,
			    {(char_u *)"followic", (char_u *)"followic"} SCTX_INIT},
    {"tagfunc",    "tfu",   P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#اگر_تعریف_شدہ FEAT_EVAL
			    (char_u *)&p_tfu, PV_TFU, did_سیٹ_tagfunc, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"taglength",   "tl",   P_عدد|P_VI_DEF,
			    (char_u *)&p_tl, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"tagrelative", "tr",   P_BOOL|P_VIM,
			    (char_u *)&p_tr, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"tags",	    "tag",  P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_لیبل_فہرست, PV_لیبل_فہرست, تہی, تہی,
			    {
#اگر defined(FEAT_EMACS_لیبل_فہرست) && !defined(CASE_INSENSITIVE_FILENAME)
			    (char_u *)"./tags,./TAGS,tags,TAGS",
#ورنہ
			    (char_u *)"./tags,tags",
#اگر_ختم
				(char_u *)0L} SCTX_INIT},
    {"tagsecure",   "tsc",   P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_tagsecure, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)TRUE} SCTX_INIT},
    {"tagstack",    "tgst", P_BOOL|P_VI_DEF,
			    (char_u *)&p_tgst, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"tcldll",      تہی,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر defined(DYNAMIC_TCL)
			    (char_u *)&p_tcldll, PV_NONE, تہی, تہی,
			    {(char_u *)DYNAMIC_TCL_DLL, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"term",	    تہی,   P_STRING|P_EXPAND|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RALL,
			    (char_u *)&T_NAME, PV_NONE, did_سیٹ_رکن, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"termbidi", "tbidi",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_ARABIC
			    (char_u *)&p_tbidi, PV_NONE, تہی, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"termencoding", "tenc", P_STRING|P_VI_DEF|P_RCLR,
			    (char_u *)&p_tenc, PV_NONE, did_سیٹ_encoding, expand_سیٹ_encoding,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"termguicolors", "tgc", P_BOOL|P_VI_DEF|P_VIM|P_RCLR,
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
			    (char_u *)&p_tgc, PV_NONE, did_سیٹ_termguicolors, تہی,
			    {(char_u *)FALSE, (char_u *)FALSE}
#ورنہ
			    (char_u*)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)FALSE}
#اگر_ختم
			    SCTX_INIT},
    {"termresize", "trz", P_STRING|P_VI_DEF,
#اگر_تعریف_شدہ UNIX
			    (char_u *)&p_trz, PV_NONE, did_سیٹ_termresize, expand_سیٹ_termresize,
			    {(char_u *)"", (char_u *)0}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"termsync", "tsy",	    P_BOOL|P_VI_DEF,
			    (char_u *)&p_tsy, PV_NONE, did_سیٹ_termsync, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"termwinkey", "twk",   P_STRING|P_ALLOCED|P_RWIN|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_TERMINAL
			    (char_u *)VAR_WIN, PV_TWK, did_سیٹ_termwinkey, تہی,
			    {(char_u *)"", (char_u *)تہی}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"termwinscroll", "twsl", P_عدد|P_VI_DEF|P_VIM|P_RBUF,
#اگر_تعریف_شدہ FEAT_TERMINAL
			    (char_u *)&p_twsl, PV_TWSL, did_سیٹ_termwinscroll, تہی,
			    {(char_u *)10000L, (char_u *)10000L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"termwinsize", "tws",  P_STRING|P_ALLOCED|P_RWIN|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_TERMINAL
			    (char_u *)VAR_WIN, PV_TWS, did_سیٹ_termwinsize, تہی,
			    {(char_u *)"", (char_u *)تہی}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"termwintype", "twt",  P_STRING|P_ALLOCED|P_VI_DEF,
#اگر defined(MSWIN) && defined(FEAT_TERMINAL)
			    (char_u *)&p_twt, PV_NONE, did_سیٹ_termwintype, expand_سیٹ_termwintype,
			    {(char_u *)"", (char_u *)تہی}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"terse",	    تہی,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_terse, PV_NONE, did_سیٹ_terse, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"textauto",    "ta",   P_BOOL|P_VIM,
			    (char_u *)&p_ta, PV_NONE, did_سیٹ_textauto, تہی,
			    {(char_u *)DFLT_TEXTAUTO, (char_u *)TRUE}
			    SCTX_INIT},
    {"textmode",    "tx",   P_BOOL|P_VI_DEF|P_NO_MKRC,
			    (char_u *)&p_tx, PV_TX, did_سیٹ_textmode, تہی,
			    {
#اگر_تعریف_شدہ USE_CRNL
			    (char_u *)TRUE,
#ورنہ
			    (char_u *)FALSE,
#اگر_ختم
				(char_u *)0L} SCTX_INIT},
    {"textwidth",   "tw",   P_عدد|P_VI_DEF|P_VIM|P_RBUF|P_HLONLY,
			    (char_u *)&p_tw, PV_TW, did_سیٹ_textwidth, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"thesaurus",   "tsr",  P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA|P_NODUP|P_NDNAME,
			    (char_u *)&p_tsr, PV_TSR, تہی, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"thesaurusfunc", "tsrfu",  P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
			    (char_u *)&p_tsrfu, PV_TSRFU,
			    did_سیٹ_thesaurusfunc, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"tildeop",	    "top",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_مقصد, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"timeout",	    "to",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_timeout, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"timeoutlen",  "tm",   P_عدد|P_VI_DEF,
			    (char_u *)&p_tm, PV_NONE, تہی, تہی,
			    {(char_u *)1000L, (char_u *)0L} SCTX_INIT},
    {"title",	    تہی,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_title, PV_NONE, did_سیٹ_title_icon, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"titlelen",    تہی,   P_عدد|P_VI_DEF,
			    (char_u *)&p_titlelen, PV_NONE, did_سیٹ_titlelen, تہی,
			    {(char_u *)85L, (char_u *)0L} SCTX_INIT},
    {"titleold",    تہی,   P_STRING|P_VI_DEF|P_GETTEXT|P_SECURE|P_NO_MKRC,
			    (char_u *)&p_titleold, PV_NONE, تہی, تہی,
			    {(char_u *)N_("Thanks for flying Vim"),
							       (char_u *)0L}
			    SCTX_INIT},
    {"titlestring", تہی,   P_STRING|P_VI_DEF|P_MLE,
			    (char_u *)&p_titlestring, PV_NONE,
			    did_سیٹ_titlestring, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"toolbar",     "tb",   P_STRING|P_ONECOMMA|P_VI_DEF|P_NODUP,
#اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
			    (char_u *)&p_toolbar, PV_NONE, did_سیٹ_toolbar, expand_سیٹ_toolbar,
			    {(char_u *)"icons,tooltips", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"toolbariconsize",	"tbis", P_STRING|P_VI_DEF,
#اگر defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
			    (char_u *)&p_tbis, PV_NONE,
			    did_سیٹ_toolbariconsize, expand_سیٹ_toolbariconsize,
			    {(char_u *)"small", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"ttimeout",    تہی,   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_ttimeout, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"ttimeoutlen", "ttm",  P_عدد|P_VI_DEF,
			    (char_u *)&p_ttm, PV_NONE, تہی, تہی,
			    {(char_u *)-1L, (char_u *)0L} SCTX_INIT},
    {"ttybuiltin",  "tbi",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_tbi, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"ttyfast",	    "tf",   P_BOOL|P_NO_MKRC|P_VI_DEF,
			    (char_u *)&p_tf, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"ttymouse",    "ttym", P_STRING|P_NODEFAULT|P_NO_MKRC|P_VI_DEF,
#اگر defined(UNIX) || defined(VMS)
			    (char_u *)&p_ttym, PV_NONE, did_سیٹ_ttymouse, expand_سیٹ_ttymouse,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"ttyscroll",   "tsl",  P_عدد|P_VI_DEF,
			    (char_u *)&p_ttyscroll, PV_NONE, تہی, تہی,
			    {(char_u *)999L, (char_u *)0L} SCTX_INIT},
    {"ttytype",	    "tty",  P_STRING|P_EXPAND|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RALL,
			    (char_u *)&T_NAME, PV_NONE, did_سیٹ_رکن, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"undodir",     "udir", P_STRING|P_EXPAND|P_ONECOMMA|P_NODUP|P_SECURE
								    |P_VI_DEF,
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
			    (char_u *)&p_udir, PV_NONE, تہی, تہی,
			    {(char_u *)".", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"undofile",    "udf",  P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
			    (char_u *)&p_udf, PV_UDF, did_سیٹ_undofile, تہی,
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
#اگر_ختم
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"undolevels",  "ul",   P_عدد|P_VI_DEF,
			    (char_u *)&p_ul, PV_UL, did_سیٹ_undolevels, تہی,
			    {
#اگر defined(UNIX) || defined(MSWIN) || defined(VMS)
			    (char_u *)1000L,
#ورنہ
			    (char_u *)100L,
#اگر_ختم
				(char_u *)0L} SCTX_INIT},
    {"undoreload",  "ur",   P_عدد|P_VI_DEF,
			    (char_u *)&p_ur, PV_NONE, تہی, تہی,
			    { (char_u *)10000L, (char_u *)0L} SCTX_INIT},
    {"updatecount", "uc",   P_عدد|P_VI_DEF,
			    (char_u *)&p_uc, PV_NONE, did_سیٹ_updatecount, تہی,
			    {(char_u *)200L, (char_u *)0L} SCTX_INIT},
    {"updatetime",  "ut",   P_عدد|P_VI_DEF,
			    (char_u *)&p_ut, PV_NONE, تہی, تہی,
			    {(char_u *)4000L, (char_u *)0L} SCTX_INIT},
    {"varsofttabstop", "vsts",  P_STRING|P_VI_DEF|P_VIM|P_COMMA,
#اگر_تعریف_شدہ FEAT_VARTABS
			    (char_u *)&p_vsts, PV_VSTS,
			    did_سیٹ_varsofttabstop, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)تہی}
#اگر_ختم
			    SCTX_INIT},
    {"vartabstop",  "vts",  P_STRING|P_VI_DEF|P_VIM|P_RBUF|P_COMMA,
#اگر_تعریف_شدہ FEAT_VARTABS
			    (char_u *)&p_vts, PV_VTS, did_سیٹ_vartabstop, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)تہی}
#اگر_ختم
			    SCTX_INIT},
    {"verbose",	    "vbs",  P_عدد|P_VI_DEF,
			    (char_u *)&p_verbose, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"verbosefile", "vfile", P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_vfile, PV_NONE, did_سیٹ_verbosefile, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"viewdir",     "vdir", P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر_تعریف_شدہ FEAT_SESSION
			    (char_u *)&p_vdir, PV_NONE, تہی, تہی,
			    {(char_u *)DFLT_VDIR, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"viewoptions", "vop",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شدہ FEAT_SESSION
			    (char_u *)&p_vop, PV_NONE, did_سیٹ_viewoptions, expand_سیٹ_sessionoptions,
			    {(char_u *)"folds,options,cursor,curdir",
								  (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"viminfo",	    "vi",   P_STRING|P_ONECOMMA|P_NODUP|P_SECURE,
#اگر_تعریف_شدہ FEAT_VIMINFO
			    (char_u *)&p_viminfo, PV_NONE, did_سیٹ_viminfo, تہی,
# اگر defined(MSWIN)
			    {(char_u *)"", (char_u *)"'100,<50,s10,h,rA:,rB:"}
# elif defined(AMIGA)
			    {(char_u *)"",
				 (char_u *)"'100,<50,s10,h,rdf0:,rdf1:,rdf2:"}
# ورنہ
			    {(char_u *)"", (char_u *)"'100,<50,s10,h"}
# endif
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"viminfofile", "vif",  P_STRING|P_EXPAND|P_ONECOMMA|P_NODUP
							    |P_SECURE|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_VIMINFO
			    (char_u *)&p_viminfofile, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"virtualedit", "ve",   P_STRING|P_ONECOMMA|P_NODUP|P_VI_DEF
							    |P_VIM|P_CURSWANT,
			    (char_u *)&p_ve, PV_VE, did_سیٹ_virtualedit, expand_سیٹ_virtualedit,
			    {(char_u *)"", (char_u *)""}
			    SCTX_INIT},
    {"visualbell",  "vb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_vb, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"w1200",	    تہی,   P_عدد|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"w300",	    تہی,   P_عدد|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"w9600",	    تہی,   P_عدد|P_VI_DEF,
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"warn",	    تہی,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_warn, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"weirdinvert", "wiv",  P_BOOL|P_VI_DEF|P_RCLR,
			    (char_u *)&p_wiv, PV_NONE, did_سیٹ_weirdinvert, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"whichwrap",   "ww",   P_STRING|P_VIM|P_ONECOMMA|P_FLAGLIST,
			    (char_u *)&p_ww, PV_NONE, did_سیٹ_whichwrap, expand_سیٹ_whichwrap,
			    {(char_u *)"", (char_u *)"b,s"} SCTX_INIT},
    {"wildchar",    "wc",   P_عدد|P_VIM,
			    (char_u *)&p_wc, PV_NONE, did_سیٹ_wildchar, تہی,
			    {(char_u *)(طویل)Ctrl_E, (char_u *)(طویل)TAB}
			    SCTX_INIT},
    {"wildcharm",   "wcm",  P_عدد|P_VI_DEF,
			    (char_u *)&p_wcm, PV_NONE, did_سیٹ_wildchar, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"wildignore",  "wig",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_wig, PV_NONE, تہی, تہی,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"wildignorecase", "wic", P_BOOL|P_VI_DEF,
			    (char_u *)&p_wic, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"wildmenu",    "wmnu", P_BOOL|P_VI_DEF,
			    (char_u *)&p_wmnu, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"wildmode",    "wim",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_wim, PV_NONE, did_سیٹ_wildmode, expand_سیٹ_wildmode,
			    {(char_u *)"full", (char_u *)0L} SCTX_INIT},
    {"wildoptions", "wop",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_wop, PV_NONE, did_سیٹ_wildoptions, expand_سیٹ_wildoptions,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"winaltkeys",  "wak",  P_STRING|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_WAK
			    (char_u *)&p_wak, PV_NONE, did_سیٹ_winaltkeys, expand_سیٹ_winaltkeys,
			    {(char_u *)"menu", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"wincolor", "wcr",	    P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_WCR, did_سیٹ_wincolor, expand_سیٹ_wincolor,
			    {(char_u *)"", (char_u *)تہی}
			    SCTX_INIT},
    {"window",	    "wi",   P_عدد|P_VI_DEF,
			    (char_u *)&p_کھڑکی, PV_NONE, did_سیٹ_کھڑکی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"winfixbuf", "wfb",    P_BOOL|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_WFB, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"winfixheight", "wfh", P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)VAR_WIN, PV_WFH, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"winfixwidth", "wfw",  P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)VAR_WIN, PV_WFW, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"winheight",   "wh",   P_عدد|P_VI_DEF,
			    (char_u *)&p_wh, PV_NONE,
			    did_سیٹ_winheight_helpheight, تہی,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"winhighlight", "whl", P_STRING|P_VI_DEF|P_RALL|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)VAR_WIN, PV_WHL, did_سیٹ_winhighlight, expand_سیٹ_winhighlight,
			    {(char_u *)"", (char_u *)تہی} SCTX_INIT},
    {"winminheight", "wmh", P_عدد|P_VI_DEF,
			    (char_u *)&p_wmh, PV_NONE, did_سیٹ_winminheight, تہی,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"winminwidth", "wmw",  P_عدد|P_VI_DEF,
			    (char_u *)&p_wmw, PV_NONE, did_سیٹ_winminwidth, تہی,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"winptydll", تہی,	    P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر defined(MSWIN) && defined(FEAT_TERMINAL)
			    (char_u *)&p_winptydll, PV_NONE, تہی, تہی, {
# ifdef _WIN64
			    (char_u *)"winpty64.dll",
# ورنہ
			    (char_u *)"winpty32.dll",
# endif
				(char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"winwidth",   "wiw",   P_عدد|P_VI_DEF,
			    (char_u *)&p_wiw, PV_NONE, did_سیٹ_winwidth, تہی,
			    {(char_u *)20L, (char_u *)0L} SCTX_INIT},
    {"wlseat",	    "wse",  P_STRING|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_WAYLAND
			    (char_u *)&p_wse, PV_NONE, did_سیٹ_wlseat, تہی,
			    {(char_u *)"", (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"wlsteal",	    "wst",  P_BOOL|P_VI_DEF, // Deprecated
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
			    SCTX_INIT},
    {"wltimeoutlen", "wtm", P_عدد|P_VI_DEF,
#اگر_تعریف_شدہ FEAT_WAYLAND
			    (char_u *)&p_wtm, PV_NONE, did_سیٹ_wltimeoutlen, تہی,
			    {(char_u *)500L, (char_u *)0L}
#ورنہ
			    (char_u *)تہی, PV_NONE, تہی, تہی,
			    {(char_u *)تہی, (char_u *)0L}
#اگر_ختم
			    SCTX_INIT},
    {"wrap",	    تہی,   P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_WRAP, did_سیٹ_wrap, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"wrapmargin",  "wm",   P_عدد|P_VI_DEF,
			    (char_u *)&p_wm, PV_WM, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"wrapscan",    "ws",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ws, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"write",	    تہی,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_write, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"writeany",    "wa",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_wa, PV_NONE, تہی, تہی,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"writebackup", "wb",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_wb, PV_NONE, تہی, تہی,
			    {
#اگر_تعریف_شدہ FEAT_WRITEBACKUP
			    (char_u *)TRUE,
#ورنہ
			    (char_u *)FALSE,
#اگر_ختم
				(char_u *)0L} SCTX_INIT},
    {"writedelay",  "wd",   P_عدد|P_VI_DEF,
			    (char_u *)&p_wd, PV_NONE, تہی, تہی,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"xtermcodes",  تہی,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_xtermcodes, PV_NONE, تہی, تہی,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},

// terminal output codes
#تعریف_کرو p_رکن(sss, vvv) \
			    {sss, تہی, P_STRING|P_VI_DEF|P_RALL|P_SECURE, \
			    (char_u *)&vvv, PV_NONE, did_سیٹ_رکن_option, تہی, \
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},

    p_رکن("t_AB", T_CAB)
    p_رکن("t_AF", T_CAF)
    p_رکن("t_AU", T_CAU)
    p_رکن("t_AL", T_CAL)
    p_رکن("t_al", T_AL)
    p_رکن("t_bc", T_BC)
    p_رکن("t_BE", T_BE)
    p_رکن("t_BD", T_BD)
    p_رکن("t_cd", T_CD)
    p_رکن("t_ce", T_CE)
    p_رکن("t_Ce", T_UCE)
    p_رکن("t_CF", T_CFO)
    p_رکن("t_cl", T_CL)
    p_رکن("t_cm", T_CM)
    p_رکن("t_Co", T_CCO)
    p_رکن("t_CS", T_CCS)
    p_رکن("t_Cs", T_UCS)
    p_رکن("t_cs", T_CS)
    p_رکن("t_CV", T_CSV)
    p_رکن("t_da", T_DA)
    p_رکن("t_db", T_DB)
    p_رکن("t_DL", T_CDL)
    p_رکن("t_dl", T_DL)
    p_رکن("t_ds", T_DS)
    p_رکن("t_Ds", T_CDS)
    p_رکن("t_EC", T_CEC)
    p_رکن("t_EI", T_CEI)
    p_رکن("t_fs", T_FS)
    p_رکن("t_fd", T_FD)
    p_رکن("t_fe", T_FE)
    p_رکن("t_GP", T_CGP)
    p_رکن("t_IE", T_CIE)
    p_رکن("t_IS", T_CIS)
    p_رکن("t_ke", T_KE)
    p_رکن("t_ks", T_KS)
    p_رکن("t_le", T_LE)
    p_رکن("t_mb", T_MB)
    p_رکن("t_md", T_MD)
    p_رکن("t_me", T_ME)
    p_رکن("t_mr", T_MR)
    p_رکن("t_ms", T_MS)
    p_رکن("t_nd", T_ND)
    p_رکن("t_op", T_OP)
    p_رکن("t_RF", T_RFG)
    p_رکن("t_RB", T_RBG)
    p_رکن("t_RC", T_چکری_جانچ)
    p_رکن("t_RI", T_CRI)
    p_رکن("t_Ri", T_SRI)
    p_رکن("t_RK", T_CRK)
    p_رکن("t_RS", T_CRS)
    p_رکن("t_RT", T_CRT)
    p_رکن("t_RV", T_CRV)
    p_رکن("t_Sb", T_CSB)
    p_رکن("t_SC", T_CSC)
    p_رکن("t_se", T_SE)
    p_رکن("t_Sf", T_CSF)
    p_رکن("t_SH", T_CSH)
    p_رکن("t_SI", T_CSI)
    p_رکن("t_Si", T_SSI)
    p_رکن("t_so", T_SO)
    p_رکن("t_SR", T_CSR)
    p_رکن("t_sr", T_SR)
    p_رکن("t_ST", T_CST)
    p_رکن("t_Te", T_STE)
    p_رکن("t_te", T_TE)
    p_رکن("t_TE", T_CTE)
    p_رکن("t_ti", T_TI)
    p_رکن("t_TI", T_CTI)
    p_رکن("t_Ts", T_STS)
    p_رکن("t_ts", T_TS)
    p_رکن("t_u7", T_U7)
    p_رکن("t_ue", T_UE)
    p_رکن("t_us", T_US)
    p_رکن("t_Us", T_USS)
    p_رکن("t_ut", T_UT)
    p_رکن("t_vb", T_VB)
    p_رکن("t_ve", T_VE)
    p_رکن("t_vi", T_VI)
    p_رکن("t_VS", T_CVS)
    p_رکن("t_vs", T_VS)
    p_رکن("t_WP", T_CWP)
    p_رکن("t_WS", T_CWS)
    p_رکن("t_XM", T_CXM)
    p_رکن("t_xn", T_XN)
    p_رکن("t_xs", T_XS)
    p_رکن("t_ZH", T_CZH)
    p_رکن("t_ZR", T_CZR)
    p_رکن("t_8f", T_8F)
    p_رکن("t_8b", T_8B)
    p_رکن("t_8u", T_8U)
    p_رکن("t_xo", T_XON)
    p_رکن("t_BS", T_BSU)
    p_رکن("t_ES", T_ESU)

// terminal key codes are not in here

    // end marker
    {تہی, تہی, 0, تہی, PV_NONE, تہی, تہی, {تہی, تہی} SCTX_INIT}
};

#تعریف_کرو OPTION_گنتی ARRAY_لمبائی(options)

// The following is needed to make the gen_opt_test.vim script work.
// {"
