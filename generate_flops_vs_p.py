import pandas as pd
import matplotlib

import matplotlib.pyplot as plt
import os

# Create output directory if it doesn't exist
os.makedirs('results/graphs', exist_ok=True)

# Set matplotlib style for better looking plots
plt.style.use('seaborn-v0_8')

# Ensure expected columns exist
required_columns = ['FUNCTION', 'PROCESSORS', 'TIME', 'GRIDSIZE', 'FLOPS'] #FLOPS should be FLOP
df_x86 = pd.read_csv('results/benchmark_it3_x86.csv')[required_columns]
# Filter out rows where GRIDSIZE is not in [16384, 8192]
#df_x86 = df_x86[df_x86['GRIDSIZE'].isin([16384, 8192])]

df_x86['REAL_FLOPS'] = df_x86['FLOPS'] / df_x86['TIME']


function_list = sorted(df_x86['FUNCTION'].unique())
color_map = matplotlib.colormaps['tab10']
color_dict = {func: color_map(i / len(function_list)) for i, func in enumerate(function_list)}


#Now is the plot is about REAL_FLOPS vs PROCESSORS so graph each function on same graph
for grid, grid_group in df_x86.groupby('GRIDSIZE'):
    plt.figure(figsize=(12, 8))
    
    for func, func_group in grid_group.groupby('FUNCTION'):
        plt.plot(func_group['PROCESSORS'], func_group['REAL_FLOPS'], 
                marker='o', label=f'{func}', linewidth=2, markersize=6,
                color=color_dict[func])
    
    plt.title(f'FLOPS Comparison by Function - Grid Size {grid}x{grid}', fontsize=14, fontweight='bold')
    plt.xlabel('Threads (p)', fontsize=12)
    plt.ylabel('FLOPS (in billions)', fontsize=12)
    plt.yscale('log')
    plt.grid(True, alpha=0.5, linestyle='-', linewidth=0.8, color='gray')

    plt.xticks(func_group['PROCESSORS'])
    plt.grid(True, alpha=0.3)
    plt.legend(bbox_to_anchor=(1.02, 1), loc='upper left', fontsize=10, frameon=True)
    plt.tight_layout(pad = 8.0)
    
    plt.savefig(f'results/graphs/flops_comparison_grid_{grid}.png', dpi=300, bbox_inches='tight')
    plt.close()