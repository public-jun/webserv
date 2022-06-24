#!/usr/bin/python3

print("Content-Type: text/plain; charset=utf-8\n")

# 消費税
tax = 1.08
# 所持金
wallet = 10000
# 人数
num = 5

# 商品価格A
priceA = 500
# 商品価格B
priceB = 700

# 購入金額
total = (priceA * 7 + priceB * 4) * tax

print("購入金額：{0:.0f}円".format(total));
print("残高：{0:.0f}円".format(wallet - total))
print("1人当たり：{0:.0f}円".format(total // num))
print("購入者負担：{0:.0f}円".format(total % num))
