#pragma once

template <typename T>
class Storage
{
private:
    T data[100];
    int count;

public:
    Storage() : count(0) {}

    // ── Add ───────────────────────────────────────────────────────────────────
    bool add(const T &item)
    {
        if (count >= 100)
            return false;
        data[count++] = item;
        return true;
    }

    // ── Remove by index (internal helper) ────────────────────────────────────
    void removeAt(int idx)
    {
        if (idx < 0 || idx >= count)
            return;
        for (int i = idx; i < count - 1; i++)
        {
            data[i] = data[i + 1];
        }
        count--;
    }

    
    bool removeByID(int id)
    {
        for (int i = 0; i < count; i++)
        {
            if (data[i].getID() == id)
            {
                removeAt(i);
                return true;
            }
        }
        return false;
    }

    // ── Find by ID — returns pointer or nullptr ───────────────────────────────
    T *findByID(int id)
    {
        for (int i = 0; i < count; i++)
        {
            if (data[i].getID() == id)
                return &data[i];
        }
        return nullptr;
    }

    const T *findByID(int id) const
    {
        for (int i = 0; i < count; i++)
        {
            if (data[i].getID() == id)
                return &data[i];
        }
        return nullptr;
    }

    // ── Accessors ─────────────────────────────────────────────────────────────
    T &get(int idx) { return data[idx]; }
    const T &get(int idx) const { return data[idx]; }
    int size() const { return count; }
    void clear() { count = 0; }

    // ── Get all (returns pointer to internal array) ───────────────────────────
    T *getAll() { return data; }
    const T *getAll() const { return data; }
};
