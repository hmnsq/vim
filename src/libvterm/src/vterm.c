#define DEFINE_INLINES

// vim: set sw=2 :
#include "vterm_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "utf8.h"

/*****************
 * API functions *
 *****************/

static void *default_malloc(size_t size, void *allocdata UNUSED)
{
  void *ptr = malloc(size);
  nếu(ptr)
    memset(ptr, 0, size);
  trả_về ptr;
}

static void default_free(void *ptr, void *allocdata UNUSED)
{
  free(ptr);
}

static VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(số_nguyên rows, số_nguyên cols)
{
  struct VTermBuilder builder;
  memset(&builder, 0, sizeof(builder));
  builder.rows = rows;
  builder.cols = cols;
  trả_về vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(số_nguyên rows, số_nguyên cols, VTermAllocatorFunctions *funcs, void *allocdata)
{
  struct VTermBuilder builder;
  memset(&builder, 0, sizeof(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  trả_về vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#define DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(const struct VTermBuilder *builder)
{
  const VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->malloc)(sizeof(VTerm), builder->allocdata);

  vt->allocator = allocator;
  vt->allocdata = builder->allocdata;

  vt->rows = builder->rows;
  vt->cols = builder->cols;

  vt->parser.state = NORMAL;

  vt->parser.callbacks = NULL;
  vt->parser.cbdata    = NULL;

  vt->parser.emit_nul  = FALSE;

  vt->outfunc = NULL;
  vt->outdata = NULL;

  vt->outbuffer_len = DEFAULT(builder->outbuffer_len, 4096);
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  vt->tmpbuffer_len = DEFAULT(builder->tmpbuffer_len, 4096);
  vt->tmpbuffer = vterm_allocator_malloc(vt, vt->tmpbuffer_len);

  nếu (vt->tmpbuffer == NULL
      || vt->outbuffer == NULL
      || vt->tmpbuffer == NULL)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    trả_về NULL;
  }

  trả_về vt;
}

void vterm_free(VTerm *vt)
{
  nếu(vt->screen)
    vterm_screen_free(vt->screen);

  nếu(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL void *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  trả_về (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL void vterm_allocator_free(VTerm *vt, void *ptr)
{
  nếu (ptr)
    (*vt->allocator->free)(ptr, vt->allocdata);
}

void vterm_get_size(const VTerm *vt, số_nguyên *rowsp, số_nguyên *colsp)
{
  nếu(rowsp)
    *rowsp = vt->rows;
  nếu(colsp)
    *colsp = vt->cols;
}

void vterm_set_size(VTerm *vt, số_nguyên rows, số_nguyên cols)
{
  nếu(rows < 1 || cols < 1)
    trả_về;

  vt->rows = rows;
  vt->cols = cols;

  nếu(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

số_nguyên vterm_get_utf8(const VTerm *vt)
{
  trả_về vt->mode.utf8;
}

void vterm_set_utf8(VTerm *vt, số_nguyên is_utf8)
{
  vt->mode.utf8 = is_utf8;
}

void vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, void *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

INTERNAL void vterm_push_output_bytes(VTerm *vt, const char *bytes, size_t len)
{
  nếu(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    trả_về;
  }

  nếu(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    trả_về;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL void vterm_push_output_vsprintf(VTerm *vt, const char *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#không_thì
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  nếu (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#endif
  vterm_push_output_bytes(vt, vt->tmpbuffer, len);
}

INTERNAL void vterm_push_output_sprintf(VTerm *vt, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vterm_push_output_vsprintf(vt, format, args);
  va_end(args);
}

INTERNAL void vterm_push_output_sprintf_ctrl(VTerm *vt, unsigned char ctrl, const char *fmt, ...)
{
  size_t cur;

  nếu(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  không_thì
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  nếu(cur >= vt->tmpbuffer_len)
    trả_về;
  vterm_push_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL void vterm_push_output_sprintf_str(VTerm *vt, unsigned char ctrl, số_nguyên term, const char *fmt, ...)
{
  size_t cur;

  nếu(ctrl) {
    nếu(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    không_thì
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    nếu(cur >= vt->tmpbuffer_len)
      trả_về;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);

  nếu(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    nếu(cur >= vt->tmpbuffer_len)
      trả_về;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(const VTerm *vt)
{
  trả_về vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(const VTerm *vt)
{
  trả_về vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(const VTerm *vt)
{
  trả_về vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, char *buffer, size_t len)
{
  nếu(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  nếu(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  trả_về len;
}

VTermValueType vterm_get_attr_type(VTermAttr attr)
{
  switch(attr) {
    case VTERM_ATTR_BOLD:       trả_về VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_UNDERLINE:  trả_về VTERM_VALUETYPE_INT;
    case VTERM_ATTR_ITALIC:     trả_về VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_BLINK:      trả_về VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_REVERSE:    trả_về VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_CONCEAL:    trả_về VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_STRIKE:     trả_về VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_FONT:       trả_về VTERM_VALUETYPE_INT;
    case VTERM_ATTR_FOREGROUND: trả_về VTERM_VALUETYPE_COLOR;
    case VTERM_ATTR_BACKGROUND: trả_về VTERM_VALUETYPE_COLOR;
    case VTERM_ATTR_SMALL:      trả_về VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_BASELINE:   trả_về VTERM_VALUETYPE_INT;

    case VTERM_N_ATTRS: trả_về 0;
  }
  trả_về 0; /* UNREACHABLE */
}

VTermValueType vterm_get_prop_type(VTermProp prop)
{
  switch(prop) {
    case VTERM_PROP_CURSORVISIBLE: trả_về VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_CURSORBLINK:   trả_về VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_ALTSCREEN:     trả_về VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_TITLE:         trả_về VTERM_VALUETYPE_STRING;
    case VTERM_PROP_ICONNAME:      trả_về VTERM_VALUETYPE_STRING;
    case VTERM_PROP_REVERSE:       trả_về VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_CURSORSHAPE:   trả_về VTERM_VALUETYPE_INT;
    case VTERM_PROP_MOUSE:         trả_về VTERM_VALUETYPE_INT;
    case VTERM_PROP_FOCUSREPORT:   trả_về VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_CURSORCOLOR:   trả_về VTERM_VALUETYPE_STRING;

    case VTERM_N_PROPS: trả_về 0;
  }
  trả_về 0; /* UNREACHABLE */
}

void vterm_scroll_rect(VTermRect rect,
    số_nguyên downward,
    số_nguyên rightward,
    số_nguyên (*moverect)(VTermRect src, VTermRect dest, void *user),
    số_nguyên (*eraserect)(VTermRect rect, số_nguyên selective, void *user),
    void *user)
{
  VTermRect src;
  VTermRect dest;

  nếu(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    trả_về;
  }

  nếu(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  không_thì {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    số_nguyên leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  nếu(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  không_thì {
    số_nguyên upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  nếu(moverect)
    (*moverect)(dest, src, user);

  nếu(downward > 0)
    rect.start_row = rect.end_row - downward;
  không_thì nếu(downward < 0)
    rect.end_row = rect.start_row - downward;

  nếu(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  không_thì nếu(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

void vterm_copy_cells(VTermRect dest,
    VTermRect src,
    void (*copycell)(VTermPos dest, VTermPos src, void *user),
    void *user)
{
  số_nguyên downward  = src.start_row - dest.start_row;
  số_nguyên rightward = src.start_col - dest.start_col;

  số_nguyên init_row, test_row, init_col, test_col;
  số_nguyên inc_row, inc_col;

  nếu(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  không_thì /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  nếu(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  không_thì /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  cho(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    cho(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

void vterm_check_version(số_nguyên major, số_nguyên minor)
{
  nếu(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  nếu(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
