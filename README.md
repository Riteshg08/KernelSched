# CPU Scheduler Simulation

A C++ operating system scheduling simulator that models how processes move through CPU, input, and output queues. The simulator supports **FCFS, SJF, Priority, and Round Robin (RR)** scheduling algorithms with configurable parameters.

The project simulates how an operating system scheduler manages processes, CPU bursts, I/O bursts, queues, waiting time, and resource utilization over time.

---

## Features

* Simulates CPU process scheduling
* Supports **First Come First Serve (FCFS)**
* Supports **Shortest Job First (SJF)** scheduling
* Supports **Priority Scheduling**
* Supports **Round Robin (RR)** with configurable time quantum
* Uses Entry, Ready, Input, and Output queues
* Simulates dedicated input and output devices
* Supports CPU and I/O bursts
* Supports process priorities
* Tracks CPU, input, output, and waiting time
* Displays queue states during execution
* Calculates final scheduling statistics
* Supports multiple workload input files
* Allows comparison of different scheduling algorithms

---

## Technologies Used

* C++
* STL `vector`
* STL `deque`
* File input handling
* Command-line arguments
* Console-based simulation
* Makefile

---

## Operating System Concepts Demonstrated

* CPU scheduling
* FCFS scheduling
* Shortest Job First scheduling
* Priority scheduling
* Round Robin scheduling
* Preemptive scheduling
* Non-preemptive scheduling
* Process state transitions
* CPU bursts
* Input/Output bursts
* Ready queue management
* Process priorities
* Waiting time
* CPU utilization
* Time slicing
* Resource management
* Discrete-event simulation

---

## Project Structure

```txt
cpu-scheduler-simulation/

├── README.md
├── Makefile
├── cpu-scheduler-simulation.cpp
├── screenshots/
│   ├── mixed-workload-output.png
│   ├── stress-test-output.png
│   └── statistics-summary.png
│
└── input/
    ├── input.txt
    ├── heavy-cpu.txt
    ├── heavy-io.txt
    ├── short-jobs.txt
    ├── mixed-workload.txt
    └── stress-test.txt
```

---

## How It Works

The simulator reads process information from an input file.

Each process contains:

* Process name
* Arrival time
* Priority
* CPU bursts
* Input bursts
* Output bursts

Processes move through multiple queues during execution:

**Entry Queue** → Waiting for arrival

**Ready Queue** → Waiting for CPU execution

**Input Queue** → Waiting for the input device

**Output Queue** → Waiting for the output device

The scheduler updates the simulation one clock tick at a time until all processes terminate or the maximum simulation time is reached.

---

## Scheduling Algorithms

### FCFS

**First Come First Serve** executes processes according to their arrival/order in the ready queue.

It is a **non-preemptive** scheduling algorithm.

```bash
./cpu-scheduler-simulation input/input.txt FCFS
```

---

### SJF

**Shortest Job First** selects the process with the shortest current CPU burst from the ready queue.

The implementation uses **non-preemptive SJF** scheduling.

```bash
./cpu-scheduler-simulation input/input.txt SJF
```

SJF can reduce average waiting time when short CPU bursts are available.

---

### Priority Scheduling

Priority scheduling selects the process with the highest priority from the ready queue.

In this implementation:

> **Lower priority number = Higher priority**

For example:

```txt
Priority 1 → Highest
Priority 2
Priority 3
Priority 4 → Lower
```

The implementation uses **non-preemptive Priority Scheduling**.

```bash
./cpu-scheduler-simulation input/input.txt PRIORITY
```

---

### Round Robin

Round Robin gives each process a fixed amount of CPU time called a **time quantum**.

```bash
./cpu-scheduler-simulation input/input.txt RR 5
```

Here, `5` is the time quantum.

A smaller quantum generally provides better responsiveness but can result in more context switching.

---

## Build

### Using Makefile

Compile the project using:

```bash
make
```

Clean generated object files and executable:

```bash
make clean
```

### Using g++

You can also compile directly:

```bash
g++ -Wall -Wextra -std=c++11 cpu-scheduler-simulation.cpp -o cpu-scheduler-simulation
```

---

## Input File Format

Each process starts with:

```txt
PROCESS_NAME ARRIVAL_TIME PRIORITY
```

The following values describe the process execution history:

```txt
C <time>    CPU burst
I <time>    Input burst
O <time>    Output burst
N 0         End of process
```

Example:

```txt
PROGRAMA 0 3
C 6 I 8 C 4 O 8 C 5 N 0

PROGRAMB 2 1
C 4 I 10 C 5 N 0
```

Here:

* `PROGRAMA` arrives at time `0`
* Its priority is `3`
* It first requires `6` units of CPU time
* Then `8` units of input time
* Then `4` units of CPU time
* Then `8` units of output time
* Finally, it requires another `5` units of CPU time

---

## Running the Simulator

### FCFS

```bash
./cpu-scheduler-simulation input/input.txt FCFS
```

### SJF

```bash
./cpu-scheduler-simulation input/input.txt SJF
```

### Priority Scheduling

```bash
./cpu-scheduler-simulation input/input.txt PRIORITY
```

### Round Robin

Run with the default quantum:

```bash
./cpu-scheduler-simulation input/input.txt RR 5
```

Run with a custom quantum:

```bash
./cpu-scheduler-simulation input/input.txt RR 10
```

---

## Example Workloads

### CPU-Heavy Workload

```bash
./cpu-scheduler-simulation input/heavy-cpu.txt FCFS
```

### I/O-Heavy Workload

```bash
./cpu-scheduler-simulation input/heavy-io.txt RR 5
```

### Mixed Workload

```bash
./cpu-scheduler-simulation input/mixed-workload.txt SJF
```

### Short Jobs

Useful for observing SJF behavior:

```bash
./cpu-scheduler-simulation input/short-jobs.txt SJF
```

### Stress Test

```bash
./cpu-scheduler-simulation input/stress-test.txt PRIORITY
```

---

## Example Output

```txt
--- Time 125 ---

Scheduling Algorithm: SJF

Active: 110
IActive: 116
OActive: 114

Entry Q: 123 125 127
Ready Q: 118
Input Q: 109
Output Q: [Empty]
```

The simulator periodically displays the current state of the system, including active processes and queue contents.

---

## Final Statistics

At the end of the simulation, statistics such as CPU utilization, waiting time, and resource usage are displayed.

Example:

```txt
========== Final Simulation Summary ==========

Scheduling Algorithm: PRIORITY

Final Timer: 211
Processes Terminated: 12
CPU Idle Time: 4
CPU Utilization: 98.10%

Average Waiting Time: 43.00

Total CPU Time: 207
Total Input Time: 170
Total Output Time: 117

Processes Left in Entry Queue: 0
Processes Left in Ready Queue: 0
Processes Left in Input Queue: 0
Processes Left in Output Queue: 0

=============================================
```

---

## Scheduling Algorithm Comparison

The simulator can be used to observe how different scheduling strategies affect process execution.

### FCFS

```bash
./cpu-scheduler-simulation input/mixed-workload.txt FCFS
```

### SJF

```bash
./cpu-scheduler-simulation input/mixed-workload.txt SJF
```

### Priority

```bash
./cpu-scheduler-simulation input/mixed-workload.txt PRIORITY
```

### Round Robin

```bash
./cpu-scheduler-simulation input/mixed-workload.txt RR 5
```

Different scheduling strategies can affect:

* Average waiting time
* CPU utilization
* Process responsiveness
* Queue buildup
* Process completion order
* Overall throughput

---

## What I Learned

This project improved my understanding of:

* Operating system scheduling algorithms
* FCFS, SJF, Priority, and Round Robin scheduling
* Preemptive and non-preemptive scheduling
* Queue-based process management
* CPU and I/O burst handling
* Process state transitions
* Resource utilization tracking
* Simulation-driven programming
* STL containers in C++
* Command-line program design
* File-based workload configuration

The project also provided practical experience implementing scheduling logic and managing multiple system resources in a simulation environment.

---

## Future Improvements

* Add graphical Gantt chart visualization
* Export scheduling statistics to CSV
* Add additional scheduling algorithms
* Add preemptive Priority Scheduling
* Add preemptive SJF / Shortest Remaining Time First
* Improve process state tracking
* Add context-switching overhead
* Add multi-core CPU simulation
* Add configurable I/O device counts
* Add automated performance comparison between algorithms
