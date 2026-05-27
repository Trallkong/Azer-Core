//
// Created by Trallkong on 2026/4/18.
//

#ifndef AZER_DELTATIME_H
#define AZER_DELTATIME_H

#include <chrono>

namespace azer {

class DeltaTime {
public:
    DeltaTime();
    ~DeltaTime();

    float GetDeltaTime();
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
};

} // azer

#endif //AZER_DELTATIME_H
