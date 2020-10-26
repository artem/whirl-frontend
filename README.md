# Whirl 

## Example

[KV](/examples/kv/main.cpp) - simple replicated KV store

- Quorum reads/writes
- Last-write-wins via wall clock
- Persistent local storage

## Inspiration

- [Testing Distributed Systems w/ Deterministic Simulation](https://www.youtube.com/watch?v=4fFDFbi3toc)
- [Paxi - Paxos protocol framework](https://github.com/ailidani/paxi)
- [Teaching Rigorous Distributed Systems With Efficient Model Checking](https://ellismichael.com/papers/dslabs-eurosys19.pdf)

## Requirements

- x86-64
- Clang++ (>= 8)

## Dependencies

- [await](https://gitlab.com/Lipovsky/await) - Concurrency: fibers and futures with customizable runtime
- [cereal](https://github.com/USCiLab/cereal) - Serialization
- [fmt](https://github.com/fmtlib/fmt)

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