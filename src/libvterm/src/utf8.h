/* The following functions copied and adapted from libtermkey
 *
 * http://www.leonerd.org.uk/code/libtermkey/
 */
अचिन्हित पूर्णाङ्क utf8_seqlen(दीर्घ codepoint);

#यदि defined(DEFINE_INLINES) || USE_INLINE
INLINE अचिन्हित पूर्णाङ्क utf8_seqlen(दीर्घ codepoint)
{
  यदि(codepoint < 0x0000080) फर्काऊ 1;
  यदि(codepoint < 0x0000800) फर्काऊ 2;
  यदि(codepoint < 0x0010000) फर्काऊ 3;
  यदि(codepoint < 0x0200000) फर्काऊ 4;
  यदि(codepoint < 0x4000000) फर्काऊ 5;
  फर्काऊ 6;
}
#endif

/* Does NOT NUL-terminate the buffer */
पूर्णाङ्क fill_utf8(दीर्घ codepoint, वर्ण *str);

#यदि defined(DEFINE_INLINES) || USE_INLINE
INLINE पूर्णाङ्क fill_utf8(दीर्घ codepoint, वर्ण *str)
{
  पूर्णाङ्क nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  पूर्णाङ्क b = nbytes;
  जबसम्म(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  विकल्प(nbytes) {
    पक्ष 1: str[0] =        (codepoint & 0x7f); रोक;
    पक्ष 2: str[0] = 0xc0 | (codepoint & 0x1f); रोक;
    पक्ष 3: str[0] = 0xe0 | (codepoint & 0x0f); रोक;
    पक्ष 4: str[0] = 0xf0 | (codepoint & 0x07); रोक;
    पक्ष 5: str[0] = 0xf8 | (codepoint & 0x03); रोक;
    पक्ष 6: str[0] = 0xfc | (codepoint & 0x01); रोक;
  }

  फर्काऊ nbytes;
}
#endif
/* end copy */
