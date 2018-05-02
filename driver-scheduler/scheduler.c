
#include "schedule.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// initalisation and closing logic
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Kernel* initKernel(char* kernel_name, int nd_range[], 
                int inputs[], int inputs_width[], int num_of_inputs,
                int outputs[], int inputs_width[], int num_of_outputs)
{
  Kernel* k = (Kernel*) malloc(sizeof(Kernel));
  
  k->name = kernel_name;
  k->ioData = (KernelIOData*) malloc(sizeof(KernelIOData));
  k->phy_ioData = (KernelIOData*) malloc(sizeof(KernelIOData));
  k->virtual_ioData = (KernelIOData*) malloc(sizeof(KernelIOData));
  
  k->ioData->inputs = inputs;
  k->ioData->inputs_width = inputs_width;
  k->ioData->outputs = outputs;
  k->ioData->outputs_width = outputs_width;
  k->ioData->num_of_inputs = num_of_inputs;
  k->ioData->num_of_outputs = num_of_outputs;
  
  k->nd_range = nd_range;
  k->isDone = false;
  // set wg_id to zero
  k->wg_id[0] = 0; k->wg_id[1] = 0; k->wg_id[2] = 0;
  return k;
}

void releaseKernel(Kernel* k)
{ 
  for(int i = 0; i < num_of_instances; i++)
    releaseAccel(k->accelerators[i]);
    
  restore_io_data(k);
  k->isDone = true;
} // closeKernel()

void initAccel(Kernel* k, int slotLocation, Bitstream* bitstream)
{
  /*
    Allocate memory and fill the parameters
    update kernels indexInActiveArray
  */
  Accelerator* accel = malloc(sizeof(Accelerator*));
  accel->kernel = k;
  accel->slotLocation = slotLocation;
  accel->bitstream = bitstream;  
  
  k->accelerators[k->num_of_instances] = (struct Accelerator*)accel;
  k->num_of_instances++;
} // initAccel()

void releaseAccel(Accelerator* accel)
{
  // mark all used slots by an instance free
  for(int i = 0; i < accel->kernel->current_slot_size; i++)
    FPGA[accel->slotLocation + i] = FREE;
  
  free(accel);
} // releaseAccel()

void destroy_kernel_ioData(KernelIOData* io)
{
  for(int i = 0; i < io->num_of_inputs; i++)
    free(io->inputs[i]);
  
  for(int i = 0; i < io->num_of_outputs; i++)
    free(io->outputs[i]);
  
  free(io->inputs);
  free(io->inputs_width);
  
  free(io->outputs);
  free(io->outputs_width);
} // destroy_kernel_ioData()

void destroy_kernel(Kernel* k)
{
  destroy_kernel_ioData(k->phy_ioData);
  destroy_kernel_ioData(k->virtual_ioData);
  
  // internal buffers for ioData belong to user hence are not freed by scheduler
  free(k->ioData); 
  free(k->phy_ioData);
  free(k->virtual_ioData);
  free(k->accelerators);
  free(k);
} // destroy_kernel()

void cleanUp()
{
  execution_active = false;
  
  // free memory
  destroy_lib();
  
  for(int i = 0; i < MAX_SLOTS; i++)
    releaseAccel(FPGA[i]);
    
} // cleanUp()

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// IO management for accelerators
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

inline bool isAccelFree(Accelerator* accel)
{
  // Query the hardware memory location of control reg to check if the needed
  // accelerator is free or not.
  return is_done(vir_slot_addrs[accel->slotLocation]);
} // isAccelFree()

void feedDataToAccelerator(Accelerator* accel)
{ 
  // Calculate group ID for the next workgroup
  // update kernel datastructure
  // set the required parameters for accelerator run 
  
  Kernel* k = accel->kernel;
  KernelIOData* phy_ioData = k->phy_ioData;
  
  increment_wg_id(k->wg_id, k->wg_range);
  
  launchWg(vir_slot_addrs[accel->slotLocation], k->wg_id,
            phy_ioData->num_of_inputs, phy_ioData->inputs, 
            accel->bitstream->input_param_width,
            phy_ioData->num_of_outputs, phy_ioData->outputs, 
            accel->bitstream->output_param_width,
            false);
            
} // feedDataToAccelerator()

void feedDataToAllFreeAccelerators()
{
  for(int i = 0; i < HW_SLOTS; i++)
  {
    if(FPGA[i] != FREE && FPGA[i] != OCCUPIED 
       && isAccelFree(FPGA[i]))
        feedDataToAccelerator(Accelerator* accel);
  } // for
} // feedDataToAllFreeAccelerators()

void update_wg_range(Kernel* k , Bitstream* bitstream)
{
  k->wg_range[0] = k->ndrange / bitstream->wg_vector[0];
  k->wg_range[1] = k->ndrange / bitstream->wg_vector[1];
  k->wg_range[2] = k->ndrange / bitstream->wg_vector[2];
}

bool workLeft(Kernel* k)
{
  if(k->wg_range[3] < k->wg_id[3] * )
    return true;
  else if(k->wg_range[3] == k->wg_id[3]
          && k->wg_range[2] < k->wg_id[2])
    return true;
  else if(k->wg_range[3] == k->wg_id[3]
          && k->wg_range[2] == k->wg_id[2]
          && k->wg_range[1] < k->wg_id[1])
    return true;
  else
    return false;
} // workLeft

int numOfConsecutiveSlotsOfGivenSize(int size)
{
  int slotCountForGivenSize = 0;
  int consecutiveFreeSlots = 0;
  for(int i = 0; i < HW_SLOTS; i++)
  {
    if(activeAccelerators[i] == FREE)
    {
      consecutiveFreeSlots++;
      if(consecutiveFreeSlots >= size)
      {
        slotCountForGivenSize++;
        consecutiveFreeSlots = 0;
      }
    }
    else
        consecutiveFreeSlots = 0;
  } // for
  return slotCountForGivenSize;
} // numOfConsecutiveSlotsOfGivenSize()

uint32_t* physical_io_buffer = 0x40000000;
uint32_t* physical_io_buffer_ctr = 0;

void memoryMapIO(Kernel* k)
{
  // map data from ioData to virtual_ioData and phy_ioData using mmap
  KernelIOData* ioData = k->ioData;
  KernelIOData* phy_ioData = k->phy_ioData;
  KernelIOData* virtual_ioData = k->virtual_ioData;
  
  phy_ioData->num_of_inputs = ioData->num_of_inputs;
  virtual_ioData->num_of_inputs = ioData->num_of_inputs;
  
  phy_ioData->input_param_width = ioData->input_param_width;
  virtual_ioData->input_param_width = ioData->input_param_width;
  
  // TODO Avoid duplication
  // Map inputs from user space to physical space 
  for(int i = 0; i < ioData->num_of_inputs; i++)
  {
    // allocate buffer in cyclic fashion
    if(physical_io_buffer_ctr >= MAX_IO_PAGES)
      physical_io_buffer_ctr = 0;

    size_t pagesize = sysconf(_SC_PAGE_SIZE);
    off_t orig_data_mem = physical_io_buffer + physical_io_buffer_ctr*pagesize;
    
    uint32_t* virtual_buffer = allocate_phy_mem(orig_data_mem);
    physical_io_buffer_ctr++;
    
    // Copy data from user space to physical space
    for(int j = 0; j < ioData->inputs_width[i]; j++)
      virtual_buffer[j] = ((uint32_t*)ioData->inputs[i])[j];
    
    // Update the io data structures
    phy_ioData->inputs[i] = orig_data_mem;
    phy_ioData->inputs_width[i] = ioData->inputs_width[i];
    
    virtual_ioData->inputs[i] = virtual_buffer;
    virtual_ioData->inputs_width[i] = ioData->inputs_width[i];
  }
  
  // Map outputs from user space to physical space
  for(int i = 0; i < ioData->num_of_outputs; i++)
  {
    // allocate buffer in cyclic fashion
    if(physical_io_buffer_ctr >= MAX_IO_PAGES)
      physical_io_buffer_ctr = 0;

    size_t pagesize = sysconf(_SC_PAGE_SIZE);
    off_t orig_data_mem = physical_io_buffer + physical_io_buffer_ctr*pagesize;
    
    uint32_t* virtual_buffer = allocate_phy_mem(orig_data_mem);
    physical_io_buffer_ctr++;
    
    // Copy data from user space to physical space
    for(int j = 0; j < ioData->outputs_width[i]; j++)
      virtual_buffer[j] = ((uint32_t*)ioData->outputs[i])[j];
    
    // Update the io data structures
    phy_ioData->outputs[i] = orig_data_mem;
    phy_ioData->outputs_width[i] = ioData->outputs_width[i];
    
    virtual_ioData->outputs[i] = virtual_buffer;
    virtual_ioData->outputs_width[i] = ioData->outputs_width[i];
  }
  
} // memoryMapIO()

void restore_io_data(Kernel* k)
{
  // copy data from virtual_ioData to ioData
  KernelIOData* ioData = k->ioData;
  KernelIOData* virtual_ioData = k->virtual_ioData;
  
  // Map outputs from user space to physical space
  for(int i = 0; i < ioData->outputs_width; i++)
  {    
    // Copy data from physical space to user space
    for(int j = 0; j < ioData->outputs_width[i]; j++)
    {
      ((uint32_t*)ioData->outputs[i])[j] 
          = ((uint32_t*)virtual_ioData->outputs[i])[j];
    }
  } // for
} // memoryMapIO()


void init_slot_access()
{
  size_t pagesize = sysconf(_SC_PAGE_SIZE);
  
  fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd < 1) {
		perror("Cannot open /dev/mem");
		return -1;
	}

  for(int i = 0; i < HW_SLOTS; i++)
    vir_slot_addrs[i] = allocate_phy_mem(phy_slot_addrs[i]);
    
} // init_slot_access

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Partial region management logic
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void loadAccel(Kernel* k, Bitstream* bitstream, int slot)
{
  decouple(slot, true);
  place_accelerator_ptr(bitstream->filename, slot);
  reset_island(slot);
  decouple(slot, false);
  
  initAccel(k, slot, bitstream);
} // loadKernel()

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Scheduler logic
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Bitstream* retrieveAccelerator(Kernel* k, int maxSize)
{
  // get all bistreams for the module from the library
  Bitstream** bitstreams = getAccel(char* module_name);
  if(!bitstreams)
  {
    perror("Accelerator not found in library");
    exit(-1);
  } 
  
  Bitstream* bitstream;
  int currentMaxSize = -1;
  for(int i = 0; i < MAX_BITSTREAMS; i++)
  {
    int current_size = bitstreams[i]->no_of_slots;
    // find biggest module which is <= to maxSize
    if(current_size > currentMaxSize && current_size <= maxSize)
    {
      currentMaxSize = current_size;
      bitstream = bitstream[i];
    } // if
  } // for
  
  return bitstream;
} // retrieveAccelerator

bool execution_active = true;

void scheduler()
{ 

/*
  1) Query status of the currently executing accelerators.
  2) If any accelerator is free and work-groups are left for
  execution, issue a new work-group else free its slot
  allocation and update the kernel’s execution state.
  3) If there are no kernels in the waiting queue or there are
  no free slots available go to Step 1).
  4) Memory map buffers for inputs and outputs of the new
  kernel using mmap(), for the kernel to operate on.
  5) Retrieve XML description of the accelerator from the
  library for the maximum feasible module implementation
  on FPGA in consecutive.
  6) Perform partial reconfiguration using Configuration Con-
  troller and update the logical state of FPGA correspond-
  ingly.
  7) Program the accelerators for the work-group execution
  and then go to Step 1).
 */
  
  while(execution_active)
  {
    for(i = 0; i < MAX_SLOTS; i++)
    {
      if(FPGA[i] != FREE && FPGA[i] != OCCUPIED)
      {
        Accelerator* accel = FPGA[i];
        if(isAccelFree(accel) && if(!workLeft(accel->kernel)))
          releaseKernel(accel->kernel);
      } // if
    } // for
    
    int freeSlots = 0;
    int firstFreeSlot = -1;
    int biggestContiguousRegion = 0;
    int contiguousRegion = 0;
    for(i = 0; i < MAX_SLOTS; i++)
    {
      if(FPGA[i] == FREE)
      {
        freeSlots++;
        contiguousRegion++;
      }
      else if(contiguousRegion > biggestContiguousRegion)
      {
        biggestContiguousRegion = contiguousRegion;
        firstFreeSlot = i - contiguousRegion;
        contiguousRegion = 0;
      }
    } // for
    
    // In case all slots are free. 
    if(contiguousRegion == MAX_SLOTS)
    {
      biggestContiguousRegion = MAX_SLOTS;
      firstFreeSlot = 0;
    }
    
    if(freeSlots && !wqIsEmpty())
    {
      Kernel* k = wqRemove();
      memoryMapIO(k);
      // extract the biggest module for the available free slots
      Bitstream* bitstream = retrieveAccelerator(k, biggestContiguousRegion);
      // load multiple instances
      int num_of_instances = 0;
      for(int slot = firstFreeSlot; 
          slot < biggestContiguousRegion; 
          slot += bitstream->no_of_slots)
      {
        loadAccel(k, bitstream, slot);
        num_of_instances++;
      }
      update_wg_range(k, bitstream);
    } // if
    
    feedDataToAllFreeAccelerators();
  } // while
} // scheduler()

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Functions for users
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Kernel* kernel clEnqueueNDRangeKernel(char* kernel_name, int nd_range[], 
                                       int inputs[], int inputs_width[], 
                                       int num_of_inputs, 
                                       int outputs[], int outputs_width[], 
                                       int num_of_outputs)
{
  Kernel* kernel = initKernel(kernel_name, nd_range, 
                              inputs, inputs_width, num_of_inputs,
                              outputs, outputs_width, num_of_outputs);
  wqInsert(kernel);
  return kernel;
} // clEnqueueNDRangeKernel()

void clFinish(Kernel* k)
{
  // wait until kernel is done with its NDRange execution
  while(! k->isDone);
} // clFinish()

void startScheduler()
{
  build_accel_lib();
  
  init_slot_access();
  init_decoupler();
  
  pthread_t tid;
  pthread_create(&tid, NULL, scheduler, NULL);
} // startScheduler()

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Example
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int main()
{
  startScheduler();
  
  // TODO example
  //---------------------------------------------------------
  // Set up IO for vADD
  //----------------------------------------------------------
  
  //---------------------------------------------------------
  // Launch kernel
  //---------------------------------------------------------
  
  //---------------------------------------------------------
  // Display result
  //----------------------------------------------------------
  
  cleanUp();
  exit(0);
}
