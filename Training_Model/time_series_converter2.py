import pandas as pd

csv_path = 'Training_Data_R1.csv'

# 1) Print the file path and working directory
import os
print("Current working directory:", os.getcwd())
print("CSV path:", csv_path)
print("File exists?", os.path.exists(csv_path))

# 2) Load the data
df = pd.read_csv(csv_path, header=0)  # or names=[...] if no header
df['label'] = df['label'].astype(int)  # make sure label is int

# 3) Print first rows and label distribution
print("First 10 rows:\n", df.head(10))
print("Label distribution:\n", df['label'].value_counts())

# 4) Window function
def create_windows(data, window_size, step_size):
    windows = []
    labels = []

    for start in range(0, len(data) - window_size, step_size):
        window = data.iloc[start:start+window_size]
        
        # Debug: print the label array for each window
        print(f"Window {start}-{start+window_size} label values:", window['label'].values)

        # Flatten angles
        feature_vector = window[['angleX', 'angleY', 'angleZ']].values.flatten()
        # Label = 1 if majority of labels in window are 1
        label = 1 if window['label'].sum() > (window_size / 2) else 0

        windows.append(feature_vector)
        labels.append(label)

    return windows, labels

# 5) Create windows
X, y = create_windows(df, window_size=120, step_size=25)

print(f"\nFinal window count = {len(X)}")
print(pd.Series(y).value_counts())

# Convert to DataFrame and save
windowed_df = pd.DataFrame(X)
windowed_df['label'] = y
windowed_df.to_csv(r'Training_Model\time_series_key_turn_windows.csv', index=False)

print("Saved windowed dataset to 'time_series_key_turn_windows.csv'")
