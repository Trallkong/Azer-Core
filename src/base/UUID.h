//
// Created by Trallkong on 2026/8/1.
//

#pragma once

#include <random>
#include <sstream>
#include <iomanip>

namespace Azer {

    inline std::string generate_uuid() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);

        std::stringstream ss;
        ss << std::hex << std::setfill('0');

        // 生成 16 字节
        for (int i = 0; i < 16; i++) {
            if (i == 4 || i == 6 || i == 8 || i == 10) ss << '-';
            ss << std::setw(2) << dis(gen);
        }

        // 设置版本号 (版本4) 和变体
        std::string uuid = ss.str();
        uuid[14] = '4';  // 版本号
        uuid[19] = '8';  // 变体 (8,9,A,B)

        return uuid;
    }
}


