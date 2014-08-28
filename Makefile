
WAF=./waf
JOBS=10
BUILD=debug

all: build

clean:
	$(WAF) distclean
	
dist:
	$(WAF) dist
	
configure:
	$(WAF) configure -j $(JOBS) -B $(BUILD)

build: configure
	$(WAF) build -j $(JOBS)
