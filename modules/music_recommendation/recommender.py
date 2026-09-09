import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import plotly.express as px

from tensorflow.keras.models import Model # type: ignore
from tensorflow.keras.layers import Input, Dense # type: ignore
# === 新增模块导入 ===
from sklearn.preprocessing import MinMaxScaler
from sklearn.model_selection import train_test_split
from sklearn.cluster import KMeans
from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import Pipeline
from sklearn.manifold import TSNE
from sklearn.decomposition import PCA
from sklearn.metrics.pairwise import cosine_similarity
from scipy.spatial.distance import cdist
from collections import defaultdict
from tqdm import tqdm


import warnings
warnings.filterwarnings("ignore")

# === Load Data ===
data = pd.read_csv("data.csv")
genre_data = pd.read_csv("data_by_genres.csv")
year_data = pd.read_csv("data_by_year.csv")
user_data = pd.read_csv("user_data_v2.csv")

# === Genre Clustering ===
cluster_pipeline = Pipeline([
    ('scaler', StandardScaler()),
    ('kmeans', KMeans(n_clusters=10))
])
X_genre = genre_data.select_dtypes(np.number)
cluster_pipeline.fit(X_genre)
genre_data['cluster'] = cluster_pipeline.predict(X_genre)

# === Song Clustering with Clean Features ===
excluded_cols = ['name', 'artists', 'id', 'release_date', 'explicit']
X_song = data.drop(columns=excluded_cols, errors='ignore')
X_song = X_song.select_dtypes(include=[np.number]).astype(np.float64)
X_song.columns = X_song.columns.str.strip()
number_cols = list(X_song.columns)
print("✅ 使用的特征列:", number_cols)

song_cluster_pipeline = Pipeline([
    ('scaler', StandardScaler()),
    ('kmeans', KMeans(n_clusters=20))
])
song_cluster_pipeline.fit(X_song)
data['cluster_label'] = song_cluster_pipeline.predict(X_song)

# === PCA Visualization (songs) ===
def visualize_song_clusters(data, number_cols):
    pca_pipeline = Pipeline([
        ('scaler', StandardScaler()),
        ('pca', PCA(n_components=2))
    ])
    song_embedding = pca_pipeline.fit_transform(data[number_cols])
    projection = pd.DataFrame(columns=['x', 'y'], data=song_embedding)
    projection['title'] = data['name'] if 'name' in data.columns else data['track_name']
    projection['cluster'] = data['cluster_label']

    fig = px.scatter(projection, x='x', y='y', color='cluster', hover_data=['x', 'y', 'title'])
    fig.update_layout(title='PCA Projection of Songs by Cluster')
    fig.show()

# === TSNE Visualization (genres) ===
def visualize_genre_tsne(genre_data):
    tsne_pipeline = Pipeline([
        ('scaler', StandardScaler()),
        ('tsne', TSNE(n_components=2, verbose=1))
    ])
    X = genre_data.select_dtypes(np.number)
    genre_embedding = tsne_pipeline.fit_transform(X)
    projection = pd.DataFrame(columns=['x', 'y'], data=genre_embedding)
    projection['genres'] = genre_data['genres']
    projection['cluster'] = genre_data['cluster']

    fig = px.scatter(projection, x='x', y='y', color='cluster', hover_data=['x', 'y', 'genres'])
    fig.update_layout(title='t-SNE Projection of Genres by Cluster')
    fig.show()

# === Statistics Summary ===
def show_statistics(data):
    print("\n📊 基本统计信息：")
    print(data.describe())
    print("\n🎶 按流派统计歌曲数量：")
    if 'genre' in data.columns:
        print(data['genre'].value_counts().head(10))
    print("\n📅 按年份统计歌曲数量：")
    print(data['year'].value_counts().sort_index())

# === Helper Functions ===
def find_song(name, year, spotify_data):
    song_data = spotify_data[
        (spotify_data['name'].str.lower() == name.lower()) &
        (spotify_data['year'] == year)
    ]
    if len(song_data) == 0:
        print(f"⚠️ Warning: '{name}' ({year}) not found in dataset.")
        return None
    return song_data.iloc[0]

def get_song_data(song, spotify_data):
    return find_song(song['name'], song['year'], spotify_data)

def get_mean_vector(song_list, spotify_data):
    song_vectors = []
    for song in song_list:
        song_data = get_song_data(song, spotify_data)
        if song_data is None:
            continue
        song_vector = song_data[number_cols].dropna().to_numpy(dtype=np.float32)
        song_vectors.append(song_vector)
    if not song_vectors:
        raise ValueError("❌ 没有找到任何有效歌曲")
    song_matrix = np.vstack(song_vectors)
    mean_vector = np.mean(song_matrix, axis=0)
    return mean_vector.astype(np.float32)

def flatten_dict_list(dict_list):
    flattened_dict = defaultdict(list)
    for dictionary in dict_list:
        for key, value in dictionary.items():
            flattened_dict[key].append(value)
    return flattened_dict

def recommend_songs(song_list, spotify_data, n_songs=10):
    metadata_cols = ['name', 'year', 'artists'] if 'artists' in spotify_data.columns else ['name', 'year']
    song_dict = flatten_dict_list(song_list)
    song_center = get_mean_vector(song_list, spotify_data)
    scaler = song_cluster_pipeline.steps[0][1]
    scaled_data = scaler.transform(spotify_data[number_cols])
    scaled_song_center = scaler.transform(song_center.reshape(1, -1))
    distances = cdist(scaled_song_center, scaled_data, 'cosine')
    index = list(np.argsort(distances)[:, :n_songs][0])
    rec_songs = spotify_data.iloc[index]
    rec_songs = rec_songs[~rec_songs['name'].isin(song_dict['name'])]
    return rec_songs[metadata_cols].to_dict(orient='records')

# === Autoencoder ===
def build_autoencoder(input_dim, encoding_dim=8):
    input_layer = Input(shape=(input_dim,))
    encoded = Dense(128, activation='relu')(input_layer)
    encoded = Dense(64, activation='relu')(encoded)
    encoded = Dense(encoding_dim, activation='relu')(encoded)
    decoded = Dense(64, activation='relu')(encoded)
    decoded = Dense(128, activation='relu')(decoded)
    decoded = Dense(input_dim, activation='linear')(decoded)
    autoencoder = Model(input_layer, decoded)
    encoder = Model(input_layer, encoded)
    autoencoder.compile(optimizer='adam', loss='mse')
    return autoencoder, encoder

def train_autoencoder(data, number_cols):
    scaler = StandardScaler()
    X = scaler.fit_transform(data[number_cols].values)
    # 保存下来供预测时使用
    X_train, X_val = train_test_split(X, test_size=0.2, random_state=42)
    autoencoder, encoder = build_autoencoder(input_dim=X.shape[1])
    autoencoder.fit(X_train, X_train, epochs=10, batch_size=128, shuffle=True,
                    validation_data=(X_val, X_val), verbose=1)
    return encoder

def recommend_with_autoencoder(favorite_songs, data, encoder, number_cols, n_songs=10):
    song_dict = flatten_dict_list(favorite_songs)
    X_all = data[number_cols].values
    song_center = get_mean_vector(favorite_songs, data)
    song_center = np.asarray(song_center, dtype=np.float32).reshape(1, -1)
    song_encoded_all = encoder.predict(X_all)
    song_encoded_center = encoder.predict(song_center.reshape(1, -1))
    distances = cdist(song_encoded_center, song_encoded_all, 'cosine')[0]
    index = np.argsort(distances)[:n_songs]
    rec_songs = data.iloc[index]
    rec_songs = rec_songs[~rec_songs['name'].isin(song_dict['name'])]
    metadata_cols = ['name', 'year', 'artists'] if 'artists' in data.columns else ['name', 'year']
    return rec_songs[metadata_cols].to_dict(orient='records')

# === Collaborative Filtering (Pandas Implementation) ===
def recommend_cf_with_pandas(user_data, target_user, candidate_songs=None, top_n=10,
                             alpha=0.5, beta=0.3, gamma=0.2):
    # Step 1: 构建评分
    df = user_data.copy()
    if 'rating' not in df.columns:
        df['rating_score'] = (
            alpha * df['play_ratio'] +
            beta * np.log1p(df['play_count']) +
            gamma * (1 - df['skipped'])
        )
    else:
        df['rating_score'] = df['rating']

    # Step 2: 构建用户-物品矩阵
    user_item_matrix = df.pivot_table(index='user_id', columns='song_name', values='rating_score')

    # Step 3: 计算相似度 + 预测评分
    item_similarity = pd.DataFrame(
        cosine_similarity(user_item_matrix.T.fillna(0)),
        index=user_item_matrix.columns,
        columns=user_item_matrix.columns
    )

    user_ratings = user_item_matrix.loc[target_user].dropna()
    scores = item_similarity[user_ratings.index].dot(user_ratings)
    scores = scores / (item_similarity[user_ratings.index].sum() + 1e-8)

    # ✅ 限制只对候选歌曲评分
    if candidate_songs is not None:
        scores = scores[scores.index.isin(candidate_songs)]
    # 不要 .head(top_n)
    return scores.sort_values(ascending=False)



# # === Main ===
# if __name__ == "__main__":
#     favorite_songs = [
#         {'name': '夢一場', 'year': 1999},
#         {'name': 'Love Story', 'year': 2008},
#         {'name': 'Someone Like You', 'year': 2011},
#     ]

#     print("\n🎧 [方式1] KMeans + Cosine 推荐结果：\n")
#     recommendations = recommend_songs(favorite_songs, data, n_songs=10)
#     for i, rec in enumerate(recommendations, 1):
#         print(f"{i}. {rec['name']} ({rec['year']})")

#     print("\n🤖 [方式2] Autoencoder 推荐结果：\n")
#     encoder = train_autoencoder(data, number_cols)
#     ae_recommendations = recommend_with_autoencoder(favorite_songs, data, encoder, number_cols, n_songs=10)
#     for i, rec in enumerate(ae_recommendations, 1):
#         print(f"{i}. {rec['name']} ({rec['year']})")

#     print("\n🧠 [方式3] 协同过滤推荐结果（pandas实现）：\n")
#     test_user = user_data['user_id'].unique()[0]
#     cf_scores = recommend_cf_with_pandas(user_data, test_user, top_n=10)
#     for i, (song, score) in enumerate(cf_scores.items(), 1):
#         print(f"{i}. {song} (预测评分: {round(score, 2)})")

#     show_statistics(data)
#     visualize_song_clusters(data, number_cols)
#     visualize_genre_tsne(genre_data)

# === 改进版：融合推荐系统核心函数 ===
def hybrid_recommendation(favorite_songs, spotify_data, user_data, encoder, number_cols, target_user, top_n=10):
    print("[Step 1] 正在进行内容推荐 (KMeans + Cosine)...")
    # --- Step 1: 内容推荐候选（KMeans + cosine） ---
    song_dict = flatten_dict_list(favorite_songs)
    song_center = get_mean_vector(favorite_songs, spotify_data)
    scaler = song_cluster_pipeline.steps[0][1]
    scaled_data = scaler.transform(spotify_data[number_cols])
    scaled_center = scaler.transform(song_center.reshape(1, -1))
    cosine_distances = cdist(scaled_center, scaled_data, 'cosine')[0]
    top_k_idx = np.argsort(cosine_distances)[:100]  # 初步候选
    candidates = spotify_data.iloc[top_k_idx].copy()
    candidates = candidates[~candidates['name'].isin(song_dict['name'])].reset_index(drop=True)

    print("[Step 2] 正在进行 Autoencoder 筛选...")
    # --- Step 2: Autoencoder 精细筛选 ---
    X_cand = candidates[number_cols].values
    cand_encoded = encoder.predict(X_cand)
    center_encoded = encoder.predict(song_center.reshape(1, -1))
    ae_distances = cdist(center_encoded, cand_encoded, 'cosine')[0]
    ae_top_idx = np.argsort(ae_distances)[:top_n * 3]  # 再次筛选
    refined_candidates = candidates.iloc[ae_top_idx].copy().reset_index(drop=True)

    print("[Step 3] 正在进行协同过滤打分...")
    # --- Step 3: 协同过滤打分排序 ---
    behavior_data = user_data.copy()
    if {'play_count', 'play_ratio', 'skipped'}.issubset(user_data.columns):
        user_data['rating'] = (
            0.5 * user_data['play_ratio'] +
            0.3 * MinMaxScaler().fit_transform(user_data[['play_count']])[:, 0] +
            -0.3 * user_data['skipped'] +
            np.random.uniform(0, 0.3, size=len(user_data))
        ) * 5
        user_data['rating'] = user_data['rating'].clip(0, 5).round(2)

    pivot = user_data.pivot_table(index='user_id', columns='song_name', values='rating')
    cf_scores = []
    print("[Step 3.1] 正在为候选歌曲计算协同过滤得分：")
    for song in tqdm(refined_candidates['name']):
        if song not in pivot.columns:
            cf_scores.append(0)
            continue
        item_sim = cosine_similarity(pivot.fillna(0).T)[pivot.columns.get_loc(song)]
        target_ratings = pivot.loc[target_user].fillna(0).values
        score = np.dot(item_sim, target_ratings) / (np.sum(item_sim) + 1e-8)
        cf_scores.append(score)

    refined_candidates['cf_score'] = cf_scores
    refined_candidates = refined_candidates.sort_values(by='cf_score', ascending=False).head(top_n)

    metadata_cols = ['name', 'year', 'artists'] if 'artists' in refined_candidates.columns else ['name', 'year']
    refined_candidates['cf_score'] = refined_candidates['cf_score'].round(3)
    print("[Done] 推荐完成！")
    return refined_candidates[metadata_cols + ['cf_score']].to_dict(orient='records')

def hybrid_recommendation(favorite_songs, data, user_data, encoder, number_cols,
                          target_user, top_n=10):
    # ========== Stage 1: KMeans 初筛 ==========
    print("🔍 [Stage 1] 基于 KMeans 聚类筛选候选歌曲...")
    song_center = get_mean_vector(favorite_songs, data)
    cluster_model = song_cluster_pipeline.steps[1][1]
    scaler = song_cluster_pipeline.steps[0][1]
    scaled_song_center = scaler.transform(song_center.reshape(1, -1)).astype(np.float64)
    cluster_label = cluster_model.predict(scaled_song_center)[0]
    candidate_data = data[data['cluster_label'] == cluster_label].copy()

    print(f"🎯 初步聚类选择 cluster {cluster_label}，候选数量: {len(candidate_data)}")

    # ========== Stage 2: Autoencoder 精筛 ==========
    print("🧠 [Stage 2] 使用 Autoencoder 进行相似性排序...")
    try:
        song_encoded_all = encoder.predict(candidate_data[number_cols].values)
        song_center_encoded = encoder.predict(song_center.reshape(1, -1))
    except Exception as e:
        print(f"❌ Autoencoder 编码失败: {e}")
        return []

    distances = cdist(song_center_encoded, song_encoded_all, 'cosine')[0]
    candidate_data['ae_score'] = 1 - distances  # 越高越相似
    candidate_data = candidate_data.sort_values(by='ae_score', ascending=False).head(top_n * 3)

    # ✅ 打印前几条候选歌曲信息（包含 ae_score）
    print("\n🎵 [AE精筛后候选歌曲 Top5 示例]：")
    print(candidate_data.head())

    # ========== Stage 3: 融合排序推荐 ==========

    print("👥 [Stage 3] 使用协同过滤进行评分排序...")
    df_cf_input = user_data.copy()
    song_names = candidate_data['name'].unique()

    # CF预测所有歌曲得分（不限制top_n）
    cf_scores = recommend_cf_with_pandas(df_cf_input, target_user)

    # 合并CF得分
    candidate_data['cf_score'] = candidate_data['name'].map(cf_scores)
    if candidate_data['cf_score'].isna().all():
        # print("⚠️ CF完全失效，CF得分设为0")
        candidate_data['cf_score'] = 0
    else:
        candidate_data['cf_score'] = candidate_data['cf_score'].fillna(0)
        # 缩放到0~1范围（可选）
        from sklearn.preprocessing import MinMaxScaler
        candidate_data['cf_score'] = MinMaxScaler().fit_transform(candidate_data[['cf_score']])

    # === 聚类得分：是否和中心在同一cluster ===
    center_cluster = song_cluster_pipeline.steps[1][1].predict(
        song_cluster_pipeline.steps[0][1].transform(song_center.reshape(1, -1)).astype(np.float64)
)[0]
    candidate_data['kmeans_score'] = (candidate_data['cluster_label'] == center_cluster).astype(int)

    # === 加权融合得分 ===
    w1, w2, w3 = 0.2, 0.4, 0.4
    candidate_data['final_score'] = (
        w1 * candidate_data['kmeans_score'] +
        w2 * candidate_data['ae_score'] +
        w3 * candidate_data['cf_score']
    )

    # 输出 top-N
    final_rec = candidate_data.sort_values(by='final_score', ascending=False).head(top_n)
    metadata_cols = ['name', 'year', 'artists'] if 'artists' in final_rec.columns else ['name', 'year']
    return final_rec[metadata_cols + ['final_score']].to_dict(orient='records')

if __name__ == "__main__":
    favorite_songs = [
        {'name': '夢一場', 'year': 1999},
        {'name': 'Love Story', 'year': 2008},
        {'name': 'Someone Like You', 'year': 2011},
    ]

    print("[启动] 正在训练 Autoencoder 编码器...")
    encoder = train_autoencoder(data, number_cols)
    target_user = user_data['user_id'].unique()[0]

    print("\n🎯 [融合推荐结果]：\n")
    hybrid_results = hybrid_recommendation(
        favorite_songs, data, user_data, encoder, number_cols, target_user, top_n=10
    )

    for i, rec in enumerate(hybrid_results, 1):
        print(f"{i}. {rec['name']} ({rec['year']}) - 融合得分: {rec['final_score']:.4f}")
