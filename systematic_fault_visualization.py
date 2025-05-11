
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import seaborn as sns

# Read the data
df = pd.read_csv("systematic_fault_results.csv")

# Set up the plotting style
sns.set(style="whitegrid")
plt.rcParams["figure.figsize"] = (15, 10)

# Create a figure for fault pattern comparison
plt.figure()

# Group by pattern and protection status, calculate mean MSE
pattern_mse = df.groupby(['pattern', 'protection'])['mse'].mean().unstack()

# Plot pattern comparison
ax = pattern_mse.plot(kind='bar', color=['red', 'green'])
plt.title('MSE by Fault Pattern and Protection Status', fontsize=16)
plt.xlabel('Fault Pattern', fontsize=14)
plt.ylabel('Mean Squared Error', fontsize=14)
plt.xticks(rotation=45)
plt.legend(['Protection Disabled', 'Protection Enabled'])
plt.tight_layout()
plt.savefig('mse_by_pattern.png', dpi=300)

# Create a figure for correction rate by pattern
plt.figure()
protection_enabled = df[df['protection'] == 'enabled']
sns.barplot(x='pattern', y='correction_rate', data=protection_enabled)
plt.title('Error Correction Rate by Fault Pattern', fontsize=16)
plt.xlabel('Fault Pattern', fontsize=14)
plt.ylabel('Correction Rate (%)', fontsize=14)
plt.xticks(rotation=45)
plt.tight_layout()
plt.savefig('correction_by_pattern.png', dpi=300)

# Create a figure for MSE vs. coverage percentage
plt.figure()
coverage_plot = sns.lineplot(x='coverage', y='mse', hue='protection', 
                             style='pattern', data=df, markers=True)
plt.title('MSE vs. Coverage Percentage', fontsize=16)
plt.xlabel('Coverage Percentage', fontsize=14)
plt.ylabel('Mean Squared Error', fontsize=14)
plt.tight_layout()
plt.savefig('mse_vs_coverage.png', dpi=300)

# Create a heatmap for pattern effectiveness
plt.figure(figsize=(12, 8))
pivot_table = df.pivot_table(values='mse', 
                            index='pattern', 
                            columns=['protection', 'coverage'])
sns.heatmap(pivot_table, annot=True, cmap="YlGnBu", fmt=".3f")
plt.title('MSE Heatmap by Pattern, Protection and Coverage', fontsize=16)
plt.tight_layout()
plt.savefig('mse_heatmap.png', dpi=300)

# Summary statistics
print("\nSummary Statistics:")
print(df.groupby(['pattern', 'protection'])['mse', 'correction_rate'].agg(['mean', 'std', 'min', 'max']))

# Create scatter plot of injected faults vs correction rate
plt.figure()
sns.scatterplot(x='injected_faults', y='correction_rate', hue='pattern', 
                size='coverage', sizes=(50, 200), data=protection_enabled)
plt.title('Error Correction Rate vs. Injected Faults', fontsize=16)
plt.xlabel('Number of Injected Faults', fontsize=14)
plt.ylabel('Correction Rate (%)', fontsize=14)
plt.tight_layout()
plt.savefig('correction_vs_faults.png', dpi=300)

print("Visualizations complete. Check the PNG files.")
