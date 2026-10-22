#تعریف_کرو DEFINE_INLINES

// vim: set sw=2 :
#شامل "vterm_internal.h"

#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <stdarg.h>
#شامل <string.h>

#شامل "utf8.h"

/*****************
 * API functions *
 *****************/

ساکن خالی *default_malloc(size_t سائز, خالی *allocdata غیر_مستعمل)
{
  خالی *ptr = حافظہ_دو(سائز);
  اگر(ptr)
    memset(ptr, 0, سائز);
  واپس ptr;
}

ساکن خالی default_free(خالی *ptr, خالی *allocdata غیر_مستعمل)
{
  آزاد(ptr);
}

ساکن VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(صحیح صفیں_2, صحیح ستون_تعداد)
{
  ساخت VTermBuilder builder;
  memset(&builder, 0, حجم(builder));
  builder.صفیں_2 = صفیں_2;
  builder.ستون_تعداد = ستون_تعداد;
  واپس vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(صحیح صفیں_2, صحیح ستون_تعداد, VTermAllocatorFunctions *funcs, خالی *allocdata)
{
  ساخت VTermBuilder builder;
  memset(&builder, 0, حجم(builder));
  builder.صفیں_2 = صفیں_2;
  builder.ستون_تعداد = ستون_تعداد;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  واپس vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#تعریف_کرو DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(ثابت ساخت VTermBuilder *builder)
{
  ثابت VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->حافظہ_دو)(حجم(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->صفیں_2 = builder->صفیں_2;
  vt->ستون_تعداد = builder->ستون_تعداد;

  vt->parser.state = معمول_2;

  vt->parser.callbacks = تہی;
  vt->parser.cbdata    = تہی;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = تہی;
  vt->outdata = تہی;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  اگر (vt->tmpbuffer == تہی
      || vt->outbuffer == تہی
      || vt->tmpbuffer == تہی)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    واپس تہی;
  }

  واپس vt;
}

خالی vterm_free(VTerm *vt)
{
  اگر(vt->screen)
    vterm_screen_free(vt->screen);

  اگر(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

اندرونی_2 خالی *vterm_allocator_malloc(VTerm *vt, size_t سائز)
{
  واپس (*vt->allocator->حافظہ_دو)(سائز, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
اندرونی_2 خالی vterm_allocator_free(VTerm *vt, خالی *ptr)
{
  اگر (ptr)
    (*vt->allocator->آزاد)(ptr, vt->allocdata);
}

خالی vterm_get_سائز(ثابت VTerm *vt, صحیح *rowsp, صحیح *colsp)
{
  اگر(rowsp)
    *rowsp = vt->صفیں_2;
  اگر(colsp)
    *colsp = vt->ستون_تعداد;
}

خالی vterm_سیٹ_سائز(VTerm *vt, صحیح صفیں_2, صحیح ستون_تعداد)
{
  اگر(صفیں_2 < 1 || ستون_تعداد < 1)
    واپس;

  vt->صفیں_2 = صفیں_2;
  vt->ستون_تعداد = ستون_تعداد;

  اگر(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(صفیں_2, ستون_تعداد, vt->parser.cbdata);
}

صحیح vterm_get_utf8(ثابت VTerm *vt)
{
  واپس vt->منوال.utf8;
}

خالی vterm_سیٹ_utf8(VTerm *vt, صحیح is_utf8)
{
  vt->منوال.utf8 = is_utf8;
}

خالی vterm_output_سیٹ_callback(VTerm *vt, VTermOutputCallback *func, خالی *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

اندرونی_2 خالی vterm_ڈالو_output_بائٹیں(VTerm *vt, ثابت حرف *بائٹیں, size_t len)
{
  اگر(vt->outfunc) {
    (vt->outfunc)(بائٹیں, len, vt->outdata);
    واپس;
  }

  اگر(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    واپس;
  }

  حافظہ_نقل(vt->outbuffer + vt->outbuffer_cur, بائٹیں, len);
  vt->outbuffer_cur += len;
}

اندرونی_2 خالی vterm_ڈالو_output_vsprintf(VTerm *vt, ثابت حرف *format, va_list دلائل)
{
  size_t len;
#اگر_تعریف_شدہ VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, دلائل);
#ورنہ
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, دلائل);
  اگر (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#اگر_ختم
  vterm_ڈالو_output_بائٹیں(vt, vt->tmpbuffer, len);
}

اندرونی_2 خالی vterm_ڈالو_output_sprintf(VTerm *vt, ثابت حرف *format, ...)
{
  va_list دلائل;
  va_start(دلائل, format);
  vterm_ڈالو_output_vsprintf(vt, format, دلائل);
  va_end(دلائل);
}

اندرونی_2 خالی vterm_ڈالو_output_sprintf_ctrl(VTerm *vt, غیرعلامتی حرف ctrl, ثابت حرف *fmt, ...)
{
  size_t cur;

  اگر(ctrl >= 0x80 && !vt->منوال.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  ورنہ
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  اگر(cur >= vt->tmpbuffer_len)
    واپس;
  vterm_ڈالو_output_بائٹیں(vt, vt->tmpbuffer, cur);

  va_list دلائل;
  va_start(دلائل, fmt);
  vterm_ڈالو_output_vsprintf(vt, fmt, دلائل);
  va_end(دلائل);
}

اندرونی_2 خالی vterm_ڈالو_output_sprintf_str(VTerm *vt, غیرعلامتی حرف ctrl, صحیح رکن, ثابت حرف *fmt, ...)
{
  size_t cur;

  اگر(ctrl) {
    اگر(ctrl >= 0x80 && !vt->منوال.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    ورنہ
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    اگر(cur >= vt->tmpbuffer_len)
      واپس;
    vterm_ڈالو_output_بائٹیں(vt, vt->tmpbuffer, cur);
  }

  va_list دلائل;
  va_start(دلائل, fmt);
  vterm_ڈالو_output_vsprintf(vt, fmt, دلائل);
  va_end(دلائل);

  اگر(رکن) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->منوال.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    اگر(cur >= vt->tmpbuffer_len)
      واپس;
    vterm_ڈالو_output_بائٹیں(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_بفر_سائز(ثابت VTerm *vt)
{
  واپس vt->outbuffer_len;
}

size_t vterm_output_get_بفر_current(ثابت VTerm *vt)
{
  واپس vt->outbuffer_cur;
}

size_t vterm_output_get_بفر_بقایا(ثابت VTerm *vt)
{
  واپس vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, حرف *بفر, size_t len)
{
  اگر(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  حافظہ_نقل(بفر, vt->outbuffer, len);

  اگر(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  واپس len;
}

VTerm_قدر_Type vterm_get_attr_type(VTermAttr attr)
{
  انتخاب(attr) {
    حالت VTERM_ATTR_BOLD:       واپس VTERM_VALUETYPE_BOOL;
    حالت VTERM_ATTR_UNDERLINE:  واپس VTERM_VALUETYPE_INT;
    حالت VTERM_ATTR_ITALIC:     واپس VTERM_VALUETYPE_BOOL;
    حالت VTERM_ATTR_BLINK:      واپس VTERM_VALUETYPE_BOOL;
    حالت VTERM_ATTR_الٹ:    واپس VTERM_VALUETYPE_BOOL;
    حالت VTERM_ATTR_CONCEAL:    واپس VTERM_VALUETYPE_BOOL;
    حالت VTERM_ATTR_STRIKE:     واپس VTERM_VALUETYPE_BOOL;
    حالت VTERM_ATTR_FONT:       واپس VTERM_VALUETYPE_INT;
    حالت VTERM_ATTR_FOREGROUND: واپس VTERM_VALUETYPE_COLOR;
    حالت VTERM_ATTR_BACKGROUND: واپس VTERM_VALUETYPE_COLOR;
    حالت VTERM_ATTR_SMALL:      واپس VTERM_VALUETYPE_BOOL;
    حالت VTERM_ATTR_BASELINE:   واپس VTERM_VALUETYPE_INT;

    حالت VTERM_N_ATTRS: واپس 0;
  }
  واپس 0; /* UNREACHABLE */
}

VTerm_قدر_Type vterm_get_prop_type(VTermProp prop)
{
  انتخاب(prop) {
    حالت VTERM_PROP_CURSORVISIBLE: واپس VTERM_VALUETYPE_BOOL;
    حالت VTERM_PROP_CURSORBLINK:   واپس VTERM_VALUETYPE_BOOL;
    حالت VTERM_PROP_ALTSCREEN:     واپس VTERM_VALUETYPE_BOOL;
    حالت VTERM_PROP_TITLE:         واپس VTERM_VALUETYPE_STRING;
    حالت VTERM_PROP_ICONNAME:      واپس VTERM_VALUETYPE_STRING;
    حالت VTERM_PROP_الٹ:       واپس VTERM_VALUETYPE_BOOL;
    حالت VTERM_PROP_CURSORSHAPE:   واپس VTERM_VALUETYPE_INT;
    حالت VTERM_PROP_MOUSE:         واپس VTERM_VALUETYPE_INT;
    حالت VTERM_PROP_FOCUSREPORT:   واپس VTERM_VALUETYPE_BOOL;
    حالت VTERM_PROP_CURSORCOLOR:   واپس VTERM_VALUETYPE_STRING;

    حالت VTERM_N_PROPS: واپس 0;
  }
  واپس 0; /* UNREACHABLE */
}

خالی vterm_scroll_rect(VTermRect rect,
    صحیح downward,
    صحیح rightward,
    صحیح (*moverect)(VTermRect src, VTermRect dest, خالی *user),
    صحیح (*eraserect)(VTermRect rect, صحیح selective, خالی *user),
    خالی *user)
{
  VTermRect src;
  VTermRect dest;

  اگر(مطلق_قدر(downward)  >= rect.انتہا_صف - rect.آغاز_صف ||
     مطلق_قدر(rightward) >= rect.انتہا_ستون - rect.آغاز_ستون) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    واپس;
  }

  اگر(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.آغاز_ستون = rect.آغاز_ستون;
    dest.انتہا_ستون   = rect.انتہا_ستون   - rightward;
    src.آغاز_ستون  = rect.آغاز_ستون + rightward;
    src.انتہا_ستون    = rect.انتہا_ستون;
  }
  ورنہ {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    صحیح leftward = -rightward;
    dest.آغاز_ستون = rect.آغاز_ستون + leftward;
    dest.انتہا_ستون   = rect.انتہا_ستون;
    src.آغاز_ستون  = rect.آغاز_ستون;
    src.انتہا_ستون    = rect.انتہا_ستون - leftward;
  }

  اگر(downward >= 0) {
    dest.آغاز_صف = rect.آغاز_صف;
    dest.انتہا_صف   = rect.انتہا_صف   - downward;
    src.آغاز_صف  = rect.آغاز_صف + downward;
    src.انتہا_صف    = rect.انتہا_صف;
  }
  ورنہ {
    صحیح upward = -downward;
    dest.آغاز_صف = rect.آغاز_صف + upward;
    dest.انتہا_صف   = rect.انتہا_صف;
    src.آغاز_صف  = rect.آغاز_صف;
    src.انتہا_صف    = rect.انتہا_صف - upward;
  }

  اگر(moverect)
    (*moverect)(dest, src, user);

  اگر(downward > 0)
    rect.آغاز_صف = rect.انتہا_صف - downward;
  ورنہ اگر(downward < 0)
    rect.انتہا_صف = rect.آغاز_صف - downward;

  اگر(rightward > 0)
    rect.آغاز_ستون = rect.انتہا_ستون - rightward;
  ورنہ اگر(rightward < 0)
    rect.انتہا_ستون = rect.آغاز_ستون - rightward;

  (*eraserect)(rect, 0, user);
}

خالی vterm_copy_cells(VTermRect dest,
    VTermRect src,
    خالی (*copycell)(VTermPos dest, VTermPos src, خالی *user),
    خالی *user)
{
  صحیح downward  = src.آغاز_صف - dest.آغاز_صف;
  صحیح rightward = src.آغاز_ستون - dest.آغاز_ستون;

  صحیح init_صف, test_صف, init_ستون, test_ستون;
  صحیح inc_صف, inc_ستون;

  اگر(downward < 0) {
    init_صف = dest.انتہا_صف - 1;
    test_صف = dest.آغاز_صف - 1;
    inc_صف = -1;
  }
  ورنہ /* downward >= 0 */ {
    init_صف = dest.آغاز_صف;
    test_صف = dest.انتہا_صف;
    inc_صف = +1;
  }

  اگر(rightward < 0) {
    init_ستون = dest.انتہا_ستون - 1;
    test_ستون = dest.آغاز_ستون - 1;
    inc_ستون = -1;
  }
  ورنہ /* rightward >= 0 */ {
    init_ستون = dest.آغاز_ستون;
    test_ستون = dest.انتہا_ستون;
    inc_ستون = +1;
  }

  VTermPos pos;
  برائے(pos.صف = init_صف; pos.صف != test_صف; pos.صف += inc_صف)
    برائے(pos.ستون = init_ستون; pos.ستون != test_ستون; pos.ستون += inc_ستون) {
      VTermPos srcpos;
      srcpos.صف = pos.صف + downward;
      srcpos.ستون = pos.ستون + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

خالی vterm_check_نسخہ(صحیح major, صحیح minor)
{
  اگر(major != VTERM_نسخہ_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_نسخہ_MAJOR);
    باہر_نکلو(1);
  }

  اگر(minor > VTERM_نسخہ_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_نسخہ_MINOR);
    باہر_نکلو(1);
  }

  // Happy
}
