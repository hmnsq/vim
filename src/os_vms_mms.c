#অন্তর্ভাব	<stdio.h>
#অন্তর্ভাব	<stdlib.h>
#অন্তর্ভাব	<string.h>
#অন্তর্ভাব	<unixio.h>
#অন্তর্ভাব	"vim.h"
পূর্ণসংখ্যা প্রধান(পূর্ণসংখ্যা argc, অক্ষর *argv[])
{
    FILE	*fpi, *fpo;
    অক্ষর	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    পূর্ণসংখ্যা		err = 0, err_সারি = 0;

    mms = "mms";
    argc--;
    argv++;
    যখন (argc-- > 0)
    {
	argp = *argv++;
	যদি (*argp == '-')
	{
	    নির্বাচন (*++argp)
	    {
		ক্ষেত্র 'm':
		    mms = ++argp;
		    বিরতি;
		ক্ষেত্র 'e':
		    যদি (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    বিরতি;
		পূর্বনির্ধারিত:
		    বিরতি;
	    }
	}
	অন্যথা
	{
	    যদি (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, আকার(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    যখন (fgets(buf, BUFSIZ, fpi))
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
    যখন (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    ফেরত(0);
}
