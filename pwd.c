#include "types.h"
#include "fcntl.h"
#include "fs.h"
#include "stat.h"
#include "user.h"

#define NULL   ((void*)0)
#define FALSE  (0)
#define TRUE   (1)

#define PATH_SEPARATOR   "/"

static int getcwd(char* path);
static char* find(int ino, char* arcPath, char* path);
static int dirlookup(int fd, int ino, char* p);

int main(int argc, char *argv[]) {
  char path[512];
  if (getcwd(path))
    printf(1, "%s\n", path);
  else
    printf(2, "pwd failed");
  exit();
}

int getcwd(char* path) {
  path[0] = '\0';

  char arcPath[512];
  strcpy(arcPath, ".");

  struct stat st;
  if (stat(arcPath, &st) < 0)
  {
    return FALSE;
  }

  char* p = find(st.ino, arcPath, path);
  if (p == NULL)
  {
    return FALSE;
  }
  if (path[0] == '\0')
  {
    strcpy(path, PATH_SEPARATOR);
  }
  return TRUE;
}

char* find(int ino, char* arcPath, char* path) 
{
  strcpy(arcPath + strlen(arcPath), PATH_SEPARATOR "..");
  struct stat st;
  if (stat(arcPath, &st) < 0)
  {
    return NULL;
  }

  if (st.ino == ino) 
  {
    // No parent directory exists: must be the root.
    return path;
  }

  char* foundPath = NULL;
  int fd = open(arcPath, O_RDONLY);
  
  if (fd >= 0) 
  {
    char* p = find(st.ino, arcPath, path);
    if (p != NULL) 
    {
      strcpy(p, PATH_SEPARATOR);
      p += sizeof(PATH_SEPARATOR) - 1;

      // Find current directory.
      if (dirlookup(fd, ino, p))
      {
        foundPath = p + strlen(p);
      }
   }
    close(fd);
  }
  return foundPath;
}

//  fd   file descriptor for a directory.
//  ino  target inode number.
//  p    [out] file name (part of absPath), overwritten by the file name of the ino.
int dirlookup(int fd, int ino, char* p) {
  struct dirent de;
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;
    if (de.inum == ino) {
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = '\0';
      return TRUE;
    }
  }
  return FALSE;
}
