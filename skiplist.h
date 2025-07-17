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
    SkipList(int max_level): max_level(max_level), skip_list_level(0), element_count(0) {
        K key;
        V value;
        head = new Node<K, V>(key, value, max_level);
    }

    ~SkipList() {
        if (_file_writer.is_open()) {
            _file_writer.close();
        }
        if (_file_reader.is_open()) {
            _file_reader.close();
        }
        // 递归删除跳表链条
        if (head->forward[0] != nullptr) {
            clear(head->forward[0]);
        }
        delete head;
    }

    void clear(Node<K, V>* cur) {
        if (cur->forward[0] != nullptr) {
            clear(cur->forward[0]);
        }
        delete cur;
    }

    int size() {
        return element_count;
    }

    int get_random_level() {
        int k = 1;
        while (rand() % 2) {
            k++;
        }
        k = (k < max_level) ? k : max_level;
        return k;
    }

    Node<K, V>* create_node(const K key, const V value, int level) {
        Node<K, V>* node = new Node<K, V>(key, value, level);
        return node;
    }

    // 插入元素操作, 返回0表示插入成功, 返回1表示跳表中已有该元素
    int insert_element(const K key, const V value) {
        mtx.lock();
        Node<K, V>* current = this->head;

        Node<K, V>* update[max_level + 1];
        memset(update, nullptr, sizeof(Node<K, V>*)*(max_level + 1));
        
        // 从最高层更新forward
        for (int i = skip_list_level; i >= 0; --i) {
            while (current->forward[i] != nullptr && current->forward[i]->get_key() < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        // 更新current
        current = current->forward[0];
        // 跳表中已存在key
        if (current != nullptr && current->get_key() == key) {
            std::cout << "key: " << key << ", exist" << std::endl;
            mtx.unlock();
            return 1;
        }

        if (current == nullptr || current->get_key() != key) {
            int random_level == get_random_level();

            // 获得到的随机层级大于调表目前的层级, 则初始化head
            if (random_level >skip_list_level) {
                for (int i = skip_list_level + 1; i < random_level + 1; ++i) {
                    update[i] = head;
                }
                skip_list_level = random_level;
            }

            // 用随机层级初始化新结点
            Node<K, V>* inserted_node = create_node(key, value, random_level);

            // 插入结点
            for (int i = 0; i <= random_level; ++i) {
                inserted_node->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = inserted_node;
            }
            std::cout << "Successfully insert key: " << key << ", value: " << value << std::endl;
            element_count++;
        }
        mtx.unlock()
        return 0;
    }

    // 删除结点key
    void delete_element(K key) {
        mtx;lock();
        Node<K, V>* current = this->head;
        Node<K, V>* update[max_level + 1];
        memset(update, nullptr, sizeof(Node<K, V>*)*(max_level + 1));

        // 从最高层开始更新
        for (int i = skip_list_level; i >= 0; --i) {
            while (current->forward[i] != nullptr && current->forward[i]->get_key < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];
        if (current != nullptr && current->get_key() == key) {
            // 从最低点开始删除结点
            for (int i = 0; i <= skip_list_level; ++i) {
                // 第 i 层已经没有待删除结点了直接退出
                if (update[i]->forword[i] != current) break;
                update[i]->forward[i] = current->forward[i];
            }

            // 删除没有元素的层
            while (skip_list_level > 0 && head->forward[skip_list_level] == 0) {
                --skip_list_level;
            }

            std::cout << "Successfully delete key " << key << std::endl;
            delete current;
            element_count--;
        }
        mtx.unlock();
    }

    bool search_element(K key) {
        std::cout << "search_element-------------------------------------------------\n";
        Node<K, V>* current = head;
        bool res = false;

        for (int i = skip_list_level; i >= 0; --i) {
            while (current->forward[i] && current->forward[i]->get_key() < key) {
                current = current->forward[i];
            }
        }

        current = current->forward[0];
        if (current && current->get_key() == key) {
            std::cout<< "Find key: " << key << ", value: " << current->get_value() << std::endl;
            res = true;
        }
        else {
            std::cout << "No key: " << key << " in the skip list" << std::endl; 
        }

        return res;
    }

    void display_list() {
        std::cout << "\n************************* Skip List *************************\n";

        for (int i = 0; i <= skip_list_level; ++i) {
            Node<K, V>* node = this->head->forward[i];
            std::cout << "Level " << i << ": ";
            while (node != nullptr) {
                std::cout << node->get_key() << ":" << node->get_value() << ";";
                node = node->forward[i];
            }
            std::cout << std::endl;
        }
    }

    // 文件操作-读取文件
    void load_file() {
        _file_reader.open(STORE_FILE);
        std::cout << "load file------------------------------------------------------" << std::endl;
        std::string line;
        std::string* key = new std::string();
        std::string* value = new std::string();
        while (getline(_file_reader, line)) {
            get_key_value_from_string(line, key, value);
            if (key->empty() || value->empty()) continue;
            insert_element(stoi(*key), *value);
            std::cout << "key: " << *key << " value: " << *value << std::endl; 
        }
        delete key;
        delete value;
        _file_reader.close();
    }

    // 文件操作-写入文件
    void dump_file() {
        _file_writer.open(STORE_FILE);
        std::cout << "dump file------------------------------------------------------" << std::endl;
        Node<K, V>* node = this->head->forward[0];
        while (node != nullptr) {
            _file_writer << node->get_key() << delimiter << node->get_value() << std::endl;
            std::cout << node->get_key() << delimiter << node->get_value() << std::endl;
            node = node->forward[0];
        }
        _file_writer.flush();
        _file_writer.close();
    }
};