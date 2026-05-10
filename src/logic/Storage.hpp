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

    // remove function through index
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


    T &get(int idx) { return data[idx]; }
    const T &get(int idx) const { return data[idx]; }

    int size() const { return count; }
    void clear() { count = 0; }


    T *getAll() { return data; }
    const T *getAll() const { return data; }
};
