#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_INLINES

// vim: set sw=2 :
#ਅਨ੍ਤਰ੍ਭਾਵ "vterm_internal.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

/*****************
 * API functions *
 *****************/

ਸਥਿਰ ਖਾਲੀ *default_malloc(size_t ਮਾਪ, ਖਾਲੀ *allocdata ਅਣਵਰਤਿਆ)
{
  ਖਾਲੀ *ptr = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਮਾਪ);
  ਜੇ(ptr)
    memset(ptr, 0, ਮਾਪ);
  ਵਾਪਸ ptr;
}

ਸਥਿਰ ਖਾਲੀ default_free(ਖਾਲੀ *ptr, ਖਾਲੀ *allocdata ਅਣਵਰਤਿਆ)
{
  ਮੁਕਤ(ptr);
}

ਸਥਿਰ VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ)
{
  ਬਣਤਰ VTermBuilder builder;
  memset(&builder, 0, ਆਕਾਰ(builder));
  builder.ਪੰਕਤੀਆਂ_2 = ਪੰਕਤੀਆਂ_2;
  builder.ਖਾਨੇ_ਗਿਣਤੀ = ਖਾਨੇ_ਗਿਣਤੀ;
  ਵਾਪਸ vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, VTermAllocatorFunctions *funcs, ਖਾਲੀ *allocdata)
{
  ਬਣਤਰ VTermBuilder builder;
  memset(&builder, 0, ਆਕਾਰ(builder));
  builder.ਪੰਕਤੀਆਂ_2 = ਪੰਕਤੀਆਂ_2;
  builder.ਖਾਨੇ_ਗਿਣਤੀ = ਖਾਨੇ_ਗਿਣਤੀ;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  ਵਾਪਸ vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(ਅਚਲ ਬਣਤਰ VTermBuilder *builder)
{
  ਅਚਲ VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->ਸਮ੍ਰਿਤੀ_ਦਿਓ)(ਆਕਾਰ(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->ਪੰਕਤੀਆਂ_2 = builder->ਪੰਕਤੀਆਂ_2;
  vt->ਖਾਨੇ_ਗਿਣਤੀ = builder->ਖਾਨੇ_ਗਿਣਤੀ;

  vt->parser.state = ਸਾਧਾਰਨ_2;

  vt->parser.callbacks = ਨਿਰਾ;
  vt->parser.cbdata    = ਨਿਰਾ;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = ਨਿਰਾ;
  vt->outdata = ਨਿਰਾ;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  ਜੇ (vt->tmpbuffer == ਨਿਰਾ
      || vt->outbuffer == ਨਿਰਾ
      || vt->tmpbuffer == ਨਿਰਾ)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    ਵਾਪਸ ਨਿਰਾ;
  }

  ਵਾਪਸ vt;
}

ਖਾਲੀ vterm_free(VTerm *vt)
{
  ਜੇ(vt->screen)
    vterm_screen_free(vt->screen);

  ਜੇ(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

ਅੰਦਰੂਨੀ ਖਾਲੀ *vterm_allocator_malloc(VTerm *vt, size_t ਮਾਪ)
{
  ਵਾਪਸ (*vt->allocator->ਸਮ੍ਰਿਤੀ_ਦਿਓ)(ਮਾਪ, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
ਅੰਦਰੂਨੀ ਖਾਲੀ vterm_allocator_free(VTerm *vt, ਖਾਲੀ *ptr)
{
  ਜੇ (ptr)
    (*vt->allocator->ਮੁਕਤ)(ptr, vt->allocdata);
}

ਖਾਲੀ vterm_get_ਮਾਪ(ਅਚਲ VTerm *vt, ਪੂਰਨ *rowsp, ਪੂਰਨ *colsp)
{
  ਜੇ(rowsp)
    *rowsp = vt->ਪੰਕਤੀਆਂ_2;
  ਜੇ(colsp)
    *colsp = vt->ਖਾਨੇ_ਗਿਣਤੀ;
}

ਖਾਲੀ vterm_ਸੈੱਟ_ਮਾਪ(VTerm *vt, ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ)
{
  ਜੇ(ਪੰਕਤੀਆਂ_2 < 1 || ਖਾਨੇ_ਗਿਣਤੀ < 1)
    ਵਾਪਸ;

  vt->ਪੰਕਤੀਆਂ_2 = ਪੰਕਤੀਆਂ_2;
  vt->ਖਾਨੇ_ਗਿਣਤੀ = ਖਾਨੇ_ਗਿਣਤੀ;

  ਜੇ(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ, vt->parser.cbdata);
}

ਪੂਰਨ vterm_get_utf8(ਅਚਲ VTerm *vt)
{
  ਵਾਪਸ vt->ਬਹੁਲਕ.utf8;
}

ਖਾਲੀ vterm_ਸੈੱਟ_utf8(VTerm *vt, ਪੂਰਨ is_utf8)
{
  vt->ਬਹੁਲਕ.utf8 = is_utf8;
}

ਖਾਲੀ vterm_output_ਸੈੱਟ_callback(VTerm *vt, VTermOutputCallback *func, ਖਾਲੀ *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

ਅੰਦਰੂਨੀ ਖਾਲੀ vterm_ਧੱਕੋ_output_ਬਾਈਟਾਂ(VTerm *vt, ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, size_t len)
{
  ਜੇ(vt->outfunc) {
    (vt->outfunc)(ਬਾਈਟਾਂ, len, vt->outdata);
    ਵਾਪਸ;
  }

  ਜੇ(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    ਵਾਪਸ;
  }

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(vt->outbuffer + vt->outbuffer_cur, ਬਾਈਟਾਂ, len);
  vt->outbuffer_cur += len;
}

ਅੰਦਰੂਨੀ ਖਾਲੀ vterm_ਧੱਕੋ_output_vsprintf(VTerm *vt, ਅਚਲ ਅੱਖਰ *format, va_list ਦਲੀਲਾਂ)
{
  size_t len;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, ਦਲੀਲਾਂ);
#ਨਹੀਂਤਾਂ
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, ਦਲੀਲਾਂ);
  ਜੇ (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#ਜੇ_ਅੰਤ
  vterm_ਧੱਕੋ_output_ਬਾਈਟਾਂ(vt, vt->tmpbuffer, len);
}

ਅੰਦਰੂਨੀ ਖਾਲੀ vterm_ਧੱਕੋ_output_sprintf(VTerm *vt, ਅਚਲ ਅੱਖਰ *format, ...)
{
  va_list ਦਲੀਲਾਂ;
  va_start(ਦਲੀਲਾਂ, format);
  vterm_ਧੱਕੋ_output_vsprintf(vt, format, ਦਲੀਲਾਂ);
  va_end(ਦਲੀਲਾਂ);
}

ਅੰਦਰੂਨੀ ਖਾਲੀ vterm_ਧੱਕੋ_output_sprintf_ctrl(VTerm *vt, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ctrl, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  size_t cur;

  ਜੇ(ctrl >= 0x80 && !vt->ਬਹੁਲਕ.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  ਨਹੀਂਤਾਂ
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  ਜੇ(cur >= vt->tmpbuffer_len)
    ਵਾਪਸ;
  vterm_ਧੱਕੋ_output_ਬਾਈਟਾਂ(vt, vt->tmpbuffer, cur);

  va_list ਦਲੀਲਾਂ;
  va_start(ਦਲੀਲਾਂ, fmt);
  vterm_ਧੱਕੋ_output_vsprintf(vt, fmt, ਦਲੀਲਾਂ);
  va_end(ਦਲੀਲਾਂ);
}

ਅੰਦਰੂਨੀ ਖਾਲੀ vterm_ਧੱਕੋ_output_sprintf_str(VTerm *vt, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ctrl, ਪੂਰਨ ਪਦ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  size_t cur;

  ਜੇ(ctrl) {
    ਜੇ(ctrl >= 0x80 && !vt->ਬਹੁਲਕ.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    ਨਹੀਂਤਾਂ
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    ਜੇ(cur >= vt->tmpbuffer_len)
      ਵਾਪਸ;
    vterm_ਧੱਕੋ_output_ਬਾਈਟਾਂ(vt, vt->tmpbuffer, cur);
  }

  va_list ਦਲੀਲਾਂ;
  va_start(ਦਲੀਲਾਂ, fmt);
  vterm_ਧੱਕੋ_output_vsprintf(vt, fmt, ਦਲੀਲਾਂ);
  va_end(ਦਲੀਲਾਂ);

  ਜੇ(ਪਦ) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->ਬਹੁਲਕ.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    ਜੇ(cur >= vt->tmpbuffer_len)
      ਵਾਪਸ;
    vterm_ਧੱਕੋ_output_ਬਾਈਟਾਂ(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_ਬਫਰ_ਮਾਪ(ਅਚਲ VTerm *vt)
{
  ਵਾਪਸ vt->outbuffer_len;
}

size_t vterm_output_get_ਬਫਰ_current(ਅਚਲ VTerm *vt)
{
  ਵਾਪਸ vt->outbuffer_cur;
}

size_t vterm_output_get_ਬਫਰ_ਬਚਿਆ(ਅਚਲ VTerm *vt)
{
  ਵਾਪਸ vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, ਅੱਖਰ *ਬਫਰ, size_t len)
{
  ਜੇ(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਬਫਰ, vt->outbuffer, len);

  ਜੇ(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  ਵਾਪਸ len;
}

VTerm_ਮੁੱਲ_Type vterm_get_attr_type(VTermAttr attr)
{
  ਚੋਣ(attr) {
    ਹਾਲਤ VTERM_ATTR_BOLD:       ਵਾਪਸ VTERM_VALUETYPE_BOOL;
    ਹਾਲਤ VTERM_ATTR_UNDERLINE:  ਵਾਪਸ VTERM_VALUETYPE_INT;
    ਹਾਲਤ VTERM_ATTR_ITALIC:     ਵਾਪਸ VTERM_VALUETYPE_BOOL;
    ਹਾਲਤ VTERM_ATTR_BLINK:      ਵਾਪਸ VTERM_VALUETYPE_BOOL;
    ਹਾਲਤ VTERM_ATTR_ਉਲਟ:    ਵਾਪਸ VTERM_VALUETYPE_BOOL;
    ਹਾਲਤ VTERM_ATTR_CONCEAL:    ਵਾਪਸ VTERM_VALUETYPE_BOOL;
    ਹਾਲਤ VTERM_ATTR_STRIKE:     ਵਾਪਸ VTERM_VALUETYPE_BOOL;
    ਹਾਲਤ VTERM_ATTR_FONT:       ਵਾਪਸ VTERM_VALUETYPE_INT;
    ਹਾਲਤ VTERM_ATTR_FOREGROUND: ਵਾਪਸ VTERM_VALUETYPE_COLOR;
    ਹਾਲਤ VTERM_ATTR_BACKGROUND: ਵਾਪਸ VTERM_VALUETYPE_COLOR;
    ਹਾਲਤ VTERM_ATTR_SMALL:      ਵਾਪਸ VTERM_VALUETYPE_BOOL;
    ਹਾਲਤ VTERM_ATTR_BASELINE:   ਵਾਪਸ VTERM_VALUETYPE_INT;

    ਹਾਲਤ VTERM_N_ATTRS: ਵਾਪਸ 0;
  }
  ਵਾਪਸ 0; /* UNREACHABLE */
}

VTerm_ਮੁੱਲ_Type vterm_get_prop_type(VTermProp prop)
{
  ਚੋਣ(prop) {
    ਹਾਲਤ VTERM_PROP_CURSORVISIBLE: ਵਾਪਸ VTERM_VALUETYPE_BOOL;
    ਹਾਲਤ VTERM_PROP_CURSORBLINK:   ਵਾਪਸ VTERM_VALUETYPE_BOOL;
    ਹਾਲਤ VTERM_PROP_ALTSCREEN:     ਵਾਪਸ VTERM_VALUETYPE_BOOL;
    ਹਾਲਤ VTERM_PROP_TITLE:         ਵਾਪਸ VTERM_VALUETYPE_STRING;
    ਹਾਲਤ VTERM_PROP_ICONNAME:      ਵਾਪਸ VTERM_VALUETYPE_STRING;
    ਹਾਲਤ VTERM_PROP_ਉਲਟ:       ਵਾਪਸ VTERM_VALUETYPE_BOOL;
    ਹਾਲਤ VTERM_PROP_CURSORSHAPE:   ਵਾਪਸ VTERM_VALUETYPE_INT;
    ਹਾਲਤ VTERM_PROP_MOUSE:         ਵਾਪਸ VTERM_VALUETYPE_INT;
    ਹਾਲਤ VTERM_PROP_FOCUSREPORT:   ਵਾਪਸ VTERM_VALUETYPE_BOOL;
    ਹਾਲਤ VTERM_PROP_CURSORCOLOR:   ਵਾਪਸ VTERM_VALUETYPE_STRING;

    ਹਾਲਤ VTERM_N_PROPS: ਵਾਪਸ 0;
  }
  ਵਾਪਸ 0; /* UNREACHABLE */
}

ਖਾਲੀ vterm_scroll_rect(VTermRect rect,
    ਪੂਰਨ downward,
    ਪੂਰਨ rightward,
    ਪੂਰਨ (*moverect)(VTermRect src, VTermRect dest, ਖਾਲੀ *user),
    ਪੂਰਨ (*eraserect)(VTermRect rect, ਪੂਰਨ selective, ਖਾਲੀ *user),
    ਖਾਲੀ *user)
{
  VTermRect src;
  VTermRect dest;

  ਜੇ(ਨਿਰਪੇਖ(downward)  >= rect.ਸਿਰਾ_ਪੰਕਤੀ - rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ ||
     ਨਿਰਪੇਖ(rightward) >= rect.ਸਿਰਾ_ਖਾਨਾ - rect.ਸ਼ੁਰੂ_ਖਾਨਾ) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    ਵਾਪਸ;
  }

  ਜੇ(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.ਸ਼ੁਰੂ_ਖਾਨਾ = rect.ਸ਼ੁਰੂ_ਖਾਨਾ;
    dest.ਸਿਰਾ_ਖਾਨਾ   = rect.ਸਿਰਾ_ਖਾਨਾ   - rightward;
    src.ਸ਼ੁਰੂ_ਖਾਨਾ  = rect.ਸ਼ੁਰੂ_ਖਾਨਾ + rightward;
    src.ਸਿਰਾ_ਖਾਨਾ    = rect.ਸਿਰਾ_ਖਾਨਾ;
  }
  ਨਹੀਂਤਾਂ {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    ਪੂਰਨ leftward = -rightward;
    dest.ਸ਼ੁਰੂ_ਖਾਨਾ = rect.ਸ਼ੁਰੂ_ਖਾਨਾ + leftward;
    dest.ਸਿਰਾ_ਖਾਨਾ   = rect.ਸਿਰਾ_ਖਾਨਾ;
    src.ਸ਼ੁਰੂ_ਖਾਨਾ  = rect.ਸ਼ੁਰੂ_ਖਾਨਾ;
    src.ਸਿਰਾ_ਖਾਨਾ    = rect.ਸਿਰਾ_ਖਾਨਾ - leftward;
  }

  ਜੇ(downward >= 0) {
    dest.ਸ਼ੁਰੂ_ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ;
    dest.ਸਿਰਾ_ਪੰਕਤੀ   = rect.ਸਿਰਾ_ਪੰਕਤੀ   - downward;
    src.ਸ਼ੁਰੂ_ਪੰਕਤੀ  = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ + downward;
    src.ਸਿਰਾ_ਪੰਕਤੀ    = rect.ਸਿਰਾ_ਪੰਕਤੀ;
  }
  ਨਹੀਂਤਾਂ {
    ਪੂਰਨ upward = -downward;
    dest.ਸ਼ੁਰੂ_ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ + upward;
    dest.ਸਿਰਾ_ਪੰਕਤੀ   = rect.ਸਿਰਾ_ਪੰਕਤੀ;
    src.ਸ਼ੁਰੂ_ਪੰਕਤੀ  = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ;
    src.ਸਿਰਾ_ਪੰਕਤੀ    = rect.ਸਿਰਾ_ਪੰਕਤੀ - upward;
  }

  ਜੇ(moverect)
    (*moverect)(dest, src, user);

  ਜੇ(downward > 0)
    rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = rect.ਸਿਰਾ_ਪੰਕਤੀ - downward;
  ਨਹੀਂਤਾਂ ਜੇ(downward < 0)
    rect.ਸਿਰਾ_ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ - downward;

  ਜੇ(rightward > 0)
    rect.ਸ਼ੁਰੂ_ਖਾਨਾ = rect.ਸਿਰਾ_ਖਾਨਾ - rightward;
  ਨਹੀਂਤਾਂ ਜੇ(rightward < 0)
    rect.ਸਿਰਾ_ਖਾਨਾ = rect.ਸ਼ੁਰੂ_ਖਾਨਾ - rightward;

  (*eraserect)(rect, 0, user);
}

ਖਾਲੀ vterm_copy_cells(VTermRect dest,
    VTermRect src,
    ਖਾਲੀ (*copycell)(VTermPos dest, VTermPos src, ਖਾਲੀ *user),
    ਖਾਲੀ *user)
{
  ਪੂਰਨ downward  = src.ਸ਼ੁਰੂ_ਪੰਕਤੀ - dest.ਸ਼ੁਰੂ_ਪੰਕਤੀ;
  ਪੂਰਨ rightward = src.ਸ਼ੁਰੂ_ਖਾਨਾ - dest.ਸ਼ੁਰੂ_ਖਾਨਾ;

  ਪੂਰਨ init_ਪੰਕਤੀ, test_ਪੰਕਤੀ, init_ਖਾਨਾ, test_ਖਾਨਾ;
  ਪੂਰਨ inc_ਪੰਕਤੀ, inc_ਖਾਨਾ;

  ਜੇ(downward < 0) {
    init_ਪੰਕਤੀ = dest.ਸਿਰਾ_ਪੰਕਤੀ - 1;
    test_ਪੰਕਤੀ = dest.ਸ਼ੁਰੂ_ਪੰਕਤੀ - 1;
    inc_ਪੰਕਤੀ = -1;
  }
  ਨਹੀਂਤਾਂ /* downward >= 0 */ {
    init_ਪੰਕਤੀ = dest.ਸ਼ੁਰੂ_ਪੰਕਤੀ;
    test_ਪੰਕਤੀ = dest.ਸਿਰਾ_ਪੰਕਤੀ;
    inc_ਪੰਕਤੀ = +1;
  }

  ਜੇ(rightward < 0) {
    init_ਖਾਨਾ = dest.ਸਿਰਾ_ਖਾਨਾ - 1;
    test_ਖਾਨਾ = dest.ਸ਼ੁਰੂ_ਖਾਨਾ - 1;
    inc_ਖਾਨਾ = -1;
  }
  ਨਹੀਂਤਾਂ /* rightward >= 0 */ {
    init_ਖਾਨਾ = dest.ਸ਼ੁਰੂ_ਖਾਨਾ;
    test_ਖਾਨਾ = dest.ਸਿਰਾ_ਖਾਨਾ;
    inc_ਖਾਨਾ = +1;
  }

  VTermPos pos;
  ਲਈ(pos.ਪੰਕਤੀ = init_ਪੰਕਤੀ; pos.ਪੰਕਤੀ != test_ਪੰਕਤੀ; pos.ਪੰਕਤੀ += inc_ਪੰਕਤੀ)
    ਲਈ(pos.ਖਾਨਾ = init_ਖਾਨਾ; pos.ਖਾਨਾ != test_ਖਾਨਾ; pos.ਖਾਨਾ += inc_ਖਾਨਾ) {
      VTermPos srcpos;
      srcpos.ਪੰਕਤੀ = pos.ਪੰਕਤੀ + downward;
      srcpos.ਖਾਨਾ = pos.ਖਾਨਾ + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

ਖਾਲੀ vterm_check_ਸੰਸਕਰਣ(ਪੂਰਨ major, ਪੂਰਨ minor)
{
  ਜੇ(major != VTERM_ਸੰਸਕਰਣ_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_ਸੰਸਕਰਣ_MAJOR);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਜੇ(minor > VTERM_ਸੰਸਕਰਣ_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_ਸੰਸਕਰਣ_MINOR);
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  // Happy
}
