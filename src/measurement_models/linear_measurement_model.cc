#include "refill/measurement_models/linear_measurement_model.h"

namespace refill {

LinearMeasurementModel::LinearMeasurementModel()
    : LinearMeasurementModel(Eigen::MatrixXd::Identity(0, 0),
                             GaussianDistribution(),
                             Eigen::MatrixXd::Identity(0, 0)) {}

/**
 * This constructor sets the noise mapping to an identity matrix.
 *
 * @param measurement_mapping The matrix @f$ H_k @f$.
 * @param measurement_noise The measurement noise @f$ w_k @f$.
 */
LinearMeasurementModel::LinearMeasurementModel(
    const Eigen::MatrixXd& measurement_mapping,
    const DistributionInterface& measurement_noise)
    : LinearMeasurementModel(
        measurement_mapping,
        measurement_noise,
        Eigen::MatrixXd::Identity(measurement_mapping.rows(),
                                  measurement_noise.mean().size())) {}

/**
 * This constructor sets all linear measurement model parameters.
 *
 * It also checks that the dimensions of @e noise_mapping are right.
 *
 * @param measurement_mapping The matrix @f$ H_k @f$.
 * @param measurement_noise The measurement noise @f$ w_k @f$
 * @param noise_mapping The matrix @f$ M_k @f$.
 */
LinearMeasurementModel::LinearMeasurementModel(
    const Eigen::MatrixXd& measurement_mapping,
    const DistributionInterface& measurement_noise,
    const Eigen::MatrixXd& noise_mapping)
    : LinearizedMeasurementModel(measurement_mapping.cols(),
                                 measurement_mapping.rows(),
                                 measurement_noise) {
  this->setMeasurementParameters(measurement_mapping,
                                 measurement_noise,
                                 noise_mapping);
}

/**
 * Sets the noise mapping to an identity matrix.
 *
 * @param measurement_mapping The matrix @f$ H_k @f$.
 * @param measurement_noise The measurement noise @f$ w_k @f$.
 */
void LinearMeasurementModel::setMeasurementParameters(
    const Eigen::MatrixXd& measurement_mapping,
    const DistributionInterface& measurement_noise) {
  this->setMeasurementParameters(
      measurement_mapping,
      measurement_noise,
      Eigen::MatrixXd::Identity(measurement_mapping.rows(),
                                measurement_noise.mean().size()));
}

/**
 * Sets all parameters of the linear measurement model.
 *
 * Also checks that the dimensions of @e noise_mapping are right.
 *
 * @param measurement_mapping The matrix @f$ H_k @f$.
 * @param measurement_noise The measurement noise @f$ w_k @f$.
 * @param noise_mapping The matrix @f$ M_k @f$.
 */
void LinearMeasurementModel::setMeasurementParameters(
    const Eigen::MatrixXd& measurement_mapping,
    const DistributionInterface& measurement_noise,
    const Eigen::MatrixXd& noise_mapping) {
  CHECK_EQ(measurement_mapping.rows(), noise_mapping.rows());
  CHECK_EQ(noise_mapping.cols(), measurement_noise.mean().size());

  measurement_mapping_ = measurement_mapping;
  noise_mapping_ = noise_mapping;

  this->setMeasurementModelBaseParameters(measurement_mapping.cols(),
                                          measurement_mapping.rows(),
                                          measurement_noise);
}

/**
 * Also checks that the state vector has compatible dimensions with the
 * measurement model.
 *
 * @param state The current system state.
 * @return the expected measurement.
 */
Eigen::VectorXd LinearMeasurementModel::observe(
    const Eigen::VectorXd& state) const {
  CHECK_NE(this->getStateDim(), 0)
      << " Measurement model has not been initialized.";
  CHECK_EQ(state.size(), this->getStateDim());

  return measurement_mapping_ * state
      + noise_mapping_ * this->getMeasurementNoise()->mean();
}

/**
 * Since this is only a linear model, it only returns @f$ H_k @f$.
 *
 * @param state The current system state.
 * @return the measurement model Jacobian w.r.t. the system state.
 */
Eigen::MatrixXd LinearMeasurementModel::getMeasurementJacobian(
    const Eigen::VectorXd& state) const {
  CHECK_NE(this->getStateDim(), 0)
      << " Measurement model has not been initialized.";
  return measurement_mapping_;
}

/**
 * Since this is only a linear model, it only returns @f$ M_k @f$.
 *
 * @param state The current system state.
 * @return the measurement model Jacobian w.r.t. the measurement noise.
 */
Eigen::MatrixXd LinearMeasurementModel::getNoiseJacobian(
    const Eigen::VectorXd& state) const {
  CHECK_NE(this->getStateDim(), 0)
      << " Measurement model has not been initialized.";
  return noise_mapping_;
}

}  // namespace refill
