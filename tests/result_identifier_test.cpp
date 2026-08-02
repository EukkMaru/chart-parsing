#include "chart/reconstruction.hpp"

#include <cassert>
#include <cstdint>
#include <limits>

int main() {
    using chart::reconstruction::C2sResultComponentIdentifiers;
    using chart::reconstruction::allocate_c2s_root_result_identifiers;
    using chart::reconstruction::attach_c2s_secondary_result_identifier;
    using chart::reconstruction::c2s_root_result_identifier_count;
    using chart::reconstruction::select_note_result_identifier;

    assert(c2s_root_result_identifier_count(0) == 1);
    assert(c2s_root_result_identifier_count(4) == 1);
    assert(c2s_root_result_identifier_count(6) == 1);
    assert(c2s_root_result_identifier_count(11) == 1);
    assert(c2s_root_result_identifier_count(1) == 2);
    assert(c2s_root_result_identifier_count(2) == 2);
    assert(c2s_root_result_identifier_count(9) == 2);
    assert(c2s_root_result_identifier_count(10) == 2);
    assert(c2s_root_result_identifier_count(12) == 2);
    assert(c2s_root_result_identifier_count(13) == 2);
    assert(c2s_root_result_identifier_count(3) == 0);
    assert(c2s_root_result_identifier_count(5) == 0);
    assert(c2s_root_result_identifier_count(8) == 0);

    std::int32_t next = 0;
    const auto tap = allocate_c2s_root_result_identifiers(0, next);
    assert(tap.primary == 0 && tap.middle == -1 && tap.secondary == -1);
    assert(next == 1);

    auto hold = allocate_c2s_root_result_identifiers(1, next);
    assert(hold.primary == 1 && hold.middle == 2 && hold.secondary == -1);
    assert(next == 3);

    attach_c2s_secondary_result_identifier(hold, next);
    assert(hold.secondary == 3 && next == 4);
    attach_c2s_secondary_result_identifier(hold, next);
    assert(hold.secondary == 3 && next == 4);

    assert(select_note_result_identifier(hold, 0) == 1);
    assert(select_note_result_identifier(hold, 1) == 1);
    for (std::int32_t category = 2; category <= 6; ++category) {
        assert(select_note_result_identifier(hold, category) == 2);
    }
    for (std::int32_t category = 7; category <= 13; ++category) {
        assert(select_note_result_identifier(hold, category) == 3);
    }
    assert(select_note_result_identifier(hold, -1) == 1);
    assert(select_note_result_identifier(hold, 14) == 1);

    std::int32_t wrapped_next =
        std::numeric_limits<std::int32_t>::max();
    const C2sResultComponentIdentifiers wrapped =
        allocate_c2s_root_result_identifiers(1, wrapped_next);
    assert(wrapped.primary == std::numeric_limits<std::int32_t>::max());
    assert(wrapped.middle == std::numeric_limits<std::int32_t>::min());
    assert(wrapped_next ==
           std::numeric_limits<std::int32_t>::min() + 1);
}
