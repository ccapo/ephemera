#include "data.h"
#include "ephemera.h"

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

  // Example cache insertion using default expiry (60 seconds)
  data_t value1 = {"foo", 123};
  Ephemera::instance()->set("key1", value1);
  Ephemera::instance()->set("key2", value1);
  Ephemera::instance()->set("key3", value1);

  // Example cache insertion using explicit expiry
  data_t value2 = {"bar", 321};
  Ephemera::instance()->set("keyA", value2, 2*DEFAULT_TTL);
  Ephemera::instance()->set("keyB", value2, 3*DEFAULT_TTL);
  Ephemera::instance()->set("keyC", value2, 4*DEFAULT_TTL);

  // Example cache retrieval
  std::string key = "keyC";
  data_t value3 = Ephemera::instance()->get(key);
  std::cout << "Key: '" << key << "'" << std::endl;
  std::cout << "Value: " << value3 << std::endl;

  // Waiting for cache expiry thread to join
	cacheExpiry.join();

  std::cout << "Done" << std::endl;

	return OK;
}
