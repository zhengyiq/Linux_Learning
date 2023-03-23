#include <iostream>
#include <cstdio> // 使用c++风格的c语言的头文件 
using namespace std;

int main()
{
  // Linux下一切皆文件，所以，向显示器打印本质上就是向文件中写入，怎样理解？
  // c

  printf("hello printf->stdout\n");
  fprintf(stdout, "helo fprintf->stdout\n");
  fprintf(stderr, "helo fprintf->stderr\n");

  // c++
  // cout << "hello cout -> cout" << endl;
  // cerr << "hello cerr -> cerr" << endl;
}
