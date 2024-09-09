#include <timepix_sort/event_statistics.h>

namespace dm = timepix::data_model;

void dm::EventStatistics::show(std::ostream& strm) const
{
    uint64_t n_total_matched =
	this->m_n_pixels +
	this->m_n_timestamps +
	this->m_n_control_indications +
	this->m_n_global_time +
	this->m_n_event_flagged_zero +
	this->m_n_event_flagged_two
	;

    strm<< "EventStatistic("
	<< "n_pixels=" << this->m_n_pixels
	<< ", n_timestamps=" << this->m_n_timestamps
	<< ", n_control_indications=" << this->m_n_control_indications
	<< ", n_global_time=" << this->m_n_global_time
	<< ", n_unmatched_events=" << int64_t(this->m_n_events) - int64_t(n_total_matched)
	<< ")"
	;
}


std::ostream& dm::operator<<(std::ostream& strm, const dm::EventStatistics& ev_stat)
{
    ev_stat.show(strm);
    return strm;
}
