#ifndef TIMEPIX_DETAIL_PROCESS_CHUNKS_H
#define TIMEPIX_DETAIL_PROCESS_CHUNKS_H

#include <timepix_sort/data_model.h>
#include <timepix_sort/hardware/tpx3.h>

namespace timepix::sort::detail {


    /* todo: calculate all in fs */
    const uint64_t nanoseconds2femtoseconds = uint64_t(1000* 1000);
    const uint64_t seconds2nanoseconds = uint64_t(1000 * 1000 * 1000);
    const uint64_t seconds2femtoseconds =  nanoseconds2femtoseconds * seconds2nanoseconds;
    const double time_unit = 25. / 4096;


    /**
     *
     * assuming time arrives in correct type: expecting femto seconds
     *
     * warning: currently not assuming that we are
     */
    static inline auto convert_time(const uint64_t time_stamp, const uint64_t max_time, const double eps = 1)
    {
	if(time_stamp < eps){
	    throw std::range_error("timestamp too small!");
	}
	if(time_stamp > max_time + eps){
	    throw std::range_error("timestamp larger than max_time");
	}
	return time_stamp;
    }

    /* time stamp in fs */
    static inline auto tdc_time_stamp(const uint64_t datum)
    {

	double coarsetime = (datum >> 12) & 0xFFFFFFFF;
	unsigned long tmpfine = (datum >> 5) & 0xF;
	tmpfine = ((tmpfine - 1) << 9) / 12;
	unsigned long trigtime_fine = (datum & 0x0000000000000E00) | (tmpfine & 0x00000000000001FF);
	double TDC_timestamp = coarsetime * 25.0 + trigtime_fine * time_unit*1.0;

	using timepix::data_model::TimeOfFlightEvent;
	return TimeOfFlightEvent(
	    convert_time(
		TDC_timestamp * nanoseconds2femtoseconds,
		timepix::hardware::tpx3::tdc_max_time_fs
		)
	    );
    }

    static inline auto unfold_pixel_event(const uint64_t datum, const int8_t chip_nr){

	unsigned short spidrTime = (unsigned short)(datum & 0xffff);
	long  dcol = (datum & 0x0FE0000000000000L) >> 52;
	long  spix = (datum & 0x001F800000000000L) >> 45;
	long  pix = (datum & 0x0000700000000000L) >> 44;
	int x = (int)(dcol + pix / 4);
	int y = (int)(spix + (pix & 0x3));
	unsigned short TOA = (unsigned short)((datum >> (16 + 14)) & 0x3fff);
	unsigned short TOT = (unsigned short)((datum >> (16 + 4)) & 0x3ff);
	///> todo: check cast from unsigned char to char
	char FTOA = (unsigned char)((datum >> 16) & 0xf);
	int CTOA = (TOA << 4) | (~FTOA & 0xf);
	double spidrTimens = spidrTime * 25.0 * 16384.0;
	double            TOAns = TOA * 25.0;
	double TOTns = TOT * 25.0;
	double global_timestamp = spidrTimens + CTOA * (25.0 / 16.0);

	return timepix::data_model::PixelEvent(
	    timepix::data_model::PixelPos(x, y),
	    convert_time(
		global_timestamp * nanoseconds2femtoseconds,
		timepix::hardware::tpx3::pixel_max_time_fs),
	    TOT,
	    chip_nr
	    );

//    if TOT > TOT_check and centerpixel != 0:
//        # Remove events with lower TOT to improve time resolution
//        # ignore  the switch for start
//        if chip_nr == 0:
//            xx = 255 - xx
//            yy = 255 - yy + 258
//        elif chip_nr == 1:
//            xx = xx
//            yy = yy
//        elif chip_nr == 2:
//            xx = xx + 258
//            yy = yy
//        elif chip_nr == 3:
//            xx = 255 - xx + 258
//            yy = 255 - yy + 258
//        else:
//            raise AssertionError(f"unknown chip number {chip_nr}")
    }
};
#endif /* TIMEPIX_DETAIL_PROCESS_CHUNKS_H */
