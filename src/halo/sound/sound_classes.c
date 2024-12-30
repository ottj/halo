#include "../../common.h"
#include <string.h>
#include <math.h> // For isnan()
/* Certainly! Below is the rewritten function with added comments to help understand its purpose and
   logic. The function name and any called functions remain unchanged.
   */
// Global arrays
extern const char *sound_class_names[MAXIMUM_SOUND_CLASSES]; // Array of sound class names
extern short maximum_number_per_definition[MAXIMUM_SOUND_CLASSES]; // Per-definition limits
extern short maximum_number_per_object[MAXIMUM_SOUND_CLASSES];     // Per-object limits

// Global sound class data pointer
extern char *sound_class_data; // Base pointer for sound class data

short *FUN_001c88c0(short class_index) { //*get_sound_class_definition(short class_index) {
    // Check if the class index is within valid bounds
    assert_halt_msg(
        class_index >= 0 && class_index < MAXIMUM_SOUND_CLASSES,
        "class_index >= 0 && class_index < NUMBER_OF_SOUND_CLASSES"
    );

    // Check if the sound class name exists and is not empty
    assert_halt_msg(
        sound_class_names[class_index] != NULL && sound_class_names[class_index][0] != '\0',
        "sound_class_names[class_index][0]"
    );

    // Check if the maximum number per definition does not exceed the limit
    assert_halt_msg(
        maximum_number_per_definition[class_index] <= MAXIMUM_SOUND_INSTANCES_PER_DEFINITION,
        "definition->maximum_number_per_definition <= MAXIMUM_SOUND_INSTANCES_PER_DEFINITION"
    );

    // Check if the maximum number per object does not exceed the limit
    assert_halt_msg(
        maximum_number_per_object[class_index] <= MAXIMUM_SOUND_INSTANCES_PER_OBJECT_PER_DEFINITION,
        "definition->maximum_number_per_object <= MAXIMUM_SOUND_INSTANCES_PER_OBJECT_PER_DEFINITION"
    );

    // Return a pointer to the definition entry
    return &maximum_number_per_definition[class_index];
}


/* Certainly! Let's go through the function and add comments to better understand what it does. The
   function seems to be related to accessing sound class data based on an index.

   Assumptions and Structs:

    Global Variables:
        DAT_0050548c is a pointer to a block of sound class data. We'll define it as sound_class_data.
    Constants:
        MAXIMUM_SOUND_CLASSES remains 50.
        Each sound class occupies 12 bytes in the data block (from the unaff_SI * 0xc calculation).
Explanation of Changes:

    Macro Usage: Replaced raw error handling with assert_halt_msg for clarity and consistency.
    Readable Names: Used meaningful variable and function names (index, get_sound_class_data_offset).
    Globals: Used a global variable sound_class_data to represent DAT_0050548c.
    Constants: Used a named constant (MAXIMUM_SOUND_CLASSES) for clarity instead of hardcoding 0x32.
    Pointer Arithmetic: The calculation DAT_0050548c + unaff_SI * 0xc is translated to sound_class_data + index * 12.
 */

int FUN_001c89d0(short index){ //get_sound_class_data_offset(short index) {
    // Check if the index is within valid bounds
    assert_halt_msg(
        index >= 0 && index < MAXIMUM_SOUND_CLASSES,
        "index >= 0 && index < NUMBER_OF_SOUND_CLASSES"
    );

    // Check if the sound class data pointer is valid
    assert_halt_msg(
        sound_class_data != NULL,
        "sound_class_data"
    );

    // Return the offset into the sound class data for the given index
    return (int)(sound_class_data + index * 12);
}

/*
Assumptions and Analysis:

    Purpose: The function appears to adjust properties of sound classes based on the param_1 value, iterating over all sound classes (MAXIMUM_SOUND_CLASSES = 50).
    Global Data:
        Each sound class data entry contains:
            A float at offset 0 (field value).
            Another float at offset 1 (field current_value).
            A short at offset 2 (field instance_count).
Key Changes:

    Struct Use: Defined a sound_class_t struct to represent each sound class entry.
    Iteration: Replaced the do-while loop with a for loop, iterating from 0 to MAXIMUM_SOUND_CLASSES.
    Variable Names: Used descriptive names (adjustment, sound_class, instance_count) for clarity.
    Avoided Magic Numbers: Replaced 0x33 with MAXIMUM_SOUND_CLASSES for better maintainability.
    Type Safety: Used short for the loop index (index) to align with the type of the sound class data indices.
*/
void FUN_001c8c00(int adjustment) { // adjust_sound_classes(int adjustment) {
    // Iterate over all sound classes
    if (adjustment > 0) {
        for (short index = 0; index < MAXIMUM_SOUND_CLASSES; index++) {
            // Get the data offset for the sound class
            sound_class_t *sound_class = (sound_class_t *)FUN_001c89d0(index);// get_sound_class_data_offset(index);

            // Retrieve the current instance count
            short instance_count = sound_class->instance_count;

            if (adjustment < instance_count) {
                // Reduce instance count and adjust current value proportionally
                sound_class->instance_count -= adjustment;
                sound_class->current_value += 
                    (sound_class->value - sound_class->current_value) * 
                    ((float)adjustment / (float)instance_count);
            } else {
                // Set current value to value and reset instance count to 0
                sound_class->current_value = sound_class->value;
                sound_class->instance_count = 0;
            }
        }
    }
}
/*
To rewrite this function in a similar way, we will leverage the updated function get_sound_class_data_offset and use the new sound_class_t struct for clarity.
Explanation:

    Struct Usage: The function casts the result of get_sound_class_data_offset to a pointer to sound_class_t.
    Field Access: Instead of manually calculating offsets, it accesses the current_value field directly using the struct.
    Readable Name: The function is renamed to get_sound_class_current_value to reflect its purpose.
    Parameter: The function now takes an index parameter (short) to specify which sound class to query.

Functionality:

    This function retrieves the current_value of a specific sound class, identified by its index.

Example Usage:
float current_value = get_sound_class_current_value(5); // Get current value for sound class at index 5

*/
float  FUN_001c8c60(short index) {//get_sound_class_current_value(short index) {
    // Get the sound class data for the given index
    sound_class_t *sound_class = (sound_class_t *)FUN_001c89d0(index);//get_sound_class_data_offset(index);

    // Return the current value of the sound class
    return sound_class->current_value;
}


/*
Key Changes:

    Descriptive Names:
        Renamed the function to adjust_sound_class_by_name for clarity.
        Used descriptive parameter names (name, value, instance_count).

    Struct Usage:
        Replaced manual offset calculations with the sound_class_t struct.

    Clamping Logic:
        Used isnan() (from <math.h>) to handle NaN checks for value.
        Ensured value is clamped between 0.0 and 1.0.
        Prevented negative values for instance_count.

    Iteration:
        Iterated over MAXIMUM_SOUND_CLASSES using a for loop.

    Readable Conditions:
        Checked for non-NULL sound_class_names[index] and matched the name using strstr.

    Globals:
        Used sound_class_names, an array of strings holding sound class names.
*/
void FUN_001c8c80(const char *name, float value, short instance_count) {// adjust_sound_class_by_name(const char *name, float value, short instance_count) {
    for (short index = 0; index < MAXIMUM_SOUND_CLASSES; index++) {
        // Check if the current sound class name is valid and matches the input name
        if (sound_class_names[index] != NULL && strstr(sound_class_names[index], name) != NULL) {
            // Get the sound class data for the current index
            sound_class_t *sound_class = (sound_class_t *)FUN_001c89d0(index);//get_sound_class_data_offset(index);

            // Clamp the value between 0.0 and 1.0, treating NaN as 0.0
            float clamped_value = (!isnan(value) && value > 0.0f) ? (value > 1.0f ? 1.0f : value) : 0.0f;
            sound_class->value = clamped_value;

            // Ensure instance_count is non-negative
            sound_class->instance_count = (instance_count > 0) ? instance_count : 0;
        }
    }
}



#define NUMBER_OF_SOUND_CLASSES 51


typedef struct {
    float field_0;      // Offset 0x00: Initialized to 1.0f
    float field_4;      // Offset 0x04: Initialized to 1.0f
    uint16_t field_8;   // Offset 0x08: Initialized to 0
    uint16_t padding;   // Offset 0x0A: Padding for alignment
} sound_class_t;


/*
Key Improvements

    Macro-Based Error Handling:
        Replaced raw checks with assert_halt_msg to improve readability and ensure robust error handling.

    Readable Loop:
        Used a for loop instead of the do-while for cleaner iteration and removed unnecessary increment variables.

    Descriptive Variable Names:
        Renamed sVar2 to index for clarity.
        current_sound_class describes the pointer to the current sound class being processed.

    Initialization Logic:
        Clearly initializes each field of the sound class structure:
            0x3F800000 corresponds to 1.0f for the first two fields.
            The third field is set to 0.

    Maintainability:
        Removed hardcoded magic values and integrated meaningful names like 51 for the total number of sound classes.

Advantages of Using the Struct

    Readability:
        The struct provides clear names and offsets for each field, making the code easier to understand.

    Maintainability:
        If the structure of sound_class_data changes, updates can be made to the struct without affecting all the code.

    Alignment:
        Explicit padding ensures proper memory alignment for performance and compatibility.


*/
void sound_classes_initialize_for_new_map(void) {
    // Ensure the sound class data pointer is valid
    assert_halt_msg(
        sound_class_data != NULL,
        "sound_class_data must not be NULL"
    );

    // Cast the `sound_class_data` pointer to an array of `sound_class_t`
    sound_class_t *classes = (sound_class_t *)sound_class_data;

    // Iterate through each sound class and initialize
    for (int index = 0; index < 51; index++) {
        // Validate the index range
        assert_halt_msg(
            index >= 0 && index < 51,
            "index >= 0 && index < NUMBER_OF_SOUND_CLASSES"
        );

        // Initialize the current sound class
        classes[index].field_0 = 1.0f; // Set the first field to 1.0
        classes[index].field_4 = 1.0f; // Set the second field to 1.0
        classes[index].field_8 = 0;    // Set the third field to 0
    }
}



/*
sound_class_data was DAT_0050548c
Yes, the sound_class_t struct is compatible with the declared size in sound_classes_initialize, provided the allocation size (0x264) matches the expected total size of the array of sound classes.
Breakdown

    Size of sound_class_t:
        The sound_class_t struct has the following fields:
            field_0: float (4 bytes)
            field_4: float (4 bytes)
            field_8: uint16_t (2 bytes)
            padding: uint16_t (2 bytes)
        Total size: 4 + 4 + 2 + 2 = 12 bytes (0xC)

    Total Array Size:
        sound_classes_initialize allocates 0x264 bytes.
        0x264 in decimal is 612 bytes.

    Number of Entries:
        Each sound_class_t entry is 12 bytes.
        The total number of entries: 612 / 12 = 51.

    Conclusion:
        The sound_class_t struct and the allocation size are fully compatible.
        The array will contain exactly 51 sound class entries.
*/
void sound_classes_initialize(void)

{
    // Calculate the required allocation size
    size_t allocation_size = sizeof(sound_class_t) * NUMBER_OF_SOUND_CLASSES;

    // Allocate memory for the sound classes
    sound_class_data = (sound_class_t *)game_state_malloc("sound classes", 0, allocation_size);

  return;
}

void sound_classes_dispose_from_old_map(void)
{
  return;
}

void sound_classes_dispose(void)

{
  sound_class_data= 0; // DAT_0050548c = 0;
  return;
}
      
