#include <chrono>
#include <iostream>
#include <stdint.h>
#include <memory>
//#include <parallel/algorithm>
#include <algorithm>

#include <timepix_sort/read.h>
#include <timepix_sort/process.h>
#include <timepix_sort/utils.h>

namespace dm = timepix::data_model;

void usage(const char * progname)
{
    std::cout << "Usage " << progname << " file_to_read "   << std::endl;
    exit(1);
}


int main(int argc, char *argv[])
{

    if(argc != 2){
	usage(argv[0]);
    }

    auto now = std::chrono::steady_clock::now;

    const auto buffer = timepix::sort::detail::read_raw(std::string(argv[1]));
    const auto timestamp_start{now()};
    auto r = timepix::sort::read_chunks(buffer);
    const auto timestamp_read{now()};

    const int rising_edge = 0x6E;
    // correct ?
    const int falling_edge = 0x6f;
    auto events = timepix::sort::process(r, falling_edge, 6);
    const auto timestamp_process{now()};
    std::cout << "Got " << events.size() << " timestamps\n";

    const auto timestamp_extract_start{now()};
    std::vector<uint64_t> timestamps(events.size());
    std::transform(
	events.begin(), events.end(), timestamps.begin(),
	[](const auto &ev){ return ev.time_of_arrival(); }
	);

    // indices to the events in sorted manner
    const auto timestamp_sort_start{now()};
    const auto indices = timepix::sort::detail::sort_indices(timestamps);
    const auto timestamp_sort_end{now()};

    std::cerr << "timestamps size " << timestamps.size()
	      << std::endl;

    // check that the timestamps are sorted correctly.
    auto old_timestamp = events[indices[0]].time_of_arrival();
    for(size_t i = 1; i < indices.size(); ++i){
	const auto volatile t_timestamp = events[indices[i]].time_of_arrival();
	assert(old_timestamp <= t_timestamp);
	old_timestamp = t_timestamp;
    }

    std::vector<dm::PixelEvent> pixel_events_diff_time;
    // a little overcommitment here and there
    pixel_events_diff_time.reserve(events.size());

    // force some variables to be visible for the debugger
    volatile uint64_t stamp_of_last_trigger = -1;
    volatile bool have_found_trigger = false;
    // no fancy indexing: need to use a transfrom that is sequential
    for(volatile size_t i = 0; i<indices.size(); ){
	i = i + 1;
	const auto& ev = events[indices[i]];
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
    const auto timestamp_diff_calc_end{now()};


    std::vector<uint64_t> timestamps_diff(pixel_events_diff_time.size());
    std::transform(
	pixel_events_diff_time.begin(),
	pixel_events_diff_time.end(),
	timestamps_diff.begin(),
	[](const auto &ev){ return ev.time_of_arrival(); }
	);


    std::cerr << "sorting " <<
	timestamps_diff.size()
	      << " pixel events by their diff time\n";
    const auto diff_timestamp_sort_start{now()};
    const auto diff_indices = timepix::sort::detail::sort_indices(timestamps_diff);
    const auto diff_timestamp_sort_end{now()};

    //parallel::sort(timestamps.begin(), timestamps.end());

    const std::chrono::duration<double>
	for_reading     { timestamp_read            - timestamp_start           } ,
	for_processing  { timestamp_process         - timestamp_read            } ,
	for_extracting  { timestamp_sort_start      - timestamp_extract_start   } ,
	for_sorting     { timestamp_sort_end        - timestamp_sort_start      } ,
	for_time_diff   { timestamp_diff_calc_end   - timestamp_sort_end        } ,
	for_diff_ext    { diff_timestamp_sort_start - timestamp_diff_calc_end   } ,
	for_diff_sort   { diff_timestamp_sort_end   - diff_timestamp_sort_start } ;

    std::cout << "Processing in main "
	      << "\n\t reading         " << for_reading.count()
	      << "\n\t processing      " << for_processing.count()
	      << "\n\t extracting      " << for_extracting.count()
	      << "\n\t sorting         " << for_sorting.count()
	      << "\n\t time difference " << for_time_diff.count()
	      << "\n\t extracting diff " << for_diff_ext.count()
	      << "\n\t sorting diff    " << for_diff_sort.count()
	      << std::endl;


}
