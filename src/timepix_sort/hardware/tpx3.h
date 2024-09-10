#ifndef TIMEPIX_HARDWARE_TPX3
#define TIMEPIX_HARDWARE_TPX3

#include <stdint.h>


namespace timepix::hardware::tpx3 {

    namespace {
	const uint64_t nanoseconds2femtoseconds = uint64_t(1000* 1000);
	const uint64_t seconds2nanoseconds = uint64_t(1000 * 1000 * 1000);
	const uint64_t seconds2femtoseconds =  nanoseconds2femtoseconds * seconds2nanoseconds;

    }
    const double pixel_max_time = 26.8435456;
    const double tdc_max_time = 107.3741824;

    // need to find out how to compute this time correctly
    const uint64_t pixel_max_time_fs = pixel_max_time * seconds2femtoseconds;
    const uint64_t tdc_max_time_fs = tdc_max_time * seconds2femtoseconds;
}

#endif /* TIMEPIX_HARDWARE_TPX3*/
