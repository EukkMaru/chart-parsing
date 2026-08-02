#include "chart/reconstruction.hpp"

#include <cassert>
#include <limits>
#include <string_view>

int main() {
    using chart::reconstruction::C2sHoldCommandForm;
    using chart::reconstruction::c2s_hxd_subtype_code;
    using chart::reconstruction::c2s_hxd_subtype_names;
    using chart::reconstruction::note_checker_profile_selector;
    using chart::reconstruction::parse_c2s_hold_command_variant;
    using chart::reconstruction::parse_c2s_hold_geometry;

    for (std::size_t index = 0; index < c2s_hxd_subtype_names.size();
         ++index) {
        assert(c2s_hxd_subtype_code(c2s_hxd_subtype_names[index]) ==
               static_cast<std::int32_t>(index));
    }
    assert(c2s_hxd_subtype_code("") == 0);
    assert(c2s_hxd_subtype_code("UNKNOWN") == 0);
    assert(c2s_hxd_subtype_code("up") == 0);

    const std::string_view complete[]{
        "1", "0", "3", "4", "96", "BS", "ignored"};
    const auto hxd =
        parse_c2s_hold_command_variant(C2sHoldCommandForm::hxd, complete);
    assert(hxd.extended_form);
    assert(hxd.subtype_code == 7);

    // Backward-compatible five-field HXD records still set the extended flag
    // and default their missing subtype to index zero.
    const std::string_view legacy[]{"1", "0", "3", "4", "96"};
    const auto legacy_hxd =
        parse_c2s_hold_command_variant(C2sHoldCommandForm::hxd, legacy);
    assert(legacy_hxd.extended_form);
    assert(legacy_hxd.subtype_code == 0);

    const auto hld =
        parse_c2s_hold_command_variant(C2sHoldCommandForm::hld, complete);
    assert(!hld.extended_form);
    assert(hld.subtype_code == 0);

    const auto geometry = parse_c2s_hold_geometry(complete);
    assert(geometry.start.major == 1.0F);
    assert(geometry.start.minor == 0.0F);
    assert(geometry.end.major == 1.0F);
    assert(geometry.end.minor == 1.0F);
    assert(geometry.lane == 3);
    assert(geometry.width == 4);
    assert(geometry.encoded_width == 3);
    assert(geometry.duration == 96);

    const auto mirrored = parse_c2s_hold_geometry(complete, true);
    assert(mirrored.lane == 9);

    const std::string_view wrapped_fields[]{
        "0", "2147483647", "-2147483648", "16", "1"};
    const auto wrapped = parse_c2s_hold_geometry(wrapped_fields, true);
    assert(wrapped.lane == std::numeric_limits<std::int32_t>::min());
    const auto expected_wrapped_end =
        chart::reconstruction::canonicalize_c2s_position(
            0, std::numeric_limits<std::int32_t>::min());
    assert(wrapped.end.major == expected_wrapped_end.major);
    assert(wrapped.end.minor == expected_wrapped_end.minor);

    const std::string_view missing[]{"2"};
    const auto defaulted = parse_c2s_hold_geometry(missing);
    assert(defaulted.start.major == 2.0F);
    assert(defaulted.end.major == 2.0F);
    assert(defaulted.lane == 0);
    assert(defaulted.width == 1);
    assert(defaulted.duration == 0);

    assert(note_checker_profile_selector(1, false) == 0);
    assert(note_checker_profile_selector(1, true) == 4);
    assert(note_checker_profile_selector(2, true) == 4);
    assert(note_checker_profile_selector(13, true) == 4);
    assert(note_checker_profile_selector(4, false) == 4);
    assert(note_checker_profile_selector(6, false) == 6);
    assert(note_checker_profile_selector(11, false) == 11);
    assert(note_checker_profile_selector(0, true) == 0);
    assert(note_checker_profile_selector(-1, true) == -1);
    assert(note_checker_profile_selector(14, true) == -1);
}
