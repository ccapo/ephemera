// MIT License
//
// Copyright (c) 2021 Chris Capobianco
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include <string>

#include "include/data.h"
#include "include/ephemera.h"

#define NKEYS (1000000)

// Signal Handler
void signalHandler(int signum) {
  void *array[10];
  size_t size;

  Ephemera::Active = false;
  std::cerr << "Interrupt signal (" << signum << ") received" << std::endl;

  // Print stack trace in event of segmentation fault
  if (signum == SIGSEGV) {
    // Get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // Print out all the frames to stderr
    std::cerr << "Error: signal " << signum << std::endl;
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(ERROR);
  }
}

int main() {
  // Trigger execution of signalHandler if we receive these interrupts
  signal(SIGHUP, signalHandler);
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);
  signal(SIGPIPE, signalHandler);
  signal(SIGSEGV, signalHandler);

  // Spawn cache expiry thread
  std::thread cacheExpiry(Ephemera::instance()->cacheExpiryLoop);

  // Benchmark cache insertion
  data_t value = {"foo", 123};
  for (int i = 0; i < NKEYS; ++i) {
    std::string key = "key" + std::to_string(i);
    Ephemera::instance()->set(key, value, 1 + floor(i/100000.0));
  }

  // Check for last inserted key, then gracefully shutdown cache
  data_t v = Ephemera::instance()->get("key999999");
  while (v.empty() == false) {
    std::this_thread::sleep_for(std::chrono::milliseconds(MS));
    v = Ephemera::instance()->get("key999999");
  }
  std::cout << std::endl;
  Ephemera::Active = false;

  // Waiting for cache expiry thread to join
  cacheExpiry.join();

  std::cout << "Done" << std::endl;

  return OK;
}
