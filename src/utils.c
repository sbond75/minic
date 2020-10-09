#include "utils.h"

#include <stdlib.h>

// Based on code from https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c
bool withContents(const char* filename, void (*processContents)(char*)) {
  char * buffer = 0;
  long length;
  FILE * f = fopen (filename, "r");

  if (f)
    {
      fseek (f, 0, SEEK_END);
      length = ftell (f);
      fseek (f, 0, SEEK_SET);
      buffer = malloc (length);
      if (buffer)
	{
	  fread (buffer, 1, length, f);
	}
      fclose (f);
    }

  if (buffer)
    {
      // start to process your data / extract strings here...
      processContents(buffer);
      free(buffer);
      return true;
    }
  else {
    return false;
  }
}

bool withFile(const char* filename, void (*processFile)(FILE*)) {
  char * buffer = 0;
  long length;
  FILE * f = fopen (filename, "r");

  if (f)
    {
      processFile(f);
      fclose (f);
      return true;
    }
  else {
    return false;
  }
}
