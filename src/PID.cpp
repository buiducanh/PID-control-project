#include "PID.h"

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
  _Kp = Kp;
  _Ki = Ki;
  _Kd = Kd;
  p_error = i_error = d_error = 0;
}

void PID::setCoeffs(double p[3]) {
  _Kp = p[0];
  _Ki = p[1];
  _Kd = p[2];
}

void PID::UpdateError(double cte) {
  d_error = cte - p_error;
  p_error = cte;
  i_error += cte;
}

double PID::TotalError() {
  return _Kp * p_error + _Ki * i_error + _Kd * d_error;
}

