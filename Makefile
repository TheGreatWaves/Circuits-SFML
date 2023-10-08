sim: main.cpp gate.cpp component_recipe.cpp pin.cpp gui/wire_gui.cpp gui/belt_gui.cpp gui/pin_port_gui.cpp -lsfml-graphics -lsfml-window -lsfml-system
	g++ -g $^ -o $@


.PHONY: clean
clean:
	rm -rf *.o *~ sim
