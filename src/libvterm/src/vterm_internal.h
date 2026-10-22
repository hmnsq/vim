#اگر_غیر_تعریف_شدہ __VTERM_INTERNAL_H__
#تعریف_کرو __VTERM_INTERNAL_H__

#شامل "vterm.h"

#شامل <stdarg.h>

#اگر defined(__GNUC__) && !defined(__MINGW32__)
# define اندرونی_2 __attribute__((visibility("internal")))
#ورنہ
# define اندرونی_2
#اگر_ختم

#اگر defined(__GNUC__) || defined(__MINGW32__)
# define غیر_مستعمل __attribute__((غیر_مستعمل_2))
#ورنہ
# define غیر_مستعمل
#اگر_ختم

#اگر_تعریف_شدہ DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#ورنہ
# define DEBUG_LOG(s)
# define DEBUG_LOG1(s, a)
# define DEBUG_LOG2(s, a, b)
# define DEBUG_LOG3(s, a, b, c)
#اگر_ختم

#تعریف_کرو ESC_S "\x1b"

#تعریف_کرو INTERMED_MAX 16

#تعریف_کرو CSI_دلائل_MAX 16
#تعریف_کرو CSI_LEADER_MAX 16

#تعریف_کرو BUFIDX_PRIMARY   0
#تعریف_کرو BUFIDX_ALTSCREEN 1

قسمتعریف ساخت VTermEncoding VTermEncoding;

قسمتعریف ساخت {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  حرف           ڈیٹا[4*حجم(uint32_t)];
} VTermEncodingInstance;

ساخت VTermPen
{
  VTermColor fg;
  VTermColor bg;
  غیرعلامتی صحیح bold:1;
  غیرعلامتی صحیح underline:2;
  غیرعلامتی صحیح italic:1;
  غیرعلامتی صحیح blink:1;
  غیرعلامتی صحیح الٹ:1;
  غیرعلامتی صحیح conceal:1;
  غیرعلامتی صحیح strike:1;
  غیرعلامتی صحیح font:4; /* To store 0-9 */
  غیرعلامتی صحیح small:1;
  غیرعلامتی صحیح baseline:2;
};

ساخت VTermState
{
  VTerm *vt;

  ثابت VTermStateCallbacks *callbacks;
  خالی *cbdata;
  صحیح callbacks_has_premove;

  ثابت VTermStateFallbacks *fallbacks;
  خالی *fbdata;

  صحیح صفیں_2;
  صحیح ستون_تعداد;

  /* Current cursor position */
  VTermPos pos;

  صحیح at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  صحیح scrollregion_چوٹی;
  صحیح scrollregion_bottom; /* -1 means unbounded */
#تعریف_کرو SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->صفیں_2)
  صحیح scrollregion_بایاں;
#تعریف_کرو SCROLLREGION_بایاں(state)  ((state)->منوال.leftrightmargin ? (state)->scrollregion_بایاں : 0)
  صحیح scrollregion_دایاں; /* -1 means unbounded */
#تعریف_کرو SCROLLREGION_دایاں(state) ((state)->منوال.leftrightmargin && (state)->scrollregion_دایاں > -1 ? (state)->scrollregion_دایاں : (state)->ستون_تعداد)

  /* Bitvector of tab stops */
  غیرعلامتی حرف *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_سطر_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_سطر_Info *lineinfo;
#تعریف_کرو ROWWIDTH(state,صف) ((state)->lineinfo[(صف)].doublewidth ? ((state)->ستون_تعداد / 2) : (state)->ستون_تعداد)
#تعریف_کرو THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.صف)

  /* Mouse state */
  صحیح mouse_ستون, mouse_صف;
  صحیح mouse_buttons;
  صحیح mouse_flags;

  شمار { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_سائز; // Number of ELEMENTS in the above
  صحیح combine_چوڑائی; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  ساخت {
    غیرعلامتی صحیح keypad:1;
    غیرعلامتی صحیح cursor:1;
    غیرعلامتی صحیح autowrap:1;
    غیرعلامتی صحیح داخل_کرو:1;
    غیرعلامتی صحیح newline:1;
    غیرعلامتی صحیح cursor_visible:1;
    غیرعلامتی صحیح cursor_blink:1;
    غیرعلامتی صحیح cursor_shape:2;
    غیرعلامتی صحیح alt_screen:1;
    غیرعلامتی صحیح origin:1;
    غیرعلامتی صحیح screen:1;
    غیرعلامتی صحیح leftrightmargin:1;
    غیرعلامتی صحیح bracketpaste:1;
    غیرعلامتی صحیح report_focus:1;
    غیرعلامتی صحیح modify_other_کلیدیں:1;
    غیرعلامتی صحیح kitty_keyboard:1;
  } منوال;

  VTermEncodingInstance encoding[4], encoding_utf8;
  صحیح gl_سیٹ, gr_سیٹ, gsingle_سیٹ;

  ساخت VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  صحیح bold_is_highbright;

  غیرعلامتی صحیح protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  ساخت {
    VTermPos pos;
    ساخت VTermPen pen;

    ساخت {
      غیرعلامتی صحیح cursor_visible:1;
      غیرعلامتی صحیح cursor_blink:1;
      غیرعلامتی صحیح cursor_shape:2;
    } منوال;
  } saved;

  /* Temporary state for DECRQSS parsing */
  اتحاد {
    حرف decrqss[4];
    ساخت {
      uint16_t نقاب;
      شمار {
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

  ساخت {
    ثابت VTermSelectionCallbacks *callbacks;
    خالی *user;
    حرف *بفر;
    size_t buflen;
  } selection;
};

ساخت VTerm
{
  ثابت VTermAllocatorFunctions *allocator;
  خالی *allocdata;

  صحیح صفیں_2;
  صحیح ستون_تعداد;

  ساخت {
    غیرعلامتی صحیح utf8:1;
    غیرعلامتی صحیح ctrl8bit:1;
  } منوال;

  ساخت {
    شمار VTermParserState {
      معمول_2,
      CSI_LEADER,
      CSI_دلائل,
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

    غیرعلامتی صحیح اندر_esc : 1;

    صحیح intermedlen;
    حرف intermed[INTERMED_MAX];

    اتحاد {
      ساخت {
        صحیح leaderlen;
        حرف leader[CSI_LEADER_MAX];

        صحیح argi;
        طویل دلائل[CSI_دلائل_MAX];
      } csi;
      ساخت {
        صحیح command;
      } osc;
      ساخت {
        صحیح commandlen;
        حرف command[CSI_LEADER_MAX];
      } dcs;
    } v;

    ثابت VTermParserCallbacks *callbacks;
    خالی *cbdata;

    صحیح string_initial;

    صحیح emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  خالی                *outdata;

  حرف  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  حرف  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  صحیح اندر_backspace;
};

ساخت VTermEncoding {
  خالی (*init) (VTermEncoding *enc, خالی *ڈیٹا);
  خالی (*decode)(VTermEncoding *enc, خالی *ڈیٹا,
                 uint32_t cp[], صحیح *cpi, صحیح cplen,
                 ثابت حرف بائٹیں[], size_t *pos, size_t len);
};

قسمتعریف شمار {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

خالی *vterm_allocator_malloc(VTerm *vt, size_t سائز);
خالی  vterm_allocator_free(VTerm *vt, خالی *ptr);

خالی vterm_ڈالو_output_بائٹیں(VTerm *vt, ثابت حرف *بائٹیں, size_t len);
خالی vterm_ڈالو_output_vsprintf(VTerm *vt, ثابت حرف *format, va_list دلائل);
خالی vterm_ڈالو_output_sprintf(VTerm *vt, ثابت حرف *format, ...);
خالی vterm_ڈالو_output_sprintf_ctrl(VTerm *vt, غیرعلامتی حرف ctrl, ثابت حرف *fmt, ...);
خالی vterm_ڈالو_output_sprintf_str(VTerm *vt, غیرعلامتی حرف ctrl, صحیح رکن, ثابت حرف *fmt, ...);

خالی vterm_state_free(VTermState *state);

خالی vterm_state_newpen(VTermState *state);
خالی vterm_state_resetpen(VTermState *state);
خالی vterm_state_setpen(VTermState *state, ثابت طویل دلائل[], صحیح argcount);
صحیح  vterm_state_getpen(VTermState *state, طویل دلائل[], صحیح argcount);
خالی vterm_state_savepen(VTermState *state, صحیح save);

شمار {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

خالی vterm_state_ڈالو_output_sprintf_CSI(VTermState *vts, ثابت حرف *format, ...);

خالی vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, حرف designation);

صحیح vterm_unicode_چوڑائی(uint32_t codepoint);
صحیح vterm_unicode_is_combining(uint32_t codepoint);
صحیح vterm_unicode_is_ambiguous(uint32_t codepoint);
صحیح vterm_get_special_pty_type(خالی);

#اگر (defined(XOPEN_ماخذ) && XOPEN_ماخذ >= 500) \
	|| defined(ISOC99_ماخذ) || defined(BSD_ماخذ)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#ورنہ
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
صحیح VSNPRINTF(حرف *str, size_t str_m, ثابت حرف *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
صحیح SNPRINTF(حرف *str, size_t str_m, ثابت حرف *fmt, ...);
# endif
#اگر_ختم
#اگر_غیر_تعریف_شدہ SNPRINTF
# define SNPRINTF snprintf
#اگر_ختم


#اگر_ختم
