//
// Created by ByteDance on 2023/5/7.
//

#ifndef TANGO_ALGORITHM_BASE_ALGORITHM_HPP
#define TANGO_ALGORITHM_BASE_ALGORITHM_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <sw/redis++/redis++.h>

#include "network.hpp"
#include "task.hpp"

typedef std::pair<int, int> Pair;
using namespace sw::redis;

class AlgorithmBase {
public:
    RegionSet region_set;
    TaskSet full_task_set;
    std::vector<Pair> task_index;

    Redis redis;
    std::string TASK_FIELD, CLOUD_INFO_KEY;
    std::string redis_key, output_field;
    std::stringstream _its, _irs, _plan;

    AlgorithmBase(std::string redis_url, std::string redis_key) : redis(Redis(redis_url)), redis_key(redis_key) {
        TASK_FIELD = std::string("input");
        CLOUD_INFO_KEY = std::string("cloud_info");

        auto _task_input = redis.hget(redis_key, TASK_FIELD);
        auto _network_input = redis.get(CLOUD_INFO_KEY);
        if (_task_input && _network_input) {
            _its = std::stringstream(*_task_input);
            _irs = std::stringstream(*_network_input);
            initialize();
            region_set.initialize();
        } else {
            throw "Error while reading input data from redis";
        }
    }

    void init_task_set() {
        _its >> full_task_set.task_count;
        for (int i = 1; i <= full_task_set.task_count; i++) {
            int id, cpu, mem, disk, delay;
            _its >> id >> cpu >> mem >> disk >> delay;
            full_task_set.tasks.emplace_back(id, cpu, mem, disk, delay);
        }
        int inter_constrain_count;
        _its >> inter_constrain_count;
        for (int i = 1; i <= inter_constrain_count; i++) {
            int u, v, band, delay;
            _its >> u >> v >> band >> delay;
            full_task_set.band_inter_task[u][v] = band;
            full_task_set.band_inter_task[v][u] = band;
            full_task_set.delay_inter_task[u][v] = delay;
            full_task_set.delay_inter_task[v][u] = delay;
        }
    }

    void init_region_set() {
        _irs >> region_set.region_count;
        for (int i = 1; i <= region_set.region_count; i++) {
            int id, cpu, mem, disk, cpu_rem, mem_rem, disk_rem;
            int ucp, ump, udp;
            int td, ad;
            _irs >> id >> cpu >> mem >> disk >> cpu_rem >> mem_rem >> disk_rem;
            _irs >> ucp >> ump >> udp;
            _irs >> td >> ad;
            region_set.regions.emplace_back(id, cpu, mem, disk, cpu_rem, mem_rem, disk_rem, ucp, ump, udp, td, ad);
        }
        int inter_count;
        _irs >> inter_count;
        for (int i = 1; i <= inter_count; i++) {
            int u, v, band, delay, _;
            _irs >> u >> v >> band >> delay;
            region_set.band_inter_region[u][v] = band;
            region_set.band_inter_region[v][u] = band;
            region_set.delay_inter_region[u][v] = delay;
            region_set.delay_inter_region[v][u] = delay;
        }
    }

    void initialize() {
        init_region_set();
        init_task_set();
    }

    int calc_total_cost() {
        int res = 0;
        for (auto task: full_task_set.tasks) {
            Region &region = region_set.regions[task.region_id];
            res += region.unit_cpu_price * task.comp_power_demand;
            res += region.unit_mem_price * task.mem_dem;
            res += region.unit_disk_price * task.disk_dem;
        }
        return res;
    }

    void scheduling_result() {
        int total_cost = calc_total_cost();
        _plan << total_cost << '\n';
        for (auto task: full_task_set.tasks) {
            _plan << task.task_id << ' ' << task.region_id << '\n';
        }
        redis.hset(redis_key, output_field, _plan.str());
    }

    bool try_assign(Region &region, TaskSet &task_set, Task &curr_task) {
        if (curr_task.region_id != -1) return false;
        if (region.tail_delay > curr_task.delay_demand) return false;

        // check remain computing power
        region.comp_remain -= curr_task.comp_power_demand;
        region.mem_rem -= curr_task.mem_dem;
        region.disk_rem -= curr_task.disk_dem;
        if (region.comp_remain < 0) {
            region.comp_remain += curr_task.comp_power_demand;
            region.mem_rem += curr_task.mem_dem;
            region.disk_rem += curr_task.disk_dem;
            return false;
        }

        for (int i = 0; i < task_set.task_count; i++) {
            if (i == curr_task.task_id) continue;
            Task &inter_task = task_set.tasks[i];
            if (inter_task.region_id == -1) continue;

            region_set.band_inter_region[region.region_id][inter_task.region_id] -= task_set.band_inter_task[curr_task.task_id][inter_task.task_id];

            if (region_set.band_inter_region[region.region_id][inter_task.region_id] < 0
                || region_set.delay_inter_region[region.region_id][inter_task.region_id] >
                   task_set.delay_inter_task[curr_task.task_id][inter_task.task_id]
                || (task_set.conflict_inter_task[curr_task.task_id][inter_task.task_id] == 1 &&
                    region.region_id == inter_task.region_id)) {

                for (int j = i; j >= 0; j--) {
                    if (task_set.tasks[j].region_id == -1) continue;
                    if (j == curr_task.task_id) continue;
                    region_set.band_inter_region[region.region_id][task_set.tasks[j].region_id]
                            += task_set.band_inter_task[curr_task.task_id][j];
                }

                region.comp_remain += curr_task.comp_power_demand;
                return false;
            }
        }

        curr_task.region_id = region.region_id;
        return true;
    }

    void dis_assign(Region &region, TaskSet &task_set, Task &task) {
        region.comp_remain += task.comp_power_demand;
        region.mem_rem += task.mem_dem;
        region.disk_rem += task.disk_dem;
        for (int i = 0; i < task_set.task_count; i++) {
            if (i == task.task_id || task_set.tasks[i].region_id == -1) continue;
            region_set.band_inter_region[task.region_id][task_set.tasks[i].region_id] += task_set.band_inter_task[task.task_id][i];
        }
        task.region_id = -1;
    }
};


#endif //TANGO_ALGORITHM_BASE_ALGORITHM_HPP
