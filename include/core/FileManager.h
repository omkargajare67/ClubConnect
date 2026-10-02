#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <mutex>
#include "json.hpp"

// SINGLETON PATTERN & ENCAPSULATION: Centralized disk persistence manager.
// Ensures only one instance coordinates thread-safe file I/O to the data/ directory.
class FileManager {
private:
    static FileManager* instance;
    static std::mutex instanceMutex;
    std::mutex ioMutex;

    // Private constructor enforces Singleton pattern
    FileManager() = default;

public:
    // Destructor
    ~FileManager() = default;

    // Delete copy and assignment operators
    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;

    // DESIGN PATTERN: Thread-safe Meyer's or synchronized Singleton accessor.
    static FileManager& getInstance();

    // Saves JSON content atomically to disk inside the data directory
    bool save(const std::string& filename, const nlohmann::json& data);

    // Loads JSON content from disk inside the data directory
    nlohmann::json load(const std::string& filename);
};

#endif // FILE_MANAGER_H
