#include "symbolic_functions.h"

#include <limits>
#include <cmath>
#include <sstream>
#include <string>

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double LinearFunction::value(double x) {
  return slope_ * x + intercept_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string LinearFunction::Print() {
  std::stringstream ss("");
  ss << "y = " << slope_ << " * x + " << intercept_;
  return ss.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double ExponentialFunction::value(double x) {
  double exp_input = exponent_ * x;
  if (exp_input > std::numeric_limits<double>::max_exponent) {
    return std::numeric_limits<double>::infinity();
  } else if (exp_input < std::numeric_limits<double>::min_exponent) {
    return intercept_;  // exp(-inf) is 0, so we return intercept_
  }
  return constant_ * std::exp(exp_input) + intercept_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string ExponentialFunction::Print() {
  std::stringstream ss("");
  ss << "y = " << constant_
     << " * exp(" << exponent_ << " * x) + " << intercept_;
  return ss.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double PiecewiseFunction::value(double x) {
  double ret;
  if (functions_.empty() || (x < functions_.front().xoffset)) {
    ret = 0.0;
  } else {
    std::list<PiecewiseFunctionInfo>::iterator f = functions_.begin();
    std::list<PiecewiseFunctionInfo>::iterator next = std::next(f);
    while (next != functions_.end() && (x >= next->xoffset)) {
      f = next;
      ++next;
    }
    ret = f->function->value(x - f->xoffset) + f->yoffset;
  }
  return ret;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string PiecewiseFunction::Print() {
  std::stringstream ss("");
  ss << "Piecewise Function comprised of: ";
  std::list<PiecewiseFunctionInfo>::iterator f;
  for (f = functions_.begin(); f != functions_.end(); f++) {
    ss << " * " << f->function->Print()
       << " starting at coordinate (" << f->xoffset << ","
       << f->yoffset << ")";
  }
  return ss.str();
}

}  // namespace toolkit
}  // namespace cyclus
