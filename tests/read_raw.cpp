#include "mmappable_vector.h"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <timepix_sort/read.h>

namespace tpxd = timepix::sort::detail;


std::shared_ptr<std::vector<uint64_t>> tpxd::read_raw(std::string filename)
{

    std::filesystem::path p{filename};
    const auto size = std::filesystem::file_size(p);

    //std::cout << "treating file " << p.u8string()
//	      << " with size " << size << "\n";

    using namespace mmap_allocator_namespace;
    mmappable_vector<uint64_t> read_buffer;
    // mmappable_vector<uint64_t> read_buffer(filename, READ_ONLY, 0, size);
    const auto timestamp_start{std::chrono::steady_clock::now()};
    read_buffer.mmap_file(filename, READ_ONLY, 0, size);
    const auto timestamp_mapped{std::chrono::steady_clock::now()};

    const auto buffer = std::make_shared<std::vector<uint64_t>>(read_buffer.begin(), read_buffer.begin() +  read_buffer.size() / 8);
    const auto timestamp_vector{std::chrono::steady_clock::now()};

    const std::chrono::duration<double>
	for_mapping { timestamp_mapped - timestamp_start  } ,
	for_buffer  { timestamp_vector - timestamp_mapped };

    std::cout << "Processing raw data required "
	      << "\n\t mapping     " << for_mapping.count()
	      << "\n\t into buffer " << for_buffer.count()
	      << std::endl;

    return buffer;

}
