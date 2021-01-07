# Ephemera
Write-Only, In-Memory, Header File-Only, Key-Value Cache Library

## Requirements
- Include `include/ephemera.h` and `include/data.h` into project.
- The `libpthread` library is required (see `Makefile` for compilation options).
- User must provide a struct defintion for `data_t`, including a helper function to test if an instance of the struct is unpopulated (see `include/data.h` for an example). Optionally the user should provide a custom output stream function for `data_t`.

## Usage
- A signal handler should be used to set `Ephemera::Active` to `false` to shutdown the cache expiry thread.
- Spawn the cache expiry thread: `std::thread cacheExpiry(Ephemera::instance()->cacheExpiryLoop);`
- After spawning cache expiry thread the following: `cacheExpiry.join();` will block until the cache expiry thread re-joins the main thread.
- To insert a new key-value pair: `Ephemera::instance()->set(key, value, DEFAULT_TTL);` (where DEFAULT_TTL is 60 seconds)
- To retrieve the value for a key: `data_t value = Ephemera::instance()->get(key);`
- It is good practice to test if the returned value is unpopulated: `if (value.empty() == false) std::cout << value << std::endl;`

## Features
- Simple API: `set` and `get`
- User-specified data value

## Limitations
- Does not include commands to update or delete a key.
- Negative expiry times are not permitted.

## Examples
- See examples in the `src` directory
- To build examples, run: `make all`
