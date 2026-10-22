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
#నిర్వచించు PV_BOTH 0x1000
#నిర్వచించు PV_WIN  0x2000
#నిర్వచించు PV_BUF  0x4000
#నిర్వచించు PV_ముసుగు 0x0fff
#నిర్వచించు OPT_WIN(x)  (idopt_T)(PV_WIN + (పూర్ణాంకం)(x))
#నిర్వచించు OPT_BUF(x)  (idopt_T)(PV_BUF + (పూర్ణాంకం)(x))
#నిర్వచించు OPT_BOTH(x) (idopt_T)(PV_BOTH + (పూర్ణాంకం)(x))

// Definition of the PV_ values for buffer-local options.
// The BV_ values are defined in option.h.
#నిర్వచించు PV_AC		OPT_BOTH(OPT_BUF(BV_AC))
#నిర్వచించు PV_AI		OPT_BUF(BV_AI)
#నిర్వచించు PV_AR		OPT_BOTH(OPT_BUF(BV_AR))
#నిర్వచించు PV_BKC		OPT_BOTH(OPT_BUF(BV_BKC))
#నిర్వచించు PV_BH		OPT_BUF(BV_BH)
#నిర్వచించు PV_BT		OPT_BUF(BV_BT)
#నిర్వచితమైతే FEAT_QUICKFIX
# define PV_EFM		OPT_BOTH(OPT_BUF(BV_EFM))
# define PV_GEFM	OPT_BOTH(OPT_BUF(BV_GEFM))
# define PV_GP		OPT_BOTH(OPT_BUF(BV_GP))
# define PV_MP		OPT_BOTH(OPT_BUF(BV_MP))
#అయితే_ముగింపు
#నిర్వచించు PV_BIN		OPT_BUF(BV_BIN)
#నిర్వచించు PV_BL		OPT_BUF(BV_BL)
#నిర్వచించు PV_BOMB		OPT_BUF(BV_BOMB)
#నిర్వచించు PV_CI		OPT_BUF(BV_CI)
#నిర్వచించు PV_CIN		OPT_BUF(BV_CIN)
#నిర్వచించు PV_CINK		OPT_BUF(BV_CINK)
#నిర్వచించు PV_CINO		OPT_BUF(BV_CINO)
#నిర్వచించు PV_CINSD	OPT_BUF(BV_CINSD)
#నిర్వచించు PV_CINW		OPT_BUF(BV_CINW)
#నిర్వచించు PV_CM		OPT_BOTH(OPT_BUF(BV_CM))
#నిర్వచించు PV_CMS		OPT_BUF(BV_CMS)
#నిర్వచించు PV_COM		OPT_BUF(BV_COM)
#నిర్వచించు PV_COT		OPT_BOTH(OPT_BUF(BV_COT))
#నిర్వచించు PV_CPT		OPT_BUF(BV_CPT)
#నిర్వచించు PV_DICT		OPT_BOTH(OPT_BUF(BV_DICT))
#నిర్వచితమైతే FEAT_DIFF
# define PV_DIA		OPT_BOTH(OPT_BUF(BV_DIA))
#అయితే_ముగింపు
#నిర్వచించు PV_TSR		OPT_BOTH(OPT_BUF(BV_TSR))
#నిర్వచించు PV_FFU		OPT_BOTH(OPT_BUF(BV_FFU))
#నిర్వచించు PV_CSL		OPT_BUF(BV_CSL)
#నిర్వచితమైతే FEAT_COMPL_FUNC
# define PV_CFU		OPT_BUF(BV_CFU)
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_కనుగొను_ID
# define PV_DEF		OPT_BOTH(OPT_BUF(BV_DEF))
# define PV_INC		OPT_BOTH(OPT_BUF(BV_INC))
#అయితే_ముగింపు
#నిర్వచించు PV_EOF		OPT_BUF(BV_EOF)
#నిర్వచించు PV_EOL		OPT_BUF(BV_EOL)
#నిర్వచించు PV_FIXEOL	OPT_BUF(BV_FIXEOL)
#నిర్వచించు PV_EP		OPT_BOTH(OPT_BUF(BV_EP))
#నిర్వచించు PV_ET		OPT_BUF(BV_ET)
#నిర్వచించు PV_FENC		OPT_BUF(BV_FENC)
#ఒకవేళ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
# define PV_BEXPR	OPT_BOTH(OPT_BUF(BV_BEXPR))
#అయితే_ముగింపు
#నిర్వచించు PV_FP		OPT_BOTH(OPT_BUF(BV_FP))
#నిర్వచితమైతే FEAT_EVAL
# define PV_FEX		OPT_BUF(BV_FEX)
#అయితే_ముగింపు
#నిర్వచించు PV_FF		OPT_BUF(BV_FF)
#నిర్వచించు PV_FLP		OPT_BUF(BV_FLP)
#నిర్వచించు PV_FO		OPT_BUF(BV_FO)
#నిర్వచితమైతే HAVE_FSYNC
# define PV_FS		OPT_BOTH(OPT_BUF(BV_FS))
#అయితే_ముగింపు
#నిర్వచించు PV_FT		OPT_BUF(BV_FT)
#నిర్వచించు PV_IMI		OPT_BUF(BV_IMI)
#నిర్వచించు PV_IMS		OPT_BUF(BV_IMS)
#ఒకవేళ defined(FEAT_EVAL)
# define PV_INDE	OPT_BUF(BV_INDE)
# define PV_INDK	OPT_BUF(BV_INDK)
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_కనుగొను_ID) && defined(FEAT_EVAL)
# define PV_INEX	OPT_BUF(BV_INEX)
#అయితే_ముగింపు
#నిర్వచించు PV_INF		OPT_BUF(BV_INF)
#నిర్వచించు PV_ISK		OPT_BUF(BV_ISK)
#నిర్వచితమైతే FEAT_CRYPT
# define PV_కీలకం		OPT_BUF(BV_కీలకం)
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_KEYMAP
# define PV_KMAP	OPT_BUF(BV_KMAP)
#అయితే_ముగింపు
#నిర్వచించు PV_KP		OPT_BOTH(OPT_BUF(BV_KP))
#నిర్వచించు PV_LISP		OPT_BUF(BV_LISP)
#నిర్వచించు PV_LOP		OPT_BUF(BV_LOP)
#నిర్వచించు PV_LW		OPT_BOTH(OPT_BUF(BV_LW))
#నిర్వచించు PV_MENC		OPT_BOTH(OPT_BUF(BV_MENC))
#నిర్వచించు PV_MA		OPT_BUF(BV_MA)
#నిర్వచించు PV_ML		OPT_BUF(BV_ML)
#నిర్వచించు PV_MOD		OPT_BUF(BV_MOD)
#నిర్వచించు PV_MPS		OPT_BUF(BV_MPS)
#నిర్వచించు PV_NF		OPT_BUF(BV_NF)
#నిర్వచితమైతే FEAT_COMPL_FUNC
# define PV_OFU		OPT_BUF(BV_OFU)
#అయితే_ముగింపు
#నిర్వచించు PV_మార్గం		OPT_BOTH(OPT_BUF(BV_మార్గం))
#నిర్వచించు PV_PI		OPT_BUF(BV_PI)
#నిర్వచించు PV_QE		OPT_BUF(BV_QE)
#నిర్వచించు PV_RO		OPT_BUF(BV_RO)
#నిర్వచించు PV_SI		OPT_BUF(BV_SI)
#నిర్వచించు PV_SN		OPT_BUF(BV_SN)
#నిర్వచితమైతే FEAT_SYN_HL
# define PV_SMC		OPT_BUF(BV_SMC)
# define PV_SYN		OPT_BUF(BV_SYN)
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
# define PV_SPC		OPT_BUF(BV_SPC)
# define PV_SPF		OPT_BUF(BV_SPF)
# define PV_SPL		OPT_BUF(BV_SPL)
# define PV_SPO		OPT_BUF(BV_SPO)
#అయితే_ముగింపు
#నిర్వచించు PV_STS		OPT_BUF(BV_STS)
#నిర్వచించు PV_SUA		OPT_BUF(BV_SUA)
#నిర్వచించు PV_SW		OPT_BUF(BV_SW)
#నిర్వచించు PV_SWF		OPT_BUF(BV_SWF)
#నిర్వచితమైతే FEAT_EVAL
# define PV_TFU		OPT_BUF(BV_TFU)
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_COMPL_FUNC
# define PV_TSRFU	OPT_BOTH(OPT_BUF(BV_TSRFU))
#అయితే_ముగింపు
#నిర్వచించు PV_ట్యాగులు		OPT_BOTH(OPT_BUF(BV_ట్యాగులు))
#నిర్వచించు PV_TC		OPT_BOTH(OPT_BUF(BV_TC))
#నిర్వచించు PV_TS		OPT_BUF(BV_TS)
#నిర్వచించు PV_TW		OPT_BUF(BV_TW)
#నిర్వచించు PV_TX		OPT_BUF(BV_TX)
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
# define PV_UDF		OPT_BUF(BV_UDF)
#అయితే_ముగింపు
#నిర్వచించు PV_WM		OPT_BUF(BV_WM)
#నిర్వచితమైతే FEAT_VARTABS
# define PV_VSTS	OPT_BUF(BV_VSTS)
# define PV_VTS		OPT_BUF(BV_VTS)
#అయితే_ముగింపు

// Definition of the PV_ values for window-local options.
// The WV_ values are defined in option.h.
#నిర్వచించు PV_LIST		OPT_WIN(WV_LIST)
#నిర్వచితమైతే FEAT_ARABIC
# define PV_ARAB	OPT_WIN(WV_ARAB)
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
# define PV_BRI		OPT_WIN(WV_BRI)
# define PV_BRIOPT	OPT_WIN(WV_BRIOPT)
#అయితే_ముగింపు
#నిర్వచించు PV_WCR		OPT_WIN(WV_WCR)
#నిర్వచితమైతే FEAT_DIFF
# define PV_DIFF	OPT_WIN(WV_DIFF)
#అయితే_ముగింపు
#నిర్వచించు PV_EIW		OPT_WIN(WV_EIW)
#నిర్వచితమైతే FEAT_FOLDING
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
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
# define PV_LBR		OPT_WIN(WV_LBR)
#అయితే_ముగింపు
#నిర్వచించు PV_LCS		OPT_BOTH(OPT_WIN(WV_LCS))
#నిర్వచించు PV_FCS		OPT_BOTH(OPT_WIN(WV_FCS))
#నిర్వచించు PV_NU		OPT_WIN(WV_NU)
#నిర్వచించు PV_RNU		OPT_WIN(WV_RNU)
#నిర్వచించు PV_VE		OPT_BOTH(OPT_WIN(WV_VE))
#నిర్వచితమైతే FEAT_LINEBREAK
# define PV_NUW		OPT_WIN(WV_NUW)
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_QUICKFIX)
# define PV_PVW		OPT_WIN(WV_PVW)
# define PV_LHI         OPT_WIN(WV_LHI)
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RIGHTLEFT
# define PV_RL		OPT_WIN(WV_RL)
# define PV_RLC		OPT_WIN(WV_RLC)
#అయితే_ముగింపు
#నిర్వచించు PV_SCBIND	OPT_WIN(WV_SCBIND)
#నిర్వచించు PV_SCROLL	OPT_WIN(WV_SCROLL)
#నిర్వచించు PV_SMS		OPT_WIN(WV_SMS)
#నిర్వచించు PV_SISO		OPT_BOTH(OPT_WIN(WV_SISO))
#నిర్వచించు PV_SO		OPT_BOTH(OPT_WIN(WV_SO))
#నిర్వచించు PV_SOP		OPT_BOTH(OPT_WIN(WV_SOP))
#నిర్వచితమైతే FEAT_SPELL
# define PV_SPELL	OPT_WIN(WV_SPELL)
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
# define PV_CUC		OPT_WIN(WV_CUC)
# define PV_CUL		OPT_WIN(WV_CUL)
# define PV_CULOPT	OPT_WIN(WV_CULOPT)
# define PV_CC		OPT_WIN(WV_CC)
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
# define PV_SBR		OPT_BOTH(OPT_WIN(WV_SBR))
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_STL_OPT
# define PV_STL		OPT_BOTH(OPT_WIN(WV_STL))
# define PV_STLO	OPT_BOTH(OPT_WIN(WV_STLO))
#అయితే_ముగింపు
#నిర్వచించు PV_UL		OPT_BOTH(OPT_BUF(BV_UL))
#నిర్వచించు PV_WFB		OPT_WIN(WV_WFB)
#నిర్వచించు PV_WFH		OPT_WIN(WV_WFH)
#నిర్వచించు PV_WFW		OPT_WIN(WV_WFW)
#నిర్వచించు PV_WHL		OPT_WIN(WV_WHL)
#నిర్వచించు PV_WRAP		OPT_WIN(WV_WRAP)
#నిర్వచించు PV_CRBIND	OPT_WIN(WV_CRBIND)
#నిర్వచితమైతే FEAT_CONCEAL
# define PV_COCU	OPT_WIN(WV_COCU)
# define PV_COLE	OPT_WIN(WV_COLE)
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
# define PV_TWK		OPT_WIN(WV_TWK)
# define PV_TWS		OPT_WIN(WV_TWS)
# define PV_TWSL	OPT_BUF(BV_TWSL)
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
# define PV_SCL		OPT_WIN(WV_SCL)
#అయితే_ముగింపు

// WV_ and BV_ values get typecasted to this for the "indir" field
రకనిర్వచనం గణన
{
    PV_NONE = 0,
    PV_MAXVAL = 0xffff    // to avoid warnings for value out of range
} idopt_T;

// Options local to a window have a value local to a buffer and global to all
// buffers.  Indicate this by setting "var" to VAR_WIN.
#నిర్వచించు VAR_WIN ((char_u *)-1)

// Saved values for when 'bin' is set.
స్థిర పూర్ణాంకం	p_et_nobin;
స్థిర పూర్ణాంకం	p_ml_nobin;
స్థిర దీర్ఘ	p_tw_nobin;
స్థిర దీర్ఘ	p_wm_nobin;

// Saved values for when 'paste' is set
స్థిర పూర్ణాంకం	p_ai_nopaste;
స్థిర పూర్ణాంకం	p_et_nopaste;
స్థిర దీర్ఘ	p_sts_nopaste;
స్థిర దీర్ఘ	p_tw_nopaste;
స్థిర దీర్ఘ	p_wm_nopaste;
#నిర్వచితమైతే FEAT_VARTABS
స్థిర char_u	*p_vsts_nopaste;
#అయితే_ముగింపు

నిర్మాణం vimoption
{
    అక్షరం	*fullname;	// full option name
    అక్షరం	*shortname;	// permissible abbreviation
    long_u	flags;		// see in option.h
    char_u	*var;		// global option: pointer to variable;
				// window-local option: VAR_WIN;
				// buffer-local option: global value
    idopt_T	indir;		// global option: PV_NONE;
				// local option: indirect option index
    // callback function to invoke after an option is modified to validate and
    // apply the new value.
    opt_did_సమితి_cb_T	opt_did_సమితి_cb;

    // callback function to invoke when expanding possible values on the
    // cmdline. Only useful for string options.
    opt_expand_cb_T	opt_expand_cb;

    char_u	*def_val[2];	// default values for variable (vi and vim)
#నిర్వచితమైతే FEAT_EVAL
    sctx_T	script_ctx;	// script context where the option was last set
# define SCTX_INIT , {0, 0, 0, 1}
#లేకపోతే
# define SCTX_INIT
#అయితే_ముగింపు
};

#నిర్వచించు VI_DEFAULT  0	    // def_val[VI_DEFAULT] is Vi default value
#నిర్వచించు VIM_DEFAULT 1	    // def_val[VIM_DEFAULT] is Vim default value

#నిర్వచించు ISK_LATIN1  (char_u *)"@,48-57,_,192-255"

// 'isprint' for latin1 is also used for MS-Windows cp1252, where 0x80 is used
// for the currency sign.
#ఒకవేళ defined(MSWIN)
# define ISP_LATIN1 (char_u *)"@,~-255"
#లేకపోతే
# define ISP_LATIN1 (char_u *)"@,161-255"
#అయితే_ముగింపు

#నిర్వచించు HIGHLIGHT_INIT "8:SpecialKey,~:EndOfBuffer,@:NonText,d:Directory,e:ErrorMsg,i:IncSearch,l:Search,y:CurSearch,m:MoreMsg,M:ModeMsg,n:LineNr,a:LineNrAbove,b:LineNrBelow,N:CursorLineNr,G:CursorLineSign,O:CursorLineFold,r:Question,s:StatusLine,S:StatusLineNC,c:VertSplit,|:VertSplitNC,t:Title,v:Visual,V:VisualNOS,w:WarningMsg,W:WildMenu,f:Folded,F:FoldColumn,A:DiffAdd,C:DiffChange,D:DiffDelete,T:DiffText,E:DiffTextAdd,>:SignColumn,-:Conceal,B:SpellBad,P:SpellCap,R:SpellRare,L:SpellLocal,+:Pmenu,=:PmenuSel,k:PmenuMatch,<:PmenuMatchSel,[:PmenuKind,]:PmenuKindSel,{:PmenuExtra,}:PmenuExtraSel,x:PmenuSbar,X:PmenuThumb,j:PmenuBorder,H:PmenuShadow,p:Popup,J:PopupBorder,Q:PopupTitle,*:TabLine,#:TabLineSel,_:TabLineFill,!:CursorColumn,.:CursorLine,o:ColorColumn,q:QuickFixLine,z:StatusLineTerm,Z:StatusLineTermNC,g:MsgArea,h:ComplMatchIns,%:TabPanel,^:TabPanelSel,&:TabPanelFill,I:PreInsert"

// Default python version for pyx* commands
#ఒకవేళ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
# define DEFAULT_PYTHON_VER	0
#లేదావేళ defined(FEAT_PYTHON3)
# define DEFAULT_PYTHON_VER	3
#లేదావేళ defined(FEAT_PYTHON)
# define DEFAULT_PYTHON_VER	2
#లేకపోతే
# define DEFAULT_PYTHON_VER	0
#అయితే_ముగింపు

// used for 'cinkeys' and 'indentkeys'
#నిర్వచించు INDENTKEYS_DEFAULT (char_u *)"0{,0},0),0],:,0#,!^F,o,O,e"

// options[] is initialized here.
// The order of the options MUST be alphabetic for ":set all" and findoption().
// All option names MUST start with a lowercase letter (for findoption()).
// Exception: "t_" options are at the end.
// The options with a NULL variable are 'hidden': a set command for them is
// ignored and they are not printed.
స్థిర నిర్మాణం vimoption options[] =
{
    {"aleph",	    "al",   P_అంకె|P_VI_DEF|P_CURSWANT,
#నిర్వచితమైతే FEAT_RIGHTLEFT
			    (char_u *)&p_aleph, PV_NONE,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE,
#అయితే_ముగింపు
			    ఖాళీ, ఖాళీ,
			    {
#ఒకవేళ defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
			    (char_u *)128L,
#లేకపోతే
			    (char_u *)224L,
#అయితే_ముగింపు
					    (char_u *)0L} SCTX_INIT},
    {"allowrevins", "ari",  P_BOOL|P_VI_DEF|P_VIM,
#నిర్వచితమైతే FEAT_RIGHTLEFT
			    (char_u *)&p_ari, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"altkeymap",   "akm",  P_BOOL|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"ambiwidth",  "ambw",  P_STRING|P_VI_DEF|P_RCLR,
			    (char_u *)&p_ambw, PV_NONE, did_సమితి_ambiwidth, expand_సమితి_ambiwidth,
			    {(char_u *)"single", (char_u *)0L}
			    SCTX_INIT},
    {"antialias",   "anti", P_BOOL|P_VI_DEF|P_VIM|P_RCLR,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)FALSE}
			    SCTX_INIT},
    {"arabic",	    "arab", P_BOOL|P_VI_DEF|P_VIM|P_CURSWANT,
#నిర్వచితమైతే FEAT_ARABIC
			    (char_u *)VAR_WIN, PV_ARAB, did_సమితి_arabic, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"arabicshape", "arshape", P_BOOL|P_VI_DEF|P_VIM|P_RCLR,
#నిర్వచితమైతే FEAT_ARABIC
			    (char_u *)&p_arshape, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"autochdir",  "acd",   P_BOOL|P_VI_DEF,
#నిర్వచితమైతే FEAT_AUTOCHDIR
			    (char_u *)&p_acd, PV_NONE, did_సమితి_autochdir, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
#నిర్వచితమైతే ELAPSED_FUNC
    {"autocomplete",  "ac", P_BOOL|P_VI_DEF,
			    (char_u *)&p_ac, PV_AC, ఖాళీ,
			    ఖాళీ, {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autocompletedelay", "acl", P_అంకె|P_VI_DEF,
			    (char_u *)&p_acl, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"autocompletetimeout", "act", P_అంకె|P_VI_DEF,
			    (char_u *)&p_act, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)80L, (char_u *)0L} SCTX_INIT},
#అయితే_ముగింపు
    {"autoindent",  "ai",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ai, PV_AI, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autoprint",   "ap",   P_BOOL|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autoread",    "ar",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ar, PV_AR, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autoshelldir",  "asd",   P_BOOL|P_VI_DEF,
#నిర్వచితమైతే FEAT_AUTOSHELLDIR
			    (char_u *)&p_asd, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"autowrite",   "aw",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_aw, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autowriteall","awa",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_awa, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"background",  "bg",   P_STRING|P_VI_DEF|P_RCLR|P_HLONLY,
			    (char_u *)&p_bg, PV_NONE, did_సమితి_background, expand_సమితి_background,
			    {
#ఒకవేళ (defined(MSWIN)) && !defined(FEAT_GUI)
			    (char_u *)"dark",
#లేకపోతే
			    (char_u *)"light",
#అయితే_ముగింపు
					    (char_u *)0L} SCTX_INIT},
    {"backspace",   "bs",   P_STRING|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_bs, PV_NONE, did_సమితి_backspace, expand_సమితి_backspace,
			    {(char_u *)"", (char_u *)"indent,eol,start"} SCTX_INIT},
    {"backup",	    "bk",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_bk, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"backupcopy",  "bkc",  P_STRING|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_bkc, PV_BKC, did_సమితి_backupcopy, expand_సమితి_backupcopy,
#నిర్వచితమైతే UNIX
			    {(char_u *)"yes", (char_u *)"auto"}
#లేకపోతే
			    {(char_u *)"auto", (char_u *)"auto"}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"backupdir",   "bdir", P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA
							    |P_NODUP|P_SECURE,
			    (char_u *)&p_bdir, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DFLT_BDIR, (char_u *)0L} SCTX_INIT},
    {"backupext",   "bex",  P_STRING|P_VI_DEF|P_NFNAME,
			    (char_u *)&p_bex, PV_NONE,
			    did_సమితి_backupext_or_patchmode, ఖాళీ,
			    {
#నిర్వచితమైతే VMS
			    (char_u *)"_",
#లేకపోతే
			    (char_u *)"~",
#అయితే_ముగింపు
					    (char_u *)0L} SCTX_INIT},
    {"backupskip",  "bsk",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_bsk, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"balloondelay","bdlay",P_అంకె|P_VI_DEF,
#నిర్వచితమైతే FEAT_BEVAL
			    (char_u *)&p_bdlay, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)600L, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"ballooneval", "beval",P_BOOL|P_VI_DEF|P_NO_MKRC,
#నిర్వచితమైతే FEAT_BEVAL_GUI
			    (char_u *)&p_beval, PV_NONE, did_సమితి_ballooneval, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"balloonevalterm", "bevalterm",P_BOOL|P_VI_DEF|P_NO_MKRC,
#నిర్వచితమైతే FEAT_BEVAL_పదం
			    (char_u *)&p_bevalterm, PV_NONE,
			    did_సమితి_balloonevalterm, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"balloonexpr", "bexpr", P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_MLE,
#ఒకవేళ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
			    (char_u *)&p_bexpr, PV_BEXPR, did_సమితి_optexpr, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"beautify",    "bf",   P_BOOL|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"belloff",      "bo",  P_STRING|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_bo, PV_NONE, did_సమితి_belloff, expand_సమితి_belloff,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"binary",	    "bin",  P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_bin, PV_BIN, did_సమితి_ద్విమాన, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"bioskey",	    "biosk",P_BOOL|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"bomb",	    ఖాళీ,   P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_bomb, PV_BOMB,
			    did_సమితి_eof_eol_fixeol_bomb, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"breakat",	    "brk",  P_STRING|P_VI_DEF|P_RALL|P_FLAGLIST,
#నిర్వచితమైతే FEAT_LINEBREAK
			    (char_u *)&p_breakat, PV_NONE, did_సమితి_breakat, ఖాళీ,
			    {(char_u *)" \t!@*-+;:,./?", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"breakindent",   "bri",  P_BOOL|P_VI_DEF|P_VIM|P_RWIN,
#నిర్వచితమైతే FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_BRI, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"breakindentopt", "briopt", P_STRING|P_ALLOCED|P_VI_DEF|P_RBUF
						  |P_ONECOMMA|P_NODUP,
#నిర్వచితమైతే FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_BRIOPT,
			    did_సమితి_breakindentopt, expand_సమితి_breakindentopt,
			    {(char_u *)"", (char_u *)ఖాళీ}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)ఖాళీ}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"browsedir",   "bsdir",P_STRING|P_VI_DEF,
#నిర్వచితమైతే FEAT_BROWSE
			    (char_u *)&p_bsdir, PV_NONE, did_సమితి_browsedir, expand_సమితి_browsedir,
			    {(char_u *)"last", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"bufhidden",   "bh",   P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_bh, PV_BH, did_సమితి_bufhidden, expand_సమితి_bufhidden,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"buflisted",   "bl",   P_BOOL|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_bl, PV_BL, did_సమితి_buflisted, ఖాళీ,
			    {(char_u *)1L, (char_u *)0L}
			    SCTX_INIT},
    {"buftype",	    "bt",   P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_bt, PV_BT, did_సమితి_buftype, expand_సమితి_buftype,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"casemap",	    "cmp",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cmp, PV_NONE, did_సమితి_casemap, expand_సమితి_casemap,
			    {(char_u *)"internal,keepascii", (char_u *)0L}
			    SCTX_INIT},
    {"cdhome",	    "cdh",  P_BOOL|P_VI_DEF|P_VIM|P_SECURE,
			    (char_u *)&p_cdh, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L}
			    SCTX_INIT},
    {"cdpath",	    "cd",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE|P_COMMA|P_NODUP,
			    (char_u *)&p_cdpath, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)",,", (char_u *)0L}
			    SCTX_INIT},
    {"cedit",	    ఖాళీ,   P_STRING,
			    (char_u *)&p_cedit, PV_NONE, did_సమితి_cedit, ఖాళీ,
			    {(char_u *)"", (char_u *)CTRL_F_STR}
			    SCTX_INIT},
    {"charconvert",  "ccv", P_STRING|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(FEAT_EVAL)
			    (char_u *)&p_ccv, PV_NONE, did_సమితి_optexpr, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"chistory",    "chi",  P_అంకె|P_VI_DEF,
#నిర్వచితమైతే FEAT_QUICKFIX
			    (char_u *)&p_chi, PV_NONE, did_సమితి_xhistory, ఖాళీ,
			    {(char_u *)10L, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"cindent",	    "cin",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_cin, PV_CIN, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cinkeys",	    "cink", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cink, PV_CINK, ఖాళీ, ఖాళీ,
			    {INDENTKEYS_DEFAULT, (char_u *)0L} SCTX_INIT},
    {"cinoptions",  "cino", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cino, PV_CINO, did_సమితి_cinoptions, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"cinscopedecls", "cinsd", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cinsd, PV_CINSD, ఖాళీ, ఖాళీ,
			    {(char_u *)"public,protected,private", (char_u *)0L}
			    SCTX_INIT},
    {"cinwords",    "cinw", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cinw, PV_CINW, ఖాళీ, ఖాళీ,
			    {(char_u *)"if,else,while,do,for,switch",
				(char_u *)0L}
			    SCTX_INIT},
    {"clipboard",   "cb",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#నిర్వచితమైతే HAVE_CLIPMETHOD
			    (char_u *)&p_cb, PV_NONE, did_సమితి_clipboard, expand_సమితి_clipboard,
# ఒకవేళ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
			    {(char_u *)"autoselect,exclude:cons\\|linux",
							       (char_u *)0L}
# లేకపోతే
			    {(char_u *)"", (char_u *)0L}
# endif
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"clipmethod", "cpm",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#నిర్వచితమైతే HAVE_CLIPMETHOD
			    (char_u *)&p_cpm, PV_NONE, did_సమితి_clipmethod, expand_సమితి_clipmethod,
# ifdef UNIX
			    {(char_u *)"wayland,x11", (char_u *)0L}
# elif defined(VMS)
			    {(char_u *)"x11", (char_u *)0L}
# లేకపోతే
			    {(char_u *)"", (char_u *)0L}
# endif
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"cmdheight",   "ch",   P_అంకె|P_VI_DEF|P_RALL,
			    (char_u *)&p_ch, PV_NONE, did_సమితి_cmdheight, ఖాళీ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"cmdwinheight", "cwh", P_అంకె|P_VI_DEF,
			    (char_u *)&p_cwh, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)7L, (char_u *)0L} SCTX_INIT},
    {"colorcolumn", "cc",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_RWIN|P_HLONLY,
#నిర్వచితమైతే FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CC, did_సమితి_colorcolumn, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"columns",	    "co",   P_అంకె|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RCLR,
			    (char_u *)&Columns, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)80L, (char_u *)0L} SCTX_INIT},
    {"comments",    "com",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_com, PV_COM, did_సమితి_comments, ఖాళీ,
			    {(char_u *)"s1:/*,mb:*,ex:*/,://,b:#,:%,:XCOMM,n:>,fb:-",
				(char_u *)0L}
			    SCTX_INIT},
    {"commentstring", "cms", P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)&p_cms, PV_CMS, did_సమితి_commentstring, ఖాళీ,
			    {(char_u *)"/* %s */", (char_u *)0L}
			    SCTX_INIT},
			    // P_PRI_MKRC isn't needed here, optval_default()
			    // always returns TRUE for 'compatible'
    {"compatible",  "cp",   P_BOOL|P_RALL,
			    (char_u *)&p_cp, PV_NONE, did_సమితి_compatible, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)FALSE} SCTX_INIT},
    {"complete",    "cpt",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP|P_MLE,
			    (char_u *)&p_cpt, PV_CPT, did_సమితి_complete, expand_సమితి_complete,
			    {(char_u *)".,w,b,u,t,i", (char_u *)0L}
			    SCTX_INIT},
    {"completefunc", "cfu", P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#నిర్వచితమైతే FEAT_COMPL_FUNC
			    (char_u *)&p_cfu, PV_CFU, did_సమితి_completefunc, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"completefuzzycollect", "cfc", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cfc, PV_NONE, did_సమితి_completefuzzycollect, expand_సమితి_completefuzzycollect,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"completeitemalign", "cia", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cia, PV_NONE, did_సమితి_completeitemalign, ఖాళీ,
			    {(char_u *)"abbr,kind,menu", (char_u *)0L}
			    SCTX_INIT},
    {"completeopt",   "cot",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cot, PV_COT, did_సమితి_completeopt, expand_సమితి_completeopt,
			    {(char_u *)"menu,preview", (char_u *)0L}
			    SCTX_INIT},
    {"completepopup", "cpp", P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON,
#ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
			    (char_u *)&p_cpp, PV_NONE, did_సమితి_completepopup, expand_సమితి_completepopup,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"completeslash",   "csl",  P_STRING|P_VI_DEF|P_VIM,
#ఒకవేళ defined(BACKSLASH_లోపల_FILENAME)
			    (char_u *)&p_csl, PV_CSL, did_సమితి_completeslash, expand_సమితి_completeslash,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"completetimeout", "cto", P_అంకె|P_VI_DEF,
			    (char_u *)&p_cto, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"concealcursor","cocu", P_STRING|P_ALLOCED|P_RWIN|P_VI_DEF|P_FLAGLIST,
#నిర్వచితమైతే FEAT_CONCEAL
			    (char_u *)VAR_WIN, PV_COCU, did_సమితి_concealcursor, expand_సమితి_concealcursor,
			    {(char_u *)"", (char_u *)ఖాళీ}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"conceallevel","cole", P_అంకె|P_RWIN|P_VI_DEF,
#నిర్వచితమైతే FEAT_CONCEAL
			    (char_u *)VAR_WIN, PV_COLE, did_సమితి_conceallevel, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)0L, (char_u *)0L}
			    SCTX_INIT},
    {"confirm",     "cf",   P_BOOL|P_VI_DEF,
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			    (char_u *)&p_confirm, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"conskey",	    "consk",P_BOOL|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"copyindent",  "ci",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_ci, PV_CI, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cpoptions",   "cpo",  P_STRING|P_VIM|P_RALL|P_FLAGLIST,
			    (char_u *)&p_cpo, PV_NONE, did_సమితి_cpoptions, expand_సమితి_cpoptions,
			    {(char_u *)CPO_VI, (char_u *)CPO_VIM}
			    SCTX_INIT},
    {"cryptmethod", "cm",   P_STRING|P_ALLOCED|P_VI_DEF,
#నిర్వచితమైతే FEAT_CRYPT
			    (char_u *)&p_cm, PV_CM, did_సమితి_cryptmethod, expand_సమితి_cryptmethod,
			    {(char_u *)"blowfish2", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"cscopepathcomp", "cspc", P_అంకె|P_VI_DEF|P_VIM,
#నిర్వచితమైతే FEAT_CSCOPE
			    (char_u *)&p_cspc, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopeprg",   "csprg", P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#నిర్వచితమైతే FEAT_CSCOPE
			    (char_u *)&p_csprg, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"cscope", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"cscopequickfix", "csqf", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ఒకవేళ defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
			    (char_u *)&p_csqf, PV_NONE,
			    did_సమితి_cscopequickfix, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"cscoperelative", "csre", P_BOOL|P_VI_DEF|P_VIM,
#నిర్వచితమైతే FEAT_CSCOPE
			    (char_u *)&p_csre, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopetag",   "cst",  P_BOOL|P_VI_DEF|P_VIM,
#నిర్వచితమైతే FEAT_CSCOPE
			    (char_u *)&p_cst, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopetagorder", "csto", P_అంకె|P_VI_DEF|P_VIM,
#నిర్వచితమైతే FEAT_CSCOPE
			    (char_u *)&p_csto, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopeverbose", "csverb", P_BOOL|P_VI_DEF|P_VIM,
#నిర్వచితమైతే FEAT_CSCOPE
			    (char_u *)&p_csverbose, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cursorbind",  "crb",  P_BOOL|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_CRBIND, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cursorcolumn", "cuc", P_BOOL|P_VI_DEF|P_RWIN|P_HLONLY,
#నిర్వచితమైతే FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CUC, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cursorline",   "cul", P_BOOL|P_VI_DEF|P_RWIN|P_HLONLY,
#నిర్వచితమైతే FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CUL, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cursorlineopt", "culopt", P_STRING|P_VI_DEF|P_RWIN|P_HLONLY|P_ONECOMMA|P_NODUP,
#నిర్వచితమైతే FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CULOPT,
			    did_సమితి_cursorlineopt, expand_సమితి_cursorlineopt,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)"both", (char_u *)0L} SCTX_INIT},
    {"debug",	    ఖాళీ,   P_STRING|P_VI_DEF,
			    (char_u *)&p_debug, PV_NONE, did_సమితి_debug, expand_సమితి_debug,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"define",	    "def",  P_STRING|P_ALLOCED|P_VI_DEF,
#నిర్వచితమైతే FEAT_కనుగొను_ID
			    (char_u *)&p_def, PV_DEF, ఖాళీ, ఖాళీ,
			    {(char_u *)"^\\s*#\\s*define", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"delcombine", "deco",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_deco, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"dictionary",  "dict", P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA|P_NODUP|P_NDNAME,
			    (char_u *)&p_dict, PV_DICT, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"diff",	    ఖాళీ,   P_BOOL|P_VI_DEF|P_RWIN|P_NOGLOB,
#నిర్వచితమైతే FEAT_DIFF
			    (char_u *)VAR_WIN, PV_DIFF, did_సమితి_diff, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"diffanchors", "dia",  P_STRING|P_VI_DEF|P_ONECOMMA,
#నిర్వచితమైతే FEAT_DIFF
			    (char_u *)&p_dia, PV_DIA, did_సమితి_diffanchors, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)"", (char_u *)ఖాళీ} SCTX_INIT},
    {"diffexpr",    "dex",  P_STRING|P_VI_DEF|P_SECURE|P_CURSWANT,
#ఒకవేళ defined(FEAT_DIFF) && defined(FEAT_EVAL)
			    (char_u *)&p_dex, PV_NONE, did_సమితి_optexpr, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"diffopt",	    "dip",  P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN|P_ONECOMMA|P_COLON
								     |P_NODUP,
#నిర్వచితమైతే FEAT_DIFF
			    (char_u *)&p_dip, PV_NONE, did_సమితి_diffopt, expand_సమితి_diffopt,
			    {(char_u *)"internal,filler,closeoff,indent-heuristic,inline:char",
								(char_u *)ఖాళీ}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)ఖాళీ}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"digraph",	    "dg",   P_BOOL|P_VI_DEF|P_VIM,
#నిర్వచితమైతే FEAT_DIGRAPHS
			    (char_u *)&p_dg, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"directory",   "dir",  P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA
							    |P_NODUP|P_SECURE,
			    (char_u *)&p_dir, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DFLT_DIR, (char_u *)0L} SCTX_INIT},
    {"display",	    "dy",   P_STRING|P_VI_DEF|P_ONECOMMA|P_RALL|P_NODUP,
			    (char_u *)&p_dy, PV_NONE, did_సమితి_display, expand_సమితి_display,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"eadirection", "ead",  P_STRING|P_VI_DEF,
			    (char_u *)&p_ead, PV_NONE, did_సమితి_eadirection, expand_సమితి_eadirection,
			    {(char_u *)"both", (char_u *)0L}
			    SCTX_INIT},
    {"edcompatible","ed",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ed, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"emoji",      "emo",   P_BOOL|P_VI_DEF|P_RCLR,
			    (char_u *)&p_emoji, PV_NONE, did_సమితి_ambiwidth, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L}
			    SCTX_INIT},
    {"encoding",    "enc",  P_STRING|P_VI_DEF|P_RCLR|P_NO_ML,
			    (char_u *)&p_enc, PV_NONE, did_సమితి_encoding, expand_సమితి_encoding,
			    {(char_u *)ENC_DFLT, (char_u *)0L}
			    SCTX_INIT},
    {"endoffile",   "eof",  P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_eof, PV_EOF,
			    did_సమితి_eof_eol_fixeol_bomb, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"endofline",   "eol",  P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_eol, PV_EOL,
			    did_సమితి_eof_eol_fixeol_bomb, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"equalalways", "ea",   P_BOOL|P_VI_DEF|P_RALL,
			    (char_u *)&p_ea, PV_NONE, did_సమితి_equalalways, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"equalprg",    "ep",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_ep, PV_EP, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"errorbells",  "eb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_eb, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"errorfile",   "ef",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#నిర్వచితమైతే FEAT_QUICKFIX
			    (char_u *)&p_ef, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DFLT_ERRORFILE, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"errorformat", "efm",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#నిర్వచితమైతే FEAT_QUICKFIX
			    (char_u *)&p_efm, PV_EFM, ఖాళీ, ఖాళీ,
			    {(char_u *)DFLT_EFM, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"esckeys",	    "ek",   P_BOOL|P_VIM,
			    (char_u *)&p_ek, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"eventignore", "ei",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_ei, PV_NONE, did_సమితి_eventignore, expand_సమితి_eventignore,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"eventignorewin", "eiw",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)VAR_WIN, PV_EIW, did_సమితి_eventignore, expand_సమితి_eventignore,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"expandtab",   "et",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_et, PV_ET, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"exrc",	    "ex",   P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_exrc, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"fileencoding","fenc", P_STRING|P_ALLOCED|P_VI_DEF|P_RSTAT|P_RBUF
								   |P_NO_MKRC,
			    (char_u *)&p_fenc, PV_FENC, did_సమితి_encoding, expand_సమితి_encoding,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"fileencodings","fencs", P_STRING|P_VI_DEF|P_ONECOMMA,
			    (char_u *)&p_fencs, PV_NONE, ఖాళీ, expand_సమితి_encoding,
			    {(char_u *)"ucs-bom", (char_u *)0L}
			    SCTX_INIT},
    {"fileformat",  "ff",   P_STRING|P_ALLOCED|P_VI_DEF|P_RSTAT|P_NO_MKRC
								  |P_CURSWANT,
			    (char_u *)&p_ff, PV_FF, did_సమితి_fileformat, expand_సమితి_fileformat,
			    {(char_u *)DFLT_FF, (char_u *)0L} SCTX_INIT},
    {"fileformats", "ffs",  P_STRING|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_ffs, PV_NONE, did_సమితి_fileformats, expand_సమితి_fileformat,
			    {(char_u *)DFLT_FFS_VI, (char_u *)DFLT_FFS_VIM}
			    SCTX_INIT},
    {"fileignorecase", "fic", P_BOOL|P_VI_DEF,
			    (char_u *)&p_fic, PV_NONE, ఖాళీ, ఖాళీ,
			    {
#నిర్వచితమైతే CASE_INSENSITIVE_FILENAME
				    (char_u *)TRUE,
#లేకపోతే
				    (char_u *)FALSE,
#అయితే_ముగింపు
					(char_u *)0L} SCTX_INIT},
    {"filetype",    "ft",   P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB|P_NFNAME,
			    (char_u *)&p_ft, PV_FT,
			    did_సమితి_filetype_or_syntax, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"fillchars",   "fcs",  P_STRING|P_VI_DEF|P_RALL|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_fcs, PV_FCS, did_సమితి_chars_option, expand_సమితి_chars_option,
			    {(char_u *)"vert:|,fold:-,eob:~,lastline:@",
								  (char_u *)0L}
			    SCTX_INIT},
    {"findfunc", "ffu",     P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#నిర్వచితమైతే FEAT_EVAL
			    (char_u *)&p_ffu, PV_FFU,
			    did_సమితి_findfunc, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"fixendofline",  "fixeol", P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_fixeol, PV_FIXEOL,
			    did_సమితి_eof_eol_fixeol_bomb, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"fkmap",	    "fk",   P_BOOL|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"flash",	    "fl",   P_BOOL|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"foldclose",   "fcl",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_RWIN,
#నిర్వచితమైతే FEAT_FOLDING
			    (char_u *)&p_fcl, PV_NONE, did_సమితి_foldclose, expand_సమితి_foldclose,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"foldcolumn",  "fdc",  P_అంకె|P_VI_DEF|P_RWIN,
#నిర్వచితమైతే FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDC, did_సమితి_foldcolumn, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"foldenable",  "fen",  P_BOOL|P_VI_DEF|P_RWIN,
#నిర్వచితమైతే FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FEN, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"foldexpr",    "fde",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN|P_MLE,
#ఒకవేళ defined(FEAT_FOLDING) && defined(FEAT_EVAL)
			    (char_u *)VAR_WIN, PV_FDE, did_సమితి_foldexpr, ఖాళీ,
			    {(char_u *)"0", (char_u *)ఖాళీ}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"foldignore",  "fdi",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN,
#నిర్వచితమైతే FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDI, did_సమితి_foldignore, ఖాళీ,
			    {(char_u *)"#", (char_u *)ఖాళీ}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"foldlevel",   "fdl",  P_అంకె|P_VI_DEF|P_RWIN,
#నిర్వచితమైతే FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDL, did_సమితి_foldlevel, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"foldlevelstart","fdls", P_అంకె|P_VI_DEF|P_CURSWANT,
#నిర్వచితమైతే FEAT_FOLDING
			    (char_u *)&p_fdls, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)-1L, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"foldmarker",  "fmr",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|
						    P_RWIN|P_ONECOMMA|P_NODUP,
#నిర్వచితమైతే FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FMR, did_సమితి_foldmarker, ఖాళీ,
			    {(char_u *)"{{{,}}}", (char_u *)ఖాళీ}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"foldmethod",  "fdm",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN,
#నిర్వచితమైతే FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDM, did_సమితి_foldmethod, expand_సమితి_foldmethod,
			    {(char_u *)"manual", (char_u *)ఖాళీ}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"foldminlines","fml",  P_అంకె|P_VI_DEF|P_RWIN,
#నిర్వచితమైతే FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FML, did_సమితి_foldminlines, ఖాళీ,
			    {(char_u *)1L, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"foldnestmax", "fdn",  P_అంకె|P_VI_DEF|P_RWIN,
#నిర్వచితమైతే FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDN, did_సమితి_foldnestmax, ఖాళీ,
			    {(char_u *)20L, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"foldopen",    "fdo",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_CURSWANT,
#నిర్వచితమైతే FEAT_FOLDING
			    (char_u *)&p_fdo, PV_NONE, did_సమితి_foldopen, expand_సమితి_foldopen,
		 {(char_u *)"block,hor,mark,percent,quickfix,search,tag,undo",
						 (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"foldtext",    "fdt",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN|P_MLE,
#ఒకవేళ defined(FEAT_FOLDING) && defined(FEAT_EVAL)
			    (char_u *)VAR_WIN, PV_FDT, did_సమితి_optexpr, ఖాళీ,
			    {(char_u *)"foldtext()", (char_u *)ఖాళీ}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"formatexpr", "fex",   P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_MLE,
#నిర్వచితమైతే FEAT_EVAL
			    (char_u *)&p_fex, PV_FEX, did_సమితి_optexpr, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"formatlistpat","flp", P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)&p_flp, PV_FLP, ఖాళీ, ఖాళీ,
			    {(char_u *)"^\\s*\\d\\+[\\]:.)}\\t ]\\s*",
						 (char_u *)0L} SCTX_INIT},
    {"formatoptions","fo",  P_STRING|P_ALLOCED|P_VIM|P_FLAGLIST,
			    (char_u *)&p_fo, PV_FO, did_సమితి_formatoptions, expand_సమితి_formatoptions,
			    {(char_u *)DFLT_FO_VI, (char_u *)DFLT_FO_VIM}
			    SCTX_INIT},
    {"formatprg",   "fp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_fp, PV_FP, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"fsync",       "fs",   P_BOOL|P_SECURE|P_VI_DEF,
#నిర్వచితమైతే HAVE_FSYNC
			    (char_u *)&p_fs, PV_FS, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"gdefault",    "gd",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_gd, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"graphic",	    "gr",   P_BOOL|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"grepformat",  "gfm",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#నిర్వచితమైతే FEAT_QUICKFIX
			    (char_u *)&p_gefm, PV_GEFM, ఖాళీ, ఖాళీ,
			    {(char_u *)DFLT_GREPFORMAT, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"grepprg",	    "gp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#నిర్వచితమైతే FEAT_QUICKFIX
			    (char_u *)&p_gp, PV_GP, ఖాళీ, ఖాళీ,
			    {
# ఒకవేళ defined(MSWIN)
			    // may be changed to "grep -n" in os_win32.c
			    (char_u *)"findstr /n",
# elif defined(UNIX)
			    // Add an extra file name so that grep will always
			    // insert a file name in the match line.
			    (char_u *)"grep -n $* /dev/null",
# elif defined(VMS)
			    (char_u *)"SEARCH/NUMBERS ",
# లేకపోతే
			    (char_u *)"grep -n ",
# endif
			    (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"guicursor",    "gcr", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#నిర్వచితమైతే CURSOR_SHAPE
			    (char_u *)&p_guicursor, PV_NONE,
			    did_సమితి_guicursor, ఖాళీ,
			    {
# ifdef FEAT_GUI
				(char_u *)"n-v-c:block-Cursor/lCursor,ve:ver35-Cursor,o:hor50-Cursor,i-ci:ver25-Cursor/lCursor,r-cr:hor20-Cursor/lCursor,sm:block-Cursor-blinkwait175-blinkoff150-blinkon175",
# లేకపోతే	// Win32 console
				(char_u *)"n-v-c:block,o:hor50,i-ci:hor15,r-cr:hor30,sm:block",
# endif
				    (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"guifont",	    "gfn",  P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP
#ఒకవేళ !defined(FEAT_GUI_GTK)
				|P_COLON
#అయితే_ముగింపు
				,
#నిర్వచితమైతే FEAT_GUI
			    (char_u *)&p_guifont, PV_NONE, did_సమితి_guifont, expand_సమితి_guifont,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"guifontset",  "gfs",  P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA,
#ఒకవేళ defined(FEAT_GUI) && defined(FEAT_XFONTSET)
			    (char_u *)&p_guifontset, PV_NONE,
			    did_సమితి_guifontset, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"guifontwide", "gfw",  P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP
#ఒకవేళ !defined(FEAT_GUI_GTK)
				|P_COLON
#అయితే_ముగింపు
				,
#ఒకవేళ defined(FEAT_GUI)
			    (char_u *)&p_guifontwide, PV_NONE,
			    did_సమితి_guifontwide, expand_సమితి_guifont,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},


    {"guiheadroom", "ghr",  P_అంకె|P_VI_DEF,
#ఒకవేళ (defined(FEAT_GUI_GTK) && !defined(USE_GTK4)) || defined(FEAT_GUI_X11)
			    (char_u *)&p_ghr, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)50L, (char_u *)0L} SCTX_INIT},

    {"guiligatures", "gli", P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP,
#ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
			    (char_u *)&p_guiligatures, PV_NONE,
			    did_సమితి_guiligatures, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},

    {"guioptions",  "go",   P_STRING|P_VI_DEF|P_RALL|P_FLAGLIST,
#ఒకవేళ defined(FEAT_GUI)
			    (char_u *)&p_go, PV_NONE, did_సమితి_guioptions, expand_సమితి_guioptions,
# ఒకవేళ defined(UNIX)
			    {(char_u *)"aegimrLtT", (char_u *)0L}
# లేకపోతే
			    {(char_u *)"egmrLtT", (char_u *)0L}
# endif
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"guipty",	    ఖాళీ,   P_BOOL|P_VI_DEF,
#ఒకవేళ defined(FEAT_GUI)
			    (char_u *)&p_guipty, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"guitablabel",  "gtl", P_STRING|P_VI_DEF|P_RWIN|P_MLE,
#ఒకవేళ defined(FEAT_GUI_TABLINE)
			    (char_u *)&p_gtl, PV_NONE, did_సమితి_guitablabel, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"guitabtooltip",  "gtt", P_STRING|P_VI_DEF|P_RWIN|P_MLE,
#ఒకవేళ defined(FEAT_GUI_TABLINE)
			    (char_u *)&p_gtt, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"hardtabs",    "ht",   P_అంకె|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"helpfile",    "hf",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_hf, PV_NONE, did_సమితి_helpfile, ఖాళీ,
			    {(char_u *)DFLT_HELPFILE, (char_u *)0L}
			    SCTX_INIT},
    {"helpheight",  "hh",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_hh, PV_NONE,
			    did_సమితి_winheight_helpheight, ఖాళీ,
			    {(char_u *)20L, (char_u *)0L} SCTX_INIT},
    {"helplang",    "hlg",  P_STRING|P_VI_DEF|P_ONECOMMA,
#నిర్వచితమైతే FEAT_MULTI_LANG
			    (char_u *)&p_hlg, PV_NONE, did_సమితి_helplang, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"hidden",	    "hid",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_hid, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"highlight",   "hl",   P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_hl, PV_NONE, did_సమితి_highlight, expand_సమితి_highlight,
			    {(char_u *)HIGHLIGHT_INIT, (char_u *)0L}
			    SCTX_INIT},
    {"history",	    "hi",   P_అంకె|P_VIM,
			    (char_u *)&p_hi, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)200L} SCTX_INIT},
    {"hkmap",	    "hk",   P_BOOL|P_VI_DEF|P_VIM,
#నిర్వచితమైతే FEAT_RIGHTLEFT
			    (char_u *)&p_hkmap, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"hkmapp",	    "hkp",  P_BOOL|P_VI_DEF|P_VIM,
#నిర్వచితమైతే FEAT_RIGHTLEFT
			    (char_u *)&p_hkmapp, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"hlsearch",    "hls",  P_BOOL|P_VI_DEF|P_VIM|P_RALL|P_HLONLY,
#ఒకవేళ defined(FEAT_శోధన_EXTRA)
			    (char_u *)&p_hls, PV_NONE, did_సమితి_hlsearch, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"icon",	    ఖాళీ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_icon, PV_NONE, did_సమితి_title_icon, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"iconstring",  ఖాళీ,   P_STRING|P_VI_DEF|P_MLE,
			    (char_u *)&p_iconstring, PV_NONE,
			    did_సమితి_iconstring, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"ignorecase",  "ic",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ic, PV_NONE, did_సమితి_ignorecase, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"imactivatefunc","imaf",P_STRING|P_VI_DEF|P_SECURE|P_FUNC,
#ఒకవేళ defined(FEAT_EVAL) && \
	(defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
			    (char_u *)&p_imaf, PV_NONE,
			    did_సమితి_imactivatefunc, ఖాళీ,
			    {(char_u *)"", (char_u *)ఖాళీ}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"imactivatekey","imak",P_STRING|P_VI_DEF,
#ఒకవేళ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
			    (char_u *)&p_imak, PV_NONE,
			    did_సమితి_imactivatekey, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"imageprotocol", "ipc",P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_COLON,
#నిర్వచితమైతే FEAT_IMAGE
			    (char_u *)&p_ipc, PV_NONE, did_సమితి_imageprotocol, expand_సమితి_imageprotocol,
			    {(char_u *)"kitty:kitty,foot:sixel,ghostty:kitty,wezterm:kitty,xterm:sixel", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"imcmdline",   "imc",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_imcmdline, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"imdisable",   "imd",  P_BOOL|P_VI_DEF,
#ఒకవేళ defined(HAVE_ఆదానం_పద్ధతి)
			    (char_u *)&p_imdisable, PV_NONE,
			    did_సమితి_imdisable, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L}
			    SCTX_INIT},
    {"iminsert",    "imi",  P_అంకె|P_VI_DEF,
			    (char_u *)&p_iminsert, PV_IMI, did_సమితి_iminsert, ఖాళీ,
			    {(char_u *)B_IMODE_NONE, (char_u *)0L}
			    SCTX_INIT},
    {"imsearch",    "ims",  P_అంకె|P_VI_DEF,
			    (char_u *)&p_imsearch, PV_IMS, did_సమితి_imsearch, ఖాళీ,
			    {(char_u *)B_IMODE_USE_చొప్పించు, (char_u *)0L}
			    SCTX_INIT},
    {"imstatusfunc","imsf",P_STRING|P_VI_DEF|P_SECURE|P_FUNC,
#ఒకవేళ defined(FEAT_EVAL) && \
	(defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
			    (char_u *)&p_imsf, PV_NONE, did_సమితి_imstatusfunc, ఖాళీ,
			    {(char_u *)"", (char_u *)ఖాళీ}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"imstyle",	    "imst", P_అంకె|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
			    (char_u *)&p_imst, PV_NONE, did_సమితి_imstyle, ఖాళీ,
			    {(char_u *)IM_OVER_THE_SPOT, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"include",	    "inc",  P_STRING|P_ALLOCED|P_VI_DEF,
#నిర్వచితమైతే FEAT_కనుగొను_ID
			    (char_u *)&p_inc, PV_INC, ఖాళీ, ఖాళీ,
			    {(char_u *)"^\\s*#\\s*include", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"includeexpr", "inex", P_STRING|P_ALLOCED|P_VI_DEF|P_MLE,
#ఒకవేళ defined(FEAT_కనుగొను_ID) && defined(FEAT_EVAL)
			    (char_u *)&p_inex, PV_INEX, did_సమితి_optexpr, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"incsearch",   "is",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_is, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"indentexpr", "inde",  P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_MLE,
#ఒకవేళ defined(FEAT_EVAL)
			    (char_u *)&p_inde, PV_INDE, did_సమితి_optexpr, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"indentkeys", "indk",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ఒకవేళ defined(FEAT_EVAL)
			    (char_u *)&p_indk, PV_INDK, ఖాళీ, ఖాళీ,
			    {INDENTKEYS_DEFAULT, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"infercase",   "inf",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_inf, PV_INF, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"insertmode",  "im",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_im, PV_NONE, did_సమితి_insertmode, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"isfname",	    "isf",  P_STRING|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_isf, PV_NONE, did_సమితి_isopt, ఖాళీ,
			    {
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
				// Excluded are: & and ^ are special in cmd.exe
				// ( and ) are used in text separating fnames
			    (char_u *)"@,48-57,/,\\,.,-,_,+,,,#,$,%,{,},[,],:,@-@,!,~,=",
#లేకపోతే
# ఒకవేళ defined(AMIGA)
			    (char_u *)"@,48-57,/,.,-,_,+,,,$,:",
# elif defined(VMS)
			    (char_u *)"@,48-57,/,.,-,_,+,,,#,$,%,<,>,[,],:,;,~",
# లేకపోతే // UNIX et al.
			    (char_u *)"@,48-57,/,.,-,_,+,,,#,$,%,~,=",
# endif
#అయితే_ముగింపు
				(char_u *)0L} SCTX_INIT},
    {"isident",	    "isi",  P_STRING|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_isi, PV_NONE, did_సమితి_isopt, ఖాళీ,
			    {
#ఒకవేళ defined(MSWIN)
			    (char_u *)"@,48-57,_,128-167,224-235",
#లేకపోతే
			    (char_u *)"@,48-57,_,192-255",
#అయితే_ముగింపు
				(char_u *)0L} SCTX_INIT},
    {"iskeyword",   "isk",  P_STRING|P_ALLOCED|P_VIM|P_COMMA|P_NODUP,
			    (char_u *)&p_isk, PV_ISK, did_సమితి_iskeyword, ఖాళీ,
			    {
				(char_u *)"@,48-57,_",
#ఒకవేళ defined(MSWIN)
				(char_u *)"@,48-57,_,128-167,224-235"
#లేకపోతే
				ISK_LATIN1
#అయితే_ముగింపు
			    } SCTX_INIT},
    {"isprint",	    "isp",  P_STRING|P_VI_DEF|P_RALL|P_COMMA|P_NODUP,
			    (char_u *)&p_isp, PV_NONE, did_సమితి_isopt, ఖాళీ,
			    {
#ఒకవేళ defined(MSWIN) || defined(VMS)
			    (char_u *)"@,~-255",
#లేకపోతే
			    ISP_LATIN1,
#అయితే_ముగింపు
				(char_u *)0L} SCTX_INIT},
    {"joinspaces",  "js",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_js, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"jumpoptions", "jop",  P_STRING|P_VI_DEF|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_jop, PV_NONE, did_సమితి_jumpoptions, expand_సమితి_jumpoptions,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"key",	    ఖాళీ,   P_STRING|P_ALLOCED|P_VI_DEF|P_NO_MKRC|P_NO_CMD_EXPAND,
#నిర్వచితమైతే FEAT_CRYPT
			    (char_u *)&p_కీలకం, PV_కీలకం, did_సమితి_cryptkey, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"keymap",	    "kmp",  P_STRING|P_ALLOCED|P_VI_DEF|P_RBUF|P_RSTAT|P_NFNAME|P_PRI_MKRC,
#నిర్వచితమైతే FEAT_KEYMAP
			    (char_u *)&p_keymap, PV_KMAP, did_సమితి_keymap, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"keymodel",    "km",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_km, PV_NONE, did_సమితి_keymodel, expand_సమితి_keymodel,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"keyprotocol", "kpc",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_kpc, PV_NONE, did_సమితి_keyprotocol, expand_సమితి_keyprotocol,
			    {(char_u *)"kitty:kitty,foot:kitty,ghostty:kitty,wezterm:kitty,xterm:mok2", (char_u *)0L}
			    SCTX_INIT},
    {"keywordprg",  "kp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_kp, PV_KP, ఖాళీ, ఖాళీ,
			    {
#ఒకవేళ defined(MSWIN)
			    (char_u *)":help",
#లేదావేళ defined(VMS)
			    (char_u *)"help",
#లేదావేళ defined(USEMAN_S)
			    (char_u *)"man -s",
#లేకపోతే
			    (char_u *)"man",
#అయితే_ముగింపు
				(char_u *)0L} SCTX_INIT},
    {"langmap",     "lmap", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_SECURE,
#నిర్వచితమైతే FEAT_LANGMAP
			    (char_u *)&p_langmap, PV_NONE, did_సమితి_langmap, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"langmenu",    "lm",   P_STRING|P_VI_DEF|P_NFNAME,
#ఒకవేళ defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
			    (char_u *)&p_lm, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"langnoremap",  "lnr",   P_BOOL|P_VI_DEF,
#నిర్వచితమైతే FEAT_LANGMAP
			    (char_u *)&p_lnr, PV_NONE, did_సమితి_langnoremap, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"langremap",  "lrm",   P_BOOL|P_VI_DEF,
#నిర్వచితమైతే FEAT_LANGMAP
			    (char_u *)&p_lrm, PV_NONE, did_సమితి_langremap, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"laststatus",  "ls",   P_అంకె|P_VI_DEF|P_RALL,
			    (char_u *)&p_ls, PV_NONE, did_సమితి_laststatus, ఖాళీ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"lazyredraw",  "lz",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_lz, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"lhistory", "lhi",	    P_అంకె|P_VI_DEF,
#నిర్వచితమైతే FEAT_QUICKFIX
			    (char_u *)VAR_WIN, PV_LHI, did_సమితి_xhistory, ఖాళీ,
			    {(char_u *)10L, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"linebreak",   "lbr",  P_BOOL|P_VI_DEF|P_RWIN,
#నిర్వచితమైతే FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_LBR, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"lines",	    ఖాళీ,   P_అంకె|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RCLR,
			    (char_u *)&అడ్డువరుసలు, PV_NONE, ఖాళీ, ఖాళీ,
			    {
#ఒకవేళ defined(MSWIN)
			    (char_u *)25L,
#లేకపోతే
			    (char_u *)24L,
#అయితే_ముగింపు
					    (char_u *)0L} SCTX_INIT},
    {"linespace",   "lsp",  P_అంకె|P_VI_DEF|P_RCLR,
#నిర్వచితమైతే FEAT_GUI
			    (char_u *)&p_linespace, PV_NONE,
			    did_సమితి_linespace, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_MSWIN
			    {(char_u *)1L, (char_u *)0L}
#లేకపోతే
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"lisp",	    ఖాళీ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_lisp, PV_LISP, did_సమితి_lisp, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"lispoptions", "lop",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_lop, PV_LOP, did_సమితి_lispoptions, expand_సమితి_lispoptions,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"lispwords",   "lw",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_lispwords, PV_LW, ఖాళీ, ఖాళీ,
			    {(char_u *)LISPWORD_విలువ, (char_u *)0L} SCTX_INIT},
    {"list",	    ఖాళీ,   P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_LIST, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"listchars",   "lcs",  P_STRING|P_VI_DEF|P_RALL|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_lcs, PV_LCS, did_సమితి_chars_option, expand_సమితి_chars_option,
			    {(char_u *)"eol:$", (char_u *)0L} SCTX_INIT},
    {"loadplugins", "lpl",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_lpl, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"luadll",      ఖాళీ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(DYNAMIC_LUA)
			    (char_u *)&p_luadll, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DYNAMIC_LUA_DLL, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"macatsui",    ఖాళీ,   P_BOOL|P_VI_DEF|P_RCLR,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"magic",	    ఖాళీ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_magic, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"makeef",	    "mef",  P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#నిర్వచితమైతే FEAT_QUICKFIX
			    (char_u *)&p_mef, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"makeencoding","menc", P_STRING|P_VI_DEF,
			    (char_u *)&p_menc, PV_MENC, did_సమితి_encoding, expand_సమితి_encoding,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"makeprg",	    "mp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#నిర్వచితమైతే FEAT_QUICKFIX
			    (char_u *)&p_mp, PV_MP, ఖాళీ, ఖాళీ,
# ifdef VMS
			    {(char_u *)"MMS", (char_u *)0L}
# లేకపోతే
			    {(char_u *)"make", (char_u *)0L}
# endif
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"matchpairs",  "mps",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_mps, PV_MPS, did_సమితి_matchpairs, ఖాళీ,
			    {(char_u *)"(:),{:},[:]", (char_u *)0L}
			    SCTX_INIT},
    {"matchtime",   "mat",  P_అంకె|P_VI_DEF,
			    (char_u *)&p_mat, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)5L, (char_u *)0L} SCTX_INIT},
    {"maxcombine",  "mco",  P_అంకె|P_VI_DEF|P_CURSWANT,
			    (char_u *)&p_mco, PV_NONE, did_సమితి_maxcombine, ఖాళీ,
			    {(char_u *)2, (char_u *)0L} SCTX_INIT},
    {"maxfuncdepth", "mfd", P_అంకె|P_VI_DEF,
#నిర్వచితమైతే FEAT_EVAL
			    (char_u *)&p_mfd, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)100L, (char_u *)0L} SCTX_INIT},
    {"maxmapdepth", "mmd",  P_అంకె|P_VI_DEF,
			    (char_u *)&p_mmd, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)1000L, (char_u *)0L} SCTX_INIT},
    {"maxmem",	    "mm",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_mm, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DFLT_MAXMEM, (char_u *)0L}
			    SCTX_INIT},
    {"maxmempattern","mmp", P_అంకె|P_VI_DEF,
			    (char_u *)&p_mmp, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)1000L, (char_u *)0L} SCTX_INIT},
    {"maxmemtot",   "mmt",  P_అంకె|P_VI_DEF,
			    (char_u *)&p_mmt, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DFLT_MAXMEMTOT, (char_u *)0L}
			    SCTX_INIT},
    {"maxsearchcount", "msc", P_అంకె|P_VI_DEF,
			    (char_u *)&p_msc, PV_NONE, did_సమితి_maxsearchcount, ఖాళీ,
			    {(char_u *)99L, (char_u *)0L} SCTX_INIT},
    {"menuitems",   "mis",  P_అంకె|P_VI_DEF,
#నిర్వచితమైతే FEAT_MENU
			    (char_u *)&p_mis, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)25L, (char_u *)0L} SCTX_INIT},
    {"mesg",	    ఖాళీ,   P_BOOL|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"messagesopt","mopt",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_COLON|P_NODUP,
			    (char_u *)&p_mopt, PV_NONE, did_సమితి_messagesopt, expand_సమితి_messagesopt,
			    {(char_u *)"hit-enter,history:500", (char_u *)ఖాళీ} SCTX_INIT},
    {"mkspellmem",  "msm",  P_STRING|P_VI_DEF|P_EXPAND|P_SECURE,
#నిర్వచితమైతే FEAT_SPELL
			    (char_u *)&p_msm, PV_NONE, did_సమితి_mkspellmem, ఖాళీ,
			    {(char_u *)"460000,2000,500", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"modeline",    "ml",   P_BOOL|P_VIM,
			    (char_u *)&p_ml, PV_ML, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"modelineexpr", "mle",  P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_mle, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"modelines",   "mls",  P_అంకె|P_VI_DEF,
			    (char_u *)&p_mls, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)5L, (char_u *)0L} SCTX_INIT},
    {"modelinestrict", "mlst", P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_mlstr, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"modifiable",  "ma",   P_BOOL|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_ma, PV_MA, did_సమితి_modifiable, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"modified",    "mod",  P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_mod, PV_MOD, did_సమితి_modified, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"more",	    ఖాళీ,   P_BOOL|P_VIM,
			    (char_u *)&p_మరిన్ని, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"mouse",	    ఖాళీ,   P_STRING|P_VI_DEF|P_FLAGLIST,
			    (char_u *)&p_mouse, PV_NONE, did_సమితి_mouse, expand_సమితి_mouse,
			    {
#ఒకవేళ defined(MSWIN)
				(char_u *)"a",
#లేకపోతే
				(char_u *)"",
#అయితే_ముగింపు
				(char_u *)0L} SCTX_INIT},
    {"mousefocus",   "mousef", P_BOOL|P_VI_DEF,
#నిర్వచితమైతే FEAT_GUI
			    (char_u *)&p_mousef, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"mousehide",   "mh",   P_BOOL|P_VI_DEF,
#నిర్వచితమైతే FEAT_GUI
			    (char_u *)&p_mh, PV_NONE, did_సమితి_mousehide, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"mousemodel",  "mousem", P_STRING|P_VI_DEF,
			    (char_u *)&p_mousem, PV_NONE, did_సమితి_mousemodel, expand_సమితి_mousemodel,
			    {
#ఒకవేళ defined(MSWIN)
				(char_u *)"popup",
#లేకపోతే
# ఒకవేళ defined(MACOS_X)
				(char_u *)"popup_setpos",
# లేకపోతే
				(char_u *)"extend",
# endif
#అయితే_ముగింపు
				(char_u *)0L} SCTX_INIT},
    {"mousemoveevent",   "mousemev",   P_BOOL|P_VI_DEF,
#నిర్వచితమైతే FEAT_GUI
			    (char_u *)&p_mousemev, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"mouseshape",  "mouses",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#నిర్వచితమైతే FEAT_MOUSESHAPE
			    (char_u *)&p_mouseshape, PV_NONE,
			    did_సమితి_mouseshape, ఖాళీ,
			    {(char_u *)"i-r:beam,s:updown,sd:udsizing,vs:leftright,vd:lrsizing,m:no,ml:up-arrow,v:rightup-arrow", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"mousetime",   "mouset",	P_అంకె|P_VI_DEF,
			    (char_u *)&p_mouset, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)500L, (char_u *)0L} SCTX_INIT},
    {"mzquantum",  "mzq",   P_అంకె,
#నిర్వచితమైతే FEAT_MZSCHEME
			    (char_u *)&p_mzq, PV_NONE, did_సమితి_mzquantum, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)100L, (char_u *)100L} SCTX_INIT},
    {"mzschemedll", ఖాళీ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(DYNAMIC_MZSCHEME)
			    (char_u *)&p_mzschemedll, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DYNAMIC_MZSCH_DLL, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"mzschemegcdll", ఖాళీ, P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(DYNAMIC_MZSCHEME)
			    (char_u *)&p_mzschemegcdll, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DYNAMIC_MZGC_DLL, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"novice",	    ఖాళీ,   P_BOOL|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"nrformats",   "nf",   P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_nf, PV_NF, did_సమితి_nrformats, expand_సమితి_nrformats,
			    {(char_u *)"bin,octal,hex", (char_u *)0L}
			    SCTX_INIT},
    {"number",	    "nu",   P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_NU,
			    did_సమితి_సంఖ్య_relativenumber, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"numberwidth", "nuw",  P_అంకె|P_RWIN|P_VIM,
#నిర్వచితమైతే FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_NUW, did_సమితి_numberwidth, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)8L, (char_u *)4L} SCTX_INIT},
    {"omnifunc",    "ofu",  P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#నిర్వచితమైతే FEAT_COMPL_FUNC
			    (char_u *)&p_ofu, PV_OFU, did_సమితి_omnifunc, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"open",	    ఖాళీ,   P_BOOL|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"opendevice",  "odev", P_BOOL|P_VI_DEF,
#ఒకవేళ defined(MSWIN)
			    (char_u *)&p_odev, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)FALSE}
			    SCTX_INIT},
    {"operatorfunc", "opfunc", P_STRING|P_VI_DEF|P_SECURE|P_FUNC,
			    (char_u *)&p_opfunc, PV_NONE,
			    did_సమితి_operatorfunc, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"optimize",    "opt",  P_BOOL|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"osctimeoutlen", "ost", P_అంకె|P_VI_DEF,
			    (char_u *)&p_ost, PV_NONE, did_సమితి_osctimeoutlen, ఖాళీ,
			    {(char_u *)1000, (char_u *)0L} SCTX_INIT},
    {"osfiletype",  "oft",  P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"packpath",    "pp",   P_STRING|P_VI_DEF|P_EXPAND|P_ONECOMMA|P_NODUP
								    |P_SECURE,
			    (char_u *)&p_pp, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DFLT_RUNTIMEPATH, (char_u *)0L}
			    SCTX_INIT},
    {"paragraphs",  "para", P_STRING|P_VI_DEF,
			    (char_u *)&p_para, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"IPLPPPQPP TPHPLIPpLpItpplpipbp",
				(char_u *)0L} SCTX_INIT},
    {"paste",	    ఖాళీ,   P_BOOL|P_VI_DEF|P_PRI_MKRC,
			    (char_u *)&p_paste, PV_NONE, did_సమితి_paste, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"pastetoggle", "pt",   P_STRING|P_VI_DEF,
			    (char_u *)&p_pt, PV_NONE, did_సమితి_pastetoggle, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"patchexpr",   "pex",  P_STRING|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(FEAT_DIFF) && defined(FEAT_EVAL)
			    (char_u *)&p_pex, PV_NONE, did_సమితి_optexpr, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"patchmode",   "pm",   P_STRING|P_VI_DEF|P_NFNAME,
			    (char_u *)&p_pm, PV_NONE,
			    did_సమితి_backupext_or_patchmode, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"path",	    "pa",   P_STRING|P_EXPAND|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_మార్గం, PV_మార్గం, ఖాళీ, ఖాళీ,
			    {
#ఒకవేళ defined(AMIGA) || defined(MSWIN)
			    (char_u *)".,,",
#లేకపోతే
			    (char_u *)".,/usr/include,,",
#అయితే_ముగింపు
				(char_u *)0L} SCTX_INIT},
    {"perldll",     ఖాళీ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(DYNAMIC_PERL)
			    (char_u *)&p_perldll, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DYNAMIC_PERL_DLL, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"preserveindent", "pi", P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_pi, PV_PI, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"previewheight", "pvh", P_అంకె|P_VI_DEF,
#ఒకవేళ defined(FEAT_QUICKFIX)
			    (char_u *)&p_pvh, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)12L, (char_u *)0L} SCTX_INIT},
    {"previewpopup", "pvp", P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON,
#నిర్వచితమైతే FEAT_PROP_POPUP
			    (char_u *)&p_pvp, PV_NONE, did_సమితి_previewpopup, expand_సమితి_previewpopup,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"previewwindow", "pvw", P_BOOL|P_VI_DEF|P_RSTAT|P_NOGLOB,
#ఒకవేళ defined(FEAT_QUICKFIX)
			    (char_u *)VAR_WIN, PV_PVW, did_సమితి_previewwindow, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"printdevice", "pdev", P_STRING|P_VI_DEF|P_SECURE,
#నిర్వచితమైతే FEAT_PRINTER
			    (char_u *)&p_pdev, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"printencoding", "penc", P_STRING|P_VI_DEF,
#నిర్వచితమైతే FEAT_POSTSCRIPT
			    (char_u *)&p_penc, PV_NONE,
			    did_సమితి_printencoding, expand_సమితి_encoding,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"printexpr", "pexpr",  P_STRING|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
			    (char_u *)&p_pexpr, PV_NONE, did_సమితి_optexpr, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"printfont", "pfn",    P_STRING|P_VI_DEF,
#నిర్వచితమైతే FEAT_PRINTER
			    (char_u *)&p_pfn, PV_NONE, ఖాళీ, ఖాళీ,
			    {
# ifdef MSWIN
				(char_u *)"Courier_New:h10",
# elif defined(FEAT_PRINT_PANGO)
				(char_u *)"Courier 10",
# లేకపోతే
				(char_u *)"courier",
# endif
				(char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"printheader", "pheader",  P_STRING|P_VI_DEF|P_GETTEXT|P_MLE,
#నిర్వచితమైతే FEAT_PRINTER
			    (char_u *)&p_శీర్షిక, PV_NONE, ఖాళీ, ఖాళీ,
			    // untranslated to avoid problems when 'encoding'
			    // is changed
			    {(char_u *)"%<%f%h%m%=Page %N", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"printmbcharset", "pmbcs",  P_STRING|P_VI_DEF,
#ఒకవేళ defined(FEAT_POSTSCRIPT)
			    (char_u *)&p_pmcs, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"printmbfont", "pmbfn",  P_STRING|P_VI_DEF,
#ఒకవేళ defined(FEAT_POSTSCRIPT)
			    (char_u *)&p_pmfn, PV_NONE, parse_printmbfont, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"printoptions", "popt", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#నిర్వచితమైతే FEAT_PRINTER
			    (char_u *)&p_popt, PV_NONE, parse_printoptions, expand_సమితి_printoptions,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"prompt",	    ఖాళీ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_prompt, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"pumborder",   "pb",   P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON|P_SECURE,
			    (char_u *)&p_pb, PV_NONE, did_సమితి_pumborder, expand_సమితి_pumborder,
			    {(char_u *)"", (char_u *)ఖాళీ} SCTX_INIT},
    {"pumheight",   "ph",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_ph, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"pummaxwidth", "pmw",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_pmw, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"pumopt",	    ఖాళీ,   P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON,
			    (char_u *)&p_pumopt, PV_NONE,
			    did_సమితి_pumopt, expand_సమితి_pumopt,
			    {(char_u *)"", (char_u *)ఖాళీ} SCTX_INIT},
    {"pumwidth",    "pw",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_pw, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)15L, (char_u *)15L} SCTX_INIT},
    {"pythondll",   ఖాళీ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(DYNAMIC_PYTHON)
			    (char_u *)&p_pydll, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DYNAMIC_PYTHON_DLL, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"pythonhome",  ఖాళీ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(FEAT_PYTHON)
			    (char_u *)&p_pyhome, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"pythonthreedll",  ఖాళీ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(DYNAMIC_PYTHON3)
			    (char_u *)&p_py3dll, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DYNAMIC_PYTHON3_DLL, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"pythonthreehome", ఖాళీ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(FEAT_PYTHON3)
			    (char_u *)&p_py3home, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"pyxversion", "pyx",   P_అంకె|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
			    (char_u *)&p_pyx, PV_NONE, did_సమితి_pyxversion, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)DEFAULT_PYTHON_VER, (char_u *)0L}
			    SCTX_INIT},
    {"quickfixtextfunc", "qftf", P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_SECURE|P_FUNC,
#ఒకవేళ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
			    (char_u *)&p_qftf, PV_NONE,
			    did_సమితి_quickfixtextfunc, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)ఖాళీ}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"quoteescape", "qe",   P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)&p_qe, PV_QE, ఖాళీ, ఖాళీ,
			    {(char_u *)"\\", (char_u *)0L} SCTX_INIT},
    {"readonly",    "ro",   P_BOOL|P_VI_DEF|P_RSTAT|P_NOGLOB,
			    (char_u *)&p_ro, PV_RO, did_సమితి_readonly, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"redraw",	    ఖాళీ,   P_BOOL|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"redrawtime",  "rdt",  P_అంకె|P_VI_DEF,
#నిర్వచితమైతే FEAT_RELTIME
			    (char_u *)&p_rdt, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)2000L, (char_u *)0L} SCTX_INIT},
    {"regexpengine", "re",  P_అంకె|P_VI_DEF,
			    (char_u *)&p_re, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"relativenumber", "rnu", P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_RNU,
			    did_సమితి_సంఖ్య_relativenumber, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"remap",	    ఖాళీ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_remap, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"renderoptions", "rop", P_STRING|P_ONECOMMA|P_RCLR|P_VI_DEF,
#నిర్వచితమైతే FEAT_RENDER_OPTIONS
			    (char_u *)&p_rop, PV_NONE, did_సమితి_renderoptions, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"report",	    ఖాళీ,   P_అంకె|P_VI_DEF,
			    (char_u *)&p_report, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)2L, (char_u *)0L} SCTX_INIT},
    {"restorescreen", "rs", P_BOOL|P_VI_DEF,
#నిర్వచితమైతే MSWIN
			    (char_u *)&p_rs, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"revins",	    "ri",   P_BOOL|P_VI_DEF|P_VIM,
#నిర్వచితమైతే FEAT_RIGHTLEFT
			    (char_u *)&p_ri, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"rightleft",   "rl",   P_BOOL|P_VI_DEF|P_RWIN,
#నిర్వచితమైతే FEAT_RIGHTLEFT
			    (char_u *)VAR_WIN, PV_RL, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"rightleftcmd", "rlc", P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN,
#నిర్వచితమైతే FEAT_RIGHTLEFT
			    (char_u *)VAR_WIN, PV_RLC, did_సమితి_rightleftcmd, expand_సమితి_rightleftcmd,
			    {(char_u *)"search", (char_u *)ఖాళీ}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"rubydll",     ఖాళీ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(DYNAMIC_RUBY)
			    (char_u *)&p_rubydll, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DYNAMIC_RUBY_DLL, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"ruler",	    "ru",   P_BOOL|P_VIM|P_RSTAT,
			    (char_u *)&p_ru, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"rulerformat", "ruf",  P_STRING|P_VI_DEF|P_ALLOCED|P_RSTAT|P_MLE,
#నిర్వచితమైతే FEAT_STL_OPT
			    (char_u *)&p_ruf, PV_NONE, did_సమితి_rulerformat, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"runtimepath", "rtp",  P_STRING|P_VI_DEF|P_EXPAND|P_ONECOMMA|P_NODUP
								    |P_SECURE,
			    (char_u *)&p_rtp, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DFLT_RUNTIMEPATH, (char_u *)0L}
			    SCTX_INIT},
    {"scroll",	    "scr",  P_అంకె|P_NO_MKRC|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_SCROLL, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"scrollbind",  "scb",  P_BOOL|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_SCBIND, did_సమితి_scrollbind, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"scrollfocus", "scf",  P_BOOL|P_VI_DEF,
#ఒకవేళ defined(MSWIN) && defined(FEAT_GUI)
			    (char_u *)&p_scf, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"scrolljump",  "sj",   P_అంకె|P_VI_DEF|P_VIM,
			    (char_u *)&p_sj, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"scrolloff",   "so",   P_అంకె|P_VI_DEF|P_VIM|P_RALL,
			    (char_u *)&p_so, PV_SO, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"scrolloffpad", "sop", P_అంకె|P_VI_DEF|P_VIM|P_RALL,
				(char_u *)&p_sop, PV_SOP, ఖాళీ, ఖాళీ,
				{(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"scrollopt",   "sbo",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_sbo, PV_NONE, did_సమితి_scrollopt, expand_సమితి_scrollopt,
			    {(char_u *)"ver,jump", (char_u *)0L}
			    SCTX_INIT},
    {"sections",    "sect", P_STRING|P_VI_DEF,
			    (char_u *)&p_sections, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"SHNHH HUnhsh", (char_u *)0L}
			    SCTX_INIT},
    {"secure",	    ఖాళీ,   P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_secure, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"selection",   "sel",  P_STRING|P_VI_DEF,
			    (char_u *)&p_sel, PV_NONE, did_సమితి_selection, expand_సమితి_selection,
			    {(char_u *)"inclusive", (char_u *)0L}
			    SCTX_INIT},
    {"selectmode",  "slm",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_slm, PV_NONE, did_సమితి_selectmode, expand_సమితి_selectmode,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"sessionoptions", "ssop", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#నిర్వచితమైతే FEAT_SESSION
			    (char_u *)&p_ssop, PV_NONE,
			    did_సమితి_sessionoptions, expand_సమితి_sessionoptions,
	 {(char_u *)"blank,buffers,curdir,folds,help,options,tabpages,winsize,terminal",
							       (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"shell",	    "sh",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_sh, PV_NONE, ఖాళీ, ఖాళీ,
			    {
#నిర్వచితమైతే VMS
			    (char_u *)"-",
#లేకపోతే
# ఒకవేళ defined(MSWIN)
			    (char_u *)"",	// set in set_init_1()
# లేకపోతే
			    (char_u *)"sh",
# endif
#అయితే_ముగింపు // VMS
				(char_u *)0L} SCTX_INIT},
    {"shellcmdflag","shcf", P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_shcf, PV_NONE, ఖాళీ, ఖాళీ,
			    {
#ఒకవేళ defined(MSWIN)
			    (char_u *)"/c",
#లేకపోతే
			    (char_u *)"-c",
#అయితే_ముగింపు
				(char_u *)0L} SCTX_INIT},
    {"shellpipe",   "sp",   P_STRING|P_VI_DEF|P_SECURE,
#నిర్వచితమైతే FEAT_QUICKFIX
			    (char_u *)&p_sp, PV_NONE, did_సమితి_shellpipe_redir, ఖాళీ,
			    {
# ఒకవేళ defined(UNIX)
			    (char_u *)"| tee",
# లేకపోతే
			    (char_u *)">",
# endif
				(char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"shellquote",  "shq",  P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_shq, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"shellredir",  "srr",  P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_srr, PV_NONE, did_సమితి_shellpipe_redir, ఖాళీ,
			    {(char_u *)">", (char_u *)0L} SCTX_INIT},
    {"shellslash",  "ssl",   P_BOOL|P_VI_DEF,
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
			    (char_u *)&p_ssl, PV_NONE, did_సమితి_shellslash, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"shelltemp",   "stmp", P_BOOL,
			    (char_u *)&p_stmp, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"shelltype",   "st",   P_అంకె|P_VI_DEF,
#నిర్వచితమైతే AMIGA
			    (char_u *)&p_st, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"shellxescape", "sxe", P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_sxe, PV_NONE, ఖాళీ, ఖాళీ,
			    {
#ఒకవేళ defined(MSWIN)
			    (char_u *)"\"&|<>()@^",
#లేకపోతే
			    (char_u *)"",
#అయితే_ముగింపు
				(char_u *)0L} SCTX_INIT},
    {"shellxquote", "sxq",  P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_sxq, PV_NONE, ఖాళీ, ఖాళీ,
			    {
#ఒకవేళ defined(UNIX) && defined(USE_SYSTEM)
			    (char_u *)"\"",
#లేకపోతే
			    (char_u *)"",
#అయితే_ముగింపు
				(char_u *)0L} SCTX_INIT},
    {"shiftround",  "sr",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sr, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"shiftwidth",  "sw",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_sw, PV_SW,
			    did_సమితి_shiftwidth_tabstop, ఖాళీ,
			    {(char_u *)8L, (char_u *)0L} SCTX_INIT},
    {"shortmess",   "shm",  P_STRING|P_VIM|P_FLAGLIST,
			    (char_u *)&p_shm, PV_NONE, did_సమితి_shortmess, expand_సమితి_shortmess,
			    {(char_u *)"S", (char_u *)"filnxtToOS"}
			    SCTX_INIT},
    {"shortname",   "sn",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sn, PV_SN, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"showbreak",   "sbr",  P_STRING|P_VI_DEF|P_RALL,
#నిర్వచితమైతే FEAT_LINEBREAK
			    (char_u *)&p_sbr, PV_SBR, did_సమితి_showbreak, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"showcmd",	    "sc",   P_BOOL|P_VIM,
			    (char_u *)&p_sc, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"showcmdloc",  "sloc", P_STRING|P_RSTAT,
			    (char_u *)&p_sloc, PV_NONE, did_సమితి_showcmdloc, expand_సమితి_showcmdloc,
			    {(char_u *)"last", (char_u *)"last"} SCTX_INIT},
    {"showfulltag", "sft",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_sft, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"showmatch",   "sm",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sm, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"showmode",    "smd",  P_BOOL|P_VIM,
			    (char_u *)&p_smd, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"showtabline", "stal", P_అంకె|P_VI_DEF|P_RALL,
			    (char_u *)&p_stal, PV_NONE, did_సమితి_showtabline, ఖాళీ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
#ఒకవేళ defined(FEAT_TABPANEL)
    {"showtabpanel", "stpl", P_అంకె|P_RALL,
			    (char_u *)&p_stpl, PV_NONE, did_సమితి_showtabpanel, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
#అయితే_ముగింపు
    {"sidescroll",  "ss",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_ss, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"sidescrolloff", "siso", P_అంకె|P_VI_DEF|P_VIM|P_RBUF,
			    (char_u *)&p_siso, PV_SISO, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"signcolumn",   "scl",  P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN,
#నిర్వచితమైతే FEAT_SIGNS
			    (char_u *)VAR_WIN, PV_SCL, did_సమితి_signcolumn, expand_సమితి_signcolumn,
			    {(char_u *)"auto", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"slowopen",    "slow", P_BOOL|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smartcase",   "scs",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_scs, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smartindent", "si",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_si, PV_SI, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smarttab",    "sta",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sta, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smoothscroll", "sms", P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_SMS, did_సమితి_smoothscroll, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"softtabstop", "sts",  P_అంకె|P_VI_DEF|P_VIM,
			    (char_u *)&p_sts, PV_STS, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"sourceany",   ఖాళీ,   P_BOOL|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"spell",	    ఖాళీ,   P_BOOL|P_VI_DEF|P_RWIN|P_HLONLY,
#నిర్వచితమైతే FEAT_SPELL
			    (char_u *)VAR_WIN, PV_SPELL, did_సమితి_spell, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"spellcapcheck", "spc", P_STRING|P_ALLOCED|P_VI_DEF|P_RBUF|P_HLONLY,
#నిర్వచితమైతే FEAT_SPELL
			    (char_u *)&p_spc, PV_SPC, did_సమితి_spellcapcheck, ఖాళీ,
			    {(char_u *)"[.?!]\\_[\\])'\"	 ]\\+", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"spellfile",   "spf",  P_STRING|P_EXPAND|P_ALLOCED|P_VI_DEF|P_SECURE
								  |P_ONECOMMA,
#నిర్వచితమైతే FEAT_SPELL
			    (char_u *)&p_spf, PV_SPF, did_సమితి_spellfile, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"spelllang",   "spl",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA
						     |P_RBUF|P_HLONLY|P_EXPAND,
#నిర్వచితమైతే FEAT_SPELL
			    (char_u *)&p_spl, PV_SPL, did_సమితి_spelllang, ఖాళీ,
			    {(char_u *)"en", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"spelloptions", "spo",  P_STRING|P_ALLOCED|P_VI_DEF
					   |P_ONECOMMA|P_NODUP|P_RBUF|P_HLONLY,
#నిర్వచితమైతే FEAT_SPELL
			    (char_u *)&p_spo, PV_SPO, did_సమితి_spelloptions, expand_సమితి_spelloptions,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"spellsuggest", "sps", P_STRING|P_VI_DEF|P_EXPAND|P_SECURE|P_ONECOMMA,
#నిర్వచితమైతే FEAT_SPELL
			    (char_u *)&p_sps, PV_NONE, did_సమితి_spellsuggest, expand_సమితి_spellsuggest,
			    {(char_u *)"best", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"splitbelow",  "sb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sb, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"splitkeep",   "spk",  P_STRING,
			    (char_u *)&p_spk, PV_NONE, did_సమితి_splitkeep, expand_సమితి_splitkeep,
			    {(char_u *)"cursor", (char_u *)"cursor"} SCTX_INIT},
    {"splitright",  "spr",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_spr, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"startofline", "sol",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sol, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"statusline"  ,"stl",  P_STRING|P_VI_DEF|P_ALLOCED|P_RSTAT|P_MLE,
#నిర్వచితమైతే FEAT_STL_OPT
			    (char_u *)&p_stl, PV_STL, did_సమితి_statusline, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"statuslineopt"  ,"stlo",  P_STRING|P_VI_DEF|P_ALLOCED|P_RSTAT|P_MLE
			    |P_ONECOMMA|P_COLON|P_NODUP,
#నిర్వచితమైతే FEAT_STL_OPT
			    (char_u *)&p_stlo, PV_STLO, did_సమితి_statuslineopt, expand_సమితి_statuslineopt,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"suffixes",    "su",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_su, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)".bak,~,.o,.h,.info,.swp,.obj",
				(char_u *)0L} SCTX_INIT},
    {"suffixesadd", "sua",  P_STRING|P_VI_DEF|P_ALLOCED|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_sua, PV_SUA, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"swapfile",    "swf",  P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_swf, PV_SWF, did_సమితి_swapfile, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"swapsync",    "sws",  P_STRING|P_VI_DEF,
			    (char_u *)&p_sws, PV_NONE, did_సమితి_swapsync, expand_సమితి_swapsync,
			    {(char_u *)"fsync", (char_u *)0L} SCTX_INIT},
    {"switchbuf",   "swb",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_swb, PV_NONE, did_సమితి_switchbuf, expand_సమితి_switchbuf,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"synmaxcol",   "smc",  P_అంకె|P_VI_DEF|P_RBUF,
#నిర్వచితమైతే FEAT_SYN_HL
			    (char_u *)&p_smc, PV_SMC, ఖాళీ, ఖాళీ,
			    {(char_u *)3000L, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"syntax",	    "syn",  P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB|P_NFNAME,
#నిర్వచితమైతే FEAT_SYN_HL
			    (char_u *)&p_syn, PV_SYN,
			    did_సమితి_filetype_or_syntax, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"tabclose",    "tcl",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_tcl, PV_NONE, did_సమితి_tabclose, expand_సమితి_tabclose,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"tabline",	    "tal",  P_STRING|P_VI_DEF|P_RALL|P_MLE,
#నిర్వచితమైతే FEAT_STL_OPT
			    (char_u *)&p_tal, PV_NONE, did_సమితి_tabline, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"tabpagemax",  "tpm",  P_అంకె|P_VI_DEF,
			    (char_u *)&p_tpm, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)10L, (char_u *)0L} SCTX_INIT},
#ఒకవేళ defined(FEAT_TABPANEL)
    {"tabpanel",  "tpl",    P_STRING|P_VI_DEF|P_RALL|P_MLE,
			    (char_u *)&p_tpl, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"tabpanelopt","tplo",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_COLON
								    |P_NODUP,
			    (char_u *)&p_tplo, PV_NONE, did_సమితి_tabpanelopt,
			    expand_సమితి_tabpanelopt,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
#అయితే_ముగింపు
    {"tabstop",	    "ts",   P_అంకె|P_VI_DEF|P_RBUF,
			    (char_u *)&p_ts, PV_TS,
			    did_సమితి_shiftwidth_tabstop, ఖాళీ,
			    {(char_u *)8L, (char_u *)0L} SCTX_INIT},
    {"tagbsearch",  "tbs",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_tbs, PV_NONE, ఖాళీ, ఖాళీ,
#నిర్వచితమైతే VMS	// binary searching doesn't appear to work on VMS
			    {(char_u *)0L, (char_u *)0L}
#లేకపోతే
			    {(char_u *)TRUE, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"tagcase",	    "tc",   P_STRING|P_VIM,
			    (char_u *)&p_tc, PV_TC, did_సమితి_tagcase, expand_సమితి_tagcase,
			    {(char_u *)"followic", (char_u *)"followic"} SCTX_INIT},
    {"tagfunc",    "tfu",   P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#నిర్వచితమైతే FEAT_EVAL
			    (char_u *)&p_tfu, PV_TFU, did_సమితి_tagfunc, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"taglength",   "tl",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_tl, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"tagrelative", "tr",   P_BOOL|P_VIM,
			    (char_u *)&p_tr, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"tags",	    "tag",  P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_ట్యాగులు, PV_ట్యాగులు, ఖాళీ, ఖాళీ,
			    {
#ఒకవేళ defined(FEAT_EMACS_ట్యాగులు) && !defined(CASE_INSENSITIVE_FILENAME)
			    (char_u *)"./tags,./TAGS,tags,TAGS",
#లేకపోతే
			    (char_u *)"./tags,tags",
#అయితే_ముగింపు
				(char_u *)0L} SCTX_INIT},
    {"tagsecure",   "tsc",   P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_tagsecure, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)TRUE} SCTX_INIT},
    {"tagstack",    "tgst", P_BOOL|P_VI_DEF,
			    (char_u *)&p_tgst, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"tcldll",      ఖాళీ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(DYNAMIC_TCL)
			    (char_u *)&p_tcldll, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DYNAMIC_TCL_DLL, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"term",	    ఖాళీ,   P_STRING|P_EXPAND|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RALL,
			    (char_u *)&T_NAME, PV_NONE, did_సమితి_పదం, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"termbidi", "tbidi",   P_BOOL|P_VI_DEF,
#నిర్వచితమైతే FEAT_ARABIC
			    (char_u *)&p_tbidi, PV_NONE, ఖాళీ, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"termencoding", "tenc", P_STRING|P_VI_DEF|P_RCLR,
			    (char_u *)&p_tenc, PV_NONE, did_సమితి_encoding, expand_సమితి_encoding,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"termguicolors", "tgc", P_BOOL|P_VI_DEF|P_VIM|P_RCLR,
#నిర్వచితమైతే FEAT_TERMGUICOLORS
			    (char_u *)&p_tgc, PV_NONE, did_సమితి_termguicolors, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)FALSE}
#లేకపోతే
			    (char_u*)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)FALSE}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"termresize", "trz", P_STRING|P_VI_DEF,
#నిర్వచితమైతే UNIX
			    (char_u *)&p_trz, PV_NONE, did_సమితి_termresize, expand_సమితి_termresize,
			    {(char_u *)"", (char_u *)0}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"termsync", "tsy",	    P_BOOL|P_VI_DEF,
			    (char_u *)&p_tsy, PV_NONE, did_సమితి_termsync, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"termwinkey", "twk",   P_STRING|P_ALLOCED|P_RWIN|P_VI_DEF,
#నిర్వచితమైతే FEAT_TERMINAL
			    (char_u *)VAR_WIN, PV_TWK, did_సమితి_termwinkey, ఖాళీ,
			    {(char_u *)"", (char_u *)ఖాళీ}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"termwinscroll", "twsl", P_అంకె|P_VI_DEF|P_VIM|P_RBUF,
#నిర్వచితమైతే FEAT_TERMINAL
			    (char_u *)&p_twsl, PV_TWSL, did_సమితి_termwinscroll, ఖాళీ,
			    {(char_u *)10000L, (char_u *)10000L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"termwinsize", "tws",  P_STRING|P_ALLOCED|P_RWIN|P_VI_DEF,
#నిర్వచితమైతే FEAT_TERMINAL
			    (char_u *)VAR_WIN, PV_TWS, did_సమితి_termwinsize, ఖాళీ,
			    {(char_u *)"", (char_u *)ఖాళీ}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"termwintype", "twt",  P_STRING|P_ALLOCED|P_VI_DEF,
#ఒకవేళ defined(MSWIN) && defined(FEAT_TERMINAL)
			    (char_u *)&p_twt, PV_NONE, did_సమితి_termwintype, expand_సమితి_termwintype,
			    {(char_u *)"", (char_u *)ఖాళీ}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"terse",	    ఖాళీ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_terse, PV_NONE, did_సమితి_terse, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"textauto",    "ta",   P_BOOL|P_VIM,
			    (char_u *)&p_ta, PV_NONE, did_సమితి_textauto, ఖాళీ,
			    {(char_u *)DFLT_TEXTAUTO, (char_u *)TRUE}
			    SCTX_INIT},
    {"textmode",    "tx",   P_BOOL|P_VI_DEF|P_NO_MKRC,
			    (char_u *)&p_tx, PV_TX, did_సమితి_textmode, ఖాళీ,
			    {
#నిర్వచితమైతే USE_CRNL
			    (char_u *)TRUE,
#లేకపోతే
			    (char_u *)FALSE,
#అయితే_ముగింపు
				(char_u *)0L} SCTX_INIT},
    {"textwidth",   "tw",   P_అంకె|P_VI_DEF|P_VIM|P_RBUF|P_HLONLY,
			    (char_u *)&p_tw, PV_TW, did_సమితి_textwidth, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"thesaurus",   "tsr",  P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA|P_NODUP|P_NDNAME,
			    (char_u *)&p_tsr, PV_TSR, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"thesaurusfunc", "tsrfu",  P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#నిర్వచితమైతే FEAT_COMPL_FUNC
			    (char_u *)&p_tsrfu, PV_TSRFU,
			    did_సమితి_thesaurusfunc, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"tildeop",	    "top",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_గమ్యం, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"timeout",	    "to",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_timeout, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"timeoutlen",  "tm",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_tm, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)1000L, (char_u *)0L} SCTX_INIT},
    {"title",	    ఖాళీ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_title, PV_NONE, did_సమితి_title_icon, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"titlelen",    ఖాళీ,   P_అంకె|P_VI_DEF,
			    (char_u *)&p_titlelen, PV_NONE, did_సమితి_titlelen, ఖాళీ,
			    {(char_u *)85L, (char_u *)0L} SCTX_INIT},
    {"titleold",    ఖాళీ,   P_STRING|P_VI_DEF|P_GETTEXT|P_SECURE|P_NO_MKRC,
			    (char_u *)&p_titleold, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)N_("Thanks for flying Vim"),
							       (char_u *)0L}
			    SCTX_INIT},
    {"titlestring", ఖాళీ,   P_STRING|P_VI_DEF|P_MLE,
			    (char_u *)&p_titlestring, PV_NONE,
			    did_సమితి_titlestring, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"toolbar",     "tb",   P_STRING|P_ONECOMMA|P_VI_DEF|P_NODUP,
#ఒకవేళ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
			    (char_u *)&p_toolbar, PV_NONE, did_సమితి_toolbar, expand_సమితి_toolbar,
			    {(char_u *)"icons,tooltips", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"toolbariconsize",	"tbis", P_STRING|P_VI_DEF,
#ఒకవేళ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
			    (char_u *)&p_tbis, PV_NONE,
			    did_సమితి_toolbariconsize, expand_సమితి_toolbariconsize,
			    {(char_u *)"small", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"ttimeout",    ఖాళీ,   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_ttimeout, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"ttimeoutlen", "ttm",  P_అంకె|P_VI_DEF,
			    (char_u *)&p_ttm, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)-1L, (char_u *)0L} SCTX_INIT},
    {"ttybuiltin",  "tbi",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_tbi, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"ttyfast",	    "tf",   P_BOOL|P_NO_MKRC|P_VI_DEF,
			    (char_u *)&p_tf, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"ttymouse",    "ttym", P_STRING|P_NODEFAULT|P_NO_MKRC|P_VI_DEF,
#ఒకవేళ defined(UNIX) || defined(VMS)
			    (char_u *)&p_ttym, PV_NONE, did_సమితి_ttymouse, expand_సమితి_ttymouse,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"ttyscroll",   "tsl",  P_అంకె|P_VI_DEF,
			    (char_u *)&p_ttyscroll, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)999L, (char_u *)0L} SCTX_INIT},
    {"ttytype",	    "tty",  P_STRING|P_EXPAND|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RALL,
			    (char_u *)&T_NAME, PV_NONE, did_సమితి_పదం, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"undodir",     "udir", P_STRING|P_EXPAND|P_ONECOMMA|P_NODUP|P_SECURE
								    |P_VI_DEF,
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
			    (char_u *)&p_udir, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)".", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"undofile",    "udf",  P_BOOL|P_VI_DEF|P_VIM,
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
			    (char_u *)&p_udf, PV_UDF, did_సమితి_undofile, ఖాళీ,
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
#అయితే_ముగింపు
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"undolevels",  "ul",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_ul, PV_UL, did_సమితి_undolevels, ఖాళీ,
			    {
#ఒకవేళ defined(UNIX) || defined(MSWIN) || defined(VMS)
			    (char_u *)1000L,
#లేకపోతే
			    (char_u *)100L,
#అయితే_ముగింపు
				(char_u *)0L} SCTX_INIT},
    {"undoreload",  "ur",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_ur, PV_NONE, ఖాళీ, ఖాళీ,
			    { (char_u *)10000L, (char_u *)0L} SCTX_INIT},
    {"updatecount", "uc",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_uc, PV_NONE, did_సమితి_updatecount, ఖాళీ,
			    {(char_u *)200L, (char_u *)0L} SCTX_INIT},
    {"updatetime",  "ut",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_ut, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)4000L, (char_u *)0L} SCTX_INIT},
    {"varsofttabstop", "vsts",  P_STRING|P_VI_DEF|P_VIM|P_COMMA,
#నిర్వచితమైతే FEAT_VARTABS
			    (char_u *)&p_vsts, PV_VSTS,
			    did_సమితి_varsofttabstop, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)ఖాళీ}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"vartabstop",  "vts",  P_STRING|P_VI_DEF|P_VIM|P_RBUF|P_COMMA,
#నిర్వచితమైతే FEAT_VARTABS
			    (char_u *)&p_vts, PV_VTS, did_సమితి_vartabstop, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)ఖాళీ}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"verbose",	    "vbs",  P_అంకె|P_VI_DEF,
			    (char_u *)&p_verbose, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"verbosefile", "vfile", P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_vfile, PV_NONE, did_సమితి_verbosefile, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"viewdir",     "vdir", P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#నిర్వచితమైతే FEAT_SESSION
			    (char_u *)&p_vdir, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)DFLT_VDIR, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"viewoptions", "vop",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#నిర్వచితమైతే FEAT_SESSION
			    (char_u *)&p_vop, PV_NONE, did_సమితి_viewoptions, expand_సమితి_sessionoptions,
			    {(char_u *)"folds,options,cursor,curdir",
								  (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"viminfo",	    "vi",   P_STRING|P_ONECOMMA|P_NODUP|P_SECURE,
#నిర్వచితమైతే FEAT_VIMINFO
			    (char_u *)&p_viminfo, PV_NONE, did_సమితి_viminfo, ఖాళీ,
# ఒకవేళ defined(MSWIN)
			    {(char_u *)"", (char_u *)"'100,<50,s10,h,rA:,rB:"}
# elif defined(AMIGA)
			    {(char_u *)"",
				 (char_u *)"'100,<50,s10,h,rdf0:,rdf1:,rdf2:"}
# లేకపోతే
			    {(char_u *)"", (char_u *)"'100,<50,s10,h"}
# endif
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"viminfofile", "vif",  P_STRING|P_EXPAND|P_ONECOMMA|P_NODUP
							    |P_SECURE|P_VI_DEF,
#నిర్వచితమైతే FEAT_VIMINFO
			    (char_u *)&p_viminfofile, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"virtualedit", "ve",   P_STRING|P_ONECOMMA|P_NODUP|P_VI_DEF
							    |P_VIM|P_CURSWANT,
			    (char_u *)&p_ve, PV_VE, did_సమితి_virtualedit, expand_సమితి_virtualedit,
			    {(char_u *)"", (char_u *)""}
			    SCTX_INIT},
    {"visualbell",  "vb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_vb, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"w1200",	    ఖాళీ,   P_అంకె|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"w300",	    ఖాళీ,   P_అంకె|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"w9600",	    ఖాళీ,   P_అంకె|P_VI_DEF,
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"warn",	    ఖాళీ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_warn, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"weirdinvert", "wiv",  P_BOOL|P_VI_DEF|P_RCLR,
			    (char_u *)&p_wiv, PV_NONE, did_సమితి_weirdinvert, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"whichwrap",   "ww",   P_STRING|P_VIM|P_ONECOMMA|P_FLAGLIST,
			    (char_u *)&p_ww, PV_NONE, did_సమితి_whichwrap, expand_సమితి_whichwrap,
			    {(char_u *)"", (char_u *)"b,s"} SCTX_INIT},
    {"wildchar",    "wc",   P_అంకె|P_VIM,
			    (char_u *)&p_wc, PV_NONE, did_సమితి_wildchar, ఖాళీ,
			    {(char_u *)(దీర్ఘ)Ctrl_E, (char_u *)(దీర్ఘ)TAB}
			    SCTX_INIT},
    {"wildcharm",   "wcm",  P_అంకె|P_VI_DEF,
			    (char_u *)&p_wcm, PV_NONE, did_సమితి_wildchar, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"wildignore",  "wig",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_wig, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"wildignorecase", "wic", P_BOOL|P_VI_DEF,
			    (char_u *)&p_wic, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"wildmenu",    "wmnu", P_BOOL|P_VI_DEF,
			    (char_u *)&p_wmnu, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"wildmode",    "wim",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_wim, PV_NONE, did_సమితి_wildmode, expand_సమితి_wildmode,
			    {(char_u *)"full", (char_u *)0L} SCTX_INIT},
    {"wildoptions", "wop",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_wop, PV_NONE, did_సమితి_wildoptions, expand_సమితి_wildoptions,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"winaltkeys",  "wak",  P_STRING|P_VI_DEF,
#నిర్వచితమైతే FEAT_WAK
			    (char_u *)&p_wak, PV_NONE, did_సమితి_winaltkeys, expand_సమితి_winaltkeys,
			    {(char_u *)"menu", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"wincolor", "wcr",	    P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_WCR, did_సమితి_wincolor, expand_సమితి_wincolor,
			    {(char_u *)"", (char_u *)ఖాళీ}
			    SCTX_INIT},
    {"window",	    "wi",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_కిటికీ, PV_NONE, did_సమితి_కిటికీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"winfixbuf", "wfb",    P_BOOL|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_WFB, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"winfixheight", "wfh", P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)VAR_WIN, PV_WFH, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"winfixwidth", "wfw",  P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)VAR_WIN, PV_WFW, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"winheight",   "wh",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_wh, PV_NONE,
			    did_సమితి_winheight_helpheight, ఖాళీ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"winhighlight", "whl", P_STRING|P_VI_DEF|P_RALL|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)VAR_WIN, PV_WHL, did_సమితి_winhighlight, expand_సమితి_winhighlight,
			    {(char_u *)"", (char_u *)ఖాళీ} SCTX_INIT},
    {"winminheight", "wmh", P_అంకె|P_VI_DEF,
			    (char_u *)&p_wmh, PV_NONE, did_సమితి_winminheight, ఖాళీ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"winminwidth", "wmw",  P_అంకె|P_VI_DEF,
			    (char_u *)&p_wmw, PV_NONE, did_సమితి_winminwidth, ఖాళీ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"winptydll", ఖాళీ,	    P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ఒకవేళ defined(MSWIN) && defined(FEAT_TERMINAL)
			    (char_u *)&p_winptydll, PV_NONE, ఖాళీ, ఖాళీ, {
# ifdef _WIN64
			    (char_u *)"winpty64.dll",
# లేకపోతే
			    (char_u *)"winpty32.dll",
# endif
				(char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"winwidth",   "wiw",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_wiw, PV_NONE, did_సమితి_winwidth, ఖాళీ,
			    {(char_u *)20L, (char_u *)0L} SCTX_INIT},
    {"wlseat",	    "wse",  P_STRING|P_VI_DEF,
#నిర్వచితమైతే FEAT_WAYLAND
			    (char_u *)&p_wse, PV_NONE, did_సమితి_wlseat, ఖాళీ,
			    {(char_u *)"", (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"wlsteal",	    "wst",  P_BOOL|P_VI_DEF, // Deprecated
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
			    SCTX_INIT},
    {"wltimeoutlen", "wtm", P_అంకె|P_VI_DEF,
#నిర్వచితమైతే FEAT_WAYLAND
			    (char_u *)&p_wtm, PV_NONE, did_సమితి_wltimeoutlen, ఖాళీ,
			    {(char_u *)500L, (char_u *)0L}
#లేకపోతే
			    (char_u *)ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)ఖాళీ, (char_u *)0L}
#అయితే_ముగింపు
			    SCTX_INIT},
    {"wrap",	    ఖాళీ,   P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_WRAP, did_సమితి_wrap, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"wrapmargin",  "wm",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_wm, PV_WM, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"wrapscan",    "ws",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ws, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"write",	    ఖాళీ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_write, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"writeany",    "wa",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_wa, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"writebackup", "wb",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_wb, PV_NONE, ఖాళీ, ఖాళీ,
			    {
#నిర్వచితమైతే FEAT_WRITEBACKUP
			    (char_u *)TRUE,
#లేకపోతే
			    (char_u *)FALSE,
#అయితే_ముగింపు
				(char_u *)0L} SCTX_INIT},
    {"writedelay",  "wd",   P_అంకె|P_VI_DEF,
			    (char_u *)&p_wd, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"xtermcodes",  ఖాళీ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_xtermcodes, PV_NONE, ఖాళీ, ఖాళీ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},

// terminal output codes
#నిర్వచించు p_పదం(sss, vvv) \
			    {sss, ఖాళీ, P_STRING|P_VI_DEF|P_RALL|P_SECURE, \
			    (char_u *)&vvv, PV_NONE, did_సమితి_పదం_option, ఖాళీ, \
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},

    p_పదం("t_AB", T_CAB)
    p_పదం("t_AF", T_CAF)
    p_పదం("t_AU", T_CAU)
    p_పదం("t_AL", T_CAL)
    p_పదం("t_al", T_AL)
    p_పదం("t_bc", T_BC)
    p_పదం("t_BE", T_BE)
    p_పదం("t_BD", T_BD)
    p_పదం("t_cd", T_CD)
    p_పదం("t_ce", T_CE)
    p_పదం("t_Ce", T_UCE)
    p_పదం("t_CF", T_CFO)
    p_పదం("t_cl", T_CL)
    p_పదం("t_cm", T_CM)
    p_పదం("t_Co", T_CCO)
    p_పదం("t_CS", T_CCS)
    p_పదం("t_Cs", T_UCS)
    p_పదం("t_cs", T_CS)
    p_పదం("t_CV", T_CSV)
    p_పదం("t_da", T_DA)
    p_పదం("t_db", T_DB)
    p_పదం("t_DL", T_CDL)
    p_పదం("t_dl", T_DL)
    p_పదం("t_ds", T_DS)
    p_పదం("t_Ds", T_CDS)
    p_పదం("t_EC", T_CEC)
    p_పదం("t_EI", T_CEI)
    p_పదం("t_fs", T_FS)
    p_పదం("t_fd", T_FD)
    p_పదం("t_fe", T_FE)
    p_పదం("t_GP", T_CGP)
    p_పదం("t_IE", T_CIE)
    p_పదం("t_IS", T_CIS)
    p_పదం("t_ke", T_KE)
    p_పదం("t_ks", T_KS)
    p_పదం("t_le", T_LE)
    p_పదం("t_mb", T_MB)
    p_పదం("t_md", T_MD)
    p_పదం("t_me", T_ME)
    p_పదం("t_mr", T_MR)
    p_పదం("t_ms", T_MS)
    p_పదం("t_nd", T_ND)
    p_పదం("t_op", T_OP)
    p_పదం("t_RF", T_RFG)
    p_పదం("t_RB", T_RBG)
    p_పదం("t_RC", T_చక్రీయ_తనిఖీ)
    p_పదం("t_RI", T_CRI)
    p_పదం("t_Ri", T_SRI)
    p_పదం("t_RK", T_CRK)
    p_పదం("t_RS", T_CRS)
    p_పదం("t_RT", T_CRT)
    p_పదం("t_RV", T_CRV)
    p_పదం("t_Sb", T_CSB)
    p_పదం("t_SC", T_CSC)
    p_పదం("t_se", T_SE)
    p_పదం("t_Sf", T_CSF)
    p_పదం("t_SH", T_CSH)
    p_పదం("t_SI", T_CSI)
    p_పదం("t_Si", T_SSI)
    p_పదం("t_so", T_SO)
    p_పదం("t_SR", T_CSR)
    p_పదం("t_sr", T_SR)
    p_పదం("t_ST", T_CST)
    p_పదం("t_Te", T_STE)
    p_పదం("t_te", T_TE)
    p_పదం("t_TE", T_CTE)
    p_పదం("t_ti", T_TI)
    p_పదం("t_TI", T_CTI)
    p_పదం("t_Ts", T_STS)
    p_పదం("t_ts", T_TS)
    p_పదం("t_u7", T_U7)
    p_పదం("t_ue", T_UE)
    p_పదం("t_us", T_US)
    p_పదం("t_Us", T_USS)
    p_పదం("t_ut", T_UT)
    p_పదం("t_vb", T_VB)
    p_పదం("t_ve", T_VE)
    p_పదం("t_vi", T_VI)
    p_పదం("t_VS", T_CVS)
    p_పదం("t_vs", T_VS)
    p_పదం("t_WP", T_CWP)
    p_పదం("t_WS", T_CWS)
    p_పదం("t_XM", T_CXM)
    p_పదం("t_xn", T_XN)
    p_పదం("t_xs", T_XS)
    p_పదం("t_ZH", T_CZH)
    p_పదం("t_ZR", T_CZR)
    p_పదం("t_8f", T_8F)
    p_పదం("t_8b", T_8B)
    p_పదం("t_8u", T_8U)
    p_పదం("t_xo", T_XON)
    p_పదం("t_BS", T_BSU)
    p_పదం("t_ES", T_ESU)

// terminal key codes are not in here

    // end marker
    {ఖాళీ, ఖాళీ, 0, ఖాళీ, PV_NONE, ఖాళీ, ఖాళీ, {ఖాళీ, ఖాళీ} SCTX_INIT}
};

#నిర్వచించు OPTION_లెక్క ARRAY_నిడివి(options)

// The following is needed to make the gen_opt_test.vim script work.
// {"
