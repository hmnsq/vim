/*
 * Some utility functions on VTermRect structures
 */

#define STRFrect "(%d,%d-%d,%d)"
#define ARGSrect(r) (r).start_row, (r).start_col, (r).end_row, (r).end_col

/* Expand dst to contain src as well */
static void rect_expand(VTermRect *dst, VTermRect *src)
{
  если(dst->start_row > src->start_row) dst->start_row = src->start_row;
  если(dst->start_col > src->start_col) dst->start_col = src->start_col;
  если(dst->end_row   < src->end_row)   dst->end_row   = src->end_row;
  если(dst->end_col   < src->end_col)   dst->end_col   = src->end_col;
}

/* Clip the dst to ensure it does not step outside of bounds */
static void rect_clip(VTermRect *dst, VTermRect *bounds)
{
  если(dst->start_row < bounds->start_row) dst->start_row = bounds->start_row;
  если(dst->start_col < bounds->start_col) dst->start_col = bounds->start_col;
  если(dst->end_row   > bounds->end_row)   dst->end_row   = bounds->end_row;
  если(dst->end_col   > bounds->end_col)   dst->end_col   = bounds->end_col;
  /* Ensure it doesn't end up negatively-sized */
  если(dst->end_row < dst->start_row) dst->end_row = dst->start_row;
  если(dst->end_col < dst->start_col) dst->end_col = dst->start_col;
}

/* True if the two rectangles are equal */
static целое rect_equal(VTermRect *a, VTermRect *b)
{
  вернуть (a->start_row == b->start_row) &&
         (a->start_col == b->start_col) &&
         (a->end_row   == b->end_row)   &&
         (a->end_col   == b->end_col);
}

/* True if small is contained entirely within big */
static целое rect_contains(VTermRect *big, VTermRect *small)
{
  если(small->start_row < big->start_row) вернуть 0;
  если(small->start_col < big->start_col) вернуть 0;
  если(small->end_row   > big->end_row)   вернуть 0;
  если(small->end_col   > big->end_col)   вернуть 0;
  вернуть 1;
}

/* True if the rectangles overlap at all */
static целое rect_intersects(VTermRect *a, VTermRect *b)
{
  если(a->start_row > b->end_row || b->start_row > a->end_row)
    вернуть 0;
  если(a->start_col > b->end_col || b->start_col > a->end_col)
    вернуть 0;
  вернуть 1;
}
