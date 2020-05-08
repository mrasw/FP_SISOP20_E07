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
