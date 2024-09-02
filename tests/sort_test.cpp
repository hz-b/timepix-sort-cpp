//#include <parallel/algorithm>
#include <algorithm>
#include <ostream>
#include <iostream>
#include <stdint.h>
#include <timepix_sort/utils.h>

using timepix::sort::detail::sort_indices;

class SortItem
{
    uint64_t m_index;
    uint64_t m_time_stamp;

public:
    inline SortItem(uint64_t index, uint64_t time_stamp)
	: m_index(index)
	, m_time_stamp(time_stamp)
	{}

    SortItem (SortItem&& o) = default;
    SortItem& operator= (SortItem&& o) = default;

    SortItem (const SortItem& o) = delete;
    SortItem& operator= (const SortItem& o) = delete;

    inline auto getTimeStamp() const {
	return this->m_time_stamp;
    }
    inline const bool operator< (SortItem const &o) const {
	return this->m_time_stamp  <  o.m_time_stamp;
    }
    inline const bool operator== (SortItem const &o) const {
	return this->m_time_stamp  ==  o.m_time_stamp;
    }
    inline const bool operator> (SortItem const &o) const {
	return this->m_time_stamp  >  o.m_time_stamp;
    }

    void show(std::ostream& strm) const {
	strm << "SortItem("
	     << "idx=" << this->m_index
	     << ", t_stmp=" << this->m_time_stamp
	     << ")";
    }

};

std::ostream& operator<<(std::ostream& strm, const SortItem& item) {
    item.show(strm);
    return strm;
}


int main(int argc, char *argv[])
{
    SortItem a(0, 100), b(1, 80), c(2, 42), d(3,53), e(4, 26);

    std::cout << "Sort item          move constructible " << std::is_move_constructible<SortItem>()
	      << "\n          trival move constructible " << std::is_trivially_move_constructible<SortItem>()
	      << "\n                 move assignable    " << std::is_move_assignable<SortItem>()
	      << "\n          trival move assignable    " << std::is_trivially_move_assignable<SortItem>()
	      << std::endl;

    std::vector<SortItem> sorted;
    sorted.push_back(std::move(a));
    sorted.push_back(std::move(b));
    sorted.push_back(std::move(c));
    sorted.push_back(std::move(d));
    sorted.push_back(std::move(e));
    //__gnu_parallel::sort(sorted.begin(), sorted.end());

    const auto indices = sort_indices(sorted);

    std::cout << sorted.size()
	      << ": " << sorted[0]
	      << " -- "  << sorted[sorted.size() / 2]
	      <<" -- " << sorted[sorted.size() - 1]
	      << "." << std::endl;

}
