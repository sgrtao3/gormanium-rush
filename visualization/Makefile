CPPFLAGS = "-I${GRAPHVIZ_HOME}/include"
LDFLAGS = "-L${GRAPHVIZ_HOME}/lib"

all: example

e: graphviz_example.cpp
	${CXX} ${CPPFLAGS} ${CXXFLAGS} graphviz_example.cpp ${LDFLAGS} -lgvc -lcgraph -o example