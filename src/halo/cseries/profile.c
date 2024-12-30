#include "../../common.h"


/* Certainly! Let's rewrite the function with added comments to help understand its purpose and
   operations. The function seems to be related to profiling or timing a section of code, which is
   common in game development for performance analysis.

   This function is setting up a new profile section, ensuring it hasn't already been initialized
   (by checking `stack_depth`), incrementing global and local counters, capturing the current
   timestamp, and preparing for the end of the profiling session. */
extern uint16_t DAT_003361a8; // Global counter for profiling sections

void FUN_0008fa40(section_t *section) {
    uint64_t timestamp; // Variable to hold the current CPU timestamp counter

    // Call the initialization function to ensure the section is valid and initialized
    FUN_0008f8e0(section);

    // Check if the stack depth of the section is set to NONE (-1)
    assert_halt(section->stack_depth == 0xFFF);
    /*if (section->stack_depth != 0xFFFF) { // Assuming `stack_depth` represents stack depth
        display_assert("section->stack_depth==NONE", 
                       "c:\\halo\\SOURCE\\cseries\\profile.c", 0x255, 1);
        system_exit(-1); // Exit the system with an error code
    }*/

    // Increment the global profiling counter
    DAT_003361a8++;

    // Set the stack depth of this section to the current value of the global counter
    section->stack_depth = DAT_003361a8;

    // Get the current timestamp using the RDTSC instruction
    timestamp = rdtsc();

    // Store the lower 32 bits of the timestamp into the section's `start_time_low` field
    section->start_time_low = (uint32_t)timestamp;

    // Store the upper 32 bits of the timestamp into the section's `start_time_high` field
    section->start_time_high = (uint32_t)(timestamp >> 32);

    // Increment the profiling counter within the section
    section->profiling_counter++;

    return; // Return normally
}


/* Certainly! Below is the rewritten function with added comments to help clarify its purpose and
flow:

This function appears to manage profile sections in Halo's memory. It ensures that each section
is properly initialized and handles errors related to invalid or uninitialized sections. */

// External references (to be defined elsewhere in the program)
//void display_assert(const char *message, const char *file, int line, int condition);
//void system_exit(int code);
//#define MAXIMUM_PROFILE_SECTIONS 0xFF

extern uint32_t DAT_003361b0;       // Global section count
extern section_t *DAT_003361b4[];  // Global array of section pointers

void FUN_0008f8e0(section_t *section) {
    // Check if the provided parameter (section) is NULL
    assert_halt(section!= NULL);
    /*if (section == NULL) {
        display_assert("section", "c:\\halo\\SOURCE\\cseries\\profile.c", 0x22f, 1);
        system_exit(-1); // Exit the system with error code -1
    }*/

    // Check if the 'active' flag of the section is not set (i.e., it's inactive)
    assert_halt(section->active!=0);
    /*if (section->active == 0) {
        display_assert("section->active", "c:\\halo\\SOURCE\\cseries\\profile.c", 0x230, 1);
        system_exit(-1); // Exit the system with error code -1
    }*/

    // Check if the section index is -1, meaning it has not been initialized
    if (section->index == -1) {
        // Check if we have reached the maximum number of profile sections allowed
        assert_halt(DAT_003361b0 < MAXIMUM_PROFILE_SECTIONS);
        /*if (DAT_003361b0 >= MAXIMUM_PROFILE_SECTIONS) {
            display_assert("profile_globals.section_count<MAXIMUM_PROFILE_SECTIONS",
                           "c:\\halo\\SOURCE\\cseries\\profile.c", 0x23a, 1);
            system_exit(-1); // Exit the system with error code -1
        }*/


        // Assign a new index to this section and increment the global section count
        section->index = DAT_003361b0++;
        DAT_003361b4[section->index] = section;

        // Initialize various fields of the section to zero or default values
        memset(section->memory_block2, 0, sizeof(section->memory_block2)); // Clear 960 bytes
        memset(section->memory_block1, 0, sizeof(section->memory_block1)); // Clear 480 bytes

        section->start_time_low = 0;
        section->start_time_high = 0;
        section->field_24 = 0;
        section->stack_depth = 0xFFFF; // Set to -1 (65535)
        section->field_5c8 = 0;
        section->profiling_counter = 0;
        section->field_5d0 = 0;
        section->field_5d4 = 0;
        section->field_5d8 = 0;
        section->field_5e0 = 0;
        section->field_5e4 = 0;
        section->field_5e8 = 0;
        section->field_5f0 = 0;
        section->field_5f4 = 0;
    } else {
        // If the section index is not -1, ensure it's valid and points to this section
        assert_halt_msg(!(section->index < 0 || section->index >= DAT_003361b0 || DAT_003361b4[section->index] != section),"don\'t call profile_enter_private(), call profile_enter()");
        /*if (section->index < 0 || section->index >= DAT_003361b0 || DAT_003361b4[section->index] != section) {
            display_assert("don\'t call profile_enter_private(), call profile_enter()",
                           "c:\\halo\\SOURCE\\cseries\\profile.c", 0x236, 1);
            system_exit(-1); // Exit the system with error code -1
        }*/
    }

    return; // Return normally if all checks pass
}
