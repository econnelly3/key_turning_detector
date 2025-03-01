import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix, precision_recall_curve
from micromlgen import port
import joblib  # For saving/loading model as .pkl
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

# ✅ Load your time-series CSV
df = pd.read_csv(r'Training_Model\time_series_key_turn_windows.csv', header=0)

# ✅ Drop non-numeric columns (if any)
df = df.apply(pd.to_numeric, errors='coerce')
df = df.dropna()

# ✅ Separate features (X) and labels (y)
X = df.drop(columns=['label']).values
y = df['label'].values

# ✅ Split into training and testing sets
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.3, random_state=42, stratify=y
)

# ✅ Train a Random Forest model
model = RandomForestClassifier(n_estimators=120, max_depth=10, random_state=42)
model.fit(X_train, y_train)

# ✅ Evaluate model
y_pred = model.predict(X_test)
accuracy = accuracy_score(y_test, y_pred)
print(f"Model Accuracy: {accuracy:.2f}")
print("\nClassification Report:")
print(classification_report(y_test, y_pred))

# ✅ Display confusion matrix
print("\nConfusion Matrix:")
print(confusion_matrix(y_test, y_pred))

# ✅ Save model for ESP32 (.h)
with open('key_turn_model.h', 'w') as f:
    f.write(port(model))
print("✅ Model exported to 'key_turn_model.h' for ESP32.")

# ✅ Save model as .pkl for Python testing
joblib.dump(model, "key_turn_model.pkl")
print("✅ Model saved as 'key_turn_model.pkl' for testing in Python.")

# ✅ 1. Confusion Matrix Plot
plt.figure(figsize=(6, 5))
sns.heatmap(confusion_matrix(y_test, y_pred), annot=True, fmt="d", cmap="Blues", xticklabels=["No Turn", "Turn"], yticklabels=["No Turn", "Turn"])
plt.xlabel("Predicted")
plt.ylabel("Actual")
plt.title("Confusion Matrix")
plt.show()

# ✅ 2. Precision-Recall Curve
y_probs = model.predict_proba(X_test)[:, 1]  # Get probability scores
precision, recall, _ = precision_recall_curve(y_test, y_probs)

plt.figure(figsize=(6, 5))
plt.plot(recall, precision, marker='.')
plt.xlabel('Recall')
plt.ylabel('Precision')
plt.title('Precision-Recall Curve')
plt.grid()
plt.show()

# ✅ 3. Prediction vs. Ground Truth Over Time
plt.figure(figsize=(8, 4))
plt.plot(np.arange(len(y_test)), y_test, 'bo', label='Actual')
plt.plot(np.arange(len(y_pred)), y_pred, 'ro', label='Predicted', alpha=0.6)
plt.xlabel("Sample Index")
plt.ylabel("Class (0 = No Turn, 1 = Turn)")
plt.title("Predicted vs. Actual Key Turns")
plt.legend()
plt.show()
