#تعریف_کرو DEFINE_INLINES

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

ساكن فراغ *default_malloc(size_t ناپ, فراغ *allocdata بن_ورتیا)
{
  فراغ *ptr = حافظہ_دیو(ناپ);
  إذا(ptr)
    memset(ptr, 0, ناپ);
  أرجع ptr;
}

ساكن فراغ default_free(فراغ *ptr, فراغ *allocdata بن_ورتیا)
{
  حرر(ptr);
}

ساكن VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(صحيح صفاں_2, صحيح خانے_گنتی)
{
  بنية VTermBuilder builder;
  memset(&builder, 0, حجم(builder));
  builder.صفاں_2 = صفاں_2;
  builder.خانے_گنتی = خانے_گنتی;
  أرجع vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(صحيح صفاں_2, صحيح خانے_گنتی, VTermAllocatorFunctions *funcs, فراغ *allocdata)
{
  بنية VTermBuilder builder;
  memset(&builder, 0, حجم(builder));
  builder.صفاں_2 = صفاں_2;
  builder.خانے_گنتی = خانے_گنتی;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  أرجع vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#تعریف_کرو DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(ثابت بنية VTermBuilder *builder)
{
  ثابت VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->حافظہ_دیو)(حجم(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->صفاں_2 = builder->صفاں_2;
  vt->خانے_گنتی = builder->خانے_گنتی;

  vt->parser.state = عام_3;

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

اندرونی فراغ *vterm_allocator_malloc(VTerm *vt, size_t ناپ)
{
  أرجع (*vt->allocator->حافظہ_دیو)(ناپ, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
اندرونی فراغ vterm_allocator_free(VTerm *vt, فراغ *ptr)
{
  إذا (ptr)
    (*vt->allocator->حرر)(ptr, vt->allocdata);
}

فراغ vterm_get_ناپ(ثابت VTerm *vt, صحيح *rowsp, صحيح *colsp)
{
  إذا(rowsp)
    *rowsp = vt->صفاں_2;
  إذا(colsp)
    *colsp = vt->خانے_گنتی;
}

فراغ vterm_سیٹ_ناپ(VTerm *vt, صحيح صفاں_2, صحيح خانے_گنتی)
{
  إذا(صفاں_2 < 1 || خانے_گنتی < 1)
    أرجع;

  vt->صفاں_2 = صفاں_2;
  vt->خانے_گنتی = خانے_گنتی;

  إذا(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(صفاں_2, خانے_گنتی, vt->parser.cbdata);
}

صحيح vterm_get_utf8(ثابت VTerm *vt)
{
  أرجع vt->منوال.utf8;
}

فراغ vterm_سیٹ_utf8(VTerm *vt, صحيح is_utf8)
{
  vt->منوال.utf8 = is_utf8;
}

فراغ vterm_output_سیٹ_callback(VTerm *vt, VTermOutputCallback *func, فراغ *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

اندرونی فراغ vterm_ڈالو_output_بائٹاں(VTerm *vt, ثابت حرف *بائٹاں, size_t len)
{
  إذا(vt->outfunc) {
    (vt->outfunc)(بائٹاں, len, vt->outdata);
    أرجع;
  }

  إذا(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    أرجع;
  }

  حافظہ_نقل(vt->outbuffer + vt->outbuffer_cur, بائٹاں, len);
  vt->outbuffer_cur += len;
}

اندرونی فراغ vterm_ڈالو_output_vsprintf(VTerm *vt, ثابت حرف *format, va_list دلیلاں)
{
  size_t len;
#جے_تعریف_شدہ VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, دلیلاں);
#وإلا
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, دلیلاں);
  إذا (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#جے_انت
  vterm_ڈالو_output_بائٹاں(vt, vt->tmpbuffer, len);
}

اندرونی فراغ vterm_ڈالو_output_sprintf(VTerm *vt, ثابت حرف *format, ...)
{
  va_list دلیلاں;
  va_start(دلیلاں, format);
  vterm_ڈالو_output_vsprintf(vt, format, دلیلاں);
  va_end(دلیلاں);
}

اندرونی فراغ vterm_ڈالو_output_sprintf_ctrl(VTerm *vt, بنانشانی حرف ctrl, ثابت حرف *fmt, ...)
{
  size_t cur;

  إذا(ctrl >= 0x80 && !vt->منوال.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  وإلا
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  إذا(cur >= vt->tmpbuffer_len)
    أرجع;
  vterm_ڈالو_output_بائٹاں(vt, vt->tmpbuffer, cur);

  va_list دلیلاں;
  va_start(دلیلاں, fmt);
  vterm_ڈالو_output_vsprintf(vt, fmt, دلیلاں);
  va_end(دلیلاں);
}

اندرونی فراغ vterm_ڈالو_output_sprintf_str(VTerm *vt, بنانشانی حرف ctrl, صحيح رکن, ثابت حرف *fmt, ...)
{
  size_t cur;

  إذا(ctrl) {
    إذا(ctrl >= 0x80 && !vt->منوال.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    وإلا
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    إذا(cur >= vt->tmpbuffer_len)
      أرجع;
    vterm_ڈالو_output_بائٹاں(vt, vt->tmpbuffer, cur);
  }

  va_list دلیلاں;
  va_start(دلیلاں, fmt);
  vterm_ڈالو_output_vsprintf(vt, fmt, دلیلاں);
  va_end(دلیلاں);

  إذا(رکن) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->منوال.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    إذا(cur >= vt->tmpbuffer_len)
      أرجع;
    vterm_ڈالو_output_بائٹاں(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_بفر_ناپ(ثابت VTerm *vt)
{
  أرجع vt->outbuffer_len;
}

size_t vterm_output_get_بفر_current(ثابت VTerm *vt)
{
  أرجع vt->outbuffer_cur;
}

size_t vterm_output_get_بفر_بچیا(ثابت VTerm *vt)
{
  أرجع vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, حرف *بفر, size_t len)
{
  إذا(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  حافظہ_نقل(بفر, vt->outbuffer, len);

  إذا(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  أرجع len;
}

VTerm_قدر_Type vterm_get_attr_type(VTermAttr attr)
{
  بدل(attr) {
    حالة VTERM_ATTR_BOLD:       أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_UNDERLINE:  أرجع VTERM_VALUETYPE_INT;
    حالة VTERM_ATTR_ITALIC:     أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_BLINK:      أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_ATTR_الٹ:    أرجع VTERM_VALUETYPE_BOOL;
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

VTerm_قدر_Type vterm_get_prop_type(VTermProp prop)
{
  بدل(prop) {
    حالة VTERM_PROP_CURSORVISIBLE: أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_CURSORBLINK:   أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_ALTSCREEN:     أرجع VTERM_VALUETYPE_BOOL;
    حالة VTERM_PROP_TITLE:         أرجع VTERM_VALUETYPE_STRING;
    حالة VTERM_PROP_ICONNAME:      أرجع VTERM_VALUETYPE_STRING;
    حالة VTERM_PROP_الٹ:       أرجع VTERM_VALUETYPE_BOOL;
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

  إذا(مطلق_قدر(downward)  >= rect.سرا_صف - rect.شروع_صف ||
     مطلق_قدر(rightward) >= rect.سرا_خانہ - rect.شروع_خانہ) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    أرجع;
  }

  إذا(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.شروع_خانہ = rect.شروع_خانہ;
    dest.سرا_خانہ   = rect.سرا_خانہ   - rightward;
    src.شروع_خانہ  = rect.شروع_خانہ + rightward;
    src.سرا_خانہ    = rect.سرا_خانہ;
  }
  وإلا {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    صحيح leftward = -rightward;
    dest.شروع_خانہ = rect.شروع_خانہ + leftward;
    dest.سرا_خانہ   = rect.سرا_خانہ;
    src.شروع_خانہ  = rect.شروع_خانہ;
    src.سرا_خانہ    = rect.سرا_خانہ - leftward;
  }

  إذا(downward >= 0) {
    dest.شروع_صف = rect.شروع_صف;
    dest.سرا_صف   = rect.سرا_صف   - downward;
    src.شروع_صف  = rect.شروع_صف + downward;
    src.سرا_صف    = rect.سرا_صف;
  }
  وإلا {
    صحيح upward = -downward;
    dest.شروع_صف = rect.شروع_صف + upward;
    dest.سرا_صف   = rect.سرا_صف;
    src.شروع_صف  = rect.شروع_صف;
    src.سرا_صف    = rect.سرا_صف - upward;
  }

  إذا(moverect)
    (*moverect)(dest, src, user);

  إذا(downward > 0)
    rect.شروع_صف = rect.سرا_صف - downward;
  وإلا إذا(downward < 0)
    rect.سرا_صف = rect.شروع_صف - downward;

  إذا(rightward > 0)
    rect.شروع_خانہ = rect.سرا_خانہ - rightward;
  وإلا إذا(rightward < 0)
    rect.سرا_خانہ = rect.شروع_خانہ - rightward;

  (*eraserect)(rect, 0, user);
}

فراغ vterm_copy_cells(VTermRect dest,
    VTermRect src,
    فراغ (*copycell)(VTermPos dest, VTermPos src, فراغ *user),
    فراغ *user)
{
  صحيح downward  = src.شروع_صف - dest.شروع_صف;
  صحيح rightward = src.شروع_خانہ - dest.شروع_خانہ;

  صحيح init_صف, test_صف, init_خانہ, test_خانہ;
  صحيح inc_صف, inc_خانہ;

  إذا(downward < 0) {
    init_صف = dest.سرا_صف - 1;
    test_صف = dest.شروع_صف - 1;
    inc_صف = -1;
  }
  وإلا /* downward >= 0 */ {
    init_صف = dest.شروع_صف;
    test_صف = dest.سرا_صف;
    inc_صف = +1;
  }

  إذا(rightward < 0) {
    init_خانہ = dest.سرا_خانہ - 1;
    test_خانہ = dest.شروع_خانہ - 1;
    inc_خانہ = -1;
  }
  وإلا /* rightward >= 0 */ {
    init_خانہ = dest.شروع_خانہ;
    test_خانہ = dest.سرا_خانہ;
    inc_خانہ = +1;
  }

  VTermPos pos;
  لكل(pos.صف = init_صف; pos.صف != test_صف; pos.صف += inc_صف)
    لكل(pos.خانہ = init_خانہ; pos.خانہ != test_خانہ; pos.خانہ += inc_خانہ) {
      VTermPos srcpos;
      srcpos.صف = pos.صف + downward;
      srcpos.خانہ = pos.خانہ + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

فراغ vterm_check_نسخہ(صحيح major, صحيح minor)
{
  إذا(major != VTERM_نسخہ_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_نسخہ_MAJOR);
    باہر_نکلو(1);
  }

  إذا(minor > VTERM_نسخہ_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_نسخہ_MINOR);
    باہر_نکلو(1);
  }

  // Happy
}
