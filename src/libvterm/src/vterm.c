#define DEFINE_INLINES

// vim: set sw=2 :
#অন্তর্ভাব "vterm_internal.h"

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <stdarg.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "utf8.h"

/*****************
 * API functions *
 *****************/

স্থির শূন্য *default_malloc(size_t আকার_2, শূন্য *allocdata UNUSED)
{
  শূন্য *ptr = malloc(আকার_2);
  যদি(ptr)
    memset(ptr, 0, আকার_2);
  ফেরত ptr;
}

স্থির শূন্য default_free(শূন্য *ptr, শূন্য *allocdata UNUSED)
{
  মুক্ত(ptr);
}

স্থির VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(পূর্ণসংখ্যা rows, পূর্ণসংখ্যা cols)
{
  কাঠামো VTermBuilder builder;
  memset(&builder, 0, আকার(builder));
  builder.rows = rows;
  builder.cols = cols;
  ফেরত vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(পূর্ণসংখ্যা rows, পূর্ণসংখ্যা cols, VTermAllocatorFunctions *funcs, শূন্য *allocdata)
{
  কাঠামো VTermBuilder builder;
  memset(&builder, 0, আকার(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  ফেরত vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#define DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(ধ্রুবক কাঠামো VTermBuilder *builder)
{
  ধ্রুবক VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->malloc)(আকার(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->rows = builder->rows;
  vt->cols = builder->cols;

  vt->parser.state = NORMAL;

  vt->parser.callbacks = ফাঁকা;
  vt->parser.cbdata    = ফাঁকা;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = ফাঁকা;
  vt->outdata = ফাঁকা;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  যদি (vt->tmpbuffer == ফাঁকা
      || vt->outbuffer == ফাঁকা
      || vt->tmpbuffer == ফাঁকা)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    ফেরত ফাঁকা;
  }

  ফেরত vt;
}

শূন্য vterm_free(VTerm *vt)
{
  যদি(vt->screen)
    vterm_screen_free(vt->screen);

  যদি(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL শূন্য *vterm_allocator_malloc(VTerm *vt, size_t আকার_2)
{
  ফেরত (*vt->allocator->malloc)(আকার_2, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL শূন্য vterm_allocator_free(VTerm *vt, শূন্য *ptr)
{
  যদি (ptr)
    (*vt->allocator->মুক্ত)(ptr, vt->allocdata);
}

শূন্য vterm_get_আকার(ধ্রুবক VTerm *vt, পূর্ণসংখ্যা *rowsp, পূর্ণসংখ্যা *colsp)
{
  যদি(rowsp)
    *rowsp = vt->rows;
  যদি(colsp)
    *colsp = vt->cols;
}

শূন্য vterm_set_আকার(VTerm *vt, পূর্ণসংখ্যা rows, পূর্ণসংখ্যা cols)
{
  যদি(rows < 1 || cols < 1)
    ফেরত;

  vt->rows = rows;
  vt->cols = cols;

  যদি(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

পূর্ণসংখ্যা vterm_get_utf8(ধ্রুবক VTerm *vt)
{
  ফেরত vt->mode.utf8;
}

শূন্য vterm_set_utf8(VTerm *vt, পূর্ণসংখ্যা is_utf8)
{
  vt->mode.utf8 = is_utf8;
}

শূন্য vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, শূন্য *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

INTERNAL শূন্য vterm_ঠেলো_output_bytes(VTerm *vt, ধ্রুবক অক্ষর *bytes, size_t len)
{
  যদি(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    ফেরত;
  }

  যদি(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    ফেরত;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL শূন্য vterm_ঠেলো_output_vsprintf(VTerm *vt, ধ্রুবক অক্ষর *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#অন্যথা
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  যদি (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#endif
  vterm_ঠেলো_output_bytes(vt, vt->tmpbuffer, len);
}

INTERNAL শূন্য vterm_ঠেলো_output_sprintf(VTerm *vt, ধ্রুবক অক্ষর *format, ...)
{
  va_list args;
  va_start(args, format);
  vterm_ঠেলো_output_vsprintf(vt, format, args);
  va_end(args);
}

INTERNAL শূন্য vterm_ঠেলো_output_sprintf_ctrl(VTerm *vt, unsigned অক্ষর ctrl, ধ্রুবক অক্ষর *fmt, ...)
{
  size_t cur;

  যদি(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  অন্যথা
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  যদি(cur >= vt->tmpbuffer_len)
    ফেরত;
  vterm_ঠেলো_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_ঠেলো_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL শূন্য vterm_ঠেলো_output_sprintf_str(VTerm *vt, unsigned অক্ষর ctrl, পূর্ণসংখ্যা term, ধ্রুবক অক্ষর *fmt, ...)
{
  size_t cur;

  যদি(ctrl) {
    যদি(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    অন্যথা
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    যদি(cur >= vt->tmpbuffer_len)
      ফেরত;
    vterm_ঠেলো_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_ঠেলো_output_vsprintf(vt, fmt, args);
  va_end(args);

  যদি(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    যদি(cur >= vt->tmpbuffer_len)
      ফেরত;
    vterm_ঠেলো_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_বাফার_আকার(ধ্রুবক VTerm *vt)
{
  ফেরত vt->outbuffer_len;
}

size_t vterm_output_get_বাফার_current(ধ্রুবক VTerm *vt)
{
  ফেরত vt->outbuffer_cur;
}

size_t vterm_output_get_বাফার_remaining(ধ্রুবক VTerm *vt)
{
  ফেরত vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, অক্ষর *বাফার, size_t len)
{
  যদি(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(বাফার, vt->outbuffer, len);

  যদি(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  ফেরত len;
}

VTerm_মান_Type vterm_get_attr_type(VTermAttr attr)
{
  নির্বাচন(attr) {
    ক্ষেত্র VTERM_ATTR_BOLD:       ফেরত VTERM_VALUETYPE_BOOL;
    ক্ষেত্র VTERM_ATTR_UNDERLINE:  ফেরত VTERM_VALUETYPE_INT;
    ক্ষেত্র VTERM_ATTR_ITALIC:     ফেরত VTERM_VALUETYPE_BOOL;
    ক্ষেত্র VTERM_ATTR_BLINK:      ফেরত VTERM_VALUETYPE_BOOL;
    ক্ষেত্র VTERM_ATTR_REVERSE:    ফেরত VTERM_VALUETYPE_BOOL;
    ক্ষেত্র VTERM_ATTR_CONCEAL:    ফেরত VTERM_VALUETYPE_BOOL;
    ক্ষেত্র VTERM_ATTR_STRIKE:     ফেরত VTERM_VALUETYPE_BOOL;
    ক্ষেত্র VTERM_ATTR_FONT:       ফেরত VTERM_VALUETYPE_INT;
    ক্ষেত্র VTERM_ATTR_FOREGROUND: ফেরত VTERM_VALUETYPE_COLOR;
    ক্ষেত্র VTERM_ATTR_BACKGROUND: ফেরত VTERM_VALUETYPE_COLOR;
    ক্ষেত্র VTERM_ATTR_SMALL:      ফেরত VTERM_VALUETYPE_BOOL;
    ক্ষেত্র VTERM_ATTR_BASELINE:   ফেরত VTERM_VALUETYPE_INT;

    ক্ষেত্র VTERM_N_ATTRS: ফেরত 0;
  }
  ফেরত 0; /* UNREACHABLE */
}

VTerm_মান_Type vterm_get_prop_type(VTermProp prop)
{
  নির্বাচন(prop) {
    ক্ষেত্র VTERM_PROP_CURSORVISIBLE: ফেরত VTERM_VALUETYPE_BOOL;
    ক্ষেত্র VTERM_PROP_CURSORBLINK:   ফেরত VTERM_VALUETYPE_BOOL;
    ক্ষেত্র VTERM_PROP_ALTSCREEN:     ফেরত VTERM_VALUETYPE_BOOL;
    ক্ষেত্র VTERM_PROP_TITLE:         ফেরত VTERM_VALUETYPE_STRING;
    ক্ষেত্র VTERM_PROP_ICONNAME:      ফেরত VTERM_VALUETYPE_STRING;
    ক্ষেত্র VTERM_PROP_REVERSE:       ফেরত VTERM_VALUETYPE_BOOL;
    ক্ষেত্র VTERM_PROP_CURSORSHAPE:   ফেরত VTERM_VALUETYPE_INT;
    ক্ষেত্র VTERM_PROP_MOUSE:         ফেরত VTERM_VALUETYPE_INT;
    ক্ষেত্র VTERM_PROP_FOCUSREPORT:   ফেরত VTERM_VALUETYPE_BOOL;
    ক্ষেত্র VTERM_PROP_CURSORCOLOR:   ফেরত VTERM_VALUETYPE_STRING;

    ক্ষেত্র VTERM_N_PROPS: ফেরত 0;
  }
  ফেরত 0; /* UNREACHABLE */
}

শূন্য vterm_scroll_rect(VTermRect rect,
    পূর্ণসংখ্যা downward,
    পূর্ণসংখ্যা rightward,
    পূর্ণসংখ্যা (*moverect)(VTermRect src, VTermRect dest, শূন্য *user),
    পূর্ণসংখ্যা (*eraserect)(VTermRect rect, পূর্ণসংখ্যা selective, শূন্য *user),
    শূন্য *user)
{
  VTermRect src;
  VTermRect dest;

  যদি(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    ফেরত;
  }

  যদি(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  অন্যথা {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    পূর্ণসংখ্যা leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  যদি(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  অন্যথা {
    পূর্ণসংখ্যা upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  যদি(moverect)
    (*moverect)(dest, src, user);

  যদি(downward > 0)
    rect.start_row = rect.end_row - downward;
  অন্যথা যদি(downward < 0)
    rect.end_row = rect.start_row - downward;

  যদি(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  অন্যথা যদি(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

শূন্য vterm_copy_cells(VTermRect dest,
    VTermRect src,
    শূন্য (*copycell)(VTermPos dest, VTermPos src, শূন্য *user),
    শূন্য *user)
{
  পূর্ণসংখ্যা downward  = src.start_row - dest.start_row;
  পূর্ণসংখ্যা rightward = src.start_col - dest.start_col;

  পূর্ণসংখ্যা init_row, test_row, init_col, test_col;
  পূর্ণসংখ্যা inc_row, inc_col;

  যদি(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  অন্যথা /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  যদি(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  অন্যথা /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  জন্য(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    জন্য(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

শূন্য vterm_check_version(পূর্ণসংখ্যা major, পূর্ণসংখ্যা minor)
{
  যদি(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  যদি(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
