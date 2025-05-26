#include <stdio.h>
int main() {
  int age = 25;
  printf("%d", age);
  if (age) {
    printf("%s", "Adult");
  } else {
    printf("%s", "Minor");
  }
  for (int i = 1; i <= 3; ++i) {
    printf("%d", i);
  }
  for (int __i = 0; __i < 2; ++__i) {
    printf("%s", "Hi");
  }
  return 0;
}
