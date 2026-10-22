#وصف_ڪريو DEFINE_INLINES

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

ساكن فراغ *default_malloc(size_t سائيز, فراغ *allocdata اڻ_استعمال_ٿيل)
{
  فراغ *ptr = يادگيري_ڏيو(سائيز);
  إذا(ptr)
    memset(ptr, 0, سائيز);
  أرجع ptr;
}

ساكن فراغ default_free(فراغ *ptr, فراغ *allocdata اڻ_استعمال_ٿيل)
{
  حرر(ptr);
}

ساكن VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(صحيح صفون_2, صحيح کانن_ڳڻپ)
{
  بنية VTermBuilder builder;
  memset(&builder, 0, حجم(builder));
  builder.صفون_2 = صفون_2;
  builder.کانن_ڳڻپ = کانن_ڳڻپ;
  أرجع vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(صحيح صفون_2, صحيح کانن_ڳڻپ, VTermAllocatorFunctions *funcs, فراغ *allocdata)
{
  بنية VTermBuilder builder;
  memset(&builder, 0, حجم(builder));
  builder.صفون_2 = صفون_2;
  builder.کانن_ڳڻپ = کانن_ڳڻپ;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  أرجع vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#وصف_ڪريو DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(ثابت بنية VTermBuilder *builder)
{
  ثابت VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->يادگيري_ڏيو)(حجم(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->صفون_2 = builder->صفون_2;
  vt->کانن_ڳڻپ = builder->کانن_ڳڻپ;

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

اندروني فراغ *vterm_allocator_malloc(VTerm *vt, size_t سائيز)
{
  أرجع (*vt->allocator->يادگيري_ڏيو)(سائيز, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
اندروني فراغ vterm_allocator_free(VTerm *vt, فراغ *ptr)
{
  إذا (ptr)
    (*vt->allocator->حرر)(ptr, vt->allocdata);
}

فراغ vterm_get_سائيز(ثابت VTerm *vt, صحيح *rowsp, صحيح *colsp)
{
  إذا(rowsp)
    *rowsp = vt->صفون_2;
  إذا(colsp)
    *colsp = vt->کانن_ڳڻپ;
}

فراغ vterm_سيٽ_سائيز(VTerm *vt, صحيح صفون_2, صحيح کانن_ڳڻپ)
{
  إذا(صفون_2 < 1 || کانن_ڳڻپ < 1)
    أرجع;

  vt->صفون_2 = صفون_2;
  vt->کانن_ڳڻپ = کانن_ڳڻپ;

  إذا(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(صفون_2, کانن_ڳڻپ, vt->parser.cbdata);
}

صحيح vterm_get_utf8(ثابت VTerm *vt)
{
  أرجع vt->گهڻائي.utf8;
}

فراغ vterm_سيٽ_utf8(VTerm *vt, صحيح is_utf8)
{
  vt->گهڻائي.utf8 = is_utf8;
}

فراغ vterm_output_سيٽ_callback(VTerm *vt, VTermOutputCallback *func, فراغ *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

اندروني فراغ vterm_ڌڪو_output_بائيٽون(VTerm *vt, ثابت حرف *بائيٽون, size_t len)
{
  إذا(vt->outfunc) {
    (vt->outfunc)(بائيٽون, len, vt->outdata);
    أرجع;
  }

  إذا(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    أرجع;
  }

  يادگيري_نقل(vt->outbuffer + vt->outbuffer_cur, بائيٽون, len);
  vt->outbuffer_cur += len;
}

اندروني فراغ vterm_ڌڪو_output_vsprintf(VTerm *vt, ثابت حرف *format, va_list دليلون)
{
  size_t len;
#جيڪڏهن_وصف_ٿيل VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, دليلون);
#وإلا
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, دليلون);
  إذا (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#جيڪڏهن_پورو
  vterm_ڌڪو_output_بائيٽون(vt, vt->tmpbuffer, len);
}

اندروني فراغ vterm_ڌڪو_output_sprintf(VTerm *vt, ثابت حرف *format, ...)
{
  va_list دليلون;
  va_start(دليلون, format);
  vterm_ڌڪو_output_vsprintf(vt, format, دليلون);
  va_end(دليلون);
}

اندروني فراغ vterm_ڌڪو_output_sprintf_ctrl(VTerm *vt, بنانشان حرف ctrl, ثابت حرف *fmt, ...)
{
  size_t cur;

  إذا(ctrl >= 0x80 && !vt->گهڻائي.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  وإلا
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  إذا(cur >= vt->tmpbuffer_len)
    أرجع;
  vterm_ڌڪو_output_بائيٽون(vt, vt->tmpbuffer, cur);

  va_list دليلون;
  va_start(دليلون, fmt);
  vterm_ڌڪو_output_vsprintf(vt, fmt, دليلون);
  va_end(دليلون);
}

اندروني فراغ vterm_ڌڪو_output_sprintf_str(VTerm *vt, بنانشان حرف ctrl, صحيح رڪن, ثابت حرف *fmt, ...)
{
  size_t cur;

  إذا(ctrl) {
    إذا(ctrl >= 0x80 && !vt->گهڻائي.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    وإلا
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    إذا(cur >= vt->tmpbuffer_len)
      أرجع;
    vterm_ڌڪو_output_بائيٽون(vt, vt->tmpbuffer, cur);
  }

  va_list دليلون;
  va_start(دليلون, fmt);
  vterm_ڌڪو_output_vsprintf(vt, fmt, دليلون);
  va_end(دليلون);

  إذا(رڪن) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->گهڻائي.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    إذا(cur >= vt->tmpbuffer_len)
      أرجع;
    vterm_ڌڪو_output_بائيٽون(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_بفر_سائيز(ثابت VTerm *vt)
{
  أرجع vt->outbuffer_len;
}

size_t vterm_output_get_بفر_current(ثابت VTerm *vt)
{
  أرجع vt->outbuffer_cur;
}

size_t vterm_output_get_بفر_بچيل(ثابت VTerm *vt)
{
  أرجع vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, حرف *بفر, size_t len)
{
  إذا(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  يادگيري_نقل(بفر, vt->outbuffer, len);

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
    حالة VTERM_ATTR_ابتو:    أرجع VTERM_VALUETYPE_BOOL;
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
    حالة VTERM_PROP_ابتو:       أرجع VTERM_VALUETYPE_BOOL;
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

  إذا(مطلق_قدر(downward)  >= rect.پڇاڙي_صف - rect.شروعات_صف ||
     مطلق_قدر(rightward) >= rect.پڇاڙي_کانو - rect.شروعات_کانو) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    أرجع;
  }

  إذا(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.شروعات_کانو = rect.شروعات_کانو;
    dest.پڇاڙي_کانو   = rect.پڇاڙي_کانو   - rightward;
    src.شروعات_کانو  = rect.شروعات_کانو + rightward;
    src.پڇاڙي_کانو    = rect.پڇاڙي_کانو;
  }
  وإلا {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    صحيح leftward = -rightward;
    dest.شروعات_کانو = rect.شروعات_کانو + leftward;
    dest.پڇاڙي_کانو   = rect.پڇاڙي_کانو;
    src.شروعات_کانو  = rect.شروعات_کانو;
    src.پڇاڙي_کانو    = rect.پڇاڙي_کانو - leftward;
  }

  إذا(downward >= 0) {
    dest.شروعات_صف = rect.شروعات_صف;
    dest.پڇاڙي_صف   = rect.پڇاڙي_صف   - downward;
    src.شروعات_صف  = rect.شروعات_صف + downward;
    src.پڇاڙي_صف    = rect.پڇاڙي_صف;
  }
  وإلا {
    صحيح upward = -downward;
    dest.شروعات_صف = rect.شروعات_صف + upward;
    dest.پڇاڙي_صف   = rect.پڇاڙي_صف;
    src.شروعات_صف  = rect.شروعات_صف;
    src.پڇاڙي_صف    = rect.پڇاڙي_صف - upward;
  }

  إذا(moverect)
    (*moverect)(dest, src, user);

  إذا(downward > 0)
    rect.شروعات_صف = rect.پڇاڙي_صف - downward;
  وإلا إذا(downward < 0)
    rect.پڇاڙي_صف = rect.شروعات_صف - downward;

  إذا(rightward > 0)
    rect.شروعات_کانو = rect.پڇاڙي_کانو - rightward;
  وإلا إذا(rightward < 0)
    rect.پڇاڙي_کانو = rect.شروعات_کانو - rightward;

  (*eraserect)(rect, 0, user);
}

فراغ vterm_copy_cells(VTermRect dest,
    VTermRect src,
    فراغ (*copycell)(VTermPos dest, VTermPos src, فراغ *user),
    فراغ *user)
{
  صحيح downward  = src.شروعات_صف - dest.شروعات_صف;
  صحيح rightward = src.شروعات_کانو - dest.شروعات_کانو;

  صحيح init_صف, test_صف, init_کانو, test_کانو;
  صحيح inc_صف, inc_کانو;

  إذا(downward < 0) {
    init_صف = dest.پڇاڙي_صف - 1;
    test_صف = dest.شروعات_صف - 1;
    inc_صف = -1;
  }
  وإلا /* downward >= 0 */ {
    init_صف = dest.شروعات_صف;
    test_صف = dest.پڇاڙي_صف;
    inc_صف = +1;
  }

  إذا(rightward < 0) {
    init_کانو = dest.پڇاڙي_کانو - 1;
    test_کانو = dest.شروعات_کانو - 1;
    inc_کانو = -1;
  }
  وإلا /* rightward >= 0 */ {
    init_کانو = dest.شروعات_کانو;
    test_کانو = dest.پڇاڙي_کانو;
    inc_کانو = +1;
  }

  VTermPos pos;
  لكل(pos.صف = init_صف; pos.صف != test_صف; pos.صف += inc_صف)
    لكل(pos.کانو = init_کانو; pos.کانو != test_کانو; pos.کانو += inc_کانو) {
      VTermPos srcpos;
      srcpos.صف = pos.صف + downward;
      srcpos.کانو = pos.کانو + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

فراغ vterm_check_نسخو(صحيح major, صحيح minor)
{
  إذا(major != VTERM_نسخو_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_نسخو_MAJOR);
    ٻاهر_نڪرو(1);
  }

  إذا(minor > VTERM_نسخو_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_نسخو_MINOR);
    ٻاهر_نڪرو(1);
  }

  // Happy
}
