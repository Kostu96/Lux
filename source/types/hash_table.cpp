#include "hash_table.hpp"

#include <utility>

namespace Lux {

    HashTable::HashTable() :
        m_capacity{ 8 },
        m_size{ 0 },
        m_entries{ new Entry[m_capacity] }
    {}

    HashTable::~HashTable()
    {
        delete[] m_entries;
    }

    void HashTable::clear()
    {
        m_capacity = 8;
        m_size = 0;
        delete[] m_entries;
        m_entries = new Entry[m_capacity];
    }

    void HashTable::insert(String key, Value value)
    {
        adjustCapacity();
        Entry& entry = find(key);
        if (entry.key.isNull() && entry.value.isNil()) m_size++;

        entry.key = std::move(key);
        entry.value = value;
    }

    bool HashTable::remove(const String& key)
    {
        if (m_size == 0) return false;
        
        Entry& entry = find(key);
        if (entry.key.isNull()) return false;

        // Place a tombstone in the entry.
        entry.key = String{};
        entry.value = Value::makeBool(true);
        return true;
    }

    bool HashTable::contains(const String& key)
    {
        return !find(key).key.isNull();
    }

    HashTable::Entry& HashTable::find(const String& key)
    {
        size_t index = key.hash() % m_capacity;
        Entry* tombstone = nullptr;
        while (true)
        {
            Entry& entry = m_entries[index];

            if (entry.key.isNull())
            {
                if (entry.value.isNil())
                    return tombstone != nullptr ? *tombstone : entry; // Empty entry.
                else if (tombstone == nullptr)
                    tombstone = &entry; // We found a tombstone.
            }
            else if (entry.key == key)
                return entry; // We found the key.

            index = (index + 1) % m_capacity;
        }
    }

    void HashTable::adjustCapacity()
    {
        if (m_size + 1 < m_capacity * MAX_LOAD_FACTOR) return;

        size_t newCapacity = m_capacity + (m_capacity >> 1);
        Entry* newEntries = new Entry[newCapacity];

        m_size = 0;
        for (size_t i = 0; i < m_capacity; i++)
        {
            const Entry& entry = m_entries[i];
            if (entry.key.isNull()) continue;

            Entry& dest = find(entry.key);
            dest.key = entry.key;
            dest.value = entry.value;
            m_size++;
        }

        delete[] m_entries;
        m_capacity = newCapacity;
        m_entries = newEntries;
    }

} // namespace Lux
