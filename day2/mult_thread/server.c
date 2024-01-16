#include	<stdio.h>
#include	<unistd.h>
#include	<string.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<pthread.h>


#define MAXLINE	80
#define SERV_PORT 9999

struct s_info{
	struct sockaddr_in cliaddr;
	int connfd;
};
void*
do_work(void *arg)
{
	int n,i;
	struct s_info *ts = (struct s_info*)arg;
	char buf[MAXLINE];
	char str[BUFSIZ];
	pthread_detach(pthread_self());
	while(1){
		n = read(ts->connfd,buf,MAXLINE);
		if(n==0){
			printf("the other side has been closed.\n");
			break;
		}
		printf("received from %s at PORT %d\n",
			inet_ntop(AF_INET,&(*ts).cliaddr.sin_addr,str,sizeof(str)),
			ntohs((*ts).cliaddr.sin_port));
		for(i = 0;i<n;i++)
			buf[i] = toupper(buf[i]);
		write(ts->connfd,buf,n);
		write(STDOUT_FILENO,buf,n);
	}
	close(ts->connfd);
}
int
main()
{
	struct sockaddr_in servaddr,cliaddr;
	socklen_t cliaddr_len;
	int listenfd,connfd;
	int i = 0;
	pthread_t tid;
	struct s_info ts[256];
	
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	
	bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	listen(listenfd,20);
	printf("Accepting connections...\n");
	while(1){
		connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&cliaddr_len);
		ts[i].cliaddr = cliaddr;
		ts[i].connfd = connfd;
		pthread_create(&tid,NULL,do_work,(void*)&ts[i]);
		i++;
	}
	return 0;
}
