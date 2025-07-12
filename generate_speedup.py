import pandas as pd
import matplotlib

import matplotlib.pyplot as plt
import os

os.makedirs('results/graphs', exist_ok=True)
plt.style.use('seaborn-v0_8')

# Ensure expected columns exist
required_columns = ['FUNCTION', 'PROCESSORS', 'TIME', 'GRIDSIZE', 'FLOPS']
df_x86 = pd.read_csv('results/benchmark_it3_x86.csv')[required_columns]

# Filter out rows where GRIDSIZE is not in [16384, 8192]
df_x86 = df_x86[df_x86['GRIDSIZE'].isin([16384, 8192])]

df_cuda = pd.read_csv('results/benchmark_it4_cuda.csv')[required_columns]
df_x86['SPEEDUP'] = float('nan')

for (func,grid),group in df_x86.groupby(['FUNCTION', 'GRIDSIZE']):
    #get ts
    base_row = group[group["PROCESSORS"] == 1]
    ts = base_row['TIME'].values[0]
    
    df_x86.loc[group.index, 'SPEEDUP'] = ts / group['TIME']
    
    
function_list = sorted(df_x86['FUNCTION'].unique())
color_map = matplotlib.colormaps['tab10']
color_dict = {func: color_map(i / len(function_list)) for i, func in enumerate(function_list)}

# Group by grid size and plot all functions in the same graph
for grid, grid_group in df_x86.groupby('GRIDSIZE'):
    plt.figure(figsize=(12, 8))
    
    for func, func_group in grid_group.groupby('FUNCTION'):
        plt.plot(func_group['PROCESSORS'], func_group['SPEEDUP'], 
                marker='o', label=f'{func}', linewidth=2, markersize=6,
                color=color_dict[func])
    
    processors_range = grid_group['PROCESSORS'].unique()
    plt.plot(processors_range, processors_range, 
            linestyle='--', color='black', linewidth=2, alpha=0.7, 
            label='Theoretical Speedup')
    
    plt.title(f'Speedup Comparison by Function - Grid Size {grid}x{grid}', fontsize=14, fontweight='bold')
    plt.xlabel('Threads (p)', fontsize=12)
    plt.ylabel('Speedup', fontsize=12)
    plt.xticks(processors_range)
    plt.grid(True, alpha=0.3)
    plt.legend(bbox_to_anchor=(1.02, 1), loc='upper left', fontsize=10, frameon=True)
    plt.tight_layout(pad = 8.0)
    
    plt.savefig(f'results/graphs/speedup_comparison_grid_{grid}.png', dpi=300, bbox_inches='tight')
    plt.close()