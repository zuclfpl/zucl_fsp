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

    const char memDeviceICAP[] = "/dev/icap0";
    const char memDeviceDcpl0[] = "/sys/class/CLASS_TUT100/tutDcplr/setDcpl";
    const char memDeviceDcpl1[] = "/sys/class/CLASS_TUT110/tutDcplr/setDcpl";
    const char memDeviceDcpl2[] = "/sys/class/CLASS_TUT120/tutDcplr/setDcpl";
    const char memDeviceDcpl3[] = "/sys/class/CLASS_TUT130/tutDcplr/setDcpl";

    int fdICAP, fdDcpl0, fdDcpl1, fdDcpl2, fdDcpl3;

#if 0
    u32 PartialAddress;
    u32 IntrStsReg = 0;
    u32 StatusReg;
#endif
    int res_avail;

    // mkdir /lib/firmware
    char * OutBitFile = "/lib/firmware/Trenz_partial.bit.bin";
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
#ifdef ICAP_CONFIG
    // program the FPGA via the ICAP
    fdICAP  = open( memDeviceICAP, O_RDWR | O_SYNC );
#endif
    fdDcpl0 = open( memDeviceDcpl0, O_WRONLY );
    fdDcpl1 = open( memDeviceDcpl1, O_WRONLY );
    fdDcpl2 = open( memDeviceDcpl2, O_WRONLY );
    fdDcpl3 = open( memDeviceDcpl3, O_WRONLY );

    // de-activate the PR module before partial reconfiguration
    printf("De-activating module in island %d\n", island_num);

    switch(island_num)
    {
        case 0:
            write(fdDcpl0, "1", 1*sizeof(int));
            break;
        case 1:
            write(fdDcpl1, "1", 1*sizeof(int));
            break;
        case 2:
            write(fdDcpl2, "1", 1*sizeof(int));
            break;
        case 3:
            write(fdDcpl3, "1", 1*sizeof(int));
            break;
        default:
            printf("Invalid island %d\n", island_num);
            return ISLAND_INVALID;
    }

    // partial reconfiguration via ICAP
    printf("Configuring module in island %d\n", island_num);

#ifdef PCAP_CONFIG
    // prepare the Trenz_partial.bit.bin for PCAP programming
    OutBitFilePtr = fopen(OutBitFile, "wb+");
    fwrite(inData, 4, bitstream_len, OutBitFilePtr);
    // program the PCAP via Peta-linux FPGA's Manager firmware
    system("cd /lib/firmware");
    system("echo 1 > /sys/class/fpga_manager/fpga0/flags");
    system("echo Trenz_partial.bit.bin > /sys/class/fpga_manager/fpga0/firmware");
#endif

#ifdef ICAP_CONFIG
    write(fdICAP, inData, bitstream_len*4);
#endif
    // activate the PR module before partial reconfiguration
    printf("Activating module in island %d\n", island_num);

    switch(island_num)
    {
        case 0:
            write(fdDcpl0, "0", 1*sizeof(int));
            break;
        case 1:
            write(fdDcpl1, "0", 1*sizeof(int));
            break;
        case 2:
            write(fdDcpl2, "0", 1*sizeof(int));
            break;
        case 3:
            write(fdDcpl3, "0", 1*sizeof(int));
            break;
        default:
            printf("Invalid island %d\n", island_num);
            return ISLAND_INVALID;
    }

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

    IslandString[len] = 0;

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

    IslandString[len] = 0;
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
