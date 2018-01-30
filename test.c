#include <stdio.h>

int a;
int fun1(){
  a = 17;
  return 3;
}

void main(){
  a = a + fun1();
  printf("%d \n",a );

}
