
#ifndef CONFIG_API_L_H
#define CONFIG_API_L_H

#include "devices.h"
#include "bitman.h"
#include "xmltree.h"

int init_resource(char *Device);
int check_available_region(int island);
int check_available_region_RS(int island, char * ModuleResource);
int set_occupied_region(int island);
int set_avail_region(int island);
int report_running_regions();
char *getIslandString(int island);

struct avail_region *report_available_regions();

//int notify_done(int island);
int find_optimal_islands(int island);

struct island_data
{
    int from_X;
    int from_Y;
    int to_X;
    int to_Y;
    int avail;
    char * IslandString;
    char * KernelName;
    char * KernelFileName;
};

struct avail_region
{
    int total;
    int island_id;
    char * IslandString;
};

struct moving_kernel
{
    int src_island;
    int dst_island;
    char * KernelName;
    char * KernelFileName;
};

int checkResourceAvail(int island, int height, int width, int start_column);
void setResourceOccupied(int island, int height, int width, int start_column);
void backupResources(int island, int height, int width, int start_column);
void rebaseResources(int island, int height, int width, int start_column);

//int replace_accelerator(char *BitFile, int island);
//int query_kernel(char * KernelName);

#endif
