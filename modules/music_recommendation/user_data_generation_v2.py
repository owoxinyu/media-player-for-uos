import pandas as pd
import numpy as np
import random

from sklearn.pipeline import Pipeline
from sklearn.preprocessing import StandardScaler
from sklearn.cluster import KMeans


# 加载歌曲数据（需要有 cluster_label）
data = pd.read_csv("D:/spotify_dataset/data.csv")

# 防止缺失
if 'cluster_label' not in data.columns:
    from sklearn.cluster import KMeans
    from sklearn.preprocessing import StandardScaler
    number_cols = data.select_dtypes(np.number).columns
    pipeline = Pipeline([('scaler', StandardScaler()), ('kmeans', KMeans(n_clusters=20, random_state=42))])
    data['cluster_label'] = pipeline.fit_predict(data[number_cols])

# Step 1: 用户设置
num_users = 200
ratings_per_user = 200
users = [f"user_{i:03d}" for i in range(1, num_users + 1)]

records = []

for user in users:
    # 为每个用户分配 1-2 个偏好 cluster（音乐风格）
    preferred_clusters = random.sample(range(20), k=random.choice([1, 2]))

    # 从偏好聚类中选择歌曲
    candidate_songs = data[data['cluster_label'].isin(preferred_clusters)].copy()
    candidate_songs = candidate_songs.sample(n=min(ratings_per_user, len(candidate_songs)), random_state=random.randint(0, 9999))

    for _, row in candidate_songs.iterrows():
        song_name = row['name']
        duration = random.randint(120, 300)  # 歌曲总时长（秒）
        play_time = random.randint(0, duration)
        play_ratio = round(play_time / duration, 2)
        play_count = random.randint(1, 20)
        skipped = int(play_ratio < 0.3)

        # 计算综合评分
        rating = (
            0.5 * play_ratio +
            0.1 * (play_count / 20) +
            -0.3 * skipped +
            random.uniform(0, 0.3)
        ) * 5
        rating = max(0, min(5, round(rating, 2)))  # 映射到 0~5 分

        records.append([
            user,
            song_name,
            play_count,
            duration,
            play_ratio,
            skipped,
            rating
        ])

# 创建 DataFrame
df_user_song = pd.DataFrame(records, columns=[
    'user_id', 'song_name', 'play_count', 'duration', 'play_ratio', 'skipped', 'rating'
])

# 保存为 CSV 文件
output_path = "D:/spotify_dataset/user_data_v2.csv"
df_user_song.to_csv(output_path, index=False)
print(f"✅ 用户行为数据生成完毕：{output_path}")
