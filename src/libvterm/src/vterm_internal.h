#ifndef __VTERM_INTERNAL_H__
#define __VTERM_INTERNAL_H__

#অন্তর্ভাব "vterm.h"

#অন্তর্ভাব <stdarg.h>

#যদি defined(__GNUC__) && !defined(__MINGW32__)
# define INTERNAL __attribute__((visibility("internal")))
#অন্যথা
# define INTERNAL
#endif

#যদি defined(__GNUC__) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#অন্যথা
# define UNUSED
#endif

#ifdef DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#অন্যথা
# define DEBUG_LOG(s)
# define DEBUG_LOG1(s, a)
# define DEBUG_LOG2(s, a, b)
# define DEBUG_LOG3(s, a, b, c)
#endif

#define ESC_S "\x1b"

#define INTERMED_MAX 16

#define CSI_ARGS_MAX 16
#define CSI_LEADER_MAX 16

#define BUFIDX_PRIMARY   0
#define BUFIDX_ALTSCREEN 1

প্রকারলেখ কাঠামো VTermEncoding VTermEncoding;

প্রকারলেখ কাঠামো {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  অক্ষর           তথ্য[4*আকার(uint32_t)];
} VTermEncodingInstance;

কাঠামো VTermPen
{
  VTermColor fg;
  VTermColor bg;
  unsigned পূর্ণসংখ্যা bold:1;
  unsigned পূর্ণসংখ্যা underline:2;
  unsigned পূর্ণসংখ্যা italic:1;
  unsigned পূর্ণসংখ্যা blink:1;
  unsigned পূর্ণসংখ্যা reverse:1;
  unsigned পূর্ণসংখ্যা conceal:1;
  unsigned পূর্ণসংখ্যা strike:1;
  unsigned পূর্ণসংখ্যা font:4; /* To store 0-9 */
  unsigned পূর্ণসংখ্যা small:1;
  unsigned পূর্ণসংখ্যা baseline:2;
};

কাঠামো VTermState
{
  VTerm *vt;

  ধ্রুবক VTermStateCallbacks *callbacks;
  শূন্য *cbdata;
  পূর্ণসংখ্যা callbacks_has_premove;

  ধ্রুবক VTermStateFallbacks *fallbacks;
  শূন্য *fbdata;

  পূর্ণসংখ্যা rows;
  পূর্ণসংখ্যা cols;

  /* Current cursor position */
  VTermPos pos;

  পূর্ণসংখ্যা at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  পূর্ণসংখ্যা scrollregion_শীর্ষ;
  পূর্ণসংখ্যা scrollregion_bottom; /* -1 means unbounded */
#define SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->rows)
  পূর্ণসংখ্যা scrollregion_বাম;
#define SCROLLREGION_বাম(state)  ((state)->mode.leftrightmargin ? (state)->scrollregion_বাম : 0)
  পূর্ণসংখ্যা scrollregion_ডান; /* -1 means unbounded */
#define SCROLLREGION_ডান(state) ((state)->mode.leftrightmargin && (state)->scrollregion_ডান > -1 ? (state)->scrollregion_ডান : (state)->cols)

  /* Bitvector of tab stops */
  unsigned অক্ষর *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_সারি_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_সারি_Info *lineinfo;
#define ROWWIDTH(state,row) ((state)->lineinfo[(row)].doublewidth ? ((state)->cols / 2) : (state)->cols)
#define THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.row)

  /* Mouse state */
  পূর্ণসংখ্যা mouse_col, mouse_row;
  পূর্ণসংখ্যা mouse_buttons;
  পূর্ণসংখ্যা mouse_flags;

  গণনা { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_আকার; // Number of ELEMENTS in the above
  পূর্ণসংখ্যা combine_width; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  কাঠামো {
    unsigned পূর্ণসংখ্যা keypad:1;
    unsigned পূর্ণসংখ্যা cursor:1;
    unsigned পূর্ণসংখ্যা autowrap:1;
    unsigned পূর্ণসংখ্যা ঢোকাও:1;
    unsigned পূর্ণসংখ্যা newline:1;
    unsigned পূর্ণসংখ্যা cursor_visible:1;
    unsigned পূর্ণসংখ্যা cursor_blink:1;
    unsigned পূর্ণসংখ্যা cursor_shape:2;
    unsigned পূর্ণসংখ্যা alt_screen:1;
    unsigned পূর্ণসংখ্যা origin:1;
    unsigned পূর্ণসংখ্যা screen:1;
    unsigned পূর্ণসংখ্যা leftrightmargin:1;
    unsigned পূর্ণসংখ্যা bracketpaste:1;
    unsigned পূর্ণসংখ্যা report_focus:1;
    unsigned পূর্ণসংখ্যা modify_other_চাবিগুলি:1;
    unsigned পূর্ণসংখ্যা kitty_keyboard:1;
  } mode;

  VTermEncodingInstance encoding[4], encoding_utf8;
  পূর্ণসংখ্যা gl_set, gr_set, gsingle_set;

  কাঠামো VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  পূর্ণসংখ্যা bold_is_highbright;

  unsigned পূর্ণসংখ্যা protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  কাঠামো {
    VTermPos pos;
    কাঠামো VTermPen pen;

    কাঠামো {
      unsigned পূর্ণসংখ্যা cursor_visible:1;
      unsigned পূর্ণসংখ্যা cursor_blink:1;
      unsigned পূর্ণসংখ্যা cursor_shape:2;
    } mode;
  } saved;

  /* Temporary state for DECRQSS parsing */
  সংঘ {
    অক্ষর decrqss[4];
    কাঠামো {
      uint16_t mask;
      গণনা {
        SELECTION_INITIAL,
        SELECTION_SELECTED,
        SELECTION_QUERY,
        SELECTION_SET_INITIAL,
        SELECTION_SET,
        SELECTION_INVALID,
      } state : 8;
      uint32_t recvpartial;
      uint32_t sendpartial;
    } selection;
  } tmp;

  কাঠামো {
    ধ্রুবক VTermSelectionCallbacks *callbacks;
    শূন্য *user;
    অক্ষর *বাফার;
    size_t buflen;
  } selection;
};

কাঠামো VTerm
{
  ধ্রুবক VTermAllocatorFunctions *allocator;
  শূন্য *allocdata;

  পূর্ণসংখ্যা rows;
  পূর্ণসংখ্যা cols;

  কাঠামো {
    unsigned পূর্ণসংখ্যা utf8:1;
    unsigned পূর্ণসংখ্যা ctrl8bit:1;
  } mode;

  কাঠামো {
    গণনা VTermParserState {
      NORMAL,
      CSI_LEADER,
      CSI_ARGS,
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

    unsigned পূর্ণসংখ্যা in_esc : 1;

    পূর্ণসংখ্যা intermedlen;
    অক্ষর intermed[INTERMED_MAX];

    সংঘ {
      কাঠামো {
        পূর্ণসংখ্যা leaderlen;
        অক্ষর leader[CSI_LEADER_MAX];

        পূর্ণসংখ্যা argi;
        long args[CSI_ARGS_MAX];
      } csi;
      কাঠামো {
        পূর্ণসংখ্যা command;
      } osc;
      কাঠামো {
        পূর্ণসংখ্যা commandlen;
        অক্ষর command[CSI_LEADER_MAX];
      } dcs;
    } v;

    ধ্রুবক VTermParserCallbacks *callbacks;
    শূন্য *cbdata;

    পূর্ণসংখ্যা string_initial;

    পূর্ণসংখ্যা emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  শূন্য                *outdata;

  অক্ষর  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  অক্ষর  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  পূর্ণসংখ্যা in_backspace;
};

কাঠামো VTermEncoding {
  শূন্য (*init) (VTermEncoding *enc, শূন্য *তথ্য);
  শূন্য (*decode)(VTermEncoding *enc, শূন্য *তথ্য,
                 uint32_t cp[], পূর্ণসংখ্যা *cpi, পূর্ণসংখ্যা cplen,
                 ধ্রুবক অক্ষর bytes[], size_t *pos, size_t len);
};

প্রকারলেখ গণনা {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

শূন্য *vterm_allocator_malloc(VTerm *vt, size_t আকার_2);
শূন্য  vterm_allocator_free(VTerm *vt, শূন্য *ptr);

শূন্য vterm_ঠেলো_output_bytes(VTerm *vt, ধ্রুবক অক্ষর *bytes, size_t len);
শূন্য vterm_ঠেলো_output_vsprintf(VTerm *vt, ধ্রুবক অক্ষর *format, va_list args);
শূন্য vterm_ঠেলো_output_sprintf(VTerm *vt, ধ্রুবক অক্ষর *format, ...);
শূন্য vterm_ঠেলো_output_sprintf_ctrl(VTerm *vt, unsigned অক্ষর ctrl, ধ্রুবক অক্ষর *fmt, ...);
শূন্য vterm_ঠেলো_output_sprintf_str(VTerm *vt, unsigned অক্ষর ctrl, পূর্ণসংখ্যা term, ধ্রুবক অক্ষর *fmt, ...);

শূন্য vterm_state_free(VTermState *state);

শূন্য vterm_state_newpen(VTermState *state);
শূন্য vterm_state_resetpen(VTermState *state);
শূন্য vterm_state_setpen(VTermState *state, ধ্রুবক long args[], পূর্ণসংখ্যা argcount);
পূর্ণসংখ্যা  vterm_state_getpen(VTermState *state, long args[], পূর্ণসংখ্যা argcount);
শূন্য vterm_state_savepen(VTermState *state, পূর্ণসংখ্যা save);

গণনা {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

শূন্য vterm_state_ঠেলো_output_sprintf_CSI(VTermState *vts, ধ্রুবক অক্ষর *format, ...);

শূন্য vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, অক্ষর designation);

পূর্ণসংখ্যা vterm_unicode_width(uint32_t codepoint);
পূর্ণসংখ্যা vterm_unicode_is_combining(uint32_t codepoint);
পূর্ণসংখ্যা vterm_unicode_is_ambiguous(uint32_t codepoint);
পূর্ণসংখ্যা vterm_get_special_pty_type(শূন্য);

#যদি (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500) \
	|| defined(_ISOC99_SOURCE) || defined(_BSD_SOURCE)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#অন্যথা
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
পূর্ণসংখ্যা VSNPRINTF(অক্ষর *str, size_t str_m, ধ্রুবক অক্ষর *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
পূর্ণসংখ্যা SNPRINTF(অক্ষর *str, size_t str_m, ধ্রুবক অক্ষর *fmt, ...);
# endif
#endif
#ifndef SNPRINTF
# define SNPRINTF snprintf
#endif


#endif
