#ifndef TIMEPIX_DETAIL_PROCESS_CHUNKS_H
#define TIMEPIX_DETAIL_PROCESS_CHUNKS_H

#include <timepix_sort/data_model.h>

namespace timepix::sort::detail {

    /* todo: calculate all in fs */
    const uint64_t nanoseconds2femtoseconds = uint64_t(1000* 1000);
    const uint64_t seconds2femtoseconds =  nanoseconds2femtoseconds * uint64_t(1000 * 1000 * 1000);

    /* time stamp in fs */
    static inline auto tdc_time_stamp(const uint64_t datum)
    {

	double time_of_arrival;
	uint64_t coarsetime = (datum >> 9) & 0x1FFFFFFFF;
	double coarsetimef = coarsetime * (1 / 320e6);
	uint64_t tmpfine = (datum >> 5) & 0xF;

	if(tmpfine > 0){
	    time_of_arrival = coarsetimef + (tmpfine - 1) * 260e-12;
	} else {
	    throw std::runtime_error("TDC timestamp unknown!");
	}

	using timepix::data_model::TimeOfFlightEvent;
	return TimeOfFlightEvent(uint64_t(time_of_arrival * seconds2femtoseconds));
    }

    static inline auto unfold_pixel_event(const uint64_t pkg, const int8_t chip_nr){

	uint64_t spidrTime = pkg & 0xFFFF;
	uint64_t dcol = (pkg & 0x0FE0000000000000) >> 52;  //# (pkg >> 52) & 0xfe;
	uint64_t spix = (pkg & 0x001F800000000000) >> 45;  //# (pkg >> 45) & 0x1f8; //
	uint64_t pix = (pkg & 0x0000700000000000) >> 44;  //# (pkg >> 44) & 0x7 //

	int xx = dcol + pix / 4;
	int yy = spix + (pix & 0x3);
// time of arrival
	uint64_t TOA = (pkg >> (16 + 14)) & 0x3FFF;
// time over threshold
	uint64_t TOT = (pkg >> (16 + 4)) & 0x3FF;
	// fine time of arrival
	uint64_t FTOA = (pkg >> 16) & 0xF;
	// coarse time of arrival ?
	uint64_t CTOA = (TOA << 4) | (~FTOA & 0xF);
	// time over treshshold

	if (xx > 193 && xx < 204) {  // "Tram line" correction
		CTOA -= 8;
	} else {
	    CTOA += 8;
	}

	/*
    if xx in [0, 255] or yy in [0, 255]:
        # mark every 3rd count at edge to equalize intensity
        centerpixel = index % 3
    else:
        centerpixel = 1
	*/

	spidrTime = spidrTime * 25 * 16384;

       // todo: float or int division ?
	// uint64_t TOA_s = spidrTime + CTOA * (25.0 / 16.0);
	double TOA_s = spidrTime + CTOA * (25.0 / 16.0);
#if 0
	if (chip_nr == 3) {
// # correct for chip dependent TOT shift
	    TOT_check = TOT_min - 1;
	} else {
	    TOT_check = TOT_min;
	}

#endif
	return timepix::data_model::PixelEvent(
	    timepix::data_model::PixelPos(xx,yy),
	    uint64_t(TOA_s * nanoseconds2femtoseconds),
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
