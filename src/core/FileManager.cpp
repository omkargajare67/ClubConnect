#include "core/FileManager.h"
#include <fstream>
#include <iostream>

// SINGLETON PATTERN: Static instance and initialization mutex.
FileManager* FileManager::instance = nullptr;
std::mutex FileManager::instanceMutex;

FileManager& FileManager::getInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex);
    if (instance == nullptr) {
        instance = new FileManager();
    }
    return *instance;
}

// THREAD SAFETY: Guarantees serialized file writes using ioMutex.
bool FileManager::save(const std::string& filename, const nlohmann::json& data) {
    std::lock_guard<std::mutex> lock(ioMutex);
    std::string path = "data/" + filename;
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "[FileManager] Error: Could not open file for writing: " << path << std::endl;
        return false;
    }
    file << data.dump(4);
    return true;
}

// THREAD SAFETY: Guarantees serialized file reads using ioMutex.
nlohmann::json FileManager::load(const std::string& filename) {
    std::lock_guard<std::mutex> lock(ioMutex);
    std::string path = "data/" + filename;
    std::ifstream file(path);
    if (!file.is_open()) {
        // Return empty array if file does not exist yet
        return nlohmann::json::array();
    }
    try {
        nlohmann::json data;
        file >> data;
        return data;
    } catch (const std::exception& e) {
        std::cerr << "[FileManager] Warning: JSON parsing error in " << path << ": " << e.what() << std::endl;
        return nlohmann::json::array();
    }
}
