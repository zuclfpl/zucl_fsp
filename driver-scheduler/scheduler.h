#define DEBUG                               1
#define WAITING_QUEUE_SIZE                  8
#define HW_SLOTS                            4
#define MAX_BITSTREAMS                      4
#define MAX_IO_PAGES                        16

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
#include <pthread.h>
#include <stdint.h>

#include "driver.c"

typedef struct Bitstream 
{
  char* filename;
  // data for placement and management
  int wg_vector[3];
  int no_of_slots;
  
  // data for scheduling hueristics
  int wg_latency;
  
  int input_param_width;
  int output_param_width;
  
} Bitstream;

typedef struct KernelIOData 
{
  uint32_t* inputs[];
  uint32_t* inputs_width[];
  int num_of_inputs;
  
  uint32_t* outputs[];
  uint32_t* outputs_width[];
  int num_of_outputs;
} KernelIOData;

// Kernel description data structure
typedef struct Kernel 
{
  char* name;
  bool isDone;
  
  // IO data management
  KernelIOData* ioData;
  KernelIOData* phy_ioData;
  KernelIOData* virtual_ioData;
  
  // current state
  int wg_id[3];
  int wg_range[3];
  int nd_range[3];
  
  int current_slot_size;
  int num_of_instances;
  struct Accelerator** accelerators;
        
} Kernel;

typedef struct Accelerator 
{
  Bitstream* bitstream;
  int slotLocation;
  Kernel* kernel;
  
} Accelerator;

Accelerator* OCCUPIED = (Accelerator *) -1;
Accelerator* FREE = (Accelerator *) 0;

// File descripter for main memory
int fd;

// Static specific details
// TODO
volatile int* phy_slot_addrs = {0x80010000, 0x80020000, 0x80030000, 0x80040000};
volatile int* vir_slot_addrs[HW_SLOTS];

volatile int* phy_dec_addrs = {0x80010000, 0x80020000, 0x80030000, 0x80040000};
volatile int* vir_dec_addrs[HW_SLOTS];

Accelerator* FPGA[HW_SLOTS];

// Allocate physical memory using mmap 
uint32_t* allocate_phy_mem(off_t orig_data_mem)
{
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

// Waiting Queue Data structure
// Employs FIFO semantics: Thus follows Round Robin for Time domain multiplexing
#define QUEUE_TYPE Kernel*

QUEUE_TYPE waitingQueue[WAITING_QUEUE_SIZE];
int wqFront = 0;
int wqRear = -1;
int wqCount = 0;

QUEUE_TYPE wqPeek() 
{
   return waitingQueue[wqFront];
}

bool wqIsEmpty() 
{
   return wqCount == 0;
}

bool wqIsFull() 
{
   return wqCount == WAITING_QUEUE_SIZE;
}

int wqSize() 
{
   return wqCount;
}  

void wqInsert(QUEUE_TYPE data) 
{
   if(!wqIsFull()) {
      if(wqRear == WAITING_QUEUE_SIZE-1) {
         wqRear = -1;            
      }       
      waitingQueue[++wqRear] = data;
      wqCount++;
   }
}

QUEUE_TYPE wqRemove() 
{
   QUEUE_TYPE data = waitingQueue[wqFront++];
   if(wqFront == WAITING_QUEUE_SIZE)
      wqFront = 0;
   wqCount--;
   return data;
}
