import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix
from micromlgen import port

# Load your time-series CSV (skip header if needed)
df = pd.read_csv(r'Training_Model\time_series_key_turn_windows.csv', header=0)

# Drop any non-numeric columns (like headers accidentally in rows)
df = df.apply(pd.to_numeric, errors='coerce')
df = df.dropna()

# Separate features (X) and labels (y)
X = df.drop(columns=['label']).values
y = df['label'].values

# Split into training and testing sets
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.3, random_state=42, stratify=y
)

# Train a Random Forest model
model = RandomForestClassifier(n_estimators=100, max_depth=10, random_state=42)
model.fit(X_train, y_train)

# Evaluate model
y_pred = model.predict(X_test)
accuracy = accuracy_score(y_test, y_pred)
print(f"Model Accuracy: {accuracy:.2f}")
print("\nClassification Report:")
print(classification_report(y_test, y_pred))

# Display confusion matrix
print("\nConfusion Matrix:")
print(confusion_matrix(y_test, y_pred))

# Save model for ESP32 (convert to C++ header file)
from micromlgen import port
with open('key_turn_model.h', 'w') as f:
    f.write(port(model))

print("Model exported to 'key_turn_model.h' for ESP32.")
