/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned পূর্ণাঙ্ক utf8_seqlen(long codepoint);

#যদি defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned পূর্ণাঙ্ক utf8_seqlen(long codepoint)
{
  যদি(codepoint < 0x0000080) ঘূৰাই 1;
  যদি(codepoint < 0x0000800) ঘূৰাই 2;
  যদি(codepoint < 0x0010000) ঘূৰাই 3;
  যদি(codepoint < 0x0200000) ঘূৰাই 4;
  যদি(codepoint < 0x4000000) ঘূৰাই 5;
  ঘূৰাই 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
পূর্ণাঙ্ক fill_utf8(long codepoint, ৱর্ণ *str);

#যদি defined(DEFINE_INLINES) || USE_INLINE
INLINE পূর্ণাঙ্ক fill_utf8(long codepoint, ৱর্ণ *str)
{
  পূর্ণাঙ্ক nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  পূর্ণাঙ্ক b = nbytes;
  যেতিয়ালৈ(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  ৱিকল্প(nbytes) {
    পক্ষ 1: str[0] =        (codepoint & 0x7f); ভঙ্গ;
    পক্ষ 2: str[0] = 0xc0 | (codepoint & 0x1f); ভঙ্গ;
    পক্ষ 3: str[0] = 0xe0 | (codepoint & 0x0f); ভঙ্গ;
    পক্ষ 4: str[0] = 0xf0 | (codepoint & 0x07); ভঙ্গ;
    পক্ষ 5: str[0] = 0xf8 | (codepoint & 0x03); ভঙ্গ;
    পক্ষ 6: str[0] = 0xfc | (codepoint & 0x01); ভঙ্গ;
  }

  ঘূৰাই nbytes;
}
#endif
/* end copy */
