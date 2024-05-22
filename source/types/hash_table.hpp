#pragma once
#include "string.hpp"
#include "value.hpp"

namespace Lux {

    class HashTable
    {
    public:
        struct Entry
        {
            String key{};
            Value value = Value::makeNil();
        };

        HashTable();
        ~HashTable();

        void clear();
        void insert(String key, Value value);
        bool remove(const String& key);
        bool contains(const String& key);
        Entry& find(const String& key);

        HashTable(const HashTable&) = delete;
        HashTable& operator=(const HashTable&) = delete;
    private:
        static constexpr float MAX_LOAD_FACTOR = 0.75;

        void adjustCapacity();

        size_t m_capacity;
        size_t m_size;
        Entry* m_entries;
    };

} // namespace Lux
