#ifndef __VTERM_INTERNAL_H__
#define __VTERM_INTERNAL_H__

#അന്തര്ഭാവ "vterm.h"

#അന്തര്ഭാവ <stdarg.h>

#എങ്കിൽ defined(__GNUC__) && !defined(__MINGW32__)
# define INTERNAL __attribute__((visibility("internal")))
#അല്ലെങ്കിൽ
# define INTERNAL
#endif

#എങ്കിൽ defined(__GNUC__) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#അല്ലെങ്കിൽ
# define UNUSED
#endif

#ifdef DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#അല്ലെങ്കിൽ
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

തരനിർവചനം ഘടന VTermEncoding VTermEncoding;

തരനിർവചനം ഘടന {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  അക്ഷരം           data[4*വലുപ്പം(uint32_t)];
} VTermEncodingInstance;

ഘടന VTermPen
{
  VTermColor fg;
  VTermColor bg;
  unsigned പൂർണ്ണസംഖ്യ bold:1;
  unsigned പൂർണ്ണസംഖ്യ underline:2;
  unsigned പൂർണ്ണസംഖ്യ italic:1;
  unsigned പൂർണ്ണസംഖ്യ blink:1;
  unsigned പൂർണ്ണസംഖ്യ reverse:1;
  unsigned പൂർണ്ണസംഖ്യ conceal:1;
  unsigned പൂർണ്ണസംഖ്യ strike:1;
  unsigned പൂർണ്ണസംഖ്യ font:4; /* To store 0-9 */
  unsigned പൂർണ്ണസംഖ്യ small:1;
  unsigned പൂർണ്ണസംഖ്യ baseline:2;
};

ഘടന VTermState
{
  VTerm *vt;

  സ്ഥിരം VTermStateCallbacks *callbacks;
  ശൂന്യം *cbdata;
  പൂർണ്ണസംഖ്യ callbacks_has_premove;

  സ്ഥിരം VTermStateFallbacks *fallbacks;
  ശൂന്യം *fbdata;

  പൂർണ്ണസംഖ്യ rows;
  പൂർണ്ണസംഖ്യ cols;

  /* Current cursor position */
  VTermPos pos;

  പൂർണ്ണസംഖ്യ at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  പൂർണ്ണസംഖ്യ scrollregion_മുകൾ;
  പൂർണ്ണസംഖ്യ scrollregion_bottom; /* -1 means unbounded */
#define SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->rows)
  പൂർണ്ണസംഖ്യ scrollregion_ഇടത്;
#define SCROLLREGION_ഇടത്(state)  ((state)->mode.leftrightmargin ? (state)->scrollregion_ഇടത് : 0)
  പൂർണ്ണസംഖ്യ scrollregion_വലത്; /* -1 means unbounded */
#define SCROLLREGION_വലത്(state) ((state)->mode.leftrightmargin && (state)->scrollregion_വലത് > -1 ? (state)->scrollregion_വലത് : (state)->cols)

  /* Bitvector of tab stops */
  unsigned അക്ഷരം *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_വരി_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_വരി_Info *lineinfo;
#define ROWWIDTH(state,row) ((state)->lineinfo[(row)].doublewidth ? ((state)->cols / 2) : (state)->cols)
#define THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.row)

  /* Mouse state */
  പൂർണ്ണസംഖ്യ mouse_col, mouse_row;
  പൂർണ്ണസംഖ്യ mouse_buttons;
  പൂർണ്ണസംഖ്യ mouse_flags;

  എണ്ണൽ { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_size; // Number of ELEMENTS in the above
  പൂർണ്ണസംഖ്യ combine_width; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  ഘടന {
    unsigned പൂർണ്ണസംഖ്യ keypad:1;
    unsigned പൂർണ്ണസംഖ്യ cursor:1;
    unsigned പൂർണ്ണസംഖ്യ autowrap:1;
    unsigned പൂർണ്ണസംഖ്യ തിരുകുക:1;
    unsigned പൂർണ്ണസംഖ്യ newline:1;
    unsigned പൂർണ്ണസംഖ്യ cursor_visible:1;
    unsigned പൂർണ്ണസംഖ്യ cursor_blink:1;
    unsigned പൂർണ്ണസംഖ്യ cursor_shape:2;
    unsigned പൂർണ്ണസംഖ്യ alt_screen:1;
    unsigned പൂർണ്ണസംഖ്യ origin:1;
    unsigned പൂർണ്ണസംഖ്യ screen:1;
    unsigned പൂർണ്ണസംഖ്യ leftrightmargin:1;
    unsigned പൂർണ്ണസംഖ്യ bracketpaste:1;
    unsigned പൂർണ്ണസംഖ്യ report_focus:1;
    unsigned പൂർണ്ണസംഖ്യ modify_other_താക്കോലുകൾ:1;
    unsigned പൂർണ്ണസംഖ്യ kitty_keyboard:1;
  } mode;

  VTermEncodingInstance encoding[4], encoding_utf8;
  പൂർണ്ണസംഖ്യ gl_set, gr_set, gsingle_set;

  ഘടന VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  പൂർണ്ണസംഖ്യ bold_is_highbright;

  unsigned പൂർണ്ണസംഖ്യ protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  ഘടന {
    VTermPos pos;
    ഘടന VTermPen pen;

    ഘടന {
      unsigned പൂർണ്ണസംഖ്യ cursor_visible:1;
      unsigned പൂർണ്ണസംഖ്യ cursor_blink:1;
      unsigned പൂർണ്ണസംഖ്യ cursor_shape:2;
    } mode;
  } saved;

  /* Temporary state for DECRQSS parsing */
  സംഘം {
    അക്ഷരം decrqss[4];
    ഘടന {
      uint16_t mask;
      എണ്ണൽ {
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

  ഘടന {
    സ്ഥിരം VTermSelectionCallbacks *callbacks;
    ശൂന്യം *user;
    അക്ഷരം *buffer;
    size_t buflen;
  } selection;
};

ഘടന VTerm
{
  സ്ഥിരം VTermAllocatorFunctions *allocator;
  ശൂന്യം *allocdata;

  പൂർണ്ണസംഖ്യ rows;
  പൂർണ്ണസംഖ്യ cols;

  ഘടന {
    unsigned പൂർണ്ണസംഖ്യ utf8:1;
    unsigned പൂർണ്ണസംഖ്യ ctrl8bit:1;
  } mode;

  ഘടന {
    എണ്ണൽ VTermParserState {
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

    unsigned പൂർണ്ണസംഖ്യ in_esc : 1;

    പൂർണ്ണസംഖ്യ intermedlen;
    അക്ഷരം intermed[INTERMED_MAX];

    സംഘം {
      ഘടന {
        പൂർണ്ണസംഖ്യ leaderlen;
        അക്ഷരം leader[CSI_LEADER_MAX];

        പൂർണ്ണസംഖ്യ argi;
        long args[CSI_ARGS_MAX];
      } csi;
      ഘടന {
        പൂർണ്ണസംഖ്യ command;
      } osc;
      ഘടന {
        പൂർണ്ണസംഖ്യ commandlen;
        അക്ഷരം command[CSI_LEADER_MAX];
      } dcs;
    } v;

    സ്ഥിരം VTermParserCallbacks *callbacks;
    ശൂന്യം *cbdata;

    പൂർണ്ണസംഖ്യ string_initial;

    പൂർണ്ണസംഖ്യ emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  ശൂന്യം                *outdata;

  അക്ഷരം  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  അക്ഷരം  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  പൂർണ്ണസംഖ്യ in_backspace;
};

ഘടന VTermEncoding {
  ശൂന്യം (*init) (VTermEncoding *enc, ശൂന്യം *data);
  ശൂന്യം (*decode)(VTermEncoding *enc, ശൂന്യം *data,
                 uint32_t cp[], പൂർണ്ണസംഖ്യ *cpi, പൂർണ്ണസംഖ്യ cplen,
                 സ്ഥിരം അക്ഷരം bytes[], size_t *pos, size_t len);
};

തരനിർവചനം എണ്ണൽ {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

ശൂന്യം *vterm_allocator_malloc(VTerm *vt, size_t size);
ശൂന്യം  vterm_allocator_free(VTerm *vt, ശൂന്യം *ptr);

ശൂന്യം vterm_തള്ളുക_output_bytes(VTerm *vt, സ്ഥിരം അക്ഷരം *bytes, size_t len);
ശൂന്യം vterm_തള്ളുക_output_vsprintf(VTerm *vt, സ്ഥിരം അക്ഷരം *format, va_list args);
ശൂന്യം vterm_തള്ളുക_output_sprintf(VTerm *vt, സ്ഥിരം അക്ഷരം *format, ...);
ശൂന്യം vterm_തള്ളുക_output_sprintf_ctrl(VTerm *vt, unsigned അക്ഷരം ctrl, സ്ഥിരം അക്ഷരം *fmt, ...);
ശൂന്യം vterm_തള്ളുക_output_sprintf_str(VTerm *vt, unsigned അക്ഷരം ctrl, പൂർണ്ണസംഖ്യ term, സ്ഥിരം അക്ഷരം *fmt, ...);

ശൂന്യം vterm_state_free(VTermState *state);

ശൂന്യം vterm_state_newpen(VTermState *state);
ശൂന്യം vterm_state_resetpen(VTermState *state);
ശൂന്യം vterm_state_setpen(VTermState *state, സ്ഥിരം long args[], പൂർണ്ണസംഖ്യ argcount);
പൂർണ്ണസംഖ്യ  vterm_state_getpen(VTermState *state, long args[], പൂർണ്ണസംഖ്യ argcount);
ശൂന്യം vterm_state_savepen(VTermState *state, പൂർണ്ണസംഖ്യ save);

എണ്ണൽ {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

ശൂന്യം vterm_state_തള്ളുക_output_sprintf_CSI(VTermState *vts, സ്ഥിരം അക്ഷരം *format, ...);

ശൂന്യം vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, അക്ഷരം designation);

പൂർണ്ണസംഖ്യ vterm_unicode_width(uint32_t codepoint);
പൂർണ്ണസംഖ്യ vterm_unicode_is_combining(uint32_t codepoint);
പൂർണ്ണസംഖ്യ vterm_unicode_is_ambiguous(uint32_t codepoint);
പൂർണ്ണസംഖ്യ vterm_get_special_pty_type(ശൂന്യം);

#എങ്കിൽ (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500) \
	|| defined(_ISOC99_SOURCE) || defined(_BSD_SOURCE)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#അല്ലെങ്കിൽ
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
പൂർണ്ണസംഖ്യ VSNPRINTF(അക്ഷരം *str, size_t str_m, സ്ഥിരം അക്ഷരം *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
പൂർണ്ണസംഖ്യ SNPRINTF(അക്ഷരം *str, size_t str_m, സ്ഥിരം അക്ഷരം *fmt, ...);
# endif
#endif
#ifndef SNPRINTF
# define SNPRINTF snprintf
#endif


#endif
