#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unixio.h>
#include	"vim.h"
จำนวนเต็ม หลัก(จำนวนเต็ม argc, var_na *argv[])
{
    FILE	*fpi, *fpo;
    var_na	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    จำนวนเต็ม		err = 0, err_line = 0;

    mms = "mms";
    argc--;
    argv++;
    ขณะ (argc-- > 0)
    {
	argp = *argv++;
	ถ้า (*argp == '-')
	{
	    switch (*++argp)
	    {
		pak_sa 'm':
		    mms = ++argp;
		    bha_nga;
		pak_sa 'e':
		    ถ้า (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    bha_nga;
		default:
		    bha_nga;
	    }
	}
	มิฉะนั้น
	{
	    ถ้า (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, parimaa_na(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    ขณะ (fgets(buf, BUFSIZ, fpi))
    {
	ถ้า (!memcmp(buf, "%CC-", 4))
	{
	    err_line++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	มิฉะนั้น
	{
	    ถ้า (err_line)
	    {
		ถ้า (strstr(buf, _("At line")))
		{
		    err_line = 0;
		    fprintf(fpo, "@");
		}
		มิฉะนั้น
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    ขณะ (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    คืน(0);
}
