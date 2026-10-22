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
  если(ptr)
    memset(ptr, 0, size);
  вернуть ptr;
}

static void default_free(void *ptr, void *allocdata UNUSED)
{
  free(ptr);
}

static VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(целое rows, целое cols)
{
  struct VTermBuilder builder;
  memset(&builder, 0, sizeof(builder));
  builder.rows = rows;
  builder.cols = cols;
  вернуть vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(целое rows, целое cols, VTermAllocatorFunctions *funcs, void *allocdata)
{
  struct VTermBuilder builder;
  memset(&builder, 0, sizeof(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  вернуть vterm_build(&builder);
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

  если (vt->tmpbuffer == NULL
      || vt->outbuffer == NULL
      || vt->tmpbuffer == NULL)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    вернуть NULL;
  }

  вернуть vt;
}

void vterm_free(VTerm *vt)
{
  если(vt->screen)
    vterm_screen_free(vt->screen);

  если(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL void *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  вернуть (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL void vterm_allocator_free(VTerm *vt, void *ptr)
{
  если (ptr)
    (*vt->allocator->free)(ptr, vt->allocdata);
}

void vterm_get_size(const VTerm *vt, целое *rowsp, целое *colsp)
{
  если(rowsp)
    *rowsp = vt->rows;
  если(colsp)
    *colsp = vt->cols;
}

void vterm_set_size(VTerm *vt, целое rows, целое cols)
{
  если(rows < 1 || cols < 1)
    вернуть;

  vt->rows = rows;
  vt->cols = cols;

  если(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

целое vterm_get_utf8(const VTerm *vt)
{
  вернуть vt->mode.utf8;
}

void vterm_set_utf8(VTerm *vt, целое is_utf8)
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
  если(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    вернуть;
  }

  если(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    вернуть;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL void vterm_push_output_vsprintf(VTerm *vt, const char *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#иначе
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  если (len >= vt->tmpbuffer_len)
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

  если(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  иначе
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  если(cur >= vt->tmpbuffer_len)
    вернуть;
  vterm_push_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL void vterm_push_output_sprintf_str(VTerm *vt, unsigned char ctrl, целое term, const char *fmt, ...)
{
  size_t cur;

  если(ctrl) {
    если(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    иначе
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    если(cur >= vt->tmpbuffer_len)
      вернуть;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);

  если(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    если(cur >= vt->tmpbuffer_len)
      вернуть;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(const VTerm *vt)
{
  вернуть vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(const VTerm *vt)
{
  вернуть vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(const VTerm *vt)
{
  вернуть vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, char *buffer, size_t len)
{
  если(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  если(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  вернуть len;
}

VTermValueType vterm_get_attr_type(VTermAttr attr)
{
  switch(attr) {
    case VTERM_ATTR_BOLD:       вернуть VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_UNDERLINE:  вернуть VTERM_VALUETYPE_INT;
    case VTERM_ATTR_ITALIC:     вернуть VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_BLINK:      вернуть VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_REVERSE:    вернуть VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_CONCEAL:    вернуть VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_STRIKE:     вернуть VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_FONT:       вернуть VTERM_VALUETYPE_INT;
    case VTERM_ATTR_FOREGROUND: вернуть VTERM_VALUETYPE_COLOR;
    case VTERM_ATTR_BACKGROUND: вернуть VTERM_VALUETYPE_COLOR;
    case VTERM_ATTR_SMALL:      вернуть VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_BASELINE:   вернуть VTERM_VALUETYPE_INT;

    case VTERM_N_ATTRS: вернуть 0;
  }
  вернуть 0; /* UNREACHABLE */
}

VTermValueType vterm_get_prop_type(VTermProp prop)
{
  switch(prop) {
    case VTERM_PROP_CURSORVISIBLE: вернуть VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_CURSORBLINK:   вернуть VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_ALTSCREEN:     вернуть VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_TITLE:         вернуть VTERM_VALUETYPE_STRING;
    case VTERM_PROP_ICONNAME:      вернуть VTERM_VALUETYPE_STRING;
    case VTERM_PROP_REVERSE:       вернуть VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_CURSORSHAPE:   вернуть VTERM_VALUETYPE_INT;
    case VTERM_PROP_MOUSE:         вернуть VTERM_VALUETYPE_INT;
    case VTERM_PROP_FOCUSREPORT:   вернуть VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_CURSORCOLOR:   вернуть VTERM_VALUETYPE_STRING;

    case VTERM_N_PROPS: вернуть 0;
  }
  вернуть 0; /* UNREACHABLE */
}

void vterm_scroll_rect(VTermRect rect,
    целое downward,
    целое rightward,
    целое (*moverect)(VTermRect src, VTermRect dest, void *user),
    целое (*eraserect)(VTermRect rect, целое selective, void *user),
    void *user)
{
  VTermRect src;
  VTermRect dest;

  если(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    вернуть;
  }

  если(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  иначе {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    целое leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  если(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  иначе {
    целое upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  если(moverect)
    (*moverect)(dest, src, user);

  если(downward > 0)
    rect.start_row = rect.end_row - downward;
  иначе если(downward < 0)
    rect.end_row = rect.start_row - downward;

  если(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  иначе если(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

void vterm_copy_cells(VTermRect dest,
    VTermRect src,
    void (*copycell)(VTermPos dest, VTermPos src, void *user),
    void *user)
{
  целое downward  = src.start_row - dest.start_row;
  целое rightward = src.start_col - dest.start_col;

  целое init_row, test_row, init_col, test_col;
  целое inc_row, inc_col;

  если(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  иначе /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  если(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  иначе /* rightward >= 0 */ {
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

void vterm_check_version(целое major, целое minor)
{
  если(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  если(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
