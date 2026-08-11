#include "chart/reconstruction.hpp"

#include <cassert>
#include <cmath>

namespace {

bool near(float left, float right) {
    return std::fabs(left - right) < 0.0001F;
}

}  // namespace

int main() {
    using namespace chart::reconstruction;

    assert(air_model_resource_row(1, AirDirectionCode::air) == 161);
    assert(air_model_resource_row(16, AirDirectionCode::air) == 146);
    assert(air_model_resource_row(1, AirDirectionCode::aur) == 177);
    assert(air_model_resource_row(16, AirDirectionCode::aul) == 162);
    assert(air_model_resource_row(1, AirDirectionCode::adw) == 193);
    assert(air_model_resource_row(1, AirDirectionCode::adr) == 209);
    assert(air_model_resource_row(16, AirDirectionCode::adl) == 194);
    assert(air_model_resource_row(0, AirDirectionCode::air) == 161);
    assert(air_model_resource_row(99, AirDirectionCode::air) == 146);

    assert(!air_stored_presentation_property_bit(
        0, AirDirectionCode::air));
    assert(air_stored_presentation_property_bit(
        0, AirDirectionCode::adw));
    assert(!air_stored_presentation_property_bit(
        1, AirDirectionCode::adw));
    assert(air_stored_presentation_property_bit(
        2, AirDirectionCode::air));

    assert(near(air_manager_reference_position(100.0F, false, 3.0F),
                216006.0F));
    assert(near(air_manager_reference_position(100.0F, true, 2.0F),
                216012.0F));
    assert(near(air_direction_scaled_reference(
                    100.0F, AirDirectionCode::aur, false, 2.0F, 3.0F),
                100.0F));
    assert(near(air_direction_scaled_reference(
                    100.0F, AirDirectionCode::aul, true, 2.0F, 3.0F),
                200.0F));
    assert(near(air_direction_scaled_reference(
                    100.0F, AirDirectionCode::adw, false, 2.0F, 3.0F),
                40.0F));
    assert(near(air_direction_scaled_reference(
                    100.0F, AirDirectionCode::adr, true, 2.0F, 3.0F),
                120.0F));
    assert(near(air_model_projection_delta(
                    100.0F, 25.0F, AirDirectionCode::air,
                    false, 2.0F, 3.0F),
                75.0F));

    assert(near(air_external_resource_lateral_offset(1.5F), 6.0F));
    assert(near(air_external_resource_vertical_offset(2.0F), 7.7869998F));
    assert(near(air_external_resource_depth_offset(
                    10.0F, 2.0F, 3.0F, 0.5F),
                -19.5F));
    assert(near(air_external_resource_depth_offset(
                    -10.0F, 2.0F, 3.0F, 0.5F),
                21.0F));

    assert(air_root_model_update_enabled(0));
    assert(!air_root_model_update_enabled(3));
    assert(air_terminal_cue_selector(AirDirectionCode::air) == 0x3c);
    assert(air_terminal_cue_selector(AirDirectionCode::adl) == 0x134);
    assert(air_terminal_emits_field_feedback(AirDirectionCode::aul));
    assert(!air_terminal_emits_field_feedback(AirDirectionCode::adr));
    assert(near(air_direction_resource_scale_sign(2), -1.0F));
    assert(near(air_direction_resource_scale_sign(4), -1.0F));
}
