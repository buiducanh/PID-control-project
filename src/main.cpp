#include "PID.h"
#include "Twiddle.h"
#include "json.hpp"
#include <iostream>
#include <math.h>
#include <uWS/uWS.h>

// for convenience
using json = nlohmann::json;
using namespace std;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s)
{
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  } else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main()
{
  uWS::Hub h;

  PID pid;
  // TODO: Initialize the pid variable.

  // initialize twiddle parameters
  double p[3] = { 0.14, 0.0005, 0.7 };
  // search sensitivity
  double alpha = 1.1, beta = 0.9;
  // probing steps
  double dp[3] = { 0.0001, 0, 0.005 };
  // length of each run to probe again
  int run_steps = 5;
  double tol = 0.00000001;
  unsigned int max_tw_i = 10000;

  Twiddle tw(p, dp, alpha, beta, run_steps, tol, max_tw_i);

  pid.Init(p[0], p[1], p[2]);

  h.onMessage([&pid, &tw](uWS::WebSocket<uWS::SERVER> ws, char* data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2') {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value;
          /*
          * TODO: Calcuate steering value here, remember the steering value is
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */

          // evaluate twiddle parameters
          //tw.run(cte);

          /* Twiddle tuning
          cout << endl
               << endl
               << "< ------------------------------------------------------------>" << endl
               << endl
               << endl
               << endl;
          cout << "Iteration " << tw._tw_i << ", Best Err " << tw._best_err << endl;
          cout << "DP " << tw._dp[0] << " " << tw._dp[1] << " " << tw._dp[2] << endl;
          cout << "P " << tw._p[0] << " " << tw._p[1] << " " << tw._p[2] << endl
               << endl
               << endl;
          cout << "< ------------------------------------------------------------>" << endl
               << endl
               << endl;
          */

          pid.setCoeffs(tw._p);
          pid.UpdateError(cte);
          steer_value = -pid.TotalError();
          if (steer_value < -1) {
            steer_value = -1;
          }

          if (steer_value > 1) {
            steer_value = 1;
          }

          // DEBUG
          std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.3;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse* res, uWS::HttpRequest req, char* data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1) {
      res->end(s.data(), s.length());
    } else {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char* message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
