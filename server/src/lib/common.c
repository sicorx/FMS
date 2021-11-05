#include "include.h"

void handler(int signo)
{
    switch (signo)
    {
        case SIGSEGV:
            fileLog(WARNING, "SIGSEGV. process exit.\n");
            break;
        case SIGTERM:
            fileLog(WARNING, "SIGTERM. process exit.\n");
            break;
        case SIGKILL:
            fileLog(WARNING, "SIGKILL. process exit.\n");
            break;
        case SIGQUIT:
            fileLog(WARNING, "SIGQUIT. process exit.\n");
            break;
        case SIGBUS:
            fileLog(WARNING, "SIGBUS. process exit.\n");
            break;
        default :
            fileLog(WARNING, "(%s/%d) signal catched. process exit.\n", strsignal(signo), signo);
            break;
    }
    exit(0);
}

void sig_chld(int signo)
{
	int status, pid;

	if((pid = waitpid(-1, &status, WNOHANG)) < 0)
	{
		fileLog(WARNING, "(%s/%d) signal catched. waitpid failed(%s)\n", strsignal(signo), signo, strerror(errno));
		return;
	}

	/* normal exit */
	if(WIFEXITED(status)) fileLog(WARNING, "child's exited normally with status %d\n", WEXITSTATUS(status));
	/* signal exit */
	else if(WIFSIGNALED(status)) fileLog(WARNING, "abnormal termination, signal number = %d\n", WTERMSIG(status));
}

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

    if(sigaction(SIGHUP,   &ign_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGINT,   &ign_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGQUIT,  &ign_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGILL,   &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGTRAP,  &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGABRT,  &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGBUS,   &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGFPE,   &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGUSR1,  &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGSEGV,  &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGUSR2,  &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGPIPE,  &ign_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGALRM,  &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGTERM,  &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGCHLD,  &cld_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGCONT,  &ign_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    /*if(sigaction(SIGSTOP,  &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");*/
    if(sigaction(SIGTSTP,  &ign_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGTTIN,  &ign_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGTTOU,  &ign_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGURG,   &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGXCPU,  &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGXFSZ,  &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGVTALRM,&sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGPROF,  &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGWINCH, &ign_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGIO,    &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGPWR,   &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
    if(sigaction(SIGSYS,   &sig_act, NULL) == -1) err_exit(WARNING, "sigaction failed\n");
}

int set_linger(int socket_fd, int on_off, int linger_timeout)
{
    int status = 0;
    char err_buf[256] = {0x00, };
    struct linger linger;
    linger.l_onoff  = on_off;
    linger.l_linger = linger_timeout;

    status = setsockopt(socket_fd, SOL_SOCKET, SO_LINGER, &linger, (socklen_t)sizeof(linger));
    if(-1 == status)
    {
        fileLog(WARNING, "setsockopt(SO_LINGER) error!! (%s)\n", (char *)strerror_r(errno, err_buf, sizeof(err_buf)));
    }

    return status;
}


int makeServer(unsigned short portnum)
{
    int sockfd, on = 1;
    struct sockaddr_in server_addr;

    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        fileLog(WARNING, "Server Socket Create Fail\n");
        return -1;
    }

    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)) == -1)
    {
        fileLog(WARNING, "Server Socket Option(SO_REUSEADDR) Setting Fail\n");
        return -1;
    }

    set_linger(sockfd, 1, 10); /* version 1.3.32 */

    memset(&server_addr, 0x00, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port        = htons(portnum);

    if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        close(sockfd);
        fileLog(WARNING, "Server Socket Bind Error %s\n", strerror(errno));
        return -1;
    }

    listen(sockfd, LISTEN_QUEUE_SIZE);
    fileLog(INFO, "Server Mode Start: Port=[%d], Socket Fd=[%d]\n", portnum, sockfd);

    return sockfd;
}

char get_hex(signed char value)
{
    char ret = 0;

    if(isxdigit(value) != 0) {
        if(value >= '0' && value <='9')
            ret = value - '0';
        else if(value >= 'A' && value <='F')
            ret = value - 'A' + 0x0A;
        else if(value >= 'a' && value <='f')
            ret = value - 'a' + 0x0A;
    } else if(value >= 0 && value <= 9) {
        ret = value + '0';
    } else if(value >= 10 && value <= 0x0F) {
        ret = value - 10 + 'A';
    }

    return ret;
}

int connectServer(char *remoteHost, unsigned short portnum)
{
    int sockfd, status=0, h_errnop=0;
    char err_buf[256] = {0x00, };
    struct sockaddr_in client_addr;
    struct hostent ret, *result=NULL;
    char buf[1024];

    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        fileLog(CRITICAL, "[%s:%d] create socket error!! remoteHost:%s, portnum:%d, (%s)\n", __FUNCTION__, __LINE__, remoteHost, portnum, (char *)strerror_r(errno, err_buf, sizeof(err_buf)));
        return -1;
    }

    h_errnop = 0;
    gethostbyname_r(remoteHost, &ret, buf, sizeof(buf), &result, &h_errnop);
    if(h_errnop != 0)
    {
        h_errnop = 0;
        gethostbyaddr_r(remoteHost, strlen(remoteHost), AF_INET, &ret, buf, sizeof(buf), &result, &h_errnop);
        if(h_errnop != 0)
        {
            close(sockfd);
            fileLog(CRITICAL, "[%s:%d] error resolving!! remoteHost:%s, portnum:%d, (%s)\n", __FUNCTION__, __LINE__, remoteHost, portnum, (char *)strerror_r(errno, err_buf, sizeof(err_buf)));
            return -1;
        }
    }

    /* close 호출시 소켓이 바로 종료되도록 변경 한다.
     * TIME_WATE 상태로 남는게 싫다. */
    set_linger(sockfd, 1, 10);

    client_addr.sin_family = AF_INET;
    client_addr.sin_port   = htons(portnum);
    (void)memcpy(&client_addr.sin_addr, ret.h_addr, ret.h_length);

    status = connect(sockfd, (struct sockaddr *) &client_addr, sizeof(client_addr));
    if(status == -1 )
    {
        close(sockfd);
        fileLog(CRITICAL, "[%s:%d] connect error!! remoteHost:%s, portnum:%d, (%s)\n", __FUNCTION__, __LINE__, remoteHost, portnum, (char *)strerror_r(errno, err_buf, sizeof(err_buf)));
        return -1;
    }

    return sockfd;
}

void space_rtrim(char *value)
{
    int j;

    for(j = strlen(value) - 1; ; j--)
    {
        if( value[j] == 0x20 || value[j] == '\n' ||
            value[j] == 0x09 || value[j] == 0x0D)   /* 0x0D = CR */
        {
            value[j] = 0x00;
        }
        else
        {
            break;
        }
    }
}

int get_key_value(const char *filename, const char *key, char *value)
{
    int i = 0, len, rvalue = -1;
    char buf[256] = {0x00, };
    FILE *fp = NULL;

    if((len = strlen(key)) == 0 || key == NULL)
    {
        fprintf(stderr, "Key is Empty\n");
        return -1;
    }

    if(value == NULL)
    {
        fprintf(stderr, "Buffer Error\n");
        return -1;
    }

    if((fp = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "Filename=[%s] open error!! (%s)\n", filename, (char *)strerror(errno));
        return -1;
    }

    while(fgets(buf, sizeof(buf), fp) != NULL)
    {
        if(buf[0] == '\n' || buf[0] == '#')
        {
            continue;
        }
        else if(!strncmp(key, buf, len))
        {
            for(i = len; i < (int)strlen(buf); i++)
            {
                /* 0x09[TAB] 0x20[SPACE] */
                if(buf[i] == 0x09 || buf[i] == 0x20 || buf[i] == '=')
                {
                    continue;
                }
                break;
            }
            strncpy(value, buf+i, strlen(buf) - i -1);
            rvalue = 0;
            break;
        }
    }

    if(fclose(fp) != 0)
    {
        fprintf(stderr, "file(%s) close error!! (%s)\n", filename, (char *)strerror(errno));
    }

    /* trim trailing space */
    space_rtrim(value);

    fflush(stderr);

    return rvalue;
}

int getid(void)
{
   uid_t          user_id;
   struct passwd *user_pw;

   user_id  = getuid();
   user_pw  = getpwuid( user_id);

   fileLog(WARNING, "user name      :%s\n", user_pw->pw_name  );
   fileLog(WARNING, "user id        :%d\n", user_pw->pw_uid   );
   fileLog(WARNING, "group id       :%d\n", user_pw->pw_gid   );
   fileLog(WARNING, "reall name     :%s\n", user_pw->pw_gecos );
   fileLog(WARNING, "home directory :%s\n", user_pw->pw_dir   );
   fileLog(WARNING, "shell program  :%s\n", user_pw->pw_shell );
   return user_pw->pw_uid;
}