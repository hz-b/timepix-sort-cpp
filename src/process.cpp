#include <timepix_sort/read.h>
#include <timepix_sort/process.h>
#include <timepix_sort/detail/process_chunks.h>
#include <timepix_sort/data_model.h>
#include <timepix_sort/utils.h>
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>


/*
enum TDC1TriggerMode{
    rising_edge = 0x6F,
    falling_edge = 0x6A
};

enum TDC2TriggerMode{
    rising_edge = 0x6E,
    falling_edge = 0x6B
};
*/

enum TDCEventType{
    timestamp = 0x6,
    pixel = 0xb
};

namespace tpxs = timepix::sort;
namespace tpxd = timepix::sort::detail;
namespace dm = timepix::data_model;


std::vector<timepix::data_model::Event>
tpxs::process(
    const timepix::data_model::ChunkCollection& collection,
    const int select_trigger_mode,
    const uint64_t minimum_time_over_threshold
    )
{

    uint64_t n_events=0, n_pixels=0, n_timestamps=0,  n_timestamps_with_trigger=0;
    using timepix::data_model::Event;
    std::vector<Event> events;

    n_events=0;
    for(size_t i=0; i < collection.size(); ++i){
	n_events += collection.get(i).n_events();
    }

    // find out how much memory ... allow for over comittment
    events.reserve(n_events);

    n_events=0;
    for(size_t i=0; i < collection.size(); ++i){
	const auto& view = collection.get(i);
	const auto& raw_events = view.events();
	// Todo: recheck that the header is found !
#if 0
	{
	    assert(events.size() == view.n_events());
	    char chip_nr;
	    int check;
	    std::tie(chip_nr, check) = tpxs::process_header(view.header());
	    assert(check == events.size());

	}
#endif

	for(const uint64_t ev : raw_events){
	    int event_type = (ev >> 60) & 0xf;
	    int trigger_mode = ev >> 56;

	    switch(event_type){
	    case timestamp:
		n_timestamps++;
		if (select_trigger_mode == trigger_mode){
		    events.push_back(std::move(tpxd::tdc_time_stamp(ev)));
		    n_timestamps_with_trigger++;
		}
		break;
	    case pixel:
		n_pixels++;
		const auto pix_ev = tpxd::unfold_pixel_event(ev,  view.chip_nr());
		if (pix_ev.time_over_threshold() >= minimum_time_over_threshold) {
		    events.push_back(Event(std::move(pix_ev)));
		}
		break;
	    }
	    n_events++;
	}
    }
    std::cout << "procssed " << collection.size() << " chunks"
	      << " containing"
	      << "\n\t events" << n_events
	      << "\n\t pixels " << n_pixels
	      << "\n\t timestamps " << n_timestamps
	      << "\n\t timestamps with incorrect trigger (edge)" << n_timestamps - n_timestamps_with_trigger
	      << "\n\t remaining " << int64_t(n_events) - int64_t(n_pixels + n_timestamps)
	      << std::endl;

    return events;
}


std::vector<size_t>
tpxs::sort_indices(const dm::EventCollection& col)
{
    std::vector<uint64_t> timestamps;
    timestamps.reserve(col.size());
    std::transform(
	col.begin(), col.end(), std::back_inserter(timestamps),
	[](const auto &ev){ return ev.time_of_arrival(); }
	);

    return timepix::sort::detail::sort_indices(timestamps);
}


std::vector<dm::PixelEvent>
tpxs::calculate_diff_time(const dm::EventCollection& col, const std::vector<size_t>& indices)
{

    std::vector<dm::PixelEvent> pixel_events_diff_time;
    pixel_events_diff_time.reserve(col.size());

    uint64_t stamp_of_last_trigger = -1;
    bool have_found_trigger = false;

    for(size_t i = 0; i<indices.size(); ++i){
	const auto& ev = col[indices[i]];
	if(ev.is_trigger_event()){
	    if(have_found_trigger == false){
		std::cerr << "Found first trigger for"
			  << " i = " << i
			  << " index " << indices[i]
			  << " !" << std::endl;
	    }
	    have_found_trigger = true;
	    stamp_of_last_trigger = ev.time_of_arrival();
	    continue;
	}
	assert(ev.is_pixel_event());
	if(have_found_trigger == false){
	    // no trigger yet, ignore predated pixel events
	    continue;
	}
	pixel_events_diff_time.push_back(ev.pixel_event_with_diff_time(stamp_of_last_trigger));
    }

    return pixel_events_diff_time;
}


void tpxs::sort_pixel_events(std::vector<dm::PixelEvent>& pixel_events)
{

    __gnu_parallel::sort(
	pixel_events.begin(), pixel_events.end(),
	[] (const auto& lhs, const auto& rhs) {
	    return lhs.time_of_arrival() < rhs.time_of_arrival();
	}
	);

}
