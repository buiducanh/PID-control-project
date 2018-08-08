#include "Twiddle.h"
#include "algorithm"

using namespace std;

Twiddle::Twiddle(double p[3], double dp[3], double alpha,
    double beta, unsigned int run_steps, double tol, unsigned int max_iter)
{
  _tol = tol;
  _max_iter = max_iter;
  std::copy(p, p + 3, _p);
  std::copy(dp, dp + 3, _dp);
  _alpha = alpha;
  _beta = beta;
  _run_steps = run_steps;
  _pi = 0;
  std::fill(_tw_reset_flag, _tw_reset_flag + 3, true);
  _best_err = 1 << 31;
  _cur_err = 0;
  _tw_init = false;
  _tw_i = 0;
  _step = 0;
}

Twiddle::~Twiddle() {}

void Twiddle::run(double err)
{
  if (canRun()) {
    ++_step %= _run_steps;
    _cur_err += abs(err);
    // each twiddle step
    if (_step == 0) {
      _tw_i++;
      _cur_err /= _run_steps;
      _p[_pi] += _dp[_pi];

      // on first run just init best_err
      if (!_tw_init) {
        _tw_init = true;
        _best_err = _cur_err;
        _tw_i = 0;
      } else {
        if (_cur_err < _best_err) {
          // we are decreasing avg err so search more agressively
          _best_err = _cur_err;
          _dp[_pi] *= _alpha;

          // move to next param
          ++_pi %= 3;
        } else if (_tw_reset_flag[_pi]) {
          // we try the lower search band before tightening the search band
          _p[_pi] -= 2 * _dp[_pi];
          _tw_reset_flag[_pi] = !_tw_reset_flag[_pi];
        } else {
          // lower search band didn't work so we move to the middle and tighten the search band
          _p[_pi] += _dp[_pi];
          _dp[_pi] *= _beta;

          _tw_reset_flag[_pi] = !_tw_reset_flag[_pi];
          // move to next param
          ++_pi %= 3;
        }
      }
      _cur_err = 0;
    }
  }
}

bool Twiddle::canRun()
{
  return (_dp[0] + _dp[1] + _dp[2] > _tol) && (_tw_i < _max_iter);
}
