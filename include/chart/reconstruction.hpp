#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cerrno>
#include <cstddef>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <map>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

// Clean-room gameplay reconstruction namespace.
//
// Do not add behavior here until it has a reconstructed claim and specification.
// Keep unknown external constants explicit parameters rather than guessed values.
namespace chart::reconstruction {

// claim.pipeline.source-chart-path-selection
//
// Gameplay setup selects a source chart path from the current content-catalog
// record before applying the .xml-to-.c2s transformation below. The snapshot
// only considers a catalog record with at least six fixed-size selector
// entries. A requested selector outside the external selector-table count is
// clamped to the table's final entry. The snapshot does not validate a zero
// selector count or check the resulting index against the catalog vector; the
// reconstruction reports those malformed external domains instead of
// reproducing an unchecked access.
enum class SourceChartPathSelectionStatus : std::uint8_t {
    unavailable,
    selected,
    invalid_external_configuration,
};

struct SourceChartPathSelection {
    SourceChartPathSelectionStatus status{
        SourceChartPathSelectionStatus::unavailable};
    std::uint8_t selected_index{};
    std::string_view source_path{};
};

constexpr SourceChartPathSelection select_source_chart_path(
    std::span<const std::string_view> catalog_paths,
    std::uint8_t requested_selector,
    std::uint8_t external_selector_count) {
    if (catalog_paths.size() <= 5) {
        return {};
    }
    if (external_selector_count == 0) {
        return {
            SourceChartPathSelectionStatus::invalid_external_configuration};
    }

    const std::uint8_t selected =
        requested_selector < external_selector_count
            ? requested_selector
            : static_cast<std::uint8_t>(external_selector_count - 1);
    if (selected >= catalog_paths.size()) {
        return {
            SourceChartPathSelectionStatus::invalid_external_configuration,
            selected,
        };
    }
    return {
        SourceChartPathSelectionStatus::selected,
        selected,
        catalog_paths[selected],
    };
}

// claim.pipeline.c2s-path-derivation
//
// The source snapshot derives the chart path by replacing the first literal
// ".xml" substring.  It does not require the substring to be a suffix and it
// leaves a path without that exact lowercase substring unchanged.
inline std::string derive_c2s_path(std::string_view source_path) {
    std::string result(source_path);
    const auto position = result.find(".xml");
    if (position != std::string::npos) {
        result.replace(position, 4, ".c2s");
    }
    return result;
}

// claim.pipeline.runtime-note-materialization-order
//
// The gameplay-facing load entry clears the parsed chart before file checking
// and tokenization. File-check and tokenizer failures therefore leave no
// parsed records. Once tokenization succeeds, the parser orchestration is
// considered successful even if individual record handlers reject records;
// accepted_record_count is the surviving count after those skips. Scene setup
// does not construct those records immediately: it queues every surviving
// parsed-record index for later eligibility checks.
enum class C2sInputOutcome : std::uint8_t {
    file_precheck_failed,
    tokenization_failed,
    tokenized,
};

struct GameplayChartLoadAttempt {
    bool load_succeeded{};
    bool parser_orchestration_ran{};
    bool success_only_configuration_ran{};
    bool setup_continues{true};
    std::size_t parsed_record_count{};
    std::size_t materialization_queue_count{};
    std::size_t initial_runtime_note_count{};
};

constexpr GameplayChartLoadAttempt reconstruct_gameplay_chart_load_attempt(
    C2sInputOutcome input_outcome,
    std::size_t accepted_record_count) {
    if (input_outcome != C2sInputOutcome::tokenized) {
        return {};
    }
    return {
        true,
        true,
        true,
        true,
        accepted_record_count,
        accepted_record_count,
        0,
    };
}

// claim.pipeline.note-view-preload-barrier
//
// Before the loading scene can advance, NotesManager constructs one instance
// of each RTTI-identified projView note presentation class in this order. The
// latest object advances an integer resource step until both its own fixed
// completion step and the manager's step-10 floor have been reached. Previous
// objects have already latched ready, so the latest object controls progress.
enum class NoteViewPreloadClass : std::uint8_t {
    tap,
    chara_tap,
    hold,
    slide,
    air,
    air_hold,
    air_slide,
    air_ladder,
    air_solid,
    flick,
    mine,
};

inline constexpr std::array<NoteViewPreloadClass, 11>
    note_view_preload_order{{
        NoteViewPreloadClass::tap,
        NoteViewPreloadClass::chara_tap,
        NoteViewPreloadClass::hold,
        NoteViewPreloadClass::slide,
        NoteViewPreloadClass::air,
        NoteViewPreloadClass::air_hold,
        NoteViewPreloadClass::air_slide,
        NoteViewPreloadClass::air_ladder,
        NoteViewPreloadClass::air_solid,
        NoteViewPreloadClass::flick,
        NoteViewPreloadClass::mine,
    }};

// First step values for which each concrete readiness virtual returns true.
inline constexpr std::array<std::uint32_t, 11>
    note_view_first_ready_steps{{8, 10, 12, 20, 12, 15, 13, 11, 0, 9, 29}};
inline constexpr std::uint32_t note_view_manager_step_floor = 10;

constexpr bool note_view_preload_stage_ready(
    NoteViewPreloadClass view,
    std::uint32_t resource_step) {
    const auto index = static_cast<std::size_t>(view);
    return resource_step >= note_view_manager_step_floor &&
           resource_step >= note_view_first_ready_steps[index];
}

// Manager stage 0 is idle, stage 1 constructs Tap, stages 2..12 wait on the
// most recently constructed class, and stage 13 is complete.
struct NoteViewPreloadState {
    std::uint8_t stage{};
    std::uint32_t resource_step{};
    std::uint8_t owned_object_count{};
};

enum class NoteViewPreloadAction : std::uint8_t {
    none,
    construct_first,
    finalize_and_construct_next,
    finalize_and_complete,
};

struct NoteViewPreloadStepResult {
    NoteViewPreloadAction action{NoteViewPreloadAction::none};
    bool has_constructed_class{};
    NoteViewPreloadClass constructed_class{NoteViewPreloadClass::tap};
    std::uint8_t finalized_object_count{};
    bool completed{};
};

constexpr NoteViewPreloadState start_note_view_preload() {
    return {1, 0, 0};
}

constexpr void reset_note_view_preload(NoteViewPreloadState& state) {
    state = {};
}

constexpr NoteViewPreloadStepResult advance_note_view_preload(
    NoteViewPreloadState& state) {
    NoteViewPreloadStepResult result;
    if (state.stage == 0 || state.stage == 13) {
        result.completed = true;
        return result;
    }

    if (state.stage == 1) {
        state.stage = 2;
        state.resource_step = 0;
        state.owned_object_count = 1;
        result.action = NoteViewPreloadAction::construct_first;
        result.has_constructed_class = true;
        result.constructed_class = note_view_preload_order[0];
        return result;
    }

    const std::size_t latest_index = state.stage - 2U;
    if (!note_view_preload_stage_ready(
            note_view_preload_order[latest_index], state.resource_step)) {
        ++state.resource_step;
        return result;
    }

    result.finalized_object_count = state.owned_object_count;
    state.resource_step = 0;
    if (state.stage == 12) {
        state.stage = 13;
        result.action = NoteViewPreloadAction::finalize_and_complete;
        result.completed = true;
        return result;
    }

    result.action = NoteViewPreloadAction::finalize_and_construct_next;
    result.has_constructed_class = true;
    result.constructed_class = note_view_preload_order[latest_index + 1U];
    ++state.owned_object_count;
    ++state.stage;
    return result;
}

// claim.parser.header-default-dispatch
//
// Header-only parsing reads at most the first 20 source lines. The ordinary
// gameplay parse has no line limit and runs the later BPM/event passes.
inline constexpr std::size_t c2s_header_only_line_limit = 20;

struct C2sVersion {
    std::string raw;
    std::int32_t major{};
    std::int32_t minor{};
    std::int32_t patch{};
};

inline C2sVersion parse_c2s_version(std::string_view raw) {
    C2sVersion result;
    result.raw.assign(raw);
    if (std::sscanf(result.raw.c_str(), "%d.%d.%d", &result.major,
                    &result.minor, &result.patch) != 3) {
        result.major = 0;
        result.minor = 0;
        result.patch = 0;
    }
    return result;
}

inline float parse_c2s_float_field(std::span<const std::string_view> fields,
                                   std::size_t index) {
    if (index >= fields.size() || fields[index].empty()) {
        return 0.0F;
    }

    std::string text(fields[index]);
    char* end = nullptr;
    errno = 0;
    const float value = std::strtof(text.c_str(), &end);
    if (end == text.c_str()) {
        throw std::invalid_argument("c2s float field");
    }
    if (errno == ERANGE) {
        throw std::out_of_range("c2s float field");
    }
    return value;
}

inline std::int32_t parse_c2s_integer_field(
    std::span<const std::string_view> fields,
    std::size_t index) {
    if (index >= fields.size() || fields[index].empty()) {
        return 0;
    }

    std::string text(fields[index]);
    char* end = nullptr;
    errno = 0;
    const long long value = std::strtoll(text.c_str(), &end, 10);
    if (end == text.c_str()) {
        throw std::invalid_argument("c2s integer field");
    }
    if (errno == ERANGE ||
        value < std::numeric_limits<std::int32_t>::min() ||
        value > std::numeric_limits<std::int32_t>::max()) {
        throw std::out_of_range("c2s integer field");
    }
    return static_cast<std::int32_t>(value);
}

inline std::string c2s_string_field(
    std::span<const std::string_view> fields,
    std::size_t index) {
    return index < fields.size() ? std::string(fields[index]) : std::string{};
}

// claim.parser.legacy-metadata-command-exclusion
//
// These 24 spellings occur in every local backward-compatible chart, but they
// are absent from the executable's exact 91-entry descriptor registry. The
// source line loader therefore drops them as unknown commands.
inline constexpr std::array<std::string_view, 24>
    ignored_legacy_metadata_commands{{
        "T_FIRST_MSEC", "T_FIRST_RES", "T_FINAL_MSEC", "T_FINAL_RES",
        "T_PROG_00", "T_PROG_05", "T_PROG_10", "T_PROG_15", "T_PROG_20",
        "T_PROG_25", "T_PROG_30", "T_PROG_35", "T_PROG_40", "T_PROG_45",
        "T_PROG_50", "T_PROG_55", "T_PROG_60", "T_PROG_65", "T_PROG_70",
        "T_PROG_75", "T_PROG_80", "T_PROG_85", "T_PROG_90", "T_PROG_95",
    }};

constexpr bool c2s_command_is_ignored_legacy_metadata(
    std::string_view command) {
    return std::find(ignored_legacy_metadata_commands.begin(),
                     ignored_legacy_metadata_commands.end(),
                     command) != ignored_legacy_metadata_commands.end();
}

// claim.parser.command-descriptor-schema
//
// The executable's descriptor registry is one contiguous, case-sensitive
// table. IDs are part of the parser's dispatch contract, so keep the complete
// order visible instead of accepting command-name prefixes.
inline constexpr std::array<std::string_view, 91>
    c2s_command_descriptor_names{{
        "VERSION",       "MUSIC",         "SEQUENCEID",
        "DIFFICULT",     "LEVEL",         "CREATOR",
        "BPM_DEF",       "MET_DEF",       "RESOLUTION",
        "CLK_DEF",       "PROGJUDGE_BPM", "PROGJUDGE_AER",
        "TUTORIAL",      "BPM",           "MET",
        "STP",           "SFL",           "SFE",
        "SLP",           "DCM",           "CLK",
        "TAP",           "CHR",           "FLK",
        "MNE",           "HLD",           "HXD",
        "SLD",           "SXD",           "SLC",
        "SXC",           "AIR",           "AUR",
        "AUL",           "ADW",           "ADR",
        "ADL",           "AHD",           "AHX",
        "ASD",           "ASC",           "ALD",
        "ASO",           "SLA",           "HHD",
        "HHX",           "T_REC_TAP",     "T_REC_CHR",
        "T_REC_FLK",     "T_REC_MNE",     "T_REC_HLD",
        "T_REC_SLD",     "T_REC_AIR",     "T_REC_AHD",
        "T_REC_ALL",     "T_NOTE_TAP",    "T_NOTE_CHR",
        "T_NOTE_FLK",    "T_NOTE_MNE",    "T_NOTE_HLD",
        "T_NOTE_SLD",    "T_NOTE_AIR",    "T_NOTE_AHD",
        "T_NOTE_ALL",    "T_NUM_TAP",     "T_NUM_CHR",
        "T_NUM_FLK",     "T_NUM_MNE",     "T_NUM_HLD",
        "T_NUM_SLD",     "T_NUM_AIR",     "T_NUM_AHD",
        "T_NUM_AAC",     "T_CHRTYPE_UP",  "T_CHRTYPE_DW",
        "T_CHRTYPE_CE",  "T_CHRTYPE_RC",  "T_CHRTYPE_LC",
        "T_CHRTYPE_RS",  "T_CHRTYPE_LS",  "T_CHRTYPE_BS",
        "T_LEN_HLD",     "T_LEN_SLD",     "T_LEN_AHD",
        "T_LEN_ALL",     "T_JUDGE_TAP",   "T_JUDGE_HLD",
        "T_JUDGE_SLD",   "T_JUDGE_AIR",   "T_JUDGE_FLK",
        "T_JUDGE_ALL",
    }};

constexpr std::size_t c2s_command_descriptor_index(
    std::string_view command) {
    for (std::size_t index = 0;
         index < c2s_command_descriptor_names.size(); ++index) {
        if (c2s_command_descriptor_names[index] == command) {
            return index;
        }
    }
    return c2s_command_descriptor_names.size();
}

constexpr bool c2s_command_is_registered(std::string_view command) {
    return c2s_command_descriptor_index(command) <
           c2s_command_descriptor_names.size();
}

// claim.parser.derived-command-overwrite
//
// Descriptor IDs 0x2e through 0x5a are group-3 commands with one integer
// field. The parser temporarily stores them in this exact ID order. Before the
// ordinary parser returns, its derived-summary pass clears the same 45 slots
// without reading them and rebuilds unrelated chart-derived statistics there.
inline constexpr std::array<std::string_view, 45>
    c2s_derived_command_names{{
        "T_REC_TAP",    "T_REC_CHR",    "T_REC_FLK",
        "T_REC_MNE",    "T_REC_HLD",    "T_REC_SLD",
        "T_REC_AIR",    "T_REC_AHD",    "T_REC_ALL",
        "T_NOTE_TAP",   "T_NOTE_CHR",   "T_NOTE_FLK",
        "T_NOTE_MNE",   "T_NOTE_HLD",   "T_NOTE_SLD",
        "T_NOTE_AIR",   "T_NOTE_AHD",   "T_NOTE_ALL",
        "T_NUM_TAP",    "T_NUM_CHR",    "T_NUM_FLK",
        "T_NUM_MNE",    "T_NUM_HLD",    "T_NUM_SLD",
        "T_NUM_AIR",    "T_NUM_AHD",    "T_NUM_AAC",
        "T_CHRTYPE_UP", "T_CHRTYPE_DW", "T_CHRTYPE_CE",
        "T_CHRTYPE_RC", "T_CHRTYPE_LC", "T_CHRTYPE_RS",
        "T_CHRTYPE_LS", "T_CHRTYPE_BS", "T_LEN_HLD",
        "T_LEN_SLD",    "T_LEN_AHD",    "T_LEN_ALL",
        "T_JUDGE_TAP",  "T_JUDGE_HLD",  "T_JUDGE_SLD",
        "T_JUDGE_AIR",  "T_JUDGE_FLK",  "T_JUDGE_ALL",
    }};

struct C2sDerivedCommandStorage {
    std::array<std::int32_t, c2s_derived_command_names.size()>
        authored_values{};
};

constexpr std::size_t c2s_derived_command_index(std::string_view command) {
    for (std::size_t index = 0; index < c2s_derived_command_names.size();
         ++index) {
        if (c2s_derived_command_names[index] == command) {
            return index;
        }
    }
    return c2s_derived_command_names.size();
}

inline bool apply_c2s_derived_command(
    C2sDerivedCommandStorage& storage,
    std::string_view command,
    std::span<const std::string_view> fields) {
    const std::size_t index = c2s_derived_command_index(command);
    if (index == c2s_derived_command_names.size()) {
        return false;
    }
    storage.authored_values[index] = parse_c2s_integer_field(fields, 0);
    return true;
}

// This models the destructive first action of the summary producer. The
// rebuilt values have no live gameplay consumer and are intentionally not
// represented as authored configuration.
constexpr void begin_c2s_derived_summary_rebuild(
    C2sDerivedCommandStorage& storage) {
    storage.authored_values.fill(0);
}

struct C2sHeader {
    std::array<C2sVersion, 2> version{};
    std::int32_t music{};
    std::int32_t sequence_id{};
    std::string difficulty;
    float level{};
    std::string creator;
    std::array<float, 4> bpm_def{150.0F, 150.0F, 150.0F, 150.0F};
    std::array<std::int32_t, 2> met_def{4, 4};
    std::int32_t resolution{384};
    std::int32_t clk_def{};
    float progjudge_bpm{240.0F};
    float progjudge_aer{std::bit_cast<float>(std::uint32_t{0x3f7fbe77})};
    bool tutorial{};
};

// Applies one descriptor-resolved group-0 record. Fields exclude the command
// token. A true result matches the parser's accepted-record accounting.
// Repeated commands are intentionally applied in source order, so later
// records replace earlier values.
inline bool apply_c2s_header_record(
    C2sHeader& header,
    std::string_view command,
    std::span<const std::string_view> fields) {
    if (command == "VERSION") {
        header.version[0] = parse_c2s_version(c2s_string_field(fields, 0));
        header.version[1] = parse_c2s_version(c2s_string_field(fields, 1));
    } else if (command == "MUSIC") {
        header.music = parse_c2s_integer_field(fields, 0);
    } else if (command == "SEQUENCEID") {
        header.sequence_id = parse_c2s_integer_field(fields, 0);
    } else if (command == "DIFFICULT") {
        header.difficulty = c2s_string_field(fields, 0);
    } else if (command == "LEVEL") {
        header.level = parse_c2s_float_field(fields, 0);
    } else if (command == "CREATOR") {
        header.creator = c2s_string_field(fields, 0);
    } else if (command == "BPM_DEF") {
        for (std::size_t index = 0; index < header.bpm_def.size(); ++index) {
            header.bpm_def[index] = parse_c2s_float_field(fields, index);
        }
    } else if (command == "MET_DEF") {
        for (std::size_t index = 0; index < header.met_def.size(); ++index) {
            header.met_def[index] = parse_c2s_integer_field(fields, index);
        }
    } else if (command == "RESOLUTION") {
        return false;
    } else if (command == "CLK_DEF") {
        header.clk_def = parse_c2s_integer_field(fields, 0);
    } else if (command == "PROGJUDGE_BPM") {
        header.progjudge_bpm = parse_c2s_float_field(fields, 0);
    } else if (command == "PROGJUDGE_AER") {
        header.progjudge_aer = parse_c2s_float_field(fields, 0);
    } else if (command == "TUTORIAL") {
        header.tutorial = 0 < parse_c2s_integer_field(fields, 0);
    } else {
        return false;
    }
    return true;
}

// claim.input.tap-rising-edge-snapshot
//
// TAP input is formed from two 16-lane source banks. Edges are detected per
// source bit before the two banks are folded into logical lanes.
struct TapLaneInputSnapshot {
    std::uint32_t raw_level;
    std::uint16_t logical_level;
    std::uint16_t logical_rising;
};

constexpr TapLaneInputSnapshot derive_tap_lane_input(
    const std::array<std::uint8_t, 32>& current_sources,
    std::uint32_t previous_raw_level) {
    std::uint32_t raw_level = 0;
    for (std::uint32_t source = 0; source < current_sources.size(); ++source) {
        if (current_sources[source] != 0) {
            raw_level |= std::uint32_t{1} << source;
        }
    }

    const std::uint32_t raw_rising = raw_level & ~previous_raw_level;
    const auto fold_banks = [](std::uint32_t bits) {
        return static_cast<std::uint16_t>((bits | (bits >> 16U)) & 0xffffU);
    };
    return {raw_level, fold_banks(raw_level), fold_banks(raw_rising)};
}

// claim.input.snapshot-profile-synthesis
//
// Six photo-sensor inputs feed a conditioned mask and a residual mask. The
// residual mask is converted to a scalar position, whose bounded per-snapshot
// motion feeds eight externally configured derived profiles. Configuration
// values are resource-owned in the executable and therefore remain explicit.
struct InputProfileSynthesisConfig {
    std::uint32_t conditioning_history_count{};
    std::uint32_t conditioning_required_count{};
    std::array<std::uint32_t, 8> motion_history_counts{};
    std::array<std::int32_t, 8> absolute_motion_thresholds{};
    std::array<std::int32_t, 8> positive_motion_thresholds{};
    std::array<std::int32_t, 8> negative_motion_thresholds{};
    std::int32_t profile_7_position_lower{};
    std::int32_t profile_7_position_upper{};
};

struct InputSynthesisSnapshot {
    std::uint32_t raw_level{};
    std::uint32_t raw_rising{};
    std::uint16_t logical_level{};
    std::uint16_t logical_rising{};
    std::uint32_t sustain_marker{};
    std::uint8_t conditioned_photo_mask{};
    std::uint8_t residual_photo_mask{};
    std::int32_t photo_position{65};
    std::int32_t photo_position_delta{};
    std::array<std::uint8_t, 8> profile_levels{};
    bool air_sample_marker{};
};

constexpr std::int32_t residual_photo_position(std::uint8_t mask,
                                               std::int32_t previous) {
    mask &= 0x3fU;
    if (mask == 0) {
        return previous < 140 ? 65 : 215;
    }

    if ((mask & 0x05U) == 0x01U && (mask & 0x38U) == 0) {
        return (mask & 0x02U) == 0 ? 75 : 95;
    }
    if ((mask & 0x0fU) == 0 && (mask & 0x20U) != 0) {
        return (mask & 0x10U) == 0 ? 205 : 185;
    }

    std::int32_t index_sum = 0;
    std::int32_t count = 0;
    for (std::int32_t index = 0; index < 6; ++index) {
        if ((mask & (std::uint8_t{1} << index)) != 0) {
            index_sum += index;
            ++count;
        }
    }
    return 90 + (index_sum * 20) / count;
}

constexpr void accumulate_profile_motion(std::int32_t delta,
                                         std::int32_t& absolute,
                                         std::int32_t& positive,
                                         std::int32_t& negative) {
    absolute += delta < 0 ? -delta : delta;
    if (0 < delta) {
        positive += delta;
    } else if (delta < 0) {
        negative -= delta;
    }
}

inline InputSynthesisSnapshot synthesize_input_snapshot(
    const InputProfileSynthesisConfig& config,
    const std::array<std::uint8_t, 32>& touch_sources,
    std::uint8_t photo_sensor_mask,
    std::span<const InputSynthesisSnapshot> history_oldest_to_newest) {
    const InputSynthesisSnapshot default_previous{};
    const InputSynthesisSnapshot& previous =
        history_oldest_to_newest.empty() ? default_previous
                                         : history_oldest_to_newest.back();

    InputSynthesisSnapshot snapshot{};
    const TapLaneInputSnapshot lanes =
        derive_tap_lane_input(touch_sources, previous.raw_level);
    snapshot.raw_level = lanes.raw_level;
    snapshot.raw_rising = lanes.raw_level & ~previous.raw_level;
    snapshot.logical_level = lanes.logical_level;
    snapshot.logical_rising = lanes.logical_rising;

    const std::size_t conditioning_count =
        std::min<std::size_t>(config.conditioning_history_count,
                              history_oldest_to_newest.size());
    std::array<std::uint32_t, 6> prior_counts{};
    for (std::size_t age = 0; age < conditioning_count; ++age) {
        const std::uint8_t prior_mask =
            history_oldest_to_newest[history_oldest_to_newest.size() - 1 -
                                     age]
                .conditioned_photo_mask;
        for (std::size_t sensor = 0; sensor < prior_counts.size(); ++sensor) {
            if ((prior_mask & (std::uint8_t{1} << sensor)) != 0) {
                ++prior_counts[sensor];
            }
        }
    }

    snapshot.conditioned_photo_mask = previous.conditioned_photo_mask;
    for (std::size_t sensor = 0; sensor < prior_counts.size(); ++sensor) {
        const std::uint8_t bit = std::uint8_t{1} << sensor;
        if (prior_counts[sensor] < config.conditioning_required_count) {
            snapshot.conditioned_photo_mask &= static_cast<std::uint8_t>(~bit);
        } else if (!previous.air_sample_marker) {
            snapshot.conditioned_photo_mask |= bit;
        }
    }

    photo_sensor_mask &= 0x3fU;
    snapshot.residual_photo_mask = static_cast<std::uint8_t>(
        photo_sensor_mask & ~snapshot.conditioned_photo_mask);
    snapshot.photo_position = residual_photo_position(
        snapshot.residual_photo_mask, previous.photo_position);
    snapshot.photo_position_delta =
        snapshot.photo_position - previous.photo_position;
    if (snapshot.photo_position_delta < -50 ||
        50 < snapshot.photo_position_delta) {
        snapshot.photo_position_delta = 0;
    }

    snapshot.profile_levels[7] =
        config.profile_7_position_lower <= snapshot.photo_position &&
                snapshot.photo_position <= config.profile_7_position_upper
            ? 1
            : 0;

    // Profiles 0..6 use accumulated motion. Profile 7 is resolved solely by
    // the inclusive scalar-position range above, even though the owner stores
    // an eighth threshold record.
    for (std::size_t profile = 0; profile < 7; ++profile) {
        std::int32_t absolute = 0;
        std::int32_t positive = 0;
        std::int32_t negative = 0;
        accumulate_profile_motion(snapshot.photo_position_delta, absolute,
                                  positive, negative);

        const std::uint32_t requested_count =
            config.motion_history_counts[profile];
        for (std::size_t age = 0;
             age < history_oldest_to_newest.size() &&
             age + 1 < requested_count;
             ++age) {
            const std::int32_t delta =
                history_oldest_to_newest
                    [history_oldest_to_newest.size() - 1 - age]
                        .photo_position_delta;
            accumulate_profile_motion(delta, absolute, positive, negative);
        }

        const bool motion_active =
            config.absolute_motion_thresholds[profile] <= absolute ||
            config.positive_motion_thresholds[profile] <= positive ||
            config.negative_motion_thresholds[profile] <= negative;
        snapshot.profile_levels[profile] = static_cast<std::uint8_t>(
            snapshot.profile_levels[profile] != 0 || motion_active);
    }
    return snapshot;
}

inline constexpr std::size_t retained_input_snapshot_count = 301;

inline void append_input_snapshot(
    std::vector<InputSynthesisSnapshot>& history_oldest_to_newest,
    const InputSynthesisSnapshot& snapshot) {
    if (300 < history_oldest_to_newest.size()) {
        history_oldest_to_newest.erase(history_oldest_to_newest.begin());
    }
    history_oldest_to_newest.push_back(snapshot);
}

// claim.judgement.tap-window-classification
//
// The window values are supplied by external configuration in the source
// snapshot.  The reconstruction therefore models them as inputs.  Bands are
// ordered from the widest/outermost band to the center band.
struct TapInterval {
    float lower;
    float upper;

    constexpr bool contains(float value) const {
        return lower <= value && value < upper;
    }
};

struct TapLaneWindows {
    bool enabled;
    std::array<TapInterval, 5> bands_outer_to_center;
    float side_pivot;
};

// claim.judgement.tap-adjacent-window-adjustment
//
// Chart postprocessing can attach per-lane distances to a preceding and/or
// following nearby record. The checker initializer uses those distances to
// trim only the corresponding side of every timing band. The two internal
// cap values are loaded alongside the ordinary window configuration.
struct TapAdjacentWindowAdjustment {
    bool preceding_active;
    float preceding_distance;
    bool following_active;
    float following_distance;
    float base_offset;
    float early_upper_cap;
    float late_lower_cap;
};

constexpr void apply_tap_adjacent_window_adjustment(
    TapLaneWindows& windows,
    const TapAdjacentWindowAdjustment& adjustment) {
    if (!windows.enabled) {
        return;
    }

    if (adjustment.preceding_active) {
        float lower_limit =
            adjustment.base_offset - adjustment.preceding_distance;
        if (adjustment.early_upper_cap <= lower_limit) {
            lower_limit = adjustment.early_upper_cap;
        }
        if (windows.side_pivot <= lower_limit) {
            lower_limit = windows.side_pivot;
        }
        for (TapInterval& band : windows.bands_outer_to_center) {
            if (band.lower <= lower_limit && band.lower != lower_limit) {
                band.lower = lower_limit;
            }
        }
    }

    if (adjustment.following_active) {
        float upper_limit =
            adjustment.base_offset + adjustment.following_distance;
        if (upper_limit <= adjustment.late_lower_cap) {
            upper_limit = adjustment.late_lower_cap;
        }
        if (upper_limit <= windows.side_pivot) {
            upper_limit = windows.side_pivot;
        }
        for (TapInterval& band : windows.bands_outer_to_center) {
            if (upper_limit < band.upper) {
                band.upper = upper_limit;
            }
        }
    }
}

// Returns the snapshot's internal fine-result table index.  These indices are
// intentionally not assigned player-facing judgement names until the
// downstream outcome mapping is recovered.
constexpr std::uint8_t classify_tap_delta(const TapLaneWindows& windows,
                                          float delta) {
    if (!windows.enabled) {
        return 0;
    }

    if (windows.bands_outer_to_center[4].contains(delta)) {
        return 6;
    }
    if (windows.bands_outer_to_center[3].contains(delta)) {
        return delta < windows.side_pivot ? 5 : 7;
    }
    if (windows.bands_outer_to_center[2].contains(delta)) {
        return delta < windows.side_pivot ? 4 : 8;
    }
    if (windows.bands_outer_to_center[1].contains(delta)) {
        return delta < windows.side_pivot ? 3 : 9;
    }
    if (windows.bands_outer_to_center[0].contains(delta)) {
        return delta < windows.side_pivot ? 2 : 10;
    }
    return delta < windows.side_pivot ? 0 : 11;
}

struct TapFineResultRecord {
    std::uint8_t acceptance_code;
    std::uint8_t coarse_tier;
    std::uint8_t side_code;
    std::uint8_t table_code;

    constexpr bool accepted() const { return acceptance_code != 0; }
};

// Static result metadata indexed by classify_tap_delta().  table_code is kept
// deliberately anonymous because no player-facing label is needed for the
// recovered gameplay behavior.
inline constexpr std::array<TapFineResultRecord, 12> tap_fine_results{{
    {0, 0, 0, 1},
    {0, 0, 0, 4},
    {1, 0, 1, 6},
    {1, 1, 1, 8},
    {1, 2, 1, 10},
    {1, 3, 1, 11},
    {1, 4, 0, 9},
    {1, 3, 2, 7},
    {1, 2, 2, 5},
    {1, 1, 2, 3},
    {1, 0, 2, 1},
    {2, 0, 0, 0},
}};

// claim.note.flick-motion-judgement
//
// A FLK record starts with the shared rising-edge/timing classifier, then
// tracks lateral held-source motion. Three groups are evaluated in order: the
// union of both physical source banks, bank zero, and bank one.
enum class FlickPhase : std::uint8_t {
    awaiting_edge = 0,
    tracking_motion = 1,
    edge_timeout = 2,
    motion_timeout = 3,
    negative_motion = 4,
    positive_motion = 5,
};

enum class FlickMotionOutcome : std::uint8_t {
    tracking,
    negative,
    positive,
};

struct FlickMotionGroup {
    bool initialized{};
    float current{};
    float minimum{};
    float maximum{};
    // 0 is ordinary, 1 remembers the left edge, and 2 remembers the right.
    std::uint8_t boundary_status{};
};

struct FlickMotionTracker {
    std::array<FlickMotionGroup, 3> groups{};
};

constexpr bool flick_is_terminal(FlickPhase phase) {
    return phase == FlickPhase::edge_timeout ||
           phase == FlickPhase::motion_timeout ||
           phase == FlickPhase::negative_motion ||
           phase == FlickPhase::positive_motion;
}

constexpr bool flick_exposes_start_candidate(FlickPhase phase) {
    return phase == FlickPhase::awaiting_edge;
}

// claim.judgement.tap-center-window-adjustment
//
// After candidate exposure and before input classification, TAP-derived notes
// widen each center endpoint to the corresponding inner endpoint when inner
// and middle differ by less than this executable-owned epsilon. The endpoint
// checks are independent. FLK reuses the same checker mutation while awaiting
// its initiating edge.
inline constexpr float tap_center_endpoint_epsilon = 0.00001F;

inline void apply_tap_center_window_adjustment(
    TapLaneWindows& windows,
    float equality_epsilon = tap_center_endpoint_epsilon) {
    if (!windows.enabled) {
        return;
    }
    const TapInterval& middle = windows.bands_outer_to_center[2];
    const TapInterval& inner = windows.bands_outer_to_center[3];
    TapInterval& center = windows.bands_outer_to_center[4];
    if (std::fabs(inner.lower - middle.lower) < equality_epsilon) {
        center.lower = inner.lower;
    }
    if (std::fabs(inner.upper - middle.upper) < equality_epsilon) {
        center.upper = inner.upper;
    }
}

inline void apply_flick_center_window_adjustment(
    TapLaneWindows& windows,
    float equality_epsilon = tap_center_endpoint_epsilon) {
    apply_tap_center_window_adjustment(windows, equality_epsilon);
}

// Applies one finite held-source sample. observation_span and travel_threshold
// are externally configured in the source snapshot. The note range arguments
// are the already bounded start/count fields used by gameplay.
inline FlickMotionOutcome update_flick_motion(
    FlickMotionTracker& tracker,
    const std::array<bool, 16>& bank_zero,
    const std::array<bool, 16>& bank_one,
    int bounded_start,
    int bounded_count,
    int observation_span,
    float travel_threshold) {
    const auto clamp_lane = [](int lane) {
        if (lane < 0) {
            return 0;
        }
        return lane < 16 ? lane : 15;
    };

    const int padding = (observation_span - bounded_count + 1) / 2;
    const int first_lane = clamp_lane(bounded_start - padding);
    const int last_lane =
        clamp_lane(bounded_start + bounded_count - 1 + padding);

    std::array<int, 3> active_counts{};
    std::array<float, 3> lane_sums{};
    if (first_lane <= last_lane) {
        for (int lane = first_lane; lane <= last_lane; ++lane) {
            const bool zero = bank_zero[static_cast<std::size_t>(lane)];
            const bool one = bank_one[static_cast<std::size_t>(lane)];
            if (zero || one) {
                ++active_counts[0];
                lane_sums[0] += static_cast<float>(lane);
            }
            if (zero) {
                ++active_counts[1];
                lane_sums[1] += static_cast<float>(lane);
            }
            if (one) {
                ++active_counts[2];
                lane_sums[2] += static_cast<float>(lane);
            }
        }
    }

    const bool logical_left = bank_zero[0] || bank_one[0];
    const bool logical_right = bank_zero[15] || bank_one[15];
    for (std::size_t group_index = 0; group_index < tracker.groups.size();
         ++group_index) {
        FlickMotionGroup& group = tracker.groups[group_index];
        if (active_counts[group_index] != 0) {
            group.current =
                lane_sums[group_index] /
                    static_cast<float>(active_counts[group_index]) +
                0.5F;
            if (bounded_start == 0 && logical_left) {
                group.boundary_status = 1;
            } else if (bounded_start + bounded_count == 16 && logical_right) {
                group.boundary_status = 2;
            } else {
                group.boundary_status = 0;
            }
        } else {
            if (group.boundary_status == 1) {
                group.current = -0.5F;
            } else if (group.boundary_status == 2) {
                group.current = 16.5F;
            }
            group.boundary_status = 0;
        }
    }

    for (std::size_t group_index = 0; group_index < tracker.groups.size();
         ++group_index) {
        FlickMotionGroup& group = tracker.groups[group_index];
        if (!group.initialized && active_counts[group_index] != 0) {
            group.initialized = true;
            group.minimum = group.current;
            group.maximum = group.current;
        }
    }

    for (FlickMotionGroup& group : tracker.groups) {
        if (!group.initialized) {
            continue;
        }
        if (group.current < group.minimum) {
            group.minimum = group.current;
            if (std::fabs(group.maximum - group.current) >= travel_threshold) {
                return FlickMotionOutcome::negative;
            }
        }
        if (group.maximum < group.current) {
            group.maximum = group.current;
            if (std::fabs(group.current - group.minimum) >= travel_threshold) {
                return FlickMotionOutcome::positive;
            }
        }
    }
    return FlickMotionOutcome::tracking;
}

// The motion-stage state machine is evaluated after the start stage in the
// same note update, allowing a newly accepted edge to complete its motion on
// that same tick.
constexpr FlickPhase advance_flick_phase(FlickPhase phase,
                                         float delta,
                                         float motion_early_bound,
                                         float start_late_bound,
                                         float motion_late_bound,
                                         bool accepted_start_edge,
                                         FlickMotionOutcome motion) {
    if (flick_is_terminal(phase) || delta < motion_early_bound) {
        return phase;
    }

    if (phase == FlickPhase::awaiting_edge) {
        if (start_late_bound <= delta) {
            phase = FlickPhase::edge_timeout;
        } else if (accepted_start_edge) {
            phase = FlickPhase::tracking_motion;
        }
    }

    if (phase == FlickPhase::tracking_motion) {
        if (motion_late_bound <= delta) {
            return FlickPhase::motion_timeout;
        }
        if (motion == FlickMotionOutcome::negative) {
            return FlickPhase::negative_motion;
        }
        if (motion == FlickMotionOutcome::positive) {
            return FlickPhase::positive_motion;
        }
    }
    return phase;
}

struct FlickDirectionResult {
    std::uint8_t provisional_result;
    std::uint8_t side_code;
    std::uint8_t control_code;
};

// Successful directional completion keeps the start timing's coarse tier as
// the provisional downstream result. The sum of start and completion tiers
// selects a separate control code and can normalize the side code.
constexpr FlickDirectionResult compose_flick_direction_result(
    std::uint8_t start_fine_index,
    std::uint8_t completion_fine_index) {
    const TapFineResultRecord& start = tap_fine_results[start_fine_index];
    const TapFineResultRecord& completion =
        tap_fine_results[completion_fine_index];

    std::uint8_t combined =
        static_cast<std::uint8_t>(start.coarse_tier + completion.coarse_tier);
    std::uint8_t side_code = start.side_code;
    if (combined < 3) {
        combined = 3;
        side_code = 2;
    }

    constexpr std::array<std::uint8_t, 11> control_codes{
        0, 1, 1, 2, 2, 4, 4, 4, 4, 4, 4,
    };
    if (combined > 10) {
        combined = 10;
    }
    const std::uint8_t control_code = control_codes[combined];
    if (control_code == 4) {
        side_code = 0;
    }
    return {start.coarse_tier, side_code, control_code};
}

constexpr std::int32_t flick_terminal_metadata(FlickPhase phase) {
    switch (phase) {
        case FlickPhase::edge_timeout:
        case FlickPhase::motion_timeout:
            return 16;
        case FlickPhase::negative_motion:
            return 14;
        case FlickPhase::positive_motion:
            return 15;
        default:
            return -1;
    }
}

// Every FlickNote construction restores this shared flag to true. Forced FLK
// completions then alternate negative/positive direction across notes.
constexpr FlickPhase select_forced_flick_direction(bool& shared_flag) {
    if (shared_flag) {
        shared_flag = false;
        return FlickPhase::negative_motion;
    }
    shared_flag = true;
    return FlickPhase::positive_motion;
}

// claim.note.mine-contact-aggregate-judgement
//
// MNE does not expose a lane candidate. Once its externally configured
// activation start has been reached, it credits the clipped previous-to-
// current timing interval whenever at least one covered logical lane is held.
// The reconstruction is specified for finite float inputs; configuration
// values remain parameters because they are not embedded in the executable.
enum class MinePhase : std::uint8_t {
    accumulating = 0,
    threshold_reached = 1,
    threshold_unreachable = 2,
};

// claim.presentation.mine-root-success-effect
//
// The root model table is the executable-owned descending sequence 145..130.
// It otherwise uses the shared lane center, native-width scale, parked depth,
// and executable depth-axis scale documented by the presentation claims.
constexpr std::int32_t mine_model_resource_row(
    std::int32_t decoded_width) {
    return 145 - std::clamp(decoded_width - 1, 0, 15);
}

constexpr bool mine_root_model_update_enabled(MinePhase phase) {
    return phase == MinePhase::accumulating;
}

// Normal success uses the average index of currently held covered lanes when
// at least one exists. With no current held lane, and for forced success, the
// span center remains selected. The common lane-edge transform then converts
// this coordinate to chart-space x.
constexpr float mine_success_effect_lane_coordinate(
    std::int32_t start_lane,
    std::int32_t decoded_width,
    std::int32_t held_lane_index_sum,
    std::int32_t held_lane_count,
    bool forced_result) {
    if (!forced_result && 0 < held_lane_count) {
        return static_cast<float>(held_lane_index_sum) /
               static_cast<float>(held_lane_count);
    }
    return static_cast<float>(start_lane) +
           static_cast<float>(decoded_width) * 0.5F;
}

constexpr float mine_success_effect_lateral(
    std::int32_t start_lane,
    std::int32_t decoded_width,
    std::int32_t held_lane_index_sum,
    std::int32_t held_lane_count,
    bool forced_result) {
    return (mine_success_effect_lane_coordinate(
                start_lane, decoded_width, held_lane_index_sum,
                held_lane_count, forced_result) -
            8.0F) *
           4.0F;
}

// FUN_00b29920 uses an inclusive upper bound, unlike model-row indexing, then
// reads a startup table whose first two entries are zero and remaining entries
// are width-1. The selected value is executable-owned, not external data.
constexpr std::int32_t mine_success_effect_width_selector(
    std::int32_t decoded_width) {
    return std::max(std::clamp(decoded_width, 0, 16) - 1, 0);
}

constexpr bool mine_emits_success_effect(std::uint8_t remapped_result_byte) {
    return remapped_result_byte == 0;
}

inline constexpr float mine_success_effect_depth = -65.0F;
inline constexpr std::int32_t mine_primary_effect_kind = 8;
inline constexpr std::int32_t mine_secondary_effect_kind = 9;
inline constexpr std::int32_t mine_primary_effect_lifetime = 100;

struct MineContactState {
    MinePhase phase{MinePhase::accumulating};
    float accumulated{};
};

constexpr bool mine_is_terminal(MinePhase phase) {
    return phase == MinePhase::threshold_reached ||
           phase == MinePhase::threshold_unreachable;
}

constexpr bool mine_exposes_candidate(MinePhase) {
    return false;
}

constexpr MinePhase update_mine_contact(MineContactState& state,
                                        float current_delta,
                                        float previous_delta,
                                        bool any_covered_lane_held,
                                        float activation_start,
                                        float activation_end,
                                        float required_contact) {
    if (mine_is_terminal(state.phase) || current_delta < activation_start) {
        return state.phase;
    }

    if (any_covered_lane_held) {
        const float segment_start = previous_delta < activation_start
                                        ? activation_start
                                        : previous_delta;
        const float segment_end = activation_end < current_delta
                                      ? activation_end
                                      : current_delta;
        const float overlap = segment_end - segment_start;
        if (0.0F < overlap) {
            state.accumulated += overlap;
        }
    }

    if (0.0F <= current_delta) {
        const float raw_remaining = activation_end - current_delta;
        const float remaining = raw_remaining < 0.0F ? 0.0F : raw_remaining;
        if (required_contact > state.accumulated + remaining) {
            state.phase = MinePhase::threshold_unreachable;
        } else if (state.accumulated >= required_contact) {
            state.phase = MinePhase::threshold_reached;
        }
    }
    return state.phase;
}

// Ordinary terminal resolution supplies byte 0 for a reached threshold and
// byte 4 for an unreachable threshold. Forced MNE resolution uses the selected
// byte's zero/nonzero distinction to choose the same two phases.
constexpr std::uint8_t mine_terminal_result_byte(MinePhase phase) {
    return phase == MinePhase::threshold_unreachable ? 4 : 0;
}

constexpr MinePhase mine_phase_from_forced_result(std::uint8_t result_byte) {
    return result_byte == 0 ? MinePhase::threshold_reached
                            : MinePhase::threshold_unreachable;
}

inline constexpr std::int32_t mine_source_category = 17;

// claim.note.air-secondary-judgement
//
// AIR-family commands do not create standalone parsed records. They attach a
// direction code to a compatible root record; the runtime factory then creates
// an AirNote in the separately owned and later-updated secondary vector.
enum class AirCommand : std::uint8_t {
    air,
    aur,
    aul,
    adw,
    adr,
    adl,
};

enum class AirDirectionCode : std::uint8_t {
    air = 0,
    aur = 1,
    aul = 2,
    adw = 3,
    adr = 4,
    adl = 5,
};

constexpr AirDirectionCode air_direction_code(AirCommand command,
                                              bool mirrored) {
    switch (command) {
        case AirCommand::air:
            return AirDirectionCode::air;
        case AirCommand::aur:
            return mirrored ? AirDirectionCode::aul
                            : AirDirectionCode::aur;
        case AirCommand::aul:
            return mirrored ? AirDirectionCode::aur
                            : AirDirectionCode::aul;
        case AirCommand::adw:
            return AirDirectionCode::adw;
        case AirCommand::adr:
            return mirrored ? AirDirectionCode::adl
                            : AirDirectionCode::adr;
        case AirCommand::adl:
            return mirrored ? AirDirectionCode::adr
                            : AirDirectionCode::adl;
    }
    return AirDirectionCode::air;
}

constexpr bool air_uses_upper_profile(AirDirectionCode direction) {
    return static_cast<std::uint8_t>(direction) <=
           static_cast<std::uint8_t>(AirDirectionCode::aul);
}

// Returns the source-profile index used by the AIR timing checker, or -1 for
// a root type not handled by the executable's profile initializer.
constexpr std::int32_t air_input_profile(std::int32_t root_type,
                                         AirDirectionCode direction) {
    std::int32_t base = -1;
    switch (root_type) {
        case 1:
        case 2:
        case 13:
            base = 0;
            break;
        case 0:
        case 4:
        case 11:
            base = 2;
            break;
        case 6:
            base = 4;
            break;
        default:
            return -1;
    }
    return base + (air_uses_upper_profile(direction) ? 0 : 1);
}

// The newest input snapshot stores eight derived profile bytes and a distinct
// marker. AIR sets the marker before reading its selected byte. The marker's
// effect on construction of the following snapshot is specified separately;
// setting it does not alter the byte returned for the current snapshot.
struct AirProfileSnapshot {
    std::array<std::uint8_t, 8> profile_levels{};
    bool sample_marker{};
};

constexpr bool sample_air_profile(AirProfileSnapshot& snapshot,
                                  std::int32_t profile) {
    snapshot.sample_marker = true;
    return 0 <= profile &&
           static_cast<std::size_t>(profile) < snapshot.profile_levels.size() &&
           snapshot.profile_levels[static_cast<std::size_t>(profile)] != 0;
}

struct AirFineResultRecord {
    std::uint8_t rank;
    std::uint8_t acceptance_code;
    std::uint8_t coarse_tier;
    std::uint8_t side_code;

    constexpr bool accepted() const { return acceptance_code != 0; }
};

// AIR's retained-input classifier has its own fixed table. The fine indices
// share the TAP interval geometry, but the acceptance and rank fields are
// independent executable data.
inline constexpr std::array<AirFineResultRecord, 12> air_fine_results{{
    {0, 0, 0, 0},
    {1, 0, 0, 0},
    {4, 1, 0, 1},
    {6, 1, 1, 1},
    {8, 1, 2, 1},
    {10, 1, 3, 1},
    {11, 1, 4, 0},
    {9, 1, 3, 2},
    {7, 1, 2, 2},
    {5, 1, 1, 2},
    {3, 1, 0, 2},
    {2, 1, 0, 2},
}};

struct AirTimingState {
    std::uint8_t retained_fine_index{};
};

struct AirTimingDecision {
    std::uint8_t fine_index;
    std::uint8_t coarse_tier;
    std::uint8_t side_code;
    bool completes;
};

// Applies one current-profile sample. Input before the pivot can improve the
// retained fine result without completing. At/after the pivot, a retained
// result is emitted when it is better than the current timing location; a
// retained center result is emitted immediately. Reaching the outer upper
// endpoint otherwise produces fine index 11.
constexpr AirTimingDecision update_air_timing(
    AirTimingState& state,
    const TapLaneWindows& windows,
    float delta,
    bool profile_level) {
    const auto make_decision = [](std::uint8_t fine, bool completes) {
        const AirFineResultRecord& record = air_fine_results[fine];
        return AirTimingDecision{
            fine, record.coarse_tier, record.side_code, completes};
    };

    const TapInterval& outermost = windows.bands_outer_to_center[0];
    if (!windows.enabled || !(outermost.lower < delta)) {
        return make_decision(0, false);
    }

    const std::uint8_t current = classify_tap_delta(windows, delta);
    const std::uint8_t retained = state.retained_fine_index;
    std::uint8_t selected = current;
    bool retained_or_sampled = false;

    if (air_fine_results[retained].rank <=
        air_fine_results[current].rank) {
        if (profile_level) {
            state.retained_fine_index = current;
            selected = current;
            retained_or_sampled = true;
        }
    } else {
        selected = retained;
        retained_or_sampled = true;
    }

    std::uint8_t output = selected;
    if (delta < windows.side_pivot) {
        output = 1;
    } else if (state.retained_fine_index == 6) {
        output = 6;
    } else if (outermost.upper <= delta) {
        output = 11;
    } else if (!retained_or_sampled) {
        output = 1;
    }

    return make_decision(
        output,
        windows.side_pivot <= delta && air_fine_results[output].accepted());
}

enum class AirPhase : std::uint8_t {
    awaiting_result = 0,
    resolved = 3,
};

constexpr bool air_is_terminal(AirPhase phase) {
    return phase == AirPhase::resolved;
}

constexpr bool air_exposes_candidate(AirPhase) {
    return false;
}

constexpr std::int32_t air_source_category(AirDirectionCode direction) {
    return air_uses_upper_profile(direction) ? 7 : 8;
}

// claim.judgement.active-tier-zeroing
//
// A loaded skill profile owns an ordered vector of change-result controls.
// The remap reads only the first unit and deliberately does not consult the
// separate temporary-effect lifetime predicate used by other skill consumers.
// The result-type count and threshold originate outside the classifier.
struct ActiveResultControlUnit {
    bool source_record_present{};
    std::uint8_t threshold{};
};

struct LoadedActiveResultControls {
    std::int32_t skill_profile_id{-1};
    std::span<const ActiveResultControlUnit> units{};
};

// The same SkillBefore rebuild owns all five gameplay control vectors. Any
// changed component of the selected identity triple resets every vector. The
// middle component is the ordered-map lookup key. Source units are routed in
// order by exact type; type 4 and unknown values are ignored.
struct SkillBeforeControlIdentity {
    std::int32_t first{};
    std::int32_t profile_id{-1};
    std::int32_t third{};
};

enum class SkillBeforeGameplayControlRoute : std::uint8_t {
    gauge_assist,
    gauge_keep,
    damage_guard,
    death_penalty,
    change_judge_result,
    ignored,
};

constexpr bool skill_before_controls_require_rebuild(
    const SkillBeforeControlIdentity& loaded,
    const SkillBeforeControlIdentity& selected) {
    return loaded.first != selected.first ||
           loaded.profile_id != selected.profile_id ||
           loaded.third != selected.third;
}

constexpr std::int32_t skill_before_control_lookup_key(
    const SkillBeforeControlIdentity& selected) {
    return selected.profile_id;
}

constexpr SkillBeforeGameplayControlRoute route_skill_before_gameplay_unit(
    std::int32_t type) {
    switch (type) {
    case 0:
        return SkillBeforeGameplayControlRoute::gauge_assist;
    case 1:
        return SkillBeforeGameplayControlRoute::gauge_keep;
    case 2:
        return SkillBeforeGameplayControlRoute::damage_guard;
    case 3:
        return SkillBeforeGameplayControlRoute::death_penalty;
    case 5:
        return SkillBeforeGameplayControlRoute::change_judge_result;
    default:
        return SkillBeforeGameplayControlRoute::ignored;
    }
}

constexpr std::uint8_t apply_loaded_active_result_controls(
    std::uint8_t provisional_tier,
    LoadedActiveResultControls controls,
    std::uint8_t result_type_count) {
    if (controls.skill_profile_id < 0 || controls.units.empty() ||
        !controls.units.front().source_record_present) {
        return provisional_tier;
    }

    const std::uint8_t threshold = controls.units.front().threshold;
    if (provisional_tier < result_type_count &&
        threshold < result_type_count && provisional_tier <= threshold) {
        return 0;
    }
    return provisional_tier;
}

// claim.presentation.shared-result-feedback
//
// All twelve vtables use the common result consumer at +0x44. At +0x48,
// Mine and Flick alone use opcode-identical wrappers which apply the active
// result-control remap to the incoming result byte before forwarding all
// arguments to the common one-position wrapper. The other ten families use
// the common one-position wrapper directly.
enum class RuntimeFeedbackFamily : std::uint8_t {
    slide,
    air_ladder,
    heaven_hold,
    air_solid,
    tap,
    air,
    mine,
    flick,
    air_hold,
    air_slide,
    hold,
    chara_tap,
};

constexpr bool feedback_wrapper_applies_active_result_control(
    RuntimeFeedbackFamily family) {
    return family == RuntimeFeedbackFamily::mine ||
           family == RuntimeFeedbackFamily::flick;
}

constexpr std::uint8_t apply_feedback_wrapper_result_control(
    RuntimeFeedbackFamily family,
    std::uint8_t incoming_result,
    LoadedActiveResultControls controls,
    std::uint8_t result_type_count) {
    if (!feedback_wrapper_applies_active_result_control(family)) {
        return incoming_result;
    }
    return apply_loaded_active_result_controls(
        incoming_result, controls, result_type_count);
}

// Maps the coarse tier and side code to the detailed downstream result code
// produced by the TAP checker.  side_code 1 is the early-side branch for tiers
// 1 through 3; all other values take the late-side branch.
constexpr std::uint8_t tap_detailed_result_code(std::uint8_t coarse_tier,
                                                std::uint8_t side_code) {
    switch (coarse_tier) {
        case 0:
            return 11;
        case 1:
            return side_code == 1 ? 3 : 9;
        case 2:
            return side_code == 1 ? 4 : 8;
        case 3:
            return side_code == 1 ? 5 : 7;
        case 4:
            return 6;
        default:
            return 0;
    }
}

// claim.matching.tap-deferred-terminal-candidate
//
// The shared manager keeps the smallest nonnegative exposed candidate.  Its
// first-candidate sentinel is -1; when the sentinel is still selected, the
// first exposed value is copied exactly.
constexpr std::int32_t reduce_lane_candidate(std::int32_t selected,
                                             std::int32_t exposed) {
    if (selected == -1 || (exposed >= 0 && exposed < selected)) {
        return exposed;
    }
    return selected;
}

// claim.matching.flick-candidate-asymmetry
//
// This models only the manager-candidate part of start eligibility. The caller
// must separately establish rising input and local timing acceptance. TAP,
// CHR, unresolved HOLD starts, and unresolved Slide starts require equality
// with the lane minimum. Ordinary FLK start timing is independent of that
// minimum even though FLK contributed its own exposed candidate.
enum class LaneCandidateGate : std::uint8_t {
    selected_equality,
    independent,
};

constexpr bool lane_candidate_gate_allows_start(
    LaneCandidateGate gate,
    std::int32_t selected,
    std::int32_t exposed) {
    if (gate == LaneCandidateGate::independent) {
        return true;
    }
    return selected >= 0 && selected == exposed;
}

// claim.interactions.cross-family-candidate-result-order
//
// Candidate production and input updates are separate full-vector passes.
// Reduction is complete before any participant reads the selected value, and
// neither the selected candidate nor the rising edge is consumed. The returned
// acceptance vector therefore preserves active-vector order without using it
// as a tie breaker.
struct LaneCandidateParticipant {
    std::int32_t exposed{-1};
    LaneCandidateGate gate{LaneCandidateGate::selected_equality};
    bool local_input_accepts{};
};

struct LaneCandidateFanoutDecision {
    std::int32_t selected{-1};
    std::vector<bool> accepts;
};

inline LaneCandidateFanoutDecision evaluate_lane_candidate_fanout(
    std::span<const LaneCandidateParticipant> participants) {
    LaneCandidateFanoutDecision decision;
    for (const auto& participant : participants) {
        decision.selected =
            reduce_lane_candidate(decision.selected, participant.exposed);
    }
    decision.accepts.reserve(participants.size());
    for (const auto& participant : participants) {
        decision.accepts.push_back(
            participant.local_input_accepts &&
            lane_candidate_gate_allows_start(
                participant.gate, decision.selected, participant.exposed));
    }
    return decision;
}

// The base note state machine stores requested transitions separately from
// the current state. TAP result handling requests state 2 while state 1 is
// current; the next normal note tick commits that request. Candidate reduction
// happens before that tick, so callers must not commit early.
struct NoteLifecycleState {
    std::int32_t current;
    std::int32_t requested;
};

// claim.pipeline.runtime-note-dispatch
//
// Every primary and attached-secondary class reachable from the runtime
// factory uses the same three-state callback table. Construction leaves the
// current state at -1 and requests state 0. A manager substep commits a pending
// transition before choosing the callback for the new current state. All
// concrete state-0 predicates return true, so state 0 requests state 1; only
// state 1 invokes the note-specific gameplay update. A terminal request made
// by that update is committed by a later manager substep.
enum class RuntimeNoteScheduledCallback : std::uint8_t {
    none,
    state_0_activation,
    state_1_update,
};

struct RuntimeNoteDispatchResult {
    bool transition_committed{};
    RuntimeNoteScheduledCallback callback{
        RuntimeNoteScheduledCallback::none};
    bool removable_after_substep{};
};

// NotesBase inherits an optional child-state action hook from the generic
// state-machine framework. Both child factories are initialized empty, and
// every factory-reachable constructor/load path leaves them empty. The
// post-gameplay indirect call therefore has no target for runtime notes.
inline constexpr bool runtime_note_has_state_action_child = false;

// The common note-tick wrapper has a byte guard at base offset +0x65. The
// shared NotesBase constructor clears it and the closed factory, load,
// manager, registry, and transition paths contain no writer, so runtime notes
// cannot be paused through that guard in this snapshot.
inline constexpr bool runtime_note_tick_is_suppressed = false;

constexpr NoteLifecycleState make_runtime_note_lifecycle() {
    return {-1, 0};
}

constexpr void request_terminal_transition(NoteLifecycleState& state) {
    if (state.current == 1) {
        state.requested = 2;
    }
}

constexpr bool commit_requested_transition(NoteLifecycleState& state) {
    if (state.requested < 0) {
        return false;
    }
    state.current = state.requested;
    state.requested = -1;
    return true;
}

constexpr bool is_terminal(const NoteLifecycleState& state) {
    return state.current == 2;
}

constexpr RuntimeNoteDispatchResult dispatch_runtime_note_substep(
    NoteLifecycleState& state,
    bool state_1_update_requests_terminal = false) {
    RuntimeNoteDispatchResult result;
    result.transition_committed = commit_requested_transition(state);

    if (state.current == 0) {
        result.callback = RuntimeNoteScheduledCallback::state_0_activation;
        state.requested = 1;
    } else if (state.current == 1) {
        result.callback = RuntimeNoteScheduledCallback::state_1_update;
        if (state_1_update_requests_terminal) {
            state.requested = 2;
        }
    }

    result.removable_after_substep = is_terminal(state);
    return result;
}

// claim.matching.tap-deferred-terminal-candidate
//
// TAP stores its detailed result in the embedded checker. The state callback
// treats values above one as already resolved and skips its judgement gate.
constexpr bool tap_has_terminal_detailed_result(std::int32_t detailed_result) {
    return detailed_result > 1;
}

// claim.timing.gameplay-clock-reconstruction
//
// The executable's chart-time scale is 0.06 scalar units per integer
// millisecond. Keep the two multiplies and final addition separate: that is
// the source operation order and can matter at float precision.
inline constexpr float chart_units_per_millisecond = 0.06F;
inline constexpr std::uint64_t invalid_music_cursor_milliseconds =
    std::numeric_limits<std::uint64_t>::max();

// The scene's music handle query returns a 64-bit cursor and a positive
// cursor-unit rate. The executable converts their ratio to integer
// milliseconds with a double-precision multiply by 1000.0. Query failure,
// a cursor with its sign bit set, or a nonpositive rate yields UINT64_MAX.
inline std::uint64_t music_cursor_milliseconds(
    bool query_succeeded,
    std::uint64_t cursor_units,
    std::int32_t cursor_units_per_second) {
    if (!query_succeeded ||
        cursor_units >
            static_cast<std::uint64_t>(
                std::numeric_limits<std::int64_t>::max()) ||
        cursor_units_per_second <= 0) {
        return invalid_music_cursor_milliseconds;
    }
    const double milliseconds =
        static_cast<double>(cursor_units) /
        static_cast<double>(cursor_units_per_second) * 1000.0;
    return static_cast<std::uint64_t>(milliseconds);
}

// Gameplay state entry narrows the sampled unsigned millisecond cursor through
// double to float before the ordinary clock applies its 0.06F scale.
inline float music_cursor_base_time_value(
    std::uint64_t cursor_milliseconds) {
    return static_cast<float>(static_cast<double>(cursor_milliseconds));
}

// The resettable timer keeps only the low 32-bit millisecond result. Its
// current/captured value minus origin is therefore native modulo-2^32
// subtraction before the gameplay clock converts it as unsigned.
constexpr std::uint32_t elapsed_milliseconds_modulo(
    std::uint32_t current_milliseconds,
    std::uint32_t origin_milliseconds) {
    return current_milliseconds - origin_milliseconds;
}

// claim.judgement.periodic-aggregate-reevaluation
//
// The post-note aggregate pass converts the current manager position back to
// an integer tick with this separately initialized single-precision factor.
// It is intentionally not expressed as 1 / chart_units_per_millisecond: the
// two source constants are not exact floating-point inverses. The executable
// floors the product before its signed 64-bit conversion.
inline constexpr float periodic_milliseconds_per_chart_unit = 16.666666F;

inline std::int64_t periodic_aggregate_position_tick(
    float manager_position) {
    const float scaled =
        manager_position * periodic_milliseconds_per_chart_unit;
    return static_cast<std::int64_t>(std::floor(scaled));
}

// claim.note.tap-construction
//
// The parsed floating position at record +0x08 is multiplied directly by the
// same initialized scale when the runtime note stores its scheduled position.
constexpr float make_note_scheduled_position(float parsed_position) {
    return parsed_position * chart_units_per_millisecond;
}

// claim.parser.common-lane-width-encoding
//
// The group-2 parser clamps an authored note width to [1, 16], then stores the
// fixed table encoding width - 1. Runtime loading decodes indices [0, 15] back
// to widths [1, 16]; an invalid encoded value decodes to zero. The standalone
// encoder also has fixed behavior outside the parser's already-clamped domain.
constexpr std::int32_t encode_c2s_note_width(std::int32_t width) {
    if (width > 16) {
        return 15;
    }
    if (width < 0) {
        width = 0;
    }
    return width <= 1 ? 0 : width - 1;
}

constexpr std::int32_t decode_c2s_note_width(std::int32_t encoded_width) {
    return encoded_width >= 0 && encoded_width < 16
               ? encoded_width + 1
               : 0;
}

struct C2sCommonLaneGeometry {
    std::int32_t lane{};
    std::int32_t width{1};
    std::int32_t encoded_width{};
};

constexpr C2sCommonLaneGeometry parse_c2s_common_lane_geometry(
    std::int32_t lane,
    std::int32_t authored_width) {
    const std::int32_t width = std::clamp(authored_width, 1, 16);
    return {lane, width, encode_c2s_note_width(width)};
}

struct RuntimeLaneExtent {
    std::int32_t start{};
    std::int32_t count{};
};

// The source's 32-bit lane-plus-width addition wraps before its signed
// comparisons. Express that explicitly so malformed extremes do not invoke
// C++ signed-overflow undefined behavior.
constexpr std::int32_t add_i32_wrapped(
    std::int32_t left,
    std::int32_t right) {
    return std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(left) +
        static_cast<std::uint32_t>(right));
}

constexpr std::int32_t subtract_i32_wrapped(
    std::int32_t left,
    std::int32_t right) {
    return std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(left) -
        static_cast<std::uint32_t>(right));
}

// claim.presentation.chart-mirror-transform
//
// The group-2 parser clamps each authored width first, then mirrors every
// root/control lane span at signed 32-bit width. Presentation consumes the
// resulting lane values; it does not apply a second chart-mirror flag.
constexpr std::int32_t mirror_c2s_lane_origin(
    std::int32_t lane,
    std::int32_t clamped_width) {
    return subtract_i32_wrapped(
        subtract_i32_wrapped(16, lane), clamped_width);
}

// x86 CVTTSS2SI truncates a finite in-range float toward zero and returns the
// signed integer-indefinite bit pattern for NaN, infinity, or an out-of-range
// value. Keep every chart-reachable float-to-tick/property conversion defined.
constexpr std::int32_t cvttss2si_i32(float value) {
    if (!(value >= -2147483648.0F && value < 2147483648.0F)) {
        return std::numeric_limits<std::int32_t>::min();
    }
    return static_cast<std::int32_t>(value);
}

constexpr RuntimeLaneExtent bounded_note_lane_extent(
    std::int32_t lane,
    std::int32_t decoded_width) {
    const std::int32_t start = std::max(lane, 0);
    const std::int32_t end =
        std::min(add_i32_wrapped(lane, decoded_width), 16);
    return {start, std::max(subtract_i32_wrapped(end, start), 0)};
}

// claim.judgement.result-component-identifier-flow
//
// A parser-local 32-bit counter starts at zero for each event pass. Accepted
// root types reserve either one or two consecutive component identifiers;
// attached type 3/5/8 records lazily reserve the third slot on their root.
// Shared result submission chooses one of the copied slots by source category.
struct C2sResultComponentIdentifiers {
    std::int32_t primary{-1};
    std::int32_t middle{-1};
    std::int32_t secondary{-1};
};

constexpr std::int32_t c2s_root_result_identifier_count(
    std::int32_t parsed_type) {
    switch (parsed_type) {
    case 0:
    case 4:
    case 6:
    case 11:
        return 1;
    case 1:
    case 2:
    case 9:
    case 10:
    case 12:
    case 13:
        return 2;
    default:
        return 0;
    }
}

constexpr C2sResultComponentIdentifiers allocate_c2s_root_result_identifiers(
    std::int32_t parsed_type,
    std::int32_t& next_identifier) {
    C2sResultComponentIdentifiers identifiers;
    const std::int32_t count =
        c2s_root_result_identifier_count(parsed_type);
    if (count == 0) {
        return identifiers;
    }

    identifiers.primary = next_identifier;
    next_identifier = add_i32_wrapped(next_identifier, 1);
    if (count == 2) {
        identifiers.middle = next_identifier;
        next_identifier = add_i32_wrapped(next_identifier, 1);
    }
    return identifiers;
}

constexpr void attach_c2s_secondary_result_identifier(
    C2sResultComponentIdentifiers& identifiers,
    std::int32_t& next_identifier) {
    if (identifiers.secondary < 0) {
        identifiers.secondary = next_identifier;
        next_identifier = add_i32_wrapped(next_identifier, 1);
    }
}

constexpr std::int32_t select_note_result_identifier(
    const C2sResultComponentIdentifiers& identifiers,
    std::int32_t source_result_category) {
    if (2 <= source_result_category && source_result_category <= 6) {
        return identifiers.middle;
    }
    if (7 <= source_result_category && source_result_category <= 13) {
        return identifiers.secondary;
    }
    return identifiers.primary;
}

// claim.pipeline.runtime-note-materialization-order
//
// Runtime-note creation is gated by a projected-position probe. The raw-delta
// shortcut is evaluated before an optional chart-region adjustment. For a far
// probe, positive adjusted deltas select a chart-owned projection factor;
// nonpositive deltas use 1.0. The speed and base offset are runtime inputs.
inline constexpr float runtime_materialization_near_delta = 30.0F;
inline constexpr float runtime_materialization_projection_origin = -65.0F;
inline constexpr float runtime_materialization_projection_lower = -550.0F;
inline constexpr float runtime_materialization_projection_upper = 550.0F;
inline constexpr float runtime_materialization_fixed_scale = 1.5F;

// claim.pipeline.presentation-boundary-map
//
// The active note-view projection uses the same chart-space origin and
// multiplicative order as materialization, but exposes two call-site flags.
// The DCM factor is consulted only when base-offset mode is enabled and the
// supplied delta is positive. This is the resource/model depth before any
// downstream camera or viewport conversion.
constexpr float active_note_projected_depth(
    float delta,
    float runtime_speed,
    float projection_base_offset,
    float positive_delta_projection_factor,
    bool apply_projection_base_offset,
    bool apply_positive_delta_factor,
    float approach_scale = runtime_materialization_fixed_scale) {
    float factor = 1.0F;
    if (apply_projection_base_offset) {
        if (apply_positive_delta_factor && 0.0F < delta) {
            factor = positive_delta_projection_factor;
        }
        delta = projection_base_offset + delta;
    }
    return runtime_materialization_projection_origin -
           approach_scale * delta * runtime_speed * factor;
}

// Common lane-coordinate conversion from FUN_00b28870. Runtime lanes are
// already mirrored before this point. Sixteen lanes occupy 64 chart-space
// units, centered on zero.
constexpr float presentation_lane_coordinate_lateral(float coordinate) {
    return (coordinate - 8.0F) * 4.0F;
}

constexpr float mirror_presentation_lane_center(float lane_center) {
    return 16.0F - lane_center;
}

// Tap-family model placement from FUN_00b28820 and FUN_00c1da90.
constexpr float tap_model_lateral_center(
    std::int32_t start_lane,
    float decoded_width) {
    return presentation_lane_coordinate_lateral(
        static_cast<float>(start_lane) + decoded_width * 0.5F);
}

enum class TapModelResourceFamily : std::uint8_t {
    ordinary,
    extended_primary,
    extended_alternate,
};

// The three executable-owned, width-indexed model-row tables are contiguous
// descending integer IDs: 81..66, 97..82, and 113..98. Their referenced model
// contents remain external resource data.
constexpr std::int32_t tap_model_resource_row(
    std::int32_t decoded_width,
    TapModelResourceFamily family) {
    std::int32_t width_index = decoded_width - 1;
    if (width_index < 0) {
        width_index = 0;
    } else if (15 < width_index) {
        width_index = 15;
    }

    std::int32_t first_row = 81;
    if (family == TapModelResourceFamily::extended_primary) {
        first_row = 97;
    } else if (family == TapModelResourceFamily::extended_alternate) {
        first_row = 113;
    }
    return first_row - width_index;
}

// InstancingModel is initially parked at depth -10000. Its lateral scale is
// decoded width divided by the external model record's native width; invalid
// native widths select 1.0. The depth-axis scale is selected by an executable
// platform flag during PlayMusic construction.
inline constexpr float tap_model_parked_depth = -10000.0F;

constexpr float tap_model_lateral_scale(
    float decoded_width,
    std::int32_t external_native_width) {
    return external_native_width < 1
               ? 1.0F
               : decoded_width /
                     static_cast<float>(external_native_width);
}

constexpr float tap_model_depth_scale(bool alternate_platform_mode) {
    return alternate_platform_mode ? 1.3F : 1.0F;
}

// FlickNote uses its own executable-owned, width-indexed resource-row table,
// but reuses the Tap-family lane center, native-width scale, parked depth, and
// executable depth-axis scale. The table is the descending sequence 129..114.
constexpr std::int32_t flick_model_resource_row(
    std::int32_t decoded_width) {
    return 129 - std::clamp(decoded_width - 1, 0, 15);
}

// Flick's owned phase domain is 0..5. Its executable terminal byte table is
// {0,0,1,1,1,1}; the active root-model path therefore covers only phases 0/1.
constexpr bool flick_phase_is_terminal(std::int32_t phase) {
    return 2 <= phase && phase <= 5;
}

constexpr bool flick_root_model_visible(std::int32_t phase) {
    return !flick_phase_is_terminal(phase);
}

// HOLD uses one root model and one far marker plus a separate dynamic body.
// Ordinary root rows are 49..34. Extended roots reuse the two Tap-family
// extended tables. The far marker selects rows 65..50 normally and 289..274
// when parsed attachment type 3, 5, or 8 set the common secondary byte.
constexpr std::int32_t hold_root_model_resource_row(
    std::int32_t decoded_width,
    bool extended,
    bool alternate_extended_variant) {
    if (!extended) {
        return 49 - std::clamp(decoded_width - 1, 0, 15);
    }
    return tap_model_resource_row(
        decoded_width,
        alternate_extended_variant
            ? TapModelResourceFamily::extended_alternate
            : TapModelResourceFamily::extended_primary);
}

constexpr std::int32_t hold_far_marker_resource_row(
    std::int32_t decoded_width,
    bool has_attached_secondary) {
    const std::int32_t first_row = has_attached_secondary ? 289 : 65;
    return first_row - std::clamp(decoded_width - 1, 0, 15);
}

enum class HoldPresentationMode : std::uint8_t {
    base = 0,
    animated = 1,
    alternate = 2,
};

// Three startup initializers construct these shared packed colors. They are
// executable constants used by Hold/HeavenHold and the common sustain meshes,
// not externally loaded material values.
inline constexpr std::uint32_t presentation_static_base_color = 0xffffffffU;
inline constexpr std::uint32_t presentation_static_low_alpha_color =
    0x40ffffffU;
inline constexpr std::uint32_t presentation_static_alternate_color =
    0xff666666U;

enum class SharedAirPathPresentationMode : std::uint8_t {
    base = 0,
    animated = 1,
    alternate = 2,
};

constexpr std::uint32_t shared_air_path_primary_color(
    SharedAirPathPresentationMode mode) {
    return mode == SharedAirPathPresentationMode::alternate
               ? presentation_static_alternate_color
               : presentation_static_base_color;
}

constexpr HoldPresentationMode hold_presentation_mode(
    std::int32_t start_phase,
    std::int32_t path_phase) {
    if (path_phase == 2) {
        return HoldPresentationMode::animated;
    }
    if (start_phase == 4) {
        return path_phase == 3 ? HoldPresentationMode::alternate
                               : HoldPresentationMode::animated;
    }
    return HoldPresentationMode::base;
}

constexpr bool hold_root_and_far_markers_visible(
    std::int32_t start_phase) {
    return start_phase != 4;
}

constexpr bool hold_body_update_enabled(
    std::int32_t start_phase,
    std::int32_t path_phase) {
    return start_phase != 4 || path_phase != 4;
}

inline float hold_animated_resource_scale(std::uint32_t phase_counter) {
    const float phase = std::fmod(
        static_cast<float>(phase_counter) * 0.05F, 1.0F);
    return std::sin(phase * 6.2831855F) * 0.25F + 1.5F;
}

// Scene construction and gameplay-state entry both clear the scan/scene-exit
// bypass byte. Its complete reference set contains no nonzero writer.
inline constexpr bool runtime_materialization_scan_is_bypassed = false;

struct RuntimeMaterializationProbe {
    float raw_delta{};
    float adjusted_delta{};
    float positive_delta_projection_factor{1.0F};
};

// Gameplay setup starts from the scene's PlayOptionSpeedID (zero after fresh
// construction). The first matching SkillBefore unit of type 6 is decisive:
// a valid table ID replaces the current ID, while an invalid one prevents
// later units from being considered and leaves the current ID unchanged.
struct MaterializationSkillBeforeUnit {
    std::int32_t type{};
    std::uint32_t play_option_speed_id{};
};

struct PlayOptionSpeedTableRecord {
    double speed{};
};

constexpr std::uint32_t select_materialization_speed_id(
    std::uint32_t current_id,
    std::span<const MaterializationSkillBeforeUnit> skill_before_units,
    std::size_t play_option_speed_table_count) {
    for (const auto& unit : skill_before_units) {
        if (unit.type != 6) {
            continue;
        }
        return unit.play_option_speed_id < play_option_speed_table_count
                   ? unit.play_option_speed_id
                   : current_id;
    }
    return current_id;
}

inline float resolve_materialization_runtime_speed(
    std::uint32_t current_id,
    std::span<const MaterializationSkillBeforeUnit> skill_before_units,
    std::span<const PlayOptionSpeedTableRecord> speed_table) {
    const std::uint32_t selected_id = select_materialization_speed_id(
        current_id, skill_before_units, speed_table.size());
    const float selected =
        selected_id < speed_table.size()
            ? static_cast<float>(speed_table[selected_id].speed)
            : 0.0F;

    // The snapshot converts the table double to float, then uses MAXSS with
    // 0.1F as the source operand. This expression preserves the observed
    // clamp, including selecting 0.1F for NaN.
    return selected > 0.1F ? selected : 0.1F;
}

// The projection base offset is the float selected by the configuration key
// [OFFSET] DRAW. Its descriptor default is exactly 0.0F.
constexpr float resolve_materialization_projection_base_offset(
    bool offset_draw_present,
    float parsed_offset_draw) {
    return offset_draw_present ? parsed_offset_draw : 0.0F;
}

constexpr float runtime_materialization_projected_position(
    const RuntimeMaterializationProbe& probe,
    float runtime_speed,
    float projection_base_offset,
    float fixed_scale = runtime_materialization_fixed_scale) {
    const float factor = probe.adjusted_delta > 0.0F
                             ? probe.positive_delta_projection_factor
                             : 1.0F;
    float projected = fixed_scale *
                      (projection_base_offset + probe.adjusted_delta);
    projected *= runtime_speed;
    projected *= factor;
    return runtime_materialization_projection_origin - projected;
}

constexpr bool runtime_materialization_probe_is_eligible(
    const RuntimeMaterializationProbe& probe,
    float runtime_speed,
    float projection_base_offset,
    float fixed_scale = runtime_materialization_fixed_scale) {
    if (runtime_materialization_near_delta <= probe.raw_delta) {
        const float projected = runtime_materialization_projected_position(
            probe, runtime_speed, projection_base_offset, fixed_scale);
        if (projected < runtime_materialization_projection_lower ||
            runtime_materialization_projection_upper < projected) {
            return false;
        }
    }
    return true;
}

constexpr bool should_materialize_runtime_record(
    const RuntimeMaterializationProbe& start,
    bool has_endpoint,
    const RuntimeMaterializationProbe& endpoint,
    float runtime_speed,
    float projection_base_offset,
    float fixed_scale = runtime_materialization_fixed_scale) {
    if (runtime_materialization_probe_is_eligible(
            start, runtime_speed, projection_base_offset, fixed_scale)) {
        return true;
    }
    return has_endpoint && runtime_materialization_probe_is_eligible(
                               endpoint, runtime_speed,
                               projection_base_offset, fixed_scale);
}

// The primary factory has no standalone cases for attachment types 3, 5, and
// 8 or for region-only type 12. An eligible pending record is consumed even
// when this function is false.
constexpr bool runtime_factory_constructs_primary(std::int32_t parsed_type) {
    switch (parsed_type) {
    case 0:
    case 1:
    case 2:
    case 4:
    case 6:
    case 9:
    case 10:
    case 11:
    case 13:
        return true;
    default:
        return false;
    }
}

// claim.interactions.cross-family-candidate-result-order
// claim.presentation.cross-family-update-composition
//
// Existing objects stay ahead of newly materialized objects in each of the two
// active vectors. One pending scan visits eligible indices in queue order; a
// supported root appends to the primary vector and its optional attachment
// appends to a distinct secondary vector. An eligible factory-default record
// appends nothing. The manager never interleaves those vectors: every primary
// update precedes every attached-secondary update in a substep.
enum class RuntimeAppendRole : std::uint8_t {
    primary,
    attached_secondary,
};

struct RuntimeAppendEvent {
    std::int32_t parsed_index{};
    RuntimeAppendRole role{RuntimeAppendRole::primary};

    constexpr bool operator==(const RuntimeAppendEvent&) const = default;
};

struct RuntimeActiveVectors {
    std::vector<std::int32_t> primaries;
    std::vector<std::int32_t> attached_secondaries;
};

inline void append_runtime_factory_events(
    RuntimeActiveVectors& active,
    std::int32_t parsed_index,
    bool primary_constructed,
    bool attached_secondary_constructed) {
    if (!primary_constructed) {
        return;
    }
    active.primaries.push_back(parsed_index);
    if (attached_secondary_constructed) {
        active.attached_secondaries.push_back(parsed_index);
    }
}

inline std::vector<RuntimeAppendEvent> runtime_manager_update_order(
    const RuntimeActiveVectors& active) {
    std::vector<RuntimeAppendEvent> order;
    order.reserve(active.primaries.size() +
                  active.attached_secondaries.size());
    for (const std::int32_t parsed_index : active.primaries) {
        order.push_back({parsed_index, RuntimeAppendRole::primary});
    }
    for (const std::int32_t parsed_index : active.attached_secondaries) {
        order.push_back(
            {parsed_index, RuntimeAppendRole::attached_secondary});
    }
    return order;
}

enum class CrossFamilyOuterEventKind : std::uint8_t {
    primary_update,
    attached_secondary_update,
    effect_list_update,
    pending_materialization,
    post_manager_scene_export,
};

struct CrossFamilyOuterEvent {
    CrossFamilyOuterEventKind kind{CrossFamilyOuterEventKind::primary_update};
    std::int32_t index{};

    constexpr bool operator==(const CrossFamilyOuterEvent&) const = default;
};

// The ordinary path may run multiple catch-up substeps; the alternate path
// supplies one. Both paths update the ten EffectManager lists exactly once
// after all note substeps, then materialize pending records, then export the
// already-computed presentation state. Newly materialized records therefore
// do not occur in this trace until the next outer update.
inline std::vector<CrossFamilyOuterEvent>
build_cross_family_outer_update_schedule(
    const RuntimeActiveVectors& active,
    std::size_t manager_substeps) {
    std::vector<CrossFamilyOuterEvent> events;
    events.reserve(
        manager_substeps *
            (active.primaries.size() +
             active.attached_secondaries.size()) +
        12U);
    for (std::size_t substep = 0; substep < manager_substeps; ++substep) {
        for (const std::int32_t parsed_index : active.primaries) {
            events.push_back(
                {CrossFamilyOuterEventKind::primary_update, parsed_index});
        }
        for (const std::int32_t parsed_index :
             active.attached_secondaries) {
            events.push_back({
                CrossFamilyOuterEventKind::attached_secondary_update,
                parsed_index});
        }
    }
    for (std::int32_t list_index = 0; list_index < 10; ++list_index) {
        events.push_back(
            {CrossFamilyOuterEventKind::effect_list_update, list_index});
    }
    events.push_back(
        {CrossFamilyOuterEventKind::pending_materialization, 0});
    events.push_back(
        {CrossFamilyOuterEventKind::post_manager_scene_export, 0});
    return events;
}

constexpr float make_raw_play_position(float music_cursor_base_milliseconds,
                                       std::uint32_t elapsed_milliseconds) {
    return music_cursor_base_milliseconds * chart_units_per_millisecond +
           static_cast<float>(elapsed_milliseconds) *
               chart_units_per_millisecond;
}

enum class PlayClockMode : std::uint8_t {
    ordinary,
    alternate,
};

// The source applies this after its mode-specific quantization. The strict
// comparison is intentional: a discrepancy equal to 0.1 is left alone.
inline float correct_play_position_discrepancy(float raw_position,
                                               float quantized_position) {
    constexpr float threshold = 0.1F;
    constexpr float step = 0.01F;

    if (std::fabs(quantized_position - raw_position) > threshold) {
        if (raw_position - step <= quantized_position) {
            if (quantized_position <= raw_position + step) {
                quantized_position = raw_position;
            } else {
                quantized_position -= step;
            }
        } else {
            quantized_position += step;
        }
    }
    return quantized_position;
}

// Advances the smoothed clock for finite gameplay values. fixed_increment_mode
// represents the source record's paired flags without assigning an unsupported
// player-facing meaning to them.
inline float advance_smoothed_play_position(float raw_position,
                                            float smoothed_position,
                                            PlayClockMode mode,
                                            bool fixed_increment_mode = false) {
    const float bias = mode == PlayClockMode::ordinary ? 1.0F : 0.5F;

    if (fixed_increment_mode) {
        return smoothed_position + bias;
    }

    const float error = raw_position - smoothed_position;
    float next = smoothed_position;

    if (mode == PlayClockMode::ordinary) {
        if (0.5F <= bias + error) {
            if (error - bias <= 1.5F) {
                next += 1.0F;
            } else {
                next += std::floor((error - bias) + 0.5F);
            }
        } else {
            next += std::floor((bias + error) + 0.5F);
        }
    } else if (0.25F <= bias + error) {
        if (0.9F <= error && error < 1.25F) {
            // This branch converts to double, adds 0.8, then narrows to float.
            next = static_cast<float>(static_cast<double>(smoothed_position) +
                                      0.8);
        } else {
            const float adjusted = error - bias;
            if (adjusted <= 0.75F) {
                next += 0.5F;
            } else {
                next += std::floor(adjusted + adjusted + 0.5F) * 0.5F;
            }
        }
    } else {
        next += std::floor(error + error + bias * 2.0F + 0.5F) * 0.5F;
    }

    return correct_play_position_discrepancy(raw_position, next);
}

constexpr float integrate_play_position(float accumulated_position,
                                        float previous_smoothed_position,
                                        float smoothed_position,
                                        float runtime_rate_factor) {
    return accumulated_position +
           (smoothed_position - previous_smoothed_position) *
               runtime_rate_factor;
}

// claim.timing.play-timing-runtime-ownership
//
// ViewTimingManager initializes enabled false and both factors to 1.0. The
// guarded product is retained as an explicit interface even though no internal
// executable writer was recovered for a non-default header state.
struct ViewTimingRateState {
    bool enabled{};
    float factor_a{1.0F};
    float factor_b{1.0F};
};

constexpr float select_play_clock_rate(const ViewTimingRateState& state) {
    return state.enabled ? state.factor_a * state.factor_b : 1.0F;
}

// Only the gameplay-relevant field of the externally loaded 0x38-byte record
// is represented. The executable reads this double and the manager narrows it
// to float before composing judgement time.
struct PlayTimingOffsetRecord {
    double offset{};
};

constexpr float select_play_timing_offset(
    std::span<const PlayTimingOffsetRecord> records,
    std::uint32_t play_option_set_id) {
    if (play_option_set_id >= records.size()) {
        return 0.0F;
    }
    return static_cast<float>(records[play_option_set_id].offset);
}

// claim.timing.tempo-measure-schedule
//
// C2S positions are normalized to a pair whose scalar value is
// major * 4 + minor. With the executable-owned resolution of 384, one authored
// minor unit advances that scalar by 4 / 384. The RESOLUTION descriptor is
// accepted by tokenization but has no header-handler case in this snapshot;
// reset supplies 384 to every position parser.
struct ChartPosition {
    float major{};
    float minor{};
};

constexpr float chart_position_scalar(const ChartPosition& position) {
    return position.major * 4.0F + position.minor;
}

inline ChartPosition canonicalize_c2s_position(std::int32_t major,
                                               std::int32_t minor,
                                               std::int32_t resolution = 384) {
    if (resolution == 0) {
        return {};
    }

    const float authored =
        static_cast<float>(major) +
        static_cast<float>(minor) / static_cast<float>(resolution);
    const float rounded_grid =
        std::floor(authored * 4.0F * 384.0F + 0.5F) *
        (1.0F / 384.0F);
    const float canonical_major = std::floor(rounded_grid * 0.25F);
    return {canonical_major,
            rounded_grid - canonical_major * 4.0F};
}

// claim.note.air-solid-nonjudgement
//
// ASO is parsed as a two-endpoint type-10 record. Four anonymous float
// properties are converted to integer tenths by adding 0.5 before truncation;
// their player-facing meanings are not established. The final token uses the
// executable's shared, case-sensitive 16-entry color/style table.
inline constexpr std::array<std::string_view, 16> c2s_color_style_names{
    "DEF", "RED", "ORN", "YEL", "LIM", "GRN", "AQA", "CYN",
    "DGR", "BLU", "PPL", "VLT", "PNK", "GRY", "BLK", "NON",
};

constexpr std::int32_t parse_c2s_color_style_code(std::string_view name) {
    for (std::size_t index = 0; index < c2s_color_style_names.size(); ++index) {
        if (name == c2s_color_style_names[index]) {
            return static_cast<std::int32_t>(index);
        }
    }
    return 0;
}

constexpr std::int32_t quantize_c2s_aso_property(float value) {
    const float transformed = value * 10.0F + 0.5F;
    return cvttss2si_i32(transformed);
}

struct C2sAsoEndpoint {
    ChartPosition position{};
    std::int32_t lane{};
    std::int32_t width{1};
    std::int32_t property_a_tenths{};
    std::int32_t property_b_tenths{};
};

struct C2sAsoSegment {
    C2sAsoEndpoint start{};
    C2sAsoEndpoint end{};
    std::int32_t color_style_code{};
};

inline C2sAsoSegment parse_c2s_aso_record(
    std::span<const std::string_view> fields,
    bool mirrored = false,
    std::int32_t resolution = 384) {
    const std::int32_t major = parse_c2s_integer_field(fields, 0);
    const std::int32_t minor = parse_c2s_integer_field(fields, 1);
    std::int32_t start_lane = parse_c2s_integer_field(fields, 2);
    const std::int32_t start_width =
        std::clamp(parse_c2s_integer_field(fields, 3), 1, 16);
    const std::int32_t start_property_a =
        quantize_c2s_aso_property(parse_c2s_float_field(fields, 4));
    const std::int32_t start_property_b =
        quantize_c2s_aso_property(parse_c2s_float_field(fields, 5));
    const std::int32_t duration = parse_c2s_integer_field(fields, 6);
    std::int32_t end_lane = parse_c2s_integer_field(fields, 7);
    const std::int32_t end_width =
        std::clamp(parse_c2s_integer_field(fields, 8), 1, 16);
    const std::int32_t end_property_a =
        quantize_c2s_aso_property(parse_c2s_float_field(fields, 9));
    const std::int32_t end_property_b =
        quantize_c2s_aso_property(parse_c2s_float_field(fields, 10));

    if (mirrored) {
        start_lane = mirror_c2s_lane_origin(start_lane, start_width);
        end_lane = mirror_c2s_lane_origin(end_lane, end_width);
    }

    return {
        {canonicalize_c2s_position(major, minor, resolution),
         start_lane,
         start_width,
         start_property_a,
         start_property_b},
        {canonicalize_c2s_position(
             major, add_i32_wrapped(minor, duration), resolution),
         end_lane,
         end_width,
         end_property_a,
         end_property_b},
        parse_c2s_color_style_code(c2s_string_field(fields, 11)),
    };
}

// The event handler extends the first existing type-10 chain whose prior end
// agrees with the new start in style, lane span, both integer-tenth
// properties, and chart position. Position agreement is inclusive at the
// executable's 1/192 scalar epsilon.
constexpr bool c2s_aso_segments_connect(const C2sAsoSegment& existing,
                                        const C2sAsoSegment& next) {
    const float existing_end = chart_position_scalar(existing.end.position);
    const float next_start = chart_position_scalar(next.start.position);
    return existing.color_style_code == next.color_style_code &&
           existing.end.lane == next.start.lane &&
           existing.end.width == next.start.width &&
           existing.end.property_a_tenths == next.start.property_a_tenths &&
           existing.end.property_b_tenths == next.start.property_b_tenths &&
           next_start <= existing_end + (1.0F / 192.0F) &&
           existing_end <= next_start + (1.0F / 192.0F);
}

// claim.parser.sla-materialization-selection
//
// SLA is a parser-owned region directive rather than a runtime note. Its six
// numeric fields are major, minor, lane, width, duration, and tag. The source
// clamps width before applying the optional 16-lane mirror transform, and
// forms the end position by adding duration to the authored minor field.
struct C2sSlaRegion {
    ChartPosition start{};
    ChartPosition end{};
    std::int32_t lane{};
    std::int32_t width{1};
    std::int32_t tag{};
};

inline C2sSlaRegion parse_c2s_sla_record(
    std::span<const std::string_view> fields,
    bool mirrored = false,
    std::int32_t resolution = 384) {
    const std::int32_t major = parse_c2s_integer_field(fields, 0);
    const std::int32_t minor = parse_c2s_integer_field(fields, 1);
    std::int32_t lane = parse_c2s_integer_field(fields, 2);
    const std::int32_t width =
        std::clamp(parse_c2s_integer_field(fields, 3), 1, 16);
    const std::int32_t duration = parse_c2s_integer_field(fields, 4);
    const std::int32_t tag = parse_c2s_integer_field(fields, 5);
    if (mirrored) {
        lane = mirror_c2s_lane_origin(lane, width);
    }
    return {
        canonicalize_c2s_position(major, minor, resolution),
        canonicalize_c2s_position(
            major, add_i32_wrapped(minor, duration), resolution),
        lane,
        width,
        tag,
    };
}

// The ordinary selector shifts the query position forward by the same
// half-grid epsilon used by the parser's position comparisons. Regions are
// half-open in shifted chart time and must fully contain the queried lane
// span. Overlaps select the greatest positive tag; zero is the fallback.
constexpr std::int32_t select_c2s_sla_tag(
    const ChartPosition& position,
    std::int32_t lane,
    std::int32_t width,
    std::span<const C2sSlaRegion> regions) {
    const float shifted_position =
        chart_position_scalar(position) + (1.0F / 192.0F);
    std::int32_t selected = 0;
    for (const auto& region : regions) {
        if (selected < region.tag &&
            chart_position_scalar(region.start) <= shifted_position &&
            shifted_position < chart_position_scalar(region.end) &&
            region.lane <= lane &&
            add_i32_wrapped(lane, width) <=
                add_i32_wrapped(region.lane, region.width)) {
            selected = region.tag;
        }
    }
    return selected;
}

// AirLadder's interpolated spans use the same time test and maximum-tag rule,
// with executable-owned +/-0.00001F tolerance on the two lane boundaries.
constexpr std::int32_t select_c2s_sla_tag_for_float_span(
    const ChartPosition& position,
    float lane,
    float width,
    std::span<const C2sSlaRegion> regions) {
    const float shifted_position =
        chart_position_scalar(position) + (1.0F / 192.0F);
    std::int32_t selected = 0;
    for (const auto& region : regions) {
        if (selected < region.tag &&
            chart_position_scalar(region.start) <= shifted_position &&
            shifted_position < chart_position_scalar(region.end) &&
            -0.00001F <= lane - static_cast<float>(region.lane) &&
            lane + width -
                    static_cast<float>(
                        add_i32_wrapped(region.lane, region.width)) <=
                0.00001F) {
            selected = region.tag;
        }
    }
    return selected;
}

// claim.note.hold-extended-profile-selection
//
// HLD and HXD share parsed type 1. HXD adds a sixth string field and sets an
// extended-form flag even when that field is missing. The exact subtype table
// is case-sensitive; missing, empty, and unknown strings all use index zero.
enum class C2sHoldCommandForm : std::uint8_t {
    hld,
    hxd,
};

inline constexpr std::array<std::string_view, 8> c2s_hxd_subtype_names{
    "UP", "DW", "CE", "RC", "LC", "RS", "LS", "BS",
};

constexpr std::int32_t c2s_hxd_subtype_code(std::string_view name) {
    for (std::size_t index = 0; index < c2s_hxd_subtype_names.size();
         ++index) {
        if (c2s_hxd_subtype_names[index] == name) {
            return static_cast<std::int32_t>(index);
        }
    }
    return 0;
}

struct C2sHoldCommandVariant {
    bool extended_form{};
    std::int32_t subtype_code{};
};

struct C2sHoldGeometry {
    ChartPosition start{};
    ChartPosition end{};
    std::int32_t lane{};
    std::int32_t width{1};
    std::int32_t encoded_width{};
    std::int32_t duration{};
};

inline C2sHoldGeometry parse_c2s_hold_geometry(
    std::span<const std::string_view> fields,
    bool mirrored = false,
    std::int32_t resolution = 384) {
    const std::int32_t major = parse_c2s_integer_field(fields, 0);
    const std::int32_t minor = parse_c2s_integer_field(fields, 1);
    std::int32_t lane = parse_c2s_integer_field(fields, 2);
    const std::int32_t width =
        std::clamp(parse_c2s_integer_field(fields, 3), 1, 16);
    const std::int32_t duration = parse_c2s_integer_field(fields, 4);
    if (mirrored) {
        lane = mirror_c2s_lane_origin(lane, width);
    }
    return {
        canonicalize_c2s_position(major, minor, resolution),
        canonicalize_c2s_position(
            major, add_i32_wrapped(minor, duration), resolution),
        lane,
        width,
        encode_c2s_note_width(width),
        duration,
    };
}

inline C2sHoldCommandVariant parse_c2s_hold_command_variant(
    C2sHoldCommandForm form,
    std::span<const std::string_view> fields) {
    if (form == C2sHoldCommandForm::hxd) {
        return {
            true,
            c2s_hxd_subtype_code(c2s_string_field(fields, 5)),
        };
    }
    return {};
}

// The checker initializer begins with this executable-owned parsed-type map.
// Extended forms of HOLD, SLIDE, and HeavenHold override their ordinary
// selector with 4. Invalid parsed types retain the initializer's -1 sentinel.
constexpr std::int32_t note_checker_profile_selector(
    std::int32_t parsed_type,
    bool extended_form) {
    if (parsed_type < 0 || 13 < parsed_type) {
        return -1;
    }
    if (parsed_type == 4 ||
        (extended_form &&
         (parsed_type == 1 || parsed_type == 2 || parsed_type == 13))) {
        return 4;
    }
    if (parsed_type == 6) {
        return 6;
    }
    if (parsed_type == 11) {
        return 11;
    }
    return 0;
}

struct BpmScheduleRecord {
    ChartPosition position{};
    float scheduled_milliseconds{};
    std::uint32_t source_sequence{};
    float beats_per_minute{150.0F};
};

// claim.timing.tempo-measure-schedule
//
// MET stores beat-unit first and count second. The generated beat vector uses
// resolution/unit while the generated bar vector uses count*resolution/unit.
// The x86 operations retain low 32-bit multiplication and unsigned division;
// The current MET record is appended to both meter-derived vectors before the
// zero-component test. Zero components therefore retain that one anchor and
// stop generation before either division.
struct MeterGridSteps {
    bool components_nonzero{};
    std::uint32_t beat_ticks{};
    std::uint32_t bar_ticks{};
    bool current_anchor_retained{};
};

constexpr MeterGridSteps c2s_meter_grid_steps(
    std::int32_t beat_unit,
    std::int32_t count,
    std::int32_t resolution = 384) {
    if (beat_unit == 0 || count == 0) {
        return {false, 0U, 0U, true};
    }
    const std::uint32_t denominator =
        static_cast<std::uint32_t>(beat_unit);
    const std::uint32_t unsigned_resolution =
        static_cast<std::uint32_t>(resolution);
    const std::uint32_t numerator =
        static_cast<std::uint32_t>(count) * unsigned_resolution;
    return {
        true,
        unsigned_resolution / denominator,
        numerator / denominator,
        true,
    };
}

inline constexpr float chart_position_compare_epsilon = 1.0F / 192.0F;

constexpr bool bpm_record_precedes(const BpmScheduleRecord& left,
                                   const BpmScheduleRecord& right) {
    return chart_position_scalar(left.position) +
               chart_position_compare_epsilon <
           chart_position_scalar(right.position);
}

// The snapshot uses the 32-bit MSVC three-way introsort for both BPM records
// and keyed projection intervals. Keeping the compiled algorithm explicit
// also defines its deterministic behavior when the float comparator is
// unordered by NaN; host std::sort would have an invalid comparator contract.
template <typename Record, typename Precedes>
inline void snapshot_msvc_median_three(
    std::vector<Record>& records,
    std::size_t left,
    std::size_t middle,
    std::size_t right,
    Precedes precedes) {
    if (precedes(records[middle], records[left])) {
        std::swap(records[middle], records[left]);
    }
    if (precedes(records[right], records[middle])) {
        std::swap(records[right], records[middle]);
        if (precedes(records[middle], records[left])) {
            std::swap(records[middle], records[left]);
        }
    }
}

template <typename Record, typename Precedes>
inline void snapshot_msvc_guess_median(
    std::vector<Record>& records,
    std::size_t first,
    std::size_t last,
    Precedes precedes) {
    const std::size_t count = last - first;
    const std::size_t middle = first + count / 2U;
    const std::size_t right = last - 1U;
    if (count >= 42U) {
        const std::size_t step = count / 8U;
        snapshot_msvc_median_three(
            records, first, first + step, first + step * 2U, precedes);
        snapshot_msvc_median_three(
            records, middle - step, middle, middle + step, precedes);
        snapshot_msvc_median_three(
            records, right - step * 2U, right - step, right, precedes);
        snapshot_msvc_median_three(
            records, first + step, middle, right - step, precedes);
        return;
    }
    snapshot_msvc_median_three(records, first, middle, right, precedes);
}

struct SnapshotSortPartition {
    std::size_t equal_first{};
    std::size_t equal_last{};
};

template <typename Record, typename Precedes>
inline SnapshotSortPartition snapshot_msvc_partition(
    std::vector<Record>& records,
    std::size_t first,
    std::size_t last,
    Precedes precedes) {
    snapshot_msvc_guess_median(records, first, last, precedes);
    const std::size_t middle = first + (last - first) / 2U;
    std::size_t equal_first = middle;
    std::size_t equal_last = middle + 1U;

    while (first < equal_first &&
           !precedes(records[equal_first - 1U], records[equal_first]) &&
           !precedes(records[equal_first], records[equal_first - 1U])) {
        --equal_first;
    }
    while (equal_last < last &&
           !precedes(records[equal_last], records[equal_first]) &&
           !precedes(records[equal_first], records[equal_last])) {
        ++equal_last;
    }

    std::size_t scan_right = equal_last;
    std::size_t scan_left = equal_first;
    for (;;) {
        for (; scan_right < last; ++scan_right) {
            if (precedes(records[equal_first], records[scan_right])) {
                continue;
            }
            if (precedes(records[scan_right], records[equal_first])) {
                break;
            }
            if (equal_last != scan_right) {
                std::swap(records[equal_last], records[scan_right]);
            }
            ++equal_last;
        }

        for (; first < scan_left; --scan_left) {
            const std::size_t candidate = scan_left - 1U;
            if (precedes(records[candidate], records[equal_first])) {
                continue;
            }
            if (precedes(records[equal_first], records[candidate])) {
                break;
            }
            --equal_first;
            if (equal_first != candidate) {
                std::swap(records[equal_first], records[candidate]);
            }
        }

        if (scan_left == first) {
            if (scan_right == last) {
                return {equal_first, equal_last};
            }
            if (equal_last != scan_right) {
                std::swap(records[equal_last], records[scan_right]);
            }
            ++equal_last;
            ++scan_right;
        } else if (scan_right == last) {
            --scan_left;
            --equal_first;
            if (scan_left != equal_first) {
                std::swap(records[scan_left], records[equal_first]);
            }
            --equal_last;
            if (equal_first != equal_last) {
                std::swap(records[equal_first], records[equal_last]);
            }
        } else {
            --scan_left;
            std::swap(records[scan_right], records[scan_left]);
            ++scan_right;
        }
    }
}

template <typename Record, typename Precedes>
inline void snapshot_msvc_insertion_sort(
    std::vector<Record>& records,
    std::size_t first,
    std::size_t last,
    Precedes precedes) {
    for (std::size_t next = first + 1U; next < last; ++next) {
        Record value = records[next];
        if (precedes(value, records[first])) {
            for (std::size_t hole = next; hole > first; --hole) {
                records[hole] = records[hole - 1U];
            }
            records[first] = value;
            continue;
        }

        std::size_t hole = next;
        while (precedes(value, records[hole - 1U])) {
            records[hole] = records[hole - 1U];
            --hole;
        }
        records[hole] = value;
    }
}

template <typename Record, typename Precedes>
inline void snapshot_msvc_push_heap(
    std::vector<Record>& records,
    std::size_t first,
    std::size_t hole,
    std::size_t top,
    Record value,
    Precedes precedes) {
    while (top < hole) {
        const std::size_t parent = (hole - 1U) / 2U;
        if (!precedes(records[first + parent], value)) {
            break;
        }
        records[first + hole] = records[first + parent];
        hole = parent;
    }
    records[first + hole] = value;
}

template <typename Record, typename Precedes>
inline void snapshot_msvc_pop_heap_hole(
    std::vector<Record>& records,
    std::size_t first,
    std::size_t hole,
    std::size_t bottom,
    Record value,
    Precedes precedes) {
    const std::size_t top = hole;
    const std::size_t last_parent = (bottom - 1U) / 2U;
    while (hole < last_parent) {
        std::size_t child = hole * 2U + 2U;
        if (precedes(records[first + child],
                     records[first + child - 1U])) {
            --child;
        }
        records[first + hole] = records[first + child];
        hole = child;
    }
    if (hole == last_parent && bottom % 2U == 0U) {
        records[first + hole] = records[first + bottom - 1U];
        hole = bottom - 1U;
    }
    snapshot_msvc_push_heap(
        records, first, hole, top, value, precedes);
}

template <typename Record, typename Precedes>
inline void snapshot_msvc_heap_sort(
    std::vector<Record>& records,
    std::size_t first,
    std::size_t last,
    Precedes precedes) {
    const std::size_t count = last - first;
    for (std::size_t hole = count / 2U; hole > 0U;) {
        --hole;
        const Record value = records[first + hole];
        snapshot_msvc_pop_heap_hole(
            records, first, hole, count, value, precedes);
    }
    for (std::size_t bottom = count; bottom > 1U;) {
        --bottom;
        const Record value = records[first + bottom];
        records[first + bottom] = records[first];
        snapshot_msvc_pop_heap_hole(
            records, first, 0U, bottom, value, precedes);
    }
}

template <typename Record, typename Precedes>
inline void snapshot_msvc_sort_range(
    std::vector<Record>& records,
    std::size_t first,
    std::size_t last,
    std::size_t ideal,
    Precedes precedes) {
    for (;;) {
        const std::size_t count = last - first;
        if (count < 33U) {
            if (count > 1U) {
                snapshot_msvc_insertion_sort(
                    records, first, last, precedes);
            }
            return;
        }
        if (ideal < 1U) {
            snapshot_msvc_heap_sort(records, first, last, precedes);
            return;
        }

        const SnapshotSortPartition partition =
            snapshot_msvc_partition(records, first, last, precedes);
        ideal = ideal / 2U + (ideal / 2U) / 2U;
        if (partition.equal_first - first <
            last - partition.equal_last) {
            snapshot_msvc_sort_range(
                records, first, partition.equal_first, ideal, precedes);
            first = partition.equal_last;
        } else {
            snapshot_msvc_sort_range(
                records, partition.equal_last, last, ideal, precedes);
            last = partition.equal_first;
        }
    }
}

template <typename Record, typename Precedes>
inline void snapshot_msvc_sort(
    std::vector<Record>& records,
    Precedes precedes) {
    snapshot_msvc_sort_range(
        records, 0U, records.size(), records.size(), precedes);
}

inline void snapshot_bpm_heap_sort(
    std::vector<BpmScheduleRecord>& records,
    std::size_t first,
    std::size_t last) {
    snapshot_msvc_heap_sort(
        records, first, last, bpm_record_precedes);
}

inline void snapshot_bpm_sort(
    std::vector<BpmScheduleRecord>& records) {
    snapshot_msvc_sort(records, bpm_record_precedes);
}

struct QuantizedScheduleDelta {
    float scalar{};
    float whole_major{};
};

// The source first rounds to a 1/384 scalar grid, then splits the value at
// whole-major boundaries before recombining it. Keeping both components lets
// the two source call sites preserve their opposite addition order.
inline QuantizedScheduleDelta quantize_schedule_delta(float scalar_delta) {
    const float scalar =
        std::floor(scalar_delta * 384.0F + 0.5F) *
        (1.0F / 384.0F);
    return {scalar, std::floor(scalar * 0.25F) * 4.0F};
}

// BPM records are sorted before any ordinary C2S event is parsed. The first
// interval starts at chart position zero and uses the first sorted record's
// BPM; each later interval uses the preceding record's BPM.
inline void finalize_bpm_schedule(std::vector<BpmScheduleRecord>& records) {
    if (records.empty()) {
        return;
    }

    snapshot_bpm_sort(records);

    ChartPosition previous_position{};
    float previous_bpm = records.front().beats_per_minute;
    float cumulative = 0.0F;
    for (auto& record : records) {
        const auto delta = quantize_schedule_delta(
            chart_position_scalar(record.position) -
            chart_position_scalar(previous_position));
        const float recombined =
            delta.whole_major + (delta.scalar - delta.whole_major);
        cumulative =
            recombined * 60000.0F / previous_bpm + cumulative;
        record.scheduled_milliseconds = cumulative;
        previous_position = record.position;
        previous_bpm = record.beats_per_minute;
    }
}

// Selects the latest BPM position no later than target (allowing the source's
// half-grid epsilon), then extends that record's cumulative millisecond value.
// Empty maps and targets before the first qualifying record return zero.
inline float schedule_at_chart_position(
    const ChartPosition& target,
    std::span<const BpmScheduleRecord> records) {
    const float target_scalar = chart_position_scalar(target);
    for (auto record = records.rbegin(); record != records.rend(); ++record) {
        const float record_scalar = chart_position_scalar(record->position);
        if (record_scalar <=
            target_scalar + chart_position_compare_epsilon) {
            const auto delta =
                quantize_schedule_delta(target_scalar - record_scalar);
            const float recombined =
                (delta.scalar - delta.whole_major) + delta.whole_major;
            return recombined * 60000.0F / record->beats_per_minute +
                   record->scheduled_milliseconds;
        }
    }
    return 0.0F;
}

// claim.timing.projection-schedule-materialization
//
// STP, SFL, and SLP produce keyed intervals. STP and SFL use key zero;
// SLP supplies its key explicitly. DCM records remain in source order in a
// separate factor vector. CLK is retained here only to close the group-1
// parser shape; its click schedule has no recovered gameplay consumer. SFE is
// registered but deliberately has no group-1 handler case.
enum class C2sProjectionCommandDisposition : std::uint8_t {
    unrecognized,
    recognized_but_rejected,
    keyed_interval,
    factor_interval,
    presentation_click,
};

struct ProjectionScheduleInterval {
    ChartPosition start{};
    float start_milliseconds{};
    std::uint32_t source_sequence{};
    float factor{};
    ChartPosition end{};
    float end_milliseconds{};
};

struct ProjectionClickRecord {
    ChartPosition position{};
    float scheduled_milliseconds{};
    std::uint32_t source_sequence{};
};

struct C2sProjectionSchedule {
    std::map<std::int32_t, std::vector<ProjectionScheduleInterval>>
        keyed_intervals;
    std::vector<ProjectionScheduleInterval> factor_intervals;
    std::vector<ProjectionClickRecord> presentation_clicks;

    void reset() {
        keyed_intervals.clear();
        factor_intervals.clear();
        presentation_clicks.clear();
    }
};

inline C2sProjectionCommandDisposition apply_c2s_projection_command(
    C2sProjectionSchedule& schedule,
    std::string_view command,
    std::span<const std::string_view> fields,
    std::span<const BpmScheduleRecord> bpm_records,
    std::uint32_t source_sequence,
    std::int32_t resolution = 384) {
    if (command == "SFE") {
        return C2sProjectionCommandDisposition::recognized_but_rejected;
    }

    if (command == "CLK") {
        const ChartPosition position = canonicalize_c2s_position(
            parse_c2s_integer_field(fields, 0),
            parse_c2s_integer_field(fields, 1),
            resolution);
        schedule.presentation_clicks.push_back({
            position,
            schedule_at_chart_position(position, bpm_records),
            source_sequence,
        });
        return C2sProjectionCommandDisposition::presentation_click;
    }

    const bool is_stp = command == "STP";
    const bool is_sfl = command == "SFL";
    const bool is_slp = command == "SLP";
    const bool is_dcm = command == "DCM";
    if (!is_stp && !is_sfl && !is_slp && !is_dcm) {
        return C2sProjectionCommandDisposition::unrecognized;
    }

    const std::int32_t major = parse_c2s_integer_field(fields, 0);
    const std::int32_t minor = parse_c2s_integer_field(fields, 1);
    const std::int32_t duration = parse_c2s_integer_field(fields, 2);
    const ChartPosition start =
        canonicalize_c2s_position(major, minor, resolution);
    const ChartPosition end = canonicalize_c2s_position(
        major, add_i32_wrapped(minor, duration), resolution);
    const float factor =
        is_stp ? 0.0F : parse_c2s_float_field(fields, 3);
    const ProjectionScheduleInterval interval{
        start,
        schedule_at_chart_position(start, bpm_records),
        source_sequence,
        factor,
        end,
        schedule_at_chart_position(end, bpm_records),
    };

    if (is_dcm) {
        schedule.factor_intervals.push_back(interval);
        return C2sProjectionCommandDisposition::factor_interval;
    }

    const std::int32_t key =
        is_slp ? parse_c2s_integer_field(fields, 4) : 0;
    schedule.keyed_intervals[key].push_back(interval);
    return C2sProjectionCommandDisposition::keyed_interval;
}

constexpr bool projection_interval_precedes(
    const ProjectionScheduleInterval& left,
    const ProjectionScheduleInterval& right) {
    return chart_position_scalar(left.start) +
               chart_position_compare_epsilon <
           chart_position_scalar(right.start);
}

inline void finalize_c2s_projection_schedule(
    C2sProjectionSchedule& schedule) {
    for (auto& [key, intervals] : schedule.keyed_intervals) {
        (void)key;
        snapshot_msvc_sort(intervals, projection_interval_precedes);
    }
}

// Materialization uses the source helper's forward-only mode. Missing keys and
// a backwards query leave the target unchanged. Every overlapping interval
// contributes independently, including overlaps with earlier intervals.
inline float adjust_projection_target_milliseconds(
    const C2sProjectionSchedule& schedule,
    std::int32_t key,
    float from_milliseconds,
    float target_milliseconds) {
    if (target_milliseconds < from_milliseconds) {
        return target_milliseconds;
    }
    const auto found = schedule.keyed_intervals.find(key);
    if (found == schedule.keyed_intervals.end()) {
        return target_milliseconds;
    }

    float adjusted = target_milliseconds;
    for (const auto& interval : found->second) {
        if (!(interval.start_milliseconds < target_milliseconds)) {
            break;
        }
        if (from_milliseconds < interval.end_milliseconds) {
            const float overlap_start =
                std::max(from_milliseconds, interval.start_milliseconds);
            const float overlap_end =
                std::min(target_milliseconds, interval.end_milliseconds);
            if (overlap_start <= overlap_end) {
                adjusted =
                    (overlap_end - overlap_start) *
                        (interval.factor - 1.0F) +
                    adjusted;
            }
        }
    }
    return adjusted;
}

// DCM lookup intentionally preserves source order. The query is shifted by
// exactly 1.0 millisecond; the first enclosing nonzero interval wins, a future
// start stops the scan, and the fallback is 1.0.
constexpr float projection_factor_at_milliseconds(
    float query_milliseconds,
    std::span<const ProjectionScheduleInterval> factor_intervals) {
    const float shifted_query = query_milliseconds + 1.0F;
    for (const auto& interval : factor_intervals) {
        if (shifted_query < interval.start_milliseconds) {
            break;
        }
        if (shifted_query < interval.end_milliseconds &&
            interval.factor != 0.0F) {
            return interval.factor;
        }
    }
    return 1.0F;
}

// Compose parsed projection schedules with the exact far-path conversion used
// by runtime materialization. The initial raw shortcut still precedes this
// adjusted result when the returned probe is evaluated.
inline RuntimeMaterializationProbe runtime_materialization_probe_from_schedule(
    float scheduled_milliseconds,
    std::int32_t projection_key,
    float manager_position,
    const C2sProjectionSchedule& schedule) {
    const float scaled_target =
        scheduled_milliseconds * chart_units_per_millisecond;
    RuntimeMaterializationProbe probe{
        scaled_target - manager_position,
        scaled_target - manager_position,
        1.0F,
    };

    if (projection_key >= 0) {
        const float adjusted_target_milliseconds =
            adjust_projection_target_milliseconds(
                schedule,
                projection_key,
                manager_position * periodic_milliseconds_per_chart_unit,
                scaled_target * periodic_milliseconds_per_chart_unit);
        probe.adjusted_delta =
            adjusted_target_milliseconds * chart_units_per_millisecond -
            manager_position;
    }
    if (probe.adjusted_delta > 0.0F) {
        const float factor_query =
            (manager_position + probe.adjusted_delta) *
            periodic_milliseconds_per_chart_unit;
        probe.positive_delta_projection_factor =
            projection_factor_at_milliseconds(
                factor_query, schedule.factor_intervals);
    }
    return probe;
}

// Sustained-note updates call the common projection helper separately for the
// root and every authored/generated path endpoint. This adapter deliberately
// accepts one endpoint at a time: keyed adjustment and DCM lookup belong to
// that endpoint's schedule/key, not to an entity-wide body factor. Mesh
// vertices subsequently interpolate the already projected endpoints.
inline float sustain_endpoint_projected_depth_from_schedule(
    float scheduled_milliseconds,
    std::int32_t projection_key,
    float manager_position,
    float runtime_speed,
    float projection_base_offset,
    const C2sProjectionSchedule& schedule) {
    const RuntimeMaterializationProbe probe =
        runtime_materialization_probe_from_schedule(
            scheduled_milliseconds, projection_key, manager_position,
            schedule);
    return active_note_projected_depth(
        probe.adjusted_delta,
        runtime_speed,
        projection_base_offset,
        probe.positive_delta_projection_factor,
        true,
        true);
}

// AirHold, AirSlide, and HeavenHold select BPM by the already-computed
// scheduled millisecond value. The source assumes a nonempty map and
// dereferences its first record as the before-first fallback.
enum class ScheduledBpmSelectionDisposition : std::uint8_t {
    selected,
    source_empty_map_dereference,
};

struct ScheduledBpmSelection {
    ScheduledBpmSelectionDisposition disposition{
        ScheduledBpmSelectionDisposition::source_empty_map_dereference};
    float beats_per_minute{};
};

constexpr ScheduledBpmSelection evaluate_bpm_at_scheduled_position(
    float scheduled_milliseconds,
    std::span<const BpmScheduleRecord> records) {
    for (auto record = records.rbegin(); record != records.rend(); ++record) {
        if (record->scheduled_milliseconds <= scheduled_milliseconds) {
            return {
                ScheduledBpmSelectionDisposition::selected,
                record->beats_per_minute,
            };
        }
    }
    if (records.empty()) {
        return {};
    }
    return {
        ScheduledBpmSelectionDisposition::selected,
        records.front().beats_per_minute,
    };
}

// Convenience for callers that have already established the nonempty source
// precondition. The zero value is a clean-room sentinel for an invalid empty
// map, not a fallback present in the executable.
constexpr float bpm_at_scheduled_position(
    float scheduled_milliseconds,
    std::span<const BpmScheduleRecord> records) {
    return evaluate_bpm_at_scheduled_position(
               scheduled_milliseconds, records)
        .beats_per_minute;
}

// claim.timing.gameplay-substep-order
//
// The normal update loop always processes offset zero and includes only one
// prior offset even when the external update counter is farther behind.
constexpr std::int32_t first_catch_up_offset(
    std::uint32_t previous_counter,
    std::uint32_t current_counter) {
    const std::uint32_t difference_bits =
        previous_counter - current_counter + 1U;
    const std::int32_t signed_difference =
        std::bit_cast<std::int32_t>(difference_bits);
    return std::clamp(signed_difference, -1, 0);
}

// The selected runtime correction is intentionally an argument: its external
// table values are not present in the workspace.
constexpr float make_manager_judgement_position(
    float accumulated_play_position,
    float substep_offset,
    float manager_base_offset,
    float selected_runtime_correction) {
    return manager_base_offset + accumulated_play_position + substep_offset -
           selected_runtime_correction;
}

// claim.input.hold-source-continuation
//
// A HOLD tracks the two physical source banks separately for every logical
// lane. A source becomes armed after it has been observed released. A marker
// written by a sustaining HOLD in the preceding input snapshot instead arms
// every currently held source for that logical lane; this preserves the
// executable's cross-note/source-bank hand-off rule.
struct HoldLaneSourceState {
    std::array<bool, 2> armed{};
    std::array<bool, 2> active{};
};

constexpr bool update_hold_lane_sources(
    HoldLaneSourceState& state,
    const std::array<bool, 2>& current_level,
    const std::array<bool, 2>& previous_sustain_marker,
    bool sustain_started,
    std::array<bool, 2>& current_sustain_marker) {
    bool continues_marked_source = false;
    for (std::size_t source = 0; source < current_level.size(); ++source) {
        if (current_level[source] && previous_sustain_marker[source]) {
            continues_marked_source = true;
            break;
        }
    }

    for (std::size_t source = 0; source < current_level.size(); ++source) {
        state.armed[source] =
            state.armed[source] |
            (continues_marked_source ? current_level[source]
                                     : !current_level[source]);
    }

    bool any_active = false;
    for (std::size_t source = 0; source < current_level.size(); ++source) {
        state.active[source] =
            sustain_started && state.armed[source] && current_level[source];
        if (state.active[source]) {
            current_sustain_marker[source] = true;
            any_active = true;
        }
    }
    return any_active;
}

// claim.note.hold-sustain-gap-lifecycle
//
// Four thresholds are loaded for HOLD records in this snapshot. They stay
// explicit because their values are external configuration, not executable
// constants.
struct HoldGapState {
    float update_gate{};
    float participation_floor{};
    float end{};
    std::array<float, 4> thresholds{};
    bool ever_active{};
    float last_active{};
    float current{};
    float maximum_gap{};
};

constexpr std::int32_t hold_gap_index(bool ever_active,
                                      float gap,
                                      const std::array<float, 4>& thresholds) {
    if (ever_active) {
        for (std::size_t index = 0; index < thresholds.size(); ++index) {
            if (gap <= thresholds[index]) {
                return static_cast<std::int32_t>(index);
            }
        }
    }
    return static_cast<std::int32_t>(thresholds.size());
}

// Updates the cumulative maximum inactive gap and returns the current
// anonymous threshold index. -1 means the elapsed position has not reached
// the configured update gate.
constexpr std::int32_t update_hold_gap(HoldGapState& state,
                                       float elapsed,
                                       bool active) {
    if (elapsed < state.update_gate) {
        return -1;
    }

    const float clamped = elapsed <= state.end ? elapsed : state.end;
    state.ever_active = state.ever_active | active;
    state.current = clamped;
    if (active) {
        state.last_active = clamped;
    }

    const float baseline = state.last_active <= state.participation_floor
                               ? state.participation_floor
                               : state.last_active;
    const float gap = clamped - baseline;
    if (state.maximum_gap <= gap) {
        state.maximum_gap = gap;
    }
    return hold_gap_index(state.ever_active, gap, state.thresholds);
}

// Classifies the retained maximum gap at a HOLD checkpoint. -1 denotes a
// checkpoint before the participation floor; a never-active HOLD maps to the
// one-past-last threshold index.
constexpr std::int32_t classify_hold_gap(const HoldGapState& state) {
    if (state.current < state.participation_floor) {
        return -1;
    }
    return hold_gap_index(state.ever_active, state.maximum_gap,
                          state.thresholds);
}

// After emitting a scheduled checkpoint, the source keeps an ongoing inactive
// streak but discards any older, completed worst gap.
constexpr void reset_hold_gap_after_checkpoint(HoldGapState& state) {
    const float baseline = state.last_active <= state.participation_floor
                               ? state.participation_floor
                               : state.last_active;
    state.maximum_gap = state.current - baseline;
}

// Converts the threshold index to the anonymous byte used by the HOLD result
// paths. Player-facing names remain intentionally unresolved.
constexpr std::uint8_t hold_gap_result_byte(std::int32_t index) {
    switch (index) {
        case 0:
            return 4;
        case 1:
            return 3;
        case 2:
            return 2;
        case 3:
            return 1;
        default:
            return 0;
    }
}

// claim.note.air-hold-secondary-judgement
//
// AHD and AHX attach secondary parsed type 5 to an existing supported root.
// When another type-5 command extends that secondary, the previously saved
// endpoint is appended to the authored-checkpoint vector only if it came from
// AHX. The final saved endpoint follows the same rule during runtime loading.
enum class AirHoldCommand : std::uint8_t {
    ahd,
    ahx,
};

// The AHD/AHX case reads the six data fields through duration. Descriptor
// arity checking is disabled, so later fields such as corpus DEF/PNK labels
// are accepted but never read by this command handler.
inline constexpr std::size_t air_hold_consumed_data_field_count = 6U;

constexpr bool air_hold_data_field_is_consumed(std::size_t index) {
    return index < air_hold_consumed_data_field_count;
}

constexpr bool air_hold_is_authored_checkpoint(AirHoldCommand command) {
    return command == AirHoldCommand::ahx;
}

constexpr std::size_t air_hold_runtime_checkpoint_count(
    std::size_t saved_ahx_count,
    AirHoldCommand final_command) {
    return saved_ahx_count +
           (air_hold_is_authored_checkpoint(final_command) ? 1U : 0U);
}

// Chart postprocessing converts an AirHold anchor chain to generated path
// records on a 384-tick major-unit grid. Position interpolation remains a
// caller-supplied path service; callers can compose schedule_at_chart_position
// and bpm_at_scheduled_position for the now-reconstructed timing services.
struct AirHoldPathPoint {
    float major{};
    float minor{};
    float scheduled{};
};

enum class AirHoldGeneratedRecordKind : std::int32_t {
    sample = 5,
    segment_boundary = 6,
    path_end = 7,
};

struct AirHoldGeneratedPathRecord {
    AirHoldPathPoint point{};
    AirHoldGeneratedRecordKind kind{};
    bool emission_enabled{};
};

// The binary performs this operation in single precision and converts with
// CVTTSS2SI. Malformed nonfinite/out-of-range positions become INT32_MIN.
inline std::int32_t air_hold_grid_tick(const AirHoldPathPoint& point) {
    return cvttss2si_i32(
        (point.major + point.minor * 0.25F) * 384.0F + 0.5F);
}

enum class AirHoldPathGenerationDisposition : std::uint8_t {
    no_anchors,
    generated,
    source_large_unsigned_span_expansion,
};

constexpr std::uint32_t air_path_unsigned_tick_delta(
    std::int32_t start_tick,
    std::int32_t end_tick) {
    return static_cast<std::uint32_t>(end_tick) -
           static_cast<std::uint32_t>(start_tick);
}

// Type-5 duration fields are signed and have no nonnegative parser check.
// The producer forms its delta with wrapped i32 subtraction and compares it as
// uint32. A delta with its high bit set enters the source's very large unsigned
// generation domain regardless of the signed ordering of the endpoint ticks.
constexpr AirHoldPathGenerationDisposition
evaluate_air_hold_path_generation(
    std::span<const AirHoldPathPoint> anchors) {
    if (anchors.empty()) {
        return AirHoldPathGenerationDisposition::no_anchors;
    }
    for (std::size_t index = 1; index < anchors.size(); ++index) {
        const std::uint32_t delta = air_path_unsigned_tick_delta(
            air_hold_grid_tick(anchors[index - 1U]),
            air_hold_grid_tick(anchors[index]));
        if (delta >
            static_cast<std::uint32_t>(
                std::numeric_limits<std::int32_t>::max())) {
            return AirHoldPathGenerationDisposition::
                source_large_unsigned_span_expansion;
        }
    }
    return AirHoldPathGenerationDisposition::generated;
}

// Sampling begins at 384 ticks. Each time the tempo-map value is below four
// times the parser reference, the value is doubled and the integer step is
// halved. The parser does not validate these floats. If a nonpositive value
// enters the loop, doubling cannot make it reach a larger threshold and the
// source loop does not terminate. An unordered comparison (NaN in either
// operand) skips the loop.
enum class AdaptiveAirStepDisposition : std::uint8_t {
    produced,
    source_loop_does_not_terminate,
    source_path_cursor_does_not_advance,
};

struct AdaptiveAirStepEvaluation {
    AdaptiveAirStepDisposition disposition{
        AdaptiveAirStepDisposition::produced};
    std::int32_t step{384};
};

constexpr AdaptiveAirStepEvaluation evaluate_air_hold_sample_step(
    float tempo_map_value,
    float parser_reference) {
    std::int32_t step = 384;
    const float threshold = parser_reference * 4.0F;
    if (tempo_map_value < threshold && tempo_map_value <= 0.0F) {
        return {
            AdaptiveAirStepDisposition::source_loop_does_not_terminate,
            step,
        };
    }
    while (tempo_map_value < threshold) {
        tempo_map_value += tempo_map_value;
        step /= 2;
    }
    if (step == 0) {
        return {
            AdaptiveAirStepDisposition::source_path_cursor_does_not_advance,
            step,
        };
    }
    return {AdaptiveAirStepDisposition::produced, step};
}

// Callers that construct Air-family samples require both the tempo-doubling
// loop to terminate and the resulting step to remain positive. Use the
// evaluator before admitting malformed chart inputs; the raw helper preserves
// the source's zero step for exact downstream nonprogress behavior.
constexpr std::int32_t air_hold_sample_step(float tempo_map_value,
                                            float parser_reference) {
    return evaluate_air_hold_sample_step(
               tempo_map_value, parser_reference)
        .step;
}

// The input chain is root start, zero or more saved AHX anchors, then the saved
// final endpoint. The binary appends a disabled boundary before every segment
// after the first, disables the first generated sample of every segment, and
// appends one disabled path-end record. Samples are strictly inside a segment.
// Callers accepting malformed authored input must evaluate the span first.
template <typename TempoAtSchedule,
          typename PointAtGridTick,
          typename ScheduleAtPoint>
std::vector<AirHoldGeneratedPathRecord> generate_air_hold_path_records(
    std::span<const AirHoldPathPoint> anchors,
    float parser_reference,
    TempoAtSchedule tempo_at_schedule,
    PointAtGridTick point_at_grid_tick,
    ScheduleAtPoint schedule_at_point) {
    std::vector<AirHoldGeneratedPathRecord> records;
    if (anchors.empty()) {
        return records;
    }

    for (std::size_t segment = 0; segment + 1U < anchors.size(); ++segment) {
        const auto& start = anchors[segment];
        const auto& end = anchors[segment + 1U];
        if (segment != 0U) {
            records.push_back({start,
                               AirHoldGeneratedRecordKind::segment_boundary,
                               false});
        }

        const std::int32_t start_tick = air_hold_grid_tick(start);
        const std::int32_t delta = subtract_i32_wrapped(
            air_hold_grid_tick(end), start_tick);
        std::int32_t offset = air_hold_sample_step(
            tempo_at_schedule(end.scheduled), parser_reference);
        bool first_sample = true;
        while (static_cast<std::uint32_t>(offset) <
               static_cast<std::uint32_t>(delta)) {
            AirHoldPathPoint point = point_at_grid_tick(
                add_i32_wrapped(start_tick, offset));
            point.scheduled = schedule_at_point(point);
            records.push_back({point, AirHoldGeneratedRecordKind::sample,
                               !first_sample});
            first_sample = false;
            offset = add_i32_wrapped(
                offset,
                air_hold_sample_step(
                    tempo_at_schedule(point.scheduled), parser_reference));
        }
    }

    records.push_back({anchors.back(), AirHoldGeneratedRecordKind::path_end,
                       false});
    return records;
}

struct AirHoldExclusionInterval {
    float start{};
    float end{};
    std::int32_t selector{};
};

constexpr bool air_hold_in_exclusion_interval(
    float scheduled,
    std::span<const AirHoldExclusionInterval> intervals) {
    for (const auto& interval : intervals) {
        if (interval.start < scheduled && scheduled < interval.end &&
            interval.selector == 0) {
            return true;
        }
    }
    return false;
}

// Final AHD (not AHX) suppresses samples whose grid position plus a
// tempo-adaptive external end margin reaches the final endpoint. A separately
// enabled key-0 interval table can only clear additional emission flags.
template <typename TempoAtSchedule>
void filter_air_hold_path_emissions(
    std::span<AirHoldGeneratedPathRecord> records,
    AirHoldCommand final_command,
    const AirHoldPathPoint& final_endpoint,
    float parser_reference,
    float end_margin,
    bool exclusion_filter_enabled,
    std::span<const AirHoldExclusionInterval> exclusion_intervals,
    TempoAtSchedule tempo_at_schedule) {
    const bool filter_near_end =
        final_command == AirHoldCommand::ahd && end_margin >= 0.0F;
    const std::int32_t end_tick = air_hold_grid_tick(final_endpoint);
    for (auto& record : records) {
        if (filter_near_end) {
            const std::int32_t step = air_hold_sample_step(
                tempo_at_schedule(record.point.scheduled), parser_reference);
            const std::int32_t margin = cvttss2si_i32(
                static_cast<float>(step) * end_margin + 0.5F);
            if (add_i32_wrapped(
                    air_hold_grid_tick(record.point), margin) >= end_tick) {
                record.emission_enabled = false;
            }
        }
        if (exclusion_filter_enabled &&
            air_hold_in_exclusion_interval(record.point.scheduled,
                                           exclusion_intervals)) {
            record.emission_enabled = false;
        }
    }
}

// The parser leaves the secondary direction field at its reset value zero, so
// AirHold's root-relative start checker always selects the lower-numbered
// member of the same root-family profile pair used by AIR.
constexpr std::int32_t air_hold_start_profile(std::int32_t root_type) {
    return air_input_profile(root_type, AirDirectionCode::air);
}

struct AirHoldContactState {
    bool admitted{};
};

// The sustain tracker reads derived profile 7 and profile 6 in this order. Its
// admission bit latches when profile 7 is absent or profile 6 is present.
// Current activity additionally requires the start position to have been
// reached and profile 7 to be present.
constexpr bool update_air_hold_contact(AirHoldContactState& state,
                                       bool at_or_after_start,
                                       bool profile_6,
                                       bool profile_7) {
    state.admitted = state.admitted || !profile_7 || profile_6;
    return at_or_after_start && state.admitted && profile_7;
}

enum class AirHoldStartPhase : std::uint8_t {
    awaiting_result = 0,
    complete = 5,
};

enum class AirHoldPathPhase : std::uint8_t {
    before_start = 0,
    best_current_gap = 3,
    other_current_gap = 4,
    complete = 5,
};

constexpr AirHoldPathPhase air_hold_active_path_phase(
    std::int32_t current_gap_index) {
    return current_gap_index == 0 ? AirHoldPathPhase::best_current_gap
                                  : AirHoldPathPhase::other_current_gap;
}

constexpr bool air_hold_path_complete(std::size_t generated_remaining,
                                      std::size_t authored_resolved,
                                      std::size_t authored_total) {
    return generated_remaining == 0 && authored_resolved == authored_total;
}

constexpr bool air_hold_is_terminal(AirHoldStartPhase start,
                                    AirHoldPathPhase path) {
    return start == AirHoldStartPhase::complete &&
           path == AirHoldPathPhase::complete;
}

constexpr bool air_hold_exposes_candidate() {
    return false;
}

inline constexpr std::int32_t air_hold_start_source_category = 9;
inline constexpr std::int32_t air_hold_generated_source_category = 10;

constexpr std::int32_t air_hold_authored_source_category(
    std::size_t checkpoint_index,
    std::size_t checkpoint_count) {
    return checkpoint_index + 1U < checkpoint_count ? 12 : 13;
}

struct AirHoldGeneratedCheckpointDecision {
    bool consumed{};
    bool emit{};
    bool ordinary_result_valid{};
    bool reset_maximum_gap{};
    std::uint8_t ordinary_result_byte{};
};

// At most the front generated path record is consumed in one substep. A clear
// emission flag still consumes it but neither classifies nor resets the gap.
// Forced-result selection also bypasses the ordinary maximum-gap reset.
constexpr AirHoldGeneratedCheckpointDecision
update_air_hold_generated_checkpoint(HoldGapState& gap,
                                     bool front_due,
                                     bool emission_enabled,
                                     bool forced_result_active) {
    AirHoldGeneratedCheckpointDecision decision{};
    if (!front_due) {
        return decision;
    }

    decision.consumed = true;
    decision.emit = emission_enabled;
    if (emission_enabled && !forced_result_active) {
        decision.ordinary_result_valid = true;
        decision.ordinary_result_byte =
            hold_gap_result_byte(classify_hold_gap(gap));
        reset_hold_gap_after_checkpoint(gap);
        decision.reset_maximum_gap = true;
    }
    return decision;
}

// claim.note.air-slide-secondary-judgement
//
// ASD and ASC attach parsed secondary type 8. The command identity is retained
// on each 0x24-byte control point: ASD marks an authored checkpoint and a
// generated-path cadence restart, while ASC does neither.
enum class AirSlideCommand : std::uint8_t {
    asd,
    asc,
};

constexpr bool air_slide_is_asd_marker(AirSlideCommand command) {
    return command == AirSlideCommand::asd;
}

constexpr bool air_slide_continuation_reference_matches(
    AirSlideCommand previous,
    AirSlideCommand referenced) {
    return previous == referenced;
}

struct AirSlidePathPoint {
    float major{};
    float minor{};
    float scheduled{};
    AirSlideCommand command{AirSlideCommand::asc};
};

enum class AirSlideGeneratedRecordKind : std::int32_t {
    sample = 5,
    segment_boundary = 6,
    path_end = 7,
};

struct AirSlideGeneratedPathRecord {
    AirSlidePathPoint point{};
    AirSlideGeneratedRecordKind kind{};
    bool emission_enabled{};
};

inline std::int32_t air_slide_grid_tick(const AirSlidePathPoint& point) {
    return cvttss2si_i32(
        (point.major + point.minor * 0.25F) * 384.0F + 0.5F);
}

enum class AirSlideSegmentGenerationDisposition : std::uint8_t {
    no_interior_samples,
    generated,
    source_path_cursor_does_not_advance,
    source_cursor_wrap_expansion,
};

// AirSlide computes anchor+step with wrapped i32 arithmetic before its signed
// cursor<end test. This evaluator classifies the first iteration of any root
// or ASD-restart segment. Later dynamic steps use the same wrap/nonprogress
// rules before each cursor update.
constexpr AirSlideSegmentGenerationDisposition
evaluate_air_slide_segment_generation(
    std::int32_t anchor_tick,
    std::int32_t end_tick,
    std::int32_t step) {
    const std::int32_t first_cursor =
        add_i32_wrapped(anchor_tick, step);
    if (!(first_cursor < end_tick)) {
        return AirSlideSegmentGenerationDisposition::no_interior_samples;
    }
    if (step == 0) {
        return AirSlideSegmentGenerationDisposition::
            source_path_cursor_does_not_advance;
    }
    if (0 < step && first_cursor < anchor_tick) {
        return AirSlideSegmentGenerationDisposition::
            source_cursor_wrap_expansion;
    }
    return AirSlideSegmentGenerationDisposition::generated;
}

constexpr AirSlideSegmentGenerationDisposition
evaluate_air_slide_cursor_advance(
    std::int32_t cursor_tick,
    std::int32_t end_tick,
    std::int32_t step) {
    if (!(cursor_tick < end_tick)) {
        return AirSlideSegmentGenerationDisposition::no_interior_samples;
    }
    if (step == 0) {
        return AirSlideSegmentGenerationDisposition::
            source_path_cursor_does_not_advance;
    }
    if (0 < step && add_i32_wrapped(cursor_tick, step) < cursor_tick) {
        return AirSlideSegmentGenerationDisposition::
            source_cursor_wrap_expansion;
    }
    return AirSlideSegmentGenerationDisposition::generated;
}

// The generated cursor starts one adaptive step after the root. An ASD control
// point emits a disabled boundary and restarts the cursor one adaptive step
// after that point; ASC carries the already computed cursor across the point.
// The first sample after the root or an ASD restart has emission disabled.
template <typename TempoAtSchedule,
          typename PointAtGridTick,
          typename ScheduleAtPoint>
std::vector<AirSlideGeneratedPathRecord> generate_air_slide_path_records(
    const AirSlidePathPoint& root,
    std::span<const AirSlidePathPoint> controls,
    float parser_reference,
    TempoAtSchedule tempo_at_schedule,
    PointAtGridTick point_at_grid_tick,
    ScheduleAtPoint schedule_at_point) {
    std::vector<AirSlideGeneratedPathRecord> records;
    if (controls.empty()) {
        return records;
    }

    std::int32_t cursor_tick = add_i32_wrapped(
        air_slide_grid_tick(root),
        air_hold_sample_step(tempo_at_schedule(root.scheduled),
                             parser_reference));
    bool suppress_next_sample = true;

    for (std::size_t index = 0; index < controls.size(); ++index) {
        if (index != 0U &&
            air_slide_is_asd_marker(controls[index - 1U].command)) {
            const auto& restart = controls[index - 1U];
            records.push_back({restart,
                               AirSlideGeneratedRecordKind::segment_boundary,
                               false});
            cursor_tick = add_i32_wrapped(
                air_slide_grid_tick(restart),
                air_hold_sample_step(tempo_at_schedule(restart.scheduled),
                                     parser_reference));
            suppress_next_sample = true;
        }

        const std::int32_t end_tick = air_slide_grid_tick(controls[index]);
        while (cursor_tick < end_tick) {
            AirSlidePathPoint point = point_at_grid_tick(cursor_tick);
            point.scheduled = schedule_at_point(point);
            records.push_back({point, AirSlideGeneratedRecordKind::sample,
                               !suppress_next_sample});
            suppress_next_sample = false;
            cursor_tick = add_i32_wrapped(
                cursor_tick,
                air_hold_sample_step(
                    tempo_at_schedule(point.scheduled), parser_reference));
        }
    }

    records.push_back(
        {controls.back(), AirSlideGeneratedRecordKind::path_end, false});
    return records;
}

// A final ASC, but not a final ASD, enables the same parameterized end-margin
// test used by the type-5 producer. The key-0 interval table is shared and can
// only clear additional emission flags.
template <typename TempoAtSchedule>
void filter_air_slide_path_emissions(
    std::span<AirSlideGeneratedPathRecord> records,
    const AirSlidePathPoint& final_control,
    float parser_reference,
    float end_margin,
    bool exclusion_filter_enabled,
    std::span<const AirHoldExclusionInterval> exclusion_intervals,
    TempoAtSchedule tempo_at_schedule) {
    const bool filter_near_end =
        !air_slide_is_asd_marker(final_control.command) &&
        end_margin >= 0.0F;
    const std::int32_t end_tick = air_slide_grid_tick(final_control);
    for (auto& record : records) {
        if (filter_near_end) {
            const std::int32_t step = air_hold_sample_step(
                tempo_at_schedule(record.point.scheduled), parser_reference);
            const std::int32_t margin = cvttss2si_i32(
                static_cast<float>(step) * end_margin + 0.5F);
            if (add_i32_wrapped(
                    air_slide_grid_tick(record.point), margin) >= end_tick) {
                record.emission_enabled = false;
            }
        }
        if (exclusion_filter_enabled &&
            air_hold_in_exclusion_interval(record.point.scheduled,
                                           exclusion_intervals)) {
            record.emission_enabled = false;
        }
    }
}

constexpr std::size_t air_slide_authored_checkpoint_count(
    std::span<const AirSlidePathPoint> controls) {
    std::size_t count = 0;
    for (const auto& control : controls) {
        if (air_slide_is_asd_marker(control.command)) {
            ++count;
        }
    }
    return count;
}

constexpr std::int32_t air_slide_start_profile(std::int32_t root_type) {
    return air_input_profile(root_type, AirDirectionCode::air);
}

using AirSlideContactState = AirHoldContactState;

constexpr bool update_air_slide_contact(AirSlideContactState& state,
                                        bool at_or_after_start,
                                        bool profile_6,
                                        bool profile_7) {
    return update_air_hold_contact(state, at_or_after_start, profile_6,
                                   profile_7);
}

enum class AirSlideStartPhase : std::uint8_t {
    awaiting_result = 0,
    complete = 5,
};

enum class AirSlidePathPhase : std::uint8_t {
    before_start = 0,
    best_current_gap = 3,
    other_current_gap = 4,
    complete = 5,
};

constexpr AirSlidePathPhase air_slide_active_path_phase(
    std::int32_t current_gap_index) {
    return current_gap_index == 0 ? AirSlidePathPhase::best_current_gap
                                  : AirSlidePathPhase::other_current_gap;
}

constexpr bool air_slide_path_complete(std::size_t generated_remaining,
                                       std::size_t authored_resolved,
                                       std::size_t authored_total) {
    return generated_remaining == 0 && authored_resolved == authored_total;
}

constexpr bool air_slide_is_terminal(AirSlideStartPhase start,
                                     AirSlidePathPhase path) {
    return start == AirSlideStartPhase::complete &&
           path == AirSlidePathPhase::complete;
}

constexpr bool air_slide_exposes_candidate() {
    return false;
}

inline constexpr std::int32_t air_slide_start_source_category = 9;
inline constexpr std::int32_t air_slide_generated_source_category = 11;

// Only ASD-marked controls own authored checkers. Their source category is 13
// exactly when that ASD is also the final control point; otherwise it is 12.
constexpr std::int32_t air_slide_authored_source_category(
    std::size_t control_index,
    std::size_t control_count) {
    return control_index + 1U == control_count ? 13 : 12;
}

struct AirSlideGeneratedCheckpointDecision {
    bool consumed{};
    bool emit{};
    bool ordinary_result_valid{};
    bool reset_maximum_gap{};
    std::uint8_t ordinary_result_byte{};
};

// Unlike AirHold, the ordinary type-8 path consumes and resets the retained
// maximum even when a due record's emission flag is clear. The flag gates only
// result submission. Forced selection still bypasses the ordinary reset.
constexpr AirSlideGeneratedCheckpointDecision
update_air_slide_generated_checkpoint(HoldGapState& gap,
                                      bool front_due,
                                      bool emission_enabled,
                                      bool forced_result_active) {
    AirSlideGeneratedCheckpointDecision decision{};
    if (!front_due) {
        return decision;
    }

    decision.consumed = true;
    decision.emit = emission_enabled;
    if (!forced_result_active) {
        decision.ordinary_result_valid = true;
        decision.ordinary_result_byte =
            hold_gap_result_byte(classify_hold_gap(gap));
        reset_hold_gap_after_checkpoint(gap);
        decision.reset_maximum_gap = true;
    }
    return decision;
}

// claim.note.air-ladder-generated-checkpoints
//
// ALD normally constructs AirLadderNote. One exact parsed-field combination
// selects the separate HeavenHoldNote implementation shared with parsed type
// 13: sampling interval zero and style-table code 15 (the executable string
// "NON").
enum class AldRuntimeKind : std::uint8_t {
    air_ladder,
    heaven_hold,
};

inline constexpr std::int32_t ald_non_style_code = 15;
inline constexpr std::int32_t ald_missing_style_code = 0;

constexpr AldRuntimeKind select_ald_runtime(std::int32_t sampling_interval,
                                            std::int32_t style_code) {
    return sampling_interval == 0 && style_code == ald_non_style_code
               ? AldRuntimeKind::heaven_hold
               : AldRuntimeKind::air_ladder;
}

inline constexpr std::int32_t air_ladder_input_profile = 7;
inline constexpr std::int32_t air_ladder_source_category = 18;

constexpr bool air_ladder_exposes_candidate() {
    return false;
}

// Type-9 ALD keeps authored 0x24-byte controls separate from its generated
// 0x20-byte samples. The parser supplies already-decoded widths to the
// generator and stores the visible vertical property in tenths before this
// interpolation step.
struct AirLadderPathPoint {
    ChartPosition position{};
    float scheduled{};
    float lane{};
    float vertical{};
    float decoded_width{1.0F};
};

struct AirLadderGeneratedCheckpoint {
    AirLadderPathPoint point{};
    bool enabled{true};
};

// The main type-9 path does not use the generated checkpoint vector. Its
// precompute owns root plus authored-control path points. Each point is a
// decoded width, lane center, vertical value, and a marker set only on the
// final authored endpoint. An empty control list has no valid authored span.
struct AirLadderAuthoredGeometryPoint {
    float decoded_width{1.0F};
    float lane_center{};
    float vertical{};
    bool final_endpoint{};
};

inline std::vector<AirLadderAuthoredGeometryPoint>
build_air_ladder_authored_geometry_path(
    const AirLadderPathPoint& root,
    std::span<const AirLadderPathPoint> controls) {
    std::vector<AirLadderAuthoredGeometryPoint> path;
    if (controls.empty()) {
        return path;
    }
    path.reserve(controls.size() + 1);
    const auto append = [&path](const AirLadderPathPoint& point,
                                bool final_endpoint) {
        path.push_back(AirLadderAuthoredGeometryPoint{
            point.decoded_width,
            point.lane + point.decoded_width * 0.5F,
            point.vertical,
            final_endpoint,
        });
    };
    append(root, false);
    for (std::size_t index = 0; index < controls.size(); ++index) {
        append(controls[index], index + 1 == controls.size());
    }
    return path;
}

inline std::int32_t air_ladder_grid_tick(
    const AirLadderPathPoint& point) {
    return cvttss2si_i32(
        (point.position.major + point.position.minor * 0.25F) * 384.0F +
        0.5F);
}

// The producer holds its cursor in a uint32 register and uses unsigned
// quotient/remainder when converting a sampled tick back to C2S position.
// For ordinary nonnegative chart positions this is the familiar major/minor
// split; preserving the register interpretation also defines negative-bit
// patterns without C++ signed-division assumptions.
inline ChartPosition air_ladder_position_from_grid_tick(
    std::int32_t tick) {
    const std::uint32_t raw = std::bit_cast<std::uint32_t>(tick);
    return canonicalize_c2s_position(
        static_cast<std::int32_t>(raw / 384U),
        static_cast<std::int32_t>(raw % 384U));
}

enum class AirLadderGenerationDisposition : std::uint8_t {
    generated,
    no_authored_controls,
    nonpositive_interval,
    no_forward_span,
    source_cursor_wrap_expansion,
};

struct AirLadderGenerationResult {
    AirLadderGenerationDisposition disposition{
        AirLadderGenerationDisposition::generated};
    std::vector<AirLadderGeneratedCheckpoint> checkpoints{};
};

// The cursor begins at the root, not one interval after it. A final authored
// endpoint is present only when the fixed interval lands on it exactly;
// overshoot leaves the last generated point before the end. Each returned
// point receives the parser's post-generation schedule value through the
// callback. The two nonprogress dispositions report malformed source domains
// instead of reproducing an unbounded allocation/loop in the clean-room code.
template <typename ScheduleAtPosition>
AirLadderGenerationResult generate_air_ladder_checkpoints(
    const AirLadderPathPoint& root,
    std::span<const AirLadderPathPoint> controls,
    std::int32_t interval,
    ScheduleAtPosition schedule_at_position) {
    AirLadderGenerationResult result{};
    if (controls.empty()) {
        result.disposition =
            AirLadderGenerationDisposition::no_authored_controls;
        return result;
    }
    if (interval <= 0) {
        result.disposition =
            AirLadderGenerationDisposition::nonpositive_interval;
        return result;
    }

    std::int32_t cursor = air_ladder_grid_tick(root);
    const std::int32_t final_tick =
        air_ladder_grid_tick(controls.back());
    if (!(cursor < final_tick)) {
        result.disposition =
            AirLadderGenerationDisposition::no_forward_span;
        return result;
    }

    std::size_t control_index = 0;
    std::int32_t previous_tick = cursor;
    AirLadderPathPoint previous = root;

    while (cursor < final_tick) {
        bool emitted = false;
        while (control_index < controls.size()) {
            const auto& current = controls[control_index];
            const std::int32_t current_tick =
                air_ladder_grid_tick(current);
            if (cursor < current_tick && previous_tick != current_tick) {
                const float fraction =
                    static_cast<float>(
                        subtract_i32_wrapped(cursor, previous_tick)) /
                    static_cast<float>(subtract_i32_wrapped(
                        current_tick, previous_tick));
                AirLadderPathPoint point{};
                point.position =
                    air_ladder_position_from_grid_tick(cursor);
                point.lane =
                    (current.lane - previous.lane) * fraction +
                    previous.lane;
                point.vertical =
                    (current.vertical - previous.vertical) * fraction +
                    previous.vertical;
                point.decoded_width =
                    (current.decoded_width - previous.decoded_width) *
                        fraction +
                    previous.decoded_width;
                point.scheduled = schedule_at_position(point.position);
                result.checkpoints.push_back({point, true});
                emitted = true;
                break;
            }

            previous = current;
            previous_tick = current_tick;
            ++control_index;
        }

        // Since controls.back() owns final_tick and cursor < final_tick, the
        // scan must encounter a bounding nonzero span. Reaching the end would
        // contradict those invariants even when earlier controls are unordered
        // or duplicated.
        if (!emitted) {
            return result;
        }

        const std::int32_t next = add_i32_wrapped(cursor, interval);
        if (next < cursor) {
            result.disposition = AirLadderGenerationDisposition::
                source_cursor_wrap_expansion;
            return result;
        }
        cursor = next;
    }

    if (cursor == final_tick) {
        AirLadderPathPoint point = controls.back();
        point.position = air_ladder_position_from_grid_tick(final_tick);
        point.scheduled = schedule_at_position(point.position);
        result.checkpoints.push_back({point, true});
    }
    result.disposition = AirLadderGenerationDisposition::generated;
    return result;
}

// The runtime visits every unresolved 0x88-byte checker in one update; unlike
// front-only path consumers, more than one can resolve in the same substep.
constexpr std::size_t air_ladder_new_resolution_count(
    std::span<const std::int32_t> states_before,
    std::span<const std::int32_t> states_after) {
    const std::size_t count =
        states_before.size() < states_after.size() ? states_before.size()
                                                   : states_after.size();
    std::size_t resolved = 0;
    for (std::size_t index = 0; index < count; ++index) {
        if (states_before[index] < 2 && 1 < states_after[index]) {
            ++resolved;
        }
    }
    return resolved;
}

// The runtime copies the final authored endpoint schedule from the type-9
// precompute object. The comparison below preserves the executable's exact
// two-comparison guard instead of simplifying it to current >= end: either
// NaN operand bypasses the schedule guard, after which checker cardinality is
// still required. Equality at the authored end is terminal-eligible.
constexpr bool air_ladder_is_terminal(float current,
                                      float authored_end_schedule,
                                      std::size_t resolved_count,
                                      std::size_t checkpoint_count) {
    if (current <= authored_end_schedule &&
        authored_end_schedule != current) {
        return false;
    }
    return resolved_count == checkpoint_count;
}

// claim.note.air-ladder-precalc-presentation
//
// These neutral structures reconstruct the asset-independent portion of the
// shared type-9 geometry builder. Resource identities, colors, textures, and
// stream names are intentionally absent. The executable carries raw-relative
// and projected position separately; clipping changes projected position and
// the geometric values, but not raw-relative position or the endpoint marker.
struct AirLadderGeometryEndpoint {
    float raw_relative{};
    float projected{};
    float lateral{};
    float vertical{};
    float decoded_width{1.0F};
    float style_coordinate{};
    float normalized_left{};
    float normalized_right{};
    bool path_marker{};
};

struct AirLadderGeometrySegment {
    bool enabled{true};
    AirLadderGeometryEndpoint start{};
    AirLadderGeometryEndpoint end{};
};

inline constexpr float air_ladder_projection_near = 50.0F;
inline constexpr float air_ladder_projection_far = -600.0F;
inline constexpr float air_ladder_clip_minimum_span = 0.000001F;
// InitializeAirPathJointDescriptors receives selector 9 from the AirLadder
// precompute producer. Its first topology is 3 for selectors 8/9 and 4 for
// every other value; the remaining two are fixed at 3/2. Therefore the
// factory-reachable AirLadder path uses the same topology triple as AirSlide.
inline constexpr std::array<std::int32_t, 3>
    air_ladder_primitive_topologies{3, 3, 2};
inline constexpr std::array<std::int32_t, 3>
    air_ladder_primitive_counter_categories{7, 9, 8};

// The type-9 precompute selects one embedded coordinate by normalized style
// code and copies it to the final float of every neutral vertex. It is kept as
// geometry data without assigning a texture-atlas or resource name.
inline constexpr std::array<float, 16>
    air_ladder_style_vertex_coordinates{
        0.1561999917F,
        0.9688000083F,
        0.9061999917F,
        0.8436999917F,
        0.7811999917F,
        0.7186999917F,
        0.6561999917F,
        0.5938000083F,
        0.5311999917F,
        0.4688000083F,
        0.4061999917F,
        0.3438000083F,
        0.2811999917F,
        0.09399998188F,
        0.03100001812F,
        0.0F,
    };

constexpr float air_ladder_style_vertex_coordinate(
    std::int32_t style_code) {
    const std::int32_t normalized =
        0 <= style_code && style_code < 16 ? style_code : 0;
    return air_ladder_style_vertex_coordinates[
        static_cast<std::size_t>(normalized)];
}

constexpr float air_ladder_render_lateral(float value) {
    return presentation_lane_coordinate_lateral(value);
}

constexpr float common_air_render_vertical(float value) {
    return (value - 1.0F) * 3.8934999F;
}

constexpr float common_air_action_render_vertical(float value) {
    return common_air_render_vertical(value) + 0.14999962F;
}

// AirNote mirrors the external resource's lateral scale for direction codes
// 2 (AUL) and 4 (ADR). The magnitude comes from the external resource.
constexpr float air_direction_resource_scale_sign(
    std::int32_t direction_code) {
    return direction_code == 2 || direction_code == 4 ? -1.0F : 1.0F;
}

// claim.presentation.air-root-motion-feedback
//
// AirNote has six direction rows, with mirrored pairs sharing the same
// external resource family. Width indexing clamps decoded_width-1 to 0..15.
constexpr std::int32_t air_model_resource_row(
    std::int32_t decoded_width,
    AirDirectionCode direction) {
    const std::int32_t width_index =
        std::clamp(decoded_width - 1, 0, 15);
    std::int32_t first_row = 0;
    switch (direction) {
    case AirDirectionCode::air:
        first_row = 161;
        break;
    case AirDirectionCode::aur:
    case AirDirectionCode::aul:
        first_row = 177;
        break;
    case AirDirectionCode::adw:
        first_row = 193;
        break;
    case AirDirectionCode::adr:
    case AirDirectionCode::adl:
        first_row = 209;
        break;
    }
    return first_row - width_index;
}

// Parsed property zero follows the lower-direction class, property one clears
// the stored bit, and property two sets it. No active Air presentation consumer
// of this stored bit was found in the closed family path.
constexpr bool air_stored_presentation_property_bit(
    std::int32_t parsed_property,
    AirDirectionCode direction) {
    if (parsed_property == 0) {
        return !air_uses_upper_profile(direction);
    }
    return parsed_property == 2;
}

inline constexpr float air_manager_reference_origin = 216000.0F;
inline constexpr float air_manager_reference_unit = 0.06F;
inline constexpr float air_lower_direction_reference_factor = 0.4F;

constexpr float air_manager_reference_position(
    float mapped_chart_position,
    bool external_config_enabled,
    float external_reference_scale) {
    const float scale =
        external_config_enabled ? external_reference_scale : 1.0F;
    return mapped_chart_position * air_manager_reference_unit * scale +
           air_manager_reference_origin;
}

constexpr float air_direction_scaled_reference(
    float manager_reference_position,
    AirDirectionCode direction,
    bool external_config_enabled,
    float external_upper_scale,
    float external_lower_scale) {
    if (air_uses_upper_profile(direction)) {
        const float scale =
            external_config_enabled ? external_upper_scale : 1.0F;
        return manager_reference_position * scale;
    }
    const float scale =
        external_config_enabled ? external_lower_scale : 1.0F;
    return manager_reference_position * scale *
           air_lower_direction_reference_factor;
}

constexpr float air_model_projection_delta(
    float manager_reference_position,
    float adjusted_scheduled_position,
    AirDirectionCode direction,
    bool external_config_enabled,
    float external_upper_scale,
    float external_lower_scale) {
    return air_direction_scaled_reference(
               manager_reference_position, direction,
               external_config_enabled, external_upper_scale,
               external_lower_scale) -
           adjusted_scheduled_position;
}

constexpr float air_external_resource_lateral_offset(float value) {
    return value * 4.0F;
}

constexpr float air_external_resource_vertical_offset(float value) {
    return value * 3.8934999F;
}

// Resource +0xa4 uses the shared depth transform without the -65 origin.
constexpr float air_external_resource_depth_offset(
    float resource_delta,
    float runtime_speed,
    float projection_base_offset,
    float positive_delta_projection_factor,
    float approach_scale = runtime_materialization_fixed_scale) {
    const float factor = 0.0F < resource_delta
                             ? positive_delta_projection_factor
                             : 1.0F;
    return -approach_scale *
           (projection_base_offset + resource_delta) * runtime_speed *
           factor;
}

constexpr bool air_root_model_update_enabled(std::int32_t phase) {
    return phase != 3;
}

constexpr std::int32_t air_terminal_cue_selector(
    AirDirectionCode direction) {
    return air_uses_upper_profile(direction) ? 0x3c : 0x134;
}

constexpr bool air_terminal_emits_field_feedback(
    AirDirectionCode direction) {
    return air_uses_upper_profile(direction);
}

// claim.presentation.air-hold-model-path
//
// AirHold presentation is resource-backed rather than a custom vertex stream.
// These neutral transforms retain exact executable placement, visibility, and
// selector behavior while leaving external resource identities as caller data.
constexpr std::int32_t air_hold_resource_width_index(
    std::int32_t decoded_width) {
    return std::clamp(decoded_width - 1, 0, 15);
}

constexpr bool air_hold_presentation_update_enabled(
    AirHoldStartPhase start_phase,
    AirHoldPathPhase path_phase) {
    return !air_hold_is_terminal(start_phase, path_phase);
}

constexpr bool air_hold_root_resource_visible(
    AirHoldStartPhase start_phase) {
    return start_phase != AirHoldStartPhase::complete;
}

constexpr bool air_hold_checkpoint_resources_visible(bool resolved) {
    return !resolved;
}

enum class AirHoldPathPresentationVariant : std::uint8_t {
    default_state,
    best_current_gap,
    other_current_gap,
};

constexpr AirHoldPathPresentationVariant
air_hold_path_presentation_variant(AirHoldPathPhase phase) {
    if (phase == AirHoldPathPhase::best_current_gap) {
        return AirHoldPathPresentationVariant::best_current_gap;
    }
    if (phase == AirHoldPathPhase::other_current_gap) {
        return AirHoldPathPresentationVariant::other_current_gap;
    }
    return AirHoldPathPresentationVariant::default_state;
}

constexpr float air_hold_primary_path_resource_scale(
    AirHoldPathPhase phase) {
    return phase == AirHoldPathPhase::other_current_gap ? 0.5F : 1.0F;
}

constexpr bool air_hold_emits_field_feedback(AirHoldPathPhase phase) {
    return phase == AirHoldPathPhase::best_current_gap;
}

inline constexpr float air_hold_checkpoint_vertical = 15.724F;
inline constexpr float air_hold_path_length_scale = 0.25F;
inline constexpr float air_hold_path_lateral_bias = 0.0001F;
inline constexpr std::array<float, 6>
    air_hold_resource_attachment_verticals{
        0.0F, 15.574F, 15.574F, 0.0F, 15.724F, -15.574F};

struct AirHoldResourceTransform {
    bool visible{true};
    float lateral{};
    float vertical{};
    float projected{};
    float lateral_scale{1.0F};
    float vertical_scale{1.0F};
    float projected_scale{1.0F};
};

constexpr AirHoldResourceTransform build_air_hold_root_transform(
    AirHoldStartPhase phase,
    float base_lateral,
    float base_vertical,
    float start_projected,
    float external_lateral_offset,
    float external_vertical_offset,
    float external_projected_offset,
    std::int32_t decoded_width,
    std::int32_t external_native_width) {
    return {
        air_hold_root_resource_visible(phase),
        base_lateral + external_lateral_offset,
        base_vertical + external_vertical_offset,
        start_projected + external_projected_offset,
        tap_model_lateral_scale(decoded_width, external_native_width),
        1.0F,
        1.0F,
    };
}

constexpr AirHoldResourceTransform build_air_hold_checkpoint_transform(
    bool resolved,
    float base_lateral,
    float checkpoint_projected,
    float external_lateral_offset,
    float external_vertical_offset,
    float external_projected_offset,
    std::int32_t decoded_width,
    std::int32_t external_native_width) {
    return {
        air_hold_checkpoint_resources_visible(resolved),
        base_lateral + external_lateral_offset,
        air_hold_checkpoint_vertical + external_vertical_offset,
        checkpoint_projected + external_projected_offset,
        tap_model_lateral_scale(decoded_width, external_native_width),
        1.0F,
        1.0F,
    };
}

struct AirHoldPresentationCheckpoint {
    bool resolved{};
    float scheduled{};
    float projected{};
};

struct AirHoldProjectionEnvelope {
    float minimum{};
    float maximum{};
    bool includes_judgement_plane{};
};

inline AirHoldProjectionEnvelope build_air_hold_projection_envelope(
    float manager_current,
    float start_projected,
    float end_schedule,
    float end_projected,
    AirHoldStartPhase start_phase,
    float judgement_plane_projected,
    std::span<const AirHoldPresentationCheckpoint> checkpoints) {
    AirHoldProjectionEnvelope result{
        start_projected, start_projected, false};
    if (end_projected <= result.minimum) {
        result.minimum = end_projected;
    }
    if (result.maximum <= end_projected) {
        result.maximum = end_projected;
    }

    bool has_future_unresolved = 0.0F < end_schedule - manager_current;
    for (const auto& checkpoint : checkpoints) {
        if (checkpoint.resolved) {
            continue;
        }
        if (0.0F < checkpoint.scheduled - manager_current) {
            has_future_unresolved = true;
        }
        if (checkpoint.projected <= result.minimum) {
            result.minimum = checkpoint.projected;
        }
        if (result.maximum <= checkpoint.projected) {
            result.maximum = checkpoint.projected;
        }
    }

    if (has_future_unresolved &&
        start_phase == AirHoldStartPhase::complete) {
        result.includes_judgement_plane = true;
        if (judgement_plane_projected <= result.minimum) {
            result.minimum = judgement_plane_projected;
        }
        if (result.maximum <= judgement_plane_projected) {
            result.maximum = judgement_plane_projected;
        }
    }
    return result;
}

constexpr AirHoldResourceTransform build_air_hold_path_envelope_transform(
    const AirHoldProjectionEnvelope& envelope,
    float base_lateral,
    float base_vertical,
    float external_lateral_offset,
    float external_vertical_offset,
    float external_projected_offset,
    float lateral_scale) {
    return {
        true,
        base_lateral + external_lateral_offset +
            air_hold_path_lateral_bias,
        base_vertical + external_vertical_offset,
        envelope.maximum + external_projected_offset,
        lateral_scale,
        1.0F,
        (envelope.maximum - envelope.minimum) *
            air_hold_path_length_scale,
    };
}

// claim.presentation.air-slide-model-path
//
// AirSlide shares the type-9 three-stream geometry implementation, but owns a
// distinct resource-backed root and ASD-action path.  The helpers below keep
// the external resource identities and native widths as inputs while
// preserving the executable-owned selectors, transforms, and visibility.
constexpr std::int32_t air_slide_resource_width_index(
    std::int32_t decoded_width) {
    return std::clamp(decoded_width - 1, 0, 15);
}

constexpr bool air_slide_presentation_update_enabled(
    AirSlideStartPhase start_phase,
    AirSlidePathPhase path_phase) {
    return !air_slide_is_terminal(start_phase, path_phase);
}

constexpr bool air_slide_root_resource_visible(
    AirSlideStartPhase start_phase) {
    return start_phase != AirSlideStartPhase::complete;
}

constexpr bool air_slide_action_resources_visible(bool resolved) {
    return !resolved;
}

enum class AirSlidePathPresentationVariant : std::uint8_t {
    default_state = 0,
    best_current_gap = 1,
    other_current_gap = 2,
};

constexpr AirSlidePathPresentationVariant
air_slide_path_presentation_variant(AirSlidePathPhase phase) {
    if (phase == AirSlidePathPhase::best_current_gap) {
        return AirSlidePathPresentationVariant::best_current_gap;
    }
    if (phase == AirSlidePathPhase::other_current_gap) {
        return AirSlidePathPresentationVariant::other_current_gap;
    }
    return AirSlidePathPresentationVariant::default_state;
}

constexpr SharedAirPathPresentationMode
air_slide_shared_geometry_mode(AirSlidePathPhase phase) {
    if (phase == AirSlidePathPhase::best_current_gap) {
        return SharedAirPathPresentationMode::animated;
    }
    if (phase == AirSlidePathPhase::other_current_gap) {
        return SharedAirPathPresentationMode::alternate;
    }
    return SharedAirPathPresentationMode::base;
}

constexpr bool air_slide_emits_field_feedback(AirSlidePathPhase phase) {
    return phase == AirSlidePathPhase::best_current_gap;
}

// The parsed appearance field is normalized into the three-entry stride used
// by each of the shared geometry's external resource streams.  Unrecognized
// values retain the constructor's zero row.
constexpr std::int32_t air_slide_resource_style_row(
    std::int32_t parsed_style) {
    if (parsed_style == 0) {
        return 2;
    }
    if (parsed_style == 2) {
        return 1;
    }
    return 0;
}

constexpr std::array<std::int32_t, 3>
air_slide_resource_style_offsets(std::int32_t parsed_style) {
    const std::int32_t row =
        air_slide_resource_style_row(parsed_style);
    return {row, row + 3, row + 6};
}

inline constexpr std::array<std::int32_t, 3>
    air_slide_primitive_topologies{3, 3, 2};
// Diagnostic counter calls occur in stream order 0, 2, 1.  They are not a
// scene-graph draw-order interface.
inline constexpr std::array<std::int32_t, 3>
    air_slide_primitive_counter_categories{4, 6, 5};
inline constexpr std::uint32_t air_slide_action_resource_count = 2;
inline constexpr std::uint32_t air_slide_preload_ready_after_step = 12;
inline constexpr float air_slide_initial_projected = -10000.0F;
inline constexpr float air_slide_root_vertical_scale_factor = 0.06420958F;
inline constexpr float air_slide_action_vertical_scale_factor =
    0.063597046F;

constexpr float air_slide_initial_root_vertical_value(
    bool type_13_attachment,
    float final_control_vertical) {
    return type_13_attachment ? final_control_vertical : 1.0F;
}

using AirSlideResourceTransform = AirHoldResourceTransform;

constexpr AirSlideResourceTransform build_air_slide_root_transform(
    AirSlideStartPhase phase,
    float base_lateral,
    float initial_root_vertical_value,
    float authored_root_vertical_value,
    float start_projected,
    float external_lateral_offset,
    float external_vertical_offset,
    float external_projected_offset,
    std::int32_t decoded_width,
    std::int32_t external_native_width) {
    return {
        air_slide_root_resource_visible(phase),
        base_lateral + external_lateral_offset,
        common_air_render_vertical(initial_root_vertical_value) +
            external_vertical_offset,
        start_projected + external_projected_offset,
        tap_model_lateral_scale(decoded_width, external_native_width),
        common_air_render_vertical(authored_root_vertical_value) *
            air_slide_root_vertical_scale_factor,
        1.0F,
    };
}

constexpr std::array<AirSlideResourceTransform,
                     air_slide_action_resource_count>
build_air_slide_action_transforms(
    bool resolved,
    float base_lateral,
    float authored_vertical_value,
    float projected,
    float external_lateral_offset,
    float external_vertical_offset,
    float external_projected_offset,
    std::int32_t decoded_width,
    std::int32_t external_native_width) {
    const float vertical =
        common_air_action_render_vertical(authored_vertical_value);
    const float lateral_scale =
        tap_model_lateral_scale(decoded_width, external_native_width);
    const AirSlideResourceTransform first{
        air_slide_action_resources_visible(resolved),
        base_lateral + external_lateral_offset,
        vertical + external_vertical_offset,
        projected + external_projected_offset,
        lateral_scale,
        1.0F,
        1.0F,
    };
    AirSlideResourceTransform second = first;
    second.vertical_scale =
        vertical * air_slide_action_vertical_scale_factor;
    return {first, second};
}

struct AirSlidePresentationPoint {
    float adjusted_scheduled{};
    float raw_scheduled{};
    float lane{};
    float decoded_width{1.0F};
    float vertical{};
};

struct AirSlideGeometrySchedule {
    float adjusted_relative{};
    float raw_relative{};
};

// The update supplies root first, then every authored control, to parallel
// adjusted-relative and raw-relative arrays.  Geometry construction uses the
// same root/control order; only the final authored endpoint carries the path
// marker consumed by the shared builder.
inline std::vector<AirSlideGeometrySchedule>
build_air_slide_geometry_schedules(
    float manager_current,
    const AirSlidePresentationPoint& root,
    std::span<const AirSlidePresentationPoint> controls) {
    std::vector<AirSlideGeometrySchedule> result;
    result.reserve(controls.size() + 1U);
    const auto append = [&result, manager_current](
                            const AirSlidePresentationPoint& point) {
        result.push_back({point.adjusted_scheduled - manager_current,
                          point.raw_scheduled - manager_current});
    };
    append(root);
    for (const auto& control : controls) {
        append(control);
    }
    return result;
}

inline std::vector<AirLadderAuthoredGeometryPoint>
build_air_slide_authored_geometry_path(
    const AirSlidePresentationPoint& root,
    std::span<const AirSlidePresentationPoint> controls) {
    std::vector<AirLadderAuthoredGeometryPoint> path;
    if (controls.empty()) {
        return path;
    }
    path.reserve(controls.size() + 1U);
    path.push_back({root.decoded_width,
                    root.lane + root.decoded_width * 0.5F,
                    root.vertical,
                    false});
    for (std::size_t index = 0; index < controls.size(); ++index) {
        const auto& control = controls[index];
        path.push_back({control.decoded_width,
                        control.lane + control.decoded_width * 0.5F,
                        control.vertical,
                        index + 1U == controls.size()});
    }
    return path;
}

constexpr float air_ladder_render_vertical(float value) {
    return common_air_render_vertical(value);
}

constexpr float air_ladder_stream_zero_half_extent(float decoded_width) {
    return air_ladder_render_lateral(decoded_width + 8.0F) * 0.5F *
           0.75F;
}

constexpr float air_ladder_stream_one_scale(float vertical) {
    if (0.0F < vertical) {
        if (15.574F <= vertical) {
            return 0.65F;
        }
        return (1.0F - vertical * 0.06420958F) * 0.100000024F +
               0.65F;
    }
    return 0.75F;
}

constexpr float air_ladder_stream_one_half_extent(float decoded_width,
                                                   float vertical) {
    return air_ladder_render_lateral(decoded_width + 8.0F) * 0.5F *
           air_ladder_stream_one_scale(vertical);
}

constexpr float air_ladder_stream_two_half_extent(bool compact) {
    return compact ? 0.98F : 1.96F;
}

// Each generated checkpoint owns a separate presentation resource transform.
// The resource identity comes from one of two external tables (first sample
// versus later samples), indexed by the decoded width. Clean-room code keeps
// only the exact selector and transform; it accepts the resource's intrinsic
// width as an explicit external parameter instead of copying resource data.
struct AirLadderCheckpointTransform {
    std::int32_t resource_slot{};
    float lateral{};
    float vertical{};
    float projected{-10000.0F};
    float lateral_scale{1.0F};
    float vertical_scale{1.0F};
    float projected_scale{1.0F};
};

inline constexpr std::uint8_t
    air_ladder_unresolved_result_table_index = 0xff;

// FUN_00c196f0 produces judgement tiers 0..11. FUN_00c18270 indexes this
// executable-owned mapping with that tier and stores the selected
// NotesJudgeResultTable row index on the checkpoint record.
inline constexpr std::array<std::uint8_t, 12>
    air_ladder_result_table_indices{
        0, 0, 0, 1, 2, 3, 4, 3, 2, 1, 0, 0};

constexpr std::uint8_t air_ladder_result_table_index(
    std::uint8_t judgement_tier) {
    if (judgement_tier >= air_ladder_result_table_indices.size()) {
        throw std::out_of_range{"invalid AirLadder judgement tier"};
    }
    return air_ladder_result_table_indices[judgement_tier];
}

// Checkpoint records begin with the 0xff sentinel. Resolution replaces it with
// the embedded judgement-tier mapping's NotesJudgeResultTable row index. The
// presentation update narrows the externally loaded table's row count to one
// byte and hides the resource exactly when the stored index is in range.
// Returning true means the transform/visible update is taken; a missing
// external resource still makes that update a no-op inside the resource path.
constexpr bool air_ladder_checkpoint_effect_visible(
    std::uint8_t result_table_index,
    std::size_t loaded_result_table_rows) {
    const auto narrowed_rows =
        static_cast<std::uint8_t>(loaded_result_table_rows);
    return !(result_table_index < narrowed_rows);
}

inline std::int32_t air_ladder_checkpoint_resource_slot(
    float decoded_width) {
    const std::int32_t raw =
        subtract_i32_wrapped(cvttss2si_i32(decoded_width + 0.999F), 1);
    return std::clamp(raw, 0, 15);
}

constexpr float air_ladder_checkpoint_vertical(float vertical) {
    return common_air_action_render_vertical(vertical);
}

constexpr float air_ladder_checkpoint_lateral_scale(
    float decoded_width,
    std::int32_t resource_intrinsic_width) {
    return resource_intrinsic_width < 1
               ? 1.0F
               : decoded_width /
                     static_cast<float>(resource_intrinsic_width);
}

inline AirLadderCheckpointTransform build_air_ladder_checkpoint_transform(
    float lane,
    float decoded_width,
    float vertical,
    std::int32_t resource_intrinsic_width) {
    return AirLadderCheckpointTransform{
        air_ladder_checkpoint_resource_slot(decoded_width),
        air_ladder_render_lateral(lane + decoded_width * 0.5F),
        air_ladder_checkpoint_vertical(vertical),
        -10000.0F,
        air_ladder_checkpoint_lateral_scale(
            decoded_width, resource_intrinsic_width),
        1.0F,
        1.0F,
    };
}

constexpr float air_ladder_clamp_unit(float value) {
    if (value <= 1.0F) {
        return value <= 0.0F ? 0.0F : value;
    }
    return 1.0F;
}

constexpr float air_ladder_mix(float from, float to, float fraction) {
    return from * (1.0F - fraction) + to * fraction;
}

constexpr void air_ladder_clip_endpoint(
    AirLadderGeometryEndpoint& endpoint,
    const AirLadderGeometryEndpoint& opposite,
    float projected_boundary,
    float fraction) {
    endpoint.projected = projected_boundary;
    endpoint.lateral =
        air_ladder_mix(endpoint.lateral, opposite.lateral, fraction);
    endpoint.vertical =
        air_ladder_mix(endpoint.vertical, opposite.vertical, fraction);
    endpoint.decoded_width = air_ladder_mix(
        endpoint.decoded_width, opposite.decoded_width, fraction);
    endpoint.style_coordinate = air_ladder_mix(
        endpoint.style_coordinate, opposite.style_coordinate, fraction);
    endpoint.normalized_left = air_ladder_mix(
        endpoint.normalized_left, opposite.normalized_left, fraction);
    endpoint.normalized_right = air_ladder_mix(
        endpoint.normalized_right, opposite.normalized_right, fraction);
}

constexpr AirLadderGeometrySegment clip_air_ladder_geometry_segment(
    AirLadderGeometrySegment segment) {
    const AirLadderGeometryEndpoint original_start = segment.start;
    const AirLadderGeometryEndpoint original_end = segment.end;
    const float start_projected = original_start.projected;
    const float end_projected = original_end.projected;

    if ((start_projected < air_ladder_projection_far &&
         end_projected < air_ladder_projection_far) ||
        (air_ladder_projection_near < start_projected &&
         air_ladder_projection_near < end_projected)) {
        segment.enabled = false;
        return segment;
    }

    float span = start_projected - end_projected;
    if (span < 0.0F) {
        span = -span;
    }
    if (!(air_ladder_clip_minimum_span <= span)) {
        return segment;
    }

    if (air_ladder_projection_near < start_projected) {
        const float fraction = air_ladder_clamp_unit(
            (start_projected - air_ladder_projection_near) / span);
        segment.start = original_start;
        air_ladder_clip_endpoint(segment.start, original_end,
                                 air_ladder_projection_near, fraction);
    }
    if (air_ladder_projection_near < end_projected) {
        const float fraction = air_ladder_clamp_unit(
            (end_projected - air_ladder_projection_near) / span);
        segment.end = original_end;
        air_ladder_clip_endpoint(segment.end, original_start,
                                 air_ladder_projection_near, fraction);
    }
    if (start_projected < air_ladder_projection_far) {
        const float fraction = air_ladder_clamp_unit(
            (air_ladder_projection_far - start_projected) / span);
        segment.start = original_start;
        air_ladder_clip_endpoint(segment.start, original_end,
                                 air_ladder_projection_far, fraction);
    }
    if (end_projected < air_ladder_projection_far) {
        const float fraction = air_ladder_clamp_unit(
            (air_ladder_projection_far - end_projected) / span);
        segment.end = original_end;
        air_ladder_clip_endpoint(segment.end, original_start,
                                 air_ladder_projection_far, fraction);
    }
    return segment;
}

// FUN_00c03c00 appends 0x18-byte vertices with render-space lateral,
// vertical, projected position, an executable-owned static packed color, and
// two neutral coordinates. Streams zero and one emit one six-vertex quad with
// a winding selected by projected endpoint order. Stream two emits both
// windings (twelve vertices), making its quad double-sided.
struct AirLadderGeometryVertex {
    float lateral{};
    float vertical{};
    float projected{};
    std::uint32_t color{};
    float coordinate_u{};
    float coordinate_v{};
};

static_assert(sizeof(AirLadderGeometryVertex) == 0x18);

constexpr AirLadderGeometryVertex air_ladder_geometry_vertex(
    const AirLadderGeometryEndpoint& endpoint,
    float lateral,
    float vertical,
    std::uint32_t color,
    float coordinate_u) {
    return AirLadderGeometryVertex{
        lateral,
        vertical,
        endpoint.projected,
        color,
        coordinate_u,
        endpoint.style_coordinate,
    };
}

constexpr std::array<AirLadderGeometryVertex, 6>
air_ladder_single_sided_quad(const AirLadderGeometryVertex& start_left,
                             const AirLadderGeometryVertex& start_right,
                             const AirLadderGeometryVertex& end_left,
                             const AirLadderGeometryVertex& end_right,
                             bool end_not_after_start) {
    if (end_not_after_start) {
        return {start_left, end_right, end_left,
                start_left, start_right, end_right};
    }
    return {start_left, end_left, end_right,
            start_left, end_right, start_right};
}

// claim.presentation.air-solid-path
//
// AirSolid uses the same 0x18-byte neutral vertex layout and common Air
// lane/property transforms as AirLadder. Resource identity is external, but
// the packed vertex color is the executable's static base white.
using AirSolidVertex = AirLadderGeometryVertex;

struct AirSolidGeometryEndpoint {
    float raw_relative{};
    float projected{};
    float lateral{};
    float vertical_a{};
    float vertical_b{};
    float decoded_width{1.0F};
    float coordinate_u{0.5F};
    float coordinate_v{};
};

struct AirSolidGeometrySegment {
    bool enabled{true};
    bool emit_bridge{true};
    bool emit_shell{true};
    AirSolidGeometryEndpoint start{};
    AirSolidGeometryEndpoint end{};
};

constexpr float air_solid_style_vertex_coordinate(
    std::int32_t style_code) {
    return air_ladder_style_vertex_coordinate(style_code);
}

constexpr bool air_solid_style_has_external_resource(
    std::int32_t style_code) {
    const std::int32_t normalized =
        0 <= style_code && style_code < 16 ? style_code : 0;
    return normalized != 15;
}

constexpr bool air_solid_is_terminal(float current,
                                     float authored_end_schedule) {
    return authored_end_schedule <= current;
}

constexpr float air_solid_half_extent(float decoded_width) {
    return decoded_width * 2.0F;
}

constexpr bool air_solid_crosses_raw_origin(
    const AirSolidGeometrySegment& segment) {
    return segment.start.raw_relative < air_ladder_clip_minimum_span &&
           segment.end.raw_relative > -air_ladder_clip_minimum_span;
}

constexpr float air_solid_raw_origin_fraction(
    const AirSolidGeometrySegment& segment) {
    float span = segment.end.raw_relative - segment.start.raw_relative;
    if (span < 0.0F) {
        span = -span;
    }
    if (span < air_ladder_clip_minimum_span) {
        span = air_ladder_clip_minimum_span;
    }
    float distance = segment.start.raw_relative;
    if (distance < 0.0F) {
        distance = -distance;
    }
    return air_ladder_clamp_unit(distance / span);
}

constexpr AirSolidGeometryEndpoint air_solid_mix_endpoint(
    const AirSolidGeometryEndpoint& from,
    const AirSolidGeometryEndpoint& to,
    float fraction) {
    return {
        air_ladder_mix(from.raw_relative, to.raw_relative, fraction),
        air_ladder_mix(from.projected, to.projected, fraction),
        air_ladder_mix(from.lateral, to.lateral, fraction),
        air_ladder_mix(from.vertical_a, to.vertical_a, fraction),
        air_ladder_mix(from.vertical_b, to.vertical_b, fraction),
        air_ladder_mix(from.decoded_width, to.decoded_width, fraction),
        air_ladder_mix(from.coordinate_u, to.coordinate_u, fraction),
        air_ladder_mix(from.coordinate_v, to.coordinate_v, fraction),
    };
}

constexpr void air_solid_clip_endpoint(
    AirSolidGeometryEndpoint& endpoint,
    const AirSolidGeometryEndpoint& opposite,
    float projected_boundary,
    float fraction) {
    endpoint.projected = projected_boundary;
    endpoint.lateral =
        air_ladder_mix(endpoint.lateral, opposite.lateral, fraction);
    endpoint.vertical_a =
        air_ladder_mix(endpoint.vertical_a, opposite.vertical_a, fraction);
    endpoint.vertical_b =
        air_ladder_mix(endpoint.vertical_b, opposite.vertical_b, fraction);
    endpoint.decoded_width = air_ladder_mix(
        endpoint.decoded_width, opposite.decoded_width, fraction);
    endpoint.coordinate_u = air_ladder_mix(
        endpoint.coordinate_u, opposite.coordinate_u, fraction);
    endpoint.coordinate_v = air_ladder_mix(
        endpoint.coordinate_v, opposite.coordinate_v, fraction);
}

constexpr AirSolidGeometrySegment clip_air_solid_geometry_segment(
    AirSolidGeometrySegment segment) {
    const AirSolidGeometryEndpoint original_start = segment.start;
    const AirSolidGeometryEndpoint original_end = segment.end;
    const float start_projected = original_start.projected;
    const float end_projected = original_end.projected;

    if ((start_projected < air_ladder_projection_far &&
         end_projected < air_ladder_projection_far) ||
        (air_ladder_projection_near < start_projected &&
         air_ladder_projection_near < end_projected)) {
        segment.enabled = false;
        return segment;
    }

    float span = start_projected - end_projected;
    if (span < 0.0F) {
        span = -span;
    }
    if (!(air_ladder_clip_minimum_span <= span)) {
        return segment;
    }

    if (air_ladder_projection_near < start_projected) {
        const float fraction = air_ladder_clamp_unit(
            (start_projected - air_ladder_projection_near) / span);
        segment.start = original_start;
        air_solid_clip_endpoint(segment.start, original_end,
                                air_ladder_projection_near, fraction);
    }
    if (air_ladder_projection_near < end_projected) {
        const float fraction = air_ladder_clamp_unit(
            (end_projected - air_ladder_projection_near) / span);
        segment.end = original_end;
        air_solid_clip_endpoint(segment.end, original_start,
                                air_ladder_projection_near, fraction);
    }
    if (start_projected < air_ladder_projection_far) {
        const float fraction = air_ladder_clamp_unit(
            (air_ladder_projection_far - start_projected) / span);
        segment.start = original_start;
        air_solid_clip_endpoint(segment.start, original_end,
                                air_ladder_projection_far, fraction);
    }
    if (end_projected < air_ladder_projection_far) {
        const float fraction = air_ladder_clamp_unit(
            (air_ladder_projection_far - end_projected) / span);
        segment.end = original_end;
        air_solid_clip_endpoint(segment.end, original_start,
                                air_ladder_projection_far, fraction);
    }
    return segment;
}

constexpr bool air_solid_uses_reverse_winding(
    const AirSolidGeometrySegment& segment) {
    return segment.end.projected <= segment.start.projected &&
           segment.start.projected != segment.end.projected;
}

struct AirSolidGeometryCorners {
    AirSolidVertex start_left_a{};
    AirSolidVertex start_left_b{};
    AirSolidVertex start_right_a{};
    AirSolidVertex start_right_b{};
    AirSolidVertex end_left_a{};
    AirSolidVertex end_left_b{};
    AirSolidVertex end_right_a{};
    AirSolidVertex end_right_b{};
};

constexpr AirSolidVertex air_solid_vertex(
    const AirSolidGeometryEndpoint& endpoint,
    float lateral,
    float vertical) {
    return {
        lateral,
        vertical,
        endpoint.projected,
        presentation_static_base_color,
        endpoint.coordinate_u,
        endpoint.coordinate_v,
    };
}

constexpr AirSolidGeometryCorners air_solid_geometry_corners(
    const AirSolidGeometrySegment& segment) {
    const float start_half =
        air_solid_half_extent(segment.start.decoded_width);
    const float end_half =
        air_solid_half_extent(segment.end.decoded_width);
    const float start_left = segment.start.lateral - start_half;
    const float start_right = segment.start.lateral + start_half;
    const float end_left = segment.end.lateral - end_half;
    const float end_right = segment.end.lateral + end_half;
    return {
        air_solid_vertex(segment.start, start_left,
                         segment.start.vertical_a),
        air_solid_vertex(segment.start, start_left,
                         segment.start.vertical_b),
        air_solid_vertex(segment.start, start_right,
                         segment.start.vertical_a),
        air_solid_vertex(segment.start, start_right,
                         segment.start.vertical_b),
        air_solid_vertex(segment.end, end_left,
                         segment.end.vertical_a),
        air_solid_vertex(segment.end, end_left,
                         segment.end.vertical_b),
        air_solid_vertex(segment.end, end_right,
                         segment.end.vertical_a),
        air_solid_vertex(segment.end, end_right,
                         segment.end.vertical_b),
    };
}

inline std::vector<AirSolidVertex> build_air_solid_vertices(
    const AirSolidGeometrySegment& segment) {
    std::vector<AirSolidVertex> vertices;
    if (!segment.enabled) {
        return vertices;
    }
    vertices.reserve((segment.emit_bridge ? 6U : 0U) +
                     (segment.emit_shell ? 24U : 0U));
    const auto c = air_solid_geometry_corners(segment);
    const bool reverse = air_solid_uses_reverse_winding(segment);
    const auto append = [&vertices](const AirSolidVertex& a,
                                    const AirSolidVertex& b,
                                    const AirSolidVertex& c) {
        vertices.push_back(a);
        vertices.push_back(b);
        vertices.push_back(c);
    };

    if (segment.emit_bridge) {
        if (reverse) {
            append(c.end_left_a, c.end_right_a, c.start_left_b);
            append(c.start_left_b, c.end_right_a, c.start_right_b);
        } else {
            append(c.end_left_a, c.start_left_b, c.end_right_a);
            append(c.end_right_a, c.start_left_b, c.start_right_b);
        }
    }
    if (!segment.emit_shell) {
        return vertices;
    }

    if (reverse) {
        append(c.end_left_a, c.end_right_a, c.start_left_a);
        append(c.start_left_a, c.end_right_a, c.start_right_a);
        append(c.end_left_a, c.start_left_a, c.end_left_b);
        append(c.end_left_b, c.start_left_a, c.start_left_b);
        append(c.start_right_a, c.end_right_a, c.start_right_b);
        append(c.start_right_b, c.end_right_a, c.end_right_b);
        append(c.start_left_b, c.start_right_b, c.end_left_b);
        append(c.end_left_b, c.start_right_b, c.end_right_b);
    } else {
        append(c.end_left_a, c.start_left_a, c.end_right_a);
        append(c.end_right_a, c.start_left_a, c.start_right_a);
        append(c.end_left_a, c.end_left_b, c.start_left_a);
        append(c.start_left_a, c.end_left_b, c.start_left_b);
        append(c.start_right_a, c.start_right_b, c.end_right_a);
        append(c.end_right_a, c.start_right_b, c.end_right_b);
        append(c.start_left_b, c.end_left_b, c.start_right_b);
        append(c.start_right_b, c.end_left_b, c.end_right_b);
    }
    return vertices;
}

// claim.presentation.hold-root-body-transform
//
// JointHold emits one single-sided, constant-width quad. It keeps raw and
// schedule-adjusted deltas separately: adjusted values feed projection while
// raw values decide judgement-plane crossing and its resource coordinate.
using HoldBodyVertex = AirLadderGeometryVertex;

struct HoldBodyGeometry {
    bool enabled{true};
    HoldPresentationMode mode{HoldPresentationMode::base};
    float left{};
    float right{};
    float start_projected{};
    float end_projected{};
    float start_resource_coordinate{1.0F};
    float end_resource_coordinate{};
    float resource_scale{1.0F};
};

inline HoldBodyGeometry prepare_hold_body_geometry(
    HoldPresentationMode mode,
    float raw_start_delta,
    float projected_start,
    float raw_end_delta,
    float projected_end,
    float projected_judgement_plane,
    float lane_center,
    float decoded_width,
    std::uint32_t phase_counter = 0) {
    constexpr float epsilon = 0.000001F;
    HoldBodyGeometry geometry{};
    geometry.mode = mode;
    geometry.left = presentation_lane_coordinate_lateral(
        lane_center - decoded_width * 0.5F);
    geometry.right = presentation_lane_coordinate_lateral(
        lane_center + decoded_width * 0.5F);
    geometry.resource_scale =
        mode == HoldPresentationMode::animated
            ? hold_animated_resource_scale(phase_counter)
            : 1.0F;

    const bool crosses_raw_origin =
        raw_start_delta * raw_end_delta < epsilon;
    float visibility_start = projected_start;
    const bool end_reaches_judgement_plane =
        -epsilon < projected_end - projected_judgement_plane;
    if (crosses_raw_origin && end_reaches_judgement_plane) {
        visibility_start = projected_judgement_plane;
    }

    if ((visibility_start < air_ladder_projection_far &&
         projected_end < air_ladder_projection_far) ||
        (air_ladder_projection_near < visibility_start &&
         air_ladder_projection_near < projected_end)) {
        geometry.enabled = false;
        return geometry;
    }

    float span = visibility_start - projected_end;
    if (span < 0.0F) {
        span = -span;
    }
    if (!(epsilon <= span)) {
        geometry.enabled = false;
        return geometry;
    }

    geometry.start_projected = std::clamp(
        projected_start, air_ladder_projection_far,
        air_ladder_projection_near);
    geometry.end_projected = std::clamp(
        projected_end, air_ladder_projection_far,
        air_ladder_projection_near);
    float start_trim = projected_start - geometry.start_projected;
    if (start_trim < 0.0F) {
        start_trim = -start_trim;
    }
    geometry.start_resource_coordinate = 1.0F - start_trim / span;
    float end_trim = geometry.end_projected - projected_end;
    if (end_trim < 0.0F) {
        end_trim = -end_trim;
    }
    geometry.end_resource_coordinate = end_trim / span;

    if (crosses_raw_origin &&
        (end_reaches_judgement_plane ||
         mode == HoldPresentationMode::animated)) {
        geometry.start_projected = projected_judgement_plane;
        const float raw_span = raw_end_delta - raw_start_delta;
        if (epsilon <= raw_span) {
            geometry.start_resource_coordinate = raw_end_delta / raw_span;
        }
    }
    return geometry;
}

constexpr std::uint32_t hold_body_color(
    HoldPresentationMode mode) {
    return mode == HoldPresentationMode::alternate
               ? presentation_static_alternate_color
               : presentation_static_base_color;
}

constexpr std::array<HoldBodyVertex, 6> build_hold_body_vertices(
    const HoldBodyGeometry& geometry) {
    const std::uint32_t color = hold_body_color(geometry.mode);
    const HoldBodyVertex start_left{
        geometry.left, 0.0F, geometry.start_projected, color, 0.0F,
        geometry.start_resource_coordinate};
    const HoldBodyVertex start_right{
        geometry.right, 0.0F, geometry.start_projected, color, 1.0F,
        geometry.start_resource_coordinate};
    const HoldBodyVertex end_left{
        geometry.left, 0.0F, geometry.end_projected, color, 0.0F,
        geometry.end_resource_coordinate};
    const HoldBodyVertex end_right{
        geometry.right, 0.0F, geometry.end_projected, color, 1.0F,
        geometry.end_resource_coordinate};
    return air_ladder_single_sided_quad(
        start_left, start_right, end_left, end_right,
        geometry.end_projected <= geometry.start_projected);
}

constexpr std::array<AirLadderGeometryVertex, 6>
build_air_ladder_stream_zero_vertices(const AirLadderGeometrySegment& segment,
                                      SharedAirPathPresentationMode mode) {
    const std::uint32_t color = shared_air_path_primary_color(mode);
    const float start_half =
        air_ladder_stream_zero_half_extent(segment.start.decoded_width);
    const float end_half =
        air_ladder_stream_zero_half_extent(segment.end.decoded_width);
    const auto start_left = air_ladder_geometry_vertex(
        segment.start, segment.start.lateral - start_half,
        segment.start.vertical, color, segment.start.normalized_left);
    const auto start_right = air_ladder_geometry_vertex(
        segment.start, segment.start.lateral + start_half,
        segment.start.vertical, color, segment.start.normalized_right);
    const auto end_left = air_ladder_geometry_vertex(
        segment.end, segment.end.lateral - end_half,
        segment.end.vertical, color, segment.end.normalized_left);
    const auto end_right = air_ladder_geometry_vertex(
        segment.end, segment.end.lateral + end_half,
        segment.end.vertical, color, segment.end.normalized_right);
    return air_ladder_single_sided_quad(
        start_left, start_right, end_left, end_right,
        segment.end.projected <= segment.start.projected);
}

constexpr std::array<AirLadderGeometryVertex, 6>
build_air_ladder_stream_one_vertices(const AirLadderGeometrySegment& segment,
                                     SharedAirPathPresentationMode mode) {
    const std::uint32_t color = shared_air_path_primary_color(mode);
    const float start_half = air_ladder_stream_one_half_extent(
        segment.start.decoded_width, segment.start.vertical);
    const float end_half = air_ladder_stream_one_half_extent(
        segment.end.decoded_width, segment.end.vertical);
    const auto start_left = air_ladder_geometry_vertex(
        segment.start, segment.start.lateral - start_half, 0.0F, color,
        segment.start.normalized_left);
    const auto start_right = air_ladder_geometry_vertex(
        segment.start, segment.start.lateral + start_half, 0.0F, color,
        segment.start.normalized_right);
    const auto end_left = air_ladder_geometry_vertex(
        segment.end, segment.end.lateral - end_half, 0.0F, color,
        segment.end.normalized_left);
    const auto end_right = air_ladder_geometry_vertex(
        segment.end, segment.end.lateral + end_half, 0.0F, color,
        segment.end.normalized_right);
    return air_ladder_single_sided_quad(
        start_left, start_right, end_left, end_right,
        segment.end.projected <= segment.start.projected);
}

constexpr std::array<AirLadderGeometryVertex, 12>
build_air_ladder_stream_two_vertices(const AirLadderGeometrySegment& segment,
                                     bool compact) {
    constexpr std::uint32_t color =
        presentation_static_low_alpha_color;
    const float half_extent =
        air_ladder_stream_two_half_extent(compact);
    const auto start_left = air_ladder_geometry_vertex(
        segment.start, segment.start.lateral - half_extent,
        segment.start.vertical, color, 0.0F);
    const auto start_right = air_ladder_geometry_vertex(
        segment.start, segment.start.lateral + half_extent,
        segment.start.vertical, color, 1.0F);
    const auto end_left = air_ladder_geometry_vertex(
        segment.end, segment.end.lateral - half_extent,
        segment.end.vertical, color, 0.0F);
    const auto end_right = air_ladder_geometry_vertex(
        segment.end, segment.end.lateral + half_extent,
        segment.end.vertical, color, 1.0F);
    return {
        start_left, end_left, end_right,
        start_left, end_right, start_right,
        start_left, end_right, end_left,
        start_left, start_right, end_right,
    };
}

// claim.note.slide-hld-heaven-retyping
//
// Slide field 8 uses an exact three-string table. SLD and any unrecognized
// value map to zero, HLD maps to one, and GRN maps to two. The ordinary
// post-parse path changes only code one from parsed type 2 to type 13 and
// normalizes the root/control path-scalar slots to ten. HeavenHold precompute
// consumes those integers as tenths, so the rewritten path lies at scalar
// 1.0. This is distinct from parsed +0xb0, which selects the HeavenHold
// mirror/geometry-mode tables and remains zero for a Slide-origin chain.
inline constexpr std::int32_t slide_sld_style_code = 0;
inline constexpr std::int32_t slide_hld_style_code = 1;
inline constexpr std::int32_t slide_grn_style_code = 2;
inline constexpr std::int32_t slide_heaven_path_scalar_tenths = 10;

constexpr std::int32_t parse_slide_style_code(std::string_view name) {
    if (name == "HLD") {
        return slide_hld_style_code;
    }
    if (name == "GRN") {
        return slide_grn_style_code;
    }
    return slide_sld_style_code;
}

constexpr bool slide_style_retypes_to_heaven_hold(
    std::int32_t style_code) {
    return style_code == slide_hld_style_code;
}

enum class SlideCommandForm : std::uint8_t {
    sld,
    sxd,
    slc,
    sxc,
};

constexpr bool slide_command_uses_extended_profile(SlideCommandForm command) {
    return command == SlideCommandForm::sxd || command == SlideCommandForm::sxc;
}

// The second command-form field is also copied into generated path boundary
// markers. D forms set it and C forms clear it.
constexpr bool slide_command_sets_path_marker(SlideCommandForm command) {
    return command == SlideCommandForm::sld ||
           command == SlideCommandForm::sxd;
}

// claim.note.slide-presentation-classes
//
// The command-form flag belongs to the root record. For an ordinary type-2
// Slide it selects the extended root-resource branch; resource identities are
// external and therefore are not reproduced here.
constexpr bool slide_root_uses_extended_resource(SlideCommandForm command) {
    return slide_command_uses_extended_profile(command);
}

// Slide field 9 uses the same exact eight-name decoder as HXD. It is copied
// only for an extended root and later selects one of two result-feedback
// tables. It does not select persistent path geometry.
constexpr std::int32_t slide_feedback_code(std::string_view name) {
    return c2s_hxd_subtype_code(name);
}

constexpr std::int32_t slide_bounded_style_resource_index(
    std::int32_t style_code) {
    return std::clamp(style_code, slide_sld_style_code,
                      slide_grn_style_code);
}

inline constexpr std::uint8_t slide_unresolved_result_table_index = 0xff;

constexpr bool slide_generated_endpoint_resource_present(
    bool ending_boundary_marker) {
    return ending_boundary_marker;
}

// The lazy endpoint allocator decodes generated-segment +0x14. The builder
// writes the ending control's width there; +0x10 is the preceding width.
constexpr std::int32_t slide_generated_endpoint_decoded_width(
    std::int32_t preceding_width,
    std::int32_t ending_width) {
    (void)preceding_width;
    return ending_width;
}

// Slide control construction copies the root width before optional fields are
// considered. The snapshot's comparison includes the command token, so it
// overwrites from field 7 when there are at least seven data fields. The
// accepted six-data-field legacy form retains the root width.
constexpr std::int32_t c2s_slide_endpoint_width(
    std::int32_t data_field_count,
    std::int32_t root_width,
    std::int32_t authored_endpoint_width) {
    const auto selected = data_field_count > 6
        ? authored_endpoint_width
        : root_width;
    return std::clamp(selected, std::int32_t{1}, std::int32_t{16});
}

// A due generated segment stores the current mapped result on marked
// endpoints and hardcodes four on shape-only segments. Shape-only segments do
// not own an endpoint resource, so the hardcoded value has no persistent
// endpoint-visibility consumer.
constexpr std::uint8_t slide_generated_segment_result_table_index(
    bool ending_boundary_marker,
    std::uint8_t mapped_result_table_index) {
    return ending_boundary_marker ? mapped_result_table_index : 4U;
}

constexpr bool slide_generated_endpoint_resource_visible(
    std::uint8_t result_table_index,
    std::size_t loaded_result_table_rows) {
    const auto narrowed_rows =
        static_cast<std::uint8_t>(loaded_result_table_rows);
    return !(result_table_index < narrowed_rows);
}

template <typename ResourceId>
constexpr ResourceId select_slide_feedback_resource(
    std::int32_t feedback_code,
    bool alternate_table,
    const std::array<ResourceId, 8>& primary,
    const std::array<ResourceId, 8>& alternate,
    ResourceId unavailable) {
    if (static_cast<std::uint32_t>(feedback_code) >= primary.size()) {
        return unavailable;
    }
    const auto index = static_cast<std::size_t>(feedback_code);
    return alternate_table ? alternate[index] : primary[index];
}

// claim.note.heaven-hold-judgement
//
// Parsed type 13 (HHD/HHX and retyped HLD-styled Slide) and ALD's exact
// zero/NON exception construct the same HeavenHoldNote class. The parsed-record
// constructor clears the primary generated vector. Chart postprocessing fills
// it for type 13, but deliberately leaves it empty for the exceptional parsed
// type-9 ALD path.
enum class HeavenHoldCommand : std::uint8_t {
    ald_zero_non,
    hhd,
    hhx,
    slide_hld_standard,
    slide_hld_extended,
};

constexpr std::int32_t heaven_hold_parsed_type(HeavenHoldCommand command) {
    return command == HeavenHoldCommand::ald_zero_non ? 9 : 13;
}

constexpr bool heaven_hold_generates_path_records(HeavenHoldCommand command) {
    return heaven_hold_parsed_type(command) == 13;
}

// claim.presentation.heaven-hold-authored-mesh
//
// HHD/HHX token 10 is stored at parsed +0xb0. Two embedded five-byte tables
// consume it. Values outside 0..4 use the explicit fallback. Retyped HLD and
// exceptional zero/NON ALD retain constructor-default +0xb0 == 0; the HLD
// rewrite's separate value 10 is the tenths path scalar above.
inline constexpr std::array<std::uint8_t, 5>
    heaven_hold_endpoint_mirror_flags{{0, 1, 0, 1, 0}};
inline constexpr std::array<std::uint8_t, 5>
    heaven_hold_geometry_modes{{0, 0, 1, 1, 0x83}};

constexpr bool heaven_hold_mirrors_authored_endpoints(
    std::int32_t parsed_selector) {
    return 0 <= parsed_selector &&
                   parsed_selector < static_cast<std::int32_t>(
                       heaven_hold_endpoint_mirror_flags.size())
               ? heaven_hold_endpoint_mirror_flags[
                     static_cast<std::size_t>(parsed_selector)] != 0
               : false;
}

constexpr std::uint8_t heaven_hold_geometry_mode(
    std::int32_t parsed_selector) {
    return 0 <= parsed_selector &&
                   parsed_selector < static_cast<std::int32_t>(
                       heaven_hold_geometry_modes.size())
               ? heaven_hold_geometry_modes[
                     static_cast<std::size_t>(parsed_selector)]
               : 0x83;
}

constexpr float heaven_hold_retyped_hld_path_scalar() {
    return static_cast<float>(slide_heaven_path_scalar_tenths) * 0.1F;
}

struct HeavenHoldAuthoredPresentationPoint {
    float scheduled{};
    float lane{};
    float decoded_width{1.0F};
    float path_scalar{1.0F};
    std::int32_t sla_key{};
};

inline std::vector<HeavenHoldAuthoredPresentationPoint>
build_heaven_hold_authored_presentation_path(
    const HeavenHoldAuthoredPresentationPoint& root,
    std::span<const HeavenHoldAuthoredPresentationPoint> authored_endpoints,
    std::int32_t parsed_selector) {
    std::vector<HeavenHoldAuthoredPresentationPoint> result;
    if (authored_endpoints.empty()) {
        return result;
    }
    result.reserve(authored_endpoints.size() + 1);
    result.push_back(root);
    const float root_center = root.lane + root.decoded_width * 0.5F;
    const bool mirror =
        heaven_hold_mirrors_authored_endpoints(parsed_selector);
    for (auto endpoint : authored_endpoints) {
        if (mirror) {
            endpoint.lane = root_center * 2.0F - endpoint.lane -
                            endpoint.decoded_width;
        }
        result.push_back(endpoint);
    }
    return result;
}

struct HeavenHoldGeometryEndpoint {
    float raw_relative{};
    float projected{};
    float lateral{};
    float vertical{};
    float decoded_width{1.0F};
    float resource_coordinate{};
};

struct HeavenHoldGeometrySegment {
    bool enabled{true};
    HeavenHoldGeometryEndpoint start{};
    HeavenHoldGeometryEndpoint end{};
};

constexpr HeavenHoldGeometryEndpoint heaven_hold_mix_endpoint(
    const HeavenHoldGeometryEndpoint& from,
    const HeavenHoldGeometryEndpoint& to,
    float fraction) {
    return {
        air_ladder_mix(from.raw_relative, to.raw_relative, fraction),
        air_ladder_mix(from.projected, to.projected, fraction),
        air_ladder_mix(from.lateral, to.lateral, fraction),
        air_ladder_mix(from.vertical, to.vertical, fraction),
        air_ladder_mix(from.decoded_width, to.decoded_width, fraction),
        air_ladder_mix(from.resource_coordinate,
                       to.resource_coordinate, fraction),
    };
}

// When the first raw-relative point is negative and a later point reaches
// zero, a nonzero geometry mode discards preceding points and inserts the
// exact linearly interpolated origin. With no crossing, the source leaves the
// sequence unchanged; the owning path phase controls final hiding.
inline std::vector<HeavenHoldGeometryEndpoint>
trim_heaven_hold_nonzero_mode_at_origin(
    std::span<const HeavenHoldGeometryEndpoint> points) {
    if (points.size() < 2 || !(points.front().raw_relative < 0.0F)) {
        return {points.begin(), points.end()};
    }
    for (std::size_t index = 0; index + 1 < points.size(); ++index) {
        const auto& left = points[index];
        const auto& right = points[index + 1];
        if (left.raw_relative < 0.0F &&
            0.0F <= right.raw_relative) {
            const float span = right.raw_relative - left.raw_relative;
            const float fraction =
                span == 0.0F ? 0.0F : -left.raw_relative / span;
            auto origin = heaven_hold_mix_endpoint(left, right, fraction);
            origin.raw_relative = 0.0F;
            std::vector<HeavenHoldGeometryEndpoint> result;
            result.reserve(points.size() - index);
            result.push_back(origin);
            result.insert(result.end(), points.begin() + index + 1,
                          points.end());
            return result;
        }
    }
    return {points.begin(), points.end()};
}

// For nonzero parsed geometry mode, the builder reverses the surviving record
// sequence and reflects projected/lateral/vertical/resource coordinates about
// the first/final midpoint. Raw-relative values and decoded widths are merely
// reversed. The endpoint values therefore remain fixed while interior bends
// are reflected.
inline std::vector<HeavenHoldGeometryEndpoint>
reflect_heaven_hold_nonzero_mode_geometry(
    std::span<const HeavenHoldGeometryEndpoint> points) {
    if (points.size() < 2) {
        return {points.begin(), points.end()};
    }
    const auto& first = points.front();
    const auto& last = points.back();
    std::vector<HeavenHoldGeometryEndpoint> result;
    result.reserve(points.size());
    for (auto cursor = points.rbegin(); cursor != points.rend(); ++cursor) {
        result.push_back({
            cursor->raw_relative,
            first.projected + last.projected - cursor->projected,
            first.lateral + last.lateral - cursor->lateral,
            first.vertical + last.vertical - cursor->vertical,
            cursor->decoded_width,
            first.resource_coordinate + last.resource_coordinate -
                cursor->resource_coordinate,
        });
    }
    return result;
}

constexpr void heaven_hold_clip_endpoint(
    HeavenHoldGeometryEndpoint& endpoint,
    const HeavenHoldGeometryEndpoint& opposite,
    float projected_boundary,
    float fraction) {
    endpoint.projected = projected_boundary;
    endpoint.lateral =
        air_ladder_mix(endpoint.lateral, opposite.lateral, fraction);
    endpoint.vertical =
        air_ladder_mix(endpoint.vertical, opposite.vertical, fraction);
    endpoint.decoded_width = air_ladder_mix(
        endpoint.decoded_width, opposite.decoded_width, fraction);
    endpoint.resource_coordinate = air_ladder_mix(
        endpoint.resource_coordinate, opposite.resource_coordinate, fraction);
}

constexpr HeavenHoldGeometrySegment clip_heaven_hold_geometry_segment(
    HeavenHoldGeometrySegment segment) {
    const auto original_start = segment.start;
    const auto original_end = segment.end;
    const float start_projected = original_start.projected;
    const float end_projected = original_end.projected;
    if ((start_projected < air_ladder_projection_far &&
         end_projected < air_ladder_projection_far) ||
        (air_ladder_projection_near < start_projected &&
         air_ladder_projection_near < end_projected)) {
        segment.enabled = false;
        return segment;
    }
    float span = start_projected - end_projected;
    if (span < 0.0F) {
        span = -span;
    }
    if (!(air_ladder_clip_minimum_span <= span)) {
        return segment;
    }
    if (air_ladder_projection_near < start_projected) {
        heaven_hold_clip_endpoint(
            segment.start, original_end, air_ladder_projection_near,
            air_ladder_clamp_unit(
                (start_projected - air_ladder_projection_near) / span));
    }
    if (air_ladder_projection_near < end_projected) {
        heaven_hold_clip_endpoint(
            segment.end, original_start, air_ladder_projection_near,
            air_ladder_clamp_unit(
                (end_projected - air_ladder_projection_near) / span));
    }
    if (start_projected < air_ladder_projection_far) {
        heaven_hold_clip_endpoint(
            segment.start, original_end, air_ladder_projection_far,
            air_ladder_clamp_unit(
                (air_ladder_projection_far - start_projected) / span));
    }
    if (end_projected < air_ladder_projection_far) {
        heaven_hold_clip_endpoint(
            segment.end, original_start, air_ladder_projection_far,
            air_ladder_clamp_unit(
                (air_ladder_projection_far - end_projected) / span));
    }
    return segment;
}

using HeavenHoldGeometryVertex = AirLadderGeometryVertex;
static_assert(sizeof(HeavenHoldGeometryVertex) == 0x18);

enum class HeavenHoldPresentationMode : std::uint8_t {
    base = 0,
    animated = 1,
    alternate = 2,
};

constexpr float heaven_hold_geometry_half_extent(float decoded_width) {
    return air_ladder_render_lateral(decoded_width + 8.0F) * 0.5F;
}

constexpr HeavenHoldGeometryVertex heaven_hold_geometry_vertex(
    const HeavenHoldGeometryEndpoint& endpoint,
    float lateral,
    std::uint32_t color,
    float coordinate_u) {
    return {
        lateral,
        endpoint.vertical,
        endpoint.projected,
        color,
        coordinate_u,
        endpoint.resource_coordinate,
    };
}

constexpr std::array<HeavenHoldGeometryVertex, 6>
build_heaven_hold_geometry_vertices(
    const HeavenHoldGeometrySegment& segment,
    HeavenHoldPresentationMode mode) {
    const std::uint32_t color =
        mode == HeavenHoldPresentationMode::alternate
            ? presentation_static_alternate_color
            : presentation_static_base_color;
    const float start_half =
        heaven_hold_geometry_half_extent(segment.start.decoded_width);
    const float end_half =
        heaven_hold_geometry_half_extent(segment.end.decoded_width);
    const auto start_left = heaven_hold_geometry_vertex(
        segment.start, segment.start.lateral - start_half,
        color, 0.0F);
    const auto start_right = heaven_hold_geometry_vertex(
        segment.start, segment.start.lateral + start_half,
        color, 1.0F);
    const auto end_left = heaven_hold_geometry_vertex(
        segment.end, segment.end.lateral - end_half,
        color, 0.0F);
    const auto end_right = heaven_hold_geometry_vertex(
        segment.end, segment.end.lateral + end_half,
        color, 1.0F);
    return air_ladder_single_sided_quad(
        start_left, start_right, end_left, end_right,
        segment.end.projected <= segment.start.projected);
}

constexpr bool heaven_hold_uses_secondary_body_resource(
    std::int32_t attached_secondary_type) {
    return attached_secondary_type == 3 ||
           attached_secondary_type == 5 ||
           attached_secondary_type == 8;
}

// HHD, exceptional ALD, and retyped SLD/SLC retain profile family 0. HHX and
// retyped SXD/SXC retain the set parsed command-form flag and select family 4.
// Each family then selects its upper member when the clamped runtime input
// variant is at least two.
constexpr std::int32_t heaven_hold_start_profile(
    HeavenHoldCommand command,
    bool input_variant_at_least_two) {
    const bool extended =
        command == HeavenHoldCommand::hhx ||
        command == HeavenHoldCommand::slide_hld_extended;
    const std::int32_t base = extended ? 2 : 0;
    return base + (input_variant_at_least_two ? 1 : 0);
}

using HeavenHoldPathPoint = AirHoldPathPoint;

enum class HeavenHoldGeneratedRecordKind : std::int32_t {
    sample = 0,
    path_end = 1,
};

struct HeavenHoldGeneratedPathRecord {
    HeavenHoldPathPoint point{};
    HeavenHoldGeneratedRecordKind kind{};
    bool emission_enabled{};
};

// The type-13 parser accepts a signed duration token without a range gate. The
// producer wraps endpoint-root at i32 width and compares the result as uint32.
// A delta with its high bit set enters a very large unsigned generation range;
// signed endpoint ordering alone is insufficient at the wrap boundary.
enum class HeavenHoldPathGenerationDisposition : std::uint8_t {
    no_generated_path,
    generated,
    source_large_unsigned_span_expansion,
};

constexpr HeavenHoldPathGenerationDisposition
evaluate_heaven_hold_path_generation(
    HeavenHoldCommand command,
    const HeavenHoldPathPoint& root,
    const HeavenHoldPathPoint& final_endpoint) {
    if (!heaven_hold_generates_path_records(command)) {
        return HeavenHoldPathGenerationDisposition::no_generated_path;
    }
    const std::uint32_t delta = air_path_unsigned_tick_delta(
        air_hold_grid_tick(root), air_hold_grid_tick(final_endpoint));
    return delta > static_cast<std::uint32_t>(
                       std::numeric_limits<std::int32_t>::max())
               ? HeavenHoldPathGenerationDisposition::
                     source_large_unsigned_span_expansion
               : HeavenHoldPathGenerationDisposition::generated;
}

// Type 13 samples a single root-to-final-end span. Cadence starts one adaptive
// step after the root; ordinary samples default enabled and can only be cleared
// by an enabled selector-0 open interval. The final record is always kind 1 and
// enabled. Callers must use evaluate_heaven_hold_path_generation before this
// producer when accepting malformed authored input.
template <typename TempoAtSchedule,
          typename PointAtGridTick,
          typename ScheduleAtPoint>
std::vector<HeavenHoldGeneratedPathRecord>
generate_heaven_hold_path_records(
    HeavenHoldCommand command,
    const HeavenHoldPathPoint& root,
    const HeavenHoldPathPoint& final_endpoint,
    float parser_reference,
    bool exclusion_filter_enabled,
    std::span<const AirHoldExclusionInterval> exclusion_intervals,
    TempoAtSchedule tempo_at_schedule,
    PointAtGridTick point_at_grid_tick,
    ScheduleAtPoint schedule_at_point) {
    std::vector<HeavenHoldGeneratedPathRecord> records;
    if (!heaven_hold_generates_path_records(command)) {
        return records;
    }

    const std::int32_t start_tick = air_hold_grid_tick(root);
    const std::int32_t delta = subtract_i32_wrapped(
        air_hold_grid_tick(final_endpoint), start_tick);
    std::int32_t offset = air_hold_sample_step(
        tempo_at_schedule(root.scheduled), parser_reference);
    while (static_cast<std::uint32_t>(offset) <
           static_cast<std::uint32_t>(delta)) {
        HeavenHoldPathPoint point = point_at_grid_tick(
            add_i32_wrapped(start_tick, offset));
        point.scheduled = schedule_at_point(point);
        const bool excluded =
            exclusion_filter_enabled &&
            air_hold_in_exclusion_interval(point.scheduled,
                                           exclusion_intervals);
        records.push_back({point, HeavenHoldGeneratedRecordKind::sample,
                           !excluded});
        offset = add_i32_wrapped(
            offset,
            air_hold_sample_step(
                tempo_at_schedule(point.scheduled), parser_reference));
    }

    records.push_back({final_endpoint,
                       HeavenHoldGeneratedRecordKind::path_end, true});
    return records;
}

struct HeavenHoldLaneContactState {
    std::array<bool, 2> admitted{};
};

struct HeavenHoldLaneContactDecision {
    std::array<bool, 2> admission_checked{};
    std::array<bool, 2> active{};
    bool any_active{};
};

// Each lane has two physical banks. Admission checks are ordered and stop at
// the first asserted bank that passes. If one passes, asserted banks latch;
// otherwise deasserted banks latch. Current physical activity requires both the
// retained latch and an asserted source. Forced activity is folded only into
// the gap tracker's aggregate and does not rewrite the physical-bank bytes.
constexpr HeavenHoldLaneContactDecision update_heaven_hold_lane_contact(
    HeavenHoldLaneContactState& state,
    bool at_or_after_start,
    const std::array<bool, 2>& raw_active,
    const std::array<bool, 2>& admission_passes,
    bool forced_active) {
    HeavenHoldLaneContactDecision decision{};
    bool any_admitted = false;
    for (std::size_t bank = 0; bank < raw_active.size(); ++bank) {
        if (!raw_active[bank]) {
            continue;
        }
        decision.admission_checked[bank] = true;
        if (admission_passes[bank]) {
            any_admitted = true;
            break;
        }
    }

    for (std::size_t bank = 0; bank < raw_active.size(); ++bank) {
        state.admitted[bank] =
            state.admitted[bank] ||
            (any_admitted ? raw_active[bank] : !raw_active[bank]);
        decision.active[bank] =
            at_or_after_start && state.admitted[bank] && raw_active[bank];
        decision.any_active = decision.any_active || decision.active[bank];
    }
    decision.any_active = decision.any_active || forced_active;
    return decision;
}

enum class HeavenHoldStartPhase : std::uint8_t {
    awaiting_result = 0,
    complete = 4,
};

enum class HeavenHoldPathPhase : std::uint8_t {
    before_start = 0,
    best_current_gap = 2,
    other_current_gap = 3,
    complete = 4,
};

constexpr bool heaven_hold_start_resource_visible(
    HeavenHoldStartPhase start) {
    return start != HeavenHoldStartPhase::complete;
}

constexpr bool heaven_hold_body_resource_visible(
    HeavenHoldPathPhase path) {
    return path != HeavenHoldPathPhase::complete;
}

constexpr HeavenHoldPresentationMode heaven_hold_presentation_mode(
    HeavenHoldStartPhase start,
    HeavenHoldPathPhase path) {
    if (path == HeavenHoldPathPhase::best_current_gap) {
        return HeavenHoldPresentationMode::animated;
    }
    if (start == HeavenHoldStartPhase::complete) {
        return path == HeavenHoldPathPhase::other_current_gap
                   ? HeavenHoldPresentationMode::alternate
                   : HeavenHoldPresentationMode::animated;
    }
    return HeavenHoldPresentationMode::base;
}

// Mode 1 retains the default externally selected packed color and animates a
// resource scale. Mode 2 selects the alternate packed color. The exact scale
// expression is executable-owned; its phase counter is an explicit input.
inline float heaven_hold_animated_resource_scale(
    std::uint32_t phase_counter) {
    return hold_animated_resource_scale(phase_counter);
}

constexpr HeavenHoldPathPhase heaven_hold_path_phase(
    std::int32_t current_gap_index,
    std::size_t generated_remaining) {
    if (generated_remaining == 0) {
        return HeavenHoldPathPhase::complete;
    }
    return current_gap_index == 0 ? HeavenHoldPathPhase::best_current_gap
                                  : HeavenHoldPathPhase::other_current_gap;
}

constexpr bool heaven_hold_is_terminal(HeavenHoldStartPhase start,
                                        HeavenHoldPathPhase path) {
    return start == HeavenHoldStartPhase::complete &&
           path == HeavenHoldPathPhase::complete;
}

constexpr bool heaven_hold_exposes_candidate(HeavenHoldStartPhase start) {
    return start != HeavenHoldStartPhase::complete;
}

constexpr std::int32_t heaven_hold_start_source_category(
    bool variant_enabled,
    bool variant_suppressed) {
    return variant_enabled && !variant_suppressed ? 1 : 0;
}

constexpr std::int32_t heaven_hold_generated_source_category(
    HeavenHoldGeneratedRecordKind kind) {
    return kind == HeavenHoldGeneratedRecordKind::path_end ? 3 : 2;
}

struct HeavenHoldGeneratedCheckpointDecision {
    bool consumed{};
    bool emit{};
    bool ordinary_result_valid{};
    bool reset_maximum_gap{};
    std::uint8_t ordinary_result_byte{};
    std::int32_t source_category{};
};

// A due front record is the only generated record consumed in one substep.
// Ordinary mode classifies and resets the retained maximum before the emission
// flag is consulted, so disabled samples remain stateful. Forced selection
// bypasses the ordinary classification/reset but still consumes the front.
constexpr HeavenHoldGeneratedCheckpointDecision
update_heaven_hold_generated_checkpoint(
    HoldGapState& gap,
    bool front_due,
    HeavenHoldGeneratedRecordKind kind,
    bool emission_enabled,
    bool forced_result_active) {
    HeavenHoldGeneratedCheckpointDecision decision{};
    if (!front_due) {
        return decision;
    }

    decision.consumed = true;
    decision.emit = emission_enabled;
    decision.source_category = heaven_hold_generated_source_category(kind);
    if (!forced_result_active) {
        decision.ordinary_result_valid = true;
        decision.ordinary_result_byte =
            hold_gap_result_byte(classify_hold_gap(gap));
        reset_hold_gap_after_checkpoint(gap);
        decision.reset_maximum_gap = true;
    }
    return decision;
}

// The once-only adjusted-end branch is indexed feedback only. It does not
// submit through the shared result dispatcher and does not complete a phase.
constexpr bool heaven_hold_end_feedback_submits_result() {
    return false;
}

// claim.note.slide-path-sustain-judgement
//
// SLD/SXD/SLC/SXC records use the shared TAP start checker, then independently
// track a generated path. Both path-segment and per-lane windows use this
// six-state classifier. The four endpoints stay explicit because the builder
// derives them from external configuration and parsed control points.
enum class SlideWindowPhase : std::uint8_t {
    disabled = 0,
    before = 1,
    after = 2,
    early = 3,
    late = 4,
    center = 5,
};

constexpr SlideWindowPhase classify_slide_window(
    bool enabled,
    float current,
    const std::array<float, 4>& edges) {
    if (!enabled) {
        return SlideWindowPhase::disabled;
    }
    if (current < edges[0]) {
        return SlideWindowPhase::before;
    }
    if (current < edges[1]) {
        return SlideWindowPhase::early;
    }
    if (current <= edges[2]) {
        return SlideWindowPhase::center;
    }
    if (current <= edges[3]) {
        return SlideWindowPhase::late;
    }
    return SlideWindowPhase::after;
}

constexpr bool slide_window_participates(SlideWindowPhase phase) {
    return phase == SlideWindowPhase::early ||
           phase == SlideWindowPhase::late ||
           phase == SlideWindowPhase::center;
}

// Overlapping generated path records are reduced by their literal numeric
// phase code; center therefore dominates late, which dominates early.
constexpr SlideWindowPhase combine_slide_window_phases(
    SlideWindowPhase left,
    SlideWindowPhase right) {
    return static_cast<std::uint8_t>(left) <
                   static_cast<std::uint8_t>(right)
               ? right
               : left;
}

// The type-2 path builder selects one external five-float profile by clamped
// width. The first value produces a lane-anchor offset; the remaining four are
// relative window endpoints. Their values are runtime-loaded and therefore
// stay explicit clean-room inputs.
struct SlideEndpointProfile {
    float lane_reference{};
    std::array<float, 4> edge_offsets{};
};

struct SlidePathPoint {
    float scheduled_position{};
    std::int32_t lane{};
    std::int32_t width{1};
    bool marker{};
};

struct SlideGeneratedLaneWindow {
    bool enabled{};
    std::array<float, 4> edges{};
};

struct SlideGeneratedSegment {
    float start_position{};
    float end_position{};
    bool final_segment{};
    bool start_marker{};
    bool end_marker{};
    std::array<float, 4> edges{};
    std::array<SlideGeneratedLaneWindow, 16> lane_windows{};
};

struct SlideGeneratedPath {
    bool enabled{};
    std::array<float, 4> edges{};
    std::vector<SlideGeneratedSegment> segments{};
};

// SlideNote uses the parsed +0x84 key with the source map's checked lookup.
// A missing key calls the standard out-of-range routine; there is no fallback
// path container or disabled-path substitute.
inline const SlideGeneratedPath& require_slide_generated_path(
    const std::map<std::uint32_t, SlideGeneratedPath>& paths,
    std::uint32_t key) {
    const auto found = paths.find(key);
    if (found == paths.end()) {
        throw std::out_of_range("missing slide generated-path key");
    }
    return found->second;
}

constexpr std::size_t slide_endpoint_profile_index(std::int32_t width) {
    const std::int32_t bounded = std::clamp(width, 1, 16);
    return static_cast<std::size_t>(16 - bounded);
}

constexpr float slide_lane_anchor_offset(
    const SlidePathPoint& point,
    const std::array<SlideEndpointProfile, 16>& profiles) {
    const auto& profile = profiles[slide_endpoint_profile_index(point.width)];
    return (profile.lane_reference - static_cast<float>(point.lane)) * 0.5F;
}

constexpr std::array<float, 4> slide_point_window_edges(
    const SlidePathPoint& point,
    const std::array<SlideEndpointProfile, 16>& profiles,
    float endpoint_correction) {
    const auto& offsets =
        profiles[slide_endpoint_profile_index(point.width)].edge_offsets;
    return {
        point.scheduled_position + offsets[0] + endpoint_correction,
        point.scheduled_position + offsets[1] + endpoint_correction,
        point.scheduled_position + offsets[2] + endpoint_correction,
        point.scheduled_position + offsets[3] + endpoint_correction,
    };
}

constexpr bool slide_lane_in_point_span(std::int32_t lane,
                                        const SlidePathPoint& point) {
    const std::int32_t bounded_width = std::clamp(point.width, 1, 16);
    return point.lane <= lane &&
           lane < add_i32_wrapped(point.lane, bounded_width);
}

constexpr bool slide_lane_in_swept_span(std::int32_t lane,
                                        const SlidePathPoint& start,
                                        const SlidePathPoint& end) {
    const std::int32_t start_end =
        add_i32_wrapped(start.lane, std::clamp(start.width, 1, 16));
    const std::int32_t end_end =
        add_i32_wrapped(end.lane, std::clamp(end.width, 1, 16));
    return (start.lane <= lane || end.lane <= lane) &&
           (lane < start_end || lane < end_end);
}

constexpr SlideGeneratedLaneWindow build_slide_lane_window(
    std::int32_t lane,
    const SlidePathPoint& start,
    const SlidePathPoint& end,
    const std::array<SlideEndpointProfile, 16>& profiles,
    float endpoint_correction) {
    SlideGeneratedLaneWindow result{};
    if (!slide_lane_in_swept_span(lane, start, end)) {
        return result;
    }

    result.enabled = true;
    const auto start_edges =
        slide_point_window_edges(start, profiles, endpoint_correction);
    const auto end_edges =
        slide_point_window_edges(end, profiles, endpoint_correction);
    result.edges = {start_edges[0], start_edges[1],
                    end_edges[2], end_edges[3]};

    const bool in_start = slide_lane_in_point_span(lane, start);
    const bool in_end = slide_lane_in_point_span(lane, end);
    if (in_start && in_end) {
        return result;
    }

    const float center = static_cast<float>(lane) + 0.5F;
    const float start_offset = slide_lane_anchor_offset(start, profiles);
    const float end_offset = slide_lane_anchor_offset(end, profiles);
    const std::int32_t start_end_lane =
        add_i32_wrapped(start.lane, std::clamp(start.width, 1, 16));
    const std::int32_t end_end_lane =
        add_i32_wrapped(end.lane, std::clamp(end.width, 1, 16));

    const bool use_lower_boundaries = lane < start_end_lane;
    const float start_anchor =
        use_lower_boundaries
            ? static_cast<float>(start.lane) - start_offset
            : static_cast<float>(start_end_lane) + start_offset;
    const float end_anchor =
        use_lower_boundaries
            ? static_cast<float>(end.lane) - end_offset
            : static_cast<float>(end_end_lane) + end_offset;
    const float start_distance = std::fabs(center - start_anchor);
    const float end_distance = std::fabs(center - end_anchor);
    const float inverse_total = 1.0F / (start_distance + end_distance);
    const float start_weight = inverse_total * end_distance;
    const float end_weight = inverse_total * start_distance;
    const float interpolated_position =
        end_weight * end.scheduled_position +
        start_weight * start.scheduled_position;
    const auto& start_offsets =
        profiles[slide_endpoint_profile_index(start.width)].edge_offsets;
    const auto& end_offsets =
        profiles[slide_endpoint_profile_index(end.width)].edge_offsets;

    if (!in_start) {
        result.edges[0] =
            end_offsets[0] * end_weight +
            start_offsets[0] * start_weight +
            endpoint_correction + interpolated_position;
        result.edges[1] =
            end_offsets[1] * end_weight +
            start_offsets[1] * start_weight +
            endpoint_correction + interpolated_position;
    }
    if (!in_end) {
        result.edges[2] =
            start_offsets[2] * start_weight +
            end_offsets[2] * end_weight +
            endpoint_correction + interpolated_position;
        result.edges[3] =
            start_offsets[3] * start_weight +
            end_offsets[3] * end_weight +
            endpoint_correction + interpolated_position;
    }
    return result;
}

inline SlideGeneratedPath build_slide_generated_path(
    const SlidePathPoint& root,
    std::span<const SlidePathPoint> controls,
    const std::array<SlideEndpointProfile, 16>& profiles,
    float endpoint_correction) {
    SlideGeneratedPath result{};
    if (controls.empty()) {
        return result;
    }

    result.enabled = true;
    const auto root_edges =
        slide_point_window_edges(root, profiles, endpoint_correction);
    const auto final_edges =
        slide_point_window_edges(controls.back(), profiles,
                                 endpoint_correction);
    result.edges = {root_edges[0], root_edges[1],
                    final_edges[2], final_edges[3]};
    result.segments.reserve(controls.size());

    SlidePathPoint start = root;
    start.marker = true;
    for (std::size_t index = 0; index < controls.size(); ++index) {
        const auto& end = controls[index];
        SlideGeneratedSegment segment{
            .start_position = start.scheduled_position,
            .end_position = end.scheduled_position,
            .final_segment = index + 1U == controls.size(),
            .start_marker = start.marker,
            .end_marker =
                index + 1U == controls.size() ? true : end.marker,
            .edges = {
                slide_point_window_edges(start, profiles,
                                         endpoint_correction)[0],
                slide_point_window_edges(start, profiles,
                                         endpoint_correction)[1],
                slide_point_window_edges(end, profiles,
                                         endpoint_correction)[2],
                slide_point_window_edges(end, profiles,
                                         endpoint_correction)[3],
            },
        };
        for (std::int32_t lane = 0; lane < 16; ++lane) {
            segment.lane_windows[static_cast<std::size_t>(lane)] =
                build_slide_lane_window(lane, start, end, profiles,
                                        endpoint_correction);
        }
        result.segments.push_back(segment);
        start = end;
    }
    return result;
}

// Early and late path phases preserve the same two-bank arming state as HOLD,
// but only a center phase can make a bank active and write its continuation
// marker. Leaving the participating phases clears both bank arrays.
constexpr bool update_slide_lane_sources(
    HoldLaneSourceState& state,
    const std::array<bool, 2>& current_level,
    const std::array<bool, 2>& previous_sustain_marker,
    SlideWindowPhase phase,
    std::array<bool, 2>& current_sustain_marker) {
    if (!slide_window_participates(phase)) {
        state.armed = {};
        state.active = {};
        return false;
    }
    return update_hold_lane_sources(
        state, current_level, previous_sustain_marker,
        phase == SlideWindowPhase::center, current_sustain_marker);
}

enum class SlideStartPhase : std::uint8_t {
    awaiting_result = 0,
    resolved = 4,
};

enum class SlidePathPhase : std::uint8_t {
    before_start = 0,
    best_current_gap = 2,
    other_current_gap = 3,
    complete = 4,
};

// claim.note.slide-path-presentation-geometry
//
// SlideNote derives this literal three-state presentation input from its two
// gameplay components before calling JointSlide. Names describe only the
// observed geometry consequences; no player-facing state label is inferred.
enum class SlidePresentationMode : std::uint8_t {
    base = 0,
    hide_past_with_overlay = 1,
    alternate_color = 2,
};

constexpr SlidePresentationMode slide_presentation_mode(
    SlideStartPhase start,
    SlidePathPhase path) {
    if (path == SlidePathPhase::best_current_gap) {
        return SlidePresentationMode::hide_past_with_overlay;
    }
    if (start == SlideStartPhase::resolved) {
        return path == SlidePathPhase::other_current_gap
                   ? SlidePresentationMode::alternate_color
                   : SlidePresentationMode::hide_past_with_overlay;
    }
    return SlidePresentationMode::base;
}

inline constexpr float slide_geometry_epsilon = 0.000001F;
inline constexpr float slide_width_change_epsilon = 0.00000011920929F;
inline constexpr float slide_projection_far = -600.0F;
inline constexpr float slide_projection_near = 50.0F;
inline constexpr float slide_inner_width_scale = 0.7F;
inline constexpr float slide_inner_coordinate_left = 0.15F;
inline constexpr float slide_inner_coordinate_right = 0.85F;
inline constexpr float slide_center_stream_half_extent = 2.0F;
inline constexpr std::uint32_t slide_base_main_stream_color =
    presentation_static_base_color;
inline constexpr std::uint32_t slide_alternate_main_stream_color =
    presentation_static_alternate_color;
inline constexpr std::uint32_t slide_center_stream_color =
    presentation_static_base_color;
inline constexpr std::uint32_t slide_shared_low_alpha_color =
    presentation_static_low_alpha_color;
inline constexpr std::uint32_t slide_overlay_stream_color = 0x20ffffffU;
inline constexpr std::array<std::int32_t, 3>
    slide_primitive_counter_categories{1, 2, 3};
inline constexpr std::array<std::int32_t, 3>
    slide_stream_topology_modes{4, 3, 3};
inline constexpr std::array<std::int32_t, 3>
    slide_joint_submission_order{0, 1, 2};

struct SlidePresentationPoint {
    float decoded_width{};
    float lane_center{};
    bool marker{};
};

struct SlidePresentationSegment {
    bool visible{true};
    bool crosses_judgement{};
    float raw_start{};
    float raw_end{};
    float projected_start{};
    float projected_end{};
    float lateral_start{};
    float lateral_end{};
    float width_start{};
    float width_end{};
    bool start_marker{};
    bool end_marker{};
    float coordinate_start{1.0F};
    float coordinate_end{};
};

struct SlidePresentationGeometry {
    bool cardinality_valid{};
    std::vector<SlidePresentationSegment> segments{};
};

struct SlideGeometryVertex {
    float lateral{};
    float vertical{};
    float projected{};
    std::uint32_t color{};
    float coordinate_u{};
    float coordinate_v{};
};

constexpr float slide_render_lateral(float value) {
    return presentation_lane_coordinate_lateral(value);
}

constexpr float slide_render_half_width(float decoded_width) {
    return slide_render_lateral(decoded_width + 8.0F) * 0.5F;
}

constexpr float slide_clamp_unit(float value) {
    return std::clamp(value, 0.0F, 1.0F);
}

constexpr void interpolate_slide_segment_start(
    SlidePresentationSegment& segment,
    float fraction,
    float projected) {
    const float retained = 1.0F - fraction;
    segment.lateral_start =
        segment.lateral_start * retained + fraction * segment.lateral_end;
    segment.width_start =
        segment.width_start * retained + fraction * segment.width_end;
    segment.coordinate_start = segment.coordinate_start * retained +
                               fraction * segment.coordinate_end;
    segment.projected_start = projected;
}

constexpr void interpolate_slide_segment_end(
    SlidePresentationSegment& segment,
    float fraction,
    float projected) {
    const float retained = 1.0F - fraction;
    segment.lateral_end =
        segment.lateral_end * retained + fraction * segment.lateral_start;
    segment.width_end =
        segment.width_end * retained + fraction * segment.width_start;
    segment.coordinate_end = segment.coordinate_end * retained +
                             fraction * segment.coordinate_start;
    segment.projected_end = projected;
}

constexpr void clip_slide_presentation_segment(
    SlidePresentationSegment& segment) {
    if ((segment.projected_start < slide_projection_far &&
         segment.projected_end < slide_projection_far) ||
        (slide_projection_near < segment.projected_start &&
         slide_projection_near < segment.projected_end)) {
        segment.visible = false;
        return;
    }

    const SlidePresentationSegment original = segment;
    const float span =
        std::fabs(original.projected_start - original.projected_end);
    if (span < slide_geometry_epsilon) {
        return;
    }
    if (slide_projection_near < original.projected_start) {
        const float fraction = slide_clamp_unit(
            (original.projected_start - slide_projection_near) / span);
        const float retained = 1.0F - fraction;
        segment.projected_start = slide_projection_near;
        segment.lateral_start = original.lateral_start * retained +
                                fraction * original.lateral_end;
        segment.width_start = original.width_start * retained +
                              fraction * original.width_end;
        segment.coordinate_start = original.coordinate_start * retained +
                                   fraction * original.coordinate_end;
    }
    if (slide_projection_near < original.projected_end) {
        const float fraction = slide_clamp_unit(
            (original.projected_end - slide_projection_near) / span);
        const float retained = 1.0F - fraction;
        segment.projected_end = slide_projection_near;
        segment.lateral_end = original.lateral_end * retained +
                              fraction * original.lateral_start;
        segment.width_end = original.width_end * retained +
                            fraction * original.width_start;
        segment.coordinate_end = original.coordinate_end * retained +
                                 fraction * original.coordinate_start;
    }
    if (original.projected_start < slide_projection_far) {
        const float fraction = slide_clamp_unit(
            (slide_projection_far - original.projected_start) / span);
        const float retained = 1.0F - fraction;
        segment.projected_start = slide_projection_far;
        segment.lateral_start = original.lateral_start * retained +
                                fraction * original.lateral_end;
        segment.width_start = original.width_start * retained +
                              fraction * original.width_end;
        segment.coordinate_start = original.coordinate_start * retained +
                                   fraction * original.coordinate_end;
    }
    if (original.projected_end < slide_projection_far) {
        const float fraction = slide_clamp_unit(
            (slide_projection_far - original.projected_end) / span);
        const float retained = 1.0F - fraction;
        segment.projected_end = slide_projection_far;
        segment.lateral_end = original.lateral_end * retained +
                              fraction * original.lateral_start;
        segment.width_end = original.width_end * retained +
                            fraction * original.width_start;
        segment.coordinate_end = original.coordinate_end * retained +
                                 fraction * original.coordinate_start;
    }
}

inline SlidePresentationGeometry build_slide_presentation_geometry(
    std::span<const SlidePresentationPoint> points,
    std::span<const float> raw_positions,
    std::span<const float> projected_positions,
    SlidePresentationMode mode,
    float projected_judgement_plane) {
    SlidePresentationGeometry result{};
    if (points.size() < 2 || raw_positions.size() != points.size() ||
        projected_positions.size() != points.size()) {
        return result;
    }
    result.cardinality_valid = true;
    result.segments.reserve(points.size());
    for (std::size_t index = 0; index + 1U < points.size(); ++index) {
        const auto& start = points[index];
        const auto& end = points[index + 1U];
        result.segments.push_back({
            .visible = true,
            .crosses_judgement =
                raw_positions[index] < slide_geometry_epsilon &&
                -slide_geometry_epsilon < raw_positions[index + 1U],
            .raw_start = raw_positions[index],
            .raw_end = raw_positions[index + 1U],
            .projected_start = projected_positions[index],
            .projected_end = projected_positions[index + 1U],
            .lateral_start = slide_render_lateral(start.lane_center),
            .lateral_end = slide_render_lateral(end.lane_center),
            .width_start = start.decoded_width,
            .width_end = end.decoded_width,
            .start_marker = index == 0U || start.marker,
            .end_marker = index + 2U == points.size() || end.marker,
        });
    }

    std::size_t group_start = 0;
    for (std::size_t index = 0; index < result.segments.size(); ++index) {
        if (!result.segments[index].end_marker) {
            continue;
        }
        const float denominator = std::max(
            std::fabs(result.segments[index].raw_end -
                      result.segments[group_start].raw_start),
            0.00001F);
        float distance = 0.0F;
        for (std::size_t member = group_start; member <= index; ++member) {
            auto& segment = result.segments[member];
            segment.coordinate_start =
                slide_clamp_unit(distance / denominator);
            distance += std::fabs(segment.raw_start - segment.raw_end);
            segment.coordinate_end =
                slide_clamp_unit(distance / denominator);
        }
        group_start = index + 1U;
    }

    const auto crossing = std::find_if(
        result.segments.begin(), result.segments.end(),
        [](const SlidePresentationSegment& segment) {
            return segment.crosses_judgement;
        });
    if (crossing != result.segments.end()) {
        const std::size_t index =
            static_cast<std::size_t>(crossing - result.segments.begin());
        const SlidePresentationSegment original = *crossing;
        const float denominator = std::max(
            std::fabs(original.raw_end - original.raw_start),
            slide_geometry_epsilon);
        const float fraction =
            slide_clamp_unit(std::fabs(original.raw_start) / denominator);
        SlidePresentationSegment past = original;
        SlidePresentationSegment future = original;
        past.raw_start = 0.0F;
        past.raw_end = 0.0F;
        interpolate_slide_segment_end(
            past, fraction, projected_judgement_plane);
        past.end_marker = false;
        interpolate_slide_segment_start(
            future, fraction, projected_judgement_plane);
        future.start_marker = false;
        result.segments[index] = past;
        result.segments.insert(result.segments.begin() + index + 1U, future);
    }

    for (auto& segment : result.segments) {
        if (mode == SlidePresentationMode::hide_past_with_overlay &&
            segment.raw_end < slide_geometry_epsilon) {
            segment.visible = false;
        }
        if (segment.visible) {
            clip_slide_presentation_segment(segment);
        }
    }
    return result;
}

constexpr SlideGeometryVertex slide_geometry_vertex(
    float lateral,
    float projected,
    std::uint32_t color,
    float coordinate_u,
    float coordinate_v) {
    return {lateral, 0.0F, projected, color, coordinate_u, coordinate_v};
}

inline std::vector<SlideGeometryVertex> build_slide_main_stream_vertices(
    const SlidePresentationSegment& segment,
    std::uint32_t color) {
    if (!segment.visible) {
        return {};
    }
    const float start_half = slide_render_half_width(segment.width_start);
    const float end_half = slide_render_half_width(segment.width_end);
    const auto sl = slide_geometry_vertex(
        segment.lateral_start - start_half, segment.projected_start, color,
        0.0F, segment.coordinate_start);
    const auto sr = slide_geometry_vertex(
        segment.lateral_start + start_half, segment.projected_start, color,
        1.0F, segment.coordinate_start);
    const auto el = slide_geometry_vertex(
        segment.lateral_end - end_half, segment.projected_end, color,
        0.0F, segment.coordinate_end);
    const auto er = slide_geometry_vertex(
        segment.lateral_end + end_half, segment.projected_end, color,
        1.0F, segment.coordinate_end);
    const bool reverse = segment.projected_end <= segment.projected_start;
    if (std::fabs(segment.width_start - segment.width_end) <
        slide_width_change_epsilon) {
        return reverse ? std::vector<SlideGeometryVertex>{sl, er, el,
                                                          sl, sr, er}
                       : std::vector<SlideGeometryVertex>{sl, el, er,
                                                          sl, er, sr};
    }

    const auto sli = slide_geometry_vertex(
        segment.lateral_start - start_half * slide_inner_width_scale,
        segment.projected_start, color, slide_inner_coordinate_left,
        segment.coordinate_start);
    const auto sri = slide_geometry_vertex(
        segment.lateral_start + start_half * slide_inner_width_scale,
        segment.projected_start, color, slide_inner_coordinate_right,
        segment.coordinate_start);
    const auto eli = slide_geometry_vertex(
        segment.lateral_end - end_half * slide_inner_width_scale,
        segment.projected_end, color, slide_inner_coordinate_left,
        segment.coordinate_end);
    const auto eri = slide_geometry_vertex(
        segment.lateral_end + end_half * slide_inner_width_scale,
        segment.projected_end, color, slide_inner_coordinate_right,
        segment.coordinate_end);
    std::vector<SlideGeometryVertex> vertices;
    vertices.reserve(18);
    if (reverse) {
        vertices.insert(vertices.end(), {eli, sli, eri, sli, sri, eri});
        if (segment.width_end <= segment.width_start) {
            vertices.insert(vertices.end(),
                            {el, sl, eli, sl, sli, eli,
                             eri, sri, sr, er, eri, sr});
        } else {
            vertices.insert(vertices.end(),
                            {el, sl, sli, eli, el, sli,
                             eri, sri, er, sri, sr, er});
        }
    } else {
        vertices.insert(vertices.end(), {sli, eli, eri, sri, sli, eri});
        if (segment.width_end <= segment.width_start) {
            vertices.insert(vertices.end(),
                            {sl, el, eli, sli, sl, eli,
                             sri, eri, sr, eri, er, sr});
        } else {
            vertices.insert(vertices.end(),
                            {sl, el, sli, el, eli, sli,
                             sri, eri, er, sr, sri, er});
        }
    }
    return vertices;
}

inline std::vector<SlideGeometryVertex> build_slide_center_stream_vertices(
    const SlidePresentationSegment& segment) {
    if (!segment.visible) {
        return {};
    }
    const auto sl = slide_geometry_vertex(
        segment.lateral_start - slide_center_stream_half_extent,
        segment.projected_start, slide_center_stream_color, 0.0F,
        segment.coordinate_start);
    const auto sr = slide_geometry_vertex(
        segment.lateral_start + slide_center_stream_half_extent,
        segment.projected_start, slide_center_stream_color, 1.0F,
        segment.coordinate_start);
    const auto el = slide_geometry_vertex(
        segment.lateral_end - slide_center_stream_half_extent,
        segment.projected_end, slide_center_stream_color, 0.0F,
        segment.coordinate_end);
    const auto er = slide_geometry_vertex(
        segment.lateral_end + slide_center_stream_half_extent,
        segment.projected_end, slide_center_stream_color, 1.0F,
        segment.coordinate_end);
    return segment.projected_end <= segment.projected_start
               ? std::vector<SlideGeometryVertex>{sl, er, el, sl, sr, er}
               : std::vector<SlideGeometryVertex>{sl, el, er, sl, er, sr};
}

inline std::vector<SlideGeometryVertex> build_slide_overlay_stream_vertices(
    const SlidePresentationSegment& segment,
    SlidePresentationMode mode) {
    if (mode != SlidePresentationMode::hide_past_with_overlay ||
        !segment.visible) {
        return {};
    }
    const float start_half = slide_render_half_width(segment.width_start);
    const float end_half = slide_render_half_width(segment.width_end);
    const auto sl = slide_geometry_vertex(
        segment.lateral_start - start_half, segment.projected_start,
        slide_overlay_stream_color, 0.0F, segment.coordinate_start);
    const auto sr = slide_geometry_vertex(
        segment.lateral_start + start_half, segment.projected_start,
        slide_overlay_stream_color, 1.0F, segment.coordinate_start);
    const auto el = slide_geometry_vertex(
        segment.lateral_end - end_half, segment.projected_end,
        slide_overlay_stream_color, 0.0F, segment.coordinate_end);
    const auto er = slide_geometry_vertex(
        segment.lateral_end + end_half, segment.projected_end,
        slide_overlay_stream_color, 1.0F, segment.coordinate_end);
    return segment.projected_end <= segment.projected_start
               ? std::vector<SlideGeometryVertex>{sl, er, el, sl, sr, er}
               : std::vector<SlideGeometryVertex>{sl, el, er, sl, er, sr};
}

inline float slide_mode_one_intensity(float counter) {
    constexpr float two_pi = 6.2831855F;
    const float phase = std::fmod(counter * 0.05F, 1.0F);
    return std::sin(phase * two_pi) * 0.25F + 1.5F;
}

constexpr std::uint32_t slide_main_stream_color(SlidePresentationMode mode) {
    return mode == SlidePresentationMode::alternate_color
               ? slide_alternate_main_stream_color
               : slide_base_main_stream_color;
}

constexpr bool slide_exposes_candidate(SlideStartPhase phase) {
    return phase != SlideStartPhase::resolved;
}

constexpr bool slide_is_terminal(SlideStartPhase start,
                                 SlidePathPhase path) {
    return start == SlideStartPhase::resolved &&
           path == SlidePathPhase::complete;
}

constexpr bool slide_gap_active(bool forced_activity,
                                bool any_center_lane_active) {
    return forced_activity || any_center_lane_active;
}

// A path-checkpoint type outside 2..4 still can be consumed, but its -1 source
// category is rejected by the shared result handler.
constexpr std::int32_t slide_checkpoint_source_category(
    std::int32_t checkpoint_type) {
    switch (checkpoint_type) {
        case 2:
            return 4;
        case 3:
            return 5;
        case 4:
            return 6;
        default:
            return -1;
    }
}

struct SlideCheckpointProgress {
    std::size_t remaining{};
    SlidePathPhase phase{SlidePathPhase::before_start};
};

struct SlideCheckpointDecision {
    bool consumed{};
    bool emit{};
    std::uint8_t ordinary_result_byte{};
    std::int32_t source_category{-1};
};

// One gameplay substep considers only the vector front. A due checkpoint is
// consumed and resets the retained-gap maximum even when its emission flag is
// clear. Forced-result selection and the active-result remap occur downstream
// of this ordinary result byte and remain separate reconstructed operations.
constexpr SlideCheckpointDecision update_slide_checkpoints(
    SlideCheckpointProgress& progress,
    HoldGapState& gap,
    bool adjusted_start_reached,
    std::int32_t current_gap_index,
    bool front_due,
    bool front_emission_enabled,
    std::int32_t front_type) {
    SlideCheckpointDecision decision{};
    if (!adjusted_start_reached) {
        progress.phase = SlidePathPhase::before_start;
        return decision;
    }

    if (progress.remaining != 0 && front_due) {
        decision.consumed = true;
        decision.emit = front_emission_enabled;
        decision.ordinary_result_byte =
            hold_gap_result_byte(classify_hold_gap(gap));
        decision.source_category =
            slide_checkpoint_source_category(front_type);
        reset_hold_gap_after_checkpoint(gap);
        --progress.remaining;
    }

    if (progress.remaining == 0) {
        progress.phase = SlidePathPhase::complete;
    } else if (current_gap_index == 0) {
        progress.phase = SlidePathPhase::best_current_gap;
    } else {
        progress.phase = SlidePathPhase::other_current_gap;
    }
    return decision;
}

// claim.judgement.forced-result-mode
//
// The second source argument is present at the binary call boundary but is not
// read by the selector in this snapshot. Random-mode callers supply the random
// value so this pure reconstruction does not invent an RNG implementation.
// The only recovered nonzero in-binary producer is the tutorial controller: a
// selected tutorial-step flag writes mode 2, and its clear form writes 0.
constexpr std::int32_t tutorial_forced_result_mode(bool step_flag) {
    return step_flag ? 2 : 0;
}

// Whole-binary owner/write and address-escape closure leaves only reset mode 0
// and tutorial mode 2 reachable in this exact snapshot. The other switch cases
// below are reconstructed executable interfaces, not reachable gameplay modes.
constexpr bool forced_result_mode_has_snapshot_producer(std::int32_t mode) {
    return mode == 0 || mode == 2;
}

// The companion selector field is reset to zero and has no other write or
// address escape in the NotesManager owner. Its selector therefore returns
// zero while disabled and one whenever the enable byte is set.
constexpr std::uint8_t select_reachable_forced_result_companion(
    bool enabled) {
    return enabled ? 1 : 0;
}

constexpr std::uint8_t select_forced_result_byte(bool enabled,
                                                 std::int32_t mode,
                                                 std::uint32_t& cycle_counter,
                                                 std::uint32_t random_value) {
    if (!enabled) {
        return 0;
    }

    switch (mode) {
        case 1:
            return 4;
        case 2:
            return 3;
        case 3:
            return 2;
        case 4:
            return 1;
        case 5: {
            const auto result =
                static_cast<std::uint8_t>(4U - (cycle_counter & 3U));
            ++cycle_counter;
            return result;
        }
        case 6:
            return static_cast<std::uint8_t>(4U - (random_value % 5U));
        default:
            return 0;
    }
}

// claim.judgement.shared-result-two-stage-routing
//
// The shared note-result handler first converts its source category into a
// dispatch category. The dispatcher then selects independent progress and
// aggregate categories. The fixed tables are executable data; the two runtime
// table sizes remain explicit inputs at their validation boundaries.
//
// claim.note.chara-tap-result-category
// The common TAP finalizer selects source category 1 when its variant flag is
// set and its suppression flag is clear. CharaTapNote fixes those flags to
// true/false; ordinary TapNote selects category 0.
constexpr std::int32_t tap_variant_source_category(bool variant_enabled,
                                                   bool variant_suppressed) {
    return variant_enabled && !variant_suppressed ? 1 : 0;
}

constexpr std::int32_t map_shared_result_category(
    std::int32_t source_category) {
    switch (source_category) {
        case 0:
            return 0;
        case 1:
            return 4;
        case 2:
        case 3:
            return 1;
        case 4:
        case 5:
        case 6:
            return 2;
        case 7:
        case 8:
        case 9:
            return 3;
        case 10:
        case 11:
            return 5;
        case 12:
        case 13:
        case 18:
            return 7;
        case 14:
        case 15:
        case 16:
            return 6;
        case 17:
            return 11;
        default:
            return -1;
    }
}

// claim.judgement.shared-result-two-stage-routing
//
// The first mapping above selects one of fourteen dispatch records. The sole
// dispatcher then uses two independent fields from that record: a five-way
// progress category and a nine-way aggregate category.
constexpr std::int32_t map_dispatch_progress_category(
    std::int32_t dispatch_category) {
    constexpr std::array<std::int32_t, 14> map{
        0, 1, 2, 3, 0, 3, 4, 3, 3, 3, 3, 0, 3, 3};
    if (dispatch_category < 0 ||
        static_cast<std::size_t>(dispatch_category) >= map.size()) {
        return -1;
    }
    return map[static_cast<std::size_t>(dispatch_category)];
}

constexpr std::int32_t map_dispatch_aggregate_category(
    std::int32_t dispatch_category) {
    constexpr std::array<std::int32_t, 14> map{
        0, 1, 2, 3, 4, 5, 6, 7, 5, 7, 7, 8, 7, 7};
    if (dispatch_category < 0 ||
        static_cast<std::size_t>(dispatch_category) >= map.size()) {
        return -1;
    }
    return map[static_cast<std::size_t>(dispatch_category)];
}

enum class SharedResultRoute : std::uint8_t {
    ignored,
    observer_only,
    dispatched_without_aggregate,
    authoritative_aggregate,
};

// Returns the route selected before any externally configured contribution is
// calculated. First-stage categories 0..13 reach the dispatcher. Their fixed
// aggregate mapping always lands in 0..8; on this closed path, only an invalid
// result byte prevents ordinary authoritative aggregation.
constexpr SharedResultRoute route_shared_result(
    std::int32_t source_category,
    std::uint32_t source_category_count,
    std::uint8_t result_byte,
    std::uint32_t result_type_count,
    bool terminal_route_active) {
    if (source_category < 0 ||
        static_cast<std::uint32_t>(source_category) >= source_category_count) {
        return SharedResultRoute::ignored;
    }

    const std::int32_t dispatch_category =
        map_shared_result_category(source_category);
    if (dispatch_category < 0 || dispatch_category >= 14) {
        return SharedResultRoute::ignored;
    }
    if (terminal_route_active) {
        return SharedResultRoute::observer_only;
    }
    const std::int32_t aggregate_category =
        map_dispatch_aggregate_category(dispatch_category);
    if (aggregate_category >= 0 && aggregate_category < 9 &&
        result_byte < result_type_count) {
        return SharedResultRoute::authoritative_aggregate;
    }
    return SharedResultRoute::dispatched_without_aggregate;
}

constexpr std::uint32_t saturating_result_count_increment(
    std::uint32_t count) {
    return count == UINT32_MAX ? UINT32_MAX : count + 1U;
}

// The optional observer installed by the gameplay controller tracks processed
// events after the dispatch record's five-way progress mapping. Every fixed
// dispatch record maps into that range. Its callback increments first and then
// clamps to the precomputed expected count. Unsigned wrap is retained here
// because that is the binary's instruction-level order.
constexpr std::uint32_t capped_result_observer_increment(
    std::uint32_t processed,
    std::uint32_t expected) {
    const std::uint32_t incremented = processed + 1U;
    return incremented < expected ? incremented : expected;
}

// claim.judgement.ordinary-terminal-summary-producers
//
// Ordinary aggregate evaluation can request terminal routing from two ordered
// sources. A matched configured rule has precedence and sets only the primary
// summary flag. If no such rule requested termination, a later runtime end
// threshold sets both flags. Either terminal result discards the computed
// contribution. The calculations feeding the two booleans remain explicit
// parameters because their runtime configuration is not embedded in the
// executable snapshot.
struct OrdinaryTerminalSummary {
    double value{};
    bool primary{};
    bool secondary{};
};

// claim.judgement.periodic-aggregate-reevaluation
//
// Valid result events build kind 1 snapshots. The unconditional post-note
// pass builds kind 2 from retained owner state and a zero event contribution.
enum class OrdinaryAggregateSnapshotKind : std::uint32_t {
    result = 1,
    periodic = 2,
};

constexpr bool ordinary_negative_adjustment_accepts(
    OrdinaryAggregateSnapshotKind kind) {
    return kind == OrdinaryAggregateSnapshotKind::result;
}

constexpr bool ordinary_configured_terminal_accepts(
    OrdinaryAggregateSnapshotKind kind) {
    return kind == OrdinaryAggregateSnapshotKind::result;
}

constexpr bool ordinary_periodic_progress_accepts(
    OrdinaryAggregateSnapshotKind kind,
    bool runtime_active) {
    return kind == OrdinaryAggregateSnapshotKind::periodic && runtime_active;
}

// A contribution unit can be marked one-shot. The evaluator tests the unit's
// source-order index in a bit vector, then attempts to mark it regardless of
// the previous value. A previously set valid bit rejects the unit. An
// out-of-range index reads false and cannot be marked, so it remains eligible
// on every call rather than being treated as consumed.
inline bool ordinary_rule_one_shot_allows(
    bool one_shot,
    std::uint32_t unit_index,
    std::span<std::uint32_t> consumed_words,
    std::uint32_t& consumed_count) {
    if (!one_shot) {
        return true;
    }

    const std::size_t word_index = unit_index >> 5U;
    if (word_index >= consumed_words.size()) {
        return true;
    }

    const std::uint32_t mask = 1U << (unit_index & 31U);
    const bool was_consumed = (consumed_words[word_index] & mask) != 0U;
    if (!was_consumed) {
        consumed_words[word_index] |= mask;
        ++consumed_count;
    }
    return !was_consumed;
}

// claim.judgement.ordinary-terminal-end-threshold
//
// A nonzero PlayOptionTrackSkipID enables the ordinary end-threshold
// comparison. ID 7 selects a current-track-derived value when that record is
// available; every other nonzero ID supplies a value mapped through the
// external PlayOptionTrackSkip and ScoreRank tables. The executable performs
// 32-bit unsigned subtraction/addition for the final threshold test.
inline constexpr std::uint32_t ordinary_terminal_special_track_skip_id = 7U;
inline constexpr std::uint32_t ordinary_terminal_score_ceiling = 1010000U;

constexpr bool ordinary_terminal_end_threshold_reached(
    std::uint32_t track_skip_id,
    std::uint32_t aggregate_metric,
    std::int32_t mapped_score_rank_value,
    bool current_track_value_available,
    std::int32_t current_track_value) {
    if (track_skip_id == 0U) {
        return false;
    }
    const std::int32_t selected_value =
        track_skip_id == ordinary_terminal_special_track_skip_id
            ? (current_track_value_available ? current_track_value : 0)
            : mapped_score_rank_value;
    const std::uint32_t threshold =
        ordinary_terminal_score_ceiling -
        static_cast<std::uint32_t>(selected_value);
    return threshold + 1U <= aggregate_metric;
}

// claim.judgement.ordinary-configured-terminal-rule
//
// Type-3 configured rule records are evaluated only for ordinary event type 1.
// Their five conditions are ordered because a matched result band can
// decrement the mode-2 gauge before the final zero-gauge condition.
struct OrdinaryConfiguredTerminalRule {
    bool value_floor_enabled{};
    bool first_bucket_limit_enabled{};
    std::uint32_t first_bucket_count_limit{};
    bool aggregate_metric_limit_enabled{};
    std::uint32_t aggregate_metric_threshold{};
    std::uint8_t result_ceiling{};
    std::uint32_t cumulative_result_weight_threshold{};
    bool zero_gauge_enabled{};
};

struct OrdinaryConfiguredTerminalContext {
    std::uint32_t event_type{};
    double computed_value{};
    std::int32_t event_scale_units{};
    bool first_result_bucket_nonzero{};
    std::uint32_t event_count{};
    std::uint32_t aggregate_metric{};
    std::uint8_t result_byte{};
    std::uint8_t result_type_count{};
    std::uint32_t cumulative_result_weight{};
    bool gauge_update_allowed{};
    bool gauge_enabled{};
    std::int32_t gauge_mode{};
    std::uint32_t gauge_current{};
};

struct OrdinaryConfiguredTerminalResult {
    bool matched{};
    bool terminal_requested{};
    std::uint32_t gauge_current{};
};

inline constexpr double ordinary_configured_value_scale = 0.01;

constexpr OrdinaryConfiguredTerminalResult
evaluate_ordinary_configured_terminal_rule(
    const OrdinaryConfiguredTerminalRule& rule,
    const OrdinaryConfiguredTerminalContext& context) {
    OrdinaryConfiguredTerminalResult result{
        false, false, context.gauge_current};
    if (context.event_type != 1U) {
        return result;
    }

    if (rule.value_floor_enabled) {
        bool at_or_below_floor{};
        if (context.event_scale_units == 0) {
            at_or_below_floor = !(0.0 < context.computed_value);
        } else {
            const double floor =
                static_cast<double>(context.event_scale_units) *
                ordinary_configured_value_scale;
            at_or_below_floor =
                !(floor < context.computed_value ||
                  floor == context.computed_value);
        }
        if (at_or_below_floor) {
            result.matched = true;
            result.terminal_requested = true;
        }
    }

    if (rule.first_bucket_limit_enabled &&
        context.first_result_bucket_nonzero &&
        context.event_count < rule.first_bucket_count_limit) {
        result.matched = true;
        result.terminal_requested = true;
    }

    if (rule.aggregate_metric_limit_enabled &&
        rule.aggregate_metric_threshold <= context.aggregate_metric) {
        result.matched = true;
        result.terminal_requested = true;
    }

    const bool result_band_matched =
        rule.result_ceiling < context.result_type_count &&
        context.result_byte < context.result_type_count &&
        context.result_byte <= rule.result_ceiling;
    if (result_band_matched) {
        result.matched = true;
        if (rule.cumulative_result_weight_threshold <=
            context.cumulative_result_weight) {
            result.terminal_requested = true;
        }
        if (context.gauge_update_allowed && context.gauge_enabled &&
            context.gauge_mode == 2 && result.gauge_current != 0U) {
            --result.gauge_current;
        }
    }

    if (rule.zero_gauge_enabled && context.gauge_enabled &&
        context.gauge_mode == 2 && result.gauge_current == 0U) {
        result.matched = true;
        result.terminal_requested = true;
    }
    return result;
}

constexpr OrdinaryTerminalSummary produce_ordinary_terminal_summary(
    double computed_value,
    bool terminal_rule_requested,
    bool end_threshold_reached) {
    if (terminal_rule_requested) {
        return {0.0, true, false};
    }
    if (end_threshold_reached) {
        return {0.0, true, true};
    }
    return {computed_value, false, false};
}

// A kind-2 evaluation starts from the retained value, adds the already
// evaluated common-rule contribution, then permits the common promotion
// vector to raise it. Configured terminal rules are kind-1-only, but the later
// end-threshold producer remains eligible. External rule fields and evaluated
// predicates are represented by the supplied values rather than guessed here;
// one-shot eligibility is reconstructed separately above.
constexpr OrdinaryTerminalSummary apply_ordinary_periodic_aggregate(
    double retained_value,
    double periodic_rule_contribution,
    bool promotion_matched,
    double promotion_value,
    bool end_threshold_reached) {
    double computed_value = retained_value + periodic_rule_contribution;
    if (promotion_matched && computed_value < promotion_value) {
        computed_value = promotion_value;
    }
    return produce_ordinary_terminal_summary(
        computed_value, false, end_threshold_reached);
}

// claim.pipeline.active-gameplay-exit-gate
//
// Setup enables result-count completion only when the five progress-category
// expected counts have a nonzero 32-bit sum. The active predicate then uses an
// unsigned inclusive comparison against the processed result total.
constexpr bool result_count_completion_enabled_for_setup(
    std::uint32_t expected_total) {
    return expected_total != 0U;
}

constexpr bool result_count_completion_reached(
    bool enabled,
    std::uint32_t expected_total,
    std::uint32_t processed_total) {
    return enabled && expected_total <= processed_total;
}

// These state codes and selector inputs deliberately retain structural names:
// their player-facing scene meanings are not established by this snapshot.
inline constexpr std::uint32_t active_gameplay_next_state_variant = 0x10U;
inline constexpr std::uint32_t active_gameplay_next_state_default = 0x11U;
inline constexpr std::uint32_t post_active_drain_state = 0x12U;
inline constexpr std::uint32_t runtime_note_teardown_state = 0x13U;
inline constexpr std::uint32_t final_scene_report_state = 0x14U;
inline constexpr std::uint32_t first_pre_active_priming_state = 0x0aU;
inline constexpr std::uint32_t last_pre_active_priming_state = 0x0eU;
inline constexpr std::uint32_t structural_active_gameplay_state = 0x0fU;

// claim.pipeline.pre-active-zero-base-priming
//
// Five registered states before structural active gameplay run the complete
// outer update with a false argument. This resets the accumulated clock base
// and supplies a zero-initialized physical-input sample while retaining note
// dispatch, materialization, and periodic aggregate work. The active state and
// its three drain states pass true. Later states do not call the outer update.
enum class OuterGameplayUpdateMode : std::uint8_t {
    none,
    zero_base_priming,
    live,
};

constexpr OuterGameplayUpdateMode outer_gameplay_update_mode_for_scene_state(
    std::uint32_t scene_state) {
    if (first_pre_active_priming_state <= scene_state &&
        scene_state <= last_pre_active_priming_state) {
        return OuterGameplayUpdateMode::zero_base_priming;
    }
    if (structural_active_gameplay_state <= scene_state &&
        scene_state <= post_active_drain_state) {
        return OuterGameplayUpdateMode::live;
    }
    return OuterGameplayUpdateMode::none;
}

struct ActiveGameplayExitContext {
    bool alternate_controller{};
    bool result_count_complete{};
    bool selected_terminal{};
    bool alternate_state_three{};
    bool alternate_ready{};
    bool scene_variant{};
};

struct ActiveGameplayExitDecision {
    bool requested{};
    std::uint32_t next_state{};
    bool transition_timestamp_requested{};
    bool final_report_requested{};
};

// The caller has already completed the outer gameplay update and ordinary
// periodic report before reaching this decision. The ordinary controller exits
// on count completion or its selected terminal predicate. The alternate
// controller exits immediately for nested state 3, or otherwise only when
// count completion and its independent ready byte are both true.
constexpr ActiveGameplayExitDecision decide_active_gameplay_exit(
    const ActiveGameplayExitContext& context) {
    const bool requested = context.alternate_controller
                               ? context.alternate_state_three ||
                                     (context.result_count_complete &&
                                      context.alternate_ready)
                               : context.result_count_complete ||
                                     context.selected_terminal;
    if (!requested) {
        return {};
    }
    if (context.scene_variant) {
        return {true, active_gameplay_next_state_variant, true, false};
    }
    return {true, active_gameplay_next_state_default, true, true};
}

// claim.pipeline.post-active-gameplay-drain
//
// Departing the active state does not stop gameplay processing. The update
// callbacks for structural states 0x10, 0x11, and 0x12 each invoke the same
// complete outer update. Entry to 0x13 is the first post-active runtime-note
// teardown boundary; later states do not call the outer update.
constexpr bool post_active_outer_update_required(std::uint32_t scene_state) {
    return scene_state > structural_active_gameplay_state &&
           outer_gameplay_update_mode_for_scene_state(scene_state) ==
               OuterGameplayUpdateMode::live;
}

struct PostActiveDrainGateContext {
    bool alternate_controller{};
    bool local_bypass{};
    bool ordinary_wait_active{};
    bool ordinary_release_signal{};
    bool alternate_state_three{};
    bool alternate_ready{};
};

// This gate runs only after state 0x12 has performed its full outer update.
// Names remain structural because the local and external presentation inputs
// have no established player-facing identities.
constexpr bool post_active_drain_ready_for_teardown(
    const PostActiveDrainGateContext& context) {
    if (context.alternate_controller) {
        return context.alternate_state_three || context.alternate_ready;
    }
    return context.local_bypass ||
           (!context.ordinary_wait_active && context.ordinary_release_signal);
}

// claim.interactions.result-terminal-short-circuit
//
// A terminal condition produced while aggregating one event is visible to the
// next result dispatch. The exact condition is calculated by external/runtime
// rule units, so the pure reconstruction accepts that outcome as an argument.
// The observer is normally notified only by valid events after the independent
// five-way progress mapping; an already-active terminal route invokes it
// directly and skips all normal result-state and aggregate work.
struct SharedResultRoutingState {
    bool terminal_route_active{};
    std::uint32_t aggregate_count{};
    std::uint32_t observer_notification_count{};
    std::uint32_t nonaggregate_dispatch_count{};
};

constexpr SharedResultRoute dispatch_shared_result(
    SharedResultRoutingState& state,
    std::int32_t source_category,
    std::uint32_t source_category_count,
    std::uint8_t result_byte,
    std::uint32_t result_type_count,
    bool terminal_after_aggregate) {
    const SharedResultRoute route =
        route_shared_result(source_category, source_category_count, result_byte,
                            result_type_count, state.terminal_route_active);
    switch (route) {
        case SharedResultRoute::authoritative_aggregate: {
            state.aggregate_count =
                saturating_result_count_increment(state.aggregate_count);
            const std::int32_t dispatch_category =
                map_shared_result_category(source_category);
            const std::int32_t progress_category =
                map_dispatch_progress_category(dispatch_category);
            if (progress_category >= 0 && progress_category < 5) {
                state.observer_notification_count =
                    saturating_result_count_increment(
                        state.observer_notification_count);
            }
            state.terminal_route_active =
                state.terminal_route_active || terminal_after_aggregate;
            break;
        }
        case SharedResultRoute::observer_only:
            state.observer_notification_count =
                saturating_result_count_increment(
                    state.observer_notification_count);
            break;
        case SharedResultRoute::dispatched_without_aggregate:
            state.nonaggregate_dispatch_count = saturating_result_count_increment(
                state.nonaggregate_dispatch_count);
            break;
        case SharedResultRoute::ignored:
            break;
    }
    return route;
}

// claim.judgement.alternate-terminal-meter
//
// Three controller bytes select this path when their bitwise OR is exactly 1.
constexpr bool alternate_terminal_meter_selected(
    std::uint8_t selector_0,
    std::uint8_t selector_1,
    std::uint8_t selector_2) {
    return static_cast<std::uint8_t>(selector_0 | selector_1 | selector_2) ==
           1U;
}

struct AlternateTerminalMeter {
    std::int32_t current{};
    std::int32_t maximum{};
    bool auxiliary_terminal_mode{};
    bool zero_latched{};
    std::int32_t auxiliary_count{};
    std::int32_t auxiliary_participant_limit{};
    bool auxiliary_locked{};
};

// In the ordinary form, a configured nonzero maximum makes zero current
// terminal. The alternate form uses a count captured by gameplay setup.
constexpr bool alternate_terminal_meter_active(
    const AlternateTerminalMeter& meter) {
    if (meter.auxiliary_terminal_mode) {
        return meter.auxiliary_count == 0;
    }
    return meter.maximum != 0 && meter.current == 0;
}

// Select the same pre-dispatch terminal source as the controller. The ordinary
// byte may remain set while one of the alternate selectors is stable; in that
// case it is not the currently selected predicate.
constexpr bool selected_shared_result_terminal_route(
    std::uint8_t selector_0,
    std::uint8_t selector_1,
    std::uint8_t selector_2,
    bool ordinary_terminal_route_active,
    const AlternateTerminalMeter& meter) {
    if (alternate_terminal_meter_selected(
            selector_0, selector_1, selector_2)) {
        return alternate_terminal_meter_active(meter);
    }
    return ordinary_terminal_route_active;
}

// claim.judgement.alternate-terminal-meter
//
// Gameplay setup derives the auxiliary count from a bounded prefix of runtime
// participant records. Mode codes 1, 2, and 4 restrict selection to the local
// identity. Other modes also admit records whose shared-mode flag is set.
// The local identity is omitted after the meter's ordinary zero latch has been
// set; a non-local record is counted only when its effective current value is
// nonzero. Names remain structural because the player-facing modes and record
// schema are not present in the snapshot.
struct AlternateMeterParticipantSeed {
    bool local_identity{};
    bool shared_mode_enabled{};
    std::int32_t effective_current{};
};

constexpr bool alternate_auxiliary_restricts_to_local(
    std::int32_t setup_mode) {
    return setup_mode == 1 || setup_mode == 2 || setup_mode == 4;
}

template <std::size_t N>
constexpr std::int32_t count_alternate_meter_participants(
    const std::array<AlternateMeterParticipantSeed, N>& records,
    std::size_t participant_limit,
    std::int32_t setup_mode,
    bool local_zero_latched) {
    const std::size_t bounded_limit =
        participant_limit < N ? participant_limit : N;
    const bool local_only =
        alternate_auxiliary_restricts_to_local(setup_mode);
    std::int32_t count = 0;
    for (std::size_t index = 0; index < bounded_limit; ++index) {
        const AlternateMeterParticipantSeed& record = records[index];
        if (!record.local_identity &&
            (local_only || !record.shared_mode_enabled)) {
            continue;
        }
        if (record.local_identity) {
            if (!local_zero_latched) {
                ++count;
            }
        } else if (record.effective_current != 0) {
            ++count;
        }
    }
    return count;
}

// The owner writes the count and participant limit only while byte +0x40 is
// clear. The initial gameplay setup requests the lock; later periodic attempts
// therefore leave the captured values unchanged until construction or
// configuration reset clears the lock and both integers.
constexpr bool initialize_alternate_meter_auxiliary(
    AlternateTerminalMeter& meter,
    std::int32_t participant_count,
    std::int32_t participant_limit,
    bool lock_after_write) {
    if (meter.auxiliary_locked) {
        return false;
    }
    if (lock_after_write) {
        meter.auxiliary_locked = true;
    }
    meter.auxiliary_count = participant_count;
    meter.auxiliary_participant_limit = participant_limit;
    return true;
}

// During the one internally supported selector-2 setup, an initially writable
// zero-participant capture takes this fallback only when the selected terminal
// predicate was still false. It forces a nonzero ordinary-form meter current
// to zero, latches that transition, and also sets the ordinary result byte.
// Selector ownership remains unchanged, so the byte is not selected while the
// alternate selector remains active.
constexpr bool apply_selector_2_empty_participant_setup_fallback(
    std::uint8_t selector_0,
    std::uint8_t selector_1,
    std::uint8_t selector_2,
    bool capture_was_initialized,
    std::int32_t participant_count,
    bool& ordinary_terminal_route_active,
    AlternateTerminalMeter& meter) {
    if (selector_2 == 0 || !capture_was_initialized ||
        participant_count != 0 ||
        selected_shared_result_terminal_route(
            selector_0, selector_1, selector_2,
            ordinary_terminal_route_active, meter)) {
        return false;
    }

    if (meter.current != 0) {
        meter.current = 0;
        meter.zero_latched = true;
    }
    ordinary_terminal_route_active = true;
    return true;
}

// Anonymous result codes select four externally configured signed deltas.
// Codes 3 and 4 share a delta; all other codes select zero.
constexpr std::int32_t select_alternate_meter_delta(
    std::uint8_t result_code,
    const std::array<std::int32_t, 4>& configured_deltas) {
    switch (result_code) {
        case 0:
            return configured_deltas[0];
        case 1:
            return configured_deltas[1];
        case 2:
            return configured_deltas[2];
        case 3:
        case 4:
            return configured_deltas[3];
        default:
            return 0;
    }
}

// Positive deltas drain current and negative deltas restore it. This models
// the meter and terminal-latch fields only; the binary also updates a separate
// presentation/secondary-meter value using external multipliers.
constexpr void apply_alternate_meter_delta(
    AlternateTerminalMeter& meter,
    std::int32_t delta) {
    if (delta == 0 || (delta < 0 && meter.current == meter.maximum) ||
        (meter.auxiliary_terminal_mode && meter.zero_latched)) {
        return;
    }

    const std::int64_t updated =
        static_cast<std::int64_t>(meter.current) - delta;
    if (updated < 1) {
        meter.current = 0;
        meter.zero_latched = true;
    } else if (updated > meter.maximum) {
        meter.current = meter.maximum;
    } else {
        meter.current = static_cast<std::int32_t>(updated);
    }
}

// claim.presentation.common-scene-camera
//
// projView creates two scenes in ascending render priority: BgScene at 9900
// and MainScene at 10000. Each owns an air::BasicCamera. The executable's
// projection, view, and viewport conversion are asset-independent and are
// reconstructed below. The active camera pose may instead be populated by an
// external UVC/SVO graph; callers must therefore supply that pose explicitly
// when it is available and must not treat a visually fitted pose as recovered.
struct PresentationVec3 {
    float x{};
    float y{};
    float z{};

    constexpr bool operator==(const PresentationVec3&) const = default;
};

struct PresentationMat4 {
    std::array<float, 16> value{};

    constexpr float& at(std::size_t row, std::size_t column) {
        return value[row * 4U + column];
    }

    constexpr float at(std::size_t row, std::size_t column) const {
        return value[row * 4U + column];
    }
};

// claim.presentation.air-sprite-dynamic-primitive-closure
//
// air::Sprite emits one six-vertex triangle list. The render-target/resource
// pixels remain external, but the anchor, local transform, UV order, optional
// UV transform, color replication, and primitive setup tuple are owned by the
// executable and reconstructed here.
struct AirSpriteVertex {
    PresentationVec3 position{};
    std::uint32_t color{};
    float u{};
    float v{};

    constexpr bool operator==(const AirSpriteVertex&) const = default;
};

struct AirSpriteQuadParameters {
    float width{16.0F};
    float height{16.0F};
    float x{};
    float y{};
    float scale_x{1.0F};
    float scale_y{1.0F};
    float rotation_radians{};
    float u0{};
    float v0{};
    float u1{1.0F};
    float v1{1.0F};
    std::uint32_t color{0xffffffffU};
    std::uint32_t anchor_mode{};
    std::optional<PresentationMat4> uv_transform{};
};

// Preserve the four observed setup arguments without assigning unavailable
// engine-enum names to them.
inline constexpr std::array<std::uint32_t, 4>
    air_sprite_dynamic_primitive_setup{{4, 3, 6, 1}};

// claim.presentation.model-resource-pool-boundary
//
// Power-on population always creates sixteen Sprite-backed Joint texture
// wrappers. Slot i selects TextureTable row i while it exists; once i reaches
// the external table count, the executable repeats the last row. An empty
// table therefore selects -1 for every wrapper and leaves every handle zero.
inline constexpr std::size_t presentation_joint_texture_wrapper_count = 16;

constexpr std::array<std::int32_t,
                     presentation_joint_texture_wrapper_count>
presentation_joint_texture_source_rows(std::int32_t table_record_count) {
    std::array<std::int32_t,
               presentation_joint_texture_wrapper_count> rows{};
    for (std::size_t index = 0; index < rows.size(); ++index) {
        rows[index] = table_record_count <= 0
            ? -1
            : std::min(static_cast<std::int32_t>(index),
                       table_record_count - 1);
    }
    return rows;
}

constexpr bool presentation_joint_texture_index_admitted(
    std::int32_t requested_index,
    std::size_t wrapper_count = presentation_joint_texture_wrapper_count) {
    return static_cast<std::uint32_t>(requested_index) < wrapper_count;
}

// claim.presentation.dynamic-primitive-finalizer-teardown-closure
//
// Preserve the common finalizer's offset-visible state transition without
// inventing semantic names for fields whose engine enum/meaning is not
// established. The executable performs these writes before submitting the
// payload to the optional collector/default queue, then clears pending_150
// after that call returns.
struct DynamicPrimitiveFinalizeState {
    std::uint32_t layout_selector_08{};
    std::uint32_t topology_mode_0c{};
    std::uint32_t vertex_count_10{};
    std::uint32_t vertex_stride_14{};
    std::uint32_t vertex_byte_count_18{};
    bool flag_1c{};
    std::uint32_t submission_flags_80{};
    std::uint32_t submitted_90{};
    std::uint32_t submitted_94{};
    std::uint32_t payload_flags_a4{};
    std::uintptr_t pending_150{};
};

// claim.presentation.dynamic-primitive-entry-setup-reset-closure
//
// The stride and acquired write pointer come from the external renderer
// backend. This helper reconstructs the executable-owned field selection and
// 32-bit count arithmetic once those two backend results are supplied.
constexpr void set_dynamic_primitive_entry_configuration(
    DynamicPrimitiveFinalizeState& state,
    std::uint32_t layout_selector,
    std::uint32_t topology_mode,
    std::uint32_t vertex_count,
    bool submission_flag,
    std::uint32_t resolved_vertex_stride,
    std::uintptr_t acquired_write_pointer) {
    state.layout_selector_08 = layout_selector;
    state.topology_mode_0c = topology_mode;
    state.vertex_count_10 = vertex_count;
    state.flag_1c = submission_flag;
    state.vertex_stride_14 = resolved_vertex_stride;
    state.submitted_94 = topology_mode;
    state.vertex_byte_count_18 = vertex_count * resolved_vertex_stride;
    state.submitted_90 = layout_selector;
    state.pending_150 = acquired_write_pointer;
}

constexpr void finalize_dynamic_primitive_state(
    DynamicPrimitiveFinalizeState& state) {
    state.payload_flags_a4 |= 1U;
    state.submission_flags_80 =
        (state.submission_flags_80 & ~0x80U) |
        (state.flag_1c ? 0x80U : 0U);
    state.submitted_94 = state.topology_mode_0c;
    state.submitted_90 = state.layout_selector_08;
    state.pending_150 = 0;
}

// The common two-vertex line helper uses this exact setup tuple before
// reaching the same virtual finalizer.
inline constexpr std::array<std::uint32_t, 4>
    dynamic_primitive_line_setup{{3, 1, 2, 1}};

// claim.presentation.primitive-topology-derived-flags
//
// SetPrimitiveTopologyMode stores the low six mode bits and derives three
// flags from an executable-owned 62-row property table. The generic helper
// takes the already selected/clamped row properties so its threshold and bit
// behavior can be represented without copying unrelated engine rows.
struct PrimitiveTopologyModeState {
    std::uint32_t packed_mode_word{};
    std::uint32_t auxiliary_flags_58{};
    std::uint32_t payload_flags_60{};
};

struct PrimitiveTopologyModeProperties {
    bool property_08{};
    bool property_18{};
};

constexpr void set_primitive_topology_mode(
    PrimitiveTopologyModeState& state,
    std::int32_t requested_mode,
    PrimitiveTopologyModeProperties properties) {
    const auto stored_mode =
        static_cast<std::uint32_t>(requested_mode) & 0x3fU;
    state.packed_mode_word =
        (state.packed_mode_word & ~0x3fU) | stored_mode;

    if (requested_mode < 33) {
        state.payload_flags_60 &= ~0x800U;
    } else {
        state.payload_flags_60 |= 0x800U;
    }

    state.payload_flags_60 =
        (state.payload_flags_60 & ~0x20U) |
        (properties.property_08 ? 0x20U : 0U);
    state.payload_flags_60 =
        (state.payload_flags_60 & ~0x40U) |
        (properties.property_18 ? 0x40U : 0U);
    state.auxiliary_flags_58 =
        (state.auxiliary_flags_58 & ~0x08U) |
        (properties.property_08 ? 0U : 0x08U);

    if (requested_mode > 32) {
        state.payload_flags_60 |= 0x20U;
    }
}

constexpr std::optional<PrimitiveTopologyModeProperties>
chart_primitive_topology_mode_properties(std::uint32_t mode) {
    switch (mode) {
        case 2:
            return PrimitiveTopologyModeProperties{true, true};
        case 3:
        case 4:
            return PrimitiveTopologyModeProperties{true, false};
        default:
            return std::nullopt;
    }
}

inline std::array<AirSpriteVertex, 6> build_air_sprite_quad(
    const AirSpriteQuadParameters& parameters) {
    float x0{};
    float x1{};
    switch (parameters.anchor_mode) {
        case 0:
        case 3:
        case 6:
            x1 = parameters.width;
            break;
        case 1:
        case 4:
        case 7:
            x0 = parameters.width * -0.5F;
            x1 = parameters.width * 0.5F;
            break;
        default:
            x0 = -parameters.width;
            break;
    }

    float y0{};
    float y1{};
    if (parameters.anchor_mode <= 2) {
        y1 = parameters.height;
    } else if (parameters.anchor_mode <= 5) {
        y0 = parameters.height * -0.5F;
        y1 = parameters.height * 0.5F;
    } else {
        y0 = -parameters.height;
    }

    const float sine = std::sin(parameters.rotation_radians);
    const float cosine = std::cos(parameters.rotation_radians);
    const auto transform_position = [&](float local_x, float local_y) {
        local_x *= parameters.scale_x;
        local_y *= parameters.scale_y;
        return PresentationVec3{
            local_x * cosine - local_y * sine + parameters.x,
            local_y * cosine + local_x * sine + parameters.y,
            0.0F,
        };
    };
    const auto transform_uv = [&](float u, float v) {
        if (!parameters.uv_transform.has_value()) {
            return std::array<float, 2>{u, v};
        }
        const auto& matrix = *parameters.uv_transform;
        return std::array<float, 2>{
            matrix.at(0, 0) * u + matrix.at(0, 1) * v + matrix.at(0, 3),
            matrix.at(1, 0) * u + matrix.at(1, 1) * v + matrix.at(1, 3),
        };
    };
    const auto make_vertex = [&](float local_x,
                                 float local_y,
                                 float u,
                                 float v) {
        const auto transformed_uv = transform_uv(u, v);
        return AirSpriteVertex{
            transform_position(local_x, local_y),
            parameters.color,
            transformed_uv[0],
            transformed_uv[1],
        };
    };

    return {{
        make_vertex(x0, y0, parameters.u0, parameters.v0),
        make_vertex(x0, y1, parameters.u0, parameters.v1),
        make_vertex(x1, y0, parameters.u1, parameters.v0),
        make_vertex(x0, y1, parameters.u0, parameters.v1),
        make_vertex(x1, y1, parameters.u1, parameters.v1),
        make_vertex(x1, y0, parameters.u1, parameters.v0),
    }};
}

struct PresentationSceneDescriptor {
    std::string_view name{};
    std::int32_t render_priority{};
    std::uint32_t draw_index{};
    bool immediate{};
};

inline constexpr PresentationSceneDescriptor background_scene_descriptor{
    "BgScene", 9900, 16, false};
inline constexpr PresentationSceneDescriptor main_scene_descriptor{
    "MainScene", 10000, 0, true};
inline constexpr std::array<PresentationSceneDescriptor, 2>
    presentation_scene_traversal{{
        background_scene_descriptor,
        main_scene_descriptor,
    }};

struct BasicCameraParameters {
    float near_plane{1.0F};
    float far_plane{30000.0F};
    float vertical_fov_degrees{45.0F};
    float aspect{1.0F};
    float width{1.0F};
    float height{1.0F};
    bool perspective{true};
    PresentationVec3 position{0.0F, 0.0F, 30.0F};
    PresentationVec3 target{0.0F, 0.0F, 0.0F};
    PresentationVec3 up{0.0F, 1.0F, 0.0F};
    float projection_offset_x{};
    float projection_offset_y{};
};

enum class ExternalCameraMotionMode : std::uint8_t {
    normal = 0,
    offset = 1,
    exist = 2,
};

struct ExternalCameraMotionControl {
    ExternalCameraMotionMode mode{ExternalCameraMotionMode::normal};
    float blend{1.0F};
    float link{8.0F};
    float frame{};
    bool play{true};
    bool loop{};
    bool delete_after_playback{};
};

// Gameplay presentation loads three externally indexed Forester resources
// while building the shared field presentation. Every resource path is the
// acroarts/ root plus the row's string from AcroartsTableRecord.bin. The
// executable exposes the table/row selectors and invalid-row fallback, but not
// the selected strings or UVC graph payloads in this clean-room boundary.
inline constexpr std::array<std::int32_t, 3>
    gameplay_forester_external_resource_indices{{43, 47, 46}};
inline constexpr std::string_view gameplay_forester_resource_table_filename{
    "AcroartsTableRecord.bin"};
inline constexpr std::string_view gameplay_forester_resource_root{
    "acroarts/"};
inline constexpr std::string_view gameplay_forester_invalid_row_string{};

// Field setup also selects the Forester 2DLayer from LayerTableRecord.bin.
// The first player uses row 6; the second and third use row 29. A bad row
// returns integer zero. These are layer selectors, not TargetScene names.
inline constexpr std::string_view gameplay_forester_layer_table_filename{
    "LayerTableRecord.bin"};
inline constexpr std::array<std::int32_t, 3>
    gameplay_forester_external_layer_indices{{6, 29, 29}};
inline constexpr std::int32_t gameplay_forester_invalid_layer_fallback = 0;

constexpr float resolve_camera_aspect(
    std::int32_t viewport_width,
    std::int32_t viewport_height) {
    const std::int32_t clamped_height =
        viewport_height < 1 ? 1 : viewport_height;
    return static_cast<float>(viewport_width) /
           static_cast<float>(clamped_height);
}

constexpr PresentationMat4 multiply_presentation_matrices(
    const PresentationMat4& left,
    const PresentationMat4& right) {
    PresentationMat4 result{};
    for (std::size_t row = 0; row < 4; ++row) {
        for (std::size_t column = 0; column < 4; ++column) {
            float value = 0.0F;
            for (std::size_t component = 0; component < 4; ++component) {
                value += left.at(row, component) *
                         right.at(component, column);
            }
            result.at(row, column) = value;
        }
    }
    return result;
}

inline PresentationMat4 make_presentation_perspective(
    float vertical_fov_degrees,
    float aspect,
    float near_plane,
    float far_plane) {
    if (far_plane <= near_plane + 0.0001F) {
        far_plane = near_plane + 0.0001F;
    }
    constexpr float degrees_to_radians = 0.017453292F;
    const float q = 1.0F /
                    std::tan(vertical_fov_degrees * degrees_to_radians *
                             0.5F);
    PresentationMat4 result{};
    result.at(0, 0) = q / aspect;
    result.at(1, 1) = q;
    result.at(2, 2) = far_plane / (near_plane - far_plane);
    result.at(2, 3) =
        (near_plane * far_plane) / (near_plane - far_plane);
    result.at(3, 2) = -1.0F;
    return result;
}

inline float presentation_length(PresentationVec3 value) {
    return std::sqrt(value.x * value.x + value.y * value.y +
                     value.z * value.z);
}

inline PresentationVec3 normalize_presentation_vector(
    PresentationVec3 value) {
    const float inverse = 1.0F / presentation_length(value);
    return {value.x * inverse, value.y * inverse, value.z * inverse};
}

constexpr PresentationVec3 cross_presentation_vectors(
    PresentationVec3 left,
    PresentationVec3 right) {
    return {
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x,
    };
}

constexpr float dot_presentation_vectors(
    PresentationVec3 left,
    PresentationVec3 right) {
    return left.x * right.x + left.y * right.y + left.z * right.z;
}

inline PresentationMat4 make_presentation_view(
    PresentationVec3 position,
    PresentationVec3 target,
    PresentationVec3 up) {
    constexpr float epsilon = 1.1920929e-07F;
    const PresentationVec3 difference{
        position.x - target.x,
        position.y - target.y,
        position.z - target.z,
    };
    if (presentation_length(difference) < epsilon) {
        target.z += 0.0001F;
    }
    if (std::fabs(up.x) < epsilon && std::fabs(up.y) < epsilon &&
        std::fabs(up.z) < epsilon) {
        up = {0.0F, 1.0F, 0.0F};
    }

    const PresentationVec3 z_axis = normalize_presentation_vector({
        position.x - target.x,
        position.y - target.y,
        position.z - target.z,
    });
    const PresentationVec3 x_axis =
        normalize_presentation_vector(cross_presentation_vectors(up, z_axis));
    const PresentationVec3 y_axis =
        cross_presentation_vectors(z_axis, x_axis);

    PresentationMat4 result{};
    result.at(0, 0) = x_axis.x;
    result.at(0, 1) = x_axis.y;
    result.at(0, 2) = x_axis.z;
    result.at(0, 3) = -dot_presentation_vectors(x_axis, position);
    result.at(1, 0) = y_axis.x;
    result.at(1, 1) = y_axis.y;
    result.at(1, 2) = y_axis.z;
    result.at(1, 3) = -dot_presentation_vectors(y_axis, position);
    result.at(2, 0) = z_axis.x;
    result.at(2, 1) = z_axis.y;
    result.at(2, 2) = z_axis.z;
    result.at(2, 3) = -dot_presentation_vectors(z_axis, position);
    result.at(3, 3) = 1.0F;
    return result;
}

struct PresentationViewportPoint {
    float x{};
    float y{};
    float depth{};
};

constexpr PresentationViewportPoint project_presentation_point(
    const PresentationMat4& projection_view,
    PresentationVec3 point,
    std::int32_t viewport_width,
    std::int32_t viewport_height) {
    const float clip_x = projection_view.at(0, 0) * point.x +
                         projection_view.at(0, 1) * point.y +
                         projection_view.at(0, 2) * point.z +
                         projection_view.at(0, 3);
    const float clip_y = projection_view.at(1, 0) * point.x +
                         projection_view.at(1, 1) * point.y +
                         projection_view.at(1, 2) * point.z +
                         projection_view.at(1, 3);
    const float clip_z = projection_view.at(2, 0) * point.x +
                         projection_view.at(2, 1) * point.y +
                         projection_view.at(2, 2) * point.z +
                         projection_view.at(2, 3);
    const float clip_w = projection_view.at(3, 0) * point.x +
                         projection_view.at(3, 1) * point.y +
                         projection_view.at(3, 2) * point.z +
                         projection_view.at(3, 3);
    const float inverse_w = 1.0F / clip_w;
    return {
        static_cast<float>(viewport_width) * 0.5F *
            (clip_x * inverse_w + 1.0F),
        static_cast<float>(viewport_height) * 0.5F *
            (1.0F - clip_y * inverse_w),
        clip_z * inverse_w,
    };
}

inline PresentationMat4 build_presentation_projection_view(
    const BasicCameraParameters& camera) {
    return multiply_presentation_matrices(
        make_presentation_perspective(
            camera.vertical_fov_degrees,
            camera.aspect,
            camera.near_plane,
            camera.far_plane),
        make_presentation_view(camera.position, camera.target, camera.up));
}

// claim.presentation.common-scene-pass-ordering
//
// Scene pass descriptions are resource-graph inputs, but the executable owns
// their validation domain, stable PassIndex ordering, first-match routing,
// range predicates, and the eight submission sort modes.  The clean-room
// reconstruction therefore accepts the pass table as data instead of
// inventing a canonical gameplay table.
enum class PresentationPassType : std::uint8_t {
    opaque = 0,
    punch = 1,
    trans = 2,
    two_d = 3,
    reduce = 4,
    three_d_all = 5,
    all = 6,
    opaque_punch = 7,
};

enum class PresentationPassSortMode : std::uint8_t {
    shader_upper = 0,
    shader_lower = 1,
    depth_upper = 2,
    depth_lower = 3,
    layer_all_upper = 4,
    layer_all_lower = 5,
    layer_lower_depth_upper = 6,
    layer_lower_depth_lower = 7,
};

enum class PresentationPassRange : std::uint8_t {
    all = 0,
    depth_front = 1,
    depth_back = 2,
    layer_front = 3,
    layer_back = 4,
};

struct PresentationPassConfiguration {
    std::uint8_t pass_index{};
    bool entry{true};
    PresentationPassType type{PresentationPassType::opaque};
    std::uint8_t user{};
    PresentationPassSortMode sort{PresentationPassSortMode::shader_upper};
    PresentationPassRange range{PresentationPassRange::all};
    float range_value_f32{};
    std::uint32_t range_value_u32{};
};

struct PresentationPassPlanEntry {
    PresentationPassConfiguration configuration{};
    std::size_t source_index{};
};

struct PresentationPassPlan {
    bool valid_external_configuration{true};
    std::vector<PresentationPassPlanEntry> entries;
};

constexpr bool valid_presentation_pass_configuration(
    const PresentationPassConfiguration& pass) {
    return pass.pass_index < 32U &&
           static_cast<std::uint8_t>(pass.type) < 8U && pass.user < 16U &&
           static_cast<std::uint8_t>(pass.sort) < 8U &&
           static_cast<std::uint8_t>(pass.range) < 5U;
}

inline PresentationPassPlan build_presentation_pass_plan(
    std::span<const PresentationPassConfiguration> configurations) {
    PresentationPassPlan plan;
    plan.entries.reserve(configurations.size());
    for (std::size_t index = 0; index < configurations.size(); ++index) {
        const auto& configuration = configurations[index];
        if (!valid_presentation_pass_configuration(configuration)) {
            return {false, {}};
        }
        if (configuration.entry) {
            plan.entries.push_back({configuration, index});
        }
    }
    std::stable_sort(
        plan.entries.begin(),
        plan.entries.end(),
        [](const auto& left, const auto& right) {
            return left.configuration.pass_index <
                   right.configuration.pass_index;
        });
    return plan;
}

struct PresentationSubmission {
    PresentationPassType type{PresentationPassType::opaque};
    std::uint8_t user{};
    std::uint16_t layer{};
    float depth{};
    std::uint32_t material_sort_low{};
    std::uint32_t material_sort_high{};

    // Clean-room identity carried through ordering tests/callers. The source
    // sorter does not inspect this field.
    std::uint32_t source_sequence{};
};

constexpr bool presentation_pass_type_matches(
    PresentationPassType pass_type,
    PresentationPassType submission_type) {
    const auto pass = static_cast<std::uint8_t>(pass_type);
    const auto submission = static_cast<std::uint8_t>(submission_type);
    return pass == submission || pass == 6U ||
           (pass == 7U && submission <= 1U) ||
           (pass == 5U && submission <= 2U);
}

constexpr bool presentation_pass_range_matches(
    const PresentationPassConfiguration& pass,
    float depth_probe,
    std::uint16_t layer) {
    switch (pass.range) {
        case PresentationPassRange::all:
            return true;
        case PresentationPassRange::depth_front:
            return depth_probe < pass.range_value_f32;
        case PresentationPassRange::depth_back:
            return pass.range_value_f32 <= depth_probe;
        case PresentationPassRange::layer_front:
            return pass.range_value_u32 <= layer;
        case PresentationPassRange::layer_back:
            return layer < pass.range_value_u32;
    }
    return false;
}

constexpr bool presentation_pass_accepts(
    const PresentationPassConfiguration& pass,
    const PresentationSubmission& submission,
    float depth_probe) {
    return pass.entry &&
           presentation_pass_type_matches(pass.type, submission.type) &&
           pass.user == submission.user &&
           presentation_pass_range_matches(
               pass, depth_probe, submission.layer);
}

struct PresentationSubmissionRoute {
    bool valid_external_configuration{true};
    std::optional<std::size_t> plan_index;
    PresentationSubmission submission{};
};

inline PresentationSubmissionRoute route_presentation_submission(
    const PresentationPassPlan& plan,
    PresentationSubmission submission,
    float depth_probe) {
    if (!plan.valid_external_configuration) {
        return {false, std::nullopt, submission};
    }
    for (std::size_t index = 0; index < plan.entries.size(); ++index) {
        const auto& pass = plan.entries[index].configuration;
        if (presentation_pass_accepts(pass, submission, depth_probe)) {
            // Every valid binary Sort descriptor is below eight, so the
            // routing helper stores the probe in the shared +0x30 key slot.
            submission.depth = depth_probe;
            return {true, index, submission};
        }
    }
    return {true, std::nullopt, submission};
}

// Strict relation used by the source insertion-sort leaves. Floating unordered
// comparisons are false, matching COMISS with the source's branch choices.
constexpr bool presentation_submission_precedes(
    const PresentationSubmission& left,
    const PresentationSubmission& right,
    PresentationPassSortMode mode) {
    switch (mode) {
        case PresentationPassSortMode::shader_upper:
        case PresentationPassSortMode::shader_lower:
            return left.material_sort_high != right.material_sort_high
                       ? left.material_sort_high > right.material_sort_high
                       : left.material_sort_low > right.material_sort_low;
        case PresentationPassSortMode::depth_upper:
            return left.depth > right.depth;
        case PresentationPassSortMode::depth_lower:
            return left.depth < right.depth;
        case PresentationPassSortMode::layer_all_upper:
            return left.layer > right.layer;
        case PresentationPassSortMode::layer_all_lower:
            return left.layer < right.layer;
        case PresentationPassSortMode::layer_lower_depth_upper:
            return left.layer != right.layer ? left.layer < right.layer
                                             : left.depth > right.depth;
        case PresentationPassSortMode::layer_lower_depth_lower:
            return left.layer != right.layer ? left.layer < right.layer
                                             : left.depth < right.depth;
    }
    return false;
}

// Inclusive ordered relation used by the adaptive merge. Unlike negating the
// strict relation, each floating comparison deliberately returns false when
// either operand is NaN; this preserves the executable's deterministic
// unordered branch behavior above the 32-record insertion threshold.
constexpr bool presentation_submission_ordered_not_after(
    const PresentationSubmission& left,
    const PresentationSubmission& right,
    PresentationPassSortMode mode) {
    switch (mode) {
        case PresentationPassSortMode::shader_upper:
        case PresentationPassSortMode::shader_lower:
            return left.material_sort_high != right.material_sort_high
                       ? left.material_sort_high > right.material_sort_high
                       : left.material_sort_low >= right.material_sort_low;
        case PresentationPassSortMode::depth_upper:
            return left.depth >= right.depth;
        case PresentationPassSortMode::depth_lower:
            return left.depth <= right.depth;
        case PresentationPassSortMode::layer_all_upper:
            return left.layer >= right.layer;
        case PresentationPassSortMode::layer_all_lower:
            return left.layer <= right.layer;
        case PresentationPassSortMode::layer_lower_depth_upper:
            return left.layer != right.layer ? left.layer < right.layer
                                             : left.depth >= right.depth;
        case PresentationPassSortMode::layer_lower_depth_lower:
            return left.layer != right.layer ? left.layer < right.layer
                                             : left.depth <= right.depth;
    }
    return false;
}

inline void sort_presentation_submissions_range(
    std::vector<PresentationSubmission>& submissions,
    std::size_t first,
    std::size_t last,
    PresentationPassSortMode mode) {
    const std::size_t count = last - first;
    if (count < 33U) {
        for (std::size_t current = first + (count != 0U); current < last;
             ++current) {
            PresentationSubmission value = submissions[current];
            std::size_t insertion = current;
            while (insertion > first && presentation_submission_precedes(
                                              value,
                                              submissions[insertion - 1U],
                                              mode)) {
                submissions[insertion] = submissions[insertion - 1U];
                --insertion;
            }
            submissions[insertion] = value;
        }
        return;
    }

    const std::size_t left_count = (count + 1U) / 2U;
    const std::size_t middle = first + left_count;
    const std::size_t right_count = last - middle;
    sort_presentation_submissions_range(submissions, first, middle, mode);
    sort_presentation_submissions_range(submissions, middle, last, mode);

    if (left_count <= right_count) {
        const std::vector<PresentationSubmission> left(
            submissions.begin() + static_cast<std::ptrdiff_t>(first),
            submissions.begin() + static_cast<std::ptrdiff_t>(middle));
        std::size_t left_index = 0;
        std::size_t right_index = middle;
        std::size_t output = first;
        while (left_index < left.size() && right_index < last) {
            if (presentation_submission_ordered_not_after(
                    left[left_index], submissions[right_index], mode)) {
                submissions[output++] = left[left_index++];
            } else {
                submissions[output++] = submissions[right_index++];
            }
        }
        while (left_index < left.size()) {
            submissions[output++] = left[left_index++];
        }
        return;
    }

    const std::vector<PresentationSubmission> right(
        submissions.begin() + static_cast<std::ptrdiff_t>(middle),
        submissions.begin() + static_cast<std::ptrdiff_t>(last));
    std::size_t left_index = middle;
    std::size_t right_index = right.size();
    std::size_t output = last;
    while (left_index > first && right_index > 0U) {
        if (presentation_submission_ordered_not_after(
                submissions[left_index - 1U], right[right_index - 1U], mode)) {
            submissions[--output] = right[--right_index];
        } else {
            submissions[--output] = submissions[--left_index];
        }
    }
    while (right_index > 0U) {
        submissions[--output] = right[--right_index];
    }
}

inline void sort_presentation_submissions(
    std::vector<PresentationSubmission>& submissions,
    PresentationPassSortMode mode) {
    sort_presentation_submissions_range(
        submissions, 0U, submissions.size(), mode);
}

// claim.presentation.material-pass-flags
//
// The three backend wrapper kinds select their pass type from different
// object fields. Keep their neutral numeric identities here: the binary does
// not attach a stable gameplay-note family name to these generic scene
// wrappers. Material contents remain external, while these bit consumers and
// their precedence are executable-owned.
enum class PresentationSubmissionBackendKind : std::uint8_t {
    kind_0 = 0,
    kind_1 = 1,
    kind_2 = 2,
};

struct PresentationPassTypeInputs {
    PresentationSubmissionBackendKind backend{
        PresentationSubmissionBackendKind::kind_0};
    std::uint32_t material_flags{};
    bool kind_1_secondary_force_2d{};
    bool kind_2_punch{};
    bool kind_2_trans{};
    bool kind_2_force_2d{};
};

constexpr std::optional<PresentationPassType>
presentation_pass_type_from_material(const PresentationPassTypeInputs& input) {
    if (input.backend == PresentationSubmissionBackendKind::kind_2) {
        PresentationPassType result = input.kind_2_trans
                                          ? PresentationPassType::trans
                                          : input.kind_2_punch
                                                ? PresentationPassType::punch
                                                : PresentationPassType::opaque;
        if (input.kind_2_force_2d) {
            result = PresentationPassType::two_d;
        }
        return result;
    }

    if (input.backend != PresentationSubmissionBackendKind::kind_0 &&
        input.backend != PresentationSubmissionBackendKind::kind_1) {
        return std::nullopt;
    }

    PresentationPassType result =
        (input.material_flags & 0x20U) != 0U
            ? PresentationPassType::trans
            : (input.material_flags & 0x40U) != 0U
                  ? PresentationPassType::punch
                  : PresentationPassType::opaque;

    if (input.backend == PresentationSubmissionBackendKind::kind_0) {
        if ((input.material_flags & 0x80U) != 0U) {
            result = PresentationPassType::two_d;
        }
    } else if ((input.material_flags & 0x80000000U) != 0U ||
               input.kind_1_secondary_force_2d) {
        result = PresentationPassType::two_d;
    }

    if ((input.material_flags & 0x2000U) != 0U) {
        result = PresentationPassType::reduce;
    }
    return result;
}

constexpr std::uint8_t presentation_pass_user_from_material_flags(
    std::uint32_t material_flags) {
    return static_cast<std::uint8_t>((material_flags >> 25U) & 0x0fU);
}

struct PresentationOffscreenRequests {
    std::uint32_t color_pass_mask{};
    std::uint32_t depth_pass_mask{};
    bool request_color{};
    bool request_depth{};
};

constexpr bool accumulate_presentation_offscreen_requests(
    PresentationOffscreenRequests& requests,
    std::uint8_t pass_index,
    std::uint32_t material_flags) {
    if (pass_index >= 32U) {
        return false;
    }
    const std::uint32_t pass_bit = std::uint32_t{1} << pass_index;
    if ((material_flags & 0x200U) != 0U) {
        requests.color_pass_mask |= pass_bit;
        requests.request_color = true;
    }
    if ((material_flags & 0x400U) != 0U) {
        requests.depth_pass_mask |= pass_bit;
        requests.request_depth = true;
    }
    if ((material_flags & 0x800U) != 0U) {
        requests.request_color = true;
    }
    if ((material_flags & 0x1000U) != 0U) {
        requests.request_depth = true;
    }
    return true;
}

struct PresentationOffscreenSelection {
    bool color{};
    bool depth{};
};

constexpr PresentationOffscreenSelection
presentation_pass_offscreen_selection(
    const PresentationOffscreenRequests& requests,
    std::uint8_t pass_index) {
    if (pass_index >= 32U) {
        return {};
    }
    const std::uint32_t pass_bit = std::uint32_t{1} << pass_index;
    return {(requests.color_pass_mask & pass_bit) != 0U,
            (requests.depth_pass_mask & pass_bit) != 0U};
}

constexpr PresentationOffscreenSelection
presentation_submission_offscreen_selection(std::uint32_t material_flags) {
    return {(material_flags & 0x800U) != 0U,
            (material_flags & 0x1000U) != 0U};
}

// claim.presentation.shared-result-feedback
//
// The common feedback consumer adjusts the transient presentation result and
// variant before it submits any result to the authoritative owner. Results
// 3/4 share one externally selected four-entry mask group; results 2 and 1
// use two other externally selected groups. The executable assumes each
// loaded group is nonempty and that its selected entry lies in the embedded
// four-byte masks. The reconstruction reports a malformed external domain
// instead of reading beyond those masks.
struct FeedbackVariantSelectorGroup {
    std::uint32_t selected{};
    std::uint32_t loaded_count{};
};

struct FeedbackVariantSelectors {
    FeedbackVariantSelectorGroup result_3_or_4{};
    FeedbackVariantSelectorGroup result_2{};
    FeedbackVariantSelectorGroup result_1{};
};

struct FeedbackResultAdjustment {
    bool valid_external_configuration{true};
    std::uint8_t result{};
    std::uint32_t variant{};
    std::uint32_t selected_mask_index{};
};

inline constexpr std::array<std::uint8_t, 4>
    feedback_result_4_remap_to_5{{0, 0, 1, 1}};
inline constexpr std::array<std::uint8_t, 4>
    feedback_result_3_suppress_variant{{1, 0, 0, 1}};
inline constexpr std::array<std::uint8_t, 4>
    feedback_result_3_remap_to_5{{0, 0, 1, 1}};
inline constexpr std::array<std::uint8_t, 4>
    feedback_result_2_suppress_variant{{1, 0, 0, 1}};
inline constexpr std::array<std::uint8_t, 4>
    feedback_result_2_remap_to_5{{0, 0, 1, 1}};
inline constexpr std::array<std::uint8_t, 4>
    feedback_result_1_suppress_variant{{1, 0, 0, 1}};
inline constexpr std::array<std::uint8_t, 4>
    feedback_result_1_remap_to_5{{0, 0, 1, 1}};

constexpr FeedbackResultAdjustment adjust_feedback_result_variant(
    std::uint8_t result,
    std::uint32_t requested_variant,
    const FeedbackVariantSelectors& selectors) {
    FeedbackResultAdjustment adjusted{true, result, requested_variant, 0};
    if (result == 0 || result > 4) {
        return adjusted;
    }

    const FeedbackVariantSelectorGroup* group = nullptr;
    if (result >= 3) {
        group = &selectors.result_3_or_4;
    } else if (result == 2) {
        group = &selectors.result_2;
    } else {
        group = &selectors.result_1;
    }
    if (group->loaded_count == 0) {
        adjusted.valid_external_configuration = false;
        return adjusted;
    }

    adjusted.selected_mask_index =
        std::min(group->selected, group->loaded_count - 1U);
    if (adjusted.selected_mask_index >= 4U) {
        adjusted.valid_external_configuration = false;
        return adjusted;
    }
    const std::size_t index = adjusted.selected_mask_index;

    if (result == 4) {
        adjusted.variant = 0;
        if (feedback_result_4_remap_to_5[index] != 0) {
            adjusted.result = 5;
        }
        return adjusted;
    }

    const std::array<std::uint8_t, 4>* suppress = nullptr;
    const std::array<std::uint8_t, 4>* remap = nullptr;
    if (result == 3) {
        suppress = &feedback_result_3_suppress_variant;
        remap = &feedback_result_3_remap_to_5;
    } else if (result == 2) {
        suppress = &feedback_result_2_suppress_variant;
        remap = &feedback_result_2_remap_to_5;
    } else {
        suppress = &feedback_result_1_suppress_variant;
        remap = &feedback_result_1_remap_to_5;
    }
    if ((*suppress)[index] != 0) {
        adjusted.variant = 0;
    }
    if ((*remap)[index] != 0) {
        adjusted.result = 5;
    }
    return adjusted;
}

// The adjusted result and variant select an external effect resource mode.
// Variant values other than 1 and 2 take the default entry.
constexpr std::int32_t map_feedback_resource_mode(
    std::uint8_t result,
    std::uint32_t variant,
    bool result_4_special_mode) {
    switch (result) {
        case 0:
            return 7;
        case 1:
            return variant == 1 ? 5 : variant == 2 ? 6 : 4;
        case 2:
            return variant == 1 ? 2 : variant == 2 ? 3 : 1;
        case 3:
            return variant == 1 ? 8 : variant == 2 ? 9 : 0;
        case 4:
            return result_4_special_mode ? 13 : 0;
        case 5:
            return variant == 1 ? 10 : variant == 2 ? 11 : 12;
        default:
            return -1;
    }
}

constexpr bool ordinary_feedback_enabled(bool selected_terminal_active) {
    return !selected_terminal_active;
}

constexpr bool feedback_kind_2_enabled(
    bool selected_terminal_active,
    bool restrict_to_middle_end_categories,
    std::int32_t source_category) {
    return ordinary_feedback_enabled(selected_terminal_active) &&
           (!restrict_to_middle_end_categories ||
            (source_category >= 2 && source_category <= 6));
}

constexpr bool feedback_kind_1_enabled(
    bool selected_terminal_active,
    std::int32_t source_category) {
    return ordinary_feedback_enabled(selected_terminal_active) &&
           !(source_category >= 2 && source_category <= 6);
}

struct FeedbackSpanResourceSelection {
    bool has_resource_field{};
    std::uint32_t row_byte_offset{};
    std::int32_t bucket_width{};

    constexpr bool operator==(
        const FeedbackSpanResourceSelection&) const = default;
};

constexpr FeedbackSpanResourceSelection select_feedback_span_resource(
    std::int32_t span) {
    switch (span) {
        case 1:
            return {true, 0x34, 1};
        case 2:
            return {true, 0x30, 2};
        case 3:
            return {true, 0x2c, 3};
        case 4:
            return {true, 0x28, 4};
        case 5:
        case 6:
            return {true, 0x20, 6};
        case 7:
        case 8:
            return {true, 0x1c, 8};
        default:
            if (span >= 9 && span <= 16) {
                return {true, 0x0c, 16};
            }
            return {};
    }
}

constexpr float feedback_span_lateral_scale(
    std::int32_t source_category,
    std::int32_t span,
    const FeedbackSpanResourceSelection& resource) {
    if (source_category >= 7 && source_category <= 9 &&
        resource.bucket_width != 0) {
        return static_cast<float>(span) /
               static_cast<float>(resource.bucket_width);
    }
    return 1.0F;
}

enum class FeedbackScene : std::uint8_t {
    main,
    background,
};

constexpr FeedbackScene ordinary_feedback_scene(
    std::uint8_t effect_list_kind,
    std::int32_t source_category) {
    return effect_list_kind <= 1 && source_category == 15
               ? FeedbackScene::background
               : FeedbackScene::main;
}

struct LaneFeedbackRequest {
    bool updates_lanes{};
    bool zero_result_override{};
};

constexpr LaneFeedbackRequest select_lane_feedback_request(
    std::int32_t source_category,
    std::uint8_t result) {
    switch (source_category) {
        case 0:
        case 1:
        case 14:
        case 15:
            return {true, false};
        case 17:
            return result == 0 ? LaneFeedbackRequest{true, true}
                               : LaneFeedbackRequest{};
        default:
            return {};
    }
}

struct LaneFeedbackState {
    std::array<std::uint8_t, 16> result{};
    std::array<std::uint32_t, 16> serial{};
    std::uint32_t next_serial{};
};

constexpr bool apply_lane_feedback(
    LaneFeedbackState& state,
    std::int32_t start_lane,
    std::int32_t span,
    std::uint8_t result,
    bool zero_result_override) {
    if (start_lane >= 16) {
        return false;
    }
    const std::int32_t clamped_start = std::max(start_lane, 0);
    std::int32_t end = add_i32_wrapped(start_lane, span);
    if (end <= 0) {
        return false;
    }
    if (end > 15) {
        end = 16;
    }
    if (clamped_start >= end ||
        (result == 0 && !zero_result_override)) {
        return false;
    }

    ++state.next_serial;
    for (std::int32_t lane = clamped_start; lane < end; ++lane) {
        const std::uint32_t previous_serial = state.serial[lane];
        if (previous_serial != 0) {
            for (std::size_t candidate = 0; candidate < 16; ++candidate) {
                if (state.serial[candidate] == previous_serial) {
                    state.serial[candidate] = 0;
                    state.result[candidate] = 0;
                }
            }
        }
        state.serial[lane] = state.next_serial;
        state.result[lane] = result;
    }
    return true;
}

constexpr bool fixed_post_result_cue_enabled(
    std::uint32_t configured_mode,
    std::uint8_t result,
    bool selected_terminal_active_after_dispatch) {
    return !selected_terminal_active_after_dispatch &&
           configured_mode >= 1 && configured_mode <= 3 &&
           result < configured_mode;
}

// claim.presentation.effect-player-state-machine-closure
//
// Every accepted EffectManager submission owns this four-state executable
// wrapper around the external resource player.  State writes are pending: the
// next update applies the transition before running that state's update
// callback.  In particular, disappearance while live queues terminal state
// but does not make the current-state terminal predicate true until the next
// update.
enum class FeedbackEffectPlayerPhase : std::int32_t {
    idle = 0,
    starting = 1,
    live = 2,
    terminal = 3,
};

struct FeedbackEffectPlayerState {
    std::int32_t current_phase{-1};
    std::int32_t pending_phase{-1};
    std::uint32_t ticks_in_phase{};
    bool visible{true};
    bool stopped{};
};

constexpr void start_feedback_effect_player_state(
    FeedbackEffectPlayerState& state) {
    state.pending_phase =
        static_cast<std::int32_t>(FeedbackEffectPlayerPhase::starting);
}

constexpr bool stop_feedback_effect_player_state(
    FeedbackEffectPlayerState& state) {
    if (state.stopped) {
        return false;
    }
    state.pending_phase =
        static_cast<std::int32_t>(FeedbackEffectPlayerPhase::terminal);
    state.stopped = true;
    return true;
}

constexpr bool feedback_effect_player_is_terminal(
    const FeedbackEffectPlayerState& state) {
    return state.current_phase ==
           static_cast<std::int32_t>(FeedbackEffectPlayerPhase::terminal);
}

struct FeedbackEffectPlayerUpdateOutcome {
    bool applied_pending_transition{};
    bool queued_live{};
    bool queued_terminal{};
    bool terminal_after_update{};
};

constexpr FeedbackEffectPlayerUpdateOutcome update_feedback_effect_player_state(
    FeedbackEffectPlayerState& state,
    bool external_instance_present,
    bool paused = false) {
    FeedbackEffectPlayerUpdateOutcome outcome{};
    if (!paused) {
        ++state.ticks_in_phase;
        if (state.pending_phase >= 0) {
            state.current_phase = state.pending_phase;
            state.pending_phase = -1;
            state.ticks_in_phase = 0;
            outcome.applied_pending_transition = true;
        }

        if (state.current_phase ==
            static_cast<std::int32_t>(FeedbackEffectPlayerPhase::starting)) {
            state.pending_phase =
                static_cast<std::int32_t>(FeedbackEffectPlayerPhase::live);
            outcome.queued_live = true;
        } else if (
            state.current_phase ==
                static_cast<std::int32_t>(FeedbackEffectPlayerPhase::live) &&
            !external_instance_present) {
            state.pending_phase =
                static_cast<std::int32_t>(FeedbackEffectPlayerPhase::terminal);
            outcome.queued_terminal = true;
        }
    }
    outcome.terminal_after_update = feedback_effect_player_is_terminal(state);
    return outcome;
}

constexpr std::uint32_t apply_feedback_effect_visibility_flag(
    std::uint32_t instance_flags,
    bool visible) {
    return visible ? instance_flags & ~0x4U : instance_flags | 0x4U;
}

constexpr std::array<float, 16> feedback_effect_translation_matrix(
    float x,
    float y,
    float z) {
    return {
        1.0F, 0.0F, 0.0F, 0.0F,
        0.0F, 1.0F, 0.0F, 0.0F,
        0.0F, 0.0F, 1.0F, 0.0F,
        x, y, z, 1.0F,
    };
}

// Slide stores a one-shot nonzero latch beside its retained feedback handle.
// Construction and parsed-record loading set it to -1.  The first update with
// a live handle and Slide presentation phase 2 selects external resource entry
// zero and clears the latch.  Hidden phases deliberately leave it pending.
struct SlideRetainedFeedbackControlState {
    std::int32_t pending_resource_entry_latch{-1};
};

struct SlideRetainedFeedbackControlOutcome {
    bool controls_handle{};
    bool visible{};
    bool updates_position{};
    std::optional<std::int32_t> selected_resource_entry{};
};

constexpr SlideRetainedFeedbackControlOutcome
update_slide_retained_feedback_control(
    SlideRetainedFeedbackControlState& state,
    bool handle_present,
    std::int32_t slide_presentation_phase) {
    SlideRetainedFeedbackControlOutcome outcome{};
    if (!handle_present) {
        return outcome;
    }

    outcome.controls_handle = true;
    if (slide_presentation_phase != 2) {
        return outcome;
    }

    outcome.visible = true;
    outcome.updates_position = true;
    if (state.pending_resource_entry_latch != 0) {
        outcome.selected_resource_entry = 0;
        state.pending_resource_entry_latch = 0;
    }
    return outcome;
}

struct SlidePreloadFeedbackControl {
    bool controls_each_handle{};
    bool visible{};
    std::int32_t selected_resource_entry{-1};
};

constexpr SlidePreloadFeedbackControl slide_preload_feedback_control(
    std::uint32_t resource_step) {
    switch (resource_step) {
    case 11:
        return {true, true, 0};
    case 12:
        return {true, true, 1};
    case 13:
        return {true, false, 1};
    default:
        return {};
    }
}

enum class FeedbackEffectList : std::uint8_t {
    bomb,
    reaction,
    text,
    continue_effect,
    air_ring,
    sonic_boom,
    chara_note,
    chara_background,
    mine,
    mine_background,
};

struct FeedbackEffectListDescriptor {
    FeedbackEffectList list{};
    std::uint32_t capacity{};
    std::int32_t cooldown_frame_count{};
};

inline constexpr std::array<FeedbackEffectListDescriptor, 10>
    feedback_effect_lists{{
        {FeedbackEffectList::bomb, 24, 0},
        {FeedbackEffectList::reaction, 24, 0},
        {FeedbackEffectList::text, 24, 0},
        {FeedbackEffectList::continue_effect, 32, 0},
        {FeedbackEffectList::air_ring, 16, 0},
        {FeedbackEffectList::sonic_boom, 16, 0},
        {FeedbackEffectList::chara_note, 16, 0},
        {FeedbackEffectList::chara_background, 4, 5},
        {FeedbackEffectList::mine, 48, 0},
        {FeedbackEffectList::mine_background, 4, 5},
    }};

inline constexpr std::array<FeedbackEffectList, 3>
    ordinary_feedback_submit_order{{
        FeedbackEffectList::text,
        FeedbackEffectList::bomb,
        FeedbackEffectList::reaction,
    }};

inline constexpr std::array<FeedbackEffectList, 2>
    slide_extended_feedback_submit_order{{
        FeedbackEffectList::chara_note,
        FeedbackEffectList::chara_background,
    }};

constexpr bool feedback_effect_list_accepts_normal_submission(
    const FeedbackEffectListDescriptor& descriptor,
    std::uint32_t submissions_since_update,
    bool cooldown_complete) {
    return submissions_since_update < descriptor.capacity &&
           cooldown_complete;
}

// claim.presentation.effect-list-lifecycle-closure
//
// EffList capacity has two distinct uses. Admission compares the number of
// submissions since the last list update, while a non-forced append separately
// stops and removes the oldest active effect if occupancy is already at the
// same fixed capacity. Forced/preload append bypasses both admission and that
// eviction, so active occupancy may exceed capacity.
struct FeedbackEffectListState {
    std::uint32_t active_count{};
    std::uint32_t submissions_since_update{};
    std::uint32_t maximum_occupancy{};
    std::uint32_t accepted_total{};
    bool cooldown_ready{true};
};

struct FeedbackEffectListAppendOutcome {
    bool evicted_oldest{};
    bool restarted_cooldown{};
};

constexpr FeedbackEffectListAppendOutcome append_feedback_effect_list_state(
    FeedbackEffectListState& state,
    const FeedbackEffectListDescriptor& descriptor,
    bool forced) {
    FeedbackEffectListAppendOutcome outcome{};
    if (!forced) {
        if (state.active_count >= descriptor.capacity &&
            state.active_count != 0U) {
            --state.active_count;
            outcome.evicted_oldest = true;
        }
        state.cooldown_ready = false;
        outcome.restarted_cooldown = true;
    }
    ++state.active_count;
    ++state.submissions_since_update;
    ++state.accepted_total;
    return outcome;
}

constexpr void update_feedback_effect_list_state(
    FeedbackEffectListState& state,
    std::uint32_t active_after_terminal_removal) {
    state.active_count = active_after_terminal_removal;
    state.maximum_occupancy =
        std::max(state.maximum_occupancy, state.active_count);
    state.submissions_since_update = 0;
}

constexpr void clear_feedback_effect_list_state(
    FeedbackEffectListState& state) {
    state.active_count = 0;
    state.maximum_occupancy = 0;
    state.accepted_total = 0;
    state.cooldown_ready = true;
    // The executable clear slot deliberately does not write this field.
}

constexpr std::int32_t feedback_effect_cooldown_milliseconds(
    std::int32_t frame_count,
    double frame_rate) {
    if (frame_rate <= 0.0) {
        return 0;
    }
    const float milliseconds = static_cast<float>(
        (1000.0 / frame_rate) * static_cast<double>(frame_count));
    return milliseconds > 0.0F
               ? static_cast<std::int32_t>(milliseconds + 0.5F)
               : static_cast<std::int32_t>(milliseconds - 0.5F);
}

constexpr std::uint32_t allocate_feedback_sequence(
    std::uint32_t& next_sequence) {
    const std::uint32_t allocated = next_sequence;
    ++next_sequence;
    return allocated;
}

inline constexpr std::array<std::int32_t, 8>
    slide_embedded_feedback_rows{{0, 1, 2, 4, 3, 6, 5, 7}};

constexpr std::int32_t select_slide_extended_feedback_row(
    std::uint32_t trailing_selector,
    bool embedded_mode,
    const std::array<std::int32_t, 8>& runtime_rows) {
    if (trailing_selector >= 8U) {
        return -1;
    }
    return embedded_mode
               ? slide_embedded_feedback_rows[trailing_selector]
               : runtime_rows[trailing_selector];
}

// claim.presentation.slide-extended-feedback-admission
//
// CharaEffectManager owns two independent pre-submission gates for the two
// Slide extended-feedback kinds. The lane gate deliberately scans a half-open
// interval but writes the same interval with its last cell included. Preserve
// that asymmetric boundary exactly.
struct SlideCharaEffectAdmissionState {
    std::array<float, 32> lane_expiry{};
    float global_next_allowed{};
};

inline constexpr std::array<std::int32_t, 16>
    slide_chara_effect_lane_footprints{{
        2, 4, 6, 8, 2, 12, 2, 16, 2, 2, 2, 2, 2, 2, 2, 32,
    }};

inline constexpr std::array<float, 16>
    slide_chara_effect_lane_durations{{
        1.0F, 1.0F, 1.0F, 1.0F,
        1.0F, 1.0F, 1.0F, 1.0F,
        1.0F, 1.0F, 1.0F, 1.0F,
        1.0F, 1.0F, 1.0F, 1.0F,
    }};

inline constexpr float slide_chara_effect_global_duration = 7.0F;

constexpr void reset_slide_chara_effect_admission(
    SlideCharaEffectAdmissionState& state) {
    state.lane_expiry.fill(0.0F);
    state.global_next_allowed = 0.0F;
}

constexpr bool check_and_reserve_slide_chara_effect_lane_overlap(
    SlideCharaEffectAdmissionState& state,
    std::int32_t start_lane,
    std::int32_t decoded_width,
    float current_time) {
    if (start_lane < 0 || decoded_width < 1 || decoded_width > 16 ||
        start_lane > 16 - decoded_width) {
        return false;
    }

    const std::size_t width_index =
        static_cast<std::size_t>(decoded_width - 1);
    const std::int32_t footprint =
        slide_chara_effect_lane_footprints[width_index];
    const std::int32_t center_subcell = start_lane * 2 + decoded_width;
    const std::int32_t first = std::clamp(
        center_subcell - footprint / 2, 0, 31);
    const std::int32_t last = std::clamp(
        center_subcell - 1 + footprint / 2, 0, 31);

    for (std::int32_t cell = first; cell < last; ++cell) {
        if (current_time < state.lane_expiry[static_cast<std::size_t>(cell)]) {
            return false;
        }
    }

    const float expiry =
        current_time + slide_chara_effect_lane_durations[width_index];
    for (std::int32_t cell = first; cell <= last; ++cell) {
        state.lane_expiry[static_cast<std::size_t>(cell)] = expiry;
    }
    return true;
}

constexpr bool check_and_reserve_slide_chara_effect_global_cooldown(
    SlideCharaEffectAdmissionState& state,
    float current_time) {
    if (current_time < state.global_next_allowed) {
        return false;
    }
    state.global_next_allowed =
        current_time + slide_chara_effect_global_duration;
    return true;
}

}  // namespace chart::reconstruction
