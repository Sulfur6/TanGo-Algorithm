//
// Created by ByteDance on 2023/5/5.
//

#ifndef TANGO_ALGORITHM_NETWORK_HPP
#define TANGO_ALGORITHM_NETWORK_HPP

#include <vector>
#include <climits>
#include <algorithm>

const int MAX_REGION_COUNT = 1000 + 5;


class Region {
public:
    int comp_power;
    int mem;
    int disk;

    int comp_remain;
    int mem_rem;
    int disk_rem;

    int unit_cpu_price;
    int unit_mem_price;
    int unit_disk_price;

    int tail_delay, avg_delay;

    int region_id;

    Region() {}

//    Region(int u, int c, int id) : unit_price(u), comp_power(c), comp_remain(c), region_id(id) {}

    Region(int id, int cpu, int mem, int disk, int cpu_rem, int mem_rem, int disk_rem, int ucp, int ump, int udp,
           int td, int ad)
            : region_id(id), comp_power(cpu),
              mem(mem), disk(disk),
              comp_remain(cpu_rem),
              mem_rem(mem_rem),
              disk_rem(disk_rem), unit_cpu_price(ucp), unit_mem_price(ump), unit_disk_price(udp), tail_delay(td),
              avg_delay(ad) {}
};

class RegionSet {
public:
    int region_count;
    int max_unit_price, max_mem_price, max_disk_price;
    std::vector<Region> regions;
    int delay_inter_region[MAX_REGION_COUNT][MAX_REGION_COUNT];
    int band_inter_region[MAX_REGION_COUNT][MAX_REGION_COUNT];

    RegionSet() : max_unit_price(-1), max_mem_price(-1), max_disk_price(-1) {}

    void initialize() {
        for (auto region: regions) {
            max_unit_price = std::max(max_unit_price, region.unit_cpu_price);
            max_mem_price = std::max(max_mem_price, region.unit_mem_price);
            max_disk_price = std::max(max_disk_price, region.unit_disk_price);
        }
    }

    void rebuild() {
        for (auto region : regions) {
            region.comp_remain = region.comp_power;
            region.mem_rem = region.mem;
            region.disk_rem = region.disk;
        }
    }
};

#endif //TANGO_ALGORITHM_NETWORK_HPP
