/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned صحيح utf8_seqlen(long codepoint);

#אם defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned صحيح utf8_seqlen(long codepoint)
{
  אם(codepoint < 0x0000080) החזר 1;
  אם(codepoint < 0x0000800) החזר 2;
  אם(codepoint < 0x0010000) החזר 3;
  אם(codepoint < 0x0200000) החזר 4;
  אם(codepoint < 0x4000000) החזר 5;
  החזר 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
صحيح fill_utf8(long codepoint, حرف *str);

#אם defined(DEFINE_INLINES) || USE_INLINE
INLINE صحيح fill_utf8(long codepoint, حرف *str)
{
  صحيح nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  صحيح b = nbytes;
  כלעוד(b > 1) {
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

  החזר nbytes;
}
#endif
/* end copy */
