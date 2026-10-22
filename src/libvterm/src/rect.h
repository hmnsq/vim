/*
 * Some utility functions on VTermRect structures
 */

#define STRFrect "(%d,%d-%d,%d)"
#define ARGSrect(r) (r).start_row, (r).start_col, (r).end_row, (r).end_col

/* Expand dst to contain src as well */
static void rect_expand(VTermRect *dst, VTermRect *src)
{
  ຖ້າ(dst->start_row > src->start_row) dst->start_row = src->start_row;
  ຖ້າ(dst->start_col > src->start_col) dst->start_col = src->start_col;
  ຖ້າ(dst->end_row   < src->end_row)   dst->end_row   = src->end_row;
  ຖ້າ(dst->end_col   < src->end_col)   dst->end_col   = src->end_col;
}

/* Clip the dst to ensure it does not step outside of bounds */
static void rect_clip(VTermRect *dst, VTermRect *bounds)
{
  ຖ້າ(dst->start_row < bounds->start_row) dst->start_row = bounds->start_row;
  ຖ້າ(dst->start_col < bounds->start_col) dst->start_col = bounds->start_col;
  ຖ້າ(dst->end_row   > bounds->end_row)   dst->end_row   = bounds->end_row;
  ຖ້າ(dst->end_col   > bounds->end_col)   dst->end_col   = bounds->end_col;
  /* Ensure it doesn't end up negatively-sized */
  ຖ້າ(dst->end_row < dst->start_row) dst->end_row = dst->start_row;
  ຖ້າ(dst->end_col < dst->start_col) dst->end_col = dst->start_col;
}

/* True if the two rectangles are equal */
static ຈຳນວນເຕັມ rect_equal(VTermRect *a, VTermRect *b)
{
  ສົ່ງຄືນ (a->start_row == b->start_row) &&
         (a->start_col == b->start_col) &&
         (a->end_row   == b->end_row)   &&
         (a->end_col   == b->end_col);
}

/* True if small is contained entirely within big */
static ຈຳນວນເຕັມ rect_contains(VTermRect *big, VTermRect *small)
{
  ຖ້າ(small->start_row < big->start_row) ສົ່ງຄືນ 0;
  ຖ້າ(small->start_col < big->start_col) ສົ່ງຄືນ 0;
  ຖ້າ(small->end_row   > big->end_row)   ສົ່ງຄືນ 0;
  ຖ້າ(small->end_col   > big->end_col)   ສົ່ງຄືນ 0;
  ສົ່ງຄືນ 1;
}

/* True if the rectangles overlap at all */
static ຈຳນວນເຕັມ rect_intersects(VTermRect *a, VTermRect *b)
{
  ຖ້າ(a->start_row > b->end_row || b->start_row > a->end_row)
    ສົ່ງຄືນ 0;
  ຖ້າ(a->start_col > b->end_col || b->start_col > a->end_col)
    ສົ່ງຄືນ 0;
  ສົ່ງຄືນ 1;
}
