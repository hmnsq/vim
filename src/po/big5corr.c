/*
 * Simplistic program to correct Big5 inside strings.  When a trail byte is a
 * backslash it needs to be doubled.
 * Public domain.
 */

/*
 * 06.11.23, added by Restorer:
 * For more details, see:
 * https://github.com/vim/vim/pull/3261
 * https://github.com/vim/vim/pull/3476
 * https://github.com/vim/vim/pull/12153
 * (read all comments)
 *
 * I checked the workability on the list of backslash characters
 * specified in zh_TW.UTF-8.po. It works.
 * But it is better to have someone native speaker check it.
 *
 */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

	पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण **argv)
{
	वर्ण buffer[बफ_मान];
	वर्ण *p;

	जबसम्म (ख_माला_लो(buffer, बफ_मान, मानक_निवेश) != रिक्त)
	{
		लागि (p = buffer; *p != 0; p++)
		{
			यदि (म_भेदन(p, "charset=utf-8", 13) == 0
				|| म_भेदन(p, "charset=UTF-8", 13) == 0)
			{
				ख_माला_दो("charset=BIG-5", मानक_निकास);
				p += 12;
			}
			अन्यथा यदि (म_भेदन(p, "# Original translations", 23) == 0)
			{
				ख_माला_दो("# Generated from zh_TW.UTF-8.po, DO NOT EDIT.", मानक_निकास);
				जबसम्म (p[1] != '\n')
					++p;
			}
			अन्यथा
			{
				यदि (*(अचिन्हित वर्ण *)p >= 0xA1)
				{
					अक्षर_दो(*p++);
					यदि (*p == '\\')
						अक्षर_दो(*p);
				}
				अक्षर_दो(*p);
			}
		}
	}
}
