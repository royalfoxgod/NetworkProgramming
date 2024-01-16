#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<arpa/inet.h>
#include	<ctype.h>

#define SERV_PORT 9999

int
main(){
	int i,j,n,nready;
	int maxfd = 0;
	int listenfd,connfd;
	char buf[BUFSIZ];
	struct sockaddr_in clie_addr,serv_addr;
	socklen_t clie_addr_len;

	listenfd = socket(AF_INET,SOCK_STREAM,0);
	int opt = 1;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERV_PORT);
	bind(listenfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	listen(listenfd,128);

	fd_set rset,allset;
	maxfd = listenfd;
	FD_ZERO(&allset);
	FD_SET(listenfd,&allset);

	while(1){
		rset = allset;
		nready = select(maxfd+1,&rset,NULL,NULL,NULL);
		if(nready<0)
			perror("select error");
		if(FD_ISSET(listenfd,&rset)){
			clie_addr_len = sizeof(clie_addr);
			connfd = accept(listenfd,(struct sockaddr*)&clie_addr,&clie_addr_len);
			FD_SET(connfd,&allset);

			if(maxfd<connfd)
				maxfd = connfd;
			if(0==--nready)	continue;
		}
		for(i = listenfd+1;i<=maxfd;i++){
			if(FD_ISSET(i,&rset)){
				if((n=read(i,buf,sizeof(buf)))==0){
					close(i);
					FD_CLR(i,&allset);
				}else if(n>0){
					for(j = 0;j<n;j++)
						buf[j] = toupper(buf[j]);
					write(i,buf,n);
					write(STDOUT_FILENO,buf,n);
				}
			}
		}
	}
}

