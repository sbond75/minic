#include <stdbool.h>
#include <stdio.h>

// Reads the file `filename` into a buffer, then calls `processContents`. If not successful, returns false.
extern bool withContents(const char* filename, void (*processContents)(char*));

extern bool withFile(const char* filename, void (*processFile)(FILE*));
