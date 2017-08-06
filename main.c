#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define GOOD 1
#define BAD -1
#define DRAW 0
#define DELTA_GOOD 2000
#define DELTA_BAD -2000
#define DELTA_DRAW 1000
#define NMAX 10000
#define NSTART 5000
#define NMIN 1

static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
}

typedef struct {
	char f[10];
	int p[9];
	void *next; // pointer to next item in list
} neuron;

typedef struct {
	void *ptr; // ptr to node to modify
	int boy; // good boy or bad boy :D
	int cell; // cell to be encouraged
} nodemod;

int keyboard();
int kauto();
void printFld(char *f);
int check(char *f);
int AI(char *f, nodemod **game, int *gamelen, neuron **net, int *netlen);
neuron *findN(char *f, neuron *net, int netlen);
neuron *lastN(neuron *net, int netlen);
neuron *prenultN(neuron *net, int netlen);
neuron *createN(char *f, neuron **net, int *netlen);
int rmlastN(neuron **net, int *netlen);
int rmnetN(neuron **net, int *netlen);
int modifyN(nodemod *game, int gamelen, neuron **net, int netlen);
int setweight(int weight, nodemod *game, int gamelen);
int saveN (char *filename, neuron *net, int netlen);
int loadN(char *filename, neuron **net, int *netlen);
char *binFileRead(char *filename, int *fileln);
int binFileWrite(char *filename, char *string, int strln);

neuron *net; // pointer to first neuron in list
int netlen; // total count of network neurons
nodemod *game; // neuron chain of game
int gamelen; // neuron chain length

int main()
{
	int win;
	srand(time(NULL));
	char input = 0;
	int manual = 0;
	while ( input != 'm' && input != 'M' && 
			input != 'a' && input != 'A') {
		printf("Chose mode manual(m) or auto(a): ");
		input = getchar();
		if (input == 'm' || input == 'M') {
			manual = 1;
		} else { 
			printf("Atomatic learning...\n");
			signal(SIGINT, intHandler);
		}
	}
	while (keepRunning) {
		net = NULL; // pointer to first neuron in list
		netlen = 0; // total count of network neurons
		game = NULL; // neuron chain of game
		gamelen = 0; // neuron chain length
		loadN("net.mlx", &net, &netlen);
		char gameState[10] = {0};
		memset(gameState+1, '.', 9);
		int cell = 0;
		while (1) {
			if (manual) {
				keepRunning = 0;
				printFld(gameState);
				if (gameState[0] == 0)
					printf("X turn\n");
				else
					printf("O turn\n");
			}
			int set = 0;
			while (set == 0) {
				if (gameState[0] == 0)
					if (manual)
						cell = keyboard();
					else
						cell = kauto();
				else
					if (!(cell = AI(gameState, &game, &gamelen,
							&net, &netlen))) {
						printf("AI returned err cell %d\n", 
										cell);
						saveN("net.mlx", net, netlen);
						exit(-1);
					}
				if (gameState[cell] != '.') {
					if (manual)
						printf("this cell is already filled\n");
				} else {
					if (gameState[0] == 0)
						gameState[cell] = 'X';
					else
						gameState[cell] = 'O';
					set = 1;
				}
			}
			set = 0;
			if ((win = check(gameState))) {
				if (manual)
					switch (win){
					case 1:
						printf("X is winner!\n");
						break;
					case 2:
						printf("O is winner!\n");
						break;
					case 3:
						printf("Draw...\n");
						break;
					}
				break;
			}
			if (gameState[0] == 0)
				gameState[0] = 1;
			else
				gameState[0] = 0;
		}
		modifyN(game, gamelen, &net, netlen);
		free(game);
		saveN("net.mlx", net, netlen);
		rmnetN(&net, &netlen);
	}
	return 0;
}

int keyboard()
{
	char str[11];
	int ret = 0;
	while (ret < 1 || ret > 9) {
		int k = 0;
		printf("> ");
		while (k <= 10 && str[k-1] != '\n') {
			str[k] = getchar();
			k++;
		}
		str[k-1] = '\0';
		k = 0;
		ret = atoi(str);
		if (ret < 1 || ret > 9)
			printf("Your number does not met requirements\n");
	}
	return ret;
}

int kauto()
{
	int ret = 0;
	ret = rand()%9+1;
	return ret;
}

void printFld(char *f)
{
	printf("|%c%c%c|\n", f[1], f[2], f[3]);
	printf("|%c%c%c|\n", f[4], f[5], f[6]);
	printf("|%c%c%c|\n", f[7], f[8], f[9]);
}

int check(char *f)
{
	if ((f[1] == 'X' && f[2] == 'X' && f[3] == 'X')
	||  (f[4] == 'X' && f[5] == 'X' && f[6] == 'X')
	||  (f[7] == 'X' && f[8] == 'X' && f[9] == 'X')
	||  (f[1] == 'X' && f[4] == 'X' && f[7] == 'X')
	||  (f[2] == 'X' && f[5] == 'X' && f[8] == 'X')
	||  (f[3] == 'X' && f[6] == 'X' && f[9] == 'X')
	||  (f[1] == 'X' && f[5] == 'X' && f[9] == 'X')
	||  (f[7] == 'X' && f[5] == 'X' && f[3] == 'X')) {
		setweight(BAD, game, gamelen);
		return 1;
	}

	if ((f[1] == 'O' && f[2] == 'O' && f[3] == 'O')
	||  (f[4] == 'O' && f[5] == 'O' && f[6] == 'O')
	||  (f[7] == 'O' && f[8] == 'O' && f[9] == 'O')
	||  (f[1] == 'O' && f[4] == 'O' && f[7] == 'O')
	||  (f[2] == 'O' && f[5] == 'O' && f[8] == 'O')
	||  (f[3] == 'O' && f[6] == 'O' && f[9] == 'O')
	||  (f[1] == 'O' && f[5] == 'O' && f[9] == 'O')
	||  (f[7] == 'O' && f[5] == 'O' && f[3] == 'O')) {
		setweight(GOOD, game, gamelen);
		return 2;
	}

	if (f[1] != '.' && f[2] != '.' && f[3] != '.'
	&&  f[4] != '.' && f[5] != '.' && f[6] != '.'
	&&  f[7] != '.' && f[8] != '.' && f[9] != '.') {
		setweight(DRAW, game, gamelen);
		return 3;
	}
	return 0;
}

int AI(char *f, nodemod **game, int *gamelen, neuron **net, int *netlen)
{
	neuron *a;
	if (!(a = findN(f, *net, *netlen)))
		a = createN(f, net, netlen);
	int tmp = 0;
	int sum = 0;
	int prev = 0;
	for (int i = 0; i < 9; i++)
		sum += a->p[i];
	tmp = rand()%(sum);
	//printf("sum = %d, tmp = %d\n",sum, tmp);
	for (int i = 0; i < 9; i++) {
		prev += a->p[i];
		if (tmp < prev) {
			*gamelen += 1;
			*game = realloc(*game, *gamelen * sizeof(nodemod));
			(*game)[*gamelen-1].ptr = a;
			(*game)[*gamelen-1].cell = i;
			return i+1;
		}
	}
	return 0;
}

/* Returns node by given hash (game state) */
neuron *findN(char *f, neuron *net, int netlen) 
{
	neuron *ptr = net;
	for (int i = 0; i < netlen; i++) {
		if (!(memcmp(f, ptr->f, 10))) {
			/*
			for (int j = 0; j < 9; j++) {
				printf("p[%d] = %d\n", j, ptr->p[j]);
			}
			*/
			break;
		} else {
			ptr = ptr->next;
		}
	}
	return ptr; // 0 if not found
}

/* Returns prenult node in list */
neuron *prenultN(neuron *net, int netlen)
{
	neuron *ptr = net;
	for (int i = 0; i < netlen-2; i++) {
		ptr = ptr->next;
	}
	return ptr;
}

/* Returns last node in list */
neuron *lastN(neuron *net, int netlen)
{
	neuron *ptr = net;
	for (int i = 0; i < netlen-1; i++) {
		ptr = ptr->next;
	}
	return ptr;
}

/* Creates absolutely new node */
neuron *createN(char *f, neuron **net, int *netlen)
{
	neuron *ptr = *net;
	if (ptr && *netlen) {
		ptr = lastN(*net, *netlen);
		ptr->next = (neuron *)malloc(sizeof(neuron));
		ptr = ptr->next;
	} else {
		*net = (neuron *)malloc(sizeof(neuron));
		ptr = *net;
	}
	*netlen += 1;
	memcpy(ptr->f, f, 10 * sizeof(char));
	/* Probability distribution */
	int count = 0;
	for (int i = 0; i < 9; i++) {
		if (ptr->f[i+1] == '.') {
			ptr->p[i] = NSTART;
			count++;
		} else {
			ptr->p[i] = 0;
		}
	}
	/* Important to clear ptr to next node in list */
	ptr->next = NULL;
	return ptr;
}

/* Removes last node in list */
int rmlastN(neuron **net, int *netlen)
{
	neuron *ptr = prenultN(*net, *netlen);
	free(ptr->next);
	*netlen -= 1;
	return 0;
}

/* Removes all the list */
int rmnetN(neuron **net, int *netlen)
{
	int netlenToBeDeleted = *netlen;
	for (int i = 0; i < netlenToBeDeleted-1; i++) {
		rmlastN(net, netlen);
	}
	free(*net);
	*netlen -= 1;
	return 0;
}

/* Modifies weights of connections in all the net according to game chain */
int modifyN(nodemod *game, int gamelen, neuron **net, int netlen)
{
	neuron *a;
	int delta;
	for (int i = 0; i < gamelen; i++) {
		a = game[i].ptr;
		switch(game[i].boy) {
		case GOOD:
			delta = DELTA_GOOD;
			if ((a->p[game[i].cell] + delta) > NMAX)
				delta = NMAX - a->p[game[i].cell];
			break;
		case BAD:
			delta = DELTA_BAD;
			if ((a->p[game[i].cell] + delta) < NMIN)
				delta = NMIN - a->p[game[i].cell];
			break;
		case DRAW:
			delta = DELTA_DRAW;
			if ((a->p[game[i].cell] + delta) > NMAX)
				delta = NMAX - a->p[game[i].cell];
			break;
		default:
			printf("Encorage type error %d\n", game->boy);
			exit (-1);	
		}
		a->p[game[i].cell] += delta;
	}
	return 0;
}

/* Modifies weights in game chain */
int setweight(int weight, nodemod *game, int gamelen)
{
	for (int i = 0; i < gamelen; i++) {
		game[i].boy = weight;
	}
	return 0;
}

/* Saves net to a binary file */
int saveN (char *filename, neuron *net, int netlen) 
{
	static time_t timer;
	static time_t timercur;
	char *string;
	int nodeln = 10*sizeof(char) + 9*sizeof(int) + 1;
	int strln = netlen * nodeln + 7 + sizeof(int) + 1;
	neuron *ptr;
	string = (char *)malloc(strln);
	memcpy(string, "MLv0.1\n", 7);
	memcpy(string+7, &netlen, sizeof(int));
	if ((ptr = net)) {
		for (int i = 0; i < netlen; i++) {
			memcpy(string+7+4+i*nodeln, ptr->f, 10 * sizeof(char));
			memcpy(string+7+4+i*nodeln+10*sizeof(char), ptr->p,
							9 * sizeof(int));
			memcpy(string+7+4+(i+1)*nodeln-1, "\n", 1);
			ptr = ptr->next;
		}
	} else {
		return -1;
	}
	memcpy(string+strln-1, "\0", 1);
	binFileWrite(filename, string, strln);
	free(string);
	timercur = time(NULL); 
	if (timercur - timer >= 10) {
		timer = time(NULL);
		printf("%d nodes\n", netlen);
	}
	return 0;
}

/* Loads net from a binary file */
int loadN(char *filename, neuron **net, int *netlen)
{
	int nodeln = 10*sizeof(char) + 9*sizeof(int) + 1;
	int blen;
	neuron *ptr;
	char f[10];
	char *b; 
	char *string;
	if (!(string = binFileRead(filename, &blen))) {
		printf("Can not open file %s\n", filename);
		return -1;
	}
	b = string;
	b = b + 7;
	blen -= 7;
	int netlenToBe = (int)*((int *)b);
	b = b + 4;
	blen -= 4;
	for (int i = 0; i < netlenToBe; i++) {
		memcpy(f, b + i*nodeln, 10*sizeof(char));
		ptr = createN(f, net, netlen);
		memcpy(ptr->p, b + 10*sizeof(char) + i*nodeln, 9*sizeof(int));
	}
	free(string);
	return 0;
}

/* Reads binary file and return strig with content
 * fileln will contain length in bytes
 */
char *binFileRead(char *filename, int *fileln)
{
	FILE *fp;
	char *textFile;
	int textFileSize;

	// Open file
	if (!(fp = fopen(filename, "r"))) {
		return 0;
	}

	// Find out file length
	fseek(fp, 0L, SEEK_END);
	textFileSize = ftell(fp);
	*fileln = textFileSize;
	fseek(fp, 0L, SEEK_SET);
	
	// Allocate necessary space
	textFile = (char *)malloc(textFileSize+1);

	// Read file into string and put '\0' to end
	fread(textFile, textFileSize, 1, fp);
	textFile[textFileSize] = '\0';
	fclose(fp);
	return textFile;
}

/* Writes binary file over */
int binFileWrite(char *filename, char *string, int strln)
{
	FILE *fp;

	// Open file
	fp = fopen(filename, "w+");

	// Write sring into file and close
	for (int i = 0; i < strln; i++) {
		int c = (char)*(string+i);
		fputc(c, fp);
	}
	fclose(fp);
	return 0;
}
