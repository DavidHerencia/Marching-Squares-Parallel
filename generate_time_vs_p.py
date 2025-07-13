import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
import os

SAVE_DIR = 'results/graphs/'

os.makedirs(SAVE_DIR, exist_ok=True)
plt.style.use('seaborn-v0_8')

# Ensure expected columns exist
required_columns = ['FUNCTION', 'PROCESSORS', 'TIME', 'GRIDSIZE', 'FLOPS']
df_x86 = pd.read_csv('./results/benchmark_it3_x86.csv')[required_columns]

# Filter out rows where GRIDSIZE is not in [16384, 8192]
#df_x86 = df_x86[df_x86['GRIDSIZE'].isin([16384, 8192])]

function_list = sorted(df_x86['FUNCTION'].unique())
color_map = matplotlib.colormaps['tab10']
color_dict = {func: color_map(i / len(function_list)) for i, func in enumerate(function_list)}

# Group by grid size and plot all functions in the same graph
for grid, grid_group in df_x86.groupby('GRIDSIZE'):
    plt.figure(figsize=(12, 8))
    
    for func, func_group in grid_group.groupby('FUNCTION'):
        plt.plot(func_group['PROCESSORS'], func_group['TIME'], 
                marker='o', label=f'{func}', linewidth=2, markersize=6,
                color=color_dict[func])
    
    processors_range = grid_group['PROCESSORS'].unique()
    grid_size = grid_group['GRIDSIZE'].values[0]
    
    # Theoretical time based on parallel efficiency model
    # Model: T(p) = T(1) * (1 + alpha * (p-1)) / p
    # Where alpha represents communication/synchronization overhead
    base_times = {}
    for func, func_group in grid_group.groupby('FUNCTION'):
        base_time = func_group[func_group['PROCESSORS'] == 1]['TIME'].values[0]
        base_times[func] = base_time
        
        # Simple theoretical model: T(p) = T(1) / p (ideal case)
        #ideal_times = base_time / processors_range
        #plt.plot(processors_range, ideal_times, 
        #        linestyle='--', color=color_dict[func], linewidth=1, alpha=0.5,
        #        label=f'{func} (Ideal)')
    
    plt.grid(True, alpha=0.5, linestyle='-', linewidth=0.8, color='gray')
    plt.title(f'Execution Time vs Processors - Grid Size {grid}x{grid}', fontsize=14, fontweight='bold')
    plt.xlabel('Threads (p)', fontsize=12)
    plt.ylabel('Time (seconds)', fontsize=12)
    plt.xticks(processors_range)
    plt.yscale('log')  # Log scale for better visualization of time differences
    plt.grid(True, alpha=0.3)
    plt.legend(bbox_to_anchor=(1.02, 1), loc='upper left', fontsize=10, frameon=True)
    plt.tight_layout(pad=8.0)
    
    plt.savefig(SAVE_DIR + f'time_vs_processors_grid_{grid}.png', dpi=300, bbox_inches='tight')
    plt.close()

print("Time vs Processors graphs generated successfully!")