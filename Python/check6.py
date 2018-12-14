
import sys
sys.stdin = open('../input.txt')

# MAXCITY = 100
# MAXCOUNT = 2000 + 1
INF = 999999
BIAS = 24*60
city_name = {}
trains = [[],[]] # trains, rtrains
from_to = [[],[],[],[]] # fr_h, fr_t, to_h, to_t
ncity = 0
nconn = 0

def m2hm(m, hm):
    hm[0] = int((m / 60))
    hm[1] = m - (60 * hm[0])

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

    dpt = [0,0]; arv = [0,0]
    dpt[0],dpt[1] = map(int, buf[1].split(":"))
    arv[0],arv[1] = map(int, buf[3].split(":"))
    if (dpt[0]*60 + dpt[1] < start) or (arv[0]*60 + arv[1] > end):
        return

#     print(dpt[0]," ",dpt[1]," ",arv[0]," ",arv[1])

    # リスト内に辞書を追加
    trains[0].append({})
    trains[0][nconn]["from"] = city_id(buf[0])
    trains[0][nconn]["dpt"] = dpt[0]*60 + dpt[1]
    trains[0][nconn]["to"] = city_id(buf[2])
    trains[0][nconn]["arv"] = arv[0]*60 + arv[1]
    trains[0][nconn]["fare"] = int(buf[4])
    nconn += 1

# trains[1]の用意，ソート
def prepare_data():
    global INF, ncity, nconn, BIAS

    for i in range(0,nconn):
        trains[1].append({})
        trains[1][i]["from"] = trains[0][i]["to"]
        trains[1][i]["to"] = trains[0][i]["from"]
        trains[1][i]["dpt"] = BIAS - trains[0][i]["arv"]
        trains[1][i]["arv"] = BIAS - trains[0][i]["dpt"]
        trains[1][i]["fare"] = trains[0][i]["fare"]

    # "arv"をキーにしてソート
    trains[0].sort(key=lambda x:x["arv"])
    trains[1].sort(key=lambda x:x["arv"])

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
    for i in range(0,ncity):
        tbl.append([])
        for j in range(0,nconn+1):
            tbl[i].append([INF,0,0])
#             tbl[i][0].append(INF)
    tbl[org][0][0] = 0

    # 列車の数だけ更新
    for ti in range(0,nconn):
        # 前の状態を引き継ぐ
        for i in range(0,ncity):
            for j in range(0,3):
                tbl[i][ti+1][j] = tbl[i][ti][j]

        # 列車配列tvの中で，tv[ti].dpt以前に駅tv[ti].fromに到着する列車を探す
        # つまり，列車tv[ti]に乗り継げる列車を探す。
        # ti-1: 探索を開始する列車到着事象の番号
        a = change(tr, ti-1, tr[ti]["from"], tr[ti]["dpt"])

#         #安い方を引き継ぐ
        if tbl[tr[ti]["to"]][ti][0] > tr[ti]["fare"] + tbl[tr[ti]["from"]][a+1][0]:
            tbl[tr[ti]["to"]][ti+1][0] = tr[ti]["fare"] + tbl[tr[ti]["from"]][a+1][0];
            tbl[tr[ti]["to"]][ti+1][1] = tr[ti]["dpt"];
            tbl[tr[ti]["to"]][ti+1][2] = tr[ti]["arv"];
#         else :
#             tbl[tr[ti]["to"]][ti+1][0] = tbl[tr[ti]["to"]][ti][0]

def calc_cost(city, minc):
    global INF, ncity, nconn, BIAS, meetTime

    a = 0
    d = nconn - 1
    minc[0] = INF

    while True:
        # 面会時間
        stay = (BIAS - trains[1][d]["arv"]) - trains[0][a]["arv"]
        if stay < meetTime:
            d -= 1
            if d < 0: break
            continue

        c = 0
        for i in range(0,2): c += from_to[i][city][a+1][0]
        for i in range(0,2): c += from_to[i+2][city][d+1][0]

        if minc[0] > c:
            minc[0] = c
            minc[1] = max(from_to[0][city][a+1][2],from_to[1][city][a+1][2])
            minc[2] = BIAS - max(from_to[2][city][d+1][2],from_to[3][city][d+1][2])

        a += 1
        if a >= nconn: break

# main内のもろもろの処理
def solve(minc):
    global INF, ncity, nconn

    prepare_data();
    for i in range(0,4):
        from_to[i].clear()
        make_tbl(from_to[i], city_id(station[i%2]), trains[int(i/2)])

    minc[0] = INF
    for c in range(0,ncity):
        tmp = [0,0,0]
        calc_cost(c,tmp)
        if minc[0] > tmp[0]:
            for i in range(0,3): minc[i] = tmp[i]
            minc[3] = c

        # チェック4のテスト用
#         for name in city_name:
#             if city_name[name] == c:
#                 print(name,": ",tmp[0])

    if minc[0] >= INF: minc[0] = 0

# 表出力
def print_tbl(num):
    global INF, ncity, nconn, station

    print("From " if num < 2 else "To ",end = "")
    print(station[num%2],":\ni\ttime|",end = "")

    for i in range(0,ncity): print(i,"\t",end = "")
    print("\n----------------+------------------------")
    for i in range(0,nconn+1):
        print(i-1,"\t",end = "")
        if i: print(trains[int(num/2)][i-1]["arv"],end = "")
        print("\t|",end = "")
        for j in range(0,ncity):
            for k in range(0,3):
                print(from_to[num][j][i][k],"\t",end = "")
        print()

    print()

if __name__ == '__main__':

    # 実行引数格納
    argv = sys.argv
    if len(argv) != 6:
        print("argment error")
        sys.exit()

    station = []
    for i in range(0,2): station.append(argv[i+1])
    h,m = map(int, argv[3].split(":"))
    start = h*60 + m
    h,m = map(int, argv[4].split(":"))
    end = h*60 + m
    meetTime = int(argv[5]);

    while True:
        # 変数群のリセット
        ncity = 0
        nconn = 0
        for i in range(0,2): trains[i].clear()
        city_name.clear()

        # 列車数読み込み
        count = int(input())
#         print(count)

        # 終了条件
        if count == 0: break

        # 列車読み込み
        for i in range(0,count):
            buf = input().split()
            parse_connection(buf)

        # main内のもろもろの処理
        minc = [0,0,0,0]
        solve(minc)

        # 表出力
#         for i in range(0,4): print_tbl(i)

        print(minc[0]," ",end = "")
        if minc[0] :
            for name in city_name:
                if city_name[name] == minc[3]:
                    print(name," ",end = "")
            hm = [0,0]
            m2hm(minc[1], hm)
            print("{0:02d}:{1:02d} - ".format(hm[0],hm[1]),end = "")
            m2hm(minc[2], hm)
            print("{0:02d}:{1:02d} ".format(hm[0],hm[1]),end = "")

        print()
