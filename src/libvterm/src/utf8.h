/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
غیرعلامتی صحیح utf8_seqlen(طویل codepoint);

#اگر defined(DEFINE_INLINES) || USE_INLINE
INLINE غیرعلامتی صحیح utf8_seqlen(طویل codepoint)
{
  اگر(codepoint < 0x0000080) واپس 1;
  اگر(codepoint < 0x0000800) واپس 2;
  اگر(codepoint < 0x0010000) واپس 3;
  اگر(codepoint < 0x0200000) واپس 4;
  اگر(codepoint < 0x4000000) واپس 5;
  واپس 6;
}
#اگر_ختم

/* Does NOT NUL-terminate the buffer */
صحیح fill_utf8(طویل codepoint, حرف *str);

#اگر defined(DEFINE_INLINES) || USE_INLINE
INLINE صحیح fill_utf8(طویل codepoint, حرف *str)
{
  صحیح nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  صحیح b = nbytes;
  جب(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  انتخاب(nbytes) {
    حالت 1: str[0] =        (codepoint & 0x7f); توڑو;
    حالت 2: str[0] = 0xc0 | (codepoint & 0x1f); توڑو;
    حالت 3: str[0] = 0xe0 | (codepoint & 0x0f); توڑو;
    حالت 4: str[0] = 0xf0 | (codepoint & 0x07); توڑو;
    حالت 5: str[0] = 0xf8 | (codepoint & 0x03); توڑو;
    حالت 6: str[0] = 0xfc | (codepoint & 0x01); توڑو;
  }

  واپس nbytes;
}
#اگر_ختم
/* end copy */
