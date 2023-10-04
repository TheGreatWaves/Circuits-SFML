sim: digital_serializer.cpp pin.cpp gui/wire_gui.cpp gui/belt_gui.cpp -lsfml-graphics -lsfml-window -lsfml-system
	g++ -g $^ -o $@


.PHONY: clean
clean:
	rm -rf *.o *~ sim
