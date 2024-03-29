# Whirl

Framework for implementing & testing tiny distributed systems

## Frontend

* Concurrency (via `await`)
* RPC (via `commute`)
* Filesystem
* Persistent KV storage
* Wall / monotonic time
* Randomness
* Logging (via `timber` and `fmtlib`)

## Engines

| Engine | Status | Description |
| --- | --- | --- |
| [`matrix`](https://gitlab.com/whirl-framework/whirl-matrix) | ✓ | Deterministic simulator |
| `process` | ✗ | Standalone node process |

## Inspiration

### Simulation + Fault Injection

#### FoundationDB
- [Testing Distributed Systems w/ Deterministic Simulation](https://www.youtube.com/watch?v=4fFDFbi3toc)
- [FoundationDB or: How I Learned to Stop Worrying and Trust the Database](https://www.youtube.com/watch?v=OJb8A6h9jQQ&list=PLSE8ODhjZXjagqlf1NxuBQwaMkrHXi-iz&index=22)

#### AWS

- [Millions of Tiny Databases](https://www.usenix.org/system/files/nsdi20-paper-brooker.pdf)

#### Jepsen
- https://github.com/jepsen-io/maelstrom

#### Paxi
- [Dissecting the Performance of Strongly-Consistent Replication Protocols](https://cse.buffalo.edu/~demirbas/publications/dissecting.pdf)
- https://github.com/ailidani/paxi

### Model checking

#### Microsoft Coyote

- https://microsoft.github.io/coyote
- https://github.com/microsoft/coyote

#### DSLabs
- [Teaching Rigorous Distributed Systems With Efficient Model Checking](https://ellismichael.com/papers/dslabs-eurosys19.pdf)
- https://github.com/emichael/dslabs  

#### StateRight
- https://github.com/stateright/stateright

### Fuzzing

- [Fuzzing Raft for Fun and Publication](https://colin-scott.github.io/blog/2015/10/07/fuzzing-raft-for-fun-and-profit/)
- [Minimizing Faulty Executions of Distributed Systems](https://www.usenix.org/conference/nsdi16/technical-sessions/presentation/scott)

## Requirements

- x86-64
- Clang++ (>= 8)

## Dependencies

- [Await](https://gitlab.com/Lipovsky/await) – concurrency: fibers + futures with customizable runtime
- [Commute](https://gitlab.com/whirl-framework/commute) – net transport + RPC
- [Timber](https://gitlab.com/whirl-framework/timber) – logging frontend (via [fmtlib](https://github.com/fmtlib/fmt))
- [Muesli](https://gitlab.com/whirl-framework/muesli) – serialization (via [cereal](https://github.com/USCiLab/cereal))
- [Wheels](https://gitlab.com/Lipovsky/wheels) - core utilities

## Build

### Whirl-Matrix

```shell
# Clone repo with matrix engine
git clone https://gitlab.com/whirl-framework/whirl-matrix.git
cd whirl-matrix
# Generate build files
mkdir build && cd build
cmake -DWHIRL_MATRIX_EXAMPLES=ON ..
# Build kv example
make whirl_example_kv
# Run kv example
# --det - run determinism check
# --sims - number of simulations to run
./examples/kv/whirl_example_kv --det --sims 12345
