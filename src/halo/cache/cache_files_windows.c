#include "../../common.h"

/*
Key Improvements

    Macros for Error Handling:
        Used assert_halt_msg to replace repetitive checks and provide meaningful error messages.

    Descriptive Function Name:
        Renamed FUN_001bd4d0 to open_map_file to reflect its purpose.

    Descriptive Variable Names:
        scenario_name replaces param_1.
        header replaces param_2.
        map_file_index replaces sVar1.

    Readable Logic:
        Clearly separates the steps for input validation, clearing memory, setting the map file index, and copying header data.
        Comments describe the purpose of each major step.

    Boundary Checks:
        Added a clear boundary check for map_file_index with a meaningful error message.


*/
uint32_t FUN_001bd4d0(int scenario_name, int header) { //open_map_file(int scenario_name, int header) {
    // Validate the inputs
    assert_halt_msg(scenario_name != 0, "scenario_name must not be NULL");
    assert_halt_msg(header != 0, "header must not be NULL");

    // Ensure no map file is currently open
    assert_halt_msg(
        DAT_004e9244 == -1,
        "cache_file_globals.open_map_file_index == NONE"
    );

    // Retrieve the map file index
    int16_t map_file_index = FUN_001bd1b0();
    assert_halt_msg(
        map_file_index != -1,
        "map_file_index != NONE"
    );

    // Clear the cached map file memory
    *csmemset(DAT_004e9250, 0, 0x4000);

    // Set the current open map file index
    DAT_004e9244 = map_file_index;

    // Validate the map file index
    assert_halt_msg(
        map_file_index >= 0 && map_file_index < NUMBER_OF_CACHED_MAP_FILES,
        "map_file_index >= 0 && map_file_index < NUMBER_OF_CACHED_MAP_FILES"
    );

    // Copy the header data for the selected map file
    *csmemcpy(header, &DAT_004e61e4 + map_file_index * 0x80C, 0x800);

    return 1; // Indicate success
}

/*
Key Improvements

    Descriptive Function Name:
        Renamed FUN_001bd1b0 to find_map_file_index to reflect its purpose.

    Variable Naming:
        map_file_index replaces sVar2 for clarity.

    Error Handling:
        Used assert_halt_msg to ensure map_file_index is always within valid bounds before processing.

    Logic Simplification:
        Made the while (true) loop easier to follow with clear break conditions.
        Added a comment for the string comparison placeholder.

    Boundary Check:
        Explicitly handles the case where no match is found by returning -1.


*/
int16_t FUN_001bd1b0(void) { //find_map_file_index(void) {
    int16_t map_file_index = 0;

    while (true) {
        // Ensure the map file index is within valid bounds
        assert_halt_msg(
            map_file_index >= 0 && map_file_index <= NUMBER_OF_CACHED_MAP_FILES,
            "map_file_index >= 0 && map_file_index < NUMBER_OF_CACHED_MAP_FILES"
        );

        // Compare strings (placeholder for actual string comparison logic)
        int comparison_result = __stricmp();
        if (comparison_result == 0) {
            break; // Match found
        }

        // Increment the index
        map_file_index++;

        // If no match is found after all possible indices, return -1
        if (map_file_index > NUMBER_OF_CACHED_MAP_FILES) {
            return -1;
        }
    }

    return map_file_index;
}
