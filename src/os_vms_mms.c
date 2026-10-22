#ಅನ್ತರ್ಭಾವ	<stdio.h>
#ಅನ್ತರ್ಭಾವ	<stdlib.h>
#ಅನ್ತರ್ಭಾವ	<string.h>
#ಅನ್ತರ್ಭಾವ	<unixio.h>
#ಅನ್ತರ್ಭಾವ	"vim.h"
ಪೂರ್ಣಾಙ್ಕ main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
{
    FILE	*fpi, *fpo;
    ವರ್ಣ	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    ಪೂರ್ಣಾಙ್ಕ		err = 0, err_ಸಾಲು = 0;

    mms = "mms";
    argc--;
    argv++;
    ಯಾವತ್ (argc-- > 0)
    {
	argp = *argv++;
	ಯದಿ (*argp == '-')
	{
	    ವಿಕಲ್ಪ (*++argp)
	    {
		ಪಕ್ಷ 'm':
		    mms = ++argp;
		    ಭಙ್ಗ;
		ಪಕ್ಷ 'e':
		    ಯದಿ (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		    ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, ಪರಿಮಾಣ(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    ಯಾವತ್ (fgets(buf, BUFSIZ, fpi))
    {
	ಯದಿ (!memcmp(buf, "%CC-", 4))
	{
	    err_ಸಾಲು++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (err_ಸಾಲು)
	    {
		ಯದಿ (strstr(buf, _("At line")))
		{
		    err_ಸಾಲು = 0;
		    fprintf(fpo, "@");
		}
		ಅನ್ಯಥಾ
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    ಯಾವತ್ (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    ಪ್ರತ್ಯಾಗಮ(0);
}
