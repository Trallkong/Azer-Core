//
// Created by Trallkong on 2026/5/1.
//

#pragma once

#include "Application.h"
#include "Logger.h"

extern Azer::Application* Azer::CreateApplication();

int main(int argc, char* argv[])
{
    Azer::Logger::Init();

    Azer::Application* app = Azer::CreateApplication();
    app->Run();
    delete app;

    return 0;
}

