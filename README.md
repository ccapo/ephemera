# Ephemera
An In-Memory, Write-Only, Header-Only, Key-Value Cache Library

## Build Status
![C/C++ CI](https://github.com/ccapo/ephemera/workflows/C/C++%20CI/badge.svg)

## Requirements
- Include `include/ephemera.h`.
- The `libpthread` library is required (see `Makefile` for compilation options).
- User must provide a data type for the template (see `include/data.h` for an example). Optionally the user should provide a custom output stream function for their data type.

## Usage
- A signal handler should be used to set `Ephemera::Active` to `false` to shutdown the cache expiry thread.
- Create an instance of `Ephemera<T> ec;`, where is `T` is the user supplied data type to be stored.
- Spawn the cache expiry thread: `std::thread cacheExpiry(Ephemera<T>::cacheExpiryLoop, std::ref(ec));`
- After spawning cache expiry thread the following: `cacheExpiry.join();` will block until the cache expiry thread re-joins the main thread.
- To insert a new key-value pair: `ec.set(key, value, ttl);` (the default ttl is 60 seconds)
- To retrieve the value for a key: `T value = ec.get(key);`

## Features
- Simple API: `set` and `get`
- User-specified data type

## Limitations
- Does not include commands to update or delete a key.
- Negative expiry times are not permitted.

## Examples
- See examples in the `src` directory
- To build examples, run: `make all`
