#include <iostream>
#include <string>
#include <sstream>
#include <sw/redis++/redis++.h>

using namespace sw::redis;

void test_read() {
    auto redis = Redis("tcp://127.0.0.1:6379");
    auto input = redis.hget("scheduling_task_1", "input");
    std::cout << *input << std::endl;
    std::stringstream input_stream(*input);
    int n;
    input_stream >> n;
    std::cout << n;
}

int main() {
    test_read();
    return 0;
}