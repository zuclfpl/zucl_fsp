/*
  Decoupler driver
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define HW_SLOTS 4

volatile uint32_t phy_dec_addrs[HW_SLOTS] = {0xa0010000, 0xa0020000, 0xa0030000, 0xa0040000};
volatile uint32_t* vir_dec_addrs[HW_SLOTS];

int fd;

// Allocate physical memory using mmap 
uint32_t* allocate_phy_mem_of_decoupler(uint32_t orig_data_mem)
{
  printf("iniside decoupler\n");
  size_t pagesize = sysconf(_SC_PAGE_SIZE);
  uint32_t* data_mem = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, 
                            MAP_SHARED, fd, orig_data_mem);
  if (data_mem == MAP_FAILED) 
  {
    perror("Can't map data_memory");
    return -1;
  }
  
  return data_mem;
}

/*
  Edit scheduler.h for var declaration of phy_dec_addrs to set the decoupler
  address of the static in use.
 */

int init_decoupler()
{  
  size_t pagesize = sysconf(_SC_PAGE_SIZE);
  
  fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd < 1) {
		perror("Cannot open /dev/mem");
		return -1;
	}
  
  int i;
  for(i = 0; i < HW_SLOTS; i++)
  {
    printf("Mapping slot: %d\n", i);
    int phy_addr = phy_dec_addrs[i];
    printf("phy_addr: %x\n", phy_addr);
    vir_dec_addrs[i] = allocate_phy_mem_of_decoupler(phy_addr);
    printf("vir_dec_addrs[%d] = %x\n", i, vir_dec_addrs[i]);
  }
  
  return 0;
}

void decouple(int slot, bool turn_on)
{
  *(vir_dec_addrs[slot]) = (turn_on? 1 : 0);
}

int main(int argc, char *argv[]) 
{
  if (argc < 4) 
  {
    printf("Usage: %s <slot> <bool> <read/write>\n", argv[0]);
    return 0;
  }
  
  int slot = strtoul(argv[1], NULL, 0);
  
  int boolean = strtoul(argv[2], NULL, 0);
  
  printf("Initiaing decoupler memory map.\n");
  init_decoupler();
  printf("Finished decoupler memory map.\n");

  if(!strcmp(argv[3], "w"))
  {  
    printf("Initial status of decoupler: %x\n", *vir_dec_addrs[slot]);
    decouple(slot, boolean);
    printf("Final status of decoupler: %x\n", *vir_dec_addrs[slot]);
  }
  else
  {
    printf("current status of decoupler: %x\n", *vir_dec_addrs[slot]);
  }
}
