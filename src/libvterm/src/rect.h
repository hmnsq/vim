/*
 * Some utility functions on VTermRect structures
 */

#define STRFrect "(%d,%d-%d,%d)"
#define ARGSrect(r) (r).start_row, (r).start_col, (r).end_row, (r).end_col

/* Expand dst to contain src as well */
static void rect_expand(VTermRect *dst, VTermRect *src)
{
  如果(dst->start_row > src->start_row) dst->start_row = src->start_row;
  如果(dst->start_col > src->start_col) dst->start_col = src->start_col;
  如果(dst->end_row   < src->end_row)   dst->end_row   = src->end_row;
  如果(dst->end_col   < src->end_col)   dst->end_col   = src->end_col;
}

/* Clip the dst to ensure it does not step outside of bounds */
static void rect_clip(VTermRect *dst, VTermRect *bounds)
{
  如果(dst->start_row < bounds->start_row) dst->start_row = bounds->start_row;
  如果(dst->start_col < bounds->start_col) dst->start_col = bounds->start_col;
  如果(dst->end_row   > bounds->end_row)   dst->end_row   = bounds->end_row;
  如果(dst->end_col   > bounds->end_col)   dst->end_col   = bounds->end_col;
  /* Ensure it doesn't end up negatively-sized */
  如果(dst->end_row < dst->start_row) dst->end_row = dst->start_row;
  如果(dst->end_col < dst->start_col) dst->end_col = dst->start_col;
}

/* True if the two rectangles are equal */
static 整数 rect_equal(VTermRect *a, VTermRect *b)
{
  返回 (a->start_row == b->start_row) &&
         (a->start_col == b->start_col) &&
         (a->end_row   == b->end_row)   &&
         (a->end_col   == b->end_col);
}

/* True if small is contained entirely within big */
static 整数 rect_contains(VTermRect *big, VTermRect *small)
{
  如果(small->start_row < big->start_row) 返回 0;
  如果(small->start_col < big->start_col) 返回 0;
  如果(small->end_row   > big->end_row)   返回 0;
  如果(small->end_col   > big->end_col)   返回 0;
  返回 1;
}

/* True if the rectangles overlap at all */
static 整数 rect_intersects(VTermRect *a, VTermRect *b)
{
  如果(a->start_row > b->end_row || b->start_row > a->end_row)
    返回 0;
  如果(a->start_col > b->end_col || b->start_col > a->end_col)
    返回 0;
  返回 1;
}
