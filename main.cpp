#include <iostream>
#include <string>
#include <sstream>
#include <sw/redis++/redis++.h>

#include "httplib.h"
#include "SM_GRRS.hpp"

using namespace sw::redis;
using namespace httplib;

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
    Server svr;
    std::string REDIS_URL = "tcp://127.0.0.1:6379";
    std::string REDIS_KEY = "scheduling_task_";
    std::string CALLBACK_ADDR = "http://localhost:8000/";
    std::string CALLBACK_URL = "/api/scheduling/callback";

    svr.Get(R"(/scheduling/(\d+))", [&](const Request& req, Response& res) {
        auto task_set_id = req.matches[1];
        std::string redis_key = REDIS_KEY + std::string(task_set_id);
        std::string call_back_url = CALLBACK_URL + std::string(task_set_id);

        static SM_GRRS sm_grrs(REDIS_URL, redis_key);
        sm_grrs.sm_grrs_main_procedure();

        Client cli(CALLBACK_ADDR);
        auto _res = cli.Get(call_back_url);
    });

    svr.listen("localhost", 2333);
    return 0;
}