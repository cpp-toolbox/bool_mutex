#ifndef BOOL_MUTEX_HPP
#define BOOL_MUTEX_HPP

#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <memory>
#include <iostream>

class BoolMutex {
  public:
    class Entry {
      public:
        Entry(BoolMutex &parent, int id) : parent(parent), id(id) {}

        void set(bool value) {
            if (value) {
                for (const auto &excluded : parent.exclusions[id]) {
                    parent.properties[excluded] = false;
                }
            }
            parent.properties[id] = value;
        }

        bool get() const {
            auto it = parent.properties.find(id);
            if (it == parent.properties.end()) {
                throw std::runtime_error("Entry does not exist");
            }
            return it->second;
        }

      private:
        BoolMutex &parent;
        int id;

        friend class BoolMutex;
    };

    std::shared_ptr<Entry> create(bool initial_value = false) {
        int new_id = next_id++;
        properties[new_id] = initial_value;
        return std::make_shared<Entry>(*this, new_id);
    }

    void add_exclusion(const std::shared_ptr<Entry> &a, const std::shared_ptr<Entry> &b) {
        exclusions[a->id].insert(b->id);
        exclusions[b->id].insert(a->id);
    }

    void print_properties() const {
        for (const auto &[key, value] : properties) {
            std::cout << "ID " << key << ": " << value << "\n";
        }
    }

  private:
    int next_id = 0;
    std::unordered_map<int, bool> properties;
    std::unordered_map<int, std::unordered_set<int>> exclusions;
};

#endif // BOOL_MUTEX_HPP
