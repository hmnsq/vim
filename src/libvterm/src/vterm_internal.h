#ifndef __VTERM_INTERNAL_H__
#define __VTERM_INTERNAL_H__

#ಅನ್ತರ್ಭಾವ "vterm.h"

#ಅನ್ತರ್ಭಾವ <stdarg.h>

#ಯದಿ defined(__GNUC__) && !defined(__MINGW32__)
# define INTERNAL __attribute__((visibility("internal")))
#ಅನ್ಯಥಾ
# define INTERNAL
#endif

#ಯದಿ defined(__GNUC__) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#ಅನ್ಯಥಾ
# define UNUSED
#endif

#ifdef DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#ಅನ್ಯಥಾ
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

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ VTermEncoding VTermEncoding;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  ವರ್ಣ           data[4*ಪರಿಮಾಣ(uint32_t)];
} VTermEncodingInstance;

ಸಂರಚನಾ VTermPen
{
  VTermColor fg;
  VTermColor bg;
  unsigned ಪೂರ್ಣಾಙ್ಕ bold:1;
  unsigned ಪೂರ್ಣಾಙ್ಕ underline:2;
  unsigned ಪೂರ್ಣಾಙ್ಕ italic:1;
  unsigned ಪೂರ್ಣಾಙ್ಕ blink:1;
  unsigned ಪೂರ್ಣಾಙ್ಕ reverse:1;
  unsigned ಪೂರ್ಣಾಙ್ಕ conceal:1;
  unsigned ಪೂರ್ಣಾಙ್ಕ strike:1;
  unsigned ಪೂರ್ಣಾಙ್ಕ font:4; /* To store 0-9 */
  unsigned ಪೂರ್ಣಾಙ್ಕ small:1;
  unsigned ಪೂರ್ಣಾಙ್ಕ baseline:2;
};

ಸಂರಚನಾ VTermState
{
  VTerm *vt;

  ಧ್ರುವ VTermStateCallbacks *callbacks;
  ಶೂನ್ಯ *cbdata;
  ಪೂರ್ಣಾಙ್ಕ callbacks_has_premove;

  ಧ್ರುವ VTermStateFallbacks *fallbacks;
  ಶೂನ್ಯ *fbdata;

  ಪೂರ್ಣಾಙ್ಕ rows;
  ಪೂರ್ಣಾಙ್ಕ cols;

  /* Current cursor position */
  VTermPos pos;

  ಪೂರ್ಣಾಙ್ಕ at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  ಪೂರ್ಣಾಙ್ಕ scrollregion_ತುದಿ;
  ಪೂರ್ಣಾಙ್ಕ scrollregion_bottom; /* -1 means unbounded */
#define SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->rows)
  ಪೂರ್ಣಾಙ್ಕ scrollregion_ಎಡ;
#define SCROLLREGION_ಎಡ(state)  ((state)->mode.leftrightmargin ? (state)->scrollregion_ಎಡ : 0)
  ಪೂರ್ಣಾಙ್ಕ scrollregion_ಬಲ; /* -1 means unbounded */
#define SCROLLREGION_ಬಲ(state) ((state)->mode.leftrightmargin && (state)->scrollregion_ಬಲ > -1 ? (state)->scrollregion_ಬಲ : (state)->cols)

  /* Bitvector of tab stops */
  unsigned ವರ್ಣ *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_ಸಾಲು_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_ಸಾಲು_Info *lineinfo;
#define ROWWIDTH(state,row) ((state)->lineinfo[(row)].doublewidth ? ((state)->cols / 2) : (state)->cols)
#define THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.row)

  /* Mouse state */
  ಪೂರ್ಣಾಙ್ಕ mouse_col, mouse_row;
  ಪೂರ್ಣಾಙ್ಕ mouse_buttons;
  ಪೂರ್ಣಾಙ್ಕ mouse_flags;

  ಗಣನಾ { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_size; // Number of ELEMENTS in the above
  ಪೂರ್ಣಾಙ್ಕ combine_width; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  ಸಂರಚನಾ {
    unsigned ಪೂರ್ಣಾಙ್ಕ keypad:1;
    unsigned ಪೂರ್ಣಾಙ್ಕ cursor:1;
    unsigned ಪೂರ್ಣಾಙ್ಕ autowrap:1;
    unsigned ಪೂರ್ಣಾಙ್ಕ ಸೇರಿಸು:1;
    unsigned ಪೂರ್ಣಾಙ್ಕ newline:1;
    unsigned ಪೂರ್ಣಾಙ್ಕ cursor_visible:1;
    unsigned ಪೂರ್ಣಾಙ್ಕ cursor_blink:1;
    unsigned ಪೂರ್ಣಾಙ್ಕ cursor_shape:2;
    unsigned ಪೂರ್ಣಾಙ್ಕ alt_screen:1;
    unsigned ಪೂರ್ಣಾಙ್ಕ origin:1;
    unsigned ಪೂರ್ಣಾಙ್ಕ screen:1;
    unsigned ಪೂರ್ಣಾಙ್ಕ leftrightmargin:1;
    unsigned ಪೂರ್ಣಾಙ್ಕ bracketpaste:1;
    unsigned ಪೂರ್ಣಾಙ್ಕ report_focus:1;
    unsigned ಪೂರ್ಣಾಙ್ಕ modify_other_ಕೀಲಿಗಳು:1;
    unsigned ಪೂರ್ಣಾಙ್ಕ kitty_keyboard:1;
  } mode;

  VTermEncodingInstance encoding[4], encoding_utf8;
  ಪೂರ್ಣಾಙ್ಕ gl_set, gr_set, gsingle_set;

  ಸಂರಚನಾ VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  ಪೂರ್ಣಾಙ್ಕ bold_is_highbright;

  unsigned ಪೂರ್ಣಾಙ್ಕ protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  ಸಂರಚನಾ {
    VTermPos pos;
    ಸಂರಚನಾ VTermPen pen;

    ಸಂರಚನಾ {
      unsigned ಪೂರ್ಣಾಙ್ಕ cursor_visible:1;
      unsigned ಪೂರ್ಣಾಙ್ಕ cursor_blink:1;
      unsigned ಪೂರ್ಣಾಙ್ಕ cursor_shape:2;
    } mode;
  } saved;

  /* Temporary state for DECRQSS parsing */
  ಸಙ್ಘ {
    ವರ್ಣ decrqss[4];
    ಸಂರಚನಾ {
      uint16_t mask;
      ಗಣನಾ {
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

  ಸಂರಚನಾ {
    ಧ್ರುವ VTermSelectionCallbacks *callbacks;
    ಶೂನ್ಯ *user;
    ವರ್ಣ *buffer;
    size_t buflen;
  } selection;
};

ಸಂರಚನಾ VTerm
{
  ಧ್ರುವ VTermAllocatorFunctions *allocator;
  ಶೂನ್ಯ *allocdata;

  ಪೂರ್ಣಾಙ್ಕ rows;
  ಪೂರ್ಣಾಙ್ಕ cols;

  ಸಂರಚನಾ {
    unsigned ಪೂರ್ಣಾಙ್ಕ utf8:1;
    unsigned ಪೂರ್ಣಾಙ್ಕ ctrl8bit:1;
  } mode;

  ಸಂರಚನಾ {
    ಗಣನಾ VTermParserState {
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

    unsigned ಪೂರ್ಣಾಙ್ಕ in_esc : 1;

    ಪೂರ್ಣಾಙ್ಕ intermedlen;
    ವರ್ಣ intermed[INTERMED_MAX];

    ಸಙ್ಘ {
      ಸಂರಚನಾ {
        ಪೂರ್ಣಾಙ್ಕ leaderlen;
        ವರ್ಣ leader[CSI_LEADER_MAX];

        ಪೂರ್ಣಾಙ್ಕ argi;
        long args[CSI_ARGS_MAX];
      } csi;
      ಸಂರಚನಾ {
        ಪೂರ್ಣಾಙ್ಕ command;
      } osc;
      ಸಂರಚನಾ {
        ಪೂರ್ಣಾಙ್ಕ commandlen;
        ವರ್ಣ command[CSI_LEADER_MAX];
      } dcs;
    } v;

    ಧ್ರುವ VTermParserCallbacks *callbacks;
    ಶೂನ್ಯ *cbdata;

    ಪೂರ್ಣಾಙ್ಕ string_initial;

    ಪೂರ್ಣಾಙ್ಕ emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  ಶೂನ್ಯ                *outdata;

  ವರ್ಣ  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  ವರ್ಣ  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  ಪೂರ್ಣಾಙ್ಕ in_backspace;
};

ಸಂರಚನಾ VTermEncoding {
  ಶೂನ್ಯ (*init) (VTermEncoding *enc, ಶೂನ್ಯ *data);
  ಶೂನ್ಯ (*decode)(VTermEncoding *enc, ಶೂನ್ಯ *data,
                 uint32_t cp[], ಪೂರ್ಣಾಙ್ಕ *cpi, ಪೂರ್ಣಾಙ್ಕ cplen,
                 ಧ್ರುವ ವರ್ಣ bytes[], size_t *pos, size_t len);
};

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

ಶೂನ್ಯ *vterm_allocator_malloc(VTerm *vt, size_t size);
ಶೂನ್ಯ  vterm_allocator_free(VTerm *vt, ಶೂನ್ಯ *ptr);

ಶೂನ್ಯ vterm_ತಳ್ಳು_output_bytes(VTerm *vt, ಧ್ರುವ ವರ್ಣ *bytes, size_t len);
ಶೂನ್ಯ vterm_ತಳ್ಳು_output_vsprintf(VTerm *vt, ಧ್ರುವ ವರ್ಣ *format, va_list args);
ಶೂನ್ಯ vterm_ತಳ್ಳು_output_sprintf(VTerm *vt, ಧ್ರುವ ವರ್ಣ *format, ...);
ಶೂನ್ಯ vterm_ತಳ್ಳು_output_sprintf_ctrl(VTerm *vt, unsigned ವರ್ಣ ctrl, ಧ್ರುವ ವರ್ಣ *fmt, ...);
ಶೂನ್ಯ vterm_ತಳ್ಳು_output_sprintf_str(VTerm *vt, unsigned ವರ್ಣ ctrl, ಪೂರ್ಣಾಙ್ಕ term, ಧ್ರುವ ವರ್ಣ *fmt, ...);

ಶೂನ್ಯ vterm_state_free(VTermState *state);

ಶೂನ್ಯ vterm_state_newpen(VTermState *state);
ಶೂನ್ಯ vterm_state_resetpen(VTermState *state);
ಶೂನ್ಯ vterm_state_setpen(VTermState *state, ಧ್ರುವ long args[], ಪೂರ್ಣಾಙ್ಕ argcount);
ಪೂರ್ಣಾಙ್ಕ  vterm_state_getpen(VTermState *state, long args[], ಪೂರ್ಣಾಙ್ಕ argcount);
ಶೂನ್ಯ vterm_state_savepen(VTermState *state, ಪೂರ್ಣಾಙ್ಕ save);

ಗಣನಾ {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

ಶೂನ್ಯ vterm_state_ತಳ್ಳು_output_sprintf_CSI(VTermState *vts, ಧ್ರುವ ವರ್ಣ *format, ...);

ಶೂನ್ಯ vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, ವರ್ಣ designation);

ಪೂರ್ಣಾಙ್ಕ vterm_unicode_width(uint32_t codepoint);
ಪೂರ್ಣಾಙ್ಕ vterm_unicode_is_combining(uint32_t codepoint);
ಪೂರ್ಣಾಙ್ಕ vterm_unicode_is_ambiguous(uint32_t codepoint);
ಪೂರ್ಣಾಙ್ಕ vterm_get_special_pty_type(ಶೂನ್ಯ);

#ಯದಿ (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500) \
	|| defined(_ISOC99_SOURCE) || defined(_BSD_SOURCE)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#ಅನ್ಯಥಾ
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
ಪೂರ್ಣಾಙ್ಕ VSNPRINTF(ವರ್ಣ *str, size_t str_m, ಧ್ರುವ ವರ್ಣ *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
ಪೂರ್ಣಾಙ್ಕ SNPRINTF(ವರ್ಣ *str, size_t str_m, ಧ್ರುವ ವರ್ಣ *fmt, ...);
# endif
#endif
#ifndef SNPRINTF
# define SNPRINTF snprintf
#endif


#endif
