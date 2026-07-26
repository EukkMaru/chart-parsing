#include <chart/reconstruction.hpp>

#include <array>
#include <cassert>

int main() {
    using chart::reconstruction::derive_c2s_path;
    using chart::reconstruction::select_source_chart_path;
    using chart::reconstruction::SourceChartPathSelectionStatus;

    constexpr std::array<std::string_view, 6> catalog_paths{
        "music/track_0.xml",
        "music/track_1.xml",
        "music/track_2.xml",
        "music/track_3.xml",
        "music/track_4.xml",
        "music/track_5.xml",
    };

    const auto direct = select_source_chart_path(catalog_paths, 2, 6);
    assert(direct.status == SourceChartPathSelectionStatus::selected);
    assert(direct.selected_index == 2);
    assert(direct.source_path == "music/track_2.xml");

    const auto clamped = select_source_chart_path(catalog_paths, 9, 6);
    assert(clamped.status == SourceChartPathSelectionStatus::selected);
    assert(clamped.selected_index == 5);
    assert(clamped.source_path == "music/track_5.xml");

    const auto smaller_external_table =
        select_source_chart_path(catalog_paths, 5, 3);
    assert(smaller_external_table.status ==
           SourceChartPathSelectionStatus::selected);
    assert(smaller_external_table.selected_index == 2);

    const auto too_few_catalog_entries = select_source_chart_path(
        std::span<const std::string_view>{catalog_paths}.first<5>(), 2, 6);
    assert(too_few_catalog_entries.status ==
           SourceChartPathSelectionStatus::unavailable);
    assert(too_few_catalog_entries.source_path.empty());

    const auto zero_external_count =
        select_source_chart_path(catalog_paths, 0, 0);
    assert(zero_external_count.status ==
           SourceChartPathSelectionStatus::invalid_external_configuration);

    const auto catalog_mismatch =
        select_source_chart_path(catalog_paths, 7, 8);
    assert(catalog_mismatch.status ==
           SourceChartPathSelectionStatus::invalid_external_configuration);

    assert(derive_c2s_path("music/track.xml") == "music/track.c2s");
    assert(derive_c2s_path("music/track.XML") == "music/track.XML");
    assert(derive_c2s_path("music/track") == "music/track");
    assert(derive_c2s_path("a.xml/b.xml") == "a.c2s/b.xml");
    assert(derive_c2s_path("track.xml.backup") == "track.c2s.backup");
}
