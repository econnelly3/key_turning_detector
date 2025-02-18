import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv(r'Training_Model\Labeled Data V0.csv', header=0)

plt.figure(figsize=(12, 5))
plt.plot(df.index, df['angleX'], label='Angle X')
plt.scatter(df[df['label'] == 1].index, df[df['label'] == 1]['angleX'], color='red', label='Key Turn (1)')
plt.xlabel("Time")
plt.ylabel("Angle X")
plt.legend()
plt.show()
