#! /bin/sh
DIRNAME=$(cd $(dirname $0);pwd)

ANTLR=${DIRNAME}/external/antlr/antlr-3.4-complete.jar
ANTLR_SRC=${DIRNAME}/src/chisa/donut/parser
ANTLR_OUT=${ANTLR_SRC}/output

if [ $MSYSTEM = MINGW32 ] ; then
	CURDIR=${DIRNAME} \
	ANTLR_SRC=${ANTLR_SRC} \
	ANTLR_OUT=${ANTLR_OUT} \
	ANTLR=${ANTLR} \
	cmd << EOF
set CLASSPATH=%CURDIR%\external\antlr\antlr-3.4-complete.jar;%CLASSPATH%
java org.antlr.Tool -fo %ANTLR_OUT% -make %ANTLR_SRC%/Donut.g %ANTLR_SRC%/Compiler.g

for /F "usebackq" %%s in (\`echo %ANTLR_OUT% ^\| sed s_/_\\\\\_g\`) do set ANTLR_OUT_W=%%s
ren %ANTLR_OUT_W%\*.c *.cpp
EOF
else
	CLASSPATH=${ANTLR}:$CLASSPATH \
	java org.antlr.Tool -fo ${ANTLR_OUT} -make \
		${ANTLR_SRC}/Donut.g ${ANTLR_SRC}/Compiler.g
	
	find ${ANTLR_OUT} -type f -name *.c | while read f; do
	  mv $f ${ANTLR_OUT}/$(basename $f .c).cpp
	done
fi


