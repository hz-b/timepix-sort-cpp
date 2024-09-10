#define BOOST_TEST_MODULE gtpsa_arithmetic
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>
#include <string>
#include <timepix_sort/read.h>
#include <timepix_sort/process.h>
#include <timepix_sort/data_model.h>

namespace dm = timepix::data_model;
namespace tps = timepix::sort;

static const double fs2s = 1e-15;

///> todo: where does that need to go to?
static const int falling_edge = 0x6f;


static const auto reference_buffer()
{
    const std::string test_filename = "test_data/Co_pos_0000.tpx3";
    return timepix::sort::detail::read_raw(test_filename);
}

// test that the correct number of chunks is read
// test the first and last one for the correct length
// data used retrieved by manufacturer document
BOOST_AUTO_TEST_CASE(test10_read_chunks)
{
    const auto r = timepix::sort::read_chunks(reference_buffer());
    const auto col_end = r.at(r.size() - 1);


    BOOST_CHECK(r.size() == 178693);

    // first chunk
    {
        const auto chunk = r.at(0);
        BOOST_CHECK_EQUAL(chunk.n_events(),  1);
        BOOST_CHECK_EQUAL(chunk.chip_nr(),   3);
        BOOST_CHECK_EQUAL(chunk.offset(),    0);
    }

    // last chunk
    {
        const auto chunk = r.at(r.size() - 1);
        const uint64_t offset = 21338936;

        const uint64_t used_offset = offset / 8 - 1;

        BOOST_CHECK_EQUAL(offset  %  8, 0);

        BOOST_CHECK_EQUAL(chunk.n_events(),  1            );
        BOOST_CHECK_EQUAL(chunk.chip_nr(),   2            );
        BOOST_CHECK_EQUAL(chunk.offset(),    used_offset  );
    }

    // second chunk
    {
        const auto chunk = r.at(1);
        const uint64_t offset = 24;

        const uint64_t used_offset = offset / 8 - 1;

        BOOST_CHECK_EQUAL(offset  %  8, 0);

        BOOST_CHECK_EQUAL(chunk.n_events(),  1            );
        BOOST_CHECK_EQUAL(chunk.chip_nr(),   2            );
        BOOST_CHECK_EQUAL(chunk.offset(),    used_offset  );
    }

    // first large chunk
    {
        const auto chunk = r.at(132 - 1);
        const uint64_t offset = 2368;

        const uint64_t used_offset = offset / 8 - 1;

        BOOST_CHECK_EQUAL(offset  %  8, 0);

        BOOST_CHECK_EQUAL(chunk.n_events(),  38           );
        BOOST_CHECK_EQUAL(chunk.chip_nr(),   1            );
        BOOST_CHECK_EQUAL(chunk.offset(),    used_offset  );
    }

    // one really large chunk
    {
        const auto chunk = r.at(272 - 1);
        const uint64_t offset = 11600;

        const uint64_t used_offset = offset / 8 - 1;

        BOOST_CHECK_EQUAL(offset  %  8, 0);

        BOOST_CHECK_EQUAL(chunk.n_events(),  1119         );
        BOOST_CHECK_EQUAL(chunk.chip_nr(),   3            );
        BOOST_CHECK_EQUAL(chunk.offset(),    used_offset  );
    }

    // really large chunk
    {
        const auto chunk = r.at(4226 - 1);
        const uint64_t offset = 482200;

        const uint64_t used_offset = offset / 8 - 1;

        BOOST_CHECK_EQUAL(offset  %  8, 0);

        BOOST_CHECK_EQUAL(chunk.n_events(),  1119         );
        BOOST_CHECK_EQUAL(chunk.chip_nr(),   3            );
        BOOST_CHECK_EQUAL(chunk.offset(),    used_offset  );
    }
    // chunk after large chunk
    {
        const auto chunk = r.at(4227 - 1);
        const uint64_t offset = 491160;

        const uint64_t used_offset = offset / 8 - 1;

        BOOST_CHECK_EQUAL(offset  %  8, 0);

        BOOST_CHECK_EQUAL(chunk.n_events(),  1            );
        BOOST_CHECK_EQUAL(chunk.chip_nr(),   2            );
        BOOST_CHECK_EQUAL(chunk.offset(),    used_offset  );
    }

    // last large chunk
    {
        const auto chunk = r.at(178686 - 1);
        const uint64_t offset = 21330880;
        const uint64_t used_offset = offset / 8 - 1;

        BOOST_CHECK_EQUAL(offset  %  8, 0);

        BOOST_CHECK_EQUAL(chunk.n_events(),  982          );
        BOOST_CHECK_EQUAL(chunk.chip_nr(),   3            );
        BOOST_CHECK_EQUAL(chunk.offset(),    used_offset  );
    }

}


// test that the correct number of chunks is read
// test the first and last one for the correct length
// data used retrieved by manufacturer document
BOOST_AUTO_TEST_CASE(test20_read_events_raw)
{


    const auto chunks = timepix::sort::read_chunks(reference_buffer());

    // first chunk with pixels
    {
        const auto& view = chunks.get(129 - 1);

        dm::EventStatistics ev_stat;
        const auto chip_nr = view.chip_nr();
        const auto& raw_events = view.events();

        BOOST_CHECK_EQUAL(raw_events.size(), 1);
        BOOST_CHECK_EQUAL(chip_nr, 2);

        const auto event = tps::process_raw_event(
            raw_events.at(0),
            view.chip_nr(),
            falling_edge,
            0,
            ev_stat);

        BOOST_CHECK(event.is_pixel_event());

        const auto pixel = event.as_pixel_event();
        BOOST_CHECK_EQUAL(pixel.x(), 84);
        BOOST_CHECK_EQUAL(pixel.y(), 55);
        BOOST_CHECK_EQUAL(pixel.chip_nr(), chip_nr);
        BOOST_CHECK_CLOSE(pixel.time_of_arrival() * fs2s, 19.856125334375, 1e-12);

    }


    // second chunk with pixels
    {
        const auto& view = chunks.get(130 - 1);

        dm::EventStatistics ev_stat;
        const auto chip_nr = view.chip_nr();
        const auto& raw_events = view.events();

        BOOST_CHECK_EQUAL(raw_events.size(), 1);

        const auto event = tps::process_raw_event(
            raw_events.at(0),
            view.chip_nr(),
            falling_edge,
            0,
            ev_stat);

        BOOST_CHECK(event.is_pixel_event());

        const auto pixel = event.as_pixel_event();
        BOOST_CHECK_EQUAL(pixel.x(), 54);
        BOOST_CHECK_EQUAL(pixel.y(), 236);
        BOOST_CHECK_EQUAL(pixel.chip_nr(), chip_nr);
        BOOST_CHECK_CLOSE(pixel.time_of_arrival() * fs2s, 19.8561399671875, 1e-12);

    }

    // third chunk: 2  pixels
    {
        const auto& view = chunks.get(131 - 1);

        dm::EventStatistics ev_stat;
        const auto chip_nr = view.chip_nr();
        const auto& raw_events = view.events();

        BOOST_CHECK_EQUAL(raw_events.size(), 2);

        {
            const auto event = tps::process_raw_event(
                raw_events.at(0),
                view.chip_nr(),
                falling_edge,
                0,
                ev_stat);

            BOOST_CHECK(event.is_pixel_event());

            const auto pixel = event.as_pixel_event();
            BOOST_CHECK_EQUAL(pixel.x(), 27);
            BOOST_CHECK_EQUAL(pixel.y(), 146);
            BOOST_CHECK_EQUAL(pixel.chip_nr(), chip_nr);
            BOOST_CHECK_CLOSE(pixel.time_of_arrival() * fs2s, 19.8561518125, 1e-12);
        }

        {
            const auto event = tps::process_raw_event(
                raw_events.at(1),
                view.chip_nr(),
                falling_edge,
                0,
                ev_stat);

            BOOST_CHECK(event.is_pixel_event());

            const auto pixel = event.as_pixel_event();
            BOOST_CHECK_EQUAL(pixel.x(), 30);
            BOOST_CHECK_EQUAL(pixel.y(), 209);
            BOOST_CHECK_EQUAL(pixel.chip_nr(), chip_nr);
            BOOST_CHECK_CLOSE(pixel.time_of_arrival() * fs2s, 19.8561529265625, 1e-12);
        }
    }

    // a late one containing both
    {
        //21338712         178686  978     trigger             74.5432161914062
        const auto& view = chunks.get(178686 - 1);

        dm::EventStatistics ev_stat;
        const auto chip_nr = view.chip_nr();
        const auto& raw_events = view.events();

        BOOST_CHECK_EQUAL(raw_events.size(), 982);

        {
            const auto event = tps::process_raw_event(
                raw_events.at(978),
                view.chip_nr(),
                falling_edge,
                0,
                ev_stat
                );

            BOOST_CHECK(event.is_trigger_event());

            const auto trigger = event.as_time_of_flight_event();
            const double ref_time = 74.5432161914062;
            BOOST_CHECK_CLOSE(trigger.time_of_arrival() * fs2s, ref_time, 1e-12);
            BOOST_CHECK_CLOSE(event.time_of_arrival() * fs2s, ref_time, 1e-12);

        }

        {
            const auto event = tps::process_raw_event(
                raw_events.at(979),
                view.chip_nr(),
                falling_edge,
                0,
                ev_stat
                );

            BOOST_CHECK(event.is_pixel_event());

            const auto trigger = event.as_pixel_event();
            const double ref_time = 20.8561234265625;
            BOOST_CHECK_CLOSE(trigger.time_of_arrival() * fs2s, ref_time, 1e-12);
            BOOST_CHECK_CLOSE(event.time_of_arrival() * fs2s, ref_time, 1e-12);
        }

    }
}


BOOST_AUTO_TEST_CASE(test30_read_events)
{
    const auto chunks = timepix::sort::read_chunks(reference_buffer());
    // include all events
    auto tmp =  timepix::sort::process(chunks, falling_edge, 0);
    const auto events = std::get<0>(tmp);

    {
        const int chip_nr = 2;
        const auto event = events.at(0);
        BOOST_CHECK(event.is_pixel_event());

        const auto pixel = event.as_pixel_event();
        BOOST_CHECK_EQUAL(pixel.x(), 84);
        BOOST_CHECK_EQUAL(pixel.y(), 55);
        BOOST_CHECK_EQUAL(pixel.chip_nr(), chip_nr);
        BOOST_CHECK_CLOSE(pixel.time_of_arrival() * fs2s, 19.856125334375, 1e-12);
    }

    {
        const int chip_nr = 0;
        const auto event = events.at(1);
        BOOST_CHECK(event.is_pixel_event());

        const auto pixel = event.as_pixel_event();
        BOOST_CHECK_EQUAL(pixel.x(), 54);
        BOOST_CHECK_EQUAL(pixel.y(), 236);
        BOOST_CHECK_EQUAL(int(pixel.chip_nr()), chip_nr);
        BOOST_CHECK_CLOSE(pixel.time_of_arrival() * fs2s, 19.8561399671875, 1e-12);
    }

    {
        const int chip_nr = 2;
        const auto event = events.at(2);
        BOOST_CHECK(event.is_pixel_event());

        const auto pixel = event.as_pixel_event();
        BOOST_CHECK_EQUAL(pixel.x(), 27);
        BOOST_CHECK_EQUAL(pixel.y(), 146);
        BOOST_CHECK_EQUAL(int(pixel.chip_nr()), chip_nr);
        BOOST_CHECK_CLOSE(pixel.time_of_arrival() * fs2s, 19.8561518125, 1e-12);
    }
    {
        const int chip_nr = 2;
        const auto event = events.at(3);
        BOOST_CHECK(event.is_pixel_event());

        const auto pixel = event.as_pixel_event();
        BOOST_CHECK_EQUAL(pixel.x(), 30);
        BOOST_CHECK_EQUAL(pixel.y(), 209);
        BOOST_CHECK_EQUAL(int(pixel.chip_nr()), chip_nr);
        BOOST_CHECK_CLOSE(pixel.time_of_arrival() * fs2s, 19.8561529265625, 1e-12);
    }

}
