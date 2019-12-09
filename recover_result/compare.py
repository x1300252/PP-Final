import pandas as pd
import matplotlib.pyplot as plt

size_distribution = pd.read_csv('../packet_handler/flow_data/app_flow_size_distribution.csv', usecols=[1,2,5])

recover_result_1000000 = pd.read_csv('./1000000.csv')
result = pd.merge(size_distribution, recover_result_1000000, on='Flow.size', how='outer')

result.plot(x='Flow.size', title='table size = 1000000')

plt.yscale('log')
plt.show()