import pandas as pd

df = pd.read_csv('Training_Model\Labeled Data V0.csv')

print("Min Values:\n", df.min())
print("Max Values:\n", df.max())
print(df['label'].value_counts())
import matplotlib.pyplot as plt

plt.figure(figsize=(12, 5))
plt.plot(df.index, df['angleX'], label='Angle X')
plt.scatter(df[df['label'] == 1].index, df[df['label'] == 1]['angleX'], color='red', label='Key Turn (1)')
plt.xlabel("Time")
plt.ylabel("Angle X")
plt.legend()
plt.show()
