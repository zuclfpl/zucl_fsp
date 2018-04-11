#include <stdio.h>
#include <stdlib.h>
#include "config-api.h"
#include "config-api_l.h"

int main()
{
    char *file = "./trenz_incr-3d.xml";
    char *Dev = "XCZU9EG";
    int island = 1;
    int result;
    int addr;
    RC acc;

    printf("Testing...\n");
    result = init_resource(Dev);
    if (result < 0) {
        printf("Failed to init the FPGA resource\n");
        return -1;
    }
    // place the accelerator on the chosen island
    acc = try_place_accelerator(file, island);
    if (acc == SUCCESS)
        printf("Module is placed in the island %d with the base address\n", island);
    else
        printf("Failed to place the module\n");
    // check resource running
    report_running_regions();
    report_available_regions();
}
