#include	<stdio.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include	<string.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<signal.h>
#include	<sys/wait.h>
#include	<stdlib.h>
#include	<sys/types.h>

#define MAXLINE 80
#define SERV_PORT	9999

void
do_sigchild(int num)
{
	while(waitpid(0,NULL,WNOHANG)>0);
}
int
main()
{
	struct sockaddr_in servaddr,cliaddr;
	socklen_t cliaddr_len;
	int listenfd,connfd;
	char buf[MAXLINE];
	char str[BUFSIZ];
	int i,n;
	pid_t pid;
	
	struct sigaction newact;
	newact.sa_handler = do_sigchild;
	sigemptyset(&newact.sa_mask);
	newact.sa_flags = 0;
	sigaction(SIGCHLD,&newact,NULL);
	
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	
	bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	listen(listenfd,20);
	
	printf("Accepting connections...\n");
	while(1){
		cliaddr_len = sizeof(cliaddr);
		connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&cliaddr_len);
		pid = fork();
		if(pid==0){
			close(listenfd);
			while(1){
				n = read(connfd,buf,MAXLINE);
				if(n==0){
					printf("the other side has been closed.\n");
					break;
				}else if(n<0){
					exit(-1);
				}

				printf("received from %s at PORT %d \n",
					inet_ntop(AF_INET,&cliaddr.sin_addr,str,sizeof(str)),					      ntohs(cliaddr.sin_port));
				for(i = 0;i<n;i++)
					buf[i]=toupper(buf[i]);
				write(connfd,buf,n);
				write(STDOUT_FILENO,buf,n);
			}			close(connfd);
			return 0;
		}else if(pid>0){
			close(connfd);
		}
	}
	close(listenfd);
	return 0;
}	

				
