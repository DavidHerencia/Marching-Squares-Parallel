import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os

# Create output directory if it doesn't exist
os.makedirs('results/graphs', exist_ok=True)

# Set matplotlib style for better looking plots
plt.style.use('seaborn-v0_8')

# Load all benchmark data
print("Loading benchmark data...")
required_columns = ['FUNCTION', 'PROCESSORS', 'TIME', 'GRIDSIZE']

# Load CPU data (x86 with 48 processors - best configuration)
df_x86 = pd.read_csv('results/benchmark_it3_x86.csv')[required_columns]
cpu_best = df_x86[df_x86['PROCESSORS'] == 48]

# Load CUDA data from all GPUs
df_cuda_a100 = pd.read_csv('results/benchmark_it4_cuda_a100.csv')[required_columns]
df_cuda_a6000 = pd.read_csv('results/benchmark_it4_cuda_a6000.csv')[required_columns]
df_cuda_t4 = pd.read_csv('results/benchmark_it4_cuda_t4.csv')[required_columns]

# Add GPU type identifier
cpu_best = cpu_best.copy()
cpu_best['GPU_TYPE'] = 'CPU x86 (48p)'

df_cuda_a100 = df_cuda_a100.copy()
df_cuda_a100['GPU_TYPE'] = 'NVIDIA A100'

df_cuda_a6000 = df_cuda_a6000.copy()
df_cuda_a6000['GPU_TYPE'] = 'NVIDIA A6000'

df_cuda_t4 = df_cuda_t4.copy()
df_cuda_t4['GPU_TYPE'] = 'NVIDIA T4'

# Combine all data
all_data = pd.concat([cpu_best, df_cuda_a100, df_cuda_a6000, df_cuda_t4], ignore_index=True)

# Since T4 is not available for higher gridsizes due to memory limitations, 
# we'll use all available grid sizes and let T4 show only what it can handle
GRID_SIZES = sorted(cpu_best['GRIDSIZE'].unique())
print(f"Available grid sizes: {GRID_SIZES}")


# Identify the worst performing functions based on CPU times
avg_times_cpu = cpu_best.groupby('FUNCTION')['TIME'].mean().sort_values(ascending=False)
worst_functions = avg_times_cpu.head(2).index.tolist()

print(f"Worst performing functions (slowest on CPU): {worst_functions}")
print(f"Benchmarks included: CPU x86 (48p), NVIDIA A100, A6000, T4")

# Set up color mapping for consistent styling
gpu_types = all_data['GPU_TYPE'].unique()
color_map = matplotlib.colormaps['tab10']
color_dict = {gpu: color_map(i / len(gpu_types)) for i, gpu in enumerate(gpu_types)}

# Define markers for each GPU type
marker_dict = {
    'CPU x86 (48p)': 'o',
    'NVIDIA A100': 's', 
    'NVIDIA A6000': '^',
    'NVIDIA T4': 'D'
}

# Generate individual graphs for each worst performing function
for function in worst_functions:
    plt.figure(figsize=(14, 10))
    
    # Filter data for the specific function
    func_data = all_data[all_data['FUNCTION'] == function]
    
    # Plot each GPU type
    for gpu_type in gpu_types:
        gpu_data = func_data[func_data['GPU_TYPE'] == gpu_type].sort_values('GRIDSIZE')
        
        if len(gpu_data) > 0:
            plt.plot(gpu_data['GRIDSIZE'], gpu_data['TIME'], 
                    marker=marker_dict[gpu_type], linewidth=3, markersize=10, 
                    color=color_dict[gpu_type], label=gpu_type, 
                    linestyle='-' if 'CPU' in gpu_type else '--',
                    alpha=0.8)
    
    # Chart configuration with improved styling
    plt.xlabel('Grid Size (N × N)', fontsize=14, fontweight='bold')
    plt.ylabel('Execution Time (seconds)', fontsize=14, fontweight='bold')
    plt.title(f'CPU vs CUDA GPUs - Performance Comparison: {function}', 
              fontsize=16, fontweight='bold', pad=20)
    
    # Use linear scale for X-axis to show exact grid sizes clearly
    plt.xscale('linear')
    plt.yscale('log')  # Keep Y-axis logarithmic for better time visualization
    
    # Set specific X-axis ticks to show all grid sizes clearly
    all_gridsizes = sorted(func_data['GRIDSIZE'].unique())
    plt.xticks(all_gridsizes, [f'{int(size)}' for size in all_gridsizes], rotation=45)
    
    # Enhanced grid styling - cleaner appearance
    plt.grid(True, alpha=0.5, linestyle='-', linewidth=0.8, color='gray')
    plt.legend(bbox_to_anchor=(1.02, 1), loc='upper left', fontsize=12, 
              frameon=True, fancybox=True, shadow=True)
    
    # Adjust layout and save
    plt.tight_layout(pad=3.0)
    
    # Clean filename
    safe_function_name = function.replace(' ', '_').replace('(', '').replace(')', '').replace(',', '')
    filename = f'results/graphs/cpu_vs_all_gpus_{safe_function_name}.png'
    plt.savefig(filename, dpi=300, bbox_inches='tight')
    plt.close()
    
    print(f"Generated: {filename}")

# Create combined comparison graph with subplots
fig, axes = plt.subplots(1, 2, figsize=(24, 10))

for i, function in enumerate(worst_functions):
    ax = axes[i]
    
    # Filter data for the specific function
    func_data = all_data[all_data['FUNCTION'] == function]
    
    # Plot each GPU type in the subplot
    for gpu_type in gpu_types:
        gpu_data = func_data[func_data['GPU_TYPE'] == gpu_type].sort_values('GRIDSIZE')
        
        if len(gpu_data) > 0:
            ax.plot(gpu_data['GRIDSIZE'], gpu_data['TIME'], 
                   marker=marker_dict[gpu_type], linewidth=3, markersize=8, 
                   color=color_dict[gpu_type], label=gpu_type, 
                   linestyle='-' if 'CPU' in gpu_type else '--',
                   alpha=0.8)
    
    # Subplot configuration
    ax.set_xlabel('Grid Size (N × N)', fontsize=12, fontweight='bold')
    ax.set_ylabel('Execution Time (seconds)', fontsize=12, fontweight='bold')
    ax.set_title(f'{function}', fontsize=14, fontweight='bold')
    
    # Use linear scale for X-axis, logarithmic for Y-axis
    ax.set_xscale('linear')
    ax.set_yscale('log')
    
    # Set specific X-axis ticks for this subplot
    func_gridsizes = sorted(func_data['GRIDSIZE'].unique())
    ax.set_xticks(func_gridsizes)
    ax.set_xticklabels([f'{int(size)}' for size in func_gridsizes], rotation=45)
    
    # Enhanced grid styling for subplots
    ax.grid(True, alpha=0.5, linestyle='-', linewidth=0.8, color='gray')
    ax.legend(fontsize=10, frameon=True, fancybox=True, shadow=True)

# Overall title and styling
plt.suptitle('CPU vs All CUDA GPUs Performance Comparison - Worst Performing Functions', 
             fontsize=18, fontweight='bold', y=0.98)
plt.tight_layout(pad=3.0)
plt.savefig('results/graphs/cpu_vs_all_gpus_comparison_worst_functions.png', 
            dpi=300, bbox_inches='tight')
plt.close()

print("\n" + "="*60)
print("PERFORMANCE COMPARISON SUMMARY")
print("="*60)
print(f"✅ Generated individual graphs for {len(worst_functions)} worst performing functions")
print(f"✅ Generated combined comparison graph")
print(f"✅ All graphs saved to results/graphs/")
print(f"✅ Benchmarks included:")
print(f"   • CPU x86 (48 threads - best configuration)")
print(f"   • NVIDIA A100 GPU")
print(f"   • NVIDIA A6000 GPU") 
print(f"   • NVIDIA T4 GPU (limited grid sizes due to memory constraints)")
print(f"✅ Functions analyzed: {', '.join(worst_functions)}")
print(f"✅ X-axis: Linear scale showing exact grid sizes")
print(f"✅ Y-axis: Logarithmic scale for better time comparison")
print(f"📝 Note: T4 may not appear on larger grid sizes due to GPU memory limitations")
print("="*60)