#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __VTERM_INTERNAL_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __VTERM_INTERNAL_H__

#ಅನ್ತರ್ಭಾವ "vterm.h"

#ಅನ್ತರ್ಭಾವ <stdarg.h>

#ಆದರೆ defined(__GNUC__) && !defined(__MINGW32__)
# define ಆಂತರಿಕ __attribute__((visibility("internal")))
#ಇಲ್ಲದಿದ್ದರೆ
# define ಆಂತರಿಕ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__GNUC__) || defined(__MINGW32__)
# define ಬಳಸದ __attribute__((ಬಳಸದ_2))
#ಇಲ್ಲದಿದ್ದರೆ
# define ಬಳಸದ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
# define DEBUG_LOG(s) fprintf(stderr, s)
# define DEBUG_LOG1(s, a) fprintf(stderr, s, a)
# define DEBUG_LOG2(s, a, b) fprintf(stderr, s, a, b)
# define DEBUG_LOG3(s, a, b, c) fprintf(stderr, s, a, b, c)
#ಇಲ್ಲದಿದ್ದರೆ
# define DEBUG_LOG(s)
# define DEBUG_LOG1(s, a)
# define DEBUG_LOG2(s, a, b)
# define DEBUG_LOG3(s, a, b, c)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು ESC_S "\x1b"

#ವ್ಯಾಖ್ಯಾನಿಸು INTERMED_MAX 16

#ವ್ಯಾಖ್ಯಾನಿಸು CSI_ವಾದಗಳು_MAX 16
#ವ್ಯಾಖ್ಯಾನಿಸು CSI_LEADER_MAX 16

#ವ್ಯಾಖ್ಯಾನಿಸು BUFIDX_PRIMARY   0
#ವ್ಯಾಖ್ಯಾನಿಸು BUFIDX_ALTSCREEN 1

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ VTermEncoding VTermEncoding;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  VTermEncoding *enc;

  // This size should be increased if required by other stateful encodings
  ಅಕ್ಷರ           ದತ್ತಾಂಶ[4*ಗಾತ್ರ(uint32_t)];
} VTermEncodingInstance;

ರಚನೆ VTermPen
{
  VTermColor fg;
  VTermColor bg;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ bold:1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ underline:2;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ italic:1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ blink:1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಹಿಮ್ಮುಖ:1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ conceal:1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ strike:1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ font:4; /* To store 0-9 */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ small:1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ baseline:2;
};

ರಚನೆ VTermState
{
  VTerm *vt;

  ಸ್ಥಿರಾಂಕ VTermStateCallbacks *callbacks;
  ಶೂನ್ಯ *cbdata;
  ಪೂರ್ಣಾಂಕ callbacks_has_premove;

  ಸ್ಥಿರಾಂಕ VTermStateFallbacks *fallbacks;
  ಶೂನ್ಯ *fbdata;

  ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2;
  ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  /* Current cursor position */
  VTermPos pos;

  ಪೂರ್ಣಾಂಕ at_phantom; /* True if we're on the "81st" phantom column to defer a wraparound */

  ಪೂರ್ಣಾಂಕ scrollregion_ತುದಿ;
  ಪೂರ್ಣಾಂಕ scrollregion_bottom; /* -1 means unbounded */
#ವ್ಯಾಖ್ಯಾನಿಸು SCROLLREGION_BOTTOM(state) ((state)->scrollregion_bottom > -1 ? (state)->scrollregion_bottom : (state)->ಅಡ್ಡಸಾಲುಗಳು_2)
  ಪೂರ್ಣಾಂಕ scrollregion_ಎಡ;
#ವ್ಯಾಖ್ಯಾನಿಸು SCROLLREGION_ಎಡ(state)  ((state)->ಬಹುಲಕ.leftrightmargin ? (state)->scrollregion_ಎಡ : 0)
  ಪೂರ್ಣಾಂಕ scrollregion_ಬಲ; /* -1 means unbounded */
#ವ್ಯಾಖ್ಯಾನಿಸು SCROLLREGION_ಬಲ(state) ((state)->ಬಹುಲಕ.leftrightmargin && (state)->scrollregion_ಬಲ > -1 ? (state)->scrollregion_ಬಲ : (state)->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)

  /* Bitvector of tab stops */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *tabstops;

  /* Primary and Altscreen; lineinfos[1] is lazily allocated as needed */
  VTerm_ಸಾಲು_Info *lineinfos[2];

  /* lineinfo will == lineinfos[0] or lineinfos[1], depending on altscreen */
  VTerm_ಸಾಲು_Info *lineinfo;
#ವ್ಯಾಖ್ಯಾನಿಸು ROWWIDTH(state,ಅಡ್ಡಸಾಲು) ((state)->lineinfo[(ಅಡ್ಡಸಾಲು)].doublewidth ? ((state)->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ / 2) : (state)->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
#ವ್ಯಾಖ್ಯಾನಿಸು THISROWWIDTH(state) ROWWIDTH(state, (state)->pos.ಅಡ್ಡಸಾಲು)

  /* Mouse state */
  ಪೂರ್ಣಾಂಕ mouse_ಲಂಬಸಾಲು, mouse_ಅಡ್ಡಸಾಲು;
  ಪೂರ್ಣಾಂಕ mouse_buttons;
  ಪೂರ್ಣಾಂಕ mouse_flags;

  ಎಣಿಕೆ { MOUSE_X10, MOUSE_UTF8, MOUSE_SGR, MOUSE_RXVT } mouse_protocol;

  /* Last glyph output, for Unicode recombining purposes */
  uint32_t *combine_chars;
  size_t combine_chars_ಗಾತ್ರ; // Number of ELEMENTS in the above
  ಪೂರ್ಣಾಂಕ combine_ಅಗಲ; // The width of the glyph above
  VTermPos combine_pos;   // Position before movement

  ರಚನೆ {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ keypad:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cursor:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ autowrap:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸೇರಿಸು:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ newline:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cursor_visible:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cursor_blink:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cursor_shape:2;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ alt_screen:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ origin:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ screen:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ leftrightmargin:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ bracketpaste:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ report_focus:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ modify_other_ಕೀಲಿಗಳು:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ kitty_keyboard:1;
  } ಬಹುಲಕ;

  VTermEncodingInstance encoding[4], encoding_utf8;
  ಪೂರ್ಣಾಂಕ gl_ಗಣ, gr_ಗಣ, gsingle_ಗಣ;

  ರಚನೆ VTermPen pen;

  VTermColor default_fg;
  VTermColor default_bg;
  VTermColor colors[16]; // Store the 8 ANSI and the 8 ANSI high-brights only

  ಪೂರ್ಣಾಂಕ bold_is_highbright;

  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ protected_cell : 1;

  /* Saved state under DEC mode 1048/1049 */
  ರಚನೆ {
    VTermPos pos;
    ರಚನೆ VTermPen pen;

    ರಚನೆ {
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cursor_visible:1;
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cursor_blink:1;
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cursor_shape:2;
    } ಬಹುಲಕ;
  } saved;

  /* Temporary state for DECRQSS parsing */
  ಒಕ್ಕೂಟ {
    ಅಕ್ಷರ decrqss[4];
    ರಚನೆ {
      uint16_t ಮುಖವಾಡ;
      ಎಣಿಕೆ {
        SELECTION_INITIAL,
        SELECTION_SELECTED,
        SELECTION_QUERY,
        SELECTION_ಗಣ_INITIAL,
        SELECTION_ಗಣ,
        SELECTION_INVALID,
      } state : 8;
      uint32_t recvpartial;
      uint32_t sendpartial;
    } selection;
  } tmp;

  ರಚನೆ {
    ಸ್ಥಿರಾಂಕ VTermSelectionCallbacks *callbacks;
    ಶೂನ್ಯ *user;
    ಅಕ್ಷರ *ಬಫರ್;
    size_t buflen;
  } selection;
};

ರಚನೆ VTerm
{
  ಸ್ಥಿರಾಂಕ VTermAllocatorFunctions *allocator;
  ಶೂನ್ಯ *allocdata;

  ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2;
  ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  ರಚನೆ {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ utf8:1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ctrl8bit:1;
  } ಬಹುಲಕ;

  ರಚನೆ {
    ಎಣಿಕೆ VTermParserState {
      ಸಾಮಾನ್ಯ_2,
      CSI_LEADER,
      CSI_ವಾದಗಳು,
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

    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಒಳ_esc : 1;

    ಪೂರ್ಣಾಂಕ intermedlen;
    ಅಕ್ಷರ intermed[INTERMED_MAX];

    ಒಕ್ಕೂಟ {
      ರಚನೆ {
        ಪೂರ್ಣಾಂಕ leaderlen;
        ಅಕ್ಷರ leader[CSI_LEADER_MAX];

        ಪೂರ್ಣಾಂಕ argi;
        ದೀರ್ಘ ವಾದಗಳು[CSI_ವಾದಗಳು_MAX];
      } csi;
      ರಚನೆ {
        ಪೂರ್ಣಾಂಕ command;
      } osc;
      ರಚನೆ {
        ಪೂರ್ಣಾಂಕ commandlen;
        ಅಕ್ಷರ command[CSI_LEADER_MAX];
      } dcs;
    } v;

    ಸ್ಥಿರಾಂಕ VTermParserCallbacks *callbacks;
    ಶೂನ್ಯ *cbdata;

    ಪೂರ್ಣಾಂಕ string_initial;

    ಪೂರ್ಣಾಂಕ emit_nul;
  } parser;

  /* len == malloc()ed size; cur == number of valid bytes */

  VTermOutputCallback *outfunc;
  ಶೂನ್ಯ                *outdata;

  ಅಕ್ಷರ  *outbuffer;
  size_t outbuffer_len;
  size_t outbuffer_cur;

  ಅಕ್ಷರ  *tmpbuffer;
  size_t tmpbuffer_len;

  VTermState *state;
  VTermScreen *screen;

  ಪೂರ್ಣಾಂಕ ಒಳ_backspace;
};

ರಚನೆ VTermEncoding {
  ಶೂನ್ಯ (*init) (VTermEncoding *enc, ಶೂನ್ಯ *ದತ್ತಾಂಶ);
  ಶೂನ್ಯ (*decode)(VTermEncoding *enc, ಶೂನ್ಯ *ದತ್ತಾಂಶ,
                 uint32_t cp[], ಪೂರ್ಣಾಂಕ *cpi, ಪೂರ್ಣಾಂಕ cplen,
                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ಬೈಟುಗಳು[], size_t *pos, size_t len);
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  ENC_UTF8,
  ENC_SINGLE_94
} VTermEncodingType;

ಶೂನ್ಯ *vterm_allocator_malloc(VTerm *vt, size_t ಗಾತ್ರ_2);
ಶೂನ್ಯ  vterm_allocator_free(VTerm *vt, ಶೂನ್ಯ *ptr);

ಶೂನ್ಯ vterm_ತಳ್ಳು_output_ಬೈಟುಗಳು(VTerm *vt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, size_t len);
ಶೂನ್ಯ vterm_ತಳ್ಳು_output_vsprintf(VTerm *vt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, va_list ವಾದಗಳು);
ಶೂನ್ಯ vterm_ತಳ್ಳು_output_sprintf(VTerm *vt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...);
ಶೂನ್ಯ vterm_ತಳ್ಳು_output_sprintf_ctrl(VTerm *vt, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ctrl, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...);
ಶೂನ್ಯ vterm_ತಳ್ಳು_output_sprintf_str(VTerm *vt, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ctrl, ಪೂರ್ಣಾಂಕ ಪದ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...);

ಶೂನ್ಯ vterm_state_free(VTermState *state);

ಶೂನ್ಯ vterm_state_newpen(VTermState *state);
ಶೂನ್ಯ vterm_state_resetpen(VTermState *state);
ಶೂನ್ಯ vterm_state_setpen(VTermState *state, ಸ್ಥಿರಾಂಕ ದೀರ್ಘ ವಾದಗಳು[], ಪೂರ್ಣಾಂಕ argcount);
ಪೂರ್ಣಾಂಕ  vterm_state_getpen(VTermState *state, ದೀರ್ಘ ವಾದಗಳು[], ಪೂರ್ಣಾಂಕ argcount);
ಶೂನ್ಯ vterm_state_savepen(VTermState *state, ಪೂರ್ಣಾಂಕ save);

ಎಣಿಕೆ {
  C1_SS3 = 0x8f,
  C1_DCS = 0x90,
  C1_CSI = 0x9b,
  C1_ST  = 0x9c,
  C1_OSC = 0x9d,
};

ಶೂನ್ಯ vterm_state_ತಳ್ಳು_output_sprintf_CSI(VTermState *vts, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...);

ಶೂನ್ಯ vterm_screen_free(VTermScreen *screen);

VTermEncoding *vterm_lookup_encoding(VTermEncodingType type, ಅಕ್ಷರ designation);

ಪೂರ್ಣಾಂಕ vterm_unicode_ಅಗಲ(uint32_t codepoint);
ಪೂರ್ಣಾಂಕ vterm_unicode_is_combining(uint32_t codepoint);
ಪೂರ್ಣಾಂಕ vterm_unicode_is_ambiguous(uint32_t codepoint);
ಪೂರ್ಣಾಂಕ vterm_get_special_pty_type(ಶೂನ್ಯ);

#ಆದರೆ (defined(XOPEN_ಮೂಲ) && XOPEN_ಮೂಲ >= 500) \
	|| defined(ISOC99_ಮೂಲ) || defined(BSD_ಮೂಲ)
# undef VSNPRINTF
# define VSNPRINTF vsnprintf
# undef SNPRINTF
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef VSNPRINTF
// Use a provided vsnprintf() function.
ಪೂರ್ಣಾಂಕ VSNPRINTF(ಅಕ್ಷರ *str, size_t str_m, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, va_list ap);
# endif
# ifdef SNPRINTF
// Use a provided snprintf() function.
ಪೂರ್ಣಾಂಕ SNPRINTF(ಅಕ್ಷರ *str, size_t str_m, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...);
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SNPRINTF
# define SNPRINTF snprintf
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ_ಅಂತ್ಯ
