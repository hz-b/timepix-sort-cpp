#ifndef TIMEPIX_DATA_MODEL_H
#define TIMEPIX_DATA_MODEL_H

#include <ostream>
#include <stdint.h>
#include <limits>
#include <vector>
#include <variant>

namespace timepix::data_model {


    class PixelPos{
    private:
	///> pixel pos signed as I am not sure if pixels extend to
	///> negative range in the end
	int16_t m_x, m_y;

    public:
	PixelPos(const int16_t x, const int16_t y)
	    : m_x(x)
	    , m_y(y)
	{}

	inline auto x() const { return this->m_x; }
	inline auto y() const { return this->m_y; }
    };


    template<class D>
    struct HasTimeOfArrival
    {
	inline const auto time_of_arrival() const {
	    return static_cast<const D*>(this)->time_of_arrival_impl();
	}
	void show(std::ostream& o) const {
	    return static_cast<const D*>(this)->show_impl(o);
	}
    };

    struct GlobalTimeStamps
    {
	unsigned long timer_msb16 = 0;

    };

    /**
     * @todo investigate assignment operators
     */
    class TimeOfFlightEvent : public HasTimeOfArrival<TimeOfFlightEvent>{
    private:
	const uint64_t m_time_of_arrival;

    public:

	TimeOfFlightEvent(const uint64_t time_of_arrival)
	    : m_time_of_arrival(time_of_arrival)
	    {}

	const TimeOfFlightEvent& clone() const { return *this; }

	inline const auto time_of_arrival_impl() const {
	    return this->m_time_of_arrival;
	}
	void show_impl(std::ostream& o) const {
	    o << "TimeOfFlightEvent(time_of_arrival="
	      << this->time_of_arrival()
	      << ")";
	}
    };


    /**
     * @todo investigate assignment operators, avoid copy operators
     */
    class PixelEvent : public HasTimeOfArrival<PixelEvent>{
    private:
	// uint64_t m_time_of_arrival;
	uint64_t m_time_of_arrival;
	uint64_t m_time_over_threshold;
	PixelPos m_pos;
	int8_t m_chip_nr;

    public:
	inline PixelEvent(const PixelPos& pos, const uint64_t time_of_arrival, const int64_t time_over_threshold, int8_t chip_nr)
	    : m_time_of_arrival(time_of_arrival)
	    , m_time_over_threshold(time_over_threshold)
	    , m_pos(pos)
	    , m_chip_nr(chip_nr)
	    {}
	/* methods required for sorting? should these be handled by a proxy */

	inline auto time_over_threshold() const { return this->m_time_over_threshold; }
	inline auto x()                   const { return this->m_pos.x();             }
	inline auto y()                   const { return this->m_pos.y();             }
	inline auto pos()                 const { return this->m_pos;                 }
	inline auto chip_nr()             const { return this->m_chip_nr;             }

	inline const auto time_of_arrival_impl() const {
	    return this->m_time_of_arrival;
	}

	void show_impl(std::ostream& o) const {
	    o << "PixelEvent(time_of_arrival="  << this->time_of_arrival()
	      << ", time_over_threshhold=" << this->time_over_threshold()
	      << ", chip_nr=" << int(this->chip_nr())
	      << ", pos=PixelPos(x=" << this->x()
	      << ", y=" << this->y()
	      << "))";
	}

    };


    namespace {
	// helper type for the visitor #4
	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	// explicit deduction guide (not needed as of C++20)
	template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
    }

    // a place holder to signal that a event was recoginsed.
    // It's content was not processed any further
    struct EmptyEvent{
	inline constexpr const auto time_of_arrival() const {
	    return std::numeric_limits<uint64_t>::max();
	}
	void show(std::ostream& o) const {
	    o << "EmptyEvent()";
	}
    };

    typedef struct std::variant<TimeOfFlightEvent,PixelEvent,EmptyEvent> a_event;


    class Event
    {
	const a_event m_event;


    public:
	inline Event(a_event&& event)
	    : m_event(std::move(event))
	    {}

	inline Event(TimeOfFlightEvent&& event)
	    : m_event(std::move(event))
	    {}

	inline Event(PixelEvent&& event)
	    : m_event(std::move(event))
	    {}

	inline Event(EmptyEvent&& event)
	    : m_event(std::move(event))
	    {}

	/**
	 * is that required ?
	 * simplifies writing the pybind11 wrapper
	 * should typically not used by C++ code
	 */
	inline auto& get_event() const {
	    return this->m_event;
	}

	void show(std::ostream& strm) const {
	    std::visit(overloaded{
		    [&strm] (const PixelEvent& ev){
			ev.show(strm);
		    },
		    [&strm] (const auto& ev){
			ev.show(strm);
		    },
		}, this->m_event);
	}

	inline auto time_of_arrival () const {
	    uint64_t t;
	    std::visit(overloaded{
		    [&t] (const PixelEvent& ev){
			t = ev.time_of_arrival();
		    },
		    [&t] (const auto& ev){
			t = ev.time_of_arrival();
		    },
		}, this->m_event);
	    return t;
	}

	inline const TimeOfFlightEvent& as_time_of_flight_event() const {
	    return std::get<TimeOfFlightEvent>(this->m_event);
	}

	inline const PixelEvent as_pixel_event() const {
	    return std::get<PixelEvent>(this->m_event);
	}

	inline bool is_trigger_event() const {
	    return std::holds_alternative<TimeOfFlightEvent>(this->m_event);
	}

	inline bool is_pixel_event() const {
	    return std::holds_alternative<PixelEvent>(this->m_event);
	}
	inline bool is_empty_event() const {
	    return std::holds_alternative<EmptyEvent>(this->m_event);
	}

	/**
	 * Todo:
	 *    should that be a friend function ?
	 *    should it return a specific type?
	 *    time of arrival is inappropriate, furthermore its type could be
	 *    reduced
	 */
	inline PixelEvent pixel_event_with_diff_time(const uint64_t reftime_stamp) const {
	    // const PixelEvent& px_ev = std::get<PixelEvent>(this->m_event);
	    const PixelEvent& px_ev = std::get<PixelEvent>(this->m_event);
	    return std::move(
		PixelEvent(
		    px_ev.pos(),
		    px_ev.time_of_arrival() - reftime_stamp,
		    px_ev.time_over_threshold(),
		    px_ev.chip_nr()
		    )
		);
	}
    };


    class EventCollection
    {
	const std::vector<Event> m_events;

    public:
	inline EventCollection(std::vector<Event>&& events)
	    : m_events(std::move(events))
	    {}

	inline const auto operator[](size_t i) const {
	    return this->m_events[i];
	}
	inline const auto at(size_t i) const {
	    return this->m_events.at(i);
	}
	inline const auto size() const {
	    return this->m_events.size();
	}
	inline const auto begin() const {
	    return this->m_events.begin();
	}
	inline const auto end() const {
	    return this->m_events.end();
	}
    };


};

#endif
