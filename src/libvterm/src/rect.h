/*
 * Some utility functions on VTermRect structures
 */

#define STRFrect "(%d,%d-%d,%d)"
#define ARGSrect(r) (r).start_row, (r).start_col, (r).end_row, (r).end_col

/* Expand dst to contain src as well */
static void rect_expand(VTermRect *dst, VTermRect *src)
{
  ถ้า(dst->start_row > src->start_row) dst->start_row = src->start_row;
  ถ้า(dst->start_col > src->start_col) dst->start_col = src->start_col;
  ถ้า(dst->end_row   < src->end_row)   dst->end_row   = src->end_row;
  ถ้า(dst->end_col   < src->end_col)   dst->end_col   = src->end_col;
}

/* Clip the dst to ensure it does not step outside of bounds */
static void rect_clip(VTermRect *dst, VTermRect *bounds)
{
  ถ้า(dst->start_row < bounds->start_row) dst->start_row = bounds->start_row;
  ถ้า(dst->start_col < bounds->start_col) dst->start_col = bounds->start_col;
  ถ้า(dst->end_row   > bounds->end_row)   dst->end_row   = bounds->end_row;
  ถ้า(dst->end_col   > bounds->end_col)   dst->end_col   = bounds->end_col;
  /* Ensure it doesn't end up negatively-sized */
  ถ้า(dst->end_row < dst->start_row) dst->end_row = dst->start_row;
  ถ้า(dst->end_col < dst->start_col) dst->end_col = dst->start_col;
}

/* True if the two rectangles are equal */
static จำนวนเต็ม rect_equal(VTermRect *a, VTermRect *b)
{
  คืน (a->start_row == b->start_row) &&
         (a->start_col == b->start_col) &&
         (a->end_row   == b->end_row)   &&
         (a->end_col   == b->end_col);
}

/* True if small is contained entirely within big */
static จำนวนเต็ม rect_contains(VTermRect *big, VTermRect *small)
{
  ถ้า(small->start_row < big->start_row) คืน 0;
  ถ้า(small->start_col < big->start_col) คืน 0;
  ถ้า(small->end_row   > big->end_row)   คืน 0;
  ถ้า(small->end_col   > big->end_col)   คืน 0;
  คืน 1;
}

/* True if the rectangles overlap at all */
static จำนวนเต็ม rect_intersects(VTermRect *a, VTermRect *b)
{
  ถ้า(a->start_row > b->end_row || b->start_row > a->end_row)
    คืน 0;
  ถ้า(a->start_col > b->end_col || b->start_col > a->end_col)
    คืน 0;
  คืน 1;
}
