#ifndef TIMEPIX_EVENTS_H
#define TIMEPIX_EVENTS_H

#include <timepix_sort/data_model.h>
#include <timepix_sort/process.h>

namespace timepix::sort {

    class PixelEventsDiffTime
    {
    protected:
	// to be able to use numpy buffers to access time of arrival
	std::vector<timepix::data_model::PixelEvent> m_pixel_events;

    private:
	bool m_is_sorted = false;


    public:
	PixelEventsDiffTime(
	    std::vector<timepix::data_model::PixelEvent>&& pixel_events,
	    bool is_sorted = false
	    )
	    : m_pixel_events(std::move(pixel_events))
	    , m_is_sorted(is_sorted)
	    {}

	/*
	  PixelEventsDiffTime(const std::vector<timepix::data_model::PixelEvent>& pixel_events)
	  : m_pixel_events(pixel_events)
	  {}
	*/
	const inline auto size() const { return this->m_pixel_events.size(); }
	const inline auto& at(const size_t i) const { return this->m_pixel_events.at(i); }
	const inline auto& operator[](const size_t i) const { return this->m_pixel_events[i]; }
	const inline auto begin() const { return this->m_pixel_events.begin(); }
	const inline auto end() const { return this->m_pixel_events.end(); }

	const inline auto is_sorted() const { return this->m_is_sorted; }
	void sort() {
	    sort_pixel_events(this->m_pixel_events);
	    this->m_is_sorted = true;
	    // return *this;
	}

    };

    /**
     * part of IGOR code ... needs to be improved
     * perhaps one should separate the pixels artificially a bit at this stage
     * and treat the overlap at the center at a later stage
     *   switch (chip_nr)
     *           case 0:
     *                   xx = 255 - xx
     *                   yy = 255 - yy + 258
     *                   break
     *
     *           case 1:
     *                   xx = xx
     *                   yy = yy
     *                   break
     *
     *           case 2:
     *                   xx = xx + 258
     *                   yy = yy
     *                   break
     *
     *           case 3:
     *                   xx = 255 - xx + 258
     *                   yy = 255 - yy + 258
     *                   break
     *
     *           default:
     *                   break
     *
     *   endswitch
     */
    inline timepix::data_model::PixelPos map_pixel_and_chip_nr_to_global_pixel(
	const timepix::data_model::PixelPos& pos,
	const uint16_t& chip_nr
	)
    {
	using timepix::data_model::PixelPos;
	switch (chip_nr){
	case 0:
	    return std::move(PixelPos(255 - pos.x(), 255 - pos.y() + 258));
	    break;
	case 1:
	    return std::move(pos);
	    break;
	case 2:
	    return std::move(PixelPos(258 + pos.x(), pos.y()));
	    break;
	case 3:
	    return std::move(PixelPos(255  - pos.x() + 258, 255 - pos.y() + 258));
	    break;
	default:
	    throw std::range_error("chip nr out of range!");
	}
    }

};

#endif /* TIMEPIX_EVENTS_H */
