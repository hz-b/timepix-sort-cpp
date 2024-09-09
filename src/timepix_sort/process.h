#ifndef TIMEPIX_PROCESS_H
#define TIMEPIX_PROCESS_H

#include <timepix_sort/data_model_chunks.h>
#include <timepix_sort/data_model.h>
#include <timepix_sort/event_statistics.h>

namespace timepix::sort {
    std::pair<std::vector<timepix::data_model::Event>,timepix::data_model::EventStatistics>
	process(
	const timepix::data_model::ChunkCollection& collection,
	const int select_trigger_mode,
	const uint64_t minimum_time_over_threshold
	);

    std::vector<size_t> sort_indices(const timepix::data_model::EventCollection& col);

    std::vector<timepix::data_model::PixelEvent>
	calculate_diff_time(const timepix::data_model::EventCollection& col, const std::vector<size_t>& indices);

    /**
     * @warning: in place operation!
     **/
    void
	sort_pixel_events(std::vector<timepix::data_model::PixelEvent>& pixel_events);

    std::vector<timepix::data_model::PixelEvent>
	calculate_diff_time_sorted(
	    const timepix::data_model::EventCollection& col,
	    const std::vector<size_t>& indices
	    );


}// namespace timepix::sort
#endif // TIMEPIX_PROCESS_H
