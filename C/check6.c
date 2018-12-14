
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FOR(i,a,b) for(int i=a;i<b;i++)
#define REP(i,n)  FOR(i,0,n)
#define MIN(a,b) (((a)<(b)) ? (a):(b))
#define MAX(a,b) (((a)<(b)) ? (b):(a))

#define MAXCITY 100
#define MAXCOUNT 2000 + 1
#define INFINITE 999999
#define BIAS 24*60 //24h * 60min 1440

char station[2][18];
int start, end, meetTime;

char city_name[MAXCITY][18]; //駅名の表
int ncity; //駅数
int nconn; //列車数

// fr_?, to_? の最低額とその時の出発・到着時間をまとめた配列
int ft[4][MAXCITY][MAXCOUNT][3];

// 列車配列
struct train {
	int from, to;
	int dpt, arv;
	int fare;
} trains[2][MAXCOUNT];//,rtrains[MAXCOUNT];

//駅名 -> 駅番号
int city_id(char *name) {
	FOR(i,0,ncity) {
		if(strcmp(name, &(city_name[i][0])) == 0) return i;
	}
	strcpy(&(city_name[ncity][0]), name);
	return ncity++;
}

// 駅追加
void parse_connection(char *buf) {
	char from[18], to[18];
	int dpt[2], arv[2], fare;

	sscanf(buf,"%s %d:%d %s %d:%d %d",
			from, &(dpt[0]), &(dpt[1]), to, &(arv[0]), &(arv[1]), &fare);

	if(dpt[0]*60 + dpt[1] < start || arv[0]*60 + arv[1] > end) return;

	trains[0][nconn].from = city_id(from);
	trains[0][nconn].to = city_id(to);
	trains[0][nconn].dpt = dpt[0]*60 + dpt[1];
	trains[0][nconn].arv = arv[0]*60 + arv[1];
	trains[0][nconn].fare = fare;
	nconn++;
}

// ソート用
int cmp_arv(const void *t1, const void *t2) {
    return ((struct train*)t1)->arv - ((struct train*)t2)->arv;
}

// 分 -> 何時何分
void m2hm(int min, int hm[]) {
	hm[0] = (int)(min / 60);
	hm[1] = min - 60 * (int)(min / 60);
}

// trains, rtrains配列の準備
void prepare_data(void) {
	REP(i,nconn) {
		trains[1][i].from = trains[0][i].to;
		trains[1][i].to = trains[0][i].from;
		trains[1][i].dpt = BIAS - trains[0][i].arv;
		trains[1][i].arv = BIAS - trains[0][i].dpt;
		trains[1][i].fare = trains[0][i].fare;
	}
	qsort(trains[0], nconn, sizeof(struct train), cmp_arv);
	qsort(trains[1], nconn, sizeof(struct train), cmp_arv);
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

//列車配列trを参照して，駅orgを起点として，
//fr_h または fr_t を二次元配列ftに作成する。
void make_table(int ft[MAXCITY][MAXCOUNT][3], int org, struct train tr[]) {
	int a;

	REP(i,ncity) ft[i][0][0] = INFINITE;
	ft[org][0][0] = 0;

	REP(ti,nconn) {
		REP(i,ncity) REP(j,3) ft[i][ti+1][j] = ft[i][ti][j];

		//列車配列tvの中で，tv[ti].dpt以前に駅tv[ti].fromに到着する列車を探す
		//つまり，列車tv[ti]に乗り継げる列車を探す。
		//ti-1: 探索を開始する列車到着事象の番号
		a = change(tr, ti-1, tr[ti].from, tr[ti].dpt);

//		v[tv[ti].to][ti+1][0] = MIN(v[tv[ti].to][ti][0], tv[ti].fare+v[tv[ti].from][a+1][0]);
		if(ft[tr[ti].to][ti][0] > tr[ti].fare+ft[tr[ti].from][a+1][0]) {
			ft[tr[ti].to][ti+1][0] = tr[ti].fare+ft[tr[ti].from][a+1][0];
			ft[tr[ti].to][ti+1][1] = tr[ti].dpt;
			ft[tr[ti].to][ti+1][2] = tr[ti].arv;
		}
	}
}

// コスト計算
// min[]はコストとその時の面会開始・終了時間
void calc_cost(int city, int min[]) {
	int a = 0, d = nconn-1;
	int stay;
	int c;

	min[0] = INFINITE;
	while(1) {
		stay = (BIAS - trains[1][d].arv) - trains[0][a].arv;
		if(stay < meetTime) {
			d--;
			if(d < 0) break;
			continue;
		}

		c = 0;
		REP(i,2) c += ft[i][city][a+1][0];
		REP(i,2) c += ft[i+2][city][d+1][0];

		if(c < min[0]) {
			min[0] = c;
			min[1] = MAX(ft[0][city][a+1][2],ft[1][city][a+1][2]);
			min[2] = BIAS - MAX(ft[2][city][d+1][2],ft[3][city][d+1][2]);

//			int hm[2];
//			printf("%s ",city_name[city]);
//			printf("%d ",min[0]);
//			m2hm(min[1], hm);
//			printf("%02d:%02d - ",hm[0],hm[1]);
//			m2hm(min[2], hm);
//			printf("%02d:%02d",hm[0],hm[1]);
//			printf("\n");
		}
		a++;
		if(a >= nconn) break;
	}
}

// コスト最小値とその時の面会時間
void solve(int min[]) {
	int tmp[3];//, min[3];

	//
	prepare_data();
	REP(i,4) make_table(ft[i], city_id(station[i%2]), trains[i/2]);

	min[0] = INFINITE;
	REP(c,ncity) {
		calc_cost(c, tmp);
		if(min[0] > tmp[0]) {
			REP(i,3) min[i] = tmp[i];
			min[3] = c;
		}
//		printf("%s: %d\n",city_name[c],tmp[0]); //チェックポイント４のテスト
	}
	if(min[0] >= INFINITE) min[0] = 0;
}

// 出力用
void print_table(int num) {
	printf(num < 2 ? "From":"To");
	printf(" %s:\ni\ttime|",station[num%2]);
	REP(i,ncity) printf("%d\t",i);
	printf("\n----------------+------------------------\n");
	REP(i,nconn+1) {
		printf("%d\t",i-1);
		if(i) printf("%d",trains[num/2][i-1].arv);
		printf("\t|");
		REP(j,ncity) REP(k,3) printf("%d ",ft[num][j][i][k]);
		printf("\n");
	}
	printf("\n");
}

int main(int argc, char *argv[]) {
	int count, min[4], tmp[2], hm[2];
	char buf[64];

	// 引数を変数に格納
	if(argc != 6) {
		fprintf(stderr, "argc error\n");
		return 1;
	}
	REP(i,2) strcpy(station[i],argv[i+1]);
	sscanf(argv[3],"%d:%d",&tmp[0],&tmp[1]);
	start = tmp[0]*60 + tmp[1];
	sscanf(argv[4],"%d:%d",&tmp[0],&tmp[1]);
	end = tmp[0]*60 + tmp[1];
	meetTime = atoi(argv[5]);

	count = 0;
	while(fgets(buf, sizeof(buf), stdin) != NULL) {
		nconn = 0;
		ncity = 0;

		// 列車読み込み
		sscanf(buf, "%d", &count);
		if(!count) break;
		REP(i,count) {
			if(fgets(buf, sizeof(buf), stdin) == NULL) break;
			parse_connection(buf);
		}

		// コスト最小値とその時の面会時間
		solve(min);
//		REP(i,4) print_table(i);

		// 結果出力
		printf("%d ",min[0]);
		if(min[0]) {
			printf("%s: ",city_name[min[3]]);
			m2hm(min[1], hm);
			printf("%02d:%02d - ",hm[0],hm[1]);
			m2hm(min[2], hm);
			printf("%02d:%02d ",hm[0],hm[1]);
		}
		printf("\n");
	}
	return 0;
}




