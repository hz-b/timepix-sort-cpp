#include <timepix_sort/read.h>
#include <timepix_sort/process.h>
#include <timepix_sort/detail/process_chunks.h>
#include <timepix_sort/data_model.h>
#include <timepix_sort/utils.h>
#include <algorithm>
#include <cassert>
#include <ios>
#include <iostream>
#include <sstream>
#include <vector>


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
    pixel = 0xb,
    global_time = 0x4,
    control_indication = 0x7,
};

namespace tpxs = timepix::sort;
namespace tpxd = timepix::sort::detail;
namespace dm = timepix::data_model;




std::pair<std::vector<timepix::data_model::Event>,dm::EventStatistics>
tpxs::process(
    const timepix::data_model::ChunkCollection& collection,
    const int select_trigger_mode,
    const uint64_t minimum_time_over_threshold
    )
{

    using timepix::data_model::Event;
    dm::EventStatistics ev_stat;
    std::vector<Event> events;

    for(size_t i=0; i < collection.size(); ++i){
	ev_stat.inc_n_events();
    }

    // find out how much memory ... allow for over comittment
    events.reserve(ev_stat.n_events());
    ev_stat.reset_n_events();

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
	    int event_type = (ev >> 60);
	    int trigger_mode = ev >> 56;
	    ev_stat.inc_n_events();
	    switch(event_type){
	    case timestamp:
		ev_stat.inc_n_timestamps();
		if (select_trigger_mode == trigger_mode) {
		    events.push_back(std::move(tpxd::tdc_time_stamp(ev)));
		    ev_stat.inc_n_timestamps_with_trigger();
		}
		break;
	    case pixel:
	    { // Why do I need this brackets ?
		ev_stat.inc_n_pixels();
		const auto pix_ev = tpxd::unfold_pixel_event(ev,  view.chip_nr());
		if (pix_ev.time_over_threshold() >= minimum_time_over_threshold) {
		    events.push_back(Event(std::move(pix_ev)));
		}
	    } //
	    break;
	    case global_time:
		ev_stat.inc_n_global_time();
		break;
	    case control_indication:
		ev_stat.inc_n_control_indications();
		break;
	    case 0xf:
		break;
	    default:
		std::stringstream strm;
		strm << "Found unknown event data type " << std::hex << event_type
		     << " in data";
		throw std::invalid_argument(strm.str());
	    }
	}
    }
    return std::make_pair(std::move(events), std::move(ev_stat));
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
