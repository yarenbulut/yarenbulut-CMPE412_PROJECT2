#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <iomanip>

using namespace std;

// Constants for production parameters
const int PROCESSING_TIMES[] = {7, 12, 9, 7, 5}; // Loading, Machining, Assembling, Inspecting, Packaging (in minutes)
const int MAINTENANCE_TIME = 4; // Base repair time (in minutes)
const double BREAKDOWN_PROB = 0.15; // Probability of a breakdown during a stage
const int WORKER_COUNTS[] = {3, 4, 5, 3, 4}; // Number of workers for each stage
const int SHIFT_LENGTH = 8 * 60; // Shift length in minutes (8 hours)
const int SIMULATION_DURATION = 100; // Simulation duration in minutes
const int NUM_PRODUCT_TYPES = 2; // Number of product types
const int delay_multiplier = 10;


class ProductionLine {
public:
    ProductionLine() : current_time(0) {
        // Initialize worker counts
        for (int i = 0; i < 5; ++i) {
            workers[i] = WORKER_COUNTS[i];
        }
    }

    void process_part(int part_id, int product_type) {
        vector<string> stages = {"Loading", "Machining", "Assembling", "Inspecting", "Packaging"};

        // Simulate each stage of production
        for (int i = 0; i < 5; ++i) {
            double duration = PROCESSING_TIMES[i] * (product_type == 1 ? 1.0 : 1.2); // Adjust duration for product type
            double start_time = current_time;

            // Wait for a worker to become available
            while (workers[i] == 0) {
                std::this_thread::sleep_for(chrono::seconds(1));
                current_time++;
            }
            workers[i]--; // Decrement worker count

            // Simulate processing and potential breakdown
            std::this_thread::sleep_for(chrono::milliseconds(static_cast<int>(duration * delay_multiplier))); // Convert minutes to milliseconds
            if (random_breakdown()) {
                // Simulate repair time with a random duration within a range
                int repair_duration = rand() % (MAINTENANCE_TIME * 2) + MAINTENANCE_TIME; // Example: 2-6 minutes
                std::this_thread::sleep_for(chrono::milliseconds(repair_duration * delay_multiplier));
            }
            current_time += duration;
            workers[i]++; // Increment worker count

            // Log production data
            LogEntry entry;
            entry.part_id = part_id;
            entry.stage = stages[i];
            entry.start_time = start_time;
            entry.finish_time = current_time;
            entry.duration = duration;
            entry.product_type = product_type;
            log_data.push_back(entry);
        }
    }

    void print_log_data() {
        // Print table header
        cout << left << setw(10) << "Part"
             << left << setw(12) << "Stage"
             << left << setw(15) << "Start Time"
             << left << setw(15) << "Finish Time"
             << left << setw(10) << "Duration"
             << left << setw(15) << "Product Type"
             << endl;

        // Print each log entry
        for (const LogEntry& entry : log_data) {
            cout << left << setw(10) << entry.part_id
                 << left << setw(12) << entry.stage
                 << left << setw(15) << entry.start_time
                 << left << setw(15) << entry.finish_time
                 << left << setw(10) << entry.duration
                 << left << setw(15) << entry.product_type
                 << endl;
        }
    }

private:
    struct LogEntry { // Structure to store production data
        int part_id;
        string stage;
        double start_time;
        double finish_time;
        double duration;
        int product_type;
    };

    vector<LogEntry> log_data;
    int workers[5];
    double current_time;

    bool random_breakdown() {
        static random_device rd;
        static mt19937 gen(rd());
        static uniform_real_distribution<> dis(0.0, 1.0);
        return dis(gen) < BREAKDOWN_PROB;
    }
};

void create_part(ProductionLine& line, int part_id, int product_type) {
    line.process_part(part_id, product_type);
}

void setup(int num_parts_per_type) {
    ProductionLine line;
    for (int product_type = 1; product_type <= NUM_PRODUCT_TYPES; ++product_type) {
        for (int i = 0; i < num_parts_per_type; ++i) {
            create_part(line, i + 1, product_type);
        }
    }
    line.print_log_data();
}

int main() {
    setup(5); // Simulate one setup with 5 parts
}