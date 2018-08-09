//----------------------------------------------------------
// Design: C driver for ZUCL
// Author: Anuj Vaishnav
// Date: 09-08-2018
// Description: This file contains the driver functionality for ZUCL framework.
// It includes API for issuing whole NDRange to the OpenCL kernel, partial 
// NDRange, is accelerator done, and getting/setting status of the openCL 
// accelerator. 
//
// The function add() shows how to use the driver for an addition 
// accelerator. 
// 
// The main() function shows how to map memory for the accelerator to operate on.
//
// TODO: Support more 1 page io buffers by using the kernel modules available online for contiguous memory allocation.  
//----------------------------------------------------------
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
#include <stdint.h>

#define DEBUG 1

#define DATA_LENGTH 1

// The page frame shifted left by PAGE_SHIFT will give us the physcial address of the frame
// Note that this number is architecture dependent. For me on x86_64 with 4096 page sizes,
// it is defined as 12. If you're running something different, check the kernel source
// for what it is defined as.
#define PAGE_SHIFT 12

int i;
int fd;

/*
  The function launches the whole ndrange to the accelerator by issuing 
  one workgroup at a time. It uses polling to identify if the accelerator
  is finished with its work.   
  
  ***Note, the driver assumes an ordering of kernel arguments: inputs followed by
  outputs.***
  
  Possible failures and their definitions:
  
  Address translation fault of level 1 takes place if an int* is converted to 
  unsigned int and then back into int* as Vivado hls has 32 address hardcoded
  into driver. This should not affect our driver but it seems the petalinux 
  compiler cannot handle this gracefully.
  
  Address translation fault of level 2 takes place if some other process/hardware
  is allocated the address space to which you are attempting to write. 
 */
void launchKernel(volatile int* kernel_loc,
                   int num_of_inputs, uint32_t* inputs[], int input_width,
                   int num_of_outputs, uint32_t* outputs[], int output_width)
{
  #ifdef DEBUG
    printf("kernel_loc: 0x%015x \n\r", kernel_loc);
  #endif
  
  volatile char *control = (volatile char*)kernel_loc;

  volatile int *inputs_addr = (volatile int*)(kernel_loc + 4);
  volatile int *outputs_addr = (volatile int*)(kernel_loc + 4 
                                                + num_of_inputs*input_width);


  #ifdef DEBUG
    printf("input_addrs: 0x%015x  output_addrs: 0x%015x \n\r", 
           inputs_addr, outputs_addr);
  #endif
  
  printf("Status of control register: \n\r");
  unsigned int con_1 = *control;
  
  // print status register bits
  for (i = 0; i < 8; i ++) {
	  if (con_1 & (1 << i) ) {
		  printf("1");
	  } else {
		  printf("0");
	  }
  }
  printf("\n\r");

  for(i = 0; i < num_of_inputs; i++)
  {
	  *(volatile int *)(inputs_addr + i*input_width) 
	    = (volatile int *) inputs[i];
	    
	  if(input_width > 2)
	  {
	    // padd [63:32] bits to zero
	    #ifdef DEBUG
	      printf("seting input %d at addrs: %x\n", i,
	             (inputs_addr + i*input_width + 1));
	    #endif
	    *(volatile int *)(inputs_addr + i*input_width + 1) = 0;
	  }
  }
  
  for(i = 0; i < num_of_outputs; i++)
  {
	  *(volatile int *)(outputs_addr + i*output_width) 
      = (volatile int *) outputs[i];
    
    if(output_width > 2)
	  {
	    // padd [63:32] bits to zero
	    #ifdef DEBUG
	      printf("seting output %d at addrs: %x\n", i,
	             (outputs_addr + i*output_width + 1));
	    #endif
	    *(volatile int *)(outputs_addr + i*output_width + 1) = 0;
	  }
  }
  #ifdef DEBUG
    printf("Status of control register: \n\r");
  #endif
  
  unsigned int con = *control;
  
  #ifdef DEBUG
  // print status register bits
  for (i = 0; i < 8; i ++) {
	  if (con & (1 << i) ) {
		  printf("1");
	  } else {
		  printf("0");
	  }
  }
  printf("\n\r");

  printf("Starting C kernel execution\n\r");
  #endif
  *control = *control | 1; // start

  //  waiting for hardware to report "done" 
  while (! ((*control) & 2));//{printf(".");}
}

/*
  Retrives the control register status of the accelerator as per the Xilinx 
  standard. 
  
  0x00 : Control signals
  bit 0  - ap_start (Read/Write/COH)
  bit 1  - ap_done (Read/COR)
  bit 2  - ap_idle (Read)
  bit 3  - ap_ready (Read)
  bit 7  - auto_restart (Read/Write)
  others - reserved
 */
char get_status(volatile int* kernel_loc)
{
  volatile char *control = (volatile char*)kernel_loc;
  return *control;
} // get_status()

void set_status(volatile int* kernel_loc, volatile char* control)
{
  ((volatile char*) kernel_loc)[0] = *control;
} // get_status()

/*
  queries done signal from accelerator
 */
int is_done(volatile int* kernel_loc)
{
  volatile char *control = (volatile char*)kernel_loc;
  return ((*control) & 2);
}

/*
  Example source code for programming an C kernel (Add)
  using the ZUCL C driver.
 */
 
uint32_t* programAdd(int* baseAddress, int* originalAddress, 
                      uint32_t* data_mem, uint32_t* orig_data_mem)
{
  // allocate data for kernel in mmapped space
  uint32_t *a_data = data_mem;
  uint32_t *a_data_phy = orig_data_mem;

  uint32_t *b_data = data_mem + DATA_LENGTH;
  uint32_t *b_data_phy = orig_data_mem + DATA_LENGTH;
   
  uint32_t *c_data = data_mem + DATA_LENGTH*2;
  uint32_t *c_data_phy = orig_data_mem + DATA_LENGTH*2;
  
  // Set up input data for kernel to work on.
	for(i = 0; i < DATA_LENGTH; i++)
	{
		a_data[i] = 5;
		b_data[i] = 4;
		c_data[i] = -1;
	}
  
  #ifdef DEBUG
    printf("a_data_vir: %15x, b_data_vir: %15x \n", a_data, b_data);
  #endif
  
  // Setup arguments for the driver. 
  int num_of_inputs = 2;
  uint32_t* inputs[] = {a_data_phy, b_data_phy};
  int input_width = 3; // num of ints before next input begins

  int num_of_outputs = 1;
  uint32_t* outputs[] = {c_data_phy};
  int output_width = 3; // num of ints before next output begins
  
  // Call openCL driver to program the accelerator for kernel execution.
  launchKernel(baseAddress,
               num_of_inputs, inputs, input_width,
               num_of_outputs, outputs, output_width
               );
                   
	return c_data;
} // programAdd

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

unsigned long get_page_frame_number_of_address(void *addr) {
   // Open the pagemap file for the current process
   FILE *pagemap = fopen("/proc/self/pagemap", "rb");

   // Seek to the page that the buffer is on
   unsigned long offset = (unsigned long)addr / getpagesize() * PAGEMAP_LENGTH;
   if(fseek(pagemap, (unsigned long)offset, SEEK_SET) != 0) {
      fprintf(stderr, "Failed to seek pagemap to proper location\n");
      exit(1);
   }

   // The page frame number is in bits 0-54 so read the first 7 bytes 
   // and clear the 55th bit
   unsigned long page_frame_number = 0;
   fread(&page_frame_number, 1, PAGEMAP_LENGTH-1, pagemap);

   page_frame_number &= 0x7FFFFFFFFFFFFF;

   fclose(pagemap);

   return page_frame_number;
}

// Returns phy addrs to a page allocated in main memory by a user.
uint64_t allocate_page_for_io_and_get_phy_addr()
{
  // Allocate some memory to manipulate
  size_t buf_size = sizeof(char)*BUF_LEN;
  
  void *buffer = malloc(buf_size);
  if(buffer == NULL) {
    fprintf(stderr, "Failed to allocate memory for buffer\n");
    exit(1);
  }

  // Lock the page in memory
  // Do this before writing data to the buffer so that any copy-on-write
  // mechanisms will give us our own page locked in memory
  if(mlock(buffer, buf_size) == -1) {
    fprintf(stderr, "Failed to lock page in memory: %s\n", strerror(errno));
    exit(1);
  }
  
  unsigned int page_frame_number = get_page_frame_number_of_address(buffer);  

  // Find the difference from the buffer to the page boundary
  unsigned int distance_from_page_boundary = (unsigned long)buffer % getpagesize();

  // Determine how far to seek into memory to find the buffer
  uint64_t orig_data_mem = (page_frame_number << PAGE_SHIFT);
  
  return orig_data_mem;
}

/*
  This main method shows an example for managing data. 
  It receives the memory location where the kernel is located as user input. 
  Then it allocates the physical memory using mmap for kernel to work on as 
  well as the control registers of the kernel. 
  Finally, it simply calls the user application (add) with the 
  mapped memory arguments and displays the results. 
  Note, the memory allocation of the IO data is bound to a page size. 
  Beyond that it may not return contiguous memory for accelerator to operate on.
 */

int main(int argc, char *argv[]) 
{
  if (argc < 2) 
  {
    printf("Usage: %s <phys_addr> \n", argv[0]);
    return 0;
  }

  off_t offset = strtoul(argv[1], NULL, 0);
  // Truncate offset to a multiple of the page size, or mmap will fail.
  size_t pagesize = sysconf(_SC_PAGE_SIZE);
  printf("accel address: %15x with page size %d: \n", offset, pagesize);

  fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd < 1) {
		perror("Cannot open /dev/mem");
		return -1;
	}
	
  int* mem = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, 
                  MAP_SHARED, fd, offset);
  if (mem == MAP_FAILED) 
  {
    perror("Can't map memory");
    return -1;
  }

  // Allocate some memory to manipulate
  off_t orig_data_mem = allocate_page_for_io_and_get_phy_addr();
  // arbitrarily chosen location which is known to be free
  int* data_mem = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, 
                       MAP_SHARED, fd, orig_data_mem);
  
  if (data_mem == MAP_FAILED) 
  {
    perror("Can't map data_memory");
    return -1;
  }

  printf("Begin programmig the openCL kernel with location %15x: \n", mem);
  uint32_t* out_data = programAdd(mem, offset, data_mem, orig_data_mem);
  
  printf("------Output of the kernel-----\n");
  for (i = 0; i < DATA_LENGTH; i++) 
	  printf("data[%d]: %x\n", i, out_data[i]);
  
  if (munmap(mem, pagesize) == -1
      || munmap(data_mem, pagesize) == -1)
  {
    close(fd);
    perror("Error un-mmapping the file");
    exit(EXIT_FAILURE);
  }

  // Un-mmaping doesn't close the file, so we still need to do that.
  close(fd);
  
  return 0;
}
