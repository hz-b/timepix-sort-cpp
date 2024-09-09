#include <timepix_sort/data_model_chunks.h>
#include <string>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <timepix_sort/read.h>

#include <cassert>
#include <iostream>

static const std::string expected = "TPX3";
namespace tpxd = timepix::data_model;
namespace tpxs = timepix::sort;

std::tuple<uint8_t, uint64_t, uint8_t> tpxs::process_header(const uint64_t datum)
{

    std::array<int8_t,8> header {
	(char) datum,
	(char)(datum >> 8),
	(char)(datum >> 16),
	(char)(datum >> 24),
	(char)(datum >> 32),
	(char)(datum >> 40),
	(char)(datum >> 48),
	(char)(datum >> 56)
    };

    if(header[0] == 'T' && header[1] == 'P' && header[2] == 'X'){
    } else {
	std::invalid_argument("header not valid!");
    }

    int size = ((0xff & header[7]) << 8) | (0xff & header[6]);
    const int8_t chip_nr = header[4];
    const uint8_t mode = header[5];


    if (chip_nr < 0 || chip_nr > 3){
	std::cerr << "chip nr "<< int(chip_nr) << " not in 0..3" << std::endl;
	// throw std::runtime_error("chip nr not in 0..3");
    }
    assert(size > 0);
    const uint64_t n_entries = size / 8;
    const uint8_t padding = size % 8;
    if(padding != 0){
	std::stringstream strm;
	strm << "Header gave size in (bytes) " << size
	     << " which corresponds to " << n_entries
	     << " events. But it leaves " << padding
	     << " bytes ";
	throw std::invalid_argument(strm.str());
    }
    return std::make_tuple(chip_nr, n_entries, padding);

}



tpxd::ChunkCollection
tpxs::read_chunks(std::shared_ptr<const std::vector<uint64_t>> buffer)
{

    uint64_t chunk_nr=0, total_events=0, n_elm =  buffer->size();

    std::vector<tpxd::ChunkAddress> chunk_addresses;
    for(uint64_t cnt = 0; cnt  < n_elm; ){
	int32_t n_events, n_events_check;
	uint8_t chip_nr, chip_nr_check, n_padding;
	try{
	    std::tie(chip_nr, n_events, n_padding) = tpxs::process_header(buffer->at(cnt));
	}
	catch(const std::runtime_error &e) {
		std::cerr << "error occured after reading " << chunk_nr << " chunks " << std::endl;
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
	++chunk_nr;
    }
    std::cout << "read " << chunk_nr << " chunks containing "  << total_events << " events" << std::endl;

    return tpxd::ChunkCollection(buffer, chunk_addresses);
}
