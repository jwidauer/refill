#include "refill/filters/extended_kalman_filter.h"

#include <Eigen/Dense>

namespace refill {

ExtendedKalmanFilter::ExtendedKalmanFilter()
    : system_model_(new LinearSystemModel()),
      measurement_model_(new LinearMeasurementModel()) {}

ExtendedKalmanFilter::ExtendedKalmanFilter(
    const GaussianDistribution& initial_state)
    : state_(initial_state),
      system_model_(nullptr),
      measurement_model_(nullptr) {}

ExtendedKalmanFilter::ExtendedKalmanFilter(
    const GaussianDistribution& initial_state,
    std::unique_ptr<LinearizedSystemModel> system_model,
    std::unique_ptr<LinearizedMeasurementModel> measurement_model)
    : state_(initial_state),
      system_model_(std::move(system_model)),
      measurement_model_(std::move(measurement_model)) {
  const int state_dimension = state_.mean().size();
  const Eigen::VectorXd zero_input = Eigen::VectorXd::Zero(state_dimension);

  CHECK_EQ(system_model->getStateJacobian(state_.mean(), zero_input).rows(),
           state_dimension);
  CHECK_EQ(measurement_model->getMeasurementJacobian(state_.mean()).cols(),
           state_dimension);
}

void ExtendedKalmanFilter::setState(const GaussianDistribution& state) {
  state_ = state;
}

void ExtendedKalmanFilter::predict() {
  CHECK_NE(this->sytem_model_, nullptr) << "No default system model provided.";
  this->predict(Eigen::VectorXd::Zero(this->system_model_->getInputDim()));
}

void ExtendedKalmanFilter::predict(const Eigen::VectorXd& input) {
  CHECK_NE(this->sytem_model_, nullptr) << "No default system model provided.";
  this->predict(*this->system_model_, input);
}

void ExtendedKalmanFilter::predict(const LinearizedSystemModel& system_model) {
  this->predict(system_model,
                Eigen::VectorXd::Zero(system_model.getInputDim()));
}

void ExtendedKalmanFilter::predict(const LinearizedSystemModel& system_model,
                                   const Eigen::VectorXd& input) {
  CHECK_EQ(system_model.getStateDim(), state_.mean().size());
  CHECK_EQ(system_model.getInputDim(), input.size());

  const Eigen::MatrixXd system_jacobian =
      system_model.getStateJacobian(state_.mean(), input);
  const Eigen::MatrixXd noise_jacobian =
      system_model.getNoiseJacobian(state_.mean(), input);

  const Eigen::VectorXd new_state_mean =
      system_model.propagate(state_.mean(), input);
  const Eigen::MatrixXd new_state_cov =
      system_jacobian * state_.cov() * system_jacobian.transpose() +
      noise_jacobian * system_model.getSystemNoise()->cov() *
          noise_jacobian.transpose();

  state_.setDistParam(new_state_mean, new_state_cov);
}

void ExtendedKalmanFilter::update(const Eigen::VectorXd& measurement) {
  CHECK_NE(this->measurement_model_, nullptr)
      << "No default measurement model provided.";
  this->update(*this->measurement_model_, measurement);
}

void ExtendedKalmanFilter::update(
    const LinearizedMeasurementModel& measurement_model,
    const Eigen::VectorXd& measurement) {
  CHECK_EQ(measurement_model.getMeasurementDim(), measurement.size());
  CHECK_EQ(measurement_model.getStateDim(), state_.mean().size());

  const Eigen::MatrixXd measurement_jacobian =
      measurement_model.getMeasurementJacobian(state_.mean());
  const Eigen::MatrixXd noise_jacobian =
      measurement_model.getNoiseJacobian(state_.mean());

  const Eigen::VectorXd innovation =
      measurement - measurement_model.observe(state_.mean());
  const Eigen::MatrixXd residual_cov =
      measurement_jacobian * state_.cov() * measurement_jacobian.transpose() +
      noise_jacobian * measurement_model.getMeasurementNoise()->cov() *
          noise_jacobian.transpose();
  const Eigen::MatrixXd kalman_gain =
      state_.cov() * measurement_jacobian.transpose() * residual_cov.inverse();

  const Eigen::VectorXd new_state_mean =
      state_.mean() + kalman_gain * innovation;
  const Eigen::MatrixXd new_state_cov =
      state_.cov() - kalman_gain * residual_cov * kalman_gain.transpose();

  state_.setDistParam(new_state_mean, new_state_cov);
}

}  // namespace refill
