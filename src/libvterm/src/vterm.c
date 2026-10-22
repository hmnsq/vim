#تعریف_کړه DEFINE_INLINES

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

ساكن فراغ *default_malloc(size_t اندازه, فراغ *allocdata ناکارول_شوی)
{
  فراغ *ptr = حافظه_ورکړه(اندازه);
  إذا(ptr)
    memset(ptr, 0, اندازه);
  أرجع ptr;
}

ساكن فراغ default_free(فراغ *ptr, فراغ *allocdata ناکارول_شوی)
{
  حرر(ptr);
}

ساكن VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(صحيح لیکې_2, صحيح ستون_شمېر)
{
  بنية VTermBuilder builder;
  memset(&builder, 0, حجم(builder));
  builder.لیکې_2 = لیکې_2;
  builder.ستون_شمېر = ستون_شمېر;
  أرجع vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(صحيح لیکې_2, صحيح ستون_شمېر, VTermAllocatorFunctions *funcs, فراغ *allocdata)
{
  بنية VTermBuilder builder;
  memset(&builder, 0, حجم(builder));
  builder.لیکې_2 = لیکې_2;
  builder.ستون_شمېر = ستون_شمېر;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  أرجع vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#تعریف_کړه DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(ثابت بنية VTermBuilder *builder)
{
  ثابت VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->حافظه_ورکړه)(حجم(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->لیکې_2 = builder->لیکې_2;
  vt->ستون_شمېر = builder->ستون_شمېر;

  vt->parser.state = نورمال_2;

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

دننی فراغ *vterm_allocator_malloc(VTerm *vt, size_t اندازه)
{
  أرجع (*vt->allocator->حافظه_ورکړه)(اندازه, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
دننی فراغ vterm_allocator_free(VTerm *vt, فراغ *ptr)
{
  إذا (ptr)
    (*vt->allocator->حرر)(ptr, vt->allocdata);
}

فراغ vterm_get_اندازه(ثابت VTerm *vt, صحيح *rowsp, صحيح *colsp)
{
  إذا(rowsp)
    *rowsp = vt->لیکې_2;
  إذا(colsp)
    *colsp = vt->ستون_شمېر;
}

فراغ vterm_سېټ_اندازه(VTerm *vt, صحيح لیکې_2, صحيح ستون_شمېر)
{
  إذا(لیکې_2 < 1 || ستون_شمېر < 1)
    أرجع;

  vt->لیکې_2 = لیکې_2;
  vt->ستون_شمېر = ستون_شمېر;

  إذا(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(لیکې_2, ستون_شمېر, vt->parser.cbdata);
}

صحيح vterm_get_utf8(ثابت VTerm *vt)
{
  أرجع vt->ډېرپېښ.utf8;
}

فراغ vterm_سېټ_utf8(VTerm *vt, صحيح is_utf8)
{
  vt->ډېرپېښ.utf8 = is_utf8;
}

فراغ vterm_output_سېټ_callback(VTerm *vt, VTermOutputCallback *func, فراغ *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

دننی فراغ vterm_ټېل_output_بایټونه(VTerm *vt, ثابت حرف *بایټونه, size_t len)
{
  إذا(vt->outfunc) {
    (vt->outfunc)(بایټونه, len, vt->outdata);
    أرجع;
  }

  إذا(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    أرجع;
  }

  حافظه_نقل(vt->outbuffer + vt->outbuffer_cur, بایټونه, len);
  vt->outbuffer_cur += len;
}

دننی فراغ vterm_ټېل_output_vsprintf(VTerm *vt, ثابت حرف *format, va_list دلیلونه)
{
  size_t len;
#که_تعریف_شوی VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, دلیلونه);
#وإلا
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, دلیلونه);
  إذا (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#د_که_پای
  vterm_ټېل_output_بایټونه(vt, vt->tmpbuffer, len);
}

دننی فراغ vterm_ټېل_output_sprintf(VTerm *vt, ثابت حرف *format, ...)
{
  va_list دلیلونه;
  va_start(دلیلونه, format);
  vterm_ټېل_output_vsprintf(vt, format, دلیلونه);
  va_end(دلیلونه);
}

دننی فراغ vterm_ټېل_output_sprintf_ctrl(VTerm *vt, بینښه حرف ctrl, ثابت حرف *fmt, ...)
{
  size_t cur;

  إذا(ctrl >= 0x80 && !vt->ډېرپېښ.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  وإلا
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  إذا(cur >= vt->tmpbuffer_len)
    أرجع;
  vterm_ټېل_output_بایټونه(vt, vt->tmpbuffer, cur);

  va_list دلیلونه;
  va_start(دلیلونه, fmt);
  vterm_ټېل_output_vsprintf(vt, fmt, دلیلونه);
  va_end(دلیلونه);
}

دننی فراغ vterm_ټېل_output_sprintf_str(VTerm *vt, بینښه حرف ctrl, صحيح غړی, ثابت حرف *fmt, ...)
{
  size_t cur;

  إذا(ctrl) {
    إذا(ctrl >= 0x80 && !vt->ډېرپېښ.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    وإلا
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    إذا(cur >= vt->tmpbuffer_len)
      أرجع;
    vterm_ټېل_output_بایټونه(vt, vt->tmpbuffer, cur);
  }

  va_list دلیلونه;
  va_start(دلیلونه, fmt);
  vterm_ټېل_output_vsprintf(vt, fmt, دلیلونه);
  va_end(دلیلونه);

  إذا(غړی) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->ډېرپېښ.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    إذا(cur >= vt->tmpbuffer_len)
      أرجع;
    vterm_ټېل_output_بایټونه(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_بفر_اندازه(ثابت VTerm *vt)
{
  أرجع vt->outbuffer_len;
}

size_t vterm_output_get_بفر_current(ثابت VTerm *vt)
{
  أرجع vt->outbuffer_cur;
}

size_t vterm_output_get_بفر_پاتې_شوی(ثابت VTerm *vt)
{
  أرجع vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, حرف *بفر, size_t len)
{
  إذا(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  حافظه_نقل(بفر, vt->outbuffer, len);

  إذا(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  أرجع len;
}

VTerm_ارزښت_Type vterm_get_attr_type(VTermAttr attr)
{
  بدل(attr) {
    حالة VTERM_ATTR_BOLD:       أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_UNDERLINE:  أرجع VTERM_VALUETYPE_INT;
    حالة VTERM_ATTR_ITALIC:     أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_BLINK:      أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_سرچپه:    أرجع VTERM_VALUETYPE_BOOL;
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

VTerm_ارزښت_Type vterm_get_prop_type(VTermProp prop)
{
  بدل(prop) {
    حالة VTERM_PROP_CURSORVISIBLE: أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_CURSORBLINK:   أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_ALTSCREEN:     أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_TITLE:         أرجع VTERM_VALUETYPE_STRING;
    حالة VTERM_PROP_ICONNAME:      أرجع VTERM_VALUETYPE_STRING;
    حالة VTERM_PROP_سرچپه:       أرجع VTERM_VALUETYPE_BOOL;
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

  إذا(مطلق_قیمت(downward)  >= rect.پای_ټکی_لیکه - rect.پیل_لیکه ||
     مطلق_قیمت(rightward) >= rect.پای_ټکی_ستون - rect.پیل_ستون) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    أرجع;
  }

  إذا(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.پیل_ستون = rect.پیل_ستون;
    dest.پای_ټکی_ستون   = rect.پای_ټکی_ستون   - rightward;
    src.پیل_ستون  = rect.پیل_ستون + rightward;
    src.پای_ټکی_ستون    = rect.پای_ټکی_ستون;
  }
  وإلا {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    صحيح leftward = -rightward;
    dest.پیل_ستون = rect.پیل_ستون + leftward;
    dest.پای_ټکی_ستون   = rect.پای_ټکی_ستون;
    src.پیل_ستون  = rect.پیل_ستون;
    src.پای_ټکی_ستون    = rect.پای_ټکی_ستون - leftward;
  }

  إذا(downward >= 0) {
    dest.پیل_لیکه = rect.پیل_لیکه;
    dest.پای_ټکی_لیکه   = rect.پای_ټکی_لیکه   - downward;
    src.پیل_لیکه  = rect.پیل_لیکه + downward;
    src.پای_ټکی_لیکه    = rect.پای_ټکی_لیکه;
  }
  وإلا {
    صحيح upward = -downward;
    dest.پیل_لیکه = rect.پیل_لیکه + upward;
    dest.پای_ټکی_لیکه   = rect.پای_ټکی_لیکه;
    src.پیل_لیکه  = rect.پیل_لیکه;
    src.پای_ټکی_لیکه    = rect.پای_ټکی_لیکه - upward;
  }

  إذا(moverect)
    (*moverect)(dest, src, user);

  إذا(downward > 0)
    rect.پیل_لیکه = rect.پای_ټکی_لیکه - downward;
  وإلا إذا(downward < 0)
    rect.پای_ټکی_لیکه = rect.پیل_لیکه - downward;

  إذا(rightward > 0)
    rect.پیل_ستون = rect.پای_ټکی_ستون - rightward;
  وإلا إذا(rightward < 0)
    rect.پای_ټکی_ستون = rect.پیل_ستون - rightward;

  (*eraserect)(rect, 0, user);
}

فراغ vterm_copy_cells(VTermRect dest,
    VTermRect src,
    فراغ (*copycell)(VTermPos dest, VTermPos src, فراغ *user),
    فراغ *user)
{
  صحيح downward  = src.پیل_لیکه - dest.پیل_لیکه;
  صحيح rightward = src.پیل_ستون - dest.پیل_ستون;

  صحيح init_لیکه, test_لیکه, init_ستون, test_ستون;
  صحيح inc_لیکه, inc_ستون;

  إذا(downward < 0) {
    init_لیکه = dest.پای_ټکی_لیکه - 1;
    test_لیکه = dest.پیل_لیکه - 1;
    inc_لیکه = -1;
  }
  وإلا /* downward >= 0 */ {
    init_لیکه = dest.پیل_لیکه;
    test_لیکه = dest.پای_ټکی_لیکه;
    inc_لیکه = +1;
  }

  إذا(rightward < 0) {
    init_ستون = dest.پای_ټکی_ستون - 1;
    test_ستون = dest.پیل_ستون - 1;
    inc_ستون = -1;
  }
  وإلا /* rightward >= 0 */ {
    init_ستون = dest.پیل_ستون;
    test_ستون = dest.پای_ټکی_ستون;
    inc_ستون = +1;
  }

  VTermPos pos;
  لكل(pos.لیکه = init_لیکه; pos.لیکه != test_لیکه; pos.لیکه += inc_لیکه)
    لكل(pos.ستون = init_ستون; pos.ستون != test_ستون; pos.ستون += inc_ستون) {
      VTermPos srcpos;
      srcpos.لیکه = pos.لیکه + downward;
      srcpos.ستون = pos.ستون + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

فراغ vterm_check_نسخه(صحيح major, صحيح minor)
{
  إذا(major != VTERM_نسخه_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_نسخه_MAJOR);
    وتل(1);
  }

  إذا(minor > VTERM_نسخه_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_نسخه_MINOR);
    وتل(1);
  }

  // Happy
}
