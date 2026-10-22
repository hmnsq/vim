#جيڪڏهن_وصف_نه_ٿيل __VTERM_INTERNAL_H__
#وصف_ڪريو __VTERM_INTERNAL_H__

#ضمن "vterm.h"

#ضمن <stdarg.h>

#إذا defined(__GNUC__) && !defined(__MINGW32__)
# define اندروني __attribute__((visibility("internal")))
#وإلا
# define اندروني
#جيڪڏهن_پورو

#إذا defined(__GNUC__) || defined(__MINGW32__)
# define اڻ_استعمال_ٿيل __attribute__((اڻ_استعمال_ٿيل_2))
#وإلا
# define اڻ_استعمال_ٿيل
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#وإلا
# define DEBUG_LOG(s)
# define DEBUG_LOG1(s, a)
# define DEBUG_LOG2(s, a, b)
# define DEBUG_LOG3(s, a, b, c)
#جيڪڏهن_پورو

#وصف_ڪريو ESC_S "\x1b"

#وصف_ڪريو INTERMED_MAX 16

#وصف_ڪريو CSI_دليلون_MAX 16
#وصف_ڪريو CSI_LEADER_MAX 16

#وصف_ڪريو BUFIDX_PRIMARY   0
#وصف_ڪريو BUFIDX_ALTSCREEN 1

تعريفنوع بنية VTermEncoding VTermEncoding;

تعريفنوع بنية {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  حرف           معلومات[4*حجم(uint32_t)];
} VTermEncodingInstance;

بنية VTermPen
{
  VTermColor fg;
  VTermColor bg;
  بنانشان صحيح bold:1;
  بنانشان صحيح underline:2;
  بنانشان صحيح italic:1;
  بنانشان صحيح blink:1;
  بنانشان صحيح ابتو:1;
  بنانشان صحيح conceal:1;
  بنانشان صحيح strike:1;
  بنانشان صحيح font:4; /* To store 0-9 */
  بنانشان صحيح small:1;
  بنانشان صحيح baseline:2;
};

بنية VTermState
{
  VTerm *vt;

  ثابت VTermStateCallbacks *callbacks;
  فراغ *cbdata;
  صحيح callbacks_has_premove;

  ثابت VTermStateFallbacks *fallbacks;
  فراغ *fbdata;

  صحيح صفون_2;
  صحيح کانن_ڳڻپ;

  /* Current cursor position */
  VTermPos pos;

  صحيح at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  صحيح scrollregion_چوٽي;
  صحيح scrollregion_bottom; /* -1 means unbounded */
#وصف_ڪريو SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->صفون_2)
  صحيح scrollregion_کاٻو;
#وصف_ڪريو SCROLLREGION_کاٻو(state)  ((state)->گهڻائي.leftrightmargin ? (state)->scrollregion_کاٻو : 0)
  صحيح scrollregion_ساڄو; /* -1 means unbounded */
#وصف_ڪريو SCROLLREGION_ساڄو(state) ((state)->گهڻائي.leftrightmargin && (state)->scrollregion_ساڄو > -1 ? (state)->scrollregion_ساڄو : (state)->کانن_ڳڻپ)

  /* Bitvector of tab stops */
  بنانشان حرف *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_سٽ_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_سٽ_Info *lineinfo;
#وصف_ڪريو ROWWIDTH(state,صف) ((state)->lineinfo[(صف)].doublewidth ? ((state)->کانن_ڳڻپ / 2) : (state)->کانن_ڳڻپ)
#وصف_ڪريو THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.صف)

  /* Mouse state */
  صحيح mouse_کانو, mouse_صف;
  صحيح mouse_buttons;
  صحيح mouse_flags;

  تعداد { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_سائيز; // Number of ELEMENTS in the above
  صحيح combine_ويڪر; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  بنية {
    بنانشان صحيح keypad:1;
    بنانشان صحيح cursor:1;
    بنانشان صحيح autowrap:1;
    بنانشان صحيح وجهو:1;
    بنانشان صحيح newline:1;
    بنانشان صحيح cursor_visible:1;
    بنانشان صحيح cursor_blink:1;
    بنانشان صحيح cursor_shape:2;
    بنانشان صحيح alt_screen:1;
    بنانشان صحيح origin:1;
    بنانشان صحيح screen:1;
    بنانشان صحيح leftrightmargin:1;
    بنانشان صحيح bracketpaste:1;
    بنانشان صحيح report_focus:1;
    بنانشان صحيح modify_other_ڪنجيون:1;
    بنانشان صحيح kitty_keyboard:1;
  } گهڻائي;

  VTermEncodingInstance encoding[4], encoding_utf8;
  صحيح gl_سيٽ, gr_سيٽ, gsingle_سيٽ;

  بنية VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  صحيح bold_is_highbright;

  بنانشان صحيح protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  بنية {
    VTermPos pos;
    بنية VTermPen pen;

    بنية {
      بنانشان صحيح cursor_visible:1;
      بنانشان صحيح cursor_blink:1;
      بنانشان صحيح cursor_shape:2;
    } گهڻائي;
  } saved;

  /* Temporary state for DECRQSS parsing */
  اتحاد {
    حرف decrqss[4];
    بنية {
      uint16_t نقاب;
      تعداد {
        SELECTION_INITIAL,
        SELECTION_SELECTED,
        SELECTION_QUERY,
        SELECTION_سيٽ_INITIAL,
        SELECTION_سيٽ,
        SELECTION_INVALID,
      } state : 8;
      uint32_t recvpartial;
      uint32_t sendpartial;
    } selection;
  } tmp;

  بنية {
    ثابت VTermSelectionCallbacks *callbacks;
    فراغ *user;
    حرف *بفر;
    size_t buflen;
  } selection;
};

بنية VTerm
{
  ثابت VTermAllocatorFunctions *allocator;
  فراغ *allocdata;

  صحيح صفون_2;
  صحيح کانن_ڳڻپ;

  بنية {
    بنانشان صحيح utf8:1;
    بنانشان صحيح ctrl8bit:1;
  } گهڻائي;

  بنية {
    تعداد VTermParserState {
      عام_3,
      CSI_LEADER,
      CSI_دليلون,
      CSI_INTERMED,
      DCS_COMMAND,
      /* below here are the "string states" */
      OSC_COMMAND,
      OSC,
      DCS,
      APC,
      PM,
      SOS,
    } state;

    بنانشان صحيح اندريون_esc : 1;

    صحيح intermedlen;
    حرف intermed[INTERMED_MAX];

    اتحاد {
      بنية {
        صحيح leaderlen;
        حرف leader[CSI_LEADER_MAX];

        صحيح argi;
        ڊگهو دليلون[CSI_دليلون_MAX];
      } csi;
      بنية {
        صحيح command;
      } osc;
      بنية {
        صحيح commandlen;
        حرف command[CSI_LEADER_MAX];
      } dcs;
    } v;

    ثابت VTermParserCallbacks *callbacks;
    فراغ *cbdata;

    صحيح string_initial;

    صحيح emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  فراغ                *outdata;

  حرف  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  حرف  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  صحيح اندريون_backspace;
};

بنية VTermEncoding {
  فراغ (*init) (VTermEncoding *enc, فراغ *معلومات);
  فراغ (*decode)(VTermEncoding *enc, فراغ *معلومات,
                 uint32_t cp[], صحيح *cpi, صحيح cplen,
                 ثابت حرف بائيٽون[], size_t *pos, size_t len);
};

تعريفنوع تعداد {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

فراغ *vterm_allocator_malloc(VTerm *vt, size_t سائيز);
فراغ  vterm_allocator_free(VTerm *vt, فراغ *ptr);

فراغ vterm_ڌڪو_output_بائيٽون(VTerm *vt, ثابت حرف *بائيٽون, size_t len);
فراغ vterm_ڌڪو_output_vsprintf(VTerm *vt, ثابت حرف *format, va_list دليلون);
فراغ vterm_ڌڪو_output_sprintf(VTerm *vt, ثابت حرف *format, ...);
فراغ vterm_ڌڪو_output_sprintf_ctrl(VTerm *vt, بنانشان حرف ctrl, ثابت حرف *fmt, ...);
فراغ vterm_ڌڪو_output_sprintf_str(VTerm *vt, بنانشان حرف ctrl, صحيح رڪن, ثابت حرف *fmt, ...);

فراغ vterm_state_free(VTermState *state);

فراغ vterm_state_newpen(VTermState *state);
فراغ vterm_state_resetpen(VTermState *state);
فراغ vterm_state_setpen(VTermState *state, ثابت ڊگهو دليلون[], صحيح argcount);
صحيح  vterm_state_getpen(VTermState *state, ڊگهو دليلون[], صحيح argcount);
فراغ vterm_state_savepen(VTermState *state, صحيح save);

تعداد {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

فراغ vterm_state_ڌڪو_output_sprintf_CSI(VTermState *vts, ثابت حرف *format, ...);

فراغ vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, حرف designation);

صحيح vterm_unicode_ويڪر(uint32_t codepoint);
صحيح vterm_unicode_is_combining(uint32_t codepoint);
صحيح vterm_unicode_is_ambiguous(uint32_t codepoint);
صحيح vterm_get_special_pty_type(فراغ);

#إذا (defined(XOPEN_ذريعو) && XOPEN_ذريعو >= 500) \
	|| defined(ISOC99_ذريعو) || defined(BSD_ذريعو)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#وإلا
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
صحيح VSNPRINTF(حرف *str, size_t str_m, ثابت حرف *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
صحيح SNPRINTF(حرف *str, size_t str_m, ثابت حرف *fmt, ...);
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل SNPRINTF
# define SNPRINTF snprintf
#جيڪڏهن_پورو


#جيڪڏهن_پورو
