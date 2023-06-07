//
// Created by ByteDance on 2023/5/31.
//

#ifndef TANGO_ALGORITHM_SM_GRRS_HPP
#define TANGO_ALGORITHM_SM_GRRS_HPP

#include "base_algorithm.hpp"

class SM_GRRS : public AlgorithmBase {
public:
    SM_GRRS(std::string redis_url) : AlgorithmBase(redis_url) {}

    void build_feasible_task_set(Region &region, std::set<int> &curr_fsets, int &curr_val) {
        for (int i = 0; i < full_task_set.task_count; i++) {
            Task &curr_task = full_task_set.tasks[i];
            if (try_assign(region, full_task_set, curr_task)) {
                curr_fsets.insert(curr_task.task_id);
                curr_val += (region_set.max_unit_price - region.unit_cpu_price) * curr_task.comp_power_demand;
                curr_val += (region_set.max_mem_price - region.unit_mem_price) * curr_task.mem_dem;
                curr_val += (region_set.max_disk_price - region.unit_disk_price) * curr_task.disk_dem;
            }
        }
    }

    void sm_grrs_main_procedure() {
        std::sort(region_set.regions.begin(), region_set.regions.end(), [](const Region &a, const Region &b) {
            if (a.unit_cpu_price != b.unit_cpu_price) return a.unit_cpu_price < b.unit_cpu_price;
            if (a.unit_mem_price != b.unit_mem_price) return a.unit_mem_price < b.unit_mem_price;
            return a.unit_disk_price < b.unit_disk_price;
        });
        std::sort(full_task_set.tasks.begin(), full_task_set.tasks.end(), [](const Task &a, const Task &b) {
            if (a.comp_power_demand != b.comp_power_demand) return a.comp_power_demand > b.comp_power_demand;
            if (a.mem_dem != b.mem_dem) return a.mem_dem > b.mem_dem;
            return a.disk_dem > b.disk_dem;
        });
        clock_t start, end;
        start = clock();
        int assigned_task_count = 0;
        for (int i = 0; i < region_set.region_count; i++) {
            std::set<int> best_fsets, curr_fsets;
            int best_val = 0, curr_val = 0, assigned_region_id = i;

            build_feasible_task_set(region_set.regions[i], best_fsets, best_val);

            assigned_task_count += (int) best_fsets.size();

            // for (auto it : best_fsets) {
            //     int assigned_tenant_id = it.first, assigned_task_id = it.second;
            //     assert(try_assign(region_set.regions[assigned_region_id], tenants[assigned_tenant_id], tenants[assigned_tenant_id].tasks[assigned_task_id]));
            // }
        }

        end = clock();
        _plan.clear();
        _plan << "run time cost of algorithm DRT_AG is " << double(end - start) / CLOCKS_PER_SEC << "seconds\n";
        scheduling_result();
    }
};

#endif //TANGO_ALGORITHM_SM_GRRS_HPP
