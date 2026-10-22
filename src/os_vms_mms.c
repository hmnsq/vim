#অন্তর্ভাব	<stdio.h>
#অন্তর্ভাব	<stdlib.h>
#অন্তর্ভাব	<string.h>
#অন্তর্ভাব	<unixio.h>
#অন্তর্ভাব	"vim.h"
পূর্ণাঙ্ক main(পূর্ণাঙ্ক argc, বর্ণ *argv[])
{
    FILE	*fpi, *fpo;
    বর্ণ	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    পূর্ণাঙ্ক		err = 0, err_সারি = 0;

    mms = "mms";
    argc--;
    argv++;
    যাবত্ (argc-- > 0)
    {
	argp = *argv++;
	যদি (*argp == '-')
	{
	    বিকল্প (*++argp)
	    {
		পক্ষ 'm':
		    mms = ++argp;
		    ভঙ্গ;
		পক্ষ 'e':
		    যদি (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    ভঙ্গ;
		স্বভাব:
		    ভঙ্গ;
	    }
	}
	অন্যথা
	{
	    যদি (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, পরিমাণ(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    যাবত্ (fgets(buf, BUFSIZ, fpi))
    {
	যদি (!memcmp(buf, "%CC-", 4))
	{
	    err_সারি++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	অন্যথা
	{
	    যদি (err_সারি)
	    {
		যদি (strstr(buf, _("At line")))
		{
		    err_সারি = 0;
		    fprintf(fpo, "@");
		}
		অন্যথা
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    যাবত্ (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    প্রত্যাগম(0);
}
