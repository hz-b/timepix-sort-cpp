/**
 * test boost histogramm on features expected for
 * timepix
 */
#include <boost/histogram.hpp>

int main(int argc, char *argv[])
{
    namespace bs = boost::histogram;
    auto time_axis = bs::axis::regular<>{20000, 0, 1.5};
    auto x_pix_axis = bs::axis::integer<>{-255, 255};
    auto y_pix_axis = bs::axis::integer<>{-255, 255};

    auto h = boost::histogram::make_histogram(x_pix_axis, y_pix_axis, time_axis);
    return 0;
}
