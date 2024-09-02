#ifndef TIMEPIX_DATA_MODEL_CHUNKS_H
#define TIMEPIX_DATA_MODEL_CHUNKS_H

#include <stdint.h>
#include <memory>
#include <vector>
#include <cassert>
#include <ranges>

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
	inline size_t size() const { return this->m_chunk_addresses.size(); }

	inline ChunkView operator[](const size_t index) const;

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

    inline ChunkView ChunkCollection::operator[](const size_t index) const {
	return this->get(index);
    }


} // namespace timepi::datamodel
#endif // TIMEPIX_DATA_MODEL_CHUNKS_H
