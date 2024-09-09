#include <chrono>
#include <iostream>
#include <stdint.h>
#include <memory>
//#include <parallel/algorithm>
#include <algorithm>
#include <ranges>

#include <timepix_sort/read.h>
#include <timepix_sort/process.h>
#include <timepix_sort/utils.h>
#include <timepix_sort/events.h>

namespace dm = timepix::data_model;
namespace tps = timepix::sort;


static void basis_size(void){
    std::cout
	<< "\nunsigned long long " << sizeof( unsigned long long ) << "\t uint64_t " << sizeof(uint64_t)
	<< "\nunsigned short     " << sizeof( unsigned short     ) << "\t uint16_t " << sizeof(uint16_t)
	<< "\nint                " << sizeof( int                ) << "\t  int32_t " << sizeof(uint32_t)
	<< "\nchar               " << sizeof( char               ) << "\t   int8_t " << sizeof(uint8_t)
	<< std::endl;
}
static void usage(const char * progname)
{
    std::cout << "Usage " << progname << " file_to_read "   << std::endl;
    exit(1);
}


int main(int argc, char *argv[])
{

    basis_size();
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
    auto tmp = timepix::sort::process(r, falling_edge, 6);
    std::cout << "data contained " << std::get<dm::EventStatistics>(tmp);
    auto col = dm::EventCollection(std::move(std::get<std::vector<dm::Event>>(tmp)));
    const auto timestamp_process{now()};
    std::cout << "Got " << col.size() << " timestamps\n";

    const auto timestamp_extract_start{now()};
    std::vector<uint64_t> timestamps(col.size());
    std::transform(
	col.begin(), col.end(), timestamps.begin(),
	[](const auto &ev){ return ev.time_of_arrival(); }
	);

    auto pixel_timestamps = col |					\
	std::views::filter([](const dm::Event &ev){ return ev.is_pixel_event();}) |
	std::views::transform([](const dm::Event &ev){ return ev.time_of_arrival();})
	;
    auto trigger_timestamps = col |					\
	std::views::filter([](const dm::Event &ev){ return ev.is_trigger_event();}) |
	std::views::transform([](const dm::Event &ev){ return ev.time_of_arrival();})
	;

    std::cerr << "pixels time range: "
	      << *std::ranges::min_element(pixel_timestamps.begin(), pixel_timestamps.end())
	      << " .. "
	      << *std::ranges::max_element(pixel_timestamps.begin(), pixel_timestamps.end())
	      << std::endl ;

    std::cerr << "trigger time range: "
	      << *std::ranges::min_element(trigger_timestamps.begin(), trigger_timestamps.end())
	      << " .. "
	      << *std::ranges::max_element(trigger_timestamps.begin(), trigger_timestamps.end())
	      << std::endl ;

    // indices to the events in sorted manner
    const auto timestamp_sort_start{now()};
    const auto indices = timepix::sort::detail::sort_indices(timestamps);
    const auto timestamp_sort_end{now()};

    std::cerr << "timestamps size " << timestamps.size()
	      << std::endl;

    // check that the timestamps are sorted correctly.
    auto old_timestamp = col[indices[0]].time_of_arrival();
    for(size_t i = 1; i < indices.size(); ++i){
	const auto volatile t_timestamp = col[indices[i]].time_of_arrival();
	assert(old_timestamp <= t_timestamp);
	old_timestamp = t_timestamp;
    }

    const auto timestamp_diff_calc_end{now()};
    auto pixel_diff_time = tps::PixelEventsDiffTime(
	std::move(tps::calculate_diff_time(col, indices))
	);
    pixel_diff_time.sort();
    std::cerr << "sorting "
	      << pixel_diff_time.size()
	      << " pixel events by their diff time\n";
    const auto diff_timestamp_sort_start{now()};
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


    // create the histogram ... down here could explode the memory
    // first decide how much axis to build

    std::vector<dm::PixelPos> global_pixel_pos;
    global_pixel_pos.reserve(pixel_diff_time.size());
    std::transform(pixel_diff_time.begin(), pixel_diff_time.end(), std::back_inserter(global_pixel_pos),
		   [](const dm::PixelEvent&ev){
		       return tps::map_pixel_and_chip_nr_to_global_pixel(ev.pos(), ev.chip_nr());
		   }
	);

    auto x_larger = [](const dm::PixelPos& lhs, const dm::PixelPos& rhs){
	return lhs.x() < rhs.x();
    };
    auto y_larger = [](const dm::PixelPos& lhs, const dm::PixelPos& rhs){
	return lhs.y() < rhs.y();
    };

    auto larger = [](const int& lhs, const int& rhs){
	return lhs < rhs;
    };


    std::cout << " event 'volumne' "
	      << "\n\t x "     <<  std::min_element(global_pixel_pos.begin(), global_pixel_pos.end(), x_larger)->x()
	      << " .. "        <<  std::max_element(global_pixel_pos.begin(), global_pixel_pos.end(), x_larger)->x()
	      << "\n\t y "     <<  std::min_element(global_pixel_pos.begin(), global_pixel_pos.end(), y_larger)->y()
	      << " .. "        <<  std::max_element(global_pixel_pos.begin(), global_pixel_pos.end(), y_larger)->y()
	      << "\n\t time " << pixel_diff_time[0].time_of_arrival()
	      << " ..       " << pixel_diff_time.at(pixel_diff_time.size() - 1).time_of_arrival()
	      << std::endl;

    return 0;

}
