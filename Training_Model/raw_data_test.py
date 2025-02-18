import pandas as pd

# Load the original manually labeled data
df = pd.read_csv('Labeled Data V0.csv')

# Print total rows
print(f"Total Rows in Original Data: {len(df)}")

# Check label distribution
print(df['label'].value_counts())
