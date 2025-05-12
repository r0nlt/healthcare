
import matplotlib.pyplot as plt
import pandas as pd
import glob
import os
import numpy as np

# Find all CSV files with radiation test results
csv_files = glob.glob("radiation_test_*.csv")

# Process each file
for csv_file in csv_files:
    try:
        # Extract test parameters from filename
        filename = os.path.basename(csv_file)
        params = filename.replace("radiation_test_", "").replace(".csv", "").split("_")
        radiation_intensity = float(params[0])
        protection_status = params[1]
        
        # Read data
        df = pd.read_csv(csv_file)
        
        # Create figure with 2x2 subplots
        fig, axs = plt.subplots(2, 2, figsize=(15, 10))
        fig.suptitle(f"Radiation Test Results - Intensity: {radiation_intensity}, Protection: {protection_status}", 
                     fontsize=16)
        
        # Plot 1: MSE over samples
        axs[0, 0].plot(df['sample_id'], df['mse'], 'b-')
        axs[0, 0].set_title('MSE over Samples')
        axs[0, 0].set_xlabel('Sample ID')
        axs[0, 0].set_ylabel('Mean Squared Error')
        axs[0, 0].grid(True)
        
        # Plot 2: Errors over samples
        axs[0, 1].plot(df['sample_id'], df['raw_error'], 'r-', label='Raw Errors')
        if protection_status == "enabled":
            axs[0, 1].plot(df['sample_id'], df['corrected_error'], 'g-', label='Corrected Errors')
        axs[0, 1].set_title('Errors over Samples')
        axs[0, 1].set_xlabel('Sample ID')
        axs[0, 1].set_ylabel('Number of Errors')
        axs[0, 1].legend()
        axs[0, 1].grid(True)
        
        # Plot 3: Histogram of MSE
        axs[1, 0].hist(df['mse'], bins=20, alpha=0.7, color='blue')
        axs[1, 0].set_title('MSE Distribution')
        axs[1, 0].set_xlabel('MSE')
        axs[1, 0].set_ylabel('Frequency')
        axs[1, 0].grid(True)
        
        # Plot 4: Error correction effectiveness (if protection enabled)
        if protection_status == "enabled":
            correction_rate = df['corrected_error'].sum() / max(df['raw_error'].sum(), 1) * 100
            labels = ['Corrected', 'Uncorrected']
            sizes = [correction_rate, 100 - correction_rate]
            axs[1, 1].pie(sizes, labels=labels, autopct='%1.1f%%', 
                          colors=['green', 'red'], startangle=90)
            axs[1, 1].set_title('Error Correction Rate')
        else:
            axs[1, 1].text(0.5, 0.5, 'Protection Disabled\nNo Error Correction',
                          horizontalalignment='center', verticalalignment='center',
                          transform=axs[1, 1].transAxes, fontsize=14)
            axs[1, 1].set_title('Error Correction Rate')
            axs[1, 1].axis('off')
        
        # Add statistics as text
        stats_text = f"Statistics:\n"
        stats_text += f"Total Samples: {len(df)}\n"
        stats_text += f"Avg MSE: {df['mse'].mean():.6f}\n"
        stats_text += f"Total Raw Errors: {df['raw_error'].sum()}\n"
        if protection_status == "enabled":
            stats_text += f"Total Corrected Errors: {df['corrected_error'].sum()}\n"
            stats_text += f"Correction Rate: {correction_rate:.2f}%"
        
        fig.text(0.5, 0.01, stats_text, horizontalalignment='center',
                 bbox=dict(facecolor='white', alpha=0.8))
        
        # Adjust layout and save
        plt.tight_layout(rect=[0, 0.05, 1, 0.95])
        plt.savefig(f"radiation_test_{radiation_intensity}_{protection_status}.png", dpi=300)
        plt.close()
        
        print(f"Created visualization for {csv_file}")
        
    except Exception as e:
        print(f"Error processing {csv_file}: {e}")

print("Visualization complete. Check the generated PNG files.")
