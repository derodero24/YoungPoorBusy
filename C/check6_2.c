
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

char station[2][18];
int start, end, meetTime;

char city_name[MAXCITY][18]; //駅名の表
int ncity; //駅数
int nconn; //列車数

int ft[4][MAXCITY][MAXCOUNT];

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

int cmp_arv(const void *t1, const void *t2) {
    return ((struct train*)t1)->arv - ((struct train*)t2)->arv;
}

//到着時刻の早い順に並べ替え
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

void m2hm(int min, int hm[]) {
	hm[0] = (int)(min / 60);
	hm[1] = min - 60 * (int)(min / 60);
}

int test(int d, int city) {
	for(int i = d;d>=0;d--) {
		if(trains[1][i].to == city)
			return BIAS - trains[1][i].arv;
	}
	return 0;
}

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
		REP(i,2) c += ft[i][city][a+1];
		REP(i,2) c += ft[i+2][city][d+1];

		if(c < min[0]) {
			min[0] = c;
			min[1] = trains[0][a].arv;
			min[2] = BIAS - trains[1][d].arv;
//			min[2] = test(d,city);

			printf("%d ",stay);
			int hm[2];
			printf("%s ",city_name[city]);
			printf("%d ",min[0]);
			m2hm(min[1], hm);
			printf("%02d:%02d - ",hm[0],hm[1]);
			m2hm(min[2], hm);
			printf("%02d:%02d",hm[0],hm[1]);
			printf("\n");
		}
		a++;
		if(a >= nconn) break;
	}
}

void solve(int min[]) {
	int tmp[3];//, min[3];

	prepare_data();
	REP(i,4) make_table(ft[i], city_id(station[i%2]), trains[i/2]);

	min[0] = INFINITE;
	REP(c,ncity) {
		calc_cost(c, tmp);
		if(min[0] > tmp[0]) {
			REP(i,3) min[i] = tmp[i];
			min[3] = c;
		}
	}
	if(min[0] >= INFINITE) min[0] = 0;
//	return min[0];
}

void print_table(int num) {
	printf(num < 2 ? "From":"To");
	printf(" %s:\ni\ttime|",station[num%2]);
	REP(i,ncity) printf("%d\t",i);
	printf("\n----------------+------------------------\n");
	REP(i,nconn+1) {
		printf("%d\t",i-1);
		if(i) printf("%d",trains[num/2][i-1].arv);
		printf("\t|");
		REP(j,ncity) printf("%d ",ft[num][j][i]);
		printf("\n");
	}
	printf("\n");
}

int main(int argc, char *argv[]) {
	int count, min[4], tmp[2], hm[2];
	char buf[64];

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
//		REP(i,MAXCITY) city_name[i][0] = ' ';

		sscanf(buf, "%d", &count);
//		printf("%d\n", count);
		if(!count) break;
		REP(i,count) {
			if(fgets(buf, sizeof(buf), stdin) == NULL) break;
			parse_connection(buf);
		}

		solve(min);
//		REP(i,4) print_table(i);

		printf("Answer:%d ",min[0]);
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




