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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_BOTH 0x1000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_WIN  0x2000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_BUF  0x4000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_ਨਕਾਬ 0x0fff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPT_WIN(x)  (idopt_T)(PV_WIN + (ਪੂਰਨ)(x))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPT_BUF(x)  (idopt_T)(PV_BUF + (ਪੂਰਨ)(x))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPT_BOTH(x) (idopt_T)(PV_BOTH + (ਪੂਰਨ)(x))

// Definition of the PV_ values for buffer-local options.
// The BV_ values are defined in option.h.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_AC		OPT_BOTH(OPT_BUF(BV_AC))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_AI		OPT_BUF(BV_AI)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_AR		OPT_BOTH(OPT_BUF(BV_AR))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_BKC		OPT_BOTH(OPT_BUF(BV_BKC))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_BH		OPT_BUF(BV_BH)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_BT		OPT_BUF(BV_BT)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
# define PV_EFM		OPT_BOTH(OPT_BUF(BV_EFM))
# define PV_GEFM	OPT_BOTH(OPT_BUF(BV_GEFM))
# define PV_GP		OPT_BOTH(OPT_BUF(BV_GP))
# define PV_MP		OPT_BOTH(OPT_BUF(BV_MP))
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_BIN		OPT_BUF(BV_BIN)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_BL		OPT_BUF(BV_BL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_BOMB		OPT_BUF(BV_BOMB)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_CI		OPT_BUF(BV_CI)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_CIN		OPT_BUF(BV_CIN)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_CINK		OPT_BUF(BV_CINK)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_CINO		OPT_BUF(BV_CINO)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_CINSD	OPT_BUF(BV_CINSD)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_CINW		OPT_BUF(BV_CINW)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_CM		OPT_BOTH(OPT_BUF(BV_CM))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_CMS		OPT_BUF(BV_CMS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_COM		OPT_BUF(BV_COM)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_COT		OPT_BOTH(OPT_BUF(BV_COT))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_CPT		OPT_BUF(BV_CPT)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_DICT		OPT_BOTH(OPT_BUF(BV_DICT))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
# define PV_DIA		OPT_BOTH(OPT_BUF(BV_DIA))
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_TSR		OPT_BOTH(OPT_BUF(BV_TSR))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_FFU		OPT_BOTH(OPT_BUF(BV_FFU))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_CSL		OPT_BUF(BV_CSL)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
# define PV_CFU		OPT_BUF(BV_CFU)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
# define PV_DEF		OPT_BOTH(OPT_BUF(BV_DEF))
# define PV_INC		OPT_BOTH(OPT_BUF(BV_INC))
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_EOF		OPT_BUF(BV_EOF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_EOL		OPT_BUF(BV_EOL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_FIXEOL	OPT_BUF(BV_FIXEOL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_EP		OPT_BOTH(OPT_BUF(BV_EP))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_ET		OPT_BUF(BV_ET)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_FENC		OPT_BUF(BV_FENC)
#ਜੇ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
# define PV_BEXPR	OPT_BOTH(OPT_BUF(BV_BEXPR))
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_FP		OPT_BOTH(OPT_BUF(BV_FP))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
# define PV_FEX		OPT_BUF(BV_FEX)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_FF		OPT_BUF(BV_FF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_FLP		OPT_BUF(BV_FLP)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_FO		OPT_BUF(BV_FO)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FSYNC
# define PV_FS		OPT_BOTH(OPT_BUF(BV_FS))
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_FT		OPT_BUF(BV_FT)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_IMI		OPT_BUF(BV_IMI)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_IMS		OPT_BUF(BV_IMS)
#ਜੇ defined(FEAT_EVAL)
# define PV_INDE	OPT_BUF(BV_INDE)
# define PV_INDK	OPT_BUF(BV_INDK)
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_ਲੱਭੋ_ID) && defined(FEAT_EVAL)
# define PV_INEX	OPT_BUF(BV_INEX)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_INF		OPT_BUF(BV_INF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_ISK		OPT_BUF(BV_ISK)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
# define PV_ਕੁੰਜੀ		OPT_BUF(BV_ਕੁੰਜੀ)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
# define PV_KMAP	OPT_BUF(BV_KMAP)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_KP		OPT_BOTH(OPT_BUF(BV_KP))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_LISP		OPT_BUF(BV_LISP)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_LOP		OPT_BUF(BV_LOP)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_LW		OPT_BOTH(OPT_BUF(BV_LW))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_MENC		OPT_BOTH(OPT_BUF(BV_MENC))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_MA		OPT_BUF(BV_MA)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_ML		OPT_BUF(BV_ML)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_MOD		OPT_BUF(BV_MOD)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_MPS		OPT_BUF(BV_MPS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_NF		OPT_BUF(BV_NF)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
# define PV_OFU		OPT_BUF(BV_OFU)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_ਰਸਤਾ		OPT_BOTH(OPT_BUF(BV_ਰਸਤਾ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_PI		OPT_BUF(BV_PI)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_QE		OPT_BUF(BV_QE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_RO		OPT_BUF(BV_RO)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_SI		OPT_BUF(BV_SI)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_SN		OPT_BUF(BV_SN)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
# define PV_SMC		OPT_BUF(BV_SMC)
# define PV_SYN		OPT_BUF(BV_SYN)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
# define PV_SPC		OPT_BUF(BV_SPC)
# define PV_SPF		OPT_BUF(BV_SPF)
# define PV_SPL		OPT_BUF(BV_SPL)
# define PV_SPO		OPT_BUF(BV_SPO)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_STS		OPT_BUF(BV_STS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_SUA		OPT_BUF(BV_SUA)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_SW		OPT_BUF(BV_SW)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_SWF		OPT_BUF(BV_SWF)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
# define PV_TFU		OPT_BUF(BV_TFU)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
# define PV_TSRFU	OPT_BOTH(OPT_BUF(BV_TSRFU))
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_ਟੈਗ_ਸੂਚੀ		OPT_BOTH(OPT_BUF(BV_ਟੈਗ_ਸੂਚੀ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_TC		OPT_BOTH(OPT_BUF(BV_TC))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_TS		OPT_BUF(BV_TS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_TW		OPT_BUF(BV_TW)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_TX		OPT_BUF(BV_TX)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
# define PV_UDF		OPT_BUF(BV_UDF)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_WM		OPT_BUF(BV_WM)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
# define PV_VSTS	OPT_BUF(BV_VSTS)
# define PV_VTS		OPT_BUF(BV_VTS)
#ਜੇ_ਅੰਤ

// Definition of the PV_ values for window-local options.
// The WV_ values are defined in option.h.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_LIST		OPT_WIN(WV_LIST)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
# define PV_ARAB	OPT_WIN(WV_ARAB)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
# define PV_BRI		OPT_WIN(WV_BRI)
# define PV_BRIOPT	OPT_WIN(WV_BRIOPT)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_WCR		OPT_WIN(WV_WCR)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
# define PV_DIFF	OPT_WIN(WV_DIFF)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_EIW		OPT_WIN(WV_EIW)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
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
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
# define PV_LBR		OPT_WIN(WV_LBR)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_LCS		OPT_BOTH(OPT_WIN(WV_LCS))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_FCS		OPT_BOTH(OPT_WIN(WV_FCS))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_NU		OPT_WIN(WV_NU)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_RNU		OPT_WIN(WV_RNU)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_VE		OPT_BOTH(OPT_WIN(WV_VE))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
# define PV_NUW		OPT_WIN(WV_NUW)
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_QUICKFIX)
# define PV_PVW		OPT_WIN(WV_PVW)
# define PV_LHI         OPT_WIN(WV_LHI)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
# define PV_RL		OPT_WIN(WV_RL)
# define PV_RLC		OPT_WIN(WV_RLC)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_SCBIND	OPT_WIN(WV_SCBIND)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_SCROLL	OPT_WIN(WV_SCROLL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_SMS		OPT_WIN(WV_SMS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_SISO		OPT_BOTH(OPT_WIN(WV_SISO))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_SO		OPT_BOTH(OPT_WIN(WV_SO))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_SOP		OPT_BOTH(OPT_WIN(WV_SOP))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
# define PV_SPELL	OPT_WIN(WV_SPELL)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
# define PV_CUC		OPT_WIN(WV_CUC)
# define PV_CUL		OPT_WIN(WV_CUL)
# define PV_CULOPT	OPT_WIN(WV_CULOPT)
# define PV_CC		OPT_WIN(WV_CC)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
# define PV_SBR		OPT_BOTH(OPT_WIN(WV_SBR))
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
# define PV_STL		OPT_BOTH(OPT_WIN(WV_STL))
# define PV_STLO	OPT_BOTH(OPT_WIN(WV_STLO))
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_UL		OPT_BOTH(OPT_BUF(BV_UL))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_WFB		OPT_WIN(WV_WFB)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_WFH		OPT_WIN(WV_WFH)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_WFW		OPT_WIN(WV_WFW)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_WHL		OPT_WIN(WV_WHL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_WRAP		OPT_WIN(WV_WRAP)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PV_CRBIND	OPT_WIN(WV_CRBIND)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
# define PV_COCU	OPT_WIN(WV_COCU)
# define PV_COLE	OPT_WIN(WV_COLE)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
# define PV_TWK		OPT_WIN(WV_TWK)
# define PV_TWS		OPT_WIN(WV_TWS)
# define PV_TWSL	OPT_BUF(BV_TWSL)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
# define PV_SCL		OPT_WIN(WV_SCL)
#ਜੇ_ਅੰਤ

// WV_ and BV_ values get typecasted to this for the "indir" field
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    PV_NONE = 0,
    PV_MAXVAL = 0xffff    // to avoid warnings for value out of range
} idopt_T;

// Options local to a window have a value local to a buffer and global to all
// buffers.  Indicate this by setting "var" to VAR_WIN.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_WIN ((char_u *)-1)

// Saved values for when 'bin' is set.
ਸਥਿਰ ਪੂਰਨ	p_et_nobin;
ਸਥਿਰ ਪੂਰਨ	p_ml_nobin;
ਸਥਿਰ ਲੰਮਾ	p_tw_nobin;
ਸਥਿਰ ਲੰਮਾ	p_wm_nobin;

// Saved values for when 'paste' is set
ਸਥਿਰ ਪੂਰਨ	p_ai_nopaste;
ਸਥਿਰ ਪੂਰਨ	p_et_nopaste;
ਸਥਿਰ ਲੰਮਾ	p_sts_nopaste;
ਸਥਿਰ ਲੰਮਾ	p_tw_nopaste;
ਸਥਿਰ ਲੰਮਾ	p_wm_nopaste;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
ਸਥਿਰ char_u	*p_vsts_nopaste;
#ਜੇ_ਅੰਤ

ਬਣਤਰ vimoption
{
    ਅੱਖਰ	*fullname;	// full option name
    ਅੱਖਰ	*shortname;	// permissible abbreviation
    long_u	flags;		// see in option.h
    char_u	*var;		// global option: pointer to variable;
				// window-local option: VAR_WIN;
				// buffer-local option: global value
    idopt_T	indir;		// global option: PV_NONE;
				// local option: indirect option index
    // callback function to invoke after an option is modified to validate and
    // apply the new value.
    opt_did_ਸੈੱਟ_cb_T	opt_did_ਸੈੱਟ_cb;

    // callback function to invoke when expanding possible values on the
    // cmdline. Only useful for string options.
    opt_expand_cb_T	opt_expand_cb;

    char_u	*def_val[2];	// default values for variable (vi and vim)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    sctx_T	script_ctx;	// script context where the option was last set
# define SCTX_INIT , {0, 0, 0, 1}
#ਨਹੀਂਤਾਂ
# define SCTX_INIT
#ਜੇ_ਅੰਤ
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VI_DEFAULT  0	    // def_val[VI_DEFAULT] is Vi default value
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_DEFAULT 1	    // def_val[VIM_DEFAULT] is Vim default value

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISK_LATIN1  (char_u *)"@,48-57,_,192-255"

// 'isprint' for latin1 is also used for MS-Windows cp1252, where 0x80 is used
// for the currency sign.
#ਜੇ defined(MSWIN)
# define ISP_LATIN1 (char_u *)"@,~-255"
#ਨਹੀਂਤਾਂ
# define ISP_LATIN1 (char_u *)"@,161-255"
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HIGHLIGHT_INIT "8:SpecialKey,~:EndOfBuffer,@:NonText,d:Directory,e:ErrorMsg,i:IncSearch,l:Search,y:CurSearch,m:MoreMsg,M:ModeMsg,n:LineNr,a:LineNrAbove,b:LineNrBelow,N:CursorLineNr,G:CursorLineSign,O:CursorLineFold,r:Question,s:StatusLine,S:StatusLineNC,c:VertSplit,|:VertSplitNC,t:Title,v:Visual,V:VisualNOS,w:WarningMsg,W:WildMenu,f:Folded,F:FoldColumn,A:DiffAdd,C:DiffChange,D:DiffDelete,T:DiffText,E:DiffTextAdd,>:SignColumn,-:Conceal,B:SpellBad,P:SpellCap,R:SpellRare,L:SpellLocal,+:Pmenu,=:PmenuSel,k:PmenuMatch,<:PmenuMatchSel,[:PmenuKind,]:PmenuKindSel,{:PmenuExtra,}:PmenuExtraSel,x:PmenuSbar,X:PmenuThumb,j:PmenuBorder,H:PmenuShadow,p:Popup,J:PopupBorder,Q:PopupTitle,*:TabLine,#:TabLineSel,_:TabLineFill,!:CursorColumn,.:CursorLine,o:ColorColumn,q:QuickFixLine,z:StatusLineTerm,Z:StatusLineTermNC,g:MsgArea,h:ComplMatchIns,%:TabPanel,^:TabPanelSel,&:TabPanelFill,I:PreInsert"

// Default python version for pyx* commands
#ਜੇ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
# define DEFAULT_PYTHON_VER	0
#ਜਾਂਜੇ defined(FEAT_PYTHON3)
# define DEFAULT_PYTHON_VER	3
#ਜਾਂਜੇ defined(FEAT_PYTHON)
# define DEFAULT_PYTHON_VER	2
#ਨਹੀਂਤਾਂ
# define DEFAULT_PYTHON_VER	0
#ਜੇ_ਅੰਤ

// used for 'cinkeys' and 'indentkeys'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INDENTKEYS_DEFAULT (char_u *)"0{,0},0),0],:,0#,!^F,o,O,e"

// options[] is initialized here.
// The order of the options MUST be alphabetic for ":set all" and findoption().
// All option names MUST start with a lowercase letter (for findoption()).
// Exception: "t_" options are at the end.
// The options with a NULL variable are 'hidden': a set command for them is
// ignored and they are not printed.
ਸਥਿਰ ਬਣਤਰ vimoption options[] =
{
    {"aleph",	    "al",   P_ਅੰਕ|P_VI_DEF|P_CURSWANT,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
			    (char_u *)&p_aleph, PV_NONE,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE,
#ਜੇ_ਅੰਤ
			    ਨਿਰਾ, ਨਿਰਾ,
			    {
#ਜੇ defined(MSWIN) && !defined(FEAT_GUI_MSWIN)
			    (char_u *)128L,
#ਨਹੀਂਤਾਂ
			    (char_u *)224L,
#ਜੇ_ਅੰਤ
					    (char_u *)0L} SCTX_INIT},
    {"allowrevins", "ari",  P_BOOL|P_VI_DEF|P_VIM,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
			    (char_u *)&p_ari, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"altkeymap",   "akm",  P_BOOL|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"ambiwidth",  "ambw",  P_STRING|P_VI_DEF|P_RCLR,
			    (char_u *)&p_ambw, PV_NONE, did_ਸੈੱਟ_ambiwidth, expand_ਸੈੱਟ_ambiwidth,
			    {(char_u *)"single", (char_u *)0L}
			    SCTX_INIT},
    {"antialias",   "anti", P_BOOL|P_VI_DEF|P_VIM|P_RCLR,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)FALSE}
			    SCTX_INIT},
    {"arabic",	    "arab", P_BOOL|P_VI_DEF|P_VIM|P_CURSWANT,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
			    (char_u *)VAR_WIN, PV_ARAB, did_ਸੈੱਟ_arabic, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"arabicshape", "arshape", P_BOOL|P_VI_DEF|P_VIM|P_RCLR,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
			    (char_u *)&p_arshape, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"autochdir",  "acd",   P_BOOL|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_AUTOCHDIR
			    (char_u *)&p_acd, PV_NONE, did_ਸੈੱਟ_autochdir, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
    {"autocomplete",  "ac", P_BOOL|P_VI_DEF,
			    (char_u *)&p_ac, PV_AC, ਨਿਰਾ,
			    ਨਿਰਾ, {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autocompletedelay", "acl", P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_acl, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"autocompletetimeout", "act", P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_act, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)80L, (char_u *)0L} SCTX_INIT},
#ਜੇ_ਅੰਤ
    {"autoindent",  "ai",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ai, PV_AI, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autoprint",   "ap",   P_BOOL|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autoread",    "ar",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ar, PV_AR, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autoshelldir",  "asd",   P_BOOL|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_AUTOSHELLDIR
			    (char_u *)&p_asd, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"autowrite",   "aw",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_aw, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"autowriteall","awa",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_awa, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"background",  "bg",   P_STRING|P_VI_DEF|P_RCLR|P_HLONLY,
			    (char_u *)&p_bg, PV_NONE, did_ਸੈੱਟ_background, expand_ਸੈੱਟ_background,
			    {
#ਜੇ (defined(MSWIN)) && !defined(FEAT_GUI)
			    (char_u *)"dark",
#ਨਹੀਂਤਾਂ
			    (char_u *)"light",
#ਜੇ_ਅੰਤ
					    (char_u *)0L} SCTX_INIT},
    {"backspace",   "bs",   P_STRING|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_bs, PV_NONE, did_ਸੈੱਟ_backspace, expand_ਸੈੱਟ_backspace,
			    {(char_u *)"", (char_u *)"indent,eol,start"} SCTX_INIT},
    {"backup",	    "bk",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_bk, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"backupcopy",  "bkc",  P_STRING|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_bkc, PV_BKC, did_ਸੈੱਟ_backupcopy, expand_ਸੈੱਟ_backupcopy,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
			    {(char_u *)"yes", (char_u *)"auto"}
#ਨਹੀਂਤਾਂ
			    {(char_u *)"auto", (char_u *)"auto"}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"backupdir",   "bdir", P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA
							    |P_NODUP|P_SECURE,
			    (char_u *)&p_bdir, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DFLT_BDIR, (char_u *)0L} SCTX_INIT},
    {"backupext",   "bex",  P_STRING|P_VI_DEF|P_NFNAME,
			    (char_u *)&p_bex, PV_NONE,
			    did_ਸੈੱਟ_backupext_or_patchmode, ਨਿਰਾ,
			    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
			    (char_u *)"_",
#ਨਹੀਂਤਾਂ
			    (char_u *)"~",
#ਜੇ_ਅੰਤ
					    (char_u *)0L} SCTX_INIT},
    {"backupskip",  "bsk",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_bsk, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"balloondelay","bdlay",P_ਅੰਕ|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL
			    (char_u *)&p_bdlay, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)600L, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"ballooneval", "beval",P_BOOL|P_VI_DEF|P_NO_MKRC,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
			    (char_u *)&p_beval, PV_NONE, did_ਸੈੱਟ_ballooneval, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"balloonevalterm", "bevalterm",P_BOOL|P_VI_DEF|P_NO_MKRC,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_ਪਦ
			    (char_u *)&p_bevalterm, PV_NONE,
			    did_ਸੈੱਟ_balloonevalterm, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"balloonexpr", "bexpr", P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_MLE,
#ਜੇ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
			    (char_u *)&p_bexpr, PV_BEXPR, did_ਸੈੱਟ_optexpr, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"beautify",    "bf",   P_BOOL|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"belloff",      "bo",  P_STRING|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_bo, PV_NONE, did_ਸੈੱਟ_belloff, expand_ਸੈੱਟ_belloff,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"binary",	    "bin",  P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_bin, PV_BIN, did_ਸੈੱਟ_ਦੁਆਧਾਰੀ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"bioskey",	    "biosk",P_BOOL|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"bomb",	    ਨਿਰਾ,   P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_bomb, PV_BOMB,
			    did_ਸੈੱਟ_eof_eol_fixeol_bomb, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"breakat",	    "brk",  P_STRING|P_VI_DEF|P_RALL|P_FLAGLIST,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
			    (char_u *)&p_breakat, PV_NONE, did_ਸੈੱਟ_breakat, ਨਿਰਾ,
			    {(char_u *)" \t!@*-+;:,./?", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"breakindent",   "bri",  P_BOOL|P_VI_DEF|P_VIM|P_RWIN,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_BRI, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"breakindentopt", "briopt", P_STRING|P_ALLOCED|P_VI_DEF|P_RBUF
						  |P_ONECOMMA|P_NODUP,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_BRIOPT,
			    did_ਸੈੱਟ_breakindentopt, expand_ਸੈੱਟ_breakindentopt,
			    {(char_u *)"", (char_u *)ਨਿਰਾ}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)ਨਿਰਾ}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"browsedir",   "bsdir",P_STRING|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
			    (char_u *)&p_bsdir, PV_NONE, did_ਸੈੱਟ_browsedir, expand_ਸੈੱਟ_browsedir,
			    {(char_u *)"last", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"bufhidden",   "bh",   P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_bh, PV_BH, did_ਸੈੱਟ_bufhidden, expand_ਸੈੱਟ_bufhidden,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"buflisted",   "bl",   P_BOOL|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_bl, PV_BL, did_ਸੈੱਟ_buflisted, ਨਿਰਾ,
			    {(char_u *)1L, (char_u *)0L}
			    SCTX_INIT},
    {"buftype",	    "bt",   P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_bt, PV_BT, did_ਸੈੱਟ_buftype, expand_ਸੈੱਟ_buftype,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"casemap",	    "cmp",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cmp, PV_NONE, did_ਸੈੱਟ_casemap, expand_ਸੈੱਟ_casemap,
			    {(char_u *)"internal,keepascii", (char_u *)0L}
			    SCTX_INIT},
    {"cdhome",	    "cdh",  P_BOOL|P_VI_DEF|P_VIM|P_SECURE,
			    (char_u *)&p_cdh, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L}
			    SCTX_INIT},
    {"cdpath",	    "cd",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE|P_COMMA|P_NODUP,
			    (char_u *)&p_cdpath, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)",,", (char_u *)0L}
			    SCTX_INIT},
    {"cedit",	    ਨਿਰਾ,   P_STRING,
			    (char_u *)&p_cedit, PV_NONE, did_ਸੈੱਟ_cedit, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)CTRL_F_STR}
			    SCTX_INIT},
    {"charconvert",  "ccv", P_STRING|P_VI_DEF|P_SECURE,
#ਜੇ defined(FEAT_EVAL)
			    (char_u *)&p_ccv, PV_NONE, did_ਸੈੱਟ_optexpr, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"chistory",    "chi",  P_ਅੰਕ|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			    (char_u *)&p_chi, PV_NONE, did_ਸੈੱਟ_xhistory, ਨਿਰਾ,
			    {(char_u *)10L, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"cindent",	    "cin",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_cin, PV_CIN, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cinkeys",	    "cink", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cink, PV_CINK, ਨਿਰਾ, ਨਿਰਾ,
			    {INDENTKEYS_DEFAULT, (char_u *)0L} SCTX_INIT},
    {"cinoptions",  "cino", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cino, PV_CINO, did_ਸੈੱਟ_cinoptions, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"cinscopedecls", "cinsd", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cinsd, PV_CINSD, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"public,protected,private", (char_u *)0L}
			    SCTX_INIT},
    {"cinwords",    "cinw", P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cinw, PV_CINW, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"if,else,while,do,for,switch",
				(char_u *)0L}
			    SCTX_INIT},
    {"clipboard",   "cb",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
			    (char_u *)&p_cb, PV_NONE, did_ਸੈੱਟ_clipboard, expand_ਸੈੱਟ_clipboard,
# ਜੇ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
			    {(char_u *)"autoselect,exclude:cons\\|linux",
							       (char_u *)0L}
# ਨਹੀਂਤਾਂ
			    {(char_u *)"", (char_u *)0L}
# endif
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"clipmethod", "cpm",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
			    (char_u *)&p_cpm, PV_NONE, did_ਸੈੱਟ_clipmethod, expand_ਸੈੱਟ_clipmethod,
# ifdef UNIX
			    {(char_u *)"wayland,x11", (char_u *)0L}
# elif defined(VMS)
			    {(char_u *)"x11", (char_u *)0L}
# ਨਹੀਂਤਾਂ
			    {(char_u *)"", (char_u *)0L}
# endif
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"cmdheight",   "ch",   P_ਅੰਕ|P_VI_DEF|P_RALL,
			    (char_u *)&p_ch, PV_NONE, did_ਸੈੱਟ_cmdheight, ਨਿਰਾ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"cmdwinheight", "cwh", P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_cwh, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)7L, (char_u *)0L} SCTX_INIT},
    {"colorcolumn", "cc",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_RWIN|P_HLONLY,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CC, did_ਸੈੱਟ_colorcolumn, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"columns",	    "co",   P_ਅੰਕ|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RCLR,
			    (char_u *)&Columns, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)80L, (char_u *)0L} SCTX_INIT},
    {"comments",    "com",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_com, PV_COM, did_ਸੈੱਟ_comments, ਨਿਰਾ,
			    {(char_u *)"s1:/*,mb:*,ex:*/,://,b:#,:%,:XCOMM,n:>,fb:-",
				(char_u *)0L}
			    SCTX_INIT},
    {"commentstring", "cms", P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)&p_cms, PV_CMS, did_ਸੈੱਟ_commentstring, ਨਿਰਾ,
			    {(char_u *)"/* %s */", (char_u *)0L}
			    SCTX_INIT},
			    // P_PRI_MKRC isn't needed here, optval_default()
			    // always returns TRUE for 'compatible'
    {"compatible",  "cp",   P_BOOL|P_RALL,
			    (char_u *)&p_cp, PV_NONE, did_ਸੈੱਟ_compatible, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)FALSE} SCTX_INIT},
    {"complete",    "cpt",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP|P_MLE,
			    (char_u *)&p_cpt, PV_CPT, did_ਸੈੱਟ_complete, expand_ਸੈੱਟ_complete,
			    {(char_u *)".,w,b,u,t,i", (char_u *)0L}
			    SCTX_INIT},
    {"completefunc", "cfu", P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
			    (char_u *)&p_cfu, PV_CFU, did_ਸੈੱਟ_completefunc, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"completefuzzycollect", "cfc", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cfc, PV_NONE, did_ਸੈੱਟ_completefuzzycollect, expand_ਸੈੱਟ_completefuzzycollect,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"completeitemalign", "cia", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cia, PV_NONE, did_ਸੈੱਟ_completeitemalign, ਨਿਰਾ,
			    {(char_u *)"abbr,kind,menu", (char_u *)0L}
			    SCTX_INIT},
    {"completeopt",   "cot",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_cot, PV_COT, did_ਸੈੱਟ_completeopt, expand_ਸੈੱਟ_completeopt,
			    {(char_u *)"menu,preview", (char_u *)0L}
			    SCTX_INIT},
    {"completepopup", "cpp", P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON,
#ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
			    (char_u *)&p_cpp, PV_NONE, did_ਸੈੱਟ_completepopup, expand_ਸੈੱਟ_completepopup,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"completeslash",   "csl",  P_STRING|P_VI_DEF|P_VIM,
#ਜੇ defined(BACKSLASH_ਵਿੱਚ_FILENAME)
			    (char_u *)&p_csl, PV_CSL, did_ਸੈੱਟ_completeslash, expand_ਸੈੱਟ_completeslash,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"completetimeout", "cto", P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_cto, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"concealcursor","cocu", P_STRING|P_ALLOCED|P_RWIN|P_VI_DEF|P_FLAGLIST,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
			    (char_u *)VAR_WIN, PV_COCU, did_ਸੈੱਟ_concealcursor, expand_ਸੈੱਟ_concealcursor,
			    {(char_u *)"", (char_u *)ਨਿਰਾ}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"conceallevel","cole", P_ਅੰਕ|P_RWIN|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
			    (char_u *)VAR_WIN, PV_COLE, did_ਸੈੱਟ_conceallevel, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)0L, (char_u *)0L}
			    SCTX_INIT},
    {"confirm",     "cf",   P_BOOL|P_VI_DEF,
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			    (char_u *)&p_confirm, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"conskey",	    "consk",P_BOOL|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"copyindent",  "ci",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_ci, PV_CI, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cpoptions",   "cpo",  P_STRING|P_VIM|P_RALL|P_FLAGLIST,
			    (char_u *)&p_cpo, PV_NONE, did_ਸੈੱਟ_cpoptions, expand_ਸੈੱਟ_cpoptions,
			    {(char_u *)CPO_VI, (char_u *)CPO_VIM}
			    SCTX_INIT},
    {"cryptmethod", "cm",   P_STRING|P_ALLOCED|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
			    (char_u *)&p_cm, PV_CM, did_ਸੈੱਟ_cryptmethod, expand_ਸੈੱਟ_cryptmethod,
			    {(char_u *)"blowfish2", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"cscopepathcomp", "cspc", P_ਅੰਕ|P_VI_DEF|P_VIM,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
			    (char_u *)&p_cspc, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopeprg",   "csprg", P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
			    (char_u *)&p_csprg, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"cscope", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"cscopequickfix", "csqf", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ਜੇ defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
			    (char_u *)&p_csqf, PV_NONE,
			    did_ਸੈੱਟ_cscopequickfix, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"cscoperelative", "csre", P_BOOL|P_VI_DEF|P_VIM,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
			    (char_u *)&p_csre, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopetag",   "cst",  P_BOOL|P_VI_DEF|P_VIM,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
			    (char_u *)&p_cst, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopetagorder", "csto", P_ਅੰਕ|P_VI_DEF|P_VIM,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
			    (char_u *)&p_csto, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cscopeverbose", "csverb", P_BOOL|P_VI_DEF|P_VIM,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
			    (char_u *)&p_csverbose, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"cursorbind",  "crb",  P_BOOL|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_CRBIND, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cursorcolumn", "cuc", P_BOOL|P_VI_DEF|P_RWIN|P_HLONLY,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CUC, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cursorline",   "cul", P_BOOL|P_VI_DEF|P_RWIN|P_HLONLY,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CUL, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"cursorlineopt", "culopt", P_STRING|P_VI_DEF|P_RWIN|P_HLONLY|P_ONECOMMA|P_NODUP,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
			    (char_u *)VAR_WIN, PV_CULOPT,
			    did_ਸੈੱਟ_cursorlineopt, expand_ਸੈੱਟ_cursorlineopt,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)"both", (char_u *)0L} SCTX_INIT},
    {"debug",	    ਨਿਰਾ,   P_STRING|P_VI_DEF,
			    (char_u *)&p_debug, PV_NONE, did_ਸੈੱਟ_debug, expand_ਸੈੱਟ_debug,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"define",	    "def",  P_STRING|P_ALLOCED|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
			    (char_u *)&p_def, PV_DEF, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"^\\s*#\\s*define", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"delcombine", "deco",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_deco, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"dictionary",  "dict", P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA|P_NODUP|P_NDNAME,
			    (char_u *)&p_dict, PV_DICT, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"diff",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF|P_RWIN|P_NOGLOB,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
			    (char_u *)VAR_WIN, PV_DIFF, did_ਸੈੱਟ_diff, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"diffanchors", "dia",  P_STRING|P_VI_DEF|P_ONECOMMA,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
			    (char_u *)&p_dia, PV_DIA, did_ਸੈੱਟ_diffanchors, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)"", (char_u *)ਨਿਰਾ} SCTX_INIT},
    {"diffexpr",    "dex",  P_STRING|P_VI_DEF|P_SECURE|P_CURSWANT,
#ਜੇ defined(FEAT_DIFF) && defined(FEAT_EVAL)
			    (char_u *)&p_dex, PV_NONE, did_ਸੈੱਟ_optexpr, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"diffopt",	    "dip",  P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN|P_ONECOMMA|P_COLON
								     |P_NODUP,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
			    (char_u *)&p_dip, PV_NONE, did_ਸੈੱਟ_diffopt, expand_ਸੈੱਟ_diffopt,
			    {(char_u *)"internal,filler,closeoff,indent-heuristic,inline:char",
								(char_u *)ਨਿਰਾ}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)ਨਿਰਾ}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"digraph",	    "dg",   P_BOOL|P_VI_DEF|P_VIM,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
			    (char_u *)&p_dg, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"directory",   "dir",  P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA
							    |P_NODUP|P_SECURE,
			    (char_u *)&p_dir, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DFLT_DIR, (char_u *)0L} SCTX_INIT},
    {"display",	    "dy",   P_STRING|P_VI_DEF|P_ONECOMMA|P_RALL|P_NODUP,
			    (char_u *)&p_dy, PV_NONE, did_ਸੈੱਟ_display, expand_ਸੈੱਟ_display,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"eadirection", "ead",  P_STRING|P_VI_DEF,
			    (char_u *)&p_ead, PV_NONE, did_ਸੈੱਟ_eadirection, expand_ਸੈੱਟ_eadirection,
			    {(char_u *)"both", (char_u *)0L}
			    SCTX_INIT},
    {"edcompatible","ed",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ed, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"emoji",      "emo",   P_BOOL|P_VI_DEF|P_RCLR,
			    (char_u *)&p_emoji, PV_NONE, did_ਸੈੱਟ_ambiwidth, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L}
			    SCTX_INIT},
    {"encoding",    "enc",  P_STRING|P_VI_DEF|P_RCLR|P_NO_ML,
			    (char_u *)&p_enc, PV_NONE, did_ਸੈੱਟ_encoding, expand_ਸੈੱਟ_encoding,
			    {(char_u *)ENC_DFLT, (char_u *)0L}
			    SCTX_INIT},
    {"endoffile",   "eof",  P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_eof, PV_EOF,
			    did_ਸੈੱਟ_eof_eol_fixeol_bomb, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"endofline",   "eol",  P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_eol, PV_EOL,
			    did_ਸੈੱਟ_eof_eol_fixeol_bomb, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"equalalways", "ea",   P_BOOL|P_VI_DEF|P_RALL,
			    (char_u *)&p_ea, PV_NONE, did_ਸੈੱਟ_equalalways, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"equalprg",    "ep",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_ep, PV_EP, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"errorbells",  "eb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_eb, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"errorfile",   "ef",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			    (char_u *)&p_ef, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DFLT_ERRORFILE, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"errorformat", "efm",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			    (char_u *)&p_efm, PV_EFM, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DFLT_EFM, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"esckeys",	    "ek",   P_BOOL|P_VIM,
			    (char_u *)&p_ek, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"eventignore", "ei",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_ei, PV_NONE, did_ਸੈੱਟ_eventignore, expand_ਸੈੱਟ_eventignore,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"eventignorewin", "eiw",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)VAR_WIN, PV_EIW, did_ਸੈੱਟ_eventignore, expand_ਸੈੱਟ_eventignore,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"expandtab",   "et",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_et, PV_ET, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"exrc",	    "ex",   P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_exrc, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"fileencoding","fenc", P_STRING|P_ALLOCED|P_VI_DEF|P_RSTAT|P_RBUF
								   |P_NO_MKRC,
			    (char_u *)&p_fenc, PV_FENC, did_ਸੈੱਟ_encoding, expand_ਸੈੱਟ_encoding,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"fileencodings","fencs", P_STRING|P_VI_DEF|P_ONECOMMA,
			    (char_u *)&p_fencs, PV_NONE, ਨਿਰਾ, expand_ਸੈੱਟ_encoding,
			    {(char_u *)"ucs-bom", (char_u *)0L}
			    SCTX_INIT},
    {"fileformat",  "ff",   P_STRING|P_ALLOCED|P_VI_DEF|P_RSTAT|P_NO_MKRC
								  |P_CURSWANT,
			    (char_u *)&p_ff, PV_FF, did_ਸੈੱਟ_fileformat, expand_ਸੈੱਟ_fileformat,
			    {(char_u *)DFLT_FF, (char_u *)0L} SCTX_INIT},
    {"fileformats", "ffs",  P_STRING|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_ffs, PV_NONE, did_ਸੈੱਟ_fileformats, expand_ਸੈੱਟ_fileformat,
			    {(char_u *)DFLT_FFS_VI, (char_u *)DFLT_FFS_VIM}
			    SCTX_INIT},
    {"fileignorecase", "fic", P_BOOL|P_VI_DEF,
			    (char_u *)&p_fic, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CASE_INSENSITIVE_FILENAME
				    (char_u *)TRUE,
#ਨਹੀਂਤਾਂ
				    (char_u *)FALSE,
#ਜੇ_ਅੰਤ
					(char_u *)0L} SCTX_INIT},
    {"filetype",    "ft",   P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB|P_NFNAME,
			    (char_u *)&p_ft, PV_FT,
			    did_ਸੈੱਟ_filetype_or_syntax, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"fillchars",   "fcs",  P_STRING|P_VI_DEF|P_RALL|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_fcs, PV_FCS, did_ਸੈੱਟ_chars_option, expand_ਸੈੱਟ_chars_option,
			    {(char_u *)"vert:|,fold:-,eob:~,lastline:@",
								  (char_u *)0L}
			    SCTX_INIT},
    {"findfunc", "ffu",     P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
			    (char_u *)&p_ffu, PV_FFU,
			    did_ਸੈੱਟ_findfunc, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"fixendofline",  "fixeol", P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_fixeol, PV_FIXEOL,
			    did_ਸੈੱਟ_eof_eol_fixeol_bomb, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"fkmap",	    "fk",   P_BOOL|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"flash",	    "fl",   P_BOOL|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"foldclose",   "fcl",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_RWIN,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			    (char_u *)&p_fcl, PV_NONE, did_ਸੈੱਟ_foldclose, expand_ਸੈੱਟ_foldclose,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"foldcolumn",  "fdc",  P_ਅੰਕ|P_VI_DEF|P_RWIN,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDC, did_ਸੈੱਟ_foldcolumn, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"foldenable",  "fen",  P_BOOL|P_VI_DEF|P_RWIN,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FEN, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"foldexpr",    "fde",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN|P_MLE,
#ਜੇ defined(FEAT_FOLDING) && defined(FEAT_EVAL)
			    (char_u *)VAR_WIN, PV_FDE, did_ਸੈੱਟ_foldexpr, ਨਿਰਾ,
			    {(char_u *)"0", (char_u *)ਨਿਰਾ}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"foldignore",  "fdi",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDI, did_ਸੈੱਟ_foldignore, ਨਿਰਾ,
			    {(char_u *)"#", (char_u *)ਨਿਰਾ}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"foldlevel",   "fdl",  P_ਅੰਕ|P_VI_DEF|P_RWIN,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDL, did_ਸੈੱਟ_foldlevel, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"foldlevelstart","fdls", P_ਅੰਕ|P_VI_DEF|P_CURSWANT,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			    (char_u *)&p_fdls, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)-1L, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"foldmarker",  "fmr",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|
						    P_RWIN|P_ONECOMMA|P_NODUP,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FMR, did_ਸੈੱਟ_foldmarker, ਨਿਰਾ,
			    {(char_u *)"{{{,}}}", (char_u *)ਨਿਰਾ}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"foldmethod",  "fdm",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDM, did_ਸੈੱਟ_foldmethod, expand_ਸੈੱਟ_foldmethod,
			    {(char_u *)"manual", (char_u *)ਨਿਰਾ}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"foldminlines","fml",  P_ਅੰਕ|P_VI_DEF|P_RWIN,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FML, did_ਸੈੱਟ_foldminlines, ਨਿਰਾ,
			    {(char_u *)1L, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"foldnestmax", "fdn",  P_ਅੰਕ|P_VI_DEF|P_RWIN,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			    (char_u *)VAR_WIN, PV_FDN, did_ਸੈੱਟ_foldnestmax, ਨਿਰਾ,
			    {(char_u *)20L, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"foldopen",    "fdo",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_CURSWANT,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			    (char_u *)&p_fdo, PV_NONE, did_ਸੈੱਟ_foldopen, expand_ਸੈੱਟ_foldopen,
		 {(char_u *)"block,hor,mark,percent,quickfix,search,tag,undo",
						 (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"foldtext",    "fdt",  P_STRING|P_ALLOCED|P_VIM|P_VI_DEF|P_RWIN|P_MLE,
#ਜੇ defined(FEAT_FOLDING) && defined(FEAT_EVAL)
			    (char_u *)VAR_WIN, PV_FDT, did_ਸੈੱਟ_optexpr, ਨਿਰਾ,
			    {(char_u *)"foldtext()", (char_u *)ਨਿਰਾ}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"formatexpr", "fex",   P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_MLE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
			    (char_u *)&p_fex, PV_FEX, did_ਸੈੱਟ_optexpr, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"formatlistpat","flp", P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)&p_flp, PV_FLP, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"^\\s*\\d\\+[\\]:.)}\\t ]\\s*",
						 (char_u *)0L} SCTX_INIT},
    {"formatoptions","fo",  P_STRING|P_ALLOCED|P_VIM|P_FLAGLIST,
			    (char_u *)&p_fo, PV_FO, did_ਸੈੱਟ_formatoptions, expand_ਸੈੱਟ_formatoptions,
			    {(char_u *)DFLT_FO_VI, (char_u *)DFLT_FO_VIM}
			    SCTX_INIT},
    {"formatprg",   "fp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_fp, PV_FP, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"fsync",       "fs",   P_BOOL|P_SECURE|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FSYNC
			    (char_u *)&p_fs, PV_FS, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"gdefault",    "gd",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_gd, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"graphic",	    "gr",   P_BOOL|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"grepformat",  "gfm",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			    (char_u *)&p_gefm, PV_GEFM, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DFLT_GREPFORMAT, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"grepprg",	    "gp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			    (char_u *)&p_gp, PV_GP, ਨਿਰਾ, ਨਿਰਾ,
			    {
# ਜੇ defined(MSWIN)
			    // may be changed to "grep -n" in os_win32.c
			    (char_u *)"findstr /n",
# elif defined(UNIX)
			    // Add an extra file name so that grep will always
			    // insert a file name in the match line.
			    (char_u *)"grep -n $* /dev/null",
# elif defined(VMS)
			    (char_u *)"SEARCH/NUMBERS ",
# ਨਹੀਂਤਾਂ
			    (char_u *)"grep -n ",
# endif
			    (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"guicursor",    "gcr", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
			    (char_u *)&p_guicursor, PV_NONE,
			    did_ਸੈੱਟ_guicursor, ਨਿਰਾ,
			    {
# ifdef FEAT_GUI
				(char_u *)"n-v-c:block-Cursor/lCursor,ve:ver35-Cursor,o:hor50-Cursor,i-ci:ver25-Cursor/lCursor,r-cr:hor20-Cursor/lCursor,sm:block-Cursor-blinkwait175-blinkoff150-blinkon175",
# ਨਹੀਂਤਾਂ	// Win32 console
				(char_u *)"n-v-c:block,o:hor50,i-ci:hor15,r-cr:hor30,sm:block",
# endif
				    (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"guifont",	    "gfn",  P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP
#ਜੇ !defined(FEAT_GUI_GTK)
				|P_COLON
#ਜੇ_ਅੰਤ
				,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
			    (char_u *)&p_guifont, PV_NONE, did_ਸੈੱਟ_guifont, expand_ਸੈੱਟ_guifont,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"guifontset",  "gfs",  P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA,
#ਜੇ defined(FEAT_GUI) && defined(FEAT_XFONTSET)
			    (char_u *)&p_guifontset, PV_NONE,
			    did_ਸੈੱਟ_guifontset, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"guifontwide", "gfw",  P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP
#ਜੇ !defined(FEAT_GUI_GTK)
				|P_COLON
#ਜੇ_ਅੰਤ
				,
#ਜੇ defined(FEAT_GUI)
			    (char_u *)&p_guifontwide, PV_NONE,
			    did_ਸੈੱਟ_guifontwide, expand_ਸੈੱਟ_guifont,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},


    {"guiheadroom", "ghr",  P_ਅੰਕ|P_VI_DEF,
#ਜੇ (defined(FEAT_GUI_GTK) && !defined(USE_GTK4)) || defined(FEAT_GUI_X11)
			    (char_u *)&p_ghr, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)50L, (char_u *)0L} SCTX_INIT},

    {"guiligatures", "gli", P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP,
#ਜੇ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
			    (char_u *)&p_guiligatures, PV_NONE,
			    did_ਸੈੱਟ_guiligatures, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},

    {"guioptions",  "go",   P_STRING|P_VI_DEF|P_RALL|P_FLAGLIST,
#ਜੇ defined(FEAT_GUI)
			    (char_u *)&p_go, PV_NONE, did_ਸੈੱਟ_guioptions, expand_ਸੈੱਟ_guioptions,
# ਜੇ defined(UNIX)
			    {(char_u *)"aegimrLtT", (char_u *)0L}
# ਨਹੀਂਤਾਂ
			    {(char_u *)"egmrLtT", (char_u *)0L}
# endif
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"guipty",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF,
#ਜੇ defined(FEAT_GUI)
			    (char_u *)&p_guipty, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"guitablabel",  "gtl", P_STRING|P_VI_DEF|P_RWIN|P_MLE,
#ਜੇ defined(FEAT_GUI_TABLINE)
			    (char_u *)&p_gtl, PV_NONE, did_ਸੈੱਟ_guitablabel, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"guitabtooltip",  "gtt", P_STRING|P_VI_DEF|P_RWIN|P_MLE,
#ਜੇ defined(FEAT_GUI_TABLINE)
			    (char_u *)&p_gtt, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"hardtabs",    "ht",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"helpfile",    "hf",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_hf, PV_NONE, did_ਸੈੱਟ_helpfile, ਨਿਰਾ,
			    {(char_u *)DFLT_HELPFILE, (char_u *)0L}
			    SCTX_INIT},
    {"helpheight",  "hh",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_hh, PV_NONE,
			    did_ਸੈੱਟ_winheight_helpheight, ਨਿਰਾ,
			    {(char_u *)20L, (char_u *)0L} SCTX_INIT},
    {"helplang",    "hlg",  P_STRING|P_VI_DEF|P_ONECOMMA,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
			    (char_u *)&p_hlg, PV_NONE, did_ਸੈੱਟ_helplang, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"hidden",	    "hid",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_hid, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"highlight",   "hl",   P_STRING|P_VI_DEF|P_RCLR|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_hl, PV_NONE, did_ਸੈੱਟ_highlight, expand_ਸੈੱਟ_highlight,
			    {(char_u *)HIGHLIGHT_INIT, (char_u *)0L}
			    SCTX_INIT},
    {"history",	    "hi",   P_ਅੰਕ|P_VIM,
			    (char_u *)&p_hi, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)200L} SCTX_INIT},
    {"hkmap",	    "hk",   P_BOOL|P_VI_DEF|P_VIM,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
			    (char_u *)&p_hkmap, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"hkmapp",	    "hkp",  P_BOOL|P_VI_DEF|P_VIM,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
			    (char_u *)&p_hkmapp, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"hlsearch",    "hls",  P_BOOL|P_VI_DEF|P_VIM|P_RALL|P_HLONLY,
#ਜੇ defined(FEAT_ਖੋਜ_EXTRA)
			    (char_u *)&p_hls, PV_NONE, did_ਸੈੱਟ_hlsearch, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"icon",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_icon, PV_NONE, did_ਸੈੱਟ_title_icon, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"iconstring",  ਨਿਰਾ,   P_STRING|P_VI_DEF|P_MLE,
			    (char_u *)&p_iconstring, PV_NONE,
			    did_ਸੈੱਟ_iconstring, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"ignorecase",  "ic",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ic, PV_NONE, did_ਸੈੱਟ_ignorecase, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"imactivatefunc","imaf",P_STRING|P_VI_DEF|P_SECURE|P_FUNC,
#ਜੇ defined(FEAT_EVAL) && \
	(defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
			    (char_u *)&p_imaf, PV_NONE,
			    did_ਸੈੱਟ_imactivatefunc, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)ਨਿਰਾ}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"imactivatekey","imak",P_STRING|P_VI_DEF,
#ਜੇ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
			    (char_u *)&p_imak, PV_NONE,
			    did_ਸੈੱਟ_imactivatekey, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"imageprotocol", "ipc",P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_COLON,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
			    (char_u *)&p_ipc, PV_NONE, did_ਸੈੱਟ_imageprotocol, expand_ਸੈੱਟ_imageprotocol,
			    {(char_u *)"kitty:kitty,foot:sixel,ghostty:kitty,wezterm:kitty,xterm:sixel", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"imcmdline",   "imc",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_imcmdline, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"imdisable",   "imd",  P_BOOL|P_VI_DEF,
#ਜੇ defined(HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ)
			    (char_u *)&p_imdisable, PV_NONE,
			    did_ਸੈੱਟ_imdisable, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L}
			    SCTX_INIT},
    {"iminsert",    "imi",  P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_iminsert, PV_IMI, did_ਸੈੱਟ_iminsert, ਨਿਰਾ,
			    {(char_u *)B_IMODE_NONE, (char_u *)0L}
			    SCTX_INIT},
    {"imsearch",    "ims",  P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_imsearch, PV_IMS, did_ਸੈੱਟ_imsearch, ਨਿਰਾ,
			    {(char_u *)B_IMODE_USE_ਪਾਓ, (char_u *)0L}
			    SCTX_INIT},
    {"imstatusfunc","imsf",P_STRING|P_VI_DEF|P_SECURE|P_FUNC,
#ਜੇ defined(FEAT_EVAL) && \
	(defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
			    (char_u *)&p_imsf, PV_NONE, did_ਸੈੱਟ_imstatusfunc, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)ਨਿਰਾ}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"imstyle",	    "imst", P_ਅੰਕ|P_VI_DEF|P_SECURE,
#ਜੇ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
			    (char_u *)&p_imst, PV_NONE, did_ਸੈੱਟ_imstyle, ਨਿਰਾ,
			    {(char_u *)IM_OVER_THE_SPOT, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"include",	    "inc",  P_STRING|P_ALLOCED|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
			    (char_u *)&p_inc, PV_INC, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"^\\s*#\\s*include", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"includeexpr", "inex", P_STRING|P_ALLOCED|P_VI_DEF|P_MLE,
#ਜੇ defined(FEAT_ਲੱਭੋ_ID) && defined(FEAT_EVAL)
			    (char_u *)&p_inex, PV_INEX, did_ਸੈੱਟ_optexpr, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"incsearch",   "is",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_is, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"indentexpr", "inde",  P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_MLE,
#ਜੇ defined(FEAT_EVAL)
			    (char_u *)&p_inde, PV_INDE, did_ਸੈੱਟ_optexpr, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"indentkeys", "indk",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ਜੇ defined(FEAT_EVAL)
			    (char_u *)&p_indk, PV_INDK, ਨਿਰਾ, ਨਿਰਾ,
			    {INDENTKEYS_DEFAULT, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"infercase",   "inf",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_inf, PV_INF, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"insertmode",  "im",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_im, PV_NONE, did_ਸੈੱਟ_insertmode, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"isfname",	    "isf",  P_STRING|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_isf, PV_NONE, did_ਸੈੱਟ_isopt, ਨਿਰਾ,
			    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
				// Excluded are: & and ^ are special in cmd.exe
				// ( and ) are used in text separating fnames
			    (char_u *)"@,48-57,/,\\,.,-,_,+,,,#,$,%,{,},[,],:,@-@,!,~,=",
#ਨਹੀਂਤਾਂ
# ਜੇ defined(AMIGA)
			    (char_u *)"@,48-57,/,.,-,_,+,,,$,:",
# elif defined(VMS)
			    (char_u *)"@,48-57,/,.,-,_,+,,,#,$,%,<,>,[,],:,;,~",
# ਨਹੀਂਤਾਂ // UNIX et al.
			    (char_u *)"@,48-57,/,.,-,_,+,,,#,$,%,~,=",
# endif
#ਜੇ_ਅੰਤ
				(char_u *)0L} SCTX_INIT},
    {"isident",	    "isi",  P_STRING|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_isi, PV_NONE, did_ਸੈੱਟ_isopt, ਨਿਰਾ,
			    {
#ਜੇ defined(MSWIN)
			    (char_u *)"@,48-57,_,128-167,224-235",
#ਨਹੀਂਤਾਂ
			    (char_u *)"@,48-57,_,192-255",
#ਜੇ_ਅੰਤ
				(char_u *)0L} SCTX_INIT},
    {"iskeyword",   "isk",  P_STRING|P_ALLOCED|P_VIM|P_COMMA|P_NODUP,
			    (char_u *)&p_isk, PV_ISK, did_ਸੈੱਟ_iskeyword, ਨਿਰਾ,
			    {
				(char_u *)"@,48-57,_",
#ਜੇ defined(MSWIN)
				(char_u *)"@,48-57,_,128-167,224-235"
#ਨਹੀਂਤਾਂ
				ISK_LATIN1
#ਜੇ_ਅੰਤ
			    } SCTX_INIT},
    {"isprint",	    "isp",  P_STRING|P_VI_DEF|P_RALL|P_COMMA|P_NODUP,
			    (char_u *)&p_isp, PV_NONE, did_ਸੈੱਟ_isopt, ਨਿਰਾ,
			    {
#ਜੇ defined(MSWIN) || defined(VMS)
			    (char_u *)"@,~-255",
#ਨਹੀਂਤਾਂ
			    ISP_LATIN1,
#ਜੇ_ਅੰਤ
				(char_u *)0L} SCTX_INIT},
    {"joinspaces",  "js",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_js, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"jumpoptions", "jop",  P_STRING|P_VI_DEF|P_VIM|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_jop, PV_NONE, did_ਸੈੱਟ_jumpoptions, expand_ਸੈੱਟ_jumpoptions,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"key",	    ਨਿਰਾ,   P_STRING|P_ALLOCED|P_VI_DEF|P_NO_MKRC|P_NO_CMD_EXPAND,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
			    (char_u *)&p_ਕੁੰਜੀ, PV_ਕੁੰਜੀ, did_ਸੈੱਟ_cryptkey, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"keymap",	    "kmp",  P_STRING|P_ALLOCED|P_VI_DEF|P_RBUF|P_RSTAT|P_NFNAME|P_PRI_MKRC,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
			    (char_u *)&p_keymap, PV_KMAP, did_ਸੈੱਟ_keymap, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"keymodel",    "km",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_km, PV_NONE, did_ਸੈੱਟ_keymodel, expand_ਸੈੱਟ_keymodel,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"keyprotocol", "kpc",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_kpc, PV_NONE, did_ਸੈੱਟ_keyprotocol, expand_ਸੈੱਟ_keyprotocol,
			    {(char_u *)"kitty:kitty,foot:kitty,ghostty:kitty,wezterm:kitty,xterm:mok2", (char_u *)0L}
			    SCTX_INIT},
    {"keywordprg",  "kp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_kp, PV_KP, ਨਿਰਾ, ਨਿਰਾ,
			    {
#ਜੇ defined(MSWIN)
			    (char_u *)":help",
#ਜਾਂਜੇ defined(VMS)
			    (char_u *)"help",
#ਜਾਂਜੇ defined(USEMAN_S)
			    (char_u *)"man -s",
#ਨਹੀਂਤਾਂ
			    (char_u *)"man",
#ਜੇ_ਅੰਤ
				(char_u *)0L} SCTX_INIT},
    {"langmap",     "lmap", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_SECURE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LANGMAP
			    (char_u *)&p_langmap, PV_NONE, did_ਸੈੱਟ_langmap, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"langmenu",    "lm",   P_STRING|P_VI_DEF|P_NFNAME,
#ਜੇ defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
			    (char_u *)&p_lm, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"langnoremap",  "lnr",   P_BOOL|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LANGMAP
			    (char_u *)&p_lnr, PV_NONE, did_ਸੈੱਟ_langnoremap, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"langremap",  "lrm",   P_BOOL|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LANGMAP
			    (char_u *)&p_lrm, PV_NONE, did_ਸੈੱਟ_langremap, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"laststatus",  "ls",   P_ਅੰਕ|P_VI_DEF|P_RALL,
			    (char_u *)&p_ls, PV_NONE, did_ਸੈੱਟ_laststatus, ਨਿਰਾ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"lazyredraw",  "lz",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_lz, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"lhistory", "lhi",	    P_ਅੰਕ|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			    (char_u *)VAR_WIN, PV_LHI, did_ਸੈੱਟ_xhistory, ਨਿਰਾ,
			    {(char_u *)10L, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"linebreak",   "lbr",  P_BOOL|P_VI_DEF|P_RWIN,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_LBR, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"lines",	    ਨਿਰਾ,   P_ਅੰਕ|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RCLR,
			    (char_u *)&ਪੰਕਤੀਆਂ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {
#ਜੇ defined(MSWIN)
			    (char_u *)25L,
#ਨਹੀਂਤਾਂ
			    (char_u *)24L,
#ਜੇ_ਅੰਤ
					    (char_u *)0L} SCTX_INIT},
    {"linespace",   "lsp",  P_ਅੰਕ|P_VI_DEF|P_RCLR,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
			    (char_u *)&p_linespace, PV_NONE,
			    did_ਸੈੱਟ_linespace, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
			    {(char_u *)1L, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"lisp",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_lisp, PV_LISP, did_ਸੈੱਟ_lisp, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"lispoptions", "lop",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_lop, PV_LOP, did_ਸੈੱਟ_lispoptions, expand_ਸੈੱਟ_lispoptions,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"lispwords",   "lw",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_lispwords, PV_LW, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)LISPWORD_ਮੁੱਲ, (char_u *)0L} SCTX_INIT},
    {"list",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_LIST, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"listchars",   "lcs",  P_STRING|P_VI_DEF|P_RALL|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_lcs, PV_LCS, did_ਸੈੱਟ_chars_option, expand_ਸੈੱਟ_chars_option,
			    {(char_u *)"eol:$", (char_u *)0L} SCTX_INIT},
    {"loadplugins", "lpl",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_lpl, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"luadll",      ਨਿਰਾ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ defined(DYNAMIC_LUA)
			    (char_u *)&p_luadll, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DYNAMIC_LUA_DLL, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"macatsui",    ਨਿਰਾ,   P_BOOL|P_VI_DEF|P_RCLR,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"magic",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_magic, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"makeef",	    "mef",  P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			    (char_u *)&p_mef, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"makeencoding","menc", P_STRING|P_VI_DEF,
			    (char_u *)&p_menc, PV_MENC, did_ਸੈੱਟ_encoding, expand_ਸੈੱਟ_encoding,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"makeprg",	    "mp",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			    (char_u *)&p_mp, PV_MP, ਨਿਰਾ, ਨਿਰਾ,
# ifdef VMS
			    {(char_u *)"MMS", (char_u *)0L}
# ਨਹੀਂਤਾਂ
			    {(char_u *)"make", (char_u *)0L}
# endif
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"matchpairs",  "mps",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_mps, PV_MPS, did_ਸੈੱਟ_matchpairs, ਨਿਰਾ,
			    {(char_u *)"(:),{:},[:]", (char_u *)0L}
			    SCTX_INIT},
    {"matchtime",   "mat",  P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_mat, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)5L, (char_u *)0L} SCTX_INIT},
    {"maxcombine",  "mco",  P_ਅੰਕ|P_VI_DEF|P_CURSWANT,
			    (char_u *)&p_mco, PV_NONE, did_ਸੈੱਟ_maxcombine, ਨਿਰਾ,
			    {(char_u *)2, (char_u *)0L} SCTX_INIT},
    {"maxfuncdepth", "mfd", P_ਅੰਕ|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
			    (char_u *)&p_mfd, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)100L, (char_u *)0L} SCTX_INIT},
    {"maxmapdepth", "mmd",  P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_mmd, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)1000L, (char_u *)0L} SCTX_INIT},
    {"maxmem",	    "mm",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_mm, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DFLT_MAXMEM, (char_u *)0L}
			    SCTX_INIT},
    {"maxmempattern","mmp", P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_mmp, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)1000L, (char_u *)0L} SCTX_INIT},
    {"maxmemtot",   "mmt",  P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_mmt, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DFLT_MAXMEMTOT, (char_u *)0L}
			    SCTX_INIT},
    {"maxsearchcount", "msc", P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_msc, PV_NONE, did_ਸੈੱਟ_maxsearchcount, ਨਿਰਾ,
			    {(char_u *)99L, (char_u *)0L} SCTX_INIT},
    {"menuitems",   "mis",  P_ਅੰਕ|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
			    (char_u *)&p_mis, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)25L, (char_u *)0L} SCTX_INIT},
    {"mesg",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"messagesopt","mopt",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_COLON|P_NODUP,
			    (char_u *)&p_mopt, PV_NONE, did_ਸੈੱਟ_messagesopt, expand_ਸੈੱਟ_messagesopt,
			    {(char_u *)"hit-enter,history:500", (char_u *)ਨਿਰਾ} SCTX_INIT},
    {"mkspellmem",  "msm",  P_STRING|P_VI_DEF|P_EXPAND|P_SECURE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
			    (char_u *)&p_msm, PV_NONE, did_ਸੈੱਟ_mkspellmem, ਨਿਰਾ,
			    {(char_u *)"460000,2000,500", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"modeline",    "ml",   P_BOOL|P_VIM,
			    (char_u *)&p_ml, PV_ML, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"modelineexpr", "mle",  P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_mle, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"modelines",   "mls",  P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_mls, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)5L, (char_u *)0L} SCTX_INIT},
    {"modelinestrict", "mlst", P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_mlstr, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"modifiable",  "ma",   P_BOOL|P_VI_DEF|P_NOGLOB,
			    (char_u *)&p_ma, PV_MA, did_ਸੈੱਟ_modifiable, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"modified",    "mod",  P_BOOL|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_mod, PV_MOD, did_ਸੈੱਟ_modified, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"more",	    ਨਿਰਾ,   P_BOOL|P_VIM,
			    (char_u *)&p_ਹੋਰ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"mouse",	    ਨਿਰਾ,   P_STRING|P_VI_DEF|P_FLAGLIST,
			    (char_u *)&p_mouse, PV_NONE, did_ਸੈੱਟ_mouse, expand_ਸੈੱਟ_mouse,
			    {
#ਜੇ defined(MSWIN)
				(char_u *)"a",
#ਨਹੀਂਤਾਂ
				(char_u *)"",
#ਜੇ_ਅੰਤ
				(char_u *)0L} SCTX_INIT},
    {"mousefocus",   "mousef", P_BOOL|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
			    (char_u *)&p_mousef, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"mousehide",   "mh",   P_BOOL|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
			    (char_u *)&p_mh, PV_NONE, did_ਸੈੱਟ_mousehide, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"mousemodel",  "mousem", P_STRING|P_VI_DEF,
			    (char_u *)&p_mousem, PV_NONE, did_ਸੈੱਟ_mousemodel, expand_ਸੈੱਟ_mousemodel,
			    {
#ਜੇ defined(MSWIN)
				(char_u *)"popup",
#ਨਹੀਂਤਾਂ
# ਜੇ defined(MACOS_X)
				(char_u *)"popup_setpos",
# ਨਹੀਂਤਾਂ
				(char_u *)"extend",
# endif
#ਜੇ_ਅੰਤ
				(char_u *)0L} SCTX_INIT},
    {"mousemoveevent",   "mousemev",   P_BOOL|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
			    (char_u *)&p_mousemev, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"mouseshape",  "mouses",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
			    (char_u *)&p_mouseshape, PV_NONE,
			    did_ਸੈੱਟ_mouseshape, ਨਿਰਾ,
			    {(char_u *)"i-r:beam,s:updown,sd:udsizing,vs:leftright,vd:lrsizing,m:no,ml:up-arrow,v:rightup-arrow", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"mousetime",   "mouset",	P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_mouset, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)500L, (char_u *)0L} SCTX_INIT},
    {"mzquantum",  "mzq",   P_ਅੰਕ,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
			    (char_u *)&p_mzq, PV_NONE, did_ਸੈੱਟ_mzquantum, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)100L, (char_u *)100L} SCTX_INIT},
    {"mzschemedll", ਨਿਰਾ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ defined(DYNAMIC_MZSCHEME)
			    (char_u *)&p_mzschemedll, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DYNAMIC_MZSCH_DLL, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"mzschemegcdll", ਨਿਰਾ, P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ defined(DYNAMIC_MZSCHEME)
			    (char_u *)&p_mzschemegcdll, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DYNAMIC_MZGC_DLL, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"novice",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"nrformats",   "nf",   P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_nf, PV_NF, did_ਸੈੱਟ_nrformats, expand_ਸੈੱਟ_nrformats,
			    {(char_u *)"bin,octal,hex", (char_u *)0L}
			    SCTX_INIT},
    {"number",	    "nu",   P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_NU,
			    did_ਸੈੱਟ_ਸੰਖਿਆ_relativenumber, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"numberwidth", "nuw",  P_ਅੰਕ|P_RWIN|P_VIM,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
			    (char_u *)VAR_WIN, PV_NUW, did_ਸੈੱਟ_numberwidth, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)8L, (char_u *)4L} SCTX_INIT},
    {"omnifunc",    "ofu",  P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
			    (char_u *)&p_ofu, PV_OFU, did_ਸੈੱਟ_omnifunc, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"open",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"opendevice",  "odev", P_BOOL|P_VI_DEF,
#ਜੇ defined(MSWIN)
			    (char_u *)&p_odev, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)FALSE}
			    SCTX_INIT},
    {"operatorfunc", "opfunc", P_STRING|P_VI_DEF|P_SECURE|P_FUNC,
			    (char_u *)&p_opfunc, PV_NONE,
			    did_ਸੈੱਟ_operatorfunc, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"optimize",    "opt",  P_BOOL|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"osctimeoutlen", "ost", P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_ost, PV_NONE, did_ਸੈੱਟ_osctimeoutlen, ਨਿਰਾ,
			    {(char_u *)1000, (char_u *)0L} SCTX_INIT},
    {"osfiletype",  "oft",  P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"packpath",    "pp",   P_STRING|P_VI_DEF|P_EXPAND|P_ONECOMMA|P_NODUP
								    |P_SECURE,
			    (char_u *)&p_pp, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DFLT_RUNTIMEPATH, (char_u *)0L}
			    SCTX_INIT},
    {"paragraphs",  "para", P_STRING|P_VI_DEF,
			    (char_u *)&p_para, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"IPLPPPQPP TPHPLIPpLpItpplpipbp",
				(char_u *)0L} SCTX_INIT},
    {"paste",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF|P_PRI_MKRC,
			    (char_u *)&p_paste, PV_NONE, did_ਸੈੱਟ_paste, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"pastetoggle", "pt",   P_STRING|P_VI_DEF,
			    (char_u *)&p_pt, PV_NONE, did_ਸੈੱਟ_pastetoggle, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"patchexpr",   "pex",  P_STRING|P_VI_DEF|P_SECURE,
#ਜੇ defined(FEAT_DIFF) && defined(FEAT_EVAL)
			    (char_u *)&p_pex, PV_NONE, did_ਸੈੱਟ_optexpr, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"patchmode",   "pm",   P_STRING|P_VI_DEF|P_NFNAME,
			    (char_u *)&p_pm, PV_NONE,
			    did_ਸੈੱਟ_backupext_or_patchmode, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"path",	    "pa",   P_STRING|P_EXPAND|P_VI_DEF|P_COMMA|P_NODUP,
			    (char_u *)&p_ਰਸਤਾ, PV_ਰਸਤਾ, ਨਿਰਾ, ਨਿਰਾ,
			    {
#ਜੇ defined(AMIGA) || defined(MSWIN)
			    (char_u *)".,,",
#ਨਹੀਂਤਾਂ
			    (char_u *)".,/usr/include,,",
#ਜੇ_ਅੰਤ
				(char_u *)0L} SCTX_INIT},
    {"perldll",     ਨਿਰਾ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ defined(DYNAMIC_PERL)
			    (char_u *)&p_perldll, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DYNAMIC_PERL_DLL, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"preserveindent", "pi", P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_pi, PV_PI, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"previewheight", "pvh", P_ਅੰਕ|P_VI_DEF,
#ਜੇ defined(FEAT_QUICKFIX)
			    (char_u *)&p_pvh, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)12L, (char_u *)0L} SCTX_INIT},
    {"previewpopup", "pvp", P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
			    (char_u *)&p_pvp, PV_NONE, did_ਸੈੱਟ_previewpopup, expand_ਸੈੱਟ_previewpopup,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"previewwindow", "pvw", P_BOOL|P_VI_DEF|P_RSTAT|P_NOGLOB,
#ਜੇ defined(FEAT_QUICKFIX)
			    (char_u *)VAR_WIN, PV_PVW, did_ਸੈੱਟ_previewwindow, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"printdevice", "pdev", P_STRING|P_VI_DEF|P_SECURE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PRINTER
			    (char_u *)&p_pdev, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"printencoding", "penc", P_STRING|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_POSTSCRIPT
			    (char_u *)&p_penc, PV_NONE,
			    did_ਸੈੱਟ_printencoding, expand_ਸੈੱਟ_encoding,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"printexpr", "pexpr",  P_STRING|P_VI_DEF|P_SECURE,
#ਜੇ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
			    (char_u *)&p_pexpr, PV_NONE, did_ਸੈੱਟ_optexpr, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"printfont", "pfn",    P_STRING|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PRINTER
			    (char_u *)&p_pfn, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {
# ifdef MSWIN
				(char_u *)"Courier_New:h10",
# elif defined(FEAT_PRINT_PANGO)
				(char_u *)"Courier 10",
# ਨਹੀਂਤਾਂ
				(char_u *)"courier",
# endif
				(char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"printheader", "pheader",  P_STRING|P_VI_DEF|P_GETTEXT|P_MLE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PRINTER
			    (char_u *)&p_ਸਿਰਲੇਖ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    // untranslated to avoid problems when 'encoding'
			    // is changed
			    {(char_u *)"%<%f%h%m%=Page %N", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"printmbcharset", "pmbcs",  P_STRING|P_VI_DEF,
#ਜੇ defined(FEAT_POSTSCRIPT)
			    (char_u *)&p_pmcs, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"printmbfont", "pmbfn",  P_STRING|P_VI_DEF,
#ਜੇ defined(FEAT_POSTSCRIPT)
			    (char_u *)&p_pmfn, PV_NONE, parse_printmbfont, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"printoptions", "popt", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PRINTER
			    (char_u *)&p_popt, PV_NONE, parse_printoptions, expand_ਸੈੱਟ_printoptions,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"prompt",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_prompt, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"pumborder",   "pb",   P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON|P_SECURE,
			    (char_u *)&p_pb, PV_NONE, did_ਸੈੱਟ_pumborder, expand_ਸੈੱਟ_pumborder,
			    {(char_u *)"", (char_u *)ਨਿਰਾ} SCTX_INIT},
    {"pumheight",   "ph",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_ph, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"pummaxwidth", "pmw",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_pmw, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"pumopt",	    ਨਿਰਾ,   P_STRING|P_VI_DEF|P_COMMA|P_NODUP|P_COLON,
			    (char_u *)&p_pumopt, PV_NONE,
			    did_ਸੈੱਟ_pumopt, expand_ਸੈੱਟ_pumopt,
			    {(char_u *)"", (char_u *)ਨਿਰਾ} SCTX_INIT},
    {"pumwidth",    "pw",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_pw, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)15L, (char_u *)15L} SCTX_INIT},
    {"pythondll",   ਨਿਰਾ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ defined(DYNAMIC_PYTHON)
			    (char_u *)&p_pydll, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DYNAMIC_PYTHON_DLL, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"pythonhome",  ਨਿਰਾ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ defined(FEAT_PYTHON)
			    (char_u *)&p_pyhome, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"pythonthreedll",  ਨਿਰਾ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ defined(DYNAMIC_PYTHON3)
			    (char_u *)&p_py3dll, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DYNAMIC_PYTHON3_DLL, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"pythonthreehome", ਨਿਰਾ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ defined(FEAT_PYTHON3)
			    (char_u *)&p_py3home, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"pyxversion", "pyx",   P_ਅੰਕ|P_VI_DEF|P_SECURE,
#ਜੇ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
			    (char_u *)&p_pyx, PV_NONE, did_ਸੈੱਟ_pyxversion, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)DEFAULT_PYTHON_VER, (char_u *)0L}
			    SCTX_INIT},
    {"quickfixtextfunc", "qftf", P_STRING|P_ALLOCED|P_VI_DEF|P_VIM|P_SECURE|P_FUNC,
#ਜੇ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
			    (char_u *)&p_qftf, PV_NONE,
			    did_ਸੈੱਟ_quickfixtextfunc, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)ਨਿਰਾ}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"quoteescape", "qe",   P_STRING|P_ALLOCED|P_VI_DEF,
			    (char_u *)&p_qe, PV_QE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"\\", (char_u *)0L} SCTX_INIT},
    {"readonly",    "ro",   P_BOOL|P_VI_DEF|P_RSTAT|P_NOGLOB,
			    (char_u *)&p_ro, PV_RO, did_ਸੈੱਟ_readonly, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"redraw",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"redrawtime",  "rdt",  P_ਅੰਕ|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
			    (char_u *)&p_rdt, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)2000L, (char_u *)0L} SCTX_INIT},
    {"regexpengine", "re",  P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_re, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"relativenumber", "rnu", P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_RNU,
			    did_ਸੈੱਟ_ਸੰਖਿਆ_relativenumber, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"remap",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_remap, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"renderoptions", "rop", P_STRING|P_ONECOMMA|P_RCLR|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RENDER_OPTIONS
			    (char_u *)&p_rop, PV_NONE, did_ਸੈੱਟ_renderoptions, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"report",	    ਨਿਰਾ,   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_report, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)2L, (char_u *)0L} SCTX_INIT},
    {"restorescreen", "rs", P_BOOL|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
			    (char_u *)&p_rs, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"revins",	    "ri",   P_BOOL|P_VI_DEF|P_VIM,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
			    (char_u *)&p_ri, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"rightleft",   "rl",   P_BOOL|P_VI_DEF|P_RWIN,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
			    (char_u *)VAR_WIN, PV_RL, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"rightleftcmd", "rlc", P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
			    (char_u *)VAR_WIN, PV_RLC, did_ਸੈੱਟ_rightleftcmd, expand_ਸੈੱਟ_rightleftcmd,
			    {(char_u *)"search", (char_u *)ਨਿਰਾ}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"rubydll",     ਨਿਰਾ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ defined(DYNAMIC_RUBY)
			    (char_u *)&p_rubydll, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DYNAMIC_RUBY_DLL, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"ruler",	    "ru",   P_BOOL|P_VIM|P_RSTAT,
			    (char_u *)&p_ru, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"rulerformat", "ruf",  P_STRING|P_VI_DEF|P_ALLOCED|P_RSTAT|P_MLE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
			    (char_u *)&p_ruf, PV_NONE, did_ਸੈੱਟ_rulerformat, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"runtimepath", "rtp",  P_STRING|P_VI_DEF|P_EXPAND|P_ONECOMMA|P_NODUP
								    |P_SECURE,
			    (char_u *)&p_rtp, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DFLT_RUNTIMEPATH, (char_u *)0L}
			    SCTX_INIT},
    {"scroll",	    "scr",  P_ਅੰਕ|P_NO_MKRC|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_SCROLL, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"scrollbind",  "scb",  P_BOOL|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_SCBIND, did_ਸੈੱਟ_scrollbind, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"scrollfocus", "scf",  P_BOOL|P_VI_DEF,
#ਜੇ defined(MSWIN) && defined(FEAT_GUI)
			    (char_u *)&p_scf, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"scrolljump",  "sj",   P_ਅੰਕ|P_VI_DEF|P_VIM,
			    (char_u *)&p_sj, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"scrolloff",   "so",   P_ਅੰਕ|P_VI_DEF|P_VIM|P_RALL,
			    (char_u *)&p_so, PV_SO, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"scrolloffpad", "sop", P_ਅੰਕ|P_VI_DEF|P_VIM|P_RALL,
				(char_u *)&p_sop, PV_SOP, ਨਿਰਾ, ਨਿਰਾ,
				{(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"scrollopt",   "sbo",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_sbo, PV_NONE, did_ਸੈੱਟ_scrollopt, expand_ਸੈੱਟ_scrollopt,
			    {(char_u *)"ver,jump", (char_u *)0L}
			    SCTX_INIT},
    {"sections",    "sect", P_STRING|P_VI_DEF,
			    (char_u *)&p_sections, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"SHNHH HUnhsh", (char_u *)0L}
			    SCTX_INIT},
    {"secure",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_secure, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"selection",   "sel",  P_STRING|P_VI_DEF,
			    (char_u *)&p_sel, PV_NONE, did_ਸੈੱਟ_selection, expand_ਸੈੱਟ_selection,
			    {(char_u *)"inclusive", (char_u *)0L}
			    SCTX_INIT},
    {"selectmode",  "slm",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_slm, PV_NONE, did_ਸੈੱਟ_selectmode, expand_ਸੈੱਟ_selectmode,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"sessionoptions", "ssop", P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
			    (char_u *)&p_ssop, PV_NONE,
			    did_ਸੈੱਟ_sessionoptions, expand_ਸੈੱਟ_sessionoptions,
	 {(char_u *)"blank,buffers,curdir,folds,help,options,tabpages,winsize,terminal",
							       (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"shell",	    "sh",   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_sh, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
			    (char_u *)"-",
#ਨਹੀਂਤਾਂ
# ਜੇ defined(MSWIN)
			    (char_u *)"",	// set in set_init_1()
# ਨਹੀਂਤਾਂ
			    (char_u *)"sh",
# endif
#ਜੇ_ਅੰਤ // VMS
				(char_u *)0L} SCTX_INIT},
    {"shellcmdflag","shcf", P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_shcf, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {
#ਜੇ defined(MSWIN)
			    (char_u *)"/c",
#ਨਹੀਂਤਾਂ
			    (char_u *)"-c",
#ਜੇ_ਅੰਤ
				(char_u *)0L} SCTX_INIT},
    {"shellpipe",   "sp",   P_STRING|P_VI_DEF|P_SECURE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			    (char_u *)&p_sp, PV_NONE, did_ਸੈੱਟ_shellpipe_redir, ਨਿਰਾ,
			    {
# ਜੇ defined(UNIX)
			    (char_u *)"| tee",
# ਨਹੀਂਤਾਂ
			    (char_u *)">",
# endif
				(char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"shellquote",  "shq",  P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_shq, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"shellredir",  "srr",  P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_srr, PV_NONE, did_ਸੈੱਟ_shellpipe_redir, ਨਿਰਾ,
			    {(char_u *)">", (char_u *)0L} SCTX_INIT},
    {"shellslash",  "ssl",   P_BOOL|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
			    (char_u *)&p_ssl, PV_NONE, did_ਸੈੱਟ_shellslash, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"shelltemp",   "stmp", P_BOOL,
			    (char_u *)&p_stmp, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"shelltype",   "st",   P_ਅੰਕ|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
			    (char_u *)&p_st, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"shellxescape", "sxe", P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_sxe, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {
#ਜੇ defined(MSWIN)
			    (char_u *)"\"&|<>()@^",
#ਨਹੀਂਤਾਂ
			    (char_u *)"",
#ਜੇ_ਅੰਤ
				(char_u *)0L} SCTX_INIT},
    {"shellxquote", "sxq",  P_STRING|P_VI_DEF|P_SECURE,
			    (char_u *)&p_sxq, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {
#ਜੇ defined(UNIX) && defined(USE_SYSTEM)
			    (char_u *)"\"",
#ਨਹੀਂਤਾਂ
			    (char_u *)"",
#ਜੇ_ਅੰਤ
				(char_u *)0L} SCTX_INIT},
    {"shiftround",  "sr",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sr, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"shiftwidth",  "sw",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_sw, PV_SW,
			    did_ਸੈੱਟ_shiftwidth_tabstop, ਨਿਰਾ,
			    {(char_u *)8L, (char_u *)0L} SCTX_INIT},
    {"shortmess",   "shm",  P_STRING|P_VIM|P_FLAGLIST,
			    (char_u *)&p_shm, PV_NONE, did_ਸੈੱਟ_shortmess, expand_ਸੈੱਟ_shortmess,
			    {(char_u *)"S", (char_u *)"filnxtToOS"}
			    SCTX_INIT},
    {"shortname",   "sn",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sn, PV_SN, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"showbreak",   "sbr",  P_STRING|P_VI_DEF|P_RALL,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
			    (char_u *)&p_sbr, PV_SBR, did_ਸੈੱਟ_showbreak, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"showcmd",	    "sc",   P_BOOL|P_VIM,
			    (char_u *)&p_sc, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"showcmdloc",  "sloc", P_STRING|P_RSTAT,
			    (char_u *)&p_sloc, PV_NONE, did_ਸੈੱਟ_showcmdloc, expand_ਸੈੱਟ_showcmdloc,
			    {(char_u *)"last", (char_u *)"last"} SCTX_INIT},
    {"showfulltag", "sft",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_sft, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"showmatch",   "sm",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sm, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"showmode",    "smd",  P_BOOL|P_VIM,
			    (char_u *)&p_smd, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"showtabline", "stal", P_ਅੰਕ|P_VI_DEF|P_RALL,
			    (char_u *)&p_stal, PV_NONE, did_ਸੈੱਟ_showtabline, ਨਿਰਾ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
#ਜੇ defined(FEAT_TABPANEL)
    {"showtabpanel", "stpl", P_ਅੰਕ|P_RALL,
			    (char_u *)&p_stpl, PV_NONE, did_ਸੈੱਟ_showtabpanel, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
#ਜੇ_ਅੰਤ
    {"sidescroll",  "ss",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_ss, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"sidescrolloff", "siso", P_ਅੰਕ|P_VI_DEF|P_VIM|P_RBUF,
			    (char_u *)&p_siso, PV_SISO, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"signcolumn",   "scl",  P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
			    (char_u *)VAR_WIN, PV_SCL, did_ਸੈੱਟ_signcolumn, expand_ਸੈੱਟ_signcolumn,
			    {(char_u *)"auto", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"slowopen",    "slow", P_BOOL|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smartcase",   "scs",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_scs, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smartindent", "si",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_si, PV_SI, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smarttab",    "sta",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sta, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"smoothscroll", "sms", P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_SMS, did_ਸੈੱਟ_smoothscroll, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"softtabstop", "sts",  P_ਅੰਕ|P_VI_DEF|P_VIM,
			    (char_u *)&p_sts, PV_STS, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"sourceany",   ਨਿਰਾ,   P_BOOL|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"spell",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF|P_RWIN|P_HLONLY,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
			    (char_u *)VAR_WIN, PV_SPELL, did_ਸੈੱਟ_spell, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"spellcapcheck", "spc", P_STRING|P_ALLOCED|P_VI_DEF|P_RBUF|P_HLONLY,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
			    (char_u *)&p_spc, PV_SPC, did_ਸੈੱਟ_spellcapcheck, ਨਿਰਾ,
			    {(char_u *)"[.?!]\\_[\\])'\"	 ]\\+", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"spellfile",   "spf",  P_STRING|P_EXPAND|P_ALLOCED|P_VI_DEF|P_SECURE
								  |P_ONECOMMA,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
			    (char_u *)&p_spf, PV_SPF, did_ਸੈੱਟ_spellfile, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"spelllang",   "spl",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA
						     |P_RBUF|P_HLONLY|P_EXPAND,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
			    (char_u *)&p_spl, PV_SPL, did_ਸੈੱਟ_spelllang, ਨਿਰਾ,
			    {(char_u *)"en", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"spelloptions", "spo",  P_STRING|P_ALLOCED|P_VI_DEF
					   |P_ONECOMMA|P_NODUP|P_RBUF|P_HLONLY,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
			    (char_u *)&p_spo, PV_SPO, did_ਸੈੱਟ_spelloptions, expand_ਸੈੱਟ_spelloptions,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"spellsuggest", "sps", P_STRING|P_VI_DEF|P_EXPAND|P_SECURE|P_ONECOMMA,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
			    (char_u *)&p_sps, PV_NONE, did_ਸੈੱਟ_spellsuggest, expand_ਸੈੱਟ_spellsuggest,
			    {(char_u *)"best", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"splitbelow",  "sb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sb, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"splitkeep",   "spk",  P_STRING,
			    (char_u *)&p_spk, PV_NONE, did_ਸੈੱਟ_splitkeep, expand_ਸੈੱਟ_splitkeep,
			    {(char_u *)"cursor", (char_u *)"cursor"} SCTX_INIT},
    {"splitright",  "spr",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_spr, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"startofline", "sol",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sol, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"statusline"  ,"stl",  P_STRING|P_VI_DEF|P_ALLOCED|P_RSTAT|P_MLE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
			    (char_u *)&p_stl, PV_STL, did_ਸੈੱਟ_statusline, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"statuslineopt"  ,"stlo",  P_STRING|P_VI_DEF|P_ALLOCED|P_RSTAT|P_MLE
			    |P_ONECOMMA|P_COLON|P_NODUP,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
			    (char_u *)&p_stlo, PV_STLO, did_ਸੈੱਟ_statuslineopt, expand_ਸੈੱਟ_statuslineopt,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"suffixes",    "su",   P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_su, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)".bak,~,.o,.h,.info,.swp,.obj",
				(char_u *)0L} SCTX_INIT},
    {"suffixesadd", "sua",  P_STRING|P_VI_DEF|P_ALLOCED|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_sua, PV_SUA, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"swapfile",    "swf",  P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)&p_swf, PV_SWF, did_ਸੈੱਟ_swapfile, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"swapsync",    "sws",  P_STRING|P_VI_DEF,
			    (char_u *)&p_sws, PV_NONE, did_ਸੈੱਟ_swapsync, expand_ਸੈੱਟ_swapsync,
			    {(char_u *)"fsync", (char_u *)0L} SCTX_INIT},
    {"switchbuf",   "swb",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_swb, PV_NONE, did_ਸੈੱਟ_switchbuf, expand_ਸੈੱਟ_switchbuf,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"synmaxcol",   "smc",  P_ਅੰਕ|P_VI_DEF|P_RBUF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
			    (char_u *)&p_smc, PV_SMC, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)3000L, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"syntax",	    "syn",  P_STRING|P_ALLOCED|P_VI_DEF|P_NOGLOB|P_NFNAME,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
			    (char_u *)&p_syn, PV_SYN,
			    did_ਸੈੱਟ_filetype_or_syntax, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"tabclose",    "tcl",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_tcl, PV_NONE, did_ਸੈੱਟ_tabclose, expand_ਸੈੱਟ_tabclose,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"tabline",	    "tal",  P_STRING|P_VI_DEF|P_RALL|P_MLE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
			    (char_u *)&p_tal, PV_NONE, did_ਸੈੱਟ_tabline, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"tabpagemax",  "tpm",  P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_tpm, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)10L, (char_u *)0L} SCTX_INIT},
#ਜੇ defined(FEAT_TABPANEL)
    {"tabpanel",  "tpl",    P_STRING|P_VI_DEF|P_RALL|P_MLE,
			    (char_u *)&p_tpl, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"tabpanelopt","tplo",  P_STRING|P_ALLOCED|P_VI_DEF|P_ONECOMMA|P_COLON
								    |P_NODUP,
			    (char_u *)&p_tplo, PV_NONE, did_ਸੈੱਟ_tabpanelopt,
			    expand_ਸੈੱਟ_tabpanelopt,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
#ਜੇ_ਅੰਤ
    {"tabstop",	    "ts",   P_ਅੰਕ|P_VI_DEF|P_RBUF,
			    (char_u *)&p_ts, PV_TS,
			    did_ਸੈੱਟ_shiftwidth_tabstop, ਨਿਰਾ,
			    {(char_u *)8L, (char_u *)0L} SCTX_INIT},
    {"tagbsearch",  "tbs",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_tbs, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS	// binary searching doesn't appear to work on VMS
			    {(char_u *)0L, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    {(char_u *)TRUE, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"tagcase",	    "tc",   P_STRING|P_VIM,
			    (char_u *)&p_tc, PV_TC, did_ਸੈੱਟ_tagcase, expand_ਸੈੱਟ_tagcase,
			    {(char_u *)"followic", (char_u *)"followic"} SCTX_INIT},
    {"tagfunc",    "tfu",   P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
			    (char_u *)&p_tfu, PV_TFU, did_ਸੈੱਟ_tagfunc, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"taglength",   "tl",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_tl, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"tagrelative", "tr",   P_BOOL|P_VIM,
			    (char_u *)&p_tr, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)TRUE} SCTX_INIT},
    {"tags",	    "tag",  P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_ਟੈਗ_ਸੂਚੀ, PV_ਟੈਗ_ਸੂਚੀ, ਨਿਰਾ, ਨਿਰਾ,
			    {
#ਜੇ defined(FEAT_EMACS_ਟੈਗ_ਸੂਚੀ) && !defined(CASE_INSENSITIVE_FILENAME)
			    (char_u *)"./tags,./TAGS,tags,TAGS",
#ਨਹੀਂਤਾਂ
			    (char_u *)"./tags,tags",
#ਜੇ_ਅੰਤ
				(char_u *)0L} SCTX_INIT},
    {"tagsecure",   "tsc",   P_BOOL|P_VI_DEF|P_SECURE,
			    (char_u *)&p_tagsecure, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)TRUE} SCTX_INIT},
    {"tagstack",    "tgst", P_BOOL|P_VI_DEF,
			    (char_u *)&p_tgst, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"tcldll",      ਨਿਰਾ,   P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ defined(DYNAMIC_TCL)
			    (char_u *)&p_tcldll, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DYNAMIC_TCL_DLL, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"term",	    ਨਿਰਾ,   P_STRING|P_EXPAND|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RALL,
			    (char_u *)&T_NAME, PV_NONE, did_ਸੈੱਟ_ਪਦ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"termbidi", "tbidi",   P_BOOL|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
			    (char_u *)&p_tbidi, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"termencoding", "tenc", P_STRING|P_VI_DEF|P_RCLR,
			    (char_u *)&p_tenc, PV_NONE, did_ਸੈੱਟ_encoding, expand_ਸੈੱਟ_encoding,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"termguicolors", "tgc", P_BOOL|P_VI_DEF|P_VIM|P_RCLR,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
			    (char_u *)&p_tgc, PV_NONE, did_ਸੈੱਟ_termguicolors, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)FALSE}
#ਨਹੀਂਤਾਂ
			    (char_u*)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)FALSE}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"termresize", "trz", P_STRING|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
			    (char_u *)&p_trz, PV_NONE, did_ਸੈੱਟ_termresize, expand_ਸੈੱਟ_termresize,
			    {(char_u *)"", (char_u *)0}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"termsync", "tsy",	    P_BOOL|P_VI_DEF,
			    (char_u *)&p_tsy, PV_NONE, did_ਸੈੱਟ_termsync, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"termwinkey", "twk",   P_STRING|P_ALLOCED|P_RWIN|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
			    (char_u *)VAR_WIN, PV_TWK, did_ਸੈੱਟ_termwinkey, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)ਨਿਰਾ}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"termwinscroll", "twsl", P_ਅੰਕ|P_VI_DEF|P_VIM|P_RBUF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
			    (char_u *)&p_twsl, PV_TWSL, did_ਸੈੱਟ_termwinscroll, ਨਿਰਾ,
			    {(char_u *)10000L, (char_u *)10000L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"termwinsize", "tws",  P_STRING|P_ALLOCED|P_RWIN|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
			    (char_u *)VAR_WIN, PV_TWS, did_ਸੈੱਟ_termwinsize, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)ਨਿਰਾ}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"termwintype", "twt",  P_STRING|P_ALLOCED|P_VI_DEF,
#ਜੇ defined(MSWIN) && defined(FEAT_TERMINAL)
			    (char_u *)&p_twt, PV_NONE, did_ਸੈੱਟ_termwintype, expand_ਸੈੱਟ_termwintype,
			    {(char_u *)"", (char_u *)ਨਿਰਾ}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"terse",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_terse, PV_NONE, did_ਸੈੱਟ_terse, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"textauto",    "ta",   P_BOOL|P_VIM,
			    (char_u *)&p_ta, PV_NONE, did_ਸੈੱਟ_textauto, ਨਿਰਾ,
			    {(char_u *)DFLT_TEXTAUTO, (char_u *)TRUE}
			    SCTX_INIT},
    {"textmode",    "tx",   P_BOOL|P_VI_DEF|P_NO_MKRC,
			    (char_u *)&p_tx, PV_TX, did_ਸੈੱਟ_textmode, ਨਿਰਾ,
			    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CRNL
			    (char_u *)TRUE,
#ਨਹੀਂਤਾਂ
			    (char_u *)FALSE,
#ਜੇ_ਅੰਤ
				(char_u *)0L} SCTX_INIT},
    {"textwidth",   "tw",   P_ਅੰਕ|P_VI_DEF|P_VIM|P_RBUF|P_HLONLY,
			    (char_u *)&p_tw, PV_TW, did_ਸੈੱਟ_textwidth, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"thesaurus",   "tsr",  P_STRING|P_EXPAND|P_VI_DEF|P_ONECOMMA|P_NODUP|P_NDNAME,
			    (char_u *)&p_tsr, PV_TSR, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"thesaurusfunc", "tsrfu",  P_STRING|P_ALLOCED|P_VI_DEF|P_SECURE|P_FUNC,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
			    (char_u *)&p_tsrfu, PV_TSRFU,
			    did_ਸੈੱਟ_thesaurusfunc, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"tildeop",	    "top",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_ਮੰਜ਼ਿਲ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"timeout",	    "to",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_timeout, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"timeoutlen",  "tm",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_tm, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)1000L, (char_u *)0L} SCTX_INIT},
    {"title",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_title, PV_NONE, did_ਸੈੱਟ_title_icon, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"titlelen",    ਨਿਰਾ,   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_titlelen, PV_NONE, did_ਸੈੱਟ_titlelen, ਨਿਰਾ,
			    {(char_u *)85L, (char_u *)0L} SCTX_INIT},
    {"titleold",    ਨਿਰਾ,   P_STRING|P_VI_DEF|P_GETTEXT|P_SECURE|P_NO_MKRC,
			    (char_u *)&p_titleold, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)N_("Thanks for flying Vim"),
							       (char_u *)0L}
			    SCTX_INIT},
    {"titlestring", ਨਿਰਾ,   P_STRING|P_VI_DEF|P_MLE,
			    (char_u *)&p_titlestring, PV_NONE,
			    did_ਸੈੱਟ_titlestring, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"toolbar",     "tb",   P_STRING|P_ONECOMMA|P_VI_DEF|P_NODUP,
#ਜੇ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
			    (char_u *)&p_toolbar, PV_NONE, did_ਸੈੱਟ_toolbar, expand_ਸੈੱਟ_toolbar,
			    {(char_u *)"icons,tooltips", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"toolbariconsize",	"tbis", P_STRING|P_VI_DEF,
#ਜੇ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
			    (char_u *)&p_tbis, PV_NONE,
			    did_ਸੈੱਟ_toolbariconsize, expand_ਸੈੱਟ_toolbariconsize,
			    {(char_u *)"small", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"ttimeout",    ਨਿਰਾ,   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_ttimeout, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"ttimeoutlen", "ttm",  P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_ttm, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)-1L, (char_u *)0L} SCTX_INIT},
    {"ttybuiltin",  "tbi",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_tbi, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"ttyfast",	    "tf",   P_BOOL|P_NO_MKRC|P_VI_DEF,
			    (char_u *)&p_tf, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"ttymouse",    "ttym", P_STRING|P_NODEFAULT|P_NO_MKRC|P_VI_DEF,
#ਜੇ defined(UNIX) || defined(VMS)
			    (char_u *)&p_ttym, PV_NONE, did_ਸੈੱਟ_ttymouse, expand_ਸੈੱਟ_ttymouse,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"ttyscroll",   "tsl",  P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_ttyscroll, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)999L, (char_u *)0L} SCTX_INIT},
    {"ttytype",	    "tty",  P_STRING|P_EXPAND|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RALL,
			    (char_u *)&T_NAME, PV_NONE, did_ਸੈੱਟ_ਪਦ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"undodir",     "udir", P_STRING|P_EXPAND|P_ONECOMMA|P_NODUP|P_SECURE
								    |P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
			    (char_u *)&p_udir, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)".", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"undofile",    "udf",  P_BOOL|P_VI_DEF|P_VIM,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
			    (char_u *)&p_udf, PV_UDF, did_ਸੈੱਟ_undofile, ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
#ਜੇ_ਅੰਤ
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"undolevels",  "ul",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_ul, PV_UL, did_ਸੈੱਟ_undolevels, ਨਿਰਾ,
			    {
#ਜੇ defined(UNIX) || defined(MSWIN) || defined(VMS)
			    (char_u *)1000L,
#ਨਹੀਂਤਾਂ
			    (char_u *)100L,
#ਜੇ_ਅੰਤ
				(char_u *)0L} SCTX_INIT},
    {"undoreload",  "ur",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_ur, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    { (char_u *)10000L, (char_u *)0L} SCTX_INIT},
    {"updatecount", "uc",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_uc, PV_NONE, did_ਸੈੱਟ_updatecount, ਨਿਰਾ,
			    {(char_u *)200L, (char_u *)0L} SCTX_INIT},
    {"updatetime",  "ut",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_ut, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)4000L, (char_u *)0L} SCTX_INIT},
    {"varsofttabstop", "vsts",  P_STRING|P_VI_DEF|P_VIM|P_COMMA,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
			    (char_u *)&p_vsts, PV_VSTS,
			    did_ਸੈੱਟ_varsofttabstop, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)ਨਿਰਾ}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"vartabstop",  "vts",  P_STRING|P_VI_DEF|P_VIM|P_RBUF|P_COMMA,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
			    (char_u *)&p_vts, PV_VTS, did_ਸੈੱਟ_vartabstop, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)ਨਿਰਾ}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"verbose",	    "vbs",  P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_verbose, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"verbosefile", "vfile", P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
			    (char_u *)&p_vfile, PV_NONE, did_ਸੈੱਟ_verbosefile, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"viewdir",     "vdir", P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
			    (char_u *)&p_vdir, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)DFLT_VDIR, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"viewoptions", "vop",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
			    (char_u *)&p_vop, PV_NONE, did_ਸੈੱਟ_viewoptions, expand_ਸੈੱਟ_sessionoptions,
			    {(char_u *)"folds,options,cursor,curdir",
								  (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"viminfo",	    "vi",   P_STRING|P_ONECOMMA|P_NODUP|P_SECURE,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
			    (char_u *)&p_viminfo, PV_NONE, did_ਸੈੱਟ_viminfo, ਨਿਰਾ,
# ਜੇ defined(MSWIN)
			    {(char_u *)"", (char_u *)"'100,<50,s10,h,rA:,rB:"}
# elif defined(AMIGA)
			    {(char_u *)"",
				 (char_u *)"'100,<50,s10,h,rdf0:,rdf1:,rdf2:"}
# ਨਹੀਂਤਾਂ
			    {(char_u *)"", (char_u *)"'100,<50,s10,h"}
# endif
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"viminfofile", "vif",  P_STRING|P_EXPAND|P_ONECOMMA|P_NODUP
							    |P_SECURE|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
			    (char_u *)&p_viminfofile, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"virtualedit", "ve",   P_STRING|P_ONECOMMA|P_NODUP|P_VI_DEF
							    |P_VIM|P_CURSWANT,
			    (char_u *)&p_ve, PV_VE, did_ਸੈੱਟ_virtualedit, expand_ਸੈੱਟ_virtualedit,
			    {(char_u *)"", (char_u *)""}
			    SCTX_INIT},
    {"visualbell",  "vb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_vb, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"w1200",	    ਨਿਰਾ,   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"w300",	    ਨਿਰਾ,   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"w9600",	    ਨਿਰਾ,   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"warn",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_warn, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"weirdinvert", "wiv",  P_BOOL|P_VI_DEF|P_RCLR,
			    (char_u *)&p_wiv, PV_NONE, did_ਸੈੱਟ_weirdinvert, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"whichwrap",   "ww",   P_STRING|P_VIM|P_ONECOMMA|P_FLAGLIST,
			    (char_u *)&p_ww, PV_NONE, did_ਸੈੱਟ_whichwrap, expand_ਸੈੱਟ_whichwrap,
			    {(char_u *)"", (char_u *)"b,s"} SCTX_INIT},
    {"wildchar",    "wc",   P_ਅੰਕ|P_VIM,
			    (char_u *)&p_wc, PV_NONE, did_ਸੈੱਟ_wildchar, ਨਿਰਾ,
			    {(char_u *)(ਲੰਮਾ)Ctrl_E, (char_u *)(ਲੰਮਾ)TAB}
			    SCTX_INIT},
    {"wildcharm",   "wcm",  P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_wcm, PV_NONE, did_ਸੈੱਟ_wildchar, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"wildignore",  "wig",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_wig, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},
    {"wildignorecase", "wic", P_BOOL|P_VI_DEF,
			    (char_u *)&p_wic, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"wildmenu",    "wmnu", P_BOOL|P_VI_DEF,
			    (char_u *)&p_wmnu, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"wildmode",    "wim",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)&p_wim, PV_NONE, did_ਸੈੱਟ_wildmode, expand_ਸੈੱਟ_wildmode,
			    {(char_u *)"full", (char_u *)0L} SCTX_INIT},
    {"wildoptions", "wop",  P_STRING|P_VI_DEF|P_ONECOMMA|P_NODUP,
			    (char_u *)&p_wop, PV_NONE, did_ਸੈੱਟ_wildoptions, expand_ਸੈੱਟ_wildoptions,
			    {(char_u *)"", (char_u *)0L}
			    SCTX_INIT},
    {"winaltkeys",  "wak",  P_STRING|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAK
			    (char_u *)&p_wak, PV_NONE, did_ਸੈੱਟ_winaltkeys, expand_ਸੈੱਟ_winaltkeys,
			    {(char_u *)"menu", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"wincolor", "wcr",	    P_STRING|P_ALLOCED|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_WCR, did_ਸੈੱਟ_wincolor, expand_ਸੈੱਟ_wincolor,
			    {(char_u *)"", (char_u *)ਨਿਰਾ}
			    SCTX_INIT},
    {"window",	    "wi",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_ਖਿੜਕੀ, PV_NONE, did_ਸੈੱਟ_ਖਿੜਕੀ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"winfixbuf", "wfb",    P_BOOL|P_VI_DEF,
			    (char_u *)VAR_WIN, PV_WFB, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"winfixheight", "wfh", P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)VAR_WIN, PV_WFH, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"winfixwidth", "wfw",  P_BOOL|P_VI_DEF|P_RSTAT,
			    (char_u *)VAR_WIN, PV_WFW, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"winheight",   "wh",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_wh, PV_NONE,
			    did_ਸੈੱਟ_winheight_helpheight, ਨਿਰਾ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"winhighlight", "whl", P_STRING|P_VI_DEF|P_RALL|P_ONECOMMA|P_NODUP|P_COLON,
			    (char_u *)VAR_WIN, PV_WHL, did_ਸੈੱਟ_winhighlight, expand_ਸੈੱਟ_winhighlight,
			    {(char_u *)"", (char_u *)ਨਿਰਾ} SCTX_INIT},
    {"winminheight", "wmh", P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_wmh, PV_NONE, did_ਸੈੱਟ_winminheight, ਨਿਰਾ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"winminwidth", "wmw",  P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_wmw, PV_NONE, did_ਸੈੱਟ_winminwidth, ਨਿਰਾ,
			    {(char_u *)1L, (char_u *)0L} SCTX_INIT},
    {"winptydll", ਨਿਰਾ,	    P_STRING|P_EXPAND|P_VI_DEF|P_SECURE,
#ਜੇ defined(MSWIN) && defined(FEAT_TERMINAL)
			    (char_u *)&p_winptydll, PV_NONE, ਨਿਰਾ, ਨਿਰਾ, {
# ifdef _WIN64
			    (char_u *)"winpty64.dll",
# ਨਹੀਂਤਾਂ
			    (char_u *)"winpty32.dll",
# endif
				(char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"winwidth",   "wiw",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_wiw, PV_NONE, did_ਸੈੱਟ_winwidth, ਨਿਰਾ,
			    {(char_u *)20L, (char_u *)0L} SCTX_INIT},
    {"wlseat",	    "wse",  P_STRING|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND
			    (char_u *)&p_wse, PV_NONE, did_ਸੈੱਟ_wlseat, ਨਿਰਾ,
			    {(char_u *)"", (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"wlsteal",	    "wst",  P_BOOL|P_VI_DEF, // Deprecated
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
			    SCTX_INIT},
    {"wltimeoutlen", "wtm", P_ਅੰਕ|P_VI_DEF,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND
			    (char_u *)&p_wtm, PV_NONE, did_ਸੈੱਟ_wltimeoutlen, ਨਿਰਾ,
			    {(char_u *)500L, (char_u *)0L}
#ਨਹੀਂਤਾਂ
			    (char_u *)ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)ਨਿਰਾ, (char_u *)0L}
#ਜੇ_ਅੰਤ
			    SCTX_INIT},
    {"wrap",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF|P_RWIN,
			    (char_u *)VAR_WIN, PV_WRAP, did_ਸੈੱਟ_wrap, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"wrapmargin",  "wm",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_wm, PV_WM, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"wrapscan",    "ws",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ws, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"write",	    ਨਿਰਾ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_write, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},
    {"writeany",    "wa",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_wa, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)FALSE, (char_u *)0L} SCTX_INIT},
    {"writebackup", "wb",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_wb, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WRITEBACKUP
			    (char_u *)TRUE,
#ਨਹੀਂਤਾਂ
			    (char_u *)FALSE,
#ਜੇ_ਅੰਤ
				(char_u *)0L} SCTX_INIT},
    {"writedelay",  "wd",   P_ਅੰਕ|P_VI_DEF,
			    (char_u *)&p_wd, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)0L, (char_u *)0L} SCTX_INIT},
    {"xtermcodes",  ਨਿਰਾ,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_xtermcodes, PV_NONE, ਨਿਰਾ, ਨਿਰਾ,
			    {(char_u *)TRUE, (char_u *)0L} SCTX_INIT},

// terminal output codes
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ p_ਪਦ(sss, vvv) \
			    {sss, ਨਿਰਾ, P_STRING|P_VI_DEF|P_RALL|P_SECURE, \
			    (char_u *)&vvv, PV_NONE, did_ਸੈੱਟ_ਪਦ_option, ਨਿਰਾ, \
			    {(char_u *)"", (char_u *)0L} SCTX_INIT},

    p_ਪਦ("t_AB", T_CAB)
    p_ਪਦ("t_AF", T_CAF)
    p_ਪਦ("t_AU", T_CAU)
    p_ਪਦ("t_AL", T_CAL)
    p_ਪਦ("t_al", T_AL)
    p_ਪਦ("t_bc", T_BC)
    p_ਪਦ("t_BE", T_BE)
    p_ਪਦ("t_BD", T_BD)
    p_ਪਦ("t_cd", T_CD)
    p_ਪਦ("t_ce", T_CE)
    p_ਪਦ("t_Ce", T_UCE)
    p_ਪਦ("t_CF", T_CFO)
    p_ਪਦ("t_cl", T_CL)
    p_ਪਦ("t_cm", T_CM)
    p_ਪਦ("t_Co", T_CCO)
    p_ਪਦ("t_CS", T_CCS)
    p_ਪਦ("t_Cs", T_UCS)
    p_ਪਦ("t_cs", T_CS)
    p_ਪਦ("t_CV", T_CSV)
    p_ਪਦ("t_da", T_DA)
    p_ਪਦ("t_db", T_DB)
    p_ਪਦ("t_DL", T_CDL)
    p_ਪਦ("t_dl", T_DL)
    p_ਪਦ("t_ds", T_DS)
    p_ਪਦ("t_Ds", T_CDS)
    p_ਪਦ("t_EC", T_CEC)
    p_ਪਦ("t_EI", T_CEI)
    p_ਪਦ("t_fs", T_FS)
    p_ਪਦ("t_fd", T_FD)
    p_ਪਦ("t_fe", T_FE)
    p_ਪਦ("t_GP", T_CGP)
    p_ਪਦ("t_IE", T_CIE)
    p_ਪਦ("t_IS", T_CIS)
    p_ਪਦ("t_ke", T_KE)
    p_ਪਦ("t_ks", T_KS)
    p_ਪਦ("t_le", T_LE)
    p_ਪਦ("t_mb", T_MB)
    p_ਪਦ("t_md", T_MD)
    p_ਪਦ("t_me", T_ME)
    p_ਪਦ("t_mr", T_MR)
    p_ਪਦ("t_ms", T_MS)
    p_ਪਦ("t_nd", T_ND)
    p_ਪਦ("t_op", T_OP)
    p_ਪਦ("t_RF", T_RFG)
    p_ਪਦ("t_RB", T_RBG)
    p_ਪਦ("t_RC", T_ਚੱਕਰੀ_ਜਾਂਚ)
    p_ਪਦ("t_RI", T_CRI)
    p_ਪਦ("t_Ri", T_SRI)
    p_ਪਦ("t_RK", T_CRK)
    p_ਪਦ("t_RS", T_CRS)
    p_ਪਦ("t_RT", T_CRT)
    p_ਪਦ("t_RV", T_CRV)
    p_ਪਦ("t_Sb", T_CSB)
    p_ਪਦ("t_SC", T_CSC)
    p_ਪਦ("t_se", T_SE)
    p_ਪਦ("t_Sf", T_CSF)
    p_ਪਦ("t_SH", T_CSH)
    p_ਪਦ("t_SI", T_CSI)
    p_ਪਦ("t_Si", T_SSI)
    p_ਪਦ("t_so", T_SO)
    p_ਪਦ("t_SR", T_CSR)
    p_ਪਦ("t_sr", T_SR)
    p_ਪਦ("t_ST", T_CST)
    p_ਪਦ("t_Te", T_STE)
    p_ਪਦ("t_te", T_TE)
    p_ਪਦ("t_TE", T_CTE)
    p_ਪਦ("t_ti", T_TI)
    p_ਪਦ("t_TI", T_CTI)
    p_ਪਦ("t_Ts", T_STS)
    p_ਪਦ("t_ts", T_TS)
    p_ਪਦ("t_u7", T_U7)
    p_ਪਦ("t_ue", T_UE)
    p_ਪਦ("t_us", T_US)
    p_ਪਦ("t_Us", T_USS)
    p_ਪਦ("t_ut", T_UT)
    p_ਪਦ("t_vb", T_VB)
    p_ਪਦ("t_ve", T_VE)
    p_ਪਦ("t_vi", T_VI)
    p_ਪਦ("t_VS", T_CVS)
    p_ਪਦ("t_vs", T_VS)
    p_ਪਦ("t_WP", T_CWP)
    p_ਪਦ("t_WS", T_CWS)
    p_ਪਦ("t_XM", T_CXM)
    p_ਪਦ("t_xn", T_XN)
    p_ਪਦ("t_xs", T_XS)
    p_ਪਦ("t_ZH", T_CZH)
    p_ਪਦ("t_ZR", T_CZR)
    p_ਪਦ("t_8f", T_8F)
    p_ਪਦ("t_8b", T_8B)
    p_ਪਦ("t_8u", T_8U)
    p_ਪਦ("t_xo", T_XON)
    p_ਪਦ("t_BS", T_BSU)
    p_ਪਦ("t_ES", T_ESU)

// terminal key codes are not in here

    // end marker
    {ਨਿਰਾ, ਨਿਰਾ, 0, ਨਿਰਾ, PV_NONE, ਨਿਰਾ, ਨਿਰਾ, {ਨਿਰਾ, ਨਿਰਾ} SCTX_INIT}
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPTION_ਗਿਣਤੀ ARRAY_ਲੰਮਾਈ(options)

// The following is needed to make the gen_opt_test.vim script work.
// {"
