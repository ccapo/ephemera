#pragma once

#include "common.h"

// Ephemera Class
class Ephemera {
  // TTL cache, permitting multiple keys for a single TTL
	std::map <time_t, std::list<std::string> > ttl_cache;

	// Data cache
	std::unordered_map <std::string, data_t> data_cache;

	// Singleton cache instance
	static Ephemera *s_instance;

	// Gets list of keys with same TTL
	std::list<std::string> getKeys(const time_t &ttl) {
	  std::list<std::string> keys;
	  std::map<time_t, std::list<std::string> >::const_iterator found = ttl_cache.find(ttl);
	  if(found != ttl_cache.end()) {
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
				if (DEBUG) {
			    //for (auto key : ttlKeys->second) {
				  //  std::cout << "Expired Key: '" << key << "'" << std::endl;
			    //}
			    std::cout << "# of Keys Expired: " << keysExpired.size() << std::endl;
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
				data_cache.erase(key);
			}
			keysExpired.clear();
		}
	}

public:
  Ephemera() {
    std::cout << "Ephemera v" << MAJOR << "." << MINOR << "." << REVISION << std::endl;
  }

  // Inserts entry in cache
	void set(const std::string &key, const data_t &value, const time_t &expiry) {
    // Validate expiry is positive
    if (expiry <= 0) {
      std::cout << "Expiry can not be negative" << std::endl;
      return;
    }

		// Check if key already exists
	  data_t existingValue = get(key);
	  if (existingValue.empty() == false) {
      std::cout << "Key '" << key << "' already exists" << std::endl;
      return;
    }

    // Insert entry if key does not already exist
    time_t ttl = expiry + time(NULL);
	  std::list<std::string> keys = getKeys(ttl);
	  keys.push_back(key);
	  ttl_cache[ttl] = keys;
	  data_cache[key] = value;
	  if(DEBUG) std::cout << "Added Key: '" << key << "' with an expiry of " << expiry << " seconds" << std::endl;
	}

  // Inserts entry in cache using default TTL
	void set(const std::string &key, const data_t &value) {
    // Use default TTL
    const time_t expiry = DEFAULT_TTL;

		// Check if key already exists
	  data_t existingValue = get(key);
	  if (existingValue.empty() == false) {
      std::cout << "Key '" << key << "' already exists" << std::endl;
      return;
    }

    // Insert entry if key does not already exist
    time_t ttl = expiry + time(NULL);
	  std::list<std::string> keys = getKeys(ttl);
	  keys.push_back(key);
	  ttl_cache[ttl] = keys;
	  data_cache[key] = value;
	  if(DEBUG) std::cout << "Added Key: '" << key << "' with an expiry of " << expiry << " seconds" << std::endl;
	}

	// Get data for key
	data_t get(const std::string &key) {
	  data_t value;
	  std::unordered_map<std::string, data_t>::const_iterator found = data_cache.find(key);
	  if(found != data_cache.end()) {
      value = found->second;
    }
	  return value;
	}

  // Singleton instance of Cache
  static Ephemera *instance() {
    if (!s_instance) {
      s_instance = new Ephemera();
    }
    return s_instance;
  }

  // Static flag to control threads
	volatile static bool Active;

  // Static function called within a thread to check if cache keys have expired
  static void cacheExpiryLoop() {
	  while (Ephemera::Active == true) {
		  std::this_thread::sleep_for(std::chrono::milliseconds(MS));
		  Ephemera::instance()->purgeTTLKeys();
	  }

	  if(DEBUG) std::cout << "cacheExpiryLoop exited" << std::endl;
  }
};

volatile bool Ephemera::Active = true;

// Allocating and initializing Ephemera's static data member.
// The pointer is being allocated, not the object inself.
Ephemera *Ephemera::s_instance = 0;
