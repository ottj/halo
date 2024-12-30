#include "../../common.h"

void display_assert(const char *reason, const char *filepath, int lineno,
                    bool halt)
{
  if (halt) {
    stack_walk(0);
  }
  error(2, "EXCEPTION %s in %s,#%d: %s", halt ? "halt" : "warn", filepath,
        lineno, reason ? reason : "<no reason given>");
}

#ifdef strlen
#undef strlen
#endif

int csstrlen(const char *s1)
{
  int size;

  assert_halt(s1);
  size = strlen(s1);
  assert_halt(size >= 0 && size < MAXIMUM_STRING_SIZE);

  return size;
}

int FUN_0008dcb0(const byte *str1, const byte *str2) { // compare_strings(const byte *str1, const byte *str2) {
    // Validate input strings
    assert_halt_msg(
        str1 != NULL && str2 != NULL,
        "Input strings cannot be NULL"
    );

    // Compare the strings byte by byte
    while (true) {
        // Compare current bytes
        byte byte1 = *str1;
        byte byte2 = *str2;

        if (byte1 != byte2) {
            return (1 - (uint)(byte1 < byte2)) - (uint)(byte1 > byte2);
        }

        // Check for null terminator
        if (byte1 == 0) {
            return 0; // Strings are equal
        }

        // Move to the next pair of bytes
        str1++;
        str2++;
    }
}
