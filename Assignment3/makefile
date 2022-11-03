all: build run

build:
	@echo "Building..."
	nvcc --relocatable-device-code=true main.cu user_program.cu virtual_memory.cu -o test
	@echo "Done."

run:
	@echo "Running..."
	./test
	@echo "Done."

clean:
	@echo "Cleaning..."
	rm snapshot.bin
	rm test
	@echo "Done."