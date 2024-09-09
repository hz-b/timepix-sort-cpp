#include <timepix_sort/data_model.h>
#include <timepix_sort/data_model_chunks.h>

namespace dm = timepix::data_model;

dm::ChunkView dm::ChunkCollection::at_dbg(const size_t index) const {
    return this->get_checked(index);
}
