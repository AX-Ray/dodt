#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <iomanip>
#include <cmath>
#include "include/world.hpp"

struct OOP_Entity {
    float position_x, position_y, position_z;
    float velocity_x, velocity_y, velocity_z;
    float health;
    float armor;
    int team_id;
};

class OOP_World {
    std::vector<OOP_Entity> entities;
public:
    void add_entity(float px, float py, float pz, float vx, float vy, float vz, float h, float a, int tid) {
        entities.push_back({px, py, pz, vx, vy, vz, h, a, tid});
    }    
    
    void update_positions(float dt) {
        for (auto& e : entities) {
            e.position_x += e.velocity_x * dt;
            e.position_y += e.velocity_y * dt;
            e.position_z += e.velocity_z * dt;
        }
    }
    
    void apply_damage_multiplier(float factor) {
        for (auto& e : entities) {
            e.health *= factor;
        }
    }
    
    void update_armor(float delta) {
        for (auto& e : entities) {
            e.armor += delta;
        }
    }
    
    size_t size() const { return entities.size(); }
};

struct Position { float x, y, z; };
struct Velocity { float x, y, z; };
struct Health { float value; };
struct Armor { float value; };
struct TeamID { int id; };

using ECS_World = dod::World<Position, Velocity, Health, Armor, TeamID>;

class Timer {
    using clock = std::chrono::high_resolution_clock;
    clock::time_point start_time;
public:
    void start() { start_time = clock::now(); }
    
    double elapsed_ms() const {
        auto end_time = clock::now();
        return std::chrono::duration<double, std::milli>(end_time - start_time).count();
    }
};

double calculate_mean(const std::vector<double>& times) {
    double sum = 0.0;
    for (double t : times) sum += t;
    return sum / times.size();
}

double calculate_stddev(const std::vector<double>& times, double mean) {
    double sum = 0.0;
    for (double t : times) sum += (t - mean) * (t - mean);
    return std::sqrt(sum / times.size());
}

template<typename Func>
double benchmark_operation(Func operation, int iterations = 100) {
    std::vector<double> times;
    times.reserve(iterations);
        
    for (int i = 0; i < 5; ++i) {
        operation();
    }
    
    for (int i = 0; i < iterations; ++i) {
        Timer timer;
        timer.start();
        operation();
        times.push_back(timer.elapsed_ms());
    }
    
    double mean = calculate_mean(times);
    double stddev = calculate_stddev(times, mean);
    
    std::cout << "  Mean: " << std::fixed << std::setprecision(3) << mean << " ms";
    std::cout << " (±" << stddev << ")" << std::endl;
    
    return mean;
}

int main() {
    const size_t ENTITY_COUNT = 1'000'000;
    const int ITERATIONS = 50;        
        
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    std::uniform_int_distribution<int> team_dist(0, 3);
        
    std::cout << "1. Entity Creation:\n";
    
    double oop_create_time = benchmark_operation([&]() {
        OOP_World world;
        for (size_t i = 0; i < ENTITY_COUNT; ++i) {
            world.add_entity(
                dist(rng), dist(rng), dist(rng),
                dist(rng), dist(rng), dist(rng),
                dist(rng), dist(rng), team_dist(rng)
            );
        }
    }, ITERATIONS / 10); 
    
    double ecs_create_time = benchmark_operation([&]() {
        ECS_World world(ENTITY_COUNT);
        for (size_t i = 0; i < ENTITY_COUNT; ++i) {
            world.add_entity(
                Position{dist(rng), dist(rng), dist(rng)},
                Velocity{dist(rng), dist(rng), dist(rng)},
                Health{dist(rng)},
                Armor{dist(rng)},
                TeamID{team_dist(rng)}
            );
        }
    }, ITERATIONS / 10);
    
    std::cout << "  ECS speedup: " << std::fixed << std::setprecision(2) << (oop_create_time / ecs_create_time) << "x\n\n";
        
    OOP_World oop_world;
    ECS_World ecs_world(ENTITY_COUNT);
    
    for (size_t i = 0; i < ENTITY_COUNT; ++i) {
        float px = dist(rng), py = dist(rng), pz = dist(rng);
        float vx = dist(rng), vy = dist(rng), vz = dist(rng);
        float h = std::abs(dist(rng));
        float a = std::abs(dist(rng));
        int tid = team_dist(rng);
        
        oop_world.add_entity(px, py, pz, vx, vy, vz, h, a, tid);
        ecs_world.add_entity(
            Position{px, py, pz},
            Velocity{vx, vy, vz},
            Health{h},
            Armor{a},
            TeamID{tid}
        );
    }
        
    std::cout << "2. Position Update:\n";
    
    double oop_position_time = benchmark_operation([&]() {
        oop_world.update_positions(0.016f); 
    }, ITERATIONS);
    
    double ecs_position_time = benchmark_operation([&]() {
        auto* positions = ecs_world.data<Position>();
        auto* velocities = ecs_world.data<Velocity>();
        size_t count = ecs_world.size();
        
        float dt = 0.016f;
        for (size_t i = 0; i < count; ++i) {
            positions[i].x += velocities[i].x * dt;
            positions[i].y += velocities[i].y * dt;
            positions[i].z += velocities[i].z * dt;
        }
    }, ITERATIONS);
    
    std::cout << "  ECS speedup: " << std::fixed << std::setprecision(2) << (oop_position_time / ecs_position_time) << "x\n\n";
        
    std::cout << "3. Health Update:\n";
    
    double oop_health_time = benchmark_operation([&]() {
        oop_world.apply_damage_multiplier(0.9f);
    }, ITERATIONS);
    
    double ecs_health_time = benchmark_operation([&]() {
        auto* healths = ecs_world.data<Health>();
        size_t count = ecs_world.size();
        
        for (size_t i = 0; i < count; ++i) {
            healths[i].value *= 0.9f;
        }
    }, ITERATIONS);
    
    std::cout << "  ECS speedup: " << std::fixed << std::setprecision(2) << (oop_health_time / ecs_health_time) << "x\n\n";
        
    std::cout << "4. Armor Update:\n";
    
    double oop_armor_time = benchmark_operation([&]() {
        oop_world.update_armor(5.0f);
    }, ITERATIONS);
    
    double ecs_armor_time = benchmark_operation([&]() {
        auto* armors = ecs_world.data<Armor>();
        size_t count = ecs_world.size();
        
        for (size_t i = 0; i < count; ++i) {
            armors[i].value += 5.0f;
        }
    }, ITERATIONS);
    
    std::cout << "  ECS speedup: " << std::fixed << std::setprecision(2) << (oop_armor_time / ecs_armor_time) << "x\n\n";
        
    std::cout << "5. Combined Update:\n";
    
    double oop_combined_time = benchmark_operation([&]() {
        oop_world.apply_damage_multiplier(0.95f);
        oop_world.update_armor(-2.0f);
    }, ITERATIONS);
    
    double ecs_combined_time = benchmark_operation([&]() {
        auto* healths = ecs_world.data<Health>();
        auto* armors = ecs_world.data<Armor>();
        size_t count = ecs_world.size();
        
        for (size_t i = 0; i < count; ++i) {
            healths[i].value *= 0.95f;
            armors[i].value -= 2.0f;
        }
    }, ITERATIONS);
    
    std::cout << "  ECS speedup: " << std::fixed << std::setprecision(2) << (oop_combined_time / ecs_combined_time) << "x\n\n";        
    
    return 0;
}