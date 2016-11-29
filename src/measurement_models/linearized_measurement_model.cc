#include "refill/measurement_models/linearized_measurement_model.h"

namespace refill {

LinearizedMeasurementModel::LinearizedMeasurementModel(
    const std::size_t& state_dim, const std::size_t& measurement_dim,
    const DistributionInterface& measurement_noise)
    : MeasurementModelBase(state_dim, measurement_dim, measurement_noise) {}

}  // namespace refill
