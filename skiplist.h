#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>

const std::string STORE_FILE = "store/dumpFile";

std::mutex mtx;
const std::string delimiter = ":";

// 实现跳表结点
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
private:
    int max_level;              // 跳表的最大层级
    int skip_list_level;        // 跳表当前的层级

    Node<K, V>* head;           // 头结点指针

    // 文件操作
    std::ofstream _file_writer;
    std::ifstream _file_reader;

    int element_count;          // 跳表目前元素计数

private:
    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value) {
        if (!is_valid_string(str)) return;

        *key = str.substr(0, str.find(delimiter));
        *value = str.substr(str.find(delimiter) + 1, str.size());
    }

    bool is_valid_string(const std::string& str) {
        return !str.empty() && str.find(delimiter) != std::string::npos;
    }

public:
    SkipList();

    Node<K, V>* create_node(const K key, const V value, int level) {
        Node<K, V>* node = new Node<K, V>(key, value, level);
        return node;
    }

    // 插入元素操作, 返回0表示插入成功, 返回1表示跳表中已有该元素
    int insert_element(const K key, const V value) {
        mtx.lock();
        Node<K, V>* current = this->head;
    }
};