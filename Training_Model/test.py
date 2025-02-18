import pandas as pd

# Load and inspect the dataset
df = pd.read_csv('Training_Model/Labeled Data V0.csv')

print("First few rows of the dataset:")
print(df.head())  # View the first rows

print("\nUnique values in 'label' column:")
print(df['label'].unique())  # Check what values are present in 'label'

print("\nLabel count:")
print(df['label'].value_counts())  # Count the occurrences of each label
import matplotlib.pyplot as plt

# Plot labels across the dataset
plt.figure(figsize=(12, 5))
plt.plot(df.index, df['label'], marker='o', linestyle='', markersize=2, label="Label (0=No Turn, 1=Turn)")
plt.xlabel("Row Index")
plt.ylabel("Label")
plt.title("Distribution of Key Turns (Label 1) in Dataset")
plt.legend()
plt.show()
