//
// Created by Trallkong on 2026/5/31.
//

#include "azpch.h"
#include "FileSystem.h"

namespace azer
{
    Scope<FileSystem> FileSystem::s_Instance = nullptr;

    void FileSystem::Init(const std::string& rootPath)
    {
        s_Instance = CreateScope<FileSystem>();
        s_Instance->m_RootPath = rootPath;
    }

    void FileSystem::SetRootPath(const std::string& rootPath)
    {
        s_Instance->m_RootPath = rootPath;
    }

    const std::string& FileSystem::GetRootPath()
    {
        return s_Instance->m_RootPath;
    }

    std::string FileSystem::ResolvePath(const std::string& relativePath)
    {
        return (std::filesystem::path(s_Instance->m_RootPath) / relativePath).string();
    }

    std::string FileSystem::GetWorkingDirectory()
    {
        return std::filesystem::current_path().string();
    }

    // ==================== 文件操作 ====================

    bool FileSystem::Exists(const std::string& path)
    {
        std::error_code ec;
        return std::filesystem::exists(path, ec);
    }

    bool FileSystem::IsDirectory(const std::string& path)
    {
        std::error_code ec;
        return std::filesystem::is_directory(path, ec);
    }

    bool FileSystem::IsFile(const std::string& path)
    {
        std::error_code ec;
        return std::filesystem::is_regular_file(path, ec);
    }

    std::string FileSystem::ReadText(const std::string& path)
    {
        std::ifstream file(path, std::ios::in);
        if (!file.is_open())
        {
            AZ_CORE_ERROR("FileSystem: Failed to read file: {0}", path);
            return "";
        }

        std::string content;
        file.seekg(0, std::ios::end);
        content.resize(static_cast<size_t>(file.tellg()));
        file.seekg(0, std::ios::beg);
        file.read(content.data(), static_cast<std::streamsize>(content.size()));
        return content;
    }

    bool FileSystem::WriteText(const std::string& path, const std::string& text)
    {
        std::ofstream file(path, std::ios::out | std::ios::trunc);
        if (!file.is_open())
        {
            AZ_CORE_ERROR("FileSystem: Failed to write file: {0}", path);
            return false;
        }
        file << text;
        return true;
    }

    std::vector<uint8_t> FileSystem::ReadBytes(const std::string& path)
    {
        std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            AZ_CORE_ERROR("FileSystem: Failed to read binary file: {0}", path);
            return {};
        }

        auto size = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(size);
        file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(size));
        return buffer;
    }

    // ==================== 目录遍历 ====================

    static FileEntry MakeEntry(const std::filesystem::path& entryPath, const std::string& rootPath)
    {
        FileEntry fe;
        fe.Name = entryPath.filename().string();
        fe.FullPath = std::filesystem::absolute(entryPath).string();
        fe.IsDirectory = std::filesystem::is_directory(entryPath);

        std::error_code ec;
        fe.Path = std::filesystem::relative(entryPath, rootPath, ec).string();
        if (ec) fe.Path = fe.FullPath;

        fe.Size = fe.IsDirectory ? 0 : std::filesystem::file_size(entryPath);
        return fe;
    }

    std::vector<FileEntry> FileSystem::ListDirectory(const std::string& path)
    {
        std::vector<FileEntry> result;

        std::error_code ec;
        if (!std::filesystem::is_directory(path, ec))
        {
            AZ_CORE_WARN("FileSystem: Not a directory: {0}", path);
            return result;
        }

        const std::string& root = s_Instance->m_RootPath;

        for (const auto& entry : std::filesystem::directory_iterator(path, ec))
        {
            result.push_back(MakeEntry(entry.path(), root));
        }

        // 排序：目录在前，文件在后，各自按名称排序
        std::sort(result.begin(), result.end(), [](const FileEntry& a, const FileEntry& b)
        {
            if (a.IsDirectory != b.IsDirectory)
                return a.IsDirectory > b.IsDirectory;
            std::string aLower = a.Name, bLower = b.Name;
            std::transform(aLower.begin(), aLower.end(), aLower.begin(), ::tolower);
            std::transform(bLower.begin(), bLower.end(), bLower.begin(), ::tolower);
            return aLower < bLower;
        });

        return result;
    }

    std::vector<FileEntry> FileSystem::ListDirectoryRecursive(const std::string& path)
    {
        std::vector<FileEntry> result;

        std::error_code ec;
        if (!std::filesystem::is_directory(path, ec))
            return result;

        const std::string& root = s_Instance->m_RootPath;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(path, ec))
        {
            result.push_back(MakeEntry(entry.path(), root));
        }

        return result;
    }

    // ==================== 路径工具 ====================

    std::string FileSystem::GetExtension(const std::string& path)
    {
        return std::filesystem::path(path).extension().string();
    }

    std::string FileSystem::GetFilename(const std::string& path)
    {
        return std::filesystem::path(path).filename().string();
    }

    std::string FileSystem::GetFilenameNoExt(const std::string& path)
    {
        return std::filesystem::path(path).stem().string();
    }

    std::string FileSystem::GetDirectory(const std::string& path)
    {
        return std::filesystem::path(path).parent_path().string();
    }

    std::string FileSystem::Join(const std::string& a, const std::string& b)
    {
        return (std::filesystem::path(a) / b).string();
    }
}
