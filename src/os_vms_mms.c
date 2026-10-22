#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unixio.h>
#include	"vim.h"
ຈຳນວນເຕັມ main(ຈຳນວນເຕັມ argc, var_na *argv[])
{
    FILE	*fpi, *fpo;
    var_na	cmd[132], buf[BUFSIZ], *argp, *error_file, target[132], *mms;
    ຈຳນວນເຕັມ		err = 0, err_line = 0;

    mms = "mms";
    argc--;
    argv++;
    ໃນຂະນະທີ່ (argc-- > 0)
    {
	argp = *argv++;
	ຖ້າ (*argp == '-')
	{
	    switch (*++argp)
	    {
		pak_sa 'm':
		    mms = ++argp;
		    bha_nga;
		pak_sa 'e':
		    ຖ້າ (!*(error_file = ++argp))
		    {
			error_file = *argv++;
			argc--;
		    }
		    bha_nga;
		default:
		    bha_nga;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    ຖ້າ (*target)
		strcat(target, " ");
	    strcat(target, argp);
	}
    }
    vim_snprintf(cmd, parimaa_na(cmd), "%s/output=tmp:errors.vim_tmp %s",
								 mms, target);
    system(cmd);
    fpi = fopen("tmp:errors.vim_tmp", "r");
    fpo = fopen(error_file, "w");
    ໃນຂະນະທີ່ (fgets(buf, BUFSIZ, fpi))
    {
	ຖ້າ (!memcmp(buf, "%CC-", 4))
	{
	    err_line++;
	    buf[strlen(buf)-1] = '\0';
	    err++;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    ຖ້າ (err_line)
	    {
		ຖ້າ (strstr(buf, _("At line")))
		{
		    err_line = 0;
		    fprintf(fpo, "@");
		}
		ບໍ່ດັ່ງນັ້ນ
		    buf[strlen(buf)-1] = '\0';
	    }
	}
	fprintf(fpo, "%s", buf);
    }
    fclose(fpi);
    fclose(fpo);
    ໃນຂະນະທີ່ (!delete("tmp:errors.vim_tmp"))
	/*nop*/;
    exit(err ? 44 : 1);
    ສົ່ງຄືນ(0);
}
