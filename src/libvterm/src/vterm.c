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
  якщо(ptr)
    memset(ptr, 0, size);
  повернути ptr;
}

static void default_free(void *ptr, void *allocdata UNUSED)
{
  free(ptr);
}

static VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(ціле rows, ціле cols)
{
  struct VTermBuilder builder;
  memset(&builder, 0, sizeof(builder));
  builder.rows = rows;
  builder.cols = cols;
  повернути vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(ціле rows, ціле cols, VTermAllocatorFunctions *funcs, void *allocdata)
{
  struct VTermBuilder builder;
  memset(&builder, 0, sizeof(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  повернути vterm_build(&builder);
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

  якщо (vt->tmpbuffer == NULL
      || vt->outbuffer == NULL
      || vt->tmpbuffer == NULL)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    повернути NULL;
  }

  повернути vt;
}

void vterm_free(VTerm *vt)
{
  якщо(vt->screen)
    vterm_screen_free(vt->screen);

  якщо(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL void *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  повернути (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL void vterm_allocator_free(VTerm *vt, void *ptr)
{
  якщо (ptr)
    (*vt->allocator->free)(ptr, vt->allocdata);
}

void vterm_get_size(const VTerm *vt, ціле *rowsp, ціле *colsp)
{
  якщо(rowsp)
    *rowsp = vt->rows;
  якщо(colsp)
    *colsp = vt->cols;
}

void vterm_set_size(VTerm *vt, ціле rows, ціле cols)
{
  якщо(rows < 1 || cols < 1)
    повернути;

  vt->rows = rows;
  vt->cols = cols;

  якщо(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

ціле vterm_get_utf8(const VTerm *vt)
{
  повернути vt->mode.utf8;
}

void vterm_set_utf8(VTerm *vt, ціле is_utf8)
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
  якщо(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    повернути;
  }

  якщо(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    повернути;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL void vterm_push_output_vsprintf(VTerm *vt, const char *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#інакше
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  якщо (len >= vt->tmpbuffer_len)
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

  якщо(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  інакше
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  якщо(cur >= vt->tmpbuffer_len)
    повернути;
  vterm_push_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL void vterm_push_output_sprintf_str(VTerm *vt, unsigned char ctrl, ціле term, const char *fmt, ...)
{
  size_t cur;

  якщо(ctrl) {
    якщо(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    інакше
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    якщо(cur >= vt->tmpbuffer_len)
      повернути;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);

  якщо(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    якщо(cur >= vt->tmpbuffer_len)
      повернути;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(const VTerm *vt)
{
  повернути vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(const VTerm *vt)
{
  повернути vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(const VTerm *vt)
{
  повернути vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, char *buffer, size_t len)
{
  якщо(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  якщо(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  повернути len;
}

VTermValueType vterm_get_attr_type(VTermAttr attr)
{
  switch(attr) {
    case VTERM_ATTR_BOLD:       повернути VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_UNDERLINE:  повернути VTERM_VALUETYPE_INT;
    case VTERM_ATTR_ITALIC:     повернути VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_BLINK:      повернути VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_REVERSE:    повернути VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_CONCEAL:    повернути VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_STRIKE:     повернути VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_FONT:       повернути VTERM_VALUETYPE_INT;
    case VTERM_ATTR_FOREGROUND: повернути VTERM_VALUETYPE_COLOR;
    case VTERM_ATTR_BACKGROUND: повернути VTERM_VALUETYPE_COLOR;
    case VTERM_ATTR_SMALL:      повернути VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_BASELINE:   повернути VTERM_VALUETYPE_INT;

    case VTERM_N_ATTRS: повернути 0;
  }
  повернути 0; /* UNREACHABLE */
}

VTermValueType vterm_get_prop_type(VTermProp prop)
{
  switch(prop) {
    case VTERM_PROP_CURSORVISIBLE: повернути VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_CURSORBLINK:   повернути VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_ALTSCREEN:     повернути VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_TITLE:         повернути VTERM_VALUETYPE_STRING;
    case VTERM_PROP_ICONNAME:      повернути VTERM_VALUETYPE_STRING;
    case VTERM_PROP_REVERSE:       повернути VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_CURSORSHAPE:   повернути VTERM_VALUETYPE_INT;
    case VTERM_PROP_MOUSE:         повернути VTERM_VALUETYPE_INT;
    case VTERM_PROP_FOCUSREPORT:   повернути VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_CURSORCOLOR:   повернути VTERM_VALUETYPE_STRING;

    case VTERM_N_PROPS: повернути 0;
  }
  повернути 0; /* UNREACHABLE */
}

void vterm_scroll_rect(VTermRect rect,
    ціле downward,
    ціле rightward,
    ціле (*moverect)(VTermRect src, VTermRect dest, void *user),
    ціле (*eraserect)(VTermRect rect, ціле selective, void *user),
    void *user)
{
  VTermRect src;
  VTermRect dest;

  якщо(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    повернути;
  }

  якщо(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  інакше {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    ціле leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  якщо(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  інакше {
    ціле upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  якщо(moverect)
    (*moverect)(dest, src, user);

  якщо(downward > 0)
    rect.start_row = rect.end_row - downward;
  інакше якщо(downward < 0)
    rect.end_row = rect.start_row - downward;

  якщо(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  інакше якщо(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

void vterm_copy_cells(VTermRect dest,
    VTermRect src,
    void (*copycell)(VTermPos dest, VTermPos src, void *user),
    void *user)
{
  ціле downward  = src.start_row - dest.start_row;
  ціле rightward = src.start_col - dest.start_col;

  ціле init_row, test_row, init_col, test_col;
  ціле inc_row, inc_col;

  якщо(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  інакше /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  якщо(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  інакше /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  для(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    для(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

void vterm_check_version(ціле major, ціле minor)
{
  якщо(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  якщо(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
