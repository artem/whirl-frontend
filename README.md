# Whirl

Distributed system deterministic simulator

## Example

[KV](/examples/kv/main.cpp) - simple replicated KV store

- Quorum reads/writes
- Last-write-wins via wall clock
- Persistent local storage

Intentionally non-linearizable.

Use Attiya, Bar-Noy, Dolev (ABD) algorithm for linearizable behaviour.

## Features

* Rich set of concurrency primitives
* Communication via RPC
* Deterministic execution, deterministic randomness
* Time compression
* Linearizability checker
* Persistent storage and node restarts
* Local clock skew and drift
* Google TrueTime simulation
* Pluggable asynchrony and fault injection strategy
* Logging, RPC tracing

## Inspiration

- [Testing Distributed Systems w/ Deterministic Simulation](https://www.youtube.com/watch?v=4fFDFbi3toc)
- [FoundationDB or: How I Learned to Stop Worrying and Trust the Database](https://www.youtube.com/watch?v=OJb8A6h9jQQ&list=PLSE8ODhjZXjagqlf1NxuBQwaMkrHXi-iz&index=22)
- [Teaching Rigorous Distributed Systems With Efficient Model Checking](https://ellismichael.com/papers/dslabs-eurosys19.pdf)

## Requirements

- x86-64
- Clang++ (>= 8)

## Dependencies

- [await](https://gitlab.com/Lipovsky/await) – concurrency: fibers + futures with customizable runtime
- [cereal](https://github.com/USCiLab/cereal) – serialization
- [fmt](https://github.com/fmtlib/fmt)
- [ctti](https://github.com/Manu343726/ctti)

## Build

```shell
# Clone repo
git clone https://gitlab.com/Lipovsky/whirl.git 
cd whirl
# Generate build files
mkdir build && cd build
cmake -DWHIRL_EXAMPLES=ON ..
# Build kv example
make whirl_example_kv
# Run kv example
./examples/kv/bin/whirl_example_kv
