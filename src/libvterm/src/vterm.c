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
  ប្រសិនបើ(ptr)
    memset(ptr, 0, size);
  ត្រឡប់ ptr;
}

static void default_free(void *ptr, void *allocdata UNUSED)
{
  free(ptr);
}

static VTermAllocatorFunctions default_allocator = {
  &default_malloc, // malloc
  &default_free // free
};

VTerm *vterm_new(ចំនួនគត់ rows, ចំនួនគត់ cols)
{
  sa_nracanaa VTermBuilder builder;
  memset(&builder, 0, parimaa_na(builder));
  builder.rows = rows;
  builder.cols = cols;
  ត្រឡប់ vterm_build(&builder);
}

VTerm *vterm_new_with_allocator(ចំនួនគត់ rows, ចំនួនគត់ cols, VTermAllocatorFunctions *funcs, void *allocdata)
{
  sa_nracanaa VTermBuilder builder;
  memset(&builder, 0, parimaa_na(builder));
  builder.rows = rows;
  builder.cols = cols;
  builder.allocator = funcs;
  builder.allocdata = allocdata;
  ត្រឡប់ vterm_build(&builder);
}

/* A handy macro for defaulting values out of builder fields */
#define DEFAULT(v, def)  ((v) ? (v) : (def))

VTerm *vterm_build(const sa_nracanaa VTermBuilder *builder)
{
  const VTermAllocatorFunctions *allocator = DEFAULT(builder->allocator, &default_allocator);

  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*allocator->malloc)(parimaa_na(VTerm), builder->allocdata);

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

  ប្រសិនបើ (vt->tmpbuffer == NULL
      || vt->outbuffer == NULL
      || vt->tmpbuffer == NULL)
  {
    vterm_allocator_free(vt, vt->outbuffer);
    vterm_allocator_free(vt, vt->tmpbuffer);
    vterm_allocator_free(vt, vt);
    ត្រឡប់ NULL;
  }

  ត្រឡប់ vt;
}

void vterm_free(VTerm *vt)
{
  ប្រសិនបើ(vt->screen)
    vterm_screen_free(vt->screen);

  ប្រសិនបើ(vt->state)
    vterm_state_free(vt->state);

  vterm_allocator_free(vt, vt->outbuffer);
  vterm_allocator_free(vt, vt->tmpbuffer);

  vterm_allocator_free(vt, vt);
}

INTERNAL void *vterm_allocator_malloc(VTerm *vt, size_t size)
{
  ត្រឡប់ (*vt->allocator->malloc)(size, vt->allocdata);
}

/*
 * Free "ptr" unless it is NULL.
 */
INTERNAL void vterm_allocator_free(VTerm *vt, void *ptr)
{
  ប្រសិនបើ (ptr)
    (*vt->allocator->free)(ptr, vt->allocdata);
}

void vterm_get_size(const VTerm *vt, ចំនួនគត់ *rowsp, ចំនួនគត់ *colsp)
{
  ប្រសិនបើ(rowsp)
    *rowsp = vt->rows;
  ប្រសិនបើ(colsp)
    *colsp = vt->cols;
}

void vterm_set_size(VTerm *vt, ចំនួនគត់ rows, ចំនួនគត់ cols)
{
  ប្រសិនបើ(rows < 1 || cols < 1)
    ត្រឡប់;

  vt->rows = rows;
  vt->cols = cols;

  ប្រសិនបើ(vt->parser.callbacks && vt->parser.callbacks->resize)
    (*vt->parser.callbacks->resize)(rows, cols, vt->parser.cbdata);
}

ចំនួនគត់ vterm_get_utf8(const VTerm *vt)
{
  ត្រឡប់ vt->mode.utf8;
}

void vterm_set_utf8(VTerm *vt, ចំនួនគត់ is_utf8)
{
  vt->mode.utf8 = is_utf8;
}

void vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, void *user)
{
  vt->outfunc = func;
  vt->outdata = user;
}

INTERNAL void vterm_push_output_bytes(VTerm *vt, const var_na *bytes, size_t len)
{
  ប្រសិនបើ(vt->outfunc) {
    (vt->outfunc)(bytes, len, vt->outdata);
    ត្រឡប់;
  }

  ប្រសិនបើ(len > vt->outbuffer_len - vt->outbuffer_cur) {
    DEBUG_LOG("vterm_push_output_bytes(): buffer overflow; dropping output\n");
    ត្រឡប់;
  }

  memcpy(vt->outbuffer + vt->outbuffer_cur, bytes, len);
  vt->outbuffer_cur += len;
}

INTERNAL void vterm_push_output_vsprintf(VTerm *vt, const var_na *format, va_list args)
{
  size_t len;
#ifdef VSNPRINTF
  len = VSNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len, format, args);
#បើមិនដូច្នេះ
  // When vsnprintf() is not available (C90) fall back to vsprintf().
  // Use the heap-allocated tmpbuffer (default 4096 bytes) instead of a small
  // stack buffer to reduce the risk of overflow.
  len = vsprintf(vt->tmpbuffer, format, args);
  ប្រសិនបើ (len >= vt->tmpbuffer_len)
    len = vt->tmpbuffer_len - 1;
#endif
  vterm_push_output_bytes(vt, vt->tmpbuffer, len);
}

INTERNAL void vterm_push_output_sprintf(VTerm *vt, const var_na *format, ...)
{
  va_list args;
  va_start(args, format);
  vterm_push_output_vsprintf(vt, format, args);
  va_end(args);
}

INTERNAL void vterm_push_output_sprintf_ctrl(VTerm *vt, unsigned var_na ctrl, const var_na *fmt, ...)
{
  size_t cur;

  ប្រសិនបើ(ctrl >= 0x80 && !vt->mode.ctrl8bit)
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        ESC_S "%c", ctrl - 0x40);
  បើមិនដូច្នេះ
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        "%c", ctrl);
  ប្រសិនបើ(cur >= vt->tmpbuffer_len)
    ត្រឡប់;
  vterm_push_output_bytes(vt, vt->tmpbuffer, cur);

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);
}

INTERNAL void vterm_push_output_sprintf_str(VTerm *vt, unsigned var_na ctrl, ចំនួនគត់ term, const var_na *fmt, ...)
{
  size_t cur;

  ប្រសិនបើ(ctrl) {
    ប្រសិនបើ(ctrl >= 0x80 && !vt->mode.ctrl8bit)
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          ESC_S "%c", ctrl - 0x40);
    បើមិនដូច្នេះ
      cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
          "%c", ctrl);

    ប្រសិនបើ(cur >= vt->tmpbuffer_len)
      ត្រឡប់;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }

  va_list args;
  va_start(args, fmt);
  vterm_push_output_vsprintf(vt, fmt, args);
  va_end(args);

  ប្រសិនបើ(term) {
    cur = SNPRINTF(vt->tmpbuffer, vt->tmpbuffer_len,
        vt->mode.ctrl8bit ? "\x9C" : ESC_S "\\"); // ST
    ប្រសិនបើ(cur >= vt->tmpbuffer_len)
      ត្រឡប់;
    vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
  }
}

size_t vterm_output_get_buffer_size(const VTerm *vt)
{
  ត្រឡប់ vt->outbuffer_len;
}

size_t vterm_output_get_buffer_current(const VTerm *vt)
{
  ត្រឡប់ vt->outbuffer_cur;
}

size_t vterm_output_get_buffer_remaining(const VTerm *vt)
{
  ត្រឡប់ vt->outbuffer_len - vt->outbuffer_cur;
}

size_t vterm_output_read(VTerm *vt, var_na *buffer, size_t len)
{
  ប្រសិនបើ(len > vt->outbuffer_cur)
    len = vt->outbuffer_cur;

  memcpy(buffer, vt->outbuffer, len);

  ប្រសិនបើ(len < vt->outbuffer_cur)
    memmove(vt->outbuffer, vt->outbuffer + len, vt->outbuffer_cur - len);

  vt->outbuffer_cur -= len;

  ត្រឡប់ len;
}

VTermValueType vterm_get_attr_type(VTermAttr attr)
{
  switch(attr) {
    pak_sa VTERM_ATTR_BOLD:       ត្រឡប់ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_UNDERLINE:  ត្រឡប់ VTERM_VALUETYPE_INT;
    pak_sa VTERM_ATTR_ITALIC:     ត្រឡប់ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_BLINK:      ត្រឡប់ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_REVERSE:    ត្រឡប់ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_CONCEAL:    ត្រឡប់ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_STRIKE:     ត្រឡប់ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_FONT:       ត្រឡប់ VTERM_VALUETYPE_INT;
    pak_sa VTERM_ATTR_FOREGROUND: ត្រឡប់ VTERM_VALUETYPE_COLOR;
    pak_sa VTERM_ATTR_BACKGROUND: ត្រឡប់ VTERM_VALUETYPE_COLOR;
    pak_sa VTERM_ATTR_SMALL:      ត្រឡប់ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_ATTR_BASELINE:   ត្រឡប់ VTERM_VALUETYPE_INT;

    pak_sa VTERM_N_ATTRS: ត្រឡប់ 0;
  }
  ត្រឡប់ 0; /* UNREACHABLE */
}

VTermValueType vterm_get_prop_type(VTermProp prop)
{
  switch(prop) {
    pak_sa VTERM_PROP_CURSORVISIBLE: ត្រឡប់ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_CURSORBLINK:   ត្រឡប់ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_ALTSCREEN:     ត្រឡប់ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_TITLE:         ត្រឡប់ VTERM_VALUETYPE_STRING;
    pak_sa VTERM_PROP_ICONNAME:      ត្រឡប់ VTERM_VALUETYPE_STRING;
    pak_sa VTERM_PROP_REVERSE:       ត្រឡប់ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_CURSORSHAPE:   ត្រឡប់ VTERM_VALUETYPE_INT;
    pak_sa VTERM_PROP_MOUSE:         ត្រឡប់ VTERM_VALUETYPE_INT;
    pak_sa VTERM_PROP_FOCUSREPORT:   ត្រឡប់ VTERM_VALUETYPE_BOOL;
    pak_sa VTERM_PROP_CURSORCOLOR:   ត្រឡប់ VTERM_VALUETYPE_STRING;

    pak_sa VTERM_N_PROPS: ត្រឡប់ 0;
  }
  ត្រឡប់ 0; /* UNREACHABLE */
}

void vterm_scroll_rect(VTermRect rect,
    ចំនួនគត់ downward,
    ចំនួនគត់ rightward,
    ចំនួនគត់ (*moverect)(VTermRect src, VTermRect dest, void *user),
    ចំនួនគត់ (*eraserect)(VTermRect rect, ចំនួនគត់ selective, void *user),
    void *user)
{
  VTermRect src;
  VTermRect dest;

  ប្រសិនបើ(abs(downward)  >= rect.end_row - rect.start_row ||
     abs(rightward) >= rect.end_col - rect.start_col) {
    /* Scroll more than area; just erase the lot */
    (*eraserect)(rect, 0, user);
    ត្រឡប់;
  }

  ប្រសិនបើ(rightward >= 0) {
    /* rect: [XXX................]
     * src:     [----------------]
     * dest: [----------------]
     */
    dest.start_col = rect.start_col;
    dest.end_col   = rect.end_col   - rightward;
    src.start_col  = rect.start_col + rightward;
    src.end_col    = rect.end_col;
  }
  បើមិនដូច្នេះ {
    /* rect: [................XXX]
     * src:  [----------------]
     * dest:    [----------------]
     */
    ចំនួនគត់ leftward = -rightward;
    dest.start_col = rect.start_col + leftward;
    dest.end_col   = rect.end_col;
    src.start_col  = rect.start_col;
    src.end_col    = rect.end_col - leftward;
  }

  ប្រសិនបើ(downward >= 0) {
    dest.start_row = rect.start_row;
    dest.end_row   = rect.end_row   - downward;
    src.start_row  = rect.start_row + downward;
    src.end_row    = rect.end_row;
  }
  បើមិនដូច្នេះ {
    ចំនួនគត់ upward = -downward;
    dest.start_row = rect.start_row + upward;
    dest.end_row   = rect.end_row;
    src.start_row  = rect.start_row;
    src.end_row    = rect.end_row - upward;
  }

  ប្រសិនបើ(moverect)
    (*moverect)(dest, src, user);

  ប្រសិនបើ(downward > 0)
    rect.start_row = rect.end_row - downward;
  បើមិនដូច្នេះ ប្រសិនបើ(downward < 0)
    rect.end_row = rect.start_row - downward;

  ប្រសិនបើ(rightward > 0)
    rect.start_col = rect.end_col - rightward;
  បើមិនដូច្នេះ ប្រសិនបើ(rightward < 0)
    rect.end_col = rect.start_col - rightward;

  (*eraserect)(rect, 0, user);
}

void vterm_copy_cells(VTermRect dest,
    VTermRect src,
    void (*copycell)(VTermPos dest, VTermPos src, void *user),
    void *user)
{
  ចំនួនគត់ downward  = src.start_row - dest.start_row;
  ចំនួនគត់ rightward = src.start_col - dest.start_col;

  ចំនួនគត់ init_row, test_row, init_col, test_col;
  ចំនួនគត់ inc_row, inc_col;

  ប្រសិនបើ(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row = -1;
  }
  បើមិនដូច្នេះ /* downward >= 0 */ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row = +1;
  }

  ប្រសិនបើ(rightward < 0) {
    init_col = dest.end_col - 1;
    test_col = dest.start_col - 1;
    inc_col = -1;
  }
  បើមិនដូច្នេះ /* rightward >= 0 */ {
    init_col = dest.start_col;
    test_col = dest.end_col;
    inc_col = +1;
  }

  VTermPos pos;
  សម្រាប់(pos.row = init_row; pos.row != test_row; pos.row += inc_row)
    សម្រាប់(pos.col = init_col; pos.col != test_col; pos.col += inc_col) {
      VTermPos srcpos;
      srcpos.row = pos.row + downward;
      srcpos.col = pos.col + rightward;
      (*copycell)(pos, srcpos, user);
    }
}

void vterm_check_version(ចំនួនគត់ major, ចំនួនគត់ minor)
{
  ប្រសិនបើ(major != VTERM_VERSION_MAJOR) {
    fprintf(stderr, "libvterm major version mismatch; %d (wants) != %d (library)\n",
        major, VTERM_VERSION_MAJOR);
    exit(1);
  }

  ប្រសិនបើ(minor > VTERM_VERSION_MINOR) {
    fprintf(stderr, "libvterm minor version mismatch; %d (wants) > %d (library)\n",
        minor, VTERM_VERSION_MINOR);
    exit(1);
  }

  // Happy
}
