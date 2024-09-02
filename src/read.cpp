#include <timepix_sort/data_model_chunks.h>
#include <string>
#include <tuple>
#include <vector>
#include <stdexcept>
#include <timepix_sort/read.h>

#include <cassert>
#include <iostream>

static const std::string expected = "TPX3";
namespace tpxd = timepix::data_model;
namespace tpxs = timepix::sort;

std::tuple<uint8_t, int> tpxs::process_header(const int64_t datum)
{
    char token[5];
    for(int i=0; i<4; ++i){
	token[i] = (datum >> 8 * i) & 0xFF;
    }
    token[4] = 0;

    if (std::string(token) != expected){
	throw std::runtime_error("header did not contain tpx3");
    }

    const uint8_t chip_nr = (datum >> 32) & 0xFF;
    if (chip_nr < 0 || chip_nr > 3){
	throw std::runtime_error("chip nr not in 0..3");
    }
    int n_entries = ((datum >> 48) & 0xFFFF) / 8;
    if (n_entries == 0){
	throw std::runtime_error("head: namespace timepixer said: no entries");
    }
    return std::make_tuple(chip_nr, n_entries);

}



tpxd::ChunkCollection
tpxs::read_chunks(std::shared_ptr<const std::vector<uint64_t>> buffer)
{

    uint64_t i=0, total_events=0, n_elm =  buffer->size();

    std::vector<tpxd::ChunkAddress> chunk_addresses;
    for(uint64_t cnt = 0; cnt  < n_elm; ){
	int32_t n_events, n_events_check;
	uint8_t chip_nr, chip_nr_check;
	try{
	    std::tie(chip_nr, n_events) = tpxs::process_header(buffer->at(cnt));
	}
	catch(const std::runtime_error &e) {
		std::cerr << "error occured after reading " << i << " chunks " << std::endl;
		std::cerr << " cnt now at "  << cnt << std::endl;
		std::cerr << " buffer size " << buffer->size() << std::endl;
		throw e;
	}
	const auto chunk_address = tpxd::ChunkAddress(cnt, chip_nr, n_events);

	// readback and check if it matches expected data
#if 0
	std::tie(chip_nr_check, n_events_check) = tpxs::process_header(buffer->operator[cnt]);
	assert(chip_nr == chip_nr_check);
	assert(n_events == n_events_check);
#endif
	chunk_addresses.push_back(chunk_address);
	cnt += n_events + 1;
	total_events += n_events;
	++i;
    }
    std::cout << "read " << i << " chunks containing "  << total_events << " events" << std::endl;

    return tpxd::ChunkCollection(buffer, chunk_addresses);
}
