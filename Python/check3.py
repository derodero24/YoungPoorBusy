
import sys
sys.stdin = open('../input.txt')

# MAXCITY = 100
# MAXCOUNT = 2000 + 1
INF = 999999
city_name = {}
trains = []
from_to = [[], []]  # fr_h, fr_t
ncity = 0
nconn = 0

station = ["Hakodate", "Tokyo"]
start = 8 * 60
end = 18 * 60
meetTime = 30

# 駅名 -> 駅番号


def city_id(name):
    global INF, ncity, nconn

    # keyが存在すればvalueを返す
    if name in city_name:
        return city_name[name]

    # 要素を追加
    city_name[name] = ncity
    ncity += 1
    return ncity - 1

# 列車追加


def parse_connection(buf):
    global INF, ncity, nconn

    # リスト内に辞書を追加
    trains.append({})
    trains[nconn]["from"] = city_id(buf[0])
    h, m = map(int, buf[1].split(":"))
    trains[nconn]["dpt"] = h * 60 + m
    trains[nconn]["to"] = city_id(buf[2])
    h, m = map(int, buf[3].split(":"))
    trains[nconn]["arv"] = h * 60 + m
    trains[nconn]["fare"] = int(buf[4])
    nconn += 1

# 列車配列tvの中で，dpttime以前に駅stに到着する列車を探す
# p: 探索を開始する列車到着事象の番号


def change(tr, p, st, dptTime):
    global INF, ncity, nconn

    while p >= 0:
        if (tr[p]["to"] == st) and (tr[p]["arv"] <= dptTime):
            break
        p -= 1
    return p

# 列車配列tvを参照して，駅orgを起点として，
# fr_h または fr_t を二次元配列vに作成する。


def make_tbl(tbl, org, tr):
    global INF, ncity, nconn

    # tblの初期化
    for i in range(0, ncity):
        tbl.append([])
        for _ in range(0, nconn + 1):
            tbl[i].append(INF)
    tbl[org][0] = 0

    # 列車の数だけ更新
    for ti in range(0, nconn):
        # 前の状態を引き継ぐ
        for i in range(0, ncity):
            tbl[i][ti + 1] = tbl[i][ti]

        # 列車配列tvの中で，tv[ti].dpt以前に駅tv[ti].fromに到着する列車を探す
        # つまり，列車tv[ti]に乗り継げる列車を探す。
        # ti-1: 探索を開始する列車到着事象の番号
        a = change(tr, ti - 1, tr[ti]["from"], tr[ti]["dpt"])

        # 安い方を引き継ぐ
        tbl[tr[ti]["to"]][ti + 1] = \
            min(tbl[tr[ti]["to"]][ti], tr[ti]
                ["fare"] + tbl[tr[ti]["from"]][a + 1])


def print_tbl(num):
    global INF, ncity, nconn, station

    print("From " if num < 2 else "To ", end="")
    print(station[num % 2], ":\ni\ttime|", end="")

    for i in range(0, ncity):
        print(i, "\t", end="")
    print("\n----------------+------------------------")
    for i in range(0, nconn + 1):
        print(i - 1, "\t", end="")
        if i:
            print(trains[i - 1]["arv"], end="")
        print("\t|", end="")
        for j in range(0, ncity):
            print(from_to[num][j][i], "\t", end="")
        print()

    print()


if __name__ == '__main__':
    while True:
        # 変数群のリセット
        ncity = 0
        nconn = 0
        trains.clear()
        city_name.clear()

        # 列車数読み込み
        count = int(input())
        print(count)

        # 終了条件
        if count == 0:
            break

        # 列車読み込み
        for i in range(0, count):
            buf = input().split()
            parse_connection(buf)

#         # 列車情報ソート前
#         print("Before:")
#         for i in range(0,nconn):
#             for j in trains[i]:
#                 print(trains[i][j]," ",end = "")
#             print()

        # "arv"をキーにしてソート
        trains.sort(key=lambda x: x["arv"])

#         # 列車情報ソート後
#         print("After:")
#         for i in range(0,nconn):
#             for j in trains[i]:
#                 print(trains[i][j]," ",end = "")
#             print()
#         print()

        # 表作成, 出力
        for i in range(0, len(from_to)):
            from_to[i].clear()
            make_tbl(from_to[i], city_id(station[i]), trains)
            print_tbl(i)

        print()
