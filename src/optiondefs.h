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
#تعریف_کن PV_BOTH 0x1000
#تعریف_کن PV_WIN  0x2000
#تعریف_کن PV_BUF  0x4000
#تعریف_کن PV_ماسک 0x0fff
#تعریف_کن OPT_WIN(x)  (idopt_T)(PV_WIN + (صحيح)(x))
#تعریف_کن OPT_BUF(x)  (idopt_T)(PV_BUF + (صحيح)(x))
#تعریف_کن OPT_BOTH(x) (idopt_T)(PV_BOTH + (صحيح)(x))

// Definition of the PV_ values for buffer-local options.
// The BV_ values are defined in option.h.
#تعریف_کن PV_AC		OPT_BOTH(OPT_BUF(BV_AC))
#تعریف_کن PV_AI		OPT_BUF(BV_AI)
#تعریف_کن PV_AR		OPT_BOTH(OPT_BUF(BV_AR))
#تعریف_کن PV_BKC		OPT_BOTH(OPT_BUF(BV_BKC))
#تعریف_کن PV_BH		OPT_BUF(BV_BH)
#تعریف_کن PV_BT		OPT_BUF(BV_BT)
#اگر_تعریف_شده FEAT_QUICKFIX
# define PV_EFM		OPT_BOTH(OPT_BUF(BV_EFM))
# define PV_GEFM	OPT_BOTH(OPT_BUF(BV_GEFM))
# define PV_GP		OPT_BOTH(OPT_BUF(BV_GP))
# define PV_MP		OPT_BOTH(OPT_BUF(BV_MP))
#پایان_اگر
#تعریف_کن PV_BIN		OPT_BUF(BV_BIN)
#تعریف_کن PV_BL		OPT_BUF(BV_BL)
#تعریف_کن PV_BOMB		OPT_BUF(BV_BOMB)
#تعریف_کن PV_CI		OPT_BUF(BV_CI)
#تعریف_کن PV_CIN		OPT_BUF(BV_CIN)
#تعریف_کن PV_CINK		OPT_BUF(BV_CINK)
#تعریف_کن PV_CINO		OPT_BUF(BV_CINO)
#تعریف_کن PV_CINSD	OPT_BUF(BV_CINSD)
#تعریف_کن PV_CINW		OPT_BUF(BV_CINW)
#تعریف_کن PV_CM		OPT_BOTH(OPT_BUF(BV_CM))
#تعریف_کن PV_CMS		OPT_BUF(BV_CMS)
#تعریف_کن PV_COM		OPT_BUF(BV_COM)
#تعریف_کن PV_COT		OPT_BOTH(OPT_BUF(BV_COT))
#تعریف_کن PV_CPT		OPT_BUF(BV_CPT)
#تعریف_کن PV_DICT		OPT_BOTH(OPT_BUF(BV_DICT))
#اگر_تعریف_شده FEAT_DIFF
# define PV_DIA		OPT_BOTH(OPT_BUF(BV_DIA))
#پایان_اگر
#تعریف_کن PV_TSR		OPT_BOTH(OPT_BUF(BV_TSR))
#تعریف_کن PV_FFU		OPT_BOTH(OPT_BUF(BV_FFU))
#تعریف_کن PV_CSL		OPT_BUF(BV_CSL)
#اگر_تعریف_شده FEAT_COMPL_FUNC
# define PV_CFU		OPT_BUF(BV_CFU)
#پایان_اگر
#اگر_تعریف_شده FEAT_پیدا_کن_ID
# define PV_DEF		OPT_BOTH(OPT_BUF(BV_DEF))
# define PV_INC		OPT_BOTH(OPT_BUF(BV_INC))
#پایان_اگر
#تعریف_کن PV_EOF		OPT_BUF(BV_EOF)
#تعریف_کن PV_EOL		OPT_BUF(BV_EOL)
#تعریف_کن PV_FIXEOL	OPT_BUF(BV_FIXEOL)
#تعریف_کن PV_EP		OPT_BOTH(OPT_BUF(BV_EP))
#تعریف_کن PV_ET		OPT_BUF(BV_ET)
#تعریف_کن PV_FENC		OPT_BUF(BV_FENC)
#إذا defined(FEAT_BEVAL) && defined(FEAT_EVAL)
# define PV_BEXPR	OPT_BOTH(OPT_BUF(BV_BEXPR))
#پایان_اگر
#تعریف_کن PV_FP		OPT_BOTH(OPT_BUF(BV_FP))
#اگر_تعریف_شده FEAT_EVAL
# define PV_FEX		OPT_BUF(BV_FEX)
#پایان_اگر
#تعریف_کن PV_FF		OPT_BUF(BV_FF)
#تعریف_کن PV_FLP		OPT_BUF(BV_FLP)
#تعریف_کن PV_FO		OPT_BUF(BV_FO)
#اگر_تعریف_شده HAVE_FSYNC
# define PV_FS		OPT_BOTH(OPT_BUF(BV_FS))
#پایان_اگر
#تعریف_کن PV_FT		OPT_BUF(BV_FT)
#تعریف_کن PV_IMI		OPT_BUF(BV_IMI)
#تعریف_کن PV_IMS		OPT_BUF(BV_IMS)
#إذا defined(FEAT_EVAL)
# define PV_INDE	OPT_BUF(BV_INDE)
# define PV_INDK	OPT_BUF(BV_INDK)
#پایان_اگر
#إذا defined(FEAT_پیدا_کن_ID) && defined(FEAT_EVAL)
# define PV_INEX	OPT_BUF(BV_INEX)
#پایان_اگر
#تعریف_کن PV_INF		OPT_BUF(BV_INF)
#تعریف_کن PV_ISK		OPT_BUF(BV_ISK)
#اگر_تعریف_شده FEAT_CRYPT
# define PV_کلید		OPT_BUF(BV_کلید)
#پایان_اگر
#اگر_تعریف_شده FEAT_KEYMAP
# define PV_KMAP	OPT_BUF(BV_KMAP)
#پایان_اگر
#تعریف_کن PV_KP		OPT_BOTH(OPT_BUF(BV_KP))
#تعریف_کن PV_LISP		OPT_BUF(BV_LISP)
#تعریف_کن PV_LOP		OPT_BUF(BV_LOP)
#تعریف_کن PV_LW		OPT_BOTH(OPT_BUF(BV_LW))
#تعریف_کن PV_MENC		OPT_BOTH(OPT_BUF(BV_MENC))
#تعریف_کن PV_MA		OPT_BUF(BV_MA)
#تعریف_کن PV_ML		OPT_BUF(BV_ML)
#تعریف_کن PV_MOD		OPT_BUF(BV_MOD)
#تعریف_کن PV_MPS		OPT_BUF(BV_MPS)
#تعریف_کن PV_NF		OPT_BUF(BV_NF)
#اگر_تعریف_شده FEAT_COMPL_FUNC
# define PV_OFU		OPT_BUF(BV_OFU)
#پایان_اگر
#تعریف_کن PV_مسیر		OPT_BOTH(OPT_BUF(BV_مسیر))
#تعریف_کن PV_PI		OPT_BUF(BV_PI)
#تعریف_کن PV_QE		OPT_BUF(BV_QE)
#تعریف_کن PV_RO		OPT_BUF(BV_RO)
#تعریف_کن PV_SI		OPT_BUF(BV_SI)
#تعریف_کن PV_SN		OPT_BUF(BV_SN)
#اگر_تعریف_شده FEAT_SYN_HL
# define PV_SMC		OPT_BUF(BV_SMC)
# define PV_SYN		OPT_BUF(BV_SYN)
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
# define PV_SPC		OPT_BUF(BV_SPC)
# define PV_SPF		OPT_BUF(BV_SPF)
# define PV_SPL		OPT_BUF(BV_SPL)
# define PV_SPO		OPT_BUF(BV_SPO)
#پایان_اگر
#تعریف_کن PV_STS		OPT_BUF(BV_STS)
#تعریف_کن PV_SUA		OPT_BUF(BV_SUA)
#تعریف_کن PV_SW		OPT_BUF(BV_SW)
#تعریف_کن PV_SWF		OPT_BUF(BV_SWF)
#اگر_تعریف_شده FEAT_EVAL
# define PV_TFU		OPT_BUF(BV_TFU)
#پایان_اگر
#اگر_تعریف_شده FEAT_COMPL_FUNC
# define PV_TSRFU	OPT_BOTH(OPT_BUF(BV_TSRFU))
#پایان_اگر
#تعریف_کن PV_برچسبها		OPT_BOTH(OPT_BUF(BV_برچسبها))
#تعریف_کن PV_TC		OPT_BOTH(OPT_BUF(BV_TC))
#تعریف_کن PV_TS		OPT_BUF(BV_TS)
#تعریف_کن PV_TW		OPT_BUF(BV_TW)
#تعریف_کن PV_TX		OPT_BUF(BV_TX)
#اگر_تعریف_شده FEAT_PERSISTENT_UNDO
# define PV_UDF		OPT_BUF(BV_UDF)
#پایان_اگر
#تعریف_کن PV_WM		OPT_BUF(BV_WM)
#اگر_تعریف_شده FEAT_VARTABS
# define PV_VSTS	OPT_BUF(BV_VSTS)
# define PV_VTS		OPT_BUF(BV_VTS)
#پایان_اگر

// Definition of the PV_ values for window-local options.
// The WV_ values are defined in option.h.
#تعریف_کن PV_LIST		OPT_WIN(WV_LIST)
#اگر_تعریف_شده FEAT_ARABIC
# define PV_ARAB	OPT_WIN(WV_ARAB)
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
# define PV_BRI		OPT_WIN(WV_BRI)
# define PV_BRIOPT	OPT_WIN(WV_BRIOPT)
#پایان_اگر
#تعریف_کن PV_WCR		OPT_WIN(WV_WCR)
#اگر_تعریف_شده FEAT_DIFF
# define PV_DIFF	OPT_WIN(WV_DIFF)
#پایان_اگر
#تعریف_کن PV_EIW		OPT_WIN(WV_EIW)
#اگر_تعریف_شده FEAT_FOLDING
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
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
# define PV_LBR		OPT_WIN(WV_LBR)
#پایان_اگر
#تعریف_کن PV_LCS		OPT_BOTH(OPT_WIN(WV_LCS))
#تعریف_کن PV_FCS		OPT_BOTH(OPT_WIN(WV_FCS))
#تعریف_کن PV_NU		OPT_WIN(WV_NU)
#تعریف_کن PV_RNU		OPT_WIN(WV_RNU)
#تعریف_کن PV_VE		OPT_BOTH(OPT_WIN(WV_VE))
#اگر_تعریف_شده FEAT_LINEBREAK
# define PV_NUW		OPT_WIN(WV_NUW)
#پایان_اگر
#إذا defined(FEAT_QUICKFIX)
# define PV_PVW		OPT_WIN(WV_PVW)
# define PV_LHI         OPT_WIN(WV_LHI)
#پایان_اگر
#اگر_تعریف_شده FEAT_RIGHTLEFT
# define PV_RL		OPT_WIN(WV_RL)
# define PV_RLC		OPT_WIN(WV_RLC)
#پایان_اگر
#تعریف_کن PV_SCBIND	OPT_WIN(WV_SCBIND)
#تعریف_کن PV_SCROLL	OPT_WIN(WV_SCROLL)
#تعریف_کن PV_SMS		OPT_WIN(WV_SMS)
#تعریف_کن PV_SISO		OPT_BOTH(OPT_WIN(WV_SISO))
#تعریف_کن PV_SO		OPT_BOTH(OPT_WIN(WV_SO))
#تعریف_کن PV_SOP		OPT_BOTH(OPT_WIN(WV_SOP))
#اگر_تعریف_شده FEAT_SPELL
# define PV_SPELL	OPT_WIN(WV_SPELL)
#پایان_اگر
#اگر_تعریف_شده FEAT_SYN_HL
# define PV_CUC		OPT_WIN(WV_CUC)
# define PV_CUL		OPT_WIN(WV_CUL)
# define PV_CULOPT	OPT_WIN(WV_CULOPT)
# define PV_CC		OPT_WIN(WV_CC)
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
# define PV_SBR		OPT_BOTH(OPT_WIN(WV_SBR))
#پایان_اگر
#اگر_تعریف_شده FEAT_STL_OPT
# define PV_STL		OPT_BOTH(OPT_WIN(WV_STL))
# define PV_STLO	OPT_BOTH(OPT_WIN(WV_STLO))
#پایان_اگر
#تعریف_کن PV_UL		OPT_BOTH(OPT_BUF(BV_UL))
#تعریف_کن PV_WFB		OPT_WIN(WV_WFB)
#تعریف_کن PV_WFH		OPT_WIN(WV_WFH)
#تعریف_کن PV_WFW		OPT_WIN(WV_WFW)
#تعریف_کن PV_WHL		OPT_WIN(WV_WHL)
#تعریف_کن PV_WRAP		OPT_WIN(WV_WRAP)
#تعریف_کن PV_CRBIND	OPT_WIN(WV_CRBIND)
#اگر_تعریف_شده FEAT_CONCEAL
# define PV_COCU	OPT_WIN(WV_COCU)
# define PV_COLE	OPT_WIN(WV_COLE)
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMINAL
# define PV_TWK		OPT_WIN(WV_TWK)
# define PV_TWS		OPT_WIN(WV_TWS)
# define PV_TWSL	OPT_BUF(BV_TWSL)
#پایان_اگر
#اگر_تعریف_شده FEAT_SIGNS
# define PV_SCL		OPT_WIN(WV_SCL)
#پایان_اگر

// WV_ and BV_ values get typecasted to this for the "indir" field
تعريفنوع تعداد
{
    PV_NONE = 0,
    PV_MAXVAL = 0xffff    // to avoid warnings for value out of range
} idopt_T;

// Options local to a window have a value local to a buffer and global to all
// buffers.  Indicate this by setting "var" to VAR_WIN.
#تعریف_کن VAR_WIN ((char_u *)-1)

// Saved values for when 'bin' is set.
ساكن صحيح	p_et_nobin;
ساكن صحيح	p_ml_nobin;
ساكن دراز	p_tw_nobin;
ساكن دراز	p_wm_nobin;

// Saved values for when 'paste' is set
ساكن صحيح	p_ai_nopaste;
ساكن صحيح	p_et_nopaste;
ساكن دراز	p_sts_nopaste;
ساكن دراز	p_tw_nopaste;
ساكن دراز	p_wm_nopaste;
#اگر_تعریف_شده FEAT_VARTABS
ساكن char_u	*p_vsts_nopaste;
#پایان_اگر

بنية vimoption
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
    opt_did_مجموعه_cb_T	opt_did_مجموعه_cb;

    // callback function to invoke when expanding possible values on the
    // cmdline. Only useful for string options.
    opt_expand_cb_T	opt_expand_cb;

    char_u	*def_val[2];	// default values for variable (vi and vim)
#اگر_تعریف_شده FEAT_EVAL
    sctx_T	script_ctx;	// script context where the option was last set
# define SCTX_INIT , {0, 0, 0, 1}
#وإلا
# define SCTX_INIT
#پایان_اگر
};

#تعریف_کن VI_DEFAULT  0	    // def_val[VI_DEFAULT] is Vi default value
#تعریف_کن VIM_DEFAULT 1	    // def_val[VIM_DEFAULT] is Vim default value

#تعریف_کن ISK_LATIN1  (char_u *)"@,48-57,_,192-255"

// 'isprint' for latin1 is also used for MS-Windows cp1252, where 0x80 is used
// for the currency sign.
#إذا defined(MSWIN)
# define ISP_LATIN1 (char_u *)"@,~-255"
#وإلا
# define ISP_LATIN1 (char_u *)"@,161-255"
#پایان_اگر

#تعریف_کن HIGHLIGHT_INIT "8:SpecialKey,~:EndOfBuffer,@:NonText,d:Directory,e:ErrorMsg,i:IncSearch,l:Search,y:CurSearch,m:MoreMsg,M:ModeMsg,n:LineNr,a:LineNrAbove,b:LineNrBelow,N:CursorLineNr,G:CursorLineSign,O:CursorLineFold,r:Question,s:StatusLine,S:StatusLineNC,c:VertSplit,|:VertSplitNC,t:Title,v:Visual,V:VisualNOS,w:WarningMsg,W:WildMenu,f:Folded,F:FoldColumn,A:DiffAdd,C:DiffChange,D:DiffDelete,T:DiffText,E:DiffTextAdd,>:SignColumn,-:Conceal,B:SpellBad,P:SpellCap,R:SpellRare,L:SpellLocal,+:Pmenu,=:PmenuSel,k:PmenuMatch,<:PmenuMatchSel,[:PmenuKind,]:PmenuKindSel,{:PmenuExtra,}:PmenuExtraSel,x:PmenuSbar,X:PmenuThumb,j:PmenuBorder,H:PmenuShadow,p:Popup,J:PopupBorder,Q:PopupTitle,*:TabLine,#:TabLineSel,_:TabLineFill,!:CursorColumn,.:CursorLine,o:ColorColumn,q:QuickFixLine,z:StatusLineTerm,Z:StatusLineTermNC,g:MsgArea,h:ComplMatchIns,%:TabPanel,^:TabPanelSel,&:TabPanelFill,I:PreInsert"

// Default python version for pyx* commands
#إذا defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
# define DEFAULT_PYTHON_VER	0
#وإلاإذا defined(FEAT_PYTHON3)
# define DEFAULT_PYTHON_VER	3
#وإلاإذا defined(FEAT_PYTHON)
# define DEFAULT_PYTHON_VER	2
#وإلا
# define DEFAULT_PYTHON_VER	0
#پایان_اگر

// used for 'cinkeys' and 'indentkeys'
#تعریف_کن INDENTKEYS_DEFAULT (char_u *)"0{,0},0),0],:,0#,!^F,o,O,e"

// options[] is initialized here.
// The order of the options MUST be alphabetic for ":set all" and findoption().
// All option names MUST start with a lowercase letter (for findoption()).
// Exception: "t_" options are at the end.
// The options with a NULL variable are 'hidden': a set command for them is
// ignored and they are not printed.
ساكن بنية vimoption options[] =
{
    {"aleph",	    "al",   P_رقم|P_VI_DEF|P_CURSWANT,
#اگر_تعریف_شده FEAT_RIGHTLEFT
			    (char_u *)&p_aleph, PV_NONE,
#وإلا
			    (char_u *)عدم, PV_NONE,
#پایان_اگر
			    عدم, عدم,
			    {
#إذا defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
			    (char_u *)128L,
#وإلا
			    (char_u *)224L,
#پایان_اگر
					    (char_u *)0L} SCTX_INIT},
    {"allowrevins", "ari",  P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شده FEAT_RIGHTLEFT
			    (char_u *)&p_ari, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"altkeymap",   "akm",  P_BOOL|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"ambiwidth",  "ambw",  P_STRING|P_VI_DEF|P_RCLR,
			    (char_u *)&p_ambw, PV_NONE, did_مجموعه_ambiwidth, expand_مجموعه_ambiwidth,
			    {(char_u *)"single", (char_u *)0L}
			    SCTX_INIT},
    {"antialias",   "anti", P_BOOL|P_VI_DEF|P_VIM|P_RCLR,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)FALSE}
			    SCTX_INIT},
    {"arabic",	    "arab", P_BOOL|P_VI_DEF|P_VIM|P_CURSWANT,
#اگر_تعریف_شده FEAT_ARABIC
			    (char_u *)VAR_WIN, PV_ARAB, did_مجموعه_arabic, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"arabicshape", "arshape", P_BOOL|P_VI_DEF|P_VIM|P_RCLR,
#اگر_تعریف_شده FEAT_ARABIC
			    (char_u *)&p_arshape, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"autochdir",  "acd",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شده FEAT_AUTOCHDIR
			    (char_u *)&p_acd, PV_NONE, did_مجموعه_autochdir, عدم,
			    {(char_u *)FALSE, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
#اگر_تعریف_شده ELAPSED_FUNC
    {"autocomplete",  "ac", P_BOOL|P_VI_DEF,
			    (char_u *)&p_ac, PV_AC, عدم,
			    عدم, {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autocompletedelay", "acl", P_رقم|P_VI_DEF,
			    (char_u *)&p_acl, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"autocompletetimeout", "act", P_رقم|P_VI_DEF,
			    (char_u *)&p_act, PV_NONE, عدم, عدم,
			    {(char_u *)80L, (char_u *)0L} SCTX_INIT},
#پایان_اگر
    {"autoindent",  "ai",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ai, PV_AI, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autoprint",   "ap",   P_BOOL|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autoread",    "ar",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ar, PV_AR, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autoshelldir",  "asd",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شده FEAT_AUTOSHELLDIR
			    (char_u *)&p_asd, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"autowrite",   "aw",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_aw, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autowriteall","awa",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_awa, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"background",  "bg",   P_STRING|P_VI_DEF|P_RCLR|P_HLONLY,
			    (char_u *)&p_bg, PV_NONE, did_مجموعه_background, expand_مجموعه_background,
			    {
#إذا (defined(MSWIN)) && !defined(FEAT_GUI)
			    (char_u *)"dark",
#وإلا
			    (char_u *)"light",
#پایان_اگر
					    (char_u *)0L} SCTX_INIT},
    {"backspace",   "bs",   P_STRING|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_bs, PV_NONE, did_مجموعه_backspace, expand_مجموعه_backspace,
			    {(char_u *)"", (char_u *)"indent,eol,start"} SCTX_INIT},
    {"backup",	    "bk",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_bk, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"backupcopy",  "bkc",  P_STRING|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_bkc, PV_BKC, did_مجموعه_backupcopy, expand_مجموعه_backupcopy,
#اگر_تعریف_شده UNIX
			    {(char_u *)"yes", (char_u *)"auto"}
#وإلا
			    {(char_u *)"auto", (char_u *)"auto"}
#پایان_اگر
			    SCTX_INIT},
    {"backupdir",   "bdir", P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA
							    |P_NODUP|P_SECURE,
			    (char_u *)&p_bdir, PV_NONE, عدم, عدم,
			    {(char_u *)DFLT_BDIR, (char_u *)0L} SCTX_INIT},
    {"backupext",   "bex",  P_STRING|P_VI_DEF|P_NFNAME,
			    (char_u *)&p_bex, PV_NONE,
			    did_مجموعه_backupext_or_patchmode, عدم,
			    {
#اگر_تعریف_شده VMS
			    (char_u *)"_",
#وإلا
			    (char_u *)"~",
#پایان_اگر
					    (char_u *)0L} SCTX_INIT},
    {"backupskip",  "bsk",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_bsk, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"balloondelay","bdlay",P_رقم|P_VI_DEF,
#اگر_تعریف_شده FEAT_BEVAL
			    (char_u *)&p_bdlay, PV_NONE, عدم, عدم,
			    {(char_u *)600L, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"ballooneval", "beval",P_BOOL|P_VI_DEF|P_NO_MKRC,
#اگر_تعریف_شده FEAT_BEVAL_GUI
			    (char_u *)&p_beval, PV_NONE, did_مجموعه_ballooneval, عدم,
			    {(char_u *)FALSE, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"balloonevalterm", "bevalterm",P_BOOL|P_VI_DEF|P_NO_MKRC,
#اگر_تعریف_شده FEAT_BEVAL_جمله
			    (char_u *)&p_bevalterm, PV_NONE,
			    did_مجموعه_balloonevalterm, عدم,
			    {(char_u *)FALSE, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"balloonexpr", "bexpr", P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_MLE,
#إذا defined(FEAT_BEVAL) && defined(FEAT_EVAL)
			    (char_u *)&p_bexpr, PV_BEXPR, did_مجموعه_optexpr, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"beautify",    "bf",   P_BOOL|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"belloff",      "bo",  P_STRING|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_bo, PV_NONE, did_مجموعه_belloff, expand_مجموعه_belloff,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"binary",	    "bin",  P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_bin, PV_BIN, did_مجموعه_دودویی, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"bioskey",	    "biosk",P_BOOL|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"bomb",	    عدم,   P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_bomb, PV_BOMB,
			    did_مجموعه_eof_eol_fixeol_bomb, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"breakat",	    "brk",  P_STRING|P_VI_DEF|P_RALL|P_FLAGLIST,
#اگر_تعریف_شده FEAT_LINEBREAK
			    (char_u *)&p_breakat, PV_NONE, did_مجموعه_breakat, عدم,
			    {(char_u *)" \t!@*-+;:,./?", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"breakindent",   "bri",  P_BOOL|P_VI_DEF|P_VIM|P_RWIN,
#اگر_تعریف_شده FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_BRI, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"breakindentopt", "briopt", P_STRING|P_ALLOCED|P_VI_DEF|P_RBUF
						  |P_ONECOMMA|P_NODUP,
#اگر_تعریف_شده FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_BRIOPT,
			    did_مجموعه_breakindentopt, expand_مجموعه_breakindentopt,
			    {(char_u *)"", (char_u *)عدم}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)عدم}
#پایان_اگر
			    SCTX_INIT},
    {"browsedir",   "bsdir",P_STRING|P_VI_DEF,
#اگر_تعریف_شده FEAT_BROWSE
			    (char_u *)&p_bsdir, PV_NONE, did_مجموعه_browsedir, expand_مجموعه_browsedir,
			    {(char_u *)"last", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"bufhidden",   "bh",   P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_bh, PV_BH, did_مجموعه_bufhidden, expand_مجموعه_bufhidden,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"buflisted",   "bl",   P_BOOL|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_bl, PV_BL, did_مجموعه_buflisted, عدم,
			    {(char_u *)1L, (char_u *)0L}
			    SCTX_INIT},
    {"buftype",	    "bt",   P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_bt, PV_BT, did_مجموعه_buftype, expand_مجموعه_buftype,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"casemap",	    "cmp",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cmp, PV_NONE, did_مجموعه_casemap, expand_مجموعه_casemap,
			    {(char_u *)"internal,keepascii", (char_u *)0L}
			    SCTX_INIT},
    {"cdhome",	    "cdh",  P_BOOL|P_VI_DEF|P_VIM|P_SECURE,
			    (char_u *)&p_cdh, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L}
			    SCTX_INIT},
    {"cdpath",	    "cd",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE|P_COMMA|P_NODUP,
			    (char_u *)&p_cdpath, PV_NONE, عدم, عدم,
			    {(char_u *)",,", (char_u *)0L}
			    SCTX_INIT},
    {"cedit",	    عدم,   P_STRING,
			    (char_u *)&p_cedit, PV_NONE, did_مجموعه_cedit, عدم,
			    {(char_u *)"", (char_u *)CTRL_F_STR}
			    SCTX_INIT},
    {"charconvert",  "ccv", P_STRING|P_VI_DEF|P_SECURE,
#إذا defined(FEAT_EVAL)
			    (char_u *)&p_ccv, PV_NONE, did_مجموعه_optexpr, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"chistory",    "chi",  P_رقم|P_VI_DEF,
#اگر_تعریف_شده FEAT_QUICKFIX
			    (char_u *)&p_chi, PV_NONE, did_مجموعه_xhistory, عدم,
			    {(char_u *)10L, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"cindent",	    "cin",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_cin, PV_CIN, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cinkeys",	    "cink", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cink, PV_CINK, عدم, عدم,
			    {INDENTKEYS_DEFAULT, (char_u *)0L} SCTX_INIT},
    {"cinoptions",  "cino", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cino, PV_CINO, did_مجموعه_cinoptions, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"cinscopedecls", "cinsd", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cinsd, PV_CINSD, عدم, عدم,
			    {(char_u *)"public,protected,private", (char_u *)0L}
			    SCTX_INIT},
    {"cinwords",    "cinw", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cinw, PV_CINW, عدم, عدم,
			    {(char_u *)"if,else,while,do,for,switch",
				(char_u *)0L}
			    SCTX_INIT},
    {"clipboard",   "cb",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شده HAVE_CLIPMETHOD
			    (char_u *)&p_cb, PV_NONE, did_مجموعه_clipboard, expand_مجموعه_clipboard,
# إذا defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
			    {(char_u *)"autoselect,exclude:cons\\|linux",
							       (char_u *)0L}
# وإلا
			    {(char_u *)"", (char_u *)0L}
# endif
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"clipmethod", "cpm",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شده HAVE_CLIPMETHOD
			    (char_u *)&p_cpm, PV_NONE, did_مجموعه_clipmethod, expand_مجموعه_clipmethod,
# ifdef UNIX
			    {(char_u *)"wayland,x11", (char_u *)0L}
# elif defined(VMS)
			    {(char_u *)"x11", (char_u *)0L}
# وإلا
			    {(char_u *)"", (char_u *)0L}
# endif
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"cmdheight",   "ch",   P_رقم|P_VI_DEF|P_RALL,
			    (char_u *)&p_ch, PV_NONE, did_مجموعه_cmdheight, عدم,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"cmdwinheight", "cwh", P_رقم|P_VI_DEF,
			    (char_u *)&p_cwh, PV_NONE, عدم, عدم,
			    {(char_u *)7L, (char_u *)0L} SCTX_INIT},
    {"colorcolumn", "cc",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_RWIN|P_HLONLY,
#اگر_تعریف_شده FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CC, did_مجموعه_colorcolumn, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"columns",	    "co",   P_رقم|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RCLR,
			    (char_u *)&Columns, PV_NONE, عدم, عدم,
			    {(char_u *)80L, (char_u *)0L} SCTX_INIT},
    {"comments",    "com",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_com, PV_COM, did_مجموعه_comments, عدم,
			    {(char_u *)"s1:/*,mb:*,ex:*/,://,b:#,:%,:XCOMM,n:>,fb:-",
				(char_u *)0L}
			    SCTX_INIT},
    {"commentstring", "cms", P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)&p_cms, PV_CMS, did_مجموعه_commentstring, عدم,
			    {(char_u *)"/* %s */", (char_u *)0L}
			    SCTX_INIT},
			    // P_PRI_MKRC isn't needed here, optval_default()
			    // always returns TRUE for 'compatible'
    {"compatible",  "cp",   P_BOOL|P_RALL,
			    (char_u *)&p_cp, PV_NONE, did_مجموعه_compatible, عدم,
			    {(char_u *)TRUE, (char_u *)FALSE} SCTX_INIT},
    {"complete",    "cpt",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP|P_MLE,
			    (char_u *)&p_cpt, PV_CPT, did_مجموعه_complete, expand_مجموعه_complete,
			    {(char_u *)".,w,b,u,t,i", (char_u *)0L}
			    SCTX_INIT},
    {"completefunc", "cfu", P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#اگر_تعریف_شده FEAT_COMPL_FUNC
			    (char_u *)&p_cfu, PV_CFU, did_مجموعه_completefunc, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"completefuzzycollect", "cfc", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cfc, PV_NONE, did_مجموعه_completefuzzycollect, expand_مجموعه_completefuzzycollect,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"completeitemalign", "cia", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cia, PV_NONE, did_مجموعه_completeitemalign, عدم,
			    {(char_u *)"abbr,kind,menu", (char_u *)0L}
			    SCTX_INIT},
    {"completeopt",   "cot",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cot, PV_COT, did_مجموعه_completeopt, expand_مجموعه_completeopt,
			    {(char_u *)"menu,preview", (char_u *)0L}
			    SCTX_INIT},
    {"completepopup", "cpp", P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON,
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
			    (char_u *)&p_cpp, PV_NONE, did_مجموعه_completepopup, expand_مجموعه_completepopup,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"completeslash",   "csl",  P_STRING|P_VI_DEF|P_VIM,
#إذا defined(BACKSLASH_درون_FILENAME)
			    (char_u *)&p_csl, PV_CSL, did_مجموعه_completeslash, expand_مجموعه_completeslash,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"completetimeout", "cto", P_رقم|P_VI_DEF,
			    (char_u *)&p_cto, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"concealcursor","cocu", P_STRING|P_ALLOCED|P_RWIN|P_VI_DEF|P_FLAGLIST,
#اگر_تعریف_شده FEAT_CONCEAL
			    (char_u *)VAR_WIN, PV_COCU, did_مجموعه_concealcursor, expand_مجموعه_concealcursor,
			    {(char_u *)"", (char_u *)عدم}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"conceallevel","cole", P_رقم|P_RWIN|P_VI_DEF,
#اگر_تعریف_شده FEAT_CONCEAL
			    (char_u *)VAR_WIN, PV_COLE, did_مجموعه_conceallevel, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)0L, (char_u *)0L}
			    SCTX_INIT},
    {"confirm",     "cf",   P_BOOL|P_VI_DEF,
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			    (char_u *)&p_confirm, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"conskey",	    "consk",P_BOOL|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"copyindent",  "ci",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_ci, PV_CI, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cpoptions",   "cpo",  P_STRING|P_VIM|P_RALL|P_FLAGLIST,
			    (char_u *)&p_cpo, PV_NONE, did_مجموعه_cpoptions, expand_مجموعه_cpoptions,
			    {(char_u *)CPO_VI, (char_u *)CPO_VIM}
			    SCTX_INIT},
    {"cryptmethod", "cm",   P_STRING|P_ALLOCED|P_VI_DEF,
#اگر_تعریف_شده FEAT_CRYPT
			    (char_u *)&p_cm, PV_CM, did_مجموعه_cryptmethod, expand_مجموعه_cryptmethod,
			    {(char_u *)"blowfish2", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"cscopepathcomp", "cspc", P_رقم|P_VI_DEF|P_VIM,
#اگر_تعریف_شده FEAT_CSCOPE
			    (char_u *)&p_cspc, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopeprg",   "csprg", P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر_تعریف_شده FEAT_CSCOPE
			    (char_u *)&p_csprg, PV_NONE, عدم, عدم,
			    {(char_u *)"cscope", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"cscopequickfix", "csqf", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#إذا defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
			    (char_u *)&p_csqf, PV_NONE,
			    did_مجموعه_cscopequickfix, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"cscoperelative", "csre", P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شده FEAT_CSCOPE
			    (char_u *)&p_csre, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopetag",   "cst",  P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شده FEAT_CSCOPE
			    (char_u *)&p_cst, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopetagorder", "csto", P_رقم|P_VI_DEF|P_VIM,
#اگر_تعریف_شده FEAT_CSCOPE
			    (char_u *)&p_csto, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopeverbose", "csverb", P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شده FEAT_CSCOPE
			    (char_u *)&p_csverbose, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cursorbind",  "crb",  P_BOOL|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_CRBIND, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cursorcolumn", "cuc", P_BOOL|P_VI_DEF|P_RWIN|P_HLONLY,
#اگر_تعریف_شده FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CUC, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cursorline",   "cul", P_BOOL|P_VI_DEF|P_RWIN|P_HLONLY,
#اگر_تعریف_شده FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CUL, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cursorlineopt", "culopt", P_STRING|P_VI_DEF|P_RWIN|P_HLONLY|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شده FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CULOPT,
			    did_مجموعه_cursorlineopt, expand_مجموعه_cursorlineopt,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)"both", (char_u *)0L} SCTX_INIT},
    {"debug",	    عدم,   P_STRING|P_VI_DEF,
			    (char_u *)&p_debug, PV_NONE, did_مجموعه_debug, expand_مجموعه_debug,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"define",	    "def",  P_STRING|P_ALLOCED|P_VI_DEF,
#اگر_تعریف_شده FEAT_پیدا_کن_ID
			    (char_u *)&p_def, PV_DEF, عدم, عدم,
			    {(char_u *)"^\\s*#\\s*define", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"delcombine", "deco",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_deco, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"dictionary",  "dict", P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA|P_NODUP|P_NDNAME,
			    (char_u *)&p_dict, PV_DICT, عدم, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"diff",	    عدم,   P_BOOL|P_VI_DEF|P_RWIN|P_NOGLOB,
#اگر_تعریف_شده FEAT_DIFF
			    (char_u *)VAR_WIN, PV_DIFF, did_مجموعه_diff, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"diffanchors", "dia",  P_STRING|P_VI_DEF|P_ONECOMMA,
#اگر_تعریف_شده FEAT_DIFF
			    (char_u *)&p_dia, PV_DIA, did_مجموعه_diffanchors, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)"", (char_u *)عدم} SCTX_INIT},
    {"diffexpr",    "dex",  P_STRING|P_VI_DEF|P_SECURE|P_CURSWANT,
#إذا defined(FEAT_DIFF) && defined(FEAT_EVAL)
			    (char_u *)&p_dex, PV_NONE, did_مجموعه_optexpr, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"diffopt",	    "dip",  P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN|P_ONECOMMA|P_COLON
								     |P_NODUP,
#اگر_تعریف_شده FEAT_DIFF
			    (char_u *)&p_dip, PV_NONE, did_مجموعه_diffopt, expand_مجموعه_diffopt,
			    {(char_u *)"internal,filler,closeoff,indent-heuristic,inline:char",
								(char_u *)عدم}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)عدم}
#پایان_اگر
			    SCTX_INIT},
    {"digraph",	    "dg",   P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شده FEAT_DIGRAPHS
			    (char_u *)&p_dg, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"directory",   "dir",  P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA
							    |P_NODUP|P_SECURE,
			    (char_u *)&p_dir, PV_NONE, عدم, عدم,
			    {(char_u *)DFLT_DIR, (char_u *)0L} SCTX_INIT},
    {"display",	    "dy",   P_STRING|P_VI_DEF|P_ONECOMMA|P_RALL|P_NODUP,
			    (char_u *)&p_dy, PV_NONE, did_مجموعه_display, expand_مجموعه_display,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"eadirection", "ead",  P_STRING|P_VI_DEF,
			    (char_u *)&p_ead, PV_NONE, did_مجموعه_eadirection, expand_مجموعه_eadirection,
			    {(char_u *)"both", (char_u *)0L}
			    SCTX_INIT},
    {"edcompatible","ed",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ed, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"emoji",      "emo",   P_BOOL|P_VI_DEF|P_RCLR,
			    (char_u *)&p_emoji, PV_NONE, did_مجموعه_ambiwidth, عدم,
			    {(char_u *)TRUE, (char_u *)0L}
			    SCTX_INIT},
    {"encoding",    "enc",  P_STRING|P_VI_DEF|P_RCLR|P_NO_ML,
			    (char_u *)&p_enc, PV_NONE, did_مجموعه_encoding, expand_مجموعه_encoding,
			    {(char_u *)ENC_DFLT, (char_u *)0L}
			    SCTX_INIT},
    {"endoffile",   "eof",  P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_eof, PV_EOF,
			    did_مجموعه_eof_eol_fixeol_bomb, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"endofline",   "eol",  P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_eol, PV_EOL,
			    did_مجموعه_eof_eol_fixeol_bomb, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"equalalways", "ea",   P_BOOL|P_VI_DEF|P_RALL,
			    (char_u *)&p_ea, PV_NONE, did_مجموعه_equalalways, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"equalprg",    "ep",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_ep, PV_EP, عدم, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"errorbells",  "eb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_eb, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"errorfile",   "ef",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر_تعریف_شده FEAT_QUICKFIX
			    (char_u *)&p_ef, PV_NONE, عدم, عدم,
			    {(char_u *)DFLT_ERRORFILE, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"errorformat", "efm",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شده FEAT_QUICKFIX
			    (char_u *)&p_efm, PV_EFM, عدم, عدم,
			    {(char_u *)DFLT_EFM, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"esckeys",	    "ek",   P_BOOL|P_VIM,
			    (char_u *)&p_ek, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"eventignore", "ei",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_ei, PV_NONE, did_مجموعه_eventignore, expand_مجموعه_eventignore,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"eventignorewin", "eiw",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)VAR_WIN, PV_EIW, did_مجموعه_eventignore, expand_مجموعه_eventignore,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"expandtab",   "et",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_et, PV_ET, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"exrc",	    "ex",   P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_exrc, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"fileencoding","fenc", P_STRING|P_ALLOCED|P_VI_DEF|P_RSTAT|P_RBUF
								   |P_NO_MKRC,
			    (char_u *)&p_fenc, PV_FENC, did_مجموعه_encoding, expand_مجموعه_encoding,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"fileencodings","fencs", P_STRING|P_VI_DEF|P_ONECOMMA,
			    (char_u *)&p_fencs, PV_NONE, عدم, expand_مجموعه_encoding,
			    {(char_u *)"ucs-bom", (char_u *)0L}
			    SCTX_INIT},
    {"fileformat",  "ff",   P_STRING|P_ALLOCED|P_VI_DEF|P_RSTAT|P_NO_MKRC
								  |P_CURSWANT,
			    (char_u *)&p_ff, PV_FF, did_مجموعه_fileformat, expand_مجموعه_fileformat,
			    {(char_u *)DFLT_FF, (char_u *)0L} SCTX_INIT},
    {"fileformats", "ffs",  P_STRING|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_ffs, PV_NONE, did_مجموعه_fileformats, expand_مجموعه_fileformat,
			    {(char_u *)DFLT_FFS_VI, (char_u *)DFLT_FFS_VIM}
			    SCTX_INIT},
    {"fileignorecase", "fic", P_BOOL|P_VI_DEF,
			    (char_u *)&p_fic, PV_NONE, عدم, عدم,
			    {
#اگر_تعریف_شده CASE_INSENSITIVE_FILENAME
				    (char_u *)TRUE,
#وإلا
				    (char_u *)FALSE,
#پایان_اگر
					(char_u *)0L} SCTX_INIT},
    {"filetype",    "ft",   P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB|P_NFNAME,
			    (char_u *)&p_ft, PV_FT,
			    did_مجموعه_filetype_or_syntax, عدم,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"fillchars",   "fcs",  P_STRING|P_VI_DEF|P_RALL|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_fcs, PV_FCS, did_مجموعه_chars_option, expand_مجموعه_chars_option,
			    {(char_u *)"vert:|,fold:-,eob:~,lastline:@",
								  (char_u *)0L}
			    SCTX_INIT},
    {"findfunc", "ffu",     P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#اگر_تعریف_شده FEAT_EVAL
			    (char_u *)&p_ffu, PV_FFU,
			    did_مجموعه_findfunc, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"fixendofline",  "fixeol", P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_fixeol, PV_FIXEOL,
			    did_مجموعه_eof_eol_fixeol_bomb, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"fkmap",	    "fk",   P_BOOL|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"flash",	    "fl",   P_BOOL|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"foldclose",   "fcl",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_RWIN,
#اگر_تعریف_شده FEAT_FOLDING
			    (char_u *)&p_fcl, PV_NONE, did_مجموعه_foldclose, expand_مجموعه_foldclose,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"foldcolumn",  "fdc",  P_رقم|P_VI_DEF|P_RWIN,
#اگر_تعریف_شده FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDC, did_مجموعه_foldcolumn, عدم,
			    {(char_u *)FALSE, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"foldenable",  "fen",  P_BOOL|P_VI_DEF|P_RWIN,
#اگر_تعریف_شده FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FEN, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"foldexpr",    "fde",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN|P_MLE,
#إذا defined(FEAT_FOLDING) && defined(FEAT_EVAL)
			    (char_u *)VAR_WIN, PV_FDE, did_مجموعه_foldexpr, عدم,
			    {(char_u *)"0", (char_u *)عدم}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"foldignore",  "fdi",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN,
#اگر_تعریف_شده FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDI, did_مجموعه_foldignore, عدم,
			    {(char_u *)"#", (char_u *)عدم}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"foldlevel",   "fdl",  P_رقم|P_VI_DEF|P_RWIN,
#اگر_تعریف_شده FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDL, did_مجموعه_foldlevel, عدم,
			    {(char_u *)0L, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"foldlevelstart","fdls", P_رقم|P_VI_DEF|P_CURSWANT,
#اگر_تعریف_شده FEAT_FOLDING
			    (char_u *)&p_fdls, PV_NONE, عدم, عدم,
			    {(char_u *)-1L, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"foldmarker",  "fmr",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|
						    P_RWIN|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شده FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FMR, did_مجموعه_foldmarker, عدم,
			    {(char_u *)"{{{,}}}", (char_u *)عدم}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"foldmethod",  "fdm",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN,
#اگر_تعریف_شده FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDM, did_مجموعه_foldmethod, expand_مجموعه_foldmethod,
			    {(char_u *)"manual", (char_u *)عدم}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"foldminlines","fml",  P_رقم|P_VI_DEF|P_RWIN,
#اگر_تعریف_شده FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FML, did_مجموعه_foldminlines, عدم,
			    {(char_u *)1L, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"foldnestmax", "fdn",  P_رقم|P_VI_DEF|P_RWIN,
#اگر_تعریف_شده FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDN, did_مجموعه_foldnestmax, عدم,
			    {(char_u *)20L, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"foldopen",    "fdo",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_CURSWANT,
#اگر_تعریف_شده FEAT_FOLDING
			    (char_u *)&p_fdo, PV_NONE, did_مجموعه_foldopen, expand_مجموعه_foldopen,
		 {(char_u *)"block,hor,mark,percent,quickfix,search,tag,undo",
						 (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"foldtext",    "fdt",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN|P_MLE,
#إذا defined(FEAT_FOLDING) && defined(FEAT_EVAL)
			    (char_u *)VAR_WIN, PV_FDT, did_مجموعه_optexpr, عدم,
			    {(char_u *)"foldtext()", (char_u *)عدم}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"formatexpr", "fex",   P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_MLE,
#اگر_تعریف_شده FEAT_EVAL
			    (char_u *)&p_fex, PV_FEX, did_مجموعه_optexpr, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"formatlistpat","flp", P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)&p_flp, PV_FLP, عدم, عدم,
			    {(char_u *)"^\\s*\\d\\+[\\]:.)}\\t ]\\s*",
						 (char_u *)0L} SCTX_INIT},
    {"formatoptions","fo",  P_STRING|P_ALLOCED|P_VIM|P_FLAGLIST,
			    (char_u *)&p_fo, PV_FO, did_مجموعه_formatoptions, expand_مجموعه_formatoptions,
			    {(char_u *)DFLT_FO_VI, (char_u *)DFLT_FO_VIM}
			    SCTX_INIT},
    {"formatprg",   "fp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_fp, PV_FP, عدم, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"fsync",       "fs",   P_BOOL|P_SECURE|P_VI_DEF,
#اگر_تعریف_شده HAVE_FSYNC
			    (char_u *)&p_fs, PV_FS, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"gdefault",    "gd",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_gd, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"graphic",	    "gr",   P_BOOL|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"grepformat",  "gfm",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شده FEAT_QUICKFIX
			    (char_u *)&p_gefm, PV_GEFM, عدم, عدم,
			    {(char_u *)DFLT_GREPFORMAT, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"grepprg",	    "gp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر_تعریف_شده FEAT_QUICKFIX
			    (char_u *)&p_gp, PV_GP, عدم, عدم,
			    {
# إذا defined(MSWIN)
			    // may be changed to "grep -n" in os_win32.c
			    (char_u *)"findstr /n",
# elif defined(UNIX)
			    // Add an extra file name so that grep will always
			    // insert a file name in the match line.
			    (char_u *)"grep -n $* /dev/null",
# elif defined(VMS)
			    (char_u *)"SEARCH/NUMBERS ",
# وإلا
			    (char_u *)"grep -n ",
# endif
			    (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"guicursor",    "gcr", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شده CURSOR_SHAPE
			    (char_u *)&p_guicursor, PV_NONE,
			    did_مجموعه_guicursor, عدم,
			    {
# ifdef FEAT_GUI
				(char_u *)"n-v-c:block-Cursor/lCursor,ve:ver35-Cursor,o:hor50-Cursor,i-ci:ver25-Cursor/lCursor,r-cr:hor20-Cursor/lCursor,sm:block-Cursor-blinkwait175-blinkoff150-blinkon175",
# وإلا	// Win32 console
				(char_u *)"n-v-c:block,o:hor50,i-ci:hor15,r-cr:hor30,sm:block",
# endif
				    (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"guifont",	    "gfn",  P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP
#إذا !defined(FEAT_GUI_GTK)
				|P_COLON
#پایان_اگر
				,
#اگر_تعریف_شده FEAT_GUI
			    (char_u *)&p_guifont, PV_NONE, did_مجموعه_guifont, expand_مجموعه_guifont,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"guifontset",  "gfs",  P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA,
#إذا defined(FEAT_GUI) && defined(FEAT_XFONTSET)
			    (char_u *)&p_guifontset, PV_NONE,
			    did_مجموعه_guifontset, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"guifontwide", "gfw",  P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP
#إذا !defined(FEAT_GUI_GTK)
				|P_COLON
#پایان_اگر
				,
#إذا defined(FEAT_GUI)
			    (char_u *)&p_guifontwide, PV_NONE,
			    did_مجموعه_guifontwide, expand_مجموعه_guifont,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},


    {"guiheadroom", "ghr",  P_رقم|P_VI_DEF,
#إذا (defined(FEAT_GUI_GTK) && !defined(USE_GTK4)) || defined(FEAT_GUI_X11)
			    (char_u *)&p_ghr, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)50L, (char_u *)0L} SCTX_INIT},

    {"guiligatures", "gli", P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP,
#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
			    (char_u *)&p_guiligatures, PV_NONE,
			    did_مجموعه_guiligatures, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},

    {"guioptions",  "go",   P_STRING|P_VI_DEF|P_RALL|P_FLAGLIST,
#إذا defined(FEAT_GUI)
			    (char_u *)&p_go, PV_NONE, did_مجموعه_guioptions, expand_مجموعه_guioptions,
# إذا defined(UNIX)
			    {(char_u *)"aegimrLtT", (char_u *)0L}
# وإلا
			    {(char_u *)"egmrLtT", (char_u *)0L}
# endif
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"guipty",	    عدم,   P_BOOL|P_VI_DEF,
#إذا defined(FEAT_GUI)
			    (char_u *)&p_guipty, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"guitablabel",  "gtl", P_STRING|P_VI_DEF|P_RWIN|P_MLE,
#إذا defined(FEAT_GUI_TABLINE)
			    (char_u *)&p_gtl, PV_NONE, did_مجموعه_guitablabel, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"guitabtooltip",  "gtt", P_STRING|P_VI_DEF|P_RWIN|P_MLE,
#إذا defined(FEAT_GUI_TABLINE)
			    (char_u *)&p_gtt, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"hardtabs",    "ht",   P_رقم|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"helpfile",    "hf",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_hf, PV_NONE, did_مجموعه_helpfile, عدم,
			    {(char_u *)DFLT_HELPFILE, (char_u *)0L}
			    SCTX_INIT},
    {"helpheight",  "hh",   P_رقم|P_VI_DEF,
			    (char_u *)&p_hh, PV_NONE,
			    did_مجموعه_winheight_helpheight, عدم,
			    {(char_u *)20L, (char_u *)0L} SCTX_INIT},
    {"helplang",    "hlg",  P_STRING|P_VI_DEF|P_ONECOMMA,
#اگر_تعریف_شده FEAT_MULTI_LANG
			    (char_u *)&p_hlg, PV_NONE, did_مجموعه_helplang, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"hidden",	    "hid",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_hid, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"highlight",   "hl",   P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_hl, PV_NONE, did_مجموعه_highlight, expand_مجموعه_highlight,
			    {(char_u *)HIGHLIGHT_INIT, (char_u *)0L}
			    SCTX_INIT},
    {"history",	    "hi",   P_رقم|P_VIM,
			    (char_u *)&p_hi, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)200L} SCTX_INIT},
    {"hkmap",	    "hk",   P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شده FEAT_RIGHTLEFT
			    (char_u *)&p_hkmap, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"hkmapp",	    "hkp",  P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شده FEAT_RIGHTLEFT
			    (char_u *)&p_hkmapp, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"hlsearch",    "hls",  P_BOOL|P_VI_DEF|P_VIM|P_RALL|P_HLONLY,
#إذا defined(FEAT_جستجو_EXTRA)
			    (char_u *)&p_hls, PV_NONE, did_مجموعه_hlsearch, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"icon",	    عدم,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_icon, PV_NONE, did_مجموعه_title_icon, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"iconstring",  عدم,   P_STRING|P_VI_DEF|P_MLE,
			    (char_u *)&p_iconstring, PV_NONE,
			    did_مجموعه_iconstring, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"ignorecase",  "ic",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ic, PV_NONE, did_مجموعه_ignorecase, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"imactivatefunc","imaf",P_STRING|P_VI_DEF|P_SECURE|P_FUNC,
#إذا defined(FEAT_EVAL) && \
	(defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
			    (char_u *)&p_imaf, PV_NONE,
			    did_مجموعه_imactivatefunc, عدم,
			    {(char_u *)"", (char_u *)عدم}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"imactivatekey","imak",P_STRING|P_VI_DEF,
#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
			    (char_u *)&p_imak, PV_NONE,
			    did_مجموعه_imactivatekey, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"imageprotocol", "ipc",P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_COLON,
#اگر_تعریف_شده FEAT_IMAGE
			    (char_u *)&p_ipc, PV_NONE, did_مجموعه_imageprotocol, expand_مجموعه_imageprotocol,
			    {(char_u *)"kitty:kitty,foot:sixel,ghostty:kitty,wezterm:kitty,xterm:sixel", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"imcmdline",   "imc",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_imcmdline, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"imdisable",   "imd",  P_BOOL|P_VI_DEF,
#إذا defined(HAVE_درون_داد_روش)
			    (char_u *)&p_imdisable, PV_NONE,
			    did_مجموعه_imdisable, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L}
			    SCTX_INIT},
    {"iminsert",    "imi",  P_رقم|P_VI_DEF,
			    (char_u *)&p_iminsert, PV_IMI, did_مجموعه_iminsert, عدم,
			    {(char_u *)B_IMODE_NONE, (char_u *)0L}
			    SCTX_INIT},
    {"imsearch",    "ims",  P_رقم|P_VI_DEF,
			    (char_u *)&p_imsearch, PV_IMS, did_مجموعه_imsearch, عدم,
			    {(char_u *)B_IMODE_USE_درج, (char_u *)0L}
			    SCTX_INIT},
    {"imstatusfunc","imsf",P_STRING|P_VI_DEF|P_SECURE|P_FUNC,
#إذا defined(FEAT_EVAL) && \
	(defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
			    (char_u *)&p_imsf, PV_NONE, did_مجموعه_imstatusfunc, عدم,
			    {(char_u *)"", (char_u *)عدم}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"imstyle",	    "imst", P_رقم|P_VI_DEF|P_SECURE,
#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
			    (char_u *)&p_imst, PV_NONE, did_مجموعه_imstyle, عدم,
			    {(char_u *)IM_OVER_THE_SPOT, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"include",	    "inc",  P_STRING|P_ALLOCED|P_VI_DEF,
#اگر_تعریف_شده FEAT_پیدا_کن_ID
			    (char_u *)&p_inc, PV_INC, عدم, عدم,
			    {(char_u *)"^\\s*#\\s*include", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"includeexpr", "inex", P_STRING|P_ALLOCED|P_VI_DEF|P_MLE,
#إذا defined(FEAT_پیدا_کن_ID) && defined(FEAT_EVAL)
			    (char_u *)&p_inex, PV_INEX, did_مجموعه_optexpr, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"incsearch",   "is",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_is, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"indentexpr", "inde",  P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_MLE,
#إذا defined(FEAT_EVAL)
			    (char_u *)&p_inde, PV_INDE, did_مجموعه_optexpr, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"indentkeys", "indk",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
#إذا defined(FEAT_EVAL)
			    (char_u *)&p_indk, PV_INDK, عدم, عدم,
			    {INDENTKEYS_DEFAULT, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"infercase",   "inf",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_inf, PV_INF, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"insertmode",  "im",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_im, PV_NONE, did_مجموعه_insertmode, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"isfname",	    "isf",  P_STRING|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_isf, PV_NONE, did_مجموعه_isopt, عدم,
			    {
#اگر_تعریف_شده BACKSLASH_درون_FILENAME
				// Excluded are: & and ^ are special in cmd.exe
				// ( and ) are used in text separating fnames
			    (char_u *)"@,48-57,/,\\,.,-,_,+,,,#,$,%,{,},[,],:,@-@,!,~,=",
#وإلا
# إذا defined(AMIGA)
			    (char_u *)"@,48-57,/,.,-,_,+,,,$,:",
# elif defined(VMS)
			    (char_u *)"@,48-57,/,.,-,_,+,,,#,$,%,<,>,[,],:,;,~",
# وإلا // UNIX et al.
			    (char_u *)"@,48-57,/,.,-,_,+,,,#,$,%,~,=",
# endif
#پایان_اگر
				(char_u *)0L} SCTX_INIT},
    {"isident",	    "isi",  P_STRING|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_isi, PV_NONE, did_مجموعه_isopt, عدم,
			    {
#إذا defined(MSWIN)
			    (char_u *)"@,48-57,_,128-167,224-235",
#وإلا
			    (char_u *)"@,48-57,_,192-255",
#پایان_اگر
				(char_u *)0L} SCTX_INIT},
    {"iskeyword",   "isk",  P_STRING|P_ALLOCED|P_VIM|P_COMMA|P_NODUP,
			    (char_u *)&p_isk, PV_ISK, did_مجموعه_iskeyword, عدم,
			    {
				(char_u *)"@,48-57,_",
#إذا defined(MSWIN)
				(char_u *)"@,48-57,_,128-167,224-235"
#وإلا
				ISK_LATIN1
#پایان_اگر
			    } SCTX_INIT},
    {"isprint",	    "isp",  P_STRING|P_VI_DEF|P_RALL|P_COMMA|P_NODUP,
			    (char_u *)&p_isp, PV_NONE, did_مجموعه_isopt, عدم,
			    {
#إذا defined(MSWIN) || defined(VMS)
			    (char_u *)"@,~-255",
#وإلا
			    ISP_LATIN1,
#پایان_اگر
				(char_u *)0L} SCTX_INIT},
    {"joinspaces",  "js",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_js, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"jumpoptions", "jop",  P_STRING|P_VI_DEF|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_jop, PV_NONE, did_مجموعه_jumpoptions, expand_مجموعه_jumpoptions,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"key",	    عدم,   P_STRING|P_ALLOCED|P_VI_DEF|P_NO_MKRC|P_NO_CMD_EXPAND,
#اگر_تعریف_شده FEAT_CRYPT
			    (char_u *)&p_کلید, PV_کلید, did_مجموعه_cryptkey, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"keymap",	    "kmp",  P_STRING|P_ALLOCED|P_VI_DEF|P_RBUF|P_RSTAT|P_NFNAME|P_PRI_MKRC,
#اگر_تعریف_شده FEAT_KEYMAP
			    (char_u *)&p_keymap, PV_KMAP, did_مجموعه_keymap, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"keymodel",    "km",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_km, PV_NONE, did_مجموعه_keymodel, expand_مجموعه_keymodel,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"keyprotocol", "kpc",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_kpc, PV_NONE, did_مجموعه_keyprotocol, expand_مجموعه_keyprotocol,
			    {(char_u *)"kitty:kitty,foot:kitty,ghostty:kitty,wezterm:kitty,xterm:mok2", (char_u *)0L}
			    SCTX_INIT},
    {"keywordprg",  "kp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_kp, PV_KP, عدم, عدم,
			    {
#إذا defined(MSWIN)
			    (char_u *)":help",
#وإلاإذا defined(VMS)
			    (char_u *)"help",
#وإلاإذا defined(USEMAN_S)
			    (char_u *)"man -s",
#وإلا
			    (char_u *)"man",
#پایان_اگر
				(char_u *)0L} SCTX_INIT},
    {"langmap",     "lmap", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_SECURE,
#اگر_تعریف_شده FEAT_LANGMAP
			    (char_u *)&p_langmap, PV_NONE, did_مجموعه_langmap, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"langmenu",    "lm",   P_STRING|P_VI_DEF|P_NFNAME,
#إذا defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
			    (char_u *)&p_lm, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"langnoremap",  "lnr",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شده FEAT_LANGMAP
			    (char_u *)&p_lnr, PV_NONE, did_مجموعه_langnoremap, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"langremap",  "lrm",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شده FEAT_LANGMAP
			    (char_u *)&p_lrm, PV_NONE, did_مجموعه_langremap, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"laststatus",  "ls",   P_رقم|P_VI_DEF|P_RALL,
			    (char_u *)&p_ls, PV_NONE, did_مجموعه_laststatus, عدم,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"lazyredraw",  "lz",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_lz, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"lhistory", "lhi",	    P_رقم|P_VI_DEF,
#اگر_تعریف_شده FEAT_QUICKFIX
			    (char_u *)VAR_WIN, PV_LHI, did_مجموعه_xhistory, عدم,
			    {(char_u *)10L, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"linebreak",   "lbr",  P_BOOL|P_VI_DEF|P_RWIN,
#اگر_تعریف_شده FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_LBR, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"lines",	    عدم,   P_رقم|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RCLR,
			    (char_u *)&ردیفها, PV_NONE, عدم, عدم,
			    {
#إذا defined(MSWIN)
			    (char_u *)25L,
#وإلا
			    (char_u *)24L,
#پایان_اگر
					    (char_u *)0L} SCTX_INIT},
    {"linespace",   "lsp",  P_رقم|P_VI_DEF|P_RCLR,
#اگر_تعریف_شده FEAT_GUI
			    (char_u *)&p_linespace, PV_NONE,
			    did_مجموعه_linespace, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI_MSWIN
			    {(char_u *)1L, (char_u *)0L}
#وإلا
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"lisp",	    عدم,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_lisp, PV_LISP, did_مجموعه_lisp, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"lispoptions", "lop",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_lop, PV_LOP, did_مجموعه_lispoptions, expand_مجموعه_lispoptions,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"lispwords",   "lw",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_lispwords, PV_LW, عدم, عدم,
			    {(char_u *)LISPWORD_مقدار, (char_u *)0L} SCTX_INIT},
    {"list",	    عدم,   P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_LIST, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"listchars",   "lcs",  P_STRING|P_VI_DEF|P_RALL|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_lcs, PV_LCS, did_مجموعه_chars_option, expand_مجموعه_chars_option,
			    {(char_u *)"eol:$", (char_u *)0L} SCTX_INIT},
    {"loadplugins", "lpl",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_lpl, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"luadll",      عدم,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#إذا defined(DYNAMIC_LUA)
			    (char_u *)&p_luadll, PV_NONE, عدم, عدم,
			    {(char_u *)DYNAMIC_LUA_DLL, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"macatsui",    عدم,   P_BOOL|P_VI_DEF|P_RCLR,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"magic",	    عدم,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_magic, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"makeef",	    "mef",  P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر_تعریف_شده FEAT_QUICKFIX
			    (char_u *)&p_mef, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"makeencoding","menc", P_STRING|P_VI_DEF,
			    (char_u *)&p_menc, PV_MENC, did_مجموعه_encoding, expand_مجموعه_encoding,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"makeprg",	    "mp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر_تعریف_شده FEAT_QUICKFIX
			    (char_u *)&p_mp, PV_MP, عدم, عدم,
# ifdef VMS
			    {(char_u *)"MMS", (char_u *)0L}
# وإلا
			    {(char_u *)"make", (char_u *)0L}
# endif
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"matchpairs",  "mps",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_mps, PV_MPS, did_مجموعه_matchpairs, عدم,
			    {(char_u *)"(:),{:},[:]", (char_u *)0L}
			    SCTX_INIT},
    {"matchtime",   "mat",  P_رقم|P_VI_DEF,
			    (char_u *)&p_mat, PV_NONE, عدم, عدم,
			    {(char_u *)5L, (char_u *)0L} SCTX_INIT},
    {"maxcombine",  "mco",  P_رقم|P_VI_DEF|P_CURSWANT,
			    (char_u *)&p_mco, PV_NONE, did_مجموعه_maxcombine, عدم,
			    {(char_u *)2, (char_u *)0L} SCTX_INIT},
    {"maxfuncdepth", "mfd", P_رقم|P_VI_DEF,
#اگر_تعریف_شده FEAT_EVAL
			    (char_u *)&p_mfd, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)100L, (char_u *)0L} SCTX_INIT},
    {"maxmapdepth", "mmd",  P_رقم|P_VI_DEF,
			    (char_u *)&p_mmd, PV_NONE, عدم, عدم,
			    {(char_u *)1000L, (char_u *)0L} SCTX_INIT},
    {"maxmem",	    "mm",   P_رقم|P_VI_DEF,
			    (char_u *)&p_mm, PV_NONE, عدم, عدم,
			    {(char_u *)DFLT_MAXMEM, (char_u *)0L}
			    SCTX_INIT},
    {"maxmempattern","mmp", P_رقم|P_VI_DEF,
			    (char_u *)&p_mmp, PV_NONE, عدم, عدم,
			    {(char_u *)1000L, (char_u *)0L} SCTX_INIT},
    {"maxmemtot",   "mmt",  P_رقم|P_VI_DEF,
			    (char_u *)&p_mmt, PV_NONE, عدم, عدم,
			    {(char_u *)DFLT_MAXMEMTOT, (char_u *)0L}
			    SCTX_INIT},
    {"maxsearchcount", "msc", P_رقم|P_VI_DEF,
			    (char_u *)&p_msc, PV_NONE, did_مجموعه_maxsearchcount, عدم,
			    {(char_u *)99L, (char_u *)0L} SCTX_INIT},
    {"menuitems",   "mis",  P_رقم|P_VI_DEF,
#اگر_تعریف_شده FEAT_MENU
			    (char_u *)&p_mis, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)25L, (char_u *)0L} SCTX_INIT},
    {"mesg",	    عدم,   P_BOOL|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"messagesopt","mopt",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_COLON|P_NODUP,
			    (char_u *)&p_mopt, PV_NONE, did_مجموعه_messagesopt, expand_مجموعه_messagesopt,
			    {(char_u *)"hit-enter,history:500", (char_u *)عدم} SCTX_INIT},
    {"mkspellmem",  "msm",  P_STRING|P_VI_DEF|P_EXPAND|P_SECURE,
#اگر_تعریف_شده FEAT_SPELL
			    (char_u *)&p_msm, PV_NONE, did_مجموعه_mkspellmem, عدم,
			    {(char_u *)"460000,2000,500", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"modeline",    "ml",   P_BOOL|P_VIM,
			    (char_u *)&p_ml, PV_ML, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"modelineexpr", "mle",  P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_mle, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"modelines",   "mls",  P_رقم|P_VI_DEF,
			    (char_u *)&p_mls, PV_NONE, عدم, عدم,
			    {(char_u *)5L, (char_u *)0L} SCTX_INIT},
    {"modelinestrict", "mlst", P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_mlstr, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"modifiable",  "ma",   P_BOOL|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_ma, PV_MA, did_مجموعه_modifiable, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"modified",    "mod",  P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_mod, PV_MOD, did_مجموعه_modified, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"more",	    عدم,   P_BOOL|P_VIM,
			    (char_u *)&p_بیشتر, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"mouse",	    عدم,   P_STRING|P_VI_DEF|P_FLAGLIST,
			    (char_u *)&p_mouse, PV_NONE, did_مجموعه_mouse, expand_مجموعه_mouse,
			    {
#إذا defined(MSWIN)
				(char_u *)"a",
#وإلا
				(char_u *)"",
#پایان_اگر
				(char_u *)0L} SCTX_INIT},
    {"mousefocus",   "mousef", P_BOOL|P_VI_DEF,
#اگر_تعریف_شده FEAT_GUI
			    (char_u *)&p_mousef, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"mousehide",   "mh",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شده FEAT_GUI
			    (char_u *)&p_mh, PV_NONE, did_مجموعه_mousehide, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"mousemodel",  "mousem", P_STRING|P_VI_DEF,
			    (char_u *)&p_mousem, PV_NONE, did_مجموعه_mousemodel, expand_مجموعه_mousemodel,
			    {
#إذا defined(MSWIN)
				(char_u *)"popup",
#وإلا
# إذا defined(MACOS_X)
				(char_u *)"popup_setpos",
# وإلا
				(char_u *)"extend",
# endif
#پایان_اگر
				(char_u *)0L} SCTX_INIT},
    {"mousemoveevent",   "mousemev",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شده FEAT_GUI
			    (char_u *)&p_mousemev, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"mouseshape",  "mouses",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شده FEAT_MOUSESHAPE
			    (char_u *)&p_mouseshape, PV_NONE,
			    did_مجموعه_mouseshape, عدم,
			    {(char_u *)"i-r:beam,s:updown,sd:udsizing,vs:leftright,vd:lrsizing,m:no,ml:up-arrow,v:rightup-arrow", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"mousetime",   "mouset",	P_رقم|P_VI_DEF,
			    (char_u *)&p_mouset, PV_NONE, عدم, عدم,
			    {(char_u *)500L, (char_u *)0L} SCTX_INIT},
    {"mzquantum",  "mzq",   P_رقم,
#اگر_تعریف_شده FEAT_MZSCHEME
			    (char_u *)&p_mzq, PV_NONE, did_مجموعه_mzquantum, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)100L, (char_u *)100L} SCTX_INIT},
    {"mzschemedll", عدم,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#إذا defined(DYNAMIC_MZSCHEME)
			    (char_u *)&p_mzschemedll, PV_NONE, عدم, عدم,
			    {(char_u *)DYNAMIC_MZSCH_DLL, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"mzschemegcdll", عدم, P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#إذا defined(DYNAMIC_MZSCHEME)
			    (char_u *)&p_mzschemegcdll, PV_NONE, عدم, عدم,
			    {(char_u *)DYNAMIC_MZGC_DLL, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"novice",	    عدم,   P_BOOL|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"nrformats",   "nf",   P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_nf, PV_NF, did_مجموعه_nrformats, expand_مجموعه_nrformats,
			    {(char_u *)"bin,octal,hex", (char_u *)0L}
			    SCTX_INIT},
    {"number",	    "nu",   P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_NU,
			    did_مجموعه_عدد_relativenumber, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"numberwidth", "nuw",  P_رقم|P_RWIN|P_VIM,
#اگر_تعریف_شده FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_NUW, did_مجموعه_numberwidth, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)8L, (char_u *)4L} SCTX_INIT},
    {"omnifunc",    "ofu",  P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#اگر_تعریف_شده FEAT_COMPL_FUNC
			    (char_u *)&p_ofu, PV_OFU, did_مجموعه_omnifunc, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"open",	    عدم,   P_BOOL|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"opendevice",  "odev", P_BOOL|P_VI_DEF,
#إذا defined(MSWIN)
			    (char_u *)&p_odev, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)FALSE}
			    SCTX_INIT},
    {"operatorfunc", "opfunc", P_STRING|P_VI_DEF|P_SECURE|P_FUNC,
			    (char_u *)&p_opfunc, PV_NONE,
			    did_مجموعه_operatorfunc, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"optimize",    "opt",  P_BOOL|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"osctimeoutlen", "ost", P_رقم|P_VI_DEF,
			    (char_u *)&p_ost, PV_NONE, did_مجموعه_osctimeoutlen, عدم,
			    {(char_u *)1000, (char_u *)0L} SCTX_INIT},
    {"osfiletype",  "oft",  P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"packpath",    "pp",   P_STRING|P_VI_DEF|P_EXPAND|P_ONECOMMA|P_NODUP
								    |P_SECURE,
			    (char_u *)&p_pp, PV_NONE, عدم, عدم,
			    {(char_u *)DFLT_RUNTIMEPATH, (char_u *)0L}
			    SCTX_INIT},
    {"paragraphs",  "para", P_STRING|P_VI_DEF,
			    (char_u *)&p_para, PV_NONE, عدم, عدم,
			    {(char_u *)"IPLPPPQPP TPHPLIPpLpItpplpipbp",
				(char_u *)0L} SCTX_INIT},
    {"paste",	    عدم,   P_BOOL|P_VI_DEF|P_PRI_MKRC,
			    (char_u *)&p_paste, PV_NONE, did_مجموعه_paste, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"pastetoggle", "pt",   P_STRING|P_VI_DEF,
			    (char_u *)&p_pt, PV_NONE, did_مجموعه_pastetoggle, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"patchexpr",   "pex",  P_STRING|P_VI_DEF|P_SECURE,
#إذا defined(FEAT_DIFF) && defined(FEAT_EVAL)
			    (char_u *)&p_pex, PV_NONE, did_مجموعه_optexpr, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"patchmode",   "pm",   P_STRING|P_VI_DEF|P_NFNAME,
			    (char_u *)&p_pm, PV_NONE,
			    did_مجموعه_backupext_or_patchmode, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"path",	    "pa",   P_STRING|P_EXPAND|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_مسیر, PV_مسیر, عدم, عدم,
			    {
#إذا defined(AMIGA) || defined(MSWIN)
			    (char_u *)".,,",
#وإلا
			    (char_u *)".,/usr/include,,",
#پایان_اگر
				(char_u *)0L} SCTX_INIT},
    {"perldll",     عدم,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#إذا defined(DYNAMIC_PERL)
			    (char_u *)&p_perldll, PV_NONE, عدم, عدم,
			    {(char_u *)DYNAMIC_PERL_DLL, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"preserveindent", "pi", P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_pi, PV_PI, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"previewheight", "pvh", P_رقم|P_VI_DEF,
#إذا defined(FEAT_QUICKFIX)
			    (char_u *)&p_pvh, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)12L, (char_u *)0L} SCTX_INIT},
    {"previewpopup", "pvp", P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON,
#اگر_تعریف_شده FEAT_PROP_POPUP
			    (char_u *)&p_pvp, PV_NONE, did_مجموعه_previewpopup, expand_مجموعه_previewpopup,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"previewwindow", "pvw", P_BOOL|P_VI_DEF|P_RSTAT|P_NOGLOB,
#إذا defined(FEAT_QUICKFIX)
			    (char_u *)VAR_WIN, PV_PVW, did_مجموعه_previewwindow, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"printdevice", "pdev", P_STRING|P_VI_DEF|P_SECURE,
#اگر_تعریف_شده FEAT_PRINTER
			    (char_u *)&p_pdev, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"printencoding", "penc", P_STRING|P_VI_DEF,
#اگر_تعریف_شده FEAT_POSTSCRIPT
			    (char_u *)&p_penc, PV_NONE,
			    did_مجموعه_printencoding, expand_مجموعه_encoding,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"printexpr", "pexpr",  P_STRING|P_VI_DEF|P_SECURE,
#إذا defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
			    (char_u *)&p_pexpr, PV_NONE, did_مجموعه_optexpr, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"printfont", "pfn",    P_STRING|P_VI_DEF,
#اگر_تعریف_شده FEAT_PRINTER
			    (char_u *)&p_pfn, PV_NONE, عدم, عدم,
			    {
# ifdef MSWIN
				(char_u *)"Courier_New:h10",
# elif defined(FEAT_PRINT_PANGO)
				(char_u *)"Courier 10",
# وإلا
				(char_u *)"courier",
# endif
				(char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"printheader", "pheader",  P_STRING|P_VI_DEF|P_GETTEXT|P_MLE,
#اگر_تعریف_شده FEAT_PRINTER
			    (char_u *)&p_سرآیند, PV_NONE, عدم, عدم,
			    // untranslated to avoid problems when 'encoding'
			    // is changed
			    {(char_u *)"%<%f%h%m%=Page %N", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"printmbcharset", "pmbcs",  P_STRING|P_VI_DEF,
#إذا defined(FEAT_POSTSCRIPT)
			    (char_u *)&p_pmcs, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"printmbfont", "pmbfn",  P_STRING|P_VI_DEF,
#إذا defined(FEAT_POSTSCRIPT)
			    (char_u *)&p_pmfn, PV_NONE, parse_printmbfont, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"printoptions", "popt", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شده FEAT_PRINTER
			    (char_u *)&p_popt, PV_NONE, parse_printoptions, expand_مجموعه_printoptions,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"prompt",	    عدم,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_prompt, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"pumborder",   "pb",   P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON|P_SECURE,
			    (char_u *)&p_pb, PV_NONE, did_مجموعه_pumborder, expand_مجموعه_pumborder,
			    {(char_u *)"", (char_u *)عدم} SCTX_INIT},
    {"pumheight",   "ph",   P_رقم|P_VI_DEF,
			    (char_u *)&p_ph, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"pummaxwidth", "pmw",   P_رقم|P_VI_DEF,
			    (char_u *)&p_pmw, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"pumopt",	    عدم,   P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON,
			    (char_u *)&p_pumopt, PV_NONE,
			    did_مجموعه_pumopt, expand_مجموعه_pumopt,
			    {(char_u *)"", (char_u *)عدم} SCTX_INIT},
    {"pumwidth",    "pw",   P_رقم|P_VI_DEF,
			    (char_u *)&p_pw, PV_NONE, عدم, عدم,
			    {(char_u *)15L, (char_u *)15L} SCTX_INIT},
    {"pythondll",   عدم,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#إذا defined(DYNAMIC_PYTHON)
			    (char_u *)&p_pydll, PV_NONE, عدم, عدم,
			    {(char_u *)DYNAMIC_PYTHON_DLL, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"pythonhome",  عدم,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#إذا defined(FEAT_PYTHON)
			    (char_u *)&p_pyhome, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"pythonthreedll",  عدم,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#إذا defined(DYNAMIC_PYTHON3)
			    (char_u *)&p_py3dll, PV_NONE, عدم, عدم,
			    {(char_u *)DYNAMIC_PYTHON3_DLL, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"pythonthreehome", عدم,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#إذا defined(FEAT_PYTHON3)
			    (char_u *)&p_py3home, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"pyxversion", "pyx",   P_رقم|P_VI_DEF|P_SECURE,
#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
			    (char_u *)&p_pyx, PV_NONE, did_مجموعه_pyxversion, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)DEFAULT_PYTHON_VER, (char_u *)0L}
			    SCTX_INIT},
    {"quickfixtextfunc", "qftf", P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_SECURE|P_FUNC,
#إذا defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
			    (char_u *)&p_qftf, PV_NONE,
			    did_مجموعه_quickfixtextfunc, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)عدم}
#پایان_اگر
			    SCTX_INIT},
    {"quoteescape", "qe",   P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)&p_qe, PV_QE, عدم, عدم,
			    {(char_u *)"\\", (char_u *)0L} SCTX_INIT},
    {"readonly",    "ro",   P_BOOL|P_VI_DEF|P_RSTAT|P_NOGLOB,
			    (char_u *)&p_ro, PV_RO, did_مجموعه_readonly, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"redraw",	    عدم,   P_BOOL|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"redrawtime",  "rdt",  P_رقم|P_VI_DEF,
#اگر_تعریف_شده FEAT_RELTIME
			    (char_u *)&p_rdt, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)2000L, (char_u *)0L} SCTX_INIT},
    {"regexpengine", "re",  P_رقم|P_VI_DEF,
			    (char_u *)&p_re, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"relativenumber", "rnu", P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_RNU,
			    did_مجموعه_عدد_relativenumber, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"remap",	    عدم,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_remap, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"renderoptions", "rop", P_STRING|P_ONECOMMA|P_RCLR|P_VI_DEF,
#اگر_تعریف_شده FEAT_RENDER_OPTIONS
			    (char_u *)&p_rop, PV_NONE, did_مجموعه_renderoptions, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"report",	    عدم,   P_رقم|P_VI_DEF,
			    (char_u *)&p_report, PV_NONE, عدم, عدم,
			    {(char_u *)2L, (char_u *)0L} SCTX_INIT},
    {"restorescreen", "rs", P_BOOL|P_VI_DEF,
#اگر_تعریف_شده MSWIN
			    (char_u *)&p_rs, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"revins",	    "ri",   P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شده FEAT_RIGHTLEFT
			    (char_u *)&p_ri, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"rightleft",   "rl",   P_BOOL|P_VI_DEF|P_RWIN,
#اگر_تعریف_شده FEAT_RIGHTLEFT
			    (char_u *)VAR_WIN, PV_RL, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"rightleftcmd", "rlc", P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN,
#اگر_تعریف_شده FEAT_RIGHTLEFT
			    (char_u *)VAR_WIN, PV_RLC, did_مجموعه_rightleftcmd, expand_مجموعه_rightleftcmd,
			    {(char_u *)"search", (char_u *)عدم}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"rubydll",     عدم,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#إذا defined(DYNAMIC_RUBY)
			    (char_u *)&p_rubydll, PV_NONE, عدم, عدم,
			    {(char_u *)DYNAMIC_RUBY_DLL, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"ruler",	    "ru",   P_BOOL|P_VIM|P_RSTAT,
			    (char_u *)&p_ru, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"rulerformat", "ruf",  P_STRING|P_VI_DEF|P_ALLOCED|P_RSTAT|P_MLE,
#اگر_تعریف_شده FEAT_STL_OPT
			    (char_u *)&p_ruf, PV_NONE, did_مجموعه_rulerformat, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"runtimepath", "rtp",  P_STRING|P_VI_DEF|P_EXPAND|P_ONECOMMA|P_NODUP
								    |P_SECURE,
			    (char_u *)&p_rtp, PV_NONE, عدم, عدم,
			    {(char_u *)DFLT_RUNTIMEPATH, (char_u *)0L}
			    SCTX_INIT},
    {"scroll",	    "scr",  P_رقم|P_NO_MKRC|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_SCROLL, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"scrollbind",  "scb",  P_BOOL|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_SCBIND, did_مجموعه_scrollbind, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"scrollfocus", "scf",  P_BOOL|P_VI_DEF,
#إذا defined(MSWIN) && defined(FEAT_GUI)
			    (char_u *)&p_scf, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"scrolljump",  "sj",   P_رقم|P_VI_DEF|P_VIM,
			    (char_u *)&p_sj, PV_NONE, عدم, عدم,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"scrolloff",   "so",   P_رقم|P_VI_DEF|P_VIM|P_RALL,
			    (char_u *)&p_so, PV_SO, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"scrolloffpad", "sop", P_رقم|P_VI_DEF|P_VIM|P_RALL,
				(char_u *)&p_sop, PV_SOP, عدم, عدم,
				{(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"scrollopt",   "sbo",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_sbo, PV_NONE, did_مجموعه_scrollopt, expand_مجموعه_scrollopt,
			    {(char_u *)"ver,jump", (char_u *)0L}
			    SCTX_INIT},
    {"sections",    "sect", P_STRING|P_VI_DEF,
			    (char_u *)&p_sections, PV_NONE, عدم, عدم,
			    {(char_u *)"SHNHH HUnhsh", (char_u *)0L}
			    SCTX_INIT},
    {"secure",	    عدم,   P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_secure, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"selection",   "sel",  P_STRING|P_VI_DEF,
			    (char_u *)&p_sel, PV_NONE, did_مجموعه_selection, expand_مجموعه_selection,
			    {(char_u *)"inclusive", (char_u *)0L}
			    SCTX_INIT},
    {"selectmode",  "slm",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_slm, PV_NONE, did_مجموعه_selectmode, expand_مجموعه_selectmode,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"sessionoptions", "ssop", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شده FEAT_SESSION
			    (char_u *)&p_ssop, PV_NONE,
			    did_مجموعه_sessionoptions, expand_مجموعه_sessionoptions,
	 {(char_u *)"blank,buffers,curdir,folds,help,options,tabpages,winsize,terminal",
							       (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"shell",	    "sh",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_sh, PV_NONE, عدم, عدم,
			    {
#اگر_تعریف_شده VMS
			    (char_u *)"-",
#وإلا
# إذا defined(MSWIN)
			    (char_u *)"",	// set in set_init_1()
# وإلا
			    (char_u *)"sh",
# endif
#پایان_اگر // VMS
				(char_u *)0L} SCTX_INIT},
    {"shellcmdflag","shcf", P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_shcf, PV_NONE, عدم, عدم,
			    {
#إذا defined(MSWIN)
			    (char_u *)"/c",
#وإلا
			    (char_u *)"-c",
#پایان_اگر
				(char_u *)0L} SCTX_INIT},
    {"shellpipe",   "sp",   P_STRING|P_VI_DEF|P_SECURE,
#اگر_تعریف_شده FEAT_QUICKFIX
			    (char_u *)&p_sp, PV_NONE, did_مجموعه_shellpipe_redir, عدم,
			    {
# إذا defined(UNIX)
			    (char_u *)"| tee",
# وإلا
			    (char_u *)">",
# endif
				(char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"shellquote",  "shq",  P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_shq, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"shellredir",  "srr",  P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_srr, PV_NONE, did_مجموعه_shellpipe_redir, عدم,
			    {(char_u *)">", (char_u *)0L} SCTX_INIT},
    {"shellslash",  "ssl",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شده BACKSLASH_درون_FILENAME
			    (char_u *)&p_ssl, PV_NONE, did_مجموعه_shellslash, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"shelltemp",   "stmp", P_BOOL,
			    (char_u *)&p_stmp, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"shelltype",   "st",   P_رقم|P_VI_DEF,
#اگر_تعریف_شده AMIGA
			    (char_u *)&p_st, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"shellxescape", "sxe", P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_sxe, PV_NONE, عدم, عدم,
			    {
#إذا defined(MSWIN)
			    (char_u *)"\"&|<>()@^",
#وإلا
			    (char_u *)"",
#پایان_اگر
				(char_u *)0L} SCTX_INIT},
    {"shellxquote", "sxq",  P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_sxq, PV_NONE, عدم, عدم,
			    {
#إذا defined(UNIX) && defined(USE_SYSTEM)
			    (char_u *)"\"",
#وإلا
			    (char_u *)"",
#پایان_اگر
				(char_u *)0L} SCTX_INIT},
    {"shiftround",  "sr",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sr, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"shiftwidth",  "sw",   P_رقم|P_VI_DEF,
			    (char_u *)&p_sw, PV_SW,
			    did_مجموعه_shiftwidth_tabstop, عدم,
			    {(char_u *)8L, (char_u *)0L} SCTX_INIT},
    {"shortmess",   "shm",  P_STRING|P_VIM|P_FLAGLIST,
			    (char_u *)&p_shm, PV_NONE, did_مجموعه_shortmess, expand_مجموعه_shortmess,
			    {(char_u *)"S", (char_u *)"filnxtToOS"}
			    SCTX_INIT},
    {"shortname",   "sn",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sn, PV_SN, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"showbreak",   "sbr",  P_STRING|P_VI_DEF|P_RALL,
#اگر_تعریف_شده FEAT_LINEBREAK
			    (char_u *)&p_sbr, PV_SBR, did_مجموعه_showbreak, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"showcmd",	    "sc",   P_BOOL|P_VIM,
			    (char_u *)&p_sc, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"showcmdloc",  "sloc", P_STRING|P_RSTAT,
			    (char_u *)&p_sloc, PV_NONE, did_مجموعه_showcmdloc, expand_مجموعه_showcmdloc,
			    {(char_u *)"last", (char_u *)"last"} SCTX_INIT},
    {"showfulltag", "sft",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_sft, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"showmatch",   "sm",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sm, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"showmode",    "smd",  P_BOOL|P_VIM,
			    (char_u *)&p_smd, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"showtabline", "stal", P_رقم|P_VI_DEF|P_RALL,
			    (char_u *)&p_stal, PV_NONE, did_مجموعه_showtabline, عدم,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
#إذا defined(FEAT_TABPANEL)
    {"showtabpanel", "stpl", P_رقم|P_RALL,
			    (char_u *)&p_stpl, PV_NONE, did_مجموعه_showtabpanel, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
#پایان_اگر
    {"sidescroll",  "ss",   P_رقم|P_VI_DEF,
			    (char_u *)&p_ss, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"sidescrolloff", "siso", P_رقم|P_VI_DEF|P_VIM|P_RBUF,
			    (char_u *)&p_siso, PV_SISO, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"signcolumn",   "scl",  P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN,
#اگر_تعریف_شده FEAT_SIGNS
			    (char_u *)VAR_WIN, PV_SCL, did_مجموعه_signcolumn, expand_مجموعه_signcolumn,
			    {(char_u *)"auto", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"slowopen",    "slow", P_BOOL|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smartcase",   "scs",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_scs, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smartindent", "si",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_si, PV_SI, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smarttab",    "sta",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sta, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smoothscroll", "sms", P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_SMS, did_مجموعه_smoothscroll, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"softtabstop", "sts",  P_رقم|P_VI_DEF|P_VIM,
			    (char_u *)&p_sts, PV_STS, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"sourceany",   عدم,   P_BOOL|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"spell",	    عدم,   P_BOOL|P_VI_DEF|P_RWIN|P_HLONLY,
#اگر_تعریف_شده FEAT_SPELL
			    (char_u *)VAR_WIN, PV_SPELL, did_مجموعه_spell, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"spellcapcheck", "spc", P_STRING|P_ALLOCED|P_VI_DEF|P_RBUF|P_HLONLY,
#اگر_تعریف_شده FEAT_SPELL
			    (char_u *)&p_spc, PV_SPC, did_مجموعه_spellcapcheck, عدم,
			    {(char_u *)"[.?!]\\_[\\])'\"	 ]\\+", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"spellfile",   "spf",  P_STRING|P_EXPAND|P_ALLOCED|P_VI_DEF|P_SECURE
								  |P_ONECOMMA,
#اگر_تعریف_شده FEAT_SPELL
			    (char_u *)&p_spf, PV_SPF, did_مجموعه_spellfile, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"spelllang",   "spl",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA
						     |P_RBUF|P_HLONLY|P_EXPAND,
#اگر_تعریف_شده FEAT_SPELL
			    (char_u *)&p_spl, PV_SPL, did_مجموعه_spelllang, عدم,
			    {(char_u *)"en", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"spelloptions", "spo",  P_STRING|P_ALLOCED|P_VI_DEF
					   |P_ONECOMMA|P_NODUP|P_RBUF|P_HLONLY,
#اگر_تعریف_شده FEAT_SPELL
			    (char_u *)&p_spo, PV_SPO, did_مجموعه_spelloptions, expand_مجموعه_spelloptions,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"spellsuggest", "sps", P_STRING|P_VI_DEF|P_EXPAND|P_SECURE|P_ONECOMMA,
#اگر_تعریف_شده FEAT_SPELL
			    (char_u *)&p_sps, PV_NONE, did_مجموعه_spellsuggest, expand_مجموعه_spellsuggest,
			    {(char_u *)"best", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"splitbelow",  "sb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sb, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"splitkeep",   "spk",  P_STRING,
			    (char_u *)&p_spk, PV_NONE, did_مجموعه_splitkeep, expand_مجموعه_splitkeep,
			    {(char_u *)"cursor", (char_u *)"cursor"} SCTX_INIT},
    {"splitright",  "spr",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_spr, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"startofline", "sol",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sol, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"statusline"  ,"stl",  P_STRING|P_VI_DEF|P_ALLOCED|P_RSTAT|P_MLE,
#اگر_تعریف_شده FEAT_STL_OPT
			    (char_u *)&p_stl, PV_STL, did_مجموعه_statusline, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"statuslineopt"  ,"stlo",  P_STRING|P_VI_DEF|P_ALLOCED|P_RSTAT|P_MLE
			    |P_ONECOMMA|P_COLON|P_NODUP,
#اگر_تعریف_شده FEAT_STL_OPT
			    (char_u *)&p_stlo, PV_STLO, did_مجموعه_statuslineopt, expand_مجموعه_statuslineopt,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"suffixes",    "su",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_su, PV_NONE, عدم, عدم,
			    {(char_u *)".bak,~,.o,.h,.info,.swp,.obj",
				(char_u *)0L} SCTX_INIT},
    {"suffixesadd", "sua",  P_STRING|P_VI_DEF|P_ALLOCED|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_sua, PV_SUA, عدم, عدم,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"swapfile",    "swf",  P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_swf, PV_SWF, did_مجموعه_swapfile, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"swapsync",    "sws",  P_STRING|P_VI_DEF,
			    (char_u *)&p_sws, PV_NONE, did_مجموعه_swapsync, expand_مجموعه_swapsync,
			    {(char_u *)"fsync", (char_u *)0L} SCTX_INIT},
    {"switchbuf",   "swb",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_swb, PV_NONE, did_مجموعه_switchbuf, expand_مجموعه_switchbuf,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"synmaxcol",   "smc",  P_رقم|P_VI_DEF|P_RBUF,
#اگر_تعریف_شده FEAT_SYN_HL
			    (char_u *)&p_smc, PV_SMC, عدم, عدم,
			    {(char_u *)3000L, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"syntax",	    "syn",  P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB|P_NFNAME,
#اگر_تعریف_شده FEAT_SYN_HL
			    (char_u *)&p_syn, PV_SYN,
			    did_مجموعه_filetype_or_syntax, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"tabclose",    "tcl",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_tcl, PV_NONE, did_مجموعه_tabclose, expand_مجموعه_tabclose,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"tabline",	    "tal",  P_STRING|P_VI_DEF|P_RALL|P_MLE,
#اگر_تعریف_شده FEAT_STL_OPT
			    (char_u *)&p_tal, PV_NONE, did_مجموعه_tabline, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"tabpagemax",  "tpm",  P_رقم|P_VI_DEF,
			    (char_u *)&p_tpm, PV_NONE, عدم, عدم,
			    {(char_u *)10L, (char_u *)0L} SCTX_INIT},
#إذا defined(FEAT_TABPANEL)
    {"tabpanel",  "tpl",    P_STRING|P_VI_DEF|P_RALL|P_MLE,
			    (char_u *)&p_tpl, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"tabpanelopt","tplo",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_COLON
								    |P_NODUP,
			    (char_u *)&p_tplo, PV_NONE, did_مجموعه_tabpanelopt,
			    expand_مجموعه_tabpanelopt,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
#پایان_اگر
    {"tabstop",	    "ts",   P_رقم|P_VI_DEF|P_RBUF,
			    (char_u *)&p_ts, PV_TS,
			    did_مجموعه_shiftwidth_tabstop, عدم,
			    {(char_u *)8L, (char_u *)0L} SCTX_INIT},
    {"tagbsearch",  "tbs",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_tbs, PV_NONE, عدم, عدم,
#اگر_تعریف_شده VMS	// binary searching doesn't appear to work on VMS
			    {(char_u *)0L, (char_u *)0L}
#وإلا
			    {(char_u *)TRUE, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"tagcase",	    "tc",   P_STRING|P_VIM,
			    (char_u *)&p_tc, PV_TC, did_مجموعه_tagcase, expand_مجموعه_tagcase,
			    {(char_u *)"followic", (char_u *)"followic"} SCTX_INIT},
    {"tagfunc",    "tfu",   P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#اگر_تعریف_شده FEAT_EVAL
			    (char_u *)&p_tfu, PV_TFU, did_مجموعه_tagfunc, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"taglength",   "tl",   P_رقم|P_VI_DEF,
			    (char_u *)&p_tl, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"tagrelative", "tr",   P_BOOL|P_VIM,
			    (char_u *)&p_tr, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"tags",	    "tag",  P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_برچسبها, PV_برچسبها, عدم, عدم,
			    {
#إذا defined(FEAT_EMACS_برچسبها) && !defined(CASE_INSENSITIVE_FILENAME)
			    (char_u *)"./tags,./TAGS,tags,TAGS",
#وإلا
			    (char_u *)"./tags,tags",
#پایان_اگر
				(char_u *)0L} SCTX_INIT},
    {"tagsecure",   "tsc",   P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_tagsecure, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)TRUE} SCTX_INIT},
    {"tagstack",    "tgst", P_BOOL|P_VI_DEF,
			    (char_u *)&p_tgst, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"tcldll",      عدم,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#إذا defined(DYNAMIC_TCL)
			    (char_u *)&p_tcldll, PV_NONE, عدم, عدم,
			    {(char_u *)DYNAMIC_TCL_DLL, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"term",	    عدم,   P_STRING|P_EXPAND|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RALL,
			    (char_u *)&T_NAME, PV_NONE, did_مجموعه_جمله, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"termbidi", "tbidi",   P_BOOL|P_VI_DEF,
#اگر_تعریف_شده FEAT_ARABIC
			    (char_u *)&p_tbidi, PV_NONE, عدم, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"termencoding", "tenc", P_STRING|P_VI_DEF|P_RCLR,
			    (char_u *)&p_tenc, PV_NONE, did_مجموعه_encoding, expand_مجموعه_encoding,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"termguicolors", "tgc", P_BOOL|P_VI_DEF|P_VIM|P_RCLR,
#اگر_تعریف_شده FEAT_TERMGUICOLORS
			    (char_u *)&p_tgc, PV_NONE, did_مجموعه_termguicolors, عدم,
			    {(char_u *)FALSE, (char_u *)FALSE}
#وإلا
			    (char_u*)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)FALSE}
#پایان_اگر
			    SCTX_INIT},
    {"termresize", "trz", P_STRING|P_VI_DEF,
#اگر_تعریف_شده UNIX
			    (char_u *)&p_trz, PV_NONE, did_مجموعه_termresize, expand_مجموعه_termresize,
			    {(char_u *)"", (char_u *)0}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"termsync", "tsy",	    P_BOOL|P_VI_DEF,
			    (char_u *)&p_tsy, PV_NONE, did_مجموعه_termsync, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"termwinkey", "twk",   P_STRING|P_ALLOCED|P_RWIN|P_VI_DEF,
#اگر_تعریف_شده FEAT_TERMINAL
			    (char_u *)VAR_WIN, PV_TWK, did_مجموعه_termwinkey, عدم,
			    {(char_u *)"", (char_u *)عدم}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"termwinscroll", "twsl", P_رقم|P_VI_DEF|P_VIM|P_RBUF,
#اگر_تعریف_شده FEAT_TERMINAL
			    (char_u *)&p_twsl, PV_TWSL, did_مجموعه_termwinscroll, عدم,
			    {(char_u *)10000L, (char_u *)10000L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"termwinsize", "tws",  P_STRING|P_ALLOCED|P_RWIN|P_VI_DEF,
#اگر_تعریف_شده FEAT_TERMINAL
			    (char_u *)VAR_WIN, PV_TWS, did_مجموعه_termwinsize, عدم,
			    {(char_u *)"", (char_u *)عدم}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"termwintype", "twt",  P_STRING|P_ALLOCED|P_VI_DEF,
#إذا defined(MSWIN) && defined(FEAT_TERMINAL)
			    (char_u *)&p_twt, PV_NONE, did_مجموعه_termwintype, expand_مجموعه_termwintype,
			    {(char_u *)"", (char_u *)عدم}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"terse",	    عدم,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_terse, PV_NONE, did_مجموعه_terse, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"textauto",    "ta",   P_BOOL|P_VIM,
			    (char_u *)&p_ta, PV_NONE, did_مجموعه_textauto, عدم,
			    {(char_u *)DFLT_TEXTAUTO, (char_u *)TRUE}
			    SCTX_INIT},
    {"textmode",    "tx",   P_BOOL|P_VI_DEF|P_NO_MKRC,
			    (char_u *)&p_tx, PV_TX, did_مجموعه_textmode, عدم,
			    {
#اگر_تعریف_شده USE_CRNL
			    (char_u *)TRUE,
#وإلا
			    (char_u *)FALSE,
#پایان_اگر
				(char_u *)0L} SCTX_INIT},
    {"textwidth",   "tw",   P_رقم|P_VI_DEF|P_VIM|P_RBUF|P_HLONLY,
			    (char_u *)&p_tw, PV_TW, did_مجموعه_textwidth, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"thesaurus",   "tsr",  P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA|P_NODUP|P_NDNAME,
			    (char_u *)&p_tsr, PV_TSR, عدم, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"thesaurusfunc", "tsrfu",  P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#اگر_تعریف_شده FEAT_COMPL_FUNC
			    (char_u *)&p_tsrfu, PV_TSRFU,
			    did_مجموعه_thesaurusfunc, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"tildeop",	    "top",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_مقصد, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"timeout",	    "to",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_timeout, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"timeoutlen",  "tm",   P_رقم|P_VI_DEF,
			    (char_u *)&p_tm, PV_NONE, عدم, عدم,
			    {(char_u *)1000L, (char_u *)0L} SCTX_INIT},
    {"title",	    عدم,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_title, PV_NONE, did_مجموعه_title_icon, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"titlelen",    عدم,   P_رقم|P_VI_DEF,
			    (char_u *)&p_titlelen, PV_NONE, did_مجموعه_titlelen, عدم,
			    {(char_u *)85L, (char_u *)0L} SCTX_INIT},
    {"titleold",    عدم,   P_STRING|P_VI_DEF|P_GETTEXT|P_SECURE|P_NO_MKRC,
			    (char_u *)&p_titleold, PV_NONE, عدم, عدم,
			    {(char_u *)N_("Thanks for flying Vim"),
							       (char_u *)0L}
			    SCTX_INIT},
    {"titlestring", عدم,   P_STRING|P_VI_DEF|P_MLE,
			    (char_u *)&p_titlestring, PV_NONE,
			    did_مجموعه_titlestring, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"toolbar",     "tb",   P_STRING|P_ONECOMMA|P_VI_DEF|P_NODUP,
#إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
			    (char_u *)&p_toolbar, PV_NONE, did_مجموعه_toolbar, expand_مجموعه_toolbar,
			    {(char_u *)"icons,tooltips", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"toolbariconsize",	"tbis", P_STRING|P_VI_DEF,
#إذا defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
			    (char_u *)&p_tbis, PV_NONE,
			    did_مجموعه_toolbariconsize, expand_مجموعه_toolbariconsize,
			    {(char_u *)"small", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"ttimeout",    عدم,   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_ttimeout, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"ttimeoutlen", "ttm",  P_رقم|P_VI_DEF,
			    (char_u *)&p_ttm, PV_NONE, عدم, عدم,
			    {(char_u *)-1L, (char_u *)0L} SCTX_INIT},
    {"ttybuiltin",  "tbi",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_tbi, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"ttyfast",	    "tf",   P_BOOL|P_NO_MKRC|P_VI_DEF,
			    (char_u *)&p_tf, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"ttymouse",    "ttym", P_STRING|P_NODEFAULT|P_NO_MKRC|P_VI_DEF,
#إذا defined(UNIX) || defined(VMS)
			    (char_u *)&p_ttym, PV_NONE, did_مجموعه_ttymouse, expand_مجموعه_ttymouse,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"ttyscroll",   "tsl",  P_رقم|P_VI_DEF,
			    (char_u *)&p_ttyscroll, PV_NONE, عدم, عدم,
			    {(char_u *)999L, (char_u *)0L} SCTX_INIT},
    {"ttytype",	    "tty",  P_STRING|P_EXPAND|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RALL,
			    (char_u *)&T_NAME, PV_NONE, did_مجموعه_جمله, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"undodir",     "udir", P_STRING|P_EXPAND|P_ONECOMMA|P_NODUP|P_SECURE
								    |P_VI_DEF,
#اگر_تعریف_شده FEAT_PERSISTENT_UNDO
			    (char_u *)&p_udir, PV_NONE, عدم, عدم,
			    {(char_u *)".", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"undofile",    "udf",  P_BOOL|P_VI_DEF|P_VIM,
#اگر_تعریف_شده FEAT_PERSISTENT_UNDO
			    (char_u *)&p_udf, PV_UDF, did_مجموعه_undofile, عدم,
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
#پایان_اگر
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"undolevels",  "ul",   P_رقم|P_VI_DEF,
			    (char_u *)&p_ul, PV_UL, did_مجموعه_undolevels, عدم,
			    {
#إذا defined(UNIX) || defined(MSWIN) || defined(VMS)
			    (char_u *)1000L,
#وإلا
			    (char_u *)100L,
#پایان_اگر
				(char_u *)0L} SCTX_INIT},
    {"undoreload",  "ur",   P_رقم|P_VI_DEF,
			    (char_u *)&p_ur, PV_NONE, عدم, عدم,
			    { (char_u *)10000L, (char_u *)0L} SCTX_INIT},
    {"updatecount", "uc",   P_رقم|P_VI_DEF,
			    (char_u *)&p_uc, PV_NONE, did_مجموعه_updatecount, عدم,
			    {(char_u *)200L, (char_u *)0L} SCTX_INIT},
    {"updatetime",  "ut",   P_رقم|P_VI_DEF,
			    (char_u *)&p_ut, PV_NONE, عدم, عدم,
			    {(char_u *)4000L, (char_u *)0L} SCTX_INIT},
    {"varsofttabstop", "vsts",  P_STRING|P_VI_DEF|P_VIM|P_COMMA,
#اگر_تعریف_شده FEAT_VARTABS
			    (char_u *)&p_vsts, PV_VSTS,
			    did_مجموعه_varsofttabstop, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)عدم}
#پایان_اگر
			    SCTX_INIT},
    {"vartabstop",  "vts",  P_STRING|P_VI_DEF|P_VIM|P_RBUF|P_COMMA,
#اگر_تعریف_شده FEAT_VARTABS
			    (char_u *)&p_vts, PV_VTS, did_مجموعه_vartabstop, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)عدم}
#پایان_اگر
			    SCTX_INIT},
    {"verbose",	    "vbs",  P_رقم|P_VI_DEF,
			    (char_u *)&p_verbose, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"verbosefile", "vfile", P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_vfile, PV_NONE, did_مجموعه_verbosefile, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"viewdir",     "vdir", P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#اگر_تعریف_شده FEAT_SESSION
			    (char_u *)&p_vdir, PV_NONE, عدم, عدم,
			    {(char_u *)DFLT_VDIR, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"viewoptions", "vop",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#اگر_تعریف_شده FEAT_SESSION
			    (char_u *)&p_vop, PV_NONE, did_مجموعه_viewoptions, expand_مجموعه_sessionoptions,
			    {(char_u *)"folds,options,cursor,curdir",
								  (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"viminfo",	    "vi",   P_STRING|P_ONECOMMA|P_NODUP|P_SECURE,
#اگر_تعریف_شده FEAT_VIMINFO
			    (char_u *)&p_viminfo, PV_NONE, did_مجموعه_viminfo, عدم,
# إذا defined(MSWIN)
			    {(char_u *)"", (char_u *)"'100,<50,s10,h,rA:,rB:"}
# elif defined(AMIGA)
			    {(char_u *)"",
				 (char_u *)"'100,<50,s10,h,rdf0:,rdf1:,rdf2:"}
# وإلا
			    {(char_u *)"", (char_u *)"'100,<50,s10,h"}
# endif
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"viminfofile", "vif",  P_STRING|P_EXPAND|P_ONECOMMA|P_NODUP
							    |P_SECURE|P_VI_DEF,
#اگر_تعریف_شده FEAT_VIMINFO
			    (char_u *)&p_viminfofile, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"virtualedit", "ve",   P_STRING|P_ONECOMMA|P_NODUP|P_VI_DEF
							    |P_VIM|P_CURSWANT,
			    (char_u *)&p_ve, PV_VE, did_مجموعه_virtualedit, expand_مجموعه_virtualedit,
			    {(char_u *)"", (char_u *)""}
			    SCTX_INIT},
    {"visualbell",  "vb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_vb, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"w1200",	    عدم,   P_رقم|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"w300",	    عدم,   P_رقم|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"w9600",	    عدم,   P_رقم|P_VI_DEF,
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"warn",	    عدم,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_warn, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"weirdinvert", "wiv",  P_BOOL|P_VI_DEF|P_RCLR,
			    (char_u *)&p_wiv, PV_NONE, did_مجموعه_weirdinvert, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"whichwrap",   "ww",   P_STRING|P_VIM|P_ONECOMMA|P_FLAGLIST,
			    (char_u *)&p_ww, PV_NONE, did_مجموعه_whichwrap, expand_مجموعه_whichwrap,
			    {(char_u *)"", (char_u *)"b,s"} SCTX_INIT},
    {"wildchar",    "wc",   P_رقم|P_VIM,
			    (char_u *)&p_wc, PV_NONE, did_مجموعه_wildchar, عدم,
			    {(char_u *)(دراز)Ctrl_E, (char_u *)(دراز)TAB}
			    SCTX_INIT},
    {"wildcharm",   "wcm",  P_رقم|P_VI_DEF,
			    (char_u *)&p_wcm, PV_NONE, did_مجموعه_wildchar, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"wildignore",  "wig",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_wig, PV_NONE, عدم, عدم,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"wildignorecase", "wic", P_BOOL|P_VI_DEF,
			    (char_u *)&p_wic, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"wildmenu",    "wmnu", P_BOOL|P_VI_DEF,
			    (char_u *)&p_wmnu, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"wildmode",    "wim",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_wim, PV_NONE, did_مجموعه_wildmode, expand_مجموعه_wildmode,
			    {(char_u *)"full", (char_u *)0L} SCTX_INIT},
    {"wildoptions", "wop",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_wop, PV_NONE, did_مجموعه_wildoptions, expand_مجموعه_wildoptions,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"winaltkeys",  "wak",  P_STRING|P_VI_DEF,
#اگر_تعریف_شده FEAT_WAK
			    (char_u *)&p_wak, PV_NONE, did_مجموعه_winaltkeys, expand_مجموعه_winaltkeys,
			    {(char_u *)"menu", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"wincolor", "wcr",	    P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_WCR, did_مجموعه_wincolor, expand_مجموعه_wincolor,
			    {(char_u *)"", (char_u *)عدم}
			    SCTX_INIT},
    {"window",	    "wi",   P_رقم|P_VI_DEF,
			    (char_u *)&p_پنجره, PV_NONE, did_مجموعه_پنجره, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"winfixbuf", "wfb",    P_BOOL|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_WFB, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"winfixheight", "wfh", P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)VAR_WIN, PV_WFH, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"winfixwidth", "wfw",  P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)VAR_WIN, PV_WFW, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"winheight",   "wh",   P_رقم|P_VI_DEF,
			    (char_u *)&p_wh, PV_NONE,
			    did_مجموعه_winheight_helpheight, عدم,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"winhighlight", "whl", P_STRING|P_VI_DEF|P_RALL|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)VAR_WIN, PV_WHL, did_مجموعه_winhighlight, expand_مجموعه_winhighlight,
			    {(char_u *)"", (char_u *)عدم} SCTX_INIT},
    {"winminheight", "wmh", P_رقم|P_VI_DEF,
			    (char_u *)&p_wmh, PV_NONE, did_مجموعه_winminheight, عدم,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"winminwidth", "wmw",  P_رقم|P_VI_DEF,
			    (char_u *)&p_wmw, PV_NONE, did_مجموعه_winminwidth, عدم,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"winptydll", عدم,	    P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#إذا defined(MSWIN) && defined(FEAT_TERMINAL)
			    (char_u *)&p_winptydll, PV_NONE, عدم, عدم, {
# ifdef _WIN64
			    (char_u *)"winpty64.dll",
# وإلا
			    (char_u *)"winpty32.dll",
# endif
				(char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"winwidth",   "wiw",   P_رقم|P_VI_DEF,
			    (char_u *)&p_wiw, PV_NONE, did_مجموعه_winwidth, عدم,
			    {(char_u *)20L, (char_u *)0L} SCTX_INIT},
    {"wlseat",	    "wse",  P_STRING|P_VI_DEF,
#اگر_تعریف_شده FEAT_WAYLAND
			    (char_u *)&p_wse, PV_NONE, did_مجموعه_wlseat, عدم,
			    {(char_u *)"", (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"wlsteal",	    "wst",  P_BOOL|P_VI_DEF, // Deprecated
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
			    SCTX_INIT},
    {"wltimeoutlen", "wtm", P_رقم|P_VI_DEF,
#اگر_تعریف_شده FEAT_WAYLAND
			    (char_u *)&p_wtm, PV_NONE, did_مجموعه_wltimeoutlen, عدم,
			    {(char_u *)500L, (char_u *)0L}
#وإلا
			    (char_u *)عدم, PV_NONE, عدم, عدم,
			    {(char_u *)عدم, (char_u *)0L}
#پایان_اگر
			    SCTX_INIT},
    {"wrap",	    عدم,   P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_WRAP, did_مجموعه_wrap, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"wrapmargin",  "wm",   P_رقم|P_VI_DEF,
			    (char_u *)&p_wm, PV_WM, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"wrapscan",    "ws",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ws, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"write",	    عدم,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_write, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"writeany",    "wa",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_wa, PV_NONE, عدم, عدم,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"writebackup", "wb",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_wb, PV_NONE, عدم, عدم,
			    {
#اگر_تعریف_شده FEAT_WRITEBACKUP
			    (char_u *)TRUE,
#وإلا
			    (char_u *)FALSE,
#پایان_اگر
				(char_u *)0L} SCTX_INIT},
    {"writedelay",  "wd",   P_رقم|P_VI_DEF,
			    (char_u *)&p_wd, PV_NONE, عدم, عدم,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"xtermcodes",  عدم,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_xtermcodes, PV_NONE, عدم, عدم,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},

// terminal output codes
#تعریف_کن p_جمله(sss, vvv) \
			    {sss, عدم, P_STRING|P_VI_DEF|P_RALL|P_SECURE, \
			    (char_u *)&vvv, PV_NONE, did_مجموعه_جمله_option, عدم, \
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},

    p_جمله("t_AB", T_CAB)
    p_جمله("t_AF", T_CAF)
    p_جمله("t_AU", T_CAU)
    p_جمله("t_AL", T_CAL)
    p_جمله("t_al", T_AL)
    p_جمله("t_bc", T_BC)
    p_جمله("t_BE", T_BE)
    p_جمله("t_BD", T_BD)
    p_جمله("t_cd", T_CD)
    p_جمله("t_ce", T_CE)
    p_جمله("t_Ce", T_UCE)
    p_جمله("t_CF", T_CFO)
    p_جمله("t_cl", T_CL)
    p_جمله("t_cm", T_CM)
    p_جمله("t_Co", T_CCO)
    p_جمله("t_CS", T_CCS)
    p_جمله("t_Cs", T_UCS)
    p_جمله("t_cs", T_CS)
    p_جمله("t_CV", T_CSV)
    p_جمله("t_da", T_DA)
    p_جمله("t_db", T_DB)
    p_جمله("t_DL", T_CDL)
    p_جمله("t_dl", T_DL)
    p_جمله("t_ds", T_DS)
    p_جمله("t_Ds", T_CDS)
    p_جمله("t_EC", T_CEC)
    p_جمله("t_EI", T_CEI)
    p_جمله("t_fs", T_FS)
    p_جمله("t_fd", T_FD)
    p_جمله("t_fe", T_FE)
    p_جمله("t_GP", T_CGP)
    p_جمله("t_IE", T_CIE)
    p_جمله("t_IS", T_CIS)
    p_جمله("t_ke", T_KE)
    p_جمله("t_ks", T_KS)
    p_جمله("t_le", T_LE)
    p_جمله("t_mb", T_MB)
    p_جمله("t_md", T_MD)
    p_جمله("t_me", T_ME)
    p_جمله("t_mr", T_MR)
    p_جمله("t_ms", T_MS)
    p_جمله("t_nd", T_ND)
    p_جمله("t_op", T_OP)
    p_جمله("t_RF", T_RFG)
    p_جمله("t_RB", T_RBG)
    p_جمله("t_RC", T_بررسی_چرخهای)
    p_جمله("t_RI", T_CRI)
    p_جمله("t_Ri", T_SRI)
    p_جمله("t_RK", T_CRK)
    p_جمله("t_RS", T_CRS)
    p_جمله("t_RT", T_CRT)
    p_جمله("t_RV", T_CRV)
    p_جمله("t_Sb", T_CSB)
    p_جمله("t_SC", T_CSC)
    p_جمله("t_se", T_SE)
    p_جمله("t_Sf", T_CSF)
    p_جمله("t_SH", T_CSH)
    p_جمله("t_SI", T_CSI)
    p_جمله("t_Si", T_SSI)
    p_جمله("t_so", T_SO)
    p_جمله("t_SR", T_CSR)
    p_جمله("t_sr", T_SR)
    p_جمله("t_ST", T_CST)
    p_جمله("t_Te", T_STE)
    p_جمله("t_te", T_TE)
    p_جمله("t_TE", T_CTE)
    p_جمله("t_ti", T_TI)
    p_جمله("t_TI", T_CTI)
    p_جمله("t_Ts", T_STS)
    p_جمله("t_ts", T_TS)
    p_جمله("t_u7", T_U7)
    p_جمله("t_ue", T_UE)
    p_جمله("t_us", T_US)
    p_جمله("t_Us", T_USS)
    p_جمله("t_ut", T_UT)
    p_جمله("t_vb", T_VB)
    p_جمله("t_ve", T_VE)
    p_جمله("t_vi", T_VI)
    p_جمله("t_VS", T_CVS)
    p_جمله("t_vs", T_VS)
    p_جمله("t_WP", T_CWP)
    p_جمله("t_WS", T_CWS)
    p_جمله("t_XM", T_CXM)
    p_جمله("t_xn", T_XN)
    p_جمله("t_xs", T_XS)
    p_جمله("t_ZH", T_CZH)
    p_جمله("t_ZR", T_CZR)
    p_جمله("t_8f", T_8F)
    p_جمله("t_8b", T_8B)
    p_جمله("t_8u", T_8U)
    p_جمله("t_xo", T_XON)
    p_جمله("t_BS", T_BSU)
    p_جمله("t_ES", T_ESU)

// terminal key codes are not in here

    // end marker
    {عدم, عدم, 0, عدم, PV_NONE, عدم, عدم, {عدم, عدم} SCTX_INIT}
};

#تعریف_کن OPTION_شمار ARRAY_درازی(options)

// The following is needed to make the gen_opt_test.vim script work.
// {"
