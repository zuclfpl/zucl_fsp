/*
  Decoupler driver
*/

#include"scheduler.h"

int fd;

/*
  Edit scheduler.h for var declaration of phy_dec_addrs to set the decoupler
  address of the static in use.
 */

void init_decoupler();
{  
  size_t pagesize = sysconf(_SC_PAGE_SIZE);
  
  fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd < 1) {
		perror("Cannot open /dev/mem");
		return -1;
	}
  
  for(int i = 0; i < HW_SLOTS; i++)
    vir_dec_addrs[i] = allocate_phy_mem(phy_dec_addrs[i]);
}

void decouple(slot, bool turn_on)
{
  *(vir_dec_addrs[slot]) = (turn_on? 1 : 0);
}

int main(int argc, char *argv[]) 
{
  if (argc < 3) 
  {
    printf("Usage: %s <slot> <bool>\n", argv[0]);
    return 0;
  }
  
  int slot = strtoul(argv[1], NULL, 0);
  
  int boolean = strtoul(argv[2], NULL, 0);
  
  printf("Initial status of decoupler: %d", vir_dec_addrs[slot]);
  decouple(slot, boolean);
  printf("Final status of decoupler: %d", vir_dec_addrs[slot]);
}
