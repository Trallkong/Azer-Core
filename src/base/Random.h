#ifndef AZER_DEV_RANDOM_H
#define AZER_DEV_RANDOM_H

#include "Base.h"
#include <random>

namespace azer
{
    /// 简易随机数工具类。
    ///
    /// 所有实例共享同一个 Mersenne Twister 引擎，引擎在首次使用时由
    /// std::random_device 播种一次。每次调用 RandBetween 仅有分布开销，
    /// 不会重复构造随机设备或引擎。
    class Random
    {
    public:
        /// 返回 [min, max] 区间内均匀分布的随机整数。
        static uint32_t RandBetween(const uint32_t min, const uint32_t max)
        {
            std::uniform_int_distribution<int> dist(min, max);
            return dist(m_RandomEngine);
        }
    private:
        inline static std::mt19937 m_RandomEngine{std::random_device{}()};
    };
}

#endif //AZER_DEV_RANDOM_H
