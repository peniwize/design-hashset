/*!
    \file "main.cpp"

    Author: Matt Ervin <matt@impsoftware.org>
    Formatting: 4 spaces/tab (spaces only; no tabs), 120 columns.
    Doc-tool: Doxygen (http://www.doxygen.com/)

    https://leetcode.com/problems/design-hashset/
*/

//!\sa https://github.com/doctest/doctest/blob/master/doc/markdown/main.md
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "utils.hpp"

/*
    Design a HashSet without using any built-in hash table libraries.

    Implement MyHashSet class:

        * void add(key)
          Inserts the value key into the HashSet.

        * bool contains(key)
          Returns whether the value key exists in the HashSet or not.

        * void remove(key)
          Removes the value key in the HashSet.
          If key does not exist in the HashSet, do nothing.

    Constraints:

        * 0 <= key <= 10**6
        * At most 10**4 calls will be made to add, remove, and contains.

    Example 1:

        Input
            ["MyHashSet", "add", "add", "contains", "contains", "add", "contains", "remove", "contains"]
            [[], [1], [2], [1], [3], [2], [2], [2], [2]]
        Output
            [null, null, null, true, false, null, true, null, false]

        Explanation
            MyHashSet myHashSet = new MyHashSet();
            myHashSet.add(1);      // set = [1]
            myHashSet.add(2);      // set = [1, 2]
            myHashSet.contains(1); // return True
            myHashSet.contains(3); // return False, (not found)
            myHashSet.add(2);      // set = [1, 2]
            myHashSet.contains(2); // return True
            myHashSet.remove(2);   // set = [1]
            myHashSet.contains(2); // return False, (already removed)

    Your MyHashSet object will be instantiated and called as such:
    MyHashSet* obj = new MyHashSet();
    obj->add(key);
    obj->remove(key);
    bool param_3 = obj->contains(key);
*/
class MyHashSet_Solution1 {
    using entry_t = int;
    using table_t = vector<entry_t>;
    constexpr static size_t const tableCapacity = 10007;
    constexpr static int tombstone = -1;
    table_t table_{};
    size_t size_{};

    size_t hash(int value) const noexcept { return std::hash<int>{}(value) % tableCapacity; }

    table_t::iterator find(int key) {
        auto const startIdx = hash(key);
        auto tableIdx = startIdx;
        do {
            if (table_[tableIdx] == key) {
                return table_.begin() + tableIdx;
            }
            tableIdx = (tableIdx + 1) % table_.size();
        } while(startIdx != tableIdx);

        return table_.end();
    }

public:
    MyHashSet_Solution1() : table_(tableCapacity, tombstone) {}
    
    void add(int key) {
        auto const atCapacity = table_.size() == size_;
        assert(!atCapacity);
        if (!atCapacity) {
            // Find next available slot in the table.
            auto tableIdx = hash(key);
            while (tombstone != table_[tableIdx] && key != table_[tableIdx]) {
                tableIdx = (tableIdx + 1) % table_.size();
            }
            
            // Add the value to the slot.
            auto const isSameKey = key == table_[tableIdx];
            assert(tombstone == table_[tableIdx] || isSameKey);
            table_[tableIdx] = std::move(key);
            if (!isSameKey) {
                ++size_;
            }
        }
    }
    
    void remove(int key) {
        auto iter = find(key);
        auto const found = table_.end() != iter;
        if (found) {
            assert(key == *iter);
            *iter = tombstone;
            --size_;
        }
    }
    
    bool contains(int key) {
        auto iter = find(key);
        auto const found = table_.end() != iter;
        assert(!found || key == *iter);
        return found;
    }
};

/*
    Ultra minimized solution:
    All this does is keep an array of boolean values, which is sufficient
    for this problem because the key value is never actually used.  This
    solution uses a lot of memory and is extremely fast.
*/
class MyHashSet_Solution2 {
    constexpr static size_t const capacity = 10'000'000 + 1;
    using word_t = unsigned int;
    constexpr static size_t const wordSize = sizeof(word_t);
    constexpr static size_t const bitsPerWord = wordSize * 8;
    word_t* table_{};

public:
    MyHashSet_Solution2() {
        constexpr auto const allocCapacity = capacity / bitsPerWord + 1;
        table_ = new word_t[allocCapacity];
        memset(table_, 0, allocCapacity * wordSize);
    }

    virtual ~MyHashSet_Solution2() {
        delete[] table_;
    }

    void add(int key) {
        table_[key / bitsPerWord] |= (1 << (key % bitsPerWord));
    }
    
    void remove(int key) {
        table_[key / bitsPerWord] &= ~(1 << (key % bitsPerWord));
    }
    
    bool contains(int key) {
        return 0 != (table_[key / bitsPerWord] & (1 << (key % bitsPerWord)));
    }
};

/*
    Ultra minimized solution:
    All this does is keep an array of boolean values, which is sufficient
    for this problem because the key value is never actually used.  This
    solution uses a lot of memory and is extremely fast.
*/
class MyHashSet_Solution3 {
    constexpr static size_t const capacity = 10'000'000 + 1;
    vector<bool> table_{};

public:
    MyHashSet_Solution3() : table_(capacity) {}

    void add(int key) {
        table_[key] = true;
    }
    
    void remove(int key) {
        table_[key] = false;
    }
    
    bool contains(int key) {
        return table_[key];
    }
};

// [----------------(120 columns)---------------> Module Code Delimiter <---------------(120 columns)----------------]

//#define MyHashSet MyHashSet_Solution1
//#define MyHashSet MyHashSet_Solution2
#define MyHashSet MyHashSet_Solution3
#define null false

void test(
    MyHashSet& solution
    , vector<string> const& instructions
    , vector<vector<int>> const& inputs
    , vector<bool> const& expected
)
{
    assert(instructions.size() == inputs.size());
    assert(instructions.size() == expected.size());
    for (size_t idx = 0; instructions.size() > idx; ++idx) {
        if ("add" == instructions[idx])
        {
            for (auto const input : inputs[idx])
            {
                solution.add(input);
            }
        }
        else if ("contains" == instructions[idx])
        {
            assert(!inputs[idx].empty());
            CHECK(expected[idx] == solution.contains(inputs[idx][0]));
        }
        else if ("remove" == instructions[idx])
        {
            assert(!inputs[idx].empty());
            solution.remove(inputs[idx][0]);
        }
        else
        {
            assert(false);
        }
    }
}

TEST_CASE("Case 1")
{
    cerr << "Case 1" << '\n';
    { // New scope.
        auto const instructions = vector<string>{"add", "add", "contains", "contains", "add", "contains", "remove", "contains"};
        auto const inputs = vector<vector<int>>{{1}, {2}, {1}, {3}, {2}, {2}, {2}, {2}};
        auto const expected = vector<bool>{null, null, true, false, null, true, null, false};
        MyHashSet solution{};
        test(solution, instructions, inputs, expected);
        auto const start = std::chrono::steady_clock::now();
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << '\n';
}

/*
    End of "main.cpp"
*/
