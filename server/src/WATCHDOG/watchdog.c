/***************************************************************
Made By Hong-SeungHyeon
Date: 2020-10-19(월)
***************************************************************/

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define PROC_NAME_SIZE  128
#define PROC_ARG_SIZE   16
#define PROC_ARG_CNT    4
#define MAX_PROC_CNT    100
#define BUFF_SIZE       128
#define TOK_MAX_COUNT   20
#define PROC            "PROC"
#define DMN_HOME		"/home/fms/server"
#define DMN_CONF_PATH   DMN_HOME"/conf"
#define PROC_TBL        DMN_CONF_PATH"/watchdog.proc"
#define CURRENT_VERSION "1.0.0"

typedef struct _proc_table {
    int ppid;                                   /* proc id      */
    int arg_cnt;                                /* args cnt     */
    char pname[PROC_NAME_SIZE];                 /* proc name    */
    char pargs[PROC_ARG_CNT][PROC_ARG_SIZE];    /* argument     */
} PROC_T;

PROC_T proc_t[MAX_PROC_CNT];

char   g_process_name[32];


static void watchdog_exit(void);
static void watchdog_stop(void);
static int get_proc_name(PROC_T *proc_t, char *conf_file, int proc_n);
static int proc_exec(PROC_T *proc_t);
static int one_demon_exec (PROC_T *proc_tb, int idx);
static int l_str_parser(const char *src_str , const char *tok , char *des_str[]);
static void write_proc_id(PROC_T *proc_t);
static int demon_execute(PROC_T proc_t);
int same_process(const char *process);
int init_daemon(void);
void init_signal(void);
void handler(int signo);
static void sig_chld(int signo);
void err_exit(const char *format, ...);
void out_print(const char *format, ...);
void err_print(const char *format, ...);
void print_end_log(void);

static void usage(void)
{
    fprintf(stderr, "\tUsage: %s [start|restart|stop|version] \n", g_process_name);
    fprintf(stderr, "\tex) %s start\n", g_process_name);
    fprintf(stderr, "\tex) %s restart\n", g_process_name);
    fprintf(stderr, "\tex) %s stop\n", g_process_name);
    fprintf(stderr, "\tex) %s version\n", g_process_name);
}

/* ---------------------------------------------------------------------- 
 * DESC : 1. process name이 등록된 테이블을 읽는다.
 *        2. 등록된 process를 실행시킨다.
 *        3. 실행한 process를 감시한다.
 *        4. 시간체크해서 날자가 바뀌면 모든 프로세를 다시 실행시킨다
 * ---------------------------------------------------------------------- */
int main(int argc, char * argv[])
{
    int  proc_cnt, idx, rpid;
    char log_file_name[128] = {0x00, };
    char temp[128] = {0x00, };
    char *exec_proc = NULL;
    char *tmp = NULL;

    memset(g_process_name, 0x00, sizeof(g_process_name));
    tmp = strdup(argv[0]);
    if(tmp == NULL)
    {
        fprintf(stderr, "프로세스 메모리 할당 실패..(%s)\n", strerror(errno));
        exit(0);
    }
    strcpy(g_process_name, (char *)basename(tmp));
    free(tmp);

    if(argc != 2) {
        usage();
        return -1;
    }

    if(!strncmp("version", argv[1], 7) || !strncmp("v", argv[1], 1) || !strncmp("V", argv[1], 1)) {
        fprintf(stdout, "Current WATCHDOG version [%s]\n\n",CURRENT_VERSION);
        exit(0);
    }
	
	/* root가 아닐때도 실행가능하게 주석 처리
    uid_t who;
    who = getuid();
    if(who != 0) {
        fprintf(stderr, "루트권한으로만 실행 가능.. 유저아이디(%d)\n", (int)who);
        exit(0);
    }
	*/

    if(!strncmp("start", argv[1], strlen("start"))) 
	{
        if(same_process((const char *)g_process_name) == 0) 
		{
            fprintf(stderr, "프로그램이 이미 실행중입니다.\n");
            exit(0);
        }
    }
    else if(!strncmp("restart", argv[1], strlen("restart"))) 
	{
        watchdog_stop();
    }
    else if(!strncmp("stop", argv[1], strlen("stop"))) 
	{
        watchdog_exit();
    }
    else 
	{
        usage();
        return -1;
    }

    fprintf(stdout, "\n데몬 프로세스 시작..\n");
    snprintf(log_file_name, sizeof(log_file_name)-1, "WATCHDOG");

    printf("%s 을 시작합니다. ", g_process_name);
    fflush(stdout);

    /* work of demon    */
    if(init_daemon() < 0)
    {
        fprintf(stdout, "[ \x1b[33m실패\x1b[0m ]\n"
                        "데몬 프로세스 생성 실패 [%s]", strerror(errno));
        fflush(stdout);
        return -1;
    }

    init_signal();

    /* 실행할 proc name을 읽어온다. */
    memset(proc_t, 0x00, sizeof(proc_t));

	proc_cnt = get_proc_name(proc_t, PROC_TBL, MAX_PROC_CNT);

    if(proc_cnt < 0) {
        fprintf(stdout, "[ \x1b[33m실패\x1b[0m ]\n");
        fprintf(stdout, "실행할 프로세스 정보가 없습니다.\n");
        fflush(stdout);
        goto WATCHDOG_END;
    }

    fprintf(stdout, "[ \x1b[33m확인\x1b[0m ]\n");
    fprintf(stdout, "실행 프로세스 개수(%d)\n", proc_exec(proc_t));
    fflush(stdout);

    while(1) {
        sleep(10);
        for(idx=0; proc_t[idx].ppid >= 0; idx++) {
            memset(temp, 0x00, sizeof(temp));
            strcpy(temp, proc_t[idx].pname);
            exec_proc = (char *)basename(temp);

            if(same_process((const char *)exec_proc) != 0) {
                printf("(%s) 프로세스 is not running..\n", exec_proc);
                if((rpid = one_demon_exec(proc_t, idx)) > 0) {
                    printf("(%s) 프로세스 재시작 PID(%d)\n", exec_proc, rpid);
                }
                fflush(stdout);
                sleep(1);
            }
            exec_proc = NULL;
        }
    }

WATCHDOG_END:

    watchdog_exit();
    
    return 0;
}

static void watchdog_stop(void)
{
    char file_name[128]={0x00, }, proc_path[128]={0x00, };
    char rbuff[256]={0x00, }, proc_name[128]={0x00, };
    int  proc_id;
    FILE *fp=NULL;
    FILE *ff=NULL;
	char *p;

    snprintf(file_name, sizeof(file_name)-1, "%s/%s.pid", DMN_CONF_PATH, "watchdog");
    fp = fopen(file_name, "r");
    if(fp) {
        while(fgets(rbuff, sizeof(rbuff), fp)) {
            memset(proc_name, 0x00, sizeof(proc_name));
            proc_id = -1;
            sscanf(rbuff, "%s %d", proc_name, &proc_id);
            sprintf(proc_path, "/proc/%d/status", proc_id);
            ff = fopen(proc_path, "r");
            if(ff) {
                memset(rbuff, 0x00, sizeof(rbuff));
                p = fgets(rbuff, sizeof(rbuff), ff);
				if(p != NULL)
				{
					if(strstr((const char *)rbuff, (const char *)proc_name)) {
						printf("%s 을 종료합니다.  ", proc_name);
						if(kill((pid_t)proc_id, SIGTERM) == 0) {
							printf("[ \x1b[33m확인\x1b[0m ]\n");
						} else {
							printf("[ \x1b[33m실패\x1b[0m ]\n");
						}
						fflush(stdout);
						usleep(500000);
					}
				}
                fclose(ff);
            }
        }
        fclose(fp);
    }

    unlink(file_name);

    printf("데몬 프로세스 종료..\n");
    fflush(stdout);
}

/* ---------------------------------------------------------------------- 
 * NAME : watchdog_exit
 * DESC : 종료 할때 필요한 일들을 해주고 종료한다.
 * ARGS : void
 * RTN  : void
 * ---------------------------------------------------------------------- */
static void watchdog_exit(void)
{
    watchdog_stop();
    exit(0);
}

/* ---------------------------------------------------------------------- 
 * NAME : one_demon_exec
 * DESC : 하나의 process를 뛰워준다
 * ARGS : 1. proc_tb - process가 등록된 table
 *        2. idx     - 실행하고 싶은 process의 index
 * RTN  : success - 실행한 process의 pid
 *        fail    - -1
 * ---------------------------------------------------------------------- */
static int one_demon_exec (PROC_T *proc_tb, int idx)
{
    int pid;

    if((pid = fork ()) < 0) {
        return -1;
    }
    else if(pid == 0) { /* child process    */
        demon_execute(proc_tb[idx]);
        exit(0);
    }
    else {
        proc_tb[idx].ppid = pid;
    }

    write_proc_id(proc_tb);

    return pid;
}

/* ---------------------------------------------------------------------- 
 * NAME : get_proc_name
 * DESC : 실행 파일과 argument를 읽어온다.
 * IN   : 1. PROC_T - struct _proc_table * proc_t - 읽어서 return 할 구조체
 *        2. char * conf_file            - configure file path
 *        3. char * bin_path             - 실행 파일 위치
 *        4. int proc_n                  - proc_t 배열의 크기
 * OUT  : success - 실행 프로세스 갯수
 *        fail    - -1
 * ---------------------------------------------------------------------- */
static int get_proc_name(PROC_T *proc_t, char *conf_file, int proc_n)
{
    FILE *fp = NULL;
    char buff[BUFF_SIZE];
    char *tok[TOK_MAX_COUNT] = {NULL, };
    int cnt, i;
	int proc_cnt=0;

    if((fp = fopen(conf_file, "r")) == NULL) {
        return -1;
    }

    while(fgets(buff, sizeof(buff), fp) != NULL) {
        if(buff[0] == '\n' || buff[0] == '#') {
            continue;
        }

        if((cnt = l_str_parser(buff, " \t\n", tok)) > 0) {
            if(cnt < 2) {
                continue;
            }

            if(memcmp(tok[0], PROC, strlen(tok[0]))!=0) {
                continue;
            }

            sprintf(proc_t[proc_cnt].pname,"%s",tok[1]);
            if(cnt > PROC_ARG_CNT+2) {
                cnt = PROC_ARG_CNT;
            }

            if(cnt > 2) {
                for(i=2; i<cnt; i++) {
                    sprintf(proc_t[proc_cnt].pargs[i-2], "%s", tok[i]);
                }
            }
            proc_t[proc_cnt].arg_cnt = cnt-2;
            errno = 0;
            if(proc_cnt >= proc_n-1) {
                return -1;
            }
            proc_cnt++;
        }
    }

    for(i=proc_cnt; i < proc_n;  proc_t[i++].ppid = -1);

    fclose(fp);

    return proc_cnt;
}

/* ---------------------------------------------------------------------- 
 * NAME : proc_exec
 * DESC : proc_t table에 등록된 프로세스들을 실행 시켜준다.
 * IN   : PROT_T * proc_t - process table
 * OUT  : success - 실행 시킨 프로세스 수
 *        fail    - -1
 * ---------------------------------------------------------------------- */
static int proc_exec(PROC_T * proc_t)
{
    int pcnt;
    int pid;
    char temp[128]={0x00,  }, *base_name=NULL;

    for(pcnt=0; proc_t[pcnt].ppid == 0; pcnt++) {
        strcpy(temp, proc_t[pcnt].pname);
        base_name = (char *)basename(temp);
        printf("%s 을 시작합니다.  ", base_name);
        fflush(stdout);
        if((pid = fork()) < 0) {
            printf("[ \x1b[33m실패\x1b[0m ]\n");
            fflush(stdout);
            return -1;
        }
        else if(pid == 0) { /* child process */
            demon_execute(proc_t[pcnt]);
            exit(0);
        }
        else {
            printf("[ \x1b[33m확인\x1b[0m ]\n");
            proc_t[pcnt].ppid = pid;
        }
        fflush(stdout);
        sleep(1);
    }

    write_proc_id(proc_t);

    return pcnt;
}

/* --------------------------------------------------------------------------
 * NAME   : l_str_parser
 * DESC   : src_str을 tok 단위로 나누어서 des_str에 저장
 * IN     : const char * src_str - 문자열
 *          const char * tok     - 자르고자 하는 charactor
 *          char * des_str[]     - tok로 잘려진 toked들..
 * RETURN : 성공 - des_src 개수
 *          처리 할수 없음(skip해야 될 라인) - 0
 *          실패 - (-1)
 * -------------------------------------------------------------------------- */
static int l_str_parser(const char *src_str , const char *tok , char *des_str[])
{
    int i, len;

    if((len = strlen(src_str)) == 0)
    {
        return len;
    }

    if(src_str[0] == '\n' || src_str == NULL || src_str[0] == ' ' || src_str[0] == '#')
    {
        return 0;
    }

    des_str[0] = (char *)strtok((char *)src_str, tok);
    for(i=1; ;i++)
    {
        des_str[i] = (char *)strtok(NULL, tok);
        if(des_str[i] == NULL)
            break;
        else if(des_str[i][0] == '#')
        {
            des_str[i][0]='\0';
            break;
        }
    }

    return i;
}

static void write_proc_id(PROC_T *proc_t)
{
    int  pcnt;
    char file_name[128]={0x00, }, temp[128]={0x00, }, wformat[128]={0x00, };
    char *proc_name = NULL;
    FILE *fp = NULL;

    snprintf(file_name, sizeof(file_name)-1, "%s/%s.pid", DMN_CONF_PATH, "watchdog");
    fp = fopen(file_name, "w");
    if(fp) {
        /*자기자신의 프로세스이름과 아이디를 처음 적는다.*/
        memset(wformat, 0x00, sizeof(wformat));
        sprintf(wformat, "%s %d\n", g_process_name, (int)getpid());
        fwrite((const void *)wformat, strlen(wformat), sizeof(char), fp);
        for(pcnt=0; proc_t[pcnt].ppid != -1; pcnt++) {
            memset(wformat, 0x00, sizeof(wformat));
            /* wformat */
            /* ProcessName ProcessID */
            /* RTUD_col 75 */
            memset(temp, 0x00, sizeof(temp));
            strcpy(temp, proc_t[pcnt].pname);
            proc_name = (char *)basename(temp);
            sprintf(wformat, "%s %d\n", proc_name, (int)proc_t[pcnt].ppid);
            fwrite((const void *)wformat, strlen(wformat), sizeof(char), fp);
            proc_name = NULL;
        }
        fclose(fp);
    }
}

/* ---------------------------------------------------------------------- 
 * NAME : demon_execute
 * DESC : argument수에 따라 프로세스를 실행 시켜준다. 
 *        argument가 4보다 크면 에러
 * IN   : PROC_T proc_t - 실행시킬 프로세스 
 * OUT  : success - 0
 *        fail    - -1
 * ---------------------------------------------------------------------- */
static int demon_execute(PROC_T proc_t)
{
    switch(proc_t.arg_cnt) {
        case 0:
            if(execl(proc_t.pname, proc_t.pname, NULL) < 0)
                return -1;
            break;
        case 1: 
            if(execl(proc_t.pname, proc_t.pname, proc_t.pargs[0], NULL) < 0)
                return -1;
            break;
        case 2:
            if(execl(proc_t.pname, proc_t.pname, proc_t.pargs[0], proc_t.pargs[1], NULL) < 0)
                return -1;
            break;
        case 3:
            if(execl(proc_t.pname, proc_t.pname, proc_t.pargs[0],
                            proc_t.pargs[1], proc_t.pargs[2], NULL) < 0)
                return -1;
            break;
        case 4:
            if(execl(proc_t.pname, proc_t.pname, proc_t.pargs[0], 
                            proc_t.pargs[1], proc_t.pargs[2], proc_t.pargs[3], NULL) < 0)
                return -1;
            break;
        default:
            return -1;
            break;
    }

    return 0;
}

int same_process(const char *process)
{
    pid_t   pid;
    int retValue = -1;
    char filename[512] = {0x00, };
    char cmdbuff[256]  = {0x00, };
    char temp01[256];
    char temp02[256];
    struct dirent *dirent = NULL;
    struct stat buf;
    DIR *dir = NULL;
    FILE *fp = NULL;
	char *p;

    if((dir = opendir((const char *)"/proc")) == NULL)
    {
        fprintf(stderr, "/proc directory open error!! (%s)\n", (char *)strerror(errno));
        return retValue;
    }

    pid = getpid();
    while((dirent = readdir(dir)) != NULL)
    {
        if(!strncmp(dirent->d_name, ".", 1) || !strncmp(dirent->d_name, "..", 2))
        {
            continue;
        }
        if(pid == atoi(dirent->d_name))
        {
            continue;
        }

        memset (filename, 0x00, sizeof(filename));
        sprintf(filename, "/proc/%s", dirent->d_name);

        lstat(filename, &buf);

        if(S_ISDIR(buf.st_mode) && !S_ISLNK(buf.st_mode))
        {
            strcat(filename, "/status");
            if((fp = fopen(filename, "r")) == NULL)
            {
                continue;
            }

            memset(cmdbuff, 0x00, sizeof(cmdbuff));
            p = fgets(cmdbuff, sizeof(cmdbuff), fp);
			
            fclose(fp);
			
			if(p != NULL)
			{
				memset(temp01, 0x00, sizeof(temp01));
				memset(temp02, 0x00, sizeof(temp02));
				sscanf(cmdbuff, "%s %s", temp01, temp02);
			}

            if(!strcmp(temp02, process))
            {
                retValue = 0;
                break;
            }
        }
    }

    if(closedir(dir) != 0)
    {
        fprintf(stderr, "close directory error!! (%s)\n", (char *)strerror(errno));
    }

    fflush(stderr);
    return retValue;
}

/*
 * Description:
 *
 * Parameters:
 *      void
 * Returns:
 *      0 - 성공
 *      -1 - 실패
 */
int init_daemon(void)
{
    pid_t pid;

    if((pid = fork()) < 0)
        return -1;
    else if(pid != 0)   /* 부모일 경우 exit */
        exit(0);

    /*close(STDIN_FILENO); */
    /*close(STDOUT_FILENO);*/
    /*close(STDERR_FILENO);*/

    setsid();
    /*chdir("/");*/
    umask(0);

    return 0;
}


/*
 * Description:
 *      시그널 처리 함수
 * Parameters:
 *      void
 * Returns:
 *      void
 */
void init_signal(void)
{
    struct sigaction sig_act;
    struct sigaction ign_act;
    struct sigaction cld_act;
    
    sig_act.sa_handler = handler;
    sig_act.sa_flags = 0;
    sigemptyset(&sig_act.sa_mask);

    ign_act.sa_handler = SIG_IGN;
    ign_act.sa_flags   = 0;
    sigemptyset(&ign_act.sa_mask);

    cld_act.sa_handler = sig_chld;
    cld_act.sa_flags = 0;
    sigemptyset(&cld_act.sa_mask);

    if(sigaction(SIGHUP,   &ign_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGINT,   &ign_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGQUIT,  &ign_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGILL,   &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGTRAP,  &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGABRT,  &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGBUS,   &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGFPE,   &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGUSR1,  &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGSEGV,  &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGUSR2,  &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGPIPE,  &ign_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGALRM,  &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGTERM,  &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGCHLD,  &cld_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGCONT,  &ign_act, NULL) == -1) err_exit("sigaction failed\n");
    /*if(sigaction(SIGSTOP,  &sig_act, NULL) == -1) err_exit("sigaction failed\n");*/
    if(sigaction(SIGTSTP,  &ign_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGTTIN,  &ign_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGTTOU,  &ign_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGURG,   &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGXCPU,  &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGXFSZ,  &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGVTALRM,&sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGPROF,  &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGWINCH, &ign_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGIO,    &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGPWR,   &sig_act, NULL) == -1) err_exit("sigaction failed\n");
    if(sigaction(SIGSYS,   &sig_act, NULL) == -1) err_exit("sigaction failed\n");
}


/*
 * Description:
 *      시그널 발생시 처리 루틴.
 * Parameters:
 *      signo - 시그널 번호
 * Returns:
 *      void
 */
void handler(int signo)
{
    switch (signo)
    {
        case SIGSEGV:
            out_print("SIGSEGV(세그멘테이션 위반). process exit.\n");
            break;
        case SIGTERM:
            out_print("SIGTERM(소프트웨어 종료). process exit.\n");
            break;
        case SIGKILL:
            out_print("SIGKILL(강제종료). process exit.\n");
            break;
        case SIGQUIT:
            out_print("SIGQUIT(종료). process exit.\n");
            break;
        case SIGBUS:
            out_print("SIGBUS(버스 에러). process exit.\n");
            break;
        default :
            out_print("(%s/%d) signal catched. process exit.\n", strsignal(signo), signo);
            break;
    }
    exit(0);
}

/*
 * Description:
 *      SIGCHLD 시그널 발생시 처리 루틴.
 * Parameters:
 *      signo - 시그널 번호
 * Returns:
 *      void
 */
static void sig_chld(int signo)
{
  int status, pid;

  if((pid = waitpid(-1, &status, WNOHANG)) < 0)
  {
      out_print("(%s/%d) signal catched. waitpid failed(%s)\n", 
            strsignal(signo), signo, strerror(errno));
      return;
  }

  if(WIFEXITED(status))           /* normal exit */
      out_print("child's exited normally with status %d\n", WEXITSTATUS(status));
  else if(WIFSIGNALED(status))    /* signal exit */
      out_print("abnormal termination, signal number = %d\n", WTERMSIG(status));
}

/*
 * Description:
 *      표준 에러 출력으로 포맷화 된 로그와 종료 로그를 출력 후
 *      프로그램을 종료한다.
 * Parameters:
 *      format - 로그를 출력할 포맷
 * Returns:
 *      void
 */
void err_exit(const char *format, ...)
{
    va_list args;
    struct timeval now;
    struct timezone tz;
    struct tm tm, *tp = NULL;

    gettimeofday(&now, &tz);
    tp = localtime_r(&now.tv_sec, &tm);
    fprintf(stderr, "[%02d:%02d:%02d] ", tp->tm_hour, tp->tm_min, tp->tm_sec);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fflush(stderr);
    print_end_log();
    exit(1);
}

/*
 * Description:
 *      표준 출력으로 포맷화 된 로그와 종료 로그를 출력 한다.
 * Parameters:
 *      format - 로그를 출력할 포맷
 * Returns:
 *      void
 */
void out_print(const char *format, ...)
{
    va_list args;
    struct timeval now;
    struct timezone tz;
    struct tm tm, *tp = NULL;

    gettimeofday(&now, &tz);
    tp = localtime_r(&now.tv_sec, &tm);
    fprintf(stdout, "[%02d:%02d:%02d] ", tp->tm_hour, tp->tm_min, tp->tm_sec);
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    fflush(stdout);
}

/*
 * Description:
 *      표준 에러 출력으로 포맷화 된 로그와 종료 로그를 출력 한다.
 * Parameters:
 *      format - 로그를 출력할 포맷
 * Returns:
 *      void
 */
void err_print(const char *format, ...)
{
    va_list args;
    struct timeval now;
    struct timezone tz;
    struct tm tm, *tp = NULL;

    gettimeofday(&now, &tz);
    tp = localtime_r(&now.tv_sec, &tm);
    fprintf(stderr, "[%02d:%02d:%02d] ", tp->tm_hour, tp->tm_min, tp->tm_sec);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fflush(stderr);
}

/*
 * Description:
 *      프로그램 종료 로그를 출력한다.
 * Parameters:
 *      void
 * Returns:
 *      void
 */
void print_end_log(void)
{
    fprintf(stdout, "+------------------------------------------------------------------------------+\n"
                    "+     PROCESS SHUTDOWN COMPLETE !!                                             +\n"
                    "+------------------------------------------------------------------------------+\n\n\n");
    fflush(stdout);
}