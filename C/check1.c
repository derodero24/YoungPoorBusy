
#include <stdio.h>

#define FOR(i,a,b) for(int i=a;i<b;i++)
#define REP(i,n)  FOR(i,0,n)

int main() {
	int count;
	char buf[64];

	count = 0;
	while(fgets(buf, sizeof(buf), stdin) != NULL) {

		 // 数値
		sscanf(buf, "%d", &count);
		printf("%d\n", count);

		if(!count) break;

		// 行
		while(count-- > 0) {
			if(fgets(buf, sizeof(buf), stdin) == NULL) break;
			fputs(buf, stdout);
		}
	}

	return 0;
}
