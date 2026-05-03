//
// Created by Trallkong on 2026/5/1.
//

#ifndef AZER_ENTRYPOINT_H
#define AZER_ENTRYPOINT_H

#include "Application.h"
#include "Logger.h"

extern  azer::Application* azer::CreateApplication();

int main(int argc, char* argv[])
{
    azer::Logger::Init();

    azer::Application* app = azer::CreateApplication();
    app->Run();
    delete app;

    return 0;
}

#endif //AZER_ENTRYPOINT_H
