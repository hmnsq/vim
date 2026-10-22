#ifndef __VTERM_INTERNAL_H__
#define __VTERM_INTERNAL_H__

#include "vterm.h"

#include <stdarg.h>

#ຖ້າ defined(__GNUC__) && !defined(__MINGW32__)
# define INTERNAL __attribute__((visibility("internal")))
#ບໍ່ດັ່ງນັ້ນ
# define INTERNAL
#endif

#ຖ້າ defined(__GNUC__) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#ບໍ່ດັ່ງນັ້ນ
# define UNUSED
#endif

#ifdef DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#ບໍ່ດັ່ງນັ້ນ
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

typedef sa_nracanaa VTermEncoding VTermEncoding;

typedef sa_nracanaa {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  var_na           data[4*parimaa_na(uint32_t)];
} VTermEncodingInstance;

sa_nracanaa VTermPen
{
  VTermColor fg;
  VTermColor bg;
  unsigned ຈຳນວນເຕັມ bold:1;
  unsigned ຈຳນວນເຕັມ underline:2;
  unsigned ຈຳນວນເຕັມ italic:1;
  unsigned ຈຳນວນເຕັມ blink:1;
  unsigned ຈຳນວນເຕັມ reverse:1;
  unsigned ຈຳນວນເຕັມ conceal:1;
  unsigned ຈຳນວນເຕັມ strike:1;
  unsigned ຈຳນວນເຕັມ font:4; /* To store 0-9 */
  unsigned ຈຳນວນເຕັມ small:1;
  unsigned ຈຳນວນເຕັມ baseline:2;
};

sa_nracanaa VTermState
{
  VTerm *vt;

  const VTermStateCallbacks *callbacks;
  void *cbdata;
  ຈຳນວນເຕັມ callbacks_has_premove;

  const VTermStateFallbacks *fallbacks;
  void *fbdata;

  ຈຳນວນເຕັມ rows;
  ຈຳນວນເຕັມ cols;

  /* Current cursor position */
  VTermPos pos;

  ຈຳນວນເຕັມ at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  ຈຳນວນເຕັມ scrollregion_top;
  ຈຳນວນເຕັມ scrollregion_bottom; /* -1 means unbounded */
#define SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->rows)
  ຈຳນວນເຕັມ scrollregion_left;
#define SCROLLREGION_LEFT(state)  ((state)->mode.leftrightmargin ? (state)->scrollregion_left : 0)
  ຈຳນວນເຕັມ scrollregion_right; /* -1 means unbounded */
#define SCROLLREGION_RIGHT(state) ((state)->mode.leftrightmargin && (state)->scrollregion_right > -1 ? (state)->scrollregion_right : (state)->cols)

  /* Bitvector of tab stops */
  unsigned var_na *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTermLineInfo *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTermLineInfo *lineinfo;
#define ROWWIDTH(state,row) ((state)->lineinfo[(row)].doublewidth ? ((state)->cols / 2) : (state)->cols)
#define THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.row)

  /* Mouse state */
  ຈຳນວນເຕັມ mouse_col, mouse_row;
  ຈຳນວນເຕັມ mouse_buttons;
  ຈຳນວນເຕັມ mouse_flags;

  ga_nanaa { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_size; // Number of ELEMENTS in the above
  ຈຳນວນເຕັມ combine_width; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  sa_nracanaa {
    unsigned ຈຳນວນເຕັມ keypad:1;
    unsigned ຈຳນວນເຕັມ cursor:1;
    unsigned ຈຳນວນເຕັມ autowrap:1;
    unsigned ຈຳນວນເຕັມ insert:1;
    unsigned ຈຳນວນເຕັມ newline:1;
    unsigned ຈຳນວນເຕັມ cursor_visible:1;
    unsigned ຈຳນວນເຕັມ cursor_blink:1;
    unsigned ຈຳນວນເຕັມ cursor_shape:2;
    unsigned ຈຳນວນເຕັມ alt_screen:1;
    unsigned ຈຳນວນເຕັມ origin:1;
    unsigned ຈຳນວນເຕັມ screen:1;
    unsigned ຈຳນວນເຕັມ leftrightmargin:1;
    unsigned ຈຳນວນເຕັມ bracketpaste:1;
    unsigned ຈຳນວນເຕັມ report_focus:1;
    unsigned ຈຳນວນເຕັມ modify_other_keys:1;
    unsigned ຈຳນວນເຕັມ kitty_keyboard:1;
  } mode;

  VTermEncodingInstance encoding[4], encoding_utf8;
  ຈຳນວນເຕັມ gl_set, gr_set, gsingle_set;

  sa_nracanaa VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  ຈຳນວນເຕັມ bold_is_highbright;

  unsigned ຈຳນວນເຕັມ protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  sa_nracanaa {
    VTermPos pos;
    sa_nracanaa VTermPen pen;

    sa_nracanaa {
      unsigned ຈຳນວນເຕັມ cursor_visible:1;
      unsigned ຈຳນວນເຕັມ cursor_blink:1;
      unsigned ຈຳນວນເຕັມ cursor_shape:2;
    } mode;
  } saved;

  /* Temporary state for DECRQSS parsing */
  sa_ngha {
    var_na decrqss[4];
    sa_nracanaa {
      uint16_t mask;
      ga_nanaa {
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

  sa_nracanaa {
    const VTermSelectionCallbacks *callbacks;
    void *user;
    var_na *buffer;
    size_t buflen;
  } selection;
};

sa_nracanaa VTerm
{
  const VTermAllocatorFunctions *allocator;
  void *allocdata;

  ຈຳນວນເຕັມ rows;
  ຈຳນວນເຕັມ cols;

  sa_nracanaa {
    unsigned ຈຳນວນເຕັມ utf8:1;
    unsigned ຈຳນວນເຕັມ ctrl8bit:1;
  } mode;

  sa_nracanaa {
    ga_nanaa VTermParserState {
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

    unsigned ຈຳນວນເຕັມ in_esc : 1;

    ຈຳນວນເຕັມ intermedlen;
    var_na intermed[INTERMED_MAX];

    sa_ngha {
      sa_nracanaa {
        ຈຳນວນເຕັມ leaderlen;
        var_na leader[CSI_LEADER_MAX];

        ຈຳນວນເຕັມ argi;
        long args[CSI_ARGS_MAX];
      } csi;
      sa_nracanaa {
        ຈຳນວນເຕັມ command;
      } osc;
      sa_nracanaa {
        ຈຳນວນເຕັມ commandlen;
        var_na command[CSI_LEADER_MAX];
      } dcs;
    } v;

    const VTermParserCallbacks *callbacks;
    void *cbdata;

    ຈຳນວນເຕັມ string_initial;

    ຈຳນວນເຕັມ emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  void                *outdata;

  var_na  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  var_na  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  ຈຳນວນເຕັມ in_backspace;
};

sa_nracanaa VTermEncoding {
  void (*init) (VTermEncoding *enc, void *data);
  void (*decode)(VTermEncoding *enc, void *data,
                 uint32_t cp[], ຈຳນວນເຕັມ *cpi, ຈຳນວນເຕັມ cplen,
                 const var_na bytes[], size_t *pos, size_t len);
};

typedef ga_nanaa {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

void *vterm_allocator_malloc(VTerm *vt, size_t size);
void  vterm_allocator_free(VTerm *vt, void *ptr);

void vterm_push_output_bytes(VTerm *vt, const var_na *bytes, size_t len);
void vterm_push_output_vsprintf(VTerm *vt, const var_na *format, va_list args);
void vterm_push_output_sprintf(VTerm *vt, const var_na *format, ...);
void vterm_push_output_sprintf_ctrl(VTerm *vt, unsigned var_na ctrl, const var_na *fmt, ...);
void vterm_push_output_sprintf_str(VTerm *vt, unsigned var_na ctrl, ຈຳນວນເຕັມ term, const var_na *fmt, ...);

void vterm_state_free(VTermState *state);

void vterm_state_newpen(VTermState *state);
void vterm_state_resetpen(VTermState *state);
void vterm_state_setpen(VTermState *state, const long args[], ຈຳນວນເຕັມ argcount);
ຈຳນວນເຕັມ  vterm_state_getpen(VTermState *state, long args[], ຈຳນວນເຕັມ argcount);
void vterm_state_savepen(VTermState *state, ຈຳນວນເຕັມ save);

ga_nanaa {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

void vterm_state_push_output_sprintf_CSI(VTermState *vts, const var_na *format, ...);

void vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, var_na designation);

ຈຳນວນເຕັມ vterm_unicode_width(uint32_t codepoint);
ຈຳນວນເຕັມ vterm_unicode_is_combining(uint32_t codepoint);
ຈຳນວນເຕັມ vterm_unicode_is_ambiguous(uint32_t codepoint);
ຈຳນວນເຕັມ vterm_get_special_pty_type(void);

#ຖ້າ (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500) \
	|| defined(_ISOC99_SOURCE) || defined(_BSD_SOURCE)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#ບໍ່ດັ່ງນັ້ນ
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
ຈຳນວນເຕັມ VSNPRINTF(var_na *str, size_t str_m, const var_na *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
ຈຳນວນເຕັມ SNPRINTF(var_na *str, size_t str_m, const var_na *fmt, ...);
# endif
#endif
#ifndef SNPRINTF
# define SNPRINTF snprintf
#endif


#endif
