// appraiser.cpp

#include <unistd.h>  // usleep()

#include <cstdio>
#include <cstdlib>  // rand()
#include <ctime>    // time()
#include <iostream>

int main() {
  // gnuplot 파이프 열기
  FILE* gnuplotPipe = popen("gnuplot -persistent", "w");

  if (!gnuplotPipe) {
    std::cerr << "Gnuplot을 열 수 없습니다." << std::endl;
    return 1;
  }

  // 랜덤 시드 설정
  srand(time(nullptr));

  // gnuplot 설정
  fprintf(gnuplotPipe, "set title 'Real-time Random Data Plot'\n");
  fprintf(gnuplotPipe, "set xrange [0:100]\n");  // x축 범위 설정
  fprintf(gnuplotPipe, "set yrange [0:100]\n");  // y축 범위 설정
  fprintf(gnuplotPipe, "set grid\n");            // 그래프 그리드 추가

  int x = 0;
  while (true) {
    // 그래프를 매번 새로 그리기 위해 데이터를 다시 입력
    fprintf(gnuplotPipe, "plot '-' using 1:2 with lines title 'Random Data'\n");

    // 0부터 100 사이의 랜덤 데이터를 생성하여 gnuplot에 전달
    for (int i = 0; i <= 100; ++i) {
      int y = rand() % 101;  // 0부터 100 사이의 랜덤 값 생성
      fprintf(gnuplotPipe, "%d %d\n", i, y);
    }

    fprintf(gnuplotPipe, "e\n");  // 데이터 입력 종료를 나타내는 'e' 명령어
    fflush(gnuplotPipe);  // 데이터를 바로 플러시하여 업데이트

    usleep(100000);  // 0.1초 대기 (100,000 마이크로초)
  }

  pclose(gnuplotPipe);  // gnuplot 파이프 닫기
  return 0;
}
