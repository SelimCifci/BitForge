#ifndef BITFORGE_H
#define BITFORGE_H

#include <functional>
using namespace std;

namespace BitForge {
    void register_functions(function<void()> start_func, function<void(double)> update_func);
    
    void run_starts();
    
    void run_updates(double delta_time);
}

// Macro to easily register functions (single-line to avoid backslashes)
#define REGISTER(start_func, update_func) namespace { struct FunctionRegistrar { FunctionRegistrar() { BitForge::register_functions(start_func, update_func); } }; static FunctionRegistrar _function_registrar; }

#endif
