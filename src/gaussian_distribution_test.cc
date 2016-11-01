#include <gtest/gtest.h>
#include <Eigen/Dense>

#include "refill/gaussian_distribution.h"

namespace refill {

TEST(GaussianDistributionTest, Basics) {
  GaussianDistribution<> a;
  GaussianDistribution<> b;
  GaussianDistribution<> c;

  c = a + b;

  ASSERT_EQ(c.mean(), a.mean()) << "Mean not correct";
  ASSERT_EQ(c.cov(), 2.0 * a.cov()) << "Variances do not match";
}

}  // namespace refill