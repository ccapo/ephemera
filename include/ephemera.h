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
#ifndef INCLUDE_EPHEMERA_H_
#define INCLUDE_EPHEMERA_H_

#include <unordered_map>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <execinfo.h>
#include <signal.h>
#include <math.h>

#include <string>
#include <map>
#include <list>
#include <vector>

// Semantic Version
#define VERSION ("0.2.1")

// Milliseconds to second
#define MS (1000)

// Default TTL (seconds)
#define DEFAULT_TTL (60)

// Ephemera Class
template <typename T>
class Ephemera {
 public:
  struct wrap_t {
    T value;
    time_t ttl;
    time_t expiry;
    wrap_t(): ttl(0), expiry(0) {}
    explicit wrap_t(T t): value(t), ttl(0), expiry(0) {}
    wrap_t(T t, time_t ttl): value(t), ttl(ttl), expiry(ttl) {}
    wrap_t(T t, time_t ttl, time_t expiry): value(t), ttl(ttl), expiry(expiry) {}
  };

  Ephemera() {
    std::cout << "Ephemera v" << VERSION << std::endl;
  }

  // Inserts entry in cache
  bool set(const std::string &key, const T &value, const time_t &expiry) {
    // Validate expiry is positive
    if (expiry <= 0) {
      std::cerr << "Expiry time can not be negative" << std::endl;
      return false;
    }

    // Check if key already exists
    T existingValue;
    bool found = get(key, existingValue);
    if (found == true) {
      return false;
    }

    // Insert entry if key does not already exist
    time_t ttl = expiry + time(NULL);
    std::list<std::string> keys = getKeys(ttl);
    keys.push_back(key);
    ttl_cache[ttl] = keys;

    // Wrap value with TTL and expiry
    wrap_t w = wrap_t(value, ttl, expiry);
    value_cache[key] = w;

    if (logLevel >= DEBUG) {
      std::cout << "Inserted key: '" << key;
      std::cout << "' with an expiry of " << expiry << " seconds" << std::endl;
    }
    return true;
  }

  // Inserts entry in cache using default TTL
  bool set(const std::string &key, const T &value) {
    // Use default TTL
    return set(key, value, DEFAULT_TTL);
  }

  // Get value for key
  bool get(const std::string &key, T &value) {
    typename std::unordered_map<std::string, wrap_t>::const_iterator found = value_cache.find(key);
    if (found != value_cache.end()) {
      // TODO(ccapo): Add feature to extend TTL of this entry by expiry
      value = found->second.value;
      return true;
    }
    return false;
  }

  // Static flag to control threads
  static volatile bool Active;

  // Logging Levels
  enum LogLevel {
    ERROR,
    WARN,
    INFO,
    DEBUG,
    VERBOSE
  };
  static LogLevel logLevel;

  // Static function called within a thread to check if cache keys have expired
  static void cacheExpiryLoop(Ephemera<T> e) {
    while (Ephemera<T>::Active == true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(MS));
      e.purgeTTLKeys();
    }

    if (logLevel >= DEBUG) std::cout << "cacheExpiryLoop exited" << std::endl;
  }

 private:
  // TTL cache, permitting multiple keys for a single TTL
  std::map <time_t, std::list<std::string> > ttl_cache;

  // Value cache
  std::unordered_map <std::string, wrap_t> value_cache;

  // Gets list of keys with same TTL
  std::list<std::string> getKeys(const time_t &ttl) {
    std::list<std::string> keys;
    std::map<time_t, std::list<std::string> >::const_iterator found = ttl_cache.find(ttl);
    if (found != ttl_cache.end()) {
      keys = found->second;
    }
    return keys;
  }

  // Purge expired ttl and keys from cache
  void purgeTTLKeys() {
    time_t now = time(NULL);
    time_t ttlExpired;
    std::list<std::string> keysExpired;

    // Iterate through TTL cache in increasing order
    for (auto ttlKeys = ttl_cache.begin(); ttlKeys != ttl_cache.end(); ++ttlKeys) {
      // If the first TTL entry matches the current time, or is in the past
      if (difftime(ttlKeys->first, now) <= 0) {
        // Store expired TTL entry, collect list
        ttlExpired = ttlKeys->first;
        keysExpired = ttlKeys->second;
        if (logLevel >= VERBOSE) {
          for (auto key : ttlKeys->second) {
            std::cout << "Expired key: '" << key << "'" << std::endl;
          }
        }
        if (logLevel >= DEBUG) {
          std::cout << keysExpired.size() << " keys expired" << std::endl;
        }
      } else {
        break;
      }
    }

    // If we found an expired cache ttl entry, purge it
    if (ttlExpired != NULL) {
      ttl_cache.erase(ttlExpired);
    }

    // If we found expired cache key entries, purge them
    if (!keysExpired.empty()) {
      for (auto key : keysExpired) {
        value_cache.erase(key);
      }
      keysExpired.clear();
    }
  }
};

template <typename T> typename Ephemera<T>::LogLevel Ephemera<T>::logLevel = INFO;

template <typename T> volatile bool Ephemera<T>::Active = true;

#endif  // INCLUDE_EPHEMERA_H_
