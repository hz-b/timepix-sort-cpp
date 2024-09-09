#ifndef TIMEPIX_EVENT_STATISTICS_H
#define TIMEPIX_EVENT_STATISTICS_H

#include <stdint.h>
#include <ostream>

namespace timepix::data_model {
    class EventStatistics
    {
	uint64_t m_n_events=0;
	uint64_t m_n_pixels=0;
	uint64_t m_n_timestamps=0;
	uint64_t m_n_timestamps_with_trigger=0;
	uint64_t m_n_control_indications=0;
	uint64_t m_n_global_time=0;
	uint64_t m_n_event_flagged_zero=0;
	uint64_t m_n_event_flagged_two=0;

    public:
	EventStatistics() = default;

	// for comparison
	inline void inc_n_events()                  { this->m_n_events++;                  }
	inline void inc_n_pixels()                  { this->m_n_pixels++;                  }
	inline void inc_n_timestamps()              { this->m_n_timestamps++;              }
	inline void inc_n_timestamps_with_trigger() { this->m_n_timestamps_with_trigger++; }
	inline void inc_n_control_indications()     { this->m_n_control_indications++;     }
	inline void inc_n_global_time()             { this->m_n_global_time++;             }
	inline void inc_n_event_flagged_zero()      { this->m_n_event_flagged_zero++;      }
	inline void inc_n_event_flagged_two()       { this->m_n_event_flagged_two++;       }
	inline void reset_n_events()                { this->m_n_events = 0;                }

	inline auto  n_events() const { return this->m_n_events; }

	void show(std::ostream& strm) const;
    };

    std::ostream& operator<<(std::ostream& strm, const timepix::data_model::EventStatistics& ev_stat);

};


#endif // TIMEPIX_EVENT_STATISTICS_H
