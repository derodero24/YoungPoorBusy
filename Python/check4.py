
import sys
sys.stdin = open('../input.txt')

# MAXCITY = 100
# MAXCOUNT = 2000 + 1
INF = 999999
BIAS = 24 * 60
city_name = {}
trains = [[], []]  # trains, rtrains
from_to = [[], [], [], []]  # fr_h, fr_t, to_h, to_t
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

# trains[0]の用意


def parse_connection(buf):
    global INF, ncity, nconn, start, end

    dpt = [0, 0]
    arv = [0, 0]
    dpt[0], dpt[1] = map(int, buf[1].split(":"))
    arv[0], arv[1] = map(int, buf[3].split(":"))
    if (dpt[0] * 60 + dpt[1] < start) or (arv[0] * 60 + arv[1] > end):
        return

#     print(dpt[0]," ",dpt[1]," ",arv[0]," ",arv[1])

    # リスト内に辞書を追加
    trains[0].append({})
    trains[0][nconn]["from"] = city_id(buf[0])
    trains[0][nconn]["dpt"] = dpt[0] * 60 + dpt[1]
    trains[0][nconn]["to"] = city_id(buf[2])
    trains[0][nconn]["arv"] = arv[0] * 60 + arv[1]
    trains[0][nconn]["fare"] = int(buf[4])
    nconn += 1

# trains[1]の用意，ソート


def prepare_data():
    global INF, ncity, nconn, BIAS

    for i in range(0, nconn):
        trains[1].append({})
        trains[1][i]["from"] = trains[0][i]["to"]
        trains[1][i]["to"] = trains[0][i]["from"]
        trains[1][i]["dpt"] = BIAS - trains[0][i]["arv"]
        trains[1][i]["arv"] = BIAS - trains[0][i]["dpt"]
        trains[1][i]["fare"] = trains[0][i]["fare"]

    # "arv"をキーにしてソート
    trains[0].sort(key=lambda x: x["arv"])
    trains[1].sort(key=lambda x: x["arv"])

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


def calc_cost(city):
    global INF, ncity, nconn, BIAS, meetTime

    a = 0
    d = nconn - 1
    min_c = INF

    while True:
        # 面会時間
        stay = (BIAS - trains[1][d]["arv"]) - trains[0][a]["arv"]
        if stay < meetTime:
            d -= 1
            if d < 0:
                break
            continue

        c = 0
        for i in range(0, 2):
            c += from_to[i][city][a + 1]
        for i in range(0, 2):
            c += from_to[i + 2][city][d + 1]

        if min_c > c:
            min_c = c
        a += 1
        if a >= nconn:
            break

    return min_c

# main内のもろもろの処理


def solve():
    global INF, ncity, nconn

    prepare_data()
    for i in range(0, 4):
        from_to[i].clear()
        make_tbl(from_to[i], city_id(station[i % 2]), trains[int(i / 2)])

    min_cost = INF
    for c in range(0, ncity):
        cost = calc_cost(c)
        if min_cost > cost:
            min_cost = cost

    if min_cost >= INF:
        min_cost = 0

    return min_cost

# 表出力


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
            print(trains[int(num / 2)][i - 1]["arv"], end="")
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
        for i in range(0, 2):
            trains[i].clear()
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

        # main内のもろもろの処理
        minc = solve()

        # 表出力
        for i in range(0, 4):
            print_tbl(i)

        print("Answer:", minc, "\n\n")
