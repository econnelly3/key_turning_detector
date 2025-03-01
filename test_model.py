import pandas as pd
import numpy as np
import joblib
import matplotlib.pyplot as plt
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix

# ✅ Load trained model (before conversion to .h)
MODEL_PATH = "key_turn_model.pkl"  # Ensure this file exists
model = joblib.load(MODEL_PATH)

# ✅ Load test data
TEST_DATA_PATH = "Test_Data_R1.csv"  # Your collected key turn dataset
df_test = pd.read_csv(TEST_DATA_PATH)

WINDOW_SIZE = 120
STEP_SIZE = 25
window = []
predictions = []
probabilities = []
actual_labels = df_test['label'].tolist()

print("\n🔄 Running Real-Time Key Turn Detection Simulation...\n")

# ✅ Simulate real-time data
for i, row in df_test.iterrows():
    window.append(row[['angleX', 'angleY', 'angleZ']].values)
    if len(window) > WINDOW_SIZE:
        window.pop(0)

    if len(window) == WINDOW_SIZE:
        feature_vector = np.array(window).flatten().reshape(1, -1)
        prediction = model.predict(feature_vector)[0]
        prob = model.predict_proba(feature_vector)[0][1]

        predictions.append(prediction)
        probabilities.append(prob)

        print(f"⏳ Sample {i} | Prediction: {prediction} | Probability: {prob:.2f}")
        if prediction == 1:
            print("🔑 Key Turn Detected!")

# ✅ Ensure predictions align with actual labels
predictions = predictions[:len(actual_labels)]

# ✅ Print Performance Summary
print("\n📊 Performance Evaluation:\n")

accuracy = accuracy_score(actual_labels[:len(predictions)], predictions)
print(f"✅ Accuracy: {accuracy:.2f}")

report = classification_report(actual_labels[:len(predictions)], predictions, target_names=["No Turn", "Turn"])
print("\n🔍 Classification Report:\n", report)

conf_matrix = confusion_matrix(actual_labels[:len(predictions)], predictions)
print("\n🟦 Confusion Matrix:\n", conf_matrix)

# ✅ Plot results
plt.figure(figsize=(10, 5))
plt.plot(actual_labels[:len(predictions)], label="Actual Key Turn (Ground Truth)", linewidth=2)
plt.plot(predictions, label="Predicted Key Turn", linestyle="dashed", linewidth=2)
plt.xlabel("Time Step")
plt.ylabel("Key Turn Detection (0 = No, 1 = Yes)")
plt.title("Real-Time Key Turn Predictions vs. Actual Data")
plt.legend()
plt.show()

print("\n✅ Simulation Complete! Check the plot above to analyze performance.")
