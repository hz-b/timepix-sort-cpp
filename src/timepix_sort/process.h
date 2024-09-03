#ifndef TIMEPIX_PROCESS_H
#define TIMEPIX_PROCESS_H

#include <timepix_sort/data_model_chunks.h>
#include <timepix_sort/data_model.h>

namespace timepix::sort {
    std::vector<timepix::data_model::Event> process(
	const timepix::data_model::ChunkCollection& collection,
	int select_trigger_mode,
	int tot_min
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
