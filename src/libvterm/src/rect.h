/*
 * Some utility functions on VTermRect structures
 */

#تعریف_کرو STRFrect "(%d,%d-%d,%d)"
#تعریف_کرو ARGSrect(r) (r).شروع_صف, (r).شروع_خانہ, (r).سرا_صف, (r).سرا_خانہ

/* Expand dst to contain src as well */
ساكن فراغ rect_expand(VTermRect *dst, VTermRect *src)
{
  إذا(dst->شروع_صف > src->شروع_صف) dst->شروع_صف = src->شروع_صف;
  إذا(dst->شروع_خانہ > src->شروع_خانہ) dst->شروع_خانہ = src->شروع_خانہ;
  إذا(dst->سرا_صف   < src->سرا_صف)   dst->سرا_صف   = src->سرا_صف;
  إذا(dst->سرا_خانہ   < src->سرا_خانہ)   dst->سرا_خانہ   = src->سرا_خانہ;
}

/* Clip the dst to ensure it does not step outside of bounds */
ساكن فراغ rect_clip(VTermRect *dst, VTermRect *bounds)
{
  إذا(dst->شروع_صف < bounds->شروع_صف) dst->شروع_صف = bounds->شروع_صف;
  إذا(dst->شروع_خانہ < bounds->شروع_خانہ) dst->شروع_خانہ = bounds->شروع_خانہ;
  إذا(dst->سرا_صف   > bounds->سرا_صف)   dst->سرا_صف   = bounds->سرا_صف;
  إذا(dst->سرا_خانہ   > bounds->سرا_خانہ)   dst->سرا_خانہ   = bounds->سرا_خانہ;
  /* Ensure it doesn't end up negatively-sized */
  إذا(dst->سرا_صف < dst->شروع_صف) dst->سرا_صف = dst->شروع_صف;
  إذا(dst->سرا_خانہ < dst->شروع_خانہ) dst->سرا_خانہ = dst->شروع_خانہ;
}

/* True if the two rectangles are equal */
ساكن صحيح rect_equal(VTermRect *a, VTermRect *b)
{
  أرجع (a->شروع_صف == b->شروع_صف) &&
         (a->شروع_خانہ == b->شروع_خانہ) &&
         (a->سرا_صف   == b->سرا_صف)   &&
         (a->سرا_خانہ   == b->سرا_خانہ);
}

/* True if small is contained entirely within big */
ساكن صحيح rect_شامل_اے(VTermRect *big, VTermRect *small)
{
  إذا(small->شروع_صف < big->شروع_صف) أرجع 0;
  إذا(small->شروع_خانہ < big->شروع_خانہ) أرجع 0;
  إذا(small->سرا_صف   > big->سرا_صف)   أرجع 0;
  إذا(small->سرا_خانہ   > big->سرا_خانہ)   أرجع 0;
  أرجع 1;
}

/* True if the rectangles overlap at all */
ساكن صحيح rect_intersects(VTermRect *a, VTermRect *b)
{
  إذا(a->شروع_صف > b->سرا_صف || b->شروع_صف > a->سرا_صف)
    أرجع 0;
  إذا(a->شروع_خانہ > b->سرا_خانہ || b->شروع_خانہ > a->سرا_خانہ)
    أرجع 0;
  أرجع 1;
}
