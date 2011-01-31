#echo 'http://curl.haxx.se/' >> event.fifo
#echo 'ftp://cool.haxx.se/' >> event.fifo
#echo 'http://www.contactor.se/' >> event.fifo
#echo 'www.haxx.se' >> event.fifo

echo 'CACHE http://curl.haxx.se/
FETCH    ftp://cool.haxx.se/ 
  FETCH http://www.contactor.se/  
 FETCH www.haxx.se' >> /usr/local/sw/queral/var/run/queral.fifo

echo 'CACHE http://ws1.softactiva.com/' >> /usr/local/sw/queral/var/run/queral.fifo
