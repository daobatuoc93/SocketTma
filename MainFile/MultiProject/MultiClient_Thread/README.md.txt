##at Server:
#>gcc -o Server Server.c -lpthread
#>./Server [port] 
#ex: Server 4567


##at Client:
#>gcc -o Client Client.c -lpthread 
#>./Client IP_linux(local) Server Port
#ex:>./Client 127.31.42.6 4567