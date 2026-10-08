/***********************************************************************

* Simulates operating system CPU scheduling using:
*   - First Come First Serve (FCFS)
*   - Round Robin Scheduling
*   - Priority Scheduling
*   - Shortest Job First
*
* Features:
*   - CPU, Input, and Output device simulation
*   - Multiple process queues
*   - Process statistics tracking
*   - Configurable Round Robin quantum
*
* Technologies:
*   - C++
*   - STL deque/vector
*
************************************************************************/
#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

const int MAX_TIME = 500;
const int IN_USE = 5;
const int HOW_OFTEN = 25;
const int DEFAULT_QUANTUM = 5;

struct Process {
    string process_name;
    int process_id;
    int arrival_time;
    int priority = 0;

    vector<pair<char, int>> history;
    int history_index = 0;

    int cpu_timer = 0;
    int input_timer = 0;
    int output_timer = 0;

    int total_cpu_time = 0;
    int total_input_time = 0;
    int total_output_time = 0;

    int cpu_count = 0;
    int input_count = 0;
    int output_count = 0;

    int waiting_time = 0;
};

string scheduling_algorithm = "FCFS";

deque<Process*> entry_queue;
deque<Process*> ready_queue;
deque<Process*> input_queue;
deque<Process*> output_queue;

Process* active = nullptr;
Process* input_active = nullptr;
Process* output_active = nullptr;

int sys_timer = 0;
int quantum = DEFAULT_QUANTUM;
int quantum_counter = 0;
int idle_ticks = 0;
int total_done = 0;

int total_waiting_time = 0;
int total_cpu_time = 0;
int total_input_time = 0;
int total_output_time = 0;

void read_input_file(string filename);
void run_simulation();
void check_arrivals();
void dispatch_cpu();
void handle_cpu();
void handle_input();
void handle_output();
void update_waiting_time();
void print_state(int t);
void terminate_process(Process* p);

void read_input_file(string filename) {
    ifstream infile(filename);

    if (!infile.is_open()) {
        cout << "Failed to open " << filename << " file!" << endl;
        exit(1);
    }

    string name;
    int arrival;
    int next_pid = 101;

    while (infile >> name && name != "STOPHERE") {

        if (!(infile >> arrival))
            break;

        int priority;

        if (!(infile >> priority))
            break;

        Process* p = new Process();

        p->process_name = name;
        p->arrival_time = arrival;
        p->priority = priority;
        p->process_id = next_pid++;
        p->history_index = 0;

        char type;
        int val;

        while (infile >> type && type != 'N') {
            infile >> val;
            p->history.push_back(make_pair(type, val));
        }

        infile >> val;

        if (!p->history.empty()) {
            entry_queue.push_back(p);
        } else {
            delete p;
        }
    }

    infile.close();
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        cout << "Usage: ./cpu-scheduler-simulation <file> <algorithm> [quantum]" << endl;
        cout << "Algorithms: FCFS, SJF, PRIORITY, RR" << endl;
        return 1;
    }

    scheduling_algorithm = argv[2];

    if (scheduling_algorithm != "FCFS" &&
        scheduling_algorithm != "SJF" &&
        scheduling_algorithm != "PRIORITY" &&
        scheduling_algorithm != "RR") {

        cout << "Invalid scheduling algorithm!" << endl;
        cout << "Use: FCFS, SJF, PRIORITY, or RR" << endl;
        return 1;
    }

    if (scheduling_algorithm == "RR" && argc == 4) {
        quantum = stoi(argv[3]);
    }

    read_input_file(argv[1]);
    run_simulation();

    cout << "\n========== Final Simulation Summary ==========" << endl;
    cout << "Scheduling Algorithm: " << scheduling_algorithm << endl;
    cout << "Final Timer: " << sys_timer << endl;
    cout << "Processes Terminated: " << total_done << endl;
    cout << "CPU Idle Time: " << idle_ticks << endl;

    if (sys_timer > 0) {
        double cpu_utilization =
            ((double)(sys_timer - idle_ticks) / sys_timer) * 100.0;

        cout << fixed << setprecision(2);
        cout << "CPU Utilization: " << cpu_utilization << "%" << endl;
    }

    if (total_done > 0) {
        double average_waiting_time =
            (double)total_waiting_time / total_done;

        cout << "Average Waiting Time: "
             << average_waiting_time << endl;
    }

    cout << "Total CPU Time: " << total_cpu_time << endl;
    cout << "Total Input Time: " << total_input_time << endl;
    cout << "Total Output Time: " << total_output_time << endl;

    cout << "Processes Left in Entry Queue: "
         << entry_queue.size() << endl;

    cout << "Processes Left in Ready Queue: "
         << ready_queue.size() << endl;

    cout << "Processes Left in Input Queue: "
         << input_queue.size() << endl;

    cout << "Processes Left in Output Queue: "
         << output_queue.size() << endl;

    cout << "===============================================" << endl;

    return 0;
}

void run_simulation() {

    while (sys_timer < MAX_TIME) {

        check_arrivals();

        handle_cpu();
        handle_input();
        handle_output();

        dispatch_cpu();

        update_waiting_time();

        if (sys_timer % HOW_OFTEN == 0) {
            print_state(sys_timer);
        }

        sys_timer++;

        if (entry_queue.empty() &&
            ready_queue.empty() &&
            input_queue.empty() &&
            output_queue.empty() &&
            !active &&
            !input_active &&
            !output_active) {

            break;
        }
    }
}

void check_arrivals() {

    int cur_in_sys =
        (int)ready_queue.size() +
        (int)input_queue.size() +
        (int)output_queue.size();

    if (active)
        cur_in_sys++;

    if (input_active)
        cur_in_sys++;

    if (output_active)
        cur_in_sys++;

    while (!entry_queue.empty() &&
           entry_queue.front()->arrival_time <= sys_timer &&
           cur_in_sys < IN_USE) {

        Process* p = entry_queue.front();

        entry_queue.pop_front();
        ready_queue.push_back(p);

        cur_in_sys++;

        cout << "Time " << sys_timer << ": "
             << p->process_name
             << " moved to Ready" << endl;
    }
}

void dispatch_cpu() {

    if (active == nullptr && !ready_queue.empty()) {

        int selected_index = 0;

        // SJF selects the process with the shortest current CPU burst.
        if (scheduling_algorithm == "SJF") {

            for (int i = 1; i < (int)ready_queue.size(); i++) {

                int current_burst =
                    ready_queue[i]->history[
                        ready_queue[i]->history_index
                    ].second;

                int selected_burst =
                    ready_queue[selected_index]->history[
                        ready_queue[selected_index]->history_index
                    ].second;

                if (current_burst < selected_burst) {
                    selected_index = i;
                }
            }
        }

        // Lower priority number represents higher priority.
        else if (scheduling_algorithm == "PRIORITY") {

            for (int i = 1; i < (int)ready_queue.size(); i++) {

                if (ready_queue[i]->priority <
                    ready_queue[selected_index]->priority) {

                    selected_index = i;
                }
            }
        }

        Process* next_p = ready_queue[selected_index];

        ready_queue.erase(
            ready_queue.begin() + selected_index
        );

        if (next_p->history.empty() ||
            next_p->history_index >= (int)next_p->history.size()) {

            delete next_p;
            return;
        }

        active = next_p;

        active->cpu_timer =
            active->history[active->history_index].second;

        quantum_counter = 0;
    }
}

void handle_cpu() {

    if (active != nullptr) {

        active->cpu_timer--;

        active->total_cpu_time++;

        quantum_counter++;

        if (active->cpu_timer == 0) {

            active->cpu_count++;
            active->history_index++;

            if (active->history_index >=
                (int)active->history.size()) {

                terminate_process(active);

                total_done++;

                active = nullptr;

            } else {

                char burst_type =
                    active->history[active->history_index].first;

                if (burst_type == 'I') {
                    input_queue.push_back(active);
                }
                else if (burst_type == 'O') {
                    output_queue.push_back(active);
                }

                active = nullptr;
            }

        } else if (scheduling_algorithm == "RR" &&
                   quantum_counter == quantum) {

            active->history[
                active->history_index
            ].second = active->cpu_timer;

            ready_queue.push_back(active);

            active = nullptr;
        }

    } else {

        idle_ticks++;
    }
}

void handle_input() {

    if (input_active == nullptr &&
        !input_queue.empty()) {

        input_active = input_queue.front();

        input_queue.pop_front();

        input_active->input_timer =
            input_active->history[
                input_active->history_index
            ].second;
    }

    if (input_active) {

        input_active->input_timer--;

        input_active->total_input_time++;

        if (input_active->input_timer == 0) {

            input_active->input_count++;
            input_active->history_index++;

            ready_queue.push_back(input_active);

            input_active = nullptr;
        }
    }
}

void handle_output() {

    if (output_active == nullptr &&
        !output_queue.empty()) {

        output_active = output_queue.front();

        output_queue.pop_front();

        output_active->output_timer =
            output_active->history[
                output_active->history_index
            ].second;
    }

    if (output_active) {

        output_active->output_timer--;

        output_active->total_output_time++;

        if (output_active->output_timer == 0) {

            output_active->output_count++;
            output_active->history_index++;

            ready_queue.push_back(output_active);

            output_active = nullptr;
        }
    }
}

void update_waiting_time() {

    for (int i = 0; i < (int)ready_queue.size(); i++) {
        ready_queue[i]->waiting_time++;
    }

    for (int i = 0; i < (int)input_queue.size(); i++) {
        input_queue[i]->waiting_time++;
    }

    for (int i = 0; i < (int)output_queue.size(); i++) {
        output_queue[i]->waiting_time++;
    }
}

void print_state(int t) {

    cout << "\n--- Time " << t << " ---" << endl;

    cout << "Algorithm: "
         << scheduling_algorithm << endl;

    cout << "Active: "
         << (active ? to_string(active->process_id) : "None")
         << endl;

    cout << "IActive: "
         << (input_active ? to_string(input_active->process_id) : "None")
         << endl;

    cout << "OActive: "
         << (output_active ? to_string(output_active->process_id) : "None")
         << endl;

    cout << "Entry Queue: ";

    if (entry_queue.empty())
        cout << "Empty";

    for (size_t i = 0; i < entry_queue.size(); i++) {
        cout << entry_queue[i]->process_id << " ";
    }

    cout << endl;

    cout << "Ready Queue: ";

    if (ready_queue.empty())
        cout << "[Empty]";

    for (size_t i = 0; i < ready_queue.size(); i++) {
        cout << ready_queue[i]->process_id << " ";
    }

    cout << endl;

    cout << "Input Queue: ";

    if (input_queue.empty())
        cout << "[Empty]";

    for (size_t i = 0; i < input_queue.size(); i++) {
        cout << input_queue[i]->process_id << " ";
    }

    cout << endl;

    cout << "Output Queue: ";

    if (output_queue.empty())
        cout << "[Empty]";

    for (size_t i = 0; i < output_queue.size(); i++) {
        cout << output_queue[i]->process_id << " ";
    }

    cout << endl;
}

void terminate_process(Process* p) {

    cout << "Time " << sys_timer << ": "
         << p->process_name
         << " terminated" << endl;

    cout << "  - Process ID: "
         << p->process_id << endl;

    cout << "  - Priority: "
         << p->priority << endl;

    cout << "  - CPU Bursts: "
         << p->cpu_count << endl;

    cout << "  - Input Bursts: "
         << p->input_count << endl;

    cout << "  - Output Bursts: "
         << p->output_count << endl;

    cout << "  - Time in CPU: "
         << p->total_cpu_time << endl;

    cout << "  - Time in Input: "
         << p->total_input_time << endl;

    cout << "  - Time in Output: "
         << p->total_output_time << endl;

    cout << "  - Time Waiting: "
         << p->waiting_time << endl;

    cout << endl;

    total_waiting_time += p->waiting_time;
    total_cpu_time += p->total_cpu_time;
    total_input_time += p->total_input_time;
    total_output_time += p->total_output_time;

    delete p;
}