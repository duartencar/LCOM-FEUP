cd src
echo " ----------------	downloading liblm.a...	----------------"
curl -o liblm.a http://dl.dropbox.com/s/g427x78jpe7o8ni/liblm.a?dl=1 -L -O -k -s
echo " ----------------	clean previous files...	----------------"
make clean
echo " ----------------		make...		----------------"
make
cd ..
cd ..
echo " ----------------	downloading images... 	----------------"
curl http://dl.dropbox.com/s/ntabt3ctdtcifvv/images.zip -L -O -k -s
echo " ----------------	extracting images...	----------------"
unzip -q images.zip -d `pwd`/proj/src
cd proj/src
chmod 777 run.sh
chmod 777 run_p2.sh
echo " ----------------	Penaltix installed, Enjoy!	--------"
