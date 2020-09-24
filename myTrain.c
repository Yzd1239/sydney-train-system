/*
 * time complexity: O(N^2)
 * I use a 2-d array to store the time information,a linked list to store the train line.
 * I use an array to store all stations,each station linked list record the multiple trains passing thought the station
   (including station name and arrival time).
 * When we read the file ,for every stop(m*k)of each train,we store the station number into station_idx,at the same time
   build a map schedule.
 * Use find() map and linked list to get the line that reach to the destination,and pick one with the latest departure time.
 * The time complexity is st_n*st_n witch is O(N^2).
  */
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct Stop{
	int station_idx;
	int time;
	struct Stop* next;
}Stop;

typedef struct Station {
	char name[20];
}Station;

Station* stations = NULL;
int st_n;//number of stations
Stop** stop_list = NULL;//stop LL
int stop_n;//num of stops
int** map;

int find_idx_by_staname(char* name){
	int i;
	for (i = 0; i < st_n; i++) {
		if (!strcmp(name, stations[i].name)) {
			return i;
		}
	}
	return -1;
}

void freemem() {
	int i;
	free(stations);
	for (i = 0; i < stop_n; i++) {
		free(stop_list[i]);
	}
	for (i = 0; i < stop_n; i++) {
		free(map[i]);
	}
	free(stop_list);
	free(map);
}

//insert nodes
void insert_front(Stop** head, int time, char* name) {
	Stop* node = NULL;
	node = (Stop*)malloc(sizeof(Stop));
	node->next = *head;
	node->station_idx = find_idx_by_staname(name);
	node->time = time;
	*head = node;
}

//reverse
Stop* reverse(Stop* list) {
	Stop* cur = list, *next, *pre;
	next = NULL;
	pre = NULL;
	while (cur) {
		next = cur->next;
		cur->next = pre;
		pre = cur;
		cur = next;
	}
	return  pre;
}

//read data
void read_data() {
	int i, j;
	
	int train_n;//num of trains
	int time, t;
	char name[20] = { 0 };
	printf("Enter the number of stations: ");
	scanf("%d", &st_n);

	stations = (Station*)calloc(st_n, sizeof(Station));
	for (i = 0; i < st_n; i++) {
		scanf("%s", stations[i].name);
	}

	printf("Enter the number of trains: ");
	scanf("%d", &train_n);
	stop_n = train_n;
	map = (int**)malloc(sizeof(int*) * train_n);

	stop_list = (Stop * *)calloc(train_n, sizeof(Stop*));
	for (i = 0; i < train_n; i++) {
		stop_list[i] = NULL;
		map[i] = (int*)malloc(sizeof(int) * st_n);
		memset(map[i], 0, sizeof(int)*st_n);
		printf("Enter the number of stops: ");
		scanf("%d", &t);
		for (j = 0; j < st_n; j++) {
			map[i][j] = -1;
		}
		for (j = 0; j < t; j++) {
			scanf("%d%*c%s", &time, name);
			map[i][find_idx_by_staname(name)] = time;//arrive time
			insert_front(&stop_list[i], time, name);//LL
		}
	}
}

void find_train(char* src, char* des, int arrive) {
	int i, j;
	Stop* cur;
	int* mark;
	int atleastone = 0;
	int maxarr = -1, maxline = -1;
	int getend = 0, st_idx = 0, lasttime = 0, lastline = maxline;
	int src_exist = 0, des_exist = 0;
	mark = (int*)malloc(sizeof(int) * stop_n);
	memset(mark, 0, sizeof(int)* stop_n);//line satisfation
	
	//if the des and str in the middle of lane
	for (j = 0; j < stop_n; j++) {
		cur = stop_list[j];
		while (cur) {
			if (!strcmp(des, stations[cur->station_idx].name) && cur->time <= arrive) {
				mark[j] = 1;
				atleastone = 1;
				des_exist = 1;
			}
			if (!strcmp(src, stations[cur->station_idx].name) && cur->time <= arrive) {
				src_exist = 1;
			}
			cur = cur->next;
		}
	}

	if (!des_exist || !src_exist) {
		printf("No connection found.\n");
		return;
	}
	//the lane to the destination
	for (i = 0; i < stop_n; i++) {
		if (!mark[i]) {
			continue;
		}
		for (j = 0; j < stop_n; j++) {
			if (!mark[j]) {
				cur = stop_list[j];
				while (cur) {
					if (map[j][cur->station_idx] >= 0 &&
						map[j][cur->station_idx] < map[i][cur->station_idx]) {
						mark[j] = 1;
						break;
					}
					cur = cur->next;
				}
			}
		}
	}
	atleastone = 0;
	//find the start station from all the possible line that reach to the destination
	for (i = 0; i < stop_n && !atleastone; i++) {
		if (!mark[i]) {
			continue;
		}
		cur = stop_list[i];
		while (cur) {
			if (!strcmp(stations[cur->station_idx].name, src)) {
				atleastone = 1;
				break;
			}
			cur = cur->next;
		}
	}
	if (!atleastone) {
		printf("No connection found.\n");
		return;
	}
	//latest dep
	for (i = 0; i < stop_n; i++) {

		stop_list[i] = reverse(stop_list[i]);
		cur = stop_list[i];
		while (cur) {
			if (!strcmp(stations[cur->station_idx].name, src)) {
				if (mark[i] && stop_list[i]->time > maxarr) {
					maxarr = stop_list[i]->time;
					maxline = i;
					break;
				}
			}
			cur = cur->next;
		}
		
	}
	cur = stop_list[maxline];
	while (cur) {
		if (!strcmp(stations[cur->station_idx].name, src)) {
			break;
		}
		cur = cur->next;
	}
	lastline = maxline;
	while(1){
		while (cur) {
			st_idx = cur->station_idx;
			lasttime = cur->time;
			printf("%04d %s\n", lasttime, stations[st_idx].name);
			if (!strcmp(stations[st_idx].name, des)) {
				getend = 1;
				break;
			}
			cur = cur->next;
		}
		if (getend) {
			break;
		}
		//change
		cur = NULL;
		for (i = 0; i < stop_n; i++) {
			if (mark[i] && map[i][st_idx] >= lasttime && lastline!=i) {
				cur = stop_list[i];
				lastline = i;//different lane
				while (cur->station_idx != st_idx) {
					cur = cur->next;
				}
				printf("Change at Central\n");
				break;
			}
		}
		if (cur == NULL) {
			//bug
			printf("ERROR!\n");
			freemem();
			exit(-2);
		}
	}
	//re-reverse
	for (i = 0; i < stop_n; i++) {
		stop_list[i] = reverse(stop_list[i]);
	}
}
void find() {
	char src[20] = { 0 }, des[20] = { 0 };
	char arrive[20] = { 0 };
	while (1) {
		printf("From: ");
		scanf("%s", src);
		if (!strcmp(src, "done")) {
			break;
		}
		printf("To: ");
		scanf("%s", des);
		printf("Arrive by: ");
		scanf("%s", arrive);
		find_train(src, des, atoi(arrive));
	}
	printf("Thank you for using myTrain.\n");
}

int main() {
	//freopen("test3", "r", stdin);
	read_data();
	//print_data();
	find();
	freemem();
	return 0;
}
