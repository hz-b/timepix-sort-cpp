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

}// namespace timepix::sort
#endif // TIMEPIX_PROCESS_H
