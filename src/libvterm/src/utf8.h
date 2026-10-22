/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
unsigned முழு utf8_seqlen(long codepoint);

#எனில் defined(DEFINE_INLINES) || USE_INLINE
INLINE unsigned முழு utf8_seqlen(long codepoint)
{
  எனில்(codepoint < 0x0000080) திரும்பு 1;
  எனில்(codepoint < 0x0000800) திரும்பு 2;
  எனில்(codepoint < 0x0010000) திரும்பு 3;
  எனில்(codepoint < 0x0200000) திரும்பு 4;
  எனில்(codepoint < 0x4000000) திரும்பு 5;
  திரும்பு 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
முழு fill_utf8(long codepoint, எழுத்து *str);

#எனில் defined(DEFINE_INLINES) || USE_INLINE
INLINE முழு fill_utf8(long codepoint, எழுத்து *str)
{
  முழு nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  முழு b = nbytes;
  போது(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  தேர்வு(nbytes) {
    நிலை 1: str[0] =        (codepoint & 0x7f); நிறுத்து;
    நிலை 2: str[0] = 0xc0 | (codepoint & 0x1f); நிறுத்து;
    நிலை 3: str[0] = 0xe0 | (codepoint & 0x0f); நிறுத்து;
    நிலை 4: str[0] = 0xf0 | (codepoint & 0x07); நிறுத்து;
    நிலை 5: str[0] = 0xf8 | (codepoint & 0x03); நிறுத்து;
    நிலை 6: str[0] = 0xfc | (codepoint & 0x01); நிறுத்து;
  }

  திரும்பு nbytes;
}
#endif
/* end copy */
