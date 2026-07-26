#include <chart/reconstruction.hpp>

#include <cassert>

int main() {
    using chart::reconstruction::C2sInputOutcome;
    using chart::reconstruction::reconstruct_gameplay_chart_load_attempt;

    const auto missing = reconstruct_gameplay_chart_load_attempt(
        C2sInputOutcome::file_precheck_failed, 99);
    assert(!missing.load_succeeded);
    assert(!missing.parser_orchestration_ran);
    assert(!missing.success_only_configuration_ran);
    assert(missing.setup_continues);
    assert(missing.parsed_record_count == 0);
    assert(missing.materialization_queue_count == 0);
    assert(missing.initial_runtime_note_count == 0);

    const auto malformed = reconstruct_gameplay_chart_load_attempt(
        C2sInputOutcome::tokenization_failed, 99);
    assert(!malformed.load_succeeded);
    assert(!malformed.parser_orchestration_ran);
    assert(!malformed.success_only_configuration_ran);
    assert(malformed.setup_continues);
    assert(malformed.parsed_record_count == 0);
    assert(malformed.materialization_queue_count == 0);
    assert(malformed.initial_runtime_note_count == 0);

    // Record-level rejections are already absent from accepted_record_count;
    // they do not turn a tokenized chart into a failed load.
    const auto partially_accepted = reconstruct_gameplay_chart_load_attempt(
        C2sInputOutcome::tokenized, 3);
    assert(partially_accepted.load_succeeded);
    assert(partially_accepted.parser_orchestration_ran);
    assert(partially_accepted.success_only_configuration_ran);
    assert(partially_accepted.setup_continues);
    assert(partially_accepted.parsed_record_count == 3);
    assert(partially_accepted.materialization_queue_count == 3);
    assert(partially_accepted.initial_runtime_note_count == 0);

    const auto empty_but_tokenized = reconstruct_gameplay_chart_load_attempt(
        C2sInputOutcome::tokenized, 0);
    assert(empty_but_tokenized.load_succeeded);
    assert(empty_but_tokenized.success_only_configuration_ran);
    assert(empty_but_tokenized.setup_continues);
    assert(empty_but_tokenized.materialization_queue_count == 0);
    assert(empty_but_tokenized.initial_runtime_note_count == 0);
}
