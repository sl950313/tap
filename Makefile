
cc=g++
include= -I ./include/
LIB = -L ./lib -liTapTradeAPI -lTapQuoteAPI -lpthread
GLIB = `pkg-config glib-2.0 --cflags --libs`
obj = Quote.o Trade.o SimpleEvent.o strategy.o StructFunction.o config.o utiltool.o treaty.o
target = trading

$(target): main.cpp $(obj)
	$(cc) -o $(target) $(obj) main.cpp $(include) $(LIB) $(GLIB) -g -Wwrite-strings *.so
	export LD_LIBRARY_PATH=LD_LIBRARY_PATH:./lib 

Quote.o: Quote.cpp Quote.h
	$(cc) -c Quote.cpp -g -Wwrite-strings

Trade.o: Trade.cpp Trade.h 
	$(cc) -c Trade.cpp $(include) -g

treaty.o: treaty.h treaty.cpp
	$(cc) -c treaty.cpp -g

strategy.o: strategy.cpp strategy.h
	$(cc) -c strategy.cpp -g

config.o: config.cpp config.h
	$(cc) -c config.cpp $(GLIB) -g

StructFunction.o: StructFunction.cpp StructFunction.h
	$(cc) -c StructFunction.cpp -g

SimpleEvent.o: SimpleEvent.cpp SimpleEvent.h
	$(cc) -c SimpleEvent.cpp -g
	
utiltool.o: utiltool.h utiltool.cpp
	$(cc) -c utiltool.cpp -g

.PHONY : clean
clean:
	rm $(obj) $(target)

