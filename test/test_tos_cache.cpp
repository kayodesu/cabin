#include <iostream>
#include <ctime>
#include <climits>

/*
 * 测试 top of stack cache
 * Author: kayo
 */

using namespace std;

#define N INT_MAX

/*
 * 从测试结果看栈顶缓存好像没什么用啊，怎么回事？？
 */
void test()
{
    register int cache;
    auto buf = new int[10];

    time_t time1;
    time(&time1);

    for (int i = 0; i < N; i++) {
        buf[9] = i;
        buf[9] += 4;
    }

    time_t time2;
    time(&time2);

    for (int i = 0; i < N; i++) {
        cache = i;
        cache += 4;
    }

    time_t time3;
    time(&time3);

    for (int i = 0; i < N; i++) {
        __asm__ __volatile__(
            "movl $3, %eax\n\t"
            "addl $4, %eax\n\t"
        );
    }

    time_t time4;
    time(&time4);

    cout << "not use cache: " << time2 - time1
         << ", use cache: " << time3 - time2
         << ", assembly :" << time4 - time3 << endl;
}

int main()
{
    test();
    return 0;
}
