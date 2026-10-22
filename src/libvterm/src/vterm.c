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
  もし(ptr)
    memset(ptr, 0, size);
  戻る ptr;
}

static void default_free(void *ptr, void *allocdata UNUSED)
{
  free(ptr);
}

static VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(整数 rows, 整数 cols)
{
  struct VTermBuilder builder;
  memset(&builder, 0, sizeof(builder));
  builder.rows = rows;
  builder.cols = cols;
  戻る vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(整数 rows, 整数 cols, VTermAllocatorFunctions *funcs, void *allocdata)
{
  struct VTermBuilder builder;
  memset(&builder, 0, sizeof(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  戻る vterm_build(&builder);
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

  もし (vt->tmpbuffer == NULL
      || vt->outbuffer == NULL
      || vt->tmpbuffer == NULL)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    戻る NULL;
  }

  戻る vt;
}

void vterm_free(VTerm *vt)
{
  もし(vt->screen)
    vterm_screen_free(vt->screen);

  もし(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL void *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  戻る (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL void vterm_allocator_free(VTerm *vt, void *ptr)
{
  もし (ptr)
    (*vt->allocator->free)(ptr, vt->allocdata);
}

void vterm_get_size(const VTerm *vt, 整数 *rowsp, 整数 *colsp)
{
  もし(rowsp)
    *rowsp = vt->rows;
  もし(colsp)
    *colsp = vt->cols;
}

void vterm_set_size(VTerm *vt, 整数 rows, 整数 cols)
{
  もし(rows < 1 || cols < 1)
    戻る;

  vt->rows = rows;
  vt->cols = cols;

  もし(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

整数 vterm_get_utf8(const VTerm *vt)
{
  戻る vt->mode.utf8;
}

void vterm_set_utf8(VTerm *vt, 整数 is_utf8)
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
  もし(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    戻る;
  }

  もし(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    戻る;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL void vterm_push_output_vsprintf(VTerm *vt, const char *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#否则
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  もし (len >= vt->tmpbuffer_len)
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

  もし(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  否则
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  もし(cur >= vt->tmpbuffer_len)
    戻る;
  vterm_push_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL void vterm_push_output_sprintf_str(VTerm *vt, unsigned char ctrl, 整数 term, const char *fmt, ...)
{
  size_t cur;

  もし(ctrl) {
    もし(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    否则
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    もし(cur >= vt->tmpbuffer_len)
      戻る;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);

  もし(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    もし(cur >= vt->tmpbuffer_len)
      戻る;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(const VTerm *vt)
{
  戻る vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(const VTerm *vt)
{
  戻る vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(const VTerm *vt)
{
  戻る vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, char *buffer, size_t len)
{
  もし(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  もし(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  戻る len;
}

VTermValueType vterm_get_attr_type(VTermAttr attr)
{
  switch(attr) {
    case VTERM_ATTR_BOLD:       戻る VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_UNDERLINE:  戻る VTERM_VALUETYPE_INT;
    case VTERM_ATTR_ITALIC:     戻る VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_BLINK:      戻る VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_REVERSE:    戻る VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_CONCEAL:    戻る VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_STRIKE:     戻る VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_FONT:       戻る VTERM_VALUETYPE_INT;
    case VTERM_ATTR_FOREGROUND: 戻る VTERM_VALUETYPE_COLOR;
    case VTERM_ATTR_BACKGROUND: 戻る VTERM_VALUETYPE_COLOR;
    case VTERM_ATTR_SMALL:      戻る VTERM_VALUETYPE_BOOL;
    case VTERM_ATTR_BASELINE:   戻る VTERM_VALUETYPE_INT;

    case VTERM_N_ATTRS: 戻る 0;
  }
  戻る 0; /* UNREACHABLE */
}

VTermValueType vterm_get_prop_type(VTermProp prop)
{
  switch(prop) {
    case VTERM_PROP_CURSORVISIBLE: 戻る VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_CURSORBLINK:   戻る VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_ALTSCREEN:     戻る VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_TITLE:         戻る VTERM_VALUETYPE_STRING;
    case VTERM_PROP_ICONNAME:      戻る VTERM_VALUETYPE_STRING;
    case VTERM_PROP_REVERSE:       戻る VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_CURSORSHAPE:   戻る VTERM_VALUETYPE_INT;
    case VTERM_PROP_MOUSE:         戻る VTERM_VALUETYPE_INT;
    case VTERM_PROP_FOCUSREPORT:   戻る VTERM_VALUETYPE_BOOL;
    case VTERM_PROP_CURSORCOLOR:   戻る VTERM_VALUETYPE_STRING;

    case VTERM_N_PROPS: 戻る 0;
  }
  戻る 0; /* UNREACHABLE */
}

void vterm_scroll_rect(VTermRect rect,
    整数 downward,
    整数 rightward,
    整数 (*moverect)(VTermRect src, VTermRect dest, void *user),
    整数 (*eraserect)(VTermRect rect, 整数 selective, void *user),
    void *user)
{
  VTermRect src;
  VTermRect dest;

  もし(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    戻る;
  }

  もし(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  否则 {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    整数 leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  もし(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  否则 {
    整数 upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  もし(moverect)
    (*moverect)(dest, src, user);

  もし(downward > 0)
    rect.start_row = rect.end_row - downward;
  否则 もし(downward < 0)
    rect.end_row = rect.start_row - downward;

  もし(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  否则 もし(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

void vterm_copy_cells(VTermRect dest,
    VTermRect src,
    void (*copycell)(VTermPos dest, VTermPos src, void *user),
    void *user)
{
  整数 downward  = src.start_row - dest.start_row;
  整数 rightward = src.start_col - dest.start_col;

  整数 init_row, test_row, init_col, test_col;
  整数 inc_row, inc_col;

  もし(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  否则 /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  もし(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  否则 /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  ために(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    ために(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

void vterm_check_version(整数 major, 整数 minor)
{
  もし(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  もし(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
