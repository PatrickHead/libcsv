#include <stdio.h>

#include "csv.h"

int main(int argc, char **argv)
{
  csv *data = NULL;
  FILE *f = NULL;

  f = fopen("extras/systems.csv", "r");
  if (!f)
    return 1;

  data = csv_read(f);
  if (!data)
  {
    fprintf(stderr, "ERROR: csv_read() failed\n");
    return 1;
  }

  csv_write(stdout, data);

  csv_free(data);

  fclose(f);

  return 0;
}

