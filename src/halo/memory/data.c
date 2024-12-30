#include "../../common.h"

int data_allocation_size(__int16 count, __int16 size)
{
  return size * count + sizeof(data_t);
}

void data_initialize(data_t *data, char *name, __int16 maximum_count,
                     __int16 size)
{
  assert_halt(maximum_count > 0);
  assert_halt(size > 0);
  assert_halt(name);
  assert_halt(data);

  csmemset(data, 0, sizeof(*data));
  csstrncpy(data->name, name, sizeof(data->name) - 1);
  data->maximum_count = maximum_count;
  data->size = size;
  data->magic = 0x64407440;
  data->data = &data[1];
  data->valid = 0;
}

void data_make_invalid(data_t *data)
{
  data_verify(data);
  data->valid = 0;
}

void data_iterator_new(data_iter_t *iter, data_t *data)
{
  data_verify(data);
  assert_halt(data->valid);

  iter->data = data;
  iter->cookie = (unsigned int)data ^ 0x69746572;
  iter->index = 0;
  iter->datum_index = INVALID_DATUM_HANDLE;
}

void *data_iterator_next(data_iter_t *iterator)
{
  int16_t index; // cx
  void *result; // eax
  int handle; // edx
  size_t size; // [esp+14h] [ebp+8h]

  assert_halt_msg(iterator->cookie == ((int)iterator->data->name ^ 0x69746572),
                  "uninitialized iterator passed to iterator_next()");
  data_verify(iterator->data);
  assert_halt(iterator->data->valid);

  index = iterator->index;
  size = iterator->data->size;
  result = (char *)iterator->data->data + iterator->data->size * index;
  if (index >= iterator->data->current_count) {
    iterator->index = index;
    return 0;
  } else {
    while (1) {
      handle = index++ | (*(__int16 *)result << 16);
      if (*(_WORD *)result)
        break;
      result = (char *)result + size;
      if (index >= iterator->data->current_count) {
        iterator->index = index;
        return 0;
      }
    }
    iterator->datum_index.handle = handle;
    iterator->index = index;
  }
  return result;
}


/*
Key Improvements

    Descriptive Function Name:
        Renamed FUN_00119720 to process_data_structure to reflect its purpose.

    Descriptive Variable Names:
        data_ptr replaces param_1, indicating a pointer to a data structure.
        short_field indicates the specific field being modified.
        element_count, element_size, and array_base clarify their roles in the array initialization.

    Error Handling with Macros:
        Used assert_halt_msg to validate that the data is marked as valid (data->valid).

    Initialization Logic:
        Made the initialization of fields (0x2C, 0x2E, 0x30, and 0x32) more explicit and readable.
        Added comments for each operation.

    Iteration Simplification:
        Used a for loop to iterate over the elements, making the logic cleaner and easier to follow.

    Readable Bit Manipulation:
        Clarified the purpose of *short_field |= 0x8000 by describing it as setting the highest bit.
*/
void FUN_00119720(data_t *data) { // initialize_data(data_t *data) {
    // Verify the integrity of the data
    data_verify(data);

    // Ensure the data is marked as valid
    assert_halt_msg(
        data->valid,
        "data->valid"
    );

    // Reset relevant fields
    data->current_count = 0;
    data->unk_48 = 0;
    *(int16_t *)data->unk_44 = 0;

    // Initialize `unk_50` field using `csstrncpy` and set the highest bit
    *csstrncpy((char *)data->unk_50, data->name, 2);
    *(uint16_t *)data->unk_50 |= 0x8000;

    // Clear the data array if maximum_count > 0
    if (data->maximum_count > 0) {
        for (int16_t i = 0; i < data->maximum_count; i++) {
            int offset = data->size * i;
            *(int16_t *)((char *)data->data + offset) = 0;
        }
    }
}



/*
Explanation

    Struct Usage:
        Leveraged the data_t struct for field access, replacing magic numbers and offsets with meaningful names (e.g., data->maximum_count, data->magic).

    Error Messages:
        Each validation includes a specific message to identify the exact issue if an assertion fails.

    Assertions:
        data->data: Ensures the data array pointer is initialized.
        data->magic: Validates the expected "magic number."
        data->maximum_count: Ensures the maximum allowable count is non-negative.
        data->current_count: Checks the current count is within valid bounds.
        data->unk_48: Ensures the value of unk_48 is valid and does not exceed the current count.

    Descriptive Functionality:
        Simplifies the validation logic and maps directly to the defined data_t structure.
*/
void data_verify(data_t *data) {
    // Ensure the input pointer is valid
    assert_halt_msg(
        data != NULL,
        "data cannot be NULL"
    );

    // Validate the data structure fields
    assert_halt_msg(
        data->data != NULL,
        "Data array is not allocated"
    );

    assert_halt_msg(
        data->magic == 0x64407440,
        "Invalid magic number in data structure"
    );

    assert_halt_msg(
        data->maximum_count >= 0,
        "Maximum count is negative"
    );

    assert_halt_msg(
        data->current_count >= 0,
        "Current count is negative"
    );

    assert_halt_msg(
        data->maximum_count >= data->current_count,
        "Current count exceeds maximum count"
    );

    assert_halt_msg(
        data->unk_48 >= 0,
        "Field unk_48 is negative"
    );

    assert_halt_msg(
        data->current_count >= data->unk_48,
        "Field unk_48 exceeds current count"
    );
}
