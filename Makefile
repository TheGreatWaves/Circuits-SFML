sim: digital_serializer.cpp
	g++ -g -O2 $^ -o $@


.PHONY: clean
clean:
	rm -rf *.o *~ sim
