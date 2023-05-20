//
// Created by ByteDance on 2023/5/8.
//

#ifndef TANGO_ALGORITHM_SM_CTP_HPP
#define TANGO_ALGORITHM_SM_CTP_HPP

#include "base_algorithm.hpp"

class SM_CTP : public AlgorithmBase {
public:
    int feasible_task_set_count;

    SM_CTP(std::string cloud_info_path, std::string task_set_path, std::string result_path) :
        AlgorithmBase(cloud_info_path, task_set_path, result_path) {
    }

    void search_feasible_task_set(int x, Region &region, std::set<int> &best_fsets, std::set<int> &curr_fsets, int &best_val, int &curr_val, int &assigned_region_id) {
        // static int feasible_task_set_count = 0;

        if (x >= full_task_set.task_count) return;

        if (feasible_task_set_count >= full_task_set.task_count) return;

        int task_id = x;
        Task &curr_task = full_task_set.tasks[task_id];
        if (try_assign(region, full_task_set, curr_task)) {

            curr_fsets.insert(task_id);
            curr_val += (region_set.max_unit_price - region.unit_cpu_price) * curr_task.comp_power_demand;
            curr_val += (region_set.max_mem_price - region.unit_mem_price) * curr_task.mem_dem;
            curr_val += (region_set.max_disk_price- region.unit_disk_price) * curr_task.disk_dem;
            feasible_task_set_count++;

            if (curr_val > best_val || (curr_val == best_val && curr_fsets.size() > best_fsets.size())) {
                assigned_region_id = region.region_id;
                best_val = curr_val;

                best_fsets.clear();
                for (auto it : curr_fsets) best_fsets.insert(it);
            }
            search_feasible_task_set(x + 1, region, best_fsets, curr_fsets, best_val, curr_val, assigned_region_id);

            dis_assign(region, full_task_set, curr_task);
            curr_fsets.erase(task_id);
            curr_val -= (region_set.max_unit_price - region.unit_cpu_price) * curr_task.comp_power_demand;
            curr_val -= (region_set.max_mem_price - region.unit_mem_price) * curr_task.mem_dem;
            curr_val -= (region_set.max_disk_price- region.unit_disk_price) * curr_task.disk_dem;
        }

        search_feasible_task_set(x + 1, region, best_fsets, curr_fsets, best_val, curr_val, assigned_region_id);
    }

    void sm_ctp_main_procedure() {
        clock_t start, end;
        start = clock();
        int assigned_task_count = 0;
        for (int _ = 1; _ <= region_set.region_count; _++) {
            std::set<int> best_fsets, curr_fsets;
            int best_val = -1, curr_val = 0, assigned_region_id = -1;
            for (int i = 0; i < region_set.region_count; i++) {
                feasible_task_set_count = 0;
                search_feasible_task_set(0, region_set.regions[i], best_fsets, curr_fsets, best_val, curr_val, assigned_region_id);
            }

//            assert(assigned_region_id != -1);
//            printf("%d %d %d\n", _, assigned_region_id, (int)best_fsets.size());
            assigned_task_count += (int)best_fsets.size();
            if (assigned_region_id == -1) continue;

            for (auto it : best_fsets) {
                int assigned_task_id = it;
                bool flag = try_assign(region_set.regions[assigned_region_id], full_task_set, full_task_set.tasks[assigned_task_id]);
                if (!flag) {
                    std::cerr << "Problem occurs in SM_CTP" << '\n';
                }
            }
        }
        scheduling_result();
        end = clock();
        _plan << "run time cost of algorithm SM_CTP is " << double(end - start) / CLOCKS_PER_SEC << "seconds\n";
    }
};

#endif //TANGO_ALGORITHM_SM_CTP_HPP
