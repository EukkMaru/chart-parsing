#include "chart/reconstruction.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <string_view>

namespace {

bool near(float left, float right, float tolerance = 0.000001F) {
    return std::fabs(left - right) <= tolerance;
}

}  // namespace

int main() {
    using chart::reconstruction::C2sHeader;
    using chart::reconstruction::apply_c2s_header_record;
    using chart::reconstruction::c2s_command_is_ignored_legacy_progress;
    using chart::reconstruction::c2s_header_only_line_limit;
    using chart::reconstruction::ignored_legacy_progress_commands;

    C2sHeader header;
    assert(c2s_header_only_line_limit == 20);
    for (const auto command : ignored_legacy_progress_commands) {
        assert(c2s_command_is_ignored_legacy_progress(command));
    }
    assert(!c2s_command_is_ignored_legacy_progress("T_PROG_01"));
    assert(!c2s_command_is_ignored_legacy_progress("T_PROG_100"));
    assert(!c2s_command_is_ignored_legacy_progress("t_prog_00"));
    assert(!c2s_command_is_ignored_legacy_progress("T_JUDGE_ALL"));
    assert(header.music == 0);
    assert(header.sequence_id == 0);
    assert(header.difficulty.empty());
    assert(header.level == 0.0F);
    assert(header.creator.empty());
    assert((header.bpm_def == std::array<float, 4>{150.0F, 150.0F, 150.0F,
                                                   150.0F}));
    assert((header.met_def == std::array<std::int32_t, 2>{4, 4}));
    assert(header.resolution == 384);
    assert(header.clk_def == 0);
    assert(header.progjudge_bpm == 240.0F);
    assert(near(header.progjudge_aer, 0.9990000128746033F));
    assert(!header.tutorial);

    const std::string_view version_fields[]{"1.2.3suffix", "1.2"};
    assert(apply_c2s_header_record(header, "VERSION", version_fields));
    assert(header.version[0].raw == "1.2.3suffix");
    assert(header.version[0].major == 1);
    assert(header.version[0].minor == 2);
    assert(header.version[0].patch == 3);
    assert(header.version[1].raw == "1.2");
    assert(header.version[1].major == 0);
    assert(header.version[1].minor == 0);
    assert(header.version[1].patch == 0);

    const std::string_view first_music[]{"10trailing"};
    const std::string_view second_music[]{"25", "ignored"};
    assert(apply_c2s_header_record(header, "MUSIC", first_music));
    assert(apply_c2s_header_record(header, "MUSIC", second_music));
    assert(header.music == 25);  // Later duplicate wins; extras are ignored.

    const std::string_view level[]{"3.5suffix"};
    assert(apply_c2s_header_record(header, "LEVEL", level));
    assert(header.level == 3.5F);

    const std::string_view bpm_def[]{"120", "180", "90", "240"};
    assert(apply_c2s_header_record(header, "BPM_DEF", bpm_def));
    assert((header.bpm_def == std::array<float, 4>{120.0F, 180.0F, 90.0F,
                                                   240.0F}));

    const std::string_view resolution[]{"192"};
    assert(!apply_c2s_header_record(header, "RESOLUTION", resolution));
    assert(header.resolution == 384);

    const std::span<const std::string_view> missing;
    assert(apply_c2s_header_record(header, "MET_DEF", missing));
    assert((header.met_def == std::array<std::int32_t, 2>{0, 0}));
    assert(apply_c2s_header_record(header, "TUTORIAL", missing));
    assert(!header.tutorial);
    const std::string_view negative_tutorial[]{"-1"};
    assert(apply_c2s_header_record(header, "TUTORIAL", negative_tutorial));
    assert(!header.tutorial);
    const std::string_view positive_tutorial[]{"1"};
    assert(apply_c2s_header_record(header, "TUTORIAL", positive_tutorial));
    assert(header.tutorial);

    bool invalid_threw = false;
    try {
        const std::string_view invalid[]{"not-a-number"};
        apply_c2s_header_record(header, "CLK_DEF", invalid);
    } catch (const std::invalid_argument&) {
        invalid_threw = true;
    }
    assert(invalid_threw);

    bool range_threw = false;
    try {
        const std::string_view out_of_range[]{"999999999999999999999999"};
        apply_c2s_header_record(header, "SEQUENCEID", out_of_range);
    } catch (const std::out_of_range&) {
        range_threw = true;
    }
    assert(range_threw);

    assert(!apply_c2s_header_record(header, "UNKNOWN_HEADER", missing));
}
