//
// Created by ByteDance on 2023/5/5.
//

#ifndef TANGO_ALGORITHM_NETWORK_HPP
#define TANGO_ALGORITHM_NETWORK_HPP

#include <vector>

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

    int region_id;

    Region() {}

//    Region(int u, int c, int id) : unit_price(u), comp_power(c), comp_remain(c), region_id(id) {}

    Region(int id, int cpu, int mem, int disk, int cpu_rem, int mem_rem, int disk_rem, int ucp, int ump, int udp)
            : region_id(id), comp_power(cpu),
              mem(mem), disk(disk),
              comp_remain(cpu_rem),
              mem_rem(mem_rem),
              disk_rem(disk_rem), unit_cpu_price(ucp), unit_mem_price(ump), unit_disk_price(udp) {}
};

class RegionSet {
public:
    int region_count;
    int max_unit_price;
    std::vector<Region> regions;
    int delay_inter_region[MAX_REGION_COUNT][MAX_REGION_COUNT];
    int band_inter_region[MAX_REGION_COUNT][MAX_REGION_COUNT];

    RegionSet() : max_unit_price(-1) {}
};

#endif //TANGO_ALGORITHM_NETWORK_HPP
