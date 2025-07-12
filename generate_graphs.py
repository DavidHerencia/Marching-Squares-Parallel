import pandas as pd
import matplotlib.pyplot as plt
import os

# Ensure expected columns exist
required_columns = ['FUNCTION', 'PROCESSORS', 'TIME', 'GRIDSIZE', 'FLOPS']
df_x86 = pd.read_csv('results/benchmark_it3_x86.csv')[required_columns]
#remove p=128
df_x86 = df_x86[df_x86['GRIDSIZE'] == 16384]

df_cuda = pd.read_csv('results/benchmark_it4_cuda.csv')[required_columns]
df_x86['SPEEDUP'] = float('nan')

for (func,grid),group in df_x86.groupby(['FUNCTION', 'GRIDSIZE']):
    #get ts
    base_row = group[group["PROCESSORS"] == 1]
    ts = base_row['TIME'].values[0]
    
    df_x86.loc[group.index, 'SPEEDUP'] = ts / group['TIME']

for (func,grid),group in df_x86.groupby(['FUNCTION', 'GRIDSIZE']):
    plt.figure(figsize=(10, 6))
    plt.plot(group['PROCESSORS'], group['SPEEDUP'], marker='o', label=f'{func} {grid}')
    plt.title(f'Speedup for {func} with grid size {grid}')
    plt.xlabel('Number of Processors')
    plt.ylabel('Speedup')
    plt.xticks(group['PROCESSORS'])
    plt.grid(True)
    plt.legend()
    plt.savefig(f'results/graphs/speedup_{func}_{grid}.png')
    plt.close()
