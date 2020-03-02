svn checkout svn://svn.valgrind.org/valgrind/trunk
cd trunk
curl https://bugsfiles.kde.org/attachment.cgi?id=105218 > patch.txt
svn patch patch.txt
./autogen.sh
./configure
make
make install
