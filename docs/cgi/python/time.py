#!/usr/bin/python3

print("Content-Type: text/plain; charset=utf-8\n")

# 日時用モジュールをインポート
from datetime import datetime
# Unixタイムスタンプ用モジュールをインポート
import time

# 現在日時のオブジェクトを生成
now = datetime.now()

# 日時を文字列で表示
print(now.strftime('%Y年%m月%d日 %H時%M分%S秒'))

# Unixタイムスタンプを取得して秒単位に
# 1日6時間の訓練を1万時間続けた場合の秒数を足す
t = int(time.time()) + 10000 * 3600 * (24/6)

# Unixタイムスタンプから日時を文字列で表示
print(datetime.fromtimestamp(t).strftime('%Y年%m月%d日 %H時%M分%S秒'))
