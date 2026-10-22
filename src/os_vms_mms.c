#অন্তর্ভাৱ	<stdio.h>
#অন্তর্ভাৱ	<stdlib.h>
#অন্তর্ভাৱ	<string.h>
#অন্তর্ভাৱ	<unixio.h>
#অন্তর্ভাৱ	"vim.h"
পূর্ণাঙ্ক main(পূর্ণাঙ্ক argc, ৱর্ণ *argv[])
{
    FILE	*fpi, *fpo;
    ৱর্ণ	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    পূর্ণাঙ্ক		err = 0, err_সাৰি = 0;

    mms = "mms";
    argc--;
    argv++;
    যেতিয়ালৈ (argc-- > 0)
    {
	argp = *argv++;
	যদি (*argp == '-')
	{
	    ৱিকল্প (*++argp)
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
		স্ৱভাৱ:
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
    যেতিয়ালৈ (fgets(buf, BUFSIZ, fpi))
    {
	যদি (!memcmp(buf, "%CC-", 4))
	{
	    err_সাৰি++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	অন্যথা
	{
	    যদি (err_সাৰি)
	    {
		যদি (strstr(buf, _("At line")))
		{
		    err_সাৰি = 0;
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
    যেতিয়ালৈ (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    ঘূৰাই(0);
}
