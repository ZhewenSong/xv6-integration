#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "types.h"
#include "fs.h"
#define DEBUG 0

// xv6 fs img
// similar to vsfs
// unused | superblock | inode table | bitmap (data) | data blocks
// (some gaps in here)

void ASSERT(int cond, char *msg) {
  if (!cond) {
    fprintf(stderr, "%s", msg);
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  ASSERT(argc == 2, "Usage: xv6_fsck <file_system_image>\n");
  int fd = open(argv[1], O_RDONLY);
  ASSERT(fd > 0, "image not found.\n");

  struct stat sbuf;
  ASSERT(fstat(fd, &sbuf) == 0, "image not found.\n");

  void *img_ptr = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  ASSERT(img_ptr != MAP_FAILED, "mmap failed\n");

  struct superblock *sb;
  sb = (struct superblock *)(img_ptr + BSIZE);

  struct dinode *dinp = (struct dinode *)(img_ptr + 2 * BSIZE);

  uchar *bitmap = (uchar *)(img_ptr + (IBLOCK(sb->ninodes) + 1) * BSIZE);

  uint *usedaddrs = malloc(sb->nblocks * sizeof(uint));
  memset(usedaddrs, 0, sb->nblocks);

  uint *useddirs = malloc(sb->ninodes * sizeof(uint));
  memset(useddirs, 0, sb->ninodes);

  int *usedinodes = malloc(sb->ninodes * sizeof(int));
  memset(usedinodes, 0, sb->ninodes);

  uint *refcount = malloc(sb->ninodes * sizeof(int));
  memset(refcount, 0, sb->ninodes);

  uint *addrsbit = malloc(sb->nblocks * sizeof(uint));
  memset(addrsbit, 0, sb->nblocks);

  for (int inum = 0; inum < sb->ninodes; inum++) {
    ASSERT(dinp[inum].type >= 0 && dinp[inum].type <= 3, "ERROR: bad inode.\n");
    ASSERT(dinp[1].type == 1, "ERROR: root directory does not exist.\n");

    if (!dinp[inum].type) continue;
    if (inum > 1) usedinodes[inum] += 1;  // inode marked use
    uint *addrs = dinp[inum].addrs;
    uint *paddrs, *pindiraddrs;
    if (dinp[inum].type == 1) {
      ASSERT(addrs[0] > (IBLOCK(sb->ninodes) + 1) && addrs[0] < sb->size,
             "ERROR: bad direct address in inode.\n");
      struct dirent *dirp = (struct dirent *)(img_ptr + addrs[0] * BSIZE);
      // assuming the first and second entry is . and .. respectively
      ASSERT(
          strcmp(dirp->name, ".") == 0 && strcmp((dirp + 1)->name, "..") == 0,
          "ERROR: directory not properly formatted.\n");
      // usedinodes[dirp->inum]++; // avoid double counting of . and .. as inode
      // usedinodes[(dirp+1)->inum]++;
      ASSERT(inum != 1 || (dirp + 1)->inum == 1,
             "ERROR: root directory does not exist.\n");

      ASSERT(inum == 1 || inum != (dirp + 1)->inum,
             "ERROR: parent directory mismatch.\n");
      struct dinode parent = dinp[(dirp + 1)->inum];
      paddrs = parent.addrs;
      pindiraddrs = (uint *)(img_ptr + paddrs[NDIRECT] * BSIZE);
    }
    int found = 0;

    // scanning for the direct entries
    for (int j = 0; j < NDIRECT; j++) {
      ASSERT(addrs[j] == 0 ||
                 (addrs[j] > (IBLOCK(sb->ninodes) + 1) && addrs[j] < sb->size),
             "ERROR: bad direct address in inode.\n");
      if (addrs[j] > 0) {
        ASSERT((usedaddrs[addrs[j]]++) == 0,
               "ERROR: direct address used more than once.\n");
        addrsbit[addrs[j]] = 1;
      }
      if (dinp[inum].type == 1) {  // directory
        struct dirent *dirp = (struct dirent *)(img_ptr + addrs[j] * BSIZE);
        for (; dirp < (struct dirent *)(img_ptr + (addrs[j] + 1) * BSIZE);
             dirp++) {
          if (!dirp->inum) continue;

          if (DEBUG)
            printf("dir : %d, contents : %d, %s\n", inum, dirp->inum,
                   dirp->name);
          ASSERT(dinp[dirp->inum].type > 0,
                 "ERROR: inode referred to in directory but marked free.\n");
          usedinodes[dirp->inum]--;  // inode found in a dir
          struct dirent *dirp0 = (struct dirent *)(img_ptr + addrs[0] * BSIZE);
          if (dirp > dirp0 + 1 &&
              dinp[dirp->inum].type == 1)  // dir excluding . and ..
            ASSERT((useddirs[dirp->inum]++) == 0,
                   "ERROR: directory appears more than once in file system.\n");
          if (dinp[dirp->inum].type == 2) {  // file
            refcount[dirp->inum]++;
          }
        }

        struct dirent *child = (struct dirent *)(img_ptr + paddrs[j] * BSIZE);
        for (; child < (struct dirent *)(img_ptr + (paddrs[j] + 1) * BSIZE);
             child++) {
          if (child->inum == inum) {
            found = 1;
            break;
          }
        }
      }
    }

    if (!addrs[NDIRECT]) continue;

    // the 13th entry
    ASSERT(addrs[NDIRECT] == 0 || (addrs[NDIRECT] > (IBLOCK(sb->ninodes) + 1) &&
                                   addrs[NDIRECT] < sb->size),
           "ERROR: bad indirect address in inode.\n");
    ASSERT((usedaddrs[addrs[NDIRECT]]++) == 0,
           "ERROR: indirect address used more than once.\n");
    addrsbit[addrs[NDIRECT]] = 1;

    // scan for indirect entries
    uint *indiraddrs = (uint *)(img_ptr + addrs[NDIRECT] * BSIZE);
    for (int k = 0; k < NINDIRECT; k++) {
      ASSERT(indiraddrs[k] == 0 || (indiraddrs[k] > (IBLOCK(sb->ninodes) + 1) &&
                                    indiraddrs[k] < sb->size),
             "ERROR: bad indirect address in inode.\n");
      if (indiraddrs[k] > 0) {
        ASSERT((usedaddrs[indiraddrs[k]]++) == 0,
               "ERROR: indirect address used more than once.\n");
        addrsbit[indiraddrs[k]] = 1;
      }
      if (dinp[inum].type == 1) {  // directory
        struct dirent *dirp =
            (struct dirent *)(img_ptr + indiraddrs[k] * BSIZE);
        for (; dirp < (struct dirent *)(img_ptr + (indiraddrs[k] + 1) * BSIZE);
             dirp++) {
          if (!dirp->inum) continue;
          if (DEBUG)
            printf("dir : %d, contents : %d, %s\n", inum, dirp->inum,
                   dirp->name);
          ASSERT(dinp[dirp->inum].type > 0,
                 "ERROR: inode referred to in directory but marked free.\n");
          usedinodes[dirp->inum]--;        // inode found in a dir
          if (dinp[dirp->inum].type == 1)  // dir
            ASSERT((useddirs[dirp->inum]++) == 0,
                   "ERROR: directory appears more than once in file system.\n");
          if (dinp[dirp->inum].type == 2) {  // file
            refcount[dirp->inum]++;
          }
        }

        struct dirent *child =
            (struct dirent *)(img_ptr + pindiraddrs[k] * BSIZE);
        for (;
             child < (struct dirent *)(img_ptr + (pindiraddrs[k] + 1) * BSIZE);
             child++) {
          if (child->inum == inum) {
            found = 1;
            break;
          }
        }
      }
    }
    if (dinp[inum].type == 1)
      ASSERT(found, "ERROR: parent directory mismatch.\n");
  }

  for (int inum = 0; inum < sb->ninodes; inum++) {
    ASSERT(usedinodes[inum] <= 0,
           "ERROR: inode marked use but not found in a directory.\n");
    ASSERT(dinp[inum].type != 2 || refcount[inum] == dinp[inum].nlink,
           "ERROR: bad reference count for file.\n");
  }

  for (int addr = BBLOCK(sb->nblocks, sb->ninodes) + 1; addr < sb->nblocks;
       addr++) {
    if (bitmap[addr / 8] >> (addr % 8) & 1)
      ASSERT(addrsbit[addr],
             "ERROR: bitmap marks block in use but it is not in use.\n");
    else
      ASSERT(!addrsbit[addr],
             "ERROR: address used by inode but marked free in bitmap.\n");
  }

  free(usedaddrs);
  free(useddirs);
  free(usedinodes);
  free(refcount);
  free(addrsbit);
  return 0;
}
