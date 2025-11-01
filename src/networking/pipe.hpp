#pragma once

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#define PIPE_STR_SIZE 24

/**
 * @warning the returned char* buffer must be free() d
 */
char* read_pipe_output(const char *command)
{
  FILE *pipe = popen(command, "r");
  if (!pipe)
  {
    perror("popen failed");
    return NULL;
  }

  char buffer[PIPE_STR_SIZE]; // Temporary buffer for reading chunks
  char *output = NULL;
  size_t total_size = 0;

  while (fgets(buffer, sizeof(buffer), pipe) != NULL)
  {
    size_t chunk_len = strlen(buffer);
    output = (char *)realloc(output, total_size + chunk_len + 1); // +1 for null terminator
    if (!output)
    {
      perror("ERROR: read_pipe_output() realloc failed");
      pclose(pipe);
      return NULL;
    }
    memcpy(output + total_size, buffer, chunk_len);
    total_size += chunk_len;
  }

  if (output)
  {
    output[total_size] = '\0'; // Null-terminate the string
  }
  else
  {
    printf("ERROR: read_pipe_output() result string is null");
    return NULL;
  }

  pclose(pipe);
  return output;
}