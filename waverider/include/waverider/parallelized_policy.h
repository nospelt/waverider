#ifndef WAVERIDER_PARALLELIZED_POLICY_H_
#define WAVERIDER_PARALLELIZED_POLICY_H_

#include <vector>

#include <rmpcpp/core/policy_value.h>

#include "waverider/obstacle_filter.h"
#include "waverider/policy_tuning.h"

namespace waverider {
class ParallelizedPolicy {
 public:
  ParallelizedPolicy(uint num_policies, PolicyTuning tuning);

  void init(const std::vector<Eigen::Vector3f>& x_obs, const Eigen::Vector3f& x,
            const Eigen::Vector3f& xdot);

  rmpcpp::PolicyValue<3> getPolicy() {
    return {(A_sum.completeOrthogonalDecomposition().pseudoInverse() * Af_sum)
                .cast<double>(),
            A_sum.cast<double>()};
  }

  inline Eigen::Vector3f s(const Eigen::Vector3f& x) { return x / h(x.norm()); }

  // Softmax helper function
  inline float h(float z) {
    return (z + tuning_.c * std::log(1 + std::exp(-2.f * tuning_.c * z)));
  }
  inline static float wr(float s, float r) {
    const float c2 = 1 / (r * r);
    const float c1 = -2 / r;
    return (static_cast<float>(c2) * s * s) + (c1 * s) + 1.f;
  }

  void setR(float /*r*/) {}

 private:
  PolicyTuning tuning_;

  uint num_policies_;
  Eigen::Vector3f Af_sum{Eigen::Vector3f::Zero()};
  Eigen::Matrix3f A_sum{Eigen::Matrix3f::Zero()};
  Eigen::Matrix<float, 3, Eigen::Dynamic> f;
  Eigen::Matrix<float, 9, Eigen::Dynamic> A;
};
}  // namespace waverider

#endif  // WAVERIDER_PARALLELIZED_POLICY_H_
