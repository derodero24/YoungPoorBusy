
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FOR(i,a,b) for(int i=a;i<b;i++)
#define REP(i,n)  FOR(i,0,n)
#define MIN(a,b) (((a)<(b)) ? (a):(b))

#define MAXCITY 100
#define MAXCOUNT 2000 + 1
#define INFINITE 999999
char city_name[MAXCITY][18]; //駅名の表
int ncity; //駅数
int nconn; //列車数

int from_hakodate[MAXCITY][MAXCOUNT]; //fr_h
int from_tokyo[MAXCITY][MAXCOUNT];	 //fr_t

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

// qsort用関数（テキストの書き方は良くない）
int cmp_arv(const void *t1, const void *t2) {
    return ((struct train*)t1)->arv - ((struct train*)t2)->arv;
}

//到着時刻の早い順に並べ替え
void prepare_data(void) {
	qsort(trains, nconn, sizeof(struct train), cmp_arv);
}

//列車配列tvの中で，dpttime以前に駅stに到着する列車を探す
//p: 探索を開始する列車到着事象の番号
int change(struct train tv[], int p, int st, int dpttime) {
	while(p >= 0) {
		if((tv[p].to == st) && (tv[p].arv <= dpttime)) break;
		p--;
	}
	return p;
}

//列車配列tvを参照して，駅orgを起点として，
//fr_h または fr_t を二次元配列vに作成する。
void make_table(int v[MAXCITY][MAXCOUNT], int org, struct train tv[]) {
	int a;

	// 配列の初期化
	FOR(i,0,ncity) v[i][0] = INFINITE;
	v[org][0] = 0;

	FOR(ti,0,nconn) {
		// とりあえず今までの最安値を格納
		FOR(i,0,ncity) v[i][ti+1] = v[i][ti];

		//列車配列tvの中で，tv[ti].dpt以前に駅tv[ti].fromに到着する列車を探す
		//つまり，列車tv[ti]に乗り継げる列車を探す。
		//ti-1: 探索を開始する列車到着事象の番号
		a = change(tv, ti-1, tv[ti].from, tv[ti].dpt);

		// 今までの最安値と新しいルートでの金額を比較 & 安い方を採用
		v[tv[ti].to][ti+1] = MIN(v[tv[ti].to][ti], tv[ti].fare+v[tv[ti].from][a+1]);
	}
}

int main() {
	int count;
	char buf[64];

	count = 0;
	while(fgets(buf, sizeof(buf), stdin) != NULL) {
		// パラメータ初期化
		nconn = 0;
		ncity = 0;

		// 列車読み込み
		sscanf(buf, "%d", &count);
		printf("%d\n", count);
		if(!count) break;
		REP(i,count) {
			if(fgets(buf, sizeof(buf), stdin) == NULL) break;
			parse_connection(buf);
		}

		// 列車出力
		printf("Before:\n");
		REP(i,count)
			printf("%d %d %d %d %d\n",
					trains[i].from,trains[i].dpt,trains[i].to,trains[i].arv,trains[i].fare);
		// ソート
		prepare_data();

		// 列車出力
		printf("After:\n");
		REP(i,count)
			printf("%d %d %d %d %d\n",
				trains[i].from,trains[i].dpt,trains[i].to,trains[i].arv,trains[i].fare);

		// fr_hの作成
		make_table(from_hakodate, city_id("Hakodate"), trains);
		printf("From Hakodate:\ni\ttime |");
		REP(i,ncity) printf("%d	",i);
		printf("\n----------------+------------------------\n");
		REP(i,nconn+1) {
			printf("%d ",i-1);
			if(i) printf("%d\t",trains[i-1].arv); // 先頭行以外は到着時間出力
			printf("|");
			REP(j,ncity) printf("%d ",from_hakodate[j][i]);
			printf("\n");
		}
		printf("\n");

		// fr_tの作成
		make_table(from_tokyo, city_id("Tokyo"), trains);
		printf("From Tokyo:\ni\ttime |");
		REP(i,ncity) printf("%d\t",i);
		printf("\n----------------+------------------------\n");
		REP(i,nconn+1) {
			printf("%d ",i-1);
			if(i) printf("%d ",trains[i-1].arv);
			printf("|");
			REP(j,ncity) printf("%d ",from_tokyo[j][i]);
			printf("\n");
		}
		printf("\n\n");
	}
	return 0;
}






