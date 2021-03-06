#define N_ELEMENTI_BUFFER 100
#define N_X 130
#define N_Y 43
#define BUFFER_PIENO -1
#define N_PROC_DEFAULT 1 
#define PERMESSI_CODA 0666
#define PERMESSI_REPOSITORY 0642
#define PERMESSI_SEMAFORI 0642
#define FTOK_PATH "/usr"
#define RLE_FILES_NUMBER 10
#define N_PERIOD_TO_CHECK 5

#define RLE_FILE_DIR "RLE/"
#define RLE_FILE_EXT ".lif"

#define TERMINAL_GEOMETRY_FULL_SCREEN "160x48"
#define TERMINAL_GEOMETRY_SMALL "90x30"
#define TERMINAL_PATH "/usr/bin/xfce4-terminal"


#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
    int val;                  /* value for SETVAL */
    struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array;    /* array for GETALL, SETALL */
    /* Linux specific part: */
    struct seminfo *__buf;    /* buffer for IPC_INFO */
};
#endif




typedef enum { VIVO, MORTO, ICS } stato;
typedef enum { ERROR, OKK } status;
//typedef enum { TRUE, FALSE } bool;
typedef enum { UPLOAD, DOWNLOAD, AGGANCIO, ADDIO } richiesta;

typedef struct {
	int v;
	time_t t;
	pid_t p;
}buffer_entry;

typedef struct {
        buffer_entry buffer[N_ELEMENTI_BUFFER];
        int first;
        int n;
	int size;
}buffer;

//typedef struct {
  //      short  m[N_X][N_Y];
//}matrix;
typedef unsigned short matrix[N_X][N_Y];
typedef struct nodo* lista;

struct nodo{
	pid_t p;
	time_t t;
	lista nxt;
};

typedef struct {
	long type;
	pid_t p;
	richiesta r;
}msg_rqst;

typedef struct {
	long type;
	pid_t p;
	richiesta r;
	int shm_id;
	int shm_id_copy;
	int sem_id;
	int sem_id_2;
	int sem_id_counter;
	int sh_gen_id;
	int sh_rule_id;
	int start_x;
	int portion;
	pid_t pid_gr;
}msg_rspns;

typedef struct {
	short rule_s[8];
	short rule_b[8];
	short  size_rule_s;
	short  size_rule_b;
	short sierpinski;

} rule;
