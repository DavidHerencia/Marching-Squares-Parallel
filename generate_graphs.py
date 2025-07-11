import pandas as pd
import matplotlib.pyplot as plt
import os

# Read the CSV file
df = pd.read_csv('benchmark_it3.csv')

# Ensure expected columns exist
required_columns = ['FUNCTION', 'PROCESSORS', 'TIME', 'GRIDSIZE']
if not all(col in df.columns for col in required_columns):
    raise ValueError(f"CSV must contain columns: {required_columns}")

# Create main graphs directory
graphs_dir = 'graphs'
if not os.path.exists(graphs_dir):
    os.makedirs(graphs_dir)

# Get unique experiments and grid sizes
experiments = df['FUNCTION'].unique()
grid_sizes = sorted(df['GRIDSIZE'].unique())

print(f"Found experiments: {experiments}")
print(f"Found grid sizes: {grid_sizes}")

for grid_size in grid_sizes:
    # Create directory for this grid size
    grid_dir = os.path.join(graphs_dir, f'grid_{grid_size}')
    if not os.path.exists(grid_dir):
        os.makedirs(grid_dir)
    
    # Filter data for this grid size
    grid_df = df[df['GRIDSIZE'] == grid_size]
    
    for exp in experiments:
        exp_df = grid_df[grid_df['FUNCTION'] == exp]
        
        if exp_df.empty:
            continue
            
        # Sort by processors for better plotting
        exp_df = exp_df.sort_values('PROCESSORS')
        
        # Time vs Processors plot
        plt.figure(figsize=(10, 6))
        plt.plot(exp_df['PROCESSORS'], exp_df['TIME'], marker='o', linewidth=2, markersize=8)
        plt.title(f'{exp} - Time vs Processors (Grid Size: {grid_size}x{grid_size})')
        plt.xlabel('Processors')
        plt.ylabel('Time (s)')
        plt.grid(True, alpha=0.7)
        #plt.yscale('log')  # Log scale for better visualization
        plt.xscale('log', base=2)  # Log base 2 for processor count
        plt.xticks(exp_df['PROCESSORS'], exp_df['PROCESSORS'])
        filename = f'{exp.replace(" ", "_")}_time_vs_processors_grid_{grid_size}.png'
        plt.savefig(os.path.join(grid_dir, filename), dpi=300, bbox_inches='tight')
        plt.close()

        # Speedup calculation: Speedup = T1 / Tp
        #min_proc = exp_df['PROCESSORS'].min()
        #t1 = exp_df[exp_df['PROCESSORS'] == min_proc]['TIME'].values[0]
        #speedup = t1 / exp_df['TIME']
        #
        #plt.figure(figsize=(10, 6))
        #plt.plot(exp_df['PROCESSORS'], speedup, marker='o', linewidth=2, markersize=8, color='red')
        #
        ## Add ideal speedup line
        #ideal_speedup = exp_df['PROCESSORS'] / min_proc
        #plt.plot(exp_df['PROCESSORS'], ideal_speedup, '--', alpha=0.7, color='gray', label='Ideal Speedup')
        #
        #plt.title(f'{exp} - Speedup vs Processors (Grid Size: {grid_size}x{grid_size})')
        #plt.xlabel('Processors')
        #plt.ylabel('Speedup')
        #plt.grid(True, alpha=0.7)
        #plt.xscale('log', base=2)
        #plt.xticks(exp_df['PROCESSORS'], exp_df['PROCESSORS'])
        #plt.legend()
        #filename = f'{exp.replace(" ", "_")}_speedup_vs_processors_grid_{grid_size}.png'
        #plt.savefig(os.path.join(grid_dir, filename), dpi=300, bbox_inches='tight')
        #plt.close()

print(f"Graphs saved in '{graphs_dir}' directory, organized by grid size")