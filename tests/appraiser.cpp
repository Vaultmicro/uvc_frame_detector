// appraiser.cpp

#include <unistd.h>  // usleep()

#include <cstdio>
#include <cstdlib>  // rand()
#include <ctime>    // time()
#include <iostream>

int main() {
  FILE* gnuplotPipe = popen("gnuplot -persistent", "w");

  if (!gnuplotPipe) {
    std::cerr << "Gnuplot을 열 수 없습니다." << std::endl;
    return 1;
  }

  srand(time(nullptr));

  fprintf(gnuplotPipe, "set title 'Real-time Random Data Plot'\n");
  fprintf(gnuplotPipe, "set xrange [0:100]\n");  
  fprintf(gnuplotPipe, "set yrange [0:100]\n");  
  fprintf(gnuplotPipe, "set grid\n");           

  int x = 0;
  while (true) {
    fprintf(gnuplotPipe, "plot '-' using 1:2 with lines title 'Random Data'\n");

    for (int i = 0; i <= 100; ++i) {
      int y = rand() % 101;  
      fprintf(gnuplotPipe, "%d %d\n", i, y);
    }

    fprintf(gnuplotPipe, "e\n");  
    fflush(gnuplotPipe); 

    usleep(100000); 
  }

  pclose(gnuplotPipe);  
  return 0;
}
