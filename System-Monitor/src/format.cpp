#include <string>
#include<iostream>
#include "format.h"

using std::string;
// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
  int hour = seconds / 3600;
  short minute = (seconds % 3600) / 60;
  short second = (seconds % 3600) % 60;
  char buffer[9];
  sprintf(buffer, "%.2d:%.2d:%.2d", hour, minute, second);
  string elapsed_time(buffer);
  return elapsed_time;
}