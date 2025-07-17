#include <chrono>
#include <thread>
#include <random>
#include <functional>
#include "../src/skiplist.h"

// 匿名命名空间, 将常量限制在当前文件作用域内
namespace {
    constexpr int NUM_THREADS = 1;
    constexpr int TEST_COUNT = 100000;
    skip_list::SkipList<int, std::string> test_skip_list(18);   // 最大高度为18的跳表
}

int getRandomNumber(std::mt19937& gen) {
    std::uniform_int_distribution<int> distrib(0, 99);
    return distrib(gen);
}

void testInsertElement(int tid, std::mt19937& gen) {
    std::cout << tid << std::endl;
    int count_each_thread = TEST_COUNT / NUM_THREADS;
    for (int i = tid * count_each_thread; i < (tid + 1) * count_each_thread; ++i) {
        test_skip_list.insertElement(getRandomNumber(gen) % TEST_COUNT, "rain");
    }
}

void testGetElement(int tid, std::mt19937& gen) {
    std::cout << tid << std::endl;
    int count_each_thread = TEST_COUNT / NUM_THREADS;
    for (int i = tid * count_each_thread; i < (tid + 1) * count_each_thread; ++i) {
        test_skip_list.searchElement(getRandomNumber(gen) % TEST_COUNT);
    }
}

int main(int argc, char const* argv[]) {
    // 随机数生成器初始化
    unsigned seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);     // Mersenne Twister算法, std::mt19937提供高质量伪随机数生成

    // 线程容器
    {
        std::vector<std::thread> threads(NUM_THREADS);
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < NUM_THREADS; ++i) {
            std::cout << "main(): creating thread, " << i << std::endl;
            threads.emplace_back(testInsertElement, i, std::ref(gen));
        }

        for (auto& thread: threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;

        std::cout << "insert elapsed:" << elapsed.count() << std::endl;
        std::cout << "test insert element end."  << std::endl;
    }

    // 线程容器
    // {
    //     std::vector<std::thread> threads(NUM_THREADS);
    //     auto start = std::chrono::high_resolution_clock::now();

    //     for (int i = 0; i < NUM_THREADS; ++i) {
    //         std::cout << "main(): creating thread, " << i << std::endl;
    //         threads.emplace_back(testGetElement, i, std::ref(gen));
    //     }

    //     for (auto& thread: threads) {
    //         if (thread.joinable()) {
    //             thread.join();
    //         }
    //     }

    //     auto finish = std::chrono::high_resolution_clock::now();
    //     std::chrono::duration<double> elapsed = finish - start;

    //     std::cout << "get elapsed:" << elapsed.count() << std::endl;
    //     std::cout << "test get element end."  << std::endl;
    // }

    return 0;
}