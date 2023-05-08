//
// Created by ByteDance on 2023/5/8.
//

#ifndef TANGO_ALGORITHM_SM_CTP_HPP
#define TANGO_ALGORITHM_SM_CTP_HPP

#include "base_algorithm.hpp"

class SM_CTP : public AlgorithmBase {
public:
    SM_CTP(std::string redis_url, std::string redis_key) : AlgorithmBase(redis_url, redis_key) {}
};

#endif //TANGO_ALGORITHM_SM_CTP_HPP
