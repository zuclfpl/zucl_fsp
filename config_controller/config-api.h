// Copyright ... 2018

#ifndef CONFIG_API_H_ECOSCALE
#define CONFIG_API_H_ECOSCALE

#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

// this is the enumeration listing the return codes
typedef enum RC
{
  SUCCESS,
  FAILURE,
  // feel free to expand
  // with more failure codes
  // e.g. from place accelerator
  // or remove accelerator functions
  DEVICE_UNSUPPORTED,
  ISLAND_INVALID,
  ISLAND_INAVAILABLE,
  RESOURCE_MISMATCH
} RC;

//// this is the callback function that the config api would invoke
//// if it moved an accelerator. The runtime would then have to call
//// the function report_available_regions to update the list of all
//// available islands. I think it is best if we do not use this functionality
//// until such point that we discuss it in more details. There can be many 
//// permutations here and we need more clarity on this.
//typedef void (*cf_callback)(/*TBD*/);
//
//RC     init_config_api (cf_callback cb); // will we need any extra params here?
//                                       // e.g. to initialize the config-api

// returns the max num of regions available
// at the moment this is 4, but we don't want to hardcode
// that value into the runtime
size_t get_max_islands ();

//// populates an array of boolean values. The index denotes the island
//// number. The value (true or false) denotes the availability of an island.
//void   report_available_islands (bool* list,           const size_t list_len);

// false - the resource string does not match
// true - the resource string matches
bool   check_resource_string    (const int    island_num, const char* resource_string);

// checks the resource string and places an accelerator, will fail if the placement
// does not succeed (either due to resource string mismatch or otherwise)
RC     try_place_accelerator    (const char*        xml_file,   const int   island_num);

//// places the accelerator without checking the resource string. can still fail
//// due to other factors
//RC     place_accelerator        (const char*  bitstream,  const int   island_num);

RC     remove_accelerator       (const int    island_num);

//// this will not be used yet
//void   release_accelerator      (const int    island_num);


#endif
