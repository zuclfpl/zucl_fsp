#include "config-api_l.h"
#include "config-api.h"

// How the island represents for clock region on the XCZU9EG device
// Clock region X0Y3-X1Y3-X2Y3: island 0.
// Clock region X0Y4-X1Y4-X2Y4: island 1.
// Clock region X0Y5-X1Y5-X2Y5: island 2.
// Clock region X0Y6-X1Y6-X2Y6: island 3.

#define NoOfIslands 4
#define NoOfColumns 139

extern char *kernel_name;
extern char *target;
extern char *RS;
extern int bitstream_len;
extern int island;
extern char *ResourceString;
extern unsigned int *inData, *outData;

char *Device;

extern int height, width, start_column;

// Region is available when the resource[i][j] is 0 while it is occupied when 1
int resources[NoOfIslands][NoOfColumns];
int temp_resources[NoOfIslands][NoOfColumns];

struct island_data *islands;
struct island_data *optimal_islands;
struct moving_kernel moving;

int init_resource(char *Dev)
{
    int i, j;

//    if (strcmp(Dev,"XC7Z020") != 0) {
//        printf("Device is not supporting!\n");
//        return -1;
//    }

    islands = malloc(NoOfIslands * sizeof(*islands));
    if (islands == NULL) {
        printf("Failed to allocate the islands pointer\n");
        return -1;
    }
    SetGlobalDeviceParameters(Dev);
    for (i = 0; i < NoOfIslands; i++) {
        islands[i].avail = 0;
        islands[i].IslandString = getIslandString(i); 
    }

//    for (i = 0; i < NoOfIslands; i++)
//        printf("Island %d has string %s\n", i, islands[i].IslandString);
    printf("Initialized islands\n");
    return 0;
}

int check_available_region_RS(int island, char * ModuleResource)
{
    int len, i;
    int returnOfCheckRM;
    if (islands[island].avail !=0) return -1;
    len = strlen(ModuleResource);
    for (i = 0; i < len; i++)
        if (strncmp(ModuleResource, islands[island].IslandString, i))
            return -1;
    return 0;
}

int check_available_region(int island)
{
    return islands[island].avail;
}

int set_occupied_region(int island)
{
//    islands[island].avail = 1;
    islands[island].avail++;
    return 0;
}

int set_avail_region(int island)
{
//    islands[island].avail = 0;
    if (islands[island].avail > 0)
        islands[island].avail--;
    return 0;
}

struct avail_region *report_available_regions()
{
    struct avail_region *avail_regions;
    int i, j, len;
    int total = 0;

    for (i = 0; i < NoOfIslands; i++)
        if (!islands[i].avail)
            total++;

    avail_regions = malloc(total * sizeof(*avail_regions));
    if (!avail_regions) {
        printf("Couldn't alloc memory for avail_regions\n");
        return NULL;
    }

    j = 0;
    for (i = 0; i < NoOfIslands; i++)
        if (!islands[i].avail) {
            avail_regions[j].total = total;
            avail_regions[j].island_id = i;
            len = strlen(islands[i].IslandString) + 1;
            avail_regions[j].IslandString = malloc(len * sizeof(char));
            strcpy(avail_regions[j].IslandString, islands[i].IslandString);
//            printf("avail %d, region %d, %s\n", j, i, islands[i].IslandString);
//            printf("avail %d, region %d, %s, 0x%08x\n", j, i, avail_regions[j].IslandString, calNewFAR(avail_regions[j].island_id));
            printf("avail %d, region %d, %s, 0x%08x\n", j, i, avail_regions[j].IslandString, getBaseAddr(avail_regions[j].island_id));
            j++;
        }

    return avail_regions;
}

int report_running_regions()
{
    int i;
    printf("Occupied islands are ");
    for (i = 0; i < NoOfIslands; i++)
        if (check_available_region(i) == 1)
            printf("%d ", i);
    printf("\n");
    return 0;
}

//void remove_accelerator(int island)
RC     remove_accelerator       (const int    island_num)
{
    set_avail_region(island_num);

    return SUCCESS;
}

//int notify_done(int island)
//{
//    int defrag = 0;
//    int i;
//    int base_addr;
//    // Update islands' statuses
//    set_avail_region(island);
//    // Setup the optimal_islands array
//    optimal_islands = malloc(NoOfIslands * sizeof(*optimal_islands));
//    if (optimal_islands == NULL) {
//        printf("Failed to allocate the optimal_islands pointer\n");
//        return -1;
//    }
//    // Find optimal islands array
//    for (i = 0; i < NoOfIslands; i++)
//    {
//        defrag = find_optimal_islands(island);
//        if (defrag)
//        {
//            // Stop queueing jobs in the moving kernel
//            //stop_job_queue(moving.src_island);
//            base_addr = place_accelerator_ptr(moving.KernelFileName, moving.dst_island);
//            set_avail_region(moving.src_island);
//            // Start queueing jobs in the moved kernel
//            //start_job_queue(moving.dst_island);
//        }
//    }
//    // Remove the optimal_islands array if allocated
//    if (optimal_islands)
//        free(optimal_islands);
//    return base_addr;
//}

int find_optimal_islands(int island)
{
    int i, j;
    char len;

    i = island;
    for (j = i; j < NoOfIslands; j++)
        // Bubble Sort to find out the optimal_islands
        if (optimal_islands[i].avail < islands[j].avail)
        {
            // Update optimal_islands status
            optimal_islands[i].avail = islands[j].avail;

            // Update moving's fields
            moving.src_island = j;
            moving.dst_island = i;

            len = strlen(islands[j].KernelName) + 1;
            moving.KernelName = malloc(len * sizeof(char));
            strcpy(moving.KernelName, islands[j].KernelName);

            len = strlen(islands[j].KernelFileName) + 1;
            moving.KernelFileName = malloc(len * sizeof(char));
            strcpy(moving.KernelFileName, islands[j].KernelFileName);
            //Trigger the replacement
            return 1;
        }
    return 0;
}

//int checkResourceAvail(int island, int height, int width, int start_column)
//{
//    int i, j;
//    for (i = island; i < (island + height); i++)
//        for (j = start_column; j < (start_column + width); j++)
//            if (resources[i][j])
//                return resources[i][j];
//}

void setResourceOccupied(int island, int height, int width, int start_column)
{
    int i, j;
    for (i = island; i < (island + height); i++)
        for (j = start_column; j < (start_column + width); j++)
            resources[i][j] = 1;
}

//int place_accelerator(char *BitFile)
//{
//    int i;
//    int val;
//    int total_avail_regions;
//    struct avail_region *avail_regions;
//
//    // report available regions to place module
//    avail_regions = report_available_regions();
//    total_avail_regions = avail_regions[0].total;
//    for (i = 0; i < total_avail_regions; i++) {
//        place_accelerator_ptr(BitFile, avail_regions[i].island_id);
//        // if succeeds, returns the base address value
//        if (val)
//            return val;
//    }
//    // if fails, returns NULL
//    return NULL;
//}

//int place_accelerator_ptr(char *BitFile, int island)
RC     try_place_accelerator    (const char*        xml_file,   const int   island_num)
{
    // Parse data from the XML metafile
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    int i, j, tmp;
    int returnOfCheckRM;
    int PartialCfg = 1;

    char len;
    unsigned char Byte0, Byte1, Byte2, Byte3;

    int Status;
#if 0
    u32 PartialAddress;
    u32 IntrStsReg = 0;
    u32 StatusReg;
#endif
    int res_avail;

    char * OutBitFile = "./Trenz_partial.bit.bin";
    FILE * OutBitFilePtr;
    /*parse the file and get the DOM */
    doc = xmlReadFile(xml_file, NULL, 0);

    if (doc == NULL) {
        printf("error: could not parse file %s\n", xml_file);
        return FAILURE;
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

//    print_element_names(root_element);

    parseTarget(doc, root_element);
    parseRS(doc, root_element);
    parseHW(doc, root_element);
    parseLength(doc, root_element);
    parseNDRangeID(doc, root_element);
    parseBitstream(doc, root_element);
    parseHeight(doc, root_element);
    parseWidth(doc, root_element);
    parseStartColumn(doc, root_element);
//    for (i = 0; i < bitstream_len; i++)
//        printf("0x%08x\n", inData[i]);
    // Bitstream relocation
    Device = malloc(strlen(target) * sizeof(char));
    if (Device == NULL) {
        printf("Failed to allocate the Device\n");
        return DEVICE_UNSUPPORTED;
    }
    else {
        strcpy(Device, target);
        SetGlobalDeviceParameters(Device);
    }

//    // check if Resources is available
//    res_avail = checkResourceAvail(island_num, height, width, start_column);
//    if (res_avail) {
//        printf("Sufficient resource is not available at the island %d\n", island_num);
//        return ISLAND_INAVAILABLE;
//    }

    // Check resource footprint matching
    returnOfCheckRM = check_available_region_RS(island_num, RS);
    if (returnOfCheckRM) {
        printf("Resource footprint mismatched!\n");
        return RESOURCE_MISMATCH;
    }
//	printf("Checked resource footprint\n");

    // Change FAR value
    for (i = 0; i < bitstream_len; i++) {
        if ((inData[i] == 0x30002001)
                && (inData[i+1] != 0x07FC0000)) {
            tmp = calNewFAR(island_num);
            if (tmp < 0)
                printf("Failed to cal new FAR value\n");
            else {
                inData[i+1] = tmp;
            }
        }
    }
//	printf("Changed FAR\n");

    // prepare the Trenz_partial.bit.bin for PCAP programming
    OutBitFilePtr = fopen(OutBitFile, "wb+");

    for (i = 0; i < bitstream_len; i++) {
//        printf("0x%08x\n", inData[i]);
        Byte0 = (char)((inData[i] >> (8*0)) & 0x000000FF);
        Byte1 = (char)((inData[i] >> (8*1)) & 0x000000FF);
        Byte2 = (char)((inData[i] >> (8*2)) & 0x000000FF);
        Byte3 = (char)((inData[i] >> (8*3)) & 0x000000FF);

        fwrite(&Byte1, 1, 1, OutBitFilePtr);
        fwrite(&Byte0, 1, 1, OutBitFilePtr);
        fwrite(&Byte3, 1, 1, OutBitFilePtr);
        fwrite(&Byte2, 1, 1, OutBitFilePtr);
    }
    // program the PCAP via Peta-linux FPGA's Manager firmware
    system("echo 1 > /sys/class/fpga_manager/fpga0/flags");
    system("echo Trenz_partial.bit.bin > /sys/class/fpga_manager/fpga0/firmware");

    // set that island occupied
    set_occupied_region(island_num);

    // set Resources occupied
    setResourceOccupied(island_num, height, width, start_column);
    // copy the kernel_name to the islands[island].KernelName field
    len = strlen(kernel_name) + 1;
    islands[island_num].KernelName = malloc(len * sizeof(char));
    strcpy(islands[island_num].KernelName, kernel_name);
    // copy the BitFile to the islands[island].KernelFileName field
    len = strlen(xml_file) + 1;
    islands[island_num].KernelFileName = malloc(len * sizeof(char));
    strcpy(islands[island_num].KernelFileName, xml_file);
    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
//    return getBaseAddr(island);
    return SUCCESS;
}

///**
// * Query if a kernel with KernelName is already placed in FPGA fabric
// * @param KernelName a char argument
// *        name of kernel being queried
// * @return base address of the kernel if placed
// *         NULL if not
// */
//int query_kernel(char * KernelName)
//{
//    int i;
//
//    for (i = 0; i < NoOfIslands; i++)
//        if (strcmp(KernelName, islands[i].KernelName) == 0)
//            return getBaseAddr(i);
//
//    return NULL;
//}

void backupResources(int island, int height, int width, int start_column)
{
    int i, j;
    // backup the resources[][] array to the temp_resources[][] array
    for (i = 0; i < NoOfIslands; i++)
        for (j = 0; j < NoOfColumns; j++)
            temp_resources[i][j] = resources[i][j];

    // prepare the resources[][] for coming placement
    for (i = island; i < (island + height); i++)
        for (j = start_column; j < (start_column + width); j++)
            resources[i][j] = 0;

    // change the status of islands[island].avail as well
    set_avail_region(island);
}

void rebaseResources(int island, int height, int width, int start_column)
{
    int i, j;
    // copy back data in the resources[][] array from the temp_resources[][] array
    for (i = 0; i < NoOfIslands; i++)
        for (j = 0; j < NoOfColumns; j++)
            resources[i][j] = temp_resources[i][j];
    set_occupied_region(island);
}

//int replace_accelerator(char *BitFile, int island)
//{
////    int base_addr;
//    RC place_acc;
//
//    // Parse data from the XML metafile
//    xmlDoc *doc1 = NULL;
//    xmlNode *root_element1 = NULL;
//    /*parse the file and get the DOM */
//    // parse the current kernel's details to prepare the resources[][] for coming placement
//    doc1 = xmlReadFile(islands[island].KernelFileName, NULL, 0);
//
//    if (doc1 == NULL) {
//        printf("error: could not parse file %s\n", islands[island].KernelFileName);
//        return -1;
//    }
//
//    /*Get the root element node */
//    root_element1 = xmlDocGetRootElement(doc1);
//
////    print_element_names(root_element);
//
//    parseHeight(doc1, root_element1);
//    parseWidth(doc1, root_element1);
//    parseStartColumn(doc1, root_element1);
//    // backup & prepare the Resources array for placing a new kernel
//    backupResources(island, height, width, start_column);
//    // Stop queueing jobs in the moving kernel
//    //stop_job_queue(moving.src_island);
////    base_addr = place_accelerator_ptr(BitFile, island);
//    place_acc = try_place_accelerator(BitFile, island);
//    if (place_acc != SUCCESS) {
//        // get back arguments because they contained new values from BitFile
//        parseHeight(doc1, root_element1);
//        parseWidth(doc1, root_element1);
//        parseStartColumn(doc1, root_element1);
//
//        rebaseResources(island, height, width, start_column);
//        return -1;
//    }
//    // Start queueing jobs in the moved kernel
//    //start_job_queue(moving.dst_island);
//    /*free the document */
//    xmlFreeDoc(doc1);
//
//    /*
//     *Free the global variables that may
//     *have been allocated by the parser.
//     */
//    xmlCleanupParser();
//    return getBaseAddr(island);
//}

char *getIslandString(int island)
{
    char * IslandString;
    int start, end;
    int len, i;
    start = 0;
    end   = NoOfColumns - 1;
    len = end - start + 1;              
    IslandString = malloc(len*sizeof(char));
    for (i = start; i < end; i++)                 
        IslandString[i-start] = ResourceString[i];

    IslandString[len] = NULL;

    return IslandString;
}

bool   check_resource_string    (const int    island_num, const char* resource_string)
{
    char * IslandString;
    char * B;
    int islandBorder;
    int start, end;
    int len, i;

    start = 0;
    end   = NoOfColumns - 1;
    len = end - start + 1;
    IslandString = malloc(len*sizeof(char));
    for (i = start; i < end; i++)
        IslandString[i-start] = ResourceString[i];

    IslandString[len] = NULL;
    len = strlen(resource_string);
    for (i = 0; i < len; i++)
        if (strncmp(resource_string, IslandString, i))
            return false;
    return true;
}

size_t get_max_islands ()
{
    return NoOfIslands;
}
#if 0
int main()
{
    char *Dev = "XC7Z020";
    char *MS  = "CCCRC";
    int island = 0;
    int check;
    init_resource(Dev);
    check = check_available_region_RS( island, MS);
    if (check < 0) {
        printf("The island %d is not available with the Module String %s\n", island, MS);
        printf("The island %d has the string %s\n", island, islands[island].IslandString);
    }
}
#endif
