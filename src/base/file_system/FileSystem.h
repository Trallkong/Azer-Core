//
// Created by Trallkong on 2026/5/31.
//

#pragma once

#include "Base.h"
#include <string>
#include <filesystem>
#include <vector>

namespace Azer
{
    // 文件/目录条目（用于 Asset Browser 展示）
    struct FileEntry
    {
        std::string Name;       // 文件名（含扩展名）
        std::string Path;       // 相对于 root 的路径
        std::string FullPath;   // 绝对路径
        bool IsDirectory = false;
        uintmax_t Size = 0;     // 文件大小（字节），目录为 0
    };

    class FileSystem
    {
    public:
        // 不要直接构造，用 Init() 初始化
        FileSystem() = default;
        // 初始化（Application 构造时调用一次）
        static void Init(const std::string& rootPath);

        // 根路径
        static void SetRootPath(const std::string& rootPath);
        static const std::string& GetRootPath();

        // 相对路径 → 绝对路径（基于 root）
        static std::string ResolvePath(const std::string& relativePath);

        // 工作目录
        static std::string GetWorkingDirectory();

        // 文件操作
        static bool Exists(const std::string& path);
        static bool IsDirectory(const std::string& path);
        static bool IsFile(const std::string& path);

        static std::string ReadText(const std::string& path);
        static bool WriteText(const std::string& path, const std::string& text);
        static std::vector<uint8_t> ReadBytes(const std::string& path);

        // 目录遍历
        static std::vector<FileEntry> ListDirectory(const std::string& path);
        static std::vector<FileEntry> ListDirectoryRecursive(const std::string& path);

        // 路径工具
        static std::string GetExtension(const std::string& path);
        static std::string GetFilename(const std::string& path);
        static std::string GetFilenameNoExt(const std::string& path);
        static std::string GetDirectory(const std::string& path);
        static std::string Join(const std::string& a, const std::string& b);

    private:
        static Scope<FileSystem> s_Instance;

        std::string m_RootPath;
    };
}
