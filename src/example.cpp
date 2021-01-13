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

// Signal Handler
void signalHandler(int signum) {
  void *array[10];
  size_t size;

  Ephemera<data_t>::Active = false;
  std::cerr << "Interrupt signal (" << signum << ") received" << std::endl;

  // Print stack trace in event of segmentation fault
  if (signum == SIGSEGV) {
    // Get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // Print out all the frames to stderr
    std::cerr << "Error: signal " << signum << std::endl;
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
  }
}

int main() {
  // Trigger execution of signalHandler if we receive these interrupts
  signal(SIGHUP, signalHandler);
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);
  signal(SIGPIPE, signalHandler);
  signal(SIGSEGV, signalHandler);

  // Create instance of Ephemera cache
  Ephemera<data_t> ec;

  // Spawn cache expiry thread
  std::thread cacheExpiry(Ephemera<data_t>::cacheExpiryLoop, std::ref(ec));

  // Example cache insertion using default expiry (60 seconds)
  data_t value1 = {"foo", 123};
  std::string key = "key1";
  bool err = ec.set(key, value1);
  key = "key2";
  err = ec.set(key, value1);
  key = "key3";
  err = ec.set(key, value1);
  err = ec.set(key, value1);
  if (err == false) {
    std::cout << "The key '" << key << "' already exists" << std::endl;
  }

  // Example cache insertion using explicit expiry
  data_t value2 = {"bar", 321};
  key = "keyA";
  err = ec.set(key, value2, 2*DEFAULT_TTL);
  key = "keyB";
  err = ec.set(key, value2, 3*DEFAULT_TTL);
  key = "keyC";
  err = ec.set(key, value2, 4*DEFAULT_TTL);

  // Example cache retrieval
  key = "keyC";
  data_t value3;
  bool found = ec.get(key, value3);
  if (found == true) {
    std::cout << "Key: '" << key << "'" << std::endl;
    std::cout << "Value: " << value3 << std::endl;
  } else {
    std::cout << "Unable to find key: '" << key << "'" << std::endl;
  }

  key = "keyD";
  found = ec.get(key, value3);
  if (found == true) {
    std::cout << "Key: '" << key << "'" << std::endl;
    std::cout << "Value: " << value3 << std::endl;
  } else {
    std::cout << "Unable to find key: '" << key << "'" << std::endl;
  }

  // Waiting for cache expiry thread to join
  cacheExpiry.join();

  std::cout << "Done" << std::endl;

  return 0;
}
