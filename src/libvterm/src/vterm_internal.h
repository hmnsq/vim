#جے_بنا_تعریف __VTERM_INTERNAL_H__
#تعریف_کرو __VTERM_INTERNAL_H__

#ضمن "vterm.h"

#ضمن <stdarg.h>

#إذا defined(__GNUC__) && !defined(__MINGW32__)
# define اندرونی __attribute__((visibility("internal")))
#وإلا
# define اندرونی
#جے_انت

#إذا defined(__GNUC__) || defined(__MINGW32__)
# define بن_ورتیا __attribute__((بن_ورتیا_2))
#وإلا
# define بن_ورتیا
#جے_انت

#جے_تعریف_شدہ DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#وإلا
# define DEBUG_LOG(s)
# define DEBUG_LOG1(s, a)
# define DEBUG_LOG2(s, a, b)
# define DEBUG_LOG3(s, a, b, c)
#جے_انت

#تعریف_کرو ESC_S "\x1b"

#تعریف_کرو INTERMED_MAX 16

#تعریف_کرو CSI_دلیلاں_MAX 16
#تعریف_کرو CSI_LEADER_MAX 16

#تعریف_کرو BUFIDX_PRIMARY   0
#تعریف_کرو BUFIDX_ALTSCREEN 1

تعريفنوع بنية VTermEncoding VTermEncoding;

تعريفنوع بنية {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  حرف           انکڑے[4*حجم(uint32_t)];
} VTermEncodingInstance;

بنية VTermPen
{
  VTermColor fg;
  VTermColor bg;
  بنانشانی صحيح bold:1;
  بنانشانی صحيح underline:2;
  بنانشانی صحيح italic:1;
  بنانشانی صحيح blink:1;
  بنانشانی صحيح الٹ:1;
  بنانشانی صحيح conceal:1;
  بنانشانی صحيح strike:1;
  بنانشانی صحيح font:4; /* To store 0-9 */
  بنانشانی صحيح small:1;
  بنانشانی صحيح baseline:2;
};

بنية VTermState
{
  VTerm *vt;

  ثابت VTermStateCallbacks *callbacks;
  فراغ *cbdata;
  صحيح callbacks_has_premove;

  ثابت VTermStateFallbacks *fallbacks;
  فراغ *fbdata;

  صحيح صفاں_2;
  صحيح خانے_گنتی;

  /* Current cursor position */
  VTermPos pos;

  صحيح at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  صحيح scrollregion_چوٹی;
  صحيح scrollregion_bottom; /* -1 means unbounded */
#تعریف_کرو SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->صفاں_2)
  صحيح scrollregion_بایاں;
#تعریف_کرو SCROLLREGION_بایاں(state)  ((state)->منوال.leftrightmargin ? (state)->scrollregion_بایاں : 0)
  صحيح scrollregion_دایاں; /* -1 means unbounded */
#تعریف_کرو SCROLLREGION_دایاں(state) ((state)->منوال.leftrightmargin && (state)->scrollregion_دایاں > -1 ? (state)->scrollregion_دایاں : (state)->خانے_گنتی)

  /* Bitvector of tab stops */
  بنانشانی حرف *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_سطر_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_سطر_Info *lineinfo;
#تعریف_کرو ROWWIDTH(state,صف) ((state)->lineinfo[(صف)].doublewidth ? ((state)->خانے_گنتی / 2) : (state)->خانے_گنتی)
#تعریف_کرو THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.صف)

  /* Mouse state */
  صحيح mouse_خانہ, mouse_صف;
  صحيح mouse_buttons;
  صحيح mouse_flags;

  تعداد { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_ناپ; // Number of ELEMENTS in the above
  صحيح combine_چوڑائی; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  بنية {
    بنانشانی صحيح keypad:1;
    بنانشانی صحيح cursor:1;
    بنانشانی صحيح autowrap:1;
    بنانشانی صحيح داخل_کرو:1;
    بنانشانی صحيح newline:1;
    بنانشانی صحيح cursor_visible:1;
    بنانشانی صحيح cursor_blink:1;
    بنانشانی صحيح cursor_shape:2;
    بنانشانی صحيح alt_screen:1;
    بنانشانی صحيح origin:1;
    بنانشانی صحيح screen:1;
    بنانشانی صحيح leftrightmargin:1;
    بنانشانی صحيح bracketpaste:1;
    بنانشانی صحيح report_focus:1;
    بنانشانی صحيح modify_other_کلیدیں:1;
    بنانشانی صحيح kitty_keyboard:1;
  } منوال;

  VTermEncodingInstance encoding[4], encoding_utf8;
  صحيح gl_سیٹ, gr_سیٹ, gsingle_سیٹ;

  بنية VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  صحيح bold_is_highbright;

  بنانشانی صحيح protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  بنية {
    VTermPos pos;
    بنية VTermPen pen;

    بنية {
      بنانشانی صحيح cursor_visible:1;
      بنانشانی صحيح cursor_blink:1;
      بنانشانی صحيح cursor_shape:2;
    } منوال;
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
        SELECTION_سیٹ_INITIAL,
        SELECTION_سیٹ,
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

  صحيح صفاں_2;
  صحيح خانے_گنتی;

  بنية {
    بنانشانی صحيح utf8:1;
    بنانشانی صحيح ctrl8bit:1;
  } منوال;

  بنية {
    تعداد VTermParserState {
      عام_3,
      CSI_LEADER,
      CSI_دلیلاں,
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

    بنانشانی صحيح وچ_esc : 1;

    صحيح intermedlen;
    حرف intermed[INTERMED_MAX];

    اتحاد {
      بنية {
        صحيح leaderlen;
        حرف leader[CSI_LEADER_MAX];

        صحيح argi;
        لما دلیلاں[CSI_دلیلاں_MAX];
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

  صحيح وچ_backspace;
};

بنية VTermEncoding {
  فراغ (*init) (VTermEncoding *enc, فراغ *انکڑے);
  فراغ (*decode)(VTermEncoding *enc, فراغ *انکڑے,
                 uint32_t cp[], صحيح *cpi, صحيح cplen,
                 ثابت حرف بائٹاں[], size_t *pos, size_t len);
};

تعريفنوع تعداد {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

فراغ *vterm_allocator_malloc(VTerm *vt, size_t ناپ);
فراغ  vterm_allocator_free(VTerm *vt, فراغ *ptr);

فراغ vterm_ڈالو_output_بائٹاں(VTerm *vt, ثابت حرف *بائٹاں, size_t len);
فراغ vterm_ڈالو_output_vsprintf(VTerm *vt, ثابت حرف *format, va_list دلیلاں);
فراغ vterm_ڈالو_output_sprintf(VTerm *vt, ثابت حرف *format, ...);
فراغ vterm_ڈالو_output_sprintf_ctrl(VTerm *vt, بنانشانی حرف ctrl, ثابت حرف *fmt, ...);
فراغ vterm_ڈالو_output_sprintf_str(VTerm *vt, بنانشانی حرف ctrl, صحيح رکن, ثابت حرف *fmt, ...);

فراغ vterm_state_free(VTermState *state);

فراغ vterm_state_newpen(VTermState *state);
فراغ vterm_state_resetpen(VTermState *state);
فراغ vterm_state_setpen(VTermState *state, ثابت لما دلیلاں[], صحيح argcount);
صحيح  vterm_state_getpen(VTermState *state, لما دلیلاں[], صحيح argcount);
فراغ vterm_state_savepen(VTermState *state, صحيح save);

تعداد {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

فراغ vterm_state_ڈالو_output_sprintf_CSI(VTermState *vts, ثابت حرف *format, ...);

فراغ vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, حرف designation);

صحيح vterm_unicode_چوڑائی(uint32_t codepoint);
صحيح vterm_unicode_is_combining(uint32_t codepoint);
صحيح vterm_unicode_is_ambiguous(uint32_t codepoint);
صحيح vterm_get_special_pty_type(فراغ);

#إذا (defined(XOPEN_سروت) && XOPEN_سروت >= 500) \
	|| defined(ISOC99_سروت) || defined(BSD_سروت)
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
#جے_انت
#جے_بنا_تعریف SNPRINTF
# define SNPRINTF snprintf
#جے_انت


#جے_انت
