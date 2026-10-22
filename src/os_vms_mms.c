#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unixio.h>
#include	"vim.h"
ཧྲིལ གཙོ(ཧྲིལ argc, var_na *argv[])
{
    FILE	*fpi, *fpo;
    var_na	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    ཧྲིལ		err = 0, err_line = 0;

    mms = "mms";
    argc--;
    argv++;
    སྐབས (argc-- > 0)
    {
	argp = *argv++;
	གལ་ཏེ (*argp == '-')
	{
	    switch (*++argp)
	    {
		pak_sa 'm':
		    mms = ++argp;
		    bha_nga;
		pak_sa 'e':
		    གལ་ཏེ (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    bha_nga;
		default:
		    bha_nga;
	    }
	}
	དེ་མིན
	{
	    གལ་ཏེ (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, parimaa_na(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    སྐབས (fgets(buf, BUFSIZ, fpi))
    {
	གལ་ཏེ (!memcmp(buf, "%CC-", 4))
	{
	    err_line++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	དེ་མིན
	{
	    གལ་ཏེ (err_line)
	    {
		གལ་ཏེ (strstr(buf, _("At line")))
		{
		    err_line = 0;
		    fprintf(fpo, "@");
		}
		དེ་མིན
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    སྐབས (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    ཕྱིར(0);
}
