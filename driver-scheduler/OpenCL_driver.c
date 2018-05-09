//----------------------------------------------------------
// Design: OpenCL driver for ZUCL
// Author: Anuj Vaishnav
// Date: 11-04-2018
// Description: This file contains the driver functionality for ZUCL framework.
// It includes API for issuing whole NDRange to the OpenCL kernel, partial 
// NDRange, is accelerator done, and getting/setting status of the openCL 
// accelerator. 
//
// The function programVadd() shows how to use the driver for a vector add 
// accelerator. 
// 
// The main() function shows how to map memory for the accelerator to operate on.
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

// It depends on how the openCL kernel was synthesized
#define WG_SIZE_X 2
#define MAX_WGS 1
#define DEBUG 1

int i;

/*
  Helper function for incremeting the workgroup id in 3 dimensions. 
 */
void increment_wg_id(int* current_wg_id, int* max_wg_id)
{
	if(current_wg_id[0] < max_wg_id[0] - 1)
		current_wg_id[0]++;
	else
	{
		current_wg_id[0] = 0;
		if(current_wg_id[1] < max_wg_id[1] - 1)
			current_wg_id[1]++;
		else
		{
			current_wg_id[1] = 0;
			if(current_wg_id[2] < max_wg_id[2] - 1)
				current_wg_id[2]++;
			else
				return NULL;
		} // else
	} // else

	return current_wg_id;
}

/*
  The function launches the whole ndrange to the accelerator by issuing 
  one workgroup at a time. It uses polling to identify if the accelerator
  is finished with its work.   
  
  Possible failures and their definitions:
  
  Address translation fault of level 1 takes place if an int* is converted to 
  unsigned int and then back into int* as Vivado hls has 32 address hardcoded
  into driver. This should not affect our driver but it seems the petalinux 
  compiler cannot handle this gracefully.
  
  Address translation fault of level 2 takes place if some other process/hardware
  is allocated the address space to which you are attempting to write. 
 */
void launchKernel(volatile int* kernel_loc,
                   int nd_range[], int wg_size[],
                   int num_of_inputs, uint32_t* inputs[], int input_width,
                   int num_of_outputs, uint32_t* outputs[], int output_width)
{
  #ifdef DEBUG
    printf("kernel_loc: 0x%015x \n\r", kernel_loc);
  #endif
  
  volatile char *control = (volatile char*)kernel_loc;

  volatile int *wg_x = (volatile int*)(kernel_loc + 4); 
  volatile int *wg_y = (volatile int*)(kernel_loc + 6);
  volatile int *wg_z = (volatile int*)(kernel_loc + 8);

  volatile int *o_x = (volatile int*)(kernel_loc + 10);
  volatile int *o_y = (volatile int*)(kernel_loc + 12);
  volatile int *o_z = (volatile int*)(kernel_loc + 14);

  volatile int *inputs_addr = (volatile int*)(kernel_loc + 16);
  volatile int *outputs_addr = (volatile int*)(kernel_loc + 16 
                                                + num_of_inputs*input_width);

  #ifdef DEBUG
    printf("wg_x: %15x, o_z: %15x\n", wg_x, o_z);
  #endif
  
  int wg_id[3];
  int max_wg_id[3];
  int k;
  for(k = 0; k < 3; k++)
  {
	  max_wg_id[k] = nd_range[k] / wg_size[k];
	  wg_id[k] = 0;
  }

  int tot_wgs = (nd_range[0] * nd_range[1] * nd_range[2])
				  / (wg_size[0] * wg_size[1] * wg_size[2]);
  int wg_num = 0;
  while(wg_num < tot_wgs)
  {
    #ifdef DEBUG
	    printf("input_addrs: 0x%015x  output_addrs: 0x%015x \n\r", 
	           inputs_addr, outputs_addr);
    #endif
	  *wg_x = wg_id[0];
	  *wg_y = wg_id[1];
	  *wg_z = wg_id[2];

	  *o_x = 0;
	  *o_y = 0;
	  *o_z = 0;

	  for(i = 0; i < num_of_inputs; i++)
		  *(volatile int *)(inputs_addr + i*input_width) 
		    = (volatile int *) inputs[i];

	  for(i = 0; i < num_of_outputs; i++)
		  *(volatile int *)(outputs_addr + i*output_width) 
        = (volatile int *) outputs[i];

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

	  printf("Starting OpenCL kernel execution\n\r");
	  #endif
	  *control = *control | 1; // start

	  //  waiting for hardware to report "done" 
	  while (! ((*control) & 2));//{printf(".");}
	  
	  #ifdef DEBUG
	    printf("DONE with wg: %d !\n\r", wg_num);
    #endif
    
	  increment_wg_id(wg_id, max_wg_id);
	  wg_num++;
  }
}


void launchWgs(volatile int* kernel_loc,
               int nd_range[], int wg_size[], 
               int wg_id[], int max_wg_id[],
               int num_of_inputs, uint32_t* inputs[], int input_width,
               int num_of_outputs, uint32_t* outputs[], int output_width)
{
  #ifdef DEBUG
    printf("kernel_loc: 0x%015x \n\r", kernel_loc);
  #endif
  
  volatile char *control = (volatile char*)kernel_loc;

  volatile int *wg_x = (volatile int*)(kernel_loc + 4); 
  volatile int *wg_y = (volatile int*)(kernel_loc + 6);
  volatile int *wg_z = (volatile int*)(kernel_loc + 8);

  volatile int *o_x = (volatile int*)(kernel_loc + 10);
  volatile int *o_y = (volatile int*)(kernel_loc + 12);
  volatile int *o_z = (volatile int*)(kernel_loc + 14);

  volatile int *inputs_addr = (volatile int*)(kernel_loc + 16);
  volatile int *outputs_addr = (volatile int*)(kernel_loc + 16 
                                                + num_of_inputs*input_width);

  #ifdef DEBUG
    printf("wg_x: %15x, o_z: %15x\n", wg_x, o_z);
  #endif
  
  int tot_wgs = (max_wg_id[0] - wg_id[0])
                * (max_wg_id[1] - wg_id[1])
                * (max_wg_id[2] - wg_id[2]);
  int wg_num = 0;
  while(wg_num < tot_wgs)
  {
    #ifdef DEBUG
	    printf("input_addrs: 0x%015x  output_addrs: 0x%015x \n\r", 
	           inputs_addr, outputs_addr);
    #endif
	  *wg_x = wg_id[0];
	  *wg_y = wg_id[1];
	  *wg_z = wg_id[2];

	  *o_x = 0;
	  *o_y = 0;
	  *o_z = 0;

	  for(i = 0; i < num_of_inputs; i++)
		  *(volatile int *)(inputs_addr + i*input_width) 
		    = (volatile int *) inputs[i];

	  for(i = 0; i < num_of_outputs; i++)
		  *(volatile int *)(outputs_addr + i*output_width) 
        = (volatile int *) outputs[i];

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

	  printf("Starting OpenCL kernel execution\n\r");
	  #endif
	  *control = *control | 1; //start

	  // waiting for hardware to report "done" 
	  while (! ((*control) & 2)){printf(".");}
	  
	  #ifdef DEBUG
	    printf("DONE with wg: %d !\n\r", wg_num);
    #endif
    
	  increment_wg_id(wg_id, max_wg_id);
	  wg_num++;
  }
}


void launchWg(volatile int* kernel_loc, int wg_id[],
               int num_of_inputs, uint32_t* inputs[], int input_width,
               int num_of_outputs, uint32_t* outputs[], int output_width,
               int blocking)
{
  #ifdef DEBUG
    printf("kernel_loc: 0x%015x \n\r", kernel_loc);
  #endif
  
  volatile char *control = (volatile char*)kernel_loc;

  volatile int *wg_x = (volatile int*)(kernel_loc + 4); 
  volatile int *wg_y = (volatile int*)(kernel_loc + 6);
  volatile int *wg_z = (volatile int*)(kernel_loc + 8);

  volatile int *o_x = (volatile int*)(kernel_loc + 10);
  volatile int *o_y = (volatile int*)(kernel_loc + 12);
  volatile int *o_z = (volatile int*)(kernel_loc + 14);

  volatile int *inputs_addr = (volatile int*)(kernel_loc + 16);
  volatile int *outputs_addr = (volatile int*)(kernel_loc + 16 
                                                + num_of_inputs*input_width);

  #ifdef DEBUG
    printf("wg_x: %15x, o_z: %15x\n", wg_x, o_z);
  #endif
  
  #ifdef DEBUG
    printf("input_addrs: 0x%015x  output_addrs: 0x%015x \n\r", 
           inputs_addr, outputs_addr);
  #endif
  *wg_x = wg_id[0];
  *wg_y = wg_id[1];
  *wg_z = wg_id[2];

  *o_x = 0;
  *o_y = 0;
  *o_z = 0;

  for(i = 0; i < num_of_inputs; i++)
	  *(volatile int *)(inputs_addr + i*input_width) 
	    = (volatile int *) inputs[i];

  for(i = 0; i < num_of_outputs; i++)
	  *(volatile int *)(outputs_addr + i*output_width) 
      = (volatile int *) outputs[i];

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

  printf("Starting OpenCL kernel execution\n\r");
  #endif
  *control = *control | 1; // start

  if(blocking)
  {
    //  waiting for hardware to report "done" 
    while (! ((*control) & 2)){printf(".");}
  }
} // launchWg

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
  Example source code for programming an OpenCL kernel (vector add)
  using the ZUCL openCL driver.
 */
uint32_t* programInc3d(int* baseAddress, int* originalAddress, 
                      uint32_t* data_mem, uint32_t* orig_data_mem)
{
  // allocate data for kernel in mmapped space
  uint32_t *a_data = data_mem;
  uint32_t *a_data_phy = orig_data_mem;

  uint32_t *b_data = data_mem + WG_SIZE_X*MAX_WGS;
  uint32_t *b_data_phy = orig_data_mem + WG_SIZE_X*MAX_WGS;
   
  uint32_t *c_data = data_mem + WG_SIZE_X*2*MAX_WGS;
  uint32_t *c_data_phy = orig_data_mem + WG_SIZE_X*2*MAX_WGS;
  
  // Set up input data for kernel to work on.
  for (i = 0; i < WG_SIZE_X*MAX_WGS; i++)
  {
    a_data[i] = 1;
    b_data[i] = 3;
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
  
  int nd_range[] = {WG_SIZE_X*MAX_WGS, 1, 1};
  int wg_size[] = {WG_SIZE_X, 1, 1};
  
  // Call openCL driver to program the accelerator for kernel execution.
  launchKernel(baseAddress,
               nd_range, wg_size,
               num_of_inputs, inputs, input_width,
               num_of_outputs, outputs, output_width
               );
                   
	return c_data;
} // programInc3d

/*
  This main method shows an example for managing data. 
  It receives the memory location where the kernel is located as user input. 
  Then it allocates the physical memory using mmap for kernel to work on as 
  well as the control registers of the kernel. 
  Finally, it simply calls the user application (vector add) with the 
  mapped memory arguments and displays the results. 
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

  int fd = open("/dev/mem", O_RDWR | O_SYNC);
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

  // arbitrarily chosen location which is known to be free
  off_t orig_data_mem = 0x40000000; 
  int* data_mem = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, 
                       MAP_SHARED, fd, orig_data_mem);
  if (data_mem == MAP_FAILED) 
  {
    perror("Can't map data_memory");
    return -1;
  }

  printf("Begin programmig the openCL kernel with location %15x: \n", mem);
  uint32_t* c_data = programInc3d(mem, offset, data_mem, orig_data_mem);
  
  printf("------Output of the kernel-----\n");
  for (i = 0; i < WG_SIZE_X*MAX_WGS; i++) 
	  printf("data[%d]: %d\n", i, c_data[i]);
  
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
