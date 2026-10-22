#تعریف_کن DEFINE_INLINES

// vim: set sw=2 :
#ضمن "vterm_internal.h"

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <stdarg.h>
#ضمن <string.h>

#ضمن "utf8.h"

/*****************
 * API functions *
 *****************/

ساكن فراغ *default_malloc(size_t اندازه, فراغ *allocdata استفادهنشده)
{
  فراغ *ptr = حافظه_بده(اندازه);
  إذا(ptr)
    memset(ptr, 0, اندازه);
  أرجع ptr;
}

ساكن فراغ default_free(فراغ *ptr, فراغ *allocdata استفادهنشده)
{
  حرر(ptr);
}

ساكن VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(صحيح ردیفها_2, صحيح تعداد_ستون)
{
  بنية VTermBuilder builder;
  memset(&builder, 0, حجم(builder));
  builder.ردیفها_2 = ردیفها_2;
  builder.تعداد_ستون = تعداد_ستون;
  أرجع vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(صحيح ردیفها_2, صحيح تعداد_ستون, VTermAllocatorFunctions *funcs, فراغ *allocdata)
{
  بنية VTermBuilder builder;
  memset(&builder, 0, حجم(builder));
  builder.ردیفها_2 = ردیفها_2;
  builder.تعداد_ستون = تعداد_ستون;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  أرجع vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#تعریف_کن DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(ثابت بنية VTermBuilder *builder)
{
  ثابت VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->حافظه_بده)(حجم(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->ردیفها_2 = builder->ردیفها_2;
  vt->تعداد_ستون = builder->تعداد_ستون;

  vt->parser.state = نرمال_2;

  vt->parser.callbacks = عدم;
  vt->parser.cbdata    = عدم;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = عدم;
  vt->outdata = عدم;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  إذا (vt->tmpbuffer == عدم
      || vt->outbuffer == عدم
      || vt->tmpbuffer == عدم)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    أرجع عدم;
  }

  أرجع vt;
}

فراغ vterm_free(VTerm *vt)
{
  إذا(vt->screen)
    vterm_screen_free(vt->screen);

  إذا(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

داخلی فراغ *vterm_allocator_malloc(VTerm *vt, size_t اندازه)
{
  أرجع (*vt->allocator->حافظه_بده)(اندازه, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
داخلی فراغ vterm_allocator_free(VTerm *vt, فراغ *ptr)
{
  إذا (ptr)
    (*vt->allocator->حرر)(ptr, vt->allocdata);
}

فراغ vterm_get_اندازه(ثابت VTerm *vt, صحيح *rowsp, صحيح *colsp)
{
  إذا(rowsp)
    *rowsp = vt->ردیفها_2;
  إذا(colsp)
    *colsp = vt->تعداد_ستون;
}

فراغ vterm_مجموعه_اندازه(VTerm *vt, صحيح ردیفها_2, صحيح تعداد_ستون)
{
  إذا(ردیفها_2 < 1 || تعداد_ستون < 1)
    أرجع;

  vt->ردیفها_2 = ردیفها_2;
  vt->تعداد_ستون = تعداد_ستون;

  إذا(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(ردیفها_2, تعداد_ستون, vt->parser.cbdata);
}

صحيح vterm_get_utf8(ثابت VTerm *vt)
{
  أرجع vt->نما.utf8;
}

فراغ vterm_مجموعه_utf8(VTerm *vt, صحيح is_utf8)
{
  vt->نما.utf8 = is_utf8;
}

فراغ vterm_output_مجموعه_callback(VTerm *vt, VTermOutputCallback *func, فراغ *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

داخلی فراغ vterm_افزودن_output_بایتها(VTerm *vt, ثابت حرف *بایتها, size_t len)
{
  إذا(vt->outfunc) {
    (vt->outfunc)(بایتها, len, vt->outdata);
    أرجع;
  }

  إذا(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    أرجع;
  }

  کپی_حافظه(vt->outbuffer + vt->outbuffer_cur, بایتها, len);
  vt->outbuffer_cur += len;
}

داخلی فراغ vterm_افزودن_output_vsprintf(VTerm *vt, ثابت حرف *format, va_list آرگومانها)
{
  size_t len;
#اگر_تعریف_شده VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, آرگومانها);
#وإلا
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, آرگومانها);
  إذا (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#پایان_اگر
  vterm_افزودن_output_بایتها(vt, vt->tmpbuffer, len);
}

داخلی فراغ vterm_افزودن_output_sprintf(VTerm *vt, ثابت حرف *format, ...)
{
  va_list آرگومانها;
  va_start(آرگومانها, format);
  vterm_افزودن_output_vsprintf(vt, format, آرگومانها);
  va_end(آرگومانها);
}

داخلی فراغ vterm_افزودن_output_sprintf_ctrl(VTerm *vt, بیعلامت حرف ctrl, ثابت حرف *fmt, ...)
{
  size_t cur;

  إذا(ctrl >= 0x80 && !vt->نما.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  وإلا
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  إذا(cur >= vt->tmpbuffer_len)
    أرجع;
  vterm_افزودن_output_بایتها(vt, vt->tmpbuffer, cur);

  va_list آرگومانها;
  va_start(آرگومانها, fmt);
  vterm_افزودن_output_vsprintf(vt, fmt, آرگومانها);
  va_end(آرگومانها);
}

داخلی فراغ vterm_افزودن_output_sprintf_str(VTerm *vt, بیعلامت حرف ctrl, صحيح جمله, ثابت حرف *fmt, ...)
{
  size_t cur;

  إذا(ctrl) {
    إذا(ctrl >= 0x80 && !vt->نما.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    وإلا
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    إذا(cur >= vt->tmpbuffer_len)
      أرجع;
    vterm_افزودن_output_بایتها(vt, vt->tmpbuffer, cur);
  }

  va_list آرگومانها;
  va_start(آرگومانها, fmt);
  vterm_افزودن_output_vsprintf(vt, fmt, آرگومانها);
  va_end(آرگومانها);

  إذا(جمله) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->نما.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    إذا(cur >= vt->tmpbuffer_len)
      أرجع;
    vterm_افزودن_output_بایتها(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_میانگیر_اندازه(ثابت VTerm *vt)
{
  أرجع vt->outbuffer_len;
}

size_t vterm_output_get_میانگیر_current(ثابت VTerm *vt)
{
  أرجع vt->outbuffer_cur;
}

size_t vterm_output_get_میانگیر_مانده(ثابت VTerm *vt)
{
  أرجع vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, حرف *میانگیر, size_t len)
{
  إذا(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  کپی_حافظه(میانگیر, vt->outbuffer, len);

  إذا(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  أرجع len;
}

VTerm_مقدار_Type vterm_get_attr_type(VTermAttr attr)
{
  بدل(attr) {
    حالة VTERM_ATTR_BOLD:       أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_UNDERLINE:  أرجع VTERM_VALUETYPE_INT;
    حالة VTERM_ATTR_ITALIC:     أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_BLINK:      أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_معکوس:    أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_CONCEAL:    أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_STRIKE:     أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_FONT:       أرجع VTERM_VALUETYPE_INT;
    حالة VTERM_ATTR_FOREGROUND: أرجع VTERM_VALUETYPE_COLOR;
    حالة VTERM_ATTR_BACKGROUND: أرجع VTERM_VALUETYPE_COLOR;
    حالة VTERM_ATTR_SMALL:      أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_BASELINE:   أرجع VTERM_VALUETYPE_INT;

    حالة VTERM_N_ATTRS: أرجع 0;
  }
  أرجع 0; /* UNREACHABLE */
}

VTerm_مقدار_Type vterm_get_prop_type(VTermProp prop)
{
  بدل(prop) {
    حالة VTERM_PROP_CURSORVISIBLE: أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_CURSORBLINK:   أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_ALTSCREEN:     أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_TITLE:         أرجع VTERM_VALUETYPE_STRING;
    حالة VTERM_PROP_ICONNAME:      أرجع VTERM_VALUETYPE_STRING;
    حالة VTERM_PROP_معکوس:       أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_CURSORSHAPE:   أرجع VTERM_VALUETYPE_INT;
    حالة VTERM_PROP_MOUSE:         أرجع VTERM_VALUETYPE_INT;
    حالة VTERM_PROP_FOCUSREPORT:   أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_CURSORCOLOR:   أرجع VTERM_VALUETYPE_STRING;

    حالة VTERM_N_PROPS: أرجع 0;
  }
  أرجع 0; /* UNREACHABLE */
}

فراغ vterm_scroll_rect(VTermRect rect,
    صحيح downward,
    صحيح rightward,
    صحيح (*moverect)(VTermRect src, VTermRect dest, فراغ *user),
    صحيح (*eraserect)(VTermRect rect, صحيح selective, فراغ *user),
    فراغ *user)
{
  VTermRect src;
  VTermRect dest;

  إذا(قدر_مطلق(downward)  >= rect.انتها_ردیف - rect.شروع_ردیف ||
     قدر_مطلق(rightward) >= rect.انتها_ستون - rect.شروع_ستون) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    أرجع;
  }

  إذا(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.شروع_ستون = rect.شروع_ستون;
    dest.انتها_ستون   = rect.انتها_ستون   - rightward;
    src.شروع_ستون  = rect.شروع_ستون + rightward;
    src.انتها_ستون    = rect.انتها_ستون;
  }
  وإلا {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    صحيح leftward = -rightward;
    dest.شروع_ستون = rect.شروع_ستون + leftward;
    dest.انتها_ستون   = rect.انتها_ستون;
    src.شروع_ستون  = rect.شروع_ستون;
    src.انتها_ستون    = rect.انتها_ستون - leftward;
  }

  إذا(downward >= 0) {
    dest.شروع_ردیف = rect.شروع_ردیف;
    dest.انتها_ردیف   = rect.انتها_ردیف   - downward;
    src.شروع_ردیف  = rect.شروع_ردیف + downward;
    src.انتها_ردیف    = rect.انتها_ردیف;
  }
  وإلا {
    صحيح upward = -downward;
    dest.شروع_ردیف = rect.شروع_ردیف + upward;
    dest.انتها_ردیف   = rect.انتها_ردیف;
    src.شروع_ردیف  = rect.شروع_ردیف;
    src.انتها_ردیف    = rect.انتها_ردیف - upward;
  }

  إذا(moverect)
    (*moverect)(dest, src, user);

  إذا(downward > 0)
    rect.شروع_ردیف = rect.انتها_ردیف - downward;
  وإلا إذا(downward < 0)
    rect.انتها_ردیف = rect.شروع_ردیف - downward;

  إذا(rightward > 0)
    rect.شروع_ستون = rect.انتها_ستون - rightward;
  وإلا إذا(rightward < 0)
    rect.انتها_ستون = rect.شروع_ستون - rightward;

  (*eraserect)(rect, 0, user);
}

فراغ vterm_copy_cells(VTermRect dest,
    VTermRect src,
    فراغ (*copycell)(VTermPos dest, VTermPos src, فراغ *user),
    فراغ *user)
{
  صحيح downward  = src.شروع_ردیف - dest.شروع_ردیف;
  صحيح rightward = src.شروع_ستون - dest.شروع_ستون;

  صحيح init_ردیف, test_ردیف, init_ستون, test_ستون;
  صحيح inc_ردیف, inc_ستون;

  إذا(downward < 0) {
    init_ردیف = dest.انتها_ردیف - 1;
    test_ردیف = dest.شروع_ردیف - 1;
    inc_ردیف = -1;
  }
  وإلا /* downward >= 0 */ {
    init_ردیف = dest.شروع_ردیف;
    test_ردیف = dest.انتها_ردیف;
    inc_ردیف = +1;
  }

  إذا(rightward < 0) {
    init_ستون = dest.انتها_ستون - 1;
    test_ستون = dest.شروع_ستون - 1;
    inc_ستون = -1;
  }
  وإلا /* rightward >= 0 */ {
    init_ستون = dest.شروع_ستون;
    test_ستون = dest.انتها_ستون;
    inc_ستون = +1;
  }

  VTermPos pos;
  لكل(pos.ردیف = init_ردیف; pos.ردیف != test_ردیف; pos.ردیف += inc_ردیف)
    لكل(pos.ستون = init_ستون; pos.ستون != test_ستون; pos.ستون += inc_ستون) {
      VTermPos srcpos;
      srcpos.ردیف = pos.ردیف + downward;
      srcpos.ستون = pos.ستون + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

فراغ vterm_check_نسخه(صحيح major, صحيح minor)
{
  إذا(major != VTERM_نسخه_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_نسخه_MAJOR);
    خروج(1);
  }

  إذا(minor > VTERM_نسخه_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_نسخه_MINOR);
    خروج(1);
  }

  // Happy
}
