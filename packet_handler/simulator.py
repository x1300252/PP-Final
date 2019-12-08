import struct
import socket
import pandas as pd
import matplotlib.pyplot as plt
import sys

def generate_flow_label (row, table_size):
    attrs = row['Flow.ID'].split("-")
    int_attrs = (socket.inet_aton(attrs[0]), socket.inet_aton(attrs[1]), int(attrs[2]), int(attrs[3]), int(attrs[4]))
    return hash(int_attrs) % table_size
    # return  socket.inet_ntoa(struct.pack("!I", row['Source.IP'])) << 56 +\
    #         socket.inet_ntoa(struct.pack("!I", row['Destination.IP'])) << 24 +\
    #         row['Source.Port'] << 16 + row['Destination.Port'] << 8 + row['Protocol']


# df = pd.read_csv('./raw_data/app_flow.csv', usecols=[1, 2, 3, 4, 5])

# df.to_csv('./flow_data/app_flow.csv', index=False)

# df = pd.read_csv('./flow_data/app_flow.csv')
# print(df)

# flow_size = pd.DataFrame({'Flow.size' : df.groupby( ['Flow.ID'] ).size()}).reset_index()
# flow_size['Hash.idx_100000'] = flow_size.apply (lambda row: generate_flow_label(row, 100000), axis=1)
# flow_size['Hash.idx_500000'] = flow_size.apply (lambda row: generate_flow_label(row, 500000), axis=1)
# flow_size['Hash.idx_750000'] = flow_size.apply (lambda row: generate_flow_label(row, 750000), axis=1)
# flow_size['Hash.idx_1000000'] = flow_size.apply (lambda row: generate_flow_label(row, 1000000), axis=1)
# flow_size.to_csv('./flow_data/app_flow_size.csv')

flow_size = pd.read_csv('./flow_data/app_flow_size.csv')
print(flow_size)
hash_size_100000 = pd.DataFrame(flow_size.groupby(['Hash.idx_100000'])['Flow.size'].sum())
hash_size_500000 = pd.DataFrame(flow_size.groupby(['Hash.idx_500000'])['Flow.size'].sum())
hash_size_750000 = pd.DataFrame(flow_size.groupby(['Hash.idx_750000'])['Flow.size'].sum())
hash_size_1000000 = pd.DataFrame(flow_size.groupby(['Hash.idx_1000000'])['Flow.size'].sum())
# print(hash_size)
# print(hash_size['Flow.size'].sum())
# print((hash_size.groupby(['Flow.size']).size()).sum())
hash_size_distribution_100000 = pd.DataFrame({'hash_size_cnt_100000' : hash_size_100000.groupby( ['Flow.size'] ).size()}).reset_index()
hash_size_distribution_500000 = pd.DataFrame({'hash_size_cnt_500000' : hash_size_500000.groupby( ['Flow.size'] ).size()}).reset_index()
hash_size_distribution_750000 = pd.DataFrame({'hash_size_cnt_750000' : hash_size_750000.groupby( ['Flow.size'] ).size()}).reset_index()
hash_size_distribution_1000000 = pd.DataFrame({'hash_size_cnt_1000000' : hash_size_1000000.groupby( ['Flow.size'] ).size()}).reset_index()
# print(hash_size_distribution)
# print(hash_size_distribution['hash_size_cnt'].sum())


flow_size_distribution = pd.DataFrame({'flow_size_cnt' : flow_size.groupby( ['Flow.size'] ).size()}).reset_index()
# print(flow_size_distribution)
# print(flow_size_distribution['flow_size_cnt'].sum())

# flow_size_distribution.join(hash_size_distribution, on='Flow.size')
result = pd.merge(flow_size_distribution, hash_size_distribution_100000, on='Flow.size', how='outer')
result = pd.merge(result, hash_size_distribution_500000, on='Flow.size', how='outer')                 
result = pd.merge(result, hash_size_distribution_750000, on='Flow.size', how='outer')
result = pd.merge(result, hash_size_distribution_1000000, on='Flow.size', how='outer')

result.fillna(0, inplace=True)
result['hash_size_cnt_100000'] = pd.to_numeric(result['hash_size_cnt_100000']).astype(int)
result['hash_size_cnt_500000'] = pd.to_numeric(result['hash_size_cnt_500000']).astype(int)
result['hash_size_cnt_750000'] = pd.to_numeric(result['hash_size_cnt_750000']).astype(int)
result['hash_size_cnt_1000000'] = pd.to_numeric(result['hash_size_cnt_1000000']).astype(int)

result.index.name = 'idx'
result.to_csv('./flow_data/app_flow_size_distribution.csv')
print(result)
result.plot(x='Flow.size')

plt.yscale('log')
plt.show()