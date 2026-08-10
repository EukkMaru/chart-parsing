#include "chart/reconstruction.hpp"

#include <cassert>
#include <stdexcept>
#include <string_view>

int main() {
    using chart::reconstruction::C2sDerivedCommandStorage;
    using chart::reconstruction::apply_c2s_derived_command;
    using chart::reconstruction::begin_c2s_derived_summary_rebuild;
    using chart::reconstruction::c2s_command_descriptor_index;
    using chart::reconstruction::c2s_command_descriptor_names;
    using chart::reconstruction::c2s_command_is_registered;
    using chart::reconstruction::c2s_derived_command_index;
    using chart::reconstruction::c2s_derived_command_names;

    static_assert(c2s_derived_command_names.size() == 45);
    static_assert(c2s_command_descriptor_names.size() == 91);
    assert(c2s_command_descriptor_index("VERSION") == 0x00);
    assert(c2s_command_descriptor_index("BPM") == 0x0d);
    assert(c2s_command_descriptor_index("SLP") == 0x12);
    assert(c2s_command_descriptor_index("AHX") == 0x26);
    assert(c2s_command_descriptor_index("HHX") == 0x2d);
    assert(c2s_command_descriptor_index("T_REC_TAP") == 0x2e);
    assert(c2s_command_descriptor_index("T_JUDGE_ALL") == 0x5a);
    assert(c2s_command_is_registered("SFE"));
    assert(c2s_command_is_registered("ASO"));
    assert(!c2s_command_is_registered("T_FIRST_MSEC"));
    assert(!c2s_command_is_registered("T_PROG_00"));
    assert(!c2s_command_is_registered("T_NOT_A_REAL_COMMAND"));
    assert(!c2s_command_is_registered("slp"));
    assert(c2s_derived_command_index("T_REC_TAP") == 0);
    assert(c2s_derived_command_index("T_JUDGE_ALL") == 44);
    assert(c2s_derived_command_index("T_NUM_AAC") == 26);
    assert(c2s_derived_command_index("T_NUM_ALL") ==
           c2s_derived_command_names.size());

    C2sDerivedCommandStorage storage;
    const std::string_view first[]{"17suffix", "ignored"};
    assert(apply_c2s_derived_command(storage, "T_REC_TAP", first));
    assert(storage.authored_values[0] == 17);

    const std::string_view duplicate[]{"23"};
    assert(apply_c2s_derived_command(storage, "T_REC_TAP", duplicate));
    assert(storage.authored_values[0] == 23);

    const std::span<const std::string_view> missing;
    assert(apply_c2s_derived_command(
        storage, "T_JUDGE_ALL", missing));
    assert(storage.authored_values[44] == 0);
    assert(!apply_c2s_derived_command(storage, "T_PROG_00", first));
    assert(!apply_c2s_derived_command(storage, "T_FIRST_MSEC", first));

    bool malformed_threw = false;
    try {
        const std::string_view malformed[]{"bad"};
        (void)apply_c2s_derived_command(
            storage, "T_NOTE_HLD", malformed);
    } catch (const std::invalid_argument&) {
        malformed_threw = true;
    }
    assert(malformed_threw);

    begin_c2s_derived_summary_rebuild(storage);
    for (const auto value : storage.authored_values) {
        assert(value == 0);
    }
}
