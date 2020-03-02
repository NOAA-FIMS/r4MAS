R?=R

PACKAGE=stockassessment
VERSION := $(shell sed -n '/^Version: /s///p' DESCRIPTION)
THISSHA := $(shell git log -1 --format="%H")
TARBALL := $(PACKAGE)_$(VERSION).tar.gz
ZIPFILE := =$(PACKAGE)_$(VERSION).zip

CPP_SRC := $(PACKAGE)/mas/*.hpp $(PACKAGE)/*.cpp $(PACKAGE)/*.hpp 
PKG_CPPFLAGS :=

.PHONY:$(SUBDIRS) all updateData qi quick-install vignette-update 

all:
	make install
	make pdf

doc-update: $(PACKAGE)/R/*.R
	echo "library(roxygen2);roxygenize(\"$(PACKAGE)\")" | $(R) --slave
	@touch doc-update

namespace-update :: $(PACKAGE)/NAMESPACE
$(PACKAGE)/NAMESPACE: $(PACKAGE)/R/*.R
	echo "library(roxygen2);roxygenize(\"$(PACKAGE)\")" | $(R) --slave

build-package: $(TARBALL)
$(TARBALL): $(PACKAGE)/NAMESPACE $(CPP_SRC) $(PACKAGE)/R/*.R
	sed s/dummySHA/$(THISSHA)/g description-addon > description-addon-tmp
	mv $(PACKAGE)/DESCRIPTION old-description
	cat old-description description-addon-tmp > $(PACKAGE)/DESCRIPTION  
	rm description-addon-tmp
	$(R) CMD build --resave-data=no $(PACKAGE)
	rm $(PACKAGE)/DESCRIPTION  
	mv old-description $(PACKAGE)/DESCRIPTION  

install: $(TARBALL)
	$(R) CMD INSTALL --preclean --html $<
	@touch $@

qi:
	cd $(PACKAGE)/src; echo "library(TMB); compile('R4MAS.cpp')" | $(R) --slave
	$(R) CMD INSTALL $(PACKAGE)

quick-install: $(PACKAGE)/src/stockassessment.so
	$(R) CMD INSTALL $(PACKAGE)

$(PACKAGE)/src/R4MAS.so: $(PACKAGE)/src/R4MAS.cpp $(CPP_SRC)
	touch $(PACKAGE)/src/R4MAS.cpp
	cd $(PACKAGE)/src; echo "=compile('R4MAS.cpp','-O0 -g')" | $(R) --slave

unexport TEXINPUTS
pdf: $(PACKAGE).pdf
$(PACKAGE).pdf: $(PACKAGE)/man/*.Rd
	rm -f $(PACKAGE).pdf
	$(R) CMD Rd2pdf --no-preview $(PACKAGE)

build:
	$(R) CMD build $(PACKAGE)

check: $(TARBALL)
	$(R) CMD check $(TARBALL)


clean:
	rm -f install doc-update $(TARBALL) $(PACKAGE).pdf $(PACKAGE)/src/*.so $(PACKAGE)/src/*.o
	rm -rf $(PACKAGE).Rcheck


NPROCS:=1
OS:=$(shell uname -s)

ifeq ($(OS),Linux)
  NPROCS:=$(shell grep -c ^processor /proc/cpuinfo)
endif

MAKEFLAGS += --silent