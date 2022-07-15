#! /usr/bin/python3
import os    # 既存ファイルの有無を確認するため
import sys    # exit()するため
import cgi    # CGIのデータを扱うため
import cgitb    # CGIのデバッグのため

path = '/Users/nakahodojunya/42/lvl_05/webserv/docs/upload/'    # アップロードされたファイルを保存するディレクトリ
   # セキュリティ上、webサーバからアクセス可能なHTMLやCGIを置く場所とは別にすることが強く推奨される
MEGA = 1048576    # 一度にアップロードするデータサイズ


def print_header():    # HTMLヘッダを出力
  print('Content-Type: text/html')
  print('')
  print('<!DOCTYPE html>')
  print('')
  print('<html lang="ja">')
  print('')
  print('<head>')
  print('  <meta charset="UTF-8">')
  print('  <meta name="description" content="file upload">')
  print('  <meta name="keywords" content="upload">')
  print('  <title>File upload</title>')
  print('</head>')
  print('')
  print('<body>')


def print_footer():    # HTMLフッタを出力しCGIを終了
  print('</body>')
  print('')
  print('</html>')
  sys.exit(0)


cgi.maxlen = 400000000    # 受け付ける最大のデータサイズを制限
cgitb.enable()    # デバッグ目的で使用するcgitbを有効に

print_header()    # ヘッダを出力

form_data = cgi.FieldStorage()    # HTMLのフォームからデータを受け取る準備

file_name = form_data['foobar'].filename    # ファイル名を取得
full_path = path + file_name    # ファイル名にパスを付加
if os.path.exists(full_path):    # 上書き確認
  print(file_name + ' already exists.<br>')
  print_footer()    # 同名ファイルが存在する場合はアップロードせずに終了

uploaded_file = open(full_path, 'wb')    # アップロードされたデータを保存する新規ファイルを同名で作成
item = form_data['foobar']
while True:
  chunk = item.file.read(MEGA)
  if not chunk:
    break
  uploaded_file.write(chunk)
uploaded_file.close()
print(file_name + ' has just been uploaded.<br>')    # アップロードされたことを表示
print('<a href="/upload.html">戻る</a>')

print_footer()    # HTMLフッタを表示して終了