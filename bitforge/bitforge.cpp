#include <bitforge.hpp>
using namespace std;

namespace BitForge {
    namespace {
        vector<function<void()>> start_functions;
        vector<function<void(double)>> update_functions;
    }
    
    void register_functions(function<void()> start_func, function<void(double)> update_func) {
        start_functions.push_back(start_func);
        update_functions.push_back(update_func);
    }
    
    void run_starts() {
        for (auto& start_func : start_functions) {
            start_func();
        }
    }
    
    void run_updates(double delta_time) {
        for (auto& update_func : update_functions) {
            update_func(delta_time);
        }
    }
}