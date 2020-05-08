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
		printf(1, "source file is a directory\n");
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
	int fd_dest = open(com, O_RDWR|O_CREATE);
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
cp merupakan command untuk melakukan copy file yang dituju dan menyalin ke directory yang dituju. untuk mencek ini menggunakan struct stat st; jika st.type dari file tersebut bernilai 1 maka akan menampilkan bahwa file itu directory. file yang akan dicopy di open dengan O_DRONLY kemudian membuat dan menulis dengan O_RDWR.

## MV
```c
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

#define BUF_SIZE 256

void ls(char *path);
char* fmtname(char *path);

int main(int argc, char *argv[])
{
	if (argc !=3)
	{
		printf(1, "please input the command as [mv src_file dest_file]\n");
		exit();
	}
	
	//buka source file
	int fd_src = open(argv[1], O_RDONLY);
	
	//cek file directory apa bukan
	struct stat st;
	fstat(fd_src, &st);
	if (st.type == T_DIR)
	{
		printf(1, "source file is a directory, the files in that directory is:\n");
		ls(argv[1]);
		exit();
	}

    if (fd_src == -1)
	{
		printf(1, "open source file failed\n");
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
	
	//hapus file yang di pindah
	if(unlink(argv[1]) < 0)
		printf(1, "delete source file failed\n");
	
	//akhir
	exit();
}

char* fmtname(char *path)
{
	static char buf[DIRSIZ+1];
	char *p;
	
	// Find first character after last slash.
	for(p=path+strlen(path); p >= path && *p != '/'; p--)
		;
	p++;
	
	// Return blank-padded name.
	if(strlen(p) >= DIRSIZ)
		return p;
	memmove(buf, p, strlen(p));
	memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
	return buf;
}

void ls(char *path)
{
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;
	
	if((fd = open(path, 0)) < 0)
	{
		printf(2, "cannot open %s\n", path);
		return;
	}
	
	if(fstat(fd, &st) < 0)
	{
		printf(2, "cannot stat %s\n", path);
		close(fd);
		return;
	}
	
	switch(st.type)
	{
		case T_FILE:
		printf(1, "name = %s, type = file, size = %d\n", fmtname(path), st.size);
		break;
		
		case T_DIR:
		if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
		{
			printf(1, "path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf+strlen(buf);
		*p++ = '/';
		
		while(read(fd, &de, sizeof(de)) == sizeof(de))
		{
			if(de.inum == 0)
				continue;
			memmove(p, de.name, DIRSIZ);
			p[DIRSIZ] = 0;
			if(stat(buf, &st) < 0)
			{
				printf(1, "cannot stat %s\n", buf);
				continue;
			}
			printf(1, "name = %s, type = directory, size = %d\n", fmtname(buf), st.size);
		}
		break;
	}
	close(fd);
}
```
mv merupakan command untuk memindahkan file yang dituju menuju directory yang dituju, dengan menggunakan struct stat st; untuk cek apakah itu adalah directory atau bukan, jika directory maka akan menampilkan isi directory tersebut. Kemudian membuka file sumber dengan O_RDONLY dan menyalin serta membuat dengan O_WRONLY|CREATE serta menyalin file dengan char 
```c
buf[BUF_SIZE] = {};
	int len = 0;
	while((len = read(fd_src, buf, BUF_SIZE)) > 0)
		write(fd_dest, buf, len)
```
kemudian menghapus file sumber

## rename
```c
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

char buff[512];

int main(int argc, char *argv[])
{    
    int fd0,fd1,n;

    struct stat st;
    fd0 = open(argv[1], O_RDONLY);
	fstat(fd0, &st);
	    
    if (st.type == T_DIR)
	{
		printf(1, "source file is a directory, the files in that directory is:\n");
		exit();
	}

	if(fd0 < 0)
    {
        printf(1, "rename: cannot open %s\n", argv[1]);
        exit();
    }
    if((fd1 = open(argv[2], O_CREATE|O_RDWR)) < 0)
    {
        printf(1, "rename: error rename %s\n", argv[2]);
        exit();
    }

    if(unlink(argv[1]) < 0)
    {
		printf(1, "delete source file failed\n");
        exit();
    }

    while (( n = read(fd0, buff, sizeof(buff))) > 0)
    {
        write(fd1, buff, n);
    }

   close(fd0);
   close(fd1);

   exit();
}
```
rename adalah command yang merubah nama dari suatu file, konsep pengerjaannyapun sama dengan cp hanya saja menghapus file sumber.
