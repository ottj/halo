
#include "../../common.h"
#include "../../tag_files/tag_files.h"
#include "../../tag_files/tag_groups.h"



typedef struct {
    char tags_loaded;  // Indicates whether tags are loaded
    int tag_count;     // Total number of tags
    void *tag_data;    // Pointer to tag data
} cache_file_globals_t;

// Global instances
extern cache_file_globals_t *cache_file_globals;
extern void *global_tag_instances;
extern int *tag_index_limit;
extern void *tag_data_base;
extern void *debug_context;


/*
Key Improvements

    Macros:
        Used assert_halt_msg for clean error handling and reduced repetitive code.

    Struct and Globals:
        Introduced the cache_file_globals_t struct with the tags_loaded field.
        Replaced raw variables like DAT_004e4d00, DAT_005054f0, and DAT_004e5504 with descriptive globals: cache_file_globals, global_tag_instances, tag_index_limit, and tag_data_base.

    Descriptive Names:
        Replaced sVar3 with tag_index_short for clarity.
        Replaced iVar2 with tag_entry_offset.

    Readable Logic:
        Structured the function to validate tags_loaded, global_tag_instances, and the bounds of tag_index step-by-step.

    Error Messages:
        Improved and reused error messages for debugging with assert_halt_msg and FUN_0008d9d0.
*/
int FUN_001b9bf0(uint32_t tag_index) { // resolve_tag_index(uint32_t tag_index) {
    // Ensure tags are loaded
    assert_halt_msg(
        cache_file_globals != NULL && cache_file_globals->tags_loaded != '\0',
        "cache_file_globals.tags_loaded"
    );

    // Ensure global tag instances are initialized
    assert_halt_msg(
        global_tag_instances != NULL,
        "global_tag_instances"
    );

    // Extract the lower 16 bits of the tag index
    int16_t tag_index_short = (int16_t)(tag_index & 0xFFFF);

    // Validate the tag index bounds
    assert_halt_msg(
        tag_index_short >= 0 && tag_index_short < *(tag_index_limit + 3),
        "Tag index out of bounds"
    );

    // Calculate the tag entry address
    int tag_entry_offset = tag_index_short * 0x20 + (int)tag_data_base;

    // Validate the full 32-bit tag index
    if ((tag_index & 0xFFFF0000) != 0 &&
        *(uint32_t *)(tag_entry_offset + 0xC) != tag_index) {
        uint32_t debug_message = FUN_0008d9d0(
            &debug_context,
            "i don't think %08x is a tag index"
        );
        display_assert(
            debug_message,
            "c:\\halo\\SOURCE\\cache\\cache_files.c",
            0x1DB,
            true
        );
        system_exit(-1);
    }

    return tag_entry_offset;
}




uint32_t FUN_0008d9d0(uint32_t param1, uint32_t param2) { // handle_stack_processing(uint32_t param1, uint32_t param2) {
    FUN_001da20(&param1, &param2, &param1 + 3); //process_with_stack(&param1, &param2, &param1 + 3); // Adjust stack pointer as needed
    return param1; // Return the first parameter as output
}


uint32_t FUN_001da20(uint8_t *input_data, uint32_t param2, uint32_t param3) { //process_data(uint8_t *input_data, uint32_t param2, uint32_t param3) {
    uint32_t result;
    uint8_t *current_ptr = input_data;
    uint8_t *base_ptr = input_data;
    int remaining_size = 0x7FFFFFFF; // Initialize to max int
    uint32_t constant_value = 0x42; // Placeholder constant value
    
    // Prepare a processing context
    void *processing_context[4] = { base_ptr, current_ptr, &remaining_size, &constant_value };
    
    // Perform the primary processing
    result = FUN_001de452((void **)&processing_context, param2, param3);// perform_processing((void **)&processing_context, param2, param3);

    // Handle buffer operations
    if (input_data != NULL) {
        remaining_size--;
        if (remaining_size < 0) {
            // Flush the buffer when limits are reached
            __flsbu(0, (void **)&processing_context); // flush_buffer(0, (void **)&processing_context);
        } else {
            // Null-terminate the buffer
            *current_ptr = 0;
        }
    }
    
    return result;
}


/*
Key Speculations

    Purpose:
        This function processes format specifiers in a custom format string (similar to printf).
        It handles flags (-, +, #, etc.), width, precision, and conversion specifiers (d, x, s, c, etc.).

    Function Calls:
        convert_to_string: Converts a number to its string representation based on the specified base (e.g., decimal or hexadecimal).
        output_string: Handles outputting strings to the buffer, applying flags, width, and precision.

    Structures:
        No new structs were introduced here, as the function mainly operates on input parameters and local variables. It processes a format string and writes to an output buffer.

    Improved Variable Names:
        format_string: The input format string being parsed.
        parameters: A list of parameters corresponding to the format specifiers.
        output_buffer: A buffer where the formatted result is stored.
        flags, width, and precision: Track the current format specifier's attributes.

    Logic Simplifications:
        Removed extraneous branching and replaced with clearer flag handling.
        Streamlined the loop to make it easier to follow.
*/
int FUN_001de452parse_format_specifiers(uint32_t *output_buffer, uint8_t *format_string, uint32_t *parameters) { // int parse_format_specifiers(uint32_t *output_buffer, uint8_t *format_string, uint32_t *parameters) {
    // Local variables with clearer names
    uint8_t current_char;
    uint32_t temp_value;
    int char_count = 0;
    int processing_state = 0;
    int sign_flag = 0;
    uint32_t flags = 0;
    uint32_t width = 0;
    uint32_t precision = 0xFFFFFFFF; // Default precision
    uint8_t *format_ptr = format_string;
    uint32_t *param_ptr = parameters;

    // Loop through each character in the format string
    while ((current_char = *format_ptr) != '\0') {
        format_ptr++; // Move to the next character

        // Handle various cases based on the character
        if (current_char == '%') {
            // Reset flags for a new format specifier
            flags = 0;
            width = 0;
            precision = 0xFFFFFFFF;
            continue; // Move to process the next character
        }

        // Check for flags (e.g., '-', '+', ' ', '#', '0')
        if (current_char == '-') {
            flags |= 0x04; // Left-align
        } else if (current_char == '+') {
            flags |= 0x01; // Always show sign
        } else if (current_char == ' ') {
            flags |= 0x02; // Insert space for positive numbers
        } else if (current_char == '#') {
            flags |= 0x80; // Alternate form
        } else if (current_char == '0') {
            flags |= 0x08; // Zero-padding
        } else if (current_char >= '1' && current_char <= '9') {
            // Parse width specifier
            width = current_char - '0';
            while (*format_ptr >= '0' && *format_ptr <= '9') {
                width = width * 10 + (*format_ptr - '0');
                format_ptr++;
            }
        } else if (current_char == '.') {
            // Parse precision specifier
            precision = 0;
            if (*format_ptr == '*') {
                precision = *param_ptr++;
                format_ptr++;
            } else {
                while (*format_ptr >= '0' && *format_ptr <= '9') {
                    precision = precision * 10 + (*format_ptr - '0');
                    format_ptr++;
                }
            }
        } else if (current_char == '*') {
            // Width specified by parameter
            width = *param_ptr++;
        } else {
            // Handle conversion specifiers (e.g., 'd', 'x', 's', etc.)
            switch (current_char) {
                case 'd': // Decimal integer
                case 'i': // Integer
                    temp_value = *param_ptr++;
                    if ((int)temp_value < 0) {
                        sign_flag = 1; // Negative number
                        temp_value = -temp_value; // Convert to positive for output
                    }
                    // Use existing external function to handle the string conversion
                    char_count += FUN_001e2879(output_buffer, temp_value);
                    break;

                case 'x': // Hexadecimal (lowercase)
                case 'X': // Hexadecimal (uppercase)
                    temp_value = *param_ptr++;
                    char_count += FUN_001e2879(output_buffer, temp_value);
                    break;

                case 's': // String
                    char_count += _write_string((char *)*param_ptr++, flags, width, precision);
                    break;

                case 'c': // Character
                    _write_multi_char(current_char, 1, output_buffer);
                    char_count++;
                    break;

                default:
                    // Handle unknown specifier gracefully (can be expanded as needed)
                    break;
            }
        }
    }

    // Return the number of characters written to the output buffer
    return char_count;
}


/*
Key Improvements

    Descriptive Names:
        buffer for param_1, indicating it’s the target for the character to be written.
        value for param_2, indicating it’s the character value being written.

    Clear Logic:
        Explicit checks for NULL and out-of-range values.
        Returns clearly indicate success (1), failure due to NULL (0), or error (0xFFFFFFFF).

    Error Handling:
        Retained the call to FUN_001db777 and assumed it initializes or handles an error-related structure. Updated the assignment to use ' * ' as a symbolic representation for error cases.
      */
uint32_t FUN_001e2879(void *buffer, uint16_t value) { // write_char_to_buffer(void *buffer, uint16_t value) {
    // Check if the buffer is valid
    if (buffer == NULL) {
        return 0; // Indicate failure when the buffer is NULL
    }

    // Check if the value is within the allowed range
    if (value > 0xFF) {
        // Use external function to handle the error case
        uint32_t *error_handler = (uint32_t *)FUN_001db777();
        *error_handler = '*'; // Write '*' as an error indicator
        return 0xFFFFFFFF;    // Indicate error
    }

    // Write the value as a character to the buffer
    *(char *)buffer = (char)value;
    return 1; // Indicate success
}

/*
Key Improvements

    Descriptive Naming:
        Renamed iVar1 to base_value to reflect that it stores the result of FUN_001dff91.
        Renamed the function to calculate_offset to describe its purpose.

    Simplified Return:
        Directly adds 8 to the result of FUN_001dff91 and returns it.

*/
int calculate_offset(void) {
    // Call the external function and add 8 to its result
    int base_value = FUN_001dff91();
    return base_value + 8;
}


// Assumes external functions and variables are declared elsewhere
extern void KeBugCheck(int code);
extern void FUN_001e3c37(int param);
extern uint32_t FUN_001cfde0();
extern int XAPILIB::_tls_index;

/*
Key Improvements

    Descriptive Names:
        fs_base_address represents the FS segment base address for better clarity.
        tls_storage_ptr for the TLS storage location.
        buffer_ptr for the buffer being allocated or retrieved.
        Retained original variable and function names like FUN_001df1bd, KeBugCheck, and FUN_001cfde0.

    Comments and Flow:
        Added comments to clarify the logic behind FS checks, TLS pointer retrieval, and buffer initialization.
        Streamlined logic to improve readability without altering functionality.

    Memory Allocation Handling:
        Clearly separates the logic for buffer allocation and initialization.
        Handles allocation failures gracefully with the existing FUN_001e3c37.

    Preserved Functionality:
        Ensured all external function calls, memory initialization, and return behavior match the original function.
*/

// Improved function definition
uint32_t *FUN_001db777(void) { // *allocate_or_retrieve_tls_buffer(void) {
    // Local variables with descriptive names
    uint32_t *buffer_ptr;
    uint32_t default_value;
    int *tls_storage_ptr;
    int *fs_base_address = (int *)__readfsdword(0x24); // Read FS segment base

    // Check FS segment integrity
    if (fs_base_address[0x24] > 1) {
        KeBugCheck(10); // Trigger bug check if integrity fails
    }

    // Retrieve TLS storage pointer or use default
    if (*(int *)(fs_base_address[0x28] + 0x28) == 0) {
        tls_storage_ptr = &DAT_00632ca4; // Fallback to default global variable
    } else {
        tls_storage_ptr = (int *)(*(int *)(fs_base_address[4] + XAPILIB::_tls_index * 4) + 8);
    }

    // Retrieve the buffer pointer
    buffer_ptr = (uint32_t *)*tls_storage_ptr;
    if (buffer_ptr == NULL) {
        // Allocate a new buffer if none exists
        buffer_ptr = (uint32_t *)FUN_001df1bd(1, 0x84);
        if (buffer_ptr == NULL) {
            FUN_001e3c37(0x10); // Handle allocation failure
        } else {
            // Initialize the newly allocated buffer
            *tls_storage_ptr = (int)buffer_ptr;
            buffer_ptr[0x15] = (uint32_t)&DAT_00331988; // Assign a specific value
            buffer_ptr[5] = 1; // Set a specific buffer field
            default_value = FUN_001cfde0(); // Retrieve a default initialization value
            buffer_ptr[1] = 0xFFFFFFFF; // Set an initial flag
            *buffer_ptr = default_value; // Initialize the first field
        }
    }

    return buffer_ptr;
}


/*
Key Improvements

    Descriptive Names:
        count and size: Parameters for the number of elements and size per element.
        total_size: The calculated total memory size to allocate.
        aligned_size: Tracks size adjustments for alignment.
        allocation_result: Stores the result of memory allocation.

    Clear Logic:
        Explicitly checks total_size for zero to ensure a minimum allocation of 1.
        Added alignment logic in a readable format.

    Memory Handling:
        Clearly separates the logic for raw memory allocation (FUN_001d0bb3) and alignment verification (FUN_001d5c66).
        Includes fallback handling with __callnewh when the primary allocation fails.

    Preserved Behavior:
        Ensures all external calls (FUN_001d0bb3, FUN_001d5c66, __callnewh) and conditions (e.g., DAT_00632ca0, DAT_004fc36c) are retained and behave as intended in the original function.
*/
int FUN_001df1bd(int count, int size) { // allocate_memory_with_alignment(int count, int size) {
    // Local variables with descriptive names
    uint32_t total_size = count * size;
    uint32_t aligned_size;
    int allocation_result = 0;
    void *raw_memory;
    uint32_t allocation_flags = 8;

    // Ensure total_size is at least 1
    if (total_size == 0) {
        total_size = 1;
    }

    // Try allocating memory with alignment
    do {
        allocation_result = 0; // Reset allocation result

        // Check for valid size range
        if (total_size < 0xFFFFFFE1) {
            // Adjust size for alignment if required
            if (DAT_00632ca0 == 3) {
                total_size = (total_size + 0xF) & 0xFFFFFFF0; // Align to 16 bytes
            }

            // Attempt memory allocation
            raw_memory = FUN_001d0bb3(allocation_flags, total_size); // Allocate raw memory
            allocation_result = FUN_001d5c66(raw_memory, allocation_flags, total_size); // Verify alignment

            // If successful, return the allocated memory
            if (allocation_result != 0) {
                return allocation_result;
            }
        }

        // If no custom memory handler, return failure
        if (DAT_004fc36c == 0) {
            return 0;
        }

        // Attempt allocation using fallback handler
        allocation_result = __callnewh(total_size);

    } while (allocation_result != 0);

    return 0; // Return failure if all attempts fail
}
/*
Key Improvements

    Descriptive Function Name:
        Renamed the function to handle_error_and_cleanup to better describe its purpose.

    Descriptive Parameter Name:
        param_1 renamed to error_code to clarify its role as an input for error handling.

    Improved Readability:
        Added comments to clarify the steps involved: cleanup, error-specific handling, and finalization.
        Maintained the external function calls (FUN_001e59c1, FUN_001e596b, and PTR_FUN_00331940).

    Preserved Behavior:
        The function behavior remains unchanged, with all calls and parameters handled as in the original.
        */
void FUN_001e3c37(uint32_t error_code) { // handle_error_and_cleanup(uint32_t error_code) {
    // Perform initial cleanup or preparation
    FUN_001e59c1();

    // Handle the specific error code
    FUN_001e596b(error_code);

    // Finalize with a termination function
    ((void (*)(uint32_t))PTR_FUN_00331940)(0xFF);

    return;
}

/*
Key Improvements

    Descriptive Function Name:
        Renamed FUN_001cfde0 to get_tls_data_offset to reflect its purpose of retrieving a TLS data value.

    Descriptive Variable Name:
        fs_base_address indicates that the variable holds the base address of the FS segment.

    Clear Calculation:
        Explained the offset 300 as 75 * 4 bytes, making it more intuitive for a reader.

    Preserved Behavior:
        The functionality remains unchanged, retrieving the value from the specified FS segment offset.
*/
uint32_t FUN_001cfde0(void){  // get_tls_data_offset(void) {
    // Retrieve the TLS data offset from the FS segment
    int *fs_base_address = (int *)__readfsdword(0x28); // Read the base address from FS segment
    return *(uint32_t *)(fs_base_address + 75); // Access the value at offset 300 (75 * 4 bytes)
}

/*
Key Improvements

    Descriptive Function Name:
        Renamed FUN_001e59c1 to perform_cleanup_actions to describe its purpose better.

    Descriptive Comments:
        Added comments to explain each step: the initial cleanup, callback execution (if present), and final cleanup.

    Descriptive Conditions:
        Explicitly checked if DAT_004fc378 is NULL before invoking it, ensuring clarity.

    Preserved Behavior:
        Retained the original logic and behavior, including the external function calls (FUN_001e596b and DAT_004fc378).

*/
void FUN_001e59c1(void) { // perform_cleanup_actions(void) {
    // Perform the first cleanup step with a specific code
    FUN_001e596b(0xFC);

    // If a callback is registered, execute it
    if (DAT_004fc378 != NULL) {
        ((void (*)())DAT_004fc378)();
    }

    // Perform the final cleanup step with a specific code
    FUN_001e596b(0xFF);

    return;
}

/*
Key Improvements

    Descriptive Function Name:
        Renamed FUN_001e596b to log_debug_message to reflect its purpose of logging a debug message.

    Variable Naming:
        Renamed uVar1 to index to represent the array index being iterated.

    Simplified Logic:
        Retained the loop for searching the code and used a clear break when the code is found.

    Comments:
        Added comments to explain the logic of searching for the code and logging the message.

    Preserved Behavior:
        The functionality is unchanged; it searches for param_1 in an array and outputs a debug string if found.
*/
void FUN_001e596b(int code) { // log_debug_message(int code) {
    uint32_t index = 0;

    // Search for the code in the DAT_00331bb0 array
    while (index < 0x12) {
        if (code == (&DAT_00331bb0)[index * 2]) {
            break; // Code found
        }
        index++;
    }

    // If the code is found, output the corresponding debug string
    if (code == (&DAT_00331bb0)[index * 2]) {
        XAPILIB::OutputDebugStringA(*(uint32_t *)(index * 8 + 0x331bb4));
    }

    return;
}



// External function declarations
//extern uint32_t FUN_0019b0d0(uint32_t param);
extern void FUN_001be940();
extern void FUN_001bdec0();
extern char FUN_001bd4d0(uint32_t value, void *data);
extern char FUN_001b9ce0(void *data, uint32_t param, int flag);
extern void FUN_001bc9e0(int a1, int a2, int a3, void *buffer, int offset, int flag);
extern void FUN_001bccb0(void *data);
extern void *FUN_0008d9d0(void *context, const char *format, ...);

// Global variables
extern uint32_t *DAT_004e5504;
extern void *DAT_005054f0;
extern char DAT_004e4d00;
extern void *DAT_004e4d04;
extern int DAT_004e4d14;
extern int DAT_004e4d18;
extern void *DAT_005ab100;


/*
Key Improvements

    Descriptive Function Name:
        Renamed FUN_001b9e70 to initialize_cache_file to indicate its purpose.

    Meaningful Variable Names:
        result: The result of the initialization.
        cache_buffer: The buffer allocated for cache processing.
        is_complete: A flag to indicate whether the processing is finished.

    Logic Simplification:
        Clear steps for initialization, validation, memory handling, and signature verification.
        Reduced reliance on inline magic values by using descriptive comments.

    Signature Verification:
        Explicitly checks for the ASCII "tags" signature with improved error messaging.

    Maintainability:
        Modularized each section with clear comments and reused meaningful external calls.

    Error Handling:
        Preserved and clarified error-handling logic using descriptive messages.


*/
uint32_t FUN_001b9e70(uint32_t param) { // initialize_cache_file(uint32_t param) {
    uint32_t result = FUN_0019b0d0(param); // Initialize and get result
    FUN_001be940(); // Perform some setup
    FUN_001bdec0(); // Additional setup

    // Validate cache signature
    if (FUN_001bd4d0(result, &DAT_004e4d04)) {
        uint32_t *cache_buffer = (uint32_t *)FUN_001bdd50();

        if (FUN_001b9ce0(&DAT_004e4d04, param, 1)) {
            // Clear the buffer with a specific pattern
            memset(cache_buffer, 0xCD, 0x1600000);

            // Process cache data
            uint8_t is_complete = 0;
            FUN_001bc9e0(0xFFFFFFFF, DAT_004e4d14, DAT_004e4d18, cache_buffer, (int)&is_complete, 1);

            // Wait until processing is complete
            while (!is_complete) {
                XAPILIB::SwitchToThread();
            }

            // Assign global cache buffer
            DAT_004e5504 = cache_buffer;

            // Verify signature
            if (cache_buffer[8] != 0x74616773) { // 'tags' in ASCII
                FUN_0008d9d0(
                    &DAT_005ab100,
                    "signature is '%c%c%c%c', should be 'tags'",
                    *(char *)((int)cache_buffer + 0x23),
                    *(char *)((int)cache_buffer + 0x22),
                    *(char *)((int)cache_buffer + 0x21),
                    (char)cache_buffer[8],
                    't', 'a', 'g', 's',
                    "c:\\halo\\SOURCE\\cache\\cache_files.c", 0x61, 1
                );
                system_exit(0xFFFFFFFF);
            }

            // Initialize global tag instances and set flags
            DAT_005054f0 = *DAT_004e5504;
            FUN_001bccb0(DAT_004e5504);
            DAT_004e4d00 = 1;

            // Return result
            return DAT_004e5504[1];
        }
    }

    // Return error code on failure
    return 0xFFFFFFFF;
}




/*
Explanation

    Function Names:
        Renamed FUN_001be940 to initialize_global_data to describe its purpose.
        Renamed FUN_001bdec0 to initialize_secondary_global_data to differentiate it from the primary initialization function.

    Struct Usage:
        Cast DAT_004ea978 and DAT_004e9368 to data_t * for clarity and alignment with the mark_data_valid_and_initialize function.

    Readability:
        Simplified the logic by directly calling the updated mark_data_valid_and_initialize function with clear comments about the purpose of each call.


*/
void FUN_001be940(void) { //initialize_global_data(void) {
    // Initialize the global data structure at DAT_004ea978
    FUN_00119b20((data_t *)DAT_004ea978); //mark_data_valid_and_initialize((data_t *)DAT_004ea978);
}

void FUN_001bdec0(void) { //initialize_secondary_global_data(void) {
    // Initialize the secondary global data structure at DAT_004e9368
    FUN_00119b20((data_t *)DAT_004e9368); //mark_data_valid_and_initialize((data_t *)DAT_004e9368);
}



/*
Explanation

    Struct Usage:
        Replaced param_1 with a pointer to data_t for clarity and better alignment with the structure.
        Used data->valid to set the valid field instead of directly accessing the memory offset.

    Descriptive Function Name:
        Renamed FUN_00119b20 to mark_data_valid_and_initialize to describe its purpose.

    Modular Call:
        Replaced FUN_00119720 with initialize_data to match the previously rewritten initialization function.

    Readability:
        Simplified the logic to be more expressive and readable while preserving functionality.

*/
void FUN_00119b20(data_t *data) { //mark_data_valid_and_initialize(data_t *data) {
    // Verify the integrity of the data structure
    data_verify(data);

    // Mark the data as valid
    data->valid = true;

    // Initialize the data structure
    FUN_00119720(data); //initialize_data(data);
}

void *FUN_001bdd50(void)
{
  return DAT_004e9258;
}


/*
Key Improvements

    Descriptive Naming:
        validate_cache_file replaces FUN_001b9ce0 to reflect the purpose.
        Renamed variables for clarity:
            header replaces param_1 to represent the cache file header.
            strict_mode replaces param_3 for clarity in error handling mode.

    Error Handling:
        Used goto for error conditions to avoid deeply nested conditions.
        Retained calls to FUN_0008d9d0 for formatted error messages, aligning with existing behavior.

    Readable Logic:
        Simplified the structure validation and included clear checks for cache file length, version, and build compatibility.
        Encapsulated error message logic with meaningful constants (error_line and error_message).

    Boundary Conditions:
        Added explicit checks for cache file version and invalid headers with appropriate messages.

    Preserved Functionality:
        Maintained the original logic and external calls (csstrlen, FUN_0008dcb0, and FUN_0008d9d0) while improving structure and readability.
*/
uint32_t FUN_001b9ce0(int *header, uint32_t param_2, char strict_mode) { // validate_cache_file(int *header, uint32_t param_2, char strict_mode) {
    // Validate basic cache file structure
    if (header[0] == 0x68656164 && header[0x1FF] == 0x666f6f74 && header[2] >= 0 && header[2] < 0x11600001) {
        // Check the length of the cache file name
        uint32_t name_length = csstrlen((char *)(header + 8));
        if (name_length > 0x1F) {
            goto invalid_cache_file;
        }

        // Check the version of the cache file
        if (header[1] == 5) {
            int version_check = FUN_0008dcb0(header + 0x10, "01.10.12.2276");
            if (version_check == 0) {
                return 1; // Valid cache file
            }

            // Handle version mismatch
            if (strict_mode == '\0') {
                return 0; // Allow non-strict handling
            }
            uint32_t error_message = FUN_0008d9d0(
                &DAT_005ab100,
                "The cache file '%s' belongs to a different build (%s)",
                (char *)(header + 8),
                (char *)(header + 0x10),
                "c:\\halo\\SOURCE\\cache\\cache_files.c",
                0x1F6,
                1
            );
            display_assert(error_message);
            system_exit(-1);
        }

        // Handle outdated cache file
        if (strict_mode == '\0') {
            return 0;
        }
        uint32_t error_line = 0x1F1;
        const char *error_message = "The cache file '%s' is an old version";
        uint32_t error_code = FUN_0008d9d0(
            &DAT_005ab100,
            error_message,
            (char *)(header + 8),
            "c:\\halo\\SOURCE\\cache\\cache_files.c",
            error_line,
            1
        );
        display_assert(error_code);
        system_exit(-1);
    } else {
    invalid_cache_file:
        // Handle invalid cache file structure
        if (strict_mode == '\0') {
            return 0;
        }
        uint32_t error_line = 0x1ED;
        const char *error_message = "'%s' does not appear to be a cache file";
        uint32_t error_code = FUN_0008d9d0(
            &DAT_005ab100,
            error_message,
            param_2,
            "c:\\halo\\SOURCE\\cache\\cache_files.c",
            error_line,
            1
        );
        display_assert(error_code);
        system_exit(-1);
    }

    return 0; // Default return for failure
}
