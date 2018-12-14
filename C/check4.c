
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FOR(i,a,b) for(int i=a;i<b;i++)
#define REP(i,n)  FOR(i,0,n)
#define MIN(a,b) (((a)<(b)) ? (a):(b))

#define MAXCITY 100
#define MAXCOUNT 2000 + 1
#define INFINITE 999999
#define BIAS 24*60 //24h * 60min 1440
#define START 8*60
#define END 18*60

char city_name[MAXCITY][18]; //駅名の表
int ncity; //駅数
int nconn; //列車数

int from_hakodate[MAXCITY][MAXCOUNT]; //fr_h
int from_tokyo[MAXCITY][MAXCOUNT]; //fr_t
int to_hakodate[MAXCITY][MAXCOUNT]; //to_h
int to_tokyo[MAXCITY][MAXCOUNT]; //to_t

// 列車配列
struct train {
	int from, to;
	int dpt, arv;
	int fare;
} trains[MAXCOUNT],rtrains[MAXCOUNT];

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

	if(dpt[0]*60 + dpt[1] < START || arv[0]*60 + arv[1] > END) return;

	trains[nconn].from = city_id(from);
	trains[nconn].to = city_id(to);
	trains[nconn].dpt = dpt[0]*60 + dpt[1];
	trains[nconn].arv = arv[0]*60 + arv[1];
	trains[nconn].fare = fare;
	nconn++;
}

// qsort用
int cmp_arv(const void *t1, const void *t2) {
    return ((struct train*)t1)->arv - ((struct train*)t2)->arv;
}

// trains, rtrains配列の準備
void prepare_data(void) {
	// rtrainsの作成
	REP(i,nconn) {
		rtrains[i].from = trains[i].to;
		rtrains[i].to = trains[i].from;
		rtrains[i].dpt = BIAS - trains[i].arv;
		rtrains[i].arv = BIAS - trains[i].dpt;
		rtrains[i].fare = trains[i].fare;
	}
	// ソート
	qsort(trains, nconn, sizeof(struct train), cmp_arv);
	qsort(rtrains, nconn, sizeof(struct train), cmp_arv);
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

	FOR(i,0,ncity) v[i][0] = INFINITE;
	v[org][0] = 0;

	FOR(ti,0,nconn) {
		FOR(i,0,ncity) v[i][ti+1] = v[i][ti];

		//列車配列tvの中で，tv[ti].dpt以前に駅tv[ti].fromに到着する列車を探す
		//つまり，列車tv[ti]に乗り継げる列車を探す。
		//ti-1: 探索を開始する列車到着事象の番号
		a = change(tv, ti-1, tv[ti].from, tv[ti].dpt);

		v[tv[ti].to][ti+1] = MIN(v[tv[ti].to][ti], tv[ti].fare+v[tv[ti].from][a+1]);
	}
}

int calc_cost(int city) {
	int a = 0, d = nconn-1;
	int stay;
	int c, min_c = INFINITE;

	while(1) {
		// 滞在可能時間
		stay = (BIAS - rtrains[d].arv) - trains[a].arv;
		// 滞在時間が30分未満 -> 後ろを詰める
		if(stay < 30) {
			d--;
			if(d < 0) break;
			continue;
		}

		// コスト計算
		c = from_hakodate[city][a+1] +
			from_tokyo[city][a+1] +
			to_hakodate[city][d+1] +
			to_tokyo[city][d+1];

		// 最小値更新
		if(c < min_c) min_c = c;
		a++; // 前を詰める
		if(a >= nconn) break;
	}
	return min_c;
}

// コスト最小値を求める
int solve(void) {
	int cost, min_cost;

	// trains, rtrains用意
	prepare_data();
	// fr_?, to_?を用意
	make_table(from_hakodate, city_id("Hakodate"), trains);
	make_table(from_tokyo, city_id("Tokyo"), trains);
	make_table(to_hakodate, city_id("Hakodate"), rtrains);
	make_table(to_tokyo, city_id("Tokyo"), rtrains);

	// コスト最小値を求める
	min_cost = INFINITE;
	REP(c,ncity) {
		cost = calc_cost(c);
		if(min_cost > cost) min_cost = cost;
	}
	if(min_cost >= INFINITE) min_cost = 0;

	return min_cost;
}

// 表出力用
void print_table(char city[], int type, int table[MAXCITY][MAXCOUNT]) {
	printf(!type ? "From":"To");
	printf(" %s:\ni\ttime|",city);
	REP(i,ncity) printf("%d	",i);
	printf("\n----------------+------------------------\n");
	REP(i,nconn+1) {
		printf("%d\t",i-1);
		if(i) printf("%d",!type ? trains[i-1].arv:rtrains[i-1].arv);
		printf("\t|");
		REP(j,ncity) printf("%d ",table[j][i]);
		printf("\n");
	}
	printf("\n");
}

int main() {
	int count, minc;
	char buf[64];

	count = 0;
	while(fgets(buf, sizeof(buf), stdin) != NULL) {
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

		// コスト最小値
		minc = solve();

		// 出力
		print_table("Hakodate", 0, from_hakodate);
		print_table("Tokyo", 0, from_tokyo);
		print_table("Hakodate", 1, to_hakodate);
		print_table("Tokyo", 1, to_tokyo);

		printf("Answer:%d\n\n\n",minc);
	}
	return 0;
}




