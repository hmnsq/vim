#ifndef __VTERM_INTERNAL_H__
#define __VTERM_INTERNAL_H__

#include "vterm.h"

#include <stdarg.h>

#если defined(__GNUC__) && !defined(__MINGW32__)
# define INTERNAL __attribute__((visibility("internal")))
#иначе
# define INTERNAL
#endif

#если defined(__GNUC__) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#иначе
# define UNUSED
#endif

#ifdef DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#иначе
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

typedef struct VTermEncoding VTermEncoding;

typedef struct {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  char           data[4*sizeof(uint32_t)];
} VTermEncodingInstance;

struct VTermPen
{
  VTermColor fg;
  VTermColor bg;
  unsigned целое bold:1;
  unsigned целое underline:2;
  unsigned целое italic:1;
  unsigned целое blink:1;
  unsigned целое reverse:1;
  unsigned целое conceal:1;
  unsigned целое strike:1;
  unsigned целое font:4; /* To store 0-9 */
  unsigned целое small:1;
  unsigned целое baseline:2;
};

struct VTermState
{
  VTerm *vt;

  const VTermStateCallbacks *callbacks;
  void *cbdata;
  целое callbacks_has_premove;

  const VTermStateFallbacks *fallbacks;
  void *fbdata;

  целое rows;
  целое cols;

  /* Current cursor position */
  VTermPos pos;

  целое at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  целое scrollregion_top;
  целое scrollregion_bottom; /* -1 means unbounded */
#define SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->rows)
  целое scrollregion_left;
#define SCROLLREGION_LEFT(state)  ((state)->mode.leftrightmargin ? (state)->scrollregion_left : 0)
  целое scrollregion_right; /* -1 means unbounded */
#define SCROLLREGION_RIGHT(state) ((state)->mode.leftrightmargin && (state)->scrollregion_right > -1 ? (state)->scrollregion_right : (state)->cols)

  /* Bitvector of tab stops */
  unsigned char *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTermLineInfo *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTermLineInfo *lineinfo;
#define ROWWIDTH(state,row) ((state)->lineinfo[(row)].doublewidth ? ((state)->cols / 2) : (state)->cols)
#define THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.row)

  /* Mouse state */
  целое mouse_col, mouse_row;
  целое mouse_buttons;
  целое mouse_flags;

  enum { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_size; // Number of ELEMENTS in the above
  целое combine_width; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  struct {
    unsigned целое keypad:1;
    unsigned целое cursor:1;
    unsigned целое autowrap:1;
    unsigned целое insert:1;
    unsigned целое newline:1;
    unsigned целое cursor_visible:1;
    unsigned целое cursor_blink:1;
    unsigned целое cursor_shape:2;
    unsigned целое alt_screen:1;
    unsigned целое origin:1;
    unsigned целое screen:1;
    unsigned целое leftrightmargin:1;
    unsigned целое bracketpaste:1;
    unsigned целое report_focus:1;
    unsigned целое modify_other_keys:1;
    unsigned целое kitty_keyboard:1;
  } mode;

  VTermEncodingInstance encoding[4], encoding_utf8;
  целое gl_set, gr_set, gsingle_set;

  struct VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  целое bold_is_highbright;

  unsigned целое protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  struct {
    VTermPos pos;
    struct VTermPen pen;

    struct {
      unsigned целое cursor_visible:1;
      unsigned целое cursor_blink:1;
      unsigned целое cursor_shape:2;
    } mode;
  } saved;

  /* Temporary state for DECRQSS parsing */
  union {
    char decrqss[4];
    struct {
      uint16_t mask;
      enum {
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

  struct {
    const VTermSelectionCallbacks *callbacks;
    void *user;
    char *buffer;
    size_t buflen;
  } selection;
};

struct VTerm
{
  const VTermAllocatorFunctions *allocator;
  void *allocdata;

  целое rows;
  целое cols;

  struct {
    unsigned целое utf8:1;
    unsigned целое ctrl8bit:1;
  } mode;

  struct {
    enum VTermParserState {
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

    unsigned целое in_esc : 1;

    целое intermedlen;
    char intermed[INTERMED_MAX];

    union {
      struct {
        целое leaderlen;
        char leader[CSI_LEADER_MAX];

        целое argi;
        long args[CSI_ARGS_MAX];
      } csi;
      struct {
        целое command;
      } osc;
      struct {
        целое commandlen;
        char command[CSI_LEADER_MAX];
      } dcs;
    } v;

    const VTermParserCallbacks *callbacks;
    void *cbdata;

    целое string_initial;

    целое emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  void                *outdata;

  char  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  char  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  целое in_backspace;
};

struct VTermEncoding {
  void (*init) (VTermEncoding *enc, void *data);
  void (*decode)(VTermEncoding *enc, void *data,
                 uint32_t cp[], целое *cpi, целое cplen,
                 const char bytes[], size_t *pos, size_t len);
};

typedef enum {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

void *vterm_allocator_malloc(VTerm *vt, size_t size);
void  vterm_allocator_free(VTerm *vt, void *ptr);

void vterm_push_output_bytes(VTerm *vt, const char *bytes, size_t len);
void vterm_push_output_vsprintf(VTerm *vt, const char *format, va_list args);
void vterm_push_output_sprintf(VTerm *vt, const char *format, ...);
void vterm_push_output_sprintf_ctrl(VTerm *vt, unsigned char ctrl, const char *fmt, ...);
void vterm_push_output_sprintf_str(VTerm *vt, unsigned char ctrl, целое term, const char *fmt, ...);

void vterm_state_free(VTermState *state);

void vterm_state_newpen(VTermState *state);
void vterm_state_resetpen(VTermState *state);
void vterm_state_setpen(VTermState *state, const long args[], целое argcount);
целое  vterm_state_getpen(VTermState *state, long args[], целое argcount);
void vterm_state_savepen(VTermState *state, целое save);

enum {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

void vterm_state_push_output_sprintf_CSI(VTermState *vts, const char *format, ...);

void vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, char designation);

целое vterm_unicode_width(uint32_t codepoint);
целое vterm_unicode_is_combining(uint32_t codepoint);
целое vterm_unicode_is_ambiguous(uint32_t codepoint);
целое vterm_get_special_pty_type(void);

#если (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500) \
	|| defined(_ISOC99_SOURCE) || defined(_BSD_SOURCE)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#иначе
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
целое VSNPRINTF(char *str, size_t str_m, const char *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
целое SNPRINTF(char *str, size_t str_m, const char *fmt, ...);
# endif
#endif
#ifndef SNPRINTF
# define SNPRINTF snprintf
#endif


#endif
