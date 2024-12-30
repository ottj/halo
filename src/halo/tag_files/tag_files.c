#include "../../common.h"

/*
Key Improvements

    Macros for Error Handling:
        Used assert_halt_msg to validate the input and provide a meaningful error message.

    Descriptive Naming:
        file_path replaces param_1 to indicate the purpose of the input.
        last_backslash replaces pcVar1 to describe its role.

    Readable Logic:
        Clear and concise ternary operator determines the return value.
        Used '\\' explicitly instead of its ASCII code 0x5c for better readability.

    Modern Error Handling:
        Leveraged the assert_halt_msg macro to ensure the error handling is consistent and integrated into the broader system.
*/
// Function to extract the base name from a file path
char *FUN_0019b0d0(char *file_path) { //*extract_file_name(char *file_path) {
    // Validate the input using the provided assert macro
    assert_halt_msg(
        file_path != NULL,
        "file_path cannot be NULL"
    );

    // Find the last occurrence of the backslash character ('\')
    char *last_backslash = _strrchr(file_path, '\\');

    // Return the portion after the last backslash, or the original path if no backslash is found
    return (last_backslash != NULL) ? (last_backslash + 1) : file_path;
}
