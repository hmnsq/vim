#ifndef __VTERM_INTERNAL_H__
#define __VTERM_INTERNAL_H__

#अन्तर्भाव "vterm.h"

#अन्तर्भाव <stdarg.h>

#जर defined(__GNUC__) && !defined(__MINGW32__)
# define INTERNAL __attribute__((visibility("internal")))
#अन्यथा
# define INTERNAL
#endif

#जर defined(__GNUC__) || defined(__MINGW32__)
# define UNUSED __attribute__((unused))
#अन्यथा
# define UNUSED
#endif

#ifdef DEBUG
# define DEBUG_LOG(s) ख_लिखो(मानक_त्रुटि, s)
# define DEBUG_LOG1(s, a) ख_लिखो(मानक_त्रुटि, s, a)
# define DEBUG_LOG2(s, a, b) ख_लिखो(मानक_त्रुटि, s, a, b)
# define DEBUG_LOG3(s, a, b, c) ख_लिखो(मानक_त्रुटि, s, a, b, c)
#अन्यथा
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

प्रकारव्याख्या संरचना VTermEncoding VTermEncoding;

प्रकारव्याख्या संरचना {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  अक्षर           data[4*आकार(uint32_t)];
} VTermEncodingInstance;

संरचना VTermPen
{
  VTermColor fg;
  VTermColor bg;
  अचिन्हित पूर्णांक bold:1;
  अचिन्हित पूर्णांक underline:2;
  अचिन्हित पूर्णांक italic:1;
  अचिन्हित पूर्णांक blink:1;
  अचिन्हित पूर्णांक reverse:1;
  अचिन्हित पूर्णांक conceal:1;
  अचिन्हित पूर्णांक strike:1;
  अचिन्हित पूर्णांक font:4; /* To store 0-9 */
  अचिन्हित पूर्णांक small:1;
  अचिन्हित पूर्णांक baseline:2;
};

संरचना VTermState
{
  VTerm *vt;

  स्थिरांक VTermStateCallbacks *callbacks;
  शून्य *cbdata;
  पूर्णांक callbacks_has_premove;

  स्थिरांक VTermStateFallbacks *fallbacks;
  शून्य *fbdata;

  पूर्णांक rows;
  पूर्णांक cols;

  /* Current cursor position */
  VTermPos pos;

  पूर्णांक at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  पूर्णांक scrollregion_शीर्ष;
  पूर्णांक scrollregion_bottom; /* -1 means unbounded */
#define SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->rows)
  पूर्णांक scrollregion_डावा;
#define SCROLLREGION_डावा(state)  ((state)->mode.leftrightmargin ? (state)->scrollregion_डावा : 0)
  पूर्णांक scrollregion_उजवा; /* -1 means unbounded */
#define SCROLLREGION_उजवा(state) ((state)->mode.leftrightmargin && (state)->scrollregion_उजवा > -1 ? (state)->scrollregion_उजवा : (state)->cols)

  /* Bitvector of tab stops */
  अचिन्हित अक्षर *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_ओळ_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_ओळ_Info *lineinfo;
#define ROWWIDTH(state,row) ((state)->lineinfo[(row)].doublewidth ? ((state)->cols / 2) : (state)->cols)
#define THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.row)

  /* Mouse state */
  पूर्णांक mouse_col, mouse_row;
  पूर्णांक mouse_buttons;
  पूर्णांक mouse_flags;

  गणना { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  माप_प्रकार combine_chars_size; // Number of ELEMENTS in the above
  पूर्णांक combine_width; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  संरचना {
    अचिन्हित पूर्णांक keypad:1;
    अचिन्हित पूर्णांक cursor:1;
    अचिन्हित पूर्णांक autowrap:1;
    अचिन्हित पूर्णांक घाला:1;
    अचिन्हित पूर्णांक newline:1;
    अचिन्हित पूर्णांक cursor_visible:1;
    अचिन्हित पूर्णांक cursor_blink:1;
    अचिन्हित पूर्णांक cursor_shape:2;
    अचिन्हित पूर्णांक alt_screen:1;
    अचिन्हित पूर्णांक origin:1;
    अचिन्हित पूर्णांक screen:1;
    अचिन्हित पूर्णांक leftrightmargin:1;
    अचिन्हित पूर्णांक bracketpaste:1;
    अचिन्हित पूर्णांक report_focus:1;
    अचिन्हित पूर्णांक modify_other_किल्ल्या:1;
    अचिन्हित पूर्णांक kitty_keyboard:1;
  } mode;

  VTermEncodingInstance encoding[4], encoding_utf8;
  पूर्णांक gl_set, gr_set, gsingle_set;

  संरचना VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  पूर्णांक bold_is_highbright;

  अचिन्हित पूर्णांक protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  संरचना {
    VTermPos pos;
    संरचना VTermPen pen;

    संरचना {
      अचिन्हित पूर्णांक cursor_visible:1;
      अचिन्हित पूर्णांक cursor_blink:1;
      अचिन्हित पूर्णांक cursor_shape:2;
    } mode;
  } saved;

  /* Temporary state for DECRQSS parsing */
  संघ {
    अक्षर decrqss[4];
    संरचना {
      uint16_t mask;
      गणना {
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

  संरचना {
    स्थिरांक VTermSelectionCallbacks *callbacks;
    शून्य *user;
    अक्षर *buffer;
    माप_प्रकार buflen;
  } selection;
};

संरचना VTerm
{
  स्थिरांक VTermAllocatorFunctions *allocator;
  शून्य *allocdata;

  पूर्णांक rows;
  पूर्णांक cols;

  संरचना {
    अचिन्हित पूर्णांक utf8:1;
    अचिन्हित पूर्णांक ctrl8bit:1;
  } mode;

  संरचना {
    गणना VTermParserState {
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

    अचिन्हित पूर्णांक in_esc : 1;

    पूर्णांक intermedlen;
    अक्षर intermed[INTERMED_MAX];

    संघ {
      संरचना {
        पूर्णांक leaderlen;
        अक्षर leader[CSI_LEADER_MAX];

        पूर्णांक argi;
        दीर्घ args[CSI_ARGS_MAX];
      } csi;
      संरचना {
        पूर्णांक command;
      } osc;
      संरचना {
        पूर्णांक commandlen;
        अक्षर command[CSI_LEADER_MAX];
      } dcs;
    } v;

    स्थिरांक VTermParserCallbacks *callbacks;
    शून्य *cbdata;

    पूर्णांक string_initial;

    पूर्णांक emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  शून्य                *outdata;

  अक्षर  *outbuffer;
  माप_प्रकार outbuffer_len;
  माप_प्रकार outbuffer_cur;

  अक्षर  *tmpbuffer;
  माप_प्रकार tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  पूर्णांक in_backspace;
};

संरचना VTermEncoding {
  शून्य (*init) (VTermEncoding *enc, शून्य *data);
  शून्य (*decode)(VTermEncoding *enc, शून्य *data,
                 uint32_t cp[], पूर्णांक *cpi, पूर्णांक cplen,
                 स्थिरांक अक्षर bytes[], माप_प्रकार *pos, माप_प्रकार len);
};

प्रकारव्याख्या गणना {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

शून्य *vterm_allocator_malloc(VTerm *vt, माप_प्रकार size);
शून्य  vterm_allocator_free(VTerm *vt, शून्य *ptr);

शून्य vterm_ढकला_output_bytes(VTerm *vt, स्थिरांक अक्षर *bytes, माप_प्रकार len);
शून्य vterm_ढकला_output_vsprintf(VTerm *vt, स्थिरांक अक्षर *format, बहु_सूची args);
शून्य vterm_ढकला_output_sprintf(VTerm *vt, स्थिरांक अक्षर *format, ...);
शून्य vterm_ढकला_output_sprintf_ctrl(VTerm *vt, अचिन्हित अक्षर ctrl, स्थिरांक अक्षर *fmt, ...);
शून्य vterm_ढकला_output_sprintf_str(VTerm *vt, अचिन्हित अक्षर ctrl, पूर्णांक term, स्थिरांक अक्षर *fmt, ...);

शून्य vterm_state_free(VTermState *state);

शून्य vterm_state_newpen(VTermState *state);
शून्य vterm_state_resetpen(VTermState *state);
शून्य vterm_state_setpen(VTermState *state, स्थिरांक दीर्घ args[], पूर्णांक argcount);
पूर्णांक  vterm_state_getpen(VTermState *state, दीर्घ args[], पूर्णांक argcount);
शून्य vterm_state_savepen(VTermState *state, पूर्णांक save);

गणना {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

शून्य vterm_state_ढकला_output_sprintf_CSI(VTermState *vts, स्थिरांक अक्षर *format, ...);

शून्य vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, अक्षर designation);

पूर्णांक vterm_unicode_width(uint32_t codepoint);
पूर्णांक vterm_unicode_is_combining(uint32_t codepoint);
पूर्णांक vterm_unicode_is_ambiguous(uint32_t codepoint);
पूर्णांक vterm_get_special_pty_type(शून्य);

#जर (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE >= 500) \
	|| defined(_ISOC99_SOURCE) || defined(_BSD_SOURCE)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#अन्यथा
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
पूर्णांक VSNPRINTF(अक्षर *str, माप_प्रकार str_m, स्थिरांक अक्षर *fmt, बहु_सूची ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
पूर्णांक SNPRINTF(अक्षर *str, माप_प्रकार str_m, स्थिरांक अक्षर *fmt, ...);
# endif
#endif
#ifndef SNPRINTF
# define SNPRINTF snprintf
#endif


#endif
