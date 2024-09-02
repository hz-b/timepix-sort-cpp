#ifndef TIMEPIX_UTILS_H
#define TIMEPIX_UTILS_H

#include <parallel/algorithm>

namespace timepix::sort::detail {
// https://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
    template <typename T>
	std::vector<size_t> sort_indices(const std::vector<T> &v) {

	// initialize original index locations
	std::vector<size_t> idx(v.size());
	std::iota(idx.begin(), idx.end(), 0);

	// sort indexes based on comparing values in v
	// using std::stable_sort instead of std::sort
	// to avoid unnecessary index re-orderings
	// when v contains elements of equal values
	// std::stable_sort
	//__gnu_parallel::stable_sort
	__gnu_parallel::sort
	    (idx.begin(), idx.end(),
			 [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});
	return idx;
    }
    template <typename T>
	std::vector<size_t> sort_indices_time_of_arrival(const std::vector<T> &v) {

	// initialize original index locations
	std::vector<size_t> idx(v.size());
	std::iota(idx.begin(), idx.end(), 0);

	// sort indexes based on comparing values in v
	// using std::stable_sort instead of std::sort
	// to avoid unnecessary index re-orderings
	// when v contains elements of equal values
	// std::stable_sort
	//__gnu_parallel::stable_sort
	__gnu_parallel::sort
	    (idx.begin(), idx.end(),
	     [&v](size_t i1, size_t i2) {return v[i1].time_of_arrival() < v[i2].time_of_arrival();});
	return idx;
    }

};
#endif //TIMEPIX_UTILS_H
