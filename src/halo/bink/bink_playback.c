#include "../../common.h"

void bink_playback_initialize(void) {
    // Clear the bink playback globals
    csmemset(&bink_playback_globals, 0, sizeof(bink_playback_globals));

    // Register memory allocation and deallocation functions
    register_bink_memory_functions(bink_allocate_memory_block, bink_free_memory_block);

    // Mark the playback as initialized
    bink_playback_globals.playback_initialized = 1;
}


void bink_playback_dispose(void) {
    // Check if playback was initialized
    if (bink_playback_globals.playback_initialized) {
        // Perform cleanup operations
        bink_playback_cleanup();

        // Clear the playback globals
        csmemset(&bink_playback_globals, 0, sizeof(bink_playback_globals));
    }
}

void register_bink_memory_functions(int (*allocate_function)(int), void (*free_function)(int)) {
    bink_memory_allocate = allocate_function;
    bink_memory_free = free_function;
}


/* uint32_t bink_allocate_aligned_memory(uint32_t alignment, uint32_t size_in_bytes) {
    uint32_t aligned_address;

    // Calculate the aligned memory address
    aligned_address = (bink_globals.memory_pool_size - size_in_bytes) + bink_globals.memory_pool_base;

    // Validate alignment and adjust if necessary
    if (alignment != 0 && (aligned_address & (alignment - 1)) != 0) {
        // Ensure alignment is a power of 2
        error(2,"Debug: alignment=%u, aligned_address=0x%X", alignment, aligned_address);
        error(2,"Debug: size_in_bytes=%u", size_in_bytes);
        assert_halt_msg(
            alignment > 0 && (alignment & (alignment - 1)) == 0,
            "alignment_in_bytes must be > 0 and a power of 2"
        );

        // Adjust size and address for alignment
        size_in_bytes += alignment - (aligned_address & (alignment - 1));
        aligned_address -= (alignment - 1) & (alignment - aligned_address);
    }

    // Validate memory allocation size
    assert_halt_msg(size_in_bytes > 0, "size_in_bytes must be > 0");
    assert_halt_msg(
        bink_globals.memory_pool_size >= size_in_bytes,
        "bink_globals.memory_pool_size must be >= size_in_bytes"
    );

    // Validate memory pool base
    assert_halt_msg(
        bink_globals.memory_pool_base != 0,
        "bink_globals.memory_pool_base must be initialized"
    );

    // Update the memory pool size to reflect the allocation
    bink_globals.memory_pool_size -= size_in_bytes;

    return aligned_address;
} */


/* int bink_allocate_memory_block(int size_in_bytes) {
    int allocated_address;
    char retry_success;
    uint32_t local_buffer[3] = {0};
    uint32_t alignment = 0;

    // Initialize local buffer and alignment
    csmemset(local_buffer, 0, sizeof(local_buffer));
    local_buffer[0] = 0x20;
    query_memory_statistics(local_buffer);
    _DAT_0032eb9c = alignment >> 10;

    // Handle potential memory block misalignment
    if (bink_globals.pointer_block_count > 0 && bink_globals.pointer_blocks == NULL) {
        retry_success = FUN_001c5a80();
        if (!retry_success) {
            display_assert(
                "### FATAL_ERROR bink just confused the hell out of me (1)",
                "c:\\halo\\SOURCE\\bink\\bink_playback.c", 0x2df, 1
            );
            system_exit(0xffffffff);
        }
        retry_success = FUN_001c5a80();
        if (retry_success) {
            bink_globals.pointer_block_count = 0;
            bink_globals.memory_pool_offset = 0;
        }
    }

    // Validate memory allocation conditions
    if ((bink_globals.memory_pool_offset + size_in_bytes <= bink_globals.memory_pool_size) &&
        (bink_globals.pointer_block_count < 0x10) &&
        (bink_globals.memory_pool_base != 0)) {

        // Calculate allocated address
        allocated_address = bink_globals.memory_pool_base + bink_globals.memory_pool_offset;

        // Update memory pool offset
        bink_globals.memory_pool_offset += size_in_bytes;

        // Protect allocated memory
        FUN_001d371d(allocated_address, size_in_bytes, 4);

        // Validate memory pool state
        if (bink_globals.memory_pool_offset > bink_globals.memory_pool_size) {
            display_assert(
                "### FATAL_ERROR bink needs more memory (requested %d bytes over the %d-byte limit)",
                bink_globals.memory_pool_offset - bink_globals.memory_pool_size,
                bink_globals.memory_pool_size,
                "c:\\halo\\SOURCE\\bink\\bink_playback.c", 0x312, 1
            );
            system_exit(0xffffffff);
        }

        // Validate pointer block count
        if (bink_globals.pointer_block_count >= 0x10) {
            display_as */

bool are_all_pointer_blocks_cleared(void) {
    // Default return value indicates all blocks are cleared
    bool all_cleared = true;

    // Check each pointer block if the count is greater than 0
    if (bink_globals.pointer_block_count > 0) {
        for (int i = 0; i < bink_globals.pointer_block_count; i++) {
            if ((&bink_pointer_blocks)[i] != 0) {
                all_cleared = false;
                break;
            }
        }
    }

    return all_cleared;
}
