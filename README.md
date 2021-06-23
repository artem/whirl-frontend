# Whirl

Distributed system deterministic simulator

## Example

[KV](/examples/kv/main.cpp) – simple replicated KV store

- Quorum reads/writes
- Last-write-wins via wall clock
- Persistent local storage

Note: Implementation _intentionally_ violates [linearizability](https://jepsen.io/consistency/models/linearizable)!

Use Attiya, Bar-Noy, Dolev (ABD) algorithm for linearizable behaviour:
- [Original paper](https://groups.csail.mit.edu/tds/papers/Attiya/TM-423.pdf)
- [Notes on Theory of Distributed Systems](https://www.cs.yale.edu/homes/aspnes/classes/465/notes.pdf) – Chapter 16 – _Distributed Shared Memory_

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

### Simulation + Fault Injection

#### FoundationDB
- [Testing Distributed Systems w/ Deterministic Simulation](https://www.youtube.com/watch?v=4fFDFbi3toc)
- [FoundationDB or: How I Learned to Stop Worrying and Trust the Database](https://www.youtube.com/watch?v=OJb8A6h9jQQ&list=PLSE8ODhjZXjagqlf1NxuBQwaMkrHXi-iz&index=22)

#### Jepsen
- https://github.com/jepsen-io/maelstrom

#### Paxi
- [Dissecting the Performance of Strongly-Consistent Replication Protocols](https://cse.buffalo.edu/~demirbas/publications/dissecting.pdf)
- https://github.com/ailidani/paxi

### Model checking

#### DSLabs
- [Teaching Rigorous Distributed Systems With Efficient Model Checking](https://ellismichael.com/papers/dslabs-eurosys19.pdf)
- https://github.com/emichael/dslabs  

#### StateRight
- https://github.com/stateright/stateright

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
