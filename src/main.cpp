#include "skiplist.h"
using namespace skip_list;

int main(int argc, char const* argv[]) {
    SkipList<int, std::string> skip_list_(10);

    // skip_list_.insertElement(1, "是徒为静养");
    skip_list_.insertElement(3, "而不用克己工夫也");
    // skip_list_.insertElement(7, "如此");
    // skip_list_.insertElement(8, "临事便要倾倒");
    // skip_list_.insertElement(9, "人需在事上磨");
    skip_list_.insertElement(11, "事以秘成");
    skip_list_.insertElement(12, "尽人事知天命");
    skip_list_.insertElement(13, "憾无穷");
    skip_list_.insertElement(14, "人生长恨水长东");
    skip_list_.insertElement(15, "天涯去后");
    skip_list_.insertElement(16, "乡关外");
    skip_list_.insertElement(17, "听风声诉幽怀");
    skip_list_.insertElement(18, "繁华落尽终是一场空");
    skip_list_.insertElement(19, "方可立得住");

    skip_list_.dumpFile();

    std::cout << "skip_list size:" << skip_list_.size() << std::endl;

    skip_list_.searchElement(9);
    skip_list_.searchElement(18);
    skip_list_.searchElement(29);

    skip_list_.displayList();

    skip_list_.deleteElement(7);
    skip_list_.deleteElement(19);
    std::cout << "skip_list size:" << skip_list_.size() << std::endl;
    
    skip_list_.displayList(); 

    return 0;
}