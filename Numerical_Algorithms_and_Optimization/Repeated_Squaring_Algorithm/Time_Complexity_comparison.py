#!/usr/bin/env python
# coding: utf-8

# In[1]:


import pandas as pd
import matplotlib.pyplot as plt


# In[13]:


df = pd.read_csv('computing_time_comparison.csv')
df.head()


# In[17]:


plt.scatter(df['exponent'],df['Brute_force_time'], label='Brute Force Multiplication')
plt.scatter(df['exponent'],df['repeated_squaring_time'], label='Repeated Squares')
plt.grid()
plt.xlabel('Exponent')
plt.ylabel('Time required to run')

plt.legend()
plt.show()


# In[ ]:




