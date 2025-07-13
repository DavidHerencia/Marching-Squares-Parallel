import pandas as pd
import matplotlib

import matplotlib.pyplot as plt
import os

SAVE_DIR = 'results/graphs/'

os.makedirs(SAVE_DIR, exist_ok=True)
plt.style.use('seaborn-v0_8')
plt.rcParams.update({
    "font.size": 18,
    "axes.titlesize": 28,
    "axes.labelsize": 24,
    "xtick.labelsize": 18,
    "ytick.labelsize": 18,
    "legend.fontsize": 14,
    "figure.titlesize": 32
})

# Ensure expected columns exist
required_columns = ['FUNCTION', 'PROCESSORS', 'TIME', 'GRIDSIZE', 'FLOPS']
df_x86 = pd.read_csv('./results/benchmark_it3_x86.csv')[required_columns]

# Filter out rows where GRIDSIZE is not in [16384, 8192]
#df_x86 = df_x86[df_x86['GRIDSIZE'].isin([16384, 8192])]
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
    plt.figure(figsize=(14, 10)) 
    
    for func, func_group in grid_group.groupby('FUNCTION'):
        plt.plot(func_group['PROCESSORS'], func_group['SPEEDUP'], 
                marker='o', label=f'{func}', linewidth=2, markersize=6,
                color=color_dict[func])
    
    processors_range = grid_group['PROCESSORS'].unique()
    grid_size = grid_group['GRIDSIZE'].values[0]
    #O( n**2 / (n**2 / P + P))
    
    # Theoretical speedup line
    # Model: coef * (grid_size**2 / (grid_size**2 / p + p))
    theorical_raw = grid_size**2 / (grid_size**2 / processors_range + processors_range)
    coef = grid_size**2 / (grid_size**2  + 1) 
    print(f'Grid {grid_size} - Coefficient for theoretical speedup: {coef}')
    theoretical_speedup =  theorical_raw * coef 
    
    plt.plot(processors_range, theoretical_speedup, 
            linestyle='--', color='black', linewidth=2, alpha=0.7, 
            label='Theoretical speedup O(n²/((n²/p)+p))')
    
    plt.grid(True, alpha=0.5, linestyle='-', linewidth=0.8, color='gray')
    plt.title(f'Speedup Comparison by Function - Grid Size {grid}x{grid}', fontweight='bold')
    plt.xlabel('Threads (p)')
    plt.ylabel('Speedup')
    plt.xscale('log',base=2)
    plt.yscale('log',base=10)
    

    plt.xticks(processors_range)
    plt.grid(True, alpha=0.3)
    plt.legend(loc='upper left', frameon=True, fancybox=True, shadow=True)
    plt.tight_layout(pad=2.0)
    
    plt.savefig(SAVE_DIR + f'speedup_comparison_grid_{grid}.png', dpi=300, bbox_inches='tight')

    plt.close()