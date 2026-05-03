//
// Created by Trallkong on 2026/4/18.
//

#include "azpch.h"
#include "DeltaTime.h"

namespace azer
{
    DeltaTime::DeltaTime()
    {
        startTime = std::chrono::high_resolution_clock::now();
        endTime = startTime;
    }

    DeltaTime::~DeltaTime()
    {
    }

    float DeltaTime::GetDeltaTime()
    {
        endTime = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(endTime - startTime).count();
        startTime = endTime;
        return deltaTime;
    }
} // azer