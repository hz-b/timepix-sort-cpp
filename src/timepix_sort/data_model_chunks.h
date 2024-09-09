#ifndef TIMEPIX_DATA_MODEL_CHUNKS_H
#define TIMEPIX_DATA_MODEL_CHUNKS_H

#include <stdint.h>
#include <memory>
#include <vector>
#include <cassert>
#include <ranges>

/*
 *  6. Appendix: file formats
 *
 *  6.1 TPX3 raw file format
 *  The .tpx3 raw data files contain the original data as sent by the readout board. It consists of chunks
 *  with an 8 byte header prepended.
 *  The chunk header consists of:
 *      - 4 bytes indicating the type: “TPX3”;
 *      - 1 byte indicating the chip index;
 *      - 1 byte reserved;
 *      - 2 bytes containing the net size of the chunk.
 *  The chunk content consists of 8 byte words (little endian), the type of which is determined by the
 *  most significant nibble (i.e. the high nibble of the last byte):
 *      1. Pixel data with type 0xb, the maximum timestamp is 26.8435456 s;
 *      2. TDC data with type 0x6, the maximum timestamp is 107.3741824 s;
 *      3. Global time with type 0x4, the maximum is ~81 days;
 *      4. Control indications with type 0x7.
 *  The method how to decode this information from the raw data, please refer to the example code.
 *
 *  Table 6.1. TDC data packet
 *
 *    63 - 56 bit              55 - 44 bit     43 - 9 bit     8 - 5 bit      4 – 0 bit
 *       0x6                Trigger counter   Timestamp       Stamp          Reserved
 *
 *  Table 6.2. Pixel data packet
 *
 *    63 - 60 bit              59 - 44 bit    43 - 30 bit    29 - 20 bit     19 – 16 bit       15 – 0 bit
 *       0xb                    PixAddr          ToA            ToT            FToA           SPIDR time
 *
 *
 */
namespace timepix::data_model {

    // Todo: review name chunk: event basket
    struct ChunkAddress{
	uint64_t offset;
	uint32_t n_events;
	uint8_t chip_nr;

	inline ChunkAddress(uint64_t offset_, uint8_t chip_nr_, uint32_t n_events_)
	    : offset(offset_)
	    , n_events(n_events_)
	    , chip_nr(chip_nr_)
	    {}
    };

    /**
     * Access (view) to one chunk, but keeping reference to the chunk collection
     */
    class ChunkView;


    class ChunkCollection{
    private:
	std::shared_ptr<const std::vector<uint64_t>> m_buffer;
	const std::vector<ChunkAddress> m_chunk_addresses;

	inline auto buffer() const { return this->m_buffer; }
	friend class ChunkView;

    public:
    ChunkCollection(
	std::shared_ptr<const std::vector<uint64_t>> buffer,
	const std::vector<ChunkAddress> chunk_addresses
	)
	    : m_buffer(buffer)
	    , m_chunk_addresses(chunk_addresses)
	{}
	inline ChunkView get(const size_t index) const ;
	inline ChunkView get_checked(const size_t index) const ;
	inline size_t size() const { return this->m_chunk_addresses.size(); }

	inline ChunkView operator[](const size_t index) const;
	inline ChunkView at(const size_t index) const;
	ChunkView at_dbg(const size_t index) const;
    };


    class ChunkView{
    private:
	std::shared_ptr<const std::vector<uint64_t>> m_buffer;
	const ChunkAddress m_address;

    public:
    ChunkView(
	const std::shared_ptr<const std::vector<uint64_t>> buffer,
	const ChunkAddress address)
	: m_buffer(buffer)
	, m_address(address)
	{}

	inline uint64_t offset()   const { return this->m_address.offset;   }
	inline uint32_t n_events() const { return this->m_address.n_events; }
	inline auto chip_nr()      const { return this->m_address.chip_nr;  }

	inline auto header()       const { return this->m_buffer->at(this->offset()); }

	inline auto events()       const {
	    const auto start = this->offset() + 1;
	    const auto end   = start + this->n_events();
	    return std::vector<int64_t>(this->m_buffer->begin() + start, this->m_buffer->begin() + end);
	}
    };

    inline ChunkView ChunkCollection::get(size_t index) const {
	return ChunkView(this->m_buffer, this->m_chunk_addresses.at(index));
    }
    inline ChunkView ChunkCollection::get_checked(size_t index) const {
	return ChunkView(this->m_buffer, this->m_chunk_addresses.at(index));
    }

    inline ChunkView ChunkCollection::operator[](const size_t index) const {
	return this->get(index);
    }

    inline ChunkView ChunkCollection::at(const size_t index) const {
	return this->get_checked(index);
    }


} // namespace timepi::datamodel
#endif // TIMEPIX_DATA_MODEL_CHUNKS_H
