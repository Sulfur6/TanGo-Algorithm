#include <iostream>
#include <string>
#include <sstream>
#include <sw/redis++/redis++.h>

#include "SM_CTP.hpp"

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
    std::string redis_url("tcp://127.0.0.1:6379"), redis_key("scheduling_task_1");
    static SM_CTP sm_ctp(redis_url, redis_key);
    sm_ctp.sm_ctp_main_procedure();
    return 0;
}