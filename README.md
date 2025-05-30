# Parallel PageRank Algorithm

This project implements and compares high-performance parallelized versions of Google's PageRank algorithm using two different parallel computing models:
- **OpenMP + SIMD** (shared-memory, C++)
- **Apache Spark** (distributed-memory, Python)

The aim was to optimize runtime and scalability for large synthetic graph datasets.

## Features & Performance
- Parallelized PageRank computation using OpenMP directives and explicit SIMD vectorization (256-bit instructions).
- Distributed implementation with Apache Spark leveraging RDD transformations and actions.
- **13x speedup** (0.12s vs. 1.57s serial) achieved on graphs with 10M edges using OpenMP.
- Demonstrated significant performance advantage with shared-memory parallelism compared to Spark's distributed model for graphs with high node counts.

## Technologies
- **Languages:** C++, Python
- **Parallel Libraries/Frameworks:** OpenMP, Apache Spark
- **Optimizations:** SIMD, Thread Affinity (taskset)

## Performance Results

| Graph Size (Edges/Nodes) | Method          | Runtime (s) |
|--------------------------|-----------------|-------------|
| 1M edges / 10K nodes     | Serial          | 0.19        |
|                          | OpenMP (16 cores)| 0.016       |
|                          | Spark (Cluster) | 45.93       |
| 10M edges / 10K nodes    | Serial          | 1.57        |
|                          | OpenMP (16 cores)| 0.12        |
|                          | Spark (Cluster) | 192.29      |
