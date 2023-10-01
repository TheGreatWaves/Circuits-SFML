sim: digital_serializer.cpp pin.cpp
	g++ -g $^ -o $@


.PHONY: clean
clean:
	rm -rf *.o *~ sim
