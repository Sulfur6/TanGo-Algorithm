#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "SM_CTP.hpp"
#include "direct_build.hpp"
#include "direct_assign.hpp"

void test_read() {
    std::string add("./dataset/task_set_0");
    std::ifstream in;
    in.open(add, std::ios::in);
    int n;
    in >> n;
    std::cout << n << std::endl;
}

int main() {
    test_read();
    static SM_CTP smCtp("./dataset/cloud_info", "./dataset/task_set_0", "./dataset/scheduling_result_a_0");
    smCtp.sm_ctp_main_procedure();
    static DirectBuild directBuild("./dataset/cloud_info", "./dataset/task_set_0", "./dataset/scheduling_result_b_0");
    directBuild.direct_build();
    static DirectAssign directAssign("./dataset/cloud_info", "./dataset/task_set_0", "./dataset/scheduling_result_c_0");
    directAssign.direct_assign();
    std::string cloud_info("./dataset/cloud_info"), task_set("./dataset/task_set_"), result_a("./dataset/scheduling_result_a_"), result_b("./dataset/scheduling_result_b_"), result_c("./dataset/scheduling_result_c_");
    for (int i = 1; i < 1000; i++) {
        smCtp.rebuild(task_set + std::to_string(i), result_a + std::to_string(i));
        directBuild.rebuild(task_set + std::to_string(i), result_b + std::to_string(i));
        directAssign.rebuild(task_set + std::to_string(i), result_c + std::to_string(i));
        smCtp.sm_ctp_main_procedure();
        directBuild.direct_build();
        directAssign.direct_assign();
        std::cerr << "finished: " << i << "\n";
    }
//    static DirectBuild directBuild("./dataset/cloud_info", "./dataset/task_set_59", "./dataset/scheduling_result_b_59");
//    directBuild.direct_build();
    return 0;
}