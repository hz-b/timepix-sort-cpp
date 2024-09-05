#include <algorithm>
#include <vector>
#include <iterator>
#include <iostream>
#include <iomanip>


int main(int argc, char *argv[])
{
    std::vector<int> test({2, 4, 6,8});
    const auto res_elm = std::min_element(test.begin(), test.end());
    const auto res = std::min({2, 4, 6,8});

    std::cout << "element "
      	      << "\n\t min elm res  "  << typeid(res_elm).name()
      	      << "\n\t min     res  "  << typeid(res).name()
	      << std::endl;

    std::cout << res << std::endl;
    std::cout << "min element has value " << *res_elm
	      <<" and index ["
	      << std::distance(test.begin(), res_elm) << "]"
	      << std::endl;
/*
    for(const auto& r : res){
	std::cout << "r " << std::endl;
    }
*/
/*
	      << ".."       << std::max_element(test.begin(), test.end())
	      << "\n\t min" << std::min(test.begin(), test.end())
	      << ".."       << std::max(test.begin(), test.end())
*/
    return 0;
}
