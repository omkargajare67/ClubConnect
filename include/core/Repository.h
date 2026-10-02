#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <vector>
#include <memory>
#include <mutex>
#include <algorithm>
#include <functional>
#include <string>

// TEMPLATES & GENERIC PROGRAMMING: Header-only in-memory repository pattern.
// Provides type-safe CRUD operations over std::shared_ptr<T> collections.
template <typename T>
class Repository {
private:
    std::vector<std::shared_ptr<T>> items;
    mutable std::mutex mtx;

public:
    Repository() = default;

    // Adds a managed entity to the repository
    void add(std::shared_ptr<T> item) {
        std::lock_guard<std::mutex> lock(mtx);
        items.push_back(item);
    }

    // Returns a copy of all shared pointers
    std::vector<std::shared_ptr<T>> getAll() const {
        std::lock_guard<std::mutex> lock(mtx);
        return items;
    }

    // Generic search by arbitrary predicate lambda
    template <typename Predicate>
    std::shared_ptr<T> find(Predicate pred) const {
        std::lock_guard<std::mutex> lock(mtx);
        for (const auto& item : items) {
            if (pred(item)) {
                return item;
            }
        }
        return nullptr;
    }

    // Generic filter by arbitrary predicate lambda
    template <typename Predicate>
    std::vector<std::shared_ptr<T>> filter(Predicate pred) const {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<std::shared_ptr<T>> result;
        for (const auto& item : items) {
            if (pred(item)) {
                result.push_back(item);
            }
        }
        return result;
    }

    // Clears all stored entities
    void clear() {
        std::lock_guard<std::mutex> lock(mtx);
        items.clear();
    }

    // Removes an entity using an arbitrary predicate
    template <typename Predicate>
    bool remove(Predicate pred) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = std::remove_if(items.begin(), items.end(), pred);
        if (it != items.end()) {
            items.erase(it, items.end());
            return true;
        }
        return false;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx);
        return items.size();
    }
};

#endif // REPOSITORY_H
