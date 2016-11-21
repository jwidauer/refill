#include "refill/measurement_models/linear_measurement_model.h"

namespace refill {

// In case of call to standard constructor we assume a one dimensional system,
// with one measurement and univariate standard normal gaussian noise.
LinearMeasurementModel::LinearMeasurementModel()
    : LinearMeasurementModel(Eigen::MatrixXd::Identity(1, 1),
                             GaussianDistribution(),
                             Eigen::MatrixXd::Identity(1, 1)) {}

LinearMeasurementModel::LinearMeasurementModel(
    const Eigen::MatrixXd& measurement_mapping,
    const DistributionInterface& measurement_noise)
    : LinearMeasurementModel(
        measurement_mapping,
        measurement_noise,
        Eigen::MatrixXd::Identity(measurement_mapping.rows(),
                                  measurement_noise.mean().size())) {}

LinearMeasurementModel::LinearMeasurementModel(
    const Eigen::MatrixXd& measurement_mapping,
    const DistributionInterface& measurement_noise,
    const Eigen::MatrixXd& noise_mapping)
    : LinearizedMeasurementModel(measurement_mapping.cols(),
                                 measurement_mapping.rows(),
                                 measurement_noise) {
  this->setMeasurementParameters(measurement_mapping, measurement_noise,
                                 noise_mapping);
}

void LinearMeasurementModel::setMeasurementParameters(
    const Eigen::MatrixXd& measurement_mapping,
    const DistributionInterface& measurement_noise) {
  this->setMeasurementParameters(
      measurement_mapping,
      measurement_noise,
      Eigen::MatrixXd::Identity(measurement_mapping.rows(),
                                measurement_noise.mean().size()));
}

void LinearMeasurementModel::setMeasurementParameters(
    const Eigen::MatrixXd& measurement_mapping,
    const DistributionInterface& measurement_noise,
    const Eigen::MatrixXd& noise_mapping) {
  CHECK_EQ(measurement_mapping.rows(), noise_mapping.rows());
  CHECK_EQ(noise_mapping.cols(), measurement_noise.mean().size());

  measurement_mapping_ = measurement_mapping;
  noise_mapping_ = noise_mapping;

  this->setLinearizedMeasurementModelParameters(measurement_mapping.cols(),
                                                measurement_mapping.rows(),
                                                measurement_noise);
}

Eigen::VectorXd LinearMeasurementModel::observe(
    const Eigen::VectorXd& state) const {
  CHECK_EQ(state.size(), this->getStateDim());

  return measurement_mapping_ * state
      + noise_mapping_ * measurement_noise_->mean();
}

Eigen::MatrixXd LinearMeasurementModel::getMeasurementJacobian(
    const Eigen::VectorXd& state) const {
  return measurement_mapping_;
}

Eigen::MatrixXd LinearMeasurementModel::getNoiseJacobian(
    const Eigen::VectorXd& state) const {
  return noise_mapping_;
}

}  // namespace refill
