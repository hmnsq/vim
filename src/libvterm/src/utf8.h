/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned صحيح utf8_seqlen(long codepoint);

#إذا defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned صحيح utf8_seqlen(long codepoint)
{
  إذا(codepoint < 0x0000080) إرجاع 1;
  إذا(codepoint < 0x0000800) إرجاع 2;
  إذا(codepoint < 0x0010000) إرجاع 3;
  إذا(codepoint < 0x0200000) إرجاع 4;
  إذا(codepoint < 0x4000000) إرجاع 5;
  إرجاع 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
صحيح fill_utf8(long codepoint, حرف *str);

#إذا defined(DEFINE_INLINES) || USE_INLINE
INLINE صحيح fill_utf8(long codepoint, حرف *str)
{
  صحيح nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  صحيح b = nbytes;
  بينما(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  بدل(nbytes) {
    حالة 1: str[0] =        (codepoint & 0x7f); اكسر;
    حالة 2: str[0] = 0xc0 | (codepoint & 0x1f); اكسر;
    حالة 3: str[0] = 0xe0 | (codepoint & 0x0f); اكسر;
    حالة 4: str[0] = 0xf0 | (codepoint & 0x07); اكسر;
    حالة 5: str[0] = 0xf8 | (codepoint & 0x03); اكسر;
    حالة 6: str[0] = 0xfc | (codepoint & 0x01); اكسر;
  }

  إرجاع nbytes;
}
#endif
/* end copy */
