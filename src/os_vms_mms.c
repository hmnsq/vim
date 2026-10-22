#شامل	<stdio.h>
#شامل	<stdlib.h>
#شامل	<string.h>
#شامل	<unixio.h>
#شامل	"vim.h"
صحیح مین(صحیح argc, حرف *argv[])
{
    FILE	*fpi, *fpo;
    حرف	cmd[132], buf[BUFSIZ], *argp, *error_file, ہدف[132], *mms;
    صحیح		err = 0, err_سطر = 0;

    mms = "mms";
    argc--;
    argv++;
    جب (argc-- > 0)
    {
	argp = *argv++;
	اگر (*argp == '-')
	{
	    انتخاب (*++argp)
	    {
		حالت 'm':
		    mms = ++argp;
		    توڑو;
		حالت 'e':
		    اگر (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    توڑو;
		طےشدہ:
		    توڑو;
	    }
	}
	ورنہ
	{
	    اگر (*ہدف)
		strcat(ہدف, " ");
	    strcat(ہدف, argp);
	}
    }
    vim_snprintf(cmd, حجم(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, ہدف);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    جب (fgets(buf, BUFSIZ, fpi))
    {
	اگر (!memcmp(buf, "%CC-", 4))
	{
	    err_سطر++;
	    buf[لڑی_لمبائی(buf)-1] = '\0';
	    err++;
	}
	ورنہ
	{
	    اگر (err_سطر)
	    {
		اگر (لڑی_تلاش(buf, _("At line")))
		{
		    err_سطر = 0;
		    fprintf(fpo, "@");
		}
		ورنہ
		    buf[لڑی_لمبائی(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    جب (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    باہر_نکلو(err ? 44 : 1);
    واپس(0);
}
