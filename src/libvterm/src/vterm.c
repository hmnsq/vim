#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_INLINES

// vim: set sw=2 :
#ಅನ್ತರ್ಭಾವ "vterm_internal.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <stdarg.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "utf8.h"

/*****************
 * API functions *
 *****************/

ಸ್ಥಿರ ಶೂನ್ಯ *default_malloc(size_t ಗಾತ್ರ_2, ಶೂನ್ಯ *allocdata ಬಳಸದ)
{
  ಶೂನ್ಯ *ptr = ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ_2);
  ಆದರೆ(ptr)
    memset(ptr, 0, ಗಾತ್ರ_2);
  ಹಿಂತಿರುಗಿ ptr;
}

ಸ್ಥಿರ ಶೂನ್ಯ default_free(ಶೂನ್ಯ *ptr, ಶೂನ್ಯ *allocdata ಬಳಸದ)
{
  ಬಿಡುಗಡೆ(ptr);
}

ಸ್ಥಿರ VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
{
  ರಚನೆ VTermBuilder builder;
  memset(&builder, 0, ಗಾತ್ರ(builder));
  builder.ಅಡ್ಡಸಾಲುಗಳು_2 = ಅಡ್ಡಸಾಲುಗಳು_2;
  builder.ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
  ಹಿಂತಿರುಗಿ vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, VTermAllocatorFunctions *funcs, ಶೂನ್ಯ *allocdata)
{
  ರಚನೆ VTermBuilder builder;
  memset(&builder, 0, ಗಾತ್ರ(builder));
  builder.ಅಡ್ಡಸಾಲುಗಳು_2 = ಅಡ್ಡಸಾಲುಗಳು_2;
  builder.ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  ಹಿಂತಿರುಗಿ vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(ಸ್ಥಿರಾಂಕ ರಚನೆ VTermBuilder *builder)
{
  ಸ್ಥಿರಾಂಕ VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->ಸ್ಮರಣೆ_ಕೊಡು)(ಗಾತ್ರ(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->ಅಡ್ಡಸಾಲುಗಳು_2 = builder->ಅಡ್ಡಸಾಲುಗಳು_2;
  vt->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = builder->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  vt->parser.state = ಸಾಮಾನ್ಯ_2;

  vt->parser.callbacks = ಖಾಲಿ;
  vt->parser.cbdata    = ಖಾಲಿ;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = ಖಾಲಿ;
  vt->outdata = ಖಾಲಿ;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  ಆದರೆ (vt->tmpbuffer == ಖಾಲಿ
      || vt->outbuffer == ಖಾಲಿ
      || vt->tmpbuffer == ಖಾಲಿ)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }

  ಹಿಂತಿರುಗಿ vt;
}

ಶೂನ್ಯ vterm_free(VTerm *vt)
{
  ಆದರೆ(vt->screen)
    vterm_screen_free(vt->screen);

  ಆದರೆ(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

ಆಂತರಿಕ ಶೂನ್ಯ *vterm_allocator_malloc(VTerm *vt, size_t ಗಾತ್ರ_2)
{
  ಹಿಂತಿರುಗಿ (*vt->allocator->ಸ್ಮರಣೆ_ಕೊಡು)(ಗಾತ್ರ_2, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
ಆಂತರಿಕ ಶೂನ್ಯ vterm_allocator_free(VTerm *vt, ಶೂನ್ಯ *ptr)
{
  ಆದರೆ (ptr)
    (*vt->allocator->ಬಿಡುಗಡೆ)(ptr, vt->allocdata);
}

ಶೂನ್ಯ vterm_get_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ VTerm *vt, ಪೂರ್ಣಾಂಕ *rowsp, ಪೂರ್ಣಾಂಕ *colsp)
{
  ಆದರೆ(rowsp)
    *rowsp = vt->ಅಡ್ಡಸಾಲುಗಳು_2;
  ಆದರೆ(colsp)
    *colsp = vt->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
}

ಶೂನ್ಯ vterm_ಗಣ_ಗಾತ್ರ(VTerm *vt, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
{
  ಆದರೆ(ಅಡ್ಡಸಾಲುಗಳು_2 < 1 || ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ < 1)
    ಹಿಂತಿರುಗಿ;

  vt->ಅಡ್ಡಸಾಲುಗಳು_2 = ಅಡ್ಡಸಾಲುಗಳು_2;
  vt->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  ಆದರೆ(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, vt->parser.cbdata);
}

ಪೂರ್ಣಾಂಕ vterm_get_utf8(ಸ್ಥಿರಾಂಕ VTerm *vt)
{
  ಹಿಂತಿರುಗಿ vt->ಬಹುಲಕ.utf8;
}

ಶೂನ್ಯ vterm_ಗಣ_utf8(VTerm *vt, ಪೂರ್ಣಾಂಕ is_utf8)
{
  vt->ಬಹುಲಕ.utf8 = is_utf8;
}

ಶೂನ್ಯ vterm_output_ಗಣ_callback(VTerm *vt, VTermOutputCallback *func, ಶೂನ್ಯ *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

ಆಂತರಿಕ ಶೂನ್ಯ vterm_ತಳ್ಳು_output_ಬೈಟುಗಳು(VTerm *vt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, size_t len)
{
  ಆದರೆ(vt->outfunc) {
    (vt->outfunc)(ಬೈಟುಗಳು, len, vt->outdata);
    ಹಿಂತಿರುಗಿ;
  }

  ಆದರೆ(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    ಹಿಂತಿರುಗಿ;
  }

  ಸ್ಮರಣೆ_ನಕಲು(vt->outbuffer + vt->outbuffer_cur, ಬೈಟುಗಳು, len);
  vt->outbuffer_cur += len;
}

ಆಂತರಿಕ ಶೂನ್ಯ vterm_ತಳ್ಳು_output_vsprintf(VTerm *vt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, va_list ವಾದಗಳು)
{
  size_t len;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, ವಾದಗಳು);
#ಇಲ್ಲದಿದ್ದರೆ
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, ವಾದಗಳು);
  ಆದರೆ (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#ಆದರೆ_ಅಂತ್ಯ
  vterm_ತಳ್ಳು_output_ಬೈಟುಗಳು(vt, vt->tmpbuffer, len);
}

ಆಂತರಿಕ ಶೂನ್ಯ vterm_ತಳ್ಳು_output_sprintf(VTerm *vt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
  va_list ವಾದಗಳು;
  va_start(ವಾದಗಳು, format);
  vterm_ತಳ್ಳು_output_vsprintf(vt, format, ವಾದಗಳು);
  va_end(ವಾದಗಳು);
}

ಆಂತರಿಕ ಶೂನ್ಯ vterm_ತಳ್ಳು_output_sprintf_ctrl(VTerm *vt, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ctrl, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  size_t cur;

  ಆದರೆ(ctrl >= 0x80 && !vt->ಬಹುಲಕ.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  ಇಲ್ಲದಿದ್ದರೆ
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  ಆದರೆ(cur >= vt->tmpbuffer_len)
    ಹಿಂತಿರುಗಿ;
  vterm_ತಳ್ಳು_output_ಬೈಟುಗಳು(vt, vt->tmpbuffer, cur);

  va_list ವಾದಗಳು;
  va_start(ವಾದಗಳು, fmt);
  vterm_ತಳ್ಳು_output_vsprintf(vt, fmt, ವಾದಗಳು);
  va_end(ವಾದಗಳು);
}

ಆಂತರಿಕ ಶೂನ್ಯ vterm_ತಳ್ಳು_output_sprintf_str(VTerm *vt, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ctrl, ಪೂರ್ಣಾಂಕ ಪದ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  size_t cur;

  ಆದರೆ(ctrl) {
    ಆದರೆ(ctrl >= 0x80 && !vt->ಬಹುಲಕ.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    ಇಲ್ಲದಿದ್ದರೆ
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    ಆದರೆ(cur >= vt->tmpbuffer_len)
      ಹಿಂತಿರುಗಿ;
    vterm_ತಳ್ಳು_output_ಬೈಟುಗಳು(vt, vt->tmpbuffer, cur);
  }

  va_list ವಾದಗಳು;
  va_start(ವಾದಗಳು, fmt);
  vterm_ತಳ್ಳು_output_vsprintf(vt, fmt, ವಾದಗಳು);
  va_end(ವಾದಗಳು);

  ಆದರೆ(ಪದ) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->ಬಹುಲಕ.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    ಆದರೆ(cur >= vt->tmpbuffer_len)
      ಹಿಂತಿರುಗಿ;
    vterm_ತಳ್ಳು_output_ಬೈಟುಗಳು(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_ಬಫರ್_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ VTerm *vt)
{
  ಹಿಂತಿರುಗಿ vt->outbuffer_len;
}

size_t vterm_output_get_ಬಫರ್_current(ಸ್ಥಿರಾಂಕ VTerm *vt)
{
  ಹಿಂತಿರುಗಿ vt->outbuffer_cur;
}

size_t vterm_output_get_ಬಫರ್_ಉಳಿದ(ಸ್ಥಿರಾಂಕ VTerm *vt)
{
  ಹಿಂತಿರುಗಿ vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, ಅಕ್ಷರ *ಬಫರ್, size_t len)
{
  ಆದರೆ(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  ಸ್ಮರಣೆ_ನಕಲು(ಬಫರ್, vt->outbuffer, len);

  ಆದರೆ(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  ಹಿಂತಿರುಗಿ len;
}

VTerm_ಮೌಲ್ಯ_Type vterm_get_attr_type(VTermAttr attr)
{
  ಆಯ್ಕೆ(attr) {
    ಸ್ಥಿತಿ VTERM_ATTR_BOLD:       ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_BOOL;
    ಸ್ಥಿತಿ VTERM_ATTR_UNDERLINE:  ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_INT;
    ಸ್ಥಿತಿ VTERM_ATTR_ITALIC:     ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_BOOL;
    ಸ್ಥಿತಿ VTERM_ATTR_BLINK:      ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_BOOL;
    ಸ್ಥಿತಿ VTERM_ATTR_ಹಿಮ್ಮುಖ:    ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_BOOL;
    ಸ್ಥಿತಿ VTERM_ATTR_CONCEAL:    ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_BOOL;
    ಸ್ಥಿತಿ VTERM_ATTR_STRIKE:     ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_BOOL;
    ಸ್ಥಿತಿ VTERM_ATTR_FONT:       ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_INT;
    ಸ್ಥಿತಿ VTERM_ATTR_FOREGROUND: ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_COLOR;
    ಸ್ಥಿತಿ VTERM_ATTR_BACKGROUND: ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_COLOR;
    ಸ್ಥಿತಿ VTERM_ATTR_SMALL:      ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_BOOL;
    ಸ್ಥಿತಿ VTERM_ATTR_BASELINE:   ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_INT;

    ಸ್ಥಿತಿ VTERM_N_ATTRS: ಹಿಂತಿರುಗಿ 0;
  }
  ಹಿಂತಿರುಗಿ 0; /* UNREACHABLE */
}

VTerm_ಮೌಲ್ಯ_Type vterm_get_prop_type(VTermProp prop)
{
  ಆಯ್ಕೆ(prop) {
    ಸ್ಥಿತಿ VTERM_PROP_CURSORVISIBLE: ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_BOOL;
    ಸ್ಥಿತಿ VTERM_PROP_CURSORBLINK:   ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_BOOL;
    ಸ್ಥಿತಿ VTERM_PROP_ALTSCREEN:     ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_BOOL;
    ಸ್ಥಿತಿ VTERM_PROP_TITLE:         ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_STRING;
    ಸ್ಥಿತಿ VTERM_PROP_ICONNAME:      ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_STRING;
    ಸ್ಥಿತಿ VTERM_PROP_ಹಿಮ್ಮುಖ:       ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_BOOL;
    ಸ್ಥಿತಿ VTERM_PROP_CURSORSHAPE:   ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_INT;
    ಸ್ಥಿತಿ VTERM_PROP_MOUSE:         ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_INT;
    ಸ್ಥಿತಿ VTERM_PROP_FOCUSREPORT:   ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_BOOL;
    ಸ್ಥಿತಿ VTERM_PROP_CURSORCOLOR:   ಹಿಂತಿರುಗಿ VTERM_VALUETYPE_STRING;

    ಸ್ಥಿತಿ VTERM_N_PROPS: ಹಿಂತಿರುಗಿ 0;
  }
  ಹಿಂತಿರುಗಿ 0; /* UNREACHABLE */
}

ಶೂನ್ಯ vterm_scroll_rect(VTermRect rect,
    ಪೂರ್ಣಾಂಕ downward,
    ಪೂರ್ಣಾಂಕ rightward,
    ಪೂರ್ಣಾಂಕ (*moverect)(VTermRect src, VTermRect dest, ಶೂನ್ಯ *user),
    ಪೂರ್ಣಾಂಕ (*eraserect)(VTermRect rect, ಪೂರ್ಣಾಂಕ selective, ಶೂನ್ಯ *user),
    ಶೂನ್ಯ *user)
{
  VTermRect src;
  VTermRect dest;

  ಆದರೆ(ನಿರಪೇಕ್ಷ(downward)  >= rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು - rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು ||
     ನಿರಪೇಕ್ಷ(rightward) >= rect.ಕೊನೆ_ಲಂಬಸಾಲು - rect.ಆರಂಭ_ಲಂಬಸಾಲು) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    ಹಿಂತಿರುಗಿ;
  }

  ಆದರೆ(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.ಆರಂಭ_ಲಂಬಸಾಲು = rect.ಆರಂಭ_ಲಂಬಸಾಲು;
    dest.ಕೊನೆ_ಲಂಬಸಾಲು   = rect.ಕೊನೆ_ಲಂಬಸಾಲು   - rightward;
    src.ಆರಂಭ_ಲಂಬಸಾಲು  = rect.ಆರಂಭ_ಲಂಬಸಾಲು + rightward;
    src.ಕೊನೆ_ಲಂಬಸಾಲು    = rect.ಕೊನೆ_ಲಂಬಸಾಲು;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    ಪೂರ್ಣಾಂಕ leftward = -rightward;
    dest.ಆರಂಭ_ಲಂಬಸಾಲು = rect.ಆರಂಭ_ಲಂಬಸಾಲು + leftward;
    dest.ಕೊನೆ_ಲಂಬಸಾಲು   = rect.ಕೊನೆ_ಲಂಬಸಾಲು;
    src.ಆರಂಭ_ಲಂಬಸಾಲು  = rect.ಆರಂಭ_ಲಂಬಸಾಲು;
    src.ಕೊನೆ_ಲಂಬಸಾಲು    = rect.ಕೊನೆ_ಲಂಬಸಾಲು - leftward;
  }

  ಆದರೆ(downward >= 0) {
    dest.ಆರಂಭ_ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು;
    dest.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   - downward;
    src.ಆರಂಭ_ಅಡ್ಡಸಾಲು  = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು + downward;
    src.ಕೊನೆ_ಅಡ್ಡಸಾಲು    = rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಪೂರ್ಣಾಂಕ upward = -downward;
    dest.ಆರಂಭ_ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು + upward;
    dest.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು;
    src.ಆರಂಭ_ಅಡ್ಡಸಾಲು  = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು;
    src.ಕೊನೆ_ಅಡ್ಡಸಾಲು    = rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು - upward;
  }

  ಆದರೆ(moverect)
    (*moverect)(dest, src, user);

  ಆದರೆ(downward > 0)
    rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು - downward;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(downward < 0)
    rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು - downward;

  ಆದರೆ(rightward > 0)
    rect.ಆರಂಭ_ಲಂಬಸಾಲು = rect.ಕೊನೆ_ಲಂಬಸಾಲು - rightward;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rightward < 0)
    rect.ಕೊನೆ_ಲಂಬಸಾಲು = rect.ಆರಂಭ_ಲಂಬಸಾಲು - rightward;

  (*eraserect)(rect, 0, user);
}

ಶೂನ್ಯ vterm_copy_cells(VTermRect dest,
    VTermRect src,
    ಶೂನ್ಯ (*copycell)(VTermPos dest, VTermPos src, ಶೂನ್ಯ *user),
    ಶೂನ್ಯ *user)
{
  ಪೂರ್ಣಾಂಕ downward  = src.ಆರಂಭ_ಅಡ್ಡಸಾಲು - dest.ಆರಂಭ_ಅಡ್ಡಸಾಲು;
  ಪೂರ್ಣಾಂಕ rightward = src.ಆರಂಭ_ಲಂಬಸಾಲು - dest.ಆರಂಭ_ಲಂಬಸಾಲು;

  ಪೂರ್ಣಾಂಕ init_ಅಡ್ಡಸಾಲು, test_ಅಡ್ಡಸಾಲು, init_ಲಂಬಸಾಲು, test_ಲಂಬಸಾಲು;
  ಪೂರ್ಣಾಂಕ inc_ಅಡ್ಡಸಾಲು, inc_ಲಂಬಸಾಲು;

  ಆದರೆ(downward < 0) {
    init_ಅಡ್ಡಸಾಲು = dest.ಕೊನೆ_ಅಡ್ಡಸಾಲು - 1;
    test_ಅಡ್ಡಸಾಲು = dest.ಆರಂಭ_ಅಡ್ಡಸಾಲು - 1;
    inc_ಅಡ್ಡಸಾಲು = -1;
  }
  ಇಲ್ಲದಿದ್ದರೆ /* downward >= 0 */ {
    init_ಅಡ್ಡಸಾಲು = dest.ಆರಂಭ_ಅಡ್ಡಸಾಲು;
    test_ಅಡ್ಡಸಾಲು = dest.ಕೊನೆ_ಅಡ್ಡಸಾಲು;
    inc_ಅಡ್ಡಸಾಲು = +1;
  }

  ಆದರೆ(rightward < 0) {
    init_ಲಂಬಸಾಲು = dest.ಕೊನೆ_ಲಂಬಸಾಲು - 1;
    test_ಲಂಬಸಾಲು = dest.ಆರಂಭ_ಲಂಬಸಾಲು - 1;
    inc_ಲಂಬಸಾಲು = -1;
  }
  ಇಲ್ಲದಿದ್ದರೆ /* rightward >= 0 */ {
    init_ಲಂಬಸಾಲು = dest.ಆರಂಭ_ಲಂಬಸಾಲು;
    test_ಲಂಬಸಾಲು = dest.ಕೊನೆ_ಲಂಬಸಾಲು;
    inc_ಲಂಬಸಾಲು = +1;
  }

  VTermPos pos;
  ಗಾಗಿ(pos.ಅಡ್ಡಸಾಲು = init_ಅಡ್ಡಸಾಲು; pos.ಅಡ್ಡಸಾಲು != test_ಅಡ್ಡಸಾಲು; pos.ಅಡ್ಡಸಾಲು += inc_ಅಡ್ಡಸಾಲು)
    ಗಾಗಿ(pos.ಲಂಬಸಾಲು = init_ಲಂಬಸಾಲು; pos.ಲಂಬಸಾಲು != test_ಲಂಬಸಾಲು; pos.ಲಂಬಸಾಲು += inc_ಲಂಬಸಾಲು) {
      VTermPos srcpos;
      srcpos.ಅಡ್ಡಸಾಲು = pos.ಅಡ್ಡಸಾಲು + downward;
      srcpos.ಲಂಬಸಾಲು = pos.ಲಂಬಸಾಲು + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

ಶೂನ್ಯ vterm_check_ಆವೃತ್ತಿ(ಪೂರ್ಣಾಂಕ major, ಪೂರ್ಣಾಂಕ minor)
{
  ಆದರೆ(major != VTERM_ಆವೃತ್ತಿ_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_ಆವೃತ್ತಿ_MAJOR);
    ನಿರ್ಗಮಿಸು(1);
  }

  ಆದರೆ(minor > VTERM_ಆವೃತ್ತಿ_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_ಆವೃತ್ತಿ_MINOR);
    ನಿರ್ಗಮಿಸು(1);
  }

  // Happy
}
