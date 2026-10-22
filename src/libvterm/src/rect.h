/*
 * Some utility functions on VTermRect structures
 */

#وصف_ڪريو STRFrect "(%d,%d-%d,%d)"
#وصف_ڪريو ARGSrect(r) (r).شروعات_صف, (r).شروعات_کانو, (r).پڇاڙي_صف, (r).پڇاڙي_کانو

/* Expand dst to contain src as well */
ساكن فراغ rect_expand(VTermRect *dst, VTermRect *src)
{
  إذا(dst->شروعات_صف > src->شروعات_صف) dst->شروعات_صف = src->شروعات_صف;
  إذا(dst->شروعات_کانو > src->شروعات_کانو) dst->شروعات_کانو = src->شروعات_کانو;
  إذا(dst->پڇاڙي_صف   < src->پڇاڙي_صف)   dst->پڇاڙي_صف   = src->پڇاڙي_صف;
  إذا(dst->پڇاڙي_کانو   < src->پڇاڙي_کانو)   dst->پڇاڙي_کانو   = src->پڇاڙي_کانو;
}

/* Clip the dst to ensure it does not step outside of bounds */
ساكن فراغ rect_clip(VTermRect *dst, VTermRect *bounds)
{
  إذا(dst->شروعات_صف < bounds->شروعات_صف) dst->شروعات_صف = bounds->شروعات_صف;
  إذا(dst->شروعات_کانو < bounds->شروعات_کانو) dst->شروعات_کانو = bounds->شروعات_کانو;
  إذا(dst->پڇاڙي_صف   > bounds->پڇاڙي_صف)   dst->پڇاڙي_صف   = bounds->پڇاڙي_صف;
  إذا(dst->پڇاڙي_کانو   > bounds->پڇاڙي_کانو)   dst->پڇاڙي_کانو   = bounds->پڇاڙي_کانو;
  /* Ensure it doesn't end up negatively-sized */
  إذا(dst->پڇاڙي_صف < dst->شروعات_صف) dst->پڇاڙي_صف = dst->شروعات_صف;
  إذا(dst->پڇاڙي_کانو < dst->شروعات_کانو) dst->پڇاڙي_کانو = dst->شروعات_کانو;
}

/* True if the two rectangles are equal */
ساكن صحيح rect_equal(VTermRect *a, VTermRect *b)
{
  أرجع (a->شروعات_صف == b->شروعات_صف) &&
         (a->شروعات_کانو == b->شروعات_کانو) &&
         (a->پڇاڙي_صف   == b->پڇاڙي_صف)   &&
         (a->پڇاڙي_کانو   == b->پڇاڙي_کانو);
}

/* True if small is contained entirely within big */
ساكن صحيح rect_شامل_آهي(VTermRect *big, VTermRect *small)
{
  إذا(small->شروعات_صف < big->شروعات_صف) أرجع 0;
  إذا(small->شروعات_کانو < big->شروعات_کانو) أرجع 0;
  إذا(small->پڇاڙي_صف   > big->پڇاڙي_صف)   أرجع 0;
  إذا(small->پڇاڙي_کانو   > big->پڇاڙي_کانو)   أرجع 0;
  أرجع 1;
}

/* True if the rectangles overlap at all */
ساكن صحيح rect_intersects(VTermRect *a, VTermRect *b)
{
  إذا(a->شروعات_صف > b->پڇاڙي_صف || b->شروعات_صف > a->پڇاڙي_صف)
    أرجع 0;
  إذا(a->شروعات_کانو > b->پڇاڙي_کانو || b->شروعات_کانو > a->پڇاڙي_کانو)
    أرجع 0;
  أرجع 1;
}
