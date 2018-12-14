
import sys
sys.stdin = open('../input.txt')

# MAXCITY = 100
# MAXCOUNT = 2000 + 1
city_name = {}
trains = []
ncity = 0
nconn = 0

# 駅名 -> 駅番号
def city_id(name):
    global ncity, nconn

    # keyが存在すればvalueを返す
    if name in city_name:
        return city_name[name]

    # 要素を追加
    city_name[name] = ncity
    ncity += 1
    return ncity - 1

# 列車追加
def parse_connection(buf):
    global ncity, nconn

    # リスト内に辞書を追加
    trains.append({})
    trains[nconn]["from"] = city_id(buf[0])
    h,m = map(int, buf[1].split(":"))
    trains[nconn]["dpt"] = h*60 + m
    trains[nconn]["to"] = city_id(buf[2])
    h,m = map(int, buf[3].split(":"))
    trains[nconn]["arv"] = h*60 + m
    trains[nconn]["fare"] = int(buf[4])
    nconn += 1

if __name__ == '__main__':
    while True:
        ncity = 0
        nconn = 0
        trains.clear()
        city_name.clear()
        maxFare = 0
        maxTime = 0

        # 列車数読み込み
        count = int(input())
        print(count)

        # 終了条件
        if count == 0: break

        # 列車読み込み
        for i in range(0,count):
            buf = input().split()
            parse_connection(buf)

        # 最大値チェック
        for i in range(0,nconn):
            if maxFare < trains[i]["fare"]:
                maxFare = trains[i]["fare"]
            time = trains[i]["arv"] - trains[i]["dpt"]
            if maxTime < time:
                maxTime = time

        # 最大値及びその列車番号出力
        print("Max fare: ",maxFare," yen Max fare id: ",end = "")
        for i in range(0,nconn):
            if trains[i]["fare"] == maxFare:
                print(i," ",end = "")
        print()
        print("Max time: ",maxTime," yen Max time id: ",end = "")
        for i in range(0,nconn):
            if trains[i]["arv"] - trains[i]["dpt"] == maxTime:
                print(i," ",end = "")
        print()

        # 列車情報出力
        for i in range(0,nconn):
            for j in trains[i]:
                print(trains[i][j]," ",end = "")
            print()

        print()
