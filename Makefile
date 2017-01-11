
cc=g++
include= -I ./include/
LIB = -L ./lib -lTapTradeAPI -lTapQuoteAPI -lpthread
obj = Quote.o Trade.o SimpleEvent.o strategy.o StructFunction.o
target = test

$(target): Demo.cpp $(obj)
	$(cc) -o $(target) $(obj) Demo.cpp $(include) $(LIB) 
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib

Quote.o: Quote.cpp Quote.h
	$(cc) -c Quote.cpp -g

Trade.o: Trade.cpp Trade.h
	$(cc) -c Trade.cpp -g

strategy.o: strategy.cpp strategy.h
	$(cc) -c strategy.cpp -g

StructFunction.o: StructFunction.cpp StructFunction.h
	$(cc) -c StructFunction.cpp -g

SimpleEvent.o: SimpleEvent.cpp SimpleEvent.h
	$(cc) -c SimpleEvent.cpp
	

.PHONY : clean
clean:
	rm $(obj) $(target)

