#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>

const std::string STORE_FILE = "store/dumpFile";

std::mutex mtx;
const std::string delimiter = ":";

template<typename K, typename V>
class Node {
private:
    K key;
    V value;

public:
    int node_level;
    Node<K, V>** forward; // 存储前向指针

    Node() {}

    Node(const K key, const V value, int level): key(key), value(value), node_level(level) {
        // level + 1
        this->forward = new Node<K, V>*[level + 1];

        // 将前向指针数组填充为nullptr
        memset(this->forward, nullptr, sizeof(Node<K, V>*)*(level + 1));
    }

    ~Node() {
        // 释放forward数组
        delete []forward;
    }

    const K get_key() const {
        return this->key;
    }

    const V get_value() const {
        return this->value;
    }

    void set_value(V value) {
        this->value = value;
    }   
};

// 实现跳表
template <typename K, typename V>
class SkipList {

};