all: build/nxp_simtemp_cli

build:
	mkdir -p build

build/%.o: %.cpp | build
	g++ -std=c++17 -c $< -o $@

build/nxp_simtemp_cli: build/main.o build/nxp_simtemp_cli.o
	g++ $^ -o $@

.PHONY: all clean
clean:
	rm -rf build