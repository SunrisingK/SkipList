#include <iostream>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <fstream>
#include <memory>
#include <vector>

namespace kv_node {
// 实现跳表结点
template<typename K, typename V>
class Node {
private:
    K key;
    V value;

public:
    int node_level;
    std::vector<std::shared_ptr<Node<K, V>>> forward;

    Node() {}

    Node(const K key, const V value, int level): key(key), 
    value(value), node_level(level), forward(level, nullptr) {
    
    }

    const K getKey() const {
        return this->key;
    }

    const V getValue() const {
        return this->value;
    }

    void setValue(V value) {
        this->value = value;
    }   
};

template<typename K, typename V>
using NodeVec = std::vector<std::shared_ptr<kv_node::Node<K, V>>>;

} // namespace kv_node

namespace skip_list {
const std::string delimiter = ":";
const std::string STORE_FILE = "../store/dumpFile";

// 实现跳表
template <typename K, typename V>
class SkipList {
private:
    int max_level;                                      // 跳表的最大层级
    int currentt_level;                                 // 跳表当前的层级

    std::shared_ptr<kv_node::Node<K, V>> head;          // 头结点指针

    // 文件操作
    std::ofstream file_writer;
    std::ifstream file_reader;

    // 互斥锁
    std::mutex mtx;

    int element_count;                                  // 跳表目前元素计数

private:
    void getKeyValueFromString(const std::string& str, std::string key, std::string value) const {
        if (!isValidString(str)) return;

        key = str.substr(0, str.find(delimiter));
        value = str.substr(str.find(delimiter) + 1, str.size());
    }


    bool isValidString(const std::string& str) const {
        return !str.empty() && str.find(delimiter) != std::string::npos;
    }

public:
    SkipList(int max_level): max_level(max_level), currentt_level(0), element_count(0) {
        head = std::make_shared<kv_node::Node<K, V>>(K(), V(), max_level);
    }

    ~SkipList() {
        if (file_writer.is_open()) {
            file_writer.close();
        }
        if (file_reader.is_open()) {
            file_reader.close();
        }
        // 递归删除跳表
        // if (head->forward[0] != nullptr) {
        //     clear(head->forward[0]);
        // }
        // delete head;
    }

    // void clear(Node<K, V>* cur) {
    //     if (cur->forward[0] != nullptr) {
    //         clear(cur->forward[0]);
    //     }
    //     delete cur;
    // }

    const int size() const {
        return element_count;
    }

    int getRandomLevel() const {
        int k = 1;
        while (rand() % 2) {
            k++;
        }
        k = (k < max_level) ? k : max_level;
        return k;
    }

    std::shared_ptr<kv_node::Node<K, V>> createNode(const K key, const V value, int level) {
        auto node = std::make_shared<kv_node::Node<K, V>>(key, value, level);
        return node;
    }

    // 插入元素操作, 返回0表示插入成功, 返回1表示跳表中已有该元素
    int insertElement(const K key, const V value) {
        std::unique_lock<std::mutex> lock_(mtx);    // 自动加锁，等价于mtx.lock()
        std::shared_ptr<kv_node::Node<K, V>> current = this->head;

        auto update = kv_node::NodeVec<K, V>(max_level + 1);
        
        // 从最高层更新forward
        for (int i = currentt_level; i >= 0; --i) {
            while (current->forward[i] != nullptr && current->forward[i]->getKey() < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        // 到达第0层更新current
        current = current->forward[0];
        // 跳表中已存在key
        if (current != nullptr && current->getKey() == key) {
            std::cout << "key: " << key << " exist" << std::endl;
            lock_.unlock();
            return 1;
        }

        if (current == nullptr || current->getKey() != key) {
            int random_level = getRandomLevel();

            // 获得到的随机层级大于调表目前的层级, 则初始化head
            if (random_level >currentt_level) {
                for (int i = currentt_level + 1; i < random_level + 1; ++i) {
                    update[i] = head;
                }
                currentt_level = random_level;
            }

            // 用随机层级初始化新结点
            std::shared_ptr<kv_node::Node<K, V>> inserted_node = createNode(key, value, random_level);

            // 插入结点
            for (int i = 0; i <= random_level; ++i) {
                inserted_node->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = inserted_node;
            }
            std::cout << "Successfully insert key: " << key << ", value: " << value << std::endl;
            element_count++;
        }
        lock_.unlock();
        return 0;
    }

    // 删除结点key
    void deleteElement(K key) {
        std::unique_lock<std::mutex> lock_;     // 自动加锁，等价于mtx.lock()
        
        std::shared_ptr<kv_node::Node<K, V>> current = this->head;
        auto update = kv_node::NodeVec<K, V>(max_level + 1);

        // 从最高层开始更新
        for (int i = currentt_level; i >= 0; --i) {
            while (current->forward[i] != nullptr && current->forward[i]->getKey() < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];
        if (current != nullptr && current->getKey() == key) {
            // 从最低点开始删除结点
            for (int i = 0; i <= currentt_level; ++i) {
                // 第 i 层已经没有待删除结点了直接退出
                if (update[i]->forward[i] != current) break;
                update[i]->forward[i] = current->forward[i];
            }

            // 删除没有元素的层
            while (currentt_level > 0 && head->forward[currentt_level] == 0) {
                --currentt_level;
            }

            std::cout << "Successfully delete key " << key << std::endl;
            --element_count;
        }
        lock_.unlock();
    }

    bool searchElement(K key) {
        std::cout << "search element-------------------------------------------------\n";
        std::shared_ptr<kv_node::Node<K, V>> current = head;
        bool res = false;

        for (int i = currentt_level; i >= 0; --i) {
            while (current->forward[i] && current->forward[i]->getKey() < key) {
                current = current->forward[i];
            }
        }

        current = current->forward[0];
        if (current && current->getKey() == key) {
            std::cout<< "Find key: " << key << ", value: " << current->getValue() << std::endl;
            res = true;
        }
        else {
            std::cout << "No key: " << key << " in the skip list" << std::endl; 
        }

        return res;
    }

    void displayList() {
        std::cout << "\n************************* Skip List *************************\n";

        for (int i = 0; i <= currentt_level; ++i) {
            auto node = this->head->forward[i];
            std::cout << "Level " << i << ": ";
            while (node != nullptr) {
                std::cout << node->getKey() << ":" << node->getValue() << ";";
                node = node->forward[i];
            }
            std::cout << std::endl;
        }
    }

    // 文件操作-读取文件
    void loadFile() {
        file_reader.open(STORE_FILE);
        std::cout << "load file------------------------------------------------------" << std::endl;
        std::string line;
        std::string key;
        std::string value;
        while (getline(file_reader, line)) {
            getKeyValueFromString(line, key, value);
            if (key.empty() || value.empty()) continue;
            insertElement(stoi(key), value);
            std::cout << "key: " << key << " value: " << value << std::endl; 
        }
        file_reader.close();
    }

    // 文件操作-写入文件
    void dumpFile() {
        file_writer.open(STORE_FILE);
        std::cout << "dump file------------------------------------------------------" << std::endl;
        auto node = this->head->forward[0];
        while (node != nullptr) {
            file_writer << node->getKey() << delimiter << node->getValue() << std::endl;
            std::cout << node->getKey() << delimiter << node->getValue() << std::endl;
            node = node->forward[0];
        }
        file_writer.flush();
        file_writer.close();
    }
};
}