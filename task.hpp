//
// Created by ByteDance on 2023/5/6.
//

#ifndef TANGO_ALGORITHM_TASK_HPP
#define TANGO_ALGORITHM_TASK_HPP

#include <vector>
#include <climits>

const int MAX_TASK_COUNT = 1000 + 5;

class Task {
public:
    int delay_demand;
    int comp_power_demand, mem_dem, disk_dem;
    int task_id;
    int region_id;

    Task() {}

    Task(int id, int cpu, int mem, int disk, int delay) : task_id(id), comp_power_demand(cpu), mem_dem(mem),
                                                          disk_dem(disk), delay_demand(delay), region_id(-1) {}
};

class TaskSet {
public:
    int task_count;
    std::vector<Task> tasks;

    int delay_inter_task[MAX_TASK_COUNT][MAX_TASK_COUNT];
    int band_inter_task[MAX_TASK_COUNT][MAX_TASK_COUNT];
    int conflict_inter_task[MAX_TASK_COUNT][MAX_TASK_COUNT];

    TaskSet() {
        for (int i = 0; i < MAX_TASK_COUNT; i++) {
            for (int j = 0; j < MAX_TASK_COUNT; j++) {
                delay_inter_task[i][j] = INT_MAX;
                band_inter_task[i][j] = 0;
                conflict_inter_task[i][j] = 0;
            }
        }
    }

    void clear() {
        tasks.clear();
        for (int i = 0; i < MAX_TASK_COUNT; i++) {
            for (int j = 0; j < MAX_TASK_COUNT; j++) {
                delay_inter_task[i][j] = INT_MAX;
                band_inter_task[i][j] = 0;
                conflict_inter_task[i][j] = 0;
            }
        }
    }
};

#endif //TANGO_ALGORITHM_TASK_HPP