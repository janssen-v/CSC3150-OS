all: build runCluster

build:
	@echo "Building..."
	nvcc --relocatable-device-code=true main.cu user_program.cu virtual_memory.cu -o test
	@echo "Done."

run:
	@echo "Running..."
	./test
	@echo "Done."

runCluster:
	@echo "Running..."
	srun ./test
	@echo "Done."

provisionCluster:
	@echo "Provisioning 1 Thread, 1 Node Provisioned for 20 Minutes..."
	salloc -N1 -n1 -t20 -p Debug

releaseCluster:
	@echo "Releasing Resources..."
	scancel -u 119010518
	@echo "Done."

clean:
	@echo "Cleaning..."
	rm snapshot.bin
	rm test
	@echo "Done."