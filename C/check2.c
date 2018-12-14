
#include <stdio.h>
#include <string.h>

#define FOR(i,a,b) for(int i=a;i<b;i++)
#define REP(i,n)  FOR(i,0,n)

#define MAXCITY 100
#define MAXCOUNT 2000 + 1
char city_name[MAXCITY][18]; //駅名の表
int ncity; //駅数
int nconn; //列車数

// 列車配列
struct train {
	int from, to;
	int dpt, arv;
	int fare;
} trains[MAXCOUNT];

//駅名 -> 駅番号
int city_id(char *name) {
	FOR(i,0,ncity) {
		if(strcmp(name, &(city_name[i][0])) == 0) return i;
	}
	strcpy(&(city_name[ncity][0]), name);
	return ncity++;
}

// 列車追加
void parse_connection(char *buf) {
	char from[18], to[18];
	int dpt[2], arv[2], fare;

	sscanf(buf,"%s %d:%d %s %d:%d %d",
			from, &(dpt[0]), &(dpt[1]), to, &(arv[0]), &(arv[1]), &fare);

	trains[nconn].from = city_id(from);
	trains[nconn].to = city_id(to);
	trains[nconn].dpt = dpt[0]*60 + dpt[1];
	trains[nconn].arv = arv[0]*60 + arv[1];
	trains[nconn].fare = fare;
	nconn++;
}

int main() {
	int count, maxFare, maxTime, time;
	char buf[64];

	while(fgets(buf, sizeof(buf), stdin) != NULL) {
		// パラメータの初期化
		nconn = 0;
		ncity = 0;
		maxFare = 0;
		maxTime = 0;

		// 入力行数取得
		sscanf(buf, "%d", &count);
		printf("%d\n", count);

		// 0行なら終了
		if(!count) break;

		// 列車の追加
		REP(i,count) {
			if(fgets(buf, sizeof(buf), stdin) == NULL) break;
			parse_connection(buf);
		}

		//最大値チェック
		REP(i,count) {
			if(maxFare < trains[i].fare) maxFare = trains[i].fare;
			time = trains[i].arv - trains[i].dpt;
			if(maxTime < time) maxTime = time;
		}

		// 最大値の列車出力
		printf("Max fare: %d yen Max fare id: ",maxFare);
		REP(i,count) if(maxFare == trains[i].fare) printf("%d ",i);
		printf("\n");
		printf("Max time: %d min Max time id: ",maxTime);
		REP(i,count) if(maxTime == trains[i].arv - trains[i].dpt) printf("%d ",i);
		printf("\n");

		// 盛岡行きの列車を出力
		REP(i,count) {
			if(trains[i].to == city_id("Morioka"))
				printf("%d %d %d %d %d\n",
					trains[i].from,trains[i].dpt,trains[i].to,trains[i].arv,trains[i].fare);
		}
	}
	return 0;
}





