# boojum

A ZK proof aggregation library built on top of [libsnark](https://github.com/AlexandreBelling/libsnark). The name is a nod to Lewis Carroll's poem *The Hunting of the Snark*, where a Boojum is the most dangerous species of Snark — a recursive pun on SNARK (Succinct Non-interactive ARgument of Knowledge).

## Overview

boojum aggregates ppzkSNARK proofs recursively using a **cycle of elliptic curves** (MNT4/MNT6). The key insight is that the MNT4 and MNT6 curves form a 2-cycle: the scalar field of one is the base field of the other, which makes it possible to verify a proof over MNT4 inside a circuit defined over MNT6, and vice versa.

This allows building a binary **aggregation tree**: two leaf proofs are combined into a single aggregated proof at the next level, two of those are combined again, and so on. The result is a single proof attesting to the validity of an entire tree of proofs.

### Architecture

```
src/
  circuits/         -- aggregator_circuit: verifies N ppzkSNARK proofs inside a SNARK
  cycles/           -- curve cycle abstraction and aggregation system
  trees/            -- tree/node data structures for recursive aggregation
  materials/        -- serialization helpers
  libboojum.h       -- C API header
  libboojum.cpp     -- C API implementation
depends/
  libsnark/         -- libsnark submodule (R1CS ppzkSNARK, gadgetlib)
```

The core components are:

- **`aggregator_circuit`** — a libsnark gadget that takes `N` (key, input, proof) tuples for curve `from` and produces a proof over curve `to`, asserting all `N` proofs are valid.
- **`basic_aggregation_system`** — wires together the curve cycle and the aggregation circuits into a unified interface.
- **`tree<aggregation_system, N>`** — the recursive proof tree. Leaf nodes hold raw ppzkSNARK proofs; internal nodes hold aggregated proofs. Alternates between the two curves at each level (`N % cycle::size`).

## Dependencies

- CMake >= 2.8
- C++11 compiler (GCC or Clang)
- [libsnark](https://github.com/AlexandreBelling/libsnark) (included as a git submodule)
- `libprocps` (optional, for memory profiling)
- OpenMP (optional, for multicore support)

## Building

Clone the repository with its submodule:

```sh
git clone --recursive <repo-url>
cd boojum
```

Then build the `libboojum` shared library:

```sh
make
```

This runs `cmake` inside a `build/` directory and compiles `libboojum`.

To rebuild from scratch:

```sh
make clean-build && make
```

### CMake options

| Option | Default | Description |
|---|---|---|
| `CURVE` | `ALT_BN128` | Elliptic curve for libsnark internals |
| `MULTICORE` | `OFF` | Enable OpenMP parallelism |
| `WITH_PROCPS` | `ON` | Use libprocps for memory profiling |
| `VERBOSE` | `OFF` | Print internal libsnark messages |

Example:

```sh
cd build && cmake .. -DMULTICORE=ON -DWITH_PROCPS=OFF && make libboojum
```

## C API

The library exposes a C-compatible interface defined in [src/libboojum.h](src/libboojum.h):

```c
// Initialize elliptic curves and build aggregation circuits.
// Must be called first.
void initialize();

// Generate or load proving/verification keys (CRS) from the given directory.
// initialize() must be called before this.
void run_generators(const char* dir);

// Generate an example leaf proof (for testing).
void make_example_proof(void** tree_buffer);

// Aggregate two proof trees (left, right) into a single output proof tree.
// Both inputs must have the same height and arity.
void prove_aggregation(
    void*  left_node_buff,
    void*  right_node_buff,
    void** output_node_buff
);

// Verify a proof tree.
bool verify(void* node_buff);
```

### Typical usage

```c
initialize();
run_generators("./setup");

void* leaf = NULL;
make_example_proof(&leaf);

void* left  = NULL;
void* right = NULL;
make_example_proof(&left);
make_example_proof(&right);

void* aggregated = NULL;
prove_aggregation(left, right, &aggregated);

bool ok = verify(aggregated);
```

## Limitations

- Only **arity 2** (binary) aggregation trees are supported.
- Only **two levels** of the MNT4/MNT6 cycle are implemented (`height` 0 and 1).
- The Go API referenced in early commits is not included in this repository.

## License

See [LICENSE](LICENSE).
