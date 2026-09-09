import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import plotly.express as px

from tensorflow.keras.models import Model # type: ignore
from tensorflow.keras.layers import Input, Dense # type: ignore

from sklearn.model_selection import train_test_split
from sklearn.cluster import KMeans
from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import Pipeline
from sklearn.manifold import TSNE
from sklearn.decomposition import PCA
from sklearn.metrics import euclidean_distances
from scipy.spatial.distance import cdist
from collections import defaultdict

import warnings
warnings.filterwarnings("ignore")

# === Load Data ===
data = pd.read_csv("data.csv")
genre_data = pd.read_csv("data_by_genres.csv")
year_data = pd.read_csv("data_by_year.csv")
user_data = pd.read_csv("user_data.csv")

# === Genre Clustering ===
cluster_pipeline = Pipeline([
    ('scaler', StandardScaler()),
    ('kmeans', KMeans(n_clusters=10))
])
X_genre = genre_data.select_dtypes(np.number)
cluster_pipeline.fit(X_genre)
genre_data['cluster'] = cluster_pipeline.predict(X_genre)

# # === Song Clustering ===
# song_cluster_pipeline = Pipeline([
#     ('scaler', StandardScaler()),
#     ('kmeans', KMeans(n_clusters=20))
# ])
# X_song = data.select_dtypes(np.number)
# number_cols = list(X_song.columns)
# song_cluster_pipeline.fit(X_song)
# data['cluster_label'] = song_cluster_pipeline.predict(X_song)

# === Song Clustering with Clean Features ===

# 强制去除不该用于建模的列
excluded_cols = ['name', 'artists', 'id', 'release_date', 'explicit']
X_song = data.drop(columns=excluded_cols, errors='ignore')

# 只保留数值型数据，并移除所有列名中的空格
X_song = X_song.select_dtypes(include=[np.number])
X_song.columns = X_song.columns.str.strip()

# 更新用于建模的列名
number_cols = list(X_song.columns)
print("✅ 使用的特征列:", number_cols)

# 重新训练聚类模型
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

# def get_mean_vector(song_list, spotify_data):
#     song_vectors = []
#     for song in song_list:
#         song_data = get_song_data(song, spotify_data)
#         if song_data is None:
#             continue
#         song_vector = song_data[number_cols].values
#         song_vectors.append(song_vector)
#     song_matrix = np.array(song_vectors)
#     return np.mean(song_matrix, axis=0)

def get_mean_vector(song_list, spotify_data):
    song_vectors = []
    for song in song_list:
        song_data = get_song_data(song, spotify_data)
        if song_data is None:
            continue
        # 只选择数值列 + dropna + float32
        song_vector = song_data[number_cols].dropna().to_numpy(dtype=np.float32)
        song_vectors.append(song_vector)

    if not song_vectors:
        raise ValueError("❌ 没有找到任何有效歌曲")

    song_matrix = np.vstack(song_vectors)  # 合并成 2D 数组
    mean_vector = np.mean(song_matrix, axis=0)

    # ✅ DEBUG 打印
    print("✅ song_matrix shape:", song_matrix.shape)
    print("✅ mean_vector dtype:", mean_vector.dtype)
    print("✅ mean_vector shape:", mean_vector.shape)

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


############# Autoencoder ########################
def build_autoencoder(input_dim, encoding_dim=16):
    input_layer = Input(shape=(input_dim,))
    encoded = Dense(64, activation='relu')(input_layer)
    encoded = Dense(32, activation='relu')(encoded)
    encoded = Dense(encoding_dim, activation='relu')(encoded)

    decoded = Dense(32, activation='relu')(encoded)
    decoded = Dense(64, activation='relu')(decoded)
    decoded = Dense(input_dim, activation='linear')(decoded)

    autoencoder = Model(input_layer, decoded)
    encoder = Model(input_layer, encoded)
    autoencoder.compile(optimizer='adam', loss='mse')
    return autoencoder, encoder

def train_autoencoder(data, number_cols):
    X = data[number_cols].values
    X_train, X_val = train_test_split(X, test_size=0.2, random_state=42)
    autoencoder, encoder = build_autoencoder(input_dim=X.shape[1])
    autoencoder.fit(X_train, X_train,
                    epochs=50,
                    batch_size=128,
                    shuffle=True,
                    validation_data=(X_val, X_val),
                    verbose=1)
    return encoder

def recommend_with_autoencoder(favorite_songs, data, encoder, number_cols, n_songs=10):
    song_dict = flatten_dict_list(favorite_songs)
    X_all = data[number_cols].values
    song_center = get_mean_vector(favorite_songs, data)
    song_center = np.asarray(song_center, dtype=np.float32).reshape(1, -1)  # 转换为 2D 数组
    print("🎯 song_center dtype:", song_center.dtype, "shape:", song_center.shape)

    song_encoded_all = encoder.predict(X_all)
    song_encoded_center = encoder.predict(song_center.reshape(1, -1))
    distances = cdist(song_encoded_center, song_encoded_all, 'cosine')[0]

    index = np.argsort(distances)[:n_songs]
    rec_songs = data.iloc[index]
    rec_songs = rec_songs[~rec_songs['name'].isin(song_dict['name'])]
    metadata_cols = ['name', 'year', 'artists'] if 'artists' in data.columns else ['name', 'year']
    return rec_songs[metadata_cols].to_dict(orient='records')
########################



# === Example Usage ===
if __name__ == "__main__":
    favorite_songs = [
        {'name': '夢一場', 'year': 1999},
        {'name': 'Love Story', 'year': 2008},
        {'name': 'Someone Like You', 'year': 2011},
    ]

    print("\n🎧 [方式1] KMeans + Cosine 推荐结果：\n")
    recommendations = recommend_songs(favorite_songs, data, n_songs=10)
    for i, rec in enumerate(recommendations, 1):
        print(f"{i}. {rec['name']} ({rec['year']})")

    print("\n🤖 [方式2] Autoencoder 推荐结果：\n")
    encoder = train_autoencoder(data, number_cols)
    ae_recommendations = recommend_with_autoencoder(favorite_songs, data, encoder, number_cols, n_songs=10)
    for i, rec in enumerate(ae_recommendations, 1):
        print(f"{i}. {rec['name']} ({rec['year']})")



    # 展示统计信息
    show_statistics(data)

    # 展示 PCA 分布图（歌曲）
    visualize_song_clusters(data, number_cols)

    # 展示 t-SNE 分布图（流派）
    visualize_genre_tsne(genre_data)