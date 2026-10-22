#define DEFINE_INLINES

// vim: set sw=2 :
#ଅନ୍ତର୍ଭାଵ "vterm_internal.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <stdarg.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "utf8.h"

/*****************
 * API functions *
 *****************/

ସ୍ଥିର ଶୂନ୍ୟ *default_malloc(size_t size, ଶୂନ୍ୟ *allocdata UNUSED)
{
  ଶୂନ୍ୟ *ptr = malloc(size);
  ଯଦି(ptr)
    memset(ptr, 0, size);
  ଫେରନ୍ତୁ ptr;
}

ସ୍ଥିର ଶୂନ୍ୟ default_free(ଶୂନ୍ୟ *ptr, ଶୂନ୍ୟ *allocdata UNUSED)
{
  ମୁକ୍ତ(ptr);
}

ସ୍ଥିର VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols)
{
  ସଂରଚନା VTermBuilder builder;
  memset(&builder, 0, ଆକାର(builder));
  builder.rows = rows;
  builder.cols = cols;
  ଫେରନ୍ତୁ vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, VTermAllocatorFunctions *funcs, ଶୂନ୍ୟ *allocdata)
{
  ସଂରଚନା VTermBuilder builder;
  memset(&builder, 0, ଆକାର(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  ଫେରନ୍ତୁ vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#define DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(ଅଚଳ ସଂରଚନା VTermBuilder *builder)
{
  ଅଚଳ VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->malloc)(ଆକାର(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->rows = builder->rows;
  vt->cols = builder->cols;

  vt->parser.state = NORMAL;

  vt->parser.callbacks = ଖାଲି;
  vt->parser.cbdata    = ଖାଲି;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = ଖାଲି;
  vt->outdata = ଖାଲି;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  ଯଦି (vt->tmpbuffer == ଖାଲି
      || vt->outbuffer == ଖାଲି
      || vt->tmpbuffer == ଖାଲି)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    ଫେରନ୍ତୁ ଖାଲି;
  }

  ଫେରନ୍ତୁ vt;
}

ଶୂନ୍ୟ vterm_free(VTerm *vt)
{
  ଯଦି(vt->screen)
    vterm_screen_free(vt->screen);

  ଯଦି(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL ଶୂନ୍ୟ *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  ଫେରନ୍ତୁ (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL ଶୂନ୍ୟ vterm_allocator_free(VTerm *vt, ଶୂନ୍ୟ *ptr)
{
  ଯଦି (ptr)
    (*vt->allocator->ମୁକ୍ତ)(ptr, vt->allocdata);
}

ଶୂନ୍ୟ vterm_get_size(ଅଚଳ VTerm *vt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *rowsp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *colsp)
{
  ଯଦି(rowsp)
    *rowsp = vt->rows;
  ଯଦି(colsp)
    *colsp = vt->cols;
}

ଶୂନ୍ୟ vterm_set_size(VTerm *vt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols)
{
  ଯଦି(rows < 1 || cols < 1)
    ଫେରନ୍ତୁ;

  vt->rows = rows;
  vt->cols = cols;

  ଯଦି(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_get_utf8(ଅଚଳ VTerm *vt)
{
  ଫେରନ୍ତୁ vt->mode.utf8;
}

ଶୂନ୍ୟ vterm_set_utf8(VTerm *vt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_utf8)
{
  vt->mode.utf8 = is_utf8;
}

ଶୂନ୍ୟ vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, ଶୂନ୍ୟ *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

INTERNAL ଶୂନ୍ୟ vterm_ଠେଲ_output_bytes(VTerm *vt, ଅଚଳ ଅକ୍ଷର *bytes, size_t len)
{
  ଯଦି(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    ଫେରନ୍ତୁ;
  }

  ଯଦି(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    ଫେରନ୍ତୁ;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL ଶୂନ୍ୟ vterm_ଠେଲ_output_vsprintf(VTerm *vt, ଅଚଳ ଅକ୍ଷର *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#ଅନ୍ୟଥା
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  ଯଦି (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#endif
  vterm_ଠେଲ_output_bytes(vt, vt->tmpbuffer, len);
}

INTERNAL ଶୂନ୍ୟ vterm_ଠେଲ_output_sprintf(VTerm *vt, ଅଚଳ ଅକ୍ଷର *format, ...)
{
  va_list args;
  va_start(args, format);
  vterm_ଠେଲ_output_vsprintf(vt, format, args);
  va_end(args);
}

INTERNAL ଶୂନ୍ୟ vterm_ଠେଲ_output_sprintf_ctrl(VTerm *vt, unsigned ଅକ୍ଷର ctrl, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  size_t cur;

  ଯଦି(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  ଅନ୍ୟଥା
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  ଯଦି(cur >= vt->tmpbuffer_len)
    ଫେରନ୍ତୁ;
  vterm_ଠେଲ_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_ଠେଲ_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL ଶୂନ୍ୟ vterm_ଠେଲ_output_sprintf_str(VTerm *vt, unsigned ଅକ୍ଷର ctrl, ପୂର୍ଣ୍ଣସଂଖ୍ୟା term, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  size_t cur;

  ଯଦି(ctrl) {
    ଯଦି(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    ଅନ୍ୟଥା
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    ଯଦି(cur >= vt->tmpbuffer_len)
      ଫେରନ୍ତୁ;
    vterm_ଠେଲ_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_ଠେଲ_output_vsprintf(vt, fmt, args);
  va_end(args);

  ଯଦି(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    ଯଦି(cur >= vt->tmpbuffer_len)
      ଫେରନ୍ତୁ;
    vterm_ଠେଲ_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(ଅଚଳ VTerm *vt)
{
  ଫେରନ୍ତୁ vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(ଅଚଳ VTerm *vt)
{
  ଫେରନ୍ତୁ vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(ଅଚଳ VTerm *vt)
{
  ଫେରନ୍ତୁ vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, ଅକ୍ଷର *buffer, size_t len)
{
  ଯଦି(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  ଯଦି(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  ଫେରନ୍ତୁ len;
}

VTerm_ମୂଲ୍ୟ_Type vterm_get_attr_type(VTermAttr attr)
{
  ବାଛ(attr) {
    ସ୍ଥିତି VTERM_ATTR_BOLD:       ଫେରନ୍ତୁ VTERM_VALUETYPE_BOOL;
    ସ୍ଥିତି VTERM_ATTR_UNDERLINE:  ଫେରନ୍ତୁ VTERM_VALUETYPE_INT;
    ସ୍ଥିତି VTERM_ATTR_ITALIC:     ଫେରନ୍ତୁ VTERM_VALUETYPE_BOOL;
    ସ୍ଥିତି VTERM_ATTR_BLINK:      ଫେରନ୍ତୁ VTERM_VALUETYPE_BOOL;
    ସ୍ଥିତି VTERM_ATTR_REVERSE:    ଫେରନ୍ତୁ VTERM_VALUETYPE_BOOL;
    ସ୍ଥିତି VTERM_ATTR_CONCEAL:    ଫେରନ୍ତୁ VTERM_VALUETYPE_BOOL;
    ସ୍ଥିତି VTERM_ATTR_STRIKE:     ଫେରନ୍ତୁ VTERM_VALUETYPE_BOOL;
    ସ୍ଥିତି VTERM_ATTR_FONT:       ଫେରନ୍ତୁ VTERM_VALUETYPE_INT;
    ସ୍ଥିତି VTERM_ATTR_FOREGROUND: ଫେରନ୍ତୁ VTERM_VALUETYPE_COLOR;
    ସ୍ଥିତି VTERM_ATTR_BACKGROUND: ଫେରନ୍ତୁ VTERM_VALUETYPE_COLOR;
    ସ୍ଥିତି VTERM_ATTR_SMALL:      ଫେରନ୍ତୁ VTERM_VALUETYPE_BOOL;
    ସ୍ଥିତି VTERM_ATTR_BASELINE:   ଫେରନ୍ତୁ VTERM_VALUETYPE_INT;

    ସ୍ଥିତି VTERM_N_ATTRS: ଫେରନ୍ତୁ 0;
  }
  ଫେରନ୍ତୁ 0; /* UNREACHABLE */
}

VTerm_ମୂଲ୍ୟ_Type vterm_get_prop_type(VTermProp prop)
{
  ବାଛ(prop) {
    ସ୍ଥିତି VTERM_PROP_CURSORVISIBLE: ଫେରନ୍ତୁ VTERM_VALUETYPE_BOOL;
    ସ୍ଥିତି VTERM_PROP_CURSORBLINK:   ଫେରନ୍ତୁ VTERM_VALUETYPE_BOOL;
    ସ୍ଥିତି VTERM_PROP_ALTSCREEN:     ଫେରନ୍ତୁ VTERM_VALUETYPE_BOOL;
    ସ୍ଥିତି VTERM_PROP_TITLE:         ଫେରନ୍ତୁ VTERM_VALUETYPE_STRING;
    ସ୍ଥିତି VTERM_PROP_ICONNAME:      ଫେରନ୍ତୁ VTERM_VALUETYPE_STRING;
    ସ୍ଥିତି VTERM_PROP_REVERSE:       ଫେରନ୍ତୁ VTERM_VALUETYPE_BOOL;
    ସ୍ଥିତି VTERM_PROP_CURSORSHAPE:   ଫେରନ୍ତୁ VTERM_VALUETYPE_INT;
    ସ୍ଥିତି VTERM_PROP_MOUSE:         ଫେରନ୍ତୁ VTERM_VALUETYPE_INT;
    ସ୍ଥିତି VTERM_PROP_FOCUSREPORT:   ଫେରନ୍ତୁ VTERM_VALUETYPE_BOOL;
    ସ୍ଥିତି VTERM_PROP_CURSORCOLOR:   ଫେରନ୍ତୁ VTERM_VALUETYPE_STRING;

    ସ୍ଥିତି VTERM_N_PROPS: ଫେରନ୍ତୁ 0;
  }
  ଫେରନ୍ତୁ 0; /* UNREACHABLE */
}

ଶୂନ୍ୟ vterm_scroll_rect(VTermRect rect,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା downward,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rightward,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*moverect)(VTermRect src, VTermRect dest, ଶୂନ୍ୟ *user),
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*eraserect)(VTermRect rect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା selective, ଶୂନ୍ୟ *user),
    ଶୂନ୍ୟ *user)
{
  VTermRect src;
  VTermRect dest;

  ଯଦି(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    ଫେରନ୍ତୁ;
  }

  ଯଦି(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  ଅନ୍ୟଥା {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  ଯଦି(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  ଅନ୍ୟଥା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  ଯଦି(moverect)
    (*moverect)(dest, src, user);

  ଯଦି(downward > 0)
    rect.start_row = rect.end_row - downward;
  ଅନ୍ୟଥା ଯଦି(downward < 0)
    rect.end_row = rect.start_row - downward;

  ଯଦି(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  ଅନ୍ୟଥା ଯଦି(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

ଶୂନ୍ୟ vterm_copy_cells(VTermRect dest,
    VTermRect src,
    ଶୂନ୍ୟ (*copycell)(VTermPos dest, VTermPos src, ଶୂନ୍ୟ *user),
    ଶୂନ୍ୟ *user)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା downward  = src.start_row - dest.start_row;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rightward = src.start_col - dest.start_col;

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା init_row, test_row, init_col, test_col;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା inc_row, inc_col;

  ଯଦି(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  ଅନ୍ୟଥା /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  ଯଦି(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  ଅନ୍ୟଥା /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  ପାଇଁ(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    ପାଇଁ(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

ଶୂନ୍ୟ vterm_check_version(ପୂର୍ଣ୍ଣସଂଖ୍ୟା major, ପୂର୍ଣ୍ଣସଂଖ୍ୟା minor)
{
  ଯଦି(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  ଯଦି(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
