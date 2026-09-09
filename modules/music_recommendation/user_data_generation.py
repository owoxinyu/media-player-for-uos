import pandas as pd
import random

data_path = "D:/spotify_dataset/data.csv"
try:
    data = pd.read_csv(data_path)
except FileNotFoundError:
    # 临时代码路径检查失败，先模拟一些数据来演示
    data = pd.DataFrame({
        'name': [f"Song {i}" for i in range(100)]
    })

# Step 1: 创建模拟用户行为数据
num_users = 200
ratings_per_user = 200
users = [f"user_{i:03d}" for i in range(1, num_users + 1)]
songs = data['name'].dropna().unique().tolist()

records = []
for user in users:
    rated = random.sample(songs, min(ratings_per_user, len(songs)))
    for song in rated:
        rating = random.randint(3, 5)  # 假设为正反馈评分
        records.append([user, song, rating])

# 转换为 DataFrame
df_user_song = pd.DataFrame(records, columns=['user_id', 'song_name', 'rating'])

# 保存为 CSV 文件
output_path = "D:/spotify_dataset/user_data.csv"
df_user_song.to_csv(output_path, index=False)

output_path
