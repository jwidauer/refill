#ifndef REFILL_SYSTEM_MODELS_SYSTEM_MODEL_BASE_H_
#define REFILL_SYSTEM_MODELS_SYSTEM_MODEL_BASE_H_

#include <Eigen/Dense>
#include <glog/logging.h>

#include <cstdlib>
#include <memory>

#include "refill/distributions/distribution_base.h"

using std::size_t;

namespace refill {

/**
 * @brief Interface for system models.
 *
 * All system models must have this class as an ancestor.
 */
class SystemModelBase {
 public:
  /**
   * @brief Propagates a state and input vector through the system model.
   *
   * @param state The state vector to be propagated.
   * @param input The input vector to the system.
   * @return the new state vector.
   */
  virtual Eigen::VectorXd propagate(const Eigen::VectorXd& state,
                                    const Eigen::VectorXd& input) const = 0;

  /** @brief Returns the systems state dimension. */
  size_t getStateDim() const;
  /** @brief Returns the systems input dimension. */
  size_t getInputDim() const;
  /** @brief Returns the systems noise dimension. */
  size_t getSystemNoiseDim() const;
  /** @brief Returns a pointer to the system noise. */
  DistributionInterface* getSystemNoise() const;

 protected:
  /** Default constructor should not be used. */
  SystemModelBase() = delete;
  /** @brief Constructor for a system model without an input. */
  SystemModelBase(const size_t& state_dim,
                  const DistributionInterface& system_noise);
  /** @brief Constructor for a system model with input. */
  SystemModelBase(const size_t& state_dim,
                  const DistributionInterface& system_noise,
                  const size_t& input_dim);

  /** @brief Function to set the system model parameters without an input. */
  void setSystemModelBaseParameters(const size_t& state_dim,
                                    const DistributionInterface& system_noise);
  /** @brief Function to set the system model parameters with an input. */
  void setSystemModelBaseParameters(const size_t& state_dim,
                                    const DistributionInterface& system_noise,
                                    const size_t& input_dim);

 private:
  size_t state_dim_;
  size_t input_dim_;
  std::unique_ptr<DistributionInterface> system_noise_;
};

}  // namespace refill

#endif  // REFILL_SYSTEM_MODELS_SYSTEM_MODEL_BASE_H_
