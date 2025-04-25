
#include "./chron.h"

#include <stdio.h>
#include <sys/time.h>

struct timeval start_timer(){
  struct timeval start;
  gettimeofday(&start, NULL);
  return start;
}

void show_time(char *desc, struct timeval start){
  if (desc)
    printf("%s: ", desc);
  else
    printf("took: ");

  // Show difference time
  struct timeval stop;
  gettimeofday(&stop, NULL);
  long milliseconds = (stop.tv_sec - start.tv_sec) * 1000 + (stop.tv_usec - start.tv_usec) / 1000;

  printf("%lu ms\n", milliseconds);
}



