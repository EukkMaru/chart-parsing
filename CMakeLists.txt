cmake_minimum_required(VERSION 3.25)

project(chart_gameplay_reconstruction LANGUAGES CXX)

add_library(chart_logic INTERFACE)
target_include_directories(chart_logic INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(chart_logic INTERFACE cxx_std_20)

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
    if(BUILD_TESTING)
        add_executable(chart_skeleton_test tests/skeleton_test.cpp)
        target_link_libraries(chart_skeleton_test PRIVATE chart_logic)
        add_test(NAME chart.skeleton COMMAND chart_skeleton_test)

        add_executable(chart_path_test tests/chart_path_test.cpp)
        target_link_libraries(chart_path_test PRIVATE chart_logic)
        add_test(NAME chart.path_derivation COMMAND chart_path_test)

        add_executable(chart_load_lifecycle_test
                       tests/chart_load_lifecycle_test.cpp)
        target_link_libraries(chart_load_lifecycle_test PRIVATE chart_logic)
        add_test(NAME chart.load_lifecycle COMMAND chart_load_lifecycle_test)

        add_executable(runtime_materialization_test
                       tests/runtime_materialization_test.cpp)
        target_link_libraries(runtime_materialization_test PRIVATE chart_logic)
        add_test(NAME chart.runtime_materialization
                 COMMAND runtime_materialization_test)

        add_executable(projection_schedule_test
                       tests/projection_schedule_test.cpp)
        target_link_libraries(projection_schedule_test PRIVATE chart_logic)
        add_test(NAME chart.projection_schedule
                 COMMAND projection_schedule_test)

        add_executable(hold_variant_test tests/hold_variant_test.cpp)
        target_link_libraries(hold_variant_test PRIVATE chart_logic)
        add_test(NAME chart.hold_variant COMMAND hold_variant_test)

        add_executable(runtime_note_dispatch_test
                       tests/runtime_note_dispatch_test.cpp)
        target_link_libraries(runtime_note_dispatch_test PRIVATE chart_logic)
        add_test(NAME chart.runtime_note_dispatch
                 COMMAND runtime_note_dispatch_test)

        add_executable(note_view_preload_test
                       tests/note_view_preload_test.cpp)
        target_link_libraries(note_view_preload_test PRIVATE chart_logic)
        add_test(NAME chart.note_view_preload
                 COMMAND note_view_preload_test)

        add_executable(c2s_header_test tests/c2s_header_test.cpp)
        target_link_libraries(c2s_header_test PRIVATE chart_logic)
        add_test(NAME chart.c2s_header COMMAND c2s_header_test)

        add_executable(common_lane_width_test
                       tests/common_lane_width_test.cpp)
        target_link_libraries(common_lane_width_test PRIVATE chart_logic)
        add_test(NAME chart.common_lane_width COMMAND common_lane_width_test)

        add_executable(result_identifier_test
                       tests/result_identifier_test.cpp)
        target_link_libraries(result_identifier_test PRIVATE chart_logic)
        add_test(NAME chart.result_identifier COMMAND result_identifier_test)

        add_executable(derived_command_test tests/derived_command_test.cpp)
        target_link_libraries(derived_command_test PRIVATE chart_logic)
        add_test(NAME chart.derived_command COMMAND derived_command_test)

        add_executable(sla_region_test tests/sla_region_test.cpp)
        target_link_libraries(sla_region_test PRIVATE chart_logic)
        add_test(NAME chart.sla_region COMMAND sla_region_test)

        add_executable(aso_parser_test tests/aso_parser_test.cpp)
        target_link_libraries(aso_parser_test PRIVATE chart_logic)
        add_test(NAME chart.aso_parser COMMAND aso_parser_test)

        add_executable(tap_window_test tests/tap_window_test.cpp)
        target_link_libraries(tap_window_test PRIVATE chart_logic)
        add_test(NAME chart.tap_window COMMAND tap_window_test)

        add_executable(tap_input_test tests/tap_input_test.cpp)
        target_link_libraries(tap_input_test PRIVATE chart_logic)
        add_test(NAME chart.tap_input COMMAND tap_input_test)

        add_executable(input_profile_test tests/input_profile_test.cpp)
        target_link_libraries(input_profile_test PRIVATE chart_logic)
        add_test(NAME chart.input_profile COMMAND input_profile_test)

        add_executable(tap_lifecycle_test tests/tap_lifecycle_test.cpp)
        target_link_libraries(tap_lifecycle_test PRIVATE chart_logic)
        add_test(NAME chart.tap_lifecycle COMMAND tap_lifecycle_test)

        add_executable(play_clock_test tests/play_clock_test.cpp)
        target_link_libraries(play_clock_test PRIVATE chart_logic)
        add_test(NAME chart.play_clock COMMAND play_clock_test)

        add_executable(tempo_map_test tests/tempo_map_test.cpp)
        target_link_libraries(tempo_map_test PRIVATE chart_logic)
        add_test(NAME chart.tempo_map COMMAND tempo_map_test)

        add_executable(hold_sustain_test tests/hold_sustain_test.cpp)
        target_link_libraries(hold_sustain_test PRIVATE chart_logic)
        add_test(NAME chart.hold_sustain COMMAND hold_sustain_test)

        add_executable(forced_result_test tests/forced_result_test.cpp)
        target_link_libraries(forced_result_test PRIVATE chart_logic)
        add_test(NAME chart.forced_result COMMAND forced_result_test)

        add_executable(shared_result_test tests/shared_result_test.cpp)
        target_link_libraries(shared_result_test PRIVATE chart_logic)
        add_test(NAME chart.shared_result COMMAND shared_result_test)

        add_executable(gameplay_exit_test tests/gameplay_exit_test.cpp)
        target_link_libraries(gameplay_exit_test PRIVATE chart_logic)
        add_test(NAME chart.gameplay_exit COMMAND gameplay_exit_test)

        add_executable(alternate_meter_test tests/alternate_meter_test.cpp)
        target_link_libraries(alternate_meter_test PRIVATE chart_logic)
        add_test(NAME chart.alternate_meter COMMAND alternate_meter_test)

        add_executable(flick_motion_test tests/flick_motion_test.cpp)
        target_link_libraries(flick_motion_test PRIVATE chart_logic)
        add_test(NAME chart.flick_motion COMMAND flick_motion_test)

        add_executable(mine_contact_test tests/mine_contact_test.cpp)
        target_link_libraries(mine_contact_test PRIVATE chart_logic)
        add_test(NAME chart.mine_contact COMMAND mine_contact_test)

        add_executable(air_judgement_test tests/air_judgement_test.cpp)
        target_link_libraries(air_judgement_test PRIVATE chart_logic)
        add_test(NAME chart.air_judgement COMMAND air_judgement_test)

        add_executable(air_hold_judgement_test
                       tests/air_hold_judgement_test.cpp)
        target_link_libraries(air_hold_judgement_test PRIVATE chart_logic)
        add_test(NAME chart.air_hold_judgement
                 COMMAND air_hold_judgement_test)

        add_executable(air_slide_judgement_test
                       tests/air_slide_judgement_test.cpp)
        target_link_libraries(air_slide_judgement_test PRIVATE chart_logic)
        add_test(NAME chart.air_slide_judgement
                 COMMAND air_slide_judgement_test)

        add_executable(air_ladder_judgement_test
                       tests/air_ladder_judgement_test.cpp)
        target_link_libraries(air_ladder_judgement_test PRIVATE chart_logic)
        add_test(NAME chart.air_ladder_judgement
                 COMMAND air_ladder_judgement_test)

        add_executable(heaven_hold_judgement_test
                       tests/heaven_hold_judgement_test.cpp)
        target_link_libraries(heaven_hold_judgement_test PRIVATE chart_logic)
        add_test(NAME chart.heaven_hold_judgement
                 COMMAND heaven_hold_judgement_test)

        add_executable(slide_path_test tests/slide_path_test.cpp)
        target_link_libraries(slide_path_test PRIVATE chart_logic)
        add_test(NAME chart.slide_path COMMAND slide_path_test)

        add_executable(candidate_interaction_test tests/candidate_interaction_test.cpp)
        target_link_libraries(candidate_interaction_test PRIVATE chart_logic)
        add_test(NAME chart.candidate_interaction COMMAND candidate_interaction_test)
    endif()
endif()
