#include <benchmark/benchmark.h>
#include <unordered_map>
#include <vector>
#include <memory>
#include <random>
#include <algorithm>

struct TestData 
{
    int id;
    double value;
    std::string description;
    
    TestData(int i, double v, const std::string& desc) 
        : id(i), value(v), description(desc) {}
};

class CacheBenchmarkFixture : public benchmark::Fixture 
{
public:
    void SetUp(const ::benchmark::State& state) override {
        size_t num_elements = state.range(0);
        
        // Generate test data
        std::mt19937 gen(42); // Fixed seed for reproducibility
        std::uniform_int_distribution<int> key_dist(1, num_elements / 4); // Multiple entries per key
        std::uniform_real_distribution<double> value_dist(0.0, 1000.0);
        
        // Clear previous data
        multimap_data.clear();
        vector_data.clear();
        search_keys.clear();
        
        // Populate containers with heap-allocated data
        for (size_t i = 0; i < num_elements; ++i) 
        {
            int key = key_dist(gen);
            double val = value_dist(gen);
            std::string desc = "Item_" + std::to_string(i);
            
            auto data = std::make_shared<TestData>(static_cast<int>(i), val, desc);
            
            multimap_data.emplace(key, data);
            vector_data.emplace_back(key, data);
            
            if (i % 10 == 0) 
            { 
                // Store some keys for searching
                search_keys.push_back(key);
            }
        }
        
        // Shuffle search keys for realistic access patterns
        std::shuffle(search_keys.begin(), search_keys.end(), gen);
    }
    
    void TearDown(const ::benchmark::State& state) override 
    {
        multimap_data.clear();
        vector_data.clear();
        search_keys.clear();
    }

protected:
    std::unordered_multimap<int, std::shared_ptr<TestData>> multimap_data;
    std::vector<std::pair<int, std::shared_ptr<TestData>>> vector_data;
    std::vector<int> search_keys;
};

BENCHMARK_DEFINE_F(CacheBenchmarkFixture, UnorderedMultimapSearch)(benchmark::State& state) 
{
    size_t key_index = 0;
    size_t total_found = 0;
    
    for (auto _ : state) 
    {
        int search_key = search_keys[key_index % search_keys.size()];
        auto range = multimap_data.equal_range(search_key);

        for (auto it = range.first; it != range.second; ++it) 
        {
            benchmark::DoNotOptimize(it->second->value);
            ++total_found;
        }
        
        ++key_index;
    }
    
    state.counters["ElementsFound"] = benchmark::Counter(total_found);
    state.counters["ElementsPerOp"] = benchmark::Counter(static_cast<double>(total_found) / state.iterations());
}

BENCHMARK_DEFINE_F(CacheBenchmarkFixture, VectorLinearSearch)(benchmark::State& state) 
{
    size_t key_index = 0;
    size_t total_found = 0;
    
    for (auto _ : state) 
    {
        int search_key = search_keys[key_index % search_keys.size()];
        
        for (const auto& pair : vector_data) 
        {
            if (pair.first == search_key) 
            {
                benchmark::DoNotOptimize(pair.second->value);
                ++total_found;
            }
        }
        
        ++key_index;
    }
    
    state.counters["ElementsFound"] = benchmark::Counter(total_found);
    state.counters["ElementsPerOp"] = benchmark::Counter(static_cast<double>(total_found) / state.iterations());
}

// Register benchmarks with different data sizes
BENCHMARK_REGISTER_F(CacheBenchmarkFixture, UnorderedMultimapSearch)
->Range(5, 15)
->Range(100, 1000)
->Unit(benchmark::kMicrosecond);

BENCHMARK_REGISTER_F(CacheBenchmarkFixture, VectorLinearSearch)
->Range(5, 15)
->Range(100, 1000)
->Unit(benchmark::kMicrosecond);