# FP_SISOP20_E07
## CP
```c
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

#define BUF_SIZE 256

int main(int argc, char *argv[])
{
	if (argc !=3)
	{
		printf(1, "please input the command as [cp src_file dest_file]\n");
		exit();
	}
	
	//buka source file
	int fd_src = open(argv[1], O_RDONLY);
	if (fd_src == -1)
	{
		printf(1, "open source file failed\n");
		exit();
	}
	
	//cek file directory apa bukan
	struct stat st;
	fstat(fd_src, &st);
	if (st.type == T_DIR)
	{
		printf(1, "source file is a directory, the files in that directory is:\n");
		exit();
	}

	//Tentukan apakah argv[2] berakhir dengan "/", jika ya, lengkapi lintasan
	char com[128] = {};
	strcpy(com, argv[2]);
	int len1 = strlen(argv[1]);
	int len2 = strlen(argv[2]);
	if (argv[2][len2-1] == '/')
	{
		//cari file argv[1]
		int i = len1 - 1;
		for (; i >= 0; i--)
    {
			if (argv[1][i] == '/')
			{
                break;
      }
    }
        i++;
		strcpy(&com[len2], &argv[1][i]);
	}
	
    // printf(1,"%s\n",com);

	//buka file
	int fd_dest = open(com, O_WRONLY|O_CREATE);
	if (fd_dest == -1)
	{
		printf(1, "create dest file failed\n");
		exit();
	}
	
	//Salin file
	char buf[BUF_SIZE] = {};
	int len = 0;
	while((len = read(fd_src, buf, BUF_SIZE)) > 0)
		write(fd_dest, buf, len);
	
	//tutup file
	close(fd_src);
	close(fd_dest);
	
	//akhir
	exit();
}
```
