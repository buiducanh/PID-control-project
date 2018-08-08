#ifndef TWIDDLE_H
#define TWIDDLE_H

class Twiddle {
  public:
  // ending conditions
  double _tol;
  unsigned int _max_iter;

  // twiddling parameters
  double _p[3], _dp[3];

  // search sensitivity
  double _alpha, _beta;

  // length of each run to probe again
  unsigned int _run_steps;
  unsigned int _step;
  // parameter index we are twiddling
  unsigned int _pi;
  // flag if we need to minus probing
  bool _tw_reset_flag[3];
  double _best_err;
  double _cur_err;
  bool _tw_init;
  unsigned int _tw_i;

  /*
  * Constructor
  */
  Twiddle(double p[3], double dp[3], double alpha, double beta, unsigned int run_steps, double tol, unsigned int max_iter);

  /*
  * Destructor.
  */
  virtual ~Twiddle();

  void run(double err);

  bool canRun();
};

#endif /* TWIDDLE_H */
