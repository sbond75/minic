#include <stdbool.h>


// Reads the file `filename` into a buffer, then calls `processContents`. If not successful, returns false.
extern bool withContents(const char* filename, void (*processContents)(char*));
