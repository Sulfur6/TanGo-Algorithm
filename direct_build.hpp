//
// Created by ByteDance on 2023/5/20.
//

#ifndef TANGO_ALGORITHM_DIRECT_BUILD_HPP
#define TANGO_ALGORITHM_DIRECT_BUILD_HPP

#include <algorithm>
#include "base_algorithm.hpp"

class DirectBuild : public AlgorithmBase {
public:
    DirectBuild(std::string cloud_info_path, std::string task_set_path, std::string result_path) :
        AlgorithmBase(cloud_info_path, task_set_path, result_path) {
    }

    void build_feasible_task_set(Region &region, std::set<int> &curr_fsets, int &curr_val) {
        for (int i = 0; i < full_task_set.task_count; i++) {
            Task &curr_task = full_task_set.tasks[i];
            if (try_assign(region, full_task_set, curr_task)) {
                curr_fsets.insert(i);
                curr_val += (region_set.max_unit_price - region.unit_cpu_price) * curr_task.comp_power_demand;
                curr_val += (region_set.max_mem_price - region.unit_mem_price) * curr_task.mem_dem;
                curr_val += (region_set.max_disk_price - region.unit_disk_price) * curr_task.disk_dem;
            }
        }
    }

    void unbuild_feasible_task_set(Region &region, std::set<int> &curr_fsets) {
        for (auto task_id : curr_fsets) {
            dis_assign(region, full_task_set, full_task_set.tasks[task_id]);
        }
    }

    void direct_build() {
        std::sort(full_task_set.tasks.begin(), full_task_set.tasks.end(), [](const Task &a, const Task &b){
            if (a.comp_power_demand != b.comp_power_demand) return a.comp_power_demand > b.comp_power_demand;
            if (a.mem_dem != b.mem_dem) return a.mem_dem > b.mem_dem;
            return a.disk_dem > b.disk_dem;
        });
        clock_t start, end;
        start = clock();
        int assigned_task_count = 0;
        for (int _ = 1; _ <= region_set.region_count; _++) {
            std::set<int> best_fsets, curr_fsets;
            int best_val = -1, curr_val = 0, assigned_region_id = -1;
            for (int i = 0; i < region_set.region_count; i++) {
                curr_val = 0;
                curr_fsets.clear();

                build_feasible_task_set(region_set.regions[i], curr_fsets, curr_val);

                if (curr_val > best_val || (curr_val == best_val && curr_fsets.size() > best_fsets.size())) {
                    assigned_region_id = i;
                    best_val = curr_val;

                    best_fsets.clear();
                    for (auto it : curr_fsets) best_fsets.insert(it);
                }

                unbuild_feasible_task_set(region_set.regions[i], curr_fsets);
            }

            // assert(assigned_region_id != -1);
            // printf("%d %d %d\n", _, assigned_region_id, (int)best_fsets.size());
            assigned_task_count += (int)best_fsets.size();
            if (assigned_region_id == -1) continue;


            for (auto it : best_fsets) {
                int assigned_task_id = it;
                bool flag = try_assign(region_set.regions[assigned_region_id], full_task_set, full_task_set.tasks[assigned_task_id]);
                if (!flag) {
                    std::cerr << "Problem occurs in DRT_BD" << '\n';
                }
            }

        }

        scheduling_result();
        end = clock();
        _plan << "run time cost of algorithm DRT_BD is " << double(end - start) / CLOCKS_PER_SEC << "seconds\n";
    }
};

#endif //TANGO_ALGORITHM_DIRECT_BUILD_HPP
