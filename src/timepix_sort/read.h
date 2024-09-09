#ifndef TIMEPIX_READ_H
#define TIMEPIX_READ_H

#include <timepix_sort/data_model_chunks.h>

namespace timepix::sort {


    timepix::data_model::ChunkCollection
	read_chunks(std::shared_ptr<const std::vector<uint64_t>> buffer);

    std::tuple<uint8_t, uint64_t, uint8_t> process_header(const uint64_t datum);

    namespace detail {
	// todo: move to separate header
	std::shared_ptr<std::vector<uint64_t>> read_raw(std::string filename);
    }


}// namespace timepix::sort
#endif // TIMEPIX_READ_H
