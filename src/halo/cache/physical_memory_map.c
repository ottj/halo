#include "../../common.h"

typedef struct {
    void *game_state_base_address;     // Base address for the game state
    void *tag_cache_base_address;      // Base address for the tag cache
    void *texture_cache_base_address;  // Base address for the texture cache
    void *sound_cache_base_address;    // Base address for the sound cache
} physical_memory_map_globals_t;

// Global variable for physical memory map
physical_memory_map_globals_t physical_memory_map_globals;
#define GAME_STATE_BASE_ADDRESS -0x7ff9f000 //DAT_00345000
#define TAG_CACHE_BASE_ADDRESS -0x7fc5a000 //DAT_003a6000

/*
Key Improvements

    Struct for Globals:
        Replaced global variables like DAT_004e9254 with a structured and descriptive physical_memory_map_globals_t.

    Descriptive Function Calls:
        Used the rewritten allocate_contiguous_memory function for memory allocation.

    Improved Readability:
        Added meaningful variable and field names for clarity (game_state_base_address, tag_cache_base_address, etc.).
        Clearly separated each memory allocation and its corresponding error check.

    Error Handling:
        Used assert_halt_msg for robust and readable error checking.
        Included meaningful messages for each assertion to facilitate debugging.

    Modern Practices:
        Encapsulated related fields in a struct for better maintainability and context.
        Made use of NULL for pointers instead of 0 where appropriate.


*/
void physical_memory_allocate(void) {
    // Allocate game state memory
    physical_memory_map_globals.game_state_base_address =
        (void *)FUN_001d36d4(0x61000, (int)&DAT_00345000, 0, 4);
    assert_halt_msg(
        (unsigned long)physical_memory_map_globals.game_state_base_address == GAME_STATE_BASE_ADDRESS,
        "physical_memory_map_globals.game_state_base_address == GAME_STATE_BASE_ADDRESS"
    );

    // Allocate tag cache memory
    physical_memory_map_globals.tag_cache_base_address =
        (void *)FUN_001d36d4(0x1600000, (int)&DAT_003a6000, 0, 4);
    assert_halt_msg(
        (unsigned long)physical_memory_map_globals.tag_cache_base_address == TAG_CACHE_BASE_ADDRESS,
        "physical_memory_map_globals.tag_cache_base_address == TAG_CACHE_BASE_ADDRESS"
    );

    // Allocate texture cache memory
    physical_memory_map_globals.texture_cache_base_address =
        (void *)FUN_001d36d4(0x1600000, -1, 0, 0x404);
    assert_halt_msg(
        physical_memory_map_globals.texture_cache_base_address != NULL,
        "physical_memory_map_globals.texture_cache_base_address"
    );

    // Allocate sound cache memory
    physical_memory_map_globals.sound_cache_base_address =
        (void *)FUN_001d36d4(0x400000, -1, 0, 4);
    assert_halt_msg(
        physical_memory_map_globals.sound_cache_base_address != NULL,
        "physical_memory_map_globals.sound_cache_base_address"
    );
}


/*
Key Improvements

    Struct Usage:
        Replaced global variables like DAT_004e9258 and DAT_004e9254 with fields in the physical_memory_map_globals struct for clarity.

    Function Name:
        Renamed FUN_001bdc60 to verify_memory_pages to better describe its purpose.

    Readable Logic:
        Used meaningful variable names:
            current_address: Tracks the current memory address being verified.
            tag_cache_end, game_state_end: Indicate the end address for each memory region.

    Page Size Constant:
        Added 0x1000 explicitly as the page size (4 KB), making the logic easier to understand.

    Error Handling:
        Used assert_halt_msg for robust error checks with clear messages.

    Maintainability:
        Encapsulated memory verification logic in a clear structure with separate loops for each memory region.
*/
void FUN_001bdc60(void) { // verify_memory_pages(void) {
    int page_status;
    uint32_t current_address;

    // Verify tag cache memory pages
    current_address = (uint32_t)physical_memory_map_globals.tag_cache_base_address;
    uint32_t tag_cache_end = current_address + 0x1600000; // 22 MB
    while (current_address < tag_cache_end) {
        page_status = MmQueryAddressProtect((void *)current_address);
        assert_halt_msg(
            page_status == 4,
            "page_status == PAGE_READWRITE"
        );
        current_address += 0x1000; // Advance by one page (4 KB)
    }

    // Verify game state memory pages
    current_address = (uint32_t)physical_memory_map_globals.game_state_base_address;
    uint32_t game_state_end = current_address + 0x61000; // Size based on prior allocation
    while (current_address < game_state_end) {
        page_status = MmQueryAddressProtect((void *)current_address);
        assert_halt_msg(
            page_status == 4,
            "page_status == PAGE_READWRITE"
        );
        current_address += 0x1000; // Advance by one page (4 KB)
    }
}



/*
Key Improvements

    Descriptive Function Name:
        Renamed FUN_001d36d4 to allocate_contiguous_memory to describe its purpose.

    Descriptive Parameter Names:
        size: The size of the memory to allocate.
        start_address: The starting address for the allocation.
        alignment: Memory alignment requirements.
        flags: Additional flags for allocation.

    Readable Logic:
        Simplified the conditional logic for setting start_address and end_address.
        Made the adjustment of alignment and calculation of end_address more explicit.

    Error Handling:
        Added a clear comment for the error handling block where XAPILIB::SetLastError is called.

    Preserved Behavior:
        The functionality remains unchanged: it allocates memory using MmAllocateContiguousMemoryEx and handles both successful and failed allocations.
*/
int FUN_001d36d4(int size, int start_address, uint32_t alignment, uint32_t flags) { //allocate_contiguous_memory(int size, int start_address, uint32_t alignment, uint32_t flags) {
    int end_address;

    // Adjust start and end addresses based on input
    if (start_address == -1) {
        start_address = 0;
        end_address = -1; // No end address restriction
    } else {
        alignment = 0; // Alignment is ignored when a specific start address is provided
        end_address = start_address + size - 1;
    }

    // Attempt to allocate contiguous memory
    int allocation_result = MmAllocateContiguousMemoryEx(size, start_address, end_address, alignment, flags);

    // Handle allocation failure
    if (allocation_result == 0) {
        XAPILIB::SetLastError(8); // Set error code indicating allocation failure
    }

    return allocation_result;
}