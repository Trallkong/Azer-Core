import sys
from pathlib import Path

def create_project(project_name: str, base_path: Path = None):
    """
    创建 Azer 引擎项目
    
    Args:
        project_name: 项目名称
        base_path: 基础路径，默认为当前目录
    """
    if base_path is None:
        base_path = Path.cwd()
    
    # 项目路径
    project_dir = base_path / project_name
    src_dir = project_dir / "src"
    
    # 检查项目是否已存在
    if project_dir.exists():
        response = input(f"项目 '{project_name}' 已存在，是否覆盖？(y/N): ")
        if response.lower() != 'y':
            print("操作已取消")
            return False
    
    # 创建目录
    src_dir.mkdir(parents=True, exist_ok=True)
    print(f"✅ 创建目录: {project_dir}")
    
    # main.cpp 内容
    main_content = f'''#include "EntryPoint.h"
#include "Azer.h"

using namespace Azer;

class {project_name} : public Application
{{
public:
    {project_name}()
    {{
        // Initialize code
    }}

    ~{project_name}()
    {{
        // Cleanup code
    }}
}};

Azer::Application* Azer::CreateApplication()
{{
    return new {project_name}();
}}
'''
    
    # 项目 CMakeLists.txt
    project_cmake_content = f'''cmake_minimum_required(VERSION 3.14)

project({project_name})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable({project_name} src/main.cpp)

target_link_libraries({project_name} Azer)

target_include_directories({project_name} PRIVATE
    ${{CMAKE_CURRENT_SOURCE_DIR}}/../Azer-Core/src
    ${{CMAKE_CURRENT_SOURCE_DIR}}/../Azer-Core/src/base
    ${{CMAKE_CURRENT_SOURCE_DIR}}/../Azer-Core/src/renderer
    ${{CMAKE_CURRENT_SOURCE_DIR}}/../Azer-Core/src/event
    ${{CMAKE_CURRENT_SOURCE_DIR}}/../Azer-Core/src/resources
    ${{CMAKE_CURRENT_SOURCE_DIR}}/../Azer-Core/src/ecs
    ${{CMAKE_CURRENT_SOURCE_DIR}}/../Azer-Core/vendor/SDL/include
    ${{CMAKE_CURRENT_SOURCE_DIR}}/../Azer-Core/vendor/glm
    ${{CMAKE_CURRENT_SOURCE_DIR}}/../Azer-Core/vendor/imgui
    ${{CMAKE_CURRENT_SOURCE_DIR}}/../Azer-Core/vendor/spdlog/include
    ${{CMAKE_CURRENT_SOURCE_DIR}}/../Azer-Core/vendor/entt/single_include
)

target_compile_definitions({project_name} PRIVATE GLM_ENABLE_EXPERIMENTAL)
'''
    
    # 写入文件
    (project_dir / "src" / "main.cpp").write_text(main_content.strip())
    (project_dir / "CMakeLists.txt").write_text(project_cmake_content.strip())
    print(f"✅ 创建项目文件: {project_name}/src/main.cpp")
    print(f"✅ 创建项目文件: {project_name}/CMakeLists.txt")
    
    return True


def update_workspace(project_name: str, base_path: Path = None):
    """更新工作区 CMakeLists.txt"""
    if base_path is None:
        base_path = Path.cwd()
    
    workspace_cmake = base_path / "CMakeLists.txt"
    
    # 检查工作区文件是否存在
    if workspace_cmake.exists():
        # 读取现有内容
        content = workspace_cmake.read_text()
        
        # 检查是否已经添加了该项目
        if f"add_subdirectory({project_name})" in content:
            print(f"⚠️  项目 '{project_name}' 已在工作区中")
            return True
        
        # 在 add_subdirectory(Azer-Core) 后面添加新项目
        import re
        pattern = r'(add_subdirectory\(Azer-Core\))'
        replacement = r'\1\nadd_subdirectory(' + project_name + ')'
        new_content = re.sub(pattern, replacement, content)
        
        if new_content != content:
            workspace_cmake.write_text(new_content)
            print(f"✅ 更新工作区: 添加 add_subdirectory({project_name})")
        else:
            print("⚠️  无法自动更新工作区，请手动添加:")
            print(f"    add_subdirectory({project_name})")
    else:
        # 创建新的工作区文件
        workspace_content = f'''cmake_minimum_required(VERSION 3.14)

project(azer_workspace)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

add_subdirectory(Azer-Core)
add_subdirectory({project_name})
'''
        workspace_cmake.write_text(workspace_content.strip())
        print(f"✅ 创建工作区: {workspace_cmake}")
    
    return True


def main():
    if len(sys.argv) < 2:
        print("❌ 用法: python create_project.py <项目名称>")
        print("示例: python create_project.py MyGame")
        sys.exit(1)
    
    project_name = sys.argv[1]
    
    # 验证项目名称
    if not project_name.isidentifier():
        print(f"❌ 错误: '{project_name}' 不是有效的项目名称（必须是有效的 C++ 标识符）")
        sys.exit(1)
    
    # 获取当前工作目录
    base_path = Path.cwd()
    print(f"📁 工作目录: {base_path}")
    
    # 检查 Azer-Core 是否存在
    if not (base_path / "Azer-Core").exists():
        print("⚠️  警告: 未找到 Azer-Core 目录")
        print("   请确保在 Azer 引擎根目录下运行此脚本")
        response = input("是否继续？(y/N): ")
        if response.lower() != 'y':
            print("操作已取消")
            sys.exit(0)
    
    # 创建项目
    if create_project(project_name, base_path):
        # 更新工作区
        update_workspace(project_name, base_path)
        print(f"\n✅ 项目 '{project_name}' 创建完成！")
        print(f"   项目路径: {base_path / project_name}")
        print(f"   构建命令: cd {base_path} && cmake -B build && cmake --build build")
    else:
        sys.exit(1)


if __name__ == "__main__":
    main()