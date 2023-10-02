sim: digital_serializer.cpp pin.cpp -lsfml-graphics -lsfml-window -lsfml-system
	g++ -g $^ -o $@


.PHONY: clean
clean:
	rm -rf *.o *~ sim
