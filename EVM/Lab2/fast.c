#include <stdio.h>

int main() {
  double a = 1e16;
  double b = -1e16;
  double c = 1.0;

  double result = (a + b) + c;
  double result2 = a + (b + c);
  
  printf("result: %.20f\n", result);
  printf("result2: %.20f\n", result2);
  return 0;
}