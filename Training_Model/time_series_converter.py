import pandas as pd

import os
print("Current working directory:", os.getcwd())
csv_path = 'Training_Model/Labeled Data V0.csv'
print("Reading CSV from:", csv_path)
print("File exists?", os.path.exists(csv_path))
df = pd.read_csv(csv_path)
print("First 10 rows:\n", df.head(10))
print("Label distribution:\n", df['label'].value_counts())
df = pd.read_csv(csv_path, header=0)
df['label'] = df['label'].astype(str).str.strip().astype(int)
print(df.head(10))  # Check the first 10 rows
print(df['label'].value_counts())




# Load your manually labeled data
df = pd.read_csv(r'Training_Model\Labeled Data V0.csv', names=["angleX", "angleY", "angleZ", "label"])

# Parameters
WINDOW_SIZE = 10      # Number of readings per window (~0.5 sec if 100Hz)
STEP_SIZE = 5        # Overlapping window step

def create_windows(data, window_size, step_size):
    windows = []
    labels = []

    for start in range(0, len(data) - window_size, step_size):
        window = data.iloc[start:start+window_size]  # Ensure window size is correct
        
        # Flatten X, Y, Z values for ML training
        feature_vector = window[['angleX', 'angleY', 'angleZ']].values.flatten()
        
        # Debug: Print how many `1`s are in the current window
        label_count = sum(window['label'] == 1)
        print(f"Window {start}-{start+window_size}: Found {label_count} ones")

        # Ensure window is labeled `1` if any row inside it is `1`
        label = 1 if label_count > 0 else 0  

        windows.append(feature_vector)
        labels.append(label)

    return windows, labels
X, y = create_windows(df, window_size=50, step_size=25)

# Save the corrected dataset
windowed_df = pd.DataFrame(X)
windowed_df['label'] = y
windowed_df.to_csv('time_series_key_turn_windows.csv', index=False)

print(f"Fixed windowed dataset saved! New shape: {len(X)}")
print(pd.Series(y).value_counts())  # Ensure `1`s exist
