
cc=g++
include= -I ./include/
LIB = -L ./lib -lTapTradeAPI -lTapQuoteAPI -lpthread
GLIB = `pkg-config glib-2.0 --cflags --libs`
obj = Quote.o Trade.o SimpleEvent.o strategy.o StructFunction.o config.o
target = trading

$(target): main.cpp $(obj)
	$(cc) -o $(target) $(obj) main.cpp $(include) $(LIB) $(GLIB)
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib

Quote.o: Quote.cpp Quote.h
	$(cc) -c Quote.cpp -g

Trade.o: Trade.cpp Trade.h
	$(cc) -c Trade.cpp -g

strategy.o: strategy.cpp strategy.h
	$(cc) -c strategy.cpp -g

config.o: config.cpp config.h
	$(cc) -c config.cpp $(GLIB)

StructFunction.o: StructFunction.cpp StructFunction.h
	$(cc) -c StructFunction.cpp -g

SimpleEvent.o: SimpleEvent.cpp SimpleEvent.h
	$(cc) -c SimpleEvent.cpp
	

.PHONY : clean
clean:
	rm $(obj) $(target)

